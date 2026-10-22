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

#جے_بنا_تعریف MIN
# define MIN(x,y) ((x) < (y) ? (x) : (y))
#جے_انت
#جے_بنا_تعریف MAX
# define MAX(x,y) ((x) > (y) ? (x) : (y))
#جے_انت

#ضمن "libvterm/include/vterm.h"

// This is VTermScreenCell without the characters, thus much smaller.
تعريفنوع بنية {
    VTermScreenCellAttrs	attrs;
    حرف			چوڑائی;
    VTermColor			fg;
    VTermColor			bg;
} cellattr_T;

تعريفنوع بنية sb_سطر_S {
    صحيح		sb_خانے_گنتی;	// can differ per line
    صحيح		sb_بائٹاں;	// length in bytes of text
    cellattr_T	*sb_cells;	// allocated
    cellattr_T	sb_fill_attr;	// for short line
    char_u	*sb_متن;	// for tl_scrollback_postponed
    char_u	continuation;
} sb_سطر_T;

#جے_تعریف_شدہ MSWIN
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
#جے_انت

// typedef term_T in structs.h
بنية terminal_S {
    رکن_T	*tl_اگلا;

    VTerm	*tl_vterm;
    کم_T	*tl_کم;
    buf_T	*tl_بفر;
#إذا defined(FEAT_GUI)
    صحيح		tl_system;	// when non-zero used for :!cmd output
    صحيح		tl_toprow;	// row with first line of system terminal
#جے_انت

    // Set when setting the size of a vterm, reset after redrawing.
    صحيح		tl_vterm_ناپ_بدلیا;

    صحيح		tl_عام_منوال; // TRUE: Terminal-Normal mode
    صحيح		tl_channel_closing;
    صحيح		tl_channel_closed;
    صحيح		tl_channel_recently_closed; // still need to handle tl_finish

    صحيح		tl_انت;
#تعریف_کرو TL_انت_UNSET	    NUL
#تعریف_کرو TL_انت_CLOSE	    'c'	// ++close or :terminal without argument
#تعریف_کرو TL_انت_NOCLOSE   'n'	// ++noclose
#تعریف_کرو TL_انت_OPEN	    'o'	// ++open
    char_u	*tl_opencmd;
    char_u	*tl_eof_chars;
    char_u	*tl_api;	// prefix for terminal API function

    char_u	*tl_arg0_cmd;	// To format the status bar

#جے_تعریف_شدہ MSWIN
    فراغ	*tl_winpty_config;
    فراغ	*tl_winpty;

    HPCON	tl_conpty;
    DYN_STARTUPINFOEXW tl_siex;	// Structure that always needs to be hold

    FILE	*tl_باہرلا_fd;
#جے_انت
#إذا defined(FEAT_SESSION)
    char_u	*tl_command;
#جے_انت
    char_u	*tl_kill;

    // last known vterm size
    صحيح		tl_صفاں;
    صحيح		tl_خانے_گنتی;

    char_u	*tl_title; // NULL or allocated
    char_u	*tl_حالت_متن; // NULL or allocated

    // Range of screen rows to update.  Zero based.
    صحيح		tl_dirty_صف_شروع; // MAX_ROW if nothing dirty
    صحيح		tl_dirty_صف_سرا;   // row below last one to update
    صحيح		tl_dirty_snapshot;  // text updated after making snapshot
#جے_تعریف_شدہ FEAT_TIMERS
    صحيح		tl_timer_سیٹ;
    proftime_T	tl_timer_due;
#جے_انت
    صحيح		tl_postponed_scroll;	// to be scrolled up

    garray_T	tl_scrollback;
    صحيح		tl_scrollback_scrolled;
    garray_T	tl_scrollback_postponed;
    صحيح		tl_scrollback_snapshot;
    صحيح		tl_بفر_scrolled;

    char_u	*tl_highlight_name; // replaces "Terminal"; allocated

    cellattr_T	tl_default_color;

    linenr_T	tl_چوٹی_diff_صفاں;   // rows of top diff file or zero
    linenr_T	tl_bot_diff_صفاں;   // rows of bottom diff file

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

#تعریف_کرو TMODE_ONCE 1	    // CTRL-\ CTRL-N used
#تعریف_کرو TMODE_LOOP 2	    // CTRL-W N used

/*
 * List of all active terminals.
 */
ساكن رکن_T *پہلا_رکن = عدم;

// Terminal active in terminal_loop().
ساكن رکن_T *وچ_terminal_loop = عدم;

#جے_تعریف_شدہ MSWIN
ساكن BOOL has_winpty = FALSE;
ساكن BOOL has_conpty = FALSE;
#جے_انت

#تعریف_کرو MAX_صف 999999	    // used for tl_dirty_row_end to update all rows
#تعریف_کرو کلید_BUF_LEN 200

#تعریف_کرو FOR_ALL_TERMS(رکن)	\
    لكل ((رکن) = پہلا_رکن; (رکن) != عدم; (رکن) = (رکن)->tl_اگلا)

/*
 * Functions with separate implementation for MS-Windows and Unix-like systems.
 */
ساكن صحيح رکن_and_کم_init(رکن_T *رکن, typval_T *argvar, حرف **argv, jobopt_T *opt, jobopt_T *orig_opt);
ساكن صحيح create_pty_only(رکن_T *رکن, jobopt_T *opt);
ساكن فراغ رکن_report_winsize(رکن_T *رکن, صحيح صفاں_2, صحيح خانے_گنتی);
ساكن فراغ رکن_free_vterm(رکن_T *رکن);
#جے_تعریف_شدہ FEAT_GUI
ساكن فراغ update_system_رکن(رکن_T *رکن);
#جے_انت

ساكن فراغ handle_postponed_scrollback(رکن_T *رکن);

// The character that we know (or assume) that the terminal expects for the
// backspace key.
ساكن صحيح رکن_backspace_char = BS;

// Store the last set and the desired cursor properties, so that we only update
// them when needed.  Doing it unnecessary may result in flicker.
ساكن char_u	*آخری_سیٹ_cursor_color = عدم;
ساكن char_u	*desired_cursor_color = عدم;
ساكن صحيح	آخری_سیٹ_cursor_shape = -1;
ساكن صحيح	desired_cursor_shape = -1;
ساكن صحيح	آخری_سیٹ_cursor_blink = -1;
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
parse_termwinsize(win_T *wp, صحيح *صفاں_2, صحيح *خانے_گنتی)
{
    صحيح	minsize = FALSE;

    *صفاں_2 = 0;
    *خانے_گنتی = 0;

    إذا (*wp->w_p_tws == NUL)
	أرجع FALSE;

    char_u *p = vim_strchr(wp->w_p_tws, 'x');

    // Syntax of value was already checked when it's set.
    إذا (p == عدم)
    {
	minsize = TRUE;
	p = vim_strchr(wp->w_p_tws, '*');
    }
    *صفاں_2 = atoi((حرف *)wp->w_p_tws);
    *خانے_گنتی = atoi((حرف *)p + 1);
    إذا (*صفاں_2 > VTERM_MAX_صفاں)
	*صفاں_2 = VTERM_MAX_صفاں;
    إذا (*خانے_گنتی > VTERM_MAX_خانے_گنتی)
	*خانے_گنتی = VTERM_MAX_خانے_گنتی;
    أرجع minsize;
}

/*
 * Determine the terminal size from 'termwinsize' and the current window.
 */
    ساكن فراغ
سیٹ_رکن_and_win_ناپ(رکن_T *رکن, jobopt_T *opt)
{
    صحيح صفاں_2, خانے_گنتی;
    صحيح	minsize;

#جے_تعریف_شدہ FEAT_GUI
    إذا (رکن->tl_system)
    {
	// Use the entire screen (excluding the tabpanel area) for the system
	// commands.  However, it will start at the command line and scroll up
	// as needed, using tl_toprow.
	رکن->tl_صفاں = صفاں;
	رکن->tl_خانے_گنتی = topframe->fr_چوڑائی;
	أرجع;
    }
#جے_انت
    رکن->tl_صفاں = curwin->w_اچائی;
    رکن->tl_خانے_گنتی = curwin->w_چوڑائی;

    minsize = parse_termwinsize(curwin, &صفاں_2, &خانے_گنتی);
    إذا (minsize)
    {
	إذا (رکن->tl_صفاں < صفاں_2)
	    رکن->tl_صفاں = صفاں_2;
	إذا (رکن->tl_خانے_گنتی < خانے_گنتی)
	    رکن->tl_خانے_گنتی = خانے_گنتی;
    }
    إذا ((opt->jo_set2 & JO2_رکن_صفاں))
	رکن->tl_صفاں = opt->jo_رکن_صفاں;
    وإلا إذا (صفاں_2 != 0)
	رکن->tl_صفاں = صفاں_2;
    إذا ((opt->jo_set2 & JO2_رکن_خانے_گنتی))
	رکن->tl_خانے_گنتی = opt->jo_رکن_خانے_گنتی;
    وإلا إذا (خانے_گنتی != 0)
	رکن->tl_خانے_گنتی = خانے_گنتی;

    إذا (!opt->jo_hidden)
    {
	إذا (رکن->tl_صفاں != curwin->w_اچائی)
	    win_setheight_win(رکن->tl_صفاں, curwin);
	إذا (رکن->tl_خانے_گنتی != curwin->w_چوڑائی)
	    win_setwidth_win(رکن->tl_خانے_گنتی, curwin);

	// Set 'winsize' now to avoid a resize at the next redraw.
	إذا (!minsize && *curwin->w_p_tws != NUL)
	{
	    char_u buf[100];

	    vim_snprintf((حرف *)buf, 100, "%dx%d",
						 رکن->tl_صفاں, رکن->tl_خانے_گنتی);
	    سیٹ_option_قدر_give_err((char_u *)"termwinsize",
							   0L, buf, OPT_LOCAL);
	}
    }
}

/*
 * Initialize job options for a terminal job.
 * Caller may overrule some of them.
 */
    فراغ
init_کم_options(jobopt_T *opt)
{
    clear_کم_options(opt);

    opt->jo_منوال = CH_منوال_RAW;
    opt->jo_باہرلا_منوال = CH_منوال_RAW;
    opt->jo_err_منوال = CH_منوال_RAW;
    opt->jo_سیٹ = JO_منوال | JO_باہرلا_منوال | JO_ERR_منوال;
}

/*
 * Set job options mandatory for a terminal job.
 */
    ساكن فراغ
تیاری_کم_options(jobopt_T *opt, صحيح صفاں_2, صحيح خانے_گنتی)
{
#جے_بنا_تعریف MSWIN
    // Win32: Redirecting the job output won't work, thus always connect stdout
    // here.
    إذا (!(opt->jo_سیٹ & JO_باہرلا_IO))
#جے_انت
    {
	// Connect stdout to the terminal.
	opt->jo_io[PART_باہرلا] = JIO_بفر;
	opt->jo_io_buf[PART_باہرلا] = curbuf->b_fnum;
	opt->jo_modifiable[PART_باہرلا] = 0;
	opt->jo_سیٹ |= JO_باہرلا_IO + JO_باہرلا_BUF + JO_باہرلا_MODIFIABLE;
    }

#جے_بنا_تعریف MSWIN
    // Win32: Redirecting the job output won't work, thus always connect stderr
    // here.
    إذا (!(opt->jo_سیٹ & JO_ERR_IO))
#جے_انت
    {
	// Connect stderr to the terminal.
	opt->jo_io[PART_ERR] = JIO_بفر;
	opt->jo_io_buf[PART_ERR] = curbuf->b_fnum;
	opt->jo_modifiable[PART_ERR] = 0;
	opt->jo_سیٹ |= JO_ERR_IO + JO_ERR_BUF + JO_ERR_MODIFIABLE;
    }

    opt->jo_pty = TRUE;
    إذا ((opt->jo_set2 & JO2_رکن_صفاں) == 0)
	opt->jo_رکن_صفاں = صفاں_2;
    إذا ((opt->jo_set2 & JO2_رکن_خانے_گنتی) == 0)
	opt->jo_رکن_خانے_گنتی = خانے_گنتی;
}

/*
 * Flush messages on channels.
 */
    ساكن فراغ
رکن_flush_messages(فراغ)
{
    mch_check_messages();
    parse_queued_messages();
}

/*
 * Close a terminal buffer (and its window).  Used when creating the terminal
 * fails.
 */
    ساكن فراغ
رکن_close_بفر(buf_T *buf, buf_T *old_curbuf)
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
    do_بفر(DOBUF_WIPE, DOBUF_پہلا, FORWARD, buf->b_fnum, TRUE);
}

/*
 * Start a terminal window and return its buffer.
 * Use either "argvar" or "argv", the other must be NULL.
 * When "flags" has TERM_START_NOJOB only create the buffer, b_term and open
 * the window.
 * Returns NULL when failed.
 */
    buf_T *
رکن_شروع(
	typval_T    *argvar,
	حرف	    **argv,
	jobopt_T    *opt,
	صحيح	    flags)
{
    exarg_T	ونڈو_ea;
    win_T	*old_curwin = curwin;
    رکن_T	*رکن;
    buf_T	*old_curbuf = عدم;
    صحيح		res;
    buf_T	*newbuf;
    صحيح		vertical = opt->jo_vertical || (cmdmod.cmod_ونڈو & WSP_VERT);
    jobopt_T	orig_opt;  // only partly filled
    pos_T	save_cursor;

    إذا (check_restricted() || check_secure())
	أرجع عدم;
    إذا (cmdwin_type != 0)
    {
	emsg(_(e_cannot_open_terminal_from_command_سطر_کھڑکی));
	أرجع عدم;
    }

    إذا ((opt->jo_سیٹ & (JO_وچ_IO + JO_باہرلا_IO + JO_ERR_IO))
					 == (JO_وچ_IO + JO_باہرلا_IO + JO_ERR_IO)
	|| (!(opt->jo_سیٹ & JO_باہرلا_IO) && (opt->jo_سیٹ & JO_باہرلا_BUF))
	|| (!(opt->jo_سیٹ & JO_ERR_IO) && (opt->jo_سیٹ & JO_ERR_BUF))
	|| (argvar != عدم
	    && argvar->v_type == VAR_LIST
	    && argvar->vval.v_list != عدم
	    && argvar->vval.v_list->lv_پہلا == &range_list_شے))
    {
	emsg(_(e_invalid_دلیل));
	أرجع عدم;
    }

    رکن = ALLOC_CLEAR_ONE(رکن_T);
    إذا (رکن == عدم)
	أرجع عدم;
    رکن->tl_dirty_صف_سرا = MAX_صف;
    رکن->tl_cursor_visible = TRUE;
    رکن->tl_cursor_shape = VTERM_PROP_CURSORSHAPE_بلاک;
    رکن->tl_انت = opt->jo_رکن_انت;
#جے_تعریف_شدہ FEAT_GUI
    رکن->tl_system = (flags & رکن_شروع_SYSTEM);
#جے_انت
    ga_init2(&رکن->tl_scrollback, حجم(sb_سطر_T), 300);
    ga_init2(&رکن->tl_scrollback_postponed, حجم(sb_سطر_T), 300);
    ga_init2(&رکن->tl_osc_buf, حجم(حرف), 300);

    setpcmark();
    CLEAR_میدان(ونڈو_ea);
    إذا (opt->jo_curwin)
    {
	// Create a new buffer in the current window.
	إذا (!can_abandon(curbuf, flags & رکن_شروع_FORCEIT))
	{
	    no_write_سنیہا();
	    vim_free(رکن);
	    أرجع عدم;
	}
	إذا (do_ecmd(0, عدم, عدم, &ونڈو_ea, ECMD_ONE,
		      (buf_hide(curwin->w_بفر) ? ECMD_HIDE : 0)
			  + ((flags & رکن_شروع_FORCEIT) ? ECMD_FORCEIT : 0),
							       curwin) == FAIL)
	{
	    vim_free(رکن);
	    أرجع عدم;
	}
    }
    وإلا إذا (opt->jo_hidden || (flags & رکن_شروع_SYSTEM))
    {
	buf_T *buf;

	// Create a new buffer without a window. Make it the current buffer for
	// a moment to be able to do the initializations.
	buf = buflist_new((char_u *)"", عدم, (linenr_T)0,
							 BLN_NEW | BLN_LISTED);
	إذا (buf == عدم || ml_open(buf) == FAIL)
	{
	    vim_free(رکن);
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
	ونڈو_ea.cmdidx = CMD_new;
	ونڈو_ea.cmd = (char_u *)"new";
	ونڈو_ea.arg = (char_u *)"";
	إذا (opt->jo_رکن_صفاں > 0 && !vertical)
	{
	    ونڈو_ea.line2 = opt->jo_رکن_صفاں;
	    ونڈو_ea.addr_گنتی = 1;
	}
	إذا (opt->jo_رکن_خانے_گنتی > 0 && vertical)
	{
	    ونڈو_ea.line2 = opt->jo_رکن_خانے_گنتی;
	    ونڈو_ea.addr_گنتی = 1;
	}

	صحيح cmod_ونڈو_modified = FALSE;
	إذا (vertical)
	{
	    إذا (!(cmdmod.cmod_ونڈو & WSP_VERT))
		cmod_ونڈو_modified = TRUE;
	    cmdmod.cmod_ونڈو |= WSP_VERT;
	}
	ex_splitview(&ونڈو_ea);
	إذا (cmod_ونڈو_modified)
	    cmdmod.cmod_ونڈو &= ~WSP_VERT;
	إذا (curwin == old_curwin)
	{
	    // split failed
	    vim_free(رکن);
	    أرجع عدم;
	}
    }
    رکن->tl_بفر = curbuf;
    curbuf->b_رکن = رکن;

    إذا (!opt->jo_hidden)
    {
	// Only one size was taken care of with :new, do the other one.  With
	// "curwin" both need to be done.
	إذا (opt->jo_رکن_صفاں > 0 && (opt->jo_curwin || vertical))
	    win_setheight(opt->jo_رکن_صفاں);
	إذا (opt->jo_رکن_خانے_گنتی > 0 && (opt->jo_curwin || !vertical))
	    win_setwidth(opt->jo_رکن_خانے_گنتی);
    }

    // Link the new terminal in the list of active terminals.
    رکن->tl_اگلا = پہلا_رکن;
    پہلا_رکن = رکن;

    apply_autocmds(EVENT_BUFFILEPRE, عدم, عدم, FALSE, curbuf);

    إذا (opt->jo_رکن_name != عدم)
    {
	vim_free(curbuf->b_ffname);
	curbuf->b_ffname = vim_strsave(opt->jo_رکن_name);
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
			       &argvar->vval.v_list->lv_پہلا->li_tv)) == عدم)
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

    إذا (opt->jo_رکن_opencmd != عدم)
	رکن->tl_opencmd = vim_strsave(opt->jo_رکن_opencmd);

    إذا (opt->jo_eof_chars != عدم)
	رکن->tl_eof_chars = vim_strsave(opt->jo_eof_chars);

    سیٹ_string_option_سدھا((char_u *)"buftype", -1,
				  (char_u *)"terminal", OPT_FREE|OPT_LOCAL, 0);
    // Avoid that 'buftype' is reset when this buffer is entered.
    curbuf->b_p_initialized = true;

    // Mark the buffer as not modifiable. It can only be made modifiable after
    // the job finished.
    curbuf->b_p_ma = FALSE;

    سیٹ_رکن_and_win_ناپ(رکن, opt);
#جے_تعریف_شدہ MSWIN
    mch_memmove(orig_opt.jo_io, opt->jo_io, حجم(orig_opt.jo_io));
#جے_انت
    تیاری_کم_options(opt, رکن->tl_صفاں, رکن->tl_خانے_گنتی);

    إذا (flags & رکن_شروع_NOJOB)
	أرجع curbuf;

#إذا defined(FEAT_SESSION)
    // Remember the command for the session file.
    إذا (opt->jo_رکن_norestore || argv != عدم)
	رکن->tl_command = vim_strsave((char_u *)"NONE");
    وإلا إذا (argvar->v_type == VAR_STRING)
    {
	char_u	*cmd = argvar->vval.v_string;

	إذا (cmd != عدم && STRCMP(cmd, p_sh) != 0)
	    رکن->tl_command = vim_strsave(cmd);
    }
    وإلا إذا (argvar->v_type == VAR_LIST
	    && argvar->vval.v_list != عدم
	    && argvar->vval.v_list->lv_len > 0)
    {
	garray_T	ga;
	listitem_T	*شے;

	ga_init2(&ga, 1, 100);
	FOR_ALL_LIST_اشیاء(argvar->vval.v_list, شے)
	{
	    char_u *s = tv_get_string_chk(&شے->li_tv);
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
	إذا (شے == عدم)
	{
	    ga_append(&ga, NUL);
	    رکن->tl_command = ga.ga_انکڑے;
	}
	وإلا
	    ga_clear(&ga);
    }
#جے_انت

    إذا (opt->jo_رکن_kill != عدم)
    {
	char_u *p = skiptowhite(opt->jo_رکن_kill);

	رکن->tl_kill = vim_strnsave(opt->jo_رکن_kill, p - opt->jo_رکن_kill);
    }

    إذا (opt->jo_رکن_api != عدم)
    {
	char_u *p = skiptowhite(opt->jo_رکن_api);

	رکن->tl_api = vim_strnsave(opt->jo_رکن_api, p - opt->jo_رکن_api);
    }
    وإلا
	رکن->tl_api = vim_strsave((char_u *)"Tapi_");

    إذا (opt->jo_set2 & JO2_رکن_HIGHLIGHT)
	رکن->tl_highlight_name = vim_strsave(opt->jo_رکن_highlight);

#إذا defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    // Save the user-defined palette, it is only used in GUI (or 'tgc' is on).
    إذا (opt->jo_set2 & JO2_ANSI_COLORS)
    {
	رکن->tl_palette = ALLOC_MULT(long_u, 16);
	إذا (رکن->tl_palette == عدم)
	{
	    vim_free(رکن);
	    أرجع عدم;
	}
	حافظہ_نقل(رکن->tl_palette, opt->jo_ansi_colors, حجم(long_u) * 16);
    }
#جے_انت

    // System dependent: setup the vterm and maybe start the job in it.
    إذا (argv == عدم
	    && argvar->v_type == VAR_STRING
	    && argvar->vval.v_string != عدم
	    && STRCMP(argvar->vval.v_string, "NONE") == 0)
	res = create_pty_only(رکن, opt);
    وإلا
	res = رکن_and_کم_init(رکن, argvar, argv, opt, &orig_opt);

    newbuf = curbuf;
    إذا (res == OK)
    {
	// Get and remember the size we ended up with.  Update the pty.
	vterm_get_ناپ(رکن->tl_vterm, &رکن->tl_صفاں, &رکن->tl_خانے_گنتی);
	رکن_report_winsize(رکن, رکن->tl_صفاں, رکن->tl_خانے_گنتی);
#جے_تعریف_شدہ FEAT_GUI
	إذا (رکن->tl_system)
	{
	    // display first line below typed command
	    رکن->tl_toprow = پیغام_صف + 1;
	    رکن->tl_dirty_صف_سرا = 0;
	}
#جے_انت

	// Make sure we don't get stuck on sending keys to the job, it leads to
	// a deadlock if the job is waiting for Vim to read.
	channel_سیٹ_nonblock(رکن->tl_کم->jv_channel, PART_وچ);

	إذا (old_curbuf != عدم)
	{
	    --curbuf->b_nwindows;
	    curbuf = old_curbuf;
	    curwin->w_بفر = curbuf;
	    curwin->w_cursor = save_cursor;
	    ++curbuf->b_nwindows;
	}
	وإلا إذا (vgetc_busy
#جے_تعریف_شدہ FEAT_TIMERS
		|| timer_busy
#جے_انت
		|| داخلہ_busy)
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
	رکن_close_بفر(curbuf, old_curbuf);
	أرجع عدم;
    }

    apply_autocmds(EVENT_TERMINALOPEN, عدم, عدم, FALSE, newbuf);
    إذا (!opt->jo_hidden && !(flags & رکن_شروع_SYSTEM))
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
    صحيح		scroll_save = پیغام_scroll;

    پیغام_scroll = FALSE;		// don't scroll here
    autowrite_all();
    پیغام_scroll = scroll_save;

    init_کم_options(&opt);

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

#تعریف_کرو OPTARG_HAS(name) ((صحيح)(p - cmd) == حجم(name) - 1 \
				 && STRNICMP(cmd, name, حجم(name) - 1) == 0)
	إذا (OPTARG_HAS("close"))
	    opt.jo_رکن_انت = 'c';
	وإلا إذا (OPTARG_HAS("noclose"))
	    opt.jo_رکن_انت = 'n';
	وإلا إذا (OPTARG_HAS("open"))
	    opt.jo_رکن_انت = 'o';
	وإلا إذا (OPTARG_HAS("curwin"))
	    opt.jo_curwin = 1;
	وإلا إذا (OPTARG_HAS("hidden"))
	    opt.jo_hidden = 1;
	وإلا إذا (OPTARG_HAS("norestore"))
	    opt.jo_رکن_norestore = 1;
	وإلا إذا (OPTARG_HAS("shell"))
	    opt_shell = TRUE;
	وإلا إذا (OPTARG_HAS("kill") && ep != عدم)
	{
	    opt.jo_set2 |= JO2_رکن_KILL;
	    opt.jo_رکن_kill = ep + 1;
	    p = skiptowhite(cmd);
	}
	وإلا إذا (OPTARG_HAS("api"))
	{
	    opt.jo_set2 |= JO2_رکن_API;
	    إذا (ep != عدم)
	    {
		opt.jo_رکن_api = ep + 1;
		p = skiptowhite(cmd);
	    }
	    وإلا
		opt.jo_رکن_api = عدم;
	}
	وإلا إذا (OPTARG_HAS("rows") && ep != عدم && محفوظ_isdigit(ep[1]))
	{
	    opt.jo_set2 |= JO2_رکن_صفاں;
	    opt.jo_رکن_صفاں = atoi((حرف *)ep + 1);
	    p = skiptowhite(cmd);
	}
	وإلا إذا (OPTARG_HAS("cols") && ep != عدم && محفوظ_isdigit(ep[1]))
	{
	    opt.jo_set2 |= JO2_رکن_خانے_گنتی;
	    opt.jo_رکن_خانے_گنتی = atoi((حرف *)ep + 1);
	    p = skiptowhite(cmd);
	}
	وإلا إذا (OPTARG_HAS("eof") && ep != عدم)
	{
	    char_u *buf = عدم;
	    char_u *کلیدیں;

	    vim_free(opt.jo_eof_chars);
	    p = skiptowhite(cmd);
	    *p = NUL;
	    کلیدیں = replace_termcodes(ep + 1, &buf, 0,
		    REPTERM_FROM_PART | REPTERM_DO_LT | REPTERM_SPECIAL, عدم);
	    opt.jo_set2 |= JO2_EOF_CHARS;
	    opt.jo_eof_chars = vim_strsave(کلیدیں);
	    vim_free(buf);
	    *p = ' ';
	}
#جے_تعریف_شدہ MSWIN
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
		semsg(_(e_invalid_قدر_for_دلیل_str), "type");
		اذهب theend;
	    }
	    opt.jo_set2 |= JO2_TTY_TYPE;
	    opt.jo_tty_type = tty_type;
	}
#جے_انت
	وإلا
	{
	    إذا (*p)
		*p = NUL;
	    semsg(_(e_invalid_attribute_str), cmd);
	    اذهب theend;
	}
#تعریف_رد OPTARG_HAS
	cmd = skipwhite(p);
    }
    إذا (*cmd == NUL)
    {
	// Make a copy of 'shell', an autocommand may change the option.
	tofree = cmd = vim_strsave(p_sh);

	// default to close when the shell exits
	إذا (opt.jo_رکن_انت == NUL)
	    opt.jo_رکن_انت = TL_انت_CLOSE;
    }

    إذا (eap->addr_گنتی > 0)
    {
	// Write lines from current buffer to the job.
	opt.jo_سیٹ |= JO_وچ_IO | JO_وچ_BUF | JO_وچ_چوٹی | JO_وچ_BOT;
	opt.jo_io[PART_وچ] = JIO_بفر;
	opt.jo_io_buf[PART_وچ] = curbuf->b_fnum;
	opt.jo_وچ_چوٹی = eap->line1;
	opt.jo_وچ_bot = eap->line2;
    }

    إذا (opt_shell && tofree == عدم)
    {
#جے_تعریف_شدہ UNIX
	حرف	**argv = عدم;
	char_u	*tofree1 = عدم;
	char_u	*tofree2 = عدم;

	// :term ++shell command
	إذا (unix_build_argv(cmd, &argv, &tofree1, &tofree2) == OK)
	    رکن_شروع(عدم, argv, &opt, eap->forceit ? رکن_شروع_FORCEIT : 0);
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
#جے_انت
    }
    argvar[0].v_type = VAR_STRING;
    argvar[0].vval.v_string = cmd;
    argvar[1].v_type = VAR_UNKNOWN;
    رکن_شروع(argvar, عدم, &opt, eap->forceit ? رکن_شروع_FORCEIT : 0);

theend:
    vim_free(tofree);
    vim_free(opt.jo_eof_chars);
}

    ساكن char_u *
get_terminalopt_name(expand_T *xp بن_ورتیا, صحيح idx)
{
    // Note: Keep this in sync with ex_terminal.
    ساكن حرف *(p_termopt_اقدار[]) =
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

    إذا (idx < (صحيح)ARRAY_لمائی(p_termopt_اقدار))
	أرجع (char_u*)p_termopt_اقدار[idx];
    أرجع عدم;
}

    ساكن char_u *
get_termkill_name(expand_T *xp بن_ورتیا, صحيح idx)
{
    // These are platform-specific values used for job_stop(). They are defined
    // in each platform's mch_signal_job(). Just use a unified auto-complete
    // list for simplicity.
    ساكن حرف *(p_termkill_اقدار[]) =
    {
	"term",
	"hup",
	"quit",
	"int",
	"kill",
	"winch",
    };

    إذا (idx < (صحيح)ARRAY_لمائی(p_termkill_اقدار))
	أرجع (char_u*)p_termkill_اقدار[idx];
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
	صحيح	    *عدد_Matches)
{
    إذا (xp->xp_نمونہ > xp->xp_سطر && *(xp->xp_نمونہ-1) == '=')
    {
	char_u *(*cb)(expand_T *, صحيح) = عدم;

	char_u *name_سرا = xp->xp_نمونہ - 1;
	إذا (name_سرا - xp->xp_سطر >= 4
		&& STRNCMP(name_سرا - 4, "kill", 4) == 0)
	    cb = get_termkill_name;

	إذا (cb != عدم)
	{
	    أرجع ExpandGeneric(
		    pat,
		    xp,
		    rmp,
		    matches,
		    عدد_Matches,
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
	    عدد_Matches,
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
رکن_write_session(FILE *fd, win_T *wp, hashtab_T *terminal_bufs)
{
    ثابت صحيح	bufnr = wp->w_بفر->b_fnum;
    رکن_T	*رکن = wp->w_بفر->b_رکن;

    إذا (terminal_bufs != عدم && wp->w_بفر->b_nwindows > 1)
    {
	// There are multiple views into this terminal buffer. We don't want to
	// create the terminal multiple times. If it's the first time, create,
	// otherwise link to the first buffer.
	حرف	    id_as_str[NUMBUFLEN];
	hashitem_T  *اندراج;

	vim_snprintf(id_as_str, حجم(id_as_str), "%d", bufnr);

	اندراج = hash_لبھو(terminal_bufs, (char_u *)id_as_str);
	إذا (!HASHITEM_خالی_تھاواں(اندراج))
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
		رکن->tl_خانے_گنتی, Columns / 2, Columns,
		رکن->tl_صفاں, صفاں / 2, صفاں) < 0)
	أرجع FAIL;
# ifdef MSWIN
    إذا (fprintf(fd, ".. ' ++type=%s' ", رکن->tl_کم->jv_tty_type) < 0)
	أرجع FAIL;
# endif
    إذا (رکن->tl_command != عدم)
    {
	char_u *quoted_command = string_quote(رکن->tl_command, FALSE);
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
	حرف *hash_کلید = alloc(NUMBUFLEN);

	vim_snprintf(hash_کلید, NUMBUFLEN, "%d", bufnr);
	hash_جوڑو(terminal_bufs, (char_u *)hash_کلید, "terminal session");
    }

    أرجع put_eol(fd);
}

/*
 * Return TRUE if "buf" has a terminal that should be restored.
 */
    صحيح
رکن_should_restore(buf_T *buf)
{
    رکن_T	*رکن = buf->b_رکن;

    أرجع رکن != عدم && (رکن->tl_command == عدم
				     || STRCMP(رکن->tl_command, "NONE") != 0);
}
#جے_انت

/*
 * Free the scrollback buffer for "term".
 */
    ساكن فراغ
free_scrollback(رکن_T *رکن)
{
    صحيح i;

    لكل (i = 0; i < رکن->tl_scrollback.ga_len; ++i)
	vim_free(((sb_سطر_T *)رکن->tl_scrollback.ga_انکڑے + i)->sb_cells);
    ga_clear(&رکن->tl_scrollback);
    لكل (i = 0; i < رکن->tl_scrollback_postponed.ga_len; ++i)
	vim_free(((sb_سطر_T *)رکن->tl_scrollback_postponed.ga_انکڑے + i)->sb_cells);
    ga_clear(&رکن->tl_scrollback_postponed);
}


// Terminals that need to be freed soon.
ساكن رکن_T	*terminals_منزل_free = عدم;

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
    رکن_T	*رکن = buf->b_رکن;
    رکن_T	*tp;

    إذا (رکن == عدم)
	أرجع;

    // Unlink the terminal form the list of terminals.
    إذا (پہلا_رکن == رکن)
	پہلا_رکن = رکن->tl_اگلا;
    وإلا
	لكل (tp = پہلا_رکن; tp->tl_اگلا != عدم; tp = tp->tl_اگلا)
	    إذا (tp->tl_اگلا == رکن)
	    {
		tp->tl_اگلا = رکن->tl_اگلا;
		اكسر;
	    }

    إذا (رکن->tl_کم != عدم)
    {
	إذا (رکن->tl_کم->jv_حالت != کم_ENDED
		&& رکن->tl_کم->jv_حالت != کم_مکمل
		&& رکن->tl_کم->jv_حالت != کم_FAILED)
	    کم_stop(رکن->tl_کم, عدم, "kill");
	کم_unref(رکن->tl_کم);
    }
    رکن->tl_اگلا = terminals_منزل_free;
    terminals_منزل_free = رکن;

    buf->b_رکن = عدم;
    إذا (وچ_terminal_loop == رکن)
	وچ_terminal_loop = عدم;
}

    فراغ
free_بن_ورتیا_terminals(فراغ)
{
    طالما (terminals_منزل_free != عدم)
    {
	رکن_T	    *رکن = terminals_منزل_free;

	terminals_منزل_free = رکن->tl_اگلا;

	free_scrollback(رکن);
	ga_clear(&رکن->tl_osc_buf);

	رکن_free_vterm(رکن);
	vim_free(رکن->tl_api);
	vim_free(رکن->tl_title);
#جے_تعریف_شدہ FEAT_SESSION
	vim_free(رکن->tl_command);
#جے_انت
	vim_free(رکن->tl_kill);
	vim_free(رکن->tl_حالت_متن);
	vim_free(رکن->tl_opencmd);
	vim_free(رکن->tl_eof_chars);
	vim_free(رکن->tl_arg0_cmd);
#جے_تعریف_شدہ MSWIN
	إذا (رکن->tl_باہرلا_fd != عدم)
	    fclose(رکن->tl_باہرلا_fd);
#جے_انت
	vim_free(رکن->tl_highlight_name);
	vim_free(رکن->tl_cursor_color);
	vim_free(رکن->tl_palette);
	vim_free(رکن);
    }
}

/*
 * Get the part that is connected to the tty. Normally this is PART_IN, but
 * when writing buffer lines to the job it can be another.  This makes it
 * possible to do "1,5term vim -".
 */
    ساكن ch_part_T
get_tty_part(رکن_T *رکن بن_ورتیا)
{
#جے_تعریف_شدہ UNIX
    ch_part_T	parts[3] = {PART_وچ, PART_باہرلا, PART_ERR};
    صحيح		i;

    لكل (i = 0; i < 3; ++i)
    {
	صحيح fd = رکن->tl_کم->jv_channel->ch_part[parts[i]].ch_fd;

	إذا (mch_isatty(fd))
	    أرجع parts[i];
    }
#جے_انت
    أرجع PART_وچ;
}

/*
 * Read any vterm output and send it on the channel.
 */
    ساكن فراغ
رکن_forward_output(رکن_T *رکن)
{
    VTerm *vterm = رکن->tl_vterm;
    حرف   buf[کلید_BUF_LEN];
    size_t curlen = vterm_output_read(vterm, buf, کلید_BUF_LEN);

    إذا (curlen > 0)
	channel_send(رکن->tl_کم->jv_channel, get_tty_part(رکن),
					     (char_u *)buf, (صحيح)curlen, عدم);
}

/*
 * Write job output "msg[len]" to the vterm.
 */
    ساكن فراغ
رکن_write_کم_output(رکن_T *رکن, char_u *پیغام_arg, size_t len_arg)
{
    char_u	*پیغام = پیغام_arg;
    size_t	len = len_arg;
    VTerm	*vterm = رکن->tl_vterm;
    size_t	prevlen = vterm_output_get_بفر_current(vterm);
    size_t	حد_بندی = رکن->tl_بفر->b_p_twsl * رکن->tl_خانے_گنتی * 3;

    // Limit the length to 'termwinscroll' * cols * 3 bytes.  Keep the text at
    // the end.
    إذا (len > حد_بندی)
    {
	char_u *p = پیغام + len - حد_بندی;

	p -= (*mb_سر_off)(پیغام, p);
	len -= p - پیغام;
	پیغام = p;
    }

    vterm_داخلہ_write(vterm, (حرف *)پیغام, len);

    // flush vterm buffer when vterm responded to control sequence
    إذا (prevlen != vterm_output_get_بفر_current(vterm))
	رکن_forward_output(رکن);

    // this invokes the damage callbacks
    vterm_screen_flush_damage(vterm_obtain_screen(vterm));
}

    ساكن فراغ
position_cursor(win_T *wp, VTermPos *pos)
{
    wp->w_wrow = MIN(pos->صف, MAX(0, wp->w_اچائی - 1));
    wp->w_wcol = MIN(pos->خانہ, MAX(0, wp->w_چوڑائی - 1));
#جے_تعریف_شدہ FEAT_PROP_POPUP
    إذا (popup_is_popup(wp))
    {
	wp->w_wrow += popup_چوٹی_extra(wp);
	wp->w_wcol += popup_بایاں_extra(wp);
	wp->w_flags |= WFLAG_WCOL_OFF_ADDED | WFLAG_WROW_OFF_ADDED;
    }
    وإلا
	wp->w_flags &= ~(WFLAG_WCOL_OFF_ADDED | WFLAG_WROW_OFF_ADDED);
#جے_انت
    wp->w_valid |= (VALID_WCOL|VALID_WROW);
}

    ساكن فراغ
update_cursor(رکن_T *رکن, صحيح redraw)
{
    إذا (رکن->tl_عام_منوال)
	أرجع;
#جے_تعریف_شدہ FEAT_GUI
    إذا (رکن->tl_system)
	windgoto(رکن->tl_cursor_pos.صف + رکن->tl_toprow,
		firstwin->w_wincol + رکن->tl_cursor_pos.خانہ);
    وإلا
#جے_انت
    إذا (!رکن_کم_running(رکن))
	// avoid the cursor positioned below the last used line
	setcursor();
    وإلا
    {
	// do not use the window cursor position
	position_cursor(curwin, &curbuf->b_رکن->tl_cursor_pos);
	windgoto(W_WINROW(curwin) + curwin->w_wrow,
		 curwin->w_wincol + curwin->w_wcol);
    }
    إذا (redraw)
    {
	aco_save_T	aco;

	إذا (رکن->tl_بفر == curbuf && رکن->tl_cursor_visible)
	    cursor_on();
	باہرلا_flush();
#جے_تعریف_شدہ FEAT_GUI
	إذا (gui.وچ_use)
	{
	    gui_update_cursor(FALSE, FALSE);
	    gui_mch_flush();
	}
#جے_انت
	// Make sure an invoked autocmd doesn't delete the buffer (and the
	// terminal) under our fingers.
	++رکن->tl_بفر->b_locked;

	// save and restore curwin and curbuf, in case the autocmd changes them
	aucmd_prepbuf(&aco, curbuf);
	apply_autocmds(EVENT_TEXTCHANGEDT, عدم, عدم, FALSE, رکن->tl_بفر);
	aucmd_restbuf(&aco);

	--رکن->tl_بفر->b_locked;
    }
}

/*
 * Find the location of a scrollbackline in the buffer
 */
    ساكن فراغ
scrollbackline_pos_وچ_buf(رکن_T *رکن, صحيح صف, linenr_T *lnum, صحيح *شروع_خانہ, size_t *شروع_pos)
{
    sb_سطر_T	*سطریں = (sb_سطر_T *)رکن->tl_scrollback.ga_انکڑے;
    linenr_T    calc_lnum = رکن->tl_بفر_scrolled;
    size_t      calc_pos = 0;
    صحيح         calc_خانہ = 0;
    صحيح         i;

    إذا (صف < 0 || صف >= رکن->tl_scrollback.ga_len)
	أرجع;

    إذا (صف > رکن->tl_scrollback_scrolled)
    {
	// Lookback how far along in the top line we are
	لكل (i = رکن->tl_scrollback_scrolled + 1; i > 0 && سطریں[i].continuation; --i)
	{
	    calc_pos += سطریں[i - 1].sb_بائٹاں;
	    calc_خانہ += سطریں[i - 1].sb_خانے_گنتی;
	}
	i = رکن->tl_scrollback_scrolled + 1;
	calc_lnum = رکن->tl_بفر_scrolled + 1;
	// Do not count this line's bytes/cols twice
	إذا (سطریں[i].continuation)
	    ++i;
    }
    وإلا
    {
	i = 1;
	calc_lnum = 1;
    }

    لكل (; i <= صف; ++i)
    {
	إذا (!سطریں[i].continuation)
	{
	    ++calc_lnum;
	    calc_pos = 0;
	    calc_خانہ = 0;
	}
	وإلا
	{
	    calc_pos += سطریں[i - 1].sb_بائٹاں;
	    calc_خانہ += سطریں[i - 1].sb_خانے_گنتی;
	}
    }

    *lnum = calc_lnum;
    إذا (شروع_خانہ)
	*شروع_خانہ = calc_خانہ;
    إذا (شروع_pos)
	*شروع_pos = calc_pos;
}

/*
 * Find the location of a buffer line in the scrollback
 */
    ساكن فراغ
bufline_pos_وچ_scrollback(رکن_T *رکن, linenr_T lnum, صحيح خانہ, صحيح *صف, صحيح *wrapped_خانہ)
{
    buf_T	*buf = رکن->tl_بفر;
    sb_سطر_T	*سطریں = (sb_سطر_T *)رکن->tl_scrollback.ga_انکڑے;
    linenr_T    calc_صف = رکن->tl_scrollback_scrolled;
    صحيح         calc_خانہ = خانہ;
    linenr_T    l;

    إذا (lnum > buf->b_ml.ml_سطر_گنتی)
	أرجع;

    إذا (lnum > رکن->tl_بفر_scrolled)
    {
	calc_صف = رکن->tl_scrollback_scrolled;
	l = رکن->tl_بفر_scrolled + 1;

	طالما (calc_صف < رکن->tl_scrollback.ga_len && سطریں[calc_صف].continuation)
	    ++calc_صف;
    }
    وإلا
    {
	calc_صف = 0;
	l = 1;
    }

    طالما (calc_صف < رکن->tl_scrollback.ga_len && l < lnum)
    {
	++calc_صف;
	إذا (!سطریں[calc_صف].continuation)
	    ++l;
    }

    طالما (calc_صف + 1 < رکن->tl_scrollback.ga_len && سطریں[calc_صف + 1].continuation
	    && calc_خانہ >= سطریں[calc_صف].sb_خانے_گنتی)
    {
	calc_خانہ -= سطریں[calc_صف].sb_خانے_گنتی;
	++calc_صف;
    }

    إذا (صف)
	*صف = calc_صف;
    إذا (wrapped_خانہ)
	*wrapped_خانہ = calc_خانہ;
}

/*
 * Invoked when "msg" output from a job was received.  Write it to the terminal
 * of "buffer".
 */
    فراغ
write_منزل_رکن(buf_T *بفر, char_u *پیغام, channel_T *channel)
{
    size_t	len = STRLEN(پیغام);
    رکن_T	*رکن = بفر->b_رکن;

#جے_تعریف_شدہ MSWIN
    // Win32: Cannot redirect output of the job, intercept it here and write to
    // the file.
    إذا (رکن->tl_باہرلا_fd != عدم)
    {
	ch_log(channel, "Writing %d bytes to output file", (صحيح)len);
	fwrite(پیغام, len, 1, رکن->tl_باہرلا_fd);
	أرجع;
    }
#جے_انت

    إذا (رکن->tl_vterm == عدم)
    {
	ch_log(channel, "NOT writing %d bytes to terminal", (صحيح)len);
	أرجع;
    }
    ch_log(channel, "writing %d bytes to terminal", (صحيح)len);
    cursor_off();
    رکن_write_کم_output(رکن, پیغام, len);

#جے_تعریف_شدہ FEAT_GUI
    إذا (رکن->tl_system)
    {
	// show system output, scrolling up the screen as needed
	update_system_رکن(رکن);
	update_cursor(رکن, TRUE);
    }
    وإلا
#جے_انت
    // In Terminal-Normal mode we are displaying the buffer, not the terminal
    // contents, thus no screen update is needed.
    إذا (!رکن->tl_عام_منوال)
    {
	// Don't use update_screen() when editing the command line, it gets
	// cleared.
	// TODO: only update once in a while.
	ch_log(رکن->tl_کم->jv_channel, "updating screen");
	إذا (بفر == curbuf && (State & منوال_CMDLINE) == 0)
	{
	    update_screen(UPD_VALID_NO_UPDATE);
#إذا defined(FEAT_TABPANEL)
	    إذا (redraw_tabpanel)
		draw_tabpanel();
#جے_انت
	    // update_screen() can be slow, check the terminal wasn't closed
	    // already
	    إذا (بفر == curbuf && curbuf->b_رکن != عدم)
		update_cursor(curbuf->b_رکن, TRUE);
	}
	وإلا
	    redraw_after_callback(TRUE, FALSE);
    }
}

/*
 * Send a mouse position and click to the vterm
 */
    ساكن صحيح
رکن_send_mouse(VTerm *vterm, صحيح button, صحيح pressed)
{
    VTermModifier   mod = VTERM_MOD_NONE;
    صحيح		    صف = mouse_صف - W_WINROW(curwin);
    صحيح		    خانہ = mouse_خانہ - curwin->w_wincol;

#جے_تعریف_شدہ FEAT_PROP_POPUP
    إذا (popup_is_popup(curwin))
    {
	صف -= popup_چوٹی_extra(curwin);
	خانہ -= popup_بایاں_extra(curwin);
    }
#جے_انت
    vterm_mouse_move(vterm, صف, خانہ, mod);
    إذا (button != 0)
	vterm_mouse_button(vterm, button, pressed, mod);
    أرجع TRUE;
}

ساكن صحيح enter_mouse_خانہ = -1;
ساكن صحيح enter_mouse_صف = -1;

/*
 * Handle a mouse click, drag or release.
 * Return TRUE when a mouse event is sent to the terminal.
 */
    ساكن صحيح
رکن_mouse_click(VTerm *vterm, صحيح کلید)
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
	بدل (کلید)
	{
	حالة K_LEFTDRAG:
	حالة K_LEFTRELEASE:
	حالة K_RIGHTDRAG:
	حالة K_RIGHTRELEASE:
		// Ignore drag and release events when the button-down wasn't
		// seen before.
		إذا (ignore_drag_release)
		{
		    صحيح save_mouse_خانہ, save_mouse_صف;

		    إذا (enter_mouse_خانہ < 0)
			اكسر;

		    // mouse click in the window gave us focus, handle that
		    // click now
		    save_mouse_خانہ = mouse_خانہ;
		    save_mouse_صف = mouse_صف;
		    mouse_خانہ = enter_mouse_خانہ;
		    mouse_صف = enter_mouse_صف;
		    clip_modeless(MOUSE_بایاں, TRUE, FALSE);
		    mouse_خانہ = save_mouse_خانہ;
		    mouse_صف = save_mouse_صف;
		}
		// FALLTHROUGH
	حالة K_LEFTMOUSE:
	حالة K_RIGHTMOUSE:
		إذا (کلید == K_LEFTRELEASE || کلید == K_RIGHTRELEASE)
		    ignore_drag_release = TRUE;
		وإلا
		    ignore_drag_release = FALSE;
		// Should we call mouse_has() here?
		إذا (clip_تارا.دستیاب)
		{
		    صحيح	    button, is_click, is_drag;

		    button = get_mouse_button(KEY2TERMCAP1(کلید),
							 &is_click, &is_drag);
		    إذا (mouse_model_popup() && button == MOUSE_بایاں
					       && (mod_نقاب & MOD_نقاب_کھسکاؤ))
		    {
			// Translate shift-left to right button.
			button = MOUSE_دایاں;
			mod_نقاب &= ~MOD_نقاب_کھسکاؤ;
		    }
		    clip_modeless(button, is_click, is_drag);
		}
		اكسر;

	حالة K_MIDDLEMOUSE:
		إذا (clip_تارا.دستیاب)
		    داخل_کرو_reg('*', TRUE);
		اكسر;
	}
	enter_mouse_خانہ = -1;
	أرجع FALSE;
    }
#جے_انت
    enter_mouse_خانہ = -1;

    بدل (کلید)
    {
	حالة K_LEFTMOUSE:
	حالة K_LEFTMOUSE_NM:	رکن_send_mouse(vterm, 1, 1); اكسر;
	حالة K_LEFTDRAG:	رکن_send_mouse(vterm, 1, 1); اكسر;
	حالة K_LEFTRELEASE:
	حالة K_LEFTRELEASE_NM:	رکن_send_mouse(vterm, 1, 0); اكسر;
	حالة K_MOUSEMOVE:	رکن_send_mouse(vterm, 0, 0); اكسر;
	حالة K_MIDDLEMOUSE:	رکن_send_mouse(vterm, 2, 1); اكسر;
	حالة K_MIDDLEDRAG:	رکن_send_mouse(vterm, 2, 1); اكسر;
	حالة K_MIDDLERELEASE:	رکن_send_mouse(vterm, 2, 0); اكسر;
	حالة K_RIGHTMOUSE:	رکن_send_mouse(vterm, 3, 1); اكسر;
	حالة K_RIGHTDRAG:	رکن_send_mouse(vterm, 3, 1); اكسر;
	حالة K_RIGHTRELEASE:	رکن_send_mouse(vterm, 3, 0); اكسر;
    }
    أرجع TRUE;
}

/*
 * Convert typed key "c" with modifiers "modmask" into bytes to send to the
 * job.
 * Return the number of bytes in "buf".
 */
    ساكن صحيح
رکن_convert_کلید(رکن_T *رکن, صحيح c, صحيح modmask, حرف *buf)
{
    VTerm	    *vterm = رکن->tl_vterm;
    VTerm_کلید	    کلید = VTERM_کلید_NONE;
    VTermModifier   mod = VTERM_MOD_NONE;
    صحيح		    other = FALSE;

    بدل (c)
    {
	// don't use VTERM_KEY_ENTER, it may do an unwanted conversion

	حالة K_BS:
#جے_تعریف_شدہ MSWIN
	    // In ConPTY, we must use VTERM_KEY_BACKSPACE, otherwise it will
	    // delete one word, equivalent to Alt+Backspace.
	    إذا (رکن->tl_conpty)
		کلید = VTERM_کلید_BACKSPACE;
	    وإلا
#جے_انت
		// don't use VTERM_KEY_BACKSPACE, it always becomes 0x7f DEL
		c = رکن_backspace_char;
	    اكسر;

#جے_تعریف_شدہ MSWIN
	حالة BS:
	    // In ConPTY, we must use VTERM_KEY_BACKSPACE, otherwise it will
	    // delete one word, equivalent to Alt+Backspace.
	    إذا (رکن->tl_conpty)
		کلید = VTERM_کلید_BACKSPACE;
	    اكسر;
#جے_انت

	حالة ESC:		کلید = VTERM_کلید_ESCAPE; اكسر;
	حالة K_DEL:		کلید = VTERM_کلید_DEL; اكسر;
	حالة K_تھلے:		کلید = VTERM_کلید_تھلے; اكسر;
	حالة K_S_تھلے:		mod = VTERM_MOD_کھسکاؤ;
				کلید = VTERM_کلید_تھلے; اكسر;
	حالة K_سرا:		کلید = VTERM_کلید_سرا; اكسر;
	حالة K_S_سرا:		mod = VTERM_MOD_کھسکاؤ;
				کلید = VTERM_کلید_سرا; اكسر;
	حالة K_C_سرا:		mod = VTERM_MOD_CTRL;
				کلید = VTERM_کلید_سرا; اكسر;
	حالة K_F10:		کلید = VTERM_کلید_FUNCTION(10); اكسر;
	حالة K_F11:		کلید = VTERM_کلید_FUNCTION(11); اكسر;
	حالة K_F12:		کلید = VTERM_کلید_FUNCTION(12); اكسر;
	حالة K_F1:		کلید = VTERM_کلید_FUNCTION(1); اكسر;
	حالة K_F2:		کلید = VTERM_کلید_FUNCTION(2); اكسر;
	حالة K_F3:		کلید = VTERM_کلید_FUNCTION(3); اكسر;
	حالة K_F4:		کلید = VTERM_کلید_FUNCTION(4); اكسر;
	حالة K_F5:		کلید = VTERM_کلید_FUNCTION(5); اكسر;
	حالة K_F6:		کلید = VTERM_کلید_FUNCTION(6); اكسر;
	حالة K_F7:		کلید = VTERM_کلید_FUNCTION(7); اكسر;
	حالة K_F8:		کلید = VTERM_کلید_FUNCTION(8); اكسر;
	حالة K_F9:		کلید = VTERM_کلید_FUNCTION(9); اكسر;
	حالة K_HOME:		کلید = VTERM_کلید_HOME; اكسر;
	حالة K_S_HOME:		mod = VTERM_MOD_کھسکاؤ;
				کلید = VTERM_کلید_HOME; اكسر;
	حالة K_C_HOME:		mod = VTERM_MOD_CTRL;
				کلید = VTERM_کلید_HOME; اكسر;
	حالة K_INS:		کلید = VTERM_کلید_INS; اكسر;
	حالة K_K0:		کلید = VTERM_کلید_KP_0; اكسر;
	حالة K_K1:		کلید = VTERM_کلید_KP_1; اكسر;
	حالة K_K2:		کلید = VTERM_کلید_KP_2; اكسر;
	حالة K_K3:		کلید = VTERM_کلید_KP_3; اكسر;
	حالة K_K4:		کلید = VTERM_کلید_KP_4; اكسر;
	حالة K_K5:		کلید = VTERM_کلید_KP_5; اكسر;
	حالة K_K6:		کلید = VTERM_کلید_KP_6; اكسر;
	حالة K_K7:		کلید = VTERM_کلید_KP_7; اكسر;
	حالة K_K8:		کلید = VTERM_کلید_KP_8; اكسر;
	حالة K_K9:		کلید = VTERM_کلید_KP_9; اكسر;
	حالة K_KDEL:		کلید = VTERM_کلید_DEL; اكسر; // TODO
	حالة K_KDIVIDE:		کلید = VTERM_کلید_KP_DIVIDE; اكسر;
	حالة K_KEND:		کلید = VTERM_کلید_KP_1; اكسر; // TODO
	حالة K_KENTER:		کلید = VTERM_کلید_KP_ENTER; اكسر;
	حالة K_KHOME:		کلید = VTERM_کلید_KP_7; اكسر; // TODO
	حالة K_KINS:		کلید = VTERM_کلید_KP_0; اكسر; // TODO
	حالة K_KMINUS:		کلید = VTERM_کلید_KP_MINUS; اكسر;
	حالة K_KMULTIPLY:	کلید = VTERM_کلید_KP_MULT; اكسر;
	حالة K_KPAGEDOWN:	کلید = VTERM_کلید_KP_3; اكسر; // TODO
	حالة K_KPAGEUP:		کلید = VTERM_کلید_KP_9; اكسر; // TODO
	حالة K_KPLUS:		کلید = VTERM_کلید_KP_PLUS; اكسر;
	حالة K_KPOINT:		کلید = VTERM_کلید_KP_PERIOD; اكسر;
	حالة K_بایاں:		کلید = VTERM_کلید_بایاں; اكسر;
	حالة K_S_بایاں:		mod = VTERM_MOD_کھسکاؤ;
				کلید = VTERM_کلید_بایاں; اكسر;
	حالة K_C_بایاں:		mod = VTERM_MOD_CTRL;
				کلید = VTERM_کلید_بایاں; اكسر;
	حالة K_PAGEDOWN:	کلید = VTERM_کلید_PAGEDOWN; اكسر;
	حالة K_PAGEUP:		کلید = VTERM_کلید_PAGEUP; اكسر;
	حالة K_دایاں:		کلید = VTERM_کلید_دایاں; اكسر;
	حالة K_S_دایاں:		mod = VTERM_MOD_کھسکاؤ;
				کلید = VTERM_کلید_دایاں; اكسر;
	حالة K_C_دایاں:		mod = VTERM_MOD_CTRL;
				کلید = VTERM_کلید_دایاں; اكسر;
	حالة K_اتے:		کلید = VTERM_کلید_اتے; اكسر;
	حالة K_S_اتے:		mod = VTERM_MOD_کھسکاؤ;
				کلید = VTERM_کلید_اتے; اكسر;
	حالة TAB:		کلید = VTERM_کلید_TAB; اكسر;
	حالة K_S_TAB:		mod = VTERM_MOD_کھسکاؤ;
				کلید = VTERM_کلید_TAB; اكسر;

	حالة K_MOUSEUP:		other = رکن_send_mouse(vterm, 5, 1); اكسر;
	حالة K_MOUSEDOWN:	other = رکن_send_mouse(vterm, 4, 1); اكسر;
	حالة K_MOUSELEFT:	other = رکن_send_mouse(vterm, 7, 1); اكسر;
	حالة K_MOUSERIGHT:	other = رکن_send_mouse(vterm, 6, 1); اكسر;

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
	حالة K_RIGHTRELEASE:	إذا (!رکن_mouse_click(vterm, c))
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
	حالة K_XF1:		کلید = VTERM_کلید_FUNCTION(1); اكسر;
	حالة K_XF2:		کلید = VTERM_کلید_FUNCTION(2); اكسر;
	حالة K_XF3:		کلید = VTERM_کلید_FUNCTION(3); اكسر;
	حالة K_XF4:		کلید = VTERM_کلید_FUNCTION(4); اكسر;
	حالة K_SELECT:		أرجع 0;
#جے_تعریف_شدہ FEAT_GUI
	حالة K_VER_SCROLLBAR:	أرجع 0;
	حالة K_HOR_SCROLLBAR:	أرجع 0;
#جے_انت
#جے_تعریف_شدہ FEAT_GUI_TABLINE
	حالة K_TABLINE:		أرجع 0;
	حالة K_TABMENU:		أرجع 0;
#جے_انت
#جے_تعریف_شدہ FEAT_NETBEANS_INTG
	حالة K_F21:		کلید = VTERM_کلید_FUNCTION(21); اكسر;
#جے_انت
#جے_تعریف_شدہ FEAT_DND
	حالة K_DROP:		أرجع 0;
#جے_انت
	حالة K_CURSORHOLD:	أرجع 0;
	حالة K_PS:		vterm_keyboard_شروع_paste(vterm);
				other = TRUE;
				اكسر;
	حالة K_PE:		vterm_keyboard_سرا_paste(vterm);
				other = TRUE;
				اكسر;
    }

    // add modifiers for the typed key
    إذا (modmask & MOD_نقاب_کھسکاؤ)
	mod |= VTERM_MOD_کھسکاؤ;
    إذا (modmask & MOD_نقاب_CTRL)
	mod |= VTERM_MOD_CTRL;
    إذا (modmask & (MOD_نقاب_ALT | MOD_نقاب_META))
	mod |= VTERM_MOD_ALT;

    // Ctrl-Shift-i may have the key "I" instead of "i", but for the kitty
    // keyboard protocol should use "i".  Applies to all ascii letters.
    إذا (ASCII_ISUPPER(c)
	    && vterm_is_kitty_keyboard(vterm)
	    && mod == (VTERM_MOD_CTRL | VTERM_MOD_کھسکاؤ))
	c = TOLOWER_ASC(c);

    /*
     * Convert special keys to vterm keys:
     * - Write keys to vterm: vterm_keyboard_key()
     * - Write output to channel.
     */
    إذا (کلید != VTERM_کلید_NONE)
	// Special key, let vterm convert it.
	vterm_keyboard_کلید(vterm, کلید, mod);
    وإلا إذا (!other)
	// Normal character, let vterm convert it.
	vterm_keyboard_unichar(vterm, c, mod);

    // Read back the converted escape sequence.
    أرجع (صحيح)vterm_output_read(vterm, buf, کلید_BUF_LEN);
}

/*
 * Return TRUE if the job for "term" is still running.
 * If "check_job_status" is TRUE update the job status.
 * NOTE: "term" may be freed by callbacks.
 */
    ساكن صحيح
رکن_کم_running_check(رکن_T *رکن, صحيح check_کم_حالت)
{
    // Also consider the job finished when the channel is closed, to avoid a
    // race condition when updating the title.
    إذا (رکن == عدم
	|| رکن->tl_کم == عدم
	|| !channel_is_open(رکن->tl_کم->jv_channel))
	أرجع FALSE;

    کم_T *کم = رکن->tl_کم;

    // Careful: Checking the job status may invoke callbacks, which close
    // the buffer and terminate "term".  However, "job" will not be freed
    // yet.
    إذا (check_کم_حالت)
	کم_حالت(کم);
    أرجع (کم->jv_حالت == کم_STARTED
	    || (کم->jv_channel != عدم && کم->jv_channel->ch_keep_open));
}

/*
 * Return TRUE if the job for "term" is still running.
 */
    صحيح
رکن_کم_running(رکن_T *رکن)
{
    أرجع رکن_کم_running_check(رکن, FALSE);
}

/*
 * Return TRUE if the job for "term" is still running, ignoring the job was
 * "NONE".
 */
    صحيح
رکن_کم_running_not_none(رکن_T *رکن)
{
    أرجع رکن_کم_running(رکن) && !رکن_none_open(رکن);
}

/*
 * Return TRUE if "term" has an active channel and used ":term NONE".
 */
    صحيح
رکن_none_open(رکن_T *رکن)
{
    // Also consider the job finished when the channel is closed, to avoid a
    // race condition when updating the title.
    أرجع رکن != عدم
	&& رکن->tl_کم != عدم
	&& channel_is_open(رکن->tl_کم->jv_channel)
	&& رکن->tl_کم->jv_channel->ch_keep_open;
}

//
// Used to confirm whether we would like to kill a terminal.
// Return OK when the user confirms to kill it.
// Return FAIL if the user selects otherwise.
//
    صحيح
رکن_confirm_stop(buf_T *buf)
{
    char_u	buff[DIALOG_پیغام_ناپ];
    صحيح	ret;

    dialog_پیغام(buff, _("Kill job in \"%s\"?"), buf_get_fname(buf));
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
رکن_try_stop_کم(buf_T *buf)
{
    صحيح	    گنتی;
    حرف    *how = (حرف *)buf->b_رکن->tl_kill;

#إذا defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
    إذا ((how == عدم || *how == NUL)
			  && (p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)))
    {
	إذا (رکن_confirm_stop(buf) == OK)
	    how = "kill";
	وإلا
	    أرجع FAIL;
    }
#جے_انت
    إذا (how == عدم || *how == NUL)
	أرجع FAIL;

    کم_stop(buf->b_رکن->tl_کم, عدم, how);

    // wait for up to a second for the job to die
    لكل (گنتی = 0; گنتی < 100; ++گنتی)
    {
	کم_T *کم;

	// buffer, terminal and job may be cleaned up while waiting
	إذا (!buf_valid(buf)
		|| buf->b_رکن == عدم
		|| buf->b_رکن->tl_کم == عدم)
	    أرجع OK;
	کم = buf->b_رکن->tl_کم;

	// Call job_status() to update jv_status. It may cause the job to be
	// cleaned up but it won't be freed.
	کم_حالت(کم);
	إذا (کم->jv_حالت >= کم_ENDED)
	    أرجع OK;

	ui_delay(10L, TRUE);
	رکن_flush_messages();
    }
    أرجع FAIL;
}

/*
 * Add the last line of the scrollback buffer to the buffer in the window.
 */
    ساكن فراغ
جوڑو_scrollback_سطر_منزل_بفر(رکن_T *رکن, char_u *متن, صحيح len, صحيح append)
{
    buf_T	*buf = رکن->tl_بفر;
    صحيح		خالی_تھاواں_2 = (buf->b_ml.ml_flags & ML_خالی_تھاواں);
    linenr_T	lnum = buf->b_ml.ml_سطر_گنتی;

#جے_تعریف_شدہ MSWIN
    char_u *tmp = متن;
    إذا (!enc_utf8 && enc_codepage > 0)
    {
	WCHAR   *ret = عدم;
	صحيح	لمائی = 0;

	Multi_Byte_منزل_Wide_Char_alloc(CP_UTF8, 0, (حرف*)متن, len + 1,
							   &ret, &لمائی);
	إذا (ret != عدم)
	{
	    Wide_Char_منزل_Multi_Byte_alloc(enc_codepage, 0,
				      ret, لمائی, (حرف **)&متن, &len, 0, 0);
	    vim_free(ret);
	}
    }
#جے_انت

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

#جے_تعریف_شدہ MSWIN
    إذا (tmp != متن)
	vim_free(متن);
#جے_انت
    إذا (خالی_تھاواں_2)
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
    attr->چوڑائی = cell->چوڑائی;
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
جوڑو_خالی_تھاواں_scrollback(رکن_T *رکن, cellattr_T *fill_attr, صحيح lnum)
{
    إذا (ga_grow(&رکن->tl_scrollback, 1) == FAIL)
	أرجع FALSE;

    sb_سطر_T *سطر = (sb_سطر_T *)رکن->tl_scrollback.ga_انکڑے
	+ رکن->tl_scrollback.ga_len;

    إذا (lnum > 0)
    {
	صحيح i;

	لكل (i = 0; i < رکن->tl_scrollback.ga_len - lnum; ++i)
	{
	    *سطر = *(سطر - 1);
	    --سطر;
	}
    }
    سطر->sb_خانے_گنتی = 0;
    سطر->sb_بائٹاں = 0;
    سطر->sb_cells = عدم;
    سطر->sb_fill_attr = *fill_attr;
    ++رکن->tl_scrollback.ga_len;
    أرجع OK;
}

/*
 * Remove the terminal contents from the scrollback and the buffer.
 * Used before adding a new scrollback line or updating the buffer for lines
 * displayed in the terminal.
 */
    ساكن فراغ
cleanup_scrollback(رکن_T *رکن)
{
    sb_سطر_T	*سطر;
    garray_T	*gap;
    char_u	*bufline;
    size_t      bufline_لمائی;

    curbuf = رکن->tl_بفر;
    gap = &رکن->tl_scrollback;
    bufline = ml_get_buf(curbuf, curbuf->b_ml.ml_سطر_گنتی, FALSE);
    bufline_لمائی = STRLEN(bufline);
    طالما (رکن->tl_scrollback_snapshot && gap->ga_len > 0)
    {
	سطر = (sb_سطر_T *)gap->ga_انکڑے + gap->ga_len - 1;
	إذا (سطر->sb_بائٹاں < 0 || (size_t)سطر->sb_بائٹاں > bufline_لمائی)
	    اكسر;
	bufline_لمائی -= سطر->sb_بائٹاں;
	إذا (!bufline_لمائی)
	{
	    ml_delete(curbuf->b_ml.ml_سطر_گنتی);
	    bufline = ml_get_buf(curbuf, curbuf->b_ml.ml_سطر_گنتی, FALSE);
	    bufline_لمائی = STRLEN(bufline);
	}
	vim_free(سطر->sb_cells);
	--gap->ga_len;
	--رکن->tl_scrollback_snapshot;
    }
    إذا (bufline_لمائی < STRLEN(bufline))
    {
	char_u *shortened = vim_strnsave(bufline, bufline_لمائی);
	ml_replace(curbuf->b_ml.ml_سطر_گنتی, shortened, FALSE);
    }
    curbuf = curwin->w_بفر;
    إذا (curbuf == رکن->tl_بفر)
	check_cursor();
}

/*
 * Add the current lines of the terminal to scrollback and to the buffer.
 */
    ساكن فراغ
update_snapshot(رکن_T *رکن)
{
    VTermScreen	    *screen;
    VTermState	    *state;
    صحيح		    len;
    صحيح		    سطریں_skipped = 0;
    VTermPos	    pos;
    VTermScreenCell cell;
    cellattr_T	    fill_attr, new_fill_attr;
    cellattr_T	    *p;

    ch_log(رکن->tl_کم == عدم ? عدم : رکن->tl_کم->jv_channel,
				  "Adding terminal window snapshot to buffer");

    // First remove the lines that were appended before, they might be
    // outdated.
    cleanup_scrollback(رکن);

    screen = vterm_obtain_screen(رکن->tl_vterm);
    state = vterm_obtain_state(رکن->tl_vterm);
    fill_attr = new_fill_attr = رکن->tl_default_color;
    لكل (pos.صف = 0; pos.صف < رکن->tl_صفاں; ++pos.صف)
    {
	len = 0;
	لكل (pos.خانہ = 0; pos.خانہ < رکن->tl_خانے_گنتی; ++pos.خانہ)
	    إذا (vterm_screen_get_cell(screen, pos, &cell) != 0
						       && cell.chars[0] != NUL)
	    {
		len = pos.خانہ + 1;
		new_fill_attr = رکن->tl_default_color;
	    }
	    وإلا
		// Assume the last attr is the filler attr.
		cell2cellattr(&cell, &new_fill_attr);

	إذا (len == 0 && equal_celattr(&new_fill_attr, &fill_attr))
	    ++سطریں_skipped;
	وإلا
	{
	    طالما (سطریں_skipped > 0)
	    {
		// Line was skipped, add an empty line.
		--سطریں_skipped;
		إذا (جوڑو_خالی_تھاواں_scrollback(رکن, &fill_attr, 0) == OK)
		{
		    جوڑو_scrollback_سطر_منزل_بفر(رکن, (char_u *)"", 0, 0);
		    ++رکن->tl_scrollback_snapshot;
		}
	    }

	    إذا (len == 0)
		p = عدم;
	    وإلا
		p = ALLOC_MULT(cellattr_T, len);
	    إذا ((p != عدم || len == 0)
				     && ga_grow(&رکن->tl_scrollback, 1) == OK)
	    {
		garray_T    ga;
		صحيح	    چوڑائی;
		ثابت VTerm_سطر_Info *lineinfo;
		sb_سطر_T   *سطر = (sb_سطر_T *)رکن->tl_scrollback.ga_انکڑے
						  + رکن->tl_scrollback.ga_len;

		lineinfo = vterm_state_get_lineinfo(state, pos.صف);

		ga_init2(&ga, 1, 100);
		لكل (pos.خانہ = 0; pos.خانہ < len; pos.خانہ += چوڑائی)
		{
		    إذا (vterm_screen_get_cell(screen, pos, &cell) == 0)
		    {
			چوڑائی = 1;
			CLEAR_اشارہ_گر(p + pos.خانہ);
			إذا (ga_grow(&ga, 1) == OK)
			    ga.ga_len += utf_char2bytes(' ',
					     (char_u *)ga.ga_انکڑے + ga.ga_len);
		    }
		    وإلا
		    {
			چوڑائی = cell.چوڑائی;

			cell2cellattr(&cell, &p[pos.خانہ]);
			إذا (چوڑائی == 2)
			    // second cell of double-width character has the
			    // same attributes.
			    p[pos.خانہ + 1] = p[pos.خانہ];

			// Each character can be up to 6 bytes.
			إذا (ga_grow(&ga, VTERM_MAX_CHARS_فی_CELL * 6) == OK)
			{
			    صحيح	    i;
			    صحيح	    c;

			    لكل (i = 0; i < VTERM_MAX_CHARS_فی_CELL &&
				    ((c = cell.chars[i]) > 0 || i == 0); ++i)
				ga.ga_len += utf_char2bytes(c == NUL ? ' ' : c,
					     (char_u *)ga.ga_انکڑے + ga.ga_len);
			}
		    }
		}
		سطر->sb_خانے_گنتی = len;
		سطر->sb_بائٹاں = ga.ga_len;
		سطر->sb_cells = p;
		سطر->sb_fill_attr = new_fill_attr;
		سطر->continuation = (char_u)lineinfo->continuation;
		fill_attr = new_fill_attr;
		++رکن->tl_scrollback.ga_len;
		++رکن->tl_scrollback_snapshot;

		إذا (ga_grow(&ga, 1) == FAIL)
		    جوڑو_scrollback_سطر_منزل_بفر(رکن, (char_u *)"", 0, 0);
		وإلا
		{
		    *((char_u *)ga.ga_انکڑے + ga.ga_len) = NUL;
		    جوڑو_scrollback_سطر_منزل_بفر(رکن, ga.ga_انکڑے, ga.ga_len,
						  lineinfo->continuation);
		}
		ga_clear(&ga);
	    }
	    وإلا
		vim_free(p);
	}
    }

    // Add trailing empty lines.
    لكل (pos.صف = رکن->tl_scrollback.ga_len;
	    pos.صف < رکن->tl_scrollback_scrolled + رکن->tl_cursor_pos.صف;
	    ++pos.صف)
    {
	إذا (جوڑو_خالی_تھاواں_scrollback(رکن, &fill_attr, 0) == OK)
	{
	    جوڑو_scrollback_سطر_منزل_بفر(رکن, (char_u *)"", 0, 0);
	    ++رکن->tl_scrollback_snapshot;
	}
    }

    رکن->tl_dirty_snapshot = FALSE;
#جے_تعریف_شدہ FEAT_TIMERS
    رکن->tl_timer_سیٹ = FALSE;
#جے_انت
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
    وإلا إذا ((*wp)->w_اگلا != عدم)
	*wp = (*wp)->w_اگلا;
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
may_move_terminal_منزل_بفر(رکن_T *رکن, صحيح redraw)
{
    إذا (رکن->tl_vterm == عدم)
	أرجع;

    // Update the snapshot only if something changes or the buffer does not
    // have all the lines.
    إذا (رکن->tl_dirty_snapshot || رکن->tl_بفر->b_ml.ml_سطر_گنتی
					       <= رکن->tl_بفر_scrolled)
	update_snapshot(رکن);

    إذا (redraw)
    {
	win_T	    *wp = عدم;
	صحيح	    did_curwin = FALSE;

	طالما (for_all_windows_and_curwin(&wp, &did_curwin))
	{
	    إذا (wp->w_بفر == رکن->tl_بفر)
	    {
		wp->w_cursor.lnum = رکن->tl_بفر->b_ml.ml_سطر_گنتی;
		wp->w_cursor.خانہ = 0;
		wp->w_valid = 0;
		إذا (wp->w_cursor.lnum >= wp->w_اچائی)
		{
		    linenr_T min_topline = wp->w_cursor.lnum - wp->w_اچائی + 1;

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
رکن_check_timers(صحيح اگلا_due_arg, proftime_T *now)
{
    رکن_T  *رکن;
    صحيح	    اگلا_due = اگلا_due_arg;

    FOR_ALL_TERMS(رکن)
    {
	إذا (رکن->tl_timer_سیٹ && !رکن->tl_عام_منوال)
	{
	    لما    this_due = proftime_ویلا_بایاں(&رکن->tl_timer_due, now);

	    إذا (this_due <= 1)
	    {
		رکن->tl_timer_سیٹ = FALSE;
		may_move_terminal_منزل_بفر(رکن, FALSE);
	    }
	    وإلا إذا (اگلا_due == -1 || اگلا_due > this_due)
		اگلا_due = this_due;
	}
    }

    أرجع اگلا_due;
}
#جے_انت

/*
 * When "normal_mode" is TRUE set the terminal to Terminal-Normal mode,
 * otherwise end it.
 */
    ساكن فراغ
سیٹ_terminal_منوال(رکن_T *رکن, صحيح عام_منوال)
{
    رکن->tl_عام_منوال = عام_منوال;
    may_trigger_modechanged();
    إذا (!عام_منوال)
	handle_postponed_scrollback(رکن);
    VIM_CLEAR(رکن->tl_حالت_متن);
    إذا (رکن->tl_بفر == curbuf)
	maketitle();
}

/*
 * Called after the job is finished and Terminal mode is not active:
 * Move the vterm contents into the scrollback buffer and free the vterm.
 */
    ساكن فراغ
cleanup_vterm(رکن_T *رکن)
{
    سیٹ_terminal_منوال(رکن, FALSE);
    إذا (رکن->tl_انت != TL_انت_CLOSE)
	may_move_terminal_منزل_بفر(رکن, TRUE);
    رکن_free_vterm(رکن);
}

/*
 * Switch from Terminal-Job mode to Terminal-Normal mode.
 * Suspends updating the terminal window.
 */
    ساكن فراغ
رکن_enter_عام_منوال(فراغ)
{
    رکن_T     *رکن = curbuf->b_رکن;
    linenr_T   lnum;
    صحيح        خانہ;

    سیٹ_terminal_منوال(رکن, TRUE);

    // Append the current terminal contents to the buffer.
    may_move_terminal_منزل_بفر(رکن, TRUE);

    // Move the window cursor to the position of the cursor in the
    // terminal.
    lnum = رکن->tl_بفر_scrolled + 1 + رکن->tl_cursor_pos.صف;
    خانہ = رکن->tl_cursor_pos.خانہ;
    scrollbackline_pos_وچ_buf(رکن, رکن->tl_cursor_pos.صف + رکن->tl_scrollback_scrolled, &lnum, &خانہ, عدم);

    curwin->w_cursor.lnum = lnum;
    check_cursor();
    إذا (coladvance(خانہ) == FAIL)
	coladvance(MAXCOL);
    curwin->w_سیٹ_curswant = true;
    curwin->w_topline = رکن->tl_بفر_scrolled + 1;
}

/*
 * Returns TRUE if the current window contains a terminal and we are in
 * Terminal-Normal mode.
 */
    صحيح
رکن_وچ_عام_منوال(buf_T *buf)
{
    رکن_T *رکن = buf->b_رکن;

    أرجع رکن != عدم && رکن->tl_عام_منوال;
}

/*
 * Switch from Terminal-Normal mode to Terminal-Job mode.
 * Restores updating the terminal window.
 */
    فراغ
رکن_enter_کم_منوال(فراغ)
{
    رکن_T	*رکن = curbuf->b_رکن;

    سیٹ_terminal_منوال(رکن, FALSE);

    إذا (رکن->tl_channel_closed)
	cleanup_vterm(رکن);
    redraw_buf_and_حالت_later(curbuf, UPD_NOT_VALID);
#جے_تعریف_شدہ FEAT_PROP_POPUP
    إذا (WIN_IS_POPUP(curwin))
	redraw_later(UPD_NOT_VALID);
#جے_انت
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
    NRKS_سیٹ,	    // no_reduce_keys was incremented in term_vgetc() or
		    // check_no_reduce_keys(), must be decremented.
} reduce_کلید_state_T;

ساكن reduce_کلید_state_T  no_reduce_کلید_state = NRKS_NONE;

/*
 * Return TRUE if the term is using modifyOtherKeys level 2 or the kitty
 * keyboard protocol.
 */
    ساكن صحيح
vterm_using_کلید_protocol(فراغ)
{
    أرجع curbuf->b_رکن != عدم
	&& curbuf->b_رکن->tl_vterm != عدم
	&& (vterm_is_modify_other_کلیدیں(curbuf->b_رکن->tl_vterm)
		|| vterm_is_kitty_keyboard(curbuf->b_رکن->tl_vterm));
}

    فراغ
check_no_reduce_کلیدیں(فراغ)
{
    إذا (no_reduce_کلید_state != NRKS_CHECK
	    || no_reduce_کلیدیں >= 1
	    || curbuf->b_رکن == عدم
	    || curbuf->b_رکن->tl_vterm == عدم)
	أرجع;

    إذا (vterm_using_کلید_protocol())
    {
	// "modify_other_keys" or kitty keyboard protocol was enabled while
	// waiting.
	no_reduce_کلید_state = NRKS_سیٹ;
	++no_reduce_کلیدیں;
    }
}

/*
 * Get a key from the user with terminal mode mappings.
 * Note: while waiting a terminal may be closed and freed if the channel is
 * closed and ++close was used.  This may even happen before we get here.
 */
    ساكن صحيح
رکن_vgetc(فراغ)
{
    صحيح c;
    صحيح save_State = State;

    State = منوال_TERMINAL;
    got_int = FALSE;
#جے_تعریف_شدہ MSWIN
    ctrl_break_was_pressed = FALSE;
#جے_انت

    إذا (vterm_using_کلید_protocol())
    {
	++no_reduce_کلیدیں;
	no_reduce_کلید_state = NRKS_سیٹ;
    }
    وإلا
    {
	no_reduce_کلید_state = NRKS_CHECK;
    }

    c = vgetc();
    got_int = FALSE;
    State = save_State;

    إذا (no_reduce_کلید_state == NRKS_سیٹ)
	--no_reduce_کلیدیں;
    no_reduce_کلید_state = NRKS_NONE;

    أرجع c;
}

ساكن صحيح	mouse_was_outside = FALSE;

/*
 * Send key "c" with modifiers "modmask" to terminal.
 * Return FAIL when the key needs to be handled in Normal mode.
 * Return OK when the key was dropped or sent to the terminal.
 */
    صحيح
send_کلیدیں_منزل_رکن(رکن_T *رکن, صحيح c, صحيح modmask, صحيح typed)
{
    حرف	پیغام[کلید_BUF_LEN];
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
		صحيح	خانہ = mouse_خانہ;

#جے_تعریف_شدہ FEAT_PROP_POPUP
		إذا (popup_is_popup(curwin))
		{
		    صف -= popup_چوٹی_extra(curwin);
		    خانہ -= popup_بایاں_extra(curwin);
		}
#جے_انت
		إذا (صف < W_WINROW(curwin)
			|| صف >= (W_WINROW(curwin) + curwin->w_اچائی)
			|| خانہ < curwin->w_wincol
			|| خانہ >= W_ENDCOL(curwin)
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
    len = رکن_convert_کلید(رکن, c, modmask, پیغام);
    إذا (len > 0)
	// TODO: if FAIL is returned, stop?
	channel_send(رکن->tl_کم->jv_channel, get_tty_part(رکن),
						(char_u *)پیغام, (صحيح)len, عدم);

    أرجع OK;
}

/*
 * Handle CTRL-W "": send register contents to the job.
 */
    ساكن فراغ
رکن_paste_رجسٹر(صحيح prev_c بن_ورتیا)
{
    صحيح		c;
    list_T	*l;
    listitem_T	*شے;
    لما	reglen = 0;
    صحيح		type;

    إذا (جوڑو_منزل_showcmd(prev_c))
    إذا (جوڑو_منزل_showcmd('"'))
	باہرلا_flush();

    c = رکن_vgetc();
    clear_showcmd();

    إذا (!رکن_use_loop())
	// job finished while waiting for a character
	أرجع;

    // CTRL-W "= prompt for expression to evaluate.
    إذا (c == '=' && get_اظہاریہ_رجسٹر() != '=')
	أرجع;
    إذا (!رکن_use_loop())
	// job finished while waiting for a character
	أرجع;

    l = (list_T *)get_reg_contents(c, GREG_LIST);
    إذا (l == عدم)
	أرجع;

    type = get_reg_type(c, &reglen);
    FOR_ALL_LIST_اشیاء(l, شے)
    {
	char_u *s = tv_get_string(&شے->li_tv);
#جے_تعریف_شدہ MSWIN
	char_u *tmp = s;

	إذا (!enc_utf8 && enc_codepage > 0)
	{
	    WCHAR   *ret = عدم;
	    صحيح	لمائی = 0;

	    Multi_Byte_منزل_Wide_Char_alloc(enc_codepage, 0, (حرف *)s,
		    (صحيح)STRLEN(s), &ret, &لمائی);
	    إذا (ret != عدم)
	    {
		Wide_Char_منزل_Multi_Byte_alloc(CP_UTF8, 0,
			ret, لمائی, (حرف **)&s, &لمائی, 0, 0);
		vim_free(ret);
	    }
	}
#جے_انت
	channel_send(curbuf->b_رکن->tl_کم->jv_channel, PART_وچ,
		s, (صحيح)STRLEN(s), عدم);
#جے_تعریف_شدہ MSWIN
	إذا (tmp != s)
	    vim_free(s);
#جے_انت

	إذا (شے->li_اگلا != عدم || type == MLINE)
	    channel_send(curbuf->b_رکن->tl_کم->jv_channel, PART_وچ,
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
    أرجع وچ_terminal_loop != عدم;
}

/*
 * Return the highlight group ID for the terminal and the window.
 */
    ساكن صحيح
رکن_get_highlight_id(رکن_T *رکن, win_T *wp)
{
    char_u *name;

    إذا (wp != عدم && wp->w_hlfwin_id != 0)
	name = syn_id2name(wp->w_hlfwin_id);
    وإلا إذا (رکن->tl_highlight_name != عدم)
	name = رکن->tl_highlight_name;
    وإلا
	name = (char_u*)"Terminal";

    أرجع syn_name2id(name);
}

#إذا defined(FEAT_GUI)
    cursorentry_T *
رکن_get_cursor_shape(guicolor_T *fg, guicolor_T *bg)
{
    رکن_T		 *رکن = وچ_terminal_loop;
    ساكن cursorentry_T اندراج;
    صحيح			 id;
    guicolor_T		 رکن_fg = INVALCOLOR;
    guicolor_T		 رکن_bg = INVALCOLOR;

    CLEAR_میدان(اندراج);
    اندراج.shape = اندراج.mshape =
	رکن->tl_cursor_shape == VTERM_PROP_CURSORSHAPE_UNDERLINE ? SHAPE_HOR :
	رکن->tl_cursor_shape == VTERM_PROP_CURSORSHAPE_BAR_بایاں ? SHAPE_VER :
	SHAPE_بلاک;
    اندراج.percentage = 20;
    إذا (رکن->tl_cursor_blink)
    {
	اندراج.blinkwait = 700;
	اندراج.blinkon = 400;
	اندراج.blinkoff = 250;
    }

    // The highlight group overrules the defaults.
    id = رکن_get_highlight_id(رکن, curwin);
    إذا (id != 0)
	syn_id2colors(id, &رکن_fg, &رکن_bg);
    إذا (رکن_bg != INVALCOLOR)
	*fg = رکن_bg;
    وإلا
	*fg = gui.پٹھ_pixel;

    إذا (رکن->tl_cursor_color == عدم)
    {
	إذا (رکن_fg != INVALCOLOR)
	    *bg = رکن_fg;
	وإلا
	    *bg = gui.norm_pixel;
    }
    وإلا
	*bg = color_name2handle(رکن->tl_cursor_color);
    اندراج.name = "n";
    اندراج.ورتیا_for = SHAPE_CURSOR;

    أرجع &اندراج;
}
#جے_انت

    ساكن فراغ
may_output_cursor_props(فراغ)
{
    إذا (!cursor_color_equal(آخری_سیٹ_cursor_color, desired_cursor_color)
	    || آخری_سیٹ_cursor_shape != desired_cursor_shape
	    || آخری_سیٹ_cursor_blink != desired_cursor_blink)
    {
	cursor_color_copy(&آخری_سیٹ_cursor_color, desired_cursor_color);
	آخری_سیٹ_cursor_shape = desired_cursor_shape;
	آخری_سیٹ_cursor_blink = desired_cursor_blink;
	رکن_cursor_color(cursor_color_get(desired_cursor_color));
	إذا (desired_cursor_shape == -1 || desired_cursor_blink == -1)
	    // this will restore the initial cursor style, if possible
	    ui_cursor_shape_forced(TRUE);
	وإلا
	    رکن_cursor_shape(desired_cursor_shape, desired_cursor_blink);
    }
}

/*
 * Set the cursor color and shape, if not last set to these.
 */
    ساكن فراغ
may_سیٹ_cursor_props(رکن_T *رکن)
{
#جے_تعریف_شدہ FEAT_GUI
    // For the GUI the cursor properties are obtained with
    // term_get_cursor_shape().
    إذا (gui.وچ_use)
	أرجع;
#جے_انت
    إذا (وچ_terminal_loop == رکن)
    {
	cursor_color_copy(&desired_cursor_color, رکن->tl_cursor_color);
	desired_cursor_shape = رکن->tl_cursor_shape;
	desired_cursor_blink = رکن->tl_cursor_blink;
	may_output_cursor_props();
    }
}

/*
 * Reset the desired cursor properties and restore them when needed.
 */
    ساكن فراغ
prepare_restore_cursor_props(فراغ)
{
#جے_تعریف_شدہ FEAT_GUI
    إذا (gui.وچ_use)
	أرجع;
#جے_انت
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
رکن_use_loop_check(صحيح check_کم_حالت)
{
    رکن_T *رکن = curbuf->b_رکن;

    أرجع رکن != عدم
	&& !رکن->tl_عام_منوال
	&& رکن->tl_vterm != عدم
	&& رکن_کم_running_check(رکن, check_کم_حالت);
}

/*
 * Returns TRUE if the current window contains a terminal and we are sending
 * keys to the job.
 */
    صحيح
رکن_use_loop(فراغ)
{
    أرجع رکن_use_loop_check(FALSE);
}

/*
 * Called when entering a window with the mouse.  If this is a terminal window
 * we may want to change state.
 */
    فراغ
رکن_win_entered(فراغ)
{
    رکن_T *رکن = curbuf->b_رکن;

    إذا (رکن == عدم)
	أرجع;

    إذا (رکن_use_loop_check(TRUE))
    {
	reset_VIsual_and_resel();
	إذا (State & منوال_داخل_کرو)
	    stop_داخل_کرو_منوال = TRUE;
    }
    mouse_was_outside = FALSE;
    enter_mouse_خانہ = mouse_خانہ;
    enter_mouse_صف = mouse_صف;
}

    فراغ
رکن_focus_change(صحيح وچ_focus)
{
    رکن_T *رکن = curbuf->b_رکن;

    إذا (رکن == عدم || رکن->tl_vterm == عدم)
	أرجع;

    VTermState	*state = vterm_obtain_state(رکن->tl_vterm);

    إذا (وچ_focus)
	vterm_state_focus_وچ(state);
    وإلا
	vterm_state_focus_باہرلا(state);
    رکن_forward_output(رکن);
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
    إذا (c < 0x20 && vterm_using_کلید_protocol())
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
#جے_تعریف_شدہ UNIX
    صحيح		tty_fd = curbuf->b_رکن->tl_کم->jv_channel
				 ->ch_part[get_tty_part(curbuf->b_رکن)].ch_fd;
#جے_انت
    صحيح		restore_cursor = FALSE;

    // Remember the terminal we are sending keys to.  However, the terminal
    // might be closed while waiting for a character, e.g. typing "exit" in a
    // shell and ++close was used.  Therefore use curbuf->b_term instead of a
    // stored reference.
    وچ_terminal_loop = curbuf->b_رکن;

    إذا (*curwin->w_p_twk != NUL)
    {
	termwinkey = string_منزل_کلید(curwin->w_p_twk, TRUE);
	إذا (termwinkey == Ctrl_W)
	    termwinkey = 0;
    }
    position_cursor(curwin, &curbuf->b_رکن->tl_cursor_pos);
    may_سیٹ_cursor_props(curbuf->b_رکن);

    طالما (blocking || vpeekc_nomap() != NUL)
    {
#جے_تعریف_شدہ FEAT_GUI
	إذا (curbuf->b_رکن != عدم && !curbuf->b_رکن->tl_system)
#جے_انت
	    // TODO: skip screen update when handling a sequence of keys.
	    // Repeat redrawing in case a message is received while redrawing.
	    طالما (must_redraw != 0)
		إذا (update_screen(0) == FAIL)
		    اكسر;
	إذا (!رکن_use_loop_check(TRUE) || وچ_terminal_loop != curbuf->b_رکن)
	    // job finished while redrawing
	    اكسر;

	update_cursor(curbuf->b_رکن, FALSE);
	restore_cursor = TRUE;

	raw_c = رکن_vgetc();
	إذا (!رکن_use_loop_check(TRUE) || وچ_terminal_loop != curbuf->b_رکن)
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

#جے_تعریف_شدہ UNIX
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
		رکن_backspace_char = info.backspace;
	}
#جے_انت

#جے_تعریف_شدہ MSWIN
	// On Windows winpty handles CTRL-C, don't send a CTRL_C_EVENT.
	// Use CTRL-BREAK to kill the job.
	إذا (ctrl_break_was_pressed)
	    mch_signal_کم(curbuf->b_رکن->tl_کم, (char_u *)"kill");
#جے_انت
	// Was either CTRL-W (termwinkey) or CTRL-\ pressed?
	// Not in a system terminal.
	إذا ((c == (termwinkey == 0 ? Ctrl_W : termwinkey) || c == Ctrl_BSL)
#جے_تعریف_شدہ FEAT_GUI
		&& !curbuf->b_رکن->tl_system
#جے_انت
		)
	{
	    صحيح	    prev_c = c;
	    صحيح	    prev_raw_c = raw_c;
	    صحيح	    prev_mod_نقاب = mod_نقاب;

	    إذا (جوڑو_منزل_showcmd(c))
		باہرلا_flush();

	    raw_c = رکن_vgetc();
	    c = raw_c_منزل_ctrl(raw_c);

	    clear_showcmd();

	    إذا (!رکن_use_loop_check(TRUE)
					 || وچ_terminal_loop != curbuf->b_رکن)
		// job finished while waiting for a character
		اكسر;

	    إذا (prev_c == Ctrl_BSL)
	    {
		إذا (c == Ctrl_N)
		{
		    // CTRL-\ CTRL-N : go to Terminal-Normal mode.
		    رکن_enter_عام_منوال();
		    ret = FAIL;
		    اذهب theend;
		}
		// Send both keys to the terminal, first one here, second one
		// below.
		send_کلیدیں_منزل_رکن(curbuf->b_رکن, prev_raw_c, prev_mod_نقاب,
									 TRUE);
	    }
	    وإلا إذا (c == Ctrl_C)
	    {
		// "CTRL-W CTRL-C" or 'termwinkey' CTRL-C: end the job
		mch_signal_کم(curbuf->b_رکن->tl_کم, (char_u *)"kill");
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
		رکن_enter_عام_منوال();
		ret = FAIL;
		اذهب theend;
	    }
	    وإلا إذا (c == '"')
	    {
		رکن_paste_رجسٹر(prev_c);
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
#جے_تعریف_شدہ MSWIN
	إذا (!enc_utf8 && has_mbyte && raw_c >= 0x80)
	{
	    WCHAR   wc;
	    char_u  mb[3];

	    mb[0] = (بنانشانی)raw_c >> 8;
	    mb[1] = raw_c;
	    إذا (Multi_Byte_منزل_Wide_Char(GetACP(), 0, (حرف*)mb, 2, &wc, 1) > 0)
		raw_c = wc;
	}
#جے_انت
	إذا (send_کلیدیں_منزل_رکن(curbuf->b_رکن, raw_c, mod_نقاب, TRUE) != OK)
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
    وچ_terminal_loop = عدم;
    إذا (restore_cursor)
	prepare_restore_cursor_props();

    // Move a snapshot of the screen contents to the buffer, so that completion
    // works in other buffers.
    إذا (curbuf->b_رکن != عدم && !curbuf->b_رکن->tl_عام_منوال)
	may_move_terminal_منزل_بفر(curbuf->b_رکن, FALSE);

    أرجع ret;
}

    ساكن فراغ
may_toggle_cursor(رکن_T *رکن)
{
    إذا (وچ_terminal_loop != رکن)
	أرجع;

    إذا (رکن->tl_cursor_visible)
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

    إذا (VTERM_COLOR_IS_اشاریہ_دار(color))
    {
	// Use the color as-is if possible, give up otherwise.
	إذا (color->اشاریہ < t_colors)
	{
	    uint8_t اشاریہ = color->اشاریہ;
#جے_تعریف_شدہ MSWIN
	    // Convert ANSI palette to cterm color index.
	    // cterm_ansi_idx is a table for the reverse conversion from cterm
	    // color index to ANSI palette. However, the current table can
	    // be used for this conversion as well.
	    // Note: If the contents of cterm_ansi_idx change in the future, a
	    // different table may be needed for this purpose.
	    إذا (اشاریہ < 16)
		اشاریہ = cterm_ansi_idx[اشاریہ];
#جے_انت
	    أرجع اشاریہ + 1;
	}
	// 8-color terminals can actually display twice as many colors by
	// setting the high-intensity/bold bit.
	وإلا إذا (t_colors == 8 && fg && color->اشاریہ < 16)
	{
	    *boldp = TRUE;
	    أرجع (color->اشاریہ & 7) + 1;
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
    إذا (cellattrs->الٹ)
	attr |= HL_INVERSE;
    أرجع attr;
}

/*
 * Store Vterm attributes in "cell" from highlight flags.
 */
    ساكن فراغ
hl2vtermAttr(صحيح attr, cellattr_T *cell)
{
    CLEAR_میدان(cell->attrs);
    إذا (attr & HL_BOLD)
	cell->attrs.bold = 1;
    إذا (attr & HL_UNDERLINE)
	cell->attrs.underline = 1;
    إذا (attr & HL_ITALIC)
	cell->attrs.italic = 1;
    إذا (attr & HL_STRIKETHROUGH)
	cell->attrs.strike = 1;
    إذا (attr & HL_INVERSE)
	cell->attrs.الٹ = 1;
}

/*
 * Convert the attributes of a vterm cell into an attribute index.
 */
    ساكن صحيح
cell2attr(
	رکن_T			*رکن,
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
		fg = &wp->w_رکن_hlfwin.fg;
	    إذا (is_default_bg)
		bg = &wp->w_رکن_hlfwin.bg;
	}
	وإلا
	{
	    إذا (is_default_fg)
		fg = &رکن->tl_default_color.fg;
	    إذا (is_default_bg)
		bg = &رکن->tl_default_color.bg;
	}
    }

#جے_تعریف_شدہ FEAT_GUI
    إذا (gui.وچ_use)
    {
	guicolor_T guifg = gui_mch_get_rgb_color(fg->red, fg->green, fg->blue);
	guicolor_T guibg = gui_mch_get_rgb_color(bg->red, bg->green, bg->blue);
	أرجع get_gui_attr_idx(attr, guifg, guibg);
    }
    وإلا
#جے_انت
#جے_تعریف_شدہ FEAT_TERMGUICOLORS
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
#جے_انت
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
سیٹ_dirty_snapshot(رکن_T *رکن)
{
    رکن->tl_dirty_snapshot = TRUE;
#جے_تعریف_شدہ FEAT_TIMERS
    إذا (!رکن->tl_عام_منوال)
    {
	// Update the snapshot after 100 msec of not getting updates.
	profile_setlimit(100L, &رکن->tl_timer_due);
	رکن->tl_timer_سیٹ = TRUE;
    }
#جے_انت
}

    ساكن صحيح
handle_damage(VTermRect rect, فراغ *user)
{
    رکن_T *رکن = (رکن_T *)user;

    رکن->tl_dirty_صف_شروع = MIN(رکن->tl_dirty_صف_شروع, rect.شروع_صف);
    رکن->tl_dirty_صف_سرا = MAX(رکن->tl_dirty_صف_سرا, rect.سرا_صف);
    سیٹ_dirty_snapshot(رکن);
    redraw_buf_later(رکن->tl_بفر, UPD_SOME_VALID);
    أرجع 1;
}

    ساكن فراغ
رکن_scroll_اتے(رکن_T *رکن, صحيح شروع_صف, صحيح گنتی)
{
    win_T		 *wp = عدم;
    صحيح			 did_curwin = FALSE;
    VTermColor		 fg, bg;
    VTermScreenCellAttrs attr;
    صحيح			 clear_attr;

    CLEAR_میدان(attr);

    طالما (for_all_windows_and_curwin(&wp, &did_curwin))
    {
	إذا (wp->w_بفر == رکن->tl_بفر)
	{
	    // Set the color to clear lines with.
	    vterm_state_get_default_colors(vterm_obtain_state(رکن->tl_vterm),
								     &fg, &bg);
	    clear_attr = cell2attr(رکن, wp, &attr, &fg, &bg);
	    win_del_سطریں(wp, شروع_صف, گنتی, FALSE, FALSE, clear_attr);
	}
    }
}

    ساكن صحيح
handle_moverect(VTermRect dest, VTermRect src, فراغ *user)
{
    رکن_T	*رکن = (رکن_T *)user;
    صحيح		گنتی = src.شروع_صف - dest.شروع_صف;

    // Scrolling up is done much more efficiently by deleting lines instead of
    // redrawing the text. But avoid doing this multiple times, postpone until
    // the redraw happens.
    إذا (dest.شروع_خانہ == src.شروع_خانہ
	    && dest.سرا_خانہ == src.سرا_خانہ
	    && dest.شروع_صف < src.شروع_صف)
    {
	إذا (dest.شروع_صف == 0)
	    رکن->tl_postponed_scroll += گنتی;
	وإلا
	    رکن_scroll_اتے(رکن, dest.شروع_صف, گنتی);
    }

    رکن->tl_dirty_صف_شروع = MIN(رکن->tl_dirty_صف_شروع, dest.شروع_صف);
    رکن->tl_dirty_صف_سرا = MIN(رکن->tl_dirty_صف_سرا, dest.سرا_صف);
    سیٹ_dirty_snapshot(رکن);

    // Note sure if the scrolling will work correctly, let's do a complete
    // redraw later.
    redraw_buf_later(رکن->tl_بفر, UPD_NOT_VALID);
    أرجع 1;
}

    ساكن صحيح
handle_movecursor(
	VTermPos pos,
	VTermPos oldpos بن_ورتیا,
	صحيح visible,
	فراغ *user)
{
    رکن_T	*رکن = (رکن_T *)user;
    win_T	*wp = عدم;
    صحيح		did_curwin = FALSE;

    رکن->tl_cursor_pos = pos;
    رکن->tl_cursor_visible = visible;

    طالما (for_all_windows_and_curwin(&wp, &did_curwin))
    {
	إذا (wp->w_بفر == رکن->tl_بفر)
	    position_cursor(wp, &pos);
    }
    إذا (رکن->tl_بفر == curbuf && !رکن->tl_عام_منوال)
	// Don't redraw here, it will be done after
	// vterm_input_write() is finished.
	update_cursor(رکن, FALSE);

    أرجع 1;
}

    ساكن صحيح
handle_settermprop(
	VTermProp prop,
	VTerm_قدر *قدر,
	فراغ *user)
{
    رکن_T	*رکن = (رکن_T *)user;
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
	    vim_free(رکن->tl_title);
	    // a blank title isn't useful, make it empty, so that "running" is
	    // displayed
	    إذا (*skipwhite(strval) == NUL)
		رکن->tl_title = عدم;
	    // Same as blank
	    وإلا إذا (رکن->tl_arg0_cmd != عدم
		    && STRNCMP(رکن->tl_arg0_cmd, strval,
					  (صحيح)STRLEN(رکن->tl_arg0_cmd)) == 0)
		رکن->tl_title = عدم;
	    // Empty corrupted data of winpty
	    وإلا إذا (STRNCMP("  - ", strval, 4) == 0)
		رکن->tl_title = عدم;
#جے_تعریف_شدہ MSWIN
	    وإلا إذا (!enc_utf8 && enc_codepage > 0)
	    {
		WCHAR   *ret = عدم;
		صحيح	لمائی = 0;

		Multi_Byte_منزل_Wide_Char_alloc(CP_UTF8, 0,
			(حرف*)قدر->string.str,
					(صحيح)قدر->string.len, &ret, &لمائی);
		إذا (ret != عدم)
		{
		    Wide_Char_منزل_Multi_Byte_alloc(enc_codepage, 0,
					ret, لمائی, (حرف**)&رکن->tl_title,
					&لمائی, 0, 0);
		    vim_free(ret);
		}
	    }
#جے_انت
	    وإلا
	    {
		رکن->tl_title = strval;
		strval = عدم;
	    }
	    VIM_CLEAR(رکن->tl_حالت_متن);
	    إذا (رکن == curbuf->b_رکن)
	    {
		maketitle();
		curwin->w_redr_حالت = true;
	    }
	    اكسر;

	حالة VTERM_PROP_CURSORVISIBLE:
	    رکن->tl_cursor_visible = قدر->boolean;
	    may_toggle_cursor(رکن);
	    باہرلا_flush();
	    اكسر;

	حالة VTERM_PROP_CURSORBLINK:
	    رکن->tl_cursor_blink = قدر->boolean;
	    may_سیٹ_cursor_props(رکن);
	    اكسر;

	حالة VTERM_PROP_CURSORSHAPE:
	    رکن->tl_cursor_shape = قدر->نمبر;
	    may_سیٹ_cursor_props(رکن);
	    اكسر;

	حالة VTERM_PROP_CURSORCOLOR:
	    strval = vim_strnsave((char_u *)قدر->string.str,
							    قدر->string.len);
	    إذا (strval == عدم)
		اكسر;
	    cursor_color_copy(&رکن->tl_cursor_color, strval);
	    may_سیٹ_cursor_props(رکن);
	    اكسر;

	حالة VTERM_PROP_ALTSCREEN:
	    // TODO: do anything else?
	    رکن->tl_using_altscreen = قدر->boolean;
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
handle_resize(صحيح صفاں_2, صحيح خانے_گنتی, فراغ *user)
{
    رکن_T	*رکن = (رکن_T *)user;
    win_T	*wp;

    رکن->tl_صفاں = صفاں_2;
    رکن->tl_خانے_گنتی = خانے_گنتی;
    إذا (رکن->tl_vterm_ناپ_بدلیا)
	// Size was set by vterm_set_size(), don't set the window size.
	رکن->tl_vterm_ناپ_بدلیا = FALSE;
    وإلا
    {
	FOR_ALL_WINDOWS(wp)
	{
	    إذا (wp->w_بفر == رکن->tl_بفر)
	    {
		win_setheight_win(صفاں_2, wp);
		win_setwidth_win(خانے_گنتی, wp);
	    }
	}
	redraw_buf_later(رکن->tl_بفر, UPD_NOT_VALID);
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
حد_بندی_scrollback(رکن_T *رکن, garray_T *gap, صحيح update_بفر)
{
    إذا (gap->ga_len < رکن->tl_بفر->b_p_twsl)
	أرجع;

    صحيح	todo = MAX(رکن->tl_بفر->b_p_twsl / 10,
				     gap->ga_len - رکن->tl_بفر->b_p_twsl);
    صحيح	i;
    sb_سطر_T *sb_سطریں = (sb_سطر_T *)gap->ga_انکڑے;

    curbuf = رکن->tl_بفر;
    لكل (i = 0; i < todo; ++i)
    {
	إذا (update_بفر && (!sb_سطریں[i].continuation || !i))
	{
	    ml_delete(1);
	    --رکن->tl_بفر_scrolled;
	}
	vim_free(sb_سطریں[i].sb_cells);
    }
    // Continue until end of wrapped line
    لكل (; todo < gap->ga_len && sb_سطریں[todo].continuation; ++todo)
	vim_free(sb_سطریں[todo].sb_cells);
    curbuf = curwin->w_بفر;

    gap->ga_len -= todo;
    mch_memmove(gap->ga_انکڑے,
		(sb_سطر_T *)gap->ga_انکڑے + todo,
		حجم(sb_سطر_T) * gap->ga_len);
    إذا (update_بفر)
    {
	win_T *curwin_save = curwin;
	win_T *wp = عدم;

	رکن->tl_scrollback_scrolled -= todo;

	FOR_ALL_WINDOWS(wp)
	{
	    إذا (wp->w_بفر == رکن->tl_بفر)
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
handle_pushline(صحيح خانے_گنتی, ثابت VTermScreenCell *cells, صحيح continuation, فراغ *user)
{
    رکن_T	*رکن = (رکن_T *)user;
    garray_T	*gap;
    صحيح		update_بفر;

    إذا (رکن->tl_عام_منوال)
    {
	// In Terminal-Normal mode the user interacts with the buffer, thus we
	// must not change it. Postpone adding the scrollback lines.
	gap = &رکن->tl_scrollback_postponed;
	update_بفر = FALSE;
    }
    وإلا
    {
	// First remove the lines that were appended before, the pushed line
	// goes above it.
	cleanup_scrollback(رکن);
	gap = &رکن->tl_scrollback;
	update_بفر = TRUE;
    }

    حد_بندی_scrollback(رکن, gap, update_بفر);

    إذا (ga_grow(gap, 1) == FAIL)
	أرجع 0;

    cellattr_T	*p = عدم;
    صحيح		len = 0;
    صحيح		i;
    صحيح		c;
    صحيح		خانہ;
    صحيح		متن_len;
    char_u		*متن;
    sb_سطر_T	*سطر;
    garray_T	ga;
    cellattr_T	fill_attr = رکن->tl_default_color;

    // do not store empty cells at the end
    لكل (i = 0; i < خانے_گنتی; ++i)
	إذا (cells[i].chars[0] != 0)
	    len = i + 1;
	وإلا
	    cell2cellattr(&cells[i], &fill_attr);

    ga_init2(&ga, 1, 100);
    إذا (len > 0)
	p = ALLOC_MULT(cellattr_T, len);
    إذا (p != عدم)
    {
	لكل (خانہ = 0; خانہ < len; خانہ += cells[خانہ].چوڑائی)
	{
	    إذا (ga_grow(&ga, VTERM_MAX_CHARS_فی_CELL * 4) == FAIL)
	    {
		ga.ga_len = 0;
		اكسر;
	    }
	    لكل (i = 0; i < VTERM_MAX_CHARS_فی_CELL &&
		    ((c = cells[خانہ].chars[i]) > 0 || i == 0); ++i)
		ga.ga_len += utf_char2bytes(c == NUL ? ' ' : c,
			(char_u *)ga.ga_انکڑے + ga.ga_len);
	    cell2cellattr(&cells[خانہ], &p[خانہ]);
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
	متن = ga.ga_انکڑے;
	متن_len = ga.ga_len;
	*(متن + متن_len) = NUL;
    }
    إذا (update_بفر)
	جوڑو_scrollback_سطر_منزل_بفر(رکن, متن, متن_len, continuation);

    سطر = (sb_سطر_T *)gap->ga_انکڑے + gap->ga_len;
    سطر->sb_خانے_گنتی = len;
    سطر->sb_بائٹاں = متن_len;
    سطر->sb_cells = p;
    سطر->sb_fill_attr = fill_attr;
    سطر->continuation = (char_u)continuation;
    إذا (update_بفر)
    {
	سطر->sb_متن = عدم;
	++رکن->tl_scrollback_scrolled;
	إذا (!continuation)
	    ++رکن->tl_بفر_scrolled;
	ga_clear(&ga);  // free the text
    }
    وإلا
    {
	سطر->sb_متن = متن;
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
handle_postponed_scrollback(رکن_T *رکن)
{
    صحيح i;

    إذا (رکن->tl_scrollback_postponed.ga_len == 0)
	أرجع;
    ch_log(عدم, "Moving postponed scrollback to scrollback");

    // First remove the lines that were appended before, the pushed lines go
    // above it.
    cleanup_scrollback(رکن);

    لكل (i = 0; i < رکن->tl_scrollback_postponed.ga_len; ++i)
    {
	char_u		*متن;
	sb_سطر_T	*pp_سطر;
	sb_سطر_T	*سطر;

	إذا (ga_grow(&رکن->tl_scrollback, 1) == FAIL)
	    اكسر;
	pp_سطر = (sb_سطر_T *)رکن->tl_scrollback_postponed.ga_انکڑے + i;

	متن = pp_سطر->sb_متن;
	إذا (متن == عدم)
	    متن = (char_u *)"";
	جوڑو_scrollback_سطر_منزل_بفر(رکن, متن, (صحيح)STRLEN(متن), pp_سطر->continuation);
	vim_free(pp_سطر->sb_متن);

	سطر = (sb_سطر_T *)رکن->tl_scrollback.ga_انکڑے
						 + رکن->tl_scrollback.ga_len;
	سطر->sb_خانے_گنتی = pp_سطر->sb_خانے_گنتی;
	سطر->sb_بائٹاں = pp_سطر->sb_بائٹاں;
	سطر->sb_cells = pp_سطر->sb_cells;
	سطر->sb_fill_attr = pp_سطر->sb_fill_attr;
	سطر->sb_متن = عدم;
	++رکن->tl_scrollback_scrolled;
	++رکن->tl_scrollback.ga_len;
	إذا (!pp_سطر->continuation)
	    ++رکن->tl_بفر_scrolled;
    }

    ga_clear(&رکن->tl_scrollback_postponed);
    حد_بندی_scrollback(رکن, &رکن->tl_scrollback, TRUE);
}

/*
 * Called when the terminal wants to ring the system bell.
 */
    ساكن صحيح
handle_bell(فراغ *user بن_ورتیا)
{
    vim_beep(BO_رکن);
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
رکن_after_channel_closed(رکن_T *رکن)
{
    // Unless in Terminal-Normal mode: clear the vterm.
    إذا (!رکن->tl_عام_منوال)
    {
	صحيح	fnum = رکن->tl_بفر->b_fnum;

	cleanup_vterm(رکن);

	إذا (رکن->tl_انت == TL_انت_CLOSE)
	{
	    aco_save_T	aco;
	    صحيح		do_سیٹ_w_locked = رکن->tl_بفر->b_nwindows == 0;
#جے_تعریف_شدہ FEAT_PROP_POPUP
	    win_T	*pwin = عدم;

	    // If this was a terminal in a popup window, go back to the
	    // previous window.
	    إذا (popup_is_popup(curwin) && curbuf == رکن->tl_بفر)
	    {
		pwin = curwin;
		إذا (win_valid(prevwin))
		    win_enter(prevwin, FALSE);
	    }
	    وإلا
#جے_انت
	    // If this is the last normal window: exit Vim.
	    إذا (رکن->tl_بفر->b_nwindows > 0 && only_one_کھڑکی())
	    {
		exarg_T ea;

		CLEAR_میدان(ea);
		ex_quit(&ea);
		أرجع TRUE;
	    }

	    // ++close or term_finish == "close"
	    ch_log(عدم, "terminal job finished, closing window");
	    aucmd_prepbuf(&aco, رکن->tl_بفر);
	    إذا (curbuf == رکن->tl_بفر)
	    {
		win_T	*wp = curwin;

		// Avoid closing the window if we temporarily use it.
		إذا (is_aucmd_win(wp))
		    do_سیٹ_w_locked = TRUE;
		إذا (do_سیٹ_w_locked)
		    ++wp->w_locked;
		do_bufdel(DOBUF_WIPE, (char_u *)"", 1, fnum, fnum, FALSE);
		إذا (do_سیٹ_w_locked)
		    --wp->w_locked;
		aucmd_restbuf(&aco);
	    }
#جے_تعریف_شدہ FEAT_PROP_POPUP
	    إذا (pwin != عدم)
		popup_close_with_retval(pwin, 0);
#جے_انت
	    أرجع TRUE;
	}
	إذا (رکن->tl_انت == TL_انت_OPEN
				   && رکن->tl_بفر->b_nwindows == 0)
	{
	    حرف    *cmd = رکن->tl_opencmd == عدم
				? "botright sbuf %d"
				: (حرف *)رکن->tl_opencmd;
	    size_t  len = لڑی_لمبائی(cmd) + 50;
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

    redraw_buf_and_حالت_later(رکن->tl_بفر, UPD_NOT_VALID);
    أرجع FALSE;
}

#إذا defined(FEAT_PROP_POPUP)
/*
 * If the current window is a terminal in a popup window and the job has
 * finished, close the popup window and to back to the previous window.
 * Otherwise return FAIL.
 */
    صحيح
may_close_رکن_popup(فراغ)
{
    إذا (!popup_is_popup(curwin) || curbuf->b_رکن == عدم
				 || رکن_کم_running_not_none(curbuf->b_رکن))
	أرجع FAIL;

    win_T *pwin = curwin;

    إذا (win_valid(prevwin))
	win_enter(prevwin, FALSE);
    popup_close_with_retval(pwin, 0);
    أرجع OK;
}
#جے_انت

/*
 * Called when a channel is going to be closed, before invoking the close
 * callback.
 */
    فراغ
رکن_channel_closing(channel_T *ch)
{
    رکن_T *رکن;

    لكل (رکن = پہلا_رکن; رکن != عدم; رکن = رکن->tl_اگلا)
	إذا (رکن->tl_کم == ch->ch_کم && !رکن->tl_channel_closed)
	    رکن->tl_channel_closing = TRUE;
}

/*
 * Called when a channel has been closed.
 * If this was a channel for a terminal window then finish it up.
 */
    فراغ
رکن_channel_closed(channel_T *ch)
{
    رکن_T *رکن;
    رکن_T *اگلا_رکن;
    صحيح	    did_one = FALSE;

    لكل (رکن = پہلا_رکن; رکن != عدم; رکن = اگلا_رکن)
    {
	اگلا_رکن = رکن->tl_اگلا;
	إذا (رکن->tl_کم == ch->ch_کم && !رکن->tl_channel_closed)
	{
	    رکن->tl_channel_closed = TRUE;
	    did_one = TRUE;

	    VIM_CLEAR(رکن->tl_title);
	    VIM_CLEAR(رکن->tl_حالت_متن);
#جے_تعریف_شدہ MSWIN
	    إذا (رکن->tl_باہرلا_fd != عدم)
	    {
		fclose(رکن->tl_باہرلا_fd);
		رکن->tl_باہرلا_fd = عدم;
	    }
#جے_انت

	    إذا (updating_screen)
	    {
		// Cannot open or close windows now.  Can happen when
		// 'lazyredraw' is set.
		رکن->tl_channel_recently_closed = TRUE;
		استمر;
	    }

	    إذا (رکن_after_channel_closed(رکن))
		اگلا_رکن = پہلا_رکن;
	}
    }

    إذا (did_one)
    {
	redraw_statuslines();

	// Need to break out of vgetc().
	ins_char_typebuf(K_IGNORE, 0);
	typebuf_was_filled = TRUE;

	رکن = curbuf->b_رکن;
	إذا (رکن != عدم)
	{
	    إذا (رکن->tl_کم == ch->ch_کم)
		maketitle();
	    update_cursor(رکن, رکن->tl_cursor_visible);
	}
    }
}

/*
 * To be called after resetting updating_screen: handle any terminal where the
 * channel was closed.
 */
    فراغ
رکن_check_channel_closed_recently(فراغ)
{
    رکن_T *رکن;
    رکن_T *اگلا_رکن;

    لكل (رکن = پہلا_رکن; رکن != عدم; رکن = اگلا_رکن)
    {
	اگلا_رکن = رکن->tl_اگلا;
	إذا (رکن->tl_channel_recently_closed)
	{
	    رکن->tl_channel_recently_closed = FALSE;
	    إذا (رکن_after_channel_closed(رکن))
		// start over, the list may have changed
		اگلا_رکن = پہلا_رکن;
	}
    }
}

/*
 * Fill one screen line from a line of the terminal.
 * Advances "pos" to past the last column.
 */
    ساكن فراغ
رکن_line2screenline(
	رکن_T		*رکن,
	win_T		*wp,
	VTermScreen	*screen,
	VTermPos	*pos,
	صحيح		max_خانہ)
{
    صحيح off = screen_get_current_سطر_off();

    لكل (pos->خانہ = 0; pos->خانہ < max_خانہ; )
    {
	VTermScreenCell cell;
	صحيح		c;

	إذا (vterm_screen_get_cell(screen, *pos, &cell) == 0)
	    CLEAR_میدان(cell);

	c = cell.chars[0];
	إذا (c == NUL)
	{
	    Screen_سطریں[off] = ' ';
	    إذا (enc_utf8)
		Screen_سطریں_UC[off] = NUL;
	}
	وإلا
	{
	    إذا (enc_utf8)
	    {
		صحيح i;

		// composing chars
		لكل (i = 0; i < Screen_mco
			      && i + 1 < VTERM_MAX_CHARS_فی_CELL; ++i)
		{
		    Screen_سطریں_C[i][off] = cell.chars[i + 1];
		    إذا (cell.chars[i + 1] == 0)
			اكسر;
		}
		إذا (c >= 0x80 || (Screen_mco > 0
					 && Screen_سطریں_C[0][off] != 0))
		{
		    Screen_سطریں[off] = ' ';
		    Screen_سطریں_UC[off] = c;
		}
		وإلا
		{
		    Screen_سطریں[off] = c;
		    Screen_سطریں_UC[off] = NUL;
		}
	    }
#جے_تعریف_شدہ MSWIN
	    وإلا إذا (has_mbyte && c >= 0x80)
	    {
		char_u	mb[MB_MAXBYTES+1];
		WCHAR	wc = c;

		إذا (Wide_Char_منزل_Multi_Byte(GetACP(), 0, &wc, 1,
					       (حرف*)mb, 2, 0, 0) > 1)
		{
		    Screen_سطریں[off] = mb[0];
		    Screen_سطریں[off + 1] = mb[1];
		    cell.چوڑائی = mb_ptr2cells(mb);
		}
		وإلا
		    Screen_سطریں[off] = c;
	    }
#جے_انت
	    وإلا
		// This will only store the lower byte of "c".
		Screen_سطریں[off] = c;
	}
	ScreenAttrs[off] = cell2attr(رکن, wp, &cell.attrs, &cell.fg,
								     &cell.bg);

	++pos->خانہ;
	++off;
	إذا (cell.چوڑائی == 2)
	{
	    // don't set the second byte to NUL for a DBCS encoding, it
	    // has been set above
	    إذا (enc_utf8)
	    {
		Screen_سطریں_UC[off] = NUL;
		Screen_سطریں[off] = NUL;
	    }
	    وإلا إذا (!has_mbyte)
	    {
		// Can't show a double-width character with a single-byte
		// 'encoding', just use a space.
		Screen_سطریں[off] = ' ';
		ScreenAttrs[off] = ScreenAttrs[off - 1];
	    }

	    ++pos->خانہ;
	    ++off;
	}
    }
}

#إذا defined(FEAT_GUI)
    ساكن فراغ
update_system_رکن(رکن_T *رکن)
{
    VTermPos	    pos;
    VTermScreen	    *screen;

    إذا (رکن->tl_vterm == عدم)
	أرجع;
    screen = vterm_obtain_screen(رکن->tl_vterm);

    // Scroll up to make more room for terminal lines if needed.
    // Use the cursor position to determine how much to scroll, because
    // ConPTY may damage all rows on initialization even when most are
    // empty, which would cause unnecessary scrolling.
    طالما (رکن->tl_toprow > 0
		  && (صفاں - رکن->tl_toprow) < رکن->tl_cursor_pos.صف + 1)
    {
	صحيح save_p_ہور = p_ہور;

	p_ہور = FALSE;
	پیغام_صف = صفاں - 1;
	پیغام_puts("\n");
	p_ہور = save_p_ہور;
	--رکن->tl_toprow;
    }

    لكل (pos.صف = رکن->tl_dirty_صف_شروع; pos.صف < رکن->tl_dirty_صف_سرا
						  && pos.صف < صفاں; ++pos.صف)
    {
	إذا (pos.صف < رکن->tl_صفاں)
	{
	    صحيح max_خانہ = MIN(topframe->fr_چوڑائی, رکن->tl_خانے_گنتی);

	    رکن_line2screenline(رکن, عدم, screen, &pos, max_خانہ);
	}
	وإلا
	    pos.خانہ = 0;

	screen_سطر(curwin, رکن->tl_toprow + pos.صف,
		firstwin->w_wincol, pos.خانہ, topframe->fr_چوڑائی, -1, 0);
    }

    رکن->tl_dirty_صف_شروع = MAX_صف;
    رکن->tl_dirty_صف_سرا = 0;
}
#جے_انت

/*
 * Return TRUE if window "wp" is to be redrawn with term_update_window().
 * Returns FALSE when there is no terminal running in this window or it is in
 * Terminal-Normal mode.
 */
    صحيح
رکن_do_update_کھڑکی(win_T *wp)
{
    رکن_T	*رکن = wp->w_بفر->b_رکن;

    أرجع رکن != عدم && رکن->tl_vterm != عدم && !رکن->tl_عام_منوال;
}

/*
 * Called to update a window that contains an active terminal.
 */
    فراغ
رکن_update_کھڑکی(win_T *wp)
{
    رکن_T	*رکن = wp->w_بفر->b_رکن;
    VTerm	*vterm;
    VTermScreen *screen;
    VTermState	*state;
    VTermPos	pos;
    صحيح		صفاں_2, خانے_گنتی;
    صحيح		newrows, newcols;
    صحيح		minsize;
    win_T	*twp;

    vterm = رکن->tl_vterm;
    screen = vterm_obtain_screen(vterm);
    state = vterm_obtain_state(vterm);

    // We use UPD_NOT_VALID on a resize or scroll, redraw everything then.
    // With UPD_SOME_VALID only redraw what was marked dirty.
    إذا (wp->w_redr_type > UPD_SOME_VALID)
    {
	رکن->tl_dirty_صف_شروع = 0;
	رکن->tl_dirty_صف_سرا = MAX_صف;

	إذا (رکن->tl_postponed_scroll > 0
			      && رکن->tl_postponed_scroll < رکن->tl_صفاں / 3)
	    // Scrolling is usually faster than redrawing, when there are only
	    // a few lines to scroll.
	    رکن_scroll_اتے(رکن, 0, رکن->tl_postponed_scroll);
	رکن->tl_postponed_scroll = 0;
    }

    /*
     * If the window was resized a redraw will be triggered and we get here.
     * Adjust the size of the vterm unless 'termwinsize' specifies a fixed size.
     */
    minsize = parse_termwinsize(wp, &صفاں_2, &خانے_گنتی);

    newrows = 99999;
    newcols = 99999;
    لكل (twp = firstwin; ; twp = twp->w_اگلا)
    {
	// Always use curwin, it may be a popup window.
	win_T *wwp = twp == عدم ? curwin : twp;

	// When more than one window shows the same terminal, use the
	// smallest size.
	إذا (wwp->w_بفر == رکن->tl_بفر)
	{
	    newrows = MIN(newrows, wwp->w_اچائی);
	    newcols = MIN(newcols, wwp->w_چوڑائی);
	}
	إذا (twp == عدم)
	    اكسر;
    }
    إذا (newrows == 99999 || newcols == 99999)
	أرجع; // safety exit
    newrows = صفاں_2 == 0 ? newrows : minsize ? MAX(صفاں_2, newrows) : صفاں_2;
    newcols = خانے_گنتی == 0 ? newcols : minsize ? MAX(خانے_گنتی, newcols) : خانے_گنتی;

    // If no cell is visible there is no point in resizing.  Also, vterm can't
    // handle a zero height.
    إذا (newrows == 0 || newcols == 0)
	أرجع;

    إذا (رکن->tl_صفاں != newrows || رکن->tl_خانے_گنتی != newcols)
    {
	رکن->tl_vterm_ناپ_بدلیا = TRUE;
	vterm_سیٹ_ناپ(vterm, newrows, newcols);
	ch_log(رکن->tl_کم->jv_channel, "Resizing terminal to %d lines",
								      newrows);
	رکن_report_winsize(رکن, newrows, newcols);

	// Updating the terminal size will cause the snapshot to be cleared.
	// When not in terminal_loop() we need to restore it.
	إذا (رکن != وچ_terminal_loop)
	    may_move_terminal_منزل_بفر(رکن, FALSE);
    }

    // The cursor may have been moved when resizing.
    vterm_state_get_cursorpos(state, &pos);
    position_cursor(wp, &pos);

    لكل (pos.صف = رکن->tl_dirty_صف_شروع; pos.صف < رکن->tl_dirty_صف_سرا
					  && pos.صف < wp->w_اچائی; ++pos.صف)
    {
	إذا (pos.صف < رکن->tl_صفاں)
	{
	    صحيح max_خانہ = MIN(wp->w_چوڑائی, رکن->tl_خانے_گنتی);

	    رکن_line2screenline(رکن, wp, screen, &pos, max_خانہ);
	}
	وإلا
	    pos.خانہ = 0;

	screen_سطر(wp, wp->w_winrow + pos.صف
#جے_تعریف_شدہ FEAT_MENU
				+ winbar_اچائی(wp)
#جے_انت
				, wp->w_wincol, pos.خانہ, wp->w_چوڑائی, -1,
#جے_تعریف_شدہ FEAT_PROP_POPUP
				popup_is_popup(wp) ? SLF_POPUP :
#جے_انت
				0);
    }
}

/*
 * Called after updating all windows: may reset dirty rows.
 */
    فراغ
رکن_did_update_کھڑکی(win_T *wp)
{
    رکن_T	*رکن = wp->w_بفر->b_رکن;

    إذا (رکن == عدم || رکن->tl_vterm == عدم || رکن->tl_عام_منوال
						|| wp->w_redr_type != 0)
	أرجع;

    رکن->tl_dirty_صف_شروع = MAX_صف;
    رکن->tl_dirty_صف_سرا = 0;
}

/*
 * Return TRUE if "wp" is a terminal window where the job has finished.
 */
    صحيح
رکن_is_مکمل(buf_T *buf)
{
    أرجع buf->b_رکن != عدم && buf->b_رکن->tl_vterm == عدم;
}

/*
 * Return TRUE if "wp" is a terminal window where the job has finished or we
 * are in Terminal-Normal mode, thus we show the buffer contents.
 */
    صحيح
رکن_show_بفر(buf_T *buf)
{
    رکن_T *رکن = buf->b_رکن;

    أرجع رکن != عدم && (رکن->tl_vterm == عدم || رکن->tl_عام_منوال);
}

/*
 * The current buffer is going to be changed.  If there is terminal
 * highlighting remove it now.
 */
    فراغ
رکن_change_وچ_curbuf(فراغ)
{
    رکن_T *رکن = curbuf->b_رکن;

    إذا (!رکن_is_مکمل(curbuf) || رکن->tl_scrollback.ga_len <= 0)
	أرجع;

    free_scrollback(رکن);
    redraw_buf_later(رکن->tl_بفر, UPD_NOT_VALID);

    // The buffer is now like a normal buffer, it cannot be easily
    // abandoned when changed.
    سیٹ_string_option_سدھا((char_u *)"buftype", -1,
	    (char_u *)"", OPT_FREE|OPT_LOCAL, 0);
}

/*
 * Get the screen attribute for a position in the buffer.
 * Use a negative "col" to get the filler background color.
 */
    صحيح
رکن_get_attr(win_T *wp, linenr_T lnum, صحيح خانہ)
{
    buf_T	*buf = wp->w_بفر;
    رکن_T	*رکن = buf->b_رکن;
    sb_سطر_T	*سطر;
    cellattr_T	*cellattr;
    صحيح         sb_سطر = -1;
    صحيح         sb_خانہ = خانہ;

    إذا (رکن->tl_scrollback.ga_len)
	bufline_pos_وچ_scrollback(رکن, lnum, خانہ, &sb_سطر, &sb_خانہ);

    إذا (sb_سطر < 0)
	cellattr = &رکن->tl_default_color;
    وإلا
    {
	سطر = (sb_سطر_T *)رکن->tl_scrollback.ga_انکڑے + sb_سطر;
	إذا (sb_خانہ < 0 || sb_خانہ >= سطر->sb_خانے_گنتی)
	    cellattr = &سطر->sb_fill_attr;
	وإلا
	    cellattr = سطر->sb_cells + sb_خانہ;
    }
    أرجع cell2attr(رکن, wp, &cellattr->attrs, &cellattr->fg, &cellattr->bg);
}

/*
 * Convert a cterm color number 0 - 255 to RGB.
 * This is compatible with xterm.
 */
    ساكن فراغ
cterm_color2vterm(صحيح nr, VTermColor *rgb)
{
    cterm_color2rgb(nr, &rgb->red, &rgb->green, &rgb->blue, &rgb->اشاریہ);
    إذا (rgb->اشاریہ == 0)
	rgb->type = VTERM_COLOR_RGB;
    وإلا
    {
	rgb->type = VTERM_COLOR_اشاریہ_دار;
	--rgb->اشاریہ;
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
	    || gui.وچ_use
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
	    fg->red = (بنانشانی)(rgb >> 16);
	    fg->green = (بنانشانی)(rgb >> 8) & 255;
	    fg->blue = (بنانشانی)rgb & 255;
	    fg->type = VTERM_COLOR_RGB | VTERM_COLOR_DEFAULT_FG;
	}
	وإلا
	    fg->type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_FG;

	إذا (bg_rgb != INVALCOLOR)
	{
	    long_u rgb = GUI_MCH_GET_RGB(bg_rgb);
	    bg->red = (بنانشانی)(rgb >> 16);
	    bg->green = (بنانشانی)(rgb >> 8) & 255;
	    bg->blue = (بنانشانی)rgb & 255;
	    bg->type = VTERM_COLOR_RGB | VTERM_COLOR_DEFAULT_BG;
	}
	وإلا
	    bg->type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_BG;

	أرجع TRUE;
    }
    وإلا
#جے_انت
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
رکن_reset_hlfwin(win_T *wp)
{
    wp->w_رکن_hlfwin.fg.type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_FG;
    wp->w_رکن_hlfwin.bg.type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_BG;
}

/*
 * Cache the color of HLF_WIN.
 */
    فراغ
رکن_update_hlfwin(win_T *wp)
{
    صحيح id = wp->w_hlfwin_id;

    إذا (id == 0 || !get_vterm_color_from_synid(id == -1 ? 0 : id,
		&wp->w_رکن_hlfwin.fg, &wp->w_رکن_hlfwin.bg))
	رکن_reset_hlfwin(wp);
}

/*
 * Called when option 'termguicolors' was set,
 * or when any highlight is changed.
 */
    فراغ
رکن_update_hlfwin_all(فراغ)
{
    win_T	 *wp = عدم;
    صحيح		 did_curwin = FALSE;

    طالما (for_all_windows_and_curwin(&wp, &did_curwin))
	رکن_update_hlfwin(wp);
}

/*
 * Initialize term->tl_default_color from the environment.
 */
    فراغ
رکن_init_default_colors(رکن_T *رکن)
{
    VTermColor	    *fg, *bg;
    صحيح		    fgval, bgval;
    صحيح		    id;

    CLEAR_میدان(رکن->tl_default_color.attrs);
    رکن->tl_default_color.چوڑائی = 1;
    fg = &رکن->tl_default_color.fg;
    bg = &رکن->tl_default_color.bg;

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
    id = رکن_get_highlight_id(رکن, عدم);

    إذا (!get_vterm_color_from_synid(id, fg, bg))
    {
#إذا defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
	صحيح tmp;
#جے_انت

	// In an MS-Windows console we know the normal colors.
	إذا (cterm_عام_fg_color > 0)
	{
	    cterm_color2vterm(cterm_عام_fg_color - 1, fg);
#إذا defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
# ifdef VIMDLL
	    إذا (!gui.وچ_use)
# endif
	    {
		tmp = fg->red;
		fg->red = fg->blue;
		fg->blue = tmp;
	    }
#جے_انت
	}
#جے_تعریف_شدہ FEAT_TERMRESPONSE
	وإلا
	    رکن_get_fg_color(&fg->red, &fg->green, &fg->blue);
#جے_انت

	إذا (cterm_عام_bg_color > 0)
	{
	    cterm_color2vterm(cterm_عام_bg_color - 1, bg);
#إذا defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
# ifdef VIMDLL
	    إذا (!gui.وچ_use)
# endif
	    {
		tmp = fg->red;
		fg->red = fg->blue;
		fg->blue = tmp;
	    }
#جے_انت
	}
#جے_تعریف_شدہ FEAT_TERMRESPONSE
	وإلا
	    رکن_get_bg_color(&bg->red, &bg->green, &bg->blue);
#جے_انت
    }
}

#إذا defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Return TRUE if the user-defined palette (either g:terminal_ansi_colors or the
 * "ansi_colors" argument in term_start()) shall be applied.
 */
    ساكن صحيح
رکن_use_palette(فراغ)
{
    إذا (0
# ifdef FEAT_GUI
	    || gui.وچ_use
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
سیٹ_vterm_palette(VTerm *vterm, long_u *rgb)
{
    صحيح		اشاریہ = 0;
    VTermState	*state = vterm_obtain_state(vterm);

    لكل (; اشاریہ < 16; اشاریہ++)
    {
	VTermColor	color;

	color.type = VTERM_COLOR_RGB;
	color.red = (بنانشانی)(rgb[اشاریہ] >> 16);
	color.green = (بنانشانی)(rgb[اشاریہ] >> 8) & 255;
	color.blue = (بنانشانی)rgb[اشاریہ] & 255;
	color.اشاریہ = 0;
	vterm_state_سیٹ_palette_color(state, اشاریہ, &color);
    }
}

/*
 * Set the ANSI color palette from a list of colors
 */
    ساكن صحيح
سیٹ_ansi_colors_list(VTerm *vterm, list_T *list)
{
    صحيح		n = 0;
    long_u	rgb[16];
    listitem_T	*li;

    لكل (li = list->lv_پہلا; li != عدم && n < 16; li = li->li_اگلا, n++)
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

    سیٹ_vterm_palette(vterm, rgb);

    أرجع OK;
}

/*
 * Initialize the ANSI color palette from g:terminal_ansi_colors[0:15]
 */
    ساكن فراغ
init_vterm_ansi_colors(VTerm *vterm)
{
    dictitem_T	*var = لبھو_var((char_u *)"g:terminal_ansi_colors", عدم, TRUE);

    إذا (var == عدم)
	أرجع;

    إذا (var->di_tv.v_type != VAR_LIST
	    || var->di_tv.vval.v_list == عدم
	    || var->di_tv.vval.v_list->lv_پہلا == &range_list_شے
	    || سیٹ_ansi_colors_list(vterm, var->di_tv.vval.v_list) == FAIL)
	semsg(_(e_invalid_دلیل_str), "g:terminal_ansi_colors");
}
#جے_انت

/*
 * Handles a "drop" command from the job in the terminal.
 * "item" is the file name, "item->li_next" may have options.
 */
    ساكن فراغ
handle_drop_command(listitem_T *شے)
{
    char_u	*fname = tv_get_string(&شے->li_tv);
    listitem_T	*opt_شے = شے->li_اگلا;
    صحيح		bufnr;
    win_T	*wp;
    tabpage_T   *tp;
    exarg_T	ea;
    char_u	*tofree = عدم;

    bufnr = buflist_جوڑو(fname, BLN_LISTED | BLN_NOOPT);
    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	إذا (wp->w_بفر->b_fnum == bufnr)
	{
	    // buffer is in a window already, go there
	    goto_tabpage_win(tp, wp);
	    أرجع;
	}
    }

    CLEAR_میدان(ea);

    إذا (opt_شے != عدم && opt_شے->li_tv.v_type == VAR_DICT
					&& opt_شے->li_tv.vval.v_dict != عدم)
    {
	dict_T *dict = opt_شے->li_tv.vval.v_dict;
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

	إذا (dict_has_کلید(dict, "bin"))
	    ea.force_bin = FORCE_BIN;
	إذا (dict_has_کلید(dict, "binary"))
	    ea.force_bin = FORCE_BIN;
	إذا (dict_has_کلید(dict, "nobin"))
	    ea.force_bin = FORCE_NOBIN;
	إذا (dict_has_کلید(dict, "nobinary"))
	    ea.force_bin = FORCE_NOBIN;
    }

    // open in new window, like ":split fname"
    إذا (ea.cmd == عدم)
	ea.cmd = (char_u *)"split";
    ea.arg = fname;
    ea.cmdidx = CMD_ونڈو;
    ex_splitview(&ea);

    vim_free(tofree);
}

/*
 * Return TRUE if "func" starts with "pat" and "pat" isn't empty.
 */
    ساكن صحيح
is_permitted_رکن_api(char_u *func, char_u *pat)
{
    أرجع pat != عدم && *pat != NUL && STRNICMP(func, pat, STRLEN(pat)) == 0;
}

/*
 * Handles a function call from the job running in a terminal.
 * "item" is the function name, "item->li_next" has the arguments.
 */
    ساكن فراغ
handle_call_command(رکن_T *رکن, channel_T *channel, listitem_T *شے)
{
    char_u	*func;
    typval_T	argvars[2];
    typval_T	rettv;
    funcexe_T	funcexe;

    إذا (شے->li_اگلا == عدم)
    {
	ch_log(channel, "Missing function arguments for call");
	أرجع;
    }
    func = tv_get_string(&شے->li_tv);

    إذا (!is_permitted_رکن_api(func, رکن->tl_api))
    {
	ch_log(channel, "Unpermitted function: %s", func);
	أرجع;
    }

    argvars[0].v_type = VAR_نمبر;
    argvars[0].vval.v_نمبر = رکن->tl_بفر->b_fnum;
    argvars[1] = شے->li_اگلا->li_tv;
    CLEAR_میدان(funcexe);
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
    صحيح       وتھ = 7;  // len of "file://" is 7
    حرف      *pos = (حرف *)gap->ga_انکڑے + وتھ;
    char_u    *new_dir;

    // remove HOSTNAME to get PWD
    طالما (وتھ < (صحيح)gap->ga_len && *pos != '/' )
    {
	++وتھ;
	++pos;
    }

    إذا (وتھ >= (صحيح)gap->ga_len)
    {
	semsg(_(e_failed_منزل_کڈھو_pwd_from_str_check_your_shell_config),
								 gap->ga_انکڑے);
	أرجع;
    }

    new_dir = alloc(gap->ga_len - وتھ + 1);
    url_decode(pos, gap->ga_len-وتھ, new_dir);
    changedir_func(new_dir, TRUE, CDSCOPE_کھڑکی);
    vim_free(new_dir);
}

/*
 * Called by libvterm when it cannot recognize an OSC sequence.
 * We recognize a terminal API command.
 */
    ساكن صحيح
parse_osc(صحيح command, VTermStringFragment frag, فراغ *user)
{
    رکن_T	*رکن = (رکن_T *)user;
    js_read_T	reader;
    typval_T	tv;
    channel_T	*channel = رکن->tl_کم == عدم ? عدم
						    : رکن->tl_کم->jv_channel;
    garray_T	*gap = &رکن->tl_osc_buf;

    // We recognize only OSC 5 1 ; {command} and OSC 7 ; {command}
    إذا (command != 51 && (command != 7 || !p_asd))
	أرجع 0;

    // Concatenate what was received until the final piece is found.
    إذا (ga_grow(gap, (صحيح)frag.len + 1) == FAIL)
    {
	ga_clear(gap);
	أرجع 1;
    }
    mch_memmove((حرف *)gap->ga_انکڑے + gap->ga_len, frag.str, frag.len);
    gap->ga_len += (صحيح)frag.len;
    إذا (!frag.final)
	أرجع 1;

    ((حرف *)gap->ga_انکڑے)[gap->ga_len] = 0;

    إذا (command == 7)
    {
	sync_shell_dir(gap);
	ga_clear(gap);
	أرجع 1;
    }

    reader.js_buf = gap->ga_انکڑے;
    reader.js_fill = عدم;
    reader.js_ورتیا = 0;
    إذا (json_decode(&reader, &tv, 0) == OK
	    && tv.v_type == VAR_LIST
	    && tv.vval.v_list != عدم)
    {
	listitem_T *شے = tv.vval.v_list->lv_پہلا;

	إذا (شے == عدم)
	    ch_log(channel, "Missing command");
	وإلا
	{
	    char_u	*cmd = tv_get_string(&شے->li_tv);

	    // Make sure an invoked command doesn't delete the buffer (and the
	    // terminal) under our fingers.
	    ++رکن->tl_بفر->b_locked;

	    شے = شے->li_اگلا;
	    إذا (شے == عدم)
		ch_log(channel, "Missing argument for %s", cmd);
	    وإلا إذا (STRCMP(cmd, "drop") == 0)
		handle_drop_command(شے);
	    وإلا إذا (STRCMP(cmd, "call") == 0)
		handle_call_command(رکن, channel, شے);
	    وإلا
		ch_log(channel, "Invalid command received: %s", cmd);
	    --رکن->tl_بفر->b_locked;
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
	ثابت حرف  *leader بن_ورتیا,
	ثابت لما  دلیلاں[],
	صحيح	    argcount,
	ثابت حرف  *intermed بن_ورتیا,
	حرف	    command,
	فراغ	    *user)
{
    رکن_T	*رکن = (رکن_T *)user;
    حرف	buf[100];
    صحيح		len;
    صحيح		x = 0;
    صحيح		y = 0;
    win_T	*wp;

    // We recognize only CSI 13 t
    إذا (command != 't' || argcount != 1 || دلیلاں[0] != 13)
	أرجع 0; // not handled

    // When getting the window position is not possible or it fails it results
    // in zero/zero.
#إذا defined(FEAT_GUI) \
	|| (defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)) \
	|| defined(MSWIN)
    (فراغ)ui_get_winpos(&x, &y, (varnumber_T)100);
#جے_انت

    FOR_ALL_WINDOWS(wp)
	إذا (wp->w_بفر == رکن->tl_بفر)
	    اكسر;
    إذا (wp != عدم)
    {
#جے_تعریف_شدہ FEAT_GUI
	إذا (gui.وچ_use)
	{
	    x += wp->w_wincol * gui.char_چوڑائی;
	    y += W_WINROW(wp) * gui.char_اچائی;
	}
	وإلا
#جے_انت
	{
	    // We roughly estimate the position of the terminal window inside
	    // the Vim window by assuming a 10 x 7 character cell.
	    x += wp->w_wincol * 7;
	    y += W_WINROW(wp) * 10;
	}
    }

    len = vim_snprintf(buf, 100, "\x1b[3;%d;%dt", x, y);
    channel_send(رکن->tl_کم->jv_channel, get_tty_part(رکن),
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
vterm_malloc(size_t ناپ, فراغ *انکڑے بن_ورتیا)
{
    // make sure that the length is not zero
    أرجع alloc_clear(ناپ == 0 ? 1L : ناپ);
}

    ساكن فراغ
vterm_memfree(فراغ *ptr, فراغ *انکڑے بن_ورتیا)
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
create_vterm(رکن_T *رکن, صحيح صفاں_2, صحيح خانے_گنتی)
{
    VTerm	    *vterm;
    VTermScreen	    *screen;
    VTermState	    *state;
    VTerm_قدر	    قدر;

    vterm = vterm_new_with_allocator(صفاں_2, خانے_گنتی, &vterm_allocator, عدم);
    رکن->tl_vterm = vterm;
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

    vterm_screen_سیٹ_callbacks(screen, &screen_callbacks, رکن);
    vterm_screen_سیٹ_damage_ملاؤ(screen, VTERM_DAMAGE_SCROLL);
    vterm_screen_callbacks_has_pushline4(screen);
    // TODO: depends on 'encoding'.
    vterm_سیٹ_utf8(vterm, 1);

    رکن_init_default_colors(رکن);

    vterm_state_سیٹ_default_colors(
	    state,
	    &رکن->tl_default_color.fg,
	    &رکن->tl_default_color.bg);

    إذا (t_colors < 16)
	// Less than 16 colors: assume that bold means using a bright color for
	// the foreground color.
	vterm_state_سیٹ_bold_highbright(vterm_obtain_state(vterm), 1);

    // Required to initialize most things.
    vterm_screen_reset(screen, 1 /* hard */);

    // Allow using alternate screen.
    vterm_screen_enable_altscreen(screen, 1);

    // For unix do not use a blinking cursor.  In an xterm this causes the
    // cursor to blink if it's blinking in the xterm.
    // For Windows we respect the system wide setting.
#جے_تعریف_شدہ MSWIN
    إذا (Get_Caret_Blink_ویلا() == INFINITE)
	قدر.boolean = 0;
    وإلا
	قدر.boolean = 1;
#وإلا
    قدر.boolean = 0;
#جے_انت
    vterm_state_سیٹ_termprop(state, VTERM_PROP_CURSORBLINK, &قدر);
    vterm_state_سیٹ_unrecognised_fallbacks(state, &state_fallbacks, رکن);

    أرجع OK;
}

/*
 * Reset the terminal palette to its default value.
 */
    ساكن فراغ
رکن_reset_palette(VTerm *vterm)
{
    VTermState	*state = vterm_obtain_state(vterm);
    صحيح		اشاریہ;

    لكل (اشاریہ = 0; اشاریہ < 16; اشاریہ++)
    {
	VTermColor	color;

	color.type = VTERM_COLOR_اشاریہ_دار;
	ansi_color2rgb(اشاریہ, &color.red, &color.green, &color.blue,
		&color.اشاریہ);
	// The first valid index starts at 1.
	color.اشاریہ -= 1;

	vterm_state_سیٹ_palette_color(state, اشاریہ, &color);
    }
}

    ساكن فراغ
رکن_update_palette(رکن_T *رکن)
{
#إذا defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    إذا (رکن_use_palette()
	    && (رکن->tl_palette != عدم
		|| لبھو_var((char_u *)"g:terminal_ansi_colors", عدم, TRUE)
								      != عدم))
    {
	إذا (رکن->tl_palette != عدم)
	    سیٹ_vterm_palette(رکن->tl_vterm, رکن->tl_palette);
	وإلا
	    init_vterm_ansi_colors(رکن->tl_vterm);
    }
    وإلا
#جے_انت
	رکن_reset_palette(رکن->tl_vterm);
}

/*
 * Called when option 'termguicolors' is changed.
 */
    فراغ
رکن_update_palette_all(فراغ)
{
    رکن_T *رکن;

    FOR_ALL_TERMS(رکن)
    {
	إذا (رکن->tl_vterm == عدم)
	    استمر;
	رکن_update_palette(رکن);
    }
}

/*
 * Called when option 'background' or 'termguicolors' was set,
 * or when any highlight is changed.
 */
    فراغ
رکن_update_colors_all(فراغ)
{
    رکن_T *رکن;

    FOR_ALL_TERMS(رکن)
    {
	إذا (رکن->tl_vterm == عدم)
	    استمر;
	رکن_init_default_colors(رکن);
	vterm_state_سیٹ_default_colors(
		vterm_obtain_state(رکن->tl_vterm),
		&رکن->tl_default_color.fg,
		&رکن->tl_default_color.bg);
    }
}

/*
 * Return the text to show for the buffer name and status.
 */
    char_u *
رکن_get_حالت_متن(رکن_T *رکن)
{
    إذا (رکن->tl_حالت_متن != عدم)
	أرجع رکن->tl_حالت_متن;

    char_u *txt;
    size_t len;
    char_u *fname;

    إذا (رکن->tl_عام_منوال)
    {
	إذا (رکن_کم_running(رکن))
	    txt = (char_u *)_("Terminal");
	وإلا
	    txt = (char_u *)_("Terminal-finished");
    }
    وإلا إذا (رکن->tl_title != عدم)
	txt = رکن->tl_title;
    وإلا إذا (رکن_none_open(رکن))
	txt = (char_u *)_("active");
    وإلا إذا (رکن_کم_running(رکن))
	txt = (char_u *)_("running");
    وإلا
	txt = (char_u *)_("finished");
    fname = buf_get_fname(رکن->tl_بفر);
    len = 9 + STRLEN(fname) + STRLEN(txt);
    رکن->tl_حالت_متن = alloc(len);
    إذا (رکن->tl_حالت_متن != عدم)
	vim_snprintf((حرف *)رکن->tl_حالت_متن, len, "%s [%s]",
		fname, txt);
    أرجع رکن->tl_حالت_متن;
}

/*
 * Clear the cached value of the status text.
 */
    فراغ
رکن_clear_حالت_متن(رکن_T *رکن)
{
    VIM_CLEAR(رکن->tl_حالت_متن);
}

/*
 * Mark references in jobs of terminals.
 */
    صحيح
سیٹ_ref_وچ_رکن(صحيح copyID)
{
    صحيح		abort = FALSE;
    رکن_T	*رکن;
    typval_T	tv;

    لكل (رکن = پہلا_رکن; !abort && رکن != عدم; رکن = رکن->tl_اگلا)
	إذا (رکن->tl_کم != عدم)
	{
	    tv.v_type = VAR_کم;
	    tv.vval.v_کم = رکن->tl_کم;
	    abort = abort || سیٹ_ref_وچ_شے(&tv, copyID, عدم, عدم, عدم);
	}
    أرجع abort;
}

/*
 * Get the buffer from the first argument in "argvars".
 * Returns NULL when the buffer is not for a terminal window and logs a message
 * with "where".
 */
    ساكن buf_T *
رکن_get_buf(typval_T *argvars, حرف *where)
{
    buf_T *buf;

    ++emsg_off;
    buf = tv_get_buf(&argvars[0], FALSE);
    --emsg_off;
    إذا (buf == عدم || buf->b_رکن == عدم)
    {
	(فراغ)tv_get_نمبر(&argvars[0]);    // issue errmsg if type error
	ch_log(عدم, "%s: invalid buffer argument", where);
	أرجع عدم;
    }
    أرجع buf;
}

    ساكن فراغ
clear_cell(VTermScreenCell *cell)
{
    CLEAR_میدان(*cell);
    cell->fg.type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_FG;
    cell->bg.type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_BG;
}

    ساكن فراغ
dump_رکن_color(FILE *fd, VTermColor *color)
{
    صحيح اشاریہ;

    إذا (VTERM_COLOR_IS_اشاریہ_دار(color))
	اشاریہ = color->اشاریہ + 1;
    وإلا إذا (color->type == 0)
	// use RGB values
	اشاریہ = 255;
    وإلا
	// default color
	اشاریہ = 0;
    fprintf(fd, "%02x%02x%02x%d",
	    (صحيح)color->red, (صحيح)color->green, (صحيح)color->blue,
	    اشاریہ);
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
f_رکن_dumpwrite(typval_T *argvars, typval_T *rettv بن_ورتیا)
{
    buf_T	*buf;
    رکن_T	*رکن;
    char_u	*fname;
    صحيح		max_اچائی_2 = 0;
    صحيح		max_چوڑائی_2 = 0;
    stat_T	st;
    FILE	*fd;
    VTermPos	pos;
    VTermScreen *screen;
    VTermScreenCell prev_cell;
    VTermState	*state;
    VTermPos	cursor_pos;

    إذا (check_restricted() || check_secure())
	أرجع;

    إذا (وچ_vim9script()
	    && (check_for_بفر_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	أرجع;

    buf = رکن_get_buf(argvars, "term_dumpwrite()");
    إذا (buf == عدم)
	أرجع;
    رکن = buf->b_رکن;
    إذا (رکن->tl_vterm == عدم)
    {
	emsg(_(e_کم_already_مکمل));
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
	    max_اچائی_2 = dict_get_نمبر(d, "rows");
	    max_چوڑائی_2 = dict_get_نمبر(d, "columns");
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

    screen = vterm_obtain_screen(رکن->tl_vterm);
    state = vterm_obtain_state(رکن->tl_vterm);
    vterm_state_get_cursorpos(state, &cursor_pos);

    لكل (pos.صف = 0; (max_اچائی_2 == 0 || pos.صف < max_اچائی_2)
					 && pos.صف < رکن->tl_صفاں; ++pos.صف)
    {
	صحيح		repeat = 0;

	لكل (pos.خانہ = 0; (max_چوڑائی_2 == 0 || pos.خانہ < max_چوڑائی_2)
					 && pos.خانہ < رکن->tl_خانے_گنتی; ++pos.خانہ)
	{
	    VTermScreenCell cell;
	    صحيح		    same_attr;
	    صحيح		    same_chars = TRUE;
	    صحيح		    i;
	    صحيح		    is_cursor_pos = (pos.خانہ == cursor_pos.خانہ
						 && pos.صف == cursor_pos.صف);

	    إذا (vterm_screen_get_cell(screen, pos, &cell) == 0)
		clear_cell(&cell);

	    لكل (i = 0; i < VTERM_MAX_CHARS_فی_CELL; ++i)
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
	    إذا (same_chars && cell.چوڑائی == prev_cell.چوڑائی && same_attr
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

		    لكل (i = 0; i < VTERM_MAX_CHARS_فی_CELL
						  && cell.chars[i] != NUL; ++i)
		    {
			len = utf_char2bytes(cell.chars[i], charbuf);
			fwrite(charbuf, len, 1, fd);
		    }
		}

		// When only the characters differ we don't write anything, the
		// following "|", "@" or NL will indicate using the same
		// attributes.
		إذا (cell.چوڑائی != prev_cell.چوڑائی || !same_attr)
		{
		    إذا (cell.چوڑائی == 2)
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
			    dump_رکن_color(fd, &cell.fg);
			}
			إذا (vterm_color_is_equal(&cell.bg, &prev_cell.bg))
			    fputs("&", fd);
			وإلا
			{
			    fputs("#", fd);
			    dump_رکن_color(fd, &cell.bg);
			}
		    }
		}

		prev_cell = cell;
	    }

	    إذا (cell.چوڑائی == 2)
		++pos.خانہ;
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

    *(((cellattr_T *)gap->ga_انکڑے) + gap->ga_len) = *cell;
    ++gap->ga_len;
}

    ساكن فراغ
clear_cellattr(cellattr_T *cell)
{
    CLEAR_میدان(*cell);
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
    cellattr_T	    خالی_تھاواں_cell;
    رکن_T	    *رکن = curbuf->b_رکن;
    صحيح		    max_cells = 0;
    صحيح		    شروع_صف = رکن->tl_scrollback.ga_len;

    ga_init2(&ga_متن, 1, 90);
    ga_init2(&ga_cell, حجم(cellattr_T), 90);
    clear_cellattr(&cell);
    clear_cellattr(&خالی_تھاواں_cell);
    cursor_pos->صف = -1;
    cursor_pos->خانہ = -1;

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
	    إذا (ga_متن.ga_انکڑے == عدم)
		dump_is_corrupt(&ga_متن);
	    إذا (ga_grow(&رکن->tl_scrollback, 1) == OK)
	    {
		sb_سطر_T   *سطر = (sb_سطر_T *)رکن->tl_scrollback.ga_انکڑے
						  + رکن->tl_scrollback.ga_len;

		إذا (max_cells < ga_cell.ga_len)
		    max_cells = ga_cell.ga_len;
		سطر->sb_خانے_گنتی = ga_cell.ga_len;
		سطر->sb_بائٹاں = ga_متن.ga_len;
		سطر->sb_cells = ga_cell.ga_انکڑے;
		سطر->sb_fill_attr = رکن->tl_default_color;
		سطر->continuation = 0;
		++رکن->tl_scrollback.ga_len;
		ga_init(&ga_cell);

		ga_append(&ga_متن, NUL);
		ml_append(curbuf->b_ml.ml_سطر_گنتی, ga_متن.ga_انکڑے,
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
		cursor_pos->صف = رکن->tl_scrollback.ga_len - شروع_صف;
		cursor_pos->خانہ = ga_cell.ga_len;
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
	    إذا (ga_متن.ga_انکڑے != عدم)
	    {
		prev_char.لمائی = (size_t)(ga_متن.ga_len - prev_len);
		prev_char.string = vim_strnsave(
		    ((char_u *)ga_متن.ga_انکڑے) + prev_len,
		    prev_char.لمائی);
		إذا (prev_char.string == عدم)
		    prev_char.لمائی = 0;
	    }

	    إذا (c == '@' || c == '|' || c == '>' || c == '\n')
	    {
		// use all attributes from previous cell
	    }
	    وإلا إذا (c == '+' || c == '*')
	    {
		صحيح is_bg;

		cell.چوڑائی = c == '+' ? 1 : 2;

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
			    صحيح red, green, blue, اشاریہ = 0, type;

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
				اشاریہ = اشاریہ * 10 + (c - '0');
				c = fgetc(fd);
			    }
			    إذا (اشاریہ == 0 || اشاریہ == 255)
			    {
				type = VTERM_COLOR_RGB;
				إذا (اشاریہ == 0)
				{
				    إذا (is_bg)
					type |= VTERM_COLOR_DEFAULT_BG;
				    وإلا
					type |= VTERM_COLOR_DEFAULT_FG;
				}
			    }
			    وإلا
			    {
				type = VTERM_COLOR_اشاریہ_دار;
				اشاریہ -= 1;
			    }
			    إذا (is_bg)
			    {
				cell.bg.type = type;
				cell.bg.red = red;
				cell.bg.green = green;
				cell.bg.blue = blue;
				cell.bg.اشاریہ = اشاریہ;
			    }
			    وإلا
			    {
				cell.fg.type = type;
				cell.fg.red = red;
				cell.fg.green = green;
				cell.fg.blue = blue;
				cell.fg.اشاریہ = اشاریہ;
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
	    إذا (cell.چوڑائی == 2)
		append_cell(&ga_cell, &خالی_تھاواں_cell);
	}
	وإلا إذا (c == '@')
	{
	    إذا (prev_char.string == عدم)
		dump_is_corrupt(&ga_متن);
	    وإلا
	    {
		صحيح گنتی = 0;

		// repeat previous character, get the count
		لكل (;;)
		{
		    c = fgetc(fd);
		    إذا (!محفوظ_isdigit(c))
			اكسر;
		    گنتی = گنتی * 10 + (c - '0');
		}

		طالما (گنتی-- > 0)
		{
		    ga_concat_len(&ga_متن, prev_char.string, prev_char.لمائی);
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
	ml_append(curbuf->b_ml.ml_سطر_گنتی, ga_متن.ga_انکڑے,
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
get_separator(صحيح متن_چوڑائی, char_u *fname)
{
    صحيح	    چوڑائی = MAX(متن_چوڑائی, curwin->w_چوڑائی);
    char_u  *textline;
    صحيح	    fname_ناپ;
    char_u  *p = fname;
    صحيح	    i;
    size_t  off;

    textline = alloc(چوڑائی + STRLEN(fname) + 1);
    إذا (textline == عدم)
	أرجع عدم;

    fname_ناپ = vim_strsize(fname);
    إذا (fname_ناپ < چوڑائی - 8)
    {
	// enough room, don't use the full window width
	چوڑائی = MAX(متن_چوڑائی, fname_ناپ + 8);
    }
    وإلا إذا (fname_ناپ > چوڑائی - 8)
    {
	// full name doesn't fit, use only the tail
	p = gettail(fname);
	fname_ناپ = vim_strsize(p);
    }
    // skip characters until the name fits
    طالما (fname_ناپ > چوڑائی - 8)
    {
	p += (*mb_ptr2len)(p);
	fname_ناپ = vim_strsize(p);
    }

    لكل (i = 0; i < (چوڑائی - fname_ناپ) / 2 - 1; ++i)
	textline[i] = '=';
    textline[i++] = ' ';

    STRCPY(textline + i, p);
    off = STRLEN(textline);
    textline[off] = ' ';
    لكل (i = 1; i < (چوڑائی - fname_ناپ) / 2; ++i)
	textline[off + i] = '=';
    textline[off + i] = NUL;

    أرجع textline;
}

/*
 * Common for "term_dumpdiff()" and "term_dumpload()".
 */
    ساكن فراغ
رکن_لدو_dump(typval_T *argvars, typval_T *rettv, صحيح do_diff)
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
	emsg(_(e_invalid_دلیل));
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

    init_کم_options(&opt);
    إذا (argvars[do_diff ? 2 : 1].v_type != VAR_UNKNOWN
	    && get_کم_options(&argvars[do_diff ? 2 : 1], &opt, 0,
		    JO2_رکن_NAME + JO2_رکن_خانے_گنتی + JO2_رکن_صفاں
		    + JO2_VERTICAL + JO2_CURWIN + JO2_NORESTORE) == FAIL)
	اذهب theend;

    إذا (opt.jo_رکن_name == عدم)
    {
	size_t len = STRLEN(fname1) + 12;

	fname_tofree = alloc(len);
	إذا (fname_tofree != عدم)
	{
	    vim_snprintf((حرف *)fname_tofree, len, "dump diff %s", fname1);
	    opt.jo_رکن_name = fname_tofree;
	}
    }

    إذا (opt.jo_bufnr_buf != عدم)
    {
	win_T *wp = buf_jump_open_win(opt.jo_bufnr_buf);

	// With "bufnr" argument: enter the window with this buffer and make it
	// empty.
	إذا (wp == عدم)
	    semsg(_(e_invalid_دلیل_str), "bufnr");
	وإلا
	{
	    buf = curbuf;
	    طالما (!(curbuf->b_ml.ml_flags & ML_خالی_تھاواں))
		ml_delete((linenr_T)1);
	    free_scrollback(curbuf->b_رکن);
	    redraw_later(UPD_NOT_VALID);
	}
    }
    وإلا
	// Create a new terminal window.
	buf = رکن_شروع(&argvars[0], عدم, &opt, رکن_شروع_NOJOB);

    إذا (buf != عدم && buf->b_رکن != عدم)
    {
	صحيح		i;
	linenr_T	bot_lnum;
	linenr_T	lnum;
	رکن_T		*رکن = buf->b_رکن;
	صحيح		چوڑائی;
	صحيح		width2;
	VTermPos	cursor_pos1;
	VTermPos	cursor_pos2;

	رکن_init_default_colors(رکن);

	rettv->vval.v_نمبر = buf->b_fnum;

	// read the files, fill the buffer with the diff
	چوڑائی = read_dump_file(fd1, &cursor_pos1);

	// position the cursor
	إذا (cursor_pos1.صف >= 0)
	{
	    curwin->w_cursor.lnum = cursor_pos1.صف + 1;
	    coladvance(cursor_pos1.خانہ);
	}

	// Delete the empty line that was in the empty buffer.
	ml_delete(1);

	// For term_dumpload() we are done here.
	إذا (!do_diff)
	    اذهب theend;

	رکن->tl_چوٹی_diff_صفاں = curbuf->b_ml.ml_سطر_گنتی;

	textline = get_separator(چوڑائی, fname1);
	إذا (textline == عدم)
	    اذهب theend;
	إذا (جوڑو_خالی_تھاواں_scrollback(رکن, &رکن->tl_default_color, 0) == OK)
	    ml_append(curbuf->b_ml.ml_سطر_گنتی, textline, 0, FALSE);
	vim_free(textline);

	textline = get_separator(چوڑائی, fname2);
	إذا (textline == عدم)
	    اذهب theend;
	إذا (جوڑو_خالی_تھاواں_scrollback(رکن, &رکن->tl_default_color, 0) == OK)
	    ml_append(curbuf->b_ml.ml_سطر_گنتی, textline, 0, FALSE);
	textline[چوڑائی] = NUL;

	bot_lnum = curbuf->b_ml.ml_سطر_گنتی;
	width2 = read_dump_file(fd2, &cursor_pos2);
	إذا (width2 > چوڑائی)
	{
	    vim_free(textline);
	    textline = alloc(width2 + 1);
	    إذا (textline == عدم)
		اذهب theend;
	    چوڑائی = width2;
	    textline[چوڑائی] = NUL;
	}
	رکن->tl_bot_diff_صفاں = curbuf->b_ml.ml_سطر_گنتی - bot_lnum;

	لكل (lnum = 1; lnum <= رکن->tl_چوٹی_diff_صفاں; ++lnum)
	{
	    إذا (lnum + bot_lnum > curbuf->b_ml.ml_سطر_گنتی)
	    {
		// bottom part has fewer rows, fill with "-"
		لكل (i = 0; i < چوڑائی; ++i)
		    textline[i] = '-';
	    }
	    وإلا
	    {
		char_u *line1;
		char_u *line2;
		char_u *p1;
		char_u *p2;
		صحيح	خانہ;
		sb_سطر_T   *sb_سطر = (sb_سطر_T *)رکن->tl_scrollback.ga_انکڑے;
		cellattr_T *cellattr1 = (sb_سطر + lnum - 1)->sb_cells;
		cellattr_T *cellattr2 = (sb_سطر + lnum + bot_lnum - 1)
								    ->sb_cells;

		// Make a copy, getting the second line will invalidate it.
		line1 = vim_strsave(ml_get(lnum));
		إذا (line1 == عدم)
		    اكسر;
		p1 = line1;

		line2 = ml_get(lnum + bot_lnum);
		p2 = line2;
		لكل (خانہ = 0; خانہ < چوڑائی && *p1 != NUL && *p2 != NUL; ++خانہ)
		{
		    صحيح len1 = utfc_ptr2len(p1);
		    صحيح len2 = utfc_ptr2len(p2);

		    textline[خانہ] = ' ';
		    إذا (len1 != len2 || STRNCMP(p1, p2, len1) != 0)
			// text differs
			textline[خانہ] = 'X';
		    وإلا إذا (lnum == cursor_pos1.صف + 1
			    && خانہ == cursor_pos1.خانہ
			    && (cursor_pos1.صف != cursor_pos2.صف
					|| cursor_pos1.خانہ != cursor_pos2.خانہ))
			// cursor in first but not in second
			textline[خانہ] = '>';
		    وإلا إذا (lnum == cursor_pos2.صف + 1
			    && خانہ == cursor_pos2.خانہ
			    && (cursor_pos1.صف != cursor_pos2.صف
					|| cursor_pos1.خانہ != cursor_pos2.خانہ))
			// cursor in second but not in first
			textline[خانہ] = '<';
		    وإلا إذا (cellattr1 != عدم && cellattr2 != عدم)
		    {
			إذا ((cellattr1 + خانہ)->چوڑائی
						   != (cellattr2 + خانہ)->چوڑائی)
			    textline[خانہ] = 'w';
			وإلا إذا (!vterm_color_is_equal(&(cellattr1 + خانہ)->fg,
						   &(cellattr2 + خانہ)->fg))
			    textline[خانہ] = 'f';
			وإلا إذا (!vterm_color_is_equal(&(cellattr1 + خانہ)->bg,
						   &(cellattr2 + خانہ)->bg))
			    textline[خانہ] = 'b';
			وإلا إذا (vtermAttr2hl(&(cellattr1 + خانہ)->attrs)
				  != vtermAttr2hl(&((cellattr2 + خانہ)->attrs)))
			    textline[خانہ] = 'a';
		    }
		    p1 += len1;
		    p2 += len2;
		    // TODO: handle different width
		}

		طالما (خانہ < چوڑائی)
		{
		    إذا (*p1 == NUL && *p2 == NUL)
			textline[خانہ] = '?';
		    وإلا إذا (*p1 == NUL)
		    {
			textline[خانہ] = '+';
			p2 += utfc_ptr2len(p2);
		    }
		    وإلا
		    {
			textline[خانہ] = '-';
			p1 += utfc_ptr2len(p1);
		    }
		    ++خانہ;
		}

		vim_free(line1);
	    }
	    إذا (جوڑو_خالی_تھاواں_scrollback(رکن, &رکن->tl_default_color,
						 رکن->tl_چوٹی_diff_صفاں) == OK)
		ml_append(رکن->tl_چوٹی_diff_صفاں + lnum, textline, 0, FALSE);
	    ++bot_lnum;
	}

	طالما (lnum + bot_lnum <= curbuf->b_ml.ml_سطر_گنتی)
	{
	    // bottom part has more rows, fill with "+"
	    لكل (i = 0; i < چوڑائی; ++i)
		textline[i] = '+';
	    إذا (جوڑو_خالی_تھاواں_scrollback(رکن, &رکن->tl_default_color,
						 رکن->tl_چوٹی_diff_صفاں) == OK)
		ml_append(رکن->tl_چوٹی_diff_صفاں + lnum, textline, 0, FALSE);
	    ++lnum;
	    ++bot_lnum;
	}

	رکن->tl_خانے_گنتی = چوڑائی;

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
رکن_وٹاندرا_diff(فراغ)
{
    رکن_T	*رکن = curbuf->b_رکن;
    linenr_T	سطر_گنتی;
    linenr_T	چوٹی_صفاں;
    linenr_T	bot_صفاں;
    linenr_T	bot_شروع;
    linenr_T	lnum;
    char_u	*p;
    sb_سطر_T	*sb_سطر;

    إذا (رکن == عدم
	    || !رکن_is_مکمل(curbuf)
	    || رکن->tl_چوٹی_diff_صفاں == 0
	    || رکن->tl_scrollback.ga_len == 0)
	أرجع FAIL;

    سطر_گنتی = curbuf->b_ml.ml_سطر_گنتی;
    چوٹی_صفاں = رکن->tl_چوٹی_diff_صفاں;
    bot_صفاں = رکن->tl_bot_diff_صفاں;
    bot_شروع = سطر_گنتی - bot_صفاں;
    sb_سطر = (sb_سطر_T *)رکن->tl_scrollback.ga_انکڑے;

    // move lines from top to above the bottom part
    لكل (lnum = 1; lnum <= چوٹی_صفاں; ++lnum)
    {
	p = vim_strsave(ml_get(1));
	إذا (p == عدم)
	    أرجع OK;
	ml_append(bot_شروع, p, 0, FALSE);
	ml_delete(1);
	vim_free(p);
    }

    // move lines from bottom to the top
    لكل (lnum = 1; lnum <= bot_صفاں; ++lnum)
    {
	p = vim_strsave(ml_get(bot_شروع + lnum));
	إذا (p == عدم)
	    أرجع OK;
	ml_delete(bot_شروع + lnum);
	ml_append(lnum - 1, p, 0, FALSE);
	vim_free(p);
    }

    // move top title to bottom
    p = vim_strsave(ml_get(bot_صفاں + 1));
    إذا (p == عدم)
	أرجع OK;
    ml_append(سطر_گنتی - چوٹی_صفاں - 1, p, 0, FALSE);
    ml_delete(bot_صفاں + 1);
    vim_free(p);

    // move bottom title to top
    p = vim_strsave(ml_get(سطر_گنتی - چوٹی_صفاں));
    إذا (p == عدم)
	أرجع OK;
    ml_delete(سطر_گنتی - چوٹی_صفاں);
    ml_append(bot_صفاں, p, 0, FALSE);
    vim_free(p);

    إذا (چوٹی_صفاں == bot_صفاں)
    {
	// rows counts are equal, can swap cell properties
	لكل (lnum = 0; lnum < چوٹی_صفاں; ++lnum)
	{
	    sb_سطر_T	عارضی;

	    عارضی = *(sb_سطر + lnum);
	    *(sb_سطر + lnum) = *(sb_سطر + bot_شروع + lnum);
	    *(sb_سطر + bot_شروع + lnum) = عارضی;
	}
    }
    وإلا
    {
	size_t		ناپ = حجم(sb_سطر_T) * رکن->tl_scrollback.ga_len;
	sb_سطر_T	*عارضی = alloc(ناپ);

	// need to copy cell properties into temp memory
	إذا (عارضی != عدم)
	{
	    mch_memmove(عارضی, رکن->tl_scrollback.ga_انکڑے, ناپ);
	    mch_memmove(رکن->tl_scrollback.ga_انکڑے,
		    عارضی + bot_شروع,
		    حجم(sb_سطر_T) * bot_صفاں);
	    mch_memmove((sb_سطر_T *)رکن->tl_scrollback.ga_انکڑے + bot_صفاں,
		    عارضی + چوٹی_صفاں,
		    حجم(sb_سطر_T) * (سطر_گنتی - چوٹی_صفاں - bot_صفاں));
	    mch_memmove((sb_سطر_T *)رکن->tl_scrollback.ga_انکڑے
						       + سطر_گنتی - چوٹی_صفاں,
		    عارضی,
		    حجم(sb_سطر_T) * چوٹی_صفاں);
	    vim_free(عارضی);
	}
    }

    رکن->tl_چوٹی_diff_صفاں = bot_صفاں;
    رکن->tl_bot_diff_صفاں = چوٹی_صفاں;

    update_screen(UPD_NOT_VALID);
    أرجع OK;
}

/*
 * "term_dumpdiff(filename, filename, options)" function
 */
    فراغ
f_رکن_dumpdiff(typval_T *argvars, typval_T *rettv)
{
    إذا (وچ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	أرجع;

    رکن_لدو_dump(argvars, rettv, TRUE);
}

/*
 * "term_dumpload(filename, options)" function
 */
    فراغ
f_رکن_dumpload(typval_T *argvars, typval_T *rettv)
{
    إذا (وچ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	أرجع;

    رکن_لدو_dump(argvars, rettv, FALSE);
}

/*
 * "term_getaltscreen(buf)" function
 */
    فراغ
f_رکن_getaltscreen(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    إذا (وچ_vim9script() && check_for_بفر_arg(argvars, 0) == FAIL)
	أرجع;

    buf = رکن_get_buf(argvars, "term_getaltscreen()");
    إذا (buf == عدم)
	أرجع;
    rettv->vval.v_نمبر = buf->b_رکن->tl_using_altscreen;
}

/*
 * "term_getattr(attr, name)" function
 */
    فراغ
f_رکن_getattr(typval_T *argvars, typval_T *rettv)
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

    إذا (وچ_vim9script()
	    && (check_for_نمبر_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	أرجع;

    attr = tv_get_نمبر(&argvars[0]);
    name = tv_get_string_chk(&argvars[1]);
    إذا (name == عدم)
	أرجع;

    إذا (attr > HL_ALL)
	attr = syn_attr2attr(attr);
    لكل (i = 0; i < ARRAY_لمائی(attrs); ++i)
	إذا (STRCMP(name, attrs[i].name) == 0)
	{
	    rettv->vval.v_نمبر = (attr & attrs[i].attr) != 0 ? 1 : 0;
	    اكسر;
	}
}

/*
 * "term_getcursor(buf)" function
 */
    فراغ
f_رکن_getcursor(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    رکن_T	*رکن;
    list_T	*l;
    dict_T	*d;

    إذا (rettv_list_alloc(rettv) == FAIL)
	أرجع;

    إذا (وچ_vim9script() && check_for_بفر_arg(argvars, 0) == FAIL)
	أرجع;

    buf = رکن_get_buf(argvars, "term_getcursor()");
    إذا (buf == عدم)
	أرجع;
    رکن = buf->b_رکن;

    l = rettv->vval.v_list;
    list_append_نمبر(l, رکن->tl_cursor_pos.صف + 1);
    list_append_نمبر(l, رکن->tl_cursor_pos.خانہ + 1);

    d = dict_alloc();
    إذا (d == عدم)
	أرجع;

    dict_جوڑو_نمبر(d, "visible", رکن->tl_cursor_visible);
    dict_جوڑو_نمبر(d, "blink", blink_state_is_inverted()
	    ? !رکن->tl_cursor_blink : رکن->tl_cursor_blink);
    dict_جوڑو_نمبر(d, "shape", رکن->tl_cursor_shape);
    dict_جوڑو_string(d, "color", cursor_color_get(رکن->tl_cursor_color));
    إذا (list_append_dict(l, d) == FAIL)
	dict_unref(d);
}

/*
 * "term_getjob(buf)" function
 */
    فراغ
f_رکن_getjob(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    إذا (وچ_vim9script() && check_for_بفر_arg(argvars, 0) == FAIL)
	أرجع;

    buf = رکن_get_buf(argvars, "term_getjob()");
    إذا (buf == عدم)
    {
	إذا (وچ_vim9script())
	{
	    rettv->v_type = VAR_کم;
	    rettv->vval.v_کم = عدم;
	}
	وإلا
	{
	    rettv->v_type = VAR_SPECIAL;
	    rettv->vval.v_نمبر = VVAL_NULL;
	}
	أرجع;
    }

    rettv->v_type = VAR_کم;
    rettv->vval.v_کم = buf->b_رکن->tl_کم;
    إذا (rettv->vval.v_کم != عدم)
	++rettv->vval.v_کم->jv_refcount;
}

    ساكن صحيح
get_صف_نمبر(typval_T *tv, رکن_T *رکن)
{
    إذا (tv->v_type == VAR_STRING
	    && tv->vval.v_string != عدم
	    && STRCMP(tv->vval.v_string, ".") == 0)
	أرجع رکن->tl_cursor_pos.صف;
    أرجع (صحيح)tv_get_نمبر(tv) - 1;
}

/*
 * "term_getline(buf, row)" function
 */
    فراغ
f_رکن_getline(typval_T *argvars, typval_T *rettv)
{
    buf_T	    *buf;
    رکن_T	    *رکن;
    صحيح		    صف;

    rettv->v_type = VAR_STRING;

    إذا (وچ_vim9script()
	    && (check_for_بفر_arg(argvars, 0) == FAIL
		|| check_for_lnum_arg(argvars, 1) == FAIL))
	أرجع;

    buf = رکن_get_buf(argvars, "term_getline()");
    إذا (buf == عدم)
	أرجع;
    رکن = buf->b_رکن;
    صف = get_صف_نمبر(&argvars[1], رکن);

    إذا (رکن->tl_vterm == عدم)
    {
	linenr_T  lnum = 0;
	size_t	  وتھ = 0;
	صحيح	  sb_صف = رکن->tl_scrollback_scrolled + صف;
	sb_سطر_T *سطر;

	إذا (sb_صف < 0 || sb_صف >= رکن->tl_scrollback.ga_len)
	    أرجع;
	سطر = (sb_سطر_T *)رکن->tl_scrollback.ga_انکڑے + sb_صف;

	scrollbackline_pos_وچ_buf(رکن, sb_صف, &lnum, عدم, &وتھ);

	// vterm is finished, get the text from the buffer
	إذا (lnum > 0 && lnum <= buf->b_ml.ml_سطر_گنتی)
	{
	    char_u *p = ml_get_buf(buf, lnum, FALSE);
	    إذا (STRLEN(p) >= وتھ + سطر->sb_بائٹاں)
		rettv->vval.v_string = vim_strnsave(p + وتھ, سطر->sb_بائٹاں);
	}
    }
    وإلا
    {
	VTermScreen	*screen = vterm_obtain_screen(رکن->tl_vterm);
	VTermRect	rect;
	صحيح		len;
	char_u		*p;

	إذا (صف < 0 || صف >= رکن->tl_صفاں)
	    أرجع;
	len = رکن->tl_خانے_گنتی * MB_MAXBYTES + 1;
	p = alloc(len);
	إذا (p == عدم)
	    أرجع;
	rettv->vval.v_string = p;

	rect.شروع_خانہ = 0;
	rect.سرا_خانہ = رکن->tl_خانے_گنتی;
	rect.شروع_صف = صف;
	rect.سرا_صف = صف + 1;
	p[vterm_screen_get_متن(screen, (حرف *)p, len, rect)] = NUL;
    }
}

/*
 * "term_getscrolled(buf)" function
 */
    فراغ
f_رکن_getscrolled(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    إذا (وچ_vim9script() && check_for_بفر_arg(argvars, 0) == FAIL)
	أرجع;

    buf = رکن_get_buf(argvars, "term_getscrolled()");
    إذا (buf == عدم)
	أرجع;
    rettv->vval.v_نمبر = buf->b_رکن->tl_بفر_scrolled;
}

/*
 * "term_getsize(buf)" function
 */
    فراغ
f_رکن_getsize(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    list_T	*l;

    إذا (rettv_list_alloc(rettv) == FAIL)
	أرجع;

    إذا (وچ_vim9script() && check_for_بفر_arg(argvars, 0) == FAIL)
	أرجع;

    buf = رکن_get_buf(argvars, "term_getsize()");
    إذا (buf == عدم)
	أرجع;

    l = rettv->vval.v_list;
    list_append_نمبر(l, buf->b_رکن->tl_صفاں);
    list_append_نمبر(l, buf->b_رکن->tl_خانے_گنتی);
}

/*
 * "term_setsize(buf, rows, cols)" function
 */
    فراغ
f_رکن_setsize(typval_T *argvars, typval_T *rettv بن_ورتیا)
{
    buf_T	*buf;
    رکن_T	*رکن;
    varnumber_T صفاں_2, خانے_گنتی;

    إذا (وچ_vim9script()
	    && (check_for_بفر_arg(argvars, 0) == FAIL
		|| check_for_نمبر_arg(argvars, 1) == FAIL
		|| check_for_نمبر_arg(argvars, 2) == FAIL))
	أرجع;

    buf = رکن_get_buf(argvars, "term_setsize()");
    إذا (buf == عدم)
    {
	emsg(_(e_not_terminal_بفر));
	أرجع;
    }
    إذا (buf->b_رکن->tl_vterm == عدم)
	أرجع;
    رکن = buf->b_رکن;
    صفاں_2 = tv_get_نمبر(&argvars[1]);
    صفاں_2 = صفاں_2 <= 0 ? رکن->tl_صفاں : صفاں_2;
    خانے_گنتی = tv_get_نمبر(&argvars[2]);
    خانے_گنتی = خانے_گنتی <= 0 ? رکن->tl_خانے_گنتی : خانے_گنتی;
    vterm_سیٹ_ناپ(رکن->tl_vterm, صفاں_2, خانے_گنتی);
    // handle_resize() will resize the windows

    // Get and remember the size we ended up with.  Update the pty.
    vterm_get_ناپ(رکن->tl_vterm, &رکن->tl_صفاں, &رکن->tl_خانے_گنتی);
    رکن_report_winsize(رکن, رکن->tl_صفاں, رکن->tl_خانے_گنتی);
}

/*
 * "term_getstatus(buf)" function
 */
    فراغ
f_رکن_getstatus(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    رکن_T	*رکن;
    char_u	val[100];

    rettv->v_type = VAR_STRING;

    إذا (وچ_vim9script() && check_for_بفر_arg(argvars, 0) == FAIL)
	أرجع;

    buf = رکن_get_buf(argvars, "term_getstatus()");
    إذا (buf == عدم)
	أرجع;
    رکن = buf->b_رکن;

    إذا (رکن_کم_running(رکن))
	STRCPY(val, "running");
    وإلا
	STRCPY(val, "finished");
    إذا (رکن->tl_عام_منوال)
	STRCAT(val, ",normal");
    rettv->vval.v_string = vim_strsave(val);
}

/*
 * "term_gettitle(buf)" function
 */
    فراغ
f_رکن_gettitle(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    rettv->v_type = VAR_STRING;

    إذا (وچ_vim9script() && check_for_بفر_arg(argvars, 0) == FAIL)
	أرجع;

    buf = رکن_get_buf(argvars, "term_gettitle()");
    إذا (buf == عدم)
	أرجع;

    إذا (buf->b_رکن->tl_title != عدم)
	rettv->vval.v_string = vim_strsave(buf->b_رکن->tl_title);
}

/*
 * "term_gettty(buf)" function
 */
    فراغ
f_رکن_gettty(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    char_u	*p = عدم;
    صحيح		عدد = 0;

    إذا (وچ_vim9script()
	    && (check_for_بفر_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	أرجع;

    rettv->v_type = VAR_STRING;
    buf = رکن_get_buf(argvars, "term_gettty()");
    إذا (buf == عدم)
	أرجع;
    إذا (argvars[1].v_type != VAR_UNKNOWN)
	عدد = tv_get_bool(&argvars[1]);

    بدل (عدد)
    {
	حالة 0:
	    إذا (buf->b_رکن->tl_کم != عدم)
		p = buf->b_رکن->tl_کم->jv_tty_باہرلا;
	    اكسر;
	حالة 1:
	    إذا (buf->b_رکن->tl_کم != عدم)
		p = buf->b_رکن->tl_کم->jv_tty_وچ;
	    اكسر;
	افتراضي:
	    semsg(_(e_invalid_دلیل_str), tv_get_string(&argvars[1]));
	    أرجع;
    }
    إذا (p != عدم)
	rettv->vval.v_string = vim_strsave(p);
}

/*
 * "term_list()" function
 */
    فراغ
f_رکن_list(typval_T *argvars بن_ورتیا, typval_T *rettv)
{
    رکن_T	*tp;
    list_T	*l;

    إذا (rettv_list_alloc(rettv) == FAIL || پہلا_رکن == عدم)
	أرجع;

    l = rettv->vval.v_list;
    FOR_ALL_TERMS(tp)
	إذا (tp->tl_بفر != عدم)
	    إذا (list_append_نمبر(l,
				   (varnumber_T)tp->tl_بفر->b_fnum) == FAIL)
		أرجع;
}

/*
 * "term_scrape(buf, row)" function
 */
    فراغ
f_رکن_scrape(typval_T *argvars, typval_T *rettv)
{
    buf_T	    *buf;
    VTermScreen	    *screen = عدم;
    VTermPos	    pos;
    list_T	    *l;
    رکن_T	    *رکن;
    char_u	    *p;
    sb_سطر_T	    *سطر;

    إذا (rettv_list_alloc(rettv) == FAIL)
	أرجع;

    إذا (وچ_vim9script()
	    && (check_for_بفر_arg(argvars, 0) == FAIL
		|| check_for_lnum_arg(argvars, 1) == FAIL))
	أرجع;

    buf = رکن_get_buf(argvars, "term_scrape()");
    إذا (buf == عدم)
	أرجع;
    رکن = buf->b_رکن;

    l = rettv->vval.v_list;
    pos.صف = get_صف_نمبر(&argvars[1], رکن);

    إذا (رکن->tl_vterm != عدم)
    {
	screen = vterm_obtain_screen(رکن->tl_vterm);
	إذا (screen == عدم)  // can't really happen
	    أرجع;
	p = عدم;
	سطر = عدم;
    }
    وإلا
    {
	صحيح	  sb_صف = رکن->tl_scrollback_scrolled + pos.صف;
	linenr_T  lnum = 0;
	size_t	  وتھ = 0;

	scrollbackline_pos_وچ_buf(رکن, sb_صف, &lnum, عدم, &وتھ);

	إذا (sb_صف >= رکن->tl_scrollback.ga_len || lnum <= 0 || lnum > buf->b_ml.ml_سطر_گنتی)
	    أرجع;

	سطر = (sb_سطر_T *)رکن->tl_scrollback.ga_انکڑے + sb_صف;
	p = ml_get_buf(buf, lnum, FALSE);

	إذا (STRLEN(p) < وتھ + سطر->sb_بائٹاں)
	    أرجع;

	p += وتھ;
    }

    لكل (pos.خانہ = 0; pos.خانہ < رکن->tl_خانے_گنتی; )
    {
	dict_T		*dcell;
	صحيح		چوڑائی;
	VTermScreenCellAttrs attrs;
	VTermColor	fg, bg;
	char_u		rgb[8];
	size_t		rgblen;
	char_u		mbs[MB_MAXBYTES * VTERM_MAX_CHARS_فی_CELL + 1];
	size_t		mbslen;
	صحيح		i;

	إذا (screen == عدم)
	{
	    cellattr_T	*cellattr;

	    // vterm has finished, get the cell from scrollback
	    إذا (pos.خانہ >= سطر->sb_خانے_گنتی)
		اكسر;
	    cellattr = سطر->sb_cells + pos.خانہ;
	    چوڑائی = cellattr->چوڑائی;
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
	    لكل (i = 0; i < VTERM_MAX_CHARS_فی_CELL; ++i)
	    {
		إذا (cell.chars[i] == 0)
		    اكسر;
		mbslen += (*utf_char2bytes)((صحيح)cell.chars[i], mbs + mbslen);
	    }
	    mbs[mbslen] = NUL;
	    چوڑائی = cell.چوڑائی;
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

	dict_جوڑو_string_len(dcell, "chars", mbs, (صحيح)mbslen);

	rgblen = vim_snprintf_safelen((حرف *)rgb, حجم(rgb),
	    "#%02x%02x%02x", fg.red, fg.green, fg.blue);
	dict_جوڑو_string_len(dcell, "fg", rgb, (صحيح)rgblen);
	rgblen = vim_snprintf_safelen((حرف *)rgb, حجم(rgb),
	    "#%02x%02x%02x", bg.red, bg.green, bg.blue);
	dict_جوڑو_string_len(dcell, "bg", rgb, (صحيح)rgblen);

	dict_جوڑو_نمبر(dcell, "attr",
				      cell2attr(رکن, عدم, &attrs, &fg, &bg));
	dict_جوڑو_نمبر(dcell, "width", چوڑائی);

	++pos.خانہ;
	إذا (چوڑائی == 2)
	    ++pos.خانہ;
    }
}

/*
 * "term_sendkeys(buf, keys)" function
 */
    فراغ
f_رکن_sendkeys(typval_T *argvars, typval_T *rettv بن_ورتیا)
{
    buf_T	*buf;
    char_u	*پیغام;
    رکن_T	*رکن;

    إذا (وچ_vim9script()
	    && (check_for_بفر_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	أرجع;

    buf = رکن_get_buf(argvars, "term_sendkeys()");
    إذا (buf == عدم)
	أرجع;

    پیغام = tv_get_string_chk(&argvars[1]);
    إذا (پیغام == عدم)
	أرجع;
    رکن = buf->b_رکن;
    إذا (رکن->tl_vterm == عدم)
	أرجع;

    طالما (*پیغام != NUL)
    {
	صحيح c;

	إذا (*پیغام == K_SPECIAL && پیغام[1] != NUL && پیغام[2] != NUL)
	{
	    c = منزل_SPECIAL(پیغام[1], پیغام[2]);
	    پیغام += 3;
	}
	وإلا
	{
	    c = PTR2CHAR(پیغام);
	    پیغام += MB_CPTR2LEN(پیغام);
	}
	send_کلیدیں_منزل_رکن(رکن, c, 0, FALSE);
    }
}

#إذا defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * "term_getansicolors(buf)" function
 */
    فراغ
f_رکن_getansicolors(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    رکن_T	*رکن;
    VTermState	*state;
    VTermColor  color;
    char_u	hexbuf[10];
    صحيح		اشاریہ;
    list_T	*list;

    إذا (rettv_list_alloc(rettv) == FAIL)
	أرجع;

    إذا (وچ_vim9script() && check_for_بفر_arg(argvars, 0) == FAIL)
	أرجع;

    buf = رکن_get_buf(argvars, "term_getansicolors()");
    إذا (buf == عدم)
	أرجع;
    رکن = buf->b_رکن;
    إذا (رکن->tl_vterm == عدم)
	أرجع;

    list = rettv->vval.v_list;
    state = vterm_obtain_state(رکن->tl_vterm);
    لكل (اشاریہ = 0; اشاریہ < 16; اشاریہ++)
    {
	size_t	hexbuflen;

	vterm_state_get_palette_color(state, اشاریہ, &color);
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
f_رکن_setansicolors(typval_T *argvars, typval_T *rettv بن_ورتیا)
{
    buf_T	*buf;
    رکن_T	*رکن;
    listitem_T	*li;
    صحيح		n = 0;

    إذا (وچ_vim9script()
	    && (check_for_بفر_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL))
	أرجع;

    buf = رکن_get_buf(argvars, "term_setansicolors()");
    إذا (buf == عدم)
	أرجع;
    رکن = buf->b_رکن;
    إذا (رکن->tl_vterm == عدم)
	أرجع;

    إذا (check_for_nonnull_list_arg(argvars, 1) == FAIL)
	أرجع;

    إذا (argvars[1].vval.v_list->lv_پہلا == &range_list_شے
	    || argvars[1].vval.v_list->lv_len != 16)
    {
	semsg(_(e_invalid_قدر_for_دلیل_str), "\"colors\"");
	أرجع;
    }

    إذا (رکن->tl_palette == عدم)
	رکن->tl_palette = ALLOC_MULT(long_u, 16);
    إذا (رکن->tl_palette == عدم)
	أرجع;

    FOR_ALL_LIST_اشیاء(argvars[1].vval.v_list, li)
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

	رکن->tl_palette[n++] = GUI_MCH_GET_RGB(guicolor);
    }

    رکن_update_palette(رکن);
}
#جے_انت

/*
 * "term_setapi(buf, api)" function
 */
    فراغ
f_رکن_setapi(typval_T *argvars, typval_T *rettv بن_ورتیا)
{
    buf_T	*buf;
    رکن_T	*رکن;
    char_u	*api;

    إذا (وچ_vim9script()
	    && (check_for_بفر_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	أرجع;

    buf = رکن_get_buf(argvars, "term_setapi()");
    إذا (buf == عدم)
	أرجع;
    رکن = buf->b_رکن;
    vim_free(رکن->tl_api);
    api = tv_get_string_chk(&argvars[1]);
    إذا (api != عدم)
	رکن->tl_api = vim_strsave(api);
    وإلا
	رکن->tl_api = عدم;
}

/*
 * "term_setrestore(buf, command)" function
 */
    فراغ
f_رکن_setrestore(typval_T *argvars بن_ورتیا, typval_T *rettv بن_ورتیا)
{
#إذا defined(FEAT_SESSION)
    buf_T	*buf;
    رکن_T	*رکن;
    char_u	*cmd;

    إذا (وچ_vim9script()
	    && (check_for_بفر_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	أرجع;

    buf = رکن_get_buf(argvars, "term_setrestore()");
    إذا (buf == عدم)
	أرجع;
    رکن = buf->b_رکن;
    vim_free(رکن->tl_command);
    cmd = tv_get_string_chk(&argvars[1]);
    إذا (cmd != عدم)
	رکن->tl_command = vim_strsave(cmd);
    وإلا
	رکن->tl_command = عدم;
#جے_انت
}

/*
 * "term_setkill(buf, how)" function
 */
    فراغ
f_رکن_setkill(typval_T *argvars بن_ورتیا, typval_T *rettv بن_ورتیا)
{
    buf_T	*buf;
    رکن_T	*رکن;
    char_u	*how;

    إذا (وچ_vim9script()
	    && (check_for_بفر_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	أرجع;

    buf = رکن_get_buf(argvars, "term_setkill()");
    إذا (buf == عدم)
	أرجع;
    رکن = buf->b_رکن;
    vim_free(رکن->tl_kill);
    how = tv_get_string_chk(&argvars[1]);
    إذا (how != عدم)
	رکن->tl_kill = vim_strsave(how);
    وإلا
	رکن->tl_kill = عدم;
}

/*
 * "term_start(command, options)" function
 */
    فراغ
f_رکن_شروع(typval_T *argvars, typval_T *rettv)
{
    jobopt_T	opt;
    buf_T	*buf;

    إذا (وچ_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	أرجع;

    init_کم_options(&opt);
    إذا (argvars[1].v_type != VAR_UNKNOWN
	    && get_کم_options(&argvars[1], &opt,
		JO_TIMEOUT_ALL + JO_STOPONEXIT
		    + JO_CALLBACK + JO_باہرلا_CALLBACK + JO_ERR_CALLBACK
		    + JO_EXIT_CB + JO_CLOSE_CALLBACK + JO_باہرلا_IO,
		JO2_رکن_NAME + JO2_رکن_انت + JO2_HIDDEN + JO2_رکن_OPENCMD
		    + JO2_رکن_خانے_گنتی + JO2_رکن_صفاں + JO2_VERTICAL + JO2_CURWIN
		    + JO2_CWD + JO2_ENV + JO2_EOF_CHARS
		    + JO2_NORESTORE + JO2_رکن_KILL + JO2_رکن_HIGHLIGHT
		    + JO2_ANSI_COLORS + JO2_TTY_TYPE + JO2_رکن_API) == FAIL)
	أرجع;

    buf = رکن_شروع(&argvars[0], عدم, &opt, 0);

    إذا (buf != عدم && buf->b_رکن != عدم)
	rettv->vval.v_نمبر = buf->b_fnum;
}

/*
 * "term_wait" function
 */
    فراغ
f_رکن_wait(typval_T *argvars, typval_T *rettv بن_ورتیا)
{
    buf_T	*buf;

    إذا (وچ_vim9script()
	    && (check_for_بفر_arg(argvars, 0) == FAIL
		|| check_for_opt_نمبر_arg(argvars, 1) == FAIL))
	أرجع;

    buf = رکن_get_buf(argvars, "term_wait()");
    إذا (buf == عدم)
	أرجع;
    إذا (buf->b_رکن->tl_کم == عدم)
    {
	ch_log(عدم, "term_wait(): no job to wait for");
	أرجع;
    }
    إذا (buf->b_رکن->tl_کم->jv_channel == عدم)
	// channel is closed, nothing to do
	أرجع;

    // Get the job status, this will detect a job that finished.
    إذا (!buf->b_رکن->tl_کم->jv_channel->ch_keep_open
	    && STRCMP(کم_حالت(buf->b_رکن->tl_کم), "dead") == 0)
    {
	// The job is dead, keep reading channel I/O until the channel is
	// closed. buf->b_term may become NULL if the terminal was closed while
	// waiting.
	ch_log(عدم, "term_wait(): waiting for channel to close");
	طالما (buf->b_رکن != عدم && !buf->b_رکن->tl_channel_closed)
	{
	    رکن_flush_messages();

	    ui_delay(10L, FALSE);
	    إذا (!buf_valid(buf))
		// If the terminal is closed when the channel is closed the
		// buffer disappears.
		اكسر;
	    إذا (buf->b_رکن == عدم || buf->b_رکن->tl_channel_closing)
		// came here from a close callback, only wait one time
		اكسر;
	}

	رکن_flush_messages();
    }
    وإلا
    {
	لما wait = 10L;

	رکن_flush_messages();

	// Wait for some time for any channel I/O.
	إذا (argvars[1].v_type != VAR_UNKNOWN)
	    wait = tv_get_نمبر(&argvars[1]);
	ui_delay(wait, TRUE);

	// Flushing messages on channels is hopefully sufficient.
	// TODO: is there a better way?
	رکن_flush_messages();
    }
}

/*
 * Called when a channel has sent all the lines to a terminal.
 * Send a CTRL-D to mark the end of the text.
 */
    فراغ
رکن_send_eof(channel_T *ch)
{
    رکن_T	*رکن;

    FOR_ALL_TERMS(رکن)
	إذا (رکن->tl_کم == ch->ch_کم)
	{
	    إذا (رکن->tl_eof_chars != عدم)
	    {
		channel_send(ch, PART_وچ, رکن->tl_eof_chars,
					(صحيح)STRLEN(رکن->tl_eof_chars), عدم);
		channel_send(ch, PART_وچ, (char_u *)"\r", 1, عدم);
	    }
#جے_تعریف_شدہ MSWIN
	    وإلا
		// Default: CTRL-D
		channel_send(ch, PART_وچ, (char_u *)"\004\r", 2, عدم);
#جے_انت
	}
}

#إذا defined(FEAT_GUI)
    کم_T *
رکن_getjob(رکن_T *رکن)
{
    أرجع رکن != عدم ? رکن->tl_کم : عدم;
}
#جے_انت

#إذا defined(MSWIN)

///////////////////////////////////////
// 2. MS-Windows implementation.

HRESULT (WINAPI *pCreatePseudoConsole)(COORD, HANDLE, HANDLE, DWORD, HPCON*);
HRESULT (WINAPI *pResizePseudoConsole)(HPCON, COORD);
HRESULT (WINAPI *pClosePseudoConsole)(HPCON);
BOOL (WINAPI *pInitializeProcThreadAttributeList)(LPPROC_THREAD_ATTRIBUTE_LIST, DWORD, DWORD, PSIZE_T);
BOOL (WINAPI *pUpdateProcThreadAttribute)(LPPROC_THREAD_ATTRIBUTE_LIST, DWORD, DWORD_PTR, PVOID, ناپ_T, PVOID, PSIZE_T);
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
    } conpty_اندراج[] =
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
	    emsg(_(e_conpty_is_not_دستیاب));
	أرجع FAIL;
    }

    // No need to initialize twice.
    إذا (hKerneldll)
	أرجع OK;

    hKerneldll = vim_لدو_Lib("kernel32.dll");
    لكل (i = 0; conpty_اندراج[i].name != عدم
					&& conpty_اندراج[i].ptr != عدم; ++i)
    {
	إذا ((*conpty_اندراج[i].ptr = (FARPROC)Get_Proc_پتہ(hKerneldll,
						conpty_اندراج[i].name)) == عدم)
	{
	    إذا (verbose)
		semsg(_(e_could_not_لدو_library_function_str), conpty_اندراج[i].name);
	    hKerneldll = عدم;
	    أرجع FAIL;
	}
    }

    أرجع OK;
}

    ساكن صحيح
conpty_رکن_and_کم_init(
	رکن_T	    *رکن,
	typval_T    *argvar,
	حرف	    **argv بن_ورتیا,
	jobopt_T    *opt,
	jobopt_T    *orig_opt)
{
    WCHAR	    *cmd_wchar = عدم;
    WCHAR	    *cmd_wchar_copy = عدم;
    WCHAR	    *cwd_wchar = عدم;
    WCHAR	    *env_wchar = عدم;
    channel_T	    *channel = عدم;
    کم_T	    *کم = عدم;
    HANDLE	    jo = عدم;
    garray_T	    ga_cmd, ga_env;
    char_u	    *cmd = عدم;
    HRESULT	    hr;
    COORD	    consize;
    ناپ_T	    breq;
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
	cmd = ga_cmd.ga_انکڑے;
    }
    إذا (cmd == عدم || *cmd == NUL)
    {
	emsg(_(e_invalid_دلیل));
	اذهب failed;
    }

    رکن->tl_arg0_cmd = vim_strsave(cmd);

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
    env_wchar = ga_env.ga_انکڑے;

    إذا (!CreatePipe(&i_theirs, &i_ours, عدم, 0))
	اذهب failed;
    إذا (!CreatePipe(&o_ours, &o_theirs, عدم, 0))
	اذهب failed;

    consize.X = رکن->tl_خانے_گنتی;
    consize.Y = رکن->tl_صفاں;
    hr = pCreatePseudoConsole(consize, i_theirs, o_theirs, 0,
							     &رکن->tl_conpty);
    إذا (FAILED(hr))
	اذهب failed;

    رکن->tl_siex.StartupInfo.cb = حجم(رکن->tl_siex);

    // Explicitly invalidate std handles to prevent inheritance of
    // the debugger's stdout (e.g., in Visual Studio debugging sessions),
    // which could cause job output to go to the debugger instead of
    // the intended ConPTY, even with bInheritHandles set to FALSE in CreateProcess.
    رکن->tl_siex.StartupInfo.dwFlags = STARTF_USESTDHANDLES;
    رکن->tl_siex.StartupInfo.h_Std_داخلہ = INVALID_HANDLE_قدر;
    رکن->tl_siex.StartupInfo.hStdOutput = INVALID_HANDLE_قدر;
    رکن->tl_siex.StartupInfo.hStdError = INVALID_HANDLE_قدر;

    // Set up pipe inheritance safely: Vista or later.
    pInitializeProcThreadAttributeList(عدم, 1, 0, &breq);
    رکن->tl_siex.lpAttributeList = alloc(breq);
    إذا (!رکن->tl_siex.lpAttributeList)
	اذهب failed;
    إذا (!pInitializeProcThreadAttributeList(رکن->tl_siex.lpAttributeList, 1,
								     0, &breq))
	اذهب failed;
    إذا (!pUpdateProcThreadAttribute(
	    رکن->tl_siex.lpAttributeList, 0,
	    PROC_THREAD_ATTRIBUTE_PSEUDOCONSOLE, رکن->tl_conpty,
	    حجم(HPCON), عدم, عدم))
	اذهب failed;

    channel = جوڑو_channel();
    إذا (channel == عدم)
	اذهب failed;

    کم = کم_alloc();
    إذا (کم == عدم)
	اذهب failed;
    إذا (argvar->v_type == VAR_STRING)
    {
	صحيح argc;

	build_argv_from_string(cmd, &کم->jv_argv, &argc);
    }
    وإلا
    {
	صحيح argc;

	build_argv_from_list(argvar->vval.v_list, &کم->jv_argv, &argc);
    }

    إذا (opt->jo_سیٹ & JO_وچ_BUF)
	کم->jv_وچ_buf = buflist_findnr(opt->jo_io_buf[PART_وچ]);

    إذا (!CreateProcessW(عدم, cmd_wchar_copy, عدم, عدم, FALSE,
	    EXTENDED_STARTUPINFO_PRESENT | CREATE_UNICODE_ENVIRONMENT
	    | CREATE_SUSPENDED | CREATE_DEFAULT_ERROR_منوال,
	    env_wchar, cwd_wchar,
	    &رکن->tl_siex.StartupInfo, &proc_info))
    {
	errmsg = GetWin32Error();
	اذهب failed;
    }

    CloseHandle(i_theirs);
    CloseHandle(o_theirs);

    channel_سیٹ_pipes(channel,
	    (sock_T)i_ours,
	    (sock_T)o_ours,
	    (sock_T)o_ours);

    // Write lines with CR instead of NL.
    channel->ch_write_متن_منوال = TRUE;

    // Use to explicitly delete anonymous pipe handle.
    // In addition, it is used to prevent the pipe from being closed when input
    // from a buffer etc. is finished.
    channel->ch_anonymous_pipe = TRUE;

    jo = Create_کم_Object(عدم, عدم);
    إذا (jo == عدم)
	اذهب failed;

    إذا (!سونپو_Process_منزل_کم_Object(jo, proc_info.hProcess))
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

    إذا (create_vterm(رکن, رکن->tl_صفاں, رکن->tl_خانے_گنتی) == FAIL)
	اذهب failed;

# إذا defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    إذا (رکن_use_palette())
    {
	إذا (رکن->tl_palette != عدم)
	    سیٹ_vterm_palette(رکن->tl_vterm, رکن->tl_palette);
	وإلا
	    init_vterm_ansi_colors(رکن->tl_vterm);
    }
# endif

    channel_سیٹ_کم(channel, کم, opt);
    کم_سیٹ_options(کم, opt);

    کم->jv_channel = channel;
    کم->jv_proc_info = proc_info;
    کم->jv_کم_object = jo;
    کم->jv_حالت = کم_STARTED;
    کم->jv_tty_type = vim_strsave((char_u *)"conpty");
    ++کم->jv_refcount;
    رکن->tl_کم = کم;

    // Redirecting stdout and stderr doesn't work at the job level.  Instead
    // open the file here and handle it in.  opt->jo_io was changed in
    // setup_job_options(), use the original flags here.
    إذا (orig_opt->jo_io[PART_باہرلا] == JIO_FILE)
    {
	char_u *fname = opt->jo_io_name[PART_باہرلا];

	ch_log(channel, "Opening output file %s", fname);
	رکن->tl_باہرلا_fd = mch_fopen((حرف *)fname, WRITEBIN);
	إذا (رکن->tl_باہرلا_fd == عدم)
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
    إذا (کم != عدم)
    {
	کم->jv_channel = عدم;
	کم_cleanup(کم);
    }
    رکن->tl_کم = عدم;
    إذا (jo != عدم)
	CloseHandle(jo);

    إذا (رکن->tl_siex.lpAttributeList != عدم)
    {
	pDeleteProcThreadAttributeList(رکن->tl_siex.lpAttributeList);
	vim_free(رکن->tl_siex.lpAttributeList);
    }
    رکن->tl_siex.lpAttributeList = عدم;
    إذا (o_theirs != عدم)
	CloseHandle(o_theirs);
    إذا (o_ours != عدم)
	CloseHandle(o_ours);
    إذا (i_ours != عدم)
	CloseHandle(i_ours);
    إذا (i_theirs != عدم)
	CloseHandle(i_theirs);
    إذا (رکن->tl_conpty != عدم)
	pClosePseudoConsole(رکن->tl_conpty);
    رکن->tl_conpty = عدم;
    // Propagate errors that occur in CreateProcess
    إذا (errmsg)
	semsg("CreateProcess failed: %s", errmsg);
    أرجع FAIL;
}

    ساكن فراغ
conpty_رکن_report_winsize(رکن_T *رکن, صحيح صفاں_2, صحيح خانے_گنتی)
{
    COORD consize;

    consize.X = خانے_گنتی;
    consize.Y = صفاں_2;
    pResizePseudoConsole(رکن->tl_conpty, consize);
}

    ساكن فراغ
رکن_free_conpty(رکن_T *رکن)
{
    إذا (رکن->tl_siex.lpAttributeList != عدم)
    {
	pDeleteProcThreadAttributeList(رکن->tl_siex.lpAttributeList);
	vim_free(رکن->tl_siex.lpAttributeList);
    }
    رکن->tl_siex.lpAttributeList = عدم;
    إذا (رکن->tl_conpty != عدم)
	pClosePseudoConsole(رکن->tl_conpty);
    رکن->tl_conpty = عدم;
}

    صحيح
use_conpty(فراغ)
{
    أرجع has_conpty;
}

# define WINPTY_SPAWN_FLAG_AUTO_SHUTDOWN 1ul
# define WINPTY_SPAWN_FLAG_EXIT_AFTER_SHUTDOWN 2ull
# define WINPTY_MOUSE_منوال_FORCE		2

فراغ* (*winpty_config_new)(UINT64, فراغ*);
فراغ* (*winpty_open)(فراغ*, فراغ*);
فراغ* (*winpty_spawn_config_new)(UINT64, فراغ*, LPCWSTR, فراغ*, فراغ*, فراغ*);
BOOL (*winpty_spawn)(فراغ*, فراغ*, HANDLE*, HANDLE*, DWORD*, فراغ*);
فراغ (*winpty_config_سیٹ_mouse_منوال)(فراغ*, صحيح);
فراغ (*winpty_config_سیٹ_initial_ناپ)(فراغ*, صحيح, صحيح);
LPCWSTR (*winpty_conin_name)(فراغ*);
LPCWSTR (*winpty_conout_name)(فراغ*);
LPCWSTR (*winpty_conerr_name)(فراغ*);
فراغ (*winpty_free)(فراغ*);
فراغ (*winpty_config_free)(فراغ*);
فراغ (*winpty_spawn_config_free)(فراغ*);
فراغ (*winpty_error_free)(فراغ*);
LPCWSTR (*winpty_error_پیغام)(فراغ*);
BOOL (*winpty_سیٹ_ناپ)(فراغ*, صحيح, صحيح, فراغ*);
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
    } winpty_اندراج[] =
    {
	{"winpty_conerr_name", (FARPROC*)&winpty_conerr_name},
	{"winpty_config_free", (FARPROC*)&winpty_config_free},
	{"winpty_config_new", (FARPROC*)&winpty_config_new},
	{"winpty_config_set_mouse_mode",
				      (FARPROC*)&winpty_config_سیٹ_mouse_منوال},
	{"winpty_config_set_initial_size",
				    (FARPROC*)&winpty_config_سیٹ_initial_ناپ},
	{"winpty_conin_name", (FARPROC*)&winpty_conin_name},
	{"winpty_conout_name", (FARPROC*)&winpty_conout_name},
	{"winpty_error_free", (FARPROC*)&winpty_error_free},
	{"winpty_free", (FARPROC*)&winpty_free},
	{"winpty_open", (FARPROC*)&winpty_open},
	{"winpty_spawn", (FARPROC*)&winpty_spawn},
	{"winpty_spawn_config_free", (FARPROC*)&winpty_spawn_config_free},
	{"winpty_spawn_config_new", (FARPROC*)&winpty_spawn_config_new},
	{"winpty_error_msg", (FARPROC*)&winpty_error_پیغام},
	{"winpty_set_size", (FARPROC*)&winpty_سیٹ_ناپ},
	{"winpty_agent_process", (FARPROC*)&winpty_agent_process},
	{عدم, عدم}
    };

    // No need to initialize twice.
    إذا (hWinPtyDLL)
	أرجع OK;
    // Load winpty.dll, prefer using the 'winptydll' option, fall back to just
    // winpty.dll.
    إذا (*p_winptydll != NUL)
	hWinPtyDLL = vim_لدو_Lib((حرف *)p_winptydll);
    إذا (!hWinPtyDLL)
	hWinPtyDLL = vim_لدو_Lib(WINPTY_DLL);
    إذا (!hWinPtyDLL)
    {
	إذا (verbose)
	    semsg(_(e_could_not_لدو_library_str_str),
		    (*p_winptydll != NUL ? p_winptydll : (char_u *)WINPTY_DLL),
		    GetWin32Error());
	أرجع FAIL;
    }
    لكل (i = 0; winpty_اندراج[i].name != عدم
					 && winpty_اندراج[i].ptr != عدم; ++i)
    {
	إذا ((*winpty_اندراج[i].ptr = (FARPROC)Get_Proc_پتہ(hWinPtyDLL,
					      winpty_اندراج[i].name)) == عدم)
	{
	    إذا (verbose)
		semsg(_(e_could_not_لدو_library_function_str), winpty_اندراج[i].name);
	    hWinPtyDLL = عدم;
	    أرجع FAIL;
	}
    }

    أرجع OK;
}

    ساكن صحيح
winpty_رکن_and_کم_init(
	رکن_T	    *رکن,
	typval_T    *argvar,
	حرف	    **argv بن_ورتیا,
	jobopt_T    *opt,
	jobopt_T    *orig_opt)
{
    WCHAR	    *cmd_wchar = عدم;
    WCHAR	    *cwd_wchar = عدم;
    WCHAR	    *env_wchar = عدم;
    channel_T	    *channel = عدم;
    کم_T	    *کم = عدم;
    DWORD	    error;
    HANDLE	    jo = عدم;
    HANDLE	    بچہ_process_handle;
    HANDLE	    بچہ_thread_handle;
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
	cmd = ga_cmd.ga_انکڑے;
    }
    إذا (cmd == عدم || *cmd == NUL)
    {
	emsg(_(e_invalid_دلیل));
	اذهب failed;
    }

    رکن->tl_arg0_cmd = vim_strsave(cmd);

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
    env_wchar = ga_env.ga_انکڑے;

    رکن->tl_winpty_config = winpty_config_new(0, &winpty_err);
    إذا (رکن->tl_winpty_config == عدم)
	اذهب failed;

    winpty_config_سیٹ_mouse_منوال(رکن->tl_winpty_config,
						    WINPTY_MOUSE_منوال_FORCE);
    winpty_config_سیٹ_initial_ناپ(رکن->tl_winpty_config,
						 رکن->tl_خانے_گنتی, رکن->tl_صفاں);
    رکن->tl_winpty = winpty_open(رکن->tl_winpty_config, &winpty_err);
    إذا (رکن->tl_winpty == عدم)
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

    channel = جوڑو_channel();
    إذا (channel == عدم)
	اذهب failed;

    کم = کم_alloc();
    إذا (کم == عدم)
	اذهب failed;
    إذا (argvar->v_type == VAR_STRING)
    {
	صحيح argc;

	build_argv_from_string(cmd, &کم->jv_argv, &argc);
    }
    وإلا
    {
	صحيح argc;

	build_argv_from_list(argvar->vval.v_list, &کم->jv_argv, &argc);
    }

    إذا (opt->jo_سیٹ & JO_وچ_BUF)
	کم->jv_وچ_buf = buflist_findnr(opt->jo_io_buf[PART_وچ]);

    إذا (!winpty_spawn(رکن->tl_winpty, spawn_config, &بچہ_process_handle,
	    &بچہ_thread_handle, &error, &winpty_err))
	اذهب failed;

    channel_سیٹ_pipes(channel,
	(sock_T)CreateFileW(
	    winpty_conin_name(رکن->tl_winpty),
	    GENERIC_WRITE, 0, عدم,
	    OPEN_EXISTING, 0, عدم),
	(sock_T)CreateFileW(
	    winpty_conout_name(رکن->tl_winpty),
	    GENERIC_READ, 0, عدم,
	    OPEN_EXISTING, 0, عدم),
	(sock_T)CreateFileW(
	    winpty_conerr_name(رکن->tl_winpty),
	    GENERIC_READ, 0, عدم,
	    OPEN_EXISTING, 0, عدم));

    // Write lines with CR instead of NL.
    channel->ch_write_متن_منوال = TRUE;

    jo = Create_کم_Object(عدم, عدم);
    إذا (jo == عدم)
	اذهب failed;

    إذا (!سونپو_Process_منزل_کم_Object(jo, بچہ_process_handle))
    {
	// Failed, switch the way to terminate process with TerminateProcess.
	CloseHandle(jo);
	jo = عدم;
    }

    winpty_spawn_config_free(spawn_config);
    vim_free(cmd_wchar);
    vim_free(cwd_wchar);
    vim_free(env_wchar);

    إذا (create_vterm(رکن, رکن->tl_صفاں, رکن->tl_خانے_گنتی) == FAIL)
	اذهب failed;

# إذا defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    إذا (رکن_use_palette())
    {
	إذا (رکن->tl_palette != عدم)
	    سیٹ_vterm_palette(رکن->tl_vterm, رکن->tl_palette);
	وإلا
	    init_vterm_ansi_colors(رکن->tl_vterm);
    }
# endif

    channel_سیٹ_کم(channel, کم, opt);
    کم_سیٹ_options(کم, opt);

    کم->jv_channel = channel;
    کم->jv_proc_info.hProcess = بچہ_process_handle;
    کم->jv_proc_info.dwProcessId = GetProcessId(بچہ_process_handle);
    کم->jv_کم_object = jo;
    کم->jv_حالت = کم_STARTED;
    کم->jv_tty_وچ = utf16_منزل_enc(
	    (short_u *)winpty_conin_name(رکن->tl_winpty), عدم);
    کم->jv_tty_باہرلا = utf16_منزل_enc(
	    (short_u *)winpty_conout_name(رکن->tl_winpty), عدم);
    کم->jv_tty_type = vim_strsave((char_u *)"winpty");
    ++کم->jv_refcount;
    رکن->tl_کم = کم;

    // Redirecting stdout and stderr doesn't work at the job level.  Instead
    // open the file here and handle it in.  opt->jo_io was changed in
    // setup_job_options(), use the original flags here.
    إذا (orig_opt->jo_io[PART_باہرلا] == JIO_FILE)
    {
	char_u *fname = opt->jo_io_name[PART_باہرلا];

	ch_log(channel, "Opening output file %s", fname);
	رکن->tl_باہرلا_fd = mch_fopen((حرف *)fname, WRITEBIN);
	إذا (رکن->tl_باہرلا_fd == عدم)
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
    إذا (کم != عدم)
    {
	کم->jv_channel = عدم;
	کم_cleanup(کم);
    }
    رکن->tl_کم = عدم;
    إذا (jo != عدم)
	CloseHandle(jo);
    إذا (رکن->tl_winpty != عدم)
	winpty_free(رکن->tl_winpty);
    رکن->tl_winpty = عدم;
    إذا (رکن->tl_winpty_config != عدم)
	winpty_config_free(رکن->tl_winpty_config);
    رکن->tl_winpty_config = عدم;
    إذا (winpty_err != عدم)
    {
	حرف *پیغام = (حرف *)utf16_منزل_enc(
				(short_u *)winpty_error_پیغام(winpty_err), عدم);

	إذا (پیغام != عدم)
	{
	    emsg(پیغام);
	    vim_free(پیغام);
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
رکن_and_کم_init(
	رکن_T	    *رکن,
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
	أرجع conpty_رکن_and_کم_init(رکن, argvar, argv, opt, orig_opt);

    إذا (use_winpty)
	أرجع winpty_رکن_and_کم_init(رکن, argvar, argv, opt, orig_opt);

    // error
    أرجع dyn_winpty_init(TRUE);
}

    ساكن صحيح
create_pty_only(رکن_T *رکن, jobopt_T *options)
{
    HANDLE	    h_Pipe_وچ = INVALID_HANDLE_قدر;
    HANDLE	    h_Pipe_باہرلا = INVALID_HANDLE_قدر;
    حرف	    وچ_name[80], باہرلا_name[80];
    channel_T	    *channel = عدم;

    إذا (create_vterm(رکن, رکن->tl_صفاں, رکن->tl_خانے_گنتی) == FAIL)
	أرجع FAIL;

    vim_snprintf(وچ_name, حجم(وچ_name), "\\\\.\\pipe\\vim-%d-in-%d",
	    GetCurrentProcessId(),
	    curbuf->b_fnum);
    h_Pipe_وچ = CreateNamedPipe(وچ_name, PIPE_ACCESS_OUTBOUND,
	    PIPE_TYPE_سنیہا | PIPE_NOWAIT,
	    PIPE_UNLIMITED_INSTANCES,
	    0, 0, NMPWAIT_NOWAIT, عدم);
    إذا (h_Pipe_وچ == INVALID_HANDLE_قدر)
	اذهب failed;

    vim_snprintf(باہرلا_name, حجم(باہرلا_name), "\\\\.\\pipe\\vim-%d-out-%d",
	    GetCurrentProcessId(),
	    curbuf->b_fnum);
    h_Pipe_باہرلا = CreateNamedPipe(باہرلا_name, PIPE_ACCESS_INBOUND,
	    PIPE_TYPE_سنیہا | PIPE_NOWAIT,
	    PIPE_UNLIMITED_INSTANCES,
	    0, 0, 0, عدم);
    إذا (h_Pipe_باہرلا == INVALID_HANDLE_قدر)
	اذهب failed;

    ConnectNamedPipe(h_Pipe_وچ, عدم);
    ConnectNamedPipe(h_Pipe_باہرلا, عدم);

    رکن->tl_کم = کم_alloc();
    إذا (رکن->tl_کم == عدم)
	اذهب failed;
    ++رکن->tl_کم->jv_refcount;

    // behave like the job is already finished
    رکن->tl_کم->jv_حالت = کم_مکمل;

    channel = جوڑو_channel();
    إذا (channel == عدم)
	اذهب failed;
    رکن->tl_کم->jv_channel = channel;
    channel->ch_keep_open = TRUE;
    channel->ch_named_pipe = TRUE;

    channel_سیٹ_pipes(channel,
	(sock_T)h_Pipe_وچ,
	(sock_T)h_Pipe_باہرلا,
	(sock_T)h_Pipe_باہرلا);
    channel_سیٹ_کم(channel, رکن->tl_کم, options);
    رکن->tl_کم->jv_tty_وچ = vim_strsave((char_u*)وچ_name);
    رکن->tl_کم->jv_tty_باہرلا = vim_strsave((char_u*)باہرلا_name);

    أرجع OK;

failed:
    إذا (h_Pipe_وچ != عدم)
	CloseHandle(h_Pipe_وچ);
    إذا (h_Pipe_باہرلا != عدم)
	CloseHandle(h_Pipe_باہرلا);
    أرجع FAIL;
}

/*
 * Free the terminal emulator part of "term".
 */
    ساكن فراغ
رکن_free_vterm(رکن_T *رکن)
{
    رکن_free_conpty(رکن);
    إذا (رکن->tl_winpty != عدم)
	winpty_free(رکن->tl_winpty);
    رکن->tl_winpty = عدم;
    إذا (رکن->tl_winpty_config != عدم)
	winpty_config_free(رکن->tl_winpty_config);
    رکن->tl_winpty_config = عدم;
    إذا (رکن->tl_vterm != عدم)
	vterm_free(رکن->tl_vterm);
    رکن->tl_vterm = عدم;
}

/*
 * Report the size to the terminal.
 */
    ساكن فراغ
رکن_report_winsize(رکن_T *رکن, صحيح صفاں_2, صحيح خانے_گنتی)
{
    إذا (رکن->tl_conpty)
	conpty_رکن_report_winsize(رکن, صفاں_2, خانے_گنتی);
    إذا (رکن->tl_winpty)
	winpty_سیٹ_ناپ(رکن->tl_winpty, خانے_گنتی, صفاں_2, عدم);
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
رکن_and_کم_init(
	رکن_T	    *رکن,
	typval_T    *argvar,
	حرف	    **argv,
	jobopt_T    *opt,
	jobopt_T    *orig_opt بن_ورتیا)
{
    رکن->tl_arg0_cmd = عدم;

    إذا (create_vterm(رکن, رکن->tl_صفاں, رکن->tl_خانے_گنتی) == FAIL)
	أرجع FAIL;

# إذا defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    إذا (رکن_use_palette())
    {
	إذا (رکن->tl_palette != عدم)
	    سیٹ_vterm_palette(رکن->tl_vterm, رکن->tl_palette);
	وإلا
	    init_vterm_ansi_colors(رکن->tl_vterm);
    }
# endif

    // This may change a string in "argvar".
    رکن->tl_کم = کم_شروع(argvar, argv, opt, &رکن->tl_کم);
    إذا (رکن->tl_کم != عدم)
	++رکن->tl_کم->jv_refcount;

    أرجع رکن->tl_کم != عدم
	&& رکن->tl_کم->jv_channel != عدم
	&& رکن->tl_کم->jv_حالت != کم_FAILED ? OK : FAIL;
}

    ساكن صحيح
create_pty_only(رکن_T *رکن, jobopt_T *opt)
{
    إذا (create_vterm(رکن, رکن->tl_صفاں, رکن->tl_خانے_گنتی) == FAIL)
	أرجع FAIL;

    رکن->tl_کم = کم_alloc();
    إذا (رکن->tl_کم == عدم)
	أرجع FAIL;
    ++رکن->tl_کم->jv_refcount;

    // behave like the job is already finished
    رکن->tl_کم->jv_حالت = کم_مکمل;

    أرجع mch_create_pty_channel(رکن->tl_کم, opt);
}

/*
 * Free the terminal emulator part of "term".
 */
    ساكن فراغ
رکن_free_vterm(رکن_T *رکن)
{
    إذا (رکن->tl_vterm != عدم)
	vterm_free(رکن->tl_vterm);
    رکن->tl_vterm = عدم;
}

/*
 * Report the size to the terminal.
 */
    ساكن فراغ
رکن_report_winsize(رکن_T *رکن, صحيح صفاں_2, صحيح خانے_گنتی)
{
    // Use an ioctl() to report the new window size to the job.
    إذا (رکن->tl_کم == عدم || رکن->tl_کم->jv_channel == عدم)
	أرجع;

    صحيح fd = -1;
    صحيح part;

    لكل (part = PART_باہرلا; part < PART_گنتی; ++part)
    {
	fd = رکن->tl_کم->jv_channel->ch_part[part].ch_fd;
	إذا (mch_isatty(fd))
	    اكسر;
    }
    إذا (part < PART_گنتی && mch_report_winsize(fd, صفاں_2, خانے_گنتی) == OK)
	mch_signal_کم(رکن->tl_کم, (char_u *)"winch");
}

#جے_انت

#جے_انت // FEAT_TERMINAL
