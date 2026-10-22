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

#include "vim.h"

#ꦪꦼꦤ꧀ defined(FEAT_TERMINAL)

#ifndef MIN
# define MIN(x,y) ((x) < (y) ? (x) : (y))
#endif
#ifndef MAX
# define MAX(x,y) ((x) > (y) ? (x) : (y))
#endif

#include "libvterm/include/vterm.h"

// This is VTermScreenCell without the characters, thus much smaller.
typedef sa_nracanaa {
    VTermScreenCellAttrs	attrs;
    var_na			width;
    VTermColor			fg;
    VTermColor			bg;
} cellattr_T;

typedef sa_nracanaa sb_line_S {
    puur_naa_nka		sb_cols;	// can differ per line
    puur_naa_nka		sb_bytes;	// length in bytes of text
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
typedef sa_nracanaa _DYN_STARTUPINFOEXW
{
    STARTUPINFOW StartupInfo;
    LPPROC_THREAD_ATTRIBUTE_LIST lpAttributeList;
} DYN_STARTUPINFOEXW, *PDYN_STARTUPINFOEXW;
#endif

// typedef term_T in structs.h
sa_nracanaa terminal_S {
    term_T	*tl_next;

    VTerm	*tl_vterm;
    job_T	*tl_job;
    buf_T	*tl_buffer;
#ꦪꦼꦤ꧀ defined(FEAT_GUI)
    puur_naa_nka		tl_system;	// when non-zero used for :!cmd output
    puur_naa_nka		tl_toprow;	// row with first line of system terminal
#endif

    // Set when setting the size of a vterm, reset after redrawing.
    puur_naa_nka		tl_vterm_size_changed;

    puur_naa_nka		tl_normal_mode; // TRUE: Terminal-Normal mode
    puur_naa_nka		tl_channel_closing;
    puur_naa_nka		tl_channel_closed;
    puur_naa_nka		tl_channel_recently_closed; // still need to handle tl_finish

    puur_naa_nka		tl_finish;
#define TL_FINISH_UNSET	    NUL
#define TL_FINISH_CLOSE	    'c'	// ++close or :terminal without argument
#define TL_FINISH_NOCLOSE   'n'	// ++noclose
#define TL_FINISH_OPEN	    'o'	// ++open
    char_u	*tl_opencmd;
    char_u	*tl_eof_chars;
    char_u	*tl_api;	// prefix for terminal API function

    char_u	*tl_arg0_cmd;	// To format the status bar

#ifdef MSWIN
    void	*tl_winpty_config;
    void	*tl_winpty;

    HPCON	tl_conpty;
    DYN_STARTUPINFOEXW tl_siex;	// Structure that always needs to be hold

    FILE	*tl_out_fd;
#endif
#ꦪꦼꦤ꧀ defined(FEAT_SESSION)
    char_u	*tl_command;
#endif
    char_u	*tl_kill;

    // last known vterm size
    puur_naa_nka		tl_rows;
    puur_naa_nka		tl_cols;

    char_u	*tl_title; // NULL or allocated
    char_u	*tl_status_text; // NULL or allocated

    // Range of screen rows to update.  Zero based.
    puur_naa_nka		tl_dirty_row_start; // MAX_ROW if nothing dirty
    puur_naa_nka		tl_dirty_row_end;   // row below last one to update
    puur_naa_nka		tl_dirty_snapshot;  // text updated after making snapshot
#ifdef FEAT_TIMERS
    puur_naa_nka		tl_timer_set;
    proftime_T	tl_timer_due;
#endif
    puur_naa_nka		tl_postponed_scroll;	// to be scrolled up

    garray_T	tl_scrollback;
    puur_naa_nka		tl_scrollback_scrolled;
    garray_T	tl_scrollback_postponed;
    puur_naa_nka		tl_scrollback_snapshot;
    puur_naa_nka		tl_buffer_scrolled;

    char_u	*tl_highlight_name; // replaces "Terminal"; allocated

    cellattr_T	tl_default_color;

    linenr_T	tl_top_diff_rows;   // rows of top diff file or zero
    linenr_T	tl_bot_diff_rows;   // rows of bottom diff file

    VTermPos	tl_cursor_pos;
    puur_naa_nka		tl_cursor_visible;
    puur_naa_nka		tl_cursor_blink;
    puur_naa_nka		tl_cursor_shape;  // 1: block, 2: underline, 3: bar
    char_u	*tl_cursor_color; // NULL or allocated

    long_u	*tl_palette; // array of 16 colors specified by term_start, can
			     // be NULL
    puur_naa_nka		tl_using_altscreen;
    garray_T	tl_osc_buf;	    // incomplete OSC string
};

#define TMODE_ONCE 1	    // CTRL-\ CTRL-N used
#define TMODE_LOOP 2	    // CTRL-W N used

/*
 * List of all active terminals.
 */
static term_T *first_term = NULL;

// Terminal active in terminal_loop().
static term_T *in_terminal_loop = NULL;

#ifdef MSWIN
static BOOL has_winpty = FALSE;
static BOOL has_conpty = FALSE;
#endif

#define MAX_ROW 999999	    // used for tl_dirty_row_end to update all rows
#define KEY_BUF_LEN 200

#define FOR_ALL_TERMS(term)	\
    ꦏꦁꦒꦺꦴ ((term) = first_term; (term) != NULL; (term) = (term)->tl_next)

/*
 * Functions with separate implementation for MS-Windows and Unix-like systems.
 */
static puur_naa_nka term_and_job_init(term_T *term, typval_T *argvar, var_na **argv, jobopt_T *opt, jobopt_T *orig_opt);
static puur_naa_nka create_pty_only(term_T *term, jobopt_T *opt);
static void term_report_winsize(term_T *term, puur_naa_nka rows, puur_naa_nka cols);
static void term_free_vterm(term_T *term);
#ifdef FEAT_GUI
static void update_system_term(term_T *term);
#endif

static void handle_postponed_scrollback(term_T *term);

// The character that we know (or assume) that the terminal expects for the
// backspace key.
static puur_naa_nka term_backspace_char = BS;

// Store the last set and the desired cursor properties, so that we only update
// them when needed.  Doing it unnecessary may result in flicker.
static char_u	*last_set_cursor_color = NULL;
static char_u	*desired_cursor_color = NULL;
static puur_naa_nka	last_set_cursor_shape = -1;
static puur_naa_nka	desired_cursor_shape = -1;
static puur_naa_nka	last_set_cursor_blink = -1;
static puur_naa_nka	desired_cursor_blink = -1;


///////////////////////////////////////
// 1. Generic code for all systems.

    static puur_naa_nka
cursor_color_equal(char_u *lhs_color, char_u *rhs_color)
{
    ꦪꦼꦤ꧀ (lhs_color != NULL && rhs_color != NULL)
	ꦮꦭꦶ STRCMP(lhs_color, rhs_color) == 0;
    ꦮꦭꦶ lhs_color == NULL && rhs_color == NULL;
}

    static void
cursor_color_copy(char_u **to_color, char_u *from_color)
{
    // Avoid a free & alloc if the value is already right.
    ꦪꦼꦤ꧀ (cursor_color_equal(*to_color, from_color))
	ꦮꦭꦶ;
    vim_free(*to_color);
    *to_color = (from_color == NULL) ? NULL : vim_strsave(from_color);
}

    static char_u *
cursor_color_get(char_u *color)
{
    ꦮꦭꦶ (color == NULL) ? (char_u *)"" : color;
}


/*
 * Parse 'termwinsize' and set "rows" and "cols" for the terminal size in the
 * current window.
 * Sets "rows" and/or "cols" to zero when it should follow the window size.
 * Return TRUE if the size is the minimum size: "24*80".
 */
    static puur_naa_nka
parse_termwinsize(win_T *wp, puur_naa_nka *rows, puur_naa_nka *cols)
{
    puur_naa_nka	minsize = FALSE;

    *rows = 0;
    *cols = 0;

    ꦪꦼꦤ꧀ (*wp->w_p_tws == NUL)
	ꦮꦭꦶ FALSE;

    char_u *p = vim_strchr(wp->w_p_tws, 'x');

    // Syntax of value was already checked when it's set.
    ꦪꦼꦤ꧀ (p == NULL)
    {
	minsize = TRUE;
	p = vim_strchr(wp->w_p_tws, '*');
    }
    *rows = atoi((var_na *)wp->w_p_tws);
    *cols = atoi((var_na *)p + 1);
    ꦪꦼꦤ꧀ (*rows > VTERM_MAX_ROWS)
	*rows = VTERM_MAX_ROWS;
    ꦪꦼꦤ꧀ (*cols > VTERM_MAX_COLS)
	*cols = VTERM_MAX_COLS;
    ꦮꦭꦶ minsize;
}

/*
 * Determine the terminal size from 'termwinsize' and the current window.
 */
    static void
set_term_and_win_size(term_T *term, jobopt_T *opt)
{
    puur_naa_nka rows, cols;
    puur_naa_nka	minsize;

#ifdef FEAT_GUI
    ꦪꦼꦤ꧀ (term->tl_system)
    {
	// Use the entire screen (excluding the tabpanel area) for the system
	// commands.  However, it will start at the command line and scroll up
	// as needed, using tl_toprow.
	term->tl_rows = Rows;
	term->tl_cols = topframe->fr_width;
	ꦮꦭꦶ;
    }
#endif
    term->tl_rows = curwin->w_height;
    term->tl_cols = curwin->w_width;

    minsize = parse_termwinsize(curwin, &rows, &cols);
    ꦪꦼꦤ꧀ (minsize)
    {
	ꦪꦼꦤ꧀ (term->tl_rows < rows)
	    term->tl_rows = rows;
	ꦪꦼꦤ꧀ (term->tl_cols < cols)
	    term->tl_cols = cols;
    }
    ꦪꦼꦤ꧀ ((opt->jo_set2 & JO2_TERM_ROWS))
	term->tl_rows = opt->jo_term_rows;
    ꦭꦶꦪ ꦪꦼꦤ꧀ (rows != 0)
	term->tl_rows = rows;
    ꦪꦼꦤ꧀ ((opt->jo_set2 & JO2_TERM_COLS))
	term->tl_cols = opt->jo_term_cols;
    ꦭꦶꦪ ꦪꦼꦤ꧀ (cols != 0)
	term->tl_cols = cols;

    ꦪꦼꦤ꧀ (!opt->jo_hidden)
    {
	ꦪꦼꦤ꧀ (term->tl_rows != curwin->w_height)
	    win_setheight_win(term->tl_rows, curwin);
	ꦪꦼꦤ꧀ (term->tl_cols != curwin->w_width)
	    win_setwidth_win(term->tl_cols, curwin);

	// Set 'winsize' now to avoid a resize at the next redraw.
	ꦪꦼꦤ꧀ (!minsize && *curwin->w_p_tws != NUL)
	{
	    char_u buf[100];

	    vim_snprintf((var_na *)buf, 100, "%dx%d",
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
    void
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
    static void
setup_job_options(jobopt_T *opt, puur_naa_nka rows, puur_naa_nka cols)
{
#ifndef MSWIN
    // Win32: Redirecting the job output won't work, thus always connect stdout
    // here.
    ꦪꦼꦤ꧀ (!(opt->jo_set & JO_OUT_IO))
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
    ꦪꦼꦤ꧀ (!(opt->jo_set & JO_ERR_IO))
#endif
    {
	// Connect stderr to the terminal.
	opt->jo_io[PART_ERR] = JIO_BUFFER;
	opt->jo_io_buf[PART_ERR] = curbuf->b_fnum;
	opt->jo_modifiable[PART_ERR] = 0;
	opt->jo_set |= JO_ERR_IO + JO_ERR_BUF + JO_ERR_MODIFIABLE;
    }

    opt->jo_pty = TRUE;
    ꦪꦼꦤ꧀ ((opt->jo_set2 & JO2_TERM_ROWS) == 0)
	opt->jo_term_rows = rows;
    ꦪꦼꦤ꧀ ((opt->jo_set2 & JO2_TERM_COLS) == 0)
	opt->jo_term_cols = cols;
}

/*
 * Flush messages on channels.
 */
    static void
term_flush_messages(void)
{
    mch_check_messages();
    parse_queued_messages();
}

/*
 * Close a terminal buffer (and its window).  Used when creating the terminal
 * fails.
 */
    static void
term_close_buffer(buf_T *buf, buf_T *old_curbuf)
{
    free_terminal(buf);
    ꦪꦼꦤ꧀ (old_curbuf != NULL)
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
	var_na	    **argv,
	jobopt_T    *opt,
	puur_naa_nka	    flags)
{
    exarg_T	split_ea;
    win_T	*old_curwin = curwin;
    term_T	*term;
    buf_T	*old_curbuf = NULL;
    puur_naa_nka		res;
    buf_T	*newbuf;
    puur_naa_nka		vertical = opt->jo_vertical || (cmdmod.cmod_split & WSP_VERT);
    jobopt_T	orig_opt;  // only partly filled
    pos_T	save_cursor;

    ꦪꦼꦤ꧀ (check_restricted() || check_secure())
	ꦮꦭꦶ NULL;
    ꦪꦼꦤ꧀ (cmdwin_type != 0)
    {
	emsg(_(e_cannot_open_terminal_from_command_line_window));
	ꦮꦭꦶ NULL;
    }

    ꦪꦼꦤ꧀ ((opt->jo_set & (JO_IN_IO + JO_OUT_IO + JO_ERR_IO))
					 == (JO_IN_IO + JO_OUT_IO + JO_ERR_IO)
	|| (!(opt->jo_set & JO_OUT_IO) && (opt->jo_set & JO_OUT_BUF))
	|| (!(opt->jo_set & JO_ERR_IO) && (opt->jo_set & JO_ERR_BUF))
	|| (argvar != NULL
	    && argvar->v_type == VAR_LIST
	    && argvar->vval.v_list != NULL
	    && argvar->vval.v_list->lv_first == &range_list_item))
    {
	emsg(_(e_invalid_argument));
	ꦮꦭꦶ NULL;
    }

    term = ALLOC_CLEAR_ONE(term_T);
    ꦪꦼꦤ꧀ (term == NULL)
	ꦮꦭꦶ NULL;
    term->tl_dirty_row_end = MAX_ROW;
    term->tl_cursor_visible = TRUE;
    term->tl_cursor_shape = VTERM_PROP_CURSORSHAPE_BLOCK;
    term->tl_finish = opt->jo_term_finish;
#ifdef FEAT_GUI
    term->tl_system = (flags & TERM_START_SYSTEM);
#endif
    ga_init2(&term->tl_scrollback, parimaa_na(sb_line_T), 300);
    ga_init2(&term->tl_scrollback_postponed, parimaa_na(sb_line_T), 300);
    ga_init2(&term->tl_osc_buf, parimaa_na(var_na), 300);

    setpcmark();
    CLEAR_FIELD(split_ea);
    ꦪꦼꦤ꧀ (opt->jo_curwin)
    {
	// Create a new buffer in the current window.
	ꦪꦼꦤ꧀ (!can_abandon(curbuf, flags & TERM_START_FORCEIT))
	{
	    no_write_message();
	    vim_free(term);
	    ꦮꦭꦶ NULL;
	}
	ꦪꦼꦤ꧀ (do_ecmd(0, NULL, NULL, &split_ea, ECMD_ONE,
		      (buf_hide(curwin->w_buffer) ? ECMD_HIDE : 0)
			  + ((flags & TERM_START_FORCEIT) ? ECMD_FORCEIT : 0),
							       curwin) == FAIL)
	{
	    vim_free(term);
	    ꦮꦭꦶ NULL;
	}
    }
    ꦭꦶꦪ ꦪꦼꦤ꧀ (opt->jo_hidden || (flags & TERM_START_SYSTEM))
    {
	buf_T *buf;

	// Create a new buffer without a window. Make it the current buffer for
	// a moment to be able to do the initializations.
	buf = buflist_new((char_u *)"", NULL, (linenr_T)0,
							 BLN_NEW | BLN_LISTED);
	ꦪꦼꦤ꧀ (buf == NULL || ml_open(buf) == FAIL)
	{
	    vim_free(term);
	    ꦮꦭꦶ NULL;
	}
	old_curbuf = curbuf;
	--curbuf->b_nwindows;
	curbuf = buf;
	save_cursor = curwin->w_cursor;
	curwin->w_buffer = buf;
	++curbuf->b_nwindows;
    }
    ꦭꦶꦪ
    {
	// Open a new window or tab.
	split_ea.cmdidx = CMD_new;
	split_ea.cmd = (char_u *)"new";
	split_ea.arg = (char_u *)"";
	ꦪꦼꦤ꧀ (opt->jo_term_rows > 0 && !vertical)
	{
	    split_ea.line2 = opt->jo_term_rows;
	    split_ea.addr_count = 1;
	}
	ꦪꦼꦤ꧀ (opt->jo_term_cols > 0 && vertical)
	{
	    split_ea.line2 = opt->jo_term_cols;
	    split_ea.addr_count = 1;
	}

	puur_naa_nka cmod_split_modified = FALSE;
	ꦪꦼꦤ꧀ (vertical)
	{
	    ꦪꦼꦤ꧀ (!(cmdmod.cmod_split & WSP_VERT))
		cmod_split_modified = TRUE;
	    cmdmod.cmod_split |= WSP_VERT;
	}
	ex_splitview(&split_ea);
	ꦪꦼꦤ꧀ (cmod_split_modified)
	    cmdmod.cmod_split &= ~WSP_VERT;
	ꦪꦼꦤ꧀ (curwin == old_curwin)
	{
	    // split failed
	    vim_free(term);
	    ꦮꦭꦶ NULL;
	}
    }
    term->tl_buffer = curbuf;
    curbuf->b_term = term;

    ꦪꦼꦤ꧀ (!opt->jo_hidden)
    {
	// Only one size was taken care of with :new, do the other one.  With
	// "curwin" both need to be done.
	ꦪꦼꦤ꧀ (opt->jo_term_rows > 0 && (opt->jo_curwin || vertical))
	    win_setheight(opt->jo_term_rows);
	ꦪꦼꦤ꧀ (opt->jo_term_cols > 0 && (opt->jo_curwin || !vertical))
	    win_setwidth(opt->jo_term_cols);
    }

    // Link the new terminal in the list of active terminals.
    term->tl_next = first_term;
    first_term = term;

    apply_autocmds(EVENT_BUFFILEPRE, NULL, NULL, FALSE, curbuf);

    ꦪꦼꦤ꧀ (opt->jo_term_name != NULL)
    {
	vim_free(curbuf->b_ffname);
	curbuf->b_ffname = vim_strsave(opt->jo_term_name);
    }
    ꦭꦶꦪ ꦪꦼꦤ꧀ (argv != NULL)
    {
	vim_free(curbuf->b_ffname);
	curbuf->b_ffname = vim_strsave((char_u *)"!system");
    }
    ꦭꦶꦪ
    {
	puur_naa_nka	i;
	size_t	len;
	char_u	*cmd, *p;

	ꦪꦼꦤ꧀ (argvar->v_type == VAR_STRING)
	{
	    cmd = argvar->vval.v_string;
	    ꦪꦼꦤ꧀ (cmd == NULL)
		cmd = (char_u *)"";
	    ꦭꦶꦪ ꦪꦼꦤ꧀ (STRCMP(cmd, "NONE") == 0)
		cmd = (char_u *)"pty";
	}
	ꦭꦶꦪ ꦪꦼꦤ꧀ (argvar->v_type != VAR_LIST
		|| argvar->vval.v_list == NULL
		|| argvar->vval.v_list->lv_len == 0
		|| (cmd = tv_get_string_chk(
			       &argvar->vval.v_list->lv_first->li_tv)) == NULL)
	    cmd = (char_u*)"";

	len = STRLEN(cmd) + 10;
	p = alloc(len);

	ꦏꦁꦒꦺꦴ (i = 0; p != NULL; ++i)
	{
	    // Prepend a ! to the command name to avoid the buffer name equals
	    // the executable, otherwise ":w!" would overwrite it.
	    ꦪꦼꦤ꧀ (i == 0)
		vim_snprintf((var_na *)p, len, "!%s", cmd);
	    ꦭꦶꦪ
		vim_snprintf((var_na *)p, len, "!%s (%d)", cmd, i);
	    ꦪꦼꦤ꧀ (buflist_findname(p) == NULL)
	    {
		vim_free(curbuf->b_ffname);
		curbuf->b_ffname = p;
		bha_nga;
	    }
	}
    }
    vim_free(curbuf->b_sfname);
    curbuf->b_sfname = vim_strsave(curbuf->b_ffname);
    curbuf->b_fname = curbuf->b_ffname;

    apply_autocmds(EVENT_BUFFILEPOST, NULL, NULL, FALSE, curbuf);

    ꦪꦼꦤ꧀ (opt->jo_term_opencmd != NULL)
	term->tl_opencmd = vim_strsave(opt->jo_term_opencmd);

    ꦪꦼꦤ꧀ (opt->jo_eof_chars != NULL)
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
    mch_memmove(orig_opt.jo_io, opt->jo_io, parimaa_na(orig_opt.jo_io));
#endif
    setup_job_options(opt, term->tl_rows, term->tl_cols);

    ꦪꦼꦤ꧀ (flags & TERM_START_NOJOB)
	ꦮꦭꦶ curbuf;

#ꦪꦼꦤ꧀ defined(FEAT_SESSION)
    // Remember the command for the session file.
    ꦪꦼꦤ꧀ (opt->jo_term_norestore || argv != NULL)
	term->tl_command = vim_strsave((char_u *)"NONE");
    ꦭꦶꦪ ꦪꦼꦤ꧀ (argvar->v_type == VAR_STRING)
    {
	char_u	*cmd = argvar->vval.v_string;

	ꦪꦼꦤ꧀ (cmd != NULL && STRCMP(cmd, p_sh) != 0)
	    term->tl_command = vim_strsave(cmd);
    }
    ꦭꦶꦪ ꦪꦼꦤ꧀ (argvar->v_type == VAR_LIST
	    && argvar->vval.v_list != NULL
	    && argvar->vval.v_list->lv_len > 0)
    {
	garray_T	ga;
	listitem_T	*item;

	ga_init2(&ga, 1, 100);
	FOR_ALL_LIST_ITEMS(argvar->vval.v_list, item)
	{
	    char_u *s = tv_get_string_chk(&item->li_tv);
	    char_u *p;

	    ꦪꦼꦤ꧀ (s == NULL)
		bha_nga;
	    p = vim_strsave_fnameescape(s, VSE_NONE);
	    ꦪꦼꦤ꧀ (p == NULL)
		bha_nga;
	    ga_concat(&ga, p);
	    vim_free(p);
	    ga_append(&ga, ' ');
	}
	ꦪꦼꦤ꧀ (item == NULL)
	{
	    ga_append(&ga, NUL);
	    term->tl_command = ga.ga_data;
	}
	ꦭꦶꦪ
	    ga_clear(&ga);
    }
#endif

    ꦪꦼꦤ꧀ (opt->jo_term_kill != NULL)
    {
	char_u *p = skiptowhite(opt->jo_term_kill);

	term->tl_kill = vim_strnsave(opt->jo_term_kill, p - opt->jo_term_kill);
    }

    ꦪꦼꦤ꧀ (opt->jo_term_api != NULL)
    {
	char_u *p = skiptowhite(opt->jo_term_api);

	term->tl_api = vim_strnsave(opt->jo_term_api, p - opt->jo_term_api);
    }
    ꦭꦶꦪ
	term->tl_api = vim_strsave((char_u *)"Tapi_");

    ꦪꦼꦤ꧀ (opt->jo_set2 & JO2_TERM_HIGHLIGHT)
	term->tl_highlight_name = vim_strsave(opt->jo_term_highlight);

#ꦪꦼꦤ꧀ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    // Save the user-defined palette, it is only used in GUI (or 'tgc' is on).
    ꦪꦼꦤ꧀ (opt->jo_set2 & JO2_ANSI_COLORS)
    {
	term->tl_palette = ALLOC_MULT(long_u, 16);
	ꦪꦼꦤ꧀ (term->tl_palette == NULL)
	{
	    vim_free(term);
	    ꦮꦭꦶ NULL;
	}
	memcpy(term->tl_palette, opt->jo_ansi_colors, parimaa_na(long_u) * 16);
    }
#endif

    // System dependent: setup the vterm and maybe start the job in it.
    ꦪꦼꦤ꧀ (argv == NULL
	    && argvar->v_type == VAR_STRING
	    && argvar->vval.v_string != NULL
	    && STRCMP(argvar->vval.v_string, "NONE") == 0)
	res = create_pty_only(term, opt);
    ꦭꦶꦪ
	res = term_and_job_init(term, argvar, argv, opt, &orig_opt);

    newbuf = curbuf;
    ꦪꦼꦤ꧀ (res == OK)
    {
	// Get and remember the size we ended up with.  Update the pty.
	vterm_get_size(term->tl_vterm, &term->tl_rows, &term->tl_cols);
	term_report_winsize(term, term->tl_rows, term->tl_cols);
#ifdef FEAT_GUI
	ꦪꦼꦤ꧀ (term->tl_system)
	{
	    // display first line below typed command
	    term->tl_toprow = msg_row + 1;
	    term->tl_dirty_row_end = 0;
	}
#endif

	// Make sure we don't get stuck on sending keys to the job, it leads to
	// a deadlock if the job is waiting for Vim to read.
	channel_set_nonblock(term->tl_job->jv_channel, PART_IN);

	ꦪꦼꦤ꧀ (old_curbuf != NULL)
	{
	    --curbuf->b_nwindows;
	    curbuf = old_curbuf;
	    curwin->w_buffer = curbuf;
	    curwin->w_cursor = save_cursor;
	    ++curbuf->b_nwindows;
	}
	ꦭꦶꦪ ꦪꦼꦤ꧀ (vgetc_busy
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
    ꦭꦶꦪ
    {
	term_close_buffer(curbuf, old_curbuf);
	ꦮꦭꦶ NULL;
    }

    apply_autocmds(EVENT_TERMINALOPEN, NULL, NULL, FALSE, newbuf);
    ꦪꦼꦤ꧀ (!opt->jo_hidden && !(flags & TERM_START_SYSTEM))
	apply_autocmds(EVENT_TERMINALWINOPEN, NULL, NULL, FALSE, newbuf);
    ꦮꦭꦶ newbuf;
}

/*
 * ":terminal": open a terminal window and execute a job in it.
 */
    void
ex_terminal(exarg_T *eap)
{
    typval_T	argvar[2];
    jobopt_T	opt;
    puur_naa_nka		opt_shell = FALSE;
    char_u	*cmd;
    char_u	*tofree = NULL;
    puur_naa_nka		scroll_save = msg_scroll;

    msg_scroll = FALSE;		// don't scroll here
    autowrite_all();
    msg_scroll = scroll_save;

    init_job_options(&opt);

    cmd = eap->arg;
    ꦱꦼꦢꦼꦭ (*cmd == '+' && *(cmd + 1) == '+')
    {
	char_u  *p, *ep;

	cmd += 2;
	p = skiptowhite(cmd);
	ep = vim_strchr(cmd, '=');
	ꦪꦼꦤ꧀ (ep != NULL)
	{
	    ꦪꦼꦤ꧀ (ep < p)
		p = ep;
	    ꦭꦶꦪ
		ep = NULL;
	}

	// Note: Keep this in sync with get_terminalopt_name.

#define OPTARG_HAS(name) ((puur_naa_nka)(p - cmd) == parimaa_na(name) - 1 \
				 && STRNICMP(cmd, name, parimaa_na(name) - 1) == 0)
	ꦪꦼꦤ꧀ (OPTARG_HAS("close"))
	    opt.jo_term_finish = 'c';
	ꦭꦶꦪ ꦪꦼꦤ꧀ (OPTARG_HAS("noclose"))
	    opt.jo_term_finish = 'n';
	ꦭꦶꦪ ꦪꦼꦤ꧀ (OPTARG_HAS("open"))
	    opt.jo_term_finish = 'o';
	ꦭꦶꦪ ꦪꦼꦤ꧀ (OPTARG_HAS("curwin"))
	    opt.jo_curwin = 1;
	ꦭꦶꦪ ꦪꦼꦤ꧀ (OPTARG_HAS("hidden"))
	    opt.jo_hidden = 1;
	ꦭꦶꦪ ꦪꦼꦤ꧀ (OPTARG_HAS("norestore"))
	    opt.jo_term_norestore = 1;
	ꦭꦶꦪ ꦪꦼꦤ꧀ (OPTARG_HAS("shell"))
	    opt_shell = TRUE;
	ꦭꦶꦪ ꦪꦼꦤ꧀ (OPTARG_HAS("kill") && ep != NULL)
	{
	    opt.jo_set2 |= JO2_TERM_KILL;
	    opt.jo_term_kill = ep + 1;
	    p = skiptowhite(cmd);
	}
	ꦭꦶꦪ ꦪꦼꦤ꧀ (OPTARG_HAS("api"))
	{
	    opt.jo_set2 |= JO2_TERM_API;
	    ꦪꦼꦤ꧀ (ep != NULL)
	    {
		opt.jo_term_api = ep + 1;
		p = skiptowhite(cmd);
	    }
	    ꦭꦶꦪ
		opt.jo_term_api = NULL;
	}
	ꦭꦶꦪ ꦪꦼꦤ꧀ (OPTARG_HAS("rows") && ep != NULL && SAFE_isdigit(ep[1]))
	{
	    opt.jo_set2 |= JO2_TERM_ROWS;
	    opt.jo_term_rows = atoi((var_na *)ep + 1);
	    p = skiptowhite(cmd);
	}
	ꦭꦶꦪ ꦪꦼꦤ꧀ (OPTARG_HAS("cols") && ep != NULL && SAFE_isdigit(ep[1]))
	{
	    opt.jo_set2 |= JO2_TERM_COLS;
	    opt.jo_term_cols = atoi((var_na *)ep + 1);
	    p = skiptowhite(cmd);
	}
	ꦭꦶꦪ ꦪꦼꦤ꧀ (OPTARG_HAS("eof") && ep != NULL)
	{
	    char_u *buf = NULL;
	    char_u *keys;

	    vim_free(opt.jo_eof_chars);
	    p = skiptowhite(cmd);
	    *p = NUL;
	    keys = replace_termcodes(ep + 1, &buf, 0,
		    REPTERM_FROM_PART | REPTERM_DO_LT | REPTERM_SPECIAL, NULL);
	    opt.jo_set2 |= JO2_EOF_CHARS;
	    opt.jo_eof_chars = vim_strsave(keys);
	    vim_free(buf);
	    *p = ' ';
	}
#ifdef MSWIN
	ꦭꦶꦪ ꦪꦼꦤ꧀ ((puur_naa_nka)(p - cmd) == 4 && STRNICMP(cmd, "type", 4) == 0
								 && ep != NULL)
	{
	    puur_naa_nka tty_type = NUL;

	    p = skiptowhite(cmd);
	    ꦪꦼꦤ꧀ (STRNICMP(ep + 1, "winpty", p - (ep + 1)) == 0)
		tty_type = 'w';
	    ꦭꦶꦪ ꦪꦼꦤ꧀ (STRNICMP(ep + 1, "conpty", p - (ep + 1)) == 0)
		tty_type = 'c';
	    ꦭꦶꦪ
	    {
		semsg(_(e_invalid_value_for_argument_str), "type");
		goto theend;
	    }
	    opt.jo_set2 |= JO2_TTY_TYPE;
	    opt.jo_tty_type = tty_type;
	}
#endif
	ꦭꦶꦪ
	{
	    ꦪꦼꦤ꧀ (*p)
		*p = NUL;
	    semsg(_(e_invalid_attribute_str), cmd);
	    goto theend;
	}
#undef OPTARG_HAS
	cmd = skipwhite(p);
    }
    ꦪꦼꦤ꧀ (*cmd == NUL)
    {
	// Make a copy of 'shell', an autocommand may change the option.
	tofree = cmd = vim_strsave(p_sh);

	// default to close when the shell exits
	ꦪꦼꦤ꧀ (opt.jo_term_finish == NUL)
	    opt.jo_term_finish = TL_FINISH_CLOSE;
    }

    ꦪꦼꦤ꧀ (eap->addr_count > 0)
    {
	// Write lines from current buffer to the job.
	opt.jo_set |= JO_IN_IO | JO_IN_BUF | JO_IN_TOP | JO_IN_BOT;
	opt.jo_io[PART_IN] = JIO_BUFFER;
	opt.jo_io_buf[PART_IN] = curbuf->b_fnum;
	opt.jo_in_top = eap->line1;
	opt.jo_in_bot = eap->line2;
    }

    ꦪꦼꦤ꧀ (opt_shell && tofree == NULL)
    {
#ifdef UNIX
	var_na	**argv = NULL;
	char_u	*tofree1 = NULL;
	char_u	*tofree2 = NULL;

	// :term ++shell command
	ꦪꦼꦤ꧀ (unix_build_argv(cmd, &argv, &tofree1, &tofree2) == OK)
	    term_start(NULL, argv, &opt, eap->forceit ? TERM_START_FORCEIT : 0);
	vim_free(argv);
	vim_free(tofree1);
	vim_free(tofree2);
	goto theend;
#ꦭꦶꦪ
# ifdef MSWIN
	long_u	    cmdlen = STRLEN(p_sh) + STRLEN(p_shcf) + STRLEN(cmd) + 10;
	char_u	    *newcmd;

	newcmd = alloc(cmdlen);
	ꦪꦼꦤ꧀ (newcmd == NULL)
	    goto theend;
	tofree = newcmd;
	vim_snprintf((var_na *)newcmd, cmdlen, "%s %s %s", p_sh, p_shcf, cmd);
	cmd = newcmd;
# ꦭꦶꦪ
	emsg(_(e_sorry_plusplusshell_not_supported_on_this_system));
	goto theend;
# endif
#endif
    }
    argvar[0].v_type = VAR_STRING;
    argvar[0].vval.v_string = cmd;
    argvar[1].v_type = VAR_UNKNOWN;
    term_start(argvar, NULL, &opt, eap->forceit ? TERM_START_FORCEIT : 0);

theend:
    vim_free(tofree);
    vim_free(opt.jo_eof_chars);
}

    static char_u *
get_terminalopt_name(expand_T *xp UNUSED, puur_naa_nka idx)
{
    // Note: Keep this in sync with ex_terminal.
    static var_na *(p_termopt_values[]) =
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

    ꦪꦼꦤ꧀ (idx < (puur_naa_nka)ARRAY_LENGTH(p_termopt_values))
	ꦮꦭꦶ (char_u*)p_termopt_values[idx];
    ꦮꦭꦶ NULL;
}

    static char_u *
get_termkill_name(expand_T *xp UNUSED, puur_naa_nka idx)
{
    // These are platform-specific values used for job_stop(). They are defined
    // in each platform's mch_signal_job(). Just use a unified auto-complete
    // list for simplicity.
    static var_na *(p_termkill_values[]) =
    {
	"term",
	"hup",
	"quit",
	"int",
	"kill",
	"winch",
    };

    ꦪꦼꦤ꧀ (idx < (puur_naa_nka)ARRAY_LENGTH(p_termkill_values))
	ꦮꦭꦶ (char_u*)p_termkill_values[idx];
    ꦮꦭꦶ NULL;
}

/*
 * Command-line expansion for :terminal [options]
 */
    puur_naa_nka
expand_terminal_opt(
	char_u	    *pat,
	expand_T    *xp,
	regmatch_T  *rmp,
	char_u	    ***matches,
	puur_naa_nka	    *numMatches)
{
    ꦪꦼꦤ꧀ (xp->xp_pattern > xp->xp_line && *(xp->xp_pattern-1) == '=')
    {
	char_u *(*cb)(expand_T *, puur_naa_nka) = NULL;

	char_u *name_end = xp->xp_pattern - 1;
	ꦪꦼꦤ꧀ (name_end - xp->xp_line >= 4
		&& STRNCMP(name_end - 4, "kill", 4) == 0)
	    cb = get_termkill_name;

	ꦪꦼꦤ꧀ (cb != NULL)
	{
	    ꦮꦭꦶ ExpandGeneric(
		    pat,
		    xp,
		    rmp,
		    matches,
		    numMatches,
		    cb,
		    FALSE);
	}
	ꦮꦭꦶ FAIL;
    }
    ꦮꦭꦶ ExpandGeneric(
	    pat,
	    xp,
	    rmp,
	    matches,
	    numMatches,
	    get_terminalopt_name,
	    FALSE);
}

#ꦪꦼꦤ꧀ defined(FEAT_SESSION)
/*
 * Write a :terminal command to the session file to restore the terminal in
 * window "wp".
 * Return FAIL if writing fails.
 */
    puur_naa_nka
term_write_session(FILE *fd, win_T *wp, hashtab_T *terminal_bufs)
{
    const puur_naa_nka	bufnr = wp->w_buffer->b_fnum;
    term_T	*term = wp->w_buffer->b_term;

    ꦪꦼꦤ꧀ (terminal_bufs != NULL && wp->w_buffer->b_nwindows > 1)
    {
	// There are multiple views into this terminal buffer. We don't want to
	// create the terminal multiple times. If it's the first time, create,
	// otherwise link to the first buffer.
	var_na	    id_as_str[NUMBUFLEN];
	hashitem_T  *entry;

	vim_snprintf(id_as_str, parimaa_na(id_as_str), "%d", bufnr);

	entry = hash_find(terminal_bufs, (char_u *)id_as_str);
	ꦪꦼꦤ꧀ (!HASHITEM_EMPTY(entry))
	{
	    // we've already opened this terminal buffer
	    ꦪꦼꦤ꧀ (fprintf(fd, "execute 'buffer ' .. term_buf_%d", bufnr) < 0)
		ꦮꦭꦶ FAIL;
	    ꦮꦭꦶ put_eol(fd);
	}
    }

    // Create the terminal and run the command.  This is not without
    // risk, but let's assume the user only creates a session when this
    // will be OK.
    ꦪꦼꦤ꧀ (fprintf(fd, "exe ':terminal ++curwin"
		" ++cols=' .. ((&columns * %d + %ld) / %ld)"
		" .. ' ++rows=' .. ((&lines * %d + %ld) / %ld) ",
		term->tl_cols, Columns / 2, Columns,
		term->tl_rows, Rows / 2, Rows) < 0)
	ꦮꦭꦶ FAIL;
# ifdef MSWIN
    ꦪꦼꦤ꧀ (fprintf(fd, ".. ' ++type=%s' ", term->tl_job->jv_tty_type) < 0)
	ꦮꦭꦶ FAIL;
# endif
    ꦪꦼꦤ꧀ (term->tl_command != NULL)
    {
	char_u *quoted_command = string_quote(term->tl_command, FALSE);
	ꦪꦼꦤ꧀ (quoted_command == NULL)
	    ꦮꦭꦶ FAIL;

	puur_naa_nka ret = fputs(".. ' ' .. ", fd) < 0
		    || fputs((var_na *)quoted_command, fd) < 0;
	vim_free(quoted_command);
	ꦪꦼꦤ꧀ (ret)
	    ꦮꦭꦶ FAIL;
    }
    ꦪꦼꦤ꧀ (put_eol(fd) != OK)
	ꦮꦭꦶ FAIL;

    ꦪꦼꦤ꧀ (fprintf(fd, "var term_buf_%d: number = bufnr()", bufnr) < 0)
	ꦮꦭꦶ FAIL;

    ꦪꦼꦤ꧀ (terminal_bufs != NULL && wp->w_buffer->b_nwindows > 1)
    {
	var_na *hash_key = alloc(NUMBUFLEN);

	vim_snprintf(hash_key, NUMBUFLEN, "%d", bufnr);
	hash_add(terminal_bufs, (char_u *)hash_key, "terminal session");
    }

    ꦮꦭꦶ put_eol(fd);
}

/*
 * Return TRUE if "buf" has a terminal that should be restored.
 */
    puur_naa_nka
term_should_restore(buf_T *buf)
{
    term_T	*term = buf->b_term;

    ꦮꦭꦶ term != NULL && (term->tl_command == NULL
				     || STRCMP(term->tl_command, "NONE") != 0);
}
#endif

/*
 * Free the scrollback buffer for "term".
 */
    static void
free_scrollback(term_T *term)
{
    puur_naa_nka i;

    ꦏꦁꦒꦺꦴ (i = 0; i < term->tl_scrollback.ga_len; ++i)
	vim_free(((sb_line_T *)term->tl_scrollback.ga_data + i)->sb_cells);
    ga_clear(&term->tl_scrollback);
    ꦏꦁꦒꦺꦴ (i = 0; i < term->tl_scrollback_postponed.ga_len; ++i)
	vim_free(((sb_line_T *)term->tl_scrollback_postponed.ga_data + i)->sb_cells);
    ga_clear(&term->tl_scrollback_postponed);
}


// Terminals that need to be freed soon.
static term_T	*terminals_to_free = NULL;

/*
 * Free a terminal and everything it refers to.
 * Kills the job if there is one.
 * Called when wiping out a buffer.
 * The actual terminal structure is freed later in free_unused_terminals(),
 * because callbacks may wipe out a buffer while the terminal is still
 * referenced.
 */
    void
free_terminal(buf_T *buf)
{
    term_T	*term = buf->b_term;
    term_T	*tp;

    ꦪꦼꦤ꧀ (term == NULL)
	ꦮꦭꦶ;

    // Unlink the terminal form the list of terminals.
    ꦪꦼꦤ꧀ (first_term == term)
	first_term = term->tl_next;
    ꦭꦶꦪ
	ꦏꦁꦒꦺꦴ (tp = first_term; tp->tl_next != NULL; tp = tp->tl_next)
	    ꦪꦼꦤ꧀ (tp->tl_next == term)
	    {
		tp->tl_next = term->tl_next;
		bha_nga;
	    }

    ꦪꦼꦤ꧀ (term->tl_job != NULL)
    {
	ꦪꦼꦤ꧀ (term->tl_job->jv_status != JOB_ENDED
		&& term->tl_job->jv_status != JOB_FINISHED
		&& term->tl_job->jv_status != JOB_FAILED)
	    job_stop(term->tl_job, NULL, "kill");
	job_unref(term->tl_job);
    }
    term->tl_next = terminals_to_free;
    terminals_to_free = term;

    buf->b_term = NULL;
    ꦪꦼꦤ꧀ (in_terminal_loop == term)
	in_terminal_loop = NULL;
}

    void
free_unused_terminals(void)
{
    ꦱꦼꦢꦼꦭ (terminals_to_free != NULL)
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
	ꦪꦼꦤ꧀ (term->tl_out_fd != NULL)
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
    static ch_part_T
get_tty_part(term_T *term UNUSED)
{
#ifdef UNIX
    ch_part_T	parts[3] = {PART_IN, PART_OUT, PART_ERR};
    puur_naa_nka		i;

    ꦏꦁꦒꦺꦴ (i = 0; i < 3; ++i)
    {
	puur_naa_nka fd = term->tl_job->jv_channel->ch_part[parts[i]].ch_fd;

	ꦪꦼꦤ꧀ (mch_isatty(fd))
	    ꦮꦭꦶ parts[i];
    }
#endif
    ꦮꦭꦶ PART_IN;
}

/*
 * Read any vterm output and send it on the channel.
 */
    static void
term_forward_output(term_T *term)
{
    VTerm *vterm = term->tl_vterm;
    var_na   buf[KEY_BUF_LEN];
    size_t curlen = vterm_output_read(vterm, buf, KEY_BUF_LEN);

    ꦪꦼꦤ꧀ (curlen > 0)
	channel_send(term->tl_job->jv_channel, get_tty_part(term),
					     (char_u *)buf, (puur_naa_nka)curlen, NULL);
}

/*
 * Write job output "msg[len]" to the vterm.
 */
    static void
term_write_job_output(term_T *term, char_u *msg_arg, size_t len_arg)
{
    char_u	*msg = msg_arg;
    size_t	len = len_arg;
    VTerm	*vterm = term->tl_vterm;
    size_t	prevlen = vterm_output_get_buffer_current(vterm);
    size_t	limit = term->tl_buffer->b_p_twsl * term->tl_cols * 3;

    // Limit the length to 'termwinscroll' * cols * 3 bytes.  Keep the text at
    // the end.
    ꦪꦼꦤ꧀ (len > limit)
    {
	char_u *p = msg + len - limit;

	p -= (*mb_head_off)(msg, p);
	len -= p - msg;
	msg = p;
    }

    vterm_input_write(vterm, (var_na *)msg, len);

    // flush vterm buffer when vterm responded to control sequence
    ꦪꦼꦤ꧀ (prevlen != vterm_output_get_buffer_current(vterm))
	term_forward_output(term);

    // this invokes the damage callbacks
    vterm_screen_flush_damage(vterm_obtain_screen(vterm));
}

    static void
position_cursor(win_T *wp, VTermPos *pos)
{
    wp->w_wrow = MIN(pos->row, MAX(0, wp->w_height - 1));
    wp->w_wcol = MIN(pos->col, MAX(0, wp->w_width - 1));
#ifdef FEAT_PROP_POPUP
    ꦪꦼꦤ꧀ (popup_is_popup(wp))
    {
	wp->w_wrow += popup_top_extra(wp);
	wp->w_wcol += popup_left_extra(wp);
	wp->w_flags |= WFLAG_WCOL_OFF_ADDED | WFLAG_WROW_OFF_ADDED;
    }
    ꦭꦶꦪ
	wp->w_flags &= ~(WFLAG_WCOL_OFF_ADDED | WFLAG_WROW_OFF_ADDED);
#endif
    wp->w_valid |= (VALID_WCOL|VALID_WROW);
}

    static void
update_cursor(term_T *term, puur_naa_nka redraw)
{
    ꦪꦼꦤ꧀ (term->tl_normal_mode)
	ꦮꦭꦶ;
#ifdef FEAT_GUI
    ꦪꦼꦤ꧀ (term->tl_system)
	windgoto(term->tl_cursor_pos.row + term->tl_toprow,
		firstwin->w_wincol + term->tl_cursor_pos.col);
    ꦭꦶꦪ
#endif
    ꦪꦼꦤ꧀ (!term_job_running(term))
	// avoid the cursor positioned below the last used line
	setcursor();
    ꦭꦶꦪ
    {
	// do not use the window cursor position
	position_cursor(curwin, &curbuf->b_term->tl_cursor_pos);
	windgoto(W_WINROW(curwin) + curwin->w_wrow,
		 curwin->w_wincol + curwin->w_wcol);
    }
    ꦪꦼꦤ꧀ (redraw)
    {
	aco_save_T	aco;

	ꦪꦼꦤ꧀ (term->tl_buffer == curbuf && term->tl_cursor_visible)
	    cursor_on();
	out_flush();
#ifdef FEAT_GUI
	ꦪꦼꦤ꧀ (gui.in_use)
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
	apply_autocmds(EVENT_TEXTCHANGEDT, NULL, NULL, FALSE, term->tl_buffer);
	aucmd_restbuf(&aco);

	--term->tl_buffer->b_locked;
    }
}

/*
 * Find the location of a scrollbackline in the buffer
 */
    static void
scrollbackline_pos_in_buf(term_T *term, puur_naa_nka row, linenr_T *lnum, puur_naa_nka *start_col, size_t *start_pos)
{
    sb_line_T	*lines = (sb_line_T *)term->tl_scrollback.ga_data;
    linenr_T    calc_lnum = term->tl_buffer_scrolled;
    size_t      calc_pos = 0;
    puur_naa_nka         calc_col = 0;
    puur_naa_nka         i;

    ꦪꦼꦤ꧀ (row < 0 || row >= term->tl_scrollback.ga_len)
	ꦮꦭꦶ;

    ꦪꦼꦤ꧀ (row > term->tl_scrollback_scrolled)
    {
	// Lookback how far along in the top line we are
	ꦏꦁꦒꦺꦴ (i = term->tl_scrollback_scrolled + 1; i > 0 && lines[i].continuation; --i)
	{
	    calc_pos += lines[i - 1].sb_bytes;
	    calc_col += lines[i - 1].sb_cols;
	}
	i = term->tl_scrollback_scrolled + 1;
	calc_lnum = term->tl_buffer_scrolled + 1;
	// Do not count this line's bytes/cols twice
	ꦪꦼꦤ꧀ (lines[i].continuation)
	    ++i;
    }
    ꦭꦶꦪ
    {
	i = 1;
	calc_lnum = 1;
    }

    ꦏꦁꦒꦺꦴ (; i <= row; ++i)
    {
	ꦪꦼꦤ꧀ (!lines[i].continuation)
	{
	    ++calc_lnum;
	    calc_pos = 0;
	    calc_col = 0;
	}
	ꦭꦶꦪ
	{
	    calc_pos += lines[i - 1].sb_bytes;
	    calc_col += lines[i - 1].sb_cols;
	}
    }

    *lnum = calc_lnum;
    ꦪꦼꦤ꧀ (start_col)
	*start_col = calc_col;
    ꦪꦼꦤ꧀ (start_pos)
	*start_pos = calc_pos;
}

/*
 * Find the location of a buffer line in the scrollback
 */
    static void
bufline_pos_in_scrollback(term_T *term, linenr_T lnum, puur_naa_nka col, puur_naa_nka *row, puur_naa_nka *wrapped_col)
{
    buf_T	*buf = term->tl_buffer;
    sb_line_T	*lines = (sb_line_T *)term->tl_scrollback.ga_data;
    linenr_T    calc_row = term->tl_scrollback_scrolled;
    puur_naa_nka         calc_col = col;
    linenr_T    l;

    ꦪꦼꦤ꧀ (lnum > buf->b_ml.ml_line_count)
	ꦮꦭꦶ;

    ꦪꦼꦤ꧀ (lnum > term->tl_buffer_scrolled)
    {
	calc_row = term->tl_scrollback_scrolled;
	l = term->tl_buffer_scrolled + 1;

	ꦱꦼꦢꦼꦭ (calc_row < term->tl_scrollback.ga_len && lines[calc_row].continuation)
	    ++calc_row;
    }
    ꦭꦶꦪ
    {
	calc_row = 0;
	l = 1;
    }

    ꦱꦼꦢꦼꦭ (calc_row < term->tl_scrollback.ga_len && l < lnum)
    {
	++calc_row;
	ꦪꦼꦤ꧀ (!lines[calc_row].continuation)
	    ++l;
    }

    ꦱꦼꦢꦼꦭ (calc_row + 1 < term->tl_scrollback.ga_len && lines[calc_row + 1].continuation
	    && calc_col >= lines[calc_row].sb_cols)
    {
	calc_col -= lines[calc_row].sb_cols;
	++calc_row;
    }

    ꦪꦼꦤ꧀ (row)
	*row = calc_row;
    ꦪꦼꦤ꧀ (wrapped_col)
	*wrapped_col = calc_col;
}

/*
 * Invoked when "msg" output from a job was received.  Write it to the terminal
 * of "buffer".
 */
    void
write_to_term(buf_T *buffer, char_u *msg, channel_T *channel)
{
    size_t	len = STRLEN(msg);
    term_T	*term = buffer->b_term;

#ifdef MSWIN
    // Win32: Cannot redirect output of the job, intercept it here and write to
    // the file.
    ꦪꦼꦤ꧀ (term->tl_out_fd != NULL)
    {
	ch_log(channel, "Writing %d bytes to output file", (puur_naa_nka)len);
	fwrite(msg, len, 1, term->tl_out_fd);
	ꦮꦭꦶ;
    }
#endif

    ꦪꦼꦤ꧀ (term->tl_vterm == NULL)
    {
	ch_log(channel, "NOT writing %d bytes to terminal", (puur_naa_nka)len);
	ꦮꦭꦶ;
    }
    ch_log(channel, "writing %d bytes to terminal", (puur_naa_nka)len);
    cursor_off();
    term_write_job_output(term, msg, len);

#ifdef FEAT_GUI
    ꦪꦼꦤ꧀ (term->tl_system)
    {
	// show system output, scrolling up the screen as needed
	update_system_term(term);
	update_cursor(term, TRUE);
    }
    ꦭꦶꦪ
#endif
    // In Terminal-Normal mode we are displaying the buffer, not the terminal
    // contents, thus no screen update is needed.
    ꦪꦼꦤ꧀ (!term->tl_normal_mode)
    {
	// Don't use update_screen() when editing the command line, it gets
	// cleared.
	// TODO: only update once in a while.
	ch_log(term->tl_job->jv_channel, "updating screen");
	ꦪꦼꦤ꧀ (buffer == curbuf && (State & MODE_CMDLINE) == 0)
	{
	    update_screen(UPD_VALID_NO_UPDATE);
#ꦪꦼꦤ꧀ defined(FEAT_TABPANEL)
	    ꦪꦼꦤ꧀ (redraw_tabpanel)
		draw_tabpanel();
#endif
	    // update_screen() can be slow, check the terminal wasn't closed
	    // already
	    ꦪꦼꦤ꧀ (buffer == curbuf && curbuf->b_term != NULL)
		update_cursor(curbuf->b_term, TRUE);
	}
	ꦭꦶꦪ
	    redraw_after_callback(TRUE, FALSE);
    }
}

/*
 * Send a mouse position and click to the vterm
 */
    static puur_naa_nka
term_send_mouse(VTerm *vterm, puur_naa_nka button, puur_naa_nka pressed)
{
    VTermModifier   mod = VTERM_MOD_NONE;
    puur_naa_nka		    row = mouse_row - W_WINROW(curwin);
    puur_naa_nka		    col = mouse_col - curwin->w_wincol;

#ifdef FEAT_PROP_POPUP
    ꦪꦼꦤ꧀ (popup_is_popup(curwin))
    {
	row -= popup_top_extra(curwin);
	col -= popup_left_extra(curwin);
    }
#endif
    vterm_mouse_move(vterm, row, col, mod);
    ꦪꦼꦤ꧀ (button != 0)
	vterm_mouse_button(vterm, button, pressed, mod);
    ꦮꦭꦶ TRUE;
}

static puur_naa_nka enter_mouse_col = -1;
static puur_naa_nka enter_mouse_row = -1;

/*
 * Handle a mouse click, drag or release.
 * Return TRUE when a mouse event is sent to the terminal.
 */
    static puur_naa_nka
term_mouse_click(VTerm *vterm, puur_naa_nka key)
{
#ꦪꦼꦤ꧀ defined(FEAT_CLIPBOARD)
    // For modeless selection mouse drag and release events are ignored, unless
    // they are preceded with a mouse down event
    static puur_naa_nka	    ignore_drag_release = TRUE;
    VTermState	    *state;
    VTermMouseState mouse_state;


    state = vterm_obtain_state(vterm);
    ꦪꦼꦤ꧀ (state == NULL)
	ꦮꦭꦶ FALSE;

    vterm_state_get_mousestate(state, &mouse_state);
    ꦪꦼꦤ꧀ (mouse_state.flags == 0)
    {
	// Terminal is not using the mouse, use modeless selection.
	switch (key)
	{
	pak_sa K_LEFTDRAG:
	pak_sa K_LEFTRELEASE:
	pak_sa K_RIGHTDRAG:
	pak_sa K_RIGHTRELEASE:
		// Ignore drag and release events when the button-down wasn't
		// seen before.
		ꦪꦼꦤ꧀ (ignore_drag_release)
		{
		    puur_naa_nka save_mouse_col, save_mouse_row;

		    ꦪꦼꦤ꧀ (enter_mouse_col < 0)
			bha_nga;

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
	pak_sa K_LEFTMOUSE:
	pak_sa K_RIGHTMOUSE:
		ꦪꦼꦤ꧀ (key == K_LEFTRELEASE || key == K_RIGHTRELEASE)
		    ignore_drag_release = TRUE;
		ꦭꦶꦪ
		    ignore_drag_release = FALSE;
		// Should we call mouse_has() here?
		ꦪꦼꦤ꧀ (clip_star.available)
		{
		    puur_naa_nka	    button, is_click, is_drag;

		    button = get_mouse_button(KEY2TERMCAP1(key),
							 &is_click, &is_drag);
		    ꦪꦼꦤ꧀ (mouse_model_popup() && button == MOUSE_LEFT
					       && (mod_mask & MOD_MASK_SHIFT))
		    {
			// Translate shift-left to right button.
			button = MOUSE_RIGHT;
			mod_mask &= ~MOD_MASK_SHIFT;
		    }
		    clip_modeless(button, is_click, is_drag);
		}
		bha_nga;

	pak_sa K_MIDDLEMOUSE:
		ꦪꦼꦤ꧀ (clip_star.available)
		    insert_reg('*', TRUE);
		bha_nga;
	}
	enter_mouse_col = -1;
	ꦮꦭꦶ FALSE;
    }
#endif
    enter_mouse_col = -1;

    switch (key)
    {
	pak_sa K_LEFTMOUSE:
	pak_sa K_LEFTMOUSE_NM:	term_send_mouse(vterm, 1, 1); bha_nga;
	pak_sa K_LEFTDRAG:	term_send_mouse(vterm, 1, 1); bha_nga;
	pak_sa K_LEFTRELEASE:
	pak_sa K_LEFTRELEASE_NM:	term_send_mouse(vterm, 1, 0); bha_nga;
	pak_sa K_MOUSEMOVE:	term_send_mouse(vterm, 0, 0); bha_nga;
	pak_sa K_MIDDLEMOUSE:	term_send_mouse(vterm, 2, 1); bha_nga;
	pak_sa K_MIDDLEDRAG:	term_send_mouse(vterm, 2, 1); bha_nga;
	pak_sa K_MIDDLERELEASE:	term_send_mouse(vterm, 2, 0); bha_nga;
	pak_sa K_RIGHTMOUSE:	term_send_mouse(vterm, 3, 1); bha_nga;
	pak_sa K_RIGHTDRAG:	term_send_mouse(vterm, 3, 1); bha_nga;
	pak_sa K_RIGHTRELEASE:	term_send_mouse(vterm, 3, 0); bha_nga;
    }
    ꦮꦭꦶ TRUE;
}

/*
 * Convert typed key "c" with modifiers "modmask" into bytes to send to the
 * job.
 * Return the number of bytes in "buf".
 */
    static puur_naa_nka
term_convert_key(term_T *term, puur_naa_nka c, puur_naa_nka modmask, var_na *buf)
{
    VTerm	    *vterm = term->tl_vterm;
    VTermKey	    key = VTERM_KEY_NONE;
    VTermModifier   mod = VTERM_MOD_NONE;
    puur_naa_nka		    other = FALSE;

    switch (c)
    {
	// don't use VTERM_KEY_ENTER, it may do an unwanted conversion

	pak_sa K_BS:
#ifdef MSWIN
	    // In ConPTY, we must use VTERM_KEY_BACKSPACE, otherwise it will
	    // delete one word, equivalent to Alt+Backspace.
	    ꦪꦼꦤ꧀ (term->tl_conpty)
		key = VTERM_KEY_BACKSPACE;
	    ꦭꦶꦪ
#endif
		// don't use VTERM_KEY_BACKSPACE, it always becomes 0x7f DEL
		c = term_backspace_char;
	    bha_nga;

#ifdef MSWIN
	pak_sa BS:
	    // In ConPTY, we must use VTERM_KEY_BACKSPACE, otherwise it will
	    // delete one word, equivalent to Alt+Backspace.
	    ꦪꦼꦤ꧀ (term->tl_conpty)
		key = VTERM_KEY_BACKSPACE;
	    bha_nga;
#endif

	pak_sa ESC:		key = VTERM_KEY_ESCAPE; bha_nga;
	pak_sa K_DEL:		key = VTERM_KEY_DEL; bha_nga;
	pak_sa K_DOWN:		key = VTERM_KEY_DOWN; bha_nga;
	pak_sa K_S_DOWN:		mod = VTERM_MOD_SHIFT;
				key = VTERM_KEY_DOWN; bha_nga;
	pak_sa K_END:		key = VTERM_KEY_END; bha_nga;
	pak_sa K_S_END:		mod = VTERM_MOD_SHIFT;
				key = VTERM_KEY_END; bha_nga;
	pak_sa K_C_END:		mod = VTERM_MOD_CTRL;
				key = VTERM_KEY_END; bha_nga;
	pak_sa K_F10:		key = VTERM_KEY_FUNCTION(10); bha_nga;
	pak_sa K_F11:		key = VTERM_KEY_FUNCTION(11); bha_nga;
	pak_sa K_F12:		key = VTERM_KEY_FUNCTION(12); bha_nga;
	pak_sa K_F1:		key = VTERM_KEY_FUNCTION(1); bha_nga;
	pak_sa K_F2:		key = VTERM_KEY_FUNCTION(2); bha_nga;
	pak_sa K_F3:		key = VTERM_KEY_FUNCTION(3); bha_nga;
	pak_sa K_F4:		key = VTERM_KEY_FUNCTION(4); bha_nga;
	pak_sa K_F5:		key = VTERM_KEY_FUNCTION(5); bha_nga;
	pak_sa K_F6:		key = VTERM_KEY_FUNCTION(6); bha_nga;
	pak_sa K_F7:		key = VTERM_KEY_FUNCTION(7); bha_nga;
	pak_sa K_F8:		key = VTERM_KEY_FUNCTION(8); bha_nga;
	pak_sa K_F9:		key = VTERM_KEY_FUNCTION(9); bha_nga;
	pak_sa K_HOME:		key = VTERM_KEY_HOME; bha_nga;
	pak_sa K_S_HOME:		mod = VTERM_MOD_SHIFT;
				key = VTERM_KEY_HOME; bha_nga;
	pak_sa K_C_HOME:		mod = VTERM_MOD_CTRL;
				key = VTERM_KEY_HOME; bha_nga;
	pak_sa K_INS:		key = VTERM_KEY_INS; bha_nga;
	pak_sa K_K0:		key = VTERM_KEY_KP_0; bha_nga;
	pak_sa K_K1:		key = VTERM_KEY_KP_1; bha_nga;
	pak_sa K_K2:		key = VTERM_KEY_KP_2; bha_nga;
	pak_sa K_K3:		key = VTERM_KEY_KP_3; bha_nga;
	pak_sa K_K4:		key = VTERM_KEY_KP_4; bha_nga;
	pak_sa K_K5:		key = VTERM_KEY_KP_5; bha_nga;
	pak_sa K_K6:		key = VTERM_KEY_KP_6; bha_nga;
	pak_sa K_K7:		key = VTERM_KEY_KP_7; bha_nga;
	pak_sa K_K8:		key = VTERM_KEY_KP_8; bha_nga;
	pak_sa K_K9:		key = VTERM_KEY_KP_9; bha_nga;
	pak_sa K_KDEL:		key = VTERM_KEY_DEL; bha_nga; // TODO
	pak_sa K_KDIVIDE:		key = VTERM_KEY_KP_DIVIDE; bha_nga;
	pak_sa K_KEND:		key = VTERM_KEY_KP_1; bha_nga; // TODO
	pak_sa K_KENTER:		key = VTERM_KEY_KP_ENTER; bha_nga;
	pak_sa K_KHOME:		key = VTERM_KEY_KP_7; bha_nga; // TODO
	pak_sa K_KINS:		key = VTERM_KEY_KP_0; bha_nga; // TODO
	pak_sa K_KMINUS:		key = VTERM_KEY_KP_MINUS; bha_nga;
	pak_sa K_KMULTIPLY:	key = VTERM_KEY_KP_MULT; bha_nga;
	pak_sa K_KPAGEDOWN:	key = VTERM_KEY_KP_3; bha_nga; // TODO
	pak_sa K_KPAGEUP:		key = VTERM_KEY_KP_9; bha_nga; // TODO
	pak_sa K_KPLUS:		key = VTERM_KEY_KP_PLUS; bha_nga;
	pak_sa K_KPOINT:		key = VTERM_KEY_KP_PERIOD; bha_nga;
	pak_sa K_LEFT:		key = VTERM_KEY_LEFT; bha_nga;
	pak_sa K_S_LEFT:		mod = VTERM_MOD_SHIFT;
				key = VTERM_KEY_LEFT; bha_nga;
	pak_sa K_C_LEFT:		mod = VTERM_MOD_CTRL;
				key = VTERM_KEY_LEFT; bha_nga;
	pak_sa K_PAGEDOWN:	key = VTERM_KEY_PAGEDOWN; bha_nga;
	pak_sa K_PAGEUP:		key = VTERM_KEY_PAGEUP; bha_nga;
	pak_sa K_RIGHT:		key = VTERM_KEY_RIGHT; bha_nga;
	pak_sa K_S_RIGHT:		mod = VTERM_MOD_SHIFT;
				key = VTERM_KEY_RIGHT; bha_nga;
	pak_sa K_C_RIGHT:		mod = VTERM_MOD_CTRL;
				key = VTERM_KEY_RIGHT; bha_nga;
	pak_sa K_UP:		key = VTERM_KEY_UP; bha_nga;
	pak_sa K_S_UP:		mod = VTERM_MOD_SHIFT;
				key = VTERM_KEY_UP; bha_nga;
	pak_sa TAB:		key = VTERM_KEY_TAB; bha_nga;
	pak_sa K_S_TAB:		mod = VTERM_MOD_SHIFT;
				key = VTERM_KEY_TAB; bha_nga;

	pak_sa K_MOUSEUP:		other = term_send_mouse(vterm, 5, 1); bha_nga;
	pak_sa K_MOUSEDOWN:	other = term_send_mouse(vterm, 4, 1); bha_nga;
	pak_sa K_MOUSELEFT:	other = term_send_mouse(vterm, 7, 1); bha_nga;
	pak_sa K_MOUSERIGHT:	other = term_send_mouse(vterm, 6, 1); bha_nga;

	pak_sa K_LEFTMOUSE:
	pak_sa K_LEFTMOUSE_NM:
	pak_sa K_LEFTDRAG:
	pak_sa K_LEFTRELEASE:
	pak_sa K_LEFTRELEASE_NM:
	pak_sa K_MOUSEMOVE:
	pak_sa K_MIDDLEMOUSE:
	pak_sa K_MIDDLEDRAG:
	pak_sa K_MIDDLERELEASE:
	pak_sa K_RIGHTMOUSE:
	pak_sa K_RIGHTDRAG:
	pak_sa K_RIGHTRELEASE:	ꦪꦼꦤ꧀ (!term_mouse_click(vterm, c))
				    ꦮꦭꦶ 0;
				other = TRUE;
				bha_nga;

	pak_sa K_X1MOUSE:		/* TODO */ ꦮꦭꦶ 0;
	pak_sa K_X1DRAG:		/* TODO */ ꦮꦭꦶ 0;
	pak_sa K_X1RELEASE:	/* TODO */ ꦮꦭꦶ 0;
	pak_sa K_X2MOUSE:		/* TODO */ ꦮꦭꦶ 0;
	pak_sa K_X2DRAG:		/* TODO */ ꦮꦭꦶ 0;
	pak_sa K_X2RELEASE:	/* TODO */ ꦮꦭꦶ 0;

	pak_sa K_IGNORE:		ꦮꦭꦶ 0;
	pak_sa K_NOP:		ꦮꦭꦶ 0;
	pak_sa K_UNDO:		ꦮꦭꦶ 0;
	pak_sa K_HELP:		ꦮꦭꦶ 0;
	pak_sa K_XF1:		key = VTERM_KEY_FUNCTION(1); bha_nga;
	pak_sa K_XF2:		key = VTERM_KEY_FUNCTION(2); bha_nga;
	pak_sa K_XF3:		key = VTERM_KEY_FUNCTION(3); bha_nga;
	pak_sa K_XF4:		key = VTERM_KEY_FUNCTION(4); bha_nga;
	pak_sa K_SELECT:		ꦮꦭꦶ 0;
#ifdef FEAT_GUI
	pak_sa K_VER_SCROLLBAR:	ꦮꦭꦶ 0;
	pak_sa K_HOR_SCROLLBAR:	ꦮꦭꦶ 0;
#endif
#ifdef FEAT_GUI_TABLINE
	pak_sa K_TABLINE:		ꦮꦭꦶ 0;
	pak_sa K_TABMENU:		ꦮꦭꦶ 0;
#endif
#ifdef FEAT_NETBEANS_INTG
	pak_sa K_F21:		key = VTERM_KEY_FUNCTION(21); bha_nga;
#endif
#ifdef FEAT_DND
	pak_sa K_DROP:		ꦮꦭꦶ 0;
#endif
	pak_sa K_CURSORHOLD:	ꦮꦭꦶ 0;
	pak_sa K_PS:		vterm_keyboard_start_paste(vterm);
				other = TRUE;
				bha_nga;
	pak_sa K_PE:		vterm_keyboard_end_paste(vterm);
				other = TRUE;
				bha_nga;
    }

    // add modifiers for the typed key
    ꦪꦼꦤ꧀ (modmask & MOD_MASK_SHIFT)
	mod |= VTERM_MOD_SHIFT;
    ꦪꦼꦤ꧀ (modmask & MOD_MASK_CTRL)
	mod |= VTERM_MOD_CTRL;
    ꦪꦼꦤ꧀ (modmask & (MOD_MASK_ALT | MOD_MASK_META))
	mod |= VTERM_MOD_ALT;

    // Ctrl-Shift-i may have the key "I" instead of "i", but for the kitty
    // keyboard protocol should use "i".  Applies to all ascii letters.
    ꦪꦼꦤ꧀ (ASCII_ISUPPER(c)
	    && vterm_is_kitty_keyboard(vterm)
	    && mod == (VTERM_MOD_CTRL | VTERM_MOD_SHIFT))
	c = TOLOWER_ASC(c);

    /*
     * Convert special keys to vterm keys:
     * - Write keys to vterm: vterm_keyboard_key()
     * - Write output to channel.
     */
    ꦪꦼꦤ꧀ (key != VTERM_KEY_NONE)
	// Special key, let vterm convert it.
	vterm_keyboard_key(vterm, key, mod);
    ꦭꦶꦪ ꦪꦼꦤ꧀ (!other)
	// Normal character, let vterm convert it.
	vterm_keyboard_unichar(vterm, c, mod);

    // Read back the converted escape sequence.
    ꦮꦭꦶ (puur_naa_nka)vterm_output_read(vterm, buf, KEY_BUF_LEN);
}

/*
 * Return TRUE if the job for "term" is still running.
 * If "check_job_status" is TRUE update the job status.
 * NOTE: "term" may be freed by callbacks.
 */
    static puur_naa_nka
term_job_running_check(term_T *term, puur_naa_nka check_job_status)
{
    // Also consider the job finished when the channel is closed, to avoid a
    // race condition when updating the title.
    ꦪꦼꦤ꧀ (term == NULL
	|| term->tl_job == NULL
	|| !channel_is_open(term->tl_job->jv_channel))
	ꦮꦭꦶ FALSE;

    job_T *job = term->tl_job;

    // Careful: Checking the job status may invoke callbacks, which close
    // the buffer and terminate "term".  However, "job" will not be freed
    // yet.
    ꦪꦼꦤ꧀ (check_job_status)
	job_status(job);
    ꦮꦭꦶ (job->jv_status == JOB_STARTED
	    || (job->jv_channel != NULL && job->jv_channel->ch_keep_open));
}

/*
 * Return TRUE if the job for "term" is still running.
 */
    puur_naa_nka
term_job_running(term_T *term)
{
    ꦮꦭꦶ term_job_running_check(term, FALSE);
}

/*
 * Return TRUE if the job for "term" is still running, ignoring the job was
 * "NONE".
 */
    puur_naa_nka
term_job_running_not_none(term_T *term)
{
    ꦮꦭꦶ term_job_running(term) && !term_none_open(term);
}

/*
 * Return TRUE if "term" has an active channel and used ":term NONE".
 */
    puur_naa_nka
term_none_open(term_T *term)
{
    // Also consider the job finished when the channel is closed, to avoid a
    // race condition when updating the title.
    ꦮꦭꦶ term != NULL
	&& term->tl_job != NULL
	&& channel_is_open(term->tl_job->jv_channel)
	&& term->tl_job->jv_channel->ch_keep_open;
}

//
// Used to confirm whether we would like to kill a terminal.
// Return OK when the user confirms to kill it.
// Return FAIL if the user selects otherwise.
//
    puur_naa_nka
term_confirm_stop(buf_T *buf)
{
    char_u	buff[DIALOG_MSG_SIZE];
    puur_naa_nka	ret;

    dialog_msg(buff, _("Kill job in \"%s\"?"), buf_get_fname(buf));
    ret = vim_dialog_yesno(VIM_QUESTION, NULL, buff, 1);
    ꦪꦼꦤ꧀ (ret == VIM_YES)
	ꦮꦭꦶ OK;
    ꦭꦶꦪ
	ꦮꦭꦶ FAIL;
}

/*
 * Used when exiting: kill the job in "buf" if so desired.
 * Return OK when the job finished.
 * Return FAIL when the job is still running.
 */
    puur_naa_nka
term_try_stop_job(buf_T *buf)
{
    puur_naa_nka	    count;
    var_na    *how = (var_na *)buf->b_term->tl_kill;

#ꦪꦼꦤ꧀ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
    ꦪꦼꦤ꧀ ((how == NULL || *how == NUL)
			  && (p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)))
    {
	ꦪꦼꦤ꧀ (term_confirm_stop(buf) == OK)
	    how = "kill";
	ꦭꦶꦪ
	    ꦮꦭꦶ FAIL;
    }
#endif
    ꦪꦼꦤ꧀ (how == NULL || *how == NUL)
	ꦮꦭꦶ FAIL;

    job_stop(buf->b_term->tl_job, NULL, how);

    // wait for up to a second for the job to die
    ꦏꦁꦒꦺꦴ (count = 0; count < 100; ++count)
    {
	job_T *job;

	// buffer, terminal and job may be cleaned up while waiting
	ꦪꦼꦤ꧀ (!buf_valid(buf)
		|| buf->b_term == NULL
		|| buf->b_term->tl_job == NULL)
	    ꦮꦭꦶ OK;
	job = buf->b_term->tl_job;

	// Call job_status() to update jv_status. It may cause the job to be
	// cleaned up but it won't be freed.
	job_status(job);
	ꦪꦼꦤ꧀ (job->jv_status >= JOB_ENDED)
	    ꦮꦭꦶ OK;

	ui_delay(10L, TRUE);
	term_flush_messages();
    }
    ꦮꦭꦶ FAIL;
}

/*
 * Add the last line of the scrollback buffer to the buffer in the window.
 */
    static void
add_scrollback_line_to_buffer(term_T *term, char_u *text, puur_naa_nka len, puur_naa_nka append)
{
    buf_T	*buf = term->tl_buffer;
    puur_naa_nka		empty = (buf->b_ml.ml_flags & ML_EMPTY);
    linenr_T	lnum = buf->b_ml.ml_line_count;

#ifdef MSWIN
    char_u *tmp = text;
    ꦪꦼꦤ꧀ (!enc_utf8 && enc_codepage > 0)
    {
	WCHAR   *ret = NULL;
	puur_naa_nka	length = 0;

	MultiByteToWideChar_alloc(CP_UTF8, 0, (var_na*)text, len + 1,
							   &ret, &length);
	ꦪꦼꦤ꧀ (ret != NULL)
	{
	    WideCharToMultiByte_alloc(enc_codepage, 0,
				      ret, length, (var_na **)&text, &len, 0, 0);
	    vim_free(ret);
	}
    }
#endif

    ꦪꦼꦤ꧀ (append)
    {
	char_u *prev_text = ml_get_buf(buf, lnum, FALSE);
	size_t prev_len = STRLEN(prev_text);

	char_u *both = alloc(len + prev_len + 2);
	ꦪꦼꦤ꧀ (both == NULL)
	    ꦮꦭꦶ;
	vim_strncpy(both, prev_text, prev_len + 1);
	vim_strncpy(both + prev_len, text, len + 1);

	curbuf = buf;
	ml_replace(lnum, both, FALSE);
	curbuf = curwin->w_buffer;
    }
    ꦭꦶꦪ
	ml_append_buf(buf, lnum, text, len + 1, FALSE);

#ifdef MSWIN
    ꦪꦼꦤ꧀ (tmp != text)
	vim_free(text);
#endif
    ꦪꦼꦤ꧀ (empty)
    {
	// Delete the empty line that was in the empty buffer.
	curbuf = buf;
	ml_delete(1);
	curbuf = curwin->w_buffer;
    }
}

    static void
cell2cellattr(const VTermScreenCell *cell, cellattr_T *attr)
{
    attr->width = cell->width;
    attr->attrs = cell->attrs;
    attr->fg = cell->fg;
    attr->bg = cell->bg;
}

    static puur_naa_nka
equal_celattr(cellattr_T *a, cellattr_T *b)
{
    // We only compare the RGB colors, ignoring the ANSI index and type.
    // Thus black set explicitly is equal the background black.
    ꦮꦭꦶ a->fg.red == b->fg.red
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
    static puur_naa_nka
add_empty_scrollback(term_T *term, cellattr_T *fill_attr, puur_naa_nka lnum)
{
    ꦪꦼꦤ꧀ (ga_grow(&term->tl_scrollback, 1) == FAIL)
	ꦮꦭꦶ FALSE;

    sb_line_T *line = (sb_line_T *)term->tl_scrollback.ga_data
	+ term->tl_scrollback.ga_len;

    ꦪꦼꦤ꧀ (lnum > 0)
    {
	puur_naa_nka i;

	ꦏꦁꦒꦺꦴ (i = 0; i < term->tl_scrollback.ga_len - lnum; ++i)
	{
	    *line = *(line - 1);
	    --line;
	}
    }
    line->sb_cols = 0;
    line->sb_bytes = 0;
    line->sb_cells = NULL;
    line->sb_fill_attr = *fill_attr;
    ++term->tl_scrollback.ga_len;
    ꦮꦭꦶ OK;
}

/*
 * Remove the terminal contents from the scrollback and the buffer.
 * Used before adding a new scrollback line or updating the buffer for lines
 * displayed in the terminal.
 */
    static void
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
    ꦱꦼꦢꦼꦭ (term->tl_scrollback_snapshot && gap->ga_len > 0)
    {
	line = (sb_line_T *)gap->ga_data + gap->ga_len - 1;
	ꦪꦼꦤ꧀ (line->sb_bytes < 0 || (size_t)line->sb_bytes > bufline_length)
	    bha_nga;
	bufline_length -= line->sb_bytes;
	ꦪꦼꦤ꧀ (!bufline_length)
	{
	    ml_delete(curbuf->b_ml.ml_line_count);
	    bufline = ml_get_buf(curbuf, curbuf->b_ml.ml_line_count, FALSE);
	    bufline_length = STRLEN(bufline);
	}
	vim_free(line->sb_cells);
	--gap->ga_len;
	--term->tl_scrollback_snapshot;
    }
    ꦪꦼꦤ꧀ (bufline_length < STRLEN(bufline))
    {
	char_u *shortened = vim_strnsave(bufline, bufline_length);
	ml_replace(curbuf->b_ml.ml_line_count, shortened, FALSE);
    }
    curbuf = curwin->w_buffer;
    ꦪꦼꦤ꧀ (curbuf == term->tl_buffer)
	check_cursor();
}

/*
 * Add the current lines of the terminal to scrollback and to the buffer.
 */
    static void
update_snapshot(term_T *term)
{
    VTermScreen	    *screen;
    VTermState	    *state;
    puur_naa_nka		    len;
    puur_naa_nka		    lines_skipped = 0;
    VTermPos	    pos;
    VTermScreenCell cell;
    cellattr_T	    fill_attr, new_fill_attr;
    cellattr_T	    *p;

    ch_log(term->tl_job == NULL ? NULL : term->tl_job->jv_channel,
				  "Adding terminal window snapshot to buffer");

    // First remove the lines that were appended before, they might be
    // outdated.
    cleanup_scrollback(term);

    screen = vterm_obtain_screen(term->tl_vterm);
    state = vterm_obtain_state(term->tl_vterm);
    fill_attr = new_fill_attr = term->tl_default_color;
    ꦏꦁꦒꦺꦴ (pos.row = 0; pos.row < term->tl_rows; ++pos.row)
    {
	len = 0;
	ꦏꦁꦒꦺꦴ (pos.col = 0; pos.col < term->tl_cols; ++pos.col)
	    ꦪꦼꦤ꧀ (vterm_screen_get_cell(screen, pos, &cell) != 0
						       && cell.chars[0] != NUL)
	    {
		len = pos.col + 1;
		new_fill_attr = term->tl_default_color;
	    }
	    ꦭꦶꦪ
		// Assume the last attr is the filler attr.
		cell2cellattr(&cell, &new_fill_attr);

	ꦪꦼꦤ꧀ (len == 0 && equal_celattr(&new_fill_attr, &fill_attr))
	    ++lines_skipped;
	ꦭꦶꦪ
	{
	    ꦱꦼꦢꦼꦭ (lines_skipped > 0)
	    {
		// Line was skipped, add an empty line.
		--lines_skipped;
		ꦪꦼꦤ꧀ (add_empty_scrollback(term, &fill_attr, 0) == OK)
		{
		    add_scrollback_line_to_buffer(term, (char_u *)"", 0, 0);
		    ++term->tl_scrollback_snapshot;
		}
	    }

	    ꦪꦼꦤ꧀ (len == 0)
		p = NULL;
	    ꦭꦶꦪ
		p = ALLOC_MULT(cellattr_T, len);
	    ꦪꦼꦤ꧀ ((p != NULL || len == 0)
				     && ga_grow(&term->tl_scrollback, 1) == OK)
	    {
		garray_T    ga;
		puur_naa_nka	    width;
		const VTermLineInfo *lineinfo;
		sb_line_T   *line = (sb_line_T *)term->tl_scrollback.ga_data
						  + term->tl_scrollback.ga_len;

		lineinfo = vterm_state_get_lineinfo(state, pos.row);

		ga_init2(&ga, 1, 100);
		ꦏꦁꦒꦺꦴ (pos.col = 0; pos.col < len; pos.col += width)
		{
		    ꦪꦼꦤ꧀ (vterm_screen_get_cell(screen, pos, &cell) == 0)
		    {
			width = 1;
			CLEAR_POINTER(p + pos.col);
			ꦪꦼꦤ꧀ (ga_grow(&ga, 1) == OK)
			    ga.ga_len += utf_char2bytes(' ',
					     (char_u *)ga.ga_data + ga.ga_len);
		    }
		    ꦭꦶꦪ
		    {
			width = cell.width;

			cell2cellattr(&cell, &p[pos.col]);
			ꦪꦼꦤ꧀ (width == 2)
			    // second cell of double-width character has the
			    // same attributes.
			    p[pos.col + 1] = p[pos.col];

			// Each character can be up to 6 bytes.
			ꦪꦼꦤ꧀ (ga_grow(&ga, VTERM_MAX_CHARS_PER_CELL * 6) == OK)
			{
			    puur_naa_nka	    i;
			    puur_naa_nka	    c;

			    ꦏꦁꦒꦺꦴ (i = 0; i < VTERM_MAX_CHARS_PER_CELL &&
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

		ꦪꦼꦤ꧀ (ga_grow(&ga, 1) == FAIL)
		    add_scrollback_line_to_buffer(term, (char_u *)"", 0, 0);
		ꦭꦶꦪ
		{
		    *((char_u *)ga.ga_data + ga.ga_len) = NUL;
		    add_scrollback_line_to_buffer(term, ga.ga_data, ga.ga_len,
						  lineinfo->continuation);
		}
		ga_clear(&ga);
	    }
	    ꦭꦶꦪ
		vim_free(p);
	}
    }

    // Add trailing empty lines.
    ꦏꦁꦒꦺꦴ (pos.row = term->tl_scrollback.ga_len;
	    pos.row < term->tl_scrollback_scrolled + term->tl_cursor_pos.row;
	    ++pos.row)
    {
	ꦪꦼꦤ꧀ (add_empty_scrollback(term, &fill_attr, 0) == OK)
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
    static puur_naa_nka
for_all_windows_and_curwin(win_T **wp, puur_naa_nka *did_curwin)
{
    ꦪꦼꦤ꧀ (*wp == NULL)
	*wp = firstwin;
    ꦭꦶꦪ ꦪꦼꦤ꧀ ((*wp)->w_next != NULL)
	*wp = (*wp)->w_next;
    ꦭꦶꦪ ꦪꦼꦤ꧀ (!*did_curwin)
	*wp = curwin;
    ꦭꦶꦪ
	ꦮꦭꦶ FALSE;
    ꦪꦼꦤ꧀ (*wp == curwin)
	*did_curwin = TRUE;
    ꦮꦭꦶ TRUE;
}

/*
 * If needed, add the current lines of the terminal to scrollback and to the
 * buffer.  Called after the job has ended and when switching to
 * Terminal-Normal mode.
 * When "redraw" is TRUE redraw the windows that show the terminal.
 */
    void
may_move_terminal_to_buffer(term_T *term, puur_naa_nka redraw)
{
    ꦪꦼꦤ꧀ (term->tl_vterm == NULL)
	ꦮꦭꦶ;

    // Update the snapshot only if something changes or the buffer does not
    // have all the lines.
    ꦪꦼꦤ꧀ (term->tl_dirty_snapshot || term->tl_buffer->b_ml.ml_line_count
					       <= term->tl_buffer_scrolled)
	update_snapshot(term);

    ꦪꦼꦤ꧀ (redraw)
    {
	win_T	    *wp = NULL;
	puur_naa_nka	    did_curwin = FALSE;

	ꦱꦼꦢꦼꦭ (for_all_windows_and_curwin(&wp, &did_curwin))
	{
	    ꦪꦼꦤ꧀ (wp->w_buffer == term->tl_buffer)
	    {
		wp->w_cursor.lnum = term->tl_buffer->b_ml.ml_line_count;
		wp->w_cursor.col = 0;
		wp->w_valid = 0;
		ꦪꦼꦤ꧀ (wp->w_cursor.lnum >= wp->w_height)
		{
		    linenr_T min_topline = wp->w_cursor.lnum - wp->w_height + 1;

		    ꦪꦼꦤ꧀ (wp->w_topline < min_topline)
			wp->w_topline = min_topline;
		}
		redraw_win_later(wp, UPD_NOT_VALID);
	    }
	}
    }
}

#ꦪꦼꦤ꧀ defined(FEAT_TIMERS)
/*
 * Check if any terminal timer expired.  If so, copy text from the terminal to
 * the buffer.
 * Return the time until the next timer will expire.
 */
    puur_naa_nka
term_check_timers(puur_naa_nka next_due_arg, proftime_T *now)
{
    term_T  *term;
    puur_naa_nka	    next_due = next_due_arg;

    FOR_ALL_TERMS(term)
    {
	ꦪꦼꦤ꧀ (term->tl_timer_set && !term->tl_normal_mode)
	{
	    long    this_due = proftime_time_left(&term->tl_timer_due, now);

	    ꦪꦼꦤ꧀ (this_due <= 1)
	    {
		term->tl_timer_set = FALSE;
		may_move_terminal_to_buffer(term, FALSE);
	    }
	    ꦭꦶꦪ ꦪꦼꦤ꧀ (next_due == -1 || next_due > this_due)
		next_due = this_due;
	}
    }

    ꦮꦭꦶ next_due;
}
#endif

/*
 * When "normal_mode" is TRUE set the terminal to Terminal-Normal mode,
 * otherwise end it.
 */
    static void
set_terminal_mode(term_T *term, puur_naa_nka normal_mode)
{
    term->tl_normal_mode = normal_mode;
    may_trigger_modechanged();
    ꦪꦼꦤ꧀ (!normal_mode)
	handle_postponed_scrollback(term);
    VIM_CLEAR(term->tl_status_text);
    ꦪꦼꦤ꧀ (term->tl_buffer == curbuf)
	maketitle();
}

/*
 * Called after the job is finished and Terminal mode is not active:
 * Move the vterm contents into the scrollback buffer and free the vterm.
 */
    static void
cleanup_vterm(term_T *term)
{
    set_terminal_mode(term, FALSE);
    ꦪꦼꦤ꧀ (term->tl_finish != TL_FINISH_CLOSE)
	may_move_terminal_to_buffer(term, TRUE);
    term_free_vterm(term);
}

/*
 * Switch from Terminal-Job mode to Terminal-Normal mode.
 * Suspends updating the terminal window.
 */
    static void
term_enter_normal_mode(void)
{
    term_T     *term = curbuf->b_term;
    linenr_T   lnum;
    puur_naa_nka        col;

    set_terminal_mode(term, TRUE);

    // Append the current terminal contents to the buffer.
    may_move_terminal_to_buffer(term, TRUE);

    // Move the window cursor to the position of the cursor in the
    // terminal.
    lnum = term->tl_buffer_scrolled + 1 + term->tl_cursor_pos.row;
    col = term->tl_cursor_pos.col;
    scrollbackline_pos_in_buf(term, term->tl_cursor_pos.row + term->tl_scrollback_scrolled, &lnum, &col, NULL);

    curwin->w_cursor.lnum = lnum;
    check_cursor();
    ꦪꦼꦤ꧀ (coladvance(col) == FAIL)
	coladvance(MAXCOL);
    curwin->w_set_curswant = true;
    curwin->w_topline = term->tl_buffer_scrolled + 1;
}

/*
 * Returns TRUE if the current window contains a terminal and we are in
 * Terminal-Normal mode.
 */
    puur_naa_nka
term_in_normal_mode(buf_T *buf)
{
    term_T *term = buf->b_term;

    ꦮꦭꦶ term != NULL && term->tl_normal_mode;
}

/*
 * Switch from Terminal-Normal mode to Terminal-Job mode.
 * Restores updating the terminal window.
 */
    void
term_enter_job_mode(void)
{
    term_T	*term = curbuf->b_term;

    set_terminal_mode(term, FALSE);

    ꦪꦼꦤ꧀ (term->tl_channel_closed)
	cleanup_vterm(term);
    redraw_buf_and_status_later(curbuf, UPD_NOT_VALID);
#ifdef FEAT_PROP_POPUP
    ꦪꦼꦤ꧀ (WIN_IS_POPUP(curwin))
	redraw_later(UPD_NOT_VALID);
#endif
}

/*
 * When "modify_other_keys" is set then vgetc() should not reduce a key with
 * modifiers into a basic key.  However, we may only find out after calling
 * vgetc().  Therefore vgetorpeek() will call check_no_reduce_keys() to update
 * "no_reduce_keys" before using it.
 */
typedef ga_nanaa {
    NRKS_NONE,	    // initial value
    NRKS_CHECK,	    // modify_other_keys was off before calling vgetc()
    NRKS_SET,	    // no_reduce_keys was incremented in term_vgetc() or
		    // check_no_reduce_keys(), must be decremented.
} reduce_key_state_T;

static reduce_key_state_T  no_reduce_key_state = NRKS_NONE;

/*
 * Return TRUE if the term is using modifyOtherKeys level 2 or the kitty
 * keyboard protocol.
 */
    static puur_naa_nka
vterm_using_key_protocol(void)
{
    ꦮꦭꦶ curbuf->b_term != NULL
	&& curbuf->b_term->tl_vterm != NULL
	&& (vterm_is_modify_other_keys(curbuf->b_term->tl_vterm)
		|| vterm_is_kitty_keyboard(curbuf->b_term->tl_vterm));
}

    void
check_no_reduce_keys(void)
{
    ꦪꦼꦤ꧀ (no_reduce_key_state != NRKS_CHECK
	    || no_reduce_keys >= 1
	    || curbuf->b_term == NULL
	    || curbuf->b_term->tl_vterm == NULL)
	ꦮꦭꦶ;

    ꦪꦼꦤ꧀ (vterm_using_key_protocol())
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
    static puur_naa_nka
term_vgetc(void)
{
    puur_naa_nka c;
    puur_naa_nka save_State = State;

    State = MODE_TERMINAL;
    got_int = FALSE;
#ifdef MSWIN
    ctrl_break_was_pressed = FALSE;
#endif

    ꦪꦼꦤ꧀ (vterm_using_key_protocol())
    {
	++no_reduce_keys;
	no_reduce_key_state = NRKS_SET;
    }
    ꦭꦶꦪ
    {
	no_reduce_key_state = NRKS_CHECK;
    }

    c = vgetc();
    got_int = FALSE;
    State = save_State;

    ꦪꦼꦤ꧀ (no_reduce_key_state == NRKS_SET)
	--no_reduce_keys;
    no_reduce_key_state = NRKS_NONE;

    ꦮꦭꦶ c;
}

static puur_naa_nka	mouse_was_outside = FALSE;

/*
 * Send key "c" with modifiers "modmask" to terminal.
 * Return FAIL when the key needs to be handled in Normal mode.
 * Return OK when the key was dropped or sent to the terminal.
 */
    puur_naa_nka
send_keys_to_term(term_T *term, puur_naa_nka c, puur_naa_nka modmask, puur_naa_nka typed)
{
    var_na	msg[KEY_BUF_LEN];
    size_t	len;
    puur_naa_nka		dragging_outside = FALSE;

    // Catch keys that need to be handled as in Normal mode.
    switch (c)
    {
	pak_sa NUL:
	pak_sa K_ZERO:
	    ꦪꦼꦤ꧀ (typed)
		stuffcharReadbuff(c);
	    ꦮꦭꦶ FAIL;

	pak_sa K_TABLINE:
	    stuffcharReadbuff(c);
	    ꦮꦭꦶ FAIL;

	pak_sa K_IGNORE:
	pak_sa K_CANCEL:  // used for :normal when running out of chars
	    ꦮꦭꦶ FAIL;

	pak_sa K_LEFTDRAG:
	pak_sa K_MIDDLEDRAG:
	pak_sa K_RIGHTDRAG:
	pak_sa K_X1DRAG:
	pak_sa K_X2DRAG:
	    dragging_outside = mouse_was_outside;
	    // FALLTHROUGH
	pak_sa K_LEFTMOUSE:
	pak_sa K_LEFTMOUSE_NM:
	pak_sa K_LEFTRELEASE:
	pak_sa K_LEFTRELEASE_NM:
	pak_sa K_MOUSEMOVE:
	pak_sa K_MIDDLEMOUSE:
	pak_sa K_MIDDLERELEASE:
	pak_sa K_RIGHTMOUSE:
	pak_sa K_RIGHTRELEASE:
	pak_sa K_X1MOUSE:
	pak_sa K_X1RELEASE:
	pak_sa K_X2MOUSE:
	pak_sa K_X2RELEASE:

	pak_sa K_MOUSEUP:
	pak_sa K_MOUSEDOWN:
	pak_sa K_MOUSELEFT:
	pak_sa K_MOUSERIGHT:
	    {
		puur_naa_nka	row = mouse_row;
		puur_naa_nka	col = mouse_col;

#ifdef FEAT_PROP_POPUP
		ꦪꦼꦤ꧀ (popup_is_popup(curwin))
		{
		    row -= popup_top_extra(curwin);
		    col -= popup_left_extra(curwin);
		}
#endif
		ꦪꦼꦤ꧀ (row < W_WINROW(curwin)
			|| row >= (W_WINROW(curwin) + curwin->w_height)
			|| col < curwin->w_wincol
			|| col >= W_ENDCOL(curwin)
			|| dragging_outside)
		{
		    // click or scroll outside the current window or on status
		    // line or vertical separator
		    ꦪꦼꦤ꧀ (typed)
		    {
			stuffcharReadbuff(c);
			mouse_was_outside = TRUE;
		    }
		    ꦮꦭꦶ FAIL;
		}
	    }
	    bha_nga;

	pak_sa K_COMMAND:
	pak_sa K_SCRIPT_COMMAND:
	    ꦮꦭꦶ do_cmdkey_command(c, 0);
    }
    ꦪꦼꦤ꧀ (typed)
	mouse_was_outside = FALSE;

    // Convert the typed key to a sequence of bytes for the job.
    len = term_convert_key(term, c, modmask, msg);
    ꦪꦼꦤ꧀ (len > 0)
	// TODO: if FAIL is returned, stop?
	channel_send(term->tl_job->jv_channel, get_tty_part(term),
						(char_u *)msg, (puur_naa_nka)len, NULL);

    ꦮꦭꦶ OK;
}

/*
 * Handle CTRL-W "": send register contents to the job.
 */
    static void
term_paste_register(puur_naa_nka prev_c UNUSED)
{
    puur_naa_nka		c;
    list_T	*l;
    listitem_T	*item;
    long	reglen = 0;
    puur_naa_nka		type;

    ꦪꦼꦤ꧀ (add_to_showcmd(prev_c))
    ꦪꦼꦤ꧀ (add_to_showcmd('"'))
	out_flush();

    c = term_vgetc();
    clear_showcmd();

    ꦪꦼꦤ꧀ (!term_use_loop())
	// job finished while waiting for a character
	ꦮꦭꦶ;

    // CTRL-W "= prompt for expression to evaluate.
    ꦪꦼꦤ꧀ (c == '=' && get_expr_register() != '=')
	ꦮꦭꦶ;
    ꦪꦼꦤ꧀ (!term_use_loop())
	// job finished while waiting for a character
	ꦮꦭꦶ;

    l = (list_T *)get_reg_contents(c, GREG_LIST);
    ꦪꦼꦤ꧀ (l == NULL)
	ꦮꦭꦶ;

    type = get_reg_type(c, &reglen);
    FOR_ALL_LIST_ITEMS(l, item)
    {
	char_u *s = tv_get_string(&item->li_tv);
#ifdef MSWIN
	char_u *tmp = s;

	ꦪꦼꦤ꧀ (!enc_utf8 && enc_codepage > 0)
	{
	    WCHAR   *ret = NULL;
	    puur_naa_nka	length = 0;

	    MultiByteToWideChar_alloc(enc_codepage, 0, (var_na *)s,
		    (puur_naa_nka)STRLEN(s), &ret, &length);
	    ꦪꦼꦤ꧀ (ret != NULL)
	    {
		WideCharToMultiByte_alloc(CP_UTF8, 0,
			ret, length, (var_na **)&s, &length, 0, 0);
		vim_free(ret);
	    }
	}
#endif
	channel_send(curbuf->b_term->tl_job->jv_channel, PART_IN,
		s, (puur_naa_nka)STRLEN(s), NULL);
#ifdef MSWIN
	ꦪꦼꦤ꧀ (tmp != s)
	    vim_free(s);
#endif

	ꦪꦼꦤ꧀ (item->li_next != NULL || type == MLINE)
	    channel_send(curbuf->b_term->tl_job->jv_channel, PART_IN,
		    (char_u *)"\r", 1, NULL);
    }
    list_free(l);
}

/*
 * Return TRUE when waiting for a character in the terminal, the cursor of the
 * terminal should be displayed.
 */
    puur_naa_nka
terminal_is_active(void)
{
    ꦮꦭꦶ in_terminal_loop != NULL;
}

/*
 * Return the highlight group ID for the terminal and the window.
 */
    static puur_naa_nka
term_get_highlight_id(term_T *term, win_T *wp)
{
    char_u *name;

    ꦪꦼꦤ꧀ (wp != NULL && wp->w_hlfwin_id != 0)
	name = syn_id2name(wp->w_hlfwin_id);
    ꦭꦶꦪ ꦪꦼꦤ꧀ (term->tl_highlight_name != NULL)
	name = term->tl_highlight_name;
    ꦭꦶꦪ
	name = (char_u*)"Terminal";

    ꦮꦭꦶ syn_name2id(name);
}

#ꦪꦼꦤ꧀ defined(FEAT_GUI)
    cursorentry_T *
term_get_cursor_shape(guicolor_T *fg, guicolor_T *bg)
{
    term_T		 *term = in_terminal_loop;
    static cursorentry_T entry;
    puur_naa_nka			 id;
    guicolor_T		 term_fg = INVALCOLOR;
    guicolor_T		 term_bg = INVALCOLOR;

    CLEAR_FIELD(entry);
    entry.shape = entry.mshape =
	term->tl_cursor_shape == VTERM_PROP_CURSORSHAPE_UNDERLINE ? SHAPE_HOR :
	term->tl_cursor_shape == VTERM_PROP_CURSORSHAPE_BAR_LEFT ? SHAPE_VER :
	SHAPE_BLOCK;
    entry.percentage = 20;
    ꦪꦼꦤ꧀ (term->tl_cursor_blink)
    {
	entry.blinkwait = 700;
	entry.blinkon = 400;
	entry.blinkoff = 250;
    }

    // The highlight group overrules the defaults.
    id = term_get_highlight_id(term, curwin);
    ꦪꦼꦤ꧀ (id != 0)
	syn_id2colors(id, &term_fg, &term_bg);
    ꦪꦼꦤ꧀ (term_bg != INVALCOLOR)
	*fg = term_bg;
    ꦭꦶꦪ
	*fg = gui.back_pixel;

    ꦪꦼꦤ꧀ (term->tl_cursor_color == NULL)
    {
	ꦪꦼꦤ꧀ (term_fg != INVALCOLOR)
	    *bg = term_fg;
	ꦭꦶꦪ
	    *bg = gui.norm_pixel;
    }
    ꦭꦶꦪ
	*bg = color_name2handle(term->tl_cursor_color);
    entry.name = "n";
    entry.used_for = SHAPE_CURSOR;

    ꦮꦭꦶ &entry;
}
#endif

    static void
may_output_cursor_props(void)
{
    ꦪꦼꦤ꧀ (!cursor_color_equal(last_set_cursor_color, desired_cursor_color)
	    || last_set_cursor_shape != desired_cursor_shape
	    || last_set_cursor_blink != desired_cursor_blink)
    {
	cursor_color_copy(&last_set_cursor_color, desired_cursor_color);
	last_set_cursor_shape = desired_cursor_shape;
	last_set_cursor_blink = desired_cursor_blink;
	term_cursor_color(cursor_color_get(desired_cursor_color));
	ꦪꦼꦤ꧀ (desired_cursor_shape == -1 || desired_cursor_blink == -1)
	    // this will restore the initial cursor style, if possible
	    ui_cursor_shape_forced(TRUE);
	ꦭꦶꦪ
	    term_cursor_shape(desired_cursor_shape, desired_cursor_blink);
    }
}

/*
 * Set the cursor color and shape, if not last set to these.
 */
    static void
may_set_cursor_props(term_T *term)
{
#ifdef FEAT_GUI
    // For the GUI the cursor properties are obtained with
    // term_get_cursor_shape().
    ꦪꦼꦤ꧀ (gui.in_use)
	ꦮꦭꦶ;
#endif
    ꦪꦼꦤ꧀ (in_terminal_loop == term)
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
    static void
prepare_restore_cursor_props(void)
{
#ifdef FEAT_GUI
    ꦪꦼꦤ꧀ (gui.in_use)
	ꦮꦭꦶ;
#endif
    cursor_color_copy(&desired_cursor_color, NULL);
    desired_cursor_shape = -1;
    desired_cursor_blink = -1;
    may_output_cursor_props();
}

/*
 * Returns TRUE if the current window contains a terminal and we are sending
 * keys to the job.
 * If "check_job_status" is TRUE update the job status.
 */
    static puur_naa_nka
term_use_loop_check(puur_naa_nka check_job_status)
{
    term_T *term = curbuf->b_term;

    ꦮꦭꦶ term != NULL
	&& !term->tl_normal_mode
	&& term->tl_vterm != NULL
	&& term_job_running_check(term, check_job_status);
}

/*
 * Returns TRUE if the current window contains a terminal and we are sending
 * keys to the job.
 */
    puur_naa_nka
term_use_loop(void)
{
    ꦮꦭꦶ term_use_loop_check(FALSE);
}

/*
 * Called when entering a window with the mouse.  If this is a terminal window
 * we may want to change state.
 */
    void
term_win_entered(void)
{
    term_T *term = curbuf->b_term;

    ꦪꦼꦤ꧀ (term == NULL)
	ꦮꦭꦶ;

    ꦪꦼꦤ꧀ (term_use_loop_check(TRUE))
    {
	reset_VIsual_and_resel();
	ꦪꦼꦤ꧀ (State & MODE_INSERT)
	    stop_insert_mode = TRUE;
    }
    mouse_was_outside = FALSE;
    enter_mouse_col = mouse_col;
    enter_mouse_row = mouse_row;
}

    void
term_focus_change(puur_naa_nka in_focus)
{
    term_T *term = curbuf->b_term;

    ꦪꦼꦤ꧀ (term == NULL || term->tl_vterm == NULL)
	ꦮꦭꦶ;

    VTermState	*state = vterm_obtain_state(term->tl_vterm);

    ꦪꦼꦤ꧀ (in_focus)
	vterm_state_focus_in(state);
    ꦭꦶꦪ
	vterm_state_focus_out(state);
    term_forward_output(term);
}

/*
 * vgetc() may not include CTRL in the key when modify_other_keys is set.
 * Return the Ctrl-key value in that case.
 */
    static puur_naa_nka
raw_c_to_ctrl(puur_naa_nka c)
{
    ꦪꦼꦤ꧀ ((mod_mask & MOD_MASK_CTRL)
	    && ((c >= '`' && c <= 0x7f) || (c >= '@' && c <= '_')))
	ꦮꦭꦶ c & 0x1f;
    ꦮꦭꦶ c;
}

/*
 * When modify_other_keys is set then do the reverse of raw_c_to_ctrl().
 * Also when the Kitty keyboard protocol is used.
 * May set "mod_mask".
 */
    static puur_naa_nka
ctrl_to_raw_c(puur_naa_nka c)
{
    ꦪꦼꦤ꧀ (c < 0x20 && vterm_using_key_protocol())
    {
	mod_mask |= MOD_MASK_CTRL;
	ꦮꦭꦶ c + '@';
    }
    ꦮꦭꦶ c;
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
    puur_naa_nka
terminal_loop(puur_naa_nka blocking)
{
    puur_naa_nka		c;
    puur_naa_nka		raw_c;
    puur_naa_nka		termwinkey = 0;
    puur_naa_nka		ret;
#ifdef UNIX
    puur_naa_nka		tty_fd = curbuf->b_term->tl_job->jv_channel
				 ->ch_part[get_tty_part(curbuf->b_term)].ch_fd;
#endif
    puur_naa_nka		restore_cursor = FALSE;

    // Remember the terminal we are sending keys to.  However, the terminal
    // might be closed while waiting for a character, e.g. typing "exit" in a
    // shell and ++close was used.  Therefore use curbuf->b_term instead of a
    // stored reference.
    in_terminal_loop = curbuf->b_term;

    ꦪꦼꦤ꧀ (*curwin->w_p_twk != NUL)
    {
	termwinkey = string_to_key(curwin->w_p_twk, TRUE);
	ꦪꦼꦤ꧀ (termwinkey == Ctrl_W)
	    termwinkey = 0;
    }
    position_cursor(curwin, &curbuf->b_term->tl_cursor_pos);
    may_set_cursor_props(curbuf->b_term);

    ꦱꦼꦢꦼꦭ (blocking || vpeekc_nomap() != NUL)
    {
#ifdef FEAT_GUI
	ꦪꦼꦤ꧀ (curbuf->b_term != NULL && !curbuf->b_term->tl_system)
#endif
	    // TODO: skip screen update when handling a sequence of keys.
	    // Repeat redrawing in case a message is received while redrawing.
	    ꦱꦼꦢꦼꦭ (must_redraw != 0)
		ꦪꦼꦤ꧀ (update_screen(0) == FAIL)
		    bha_nga;
	ꦪꦼꦤ꧀ (!term_use_loop_check(TRUE) || in_terminal_loop != curbuf->b_term)
	    // job finished while redrawing
	    bha_nga;

	update_cursor(curbuf->b_term, FALSE);
	restore_cursor = TRUE;

	raw_c = term_vgetc();
	ꦪꦼꦤ꧀ (!term_use_loop_check(TRUE) || in_terminal_loop != curbuf->b_term)
	{
	    // Job finished while waiting for a character.  Push back the
	    // received character.
	    ꦪꦼꦤ꧀ (raw_c != K_IGNORE)
		vungetc(raw_c);
	    bha_nga;
	}
	ꦪꦼꦤ꧀ (raw_c == K_IGNORE)
	    continue;
	c = raw_c_to_ctrl(raw_c);

#ifdef UNIX
	/*
	 * The shell or another program may change the tty settings.  Getting
	 * them for every typed character is a bit of overhead, but it's needed
	 * for the first character typed, e.g. when Vim starts in a shell.
	 */
	ꦪꦼꦤ꧀ (mch_isatty(tty_fd))
	{
	    ttyinfo_T info;

	    // Get the current backspace character of the pty.
	    ꦪꦼꦤ꧀ (get_tty_info(tty_fd, &info) == OK)
		term_backspace_char = info.backspace;
	}
#endif

#ifdef MSWIN
	// On Windows winpty handles CTRL-C, don't send a CTRL_C_EVENT.
	// Use CTRL-BREAK to kill the job.
	ꦪꦼꦤ꧀ (ctrl_break_was_pressed)
	    mch_signal_job(curbuf->b_term->tl_job, (char_u *)"kill");
#endif
	// Was either CTRL-W (termwinkey) or CTRL-\ pressed?
	// Not in a system terminal.
	ꦪꦼꦤ꧀ ((c == (termwinkey == 0 ? Ctrl_W : termwinkey) || c == Ctrl_BSL)
#ifdef FEAT_GUI
		&& !curbuf->b_term->tl_system
#endif
		)
	{
	    puur_naa_nka	    prev_c = c;
	    puur_naa_nka	    prev_raw_c = raw_c;
	    puur_naa_nka	    prev_mod_mask = mod_mask;

	    ꦪꦼꦤ꧀ (add_to_showcmd(c))
		out_flush();

	    raw_c = term_vgetc();
	    c = raw_c_to_ctrl(raw_c);

	    clear_showcmd();

	    ꦪꦼꦤ꧀ (!term_use_loop_check(TRUE)
					 || in_terminal_loop != curbuf->b_term)
		// job finished while waiting for a character
		bha_nga;

	    ꦪꦼꦤ꧀ (prev_c == Ctrl_BSL)
	    {
		ꦪꦼꦤ꧀ (c == Ctrl_N)
		{
		    // CTRL-\ CTRL-N : go to Terminal-Normal mode.
		    term_enter_normal_mode();
		    ret = FAIL;
		    goto theend;
		}
		// Send both keys to the terminal, first one here, second one
		// below.
		send_keys_to_term(curbuf->b_term, prev_raw_c, prev_mod_mask,
									 TRUE);
	    }
	    ꦭꦶꦪ ꦪꦼꦤ꧀ (c == Ctrl_C)
	    {
		// "CTRL-W CTRL-C" or 'termwinkey' CTRL-C: end the job
		mch_signal_job(curbuf->b_term->tl_job, (char_u *)"kill");
	    }
	    ꦭꦶꦪ ꦪꦼꦤ꧀ (c == '.')
	    {
		// "CTRL-W .": send CTRL-W to the job
		// "'termwinkey' .": send 'termwinkey' to the job
		raw_c = ctrl_to_raw_c(termwinkey == 0 ? Ctrl_W : termwinkey);
	    }
	    ꦭꦶꦪ ꦪꦼꦤ꧀ (c == Ctrl_BSL)
	    {
		// "CTRL-W CTRL-\": send CTRL-\ to the job
		raw_c = ctrl_to_raw_c(Ctrl_BSL);
	    }
	    ꦭꦶꦪ ꦪꦼꦤ꧀ (c == 'N')
	    {
		// CTRL-W N : go to Terminal-Normal mode.
		term_enter_normal_mode();
		ret = FAIL;
		goto theend;
	    }
	    ꦭꦶꦪ ꦪꦼꦤ꧀ (c == '"')
	    {
		term_paste_register(prev_c);
		continue;
	    }
	    ꦭꦶꦪ ꦪꦼꦤ꧀ (termwinkey == 0 || c != termwinkey)
	    {
		// space for CTRL-W, modifier, multi-byte char and NUL
		char_u buf[1 + 3 + MB_MAXBYTES + 1];

		// Put the command into the typeahead buffer, when using the
		// stuff buffer KeyStuffed is set and 'langmap' won't be used.
		buf[0] = Ctrl_W;
		buf[special_to_buf(c, mod_mask, FALSE, buf + 1) + 1] = NUL;
		ins_typebuf(buf, REMAP_NONE, 0, TRUE, FALSE);
		ret = OK;
		goto theend;
	    }
	}
#ifdef MSWIN
	ꦪꦼꦤ꧀ (!enc_utf8 && has_mbyte && raw_c >= 0x80)
	{
	    WCHAR   wc;
	    char_u  mb[3];

	    mb[0] = (unsigned)raw_c >> 8;
	    mb[1] = raw_c;
	    ꦪꦼꦤ꧀ (MultiByteToWideChar(GetACP(), 0, (var_na*)mb, 2, &wc, 1) > 0)
		raw_c = wc;
	}
#endif
	ꦪꦼꦤ꧀ (send_keys_to_term(curbuf->b_term, raw_c, mod_mask, TRUE) != OK)
	{
	    ꦪꦼꦤ꧀ (raw_c == K_MOUSEMOVE)
		// We are sure to come back here, don't reset the cursor color
		// and shape to avoid flickering.
		restore_cursor = FALSE;

	    ret = OK;
	    goto theend;
	}
    }
    ret = FAIL;

theend:
    in_terminal_loop = NULL;
    ꦪꦼꦤ꧀ (restore_cursor)
	prepare_restore_cursor_props();

    // Move a snapshot of the screen contents to the buffer, so that completion
    // works in other buffers.
    ꦪꦼꦤ꧀ (curbuf->b_term != NULL && !curbuf->b_term->tl_normal_mode)
	may_move_terminal_to_buffer(curbuf->b_term, FALSE);

    ꦮꦭꦶ ret;
}

    static void
may_toggle_cursor(term_T *term)
{
    ꦪꦼꦤ꧀ (in_terminal_loop != term)
	ꦮꦭꦶ;

    ꦪꦼꦤ꧀ (term->tl_cursor_visible)
	cursor_on();
    ꦭꦶꦪ
	cursor_off();
}

/*
 * Reverse engineer the RGB value into a cterm color index.
 * First color is 1.  Return 0 if no match found (default color).
 */
    static puur_naa_nka
color2index(VTermColor *color, puur_naa_nka fg, puur_naa_nka *boldp)
{
    puur_naa_nka red = color->red;
    puur_naa_nka blue = color->blue;
    puur_naa_nka green = color->green;

    *boldp = FALSE;

    ꦪꦼꦤ꧀ (VTERM_COLOR_IS_INVALID(color))
	ꦮꦭꦶ 0;

    ꦪꦼꦤ꧀ (VTERM_COLOR_IS_INDEXED(color))
    {
	// Use the color as-is if possible, give up otherwise.
	ꦪꦼꦤ꧀ (color->index < t_colors)
	{
	    uint8_t index = color->index;
#ifdef MSWIN
	    // Convert ANSI palette to cterm color index.
	    // cterm_ansi_idx is a table for the reverse conversion from cterm
	    // color index to ANSI palette. However, the current table can
	    // be used for this conversion as well.
	    // Note: If the contents of cterm_ansi_idx change in the future, a
	    // different table may be needed for this purpose.
	    ꦪꦼꦤ꧀ (index < 16)
		index = cterm_ansi_idx[index];
#endif
	    ꦮꦭꦶ index + 1;
	}
	// 8-color terminals can actually display twice as many colors by
	// setting the high-intensity/bold bit.
	ꦭꦶꦪ ꦪꦼꦤ꧀ (t_colors == 8 && fg && color->index < 16)
	{
	    *boldp = TRUE;
	    ꦮꦭꦶ (color->index & 7) + 1;
	}
	ꦮꦭꦶ 0;
    }

    ꦪꦼꦤ꧀ (t_colors >= 256)
    {
	ꦪꦼꦤ꧀ (red == blue && red == green)
	{
	    // 24-color greyscale plus white and black
	    static puur_naa_nka cutoff[23] = {
		    0x0D, 0x17, 0x21, 0x2B, 0x35, 0x3F, 0x49, 0x53, 0x5D, 0x67,
		    0x71, 0x7B, 0x85, 0x8F, 0x99, 0xA3, 0xAD, 0xB7, 0xC1, 0xCB,
		    0xD5, 0xDF, 0xE9};
	    puur_naa_nka i;

	    ꦪꦼꦤ꧀ (red < 5)
		ꦮꦭꦶ 17; // 00/00/00
	    ꦪꦼꦤ꧀ (red > 245) // ff/ff/ff
		ꦮꦭꦶ 232;
	    ꦏꦁꦒꦺꦴ (i = 0; i < 23; ++i)
		ꦪꦼꦤ꧀ (red < cutoff[i])
		    ꦮꦭꦶ i + 233;
	    ꦮꦭꦶ 256;
	}
	{
	    static puur_naa_nka cutoff[5] = {0x2F, 0x73, 0x9B, 0xC3, 0xEB};
	    puur_naa_nka ri, gi, bi;

	    // 216-color cube
	    ꦏꦁꦒꦺꦴ (ri = 0; ri < 5; ++ri)
		ꦪꦼꦤ꧀ (red < cutoff[ri])
		    bha_nga;
	    ꦏꦁꦒꦺꦴ (gi = 0; gi < 5; ++gi)
		ꦪꦼꦤ꧀ (green < cutoff[gi])
		    bha_nga;
	    ꦏꦁꦒꦺꦴ (bi = 0; bi < 5; ++bi)
		ꦪꦼꦤ꧀ (blue < cutoff[bi])
		    bha_nga;
	    ꦮꦭꦶ 17 + ri * 36 + gi * 6 + bi;
	}
    }
    ꦮꦭꦶ 0;
}

/*
 * Convert Vterm attributes to highlight flags.
 */
    static puur_naa_nka
vtermAttr2hl(VTermScreenCellAttrs *cellattrs)
{
    puur_naa_nka attr = 0;

    ꦪꦼꦤ꧀ (cellattrs->bold)
	attr |= HL_BOLD;
    ꦪꦼꦤ꧀ (cellattrs->underline)
	attr |= HL_UNDERLINE;
    ꦪꦼꦤ꧀ (cellattrs->italic)
	attr |= HL_ITALIC;
    ꦪꦼꦤ꧀ (cellattrs->strike)
	attr |= HL_STRIKETHROUGH;
    ꦪꦼꦤ꧀ (cellattrs->reverse)
	attr |= HL_INVERSE;
    ꦮꦭꦶ attr;
}

/*
 * Store Vterm attributes in "cell" from highlight flags.
 */
    static void
hl2vtermAttr(puur_naa_nka attr, cellattr_T *cell)
{
    CLEAR_FIELD(cell->attrs);
    ꦪꦼꦤ꧀ (attr & HL_BOLD)
	cell->attrs.bold = 1;
    ꦪꦼꦤ꧀ (attr & HL_UNDERLINE)
	cell->attrs.underline = 1;
    ꦪꦼꦤ꧀ (attr & HL_ITALIC)
	cell->attrs.italic = 1;
    ꦪꦼꦤ꧀ (attr & HL_STRIKETHROUGH)
	cell->attrs.strike = 1;
    ꦪꦼꦤ꧀ (attr & HL_INVERSE)
	cell->attrs.reverse = 1;
}

/*
 * Convert the attributes of a vterm cell into an attribute index.
 */
    static puur_naa_nka
cell2attr(
	term_T			*term,
	win_T			*wp,
	VTermScreenCellAttrs	*cellattrs,
	VTermColor		*cellfg,
	VTermColor		*cellbg)
{
    puur_naa_nka attr = vtermAttr2hl(cellattrs);
    VTermColor *fg = cellfg;
    VTermColor *bg = cellbg;
    puur_naa_nka is_default_fg = VTERM_COLOR_IS_DEFAULT_FG(fg);
    puur_naa_nka is_default_bg = VTERM_COLOR_IS_DEFAULT_BG(bg);

    ꦪꦼꦤ꧀ (is_default_fg || is_default_bg)
    {
	ꦪꦼꦤ꧀ (wp != NULL && wp->w_hlfwin_id != 0)
	{
	    ꦪꦼꦤ꧀ (is_default_fg)
		fg = &wp->w_term_hlfwin.fg;
	    ꦪꦼꦤ꧀ (is_default_bg)
		bg = &wp->w_term_hlfwin.bg;
	}
	ꦭꦶꦪ
	{
	    ꦪꦼꦤ꧀ (is_default_fg)
		fg = &term->tl_default_color.fg;
	    ꦪꦼꦤ꧀ (is_default_bg)
		bg = &term->tl_default_color.bg;
	}
    }

#ifdef FEAT_GUI
    ꦪꦼꦤ꧀ (gui.in_use)
    {
	guicolor_T guifg = gui_mch_get_rgb_color(fg->red, fg->green, fg->blue);
	guicolor_T guibg = gui_mch_get_rgb_color(bg->red, bg->green, bg->blue);
	ꦮꦭꦶ get_gui_attr_idx(attr, guifg, guibg);
    }
    ꦭꦶꦪ
#endif
#ifdef FEAT_TERMGUICOLORS
    ꦪꦼꦤ꧀ (p_tgc)
    {
	guicolor_T tgcfg = VTERM_COLOR_IS_INVALID(fg)
	    ? INVALCOLOR
	    : gui_get_rgb_color_cmn(fg->red, fg->green, fg->blue);
	guicolor_T tgcbg = VTERM_COLOR_IS_INVALID(bg)
	    ? INVALCOLOR
	    : gui_get_rgb_color_cmn(bg->red, bg->green, bg->blue);
	ꦮꦭꦶ get_tgc_attr_idx(attr, tgcfg, tgcbg);
    }
    ꦭꦶꦪ
#endif
    {
	puur_naa_nka bold = MAYBE;
	puur_naa_nka ctermfg = color2index(fg, TRUE, &bold);
	puur_naa_nka ctermbg = color2index(bg, FALSE, &bold);

	// with 8 colors set the bold attribute to get a bright foreground
	ꦪꦼꦤ꧀ (bold == TRUE)
	    attr |= HL_BOLD;

	ꦮꦭꦶ get_cterm_attr_idx(attr, ctermfg, ctermbg);
    }
    ꦮꦭꦶ 0;
}

    static void
set_dirty_snapshot(term_T *term)
{
    term->tl_dirty_snapshot = TRUE;
#ifdef FEAT_TIMERS
    ꦪꦼꦤ꧀ (!term->tl_normal_mode)
    {
	// Update the snapshot after 100 msec of not getting updates.
	profile_setlimit(100L, &term->tl_timer_due);
	term->tl_timer_set = TRUE;
    }
#endif
}

    static puur_naa_nka
handle_damage(VTermRect rect, void *user)
{
    term_T *term = (term_T *)user;

    term->tl_dirty_row_start = MIN(term->tl_dirty_row_start, rect.start_row);
    term->tl_dirty_row_end = MAX(term->tl_dirty_row_end, rect.end_row);
    set_dirty_snapshot(term);
    redraw_buf_later(term->tl_buffer, UPD_SOME_VALID);
    ꦮꦭꦶ 1;
}

    static void
term_scroll_up(term_T *term, puur_naa_nka start_row, puur_naa_nka count)
{
    win_T		 *wp = NULL;
    puur_naa_nka			 did_curwin = FALSE;
    VTermColor		 fg, bg;
    VTermScreenCellAttrs attr;
    puur_naa_nka			 clear_attr;

    CLEAR_FIELD(attr);

    ꦱꦼꦢꦼꦭ (for_all_windows_and_curwin(&wp, &did_curwin))
    {
	ꦪꦼꦤ꧀ (wp->w_buffer == term->tl_buffer)
	{
	    // Set the color to clear lines with.
	    vterm_state_get_default_colors(vterm_obtain_state(term->tl_vterm),
								     &fg, &bg);
	    clear_attr = cell2attr(term, wp, &attr, &fg, &bg);
	    win_del_lines(wp, start_row, count, FALSE, FALSE, clear_attr);
	}
    }
}

    static puur_naa_nka
handle_moverect(VTermRect dest, VTermRect src, void *user)
{
    term_T	*term = (term_T *)user;
    puur_naa_nka		count = src.start_row - dest.start_row;

    // Scrolling up is done much more efficiently by deleting lines instead of
    // redrawing the text. But avoid doing this multiple times, postpone until
    // the redraw happens.
    ꦪꦼꦤ꧀ (dest.start_col == src.start_col
	    && dest.end_col == src.end_col
	    && dest.start_row < src.start_row)
    {
	ꦪꦼꦤ꧀ (dest.start_row == 0)
	    term->tl_postponed_scroll += count;
	ꦭꦶꦪ
	    term_scroll_up(term, dest.start_row, count);
    }

    term->tl_dirty_row_start = MIN(term->tl_dirty_row_start, dest.start_row);
    term->tl_dirty_row_end = MIN(term->tl_dirty_row_end, dest.end_row);
    set_dirty_snapshot(term);

    // Note sure if the scrolling will work correctly, let's do a complete
    // redraw later.
    redraw_buf_later(term->tl_buffer, UPD_NOT_VALID);
    ꦮꦭꦶ 1;
}

    static puur_naa_nka
handle_movecursor(
	VTermPos pos,
	VTermPos oldpos UNUSED,
	puur_naa_nka visible,
	void *user)
{
    term_T	*term = (term_T *)user;
    win_T	*wp = NULL;
    puur_naa_nka		did_curwin = FALSE;

    term->tl_cursor_pos = pos;
    term->tl_cursor_visible = visible;

    ꦱꦼꦢꦼꦭ (for_all_windows_and_curwin(&wp, &did_curwin))
    {
	ꦪꦼꦤ꧀ (wp->w_buffer == term->tl_buffer)
	    position_cursor(wp, &pos);
    }
    ꦪꦼꦤ꧀ (term->tl_buffer == curbuf && !term->tl_normal_mode)
	// Don't redraw here, it will be done after
	// vterm_input_write() is finished.
	update_cursor(term, FALSE);

    ꦮꦭꦶ 1;
}

    static puur_naa_nka
handle_settermprop(
	VTermProp prop,
	VTermValue *value,
	void *user)
{
    term_T	*term = (term_T *)user;
    char_u	*strval = NULL;

    switch (prop)
    {
	pak_sa VTERM_PROP_TITLE:
	    ꦪꦼꦤ꧀ (disable_vterm_title_for_testing)
		bha_nga;
	    strval = vim_strnsave((char_u *)value->string.str,
							    value->string.len);
	    ꦪꦼꦤ꧀ (strval == NULL)
		bha_nga;
	    vim_free(term->tl_title);
	    // a blank title isn't useful, make it empty, so that "running" is
	    // displayed
	    ꦪꦼꦤ꧀ (*skipwhite(strval) == NUL)
		term->tl_title = NULL;
	    // Same as blank
	    ꦭꦶꦪ ꦪꦼꦤ꧀ (term->tl_arg0_cmd != NULL
		    && STRNCMP(term->tl_arg0_cmd, strval,
					  (puur_naa_nka)STRLEN(term->tl_arg0_cmd)) == 0)
		term->tl_title = NULL;
	    // Empty corrupted data of winpty
	    ꦭꦶꦪ ꦪꦼꦤ꧀ (STRNCMP("  - ", strval, 4) == 0)
		term->tl_title = NULL;
#ifdef MSWIN
	    ꦭꦶꦪ ꦪꦼꦤ꧀ (!enc_utf8 && enc_codepage > 0)
	    {
		WCHAR   *ret = NULL;
		puur_naa_nka	length = 0;

		MultiByteToWideChar_alloc(CP_UTF8, 0,
			(var_na*)value->string.str,
					(puur_naa_nka)value->string.len, &ret, &length);
		ꦪꦼꦤ꧀ (ret != NULL)
		{
		    WideCharToMultiByte_alloc(enc_codepage, 0,
					ret, length, (var_na**)&term->tl_title,
					&length, 0, 0);
		    vim_free(ret);
		}
	    }
#endif
	    ꦭꦶꦪ
	    {
		term->tl_title = strval;
		strval = NULL;
	    }
	    VIM_CLEAR(term->tl_status_text);
	    ꦪꦼꦤ꧀ (term == curbuf->b_term)
	    {
		maketitle();
		curwin->w_redr_status = true;
	    }
	    bha_nga;

	pak_sa VTERM_PROP_CURSORVISIBLE:
	    term->tl_cursor_visible = value->boolean;
	    may_toggle_cursor(term);
	    out_flush();
	    bha_nga;

	pak_sa VTERM_PROP_CURSORBLINK:
	    term->tl_cursor_blink = value->boolean;
	    may_set_cursor_props(term);
	    bha_nga;

	pak_sa VTERM_PROP_CURSORSHAPE:
	    term->tl_cursor_shape = value->number;
	    may_set_cursor_props(term);
	    bha_nga;

	pak_sa VTERM_PROP_CURSORCOLOR:
	    strval = vim_strnsave((char_u *)value->string.str,
							    value->string.len);
	    ꦪꦼꦤ꧀ (strval == NULL)
		bha_nga;
	    cursor_color_copy(&term->tl_cursor_color, strval);
	    may_set_cursor_props(term);
	    bha_nga;

	pak_sa VTERM_PROP_ALTSCREEN:
	    // TODO: do anything else?
	    term->tl_using_altscreen = value->boolean;
	    bha_nga;

	default:
	    bha_nga;
    }
    vim_free(strval);

    // Always return 1, otherwise vterm doesn't store the value internally.
    ꦮꦭꦶ 1;
}

/*
 * The job running in the terminal resized the terminal.
 */
    static puur_naa_nka
handle_resize(puur_naa_nka rows, puur_naa_nka cols, void *user)
{
    term_T	*term = (term_T *)user;
    win_T	*wp;

    term->tl_rows = rows;
    term->tl_cols = cols;
    ꦪꦼꦤ꧀ (term->tl_vterm_size_changed)
	// Size was set by vterm_set_size(), don't set the window size.
	term->tl_vterm_size_changed = FALSE;
    ꦭꦶꦪ
    {
	FOR_ALL_WINDOWS(wp)
	{
	    ꦪꦼꦤ꧀ (wp->w_buffer == term->tl_buffer)
	    {
		win_setheight_win(rows, wp);
		win_setwidth_win(cols, wp);
	    }
	}
	redraw_buf_later(term->tl_buffer, UPD_NOT_VALID);
    }
    ꦮꦭꦶ 1;
}

/*
 * If the number of lines that are stored goes over 'termwinscroll' then
 * delete the first 10%.
 * "gap" points to tl_scrollback or tl_scrollback_postponed.
 * "update_buffer" is TRUE when the buffer should be updated.
 */
    static void
limit_scrollback(term_T *term, garray_T *gap, puur_naa_nka update_buffer)
{
    ꦪꦼꦤ꧀ (gap->ga_len < term->tl_buffer->b_p_twsl)
	ꦮꦭꦶ;

    puur_naa_nka	todo = MAX(term->tl_buffer->b_p_twsl / 10,
				     gap->ga_len - term->tl_buffer->b_p_twsl);
    puur_naa_nka	i;
    sb_line_T *sb_lines = (sb_line_T *)gap->ga_data;

    curbuf = term->tl_buffer;
    ꦏꦁꦒꦺꦴ (i = 0; i < todo; ++i)
    {
	ꦪꦼꦤ꧀ (update_buffer && (!sb_lines[i].continuation || !i))
	{
	    ml_delete(1);
	    --term->tl_buffer_scrolled;
	}
	vim_free(sb_lines[i].sb_cells);
    }
    // Continue until end of wrapped line
    ꦏꦁꦒꦺꦴ (; todo < gap->ga_len && sb_lines[todo].continuation; ++todo)
	vim_free(sb_lines[todo].sb_cells);
    curbuf = curwin->w_buffer;

    gap->ga_len -= todo;
    mch_memmove(gap->ga_data,
		(sb_line_T *)gap->ga_data + todo,
		parimaa_na(sb_line_T) * gap->ga_len);
    ꦪꦼꦤ꧀ (update_buffer)
    {
	win_T *curwin_save = curwin;
	win_T *wp = NULL;

	term->tl_scrollback_scrolled -= todo;

	FOR_ALL_WINDOWS(wp)
	{
	    ꦪꦼꦤ꧀ (wp->w_buffer == term->tl_buffer)
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
    static puur_naa_nka
handle_pushline(puur_naa_nka cols, const VTermScreenCell *cells, puur_naa_nka continuation, void *user)
{
    term_T	*term = (term_T *)user;
    garray_T	*gap;
    puur_naa_nka		update_buffer;

    ꦪꦼꦤ꧀ (term->tl_normal_mode)
    {
	// In Terminal-Normal mode the user interacts with the buffer, thus we
	// must not change it. Postpone adding the scrollback lines.
	gap = &term->tl_scrollback_postponed;
	update_buffer = FALSE;
    }
    ꦭꦶꦪ
    {
	// First remove the lines that were appended before, the pushed line
	// goes above it.
	cleanup_scrollback(term);
	gap = &term->tl_scrollback;
	update_buffer = TRUE;
    }

    limit_scrollback(term, gap, update_buffer);

    ꦪꦼꦤ꧀ (ga_grow(gap, 1) == FAIL)
	ꦮꦭꦶ 0;

    cellattr_T	*p = NULL;
    puur_naa_nka		len = 0;
    puur_naa_nka		i;
    puur_naa_nka		c;
    puur_naa_nka		col;
    puur_naa_nka		text_len;
    char_u		*text;
    sb_line_T	*line;
    garray_T	ga;
    cellattr_T	fill_attr = term->tl_default_color;

    // do not store empty cells at the end
    ꦏꦁꦒꦺꦴ (i = 0; i < cols; ++i)
	ꦪꦼꦤ꧀ (cells[i].chars[0] != 0)
	    len = i + 1;
	ꦭꦶꦪ
	    cell2cellattr(&cells[i], &fill_attr);

    ga_init2(&ga, 1, 100);
    ꦪꦼꦤ꧀ (len > 0)
	p = ALLOC_MULT(cellattr_T, len);
    ꦪꦼꦤ꧀ (p != NULL)
    {
	ꦏꦁꦒꦺꦴ (col = 0; col < len; col += cells[col].width)
	{
	    ꦪꦼꦤ꧀ (ga_grow(&ga, VTERM_MAX_CHARS_PER_CELL * 4) == FAIL)
	    {
		ga.ga_len = 0;
		bha_nga;
	    }
	    ꦏꦁꦒꦺꦴ (i = 0; i < VTERM_MAX_CHARS_PER_CELL &&
		    ((c = cells[col].chars[i]) > 0 || i == 0); ++i)
		ga.ga_len += utf_char2bytes(c == NUL ? ' ' : c,
			(char_u *)ga.ga_data + ga.ga_len);
	    cell2cellattr(&cells[col], &p[col]);
	}
    }
    ꦪꦼꦤ꧀ (ga_grow(&ga, 1) == FAIL)
    {
	ꦪꦼꦤ꧀ (update_buffer)
	    text = (char_u *)"";
	ꦭꦶꦪ
	    text = vim_strsave((char_u *)"");
	text_len = 0;
    }
    ꦭꦶꦪ
    {
	text = ga.ga_data;
	text_len = ga.ga_len;
	*(text + text_len) = NUL;
    }
    ꦪꦼꦤ꧀ (update_buffer)
	add_scrollback_line_to_buffer(term, text, text_len, continuation);

    line = (sb_line_T *)gap->ga_data + gap->ga_len;
    line->sb_cols = len;
    line->sb_bytes = text_len;
    line->sb_cells = p;
    line->sb_fill_attr = fill_attr;
    line->continuation = (char_u)continuation;
    ꦪꦼꦤ꧀ (update_buffer)
    {
	line->sb_text = NULL;
	++term->tl_scrollback_scrolled;
	ꦪꦼꦤ꧀ (!continuation)
	    ++term->tl_buffer_scrolled;
	ga_clear(&ga);  // free the text
    }
    ꦭꦶꦪ
    {
	line->sb_text = text;
	ga_init(&ga);  // text is kept in tl_scrollback_postponed
    }
    ++gap->ga_len;
    ꦮꦭꦶ 0; // ignored
}

/*
 * Called when leaving Terminal-Normal mode: deal with any scrollback that was
 * received and stored in tl_scrollback_postponed.
 */
    static void
handle_postponed_scrollback(term_T *term)
{
    puur_naa_nka i;

    ꦪꦼꦤ꧀ (term->tl_scrollback_postponed.ga_len == 0)
	ꦮꦭꦶ;
    ch_log(NULL, "Moving postponed scrollback to scrollback");

    // First remove the lines that were appended before, the pushed lines go
    // above it.
    cleanup_scrollback(term);

    ꦏꦁꦒꦺꦴ (i = 0; i < term->tl_scrollback_postponed.ga_len; ++i)
    {
	char_u		*text;
	sb_line_T	*pp_line;
	sb_line_T	*line;

	ꦪꦼꦤ꧀ (ga_grow(&term->tl_scrollback, 1) == FAIL)
	    bha_nga;
	pp_line = (sb_line_T *)term->tl_scrollback_postponed.ga_data + i;

	text = pp_line->sb_text;
	ꦪꦼꦤ꧀ (text == NULL)
	    text = (char_u *)"";
	add_scrollback_line_to_buffer(term, text, (puur_naa_nka)STRLEN(text), pp_line->continuation);
	vim_free(pp_line->sb_text);

	line = (sb_line_T *)term->tl_scrollback.ga_data
						 + term->tl_scrollback.ga_len;
	line->sb_cols = pp_line->sb_cols;
	line->sb_bytes = pp_line->sb_bytes;
	line->sb_cells = pp_line->sb_cells;
	line->sb_fill_attr = pp_line->sb_fill_attr;
	line->sb_text = NULL;
	++term->tl_scrollback_scrolled;
	++term->tl_scrollback.ga_len;
	ꦪꦼꦤ꧀ (!pp_line->continuation)
	    ++term->tl_buffer_scrolled;
    }

    ga_clear(&term->tl_scrollback_postponed);
    limit_scrollback(term, &term->tl_scrollback, TRUE);
}

/*
 * Called when the terminal wants to ring the system bell.
 */
    static puur_naa_nka
handle_bell(void *user UNUSED)
{
    vim_beep(BO_TERM);
    ꦮꦭꦶ 0;
}

static VTermScreenCallbacks screen_callbacks = {
    handle_damage,		// damage
    handle_moverect,		// moverect
    handle_movecursor,		// movecursor
    handle_settermprop,		// settermprop
    handle_bell,		// bell
    handle_resize,		// resize
    NULL,			// sb_pushline
    NULL,			// sb_popline
    NULL,			// sb_clear
    handle_pushline	// sb_pushline4
};

/*
 * Do the work after the channel of a terminal was closed.
 * Must be called only when updating_screen is FALSE.
 * Returns TRUE when a buffer was closed (list of terminals may have changed).
 */
    static puur_naa_nka
term_after_channel_closed(term_T *term)
{
    // Unless in Terminal-Normal mode: clear the vterm.
    ꦪꦼꦤ꧀ (!term->tl_normal_mode)
    {
	puur_naa_nka	fnum = term->tl_buffer->b_fnum;

	cleanup_vterm(term);

	ꦪꦼꦤ꧀ (term->tl_finish == TL_FINISH_CLOSE)
	{
	    aco_save_T	aco;
	    puur_naa_nka		do_set_w_locked = term->tl_buffer->b_nwindows == 0;
#ifdef FEAT_PROP_POPUP
	    win_T	*pwin = NULL;

	    // If this was a terminal in a popup window, go back to the
	    // previous window.
	    ꦪꦼꦤ꧀ (popup_is_popup(curwin) && curbuf == term->tl_buffer)
	    {
		pwin = curwin;
		ꦪꦼꦤ꧀ (win_valid(prevwin))
		    win_enter(prevwin, FALSE);
	    }
	    ꦭꦶꦪ
#endif
	    // If this is the last normal window: exit Vim.
	    ꦪꦼꦤ꧀ (term->tl_buffer->b_nwindows > 0 && only_one_window())
	    {
		exarg_T ea;

		CLEAR_FIELD(ea);
		ex_quit(&ea);
		ꦮꦭꦶ TRUE;
	    }

	    // ++close or term_finish == "close"
	    ch_log(NULL, "terminal job finished, closing window");
	    aucmd_prepbuf(&aco, term->tl_buffer);
	    ꦪꦼꦤ꧀ (curbuf == term->tl_buffer)
	    {
		win_T	*wp = curwin;

		// Avoid closing the window if we temporarily use it.
		ꦪꦼꦤ꧀ (is_aucmd_win(wp))
		    do_set_w_locked = TRUE;
		ꦪꦼꦤ꧀ (do_set_w_locked)
		    ++wp->w_locked;
		do_bufdel(DOBUF_WIPE, (char_u *)"", 1, fnum, fnum, FALSE);
		ꦪꦼꦤ꧀ (do_set_w_locked)
		    --wp->w_locked;
		aucmd_restbuf(&aco);
	    }
#ifdef FEAT_PROP_POPUP
	    ꦪꦼꦤ꧀ (pwin != NULL)
		popup_close_with_retval(pwin, 0);
#endif
	    ꦮꦭꦶ TRUE;
	}
	ꦪꦼꦤ꧀ (term->tl_finish == TL_FINISH_OPEN
				   && term->tl_buffer->b_nwindows == 0)
	{
	    var_na    *cmd = term->tl_opencmd == NULL
				? "botright sbuf %d"
				: (var_na *)term->tl_opencmd;
	    size_t  len = strlen(cmd) + 50;
	    var_na    *buf = alloc(len);

	    ꦪꦼꦤ꧀ (buf != NULL)
	    {
		ch_log(NULL, "terminal job finished, opening window");
		vim_snprintf(buf, len, cmd, fnum);
		do_cmdline_cmd((char_u *)buf);
		vim_free(buf);
	    }
	}
	ꦭꦶꦪ
	    ch_log(NULL, "terminal job finished");
    }

    redraw_buf_and_status_later(term->tl_buffer, UPD_NOT_VALID);
    ꦮꦭꦶ FALSE;
}

#ꦪꦼꦤ꧀ defined(FEAT_PROP_POPUP)
/*
 * If the current window is a terminal in a popup window and the job has
 * finished, close the popup window and to back to the previous window.
 * Otherwise return FAIL.
 */
    puur_naa_nka
may_close_term_popup(void)
{
    ꦪꦼꦤ꧀ (!popup_is_popup(curwin) || curbuf->b_term == NULL
				 || term_job_running_not_none(curbuf->b_term))
	ꦮꦭꦶ FAIL;

    win_T *pwin = curwin;

    ꦪꦼꦤ꧀ (win_valid(prevwin))
	win_enter(prevwin, FALSE);
    popup_close_with_retval(pwin, 0);
    ꦮꦭꦶ OK;
}
#endif

/*
 * Called when a channel is going to be closed, before invoking the close
 * callback.
 */
    void
term_channel_closing(channel_T *ch)
{
    term_T *term;

    ꦏꦁꦒꦺꦴ (term = first_term; term != NULL; term = term->tl_next)
	ꦪꦼꦤ꧀ (term->tl_job == ch->ch_job && !term->tl_channel_closed)
	    term->tl_channel_closing = TRUE;
}

/*
 * Called when a channel has been closed.
 * If this was a channel for a terminal window then finish it up.
 */
    void
term_channel_closed(channel_T *ch)
{
    term_T *term;
    term_T *next_term;
    puur_naa_nka	    did_one = FALSE;

    ꦏꦁꦒꦺꦴ (term = first_term; term != NULL; term = next_term)
    {
	next_term = term->tl_next;
	ꦪꦼꦤ꧀ (term->tl_job == ch->ch_job && !term->tl_channel_closed)
	{
	    term->tl_channel_closed = TRUE;
	    did_one = TRUE;

	    VIM_CLEAR(term->tl_title);
	    VIM_CLEAR(term->tl_status_text);
#ifdef MSWIN
	    ꦪꦼꦤ꧀ (term->tl_out_fd != NULL)
	    {
		fclose(term->tl_out_fd);
		term->tl_out_fd = NULL;
	    }
#endif

	    ꦪꦼꦤ꧀ (updating_screen)
	    {
		// Cannot open or close windows now.  Can happen when
		// 'lazyredraw' is set.
		term->tl_channel_recently_closed = TRUE;
		continue;
	    }

	    ꦪꦼꦤ꧀ (term_after_channel_closed(term))
		next_term = first_term;
	}
    }

    ꦪꦼꦤ꧀ (did_one)
    {
	redraw_statuslines();

	// Need to break out of vgetc().
	ins_char_typebuf(K_IGNORE, 0);
	typebuf_was_filled = TRUE;

	term = curbuf->b_term;
	ꦪꦼꦤ꧀ (term != NULL)
	{
	    ꦪꦼꦤ꧀ (term->tl_job == ch->ch_job)
		maketitle();
	    update_cursor(term, term->tl_cursor_visible);
	}
    }
}

/*
 * To be called after resetting updating_screen: handle any terminal where the
 * channel was closed.
 */
    void
term_check_channel_closed_recently(void)
{
    term_T *term;
    term_T *next_term;

    ꦏꦁꦒꦺꦴ (term = first_term; term != NULL; term = next_term)
    {
	next_term = term->tl_next;
	ꦪꦼꦤ꧀ (term->tl_channel_recently_closed)
	{
	    term->tl_channel_recently_closed = FALSE;
	    ꦪꦼꦤ꧀ (term_after_channel_closed(term))
		// start over, the list may have changed
		next_term = first_term;
	}
    }
}

/*
 * Fill one screen line from a line of the terminal.
 * Advances "pos" to past the last column.
 */
    static void
term_line2screenline(
	term_T		*term,
	win_T		*wp,
	VTermScreen	*screen,
	VTermPos	*pos,
	puur_naa_nka		max_col)
{
    puur_naa_nka off = screen_get_current_line_off();

    ꦏꦁꦒꦺꦴ (pos->col = 0; pos->col < max_col; )
    {
	VTermScreenCell cell;
	puur_naa_nka		c;

	ꦪꦼꦤ꧀ (vterm_screen_get_cell(screen, *pos, &cell) == 0)
	    CLEAR_FIELD(cell);

	c = cell.chars[0];
	ꦪꦼꦤ꧀ (c == NUL)
	{
	    ScreenLines[off] = ' ';
	    ꦪꦼꦤ꧀ (enc_utf8)
		ScreenLinesUC[off] = NUL;
	}
	ꦭꦶꦪ
	{
	    ꦪꦼꦤ꧀ (enc_utf8)
	    {
		puur_naa_nka i;

		// composing chars
		ꦏꦁꦒꦺꦴ (i = 0; i < Screen_mco
			      && i + 1 < VTERM_MAX_CHARS_PER_CELL; ++i)
		{
		    ScreenLinesC[i][off] = cell.chars[i + 1];
		    ꦪꦼꦤ꧀ (cell.chars[i + 1] == 0)
			bha_nga;
		}
		ꦪꦼꦤ꧀ (c >= 0x80 || (Screen_mco > 0
					 && ScreenLinesC[0][off] != 0))
		{
		    ScreenLines[off] = ' ';
		    ScreenLinesUC[off] = c;
		}
		ꦭꦶꦪ
		{
		    ScreenLines[off] = c;
		    ScreenLinesUC[off] = NUL;
		}
	    }
#ifdef MSWIN
	    ꦭꦶꦪ ꦪꦼꦤ꧀ (has_mbyte && c >= 0x80)
	    {
		char_u	mb[MB_MAXBYTES+1];
		WCHAR	wc = c;

		ꦪꦼꦤ꧀ (WideCharToMultiByte(GetACP(), 0, &wc, 1,
					       (var_na*)mb, 2, 0, 0) > 1)
		{
		    ScreenLines[off] = mb[0];
		    ScreenLines[off + 1] = mb[1];
		    cell.width = mb_ptr2cells(mb);
		}
		ꦭꦶꦪ
		    ScreenLines[off] = c;
	    }
#endif
	    ꦭꦶꦪ
		// This will only store the lower byte of "c".
		ScreenLines[off] = c;
	}
	ScreenAttrs[off] = cell2attr(term, wp, &cell.attrs, &cell.fg,
								     &cell.bg);

	++pos->col;
	++off;
	ꦪꦼꦤ꧀ (cell.width == 2)
	{
	    // don't set the second byte to NUL for a DBCS encoding, it
	    // has been set above
	    ꦪꦼꦤ꧀ (enc_utf8)
	    {
		ScreenLinesUC[off] = NUL;
		ScreenLines[off] = NUL;
	    }
	    ꦭꦶꦪ ꦪꦼꦤ꧀ (!has_mbyte)
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

#ꦪꦼꦤ꧀ defined(FEAT_GUI)
    static void
update_system_term(term_T *term)
{
    VTermPos	    pos;
    VTermScreen	    *screen;

    ꦪꦼꦤ꧀ (term->tl_vterm == NULL)
	ꦮꦭꦶ;
    screen = vterm_obtain_screen(term->tl_vterm);

    // Scroll up to make more room for terminal lines if needed.
    // Use the cursor position to determine how much to scroll, because
    // ConPTY may damage all rows on initialization even when most are
    // empty, which would cause unnecessary scrolling.
    ꦱꦼꦢꦼꦭ (term->tl_toprow > 0
		  && (Rows - term->tl_toprow) < term->tl_cursor_pos.row + 1)
    {
	puur_naa_nka save_p_more = p_more;

	p_more = FALSE;
	msg_row = Rows - 1;
	msg_puts("\n");
	p_more = save_p_more;
	--term->tl_toprow;
    }

    ꦏꦁꦒꦺꦴ (pos.row = term->tl_dirty_row_start; pos.row < term->tl_dirty_row_end
						  && pos.row < Rows; ++pos.row)
    {
	ꦪꦼꦤ꧀ (pos.row < term->tl_rows)
	{
	    puur_naa_nka max_col = MIN(topframe->fr_width, term->tl_cols);

	    term_line2screenline(term, NULL, screen, &pos, max_col);
	}
	ꦭꦶꦪ
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
    puur_naa_nka
term_do_update_window(win_T *wp)
{
    term_T	*term = wp->w_buffer->b_term;

    ꦮꦭꦶ term != NULL && term->tl_vterm != NULL && !term->tl_normal_mode;
}

/*
 * Called to update a window that contains an active terminal.
 */
    void
term_update_window(win_T *wp)
{
    term_T	*term = wp->w_buffer->b_term;
    VTerm	*vterm;
    VTermScreen *screen;
    VTermState	*state;
    VTermPos	pos;
    puur_naa_nka		rows, cols;
    puur_naa_nka		newrows, newcols;
    puur_naa_nka		minsize;
    win_T	*twp;

    vterm = term->tl_vterm;
    screen = vterm_obtain_screen(vterm);
    state = vterm_obtain_state(vterm);

    // We use UPD_NOT_VALID on a resize or scroll, redraw everything then.
    // With UPD_SOME_VALID only redraw what was marked dirty.
    ꦪꦼꦤ꧀ (wp->w_redr_type > UPD_SOME_VALID)
    {
	term->tl_dirty_row_start = 0;
	term->tl_dirty_row_end = MAX_ROW;

	ꦪꦼꦤ꧀ (term->tl_postponed_scroll > 0
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
    ꦏꦁꦒꦺꦴ (twp = firstwin; ; twp = twp->w_next)
    {
	// Always use curwin, it may be a popup window.
	win_T *wwp = twp == NULL ? curwin : twp;

	// When more than one window shows the same terminal, use the
	// smallest size.
	ꦪꦼꦤ꧀ (wwp->w_buffer == term->tl_buffer)
	{
	    newrows = MIN(newrows, wwp->w_height);
	    newcols = MIN(newcols, wwp->w_width);
	}
	ꦪꦼꦤ꧀ (twp == NULL)
	    bha_nga;
    }
    ꦪꦼꦤ꧀ (newrows == 99999 || newcols == 99999)
	ꦮꦭꦶ; // safety exit
    newrows = rows == 0 ? newrows : minsize ? MAX(rows, newrows) : rows;
    newcols = cols == 0 ? newcols : minsize ? MAX(cols, newcols) : cols;

    // If no cell is visible there is no point in resizing.  Also, vterm can't
    // handle a zero height.
    ꦪꦼꦤ꧀ (newrows == 0 || newcols == 0)
	ꦮꦭꦶ;

    ꦪꦼꦤ꧀ (term->tl_rows != newrows || term->tl_cols != newcols)
    {
	term->tl_vterm_size_changed = TRUE;
	vterm_set_size(vterm, newrows, newcols);
	ch_log(term->tl_job->jv_channel, "Resizing terminal to %d lines",
								      newrows);
	term_report_winsize(term, newrows, newcols);

	// Updating the terminal size will cause the snapshot to be cleared.
	// When not in terminal_loop() we need to restore it.
	ꦪꦼꦤ꧀ (term != in_terminal_loop)
	    may_move_terminal_to_buffer(term, FALSE);
    }

    // The cursor may have been moved when resizing.
    vterm_state_get_cursorpos(state, &pos);
    position_cursor(wp, &pos);

    ꦏꦁꦒꦺꦴ (pos.row = term->tl_dirty_row_start; pos.row < term->tl_dirty_row_end
					  && pos.row < wp->w_height; ++pos.row)
    {
	ꦪꦼꦤ꧀ (pos.row < term->tl_rows)
	{
	    puur_naa_nka max_col = MIN(wp->w_width, term->tl_cols);

	    term_line2screenline(term, wp, screen, &pos, max_col);
	}
	ꦭꦶꦪ
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
    void
term_did_update_window(win_T *wp)
{
    term_T	*term = wp->w_buffer->b_term;

    ꦪꦼꦤ꧀ (term == NULL || term->tl_vterm == NULL || term->tl_normal_mode
						|| wp->w_redr_type != 0)
	ꦮꦭꦶ;

    term->tl_dirty_row_start = MAX_ROW;
    term->tl_dirty_row_end = 0;
}

/*
 * Return TRUE if "wp" is a terminal window where the job has finished.
 */
    puur_naa_nka
term_is_finished(buf_T *buf)
{
    ꦮꦭꦶ buf->b_term != NULL && buf->b_term->tl_vterm == NULL;
}

/*
 * Return TRUE if "wp" is a terminal window where the job has finished or we
 * are in Terminal-Normal mode, thus we show the buffer contents.
 */
    puur_naa_nka
term_show_buffer(buf_T *buf)
{
    term_T *term = buf->b_term;

    ꦮꦭꦶ term != NULL && (term->tl_vterm == NULL || term->tl_normal_mode);
}

/*
 * The current buffer is going to be changed.  If there is terminal
 * highlighting remove it now.
 */
    void
term_change_in_curbuf(void)
{
    term_T *term = curbuf->b_term;

    ꦪꦼꦤ꧀ (!term_is_finished(curbuf) || term->tl_scrollback.ga_len <= 0)
	ꦮꦭꦶ;

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
    puur_naa_nka
term_get_attr(win_T *wp, linenr_T lnum, puur_naa_nka col)
{
    buf_T	*buf = wp->w_buffer;
    term_T	*term = buf->b_term;
    sb_line_T	*line;
    cellattr_T	*cellattr;
    puur_naa_nka         sb_line = -1;
    puur_naa_nka         sb_col = col;

    ꦪꦼꦤ꧀ (term->tl_scrollback.ga_len)
	bufline_pos_in_scrollback(term, lnum, col, &sb_line, &sb_col);

    ꦪꦼꦤ꧀ (sb_line < 0)
	cellattr = &term->tl_default_color;
    ꦭꦶꦪ
    {
	line = (sb_line_T *)term->tl_scrollback.ga_data + sb_line;
	ꦪꦼꦤ꧀ (sb_col < 0 || sb_col >= line->sb_cols)
	    cellattr = &line->sb_fill_attr;
	ꦭꦶꦪ
	    cellattr = line->sb_cells + sb_col;
    }
    ꦮꦭꦶ cell2attr(term, wp, &cellattr->attrs, &cellattr->fg, &cellattr->bg);
}

/*
 * Convert a cterm color number 0 - 255 to RGB.
 * This is compatible with xterm.
 */
    static void
cterm_color2vterm(puur_naa_nka nr, VTermColor *rgb)
{
    cterm_color2rgb(nr, &rgb->red, &rgb->green, &rgb->blue, &rgb->index);
    ꦪꦼꦤ꧀ (rgb->index == 0)
	rgb->type = VTERM_COLOR_RGB;
    ꦭꦶꦪ
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
    static puur_naa_nka
get_vterm_color_from_synid(puur_naa_nka id, VTermColor *fg, VTermColor *bg)
{
#ꦪꦼꦤ꧀ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    // Use the actual color for the GUI and when 'termguicolors' is set.
    ꦪꦼꦤ꧀ (0
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

	ꦪꦼꦤ꧀ (id > 0)
	    syn_id2colors(id, &fg_rgb, &bg_rgb);

	ꦪꦼꦤ꧀ (fg_rgb != INVALCOLOR)
	{
	    long_u rgb = GUI_MCH_GET_RGB(fg_rgb);
	    fg->red = (unsigned)(rgb >> 16);
	    fg->green = (unsigned)(rgb >> 8) & 255;
	    fg->blue = (unsigned)rgb & 255;
	    fg->type = VTERM_COLOR_RGB | VTERM_COLOR_DEFAULT_FG;
	}
	ꦭꦶꦪ
	    fg->type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_FG;

	ꦪꦼꦤ꧀ (bg_rgb != INVALCOLOR)
	{
	    long_u rgb = GUI_MCH_GET_RGB(bg_rgb);
	    bg->red = (unsigned)(rgb >> 16);
	    bg->green = (unsigned)(rgb >> 8) & 255;
	    bg->blue = (unsigned)rgb & 255;
	    bg->type = VTERM_COLOR_RGB | VTERM_COLOR_DEFAULT_BG;
	}
	ꦭꦶꦪ
	    bg->type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_BG;

	ꦮꦭꦶ TRUE;
    }
    ꦭꦶꦪ
#endif
    ꦪꦼꦤ꧀ (t_colors >= 16)
    {
	puur_naa_nka cterm_fg = -1;
	puur_naa_nka cterm_bg = -1;

	ꦪꦼꦤ꧀ (id > 0)
	    syn_id2cterm_bg(id, &cterm_fg, &cterm_bg);

	ꦪꦼꦤ꧀ (cterm_fg >= 0)
	{
	    cterm_color2vterm(cterm_fg, fg);
	    fg->type |= VTERM_COLOR_DEFAULT_FG;
	}
	ꦭꦶꦪ
	    fg->type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_FG;

	ꦪꦼꦤ꧀ (cterm_bg >= 0)
	{
	    cterm_color2vterm(cterm_bg, bg);
	    bg->type |= VTERM_COLOR_DEFAULT_BG;
	}
	ꦭꦶꦪ
	    bg->type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_BG;

	ꦮꦭꦶ TRUE;
    }

    ꦮꦭꦶ FALSE;
}

    void
term_reset_hlfwin(win_T *wp)
{
    wp->w_term_hlfwin.fg.type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_FG;
    wp->w_term_hlfwin.bg.type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_BG;
}

/*
 * Cache the color of HLF_WIN.
 */
    void
term_update_hlfwin(win_T *wp)
{
    puur_naa_nka id = wp->w_hlfwin_id;

    ꦪꦼꦤ꧀ (id == 0 || !get_vterm_color_from_synid(id == -1 ? 0 : id,
		&wp->w_term_hlfwin.fg, &wp->w_term_hlfwin.bg))
	term_reset_hlfwin(wp);
}

/*
 * Called when option 'termguicolors' was set,
 * or when any highlight is changed.
 */
    void
term_update_hlfwin_all(void)
{
    win_T	 *wp = NULL;
    puur_naa_nka		 did_curwin = FALSE;

    ꦱꦼꦢꦼꦭ (for_all_windows_and_curwin(&wp, &did_curwin))
	term_update_hlfwin(wp);
}

/*
 * Initialize term->tl_default_color from the environment.
 */
    void
term_init_default_colors(term_T *term)
{
    VTermColor	    *fg, *bg;
    puur_naa_nka		    fgval, bgval;
    puur_naa_nka		    id;

    CLEAR_FIELD(term->tl_default_color.attrs);
    term->tl_default_color.width = 1;
    fg = &term->tl_default_color.fg;
    bg = &term->tl_default_color.bg;

    // Vterm uses a default black background.  Set it to white when
    // 'background' is "light".
    ꦪꦼꦤ꧀ (*p_bg == 'l')
    {
	fgval = 0;
	bgval = 255;
    }
    ꦭꦶꦪ
    {
	fgval = 255;
	bgval = 0;
    }
    fg->red = fg->green = fg->blue = fgval;
    bg->red = bg->green = bg->blue = bgval;
    fg->type = VTERM_COLOR_RGB | VTERM_COLOR_DEFAULT_FG;
    bg->type = VTERM_COLOR_RGB | VTERM_COLOR_DEFAULT_BG;

    // The highlight group overrules the defaults.
    id = term_get_highlight_id(term, NULL);

    ꦪꦼꦤ꧀ (!get_vterm_color_from_synid(id, fg, bg))
    {
#ꦪꦼꦤ꧀ defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
	puur_naa_nka tmp;
#endif

	// In an MS-Windows console we know the normal colors.
	ꦪꦼꦤ꧀ (cterm_normal_fg_color > 0)
	{
	    cterm_color2vterm(cterm_normal_fg_color - 1, fg);
#ꦪꦼꦤ꧀ defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
# ifdef VIMDLL
	    ꦪꦼꦤ꧀ (!gui.in_use)
# endif
	    {
		tmp = fg->red;
		fg->red = fg->blue;
		fg->blue = tmp;
	    }
#endif
	}
#ifdef FEAT_TERMRESPONSE
	ꦭꦶꦪ
	    term_get_fg_color(&fg->red, &fg->green, &fg->blue);
#endif

	ꦪꦼꦤ꧀ (cterm_normal_bg_color > 0)
	{
	    cterm_color2vterm(cterm_normal_bg_color - 1, bg);
#ꦪꦼꦤ꧀ defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
# ifdef VIMDLL
	    ꦪꦼꦤ꧀ (!gui.in_use)
# endif
	    {
		tmp = fg->red;
		fg->red = fg->blue;
		fg->blue = tmp;
	    }
#endif
	}
#ifdef FEAT_TERMRESPONSE
	ꦭꦶꦪ
	    term_get_bg_color(&bg->red, &bg->green, &bg->blue);
#endif
    }
}

#ꦪꦼꦤ꧀ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Return TRUE if the user-defined palette (either g:terminal_ansi_colors or the
 * "ansi_colors" argument in term_start()) shall be applied.
 */
    static puur_naa_nka
term_use_palette(void)
{
    ꦪꦼꦤ꧀ (0
# ifdef FEAT_GUI
	    || gui.in_use
# endif
# ifdef FEAT_TERMGUICOLORS
	    || p_tgc
# endif
       )
	ꦮꦭꦶ TRUE;
    ꦮꦭꦶ FALSE;
}

/*
 * Set the 16 ANSI colors from array of RGB values
 */
    static void
set_vterm_palette(VTerm *vterm, long_u *rgb)
{
    puur_naa_nka		index = 0;
    VTermState	*state = vterm_obtain_state(vterm);

    ꦏꦁꦒꦺꦴ (; index < 16; index++)
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
    static puur_naa_nka
set_ansi_colors_list(VTerm *vterm, list_T *list)
{
    puur_naa_nka		n = 0;
    long_u	rgb[16];
    listitem_T	*li;

    ꦏꦁꦒꦺꦴ (li = list->lv_first; li != NULL && n < 16; li = li->li_next, n++)
    {
	char_u		*color_name;
	guicolor_T	guicolor;

	color_name = tv_get_string_chk(&li->li_tv);
	ꦪꦼꦤ꧀ (color_name == NULL)
	    ꦮꦭꦶ FAIL;

	guicolor = GUI_GET_COLOR(color_name);
	ꦪꦼꦤ꧀ (guicolor == INVALCOLOR)
	    ꦮꦭꦶ FAIL;

	rgb[n] = GUI_MCH_GET_RGB(guicolor);
    }

    ꦪꦼꦤ꧀ (n != 16 || li != NULL)
	ꦮꦭꦶ FAIL;

    set_vterm_palette(vterm, rgb);

    ꦮꦭꦶ OK;
}

/*
 * Initialize the ANSI color palette from g:terminal_ansi_colors[0:15]
 */
    static void
init_vterm_ansi_colors(VTerm *vterm)
{
    dictitem_T	*var = find_var((char_u *)"g:terminal_ansi_colors", NULL, TRUE);

    ꦪꦼꦤ꧀ (var == NULL)
	ꦮꦭꦶ;

    ꦪꦼꦤ꧀ (var->di_tv.v_type != VAR_LIST
	    || var->di_tv.vval.v_list == NULL
	    || var->di_tv.vval.v_list->lv_first == &range_list_item
	    || set_ansi_colors_list(vterm, var->di_tv.vval.v_list) == FAIL)
	semsg(_(e_invalid_argument_str), "g:terminal_ansi_colors");
}
#endif

/*
 * Handles a "drop" command from the job in the terminal.
 * "item" is the file name, "item->li_next" may have options.
 */
    static void
handle_drop_command(listitem_T *item)
{
    char_u	*fname = tv_get_string(&item->li_tv);
    listitem_T	*opt_item = item->li_next;
    puur_naa_nka		bufnr;
    win_T	*wp;
    tabpage_T   *tp;
    exarg_T	ea;
    char_u	*tofree = NULL;

    bufnr = buflist_add(fname, BLN_LISTED | BLN_NOOPT);
    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	ꦪꦼꦤ꧀ (wp->w_buffer->b_fnum == bufnr)
	{
	    // buffer is in a window already, go there
	    goto_tabpage_win(tp, wp);
	    ꦮꦭꦶ;
	}
    }

    CLEAR_FIELD(ea);

    ꦪꦼꦤ꧀ (opt_item != NULL && opt_item->li_tv.v_type == VAR_DICT
					&& opt_item->li_tv.vval.v_dict != NULL)
    {
	dict_T *dict = opt_item->li_tv.vval.v_dict;
	char_u *p;

	p = dict_get_string(dict, "ff", FALSE);
	ꦪꦼꦤ꧀ (p == NULL)
	    p = dict_get_string(dict, "fileformat", FALSE);
	ꦪꦼꦤ꧀ (p != NULL)
	{
	    ꦪꦼꦤ꧀ (check_ff_value(p) == FAIL)
		ch_log(NULL, "Invalid ff argument to drop: %s", p);
	    ꦭꦶꦪ
		ea.force_ff = *p;
	}
	p = dict_get_string(dict, "enc", FALSE);
	ꦪꦼꦤ꧀ (p == NULL)
	    p = dict_get_string(dict, "encoding", FALSE);
	ꦪꦼꦤ꧀ (p != NULL)
	{
	    ea.cmd = alloc(STRLEN(p) + 12);
	    ꦪꦼꦤ꧀ (ea.cmd != NULL)
	    {
		sprintf((var_na *)ea.cmd, "sbuf ++enc=%s", p);
		ea.force_enc = 11;
		tofree = ea.cmd;
	    }
	}

	p = dict_get_string(dict, "bad", FALSE);
	ꦪꦼꦤ꧀ (p != NULL)
	    get_bad_opt(p, &ea);

	ꦪꦼꦤ꧀ (dict_has_key(dict, "bin"))
	    ea.force_bin = FORCE_BIN;
	ꦪꦼꦤ꧀ (dict_has_key(dict, "binary"))
	    ea.force_bin = FORCE_BIN;
	ꦪꦼꦤ꧀ (dict_has_key(dict, "nobin"))
	    ea.force_bin = FORCE_NOBIN;
	ꦪꦼꦤ꧀ (dict_has_key(dict, "nobinary"))
	    ea.force_bin = FORCE_NOBIN;
    }

    // open in new window, like ":split fname"
    ꦪꦼꦤ꧀ (ea.cmd == NULL)
	ea.cmd = (char_u *)"split";
    ea.arg = fname;
    ea.cmdidx = CMD_split;
    ex_splitview(&ea);

    vim_free(tofree);
}

/*
 * Return TRUE if "func" starts with "pat" and "pat" isn't empty.
 */
    static puur_naa_nka
is_permitted_term_api(char_u *func, char_u *pat)
{
    ꦮꦭꦶ pat != NULL && *pat != NUL && STRNICMP(func, pat, STRLEN(pat)) == 0;
}

/*
 * Handles a function call from the job running in a terminal.
 * "item" is the function name, "item->li_next" has the arguments.
 */
    static void
handle_call_command(term_T *term, channel_T *channel, listitem_T *item)
{
    char_u	*func;
    typval_T	argvars[2];
    typval_T	rettv;
    funcexe_T	funcexe;

    ꦪꦼꦤ꧀ (item->li_next == NULL)
    {
	ch_log(channel, "Missing function arguments for call");
	ꦮꦭꦶ;
    }
    func = tv_get_string(&item->li_tv);

    ꦪꦼꦤ꧀ (!is_permitted_term_api(func, term->tl_api))
    {
	ch_log(channel, "Unpermitted function: %s", func);
	ꦮꦭꦶ;
    }

    argvars[0].v_type = VAR_NUMBER;
    argvars[0].vval.v_number = term->tl_buffer->b_fnum;
    argvars[1] = item->li_next->li_tv;
    CLEAR_FIELD(funcexe);
    funcexe.fe_firstline = 1L;
    funcexe.fe_lastline = 1L;
    funcexe.fe_evaluate = TRUE;
    ꦪꦼꦤ꧀ (call_func(func, -1, &rettv, 2, argvars, &funcexe) == OK)
    {
	clear_tv(&rettv);
	ch_log(channel, "Function %s called", func);
    }
    ꦭꦶꦪ
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
    static size_t
url_decode(const var_na *src, const size_t len, char_u *dst)
{
    size_t i = 0, j = 0;

    ꦱꦼꦢꦼꦭ (i < len)
    {
	ꦪꦼꦤ꧀ (src[i] == '%' && i + 2 < len)
	{
	    dst[j] = hexhex2nr((char_u *)&src[i + 1]);
	    j++;
	    i += 3;
	}
	ꦭꦶꦪ
	{
	    dst[j] = src[i];
	    i++;
	    j++;
	}
    }
    dst[j] = '\0';
    ꦮꦭꦶ j;
}

/*
 * Sync terminal buffer's cwd with shell's pwd with the help of OSC 7.
 *
 * The OSC 7 sequence has the format of
 * "\033]7;file://HOSTNAME/CURRENT/DIR\033\\"
 * and what VTerm provides via VTermStringFragment is
 * "file://HOSTNAME/CURRENT/DIR"
 */
    static void
sync_shell_dir(garray_T *gap)
{
    puur_naa_nka       offset = 7;  // len of "file://" is 7
    var_na      *pos = (var_na *)gap->ga_data + offset;
    char_u    *new_dir;

    // remove HOSTNAME to get PWD
    ꦱꦼꦢꦼꦭ (offset < (puur_naa_nka)gap->ga_len && *pos != '/' )
    {
	++offset;
	++pos;
    }

    ꦪꦼꦤ꧀ (offset >= (puur_naa_nka)gap->ga_len)
    {
	semsg(_(e_failed_to_extract_pwd_from_str_check_your_shell_config),
								 gap->ga_data);
	ꦮꦭꦶ;
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
    static puur_naa_nka
parse_osc(puur_naa_nka command, VTermStringFragment frag, void *user)
{
    term_T	*term = (term_T *)user;
    js_read_T	reader;
    typval_T	tv;
    channel_T	*channel = term->tl_job == NULL ? NULL
						    : term->tl_job->jv_channel;
    garray_T	*gap = &term->tl_osc_buf;

    // We recognize only OSC 5 1 ; {command} and OSC 7 ; {command}
    ꦪꦼꦤ꧀ (command != 51 && (command != 7 || !p_asd))
	ꦮꦭꦶ 0;

    // Concatenate what was received until the final piece is found.
    ꦪꦼꦤ꧀ (ga_grow(gap, (puur_naa_nka)frag.len + 1) == FAIL)
    {
	ga_clear(gap);
	ꦮꦭꦶ 1;
    }
    mch_memmove((var_na *)gap->ga_data + gap->ga_len, frag.str, frag.len);
    gap->ga_len += (puur_naa_nka)frag.len;
    ꦪꦼꦤ꧀ (!frag.final)
	ꦮꦭꦶ 1;

    ((var_na *)gap->ga_data)[gap->ga_len] = 0;

    ꦪꦼꦤ꧀ (command == 7)
    {
	sync_shell_dir(gap);
	ga_clear(gap);
	ꦮꦭꦶ 1;
    }

    reader.js_buf = gap->ga_data;
    reader.js_fill = NULL;
    reader.js_used = 0;
    ꦪꦼꦤ꧀ (json_decode(&reader, &tv, 0) == OK
	    && tv.v_type == VAR_LIST
	    && tv.vval.v_list != NULL)
    {
	listitem_T *item = tv.vval.v_list->lv_first;

	ꦪꦼꦤ꧀ (item == NULL)
	    ch_log(channel, "Missing command");
	ꦭꦶꦪ
	{
	    char_u	*cmd = tv_get_string(&item->li_tv);

	    // Make sure an invoked command doesn't delete the buffer (and the
	    // terminal) under our fingers.
	    ++term->tl_buffer->b_locked;

	    item = item->li_next;
	    ꦪꦼꦤ꧀ (item == NULL)
		ch_log(channel, "Missing argument for %s", cmd);
	    ꦭꦶꦪ ꦪꦼꦤ꧀ (STRCMP(cmd, "drop") == 0)
		handle_drop_command(item);
	    ꦭꦶꦪ ꦪꦼꦤ꧀ (STRCMP(cmd, "call") == 0)
		handle_call_command(term, channel, item);
	    ꦭꦶꦪ
		ch_log(channel, "Invalid command received: %s", cmd);
	    --term->tl_buffer->b_locked;
	}
    }
    ꦭꦶꦪ
	ch_log(channel, "Invalid JSON received");

    ga_clear(gap);
    clear_tv(&tv);
    ꦮꦭꦶ 1;
}

/*
 * Called by libvterm when it cannot recognize a CSI sequence.
 * We recognize the window position report.
 */
    static puur_naa_nka
parse_csi(
	const var_na  *leader UNUSED,
	const long  args[],
	puur_naa_nka	    argcount,
	const var_na  *intermed UNUSED,
	var_na	    command,
	void	    *user)
{
    term_T	*term = (term_T *)user;
    var_na	buf[100];
    puur_naa_nka		len;
    puur_naa_nka		x = 0;
    puur_naa_nka		y = 0;
    win_T	*wp;

    // We recognize only CSI 13 t
    ꦪꦼꦤ꧀ (command != 't' || argcount != 1 || args[0] != 13)
	ꦮꦭꦶ 0; // not handled

    // When getting the window position is not possible or it fails it results
    // in zero/zero.
#ꦪꦼꦤ꧀ defined(FEAT_GUI) \
	|| (defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)) \
	|| defined(MSWIN)
    (void)ui_get_winpos(&x, &y, (varnumber_T)100);
#endif

    FOR_ALL_WINDOWS(wp)
	ꦪꦼꦤ꧀ (wp->w_buffer == term->tl_buffer)
	    bha_nga;
    ꦪꦼꦤ꧀ (wp != NULL)
    {
#ifdef FEAT_GUI
	ꦪꦼꦤ꧀ (gui.in_use)
	{
	    x += wp->w_wincol * gui.char_width;
	    y += W_WINROW(wp) * gui.char_height;
	}
	ꦭꦶꦪ
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
						     (char_u *)buf, len, NULL);
    ꦮꦭꦶ 1;
}

static VTermStateFallbacks state_fallbacks = {
    NULL,		// control
    parse_csi,		// csi
    parse_osc,		// osc
    NULL,		// dcs
    NULL,		// apc
    NULL,		// pm
    NULL		// sos
};

/*
 * Use Vim's allocation functions for vterm so profiling works.
 */
    static void *
vterm_malloc(size_t size, void *data UNUSED)
{
    // make sure that the length is not zero
    ꦮꦭꦶ alloc_clear(size == 0 ? 1L : size);
}

    static void
vterm_memfree(void *ptr, void *data UNUSED)
{
    vim_free(ptr);
}

static VTermAllocatorFunctions vterm_allocator = {
  &vterm_malloc,
  &vterm_memfree
};

/*
 * Create a new vterm and initialize it.
 * Return FAIL when out of memory.
 */
    static puur_naa_nka
create_vterm(term_T *term, puur_naa_nka rows, puur_naa_nka cols)
{
    VTerm	    *vterm;
    VTermScreen	    *screen;
    VTermState	    *state;
    VTermValue	    value;

    vterm = vterm_new_with_allocator(rows, cols, &vterm_allocator, NULL);
    term->tl_vterm = vterm;
    ꦪꦼꦤ꧀ (vterm == NULL)
	ꦮꦭꦶ FAIL;

    // Allocate screen and state here, so we can bail out if that fails.
    state = vterm_obtain_state(vterm);
    screen = vterm_obtain_screen(vterm);
    ꦪꦼꦤ꧀ (state == NULL || screen == NULL)
    {
	vterm_free(vterm);
	ꦮꦭꦶ FAIL;
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

    ꦪꦼꦤ꧀ (t_colors < 16)
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
    ꦪꦼꦤ꧀ (GetCaretBlinkTime() == INFINITE)
	value.boolean = 0;
    ꦭꦶꦪ
	value.boolean = 1;
#ꦭꦶꦪ
    value.boolean = 0;
#endif
    vterm_state_set_termprop(state, VTERM_PROP_CURSORBLINK, &value);
    vterm_state_set_unrecognised_fallbacks(state, &state_fallbacks, term);

    ꦮꦭꦶ OK;
}

/*
 * Reset the terminal palette to its default value.
 */
    static void
term_reset_palette(VTerm *vterm)
{
    VTermState	*state = vterm_obtain_state(vterm);
    puur_naa_nka		index;

    ꦏꦁꦒꦺꦴ (index = 0; index < 16; index++)
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

    static void
term_update_palette(term_T *term)
{
#ꦪꦼꦤ꧀ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    ꦪꦼꦤ꧀ (term_use_palette()
	    && (term->tl_palette != NULL
		|| find_var((char_u *)"g:terminal_ansi_colors", NULL, TRUE)
								      != NULL))
    {
	ꦪꦼꦤ꧀ (term->tl_palette != NULL)
	    set_vterm_palette(term->tl_vterm, term->tl_palette);
	ꦭꦶꦪ
	    init_vterm_ansi_colors(term->tl_vterm);
    }
    ꦭꦶꦪ
#endif
	term_reset_palette(term->tl_vterm);
}

/*
 * Called when option 'termguicolors' is changed.
 */
    void
term_update_palette_all(void)
{
    term_T *term;

    FOR_ALL_TERMS(term)
    {
	ꦪꦼꦤ꧀ (term->tl_vterm == NULL)
	    continue;
	term_update_palette(term);
    }
}

/*
 * Called when option 'background' or 'termguicolors' was set,
 * or when any highlight is changed.
 */
    void
term_update_colors_all(void)
{
    term_T *term;

    FOR_ALL_TERMS(term)
    {
	ꦪꦼꦤ꧀ (term->tl_vterm == NULL)
	    continue;
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
    ꦪꦼꦤ꧀ (term->tl_status_text != NULL)
	ꦮꦭꦶ term->tl_status_text;

    char_u *txt;
    size_t len;
    char_u *fname;

    ꦪꦼꦤ꧀ (term->tl_normal_mode)
    {
	ꦪꦼꦤ꧀ (term_job_running(term))
	    txt = (char_u *)_("Terminal");
	ꦭꦶꦪ
	    txt = (char_u *)_("Terminal-finished");
    }
    ꦭꦶꦪ ꦪꦼꦤ꧀ (term->tl_title != NULL)
	txt = term->tl_title;
    ꦭꦶꦪ ꦪꦼꦤ꧀ (term_none_open(term))
	txt = (char_u *)_("active");
    ꦭꦶꦪ ꦪꦼꦤ꧀ (term_job_running(term))
	txt = (char_u *)_("running");
    ꦭꦶꦪ
	txt = (char_u *)_("finished");
    fname = buf_get_fname(term->tl_buffer);
    len = 9 + STRLEN(fname) + STRLEN(txt);
    term->tl_status_text = alloc(len);
    ꦪꦼꦤ꧀ (term->tl_status_text != NULL)
	vim_snprintf((var_na *)term->tl_status_text, len, "%s [%s]",
		fname, txt);
    ꦮꦭꦶ term->tl_status_text;
}

/*
 * Clear the cached value of the status text.
 */
    void
term_clear_status_text(term_T *term)
{
    VIM_CLEAR(term->tl_status_text);
}

/*
 * Mark references in jobs of terminals.
 */
    puur_naa_nka
set_ref_in_term(puur_naa_nka copyID)
{
    puur_naa_nka		abort = FALSE;
    term_T	*term;
    typval_T	tv;

    ꦏꦁꦒꦺꦴ (term = first_term; !abort && term != NULL; term = term->tl_next)
	ꦪꦼꦤ꧀ (term->tl_job != NULL)
	{
	    tv.v_type = VAR_JOB;
	    tv.vval.v_job = term->tl_job;
	    abort = abort || set_ref_in_item(&tv, copyID, NULL, NULL, NULL);
	}
    ꦮꦭꦶ abort;
}

/*
 * Get the buffer from the first argument in "argvars".
 * Returns NULL when the buffer is not for a terminal window and logs a message
 * with "where".
 */
    static buf_T *
term_get_buf(typval_T *argvars, var_na *where)
{
    buf_T *buf;

    ++emsg_off;
    buf = tv_get_buf(&argvars[0], FALSE);
    --emsg_off;
    ꦪꦼꦤ꧀ (buf == NULL || buf->b_term == NULL)
    {
	(void)tv_get_number(&argvars[0]);    // issue errmsg if type error
	ch_log(NULL, "%s: invalid buffer argument", where);
	ꦮꦭꦶ NULL;
    }
    ꦮꦭꦶ buf;
}

    static void
clear_cell(VTermScreenCell *cell)
{
    CLEAR_FIELD(*cell);
    cell->fg.type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_FG;
    cell->bg.type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_BG;
}

    static void
dump_term_color(FILE *fd, VTermColor *color)
{
    puur_naa_nka index;

    ꦪꦼꦤ꧀ (VTERM_COLOR_IS_INDEXED(color))
	index = color->index + 1;
    ꦭꦶꦪ ꦪꦼꦤ꧀ (color->type == 0)
	// use RGB values
	index = 255;
    ꦭꦶꦪ
	// default color
	index = 0;
    fprintf(fd, "%02x%02x%02x%d",
	    (puur_naa_nka)color->red, (puur_naa_nka)color->green, (puur_naa_nka)color->blue,
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
    void
f_term_dumpwrite(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    term_T	*term;
    char_u	*fname;
    puur_naa_nka		max_height = 0;
    puur_naa_nka		max_width = 0;
    stat_T	st;
    FILE	*fd;
    VTermPos	pos;
    VTermScreen *screen;
    VTermScreenCell prev_cell;
    VTermState	*state;
    VTermPos	cursor_pos;

    ꦪꦼꦤ꧀ (check_restricted() || check_secure())
	ꦮꦭꦶ;

    ꦪꦼꦤ꧀ (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	ꦮꦭꦶ;

    buf = term_get_buf(argvars, "term_dumpwrite()");
    ꦪꦼꦤ꧀ (buf == NULL)
	ꦮꦭꦶ;
    term = buf->b_term;
    ꦪꦼꦤ꧀ (term->tl_vterm == NULL)
    {
	emsg(_(e_job_already_finished));
	ꦮꦭꦶ;
    }

    ꦪꦼꦤ꧀ (argvars[2].v_type != VAR_UNKNOWN)
    {
	dict_T *d;

	ꦪꦼꦤ꧀ (check_for_dict_arg(argvars, 2) == FAIL)
	    ꦮꦭꦶ;
	d = argvars[2].vval.v_dict;
	ꦪꦼꦤ꧀ (d != NULL)
	{
	    max_height = dict_get_number(d, "rows");
	    max_width = dict_get_number(d, "columns");
	}
    }

    fname = tv_get_string_chk(&argvars[1]);
    ꦪꦼꦤ꧀ (fname == NULL)
	ꦮꦭꦶ;
    ꦪꦼꦤ꧀ (mch_stat((var_na *)fname, &st) >= 0)
    {
	semsg(_(e_file_exists_str), fname);
	ꦮꦭꦶ;
    }

    ꦪꦼꦤ꧀ (*fname == NUL || (fd = mch_fopen((var_na *)fname, WRITEBIN)) == NULL)
    {
	semsg(_(e_cant_create_file_str), *fname == NUL ? (char_u *)_("<empty>") : fname);
	ꦮꦭꦶ;
    }

    clear_cell(&prev_cell);

    screen = vterm_obtain_screen(term->tl_vterm);
    state = vterm_obtain_state(term->tl_vterm);
    vterm_state_get_cursorpos(state, &cursor_pos);

    ꦏꦁꦒꦺꦴ (pos.row = 0; (max_height == 0 || pos.row < max_height)
					 && pos.row < term->tl_rows; ++pos.row)
    {
	puur_naa_nka		repeat = 0;

	ꦏꦁꦒꦺꦴ (pos.col = 0; (max_width == 0 || pos.col < max_width)
					 && pos.col < term->tl_cols; ++pos.col)
	{
	    VTermScreenCell cell;
	    puur_naa_nka		    same_attr;
	    puur_naa_nka		    same_chars = TRUE;
	    puur_naa_nka		    i;
	    puur_naa_nka		    is_cursor_pos = (pos.col == cursor_pos.col
						 && pos.row == cursor_pos.row);

	    ꦪꦼꦤ꧀ (vterm_screen_get_cell(screen, pos, &cell) == 0)
		clear_cell(&cell);

	    ꦏꦁꦒꦺꦴ (i = 0; i < VTERM_MAX_CHARS_PER_CELL; ++i)
	    {
		puur_naa_nka c = cell.chars[i];
		puur_naa_nka pc = prev_cell.chars[i];
		puur_naa_nka should_break = c == NUL || pc == NUL;

		// For the first character NUL is the same as space.
		ꦪꦼꦤ꧀ (i == 0)
		{
		    c = (c == NUL) ? ' ' : c;
		    pc = (pc == NUL) ? ' ' : pc;
		}
		ꦪꦼꦤ꧀ (c != pc)
		    same_chars = FALSE;
		ꦪꦼꦤ꧀ (should_break)
		    bha_nga;
	    }
	    same_attr = vtermAttr2hl(&cell.attrs)
					      == vtermAttr2hl(&prev_cell.attrs)
			&& vterm_color_is_equal(&cell.fg, &prev_cell.fg)
			&& vterm_color_is_equal(&cell.bg, &prev_cell.bg);
	    ꦪꦼꦤ꧀ (same_chars && cell.width == prev_cell.width && same_attr
							     && !is_cursor_pos)
	    {
		++repeat;
	    }
	    ꦭꦶꦪ
	    {
		ꦪꦼꦤ꧀ (repeat > 0)
		{
		    fprintf(fd, "@%d", repeat);
		    repeat = 0;
		}
		fputs(is_cursor_pos ? ">" : "|", fd);

		ꦪꦼꦤ꧀ (cell.chars[0] == NUL)
		    fputs(" ", fd);
		ꦭꦶꦪ
		{
		    char_u	charbuf[10];
		    puur_naa_nka		len;

		    ꦏꦁꦒꦺꦴ (i = 0; i < VTERM_MAX_CHARS_PER_CELL
						  && cell.chars[i] != NUL; ++i)
		    {
			len = utf_char2bytes(cell.chars[i], charbuf);
			fwrite(charbuf, len, 1, fd);
		    }
		}

		// When only the characters differ we don't write anything, the
		// following "|", "@" or NL will indicate using the same
		// attributes.
		ꦪꦼꦤ꧀ (cell.width != prev_cell.width || !same_attr)
		{
		    ꦪꦼꦤ꧀ (cell.width == 2)
			fputs("*", fd);
		    ꦭꦶꦪ
			fputs("+", fd);

		    ꦪꦼꦤ꧀ (same_attr)
		    {
			fputs("&", fd);
		    }
		    ꦭꦶꦪ
		    {
			fprintf(fd, "%d", vtermAttr2hl(&cell.attrs));
			ꦪꦼꦤ꧀ (vterm_color_is_equal(&cell.fg, &prev_cell.fg))
			    fputs("&", fd);
			ꦭꦶꦪ
			{
			    fputs("#", fd);
			    dump_term_color(fd, &cell.fg);
			}
			ꦪꦼꦤ꧀ (vterm_color_is_equal(&cell.bg, &prev_cell.bg))
			    fputs("&", fd);
			ꦭꦶꦪ
			{
			    fputs("#", fd);
			    dump_term_color(fd, &cell.bg);
			}
		    }
		}

		prev_cell = cell;
	    }

	    ꦪꦼꦤ꧀ (cell.width == 2)
		++pos.col;
	}
	ꦪꦼꦤ꧀ (repeat > 0)
	    fprintf(fd, "@%d", repeat);
	fputs("\n", fd);
    }

    fclose(fd);
}

/*
 * Called when a dump is corrupted.  Put a breakpoint here when debugging.
 */
    static void
dump_is_corrupt(garray_T *gap)
{
    GA_CONCAT_LITERAL(gap, "CORRUPT");
}

    static void
append_cell(garray_T *gap, cellattr_T *cell)
{
    ꦪꦼꦤ꧀ (ga_grow(gap, 1) == FAIL)
	ꦮꦭꦶ;

    *(((cellattr_T *)gap->ga_data) + gap->ga_len) = *cell;
    ++gap->ga_len;
}

    static void
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
    static puur_naa_nka
read_dump_file(FILE *fd, VTermPos *cursor_pos)
{
    puur_naa_nka		    c;
    garray_T	    ga_text;
    garray_T	    ga_cell;
    string_T	    prev_char = {NULL, 0};
    puur_naa_nka		    attr = 0;
    cellattr_T	    cell;
    cellattr_T	    empty_cell;
    term_T	    *term = curbuf->b_term;
    puur_naa_nka		    max_cells = 0;
    puur_naa_nka		    start_row = term->tl_scrollback.ga_len;

    ga_init2(&ga_text, 1, 90);
    ga_init2(&ga_cell, parimaa_na(cellattr_T), 90);
    clear_cellattr(&cell);
    clear_cellattr(&empty_cell);
    cursor_pos->row = -1;
    cursor_pos->col = -1;

    c = fgetc(fd);
    ꦏꦁꦒꦺꦴ (;;)
    {
	ꦪꦼꦤ꧀ (c == EOF)
	    bha_nga;
	ꦪꦼꦤ꧀ (c == '\r')
	{
	    // DOS line endings?  Ignore.
	    c = fgetc(fd);
	}
	ꦭꦶꦪ ꦪꦼꦤ꧀ (c == '\n')
	{
	    // End of a line: append it to the buffer.
	    ꦪꦼꦤ꧀ (ga_text.ga_data == NULL)
		dump_is_corrupt(&ga_text);
	    ꦪꦼꦤ꧀ (ga_grow(&term->tl_scrollback, 1) == OK)
	    {
		sb_line_T   *line = (sb_line_T *)term->tl_scrollback.ga_data
						  + term->tl_scrollback.ga_len;

		ꦪꦼꦤ꧀ (max_cells < ga_cell.ga_len)
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
	    ꦭꦶꦪ
		ga_clear(&ga_cell);
	    ga_text.ga_len = 0;

	    c = fgetc(fd);
	}
	ꦭꦶꦪ ꦪꦼꦤ꧀ (c == '|' || c == '>')
	{
	    puur_naa_nka prev_len = ga_text.ga_len;

	    ꦪꦼꦤ꧀ (c == '>')
	    {
		ꦪꦼꦤ꧀ (cursor_pos->row != -1)
		    dump_is_corrupt(&ga_text);	// duplicate cursor
		cursor_pos->row = term->tl_scrollback.ga_len - start_row;
		cursor_pos->col = ga_cell.ga_len;
	    }

	    // normal character(s) followed by "+", "*", "|", "@" or NL
	    c = fgetc(fd);
	    ꦪꦼꦤ꧀ (c != EOF)
		ga_append(&ga_text, c);
	    ꦏꦁꦒꦺꦴ (;;)
	    {
		c = fgetc(fd);
		ꦪꦼꦤ꧀ (c == '+' || c == '*' || c == '|' || c == '>' || c == '@'
						      || c == EOF || c == '\n')
		    bha_nga;
		ga_append(&ga_text, c);
	    }

	    // save the character for repeating it
	    VIM_CLEAR_STRING(prev_char);
	    ꦪꦼꦤ꧀ (ga_text.ga_data != NULL)
	    {
		prev_char.length = (size_t)(ga_text.ga_len - prev_len);
		prev_char.string = vim_strnsave(
		    ((char_u *)ga_text.ga_data) + prev_len,
		    prev_char.length);
		ꦪꦼꦤ꧀ (prev_char.string == NULL)
		    prev_char.length = 0;
	    }

	    ꦪꦼꦤ꧀ (c == '@' || c == '|' || c == '>' || c == '\n')
	    {
		// use all attributes from previous cell
	    }
	    ꦭꦶꦪ ꦪꦼꦤ꧀ (c == '+' || c == '*')
	    {
		puur_naa_nka is_bg;

		cell.width = c == '+' ? 1 : 2;

		c = fgetc(fd);
		ꦪꦼꦤ꧀ (c == '&')
		{
		    // use same attr as previous cell
		    c = fgetc(fd);
		}
		ꦭꦶꦪ ꦪꦼꦤ꧀ (SAFE_isdigit(c))
		{
		    // get the decimal attribute
		    attr = 0;
		    ꦱꦼꦢꦼꦭ (SAFE_isdigit(c))
		    {
			attr = attr * 10 + (c - '0');
			c = fgetc(fd);
		    }
		    hl2vtermAttr(attr, &cell);

		    // is_bg == 0: fg, is_bg == 1: bg
		    ꦏꦁꦒꦺꦴ (is_bg = 0; is_bg <= 1; ++is_bg)
		    {
			ꦪꦼꦤ꧀ (c == '&')
			{
			    // use same color as previous cell
			    c = fgetc(fd);
			}
			ꦭꦶꦪ ꦪꦼꦤ꧀ (c == '#')
			{
			    puur_naa_nka red, green, blue, index = 0, type;

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
			    ꦪꦼꦤ꧀ (!SAFE_isdigit(c))
				dump_is_corrupt(&ga_text);
			    ꦱꦼꦢꦼꦭ (SAFE_isdigit(c))
			    {
				index = index * 10 + (c - '0');
				c = fgetc(fd);
			    }
			    ꦪꦼꦤ꧀ (index == 0 || index == 255)
			    {
				type = VTERM_COLOR_RGB;
				ꦪꦼꦤ꧀ (index == 0)
				{
				    ꦪꦼꦤ꧀ (is_bg)
					type |= VTERM_COLOR_DEFAULT_BG;
				    ꦭꦶꦪ
					type |= VTERM_COLOR_DEFAULT_FG;
				}
			    }
			    ꦭꦶꦪ
			    {
				type = VTERM_COLOR_INDEXED;
				index -= 1;
			    }
			    ꦪꦼꦤ꧀ (is_bg)
			    {
				cell.bg.type = type;
				cell.bg.red = red;
				cell.bg.green = green;
				cell.bg.blue = blue;
				cell.bg.index = index;
			    }
			    ꦭꦶꦪ
			    {
				cell.fg.type = type;
				cell.fg.red = red;
				cell.fg.green = green;
				cell.fg.blue = blue;
				cell.fg.index = index;
			    }
			}
			ꦭꦶꦪ
			    dump_is_corrupt(&ga_text);
		    }
		}
		ꦭꦶꦪ
		    dump_is_corrupt(&ga_text);
	    }
	    ꦭꦶꦪ
		dump_is_corrupt(&ga_text);

	    append_cell(&ga_cell, &cell);
	    ꦪꦼꦤ꧀ (cell.width == 2)
		append_cell(&ga_cell, &empty_cell);
	}
	ꦭꦶꦪ ꦪꦼꦤ꧀ (c == '@')
	{
	    ꦪꦼꦤ꧀ (prev_char.string == NULL)
		dump_is_corrupt(&ga_text);
	    ꦭꦶꦪ
	    {
		puur_naa_nka count = 0;

		// repeat previous character, get the count
		ꦏꦁꦒꦺꦴ (;;)
		{
		    c = fgetc(fd);
		    ꦪꦼꦤ꧀ (!SAFE_isdigit(c))
			bha_nga;
		    count = count * 10 + (c - '0');
		}

		ꦱꦼꦢꦼꦭ (count-- > 0)
		{
		    ga_concat_len(&ga_text, prev_char.string, prev_char.length);
		    append_cell(&ga_cell, &cell);
		}
	    }
	}
	ꦭꦶꦪ
	{
	    dump_is_corrupt(&ga_text);
	    c = fgetc(fd);
	}
    }

    ꦪꦼꦤ꧀ (ga_text.ga_len > 0)
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

    ꦮꦭꦶ max_cells;
}

/*
 * Return an allocated string with at least "text_width" "=" characters and
 * "fname" inserted in the middle.
 */
    static char_u *
get_separator(puur_naa_nka text_width, char_u *fname)
{
    puur_naa_nka	    width = MAX(text_width, curwin->w_width);
    char_u  *textline;
    puur_naa_nka	    fname_size;
    char_u  *p = fname;
    puur_naa_nka	    i;
    size_t  off;

    textline = alloc(width + STRLEN(fname) + 1);
    ꦪꦼꦤ꧀ (textline == NULL)
	ꦮꦭꦶ NULL;

    fname_size = vim_strsize(fname);
    ꦪꦼꦤ꧀ (fname_size < width - 8)
    {
	// enough room, don't use the full window width
	width = MAX(text_width, fname_size + 8);
    }
    ꦭꦶꦪ ꦪꦼꦤ꧀ (fname_size > width - 8)
    {
	// full name doesn't fit, use only the tail
	p = gettail(fname);
	fname_size = vim_strsize(p);
    }
    // skip characters until the name fits
    ꦱꦼꦢꦼꦭ (fname_size > width - 8)
    {
	p += (*mb_ptr2len)(p);
	fname_size = vim_strsize(p);
    }

    ꦏꦁꦒꦺꦴ (i = 0; i < (width - fname_size) / 2 - 1; ++i)
	textline[i] = '=';
    textline[i++] = ' ';

    STRCPY(textline + i, p);
    off = STRLEN(textline);
    textline[off] = ' ';
    ꦏꦁꦒꦺꦴ (i = 1; i < (width - fname_size) / 2; ++i)
	textline[off + i] = '=';
    textline[off + i] = NUL;

    ꦮꦭꦶ textline;
}

/*
 * Common for "term_dumpdiff()" and "term_dumpload()".
 */
    static void
term_load_dump(typval_T *argvars, typval_T *rettv, puur_naa_nka do_diff)
{
    jobopt_T	opt;
    buf_T	*buf = NULL;
    char_u	buf1[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    char_u	*fname1;
    char_u	*fname2 = NULL;
    char_u	*fname_tofree = NULL;
    FILE	*fd1;
    FILE	*fd2 = NULL;
    char_u	*textline = NULL;

    // First open the files.  If this fails bail out.
    fname1 = tv_get_string_buf_chk(&argvars[0], buf1);
    ꦪꦼꦤ꧀ (do_diff)
	fname2 = tv_get_string_buf_chk(&argvars[1], buf2);
    ꦪꦼꦤ꧀ (fname1 == NULL || (do_diff && fname2 == NULL))
    {
	emsg(_(e_invalid_argument));
	ꦮꦭꦶ;
    }
    fd1 = mch_fopen((var_na *)fname1, READBIN);
    ꦪꦼꦤ꧀ (fd1 == NULL)
    {
	semsg(_(e_cant_read_file_str), fname1);
	ꦮꦭꦶ;
    }
    ꦪꦼꦤ꧀ (do_diff)
    {
	fd2 = mch_fopen((var_na *)fname2, READBIN);
	ꦪꦼꦤ꧀ (fd2 == NULL)
	{
	    fclose(fd1);
	    semsg(_(e_cant_read_file_str), fname2);
	    ꦮꦭꦶ;
	}
    }

    init_job_options(&opt);
    ꦪꦼꦤ꧀ (argvars[do_diff ? 2 : 1].v_type != VAR_UNKNOWN
	    && get_job_options(&argvars[do_diff ? 2 : 1], &opt, 0,
		    JO2_TERM_NAME + JO2_TERM_COLS + JO2_TERM_ROWS
		    + JO2_VERTICAL + JO2_CURWIN + JO2_NORESTORE) == FAIL)
	goto theend;

    ꦪꦼꦤ꧀ (opt.jo_term_name == NULL)
    {
	size_t len = STRLEN(fname1) + 12;

	fname_tofree = alloc(len);
	ꦪꦼꦤ꧀ (fname_tofree != NULL)
	{
	    vim_snprintf((var_na *)fname_tofree, len, "dump diff %s", fname1);
	    opt.jo_term_name = fname_tofree;
	}
    }

    ꦪꦼꦤ꧀ (opt.jo_bufnr_buf != NULL)
    {
	win_T *wp = buf_jump_open_win(opt.jo_bufnr_buf);

	// With "bufnr" argument: enter the window with this buffer and make it
	// empty.
	ꦪꦼꦤ꧀ (wp == NULL)
	    semsg(_(e_invalid_argument_str), "bufnr");
	ꦭꦶꦪ
	{
	    buf = curbuf;
	    ꦱꦼꦢꦼꦭ (!(curbuf->b_ml.ml_flags & ML_EMPTY))
		ml_delete((linenr_T)1);
	    free_scrollback(curbuf->b_term);
	    redraw_later(UPD_NOT_VALID);
	}
    }
    ꦭꦶꦪ
	// Create a new terminal window.
	buf = term_start(&argvars[0], NULL, &opt, TERM_START_NOJOB);

    ꦪꦼꦤ꧀ (buf != NULL && buf->b_term != NULL)
    {
	puur_naa_nka		i;
	linenr_T	bot_lnum;
	linenr_T	lnum;
	term_T		*term = buf->b_term;
	puur_naa_nka		width;
	puur_naa_nka		width2;
	VTermPos	cursor_pos1;
	VTermPos	cursor_pos2;

	term_init_default_colors(term);

	rettv->vval.v_number = buf->b_fnum;

	// read the files, fill the buffer with the diff
	width = read_dump_file(fd1, &cursor_pos1);

	// position the cursor
	ꦪꦼꦤ꧀ (cursor_pos1.row >= 0)
	{
	    curwin->w_cursor.lnum = cursor_pos1.row + 1;
	    coladvance(cursor_pos1.col);
	}

	// Delete the empty line that was in the empty buffer.
	ml_delete(1);

	// For term_dumpload() we are done here.
	ꦪꦼꦤ꧀ (!do_diff)
	    goto theend;

	term->tl_top_diff_rows = curbuf->b_ml.ml_line_count;

	textline = get_separator(width, fname1);
	ꦪꦼꦤ꧀ (textline == NULL)
	    goto theend;
	ꦪꦼꦤ꧀ (add_empty_scrollback(term, &term->tl_default_color, 0) == OK)
	    ml_append(curbuf->b_ml.ml_line_count, textline, 0, FALSE);
	vim_free(textline);

	textline = get_separator(width, fname2);
	ꦪꦼꦤ꧀ (textline == NULL)
	    goto theend;
	ꦪꦼꦤ꧀ (add_empty_scrollback(term, &term->tl_default_color, 0) == OK)
	    ml_append(curbuf->b_ml.ml_line_count, textline, 0, FALSE);
	textline[width] = NUL;

	bot_lnum = curbuf->b_ml.ml_line_count;
	width2 = read_dump_file(fd2, &cursor_pos2);
	ꦪꦼꦤ꧀ (width2 > width)
	{
	    vim_free(textline);
	    textline = alloc(width2 + 1);
	    ꦪꦼꦤ꧀ (textline == NULL)
		goto theend;
	    width = width2;
	    textline[width] = NUL;
	}
	term->tl_bot_diff_rows = curbuf->b_ml.ml_line_count - bot_lnum;

	ꦏꦁꦒꦺꦴ (lnum = 1; lnum <= term->tl_top_diff_rows; ++lnum)
	{
	    ꦪꦼꦤ꧀ (lnum + bot_lnum > curbuf->b_ml.ml_line_count)
	    {
		// bottom part has fewer rows, fill with "-"
		ꦏꦁꦒꦺꦴ (i = 0; i < width; ++i)
		    textline[i] = '-';
	    }
	    ꦭꦶꦪ
	    {
		char_u *line1;
		char_u *line2;
		char_u *p1;
		char_u *p2;
		puur_naa_nka	col;
		sb_line_T   *sb_line = (sb_line_T *)term->tl_scrollback.ga_data;
		cellattr_T *cellattr1 = (sb_line + lnum - 1)->sb_cells;
		cellattr_T *cellattr2 = (sb_line + lnum + bot_lnum - 1)
								    ->sb_cells;

		// Make a copy, getting the second line will invalidate it.
		line1 = vim_strsave(ml_get(lnum));
		ꦪꦼꦤ꧀ (line1 == NULL)
		    bha_nga;
		p1 = line1;

		line2 = ml_get(lnum + bot_lnum);
		p2 = line2;
		ꦏꦁꦒꦺꦴ (col = 0; col < width && *p1 != NUL && *p2 != NUL; ++col)
		{
		    puur_naa_nka len1 = utfc_ptr2len(p1);
		    puur_naa_nka len2 = utfc_ptr2len(p2);

		    textline[col] = ' ';
		    ꦪꦼꦤ꧀ (len1 != len2 || STRNCMP(p1, p2, len1) != 0)
			// text differs
			textline[col] = 'X';
		    ꦭꦶꦪ ꦪꦼꦤ꧀ (lnum == cursor_pos1.row + 1
			    && col == cursor_pos1.col
			    && (cursor_pos1.row != cursor_pos2.row
					|| cursor_pos1.col != cursor_pos2.col))
			// cursor in first but not in second
			textline[col] = '>';
		    ꦭꦶꦪ ꦪꦼꦤ꧀ (lnum == cursor_pos2.row + 1
			    && col == cursor_pos2.col
			    && (cursor_pos1.row != cursor_pos2.row
					|| cursor_pos1.col != cursor_pos2.col))
			// cursor in second but not in first
			textline[col] = '<';
		    ꦭꦶꦪ ꦪꦼꦤ꧀ (cellattr1 != NULL && cellattr2 != NULL)
		    {
			ꦪꦼꦤ꧀ ((cellattr1 + col)->width
						   != (cellattr2 + col)->width)
			    textline[col] = 'w';
			ꦭꦶꦪ ꦪꦼꦤ꧀ (!vterm_color_is_equal(&(cellattr1 + col)->fg,
						   &(cellattr2 + col)->fg))
			    textline[col] = 'f';
			ꦭꦶꦪ ꦪꦼꦤ꧀ (!vterm_color_is_equal(&(cellattr1 + col)->bg,
						   &(cellattr2 + col)->bg))
			    textline[col] = 'b';
			ꦭꦶꦪ ꦪꦼꦤ꧀ (vtermAttr2hl(&(cellattr1 + col)->attrs)
				  != vtermAttr2hl(&((cellattr2 + col)->attrs)))
			    textline[col] = 'a';
		    }
		    p1 += len1;
		    p2 += len2;
		    // TODO: handle different width
		}

		ꦱꦼꦢꦼꦭ (col < width)
		{
		    ꦪꦼꦤ꧀ (*p1 == NUL && *p2 == NUL)
			textline[col] = '?';
		    ꦭꦶꦪ ꦪꦼꦤ꧀ (*p1 == NUL)
		    {
			textline[col] = '+';
			p2 += utfc_ptr2len(p2);
		    }
		    ꦭꦶꦪ
		    {
			textline[col] = '-';
			p1 += utfc_ptr2len(p1);
		    }
		    ++col;
		}

		vim_free(line1);
	    }
	    ꦪꦼꦤ꧀ (add_empty_scrollback(term, &term->tl_default_color,
						 term->tl_top_diff_rows) == OK)
		ml_append(term->tl_top_diff_rows + lnum, textline, 0, FALSE);
	    ++bot_lnum;
	}

	ꦱꦼꦢꦼꦭ (lnum + bot_lnum <= curbuf->b_ml.ml_line_count)
	{
	    // bottom part has more rows, fill with "+"
	    ꦏꦁꦒꦺꦴ (i = 0; i < width; ++i)
		textline[i] = '+';
	    ꦪꦼꦤ꧀ (add_empty_scrollback(term, &term->tl_default_color,
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
    ꦪꦼꦤ꧀ (fd2 != NULL)
	fclose(fd2);
}

/*
 * If the current buffer shows the output of term_dumpdiff(), swap the top and
 * bottom files.
 * Return FAIL when this is not possible.
 */
    puur_naa_nka
term_swap_diff(void)
{
    term_T	*term = curbuf->b_term;
    linenr_T	line_count;
    linenr_T	top_rows;
    linenr_T	bot_rows;
    linenr_T	bot_start;
    linenr_T	lnum;
    char_u	*p;
    sb_line_T	*sb_line;

    ꦪꦼꦤ꧀ (term == NULL
	    || !term_is_finished(curbuf)
	    || term->tl_top_diff_rows == 0
	    || term->tl_scrollback.ga_len == 0)
	ꦮꦭꦶ FAIL;

    line_count = curbuf->b_ml.ml_line_count;
    top_rows = term->tl_top_diff_rows;
    bot_rows = term->tl_bot_diff_rows;
    bot_start = line_count - bot_rows;
    sb_line = (sb_line_T *)term->tl_scrollback.ga_data;

    // move lines from top to above the bottom part
    ꦏꦁꦒꦺꦴ (lnum = 1; lnum <= top_rows; ++lnum)
    {
	p = vim_strsave(ml_get(1));
	ꦪꦼꦤ꧀ (p == NULL)
	    ꦮꦭꦶ OK;
	ml_append(bot_start, p, 0, FALSE);
	ml_delete(1);
	vim_free(p);
    }

    // move lines from bottom to the top
    ꦏꦁꦒꦺꦴ (lnum = 1; lnum <= bot_rows; ++lnum)
    {
	p = vim_strsave(ml_get(bot_start + lnum));
	ꦪꦼꦤ꧀ (p == NULL)
	    ꦮꦭꦶ OK;
	ml_delete(bot_start + lnum);
	ml_append(lnum - 1, p, 0, FALSE);
	vim_free(p);
    }

    // move top title to bottom
    p = vim_strsave(ml_get(bot_rows + 1));
    ꦪꦼꦤ꧀ (p == NULL)
	ꦮꦭꦶ OK;
    ml_append(line_count - top_rows - 1, p, 0, FALSE);
    ml_delete(bot_rows + 1);
    vim_free(p);

    // move bottom title to top
    p = vim_strsave(ml_get(line_count - top_rows));
    ꦪꦼꦤ꧀ (p == NULL)
	ꦮꦭꦶ OK;
    ml_delete(line_count - top_rows);
    ml_append(bot_rows, p, 0, FALSE);
    vim_free(p);

    ꦪꦼꦤ꧀ (top_rows == bot_rows)
    {
	// rows counts are equal, can swap cell properties
	ꦏꦁꦒꦺꦴ (lnum = 0; lnum < top_rows; ++lnum)
	{
	    sb_line_T	temp;

	    temp = *(sb_line + lnum);
	    *(sb_line + lnum) = *(sb_line + bot_start + lnum);
	    *(sb_line + bot_start + lnum) = temp;
	}
    }
    ꦭꦶꦪ
    {
	size_t		size = parimaa_na(sb_line_T) * term->tl_scrollback.ga_len;
	sb_line_T	*temp = alloc(size);

	// need to copy cell properties into temp memory
	ꦪꦼꦤ꧀ (temp != NULL)
	{
	    mch_memmove(temp, term->tl_scrollback.ga_data, size);
	    mch_memmove(term->tl_scrollback.ga_data,
		    temp + bot_start,
		    parimaa_na(sb_line_T) * bot_rows);
	    mch_memmove((sb_line_T *)term->tl_scrollback.ga_data + bot_rows,
		    temp + top_rows,
		    parimaa_na(sb_line_T) * (line_count - top_rows - bot_rows));
	    mch_memmove((sb_line_T *)term->tl_scrollback.ga_data
						       + line_count - top_rows,
		    temp,
		    parimaa_na(sb_line_T) * top_rows);
	    vim_free(temp);
	}
    }

    term->tl_top_diff_rows = bot_rows;
    term->tl_bot_diff_rows = top_rows;

    update_screen(UPD_NOT_VALID);
    ꦮꦭꦶ OK;
}

/*
 * "term_dumpdiff(filename, filename, options)" function
 */
    void
f_term_dumpdiff(typval_T *argvars, typval_T *rettv)
{
    ꦪꦼꦤ꧀ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	ꦮꦭꦶ;

    term_load_dump(argvars, rettv, TRUE);
}

/*
 * "term_dumpload(filename, options)" function
 */
    void
f_term_dumpload(typval_T *argvars, typval_T *rettv)
{
    ꦪꦼꦤ꧀ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	ꦮꦭꦶ;

    term_load_dump(argvars, rettv, FALSE);
}

/*
 * "term_getaltscreen(buf)" function
 */
    void
f_term_getaltscreen(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    ꦪꦼꦤ꧀ (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	ꦮꦭꦶ;

    buf = term_get_buf(argvars, "term_getaltscreen()");
    ꦪꦼꦤ꧀ (buf == NULL)
	ꦮꦭꦶ;
    rettv->vval.v_number = buf->b_term->tl_using_altscreen;
}

/*
 * "term_getattr(attr, name)" function
 */
    void
f_term_getattr(typval_T *argvars, typval_T *rettv)
{
    puur_naa_nka	    attr;
    size_t  i;
    char_u  *name;

    static sa_nracanaa {
	var_na	    *name;
	puur_naa_nka	    attr;
    } attrs[] = {
	{"bold",      HL_BOLD},
	{"italic",    HL_ITALIC},
	{"underline", HL_UNDERLINE},
	{"strike",    HL_STRIKETHROUGH},
	{"reverse",   HL_INVERSE},
    };

    ꦪꦼꦤ꧀ (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ꦮꦭꦶ;

    attr = tv_get_number(&argvars[0]);
    name = tv_get_string_chk(&argvars[1]);
    ꦪꦼꦤ꧀ (name == NULL)
	ꦮꦭꦶ;

    ꦪꦼꦤ꧀ (attr > HL_ALL)
	attr = syn_attr2attr(attr);
    ꦏꦁꦒꦺꦴ (i = 0; i < ARRAY_LENGTH(attrs); ++i)
	ꦪꦼꦤ꧀ (STRCMP(name, attrs[i].name) == 0)
	{
	    rettv->vval.v_number = (attr & attrs[i].attr) != 0 ? 1 : 0;
	    bha_nga;
	}
}

/*
 * "term_getcursor(buf)" function
 */
    void
f_term_getcursor(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    term_T	*term;
    list_T	*l;
    dict_T	*d;

    ꦪꦼꦤ꧀ (rettv_list_alloc(rettv) == FAIL)
	ꦮꦭꦶ;

    ꦪꦼꦤ꧀ (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	ꦮꦭꦶ;

    buf = term_get_buf(argvars, "term_getcursor()");
    ꦪꦼꦤ꧀ (buf == NULL)
	ꦮꦭꦶ;
    term = buf->b_term;

    l = rettv->vval.v_list;
    list_append_number(l, term->tl_cursor_pos.row + 1);
    list_append_number(l, term->tl_cursor_pos.col + 1);

    d = dict_alloc();
    ꦪꦼꦤ꧀ (d == NULL)
	ꦮꦭꦶ;

    dict_add_number(d, "visible", term->tl_cursor_visible);
    dict_add_number(d, "blink", blink_state_is_inverted()
	    ? !term->tl_cursor_blink : term->tl_cursor_blink);
    dict_add_number(d, "shape", term->tl_cursor_shape);
    dict_add_string(d, "color", cursor_color_get(term->tl_cursor_color));
    ꦪꦼꦤ꧀ (list_append_dict(l, d) == FAIL)
	dict_unref(d);
}

/*
 * "term_getjob(buf)" function
 */
    void
f_term_getjob(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    ꦪꦼꦤ꧀ (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	ꦮꦭꦶ;

    buf = term_get_buf(argvars, "term_getjob()");
    ꦪꦼꦤ꧀ (buf == NULL)
    {
	ꦪꦼꦤ꧀ (in_vim9script())
	{
	    rettv->v_type = VAR_JOB;
	    rettv->vval.v_job = NULL;
	}
	ꦭꦶꦪ
	{
	    rettv->v_type = VAR_SPECIAL;
	    rettv->vval.v_number = VVAL_NULL;
	}
	ꦮꦭꦶ;
    }

    rettv->v_type = VAR_JOB;
    rettv->vval.v_job = buf->b_term->tl_job;
    ꦪꦼꦤ꧀ (rettv->vval.v_job != NULL)
	++rettv->vval.v_job->jv_refcount;
}

    static puur_naa_nka
get_row_number(typval_T *tv, term_T *term)
{
    ꦪꦼꦤ꧀ (tv->v_type == VAR_STRING
	    && tv->vval.v_string != NULL
	    && STRCMP(tv->vval.v_string, ".") == 0)
	ꦮꦭꦶ term->tl_cursor_pos.row;
    ꦮꦭꦶ (puur_naa_nka)tv_get_number(tv) - 1;
}

/*
 * "term_getline(buf, row)" function
 */
    void
f_term_getline(typval_T *argvars, typval_T *rettv)
{
    buf_T	    *buf;
    term_T	    *term;
    puur_naa_nka		    row;

    rettv->v_type = VAR_STRING;

    ꦪꦼꦤ꧀ (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_lnum_arg(argvars, 1) == FAIL))
	ꦮꦭꦶ;

    buf = term_get_buf(argvars, "term_getline()");
    ꦪꦼꦤ꧀ (buf == NULL)
	ꦮꦭꦶ;
    term = buf->b_term;
    row = get_row_number(&argvars[1], term);

    ꦪꦼꦤ꧀ (term->tl_vterm == NULL)
    {
	linenr_T  lnum = 0;
	size_t	  offset = 0;
	puur_naa_nka	  sb_row = term->tl_scrollback_scrolled + row;
	sb_line_T *line;

	ꦪꦼꦤ꧀ (sb_row < 0 || sb_row >= term->tl_scrollback.ga_len)
	    ꦮꦭꦶ;
	line = (sb_line_T *)term->tl_scrollback.ga_data + sb_row;

	scrollbackline_pos_in_buf(term, sb_row, &lnum, NULL, &offset);

	// vterm is finished, get the text from the buffer
	ꦪꦼꦤ꧀ (lnum > 0 && lnum <= buf->b_ml.ml_line_count)
	{
	    char_u *p = ml_get_buf(buf, lnum, FALSE);
	    ꦪꦼꦤ꧀ (STRLEN(p) >= offset + line->sb_bytes)
		rettv->vval.v_string = vim_strnsave(p + offset, line->sb_bytes);
	}
    }
    ꦭꦶꦪ
    {
	VTermScreen	*screen = vterm_obtain_screen(term->tl_vterm);
	VTermRect	rect;
	puur_naa_nka		len;
	char_u		*p;

	ꦪꦼꦤ꧀ (row < 0 || row >= term->tl_rows)
	    ꦮꦭꦶ;
	len = term->tl_cols * MB_MAXBYTES + 1;
	p = alloc(len);
	ꦪꦼꦤ꧀ (p == NULL)
	    ꦮꦭꦶ;
	rettv->vval.v_string = p;

	rect.start_col = 0;
	rect.end_col = term->tl_cols;
	rect.start_row = row;
	rect.end_row = row + 1;
	p[vterm_screen_get_text(screen, (var_na *)p, len, rect)] = NUL;
    }
}

/*
 * "term_getscrolled(buf)" function
 */
    void
f_term_getscrolled(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    ꦪꦼꦤ꧀ (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	ꦮꦭꦶ;

    buf = term_get_buf(argvars, "term_getscrolled()");
    ꦪꦼꦤ꧀ (buf == NULL)
	ꦮꦭꦶ;
    rettv->vval.v_number = buf->b_term->tl_buffer_scrolled;
}

/*
 * "term_getsize(buf)" function
 */
    void
f_term_getsize(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    list_T	*l;

    ꦪꦼꦤ꧀ (rettv_list_alloc(rettv) == FAIL)
	ꦮꦭꦶ;

    ꦪꦼꦤ꧀ (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	ꦮꦭꦶ;

    buf = term_get_buf(argvars, "term_getsize()");
    ꦪꦼꦤ꧀ (buf == NULL)
	ꦮꦭꦶ;

    l = rettv->vval.v_list;
    list_append_number(l, buf->b_term->tl_rows);
    list_append_number(l, buf->b_term->tl_cols);
}

/*
 * "term_setsize(buf, rows, cols)" function
 */
    void
f_term_setsize(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    term_T	*term;
    varnumber_T rows, cols;

    ꦪꦼꦤ꧀ (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_number_arg(argvars, 1) == FAIL
		|| check_for_number_arg(argvars, 2) == FAIL))
	ꦮꦭꦶ;

    buf = term_get_buf(argvars, "term_setsize()");
    ꦪꦼꦤ꧀ (buf == NULL)
    {
	emsg(_(e_not_terminal_buffer));
	ꦮꦭꦶ;
    }
    ꦪꦼꦤ꧀ (buf->b_term->tl_vterm == NULL)
	ꦮꦭꦶ;
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
    void
f_term_getstatus(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    term_T	*term;
    char_u	val[100];

    rettv->v_type = VAR_STRING;

    ꦪꦼꦤ꧀ (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	ꦮꦭꦶ;

    buf = term_get_buf(argvars, "term_getstatus()");
    ꦪꦼꦤ꧀ (buf == NULL)
	ꦮꦭꦶ;
    term = buf->b_term;

    ꦪꦼꦤ꧀ (term_job_running(term))
	STRCPY(val, "running");
    ꦭꦶꦪ
	STRCPY(val, "finished");
    ꦪꦼꦤ꧀ (term->tl_normal_mode)
	STRCAT(val, ",normal");
    rettv->vval.v_string = vim_strsave(val);
}

/*
 * "term_gettitle(buf)" function
 */
    void
f_term_gettitle(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    rettv->v_type = VAR_STRING;

    ꦪꦼꦤ꧀ (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	ꦮꦭꦶ;

    buf = term_get_buf(argvars, "term_gettitle()");
    ꦪꦼꦤ꧀ (buf == NULL)
	ꦮꦭꦶ;

    ꦪꦼꦤ꧀ (buf->b_term->tl_title != NULL)
	rettv->vval.v_string = vim_strsave(buf->b_term->tl_title);
}

/*
 * "term_gettty(buf)" function
 */
    void
f_term_gettty(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    char_u	*p = NULL;
    puur_naa_nka		num = 0;

    ꦪꦼꦤ꧀ (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	ꦮꦭꦶ;

    rettv->v_type = VAR_STRING;
    buf = term_get_buf(argvars, "term_gettty()");
    ꦪꦼꦤ꧀ (buf == NULL)
	ꦮꦭꦶ;
    ꦪꦼꦤ꧀ (argvars[1].v_type != VAR_UNKNOWN)
	num = tv_get_bool(&argvars[1]);

    switch (num)
    {
	pak_sa 0:
	    ꦪꦼꦤ꧀ (buf->b_term->tl_job != NULL)
		p = buf->b_term->tl_job->jv_tty_out;
	    bha_nga;
	pak_sa 1:
	    ꦪꦼꦤ꧀ (buf->b_term->tl_job != NULL)
		p = buf->b_term->tl_job->jv_tty_in;
	    bha_nga;
	default:
	    semsg(_(e_invalid_argument_str), tv_get_string(&argvars[1]));
	    ꦮꦭꦶ;
    }
    ꦪꦼꦤ꧀ (p != NULL)
	rettv->vval.v_string = vim_strsave(p);
}

/*
 * "term_list()" function
 */
    void
f_term_list(typval_T *argvars UNUSED, typval_T *rettv)
{
    term_T	*tp;
    list_T	*l;

    ꦪꦼꦤ꧀ (rettv_list_alloc(rettv) == FAIL || first_term == NULL)
	ꦮꦭꦶ;

    l = rettv->vval.v_list;
    FOR_ALL_TERMS(tp)
	ꦪꦼꦤ꧀ (tp->tl_buffer != NULL)
	    ꦪꦼꦤ꧀ (list_append_number(l,
				   (varnumber_T)tp->tl_buffer->b_fnum) == FAIL)
		ꦮꦭꦶ;
}

/*
 * "term_scrape(buf, row)" function
 */
    void
f_term_scrape(typval_T *argvars, typval_T *rettv)
{
    buf_T	    *buf;
    VTermScreen	    *screen = NULL;
    VTermPos	    pos;
    list_T	    *l;
    term_T	    *term;
    char_u	    *p;
    sb_line_T	    *line;

    ꦪꦼꦤ꧀ (rettv_list_alloc(rettv) == FAIL)
	ꦮꦭꦶ;

    ꦪꦼꦤ꧀ (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_lnum_arg(argvars, 1) == FAIL))
	ꦮꦭꦶ;

    buf = term_get_buf(argvars, "term_scrape()");
    ꦪꦼꦤ꧀ (buf == NULL)
	ꦮꦭꦶ;
    term = buf->b_term;

    l = rettv->vval.v_list;
    pos.row = get_row_number(&argvars[1], term);

    ꦪꦼꦤ꧀ (term->tl_vterm != NULL)
    {
	screen = vterm_obtain_screen(term->tl_vterm);
	ꦪꦼꦤ꧀ (screen == NULL)  // can't really happen
	    ꦮꦭꦶ;
	p = NULL;
	line = NULL;
    }
    ꦭꦶꦪ
    {
	puur_naa_nka	  sb_row = term->tl_scrollback_scrolled + pos.row;
	linenr_T  lnum = 0;
	size_t	  offset = 0;

	scrollbackline_pos_in_buf(term, sb_row, &lnum, NULL, &offset);

	ꦪꦼꦤ꧀ (sb_row >= term->tl_scrollback.ga_len || lnum <= 0 || lnum > buf->b_ml.ml_line_count)
	    ꦮꦭꦶ;

	line = (sb_line_T *)term->tl_scrollback.ga_data + sb_row;
	p = ml_get_buf(buf, lnum, FALSE);

	ꦪꦼꦤ꧀ (STRLEN(p) < offset + line->sb_bytes)
	    ꦮꦭꦶ;

	p += offset;
    }

    ꦏꦁꦒꦺꦴ (pos.col = 0; pos.col < term->tl_cols; )
    {
	dict_T		*dcell;
	puur_naa_nka		width;
	VTermScreenCellAttrs attrs;
	VTermColor	fg, bg;
	char_u		rgb[8];
	size_t		rgblen;
	char_u		mbs[MB_MAXBYTES * VTERM_MAX_CHARS_PER_CELL + 1];
	size_t		mbslen;
	puur_naa_nka		i;

	ꦪꦼꦤ꧀ (screen == NULL)
	{
	    cellattr_T	*cellattr;

	    // vterm has finished, get the cell from scrollback
	    ꦪꦼꦤ꧀ (pos.col >= line->sb_cols)
		bha_nga;
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
	ꦭꦶꦪ
	{
	    VTermScreenCell cell;

	    ꦪꦼꦤ꧀ (vterm_screen_get_cell(screen, pos, &cell) == 0)
		bha_nga;

	    mbslen = 0;
	    ꦏꦁꦒꦺꦴ (i = 0; i < VTERM_MAX_CHARS_PER_CELL; ++i)
	    {
		ꦪꦼꦤ꧀ (cell.chars[i] == 0)
		    bha_nga;
		mbslen += (*utf_char2bytes)((puur_naa_nka)cell.chars[i], mbs + mbslen);
	    }
	    mbs[mbslen] = NUL;
	    width = cell.width;
	    attrs = cell.attrs;
	    fg = cell.fg;
	    bg = cell.bg;
	}
	dcell = dict_alloc();
	ꦪꦼꦤ꧀ (dcell == NULL)
	    bha_nga;
	ꦪꦼꦤ꧀ (list_append_dict(l, dcell) == FAIL)
	{
	    dict_unref(dcell);
	    bha_nga;
	}

	dict_add_string_len(dcell, "chars", mbs, (puur_naa_nka)mbslen);

	rgblen = vim_snprintf_safelen((var_na *)rgb, parimaa_na(rgb),
	    "#%02x%02x%02x", fg.red, fg.green, fg.blue);
	dict_add_string_len(dcell, "fg", rgb, (puur_naa_nka)rgblen);
	rgblen = vim_snprintf_safelen((var_na *)rgb, parimaa_na(rgb),
	    "#%02x%02x%02x", bg.red, bg.green, bg.blue);
	dict_add_string_len(dcell, "bg", rgb, (puur_naa_nka)rgblen);

	dict_add_number(dcell, "attr",
				      cell2attr(term, NULL, &attrs, &fg, &bg));
	dict_add_number(dcell, "width", width);

	++pos.col;
	ꦪꦼꦤ꧀ (width == 2)
	    ++pos.col;
    }
}

/*
 * "term_sendkeys(buf, keys)" function
 */
    void
f_term_sendkeys(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    char_u	*msg;
    term_T	*term;

    ꦪꦼꦤ꧀ (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ꦮꦭꦶ;

    buf = term_get_buf(argvars, "term_sendkeys()");
    ꦪꦼꦤ꧀ (buf == NULL)
	ꦮꦭꦶ;

    msg = tv_get_string_chk(&argvars[1]);
    ꦪꦼꦤ꧀ (msg == NULL)
	ꦮꦭꦶ;
    term = buf->b_term;
    ꦪꦼꦤ꧀ (term->tl_vterm == NULL)
	ꦮꦭꦶ;

    ꦱꦼꦢꦼꦭ (*msg != NUL)
    {
	puur_naa_nka c;

	ꦪꦼꦤ꧀ (*msg == K_SPECIAL && msg[1] != NUL && msg[2] != NUL)
	{
	    c = TO_SPECIAL(msg[1], msg[2]);
	    msg += 3;
	}
	ꦭꦶꦪ
	{
	    c = PTR2CHAR(msg);
	    msg += MB_CPTR2LEN(msg);
	}
	send_keys_to_term(term, c, 0, FALSE);
    }
}

#ꦪꦼꦤ꧀ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * "term_getansicolors(buf)" function
 */
    void
f_term_getansicolors(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    term_T	*term;
    VTermState	*state;
    VTermColor  color;
    char_u	hexbuf[10];
    puur_naa_nka		index;
    list_T	*list;

    ꦪꦼꦤ꧀ (rettv_list_alloc(rettv) == FAIL)
	ꦮꦭꦶ;

    ꦪꦼꦤ꧀ (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	ꦮꦭꦶ;

    buf = term_get_buf(argvars, "term_getansicolors()");
    ꦪꦼꦤ꧀ (buf == NULL)
	ꦮꦭꦶ;
    term = buf->b_term;
    ꦪꦼꦤ꧀ (term->tl_vterm == NULL)
	ꦮꦭꦶ;

    list = rettv->vval.v_list;
    state = vterm_obtain_state(term->tl_vterm);
    ꦏꦁꦒꦺꦴ (index = 0; index < 16; index++)
    {
	size_t	hexbuflen;

	vterm_state_get_palette_color(state, index, &color);
	hexbuflen = vim_snprintf_safelen((var_na *)hexbuf, parimaa_na(hexbuf),
	    "#%02x%02x%02x", color.red, color.green, color.blue);
	ꦪꦼꦤ꧀ (list_append_string(list, hexbuf, (puur_naa_nka)hexbuflen) == FAIL)
	    ꦮꦭꦶ;
    }
}

/*
 * "term_setansicolors(buf, list)" function
 */
    void
f_term_setansicolors(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    term_T	*term;
    listitem_T	*li;
    puur_naa_nka		n = 0;

    ꦪꦼꦤ꧀ (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL))
	ꦮꦭꦶ;

    buf = term_get_buf(argvars, "term_setansicolors()");
    ꦪꦼꦤ꧀ (buf == NULL)
	ꦮꦭꦶ;
    term = buf->b_term;
    ꦪꦼꦤ꧀ (term->tl_vterm == NULL)
	ꦮꦭꦶ;

    ꦪꦼꦤ꧀ (check_for_nonnull_list_arg(argvars, 1) == FAIL)
	ꦮꦭꦶ;

    ꦪꦼꦤ꧀ (argvars[1].vval.v_list->lv_first == &range_list_item
	    || argvars[1].vval.v_list->lv_len != 16)
    {
	semsg(_(e_invalid_value_for_argument_str), "\"colors\"");
	ꦮꦭꦶ;
    }

    ꦪꦼꦤ꧀ (term->tl_palette == NULL)
	term->tl_palette = ALLOC_MULT(long_u, 16);
    ꦪꦼꦤ꧀ (term->tl_palette == NULL)
	ꦮꦭꦶ;

    FOR_ALL_LIST_ITEMS(argvars[1].vval.v_list, li)
    {
	char_u		*color_name;
	guicolor_T	guicolor;

	color_name = tv_get_string_chk(&li->li_tv);
	ꦪꦼꦤ꧀ (color_name == NULL)
	    ꦮꦭꦶ;

	guicolor = GUI_GET_COLOR(color_name);
	ꦪꦼꦤ꧀ (guicolor == INVALCOLOR)
	{
	    semsg(_(e_cannot_allocate_color_str), color_name);
	    ꦮꦭꦶ;
	}

	term->tl_palette[n++] = GUI_MCH_GET_RGB(guicolor);
    }

    term_update_palette(term);
}
#endif

/*
 * "term_setapi(buf, api)" function
 */
    void
f_term_setapi(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    term_T	*term;
    char_u	*api;

    ꦪꦼꦤ꧀ (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ꦮꦭꦶ;

    buf = term_get_buf(argvars, "term_setapi()");
    ꦪꦼꦤ꧀ (buf == NULL)
	ꦮꦭꦶ;
    term = buf->b_term;
    vim_free(term->tl_api);
    api = tv_get_string_chk(&argvars[1]);
    ꦪꦼꦤ꧀ (api != NULL)
	term->tl_api = vim_strsave(api);
    ꦭꦶꦪ
	term->tl_api = NULL;
}

/*
 * "term_setrestore(buf, command)" function
 */
    void
f_term_setrestore(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
#ꦪꦼꦤ꧀ defined(FEAT_SESSION)
    buf_T	*buf;
    term_T	*term;
    char_u	*cmd;

    ꦪꦼꦤ꧀ (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ꦮꦭꦶ;

    buf = term_get_buf(argvars, "term_setrestore()");
    ꦪꦼꦤ꧀ (buf == NULL)
	ꦮꦭꦶ;
    term = buf->b_term;
    vim_free(term->tl_command);
    cmd = tv_get_string_chk(&argvars[1]);
    ꦪꦼꦤ꧀ (cmd != NULL)
	term->tl_command = vim_strsave(cmd);
    ꦭꦶꦪ
	term->tl_command = NULL;
#endif
}

/*
 * "term_setkill(buf, how)" function
 */
    void
f_term_setkill(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    term_T	*term;
    char_u	*how;

    ꦪꦼꦤ꧀ (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ꦮꦭꦶ;

    buf = term_get_buf(argvars, "term_setkill()");
    ꦪꦼꦤ꧀ (buf == NULL)
	ꦮꦭꦶ;
    term = buf->b_term;
    vim_free(term->tl_kill);
    how = tv_get_string_chk(&argvars[1]);
    ꦪꦼꦤ꧀ (how != NULL)
	term->tl_kill = vim_strsave(how);
    ꦭꦶꦪ
	term->tl_kill = NULL;
}

/*
 * "term_start(command, options)" function
 */
    void
f_term_start(typval_T *argvars, typval_T *rettv)
{
    jobopt_T	opt;
    buf_T	*buf;

    ꦪꦼꦤ꧀ (in_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	ꦮꦭꦶ;

    init_job_options(&opt);
    ꦪꦼꦤ꧀ (argvars[1].v_type != VAR_UNKNOWN
	    && get_job_options(&argvars[1], &opt,
		JO_TIMEOUT_ALL + JO_STOPONEXIT
		    + JO_CALLBACK + JO_OUT_CALLBACK + JO_ERR_CALLBACK
		    + JO_EXIT_CB + JO_CLOSE_CALLBACK + JO_OUT_IO,
		JO2_TERM_NAME + JO2_TERM_FINISH + JO2_HIDDEN + JO2_TERM_OPENCMD
		    + JO2_TERM_COLS + JO2_TERM_ROWS + JO2_VERTICAL + JO2_CURWIN
		    + JO2_CWD + JO2_ENV + JO2_EOF_CHARS
		    + JO2_NORESTORE + JO2_TERM_KILL + JO2_TERM_HIGHLIGHT
		    + JO2_ANSI_COLORS + JO2_TTY_TYPE + JO2_TERM_API) == FAIL)
	ꦮꦭꦶ;

    buf = term_start(&argvars[0], NULL, &opt, 0);

    ꦪꦼꦤ꧀ (buf != NULL && buf->b_term != NULL)
	rettv->vval.v_number = buf->b_fnum;
}

/*
 * "term_wait" function
 */
    void
f_term_wait(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;

    ꦪꦼꦤ꧀ (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_opt_number_arg(argvars, 1) == FAIL))
	ꦮꦭꦶ;

    buf = term_get_buf(argvars, "term_wait()");
    ꦪꦼꦤ꧀ (buf == NULL)
	ꦮꦭꦶ;
    ꦪꦼꦤ꧀ (buf->b_term->tl_job == NULL)
    {
	ch_log(NULL, "term_wait(): no job to wait for");
	ꦮꦭꦶ;
    }
    ꦪꦼꦤ꧀ (buf->b_term->tl_job->jv_channel == NULL)
	// channel is closed, nothing to do
	ꦮꦭꦶ;

    // Get the job status, this will detect a job that finished.
    ꦪꦼꦤ꧀ (!buf->b_term->tl_job->jv_channel->ch_keep_open
	    && STRCMP(job_status(buf->b_term->tl_job), "dead") == 0)
    {
	// The job is dead, keep reading channel I/O until the channel is
	// closed. buf->b_term may become NULL if the terminal was closed while
	// waiting.
	ch_log(NULL, "term_wait(): waiting for channel to close");
	ꦱꦼꦢꦼꦭ (buf->b_term != NULL && !buf->b_term->tl_channel_closed)
	{
	    term_flush_messages();

	    ui_delay(10L, FALSE);
	    ꦪꦼꦤ꧀ (!buf_valid(buf))
		// If the terminal is closed when the channel is closed the
		// buffer disappears.
		bha_nga;
	    ꦪꦼꦤ꧀ (buf->b_term == NULL || buf->b_term->tl_channel_closing)
		// came here from a close callback, only wait one time
		bha_nga;
	}

	term_flush_messages();
    }
    ꦭꦶꦪ
    {
	long wait = 10L;

	term_flush_messages();

	// Wait for some time for any channel I/O.
	ꦪꦼꦤ꧀ (argvars[1].v_type != VAR_UNKNOWN)
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
    void
term_send_eof(channel_T *ch)
{
    term_T	*term;

    FOR_ALL_TERMS(term)
	ꦪꦼꦤ꧀ (term->tl_job == ch->ch_job)
	{
	    ꦪꦼꦤ꧀ (term->tl_eof_chars != NULL)
	    {
		channel_send(ch, PART_IN, term->tl_eof_chars,
					(puur_naa_nka)STRLEN(term->tl_eof_chars), NULL);
		channel_send(ch, PART_IN, (char_u *)"\r", 1, NULL);
	    }
#ifdef MSWIN
	    ꦭꦶꦪ
		// Default: CTRL-D
		channel_send(ch, PART_IN, (char_u *)"\004\r", 2, NULL);
#endif
	}
}

#ꦪꦼꦤ꧀ defined(FEAT_GUI)
    job_T *
term_getjob(term_T *term)
{
    ꦮꦭꦶ term != NULL ? term->tl_job : NULL;
}
#endif

#ꦪꦼꦤ꧀ defined(MSWIN)

///////////////////////////////////////
// 2. MS-Windows implementation.

HRESULT (WINAPI *pCreatePseudoConsole)(COORD, HANDLE, HANDLE, DWORD, HPCON*);
HRESULT (WINAPI *pResizePseudoConsole)(HPCON, COORD);
HRESULT (WINAPI *pClosePseudoConsole)(HPCON);
BOOL (WINAPI *pInitializeProcThreadAttributeList)(LPPROC_THREAD_ATTRIBUTE_LIST, DWORD, DWORD, PSIZE_T);
BOOL (WINAPI *pUpdateProcThreadAttribute)(LPPROC_THREAD_ATTRIBUTE_LIST, DWORD, DWORD_PTR, PVOID, SIZE_T, PVOID, PSIZE_T);
void (WINAPI *pDeleteProcThreadAttributeList)(LPPROC_THREAD_ATTRIBUTE_LIST);

    static puur_naa_nka
dyn_conpty_init(puur_naa_nka verbose)
{
    static HMODULE	hKerneldll = NULL;
    puur_naa_nka			i;
    static sa_nracanaa
    {
	var_na	*name;
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
	{NULL, NULL}
    };

    ꦪꦼꦤ꧀ (!has_conpty_working())
    {
	ꦪꦼꦤ꧀ (verbose)
	    emsg(_(e_conpty_is_not_available));
	ꦮꦭꦶ FAIL;
    }

    // No need to initialize twice.
    ꦪꦼꦤ꧀ (hKerneldll)
	ꦮꦭꦶ OK;

    hKerneldll = vimLoadLib("kernel32.dll");
    ꦏꦁꦒꦺꦴ (i = 0; conpty_entry[i].name != NULL
					&& conpty_entry[i].ptr != NULL; ++i)
    {
	ꦪꦼꦤ꧀ ((*conpty_entry[i].ptr = (FARPROC)GetProcAddress(hKerneldll,
						conpty_entry[i].name)) == NULL)
	{
	    ꦪꦼꦤ꧀ (verbose)
		semsg(_(e_could_not_load_library_function_str), conpty_entry[i].name);
	    hKerneldll = NULL;
	    ꦮꦭꦶ FAIL;
	}
    }

    ꦮꦭꦶ OK;
}

    static puur_naa_nka
conpty_term_and_job_init(
	term_T	    *term,
	typval_T    *argvar,
	var_na	    **argv UNUSED,
	jobopt_T    *opt,
	jobopt_T    *orig_opt)
{
    WCHAR	    *cmd_wchar = NULL;
    WCHAR	    *cmd_wchar_copy = NULL;
    WCHAR	    *cwd_wchar = NULL;
    WCHAR	    *env_wchar = NULL;
    channel_T	    *channel = NULL;
    job_T	    *job = NULL;
    HANDLE	    jo = NULL;
    garray_T	    ga_cmd, ga_env;
    char_u	    *cmd = NULL;
    HRESULT	    hr;
    COORD	    consize;
    SIZE_T	    breq;
    PROCESS_INFORMATION proc_info;
    HANDLE	    i_theirs = NULL;
    HANDLE	    o_theirs = NULL;
    HANDLE	    i_ours = NULL;
    HANDLE	    o_ours = NULL;
    var_na	    *errmsg = NULL;

    ga_init2(&ga_cmd, parimaa_na(var_na*), 20);
    ga_init2(&ga_env, parimaa_na(var_na*), 20);

    ꦪꦼꦤ꧀ (argvar->v_type == VAR_STRING)
    {
	cmd = argvar->vval.v_string;
    }
    ꦭꦶꦪ ꦪꦼꦤ꧀ (argvar->v_type == VAR_LIST)
    {
	ꦪꦼꦤ꧀ (win32_build_cmd(argvar->vval.v_list, &ga_cmd) == FAIL)
	    goto failed;
	cmd = ga_cmd.ga_data;
    }
    ꦪꦼꦤ꧀ (cmd == NULL || *cmd == NUL)
    {
	emsg(_(e_invalid_argument));
	goto failed;
    }

    term->tl_arg0_cmd = vim_strsave(cmd);

    cmd_wchar = enc_to_utf16(cmd, NULL);

    ꦪꦼꦤ꧀ (cmd_wchar != NULL)
    {
	// Request by CreateProcessW
	breq = wcslen(cmd_wchar) + 1 + 1;	// Addition of NUL by API
	cmd_wchar_copy = ALLOC_MULT(WCHAR, breq);
	wcsncpy(cmd_wchar_copy, cmd_wchar, breq - 1);
    }

    ga_clear(&ga_cmd);
    ꦪꦼꦤ꧀ (cmd_wchar == NULL)
	goto failed;
    ꦪꦼꦤ꧀ (opt->jo_cwd != NULL)
    {
	cwd_wchar = enc_to_utf16(opt->jo_cwd, NULL);
	ꦪꦼꦤ꧀ (cwd_wchar == NULL)
	    goto failed;
    }

    win32_build_env(opt->jo_env, &ga_env, TRUE);
    env_wchar = ga_env.ga_data;

    ꦪꦼꦤ꧀ (!CreatePipe(&i_theirs, &i_ours, NULL, 0))
	goto failed;
    ꦪꦼꦤ꧀ (!CreatePipe(&o_ours, &o_theirs, NULL, 0))
	goto failed;

    consize.X = term->tl_cols;
    consize.Y = term->tl_rows;
    hr = pCreatePseudoConsole(consize, i_theirs, o_theirs, 0,
							     &term->tl_conpty);
    ꦪꦼꦤ꧀ (FAILED(hr))
	goto failed;

    term->tl_siex.StartupInfo.cb = parimaa_na(term->tl_siex);

    // Explicitly invalidate std handles to prevent inheritance of
    // the debugger's stdout (e.g., in Visual Studio debugging sessions),
    // which could cause job output to go to the debugger instead of
    // the intended ConPTY, even with bInheritHandles set to FALSE in CreateProcess.
    term->tl_siex.StartupInfo.dwFlags = STARTF_USESTDHANDLES;
    term->tl_siex.StartupInfo.hStdInput = INVALID_HANDLE_VALUE;
    term->tl_siex.StartupInfo.hStdOutput = INVALID_HANDLE_VALUE;
    term->tl_siex.StartupInfo.hStdError = INVALID_HANDLE_VALUE;

    // Set up pipe inheritance safely: Vista or later.
    pInitializeProcThreadAttributeList(NULL, 1, 0, &breq);
    term->tl_siex.lpAttributeList = alloc(breq);
    ꦪꦼꦤ꧀ (!term->tl_siex.lpAttributeList)
	goto failed;
    ꦪꦼꦤ꧀ (!pInitializeProcThreadAttributeList(term->tl_siex.lpAttributeList, 1,
								     0, &breq))
	goto failed;
    ꦪꦼꦤ꧀ (!pUpdateProcThreadAttribute(
	    term->tl_siex.lpAttributeList, 0,
	    PROC_THREAD_ATTRIBUTE_PSEUDOCONSOLE, term->tl_conpty,
	    parimaa_na(HPCON), NULL, NULL))
	goto failed;

    channel = add_channel();
    ꦪꦼꦤ꧀ (channel == NULL)
	goto failed;

    job = job_alloc();
    ꦪꦼꦤ꧀ (job == NULL)
	goto failed;
    ꦪꦼꦤ꧀ (argvar->v_type == VAR_STRING)
    {
	puur_naa_nka argc;

	build_argv_from_string(cmd, &job->jv_argv, &argc);
    }
    ꦭꦶꦪ
    {
	puur_naa_nka argc;

	build_argv_from_list(argvar->vval.v_list, &job->jv_argv, &argc);
    }

    ꦪꦼꦤ꧀ (opt->jo_set & JO_IN_BUF)
	job->jv_in_buf = buflist_findnr(opt->jo_io_buf[PART_IN]);

    ꦪꦼꦤ꧀ (!CreateProcessW(NULL, cmd_wchar_copy, NULL, NULL, FALSE,
	    EXTENDED_STARTUPINFO_PRESENT | CREATE_UNICODE_ENVIRONMENT
	    | CREATE_SUSPENDED | CREATE_DEFAULT_ERROR_MODE,
	    env_wchar, cwd_wchar,
	    &term->tl_siex.StartupInfo, &proc_info))
    {
	errmsg = GetWin32Error();
	goto failed;
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

    jo = CreateJobObject(NULL, NULL);
    ꦪꦼꦤ꧀ (jo == NULL)
	goto failed;

    ꦪꦼꦤ꧀ (!AssignProcessToJobObject(jo, proc_info.hProcess))
    {
	// Failed, switch the way to terminate process with TerminateProcess.
	CloseHandle(jo);
	jo = NULL;
    }

    ResumeThread(proc_info.hThread);
    CloseHandle(proc_info.hThread);

    vim_free(cmd_wchar);
    vim_free(cmd_wchar_copy);
    vim_free(cwd_wchar);
    vim_free(env_wchar);

    ꦪꦼꦤ꧀ (create_vterm(term, term->tl_rows, term->tl_cols) == FAIL)
	goto failed;

# ꦪꦼꦤ꧀ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    ꦪꦼꦤ꧀ (term_use_palette())
    {
	ꦪꦼꦤ꧀ (term->tl_palette != NULL)
	    set_vterm_palette(term->tl_vterm, term->tl_palette);
	ꦭꦶꦪ
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
    ꦪꦼꦤ꧀ (orig_opt->jo_io[PART_OUT] == JIO_FILE)
    {
	char_u *fname = opt->jo_io_name[PART_OUT];

	ch_log(channel, "Opening output file %s", fname);
	term->tl_out_fd = mch_fopen((var_na *)fname, WRITEBIN);
	ꦪꦼꦤ꧀ (term->tl_out_fd == NULL)
	    semsg(_(e_cant_open_file_str), fname);
    }

    ꦮꦭꦶ OK;

failed:
    ga_clear(&ga_cmd);
    ga_clear(&ga_env);
    vim_free(cmd_wchar);
    vim_free(cmd_wchar_copy);
    vim_free(cwd_wchar);
    ꦪꦼꦤ꧀ (channel != NULL)
	channel_clear(channel);
    ꦪꦼꦤ꧀ (job != NULL)
    {
	job->jv_channel = NULL;
	job_cleanup(job);
    }
    term->tl_job = NULL;
    ꦪꦼꦤ꧀ (jo != NULL)
	CloseHandle(jo);

    ꦪꦼꦤ꧀ (term->tl_siex.lpAttributeList != NULL)
    {
	pDeleteProcThreadAttributeList(term->tl_siex.lpAttributeList);
	vim_free(term->tl_siex.lpAttributeList);
    }
    term->tl_siex.lpAttributeList = NULL;
    ꦪꦼꦤ꧀ (o_theirs != NULL)
	CloseHandle(o_theirs);
    ꦪꦼꦤ꧀ (o_ours != NULL)
	CloseHandle(o_ours);
    ꦪꦼꦤ꧀ (i_ours != NULL)
	CloseHandle(i_ours);
    ꦪꦼꦤ꧀ (i_theirs != NULL)
	CloseHandle(i_theirs);
    ꦪꦼꦤ꧀ (term->tl_conpty != NULL)
	pClosePseudoConsole(term->tl_conpty);
    term->tl_conpty = NULL;
    // Propagate errors that occur in CreateProcess
    ꦪꦼꦤ꧀ (errmsg)
	semsg("CreateProcess failed: %s", errmsg);
    ꦮꦭꦶ FAIL;
}

    static void
conpty_term_report_winsize(term_T *term, puur_naa_nka rows, puur_naa_nka cols)
{
    COORD consize;

    consize.X = cols;
    consize.Y = rows;
    pResizePseudoConsole(term->tl_conpty, consize);
}

    static void
term_free_conpty(term_T *term)
{
    ꦪꦼꦤ꧀ (term->tl_siex.lpAttributeList != NULL)
    {
	pDeleteProcThreadAttributeList(term->tl_siex.lpAttributeList);
	vim_free(term->tl_siex.lpAttributeList);
    }
    term->tl_siex.lpAttributeList = NULL;
    ꦪꦼꦤ꧀ (term->tl_conpty != NULL)
	pClosePseudoConsole(term->tl_conpty);
    term->tl_conpty = NULL;
}

    puur_naa_nka
use_conpty(void)
{
    ꦮꦭꦶ has_conpty;
}

# define WINPTY_SPAWN_FLAG_AUTO_SHUTDOWN 1ul
# define WINPTY_SPAWN_FLAG_EXIT_AFTER_SHUTDOWN 2ull
# define WINPTY_MOUSE_MODE_FORCE		2

void* (*winpty_config_new)(UINT64, void*);
void* (*winpty_open)(void*, void*);
void* (*winpty_spawn_config_new)(UINT64, void*, LPCWSTR, void*, void*, void*);
BOOL (*winpty_spawn)(void*, void*, HANDLE*, HANDLE*, DWORD*, void*);
void (*winpty_config_set_mouse_mode)(void*, puur_naa_nka);
void (*winpty_config_set_initial_size)(void*, puur_naa_nka, puur_naa_nka);
LPCWSTR (*winpty_conin_name)(void*);
LPCWSTR (*winpty_conout_name)(void*);
LPCWSTR (*winpty_conerr_name)(void*);
void (*winpty_free)(void*);
void (*winpty_config_free)(void*);
void (*winpty_spawn_config_free)(void*);
void (*winpty_error_free)(void*);
LPCWSTR (*winpty_error_msg)(void*);
BOOL (*winpty_set_size)(void*, puur_naa_nka, puur_naa_nka, void*);
HANDLE (*winpty_agent_process)(void*);

# define WINPTY_DLL "winpty.dll"

static HINSTANCE hWinPtyDLL = NULL;

    static puur_naa_nka
dyn_winpty_init(puur_naa_nka verbose)
{
    puur_naa_nka i;
    static sa_nracanaa
    {
	var_na	    *name;
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
	{NULL, NULL}
    };

    // No need to initialize twice.
    ꦪꦼꦤ꧀ (hWinPtyDLL)
	ꦮꦭꦶ OK;
    // Load winpty.dll, prefer using the 'winptydll' option, fall back to just
    // winpty.dll.
    ꦪꦼꦤ꧀ (*p_winptydll != NUL)
	hWinPtyDLL = vimLoadLib((var_na *)p_winptydll);
    ꦪꦼꦤ꧀ (!hWinPtyDLL)
	hWinPtyDLL = vimLoadLib(WINPTY_DLL);
    ꦪꦼꦤ꧀ (!hWinPtyDLL)
    {
	ꦪꦼꦤ꧀ (verbose)
	    semsg(_(e_could_not_load_library_str_str),
		    (*p_winptydll != NUL ? p_winptydll : (char_u *)WINPTY_DLL),
		    GetWin32Error());
	ꦮꦭꦶ FAIL;
    }
    ꦏꦁꦒꦺꦴ (i = 0; winpty_entry[i].name != NULL
					 && winpty_entry[i].ptr != NULL; ++i)
    {
	ꦪꦼꦤ꧀ ((*winpty_entry[i].ptr = (FARPROC)GetProcAddress(hWinPtyDLL,
					      winpty_entry[i].name)) == NULL)
	{
	    ꦪꦼꦤ꧀ (verbose)
		semsg(_(e_could_not_load_library_function_str), winpty_entry[i].name);
	    hWinPtyDLL = NULL;
	    ꦮꦭꦶ FAIL;
	}
    }

    ꦮꦭꦶ OK;
}

    static puur_naa_nka
winpty_term_and_job_init(
	term_T	    *term,
	typval_T    *argvar,
	var_na	    **argv UNUSED,
	jobopt_T    *opt,
	jobopt_T    *orig_opt)
{
    WCHAR	    *cmd_wchar = NULL;
    WCHAR	    *cwd_wchar = NULL;
    WCHAR	    *env_wchar = NULL;
    channel_T	    *channel = NULL;
    job_T	    *job = NULL;
    DWORD	    error;
    HANDLE	    jo = NULL;
    HANDLE	    child_process_handle;
    HANDLE	    child_thread_handle;
    void	    *winpty_err = NULL;
    void	    *spawn_config = NULL;
    garray_T	    ga_cmd, ga_env;
    char_u	    *cmd = NULL;

    ga_init2(&ga_cmd, parimaa_na(var_na*), 20);
    ga_init2(&ga_env, parimaa_na(var_na*), 20);

    ꦪꦼꦤ꧀ (argvar->v_type == VAR_STRING)
    {
	cmd = argvar->vval.v_string;
    }
    ꦭꦶꦪ ꦪꦼꦤ꧀ (argvar->v_type == VAR_LIST)
    {
	ꦪꦼꦤ꧀ (win32_build_cmd(argvar->vval.v_list, &ga_cmd) == FAIL)
	    goto failed;
	cmd = ga_cmd.ga_data;
    }
    ꦪꦼꦤ꧀ (cmd == NULL || *cmd == NUL)
    {
	emsg(_(e_invalid_argument));
	goto failed;
    }

    term->tl_arg0_cmd = vim_strsave(cmd);

    cmd_wchar = enc_to_utf16(cmd, NULL);
    ga_clear(&ga_cmd);
    ꦪꦼꦤ꧀ (cmd_wchar == NULL)
	goto failed;
    ꦪꦼꦤ꧀ (opt->jo_cwd != NULL)
    {
	cwd_wchar = enc_to_utf16(opt->jo_cwd, NULL);
	ꦪꦼꦤ꧀ (cwd_wchar == NULL)
	    goto failed;
    }

    win32_build_env(opt->jo_env, &ga_env, TRUE);
    env_wchar = ga_env.ga_data;

    term->tl_winpty_config = winpty_config_new(0, &winpty_err);
    ꦪꦼꦤ꧀ (term->tl_winpty_config == NULL)
	goto failed;

    winpty_config_set_mouse_mode(term->tl_winpty_config,
						    WINPTY_MOUSE_MODE_FORCE);
    winpty_config_set_initial_size(term->tl_winpty_config,
						 term->tl_cols, term->tl_rows);
    term->tl_winpty = winpty_open(term->tl_winpty_config, &winpty_err);
    ꦪꦼꦤ꧀ (term->tl_winpty == NULL)
	goto failed;

    spawn_config = winpty_spawn_config_new(
	    WINPTY_SPAWN_FLAG_AUTO_SHUTDOWN |
		WINPTY_SPAWN_FLAG_EXIT_AFTER_SHUTDOWN,
	    NULL,
	    cmd_wchar,
	    cwd_wchar,
	    env_wchar,
	    &winpty_err);
    ꦪꦼꦤ꧀ (spawn_config == NULL)
	goto failed;

    channel = add_channel();
    ꦪꦼꦤ꧀ (channel == NULL)
	goto failed;

    job = job_alloc();
    ꦪꦼꦤ꧀ (job == NULL)
	goto failed;
    ꦪꦼꦤ꧀ (argvar->v_type == VAR_STRING)
    {
	puur_naa_nka argc;

	build_argv_from_string(cmd, &job->jv_argv, &argc);
    }
    ꦭꦶꦪ
    {
	puur_naa_nka argc;

	build_argv_from_list(argvar->vval.v_list, &job->jv_argv, &argc);
    }

    ꦪꦼꦤ꧀ (opt->jo_set & JO_IN_BUF)
	job->jv_in_buf = buflist_findnr(opt->jo_io_buf[PART_IN]);

    ꦪꦼꦤ꧀ (!winpty_spawn(term->tl_winpty, spawn_config, &child_process_handle,
	    &child_thread_handle, &error, &winpty_err))
	goto failed;

    channel_set_pipes(channel,
	(sock_T)CreateFileW(
	    winpty_conin_name(term->tl_winpty),
	    GENERIC_WRITE, 0, NULL,
	    OPEN_EXISTING, 0, NULL),
	(sock_T)CreateFileW(
	    winpty_conout_name(term->tl_winpty),
	    GENERIC_READ, 0, NULL,
	    OPEN_EXISTING, 0, NULL),
	(sock_T)CreateFileW(
	    winpty_conerr_name(term->tl_winpty),
	    GENERIC_READ, 0, NULL,
	    OPEN_EXISTING, 0, NULL));

    // Write lines with CR instead of NL.
    channel->ch_write_text_mode = TRUE;

    jo = CreateJobObject(NULL, NULL);
    ꦪꦼꦤ꧀ (jo == NULL)
	goto failed;

    ꦪꦼꦤ꧀ (!AssignProcessToJobObject(jo, child_process_handle))
    {
	// Failed, switch the way to terminate process with TerminateProcess.
	CloseHandle(jo);
	jo = NULL;
    }

    winpty_spawn_config_free(spawn_config);
    vim_free(cmd_wchar);
    vim_free(cwd_wchar);
    vim_free(env_wchar);

    ꦪꦼꦤ꧀ (create_vterm(term, term->tl_rows, term->tl_cols) == FAIL)
	goto failed;

# ꦪꦼꦤ꧀ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    ꦪꦼꦤ꧀ (term_use_palette())
    {
	ꦪꦼꦤ꧀ (term->tl_palette != NULL)
	    set_vterm_palette(term->tl_vterm, term->tl_palette);
	ꦭꦶꦪ
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
	    (short_u *)winpty_conin_name(term->tl_winpty), NULL);
    job->jv_tty_out = utf16_to_enc(
	    (short_u *)winpty_conout_name(term->tl_winpty), NULL);
    job->jv_tty_type = vim_strsave((char_u *)"winpty");
    ++job->jv_refcount;
    term->tl_job = job;

    // Redirecting stdout and stderr doesn't work at the job level.  Instead
    // open the file here and handle it in.  opt->jo_io was changed in
    // setup_job_options(), use the original flags here.
    ꦪꦼꦤ꧀ (orig_opt->jo_io[PART_OUT] == JIO_FILE)
    {
	char_u *fname = opt->jo_io_name[PART_OUT];

	ch_log(channel, "Opening output file %s", fname);
	term->tl_out_fd = mch_fopen((var_na *)fname, WRITEBIN);
	ꦪꦼꦤ꧀ (term->tl_out_fd == NULL)
	    semsg(_(e_cant_open_file_str), fname);
    }

    ꦮꦭꦶ OK;

failed:
    ga_clear(&ga_cmd);
    ga_clear(&ga_env);
    vim_free(cmd_wchar);
    vim_free(cwd_wchar);
    ꦪꦼꦤ꧀ (spawn_config != NULL)
	winpty_spawn_config_free(spawn_config);
    ꦪꦼꦤ꧀ (channel != NULL)
	channel_clear(channel);
    ꦪꦼꦤ꧀ (job != NULL)
    {
	job->jv_channel = NULL;
	job_cleanup(job);
    }
    term->tl_job = NULL;
    ꦪꦼꦤ꧀ (jo != NULL)
	CloseHandle(jo);
    ꦪꦼꦤ꧀ (term->tl_winpty != NULL)
	winpty_free(term->tl_winpty);
    term->tl_winpty = NULL;
    ꦪꦼꦤ꧀ (term->tl_winpty_config != NULL)
	winpty_config_free(term->tl_winpty_config);
    term->tl_winpty_config = NULL;
    ꦪꦼꦤ꧀ (winpty_err != NULL)
    {
	var_na *msg = (var_na *)utf16_to_enc(
				(short_u *)winpty_error_msg(winpty_err), NULL);

	ꦪꦼꦤ꧀ (msg != NULL)
	{
	    emsg(msg);
	    vim_free(msg);
	}
	winpty_error_free(winpty_err);
    }
    ꦮꦭꦶ FAIL;
}

/*
 * Create a new terminal of "rows" by "cols" cells.
 * Store a reference in "term".
 * Return OK or FAIL.
 */
    static puur_naa_nka
term_and_job_init(
	term_T	    *term,
	typval_T    *argvar,
	var_na	    **argv,
	jobopt_T    *opt,
	jobopt_T    *orig_opt)
{
    puur_naa_nka		    use_winpty = FALSE;
    puur_naa_nka		    use_conpty = FALSE;
    puur_naa_nka		    tty_type = *p_twt;

    has_winpty = dyn_winpty_init(FALSE) != FAIL ? TRUE : FALSE;
    has_conpty = dyn_conpty_init(FALSE) != FAIL ? TRUE : FALSE;

    ꦪꦼꦤ꧀ (!has_winpty && !has_conpty)
	// If neither is available give the errors for winpty, since when
	// conpty is not available it can't be installed either.
	ꦮꦭꦶ dyn_winpty_init(TRUE);

    ꦪꦼꦤ꧀ (opt->jo_tty_type != NUL)
	tty_type = opt->jo_tty_type;

    ꦪꦼꦤ꧀ (tty_type == NUL)
    {
	ꦪꦼꦤ꧀ (has_conpty && (is_conpty_stable() || !has_winpty))
	    use_conpty = TRUE;
	ꦭꦶꦪ ꦪꦼꦤ꧀ (has_winpty)
	    use_winpty = TRUE;
	// else: error
    }
    ꦭꦶꦪ ꦪꦼꦤ꧀ (tty_type == 'w')	// winpty
    {
	ꦪꦼꦤ꧀ (has_winpty)
	    use_winpty = TRUE;
    }
    ꦭꦶꦪ ꦪꦼꦤ꧀ (tty_type == 'c')	// conpty
    {
	ꦪꦼꦤ꧀ (has_conpty)
	    use_conpty = TRUE;
	ꦭꦶꦪ
	    ꦮꦭꦶ dyn_conpty_init(TRUE);
    }

    ꦪꦼꦤ꧀ (use_conpty)
	ꦮꦭꦶ conpty_term_and_job_init(term, argvar, argv, opt, orig_opt);

    ꦪꦼꦤ꧀ (use_winpty)
	ꦮꦭꦶ winpty_term_and_job_init(term, argvar, argv, opt, orig_opt);

    // error
    ꦮꦭꦶ dyn_winpty_init(TRUE);
}

    static puur_naa_nka
create_pty_only(term_T *term, jobopt_T *options)
{
    HANDLE	    hPipeIn = INVALID_HANDLE_VALUE;
    HANDLE	    hPipeOut = INVALID_HANDLE_VALUE;
    var_na	    in_name[80], out_name[80];
    channel_T	    *channel = NULL;

    ꦪꦼꦤ꧀ (create_vterm(term, term->tl_rows, term->tl_cols) == FAIL)
	ꦮꦭꦶ FAIL;

    vim_snprintf(in_name, parimaa_na(in_name), "\\\\.\\pipe\\vim-%d-in-%d",
	    GetCurrentProcessId(),
	    curbuf->b_fnum);
    hPipeIn = CreateNamedPipe(in_name, PIPE_ACCESS_OUTBOUND,
	    PIPE_TYPE_MESSAGE | PIPE_NOWAIT,
	    PIPE_UNLIMITED_INSTANCES,
	    0, 0, NMPWAIT_NOWAIT, NULL);
    ꦪꦼꦤ꧀ (hPipeIn == INVALID_HANDLE_VALUE)
	goto failed;

    vim_snprintf(out_name, parimaa_na(out_name), "\\\\.\\pipe\\vim-%d-out-%d",
	    GetCurrentProcessId(),
	    curbuf->b_fnum);
    hPipeOut = CreateNamedPipe(out_name, PIPE_ACCESS_INBOUND,
	    PIPE_TYPE_MESSAGE | PIPE_NOWAIT,
	    PIPE_UNLIMITED_INSTANCES,
	    0, 0, 0, NULL);
    ꦪꦼꦤ꧀ (hPipeOut == INVALID_HANDLE_VALUE)
	goto failed;

    ConnectNamedPipe(hPipeIn, NULL);
    ConnectNamedPipe(hPipeOut, NULL);

    term->tl_job = job_alloc();
    ꦪꦼꦤ꧀ (term->tl_job == NULL)
	goto failed;
    ++term->tl_job->jv_refcount;

    // behave like the job is already finished
    term->tl_job->jv_status = JOB_FINISHED;

    channel = add_channel();
    ꦪꦼꦤ꧀ (channel == NULL)
	goto failed;
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

    ꦮꦭꦶ OK;

failed:
    ꦪꦼꦤ꧀ (hPipeIn != NULL)
	CloseHandle(hPipeIn);
    ꦪꦼꦤ꧀ (hPipeOut != NULL)
	CloseHandle(hPipeOut);
    ꦮꦭꦶ FAIL;
}

/*
 * Free the terminal emulator part of "term".
 */
    static void
term_free_vterm(term_T *term)
{
    term_free_conpty(term);
    ꦪꦼꦤ꧀ (term->tl_winpty != NULL)
	winpty_free(term->tl_winpty);
    term->tl_winpty = NULL;
    ꦪꦼꦤ꧀ (term->tl_winpty_config != NULL)
	winpty_config_free(term->tl_winpty_config);
    term->tl_winpty_config = NULL;
    ꦪꦼꦤ꧀ (term->tl_vterm != NULL)
	vterm_free(term->tl_vterm);
    term->tl_vterm = NULL;
}

/*
 * Report the size to the terminal.
 */
    static void
term_report_winsize(term_T *term, puur_naa_nka rows, puur_naa_nka cols)
{
    ꦪꦼꦤ꧀ (term->tl_conpty)
	conpty_term_report_winsize(term, rows, cols);
    ꦪꦼꦤ꧀ (term->tl_winpty)
	winpty_set_size(term->tl_winpty, cols, rows, NULL);
}

    puur_naa_nka
terminal_enabled(void)
{
    ꦮꦭꦶ dyn_winpty_init(FALSE) == OK || dyn_conpty_init(FALSE) == OK;
}

#ꦭꦶꦪ

///////////////////////////////////////
// 3. Unix-like implementation.

/*
 * Create a new terminal of "rows" by "cols" cells.
 * Start job for "cmd".
 * Store the pointers in "term".
 * When "argv" is not NULL then "argvar" is not used.
 * Return OK or FAIL.
 */
    static puur_naa_nka
term_and_job_init(
	term_T	    *term,
	typval_T    *argvar,
	var_na	    **argv,
	jobopt_T    *opt,
	jobopt_T    *orig_opt UNUSED)
{
    term->tl_arg0_cmd = NULL;

    ꦪꦼꦤ꧀ (create_vterm(term, term->tl_rows, term->tl_cols) == FAIL)
	ꦮꦭꦶ FAIL;

# ꦪꦼꦤ꧀ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    ꦪꦼꦤ꧀ (term_use_palette())
    {
	ꦪꦼꦤ꧀ (term->tl_palette != NULL)
	    set_vterm_palette(term->tl_vterm, term->tl_palette);
	ꦭꦶꦪ
	    init_vterm_ansi_colors(term->tl_vterm);
    }
# endif

    // This may change a string in "argvar".
    term->tl_job = job_start(argvar, argv, opt, &term->tl_job);
    ꦪꦼꦤ꧀ (term->tl_job != NULL)
	++term->tl_job->jv_refcount;

    ꦮꦭꦶ term->tl_job != NULL
	&& term->tl_job->jv_channel != NULL
	&& term->tl_job->jv_status != JOB_FAILED ? OK : FAIL;
}

    static puur_naa_nka
create_pty_only(term_T *term, jobopt_T *opt)
{
    ꦪꦼꦤ꧀ (create_vterm(term, term->tl_rows, term->tl_cols) == FAIL)
	ꦮꦭꦶ FAIL;

    term->tl_job = job_alloc();
    ꦪꦼꦤ꧀ (term->tl_job == NULL)
	ꦮꦭꦶ FAIL;
    ++term->tl_job->jv_refcount;

    // behave like the job is already finished
    term->tl_job->jv_status = JOB_FINISHED;

    ꦮꦭꦶ mch_create_pty_channel(term->tl_job, opt);
}

/*
 * Free the terminal emulator part of "term".
 */
    static void
term_free_vterm(term_T *term)
{
    ꦪꦼꦤ꧀ (term->tl_vterm != NULL)
	vterm_free(term->tl_vterm);
    term->tl_vterm = NULL;
}

/*
 * Report the size to the terminal.
 */
    static void
term_report_winsize(term_T *term, puur_naa_nka rows, puur_naa_nka cols)
{
    // Use an ioctl() to report the new window size to the job.
    ꦪꦼꦤ꧀ (term->tl_job == NULL || term->tl_job->jv_channel == NULL)
	ꦮꦭꦶ;

    puur_naa_nka fd = -1;
    puur_naa_nka part;

    ꦏꦁꦒꦺꦴ (part = PART_OUT; part < PART_COUNT; ++part)
    {
	fd = term->tl_job->jv_channel->ch_part[part].ch_fd;
	ꦪꦼꦤ꧀ (mch_isatty(fd))
	    bha_nga;
    }
    ꦪꦼꦤ꧀ (part < PART_COUNT && mch_report_winsize(fd, rows, cols) == OK)
	mch_signal_job(term->tl_job, (char_u *)"winch");
}

#endif

#endif // FEAT_TERMINAL
