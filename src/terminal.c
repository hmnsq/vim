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

#إذا defined(FEAT_TERMINAL)

#جيڪڏهن_وصف_نه_ٿيل MIN
# define MIN(x,y) ((x) < (y) ? (x) : (y))
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل MAX
# define MAX(x,y) ((x) > (y) ? (x) : (y))
#جيڪڏهن_پورو

#ضمن "libvterm/include/vterm.h"

// This is VTermScreenCell without the characters, thus much smaller.
تعريفنوع بنية {
    VTermScreenCellAttrs	attrs;
    حرف			ويڪر;
    VTermColor			fg;
    VTermColor			bg;
} cellattr_T;

تعريفنوع بنية sb_سٽ_S {
    صحيح		sb_کانن_ڳڻپ;	// can differ per line
    صحيح		sb_بائيٽون;	// length in bytes of text
    cellattr_T	*sb_cells;	// allocated
    cellattr_T	sb_fill_attr;	// for short line
    char_u	*sb_متن;	// for tl_scrollback_postponed
    char_u	continuation;
} sb_سٽ_T;

#جيڪڏهن_وصف_ٿيل MSWIN
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
#جيڪڏهن_پورو

// typedef term_T in structs.h
بنية terminal_S {
    رڪن_T	*tl_ايندڙ;

    VTerm	*tl_vterm;
    ڪم_T	*tl_ڪم;
    buf_T	*tl_بفر;
#إذا defined(FEAT_GUI)
    صحيح		tl_system;	// when non-zero used for :!cmd output
    صحيح		tl_toprow;	// row with first line of system terminal
#جيڪڏهن_پورو

    // Set when setting the size of a vterm, reset after redrawing.
    صحيح		tl_vterm_سائيز_بدليو;

    صحيح		tl_عام_گهڻائي; // TRUE: Terminal-Normal mode
    صحيح		tl_channel_closing;
    صحيح		tl_channel_closed;
    صحيح		tl_channel_recently_closed; // still need to handle tl_finish

    صحيح		tl_پڄاڻي;
#وصف_ڪريو TL_پڄاڻي_UNSET	    NUL
#وصف_ڪريو TL_پڄاڻي_CLOSE	    'c'	// ++close or :terminal without argument
#وصف_ڪريو TL_پڄاڻي_NOCLOSE   'n'	// ++noclose
#وصف_ڪريو TL_پڄاڻي_OPEN	    'o'	// ++open
    char_u	*tl_opencmd;
    char_u	*tl_eof_chars;
    char_u	*tl_api;	// prefix for terminal API function

    char_u	*tl_arg0_cmd;	// To format the status bar

#جيڪڏهن_وصف_ٿيل MSWIN
    فراغ	*tl_winpty_config;
    فراغ	*tl_winpty;

    HPCON	tl_conpty;
    DYN_STARTUPINFOEXW tl_siex;	// Structure that always needs to be hold

    FILE	*tl_ٻاهريون_fd;
#جيڪڏهن_پورو
#إذا defined(FEAT_SESSION)
    char_u	*tl_command;
#جيڪڏهن_پورو
    char_u	*tl_kill;

    // last known vterm size
    صحيح		tl_صفون;
    صحيح		tl_کانن_ڳڻپ;

    char_u	*tl_title; // NULL or allocated
    char_u	*tl_حالت_متن; // NULL or allocated

    // Range of screen rows to update.  Zero based.
    صحيح		tl_dirty_صف_شروعات; // MAX_ROW if nothing dirty
    صحيح		tl_dirty_صف_پڇاڙي;   // row below last one to update
    صحيح		tl_dirty_snapshot;  // text updated after making snapshot
#جيڪڏهن_وصف_ٿيل FEAT_TIMERS
    صحيح		tl_timer_سيٽ;
    proftime_T	tl_timer_due;
#جيڪڏهن_پورو
    صحيح		tl_postponed_scroll;	// to be scrolled up

    garray_T	tl_scrollback;
    صحيح		tl_scrollback_scrolled;
    garray_T	tl_scrollback_postponed;
    صحيح		tl_scrollback_snapshot;
    صحيح		tl_بفر_scrolled;

    char_u	*tl_highlight_name; // replaces "Terminal"; allocated

    cellattr_T	tl_default_color;

    linenr_T	tl_چوٽي_diff_صفون;   // rows of top diff file or zero
    linenr_T	tl_bot_diff_صفون;   // rows of bottom diff file

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

#وصف_ڪريو TMODE_ONCE 1	    // CTRL-\ CTRL-N used
#وصف_ڪريو TMODE_LOOP 2	    // CTRL-W N used

/*
 * List of all active terminals.
 */
ساكن رڪن_T *پهريون_رڪن = عدم;

// Terminal active in terminal_loop().
ساكن رڪن_T *اندريون_terminal_loop = عدم;

#جيڪڏهن_وصف_ٿيل MSWIN
ساكن BOOL has_winpty = FALSE;
ساكن BOOL has_conpty = FALSE;
#جيڪڏهن_پورو

#وصف_ڪريو MAX_صف 999999	    // used for tl_dirty_row_end to update all rows
#وصف_ڪريو ڪنجي_BUF_LEN 200

#وصف_ڪريو FOR_ALL_TERMS(رڪن)	\
    لكل ((رڪن) = پهريون_رڪن; (رڪن) != عدم; (رڪن) = (رڪن)->tl_ايندڙ)

/*
 * Functions with separate implementation for MS-Windows and Unix-like systems.
 */
ساكن صحيح رڪن_and_ڪم_init(رڪن_T *رڪن, typval_T *argvar, حرف **argv, jobopt_T *opt, jobopt_T *orig_opt);
ساكن صحيح create_pty_only(رڪن_T *رڪن, jobopt_T *opt);
ساكن فراغ رڪن_report_winsize(رڪن_T *رڪن, صحيح صفون_2, صحيح کانن_ڳڻپ);
ساكن فراغ رڪن_free_vterm(رڪن_T *رڪن);
#جيڪڏهن_وصف_ٿيل FEAT_GUI
ساكن فراغ update_system_رڪن(رڪن_T *رڪن);
#جيڪڏهن_پورو

ساكن فراغ handle_postponed_scrollback(رڪن_T *رڪن);

// The character that we know (or assume) that the terminal expects for the
// backspace key.
ساكن صحيح رڪن_backspace_char = BS;

// Store the last set and the desired cursor properties, so that we only update
// them when needed.  Doing it unnecessary may result in flicker.
ساكن char_u	*آخري_سيٽ_cursor_color = عدم;
ساكن char_u	*desired_cursor_color = عدم;
ساكن صحيح	آخري_سيٽ_cursor_shape = -1;
ساكن صحيح	desired_cursor_shape = -1;
ساكن صحيح	آخري_سيٽ_cursor_blink = -1;
ساكن صحيح	desired_cursor_blink = -1;


///////////////////////////////////////
// 1. Generic code for all systems.

    ساكن صحيح
cursor_color_equal(char_u *lhs_color, char_u *rhs_color)
{
    إذا (lhs_color != عدم && rhs_color != عدم)
	أرجع STRCMP(lhs_color, rhs_color) == 0;
    أرجع lhs_color == عدم && rhs_color == عدم;
}

    ساكن فراغ
cursor_color_copy(char_u **منزل_color, char_u *from_color)
{
    // Avoid a free & alloc if the value is already right.
    إذا (cursor_color_equal(*منزل_color, from_color))
	أرجع;
    vim_free(*منزل_color);
    *منزل_color = (from_color == عدم) ? عدم : vim_strsave(from_color);
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
parse_termwinsize(win_T *wp, صحيح *صفون_2, صحيح *کانن_ڳڻپ)
{
    صحيح	minsize = FALSE;

    *صفون_2 = 0;
    *کانن_ڳڻپ = 0;

    إذا (*wp->w_p_tws == NUL)
	أرجع FALSE;

    char_u *p = vim_strchr(wp->w_p_tws, 'x');

    // Syntax of value was already checked when it's set.
    إذا (p == عدم)
    {
	minsize = TRUE;
	p = vim_strchr(wp->w_p_tws, '*');
    }
    *صفون_2 = atoi((حرف *)wp->w_p_tws);
    *کانن_ڳڻپ = atoi((حرف *)p + 1);
    إذا (*صفون_2 > VTERM_MAX_صفون)
	*صفون_2 = VTERM_MAX_صفون;
    إذا (*کانن_ڳڻپ > VTERM_MAX_کانن_ڳڻپ)
	*کانن_ڳڻپ = VTERM_MAX_کانن_ڳڻپ;
    أرجع minsize;
}

/*
 * Determine the terminal size from 'termwinsize' and the current window.
 */
    ساكن فراغ
سيٽ_رڪن_and_win_سائيز(رڪن_T *رڪن, jobopt_T *opt)
{
    صحيح صفون_2, کانن_ڳڻپ;
    صحيح	minsize;

#جيڪڏهن_وصف_ٿيل FEAT_GUI
    إذا (رڪن->tl_system)
    {
	// Use the entire screen (excluding the tabpanel area) for the system
	// commands.  However, it will start at the command line and scroll up
	// as needed, using tl_toprow.
	رڪن->tl_صفون = صفون;
	رڪن->tl_کانن_ڳڻپ = topframe->fr_ويڪر;
	أرجع;
    }
#جيڪڏهن_پورو
    رڪن->tl_صفون = curwin->w_اوچائي;
    رڪن->tl_کانن_ڳڻپ = curwin->w_ويڪر;

    minsize = parse_termwinsize(curwin, &صفون_2, &کانن_ڳڻپ);
    إذا (minsize)
    {
	إذا (رڪن->tl_صفون < صفون_2)
	    رڪن->tl_صفون = صفون_2;
	إذا (رڪن->tl_کانن_ڳڻپ < کانن_ڳڻپ)
	    رڪن->tl_کانن_ڳڻپ = کانن_ڳڻپ;
    }
    إذا ((opt->jo_set2 & JO2_رڪن_صفون))
	رڪن->tl_صفون = opt->jo_رڪن_صفون;
    وإلا إذا (صفون_2 != 0)
	رڪن->tl_صفون = صفون_2;
    إذا ((opt->jo_set2 & JO2_رڪن_کانن_ڳڻپ))
	رڪن->tl_کانن_ڳڻپ = opt->jo_رڪن_کانن_ڳڻپ;
    وإلا إذا (کانن_ڳڻپ != 0)
	رڪن->tl_کانن_ڳڻپ = کانن_ڳڻپ;

    إذا (!opt->jo_hidden)
    {
	إذا (رڪن->tl_صفون != curwin->w_اوچائي)
	    win_setheight_win(رڪن->tl_صفون, curwin);
	إذا (رڪن->tl_کانن_ڳڻپ != curwin->w_ويڪر)
	    win_setwidth_win(رڪن->tl_کانن_ڳڻپ, curwin);

	// Set 'winsize' now to avoid a resize at the next redraw.
	إذا (!minsize && *curwin->w_p_tws != NUL)
	{
	    char_u buf[100];

	    vim_snprintf((حرف *)buf, 100, "%dx%d",
						 رڪن->tl_صفون, رڪن->tl_کانن_ڳڻپ);
	    سيٽ_option_قدر_give_err((char_u *)"termwinsize",
							   0L, buf, OPT_LOCAL);
	}
    }
}

/*
 * Initialize job options for a terminal job.
 * Caller may overrule some of them.
 */
    فراغ
init_ڪم_options(jobopt_T *opt)
{
    clear_ڪم_options(opt);

    opt->jo_گهڻائي = CH_گهڻائي_RAW;
    opt->jo_ٻاهريون_گهڻائي = CH_گهڻائي_RAW;
    opt->jo_err_گهڻائي = CH_گهڻائي_RAW;
    opt->jo_سيٽ = JO_گهڻائي | JO_ٻاهريون_گهڻائي | JO_ERR_گهڻائي;
}

/*
 * Set job options mandatory for a terminal job.
 */
    ساكن فراغ
تياري_ڪم_options(jobopt_T *opt, صحيح صفون_2, صحيح کانن_ڳڻپ)
{
#جيڪڏهن_وصف_نه_ٿيل MSWIN
    // Win32: Redirecting the job output won't work, thus always connect stdout
    // here.
    إذا (!(opt->jo_سيٽ & JO_ٻاهريون_IO))
#جيڪڏهن_پورو
    {
	// Connect stdout to the terminal.
	opt->jo_io[PART_ٻاهريون] = JIO_بفر;
	opt->jo_io_buf[PART_ٻاهريون] = curbuf->b_fnum;
	opt->jo_modifiable[PART_ٻاهريون] = 0;
	opt->jo_سيٽ |= JO_ٻاهريون_IO + JO_ٻاهريون_BUF + JO_ٻاهريون_MODIFIABLE;
    }

#جيڪڏهن_وصف_نه_ٿيل MSWIN
    // Win32: Redirecting the job output won't work, thus always connect stderr
    // here.
    إذا (!(opt->jo_سيٽ & JO_ERR_IO))
#جيڪڏهن_پورو
    {
	// Connect stderr to the terminal.
	opt->jo_io[PART_ERR] = JIO_بفر;
	opt->jo_io_buf[PART_ERR] = curbuf->b_fnum;
	opt->jo_modifiable[PART_ERR] = 0;
	opt->jo_سيٽ |= JO_ERR_IO + JO_ERR_BUF + JO_ERR_MODIFIABLE;
    }

    opt->jo_pty = TRUE;
    إذا ((opt->jo_set2 & JO2_رڪن_صفون) == 0)
	opt->jo_رڪن_صفون = صفون_2;
    إذا ((opt->jo_set2 & JO2_رڪن_کانن_ڳڻپ) == 0)
	opt->jo_رڪن_کانن_ڳڻپ = کانن_ڳڻپ;
}

/*
 * Flush messages on channels.
 */
    ساكن فراغ
رڪن_flush_messages(فراغ)
{
    mch_check_messages();
    parse_queued_messages();
}

/*
 * Close a terminal buffer (and its window).  Used when creating the terminal
 * fails.
 */
    ساكن فراغ
رڪن_close_بفر(buf_T *buf, buf_T *old_curbuf)
{
    free_terminal(buf);
    إذا (old_curbuf != عدم)
    {
	--curbuf->b_nwindows;
	curbuf = old_curbuf;
	curwin->w_بفر = curbuf;
	++curbuf->b_nwindows;
    }
    CHECK_CURBUF;

    // Wiping out the buffer will also close the window and call
    // free_terminal().
    do_بفر(DOBUF_WIPE, DOBUF_پهريون, FORWARD, buf->b_fnum, TRUE);
}

/*
 * Start a terminal window and return its buffer.
 * Use either "argvar" or "argv", the other must be NULL.
 * When "flags" has TERM_START_NOJOB only create the buffer, b_term and open
 * the window.
 * Returns NULL when failed.
 */
    buf_T *
رڪن_شروعات(
	typval_T    *argvar,
	حرف	    **argv,
	jobopt_T    *opt,
	صحيح	    flags)
{
    exarg_T	ورهايو_ea;
    win_T	*old_curwin = curwin;
    رڪن_T	*رڪن;
    buf_T	*old_curbuf = عدم;
    صحيح		res;
    buf_T	*newbuf;
    صحيح		vertical = opt->jo_vertical || (cmdmod.cmod_ورهايو & WSP_VERT);
    jobopt_T	orig_opt;  // only partly filled
    pos_T	save_cursor;

    إذا (check_restricted() || check_secure())
	أرجع عدم;
    إذا (cmdwin_type != 0)
    {
	emsg(_(e_cannot_open_terminal_from_command_سٽ_دري));
	أرجع عدم;
    }

    إذا ((opt->jo_سيٽ & (JO_اندريون_IO + JO_ٻاهريون_IO + JO_ERR_IO))
					 == (JO_اندريون_IO + JO_ٻاهريون_IO + JO_ERR_IO)
	|| (!(opt->jo_سيٽ & JO_ٻاهريون_IO) && (opt->jo_سيٽ & JO_ٻاهريون_BUF))
	|| (!(opt->jo_سيٽ & JO_ERR_IO) && (opt->jo_سيٽ & JO_ERR_BUF))
	|| (argvar != عدم
	    && argvar->v_type == VAR_LIST
	    && argvar->vval.v_list != عدم
	    && argvar->vval.v_list->lv_پهريون == &range_list_شي))
    {
	emsg(_(e_invalid_دليل));
	أرجع عدم;
    }

    رڪن = ALLOC_CLEAR_ONE(رڪن_T);
    إذا (رڪن == عدم)
	أرجع عدم;
    رڪن->tl_dirty_صف_پڇاڙي = MAX_صف;
    رڪن->tl_cursor_visible = TRUE;
    رڪن->tl_cursor_shape = VTERM_PROP_CURSORSHAPE_بلاڪ;
    رڪن->tl_پڄاڻي = opt->jo_رڪن_پڄاڻي;
#جيڪڏهن_وصف_ٿيل FEAT_GUI
    رڪن->tl_system = (flags & رڪن_شروعات_SYSTEM);
#جيڪڏهن_پورو
    ga_init2(&رڪن->tl_scrollback, حجم(sb_سٽ_T), 300);
    ga_init2(&رڪن->tl_scrollback_postponed, حجم(sb_سٽ_T), 300);
    ga_init2(&رڪن->tl_osc_buf, حجم(حرف), 300);

    setpcmark();
    CLEAR_ميدان(ورهايو_ea);
    إذا (opt->jo_curwin)
    {
	// Create a new buffer in the current window.
	إذا (!can_abandon(curbuf, flags & رڪن_شروعات_FORCEIT))
	{
	    no_write_پيغام();
	    vim_free(رڪن);
	    أرجع عدم;
	}
	إذا (do_ecmd(0, عدم, عدم, &ورهايو_ea, ECMD_ONE,
		      (buf_hide(curwin->w_بفر) ? ECMD_HIDE : 0)
			  + ((flags & رڪن_شروعات_FORCEIT) ? ECMD_FORCEIT : 0),
							       curwin) == FAIL)
	{
	    vim_free(رڪن);
	    أرجع عدم;
	}
    }
    وإلا إذا (opt->jo_hidden || (flags & رڪن_شروعات_SYSTEM))
    {
	buf_T *buf;

	// Create a new buffer without a window. Make it the current buffer for
	// a moment to be able to do the initializations.
	buf = buflist_new((char_u *)"", عدم, (linenr_T)0,
							 BLN_NEW | BLN_LISTED);
	إذا (buf == عدم || ml_open(buf) == FAIL)
	{
	    vim_free(رڪن);
	    أرجع عدم;
	}
	old_curbuf = curbuf;
	--curbuf->b_nwindows;
	curbuf = buf;
	save_cursor = curwin->w_cursor;
	curwin->w_بفر = buf;
	++curbuf->b_nwindows;
    }
    وإلا
    {
	// Open a new window or tab.
	ورهايو_ea.cmdidx = CMD_new;
	ورهايو_ea.cmd = (char_u *)"new";
	ورهايو_ea.arg = (char_u *)"";
	إذا (opt->jo_رڪن_صفون > 0 && !vertical)
	{
	    ورهايو_ea.line2 = opt->jo_رڪن_صفون;
	    ورهايو_ea.addr_ڳڻپ = 1;
	}
	إذا (opt->jo_رڪن_کانن_ڳڻپ > 0 && vertical)
	{
	    ورهايو_ea.line2 = opt->jo_رڪن_کانن_ڳڻپ;
	    ورهايو_ea.addr_ڳڻپ = 1;
	}

	صحيح cmod_ورهايو_modified = FALSE;
	إذا (vertical)
	{
	    إذا (!(cmdmod.cmod_ورهايو & WSP_VERT))
		cmod_ورهايو_modified = TRUE;
	    cmdmod.cmod_ورهايو |= WSP_VERT;
	}
	ex_splitview(&ورهايو_ea);
	إذا (cmod_ورهايو_modified)
	    cmdmod.cmod_ورهايو &= ~WSP_VERT;
	إذا (curwin == old_curwin)
	{
	    // split failed
	    vim_free(رڪن);
	    أرجع عدم;
	}
    }
    رڪن->tl_بفر = curbuf;
    curbuf->b_رڪن = رڪن;

    إذا (!opt->jo_hidden)
    {
	// Only one size was taken care of with :new, do the other one.  With
	// "curwin" both need to be done.
	إذا (opt->jo_رڪن_صفون > 0 && (opt->jo_curwin || vertical))
	    win_setheight(opt->jo_رڪن_صفون);
	إذا (opt->jo_رڪن_کانن_ڳڻپ > 0 && (opt->jo_curwin || !vertical))
	    win_setwidth(opt->jo_رڪن_کانن_ڳڻپ);
    }

    // Link the new terminal in the list of active terminals.
    رڪن->tl_ايندڙ = پهريون_رڪن;
    پهريون_رڪن = رڪن;

    apply_autocmds(EVENT_BUFFILEPRE, عدم, عدم, FALSE, curbuf);

    إذا (opt->jo_رڪن_name != عدم)
    {
	vim_free(curbuf->b_ffname);
	curbuf->b_ffname = vim_strsave(opt->jo_رڪن_name);
    }
    وإلا إذا (argv != عدم)
    {
	vim_free(curbuf->b_ffname);
	curbuf->b_ffname = vim_strsave((char_u *)"!system");
    }
    وإلا
    {
	صحيح	i;
	size_t	len;
	char_u	*cmd, *p;

	إذا (argvar->v_type == VAR_STRING)
	{
	    cmd = argvar->vval.v_string;
	    إذا (cmd == عدم)
		cmd = (char_u *)"";
	    وإلا إذا (STRCMP(cmd, "NONE") == 0)
		cmd = (char_u *)"pty";
	}
	وإلا إذا (argvar->v_type != VAR_LIST
		|| argvar->vval.v_list == عدم
		|| argvar->vval.v_list->lv_len == 0
		|| (cmd = tv_get_string_chk(
			       &argvar->vval.v_list->lv_پهريون->li_tv)) == عدم)
	    cmd = (char_u*)"";

	len = STRLEN(cmd) + 10;
	p = alloc(len);

	لكل (i = 0; p != عدم; ++i)
	{
	    // Prepend a ! to the command name to avoid the buffer name equals
	    // the executable, otherwise ":w!" would overwrite it.
	    إذا (i == 0)
		vim_snprintf((حرف *)p, len, "!%s", cmd);
	    وإلا
		vim_snprintf((حرف *)p, len, "!%s (%d)", cmd, i);
	    إذا (buflist_findname(p) == عدم)
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

    إذا (opt->jo_رڪن_opencmd != عدم)
	رڪن->tl_opencmd = vim_strsave(opt->jo_رڪن_opencmd);

    إذا (opt->jo_eof_chars != عدم)
	رڪن->tl_eof_chars = vim_strsave(opt->jo_eof_chars);

    سيٽ_string_option_سڌو((char_u *)"buftype", -1,
				  (char_u *)"terminal", OPT_FREE|OPT_LOCAL, 0);
    // Avoid that 'buftype' is reset when this buffer is entered.
    curbuf->b_p_initialized = true;

    // Mark the buffer as not modifiable. It can only be made modifiable after
    // the job finished.
    curbuf->b_p_ma = FALSE;

    سيٽ_رڪن_and_win_سائيز(رڪن, opt);
#جيڪڏهن_وصف_ٿيل MSWIN
    mch_memmove(orig_opt.jo_io, opt->jo_io, حجم(orig_opt.jo_io));
#جيڪڏهن_پورو
    تياري_ڪم_options(opt, رڪن->tl_صفون, رڪن->tl_کانن_ڳڻپ);

    إذا (flags & رڪن_شروعات_NOJOB)
	أرجع curbuf;

#إذا defined(FEAT_SESSION)
    // Remember the command for the session file.
    إذا (opt->jo_رڪن_norestore || argv != عدم)
	رڪن->tl_command = vim_strsave((char_u *)"NONE");
    وإلا إذا (argvar->v_type == VAR_STRING)
    {
	char_u	*cmd = argvar->vval.v_string;

	إذا (cmd != عدم && STRCMP(cmd, p_sh) != 0)
	    رڪن->tl_command = vim_strsave(cmd);
    }
    وإلا إذا (argvar->v_type == VAR_LIST
	    && argvar->vval.v_list != عدم
	    && argvar->vval.v_list->lv_len > 0)
    {
	garray_T	ga;
	listitem_T	*شي;

	ga_init2(&ga, 1, 100);
	FOR_ALL_LIST_شيون(argvar->vval.v_list, شي)
	{
	    char_u *s = tv_get_string_chk(&شي->li_tv);
	    char_u *p;

	    إذا (s == عدم)
		اكسر;
	    p = vim_strsave_fnameescape(s, VSE_NONE);
	    إذا (p == عدم)
		اكسر;
	    ga_concat(&ga, p);
	    vim_free(p);
	    ga_append(&ga, ' ');
	}
	إذا (شي == عدم)
	{
	    ga_append(&ga, NUL);
	    رڪن->tl_command = ga.ga_معلومات;
	}
	وإلا
	    ga_clear(&ga);
    }
#جيڪڏهن_پورو

    إذا (opt->jo_رڪن_kill != عدم)
    {
	char_u *p = skiptowhite(opt->jo_رڪن_kill);

	رڪن->tl_kill = vim_strnsave(opt->jo_رڪن_kill, p - opt->jo_رڪن_kill);
    }

    إذا (opt->jo_رڪن_api != عدم)
    {
	char_u *p = skiptowhite(opt->jo_رڪن_api);

	رڪن->tl_api = vim_strnsave(opt->jo_رڪن_api, p - opt->jo_رڪن_api);
    }
    وإلا
	رڪن->tl_api = vim_strsave((char_u *)"Tapi_");

    إذا (opt->jo_set2 & JO2_رڪن_HIGHLIGHT)
	رڪن->tl_highlight_name = vim_strsave(opt->jo_رڪن_highlight);

#إذا defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    // Save the user-defined palette, it is only used in GUI (or 'tgc' is on).
    إذا (opt->jo_set2 & JO2_ANSI_COLORS)
    {
	رڪن->tl_palette = ALLOC_MULT(long_u, 16);
	إذا (رڪن->tl_palette == عدم)
	{
	    vim_free(رڪن);
	    أرجع عدم;
	}
	يادگيري_نقل(رڪن->tl_palette, opt->jo_ansi_colors, حجم(long_u) * 16);
    }
#جيڪڏهن_پورو

    // System dependent: setup the vterm and maybe start the job in it.
    إذا (argv == عدم
	    && argvar->v_type == VAR_STRING
	    && argvar->vval.v_string != عدم
	    && STRCMP(argvar->vval.v_string, "NONE") == 0)
	res = create_pty_only(رڪن, opt);
    وإلا
	res = رڪن_and_ڪم_init(رڪن, argvar, argv, opt, &orig_opt);

    newbuf = curbuf;
    إذا (res == OK)
    {
	// Get and remember the size we ended up with.  Update the pty.
	vterm_get_سائيز(رڪن->tl_vterm, &رڪن->tl_صفون, &رڪن->tl_کانن_ڳڻپ);
	رڪن_report_winsize(رڪن, رڪن->tl_صفون, رڪن->tl_کانن_ڳڻپ);
#جيڪڏهن_وصف_ٿيل FEAT_GUI
	إذا (رڪن->tl_system)
	{
	    // display first line below typed command
	    رڪن->tl_toprow = نياپو_صف + 1;
	    رڪن->tl_dirty_صف_پڇاڙي = 0;
	}
#جيڪڏهن_پورو

	// Make sure we don't get stuck on sending keys to the job, it leads to
	// a deadlock if the job is waiting for Vim to read.
	channel_سيٽ_nonblock(رڪن->tl_ڪم->jv_channel, PART_اندريون);

	إذا (old_curbuf != عدم)
	{
	    --curbuf->b_nwindows;
	    curbuf = old_curbuf;
	    curwin->w_بفر = curbuf;
	    curwin->w_cursor = save_cursor;
	    ++curbuf->b_nwindows;
	}
	وإلا إذا (vgetc_busy
#جيڪڏهن_وصف_ٿيل FEAT_TIMERS
		|| timer_busy
#جيڪڏهن_پورو
		|| داخلا_busy)
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
	رڪن_close_بفر(curbuf, old_curbuf);
	أرجع عدم;
    }

    apply_autocmds(EVENT_TERMINALOPEN, عدم, عدم, FALSE, newbuf);
    إذا (!opt->jo_hidden && !(flags & رڪن_شروعات_SYSTEM))
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
    صحيح		scroll_save = نياپو_scroll;

    نياپو_scroll = FALSE;		// don't scroll here
    autowrite_all();
    نياپو_scroll = scroll_save;

    init_ڪم_options(&opt);

    cmd = eap->arg;
    طالما (*cmd == '+' && *(cmd + 1) == '+')
    {
	char_u  *p, *ep;

	cmd += 2;
	p = skiptowhite(cmd);
	ep = vim_strchr(cmd, '=');
	إذا (ep != عدم)
	{
	    إذا (ep < p)
		p = ep;
	    وإلا
		ep = عدم;
	}

	// Note: Keep this in sync with get_terminalopt_name.

#وصف_ڪريو OPTARG_HAS(name) ((صحيح)(p - cmd) == حجم(name) - 1 \
				 && STRNICMP(cmd, name, حجم(name) - 1) == 0)
	إذا (OPTARG_HAS("close"))
	    opt.jo_رڪن_پڄاڻي = 'c';
	وإلا إذا (OPTARG_HAS("noclose"))
	    opt.jo_رڪن_پڄاڻي = 'n';
	وإلا إذا (OPTARG_HAS("open"))
	    opt.jo_رڪن_پڄاڻي = 'o';
	وإلا إذا (OPTARG_HAS("curwin"))
	    opt.jo_curwin = 1;
	وإلا إذا (OPTARG_HAS("hidden"))
	    opt.jo_hidden = 1;
	وإلا إذا (OPTARG_HAS("norestore"))
	    opt.jo_رڪن_norestore = 1;
	وإلا إذا (OPTARG_HAS("shell"))
	    opt_shell = TRUE;
	وإلا إذا (OPTARG_HAS("kill") && ep != عدم)
	{
	    opt.jo_set2 |= JO2_رڪن_KILL;
	    opt.jo_رڪن_kill = ep + 1;
	    p = skiptowhite(cmd);
	}
	وإلا إذا (OPTARG_HAS("api"))
	{
	    opt.jo_set2 |= JO2_رڪن_API;
	    إذا (ep != عدم)
	    {
		opt.jo_رڪن_api = ep + 1;
		p = skiptowhite(cmd);
	    }
	    وإلا
		opt.jo_رڪن_api = عدم;
	}
	وإلا إذا (OPTARG_HAS("rows") && ep != عدم && محفوظ_isdigit(ep[1]))
	{
	    opt.jo_set2 |= JO2_رڪن_صفون;
	    opt.jo_رڪن_صفون = atoi((حرف *)ep + 1);
	    p = skiptowhite(cmd);
	}
	وإلا إذا (OPTARG_HAS("cols") && ep != عدم && محفوظ_isdigit(ep[1]))
	{
	    opt.jo_set2 |= JO2_رڪن_کانن_ڳڻپ;
	    opt.jo_رڪن_کانن_ڳڻپ = atoi((حرف *)ep + 1);
	    p = skiptowhite(cmd);
	}
	وإلا إذا (OPTARG_HAS("eof") && ep != عدم)
	{
	    char_u *buf = عدم;
	    char_u *ڪنجيون;

	    vim_free(opt.jo_eof_chars);
	    p = skiptowhite(cmd);
	    *p = NUL;
	    ڪنجيون = replace_termcodes(ep + 1, &buf, 0,
		    REPTERM_FROM_PART | REPTERM_DO_LT | REPTERM_SPECIAL, عدم);
	    opt.jo_set2 |= JO2_EOF_CHARS;
	    opt.jo_eof_chars = vim_strsave(ڪنجيون);
	    vim_free(buf);
	    *p = ' ';
	}
#جيڪڏهن_وصف_ٿيل MSWIN
	وإلا إذا ((صحيح)(p - cmd) == 4 && STRNICMP(cmd, "type", 4) == 0
								 && ep != عدم)
	{
	    صحيح tty_type = NUL;

	    p = skiptowhite(cmd);
	    إذا (STRNICMP(ep + 1, "winpty", p - (ep + 1)) == 0)
		tty_type = 'w';
	    وإلا إذا (STRNICMP(ep + 1, "conpty", p - (ep + 1)) == 0)
		tty_type = 'c';
	    وإلا
	    {
		semsg(_(e_invalid_قدر_for_دليل_str), "type");
		اذهب theend;
	    }
	    opt.jo_set2 |= JO2_TTY_TYPE;
	    opt.jo_tty_type = tty_type;
	}
#جيڪڏهن_پورو
	وإلا
	{
	    إذا (*p)
		*p = NUL;
	    semsg(_(e_invalid_attribute_str), cmd);
	    اذهب theend;
	}
#وصف_رد OPTARG_HAS
	cmd = skipwhite(p);
    }
    إذا (*cmd == NUL)
    {
	// Make a copy of 'shell', an autocommand may change the option.
	tofree = cmd = vim_strsave(p_sh);

	// default to close when the shell exits
	إذا (opt.jo_رڪن_پڄاڻي == NUL)
	    opt.jo_رڪن_پڄاڻي = TL_پڄاڻي_CLOSE;
    }

    إذا (eap->addr_ڳڻپ > 0)
    {
	// Write lines from current buffer to the job.
	opt.jo_سيٽ |= JO_اندريون_IO | JO_اندريون_BUF | JO_اندريون_چوٽي | JO_اندريون_BOT;
	opt.jo_io[PART_اندريون] = JIO_بفر;
	opt.jo_io_buf[PART_اندريون] = curbuf->b_fnum;
	opt.jo_اندريون_چوٽي = eap->line1;
	opt.jo_اندريون_bot = eap->line2;
    }

    إذا (opt_shell && tofree == عدم)
    {
#جيڪڏهن_وصف_ٿيل UNIX
	حرف	**argv = عدم;
	char_u	*tofree1 = عدم;
	char_u	*tofree2 = عدم;

	// :term ++shell command
	إذا (unix_build_argv(cmd, &argv, &tofree1, &tofree2) == OK)
	    رڪن_شروعات(عدم, argv, &opt, eap->forceit ? رڪن_شروعات_FORCEIT : 0);
	vim_free(argv);
	vim_free(tofree1);
	vim_free(tofree2);
	اذهب theend;
#وإلا
# ifdef MSWIN
	long_u	    cmdlen = STRLEN(p_sh) + STRLEN(p_shcf) + STRLEN(cmd) + 10;
	char_u	    *newcmd;

	newcmd = alloc(cmdlen);
	إذا (newcmd == عدم)
	    اذهب theend;
	tofree = newcmd;
	vim_snprintf((حرف *)newcmd, cmdlen, "%s %s %s", p_sh, p_shcf, cmd);
	cmd = newcmd;
# وإلا
	emsg(_(e_sorry_plusplusshell_not_supported_on_this_system));
	اذهب theend;
# endif
#جيڪڏهن_پورو
    }
    argvar[0].v_type = VAR_STRING;
    argvar[0].vval.v_string = cmd;
    argvar[1].v_type = VAR_UNKNOWN;
    رڪن_شروعات(argvar, عدم, &opt, eap->forceit ? رڪن_شروعات_FORCEIT : 0);

theend:
    vim_free(tofree);
    vim_free(opt.jo_eof_chars);
}

    ساكن char_u *
get_terminalopt_name(expand_T *xp اڻ_استعمال_ٿيل, صحيح idx)
{
    // Note: Keep this in sync with ex_terminal.
    ساكن حرف *(p_termopt_قدرون[]) =
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

    إذا (idx < (صحيح)ARRAY_ڊيگهائي(p_termopt_قدرون))
	أرجع (char_u*)p_termopt_قدرون[idx];
    أرجع عدم;
}

    ساكن char_u *
get_termkill_name(expand_T *xp اڻ_استعمال_ٿيل, صحيح idx)
{
    // These are platform-specific values used for job_stop(). They are defined
    // in each platform's mch_signal_job(). Just use a unified auto-complete
    // list for simplicity.
    ساكن حرف *(p_termkill_قدرون[]) =
    {
	"term",
	"hup",
	"quit",
	"int",
	"kill",
	"winch",
    };

    إذا (idx < (صحيح)ARRAY_ڊيگهائي(p_termkill_قدرون))
	أرجع (char_u*)p_termkill_قدرون[idx];
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
	صحيح	    *انگ_Matches)
{
    إذا (xp->xp_نمونو > xp->xp_سٽ && *(xp->xp_نمونو-1) == '=')
    {
	char_u *(*cb)(expand_T *, صحيح) = عدم;

	char_u *name_پڇاڙي = xp->xp_نمونو - 1;
	إذا (name_پڇاڙي - xp->xp_سٽ >= 4
		&& STRNCMP(name_پڇاڙي - 4, "kill", 4) == 0)
	    cb = get_termkill_name;

	إذا (cb != عدم)
	{
	    أرجع ExpandGeneric(
		    pat,
		    xp,
		    rmp,
		    matches,
		    انگ_Matches,
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
	    انگ_Matches,
	    get_terminalopt_name,
	    FALSE);
}

#إذا defined(FEAT_SESSION)
/*
 * Write a :terminal command to the session file to restore the terminal in
 * window "wp".
 * Return FAIL if writing fails.
 */
    صحيح
رڪن_write_session(FILE *fd, win_T *wp, hashtab_T *terminal_bufs)
{
    ثابت صحيح	bufnr = wp->w_بفر->b_fnum;
    رڪن_T	*رڪن = wp->w_بفر->b_رڪن;

    إذا (terminal_bufs != عدم && wp->w_بفر->b_nwindows > 1)
    {
	// There are multiple views into this terminal buffer. We don't want to
	// create the terminal multiple times. If it's the first time, create,
	// otherwise link to the first buffer.
	حرف	    id_as_str[NUMBUFLEN];
	hashitem_T  *داخلا;

	vim_snprintf(id_as_str, حجم(id_as_str), "%d", bufnr);

	داخلا = hash_ڳوليو(terminal_bufs, (char_u *)id_as_str);
	إذا (!HASHITEM_خالي_جايون(داخلا))
	{
	    // we've already opened this terminal buffer
	    إذا (fprintf(fd, "execute 'buffer ' .. term_buf_%d", bufnr) < 0)
		أرجع FAIL;
	    أرجع put_eol(fd);
	}
    }

    // Create the terminal and run the command.  This is not without
    // risk, but let's assume the user only creates a session when this
    // will be OK.
    إذا (fprintf(fd, "exe ':terminal ++curwin"
		" ++cols=' .. ((&columns * %d + %ld) / %ld)"
		" .. ' ++rows=' .. ((&lines * %d + %ld) / %ld) ",
		رڪن->tl_کانن_ڳڻپ, Columns / 2, Columns,
		رڪن->tl_صفون, صفون / 2, صفون) < 0)
	أرجع FAIL;
# ifdef MSWIN
    إذا (fprintf(fd, ".. ' ++type=%s' ", رڪن->tl_ڪم->jv_tty_type) < 0)
	أرجع FAIL;
# endif
    إذا (رڪن->tl_command != عدم)
    {
	char_u *quoted_command = string_quote(رڪن->tl_command, FALSE);
	إذا (quoted_command == عدم)
	    أرجع FAIL;

	صحيح ret = fputs(".. ' ' .. ", fd) < 0
		    || fputs((حرف *)quoted_command, fd) < 0;
	vim_free(quoted_command);
	إذا (ret)
	    أرجع FAIL;
    }
    إذا (put_eol(fd) != OK)
	أرجع FAIL;

    إذا (fprintf(fd, "var term_buf_%d: number = bufnr()", bufnr) < 0)
	أرجع FAIL;

    إذا (terminal_bufs != عدم && wp->w_بفر->b_nwindows > 1)
    {
	حرف *hash_ڪنجي = alloc(NUMBUFLEN);

	vim_snprintf(hash_ڪنجي, NUMBUFLEN, "%d", bufnr);
	hash_جوڙيو(terminal_bufs, (char_u *)hash_ڪنجي, "terminal session");
    }

    أرجع put_eol(fd);
}

/*
 * Return TRUE if "buf" has a terminal that should be restored.
 */
    صحيح
رڪن_should_restore(buf_T *buf)
{
    رڪن_T	*رڪن = buf->b_رڪن;

    أرجع رڪن != عدم && (رڪن->tl_command == عدم
				     || STRCMP(رڪن->tl_command, "NONE") != 0);
}
#جيڪڏهن_پورو

/*
 * Free the scrollback buffer for "term".
 */
    ساكن فراغ
free_scrollback(رڪن_T *رڪن)
{
    صحيح i;

    لكل (i = 0; i < رڪن->tl_scrollback.ga_len; ++i)
	vim_free(((sb_سٽ_T *)رڪن->tl_scrollback.ga_معلومات + i)->sb_cells);
    ga_clear(&رڪن->tl_scrollback);
    لكل (i = 0; i < رڪن->tl_scrollback_postponed.ga_len; ++i)
	vim_free(((sb_سٽ_T *)رڪن->tl_scrollback_postponed.ga_معلومات + i)->sb_cells);
    ga_clear(&رڪن->tl_scrollback_postponed);
}


// Terminals that need to be freed soon.
ساكن رڪن_T	*terminals_منزل_free = عدم;

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
    رڪن_T	*رڪن = buf->b_رڪن;
    رڪن_T	*tp;

    إذا (رڪن == عدم)
	أرجع;

    // Unlink the terminal form the list of terminals.
    إذا (پهريون_رڪن == رڪن)
	پهريون_رڪن = رڪن->tl_ايندڙ;
    وإلا
	لكل (tp = پهريون_رڪن; tp->tl_ايندڙ != عدم; tp = tp->tl_ايندڙ)
	    إذا (tp->tl_ايندڙ == رڪن)
	    {
		tp->tl_ايندڙ = رڪن->tl_ايندڙ;
		اكسر;
	    }

    إذا (رڪن->tl_ڪم != عدم)
    {
	إذا (رڪن->tl_ڪم->jv_حالت != ڪم_ENDED
		&& رڪن->tl_ڪم->jv_حالت != ڪم_ختم
		&& رڪن->tl_ڪم->jv_حالت != ڪم_FAILED)
	    ڪم_stop(رڪن->tl_ڪم, عدم, "kill");
	ڪم_unref(رڪن->tl_ڪم);
    }
    رڪن->tl_ايندڙ = terminals_منزل_free;
    terminals_منزل_free = رڪن;

    buf->b_رڪن = عدم;
    إذا (اندريون_terminal_loop == رڪن)
	اندريون_terminal_loop = عدم;
}

    فراغ
free_اڻ_استعمال_ٿيل_terminals(فراغ)
{
    طالما (terminals_منزل_free != عدم)
    {
	رڪن_T	    *رڪن = terminals_منزل_free;

	terminals_منزل_free = رڪن->tl_ايندڙ;

	free_scrollback(رڪن);
	ga_clear(&رڪن->tl_osc_buf);

	رڪن_free_vterm(رڪن);
	vim_free(رڪن->tl_api);
	vim_free(رڪن->tl_title);
#جيڪڏهن_وصف_ٿيل FEAT_SESSION
	vim_free(رڪن->tl_command);
#جيڪڏهن_پورو
	vim_free(رڪن->tl_kill);
	vim_free(رڪن->tl_حالت_متن);
	vim_free(رڪن->tl_opencmd);
	vim_free(رڪن->tl_eof_chars);
	vim_free(رڪن->tl_arg0_cmd);
#جيڪڏهن_وصف_ٿيل MSWIN
	إذا (رڪن->tl_ٻاهريون_fd != عدم)
	    fclose(رڪن->tl_ٻاهريون_fd);
#جيڪڏهن_پورو
	vim_free(رڪن->tl_highlight_name);
	vim_free(رڪن->tl_cursor_color);
	vim_free(رڪن->tl_palette);
	vim_free(رڪن);
    }
}

/*
 * Get the part that is connected to the tty. Normally this is PART_IN, but
 * when writing buffer lines to the job it can be another.  This makes it
 * possible to do "1,5term vim -".
 */
    ساكن ch_part_T
get_tty_part(رڪن_T *رڪن اڻ_استعمال_ٿيل)
{
#جيڪڏهن_وصف_ٿيل UNIX
    ch_part_T	parts[3] = {PART_اندريون, PART_ٻاهريون, PART_ERR};
    صحيح		i;

    لكل (i = 0; i < 3; ++i)
    {
	صحيح fd = رڪن->tl_ڪم->jv_channel->ch_part[parts[i]].ch_fd;

	إذا (mch_isatty(fd))
	    أرجع parts[i];
    }
#جيڪڏهن_پورو
    أرجع PART_اندريون;
}

/*
 * Read any vterm output and send it on the channel.
 */
    ساكن فراغ
رڪن_forward_output(رڪن_T *رڪن)
{
    VTerm *vterm = رڪن->tl_vterm;
    حرف   buf[ڪنجي_BUF_LEN];
    size_t curlen = vterm_output_read(vterm, buf, ڪنجي_BUF_LEN);

    إذا (curlen > 0)
	channel_send(رڪن->tl_ڪم->jv_channel, get_tty_part(رڪن),
					     (char_u *)buf, (صحيح)curlen, عدم);
}

/*
 * Write job output "msg[len]" to the vterm.
 */
    ساكن فراغ
رڪن_write_ڪم_output(رڪن_T *رڪن, char_u *نياپو_arg, size_t len_arg)
{
    char_u	*نياپو = نياپو_arg;
    size_t	len = len_arg;
    VTerm	*vterm = رڪن->tl_vterm;
    size_t	prevlen = vterm_output_get_بفر_current(vterm);
    size_t	حد_بندي = رڪن->tl_بفر->b_p_twsl * رڪن->tl_کانن_ڳڻپ * 3;

    // Limit the length to 'termwinscroll' * cols * 3 bytes.  Keep the text at
    // the end.
    إذا (len > حد_بندي)
    {
	char_u *p = نياپو + len - حد_بندي;

	p -= (*mb_سر_off)(نياپو, p);
	len -= p - نياپو;
	نياپو = p;
    }

    vterm_داخلا_write(vterm, (حرف *)نياپو, len);

    // flush vterm buffer when vterm responded to control sequence
    إذا (prevlen != vterm_output_get_بفر_current(vterm))
	رڪن_forward_output(رڪن);

    // this invokes the damage callbacks
    vterm_screen_flush_damage(vterm_obtain_screen(vterm));
}

    ساكن فراغ
position_cursor(win_T *wp, VTermPos *pos)
{
    wp->w_wrow = MIN(pos->صف, MAX(0, wp->w_اوچائي - 1));
    wp->w_wcol = MIN(pos->کانو, MAX(0, wp->w_ويڪر - 1));
#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
    إذا (popup_is_popup(wp))
    {
	wp->w_wrow += popup_چوٽي_extra(wp);
	wp->w_wcol += popup_کاٻو_extra(wp);
	wp->w_flags |= WFLAG_WCOL_OFF_ADDED | WFLAG_WROW_OFF_ADDED;
    }
    وإلا
	wp->w_flags &= ~(WFLAG_WCOL_OFF_ADDED | WFLAG_WROW_OFF_ADDED);
#جيڪڏهن_پورو
    wp->w_valid |= (VALID_WCOL|VALID_WROW);
}

    ساكن فراغ
update_cursor(رڪن_T *رڪن, صحيح redraw)
{
    إذا (رڪن->tl_عام_گهڻائي)
	أرجع;
#جيڪڏهن_وصف_ٿيل FEAT_GUI
    إذا (رڪن->tl_system)
	windgoto(رڪن->tl_cursor_pos.صف + رڪن->tl_toprow,
		firstwin->w_wincol + رڪن->tl_cursor_pos.کانو);
    وإلا
#جيڪڏهن_پورو
    إذا (!رڪن_ڪم_running(رڪن))
	// avoid the cursor positioned below the last used line
	setcursor();
    وإلا
    {
	// do not use the window cursor position
	position_cursor(curwin, &curbuf->b_رڪن->tl_cursor_pos);
	windgoto(W_WINROW(curwin) + curwin->w_wrow,
		 curwin->w_wincol + curwin->w_wcol);
    }
    إذا (redraw)
    {
	aco_save_T	aco;

	إذا (رڪن->tl_بفر == curbuf && رڪن->tl_cursor_visible)
	    cursor_on();
	ٻاهريون_flush();
#جيڪڏهن_وصف_ٿيل FEAT_GUI
	إذا (gui.اندريون_use)
	{
	    gui_update_cursor(FALSE, FALSE);
	    gui_mch_flush();
	}
#جيڪڏهن_پورو
	// Make sure an invoked autocmd doesn't delete the buffer (and the
	// terminal) under our fingers.
	++رڪن->tl_بفر->b_locked;

	// save and restore curwin and curbuf, in case the autocmd changes them
	aucmd_prepbuf(&aco, curbuf);
	apply_autocmds(EVENT_TEXTCHANGEDT, عدم, عدم, FALSE, رڪن->tl_بفر);
	aucmd_restbuf(&aco);

	--رڪن->tl_بفر->b_locked;
    }
}

/*
 * Find the location of a scrollbackline in the buffer
 */
    ساكن فراغ
scrollbackline_pos_اندريون_buf(رڪن_T *رڪن, صحيح صف, linenr_T *lnum, صحيح *شروعات_کانو, size_t *شروعات_pos)
{
    sb_سٽ_T	*سٽون = (sb_سٽ_T *)رڪن->tl_scrollback.ga_معلومات;
    linenr_T    calc_lnum = رڪن->tl_بفر_scrolled;
    size_t      calc_pos = 0;
    صحيح         calc_کانو = 0;
    صحيح         i;

    إذا (صف < 0 || صف >= رڪن->tl_scrollback.ga_len)
	أرجع;

    إذا (صف > رڪن->tl_scrollback_scrolled)
    {
	// Lookback how far along in the top line we are
	لكل (i = رڪن->tl_scrollback_scrolled + 1; i > 0 && سٽون[i].continuation; --i)
	{
	    calc_pos += سٽون[i - 1].sb_بائيٽون;
	    calc_کانو += سٽون[i - 1].sb_کانن_ڳڻپ;
	}
	i = رڪن->tl_scrollback_scrolled + 1;
	calc_lnum = رڪن->tl_بفر_scrolled + 1;
	// Do not count this line's bytes/cols twice
	إذا (سٽون[i].continuation)
	    ++i;
    }
    وإلا
    {
	i = 1;
	calc_lnum = 1;
    }

    لكل (; i <= صف; ++i)
    {
	إذا (!سٽون[i].continuation)
	{
	    ++calc_lnum;
	    calc_pos = 0;
	    calc_کانو = 0;
	}
	وإلا
	{
	    calc_pos += سٽون[i - 1].sb_بائيٽون;
	    calc_کانو += سٽون[i - 1].sb_کانن_ڳڻپ;
	}
    }

    *lnum = calc_lnum;
    إذا (شروعات_کانو)
	*شروعات_کانو = calc_کانو;
    إذا (شروعات_pos)
	*شروعات_pos = calc_pos;
}

/*
 * Find the location of a buffer line in the scrollback
 */
    ساكن فراغ
bufline_pos_اندريون_scrollback(رڪن_T *رڪن, linenr_T lnum, صحيح کانو, صحيح *صف, صحيح *wrapped_کانو)
{
    buf_T	*buf = رڪن->tl_بفر;
    sb_سٽ_T	*سٽون = (sb_سٽ_T *)رڪن->tl_scrollback.ga_معلومات;
    linenr_T    calc_صف = رڪن->tl_scrollback_scrolled;
    صحيح         calc_کانو = کانو;
    linenr_T    l;

    إذا (lnum > buf->b_ml.ml_سٽ_ڳڻپ)
	أرجع;

    إذا (lnum > رڪن->tl_بفر_scrolled)
    {
	calc_صف = رڪن->tl_scrollback_scrolled;
	l = رڪن->tl_بفر_scrolled + 1;

	طالما (calc_صف < رڪن->tl_scrollback.ga_len && سٽون[calc_صف].continuation)
	    ++calc_صف;
    }
    وإلا
    {
	calc_صف = 0;
	l = 1;
    }

    طالما (calc_صف < رڪن->tl_scrollback.ga_len && l < lnum)
    {
	++calc_صف;
	إذا (!سٽون[calc_صف].continuation)
	    ++l;
    }

    طالما (calc_صف + 1 < رڪن->tl_scrollback.ga_len && سٽون[calc_صف + 1].continuation
	    && calc_کانو >= سٽون[calc_صف].sb_کانن_ڳڻپ)
    {
	calc_کانو -= سٽون[calc_صف].sb_کانن_ڳڻپ;
	++calc_صف;
    }

    إذا (صف)
	*صف = calc_صف;
    إذا (wrapped_کانو)
	*wrapped_کانو = calc_کانو;
}

/*
 * Invoked when "msg" output from a job was received.  Write it to the terminal
 * of "buffer".
 */
    فراغ
write_منزل_رڪن(buf_T *بفر, char_u *نياپو, channel_T *channel)
{
    size_t	len = STRLEN(نياپو);
    رڪن_T	*رڪن = بفر->b_رڪن;

#جيڪڏهن_وصف_ٿيل MSWIN
    // Win32: Cannot redirect output of the job, intercept it here and write to
    // the file.
    إذا (رڪن->tl_ٻاهريون_fd != عدم)
    {
	ch_log(channel, "Writing %d bytes to output file", (صحيح)len);
	fwrite(نياپو, len, 1, رڪن->tl_ٻاهريون_fd);
	أرجع;
    }
#جيڪڏهن_پورو

    إذا (رڪن->tl_vterm == عدم)
    {
	ch_log(channel, "NOT writing %d bytes to terminal", (صحيح)len);
	أرجع;
    }
    ch_log(channel, "writing %d bytes to terminal", (صحيح)len);
    cursor_off();
    رڪن_write_ڪم_output(رڪن, نياپو, len);

#جيڪڏهن_وصف_ٿيل FEAT_GUI
    إذا (رڪن->tl_system)
    {
	// show system output, scrolling up the screen as needed
	update_system_رڪن(رڪن);
	update_cursor(رڪن, TRUE);
    }
    وإلا
#جيڪڏهن_پورو
    // In Terminal-Normal mode we are displaying the buffer, not the terminal
    // contents, thus no screen update is needed.
    إذا (!رڪن->tl_عام_گهڻائي)
    {
	// Don't use update_screen() when editing the command line, it gets
	// cleared.
	// TODO: only update once in a while.
	ch_log(رڪن->tl_ڪم->jv_channel, "updating screen");
	إذا (بفر == curbuf && (State & گهڻائي_CMDLINE) == 0)
	{
	    update_screen(UPD_VALID_NO_UPDATE);
#إذا defined(FEAT_TABPANEL)
	    إذا (redraw_tabpanel)
		draw_tabpanel();
#جيڪڏهن_پورو
	    // update_screen() can be slow, check the terminal wasn't closed
	    // already
	    إذا (بفر == curbuf && curbuf->b_رڪن != عدم)
		update_cursor(curbuf->b_رڪن, TRUE);
	}
	وإلا
	    redraw_after_callback(TRUE, FALSE);
    }
}

/*
 * Send a mouse position and click to the vterm
 */
    ساكن صحيح
رڪن_send_mouse(VTerm *vterm, صحيح button, صحيح pressed)
{
    VTermModifier   mod = VTERM_MOD_NONE;
    صحيح		    صف = mouse_صف - W_WINROW(curwin);
    صحيح		    کانو = mouse_کانو - curwin->w_wincol;

#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
    إذا (popup_is_popup(curwin))
    {
	صف -= popup_چوٽي_extra(curwin);
	کانو -= popup_کاٻو_extra(curwin);
    }
#جيڪڏهن_پورو
    vterm_mouse_move(vterm, صف, کانو, mod);
    إذا (button != 0)
	vterm_mouse_button(vterm, button, pressed, mod);
    أرجع TRUE;
}

ساكن صحيح enter_mouse_کانو = -1;
ساكن صحيح enter_mouse_صف = -1;

/*
 * Handle a mouse click, drag or release.
 * Return TRUE when a mouse event is sent to the terminal.
 */
    ساكن صحيح
رڪن_mouse_click(VTerm *vterm, صحيح ڪنجي)
{
#إذا defined(FEAT_CLIPBOARD)
    // For modeless selection mouse drag and release events are ignored, unless
    // they are preceded with a mouse down event
    ساكن صحيح	    ignore_drag_release = TRUE;
    VTermState	    *state;
    VTermMouseState mouse_state;


    state = vterm_obtain_state(vterm);
    إذا (state == عدم)
	أرجع FALSE;

    vterm_state_get_mousestate(state, &mouse_state);
    إذا (mouse_state.flags == 0)
    {
	// Terminal is not using the mouse, use modeless selection.
	بدل (ڪنجي)
	{
	حالة K_LEFTDRAG:
	حالة K_LEFTRELEASE:
	حالة K_RIGHTDRAG:
	حالة K_RIGHTRELEASE:
		// Ignore drag and release events when the button-down wasn't
		// seen before.
		إذا (ignore_drag_release)
		{
		    صحيح save_mouse_کانو, save_mouse_صف;

		    إذا (enter_mouse_کانو < 0)
			اكسر;

		    // mouse click in the window gave us focus, handle that
		    // click now
		    save_mouse_کانو = mouse_کانو;
		    save_mouse_صف = mouse_صف;
		    mouse_کانو = enter_mouse_کانو;
		    mouse_صف = enter_mouse_صف;
		    clip_modeless(MOUSE_کاٻو, TRUE, FALSE);
		    mouse_کانو = save_mouse_کانو;
		    mouse_صف = save_mouse_صف;
		}
		// FALLTHROUGH
	حالة K_LEFTMOUSE:
	حالة K_RIGHTMOUSE:
		إذا (ڪنجي == K_LEFTRELEASE || ڪنجي == K_RIGHTRELEASE)
		    ignore_drag_release = TRUE;
		وإلا
		    ignore_drag_release = FALSE;
		// Should we call mouse_has() here?
		إذا (clip_تارو.موجود)
		{
		    صحيح	    button, is_click, is_drag;

		    button = get_mouse_button(KEY2TERMCAP1(ڪنجي),
							 &is_click, &is_drag);
		    إذا (mouse_model_popup() && button == MOUSE_کاٻو
					       && (mod_نقاب & MOD_نقاب_سرڪايو))
		    {
			// Translate shift-left to right button.
			button = MOUSE_ساڄو;
			mod_نقاب &= ~MOD_نقاب_سرڪايو;
		    }
		    clip_modeless(button, is_click, is_drag);
		}
		اكسر;

	حالة K_MIDDLEMOUSE:
		إذا (clip_تارو.موجود)
		    وجهو_reg('*', TRUE);
		اكسر;
	}
	enter_mouse_کانو = -1;
	أرجع FALSE;
    }
#جيڪڏهن_پورو
    enter_mouse_کانو = -1;

    بدل (ڪنجي)
    {
	حالة K_LEFTMOUSE:
	حالة K_LEFTMOUSE_NM:	رڪن_send_mouse(vterm, 1, 1); اكسر;
	حالة K_LEFTDRAG:	رڪن_send_mouse(vterm, 1, 1); اكسر;
	حالة K_LEFTRELEASE:
	حالة K_LEFTRELEASE_NM:	رڪن_send_mouse(vterm, 1, 0); اكسر;
	حالة K_MOUSEMOVE:	رڪن_send_mouse(vterm, 0, 0); اكسر;
	حالة K_MIDDLEMOUSE:	رڪن_send_mouse(vterm, 2, 1); اكسر;
	حالة K_MIDDLEDRAG:	رڪن_send_mouse(vterm, 2, 1); اكسر;
	حالة K_MIDDLERELEASE:	رڪن_send_mouse(vterm, 2, 0); اكسر;
	حالة K_RIGHTMOUSE:	رڪن_send_mouse(vterm, 3, 1); اكسر;
	حالة K_RIGHTDRAG:	رڪن_send_mouse(vterm, 3, 1); اكسر;
	حالة K_RIGHTRELEASE:	رڪن_send_mouse(vterm, 3, 0); اكسر;
    }
    أرجع TRUE;
}

/*
 * Convert typed key "c" with modifiers "modmask" into bytes to send to the
 * job.
 * Return the number of bytes in "buf".
 */
    ساكن صحيح
رڪن_convert_ڪنجي(رڪن_T *رڪن, صحيح c, صحيح modmask, حرف *buf)
{
    VTerm	    *vterm = رڪن->tl_vterm;
    VTerm_ڪنجي	    ڪنجي = VTERM_ڪنجي_NONE;
    VTermModifier   mod = VTERM_MOD_NONE;
    صحيح		    other = FALSE;

    بدل (c)
    {
	// don't use VTERM_KEY_ENTER, it may do an unwanted conversion

	حالة K_BS:
#جيڪڏهن_وصف_ٿيل MSWIN
	    // In ConPTY, we must use VTERM_KEY_BACKSPACE, otherwise it will
	    // delete one word, equivalent to Alt+Backspace.
	    إذا (رڪن->tl_conpty)
		ڪنجي = VTERM_ڪنجي_BACKSPACE;
	    وإلا
#جيڪڏهن_پورو
		// don't use VTERM_KEY_BACKSPACE, it always becomes 0x7f DEL
		c = رڪن_backspace_char;
	    اكسر;

#جيڪڏهن_وصف_ٿيل MSWIN
	حالة BS:
	    // In ConPTY, we must use VTERM_KEY_BACKSPACE, otherwise it will
	    // delete one word, equivalent to Alt+Backspace.
	    إذا (رڪن->tl_conpty)
		ڪنجي = VTERM_ڪنجي_BACKSPACE;
	    اكسر;
#جيڪڏهن_پورو

	حالة ESC:		ڪنجي = VTERM_ڪنجي_ESCAPE; اكسر;
	حالة K_DEL:		ڪنجي = VTERM_ڪنجي_DEL; اكسر;
	حالة K_هيٺ_ڏانهن:		ڪنجي = VTERM_ڪنجي_هيٺ_ڏانهن; اكسر;
	حالة K_S_هيٺ_ڏانهن:		mod = VTERM_MOD_سرڪايو;
				ڪنجي = VTERM_ڪنجي_هيٺ_ڏانهن; اكسر;
	حالة K_پڇاڙي:		ڪنجي = VTERM_ڪنجي_پڇاڙي; اكسر;
	حالة K_S_پڇاڙي:		mod = VTERM_MOD_سرڪايو;
				ڪنجي = VTERM_ڪنجي_پڇاڙي; اكسر;
	حالة K_C_پڇاڙي:		mod = VTERM_MOD_CTRL;
				ڪنجي = VTERM_ڪنجي_پڇاڙي; اكسر;
	حالة K_F10:		ڪنجي = VTERM_ڪنجي_FUNCTION(10); اكسر;
	حالة K_F11:		ڪنجي = VTERM_ڪنجي_FUNCTION(11); اكسر;
	حالة K_F12:		ڪنجي = VTERM_ڪنجي_FUNCTION(12); اكسر;
	حالة K_F1:		ڪنجي = VTERM_ڪنجي_FUNCTION(1); اكسر;
	حالة K_F2:		ڪنجي = VTERM_ڪنجي_FUNCTION(2); اكسر;
	حالة K_F3:		ڪنجي = VTERM_ڪنجي_FUNCTION(3); اكسر;
	حالة K_F4:		ڪنجي = VTERM_ڪنجي_FUNCTION(4); اكسر;
	حالة K_F5:		ڪنجي = VTERM_ڪنجي_FUNCTION(5); اكسر;
	حالة K_F6:		ڪنجي = VTERM_ڪنجي_FUNCTION(6); اكسر;
	حالة K_F7:		ڪنجي = VTERM_ڪنجي_FUNCTION(7); اكسر;
	حالة K_F8:		ڪنجي = VTERM_ڪنجي_FUNCTION(8); اكسر;
	حالة K_F9:		ڪنجي = VTERM_ڪنجي_FUNCTION(9); اكسر;
	حالة K_HOME:		ڪنجي = VTERM_ڪنجي_HOME; اكسر;
	حالة K_S_HOME:		mod = VTERM_MOD_سرڪايو;
				ڪنجي = VTERM_ڪنجي_HOME; اكسر;
	حالة K_C_HOME:		mod = VTERM_MOD_CTRL;
				ڪنجي = VTERM_ڪنجي_HOME; اكسر;
	حالة K_INS:		ڪنجي = VTERM_ڪنجي_INS; اكسر;
	حالة K_K0:		ڪنجي = VTERM_ڪنجي_KP_0; اكسر;
	حالة K_K1:		ڪنجي = VTERM_ڪنجي_KP_1; اكسر;
	حالة K_K2:		ڪنجي = VTERM_ڪنجي_KP_2; اكسر;
	حالة K_K3:		ڪنجي = VTERM_ڪنجي_KP_3; اكسر;
	حالة K_K4:		ڪنجي = VTERM_ڪنجي_KP_4; اكسر;
	حالة K_K5:		ڪنجي = VTERM_ڪنجي_KP_5; اكسر;
	حالة K_K6:		ڪنجي = VTERM_ڪنجي_KP_6; اكسر;
	حالة K_K7:		ڪنجي = VTERM_ڪنجي_KP_7; اكسر;
	حالة K_K8:		ڪنجي = VTERM_ڪنجي_KP_8; اكسر;
	حالة K_K9:		ڪنجي = VTERM_ڪنجي_KP_9; اكسر;
	حالة K_KDEL:		ڪنجي = VTERM_ڪنجي_DEL; اكسر; // TODO
	حالة K_KDIVIDE:		ڪنجي = VTERM_ڪنجي_KP_DIVIDE; اكسر;
	حالة K_KEND:		ڪنجي = VTERM_ڪنجي_KP_1; اكسر; // TODO
	حالة K_KENTER:		ڪنجي = VTERM_ڪنجي_KP_ENTER; اكسر;
	حالة K_KHOME:		ڪنجي = VTERM_ڪنجي_KP_7; اكسر; // TODO
	حالة K_KINS:		ڪنجي = VTERM_ڪنجي_KP_0; اكسر; // TODO
	حالة K_KMINUS:		ڪنجي = VTERM_ڪنجي_KP_MINUS; اكسر;
	حالة K_KMULTIPLY:	ڪنجي = VTERM_ڪنجي_KP_MULT; اكسر;
	حالة K_KPAGEDOWN:	ڪنجي = VTERM_ڪنجي_KP_3; اكسر; // TODO
	حالة K_KPAGEUP:		ڪنجي = VTERM_ڪنجي_KP_9; اكسر; // TODO
	حالة K_KPLUS:		ڪنجي = VTERM_ڪنجي_KP_PLUS; اكسر;
	حالة K_KPOINT:		ڪنجي = VTERM_ڪنجي_KP_PERIOD; اكسر;
	حالة K_کاٻو:		ڪنجي = VTERM_ڪنجي_کاٻو; اكسر;
	حالة K_S_کاٻو:		mod = VTERM_MOD_سرڪايو;
				ڪنجي = VTERM_ڪنجي_کاٻو; اكسر;
	حالة K_C_کاٻو:		mod = VTERM_MOD_CTRL;
				ڪنجي = VTERM_ڪنجي_کاٻو; اكسر;
	حالة K_PAGEDOWN:	ڪنجي = VTERM_ڪنجي_PAGEDOWN; اكسر;
	حالة K_PAGEUP:		ڪنجي = VTERM_ڪنجي_PAGEUP; اكسر;
	حالة K_ساڄو:		ڪنجي = VTERM_ڪنجي_ساڄو; اكسر;
	حالة K_S_ساڄو:		mod = VTERM_MOD_سرڪايو;
				ڪنجي = VTERM_ڪنجي_ساڄو; اكسر;
	حالة K_C_ساڄو:		mod = VTERM_MOD_CTRL;
				ڪنجي = VTERM_ڪنجي_ساڄو; اكسر;
	حالة K_مٿي:		ڪنجي = VTERM_ڪنجي_مٿي; اكسر;
	حالة K_S_مٿي:		mod = VTERM_MOD_سرڪايو;
				ڪنجي = VTERM_ڪنجي_مٿي; اكسر;
	حالة TAB:		ڪنجي = VTERM_ڪنجي_TAB; اكسر;
	حالة K_S_TAB:		mod = VTERM_MOD_سرڪايو;
				ڪنجي = VTERM_ڪنجي_TAB; اكسر;

	حالة K_MOUSEUP:		other = رڪن_send_mouse(vterm, 5, 1); اكسر;
	حالة K_MOUSEDOWN:	other = رڪن_send_mouse(vterm, 4, 1); اكسر;
	حالة K_MOUSELEFT:	other = رڪن_send_mouse(vterm, 7, 1); اكسر;
	حالة K_MOUSERIGHT:	other = رڪن_send_mouse(vterm, 6, 1); اكسر;

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
	حالة K_RIGHTRELEASE:	إذا (!رڪن_mouse_click(vterm, c))
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
	حالة K_XF1:		ڪنجي = VTERM_ڪنجي_FUNCTION(1); اكسر;
	حالة K_XF2:		ڪنجي = VTERM_ڪنجي_FUNCTION(2); اكسر;
	حالة K_XF3:		ڪنجي = VTERM_ڪنجي_FUNCTION(3); اكسر;
	حالة K_XF4:		ڪنجي = VTERM_ڪنجي_FUNCTION(4); اكسر;
	حالة K_SELECT:		أرجع 0;
#جيڪڏهن_وصف_ٿيل FEAT_GUI
	حالة K_VER_SCROLLBAR:	أرجع 0;
	حالة K_HOR_SCROLLBAR:	أرجع 0;
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_GUI_TABLINE
	حالة K_TABLINE:		أرجع 0;
	حالة K_TABMENU:		أرجع 0;
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_NETBEANS_INTG
	حالة K_F21:		ڪنجي = VTERM_ڪنجي_FUNCTION(21); اكسر;
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_DND
	حالة K_DROP:		أرجع 0;
#جيڪڏهن_پورو
	حالة K_CURSORHOLD:	أرجع 0;
	حالة K_PS:		vterm_keyboard_شروعات_paste(vterm);
				other = TRUE;
				اكسر;
	حالة K_PE:		vterm_keyboard_پڇاڙي_paste(vterm);
				other = TRUE;
				اكسر;
    }

    // add modifiers for the typed key
    إذا (modmask & MOD_نقاب_سرڪايو)
	mod |= VTERM_MOD_سرڪايو;
    إذا (modmask & MOD_نقاب_CTRL)
	mod |= VTERM_MOD_CTRL;
    إذا (modmask & (MOD_نقاب_ALT | MOD_نقاب_META))
	mod |= VTERM_MOD_ALT;

    // Ctrl-Shift-i may have the key "I" instead of "i", but for the kitty
    // keyboard protocol should use "i".  Applies to all ascii letters.
    إذا (ASCII_ISUPPER(c)
	    && vterm_is_kitty_keyboard(vterm)
	    && mod == (VTERM_MOD_CTRL | VTERM_MOD_سرڪايو))
	c = TOLOWER_ASC(c);

    /*
     * Convert special keys to vterm keys:
     * - Write keys to vterm: vterm_keyboard_key()
     * - Write output to channel.
     */
    إذا (ڪنجي != VTERM_ڪنجي_NONE)
	// Special key, let vterm convert it.
	vterm_keyboard_ڪنجي(vterm, ڪنجي, mod);
    وإلا إذا (!other)
	// Normal character, let vterm convert it.
	vterm_keyboard_unichar(vterm, c, mod);

    // Read back the converted escape sequence.
    أرجع (صحيح)vterm_output_read(vterm, buf, ڪنجي_BUF_LEN);
}

/*
 * Return TRUE if the job for "term" is still running.
 * If "check_job_status" is TRUE update the job status.
 * NOTE: "term" may be freed by callbacks.
 */
    ساكن صحيح
رڪن_ڪم_running_check(رڪن_T *رڪن, صحيح check_ڪم_حالت)
{
    // Also consider the job finished when the channel is closed, to avoid a
    // race condition when updating the title.
    إذا (رڪن == عدم
	|| رڪن->tl_ڪم == عدم
	|| !channel_is_open(رڪن->tl_ڪم->jv_channel))
	أرجع FALSE;

    ڪم_T *ڪم = رڪن->tl_ڪم;

    // Careful: Checking the job status may invoke callbacks, which close
    // the buffer and terminate "term".  However, "job" will not be freed
    // yet.
    إذا (check_ڪم_حالت)
	ڪم_حالت(ڪم);
    أرجع (ڪم->jv_حالت == ڪم_STARTED
	    || (ڪم->jv_channel != عدم && ڪم->jv_channel->ch_keep_open));
}

/*
 * Return TRUE if the job for "term" is still running.
 */
    صحيح
رڪن_ڪم_running(رڪن_T *رڪن)
{
    أرجع رڪن_ڪم_running_check(رڪن, FALSE);
}

/*
 * Return TRUE if the job for "term" is still running, ignoring the job was
 * "NONE".
 */
    صحيح
رڪن_ڪم_running_not_none(رڪن_T *رڪن)
{
    أرجع رڪن_ڪم_running(رڪن) && !رڪن_none_open(رڪن);
}

/*
 * Return TRUE if "term" has an active channel and used ":term NONE".
 */
    صحيح
رڪن_none_open(رڪن_T *رڪن)
{
    // Also consider the job finished when the channel is closed, to avoid a
    // race condition when updating the title.
    أرجع رڪن != عدم
	&& رڪن->tl_ڪم != عدم
	&& channel_is_open(رڪن->tl_ڪم->jv_channel)
	&& رڪن->tl_ڪم->jv_channel->ch_keep_open;
}

//
// Used to confirm whether we would like to kill a terminal.
// Return OK when the user confirms to kill it.
// Return FAIL if the user selects otherwise.
//
    صحيح
رڪن_confirm_stop(buf_T *buf)
{
    char_u	buff[DIALOG_نياپو_سائيز];
    صحيح	ret;

    dialog_نياپو(buff, _("Kill job in \"%s\"?"), buf_get_fname(buf));
    ret = vim_dialog_yesno(VIM_QUESTION, عدم, buff, 1);
    إذا (ret == VIM_YES)
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
رڪن_try_stop_ڪم(buf_T *buf)
{
    صحيح	    ڳڻپ;
    حرف    *how = (حرف *)buf->b_رڪن->tl_kill;

#إذا defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
    إذا ((how == عدم || *how == NUL)
			  && (p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)))
    {
	إذا (رڪن_confirm_stop(buf) == OK)
	    how = "kill";
	وإلا
	    أرجع FAIL;
    }
#جيڪڏهن_پورو
    إذا (how == عدم || *how == NUL)
	أرجع FAIL;

    ڪم_stop(buf->b_رڪن->tl_ڪم, عدم, how);

    // wait for up to a second for the job to die
    لكل (ڳڻپ = 0; ڳڻپ < 100; ++ڳڻپ)
    {
	ڪم_T *ڪم;

	// buffer, terminal and job may be cleaned up while waiting
	إذا (!buf_valid(buf)
		|| buf->b_رڪن == عدم
		|| buf->b_رڪن->tl_ڪم == عدم)
	    أرجع OK;
	ڪم = buf->b_رڪن->tl_ڪم;

	// Call job_status() to update jv_status. It may cause the job to be
	// cleaned up but it won't be freed.
	ڪم_حالت(ڪم);
	إذا (ڪم->jv_حالت >= ڪم_ENDED)
	    أرجع OK;

	ui_delay(10L, TRUE);
	رڪن_flush_messages();
    }
    أرجع FAIL;
}

/*
 * Add the last line of the scrollback buffer to the buffer in the window.
 */
    ساكن فراغ
جوڙيو_scrollback_سٽ_منزل_بفر(رڪن_T *رڪن, char_u *متن, صحيح len, صحيح append)
{
    buf_T	*buf = رڪن->tl_بفر;
    صحيح		خالي_جايون_2 = (buf->b_ml.ml_flags & ML_خالي_جايون);
    linenr_T	lnum = buf->b_ml.ml_سٽ_ڳڻپ;

#جيڪڏهن_وصف_ٿيل MSWIN
    char_u *tmp = متن;
    إذا (!enc_utf8 && enc_codepage > 0)
    {
	WCHAR   *ret = عدم;
	صحيح	ڊيگهائي = 0;

	Multi_Byte_منزل_Wide_Char_alloc(CP_UTF8, 0, (حرف*)متن, len + 1,
							   &ret, &ڊيگهائي);
	إذا (ret != عدم)
	{
	    Wide_Char_منزل_Multi_Byte_alloc(enc_codepage, 0,
				      ret, ڊيگهائي, (حرف **)&متن, &len, 0, 0);
	    vim_free(ret);
	}
    }
#جيڪڏهن_پورو

    إذا (append)
    {
	char_u *prev_متن = ml_get_buf(buf, lnum, FALSE);
	size_t prev_len = STRLEN(prev_متن);

	char_u *both = alloc(len + prev_len + 2);
	إذا (both == عدم)
	    أرجع;
	vim_strncpy(both, prev_متن, prev_len + 1);
	vim_strncpy(both + prev_len, متن, len + 1);

	curbuf = buf;
	ml_replace(lnum, both, FALSE);
	curbuf = curwin->w_بفر;
    }
    وإلا
	ml_append_buf(buf, lnum, متن, len + 1, FALSE);

#جيڪڏهن_وصف_ٿيل MSWIN
    إذا (tmp != متن)
	vim_free(متن);
#جيڪڏهن_پورو
    إذا (خالي_جايون_2)
    {
	// Delete the empty line that was in the empty buffer.
	curbuf = buf;
	ml_delete(1);
	curbuf = curwin->w_بفر;
    }
}

    ساكن فراغ
cell2cellattr(ثابت VTermScreenCell *cell, cellattr_T *attr)
{
    attr->ويڪر = cell->ويڪر;
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
جوڙيو_خالي_جايون_scrollback(رڪن_T *رڪن, cellattr_T *fill_attr, صحيح lnum)
{
    إذا (ga_grow(&رڪن->tl_scrollback, 1) == FAIL)
	أرجع FALSE;

    sb_سٽ_T *سٽ = (sb_سٽ_T *)رڪن->tl_scrollback.ga_معلومات
	+ رڪن->tl_scrollback.ga_len;

    إذا (lnum > 0)
    {
	صحيح i;

	لكل (i = 0; i < رڪن->tl_scrollback.ga_len - lnum; ++i)
	{
	    *سٽ = *(سٽ - 1);
	    --سٽ;
	}
    }
    سٽ->sb_کانن_ڳڻپ = 0;
    سٽ->sb_بائيٽون = 0;
    سٽ->sb_cells = عدم;
    سٽ->sb_fill_attr = *fill_attr;
    ++رڪن->tl_scrollback.ga_len;
    أرجع OK;
}

/*
 * Remove the terminal contents from the scrollback and the buffer.
 * Used before adding a new scrollback line or updating the buffer for lines
 * displayed in the terminal.
 */
    ساكن فراغ
cleanup_scrollback(رڪن_T *رڪن)
{
    sb_سٽ_T	*سٽ;
    garray_T	*gap;
    char_u	*bufline;
    size_t      bufline_ڊيگهائي;

    curbuf = رڪن->tl_بفر;
    gap = &رڪن->tl_scrollback;
    bufline = ml_get_buf(curbuf, curbuf->b_ml.ml_سٽ_ڳڻپ, FALSE);
    bufline_ڊيگهائي = STRLEN(bufline);
    طالما (رڪن->tl_scrollback_snapshot && gap->ga_len > 0)
    {
	سٽ = (sb_سٽ_T *)gap->ga_معلومات + gap->ga_len - 1;
	إذا (سٽ->sb_بائيٽون < 0 || (size_t)سٽ->sb_بائيٽون > bufline_ڊيگهائي)
	    اكسر;
	bufline_ڊيگهائي -= سٽ->sb_بائيٽون;
	إذا (!bufline_ڊيگهائي)
	{
	    ml_delete(curbuf->b_ml.ml_سٽ_ڳڻپ);
	    bufline = ml_get_buf(curbuf, curbuf->b_ml.ml_سٽ_ڳڻپ, FALSE);
	    bufline_ڊيگهائي = STRLEN(bufline);
	}
	vim_free(سٽ->sb_cells);
	--gap->ga_len;
	--رڪن->tl_scrollback_snapshot;
    }
    إذا (bufline_ڊيگهائي < STRLEN(bufline))
    {
	char_u *shortened = vim_strnsave(bufline, bufline_ڊيگهائي);
	ml_replace(curbuf->b_ml.ml_سٽ_ڳڻپ, shortened, FALSE);
    }
    curbuf = curwin->w_بفر;
    إذا (curbuf == رڪن->tl_بفر)
	check_cursor();
}

/*
 * Add the current lines of the terminal to scrollback and to the buffer.
 */
    ساكن فراغ
update_snapshot(رڪن_T *رڪن)
{
    VTermScreen	    *screen;
    VTermState	    *state;
    صحيح		    len;
    صحيح		    سٽون_skipped = 0;
    VTermPos	    pos;
    VTermScreenCell cell;
    cellattr_T	    fill_attr, new_fill_attr;
    cellattr_T	    *p;

    ch_log(رڪن->tl_ڪم == عدم ? عدم : رڪن->tl_ڪم->jv_channel,
				  "Adding terminal window snapshot to buffer");

    // First remove the lines that were appended before, they might be
    // outdated.
    cleanup_scrollback(رڪن);

    screen = vterm_obtain_screen(رڪن->tl_vterm);
    state = vterm_obtain_state(رڪن->tl_vterm);
    fill_attr = new_fill_attr = رڪن->tl_default_color;
    لكل (pos.صف = 0; pos.صف < رڪن->tl_صفون; ++pos.صف)
    {
	len = 0;
	لكل (pos.کانو = 0; pos.کانو < رڪن->tl_کانن_ڳڻپ; ++pos.کانو)
	    إذا (vterm_screen_get_cell(screen, pos, &cell) != 0
						       && cell.chars[0] != NUL)
	    {
		len = pos.کانو + 1;
		new_fill_attr = رڪن->tl_default_color;
	    }
	    وإلا
		// Assume the last attr is the filler attr.
		cell2cellattr(&cell, &new_fill_attr);

	إذا (len == 0 && equal_celattr(&new_fill_attr, &fill_attr))
	    ++سٽون_skipped;
	وإلا
	{
	    طالما (سٽون_skipped > 0)
	    {
		// Line was skipped, add an empty line.
		--سٽون_skipped;
		إذا (جوڙيو_خالي_جايون_scrollback(رڪن, &fill_attr, 0) == OK)
		{
		    جوڙيو_scrollback_سٽ_منزل_بفر(رڪن, (char_u *)"", 0, 0);
		    ++رڪن->tl_scrollback_snapshot;
		}
	    }

	    إذا (len == 0)
		p = عدم;
	    وإلا
		p = ALLOC_MULT(cellattr_T, len);
	    إذا ((p != عدم || len == 0)
				     && ga_grow(&رڪن->tl_scrollback, 1) == OK)
	    {
		garray_T    ga;
		صحيح	    ويڪر;
		ثابت VTerm_سٽ_Info *lineinfo;
		sb_سٽ_T   *سٽ = (sb_سٽ_T *)رڪن->tl_scrollback.ga_معلومات
						  + رڪن->tl_scrollback.ga_len;

		lineinfo = vterm_state_get_lineinfo(state, pos.صف);

		ga_init2(&ga, 1, 100);
		لكل (pos.کانو = 0; pos.کانو < len; pos.کانو += ويڪر)
		{
		    إذا (vterm_screen_get_cell(screen, pos, &cell) == 0)
		    {
			ويڪر = 1;
			CLEAR_اشارو(p + pos.کانو);
			إذا (ga_grow(&ga, 1) == OK)
			    ga.ga_len += utf_char2bytes(' ',
					     (char_u *)ga.ga_معلومات + ga.ga_len);
		    }
		    وإلا
		    {
			ويڪر = cell.ويڪر;

			cell2cellattr(&cell, &p[pos.کانو]);
			إذا (ويڪر == 2)
			    // second cell of double-width character has the
			    // same attributes.
			    p[pos.کانو + 1] = p[pos.کانو];

			// Each character can be up to 6 bytes.
			إذا (ga_grow(&ga, VTERM_MAX_CHARS_في_CELL * 6) == OK)
			{
			    صحيح	    i;
			    صحيح	    c;

			    لكل (i = 0; i < VTERM_MAX_CHARS_في_CELL &&
				    ((c = cell.chars[i]) > 0 || i == 0); ++i)
				ga.ga_len += utf_char2bytes(c == NUL ? ' ' : c,
					     (char_u *)ga.ga_معلومات + ga.ga_len);
			}
		    }
		}
		سٽ->sb_کانن_ڳڻپ = len;
		سٽ->sb_بائيٽون = ga.ga_len;
		سٽ->sb_cells = p;
		سٽ->sb_fill_attr = new_fill_attr;
		سٽ->continuation = (char_u)lineinfo->continuation;
		fill_attr = new_fill_attr;
		++رڪن->tl_scrollback.ga_len;
		++رڪن->tl_scrollback_snapshot;

		إذا (ga_grow(&ga, 1) == FAIL)
		    جوڙيو_scrollback_سٽ_منزل_بفر(رڪن, (char_u *)"", 0, 0);
		وإلا
		{
		    *((char_u *)ga.ga_معلومات + ga.ga_len) = NUL;
		    جوڙيو_scrollback_سٽ_منزل_بفر(رڪن, ga.ga_معلومات, ga.ga_len,
						  lineinfo->continuation);
		}
		ga_clear(&ga);
	    }
	    وإلا
		vim_free(p);
	}
    }

    // Add trailing empty lines.
    لكل (pos.صف = رڪن->tl_scrollback.ga_len;
	    pos.صف < رڪن->tl_scrollback_scrolled + رڪن->tl_cursor_pos.صف;
	    ++pos.صف)
    {
	إذا (جوڙيو_خالي_جايون_scrollback(رڪن, &fill_attr, 0) == OK)
	{
	    جوڙيو_scrollback_سٽ_منزل_بفر(رڪن, (char_u *)"", 0, 0);
	    ++رڪن->tl_scrollback_snapshot;
	}
    }

    رڪن->tl_dirty_snapshot = FALSE;
#جيڪڏهن_وصف_ٿيل FEAT_TIMERS
    رڪن->tl_timer_سيٽ = FALSE;
#جيڪڏهن_پورو
}

/*
 * Loop over all windows in the current tab, and also curwin, which is not
 * encountered when using a terminal in a popup window.
 * Return TRUE if "*wp" was set to the next window.
 */
    ساكن صحيح
for_all_windows_and_curwin(win_T **wp, صحيح *did_curwin)
{
    إذا (*wp == عدم)
	*wp = firstwin;
    وإلا إذا ((*wp)->w_ايندڙ != عدم)
	*wp = (*wp)->w_ايندڙ;
    وإلا إذا (!*did_curwin)
	*wp = curwin;
    وإلا
	أرجع FALSE;
    إذا (*wp == curwin)
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
may_move_terminal_منزل_بفر(رڪن_T *رڪن, صحيح redraw)
{
    إذا (رڪن->tl_vterm == عدم)
	أرجع;

    // Update the snapshot only if something changes or the buffer does not
    // have all the lines.
    إذا (رڪن->tl_dirty_snapshot || رڪن->tl_بفر->b_ml.ml_سٽ_ڳڻپ
					       <= رڪن->tl_بفر_scrolled)
	update_snapshot(رڪن);

    إذا (redraw)
    {
	win_T	    *wp = عدم;
	صحيح	    did_curwin = FALSE;

	طالما (for_all_windows_and_curwin(&wp, &did_curwin))
	{
	    إذا (wp->w_بفر == رڪن->tl_بفر)
	    {
		wp->w_cursor.lnum = رڪن->tl_بفر->b_ml.ml_سٽ_ڳڻپ;
		wp->w_cursor.کانو = 0;
		wp->w_valid = 0;
		إذا (wp->w_cursor.lnum >= wp->w_اوچائي)
		{
		    linenr_T min_topline = wp->w_cursor.lnum - wp->w_اوچائي + 1;

		    إذا (wp->w_topline < min_topline)
			wp->w_topline = min_topline;
		}
		redraw_win_later(wp, UPD_NOT_VALID);
	    }
	}
    }
}

#إذا defined(FEAT_TIMERS)
/*
 * Check if any terminal timer expired.  If so, copy text from the terminal to
 * the buffer.
 * Return the time until the next timer will expire.
 */
    صحيح
رڪن_check_timers(صحيح ايندڙ_due_arg, proftime_T *now)
{
    رڪن_T  *رڪن;
    صحيح	    ايندڙ_due = ايندڙ_due_arg;

    FOR_ALL_TERMS(رڪن)
    {
	إذا (رڪن->tl_timer_سيٽ && !رڪن->tl_عام_گهڻائي)
	{
	    ڊگهو    this_due = proftime_وقت_کاٻو(&رڪن->tl_timer_due, now);

	    إذا (this_due <= 1)
	    {
		رڪن->tl_timer_سيٽ = FALSE;
		may_move_terminal_منزل_بفر(رڪن, FALSE);
	    }
	    وإلا إذا (ايندڙ_due == -1 || ايندڙ_due > this_due)
		ايندڙ_due = this_due;
	}
    }

    أرجع ايندڙ_due;
}
#جيڪڏهن_پورو

/*
 * When "normal_mode" is TRUE set the terminal to Terminal-Normal mode,
 * otherwise end it.
 */
    ساكن فراغ
سيٽ_terminal_گهڻائي(رڪن_T *رڪن, صحيح عام_گهڻائي)
{
    رڪن->tl_عام_گهڻائي = عام_گهڻائي;
    may_trigger_modechanged();
    إذا (!عام_گهڻائي)
	handle_postponed_scrollback(رڪن);
    VIM_CLEAR(رڪن->tl_حالت_متن);
    إذا (رڪن->tl_بفر == curbuf)
	maketitle();
}

/*
 * Called after the job is finished and Terminal mode is not active:
 * Move the vterm contents into the scrollback buffer and free the vterm.
 */
    ساكن فراغ
cleanup_vterm(رڪن_T *رڪن)
{
    سيٽ_terminal_گهڻائي(رڪن, FALSE);
    إذا (رڪن->tl_پڄاڻي != TL_پڄاڻي_CLOSE)
	may_move_terminal_منزل_بفر(رڪن, TRUE);
    رڪن_free_vterm(رڪن);
}

/*
 * Switch from Terminal-Job mode to Terminal-Normal mode.
 * Suspends updating the terminal window.
 */
    ساكن فراغ
رڪن_enter_عام_گهڻائي(فراغ)
{
    رڪن_T     *رڪن = curbuf->b_رڪن;
    linenr_T   lnum;
    صحيح        کانو;

    سيٽ_terminal_گهڻائي(رڪن, TRUE);

    // Append the current terminal contents to the buffer.
    may_move_terminal_منزل_بفر(رڪن, TRUE);

    // Move the window cursor to the position of the cursor in the
    // terminal.
    lnum = رڪن->tl_بفر_scrolled + 1 + رڪن->tl_cursor_pos.صف;
    کانو = رڪن->tl_cursor_pos.کانو;
    scrollbackline_pos_اندريون_buf(رڪن, رڪن->tl_cursor_pos.صف + رڪن->tl_scrollback_scrolled, &lnum, &کانو, عدم);

    curwin->w_cursor.lnum = lnum;
    check_cursor();
    إذا (coladvance(کانو) == FAIL)
	coladvance(MAXCOL);
    curwin->w_سيٽ_curswant = true;
    curwin->w_topline = رڪن->tl_بفر_scrolled + 1;
}

/*
 * Returns TRUE if the current window contains a terminal and we are in
 * Terminal-Normal mode.
 */
    صحيح
رڪن_اندريون_عام_گهڻائي(buf_T *buf)
{
    رڪن_T *رڪن = buf->b_رڪن;

    أرجع رڪن != عدم && رڪن->tl_عام_گهڻائي;
}

/*
 * Switch from Terminal-Normal mode to Terminal-Job mode.
 * Restores updating the terminal window.
 */
    فراغ
رڪن_enter_ڪم_گهڻائي(فراغ)
{
    رڪن_T	*رڪن = curbuf->b_رڪن;

    سيٽ_terminal_گهڻائي(رڪن, FALSE);

    إذا (رڪن->tl_channel_closed)
	cleanup_vterm(رڪن);
    redraw_buf_and_حالت_later(curbuf, UPD_NOT_VALID);
#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
    إذا (WIN_IS_POPUP(curwin))
	redraw_later(UPD_NOT_VALID);
#جيڪڏهن_پورو
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
    NRKS_سيٽ,	    // no_reduce_keys was incremented in term_vgetc() or
		    // check_no_reduce_keys(), must be decremented.
} reduce_ڪنجي_state_T;

ساكن reduce_ڪنجي_state_T  no_reduce_ڪنجي_state = NRKS_NONE;

/*
 * Return TRUE if the term is using modifyOtherKeys level 2 or the kitty
 * keyboard protocol.
 */
    ساكن صحيح
vterm_using_ڪنجي_protocol(فراغ)
{
    أرجع curbuf->b_رڪن != عدم
	&& curbuf->b_رڪن->tl_vterm != عدم
	&& (vterm_is_modify_other_ڪنجيون(curbuf->b_رڪن->tl_vterm)
		|| vterm_is_kitty_keyboard(curbuf->b_رڪن->tl_vterm));
}

    فراغ
check_no_reduce_ڪنجيون(فراغ)
{
    إذا (no_reduce_ڪنجي_state != NRKS_CHECK
	    || no_reduce_ڪنجيون >= 1
	    || curbuf->b_رڪن == عدم
	    || curbuf->b_رڪن->tl_vterm == عدم)
	أرجع;

    إذا (vterm_using_ڪنجي_protocol())
    {
	// "modify_other_keys" or kitty keyboard protocol was enabled while
	// waiting.
	no_reduce_ڪنجي_state = NRKS_سيٽ;
	++no_reduce_ڪنجيون;
    }
}

/*
 * Get a key from the user with terminal mode mappings.
 * Note: while waiting a terminal may be closed and freed if the channel is
 * closed and ++close was used.  This may even happen before we get here.
 */
    ساكن صحيح
رڪن_vgetc(فراغ)
{
    صحيح c;
    صحيح save_State = State;

    State = گهڻائي_TERMINAL;
    got_int = FALSE;
#جيڪڏهن_وصف_ٿيل MSWIN
    ctrl_break_was_pressed = FALSE;
#جيڪڏهن_پورو

    إذا (vterm_using_ڪنجي_protocol())
    {
	++no_reduce_ڪنجيون;
	no_reduce_ڪنجي_state = NRKS_سيٽ;
    }
    وإلا
    {
	no_reduce_ڪنجي_state = NRKS_CHECK;
    }

    c = vgetc();
    got_int = FALSE;
    State = save_State;

    إذا (no_reduce_ڪنجي_state == NRKS_سيٽ)
	--no_reduce_ڪنجيون;
    no_reduce_ڪنجي_state = NRKS_NONE;

    أرجع c;
}

ساكن صحيح	mouse_was_outside = FALSE;

/*
 * Send key "c" with modifiers "modmask" to terminal.
 * Return FAIL when the key needs to be handled in Normal mode.
 * Return OK when the key was dropped or sent to the terminal.
 */
    صحيح
send_ڪنجيون_منزل_رڪن(رڪن_T *رڪن, صحيح c, صحيح modmask, صحيح typed)
{
    حرف	نياپو[ڪنجي_BUF_LEN];
    size_t	len;
    صحيح		dragging_outside = FALSE;

    // Catch keys that need to be handled as in Normal mode.
    بدل (c)
    {
	حالة NUL:
	حالة K_ZERO:
	    إذا (typed)
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
		صحيح	صف = mouse_صف;
		صحيح	کانو = mouse_کانو;

#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
		إذا (popup_is_popup(curwin))
		{
		    صف -= popup_چوٽي_extra(curwin);
		    کانو -= popup_کاٻو_extra(curwin);
		}
#جيڪڏهن_پورو
		إذا (صف < W_WINROW(curwin)
			|| صف >= (W_WINROW(curwin) + curwin->w_اوچائي)
			|| کانو < curwin->w_wincol
			|| کانو >= W_ENDCOL(curwin)
			|| dragging_outside)
		{
		    // click or scroll outside the current window or on status
		    // line or vertical separator
		    إذا (typed)
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
    إذا (typed)
	mouse_was_outside = FALSE;

    // Convert the typed key to a sequence of bytes for the job.
    len = رڪن_convert_ڪنجي(رڪن, c, modmask, نياپو);
    إذا (len > 0)
	// TODO: if FAIL is returned, stop?
	channel_send(رڪن->tl_ڪم->jv_channel, get_tty_part(رڪن),
						(char_u *)نياپو, (صحيح)len, عدم);

    أرجع OK;
}

/*
 * Handle CTRL-W "": send register contents to the job.
 */
    ساكن فراغ
رڪن_paste_رجسٽر(صحيح prev_c اڻ_استعمال_ٿيل)
{
    صحيح		c;
    list_T	*l;
    listitem_T	*شي;
    ڊگهو	reglen = 0;
    صحيح		type;

    إذا (جوڙيو_منزل_showcmd(prev_c))
    إذا (جوڙيو_منزل_showcmd('"'))
	ٻاهريون_flush();

    c = رڪن_vgetc();
    clear_showcmd();

    إذا (!رڪن_use_loop())
	// job finished while waiting for a character
	أرجع;

    // CTRL-W "= prompt for expression to evaluate.
    إذا (c == '=' && get_اظهار_رجسٽر() != '=')
	أرجع;
    إذا (!رڪن_use_loop())
	// job finished while waiting for a character
	أرجع;

    l = (list_T *)get_reg_contents(c, GREG_LIST);
    إذا (l == عدم)
	أرجع;

    type = get_reg_type(c, &reglen);
    FOR_ALL_LIST_شيون(l, شي)
    {
	char_u *s = tv_get_string(&شي->li_tv);
#جيڪڏهن_وصف_ٿيل MSWIN
	char_u *tmp = s;

	إذا (!enc_utf8 && enc_codepage > 0)
	{
	    WCHAR   *ret = عدم;
	    صحيح	ڊيگهائي = 0;

	    Multi_Byte_منزل_Wide_Char_alloc(enc_codepage, 0, (حرف *)s,
		    (صحيح)STRLEN(s), &ret, &ڊيگهائي);
	    إذا (ret != عدم)
	    {
		Wide_Char_منزل_Multi_Byte_alloc(CP_UTF8, 0,
			ret, ڊيگهائي, (حرف **)&s, &ڊيگهائي, 0, 0);
		vim_free(ret);
	    }
	}
#جيڪڏهن_پورو
	channel_send(curbuf->b_رڪن->tl_ڪم->jv_channel, PART_اندريون,
		s, (صحيح)STRLEN(s), عدم);
#جيڪڏهن_وصف_ٿيل MSWIN
	إذا (tmp != s)
	    vim_free(s);
#جيڪڏهن_پورو

	إذا (شي->li_ايندڙ != عدم || type == MLINE)
	    channel_send(curbuf->b_رڪن->tl_ڪم->jv_channel, PART_اندريون,
		    (char_u *)"\r", 1, عدم);
    }
    list_free(l);
}

/*
 * Return TRUE when waiting for a character in the terminal, the cursor of the
 * terminal should be displayed.
 */
    صحيح
terminal_is_سرگرم(فراغ)
{
    أرجع اندريون_terminal_loop != عدم;
}

/*
 * Return the highlight group ID for the terminal and the window.
 */
    ساكن صحيح
رڪن_get_highlight_id(رڪن_T *رڪن, win_T *wp)
{
    char_u *name;

    إذا (wp != عدم && wp->w_hlfwin_id != 0)
	name = syn_id2name(wp->w_hlfwin_id);
    وإلا إذا (رڪن->tl_highlight_name != عدم)
	name = رڪن->tl_highlight_name;
    وإلا
	name = (char_u*)"Terminal";

    أرجع syn_name2id(name);
}

#إذا defined(FEAT_GUI)
    cursorentry_T *
رڪن_get_cursor_shape(guicolor_T *fg, guicolor_T *bg)
{
    رڪن_T		 *رڪن = اندريون_terminal_loop;
    ساكن cursorentry_T داخلا;
    صحيح			 id;
    guicolor_T		 رڪن_fg = INVALCOLOR;
    guicolor_T		 رڪن_bg = INVALCOLOR;

    CLEAR_ميدان(داخلا);
    داخلا.shape = داخلا.mshape =
	رڪن->tl_cursor_shape == VTERM_PROP_CURSORSHAPE_UNDERLINE ? SHAPE_HOR :
	رڪن->tl_cursor_shape == VTERM_PROP_CURSORSHAPE_BAR_کاٻو ? SHAPE_VER :
	SHAPE_بلاڪ;
    داخلا.percentage = 20;
    إذا (رڪن->tl_cursor_blink)
    {
	داخلا.blinkwait = 700;
	داخلا.blinkon = 400;
	داخلا.blinkoff = 250;
    }

    // The highlight group overrules the defaults.
    id = رڪن_get_highlight_id(رڪن, curwin);
    إذا (id != 0)
	syn_id2colors(id, &رڪن_fg, &رڪن_bg);
    إذا (رڪن_bg != INVALCOLOR)
	*fg = رڪن_bg;
    وإلا
	*fg = gui.پٺ_pixel;

    إذا (رڪن->tl_cursor_color == عدم)
    {
	إذا (رڪن_fg != INVALCOLOR)
	    *bg = رڪن_fg;
	وإلا
	    *bg = gui.norm_pixel;
    }
    وإلا
	*bg = color_name2handle(رڪن->tl_cursor_color);
    داخلا.name = "n";
    داخلا.استعمال_ٿيل_for = SHAPE_CURSOR;

    أرجع &داخلا;
}
#جيڪڏهن_پورو

    ساكن فراغ
may_output_cursor_props(فراغ)
{
    إذا (!cursor_color_equal(آخري_سيٽ_cursor_color, desired_cursor_color)
	    || آخري_سيٽ_cursor_shape != desired_cursor_shape
	    || آخري_سيٽ_cursor_blink != desired_cursor_blink)
    {
	cursor_color_copy(&آخري_سيٽ_cursor_color, desired_cursor_color);
	آخري_سيٽ_cursor_shape = desired_cursor_shape;
	آخري_سيٽ_cursor_blink = desired_cursor_blink;
	رڪن_cursor_color(cursor_color_get(desired_cursor_color));
	إذا (desired_cursor_shape == -1 || desired_cursor_blink == -1)
	    // this will restore the initial cursor style, if possible
	    ui_cursor_shape_forced(TRUE);
	وإلا
	    رڪن_cursor_shape(desired_cursor_shape, desired_cursor_blink);
    }
}

/*
 * Set the cursor color and shape, if not last set to these.
 */
    ساكن فراغ
may_سيٽ_cursor_props(رڪن_T *رڪن)
{
#جيڪڏهن_وصف_ٿيل FEAT_GUI
    // For the GUI the cursor properties are obtained with
    // term_get_cursor_shape().
    إذا (gui.اندريون_use)
	أرجع;
#جيڪڏهن_پورو
    إذا (اندريون_terminal_loop == رڪن)
    {
	cursor_color_copy(&desired_cursor_color, رڪن->tl_cursor_color);
	desired_cursor_shape = رڪن->tl_cursor_shape;
	desired_cursor_blink = رڪن->tl_cursor_blink;
	may_output_cursor_props();
    }
}

/*
 * Reset the desired cursor properties and restore them when needed.
 */
    ساكن فراغ
prepare_restore_cursor_props(فراغ)
{
#جيڪڏهن_وصف_ٿيل FEAT_GUI
    إذا (gui.اندريون_use)
	أرجع;
#جيڪڏهن_پورو
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
رڪن_use_loop_check(صحيح check_ڪم_حالت)
{
    رڪن_T *رڪن = curbuf->b_رڪن;

    أرجع رڪن != عدم
	&& !رڪن->tl_عام_گهڻائي
	&& رڪن->tl_vterm != عدم
	&& رڪن_ڪم_running_check(رڪن, check_ڪم_حالت);
}

/*
 * Returns TRUE if the current window contains a terminal and we are sending
 * keys to the job.
 */
    صحيح
رڪن_use_loop(فراغ)
{
    أرجع رڪن_use_loop_check(FALSE);
}

/*
 * Called when entering a window with the mouse.  If this is a terminal window
 * we may want to change state.
 */
    فراغ
رڪن_win_entered(فراغ)
{
    رڪن_T *رڪن = curbuf->b_رڪن;

    إذا (رڪن == عدم)
	أرجع;

    إذا (رڪن_use_loop_check(TRUE))
    {
	reset_VIsual_and_resel();
	إذا (State & گهڻائي_وجهو)
	    stop_وجهو_گهڻائي = TRUE;
    }
    mouse_was_outside = FALSE;
    enter_mouse_کانو = mouse_کانو;
    enter_mouse_صف = mouse_صف;
}

    فراغ
رڪن_focus_change(صحيح اندريون_focus)
{
    رڪن_T *رڪن = curbuf->b_رڪن;

    إذا (رڪن == عدم || رڪن->tl_vterm == عدم)
	أرجع;

    VTermState	*state = vterm_obtain_state(رڪن->tl_vterm);

    إذا (اندريون_focus)
	vterm_state_focus_اندريون(state);
    وإلا
	vterm_state_focus_ٻاهريون(state);
    رڪن_forward_output(رڪن);
}

/*
 * vgetc() may not include CTRL in the key when modify_other_keys is set.
 * Return the Ctrl-key value in that case.
 */
    ساكن صحيح
raw_c_منزل_ctrl(صحيح c)
{
    إذا ((mod_نقاب & MOD_نقاب_CTRL)
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
ctrl_منزل_raw_c(صحيح c)
{
    إذا (c < 0x20 && vterm_using_ڪنجي_protocol())
    {
	mod_نقاب |= MOD_نقاب_CTRL;
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
#جيڪڏهن_وصف_ٿيل UNIX
    صحيح		tty_fd = curbuf->b_رڪن->tl_ڪم->jv_channel
				 ->ch_part[get_tty_part(curbuf->b_رڪن)].ch_fd;
#جيڪڏهن_پورو
    صحيح		restore_cursor = FALSE;

    // Remember the terminal we are sending keys to.  However, the terminal
    // might be closed while waiting for a character, e.g. typing "exit" in a
    // shell and ++close was used.  Therefore use curbuf->b_term instead of a
    // stored reference.
    اندريون_terminal_loop = curbuf->b_رڪن;

    إذا (*curwin->w_p_twk != NUL)
    {
	termwinkey = string_منزل_ڪنجي(curwin->w_p_twk, TRUE);
	إذا (termwinkey == Ctrl_W)
	    termwinkey = 0;
    }
    position_cursor(curwin, &curbuf->b_رڪن->tl_cursor_pos);
    may_سيٽ_cursor_props(curbuf->b_رڪن);

    طالما (blocking || vpeekc_nomap() != NUL)
    {
#جيڪڏهن_وصف_ٿيل FEAT_GUI
	إذا (curbuf->b_رڪن != عدم && !curbuf->b_رڪن->tl_system)
#جيڪڏهن_پورو
	    // TODO: skip screen update when handling a sequence of keys.
	    // Repeat redrawing in case a message is received while redrawing.
	    طالما (must_redraw != 0)
		إذا (update_screen(0) == FAIL)
		    اكسر;
	إذا (!رڪن_use_loop_check(TRUE) || اندريون_terminal_loop != curbuf->b_رڪن)
	    // job finished while redrawing
	    اكسر;

	update_cursor(curbuf->b_رڪن, FALSE);
	restore_cursor = TRUE;

	raw_c = رڪن_vgetc();
	إذا (!رڪن_use_loop_check(TRUE) || اندريون_terminal_loop != curbuf->b_رڪن)
	{
	    // Job finished while waiting for a character.  Push back the
	    // received character.
	    إذا (raw_c != K_IGNORE)
		vungetc(raw_c);
	    اكسر;
	}
	إذا (raw_c == K_IGNORE)
	    استمر;
	c = raw_c_منزل_ctrl(raw_c);

#جيڪڏهن_وصف_ٿيل UNIX
	/*
	 * The shell or another program may change the tty settings.  Getting
	 * them for every typed character is a bit of overhead, but it's needed
	 * for the first character typed, e.g. when Vim starts in a shell.
	 */
	إذا (mch_isatty(tty_fd))
	{
	    ttyinfo_T info;

	    // Get the current backspace character of the pty.
	    إذا (get_tty_info(tty_fd, &info) == OK)
		رڪن_backspace_char = info.backspace;
	}
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل MSWIN
	// On Windows winpty handles CTRL-C, don't send a CTRL_C_EVENT.
	// Use CTRL-BREAK to kill the job.
	إذا (ctrl_break_was_pressed)
	    mch_signal_ڪم(curbuf->b_رڪن->tl_ڪم, (char_u *)"kill");
#جيڪڏهن_پورو
	// Was either CTRL-W (termwinkey) or CTRL-\ pressed?
	// Not in a system terminal.
	إذا ((c == (termwinkey == 0 ? Ctrl_W : termwinkey) || c == Ctrl_BSL)
#جيڪڏهن_وصف_ٿيل FEAT_GUI
		&& !curbuf->b_رڪن->tl_system
#جيڪڏهن_پورو
		)
	{
	    صحيح	    prev_c = c;
	    صحيح	    prev_raw_c = raw_c;
	    صحيح	    prev_mod_نقاب = mod_نقاب;

	    إذا (جوڙيو_منزل_showcmd(c))
		ٻاهريون_flush();

	    raw_c = رڪن_vgetc();
	    c = raw_c_منزل_ctrl(raw_c);

	    clear_showcmd();

	    إذا (!رڪن_use_loop_check(TRUE)
					 || اندريون_terminal_loop != curbuf->b_رڪن)
		// job finished while waiting for a character
		اكسر;

	    إذا (prev_c == Ctrl_BSL)
	    {
		إذا (c == Ctrl_N)
		{
		    // CTRL-\ CTRL-N : go to Terminal-Normal mode.
		    رڪن_enter_عام_گهڻائي();
		    ret = FAIL;
		    اذهب theend;
		}
		// Send both keys to the terminal, first one here, second one
		// below.
		send_ڪنجيون_منزل_رڪن(curbuf->b_رڪن, prev_raw_c, prev_mod_نقاب,
									 TRUE);
	    }
	    وإلا إذا (c == Ctrl_C)
	    {
		// "CTRL-W CTRL-C" or 'termwinkey' CTRL-C: end the job
		mch_signal_ڪم(curbuf->b_رڪن->tl_ڪم, (char_u *)"kill");
	    }
	    وإلا إذا (c == '.')
	    {
		// "CTRL-W .": send CTRL-W to the job
		// "'termwinkey' .": send 'termwinkey' to the job
		raw_c = ctrl_منزل_raw_c(termwinkey == 0 ? Ctrl_W : termwinkey);
	    }
	    وإلا إذا (c == Ctrl_BSL)
	    {
		// "CTRL-W CTRL-\": send CTRL-\ to the job
		raw_c = ctrl_منزل_raw_c(Ctrl_BSL);
	    }
	    وإلا إذا (c == 'N')
	    {
		// CTRL-W N : go to Terminal-Normal mode.
		رڪن_enter_عام_گهڻائي();
		ret = FAIL;
		اذهب theend;
	    }
	    وإلا إذا (c == '"')
	    {
		رڪن_paste_رجسٽر(prev_c);
		استمر;
	    }
	    وإلا إذا (termwinkey == 0 || c != termwinkey)
	    {
		// space for CTRL-W, modifier, multi-byte char and NUL
		char_u buf[1 + 3 + MB_MAXBYTES + 1];

		// Put the command into the typeahead buffer, when using the
		// stuff buffer KeyStuffed is set and 'langmap' won't be used.
		buf[0] = Ctrl_W;
		buf[special_منزل_buf(c, mod_نقاب, FALSE, buf + 1) + 1] = NUL;
		ins_typebuf(buf, REMAP_NONE, 0, TRUE, FALSE);
		ret = OK;
		اذهب theend;
	    }
	}
#جيڪڏهن_وصف_ٿيل MSWIN
	إذا (!enc_utf8 && has_mbyte && raw_c >= 0x80)
	{
	    WCHAR   wc;
	    char_u  mb[3];

	    mb[0] = (بنانشان)raw_c >> 8;
	    mb[1] = raw_c;
	    إذا (Multi_Byte_منزل_Wide_Char(GetACP(), 0, (حرف*)mb, 2, &wc, 1) > 0)
		raw_c = wc;
	}
#جيڪڏهن_پورو
	إذا (send_ڪنجيون_منزل_رڪن(curbuf->b_رڪن, raw_c, mod_نقاب, TRUE) != OK)
	{
	    إذا (raw_c == K_MOUSEMOVE)
		// We are sure to come back here, don't reset the cursor color
		// and shape to avoid flickering.
		restore_cursor = FALSE;

	    ret = OK;
	    اذهب theend;
	}
    }
    ret = FAIL;

theend:
    اندريون_terminal_loop = عدم;
    إذا (restore_cursor)
	prepare_restore_cursor_props();

    // Move a snapshot of the screen contents to the buffer, so that completion
    // works in other buffers.
    إذا (curbuf->b_رڪن != عدم && !curbuf->b_رڪن->tl_عام_گهڻائي)
	may_move_terminal_منزل_بفر(curbuf->b_رڪن, FALSE);

    أرجع ret;
}

    ساكن فراغ
may_toggle_cursor(رڪن_T *رڪن)
{
    إذا (اندريون_terminal_loop != رڪن)
	أرجع;

    إذا (رڪن->tl_cursor_visible)
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

    إذا (VTERM_COLOR_IS_INVALID(color))
	أرجع 0;

    إذا (VTERM_COLOR_IS_اشاريي_وارو(color))
    {
	// Use the color as-is if possible, give up otherwise.
	إذا (color->اشاريو < t_colors)
	{
	    uint8_t اشاريو = color->اشاريو;
#جيڪڏهن_وصف_ٿيل MSWIN
	    // Convert ANSI palette to cterm color index.
	    // cterm_ansi_idx is a table for the reverse conversion from cterm
	    // color index to ANSI palette. However, the current table can
	    // be used for this conversion as well.
	    // Note: If the contents of cterm_ansi_idx change in the future, a
	    // different table may be needed for this purpose.
	    إذا (اشاريو < 16)
		اشاريو = cterm_ansi_idx[اشاريو];
#جيڪڏهن_پورو
	    أرجع اشاريو + 1;
	}
	// 8-color terminals can actually display twice as many colors by
	// setting the high-intensity/bold bit.
	وإلا إذا (t_colors == 8 && fg && color->اشاريو < 16)
	{
	    *boldp = TRUE;
	    أرجع (color->اشاريو & 7) + 1;
	}
	أرجع 0;
    }

    إذا (t_colors >= 256)
    {
	إذا (red == blue && red == green)
	{
	    // 24-color greyscale plus white and black
	    ساكن صحيح cutoff[23] = {
		    0x0D, 0x17, 0x21, 0x2B, 0x35, 0x3F, 0x49, 0x53, 0x5D, 0x67,
		    0x71, 0x7B, 0x85, 0x8F, 0x99, 0xA3, 0xAD, 0xB7, 0xC1, 0xCB,
		    0xD5, 0xDF, 0xE9};
	    صحيح i;

	    إذا (red < 5)
		أرجع 17; // 00/00/00
	    إذا (red > 245) // ff/ff/ff
		أرجع 232;
	    لكل (i = 0; i < 23; ++i)
		إذا (red < cutoff[i])
		    أرجع i + 233;
	    أرجع 256;
	}
	{
	    ساكن صحيح cutoff[5] = {0x2F, 0x73, 0x9B, 0xC3, 0xEB};
	    صحيح ri, gi, bi;

	    // 216-color cube
	    لكل (ri = 0; ri < 5; ++ri)
		إذا (red < cutoff[ri])
		    اكسر;
	    لكل (gi = 0; gi < 5; ++gi)
		إذا (green < cutoff[gi])
		    اكسر;
	    لكل (bi = 0; bi < 5; ++bi)
		إذا (blue < cutoff[bi])
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

    إذا (cellattrs->bold)
	attr |= HL_BOLD;
    إذا (cellattrs->underline)
	attr |= HL_UNDERLINE;
    إذا (cellattrs->italic)
	attr |= HL_ITALIC;
    إذا (cellattrs->strike)
	attr |= HL_STRIKETHROUGH;
    إذا (cellattrs->ابتو)
	attr |= HL_INVERSE;
    أرجع attr;
}

/*
 * Store Vterm attributes in "cell" from highlight flags.
 */
    ساكن فراغ
hl2vtermAttr(صحيح attr, cellattr_T *cell)
{
    CLEAR_ميدان(cell->attrs);
    إذا (attr & HL_BOLD)
	cell->attrs.bold = 1;
    إذا (attr & HL_UNDERLINE)
	cell->attrs.underline = 1;
    إذا (attr & HL_ITALIC)
	cell->attrs.italic = 1;
    إذا (attr & HL_STRIKETHROUGH)
	cell->attrs.strike = 1;
    إذا (attr & HL_INVERSE)
	cell->attrs.ابتو = 1;
}

/*
 * Convert the attributes of a vterm cell into an attribute index.
 */
    ساكن صحيح
cell2attr(
	رڪن_T			*رڪن,
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

    إذا (is_default_fg || is_default_bg)
    {
	إذا (wp != عدم && wp->w_hlfwin_id != 0)
	{
	    إذا (is_default_fg)
		fg = &wp->w_رڪن_hlfwin.fg;
	    إذا (is_default_bg)
		bg = &wp->w_رڪن_hlfwin.bg;
	}
	وإلا
	{
	    إذا (is_default_fg)
		fg = &رڪن->tl_default_color.fg;
	    إذا (is_default_bg)
		bg = &رڪن->tl_default_color.bg;
	}
    }

#جيڪڏهن_وصف_ٿيل FEAT_GUI
    إذا (gui.اندريون_use)
    {
	guicolor_T guifg = gui_mch_get_rgb_color(fg->red, fg->green, fg->blue);
	guicolor_T guibg = gui_mch_get_rgb_color(bg->red, bg->green, bg->blue);
	أرجع get_gui_attr_idx(attr, guifg, guibg);
    }
    وإلا
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_TERMGUICOLORS
    إذا (p_tgc)
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
#جيڪڏهن_پورو
    {
	صحيح bold = MAYBE;
	صحيح ctermfg = color2index(fg, TRUE, &bold);
	صحيح ctermbg = color2index(bg, FALSE, &bold);

	// with 8 colors set the bold attribute to get a bright foreground
	إذا (bold == TRUE)
	    attr |= HL_BOLD;

	أرجع get_cterm_attr_idx(attr, ctermfg, ctermbg);
    }
    أرجع 0;
}

    ساكن فراغ
سيٽ_dirty_snapshot(رڪن_T *رڪن)
{
    رڪن->tl_dirty_snapshot = TRUE;
#جيڪڏهن_وصف_ٿيل FEAT_TIMERS
    إذا (!رڪن->tl_عام_گهڻائي)
    {
	// Update the snapshot after 100 msec of not getting updates.
	profile_setlimit(100L, &رڪن->tl_timer_due);
	رڪن->tl_timer_سيٽ = TRUE;
    }
#جيڪڏهن_پورو
}

    ساكن صحيح
handle_damage(VTermRect rect, فراغ *user)
{
    رڪن_T *رڪن = (رڪن_T *)user;

    رڪن->tl_dirty_صف_شروعات = MIN(رڪن->tl_dirty_صف_شروعات, rect.شروعات_صف);
    رڪن->tl_dirty_صف_پڇاڙي = MAX(رڪن->tl_dirty_صف_پڇاڙي, rect.پڇاڙي_صف);
    سيٽ_dirty_snapshot(رڪن);
    redraw_buf_later(رڪن->tl_بفر, UPD_SOME_VALID);
    أرجع 1;
}

    ساكن فراغ
رڪن_scroll_مٿي(رڪن_T *رڪن, صحيح شروعات_صف, صحيح ڳڻپ)
{
    win_T		 *wp = عدم;
    صحيح			 did_curwin = FALSE;
    VTermColor		 fg, bg;
    VTermScreenCellAttrs attr;
    صحيح			 clear_attr;

    CLEAR_ميدان(attr);

    طالما (for_all_windows_and_curwin(&wp, &did_curwin))
    {
	إذا (wp->w_بفر == رڪن->tl_بفر)
	{
	    // Set the color to clear lines with.
	    vterm_state_get_default_colors(vterm_obtain_state(رڪن->tl_vterm),
								     &fg, &bg);
	    clear_attr = cell2attr(رڪن, wp, &attr, &fg, &bg);
	    win_del_سٽون(wp, شروعات_صف, ڳڻپ, FALSE, FALSE, clear_attr);
	}
    }
}

    ساكن صحيح
handle_moverect(VTermRect dest, VTermRect src, فراغ *user)
{
    رڪن_T	*رڪن = (رڪن_T *)user;
    صحيح		ڳڻپ = src.شروعات_صف - dest.شروعات_صف;

    // Scrolling up is done much more efficiently by deleting lines instead of
    // redrawing the text. But avoid doing this multiple times, postpone until
    // the redraw happens.
    إذا (dest.شروعات_کانو == src.شروعات_کانو
	    && dest.پڇاڙي_کانو == src.پڇاڙي_کانو
	    && dest.شروعات_صف < src.شروعات_صف)
    {
	إذا (dest.شروعات_صف == 0)
	    رڪن->tl_postponed_scroll += ڳڻپ;
	وإلا
	    رڪن_scroll_مٿي(رڪن, dest.شروعات_صف, ڳڻپ);
    }

    رڪن->tl_dirty_صف_شروعات = MIN(رڪن->tl_dirty_صف_شروعات, dest.شروعات_صف);
    رڪن->tl_dirty_صف_پڇاڙي = MIN(رڪن->tl_dirty_صف_پڇاڙي, dest.پڇاڙي_صف);
    سيٽ_dirty_snapshot(رڪن);

    // Note sure if the scrolling will work correctly, let's do a complete
    // redraw later.
    redraw_buf_later(رڪن->tl_بفر, UPD_NOT_VALID);
    أرجع 1;
}

    ساكن صحيح
handle_movecursor(
	VTermPos pos,
	VTermPos oldpos اڻ_استعمال_ٿيل,
	صحيح visible,
	فراغ *user)
{
    رڪن_T	*رڪن = (رڪن_T *)user;
    win_T	*wp = عدم;
    صحيح		did_curwin = FALSE;

    رڪن->tl_cursor_pos = pos;
    رڪن->tl_cursor_visible = visible;

    طالما (for_all_windows_and_curwin(&wp, &did_curwin))
    {
	إذا (wp->w_بفر == رڪن->tl_بفر)
	    position_cursor(wp, &pos);
    }
    إذا (رڪن->tl_بفر == curbuf && !رڪن->tl_عام_گهڻائي)
	// Don't redraw here, it will be done after
	// vterm_input_write() is finished.
	update_cursor(رڪن, FALSE);

    أرجع 1;
}

    ساكن صحيح
handle_settermprop(
	VTermProp prop,
	VTerm_قدر *قدر,
	فراغ *user)
{
    رڪن_T	*رڪن = (رڪن_T *)user;
    char_u	*strval = عدم;

    بدل (prop)
    {
	حالة VTERM_PROP_TITLE:
	    إذا (disable_vterm_title_for_testing)
		اكسر;
	    strval = vim_strnsave((char_u *)قدر->string.str,
							    قدر->string.len);
	    إذا (strval == عدم)
		اكسر;
	    vim_free(رڪن->tl_title);
	    // a blank title isn't useful, make it empty, so that "running" is
	    // displayed
	    إذا (*skipwhite(strval) == NUL)
		رڪن->tl_title = عدم;
	    // Same as blank
	    وإلا إذا (رڪن->tl_arg0_cmd != عدم
		    && STRNCMP(رڪن->tl_arg0_cmd, strval,
					  (صحيح)STRLEN(رڪن->tl_arg0_cmd)) == 0)
		رڪن->tl_title = عدم;
	    // Empty corrupted data of winpty
	    وإلا إذا (STRNCMP("  - ", strval, 4) == 0)
		رڪن->tl_title = عدم;
#جيڪڏهن_وصف_ٿيل MSWIN
	    وإلا إذا (!enc_utf8 && enc_codepage > 0)
	    {
		WCHAR   *ret = عدم;
		صحيح	ڊيگهائي = 0;

		Multi_Byte_منزل_Wide_Char_alloc(CP_UTF8, 0,
			(حرف*)قدر->string.str,
					(صحيح)قدر->string.len, &ret, &ڊيگهائي);
		إذا (ret != عدم)
		{
		    Wide_Char_منزل_Multi_Byte_alloc(enc_codepage, 0,
					ret, ڊيگهائي, (حرف**)&رڪن->tl_title,
					&ڊيگهائي, 0, 0);
		    vim_free(ret);
		}
	    }
#جيڪڏهن_پورو
	    وإلا
	    {
		رڪن->tl_title = strval;
		strval = عدم;
	    }
	    VIM_CLEAR(رڪن->tl_حالت_متن);
	    إذا (رڪن == curbuf->b_رڪن)
	    {
		maketitle();
		curwin->w_redr_حالت = true;
	    }
	    اكسر;

	حالة VTERM_PROP_CURSORVISIBLE:
	    رڪن->tl_cursor_visible = قدر->boolean;
	    may_toggle_cursor(رڪن);
	    ٻاهريون_flush();
	    اكسر;

	حالة VTERM_PROP_CURSORBLINK:
	    رڪن->tl_cursor_blink = قدر->boolean;
	    may_سيٽ_cursor_props(رڪن);
	    اكسر;

	حالة VTERM_PROP_CURSORSHAPE:
	    رڪن->tl_cursor_shape = قدر->عدد;
	    may_سيٽ_cursor_props(رڪن);
	    اكسر;

	حالة VTERM_PROP_CURSORCOLOR:
	    strval = vim_strnsave((char_u *)قدر->string.str,
							    قدر->string.len);
	    إذا (strval == عدم)
		اكسر;
	    cursor_color_copy(&رڪن->tl_cursor_color, strval);
	    may_سيٽ_cursor_props(رڪن);
	    اكسر;

	حالة VTERM_PROP_ALTSCREEN:
	    // TODO: do anything else?
	    رڪن->tl_using_altscreen = قدر->boolean;
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
handle_resize(صحيح صفون_2, صحيح کانن_ڳڻپ, فراغ *user)
{
    رڪن_T	*رڪن = (رڪن_T *)user;
    win_T	*wp;

    رڪن->tl_صفون = صفون_2;
    رڪن->tl_کانن_ڳڻپ = کانن_ڳڻپ;
    إذا (رڪن->tl_vterm_سائيز_بدليو)
	// Size was set by vterm_set_size(), don't set the window size.
	رڪن->tl_vterm_سائيز_بدليو = FALSE;
    وإلا
    {
	FOR_ALL_WINDOWS(wp)
	{
	    إذا (wp->w_بفر == رڪن->tl_بفر)
	    {
		win_setheight_win(صفون_2, wp);
		win_setwidth_win(کانن_ڳڻپ, wp);
	    }
	}
	redraw_buf_later(رڪن->tl_بفر, UPD_NOT_VALID);
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
حد_بندي_scrollback(رڪن_T *رڪن, garray_T *gap, صحيح update_بفر)
{
    إذا (gap->ga_len < رڪن->tl_بفر->b_p_twsl)
	أرجع;

    صحيح	todo = MAX(رڪن->tl_بفر->b_p_twsl / 10,
				     gap->ga_len - رڪن->tl_بفر->b_p_twsl);
    صحيح	i;
    sb_سٽ_T *sb_سٽون = (sb_سٽ_T *)gap->ga_معلومات;

    curbuf = رڪن->tl_بفر;
    لكل (i = 0; i < todo; ++i)
    {
	إذا (update_بفر && (!sb_سٽون[i].continuation || !i))
	{
	    ml_delete(1);
	    --رڪن->tl_بفر_scrolled;
	}
	vim_free(sb_سٽون[i].sb_cells);
    }
    // Continue until end of wrapped line
    لكل (; todo < gap->ga_len && sb_سٽون[todo].continuation; ++todo)
	vim_free(sb_سٽون[todo].sb_cells);
    curbuf = curwin->w_بفر;

    gap->ga_len -= todo;
    mch_memmove(gap->ga_معلومات,
		(sb_سٽ_T *)gap->ga_معلومات + todo,
		حجم(sb_سٽ_T) * gap->ga_len);
    إذا (update_بفر)
    {
	win_T *curwin_save = curwin;
	win_T *wp = عدم;

	رڪن->tl_scrollback_scrolled -= todo;

	FOR_ALL_WINDOWS(wp)
	{
	    إذا (wp->w_بفر == رڪن->tl_بفر)
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
handle_pushline(صحيح کانن_ڳڻپ, ثابت VTermScreenCell *cells, صحيح continuation, فراغ *user)
{
    رڪن_T	*رڪن = (رڪن_T *)user;
    garray_T	*gap;
    صحيح		update_بفر;

    إذا (رڪن->tl_عام_گهڻائي)
    {
	// In Terminal-Normal mode the user interacts with the buffer, thus we
	// must not change it. Postpone adding the scrollback lines.
	gap = &رڪن->tl_scrollback_postponed;
	update_بفر = FALSE;
    }
    وإلا
    {
	// First remove the lines that were appended before, the pushed line
	// goes above it.
	cleanup_scrollback(رڪن);
	gap = &رڪن->tl_scrollback;
	update_بفر = TRUE;
    }

    حد_بندي_scrollback(رڪن, gap, update_بفر);

    إذا (ga_grow(gap, 1) == FAIL)
	أرجع 0;

    cellattr_T	*p = عدم;
    صحيح		len = 0;
    صحيح		i;
    صحيح		c;
    صحيح		کانو;
    صحيح		متن_len;
    char_u		*متن;
    sb_سٽ_T	*سٽ;
    garray_T	ga;
    cellattr_T	fill_attr = رڪن->tl_default_color;

    // do not store empty cells at the end
    لكل (i = 0; i < کانن_ڳڻپ; ++i)
	إذا (cells[i].chars[0] != 0)
	    len = i + 1;
	وإلا
	    cell2cellattr(&cells[i], &fill_attr);

    ga_init2(&ga, 1, 100);
    إذا (len > 0)
	p = ALLOC_MULT(cellattr_T, len);
    إذا (p != عدم)
    {
	لكل (کانو = 0; کانو < len; کانو += cells[کانو].ويڪر)
	{
	    إذا (ga_grow(&ga, VTERM_MAX_CHARS_في_CELL * 4) == FAIL)
	    {
		ga.ga_len = 0;
		اكسر;
	    }
	    لكل (i = 0; i < VTERM_MAX_CHARS_في_CELL &&
		    ((c = cells[کانو].chars[i]) > 0 || i == 0); ++i)
		ga.ga_len += utf_char2bytes(c == NUL ? ' ' : c,
			(char_u *)ga.ga_معلومات + ga.ga_len);
	    cell2cellattr(&cells[کانو], &p[کانو]);
	}
    }
    إذا (ga_grow(&ga, 1) == FAIL)
    {
	إذا (update_بفر)
	    متن = (char_u *)"";
	وإلا
	    متن = vim_strsave((char_u *)"");
	متن_len = 0;
    }
    وإلا
    {
	متن = ga.ga_معلومات;
	متن_len = ga.ga_len;
	*(متن + متن_len) = NUL;
    }
    إذا (update_بفر)
	جوڙيو_scrollback_سٽ_منزل_بفر(رڪن, متن, متن_len, continuation);

    سٽ = (sb_سٽ_T *)gap->ga_معلومات + gap->ga_len;
    سٽ->sb_کانن_ڳڻپ = len;
    سٽ->sb_بائيٽون = متن_len;
    سٽ->sb_cells = p;
    سٽ->sb_fill_attr = fill_attr;
    سٽ->continuation = (char_u)continuation;
    إذا (update_بفر)
    {
	سٽ->sb_متن = عدم;
	++رڪن->tl_scrollback_scrolled;
	إذا (!continuation)
	    ++رڪن->tl_بفر_scrolled;
	ga_clear(&ga);  // free the text
    }
    وإلا
    {
	سٽ->sb_متن = متن;
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
handle_postponed_scrollback(رڪن_T *رڪن)
{
    صحيح i;

    إذا (رڪن->tl_scrollback_postponed.ga_len == 0)
	أرجع;
    ch_log(عدم, "Moving postponed scrollback to scrollback");

    // First remove the lines that were appended before, the pushed lines go
    // above it.
    cleanup_scrollback(رڪن);

    لكل (i = 0; i < رڪن->tl_scrollback_postponed.ga_len; ++i)
    {
	char_u		*متن;
	sb_سٽ_T	*pp_سٽ;
	sb_سٽ_T	*سٽ;

	إذا (ga_grow(&رڪن->tl_scrollback, 1) == FAIL)
	    اكسر;
	pp_سٽ = (sb_سٽ_T *)رڪن->tl_scrollback_postponed.ga_معلومات + i;

	متن = pp_سٽ->sb_متن;
	إذا (متن == عدم)
	    متن = (char_u *)"";
	جوڙيو_scrollback_سٽ_منزل_بفر(رڪن, متن, (صحيح)STRLEN(متن), pp_سٽ->continuation);
	vim_free(pp_سٽ->sb_متن);

	سٽ = (sb_سٽ_T *)رڪن->tl_scrollback.ga_معلومات
						 + رڪن->tl_scrollback.ga_len;
	سٽ->sb_کانن_ڳڻپ = pp_سٽ->sb_کانن_ڳڻپ;
	سٽ->sb_بائيٽون = pp_سٽ->sb_بائيٽون;
	سٽ->sb_cells = pp_سٽ->sb_cells;
	سٽ->sb_fill_attr = pp_سٽ->sb_fill_attr;
	سٽ->sb_متن = عدم;
	++رڪن->tl_scrollback_scrolled;
	++رڪن->tl_scrollback.ga_len;
	إذا (!pp_سٽ->continuation)
	    ++رڪن->tl_بفر_scrolled;
    }

    ga_clear(&رڪن->tl_scrollback_postponed);
    حد_بندي_scrollback(رڪن, &رڪن->tl_scrollback, TRUE);
}

/*
 * Called when the terminal wants to ring the system bell.
 */
    ساكن صحيح
handle_bell(فراغ *user اڻ_استعمال_ٿيل)
{
    vim_beep(BO_رڪن);
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
رڪن_after_channel_closed(رڪن_T *رڪن)
{
    // Unless in Terminal-Normal mode: clear the vterm.
    إذا (!رڪن->tl_عام_گهڻائي)
    {
	صحيح	fnum = رڪن->tl_بفر->b_fnum;

	cleanup_vterm(رڪن);

	إذا (رڪن->tl_پڄاڻي == TL_پڄاڻي_CLOSE)
	{
	    aco_save_T	aco;
	    صحيح		do_سيٽ_w_locked = رڪن->tl_بفر->b_nwindows == 0;
#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
	    win_T	*pwin = عدم;

	    // If this was a terminal in a popup window, go back to the
	    // previous window.
	    إذا (popup_is_popup(curwin) && curbuf == رڪن->tl_بفر)
	    {
		pwin = curwin;
		إذا (win_valid(prevwin))
		    win_enter(prevwin, FALSE);
	    }
	    وإلا
#جيڪڏهن_پورو
	    // If this is the last normal window: exit Vim.
	    إذا (رڪن->tl_بفر->b_nwindows > 0 && only_one_دري())
	    {
		exarg_T ea;

		CLEAR_ميدان(ea);
		ex_quit(&ea);
		أرجع TRUE;
	    }

	    // ++close or term_finish == "close"
	    ch_log(عدم, "terminal job finished, closing window");
	    aucmd_prepbuf(&aco, رڪن->tl_بفر);
	    إذا (curbuf == رڪن->tl_بفر)
	    {
		win_T	*wp = curwin;

		// Avoid closing the window if we temporarily use it.
		إذا (is_aucmd_win(wp))
		    do_سيٽ_w_locked = TRUE;
		إذا (do_سيٽ_w_locked)
		    ++wp->w_locked;
		do_bufdel(DOBUF_WIPE, (char_u *)"", 1, fnum, fnum, FALSE);
		إذا (do_سيٽ_w_locked)
		    --wp->w_locked;
		aucmd_restbuf(&aco);
	    }
#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
	    إذا (pwin != عدم)
		popup_close_with_retval(pwin, 0);
#جيڪڏهن_پورو
	    أرجع TRUE;
	}
	إذا (رڪن->tl_پڄاڻي == TL_پڄاڻي_OPEN
				   && رڪن->tl_بفر->b_nwindows == 0)
	{
	    حرف    *cmd = رڪن->tl_opencmd == عدم
				? "botright sbuf %d"
				: (حرف *)رڪن->tl_opencmd;
	    size_t  len = لڙي_ڊيگهه(cmd) + 50;
	    حرف    *buf = alloc(len);

	    إذا (buf != عدم)
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

    redraw_buf_and_حالت_later(رڪن->tl_بفر, UPD_NOT_VALID);
    أرجع FALSE;
}

#إذا defined(FEAT_PROP_POPUP)
/*
 * If the current window is a terminal in a popup window and the job has
 * finished, close the popup window and to back to the previous window.
 * Otherwise return FAIL.
 */
    صحيح
may_close_رڪن_popup(فراغ)
{
    إذا (!popup_is_popup(curwin) || curbuf->b_رڪن == عدم
				 || رڪن_ڪم_running_not_none(curbuf->b_رڪن))
	أرجع FAIL;

    win_T *pwin = curwin;

    إذا (win_valid(prevwin))
	win_enter(prevwin, FALSE);
    popup_close_with_retval(pwin, 0);
    أرجع OK;
}
#جيڪڏهن_پورو

/*
 * Called when a channel is going to be closed, before invoking the close
 * callback.
 */
    فراغ
رڪن_channel_closing(channel_T *ch)
{
    رڪن_T *رڪن;

    لكل (رڪن = پهريون_رڪن; رڪن != عدم; رڪن = رڪن->tl_ايندڙ)
	إذا (رڪن->tl_ڪم == ch->ch_ڪم && !رڪن->tl_channel_closed)
	    رڪن->tl_channel_closing = TRUE;
}

/*
 * Called when a channel has been closed.
 * If this was a channel for a terminal window then finish it up.
 */
    فراغ
رڪن_channel_closed(channel_T *ch)
{
    رڪن_T *رڪن;
    رڪن_T *ايندڙ_رڪن;
    صحيح	    did_one = FALSE;

    لكل (رڪن = پهريون_رڪن; رڪن != عدم; رڪن = ايندڙ_رڪن)
    {
	ايندڙ_رڪن = رڪن->tl_ايندڙ;
	إذا (رڪن->tl_ڪم == ch->ch_ڪم && !رڪن->tl_channel_closed)
	{
	    رڪن->tl_channel_closed = TRUE;
	    did_one = TRUE;

	    VIM_CLEAR(رڪن->tl_title);
	    VIM_CLEAR(رڪن->tl_حالت_متن);
#جيڪڏهن_وصف_ٿيل MSWIN
	    إذا (رڪن->tl_ٻاهريون_fd != عدم)
	    {
		fclose(رڪن->tl_ٻاهريون_fd);
		رڪن->tl_ٻاهريون_fd = عدم;
	    }
#جيڪڏهن_پورو

	    إذا (updating_screen)
	    {
		// Cannot open or close windows now.  Can happen when
		// 'lazyredraw' is set.
		رڪن->tl_channel_recently_closed = TRUE;
		استمر;
	    }

	    إذا (رڪن_after_channel_closed(رڪن))
		ايندڙ_رڪن = پهريون_رڪن;
	}
    }

    إذا (did_one)
    {
	redraw_statuslines();

	// Need to break out of vgetc().
	ins_char_typebuf(K_IGNORE, 0);
	typebuf_was_filled = TRUE;

	رڪن = curbuf->b_رڪن;
	إذا (رڪن != عدم)
	{
	    إذا (رڪن->tl_ڪم == ch->ch_ڪم)
		maketitle();
	    update_cursor(رڪن, رڪن->tl_cursor_visible);
	}
    }
}

/*
 * To be called after resetting updating_screen: handle any terminal where the
 * channel was closed.
 */
    فراغ
رڪن_check_channel_closed_recently(فراغ)
{
    رڪن_T *رڪن;
    رڪن_T *ايندڙ_رڪن;

    لكل (رڪن = پهريون_رڪن; رڪن != عدم; رڪن = ايندڙ_رڪن)
    {
	ايندڙ_رڪن = رڪن->tl_ايندڙ;
	إذا (رڪن->tl_channel_recently_closed)
	{
	    رڪن->tl_channel_recently_closed = FALSE;
	    إذا (رڪن_after_channel_closed(رڪن))
		// start over, the list may have changed
		ايندڙ_رڪن = پهريون_رڪن;
	}
    }
}

/*
 * Fill one screen line from a line of the terminal.
 * Advances "pos" to past the last column.
 */
    ساكن فراغ
رڪن_line2screenline(
	رڪن_T		*رڪن,
	win_T		*wp,
	VTermScreen	*screen,
	VTermPos	*pos,
	صحيح		max_کانو)
{
    صحيح off = screen_get_current_سٽ_off();

    لكل (pos->کانو = 0; pos->کانو < max_کانو; )
    {
	VTermScreenCell cell;
	صحيح		c;

	إذا (vterm_screen_get_cell(screen, *pos, &cell) == 0)
	    CLEAR_ميدان(cell);

	c = cell.chars[0];
	إذا (c == NUL)
	{
	    Screen_سٽون[off] = ' ';
	    إذا (enc_utf8)
		Screen_سٽون_UC[off] = NUL;
	}
	وإلا
	{
	    إذا (enc_utf8)
	    {
		صحيح i;

		// composing chars
		لكل (i = 0; i < Screen_mco
			      && i + 1 < VTERM_MAX_CHARS_في_CELL; ++i)
		{
		    Screen_سٽون_C[i][off] = cell.chars[i + 1];
		    إذا (cell.chars[i + 1] == 0)
			اكسر;
		}
		إذا (c >= 0x80 || (Screen_mco > 0
					 && Screen_سٽون_C[0][off] != 0))
		{
		    Screen_سٽون[off] = ' ';
		    Screen_سٽون_UC[off] = c;
		}
		وإلا
		{
		    Screen_سٽون[off] = c;
		    Screen_سٽون_UC[off] = NUL;
		}
	    }
#جيڪڏهن_وصف_ٿيل MSWIN
	    وإلا إذا (has_mbyte && c >= 0x80)
	    {
		char_u	mb[MB_MAXBYTES+1];
		WCHAR	wc = c;

		إذا (Wide_Char_منزل_Multi_Byte(GetACP(), 0, &wc, 1,
					       (حرف*)mb, 2, 0, 0) > 1)
		{
		    Screen_سٽون[off] = mb[0];
		    Screen_سٽون[off + 1] = mb[1];
		    cell.ويڪر = mb_ptr2cells(mb);
		}
		وإلا
		    Screen_سٽون[off] = c;
	    }
#جيڪڏهن_پورو
	    وإلا
		// This will only store the lower byte of "c".
		Screen_سٽون[off] = c;
	}
	ScreenAttrs[off] = cell2attr(رڪن, wp, &cell.attrs, &cell.fg,
								     &cell.bg);

	++pos->کانو;
	++off;
	إذا (cell.ويڪر == 2)
	{
	    // don't set the second byte to NUL for a DBCS encoding, it
	    // has been set above
	    إذا (enc_utf8)
	    {
		Screen_سٽون_UC[off] = NUL;
		Screen_سٽون[off] = NUL;
	    }
	    وإلا إذا (!has_mbyte)
	    {
		// Can't show a double-width character with a single-byte
		// 'encoding', just use a space.
		Screen_سٽون[off] = ' ';
		ScreenAttrs[off] = ScreenAttrs[off - 1];
	    }

	    ++pos->کانو;
	    ++off;
	}
    }
}

#إذا defined(FEAT_GUI)
    ساكن فراغ
update_system_رڪن(رڪن_T *رڪن)
{
    VTermPos	    pos;
    VTermScreen	    *screen;

    إذا (رڪن->tl_vterm == عدم)
	أرجع;
    screen = vterm_obtain_screen(رڪن->tl_vterm);

    // Scroll up to make more room for terminal lines if needed.
    // Use the cursor position to determine how much to scroll, because
    // ConPTY may damage all rows on initialization even when most are
    // empty, which would cause unnecessary scrolling.
    طالما (رڪن->tl_toprow > 0
		  && (صفون - رڪن->tl_toprow) < رڪن->tl_cursor_pos.صف + 1)
    {
	صحيح save_p_وڌيڪ = p_وڌيڪ;

	p_وڌيڪ = FALSE;
	نياپو_صف = صفون - 1;
	نياپو_puts("\n");
	p_وڌيڪ = save_p_وڌيڪ;
	--رڪن->tl_toprow;
    }

    لكل (pos.صف = رڪن->tl_dirty_صف_شروعات; pos.صف < رڪن->tl_dirty_صف_پڇاڙي
						  && pos.صف < صفون; ++pos.صف)
    {
	إذا (pos.صف < رڪن->tl_صفون)
	{
	    صحيح max_کانو = MIN(topframe->fr_ويڪر, رڪن->tl_کانن_ڳڻپ);

	    رڪن_line2screenline(رڪن, عدم, screen, &pos, max_کانو);
	}
	وإلا
	    pos.کانو = 0;

	screen_سٽ(curwin, رڪن->tl_toprow + pos.صف,
		firstwin->w_wincol, pos.کانو, topframe->fr_ويڪر, -1, 0);
    }

    رڪن->tl_dirty_صف_شروعات = MAX_صف;
    رڪن->tl_dirty_صف_پڇاڙي = 0;
}
#جيڪڏهن_پورو

/*
 * Return TRUE if window "wp" is to be redrawn with term_update_window().
 * Returns FALSE when there is no terminal running in this window or it is in
 * Terminal-Normal mode.
 */
    صحيح
رڪن_do_update_دري(win_T *wp)
{
    رڪن_T	*رڪن = wp->w_بفر->b_رڪن;

    أرجع رڪن != عدم && رڪن->tl_vterm != عدم && !رڪن->tl_عام_گهڻائي;
}

/*
 * Called to update a window that contains an active terminal.
 */
    فراغ
رڪن_update_دري(win_T *wp)
{
    رڪن_T	*رڪن = wp->w_بفر->b_رڪن;
    VTerm	*vterm;
    VTermScreen *screen;
    VTermState	*state;
    VTermPos	pos;
    صحيح		صفون_2, کانن_ڳڻپ;
    صحيح		newrows, newcols;
    صحيح		minsize;
    win_T	*twp;

    vterm = رڪن->tl_vterm;
    screen = vterm_obtain_screen(vterm);
    state = vterm_obtain_state(vterm);

    // We use UPD_NOT_VALID on a resize or scroll, redraw everything then.
    // With UPD_SOME_VALID only redraw what was marked dirty.
    إذا (wp->w_redr_type > UPD_SOME_VALID)
    {
	رڪن->tl_dirty_صف_شروعات = 0;
	رڪن->tl_dirty_صف_پڇاڙي = MAX_صف;

	إذا (رڪن->tl_postponed_scroll > 0
			      && رڪن->tl_postponed_scroll < رڪن->tl_صفون / 3)
	    // Scrolling is usually faster than redrawing, when there are only
	    // a few lines to scroll.
	    رڪن_scroll_مٿي(رڪن, 0, رڪن->tl_postponed_scroll);
	رڪن->tl_postponed_scroll = 0;
    }

    /*
     * If the window was resized a redraw will be triggered and we get here.
     * Adjust the size of the vterm unless 'termwinsize' specifies a fixed size.
     */
    minsize = parse_termwinsize(wp, &صفون_2, &کانن_ڳڻپ);

    newrows = 99999;
    newcols = 99999;
    لكل (twp = firstwin; ; twp = twp->w_ايندڙ)
    {
	// Always use curwin, it may be a popup window.
	win_T *wwp = twp == عدم ? curwin : twp;

	// When more than one window shows the same terminal, use the
	// smallest size.
	إذا (wwp->w_بفر == رڪن->tl_بفر)
	{
	    newrows = MIN(newrows, wwp->w_اوچائي);
	    newcols = MIN(newcols, wwp->w_ويڪر);
	}
	إذا (twp == عدم)
	    اكسر;
    }
    إذا (newrows == 99999 || newcols == 99999)
	أرجع; // safety exit
    newrows = صفون_2 == 0 ? newrows : minsize ? MAX(صفون_2, newrows) : صفون_2;
    newcols = کانن_ڳڻپ == 0 ? newcols : minsize ? MAX(کانن_ڳڻپ, newcols) : کانن_ڳڻپ;

    // If no cell is visible there is no point in resizing.  Also, vterm can't
    // handle a zero height.
    إذا (newrows == 0 || newcols == 0)
	أرجع;

    إذا (رڪن->tl_صفون != newrows || رڪن->tl_کانن_ڳڻپ != newcols)
    {
	رڪن->tl_vterm_سائيز_بدليو = TRUE;
	vterm_سيٽ_سائيز(vterm, newrows, newcols);
	ch_log(رڪن->tl_ڪم->jv_channel, "Resizing terminal to %d lines",
								      newrows);
	رڪن_report_winsize(رڪن, newrows, newcols);

	// Updating the terminal size will cause the snapshot to be cleared.
	// When not in terminal_loop() we need to restore it.
	إذا (رڪن != اندريون_terminal_loop)
	    may_move_terminal_منزل_بفر(رڪن, FALSE);
    }

    // The cursor may have been moved when resizing.
    vterm_state_get_cursorpos(state, &pos);
    position_cursor(wp, &pos);

    لكل (pos.صف = رڪن->tl_dirty_صف_شروعات; pos.صف < رڪن->tl_dirty_صف_پڇاڙي
					  && pos.صف < wp->w_اوچائي; ++pos.صف)
    {
	إذا (pos.صف < رڪن->tl_صفون)
	{
	    صحيح max_کانو = MIN(wp->w_ويڪر, رڪن->tl_کانن_ڳڻپ);

	    رڪن_line2screenline(رڪن, wp, screen, &pos, max_کانو);
	}
	وإلا
	    pos.کانو = 0;

	screen_سٽ(wp, wp->w_winrow + pos.صف
#جيڪڏهن_وصف_ٿيل FEAT_MENU
				+ winbar_اوچائي(wp)
#جيڪڏهن_پورو
				, wp->w_wincol, pos.کانو, wp->w_ويڪر, -1,
#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
				popup_is_popup(wp) ? SLF_POPUP :
#جيڪڏهن_پورو
				0);
    }
}

/*
 * Called after updating all windows: may reset dirty rows.
 */
    فراغ
رڪن_did_update_دري(win_T *wp)
{
    رڪن_T	*رڪن = wp->w_بفر->b_رڪن;

    إذا (رڪن == عدم || رڪن->tl_vterm == عدم || رڪن->tl_عام_گهڻائي
						|| wp->w_redr_type != 0)
	أرجع;

    رڪن->tl_dirty_صف_شروعات = MAX_صف;
    رڪن->tl_dirty_صف_پڇاڙي = 0;
}

/*
 * Return TRUE if "wp" is a terminal window where the job has finished.
 */
    صحيح
رڪن_is_ختم(buf_T *buf)
{
    أرجع buf->b_رڪن != عدم && buf->b_رڪن->tl_vterm == عدم;
}

/*
 * Return TRUE if "wp" is a terminal window where the job has finished or we
 * are in Terminal-Normal mode, thus we show the buffer contents.
 */
    صحيح
رڪن_show_بفر(buf_T *buf)
{
    رڪن_T *رڪن = buf->b_رڪن;

    أرجع رڪن != عدم && (رڪن->tl_vterm == عدم || رڪن->tl_عام_گهڻائي);
}

/*
 * The current buffer is going to be changed.  If there is terminal
 * highlighting remove it now.
 */
    فراغ
رڪن_change_اندريون_curbuf(فراغ)
{
    رڪن_T *رڪن = curbuf->b_رڪن;

    إذا (!رڪن_is_ختم(curbuf) || رڪن->tl_scrollback.ga_len <= 0)
	أرجع;

    free_scrollback(رڪن);
    redraw_buf_later(رڪن->tl_بفر, UPD_NOT_VALID);

    // The buffer is now like a normal buffer, it cannot be easily
    // abandoned when changed.
    سيٽ_string_option_سڌو((char_u *)"buftype", -1,
	    (char_u *)"", OPT_FREE|OPT_LOCAL, 0);
}

/*
 * Get the screen attribute for a position in the buffer.
 * Use a negative "col" to get the filler background color.
 */
    صحيح
رڪن_get_attr(win_T *wp, linenr_T lnum, صحيح کانو)
{
    buf_T	*buf = wp->w_بفر;
    رڪن_T	*رڪن = buf->b_رڪن;
    sb_سٽ_T	*سٽ;
    cellattr_T	*cellattr;
    صحيح         sb_سٽ = -1;
    صحيح         sb_کانو = کانو;

    إذا (رڪن->tl_scrollback.ga_len)
	bufline_pos_اندريون_scrollback(رڪن, lnum, کانو, &sb_سٽ, &sb_کانو);

    إذا (sb_سٽ < 0)
	cellattr = &رڪن->tl_default_color;
    وإلا
    {
	سٽ = (sb_سٽ_T *)رڪن->tl_scrollback.ga_معلومات + sb_سٽ;
	إذا (sb_کانو < 0 || sb_کانو >= سٽ->sb_کانن_ڳڻپ)
	    cellattr = &سٽ->sb_fill_attr;
	وإلا
	    cellattr = سٽ->sb_cells + sb_کانو;
    }
    أرجع cell2attr(رڪن, wp, &cellattr->attrs, &cellattr->fg, &cellattr->bg);
}

/*
 * Convert a cterm color number 0 - 255 to RGB.
 * This is compatible with xterm.
 */
    ساكن فراغ
cterm_color2vterm(صحيح nr, VTermColor *rgb)
{
    cterm_color2rgb(nr, &rgb->red, &rgb->green, &rgb->blue, &rgb->اشاريو);
    إذا (rgb->اشاريو == 0)
	rgb->type = VTERM_COLOR_RGB;
    وإلا
    {
	rgb->type = VTERM_COLOR_اشاريي_وارو;
	--rgb->اشاريو;
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
#إذا defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    // Use the actual color for the GUI and when 'termguicolors' is set.
    إذا (0
# ifdef FEAT_GUI
	    || gui.اندريون_use
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

	إذا (id > 0)
	    syn_id2colors(id, &fg_rgb, &bg_rgb);

	إذا (fg_rgb != INVALCOLOR)
	{
	    long_u rgb = GUI_MCH_GET_RGB(fg_rgb);
	    fg->red = (بنانشان)(rgb >> 16);
	    fg->green = (بنانشان)(rgb >> 8) & 255;
	    fg->blue = (بنانشان)rgb & 255;
	    fg->type = VTERM_COLOR_RGB | VTERM_COLOR_DEFAULT_FG;
	}
	وإلا
	    fg->type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_FG;

	إذا (bg_rgb != INVALCOLOR)
	{
	    long_u rgb = GUI_MCH_GET_RGB(bg_rgb);
	    bg->red = (بنانشان)(rgb >> 16);
	    bg->green = (بنانشان)(rgb >> 8) & 255;
	    bg->blue = (بنانشان)rgb & 255;
	    bg->type = VTERM_COLOR_RGB | VTERM_COLOR_DEFAULT_BG;
	}
	وإلا
	    bg->type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_BG;

	أرجع TRUE;
    }
    وإلا
#جيڪڏهن_پورو
    إذا (t_colors >= 16)
    {
	صحيح cterm_fg = -1;
	صحيح cterm_bg = -1;

	إذا (id > 0)
	    syn_id2cterm_bg(id, &cterm_fg, &cterm_bg);

	إذا (cterm_fg >= 0)
	{
	    cterm_color2vterm(cterm_fg, fg);
	    fg->type |= VTERM_COLOR_DEFAULT_FG;
	}
	وإلا
	    fg->type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_FG;

	إذا (cterm_bg >= 0)
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
رڪن_reset_hlfwin(win_T *wp)
{
    wp->w_رڪن_hlfwin.fg.type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_FG;
    wp->w_رڪن_hlfwin.bg.type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_BG;
}

/*
 * Cache the color of HLF_WIN.
 */
    فراغ
رڪن_update_hlfwin(win_T *wp)
{
    صحيح id = wp->w_hlfwin_id;

    إذا (id == 0 || !get_vterm_color_from_synid(id == -1 ? 0 : id,
		&wp->w_رڪن_hlfwin.fg, &wp->w_رڪن_hlfwin.bg))
	رڪن_reset_hlfwin(wp);
}

/*
 * Called when option 'termguicolors' was set,
 * or when any highlight is changed.
 */
    فراغ
رڪن_update_hlfwin_all(فراغ)
{
    win_T	 *wp = عدم;
    صحيح		 did_curwin = FALSE;

    طالما (for_all_windows_and_curwin(&wp, &did_curwin))
	رڪن_update_hlfwin(wp);
}

/*
 * Initialize term->tl_default_color from the environment.
 */
    فراغ
رڪن_init_default_colors(رڪن_T *رڪن)
{
    VTermColor	    *fg, *bg;
    صحيح		    fgval, bgval;
    صحيح		    id;

    CLEAR_ميدان(رڪن->tl_default_color.attrs);
    رڪن->tl_default_color.ويڪر = 1;
    fg = &رڪن->tl_default_color.fg;
    bg = &رڪن->tl_default_color.bg;

    // Vterm uses a default black background.  Set it to white when
    // 'background' is "light".
    إذا (*p_bg == 'l')
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
    id = رڪن_get_highlight_id(رڪن, عدم);

    إذا (!get_vterm_color_from_synid(id, fg, bg))
    {
#إذا defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
	صحيح tmp;
#جيڪڏهن_پورو

	// In an MS-Windows console we know the normal colors.
	إذا (cterm_عام_fg_color > 0)
	{
	    cterm_color2vterm(cterm_عام_fg_color - 1, fg);
#إذا defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
# ifdef VIMDLL
	    إذا (!gui.اندريون_use)
# endif
	    {
		tmp = fg->red;
		fg->red = fg->blue;
		fg->blue = tmp;
	    }
#جيڪڏهن_پورو
	}
#جيڪڏهن_وصف_ٿيل FEAT_TERMRESPONSE
	وإلا
	    رڪن_get_fg_color(&fg->red, &fg->green, &fg->blue);
#جيڪڏهن_پورو

	إذا (cterm_عام_bg_color > 0)
	{
	    cterm_color2vterm(cterm_عام_bg_color - 1, bg);
#إذا defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
# ifdef VIMDLL
	    إذا (!gui.اندريون_use)
# endif
	    {
		tmp = fg->red;
		fg->red = fg->blue;
		fg->blue = tmp;
	    }
#جيڪڏهن_پورو
	}
#جيڪڏهن_وصف_ٿيل FEAT_TERMRESPONSE
	وإلا
	    رڪن_get_bg_color(&bg->red, &bg->green, &bg->blue);
#جيڪڏهن_پورو
    }
}

#إذا defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Return TRUE if the user-defined palette (either g:terminal_ansi_colors or the
 * "ansi_colors" argument in term_start()) shall be applied.
 */
    ساكن صحيح
رڪن_use_palette(فراغ)
{
    إذا (0
# ifdef FEAT_GUI
	    || gui.اندريون_use
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
سيٽ_vterm_palette(VTerm *vterm, long_u *rgb)
{
    صحيح		اشاريو = 0;
    VTermState	*state = vterm_obtain_state(vterm);

    لكل (; اشاريو < 16; اشاريو++)
    {
	VTermColor	color;

	color.type = VTERM_COLOR_RGB;
	color.red = (بنانشان)(rgb[اشاريو] >> 16);
	color.green = (بنانشان)(rgb[اشاريو] >> 8) & 255;
	color.blue = (بنانشان)rgb[اشاريو] & 255;
	color.اشاريو = 0;
	vterm_state_سيٽ_palette_color(state, اشاريو, &color);
    }
}

/*
 * Set the ANSI color palette from a list of colors
 */
    ساكن صحيح
سيٽ_ansi_colors_list(VTerm *vterm, list_T *list)
{
    صحيح		n = 0;
    long_u	rgb[16];
    listitem_T	*li;

    لكل (li = list->lv_پهريون; li != عدم && n < 16; li = li->li_ايندڙ, n++)
    {
	char_u		*color_name;
	guicolor_T	guicolor;

	color_name = tv_get_string_chk(&li->li_tv);
	إذا (color_name == عدم)
	    أرجع FAIL;

	guicolor = GUI_GET_COLOR(color_name);
	إذا (guicolor == INVALCOLOR)
	    أرجع FAIL;

	rgb[n] = GUI_MCH_GET_RGB(guicolor);
    }

    إذا (n != 16 || li != عدم)
	أرجع FAIL;

    سيٽ_vterm_palette(vterm, rgb);

    أرجع OK;
}

/*
 * Initialize the ANSI color palette from g:terminal_ansi_colors[0:15]
 */
    ساكن فراغ
init_vterm_ansi_colors(VTerm *vterm)
{
    dictitem_T	*var = ڳوليو_var((char_u *)"g:terminal_ansi_colors", عدم, TRUE);

    إذا (var == عدم)
	أرجع;

    إذا (var->di_tv.v_type != VAR_LIST
	    || var->di_tv.vval.v_list == عدم
	    || var->di_tv.vval.v_list->lv_پهريون == &range_list_شي
	    || سيٽ_ansi_colors_list(vterm, var->di_tv.vval.v_list) == FAIL)
	semsg(_(e_invalid_دليل_str), "g:terminal_ansi_colors");
}
#جيڪڏهن_پورو

/*
 * Handles a "drop" command from the job in the terminal.
 * "item" is the file name, "item->li_next" may have options.
 */
    ساكن فراغ
handle_drop_command(listitem_T *شي)
{
    char_u	*fname = tv_get_string(&شي->li_tv);
    listitem_T	*opt_شي = شي->li_ايندڙ;
    صحيح		bufnr;
    win_T	*wp;
    tabpage_T   *tp;
    exarg_T	ea;
    char_u	*tofree = عدم;

    bufnr = buflist_جوڙيو(fname, BLN_LISTED | BLN_NOOPT);
    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	إذا (wp->w_بفر->b_fnum == bufnr)
	{
	    // buffer is in a window already, go there
	    goto_tabpage_win(tp, wp);
	    أرجع;
	}
    }

    CLEAR_ميدان(ea);

    إذا (opt_شي != عدم && opt_شي->li_tv.v_type == VAR_DICT
					&& opt_شي->li_tv.vval.v_dict != عدم)
    {
	dict_T *dict = opt_شي->li_tv.vval.v_dict;
	char_u *p;

	p = dict_get_string(dict, "ff", FALSE);
	إذا (p == عدم)
	    p = dict_get_string(dict, "fileformat", FALSE);
	إذا (p != عدم)
	{
	    إذا (check_ff_قدر(p) == FAIL)
		ch_log(عدم, "Invalid ff argument to drop: %s", p);
	    وإلا
		ea.force_ff = *p;
	}
	p = dict_get_string(dict, "enc", FALSE);
	إذا (p == عدم)
	    p = dict_get_string(dict, "encoding", FALSE);
	إذا (p != عدم)
	{
	    ea.cmd = alloc(STRLEN(p) + 12);
	    إذا (ea.cmd != عدم)
	    {
		sprintf((حرف *)ea.cmd, "sbuf ++enc=%s", p);
		ea.force_enc = 11;
		tofree = ea.cmd;
	    }
	}

	p = dict_get_string(dict, "bad", FALSE);
	إذا (p != عدم)
	    get_bad_opt(p, &ea);

	إذا (dict_has_ڪنجي(dict, "bin"))
	    ea.force_bin = FORCE_BIN;
	إذا (dict_has_ڪنجي(dict, "binary"))
	    ea.force_bin = FORCE_BIN;
	إذا (dict_has_ڪنجي(dict, "nobin"))
	    ea.force_bin = FORCE_NOBIN;
	إذا (dict_has_ڪنجي(dict, "nobinary"))
	    ea.force_bin = FORCE_NOBIN;
    }

    // open in new window, like ":split fname"
    إذا (ea.cmd == عدم)
	ea.cmd = (char_u *)"split";
    ea.arg = fname;
    ea.cmdidx = CMD_ورهايو;
    ex_splitview(&ea);

    vim_free(tofree);
}

/*
 * Return TRUE if "func" starts with "pat" and "pat" isn't empty.
 */
    ساكن صحيح
is_permitted_رڪن_api(char_u *func, char_u *pat)
{
    أرجع pat != عدم && *pat != NUL && STRNICMP(func, pat, STRLEN(pat)) == 0;
}

/*
 * Handles a function call from the job running in a terminal.
 * "item" is the function name, "item->li_next" has the arguments.
 */
    ساكن فراغ
handle_call_command(رڪن_T *رڪن, channel_T *channel, listitem_T *شي)
{
    char_u	*func;
    typval_T	argvars[2];
    typval_T	rettv;
    funcexe_T	funcexe;

    إذا (شي->li_ايندڙ == عدم)
    {
	ch_log(channel, "Missing function arguments for call");
	أرجع;
    }
    func = tv_get_string(&شي->li_tv);

    إذا (!is_permitted_رڪن_api(func, رڪن->tl_api))
    {
	ch_log(channel, "Unpermitted function: %s", func);
	أرجع;
    }

    argvars[0].v_type = VAR_عدد;
    argvars[0].vval.v_عدد = رڪن->tl_بفر->b_fnum;
    argvars[1] = شي->li_ايندڙ->li_tv;
    CLEAR_ميدان(funcexe);
    funcexe.fe_firstline = 1L;
    funcexe.fe_lastline = 1L;
    funcexe.fe_evaluate = TRUE;
    إذا (call_func(func, -1, &rettv, 2, argvars, &funcexe) == OK)
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
	إذا (src[i] == '%' && i + 2 < len)
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
    صحيح       وٿي = 7;  // len of "file://" is 7
    حرف      *pos = (حرف *)gap->ga_معلومات + وٿي;
    char_u    *new_dir;

    // remove HOSTNAME to get PWD
    طالما (وٿي < (صحيح)gap->ga_len && *pos != '/' )
    {
	++وٿي;
	++pos;
    }

    إذا (وٿي >= (صحيح)gap->ga_len)
    {
	semsg(_(e_failed_منزل_ڪڍو_pwd_from_str_check_your_shell_config),
								 gap->ga_معلومات);
	أرجع;
    }

    new_dir = alloc(gap->ga_len - وٿي + 1);
    url_decode(pos, gap->ga_len-وٿي, new_dir);
    changedir_func(new_dir, TRUE, CDSCOPE_دري);
    vim_free(new_dir);
}

/*
 * Called by libvterm when it cannot recognize an OSC sequence.
 * We recognize a terminal API command.
 */
    ساكن صحيح
parse_osc(صحيح command, VTermStringFragment frag, فراغ *user)
{
    رڪن_T	*رڪن = (رڪن_T *)user;
    js_read_T	reader;
    typval_T	tv;
    channel_T	*channel = رڪن->tl_ڪم == عدم ? عدم
						    : رڪن->tl_ڪم->jv_channel;
    garray_T	*gap = &رڪن->tl_osc_buf;

    // We recognize only OSC 5 1 ; {command} and OSC 7 ; {command}
    إذا (command != 51 && (command != 7 || !p_asd))
	أرجع 0;

    // Concatenate what was received until the final piece is found.
    إذا (ga_grow(gap, (صحيح)frag.len + 1) == FAIL)
    {
	ga_clear(gap);
	أرجع 1;
    }
    mch_memmove((حرف *)gap->ga_معلومات + gap->ga_len, frag.str, frag.len);
    gap->ga_len += (صحيح)frag.len;
    إذا (!frag.final)
	أرجع 1;

    ((حرف *)gap->ga_معلومات)[gap->ga_len] = 0;

    إذا (command == 7)
    {
	sync_shell_dir(gap);
	ga_clear(gap);
	أرجع 1;
    }

    reader.js_buf = gap->ga_معلومات;
    reader.js_fill = عدم;
    reader.js_استعمال_ٿيل = 0;
    إذا (json_decode(&reader, &tv, 0) == OK
	    && tv.v_type == VAR_LIST
	    && tv.vval.v_list != عدم)
    {
	listitem_T *شي = tv.vval.v_list->lv_پهريون;

	إذا (شي == عدم)
	    ch_log(channel, "Missing command");
	وإلا
	{
	    char_u	*cmd = tv_get_string(&شي->li_tv);

	    // Make sure an invoked command doesn't delete the buffer (and the
	    // terminal) under our fingers.
	    ++رڪن->tl_بفر->b_locked;

	    شي = شي->li_ايندڙ;
	    إذا (شي == عدم)
		ch_log(channel, "Missing argument for %s", cmd);
	    وإلا إذا (STRCMP(cmd, "drop") == 0)
		handle_drop_command(شي);
	    وإلا إذا (STRCMP(cmd, "call") == 0)
		handle_call_command(رڪن, channel, شي);
	    وإلا
		ch_log(channel, "Invalid command received: %s", cmd);
	    --رڪن->tl_بفر->b_locked;
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
	ثابت حرف  *leader اڻ_استعمال_ٿيل,
	ثابت ڊگهو  دليلون[],
	صحيح	    argcount,
	ثابت حرف  *intermed اڻ_استعمال_ٿيل,
	حرف	    command,
	فراغ	    *user)
{
    رڪن_T	*رڪن = (رڪن_T *)user;
    حرف	buf[100];
    صحيح		len;
    صحيح		x = 0;
    صحيح		y = 0;
    win_T	*wp;

    // We recognize only CSI 13 t
    إذا (command != 't' || argcount != 1 || دليلون[0] != 13)
	أرجع 0; // not handled

    // When getting the window position is not possible or it fails it results
    // in zero/zero.
#إذا defined(FEAT_GUI) \
	|| (defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)) \
	|| defined(MSWIN)
    (فراغ)ui_get_winpos(&x, &y, (varnumber_T)100);
#جيڪڏهن_پورو

    FOR_ALL_WINDOWS(wp)
	إذا (wp->w_بفر == رڪن->tl_بفر)
	    اكسر;
    إذا (wp != عدم)
    {
#جيڪڏهن_وصف_ٿيل FEAT_GUI
	إذا (gui.اندريون_use)
	{
	    x += wp->w_wincol * gui.char_ويڪر;
	    y += W_WINROW(wp) * gui.char_اوچائي;
	}
	وإلا
#جيڪڏهن_پورو
	{
	    // We roughly estimate the position of the terminal window inside
	    // the Vim window by assuming a 10 x 7 character cell.
	    x += wp->w_wincol * 7;
	    y += W_WINROW(wp) * 10;
	}
    }

    len = vim_snprintf(buf, 100, "\x1b[3;%d;%dt", x, y);
    channel_send(رڪن->tl_ڪم->jv_channel, get_tty_part(رڪن),
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
vterm_malloc(size_t سائيز, فراغ *معلومات اڻ_استعمال_ٿيل)
{
    // make sure that the length is not zero
    أرجع alloc_clear(سائيز == 0 ? 1L : سائيز);
}

    ساكن فراغ
vterm_memfree(فراغ *ptr, فراغ *معلومات اڻ_استعمال_ٿيل)
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
create_vterm(رڪن_T *رڪن, صحيح صفون_2, صحيح کانن_ڳڻپ)
{
    VTerm	    *vterm;
    VTermScreen	    *screen;
    VTermState	    *state;
    VTerm_قدر	    قدر;

    vterm = vterm_new_with_allocator(صفون_2, کانن_ڳڻپ, &vterm_allocator, عدم);
    رڪن->tl_vterm = vterm;
    إذا (vterm == عدم)
	أرجع FAIL;

    // Allocate screen and state here, so we can bail out if that fails.
    state = vterm_obtain_state(vterm);
    screen = vterm_obtain_screen(vterm);
    إذا (state == عدم || screen == عدم)
    {
	vterm_free(vterm);
	أرجع FAIL;
    }

    vterm_screen_سيٽ_callbacks(screen, &screen_callbacks, رڪن);
    vterm_screen_سيٽ_damage_ملايو(screen, VTERM_DAMAGE_SCROLL);
    vterm_screen_callbacks_has_pushline4(screen);
    // TODO: depends on 'encoding'.
    vterm_سيٽ_utf8(vterm, 1);

    رڪن_init_default_colors(رڪن);

    vterm_state_سيٽ_default_colors(
	    state,
	    &رڪن->tl_default_color.fg,
	    &رڪن->tl_default_color.bg);

    إذا (t_colors < 16)
	// Less than 16 colors: assume that bold means using a bright color for
	// the foreground color.
	vterm_state_سيٽ_bold_highbright(vterm_obtain_state(vterm), 1);

    // Required to initialize most things.
    vterm_screen_reset(screen, 1 /* hard */);

    // Allow using alternate screen.
    vterm_screen_enable_altscreen(screen, 1);

    // For unix do not use a blinking cursor.  In an xterm this causes the
    // cursor to blink if it's blinking in the xterm.
    // For Windows we respect the system wide setting.
#جيڪڏهن_وصف_ٿيل MSWIN
    إذا (Get_Caret_Blink_وقت() == INFINITE)
	قدر.boolean = 0;
    وإلا
	قدر.boolean = 1;
#وإلا
    قدر.boolean = 0;
#جيڪڏهن_پورو
    vterm_state_سيٽ_termprop(state, VTERM_PROP_CURSORBLINK, &قدر);
    vterm_state_سيٽ_unrecognised_fallbacks(state, &state_fallbacks, رڪن);

    أرجع OK;
}

/*
 * Reset the terminal palette to its default value.
 */
    ساكن فراغ
رڪن_reset_palette(VTerm *vterm)
{
    VTermState	*state = vterm_obtain_state(vterm);
    صحيح		اشاريو;

    لكل (اشاريو = 0; اشاريو < 16; اشاريو++)
    {
	VTermColor	color;

	color.type = VTERM_COLOR_اشاريي_وارو;
	ansi_color2rgb(اشاريو, &color.red, &color.green, &color.blue,
		&color.اشاريو);
	// The first valid index starts at 1.
	color.اشاريو -= 1;

	vterm_state_سيٽ_palette_color(state, اشاريو, &color);
    }
}

    ساكن فراغ
رڪن_update_palette(رڪن_T *رڪن)
{
#إذا defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    إذا (رڪن_use_palette()
	    && (رڪن->tl_palette != عدم
		|| ڳوليو_var((char_u *)"g:terminal_ansi_colors", عدم, TRUE)
								      != عدم))
    {
	إذا (رڪن->tl_palette != عدم)
	    سيٽ_vterm_palette(رڪن->tl_vterm, رڪن->tl_palette);
	وإلا
	    init_vterm_ansi_colors(رڪن->tl_vterm);
    }
    وإلا
#جيڪڏهن_پورو
	رڪن_reset_palette(رڪن->tl_vterm);
}

/*
 * Called when option 'termguicolors' is changed.
 */
    فراغ
رڪن_update_palette_all(فراغ)
{
    رڪن_T *رڪن;

    FOR_ALL_TERMS(رڪن)
    {
	إذا (رڪن->tl_vterm == عدم)
	    استمر;
	رڪن_update_palette(رڪن);
    }
}

/*
 * Called when option 'background' or 'termguicolors' was set,
 * or when any highlight is changed.
 */
    فراغ
رڪن_update_colors_all(فراغ)
{
    رڪن_T *رڪن;

    FOR_ALL_TERMS(رڪن)
    {
	إذا (رڪن->tl_vterm == عدم)
	    استمر;
	رڪن_init_default_colors(رڪن);
	vterm_state_سيٽ_default_colors(
		vterm_obtain_state(رڪن->tl_vterm),
		&رڪن->tl_default_color.fg,
		&رڪن->tl_default_color.bg);
    }
}

/*
 * Return the text to show for the buffer name and status.
 */
    char_u *
رڪن_get_حالت_متن(رڪن_T *رڪن)
{
    إذا (رڪن->tl_حالت_متن != عدم)
	أرجع رڪن->tl_حالت_متن;

    char_u *txt;
    size_t len;
    char_u *fname;

    إذا (رڪن->tl_عام_گهڻائي)
    {
	إذا (رڪن_ڪم_running(رڪن))
	    txt = (char_u *)_("Terminal");
	وإلا
	    txt = (char_u *)_("Terminal-finished");
    }
    وإلا إذا (رڪن->tl_title != عدم)
	txt = رڪن->tl_title;
    وإلا إذا (رڪن_none_open(رڪن))
	txt = (char_u *)_("active");
    وإلا إذا (رڪن_ڪم_running(رڪن))
	txt = (char_u *)_("running");
    وإلا
	txt = (char_u *)_("finished");
    fname = buf_get_fname(رڪن->tl_بفر);
    len = 9 + STRLEN(fname) + STRLEN(txt);
    رڪن->tl_حالت_متن = alloc(len);
    إذا (رڪن->tl_حالت_متن != عدم)
	vim_snprintf((حرف *)رڪن->tl_حالت_متن, len, "%s [%s]",
		fname, txt);
    أرجع رڪن->tl_حالت_متن;
}

/*
 * Clear the cached value of the status text.
 */
    فراغ
رڪن_clear_حالت_متن(رڪن_T *رڪن)
{
    VIM_CLEAR(رڪن->tl_حالت_متن);
}

/*
 * Mark references in jobs of terminals.
 */
    صحيح
سيٽ_ref_اندريون_رڪن(صحيح copyID)
{
    صحيح		abort = FALSE;
    رڪن_T	*رڪن;
    typval_T	tv;

    لكل (رڪن = پهريون_رڪن; !abort && رڪن != عدم; رڪن = رڪن->tl_ايندڙ)
	إذا (رڪن->tl_ڪم != عدم)
	{
	    tv.v_type = VAR_ڪم;
	    tv.vval.v_ڪم = رڪن->tl_ڪم;
	    abort = abort || سيٽ_ref_اندريون_شي(&tv, copyID, عدم, عدم, عدم);
	}
    أرجع abort;
}

/*
 * Get the buffer from the first argument in "argvars".
 * Returns NULL when the buffer is not for a terminal window and logs a message
 * with "where".
 */
    ساكن buf_T *
رڪن_get_buf(typval_T *argvars, حرف *where)
{
    buf_T *buf;

    ++emsg_off;
    buf = tv_get_buf(&argvars[0], FALSE);
    --emsg_off;
    إذا (buf == عدم || buf->b_رڪن == عدم)
    {
	(فراغ)tv_get_عدد(&argvars[0]);    // issue errmsg if type error
	ch_log(عدم, "%s: invalid buffer argument", where);
	أرجع عدم;
    }
    أرجع buf;
}

    ساكن فراغ
clear_cell(VTermScreenCell *cell)
{
    CLEAR_ميدان(*cell);
    cell->fg.type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_FG;
    cell->bg.type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_BG;
}

    ساكن فراغ
dump_رڪن_color(FILE *fd, VTermColor *color)
{
    صحيح اشاريو;

    إذا (VTERM_COLOR_IS_اشاريي_وارو(color))
	اشاريو = color->اشاريو + 1;
    وإلا إذا (color->type == 0)
	// use RGB values
	اشاريو = 255;
    وإلا
	// default color
	اشاريو = 0;
    fprintf(fd, "%02x%02x%02x%d",
	    (صحيح)color->red, (صحيح)color->green, (صحيح)color->blue,
	    اشاريو);
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
f_رڪن_dumpwrite(typval_T *argvars, typval_T *rettv اڻ_استعمال_ٿيل)
{
    buf_T	*buf;
    رڪن_T	*رڪن;
    char_u	*fname;
    صحيح		max_اوچائي_2 = 0;
    صحيح		max_ويڪر_2 = 0;
    stat_T	st;
    FILE	*fd;
    VTermPos	pos;
    VTermScreen *screen;
    VTermScreenCell prev_cell;
    VTermState	*state;
    VTermPos	cursor_pos;

    إذا (check_restricted() || check_secure())
	أرجع;

    إذا (اندريون_vim9script()
	    && (check_for_بفر_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	أرجع;

    buf = رڪن_get_buf(argvars, "term_dumpwrite()");
    إذا (buf == عدم)
	أرجع;
    رڪن = buf->b_رڪن;
    إذا (رڪن->tl_vterm == عدم)
    {
	emsg(_(e_ڪم_already_ختم));
	أرجع;
    }

    إذا (argvars[2].v_type != VAR_UNKNOWN)
    {
	dict_T *d;

	إذا (check_for_dict_arg(argvars, 2) == FAIL)
	    أرجع;
	d = argvars[2].vval.v_dict;
	إذا (d != عدم)
	{
	    max_اوچائي_2 = dict_get_عدد(d, "rows");
	    max_ويڪر_2 = dict_get_عدد(d, "columns");
	}
    }

    fname = tv_get_string_chk(&argvars[1]);
    إذا (fname == عدم)
	أرجع;
    إذا (mch_stat((حرف *)fname, &st) >= 0)
    {
	semsg(_(e_file_exists_str), fname);
	أرجع;
    }

    إذا (*fname == NUL || (fd = mch_fopen((حرف *)fname, WRITEBIN)) == عدم)
    {
	semsg(_(e_cant_create_file_str), *fname == NUL ? (char_u *)_("<empty>") : fname);
	أرجع;
    }

    clear_cell(&prev_cell);

    screen = vterm_obtain_screen(رڪن->tl_vterm);
    state = vterm_obtain_state(رڪن->tl_vterm);
    vterm_state_get_cursorpos(state, &cursor_pos);

    لكل (pos.صف = 0; (max_اوچائي_2 == 0 || pos.صف < max_اوچائي_2)
					 && pos.صف < رڪن->tl_صفون; ++pos.صف)
    {
	صحيح		repeat = 0;

	لكل (pos.کانو = 0; (max_ويڪر_2 == 0 || pos.کانو < max_ويڪر_2)
					 && pos.کانو < رڪن->tl_کانن_ڳڻپ; ++pos.کانو)
	{
	    VTermScreenCell cell;
	    صحيح		    same_attr;
	    صحيح		    same_chars = TRUE;
	    صحيح		    i;
	    صحيح		    is_cursor_pos = (pos.کانو == cursor_pos.کانو
						 && pos.صف == cursor_pos.صف);

	    إذا (vterm_screen_get_cell(screen, pos, &cell) == 0)
		clear_cell(&cell);

	    لكل (i = 0; i < VTERM_MAX_CHARS_في_CELL; ++i)
	    {
		صحيح c = cell.chars[i];
		صحيح pc = prev_cell.chars[i];
		صحيح should_break = c == NUL || pc == NUL;

		// For the first character NUL is the same as space.
		إذا (i == 0)
		{
		    c = (c == NUL) ? ' ' : c;
		    pc = (pc == NUL) ? ' ' : pc;
		}
		إذا (c != pc)
		    same_chars = FALSE;
		إذا (should_break)
		    اكسر;
	    }
	    same_attr = vtermAttr2hl(&cell.attrs)
					      == vtermAttr2hl(&prev_cell.attrs)
			&& vterm_color_is_equal(&cell.fg, &prev_cell.fg)
			&& vterm_color_is_equal(&cell.bg, &prev_cell.bg);
	    إذا (same_chars && cell.ويڪر == prev_cell.ويڪر && same_attr
							     && !is_cursor_pos)
	    {
		++repeat;
	    }
	    وإلا
	    {
		إذا (repeat > 0)
		{
		    fprintf(fd, "@%d", repeat);
		    repeat = 0;
		}
		fputs(is_cursor_pos ? ">" : "|", fd);

		إذا (cell.chars[0] == NUL)
		    fputs(" ", fd);
		وإلا
		{
		    char_u	charbuf[10];
		    صحيح		len;

		    لكل (i = 0; i < VTERM_MAX_CHARS_في_CELL
						  && cell.chars[i] != NUL; ++i)
		    {
			len = utf_char2bytes(cell.chars[i], charbuf);
			fwrite(charbuf, len, 1, fd);
		    }
		}

		// When only the characters differ we don't write anything, the
		// following "|", "@" or NL will indicate using the same
		// attributes.
		إذا (cell.ويڪر != prev_cell.ويڪر || !same_attr)
		{
		    إذا (cell.ويڪر == 2)
			fputs("*", fd);
		    وإلا
			fputs("+", fd);

		    إذا (same_attr)
		    {
			fputs("&", fd);
		    }
		    وإلا
		    {
			fprintf(fd, "%d", vtermAttr2hl(&cell.attrs));
			إذا (vterm_color_is_equal(&cell.fg, &prev_cell.fg))
			    fputs("&", fd);
			وإلا
			{
			    fputs("#", fd);
			    dump_رڪن_color(fd, &cell.fg);
			}
			إذا (vterm_color_is_equal(&cell.bg, &prev_cell.bg))
			    fputs("&", fd);
			وإلا
			{
			    fputs("#", fd);
			    dump_رڪن_color(fd, &cell.bg);
			}
		    }
		}

		prev_cell = cell;
	    }

	    إذا (cell.ويڪر == 2)
		++pos.کانو;
	}
	إذا (repeat > 0)
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
    إذا (ga_grow(gap, 1) == FAIL)
	أرجع;

    *(((cellattr_T *)gap->ga_معلومات) + gap->ga_len) = *cell;
    ++gap->ga_len;
}

    ساكن فراغ
clear_cellattr(cellattr_T *cell)
{
    CLEAR_ميدان(*cell);
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
    garray_T	    ga_متن;
    garray_T	    ga_cell;
    string_T	    prev_char = {عدم, 0};
    صحيح		    attr = 0;
    cellattr_T	    cell;
    cellattr_T	    خالي_جايون_cell;
    رڪن_T	    *رڪن = curbuf->b_رڪن;
    صحيح		    max_cells = 0;
    صحيح		    شروعات_صف = رڪن->tl_scrollback.ga_len;

    ga_init2(&ga_متن, 1, 90);
    ga_init2(&ga_cell, حجم(cellattr_T), 90);
    clear_cellattr(&cell);
    clear_cellattr(&خالي_جايون_cell);
    cursor_pos->صف = -1;
    cursor_pos->کانو = -1;

    c = fgetc(fd);
    لكل (;;)
    {
	إذا (c == EOF)
	    اكسر;
	إذا (c == '\r')
	{
	    // DOS line endings?  Ignore.
	    c = fgetc(fd);
	}
	وإلا إذا (c == '\n')
	{
	    // End of a line: append it to the buffer.
	    إذا (ga_متن.ga_معلومات == عدم)
		dump_is_corrupt(&ga_متن);
	    إذا (ga_grow(&رڪن->tl_scrollback, 1) == OK)
	    {
		sb_سٽ_T   *سٽ = (sb_سٽ_T *)رڪن->tl_scrollback.ga_معلومات
						  + رڪن->tl_scrollback.ga_len;

		إذا (max_cells < ga_cell.ga_len)
		    max_cells = ga_cell.ga_len;
		سٽ->sb_کانن_ڳڻپ = ga_cell.ga_len;
		سٽ->sb_بائيٽون = ga_متن.ga_len;
		سٽ->sb_cells = ga_cell.ga_معلومات;
		سٽ->sb_fill_attr = رڪن->tl_default_color;
		سٽ->continuation = 0;
		++رڪن->tl_scrollback.ga_len;
		ga_init(&ga_cell);

		ga_append(&ga_متن, NUL);
		ml_append(curbuf->b_ml.ml_سٽ_ڳڻپ, ga_متن.ga_معلومات,
							ga_متن.ga_len, FALSE);
	    }
	    وإلا
		ga_clear(&ga_cell);
	    ga_متن.ga_len = 0;

	    c = fgetc(fd);
	}
	وإلا إذا (c == '|' || c == '>')
	{
	    صحيح prev_len = ga_متن.ga_len;

	    إذا (c == '>')
	    {
		إذا (cursor_pos->صف != -1)
		    dump_is_corrupt(&ga_متن);	// duplicate cursor
		cursor_pos->صف = رڪن->tl_scrollback.ga_len - شروعات_صف;
		cursor_pos->کانو = ga_cell.ga_len;
	    }

	    // normal character(s) followed by "+", "*", "|", "@" or NL
	    c = fgetc(fd);
	    إذا (c != EOF)
		ga_append(&ga_متن, c);
	    لكل (;;)
	    {
		c = fgetc(fd);
		إذا (c == '+' || c == '*' || c == '|' || c == '>' || c == '@'
						      || c == EOF || c == '\n')
		    اكسر;
		ga_append(&ga_متن, c);
	    }

	    // save the character for repeating it
	    VIM_CLEAR_STRING(prev_char);
	    إذا (ga_متن.ga_معلومات != عدم)
	    {
		prev_char.ڊيگهائي = (size_t)(ga_متن.ga_len - prev_len);
		prev_char.string = vim_strnsave(
		    ((char_u *)ga_متن.ga_معلومات) + prev_len,
		    prev_char.ڊيگهائي);
		إذا (prev_char.string == عدم)
		    prev_char.ڊيگهائي = 0;
	    }

	    إذا (c == '@' || c == '|' || c == '>' || c == '\n')
	    {
		// use all attributes from previous cell
	    }
	    وإلا إذا (c == '+' || c == '*')
	    {
		صحيح is_bg;

		cell.ويڪر = c == '+' ? 1 : 2;

		c = fgetc(fd);
		إذا (c == '&')
		{
		    // use same attr as previous cell
		    c = fgetc(fd);
		}
		وإلا إذا (محفوظ_isdigit(c))
		{
		    // get the decimal attribute
		    attr = 0;
		    طالما (محفوظ_isdigit(c))
		    {
			attr = attr * 10 + (c - '0');
			c = fgetc(fd);
		    }
		    hl2vtermAttr(attr, &cell);

		    // is_bg == 0: fg, is_bg == 1: bg
		    لكل (is_bg = 0; is_bg <= 1; ++is_bg)
		    {
			إذا (c == '&')
			{
			    // use same color as previous cell
			    c = fgetc(fd);
			}
			وإلا إذا (c == '#')
			{
			    صحيح red, green, blue, اشاريو = 0, type;

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
			    إذا (!محفوظ_isdigit(c))
				dump_is_corrupt(&ga_متن);
			    طالما (محفوظ_isdigit(c))
			    {
				اشاريو = اشاريو * 10 + (c - '0');
				c = fgetc(fd);
			    }
			    إذا (اشاريو == 0 || اشاريو == 255)
			    {
				type = VTERM_COLOR_RGB;
				إذا (اشاريو == 0)
				{
				    إذا (is_bg)
					type |= VTERM_COLOR_DEFAULT_BG;
				    وإلا
					type |= VTERM_COLOR_DEFAULT_FG;
				}
			    }
			    وإلا
			    {
				type = VTERM_COLOR_اشاريي_وارو;
				اشاريو -= 1;
			    }
			    إذا (is_bg)
			    {
				cell.bg.type = type;
				cell.bg.red = red;
				cell.bg.green = green;
				cell.bg.blue = blue;
				cell.bg.اشاريو = اشاريو;
			    }
			    وإلا
			    {
				cell.fg.type = type;
				cell.fg.red = red;
				cell.fg.green = green;
				cell.fg.blue = blue;
				cell.fg.اشاريو = اشاريو;
			    }
			}
			وإلا
			    dump_is_corrupt(&ga_متن);
		    }
		}
		وإلا
		    dump_is_corrupt(&ga_متن);
	    }
	    وإلا
		dump_is_corrupt(&ga_متن);

	    append_cell(&ga_cell, &cell);
	    إذا (cell.ويڪر == 2)
		append_cell(&ga_cell, &خالي_جايون_cell);
	}
	وإلا إذا (c == '@')
	{
	    إذا (prev_char.string == عدم)
		dump_is_corrupt(&ga_متن);
	    وإلا
	    {
		صحيح ڳڻپ = 0;

		// repeat previous character, get the count
		لكل (;;)
		{
		    c = fgetc(fd);
		    إذا (!محفوظ_isdigit(c))
			اكسر;
		    ڳڻپ = ڳڻپ * 10 + (c - '0');
		}

		طالما (ڳڻپ-- > 0)
		{
		    ga_concat_len(&ga_متن, prev_char.string, prev_char.ڊيگهائي);
		    append_cell(&ga_cell, &cell);
		}
	    }
	}
	وإلا
	{
	    dump_is_corrupt(&ga_متن);
	    c = fgetc(fd);
	}
    }

    إذا (ga_متن.ga_len > 0)
    {
	// trailing characters after last NL
	dump_is_corrupt(&ga_متن);
	ga_append(&ga_متن, NUL);
	ml_append(curbuf->b_ml.ml_سٽ_ڳڻپ, ga_متن.ga_معلومات,
							ga_متن.ga_len, FALSE);
    }

    ga_clear(&ga_متن);
    ga_clear(&ga_cell);
    vim_free(prev_char.string);

    أرجع max_cells;
}

/*
 * Return an allocated string with at least "text_width" "=" characters and
 * "fname" inserted in the middle.
 */
    ساكن char_u *
get_separator(صحيح متن_ويڪر, char_u *fname)
{
    صحيح	    ويڪر = MAX(متن_ويڪر, curwin->w_ويڪر);
    char_u  *textline;
    صحيح	    fname_سائيز;
    char_u  *p = fname;
    صحيح	    i;
    size_t  off;

    textline = alloc(ويڪر + STRLEN(fname) + 1);
    إذا (textline == عدم)
	أرجع عدم;

    fname_سائيز = vim_strsize(fname);
    إذا (fname_سائيز < ويڪر - 8)
    {
	// enough room, don't use the full window width
	ويڪر = MAX(متن_ويڪر, fname_سائيز + 8);
    }
    وإلا إذا (fname_سائيز > ويڪر - 8)
    {
	// full name doesn't fit, use only the tail
	p = gettail(fname);
	fname_سائيز = vim_strsize(p);
    }
    // skip characters until the name fits
    طالما (fname_سائيز > ويڪر - 8)
    {
	p += (*mb_ptr2len)(p);
	fname_سائيز = vim_strsize(p);
    }

    لكل (i = 0; i < (ويڪر - fname_سائيز) / 2 - 1; ++i)
	textline[i] = '=';
    textline[i++] = ' ';

    STRCPY(textline + i, p);
    off = STRLEN(textline);
    textline[off] = ' ';
    لكل (i = 1; i < (ويڪر - fname_سائيز) / 2; ++i)
	textline[off + i] = '=';
    textline[off + i] = NUL;

    أرجع textline;
}

/*
 * Common for "term_dumpdiff()" and "term_dumpload()".
 */
    ساكن فراغ
رڪن_ڀريو_dump(typval_T *argvars, typval_T *rettv, صحيح do_diff)
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
    إذا (do_diff)
	fname2 = tv_get_string_buf_chk(&argvars[1], buf2);
    إذا (fname1 == عدم || (do_diff && fname2 == عدم))
    {
	emsg(_(e_invalid_دليل));
	أرجع;
    }
    fd1 = mch_fopen((حرف *)fname1, READBIN);
    إذا (fd1 == عدم)
    {
	semsg(_(e_cant_read_file_str), fname1);
	أرجع;
    }
    إذا (do_diff)
    {
	fd2 = mch_fopen((حرف *)fname2, READBIN);
	إذا (fd2 == عدم)
	{
	    fclose(fd1);
	    semsg(_(e_cant_read_file_str), fname2);
	    أرجع;
	}
    }

    init_ڪم_options(&opt);
    إذا (argvars[do_diff ? 2 : 1].v_type != VAR_UNKNOWN
	    && get_ڪم_options(&argvars[do_diff ? 2 : 1], &opt, 0,
		    JO2_رڪن_NAME + JO2_رڪن_کانن_ڳڻپ + JO2_رڪن_صفون
		    + JO2_VERTICAL + JO2_CURWIN + JO2_NORESTORE) == FAIL)
	اذهب theend;

    إذا (opt.jo_رڪن_name == عدم)
    {
	size_t len = STRLEN(fname1) + 12;

	fname_tofree = alloc(len);
	إذا (fname_tofree != عدم)
	{
	    vim_snprintf((حرف *)fname_tofree, len, "dump diff %s", fname1);
	    opt.jo_رڪن_name = fname_tofree;
	}
    }

    إذا (opt.jo_bufnr_buf != عدم)
    {
	win_T *wp = buf_jump_open_win(opt.jo_bufnr_buf);

	// With "bufnr" argument: enter the window with this buffer and make it
	// empty.
	إذا (wp == عدم)
	    semsg(_(e_invalid_دليل_str), "bufnr");
	وإلا
	{
	    buf = curbuf;
	    طالما (!(curbuf->b_ml.ml_flags & ML_خالي_جايون))
		ml_delete((linenr_T)1);
	    free_scrollback(curbuf->b_رڪن);
	    redraw_later(UPD_NOT_VALID);
	}
    }
    وإلا
	// Create a new terminal window.
	buf = رڪن_شروعات(&argvars[0], عدم, &opt, رڪن_شروعات_NOJOB);

    إذا (buf != عدم && buf->b_رڪن != عدم)
    {
	صحيح		i;
	linenr_T	bot_lnum;
	linenr_T	lnum;
	رڪن_T		*رڪن = buf->b_رڪن;
	صحيح		ويڪر;
	صحيح		width2;
	VTermPos	cursor_pos1;
	VTermPos	cursor_pos2;

	رڪن_init_default_colors(رڪن);

	rettv->vval.v_عدد = buf->b_fnum;

	// read the files, fill the buffer with the diff
	ويڪر = read_dump_file(fd1, &cursor_pos1);

	// position the cursor
	إذا (cursor_pos1.صف >= 0)
	{
	    curwin->w_cursor.lnum = cursor_pos1.صف + 1;
	    coladvance(cursor_pos1.کانو);
	}

	// Delete the empty line that was in the empty buffer.
	ml_delete(1);

	// For term_dumpload() we are done here.
	إذا (!do_diff)
	    اذهب theend;

	رڪن->tl_چوٽي_diff_صفون = curbuf->b_ml.ml_سٽ_ڳڻپ;

	textline = get_separator(ويڪر, fname1);
	إذا (textline == عدم)
	    اذهب theend;
	إذا (جوڙيو_خالي_جايون_scrollback(رڪن, &رڪن->tl_default_color, 0) == OK)
	    ml_append(curbuf->b_ml.ml_سٽ_ڳڻپ, textline, 0, FALSE);
	vim_free(textline);

	textline = get_separator(ويڪر, fname2);
	إذا (textline == عدم)
	    اذهب theend;
	إذا (جوڙيو_خالي_جايون_scrollback(رڪن, &رڪن->tl_default_color, 0) == OK)
	    ml_append(curbuf->b_ml.ml_سٽ_ڳڻپ, textline, 0, FALSE);
	textline[ويڪر] = NUL;

	bot_lnum = curbuf->b_ml.ml_سٽ_ڳڻپ;
	width2 = read_dump_file(fd2, &cursor_pos2);
	إذا (width2 > ويڪر)
	{
	    vim_free(textline);
	    textline = alloc(width2 + 1);
	    إذا (textline == عدم)
		اذهب theend;
	    ويڪر = width2;
	    textline[ويڪر] = NUL;
	}
	رڪن->tl_bot_diff_صفون = curbuf->b_ml.ml_سٽ_ڳڻپ - bot_lnum;

	لكل (lnum = 1; lnum <= رڪن->tl_چوٽي_diff_صفون; ++lnum)
	{
	    إذا (lnum + bot_lnum > curbuf->b_ml.ml_سٽ_ڳڻپ)
	    {
		// bottom part has fewer rows, fill with "-"
		لكل (i = 0; i < ويڪر; ++i)
		    textline[i] = '-';
	    }
	    وإلا
	    {
		char_u *line1;
		char_u *line2;
		char_u *p1;
		char_u *p2;
		صحيح	کانو;
		sb_سٽ_T   *sb_سٽ = (sb_سٽ_T *)رڪن->tl_scrollback.ga_معلومات;
		cellattr_T *cellattr1 = (sb_سٽ + lnum - 1)->sb_cells;
		cellattr_T *cellattr2 = (sb_سٽ + lnum + bot_lnum - 1)
								    ->sb_cells;

		// Make a copy, getting the second line will invalidate it.
		line1 = vim_strsave(ml_get(lnum));
		إذا (line1 == عدم)
		    اكسر;
		p1 = line1;

		line2 = ml_get(lnum + bot_lnum);
		p2 = line2;
		لكل (کانو = 0; کانو < ويڪر && *p1 != NUL && *p2 != NUL; ++کانو)
		{
		    صحيح len1 = utfc_ptr2len(p1);
		    صحيح len2 = utfc_ptr2len(p2);

		    textline[کانو] = ' ';
		    إذا (len1 != len2 || STRNCMP(p1, p2, len1) != 0)
			// text differs
			textline[کانو] = 'X';
		    وإلا إذا (lnum == cursor_pos1.صف + 1
			    && کانو == cursor_pos1.کانو
			    && (cursor_pos1.صف != cursor_pos2.صف
					|| cursor_pos1.کانو != cursor_pos2.کانو))
			// cursor in first but not in second
			textline[کانو] = '>';
		    وإلا إذا (lnum == cursor_pos2.صف + 1
			    && کانو == cursor_pos2.کانو
			    && (cursor_pos1.صف != cursor_pos2.صف
					|| cursor_pos1.کانو != cursor_pos2.کانو))
			// cursor in second but not in first
			textline[کانو] = '<';
		    وإلا إذا (cellattr1 != عدم && cellattr2 != عدم)
		    {
			إذا ((cellattr1 + کانو)->ويڪر
						   != (cellattr2 + کانو)->ويڪر)
			    textline[کانو] = 'w';
			وإلا إذا (!vterm_color_is_equal(&(cellattr1 + کانو)->fg,
						   &(cellattr2 + کانو)->fg))
			    textline[کانو] = 'f';
			وإلا إذا (!vterm_color_is_equal(&(cellattr1 + کانو)->bg,
						   &(cellattr2 + کانو)->bg))
			    textline[کانو] = 'b';
			وإلا إذا (vtermAttr2hl(&(cellattr1 + کانو)->attrs)
				  != vtermAttr2hl(&((cellattr2 + کانو)->attrs)))
			    textline[کانو] = 'a';
		    }
		    p1 += len1;
		    p2 += len2;
		    // TODO: handle different width
		}

		طالما (کانو < ويڪر)
		{
		    إذا (*p1 == NUL && *p2 == NUL)
			textline[کانو] = '?';
		    وإلا إذا (*p1 == NUL)
		    {
			textline[کانو] = '+';
			p2 += utfc_ptr2len(p2);
		    }
		    وإلا
		    {
			textline[کانو] = '-';
			p1 += utfc_ptr2len(p1);
		    }
		    ++کانو;
		}

		vim_free(line1);
	    }
	    إذا (جوڙيو_خالي_جايون_scrollback(رڪن, &رڪن->tl_default_color,
						 رڪن->tl_چوٽي_diff_صفون) == OK)
		ml_append(رڪن->tl_چوٽي_diff_صفون + lnum, textline, 0, FALSE);
	    ++bot_lnum;
	}

	طالما (lnum + bot_lnum <= curbuf->b_ml.ml_سٽ_ڳڻپ)
	{
	    // bottom part has more rows, fill with "+"
	    لكل (i = 0; i < ويڪر; ++i)
		textline[i] = '+';
	    إذا (جوڙيو_خالي_جايون_scrollback(رڪن, &رڪن->tl_default_color,
						 رڪن->tl_چوٽي_diff_صفون) == OK)
		ml_append(رڪن->tl_چوٽي_diff_صفون + lnum, textline, 0, FALSE);
	    ++lnum;
	    ++bot_lnum;
	}

	رڪن->tl_کانن_ڳڻپ = ويڪر;

	// looks better without wrapping
	curwin->w_p_wrap = 0;
    }

theend:
    vim_free(textline);
    vim_free(fname_tofree);
    fclose(fd1);
    إذا (fd2 != عدم)
	fclose(fd2);
}

/*
 * If the current buffer shows the output of term_dumpdiff(), swap the top and
 * bottom files.
 * Return FAIL when this is not possible.
 */
    صحيح
رڪن_مٽا_سٽا_diff(فراغ)
{
    رڪن_T	*رڪن = curbuf->b_رڪن;
    linenr_T	سٽ_ڳڻپ;
    linenr_T	چوٽي_صفون;
    linenr_T	bot_صفون;
    linenr_T	bot_شروعات;
    linenr_T	lnum;
    char_u	*p;
    sb_سٽ_T	*sb_سٽ;

    إذا (رڪن == عدم
	    || !رڪن_is_ختم(curbuf)
	    || رڪن->tl_چوٽي_diff_صفون == 0
	    || رڪن->tl_scrollback.ga_len == 0)
	أرجع FAIL;

    سٽ_ڳڻپ = curbuf->b_ml.ml_سٽ_ڳڻپ;
    چوٽي_صفون = رڪن->tl_چوٽي_diff_صفون;
    bot_صفون = رڪن->tl_bot_diff_صفون;
    bot_شروعات = سٽ_ڳڻپ - bot_صفون;
    sb_سٽ = (sb_سٽ_T *)رڪن->tl_scrollback.ga_معلومات;

    // move lines from top to above the bottom part
    لكل (lnum = 1; lnum <= چوٽي_صفون; ++lnum)
    {
	p = vim_strsave(ml_get(1));
	إذا (p == عدم)
	    أرجع OK;
	ml_append(bot_شروعات, p, 0, FALSE);
	ml_delete(1);
	vim_free(p);
    }

    // move lines from bottom to the top
    لكل (lnum = 1; lnum <= bot_صفون; ++lnum)
    {
	p = vim_strsave(ml_get(bot_شروعات + lnum));
	إذا (p == عدم)
	    أرجع OK;
	ml_delete(bot_شروعات + lnum);
	ml_append(lnum - 1, p, 0, FALSE);
	vim_free(p);
    }

    // move top title to bottom
    p = vim_strsave(ml_get(bot_صفون + 1));
    إذا (p == عدم)
	أرجع OK;
    ml_append(سٽ_ڳڻپ - چوٽي_صفون - 1, p, 0, FALSE);
    ml_delete(bot_صفون + 1);
    vim_free(p);

    // move bottom title to top
    p = vim_strsave(ml_get(سٽ_ڳڻپ - چوٽي_صفون));
    إذا (p == عدم)
	أرجع OK;
    ml_delete(سٽ_ڳڻپ - چوٽي_صفون);
    ml_append(bot_صفون, p, 0, FALSE);
    vim_free(p);

    إذا (چوٽي_صفون == bot_صفون)
    {
	// rows counts are equal, can swap cell properties
	لكل (lnum = 0; lnum < چوٽي_صفون; ++lnum)
	{
	    sb_سٽ_T	عارضي;

	    عارضي = *(sb_سٽ + lnum);
	    *(sb_سٽ + lnum) = *(sb_سٽ + bot_شروعات + lnum);
	    *(sb_سٽ + bot_شروعات + lnum) = عارضي;
	}
    }
    وإلا
    {
	size_t		سائيز = حجم(sb_سٽ_T) * رڪن->tl_scrollback.ga_len;
	sb_سٽ_T	*عارضي = alloc(سائيز);

	// need to copy cell properties into temp memory
	إذا (عارضي != عدم)
	{
	    mch_memmove(عارضي, رڪن->tl_scrollback.ga_معلومات, سائيز);
	    mch_memmove(رڪن->tl_scrollback.ga_معلومات,
		    عارضي + bot_شروعات,
		    حجم(sb_سٽ_T) * bot_صفون);
	    mch_memmove((sb_سٽ_T *)رڪن->tl_scrollback.ga_معلومات + bot_صفون,
		    عارضي + چوٽي_صفون,
		    حجم(sb_سٽ_T) * (سٽ_ڳڻپ - چوٽي_صفون - bot_صفون));
	    mch_memmove((sb_سٽ_T *)رڪن->tl_scrollback.ga_معلومات
						       + سٽ_ڳڻپ - چوٽي_صفون,
		    عارضي,
		    حجم(sb_سٽ_T) * چوٽي_صفون);
	    vim_free(عارضي);
	}
    }

    رڪن->tl_چوٽي_diff_صفون = bot_صفون;
    رڪن->tl_bot_diff_صفون = چوٽي_صفون;

    update_screen(UPD_NOT_VALID);
    أرجع OK;
}

/*
 * "term_dumpdiff(filename, filename, options)" function
 */
    فراغ
f_رڪن_dumpdiff(typval_T *argvars, typval_T *rettv)
{
    إذا (اندريون_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	أرجع;

    رڪن_ڀريو_dump(argvars, rettv, TRUE);
}

/*
 * "term_dumpload(filename, options)" function
 */
    فراغ
f_رڪن_dumpload(typval_T *argvars, typval_T *rettv)
{
    إذا (اندريون_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	أرجع;

    رڪن_ڀريو_dump(argvars, rettv, FALSE);
}

/*
 * "term_getaltscreen(buf)" function
 */
    فراغ
f_رڪن_getaltscreen(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    إذا (اندريون_vim9script() && check_for_بفر_arg(argvars, 0) == FAIL)
	أرجع;

    buf = رڪن_get_buf(argvars, "term_getaltscreen()");
    إذا (buf == عدم)
	أرجع;
    rettv->vval.v_عدد = buf->b_رڪن->tl_using_altscreen;
}

/*
 * "term_getattr(attr, name)" function
 */
    فراغ
f_رڪن_getattr(typval_T *argvars, typval_T *rettv)
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

    إذا (اندريون_vim9script()
	    && (check_for_عدد_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	أرجع;

    attr = tv_get_عدد(&argvars[0]);
    name = tv_get_string_chk(&argvars[1]);
    إذا (name == عدم)
	أرجع;

    إذا (attr > HL_ALL)
	attr = syn_attr2attr(attr);
    لكل (i = 0; i < ARRAY_ڊيگهائي(attrs); ++i)
	إذا (STRCMP(name, attrs[i].name) == 0)
	{
	    rettv->vval.v_عدد = (attr & attrs[i].attr) != 0 ? 1 : 0;
	    اكسر;
	}
}

/*
 * "term_getcursor(buf)" function
 */
    فراغ
f_رڪن_getcursor(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    رڪن_T	*رڪن;
    list_T	*l;
    dict_T	*d;

    إذا (rettv_list_alloc(rettv) == FAIL)
	أرجع;

    إذا (اندريون_vim9script() && check_for_بفر_arg(argvars, 0) == FAIL)
	أرجع;

    buf = رڪن_get_buf(argvars, "term_getcursor()");
    إذا (buf == عدم)
	أرجع;
    رڪن = buf->b_رڪن;

    l = rettv->vval.v_list;
    list_append_عدد(l, رڪن->tl_cursor_pos.صف + 1);
    list_append_عدد(l, رڪن->tl_cursor_pos.کانو + 1);

    d = dict_alloc();
    إذا (d == عدم)
	أرجع;

    dict_جوڙيو_عدد(d, "visible", رڪن->tl_cursor_visible);
    dict_جوڙيو_عدد(d, "blink", blink_state_is_inverted()
	    ? !رڪن->tl_cursor_blink : رڪن->tl_cursor_blink);
    dict_جوڙيو_عدد(d, "shape", رڪن->tl_cursor_shape);
    dict_جوڙيو_string(d, "color", cursor_color_get(رڪن->tl_cursor_color));
    إذا (list_append_dict(l, d) == FAIL)
	dict_unref(d);
}

/*
 * "term_getjob(buf)" function
 */
    فراغ
f_رڪن_getjob(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    إذا (اندريون_vim9script() && check_for_بفر_arg(argvars, 0) == FAIL)
	أرجع;

    buf = رڪن_get_buf(argvars, "term_getjob()");
    إذا (buf == عدم)
    {
	إذا (اندريون_vim9script())
	{
	    rettv->v_type = VAR_ڪم;
	    rettv->vval.v_ڪم = عدم;
	}
	وإلا
	{
	    rettv->v_type = VAR_SPECIAL;
	    rettv->vval.v_عدد = VVAL_NULL;
	}
	أرجع;
    }

    rettv->v_type = VAR_ڪم;
    rettv->vval.v_ڪم = buf->b_رڪن->tl_ڪم;
    إذا (rettv->vval.v_ڪم != عدم)
	++rettv->vval.v_ڪم->jv_refcount;
}

    ساكن صحيح
get_صف_عدد(typval_T *tv, رڪن_T *رڪن)
{
    إذا (tv->v_type == VAR_STRING
	    && tv->vval.v_string != عدم
	    && STRCMP(tv->vval.v_string, ".") == 0)
	أرجع رڪن->tl_cursor_pos.صف;
    أرجع (صحيح)tv_get_عدد(tv) - 1;
}

/*
 * "term_getline(buf, row)" function
 */
    فراغ
f_رڪن_getline(typval_T *argvars, typval_T *rettv)
{
    buf_T	    *buf;
    رڪن_T	    *رڪن;
    صحيح		    صف;

    rettv->v_type = VAR_STRING;

    إذا (اندريون_vim9script()
	    && (check_for_بفر_arg(argvars, 0) == FAIL
		|| check_for_lnum_arg(argvars, 1) == FAIL))
	أرجع;

    buf = رڪن_get_buf(argvars, "term_getline()");
    إذا (buf == عدم)
	أرجع;
    رڪن = buf->b_رڪن;
    صف = get_صف_عدد(&argvars[1], رڪن);

    إذا (رڪن->tl_vterm == عدم)
    {
	linenr_T  lnum = 0;
	size_t	  وٿي = 0;
	صحيح	  sb_صف = رڪن->tl_scrollback_scrolled + صف;
	sb_سٽ_T *سٽ;

	إذا (sb_صف < 0 || sb_صف >= رڪن->tl_scrollback.ga_len)
	    أرجع;
	سٽ = (sb_سٽ_T *)رڪن->tl_scrollback.ga_معلومات + sb_صف;

	scrollbackline_pos_اندريون_buf(رڪن, sb_صف, &lnum, عدم, &وٿي);

	// vterm is finished, get the text from the buffer
	إذا (lnum > 0 && lnum <= buf->b_ml.ml_سٽ_ڳڻپ)
	{
	    char_u *p = ml_get_buf(buf, lnum, FALSE);
	    إذا (STRLEN(p) >= وٿي + سٽ->sb_بائيٽون)
		rettv->vval.v_string = vim_strnsave(p + وٿي, سٽ->sb_بائيٽون);
	}
    }
    وإلا
    {
	VTermScreen	*screen = vterm_obtain_screen(رڪن->tl_vterm);
	VTermRect	rect;
	صحيح		len;
	char_u		*p;

	إذا (صف < 0 || صف >= رڪن->tl_صفون)
	    أرجع;
	len = رڪن->tl_کانن_ڳڻپ * MB_MAXBYTES + 1;
	p = alloc(len);
	إذا (p == عدم)
	    أرجع;
	rettv->vval.v_string = p;

	rect.شروعات_کانو = 0;
	rect.پڇاڙي_کانو = رڪن->tl_کانن_ڳڻپ;
	rect.شروعات_صف = صف;
	rect.پڇاڙي_صف = صف + 1;
	p[vterm_screen_get_متن(screen, (حرف *)p, len, rect)] = NUL;
    }
}

/*
 * "term_getscrolled(buf)" function
 */
    فراغ
f_رڪن_getscrolled(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    إذا (اندريون_vim9script() && check_for_بفر_arg(argvars, 0) == FAIL)
	أرجع;

    buf = رڪن_get_buf(argvars, "term_getscrolled()");
    إذا (buf == عدم)
	أرجع;
    rettv->vval.v_عدد = buf->b_رڪن->tl_بفر_scrolled;
}

/*
 * "term_getsize(buf)" function
 */
    فراغ
f_رڪن_getsize(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    list_T	*l;

    إذا (rettv_list_alloc(rettv) == FAIL)
	أرجع;

    إذا (اندريون_vim9script() && check_for_بفر_arg(argvars, 0) == FAIL)
	أرجع;

    buf = رڪن_get_buf(argvars, "term_getsize()");
    إذا (buf == عدم)
	أرجع;

    l = rettv->vval.v_list;
    list_append_عدد(l, buf->b_رڪن->tl_صفون);
    list_append_عدد(l, buf->b_رڪن->tl_کانن_ڳڻپ);
}

/*
 * "term_setsize(buf, rows, cols)" function
 */
    فراغ
f_رڪن_setsize(typval_T *argvars, typval_T *rettv اڻ_استعمال_ٿيل)
{
    buf_T	*buf;
    رڪن_T	*رڪن;
    varnumber_T صفون_2, کانن_ڳڻپ;

    إذا (اندريون_vim9script()
	    && (check_for_بفر_arg(argvars, 0) == FAIL
		|| check_for_عدد_arg(argvars, 1) == FAIL
		|| check_for_عدد_arg(argvars, 2) == FAIL))
	أرجع;

    buf = رڪن_get_buf(argvars, "term_setsize()");
    إذا (buf == عدم)
    {
	emsg(_(e_not_terminal_بفر));
	أرجع;
    }
    إذا (buf->b_رڪن->tl_vterm == عدم)
	أرجع;
    رڪن = buf->b_رڪن;
    صفون_2 = tv_get_عدد(&argvars[1]);
    صفون_2 = صفون_2 <= 0 ? رڪن->tl_صفون : صفون_2;
    کانن_ڳڻپ = tv_get_عدد(&argvars[2]);
    کانن_ڳڻپ = کانن_ڳڻپ <= 0 ? رڪن->tl_کانن_ڳڻپ : کانن_ڳڻپ;
    vterm_سيٽ_سائيز(رڪن->tl_vterm, صفون_2, کانن_ڳڻپ);
    // handle_resize() will resize the windows

    // Get and remember the size we ended up with.  Update the pty.
    vterm_get_سائيز(رڪن->tl_vterm, &رڪن->tl_صفون, &رڪن->tl_کانن_ڳڻپ);
    رڪن_report_winsize(رڪن, رڪن->tl_صفون, رڪن->tl_کانن_ڳڻپ);
}

/*
 * "term_getstatus(buf)" function
 */
    فراغ
f_رڪن_getstatus(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    رڪن_T	*رڪن;
    char_u	val[100];

    rettv->v_type = VAR_STRING;

    إذا (اندريون_vim9script() && check_for_بفر_arg(argvars, 0) == FAIL)
	أرجع;

    buf = رڪن_get_buf(argvars, "term_getstatus()");
    إذا (buf == عدم)
	أرجع;
    رڪن = buf->b_رڪن;

    إذا (رڪن_ڪم_running(رڪن))
	STRCPY(val, "running");
    وإلا
	STRCPY(val, "finished");
    إذا (رڪن->tl_عام_گهڻائي)
	STRCAT(val, ",normal");
    rettv->vval.v_string = vim_strsave(val);
}

/*
 * "term_gettitle(buf)" function
 */
    فراغ
f_رڪن_gettitle(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    rettv->v_type = VAR_STRING;

    إذا (اندريون_vim9script() && check_for_بفر_arg(argvars, 0) == FAIL)
	أرجع;

    buf = رڪن_get_buf(argvars, "term_gettitle()");
    إذا (buf == عدم)
	أرجع;

    إذا (buf->b_رڪن->tl_title != عدم)
	rettv->vval.v_string = vim_strsave(buf->b_رڪن->tl_title);
}

/*
 * "term_gettty(buf)" function
 */
    فراغ
f_رڪن_gettty(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    char_u	*p = عدم;
    صحيح		انگ = 0;

    إذا (اندريون_vim9script()
	    && (check_for_بفر_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	أرجع;

    rettv->v_type = VAR_STRING;
    buf = رڪن_get_buf(argvars, "term_gettty()");
    إذا (buf == عدم)
	أرجع;
    إذا (argvars[1].v_type != VAR_UNKNOWN)
	انگ = tv_get_bool(&argvars[1]);

    بدل (انگ)
    {
	حالة 0:
	    إذا (buf->b_رڪن->tl_ڪم != عدم)
		p = buf->b_رڪن->tl_ڪم->jv_tty_ٻاهريون;
	    اكسر;
	حالة 1:
	    إذا (buf->b_رڪن->tl_ڪم != عدم)
		p = buf->b_رڪن->tl_ڪم->jv_tty_اندريون;
	    اكسر;
	افتراضي:
	    semsg(_(e_invalid_دليل_str), tv_get_string(&argvars[1]));
	    أرجع;
    }
    إذا (p != عدم)
	rettv->vval.v_string = vim_strsave(p);
}

/*
 * "term_list()" function
 */
    فراغ
f_رڪن_list(typval_T *argvars اڻ_استعمال_ٿيل, typval_T *rettv)
{
    رڪن_T	*tp;
    list_T	*l;

    إذا (rettv_list_alloc(rettv) == FAIL || پهريون_رڪن == عدم)
	أرجع;

    l = rettv->vval.v_list;
    FOR_ALL_TERMS(tp)
	إذا (tp->tl_بفر != عدم)
	    إذا (list_append_عدد(l,
				   (varnumber_T)tp->tl_بفر->b_fnum) == FAIL)
		أرجع;
}

/*
 * "term_scrape(buf, row)" function
 */
    فراغ
f_رڪن_scrape(typval_T *argvars, typval_T *rettv)
{
    buf_T	    *buf;
    VTermScreen	    *screen = عدم;
    VTermPos	    pos;
    list_T	    *l;
    رڪن_T	    *رڪن;
    char_u	    *p;
    sb_سٽ_T	    *سٽ;

    إذا (rettv_list_alloc(rettv) == FAIL)
	أرجع;

    إذا (اندريون_vim9script()
	    && (check_for_بفر_arg(argvars, 0) == FAIL
		|| check_for_lnum_arg(argvars, 1) == FAIL))
	أرجع;

    buf = رڪن_get_buf(argvars, "term_scrape()");
    إذا (buf == عدم)
	أرجع;
    رڪن = buf->b_رڪن;

    l = rettv->vval.v_list;
    pos.صف = get_صف_عدد(&argvars[1], رڪن);

    إذا (رڪن->tl_vterm != عدم)
    {
	screen = vterm_obtain_screen(رڪن->tl_vterm);
	إذا (screen == عدم)  // can't really happen
	    أرجع;
	p = عدم;
	سٽ = عدم;
    }
    وإلا
    {
	صحيح	  sb_صف = رڪن->tl_scrollback_scrolled + pos.صف;
	linenr_T  lnum = 0;
	size_t	  وٿي = 0;

	scrollbackline_pos_اندريون_buf(رڪن, sb_صف, &lnum, عدم, &وٿي);

	إذا (sb_صف >= رڪن->tl_scrollback.ga_len || lnum <= 0 || lnum > buf->b_ml.ml_سٽ_ڳڻپ)
	    أرجع;

	سٽ = (sb_سٽ_T *)رڪن->tl_scrollback.ga_معلومات + sb_صف;
	p = ml_get_buf(buf, lnum, FALSE);

	إذا (STRLEN(p) < وٿي + سٽ->sb_بائيٽون)
	    أرجع;

	p += وٿي;
    }

    لكل (pos.کانو = 0; pos.کانو < رڪن->tl_کانن_ڳڻپ; )
    {
	dict_T		*dcell;
	صحيح		ويڪر;
	VTermScreenCellAttrs attrs;
	VTermColor	fg, bg;
	char_u		rgb[8];
	size_t		rgblen;
	char_u		mbs[MB_MAXBYTES * VTERM_MAX_CHARS_في_CELL + 1];
	size_t		mbslen;
	صحيح		i;

	إذا (screen == عدم)
	{
	    cellattr_T	*cellattr;

	    // vterm has finished, get the cell from scrollback
	    إذا (pos.کانو >= سٽ->sb_کانن_ڳڻپ)
		اكسر;
	    cellattr = سٽ->sb_cells + pos.کانو;
	    ويڪر = cellattr->ويڪر;
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

	    إذا (vterm_screen_get_cell(screen, pos, &cell) == 0)
		اكسر;

	    mbslen = 0;
	    لكل (i = 0; i < VTERM_MAX_CHARS_في_CELL; ++i)
	    {
		إذا (cell.chars[i] == 0)
		    اكسر;
		mbslen += (*utf_char2bytes)((صحيح)cell.chars[i], mbs + mbslen);
	    }
	    mbs[mbslen] = NUL;
	    ويڪر = cell.ويڪر;
	    attrs = cell.attrs;
	    fg = cell.fg;
	    bg = cell.bg;
	}
	dcell = dict_alloc();
	إذا (dcell == عدم)
	    اكسر;
	إذا (list_append_dict(l, dcell) == FAIL)
	{
	    dict_unref(dcell);
	    اكسر;
	}

	dict_جوڙيو_string_len(dcell, "chars", mbs, (صحيح)mbslen);

	rgblen = vim_snprintf_safelen((حرف *)rgb, حجم(rgb),
	    "#%02x%02x%02x", fg.red, fg.green, fg.blue);
	dict_جوڙيو_string_len(dcell, "fg", rgb, (صحيح)rgblen);
	rgblen = vim_snprintf_safelen((حرف *)rgb, حجم(rgb),
	    "#%02x%02x%02x", bg.red, bg.green, bg.blue);
	dict_جوڙيو_string_len(dcell, "bg", rgb, (صحيح)rgblen);

	dict_جوڙيو_عدد(dcell, "attr",
				      cell2attr(رڪن, عدم, &attrs, &fg, &bg));
	dict_جوڙيو_عدد(dcell, "width", ويڪر);

	++pos.کانو;
	إذا (ويڪر == 2)
	    ++pos.کانو;
    }
}

/*
 * "term_sendkeys(buf, keys)" function
 */
    فراغ
f_رڪن_sendkeys(typval_T *argvars, typval_T *rettv اڻ_استعمال_ٿيل)
{
    buf_T	*buf;
    char_u	*نياپو;
    رڪن_T	*رڪن;

    إذا (اندريون_vim9script()
	    && (check_for_بفر_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	أرجع;

    buf = رڪن_get_buf(argvars, "term_sendkeys()");
    إذا (buf == عدم)
	أرجع;

    نياپو = tv_get_string_chk(&argvars[1]);
    إذا (نياپو == عدم)
	أرجع;
    رڪن = buf->b_رڪن;
    إذا (رڪن->tl_vterm == عدم)
	أرجع;

    طالما (*نياپو != NUL)
    {
	صحيح c;

	إذا (*نياپو == K_SPECIAL && نياپو[1] != NUL && نياپو[2] != NUL)
	{
	    c = منزل_SPECIAL(نياپو[1], نياپو[2]);
	    نياپو += 3;
	}
	وإلا
	{
	    c = PTR2CHAR(نياپو);
	    نياپو += MB_CPTR2LEN(نياپو);
	}
	send_ڪنجيون_منزل_رڪن(رڪن, c, 0, FALSE);
    }
}

#إذا defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * "term_getansicolors(buf)" function
 */
    فراغ
f_رڪن_getansicolors(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    رڪن_T	*رڪن;
    VTermState	*state;
    VTermColor  color;
    char_u	hexbuf[10];
    صحيح		اشاريو;
    list_T	*list;

    إذا (rettv_list_alloc(rettv) == FAIL)
	أرجع;

    إذا (اندريون_vim9script() && check_for_بفر_arg(argvars, 0) == FAIL)
	أرجع;

    buf = رڪن_get_buf(argvars, "term_getansicolors()");
    إذا (buf == عدم)
	أرجع;
    رڪن = buf->b_رڪن;
    إذا (رڪن->tl_vterm == عدم)
	أرجع;

    list = rettv->vval.v_list;
    state = vterm_obtain_state(رڪن->tl_vterm);
    لكل (اشاريو = 0; اشاريو < 16; اشاريو++)
    {
	size_t	hexbuflen;

	vterm_state_get_palette_color(state, اشاريو, &color);
	hexbuflen = vim_snprintf_safelen((حرف *)hexbuf, حجم(hexbuf),
	    "#%02x%02x%02x", color.red, color.green, color.blue);
	إذا (list_append_string(list, hexbuf, (صحيح)hexbuflen) == FAIL)
	    أرجع;
    }
}

/*
 * "term_setansicolors(buf, list)" function
 */
    فراغ
f_رڪن_setansicolors(typval_T *argvars, typval_T *rettv اڻ_استعمال_ٿيل)
{
    buf_T	*buf;
    رڪن_T	*رڪن;
    listitem_T	*li;
    صحيح		n = 0;

    إذا (اندريون_vim9script()
	    && (check_for_بفر_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL))
	أرجع;

    buf = رڪن_get_buf(argvars, "term_setansicolors()");
    إذا (buf == عدم)
	أرجع;
    رڪن = buf->b_رڪن;
    إذا (رڪن->tl_vterm == عدم)
	أرجع;

    إذا (check_for_nonnull_list_arg(argvars, 1) == FAIL)
	أرجع;

    إذا (argvars[1].vval.v_list->lv_پهريون == &range_list_شي
	    || argvars[1].vval.v_list->lv_len != 16)
    {
	semsg(_(e_invalid_قدر_for_دليل_str), "\"colors\"");
	أرجع;
    }

    إذا (رڪن->tl_palette == عدم)
	رڪن->tl_palette = ALLOC_MULT(long_u, 16);
    إذا (رڪن->tl_palette == عدم)
	أرجع;

    FOR_ALL_LIST_شيون(argvars[1].vval.v_list, li)
    {
	char_u		*color_name;
	guicolor_T	guicolor;

	color_name = tv_get_string_chk(&li->li_tv);
	إذا (color_name == عدم)
	    أرجع;

	guicolor = GUI_GET_COLOR(color_name);
	إذا (guicolor == INVALCOLOR)
	{
	    semsg(_(e_cannot_allocate_color_str), color_name);
	    أرجع;
	}

	رڪن->tl_palette[n++] = GUI_MCH_GET_RGB(guicolor);
    }

    رڪن_update_palette(رڪن);
}
#جيڪڏهن_پورو

/*
 * "term_setapi(buf, api)" function
 */
    فراغ
f_رڪن_setapi(typval_T *argvars, typval_T *rettv اڻ_استعمال_ٿيل)
{
    buf_T	*buf;
    رڪن_T	*رڪن;
    char_u	*api;

    إذا (اندريون_vim9script()
	    && (check_for_بفر_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	أرجع;

    buf = رڪن_get_buf(argvars, "term_setapi()");
    إذا (buf == عدم)
	أرجع;
    رڪن = buf->b_رڪن;
    vim_free(رڪن->tl_api);
    api = tv_get_string_chk(&argvars[1]);
    إذا (api != عدم)
	رڪن->tl_api = vim_strsave(api);
    وإلا
	رڪن->tl_api = عدم;
}

/*
 * "term_setrestore(buf, command)" function
 */
    فراغ
f_رڪن_setrestore(typval_T *argvars اڻ_استعمال_ٿيل, typval_T *rettv اڻ_استعمال_ٿيل)
{
#إذا defined(FEAT_SESSION)
    buf_T	*buf;
    رڪن_T	*رڪن;
    char_u	*cmd;

    إذا (اندريون_vim9script()
	    && (check_for_بفر_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	أرجع;

    buf = رڪن_get_buf(argvars, "term_setrestore()");
    إذا (buf == عدم)
	أرجع;
    رڪن = buf->b_رڪن;
    vim_free(رڪن->tl_command);
    cmd = tv_get_string_chk(&argvars[1]);
    إذا (cmd != عدم)
	رڪن->tl_command = vim_strsave(cmd);
    وإلا
	رڪن->tl_command = عدم;
#جيڪڏهن_پورو
}

/*
 * "term_setkill(buf, how)" function
 */
    فراغ
f_رڪن_setkill(typval_T *argvars اڻ_استعمال_ٿيل, typval_T *rettv اڻ_استعمال_ٿيل)
{
    buf_T	*buf;
    رڪن_T	*رڪن;
    char_u	*how;

    إذا (اندريون_vim9script()
	    && (check_for_بفر_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	أرجع;

    buf = رڪن_get_buf(argvars, "term_setkill()");
    إذا (buf == عدم)
	أرجع;
    رڪن = buf->b_رڪن;
    vim_free(رڪن->tl_kill);
    how = tv_get_string_chk(&argvars[1]);
    إذا (how != عدم)
	رڪن->tl_kill = vim_strsave(how);
    وإلا
	رڪن->tl_kill = عدم;
}

/*
 * "term_start(command, options)" function
 */
    فراغ
f_رڪن_شروعات(typval_T *argvars, typval_T *rettv)
{
    jobopt_T	opt;
    buf_T	*buf;

    إذا (اندريون_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	أرجع;

    init_ڪم_options(&opt);
    إذا (argvars[1].v_type != VAR_UNKNOWN
	    && get_ڪم_options(&argvars[1], &opt,
		JO_TIMEOUT_ALL + JO_STOPONEXIT
		    + JO_CALLBACK + JO_ٻاهريون_CALLBACK + JO_ERR_CALLBACK
		    + JO_EXIT_CB + JO_CLOSE_CALLBACK + JO_ٻاهريون_IO,
		JO2_رڪن_NAME + JO2_رڪن_پڄاڻي + JO2_HIDDEN + JO2_رڪن_OPENCMD
		    + JO2_رڪن_کانن_ڳڻپ + JO2_رڪن_صفون + JO2_VERTICAL + JO2_CURWIN
		    + JO2_CWD + JO2_ENV + JO2_EOF_CHARS
		    + JO2_NORESTORE + JO2_رڪن_KILL + JO2_رڪن_HIGHLIGHT
		    + JO2_ANSI_COLORS + JO2_TTY_TYPE + JO2_رڪن_API) == FAIL)
	أرجع;

    buf = رڪن_شروعات(&argvars[0], عدم, &opt, 0);

    إذا (buf != عدم && buf->b_رڪن != عدم)
	rettv->vval.v_عدد = buf->b_fnum;
}

/*
 * "term_wait" function
 */
    فراغ
f_رڪن_wait(typval_T *argvars, typval_T *rettv اڻ_استعمال_ٿيل)
{
    buf_T	*buf;

    إذا (اندريون_vim9script()
	    && (check_for_بفر_arg(argvars, 0) == FAIL
		|| check_for_opt_عدد_arg(argvars, 1) == FAIL))
	أرجع;

    buf = رڪن_get_buf(argvars, "term_wait()");
    إذا (buf == عدم)
	أرجع;
    إذا (buf->b_رڪن->tl_ڪم == عدم)
    {
	ch_log(عدم, "term_wait(): no job to wait for");
	أرجع;
    }
    إذا (buf->b_رڪن->tl_ڪم->jv_channel == عدم)
	// channel is closed, nothing to do
	أرجع;

    // Get the job status, this will detect a job that finished.
    إذا (!buf->b_رڪن->tl_ڪم->jv_channel->ch_keep_open
	    && STRCMP(ڪم_حالت(buf->b_رڪن->tl_ڪم), "dead") == 0)
    {
	// The job is dead, keep reading channel I/O until the channel is
	// closed. buf->b_term may become NULL if the terminal was closed while
	// waiting.
	ch_log(عدم, "term_wait(): waiting for channel to close");
	طالما (buf->b_رڪن != عدم && !buf->b_رڪن->tl_channel_closed)
	{
	    رڪن_flush_messages();

	    ui_delay(10L, FALSE);
	    إذا (!buf_valid(buf))
		// If the terminal is closed when the channel is closed the
		// buffer disappears.
		اكسر;
	    إذا (buf->b_رڪن == عدم || buf->b_رڪن->tl_channel_closing)
		// came here from a close callback, only wait one time
		اكسر;
	}

	رڪن_flush_messages();
    }
    وإلا
    {
	ڊگهو wait = 10L;

	رڪن_flush_messages();

	// Wait for some time for any channel I/O.
	إذا (argvars[1].v_type != VAR_UNKNOWN)
	    wait = tv_get_عدد(&argvars[1]);
	ui_delay(wait, TRUE);

	// Flushing messages on channels is hopefully sufficient.
	// TODO: is there a better way?
	رڪن_flush_messages();
    }
}

/*
 * Called when a channel has sent all the lines to a terminal.
 * Send a CTRL-D to mark the end of the text.
 */
    فراغ
رڪن_send_eof(channel_T *ch)
{
    رڪن_T	*رڪن;

    FOR_ALL_TERMS(رڪن)
	إذا (رڪن->tl_ڪم == ch->ch_ڪم)
	{
	    إذا (رڪن->tl_eof_chars != عدم)
	    {
		channel_send(ch, PART_اندريون, رڪن->tl_eof_chars,
					(صحيح)STRLEN(رڪن->tl_eof_chars), عدم);
		channel_send(ch, PART_اندريون, (char_u *)"\r", 1, عدم);
	    }
#جيڪڏهن_وصف_ٿيل MSWIN
	    وإلا
		// Default: CTRL-D
		channel_send(ch, PART_اندريون, (char_u *)"\004\r", 2, عدم);
#جيڪڏهن_پورو
	}
}

#إذا defined(FEAT_GUI)
    ڪم_T *
رڪن_getjob(رڪن_T *رڪن)
{
    أرجع رڪن != عدم ? رڪن->tl_ڪم : عدم;
}
#جيڪڏهن_پورو

#إذا defined(MSWIN)

///////////////////////////////////////
// 2. MS-Windows implementation.

HRESULT (WINAPI *pCreatePseudoConsole)(COORD, HANDLE, HANDLE, DWORD, HPCON*);
HRESULT (WINAPI *pResizePseudoConsole)(HPCON, COORD);
HRESULT (WINAPI *pClosePseudoConsole)(HPCON);
BOOL (WINAPI *pInitializeProcThreadAttributeList)(LPPROC_THREAD_ATTRIBUTE_LIST, DWORD, DWORD, PSIZE_T);
BOOL (WINAPI *pUpdateProcThreadAttribute)(LPPROC_THREAD_ATTRIBUTE_LIST, DWORD, DWORD_PTR, PVOID, سائيز_T, PVOID, PSIZE_T);
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
    } conpty_داخلا[] =
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

    إذا (!has_conpty_working())
    {
	إذا (verbose)
	    emsg(_(e_conpty_is_not_موجود));
	أرجع FAIL;
    }

    // No need to initialize twice.
    إذا (hKerneldll)
	أرجع OK;

    hKerneldll = vim_ڀريو_Lib("kernel32.dll");
    لكل (i = 0; conpty_داخلا[i].name != عدم
					&& conpty_داخلا[i].ptr != عدم; ++i)
    {
	إذا ((*conpty_داخلا[i].ptr = (FARPROC)Get_Proc_پتو(hKerneldll,
						conpty_داخلا[i].name)) == عدم)
	{
	    إذا (verbose)
		semsg(_(e_could_not_ڀريو_library_function_str), conpty_داخلا[i].name);
	    hKerneldll = عدم;
	    أرجع FAIL;
	}
    }

    أرجع OK;
}

    ساكن صحيح
conpty_رڪن_and_ڪم_init(
	رڪن_T	    *رڪن,
	typval_T    *argvar,
	حرف	    **argv اڻ_استعمال_ٿيل,
	jobopt_T    *opt,
	jobopt_T    *orig_opt)
{
    WCHAR	    *cmd_wchar = عدم;
    WCHAR	    *cmd_wchar_copy = عدم;
    WCHAR	    *cwd_wchar = عدم;
    WCHAR	    *env_wchar = عدم;
    channel_T	    *channel = عدم;
    ڪم_T	    *ڪم = عدم;
    HANDLE	    jo = عدم;
    garray_T	    ga_cmd, ga_env;
    char_u	    *cmd = عدم;
    HRESULT	    hr;
    COORD	    consize;
    سائيز_T	    breq;
    PROCESS_INFORMATION proc_info;
    HANDLE	    i_theirs = عدم;
    HANDLE	    o_theirs = عدم;
    HANDLE	    i_ours = عدم;
    HANDLE	    o_ours = عدم;
    حرف	    *errmsg = عدم;

    ga_init2(&ga_cmd, حجم(حرف*), 20);
    ga_init2(&ga_env, حجم(حرف*), 20);

    إذا (argvar->v_type == VAR_STRING)
    {
	cmd = argvar->vval.v_string;
    }
    وإلا إذا (argvar->v_type == VAR_LIST)
    {
	إذا (win32_build_cmd(argvar->vval.v_list, &ga_cmd) == FAIL)
	    اذهب failed;
	cmd = ga_cmd.ga_معلومات;
    }
    إذا (cmd == عدم || *cmd == NUL)
    {
	emsg(_(e_invalid_دليل));
	اذهب failed;
    }

    رڪن->tl_arg0_cmd = vim_strsave(cmd);

    cmd_wchar = enc_منزل_utf16(cmd, عدم);

    إذا (cmd_wchar != عدم)
    {
	// Request by CreateProcessW
	breq = wcslen(cmd_wchar) + 1 + 1;	// Addition of NUL by API
	cmd_wchar_copy = ALLOC_MULT(WCHAR, breq);
	wcsncpy(cmd_wchar_copy, cmd_wchar, breq - 1);
    }

    ga_clear(&ga_cmd);
    إذا (cmd_wchar == عدم)
	اذهب failed;
    إذا (opt->jo_cwd != عدم)
    {
	cwd_wchar = enc_منزل_utf16(opt->jo_cwd, عدم);
	إذا (cwd_wchar == عدم)
	    اذهب failed;
    }

    win32_build_env(opt->jo_env, &ga_env, TRUE);
    env_wchar = ga_env.ga_معلومات;

    إذا (!CreatePipe(&i_theirs, &i_ours, عدم, 0))
	اذهب failed;
    إذا (!CreatePipe(&o_ours, &o_theirs, عدم, 0))
	اذهب failed;

    consize.X = رڪن->tl_کانن_ڳڻپ;
    consize.Y = رڪن->tl_صفون;
    hr = pCreatePseudoConsole(consize, i_theirs, o_theirs, 0,
							     &رڪن->tl_conpty);
    إذا (FAILED(hr))
	اذهب failed;

    رڪن->tl_siex.StartupInfo.cb = حجم(رڪن->tl_siex);

    // Explicitly invalidate std handles to prevent inheritance of
    // the debugger's stdout (e.g., in Visual Studio debugging sessions),
    // which could cause job output to go to the debugger instead of
    // the intended ConPTY, even with bInheritHandles set to FALSE in CreateProcess.
    رڪن->tl_siex.StartupInfo.dwFlags = STARTF_USESTDHANDLES;
    رڪن->tl_siex.StartupInfo.h_Std_داخلا = INVALID_HANDLE_قدر;
    رڪن->tl_siex.StartupInfo.hStdOutput = INVALID_HANDLE_قدر;
    رڪن->tl_siex.StartupInfo.hStdError = INVALID_HANDLE_قدر;

    // Set up pipe inheritance safely: Vista or later.
    pInitializeProcThreadAttributeList(عدم, 1, 0, &breq);
    رڪن->tl_siex.lpAttributeList = alloc(breq);
    إذا (!رڪن->tl_siex.lpAttributeList)
	اذهب failed;
    إذا (!pInitializeProcThreadAttributeList(رڪن->tl_siex.lpAttributeList, 1,
								     0, &breq))
	اذهب failed;
    إذا (!pUpdateProcThreadAttribute(
	    رڪن->tl_siex.lpAttributeList, 0,
	    PROC_THREAD_ATTRIBUTE_PSEUDOCONSOLE, رڪن->tl_conpty,
	    حجم(HPCON), عدم, عدم))
	اذهب failed;

    channel = جوڙيو_channel();
    إذا (channel == عدم)
	اذهب failed;

    ڪم = ڪم_alloc();
    إذا (ڪم == عدم)
	اذهب failed;
    إذا (argvar->v_type == VAR_STRING)
    {
	صحيح argc;

	build_argv_from_string(cmd, &ڪم->jv_argv, &argc);
    }
    وإلا
    {
	صحيح argc;

	build_argv_from_list(argvar->vval.v_list, &ڪم->jv_argv, &argc);
    }

    إذا (opt->jo_سيٽ & JO_اندريون_BUF)
	ڪم->jv_اندريون_buf = buflist_findnr(opt->jo_io_buf[PART_اندريون]);

    إذا (!CreateProcessW(عدم, cmd_wchar_copy, عدم, عدم, FALSE,
	    EXTENDED_STARTUPINFO_PRESENT | CREATE_UNICODE_ENVIRONMENT
	    | CREATE_SUSPENDED | CREATE_DEFAULT_ERROR_گهڻائي,
	    env_wchar, cwd_wchar,
	    &رڪن->tl_siex.StartupInfo, &proc_info))
    {
	errmsg = GetWin32Error();
	اذهب failed;
    }

    CloseHandle(i_theirs);
    CloseHandle(o_theirs);

    channel_سيٽ_pipes(channel,
	    (sock_T)i_ours,
	    (sock_T)o_ours,
	    (sock_T)o_ours);

    // Write lines with CR instead of NL.
    channel->ch_write_متن_گهڻائي = TRUE;

    // Use to explicitly delete anonymous pipe handle.
    // In addition, it is used to prevent the pipe from being closed when input
    // from a buffer etc. is finished.
    channel->ch_anonymous_pipe = TRUE;

    jo = Create_ڪم_Object(عدم, عدم);
    إذا (jo == عدم)
	اذهب failed;

    إذا (!سونپيو_Process_منزل_ڪم_Object(jo, proc_info.hProcess))
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

    إذا (create_vterm(رڪن, رڪن->tl_صفون, رڪن->tl_کانن_ڳڻپ) == FAIL)
	اذهب failed;

# إذا defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    إذا (رڪن_use_palette())
    {
	إذا (رڪن->tl_palette != عدم)
	    سيٽ_vterm_palette(رڪن->tl_vterm, رڪن->tl_palette);
	وإلا
	    init_vterm_ansi_colors(رڪن->tl_vterm);
    }
# endif

    channel_سيٽ_ڪم(channel, ڪم, opt);
    ڪم_سيٽ_options(ڪم, opt);

    ڪم->jv_channel = channel;
    ڪم->jv_proc_info = proc_info;
    ڪم->jv_ڪم_object = jo;
    ڪم->jv_حالت = ڪم_STARTED;
    ڪم->jv_tty_type = vim_strsave((char_u *)"conpty");
    ++ڪم->jv_refcount;
    رڪن->tl_ڪم = ڪم;

    // Redirecting stdout and stderr doesn't work at the job level.  Instead
    // open the file here and handle it in.  opt->jo_io was changed in
    // setup_job_options(), use the original flags here.
    إذا (orig_opt->jo_io[PART_ٻاهريون] == JIO_FILE)
    {
	char_u *fname = opt->jo_io_name[PART_ٻاهريون];

	ch_log(channel, "Opening output file %s", fname);
	رڪن->tl_ٻاهريون_fd = mch_fopen((حرف *)fname, WRITEBIN);
	إذا (رڪن->tl_ٻاهريون_fd == عدم)
	    semsg(_(e_cant_open_file_str), fname);
    }

    أرجع OK;

failed:
    ga_clear(&ga_cmd);
    ga_clear(&ga_env);
    vim_free(cmd_wchar);
    vim_free(cmd_wchar_copy);
    vim_free(cwd_wchar);
    إذا (channel != عدم)
	channel_clear(channel);
    إذا (ڪم != عدم)
    {
	ڪم->jv_channel = عدم;
	ڪم_cleanup(ڪم);
    }
    رڪن->tl_ڪم = عدم;
    إذا (jo != عدم)
	CloseHandle(jo);

    إذا (رڪن->tl_siex.lpAttributeList != عدم)
    {
	pDeleteProcThreadAttributeList(رڪن->tl_siex.lpAttributeList);
	vim_free(رڪن->tl_siex.lpAttributeList);
    }
    رڪن->tl_siex.lpAttributeList = عدم;
    إذا (o_theirs != عدم)
	CloseHandle(o_theirs);
    إذا (o_ours != عدم)
	CloseHandle(o_ours);
    إذا (i_ours != عدم)
	CloseHandle(i_ours);
    إذا (i_theirs != عدم)
	CloseHandle(i_theirs);
    إذا (رڪن->tl_conpty != عدم)
	pClosePseudoConsole(رڪن->tl_conpty);
    رڪن->tl_conpty = عدم;
    // Propagate errors that occur in CreateProcess
    إذا (errmsg)
	semsg("CreateProcess failed: %s", errmsg);
    أرجع FAIL;
}

    ساكن فراغ
conpty_رڪن_report_winsize(رڪن_T *رڪن, صحيح صفون_2, صحيح کانن_ڳڻپ)
{
    COORD consize;

    consize.X = کانن_ڳڻپ;
    consize.Y = صفون_2;
    pResizePseudoConsole(رڪن->tl_conpty, consize);
}

    ساكن فراغ
رڪن_free_conpty(رڪن_T *رڪن)
{
    إذا (رڪن->tl_siex.lpAttributeList != عدم)
    {
	pDeleteProcThreadAttributeList(رڪن->tl_siex.lpAttributeList);
	vim_free(رڪن->tl_siex.lpAttributeList);
    }
    رڪن->tl_siex.lpAttributeList = عدم;
    إذا (رڪن->tl_conpty != عدم)
	pClosePseudoConsole(رڪن->tl_conpty);
    رڪن->tl_conpty = عدم;
}

    صحيح
use_conpty(فراغ)
{
    أرجع has_conpty;
}

# define WINPTY_SPAWN_FLAG_AUTO_SHUTDOWN 1ul
# define WINPTY_SPAWN_FLAG_EXIT_AFTER_SHUTDOWN 2ull
# define WINPTY_MOUSE_گهڻائي_FORCE		2

فراغ* (*winpty_config_new)(UINT64, فراغ*);
فراغ* (*winpty_open)(فراغ*, فراغ*);
فراغ* (*winpty_spawn_config_new)(UINT64, فراغ*, LPCWSTR, فراغ*, فراغ*, فراغ*);
BOOL (*winpty_spawn)(فراغ*, فراغ*, HANDLE*, HANDLE*, DWORD*, فراغ*);
فراغ (*winpty_config_سيٽ_mouse_گهڻائي)(فراغ*, صحيح);
فراغ (*winpty_config_سيٽ_initial_سائيز)(فراغ*, صحيح, صحيح);
LPCWSTR (*winpty_conin_name)(فراغ*);
LPCWSTR (*winpty_conout_name)(فراغ*);
LPCWSTR (*winpty_conerr_name)(فراغ*);
فراغ (*winpty_free)(فراغ*);
فراغ (*winpty_config_free)(فراغ*);
فراغ (*winpty_spawn_config_free)(فراغ*);
فراغ (*winpty_error_free)(فراغ*);
LPCWSTR (*winpty_error_نياپو)(فراغ*);
BOOL (*winpty_سيٽ_سائيز)(فراغ*, صحيح, صحيح, فراغ*);
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
    } winpty_داخلا[] =
    {
	{"winpty_conerr_name", (FARPROC*)&winpty_conerr_name},
	{"winpty_config_free", (FARPROC*)&winpty_config_free},
	{"winpty_config_new", (FARPROC*)&winpty_config_new},
	{"winpty_config_set_mouse_mode",
				      (FARPROC*)&winpty_config_سيٽ_mouse_گهڻائي},
	{"winpty_config_set_initial_size",
				    (FARPROC*)&winpty_config_سيٽ_initial_سائيز},
	{"winpty_conin_name", (FARPROC*)&winpty_conin_name},
	{"winpty_conout_name", (FARPROC*)&winpty_conout_name},
	{"winpty_error_free", (FARPROC*)&winpty_error_free},
	{"winpty_free", (FARPROC*)&winpty_free},
	{"winpty_open", (FARPROC*)&winpty_open},
	{"winpty_spawn", (FARPROC*)&winpty_spawn},
	{"winpty_spawn_config_free", (FARPROC*)&winpty_spawn_config_free},
	{"winpty_spawn_config_new", (FARPROC*)&winpty_spawn_config_new},
	{"winpty_error_msg", (FARPROC*)&winpty_error_نياپو},
	{"winpty_set_size", (FARPROC*)&winpty_سيٽ_سائيز},
	{"winpty_agent_process", (FARPROC*)&winpty_agent_process},
	{عدم, عدم}
    };

    // No need to initialize twice.
    إذا (hWinPtyDLL)
	أرجع OK;
    // Load winpty.dll, prefer using the 'winptydll' option, fall back to just
    // winpty.dll.
    إذا (*p_winptydll != NUL)
	hWinPtyDLL = vim_ڀريو_Lib((حرف *)p_winptydll);
    إذا (!hWinPtyDLL)
	hWinPtyDLL = vim_ڀريو_Lib(WINPTY_DLL);
    إذا (!hWinPtyDLL)
    {
	إذا (verbose)
	    semsg(_(e_could_not_ڀريو_library_str_str),
		    (*p_winptydll != NUL ? p_winptydll : (char_u *)WINPTY_DLL),
		    GetWin32Error());
	أرجع FAIL;
    }
    لكل (i = 0; winpty_داخلا[i].name != عدم
					 && winpty_داخلا[i].ptr != عدم; ++i)
    {
	إذا ((*winpty_داخلا[i].ptr = (FARPROC)Get_Proc_پتو(hWinPtyDLL,
					      winpty_داخلا[i].name)) == عدم)
	{
	    إذا (verbose)
		semsg(_(e_could_not_ڀريو_library_function_str), winpty_داخلا[i].name);
	    hWinPtyDLL = عدم;
	    أرجع FAIL;
	}
    }

    أرجع OK;
}

    ساكن صحيح
winpty_رڪن_and_ڪم_init(
	رڪن_T	    *رڪن,
	typval_T    *argvar,
	حرف	    **argv اڻ_استعمال_ٿيل,
	jobopt_T    *opt,
	jobopt_T    *orig_opt)
{
    WCHAR	    *cmd_wchar = عدم;
    WCHAR	    *cwd_wchar = عدم;
    WCHAR	    *env_wchar = عدم;
    channel_T	    *channel = عدم;
    ڪم_T	    *ڪم = عدم;
    DWORD	    error;
    HANDLE	    jo = عدم;
    HANDLE	    ٻار_process_handle;
    HANDLE	    ٻار_thread_handle;
    فراغ	    *winpty_err = عدم;
    فراغ	    *spawn_config = عدم;
    garray_T	    ga_cmd, ga_env;
    char_u	    *cmd = عدم;

    ga_init2(&ga_cmd, حجم(حرف*), 20);
    ga_init2(&ga_env, حجم(حرف*), 20);

    إذا (argvar->v_type == VAR_STRING)
    {
	cmd = argvar->vval.v_string;
    }
    وإلا إذا (argvar->v_type == VAR_LIST)
    {
	إذا (win32_build_cmd(argvar->vval.v_list, &ga_cmd) == FAIL)
	    اذهب failed;
	cmd = ga_cmd.ga_معلومات;
    }
    إذا (cmd == عدم || *cmd == NUL)
    {
	emsg(_(e_invalid_دليل));
	اذهب failed;
    }

    رڪن->tl_arg0_cmd = vim_strsave(cmd);

    cmd_wchar = enc_منزل_utf16(cmd, عدم);
    ga_clear(&ga_cmd);
    إذا (cmd_wchar == عدم)
	اذهب failed;
    إذا (opt->jo_cwd != عدم)
    {
	cwd_wchar = enc_منزل_utf16(opt->jo_cwd, عدم);
	إذا (cwd_wchar == عدم)
	    اذهب failed;
    }

    win32_build_env(opt->jo_env, &ga_env, TRUE);
    env_wchar = ga_env.ga_معلومات;

    رڪن->tl_winpty_config = winpty_config_new(0, &winpty_err);
    إذا (رڪن->tl_winpty_config == عدم)
	اذهب failed;

    winpty_config_سيٽ_mouse_گهڻائي(رڪن->tl_winpty_config,
						    WINPTY_MOUSE_گهڻائي_FORCE);
    winpty_config_سيٽ_initial_سائيز(رڪن->tl_winpty_config,
						 رڪن->tl_کانن_ڳڻپ, رڪن->tl_صفون);
    رڪن->tl_winpty = winpty_open(رڪن->tl_winpty_config, &winpty_err);
    إذا (رڪن->tl_winpty == عدم)
	اذهب failed;

    spawn_config = winpty_spawn_config_new(
	    WINPTY_SPAWN_FLAG_AUTO_SHUTDOWN |
		WINPTY_SPAWN_FLAG_EXIT_AFTER_SHUTDOWN,
	    عدم,
	    cmd_wchar,
	    cwd_wchar,
	    env_wchar,
	    &winpty_err);
    إذا (spawn_config == عدم)
	اذهب failed;

    channel = جوڙيو_channel();
    إذا (channel == عدم)
	اذهب failed;

    ڪم = ڪم_alloc();
    إذا (ڪم == عدم)
	اذهب failed;
    إذا (argvar->v_type == VAR_STRING)
    {
	صحيح argc;

	build_argv_from_string(cmd, &ڪم->jv_argv, &argc);
    }
    وإلا
    {
	صحيح argc;

	build_argv_from_list(argvar->vval.v_list, &ڪم->jv_argv, &argc);
    }

    إذا (opt->jo_سيٽ & JO_اندريون_BUF)
	ڪم->jv_اندريون_buf = buflist_findnr(opt->jo_io_buf[PART_اندريون]);

    إذا (!winpty_spawn(رڪن->tl_winpty, spawn_config, &ٻار_process_handle,
	    &ٻار_thread_handle, &error, &winpty_err))
	اذهب failed;

    channel_سيٽ_pipes(channel,
	(sock_T)CreateFileW(
	    winpty_conin_name(رڪن->tl_winpty),
	    GENERIC_WRITE, 0, عدم,
	    OPEN_EXISTING, 0, عدم),
	(sock_T)CreateFileW(
	    winpty_conout_name(رڪن->tl_winpty),
	    GENERIC_READ, 0, عدم,
	    OPEN_EXISTING, 0, عدم),
	(sock_T)CreateFileW(
	    winpty_conerr_name(رڪن->tl_winpty),
	    GENERIC_READ, 0, عدم,
	    OPEN_EXISTING, 0, عدم));

    // Write lines with CR instead of NL.
    channel->ch_write_متن_گهڻائي = TRUE;

    jo = Create_ڪم_Object(عدم, عدم);
    إذا (jo == عدم)
	اذهب failed;

    إذا (!سونپيو_Process_منزل_ڪم_Object(jo, ٻار_process_handle))
    {
	// Failed, switch the way to terminate process with TerminateProcess.
	CloseHandle(jo);
	jo = عدم;
    }

    winpty_spawn_config_free(spawn_config);
    vim_free(cmd_wchar);
    vim_free(cwd_wchar);
    vim_free(env_wchar);

    إذا (create_vterm(رڪن, رڪن->tl_صفون, رڪن->tl_کانن_ڳڻپ) == FAIL)
	اذهب failed;

# إذا defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    إذا (رڪن_use_palette())
    {
	إذا (رڪن->tl_palette != عدم)
	    سيٽ_vterm_palette(رڪن->tl_vterm, رڪن->tl_palette);
	وإلا
	    init_vterm_ansi_colors(رڪن->tl_vterm);
    }
# endif

    channel_سيٽ_ڪم(channel, ڪم, opt);
    ڪم_سيٽ_options(ڪم, opt);

    ڪم->jv_channel = channel;
    ڪم->jv_proc_info.hProcess = ٻار_process_handle;
    ڪم->jv_proc_info.dwProcessId = GetProcessId(ٻار_process_handle);
    ڪم->jv_ڪم_object = jo;
    ڪم->jv_حالت = ڪم_STARTED;
    ڪم->jv_tty_اندريون = utf16_منزل_enc(
	    (short_u *)winpty_conin_name(رڪن->tl_winpty), عدم);
    ڪم->jv_tty_ٻاهريون = utf16_منزل_enc(
	    (short_u *)winpty_conout_name(رڪن->tl_winpty), عدم);
    ڪم->jv_tty_type = vim_strsave((char_u *)"winpty");
    ++ڪم->jv_refcount;
    رڪن->tl_ڪم = ڪم;

    // Redirecting stdout and stderr doesn't work at the job level.  Instead
    // open the file here and handle it in.  opt->jo_io was changed in
    // setup_job_options(), use the original flags here.
    إذا (orig_opt->jo_io[PART_ٻاهريون] == JIO_FILE)
    {
	char_u *fname = opt->jo_io_name[PART_ٻاهريون];

	ch_log(channel, "Opening output file %s", fname);
	رڪن->tl_ٻاهريون_fd = mch_fopen((حرف *)fname, WRITEBIN);
	إذا (رڪن->tl_ٻاهريون_fd == عدم)
	    semsg(_(e_cant_open_file_str), fname);
    }

    أرجع OK;

failed:
    ga_clear(&ga_cmd);
    ga_clear(&ga_env);
    vim_free(cmd_wchar);
    vim_free(cwd_wchar);
    إذا (spawn_config != عدم)
	winpty_spawn_config_free(spawn_config);
    إذا (channel != عدم)
	channel_clear(channel);
    إذا (ڪم != عدم)
    {
	ڪم->jv_channel = عدم;
	ڪم_cleanup(ڪم);
    }
    رڪن->tl_ڪم = عدم;
    إذا (jo != عدم)
	CloseHandle(jo);
    إذا (رڪن->tl_winpty != عدم)
	winpty_free(رڪن->tl_winpty);
    رڪن->tl_winpty = عدم;
    إذا (رڪن->tl_winpty_config != عدم)
	winpty_config_free(رڪن->tl_winpty_config);
    رڪن->tl_winpty_config = عدم;
    إذا (winpty_err != عدم)
    {
	حرف *نياپو = (حرف *)utf16_منزل_enc(
				(short_u *)winpty_error_نياپو(winpty_err), عدم);

	إذا (نياپو != عدم)
	{
	    emsg(نياپو);
	    vim_free(نياپو);
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
رڪن_and_ڪم_init(
	رڪن_T	    *رڪن,
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

    إذا (!has_winpty && !has_conpty)
	// If neither is available give the errors for winpty, since when
	// conpty is not available it can't be installed either.
	أرجع dyn_winpty_init(TRUE);

    إذا (opt->jo_tty_type != NUL)
	tty_type = opt->jo_tty_type;

    إذا (tty_type == NUL)
    {
	إذا (has_conpty && (is_conpty_stable() || !has_winpty))
	    use_conpty = TRUE;
	وإلا إذا (has_winpty)
	    use_winpty = TRUE;
	// else: error
    }
    وإلا إذا (tty_type == 'w')	// winpty
    {
	إذا (has_winpty)
	    use_winpty = TRUE;
    }
    وإلا إذا (tty_type == 'c')	// conpty
    {
	إذا (has_conpty)
	    use_conpty = TRUE;
	وإلا
	    أرجع dyn_conpty_init(TRUE);
    }

    إذا (use_conpty)
	أرجع conpty_رڪن_and_ڪم_init(رڪن, argvar, argv, opt, orig_opt);

    إذا (use_winpty)
	أرجع winpty_رڪن_and_ڪم_init(رڪن, argvar, argv, opt, orig_opt);

    // error
    أرجع dyn_winpty_init(TRUE);
}

    ساكن صحيح
create_pty_only(رڪن_T *رڪن, jobopt_T *options)
{
    HANDLE	    h_Pipe_اندريون = INVALID_HANDLE_قدر;
    HANDLE	    h_Pipe_ٻاهريون = INVALID_HANDLE_قدر;
    حرف	    اندريون_name[80], ٻاهريون_name[80];
    channel_T	    *channel = عدم;

    إذا (create_vterm(رڪن, رڪن->tl_صفون, رڪن->tl_کانن_ڳڻپ) == FAIL)
	أرجع FAIL;

    vim_snprintf(اندريون_name, حجم(اندريون_name), "\\\\.\\pipe\\vim-%d-in-%d",
	    GetCurrentProcessId(),
	    curbuf->b_fnum);
    h_Pipe_اندريون = CreateNamedPipe(اندريون_name, PIPE_ACCESS_OUTBOUND,
	    PIPE_TYPE_پيغام | PIPE_NOWAIT,
	    PIPE_UNLIMITED_INSTANCES,
	    0, 0, NMPWAIT_NOWAIT, عدم);
    إذا (h_Pipe_اندريون == INVALID_HANDLE_قدر)
	اذهب failed;

    vim_snprintf(ٻاهريون_name, حجم(ٻاهريون_name), "\\\\.\\pipe\\vim-%d-out-%d",
	    GetCurrentProcessId(),
	    curbuf->b_fnum);
    h_Pipe_ٻاهريون = CreateNamedPipe(ٻاهريون_name, PIPE_ACCESS_INBOUND,
	    PIPE_TYPE_پيغام | PIPE_NOWAIT,
	    PIPE_UNLIMITED_INSTANCES,
	    0, 0, 0, عدم);
    إذا (h_Pipe_ٻاهريون == INVALID_HANDLE_قدر)
	اذهب failed;

    ConnectNamedPipe(h_Pipe_اندريون, عدم);
    ConnectNamedPipe(h_Pipe_ٻاهريون, عدم);

    رڪن->tl_ڪم = ڪم_alloc();
    إذا (رڪن->tl_ڪم == عدم)
	اذهب failed;
    ++رڪن->tl_ڪم->jv_refcount;

    // behave like the job is already finished
    رڪن->tl_ڪم->jv_حالت = ڪم_ختم;

    channel = جوڙيو_channel();
    إذا (channel == عدم)
	اذهب failed;
    رڪن->tl_ڪم->jv_channel = channel;
    channel->ch_keep_open = TRUE;
    channel->ch_named_pipe = TRUE;

    channel_سيٽ_pipes(channel,
	(sock_T)h_Pipe_اندريون,
	(sock_T)h_Pipe_ٻاهريون,
	(sock_T)h_Pipe_ٻاهريون);
    channel_سيٽ_ڪم(channel, رڪن->tl_ڪم, options);
    رڪن->tl_ڪم->jv_tty_اندريون = vim_strsave((char_u*)اندريون_name);
    رڪن->tl_ڪم->jv_tty_ٻاهريون = vim_strsave((char_u*)ٻاهريون_name);

    أرجع OK;

failed:
    إذا (h_Pipe_اندريون != عدم)
	CloseHandle(h_Pipe_اندريون);
    إذا (h_Pipe_ٻاهريون != عدم)
	CloseHandle(h_Pipe_ٻاهريون);
    أرجع FAIL;
}

/*
 * Free the terminal emulator part of "term".
 */
    ساكن فراغ
رڪن_free_vterm(رڪن_T *رڪن)
{
    رڪن_free_conpty(رڪن);
    إذا (رڪن->tl_winpty != عدم)
	winpty_free(رڪن->tl_winpty);
    رڪن->tl_winpty = عدم;
    إذا (رڪن->tl_winpty_config != عدم)
	winpty_config_free(رڪن->tl_winpty_config);
    رڪن->tl_winpty_config = عدم;
    إذا (رڪن->tl_vterm != عدم)
	vterm_free(رڪن->tl_vterm);
    رڪن->tl_vterm = عدم;
}

/*
 * Report the size to the terminal.
 */
    ساكن فراغ
رڪن_report_winsize(رڪن_T *رڪن, صحيح صفون_2, صحيح کانن_ڳڻپ)
{
    إذا (رڪن->tl_conpty)
	conpty_رڪن_report_winsize(رڪن, صفون_2, کانن_ڳڻپ);
    إذا (رڪن->tl_winpty)
	winpty_سيٽ_سائيز(رڪن->tl_winpty, کانن_ڳڻپ, صفون_2, عدم);
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
رڪن_and_ڪم_init(
	رڪن_T	    *رڪن,
	typval_T    *argvar,
	حرف	    **argv,
	jobopt_T    *opt,
	jobopt_T    *orig_opt اڻ_استعمال_ٿيل)
{
    رڪن->tl_arg0_cmd = عدم;

    إذا (create_vterm(رڪن, رڪن->tl_صفون, رڪن->tl_کانن_ڳڻپ) == FAIL)
	أرجع FAIL;

# إذا defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    إذا (رڪن_use_palette())
    {
	إذا (رڪن->tl_palette != عدم)
	    سيٽ_vterm_palette(رڪن->tl_vterm, رڪن->tl_palette);
	وإلا
	    init_vterm_ansi_colors(رڪن->tl_vterm);
    }
# endif

    // This may change a string in "argvar".
    رڪن->tl_ڪم = ڪم_شروعات(argvar, argv, opt, &رڪن->tl_ڪم);
    إذا (رڪن->tl_ڪم != عدم)
	++رڪن->tl_ڪم->jv_refcount;

    أرجع رڪن->tl_ڪم != عدم
	&& رڪن->tl_ڪم->jv_channel != عدم
	&& رڪن->tl_ڪم->jv_حالت != ڪم_FAILED ? OK : FAIL;
}

    ساكن صحيح
create_pty_only(رڪن_T *رڪن, jobopt_T *opt)
{
    إذا (create_vterm(رڪن, رڪن->tl_صفون, رڪن->tl_کانن_ڳڻپ) == FAIL)
	أرجع FAIL;

    رڪن->tl_ڪم = ڪم_alloc();
    إذا (رڪن->tl_ڪم == عدم)
	أرجع FAIL;
    ++رڪن->tl_ڪم->jv_refcount;

    // behave like the job is already finished
    رڪن->tl_ڪم->jv_حالت = ڪم_ختم;

    أرجع mch_create_pty_channel(رڪن->tl_ڪم, opt);
}

/*
 * Free the terminal emulator part of "term".
 */
    ساكن فراغ
رڪن_free_vterm(رڪن_T *رڪن)
{
    إذا (رڪن->tl_vterm != عدم)
	vterm_free(رڪن->tl_vterm);
    رڪن->tl_vterm = عدم;
}

/*
 * Report the size to the terminal.
 */
    ساكن فراغ
رڪن_report_winsize(رڪن_T *رڪن, صحيح صفون_2, صحيح کانن_ڳڻپ)
{
    // Use an ioctl() to report the new window size to the job.
    إذا (رڪن->tl_ڪم == عدم || رڪن->tl_ڪم->jv_channel == عدم)
	أرجع;

    صحيح fd = -1;
    صحيح part;

    لكل (part = PART_ٻاهريون; part < PART_ڳڻپ; ++part)
    {
	fd = رڪن->tl_ڪم->jv_channel->ch_part[part].ch_fd;
	إذا (mch_isatty(fd))
	    اكسر;
    }
    إذا (part < PART_ڳڻپ && mch_report_winsize(fd, صفون_2, کانن_ڳڻپ) == OK)
	mch_signal_ڪم(رڪن->tl_ڪم, (char_u *)"winch");
}

#جيڪڏهن_پورو

#جيڪڏهن_پورو // FEAT_TERMINAL
