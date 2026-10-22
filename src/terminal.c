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

#અન્તર્ભાવ "vim.h"

#જો defined(FEAT_TERMINAL)

#ifndef MIN
# define MIN(x,y) ((x) < (y) ? (x) : (y))
#endif
#ifndef MAX
# define MAX(x,y) ((x) > (y) ? (x) : (y))
#endif

#અન્તર્ભાવ "libvterm/include/vterm.h"

// This is VTermScreenCell without the characters, thus much smaller.
પ્રકારવ્યાખ્યા સંરચના {
    VTermScreenCellAttrs	attrs;
    અક્ષર			width;
    VTermColor			fg;
    VTermColor			bg;
} cellattr_T;

પ્રકારવ્યાખ્યા સંરચના sb_લીટી_S {
    પૂર્ણાંક		sb_cols;	// can differ per line
    પૂર્ણાંક		sb_bytes;	// length in bytes of text
    cellattr_T	*sb_cells;	// allocated
    cellattr_T	sb_fill_attr;	// for short line
    char_u	*sb_text;	// for tl_scrollback_postponed
    char_u	continuation;
} sb_લીટી_T;

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
પ્રકારવ્યાખ્યા સંરચના _DYN_STARTUPINFOEXW
{
    STARTUPINFOW StartupInfo;
    LPPROC_THREAD_ATTRIBUTE_LIST lpAttributeList;
} DYN_STARTUPINFOEXW, *PDYN_STARTUPINFOEXW;
#endif

// typedef term_T in structs.h
સંરચના terminal_S {
    term_T	*tl_next;

    VTerm	*tl_vterm;
    job_T	*tl_job;
    buf_T	*tl_buffer;
#જો defined(FEAT_GUI)
    પૂર્ણાંક		tl_system;	// when non-zero used for :!cmd output
    પૂર્ણાંક		tl_toprow;	// row with first line of system terminal
#endif

    // Set when setting the size of a vterm, reset after redrawing.
    પૂર્ણાંક		tl_vterm_size_changed;

    પૂર્ણાંક		tl_normal_mode; // TRUE: Terminal-Normal mode
    પૂર્ણાંક		tl_channel_closing;
    પૂર્ણાંક		tl_channel_closed;
    પૂર્ણાંક		tl_channel_recently_closed; // still need to handle tl_finish

    પૂર્ણાંક		tl_finish;
#define TL_FINISH_UNSET	    NUL
#define TL_FINISH_CLOSE	    'c'	// ++close or :terminal without argument
#define TL_FINISH_NOCLOSE   'n'	// ++noclose
#define TL_FINISH_OPEN	    'o'	// ++open
    char_u	*tl_opencmd;
    char_u	*tl_eof_chars;
    char_u	*tl_api;	// prefix for terminal API function

    char_u	*tl_arg0_cmd;	// To format the status bar

#ifdef MSWIN
    શૂન્ય	*tl_winpty_config;
    શૂન્ય	*tl_winpty;

    HPCON	tl_conpty;
    DYN_STARTUPINFOEXW tl_siex;	// Structure that always needs to be hold

    FILE	*tl_out_fd;
#endif
#જો defined(FEAT_SESSION)
    char_u	*tl_command;
#endif
    char_u	*tl_kill;

    // last known vterm size
    પૂર્ણાંક		tl_rows;
    પૂર્ણાંક		tl_cols;

    char_u	*tl_title; // NULL or allocated
    char_u	*tl_status_text; // NULL or allocated

    // Range of screen rows to update.  Zero based.
    પૂર્ણાંક		tl_dirty_row_start; // MAX_ROW if nothing dirty
    પૂર્ણાંક		tl_dirty_row_end;   // row below last one to update
    પૂર્ણાંક		tl_dirty_snapshot;  // text updated after making snapshot
#ifdef FEAT_TIMERS
    પૂર્ણાંક		tl_timer_set;
    proftime_T	tl_timer_due;
#endif
    પૂર્ણાંક		tl_postponed_scroll;	// to be scrolled up

    garray_T	tl_scrollback;
    પૂર્ણાંક		tl_scrollback_scrolled;
    garray_T	tl_scrollback_postponed;
    પૂર્ણાંક		tl_scrollback_snapshot;
    પૂર્ણાંક		tl_buffer_scrolled;

    char_u	*tl_highlight_name; // replaces "Terminal"; allocated

    cellattr_T	tl_default_color;

    linenr_T	tl_ટોચ_diff_rows;   // rows of top diff file or zero
    linenr_T	tl_bot_diff_rows;   // rows of bottom diff file

    VTermPos	tl_cursor_pos;
    પૂર્ણાંક		tl_cursor_visible;
    પૂર્ણાંક		tl_cursor_blink;
    પૂર્ણાંક		tl_cursor_shape;  // 1: block, 2: underline, 3: bar
    char_u	*tl_cursor_color; // NULL or allocated

    long_u	*tl_palette; // array of 16 colors specified by term_start, can
			     // be NULL
    પૂર્ણાંક		tl_using_altscreen;
    garray_T	tl_osc_buf;	    // incomplete OSC string
};

#define TMODE_ONCE 1	    // CTRL-\ CTRL-N used
#define TMODE_LOOP 2	    // CTRL-W N used

/*
 * List of all active terminals.
 */
સ્થિર term_T *first_term = ખાલી;

// Terminal active in terminal_loop().
સ્થિર term_T *in_terminal_loop = ખાલી;

#ifdef MSWIN
સ્થિર BOOL has_winpty = FALSE;
સ્થિર BOOL has_conpty = FALSE;
#endif

#define MAX_ROW 999999	    // used for tl_dirty_row_end to update all rows
#define ચાવી_BUF_LEN 200

#define FOR_ALL_TERMS(term)	\
    માટે ((term) = first_term; (term) != ખાલી; (term) = (term)->tl_next)

/*
 * Functions with separate implementation for MS-Windows and Unix-like systems.
 */
સ્થિર પૂર્ણાંક term_and_job_init(term_T *term, typval_T *argvar, અક્ષર **argv, jobopt_T *opt, jobopt_T *orig_opt);
સ્થિર પૂર્ણાંક create_pty_only(term_T *term, jobopt_T *opt);
સ્થિર શૂન્ય term_report_winsize(term_T *term, પૂર્ણાંક rows, પૂર્ણાંક cols);
સ્થિર શૂન્ય term_free_vterm(term_T *term);
#ifdef FEAT_GUI
સ્થિર શૂન્ય update_system_term(term_T *term);
#endif

સ્થિર શૂન્ય handle_postponed_scrollback(term_T *term);

// The character that we know (or assume) that the terminal expects for the
// backspace key.
સ્થિર પૂર્ણાંક term_backspace_char = BS;

// Store the last set and the desired cursor properties, so that we only update
// them when needed.  Doing it unnecessary may result in flicker.
સ્થિર char_u	*last_set_cursor_color = ખાલી;
સ્થિર char_u	*desired_cursor_color = ખાલી;
સ્થિર પૂર્ણાંક	last_set_cursor_shape = -1;
સ્થિર પૂર્ણાંક	desired_cursor_shape = -1;
સ્થિર પૂર્ણાંક	last_set_cursor_blink = -1;
સ્થિર પૂર્ણાંક	desired_cursor_blink = -1;


///////////////////////////////////////
// 1. Generic code for all systems.

    સ્થિર પૂર્ણાંક
cursor_color_equal(char_u *lhs_color, char_u *rhs_color)
{
    જો (lhs_color != ખાલી && rhs_color != ખાલી)
	પાછા STRCMP(lhs_color, rhs_color) == 0;
    પાછા lhs_color == ખાલી && rhs_color == ખાલી;
}

    સ્થિર શૂન્ય
cursor_color_copy(char_u **to_color, char_u *from_color)
{
    // Avoid a free & alloc if the value is already right.
    જો (cursor_color_equal(*to_color, from_color))
	પાછા;
    vim_free(*to_color);
    *to_color = (from_color == ખાલી) ? ખાલી : vim_strsave(from_color);
}

    સ્થિર char_u *
cursor_color_get(char_u *color)
{
    પાછા (color == ખાલી) ? (char_u *)"" : color;
}


/*
 * Parse 'termwinsize' and set "rows" and "cols" for the terminal size in the
 * current window.
 * Sets "rows" and/or "cols" to zero when it should follow the window size.
 * Return TRUE if the size is the minimum size: "24*80".
 */
    સ્થિર પૂર્ણાંક
parse_termwinsize(win_T *wp, પૂર્ણાંક *rows, પૂર્ણાંક *cols)
{
    પૂર્ણાંક	minsize = FALSE;

    *rows = 0;
    *cols = 0;

    જો (*wp->w_p_tws == NUL)
	પાછા FALSE;

    char_u *p = vim_strchr(wp->w_p_tws, 'x');

    // Syntax of value was already checked when it's set.
    જો (p == ખાલી)
    {
	minsize = TRUE;
	p = vim_strchr(wp->w_p_tws, '*');
    }
    *rows = atoi((અક્ષર *)wp->w_p_tws);
    *cols = atoi((અક્ષર *)p + 1);
    જો (*rows > VTERM_MAX_ROWS)
	*rows = VTERM_MAX_ROWS;
    જો (*cols > VTERM_MAX_COLS)
	*cols = VTERM_MAX_COLS;
    પાછા minsize;
}

/*
 * Determine the terminal size from 'termwinsize' and the current window.
 */
    સ્થિર શૂન્ય
set_term_and_win_size(term_T *term, jobopt_T *opt)
{
    પૂર્ણાંક rows, cols;
    પૂર્ણાંક	minsize;

#ifdef FEAT_GUI
    જો (term->tl_system)
    {
	// Use the entire screen (excluding the tabpanel area) for the system
	// commands.  However, it will start at the command line and scroll up
	// as needed, using tl_toprow.
	term->tl_rows = Rows;
	term->tl_cols = topframe->fr_width;
	પાછા;
    }
#endif
    term->tl_rows = curwin->w_height;
    term->tl_cols = curwin->w_width;

    minsize = parse_termwinsize(curwin, &rows, &cols);
    જો (minsize)
    {
	જો (term->tl_rows < rows)
	    term->tl_rows = rows;
	જો (term->tl_cols < cols)
	    term->tl_cols = cols;
    }
    જો ((opt->jo_set2 & JO2_TERM_ROWS))
	term->tl_rows = opt->jo_term_rows;
    નહીંતર જો (rows != 0)
	term->tl_rows = rows;
    જો ((opt->jo_set2 & JO2_TERM_COLS))
	term->tl_cols = opt->jo_term_cols;
    નહીંતર જો (cols != 0)
	term->tl_cols = cols;

    જો (!opt->jo_hidden)
    {
	જો (term->tl_rows != curwin->w_height)
	    win_setheight_win(term->tl_rows, curwin);
	જો (term->tl_cols != curwin->w_width)
	    win_setwidth_win(term->tl_cols, curwin);

	// Set 'winsize' now to avoid a resize at the next redraw.
	જો (!minsize && *curwin->w_p_tws != NUL)
	{
	    char_u buf[100];

	    vim_snprintf((અક્ષર *)buf, 100, "%dx%d",
						 term->tl_rows, term->tl_cols);
	    set_option_કિંમત_give_err((char_u *)"termwinsize",
							   0L, buf, OPT_LOCAL);
	}
    }
}

/*
 * Initialize job options for a terminal job.
 * Caller may overrule some of them.
 */
    શૂન્ય
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
    સ્થિર શૂન્ય
setup_job_options(jobopt_T *opt, પૂર્ણાંક rows, પૂર્ણાંક cols)
{
#ifndef MSWIN
    // Win32: Redirecting the job output won't work, thus always connect stdout
    // here.
    જો (!(opt->jo_set & JO_OUT_IO))
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
    જો (!(opt->jo_set & JO_ERR_IO))
#endif
    {
	// Connect stderr to the terminal.
	opt->jo_io[PART_ERR] = JIO_BUFFER;
	opt->jo_io_buf[PART_ERR] = curbuf->b_fnum;
	opt->jo_modifiable[PART_ERR] = 0;
	opt->jo_set |= JO_ERR_IO + JO_ERR_BUF + JO_ERR_MODIFIABLE;
    }

    opt->jo_pty = TRUE;
    જો ((opt->jo_set2 & JO2_TERM_ROWS) == 0)
	opt->jo_term_rows = rows;
    જો ((opt->jo_set2 & JO2_TERM_COLS) == 0)
	opt->jo_term_cols = cols;
}

/*
 * Flush messages on channels.
 */
    સ્થિર શૂન્ય
term_flush_messages(શૂન્ય)
{
    mch_check_messages();
    parse_queued_messages();
}

/*
 * Close a terminal buffer (and its window).  Used when creating the terminal
 * fails.
 */
    સ્થિર શૂન્ય
term_close_buffer(buf_T *buf, buf_T *old_curbuf)
{
    free_terminal(buf);
    જો (old_curbuf != ખાલી)
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
	અક્ષર	    **argv,
	jobopt_T    *opt,
	પૂર્ણાંક	    flags)
{
    exarg_T	split_ea;
    win_T	*old_curwin = curwin;
    term_T	*term;
    buf_T	*old_curbuf = ખાલી;
    પૂર્ણાંક		res;
    buf_T	*newbuf;
    પૂર્ણાંક		vertical = opt->jo_vertical || (cmdmod.cmod_split & WSP_VERT);
    jobopt_T	orig_opt;  // only partly filled
    pos_T	save_cursor;

    જો (check_restricted() || check_secure())
	પાછા ખાલી;
    જો (cmdwin_type != 0)
    {
	emsg(_(e_cannot_open_terminal_from_command_લીટી_window));
	પાછા ખાલી;
    }

    જો ((opt->jo_set & (JO_IN_IO + JO_OUT_IO + JO_ERR_IO))
					 == (JO_IN_IO + JO_OUT_IO + JO_ERR_IO)
	|| (!(opt->jo_set & JO_OUT_IO) && (opt->jo_set & JO_OUT_BUF))
	|| (!(opt->jo_set & JO_ERR_IO) && (opt->jo_set & JO_ERR_BUF))
	|| (argvar != ખાલી
	    && argvar->v_type == VAR_LIST
	    && argvar->vval.v_list != ખાલી
	    && argvar->vval.v_list->lv_first == &range_list_item))
    {
	emsg(_(e_invalid_argument));
	પાછા ખાલી;
    }

    term = ALLOC_CLEAR_ONE(term_T);
    જો (term == ખાલી)
	પાછા ખાલી;
    term->tl_dirty_row_end = MAX_ROW;
    term->tl_cursor_visible = TRUE;
    term->tl_cursor_shape = VTERM_PROP_CURSORSHAPE_BLOCK;
    term->tl_finish = opt->jo_term_finish;
#ifdef FEAT_GUI
    term->tl_system = (flags & TERM_START_SYSTEM);
#endif
    ga_init2(&term->tl_scrollback, કદ(sb_લીટી_T), 300);
    ga_init2(&term->tl_scrollback_postponed, કદ(sb_લીટી_T), 300);
    ga_init2(&term->tl_osc_buf, કદ(અક્ષર), 300);

    setpcmark();
    CLEAR_FIELD(split_ea);
    જો (opt->jo_curwin)
    {
	// Create a new buffer in the current window.
	જો (!can_abandon(curbuf, flags & TERM_START_FORCEIT))
	{
	    no_write_message();
	    vim_free(term);
	    પાછા ખાલી;
	}
	જો (do_ecmd(0, ખાલી, ખાલી, &split_ea, ECMD_ONE,
		      (buf_hide(curwin->w_buffer) ? ECMD_HIDE : 0)
			  + ((flags & TERM_START_FORCEIT) ? ECMD_FORCEIT : 0),
							       curwin) == FAIL)
	{
	    vim_free(term);
	    પાછા ખાલી;
	}
    }
    નહીંતર જો (opt->jo_hidden || (flags & TERM_START_SYSTEM))
    {
	buf_T *buf;

	// Create a new buffer without a window. Make it the current buffer for
	// a moment to be able to do the initializations.
	buf = buflist_new((char_u *)"", ખાલી, (linenr_T)0,
							 BLN_NEW | BLN_LISTED);
	જો (buf == ખાલી || ml_open(buf) == FAIL)
	{
	    vim_free(term);
	    પાછા ખાલી;
	}
	old_curbuf = curbuf;
	--curbuf->b_nwindows;
	curbuf = buf;
	save_cursor = curwin->w_cursor;
	curwin->w_buffer = buf;
	++curbuf->b_nwindows;
    }
    નહીંતર
    {
	// Open a new window or tab.
	split_ea.cmdidx = CMD_new;
	split_ea.cmd = (char_u *)"new";
	split_ea.arg = (char_u *)"";
	જો (opt->jo_term_rows > 0 && !vertical)
	{
	    split_ea.line2 = opt->jo_term_rows;
	    split_ea.addr_ગણતરી = 1;
	}
	જો (opt->jo_term_cols > 0 && vertical)
	{
	    split_ea.line2 = opt->jo_term_cols;
	    split_ea.addr_ગણતરી = 1;
	}

	પૂર્ણાંક cmod_split_modified = FALSE;
	જો (vertical)
	{
	    જો (!(cmdmod.cmod_split & WSP_VERT))
		cmod_split_modified = TRUE;
	    cmdmod.cmod_split |= WSP_VERT;
	}
	ex_splitview(&split_ea);
	જો (cmod_split_modified)
	    cmdmod.cmod_split &= ~WSP_VERT;
	જો (curwin == old_curwin)
	{
	    // split failed
	    vim_free(term);
	    પાછા ખાલી;
	}
    }
    term->tl_buffer = curbuf;
    curbuf->b_term = term;

    જો (!opt->jo_hidden)
    {
	// Only one size was taken care of with :new, do the other one.  With
	// "curwin" both need to be done.
	જો (opt->jo_term_rows > 0 && (opt->jo_curwin || vertical))
	    win_setheight(opt->jo_term_rows);
	જો (opt->jo_term_cols > 0 && (opt->jo_curwin || !vertical))
	    win_setwidth(opt->jo_term_cols);
    }

    // Link the new terminal in the list of active terminals.
    term->tl_next = first_term;
    first_term = term;

    apply_autocmds(EVENT_BUFFILEPRE, ખાલી, ખાલી, FALSE, curbuf);

    જો (opt->jo_term_name != ખાલી)
    {
	vim_free(curbuf->b_ffname);
	curbuf->b_ffname = vim_strsave(opt->jo_term_name);
    }
    નહીંતર જો (argv != ખાલી)
    {
	vim_free(curbuf->b_ffname);
	curbuf->b_ffname = vim_strsave((char_u *)"!system");
    }
    નહીંતર
    {
	પૂર્ણાંક	i;
	size_t	len;
	char_u	*cmd, *p;

	જો (argvar->v_type == VAR_STRING)
	{
	    cmd = argvar->vval.v_string;
	    જો (cmd == ખાલી)
		cmd = (char_u *)"";
	    નહીંતર જો (STRCMP(cmd, "NONE") == 0)
		cmd = (char_u *)"pty";
	}
	નહીંતર જો (argvar->v_type != VAR_LIST
		|| argvar->vval.v_list == ખાલી
		|| argvar->vval.v_list->lv_len == 0
		|| (cmd = tv_get_string_chk(
			       &argvar->vval.v_list->lv_first->li_tv)) == ખાલી)
	    cmd = (char_u*)"";

	len = STRLEN(cmd) + 10;
	p = alloc(len);

	માટે (i = 0; p != ખાલી; ++i)
	{
	    // Prepend a ! to the command name to avoid the buffer name equals
	    // the executable, otherwise ":w!" would overwrite it.
	    જો (i == 0)
		vim_snprintf((અક્ષર *)p, len, "!%s", cmd);
	    નહીંતર
		vim_snprintf((અક્ષર *)p, len, "!%s (%d)", cmd, i);
	    જો (buflist_findname(p) == ખાલી)
	    {
		vim_free(curbuf->b_ffname);
		curbuf->b_ffname = p;
		તોડો;
	    }
	}
    }
    vim_free(curbuf->b_sfname);
    curbuf->b_sfname = vim_strsave(curbuf->b_ffname);
    curbuf->b_fname = curbuf->b_ffname;

    apply_autocmds(EVENT_BUFFILEPOST, ખાલી, ખાલી, FALSE, curbuf);

    જો (opt->jo_term_opencmd != ખાલી)
	term->tl_opencmd = vim_strsave(opt->jo_term_opencmd);

    જો (opt->jo_eof_chars != ખાલી)
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
    mch_memmove(orig_opt.jo_io, opt->jo_io, કદ(orig_opt.jo_io));
#endif
    setup_job_options(opt, term->tl_rows, term->tl_cols);

    જો (flags & TERM_START_NOJOB)
	પાછા curbuf;

#જો defined(FEAT_SESSION)
    // Remember the command for the session file.
    જો (opt->jo_term_norestore || argv != ખાલી)
	term->tl_command = vim_strsave((char_u *)"NONE");
    નહીંતર જો (argvar->v_type == VAR_STRING)
    {
	char_u	*cmd = argvar->vval.v_string;

	જો (cmd != ખાલી && STRCMP(cmd, p_sh) != 0)
	    term->tl_command = vim_strsave(cmd);
    }
    નહીંતર જો (argvar->v_type == VAR_LIST
	    && argvar->vval.v_list != ખાલી
	    && argvar->vval.v_list->lv_len > 0)
    {
	garray_T	ga;
	listitem_T	*item;

	ga_init2(&ga, 1, 100);
	FOR_ALL_LIST_વસ્તુઓ(argvar->vval.v_list, item)
	{
	    char_u *s = tv_get_string_chk(&item->li_tv);
	    char_u *p;

	    જો (s == ખાલી)
		તોડો;
	    p = vim_strsave_fnameescape(s, VSE_NONE);
	    જો (p == ખાલી)
		તોડો;
	    ga_concat(&ga, p);
	    vim_free(p);
	    ga_append(&ga, ' ');
	}
	જો (item == ખાલી)
	{
	    ga_append(&ga, NUL);
	    term->tl_command = ga.ga_data;
	}
	નહીંતર
	    ga_clear(&ga);
    }
#endif

    જો (opt->jo_term_kill != ખાલી)
    {
	char_u *p = skiptowhite(opt->jo_term_kill);

	term->tl_kill = vim_strnsave(opt->jo_term_kill, p - opt->jo_term_kill);
    }

    જો (opt->jo_term_api != ખાલી)
    {
	char_u *p = skiptowhite(opt->jo_term_api);

	term->tl_api = vim_strnsave(opt->jo_term_api, p - opt->jo_term_api);
    }
    નહીંતર
	term->tl_api = vim_strsave((char_u *)"Tapi_");

    જો (opt->jo_set2 & JO2_TERM_HIGHLIGHT)
	term->tl_highlight_name = vim_strsave(opt->jo_term_highlight);

#જો defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    // Save the user-defined palette, it is only used in GUI (or 'tgc' is on).
    જો (opt->jo_set2 & JO2_ANSI_COLORS)
    {
	term->tl_palette = ALLOC_MULT(long_u, 16);
	જો (term->tl_palette == ખાલી)
	{
	    vim_free(term);
	    પાછા ખાલી;
	}
	memcpy(term->tl_palette, opt->jo_ansi_colors, કદ(long_u) * 16);
    }
#endif

    // System dependent: setup the vterm and maybe start the job in it.
    જો (argv == ખાલી
	    && argvar->v_type == VAR_STRING
	    && argvar->vval.v_string != ખાલી
	    && STRCMP(argvar->vval.v_string, "NONE") == 0)
	res = create_pty_only(term, opt);
    નહીંતર
	res = term_and_job_init(term, argvar, argv, opt, &orig_opt);

    newbuf = curbuf;
    જો (res == OK)
    {
	// Get and remember the size we ended up with.  Update the pty.
	vterm_get_size(term->tl_vterm, &term->tl_rows, &term->tl_cols);
	term_report_winsize(term, term->tl_rows, term->tl_cols);
#ifdef FEAT_GUI
	જો (term->tl_system)
	{
	    // display first line below typed command
	    term->tl_toprow = સંદેશ_row + 1;
	    term->tl_dirty_row_end = 0;
	}
#endif

	// Make sure we don't get stuck on sending keys to the job, it leads to
	// a deadlock if the job is waiting for Vim to read.
	channel_set_nonblock(term->tl_job->jv_channel, PART_IN);

	જો (old_curbuf != ખાલી)
	{
	    --curbuf->b_nwindows;
	    curbuf = old_curbuf;
	    curwin->w_buffer = curbuf;
	    curwin->w_cursor = save_cursor;
	    ++curbuf->b_nwindows;
	}
	નહીંતર જો (vgetc_busy
#ifdef FEAT_TIMERS
		|| timer_busy
#endif
		|| આગત_busy)
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
    નહીંતર
    {
	term_close_buffer(curbuf, old_curbuf);
	પાછા ખાલી;
    }

    apply_autocmds(EVENT_TERMINALOPEN, ખાલી, ખાલી, FALSE, newbuf);
    જો (!opt->jo_hidden && !(flags & TERM_START_SYSTEM))
	apply_autocmds(EVENT_TERMINALWINOPEN, ખાલી, ખાલી, FALSE, newbuf);
    પાછા newbuf;
}

/*
 * ":terminal": open a terminal window and execute a job in it.
 */
    શૂન્ય
ex_terminal(exarg_T *eap)
{
    typval_T	argvar[2];
    jobopt_T	opt;
    પૂર્ણાંક		opt_shell = FALSE;
    char_u	*cmd;
    char_u	*tofree = ખાલી;
    પૂર્ણાંક		scroll_save = સંદેશ_scroll;

    સંદેશ_scroll = FALSE;		// don't scroll here
    autowrite_all();
    સંદેશ_scroll = scroll_save;

    init_job_options(&opt);

    cmd = eap->arg;
    જ્યારે (*cmd == '+' && *(cmd + 1) == '+')
    {
	char_u  *p, *ep;

	cmd += 2;
	p = skiptowhite(cmd);
	ep = vim_strchr(cmd, '=');
	જો (ep != ખાલી)
	{
	    જો (ep < p)
		p = ep;
	    નહીંતર
		ep = ખાલી;
	}

	// Note: Keep this in sync with get_terminalopt_name.

#define OPTARG_HAS(name) ((પૂર્ણાંક)(p - cmd) == કદ(name) - 1 \
				 && STRNICMP(cmd, name, કદ(name) - 1) == 0)
	જો (OPTARG_HAS("close"))
	    opt.jo_term_finish = 'c';
	નહીંતર જો (OPTARG_HAS("noclose"))
	    opt.jo_term_finish = 'n';
	નહીંતર જો (OPTARG_HAS("open"))
	    opt.jo_term_finish = 'o';
	નહીંતર જો (OPTARG_HAS("curwin"))
	    opt.jo_curwin = 1;
	નહીંતર જો (OPTARG_HAS("hidden"))
	    opt.jo_hidden = 1;
	નહીંતર જો (OPTARG_HAS("norestore"))
	    opt.jo_term_norestore = 1;
	નહીંતર જો (OPTARG_HAS("shell"))
	    opt_shell = TRUE;
	નહીંતર જો (OPTARG_HAS("kill") && ep != ખાલી)
	{
	    opt.jo_set2 |= JO2_TERM_KILL;
	    opt.jo_term_kill = ep + 1;
	    p = skiptowhite(cmd);
	}
	નહીંતર જો (OPTARG_HAS("api"))
	{
	    opt.jo_set2 |= JO2_TERM_API;
	    જો (ep != ખાલી)
	    {
		opt.jo_term_api = ep + 1;
		p = skiptowhite(cmd);
	    }
	    નહીંતર
		opt.jo_term_api = ખાલી;
	}
	નહીંતર જો (OPTARG_HAS("rows") && ep != ખાલી && SAFE_isdigit(ep[1]))
	{
	    opt.jo_set2 |= JO2_TERM_ROWS;
	    opt.jo_term_rows = atoi((અક્ષર *)ep + 1);
	    p = skiptowhite(cmd);
	}
	નહીંતર જો (OPTARG_HAS("cols") && ep != ખાલી && SAFE_isdigit(ep[1]))
	{
	    opt.jo_set2 |= JO2_TERM_COLS;
	    opt.jo_term_cols = atoi((અક્ષર *)ep + 1);
	    p = skiptowhite(cmd);
	}
	નહીંતર જો (OPTARG_HAS("eof") && ep != ખાલી)
	{
	    char_u *buf = ખાલી;
	    char_u *ચાવીઓ;

	    vim_free(opt.jo_eof_chars);
	    p = skiptowhite(cmd);
	    *p = NUL;
	    ચાવીઓ = replace_termcodes(ep + 1, &buf, 0,
		    REPTERM_FROM_PART | REPTERM_DO_LT | REPTERM_SPECIAL, ખાલી);
	    opt.jo_set2 |= JO2_EOF_CHARS;
	    opt.jo_eof_chars = vim_strsave(ચાવીઓ);
	    vim_free(buf);
	    *p = ' ';
	}
#ifdef MSWIN
	નહીંતર જો ((પૂર્ણાંક)(p - cmd) == 4 && STRNICMP(cmd, "type", 4) == 0
								 && ep != ખાલી)
	{
	    પૂર્ણાંક tty_type = NUL;

	    p = skiptowhite(cmd);
	    જો (STRNICMP(ep + 1, "winpty", p - (ep + 1)) == 0)
		tty_type = 'w';
	    નહીંતર જો (STRNICMP(ep + 1, "conpty", p - (ep + 1)) == 0)
		tty_type = 'c';
	    નહીંતર
	    {
		semsg(_(e_invalid_કિંમત_for_argument_str), "type");
		જાઓ theend;
	    }
	    opt.jo_set2 |= JO2_TTY_TYPE;
	    opt.jo_tty_type = tty_type;
	}
#endif
	નહીંતર
	{
	    જો (*p)
		*p = NUL;
	    semsg(_(e_invalid_attribute_str), cmd);
	    જાઓ theend;
	}
#undef OPTARG_HAS
	cmd = skipwhite(p);
    }
    જો (*cmd == NUL)
    {
	// Make a copy of 'shell', an autocommand may change the option.
	tofree = cmd = vim_strsave(p_sh);

	// default to close when the shell exits
	જો (opt.jo_term_finish == NUL)
	    opt.jo_term_finish = TL_FINISH_CLOSE;
    }

    જો (eap->addr_ગણતરી > 0)
    {
	// Write lines from current buffer to the job.
	opt.jo_set |= JO_IN_IO | JO_IN_BUF | JO_IN_ટોચ | JO_IN_BOT;
	opt.jo_io[PART_IN] = JIO_BUFFER;
	opt.jo_io_buf[PART_IN] = curbuf->b_fnum;
	opt.jo_in_ટોચ = eap->line1;
	opt.jo_in_bot = eap->line2;
    }

    જો (opt_shell && tofree == ખાલી)
    {
#ifdef UNIX
	અક્ષર	**argv = ખાલી;
	char_u	*tofree1 = ખાલી;
	char_u	*tofree2 = ખાલી;

	// :term ++shell command
	જો (unix_build_argv(cmd, &argv, &tofree1, &tofree2) == OK)
	    term_start(ખાલી, argv, &opt, eap->forceit ? TERM_START_FORCEIT : 0);
	vim_free(argv);
	vim_free(tofree1);
	vim_free(tofree2);
	જાઓ theend;
#નહીંતર
# ifdef MSWIN
	long_u	    cmdlen = STRLEN(p_sh) + STRLEN(p_shcf) + STRLEN(cmd) + 10;
	char_u	    *newcmd;

	newcmd = alloc(cmdlen);
	જો (newcmd == ખાલી)
	    જાઓ theend;
	tofree = newcmd;
	vim_snprintf((અક્ષર *)newcmd, cmdlen, "%s %s %s", p_sh, p_shcf, cmd);
	cmd = newcmd;
# નહીંતર
	emsg(_(e_sorry_plusplusshell_not_supported_on_this_system));
	જાઓ theend;
# endif
#endif
    }
    argvar[0].v_type = VAR_STRING;
    argvar[0].vval.v_string = cmd;
    argvar[1].v_type = VAR_UNKNOWN;
    term_start(argvar, ખાલી, &opt, eap->forceit ? TERM_START_FORCEIT : 0);

theend:
    vim_free(tofree);
    vim_free(opt.jo_eof_chars);
}

    સ્થિર char_u *
get_terminalopt_name(expand_T *xp UNUSED, પૂર્ણાંક idx)
{
    // Note: Keep this in sync with ex_terminal.
    સ્થિર અક્ષર *(p_termopt_કિંમતો[]) =
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

    જો (idx < (પૂર્ણાંક)ARRAY_LENGTH(p_termopt_કિંમતો))
	પાછા (char_u*)p_termopt_કિંમતો[idx];
    પાછા ખાલી;
}

    સ્થિર char_u *
get_termkill_name(expand_T *xp UNUSED, પૂર્ણાંક idx)
{
    // These are platform-specific values used for job_stop(). They are defined
    // in each platform's mch_signal_job(). Just use a unified auto-complete
    // list for simplicity.
    સ્થિર અક્ષર *(p_termkill_કિંમતો[]) =
    {
	"term",
	"hup",
	"quit",
	"int",
	"kill",
	"winch",
    };

    જો (idx < (પૂર્ણાંક)ARRAY_LENGTH(p_termkill_કિંમતો))
	પાછા (char_u*)p_termkill_કિંમતો[idx];
    પાછા ખાલી;
}

/*
 * Command-line expansion for :terminal [options]
 */
    પૂર્ણાંક
expand_terminal_opt(
	char_u	    *pat,
	expand_T    *xp,
	regmatch_T  *rmp,
	char_u	    ***matches,
	પૂર્ણાંક	    *અંક_Matches)
{
    જો (xp->xp_pattern > xp->xp_લીટી && *(xp->xp_pattern-1) == '=')
    {
	char_u *(*cb)(expand_T *, પૂર્ણાંક) = ખાલી;

	char_u *name_end = xp->xp_pattern - 1;
	જો (name_end - xp->xp_લીટી >= 4
		&& STRNCMP(name_end - 4, "kill", 4) == 0)
	    cb = get_termkill_name;

	જો (cb != ખાલી)
	{
	    પાછા ExpandGeneric(
		    pat,
		    xp,
		    rmp,
		    matches,
		    અંક_Matches,
		    cb,
		    FALSE);
	}
	પાછા FAIL;
    }
    પાછા ExpandGeneric(
	    pat,
	    xp,
	    rmp,
	    matches,
	    અંક_Matches,
	    get_terminalopt_name,
	    FALSE);
}

#જો defined(FEAT_SESSION)
/*
 * Write a :terminal command to the session file to restore the terminal in
 * window "wp".
 * Return FAIL if writing fails.
 */
    પૂર્ણાંક
term_write_session(FILE *fd, win_T *wp, hashtab_T *terminal_bufs)
{
    અચલ પૂર્ણાંક	bufnr = wp->w_buffer->b_fnum;
    term_T	*term = wp->w_buffer->b_term;

    જો (terminal_bufs != ખાલી && wp->w_buffer->b_nwindows > 1)
    {
	// There are multiple views into this terminal buffer. We don't want to
	// create the terminal multiple times. If it's the first time, create,
	// otherwise link to the first buffer.
	અક્ષર	    id_as_str[NUMBUFLEN];
	hashitem_T  *entry;

	vim_snprintf(id_as_str, કદ(id_as_str), "%d", bufnr);

	entry = hash_find(terminal_bufs, (char_u *)id_as_str);
	જો (!HASHITEM_EMPTY(entry))
	{
	    // we've already opened this terminal buffer
	    જો (fprintf(fd, "execute 'buffer ' .. term_buf_%d", bufnr) < 0)
		પાછા FAIL;
	    પાછા put_eol(fd);
	}
    }

    // Create the terminal and run the command.  This is not without
    // risk, but let's assume the user only creates a session when this
    // will be OK.
    જો (fprintf(fd, "exe ':terminal ++curwin"
		" ++cols=' .. ((&columns * %d + %ld) / %ld)"
		" .. ' ++rows=' .. ((&lines * %d + %ld) / %ld) ",
		term->tl_cols, Columns / 2, Columns,
		term->tl_rows, Rows / 2, Rows) < 0)
	પાછા FAIL;
# ifdef MSWIN
    જો (fprintf(fd, ".. ' ++type=%s' ", term->tl_job->jv_tty_type) < 0)
	પાછા FAIL;
# endif
    જો (term->tl_command != ખાલી)
    {
	char_u *quoted_command = string_quote(term->tl_command, FALSE);
	જો (quoted_command == ખાલી)
	    પાછા FAIL;

	પૂર્ણાંક ret = fputs(".. ' ' .. ", fd) < 0
		    || fputs((અક્ષર *)quoted_command, fd) < 0;
	vim_free(quoted_command);
	જો (ret)
	    પાછા FAIL;
    }
    જો (put_eol(fd) != OK)
	પાછા FAIL;

    જો (fprintf(fd, "var term_buf_%d: number = bufnr()", bufnr) < 0)
	પાછા FAIL;

    જો (terminal_bufs != ખાલી && wp->w_buffer->b_nwindows > 1)
    {
	અક્ષર *hash_ચાવી = alloc(NUMBUFLEN);

	vim_snprintf(hash_ચાવી, NUMBUFLEN, "%d", bufnr);
	hash_add(terminal_bufs, (char_u *)hash_ચાવી, "terminal session");
    }

    પાછા put_eol(fd);
}

/*
 * Return TRUE if "buf" has a terminal that should be restored.
 */
    પૂર્ણાંક
term_should_restore(buf_T *buf)
{
    term_T	*term = buf->b_term;

    પાછા term != ખાલી && (term->tl_command == ખાલી
				     || STRCMP(term->tl_command, "NONE") != 0);
}
#endif

/*
 * Free the scrollback buffer for "term".
 */
    સ્થિર શૂન્ય
free_scrollback(term_T *term)
{
    પૂર્ણાંક i;

    માટે (i = 0; i < term->tl_scrollback.ga_len; ++i)
	vim_free(((sb_લીટી_T *)term->tl_scrollback.ga_data + i)->sb_cells);
    ga_clear(&term->tl_scrollback);
    માટે (i = 0; i < term->tl_scrollback_postponed.ga_len; ++i)
	vim_free(((sb_લીટી_T *)term->tl_scrollback_postponed.ga_data + i)->sb_cells);
    ga_clear(&term->tl_scrollback_postponed);
}


// Terminals that need to be freed soon.
સ્થિર term_T	*terminals_to_free = ખાલી;

/*
 * Free a terminal and everything it refers to.
 * Kills the job if there is one.
 * Called when wiping out a buffer.
 * The actual terminal structure is freed later in free_unused_terminals(),
 * because callbacks may wipe out a buffer while the terminal is still
 * referenced.
 */
    શૂન્ય
free_terminal(buf_T *buf)
{
    term_T	*term = buf->b_term;
    term_T	*tp;

    જો (term == ખાલી)
	પાછા;

    // Unlink the terminal form the list of terminals.
    જો (first_term == term)
	first_term = term->tl_next;
    નહીંતર
	માટે (tp = first_term; tp->tl_next != ખાલી; tp = tp->tl_next)
	    જો (tp->tl_next == term)
	    {
		tp->tl_next = term->tl_next;
		તોડો;
	    }

    જો (term->tl_job != ખાલી)
    {
	જો (term->tl_job->jv_status != JOB_ENDED
		&& term->tl_job->jv_status != JOB_FINISHED
		&& term->tl_job->jv_status != JOB_FAILED)
	    job_stop(term->tl_job, ખાલી, "kill");
	job_unref(term->tl_job);
    }
    term->tl_next = terminals_to_free;
    terminals_to_free = term;

    buf->b_term = ખાલી;
    જો (in_terminal_loop == term)
	in_terminal_loop = ખાલી;
}

    શૂન્ય
free_unused_terminals(શૂન્ય)
{
    જ્યારે (terminals_to_free != ખાલી)
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
	જો (term->tl_out_fd != ખાલી)
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
    સ્થિર ch_part_T
get_tty_part(term_T *term UNUSED)
{
#ifdef UNIX
    ch_part_T	parts[3] = {PART_IN, PART_OUT, PART_ERR};
    પૂર્ણાંક		i;

    માટે (i = 0; i < 3; ++i)
    {
	પૂર્ણાંક fd = term->tl_job->jv_channel->ch_part[parts[i]].ch_fd;

	જો (mch_isatty(fd))
	    પાછા parts[i];
    }
#endif
    પાછા PART_IN;
}

/*
 * Read any vterm output and send it on the channel.
 */
    સ્થિર શૂન્ય
term_forward_output(term_T *term)
{
    VTerm *vterm = term->tl_vterm;
    અક્ષર   buf[ચાવી_BUF_LEN];
    size_t curlen = vterm_output_read(vterm, buf, ચાવી_BUF_LEN);

    જો (curlen > 0)
	channel_send(term->tl_job->jv_channel, get_tty_part(term),
					     (char_u *)buf, (પૂર્ણાંક)curlen, ખાલી);
}

/*
 * Write job output "msg[len]" to the vterm.
 */
    સ્થિર શૂન્ય
term_write_job_output(term_T *term, char_u *સંદેશ_arg, size_t len_arg)
{
    char_u	*સંદેશ = સંદેશ_arg;
    size_t	len = len_arg;
    VTerm	*vterm = term->tl_vterm;
    size_t	prevlen = vterm_output_get_buffer_current(vterm);
    size_t	limit = term->tl_buffer->b_p_twsl * term->tl_cols * 3;

    // Limit the length to 'termwinscroll' * cols * 3 bytes.  Keep the text at
    // the end.
    જો (len > limit)
    {
	char_u *p = સંદેશ + len - limit;

	p -= (*mb_head_off)(સંદેશ, p);
	len -= p - સંદેશ;
	સંદેશ = p;
    }

    vterm_આગત_write(vterm, (અક્ષર *)સંદેશ, len);

    // flush vterm buffer when vterm responded to control sequence
    જો (prevlen != vterm_output_get_buffer_current(vterm))
	term_forward_output(term);

    // this invokes the damage callbacks
    vterm_screen_flush_damage(vterm_obtain_screen(vterm));
}

    સ્થિર શૂન્ય
position_cursor(win_T *wp, VTermPos *pos)
{
    wp->w_wrow = MIN(pos->row, MAX(0, wp->w_height - 1));
    wp->w_wcol = MIN(pos->col, MAX(0, wp->w_width - 1));
#ifdef FEAT_PROP_POPUP
    જો (popup_is_popup(wp))
    {
	wp->w_wrow += popup_ટોચ_extra(wp);
	wp->w_wcol += popup_ડાબું_extra(wp);
	wp->w_flags |= WFLAG_WCOL_OFF_ADDED | WFLAG_WROW_OFF_ADDED;
    }
    નહીંતર
	wp->w_flags &= ~(WFLAG_WCOL_OFF_ADDED | WFLAG_WROW_OFF_ADDED);
#endif
    wp->w_valid |= (VALID_WCOL|VALID_WROW);
}

    સ્થિર શૂન્ય
update_cursor(term_T *term, પૂર્ણાંક redraw)
{
    જો (term->tl_normal_mode)
	પાછા;
#ifdef FEAT_GUI
    જો (term->tl_system)
	windgoto(term->tl_cursor_pos.row + term->tl_toprow,
		firstwin->w_wincol + term->tl_cursor_pos.col);
    નહીંતર
#endif
    જો (!term_job_running(term))
	// avoid the cursor positioned below the last used line
	setcursor();
    નહીંતર
    {
	// do not use the window cursor position
	position_cursor(curwin, &curbuf->b_term->tl_cursor_pos);
	windgoto(W_WINROW(curwin) + curwin->w_wrow,
		 curwin->w_wincol + curwin->w_wcol);
    }
    જો (redraw)
    {
	aco_save_T	aco;

	જો (term->tl_buffer == curbuf && term->tl_cursor_visible)
	    cursor_on();
	out_flush();
#ifdef FEAT_GUI
	જો (gui.in_use)
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
	apply_autocmds(EVENT_TEXTCHANGEDT, ખાલી, ખાલી, FALSE, term->tl_buffer);
	aucmd_restbuf(&aco);

	--term->tl_buffer->b_locked;
    }
}

/*
 * Find the location of a scrollbackline in the buffer
 */
    સ્થિર શૂન્ય
scrollbackline_pos_in_buf(term_T *term, પૂર્ણાંક row, linenr_T *lnum, પૂર્ણાંક *start_col, size_t *start_pos)
{
    sb_લીટી_T	*લીટીઓ = (sb_લીટી_T *)term->tl_scrollback.ga_data;
    linenr_T    calc_lnum = term->tl_buffer_scrolled;
    size_t      calc_pos = 0;
    પૂર્ણાંક         calc_col = 0;
    પૂર્ણાંક         i;

    જો (row < 0 || row >= term->tl_scrollback.ga_len)
	પાછા;

    જો (row > term->tl_scrollback_scrolled)
    {
	// Lookback how far along in the top line we are
	માટે (i = term->tl_scrollback_scrolled + 1; i > 0 && લીટીઓ[i].continuation; --i)
	{
	    calc_pos += લીટીઓ[i - 1].sb_bytes;
	    calc_col += લીટીઓ[i - 1].sb_cols;
	}
	i = term->tl_scrollback_scrolled + 1;
	calc_lnum = term->tl_buffer_scrolled + 1;
	// Do not count this line's bytes/cols twice
	જો (લીટીઓ[i].continuation)
	    ++i;
    }
    નહીંતર
    {
	i = 1;
	calc_lnum = 1;
    }

    માટે (; i <= row; ++i)
    {
	જો (!લીટીઓ[i].continuation)
	{
	    ++calc_lnum;
	    calc_pos = 0;
	    calc_col = 0;
	}
	નહીંતર
	{
	    calc_pos += લીટીઓ[i - 1].sb_bytes;
	    calc_col += લીટીઓ[i - 1].sb_cols;
	}
    }

    *lnum = calc_lnum;
    જો (start_col)
	*start_col = calc_col;
    જો (start_pos)
	*start_pos = calc_pos;
}

/*
 * Find the location of a buffer line in the scrollback
 */
    સ્થિર શૂન્ય
bufline_pos_in_scrollback(term_T *term, linenr_T lnum, પૂર્ણાંક col, પૂર્ણાંક *row, પૂર્ણાંક *wrapped_col)
{
    buf_T	*buf = term->tl_buffer;
    sb_લીટી_T	*લીટીઓ = (sb_લીટી_T *)term->tl_scrollback.ga_data;
    linenr_T    calc_row = term->tl_scrollback_scrolled;
    પૂર્ણાંક         calc_col = col;
    linenr_T    l;

    જો (lnum > buf->b_ml.ml_લીટી_ગણતરી)
	પાછા;

    જો (lnum > term->tl_buffer_scrolled)
    {
	calc_row = term->tl_scrollback_scrolled;
	l = term->tl_buffer_scrolled + 1;

	જ્યારે (calc_row < term->tl_scrollback.ga_len && લીટીઓ[calc_row].continuation)
	    ++calc_row;
    }
    નહીંતર
    {
	calc_row = 0;
	l = 1;
    }

    જ્યારે (calc_row < term->tl_scrollback.ga_len && l < lnum)
    {
	++calc_row;
	જો (!લીટીઓ[calc_row].continuation)
	    ++l;
    }

    જ્યારે (calc_row + 1 < term->tl_scrollback.ga_len && લીટીઓ[calc_row + 1].continuation
	    && calc_col >= લીટીઓ[calc_row].sb_cols)
    {
	calc_col -= લીટીઓ[calc_row].sb_cols;
	++calc_row;
    }

    જો (row)
	*row = calc_row;
    જો (wrapped_col)
	*wrapped_col = calc_col;
}

/*
 * Invoked when "msg" output from a job was received.  Write it to the terminal
 * of "buffer".
 */
    શૂન્ય
write_to_term(buf_T *buffer, char_u *સંદેશ, channel_T *channel)
{
    size_t	len = STRLEN(સંદેશ);
    term_T	*term = buffer->b_term;

#ifdef MSWIN
    // Win32: Cannot redirect output of the job, intercept it here and write to
    // the file.
    જો (term->tl_out_fd != ખાલી)
    {
	ch_log(channel, "Writing %d bytes to output file", (પૂર્ણાંક)len);
	fwrite(સંદેશ, len, 1, term->tl_out_fd);
	પાછા;
    }
#endif

    જો (term->tl_vterm == ખાલી)
    {
	ch_log(channel, "NOT writing %d bytes to terminal", (પૂર્ણાંક)len);
	પાછા;
    }
    ch_log(channel, "writing %d bytes to terminal", (પૂર્ણાંક)len);
    cursor_off();
    term_write_job_output(term, સંદેશ, len);

#ifdef FEAT_GUI
    જો (term->tl_system)
    {
	// show system output, scrolling up the screen as needed
	update_system_term(term);
	update_cursor(term, TRUE);
    }
    નહીંતર
#endif
    // In Terminal-Normal mode we are displaying the buffer, not the terminal
    // contents, thus no screen update is needed.
    જો (!term->tl_normal_mode)
    {
	// Don't use update_screen() when editing the command line, it gets
	// cleared.
	// TODO: only update once in a while.
	ch_log(term->tl_job->jv_channel, "updating screen");
	જો (buffer == curbuf && (State & MODE_CMDLINE) == 0)
	{
	    update_screen(UPD_VALID_NO_UPDATE);
#જો defined(FEAT_TABPANEL)
	    જો (redraw_tabpanel)
		draw_tabpanel();
#endif
	    // update_screen() can be slow, check the terminal wasn't closed
	    // already
	    જો (buffer == curbuf && curbuf->b_term != ખાલી)
		update_cursor(curbuf->b_term, TRUE);
	}
	નહીંતર
	    redraw_after_callback(TRUE, FALSE);
    }
}

/*
 * Send a mouse position and click to the vterm
 */
    સ્થિર પૂર્ણાંક
term_send_mouse(VTerm *vterm, પૂર્ણાંક button, પૂર્ણાંક pressed)
{
    VTermModifier   mod = VTERM_MOD_NONE;
    પૂર્ણાંક		    row = mouse_row - W_WINROW(curwin);
    પૂર્ણાંક		    col = mouse_col - curwin->w_wincol;

#ifdef FEAT_PROP_POPUP
    જો (popup_is_popup(curwin))
    {
	row -= popup_ટોચ_extra(curwin);
	col -= popup_ડાબું_extra(curwin);
    }
#endif
    vterm_mouse_move(vterm, row, col, mod);
    જો (button != 0)
	vterm_mouse_button(vterm, button, pressed, mod);
    પાછા TRUE;
}

સ્થિર પૂર્ણાંક enter_mouse_col = -1;
સ્થિર પૂર્ણાંક enter_mouse_row = -1;

/*
 * Handle a mouse click, drag or release.
 * Return TRUE when a mouse event is sent to the terminal.
 */
    સ્થિર પૂર્ણાંક
term_mouse_click(VTerm *vterm, પૂર્ણાંક ચાવી)
{
#જો defined(FEAT_CLIPBOARD)
    // For modeless selection mouse drag and release events are ignored, unless
    // they are preceded with a mouse down event
    સ્થિર પૂર્ણાંક	    ignore_drag_release = TRUE;
    VTermState	    *state;
    VTermMouseState mouse_state;


    state = vterm_obtain_state(vterm);
    જો (state == ખાલી)
	પાછા FALSE;

    vterm_state_get_mousestate(state, &mouse_state);
    જો (mouse_state.flags == 0)
    {
	// Terminal is not using the mouse, use modeless selection.
	પસંદગી (ચાવી)
	{
	સ્થિતિ K_LEFTDRAG:
	સ્થિતિ K_LEFTRELEASE:
	સ્થિતિ K_RIGHTDRAG:
	સ્થિતિ K_RIGHTRELEASE:
		// Ignore drag and release events when the button-down wasn't
		// seen before.
		જો (ignore_drag_release)
		{
		    પૂર્ણાંક save_mouse_col, save_mouse_row;

		    જો (enter_mouse_col < 0)
			તોડો;

		    // mouse click in the window gave us focus, handle that
		    // click now
		    save_mouse_col = mouse_col;
		    save_mouse_row = mouse_row;
		    mouse_col = enter_mouse_col;
		    mouse_row = enter_mouse_row;
		    clip_modeless(MOUSE_ડાબું, TRUE, FALSE);
		    mouse_col = save_mouse_col;
		    mouse_row = save_mouse_row;
		}
		// FALLTHROUGH
	સ્થિતિ K_LEFTMOUSE:
	સ્થિતિ K_RIGHTMOUSE:
		જો (ચાવી == K_LEFTRELEASE || ચાવી == K_RIGHTRELEASE)
		    ignore_drag_release = TRUE;
		નહીંતર
		    ignore_drag_release = FALSE;
		// Should we call mouse_has() here?
		જો (clip_star.available)
		{
		    પૂર્ણાંક	    button, is_click, is_drag;

		    button = get_mouse_button(KEY2TERMCAP1(ચાવી),
							 &is_click, &is_drag);
		    જો (mouse_model_popup() && button == MOUSE_ડાબું
					       && (mod_mask & MOD_MASK_SHIFT))
		    {
			// Translate shift-left to right button.
			button = MOUSE_જમણું;
			mod_mask &= ~MOD_MASK_SHIFT;
		    }
		    clip_modeless(button, is_click, is_drag);
		}
		તોડો;

	સ્થિતિ K_MIDDLEMOUSE:
		જો (clip_star.available)
		    ઉમેરો_reg('*', TRUE);
		તોડો;
	}
	enter_mouse_col = -1;
	પાછા FALSE;
    }
#endif
    enter_mouse_col = -1;

    પસંદગી (ચાવી)
    {
	સ્થિતિ K_LEFTMOUSE:
	સ્થિતિ K_LEFTMOUSE_NM:	term_send_mouse(vterm, 1, 1); તોડો;
	સ્થિતિ K_LEFTDRAG:	term_send_mouse(vterm, 1, 1); તોડો;
	સ્થિતિ K_LEFTRELEASE:
	સ્થિતિ K_LEFTRELEASE_NM:	term_send_mouse(vterm, 1, 0); તોડો;
	સ્થિતિ K_MOUSEMOVE:	term_send_mouse(vterm, 0, 0); તોડો;
	સ્થિતિ K_MIDDLEMOUSE:	term_send_mouse(vterm, 2, 1); તોડો;
	સ્થિતિ K_MIDDLEDRAG:	term_send_mouse(vterm, 2, 1); તોડો;
	સ્થિતિ K_MIDDLERELEASE:	term_send_mouse(vterm, 2, 0); તોડો;
	સ્થિતિ K_RIGHTMOUSE:	term_send_mouse(vterm, 3, 1); તોડો;
	સ્થિતિ K_RIGHTDRAG:	term_send_mouse(vterm, 3, 1); તોડો;
	સ્થિતિ K_RIGHTRELEASE:	term_send_mouse(vterm, 3, 0); તોડો;
    }
    પાછા TRUE;
}

/*
 * Convert typed key "c" with modifiers "modmask" into bytes to send to the
 * job.
 * Return the number of bytes in "buf".
 */
    સ્થિર પૂર્ણાંક
term_convert_ચાવી(term_T *term, પૂર્ણાંક c, પૂર્ણાંક modmask, અક્ષર *buf)
{
    VTerm	    *vterm = term->tl_vterm;
    VTerm_ચાવી	    ચાવી = VTERM_ચાવી_NONE;
    VTermModifier   mod = VTERM_MOD_NONE;
    પૂર્ણાંક		    other = FALSE;

    પસંદગી (c)
    {
	// don't use VTERM_KEY_ENTER, it may do an unwanted conversion

	સ્થિતિ K_BS:
#ifdef MSWIN
	    // In ConPTY, we must use VTERM_KEY_BACKSPACE, otherwise it will
	    // delete one word, equivalent to Alt+Backspace.
	    જો (term->tl_conpty)
		ચાવી = VTERM_ચાવી_BACKSPACE;
	    નહીંતર
#endif
		// don't use VTERM_KEY_BACKSPACE, it always becomes 0x7f DEL
		c = term_backspace_char;
	    તોડો;

#ifdef MSWIN
	સ્થિતિ BS:
	    // In ConPTY, we must use VTERM_KEY_BACKSPACE, otherwise it will
	    // delete one word, equivalent to Alt+Backspace.
	    જો (term->tl_conpty)
		ચાવી = VTERM_ચાવી_BACKSPACE;
	    તોડો;
#endif

	સ્થિતિ ESC:		ચાવી = VTERM_ચાવી_ESCAPE; તોડો;
	સ્થિતિ K_DEL:		ચાવી = VTERM_ચાવી_DEL; તોડો;
	સ્થિતિ K_DOWN:		ચાવી = VTERM_ચાવી_DOWN; તોડો;
	સ્થિતિ K_S_DOWN:		mod = VTERM_MOD_SHIFT;
				ચાવી = VTERM_ચાવી_DOWN; તોડો;
	સ્થિતિ K_END:		ચાવી = VTERM_ચાવી_END; તોડો;
	સ્થિતિ K_S_END:		mod = VTERM_MOD_SHIFT;
				ચાવી = VTERM_ચાવી_END; તોડો;
	સ્થિતિ K_C_END:		mod = VTERM_MOD_CTRL;
				ચાવી = VTERM_ચાવી_END; તોડો;
	સ્થિતિ K_F10:		ચાવી = VTERM_ચાવી_FUNCTION(10); તોડો;
	સ્થિતિ K_F11:		ચાવી = VTERM_ચાવી_FUNCTION(11); તોડો;
	સ્થિતિ K_F12:		ચાવી = VTERM_ચાવી_FUNCTION(12); તોડો;
	સ્થિતિ K_F1:		ચાવી = VTERM_ચાવી_FUNCTION(1); તોડો;
	સ્થિતિ K_F2:		ચાવી = VTERM_ચાવી_FUNCTION(2); તોડો;
	સ્થિતિ K_F3:		ચાવી = VTERM_ચાવી_FUNCTION(3); તોડો;
	સ્થિતિ K_F4:		ચાવી = VTERM_ચાવી_FUNCTION(4); તોડો;
	સ્થિતિ K_F5:		ચાવી = VTERM_ચાવી_FUNCTION(5); તોડો;
	સ્થિતિ K_F6:		ચાવી = VTERM_ચાવી_FUNCTION(6); તોડો;
	સ્થિતિ K_F7:		ચાવી = VTERM_ચાવી_FUNCTION(7); તોડો;
	સ્થિતિ K_F8:		ચાવી = VTERM_ચાવી_FUNCTION(8); તોડો;
	સ્થિતિ K_F9:		ચાવી = VTERM_ચાવી_FUNCTION(9); તોડો;
	સ્થિતિ K_HOME:		ચાવી = VTERM_ચાવી_HOME; તોડો;
	સ્થિતિ K_S_HOME:		mod = VTERM_MOD_SHIFT;
				ચાવી = VTERM_ચાવી_HOME; તોડો;
	સ્થિતિ K_C_HOME:		mod = VTERM_MOD_CTRL;
				ચાવી = VTERM_ચાવી_HOME; તોડો;
	સ્થિતિ K_INS:		ચાવી = VTERM_ચાવી_INS; તોડો;
	સ્થિતિ K_K0:		ચાવી = VTERM_ચાવી_KP_0; તોડો;
	સ્થિતિ K_K1:		ચાવી = VTERM_ચાવી_KP_1; તોડો;
	સ્થિતિ K_K2:		ચાવી = VTERM_ચાવી_KP_2; તોડો;
	સ્થિતિ K_K3:		ચાવી = VTERM_ચાવી_KP_3; તોડો;
	સ્થિતિ K_K4:		ચાવી = VTERM_ચાવી_KP_4; તોડો;
	સ્થિતિ K_K5:		ચાવી = VTERM_ચાવી_KP_5; તોડો;
	સ્થિતિ K_K6:		ચાવી = VTERM_ચાવી_KP_6; તોડો;
	સ્થિતિ K_K7:		ચાવી = VTERM_ચાવી_KP_7; તોડો;
	સ્થિતિ K_K8:		ચાવી = VTERM_ચાવી_KP_8; તોડો;
	સ્થિતિ K_K9:		ચાવી = VTERM_ચાવી_KP_9; તોડો;
	સ્થિતિ K_KDEL:		ચાવી = VTERM_ચાવી_DEL; તોડો; // TODO
	સ્થિતિ K_KDIVIDE:		ચાવી = VTERM_ચાવી_KP_DIVIDE; તોડો;
	સ્થિતિ K_KEND:		ચાવી = VTERM_ચાવી_KP_1; તોડો; // TODO
	સ્થિતિ K_KENTER:		ચાવી = VTERM_ચાવી_KP_ENTER; તોડો;
	સ્થિતિ K_KHOME:		ચાવી = VTERM_ચાવી_KP_7; તોડો; // TODO
	સ્થિતિ K_KINS:		ચાવી = VTERM_ચાવી_KP_0; તોડો; // TODO
	સ્થિતિ K_KMINUS:		ચાવી = VTERM_ચાવી_KP_MINUS; તોડો;
	સ્થિતિ K_KMULTIPLY:	ચાવી = VTERM_ચાવી_KP_MULT; તોડો;
	સ્થિતિ K_KPAGEDOWN:	ચાવી = VTERM_ચાવી_KP_3; તોડો; // TODO
	સ્થિતિ K_KPAGEUP:		ચાવી = VTERM_ચાવી_KP_9; તોડો; // TODO
	સ્થિતિ K_KPLUS:		ચાવી = VTERM_ચાવી_KP_PLUS; તોડો;
	સ્થિતિ K_KPOINT:		ચાવી = VTERM_ચાવી_KP_PERIOD; તોડો;
	સ્થિતિ K_ડાબું:		ચાવી = VTERM_ચાવી_ડાબું; તોડો;
	સ્થિતિ K_S_ડાબું:		mod = VTERM_MOD_SHIFT;
				ચાવી = VTERM_ચાવી_ડાબું; તોડો;
	સ્થિતિ K_C_ડાબું:		mod = VTERM_MOD_CTRL;
				ચાવી = VTERM_ચાવી_ડાબું; તોડો;
	સ્થિતિ K_PAGEDOWN:	ચાવી = VTERM_ચાવી_PAGEDOWN; તોડો;
	સ્થિતિ K_PAGEUP:		ચાવી = VTERM_ચાવી_PAGEUP; તોડો;
	સ્થિતિ K_જમણું:		ચાવી = VTERM_ચાવી_જમણું; તોડો;
	સ્થિતિ K_S_જમણું:		mod = VTERM_MOD_SHIFT;
				ચાવી = VTERM_ચાવી_જમણું; તોડો;
	સ્થિતિ K_C_જમણું:		mod = VTERM_MOD_CTRL;
				ચાવી = VTERM_ચાવી_જમણું; તોડો;
	સ્થિતિ K_UP:		ચાવી = VTERM_ચાવી_UP; તોડો;
	સ્થિતિ K_S_UP:		mod = VTERM_MOD_SHIFT;
				ચાવી = VTERM_ચાવી_UP; તોડો;
	સ્થિતિ TAB:		ચાવી = VTERM_ચાવી_TAB; તોડો;
	સ્થિતિ K_S_TAB:		mod = VTERM_MOD_SHIFT;
				ચાવી = VTERM_ચાવી_TAB; તોડો;

	સ્થિતિ K_MOUSEUP:		other = term_send_mouse(vterm, 5, 1); તોડો;
	સ્થિતિ K_MOUSEDOWN:	other = term_send_mouse(vterm, 4, 1); તોડો;
	સ્થિતિ K_MOUSELEFT:	other = term_send_mouse(vterm, 7, 1); તોડો;
	સ્થિતિ K_MOUSERIGHT:	other = term_send_mouse(vterm, 6, 1); તોડો;

	સ્થિતિ K_LEFTMOUSE:
	સ્થિતિ K_LEFTMOUSE_NM:
	સ્થિતિ K_LEFTDRAG:
	સ્થિતિ K_LEFTRELEASE:
	સ્થિતિ K_LEFTRELEASE_NM:
	સ્થિતિ K_MOUSEMOVE:
	સ્થિતિ K_MIDDLEMOUSE:
	સ્થિતિ K_MIDDLEDRAG:
	સ્થિતિ K_MIDDLERELEASE:
	સ્થિતિ K_RIGHTMOUSE:
	સ્થિતિ K_RIGHTDRAG:
	સ્થિતિ K_RIGHTRELEASE:	જો (!term_mouse_click(vterm, c))
				    પાછા 0;
				other = TRUE;
				તોડો;

	સ્થિતિ K_X1MOUSE:		/* TODO */ પાછા 0;
	સ્થિતિ K_X1DRAG:		/* TODO */ પાછા 0;
	સ્થિતિ K_X1RELEASE:	/* TODO */ પાછા 0;
	સ્થિતિ K_X2MOUSE:		/* TODO */ પાછા 0;
	સ્થિતિ K_X2DRAG:		/* TODO */ પાછા 0;
	સ્થિતિ K_X2RELEASE:	/* TODO */ પાછા 0;

	સ્થિતિ K_IGNORE:		પાછા 0;
	સ્થિતિ K_NOP:		પાછા 0;
	સ્થિતિ K_UNDO:		પાછા 0;
	સ્થિતિ K_HELP:		પાછા 0;
	સ્થિતિ K_XF1:		ચાવી = VTERM_ચાવી_FUNCTION(1); તોડો;
	સ્થિતિ K_XF2:		ચાવી = VTERM_ચાવી_FUNCTION(2); તોડો;
	સ્થિતિ K_XF3:		ચાવી = VTERM_ચાવી_FUNCTION(3); તોડો;
	સ્થિતિ K_XF4:		ચાવી = VTERM_ચાવી_FUNCTION(4); તોડો;
	સ્થિતિ K_SELECT:		પાછા 0;
#ifdef FEAT_GUI
	સ્થિતિ K_VER_SCROLLBAR:	પાછા 0;
	સ્થિતિ K_HOR_SCROLLBAR:	પાછા 0;
#endif
#ifdef FEAT_GUI_TABLINE
	સ્થિતિ K_TABLINE:		પાછા 0;
	સ્થિતિ K_TABMENU:		પાછા 0;
#endif
#ifdef FEAT_NETBEANS_INTG
	સ્થિતિ K_F21:		ચાવી = VTERM_ચાવી_FUNCTION(21); તોડો;
#endif
#ifdef FEAT_DND
	સ્થિતિ K_DROP:		પાછા 0;
#endif
	સ્થિતિ K_CURSORHOLD:	પાછા 0;
	સ્થિતિ K_PS:		vterm_keyboard_start_paste(vterm);
				other = TRUE;
				તોડો;
	સ્થિતિ K_PE:		vterm_keyboard_end_paste(vterm);
				other = TRUE;
				તોડો;
    }

    // add modifiers for the typed key
    જો (modmask & MOD_MASK_SHIFT)
	mod |= VTERM_MOD_SHIFT;
    જો (modmask & MOD_MASK_CTRL)
	mod |= VTERM_MOD_CTRL;
    જો (modmask & (MOD_MASK_ALT | MOD_MASK_META))
	mod |= VTERM_MOD_ALT;

    // Ctrl-Shift-i may have the key "I" instead of "i", but for the kitty
    // keyboard protocol should use "i".  Applies to all ascii letters.
    જો (ASCII_ISUPPER(c)
	    && vterm_is_kitty_keyboard(vterm)
	    && mod == (VTERM_MOD_CTRL | VTERM_MOD_SHIFT))
	c = TOLOWER_ASC(c);

    /*
     * Convert special keys to vterm keys:
     * - Write keys to vterm: vterm_keyboard_key()
     * - Write output to channel.
     */
    જો (ચાવી != VTERM_ચાવી_NONE)
	// Special key, let vterm convert it.
	vterm_keyboard_ચાવી(vterm, ચાવી, mod);
    નહીંતર જો (!other)
	// Normal character, let vterm convert it.
	vterm_keyboard_unichar(vterm, c, mod);

    // Read back the converted escape sequence.
    પાછા (પૂર્ણાંક)vterm_output_read(vterm, buf, ચાવી_BUF_LEN);
}

/*
 * Return TRUE if the job for "term" is still running.
 * If "check_job_status" is TRUE update the job status.
 * NOTE: "term" may be freed by callbacks.
 */
    સ્થિર પૂર્ણાંક
term_job_running_check(term_T *term, પૂર્ણાંક check_job_status)
{
    // Also consider the job finished when the channel is closed, to avoid a
    // race condition when updating the title.
    જો (term == ખાલી
	|| term->tl_job == ખાલી
	|| !channel_is_open(term->tl_job->jv_channel))
	પાછા FALSE;

    job_T *job = term->tl_job;

    // Careful: Checking the job status may invoke callbacks, which close
    // the buffer and terminate "term".  However, "job" will not be freed
    // yet.
    જો (check_job_status)
	job_status(job);
    પાછા (job->jv_status == JOB_STARTED
	    || (job->jv_channel != ખાલી && job->jv_channel->ch_keep_open));
}

/*
 * Return TRUE if the job for "term" is still running.
 */
    પૂર્ણાંક
term_job_running(term_T *term)
{
    પાછા term_job_running_check(term, FALSE);
}

/*
 * Return TRUE if the job for "term" is still running, ignoring the job was
 * "NONE".
 */
    પૂર્ણાંક
term_job_running_not_none(term_T *term)
{
    પાછા term_job_running(term) && !term_none_open(term);
}

/*
 * Return TRUE if "term" has an active channel and used ":term NONE".
 */
    પૂર્ણાંક
term_none_open(term_T *term)
{
    // Also consider the job finished when the channel is closed, to avoid a
    // race condition when updating the title.
    પાછા term != ખાલી
	&& term->tl_job != ખાલી
	&& channel_is_open(term->tl_job->jv_channel)
	&& term->tl_job->jv_channel->ch_keep_open;
}

//
// Used to confirm whether we would like to kill a terminal.
// Return OK when the user confirms to kill it.
// Return FAIL if the user selects otherwise.
//
    પૂર્ણાંક
term_confirm_stop(buf_T *buf)
{
    char_u	buff[DIALOG_સંદેશ_SIZE];
    પૂર્ણાંક	ret;

    dialog_સંદેશ(buff, _("Kill job in \"%s\"?"), buf_get_fname(buf));
    ret = vim_dialog_yesno(VIM_QUESTION, ખાલી, buff, 1);
    જો (ret == VIM_YES)
	પાછા OK;
    નહીંતર
	પાછા FAIL;
}

/*
 * Used when exiting: kill the job in "buf" if so desired.
 * Return OK when the job finished.
 * Return FAIL when the job is still running.
 */
    પૂર્ણાંક
term_try_stop_job(buf_T *buf)
{
    પૂર્ણાંક	    ગણતરી;
    અક્ષર    *how = (અક્ષર *)buf->b_term->tl_kill;

#જો defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
    જો ((how == ખાલી || *how == NUL)
			  && (p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)))
    {
	જો (term_confirm_stop(buf) == OK)
	    how = "kill";
	નહીંતર
	    પાછા FAIL;
    }
#endif
    જો (how == ખાલી || *how == NUL)
	પાછા FAIL;

    job_stop(buf->b_term->tl_job, ખાલી, how);

    // wait for up to a second for the job to die
    માટે (ગણતરી = 0; ગણતરી < 100; ++ગણતરી)
    {
	job_T *job;

	// buffer, terminal and job may be cleaned up while waiting
	જો (!buf_valid(buf)
		|| buf->b_term == ખાલી
		|| buf->b_term->tl_job == ખાલી)
	    પાછા OK;
	job = buf->b_term->tl_job;

	// Call job_status() to update jv_status. It may cause the job to be
	// cleaned up but it won't be freed.
	job_status(job);
	જો (job->jv_status >= JOB_ENDED)
	    પાછા OK;

	ui_delay(10L, TRUE);
	term_flush_messages();
    }
    પાછા FAIL;
}

/*
 * Add the last line of the scrollback buffer to the buffer in the window.
 */
    સ્થિર શૂન્ય
add_scrollback_લીટી_to_buffer(term_T *term, char_u *text, પૂર્ણાંક len, પૂર્ણાંક append)
{
    buf_T	*buf = term->tl_buffer;
    પૂર્ણાંક		empty = (buf->b_ml.ml_flags & ML_EMPTY);
    linenr_T	lnum = buf->b_ml.ml_લીટી_ગણતરી;

#ifdef MSWIN
    char_u *tmp = text;
    જો (!enc_utf8 && enc_codepage > 0)
    {
	WCHAR   *ret = ખાલી;
	પૂર્ણાંક	length = 0;

	MultiByteToWideChar_alloc(CP_UTF8, 0, (અક્ષર*)text, len + 1,
							   &ret, &length);
	જો (ret != ખાલી)
	{
	    WideCharToMultiByte_alloc(enc_codepage, 0,
				      ret, length, (અક્ષર **)&text, &len, 0, 0);
	    vim_free(ret);
	}
    }
#endif

    જો (append)
    {
	char_u *prev_text = ml_get_buf(buf, lnum, FALSE);
	size_t prev_len = STRLEN(prev_text);

	char_u *both = alloc(len + prev_len + 2);
	જો (both == ખાલી)
	    પાછા;
	vim_strncpy(both, prev_text, prev_len + 1);
	vim_strncpy(both + prev_len, text, len + 1);

	curbuf = buf;
	ml_replace(lnum, both, FALSE);
	curbuf = curwin->w_buffer;
    }
    નહીંતર
	ml_append_buf(buf, lnum, text, len + 1, FALSE);

#ifdef MSWIN
    જો (tmp != text)
	vim_free(text);
#endif
    જો (empty)
    {
	// Delete the empty line that was in the empty buffer.
	curbuf = buf;
	ml_delete(1);
	curbuf = curwin->w_buffer;
    }
}

    સ્થિર શૂન્ય
cell2cellattr(અચલ VTermScreenCell *cell, cellattr_T *attr)
{
    attr->width = cell->width;
    attr->attrs = cell->attrs;
    attr->fg = cell->fg;
    attr->bg = cell->bg;
}

    સ્થિર પૂર્ણાંક
equal_celattr(cellattr_T *a, cellattr_T *b)
{
    // We only compare the RGB colors, ignoring the ANSI index and type.
    // Thus black set explicitly is equal the background black.
    પાછા a->fg.red == b->fg.red
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
    સ્થિર પૂર્ણાંક
add_empty_scrollback(term_T *term, cellattr_T *fill_attr, પૂર્ણાંક lnum)
{
    જો (ga_grow(&term->tl_scrollback, 1) == FAIL)
	પાછા FALSE;

    sb_લીટી_T *લીટી = (sb_લીટી_T *)term->tl_scrollback.ga_data
	+ term->tl_scrollback.ga_len;

    જો (lnum > 0)
    {
	પૂર્ણાંક i;

	માટે (i = 0; i < term->tl_scrollback.ga_len - lnum; ++i)
	{
	    *લીટી = *(લીટી - 1);
	    --લીટી;
	}
    }
    લીટી->sb_cols = 0;
    લીટી->sb_bytes = 0;
    લીટી->sb_cells = ખાલી;
    લીટી->sb_fill_attr = *fill_attr;
    ++term->tl_scrollback.ga_len;
    પાછા OK;
}

/*
 * Remove the terminal contents from the scrollback and the buffer.
 * Used before adding a new scrollback line or updating the buffer for lines
 * displayed in the terminal.
 */
    સ્થિર શૂન્ય
cleanup_scrollback(term_T *term)
{
    sb_લીટી_T	*લીટી;
    garray_T	*gap;
    char_u	*bufline;
    size_t      bufline_length;

    curbuf = term->tl_buffer;
    gap = &term->tl_scrollback;
    bufline = ml_get_buf(curbuf, curbuf->b_ml.ml_લીટી_ગણતરી, FALSE);
    bufline_length = STRLEN(bufline);
    જ્યારે (term->tl_scrollback_snapshot && gap->ga_len > 0)
    {
	લીટી = (sb_લીટી_T *)gap->ga_data + gap->ga_len - 1;
	જો (લીટી->sb_bytes < 0 || (size_t)લીટી->sb_bytes > bufline_length)
	    તોડો;
	bufline_length -= લીટી->sb_bytes;
	જો (!bufline_length)
	{
	    ml_delete(curbuf->b_ml.ml_લીટી_ગણતરી);
	    bufline = ml_get_buf(curbuf, curbuf->b_ml.ml_લીટી_ગણતરી, FALSE);
	    bufline_length = STRLEN(bufline);
	}
	vim_free(લીટી->sb_cells);
	--gap->ga_len;
	--term->tl_scrollback_snapshot;
    }
    જો (bufline_length < STRLEN(bufline))
    {
	char_u *shortened = vim_strnsave(bufline, bufline_length);
	ml_replace(curbuf->b_ml.ml_લીટી_ગણતરી, shortened, FALSE);
    }
    curbuf = curwin->w_buffer;
    જો (curbuf == term->tl_buffer)
	check_cursor();
}

/*
 * Add the current lines of the terminal to scrollback and to the buffer.
 */
    સ્થિર શૂન્ય
update_snapshot(term_T *term)
{
    VTermScreen	    *screen;
    VTermState	    *state;
    પૂર્ણાંક		    len;
    પૂર્ણાંક		    લીટીઓ_skipped = 0;
    VTermPos	    pos;
    VTermScreenCell cell;
    cellattr_T	    fill_attr, new_fill_attr;
    cellattr_T	    *p;

    ch_log(term->tl_job == ખાલી ? ખાલી : term->tl_job->jv_channel,
				  "Adding terminal window snapshot to buffer");

    // First remove the lines that were appended before, they might be
    // outdated.
    cleanup_scrollback(term);

    screen = vterm_obtain_screen(term->tl_vterm);
    state = vterm_obtain_state(term->tl_vterm);
    fill_attr = new_fill_attr = term->tl_default_color;
    માટે (pos.row = 0; pos.row < term->tl_rows; ++pos.row)
    {
	len = 0;
	માટે (pos.col = 0; pos.col < term->tl_cols; ++pos.col)
	    જો (vterm_screen_get_cell(screen, pos, &cell) != 0
						       && cell.chars[0] != NUL)
	    {
		len = pos.col + 1;
		new_fill_attr = term->tl_default_color;
	    }
	    નહીંતર
		// Assume the last attr is the filler attr.
		cell2cellattr(&cell, &new_fill_attr);

	જો (len == 0 && equal_celattr(&new_fill_attr, &fill_attr))
	    ++લીટીઓ_skipped;
	નહીંતર
	{
	    જ્યારે (લીટીઓ_skipped > 0)
	    {
		// Line was skipped, add an empty line.
		--લીટીઓ_skipped;
		જો (add_empty_scrollback(term, &fill_attr, 0) == OK)
		{
		    add_scrollback_લીટી_to_buffer(term, (char_u *)"", 0, 0);
		    ++term->tl_scrollback_snapshot;
		}
	    }

	    જો (len == 0)
		p = ખાલી;
	    નહીંતર
		p = ALLOC_MULT(cellattr_T, len);
	    જો ((p != ખાલી || len == 0)
				     && ga_grow(&term->tl_scrollback, 1) == OK)
	    {
		garray_T    ga;
		પૂર્ણાંક	    width;
		અચલ VTerm_લીટી_Info *lineinfo;
		sb_લીટી_T   *લીટી = (sb_લીટી_T *)term->tl_scrollback.ga_data
						  + term->tl_scrollback.ga_len;

		lineinfo = vterm_state_get_lineinfo(state, pos.row);

		ga_init2(&ga, 1, 100);
		માટે (pos.col = 0; pos.col < len; pos.col += width)
		{
		    જો (vterm_screen_get_cell(screen, pos, &cell) == 0)
		    {
			width = 1;
			CLEAR_POINTER(p + pos.col);
			જો (ga_grow(&ga, 1) == OK)
			    ga.ga_len += utf_char2bytes(' ',
					     (char_u *)ga.ga_data + ga.ga_len);
		    }
		    નહીંતર
		    {
			width = cell.width;

			cell2cellattr(&cell, &p[pos.col]);
			જો (width == 2)
			    // second cell of double-width character has the
			    // same attributes.
			    p[pos.col + 1] = p[pos.col];

			// Each character can be up to 6 bytes.
			જો (ga_grow(&ga, VTERM_MAX_CHARS_PER_CELL * 6) == OK)
			{
			    પૂર્ણાંક	    i;
			    પૂર્ણાંક	    c;

			    માટે (i = 0; i < VTERM_MAX_CHARS_PER_CELL &&
				    ((c = cell.chars[i]) > 0 || i == 0); ++i)
				ga.ga_len += utf_char2bytes(c == NUL ? ' ' : c,
					     (char_u *)ga.ga_data + ga.ga_len);
			}
		    }
		}
		લીટી->sb_cols = len;
		લીટી->sb_bytes = ga.ga_len;
		લીટી->sb_cells = p;
		લીટી->sb_fill_attr = new_fill_attr;
		લીટી->continuation = (char_u)lineinfo->continuation;
		fill_attr = new_fill_attr;
		++term->tl_scrollback.ga_len;
		++term->tl_scrollback_snapshot;

		જો (ga_grow(&ga, 1) == FAIL)
		    add_scrollback_લીટી_to_buffer(term, (char_u *)"", 0, 0);
		નહીંતર
		{
		    *((char_u *)ga.ga_data + ga.ga_len) = NUL;
		    add_scrollback_લીટી_to_buffer(term, ga.ga_data, ga.ga_len,
						  lineinfo->continuation);
		}
		ga_clear(&ga);
	    }
	    નહીંતર
		vim_free(p);
	}
    }

    // Add trailing empty lines.
    માટે (pos.row = term->tl_scrollback.ga_len;
	    pos.row < term->tl_scrollback_scrolled + term->tl_cursor_pos.row;
	    ++pos.row)
    {
	જો (add_empty_scrollback(term, &fill_attr, 0) == OK)
	{
	    add_scrollback_લીટી_to_buffer(term, (char_u *)"", 0, 0);
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
    સ્થિર પૂર્ણાંક
for_all_windows_and_curwin(win_T **wp, પૂર્ણાંક *did_curwin)
{
    જો (*wp == ખાલી)
	*wp = firstwin;
    નહીંતર જો ((*wp)->w_next != ખાલી)
	*wp = (*wp)->w_next;
    નહીંતર જો (!*did_curwin)
	*wp = curwin;
    નહીંતર
	પાછા FALSE;
    જો (*wp == curwin)
	*did_curwin = TRUE;
    પાછા TRUE;
}

/*
 * If needed, add the current lines of the terminal to scrollback and to the
 * buffer.  Called after the job has ended and when switching to
 * Terminal-Normal mode.
 * When "redraw" is TRUE redraw the windows that show the terminal.
 */
    શૂન્ય
may_move_terminal_to_buffer(term_T *term, પૂર્ણાંક redraw)
{
    જો (term->tl_vterm == ખાલી)
	પાછા;

    // Update the snapshot only if something changes or the buffer does not
    // have all the lines.
    જો (term->tl_dirty_snapshot || term->tl_buffer->b_ml.ml_લીટી_ગણતરી
					       <= term->tl_buffer_scrolled)
	update_snapshot(term);

    જો (redraw)
    {
	win_T	    *wp = ખાલી;
	પૂર્ણાંક	    did_curwin = FALSE;

	જ્યારે (for_all_windows_and_curwin(&wp, &did_curwin))
	{
	    જો (wp->w_buffer == term->tl_buffer)
	    {
		wp->w_cursor.lnum = term->tl_buffer->b_ml.ml_લીટી_ગણતરી;
		wp->w_cursor.col = 0;
		wp->w_valid = 0;
		જો (wp->w_cursor.lnum >= wp->w_height)
		{
		    linenr_T min_topline = wp->w_cursor.lnum - wp->w_height + 1;

		    જો (wp->w_topline < min_topline)
			wp->w_topline = min_topline;
		}
		redraw_win_later(wp, UPD_NOT_VALID);
	    }
	}
    }
}

#જો defined(FEAT_TIMERS)
/*
 * Check if any terminal timer expired.  If so, copy text from the terminal to
 * the buffer.
 * Return the time until the next timer will expire.
 */
    પૂર્ણાંક
term_check_timers(પૂર્ણાંક next_due_arg, proftime_T *now)
{
    term_T  *term;
    પૂર્ણાંક	    next_due = next_due_arg;

    FOR_ALL_TERMS(term)
    {
	જો (term->tl_timer_set && !term->tl_normal_mode)
	{
	    long    this_due = proftime_time_ડાબું(&term->tl_timer_due, now);

	    જો (this_due <= 1)
	    {
		term->tl_timer_set = FALSE;
		may_move_terminal_to_buffer(term, FALSE);
	    }
	    નહીંતર જો (next_due == -1 || next_due > this_due)
		next_due = this_due;
	}
    }

    પાછા next_due;
}
#endif

/*
 * When "normal_mode" is TRUE set the terminal to Terminal-Normal mode,
 * otherwise end it.
 */
    સ્થિર શૂન્ય
set_terminal_mode(term_T *term, પૂર્ણાંક normal_mode)
{
    term->tl_normal_mode = normal_mode;
    may_trigger_modechanged();
    જો (!normal_mode)
	handle_postponed_scrollback(term);
    VIM_CLEAR(term->tl_status_text);
    જો (term->tl_buffer == curbuf)
	maketitle();
}

/*
 * Called after the job is finished and Terminal mode is not active:
 * Move the vterm contents into the scrollback buffer and free the vterm.
 */
    સ્થિર શૂન્ય
cleanup_vterm(term_T *term)
{
    set_terminal_mode(term, FALSE);
    જો (term->tl_finish != TL_FINISH_CLOSE)
	may_move_terminal_to_buffer(term, TRUE);
    term_free_vterm(term);
}

/*
 * Switch from Terminal-Job mode to Terminal-Normal mode.
 * Suspends updating the terminal window.
 */
    સ્થિર શૂન્ય
term_enter_normal_mode(શૂન્ય)
{
    term_T     *term = curbuf->b_term;
    linenr_T   lnum;
    પૂર્ણાંક        col;

    set_terminal_mode(term, TRUE);

    // Append the current terminal contents to the buffer.
    may_move_terminal_to_buffer(term, TRUE);

    // Move the window cursor to the position of the cursor in the
    // terminal.
    lnum = term->tl_buffer_scrolled + 1 + term->tl_cursor_pos.row;
    col = term->tl_cursor_pos.col;
    scrollbackline_pos_in_buf(term, term->tl_cursor_pos.row + term->tl_scrollback_scrolled, &lnum, &col, ખાલી);

    curwin->w_cursor.lnum = lnum;
    check_cursor();
    જો (coladvance(col) == FAIL)
	coladvance(MAXCOL);
    curwin->w_set_curswant = true;
    curwin->w_topline = term->tl_buffer_scrolled + 1;
}

/*
 * Returns TRUE if the current window contains a terminal and we are in
 * Terminal-Normal mode.
 */
    પૂર્ણાંક
term_in_normal_mode(buf_T *buf)
{
    term_T *term = buf->b_term;

    પાછા term != ખાલી && term->tl_normal_mode;
}

/*
 * Switch from Terminal-Normal mode to Terminal-Job mode.
 * Restores updating the terminal window.
 */
    શૂન્ય
term_enter_job_mode(શૂન્ય)
{
    term_T	*term = curbuf->b_term;

    set_terminal_mode(term, FALSE);

    જો (term->tl_channel_closed)
	cleanup_vterm(term);
    redraw_buf_and_status_later(curbuf, UPD_NOT_VALID);
#ifdef FEAT_PROP_POPUP
    જો (WIN_IS_POPUP(curwin))
	redraw_later(UPD_NOT_VALID);
#endif
}

/*
 * When "modify_other_keys" is set then vgetc() should not reduce a key with
 * modifiers into a basic key.  However, we may only find out after calling
 * vgetc().  Therefore vgetorpeek() will call check_no_reduce_keys() to update
 * "no_reduce_keys" before using it.
 */
પ્રકારવ્યાખ્યા ગણના {
    NRKS_NONE,	    // initial value
    NRKS_CHECK,	    // modify_other_keys was off before calling vgetc()
    NRKS_SET,	    // no_reduce_keys was incremented in term_vgetc() or
		    // check_no_reduce_keys(), must be decremented.
} reduce_ચાવી_state_T;

સ્થિર reduce_ચાવી_state_T  no_reduce_ચાવી_state = NRKS_NONE;

/*
 * Return TRUE if the term is using modifyOtherKeys level 2 or the kitty
 * keyboard protocol.
 */
    સ્થિર પૂર્ણાંક
vterm_using_ચાવી_protocol(શૂન્ય)
{
    પાછા curbuf->b_term != ખાલી
	&& curbuf->b_term->tl_vterm != ખાલી
	&& (vterm_is_modify_other_ચાવીઓ(curbuf->b_term->tl_vterm)
		|| vterm_is_kitty_keyboard(curbuf->b_term->tl_vterm));
}

    શૂન્ય
check_no_reduce_ચાવીઓ(શૂન્ય)
{
    જો (no_reduce_ચાવી_state != NRKS_CHECK
	    || no_reduce_ચાવીઓ >= 1
	    || curbuf->b_term == ખાલી
	    || curbuf->b_term->tl_vterm == ખાલી)
	પાછા;

    જો (vterm_using_ચાવી_protocol())
    {
	// "modify_other_keys" or kitty keyboard protocol was enabled while
	// waiting.
	no_reduce_ચાવી_state = NRKS_SET;
	++no_reduce_ચાવીઓ;
    }
}

/*
 * Get a key from the user with terminal mode mappings.
 * Note: while waiting a terminal may be closed and freed if the channel is
 * closed and ++close was used.  This may even happen before we get here.
 */
    સ્થિર પૂર્ણાંક
term_vgetc(શૂન્ય)
{
    પૂર્ણાંક c;
    પૂર્ણાંક save_State = State;

    State = MODE_TERMINAL;
    got_int = FALSE;
#ifdef MSWIN
    ctrl_break_was_pressed = FALSE;
#endif

    જો (vterm_using_ચાવી_protocol())
    {
	++no_reduce_ચાવીઓ;
	no_reduce_ચાવી_state = NRKS_SET;
    }
    નહીંતર
    {
	no_reduce_ચાવી_state = NRKS_CHECK;
    }

    c = vgetc();
    got_int = FALSE;
    State = save_State;

    જો (no_reduce_ચાવી_state == NRKS_SET)
	--no_reduce_ચાવીઓ;
    no_reduce_ચાવી_state = NRKS_NONE;

    પાછા c;
}

સ્થિર પૂર્ણાંક	mouse_was_outside = FALSE;

/*
 * Send key "c" with modifiers "modmask" to terminal.
 * Return FAIL when the key needs to be handled in Normal mode.
 * Return OK when the key was dropped or sent to the terminal.
 */
    પૂર્ણાંક
send_ચાવીઓ_to_term(term_T *term, પૂર્ણાંક c, પૂર્ણાંક modmask, પૂર્ણાંક typed)
{
    અક્ષર	સંદેશ[ચાવી_BUF_LEN];
    size_t	len;
    પૂર્ણાંક		dragging_outside = FALSE;

    // Catch keys that need to be handled as in Normal mode.
    પસંદગી (c)
    {
	સ્થિતિ NUL:
	સ્થિતિ K_ZERO:
	    જો (typed)
		stuffcharReadbuff(c);
	    પાછા FAIL;

	સ્થિતિ K_TABLINE:
	    stuffcharReadbuff(c);
	    પાછા FAIL;

	સ્થિતિ K_IGNORE:
	સ્થિતિ K_CANCEL:  // used for :normal when running out of chars
	    પાછા FAIL;

	સ્થિતિ K_LEFTDRAG:
	સ્થિતિ K_MIDDLEDRAG:
	સ્થિતિ K_RIGHTDRAG:
	સ્થિતિ K_X1DRAG:
	સ્થિતિ K_X2DRAG:
	    dragging_outside = mouse_was_outside;
	    // FALLTHROUGH
	સ્થિતિ K_LEFTMOUSE:
	સ્થિતિ K_LEFTMOUSE_NM:
	સ્થિતિ K_LEFTRELEASE:
	સ્થિતિ K_LEFTRELEASE_NM:
	સ્થિતિ K_MOUSEMOVE:
	સ્થિતિ K_MIDDLEMOUSE:
	સ્થિતિ K_MIDDLERELEASE:
	સ્થિતિ K_RIGHTMOUSE:
	સ્થિતિ K_RIGHTRELEASE:
	સ્થિતિ K_X1MOUSE:
	સ્થિતિ K_X1RELEASE:
	સ્થિતિ K_X2MOUSE:
	સ્થિતિ K_X2RELEASE:

	સ્થિતિ K_MOUSEUP:
	સ્થિતિ K_MOUSEDOWN:
	સ્થિતિ K_MOUSELEFT:
	સ્થિતિ K_MOUSERIGHT:
	    {
		પૂર્ણાંક	row = mouse_row;
		પૂર્ણાંક	col = mouse_col;

#ifdef FEAT_PROP_POPUP
		જો (popup_is_popup(curwin))
		{
		    row -= popup_ટોચ_extra(curwin);
		    col -= popup_ડાબું_extra(curwin);
		}
#endif
		જો (row < W_WINROW(curwin)
			|| row >= (W_WINROW(curwin) + curwin->w_height)
			|| col < curwin->w_wincol
			|| col >= W_ENDCOL(curwin)
			|| dragging_outside)
		{
		    // click or scroll outside the current window or on status
		    // line or vertical separator
		    જો (typed)
		    {
			stuffcharReadbuff(c);
			mouse_was_outside = TRUE;
		    }
		    પાછા FAIL;
		}
	    }
	    તોડો;

	સ્થિતિ K_COMMAND:
	સ્થિતિ K_SCRIPT_COMMAND:
	    પાછા do_cmdkey_command(c, 0);
    }
    જો (typed)
	mouse_was_outside = FALSE;

    // Convert the typed key to a sequence of bytes for the job.
    len = term_convert_ચાવી(term, c, modmask, સંદેશ);
    જો (len > 0)
	// TODO: if FAIL is returned, stop?
	channel_send(term->tl_job->jv_channel, get_tty_part(term),
						(char_u *)સંદેશ, (પૂર્ણાંક)len, ખાલી);

    પાછા OK;
}

/*
 * Handle CTRL-W "": send register contents to the job.
 */
    સ્થિર શૂન્ય
term_paste_register(પૂર્ણાંક prev_c UNUSED)
{
    પૂર્ણાંક		c;
    list_T	*l;
    listitem_T	*item;
    long	reglen = 0;
    પૂર્ણાંક		type;

    જો (add_to_showcmd(prev_c))
    જો (add_to_showcmd('"'))
	out_flush();

    c = term_vgetc();
    clear_showcmd();

    જો (!term_use_loop())
	// job finished while waiting for a character
	પાછા;

    // CTRL-W "= prompt for expression to evaluate.
    જો (c == '=' && get_પદાવલિ_register() != '=')
	પાછા;
    જો (!term_use_loop())
	// job finished while waiting for a character
	પાછા;

    l = (list_T *)get_reg_contents(c, GREG_LIST);
    જો (l == ખાલી)
	પાછા;

    type = get_reg_type(c, &reglen);
    FOR_ALL_LIST_વસ્તુઓ(l, item)
    {
	char_u *s = tv_get_string(&item->li_tv);
#ifdef MSWIN
	char_u *tmp = s;

	જો (!enc_utf8 && enc_codepage > 0)
	{
	    WCHAR   *ret = ખાલી;
	    પૂર્ણાંક	length = 0;

	    MultiByteToWideChar_alloc(enc_codepage, 0, (અક્ષર *)s,
		    (પૂર્ણાંક)STRLEN(s), &ret, &length);
	    જો (ret != ખાલી)
	    {
		WideCharToMultiByte_alloc(CP_UTF8, 0,
			ret, length, (અક્ષર **)&s, &length, 0, 0);
		vim_free(ret);
	    }
	}
#endif
	channel_send(curbuf->b_term->tl_job->jv_channel, PART_IN,
		s, (પૂર્ણાંક)STRLEN(s), ખાલી);
#ifdef MSWIN
	જો (tmp != s)
	    vim_free(s);
#endif

	જો (item->li_next != ખાલી || type == MLINE)
	    channel_send(curbuf->b_term->tl_job->jv_channel, PART_IN,
		    (char_u *)"\r", 1, ખાલી);
    }
    list_free(l);
}

/*
 * Return TRUE when waiting for a character in the terminal, the cursor of the
 * terminal should be displayed.
 */
    પૂર્ણાંક
terminal_is_active(શૂન્ય)
{
    પાછા in_terminal_loop != ખાલી;
}

/*
 * Return the highlight group ID for the terminal and the window.
 */
    સ્થિર પૂર્ણાંક
term_get_highlight_id(term_T *term, win_T *wp)
{
    char_u *name;

    જો (wp != ખાલી && wp->w_hlfwin_id != 0)
	name = syn_id2name(wp->w_hlfwin_id);
    નહીંતર જો (term->tl_highlight_name != ખાલી)
	name = term->tl_highlight_name;
    નહીંતર
	name = (char_u*)"Terminal";

    પાછા syn_name2id(name);
}

#જો defined(FEAT_GUI)
    cursorentry_T *
term_get_cursor_shape(guicolor_T *fg, guicolor_T *bg)
{
    term_T		 *term = in_terminal_loop;
    સ્થિર cursorentry_T entry;
    પૂર્ણાંક			 id;
    guicolor_T		 term_fg = INVALCOLOR;
    guicolor_T		 term_bg = INVALCOLOR;

    CLEAR_FIELD(entry);
    entry.shape = entry.mshape =
	term->tl_cursor_shape == VTERM_PROP_CURSORSHAPE_UNDERLINE ? SHAPE_HOR :
	term->tl_cursor_shape == VTERM_PROP_CURSORSHAPE_BAR_ડાબું ? SHAPE_VER :
	SHAPE_BLOCK;
    entry.percentage = 20;
    જો (term->tl_cursor_blink)
    {
	entry.blinkwait = 700;
	entry.blinkon = 400;
	entry.blinkoff = 250;
    }

    // The highlight group overrules the defaults.
    id = term_get_highlight_id(term, curwin);
    જો (id != 0)
	syn_id2colors(id, &term_fg, &term_bg);
    જો (term_bg != INVALCOLOR)
	*fg = term_bg;
    નહીંતર
	*fg = gui.back_pixel;

    જો (term->tl_cursor_color == ખાલી)
    {
	જો (term_fg != INVALCOLOR)
	    *bg = term_fg;
	નહીંતર
	    *bg = gui.norm_pixel;
    }
    નહીંતર
	*bg = color_name2handle(term->tl_cursor_color);
    entry.name = "n";
    entry.used_for = SHAPE_CURSOR;

    પાછા &entry;
}
#endif

    સ્થિર શૂન્ય
may_output_cursor_props(શૂન્ય)
{
    જો (!cursor_color_equal(last_set_cursor_color, desired_cursor_color)
	    || last_set_cursor_shape != desired_cursor_shape
	    || last_set_cursor_blink != desired_cursor_blink)
    {
	cursor_color_copy(&last_set_cursor_color, desired_cursor_color);
	last_set_cursor_shape = desired_cursor_shape;
	last_set_cursor_blink = desired_cursor_blink;
	term_cursor_color(cursor_color_get(desired_cursor_color));
	જો (desired_cursor_shape == -1 || desired_cursor_blink == -1)
	    // this will restore the initial cursor style, if possible
	    ui_cursor_shape_forced(TRUE);
	નહીંતર
	    term_cursor_shape(desired_cursor_shape, desired_cursor_blink);
    }
}

/*
 * Set the cursor color and shape, if not last set to these.
 */
    સ્થિર શૂન્ય
may_set_cursor_props(term_T *term)
{
#ifdef FEAT_GUI
    // For the GUI the cursor properties are obtained with
    // term_get_cursor_shape().
    જો (gui.in_use)
	પાછા;
#endif
    જો (in_terminal_loop == term)
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
    સ્થિર શૂન્ય
prepare_restore_cursor_props(શૂન્ય)
{
#ifdef FEAT_GUI
    જો (gui.in_use)
	પાછા;
#endif
    cursor_color_copy(&desired_cursor_color, ખાલી);
    desired_cursor_shape = -1;
    desired_cursor_blink = -1;
    may_output_cursor_props();
}

/*
 * Returns TRUE if the current window contains a terminal and we are sending
 * keys to the job.
 * If "check_job_status" is TRUE update the job status.
 */
    સ્થિર પૂર્ણાંક
term_use_loop_check(પૂર્ણાંક check_job_status)
{
    term_T *term = curbuf->b_term;

    પાછા term != ખાલી
	&& !term->tl_normal_mode
	&& term->tl_vterm != ખાલી
	&& term_job_running_check(term, check_job_status);
}

/*
 * Returns TRUE if the current window contains a terminal and we are sending
 * keys to the job.
 */
    પૂર્ણાંક
term_use_loop(શૂન્ય)
{
    પાછા term_use_loop_check(FALSE);
}

/*
 * Called when entering a window with the mouse.  If this is a terminal window
 * we may want to change state.
 */
    શૂન્ય
term_win_entered(શૂન્ય)
{
    term_T *term = curbuf->b_term;

    જો (term == ખાલી)
	પાછા;

    જો (term_use_loop_check(TRUE))
    {
	reset_VIsual_and_resel();
	જો (State & MODE_ઉમેરો)
	    stop_ઉમેરો_mode = TRUE;
    }
    mouse_was_outside = FALSE;
    enter_mouse_col = mouse_col;
    enter_mouse_row = mouse_row;
}

    શૂન્ય
term_focus_change(પૂર્ણાંક in_focus)
{
    term_T *term = curbuf->b_term;

    જો (term == ખાલી || term->tl_vterm == ખાલી)
	પાછા;

    VTermState	*state = vterm_obtain_state(term->tl_vterm);

    જો (in_focus)
	vterm_state_focus_in(state);
    નહીંતર
	vterm_state_focus_out(state);
    term_forward_output(term);
}

/*
 * vgetc() may not include CTRL in the key when modify_other_keys is set.
 * Return the Ctrl-key value in that case.
 */
    સ્થિર પૂર્ણાંક
raw_c_to_ctrl(પૂર્ણાંક c)
{
    જો ((mod_mask & MOD_MASK_CTRL)
	    && ((c >= '`' && c <= 0x7f) || (c >= '@' && c <= '_')))
	પાછા c & 0x1f;
    પાછા c;
}

/*
 * When modify_other_keys is set then do the reverse of raw_c_to_ctrl().
 * Also when the Kitty keyboard protocol is used.
 * May set "mod_mask".
 */
    સ્થિર પૂર્ણાંક
ctrl_to_raw_c(પૂર્ણાંક c)
{
    જો (c < 0x20 && vterm_using_ચાવી_protocol())
    {
	mod_mask |= MOD_MASK_CTRL;
	પાછા c + '@';
    }
    પાછા c;
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
    પૂર્ણાંક
terminal_loop(પૂર્ણાંક blocking)
{
    પૂર્ણાંક		c;
    પૂર્ણાંક		raw_c;
    પૂર્ણાંક		termwinkey = 0;
    પૂર્ણાંક		ret;
#ifdef UNIX
    પૂર્ણાંક		tty_fd = curbuf->b_term->tl_job->jv_channel
				 ->ch_part[get_tty_part(curbuf->b_term)].ch_fd;
#endif
    પૂર્ણાંક		restore_cursor = FALSE;

    // Remember the terminal we are sending keys to.  However, the terminal
    // might be closed while waiting for a character, e.g. typing "exit" in a
    // shell and ++close was used.  Therefore use curbuf->b_term instead of a
    // stored reference.
    in_terminal_loop = curbuf->b_term;

    જો (*curwin->w_p_twk != NUL)
    {
	termwinkey = string_to_ચાવી(curwin->w_p_twk, TRUE);
	જો (termwinkey == Ctrl_W)
	    termwinkey = 0;
    }
    position_cursor(curwin, &curbuf->b_term->tl_cursor_pos);
    may_set_cursor_props(curbuf->b_term);

    જ્યારે (blocking || vpeekc_nomap() != NUL)
    {
#ifdef FEAT_GUI
	જો (curbuf->b_term != ખાલી && !curbuf->b_term->tl_system)
#endif
	    // TODO: skip screen update when handling a sequence of keys.
	    // Repeat redrawing in case a message is received while redrawing.
	    જ્યારે (must_redraw != 0)
		જો (update_screen(0) == FAIL)
		    તોડો;
	જો (!term_use_loop_check(TRUE) || in_terminal_loop != curbuf->b_term)
	    // job finished while redrawing
	    તોડો;

	update_cursor(curbuf->b_term, FALSE);
	restore_cursor = TRUE;

	raw_c = term_vgetc();
	જો (!term_use_loop_check(TRUE) || in_terminal_loop != curbuf->b_term)
	{
	    // Job finished while waiting for a character.  Push back the
	    // received character.
	    જો (raw_c != K_IGNORE)
		vungetc(raw_c);
	    તોડો;
	}
	જો (raw_c == K_IGNORE)
	    ચાલુ;
	c = raw_c_to_ctrl(raw_c);

#ifdef UNIX
	/*
	 * The shell or another program may change the tty settings.  Getting
	 * them for every typed character is a bit of overhead, but it's needed
	 * for the first character typed, e.g. when Vim starts in a shell.
	 */
	જો (mch_isatty(tty_fd))
	{
	    ttyinfo_T info;

	    // Get the current backspace character of the pty.
	    જો (get_tty_info(tty_fd, &info) == OK)
		term_backspace_char = info.backspace;
	}
#endif

#ifdef MSWIN
	// On Windows winpty handles CTRL-C, don't send a CTRL_C_EVENT.
	// Use CTRL-BREAK to kill the job.
	જો (ctrl_break_was_pressed)
	    mch_signal_job(curbuf->b_term->tl_job, (char_u *)"kill");
#endif
	// Was either CTRL-W (termwinkey) or CTRL-\ pressed?
	// Not in a system terminal.
	જો ((c == (termwinkey == 0 ? Ctrl_W : termwinkey) || c == Ctrl_BSL)
#ifdef FEAT_GUI
		&& !curbuf->b_term->tl_system
#endif
		)
	{
	    પૂર્ણાંક	    prev_c = c;
	    પૂર્ણાંક	    prev_raw_c = raw_c;
	    પૂર્ણાંક	    prev_mod_mask = mod_mask;

	    જો (add_to_showcmd(c))
		out_flush();

	    raw_c = term_vgetc();
	    c = raw_c_to_ctrl(raw_c);

	    clear_showcmd();

	    જો (!term_use_loop_check(TRUE)
					 || in_terminal_loop != curbuf->b_term)
		// job finished while waiting for a character
		તોડો;

	    જો (prev_c == Ctrl_BSL)
	    {
		જો (c == Ctrl_N)
		{
		    // CTRL-\ CTRL-N : go to Terminal-Normal mode.
		    term_enter_normal_mode();
		    ret = FAIL;
		    જાઓ theend;
		}
		// Send both keys to the terminal, first one here, second one
		// below.
		send_ચાવીઓ_to_term(curbuf->b_term, prev_raw_c, prev_mod_mask,
									 TRUE);
	    }
	    નહીંતર જો (c == Ctrl_C)
	    {
		// "CTRL-W CTRL-C" or 'termwinkey' CTRL-C: end the job
		mch_signal_job(curbuf->b_term->tl_job, (char_u *)"kill");
	    }
	    નહીંતર જો (c == '.')
	    {
		// "CTRL-W .": send CTRL-W to the job
		// "'termwinkey' .": send 'termwinkey' to the job
		raw_c = ctrl_to_raw_c(termwinkey == 0 ? Ctrl_W : termwinkey);
	    }
	    નહીંતર જો (c == Ctrl_BSL)
	    {
		// "CTRL-W CTRL-\": send CTRL-\ to the job
		raw_c = ctrl_to_raw_c(Ctrl_BSL);
	    }
	    નહીંતર જો (c == 'N')
	    {
		// CTRL-W N : go to Terminal-Normal mode.
		term_enter_normal_mode();
		ret = FAIL;
		જાઓ theend;
	    }
	    નહીંતર જો (c == '"')
	    {
		term_paste_register(prev_c);
		ચાલુ;
	    }
	    નહીંતર જો (termwinkey == 0 || c != termwinkey)
	    {
		// space for CTRL-W, modifier, multi-byte char and NUL
		char_u buf[1 + 3 + MB_MAXBYTES + 1];

		// Put the command into the typeahead buffer, when using the
		// stuff buffer KeyStuffed is set and 'langmap' won't be used.
		buf[0] = Ctrl_W;
		buf[special_to_buf(c, mod_mask, FALSE, buf + 1) + 1] = NUL;
		ins_typebuf(buf, REMAP_NONE, 0, TRUE, FALSE);
		ret = OK;
		જાઓ theend;
	    }
	}
#ifdef MSWIN
	જો (!enc_utf8 && has_mbyte && raw_c >= 0x80)
	{
	    WCHAR   wc;
	    char_u  mb[3];

	    mb[0] = (unsigned)raw_c >> 8;
	    mb[1] = raw_c;
	    જો (MultiByteToWideChar(GetACP(), 0, (અક્ષર*)mb, 2, &wc, 1) > 0)
		raw_c = wc;
	}
#endif
	જો (send_ચાવીઓ_to_term(curbuf->b_term, raw_c, mod_mask, TRUE) != OK)
	{
	    જો (raw_c == K_MOUSEMOVE)
		// We are sure to come back here, don't reset the cursor color
		// and shape to avoid flickering.
		restore_cursor = FALSE;

	    ret = OK;
	    જાઓ theend;
	}
    }
    ret = FAIL;

theend:
    in_terminal_loop = ખાલી;
    જો (restore_cursor)
	prepare_restore_cursor_props();

    // Move a snapshot of the screen contents to the buffer, so that completion
    // works in other buffers.
    જો (curbuf->b_term != ખાલી && !curbuf->b_term->tl_normal_mode)
	may_move_terminal_to_buffer(curbuf->b_term, FALSE);

    પાછા ret;
}

    સ્થિર શૂન્ય
may_toggle_cursor(term_T *term)
{
    જો (in_terminal_loop != term)
	પાછા;

    જો (term->tl_cursor_visible)
	cursor_on();
    નહીંતર
	cursor_off();
}

/*
 * Reverse engineer the RGB value into a cterm color index.
 * First color is 1.  Return 0 if no match found (default color).
 */
    સ્થિર પૂર્ણાંક
color2index(VTermColor *color, પૂર્ણાંક fg, પૂર્ણાંક *boldp)
{
    પૂર્ણાંક red = color->red;
    પૂર્ણાંક blue = color->blue;
    પૂર્ણાંક green = color->green;

    *boldp = FALSE;

    જો (VTERM_COLOR_IS_INVALID(color))
	પાછા 0;

    જો (VTERM_COLOR_IS_INDEXED(color))
    {
	// Use the color as-is if possible, give up otherwise.
	જો (color->index < t_colors)
	{
	    uint8_t index = color->index;
#ifdef MSWIN
	    // Convert ANSI palette to cterm color index.
	    // cterm_ansi_idx is a table for the reverse conversion from cterm
	    // color index to ANSI palette. However, the current table can
	    // be used for this conversion as well.
	    // Note: If the contents of cterm_ansi_idx change in the future, a
	    // different table may be needed for this purpose.
	    જો (index < 16)
		index = cterm_ansi_idx[index];
#endif
	    પાછા index + 1;
	}
	// 8-color terminals can actually display twice as many colors by
	// setting the high-intensity/bold bit.
	નહીંતર જો (t_colors == 8 && fg && color->index < 16)
	{
	    *boldp = TRUE;
	    પાછા (color->index & 7) + 1;
	}
	પાછા 0;
    }

    જો (t_colors >= 256)
    {
	જો (red == blue && red == green)
	{
	    // 24-color greyscale plus white and black
	    સ્થિર પૂર્ણાંક cutoff[23] = {
		    0x0D, 0x17, 0x21, 0x2B, 0x35, 0x3F, 0x49, 0x53, 0x5D, 0x67,
		    0x71, 0x7B, 0x85, 0x8F, 0x99, 0xA3, 0xAD, 0xB7, 0xC1, 0xCB,
		    0xD5, 0xDF, 0xE9};
	    પૂર્ણાંક i;

	    જો (red < 5)
		પાછા 17; // 00/00/00
	    જો (red > 245) // ff/ff/ff
		પાછા 232;
	    માટે (i = 0; i < 23; ++i)
		જો (red < cutoff[i])
		    પાછા i + 233;
	    પાછા 256;
	}
	{
	    સ્થિર પૂર્ણાંક cutoff[5] = {0x2F, 0x73, 0x9B, 0xC3, 0xEB};
	    પૂર્ણાંક ri, gi, bi;

	    // 216-color cube
	    માટે (ri = 0; ri < 5; ++ri)
		જો (red < cutoff[ri])
		    તોડો;
	    માટે (gi = 0; gi < 5; ++gi)
		જો (green < cutoff[gi])
		    તોડો;
	    માટે (bi = 0; bi < 5; ++bi)
		જો (blue < cutoff[bi])
		    તોડો;
	    પાછા 17 + ri * 36 + gi * 6 + bi;
	}
    }
    પાછા 0;
}

/*
 * Convert Vterm attributes to highlight flags.
 */
    સ્થિર પૂર્ણાંક
vtermAttr2hl(VTermScreenCellAttrs *cellattrs)
{
    પૂર્ણાંક attr = 0;

    જો (cellattrs->bold)
	attr |= HL_BOLD;
    જો (cellattrs->underline)
	attr |= HL_UNDERLINE;
    જો (cellattrs->italic)
	attr |= HL_ITALIC;
    જો (cellattrs->strike)
	attr |= HL_STRIKETHROUGH;
    જો (cellattrs->reverse)
	attr |= HL_INVERSE;
    પાછા attr;
}

/*
 * Store Vterm attributes in "cell" from highlight flags.
 */
    સ્થિર શૂન્ય
hl2vtermAttr(પૂર્ણાંક attr, cellattr_T *cell)
{
    CLEAR_FIELD(cell->attrs);
    જો (attr & HL_BOLD)
	cell->attrs.bold = 1;
    જો (attr & HL_UNDERLINE)
	cell->attrs.underline = 1;
    જો (attr & HL_ITALIC)
	cell->attrs.italic = 1;
    જો (attr & HL_STRIKETHROUGH)
	cell->attrs.strike = 1;
    જો (attr & HL_INVERSE)
	cell->attrs.reverse = 1;
}

/*
 * Convert the attributes of a vterm cell into an attribute index.
 */
    સ્થિર પૂર્ણાંક
cell2attr(
	term_T			*term,
	win_T			*wp,
	VTermScreenCellAttrs	*cellattrs,
	VTermColor		*cellfg,
	VTermColor		*cellbg)
{
    પૂર્ણાંક attr = vtermAttr2hl(cellattrs);
    VTermColor *fg = cellfg;
    VTermColor *bg = cellbg;
    પૂર્ણાંક is_default_fg = VTERM_COLOR_IS_DEFAULT_FG(fg);
    પૂર્ણાંક is_default_bg = VTERM_COLOR_IS_DEFAULT_BG(bg);

    જો (is_default_fg || is_default_bg)
    {
	જો (wp != ખાલી && wp->w_hlfwin_id != 0)
	{
	    જો (is_default_fg)
		fg = &wp->w_term_hlfwin.fg;
	    જો (is_default_bg)
		bg = &wp->w_term_hlfwin.bg;
	}
	નહીંતર
	{
	    જો (is_default_fg)
		fg = &term->tl_default_color.fg;
	    જો (is_default_bg)
		bg = &term->tl_default_color.bg;
	}
    }

#ifdef FEAT_GUI
    જો (gui.in_use)
    {
	guicolor_T guifg = gui_mch_get_rgb_color(fg->red, fg->green, fg->blue);
	guicolor_T guibg = gui_mch_get_rgb_color(bg->red, bg->green, bg->blue);
	પાછા get_gui_attr_idx(attr, guifg, guibg);
    }
    નહીંતર
#endif
#ifdef FEAT_TERMGUICOLORS
    જો (p_tgc)
    {
	guicolor_T tgcfg = VTERM_COLOR_IS_INVALID(fg)
	    ? INVALCOLOR
	    : gui_get_rgb_color_cmn(fg->red, fg->green, fg->blue);
	guicolor_T tgcbg = VTERM_COLOR_IS_INVALID(bg)
	    ? INVALCOLOR
	    : gui_get_rgb_color_cmn(bg->red, bg->green, bg->blue);
	પાછા get_tgc_attr_idx(attr, tgcfg, tgcbg);
    }
    નહીંતર
#endif
    {
	પૂર્ણાંક bold = MAYBE;
	પૂર્ણાંક ctermfg = color2index(fg, TRUE, &bold);
	પૂર્ણાંક ctermbg = color2index(bg, FALSE, &bold);

	// with 8 colors set the bold attribute to get a bright foreground
	જો (bold == TRUE)
	    attr |= HL_BOLD;

	પાછા get_cterm_attr_idx(attr, ctermfg, ctermbg);
    }
    પાછા 0;
}

    સ્થિર શૂન્ય
set_dirty_snapshot(term_T *term)
{
    term->tl_dirty_snapshot = TRUE;
#ifdef FEAT_TIMERS
    જો (!term->tl_normal_mode)
    {
	// Update the snapshot after 100 msec of not getting updates.
	profile_setlimit(100L, &term->tl_timer_due);
	term->tl_timer_set = TRUE;
    }
#endif
}

    સ્થિર પૂર્ણાંક
handle_damage(VTermRect rect, શૂન્ય *user)
{
    term_T *term = (term_T *)user;

    term->tl_dirty_row_start = MIN(term->tl_dirty_row_start, rect.start_row);
    term->tl_dirty_row_end = MAX(term->tl_dirty_row_end, rect.end_row);
    set_dirty_snapshot(term);
    redraw_buf_later(term->tl_buffer, UPD_SOME_VALID);
    પાછા 1;
}

    સ્થિર શૂન્ય
term_scroll_up(term_T *term, પૂર્ણાંક start_row, પૂર્ણાંક ગણતરી)
{
    win_T		 *wp = ખાલી;
    પૂર્ણાંક			 did_curwin = FALSE;
    VTermColor		 fg, bg;
    VTermScreenCellAttrs attr;
    પૂર્ણાંક			 clear_attr;

    CLEAR_FIELD(attr);

    જ્યારે (for_all_windows_and_curwin(&wp, &did_curwin))
    {
	જો (wp->w_buffer == term->tl_buffer)
	{
	    // Set the color to clear lines with.
	    vterm_state_get_default_colors(vterm_obtain_state(term->tl_vterm),
								     &fg, &bg);
	    clear_attr = cell2attr(term, wp, &attr, &fg, &bg);
	    win_del_લીટીઓ(wp, start_row, ગણતરી, FALSE, FALSE, clear_attr);
	}
    }
}

    સ્થિર પૂર્ણાંક
handle_moverect(VTermRect dest, VTermRect src, શૂન્ય *user)
{
    term_T	*term = (term_T *)user;
    પૂર્ણાંક		ગણતરી = src.start_row - dest.start_row;

    // Scrolling up is done much more efficiently by deleting lines instead of
    // redrawing the text. But avoid doing this multiple times, postpone until
    // the redraw happens.
    જો (dest.start_col == src.start_col
	    && dest.end_col == src.end_col
	    && dest.start_row < src.start_row)
    {
	જો (dest.start_row == 0)
	    term->tl_postponed_scroll += ગણતરી;
	નહીંતર
	    term_scroll_up(term, dest.start_row, ગણતરી);
    }

    term->tl_dirty_row_start = MIN(term->tl_dirty_row_start, dest.start_row);
    term->tl_dirty_row_end = MIN(term->tl_dirty_row_end, dest.end_row);
    set_dirty_snapshot(term);

    // Note sure if the scrolling will work correctly, let's do a complete
    // redraw later.
    redraw_buf_later(term->tl_buffer, UPD_NOT_VALID);
    પાછા 1;
}

    સ્થિર પૂર્ણાંક
handle_movecursor(
	VTermPos pos,
	VTermPos oldpos UNUSED,
	પૂર્ણાંક visible,
	શૂન્ય *user)
{
    term_T	*term = (term_T *)user;
    win_T	*wp = ખાલી;
    પૂર્ણાંક		did_curwin = FALSE;

    term->tl_cursor_pos = pos;
    term->tl_cursor_visible = visible;

    જ્યારે (for_all_windows_and_curwin(&wp, &did_curwin))
    {
	જો (wp->w_buffer == term->tl_buffer)
	    position_cursor(wp, &pos);
    }
    જો (term->tl_buffer == curbuf && !term->tl_normal_mode)
	// Don't redraw here, it will be done after
	// vterm_input_write() is finished.
	update_cursor(term, FALSE);

    પાછા 1;
}

    સ્થિર પૂર્ણાંક
handle_settermprop(
	VTermProp prop,
	VTerm_કિંમત *કિંમત,
	શૂન્ય *user)
{
    term_T	*term = (term_T *)user;
    char_u	*strval = ખાલી;

    પસંદગી (prop)
    {
	સ્થિતિ VTERM_PROP_TITLE:
	    જો (disable_vterm_title_for_testing)
		તોડો;
	    strval = vim_strnsave((char_u *)કિંમત->string.str,
							    કિંમત->string.len);
	    જો (strval == ખાલી)
		તોડો;
	    vim_free(term->tl_title);
	    // a blank title isn't useful, make it empty, so that "running" is
	    // displayed
	    જો (*skipwhite(strval) == NUL)
		term->tl_title = ખાલી;
	    // Same as blank
	    નહીંતર જો (term->tl_arg0_cmd != ખાલી
		    && STRNCMP(term->tl_arg0_cmd, strval,
					  (પૂર્ણાંક)STRLEN(term->tl_arg0_cmd)) == 0)
		term->tl_title = ખાલી;
	    // Empty corrupted data of winpty
	    નહીંતર જો (STRNCMP("  - ", strval, 4) == 0)
		term->tl_title = ખાલી;
#ifdef MSWIN
	    નહીંતર જો (!enc_utf8 && enc_codepage > 0)
	    {
		WCHAR   *ret = ખાલી;
		પૂર્ણાંક	length = 0;

		MultiByteToWideChar_alloc(CP_UTF8, 0,
			(અક્ષર*)કિંમત->string.str,
					(પૂર્ણાંક)કિંમત->string.len, &ret, &length);
		જો (ret != ખાલી)
		{
		    WideCharToMultiByte_alloc(enc_codepage, 0,
					ret, length, (અક્ષર**)&term->tl_title,
					&length, 0, 0);
		    vim_free(ret);
		}
	    }
#endif
	    નહીંતર
	    {
		term->tl_title = strval;
		strval = ખાલી;
	    }
	    VIM_CLEAR(term->tl_status_text);
	    જો (term == curbuf->b_term)
	    {
		maketitle();
		curwin->w_redr_status = true;
	    }
	    તોડો;

	સ્થિતિ VTERM_PROP_CURSORVISIBLE:
	    term->tl_cursor_visible = કિંમત->boolean;
	    may_toggle_cursor(term);
	    out_flush();
	    તોડો;

	સ્થિતિ VTERM_PROP_CURSORBLINK:
	    term->tl_cursor_blink = કિંમત->boolean;
	    may_set_cursor_props(term);
	    તોડો;

	સ્થિતિ VTERM_PROP_CURSORSHAPE:
	    term->tl_cursor_shape = કિંમત->સંખ્યા;
	    may_set_cursor_props(term);
	    તોડો;

	સ્થિતિ VTERM_PROP_CURSORCOLOR:
	    strval = vim_strnsave((char_u *)કિંમત->string.str,
							    કિંમત->string.len);
	    જો (strval == ખાલી)
		તોડો;
	    cursor_color_copy(&term->tl_cursor_color, strval);
	    may_set_cursor_props(term);
	    તોડો;

	સ્થિતિ VTERM_PROP_ALTSCREEN:
	    // TODO: do anything else?
	    term->tl_using_altscreen = કિંમત->boolean;
	    તોડો;

	મૂળભૂત:
	    તોડો;
    }
    vim_free(strval);

    // Always return 1, otherwise vterm doesn't store the value internally.
    પાછા 1;
}

/*
 * The job running in the terminal resized the terminal.
 */
    સ્થિર પૂર્ણાંક
handle_resize(પૂર્ણાંક rows, પૂર્ણાંક cols, શૂન્ય *user)
{
    term_T	*term = (term_T *)user;
    win_T	*wp;

    term->tl_rows = rows;
    term->tl_cols = cols;
    જો (term->tl_vterm_size_changed)
	// Size was set by vterm_set_size(), don't set the window size.
	term->tl_vterm_size_changed = FALSE;
    નહીંતર
    {
	FOR_ALL_WINDOWS(wp)
	{
	    જો (wp->w_buffer == term->tl_buffer)
	    {
		win_setheight_win(rows, wp);
		win_setwidth_win(cols, wp);
	    }
	}
	redraw_buf_later(term->tl_buffer, UPD_NOT_VALID);
    }
    પાછા 1;
}

/*
 * If the number of lines that are stored goes over 'termwinscroll' then
 * delete the first 10%.
 * "gap" points to tl_scrollback or tl_scrollback_postponed.
 * "update_buffer" is TRUE when the buffer should be updated.
 */
    સ્થિર શૂન્ય
limit_scrollback(term_T *term, garray_T *gap, પૂર્ણાંક update_buffer)
{
    જો (gap->ga_len < term->tl_buffer->b_p_twsl)
	પાછા;

    પૂર્ણાંક	todo = MAX(term->tl_buffer->b_p_twsl / 10,
				     gap->ga_len - term->tl_buffer->b_p_twsl);
    પૂર્ણાંક	i;
    sb_લીટી_T *sb_લીટીઓ = (sb_લીટી_T *)gap->ga_data;

    curbuf = term->tl_buffer;
    માટે (i = 0; i < todo; ++i)
    {
	જો (update_buffer && (!sb_લીટીઓ[i].continuation || !i))
	{
	    ml_delete(1);
	    --term->tl_buffer_scrolled;
	}
	vim_free(sb_લીટીઓ[i].sb_cells);
    }
    // Continue until end of wrapped line
    માટે (; todo < gap->ga_len && sb_લીટીઓ[todo].continuation; ++todo)
	vim_free(sb_લીટીઓ[todo].sb_cells);
    curbuf = curwin->w_buffer;

    gap->ga_len -= todo;
    mch_memmove(gap->ga_data,
		(sb_લીટી_T *)gap->ga_data + todo,
		કદ(sb_લીટી_T) * gap->ga_len);
    જો (update_buffer)
    {
	win_T *curwin_save = curwin;
	win_T *wp = ખાલી;

	term->tl_scrollback_scrolled -= todo;

	FOR_ALL_WINDOWS(wp)
	{
	    જો (wp->w_buffer == term->tl_buffer)
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
    સ્થિર પૂર્ણાંક
handle_pushline(પૂર્ણાંક cols, અચલ VTermScreenCell *cells, પૂર્ણાંક continuation, શૂન્ય *user)
{
    term_T	*term = (term_T *)user;
    garray_T	*gap;
    પૂર્ણાંક		update_buffer;

    જો (term->tl_normal_mode)
    {
	// In Terminal-Normal mode the user interacts with the buffer, thus we
	// must not change it. Postpone adding the scrollback lines.
	gap = &term->tl_scrollback_postponed;
	update_buffer = FALSE;
    }
    નહીંતર
    {
	// First remove the lines that were appended before, the pushed line
	// goes above it.
	cleanup_scrollback(term);
	gap = &term->tl_scrollback;
	update_buffer = TRUE;
    }

    limit_scrollback(term, gap, update_buffer);

    જો (ga_grow(gap, 1) == FAIL)
	પાછા 0;

    cellattr_T	*p = ખાલી;
    પૂર્ણાંક		len = 0;
    પૂર્ણાંક		i;
    પૂર્ણાંક		c;
    પૂર્ણાંક		col;
    પૂર્ણાંક		text_len;
    char_u		*text;
    sb_લીટી_T	*લીટી;
    garray_T	ga;
    cellattr_T	fill_attr = term->tl_default_color;

    // do not store empty cells at the end
    માટે (i = 0; i < cols; ++i)
	જો (cells[i].chars[0] != 0)
	    len = i + 1;
	નહીંતર
	    cell2cellattr(&cells[i], &fill_attr);

    ga_init2(&ga, 1, 100);
    જો (len > 0)
	p = ALLOC_MULT(cellattr_T, len);
    જો (p != ખાલી)
    {
	માટે (col = 0; col < len; col += cells[col].width)
	{
	    જો (ga_grow(&ga, VTERM_MAX_CHARS_PER_CELL * 4) == FAIL)
	    {
		ga.ga_len = 0;
		તોડો;
	    }
	    માટે (i = 0; i < VTERM_MAX_CHARS_PER_CELL &&
		    ((c = cells[col].chars[i]) > 0 || i == 0); ++i)
		ga.ga_len += utf_char2bytes(c == NUL ? ' ' : c,
			(char_u *)ga.ga_data + ga.ga_len);
	    cell2cellattr(&cells[col], &p[col]);
	}
    }
    જો (ga_grow(&ga, 1) == FAIL)
    {
	જો (update_buffer)
	    text = (char_u *)"";
	નહીંતર
	    text = vim_strsave((char_u *)"");
	text_len = 0;
    }
    નહીંતર
    {
	text = ga.ga_data;
	text_len = ga.ga_len;
	*(text + text_len) = NUL;
    }
    જો (update_buffer)
	add_scrollback_લીટી_to_buffer(term, text, text_len, continuation);

    લીટી = (sb_લીટી_T *)gap->ga_data + gap->ga_len;
    લીટી->sb_cols = len;
    લીટી->sb_bytes = text_len;
    લીટી->sb_cells = p;
    લીટી->sb_fill_attr = fill_attr;
    લીટી->continuation = (char_u)continuation;
    જો (update_buffer)
    {
	લીટી->sb_text = ખાલી;
	++term->tl_scrollback_scrolled;
	જો (!continuation)
	    ++term->tl_buffer_scrolled;
	ga_clear(&ga);  // free the text
    }
    નહીંતર
    {
	લીટી->sb_text = text;
	ga_init(&ga);  // text is kept in tl_scrollback_postponed
    }
    ++gap->ga_len;
    પાછા 0; // ignored
}

/*
 * Called when leaving Terminal-Normal mode: deal with any scrollback that was
 * received and stored in tl_scrollback_postponed.
 */
    સ્થિર શૂન્ય
handle_postponed_scrollback(term_T *term)
{
    પૂર્ણાંક i;

    જો (term->tl_scrollback_postponed.ga_len == 0)
	પાછા;
    ch_log(ખાલી, "Moving postponed scrollback to scrollback");

    // First remove the lines that were appended before, the pushed lines go
    // above it.
    cleanup_scrollback(term);

    માટે (i = 0; i < term->tl_scrollback_postponed.ga_len; ++i)
    {
	char_u		*text;
	sb_લીટી_T	*pp_લીટી;
	sb_લીટી_T	*લીટી;

	જો (ga_grow(&term->tl_scrollback, 1) == FAIL)
	    તોડો;
	pp_લીટી = (sb_લીટી_T *)term->tl_scrollback_postponed.ga_data + i;

	text = pp_લીટી->sb_text;
	જો (text == ખાલી)
	    text = (char_u *)"";
	add_scrollback_લીટી_to_buffer(term, text, (પૂર્ણાંક)STRLEN(text), pp_લીટી->continuation);
	vim_free(pp_લીટી->sb_text);

	લીટી = (sb_લીટી_T *)term->tl_scrollback.ga_data
						 + term->tl_scrollback.ga_len;
	લીટી->sb_cols = pp_લીટી->sb_cols;
	લીટી->sb_bytes = pp_લીટી->sb_bytes;
	લીટી->sb_cells = pp_લીટી->sb_cells;
	લીટી->sb_fill_attr = pp_લીટી->sb_fill_attr;
	લીટી->sb_text = ખાલી;
	++term->tl_scrollback_scrolled;
	++term->tl_scrollback.ga_len;
	જો (!pp_લીટી->continuation)
	    ++term->tl_buffer_scrolled;
    }

    ga_clear(&term->tl_scrollback_postponed);
    limit_scrollback(term, &term->tl_scrollback, TRUE);
}

/*
 * Called when the terminal wants to ring the system bell.
 */
    સ્થિર પૂર્ણાંક
handle_bell(શૂન્ય *user UNUSED)
{
    vim_beep(BO_TERM);
    પાછા 0;
}

સ્થિર VTermScreenCallbacks screen_callbacks = {
    handle_damage,		// damage
    handle_moverect,		// moverect
    handle_movecursor,		// movecursor
    handle_settermprop,		// settermprop
    handle_bell,		// bell
    handle_resize,		// resize
    ખાલી,			// sb_pushline
    ખાલી,			// sb_popline
    ખાલી,			// sb_clear
    handle_pushline	// sb_pushline4
};

/*
 * Do the work after the channel of a terminal was closed.
 * Must be called only when updating_screen is FALSE.
 * Returns TRUE when a buffer was closed (list of terminals may have changed).
 */
    સ્થિર પૂર્ણાંક
term_after_channel_closed(term_T *term)
{
    // Unless in Terminal-Normal mode: clear the vterm.
    જો (!term->tl_normal_mode)
    {
	પૂર્ણાંક	fnum = term->tl_buffer->b_fnum;

	cleanup_vterm(term);

	જો (term->tl_finish == TL_FINISH_CLOSE)
	{
	    aco_save_T	aco;
	    પૂર્ણાંક		do_set_w_locked = term->tl_buffer->b_nwindows == 0;
#ifdef FEAT_PROP_POPUP
	    win_T	*pwin = ખાલી;

	    // If this was a terminal in a popup window, go back to the
	    // previous window.
	    જો (popup_is_popup(curwin) && curbuf == term->tl_buffer)
	    {
		pwin = curwin;
		જો (win_valid(prevwin))
		    win_enter(prevwin, FALSE);
	    }
	    નહીંતર
#endif
	    // If this is the last normal window: exit Vim.
	    જો (term->tl_buffer->b_nwindows > 0 && only_one_window())
	    {
		exarg_T ea;

		CLEAR_FIELD(ea);
		ex_quit(&ea);
		પાછા TRUE;
	    }

	    // ++close or term_finish == "close"
	    ch_log(ખાલી, "terminal job finished, closing window");
	    aucmd_prepbuf(&aco, term->tl_buffer);
	    જો (curbuf == term->tl_buffer)
	    {
		win_T	*wp = curwin;

		// Avoid closing the window if we temporarily use it.
		જો (is_aucmd_win(wp))
		    do_set_w_locked = TRUE;
		જો (do_set_w_locked)
		    ++wp->w_locked;
		do_bufdel(DOBUF_WIPE, (char_u *)"", 1, fnum, fnum, FALSE);
		જો (do_set_w_locked)
		    --wp->w_locked;
		aucmd_restbuf(&aco);
	    }
#ifdef FEAT_PROP_POPUP
	    જો (pwin != ખાલી)
		popup_close_with_retval(pwin, 0);
#endif
	    પાછા TRUE;
	}
	જો (term->tl_finish == TL_FINISH_OPEN
				   && term->tl_buffer->b_nwindows == 0)
	{
	    અક્ષર    *cmd = term->tl_opencmd == ખાલી
				? "botright sbuf %d"
				: (અક્ષર *)term->tl_opencmd;
	    size_t  len = strlen(cmd) + 50;
	    અક્ષર    *buf = alloc(len);

	    જો (buf != ખાલી)
	    {
		ch_log(ખાલી, "terminal job finished, opening window");
		vim_snprintf(buf, len, cmd, fnum);
		do_cmdline_cmd((char_u *)buf);
		vim_free(buf);
	    }
	}
	નહીંતર
	    ch_log(ખાલી, "terminal job finished");
    }

    redraw_buf_and_status_later(term->tl_buffer, UPD_NOT_VALID);
    પાછા FALSE;
}

#જો defined(FEAT_PROP_POPUP)
/*
 * If the current window is a terminal in a popup window and the job has
 * finished, close the popup window and to back to the previous window.
 * Otherwise return FAIL.
 */
    પૂર્ણાંક
may_close_term_popup(શૂન્ય)
{
    જો (!popup_is_popup(curwin) || curbuf->b_term == ખાલી
				 || term_job_running_not_none(curbuf->b_term))
	પાછા FAIL;

    win_T *pwin = curwin;

    જો (win_valid(prevwin))
	win_enter(prevwin, FALSE);
    popup_close_with_retval(pwin, 0);
    પાછા OK;
}
#endif

/*
 * Called when a channel is going to be closed, before invoking the close
 * callback.
 */
    શૂન્ય
term_channel_closing(channel_T *ch)
{
    term_T *term;

    માટે (term = first_term; term != ખાલી; term = term->tl_next)
	જો (term->tl_job == ch->ch_job && !term->tl_channel_closed)
	    term->tl_channel_closing = TRUE;
}

/*
 * Called when a channel has been closed.
 * If this was a channel for a terminal window then finish it up.
 */
    શૂન્ય
term_channel_closed(channel_T *ch)
{
    term_T *term;
    term_T *next_term;
    પૂર્ણાંક	    did_one = FALSE;

    માટે (term = first_term; term != ખાલી; term = next_term)
    {
	next_term = term->tl_next;
	જો (term->tl_job == ch->ch_job && !term->tl_channel_closed)
	{
	    term->tl_channel_closed = TRUE;
	    did_one = TRUE;

	    VIM_CLEAR(term->tl_title);
	    VIM_CLEAR(term->tl_status_text);
#ifdef MSWIN
	    જો (term->tl_out_fd != ખાલી)
	    {
		fclose(term->tl_out_fd);
		term->tl_out_fd = ખાલી;
	    }
#endif

	    જો (updating_screen)
	    {
		// Cannot open or close windows now.  Can happen when
		// 'lazyredraw' is set.
		term->tl_channel_recently_closed = TRUE;
		ચાલુ;
	    }

	    જો (term_after_channel_closed(term))
		next_term = first_term;
	}
    }

    જો (did_one)
    {
	redraw_statuslines();

	// Need to break out of vgetc().
	ins_char_typebuf(K_IGNORE, 0);
	typebuf_was_filled = TRUE;

	term = curbuf->b_term;
	જો (term != ખાલી)
	{
	    જો (term->tl_job == ch->ch_job)
		maketitle();
	    update_cursor(term, term->tl_cursor_visible);
	}
    }
}

/*
 * To be called after resetting updating_screen: handle any terminal where the
 * channel was closed.
 */
    શૂન્ય
term_check_channel_closed_recently(શૂન્ય)
{
    term_T *term;
    term_T *next_term;

    માટે (term = first_term; term != ખાલી; term = next_term)
    {
	next_term = term->tl_next;
	જો (term->tl_channel_recently_closed)
	{
	    term->tl_channel_recently_closed = FALSE;
	    જો (term_after_channel_closed(term))
		// start over, the list may have changed
		next_term = first_term;
	}
    }
}

/*
 * Fill one screen line from a line of the terminal.
 * Advances "pos" to past the last column.
 */
    સ્થિર શૂન્ય
term_line2screenline(
	term_T		*term,
	win_T		*wp,
	VTermScreen	*screen,
	VTermPos	*pos,
	પૂર્ણાંક		max_col)
{
    પૂર્ણાંક off = screen_get_current_લીટી_off();

    માટે (pos->col = 0; pos->col < max_col; )
    {
	VTermScreenCell cell;
	પૂર્ણાંક		c;

	જો (vterm_screen_get_cell(screen, *pos, &cell) == 0)
	    CLEAR_FIELD(cell);

	c = cell.chars[0];
	જો (c == NUL)
	{
	    Screen_લીટીઓ[off] = ' ';
	    જો (enc_utf8)
		Screen_લીટીઓ_UC[off] = NUL;
	}
	નહીંતર
	{
	    જો (enc_utf8)
	    {
		પૂર્ણાંક i;

		// composing chars
		માટે (i = 0; i < Screen_mco
			      && i + 1 < VTERM_MAX_CHARS_PER_CELL; ++i)
		{
		    Screen_લીટીઓ_C[i][off] = cell.chars[i + 1];
		    જો (cell.chars[i + 1] == 0)
			તોડો;
		}
		જો (c >= 0x80 || (Screen_mco > 0
					 && Screen_લીટીઓ_C[0][off] != 0))
		{
		    Screen_લીટીઓ[off] = ' ';
		    Screen_લીટીઓ_UC[off] = c;
		}
		નહીંતર
		{
		    Screen_લીટીઓ[off] = c;
		    Screen_લીટીઓ_UC[off] = NUL;
		}
	    }
#ifdef MSWIN
	    નહીંતર જો (has_mbyte && c >= 0x80)
	    {
		char_u	mb[MB_MAXBYTES+1];
		WCHAR	wc = c;

		જો (WideCharToMultiByte(GetACP(), 0, &wc, 1,
					       (અક્ષર*)mb, 2, 0, 0) > 1)
		{
		    Screen_લીટીઓ[off] = mb[0];
		    Screen_લીટીઓ[off + 1] = mb[1];
		    cell.width = mb_ptr2cells(mb);
		}
		નહીંતર
		    Screen_લીટીઓ[off] = c;
	    }
#endif
	    નહીંતર
		// This will only store the lower byte of "c".
		Screen_લીટીઓ[off] = c;
	}
	ScreenAttrs[off] = cell2attr(term, wp, &cell.attrs, &cell.fg,
								     &cell.bg);

	++pos->col;
	++off;
	જો (cell.width == 2)
	{
	    // don't set the second byte to NUL for a DBCS encoding, it
	    // has been set above
	    જો (enc_utf8)
	    {
		Screen_લીટીઓ_UC[off] = NUL;
		Screen_લીટીઓ[off] = NUL;
	    }
	    નહીંતર જો (!has_mbyte)
	    {
		// Can't show a double-width character with a single-byte
		// 'encoding', just use a space.
		Screen_લીટીઓ[off] = ' ';
		ScreenAttrs[off] = ScreenAttrs[off - 1];
	    }

	    ++pos->col;
	    ++off;
	}
    }
}

#જો defined(FEAT_GUI)
    સ્થિર શૂન્ય
update_system_term(term_T *term)
{
    VTermPos	    pos;
    VTermScreen	    *screen;

    જો (term->tl_vterm == ખાલી)
	પાછા;
    screen = vterm_obtain_screen(term->tl_vterm);

    // Scroll up to make more room for terminal lines if needed.
    // Use the cursor position to determine how much to scroll, because
    // ConPTY may damage all rows on initialization even when most are
    // empty, which would cause unnecessary scrolling.
    જ્યારે (term->tl_toprow > 0
		  && (Rows - term->tl_toprow) < term->tl_cursor_pos.row + 1)
    {
	પૂર્ણાંક save_p_more = p_more;

	p_more = FALSE;
	સંદેશ_row = Rows - 1;
	સંદેશ_puts("\n");
	p_more = save_p_more;
	--term->tl_toprow;
    }

    માટે (pos.row = term->tl_dirty_row_start; pos.row < term->tl_dirty_row_end
						  && pos.row < Rows; ++pos.row)
    {
	જો (pos.row < term->tl_rows)
	{
	    પૂર્ણાંક max_col = MIN(topframe->fr_width, term->tl_cols);

	    term_line2screenline(term, ખાલી, screen, &pos, max_col);
	}
	નહીંતર
	    pos.col = 0;

	screen_લીટી(curwin, term->tl_toprow + pos.row,
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
    પૂર્ણાંક
term_do_update_window(win_T *wp)
{
    term_T	*term = wp->w_buffer->b_term;

    પાછા term != ખાલી && term->tl_vterm != ખાલી && !term->tl_normal_mode;
}

/*
 * Called to update a window that contains an active terminal.
 */
    શૂન્ય
term_update_window(win_T *wp)
{
    term_T	*term = wp->w_buffer->b_term;
    VTerm	*vterm;
    VTermScreen *screen;
    VTermState	*state;
    VTermPos	pos;
    પૂર્ણાંક		rows, cols;
    પૂર્ણાંક		newrows, newcols;
    પૂર્ણાંક		minsize;
    win_T	*twp;

    vterm = term->tl_vterm;
    screen = vterm_obtain_screen(vterm);
    state = vterm_obtain_state(vterm);

    // We use UPD_NOT_VALID on a resize or scroll, redraw everything then.
    // With UPD_SOME_VALID only redraw what was marked dirty.
    જો (wp->w_redr_type > UPD_SOME_VALID)
    {
	term->tl_dirty_row_start = 0;
	term->tl_dirty_row_end = MAX_ROW;

	જો (term->tl_postponed_scroll > 0
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
    માટે (twp = firstwin; ; twp = twp->w_next)
    {
	// Always use curwin, it may be a popup window.
	win_T *wwp = twp == ખાલી ? curwin : twp;

	// When more than one window shows the same terminal, use the
	// smallest size.
	જો (wwp->w_buffer == term->tl_buffer)
	{
	    newrows = MIN(newrows, wwp->w_height);
	    newcols = MIN(newcols, wwp->w_width);
	}
	જો (twp == ખાલી)
	    તોડો;
    }
    જો (newrows == 99999 || newcols == 99999)
	પાછા; // safety exit
    newrows = rows == 0 ? newrows : minsize ? MAX(rows, newrows) : rows;
    newcols = cols == 0 ? newcols : minsize ? MAX(cols, newcols) : cols;

    // If no cell is visible there is no point in resizing.  Also, vterm can't
    // handle a zero height.
    જો (newrows == 0 || newcols == 0)
	પાછા;

    જો (term->tl_rows != newrows || term->tl_cols != newcols)
    {
	term->tl_vterm_size_changed = TRUE;
	vterm_set_size(vterm, newrows, newcols);
	ch_log(term->tl_job->jv_channel, "Resizing terminal to %d lines",
								      newrows);
	term_report_winsize(term, newrows, newcols);

	// Updating the terminal size will cause the snapshot to be cleared.
	// When not in terminal_loop() we need to restore it.
	જો (term != in_terminal_loop)
	    may_move_terminal_to_buffer(term, FALSE);
    }

    // The cursor may have been moved when resizing.
    vterm_state_get_cursorpos(state, &pos);
    position_cursor(wp, &pos);

    માટે (pos.row = term->tl_dirty_row_start; pos.row < term->tl_dirty_row_end
					  && pos.row < wp->w_height; ++pos.row)
    {
	જો (pos.row < term->tl_rows)
	{
	    પૂર્ણાંક max_col = MIN(wp->w_width, term->tl_cols);

	    term_line2screenline(term, wp, screen, &pos, max_col);
	}
	નહીંતર
	    pos.col = 0;

	screen_લીટી(wp, wp->w_winrow + pos.row
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
    શૂન્ય
term_did_update_window(win_T *wp)
{
    term_T	*term = wp->w_buffer->b_term;

    જો (term == ખાલી || term->tl_vterm == ખાલી || term->tl_normal_mode
						|| wp->w_redr_type != 0)
	પાછા;

    term->tl_dirty_row_start = MAX_ROW;
    term->tl_dirty_row_end = 0;
}

/*
 * Return TRUE if "wp" is a terminal window where the job has finished.
 */
    પૂર્ણાંક
term_is_finished(buf_T *buf)
{
    પાછા buf->b_term != ખાલી && buf->b_term->tl_vterm == ખાલી;
}

/*
 * Return TRUE if "wp" is a terminal window where the job has finished or we
 * are in Terminal-Normal mode, thus we show the buffer contents.
 */
    પૂર્ણાંક
term_show_buffer(buf_T *buf)
{
    term_T *term = buf->b_term;

    પાછા term != ખાલી && (term->tl_vterm == ખાલી || term->tl_normal_mode);
}

/*
 * The current buffer is going to be changed.  If there is terminal
 * highlighting remove it now.
 */
    શૂન્ય
term_change_in_curbuf(શૂન્ય)
{
    term_T *term = curbuf->b_term;

    જો (!term_is_finished(curbuf) || term->tl_scrollback.ga_len <= 0)
	પાછા;

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
    પૂર્ણાંક
term_get_attr(win_T *wp, linenr_T lnum, પૂર્ણાંક col)
{
    buf_T	*buf = wp->w_buffer;
    term_T	*term = buf->b_term;
    sb_લીટી_T	*લીટી;
    cellattr_T	*cellattr;
    પૂર્ણાંક         sb_લીટી = -1;
    પૂર્ણાંક         sb_col = col;

    જો (term->tl_scrollback.ga_len)
	bufline_pos_in_scrollback(term, lnum, col, &sb_લીટી, &sb_col);

    જો (sb_લીટી < 0)
	cellattr = &term->tl_default_color;
    નહીંતર
    {
	લીટી = (sb_લીટી_T *)term->tl_scrollback.ga_data + sb_લીટી;
	જો (sb_col < 0 || sb_col >= લીટી->sb_cols)
	    cellattr = &લીટી->sb_fill_attr;
	નહીંતર
	    cellattr = લીટી->sb_cells + sb_col;
    }
    પાછા cell2attr(term, wp, &cellattr->attrs, &cellattr->fg, &cellattr->bg);
}

/*
 * Convert a cterm color number 0 - 255 to RGB.
 * This is compatible with xterm.
 */
    સ્થિર શૂન્ય
cterm_color2vterm(પૂર્ણાંક nr, VTermColor *rgb)
{
    cterm_color2rgb(nr, &rgb->red, &rgb->green, &rgb->blue, &rgb->index);
    જો (rgb->index == 0)
	rgb->type = VTERM_COLOR_RGB;
    નહીંતર
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
    સ્થિર પૂર્ણાંક
get_vterm_color_from_synid(પૂર્ણાંક id, VTermColor *fg, VTermColor *bg)
{
#જો defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    // Use the actual color for the GUI and when 'termguicolors' is set.
    જો (0
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

	જો (id > 0)
	    syn_id2colors(id, &fg_rgb, &bg_rgb);

	જો (fg_rgb != INVALCOLOR)
	{
	    long_u rgb = GUI_MCH_GET_RGB(fg_rgb);
	    fg->red = (unsigned)(rgb >> 16);
	    fg->green = (unsigned)(rgb >> 8) & 255;
	    fg->blue = (unsigned)rgb & 255;
	    fg->type = VTERM_COLOR_RGB | VTERM_COLOR_DEFAULT_FG;
	}
	નહીંતર
	    fg->type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_FG;

	જો (bg_rgb != INVALCOLOR)
	{
	    long_u rgb = GUI_MCH_GET_RGB(bg_rgb);
	    bg->red = (unsigned)(rgb >> 16);
	    bg->green = (unsigned)(rgb >> 8) & 255;
	    bg->blue = (unsigned)rgb & 255;
	    bg->type = VTERM_COLOR_RGB | VTERM_COLOR_DEFAULT_BG;
	}
	નહીંતર
	    bg->type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_BG;

	પાછા TRUE;
    }
    નહીંતર
#endif
    જો (t_colors >= 16)
    {
	પૂર્ણાંક cterm_fg = -1;
	પૂર્ણાંક cterm_bg = -1;

	જો (id > 0)
	    syn_id2cterm_bg(id, &cterm_fg, &cterm_bg);

	જો (cterm_fg >= 0)
	{
	    cterm_color2vterm(cterm_fg, fg);
	    fg->type |= VTERM_COLOR_DEFAULT_FG;
	}
	નહીંતર
	    fg->type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_FG;

	જો (cterm_bg >= 0)
	{
	    cterm_color2vterm(cterm_bg, bg);
	    bg->type |= VTERM_COLOR_DEFAULT_BG;
	}
	નહીંતર
	    bg->type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_BG;

	પાછા TRUE;
    }

    પાછા FALSE;
}

    શૂન્ય
term_reset_hlfwin(win_T *wp)
{
    wp->w_term_hlfwin.fg.type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_FG;
    wp->w_term_hlfwin.bg.type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_BG;
}

/*
 * Cache the color of HLF_WIN.
 */
    શૂન્ય
term_update_hlfwin(win_T *wp)
{
    પૂર્ણાંક id = wp->w_hlfwin_id;

    જો (id == 0 || !get_vterm_color_from_synid(id == -1 ? 0 : id,
		&wp->w_term_hlfwin.fg, &wp->w_term_hlfwin.bg))
	term_reset_hlfwin(wp);
}

/*
 * Called when option 'termguicolors' was set,
 * or when any highlight is changed.
 */
    શૂન્ય
term_update_hlfwin_all(શૂન્ય)
{
    win_T	 *wp = ખાલી;
    પૂર્ણાંક		 did_curwin = FALSE;

    જ્યારે (for_all_windows_and_curwin(&wp, &did_curwin))
	term_update_hlfwin(wp);
}

/*
 * Initialize term->tl_default_color from the environment.
 */
    શૂન્ય
term_init_default_colors(term_T *term)
{
    VTermColor	    *fg, *bg;
    પૂર્ણાંક		    fgval, bgval;
    પૂર્ણાંક		    id;

    CLEAR_FIELD(term->tl_default_color.attrs);
    term->tl_default_color.width = 1;
    fg = &term->tl_default_color.fg;
    bg = &term->tl_default_color.bg;

    // Vterm uses a default black background.  Set it to white when
    // 'background' is "light".
    જો (*p_bg == 'l')
    {
	fgval = 0;
	bgval = 255;
    }
    નહીંતર
    {
	fgval = 255;
	bgval = 0;
    }
    fg->red = fg->green = fg->blue = fgval;
    bg->red = bg->green = bg->blue = bgval;
    fg->type = VTERM_COLOR_RGB | VTERM_COLOR_DEFAULT_FG;
    bg->type = VTERM_COLOR_RGB | VTERM_COLOR_DEFAULT_BG;

    // The highlight group overrules the defaults.
    id = term_get_highlight_id(term, ખાલી);

    જો (!get_vterm_color_from_synid(id, fg, bg))
    {
#જો defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
	પૂર્ણાંક tmp;
#endif

	// In an MS-Windows console we know the normal colors.
	જો (cterm_normal_fg_color > 0)
	{
	    cterm_color2vterm(cterm_normal_fg_color - 1, fg);
#જો defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
# ifdef VIMDLL
	    જો (!gui.in_use)
# endif
	    {
		tmp = fg->red;
		fg->red = fg->blue;
		fg->blue = tmp;
	    }
#endif
	}
#ifdef FEAT_TERMRESPONSE
	નહીંતર
	    term_get_fg_color(&fg->red, &fg->green, &fg->blue);
#endif

	જો (cterm_normal_bg_color > 0)
	{
	    cterm_color2vterm(cterm_normal_bg_color - 1, bg);
#જો defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
# ifdef VIMDLL
	    જો (!gui.in_use)
# endif
	    {
		tmp = fg->red;
		fg->red = fg->blue;
		fg->blue = tmp;
	    }
#endif
	}
#ifdef FEAT_TERMRESPONSE
	નહીંતર
	    term_get_bg_color(&bg->red, &bg->green, &bg->blue);
#endif
    }
}

#જો defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Return TRUE if the user-defined palette (either g:terminal_ansi_colors or the
 * "ansi_colors" argument in term_start()) shall be applied.
 */
    સ્થિર પૂર્ણાંક
term_use_palette(શૂન્ય)
{
    જો (0
# ifdef FEAT_GUI
	    || gui.in_use
# endif
# ifdef FEAT_TERMGUICOLORS
	    || p_tgc
# endif
       )
	પાછા TRUE;
    પાછા FALSE;
}

/*
 * Set the 16 ANSI colors from array of RGB values
 */
    સ્થિર શૂન્ય
set_vterm_palette(VTerm *vterm, long_u *rgb)
{
    પૂર્ણાંક		index = 0;
    VTermState	*state = vterm_obtain_state(vterm);

    માટે (; index < 16; index++)
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
    સ્થિર પૂર્ણાંક
set_ansi_colors_list(VTerm *vterm, list_T *list)
{
    પૂર્ણાંક		n = 0;
    long_u	rgb[16];
    listitem_T	*li;

    માટે (li = list->lv_first; li != ખાલી && n < 16; li = li->li_next, n++)
    {
	char_u		*color_name;
	guicolor_T	guicolor;

	color_name = tv_get_string_chk(&li->li_tv);
	જો (color_name == ખાલી)
	    પાછા FAIL;

	guicolor = GUI_GET_COLOR(color_name);
	જો (guicolor == INVALCOLOR)
	    પાછા FAIL;

	rgb[n] = GUI_MCH_GET_RGB(guicolor);
    }

    જો (n != 16 || li != ખાલી)
	પાછા FAIL;

    set_vterm_palette(vterm, rgb);

    પાછા OK;
}

/*
 * Initialize the ANSI color palette from g:terminal_ansi_colors[0:15]
 */
    સ્થિર શૂન્ય
init_vterm_ansi_colors(VTerm *vterm)
{
    dictitem_T	*var = find_var((char_u *)"g:terminal_ansi_colors", ખાલી, TRUE);

    જો (var == ખાલી)
	પાછા;

    જો (var->di_tv.v_type != VAR_LIST
	    || var->di_tv.vval.v_list == ખાલી
	    || var->di_tv.vval.v_list->lv_first == &range_list_item
	    || set_ansi_colors_list(vterm, var->di_tv.vval.v_list) == FAIL)
	semsg(_(e_invalid_argument_str), "g:terminal_ansi_colors");
}
#endif

/*
 * Handles a "drop" command from the job in the terminal.
 * "item" is the file name, "item->li_next" may have options.
 */
    સ્થિર શૂન્ય
handle_drop_command(listitem_T *item)
{
    char_u	*fname = tv_get_string(&item->li_tv);
    listitem_T	*opt_item = item->li_next;
    પૂર્ણાંક		bufnr;
    win_T	*wp;
    tabpage_T   *tp;
    exarg_T	ea;
    char_u	*tofree = ખાલી;

    bufnr = buflist_add(fname, BLN_LISTED | BLN_NOOPT);
    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	જો (wp->w_buffer->b_fnum == bufnr)
	{
	    // buffer is in a window already, go there
	    goto_tabpage_win(tp, wp);
	    પાછા;
	}
    }

    CLEAR_FIELD(ea);

    જો (opt_item != ખાલી && opt_item->li_tv.v_type == VAR_DICT
					&& opt_item->li_tv.vval.v_dict != ખાલી)
    {
	dict_T *dict = opt_item->li_tv.vval.v_dict;
	char_u *p;

	p = dict_get_string(dict, "ff", FALSE);
	જો (p == ખાલી)
	    p = dict_get_string(dict, "fileformat", FALSE);
	જો (p != ખાલી)
	{
	    જો (check_ff_કિંમત(p) == FAIL)
		ch_log(ખાલી, "Invalid ff argument to drop: %s", p);
	    નહીંતર
		ea.force_ff = *p;
	}
	p = dict_get_string(dict, "enc", FALSE);
	જો (p == ખાલી)
	    p = dict_get_string(dict, "encoding", FALSE);
	જો (p != ખાલી)
	{
	    ea.cmd = alloc(STRLEN(p) + 12);
	    જો (ea.cmd != ખાલી)
	    {
		sprintf((અક્ષર *)ea.cmd, "sbuf ++enc=%s", p);
		ea.force_enc = 11;
		tofree = ea.cmd;
	    }
	}

	p = dict_get_string(dict, "bad", FALSE);
	જો (p != ખાલી)
	    get_bad_opt(p, &ea);

	જો (dict_has_ચાવી(dict, "bin"))
	    ea.force_bin = FORCE_BIN;
	જો (dict_has_ચાવી(dict, "binary"))
	    ea.force_bin = FORCE_BIN;
	જો (dict_has_ચાવી(dict, "nobin"))
	    ea.force_bin = FORCE_NOBIN;
	જો (dict_has_ચાવી(dict, "nobinary"))
	    ea.force_bin = FORCE_NOBIN;
    }

    // open in new window, like ":split fname"
    જો (ea.cmd == ખાલી)
	ea.cmd = (char_u *)"split";
    ea.arg = fname;
    ea.cmdidx = CMD_split;
    ex_splitview(&ea);

    vim_free(tofree);
}

/*
 * Return TRUE if "func" starts with "pat" and "pat" isn't empty.
 */
    સ્થિર પૂર્ણાંક
is_permitted_term_api(char_u *func, char_u *pat)
{
    પાછા pat != ખાલી && *pat != NUL && STRNICMP(func, pat, STRLEN(pat)) == 0;
}

/*
 * Handles a function call from the job running in a terminal.
 * "item" is the function name, "item->li_next" has the arguments.
 */
    સ્થિર શૂન્ય
handle_call_command(term_T *term, channel_T *channel, listitem_T *item)
{
    char_u	*func;
    typval_T	argvars[2];
    typval_T	rettv;
    funcexe_T	funcexe;

    જો (item->li_next == ખાલી)
    {
	ch_log(channel, "Missing function arguments for call");
	પાછા;
    }
    func = tv_get_string(&item->li_tv);

    જો (!is_permitted_term_api(func, term->tl_api))
    {
	ch_log(channel, "Unpermitted function: %s", func);
	પાછા;
    }

    argvars[0].v_type = VAR_સંખ્યા;
    argvars[0].vval.v_સંખ્યા = term->tl_buffer->b_fnum;
    argvars[1] = item->li_next->li_tv;
    CLEAR_FIELD(funcexe);
    funcexe.fe_firstline = 1L;
    funcexe.fe_lastline = 1L;
    funcexe.fe_evaluate = TRUE;
    જો (call_func(func, -1, &rettv, 2, argvars, &funcexe) == OK)
    {
	clear_tv(&rettv);
	ch_log(channel, "Function %s called", func);
    }
    નહીંતર
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
    સ્થિર size_t
url_decode(અચલ અક્ષર *src, અચલ size_t len, char_u *dst)
{
    size_t i = 0, j = 0;

    જ્યારે (i < len)
    {
	જો (src[i] == '%' && i + 2 < len)
	{
	    dst[j] = hexhex2nr((char_u *)&src[i + 1]);
	    j++;
	    i += 3;
	}
	નહીંતર
	{
	    dst[j] = src[i];
	    i++;
	    j++;
	}
    }
    dst[j] = '\0';
    પાછા j;
}

/*
 * Sync terminal buffer's cwd with shell's pwd with the help of OSC 7.
 *
 * The OSC 7 sequence has the format of
 * "\033]7;file://HOSTNAME/CURRENT/DIR\033\\"
 * and what VTerm provides via VTermStringFragment is
 * "file://HOSTNAME/CURRENT/DIR"
 */
    સ્થિર શૂન્ય
sync_shell_dir(garray_T *gap)
{
    પૂર્ણાંક       offset = 7;  // len of "file://" is 7
    અક્ષર      *pos = (અક્ષર *)gap->ga_data + offset;
    char_u    *new_dir;

    // remove HOSTNAME to get PWD
    જ્યારે (offset < (પૂર્ણાંક)gap->ga_len && *pos != '/' )
    {
	++offset;
	++pos;
    }

    જો (offset >= (પૂર્ણાંક)gap->ga_len)
    {
	semsg(_(e_failed_to_extract_pwd_from_str_check_your_shell_config),
								 gap->ga_data);
	પાછા;
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
    સ્થિર પૂર્ણાંક
parse_osc(પૂર્ણાંક command, VTermStringFragment frag, શૂન્ય *user)
{
    term_T	*term = (term_T *)user;
    js_read_T	reader;
    typval_T	tv;
    channel_T	*channel = term->tl_job == ખાલી ? ખાલી
						    : term->tl_job->jv_channel;
    garray_T	*gap = &term->tl_osc_buf;

    // We recognize only OSC 5 1 ; {command} and OSC 7 ; {command}
    જો (command != 51 && (command != 7 || !p_asd))
	પાછા 0;

    // Concatenate what was received until the final piece is found.
    જો (ga_grow(gap, (પૂર્ણાંક)frag.len + 1) == FAIL)
    {
	ga_clear(gap);
	પાછા 1;
    }
    mch_memmove((અક્ષર *)gap->ga_data + gap->ga_len, frag.str, frag.len);
    gap->ga_len += (પૂર્ણાંક)frag.len;
    જો (!frag.final)
	પાછા 1;

    ((અક્ષર *)gap->ga_data)[gap->ga_len] = 0;

    જો (command == 7)
    {
	sync_shell_dir(gap);
	ga_clear(gap);
	પાછા 1;
    }

    reader.js_buf = gap->ga_data;
    reader.js_fill = ખાલી;
    reader.js_used = 0;
    જો (json_decode(&reader, &tv, 0) == OK
	    && tv.v_type == VAR_LIST
	    && tv.vval.v_list != ખાલી)
    {
	listitem_T *item = tv.vval.v_list->lv_first;

	જો (item == ખાલી)
	    ch_log(channel, "Missing command");
	નહીંતર
	{
	    char_u	*cmd = tv_get_string(&item->li_tv);

	    // Make sure an invoked command doesn't delete the buffer (and the
	    // terminal) under our fingers.
	    ++term->tl_buffer->b_locked;

	    item = item->li_next;
	    જો (item == ખાલી)
		ch_log(channel, "Missing argument for %s", cmd);
	    નહીંતર જો (STRCMP(cmd, "drop") == 0)
		handle_drop_command(item);
	    નહીંતર જો (STRCMP(cmd, "call") == 0)
		handle_call_command(term, channel, item);
	    નહીંતર
		ch_log(channel, "Invalid command received: %s", cmd);
	    --term->tl_buffer->b_locked;
	}
    }
    નહીંતર
	ch_log(channel, "Invalid JSON received");

    ga_clear(gap);
    clear_tv(&tv);
    પાછા 1;
}

/*
 * Called by libvterm when it cannot recognize a CSI sequence.
 * We recognize the window position report.
 */
    સ્થિર પૂર્ણાંક
parse_csi(
	અચલ અક્ષર  *leader UNUSED,
	અચલ long  args[],
	પૂર્ણાંક	    argcount,
	અચલ અક્ષર  *intermed UNUSED,
	અક્ષર	    command,
	શૂન્ય	    *user)
{
    term_T	*term = (term_T *)user;
    અક્ષર	buf[100];
    પૂર્ણાંક		len;
    પૂર્ણાંક		x = 0;
    પૂર્ણાંક		y = 0;
    win_T	*wp;

    // We recognize only CSI 13 t
    જો (command != 't' || argcount != 1 || args[0] != 13)
	પાછા 0; // not handled

    // When getting the window position is not possible or it fails it results
    // in zero/zero.
#જો defined(FEAT_GUI) \
	|| (defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)) \
	|| defined(MSWIN)
    (શૂન્ય)ui_get_winpos(&x, &y, (varnumber_T)100);
#endif

    FOR_ALL_WINDOWS(wp)
	જો (wp->w_buffer == term->tl_buffer)
	    તોડો;
    જો (wp != ખાલી)
    {
#ifdef FEAT_GUI
	જો (gui.in_use)
	{
	    x += wp->w_wincol * gui.char_width;
	    y += W_WINROW(wp) * gui.char_height;
	}
	નહીંતર
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
						     (char_u *)buf, len, ખાલી);
    પાછા 1;
}

સ્થિર VTermStateFallbacks state_fallbacks = {
    ખાલી,		// control
    parse_csi,		// csi
    parse_osc,		// osc
    ખાલી,		// dcs
    ખાલી,		// apc
    ખાલી,		// pm
    ખાલી		// sos
};

/*
 * Use Vim's allocation functions for vterm so profiling works.
 */
    સ્થિર શૂન્ય *
vterm_malloc(size_t size, શૂન્ય *data UNUSED)
{
    // make sure that the length is not zero
    પાછા alloc_clear(size == 0 ? 1L : size);
}

    સ્થિર શૂન્ય
vterm_memfree(શૂન્ય *ptr, શૂન્ય *data UNUSED)
{
    vim_free(ptr);
}

સ્થિર VTermAllocatorFunctions vterm_allocator = {
  &vterm_malloc,
  &vterm_memfree
};

/*
 * Create a new vterm and initialize it.
 * Return FAIL when out of memory.
 */
    સ્થિર પૂર્ણાંક
create_vterm(term_T *term, પૂર્ણાંક rows, પૂર્ણાંક cols)
{
    VTerm	    *vterm;
    VTermScreen	    *screen;
    VTermState	    *state;
    VTerm_કિંમત	    કિંમત;

    vterm = vterm_new_with_allocator(rows, cols, &vterm_allocator, ખાલી);
    term->tl_vterm = vterm;
    જો (vterm == ખાલી)
	પાછા FAIL;

    // Allocate screen and state here, so we can bail out if that fails.
    state = vterm_obtain_state(vterm);
    screen = vterm_obtain_screen(vterm);
    જો (state == ખાલી || screen == ખાલી)
    {
	vterm_free(vterm);
	પાછા FAIL;
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

    જો (t_colors < 16)
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
    જો (GetCaretBlinkTime() == INFINITE)
	કિંમત.boolean = 0;
    નહીંતર
	કિંમત.boolean = 1;
#નહીંતર
    કિંમત.boolean = 0;
#endif
    vterm_state_set_termprop(state, VTERM_PROP_CURSORBLINK, &કિંમત);
    vterm_state_set_unrecognised_fallbacks(state, &state_fallbacks, term);

    પાછા OK;
}

/*
 * Reset the terminal palette to its default value.
 */
    સ્થિર શૂન્ય
term_reset_palette(VTerm *vterm)
{
    VTermState	*state = vterm_obtain_state(vterm);
    પૂર્ણાંક		index;

    માટે (index = 0; index < 16; index++)
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

    સ્થિર શૂન્ય
term_update_palette(term_T *term)
{
#જો defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    જો (term_use_palette()
	    && (term->tl_palette != ખાલી
		|| find_var((char_u *)"g:terminal_ansi_colors", ખાલી, TRUE)
								      != ખાલી))
    {
	જો (term->tl_palette != ખાલી)
	    set_vterm_palette(term->tl_vterm, term->tl_palette);
	નહીંતર
	    init_vterm_ansi_colors(term->tl_vterm);
    }
    નહીંતર
#endif
	term_reset_palette(term->tl_vterm);
}

/*
 * Called when option 'termguicolors' is changed.
 */
    શૂન્ય
term_update_palette_all(શૂન્ય)
{
    term_T *term;

    FOR_ALL_TERMS(term)
    {
	જો (term->tl_vterm == ખાલી)
	    ચાલુ;
	term_update_palette(term);
    }
}

/*
 * Called when option 'background' or 'termguicolors' was set,
 * or when any highlight is changed.
 */
    શૂન્ય
term_update_colors_all(શૂન્ય)
{
    term_T *term;

    FOR_ALL_TERMS(term)
    {
	જો (term->tl_vterm == ખાલી)
	    ચાલુ;
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
    જો (term->tl_status_text != ખાલી)
	પાછા term->tl_status_text;

    char_u *txt;
    size_t len;
    char_u *fname;

    જો (term->tl_normal_mode)
    {
	જો (term_job_running(term))
	    txt = (char_u *)_("Terminal");
	નહીંતર
	    txt = (char_u *)_("Terminal-finished");
    }
    નહીંતર જો (term->tl_title != ખાલી)
	txt = term->tl_title;
    નહીંતર જો (term_none_open(term))
	txt = (char_u *)_("active");
    નહીંતર જો (term_job_running(term))
	txt = (char_u *)_("running");
    નહીંતર
	txt = (char_u *)_("finished");
    fname = buf_get_fname(term->tl_buffer);
    len = 9 + STRLEN(fname) + STRLEN(txt);
    term->tl_status_text = alloc(len);
    જો (term->tl_status_text != ખાલી)
	vim_snprintf((અક્ષર *)term->tl_status_text, len, "%s [%s]",
		fname, txt);
    પાછા term->tl_status_text;
}

/*
 * Clear the cached value of the status text.
 */
    શૂન્ય
term_clear_status_text(term_T *term)
{
    VIM_CLEAR(term->tl_status_text);
}

/*
 * Mark references in jobs of terminals.
 */
    પૂર્ણાંક
set_ref_in_term(પૂર્ણાંક copyID)
{
    પૂર્ણાંક		abort = FALSE;
    term_T	*term;
    typval_T	tv;

    માટે (term = first_term; !abort && term != ખાલી; term = term->tl_next)
	જો (term->tl_job != ખાલી)
	{
	    tv.v_type = VAR_JOB;
	    tv.vval.v_job = term->tl_job;
	    abort = abort || set_ref_in_item(&tv, copyID, ખાલી, ખાલી, ખાલી);
	}
    પાછા abort;
}

/*
 * Get the buffer from the first argument in "argvars".
 * Returns NULL when the buffer is not for a terminal window and logs a message
 * with "where".
 */
    સ્થિર buf_T *
term_get_buf(typval_T *argvars, અક્ષર *where)
{
    buf_T *buf;

    ++emsg_off;
    buf = tv_get_buf(&argvars[0], FALSE);
    --emsg_off;
    જો (buf == ખાલી || buf->b_term == ખાલી)
    {
	(શૂન્ય)tv_get_સંખ્યા(&argvars[0]);    // issue errmsg if type error
	ch_log(ખાલી, "%s: invalid buffer argument", where);
	પાછા ખાલી;
    }
    પાછા buf;
}

    સ્થિર શૂન્ય
clear_cell(VTermScreenCell *cell)
{
    CLEAR_FIELD(*cell);
    cell->fg.type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_FG;
    cell->bg.type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_BG;
}

    સ્થિર શૂન્ય
dump_term_color(FILE *fd, VTermColor *color)
{
    પૂર્ણાંક index;

    જો (VTERM_COLOR_IS_INDEXED(color))
	index = color->index + 1;
    નહીંતર જો (color->type == 0)
	// use RGB values
	index = 255;
    નહીંતર
	// default color
	index = 0;
    fprintf(fd, "%02x%02x%02x%d",
	    (પૂર્ણાંક)color->red, (પૂર્ણાંક)color->green, (પૂર્ણાંક)color->blue,
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
    શૂન્ય
f_term_dumpwrite(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    term_T	*term;
    char_u	*fname;
    પૂર્ણાંક		max_height = 0;
    પૂર્ણાંક		max_width = 0;
    stat_T	st;
    FILE	*fd;
    VTermPos	pos;
    VTermScreen *screen;
    VTermScreenCell prev_cell;
    VTermState	*state;
    VTermPos	cursor_pos;

    જો (check_restricted() || check_secure())
	પાછા;

    જો (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	પાછા;

    buf = term_get_buf(argvars, "term_dumpwrite()");
    જો (buf == ખાલી)
	પાછા;
    term = buf->b_term;
    જો (term->tl_vterm == ખાલી)
    {
	emsg(_(e_job_already_finished));
	પાછા;
    }

    જો (argvars[2].v_type != VAR_UNKNOWN)
    {
	dict_T *d;

	જો (check_for_dict_arg(argvars, 2) == FAIL)
	    પાછા;
	d = argvars[2].vval.v_dict;
	જો (d != ખાલી)
	{
	    max_height = dict_get_સંખ્યા(d, "rows");
	    max_width = dict_get_સંખ્યા(d, "columns");
	}
    }

    fname = tv_get_string_chk(&argvars[1]);
    જો (fname == ખાલી)
	પાછા;
    જો (mch_stat((અક્ષર *)fname, &st) >= 0)
    {
	semsg(_(e_file_exists_str), fname);
	પાછા;
    }

    જો (*fname == NUL || (fd = mch_fopen((અક્ષર *)fname, WRITEBIN)) == ખાલી)
    {
	semsg(_(e_cant_create_file_str), *fname == NUL ? (char_u *)_("<empty>") : fname);
	પાછા;
    }

    clear_cell(&prev_cell);

    screen = vterm_obtain_screen(term->tl_vterm);
    state = vterm_obtain_state(term->tl_vterm);
    vterm_state_get_cursorpos(state, &cursor_pos);

    માટે (pos.row = 0; (max_height == 0 || pos.row < max_height)
					 && pos.row < term->tl_rows; ++pos.row)
    {
	પૂર્ણાંક		repeat = 0;

	માટે (pos.col = 0; (max_width == 0 || pos.col < max_width)
					 && pos.col < term->tl_cols; ++pos.col)
	{
	    VTermScreenCell cell;
	    પૂર્ણાંક		    same_attr;
	    પૂર્ણાંક		    same_chars = TRUE;
	    પૂર્ણાંક		    i;
	    પૂર્ણાંક		    is_cursor_pos = (pos.col == cursor_pos.col
						 && pos.row == cursor_pos.row);

	    જો (vterm_screen_get_cell(screen, pos, &cell) == 0)
		clear_cell(&cell);

	    માટે (i = 0; i < VTERM_MAX_CHARS_PER_CELL; ++i)
	    {
		પૂર્ણાંક c = cell.chars[i];
		પૂર્ણાંક pc = prev_cell.chars[i];
		પૂર્ણાંક should_break = c == NUL || pc == NUL;

		// For the first character NUL is the same as space.
		જો (i == 0)
		{
		    c = (c == NUL) ? ' ' : c;
		    pc = (pc == NUL) ? ' ' : pc;
		}
		જો (c != pc)
		    same_chars = FALSE;
		જો (should_break)
		    તોડો;
	    }
	    same_attr = vtermAttr2hl(&cell.attrs)
					      == vtermAttr2hl(&prev_cell.attrs)
			&& vterm_color_is_equal(&cell.fg, &prev_cell.fg)
			&& vterm_color_is_equal(&cell.bg, &prev_cell.bg);
	    જો (same_chars && cell.width == prev_cell.width && same_attr
							     && !is_cursor_pos)
	    {
		++repeat;
	    }
	    નહીંતર
	    {
		જો (repeat > 0)
		{
		    fprintf(fd, "@%d", repeat);
		    repeat = 0;
		}
		fputs(is_cursor_pos ? ">" : "|", fd);

		જો (cell.chars[0] == NUL)
		    fputs(" ", fd);
		નહીંતર
		{
		    char_u	charbuf[10];
		    પૂર્ણાંક		len;

		    માટે (i = 0; i < VTERM_MAX_CHARS_PER_CELL
						  && cell.chars[i] != NUL; ++i)
		    {
			len = utf_char2bytes(cell.chars[i], charbuf);
			fwrite(charbuf, len, 1, fd);
		    }
		}

		// When only the characters differ we don't write anything, the
		// following "|", "@" or NL will indicate using the same
		// attributes.
		જો (cell.width != prev_cell.width || !same_attr)
		{
		    જો (cell.width == 2)
			fputs("*", fd);
		    નહીંતર
			fputs("+", fd);

		    જો (same_attr)
		    {
			fputs("&", fd);
		    }
		    નહીંતર
		    {
			fprintf(fd, "%d", vtermAttr2hl(&cell.attrs));
			જો (vterm_color_is_equal(&cell.fg, &prev_cell.fg))
			    fputs("&", fd);
			નહીંતર
			{
			    fputs("#", fd);
			    dump_term_color(fd, &cell.fg);
			}
			જો (vterm_color_is_equal(&cell.bg, &prev_cell.bg))
			    fputs("&", fd);
			નહીંતર
			{
			    fputs("#", fd);
			    dump_term_color(fd, &cell.bg);
			}
		    }
		}

		prev_cell = cell;
	    }

	    જો (cell.width == 2)
		++pos.col;
	}
	જો (repeat > 0)
	    fprintf(fd, "@%d", repeat);
	fputs("\n", fd);
    }

    fclose(fd);
}

/*
 * Called when a dump is corrupted.  Put a breakpoint here when debugging.
 */
    સ્થિર શૂન્ય
dump_is_corrupt(garray_T *gap)
{
    GA_CONCAT_LITERAL(gap, "CORRUPT");
}

    સ્થિર શૂન્ય
append_cell(garray_T *gap, cellattr_T *cell)
{
    જો (ga_grow(gap, 1) == FAIL)
	પાછા;

    *(((cellattr_T *)gap->ga_data) + gap->ga_len) = *cell;
    ++gap->ga_len;
}

    સ્થિર શૂન્ય
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
    સ્થિર પૂર્ણાંક
read_dump_file(FILE *fd, VTermPos *cursor_pos)
{
    પૂર્ણાંક		    c;
    garray_T	    ga_text;
    garray_T	    ga_cell;
    string_T	    prev_char = {ખાલી, 0};
    પૂર્ણાંક		    attr = 0;
    cellattr_T	    cell;
    cellattr_T	    empty_cell;
    term_T	    *term = curbuf->b_term;
    પૂર્ણાંક		    max_cells = 0;
    પૂર્ણાંક		    start_row = term->tl_scrollback.ga_len;

    ga_init2(&ga_text, 1, 90);
    ga_init2(&ga_cell, કદ(cellattr_T), 90);
    clear_cellattr(&cell);
    clear_cellattr(&empty_cell);
    cursor_pos->row = -1;
    cursor_pos->col = -1;

    c = fgetc(fd);
    માટે (;;)
    {
	જો (c == EOF)
	    તોડો;
	જો (c == '\r')
	{
	    // DOS line endings?  Ignore.
	    c = fgetc(fd);
	}
	નહીંતર જો (c == '\n')
	{
	    // End of a line: append it to the buffer.
	    જો (ga_text.ga_data == ખાલી)
		dump_is_corrupt(&ga_text);
	    જો (ga_grow(&term->tl_scrollback, 1) == OK)
	    {
		sb_લીટી_T   *લીટી = (sb_લીટી_T *)term->tl_scrollback.ga_data
						  + term->tl_scrollback.ga_len;

		જો (max_cells < ga_cell.ga_len)
		    max_cells = ga_cell.ga_len;
		લીટી->sb_cols = ga_cell.ga_len;
		લીટી->sb_bytes = ga_text.ga_len;
		લીટી->sb_cells = ga_cell.ga_data;
		લીટી->sb_fill_attr = term->tl_default_color;
		લીટી->continuation = 0;
		++term->tl_scrollback.ga_len;
		ga_init(&ga_cell);

		ga_append(&ga_text, NUL);
		ml_append(curbuf->b_ml.ml_લીટી_ગણતરી, ga_text.ga_data,
							ga_text.ga_len, FALSE);
	    }
	    નહીંતર
		ga_clear(&ga_cell);
	    ga_text.ga_len = 0;

	    c = fgetc(fd);
	}
	નહીંતર જો (c == '|' || c == '>')
	{
	    પૂર્ણાંક prev_len = ga_text.ga_len;

	    જો (c == '>')
	    {
		જો (cursor_pos->row != -1)
		    dump_is_corrupt(&ga_text);	// duplicate cursor
		cursor_pos->row = term->tl_scrollback.ga_len - start_row;
		cursor_pos->col = ga_cell.ga_len;
	    }

	    // normal character(s) followed by "+", "*", "|", "@" or NL
	    c = fgetc(fd);
	    જો (c != EOF)
		ga_append(&ga_text, c);
	    માટે (;;)
	    {
		c = fgetc(fd);
		જો (c == '+' || c == '*' || c == '|' || c == '>' || c == '@'
						      || c == EOF || c == '\n')
		    તોડો;
		ga_append(&ga_text, c);
	    }

	    // save the character for repeating it
	    VIM_CLEAR_STRING(prev_char);
	    જો (ga_text.ga_data != ખાલી)
	    {
		prev_char.length = (size_t)(ga_text.ga_len - prev_len);
		prev_char.string = vim_strnsave(
		    ((char_u *)ga_text.ga_data) + prev_len,
		    prev_char.length);
		જો (prev_char.string == ખાલી)
		    prev_char.length = 0;
	    }

	    જો (c == '@' || c == '|' || c == '>' || c == '\n')
	    {
		// use all attributes from previous cell
	    }
	    નહીંતર જો (c == '+' || c == '*')
	    {
		પૂર્ણાંક is_bg;

		cell.width = c == '+' ? 1 : 2;

		c = fgetc(fd);
		જો (c == '&')
		{
		    // use same attr as previous cell
		    c = fgetc(fd);
		}
		નહીંતર જો (SAFE_isdigit(c))
		{
		    // get the decimal attribute
		    attr = 0;
		    જ્યારે (SAFE_isdigit(c))
		    {
			attr = attr * 10 + (c - '0');
			c = fgetc(fd);
		    }
		    hl2vtermAttr(attr, &cell);

		    // is_bg == 0: fg, is_bg == 1: bg
		    માટે (is_bg = 0; is_bg <= 1; ++is_bg)
		    {
			જો (c == '&')
			{
			    // use same color as previous cell
			    c = fgetc(fd);
			}
			નહીંતર જો (c == '#')
			{
			    પૂર્ણાંક red, green, blue, index = 0, type;

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
			    જો (!SAFE_isdigit(c))
				dump_is_corrupt(&ga_text);
			    જ્યારે (SAFE_isdigit(c))
			    {
				index = index * 10 + (c - '0');
				c = fgetc(fd);
			    }
			    જો (index == 0 || index == 255)
			    {
				type = VTERM_COLOR_RGB;
				જો (index == 0)
				{
				    જો (is_bg)
					type |= VTERM_COLOR_DEFAULT_BG;
				    નહીંતર
					type |= VTERM_COLOR_DEFAULT_FG;
				}
			    }
			    નહીંતર
			    {
				type = VTERM_COLOR_INDEXED;
				index -= 1;
			    }
			    જો (is_bg)
			    {
				cell.bg.type = type;
				cell.bg.red = red;
				cell.bg.green = green;
				cell.bg.blue = blue;
				cell.bg.index = index;
			    }
			    નહીંતર
			    {
				cell.fg.type = type;
				cell.fg.red = red;
				cell.fg.green = green;
				cell.fg.blue = blue;
				cell.fg.index = index;
			    }
			}
			નહીંતર
			    dump_is_corrupt(&ga_text);
		    }
		}
		નહીંતર
		    dump_is_corrupt(&ga_text);
	    }
	    નહીંતર
		dump_is_corrupt(&ga_text);

	    append_cell(&ga_cell, &cell);
	    જો (cell.width == 2)
		append_cell(&ga_cell, &empty_cell);
	}
	નહીંતર જો (c == '@')
	{
	    જો (prev_char.string == ખાલી)
		dump_is_corrupt(&ga_text);
	    નહીંતર
	    {
		પૂર્ણાંક ગણતરી = 0;

		// repeat previous character, get the count
		માટે (;;)
		{
		    c = fgetc(fd);
		    જો (!SAFE_isdigit(c))
			તોડો;
		    ગણતરી = ગણતરી * 10 + (c - '0');
		}

		જ્યારે (ગણતરી-- > 0)
		{
		    ga_concat_len(&ga_text, prev_char.string, prev_char.length);
		    append_cell(&ga_cell, &cell);
		}
	    }
	}
	નહીંતર
	{
	    dump_is_corrupt(&ga_text);
	    c = fgetc(fd);
	}
    }

    જો (ga_text.ga_len > 0)
    {
	// trailing characters after last NL
	dump_is_corrupt(&ga_text);
	ga_append(&ga_text, NUL);
	ml_append(curbuf->b_ml.ml_લીટી_ગણતરી, ga_text.ga_data,
							ga_text.ga_len, FALSE);
    }

    ga_clear(&ga_text);
    ga_clear(&ga_cell);
    vim_free(prev_char.string);

    પાછા max_cells;
}

/*
 * Return an allocated string with at least "text_width" "=" characters and
 * "fname" inserted in the middle.
 */
    સ્થિર char_u *
get_separator(પૂર્ણાંક text_width, char_u *fname)
{
    પૂર્ણાંક	    width = MAX(text_width, curwin->w_width);
    char_u  *textline;
    પૂર્ણાંક	    fname_size;
    char_u  *p = fname;
    પૂર્ણાંક	    i;
    size_t  off;

    textline = alloc(width + STRLEN(fname) + 1);
    જો (textline == ખાલી)
	પાછા ખાલી;

    fname_size = vim_strsize(fname);
    જો (fname_size < width - 8)
    {
	// enough room, don't use the full window width
	width = MAX(text_width, fname_size + 8);
    }
    નહીંતર જો (fname_size > width - 8)
    {
	// full name doesn't fit, use only the tail
	p = gettail(fname);
	fname_size = vim_strsize(p);
    }
    // skip characters until the name fits
    જ્યારે (fname_size > width - 8)
    {
	p += (*mb_ptr2len)(p);
	fname_size = vim_strsize(p);
    }

    માટે (i = 0; i < (width - fname_size) / 2 - 1; ++i)
	textline[i] = '=';
    textline[i++] = ' ';

    STRCPY(textline + i, p);
    off = STRLEN(textline);
    textline[off] = ' ';
    માટે (i = 1; i < (width - fname_size) / 2; ++i)
	textline[off + i] = '=';
    textline[off + i] = NUL;

    પાછા textline;
}

/*
 * Common for "term_dumpdiff()" and "term_dumpload()".
 */
    સ્થિર શૂન્ય
term_load_dump(typval_T *argvars, typval_T *rettv, પૂર્ણાંક do_diff)
{
    jobopt_T	opt;
    buf_T	*buf = ખાલી;
    char_u	buf1[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    char_u	*fname1;
    char_u	*fname2 = ખાલી;
    char_u	*fname_tofree = ખાલી;
    FILE	*fd1;
    FILE	*fd2 = ખાલી;
    char_u	*textline = ખાલી;

    // First open the files.  If this fails bail out.
    fname1 = tv_get_string_buf_chk(&argvars[0], buf1);
    જો (do_diff)
	fname2 = tv_get_string_buf_chk(&argvars[1], buf2);
    જો (fname1 == ખાલી || (do_diff && fname2 == ખાલી))
    {
	emsg(_(e_invalid_argument));
	પાછા;
    }
    fd1 = mch_fopen((અક્ષર *)fname1, READBIN);
    જો (fd1 == ખાલી)
    {
	semsg(_(e_cant_read_file_str), fname1);
	પાછા;
    }
    જો (do_diff)
    {
	fd2 = mch_fopen((અક્ષર *)fname2, READBIN);
	જો (fd2 == ખાલી)
	{
	    fclose(fd1);
	    semsg(_(e_cant_read_file_str), fname2);
	    પાછા;
	}
    }

    init_job_options(&opt);
    જો (argvars[do_diff ? 2 : 1].v_type != VAR_UNKNOWN
	    && get_job_options(&argvars[do_diff ? 2 : 1], &opt, 0,
		    JO2_TERM_NAME + JO2_TERM_COLS + JO2_TERM_ROWS
		    + JO2_VERTICAL + JO2_CURWIN + JO2_NORESTORE) == FAIL)
	જાઓ theend;

    જો (opt.jo_term_name == ખાલી)
    {
	size_t len = STRLEN(fname1) + 12;

	fname_tofree = alloc(len);
	જો (fname_tofree != ખાલી)
	{
	    vim_snprintf((અક્ષર *)fname_tofree, len, "dump diff %s", fname1);
	    opt.jo_term_name = fname_tofree;
	}
    }

    જો (opt.jo_bufnr_buf != ખાલી)
    {
	win_T *wp = buf_jump_open_win(opt.jo_bufnr_buf);

	// With "bufnr" argument: enter the window with this buffer and make it
	// empty.
	જો (wp == ખાલી)
	    semsg(_(e_invalid_argument_str), "bufnr");
	નહીંતર
	{
	    buf = curbuf;
	    જ્યારે (!(curbuf->b_ml.ml_flags & ML_EMPTY))
		ml_delete((linenr_T)1);
	    free_scrollback(curbuf->b_term);
	    redraw_later(UPD_NOT_VALID);
	}
    }
    નહીંતર
	// Create a new terminal window.
	buf = term_start(&argvars[0], ખાલી, &opt, TERM_START_NOJOB);

    જો (buf != ખાલી && buf->b_term != ખાલી)
    {
	પૂર્ણાંક		i;
	linenr_T	bot_lnum;
	linenr_T	lnum;
	term_T		*term = buf->b_term;
	પૂર્ણાંક		width;
	પૂર્ણાંક		width2;
	VTermPos	cursor_pos1;
	VTermPos	cursor_pos2;

	term_init_default_colors(term);

	rettv->vval.v_સંખ્યા = buf->b_fnum;

	// read the files, fill the buffer with the diff
	width = read_dump_file(fd1, &cursor_pos1);

	// position the cursor
	જો (cursor_pos1.row >= 0)
	{
	    curwin->w_cursor.lnum = cursor_pos1.row + 1;
	    coladvance(cursor_pos1.col);
	}

	// Delete the empty line that was in the empty buffer.
	ml_delete(1);

	// For term_dumpload() we are done here.
	જો (!do_diff)
	    જાઓ theend;

	term->tl_ટોચ_diff_rows = curbuf->b_ml.ml_લીટી_ગણતરી;

	textline = get_separator(width, fname1);
	જો (textline == ખાલી)
	    જાઓ theend;
	જો (add_empty_scrollback(term, &term->tl_default_color, 0) == OK)
	    ml_append(curbuf->b_ml.ml_લીટી_ગણતરી, textline, 0, FALSE);
	vim_free(textline);

	textline = get_separator(width, fname2);
	જો (textline == ખાલી)
	    જાઓ theend;
	જો (add_empty_scrollback(term, &term->tl_default_color, 0) == OK)
	    ml_append(curbuf->b_ml.ml_લીટી_ગણતરી, textline, 0, FALSE);
	textline[width] = NUL;

	bot_lnum = curbuf->b_ml.ml_લીટી_ગણતરી;
	width2 = read_dump_file(fd2, &cursor_pos2);
	જો (width2 > width)
	{
	    vim_free(textline);
	    textline = alloc(width2 + 1);
	    જો (textline == ખાલી)
		જાઓ theend;
	    width = width2;
	    textline[width] = NUL;
	}
	term->tl_bot_diff_rows = curbuf->b_ml.ml_લીટી_ગણતરી - bot_lnum;

	માટે (lnum = 1; lnum <= term->tl_ટોચ_diff_rows; ++lnum)
	{
	    જો (lnum + bot_lnum > curbuf->b_ml.ml_લીટી_ગણતરી)
	    {
		// bottom part has fewer rows, fill with "-"
		માટે (i = 0; i < width; ++i)
		    textline[i] = '-';
	    }
	    નહીંતર
	    {
		char_u *line1;
		char_u *line2;
		char_u *p1;
		char_u *p2;
		પૂર્ણાંક	col;
		sb_લીટી_T   *sb_લીટી = (sb_લીટી_T *)term->tl_scrollback.ga_data;
		cellattr_T *cellattr1 = (sb_લીટી + lnum - 1)->sb_cells;
		cellattr_T *cellattr2 = (sb_લીટી + lnum + bot_lnum - 1)
								    ->sb_cells;

		// Make a copy, getting the second line will invalidate it.
		line1 = vim_strsave(ml_get(lnum));
		જો (line1 == ખાલી)
		    તોડો;
		p1 = line1;

		line2 = ml_get(lnum + bot_lnum);
		p2 = line2;
		માટે (col = 0; col < width && *p1 != NUL && *p2 != NUL; ++col)
		{
		    પૂર્ણાંક len1 = utfc_ptr2len(p1);
		    પૂર્ણાંક len2 = utfc_ptr2len(p2);

		    textline[col] = ' ';
		    જો (len1 != len2 || STRNCMP(p1, p2, len1) != 0)
			// text differs
			textline[col] = 'X';
		    નહીંતર જો (lnum == cursor_pos1.row + 1
			    && col == cursor_pos1.col
			    && (cursor_pos1.row != cursor_pos2.row
					|| cursor_pos1.col != cursor_pos2.col))
			// cursor in first but not in second
			textline[col] = '>';
		    નહીંતર જો (lnum == cursor_pos2.row + 1
			    && col == cursor_pos2.col
			    && (cursor_pos1.row != cursor_pos2.row
					|| cursor_pos1.col != cursor_pos2.col))
			// cursor in second but not in first
			textline[col] = '<';
		    નહીંતર જો (cellattr1 != ખાલી && cellattr2 != ખાલી)
		    {
			જો ((cellattr1 + col)->width
						   != (cellattr2 + col)->width)
			    textline[col] = 'w';
			નહીંતર જો (!vterm_color_is_equal(&(cellattr1 + col)->fg,
						   &(cellattr2 + col)->fg))
			    textline[col] = 'f';
			નહીંતર જો (!vterm_color_is_equal(&(cellattr1 + col)->bg,
						   &(cellattr2 + col)->bg))
			    textline[col] = 'b';
			નહીંતર જો (vtermAttr2hl(&(cellattr1 + col)->attrs)
				  != vtermAttr2hl(&((cellattr2 + col)->attrs)))
			    textline[col] = 'a';
		    }
		    p1 += len1;
		    p2 += len2;
		    // TODO: handle different width
		}

		જ્યારે (col < width)
		{
		    જો (*p1 == NUL && *p2 == NUL)
			textline[col] = '?';
		    નહીંતર જો (*p1 == NUL)
		    {
			textline[col] = '+';
			p2 += utfc_ptr2len(p2);
		    }
		    નહીંતર
		    {
			textline[col] = '-';
			p1 += utfc_ptr2len(p1);
		    }
		    ++col;
		}

		vim_free(line1);
	    }
	    જો (add_empty_scrollback(term, &term->tl_default_color,
						 term->tl_ટોચ_diff_rows) == OK)
		ml_append(term->tl_ટોચ_diff_rows + lnum, textline, 0, FALSE);
	    ++bot_lnum;
	}

	જ્યારે (lnum + bot_lnum <= curbuf->b_ml.ml_લીટી_ગણતરી)
	{
	    // bottom part has more rows, fill with "+"
	    માટે (i = 0; i < width; ++i)
		textline[i] = '+';
	    જો (add_empty_scrollback(term, &term->tl_default_color,
						 term->tl_ટોચ_diff_rows) == OK)
		ml_append(term->tl_ટોચ_diff_rows + lnum, textline, 0, FALSE);
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
    જો (fd2 != ખાલી)
	fclose(fd2);
}

/*
 * If the current buffer shows the output of term_dumpdiff(), swap the top and
 * bottom files.
 * Return FAIL when this is not possible.
 */
    પૂર્ણાંક
term_swap_diff(શૂન્ય)
{
    term_T	*term = curbuf->b_term;
    linenr_T	લીટી_ગણતરી;
    linenr_T	ટોચ_rows;
    linenr_T	bot_rows;
    linenr_T	bot_start;
    linenr_T	lnum;
    char_u	*p;
    sb_લીટી_T	*sb_લીટી;

    જો (term == ખાલી
	    || !term_is_finished(curbuf)
	    || term->tl_ટોચ_diff_rows == 0
	    || term->tl_scrollback.ga_len == 0)
	પાછા FAIL;

    લીટી_ગણતરી = curbuf->b_ml.ml_લીટી_ગણતરી;
    ટોચ_rows = term->tl_ટોચ_diff_rows;
    bot_rows = term->tl_bot_diff_rows;
    bot_start = લીટી_ગણતરી - bot_rows;
    sb_લીટી = (sb_લીટી_T *)term->tl_scrollback.ga_data;

    // move lines from top to above the bottom part
    માટે (lnum = 1; lnum <= ટોચ_rows; ++lnum)
    {
	p = vim_strsave(ml_get(1));
	જો (p == ખાલી)
	    પાછા OK;
	ml_append(bot_start, p, 0, FALSE);
	ml_delete(1);
	vim_free(p);
    }

    // move lines from bottom to the top
    માટે (lnum = 1; lnum <= bot_rows; ++lnum)
    {
	p = vim_strsave(ml_get(bot_start + lnum));
	જો (p == ખાલી)
	    પાછા OK;
	ml_delete(bot_start + lnum);
	ml_append(lnum - 1, p, 0, FALSE);
	vim_free(p);
    }

    // move top title to bottom
    p = vim_strsave(ml_get(bot_rows + 1));
    જો (p == ખાલી)
	પાછા OK;
    ml_append(લીટી_ગણતરી - ટોચ_rows - 1, p, 0, FALSE);
    ml_delete(bot_rows + 1);
    vim_free(p);

    // move bottom title to top
    p = vim_strsave(ml_get(લીટી_ગણતરી - ટોચ_rows));
    જો (p == ખાલી)
	પાછા OK;
    ml_delete(લીટી_ગણતરી - ટોચ_rows);
    ml_append(bot_rows, p, 0, FALSE);
    vim_free(p);

    જો (ટોચ_rows == bot_rows)
    {
	// rows counts are equal, can swap cell properties
	માટે (lnum = 0; lnum < ટોચ_rows; ++lnum)
	{
	    sb_લીટી_T	temp;

	    temp = *(sb_લીટી + lnum);
	    *(sb_લીટી + lnum) = *(sb_લીટી + bot_start + lnum);
	    *(sb_લીટી + bot_start + lnum) = temp;
	}
    }
    નહીંતર
    {
	size_t		size = કદ(sb_લીટી_T) * term->tl_scrollback.ga_len;
	sb_લીટી_T	*temp = alloc(size);

	// need to copy cell properties into temp memory
	જો (temp != ખાલી)
	{
	    mch_memmove(temp, term->tl_scrollback.ga_data, size);
	    mch_memmove(term->tl_scrollback.ga_data,
		    temp + bot_start,
		    કદ(sb_લીટી_T) * bot_rows);
	    mch_memmove((sb_લીટી_T *)term->tl_scrollback.ga_data + bot_rows,
		    temp + ટોચ_rows,
		    કદ(sb_લીટી_T) * (લીટી_ગણતરી - ટોચ_rows - bot_rows));
	    mch_memmove((sb_લીટી_T *)term->tl_scrollback.ga_data
						       + લીટી_ગણતરી - ટોચ_rows,
		    temp,
		    કદ(sb_લીટી_T) * ટોચ_rows);
	    vim_free(temp);
	}
    }

    term->tl_ટોચ_diff_rows = bot_rows;
    term->tl_bot_diff_rows = ટોચ_rows;

    update_screen(UPD_NOT_VALID);
    પાછા OK;
}

/*
 * "term_dumpdiff(filename, filename, options)" function
 */
    શૂન્ય
f_term_dumpdiff(typval_T *argvars, typval_T *rettv)
{
    જો (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	પાછા;

    term_load_dump(argvars, rettv, TRUE);
}

/*
 * "term_dumpload(filename, options)" function
 */
    શૂન્ય
f_term_dumpload(typval_T *argvars, typval_T *rettv)
{
    જો (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	પાછા;

    term_load_dump(argvars, rettv, FALSE);
}

/*
 * "term_getaltscreen(buf)" function
 */
    શૂન્ય
f_term_getaltscreen(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    જો (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	પાછા;

    buf = term_get_buf(argvars, "term_getaltscreen()");
    જો (buf == ખાલી)
	પાછા;
    rettv->vval.v_સંખ્યા = buf->b_term->tl_using_altscreen;
}

/*
 * "term_getattr(attr, name)" function
 */
    શૂન્ય
f_term_getattr(typval_T *argvars, typval_T *rettv)
{
    પૂર્ણાંક	    attr;
    size_t  i;
    char_u  *name;

    સ્થિર સંરચના {
	અક્ષર	    *name;
	પૂર્ણાંક	    attr;
    } attrs[] = {
	{"bold",      HL_BOLD},
	{"italic",    HL_ITALIC},
	{"underline", HL_UNDERLINE},
	{"strike",    HL_STRIKETHROUGH},
	{"reverse",   HL_INVERSE},
    };

    જો (in_vim9script()
	    && (check_for_સંખ્યા_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	પાછા;

    attr = tv_get_સંખ્યા(&argvars[0]);
    name = tv_get_string_chk(&argvars[1]);
    જો (name == ખાલી)
	પાછા;

    જો (attr > HL_ALL)
	attr = syn_attr2attr(attr);
    માટે (i = 0; i < ARRAY_LENGTH(attrs); ++i)
	જો (STRCMP(name, attrs[i].name) == 0)
	{
	    rettv->vval.v_સંખ્યા = (attr & attrs[i].attr) != 0 ? 1 : 0;
	    તોડો;
	}
}

/*
 * "term_getcursor(buf)" function
 */
    શૂન્ય
f_term_getcursor(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    term_T	*term;
    list_T	*l;
    dict_T	*d;

    જો (rettv_list_alloc(rettv) == FAIL)
	પાછા;

    જો (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	પાછા;

    buf = term_get_buf(argvars, "term_getcursor()");
    જો (buf == ખાલી)
	પાછા;
    term = buf->b_term;

    l = rettv->vval.v_list;
    list_append_સંખ્યા(l, term->tl_cursor_pos.row + 1);
    list_append_સંખ્યા(l, term->tl_cursor_pos.col + 1);

    d = dict_alloc();
    જો (d == ખાલી)
	પાછા;

    dict_add_સંખ્યા(d, "visible", term->tl_cursor_visible);
    dict_add_સંખ્યા(d, "blink", blink_state_is_inverted()
	    ? !term->tl_cursor_blink : term->tl_cursor_blink);
    dict_add_સંખ્યા(d, "shape", term->tl_cursor_shape);
    dict_add_string(d, "color", cursor_color_get(term->tl_cursor_color));
    જો (list_append_dict(l, d) == FAIL)
	dict_unref(d);
}

/*
 * "term_getjob(buf)" function
 */
    શૂન્ય
f_term_getjob(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    જો (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	પાછા;

    buf = term_get_buf(argvars, "term_getjob()");
    જો (buf == ખાલી)
    {
	જો (in_vim9script())
	{
	    rettv->v_type = VAR_JOB;
	    rettv->vval.v_job = ખાલી;
	}
	નહીંતર
	{
	    rettv->v_type = VAR_SPECIAL;
	    rettv->vval.v_સંખ્યા = VVAL_NULL;
	}
	પાછા;
    }

    rettv->v_type = VAR_JOB;
    rettv->vval.v_job = buf->b_term->tl_job;
    જો (rettv->vval.v_job != ખાલી)
	++rettv->vval.v_job->jv_refcount;
}

    સ્થિર પૂર્ણાંક
get_row_સંખ્યા(typval_T *tv, term_T *term)
{
    જો (tv->v_type == VAR_STRING
	    && tv->vval.v_string != ખાલી
	    && STRCMP(tv->vval.v_string, ".") == 0)
	પાછા term->tl_cursor_pos.row;
    પાછા (પૂર્ણાંક)tv_get_સંખ્યા(tv) - 1;
}

/*
 * "term_getline(buf, row)" function
 */
    શૂન્ય
f_term_getline(typval_T *argvars, typval_T *rettv)
{
    buf_T	    *buf;
    term_T	    *term;
    પૂર્ણાંક		    row;

    rettv->v_type = VAR_STRING;

    જો (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_lnum_arg(argvars, 1) == FAIL))
	પાછા;

    buf = term_get_buf(argvars, "term_getline()");
    જો (buf == ખાલી)
	પાછા;
    term = buf->b_term;
    row = get_row_સંખ્યા(&argvars[1], term);

    જો (term->tl_vterm == ખાલી)
    {
	linenr_T  lnum = 0;
	size_t	  offset = 0;
	પૂર્ણાંક	  sb_row = term->tl_scrollback_scrolled + row;
	sb_લીટી_T *લીટી;

	જો (sb_row < 0 || sb_row >= term->tl_scrollback.ga_len)
	    પાછા;
	લીટી = (sb_લીટી_T *)term->tl_scrollback.ga_data + sb_row;

	scrollbackline_pos_in_buf(term, sb_row, &lnum, ખાલી, &offset);

	// vterm is finished, get the text from the buffer
	જો (lnum > 0 && lnum <= buf->b_ml.ml_લીટી_ગણતરી)
	{
	    char_u *p = ml_get_buf(buf, lnum, FALSE);
	    જો (STRLEN(p) >= offset + લીટી->sb_bytes)
		rettv->vval.v_string = vim_strnsave(p + offset, લીટી->sb_bytes);
	}
    }
    નહીંતર
    {
	VTermScreen	*screen = vterm_obtain_screen(term->tl_vterm);
	VTermRect	rect;
	પૂર્ણાંક		len;
	char_u		*p;

	જો (row < 0 || row >= term->tl_rows)
	    પાછા;
	len = term->tl_cols * MB_MAXBYTES + 1;
	p = alloc(len);
	જો (p == ખાલી)
	    પાછા;
	rettv->vval.v_string = p;

	rect.start_col = 0;
	rect.end_col = term->tl_cols;
	rect.start_row = row;
	rect.end_row = row + 1;
	p[vterm_screen_get_text(screen, (અક્ષર *)p, len, rect)] = NUL;
    }
}

/*
 * "term_getscrolled(buf)" function
 */
    શૂન્ય
f_term_getscrolled(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    જો (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	પાછા;

    buf = term_get_buf(argvars, "term_getscrolled()");
    જો (buf == ખાલી)
	પાછા;
    rettv->vval.v_સંખ્યા = buf->b_term->tl_buffer_scrolled;
}

/*
 * "term_getsize(buf)" function
 */
    શૂન્ય
f_term_getsize(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    list_T	*l;

    જો (rettv_list_alloc(rettv) == FAIL)
	પાછા;

    જો (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	પાછા;

    buf = term_get_buf(argvars, "term_getsize()");
    જો (buf == ખાલી)
	પાછા;

    l = rettv->vval.v_list;
    list_append_સંખ્યા(l, buf->b_term->tl_rows);
    list_append_સંખ્યા(l, buf->b_term->tl_cols);
}

/*
 * "term_setsize(buf, rows, cols)" function
 */
    શૂન્ય
f_term_setsize(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    term_T	*term;
    varnumber_T rows, cols;

    જો (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_સંખ્યા_arg(argvars, 1) == FAIL
		|| check_for_સંખ્યા_arg(argvars, 2) == FAIL))
	પાછા;

    buf = term_get_buf(argvars, "term_setsize()");
    જો (buf == ખાલી)
    {
	emsg(_(e_not_terminal_buffer));
	પાછા;
    }
    જો (buf->b_term->tl_vterm == ખાલી)
	પાછા;
    term = buf->b_term;
    rows = tv_get_સંખ્યા(&argvars[1]);
    rows = rows <= 0 ? term->tl_rows : rows;
    cols = tv_get_સંખ્યા(&argvars[2]);
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
    શૂન્ય
f_term_getstatus(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    term_T	*term;
    char_u	val[100];

    rettv->v_type = VAR_STRING;

    જો (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	પાછા;

    buf = term_get_buf(argvars, "term_getstatus()");
    જો (buf == ખાલી)
	પાછા;
    term = buf->b_term;

    જો (term_job_running(term))
	STRCPY(val, "running");
    નહીંતર
	STRCPY(val, "finished");
    જો (term->tl_normal_mode)
	STRCAT(val, ",normal");
    rettv->vval.v_string = vim_strsave(val);
}

/*
 * "term_gettitle(buf)" function
 */
    શૂન્ય
f_term_gettitle(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    rettv->v_type = VAR_STRING;

    જો (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	પાછા;

    buf = term_get_buf(argvars, "term_gettitle()");
    જો (buf == ખાલી)
	પાછા;

    જો (buf->b_term->tl_title != ખાલી)
	rettv->vval.v_string = vim_strsave(buf->b_term->tl_title);
}

/*
 * "term_gettty(buf)" function
 */
    શૂન્ય
f_term_gettty(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    char_u	*p = ખાલી;
    પૂર્ણાંક		અંક = 0;

    જો (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	પાછા;

    rettv->v_type = VAR_STRING;
    buf = term_get_buf(argvars, "term_gettty()");
    જો (buf == ખાલી)
	પાછા;
    જો (argvars[1].v_type != VAR_UNKNOWN)
	અંક = tv_get_bool(&argvars[1]);

    પસંદગી (અંક)
    {
	સ્થિતિ 0:
	    જો (buf->b_term->tl_job != ખાલી)
		p = buf->b_term->tl_job->jv_tty_out;
	    તોડો;
	સ્થિતિ 1:
	    જો (buf->b_term->tl_job != ખાલી)
		p = buf->b_term->tl_job->jv_tty_in;
	    તોડો;
	મૂળભૂત:
	    semsg(_(e_invalid_argument_str), tv_get_string(&argvars[1]));
	    પાછા;
    }
    જો (p != ખાલી)
	rettv->vval.v_string = vim_strsave(p);
}

/*
 * "term_list()" function
 */
    શૂન્ય
f_term_list(typval_T *argvars UNUSED, typval_T *rettv)
{
    term_T	*tp;
    list_T	*l;

    જો (rettv_list_alloc(rettv) == FAIL || first_term == ખાલી)
	પાછા;

    l = rettv->vval.v_list;
    FOR_ALL_TERMS(tp)
	જો (tp->tl_buffer != ખાલી)
	    જો (list_append_સંખ્યા(l,
				   (varnumber_T)tp->tl_buffer->b_fnum) == FAIL)
		પાછા;
}

/*
 * "term_scrape(buf, row)" function
 */
    શૂન્ય
f_term_scrape(typval_T *argvars, typval_T *rettv)
{
    buf_T	    *buf;
    VTermScreen	    *screen = ખાલી;
    VTermPos	    pos;
    list_T	    *l;
    term_T	    *term;
    char_u	    *p;
    sb_લીટી_T	    *લીટી;

    જો (rettv_list_alloc(rettv) == FAIL)
	પાછા;

    જો (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_lnum_arg(argvars, 1) == FAIL))
	પાછા;

    buf = term_get_buf(argvars, "term_scrape()");
    જો (buf == ખાલી)
	પાછા;
    term = buf->b_term;

    l = rettv->vval.v_list;
    pos.row = get_row_સંખ્યા(&argvars[1], term);

    જો (term->tl_vterm != ખાલી)
    {
	screen = vterm_obtain_screen(term->tl_vterm);
	જો (screen == ખાલી)  // can't really happen
	    પાછા;
	p = ખાલી;
	લીટી = ખાલી;
    }
    નહીંતર
    {
	પૂર્ણાંક	  sb_row = term->tl_scrollback_scrolled + pos.row;
	linenr_T  lnum = 0;
	size_t	  offset = 0;

	scrollbackline_pos_in_buf(term, sb_row, &lnum, ખાલી, &offset);

	જો (sb_row >= term->tl_scrollback.ga_len || lnum <= 0 || lnum > buf->b_ml.ml_લીટી_ગણતરી)
	    પાછા;

	લીટી = (sb_લીટી_T *)term->tl_scrollback.ga_data + sb_row;
	p = ml_get_buf(buf, lnum, FALSE);

	જો (STRLEN(p) < offset + લીટી->sb_bytes)
	    પાછા;

	p += offset;
    }

    માટે (pos.col = 0; pos.col < term->tl_cols; )
    {
	dict_T		*dcell;
	પૂર્ણાંક		width;
	VTermScreenCellAttrs attrs;
	VTermColor	fg, bg;
	char_u		rgb[8];
	size_t		rgblen;
	char_u		mbs[MB_MAXBYTES * VTERM_MAX_CHARS_PER_CELL + 1];
	size_t		mbslen;
	પૂર્ણાંક		i;

	જો (screen == ખાલી)
	{
	    cellattr_T	*cellattr;

	    // vterm has finished, get the cell from scrollback
	    જો (pos.col >= લીટી->sb_cols)
		તોડો;
	    cellattr = લીટી->sb_cells + pos.col;
	    width = cellattr->width;
	    attrs = cellattr->attrs;
	    fg = cellattr->fg;
	    bg = cellattr->bg;
	    mbslen = mb_ptr2len(p);
	    mch_memmove(mbs, p, mbslen);
	    mbs[mbslen] = NUL;
	    p += mbslen;
	}
	નહીંતર
	{
	    VTermScreenCell cell;

	    જો (vterm_screen_get_cell(screen, pos, &cell) == 0)
		તોડો;

	    mbslen = 0;
	    માટે (i = 0; i < VTERM_MAX_CHARS_PER_CELL; ++i)
	    {
		જો (cell.chars[i] == 0)
		    તોડો;
		mbslen += (*utf_char2bytes)((પૂર્ણાંક)cell.chars[i], mbs + mbslen);
	    }
	    mbs[mbslen] = NUL;
	    width = cell.width;
	    attrs = cell.attrs;
	    fg = cell.fg;
	    bg = cell.bg;
	}
	dcell = dict_alloc();
	જો (dcell == ખાલી)
	    તોડો;
	જો (list_append_dict(l, dcell) == FAIL)
	{
	    dict_unref(dcell);
	    તોડો;
	}

	dict_add_string_len(dcell, "chars", mbs, (પૂર્ણાંક)mbslen);

	rgblen = vim_snprintf_safelen((અક્ષર *)rgb, કદ(rgb),
	    "#%02x%02x%02x", fg.red, fg.green, fg.blue);
	dict_add_string_len(dcell, "fg", rgb, (પૂર્ણાંક)rgblen);
	rgblen = vim_snprintf_safelen((અક્ષર *)rgb, કદ(rgb),
	    "#%02x%02x%02x", bg.red, bg.green, bg.blue);
	dict_add_string_len(dcell, "bg", rgb, (પૂર્ણાંક)rgblen);

	dict_add_સંખ્યા(dcell, "attr",
				      cell2attr(term, ખાલી, &attrs, &fg, &bg));
	dict_add_સંખ્યા(dcell, "width", width);

	++pos.col;
	જો (width == 2)
	    ++pos.col;
    }
}

/*
 * "term_sendkeys(buf, keys)" function
 */
    શૂન્ય
f_term_sendkeys(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    char_u	*સંદેશ;
    term_T	*term;

    જો (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	પાછા;

    buf = term_get_buf(argvars, "term_sendkeys()");
    જો (buf == ખાલી)
	પાછા;

    સંદેશ = tv_get_string_chk(&argvars[1]);
    જો (સંદેશ == ખાલી)
	પાછા;
    term = buf->b_term;
    જો (term->tl_vterm == ખાલી)
	પાછા;

    જ્યારે (*સંદેશ != NUL)
    {
	પૂર્ણાંક c;

	જો (*સંદેશ == K_SPECIAL && સંદેશ[1] != NUL && સંદેશ[2] != NUL)
	{
	    c = TO_SPECIAL(સંદેશ[1], સંદેશ[2]);
	    સંદેશ += 3;
	}
	નહીંતર
	{
	    c = PTR2CHAR(સંદેશ);
	    સંદેશ += MB_CPTR2LEN(સંદેશ);
	}
	send_ચાવીઓ_to_term(term, c, 0, FALSE);
    }
}

#જો defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * "term_getansicolors(buf)" function
 */
    શૂન્ય
f_term_getansicolors(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    term_T	*term;
    VTermState	*state;
    VTermColor  color;
    char_u	hexbuf[10];
    પૂર્ણાંક		index;
    list_T	*list;

    જો (rettv_list_alloc(rettv) == FAIL)
	પાછા;

    જો (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	પાછા;

    buf = term_get_buf(argvars, "term_getansicolors()");
    જો (buf == ખાલી)
	પાછા;
    term = buf->b_term;
    જો (term->tl_vterm == ખાલી)
	પાછા;

    list = rettv->vval.v_list;
    state = vterm_obtain_state(term->tl_vterm);
    માટે (index = 0; index < 16; index++)
    {
	size_t	hexbuflen;

	vterm_state_get_palette_color(state, index, &color);
	hexbuflen = vim_snprintf_safelen((અક્ષર *)hexbuf, કદ(hexbuf),
	    "#%02x%02x%02x", color.red, color.green, color.blue);
	જો (list_append_string(list, hexbuf, (પૂર્ણાંક)hexbuflen) == FAIL)
	    પાછા;
    }
}

/*
 * "term_setansicolors(buf, list)" function
 */
    શૂન્ય
f_term_setansicolors(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    term_T	*term;
    listitem_T	*li;
    પૂર્ણાંક		n = 0;

    જો (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL))
	પાછા;

    buf = term_get_buf(argvars, "term_setansicolors()");
    જો (buf == ખાલી)
	પાછા;
    term = buf->b_term;
    જો (term->tl_vterm == ખાલી)
	પાછા;

    જો (check_for_nonnull_list_arg(argvars, 1) == FAIL)
	પાછા;

    જો (argvars[1].vval.v_list->lv_first == &range_list_item
	    || argvars[1].vval.v_list->lv_len != 16)
    {
	semsg(_(e_invalid_કિંમત_for_argument_str), "\"colors\"");
	પાછા;
    }

    જો (term->tl_palette == ખાલી)
	term->tl_palette = ALLOC_MULT(long_u, 16);
    જો (term->tl_palette == ખાલી)
	પાછા;

    FOR_ALL_LIST_વસ્તુઓ(argvars[1].vval.v_list, li)
    {
	char_u		*color_name;
	guicolor_T	guicolor;

	color_name = tv_get_string_chk(&li->li_tv);
	જો (color_name == ખાલી)
	    પાછા;

	guicolor = GUI_GET_COLOR(color_name);
	જો (guicolor == INVALCOLOR)
	{
	    semsg(_(e_cannot_allocate_color_str), color_name);
	    પાછા;
	}

	term->tl_palette[n++] = GUI_MCH_GET_RGB(guicolor);
    }

    term_update_palette(term);
}
#endif

/*
 * "term_setapi(buf, api)" function
 */
    શૂન્ય
f_term_setapi(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    term_T	*term;
    char_u	*api;

    જો (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	પાછા;

    buf = term_get_buf(argvars, "term_setapi()");
    જો (buf == ખાલી)
	પાછા;
    term = buf->b_term;
    vim_free(term->tl_api);
    api = tv_get_string_chk(&argvars[1]);
    જો (api != ખાલી)
	term->tl_api = vim_strsave(api);
    નહીંતર
	term->tl_api = ખાલી;
}

/*
 * "term_setrestore(buf, command)" function
 */
    શૂન્ય
f_term_setrestore(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
#જો defined(FEAT_SESSION)
    buf_T	*buf;
    term_T	*term;
    char_u	*cmd;

    જો (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	પાછા;

    buf = term_get_buf(argvars, "term_setrestore()");
    જો (buf == ખાલી)
	પાછા;
    term = buf->b_term;
    vim_free(term->tl_command);
    cmd = tv_get_string_chk(&argvars[1]);
    જો (cmd != ખાલી)
	term->tl_command = vim_strsave(cmd);
    નહીંતર
	term->tl_command = ખાલી;
#endif
}

/*
 * "term_setkill(buf, how)" function
 */
    શૂન્ય
f_term_setkill(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    term_T	*term;
    char_u	*how;

    જો (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	પાછા;

    buf = term_get_buf(argvars, "term_setkill()");
    જો (buf == ખાલી)
	પાછા;
    term = buf->b_term;
    vim_free(term->tl_kill);
    how = tv_get_string_chk(&argvars[1]);
    જો (how != ખાલી)
	term->tl_kill = vim_strsave(how);
    નહીંતર
	term->tl_kill = ખાલી;
}

/*
 * "term_start(command, options)" function
 */
    શૂન્ય
f_term_start(typval_T *argvars, typval_T *rettv)
{
    jobopt_T	opt;
    buf_T	*buf;

    જો (in_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	પાછા;

    init_job_options(&opt);
    જો (argvars[1].v_type != VAR_UNKNOWN
	    && get_job_options(&argvars[1], &opt,
		JO_TIMEOUT_ALL + JO_STOPONEXIT
		    + JO_CALLBACK + JO_OUT_CALLBACK + JO_ERR_CALLBACK
		    + JO_EXIT_CB + JO_CLOSE_CALLBACK + JO_OUT_IO,
		JO2_TERM_NAME + JO2_TERM_FINISH + JO2_HIDDEN + JO2_TERM_OPENCMD
		    + JO2_TERM_COLS + JO2_TERM_ROWS + JO2_VERTICAL + JO2_CURWIN
		    + JO2_CWD + JO2_ENV + JO2_EOF_CHARS
		    + JO2_NORESTORE + JO2_TERM_KILL + JO2_TERM_HIGHLIGHT
		    + JO2_ANSI_COLORS + JO2_TTY_TYPE + JO2_TERM_API) == FAIL)
	પાછા;

    buf = term_start(&argvars[0], ખાલી, &opt, 0);

    જો (buf != ખાલી && buf->b_term != ખાલી)
	rettv->vval.v_સંખ્યા = buf->b_fnum;
}

/*
 * "term_wait" function
 */
    શૂન્ય
f_term_wait(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;

    જો (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_opt_સંખ્યા_arg(argvars, 1) == FAIL))
	પાછા;

    buf = term_get_buf(argvars, "term_wait()");
    જો (buf == ખાલી)
	પાછા;
    જો (buf->b_term->tl_job == ખાલી)
    {
	ch_log(ખાલી, "term_wait(): no job to wait for");
	પાછા;
    }
    જો (buf->b_term->tl_job->jv_channel == ખાલી)
	// channel is closed, nothing to do
	પાછા;

    // Get the job status, this will detect a job that finished.
    જો (!buf->b_term->tl_job->jv_channel->ch_keep_open
	    && STRCMP(job_status(buf->b_term->tl_job), "dead") == 0)
    {
	// The job is dead, keep reading channel I/O until the channel is
	// closed. buf->b_term may become NULL if the terminal was closed while
	// waiting.
	ch_log(ખાલી, "term_wait(): waiting for channel to close");
	જ્યારે (buf->b_term != ખાલી && !buf->b_term->tl_channel_closed)
	{
	    term_flush_messages();

	    ui_delay(10L, FALSE);
	    જો (!buf_valid(buf))
		// If the terminal is closed when the channel is closed the
		// buffer disappears.
		તોડો;
	    જો (buf->b_term == ખાલી || buf->b_term->tl_channel_closing)
		// came here from a close callback, only wait one time
		તોડો;
	}

	term_flush_messages();
    }
    નહીંતર
    {
	long wait = 10L;

	term_flush_messages();

	// Wait for some time for any channel I/O.
	જો (argvars[1].v_type != VAR_UNKNOWN)
	    wait = tv_get_સંખ્યા(&argvars[1]);
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
    શૂન્ય
term_send_eof(channel_T *ch)
{
    term_T	*term;

    FOR_ALL_TERMS(term)
	જો (term->tl_job == ch->ch_job)
	{
	    જો (term->tl_eof_chars != ખાલી)
	    {
		channel_send(ch, PART_IN, term->tl_eof_chars,
					(પૂર્ણાંક)STRLEN(term->tl_eof_chars), ખાલી);
		channel_send(ch, PART_IN, (char_u *)"\r", 1, ખાલી);
	    }
#ifdef MSWIN
	    નહીંતર
		// Default: CTRL-D
		channel_send(ch, PART_IN, (char_u *)"\004\r", 2, ખાલી);
#endif
	}
}

#જો defined(FEAT_GUI)
    job_T *
term_getjob(term_T *term)
{
    પાછા term != ખાલી ? term->tl_job : ખાલી;
}
#endif

#જો defined(MSWIN)

///////////////////////////////////////
// 2. MS-Windows implementation.

HRESULT (WINAPI *pCreatePseudoConsole)(COORD, HANDLE, HANDLE, DWORD, HPCON*);
HRESULT (WINAPI *pResizePseudoConsole)(HPCON, COORD);
HRESULT (WINAPI *pClosePseudoConsole)(HPCON);
BOOL (WINAPI *pInitializeProcThreadAttributeList)(LPPROC_THREAD_ATTRIBUTE_LIST, DWORD, DWORD, PSIZE_T);
BOOL (WINAPI *pUpdateProcThreadAttribute)(LPPROC_THREAD_ATTRIBUTE_LIST, DWORD, DWORD_PTR, PVOID, SIZE_T, PVOID, PSIZE_T);
શૂન્ય (WINAPI *pDeleteProcThreadAttributeList)(LPPROC_THREAD_ATTRIBUTE_LIST);

    સ્થિર પૂર્ણાંક
dyn_conpty_init(પૂર્ણાંક verbose)
{
    સ્થિર HMODULE	hKerneldll = ખાલી;
    પૂર્ણાંક			i;
    સ્થિર સંરચના
    {
	અક્ષર	*name;
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
	{ખાલી, ખાલી}
    };

    જો (!has_conpty_working())
    {
	જો (verbose)
	    emsg(_(e_conpty_is_not_available));
	પાછા FAIL;
    }

    // No need to initialize twice.
    જો (hKerneldll)
	પાછા OK;

    hKerneldll = vimLoadLib("kernel32.dll");
    માટે (i = 0; conpty_entry[i].name != ખાલી
					&& conpty_entry[i].ptr != ખાલી; ++i)
    {
	જો ((*conpty_entry[i].ptr = (FARPROC)GetProcAddress(hKerneldll,
						conpty_entry[i].name)) == ખાલી)
	{
	    જો (verbose)
		semsg(_(e_could_not_load_library_function_str), conpty_entry[i].name);
	    hKerneldll = ખાલી;
	    પાછા FAIL;
	}
    }

    પાછા OK;
}

    સ્થિર પૂર્ણાંક
conpty_term_and_job_init(
	term_T	    *term,
	typval_T    *argvar,
	અક્ષર	    **argv UNUSED,
	jobopt_T    *opt,
	jobopt_T    *orig_opt)
{
    WCHAR	    *cmd_wchar = ખાલી;
    WCHAR	    *cmd_wchar_copy = ખાલી;
    WCHAR	    *cwd_wchar = ખાલી;
    WCHAR	    *env_wchar = ખાલી;
    channel_T	    *channel = ખાલી;
    job_T	    *job = ખાલી;
    HANDLE	    jo = ખાલી;
    garray_T	    ga_cmd, ga_env;
    char_u	    *cmd = ખાલી;
    HRESULT	    hr;
    COORD	    consize;
    SIZE_T	    breq;
    PROCESS_INFORMATION proc_info;
    HANDLE	    i_theirs = ખાલી;
    HANDLE	    o_theirs = ખાલી;
    HANDLE	    i_ours = ખાલી;
    HANDLE	    o_ours = ખાલી;
    અક્ષર	    *errmsg = ખાલી;

    ga_init2(&ga_cmd, કદ(અક્ષર*), 20);
    ga_init2(&ga_env, કદ(અક્ષર*), 20);

    જો (argvar->v_type == VAR_STRING)
    {
	cmd = argvar->vval.v_string;
    }
    નહીંતર જો (argvar->v_type == VAR_LIST)
    {
	જો (win32_build_cmd(argvar->vval.v_list, &ga_cmd) == FAIL)
	    જાઓ failed;
	cmd = ga_cmd.ga_data;
    }
    જો (cmd == ખાલી || *cmd == NUL)
    {
	emsg(_(e_invalid_argument));
	જાઓ failed;
    }

    term->tl_arg0_cmd = vim_strsave(cmd);

    cmd_wchar = enc_to_utf16(cmd, ખાલી);

    જો (cmd_wchar != ખાલી)
    {
	// Request by CreateProcessW
	breq = wcslen(cmd_wchar) + 1 + 1;	// Addition of NUL by API
	cmd_wchar_copy = ALLOC_MULT(WCHAR, breq);
	wcsncpy(cmd_wchar_copy, cmd_wchar, breq - 1);
    }

    ga_clear(&ga_cmd);
    જો (cmd_wchar == ખાલી)
	જાઓ failed;
    જો (opt->jo_cwd != ખાલી)
    {
	cwd_wchar = enc_to_utf16(opt->jo_cwd, ખાલી);
	જો (cwd_wchar == ખાલી)
	    જાઓ failed;
    }

    win32_build_env(opt->jo_env, &ga_env, TRUE);
    env_wchar = ga_env.ga_data;

    જો (!CreatePipe(&i_theirs, &i_ours, ખાલી, 0))
	જાઓ failed;
    જો (!CreatePipe(&o_ours, &o_theirs, ખાલી, 0))
	જાઓ failed;

    consize.X = term->tl_cols;
    consize.Y = term->tl_rows;
    hr = pCreatePseudoConsole(consize, i_theirs, o_theirs, 0,
							     &term->tl_conpty);
    જો (FAILED(hr))
	જાઓ failed;

    term->tl_siex.StartupInfo.cb = કદ(term->tl_siex);

    // Explicitly invalidate std handles to prevent inheritance of
    // the debugger's stdout (e.g., in Visual Studio debugging sessions),
    // which could cause job output to go to the debugger instead of
    // the intended ConPTY, even with bInheritHandles set to FALSE in CreateProcess.
    term->tl_siex.StartupInfo.dwFlags = STARTF_USESTDHANDLES;
    term->tl_siex.StartupInfo.h_Std_આગત = INVALID_HANDLE_કિંમત;
    term->tl_siex.StartupInfo.hStdOutput = INVALID_HANDLE_કિંમત;
    term->tl_siex.StartupInfo.hStdError = INVALID_HANDLE_કિંમત;

    // Set up pipe inheritance safely: Vista or later.
    pInitializeProcThreadAttributeList(ખાલી, 1, 0, &breq);
    term->tl_siex.lpAttributeList = alloc(breq);
    જો (!term->tl_siex.lpAttributeList)
	જાઓ failed;
    જો (!pInitializeProcThreadAttributeList(term->tl_siex.lpAttributeList, 1,
								     0, &breq))
	જાઓ failed;
    જો (!pUpdateProcThreadAttribute(
	    term->tl_siex.lpAttributeList, 0,
	    PROC_THREAD_ATTRIBUTE_PSEUDOCONSOLE, term->tl_conpty,
	    કદ(HPCON), ખાલી, ખાલી))
	જાઓ failed;

    channel = add_channel();
    જો (channel == ખાલી)
	જાઓ failed;

    job = job_alloc();
    જો (job == ખાલી)
	જાઓ failed;
    જો (argvar->v_type == VAR_STRING)
    {
	પૂર્ણાંક argc;

	build_argv_from_string(cmd, &job->jv_argv, &argc);
    }
    નહીંતર
    {
	પૂર્ણાંક argc;

	build_argv_from_list(argvar->vval.v_list, &job->jv_argv, &argc);
    }

    જો (opt->jo_set & JO_IN_BUF)
	job->jv_in_buf = buflist_findnr(opt->jo_io_buf[PART_IN]);

    જો (!CreateProcessW(ખાલી, cmd_wchar_copy, ખાલી, ખાલી, FALSE,
	    EXTENDED_STARTUPINFO_PRESENT | CREATE_UNICODE_ENVIRONMENT
	    | CREATE_SUSPENDED | CREATE_DEFAULT_ERROR_MODE,
	    env_wchar, cwd_wchar,
	    &term->tl_siex.StartupInfo, &proc_info))
    {
	errmsg = GetWin32Error();
	જાઓ failed;
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

    jo = CreateJobObject(ખાલી, ખાલી);
    જો (jo == ખાલી)
	જાઓ failed;

    જો (!AssignProcessToJobObject(jo, proc_info.hProcess))
    {
	// Failed, switch the way to terminate process with TerminateProcess.
	CloseHandle(jo);
	jo = ખાલી;
    }

    ResumeThread(proc_info.hThread);
    CloseHandle(proc_info.hThread);

    vim_free(cmd_wchar);
    vim_free(cmd_wchar_copy);
    vim_free(cwd_wchar);
    vim_free(env_wchar);

    જો (create_vterm(term, term->tl_rows, term->tl_cols) == FAIL)
	જાઓ failed;

# જો defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    જો (term_use_palette())
    {
	જો (term->tl_palette != ખાલી)
	    set_vterm_palette(term->tl_vterm, term->tl_palette);
	નહીંતર
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
    જો (orig_opt->jo_io[PART_OUT] == JIO_FILE)
    {
	char_u *fname = opt->jo_io_name[PART_OUT];

	ch_log(channel, "Opening output file %s", fname);
	term->tl_out_fd = mch_fopen((અક્ષર *)fname, WRITEBIN);
	જો (term->tl_out_fd == ખાલી)
	    semsg(_(e_cant_open_file_str), fname);
    }

    પાછા OK;

failed:
    ga_clear(&ga_cmd);
    ga_clear(&ga_env);
    vim_free(cmd_wchar);
    vim_free(cmd_wchar_copy);
    vim_free(cwd_wchar);
    જો (channel != ખાલી)
	channel_clear(channel);
    જો (job != ખાલી)
    {
	job->jv_channel = ખાલી;
	job_cleanup(job);
    }
    term->tl_job = ખાલી;
    જો (jo != ખાલી)
	CloseHandle(jo);

    જો (term->tl_siex.lpAttributeList != ખાલી)
    {
	pDeleteProcThreadAttributeList(term->tl_siex.lpAttributeList);
	vim_free(term->tl_siex.lpAttributeList);
    }
    term->tl_siex.lpAttributeList = ખાલી;
    જો (o_theirs != ખાલી)
	CloseHandle(o_theirs);
    જો (o_ours != ખાલી)
	CloseHandle(o_ours);
    જો (i_ours != ખાલી)
	CloseHandle(i_ours);
    જો (i_theirs != ખાલી)
	CloseHandle(i_theirs);
    જો (term->tl_conpty != ખાલી)
	pClosePseudoConsole(term->tl_conpty);
    term->tl_conpty = ખાલી;
    // Propagate errors that occur in CreateProcess
    જો (errmsg)
	semsg("CreateProcess failed: %s", errmsg);
    પાછા FAIL;
}

    સ્થિર શૂન્ય
conpty_term_report_winsize(term_T *term, પૂર્ણાંક rows, પૂર્ણાંક cols)
{
    COORD consize;

    consize.X = cols;
    consize.Y = rows;
    pResizePseudoConsole(term->tl_conpty, consize);
}

    સ્થિર શૂન્ય
term_free_conpty(term_T *term)
{
    જો (term->tl_siex.lpAttributeList != ખાલી)
    {
	pDeleteProcThreadAttributeList(term->tl_siex.lpAttributeList);
	vim_free(term->tl_siex.lpAttributeList);
    }
    term->tl_siex.lpAttributeList = ખાલી;
    જો (term->tl_conpty != ખાલી)
	pClosePseudoConsole(term->tl_conpty);
    term->tl_conpty = ખાલી;
}

    પૂર્ણાંક
use_conpty(શૂન્ય)
{
    પાછા has_conpty;
}

# define WINPTY_SPAWN_FLAG_AUTO_SHUTDOWN 1ul
# define WINPTY_SPAWN_FLAG_EXIT_AFTER_SHUTDOWN 2ull
# define WINPTY_MOUSE_MODE_FORCE		2

શૂન્ય* (*winpty_config_new)(UINT64, શૂન્ય*);
શૂન્ય* (*winpty_open)(શૂન્ય*, શૂન્ય*);
શૂન્ય* (*winpty_spawn_config_new)(UINT64, શૂન્ય*, LPCWSTR, શૂન્ય*, શૂન્ય*, શૂન્ય*);
BOOL (*winpty_spawn)(શૂન્ય*, શૂન્ય*, HANDLE*, HANDLE*, DWORD*, શૂન્ય*);
શૂન્ય (*winpty_config_set_mouse_mode)(શૂન્ય*, પૂર્ણાંક);
શૂન્ય (*winpty_config_set_initial_size)(શૂન્ય*, પૂર્ણાંક, પૂર્ણાંક);
LPCWSTR (*winpty_conin_name)(શૂન્ય*);
LPCWSTR (*winpty_conout_name)(શૂન્ય*);
LPCWSTR (*winpty_conerr_name)(શૂન્ય*);
શૂન્ય (*winpty_free)(શૂન્ય*);
શૂન્ય (*winpty_config_free)(શૂન્ય*);
શૂન્ય (*winpty_spawn_config_free)(શૂન્ય*);
શૂન્ય (*winpty_error_free)(શૂન્ય*);
LPCWSTR (*winpty_error_સંદેશ)(શૂન્ય*);
BOOL (*winpty_set_size)(શૂન્ય*, પૂર્ણાંક, પૂર્ણાંક, શૂન્ય*);
HANDLE (*winpty_agent_process)(શૂન્ય*);

# define WINPTY_DLL "winpty.dll"

સ્થિર HINSTANCE hWinPtyDLL = ખાલી;

    સ્થિર પૂર્ણાંક
dyn_winpty_init(પૂર્ણાંક verbose)
{
    પૂર્ણાંક i;
    સ્થિર સંરચના
    {
	અક્ષર	    *name;
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
	{"winpty_error_msg", (FARPROC*)&winpty_error_સંદેશ},
	{"winpty_set_size", (FARPROC*)&winpty_set_size},
	{"winpty_agent_process", (FARPROC*)&winpty_agent_process},
	{ખાલી, ખાલી}
    };

    // No need to initialize twice.
    જો (hWinPtyDLL)
	પાછા OK;
    // Load winpty.dll, prefer using the 'winptydll' option, fall back to just
    // winpty.dll.
    જો (*p_winptydll != NUL)
	hWinPtyDLL = vimLoadLib((અક્ષર *)p_winptydll);
    જો (!hWinPtyDLL)
	hWinPtyDLL = vimLoadLib(WINPTY_DLL);
    જો (!hWinPtyDLL)
    {
	જો (verbose)
	    semsg(_(e_could_not_load_library_str_str),
		    (*p_winptydll != NUL ? p_winptydll : (char_u *)WINPTY_DLL),
		    GetWin32Error());
	પાછા FAIL;
    }
    માટે (i = 0; winpty_entry[i].name != ખાલી
					 && winpty_entry[i].ptr != ખાલી; ++i)
    {
	જો ((*winpty_entry[i].ptr = (FARPROC)GetProcAddress(hWinPtyDLL,
					      winpty_entry[i].name)) == ખાલી)
	{
	    જો (verbose)
		semsg(_(e_could_not_load_library_function_str), winpty_entry[i].name);
	    hWinPtyDLL = ખાલી;
	    પાછા FAIL;
	}
    }

    પાછા OK;
}

    સ્થિર પૂર્ણાંક
winpty_term_and_job_init(
	term_T	    *term,
	typval_T    *argvar,
	અક્ષર	    **argv UNUSED,
	jobopt_T    *opt,
	jobopt_T    *orig_opt)
{
    WCHAR	    *cmd_wchar = ખાલી;
    WCHAR	    *cwd_wchar = ખાલી;
    WCHAR	    *env_wchar = ખાલી;
    channel_T	    *channel = ખાલી;
    job_T	    *job = ખાલી;
    DWORD	    error;
    HANDLE	    jo = ખાલી;
    HANDLE	    child_process_handle;
    HANDLE	    child_thread_handle;
    શૂન્ય	    *winpty_err = ખાલી;
    શૂન્ય	    *spawn_config = ખાલી;
    garray_T	    ga_cmd, ga_env;
    char_u	    *cmd = ખાલી;

    ga_init2(&ga_cmd, કદ(અક્ષર*), 20);
    ga_init2(&ga_env, કદ(અક્ષર*), 20);

    જો (argvar->v_type == VAR_STRING)
    {
	cmd = argvar->vval.v_string;
    }
    નહીંતર જો (argvar->v_type == VAR_LIST)
    {
	જો (win32_build_cmd(argvar->vval.v_list, &ga_cmd) == FAIL)
	    જાઓ failed;
	cmd = ga_cmd.ga_data;
    }
    જો (cmd == ખાલી || *cmd == NUL)
    {
	emsg(_(e_invalid_argument));
	જાઓ failed;
    }

    term->tl_arg0_cmd = vim_strsave(cmd);

    cmd_wchar = enc_to_utf16(cmd, ખાલી);
    ga_clear(&ga_cmd);
    જો (cmd_wchar == ખાલી)
	જાઓ failed;
    જો (opt->jo_cwd != ખાલી)
    {
	cwd_wchar = enc_to_utf16(opt->jo_cwd, ખાલી);
	જો (cwd_wchar == ખાલી)
	    જાઓ failed;
    }

    win32_build_env(opt->jo_env, &ga_env, TRUE);
    env_wchar = ga_env.ga_data;

    term->tl_winpty_config = winpty_config_new(0, &winpty_err);
    જો (term->tl_winpty_config == ખાલી)
	જાઓ failed;

    winpty_config_set_mouse_mode(term->tl_winpty_config,
						    WINPTY_MOUSE_MODE_FORCE);
    winpty_config_set_initial_size(term->tl_winpty_config,
						 term->tl_cols, term->tl_rows);
    term->tl_winpty = winpty_open(term->tl_winpty_config, &winpty_err);
    જો (term->tl_winpty == ખાલી)
	જાઓ failed;

    spawn_config = winpty_spawn_config_new(
	    WINPTY_SPAWN_FLAG_AUTO_SHUTDOWN |
		WINPTY_SPAWN_FLAG_EXIT_AFTER_SHUTDOWN,
	    ખાલી,
	    cmd_wchar,
	    cwd_wchar,
	    env_wchar,
	    &winpty_err);
    જો (spawn_config == ખાલી)
	જાઓ failed;

    channel = add_channel();
    જો (channel == ખાલી)
	જાઓ failed;

    job = job_alloc();
    જો (job == ખાલી)
	જાઓ failed;
    જો (argvar->v_type == VAR_STRING)
    {
	પૂર્ણાંક argc;

	build_argv_from_string(cmd, &job->jv_argv, &argc);
    }
    નહીંતર
    {
	પૂર્ણાંક argc;

	build_argv_from_list(argvar->vval.v_list, &job->jv_argv, &argc);
    }

    જો (opt->jo_set & JO_IN_BUF)
	job->jv_in_buf = buflist_findnr(opt->jo_io_buf[PART_IN]);

    જો (!winpty_spawn(term->tl_winpty, spawn_config, &child_process_handle,
	    &child_thread_handle, &error, &winpty_err))
	જાઓ failed;

    channel_set_pipes(channel,
	(sock_T)CreateFileW(
	    winpty_conin_name(term->tl_winpty),
	    GENERIC_WRITE, 0, ખાલી,
	    OPEN_EXISTING, 0, ખાલી),
	(sock_T)CreateFileW(
	    winpty_conout_name(term->tl_winpty),
	    GENERIC_READ, 0, ખાલી,
	    OPEN_EXISTING, 0, ખાલી),
	(sock_T)CreateFileW(
	    winpty_conerr_name(term->tl_winpty),
	    GENERIC_READ, 0, ખાલી,
	    OPEN_EXISTING, 0, ખાલી));

    // Write lines with CR instead of NL.
    channel->ch_write_text_mode = TRUE;

    jo = CreateJobObject(ખાલી, ખાલી);
    જો (jo == ખાલી)
	જાઓ failed;

    જો (!AssignProcessToJobObject(jo, child_process_handle))
    {
	// Failed, switch the way to terminate process with TerminateProcess.
	CloseHandle(jo);
	jo = ખાલી;
    }

    winpty_spawn_config_free(spawn_config);
    vim_free(cmd_wchar);
    vim_free(cwd_wchar);
    vim_free(env_wchar);

    જો (create_vterm(term, term->tl_rows, term->tl_cols) == FAIL)
	જાઓ failed;

# જો defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    જો (term_use_palette())
    {
	જો (term->tl_palette != ખાલી)
	    set_vterm_palette(term->tl_vterm, term->tl_palette);
	નહીંતર
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
	    (short_u *)winpty_conin_name(term->tl_winpty), ખાલી);
    job->jv_tty_out = utf16_to_enc(
	    (short_u *)winpty_conout_name(term->tl_winpty), ખાલી);
    job->jv_tty_type = vim_strsave((char_u *)"winpty");
    ++job->jv_refcount;
    term->tl_job = job;

    // Redirecting stdout and stderr doesn't work at the job level.  Instead
    // open the file here and handle it in.  opt->jo_io was changed in
    // setup_job_options(), use the original flags here.
    જો (orig_opt->jo_io[PART_OUT] == JIO_FILE)
    {
	char_u *fname = opt->jo_io_name[PART_OUT];

	ch_log(channel, "Opening output file %s", fname);
	term->tl_out_fd = mch_fopen((અક્ષર *)fname, WRITEBIN);
	જો (term->tl_out_fd == ખાલી)
	    semsg(_(e_cant_open_file_str), fname);
    }

    પાછા OK;

failed:
    ga_clear(&ga_cmd);
    ga_clear(&ga_env);
    vim_free(cmd_wchar);
    vim_free(cwd_wchar);
    જો (spawn_config != ખાલી)
	winpty_spawn_config_free(spawn_config);
    જો (channel != ખાલી)
	channel_clear(channel);
    જો (job != ખાલી)
    {
	job->jv_channel = ખાલી;
	job_cleanup(job);
    }
    term->tl_job = ખાલી;
    જો (jo != ખાલી)
	CloseHandle(jo);
    જો (term->tl_winpty != ખાલી)
	winpty_free(term->tl_winpty);
    term->tl_winpty = ખાલી;
    જો (term->tl_winpty_config != ખાલી)
	winpty_config_free(term->tl_winpty_config);
    term->tl_winpty_config = ખાલી;
    જો (winpty_err != ખાલી)
    {
	અક્ષર *સંદેશ = (અક્ષર *)utf16_to_enc(
				(short_u *)winpty_error_સંદેશ(winpty_err), ખાલી);

	જો (સંદેશ != ખાલી)
	{
	    emsg(સંદેશ);
	    vim_free(સંદેશ);
	}
	winpty_error_free(winpty_err);
    }
    પાછા FAIL;
}

/*
 * Create a new terminal of "rows" by "cols" cells.
 * Store a reference in "term".
 * Return OK or FAIL.
 */
    સ્થિર પૂર્ણાંક
term_and_job_init(
	term_T	    *term,
	typval_T    *argvar,
	અક્ષર	    **argv,
	jobopt_T    *opt,
	jobopt_T    *orig_opt)
{
    પૂર્ણાંક		    use_winpty = FALSE;
    પૂર્ણાંક		    use_conpty = FALSE;
    પૂર્ણાંક		    tty_type = *p_twt;

    has_winpty = dyn_winpty_init(FALSE) != FAIL ? TRUE : FALSE;
    has_conpty = dyn_conpty_init(FALSE) != FAIL ? TRUE : FALSE;

    જો (!has_winpty && !has_conpty)
	// If neither is available give the errors for winpty, since when
	// conpty is not available it can't be installed either.
	પાછા dyn_winpty_init(TRUE);

    જો (opt->jo_tty_type != NUL)
	tty_type = opt->jo_tty_type;

    જો (tty_type == NUL)
    {
	જો (has_conpty && (is_conpty_stable() || !has_winpty))
	    use_conpty = TRUE;
	નહીંતર જો (has_winpty)
	    use_winpty = TRUE;
	// else: error
    }
    નહીંતર જો (tty_type == 'w')	// winpty
    {
	જો (has_winpty)
	    use_winpty = TRUE;
    }
    નહીંતર જો (tty_type == 'c')	// conpty
    {
	જો (has_conpty)
	    use_conpty = TRUE;
	નહીંતર
	    પાછા dyn_conpty_init(TRUE);
    }

    જો (use_conpty)
	પાછા conpty_term_and_job_init(term, argvar, argv, opt, orig_opt);

    જો (use_winpty)
	પાછા winpty_term_and_job_init(term, argvar, argv, opt, orig_opt);

    // error
    પાછા dyn_winpty_init(TRUE);
}

    સ્થિર પૂર્ણાંક
create_pty_only(term_T *term, jobopt_T *options)
{
    HANDLE	    hPipeIn = INVALID_HANDLE_કિંમત;
    HANDLE	    hPipeOut = INVALID_HANDLE_કિંમત;
    અક્ષર	    in_name[80], out_name[80];
    channel_T	    *channel = ખાલી;

    જો (create_vterm(term, term->tl_rows, term->tl_cols) == FAIL)
	પાછા FAIL;

    vim_snprintf(in_name, કદ(in_name), "\\\\.\\pipe\\vim-%d-in-%d",
	    GetCurrentProcessId(),
	    curbuf->b_fnum);
    hPipeIn = CreateNamedPipe(in_name, PIPE_ACCESS_OUTBOUND,
	    PIPE_TYPE_MESSAGE | PIPE_NOWAIT,
	    PIPE_UNLIMITED_INSTANCES,
	    0, 0, NMPWAIT_NOWAIT, ખાલી);
    જો (hPipeIn == INVALID_HANDLE_કિંમત)
	જાઓ failed;

    vim_snprintf(out_name, કદ(out_name), "\\\\.\\pipe\\vim-%d-out-%d",
	    GetCurrentProcessId(),
	    curbuf->b_fnum);
    hPipeOut = CreateNamedPipe(out_name, PIPE_ACCESS_INBOUND,
	    PIPE_TYPE_MESSAGE | PIPE_NOWAIT,
	    PIPE_UNLIMITED_INSTANCES,
	    0, 0, 0, ખાલી);
    જો (hPipeOut == INVALID_HANDLE_કિંમત)
	જાઓ failed;

    ConnectNamedPipe(hPipeIn, ખાલી);
    ConnectNamedPipe(hPipeOut, ખાલી);

    term->tl_job = job_alloc();
    જો (term->tl_job == ખાલી)
	જાઓ failed;
    ++term->tl_job->jv_refcount;

    // behave like the job is already finished
    term->tl_job->jv_status = JOB_FINISHED;

    channel = add_channel();
    જો (channel == ખાલી)
	જાઓ failed;
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

    પાછા OK;

failed:
    જો (hPipeIn != ખાલી)
	CloseHandle(hPipeIn);
    જો (hPipeOut != ખાલી)
	CloseHandle(hPipeOut);
    પાછા FAIL;
}

/*
 * Free the terminal emulator part of "term".
 */
    સ્થિર શૂન્ય
term_free_vterm(term_T *term)
{
    term_free_conpty(term);
    જો (term->tl_winpty != ખાલી)
	winpty_free(term->tl_winpty);
    term->tl_winpty = ખાલી;
    જો (term->tl_winpty_config != ખાલી)
	winpty_config_free(term->tl_winpty_config);
    term->tl_winpty_config = ખાલી;
    જો (term->tl_vterm != ખાલી)
	vterm_free(term->tl_vterm);
    term->tl_vterm = ખાલી;
}

/*
 * Report the size to the terminal.
 */
    સ્થિર શૂન્ય
term_report_winsize(term_T *term, પૂર્ણાંક rows, પૂર્ણાંક cols)
{
    જો (term->tl_conpty)
	conpty_term_report_winsize(term, rows, cols);
    જો (term->tl_winpty)
	winpty_set_size(term->tl_winpty, cols, rows, ખાલી);
}

    પૂર્ણાંક
terminal_enabled(શૂન્ય)
{
    પાછા dyn_winpty_init(FALSE) == OK || dyn_conpty_init(FALSE) == OK;
}

#નહીંતર

///////////////////////////////////////
// 3. Unix-like implementation.

/*
 * Create a new terminal of "rows" by "cols" cells.
 * Start job for "cmd".
 * Store the pointers in "term".
 * When "argv" is not NULL then "argvar" is not used.
 * Return OK or FAIL.
 */
    સ્થિર પૂર્ણાંક
term_and_job_init(
	term_T	    *term,
	typval_T    *argvar,
	અક્ષર	    **argv,
	jobopt_T    *opt,
	jobopt_T    *orig_opt UNUSED)
{
    term->tl_arg0_cmd = ખાલી;

    જો (create_vterm(term, term->tl_rows, term->tl_cols) == FAIL)
	પાછા FAIL;

# જો defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    જો (term_use_palette())
    {
	જો (term->tl_palette != ખાલી)
	    set_vterm_palette(term->tl_vterm, term->tl_palette);
	નહીંતર
	    init_vterm_ansi_colors(term->tl_vterm);
    }
# endif

    // This may change a string in "argvar".
    term->tl_job = job_start(argvar, argv, opt, &term->tl_job);
    જો (term->tl_job != ખાલી)
	++term->tl_job->jv_refcount;

    પાછા term->tl_job != ખાલી
	&& term->tl_job->jv_channel != ખાલી
	&& term->tl_job->jv_status != JOB_FAILED ? OK : FAIL;
}

    સ્થિર પૂર્ણાંક
create_pty_only(term_T *term, jobopt_T *opt)
{
    જો (create_vterm(term, term->tl_rows, term->tl_cols) == FAIL)
	પાછા FAIL;

    term->tl_job = job_alloc();
    જો (term->tl_job == ખાલી)
	પાછા FAIL;
    ++term->tl_job->jv_refcount;

    // behave like the job is already finished
    term->tl_job->jv_status = JOB_FINISHED;

    પાછા mch_create_pty_channel(term->tl_job, opt);
}

/*
 * Free the terminal emulator part of "term".
 */
    સ્થિર શૂન્ય
term_free_vterm(term_T *term)
{
    જો (term->tl_vterm != ખાલી)
	vterm_free(term->tl_vterm);
    term->tl_vterm = ખાલી;
}

/*
 * Report the size to the terminal.
 */
    સ્થિર શૂન્ય
term_report_winsize(term_T *term, પૂર્ણાંક rows, પૂર્ણાંક cols)
{
    // Use an ioctl() to report the new window size to the job.
    જો (term->tl_job == ખાલી || term->tl_job->jv_channel == ખાલી)
	પાછા;

    પૂર્ણાંક fd = -1;
    પૂર્ણાંક part;

    માટે (part = PART_OUT; part < PART_ગણતરી; ++part)
    {
	fd = term->tl_job->jv_channel->ch_part[part].ch_fd;
	જો (mch_isatty(fd))
	    તોડો;
    }
    જો (part < PART_ગણતરી && mch_report_winsize(fd, rows, cols) == OK)
	mch_signal_job(term->tl_job, (char_u *)"winch");
}

#endif

#endif // FEAT_TERMINAL
