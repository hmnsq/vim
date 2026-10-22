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

#అన్తర్భావ "vim.h"

#ఒకవేళ defined(FEAT_TERMINAL)

#నిర్వచితం_కాకపోతే MIN
# define MIN(x,y) ((x) < (y) ? (x) : (y))
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే MAX
# define MAX(x,y) ((x) > (y) ? (x) : (y))
#అయితే_ముగింపు

#అన్తర్భావ "libvterm/include/vterm.h"

// This is VTermScreenCell without the characters, thus much smaller.
రకనిర్వచనం నిర్మాణం {
    VTermScreenCellAttrs	attrs;
    అక్షరం			వెడల్పు;
    VTermColor			fg;
    VTermColor			bg;
} cellattr_T;

రకనిర్వచనం నిర్మాణం sb_పంక్తి_S {
    పూర్ణాంకం		sb_నిలువుల_సంఖ్య;	// can differ per line
    పూర్ణాంకం		sb_బైట్లు;	// length in bytes of text
    cellattr_T	*sb_cells;	// allocated
    cellattr_T	sb_fill_attr;	// for short line
    char_u	*sb_పాఠ్యం;	// for tl_scrollback_postponed
    char_u	continuation;
} sb_పంక్తి_T;

#నిర్వచితమైతే MSWIN
# ifndef HPCON
#  define HPCON VOID*
# endif
# ifndef EXTENDED_STARTUPINFO_PRESENT
#  define EXTENDED_STARTUPINFO_PRESENT 0x00080000
# endif
# ifndef PROC_THREAD_ATTRIBUTE_PSEUDOCONSOLE
#  define PROC_THREAD_ATTRIBUTE_PSEUDOCONSOLE 0x00020016
# endif
రకనిర్వచనం నిర్మాణం _DYN_STARTUPINFOEXW
{
    STARTUPINFOW StartupInfo;
    LPPROC_THREAD_ATTRIBUTE_LIST lpAttributeList;
} DYN_STARTUPINFOEXW, *PDYN_STARTUPINFOEXW;
#అయితే_ముగింపు

// typedef term_T in structs.h
నిర్మాణం terminal_S {
    పదం_T	*tl_తదుపరి;

    VTerm	*tl_vterm;
    పని_T	*tl_పని;
    buf_T	*tl_బఫరు;
#ఒకవేళ defined(FEAT_GUI)
    పూర్ణాంకం		tl_system;	// when non-zero used for :!cmd output
    పూర్ణాంకం		tl_toprow;	// row with first line of system terminal
#అయితే_ముగింపు

    // Set when setting the size of a vterm, reset after redrawing.
    పూర్ణాంకం		tl_vterm_కొలత_మారింది;

    పూర్ణాంకం		tl_సాధారణ_బహుళకం; // TRUE: Terminal-Normal mode
    పూర్ణాంకం		tl_channel_closing;
    పూర్ణాంకం		tl_channel_closed;
    పూర్ణాంకం		tl_channel_recently_closed; // still need to handle tl_finish

    పూర్ణాంకం		tl_ముగింపు;
#నిర్వచించు TL_ముగింపు_UNSET	    NUL
#నిర్వచించు TL_ముగింపు_CLOSE	    'c'	// ++close or :terminal without argument
#నిర్వచించు TL_ముగింపు_NOCLOSE   'n'	// ++noclose
#నిర్వచించు TL_ముగింపు_OPEN	    'o'	// ++open
    char_u	*tl_opencmd;
    char_u	*tl_eof_chars;
    char_u	*tl_api;	// prefix for terminal API function

    char_u	*tl_arg0_cmd;	// To format the status bar

#నిర్వచితమైతే MSWIN
    శూన్య	*tl_winpty_config;
    శూన్య	*tl_winpty;

    HPCON	tl_conpty;
    DYN_STARTUPINFOEXW tl_siex;	// Structure that always needs to be hold

    FILE	*tl_బహిర్గతం_fd;
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_SESSION)
    char_u	*tl_command;
#అయితే_ముగింపు
    char_u	*tl_kill;

    // last known vterm size
    పూర్ణాంకం		tl_అడ్డువరుసలు;
    పూర్ణాంకం		tl_నిలువుల_సంఖ్య;

    char_u	*tl_title; // NULL or allocated
    char_u	*tl_స్థితి_పాఠ్యం; // NULL or allocated

    // Range of screen rows to update.  Zero based.
    పూర్ణాంకం		tl_dirty_అడ్డువరుస_ప్రారంభం; // MAX_ROW if nothing dirty
    పూర్ణాంకం		tl_dirty_అడ్డువరుస_చివర;   // row below last one to update
    పూర్ణాంకం		tl_dirty_snapshot;  // text updated after making snapshot
#నిర్వచితమైతే FEAT_TIMERS
    పూర్ణాంకం		tl_timer_సమితి;
    proftime_T	tl_timer_due;
#అయితే_ముగింపు
    పూర్ణాంకం		tl_postponed_scroll;	// to be scrolled up

    garray_T	tl_scrollback;
    పూర్ణాంకం		tl_scrollback_scrolled;
    garray_T	tl_scrollback_postponed;
    పూర్ణాంకం		tl_scrollback_snapshot;
    పూర్ణాంకం		tl_బఫరు_scrolled;

    char_u	*tl_highlight_name; // replaces "Terminal"; allocated

    cellattr_T	tl_default_color;

    linenr_T	tl_శిఖరం_diff_అడ్డువరుసలు;   // rows of top diff file or zero
    linenr_T	tl_bot_diff_అడ్డువరుసలు;   // rows of bottom diff file

    VTermPos	tl_cursor_pos;
    పూర్ణాంకం		tl_cursor_visible;
    పూర్ణాంకం		tl_cursor_blink;
    పూర్ణాంకం		tl_cursor_shape;  // 1: block, 2: underline, 3: bar
    char_u	*tl_cursor_color; // NULL or allocated

    long_u	*tl_palette; // array of 16 colors specified by term_start, can
			     // be NULL
    పూర్ణాంకం		tl_using_altscreen;
    garray_T	tl_osc_buf;	    // incomplete OSC string
};

#నిర్వచించు TMODE_ONCE 1	    // CTRL-\ CTRL-N used
#నిర్వచించు TMODE_LOOP 2	    // CTRL-W N used

/*
 * List of all active terminals.
 */
స్థిర పదం_T *మొదటి_పదం = ఖాళీ;

// Terminal active in terminal_loop().
స్థిర పదం_T *లోపల_terminal_loop = ఖాళీ;

#నిర్వచితమైతే MSWIN
స్థిర BOOL has_winpty = FALSE;
స్థిర BOOL has_conpty = FALSE;
#అయితే_ముగింపు

#నిర్వచించు MAX_అడ్డువరుస 999999	    // used for tl_dirty_row_end to update all rows
#నిర్వచించు కీలకం_BUF_LEN 200

#నిర్వచించు FOR_ALL_TERMS(పదం)	\
    కొరకు ((పదం) = మొదటి_పదం; (పదం) != ఖాళీ; (పదం) = (పదం)->tl_తదుపరి)

/*
 * Functions with separate implementation for MS-Windows and Unix-like systems.
 */
స్థిర పూర్ణాంకం పదం_and_పని_init(పదం_T *పదం, typval_T *argvar, అక్షరం **argv, jobopt_T *opt, jobopt_T *orig_opt);
స్థిర పూర్ణాంకం create_pty_only(పదం_T *పదం, jobopt_T *opt);
స్థిర శూన్య పదం_report_winsize(పదం_T *పదం, పూర్ణాంకం అడ్డువరుసలు_2, పూర్ణాంకం నిలువుల_సంఖ్య);
స్థిర శూన్య పదం_free_vterm(పదం_T *పదం);
#నిర్వచితమైతే FEAT_GUI
స్థిర శూన్య update_system_పదం(పదం_T *పదం);
#అయితే_ముగింపు

స్థిర శూన్య handle_postponed_scrollback(పదం_T *పదం);

// The character that we know (or assume) that the terminal expects for the
// backspace key.
స్థిర పూర్ణాంకం పదం_backspace_char = BS;

// Store the last set and the desired cursor properties, so that we only update
// them when needed.  Doing it unnecessary may result in flicker.
స్థిర char_u	*చివరి_సమితి_cursor_color = ఖాళీ;
స్థిర char_u	*desired_cursor_color = ఖాళీ;
స్థిర పూర్ణాంకం	చివరి_సమితి_cursor_shape = -1;
స్థిర పూర్ణాంకం	desired_cursor_shape = -1;
స్థిర పూర్ణాంకం	చివరి_సమితి_cursor_blink = -1;
స్థిర పూర్ణాంకం	desired_cursor_blink = -1;


///////////////////////////////////////
// 1. Generic code for all systems.

    స్థిర పూర్ణాంకం
cursor_color_equal(char_u *lhs_color, char_u *rhs_color)
{
    ఒకవేళ (lhs_color != ఖాళీ && rhs_color != ఖాళీ)
	తిరిగి STRCMP(lhs_color, rhs_color) == 0;
    తిరిగి lhs_color == ఖాళీ && rhs_color == ఖాళీ;
}

    స్థిర శూన్య
cursor_color_copy(char_u **గమ్యం_color, char_u *from_color)
{
    // Avoid a free & alloc if the value is already right.
    ఒకవేళ (cursor_color_equal(*గమ్యం_color, from_color))
	తిరిగి;
    vim_free(*గమ్యం_color);
    *గమ్యం_color = (from_color == ఖాళీ) ? ఖాళీ : vim_strsave(from_color);
}

    స్థిర char_u *
cursor_color_get(char_u *color)
{
    తిరిగి (color == ఖాళీ) ? (char_u *)"" : color;
}


/*
 * Parse 'termwinsize' and set "rows" and "cols" for the terminal size in the
 * current window.
 * Sets "rows" and/or "cols" to zero when it should follow the window size.
 * Return TRUE if the size is the minimum size: "24*80".
 */
    స్థిర పూర్ణాంకం
parse_termwinsize(win_T *wp, పూర్ణాంకం *అడ్డువరుసలు_2, పూర్ణాంకం *నిలువుల_సంఖ్య)
{
    పూర్ణాంకం	minsize = FALSE;

    *అడ్డువరుసలు_2 = 0;
    *నిలువుల_సంఖ్య = 0;

    ఒకవేళ (*wp->w_p_tws == NUL)
	తిరిగి FALSE;

    char_u *p = vim_strchr(wp->w_p_tws, 'x');

    // Syntax of value was already checked when it's set.
    ఒకవేళ (p == ఖాళీ)
    {
	minsize = TRUE;
	p = vim_strchr(wp->w_p_tws, '*');
    }
    *అడ్డువరుసలు_2 = atoi((అక్షరం *)wp->w_p_tws);
    *నిలువుల_సంఖ్య = atoi((అక్షరం *)p + 1);
    ఒకవేళ (*అడ్డువరుసలు_2 > VTERM_MAX_అడ్డువరుసలు)
	*అడ్డువరుసలు_2 = VTERM_MAX_అడ్డువరుసలు;
    ఒకవేళ (*నిలువుల_సంఖ్య > VTERM_MAX_నిలువుల_సంఖ్య)
	*నిలువుల_సంఖ్య = VTERM_MAX_నిలువుల_సంఖ్య;
    తిరిగి minsize;
}

/*
 * Determine the terminal size from 'termwinsize' and the current window.
 */
    స్థిర శూన్య
సమితి_పదం_and_win_కొలత(పదం_T *పదం, jobopt_T *opt)
{
    పూర్ణాంకం అడ్డువరుసలు_2, నిలువుల_సంఖ్య;
    పూర్ణాంకం	minsize;

#నిర్వచితమైతే FEAT_GUI
    ఒకవేళ (పదం->tl_system)
    {
	// Use the entire screen (excluding the tabpanel area) for the system
	// commands.  However, it will start at the command line and scroll up
	// as needed, using tl_toprow.
	పదం->tl_అడ్డువరుసలు = అడ్డువరుసలు;
	పదం->tl_నిలువుల_సంఖ్య = topframe->fr_వెడల్పు;
	తిరిగి;
    }
#అయితే_ముగింపు
    పదం->tl_అడ్డువరుసలు = curwin->w_ఎత్తు;
    పదం->tl_నిలువుల_సంఖ్య = curwin->w_వెడల్పు;

    minsize = parse_termwinsize(curwin, &అడ్డువరుసలు_2, &నిలువుల_సంఖ్య);
    ఒకవేళ (minsize)
    {
	ఒకవేళ (పదం->tl_అడ్డువరుసలు < అడ్డువరుసలు_2)
	    పదం->tl_అడ్డువరుసలు = అడ్డువరుసలు_2;
	ఒకవేళ (పదం->tl_నిలువుల_సంఖ్య < నిలువుల_సంఖ్య)
	    పదం->tl_నిలువుల_సంఖ్య = నిలువుల_సంఖ్య;
    }
    ఒకవేళ ((opt->jo_set2 & JO2_పదం_అడ్డువరుసలు))
	పదం->tl_అడ్డువరుసలు = opt->jo_పదం_అడ్డువరుసలు;
    లేకపోతే ఒకవేళ (అడ్డువరుసలు_2 != 0)
	పదం->tl_అడ్డువరుసలు = అడ్డువరుసలు_2;
    ఒకవేళ ((opt->jo_set2 & JO2_పదం_నిలువుల_సంఖ్య))
	పదం->tl_నిలువుల_సంఖ్య = opt->jo_పదం_నిలువుల_సంఖ్య;
    లేకపోతే ఒకవేళ (నిలువుల_సంఖ్య != 0)
	పదం->tl_నిలువుల_సంఖ్య = నిలువుల_సంఖ్య;

    ఒకవేళ (!opt->jo_hidden)
    {
	ఒకవేళ (పదం->tl_అడ్డువరుసలు != curwin->w_ఎత్తు)
	    win_setheight_win(పదం->tl_అడ్డువరుసలు, curwin);
	ఒకవేళ (పదం->tl_నిలువుల_సంఖ్య != curwin->w_వెడల్పు)
	    win_setwidth_win(పదం->tl_నిలువుల_సంఖ్య, curwin);

	// Set 'winsize' now to avoid a resize at the next redraw.
	ఒకవేళ (!minsize && *curwin->w_p_tws != NUL)
	{
	    char_u buf[100];

	    vim_snprintf((అక్షరం *)buf, 100, "%dx%d",
						 పదం->tl_అడ్డువరుసలు, పదం->tl_నిలువుల_సంఖ్య);
	    సమితి_option_విలువ_give_err((char_u *)"termwinsize",
							   0L, buf, OPT_LOCAL);
	}
    }
}

/*
 * Initialize job options for a terminal job.
 * Caller may overrule some of them.
 */
    శూన్య
init_పని_options(jobopt_T *opt)
{
    clear_పని_options(opt);

    opt->jo_బహుళకం = CH_బహుళకం_RAW;
    opt->jo_బహిర్గతం_బహుళకం = CH_బహుళకం_RAW;
    opt->jo_err_బహుళకం = CH_బహుళకం_RAW;
    opt->jo_సమితి = JO_బహుళకం | JO_బహిర్గతం_బహుళకం | JO_ERR_బహుళకం;
}

/*
 * Set job options mandatory for a terminal job.
 */
    స్థిర శూన్య
అమరిక_పని_options(jobopt_T *opt, పూర్ణాంకం అడ్డువరుసలు_2, పూర్ణాంకం నిలువుల_సంఖ్య)
{
#నిర్వచితం_కాకపోతే MSWIN
    // Win32: Redirecting the job output won't work, thus always connect stdout
    // here.
    ఒకవేళ (!(opt->jo_సమితి & JO_బహిర్గతం_IO))
#అయితే_ముగింపు
    {
	// Connect stdout to the terminal.
	opt->jo_io[PART_బహిర్గతం] = JIO_బఫరు;
	opt->jo_io_buf[PART_బహిర్గతం] = curbuf->b_fnum;
	opt->jo_modifiable[PART_బహిర్గతం] = 0;
	opt->jo_సమితి |= JO_బహిర్గతం_IO + JO_బహిర్గతం_BUF + JO_బహిర్గతం_MODIFIABLE;
    }

#నిర్వచితం_కాకపోతే MSWIN
    // Win32: Redirecting the job output won't work, thus always connect stderr
    // here.
    ఒకవేళ (!(opt->jo_సమితి & JO_ERR_IO))
#అయితే_ముగింపు
    {
	// Connect stderr to the terminal.
	opt->jo_io[PART_ERR] = JIO_బఫరు;
	opt->jo_io_buf[PART_ERR] = curbuf->b_fnum;
	opt->jo_modifiable[PART_ERR] = 0;
	opt->jo_సమితి |= JO_ERR_IO + JO_ERR_BUF + JO_ERR_MODIFIABLE;
    }

    opt->jo_pty = TRUE;
    ఒకవేళ ((opt->jo_set2 & JO2_పదం_అడ్డువరుసలు) == 0)
	opt->jo_పదం_అడ్డువరుసలు = అడ్డువరుసలు_2;
    ఒకవేళ ((opt->jo_set2 & JO2_పదం_నిలువుల_సంఖ్య) == 0)
	opt->jo_పదం_నిలువుల_సంఖ్య = నిలువుల_సంఖ్య;
}

/*
 * Flush messages on channels.
 */
    స్థిర శూన్య
పదం_flush_messages(శూన్య)
{
    mch_check_messages();
    parse_queued_messages();
}

/*
 * Close a terminal buffer (and its window).  Used when creating the terminal
 * fails.
 */
    స్థిర శూన్య
పదం_close_బఫరు(buf_T *buf, buf_T *old_curbuf)
{
    free_terminal(buf);
    ఒకవేళ (old_curbuf != ఖాళీ)
    {
	--curbuf->b_nwindows;
	curbuf = old_curbuf;
	curwin->w_బఫరు = curbuf;
	++curbuf->b_nwindows;
    }
    CHECK_CURBUF;

    // Wiping out the buffer will also close the window and call
    // free_terminal().
    do_బఫరు(DOBUF_WIPE, DOBUF_మొదటి, FORWARD, buf->b_fnum, TRUE);
}

/*
 * Start a terminal window and return its buffer.
 * Use either "argvar" or "argv", the other must be NULL.
 * When "flags" has TERM_START_NOJOB only create the buffer, b_term and open
 * the window.
 * Returns NULL when failed.
 */
    buf_T *
పదం_ప్రారంభం(
	typval_T    *argvar,
	అక్షరం	    **argv,
	jobopt_T    *opt,
	పూర్ణాంకం	    flags)
{
    exarg_T	విభజించు_ea;
    win_T	*old_curwin = curwin;
    పదం_T	*పదం;
    buf_T	*old_curbuf = ఖాళీ;
    పూర్ణాంకం		res;
    buf_T	*newbuf;
    పూర్ణాంకం		vertical = opt->jo_vertical || (cmdmod.cmod_విభజించు & WSP_VERT);
    jobopt_T	orig_opt;  // only partly filled
    pos_T	save_cursor;

    ఒకవేళ (check_restricted() || check_secure())
	తిరిగి ఖాళీ;
    ఒకవేళ (cmdwin_type != 0)
    {
	emsg(_(e_cannot_open_terminal_from_command_పంక్తి_కిటికీ));
	తిరిగి ఖాళీ;
    }

    ఒకవేళ ((opt->jo_సమితి & (JO_లోపల_IO + JO_బహిర్గతం_IO + JO_ERR_IO))
					 == (JO_లోపల_IO + JO_బహిర్గతం_IO + JO_ERR_IO)
	|| (!(opt->jo_సమితి & JO_బహిర్గతం_IO) && (opt->jo_సమితి & JO_బహిర్గతం_BUF))
	|| (!(opt->jo_సమితి & JO_ERR_IO) && (opt->jo_సమితి & JO_ERR_BUF))
	|| (argvar != ఖాళీ
	    && argvar->v_type == VAR_LIST
	    && argvar->vval.v_list != ఖాళీ
	    && argvar->vval.v_list->lv_మొదటి == &range_list_అంశం))
    {
	emsg(_(e_invalid_వాదం));
	తిరిగి ఖాళీ;
    }

    పదం = ALLOC_CLEAR_ONE(పదం_T);
    ఒకవేళ (పదం == ఖాళీ)
	తిరిగి ఖాళీ;
    పదం->tl_dirty_అడ్డువరుస_చివర = MAX_అడ్డువరుస;
    పదం->tl_cursor_visible = TRUE;
    పదం->tl_cursor_shape = VTERM_PROP_CURSORSHAPE_ఖండం;
    పదం->tl_ముగింపు = opt->jo_పదం_ముగింపు;
#నిర్వచితమైతే FEAT_GUI
    పదం->tl_system = (flags & పదం_ప్రారంభం_SYSTEM);
#అయితే_ముగింపు
    ga_init2(&పదం->tl_scrollback, పరిమాణం(sb_పంక్తి_T), 300);
    ga_init2(&పదం->tl_scrollback_postponed, పరిమాణం(sb_పంక్తి_T), 300);
    ga_init2(&పదం->tl_osc_buf, పరిమాణం(అక్షరం), 300);

    setpcmark();
    CLEAR_క్షేత్రం(విభజించు_ea);
    ఒకవేళ (opt->jo_curwin)
    {
	// Create a new buffer in the current window.
	ఒకవేళ (!can_abandon(curbuf, flags & పదం_ప్రారంభం_FORCEIT))
	{
	    no_write_సందేశం();
	    vim_free(పదం);
	    తిరిగి ఖాళీ;
	}
	ఒకవేళ (do_ecmd(0, ఖాళీ, ఖాళీ, &విభజించు_ea, ECMD_ONE,
		      (buf_hide(curwin->w_బఫరు) ? ECMD_HIDE : 0)
			  + ((flags & పదం_ప్రారంభం_FORCEIT) ? ECMD_FORCEIT : 0),
							       curwin) == FAIL)
	{
	    vim_free(పదం);
	    తిరిగి ఖాళీ;
	}
    }
    లేకపోతే ఒకవేళ (opt->jo_hidden || (flags & పదం_ప్రారంభం_SYSTEM))
    {
	buf_T *buf;

	// Create a new buffer without a window. Make it the current buffer for
	// a moment to be able to do the initializations.
	buf = buflist_new((char_u *)"", ఖాళీ, (linenr_T)0,
							 BLN_NEW | BLN_LISTED);
	ఒకవేళ (buf == ఖాళీ || ml_open(buf) == FAIL)
	{
	    vim_free(పదం);
	    తిరిగి ఖాళీ;
	}
	old_curbuf = curbuf;
	--curbuf->b_nwindows;
	curbuf = buf;
	save_cursor = curwin->w_cursor;
	curwin->w_బఫరు = buf;
	++curbuf->b_nwindows;
    }
    లేకపోతే
    {
	// Open a new window or tab.
	విభజించు_ea.cmdidx = CMD_new;
	విభజించు_ea.cmd = (char_u *)"new";
	విభజించు_ea.arg = (char_u *)"";
	ఒకవేళ (opt->jo_పదం_అడ్డువరుసలు > 0 && !vertical)
	{
	    విభజించు_ea.line2 = opt->jo_పదం_అడ్డువరుసలు;
	    విభజించు_ea.addr_లెక్క = 1;
	}
	ఒకవేళ (opt->jo_పదం_నిలువుల_సంఖ్య > 0 && vertical)
	{
	    విభజించు_ea.line2 = opt->jo_పదం_నిలువుల_సంఖ్య;
	    విభజించు_ea.addr_లెక్క = 1;
	}

	పూర్ణాంకం cmod_విభజించు_modified = FALSE;
	ఒకవేళ (vertical)
	{
	    ఒకవేళ (!(cmdmod.cmod_విభజించు & WSP_VERT))
		cmod_విభజించు_modified = TRUE;
	    cmdmod.cmod_విభజించు |= WSP_VERT;
	}
	ex_splitview(&విభజించు_ea);
	ఒకవేళ (cmod_విభజించు_modified)
	    cmdmod.cmod_విభజించు &= ~WSP_VERT;
	ఒకవేళ (curwin == old_curwin)
	{
	    // split failed
	    vim_free(పదం);
	    తిరిగి ఖాళీ;
	}
    }
    పదం->tl_బఫరు = curbuf;
    curbuf->b_పదం = పదం;

    ఒకవేళ (!opt->jo_hidden)
    {
	// Only one size was taken care of with :new, do the other one.  With
	// "curwin" both need to be done.
	ఒకవేళ (opt->jo_పదం_అడ్డువరుసలు > 0 && (opt->jo_curwin || vertical))
	    win_setheight(opt->jo_పదం_అడ్డువరుసలు);
	ఒకవేళ (opt->jo_పదం_నిలువుల_సంఖ్య > 0 && (opt->jo_curwin || !vertical))
	    win_setwidth(opt->jo_పదం_నిలువుల_సంఖ్య);
    }

    // Link the new terminal in the list of active terminals.
    పదం->tl_తదుపరి = మొదటి_పదం;
    మొదటి_పదం = పదం;

    apply_autocmds(EVENT_BUFFILEPRE, ఖాళీ, ఖాళీ, FALSE, curbuf);

    ఒకవేళ (opt->jo_పదం_name != ఖాళీ)
    {
	vim_free(curbuf->b_ffname);
	curbuf->b_ffname = vim_strsave(opt->jo_పదం_name);
    }
    లేకపోతే ఒకవేళ (argv != ఖాళీ)
    {
	vim_free(curbuf->b_ffname);
	curbuf->b_ffname = vim_strsave((char_u *)"!system");
    }
    లేకపోతే
    {
	పూర్ణాంకం	i;
	size_t	len;
	char_u	*cmd, *p;

	ఒకవేళ (argvar->v_type == VAR_STRING)
	{
	    cmd = argvar->vval.v_string;
	    ఒకవేళ (cmd == ఖాళీ)
		cmd = (char_u *)"";
	    లేకపోతే ఒకవేళ (STRCMP(cmd, "NONE") == 0)
		cmd = (char_u *)"pty";
	}
	లేకపోతే ఒకవేళ (argvar->v_type != VAR_LIST
		|| argvar->vval.v_list == ఖాళీ
		|| argvar->vval.v_list->lv_len == 0
		|| (cmd = tv_get_string_chk(
			       &argvar->vval.v_list->lv_మొదటి->li_tv)) == ఖాళీ)
	    cmd = (char_u*)"";

	len = STRLEN(cmd) + 10;
	p = alloc(len);

	కొరకు (i = 0; p != ఖాళీ; ++i)
	{
	    // Prepend a ! to the command name to avoid the buffer name equals
	    // the executable, otherwise ":w!" would overwrite it.
	    ఒకవేళ (i == 0)
		vim_snprintf((అక్షరం *)p, len, "!%s", cmd);
	    లేకపోతే
		vim_snprintf((అక్షరం *)p, len, "!%s (%d)", cmd, i);
	    ఒకవేళ (buflist_findname(p) == ఖాళీ)
	    {
		vim_free(curbuf->b_ffname);
		curbuf->b_ffname = p;
		ఆపు;
	    }
	}
    }
    vim_free(curbuf->b_sfname);
    curbuf->b_sfname = vim_strsave(curbuf->b_ffname);
    curbuf->b_fname = curbuf->b_ffname;

    apply_autocmds(EVENT_BUFFILEPOST, ఖాళీ, ఖాళీ, FALSE, curbuf);

    ఒకవేళ (opt->jo_పదం_opencmd != ఖాళీ)
	పదం->tl_opencmd = vim_strsave(opt->jo_పదం_opencmd);

    ఒకవేళ (opt->jo_eof_chars != ఖాళీ)
	పదం->tl_eof_chars = vim_strsave(opt->jo_eof_chars);

    సమితి_string_option_ప్రత్యక్ష((char_u *)"buftype", -1,
				  (char_u *)"terminal", OPT_FREE|OPT_LOCAL, 0);
    // Avoid that 'buftype' is reset when this buffer is entered.
    curbuf->b_p_initialized = true;

    // Mark the buffer as not modifiable. It can only be made modifiable after
    // the job finished.
    curbuf->b_p_ma = FALSE;

    సమితి_పదం_and_win_కొలత(పదం, opt);
#నిర్వచితమైతే MSWIN
    mch_memmove(orig_opt.jo_io, opt->jo_io, పరిమాణం(orig_opt.jo_io));
#అయితే_ముగింపు
    అమరిక_పని_options(opt, పదం->tl_అడ్డువరుసలు, పదం->tl_నిలువుల_సంఖ్య);

    ఒకవేళ (flags & పదం_ప్రారంభం_NOJOB)
	తిరిగి curbuf;

#ఒకవేళ defined(FEAT_SESSION)
    // Remember the command for the session file.
    ఒకవేళ (opt->jo_పదం_norestore || argv != ఖాళీ)
	పదం->tl_command = vim_strsave((char_u *)"NONE");
    లేకపోతే ఒకవేళ (argvar->v_type == VAR_STRING)
    {
	char_u	*cmd = argvar->vval.v_string;

	ఒకవేళ (cmd != ఖాళీ && STRCMP(cmd, p_sh) != 0)
	    పదం->tl_command = vim_strsave(cmd);
    }
    లేకపోతే ఒకవేళ (argvar->v_type == VAR_LIST
	    && argvar->vval.v_list != ఖాళీ
	    && argvar->vval.v_list->lv_len > 0)
    {
	garray_T	ga;
	listitem_T	*అంశం;

	ga_init2(&ga, 1, 100);
	FOR_ALL_LIST_వస్తువులు(argvar->vval.v_list, అంశం)
	{
	    char_u *s = tv_get_string_chk(&అంశం->li_tv);
	    char_u *p;

	    ఒకవేళ (s == ఖాళీ)
		ఆపు;
	    p = vim_strsave_fnameescape(s, VSE_NONE);
	    ఒకవేళ (p == ఖాళీ)
		ఆపు;
	    ga_concat(&ga, p);
	    vim_free(p);
	    ga_append(&ga, ' ');
	}
	ఒకవేళ (అంశం == ఖాళీ)
	{
	    ga_append(&ga, NUL);
	    పదం->tl_command = ga.ga_దత్తాంశం;
	}
	లేకపోతే
	    ga_clear(&ga);
    }
#అయితే_ముగింపు

    ఒకవేళ (opt->jo_పదం_kill != ఖాళీ)
    {
	char_u *p = skiptowhite(opt->jo_పదం_kill);

	పదం->tl_kill = vim_strnsave(opt->jo_పదం_kill, p - opt->jo_పదం_kill);
    }

    ఒకవేళ (opt->jo_పదం_api != ఖాళీ)
    {
	char_u *p = skiptowhite(opt->jo_పదం_api);

	పదం->tl_api = vim_strnsave(opt->jo_పదం_api, p - opt->jo_పదం_api);
    }
    లేకపోతే
	పదం->tl_api = vim_strsave((char_u *)"Tapi_");

    ఒకవేళ (opt->jo_set2 & JO2_పదం_HIGHLIGHT)
	పదం->tl_highlight_name = vim_strsave(opt->jo_పదం_highlight);

#ఒకవేళ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    // Save the user-defined palette, it is only used in GUI (or 'tgc' is on).
    ఒకవేళ (opt->jo_set2 & JO2_ANSI_COLORS)
    {
	పదం->tl_palette = ALLOC_MULT(long_u, 16);
	ఒకవేళ (పదం->tl_palette == ఖాళీ)
	{
	    vim_free(పదం);
	    తిరిగి ఖాళీ;
	}
	జ్ఞాపకం_నకలు(పదం->tl_palette, opt->jo_ansi_colors, పరిమాణం(long_u) * 16);
    }
#అయితే_ముగింపు

    // System dependent: setup the vterm and maybe start the job in it.
    ఒకవేళ (argv == ఖాళీ
	    && argvar->v_type == VAR_STRING
	    && argvar->vval.v_string != ఖాళీ
	    && STRCMP(argvar->vval.v_string, "NONE") == 0)
	res = create_pty_only(పదం, opt);
    లేకపోతే
	res = పదం_and_పని_init(పదం, argvar, argv, opt, &orig_opt);

    newbuf = curbuf;
    ఒకవేళ (res == OK)
    {
	// Get and remember the size we ended up with.  Update the pty.
	vterm_get_కొలత(పదం->tl_vterm, &పదం->tl_అడ్డువరుసలు, &పదం->tl_నిలువుల_సంఖ్య);
	పదం_report_winsize(పదం, పదం->tl_అడ్డువరుసలు, పదం->tl_నిలువుల_సంఖ్య);
#నిర్వచితమైతే FEAT_GUI
	ఒకవేళ (పదం->tl_system)
	{
	    // display first line below typed command
	    పదం->tl_toprow = సందేశం_అడ్డువరుస + 1;
	    పదం->tl_dirty_అడ్డువరుస_చివర = 0;
	}
#అయితే_ముగింపు

	// Make sure we don't get stuck on sending keys to the job, it leads to
	// a deadlock if the job is waiting for Vim to read.
	channel_సమితి_nonblock(పదం->tl_పని->jv_channel, PART_లోపల);

	ఒకవేళ (old_curbuf != ఖాళీ)
	{
	    --curbuf->b_nwindows;
	    curbuf = old_curbuf;
	    curwin->w_బఫరు = curbuf;
	    curwin->w_cursor = save_cursor;
	    ++curbuf->b_nwindows;
	}
	లేకపోతే ఒకవేళ (vgetc_busy
#నిర్వచితమైతే FEAT_TIMERS
		|| timer_busy
#అయితే_ముగింపు
		|| ఆదానం_busy)
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
    లేకపోతే
    {
	పదం_close_బఫరు(curbuf, old_curbuf);
	తిరిగి ఖాళీ;
    }

    apply_autocmds(EVENT_TERMINALOPEN, ఖాళీ, ఖాళీ, FALSE, newbuf);
    ఒకవేళ (!opt->jo_hidden && !(flags & పదం_ప్రారంభం_SYSTEM))
	apply_autocmds(EVENT_TERMINALWINOPEN, ఖాళీ, ఖాళీ, FALSE, newbuf);
    తిరిగి newbuf;
}

/*
 * ":terminal": open a terminal window and execute a job in it.
 */
    శూన్య
ex_terminal(exarg_T *eap)
{
    typval_T	argvar[2];
    jobopt_T	opt;
    పూర్ణాంకం		opt_shell = FALSE;
    char_u	*cmd;
    char_u	*tofree = ఖాళీ;
    పూర్ణాంకం		scroll_save = సందేశం_scroll;

    సందేశం_scroll = FALSE;		// don't scroll here
    autowrite_all();
    సందేశం_scroll = scroll_save;

    init_పని_options(&opt);

    cmd = eap->arg;
    అయితే (*cmd == '+' && *(cmd + 1) == '+')
    {
	char_u  *p, *ep;

	cmd += 2;
	p = skiptowhite(cmd);
	ep = vim_strchr(cmd, '=');
	ఒకవేళ (ep != ఖాళీ)
	{
	    ఒకవేళ (ep < p)
		p = ep;
	    లేకపోతే
		ep = ఖాళీ;
	}

	// Note: Keep this in sync with get_terminalopt_name.

#నిర్వచించు OPTARG_HAS(name) ((పూర్ణాంకం)(p - cmd) == పరిమాణం(name) - 1 \
				 && STRNICMP(cmd, name, పరిమాణం(name) - 1) == 0)
	ఒకవేళ (OPTARG_HAS("close"))
	    opt.jo_పదం_ముగింపు = 'c';
	లేకపోతే ఒకవేళ (OPTARG_HAS("noclose"))
	    opt.jo_పదం_ముగింపు = 'n';
	లేకపోతే ఒకవేళ (OPTARG_HAS("open"))
	    opt.jo_పదం_ముగింపు = 'o';
	లేకపోతే ఒకవేళ (OPTARG_HAS("curwin"))
	    opt.jo_curwin = 1;
	లేకపోతే ఒకవేళ (OPTARG_HAS("hidden"))
	    opt.jo_hidden = 1;
	లేకపోతే ఒకవేళ (OPTARG_HAS("norestore"))
	    opt.jo_పదం_norestore = 1;
	లేకపోతే ఒకవేళ (OPTARG_HAS("shell"))
	    opt_shell = TRUE;
	లేకపోతే ఒకవేళ (OPTARG_HAS("kill") && ep != ఖాళీ)
	{
	    opt.jo_set2 |= JO2_పదం_KILL;
	    opt.jo_పదం_kill = ep + 1;
	    p = skiptowhite(cmd);
	}
	లేకపోతే ఒకవేళ (OPTARG_HAS("api"))
	{
	    opt.jo_set2 |= JO2_పదం_API;
	    ఒకవేళ (ep != ఖాళీ)
	    {
		opt.jo_పదం_api = ep + 1;
		p = skiptowhite(cmd);
	    }
	    లేకపోతే
		opt.jo_పదం_api = ఖాళీ;
	}
	లేకపోతే ఒకవేళ (OPTARG_HAS("rows") && ep != ఖాళీ && సురక్షిత_isdigit(ep[1]))
	{
	    opt.jo_set2 |= JO2_పదం_అడ్డువరుసలు;
	    opt.jo_పదం_అడ్డువరుసలు = atoi((అక్షరం *)ep + 1);
	    p = skiptowhite(cmd);
	}
	లేకపోతే ఒకవేళ (OPTARG_HAS("cols") && ep != ఖాళీ && సురక్షిత_isdigit(ep[1]))
	{
	    opt.jo_set2 |= JO2_పదం_నిలువుల_సంఖ్య;
	    opt.jo_పదం_నిలువుల_సంఖ్య = atoi((అక్షరం *)ep + 1);
	    p = skiptowhite(cmd);
	}
	లేకపోతే ఒకవేళ (OPTARG_HAS("eof") && ep != ఖాళీ)
	{
	    char_u *buf = ఖాళీ;
	    char_u *కీలకాలు;

	    vim_free(opt.jo_eof_chars);
	    p = skiptowhite(cmd);
	    *p = NUL;
	    కీలకాలు = replace_termcodes(ep + 1, &buf, 0,
		    REPTERM_FROM_PART | REPTERM_DO_LT | REPTERM_SPECIAL, ఖాళీ);
	    opt.jo_set2 |= JO2_EOF_CHARS;
	    opt.jo_eof_chars = vim_strsave(కీలకాలు);
	    vim_free(buf);
	    *p = ' ';
	}
#నిర్వచితమైతే MSWIN
	లేకపోతే ఒకవేళ ((పూర్ణాంకం)(p - cmd) == 4 && STRNICMP(cmd, "type", 4) == 0
								 && ep != ఖాళీ)
	{
	    పూర్ణాంకం tty_type = NUL;

	    p = skiptowhite(cmd);
	    ఒకవేళ (STRNICMP(ep + 1, "winpty", p - (ep + 1)) == 0)
		tty_type = 'w';
	    లేకపోతే ఒకవేళ (STRNICMP(ep + 1, "conpty", p - (ep + 1)) == 0)
		tty_type = 'c';
	    లేకపోతే
	    {
		semsg(_(e_invalid_విలువ_for_వాదం_str), "type");
		వెళ్ళు theend;
	    }
	    opt.jo_set2 |= JO2_TTY_TYPE;
	    opt.jo_tty_type = tty_type;
	}
#అయితే_ముగింపు
	లేకపోతే
	{
	    ఒకవేళ (*p)
		*p = NUL;
	    semsg(_(e_invalid_attribute_str), cmd);
	    వెళ్ళు theend;
	}
#నిర్వచనం_రద్దు OPTARG_HAS
	cmd = skipwhite(p);
    }
    ఒకవేళ (*cmd == NUL)
    {
	// Make a copy of 'shell', an autocommand may change the option.
	tofree = cmd = vim_strsave(p_sh);

	// default to close when the shell exits
	ఒకవేళ (opt.jo_పదం_ముగింపు == NUL)
	    opt.jo_పదం_ముగింపు = TL_ముగింపు_CLOSE;
    }

    ఒకవేళ (eap->addr_లెక్క > 0)
    {
	// Write lines from current buffer to the job.
	opt.jo_సమితి |= JO_లోపల_IO | JO_లోపల_BUF | JO_లోపల_శిఖరం | JO_లోపల_BOT;
	opt.jo_io[PART_లోపల] = JIO_బఫరు;
	opt.jo_io_buf[PART_లోపల] = curbuf->b_fnum;
	opt.jo_లోపల_శిఖరం = eap->line1;
	opt.jo_లోపల_bot = eap->line2;
    }

    ఒకవేళ (opt_shell && tofree == ఖాళీ)
    {
#నిర్వచితమైతే UNIX
	అక్షరం	**argv = ఖాళీ;
	char_u	*tofree1 = ఖాళీ;
	char_u	*tofree2 = ఖాళీ;

	// :term ++shell command
	ఒకవేళ (unix_build_argv(cmd, &argv, &tofree1, &tofree2) == OK)
	    పదం_ప్రారంభం(ఖాళీ, argv, &opt, eap->forceit ? పదం_ప్రారంభం_FORCEIT : 0);
	vim_free(argv);
	vim_free(tofree1);
	vim_free(tofree2);
	వెళ్ళు theend;
#లేకపోతే
# ifdef MSWIN
	long_u	    cmdlen = STRLEN(p_sh) + STRLEN(p_shcf) + STRLEN(cmd) + 10;
	char_u	    *newcmd;

	newcmd = alloc(cmdlen);
	ఒకవేళ (newcmd == ఖాళీ)
	    వెళ్ళు theend;
	tofree = newcmd;
	vim_snprintf((అక్షరం *)newcmd, cmdlen, "%s %s %s", p_sh, p_shcf, cmd);
	cmd = newcmd;
# లేకపోతే
	emsg(_(e_sorry_plusplusshell_not_supported_on_this_system));
	వెళ్ళు theend;
# endif
#అయితే_ముగింపు
    }
    argvar[0].v_type = VAR_STRING;
    argvar[0].vval.v_string = cmd;
    argvar[1].v_type = VAR_UNKNOWN;
    పదం_ప్రారంభం(argvar, ఖాళీ, &opt, eap->forceit ? పదం_ప్రారంభం_FORCEIT : 0);

theend:
    vim_free(tofree);
    vim_free(opt.jo_eof_chars);
}

    స్థిర char_u *
get_terminalopt_name(expand_T *xp వాడని, పూర్ణాంకం idx)
{
    // Note: Keep this in sync with ex_terminal.
    స్థిర అక్షరం *(p_termopt_విలువలు[]) =
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

    ఒకవేళ (idx < (పూర్ణాంకం)ARRAY_నిడివి(p_termopt_విలువలు))
	తిరిగి (char_u*)p_termopt_విలువలు[idx];
    తిరిగి ఖాళీ;
}

    స్థిర char_u *
get_termkill_name(expand_T *xp వాడని, పూర్ణాంకం idx)
{
    // These are platform-specific values used for job_stop(). They are defined
    // in each platform's mch_signal_job(). Just use a unified auto-complete
    // list for simplicity.
    స్థిర అక్షరం *(p_termkill_విలువలు[]) =
    {
	"term",
	"hup",
	"quit",
	"int",
	"kill",
	"winch",
    };

    ఒకవేళ (idx < (పూర్ణాంకం)ARRAY_నిడివి(p_termkill_విలువలు))
	తిరిగి (char_u*)p_termkill_విలువలు[idx];
    తిరిగి ఖాళీ;
}

/*
 * Command-line expansion for :terminal [options]
 */
    పూర్ణాంకం
expand_terminal_opt(
	char_u	    *pat,
	expand_T    *xp,
	regmatch_T  *rmp,
	char_u	    ***matches,
	పూర్ణాంకం	    *అంకె_Matches)
{
    ఒకవేళ (xp->xp_నమూనా > xp->xp_పంక్తి && *(xp->xp_నమూనా-1) == '=')
    {
	char_u *(*cb)(expand_T *, పూర్ణాంకం) = ఖాళీ;

	char_u *name_చివర = xp->xp_నమూనా - 1;
	ఒకవేళ (name_చివర - xp->xp_పంక్తి >= 4
		&& STRNCMP(name_చివర - 4, "kill", 4) == 0)
	    cb = get_termkill_name;

	ఒకవేళ (cb != ఖాళీ)
	{
	    తిరిగి ExpandGeneric(
		    pat,
		    xp,
		    rmp,
		    matches,
		    అంకె_Matches,
		    cb,
		    FALSE);
	}
	తిరిగి FAIL;
    }
    తిరిగి ExpandGeneric(
	    pat,
	    xp,
	    rmp,
	    matches,
	    అంకె_Matches,
	    get_terminalopt_name,
	    FALSE);
}

#ఒకవేళ defined(FEAT_SESSION)
/*
 * Write a :terminal command to the session file to restore the terminal in
 * window "wp".
 * Return FAIL if writing fails.
 */
    పూర్ణాంకం
పదం_write_session(FILE *fd, win_T *wp, hashtab_T *terminal_bufs)
{
    స్థిరరాశి పూర్ణాంకం	bufnr = wp->w_బఫరు->b_fnum;
    పదం_T	*పదం = wp->w_బఫరు->b_పదం;

    ఒకవేళ (terminal_bufs != ఖాళీ && wp->w_బఫరు->b_nwindows > 1)
    {
	// There are multiple views into this terminal buffer. We don't want to
	// create the terminal multiple times. If it's the first time, create,
	// otherwise link to the first buffer.
	అక్షరం	    id_as_str[NUMBUFLEN];
	hashitem_T  *నమోదు_2;

	vim_snprintf(id_as_str, పరిమాణం(id_as_str), "%d", bufnr);

	నమోదు_2 = hash_కనుగొను(terminal_bufs, (char_u *)id_as_str);
	ఒకవేళ (!HASHITEM_ఖాళీలు(నమోదు_2))
	{
	    // we've already opened this terminal buffer
	    ఒకవేళ (fprintf(fd, "execute 'buffer ' .. term_buf_%d", bufnr) < 0)
		తిరిగి FAIL;
	    తిరిగి put_eol(fd);
	}
    }

    // Create the terminal and run the command.  This is not without
    // risk, but let's assume the user only creates a session when this
    // will be OK.
    ఒకవేళ (fprintf(fd, "exe ':terminal ++curwin"
		" ++cols=' .. ((&columns * %d + %ld) / %ld)"
		" .. ' ++rows=' .. ((&lines * %d + %ld) / %ld) ",
		పదం->tl_నిలువుల_సంఖ్య, Columns / 2, Columns,
		పదం->tl_అడ్డువరుసలు, అడ్డువరుసలు / 2, అడ్డువరుసలు) < 0)
	తిరిగి FAIL;
# ifdef MSWIN
    ఒకవేళ (fprintf(fd, ".. ' ++type=%s' ", పదం->tl_పని->jv_tty_type) < 0)
	తిరిగి FAIL;
# endif
    ఒకవేళ (పదం->tl_command != ఖాళీ)
    {
	char_u *quoted_command = string_quote(పదం->tl_command, FALSE);
	ఒకవేళ (quoted_command == ఖాళీ)
	    తిరిగి FAIL;

	పూర్ణాంకం ret = fputs(".. ' ' .. ", fd) < 0
		    || fputs((అక్షరం *)quoted_command, fd) < 0;
	vim_free(quoted_command);
	ఒకవేళ (ret)
	    తిరిగి FAIL;
    }
    ఒకవేళ (put_eol(fd) != OK)
	తిరిగి FAIL;

    ఒకవేళ (fprintf(fd, "var term_buf_%d: number = bufnr()", bufnr) < 0)
	తిరిగి FAIL;

    ఒకవేళ (terminal_bufs != ఖాళీ && wp->w_బఫరు->b_nwindows > 1)
    {
	అక్షరం *hash_కీలకం = alloc(NUMBUFLEN);

	vim_snprintf(hash_కీలకం, NUMBUFLEN, "%d", bufnr);
	hash_కూడు(terminal_bufs, (char_u *)hash_కీలకం, "terminal session");
    }

    తిరిగి put_eol(fd);
}

/*
 * Return TRUE if "buf" has a terminal that should be restored.
 */
    పూర్ణాంకం
పదం_should_restore(buf_T *buf)
{
    పదం_T	*పదం = buf->b_పదం;

    తిరిగి పదం != ఖాళీ && (పదం->tl_command == ఖాళీ
				     || STRCMP(పదం->tl_command, "NONE") != 0);
}
#అయితే_ముగింపు

/*
 * Free the scrollback buffer for "term".
 */
    స్థిర శూన్య
free_scrollback(పదం_T *పదం)
{
    పూర్ణాంకం i;

    కొరకు (i = 0; i < పదం->tl_scrollback.ga_len; ++i)
	vim_free(((sb_పంక్తి_T *)పదం->tl_scrollback.ga_దత్తాంశం + i)->sb_cells);
    ga_clear(&పదం->tl_scrollback);
    కొరకు (i = 0; i < పదం->tl_scrollback_postponed.ga_len; ++i)
	vim_free(((sb_పంక్తి_T *)పదం->tl_scrollback_postponed.ga_దత్తాంశం + i)->sb_cells);
    ga_clear(&పదం->tl_scrollback_postponed);
}


// Terminals that need to be freed soon.
స్థిర పదం_T	*terminals_గమ్యం_free = ఖాళీ;

/*
 * Free a terminal and everything it refers to.
 * Kills the job if there is one.
 * Called when wiping out a buffer.
 * The actual terminal structure is freed later in free_unused_terminals(),
 * because callbacks may wipe out a buffer while the terminal is still
 * referenced.
 */
    శూన్య
free_terminal(buf_T *buf)
{
    పదం_T	*పదం = buf->b_పదం;
    పదం_T	*tp;

    ఒకవేళ (పదం == ఖాళీ)
	తిరిగి;

    // Unlink the terminal form the list of terminals.
    ఒకవేళ (మొదటి_పదం == పదం)
	మొదటి_పదం = పదం->tl_తదుపరి;
    లేకపోతే
	కొరకు (tp = మొదటి_పదం; tp->tl_తదుపరి != ఖాళీ; tp = tp->tl_తదుపరి)
	    ఒకవేళ (tp->tl_తదుపరి == పదం)
	    {
		tp->tl_తదుపరి = పదం->tl_తదుపరి;
		ఆపు;
	    }

    ఒకవేళ (పదం->tl_పని != ఖాళీ)
    {
	ఒకవేళ (పదం->tl_పని->jv_స్థితి != పని_ENDED
		&& పదం->tl_పని->jv_స్థితి != పని_ముగిసిన
		&& పదం->tl_పని->jv_స్థితి != పని_FAILED)
	    పని_stop(పదం->tl_పని, ఖాళీ, "kill");
	పని_unref(పదం->tl_పని);
    }
    పదం->tl_తదుపరి = terminals_గమ్యం_free;
    terminals_గమ్యం_free = పదం;

    buf->b_పదం = ఖాళీ;
    ఒకవేళ (లోపల_terminal_loop == పదం)
	లోపల_terminal_loop = ఖాళీ;
}

    శూన్య
free_వాడని_terminals(శూన్య)
{
    అయితే (terminals_గమ్యం_free != ఖాళీ)
    {
	పదం_T	    *పదం = terminals_గమ్యం_free;

	terminals_గమ్యం_free = పదం->tl_తదుపరి;

	free_scrollback(పదం);
	ga_clear(&పదం->tl_osc_buf);

	పదం_free_vterm(పదం);
	vim_free(పదం->tl_api);
	vim_free(పదం->tl_title);
#నిర్వచితమైతే FEAT_SESSION
	vim_free(పదం->tl_command);
#అయితే_ముగింపు
	vim_free(పదం->tl_kill);
	vim_free(పదం->tl_స్థితి_పాఠ్యం);
	vim_free(పదం->tl_opencmd);
	vim_free(పదం->tl_eof_chars);
	vim_free(పదం->tl_arg0_cmd);
#నిర్వచితమైతే MSWIN
	ఒకవేళ (పదం->tl_బహిర్గతం_fd != ఖాళీ)
	    fclose(పదం->tl_బహిర్గతం_fd);
#అయితే_ముగింపు
	vim_free(పదం->tl_highlight_name);
	vim_free(పదం->tl_cursor_color);
	vim_free(పదం->tl_palette);
	vim_free(పదం);
    }
}

/*
 * Get the part that is connected to the tty. Normally this is PART_IN, but
 * when writing buffer lines to the job it can be another.  This makes it
 * possible to do "1,5term vim -".
 */
    స్థిర ch_part_T
get_tty_part(పదం_T *పదం వాడని)
{
#నిర్వచితమైతే UNIX
    ch_part_T	parts[3] = {PART_లోపల, PART_బహిర్గతం, PART_ERR};
    పూర్ణాంకం		i;

    కొరకు (i = 0; i < 3; ++i)
    {
	పూర్ణాంకం fd = పదం->tl_పని->jv_channel->ch_part[parts[i]].ch_fd;

	ఒకవేళ (mch_isatty(fd))
	    తిరిగి parts[i];
    }
#అయితే_ముగింపు
    తిరిగి PART_లోపల;
}

/*
 * Read any vterm output and send it on the channel.
 */
    స్థిర శూన్య
పదం_forward_output(పదం_T *పదం)
{
    VTerm *vterm = పదం->tl_vterm;
    అక్షరం   buf[కీలకం_BUF_LEN];
    size_t curlen = vterm_output_read(vterm, buf, కీలకం_BUF_LEN);

    ఒకవేళ (curlen > 0)
	channel_send(పదం->tl_పని->jv_channel, get_tty_part(పదం),
					     (char_u *)buf, (పూర్ణాంకం)curlen, ఖాళీ);
}

/*
 * Write job output "msg[len]" to the vterm.
 */
    స్థిర శూన్య
పదం_write_పని_output(పదం_T *పదం, char_u *సందేశం_arg, size_t len_arg)
{
    char_u	*సందేశం = సందేశం_arg;
    size_t	len = len_arg;
    VTerm	*vterm = పదం->tl_vterm;
    size_t	prevlen = vterm_output_get_బఫరు_current(vterm);
    size_t	హద్దు = పదం->tl_బఫరు->b_p_twsl * పదం->tl_నిలువుల_సంఖ్య * 3;

    // Limit the length to 'termwinscroll' * cols * 3 bytes.  Keep the text at
    // the end.
    ఒకవేళ (len > హద్దు)
    {
	char_u *p = సందేశం + len - హద్దు;

	p -= (*mb_తల_off)(సందేశం, p);
	len -= p - సందేశం;
	సందేశం = p;
    }

    vterm_ఆదానం_write(vterm, (అక్షరం *)సందేశం, len);

    // flush vterm buffer when vterm responded to control sequence
    ఒకవేళ (prevlen != vterm_output_get_బఫరు_current(vterm))
	పదం_forward_output(పదం);

    // this invokes the damage callbacks
    vterm_screen_flush_damage(vterm_obtain_screen(vterm));
}

    స్థిర శూన్య
position_cursor(win_T *wp, VTermPos *pos)
{
    wp->w_wrow = MIN(pos->అడ్డువరుస, MAX(0, wp->w_ఎత్తు - 1));
    wp->w_wcol = MIN(pos->నిలువు, MAX(0, wp->w_వెడల్పు - 1));
#నిర్వచితమైతే FEAT_PROP_POPUP
    ఒకవేళ (popup_is_popup(wp))
    {
	wp->w_wrow += popup_శిఖరం_extra(wp);
	wp->w_wcol += popup_ఎడమ_extra(wp);
	wp->w_flags |= WFLAG_WCOL_OFF_ADDED | WFLAG_WROW_OFF_ADDED;
    }
    లేకపోతే
	wp->w_flags &= ~(WFLAG_WCOL_OFF_ADDED | WFLAG_WROW_OFF_ADDED);
#అయితే_ముగింపు
    wp->w_valid |= (VALID_WCOL|VALID_WROW);
}

    స్థిర శూన్య
update_cursor(పదం_T *పదం, పూర్ణాంకం redraw)
{
    ఒకవేళ (పదం->tl_సాధారణ_బహుళకం)
	తిరిగి;
#నిర్వచితమైతే FEAT_GUI
    ఒకవేళ (పదం->tl_system)
	windgoto(పదం->tl_cursor_pos.అడ్డువరుస + పదం->tl_toprow,
		firstwin->w_wincol + పదం->tl_cursor_pos.నిలువు);
    లేకపోతే
#అయితే_ముగింపు
    ఒకవేళ (!పదం_పని_running(పదం))
	// avoid the cursor positioned below the last used line
	setcursor();
    లేకపోతే
    {
	// do not use the window cursor position
	position_cursor(curwin, &curbuf->b_పదం->tl_cursor_pos);
	windgoto(W_WINROW(curwin) + curwin->w_wrow,
		 curwin->w_wincol + curwin->w_wcol);
    }
    ఒకవేళ (redraw)
    {
	aco_save_T	aco;

	ఒకవేళ (పదం->tl_బఫరు == curbuf && పదం->tl_cursor_visible)
	    cursor_on();
	బహిర్గతం_flush();
#నిర్వచితమైతే FEAT_GUI
	ఒకవేళ (gui.లోపల_use)
	{
	    gui_update_cursor(FALSE, FALSE);
	    gui_mch_flush();
	}
#అయితే_ముగింపు
	// Make sure an invoked autocmd doesn't delete the buffer (and the
	// terminal) under our fingers.
	++పదం->tl_బఫరు->b_locked;

	// save and restore curwin and curbuf, in case the autocmd changes them
	aucmd_prepbuf(&aco, curbuf);
	apply_autocmds(EVENT_TEXTCHANGEDT, ఖాళీ, ఖాళీ, FALSE, పదం->tl_బఫరు);
	aucmd_restbuf(&aco);

	--పదం->tl_బఫరు->b_locked;
    }
}

/*
 * Find the location of a scrollbackline in the buffer
 */
    స్థిర శూన్య
scrollbackline_pos_లోపల_buf(పదం_T *పదం, పూర్ణాంకం అడ్డువరుస, linenr_T *lnum, పూర్ణాంకం *ప్రారంభం_నిలువు, size_t *ప్రారంభం_pos)
{
    sb_పంక్తి_T	*పంక్తులు = (sb_పంక్తి_T *)పదం->tl_scrollback.ga_దత్తాంశం;
    linenr_T    calc_lnum = పదం->tl_బఫరు_scrolled;
    size_t      calc_pos = 0;
    పూర్ణాంకం         calc_నిలువు = 0;
    పూర్ణాంకం         i;

    ఒకవేళ (అడ్డువరుస < 0 || అడ్డువరుస >= పదం->tl_scrollback.ga_len)
	తిరిగి;

    ఒకవేళ (అడ్డువరుస > పదం->tl_scrollback_scrolled)
    {
	// Lookback how far along in the top line we are
	కొరకు (i = పదం->tl_scrollback_scrolled + 1; i > 0 && పంక్తులు[i].continuation; --i)
	{
	    calc_pos += పంక్తులు[i - 1].sb_బైట్లు;
	    calc_నిలువు += పంక్తులు[i - 1].sb_నిలువుల_సంఖ్య;
	}
	i = పదం->tl_scrollback_scrolled + 1;
	calc_lnum = పదం->tl_బఫరు_scrolled + 1;
	// Do not count this line's bytes/cols twice
	ఒకవేళ (పంక్తులు[i].continuation)
	    ++i;
    }
    లేకపోతే
    {
	i = 1;
	calc_lnum = 1;
    }

    కొరకు (; i <= అడ్డువరుస; ++i)
    {
	ఒకవేళ (!పంక్తులు[i].continuation)
	{
	    ++calc_lnum;
	    calc_pos = 0;
	    calc_నిలువు = 0;
	}
	లేకపోతే
	{
	    calc_pos += పంక్తులు[i - 1].sb_బైట్లు;
	    calc_నిలువు += పంక్తులు[i - 1].sb_నిలువుల_సంఖ్య;
	}
    }

    *lnum = calc_lnum;
    ఒకవేళ (ప్రారంభం_నిలువు)
	*ప్రారంభం_నిలువు = calc_నిలువు;
    ఒకవేళ (ప్రారంభం_pos)
	*ప్రారంభం_pos = calc_pos;
}

/*
 * Find the location of a buffer line in the scrollback
 */
    స్థిర శూన్య
bufline_pos_లోపల_scrollback(పదం_T *పదం, linenr_T lnum, పూర్ణాంకం నిలువు, పూర్ణాంకం *అడ్డువరుస, పూర్ణాంకం *wrapped_నిలువు)
{
    buf_T	*buf = పదం->tl_బఫరు;
    sb_పంక్తి_T	*పంక్తులు = (sb_పంక్తి_T *)పదం->tl_scrollback.ga_దత్తాంశం;
    linenr_T    calc_అడ్డువరుస = పదం->tl_scrollback_scrolled;
    పూర్ణాంకం         calc_నిలువు = నిలువు;
    linenr_T    l;

    ఒకవేళ (lnum > buf->b_ml.ml_పంక్తి_లెక్క)
	తిరిగి;

    ఒకవేళ (lnum > పదం->tl_బఫరు_scrolled)
    {
	calc_అడ్డువరుస = పదం->tl_scrollback_scrolled;
	l = పదం->tl_బఫరు_scrolled + 1;

	అయితే (calc_అడ్డువరుస < పదం->tl_scrollback.ga_len && పంక్తులు[calc_అడ్డువరుస].continuation)
	    ++calc_అడ్డువరుస;
    }
    లేకపోతే
    {
	calc_అడ్డువరుస = 0;
	l = 1;
    }

    అయితే (calc_అడ్డువరుస < పదం->tl_scrollback.ga_len && l < lnum)
    {
	++calc_అడ్డువరుస;
	ఒకవేళ (!పంక్తులు[calc_అడ్డువరుస].continuation)
	    ++l;
    }

    అయితే (calc_అడ్డువరుస + 1 < పదం->tl_scrollback.ga_len && పంక్తులు[calc_అడ్డువరుస + 1].continuation
	    && calc_నిలువు >= పంక్తులు[calc_అడ్డువరుస].sb_నిలువుల_సంఖ్య)
    {
	calc_నిలువు -= పంక్తులు[calc_అడ్డువరుస].sb_నిలువుల_సంఖ్య;
	++calc_అడ్డువరుస;
    }

    ఒకవేళ (అడ్డువరుస)
	*అడ్డువరుస = calc_అడ్డువరుస;
    ఒకవేళ (wrapped_నిలువు)
	*wrapped_నిలువు = calc_నిలువు;
}

/*
 * Invoked when "msg" output from a job was received.  Write it to the terminal
 * of "buffer".
 */
    శూన్య
write_గమ్యం_పదం(buf_T *బఫరు, char_u *సందేశం, channel_T *channel)
{
    size_t	len = STRLEN(సందేశం);
    పదం_T	*పదం = బఫరు->b_పదం;

#నిర్వచితమైతే MSWIN
    // Win32: Cannot redirect output of the job, intercept it here and write to
    // the file.
    ఒకవేళ (పదం->tl_బహిర్గతం_fd != ఖాళీ)
    {
	ch_log(channel, "Writing %d bytes to output file", (పూర్ణాంకం)len);
	fwrite(సందేశం, len, 1, పదం->tl_బహిర్గతం_fd);
	తిరిగి;
    }
#అయితే_ముగింపు

    ఒకవేళ (పదం->tl_vterm == ఖాళీ)
    {
	ch_log(channel, "NOT writing %d bytes to terminal", (పూర్ణాంకం)len);
	తిరిగి;
    }
    ch_log(channel, "writing %d bytes to terminal", (పూర్ణాంకం)len);
    cursor_off();
    పదం_write_పని_output(పదం, సందేశం, len);

#నిర్వచితమైతే FEAT_GUI
    ఒకవేళ (పదం->tl_system)
    {
	// show system output, scrolling up the screen as needed
	update_system_పదం(పదం);
	update_cursor(పదం, TRUE);
    }
    లేకపోతే
#అయితే_ముగింపు
    // In Terminal-Normal mode we are displaying the buffer, not the terminal
    // contents, thus no screen update is needed.
    ఒకవేళ (!పదం->tl_సాధారణ_బహుళకం)
    {
	// Don't use update_screen() when editing the command line, it gets
	// cleared.
	// TODO: only update once in a while.
	ch_log(పదం->tl_పని->jv_channel, "updating screen");
	ఒకవేళ (బఫరు == curbuf && (State & బహుళకం_CMDLINE) == 0)
	{
	    update_screen(UPD_VALID_NO_UPDATE);
#ఒకవేళ defined(FEAT_TABPANEL)
	    ఒకవేళ (redraw_tabpanel)
		draw_tabpanel();
#అయితే_ముగింపు
	    // update_screen() can be slow, check the terminal wasn't closed
	    // already
	    ఒకవేళ (బఫరు == curbuf && curbuf->b_పదం != ఖాళీ)
		update_cursor(curbuf->b_పదం, TRUE);
	}
	లేకపోతే
	    redraw_after_callback(TRUE, FALSE);
    }
}

/*
 * Send a mouse position and click to the vterm
 */
    స్థిర పూర్ణాంకం
పదం_send_mouse(VTerm *vterm, పూర్ణాంకం button, పూర్ణాంకం pressed)
{
    VTermModifier   mod = VTERM_MOD_NONE;
    పూర్ణాంకం		    అడ్డువరుస = mouse_అడ్డువరుస - W_WINROW(curwin);
    పూర్ణాంకం		    నిలువు = mouse_నిలువు - curwin->w_wincol;

#నిర్వచితమైతే FEAT_PROP_POPUP
    ఒకవేళ (popup_is_popup(curwin))
    {
	అడ్డువరుస -= popup_శిఖరం_extra(curwin);
	నిలువు -= popup_ఎడమ_extra(curwin);
    }
#అయితే_ముగింపు
    vterm_mouse_move(vterm, అడ్డువరుస, నిలువు, mod);
    ఒకవేళ (button != 0)
	vterm_mouse_button(vterm, button, pressed, mod);
    తిరిగి TRUE;
}

స్థిర పూర్ణాంకం enter_mouse_నిలువు = -1;
స్థిర పూర్ణాంకం enter_mouse_అడ్డువరుస = -1;

/*
 * Handle a mouse click, drag or release.
 * Return TRUE when a mouse event is sent to the terminal.
 */
    స్థిర పూర్ణాంకం
పదం_mouse_click(VTerm *vterm, పూర్ణాంకం కీలకం)
{
#ఒకవేళ defined(FEAT_CLIPBOARD)
    // For modeless selection mouse drag and release events are ignored, unless
    // they are preceded with a mouse down event
    స్థిర పూర్ణాంకం	    ignore_drag_release = TRUE;
    VTermState	    *state;
    VTermMouseState mouse_state;


    state = vterm_obtain_state(vterm);
    ఒకవేళ (state == ఖాళీ)
	తిరిగి FALSE;

    vterm_state_get_mousestate(state, &mouse_state);
    ఒకవేళ (mouse_state.flags == 0)
    {
	// Terminal is not using the mouse, use modeless selection.
	ఎంపిక (కీలకం)
	{
	స్థితి K_LEFTDRAG:
	స్థితి K_LEFTRELEASE:
	స్థితి K_RIGHTDRAG:
	స్థితి K_RIGHTRELEASE:
		// Ignore drag and release events when the button-down wasn't
		// seen before.
		ఒకవేళ (ignore_drag_release)
		{
		    పూర్ణాంకం save_mouse_నిలువు, save_mouse_అడ్డువరుస;

		    ఒకవేళ (enter_mouse_నిలువు < 0)
			ఆపు;

		    // mouse click in the window gave us focus, handle that
		    // click now
		    save_mouse_నిలువు = mouse_నిలువు;
		    save_mouse_అడ్డువరుస = mouse_అడ్డువరుస;
		    mouse_నిలువు = enter_mouse_నిలువు;
		    mouse_అడ్డువరుస = enter_mouse_అడ్డువరుస;
		    clip_modeless(MOUSE_ఎడమ, TRUE, FALSE);
		    mouse_నిలువు = save_mouse_నిలువు;
		    mouse_అడ్డువరుస = save_mouse_అడ్డువరుస;
		}
		// FALLTHROUGH
	స్థితి K_LEFTMOUSE:
	స్థితి K_RIGHTMOUSE:
		ఒకవేళ (కీలకం == K_LEFTRELEASE || కీలకం == K_RIGHTRELEASE)
		    ignore_drag_release = TRUE;
		లేకపోతే
		    ignore_drag_release = FALSE;
		// Should we call mouse_has() here?
		ఒకవేళ (clip_నక్షత్రం.అందుబాటు)
		{
		    పూర్ణాంకం	    button, is_click, is_drag;

		    button = get_mouse_button(KEY2TERMCAP1(కీలకం),
							 &is_click, &is_drag);
		    ఒకవేళ (mouse_model_popup() && button == MOUSE_ఎడమ
					       && (mod_ముసుగు & MOD_ముసుగు_జరుపు))
		    {
			// Translate shift-left to right button.
			button = MOUSE_కుడి;
			mod_ముసుగు &= ~MOD_ముసుగు_జరుపు;
		    }
		    clip_modeless(button, is_click, is_drag);
		}
		ఆపు;

	స్థితి K_MIDDLEMOUSE:
		ఒకవేళ (clip_నక్షత్రం.అందుబాటు)
		    చొప్పించు_reg('*', TRUE);
		ఆపు;
	}
	enter_mouse_నిలువు = -1;
	తిరిగి FALSE;
    }
#అయితే_ముగింపు
    enter_mouse_నిలువు = -1;

    ఎంపిక (కీలకం)
    {
	స్థితి K_LEFTMOUSE:
	స్థితి K_LEFTMOUSE_NM:	పదం_send_mouse(vterm, 1, 1); ఆపు;
	స్థితి K_LEFTDRAG:	పదం_send_mouse(vterm, 1, 1); ఆపు;
	స్థితి K_LEFTRELEASE:
	స్థితి K_LEFTRELEASE_NM:	పదం_send_mouse(vterm, 1, 0); ఆపు;
	స్థితి K_MOUSEMOVE:	పదం_send_mouse(vterm, 0, 0); ఆపు;
	స్థితి K_MIDDLEMOUSE:	పదం_send_mouse(vterm, 2, 1); ఆపు;
	స్థితి K_MIDDLEDRAG:	పదం_send_mouse(vterm, 2, 1); ఆపు;
	స్థితి K_MIDDLERELEASE:	పదం_send_mouse(vterm, 2, 0); ఆపు;
	స్థితి K_RIGHTMOUSE:	పదం_send_mouse(vterm, 3, 1); ఆపు;
	స్థితి K_RIGHTDRAG:	పదం_send_mouse(vterm, 3, 1); ఆపు;
	స్థితి K_RIGHTRELEASE:	పదం_send_mouse(vterm, 3, 0); ఆపు;
    }
    తిరిగి TRUE;
}

/*
 * Convert typed key "c" with modifiers "modmask" into bytes to send to the
 * job.
 * Return the number of bytes in "buf".
 */
    స్థిర పూర్ణాంకం
పదం_convert_కీలకం(పదం_T *పదం, పూర్ణాంకం c, పూర్ణాంకం modmask, అక్షరం *buf)
{
    VTerm	    *vterm = పదం->tl_vterm;
    VTerm_కీలకం	    కీలకం = VTERM_కీలకం_NONE;
    VTermModifier   mod = VTERM_MOD_NONE;
    పూర్ణాంకం		    other = FALSE;

    ఎంపిక (c)
    {
	// don't use VTERM_KEY_ENTER, it may do an unwanted conversion

	స్థితి K_BS:
#నిర్వచితమైతే MSWIN
	    // In ConPTY, we must use VTERM_KEY_BACKSPACE, otherwise it will
	    // delete one word, equivalent to Alt+Backspace.
	    ఒకవేళ (పదం->tl_conpty)
		కీలకం = VTERM_కీలకం_BACKSPACE;
	    లేకపోతే
#అయితే_ముగింపు
		// don't use VTERM_KEY_BACKSPACE, it always becomes 0x7f DEL
		c = పదం_backspace_char;
	    ఆపు;

#నిర్వచితమైతే MSWIN
	స్థితి BS:
	    // In ConPTY, we must use VTERM_KEY_BACKSPACE, otherwise it will
	    // delete one word, equivalent to Alt+Backspace.
	    ఒకవేళ (పదం->tl_conpty)
		కీలకం = VTERM_కీలకం_BACKSPACE;
	    ఆపు;
#అయితే_ముగింపు

	స్థితి ESC:		కీలకం = VTERM_కీలకం_ESCAPE; ఆపు;
	స్థితి K_DEL:		కీలకం = VTERM_కీలకం_DEL; ఆపు;
	స్థితి K_కిందికి:		కీలకం = VTERM_కీలకం_కిందికి; ఆపు;
	స్థితి K_S_కిందికి:		mod = VTERM_MOD_జరుపు;
				కీలకం = VTERM_కీలకం_కిందికి; ఆపు;
	స్థితి K_చివర:		కీలకం = VTERM_కీలకం_చివర; ఆపు;
	స్థితి K_S_చివర:		mod = VTERM_MOD_జరుపు;
				కీలకం = VTERM_కీలకం_చివర; ఆపు;
	స్థితి K_C_చివర:		mod = VTERM_MOD_CTRL;
				కీలకం = VTERM_కీలకం_చివర; ఆపు;
	స్థితి K_F10:		కీలకం = VTERM_కీలకం_FUNCTION(10); ఆపు;
	స్థితి K_F11:		కీలకం = VTERM_కీలకం_FUNCTION(11); ఆపు;
	స్థితి K_F12:		కీలకం = VTERM_కీలకం_FUNCTION(12); ఆపు;
	స్థితి K_F1:		కీలకం = VTERM_కీలకం_FUNCTION(1); ఆపు;
	స్థితి K_F2:		కీలకం = VTERM_కీలకం_FUNCTION(2); ఆపు;
	స్థితి K_F3:		కీలకం = VTERM_కీలకం_FUNCTION(3); ఆపు;
	స్థితి K_F4:		కీలకం = VTERM_కీలకం_FUNCTION(4); ఆపు;
	స్థితి K_F5:		కీలకం = VTERM_కీలకం_FUNCTION(5); ఆపు;
	స్థితి K_F6:		కీలకం = VTERM_కీలకం_FUNCTION(6); ఆపు;
	స్థితి K_F7:		కీలకం = VTERM_కీలకం_FUNCTION(7); ఆపు;
	స్థితి K_F8:		కీలకం = VTERM_కీలకం_FUNCTION(8); ఆపు;
	స్థితి K_F9:		కీలకం = VTERM_కీలకం_FUNCTION(9); ఆపు;
	స్థితి K_HOME:		కీలకం = VTERM_కీలకం_HOME; ఆపు;
	స్థితి K_S_HOME:		mod = VTERM_MOD_జరుపు;
				కీలకం = VTERM_కీలకం_HOME; ఆపు;
	స్థితి K_C_HOME:		mod = VTERM_MOD_CTRL;
				కీలకం = VTERM_కీలకం_HOME; ఆపు;
	స్థితి K_INS:		కీలకం = VTERM_కీలకం_INS; ఆపు;
	స్థితి K_K0:		కీలకం = VTERM_కీలకం_KP_0; ఆపు;
	స్థితి K_K1:		కీలకం = VTERM_కీలకం_KP_1; ఆపు;
	స్థితి K_K2:		కీలకం = VTERM_కీలకం_KP_2; ఆపు;
	స్థితి K_K3:		కీలకం = VTERM_కీలకం_KP_3; ఆపు;
	స్థితి K_K4:		కీలకం = VTERM_కీలకం_KP_4; ఆపు;
	స్థితి K_K5:		కీలకం = VTERM_కీలకం_KP_5; ఆపు;
	స్థితి K_K6:		కీలకం = VTERM_కీలకం_KP_6; ఆపు;
	స్థితి K_K7:		కీలకం = VTERM_కీలకం_KP_7; ఆపు;
	స్థితి K_K8:		కీలకం = VTERM_కీలకం_KP_8; ఆపు;
	స్థితి K_K9:		కీలకం = VTERM_కీలకం_KP_9; ఆపు;
	స్థితి K_KDEL:		కీలకం = VTERM_కీలకం_DEL; ఆపు; // TODO
	స్థితి K_KDIVIDE:		కీలకం = VTERM_కీలకం_KP_DIVIDE; ఆపు;
	స్థితి K_KEND:		కీలకం = VTERM_కీలకం_KP_1; ఆపు; // TODO
	స్థితి K_KENTER:		కీలకం = VTERM_కీలకం_KP_ENTER; ఆపు;
	స్థితి K_KHOME:		కీలకం = VTERM_కీలకం_KP_7; ఆపు; // TODO
	స్థితి K_KINS:		కీలకం = VTERM_కీలకం_KP_0; ఆపు; // TODO
	స్థితి K_KMINUS:		కీలకం = VTERM_కీలకం_KP_MINUS; ఆపు;
	స్థితి K_KMULTIPLY:	కీలకం = VTERM_కీలకం_KP_MULT; ఆపు;
	స్థితి K_KPAGEDOWN:	కీలకం = VTERM_కీలకం_KP_3; ఆపు; // TODO
	స్థితి K_KPAGEUP:		కీలకం = VTERM_కీలకం_KP_9; ఆపు; // TODO
	స్థితి K_KPLUS:		కీలకం = VTERM_కీలకం_KP_PLUS; ఆపు;
	స్థితి K_KPOINT:		కీలకం = VTERM_కీలకం_KP_PERIOD; ఆపు;
	స్థితి K_ఎడమ:		కీలకం = VTERM_కీలకం_ఎడమ; ఆపు;
	స్థితి K_S_ఎడమ:		mod = VTERM_MOD_జరుపు;
				కీలకం = VTERM_కీలకం_ఎడమ; ఆపు;
	స్థితి K_C_ఎడమ:		mod = VTERM_MOD_CTRL;
				కీలకం = VTERM_కీలకం_ఎడమ; ఆపు;
	స్థితి K_PAGEDOWN:	కీలకం = VTERM_కీలకం_PAGEDOWN; ఆపు;
	స్థితి K_PAGEUP:		కీలకం = VTERM_కీలకం_PAGEUP; ఆపు;
	స్థితి K_కుడి:		కీలకం = VTERM_కీలకం_కుడి; ఆపు;
	స్థితి K_S_కుడి:		mod = VTERM_MOD_జరుపు;
				కీలకం = VTERM_కీలకం_కుడి; ఆపు;
	స్థితి K_C_కుడి:		mod = VTERM_MOD_CTRL;
				కీలకం = VTERM_కీలకం_కుడి; ఆపు;
	స్థితి K_పైకి:		కీలకం = VTERM_కీలకం_పైకి; ఆపు;
	స్థితి K_S_పైకి:		mod = VTERM_MOD_జరుపు;
				కీలకం = VTERM_కీలకం_పైకి; ఆపు;
	స్థితి TAB:		కీలకం = VTERM_కీలకం_TAB; ఆపు;
	స్థితి K_S_TAB:		mod = VTERM_MOD_జరుపు;
				కీలకం = VTERM_కీలకం_TAB; ఆపు;

	స్థితి K_MOUSEUP:		other = పదం_send_mouse(vterm, 5, 1); ఆపు;
	స్థితి K_MOUSEDOWN:	other = పదం_send_mouse(vterm, 4, 1); ఆపు;
	స్థితి K_MOUSELEFT:	other = పదం_send_mouse(vterm, 7, 1); ఆపు;
	స్థితి K_MOUSERIGHT:	other = పదం_send_mouse(vterm, 6, 1); ఆపు;

	స్థితి K_LEFTMOUSE:
	స్థితి K_LEFTMOUSE_NM:
	స్థితి K_LEFTDRAG:
	స్థితి K_LEFTRELEASE:
	స్థితి K_LEFTRELEASE_NM:
	స్థితి K_MOUSEMOVE:
	స్థితి K_MIDDLEMOUSE:
	స్థితి K_MIDDLEDRAG:
	స్థితి K_MIDDLERELEASE:
	స్థితి K_RIGHTMOUSE:
	స్థితి K_RIGHTDRAG:
	స్థితి K_RIGHTRELEASE:	ఒకవేళ (!పదం_mouse_click(vterm, c))
				    తిరిగి 0;
				other = TRUE;
				ఆపు;

	స్థితి K_X1MOUSE:		/* TODO */ తిరిగి 0;
	స్థితి K_X1DRAG:		/* TODO */ తిరిగి 0;
	స్థితి K_X1RELEASE:	/* TODO */ తిరిగి 0;
	స్థితి K_X2MOUSE:		/* TODO */ తిరిగి 0;
	స్థితి K_X2DRAG:		/* TODO */ తిరిగి 0;
	స్థితి K_X2RELEASE:	/* TODO */ తిరిగి 0;

	స్థితి K_IGNORE:		తిరిగి 0;
	స్థితి K_NOP:		తిరిగి 0;
	స్థితి K_UNDO:		తిరిగి 0;
	స్థితి K_HELP:		తిరిగి 0;
	స్థితి K_XF1:		కీలకం = VTERM_కీలకం_FUNCTION(1); ఆపు;
	స్థితి K_XF2:		కీలకం = VTERM_కీలకం_FUNCTION(2); ఆపు;
	స్థితి K_XF3:		కీలకం = VTERM_కీలకం_FUNCTION(3); ఆపు;
	స్థితి K_XF4:		కీలకం = VTERM_కీలకం_FUNCTION(4); ఆపు;
	స్థితి K_SELECT:		తిరిగి 0;
#నిర్వచితమైతే FEAT_GUI
	స్థితి K_VER_SCROLLBAR:	తిరిగి 0;
	స్థితి K_HOR_SCROLLBAR:	తిరిగి 0;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_GUI_TABLINE
	స్థితి K_TABLINE:		తిరిగి 0;
	స్థితి K_TABMENU:		తిరిగి 0;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_NETBEANS_INTG
	స్థితి K_F21:		కీలకం = VTERM_కీలకం_FUNCTION(21); ఆపు;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_DND
	స్థితి K_DROP:		తిరిగి 0;
#అయితే_ముగింపు
	స్థితి K_CURSORHOLD:	తిరిగి 0;
	స్థితి K_PS:		vterm_keyboard_ప్రారంభం_paste(vterm);
				other = TRUE;
				ఆపు;
	స్థితి K_PE:		vterm_keyboard_చివర_paste(vterm);
				other = TRUE;
				ఆపు;
    }

    // add modifiers for the typed key
    ఒకవేళ (modmask & MOD_ముసుగు_జరుపు)
	mod |= VTERM_MOD_జరుపు;
    ఒకవేళ (modmask & MOD_ముసుగు_CTRL)
	mod |= VTERM_MOD_CTRL;
    ఒకవేళ (modmask & (MOD_ముసుగు_ALT | MOD_ముసుగు_META))
	mod |= VTERM_MOD_ALT;

    // Ctrl-Shift-i may have the key "I" instead of "i", but for the kitty
    // keyboard protocol should use "i".  Applies to all ascii letters.
    ఒకవేళ (ASCII_ISUPPER(c)
	    && vterm_is_kitty_keyboard(vterm)
	    && mod == (VTERM_MOD_CTRL | VTERM_MOD_జరుపు))
	c = TOLOWER_ASC(c);

    /*
     * Convert special keys to vterm keys:
     * - Write keys to vterm: vterm_keyboard_key()
     * - Write output to channel.
     */
    ఒకవేళ (కీలకం != VTERM_కీలకం_NONE)
	// Special key, let vterm convert it.
	vterm_keyboard_కీలకం(vterm, కీలకం, mod);
    లేకపోతే ఒకవేళ (!other)
	// Normal character, let vterm convert it.
	vterm_keyboard_unichar(vterm, c, mod);

    // Read back the converted escape sequence.
    తిరిగి (పూర్ణాంకం)vterm_output_read(vterm, buf, కీలకం_BUF_LEN);
}

/*
 * Return TRUE if the job for "term" is still running.
 * If "check_job_status" is TRUE update the job status.
 * NOTE: "term" may be freed by callbacks.
 */
    స్థిర పూర్ణాంకం
పదం_పని_running_check(పదం_T *పదం, పూర్ణాంకం check_పని_స్థితి)
{
    // Also consider the job finished when the channel is closed, to avoid a
    // race condition when updating the title.
    ఒకవేళ (పదం == ఖాళీ
	|| పదం->tl_పని == ఖాళీ
	|| !channel_is_open(పదం->tl_పని->jv_channel))
	తిరిగి FALSE;

    పని_T *పని = పదం->tl_పని;

    // Careful: Checking the job status may invoke callbacks, which close
    // the buffer and terminate "term".  However, "job" will not be freed
    // yet.
    ఒకవేళ (check_పని_స్థితి)
	పని_స్థితి(పని);
    తిరిగి (పని->jv_స్థితి == పని_STARTED
	    || (పని->jv_channel != ఖాళీ && పని->jv_channel->ch_keep_open));
}

/*
 * Return TRUE if the job for "term" is still running.
 */
    పూర్ణాంకం
పదం_పని_running(పదం_T *పదం)
{
    తిరిగి పదం_పని_running_check(పదం, FALSE);
}

/*
 * Return TRUE if the job for "term" is still running, ignoring the job was
 * "NONE".
 */
    పూర్ణాంకం
పదం_పని_running_not_none(పదం_T *పదం)
{
    తిరిగి పదం_పని_running(పదం) && !పదం_none_open(పదం);
}

/*
 * Return TRUE if "term" has an active channel and used ":term NONE".
 */
    పూర్ణాంకం
పదం_none_open(పదం_T *పదం)
{
    // Also consider the job finished when the channel is closed, to avoid a
    // race condition when updating the title.
    తిరిగి పదం != ఖాళీ
	&& పదం->tl_పని != ఖాళీ
	&& channel_is_open(పదం->tl_పని->jv_channel)
	&& పదం->tl_పని->jv_channel->ch_keep_open;
}

//
// Used to confirm whether we would like to kill a terminal.
// Return OK when the user confirms to kill it.
// Return FAIL if the user selects otherwise.
//
    పూర్ణాంకం
పదం_confirm_stop(buf_T *buf)
{
    char_u	buff[DIALOG_సందేశం_కొలత];
    పూర్ణాంకం	ret;

    dialog_సందేశం(buff, _("Kill job in \"%s\"?"), buf_get_fname(buf));
    ret = vim_dialog_yesno(VIM_QUESTION, ఖాళీ, buff, 1);
    ఒకవేళ (ret == VIM_YES)
	తిరిగి OK;
    లేకపోతే
	తిరిగి FAIL;
}

/*
 * Used when exiting: kill the job in "buf" if so desired.
 * Return OK when the job finished.
 * Return FAIL when the job is still running.
 */
    పూర్ణాంకం
పదం_try_stop_పని(buf_T *buf)
{
    పూర్ణాంకం	    లెక్క;
    అక్షరం    *how = (అక్షరం *)buf->b_పదం->tl_kill;

#ఒకవేళ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
    ఒకవేళ ((how == ఖాళీ || *how == NUL)
			  && (p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)))
    {
	ఒకవేళ (పదం_confirm_stop(buf) == OK)
	    how = "kill";
	లేకపోతే
	    తిరిగి FAIL;
    }
#అయితే_ముగింపు
    ఒకవేళ (how == ఖాళీ || *how == NUL)
	తిరిగి FAIL;

    పని_stop(buf->b_పదం->tl_పని, ఖాళీ, how);

    // wait for up to a second for the job to die
    కొరకు (లెక్క = 0; లెక్క < 100; ++లెక్క)
    {
	పని_T *పని;

	// buffer, terminal and job may be cleaned up while waiting
	ఒకవేళ (!buf_valid(buf)
		|| buf->b_పదం == ఖాళీ
		|| buf->b_పదం->tl_పని == ఖాళీ)
	    తిరిగి OK;
	పని = buf->b_పదం->tl_పని;

	// Call job_status() to update jv_status. It may cause the job to be
	// cleaned up but it won't be freed.
	పని_స్థితి(పని);
	ఒకవేళ (పని->jv_స్థితి >= పని_ENDED)
	    తిరిగి OK;

	ui_delay(10L, TRUE);
	పదం_flush_messages();
    }
    తిరిగి FAIL;
}

/*
 * Add the last line of the scrollback buffer to the buffer in the window.
 */
    స్థిర శూన్య
కూడు_scrollback_పంక్తి_గమ్యం_బఫరు(పదం_T *పదం, char_u *పాఠ్యం, పూర్ణాంకం len, పూర్ణాంకం append)
{
    buf_T	*buf = పదం->tl_బఫరు;
    పూర్ణాంకం		ఖాళీలు_2 = (buf->b_ml.ml_flags & ML_ఖాళీలు);
    linenr_T	lnum = buf->b_ml.ml_పంక్తి_లెక్క;

#నిర్వచితమైతే MSWIN
    char_u *tmp = పాఠ్యం;
    ఒకవేళ (!enc_utf8 && enc_codepage > 0)
    {
	WCHAR   *ret = ఖాళీ;
	పూర్ణాంకం	నిడివి = 0;

	Multi_Byte_గమ్యం_Wide_Char_alloc(CP_UTF8, 0, (అక్షరం*)పాఠ్యం, len + 1,
							   &ret, &నిడివి);
	ఒకవేళ (ret != ఖాళీ)
	{
	    Wide_Char_గమ్యం_Multi_Byte_alloc(enc_codepage, 0,
				      ret, నిడివి, (అక్షరం **)&పాఠ్యం, &len, 0, 0);
	    vim_free(ret);
	}
    }
#అయితే_ముగింపు

    ఒకవేళ (append)
    {
	char_u *prev_పాఠ్యం = ml_get_buf(buf, lnum, FALSE);
	size_t prev_len = STRLEN(prev_పాఠ్యం);

	char_u *both = alloc(len + prev_len + 2);
	ఒకవేళ (both == ఖాళీ)
	    తిరిగి;
	vim_strncpy(both, prev_పాఠ్యం, prev_len + 1);
	vim_strncpy(both + prev_len, పాఠ్యం, len + 1);

	curbuf = buf;
	ml_replace(lnum, both, FALSE);
	curbuf = curwin->w_బఫరు;
    }
    లేకపోతే
	ml_append_buf(buf, lnum, పాఠ్యం, len + 1, FALSE);

#నిర్వచితమైతే MSWIN
    ఒకవేళ (tmp != పాఠ్యం)
	vim_free(పాఠ్యం);
#అయితే_ముగింపు
    ఒకవేళ (ఖాళీలు_2)
    {
	// Delete the empty line that was in the empty buffer.
	curbuf = buf;
	ml_delete(1);
	curbuf = curwin->w_బఫరు;
    }
}

    స్థిర శూన్య
cell2cellattr(స్థిరరాశి VTermScreenCell *cell, cellattr_T *attr)
{
    attr->వెడల్పు = cell->వెడల్పు;
    attr->attrs = cell->attrs;
    attr->fg = cell->fg;
    attr->bg = cell->bg;
}

    స్థిర పూర్ణాంకం
equal_celattr(cellattr_T *a, cellattr_T *b)
{
    // We only compare the RGB colors, ignoring the ANSI index and type.
    // Thus black set explicitly is equal the background black.
    తిరిగి a->fg.red == b->fg.red
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
    స్థిర పూర్ణాంకం
కూడు_ఖాళీలు_scrollback(పదం_T *పదం, cellattr_T *fill_attr, పూర్ణాంకం lnum)
{
    ఒకవేళ (ga_grow(&పదం->tl_scrollback, 1) == FAIL)
	తిరిగి FALSE;

    sb_పంక్తి_T *పంక్తి = (sb_పంక్తి_T *)పదం->tl_scrollback.ga_దత్తాంశం
	+ పదం->tl_scrollback.ga_len;

    ఒకవేళ (lnum > 0)
    {
	పూర్ణాంకం i;

	కొరకు (i = 0; i < పదం->tl_scrollback.ga_len - lnum; ++i)
	{
	    *పంక్తి = *(పంక్తి - 1);
	    --పంక్తి;
	}
    }
    పంక్తి->sb_నిలువుల_సంఖ్య = 0;
    పంక్తి->sb_బైట్లు = 0;
    పంక్తి->sb_cells = ఖాళీ;
    పంక్తి->sb_fill_attr = *fill_attr;
    ++పదం->tl_scrollback.ga_len;
    తిరిగి OK;
}

/*
 * Remove the terminal contents from the scrollback and the buffer.
 * Used before adding a new scrollback line or updating the buffer for lines
 * displayed in the terminal.
 */
    స్థిర శూన్య
cleanup_scrollback(పదం_T *పదం)
{
    sb_పంక్తి_T	*పంక్తి;
    garray_T	*gap;
    char_u	*bufline;
    size_t      bufline_నిడివి;

    curbuf = పదం->tl_బఫరు;
    gap = &పదం->tl_scrollback;
    bufline = ml_get_buf(curbuf, curbuf->b_ml.ml_పంక్తి_లెక్క, FALSE);
    bufline_నిడివి = STRLEN(bufline);
    అయితే (పదం->tl_scrollback_snapshot && gap->ga_len > 0)
    {
	పంక్తి = (sb_పంక్తి_T *)gap->ga_దత్తాంశం + gap->ga_len - 1;
	ఒకవేళ (పంక్తి->sb_బైట్లు < 0 || (size_t)పంక్తి->sb_బైట్లు > bufline_నిడివి)
	    ఆపు;
	bufline_నిడివి -= పంక్తి->sb_బైట్లు;
	ఒకవేళ (!bufline_నిడివి)
	{
	    ml_delete(curbuf->b_ml.ml_పంక్తి_లెక్క);
	    bufline = ml_get_buf(curbuf, curbuf->b_ml.ml_పంక్తి_లెక్క, FALSE);
	    bufline_నిడివి = STRLEN(bufline);
	}
	vim_free(పంక్తి->sb_cells);
	--gap->ga_len;
	--పదం->tl_scrollback_snapshot;
    }
    ఒకవేళ (bufline_నిడివి < STRLEN(bufline))
    {
	char_u *shortened = vim_strnsave(bufline, bufline_నిడివి);
	ml_replace(curbuf->b_ml.ml_పంక్తి_లెక్క, shortened, FALSE);
    }
    curbuf = curwin->w_బఫరు;
    ఒకవేళ (curbuf == పదం->tl_బఫరు)
	check_cursor();
}

/*
 * Add the current lines of the terminal to scrollback and to the buffer.
 */
    స్థిర శూన్య
update_snapshot(పదం_T *పదం)
{
    VTermScreen	    *screen;
    VTermState	    *state;
    పూర్ణాంకం		    len;
    పూర్ణాంకం		    పంక్తులు_skipped = 0;
    VTermPos	    pos;
    VTermScreenCell cell;
    cellattr_T	    fill_attr, new_fill_attr;
    cellattr_T	    *p;

    ch_log(పదం->tl_పని == ఖాళీ ? ఖాళీ : పదం->tl_పని->jv_channel,
				  "Adding terminal window snapshot to buffer");

    // First remove the lines that were appended before, they might be
    // outdated.
    cleanup_scrollback(పదం);

    screen = vterm_obtain_screen(పదం->tl_vterm);
    state = vterm_obtain_state(పదం->tl_vterm);
    fill_attr = new_fill_attr = పదం->tl_default_color;
    కొరకు (pos.అడ్డువరుస = 0; pos.అడ్డువరుస < పదం->tl_అడ్డువరుసలు; ++pos.అడ్డువరుస)
    {
	len = 0;
	కొరకు (pos.నిలువు = 0; pos.నిలువు < పదం->tl_నిలువుల_సంఖ్య; ++pos.నిలువు)
	    ఒకవేళ (vterm_screen_get_cell(screen, pos, &cell) != 0
						       && cell.chars[0] != NUL)
	    {
		len = pos.నిలువు + 1;
		new_fill_attr = పదం->tl_default_color;
	    }
	    లేకపోతే
		// Assume the last attr is the filler attr.
		cell2cellattr(&cell, &new_fill_attr);

	ఒకవేళ (len == 0 && equal_celattr(&new_fill_attr, &fill_attr))
	    ++పంక్తులు_skipped;
	లేకపోతే
	{
	    అయితే (పంక్తులు_skipped > 0)
	    {
		// Line was skipped, add an empty line.
		--పంక్తులు_skipped;
		ఒకవేళ (కూడు_ఖాళీలు_scrollback(పదం, &fill_attr, 0) == OK)
		{
		    కూడు_scrollback_పంక్తి_గమ్యం_బఫరు(పదం, (char_u *)"", 0, 0);
		    ++పదం->tl_scrollback_snapshot;
		}
	    }

	    ఒకవేళ (len == 0)
		p = ఖాళీ;
	    లేకపోతే
		p = ALLOC_MULT(cellattr_T, len);
	    ఒకవేళ ((p != ఖాళీ || len == 0)
				     && ga_grow(&పదం->tl_scrollback, 1) == OK)
	    {
		garray_T    ga;
		పూర్ణాంకం	    వెడల్పు;
		స్థిరరాశి VTerm_పంక్తి_Info *lineinfo;
		sb_పంక్తి_T   *పంక్తి = (sb_పంక్తి_T *)పదం->tl_scrollback.ga_దత్తాంశం
						  + పదం->tl_scrollback.ga_len;

		lineinfo = vterm_state_get_lineinfo(state, pos.అడ్డువరుస);

		ga_init2(&ga, 1, 100);
		కొరకు (pos.నిలువు = 0; pos.నిలువు < len; pos.నిలువు += వెడల్పు)
		{
		    ఒకవేళ (vterm_screen_get_cell(screen, pos, &cell) == 0)
		    {
			వెడల్పు = 1;
			CLEAR_సూచిక(p + pos.నిలువు);
			ఒకవేళ (ga_grow(&ga, 1) == OK)
			    ga.ga_len += utf_char2bytes(' ',
					     (char_u *)ga.ga_దత్తాంశం + ga.ga_len);
		    }
		    లేకపోతే
		    {
			వెడల్పు = cell.వెడల్పు;

			cell2cellattr(&cell, &p[pos.నిలువు]);
			ఒకవేళ (వెడల్పు == 2)
			    // second cell of double-width character has the
			    // same attributes.
			    p[pos.నిలువు + 1] = p[pos.నిలువు];

			// Each character can be up to 6 bytes.
			ఒకవేళ (ga_grow(&ga, VTERM_MAX_CHARS_చొప్పున_CELL * 6) == OK)
			{
			    పూర్ణాంకం	    i;
			    పూర్ణాంకం	    c;

			    కొరకు (i = 0; i < VTERM_MAX_CHARS_చొప్పున_CELL &&
				    ((c = cell.chars[i]) > 0 || i == 0); ++i)
				ga.ga_len += utf_char2bytes(c == NUL ? ' ' : c,
					     (char_u *)ga.ga_దత్తాంశం + ga.ga_len);
			}
		    }
		}
		పంక్తి->sb_నిలువుల_సంఖ్య = len;
		పంక్తి->sb_బైట్లు = ga.ga_len;
		పంక్తి->sb_cells = p;
		పంక్తి->sb_fill_attr = new_fill_attr;
		పంక్తి->continuation = (char_u)lineinfo->continuation;
		fill_attr = new_fill_attr;
		++పదం->tl_scrollback.ga_len;
		++పదం->tl_scrollback_snapshot;

		ఒకవేళ (ga_grow(&ga, 1) == FAIL)
		    కూడు_scrollback_పంక్తి_గమ్యం_బఫరు(పదం, (char_u *)"", 0, 0);
		లేకపోతే
		{
		    *((char_u *)ga.ga_దత్తాంశం + ga.ga_len) = NUL;
		    కూడు_scrollback_పంక్తి_గమ్యం_బఫరు(పదం, ga.ga_దత్తాంశం, ga.ga_len,
						  lineinfo->continuation);
		}
		ga_clear(&ga);
	    }
	    లేకపోతే
		vim_free(p);
	}
    }

    // Add trailing empty lines.
    కొరకు (pos.అడ్డువరుస = పదం->tl_scrollback.ga_len;
	    pos.అడ్డువరుస < పదం->tl_scrollback_scrolled + పదం->tl_cursor_pos.అడ్డువరుస;
	    ++pos.అడ్డువరుస)
    {
	ఒకవేళ (కూడు_ఖాళీలు_scrollback(పదం, &fill_attr, 0) == OK)
	{
	    కూడు_scrollback_పంక్తి_గమ్యం_బఫరు(పదం, (char_u *)"", 0, 0);
	    ++పదం->tl_scrollback_snapshot;
	}
    }

    పదం->tl_dirty_snapshot = FALSE;
#నిర్వచితమైతే FEAT_TIMERS
    పదం->tl_timer_సమితి = FALSE;
#అయితే_ముగింపు
}

/*
 * Loop over all windows in the current tab, and also curwin, which is not
 * encountered when using a terminal in a popup window.
 * Return TRUE if "*wp" was set to the next window.
 */
    స్థిర పూర్ణాంకం
for_all_windows_and_curwin(win_T **wp, పూర్ణాంకం *did_curwin)
{
    ఒకవేళ (*wp == ఖాళీ)
	*wp = firstwin;
    లేకపోతే ఒకవేళ ((*wp)->w_తదుపరి != ఖాళీ)
	*wp = (*wp)->w_తదుపరి;
    లేకపోతే ఒకవేళ (!*did_curwin)
	*wp = curwin;
    లేకపోతే
	తిరిగి FALSE;
    ఒకవేళ (*wp == curwin)
	*did_curwin = TRUE;
    తిరిగి TRUE;
}

/*
 * If needed, add the current lines of the terminal to scrollback and to the
 * buffer.  Called after the job has ended and when switching to
 * Terminal-Normal mode.
 * When "redraw" is TRUE redraw the windows that show the terminal.
 */
    శూన్య
may_move_terminal_గమ్యం_బఫరు(పదం_T *పదం, పూర్ణాంకం redraw)
{
    ఒకవేళ (పదం->tl_vterm == ఖాళీ)
	తిరిగి;

    // Update the snapshot only if something changes or the buffer does not
    // have all the lines.
    ఒకవేళ (పదం->tl_dirty_snapshot || పదం->tl_బఫరు->b_ml.ml_పంక్తి_లెక్క
					       <= పదం->tl_బఫరు_scrolled)
	update_snapshot(పదం);

    ఒకవేళ (redraw)
    {
	win_T	    *wp = ఖాళీ;
	పూర్ణాంకం	    did_curwin = FALSE;

	అయితే (for_all_windows_and_curwin(&wp, &did_curwin))
	{
	    ఒకవేళ (wp->w_బఫరు == పదం->tl_బఫరు)
	    {
		wp->w_cursor.lnum = పదం->tl_బఫరు->b_ml.ml_పంక్తి_లెక్క;
		wp->w_cursor.నిలువు = 0;
		wp->w_valid = 0;
		ఒకవేళ (wp->w_cursor.lnum >= wp->w_ఎత్తు)
		{
		    linenr_T min_topline = wp->w_cursor.lnum - wp->w_ఎత్తు + 1;

		    ఒకవేళ (wp->w_topline < min_topline)
			wp->w_topline = min_topline;
		}
		redraw_win_later(wp, UPD_NOT_VALID);
	    }
	}
    }
}

#ఒకవేళ defined(FEAT_TIMERS)
/*
 * Check if any terminal timer expired.  If so, copy text from the terminal to
 * the buffer.
 * Return the time until the next timer will expire.
 */
    పూర్ణాంకం
పదం_check_timers(పూర్ణాంకం తదుపరి_due_arg, proftime_T *now)
{
    పదం_T  *పదం;
    పూర్ణాంకం	    తదుపరి_due = తదుపరి_due_arg;

    FOR_ALL_TERMS(పదం)
    {
	ఒకవేళ (పదం->tl_timer_సమితి && !పదం->tl_సాధారణ_బహుళకం)
	{
	    దీర్ఘ    this_due = proftime_సమయం_ఎడమ(&పదం->tl_timer_due, now);

	    ఒకవేళ (this_due <= 1)
	    {
		పదం->tl_timer_సమితి = FALSE;
		may_move_terminal_గమ్యం_బఫరు(పదం, FALSE);
	    }
	    లేకపోతే ఒకవేళ (తదుపరి_due == -1 || తదుపరి_due > this_due)
		తదుపరి_due = this_due;
	}
    }

    తిరిగి తదుపరి_due;
}
#అయితే_ముగింపు

/*
 * When "normal_mode" is TRUE set the terminal to Terminal-Normal mode,
 * otherwise end it.
 */
    స్థిర శూన్య
సమితి_terminal_బహుళకం(పదం_T *పదం, పూర్ణాంకం సాధారణ_బహుళకం)
{
    పదం->tl_సాధారణ_బహుళకం = సాధారణ_బహుళకం;
    may_trigger_modechanged();
    ఒకవేళ (!సాధారణ_బహుళకం)
	handle_postponed_scrollback(పదం);
    VIM_CLEAR(పదం->tl_స్థితి_పాఠ్యం);
    ఒకవేళ (పదం->tl_బఫరు == curbuf)
	maketitle();
}

/*
 * Called after the job is finished and Terminal mode is not active:
 * Move the vterm contents into the scrollback buffer and free the vterm.
 */
    స్థిర శూన్య
cleanup_vterm(పదం_T *పదం)
{
    సమితి_terminal_బహుళకం(పదం, FALSE);
    ఒకవేళ (పదం->tl_ముగింపు != TL_ముగింపు_CLOSE)
	may_move_terminal_గమ్యం_బఫరు(పదం, TRUE);
    పదం_free_vterm(పదం);
}

/*
 * Switch from Terminal-Job mode to Terminal-Normal mode.
 * Suspends updating the terminal window.
 */
    స్థిర శూన్య
పదం_enter_సాధారణ_బహుళకం(శూన్య)
{
    పదం_T     *పదం = curbuf->b_పదం;
    linenr_T   lnum;
    పూర్ణాంకం        నిలువు;

    సమితి_terminal_బహుళకం(పదం, TRUE);

    // Append the current terminal contents to the buffer.
    may_move_terminal_గమ్యం_బఫరు(పదం, TRUE);

    // Move the window cursor to the position of the cursor in the
    // terminal.
    lnum = పదం->tl_బఫరు_scrolled + 1 + పదం->tl_cursor_pos.అడ్డువరుస;
    నిలువు = పదం->tl_cursor_pos.నిలువు;
    scrollbackline_pos_లోపల_buf(పదం, పదం->tl_cursor_pos.అడ్డువరుస + పదం->tl_scrollback_scrolled, &lnum, &నిలువు, ఖాళీ);

    curwin->w_cursor.lnum = lnum;
    check_cursor();
    ఒకవేళ (coladvance(నిలువు) == FAIL)
	coladvance(MAXCOL);
    curwin->w_సమితి_curswant = true;
    curwin->w_topline = పదం->tl_బఫరు_scrolled + 1;
}

/*
 * Returns TRUE if the current window contains a terminal and we are in
 * Terminal-Normal mode.
 */
    పూర్ణాంకం
పదం_లోపల_సాధారణ_బహుళకం(buf_T *buf)
{
    పదం_T *పదం = buf->b_పదం;

    తిరిగి పదం != ఖాళీ && పదం->tl_సాధారణ_బహుళకం;
}

/*
 * Switch from Terminal-Normal mode to Terminal-Job mode.
 * Restores updating the terminal window.
 */
    శూన్య
పదం_enter_పని_బహుళకం(శూన్య)
{
    పదం_T	*పదం = curbuf->b_పదం;

    సమితి_terminal_బహుళకం(పదం, FALSE);

    ఒకవేళ (పదం->tl_channel_closed)
	cleanup_vterm(పదం);
    redraw_buf_and_స్థితి_later(curbuf, UPD_NOT_VALID);
#నిర్వచితమైతే FEAT_PROP_POPUP
    ఒకవేళ (WIN_IS_POPUP(curwin))
	redraw_later(UPD_NOT_VALID);
#అయితే_ముగింపు
}

/*
 * When "modify_other_keys" is set then vgetc() should not reduce a key with
 * modifiers into a basic key.  However, we may only find out after calling
 * vgetc().  Therefore vgetorpeek() will call check_no_reduce_keys() to update
 * "no_reduce_keys" before using it.
 */
రకనిర్వచనం గణన {
    NRKS_NONE,	    // initial value
    NRKS_CHECK,	    // modify_other_keys was off before calling vgetc()
    NRKS_సమితి,	    // no_reduce_keys was incremented in term_vgetc() or
		    // check_no_reduce_keys(), must be decremented.
} reduce_కీలకం_state_T;

స్థిర reduce_కీలకం_state_T  no_reduce_కీలకం_state = NRKS_NONE;

/*
 * Return TRUE if the term is using modifyOtherKeys level 2 or the kitty
 * keyboard protocol.
 */
    స్థిర పూర్ణాంకం
vterm_using_కీలకం_protocol(శూన్య)
{
    తిరిగి curbuf->b_పదం != ఖాళీ
	&& curbuf->b_పదం->tl_vterm != ఖాళీ
	&& (vterm_is_modify_other_కీలకాలు(curbuf->b_పదం->tl_vterm)
		|| vterm_is_kitty_keyboard(curbuf->b_పదం->tl_vterm));
}

    శూన్య
check_no_reduce_కీలకాలు(శూన్య)
{
    ఒకవేళ (no_reduce_కీలకం_state != NRKS_CHECK
	    || no_reduce_కీలకాలు >= 1
	    || curbuf->b_పదం == ఖాళీ
	    || curbuf->b_పదం->tl_vterm == ఖాళీ)
	తిరిగి;

    ఒకవేళ (vterm_using_కీలకం_protocol())
    {
	// "modify_other_keys" or kitty keyboard protocol was enabled while
	// waiting.
	no_reduce_కీలకం_state = NRKS_సమితి;
	++no_reduce_కీలకాలు;
    }
}

/*
 * Get a key from the user with terminal mode mappings.
 * Note: while waiting a terminal may be closed and freed if the channel is
 * closed and ++close was used.  This may even happen before we get here.
 */
    స్థిర పూర్ణాంకం
పదం_vgetc(శూన్య)
{
    పూర్ణాంకం c;
    పూర్ణాంకం save_State = State;

    State = బహుళకం_TERMINAL;
    got_int = FALSE;
#నిర్వచితమైతే MSWIN
    ctrl_break_was_pressed = FALSE;
#అయితే_ముగింపు

    ఒకవేళ (vterm_using_కీలకం_protocol())
    {
	++no_reduce_కీలకాలు;
	no_reduce_కీలకం_state = NRKS_సమితి;
    }
    లేకపోతే
    {
	no_reduce_కీలకం_state = NRKS_CHECK;
    }

    c = vgetc();
    got_int = FALSE;
    State = save_State;

    ఒకవేళ (no_reduce_కీలకం_state == NRKS_సమితి)
	--no_reduce_కీలకాలు;
    no_reduce_కీలకం_state = NRKS_NONE;

    తిరిగి c;
}

స్థిర పూర్ణాంకం	mouse_was_outside = FALSE;

/*
 * Send key "c" with modifiers "modmask" to terminal.
 * Return FAIL when the key needs to be handled in Normal mode.
 * Return OK when the key was dropped or sent to the terminal.
 */
    పూర్ణాంకం
send_కీలకాలు_గమ్యం_పదం(పదం_T *పదం, పూర్ణాంకం c, పూర్ణాంకం modmask, పూర్ణాంకం typed)
{
    అక్షరం	సందేశం[కీలకం_BUF_LEN];
    size_t	len;
    పూర్ణాంకం		dragging_outside = FALSE;

    // Catch keys that need to be handled as in Normal mode.
    ఎంపిక (c)
    {
	స్థితి NUL:
	స్థితి K_ZERO:
	    ఒకవేళ (typed)
		stuffcharReadbuff(c);
	    తిరిగి FAIL;

	స్థితి K_TABLINE:
	    stuffcharReadbuff(c);
	    తిరిగి FAIL;

	స్థితి K_IGNORE:
	స్థితి K_CANCEL:  // used for :normal when running out of chars
	    తిరిగి FAIL;

	స్థితి K_LEFTDRAG:
	స్థితి K_MIDDLEDRAG:
	స్థితి K_RIGHTDRAG:
	స్థితి K_X1DRAG:
	స్థితి K_X2DRAG:
	    dragging_outside = mouse_was_outside;
	    // FALLTHROUGH
	స్థితి K_LEFTMOUSE:
	స్థితి K_LEFTMOUSE_NM:
	స్థితి K_LEFTRELEASE:
	స్థితి K_LEFTRELEASE_NM:
	స్థితి K_MOUSEMOVE:
	స్థితి K_MIDDLEMOUSE:
	స్థితి K_MIDDLERELEASE:
	స్థితి K_RIGHTMOUSE:
	స్థితి K_RIGHTRELEASE:
	స్థితి K_X1MOUSE:
	స్థితి K_X1RELEASE:
	స్థితి K_X2MOUSE:
	స్థితి K_X2RELEASE:

	స్థితి K_MOUSEUP:
	స్థితి K_MOUSEDOWN:
	స్థితి K_MOUSELEFT:
	స్థితి K_MOUSERIGHT:
	    {
		పూర్ణాంకం	అడ్డువరుస = mouse_అడ్డువరుస;
		పూర్ణాంకం	నిలువు = mouse_నిలువు;

#నిర్వచితమైతే FEAT_PROP_POPUP
		ఒకవేళ (popup_is_popup(curwin))
		{
		    అడ్డువరుస -= popup_శిఖరం_extra(curwin);
		    నిలువు -= popup_ఎడమ_extra(curwin);
		}
#అయితే_ముగింపు
		ఒకవేళ (అడ్డువరుస < W_WINROW(curwin)
			|| అడ్డువరుస >= (W_WINROW(curwin) + curwin->w_ఎత్తు)
			|| నిలువు < curwin->w_wincol
			|| నిలువు >= W_ENDCOL(curwin)
			|| dragging_outside)
		{
		    // click or scroll outside the current window or on status
		    // line or vertical separator
		    ఒకవేళ (typed)
		    {
			stuffcharReadbuff(c);
			mouse_was_outside = TRUE;
		    }
		    తిరిగి FAIL;
		}
	    }
	    ఆపు;

	స్థితి K_COMMAND:
	స్థితి K_SCRIPT_COMMAND:
	    తిరిగి do_cmdkey_command(c, 0);
    }
    ఒకవేళ (typed)
	mouse_was_outside = FALSE;

    // Convert the typed key to a sequence of bytes for the job.
    len = పదం_convert_కీలకం(పదం, c, modmask, సందేశం);
    ఒకవేళ (len > 0)
	// TODO: if FAIL is returned, stop?
	channel_send(పదం->tl_పని->jv_channel, get_tty_part(పదం),
						(char_u *)సందేశం, (పూర్ణాంకం)len, ఖాళీ);

    తిరిగి OK;
}

/*
 * Handle CTRL-W "": send register contents to the job.
 */
    స్థిర శూన్య
పదం_paste_రిజిస్టరు(పూర్ణాంకం prev_c వాడని)
{
    పూర్ణాంకం		c;
    list_T	*l;
    listitem_T	*అంశం;
    దీర్ఘ	reglen = 0;
    పూర్ణాంకం		type;

    ఒకవేళ (కూడు_గమ్యం_showcmd(prev_c))
    ఒకవేళ (కూడు_గమ్యం_showcmd('"'))
	బహిర్గతం_flush();

    c = పదం_vgetc();
    clear_showcmd();

    ఒకవేళ (!పదం_use_loop())
	// job finished while waiting for a character
	తిరిగి;

    // CTRL-W "= prompt for expression to evaluate.
    ఒకవేళ (c == '=' && get_సమాసం_రిజిస్టరు() != '=')
	తిరిగి;
    ఒకవేళ (!పదం_use_loop())
	// job finished while waiting for a character
	తిరిగి;

    l = (list_T *)get_reg_contents(c, GREG_LIST);
    ఒకవేళ (l == ఖాళీ)
	తిరిగి;

    type = get_reg_type(c, &reglen);
    FOR_ALL_LIST_వస్తువులు(l, అంశం)
    {
	char_u *s = tv_get_string(&అంశం->li_tv);
#నిర్వచితమైతే MSWIN
	char_u *tmp = s;

	ఒకవేళ (!enc_utf8 && enc_codepage > 0)
	{
	    WCHAR   *ret = ఖాళీ;
	    పూర్ణాంకం	నిడివి = 0;

	    Multi_Byte_గమ్యం_Wide_Char_alloc(enc_codepage, 0, (అక్షరం *)s,
		    (పూర్ణాంకం)STRLEN(s), &ret, &నిడివి);
	    ఒకవేళ (ret != ఖాళీ)
	    {
		Wide_Char_గమ్యం_Multi_Byte_alloc(CP_UTF8, 0,
			ret, నిడివి, (అక్షరం **)&s, &నిడివి, 0, 0);
		vim_free(ret);
	    }
	}
#అయితే_ముగింపు
	channel_send(curbuf->b_పదం->tl_పని->jv_channel, PART_లోపల,
		s, (పూర్ణాంకం)STRLEN(s), ఖాళీ);
#నిర్వచితమైతే MSWIN
	ఒకవేళ (tmp != s)
	    vim_free(s);
#అయితే_ముగింపు

	ఒకవేళ (అంశం->li_తదుపరి != ఖాళీ || type == MLINE)
	    channel_send(curbuf->b_పదం->tl_పని->jv_channel, PART_లోపల,
		    (char_u *)"\r", 1, ఖాళీ);
    }
    list_free(l);
}

/*
 * Return TRUE when waiting for a character in the terminal, the cursor of the
 * terminal should be displayed.
 */
    పూర్ణాంకం
terminal_is_చురుకైన(శూన్య)
{
    తిరిగి లోపల_terminal_loop != ఖాళీ;
}

/*
 * Return the highlight group ID for the terminal and the window.
 */
    స్థిర పూర్ణాంకం
పదం_get_highlight_id(పదం_T *పదం, win_T *wp)
{
    char_u *name;

    ఒకవేళ (wp != ఖాళీ && wp->w_hlfwin_id != 0)
	name = syn_id2name(wp->w_hlfwin_id);
    లేకపోతే ఒకవేళ (పదం->tl_highlight_name != ఖాళీ)
	name = పదం->tl_highlight_name;
    లేకపోతే
	name = (char_u*)"Terminal";

    తిరిగి syn_name2id(name);
}

#ఒకవేళ defined(FEAT_GUI)
    cursorentry_T *
పదం_get_cursor_shape(guicolor_T *fg, guicolor_T *bg)
{
    పదం_T		 *పదం = లోపల_terminal_loop;
    స్థిర cursorentry_T నమోదు_2;
    పూర్ణాంకం			 id;
    guicolor_T		 పదం_fg = INVALCOLOR;
    guicolor_T		 పదం_bg = INVALCOLOR;

    CLEAR_క్షేత్రం(నమోదు_2);
    నమోదు_2.shape = నమోదు_2.mshape =
	పదం->tl_cursor_shape == VTERM_PROP_CURSORSHAPE_UNDERLINE ? SHAPE_HOR :
	పదం->tl_cursor_shape == VTERM_PROP_CURSORSHAPE_BAR_ఎడమ ? SHAPE_VER :
	SHAPE_ఖండం;
    నమోదు_2.percentage = 20;
    ఒకవేళ (పదం->tl_cursor_blink)
    {
	నమోదు_2.blinkwait = 700;
	నమోదు_2.blinkon = 400;
	నమోదు_2.blinkoff = 250;
    }

    // The highlight group overrules the defaults.
    id = పదం_get_highlight_id(పదం, curwin);
    ఒకవేళ (id != 0)
	syn_id2colors(id, &పదం_fg, &పదం_bg);
    ఒకవేళ (పదం_bg != INVALCOLOR)
	*fg = పదం_bg;
    లేకపోతే
	*fg = gui.వెనుక_pixel;

    ఒకవేళ (పదం->tl_cursor_color == ఖాళీ)
    {
	ఒకవేళ (పదం_fg != INVALCOLOR)
	    *bg = పదం_fg;
	లేకపోతే
	    *bg = gui.norm_pixel;
    }
    లేకపోతే
	*bg = color_name2handle(పదం->tl_cursor_color);
    నమోదు_2.name = "n";
    నమోదు_2.వాడిన_for = SHAPE_CURSOR;

    తిరిగి &నమోదు_2;
}
#అయితే_ముగింపు

    స్థిర శూన్య
may_output_cursor_props(శూన్య)
{
    ఒకవేళ (!cursor_color_equal(చివరి_సమితి_cursor_color, desired_cursor_color)
	    || చివరి_సమితి_cursor_shape != desired_cursor_shape
	    || చివరి_సమితి_cursor_blink != desired_cursor_blink)
    {
	cursor_color_copy(&చివరి_సమితి_cursor_color, desired_cursor_color);
	చివరి_సమితి_cursor_shape = desired_cursor_shape;
	చివరి_సమితి_cursor_blink = desired_cursor_blink;
	పదం_cursor_color(cursor_color_get(desired_cursor_color));
	ఒకవేళ (desired_cursor_shape == -1 || desired_cursor_blink == -1)
	    // this will restore the initial cursor style, if possible
	    ui_cursor_shape_forced(TRUE);
	లేకపోతే
	    పదం_cursor_shape(desired_cursor_shape, desired_cursor_blink);
    }
}

/*
 * Set the cursor color and shape, if not last set to these.
 */
    స్థిర శూన్య
may_సమితి_cursor_props(పదం_T *పదం)
{
#నిర్వచితమైతే FEAT_GUI
    // For the GUI the cursor properties are obtained with
    // term_get_cursor_shape().
    ఒకవేళ (gui.లోపల_use)
	తిరిగి;
#అయితే_ముగింపు
    ఒకవేళ (లోపల_terminal_loop == పదం)
    {
	cursor_color_copy(&desired_cursor_color, పదం->tl_cursor_color);
	desired_cursor_shape = పదం->tl_cursor_shape;
	desired_cursor_blink = పదం->tl_cursor_blink;
	may_output_cursor_props();
    }
}

/*
 * Reset the desired cursor properties and restore them when needed.
 */
    స్థిర శూన్య
prepare_restore_cursor_props(శూన్య)
{
#నిర్వచితమైతే FEAT_GUI
    ఒకవేళ (gui.లోపల_use)
	తిరిగి;
#అయితే_ముగింపు
    cursor_color_copy(&desired_cursor_color, ఖాళీ);
    desired_cursor_shape = -1;
    desired_cursor_blink = -1;
    may_output_cursor_props();
}

/*
 * Returns TRUE if the current window contains a terminal and we are sending
 * keys to the job.
 * If "check_job_status" is TRUE update the job status.
 */
    స్థిర పూర్ణాంకం
పదం_use_loop_check(పూర్ణాంకం check_పని_స్థితి)
{
    పదం_T *పదం = curbuf->b_పదం;

    తిరిగి పదం != ఖాళీ
	&& !పదం->tl_సాధారణ_బహుళకం
	&& పదం->tl_vterm != ఖాళీ
	&& పదం_పని_running_check(పదం, check_పని_స్థితి);
}

/*
 * Returns TRUE if the current window contains a terminal and we are sending
 * keys to the job.
 */
    పూర్ణాంకం
పదం_use_loop(శూన్య)
{
    తిరిగి పదం_use_loop_check(FALSE);
}

/*
 * Called when entering a window with the mouse.  If this is a terminal window
 * we may want to change state.
 */
    శూన్య
పదం_win_entered(శూన్య)
{
    పదం_T *పదం = curbuf->b_పదం;

    ఒకవేళ (పదం == ఖాళీ)
	తిరిగి;

    ఒకవేళ (పదం_use_loop_check(TRUE))
    {
	reset_VIsual_and_resel();
	ఒకవేళ (State & బహుళకం_చొప్పించు)
	    stop_చొప్పించు_బహుళకం = TRUE;
    }
    mouse_was_outside = FALSE;
    enter_mouse_నిలువు = mouse_నిలువు;
    enter_mouse_అడ్డువరుస = mouse_అడ్డువరుస;
}

    శూన్య
పదం_focus_change(పూర్ణాంకం లోపల_focus)
{
    పదం_T *పదం = curbuf->b_పదం;

    ఒకవేళ (పదం == ఖాళీ || పదం->tl_vterm == ఖాళీ)
	తిరిగి;

    VTermState	*state = vterm_obtain_state(పదం->tl_vterm);

    ఒకవేళ (లోపల_focus)
	vterm_state_focus_లోపల(state);
    లేకపోతే
	vterm_state_focus_బహిర్గతం(state);
    పదం_forward_output(పదం);
}

/*
 * vgetc() may not include CTRL in the key when modify_other_keys is set.
 * Return the Ctrl-key value in that case.
 */
    స్థిర పూర్ణాంకం
raw_c_గమ్యం_ctrl(పూర్ణాంకం c)
{
    ఒకవేళ ((mod_ముసుగు & MOD_ముసుగు_CTRL)
	    && ((c >= '`' && c <= 0x7f) || (c >= '@' && c <= '_')))
	తిరిగి c & 0x1f;
    తిరిగి c;
}

/*
 * When modify_other_keys is set then do the reverse of raw_c_to_ctrl().
 * Also when the Kitty keyboard protocol is used.
 * May set "mod_mask".
 */
    స్థిర పూర్ణాంకం
ctrl_గమ్యం_raw_c(పూర్ణాంకం c)
{
    ఒకవేళ (c < 0x20 && vterm_using_కీలకం_protocol())
    {
	mod_ముసుగు |= MOD_ముసుగు_CTRL;
	తిరిగి c + '@';
    }
    తిరిగి c;
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
    పూర్ణాంకం
terminal_loop(పూర్ణాంకం blocking)
{
    పూర్ణాంకం		c;
    పూర్ణాంకం		raw_c;
    పూర్ణాంకం		termwinkey = 0;
    పూర్ణాంకం		ret;
#నిర్వచితమైతే UNIX
    పూర్ణాంకం		tty_fd = curbuf->b_పదం->tl_పని->jv_channel
				 ->ch_part[get_tty_part(curbuf->b_పదం)].ch_fd;
#అయితే_ముగింపు
    పూర్ణాంకం		restore_cursor = FALSE;

    // Remember the terminal we are sending keys to.  However, the terminal
    // might be closed while waiting for a character, e.g. typing "exit" in a
    // shell and ++close was used.  Therefore use curbuf->b_term instead of a
    // stored reference.
    లోపల_terminal_loop = curbuf->b_పదం;

    ఒకవేళ (*curwin->w_p_twk != NUL)
    {
	termwinkey = string_గమ్యం_కీలకం(curwin->w_p_twk, TRUE);
	ఒకవేళ (termwinkey == Ctrl_W)
	    termwinkey = 0;
    }
    position_cursor(curwin, &curbuf->b_పదం->tl_cursor_pos);
    may_సమితి_cursor_props(curbuf->b_పదం);

    అయితే (blocking || vpeekc_nomap() != NUL)
    {
#నిర్వచితమైతే FEAT_GUI
	ఒకవేళ (curbuf->b_పదం != ఖాళీ && !curbuf->b_పదం->tl_system)
#అయితే_ముగింపు
	    // TODO: skip screen update when handling a sequence of keys.
	    // Repeat redrawing in case a message is received while redrawing.
	    అయితే (must_redraw != 0)
		ఒకవేళ (update_screen(0) == FAIL)
		    ఆపు;
	ఒకవేళ (!పదం_use_loop_check(TRUE) || లోపల_terminal_loop != curbuf->b_పదం)
	    // job finished while redrawing
	    ఆపు;

	update_cursor(curbuf->b_పదం, FALSE);
	restore_cursor = TRUE;

	raw_c = పదం_vgetc();
	ఒకవేళ (!పదం_use_loop_check(TRUE) || లోపల_terminal_loop != curbuf->b_పదం)
	{
	    // Job finished while waiting for a character.  Push back the
	    // received character.
	    ఒకవేళ (raw_c != K_IGNORE)
		vungetc(raw_c);
	    ఆపు;
	}
	ఒకవేళ (raw_c == K_IGNORE)
	    కొనసాగు;
	c = raw_c_గమ్యం_ctrl(raw_c);

#నిర్వచితమైతే UNIX
	/*
	 * The shell or another program may change the tty settings.  Getting
	 * them for every typed character is a bit of overhead, but it's needed
	 * for the first character typed, e.g. when Vim starts in a shell.
	 */
	ఒకవేళ (mch_isatty(tty_fd))
	{
	    ttyinfo_T info;

	    // Get the current backspace character of the pty.
	    ఒకవేళ (get_tty_info(tty_fd, &info) == OK)
		పదం_backspace_char = info.backspace;
	}
#అయితే_ముగింపు

#నిర్వచితమైతే MSWIN
	// On Windows winpty handles CTRL-C, don't send a CTRL_C_EVENT.
	// Use CTRL-BREAK to kill the job.
	ఒకవేళ (ctrl_break_was_pressed)
	    mch_signal_పని(curbuf->b_పదం->tl_పని, (char_u *)"kill");
#అయితే_ముగింపు
	// Was either CTRL-W (termwinkey) or CTRL-\ pressed?
	// Not in a system terminal.
	ఒకవేళ ((c == (termwinkey == 0 ? Ctrl_W : termwinkey) || c == Ctrl_BSL)
#నిర్వచితమైతే FEAT_GUI
		&& !curbuf->b_పదం->tl_system
#అయితే_ముగింపు
		)
	{
	    పూర్ణాంకం	    prev_c = c;
	    పూర్ణాంకం	    prev_raw_c = raw_c;
	    పూర్ణాంకం	    prev_mod_ముసుగు = mod_ముసుగు;

	    ఒకవేళ (కూడు_గమ్యం_showcmd(c))
		బహిర్గతం_flush();

	    raw_c = పదం_vgetc();
	    c = raw_c_గమ్యం_ctrl(raw_c);

	    clear_showcmd();

	    ఒకవేళ (!పదం_use_loop_check(TRUE)
					 || లోపల_terminal_loop != curbuf->b_పదం)
		// job finished while waiting for a character
		ఆపు;

	    ఒకవేళ (prev_c == Ctrl_BSL)
	    {
		ఒకవేళ (c == Ctrl_N)
		{
		    // CTRL-\ CTRL-N : go to Terminal-Normal mode.
		    పదం_enter_సాధారణ_బహుళకం();
		    ret = FAIL;
		    వెళ్ళు theend;
		}
		// Send both keys to the terminal, first one here, second one
		// below.
		send_కీలకాలు_గమ్యం_పదం(curbuf->b_పదం, prev_raw_c, prev_mod_ముసుగు,
									 TRUE);
	    }
	    లేకపోతే ఒకవేళ (c == Ctrl_C)
	    {
		// "CTRL-W CTRL-C" or 'termwinkey' CTRL-C: end the job
		mch_signal_పని(curbuf->b_పదం->tl_పని, (char_u *)"kill");
	    }
	    లేకపోతే ఒకవేళ (c == '.')
	    {
		// "CTRL-W .": send CTRL-W to the job
		// "'termwinkey' .": send 'termwinkey' to the job
		raw_c = ctrl_గమ్యం_raw_c(termwinkey == 0 ? Ctrl_W : termwinkey);
	    }
	    లేకపోతే ఒకవేళ (c == Ctrl_BSL)
	    {
		// "CTRL-W CTRL-\": send CTRL-\ to the job
		raw_c = ctrl_గమ్యం_raw_c(Ctrl_BSL);
	    }
	    లేకపోతే ఒకవేళ (c == 'N')
	    {
		// CTRL-W N : go to Terminal-Normal mode.
		పదం_enter_సాధారణ_బహుళకం();
		ret = FAIL;
		వెళ్ళు theend;
	    }
	    లేకపోతే ఒకవేళ (c == '"')
	    {
		పదం_paste_రిజిస్టరు(prev_c);
		కొనసాగు;
	    }
	    లేకపోతే ఒకవేళ (termwinkey == 0 || c != termwinkey)
	    {
		// space for CTRL-W, modifier, multi-byte char and NUL
		char_u buf[1 + 3 + MB_MAXBYTES + 1];

		// Put the command into the typeahead buffer, when using the
		// stuff buffer KeyStuffed is set and 'langmap' won't be used.
		buf[0] = Ctrl_W;
		buf[special_గమ్యం_buf(c, mod_ముసుగు, FALSE, buf + 1) + 1] = NUL;
		ins_typebuf(buf, REMAP_NONE, 0, TRUE, FALSE);
		ret = OK;
		వెళ్ళు theend;
	    }
	}
#నిర్వచితమైతే MSWIN
	ఒకవేళ (!enc_utf8 && has_mbyte && raw_c >= 0x80)
	{
	    WCHAR   wc;
	    char_u  mb[3];

	    mb[0] = (గుర్తులేని)raw_c >> 8;
	    mb[1] = raw_c;
	    ఒకవేళ (Multi_Byte_గమ్యం_Wide_Char(GetACP(), 0, (అక్షరం*)mb, 2, &wc, 1) > 0)
		raw_c = wc;
	}
#అయితే_ముగింపు
	ఒకవేళ (send_కీలకాలు_గమ్యం_పదం(curbuf->b_పదం, raw_c, mod_ముసుగు, TRUE) != OK)
	{
	    ఒకవేళ (raw_c == K_MOUSEMOVE)
		// We are sure to come back here, don't reset the cursor color
		// and shape to avoid flickering.
		restore_cursor = FALSE;

	    ret = OK;
	    వెళ్ళు theend;
	}
    }
    ret = FAIL;

theend:
    లోపల_terminal_loop = ఖాళీ;
    ఒకవేళ (restore_cursor)
	prepare_restore_cursor_props();

    // Move a snapshot of the screen contents to the buffer, so that completion
    // works in other buffers.
    ఒకవేళ (curbuf->b_పదం != ఖాళీ && !curbuf->b_పదం->tl_సాధారణ_బహుళకం)
	may_move_terminal_గమ్యం_బఫరు(curbuf->b_పదం, FALSE);

    తిరిగి ret;
}

    స్థిర శూన్య
may_toggle_cursor(పదం_T *పదం)
{
    ఒకవేళ (లోపల_terminal_loop != పదం)
	తిరిగి;

    ఒకవేళ (పదం->tl_cursor_visible)
	cursor_on();
    లేకపోతే
	cursor_off();
}

/*
 * Reverse engineer the RGB value into a cterm color index.
 * First color is 1.  Return 0 if no match found (default color).
 */
    స్థిర పూర్ణాంకం
color2index(VTermColor *color, పూర్ణాంకం fg, పూర్ణాంకం *boldp)
{
    పూర్ణాంకం red = color->red;
    పూర్ణాంకం blue = color->blue;
    పూర్ణాంకం green = color->green;

    *boldp = FALSE;

    ఒకవేళ (VTERM_COLOR_IS_INVALID(color))
	తిరిగి 0;

    ఒకవేళ (VTERM_COLOR_IS_సూచీకృత(color))
    {
	// Use the color as-is if possible, give up otherwise.
	ఒకవేళ (color->సూచీ < t_colors)
	{
	    uint8_t సూచీ = color->సూచీ;
#నిర్వచితమైతే MSWIN
	    // Convert ANSI palette to cterm color index.
	    // cterm_ansi_idx is a table for the reverse conversion from cterm
	    // color index to ANSI palette. However, the current table can
	    // be used for this conversion as well.
	    // Note: If the contents of cterm_ansi_idx change in the future, a
	    // different table may be needed for this purpose.
	    ఒకవేళ (సూచీ < 16)
		సూచీ = cterm_ansi_idx[సూచీ];
#అయితే_ముగింపు
	    తిరిగి సూచీ + 1;
	}
	// 8-color terminals can actually display twice as many colors by
	// setting the high-intensity/bold bit.
	లేకపోతే ఒకవేళ (t_colors == 8 && fg && color->సూచీ < 16)
	{
	    *boldp = TRUE;
	    తిరిగి (color->సూచీ & 7) + 1;
	}
	తిరిగి 0;
    }

    ఒకవేళ (t_colors >= 256)
    {
	ఒకవేళ (red == blue && red == green)
	{
	    // 24-color greyscale plus white and black
	    స్థిర పూర్ణాంకం cutoff[23] = {
		    0x0D, 0x17, 0x21, 0x2B, 0x35, 0x3F, 0x49, 0x53, 0x5D, 0x67,
		    0x71, 0x7B, 0x85, 0x8F, 0x99, 0xA3, 0xAD, 0xB7, 0xC1, 0xCB,
		    0xD5, 0xDF, 0xE9};
	    పూర్ణాంకం i;

	    ఒకవేళ (red < 5)
		తిరిగి 17; // 00/00/00
	    ఒకవేళ (red > 245) // ff/ff/ff
		తిరిగి 232;
	    కొరకు (i = 0; i < 23; ++i)
		ఒకవేళ (red < cutoff[i])
		    తిరిగి i + 233;
	    తిరిగి 256;
	}
	{
	    స్థిర పూర్ణాంకం cutoff[5] = {0x2F, 0x73, 0x9B, 0xC3, 0xEB};
	    పూర్ణాంకం ri, gi, bi;

	    // 216-color cube
	    కొరకు (ri = 0; ri < 5; ++ri)
		ఒకవేళ (red < cutoff[ri])
		    ఆపు;
	    కొరకు (gi = 0; gi < 5; ++gi)
		ఒకవేళ (green < cutoff[gi])
		    ఆపు;
	    కొరకు (bi = 0; bi < 5; ++bi)
		ఒకవేళ (blue < cutoff[bi])
		    ఆపు;
	    తిరిగి 17 + ri * 36 + gi * 6 + bi;
	}
    }
    తిరిగి 0;
}

/*
 * Convert Vterm attributes to highlight flags.
 */
    స్థిర పూర్ణాంకం
vtermAttr2hl(VTermScreenCellAttrs *cellattrs)
{
    పూర్ణాంకం attr = 0;

    ఒకవేళ (cellattrs->bold)
	attr |= HL_BOLD;
    ఒకవేళ (cellattrs->underline)
	attr |= HL_UNDERLINE;
    ఒకవేళ (cellattrs->italic)
	attr |= HL_ITALIC;
    ఒకవేళ (cellattrs->strike)
	attr |= HL_STRIKETHROUGH;
    ఒకవేళ (cellattrs->తిరగేయి)
	attr |= HL_INVERSE;
    తిరిగి attr;
}

/*
 * Store Vterm attributes in "cell" from highlight flags.
 */
    స్థిర శూన్య
hl2vtermAttr(పూర్ణాంకం attr, cellattr_T *cell)
{
    CLEAR_క్షేత్రం(cell->attrs);
    ఒకవేళ (attr & HL_BOLD)
	cell->attrs.bold = 1;
    ఒకవేళ (attr & HL_UNDERLINE)
	cell->attrs.underline = 1;
    ఒకవేళ (attr & HL_ITALIC)
	cell->attrs.italic = 1;
    ఒకవేళ (attr & HL_STRIKETHROUGH)
	cell->attrs.strike = 1;
    ఒకవేళ (attr & HL_INVERSE)
	cell->attrs.తిరగేయి = 1;
}

/*
 * Convert the attributes of a vterm cell into an attribute index.
 */
    స్థిర పూర్ణాంకం
cell2attr(
	పదం_T			*పదం,
	win_T			*wp,
	VTermScreenCellAttrs	*cellattrs,
	VTermColor		*cellfg,
	VTermColor		*cellbg)
{
    పూర్ణాంకం attr = vtermAttr2hl(cellattrs);
    VTermColor *fg = cellfg;
    VTermColor *bg = cellbg;
    పూర్ణాంకం is_default_fg = VTERM_COLOR_IS_DEFAULT_FG(fg);
    పూర్ణాంకం is_default_bg = VTERM_COLOR_IS_DEFAULT_BG(bg);

    ఒకవేళ (is_default_fg || is_default_bg)
    {
	ఒకవేళ (wp != ఖాళీ && wp->w_hlfwin_id != 0)
	{
	    ఒకవేళ (is_default_fg)
		fg = &wp->w_పదం_hlfwin.fg;
	    ఒకవేళ (is_default_bg)
		bg = &wp->w_పదం_hlfwin.bg;
	}
	లేకపోతే
	{
	    ఒకవేళ (is_default_fg)
		fg = &పదం->tl_default_color.fg;
	    ఒకవేళ (is_default_bg)
		bg = &పదం->tl_default_color.bg;
	}
    }

#నిర్వచితమైతే FEAT_GUI
    ఒకవేళ (gui.లోపల_use)
    {
	guicolor_T guifg = gui_mch_get_rgb_color(fg->red, fg->green, fg->blue);
	guicolor_T guibg = gui_mch_get_rgb_color(bg->red, bg->green, bg->blue);
	తిరిగి get_gui_attr_idx(attr, guifg, guibg);
    }
    లేకపోతే
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_TERMGUICOLORS
    ఒకవేళ (p_tgc)
    {
	guicolor_T tgcfg = VTERM_COLOR_IS_INVALID(fg)
	    ? INVALCOLOR
	    : gui_get_rgb_color_cmn(fg->red, fg->green, fg->blue);
	guicolor_T tgcbg = VTERM_COLOR_IS_INVALID(bg)
	    ? INVALCOLOR
	    : gui_get_rgb_color_cmn(bg->red, bg->green, bg->blue);
	తిరిగి get_tgc_attr_idx(attr, tgcfg, tgcbg);
    }
    లేకపోతే
#అయితే_ముగింపు
    {
	పూర్ణాంకం bold = MAYBE;
	పూర్ణాంకం ctermfg = color2index(fg, TRUE, &bold);
	పూర్ణాంకం ctermbg = color2index(bg, FALSE, &bold);

	// with 8 colors set the bold attribute to get a bright foreground
	ఒకవేళ (bold == TRUE)
	    attr |= HL_BOLD;

	తిరిగి get_cterm_attr_idx(attr, ctermfg, ctermbg);
    }
    తిరిగి 0;
}

    స్థిర శూన్య
సమితి_dirty_snapshot(పదం_T *పదం)
{
    పదం->tl_dirty_snapshot = TRUE;
#నిర్వచితమైతే FEAT_TIMERS
    ఒకవేళ (!పదం->tl_సాధారణ_బహుళకం)
    {
	// Update the snapshot after 100 msec of not getting updates.
	profile_setlimit(100L, &పదం->tl_timer_due);
	పదం->tl_timer_సమితి = TRUE;
    }
#అయితే_ముగింపు
}

    స్థిర పూర్ణాంకం
handle_damage(VTermRect rect, శూన్య *user)
{
    పదం_T *పదం = (పదం_T *)user;

    పదం->tl_dirty_అడ్డువరుస_ప్రారంభం = MIN(పదం->tl_dirty_అడ్డువరుస_ప్రారంభం, rect.ప్రారంభం_అడ్డువరుస);
    పదం->tl_dirty_అడ్డువరుస_చివర = MAX(పదం->tl_dirty_అడ్డువరుస_చివర, rect.చివర_అడ్డువరుస);
    సమితి_dirty_snapshot(పదం);
    redraw_buf_later(పదం->tl_బఫరు, UPD_SOME_VALID);
    తిరిగి 1;
}

    స్థిర శూన్య
పదం_scroll_పైకి(పదం_T *పదం, పూర్ణాంకం ప్రారంభం_అడ్డువరుస, పూర్ణాంకం లెక్క)
{
    win_T		 *wp = ఖాళీ;
    పూర్ణాంకం			 did_curwin = FALSE;
    VTermColor		 fg, bg;
    VTermScreenCellAttrs attr;
    పూర్ణాంకం			 clear_attr;

    CLEAR_క్షేత్రం(attr);

    అయితే (for_all_windows_and_curwin(&wp, &did_curwin))
    {
	ఒకవేళ (wp->w_బఫరు == పదం->tl_బఫరు)
	{
	    // Set the color to clear lines with.
	    vterm_state_get_default_colors(vterm_obtain_state(పదం->tl_vterm),
								     &fg, &bg);
	    clear_attr = cell2attr(పదం, wp, &attr, &fg, &bg);
	    win_del_పంక్తులు(wp, ప్రారంభం_అడ్డువరుస, లెక్క, FALSE, FALSE, clear_attr);
	}
    }
}

    స్థిర పూర్ణాంకం
handle_moverect(VTermRect dest, VTermRect src, శూన్య *user)
{
    పదం_T	*పదం = (పదం_T *)user;
    పూర్ణాంకం		లెక్క = src.ప్రారంభం_అడ్డువరుస - dest.ప్రారంభం_అడ్డువరుస;

    // Scrolling up is done much more efficiently by deleting lines instead of
    // redrawing the text. But avoid doing this multiple times, postpone until
    // the redraw happens.
    ఒకవేళ (dest.ప్రారంభం_నిలువు == src.ప్రారంభం_నిలువు
	    && dest.చివర_నిలువు == src.చివర_నిలువు
	    && dest.ప్రారంభం_అడ్డువరుస < src.ప్రారంభం_అడ్డువరుస)
    {
	ఒకవేళ (dest.ప్రారంభం_అడ్డువరుస == 0)
	    పదం->tl_postponed_scroll += లెక్క;
	లేకపోతే
	    పదం_scroll_పైకి(పదం, dest.ప్రారంభం_అడ్డువరుస, లెక్క);
    }

    పదం->tl_dirty_అడ్డువరుస_ప్రారంభం = MIN(పదం->tl_dirty_అడ్డువరుస_ప్రారంభం, dest.ప్రారంభం_అడ్డువరుస);
    పదం->tl_dirty_అడ్డువరుస_చివర = MIN(పదం->tl_dirty_అడ్డువరుస_చివర, dest.చివర_అడ్డువరుస);
    సమితి_dirty_snapshot(పదం);

    // Note sure if the scrolling will work correctly, let's do a complete
    // redraw later.
    redraw_buf_later(పదం->tl_బఫరు, UPD_NOT_VALID);
    తిరిగి 1;
}

    స్థిర పూర్ణాంకం
handle_movecursor(
	VTermPos pos,
	VTermPos oldpos వాడని,
	పూర్ణాంకం visible,
	శూన్య *user)
{
    పదం_T	*పదం = (పదం_T *)user;
    win_T	*wp = ఖాళీ;
    పూర్ణాంకం		did_curwin = FALSE;

    పదం->tl_cursor_pos = pos;
    పదం->tl_cursor_visible = visible;

    అయితే (for_all_windows_and_curwin(&wp, &did_curwin))
    {
	ఒకవేళ (wp->w_బఫరు == పదం->tl_బఫరు)
	    position_cursor(wp, &pos);
    }
    ఒకవేళ (పదం->tl_బఫరు == curbuf && !పదం->tl_సాధారణ_బహుళకం)
	// Don't redraw here, it will be done after
	// vterm_input_write() is finished.
	update_cursor(పదం, FALSE);

    తిరిగి 1;
}

    స్థిర పూర్ణాంకం
handle_settermprop(
	VTermProp prop,
	VTerm_విలువ *విలువ,
	శూన్య *user)
{
    పదం_T	*పదం = (పదం_T *)user;
    char_u	*strval = ఖాళీ;

    ఎంపిక (prop)
    {
	స్థితి VTERM_PROP_TITLE:
	    ఒకవేళ (disable_vterm_title_for_testing)
		ఆపు;
	    strval = vim_strnsave((char_u *)విలువ->string.str,
							    విలువ->string.len);
	    ఒకవేళ (strval == ఖాళీ)
		ఆపు;
	    vim_free(పదం->tl_title);
	    // a blank title isn't useful, make it empty, so that "running" is
	    // displayed
	    ఒకవేళ (*skipwhite(strval) == NUL)
		పదం->tl_title = ఖాళీ;
	    // Same as blank
	    లేకపోతే ఒకవేళ (పదం->tl_arg0_cmd != ఖాళీ
		    && STRNCMP(పదం->tl_arg0_cmd, strval,
					  (పూర్ణాంకం)STRLEN(పదం->tl_arg0_cmd)) == 0)
		పదం->tl_title = ఖాళీ;
	    // Empty corrupted data of winpty
	    లేకపోతే ఒకవేళ (STRNCMP("  - ", strval, 4) == 0)
		పదం->tl_title = ఖాళీ;
#నిర్వచితమైతే MSWIN
	    లేకపోతే ఒకవేళ (!enc_utf8 && enc_codepage > 0)
	    {
		WCHAR   *ret = ఖాళీ;
		పూర్ణాంకం	నిడివి = 0;

		Multi_Byte_గమ్యం_Wide_Char_alloc(CP_UTF8, 0,
			(అక్షరం*)విలువ->string.str,
					(పూర్ణాంకం)విలువ->string.len, &ret, &నిడివి);
		ఒకవేళ (ret != ఖాళీ)
		{
		    Wide_Char_గమ్యం_Multi_Byte_alloc(enc_codepage, 0,
					ret, నిడివి, (అక్షరం**)&పదం->tl_title,
					&నిడివి, 0, 0);
		    vim_free(ret);
		}
	    }
#అయితే_ముగింపు
	    లేకపోతే
	    {
		పదం->tl_title = strval;
		strval = ఖాళీ;
	    }
	    VIM_CLEAR(పదం->tl_స్థితి_పాఠ్యం);
	    ఒకవేళ (పదం == curbuf->b_పదం)
	    {
		maketitle();
		curwin->w_redr_స్థితి = true;
	    }
	    ఆపు;

	స్థితి VTERM_PROP_CURSORVISIBLE:
	    పదం->tl_cursor_visible = విలువ->boolean;
	    may_toggle_cursor(పదం);
	    బహిర్గతం_flush();
	    ఆపు;

	స్థితి VTERM_PROP_CURSORBLINK:
	    పదం->tl_cursor_blink = విలువ->boolean;
	    may_సమితి_cursor_props(పదం);
	    ఆపు;

	స్థితి VTERM_PROP_CURSORSHAPE:
	    పదం->tl_cursor_shape = విలువ->సంఖ్య;
	    may_సమితి_cursor_props(పదం);
	    ఆపు;

	స్థితి VTERM_PROP_CURSORCOLOR:
	    strval = vim_strnsave((char_u *)విలువ->string.str,
							    విలువ->string.len);
	    ఒకవేళ (strval == ఖాళీ)
		ఆపు;
	    cursor_color_copy(&పదం->tl_cursor_color, strval);
	    may_సమితి_cursor_props(పదం);
	    ఆపు;

	స్థితి VTERM_PROP_ALTSCREEN:
	    // TODO: do anything else?
	    పదం->tl_using_altscreen = విలువ->boolean;
	    ఆపు;

	అప్రమేయ:
	    ఆపు;
    }
    vim_free(strval);

    // Always return 1, otherwise vterm doesn't store the value internally.
    తిరిగి 1;
}

/*
 * The job running in the terminal resized the terminal.
 */
    స్థిర పూర్ణాంకం
handle_resize(పూర్ణాంకం అడ్డువరుసలు_2, పూర్ణాంకం నిలువుల_సంఖ్య, శూన్య *user)
{
    పదం_T	*పదం = (పదం_T *)user;
    win_T	*wp;

    పదం->tl_అడ్డువరుసలు = అడ్డువరుసలు_2;
    పదం->tl_నిలువుల_సంఖ్య = నిలువుల_సంఖ్య;
    ఒకవేళ (పదం->tl_vterm_కొలత_మారింది)
	// Size was set by vterm_set_size(), don't set the window size.
	పదం->tl_vterm_కొలత_మారింది = FALSE;
    లేకపోతే
    {
	FOR_ALL_WINDOWS(wp)
	{
	    ఒకవేళ (wp->w_బఫరు == పదం->tl_బఫరు)
	    {
		win_setheight_win(అడ్డువరుసలు_2, wp);
		win_setwidth_win(నిలువుల_సంఖ్య, wp);
	    }
	}
	redraw_buf_later(పదం->tl_బఫరు, UPD_NOT_VALID);
    }
    తిరిగి 1;
}

/*
 * If the number of lines that are stored goes over 'termwinscroll' then
 * delete the first 10%.
 * "gap" points to tl_scrollback or tl_scrollback_postponed.
 * "update_buffer" is TRUE when the buffer should be updated.
 */
    స్థిర శూన్య
హద్దు_scrollback(పదం_T *పదం, garray_T *gap, పూర్ణాంకం update_బఫరు)
{
    ఒకవేళ (gap->ga_len < పదం->tl_బఫరు->b_p_twsl)
	తిరిగి;

    పూర్ణాంకం	todo = MAX(పదం->tl_బఫరు->b_p_twsl / 10,
				     gap->ga_len - పదం->tl_బఫరు->b_p_twsl);
    పూర్ణాంకం	i;
    sb_పంక్తి_T *sb_పంక్తులు = (sb_పంక్తి_T *)gap->ga_దత్తాంశం;

    curbuf = పదం->tl_బఫరు;
    కొరకు (i = 0; i < todo; ++i)
    {
	ఒకవేళ (update_బఫరు && (!sb_పంక్తులు[i].continuation || !i))
	{
	    ml_delete(1);
	    --పదం->tl_బఫరు_scrolled;
	}
	vim_free(sb_పంక్తులు[i].sb_cells);
    }
    // Continue until end of wrapped line
    కొరకు (; todo < gap->ga_len && sb_పంక్తులు[todo].continuation; ++todo)
	vim_free(sb_పంక్తులు[todo].sb_cells);
    curbuf = curwin->w_బఫరు;

    gap->ga_len -= todo;
    mch_memmove(gap->ga_దత్తాంశం,
		(sb_పంక్తి_T *)gap->ga_దత్తాంశం + todo,
		పరిమాణం(sb_పంక్తి_T) * gap->ga_len);
    ఒకవేళ (update_బఫరు)
    {
	win_T *curwin_save = curwin;
	win_T *wp = ఖాళీ;

	పదం->tl_scrollback_scrolled -= todo;

	FOR_ALL_WINDOWS(wp)
	{
	    ఒకవేళ (wp->w_బఫరు == పదం->tl_బఫరు)
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
    స్థిర పూర్ణాంకం
handle_pushline(పూర్ణాంకం నిలువుల_సంఖ్య, స్థిరరాశి VTermScreenCell *cells, పూర్ణాంకం continuation, శూన్య *user)
{
    పదం_T	*పదం = (పదం_T *)user;
    garray_T	*gap;
    పూర్ణాంకం		update_బఫరు;

    ఒకవేళ (పదం->tl_సాధారణ_బహుళకం)
    {
	// In Terminal-Normal mode the user interacts with the buffer, thus we
	// must not change it. Postpone adding the scrollback lines.
	gap = &పదం->tl_scrollback_postponed;
	update_బఫరు = FALSE;
    }
    లేకపోతే
    {
	// First remove the lines that were appended before, the pushed line
	// goes above it.
	cleanup_scrollback(పదం);
	gap = &పదం->tl_scrollback;
	update_బఫరు = TRUE;
    }

    హద్దు_scrollback(పదం, gap, update_బఫరు);

    ఒకవేళ (ga_grow(gap, 1) == FAIL)
	తిరిగి 0;

    cellattr_T	*p = ఖాళీ;
    పూర్ణాంకం		len = 0;
    పూర్ణాంకం		i;
    పూర్ణాంకం		c;
    పూర్ణాంకం		నిలువు;
    పూర్ణాంకం		పాఠ్యం_len;
    char_u		*పాఠ్యం;
    sb_పంక్తి_T	*పంక్తి;
    garray_T	ga;
    cellattr_T	fill_attr = పదం->tl_default_color;

    // do not store empty cells at the end
    కొరకు (i = 0; i < నిలువుల_సంఖ్య; ++i)
	ఒకవేళ (cells[i].chars[0] != 0)
	    len = i + 1;
	లేకపోతే
	    cell2cellattr(&cells[i], &fill_attr);

    ga_init2(&ga, 1, 100);
    ఒకవేళ (len > 0)
	p = ALLOC_MULT(cellattr_T, len);
    ఒకవేళ (p != ఖాళీ)
    {
	కొరకు (నిలువు = 0; నిలువు < len; నిలువు += cells[నిలువు].వెడల్పు)
	{
	    ఒకవేళ (ga_grow(&ga, VTERM_MAX_CHARS_చొప్పున_CELL * 4) == FAIL)
	    {
		ga.ga_len = 0;
		ఆపు;
	    }
	    కొరకు (i = 0; i < VTERM_MAX_CHARS_చొప్పున_CELL &&
		    ((c = cells[నిలువు].chars[i]) > 0 || i == 0); ++i)
		ga.ga_len += utf_char2bytes(c == NUL ? ' ' : c,
			(char_u *)ga.ga_దత్తాంశం + ga.ga_len);
	    cell2cellattr(&cells[నిలువు], &p[నిలువు]);
	}
    }
    ఒకవేళ (ga_grow(&ga, 1) == FAIL)
    {
	ఒకవేళ (update_బఫరు)
	    పాఠ్యం = (char_u *)"";
	లేకపోతే
	    పాఠ్యం = vim_strsave((char_u *)"");
	పాఠ్యం_len = 0;
    }
    లేకపోతే
    {
	పాఠ్యం = ga.ga_దత్తాంశం;
	పాఠ్యం_len = ga.ga_len;
	*(పాఠ్యం + పాఠ్యం_len) = NUL;
    }
    ఒకవేళ (update_బఫరు)
	కూడు_scrollback_పంక్తి_గమ్యం_బఫరు(పదం, పాఠ్యం, పాఠ్యం_len, continuation);

    పంక్తి = (sb_పంక్తి_T *)gap->ga_దత్తాంశం + gap->ga_len;
    పంక్తి->sb_నిలువుల_సంఖ్య = len;
    పంక్తి->sb_బైట్లు = పాఠ్యం_len;
    పంక్తి->sb_cells = p;
    పంక్తి->sb_fill_attr = fill_attr;
    పంక్తి->continuation = (char_u)continuation;
    ఒకవేళ (update_బఫరు)
    {
	పంక్తి->sb_పాఠ్యం = ఖాళీ;
	++పదం->tl_scrollback_scrolled;
	ఒకవేళ (!continuation)
	    ++పదం->tl_బఫరు_scrolled;
	ga_clear(&ga);  // free the text
    }
    లేకపోతే
    {
	పంక్తి->sb_పాఠ్యం = పాఠ్యం;
	ga_init(&ga);  // text is kept in tl_scrollback_postponed
    }
    ++gap->ga_len;
    తిరిగి 0; // ignored
}

/*
 * Called when leaving Terminal-Normal mode: deal with any scrollback that was
 * received and stored in tl_scrollback_postponed.
 */
    స్థిర శూన్య
handle_postponed_scrollback(పదం_T *పదం)
{
    పూర్ణాంకం i;

    ఒకవేళ (పదం->tl_scrollback_postponed.ga_len == 0)
	తిరిగి;
    ch_log(ఖాళీ, "Moving postponed scrollback to scrollback");

    // First remove the lines that were appended before, the pushed lines go
    // above it.
    cleanup_scrollback(పదం);

    కొరకు (i = 0; i < పదం->tl_scrollback_postponed.ga_len; ++i)
    {
	char_u		*పాఠ్యం;
	sb_పంక్తి_T	*pp_పంక్తి;
	sb_పంక్తి_T	*పంక్తి;

	ఒకవేళ (ga_grow(&పదం->tl_scrollback, 1) == FAIL)
	    ఆపు;
	pp_పంక్తి = (sb_పంక్తి_T *)పదం->tl_scrollback_postponed.ga_దత్తాంశం + i;

	పాఠ్యం = pp_పంక్తి->sb_పాఠ్యం;
	ఒకవేళ (పాఠ్యం == ఖాళీ)
	    పాఠ్యం = (char_u *)"";
	కూడు_scrollback_పంక్తి_గమ్యం_బఫరు(పదం, పాఠ్యం, (పూర్ణాంకం)STRLEN(పాఠ్యం), pp_పంక్తి->continuation);
	vim_free(pp_పంక్తి->sb_పాఠ్యం);

	పంక్తి = (sb_పంక్తి_T *)పదం->tl_scrollback.ga_దత్తాంశం
						 + పదం->tl_scrollback.ga_len;
	పంక్తి->sb_నిలువుల_సంఖ్య = pp_పంక్తి->sb_నిలువుల_సంఖ్య;
	పంక్తి->sb_బైట్లు = pp_పంక్తి->sb_బైట్లు;
	పంక్తి->sb_cells = pp_పంక్తి->sb_cells;
	పంక్తి->sb_fill_attr = pp_పంక్తి->sb_fill_attr;
	పంక్తి->sb_పాఠ్యం = ఖాళీ;
	++పదం->tl_scrollback_scrolled;
	++పదం->tl_scrollback.ga_len;
	ఒకవేళ (!pp_పంక్తి->continuation)
	    ++పదం->tl_బఫరు_scrolled;
    }

    ga_clear(&పదం->tl_scrollback_postponed);
    హద్దు_scrollback(పదం, &పదం->tl_scrollback, TRUE);
}

/*
 * Called when the terminal wants to ring the system bell.
 */
    స్థిర పూర్ణాంకం
handle_bell(శూన్య *user వాడని)
{
    vim_beep(BO_పదం);
    తిరిగి 0;
}

స్థిర VTermScreenCallbacks screen_callbacks = {
    handle_damage,		// damage
    handle_moverect,		// moverect
    handle_movecursor,		// movecursor
    handle_settermprop,		// settermprop
    handle_bell,		// bell
    handle_resize,		// resize
    ఖాళీ,			// sb_pushline
    ఖాళీ,			// sb_popline
    ఖాళీ,			// sb_clear
    handle_pushline	// sb_pushline4
};

/*
 * Do the work after the channel of a terminal was closed.
 * Must be called only when updating_screen is FALSE.
 * Returns TRUE when a buffer was closed (list of terminals may have changed).
 */
    స్థిర పూర్ణాంకం
పదం_after_channel_closed(పదం_T *పదం)
{
    // Unless in Terminal-Normal mode: clear the vterm.
    ఒకవేళ (!పదం->tl_సాధారణ_బహుళకం)
    {
	పూర్ణాంకం	fnum = పదం->tl_బఫరు->b_fnum;

	cleanup_vterm(పదం);

	ఒకవేళ (పదం->tl_ముగింపు == TL_ముగింపు_CLOSE)
	{
	    aco_save_T	aco;
	    పూర్ణాంకం		do_సమితి_w_locked = పదం->tl_బఫరు->b_nwindows == 0;
#నిర్వచితమైతే FEAT_PROP_POPUP
	    win_T	*pwin = ఖాళీ;

	    // If this was a terminal in a popup window, go back to the
	    // previous window.
	    ఒకవేళ (popup_is_popup(curwin) && curbuf == పదం->tl_బఫరు)
	    {
		pwin = curwin;
		ఒకవేళ (win_valid(prevwin))
		    win_enter(prevwin, FALSE);
	    }
	    లేకపోతే
#అయితే_ముగింపు
	    // If this is the last normal window: exit Vim.
	    ఒకవేళ (పదం->tl_బఫరు->b_nwindows > 0 && only_one_కిటికీ())
	    {
		exarg_T ea;

		CLEAR_క్షేత్రం(ea);
		ex_quit(&ea);
		తిరిగి TRUE;
	    }

	    // ++close or term_finish == "close"
	    ch_log(ఖాళీ, "terminal job finished, closing window");
	    aucmd_prepbuf(&aco, పదం->tl_బఫరు);
	    ఒకవేళ (curbuf == పదం->tl_బఫరు)
	    {
		win_T	*wp = curwin;

		// Avoid closing the window if we temporarily use it.
		ఒకవేళ (is_aucmd_win(wp))
		    do_సమితి_w_locked = TRUE;
		ఒకవేళ (do_సమితి_w_locked)
		    ++wp->w_locked;
		do_bufdel(DOBUF_WIPE, (char_u *)"", 1, fnum, fnum, FALSE);
		ఒకవేళ (do_సమితి_w_locked)
		    --wp->w_locked;
		aucmd_restbuf(&aco);
	    }
#నిర్వచితమైతే FEAT_PROP_POPUP
	    ఒకవేళ (pwin != ఖాళీ)
		popup_close_with_retval(pwin, 0);
#అయితే_ముగింపు
	    తిరిగి TRUE;
	}
	ఒకవేళ (పదం->tl_ముగింపు == TL_ముగింపు_OPEN
				   && పదం->tl_బఫరు->b_nwindows == 0)
	{
	    అక్షరం    *cmd = పదం->tl_opencmd == ఖాళీ
				? "botright sbuf %d"
				: (అక్షరం *)పదం->tl_opencmd;
	    size_t  len = అక్షరశ్రేణి_పొడవు(cmd) + 50;
	    అక్షరం    *buf = alloc(len);

	    ఒకవేళ (buf != ఖాళీ)
	    {
		ch_log(ఖాళీ, "terminal job finished, opening window");
		vim_snprintf(buf, len, cmd, fnum);
		do_cmdline_cmd((char_u *)buf);
		vim_free(buf);
	    }
	}
	లేకపోతే
	    ch_log(ఖాళీ, "terminal job finished");
    }

    redraw_buf_and_స్థితి_later(పదం->tl_బఫరు, UPD_NOT_VALID);
    తిరిగి FALSE;
}

#ఒకవేళ defined(FEAT_PROP_POPUP)
/*
 * If the current window is a terminal in a popup window and the job has
 * finished, close the popup window and to back to the previous window.
 * Otherwise return FAIL.
 */
    పూర్ణాంకం
may_close_పదం_popup(శూన్య)
{
    ఒకవేళ (!popup_is_popup(curwin) || curbuf->b_పదం == ఖాళీ
				 || పదం_పని_running_not_none(curbuf->b_పదం))
	తిరిగి FAIL;

    win_T *pwin = curwin;

    ఒకవేళ (win_valid(prevwin))
	win_enter(prevwin, FALSE);
    popup_close_with_retval(pwin, 0);
    తిరిగి OK;
}
#అయితే_ముగింపు

/*
 * Called when a channel is going to be closed, before invoking the close
 * callback.
 */
    శూన్య
పదం_channel_closing(channel_T *ch)
{
    పదం_T *పదం;

    కొరకు (పదం = మొదటి_పదం; పదం != ఖాళీ; పదం = పదం->tl_తదుపరి)
	ఒకవేళ (పదం->tl_పని == ch->ch_పని && !పదం->tl_channel_closed)
	    పదం->tl_channel_closing = TRUE;
}

/*
 * Called when a channel has been closed.
 * If this was a channel for a terminal window then finish it up.
 */
    శూన్య
పదం_channel_closed(channel_T *ch)
{
    పదం_T *పదం;
    పదం_T *తదుపరి_పదం;
    పూర్ణాంకం	    did_one = FALSE;

    కొరకు (పదం = మొదటి_పదం; పదం != ఖాళీ; పదం = తదుపరి_పదం)
    {
	తదుపరి_పదం = పదం->tl_తదుపరి;
	ఒకవేళ (పదం->tl_పని == ch->ch_పని && !పదం->tl_channel_closed)
	{
	    పదం->tl_channel_closed = TRUE;
	    did_one = TRUE;

	    VIM_CLEAR(పదం->tl_title);
	    VIM_CLEAR(పదం->tl_స్థితి_పాఠ్యం);
#నిర్వచితమైతే MSWIN
	    ఒకవేళ (పదం->tl_బహిర్గతం_fd != ఖాళీ)
	    {
		fclose(పదం->tl_బహిర్గతం_fd);
		పదం->tl_బహిర్గతం_fd = ఖాళీ;
	    }
#అయితే_ముగింపు

	    ఒకవేళ (updating_screen)
	    {
		// Cannot open or close windows now.  Can happen when
		// 'lazyredraw' is set.
		పదం->tl_channel_recently_closed = TRUE;
		కొనసాగు;
	    }

	    ఒకవేళ (పదం_after_channel_closed(పదం))
		తదుపరి_పదం = మొదటి_పదం;
	}
    }

    ఒకవేళ (did_one)
    {
	redraw_statuslines();

	// Need to break out of vgetc().
	ins_char_typebuf(K_IGNORE, 0);
	typebuf_was_filled = TRUE;

	పదం = curbuf->b_పదం;
	ఒకవేళ (పదం != ఖాళీ)
	{
	    ఒకవేళ (పదం->tl_పని == ch->ch_పని)
		maketitle();
	    update_cursor(పదం, పదం->tl_cursor_visible);
	}
    }
}

/*
 * To be called after resetting updating_screen: handle any terminal where the
 * channel was closed.
 */
    శూన్య
పదం_check_channel_closed_recently(శూన్య)
{
    పదం_T *పదం;
    పదం_T *తదుపరి_పదం;

    కొరకు (పదం = మొదటి_పదం; పదం != ఖాళీ; పదం = తదుపరి_పదం)
    {
	తదుపరి_పదం = పదం->tl_తదుపరి;
	ఒకవేళ (పదం->tl_channel_recently_closed)
	{
	    పదం->tl_channel_recently_closed = FALSE;
	    ఒకవేళ (పదం_after_channel_closed(పదం))
		// start over, the list may have changed
		తదుపరి_పదం = మొదటి_పదం;
	}
    }
}

/*
 * Fill one screen line from a line of the terminal.
 * Advances "pos" to past the last column.
 */
    స్థిర శూన్య
పదం_line2screenline(
	పదం_T		*పదం,
	win_T		*wp,
	VTermScreen	*screen,
	VTermPos	*pos,
	పూర్ణాంకం		max_నిలువు)
{
    పూర్ణాంకం off = screen_get_current_పంక్తి_off();

    కొరకు (pos->నిలువు = 0; pos->నిలువు < max_నిలువు; )
    {
	VTermScreenCell cell;
	పూర్ణాంకం		c;

	ఒకవేళ (vterm_screen_get_cell(screen, *pos, &cell) == 0)
	    CLEAR_క్షేత్రం(cell);

	c = cell.chars[0];
	ఒకవేళ (c == NUL)
	{
	    Screen_పంక్తులు[off] = ' ';
	    ఒకవేళ (enc_utf8)
		Screen_పంక్తులు_UC[off] = NUL;
	}
	లేకపోతే
	{
	    ఒకవేళ (enc_utf8)
	    {
		పూర్ణాంకం i;

		// composing chars
		కొరకు (i = 0; i < Screen_mco
			      && i + 1 < VTERM_MAX_CHARS_చొప్పున_CELL; ++i)
		{
		    Screen_పంక్తులు_C[i][off] = cell.chars[i + 1];
		    ఒకవేళ (cell.chars[i + 1] == 0)
			ఆపు;
		}
		ఒకవేళ (c >= 0x80 || (Screen_mco > 0
					 && Screen_పంక్తులు_C[0][off] != 0))
		{
		    Screen_పంక్తులు[off] = ' ';
		    Screen_పంక్తులు_UC[off] = c;
		}
		లేకపోతే
		{
		    Screen_పంక్తులు[off] = c;
		    Screen_పంక్తులు_UC[off] = NUL;
		}
	    }
#నిర్వచితమైతే MSWIN
	    లేకపోతే ఒకవేళ (has_mbyte && c >= 0x80)
	    {
		char_u	mb[MB_MAXBYTES+1];
		WCHAR	wc = c;

		ఒకవేళ (Wide_Char_గమ్యం_Multi_Byte(GetACP(), 0, &wc, 1,
					       (అక్షరం*)mb, 2, 0, 0) > 1)
		{
		    Screen_పంక్తులు[off] = mb[0];
		    Screen_పంక్తులు[off + 1] = mb[1];
		    cell.వెడల్పు = mb_ptr2cells(mb);
		}
		లేకపోతే
		    Screen_పంక్తులు[off] = c;
	    }
#అయితే_ముగింపు
	    లేకపోతే
		// This will only store the lower byte of "c".
		Screen_పంక్తులు[off] = c;
	}
	ScreenAttrs[off] = cell2attr(పదం, wp, &cell.attrs, &cell.fg,
								     &cell.bg);

	++pos->నిలువు;
	++off;
	ఒకవేళ (cell.వెడల్పు == 2)
	{
	    // don't set the second byte to NUL for a DBCS encoding, it
	    // has been set above
	    ఒకవేళ (enc_utf8)
	    {
		Screen_పంక్తులు_UC[off] = NUL;
		Screen_పంక్తులు[off] = NUL;
	    }
	    లేకపోతే ఒకవేళ (!has_mbyte)
	    {
		// Can't show a double-width character with a single-byte
		// 'encoding', just use a space.
		Screen_పంక్తులు[off] = ' ';
		ScreenAttrs[off] = ScreenAttrs[off - 1];
	    }

	    ++pos->నిలువు;
	    ++off;
	}
    }
}

#ఒకవేళ defined(FEAT_GUI)
    స్థిర శూన్య
update_system_పదం(పదం_T *పదం)
{
    VTermPos	    pos;
    VTermScreen	    *screen;

    ఒకవేళ (పదం->tl_vterm == ఖాళీ)
	తిరిగి;
    screen = vterm_obtain_screen(పదం->tl_vterm);

    // Scroll up to make more room for terminal lines if needed.
    // Use the cursor position to determine how much to scroll, because
    // ConPTY may damage all rows on initialization even when most are
    // empty, which would cause unnecessary scrolling.
    అయితే (పదం->tl_toprow > 0
		  && (అడ్డువరుసలు - పదం->tl_toprow) < పదం->tl_cursor_pos.అడ్డువరుస + 1)
    {
	పూర్ణాంకం save_p_మరిన్ని = p_మరిన్ని;

	p_మరిన్ని = FALSE;
	సందేశం_అడ్డువరుస = అడ్డువరుసలు - 1;
	సందేశం_puts("\n");
	p_మరిన్ని = save_p_మరిన్ని;
	--పదం->tl_toprow;
    }

    కొరకు (pos.అడ్డువరుస = పదం->tl_dirty_అడ్డువరుస_ప్రారంభం; pos.అడ్డువరుస < పదం->tl_dirty_అడ్డువరుస_చివర
						  && pos.అడ్డువరుస < అడ్డువరుసలు; ++pos.అడ్డువరుస)
    {
	ఒకవేళ (pos.అడ్డువరుస < పదం->tl_అడ్డువరుసలు)
	{
	    పూర్ణాంకం max_నిలువు = MIN(topframe->fr_వెడల్పు, పదం->tl_నిలువుల_సంఖ్య);

	    పదం_line2screenline(పదం, ఖాళీ, screen, &pos, max_నిలువు);
	}
	లేకపోతే
	    pos.నిలువు = 0;

	screen_పంక్తి(curwin, పదం->tl_toprow + pos.అడ్డువరుస,
		firstwin->w_wincol, pos.నిలువు, topframe->fr_వెడల్పు, -1, 0);
    }

    పదం->tl_dirty_అడ్డువరుస_ప్రారంభం = MAX_అడ్డువరుస;
    పదం->tl_dirty_అడ్డువరుస_చివర = 0;
}
#అయితే_ముగింపు

/*
 * Return TRUE if window "wp" is to be redrawn with term_update_window().
 * Returns FALSE when there is no terminal running in this window or it is in
 * Terminal-Normal mode.
 */
    పూర్ణాంకం
పదం_do_update_కిటికీ(win_T *wp)
{
    పదం_T	*పదం = wp->w_బఫరు->b_పదం;

    తిరిగి పదం != ఖాళీ && పదం->tl_vterm != ఖాళీ && !పదం->tl_సాధారణ_బహుళకం;
}

/*
 * Called to update a window that contains an active terminal.
 */
    శూన్య
పదం_update_కిటికీ(win_T *wp)
{
    పదం_T	*పదం = wp->w_బఫరు->b_పదం;
    VTerm	*vterm;
    VTermScreen *screen;
    VTermState	*state;
    VTermPos	pos;
    పూర్ణాంకం		అడ్డువరుసలు_2, నిలువుల_సంఖ్య;
    పూర్ణాంకం		newrows, newcols;
    పూర్ణాంకం		minsize;
    win_T	*twp;

    vterm = పదం->tl_vterm;
    screen = vterm_obtain_screen(vterm);
    state = vterm_obtain_state(vterm);

    // We use UPD_NOT_VALID on a resize or scroll, redraw everything then.
    // With UPD_SOME_VALID only redraw what was marked dirty.
    ఒకవేళ (wp->w_redr_type > UPD_SOME_VALID)
    {
	పదం->tl_dirty_అడ్డువరుస_ప్రారంభం = 0;
	పదం->tl_dirty_అడ్డువరుస_చివర = MAX_అడ్డువరుస;

	ఒకవేళ (పదం->tl_postponed_scroll > 0
			      && పదం->tl_postponed_scroll < పదం->tl_అడ్డువరుసలు / 3)
	    // Scrolling is usually faster than redrawing, when there are only
	    // a few lines to scroll.
	    పదం_scroll_పైకి(పదం, 0, పదం->tl_postponed_scroll);
	పదం->tl_postponed_scroll = 0;
    }

    /*
     * If the window was resized a redraw will be triggered and we get here.
     * Adjust the size of the vterm unless 'termwinsize' specifies a fixed size.
     */
    minsize = parse_termwinsize(wp, &అడ్డువరుసలు_2, &నిలువుల_సంఖ్య);

    newrows = 99999;
    newcols = 99999;
    కొరకు (twp = firstwin; ; twp = twp->w_తదుపరి)
    {
	// Always use curwin, it may be a popup window.
	win_T *wwp = twp == ఖాళీ ? curwin : twp;

	// When more than one window shows the same terminal, use the
	// smallest size.
	ఒకవేళ (wwp->w_బఫరు == పదం->tl_బఫరు)
	{
	    newrows = MIN(newrows, wwp->w_ఎత్తు);
	    newcols = MIN(newcols, wwp->w_వెడల్పు);
	}
	ఒకవేళ (twp == ఖాళీ)
	    ఆపు;
    }
    ఒకవేళ (newrows == 99999 || newcols == 99999)
	తిరిగి; // safety exit
    newrows = అడ్డువరుసలు_2 == 0 ? newrows : minsize ? MAX(అడ్డువరుసలు_2, newrows) : అడ్డువరుసలు_2;
    newcols = నిలువుల_సంఖ్య == 0 ? newcols : minsize ? MAX(నిలువుల_సంఖ్య, newcols) : నిలువుల_సంఖ్య;

    // If no cell is visible there is no point in resizing.  Also, vterm can't
    // handle a zero height.
    ఒకవేళ (newrows == 0 || newcols == 0)
	తిరిగి;

    ఒకవేళ (పదం->tl_అడ్డువరుసలు != newrows || పదం->tl_నిలువుల_సంఖ్య != newcols)
    {
	పదం->tl_vterm_కొలత_మారింది = TRUE;
	vterm_సమితి_కొలత(vterm, newrows, newcols);
	ch_log(పదం->tl_పని->jv_channel, "Resizing terminal to %d lines",
								      newrows);
	పదం_report_winsize(పదం, newrows, newcols);

	// Updating the terminal size will cause the snapshot to be cleared.
	// When not in terminal_loop() we need to restore it.
	ఒకవేళ (పదం != లోపల_terminal_loop)
	    may_move_terminal_గమ్యం_బఫరు(పదం, FALSE);
    }

    // The cursor may have been moved when resizing.
    vterm_state_get_cursorpos(state, &pos);
    position_cursor(wp, &pos);

    కొరకు (pos.అడ్డువరుస = పదం->tl_dirty_అడ్డువరుస_ప్రారంభం; pos.అడ్డువరుస < పదం->tl_dirty_అడ్డువరుస_చివర
					  && pos.అడ్డువరుస < wp->w_ఎత్తు; ++pos.అడ్డువరుస)
    {
	ఒకవేళ (pos.అడ్డువరుస < పదం->tl_అడ్డువరుసలు)
	{
	    పూర్ణాంకం max_నిలువు = MIN(wp->w_వెడల్పు, పదం->tl_నిలువుల_సంఖ్య);

	    పదం_line2screenline(పదం, wp, screen, &pos, max_నిలువు);
	}
	లేకపోతే
	    pos.నిలువు = 0;

	screen_పంక్తి(wp, wp->w_winrow + pos.అడ్డువరుస
#నిర్వచితమైతే FEAT_MENU
				+ winbar_ఎత్తు(wp)
#అయితే_ముగింపు
				, wp->w_wincol, pos.నిలువు, wp->w_వెడల్పు, -1,
#నిర్వచితమైతే FEAT_PROP_POPUP
				popup_is_popup(wp) ? SLF_POPUP :
#అయితే_ముగింపు
				0);
    }
}

/*
 * Called after updating all windows: may reset dirty rows.
 */
    శూన్య
పదం_did_update_కిటికీ(win_T *wp)
{
    పదం_T	*పదం = wp->w_బఫరు->b_పదం;

    ఒకవేళ (పదం == ఖాళీ || పదం->tl_vterm == ఖాళీ || పదం->tl_సాధారణ_బహుళకం
						|| wp->w_redr_type != 0)
	తిరిగి;

    పదం->tl_dirty_అడ్డువరుస_ప్రారంభం = MAX_అడ్డువరుస;
    పదం->tl_dirty_అడ్డువరుస_చివర = 0;
}

/*
 * Return TRUE if "wp" is a terminal window where the job has finished.
 */
    పూర్ణాంకం
పదం_is_ముగిసిన(buf_T *buf)
{
    తిరిగి buf->b_పదం != ఖాళీ && buf->b_పదం->tl_vterm == ఖాళీ;
}

/*
 * Return TRUE if "wp" is a terminal window where the job has finished or we
 * are in Terminal-Normal mode, thus we show the buffer contents.
 */
    పూర్ణాంకం
పదం_show_బఫరు(buf_T *buf)
{
    పదం_T *పదం = buf->b_పదం;

    తిరిగి పదం != ఖాళీ && (పదం->tl_vterm == ఖాళీ || పదం->tl_సాధారణ_బహుళకం);
}

/*
 * The current buffer is going to be changed.  If there is terminal
 * highlighting remove it now.
 */
    శూన్య
పదం_change_లోపల_curbuf(శూన్య)
{
    పదం_T *పదం = curbuf->b_పదం;

    ఒకవేళ (!పదం_is_ముగిసిన(curbuf) || పదం->tl_scrollback.ga_len <= 0)
	తిరిగి;

    free_scrollback(పదం);
    redraw_buf_later(పదం->tl_బఫరు, UPD_NOT_VALID);

    // The buffer is now like a normal buffer, it cannot be easily
    // abandoned when changed.
    సమితి_string_option_ప్రత్యక్ష((char_u *)"buftype", -1,
	    (char_u *)"", OPT_FREE|OPT_LOCAL, 0);
}

/*
 * Get the screen attribute for a position in the buffer.
 * Use a negative "col" to get the filler background color.
 */
    పూర్ణాంకం
పదం_get_attr(win_T *wp, linenr_T lnum, పూర్ణాంకం నిలువు)
{
    buf_T	*buf = wp->w_బఫరు;
    పదం_T	*పదం = buf->b_పదం;
    sb_పంక్తి_T	*పంక్తి;
    cellattr_T	*cellattr;
    పూర్ణాంకం         sb_పంక్తి = -1;
    పూర్ణాంకం         sb_నిలువు = నిలువు;

    ఒకవేళ (పదం->tl_scrollback.ga_len)
	bufline_pos_లోపల_scrollback(పదం, lnum, నిలువు, &sb_పంక్తి, &sb_నిలువు);

    ఒకవేళ (sb_పంక్తి < 0)
	cellattr = &పదం->tl_default_color;
    లేకపోతే
    {
	పంక్తి = (sb_పంక్తి_T *)పదం->tl_scrollback.ga_దత్తాంశం + sb_పంక్తి;
	ఒకవేళ (sb_నిలువు < 0 || sb_నిలువు >= పంక్తి->sb_నిలువుల_సంఖ్య)
	    cellattr = &పంక్తి->sb_fill_attr;
	లేకపోతే
	    cellattr = పంక్తి->sb_cells + sb_నిలువు;
    }
    తిరిగి cell2attr(పదం, wp, &cellattr->attrs, &cellattr->fg, &cellattr->bg);
}

/*
 * Convert a cterm color number 0 - 255 to RGB.
 * This is compatible with xterm.
 */
    స్థిర శూన్య
cterm_color2vterm(పూర్ణాంకం nr, VTermColor *rgb)
{
    cterm_color2rgb(nr, &rgb->red, &rgb->green, &rgb->blue, &rgb->సూచీ);
    ఒకవేళ (rgb->సూచీ == 0)
	rgb->type = VTERM_COLOR_RGB;
    లేకపోతే
    {
	rgb->type = VTERM_COLOR_సూచీకృత;
	--rgb->సూచీ;
    }
}

/*
 * Initialize vterm color from the synID.
 * Returns TRUE if color is set to "fg" and "bg".
 * Otherwise returns FALSE.
 */
    స్థిర పూర్ణాంకం
get_vterm_color_from_synid(పూర్ణాంకం id, VTermColor *fg, VTermColor *bg)
{
#ఒకవేళ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    // Use the actual color for the GUI and when 'termguicolors' is set.
    ఒకవేళ (0
# ifdef FEAT_GUI
	    || gui.లోపల_use
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

	ఒకవేళ (id > 0)
	    syn_id2colors(id, &fg_rgb, &bg_rgb);

	ఒకవేళ (fg_rgb != INVALCOLOR)
	{
	    long_u rgb = GUI_MCH_GET_RGB(fg_rgb);
	    fg->red = (గుర్తులేని)(rgb >> 16);
	    fg->green = (గుర్తులేని)(rgb >> 8) & 255;
	    fg->blue = (గుర్తులేని)rgb & 255;
	    fg->type = VTERM_COLOR_RGB | VTERM_COLOR_DEFAULT_FG;
	}
	లేకపోతే
	    fg->type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_FG;

	ఒకవేళ (bg_rgb != INVALCOLOR)
	{
	    long_u rgb = GUI_MCH_GET_RGB(bg_rgb);
	    bg->red = (గుర్తులేని)(rgb >> 16);
	    bg->green = (గుర్తులేని)(rgb >> 8) & 255;
	    bg->blue = (గుర్తులేని)rgb & 255;
	    bg->type = VTERM_COLOR_RGB | VTERM_COLOR_DEFAULT_BG;
	}
	లేకపోతే
	    bg->type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_BG;

	తిరిగి TRUE;
    }
    లేకపోతే
#అయితే_ముగింపు
    ఒకవేళ (t_colors >= 16)
    {
	పూర్ణాంకం cterm_fg = -1;
	పూర్ణాంకం cterm_bg = -1;

	ఒకవేళ (id > 0)
	    syn_id2cterm_bg(id, &cterm_fg, &cterm_bg);

	ఒకవేళ (cterm_fg >= 0)
	{
	    cterm_color2vterm(cterm_fg, fg);
	    fg->type |= VTERM_COLOR_DEFAULT_FG;
	}
	లేకపోతే
	    fg->type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_FG;

	ఒకవేళ (cterm_bg >= 0)
	{
	    cterm_color2vterm(cterm_bg, bg);
	    bg->type |= VTERM_COLOR_DEFAULT_BG;
	}
	లేకపోతే
	    bg->type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_BG;

	తిరిగి TRUE;
    }

    తిరిగి FALSE;
}

    శూన్య
పదం_reset_hlfwin(win_T *wp)
{
    wp->w_పదం_hlfwin.fg.type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_FG;
    wp->w_పదం_hlfwin.bg.type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_BG;
}

/*
 * Cache the color of HLF_WIN.
 */
    శూన్య
పదం_update_hlfwin(win_T *wp)
{
    పూర్ణాంకం id = wp->w_hlfwin_id;

    ఒకవేళ (id == 0 || !get_vterm_color_from_synid(id == -1 ? 0 : id,
		&wp->w_పదం_hlfwin.fg, &wp->w_పదం_hlfwin.bg))
	పదం_reset_hlfwin(wp);
}

/*
 * Called when option 'termguicolors' was set,
 * or when any highlight is changed.
 */
    శూన్య
పదం_update_hlfwin_all(శూన్య)
{
    win_T	 *wp = ఖాళీ;
    పూర్ణాంకం		 did_curwin = FALSE;

    అయితే (for_all_windows_and_curwin(&wp, &did_curwin))
	పదం_update_hlfwin(wp);
}

/*
 * Initialize term->tl_default_color from the environment.
 */
    శూన్య
పదం_init_default_colors(పదం_T *పదం)
{
    VTermColor	    *fg, *bg;
    పూర్ణాంకం		    fgval, bgval;
    పూర్ణాంకం		    id;

    CLEAR_క్షేత్రం(పదం->tl_default_color.attrs);
    పదం->tl_default_color.వెడల్పు = 1;
    fg = &పదం->tl_default_color.fg;
    bg = &పదం->tl_default_color.bg;

    // Vterm uses a default black background.  Set it to white when
    // 'background' is "light".
    ఒకవేళ (*p_bg == 'l')
    {
	fgval = 0;
	bgval = 255;
    }
    లేకపోతే
    {
	fgval = 255;
	bgval = 0;
    }
    fg->red = fg->green = fg->blue = fgval;
    bg->red = bg->green = bg->blue = bgval;
    fg->type = VTERM_COLOR_RGB | VTERM_COLOR_DEFAULT_FG;
    bg->type = VTERM_COLOR_RGB | VTERM_COLOR_DEFAULT_BG;

    // The highlight group overrules the defaults.
    id = పదం_get_highlight_id(పదం, ఖాళీ);

    ఒకవేళ (!get_vterm_color_from_synid(id, fg, bg))
    {
#ఒకవేళ defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
	పూర్ణాంకం tmp;
#అయితే_ముగింపు

	// In an MS-Windows console we know the normal colors.
	ఒకవేళ (cterm_సాధారణ_fg_color > 0)
	{
	    cterm_color2vterm(cterm_సాధారణ_fg_color - 1, fg);
#ఒకవేళ defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
# ifdef VIMDLL
	    ఒకవేళ (!gui.లోపల_use)
# endif
	    {
		tmp = fg->red;
		fg->red = fg->blue;
		fg->blue = tmp;
	    }
#అయితే_ముగింపు
	}
#నిర్వచితమైతే FEAT_TERMRESPONSE
	లేకపోతే
	    పదం_get_fg_color(&fg->red, &fg->green, &fg->blue);
#అయితే_ముగింపు

	ఒకవేళ (cterm_సాధారణ_bg_color > 0)
	{
	    cterm_color2vterm(cterm_సాధారణ_bg_color - 1, bg);
#ఒకవేళ defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
# ifdef VIMDLL
	    ఒకవేళ (!gui.లోపల_use)
# endif
	    {
		tmp = fg->red;
		fg->red = fg->blue;
		fg->blue = tmp;
	    }
#అయితే_ముగింపు
	}
#నిర్వచితమైతే FEAT_TERMRESPONSE
	లేకపోతే
	    పదం_get_bg_color(&bg->red, &bg->green, &bg->blue);
#అయితే_ముగింపు
    }
}

#ఒకవేళ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Return TRUE if the user-defined palette (either g:terminal_ansi_colors or the
 * "ansi_colors" argument in term_start()) shall be applied.
 */
    స్థిర పూర్ణాంకం
పదం_use_palette(శూన్య)
{
    ఒకవేళ (0
# ifdef FEAT_GUI
	    || gui.లోపల_use
# endif
# ifdef FEAT_TERMGUICOLORS
	    || p_tgc
# endif
       )
	తిరిగి TRUE;
    తిరిగి FALSE;
}

/*
 * Set the 16 ANSI colors from array of RGB values
 */
    స్థిర శూన్య
సమితి_vterm_palette(VTerm *vterm, long_u *rgb)
{
    పూర్ణాంకం		సూచీ = 0;
    VTermState	*state = vterm_obtain_state(vterm);

    కొరకు (; సూచీ < 16; సూచీ++)
    {
	VTermColor	color;

	color.type = VTERM_COLOR_RGB;
	color.red = (గుర్తులేని)(rgb[సూచీ] >> 16);
	color.green = (గుర్తులేని)(rgb[సూచీ] >> 8) & 255;
	color.blue = (గుర్తులేని)rgb[సూచీ] & 255;
	color.సూచీ = 0;
	vterm_state_సమితి_palette_color(state, సూచీ, &color);
    }
}

/*
 * Set the ANSI color palette from a list of colors
 */
    స్థిర పూర్ణాంకం
సమితి_ansi_colors_list(VTerm *vterm, list_T *list)
{
    పూర్ణాంకం		n = 0;
    long_u	rgb[16];
    listitem_T	*li;

    కొరకు (li = list->lv_మొదటి; li != ఖాళీ && n < 16; li = li->li_తదుపరి, n++)
    {
	char_u		*color_name;
	guicolor_T	guicolor;

	color_name = tv_get_string_chk(&li->li_tv);
	ఒకవేళ (color_name == ఖాళీ)
	    తిరిగి FAIL;

	guicolor = GUI_GET_COLOR(color_name);
	ఒకవేళ (guicolor == INVALCOLOR)
	    తిరిగి FAIL;

	rgb[n] = GUI_MCH_GET_RGB(guicolor);
    }

    ఒకవేళ (n != 16 || li != ఖాళీ)
	తిరిగి FAIL;

    సమితి_vterm_palette(vterm, rgb);

    తిరిగి OK;
}

/*
 * Initialize the ANSI color palette from g:terminal_ansi_colors[0:15]
 */
    స్థిర శూన్య
init_vterm_ansi_colors(VTerm *vterm)
{
    dictitem_T	*var = కనుగొను_var((char_u *)"g:terminal_ansi_colors", ఖాళీ, TRUE);

    ఒకవేళ (var == ఖాళీ)
	తిరిగి;

    ఒకవేళ (var->di_tv.v_type != VAR_LIST
	    || var->di_tv.vval.v_list == ఖాళీ
	    || var->di_tv.vval.v_list->lv_మొదటి == &range_list_అంశం
	    || సమితి_ansi_colors_list(vterm, var->di_tv.vval.v_list) == FAIL)
	semsg(_(e_invalid_వాదం_str), "g:terminal_ansi_colors");
}
#అయితే_ముగింపు

/*
 * Handles a "drop" command from the job in the terminal.
 * "item" is the file name, "item->li_next" may have options.
 */
    స్థిర శూన్య
handle_drop_command(listitem_T *అంశం)
{
    char_u	*fname = tv_get_string(&అంశం->li_tv);
    listitem_T	*opt_అంశం = అంశం->li_తదుపరి;
    పూర్ణాంకం		bufnr;
    win_T	*wp;
    tabpage_T   *tp;
    exarg_T	ea;
    char_u	*tofree = ఖాళీ;

    bufnr = buflist_కూడు(fname, BLN_LISTED | BLN_NOOPT);
    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	ఒకవేళ (wp->w_బఫరు->b_fnum == bufnr)
	{
	    // buffer is in a window already, go there
	    goto_tabpage_win(tp, wp);
	    తిరిగి;
	}
    }

    CLEAR_క్షేత్రం(ea);

    ఒకవేళ (opt_అంశం != ఖాళీ && opt_అంశం->li_tv.v_type == VAR_DICT
					&& opt_అంశం->li_tv.vval.v_dict != ఖాళీ)
    {
	dict_T *dict = opt_అంశం->li_tv.vval.v_dict;
	char_u *p;

	p = dict_get_string(dict, "ff", FALSE);
	ఒకవేళ (p == ఖాళీ)
	    p = dict_get_string(dict, "fileformat", FALSE);
	ఒకవేళ (p != ఖాళీ)
	{
	    ఒకవేళ (check_ff_విలువ(p) == FAIL)
		ch_log(ఖాళీ, "Invalid ff argument to drop: %s", p);
	    లేకపోతే
		ea.force_ff = *p;
	}
	p = dict_get_string(dict, "enc", FALSE);
	ఒకవేళ (p == ఖాళీ)
	    p = dict_get_string(dict, "encoding", FALSE);
	ఒకవేళ (p != ఖాళీ)
	{
	    ea.cmd = alloc(STRLEN(p) + 12);
	    ఒకవేళ (ea.cmd != ఖాళీ)
	    {
		sprintf((అక్షరం *)ea.cmd, "sbuf ++enc=%s", p);
		ea.force_enc = 11;
		tofree = ea.cmd;
	    }
	}

	p = dict_get_string(dict, "bad", FALSE);
	ఒకవేళ (p != ఖాళీ)
	    get_bad_opt(p, &ea);

	ఒకవేళ (dict_has_కీలకం(dict, "bin"))
	    ea.force_bin = FORCE_BIN;
	ఒకవేళ (dict_has_కీలకం(dict, "binary"))
	    ea.force_bin = FORCE_BIN;
	ఒకవేళ (dict_has_కీలకం(dict, "nobin"))
	    ea.force_bin = FORCE_NOBIN;
	ఒకవేళ (dict_has_కీలకం(dict, "nobinary"))
	    ea.force_bin = FORCE_NOBIN;
    }

    // open in new window, like ":split fname"
    ఒకవేళ (ea.cmd == ఖాళీ)
	ea.cmd = (char_u *)"split";
    ea.arg = fname;
    ea.cmdidx = CMD_విభజించు;
    ex_splitview(&ea);

    vim_free(tofree);
}

/*
 * Return TRUE if "func" starts with "pat" and "pat" isn't empty.
 */
    స్థిర పూర్ణాంకం
is_permitted_పదం_api(char_u *func, char_u *pat)
{
    తిరిగి pat != ఖాళీ && *pat != NUL && STRNICMP(func, pat, STRLEN(pat)) == 0;
}

/*
 * Handles a function call from the job running in a terminal.
 * "item" is the function name, "item->li_next" has the arguments.
 */
    స్థిర శూన్య
handle_call_command(పదం_T *పదం, channel_T *channel, listitem_T *అంశం)
{
    char_u	*func;
    typval_T	argvars[2];
    typval_T	rettv;
    funcexe_T	funcexe;

    ఒకవేళ (అంశం->li_తదుపరి == ఖాళీ)
    {
	ch_log(channel, "Missing function arguments for call");
	తిరిగి;
    }
    func = tv_get_string(&అంశం->li_tv);

    ఒకవేళ (!is_permitted_పదం_api(func, పదం->tl_api))
    {
	ch_log(channel, "Unpermitted function: %s", func);
	తిరిగి;
    }

    argvars[0].v_type = VAR_సంఖ్య;
    argvars[0].vval.v_సంఖ్య = పదం->tl_బఫరు->b_fnum;
    argvars[1] = అంశం->li_తదుపరి->li_tv;
    CLEAR_క్షేత్రం(funcexe);
    funcexe.fe_firstline = 1L;
    funcexe.fe_lastline = 1L;
    funcexe.fe_evaluate = TRUE;
    ఒకవేళ (call_func(func, -1, &rettv, 2, argvars, &funcexe) == OK)
    {
	clear_tv(&rettv);
	ch_log(channel, "Function %s called", func);
    }
    లేకపోతే
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
    స్థిర size_t
url_decode(స్థిరరాశి అక్షరం *src, స్థిరరాశి size_t len, char_u *dst)
{
    size_t i = 0, j = 0;

    అయితే (i < len)
    {
	ఒకవేళ (src[i] == '%' && i + 2 < len)
	{
	    dst[j] = hexhex2nr((char_u *)&src[i + 1]);
	    j++;
	    i += 3;
	}
	లేకపోతే
	{
	    dst[j] = src[i];
	    i++;
	    j++;
	}
    }
    dst[j] = '\0';
    తిరిగి j;
}

/*
 * Sync terminal buffer's cwd with shell's pwd with the help of OSC 7.
 *
 * The OSC 7 sequence has the format of
 * "\033]7;file://HOSTNAME/CURRENT/DIR\033\\"
 * and what VTerm provides via VTermStringFragment is
 * "file://HOSTNAME/CURRENT/DIR"
 */
    స్థిర శూన్య
sync_shell_dir(garray_T *gap)
{
    పూర్ణాంకం       స్థానభ్రంశం = 7;  // len of "file://" is 7
    అక్షరం      *pos = (అక్షరం *)gap->ga_దత్తాంశం + స్థానభ్రంశం;
    char_u    *new_dir;

    // remove HOSTNAME to get PWD
    అయితే (స్థానభ్రంశం < (పూర్ణాంకం)gap->ga_len && *pos != '/' )
    {
	++స్థానభ్రంశం;
	++pos;
    }

    ఒకవేళ (స్థానభ్రంశం >= (పూర్ణాంకం)gap->ga_len)
    {
	semsg(_(e_failed_గమ్యం_వెలికితీయి_pwd_from_str_check_your_shell_config),
								 gap->ga_దత్తాంశం);
	తిరిగి;
    }

    new_dir = alloc(gap->ga_len - స్థానభ్రంశం + 1);
    url_decode(pos, gap->ga_len-స్థానభ్రంశం, new_dir);
    changedir_func(new_dir, TRUE, CDSCOPE_కిటికీ);
    vim_free(new_dir);
}

/*
 * Called by libvterm when it cannot recognize an OSC sequence.
 * We recognize a terminal API command.
 */
    స్థిర పూర్ణాంకం
parse_osc(పూర్ణాంకం command, VTermStringFragment frag, శూన్య *user)
{
    పదం_T	*పదం = (పదం_T *)user;
    js_read_T	reader;
    typval_T	tv;
    channel_T	*channel = పదం->tl_పని == ఖాళీ ? ఖాళీ
						    : పదం->tl_పని->jv_channel;
    garray_T	*gap = &పదం->tl_osc_buf;

    // We recognize only OSC 5 1 ; {command} and OSC 7 ; {command}
    ఒకవేళ (command != 51 && (command != 7 || !p_asd))
	తిరిగి 0;

    // Concatenate what was received until the final piece is found.
    ఒకవేళ (ga_grow(gap, (పూర్ణాంకం)frag.len + 1) == FAIL)
    {
	ga_clear(gap);
	తిరిగి 1;
    }
    mch_memmove((అక్షరం *)gap->ga_దత్తాంశం + gap->ga_len, frag.str, frag.len);
    gap->ga_len += (పూర్ణాంకం)frag.len;
    ఒకవేళ (!frag.final)
	తిరిగి 1;

    ((అక్షరం *)gap->ga_దత్తాంశం)[gap->ga_len] = 0;

    ఒకవేళ (command == 7)
    {
	sync_shell_dir(gap);
	ga_clear(gap);
	తిరిగి 1;
    }

    reader.js_buf = gap->ga_దత్తాంశం;
    reader.js_fill = ఖాళీ;
    reader.js_వాడిన = 0;
    ఒకవేళ (json_decode(&reader, &tv, 0) == OK
	    && tv.v_type == VAR_LIST
	    && tv.vval.v_list != ఖాళీ)
    {
	listitem_T *అంశం = tv.vval.v_list->lv_మొదటి;

	ఒకవేళ (అంశం == ఖాళీ)
	    ch_log(channel, "Missing command");
	లేకపోతే
	{
	    char_u	*cmd = tv_get_string(&అంశం->li_tv);

	    // Make sure an invoked command doesn't delete the buffer (and the
	    // terminal) under our fingers.
	    ++పదం->tl_బఫరు->b_locked;

	    అంశం = అంశం->li_తదుపరి;
	    ఒకవేళ (అంశం == ఖాళీ)
		ch_log(channel, "Missing argument for %s", cmd);
	    లేకపోతే ఒకవేళ (STRCMP(cmd, "drop") == 0)
		handle_drop_command(అంశం);
	    లేకపోతే ఒకవేళ (STRCMP(cmd, "call") == 0)
		handle_call_command(పదం, channel, అంశం);
	    లేకపోతే
		ch_log(channel, "Invalid command received: %s", cmd);
	    --పదం->tl_బఫరు->b_locked;
	}
    }
    లేకపోతే
	ch_log(channel, "Invalid JSON received");

    ga_clear(gap);
    clear_tv(&tv);
    తిరిగి 1;
}

/*
 * Called by libvterm when it cannot recognize a CSI sequence.
 * We recognize the window position report.
 */
    స్థిర పూర్ణాంకం
parse_csi(
	స్థిరరాశి అక్షరం  *leader వాడని,
	స్థిరరాశి దీర్ఘ  వాదనలు[],
	పూర్ణాంకం	    argcount,
	స్థిరరాశి అక్షరం  *intermed వాడని,
	అక్షరం	    command,
	శూన్య	    *user)
{
    పదం_T	*పదం = (పదం_T *)user;
    అక్షరం	buf[100];
    పూర్ణాంకం		len;
    పూర్ణాంకం		x = 0;
    పూర్ణాంకం		y = 0;
    win_T	*wp;

    // We recognize only CSI 13 t
    ఒకవేళ (command != 't' || argcount != 1 || వాదనలు[0] != 13)
	తిరిగి 0; // not handled

    // When getting the window position is not possible or it fails it results
    // in zero/zero.
#ఒకవేళ defined(FEAT_GUI) \
	|| (defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)) \
	|| defined(MSWIN)
    (శూన్య)ui_get_winpos(&x, &y, (varnumber_T)100);
#అయితే_ముగింపు

    FOR_ALL_WINDOWS(wp)
	ఒకవేళ (wp->w_బఫరు == పదం->tl_బఫరు)
	    ఆపు;
    ఒకవేళ (wp != ఖాళీ)
    {
#నిర్వచితమైతే FEAT_GUI
	ఒకవేళ (gui.లోపల_use)
	{
	    x += wp->w_wincol * gui.char_వెడల్పు;
	    y += W_WINROW(wp) * gui.char_ఎత్తు;
	}
	లేకపోతే
#అయితే_ముగింపు
	{
	    // We roughly estimate the position of the terminal window inside
	    // the Vim window by assuming a 10 x 7 character cell.
	    x += wp->w_wincol * 7;
	    y += W_WINROW(wp) * 10;
	}
    }

    len = vim_snprintf(buf, 100, "\x1b[3;%d;%dt", x, y);
    channel_send(పదం->tl_పని->jv_channel, get_tty_part(పదం),
						     (char_u *)buf, len, ఖాళీ);
    తిరిగి 1;
}

స్థిర VTermStateFallbacks state_fallbacks = {
    ఖాళీ,		// control
    parse_csi,		// csi
    parse_osc,		// osc
    ఖాళీ,		// dcs
    ఖాళీ,		// apc
    ఖాళీ,		// pm
    ఖాళీ		// sos
};

/*
 * Use Vim's allocation functions for vterm so profiling works.
 */
    స్థిర శూన్య *
vterm_malloc(size_t కొలత, శూన్య *దత్తాంశం వాడని)
{
    // make sure that the length is not zero
    తిరిగి alloc_clear(కొలత == 0 ? 1L : కొలత);
}

    స్థిర శూన్య
vterm_memfree(శూన్య *ptr, శూన్య *దత్తాంశం వాడని)
{
    vim_free(ptr);
}

స్థిర VTermAllocatorFunctions vterm_allocator = {
  &vterm_malloc,
  &vterm_memfree
};

/*
 * Create a new vterm and initialize it.
 * Return FAIL when out of memory.
 */
    స్థిర పూర్ణాంకం
create_vterm(పదం_T *పదం, పూర్ణాంకం అడ్డువరుసలు_2, పూర్ణాంకం నిలువుల_సంఖ్య)
{
    VTerm	    *vterm;
    VTermScreen	    *screen;
    VTermState	    *state;
    VTerm_విలువ	    విలువ;

    vterm = vterm_new_with_allocator(అడ్డువరుసలు_2, నిలువుల_సంఖ్య, &vterm_allocator, ఖాళీ);
    పదం->tl_vterm = vterm;
    ఒకవేళ (vterm == ఖాళీ)
	తిరిగి FAIL;

    // Allocate screen and state here, so we can bail out if that fails.
    state = vterm_obtain_state(vterm);
    screen = vterm_obtain_screen(vterm);
    ఒకవేళ (state == ఖాళీ || screen == ఖాళీ)
    {
	vterm_free(vterm);
	తిరిగి FAIL;
    }

    vterm_screen_సమితి_callbacks(screen, &screen_callbacks, పదం);
    vterm_screen_సమితి_damage_విలీనం(screen, VTERM_DAMAGE_SCROLL);
    vterm_screen_callbacks_has_pushline4(screen);
    // TODO: depends on 'encoding'.
    vterm_సమితి_utf8(vterm, 1);

    పదం_init_default_colors(పదం);

    vterm_state_సమితి_default_colors(
	    state,
	    &పదం->tl_default_color.fg,
	    &పదం->tl_default_color.bg);

    ఒకవేళ (t_colors < 16)
	// Less than 16 colors: assume that bold means using a bright color for
	// the foreground color.
	vterm_state_సమితి_bold_highbright(vterm_obtain_state(vterm), 1);

    // Required to initialize most things.
    vterm_screen_reset(screen, 1 /* hard */);

    // Allow using alternate screen.
    vterm_screen_enable_altscreen(screen, 1);

    // For unix do not use a blinking cursor.  In an xterm this causes the
    // cursor to blink if it's blinking in the xterm.
    // For Windows we respect the system wide setting.
#నిర్వచితమైతే MSWIN
    ఒకవేళ (Get_Caret_Blink_సమయం() == INFINITE)
	విలువ.boolean = 0;
    లేకపోతే
	విలువ.boolean = 1;
#లేకపోతే
    విలువ.boolean = 0;
#అయితే_ముగింపు
    vterm_state_సమితి_termprop(state, VTERM_PROP_CURSORBLINK, &విలువ);
    vterm_state_సమితి_unrecognised_fallbacks(state, &state_fallbacks, పదం);

    తిరిగి OK;
}

/*
 * Reset the terminal palette to its default value.
 */
    స్థిర శూన్య
పదం_reset_palette(VTerm *vterm)
{
    VTermState	*state = vterm_obtain_state(vterm);
    పూర్ణాంకం		సూచీ;

    కొరకు (సూచీ = 0; సూచీ < 16; సూచీ++)
    {
	VTermColor	color;

	color.type = VTERM_COLOR_సూచీకృత;
	ansi_color2rgb(సూచీ, &color.red, &color.green, &color.blue,
		&color.సూచీ);
	// The first valid index starts at 1.
	color.సూచీ -= 1;

	vterm_state_సమితి_palette_color(state, సూచీ, &color);
    }
}

    స్థిర శూన్య
పదం_update_palette(పదం_T *పదం)
{
#ఒకవేళ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    ఒకవేళ (పదం_use_palette()
	    && (పదం->tl_palette != ఖాళీ
		|| కనుగొను_var((char_u *)"g:terminal_ansi_colors", ఖాళీ, TRUE)
								      != ఖాళీ))
    {
	ఒకవేళ (పదం->tl_palette != ఖాళీ)
	    సమితి_vterm_palette(పదం->tl_vterm, పదం->tl_palette);
	లేకపోతే
	    init_vterm_ansi_colors(పదం->tl_vterm);
    }
    లేకపోతే
#అయితే_ముగింపు
	పదం_reset_palette(పదం->tl_vterm);
}

/*
 * Called when option 'termguicolors' is changed.
 */
    శూన్య
పదం_update_palette_all(శూన్య)
{
    పదం_T *పదం;

    FOR_ALL_TERMS(పదం)
    {
	ఒకవేళ (పదం->tl_vterm == ఖాళీ)
	    కొనసాగు;
	పదం_update_palette(పదం);
    }
}

/*
 * Called when option 'background' or 'termguicolors' was set,
 * or when any highlight is changed.
 */
    శూన్య
పదం_update_colors_all(శూన్య)
{
    పదం_T *పదం;

    FOR_ALL_TERMS(పదం)
    {
	ఒకవేళ (పదం->tl_vterm == ఖాళీ)
	    కొనసాగు;
	పదం_init_default_colors(పదం);
	vterm_state_సమితి_default_colors(
		vterm_obtain_state(పదం->tl_vterm),
		&పదం->tl_default_color.fg,
		&పదం->tl_default_color.bg);
    }
}

/*
 * Return the text to show for the buffer name and status.
 */
    char_u *
పదం_get_స్థితి_పాఠ్యం(పదం_T *పదం)
{
    ఒకవేళ (పదం->tl_స్థితి_పాఠ్యం != ఖాళీ)
	తిరిగి పదం->tl_స్థితి_పాఠ్యం;

    char_u *txt;
    size_t len;
    char_u *fname;

    ఒకవేళ (పదం->tl_సాధారణ_బహుళకం)
    {
	ఒకవేళ (పదం_పని_running(పదం))
	    txt = (char_u *)_("Terminal");
	లేకపోతే
	    txt = (char_u *)_("Terminal-finished");
    }
    లేకపోతే ఒకవేళ (పదం->tl_title != ఖాళీ)
	txt = పదం->tl_title;
    లేకపోతే ఒకవేళ (పదం_none_open(పదం))
	txt = (char_u *)_("active");
    లేకపోతే ఒకవేళ (పదం_పని_running(పదం))
	txt = (char_u *)_("running");
    లేకపోతే
	txt = (char_u *)_("finished");
    fname = buf_get_fname(పదం->tl_బఫరు);
    len = 9 + STRLEN(fname) + STRLEN(txt);
    పదం->tl_స్థితి_పాఠ్యం = alloc(len);
    ఒకవేళ (పదం->tl_స్థితి_పాఠ్యం != ఖాళీ)
	vim_snprintf((అక్షరం *)పదం->tl_స్థితి_పాఠ్యం, len, "%s [%s]",
		fname, txt);
    తిరిగి పదం->tl_స్థితి_పాఠ్యం;
}

/*
 * Clear the cached value of the status text.
 */
    శూన్య
పదం_clear_స్థితి_పాఠ్యం(పదం_T *పదం)
{
    VIM_CLEAR(పదం->tl_స్థితి_పాఠ్యం);
}

/*
 * Mark references in jobs of terminals.
 */
    పూర్ణాంకం
సమితి_ref_లోపల_పదం(పూర్ణాంకం copyID)
{
    పూర్ణాంకం		abort = FALSE;
    పదం_T	*పదం;
    typval_T	tv;

    కొరకు (పదం = మొదటి_పదం; !abort && పదం != ఖాళీ; పదం = పదం->tl_తదుపరి)
	ఒకవేళ (పదం->tl_పని != ఖాళీ)
	{
	    tv.v_type = VAR_పని;
	    tv.vval.v_పని = పదం->tl_పని;
	    abort = abort || సమితి_ref_లోపల_అంశం(&tv, copyID, ఖాళీ, ఖాళీ, ఖాళీ);
	}
    తిరిగి abort;
}

/*
 * Get the buffer from the first argument in "argvars".
 * Returns NULL when the buffer is not for a terminal window and logs a message
 * with "where".
 */
    స్థిర buf_T *
పదం_get_buf(typval_T *argvars, అక్షరం *where)
{
    buf_T *buf;

    ++emsg_off;
    buf = tv_get_buf(&argvars[0], FALSE);
    --emsg_off;
    ఒకవేళ (buf == ఖాళీ || buf->b_పదం == ఖాళీ)
    {
	(శూన్య)tv_get_సంఖ్య(&argvars[0]);    // issue errmsg if type error
	ch_log(ఖాళీ, "%s: invalid buffer argument", where);
	తిరిగి ఖాళీ;
    }
    తిరిగి buf;
}

    స్థిర శూన్య
clear_cell(VTermScreenCell *cell)
{
    CLEAR_క్షేత్రం(*cell);
    cell->fg.type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_FG;
    cell->bg.type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_BG;
}

    స్థిర శూన్య
dump_పదం_color(FILE *fd, VTermColor *color)
{
    పూర్ణాంకం సూచీ;

    ఒకవేళ (VTERM_COLOR_IS_సూచీకృత(color))
	సూచీ = color->సూచీ + 1;
    లేకపోతే ఒకవేళ (color->type == 0)
	// use RGB values
	సూచీ = 255;
    లేకపోతే
	// default color
	సూచీ = 0;
    fprintf(fd, "%02x%02x%02x%d",
	    (పూర్ణాంకం)color->red, (పూర్ణాంకం)color->green, (పూర్ణాంకం)color->blue,
	    సూచీ);
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
    శూన్య
f_పదం_dumpwrite(typval_T *argvars, typval_T *rettv వాడని)
{
    buf_T	*buf;
    పదం_T	*పదం;
    char_u	*fname;
    పూర్ణాంకం		max_ఎత్తు_2 = 0;
    పూర్ణాంకం		max_వెడల్పు_2 = 0;
    stat_T	st;
    FILE	*fd;
    VTermPos	pos;
    VTermScreen *screen;
    VTermScreenCell prev_cell;
    VTermState	*state;
    VTermPos	cursor_pos;

    ఒకవేళ (check_restricted() || check_secure())
	తిరిగి;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_బఫరు_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	తిరిగి;

    buf = పదం_get_buf(argvars, "term_dumpwrite()");
    ఒకవేళ (buf == ఖాళీ)
	తిరిగి;
    పదం = buf->b_పదం;
    ఒకవేళ (పదం->tl_vterm == ఖాళీ)
    {
	emsg(_(e_పని_already_ముగిసిన));
	తిరిగి;
    }

    ఒకవేళ (argvars[2].v_type != VAR_UNKNOWN)
    {
	dict_T *d;

	ఒకవేళ (check_for_dict_arg(argvars, 2) == FAIL)
	    తిరిగి;
	d = argvars[2].vval.v_dict;
	ఒకవేళ (d != ఖాళీ)
	{
	    max_ఎత్తు_2 = dict_get_సంఖ్య(d, "rows");
	    max_వెడల్పు_2 = dict_get_సంఖ్య(d, "columns");
	}
    }

    fname = tv_get_string_chk(&argvars[1]);
    ఒకవేళ (fname == ఖాళీ)
	తిరిగి;
    ఒకవేళ (mch_stat((అక్షరం *)fname, &st) >= 0)
    {
	semsg(_(e_file_exists_str), fname);
	తిరిగి;
    }

    ఒకవేళ (*fname == NUL || (fd = mch_fopen((అక్షరం *)fname, WRITEBIN)) == ఖాళీ)
    {
	semsg(_(e_cant_create_file_str), *fname == NUL ? (char_u *)_("<empty>") : fname);
	తిరిగి;
    }

    clear_cell(&prev_cell);

    screen = vterm_obtain_screen(పదం->tl_vterm);
    state = vterm_obtain_state(పదం->tl_vterm);
    vterm_state_get_cursorpos(state, &cursor_pos);

    కొరకు (pos.అడ్డువరుస = 0; (max_ఎత్తు_2 == 0 || pos.అడ్డువరుస < max_ఎత్తు_2)
					 && pos.అడ్డువరుస < పదం->tl_అడ్డువరుసలు; ++pos.అడ్డువరుస)
    {
	పూర్ణాంకం		repeat = 0;

	కొరకు (pos.నిలువు = 0; (max_వెడల్పు_2 == 0 || pos.నిలువు < max_వెడల్పు_2)
					 && pos.నిలువు < పదం->tl_నిలువుల_సంఖ్య; ++pos.నిలువు)
	{
	    VTermScreenCell cell;
	    పూర్ణాంకం		    same_attr;
	    పూర్ణాంకం		    same_chars = TRUE;
	    పూర్ణాంకం		    i;
	    పూర్ణాంకం		    is_cursor_pos = (pos.నిలువు == cursor_pos.నిలువు
						 && pos.అడ్డువరుస == cursor_pos.అడ్డువరుస);

	    ఒకవేళ (vterm_screen_get_cell(screen, pos, &cell) == 0)
		clear_cell(&cell);

	    కొరకు (i = 0; i < VTERM_MAX_CHARS_చొప్పున_CELL; ++i)
	    {
		పూర్ణాంకం c = cell.chars[i];
		పూర్ణాంకం pc = prev_cell.chars[i];
		పూర్ణాంకం should_break = c == NUL || pc == NUL;

		// For the first character NUL is the same as space.
		ఒకవేళ (i == 0)
		{
		    c = (c == NUL) ? ' ' : c;
		    pc = (pc == NUL) ? ' ' : pc;
		}
		ఒకవేళ (c != pc)
		    same_chars = FALSE;
		ఒకవేళ (should_break)
		    ఆపు;
	    }
	    same_attr = vtermAttr2hl(&cell.attrs)
					      == vtermAttr2hl(&prev_cell.attrs)
			&& vterm_color_is_equal(&cell.fg, &prev_cell.fg)
			&& vterm_color_is_equal(&cell.bg, &prev_cell.bg);
	    ఒకవేళ (same_chars && cell.వెడల్పు == prev_cell.వెడల్పు && same_attr
							     && !is_cursor_pos)
	    {
		++repeat;
	    }
	    లేకపోతే
	    {
		ఒకవేళ (repeat > 0)
		{
		    fprintf(fd, "@%d", repeat);
		    repeat = 0;
		}
		fputs(is_cursor_pos ? ">" : "|", fd);

		ఒకవేళ (cell.chars[0] == NUL)
		    fputs(" ", fd);
		లేకపోతే
		{
		    char_u	charbuf[10];
		    పూర్ణాంకం		len;

		    కొరకు (i = 0; i < VTERM_MAX_CHARS_చొప్పున_CELL
						  && cell.chars[i] != NUL; ++i)
		    {
			len = utf_char2bytes(cell.chars[i], charbuf);
			fwrite(charbuf, len, 1, fd);
		    }
		}

		// When only the characters differ we don't write anything, the
		// following "|", "@" or NL will indicate using the same
		// attributes.
		ఒకవేళ (cell.వెడల్పు != prev_cell.వెడల్పు || !same_attr)
		{
		    ఒకవేళ (cell.వెడల్పు == 2)
			fputs("*", fd);
		    లేకపోతే
			fputs("+", fd);

		    ఒకవేళ (same_attr)
		    {
			fputs("&", fd);
		    }
		    లేకపోతే
		    {
			fprintf(fd, "%d", vtermAttr2hl(&cell.attrs));
			ఒకవేళ (vterm_color_is_equal(&cell.fg, &prev_cell.fg))
			    fputs("&", fd);
			లేకపోతే
			{
			    fputs("#", fd);
			    dump_పదం_color(fd, &cell.fg);
			}
			ఒకవేళ (vterm_color_is_equal(&cell.bg, &prev_cell.bg))
			    fputs("&", fd);
			లేకపోతే
			{
			    fputs("#", fd);
			    dump_పదం_color(fd, &cell.bg);
			}
		    }
		}

		prev_cell = cell;
	    }

	    ఒకవేళ (cell.వెడల్పు == 2)
		++pos.నిలువు;
	}
	ఒకవేళ (repeat > 0)
	    fprintf(fd, "@%d", repeat);
	fputs("\n", fd);
    }

    fclose(fd);
}

/*
 * Called when a dump is corrupted.  Put a breakpoint here when debugging.
 */
    స్థిర శూన్య
dump_is_corrupt(garray_T *gap)
{
    GA_CONCAT_LITERAL(gap, "CORRUPT");
}

    స్థిర శూన్య
append_cell(garray_T *gap, cellattr_T *cell)
{
    ఒకవేళ (ga_grow(gap, 1) == FAIL)
	తిరిగి;

    *(((cellattr_T *)gap->ga_దత్తాంశం) + gap->ga_len) = *cell;
    ++gap->ga_len;
}

    స్థిర శూన్య
clear_cellattr(cellattr_T *cell)
{
    CLEAR_క్షేత్రం(*cell);
    cell->fg.type = VTERM_COLOR_DEFAULT_FG;
    cell->bg.type = VTERM_COLOR_DEFAULT_BG;
}

/*
 * Read the dump file from "fd" and append lines to the current buffer.
 * Return the cell width of the longest line.
 */
    స్థిర పూర్ణాంకం
read_dump_file(FILE *fd, VTermPos *cursor_pos)
{
    పూర్ణాంకం		    c;
    garray_T	    ga_పాఠ్యం;
    garray_T	    ga_cell;
    string_T	    prev_char = {ఖాళీ, 0};
    పూర్ణాంకం		    attr = 0;
    cellattr_T	    cell;
    cellattr_T	    ఖాళీలు_cell;
    పదం_T	    *పదం = curbuf->b_పదం;
    పూర్ణాంకం		    max_cells = 0;
    పూర్ణాంకం		    ప్రారంభం_అడ్డువరుస = పదం->tl_scrollback.ga_len;

    ga_init2(&ga_పాఠ్యం, 1, 90);
    ga_init2(&ga_cell, పరిమాణం(cellattr_T), 90);
    clear_cellattr(&cell);
    clear_cellattr(&ఖాళీలు_cell);
    cursor_pos->అడ్డువరుస = -1;
    cursor_pos->నిలువు = -1;

    c = fgetc(fd);
    కొరకు (;;)
    {
	ఒకవేళ (c == EOF)
	    ఆపు;
	ఒకవేళ (c == '\r')
	{
	    // DOS line endings?  Ignore.
	    c = fgetc(fd);
	}
	లేకపోతే ఒకవేళ (c == '\n')
	{
	    // End of a line: append it to the buffer.
	    ఒకవేళ (ga_పాఠ్యం.ga_దత్తాంశం == ఖాళీ)
		dump_is_corrupt(&ga_పాఠ్యం);
	    ఒకవేళ (ga_grow(&పదం->tl_scrollback, 1) == OK)
	    {
		sb_పంక్తి_T   *పంక్తి = (sb_పంక్తి_T *)పదం->tl_scrollback.ga_దత్తాంశం
						  + పదం->tl_scrollback.ga_len;

		ఒకవేళ (max_cells < ga_cell.ga_len)
		    max_cells = ga_cell.ga_len;
		పంక్తి->sb_నిలువుల_సంఖ్య = ga_cell.ga_len;
		పంక్తి->sb_బైట్లు = ga_పాఠ్యం.ga_len;
		పంక్తి->sb_cells = ga_cell.ga_దత్తాంశం;
		పంక్తి->sb_fill_attr = పదం->tl_default_color;
		పంక్తి->continuation = 0;
		++పదం->tl_scrollback.ga_len;
		ga_init(&ga_cell);

		ga_append(&ga_పాఠ్యం, NUL);
		ml_append(curbuf->b_ml.ml_పంక్తి_లెక్క, ga_పాఠ్యం.ga_దత్తాంశం,
							ga_పాఠ్యం.ga_len, FALSE);
	    }
	    లేకపోతే
		ga_clear(&ga_cell);
	    ga_పాఠ్యం.ga_len = 0;

	    c = fgetc(fd);
	}
	లేకపోతే ఒకవేళ (c == '|' || c == '>')
	{
	    పూర్ణాంకం prev_len = ga_పాఠ్యం.ga_len;

	    ఒకవేళ (c == '>')
	    {
		ఒకవేళ (cursor_pos->అడ్డువరుస != -1)
		    dump_is_corrupt(&ga_పాఠ్యం);	// duplicate cursor
		cursor_pos->అడ్డువరుస = పదం->tl_scrollback.ga_len - ప్రారంభం_అడ్డువరుస;
		cursor_pos->నిలువు = ga_cell.ga_len;
	    }

	    // normal character(s) followed by "+", "*", "|", "@" or NL
	    c = fgetc(fd);
	    ఒకవేళ (c != EOF)
		ga_append(&ga_పాఠ్యం, c);
	    కొరకు (;;)
	    {
		c = fgetc(fd);
		ఒకవేళ (c == '+' || c == '*' || c == '|' || c == '>' || c == '@'
						      || c == EOF || c == '\n')
		    ఆపు;
		ga_append(&ga_పాఠ్యం, c);
	    }

	    // save the character for repeating it
	    VIM_CLEAR_STRING(prev_char);
	    ఒకవేళ (ga_పాఠ్యం.ga_దత్తాంశం != ఖాళీ)
	    {
		prev_char.నిడివి = (size_t)(ga_పాఠ్యం.ga_len - prev_len);
		prev_char.string = vim_strnsave(
		    ((char_u *)ga_పాఠ్యం.ga_దత్తాంశం) + prev_len,
		    prev_char.నిడివి);
		ఒకవేళ (prev_char.string == ఖాళీ)
		    prev_char.నిడివి = 0;
	    }

	    ఒకవేళ (c == '@' || c == '|' || c == '>' || c == '\n')
	    {
		// use all attributes from previous cell
	    }
	    లేకపోతే ఒకవేళ (c == '+' || c == '*')
	    {
		పూర్ణాంకం is_bg;

		cell.వెడల్పు = c == '+' ? 1 : 2;

		c = fgetc(fd);
		ఒకవేళ (c == '&')
		{
		    // use same attr as previous cell
		    c = fgetc(fd);
		}
		లేకపోతే ఒకవేళ (సురక్షిత_isdigit(c))
		{
		    // get the decimal attribute
		    attr = 0;
		    అయితే (సురక్షిత_isdigit(c))
		    {
			attr = attr * 10 + (c - '0');
			c = fgetc(fd);
		    }
		    hl2vtermAttr(attr, &cell);

		    // is_bg == 0: fg, is_bg == 1: bg
		    కొరకు (is_bg = 0; is_bg <= 1; ++is_bg)
		    {
			ఒకవేళ (c == '&')
			{
			    // use same color as previous cell
			    c = fgetc(fd);
			}
			లేకపోతే ఒకవేళ (c == '#')
			{
			    పూర్ణాంకం red, green, blue, సూచీ = 0, type;

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
			    ఒకవేళ (!సురక్షిత_isdigit(c))
				dump_is_corrupt(&ga_పాఠ్యం);
			    అయితే (సురక్షిత_isdigit(c))
			    {
				సూచీ = సూచీ * 10 + (c - '0');
				c = fgetc(fd);
			    }
			    ఒకవేళ (సూచీ == 0 || సూచీ == 255)
			    {
				type = VTERM_COLOR_RGB;
				ఒకవేళ (సూచీ == 0)
				{
				    ఒకవేళ (is_bg)
					type |= VTERM_COLOR_DEFAULT_BG;
				    లేకపోతే
					type |= VTERM_COLOR_DEFAULT_FG;
				}
			    }
			    లేకపోతే
			    {
				type = VTERM_COLOR_సూచీకృత;
				సూచీ -= 1;
			    }
			    ఒకవేళ (is_bg)
			    {
				cell.bg.type = type;
				cell.bg.red = red;
				cell.bg.green = green;
				cell.bg.blue = blue;
				cell.bg.సూచీ = సూచీ;
			    }
			    లేకపోతే
			    {
				cell.fg.type = type;
				cell.fg.red = red;
				cell.fg.green = green;
				cell.fg.blue = blue;
				cell.fg.సూచీ = సూచీ;
			    }
			}
			లేకపోతే
			    dump_is_corrupt(&ga_పాఠ్యం);
		    }
		}
		లేకపోతే
		    dump_is_corrupt(&ga_పాఠ్యం);
	    }
	    లేకపోతే
		dump_is_corrupt(&ga_పాఠ్యం);

	    append_cell(&ga_cell, &cell);
	    ఒకవేళ (cell.వెడల్పు == 2)
		append_cell(&ga_cell, &ఖాళీలు_cell);
	}
	లేకపోతే ఒకవేళ (c == '@')
	{
	    ఒకవేళ (prev_char.string == ఖాళీ)
		dump_is_corrupt(&ga_పాఠ్యం);
	    లేకపోతే
	    {
		పూర్ణాంకం లెక్క = 0;

		// repeat previous character, get the count
		కొరకు (;;)
		{
		    c = fgetc(fd);
		    ఒకవేళ (!సురక్షిత_isdigit(c))
			ఆపు;
		    లెక్క = లెక్క * 10 + (c - '0');
		}

		అయితే (లెక్క-- > 0)
		{
		    ga_concat_len(&ga_పాఠ్యం, prev_char.string, prev_char.నిడివి);
		    append_cell(&ga_cell, &cell);
		}
	    }
	}
	లేకపోతే
	{
	    dump_is_corrupt(&ga_పాఠ్యం);
	    c = fgetc(fd);
	}
    }

    ఒకవేళ (ga_పాఠ్యం.ga_len > 0)
    {
	// trailing characters after last NL
	dump_is_corrupt(&ga_పాఠ్యం);
	ga_append(&ga_పాఠ్యం, NUL);
	ml_append(curbuf->b_ml.ml_పంక్తి_లెక్క, ga_పాఠ్యం.ga_దత్తాంశం,
							ga_పాఠ్యం.ga_len, FALSE);
    }

    ga_clear(&ga_పాఠ్యం);
    ga_clear(&ga_cell);
    vim_free(prev_char.string);

    తిరిగి max_cells;
}

/*
 * Return an allocated string with at least "text_width" "=" characters and
 * "fname" inserted in the middle.
 */
    స్థిర char_u *
get_separator(పూర్ణాంకం పాఠ్యం_వెడల్పు, char_u *fname)
{
    పూర్ణాంకం	    వెడల్పు = MAX(పాఠ్యం_వెడల్పు, curwin->w_వెడల్పు);
    char_u  *textline;
    పూర్ణాంకం	    fname_కొలత;
    char_u  *p = fname;
    పూర్ణాంకం	    i;
    size_t  off;

    textline = alloc(వెడల్పు + STRLEN(fname) + 1);
    ఒకవేళ (textline == ఖాళీ)
	తిరిగి ఖాళీ;

    fname_కొలత = vim_strsize(fname);
    ఒకవేళ (fname_కొలత < వెడల్పు - 8)
    {
	// enough room, don't use the full window width
	వెడల్పు = MAX(పాఠ్యం_వెడల్పు, fname_కొలత + 8);
    }
    లేకపోతే ఒకవేళ (fname_కొలత > వెడల్పు - 8)
    {
	// full name doesn't fit, use only the tail
	p = gettail(fname);
	fname_కొలత = vim_strsize(p);
    }
    // skip characters until the name fits
    అయితే (fname_కొలత > వెడల్పు - 8)
    {
	p += (*mb_ptr2len)(p);
	fname_కొలత = vim_strsize(p);
    }

    కొరకు (i = 0; i < (వెడల్పు - fname_కొలత) / 2 - 1; ++i)
	textline[i] = '=';
    textline[i++] = ' ';

    STRCPY(textline + i, p);
    off = STRLEN(textline);
    textline[off] = ' ';
    కొరకు (i = 1; i < (వెడల్పు - fname_కొలత) / 2; ++i)
	textline[off + i] = '=';
    textline[off + i] = NUL;

    తిరిగి textline;
}

/*
 * Common for "term_dumpdiff()" and "term_dumpload()".
 */
    స్థిర శూన్య
పదం_ఎక్కించు_dump(typval_T *argvars, typval_T *rettv, పూర్ణాంకం do_diff)
{
    jobopt_T	opt;
    buf_T	*buf = ఖాళీ;
    char_u	buf1[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    char_u	*fname1;
    char_u	*fname2 = ఖాళీ;
    char_u	*fname_tofree = ఖాళీ;
    FILE	*fd1;
    FILE	*fd2 = ఖాళీ;
    char_u	*textline = ఖాళీ;

    // First open the files.  If this fails bail out.
    fname1 = tv_get_string_buf_chk(&argvars[0], buf1);
    ఒకవేళ (do_diff)
	fname2 = tv_get_string_buf_chk(&argvars[1], buf2);
    ఒకవేళ (fname1 == ఖాళీ || (do_diff && fname2 == ఖాళీ))
    {
	emsg(_(e_invalid_వాదం));
	తిరిగి;
    }
    fd1 = mch_fopen((అక్షరం *)fname1, READBIN);
    ఒకవేళ (fd1 == ఖాళీ)
    {
	semsg(_(e_cant_read_file_str), fname1);
	తిరిగి;
    }
    ఒకవేళ (do_diff)
    {
	fd2 = mch_fopen((అక్షరం *)fname2, READBIN);
	ఒకవేళ (fd2 == ఖాళీ)
	{
	    fclose(fd1);
	    semsg(_(e_cant_read_file_str), fname2);
	    తిరిగి;
	}
    }

    init_పని_options(&opt);
    ఒకవేళ (argvars[do_diff ? 2 : 1].v_type != VAR_UNKNOWN
	    && get_పని_options(&argvars[do_diff ? 2 : 1], &opt, 0,
		    JO2_పదం_NAME + JO2_పదం_నిలువుల_సంఖ్య + JO2_పదం_అడ్డువరుసలు
		    + JO2_VERTICAL + JO2_CURWIN + JO2_NORESTORE) == FAIL)
	వెళ్ళు theend;

    ఒకవేళ (opt.jo_పదం_name == ఖాళీ)
    {
	size_t len = STRLEN(fname1) + 12;

	fname_tofree = alloc(len);
	ఒకవేళ (fname_tofree != ఖాళీ)
	{
	    vim_snprintf((అక్షరం *)fname_tofree, len, "dump diff %s", fname1);
	    opt.jo_పదం_name = fname_tofree;
	}
    }

    ఒకవేళ (opt.jo_bufnr_buf != ఖాళీ)
    {
	win_T *wp = buf_jump_open_win(opt.jo_bufnr_buf);

	// With "bufnr" argument: enter the window with this buffer and make it
	// empty.
	ఒకవేళ (wp == ఖాళీ)
	    semsg(_(e_invalid_వాదం_str), "bufnr");
	లేకపోతే
	{
	    buf = curbuf;
	    అయితే (!(curbuf->b_ml.ml_flags & ML_ఖాళీలు))
		ml_delete((linenr_T)1);
	    free_scrollback(curbuf->b_పదం);
	    redraw_later(UPD_NOT_VALID);
	}
    }
    లేకపోతే
	// Create a new terminal window.
	buf = పదం_ప్రారంభం(&argvars[0], ఖాళీ, &opt, పదం_ప్రారంభం_NOJOB);

    ఒకవేళ (buf != ఖాళీ && buf->b_పదం != ఖాళీ)
    {
	పూర్ణాంకం		i;
	linenr_T	bot_lnum;
	linenr_T	lnum;
	పదం_T		*పదం = buf->b_పదం;
	పూర్ణాంకం		వెడల్పు;
	పూర్ణాంకం		width2;
	VTermPos	cursor_pos1;
	VTermPos	cursor_pos2;

	పదం_init_default_colors(పదం);

	rettv->vval.v_సంఖ్య = buf->b_fnum;

	// read the files, fill the buffer with the diff
	వెడల్పు = read_dump_file(fd1, &cursor_pos1);

	// position the cursor
	ఒకవేళ (cursor_pos1.అడ్డువరుస >= 0)
	{
	    curwin->w_cursor.lnum = cursor_pos1.అడ్డువరుస + 1;
	    coladvance(cursor_pos1.నిలువు);
	}

	// Delete the empty line that was in the empty buffer.
	ml_delete(1);

	// For term_dumpload() we are done here.
	ఒకవేళ (!do_diff)
	    వెళ్ళు theend;

	పదం->tl_శిఖరం_diff_అడ్డువరుసలు = curbuf->b_ml.ml_పంక్తి_లెక్క;

	textline = get_separator(వెడల్పు, fname1);
	ఒకవేళ (textline == ఖాళీ)
	    వెళ్ళు theend;
	ఒకవేళ (కూడు_ఖాళీలు_scrollback(పదం, &పదం->tl_default_color, 0) == OK)
	    ml_append(curbuf->b_ml.ml_పంక్తి_లెక్క, textline, 0, FALSE);
	vim_free(textline);

	textline = get_separator(వెడల్పు, fname2);
	ఒకవేళ (textline == ఖాళీ)
	    వెళ్ళు theend;
	ఒకవేళ (కూడు_ఖాళీలు_scrollback(పదం, &పదం->tl_default_color, 0) == OK)
	    ml_append(curbuf->b_ml.ml_పంక్తి_లెక్క, textline, 0, FALSE);
	textline[వెడల్పు] = NUL;

	bot_lnum = curbuf->b_ml.ml_పంక్తి_లెక్క;
	width2 = read_dump_file(fd2, &cursor_pos2);
	ఒకవేళ (width2 > వెడల్పు)
	{
	    vim_free(textline);
	    textline = alloc(width2 + 1);
	    ఒకవేళ (textline == ఖాళీ)
		వెళ్ళు theend;
	    వెడల్పు = width2;
	    textline[వెడల్పు] = NUL;
	}
	పదం->tl_bot_diff_అడ్డువరుసలు = curbuf->b_ml.ml_పంక్తి_లెక్క - bot_lnum;

	కొరకు (lnum = 1; lnum <= పదం->tl_శిఖరం_diff_అడ్డువరుసలు; ++lnum)
	{
	    ఒకవేళ (lnum + bot_lnum > curbuf->b_ml.ml_పంక్తి_లెక్క)
	    {
		// bottom part has fewer rows, fill with "-"
		కొరకు (i = 0; i < వెడల్పు; ++i)
		    textline[i] = '-';
	    }
	    లేకపోతే
	    {
		char_u *line1;
		char_u *line2;
		char_u *p1;
		char_u *p2;
		పూర్ణాంకం	నిలువు;
		sb_పంక్తి_T   *sb_పంక్తి = (sb_పంక్తి_T *)పదం->tl_scrollback.ga_దత్తాంశం;
		cellattr_T *cellattr1 = (sb_పంక్తి + lnum - 1)->sb_cells;
		cellattr_T *cellattr2 = (sb_పంక్తి + lnum + bot_lnum - 1)
								    ->sb_cells;

		// Make a copy, getting the second line will invalidate it.
		line1 = vim_strsave(ml_get(lnum));
		ఒకవేళ (line1 == ఖాళీ)
		    ఆపు;
		p1 = line1;

		line2 = ml_get(lnum + bot_lnum);
		p2 = line2;
		కొరకు (నిలువు = 0; నిలువు < వెడల్పు && *p1 != NUL && *p2 != NUL; ++నిలువు)
		{
		    పూర్ణాంకం len1 = utfc_ptr2len(p1);
		    పూర్ణాంకం len2 = utfc_ptr2len(p2);

		    textline[నిలువు] = ' ';
		    ఒకవేళ (len1 != len2 || STRNCMP(p1, p2, len1) != 0)
			// text differs
			textline[నిలువు] = 'X';
		    లేకపోతే ఒకవేళ (lnum == cursor_pos1.అడ్డువరుస + 1
			    && నిలువు == cursor_pos1.నిలువు
			    && (cursor_pos1.అడ్డువరుస != cursor_pos2.అడ్డువరుస
					|| cursor_pos1.నిలువు != cursor_pos2.నిలువు))
			// cursor in first but not in second
			textline[నిలువు] = '>';
		    లేకపోతే ఒకవేళ (lnum == cursor_pos2.అడ్డువరుస + 1
			    && నిలువు == cursor_pos2.నిలువు
			    && (cursor_pos1.అడ్డువరుస != cursor_pos2.అడ్డువరుస
					|| cursor_pos1.నిలువు != cursor_pos2.నిలువు))
			// cursor in second but not in first
			textline[నిలువు] = '<';
		    లేకపోతే ఒకవేళ (cellattr1 != ఖాళీ && cellattr2 != ఖాళీ)
		    {
			ఒకవేళ ((cellattr1 + నిలువు)->వెడల్పు
						   != (cellattr2 + నిలువు)->వెడల్పు)
			    textline[నిలువు] = 'w';
			లేకపోతే ఒకవేళ (!vterm_color_is_equal(&(cellattr1 + నిలువు)->fg,
						   &(cellattr2 + నిలువు)->fg))
			    textline[నిలువు] = 'f';
			లేకపోతే ఒకవేళ (!vterm_color_is_equal(&(cellattr1 + నిలువు)->bg,
						   &(cellattr2 + నిలువు)->bg))
			    textline[నిలువు] = 'b';
			లేకపోతే ఒకవేళ (vtermAttr2hl(&(cellattr1 + నిలువు)->attrs)
				  != vtermAttr2hl(&((cellattr2 + నిలువు)->attrs)))
			    textline[నిలువు] = 'a';
		    }
		    p1 += len1;
		    p2 += len2;
		    // TODO: handle different width
		}

		అయితే (నిలువు < వెడల్పు)
		{
		    ఒకవేళ (*p1 == NUL && *p2 == NUL)
			textline[నిలువు] = '?';
		    లేకపోతే ఒకవేళ (*p1 == NUL)
		    {
			textline[నిలువు] = '+';
			p2 += utfc_ptr2len(p2);
		    }
		    లేకపోతే
		    {
			textline[నిలువు] = '-';
			p1 += utfc_ptr2len(p1);
		    }
		    ++నిలువు;
		}

		vim_free(line1);
	    }
	    ఒకవేళ (కూడు_ఖాళీలు_scrollback(పదం, &పదం->tl_default_color,
						 పదం->tl_శిఖరం_diff_అడ్డువరుసలు) == OK)
		ml_append(పదం->tl_శిఖరం_diff_అడ్డువరుసలు + lnum, textline, 0, FALSE);
	    ++bot_lnum;
	}

	అయితే (lnum + bot_lnum <= curbuf->b_ml.ml_పంక్తి_లెక్క)
	{
	    // bottom part has more rows, fill with "+"
	    కొరకు (i = 0; i < వెడల్పు; ++i)
		textline[i] = '+';
	    ఒకవేళ (కూడు_ఖాళీలు_scrollback(పదం, &పదం->tl_default_color,
						 పదం->tl_శిఖరం_diff_అడ్డువరుసలు) == OK)
		ml_append(పదం->tl_శిఖరం_diff_అడ్డువరుసలు + lnum, textline, 0, FALSE);
	    ++lnum;
	    ++bot_lnum;
	}

	పదం->tl_నిలువుల_సంఖ్య = వెడల్పు;

	// looks better without wrapping
	curwin->w_p_wrap = 0;
    }

theend:
    vim_free(textline);
    vim_free(fname_tofree);
    fclose(fd1);
    ఒకవేళ (fd2 != ఖాళీ)
	fclose(fd2);
}

/*
 * If the current buffer shows the output of term_dumpdiff(), swap the top and
 * bottom files.
 * Return FAIL when this is not possible.
 */
    పూర్ణాంకం
పదం_మార్పిడి_diff(శూన్య)
{
    పదం_T	*పదం = curbuf->b_పదం;
    linenr_T	పంక్తి_లెక్క;
    linenr_T	శిఖరం_అడ్డువరుసలు;
    linenr_T	bot_అడ్డువరుసలు;
    linenr_T	bot_ప్రారంభం;
    linenr_T	lnum;
    char_u	*p;
    sb_పంక్తి_T	*sb_పంక్తి;

    ఒకవేళ (పదం == ఖాళీ
	    || !పదం_is_ముగిసిన(curbuf)
	    || పదం->tl_శిఖరం_diff_అడ్డువరుసలు == 0
	    || పదం->tl_scrollback.ga_len == 0)
	తిరిగి FAIL;

    పంక్తి_లెక్క = curbuf->b_ml.ml_పంక్తి_లెక్క;
    శిఖరం_అడ్డువరుసలు = పదం->tl_శిఖరం_diff_అడ్డువరుసలు;
    bot_అడ్డువరుసలు = పదం->tl_bot_diff_అడ్డువరుసలు;
    bot_ప్రారంభం = పంక్తి_లెక్క - bot_అడ్డువరుసలు;
    sb_పంక్తి = (sb_పంక్తి_T *)పదం->tl_scrollback.ga_దత్తాంశం;

    // move lines from top to above the bottom part
    కొరకు (lnum = 1; lnum <= శిఖరం_అడ్డువరుసలు; ++lnum)
    {
	p = vim_strsave(ml_get(1));
	ఒకవేళ (p == ఖాళీ)
	    తిరిగి OK;
	ml_append(bot_ప్రారంభం, p, 0, FALSE);
	ml_delete(1);
	vim_free(p);
    }

    // move lines from bottom to the top
    కొరకు (lnum = 1; lnum <= bot_అడ్డువరుసలు; ++lnum)
    {
	p = vim_strsave(ml_get(bot_ప్రారంభం + lnum));
	ఒకవేళ (p == ఖాళీ)
	    తిరిగి OK;
	ml_delete(bot_ప్రారంభం + lnum);
	ml_append(lnum - 1, p, 0, FALSE);
	vim_free(p);
    }

    // move top title to bottom
    p = vim_strsave(ml_get(bot_అడ్డువరుసలు + 1));
    ఒకవేళ (p == ఖాళీ)
	తిరిగి OK;
    ml_append(పంక్తి_లెక్క - శిఖరం_అడ్డువరుసలు - 1, p, 0, FALSE);
    ml_delete(bot_అడ్డువరుసలు + 1);
    vim_free(p);

    // move bottom title to top
    p = vim_strsave(ml_get(పంక్తి_లెక్క - శిఖరం_అడ్డువరుసలు));
    ఒకవేళ (p == ఖాళీ)
	తిరిగి OK;
    ml_delete(పంక్తి_లెక్క - శిఖరం_అడ్డువరుసలు);
    ml_append(bot_అడ్డువరుసలు, p, 0, FALSE);
    vim_free(p);

    ఒకవేళ (శిఖరం_అడ్డువరుసలు == bot_అడ్డువరుసలు)
    {
	// rows counts are equal, can swap cell properties
	కొరకు (lnum = 0; lnum < శిఖరం_అడ్డువరుసలు; ++lnum)
	{
	    sb_పంక్తి_T	తాత్కాలిక;

	    తాత్కాలిక = *(sb_పంక్తి + lnum);
	    *(sb_పంక్తి + lnum) = *(sb_పంక్తి + bot_ప్రారంభం + lnum);
	    *(sb_పంక్తి + bot_ప్రారంభం + lnum) = తాత్కాలిక;
	}
    }
    లేకపోతే
    {
	size_t		కొలత = పరిమాణం(sb_పంక్తి_T) * పదం->tl_scrollback.ga_len;
	sb_పంక్తి_T	*తాత్కాలిక = alloc(కొలత);

	// need to copy cell properties into temp memory
	ఒకవేళ (తాత్కాలిక != ఖాళీ)
	{
	    mch_memmove(తాత్కాలిక, పదం->tl_scrollback.ga_దత్తాంశం, కొలత);
	    mch_memmove(పదం->tl_scrollback.ga_దత్తాంశం,
		    తాత్కాలిక + bot_ప్రారంభం,
		    పరిమాణం(sb_పంక్తి_T) * bot_అడ్డువరుసలు);
	    mch_memmove((sb_పంక్తి_T *)పదం->tl_scrollback.ga_దత్తాంశం + bot_అడ్డువరుసలు,
		    తాత్కాలిక + శిఖరం_అడ్డువరుసలు,
		    పరిమాణం(sb_పంక్తి_T) * (పంక్తి_లెక్క - శిఖరం_అడ్డువరుసలు - bot_అడ్డువరుసలు));
	    mch_memmove((sb_పంక్తి_T *)పదం->tl_scrollback.ga_దత్తాంశం
						       + పంక్తి_లెక్క - శిఖరం_అడ్డువరుసలు,
		    తాత్కాలిక,
		    పరిమాణం(sb_పంక్తి_T) * శిఖరం_అడ్డువరుసలు);
	    vim_free(తాత్కాలిక);
	}
    }

    పదం->tl_శిఖరం_diff_అడ్డువరుసలు = bot_అడ్డువరుసలు;
    పదం->tl_bot_diff_అడ్డువరుసలు = శిఖరం_అడ్డువరుసలు;

    update_screen(UPD_NOT_VALID);
    తిరిగి OK;
}

/*
 * "term_dumpdiff(filename, filename, options)" function
 */
    శూన్య
f_పదం_dumpdiff(typval_T *argvars, typval_T *rettv)
{
    ఒకవేళ (లోపల_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	తిరిగి;

    పదం_ఎక్కించు_dump(argvars, rettv, TRUE);
}

/*
 * "term_dumpload(filename, options)" function
 */
    శూన్య
f_పదం_dumpload(typval_T *argvars, typval_T *rettv)
{
    ఒకవేళ (లోపల_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	తిరిగి;

    పదం_ఎక్కించు_dump(argvars, rettv, FALSE);
}

/*
 * "term_getaltscreen(buf)" function
 */
    శూన్య
f_పదం_getaltscreen(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    ఒకవేళ (లోపల_vim9script() && check_for_బఫరు_arg(argvars, 0) == FAIL)
	తిరిగి;

    buf = పదం_get_buf(argvars, "term_getaltscreen()");
    ఒకవేళ (buf == ఖాళీ)
	తిరిగి;
    rettv->vval.v_సంఖ్య = buf->b_పదం->tl_using_altscreen;
}

/*
 * "term_getattr(attr, name)" function
 */
    శూన్య
f_పదం_getattr(typval_T *argvars, typval_T *rettv)
{
    పూర్ణాంకం	    attr;
    size_t  i;
    char_u  *name;

    స్థిర నిర్మాణం {
	అక్షరం	    *name;
	పూర్ణాంకం	    attr;
    } attrs[] = {
	{"bold",      HL_BOLD},
	{"italic",    HL_ITALIC},
	{"underline", HL_UNDERLINE},
	{"strike",    HL_STRIKETHROUGH},
	{"reverse",   HL_INVERSE},
    };

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_సంఖ్య_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	తిరిగి;

    attr = tv_get_సంఖ్య(&argvars[0]);
    name = tv_get_string_chk(&argvars[1]);
    ఒకవేళ (name == ఖాళీ)
	తిరిగి;

    ఒకవేళ (attr > HL_ALL)
	attr = syn_attr2attr(attr);
    కొరకు (i = 0; i < ARRAY_నిడివి(attrs); ++i)
	ఒకవేళ (STRCMP(name, attrs[i].name) == 0)
	{
	    rettv->vval.v_సంఖ్య = (attr & attrs[i].attr) != 0 ? 1 : 0;
	    ఆపు;
	}
}

/*
 * "term_getcursor(buf)" function
 */
    శూన్య
f_పదం_getcursor(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    పదం_T	*పదం;
    list_T	*l;
    dict_T	*d;

    ఒకవేళ (rettv_list_alloc(rettv) == FAIL)
	తిరిగి;

    ఒకవేళ (లోపల_vim9script() && check_for_బఫరు_arg(argvars, 0) == FAIL)
	తిరిగి;

    buf = పదం_get_buf(argvars, "term_getcursor()");
    ఒకవేళ (buf == ఖాళీ)
	తిరిగి;
    పదం = buf->b_పదం;

    l = rettv->vval.v_list;
    list_append_సంఖ్య(l, పదం->tl_cursor_pos.అడ్డువరుస + 1);
    list_append_సంఖ్య(l, పదం->tl_cursor_pos.నిలువు + 1);

    d = dict_alloc();
    ఒకవేళ (d == ఖాళీ)
	తిరిగి;

    dict_కూడు_సంఖ్య(d, "visible", పదం->tl_cursor_visible);
    dict_కూడు_సంఖ్య(d, "blink", blink_state_is_inverted()
	    ? !పదం->tl_cursor_blink : పదం->tl_cursor_blink);
    dict_కూడు_సంఖ్య(d, "shape", పదం->tl_cursor_shape);
    dict_కూడు_string(d, "color", cursor_color_get(పదం->tl_cursor_color));
    ఒకవేళ (list_append_dict(l, d) == FAIL)
	dict_unref(d);
}

/*
 * "term_getjob(buf)" function
 */
    శూన్య
f_పదం_getjob(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    ఒకవేళ (లోపల_vim9script() && check_for_బఫరు_arg(argvars, 0) == FAIL)
	తిరిగి;

    buf = పదం_get_buf(argvars, "term_getjob()");
    ఒకవేళ (buf == ఖాళీ)
    {
	ఒకవేళ (లోపల_vim9script())
	{
	    rettv->v_type = VAR_పని;
	    rettv->vval.v_పని = ఖాళీ;
	}
	లేకపోతే
	{
	    rettv->v_type = VAR_SPECIAL;
	    rettv->vval.v_సంఖ్య = VVAL_NULL;
	}
	తిరిగి;
    }

    rettv->v_type = VAR_పని;
    rettv->vval.v_పని = buf->b_పదం->tl_పని;
    ఒకవేళ (rettv->vval.v_పని != ఖాళీ)
	++rettv->vval.v_పని->jv_refcount;
}

    స్థిర పూర్ణాంకం
get_అడ్డువరుస_సంఖ్య(typval_T *tv, పదం_T *పదం)
{
    ఒకవేళ (tv->v_type == VAR_STRING
	    && tv->vval.v_string != ఖాళీ
	    && STRCMP(tv->vval.v_string, ".") == 0)
	తిరిగి పదం->tl_cursor_pos.అడ్డువరుస;
    తిరిగి (పూర్ణాంకం)tv_get_సంఖ్య(tv) - 1;
}

/*
 * "term_getline(buf, row)" function
 */
    శూన్య
f_పదం_getline(typval_T *argvars, typval_T *rettv)
{
    buf_T	    *buf;
    పదం_T	    *పదం;
    పూర్ణాంకం		    అడ్డువరుస;

    rettv->v_type = VAR_STRING;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_బఫరు_arg(argvars, 0) == FAIL
		|| check_for_lnum_arg(argvars, 1) == FAIL))
	తిరిగి;

    buf = పదం_get_buf(argvars, "term_getline()");
    ఒకవేళ (buf == ఖాళీ)
	తిరిగి;
    పదం = buf->b_పదం;
    అడ్డువరుస = get_అడ్డువరుస_సంఖ్య(&argvars[1], పదం);

    ఒకవేళ (పదం->tl_vterm == ఖాళీ)
    {
	linenr_T  lnum = 0;
	size_t	  స్థానభ్రంశం = 0;
	పూర్ణాంకం	  sb_అడ్డువరుస = పదం->tl_scrollback_scrolled + అడ్డువరుస;
	sb_పంక్తి_T *పంక్తి;

	ఒకవేళ (sb_అడ్డువరుస < 0 || sb_అడ్డువరుస >= పదం->tl_scrollback.ga_len)
	    తిరిగి;
	పంక్తి = (sb_పంక్తి_T *)పదం->tl_scrollback.ga_దత్తాంశం + sb_అడ్డువరుస;

	scrollbackline_pos_లోపల_buf(పదం, sb_అడ్డువరుస, &lnum, ఖాళీ, &స్థానభ్రంశం);

	// vterm is finished, get the text from the buffer
	ఒకవేళ (lnum > 0 && lnum <= buf->b_ml.ml_పంక్తి_లెక్క)
	{
	    char_u *p = ml_get_buf(buf, lnum, FALSE);
	    ఒకవేళ (STRLEN(p) >= స్థానభ్రంశం + పంక్తి->sb_బైట్లు)
		rettv->vval.v_string = vim_strnsave(p + స్థానభ్రంశం, పంక్తి->sb_బైట్లు);
	}
    }
    లేకపోతే
    {
	VTermScreen	*screen = vterm_obtain_screen(పదం->tl_vterm);
	VTermRect	rect;
	పూర్ణాంకం		len;
	char_u		*p;

	ఒకవేళ (అడ్డువరుస < 0 || అడ్డువరుస >= పదం->tl_అడ్డువరుసలు)
	    తిరిగి;
	len = పదం->tl_నిలువుల_సంఖ్య * MB_MAXBYTES + 1;
	p = alloc(len);
	ఒకవేళ (p == ఖాళీ)
	    తిరిగి;
	rettv->vval.v_string = p;

	rect.ప్రారంభం_నిలువు = 0;
	rect.చివర_నిలువు = పదం->tl_నిలువుల_సంఖ్య;
	rect.ప్రారంభం_అడ్డువరుస = అడ్డువరుస;
	rect.చివర_అడ్డువరుస = అడ్డువరుస + 1;
	p[vterm_screen_get_పాఠ్యం(screen, (అక్షరం *)p, len, rect)] = NUL;
    }
}

/*
 * "term_getscrolled(buf)" function
 */
    శూన్య
f_పదం_getscrolled(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    ఒకవేళ (లోపల_vim9script() && check_for_బఫరు_arg(argvars, 0) == FAIL)
	తిరిగి;

    buf = పదం_get_buf(argvars, "term_getscrolled()");
    ఒకవేళ (buf == ఖాళీ)
	తిరిగి;
    rettv->vval.v_సంఖ్య = buf->b_పదం->tl_బఫరు_scrolled;
}

/*
 * "term_getsize(buf)" function
 */
    శూన్య
f_పదం_getsize(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    list_T	*l;

    ఒకవేళ (rettv_list_alloc(rettv) == FAIL)
	తిరిగి;

    ఒకవేళ (లోపల_vim9script() && check_for_బఫరు_arg(argvars, 0) == FAIL)
	తిరిగి;

    buf = పదం_get_buf(argvars, "term_getsize()");
    ఒకవేళ (buf == ఖాళీ)
	తిరిగి;

    l = rettv->vval.v_list;
    list_append_సంఖ్య(l, buf->b_పదం->tl_అడ్డువరుసలు);
    list_append_సంఖ్య(l, buf->b_పదం->tl_నిలువుల_సంఖ్య);
}

/*
 * "term_setsize(buf, rows, cols)" function
 */
    శూన్య
f_పదం_setsize(typval_T *argvars, typval_T *rettv వాడని)
{
    buf_T	*buf;
    పదం_T	*పదం;
    varnumber_T అడ్డువరుసలు_2, నిలువుల_సంఖ్య;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_బఫరు_arg(argvars, 0) == FAIL
		|| check_for_సంఖ్య_arg(argvars, 1) == FAIL
		|| check_for_సంఖ్య_arg(argvars, 2) == FAIL))
	తిరిగి;

    buf = పదం_get_buf(argvars, "term_setsize()");
    ఒకవేళ (buf == ఖాళీ)
    {
	emsg(_(e_not_terminal_బఫరు));
	తిరిగి;
    }
    ఒకవేళ (buf->b_పదం->tl_vterm == ఖాళీ)
	తిరిగి;
    పదం = buf->b_పదం;
    అడ్డువరుసలు_2 = tv_get_సంఖ్య(&argvars[1]);
    అడ్డువరుసలు_2 = అడ్డువరుసలు_2 <= 0 ? పదం->tl_అడ్డువరుసలు : అడ్డువరుసలు_2;
    నిలువుల_సంఖ్య = tv_get_సంఖ్య(&argvars[2]);
    నిలువుల_సంఖ్య = నిలువుల_సంఖ్య <= 0 ? పదం->tl_నిలువుల_సంఖ్య : నిలువుల_సంఖ్య;
    vterm_సమితి_కొలత(పదం->tl_vterm, అడ్డువరుసలు_2, నిలువుల_సంఖ్య);
    // handle_resize() will resize the windows

    // Get and remember the size we ended up with.  Update the pty.
    vterm_get_కొలత(పదం->tl_vterm, &పదం->tl_అడ్డువరుసలు, &పదం->tl_నిలువుల_సంఖ్య);
    పదం_report_winsize(పదం, పదం->tl_అడ్డువరుసలు, పదం->tl_నిలువుల_సంఖ్య);
}

/*
 * "term_getstatus(buf)" function
 */
    శూన్య
f_పదం_getstatus(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    పదం_T	*పదం;
    char_u	val[100];

    rettv->v_type = VAR_STRING;

    ఒకవేళ (లోపల_vim9script() && check_for_బఫరు_arg(argvars, 0) == FAIL)
	తిరిగి;

    buf = పదం_get_buf(argvars, "term_getstatus()");
    ఒకవేళ (buf == ఖాళీ)
	తిరిగి;
    పదం = buf->b_పదం;

    ఒకవేళ (పదం_పని_running(పదం))
	STRCPY(val, "running");
    లేకపోతే
	STRCPY(val, "finished");
    ఒకవేళ (పదం->tl_సాధారణ_బహుళకం)
	STRCAT(val, ",normal");
    rettv->vval.v_string = vim_strsave(val);
}

/*
 * "term_gettitle(buf)" function
 */
    శూన్య
f_పదం_gettitle(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    rettv->v_type = VAR_STRING;

    ఒకవేళ (లోపల_vim9script() && check_for_బఫరు_arg(argvars, 0) == FAIL)
	తిరిగి;

    buf = పదం_get_buf(argvars, "term_gettitle()");
    ఒకవేళ (buf == ఖాళీ)
	తిరిగి;

    ఒకవేళ (buf->b_పదం->tl_title != ఖాళీ)
	rettv->vval.v_string = vim_strsave(buf->b_పదం->tl_title);
}

/*
 * "term_gettty(buf)" function
 */
    శూన్య
f_పదం_gettty(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    char_u	*p = ఖాళీ;
    పూర్ణాంకం		అంకె = 0;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_బఫరు_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	తిరిగి;

    rettv->v_type = VAR_STRING;
    buf = పదం_get_buf(argvars, "term_gettty()");
    ఒకవేళ (buf == ఖాళీ)
	తిరిగి;
    ఒకవేళ (argvars[1].v_type != VAR_UNKNOWN)
	అంకె = tv_get_bool(&argvars[1]);

    ఎంపిక (అంకె)
    {
	స్థితి 0:
	    ఒకవేళ (buf->b_పదం->tl_పని != ఖాళీ)
		p = buf->b_పదం->tl_పని->jv_tty_బహిర్గతం;
	    ఆపు;
	స్థితి 1:
	    ఒకవేళ (buf->b_పదం->tl_పని != ఖాళీ)
		p = buf->b_పదం->tl_పని->jv_tty_లోపల;
	    ఆపు;
	అప్రమేయ:
	    semsg(_(e_invalid_వాదం_str), tv_get_string(&argvars[1]));
	    తిరిగి;
    }
    ఒకవేళ (p != ఖాళీ)
	rettv->vval.v_string = vim_strsave(p);
}

/*
 * "term_list()" function
 */
    శూన్య
f_పదం_list(typval_T *argvars వాడని, typval_T *rettv)
{
    పదం_T	*tp;
    list_T	*l;

    ఒకవేళ (rettv_list_alloc(rettv) == FAIL || మొదటి_పదం == ఖాళీ)
	తిరిగి;

    l = rettv->vval.v_list;
    FOR_ALL_TERMS(tp)
	ఒకవేళ (tp->tl_బఫరు != ఖాళీ)
	    ఒకవేళ (list_append_సంఖ్య(l,
				   (varnumber_T)tp->tl_బఫరు->b_fnum) == FAIL)
		తిరిగి;
}

/*
 * "term_scrape(buf, row)" function
 */
    శూన్య
f_పదం_scrape(typval_T *argvars, typval_T *rettv)
{
    buf_T	    *buf;
    VTermScreen	    *screen = ఖాళీ;
    VTermPos	    pos;
    list_T	    *l;
    పదం_T	    *పదం;
    char_u	    *p;
    sb_పంక్తి_T	    *పంక్తి;

    ఒకవేళ (rettv_list_alloc(rettv) == FAIL)
	తిరిగి;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_బఫరు_arg(argvars, 0) == FAIL
		|| check_for_lnum_arg(argvars, 1) == FAIL))
	తిరిగి;

    buf = పదం_get_buf(argvars, "term_scrape()");
    ఒకవేళ (buf == ఖాళీ)
	తిరిగి;
    పదం = buf->b_పదం;

    l = rettv->vval.v_list;
    pos.అడ్డువరుస = get_అడ్డువరుస_సంఖ్య(&argvars[1], పదం);

    ఒకవేళ (పదం->tl_vterm != ఖాళీ)
    {
	screen = vterm_obtain_screen(పదం->tl_vterm);
	ఒకవేళ (screen == ఖాళీ)  // can't really happen
	    తిరిగి;
	p = ఖాళీ;
	పంక్తి = ఖాళీ;
    }
    లేకపోతే
    {
	పూర్ణాంకం	  sb_అడ్డువరుస = పదం->tl_scrollback_scrolled + pos.అడ్డువరుస;
	linenr_T  lnum = 0;
	size_t	  స్థానభ్రంశం = 0;

	scrollbackline_pos_లోపల_buf(పదం, sb_అడ్డువరుస, &lnum, ఖాళీ, &స్థానభ్రంశం);

	ఒకవేళ (sb_అడ్డువరుస >= పదం->tl_scrollback.ga_len || lnum <= 0 || lnum > buf->b_ml.ml_పంక్తి_లెక్క)
	    తిరిగి;

	పంక్తి = (sb_పంక్తి_T *)పదం->tl_scrollback.ga_దత్తాంశం + sb_అడ్డువరుస;
	p = ml_get_buf(buf, lnum, FALSE);

	ఒకవేళ (STRLEN(p) < స్థానభ్రంశం + పంక్తి->sb_బైట్లు)
	    తిరిగి;

	p += స్థానభ్రంశం;
    }

    కొరకు (pos.నిలువు = 0; pos.నిలువు < పదం->tl_నిలువుల_సంఖ్య; )
    {
	dict_T		*dcell;
	పూర్ణాంకం		వెడల్పు;
	VTermScreenCellAttrs attrs;
	VTermColor	fg, bg;
	char_u		rgb[8];
	size_t		rgblen;
	char_u		mbs[MB_MAXBYTES * VTERM_MAX_CHARS_చొప్పున_CELL + 1];
	size_t		mbslen;
	పూర్ణాంకం		i;

	ఒకవేళ (screen == ఖాళీ)
	{
	    cellattr_T	*cellattr;

	    // vterm has finished, get the cell from scrollback
	    ఒకవేళ (pos.నిలువు >= పంక్తి->sb_నిలువుల_సంఖ్య)
		ఆపు;
	    cellattr = పంక్తి->sb_cells + pos.నిలువు;
	    వెడల్పు = cellattr->వెడల్పు;
	    attrs = cellattr->attrs;
	    fg = cellattr->fg;
	    bg = cellattr->bg;
	    mbslen = mb_ptr2len(p);
	    mch_memmove(mbs, p, mbslen);
	    mbs[mbslen] = NUL;
	    p += mbslen;
	}
	లేకపోతే
	{
	    VTermScreenCell cell;

	    ఒకవేళ (vterm_screen_get_cell(screen, pos, &cell) == 0)
		ఆపు;

	    mbslen = 0;
	    కొరకు (i = 0; i < VTERM_MAX_CHARS_చొప్పున_CELL; ++i)
	    {
		ఒకవేళ (cell.chars[i] == 0)
		    ఆపు;
		mbslen += (*utf_char2bytes)((పూర్ణాంకం)cell.chars[i], mbs + mbslen);
	    }
	    mbs[mbslen] = NUL;
	    వెడల్పు = cell.వెడల్పు;
	    attrs = cell.attrs;
	    fg = cell.fg;
	    bg = cell.bg;
	}
	dcell = dict_alloc();
	ఒకవేళ (dcell == ఖాళీ)
	    ఆపు;
	ఒకవేళ (list_append_dict(l, dcell) == FAIL)
	{
	    dict_unref(dcell);
	    ఆపు;
	}

	dict_కూడు_string_len(dcell, "chars", mbs, (పూర్ణాంకం)mbslen);

	rgblen = vim_snprintf_safelen((అక్షరం *)rgb, పరిమాణం(rgb),
	    "#%02x%02x%02x", fg.red, fg.green, fg.blue);
	dict_కూడు_string_len(dcell, "fg", rgb, (పూర్ణాంకం)rgblen);
	rgblen = vim_snprintf_safelen((అక్షరం *)rgb, పరిమాణం(rgb),
	    "#%02x%02x%02x", bg.red, bg.green, bg.blue);
	dict_కూడు_string_len(dcell, "bg", rgb, (పూర్ణాంకం)rgblen);

	dict_కూడు_సంఖ్య(dcell, "attr",
				      cell2attr(పదం, ఖాళీ, &attrs, &fg, &bg));
	dict_కూడు_సంఖ్య(dcell, "width", వెడల్పు);

	++pos.నిలువు;
	ఒకవేళ (వెడల్పు == 2)
	    ++pos.నిలువు;
    }
}

/*
 * "term_sendkeys(buf, keys)" function
 */
    శూన్య
f_పదం_sendkeys(typval_T *argvars, typval_T *rettv వాడని)
{
    buf_T	*buf;
    char_u	*సందేశం;
    పదం_T	*పదం;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_బఫరు_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	తిరిగి;

    buf = పదం_get_buf(argvars, "term_sendkeys()");
    ఒకవేళ (buf == ఖాళీ)
	తిరిగి;

    సందేశం = tv_get_string_chk(&argvars[1]);
    ఒకవేళ (సందేశం == ఖాళీ)
	తిరిగి;
    పదం = buf->b_పదం;
    ఒకవేళ (పదం->tl_vterm == ఖాళీ)
	తిరిగి;

    అయితే (*సందేశం != NUL)
    {
	పూర్ణాంకం c;

	ఒకవేళ (*సందేశం == K_SPECIAL && సందేశం[1] != NUL && సందేశం[2] != NUL)
	{
	    c = గమ్యం_SPECIAL(సందేశం[1], సందేశం[2]);
	    సందేశం += 3;
	}
	లేకపోతే
	{
	    c = PTR2CHAR(సందేశం);
	    సందేశం += MB_CPTR2LEN(సందేశం);
	}
	send_కీలకాలు_గమ్యం_పదం(పదం, c, 0, FALSE);
    }
}

#ఒకవేళ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * "term_getansicolors(buf)" function
 */
    శూన్య
f_పదం_getansicolors(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    పదం_T	*పదం;
    VTermState	*state;
    VTermColor  color;
    char_u	hexbuf[10];
    పూర్ణాంకం		సూచీ;
    list_T	*list;

    ఒకవేళ (rettv_list_alloc(rettv) == FAIL)
	తిరిగి;

    ఒకవేళ (లోపల_vim9script() && check_for_బఫరు_arg(argvars, 0) == FAIL)
	తిరిగి;

    buf = పదం_get_buf(argvars, "term_getansicolors()");
    ఒకవేళ (buf == ఖాళీ)
	తిరిగి;
    పదం = buf->b_పదం;
    ఒకవేళ (పదం->tl_vterm == ఖాళీ)
	తిరిగి;

    list = rettv->vval.v_list;
    state = vterm_obtain_state(పదం->tl_vterm);
    కొరకు (సూచీ = 0; సూచీ < 16; సూచీ++)
    {
	size_t	hexbuflen;

	vterm_state_get_palette_color(state, సూచీ, &color);
	hexbuflen = vim_snprintf_safelen((అక్షరం *)hexbuf, పరిమాణం(hexbuf),
	    "#%02x%02x%02x", color.red, color.green, color.blue);
	ఒకవేళ (list_append_string(list, hexbuf, (పూర్ణాంకం)hexbuflen) == FAIL)
	    తిరిగి;
    }
}

/*
 * "term_setansicolors(buf, list)" function
 */
    శూన్య
f_పదం_setansicolors(typval_T *argvars, typval_T *rettv వాడని)
{
    buf_T	*buf;
    పదం_T	*పదం;
    listitem_T	*li;
    పూర్ణాంకం		n = 0;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_బఫరు_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL))
	తిరిగి;

    buf = పదం_get_buf(argvars, "term_setansicolors()");
    ఒకవేళ (buf == ఖాళీ)
	తిరిగి;
    పదం = buf->b_పదం;
    ఒకవేళ (పదం->tl_vterm == ఖాళీ)
	తిరిగి;

    ఒకవేళ (check_for_nonnull_list_arg(argvars, 1) == FAIL)
	తిరిగి;

    ఒకవేళ (argvars[1].vval.v_list->lv_మొదటి == &range_list_అంశం
	    || argvars[1].vval.v_list->lv_len != 16)
    {
	semsg(_(e_invalid_విలువ_for_వాదం_str), "\"colors\"");
	తిరిగి;
    }

    ఒకవేళ (పదం->tl_palette == ఖాళీ)
	పదం->tl_palette = ALLOC_MULT(long_u, 16);
    ఒకవేళ (పదం->tl_palette == ఖాళీ)
	తిరిగి;

    FOR_ALL_LIST_వస్తువులు(argvars[1].vval.v_list, li)
    {
	char_u		*color_name;
	guicolor_T	guicolor;

	color_name = tv_get_string_chk(&li->li_tv);
	ఒకవేళ (color_name == ఖాళీ)
	    తిరిగి;

	guicolor = GUI_GET_COLOR(color_name);
	ఒకవేళ (guicolor == INVALCOLOR)
	{
	    semsg(_(e_cannot_allocate_color_str), color_name);
	    తిరిగి;
	}

	పదం->tl_palette[n++] = GUI_MCH_GET_RGB(guicolor);
    }

    పదం_update_palette(పదం);
}
#అయితే_ముగింపు

/*
 * "term_setapi(buf, api)" function
 */
    శూన్య
f_పదం_setapi(typval_T *argvars, typval_T *rettv వాడని)
{
    buf_T	*buf;
    పదం_T	*పదం;
    char_u	*api;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_బఫరు_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	తిరిగి;

    buf = పదం_get_buf(argvars, "term_setapi()");
    ఒకవేళ (buf == ఖాళీ)
	తిరిగి;
    పదం = buf->b_పదం;
    vim_free(పదం->tl_api);
    api = tv_get_string_chk(&argvars[1]);
    ఒకవేళ (api != ఖాళీ)
	పదం->tl_api = vim_strsave(api);
    లేకపోతే
	పదం->tl_api = ఖాళీ;
}

/*
 * "term_setrestore(buf, command)" function
 */
    శూన్య
f_పదం_setrestore(typval_T *argvars వాడని, typval_T *rettv వాడని)
{
#ఒకవేళ defined(FEAT_SESSION)
    buf_T	*buf;
    పదం_T	*పదం;
    char_u	*cmd;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_బఫరు_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	తిరిగి;

    buf = పదం_get_buf(argvars, "term_setrestore()");
    ఒకవేళ (buf == ఖాళీ)
	తిరిగి;
    పదం = buf->b_పదం;
    vim_free(పదం->tl_command);
    cmd = tv_get_string_chk(&argvars[1]);
    ఒకవేళ (cmd != ఖాళీ)
	పదం->tl_command = vim_strsave(cmd);
    లేకపోతే
	పదం->tl_command = ఖాళీ;
#అయితే_ముగింపు
}

/*
 * "term_setkill(buf, how)" function
 */
    శూన్య
f_పదం_setkill(typval_T *argvars వాడని, typval_T *rettv వాడని)
{
    buf_T	*buf;
    పదం_T	*పదం;
    char_u	*how;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_బఫరు_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	తిరిగి;

    buf = పదం_get_buf(argvars, "term_setkill()");
    ఒకవేళ (buf == ఖాళీ)
	తిరిగి;
    పదం = buf->b_పదం;
    vim_free(పదం->tl_kill);
    how = tv_get_string_chk(&argvars[1]);
    ఒకవేళ (how != ఖాళీ)
	పదం->tl_kill = vim_strsave(how);
    లేకపోతే
	పదం->tl_kill = ఖాళీ;
}

/*
 * "term_start(command, options)" function
 */
    శూన్య
f_పదం_ప్రారంభం(typval_T *argvars, typval_T *rettv)
{
    jobopt_T	opt;
    buf_T	*buf;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	తిరిగి;

    init_పని_options(&opt);
    ఒకవేళ (argvars[1].v_type != VAR_UNKNOWN
	    && get_పని_options(&argvars[1], &opt,
		JO_TIMEOUT_ALL + JO_STOPONEXIT
		    + JO_CALLBACK + JO_బహిర్గతం_CALLBACK + JO_ERR_CALLBACK
		    + JO_EXIT_CB + JO_CLOSE_CALLBACK + JO_బహిర్గతం_IO,
		JO2_పదం_NAME + JO2_పదం_ముగింపు + JO2_HIDDEN + JO2_పదం_OPENCMD
		    + JO2_పదం_నిలువుల_సంఖ్య + JO2_పదం_అడ్డువరుసలు + JO2_VERTICAL + JO2_CURWIN
		    + JO2_CWD + JO2_ENV + JO2_EOF_CHARS
		    + JO2_NORESTORE + JO2_పదం_KILL + JO2_పదం_HIGHLIGHT
		    + JO2_ANSI_COLORS + JO2_TTY_TYPE + JO2_పదం_API) == FAIL)
	తిరిగి;

    buf = పదం_ప్రారంభం(&argvars[0], ఖాళీ, &opt, 0);

    ఒకవేళ (buf != ఖాళీ && buf->b_పదం != ఖాళీ)
	rettv->vval.v_సంఖ్య = buf->b_fnum;
}

/*
 * "term_wait" function
 */
    శూన్య
f_పదం_wait(typval_T *argvars, typval_T *rettv వాడని)
{
    buf_T	*buf;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_బఫరు_arg(argvars, 0) == FAIL
		|| check_for_opt_సంఖ్య_arg(argvars, 1) == FAIL))
	తిరిగి;

    buf = పదం_get_buf(argvars, "term_wait()");
    ఒకవేళ (buf == ఖాళీ)
	తిరిగి;
    ఒకవేళ (buf->b_పదం->tl_పని == ఖాళీ)
    {
	ch_log(ఖాళీ, "term_wait(): no job to wait for");
	తిరిగి;
    }
    ఒకవేళ (buf->b_పదం->tl_పని->jv_channel == ఖాళీ)
	// channel is closed, nothing to do
	తిరిగి;

    // Get the job status, this will detect a job that finished.
    ఒకవేళ (!buf->b_పదం->tl_పని->jv_channel->ch_keep_open
	    && STRCMP(పని_స్థితి(buf->b_పదం->tl_పని), "dead") == 0)
    {
	// The job is dead, keep reading channel I/O until the channel is
	// closed. buf->b_term may become NULL if the terminal was closed while
	// waiting.
	ch_log(ఖాళీ, "term_wait(): waiting for channel to close");
	అయితే (buf->b_పదం != ఖాళీ && !buf->b_పదం->tl_channel_closed)
	{
	    పదం_flush_messages();

	    ui_delay(10L, FALSE);
	    ఒకవేళ (!buf_valid(buf))
		// If the terminal is closed when the channel is closed the
		// buffer disappears.
		ఆపు;
	    ఒకవేళ (buf->b_పదం == ఖాళీ || buf->b_పదం->tl_channel_closing)
		// came here from a close callback, only wait one time
		ఆపు;
	}

	పదం_flush_messages();
    }
    లేకపోతే
    {
	దీర్ఘ wait = 10L;

	పదం_flush_messages();

	// Wait for some time for any channel I/O.
	ఒకవేళ (argvars[1].v_type != VAR_UNKNOWN)
	    wait = tv_get_సంఖ్య(&argvars[1]);
	ui_delay(wait, TRUE);

	// Flushing messages on channels is hopefully sufficient.
	// TODO: is there a better way?
	పదం_flush_messages();
    }
}

/*
 * Called when a channel has sent all the lines to a terminal.
 * Send a CTRL-D to mark the end of the text.
 */
    శూన్య
పదం_send_eof(channel_T *ch)
{
    పదం_T	*పదం;

    FOR_ALL_TERMS(పదం)
	ఒకవేళ (పదం->tl_పని == ch->ch_పని)
	{
	    ఒకవేళ (పదం->tl_eof_chars != ఖాళీ)
	    {
		channel_send(ch, PART_లోపల, పదం->tl_eof_chars,
					(పూర్ణాంకం)STRLEN(పదం->tl_eof_chars), ఖాళీ);
		channel_send(ch, PART_లోపల, (char_u *)"\r", 1, ఖాళీ);
	    }
#నిర్వచితమైతే MSWIN
	    లేకపోతే
		// Default: CTRL-D
		channel_send(ch, PART_లోపల, (char_u *)"\004\r", 2, ఖాళీ);
#అయితే_ముగింపు
	}
}

#ఒకవేళ defined(FEAT_GUI)
    పని_T *
పదం_getjob(పదం_T *పదం)
{
    తిరిగి పదం != ఖాళీ ? పదం->tl_పని : ఖాళీ;
}
#అయితే_ముగింపు

#ఒకవేళ defined(MSWIN)

///////////////////////////////////////
// 2. MS-Windows implementation.

HRESULT (WINAPI *pCreatePseudoConsole)(COORD, HANDLE, HANDLE, DWORD, HPCON*);
HRESULT (WINAPI *pResizePseudoConsole)(HPCON, COORD);
HRESULT (WINAPI *pClosePseudoConsole)(HPCON);
BOOL (WINAPI *pInitializeProcThreadAttributeList)(LPPROC_THREAD_ATTRIBUTE_LIST, DWORD, DWORD, PSIZE_T);
BOOL (WINAPI *pUpdateProcThreadAttribute)(LPPROC_THREAD_ATTRIBUTE_LIST, DWORD, DWORD_PTR, PVOID, కొలత_T, PVOID, PSIZE_T);
శూన్య (WINAPI *pDeleteProcThreadAttributeList)(LPPROC_THREAD_ATTRIBUTE_LIST);

    స్థిర పూర్ణాంకం
dyn_conpty_init(పూర్ణాంకం verbose)
{
    స్థిర HMODULE	hKerneldll = ఖాళీ;
    పూర్ణాంకం			i;
    స్థిర నిర్మాణం
    {
	అక్షరం	*name;
	FARPROC	*ptr;
    } conpty_నమోదు[] =
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
	{ఖాళీ, ఖాళీ}
    };

    ఒకవేళ (!has_conpty_working())
    {
	ఒకవేళ (verbose)
	    emsg(_(e_conpty_is_not_అందుబాటు));
	తిరిగి FAIL;
    }

    // No need to initialize twice.
    ఒకవేళ (hKerneldll)
	తిరిగి OK;

    hKerneldll = vim_ఎక్కించు_Lib("kernel32.dll");
    కొరకు (i = 0; conpty_నమోదు[i].name != ఖాళీ
					&& conpty_నమోదు[i].ptr != ఖాళీ; ++i)
    {
	ఒకవేళ ((*conpty_నమోదు[i].ptr = (FARPROC)Get_Proc_చిరునామా(hKerneldll,
						conpty_నమోదు[i].name)) == ఖాళీ)
	{
	    ఒకవేళ (verbose)
		semsg(_(e_could_not_ఎక్కించు_library_function_str), conpty_నమోదు[i].name);
	    hKerneldll = ఖాళీ;
	    తిరిగి FAIL;
	}
    }

    తిరిగి OK;
}

    స్థిర పూర్ణాంకం
conpty_పదం_and_పని_init(
	పదం_T	    *పదం,
	typval_T    *argvar,
	అక్షరం	    **argv వాడని,
	jobopt_T    *opt,
	jobopt_T    *orig_opt)
{
    WCHAR	    *cmd_wchar = ఖాళీ;
    WCHAR	    *cmd_wchar_copy = ఖాళీ;
    WCHAR	    *cwd_wchar = ఖాళీ;
    WCHAR	    *env_wchar = ఖాళీ;
    channel_T	    *channel = ఖాళీ;
    పని_T	    *పని = ఖాళీ;
    HANDLE	    jo = ఖాళీ;
    garray_T	    ga_cmd, ga_env;
    char_u	    *cmd = ఖాళీ;
    HRESULT	    hr;
    COORD	    consize;
    కొలత_T	    breq;
    PROCESS_INFORMATION proc_info;
    HANDLE	    i_theirs = ఖాళీ;
    HANDLE	    o_theirs = ఖాళీ;
    HANDLE	    i_ours = ఖాళీ;
    HANDLE	    o_ours = ఖాళీ;
    అక్షరం	    *errmsg = ఖాళీ;

    ga_init2(&ga_cmd, పరిమాణం(అక్షరం*), 20);
    ga_init2(&ga_env, పరిమాణం(అక్షరం*), 20);

    ఒకవేళ (argvar->v_type == VAR_STRING)
    {
	cmd = argvar->vval.v_string;
    }
    లేకపోతే ఒకవేళ (argvar->v_type == VAR_LIST)
    {
	ఒకవేళ (win32_build_cmd(argvar->vval.v_list, &ga_cmd) == FAIL)
	    వెళ్ళు failed;
	cmd = ga_cmd.ga_దత్తాంశం;
    }
    ఒకవేళ (cmd == ఖాళీ || *cmd == NUL)
    {
	emsg(_(e_invalid_వాదం));
	వెళ్ళు failed;
    }

    పదం->tl_arg0_cmd = vim_strsave(cmd);

    cmd_wchar = enc_గమ్యం_utf16(cmd, ఖాళీ);

    ఒకవేళ (cmd_wchar != ఖాళీ)
    {
	// Request by CreateProcessW
	breq = wcslen(cmd_wchar) + 1 + 1;	// Addition of NUL by API
	cmd_wchar_copy = ALLOC_MULT(WCHAR, breq);
	wcsncpy(cmd_wchar_copy, cmd_wchar, breq - 1);
    }

    ga_clear(&ga_cmd);
    ఒకవేళ (cmd_wchar == ఖాళీ)
	వెళ్ళు failed;
    ఒకవేళ (opt->jo_cwd != ఖాళీ)
    {
	cwd_wchar = enc_గమ్యం_utf16(opt->jo_cwd, ఖాళీ);
	ఒకవేళ (cwd_wchar == ఖాళీ)
	    వెళ్ళు failed;
    }

    win32_build_env(opt->jo_env, &ga_env, TRUE);
    env_wchar = ga_env.ga_దత్తాంశం;

    ఒకవేళ (!CreatePipe(&i_theirs, &i_ours, ఖాళీ, 0))
	వెళ్ళు failed;
    ఒకవేళ (!CreatePipe(&o_ours, &o_theirs, ఖాళీ, 0))
	వెళ్ళు failed;

    consize.X = పదం->tl_నిలువుల_సంఖ్య;
    consize.Y = పదం->tl_అడ్డువరుసలు;
    hr = pCreatePseudoConsole(consize, i_theirs, o_theirs, 0,
							     &పదం->tl_conpty);
    ఒకవేళ (FAILED(hr))
	వెళ్ళు failed;

    పదం->tl_siex.StartupInfo.cb = పరిమాణం(పదం->tl_siex);

    // Explicitly invalidate std handles to prevent inheritance of
    // the debugger's stdout (e.g., in Visual Studio debugging sessions),
    // which could cause job output to go to the debugger instead of
    // the intended ConPTY, even with bInheritHandles set to FALSE in CreateProcess.
    పదం->tl_siex.StartupInfo.dwFlags = STARTF_USESTDHANDLES;
    పదం->tl_siex.StartupInfo.h_Std_ఆదానం = INVALID_HANDLE_విలువ;
    పదం->tl_siex.StartupInfo.hStdOutput = INVALID_HANDLE_విలువ;
    పదం->tl_siex.StartupInfo.hStdError = INVALID_HANDLE_విలువ;

    // Set up pipe inheritance safely: Vista or later.
    pInitializeProcThreadAttributeList(ఖాళీ, 1, 0, &breq);
    పదం->tl_siex.lpAttributeList = alloc(breq);
    ఒకవేళ (!పదం->tl_siex.lpAttributeList)
	వెళ్ళు failed;
    ఒకవేళ (!pInitializeProcThreadAttributeList(పదం->tl_siex.lpAttributeList, 1,
								     0, &breq))
	వెళ్ళు failed;
    ఒకవేళ (!pUpdateProcThreadAttribute(
	    పదం->tl_siex.lpAttributeList, 0,
	    PROC_THREAD_ATTRIBUTE_PSEUDOCONSOLE, పదం->tl_conpty,
	    పరిమాణం(HPCON), ఖాళీ, ఖాళీ))
	వెళ్ళు failed;

    channel = కూడు_channel();
    ఒకవేళ (channel == ఖాళీ)
	వెళ్ళు failed;

    పని = పని_alloc();
    ఒకవేళ (పని == ఖాళీ)
	వెళ్ళు failed;
    ఒకవేళ (argvar->v_type == VAR_STRING)
    {
	పూర్ణాంకం argc;

	build_argv_from_string(cmd, &పని->jv_argv, &argc);
    }
    లేకపోతే
    {
	పూర్ణాంకం argc;

	build_argv_from_list(argvar->vval.v_list, &పని->jv_argv, &argc);
    }

    ఒకవేళ (opt->jo_సమితి & JO_లోపల_BUF)
	పని->jv_లోపల_buf = buflist_findnr(opt->jo_io_buf[PART_లోపల]);

    ఒకవేళ (!CreateProcessW(ఖాళీ, cmd_wchar_copy, ఖాళీ, ఖాళీ, FALSE,
	    EXTENDED_STARTUPINFO_PRESENT | CREATE_UNICODE_ENVIRONMENT
	    | CREATE_SUSPENDED | CREATE_DEFAULT_ERROR_బహుళకం,
	    env_wchar, cwd_wchar,
	    &పదం->tl_siex.StartupInfo, &proc_info))
    {
	errmsg = GetWin32Error();
	వెళ్ళు failed;
    }

    CloseHandle(i_theirs);
    CloseHandle(o_theirs);

    channel_సమితి_pipes(channel,
	    (sock_T)i_ours,
	    (sock_T)o_ours,
	    (sock_T)o_ours);

    // Write lines with CR instead of NL.
    channel->ch_write_పాఠ్యం_బహుళకం = TRUE;

    // Use to explicitly delete anonymous pipe handle.
    // In addition, it is used to prevent the pipe from being closed when input
    // from a buffer etc. is finished.
    channel->ch_anonymous_pipe = TRUE;

    jo = Create_పని_Object(ఖాళీ, ఖాళీ);
    ఒకవేళ (jo == ఖాళీ)
	వెళ్ళు failed;

    ఒకవేళ (!కేటాయించు_Process_గమ్యం_పని_Object(jo, proc_info.hProcess))
    {
	// Failed, switch the way to terminate process with TerminateProcess.
	CloseHandle(jo);
	jo = ఖాళీ;
    }

    ResumeThread(proc_info.hThread);
    CloseHandle(proc_info.hThread);

    vim_free(cmd_wchar);
    vim_free(cmd_wchar_copy);
    vim_free(cwd_wchar);
    vim_free(env_wchar);

    ఒకవేళ (create_vterm(పదం, పదం->tl_అడ్డువరుసలు, పదం->tl_నిలువుల_సంఖ్య) == FAIL)
	వెళ్ళు failed;

# ఒకవేళ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    ఒకవేళ (పదం_use_palette())
    {
	ఒకవేళ (పదం->tl_palette != ఖాళీ)
	    సమితి_vterm_palette(పదం->tl_vterm, పదం->tl_palette);
	లేకపోతే
	    init_vterm_ansi_colors(పదం->tl_vterm);
    }
# endif

    channel_సమితి_పని(channel, పని, opt);
    పని_సమితి_options(పని, opt);

    పని->jv_channel = channel;
    పని->jv_proc_info = proc_info;
    పని->jv_పని_object = jo;
    పని->jv_స్థితి = పని_STARTED;
    పని->jv_tty_type = vim_strsave((char_u *)"conpty");
    ++పని->jv_refcount;
    పదం->tl_పని = పని;

    // Redirecting stdout and stderr doesn't work at the job level.  Instead
    // open the file here and handle it in.  opt->jo_io was changed in
    // setup_job_options(), use the original flags here.
    ఒకవేళ (orig_opt->jo_io[PART_బహిర్గతం] == JIO_FILE)
    {
	char_u *fname = opt->jo_io_name[PART_బహిర్గతం];

	ch_log(channel, "Opening output file %s", fname);
	పదం->tl_బహిర్గతం_fd = mch_fopen((అక్షరం *)fname, WRITEBIN);
	ఒకవేళ (పదం->tl_బహిర్గతం_fd == ఖాళీ)
	    semsg(_(e_cant_open_file_str), fname);
    }

    తిరిగి OK;

failed:
    ga_clear(&ga_cmd);
    ga_clear(&ga_env);
    vim_free(cmd_wchar);
    vim_free(cmd_wchar_copy);
    vim_free(cwd_wchar);
    ఒకవేళ (channel != ఖాళీ)
	channel_clear(channel);
    ఒకవేళ (పని != ఖాళీ)
    {
	పని->jv_channel = ఖాళీ;
	పని_cleanup(పని);
    }
    పదం->tl_పని = ఖాళీ;
    ఒకవేళ (jo != ఖాళీ)
	CloseHandle(jo);

    ఒకవేళ (పదం->tl_siex.lpAttributeList != ఖాళీ)
    {
	pDeleteProcThreadAttributeList(పదం->tl_siex.lpAttributeList);
	vim_free(పదం->tl_siex.lpAttributeList);
    }
    పదం->tl_siex.lpAttributeList = ఖాళీ;
    ఒకవేళ (o_theirs != ఖాళీ)
	CloseHandle(o_theirs);
    ఒకవేళ (o_ours != ఖాళీ)
	CloseHandle(o_ours);
    ఒకవేళ (i_ours != ఖాళీ)
	CloseHandle(i_ours);
    ఒకవేళ (i_theirs != ఖాళీ)
	CloseHandle(i_theirs);
    ఒకవేళ (పదం->tl_conpty != ఖాళీ)
	pClosePseudoConsole(పదం->tl_conpty);
    పదం->tl_conpty = ఖాళీ;
    // Propagate errors that occur in CreateProcess
    ఒకవేళ (errmsg)
	semsg("CreateProcess failed: %s", errmsg);
    తిరిగి FAIL;
}

    స్థిర శూన్య
conpty_పదం_report_winsize(పదం_T *పదం, పూర్ణాంకం అడ్డువరుసలు_2, పూర్ణాంకం నిలువుల_సంఖ్య)
{
    COORD consize;

    consize.X = నిలువుల_సంఖ్య;
    consize.Y = అడ్డువరుసలు_2;
    pResizePseudoConsole(పదం->tl_conpty, consize);
}

    స్థిర శూన్య
పదం_free_conpty(పదం_T *పదం)
{
    ఒకవేళ (పదం->tl_siex.lpAttributeList != ఖాళీ)
    {
	pDeleteProcThreadAttributeList(పదం->tl_siex.lpAttributeList);
	vim_free(పదం->tl_siex.lpAttributeList);
    }
    పదం->tl_siex.lpAttributeList = ఖాళీ;
    ఒకవేళ (పదం->tl_conpty != ఖాళీ)
	pClosePseudoConsole(పదం->tl_conpty);
    పదం->tl_conpty = ఖాళీ;
}

    పూర్ణాంకం
use_conpty(శూన్య)
{
    తిరిగి has_conpty;
}

# define WINPTY_SPAWN_FLAG_AUTO_SHUTDOWN 1ul
# define WINPTY_SPAWN_FLAG_EXIT_AFTER_SHUTDOWN 2ull
# define WINPTY_MOUSE_బహుళకం_FORCE		2

శూన్య* (*winpty_config_new)(UINT64, శూన్య*);
శూన్య* (*winpty_open)(శూన్య*, శూన్య*);
శూన్య* (*winpty_spawn_config_new)(UINT64, శూన్య*, LPCWSTR, శూన్య*, శూన్య*, శూన్య*);
BOOL (*winpty_spawn)(శూన్య*, శూన్య*, HANDLE*, HANDLE*, DWORD*, శూన్య*);
శూన్య (*winpty_config_సమితి_mouse_బహుళకం)(శూన్య*, పూర్ణాంకం);
శూన్య (*winpty_config_సమితి_initial_కొలత)(శూన్య*, పూర్ణాంకం, పూర్ణాంకం);
LPCWSTR (*winpty_conin_name)(శూన్య*);
LPCWSTR (*winpty_conout_name)(శూన్య*);
LPCWSTR (*winpty_conerr_name)(శూన్య*);
శూన్య (*winpty_free)(శూన్య*);
శూన్య (*winpty_config_free)(శూన్య*);
శూన్య (*winpty_spawn_config_free)(శూన్య*);
శూన్య (*winpty_error_free)(శూన్య*);
LPCWSTR (*winpty_error_సందేశం)(శూన్య*);
BOOL (*winpty_సమితి_కొలత)(శూన్య*, పూర్ణాంకం, పూర్ణాంకం, శూన్య*);
HANDLE (*winpty_agent_process)(శూన్య*);

# define WINPTY_DLL "winpty.dll"

స్థిర HINSTANCE hWinPtyDLL = ఖాళీ;

    స్థిర పూర్ణాంకం
dyn_winpty_init(పూర్ణాంకం verbose)
{
    పూర్ణాంకం i;
    స్థిర నిర్మాణం
    {
	అక్షరం	    *name;
	FARPROC	    *ptr;
    } winpty_నమోదు[] =
    {
	{"winpty_conerr_name", (FARPROC*)&winpty_conerr_name},
	{"winpty_config_free", (FARPROC*)&winpty_config_free},
	{"winpty_config_new", (FARPROC*)&winpty_config_new},
	{"winpty_config_set_mouse_mode",
				      (FARPROC*)&winpty_config_సమితి_mouse_బహుళకం},
	{"winpty_config_set_initial_size",
				    (FARPROC*)&winpty_config_సమితి_initial_కొలత},
	{"winpty_conin_name", (FARPROC*)&winpty_conin_name},
	{"winpty_conout_name", (FARPROC*)&winpty_conout_name},
	{"winpty_error_free", (FARPROC*)&winpty_error_free},
	{"winpty_free", (FARPROC*)&winpty_free},
	{"winpty_open", (FARPROC*)&winpty_open},
	{"winpty_spawn", (FARPROC*)&winpty_spawn},
	{"winpty_spawn_config_free", (FARPROC*)&winpty_spawn_config_free},
	{"winpty_spawn_config_new", (FARPROC*)&winpty_spawn_config_new},
	{"winpty_error_msg", (FARPROC*)&winpty_error_సందేశం},
	{"winpty_set_size", (FARPROC*)&winpty_సమితి_కొలత},
	{"winpty_agent_process", (FARPROC*)&winpty_agent_process},
	{ఖాళీ, ఖాళీ}
    };

    // No need to initialize twice.
    ఒకవేళ (hWinPtyDLL)
	తిరిగి OK;
    // Load winpty.dll, prefer using the 'winptydll' option, fall back to just
    // winpty.dll.
    ఒకవేళ (*p_winptydll != NUL)
	hWinPtyDLL = vim_ఎక్కించు_Lib((అక్షరం *)p_winptydll);
    ఒకవేళ (!hWinPtyDLL)
	hWinPtyDLL = vim_ఎక్కించు_Lib(WINPTY_DLL);
    ఒకవేళ (!hWinPtyDLL)
    {
	ఒకవేళ (verbose)
	    semsg(_(e_could_not_ఎక్కించు_library_str_str),
		    (*p_winptydll != NUL ? p_winptydll : (char_u *)WINPTY_DLL),
		    GetWin32Error());
	తిరిగి FAIL;
    }
    కొరకు (i = 0; winpty_నమోదు[i].name != ఖాళీ
					 && winpty_నమోదు[i].ptr != ఖాళీ; ++i)
    {
	ఒకవేళ ((*winpty_నమోదు[i].ptr = (FARPROC)Get_Proc_చిరునామా(hWinPtyDLL,
					      winpty_నమోదు[i].name)) == ఖాళీ)
	{
	    ఒకవేళ (verbose)
		semsg(_(e_could_not_ఎక్కించు_library_function_str), winpty_నమోదు[i].name);
	    hWinPtyDLL = ఖాళీ;
	    తిరిగి FAIL;
	}
    }

    తిరిగి OK;
}

    స్థిర పూర్ణాంకం
winpty_పదం_and_పని_init(
	పదం_T	    *పదం,
	typval_T    *argvar,
	అక్షరం	    **argv వాడని,
	jobopt_T    *opt,
	jobopt_T    *orig_opt)
{
    WCHAR	    *cmd_wchar = ఖాళీ;
    WCHAR	    *cwd_wchar = ఖాళీ;
    WCHAR	    *env_wchar = ఖాళీ;
    channel_T	    *channel = ఖాళీ;
    పని_T	    *పని = ఖాళీ;
    DWORD	    error;
    HANDLE	    jo = ఖాళీ;
    HANDLE	    బిడ్డ_process_handle;
    HANDLE	    బిడ్డ_thread_handle;
    శూన్య	    *winpty_err = ఖాళీ;
    శూన్య	    *spawn_config = ఖాళీ;
    garray_T	    ga_cmd, ga_env;
    char_u	    *cmd = ఖాళీ;

    ga_init2(&ga_cmd, పరిమాణం(అక్షరం*), 20);
    ga_init2(&ga_env, పరిమాణం(అక్షరం*), 20);

    ఒకవేళ (argvar->v_type == VAR_STRING)
    {
	cmd = argvar->vval.v_string;
    }
    లేకపోతే ఒకవేళ (argvar->v_type == VAR_LIST)
    {
	ఒకవేళ (win32_build_cmd(argvar->vval.v_list, &ga_cmd) == FAIL)
	    వెళ్ళు failed;
	cmd = ga_cmd.ga_దత్తాంశం;
    }
    ఒకవేళ (cmd == ఖాళీ || *cmd == NUL)
    {
	emsg(_(e_invalid_వాదం));
	వెళ్ళు failed;
    }

    పదం->tl_arg0_cmd = vim_strsave(cmd);

    cmd_wchar = enc_గమ్యం_utf16(cmd, ఖాళీ);
    ga_clear(&ga_cmd);
    ఒకవేళ (cmd_wchar == ఖాళీ)
	వెళ్ళు failed;
    ఒకవేళ (opt->jo_cwd != ఖాళీ)
    {
	cwd_wchar = enc_గమ్యం_utf16(opt->jo_cwd, ఖాళీ);
	ఒకవేళ (cwd_wchar == ఖాళీ)
	    వెళ్ళు failed;
    }

    win32_build_env(opt->jo_env, &ga_env, TRUE);
    env_wchar = ga_env.ga_దత్తాంశం;

    పదం->tl_winpty_config = winpty_config_new(0, &winpty_err);
    ఒకవేళ (పదం->tl_winpty_config == ఖాళీ)
	వెళ్ళు failed;

    winpty_config_సమితి_mouse_బహుళకం(పదం->tl_winpty_config,
						    WINPTY_MOUSE_బహుళకం_FORCE);
    winpty_config_సమితి_initial_కొలత(పదం->tl_winpty_config,
						 పదం->tl_నిలువుల_సంఖ్య, పదం->tl_అడ్డువరుసలు);
    పదం->tl_winpty = winpty_open(పదం->tl_winpty_config, &winpty_err);
    ఒకవేళ (పదం->tl_winpty == ఖాళీ)
	వెళ్ళు failed;

    spawn_config = winpty_spawn_config_new(
	    WINPTY_SPAWN_FLAG_AUTO_SHUTDOWN |
		WINPTY_SPAWN_FLAG_EXIT_AFTER_SHUTDOWN,
	    ఖాళీ,
	    cmd_wchar,
	    cwd_wchar,
	    env_wchar,
	    &winpty_err);
    ఒకవేళ (spawn_config == ఖాళీ)
	వెళ్ళు failed;

    channel = కూడు_channel();
    ఒకవేళ (channel == ఖాళీ)
	వెళ్ళు failed;

    పని = పని_alloc();
    ఒకవేళ (పని == ఖాళీ)
	వెళ్ళు failed;
    ఒకవేళ (argvar->v_type == VAR_STRING)
    {
	పూర్ణాంకం argc;

	build_argv_from_string(cmd, &పని->jv_argv, &argc);
    }
    లేకపోతే
    {
	పూర్ణాంకం argc;

	build_argv_from_list(argvar->vval.v_list, &పని->jv_argv, &argc);
    }

    ఒకవేళ (opt->jo_సమితి & JO_లోపల_BUF)
	పని->jv_లోపల_buf = buflist_findnr(opt->jo_io_buf[PART_లోపల]);

    ఒకవేళ (!winpty_spawn(పదం->tl_winpty, spawn_config, &బిడ్డ_process_handle,
	    &బిడ్డ_thread_handle, &error, &winpty_err))
	వెళ్ళు failed;

    channel_సమితి_pipes(channel,
	(sock_T)CreateFileW(
	    winpty_conin_name(పదం->tl_winpty),
	    GENERIC_WRITE, 0, ఖాళీ,
	    OPEN_EXISTING, 0, ఖాళీ),
	(sock_T)CreateFileW(
	    winpty_conout_name(పదం->tl_winpty),
	    GENERIC_READ, 0, ఖాళీ,
	    OPEN_EXISTING, 0, ఖాళీ),
	(sock_T)CreateFileW(
	    winpty_conerr_name(పదం->tl_winpty),
	    GENERIC_READ, 0, ఖాళీ,
	    OPEN_EXISTING, 0, ఖాళీ));

    // Write lines with CR instead of NL.
    channel->ch_write_పాఠ్యం_బహుళకం = TRUE;

    jo = Create_పని_Object(ఖాళీ, ఖాళీ);
    ఒకవేళ (jo == ఖాళీ)
	వెళ్ళు failed;

    ఒకవేళ (!కేటాయించు_Process_గమ్యం_పని_Object(jo, బిడ్డ_process_handle))
    {
	// Failed, switch the way to terminate process with TerminateProcess.
	CloseHandle(jo);
	jo = ఖాళీ;
    }

    winpty_spawn_config_free(spawn_config);
    vim_free(cmd_wchar);
    vim_free(cwd_wchar);
    vim_free(env_wchar);

    ఒకవేళ (create_vterm(పదం, పదం->tl_అడ్డువరుసలు, పదం->tl_నిలువుల_సంఖ్య) == FAIL)
	వెళ్ళు failed;

# ఒకవేళ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    ఒకవేళ (పదం_use_palette())
    {
	ఒకవేళ (పదం->tl_palette != ఖాళీ)
	    సమితి_vterm_palette(పదం->tl_vterm, పదం->tl_palette);
	లేకపోతే
	    init_vterm_ansi_colors(పదం->tl_vterm);
    }
# endif

    channel_సమితి_పని(channel, పని, opt);
    పని_సమితి_options(పని, opt);

    పని->jv_channel = channel;
    పని->jv_proc_info.hProcess = బిడ్డ_process_handle;
    పని->jv_proc_info.dwProcessId = GetProcessId(బిడ్డ_process_handle);
    పని->jv_పని_object = jo;
    పని->jv_స్థితి = పని_STARTED;
    పని->jv_tty_లోపల = utf16_గమ్యం_enc(
	    (short_u *)winpty_conin_name(పదం->tl_winpty), ఖాళీ);
    పని->jv_tty_బహిర్గతం = utf16_గమ్యం_enc(
	    (short_u *)winpty_conout_name(పదం->tl_winpty), ఖాళీ);
    పని->jv_tty_type = vim_strsave((char_u *)"winpty");
    ++పని->jv_refcount;
    పదం->tl_పని = పని;

    // Redirecting stdout and stderr doesn't work at the job level.  Instead
    // open the file here and handle it in.  opt->jo_io was changed in
    // setup_job_options(), use the original flags here.
    ఒకవేళ (orig_opt->jo_io[PART_బహిర్గతం] == JIO_FILE)
    {
	char_u *fname = opt->jo_io_name[PART_బహిర్గతం];

	ch_log(channel, "Opening output file %s", fname);
	పదం->tl_బహిర్గతం_fd = mch_fopen((అక్షరం *)fname, WRITEBIN);
	ఒకవేళ (పదం->tl_బహిర్గతం_fd == ఖాళీ)
	    semsg(_(e_cant_open_file_str), fname);
    }

    తిరిగి OK;

failed:
    ga_clear(&ga_cmd);
    ga_clear(&ga_env);
    vim_free(cmd_wchar);
    vim_free(cwd_wchar);
    ఒకవేళ (spawn_config != ఖాళీ)
	winpty_spawn_config_free(spawn_config);
    ఒకవేళ (channel != ఖాళీ)
	channel_clear(channel);
    ఒకవేళ (పని != ఖాళీ)
    {
	పని->jv_channel = ఖాళీ;
	పని_cleanup(పని);
    }
    పదం->tl_పని = ఖాళీ;
    ఒకవేళ (jo != ఖాళీ)
	CloseHandle(jo);
    ఒకవేళ (పదం->tl_winpty != ఖాళీ)
	winpty_free(పదం->tl_winpty);
    పదం->tl_winpty = ఖాళీ;
    ఒకవేళ (పదం->tl_winpty_config != ఖాళీ)
	winpty_config_free(పదం->tl_winpty_config);
    పదం->tl_winpty_config = ఖాళీ;
    ఒకవేళ (winpty_err != ఖాళీ)
    {
	అక్షరం *సందేశం = (అక్షరం *)utf16_గమ్యం_enc(
				(short_u *)winpty_error_సందేశం(winpty_err), ఖాళీ);

	ఒకవేళ (సందేశం != ఖాళీ)
	{
	    emsg(సందేశం);
	    vim_free(సందేశం);
	}
	winpty_error_free(winpty_err);
    }
    తిరిగి FAIL;
}

/*
 * Create a new terminal of "rows" by "cols" cells.
 * Store a reference in "term".
 * Return OK or FAIL.
 */
    స్థిర పూర్ణాంకం
పదం_and_పని_init(
	పదం_T	    *పదం,
	typval_T    *argvar,
	అక్షరం	    **argv,
	jobopt_T    *opt,
	jobopt_T    *orig_opt)
{
    పూర్ణాంకం		    use_winpty = FALSE;
    పూర్ణాంకం		    use_conpty = FALSE;
    పూర్ణాంకం		    tty_type = *p_twt;

    has_winpty = dyn_winpty_init(FALSE) != FAIL ? TRUE : FALSE;
    has_conpty = dyn_conpty_init(FALSE) != FAIL ? TRUE : FALSE;

    ఒకవేళ (!has_winpty && !has_conpty)
	// If neither is available give the errors for winpty, since when
	// conpty is not available it can't be installed either.
	తిరిగి dyn_winpty_init(TRUE);

    ఒకవేళ (opt->jo_tty_type != NUL)
	tty_type = opt->jo_tty_type;

    ఒకవేళ (tty_type == NUL)
    {
	ఒకవేళ (has_conpty && (is_conpty_stable() || !has_winpty))
	    use_conpty = TRUE;
	లేకపోతే ఒకవేళ (has_winpty)
	    use_winpty = TRUE;
	// else: error
    }
    లేకపోతే ఒకవేళ (tty_type == 'w')	// winpty
    {
	ఒకవేళ (has_winpty)
	    use_winpty = TRUE;
    }
    లేకపోతే ఒకవేళ (tty_type == 'c')	// conpty
    {
	ఒకవేళ (has_conpty)
	    use_conpty = TRUE;
	లేకపోతే
	    తిరిగి dyn_conpty_init(TRUE);
    }

    ఒకవేళ (use_conpty)
	తిరిగి conpty_పదం_and_పని_init(పదం, argvar, argv, opt, orig_opt);

    ఒకవేళ (use_winpty)
	తిరిగి winpty_పదం_and_పని_init(పదం, argvar, argv, opt, orig_opt);

    // error
    తిరిగి dyn_winpty_init(TRUE);
}

    స్థిర పూర్ణాంకం
create_pty_only(పదం_T *పదం, jobopt_T *options)
{
    HANDLE	    h_Pipe_లోపల = INVALID_HANDLE_విలువ;
    HANDLE	    h_Pipe_బహిర్గతం = INVALID_HANDLE_విలువ;
    అక్షరం	    లోపల_name[80], బహిర్గతం_name[80];
    channel_T	    *channel = ఖాళీ;

    ఒకవేళ (create_vterm(పదం, పదం->tl_అడ్డువరుసలు, పదం->tl_నిలువుల_సంఖ్య) == FAIL)
	తిరిగి FAIL;

    vim_snprintf(లోపల_name, పరిమాణం(లోపల_name), "\\\\.\\pipe\\vim-%d-in-%d",
	    GetCurrentProcessId(),
	    curbuf->b_fnum);
    h_Pipe_లోపల = CreateNamedPipe(లోపల_name, PIPE_ACCESS_OUTBOUND,
	    PIPE_TYPE_సందేశం | PIPE_NOWAIT,
	    PIPE_UNLIMITED_INSTANCES,
	    0, 0, NMPWAIT_NOWAIT, ఖాళీ);
    ఒకవేళ (h_Pipe_లోపల == INVALID_HANDLE_విలువ)
	వెళ్ళు failed;

    vim_snprintf(బహిర్గతం_name, పరిమాణం(బహిర్గతం_name), "\\\\.\\pipe\\vim-%d-out-%d",
	    GetCurrentProcessId(),
	    curbuf->b_fnum);
    h_Pipe_బహిర్గతం = CreateNamedPipe(బహిర్గతం_name, PIPE_ACCESS_INBOUND,
	    PIPE_TYPE_సందేశం | PIPE_NOWAIT,
	    PIPE_UNLIMITED_INSTANCES,
	    0, 0, 0, ఖాళీ);
    ఒకవేళ (h_Pipe_బహిర్గతం == INVALID_HANDLE_విలువ)
	వెళ్ళు failed;

    ConnectNamedPipe(h_Pipe_లోపల, ఖాళీ);
    ConnectNamedPipe(h_Pipe_బహిర్గతం, ఖాళీ);

    పదం->tl_పని = పని_alloc();
    ఒకవేళ (పదం->tl_పని == ఖాళీ)
	వెళ్ళు failed;
    ++పదం->tl_పని->jv_refcount;

    // behave like the job is already finished
    పదం->tl_పని->jv_స్థితి = పని_ముగిసిన;

    channel = కూడు_channel();
    ఒకవేళ (channel == ఖాళీ)
	వెళ్ళు failed;
    పదం->tl_పని->jv_channel = channel;
    channel->ch_keep_open = TRUE;
    channel->ch_named_pipe = TRUE;

    channel_సమితి_pipes(channel,
	(sock_T)h_Pipe_లోపల,
	(sock_T)h_Pipe_బహిర్గతం,
	(sock_T)h_Pipe_బహిర్గతం);
    channel_సమితి_పని(channel, పదం->tl_పని, options);
    పదం->tl_పని->jv_tty_లోపల = vim_strsave((char_u*)లోపల_name);
    పదం->tl_పని->jv_tty_బహిర్గతం = vim_strsave((char_u*)బహిర్గతం_name);

    తిరిగి OK;

failed:
    ఒకవేళ (h_Pipe_లోపల != ఖాళీ)
	CloseHandle(h_Pipe_లోపల);
    ఒకవేళ (h_Pipe_బహిర్గతం != ఖాళీ)
	CloseHandle(h_Pipe_బహిర్గతం);
    తిరిగి FAIL;
}

/*
 * Free the terminal emulator part of "term".
 */
    స్థిర శూన్య
పదం_free_vterm(పదం_T *పదం)
{
    పదం_free_conpty(పదం);
    ఒకవేళ (పదం->tl_winpty != ఖాళీ)
	winpty_free(పదం->tl_winpty);
    పదం->tl_winpty = ఖాళీ;
    ఒకవేళ (పదం->tl_winpty_config != ఖాళీ)
	winpty_config_free(పదం->tl_winpty_config);
    పదం->tl_winpty_config = ఖాళీ;
    ఒకవేళ (పదం->tl_vterm != ఖాళీ)
	vterm_free(పదం->tl_vterm);
    పదం->tl_vterm = ఖాళీ;
}

/*
 * Report the size to the terminal.
 */
    స్థిర శూన్య
పదం_report_winsize(పదం_T *పదం, పూర్ణాంకం అడ్డువరుసలు_2, పూర్ణాంకం నిలువుల_సంఖ్య)
{
    ఒకవేళ (పదం->tl_conpty)
	conpty_పదం_report_winsize(పదం, అడ్డువరుసలు_2, నిలువుల_సంఖ్య);
    ఒకవేళ (పదం->tl_winpty)
	winpty_సమితి_కొలత(పదం->tl_winpty, నిలువుల_సంఖ్య, అడ్డువరుసలు_2, ఖాళీ);
}

    పూర్ణాంకం
terminal_enabled(శూన్య)
{
    తిరిగి dyn_winpty_init(FALSE) == OK || dyn_conpty_init(FALSE) == OK;
}

#లేకపోతే

///////////////////////////////////////
// 3. Unix-like implementation.

/*
 * Create a new terminal of "rows" by "cols" cells.
 * Start job for "cmd".
 * Store the pointers in "term".
 * When "argv" is not NULL then "argvar" is not used.
 * Return OK or FAIL.
 */
    స్థిర పూర్ణాంకం
పదం_and_పని_init(
	పదం_T	    *పదం,
	typval_T    *argvar,
	అక్షరం	    **argv,
	jobopt_T    *opt,
	jobopt_T    *orig_opt వాడని)
{
    పదం->tl_arg0_cmd = ఖాళీ;

    ఒకవేళ (create_vterm(పదం, పదం->tl_అడ్డువరుసలు, పదం->tl_నిలువుల_సంఖ్య) == FAIL)
	తిరిగి FAIL;

# ఒకవేళ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    ఒకవేళ (పదం_use_palette())
    {
	ఒకవేళ (పదం->tl_palette != ఖాళీ)
	    సమితి_vterm_palette(పదం->tl_vterm, పదం->tl_palette);
	లేకపోతే
	    init_vterm_ansi_colors(పదం->tl_vterm);
    }
# endif

    // This may change a string in "argvar".
    పదం->tl_పని = పని_ప్రారంభం(argvar, argv, opt, &పదం->tl_పని);
    ఒకవేళ (పదం->tl_పని != ఖాళీ)
	++పదం->tl_పని->jv_refcount;

    తిరిగి పదం->tl_పని != ఖాళీ
	&& పదం->tl_పని->jv_channel != ఖాళీ
	&& పదం->tl_పని->jv_స్థితి != పని_FAILED ? OK : FAIL;
}

    స్థిర పూర్ణాంకం
create_pty_only(పదం_T *పదం, jobopt_T *opt)
{
    ఒకవేళ (create_vterm(పదం, పదం->tl_అడ్డువరుసలు, పదం->tl_నిలువుల_సంఖ్య) == FAIL)
	తిరిగి FAIL;

    పదం->tl_పని = పని_alloc();
    ఒకవేళ (పదం->tl_పని == ఖాళీ)
	తిరిగి FAIL;
    ++పదం->tl_పని->jv_refcount;

    // behave like the job is already finished
    పదం->tl_పని->jv_స్థితి = పని_ముగిసిన;

    తిరిగి mch_create_pty_channel(పదం->tl_పని, opt);
}

/*
 * Free the terminal emulator part of "term".
 */
    స్థిర శూన్య
పదం_free_vterm(పదం_T *పదం)
{
    ఒకవేళ (పదం->tl_vterm != ఖాళీ)
	vterm_free(పదం->tl_vterm);
    పదం->tl_vterm = ఖాళీ;
}

/*
 * Report the size to the terminal.
 */
    స్థిర శూన్య
పదం_report_winsize(పదం_T *పదం, పూర్ణాంకం అడ్డువరుసలు_2, పూర్ణాంకం నిలువుల_సంఖ్య)
{
    // Use an ioctl() to report the new window size to the job.
    ఒకవేళ (పదం->tl_పని == ఖాళీ || పదం->tl_పని->jv_channel == ఖాళీ)
	తిరిగి;

    పూర్ణాంకం fd = -1;
    పూర్ణాంకం part;

    కొరకు (part = PART_బహిర్గతం; part < PART_లెక్క; ++part)
    {
	fd = పదం->tl_పని->jv_channel->ch_part[part].ch_fd;
	ఒకవేళ (mch_isatty(fd))
	    ఆపు;
    }
    ఒకవేళ (part < PART_లెక్క && mch_report_winsize(fd, అడ్డువరుసలు_2, నిలువుల_సంఖ్య) == OK)
	mch_signal_పని(పదం->tl_పని, (char_u *)"winch");
}

#అయితే_ముగింపు

#అయితే_ముగింపు // FEAT_TERMINAL
