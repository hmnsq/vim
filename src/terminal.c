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

#شامل "vim.h"

#اگر defined(FEAT_TERMINAL)

#اگر_غیر_تعریف_شدہ MIN
# define MIN(x,y) ((x) < (y) ? (x) : (y))
#اگر_ختم
#اگر_غیر_تعریف_شدہ MAX
# define MAX(x,y) ((x) > (y) ? (x) : (y))
#اگر_ختم

#شامل "libvterm/include/vterm.h"

// This is VTermScreenCell without the characters, thus much smaller.
قسمتعریف ساخت {
    VTermScreenCellAttrs	attrs;
    حرف			چوڑائی;
    VTermColor			fg;
    VTermColor			bg;
} cellattr_T;

قسمتعریف ساخت sb_سطر_S {
    صحیح		sb_ستون_تعداد;	// can differ per line
    صحیح		sb_بائٹیں;	// length in bytes of text
    cellattr_T	*sb_cells;	// allocated
    cellattr_T	sb_fill_attr;	// for short line
    char_u	*sb_متن;	// for tl_scrollback_postponed
    char_u	continuation;
} sb_سطر_T;

#اگر_تعریف_شدہ MSWIN
# ifndef HPCON
#  define HPCON VOID*
# endif
# ifndef EXTENDED_STARTUPINFO_PRESENT
#  define EXTENDED_STARTUPINFO_PRESENT 0x00080000
# endif
# ifndef PROC_THREAD_ATTRIBUTE_PSEUDOCONSOLE
#  define PROC_THREAD_ATTRIBUTE_PSEUDOCONSOLE 0x00020016
# endif
قسمتعریف ساخت _DYN_STARTUPINFOEXW
{
    STARTUPINFOW StartupInfo;
    LPPROC_THREAD_ATTRIBUTE_LIST lpAttributeList;
} DYN_STARTUPINFOEXW, *PDYN_STARTUPINFOEXW;
#اگر_ختم

// typedef term_T in structs.h
ساخت terminal_S {
    رکن_T	*tl_اگلا;

    VTerm	*tl_vterm;
    کام_T	*tl_کام;
    buf_T	*tl_بفر;
#اگر defined(FEAT_GUI)
    صحیح		tl_system;	// when non-zero used for :!cmd output
    صحیح		tl_toprow;	// row with first line of system terminal
#اگر_ختم

    // Set when setting the size of a vterm, reset after redrawing.
    صحیح		tl_vterm_سائز_بدلا;

    صحیح		tl_معمول_منوال; // TRUE: Terminal-Normal mode
    صحیح		tl_channel_closing;
    صحیح		tl_channel_closed;
    صحیح		tl_channel_recently_closed; // still need to handle tl_finish

    صحیح		tl_اختتام;
#تعریف_کرو TL_اختتام_UNSET	    NUL
#تعریف_کرو TL_اختتام_CLOSE	    'c'	// ++close or :terminal without argument
#تعریف_کرو TL_اختتام_NOCLOSE   'n'	// ++noclose
#تعریف_کرو TL_اختتام_OPEN	    'o'	// ++open
    char_u	*tl_opencmd;
    char_u	*tl_eof_chars;
    char_u	*tl_api;	// prefix for terminal API function

    char_u	*tl_arg0_cmd;	// To format the status bar

#اگر_تعریف_شدہ MSWIN
    خالی	*tl_winpty_config;
    خالی	*tl_winpty;

    HPCON	tl_conpty;
    DYN_STARTUPINFOEXW tl_siex;	// Structure that always needs to be hold

    FILE	*tl_برآمد_fd;
#اگر_ختم
#اگر defined(FEAT_SESSION)
    char_u	*tl_command;
#اگر_ختم
    char_u	*tl_kill;

    // last known vterm size
    صحیح		tl_صفیں;
    صحیح		tl_ستون_تعداد;

    char_u	*tl_title; // NULL or allocated
    char_u	*tl_حالت_متن; // NULL or allocated

    // Range of screen rows to update.  Zero based.
    صحیح		tl_dirty_صف_آغاز; // MAX_ROW if nothing dirty
    صحیح		tl_dirty_صف_انتہا;   // row below last one to update
    صحیح		tl_dirty_snapshot;  // text updated after making snapshot
#اگر_تعریف_شدہ FEAT_TIMERS
    صحیح		tl_timer_سیٹ;
    proftime_T	tl_timer_due;
#اگر_ختم
    صحیح		tl_postponed_scroll;	// to be scrolled up

    garray_T	tl_scrollback;
    صحیح		tl_scrollback_scrolled;
    garray_T	tl_scrollback_postponed;
    صحیح		tl_scrollback_snapshot;
    صحیح		tl_بفر_scrolled;

    char_u	*tl_highlight_name; // replaces "Terminal"; allocated

    cellattr_T	tl_default_color;

    linenr_T	tl_چوٹی_diff_صفیں;   // rows of top diff file or zero
    linenr_T	tl_bot_diff_صفیں;   // rows of bottom diff file

    VTermPos	tl_cursor_pos;
    صحیح		tl_cursor_visible;
    صحیح		tl_cursor_blink;
    صحیح		tl_cursor_shape;  // 1: block, 2: underline, 3: bar
    char_u	*tl_cursor_color; // NULL or allocated

    long_u	*tl_palette; // array of 16 colors specified by term_start, can
			     // be NULL
    صحیح		tl_using_altscreen;
    garray_T	tl_osc_buf;	    // incomplete OSC string
};

#تعریف_کرو TMODE_ONCE 1	    // CTRL-\ CTRL-N used
#تعریف_کرو TMODE_LOOP 2	    // CTRL-W N used

/*
 * List of all active terminals.
 */
ساکن رکن_T *پہلا_رکن = تہی;

// Terminal active in terminal_loop().
ساکن رکن_T *اندر_terminal_loop = تہی;

#اگر_تعریف_شدہ MSWIN
ساکن BOOL has_winpty = FALSE;
ساکن BOOL has_conpty = FALSE;
#اگر_ختم

#تعریف_کرو MAX_صف 999999	    // used for tl_dirty_row_end to update all rows
#تعریف_کرو کلید_BUF_LEN 200

#تعریف_کرو FOR_ALL_TERMS(رکن)	\
    برائے ((رکن) = پہلا_رکن; (رکن) != تہی; (رکن) = (رکن)->tl_اگلا)

/*
 * Functions with separate implementation for MS-Windows and Unix-like systems.
 */
ساکن صحیح رکن_and_کام_init(رکن_T *رکن, typval_T *argvar, حرف **argv, jobopt_T *opt, jobopt_T *orig_opt);
ساکن صحیح create_pty_only(رکن_T *رکن, jobopt_T *opt);
ساکن خالی رکن_report_winsize(رکن_T *رکن, صحیح صفیں_2, صحیح ستون_تعداد);
ساکن خالی رکن_free_vterm(رکن_T *رکن);
#اگر_تعریف_شدہ FEAT_GUI
ساکن خالی update_system_رکن(رکن_T *رکن);
#اگر_ختم

ساکن خالی handle_postponed_scrollback(رکن_T *رکن);

// The character that we know (or assume) that the terminal expects for the
// backspace key.
ساکن صحیح رکن_backspace_char = BS;

// Store the last set and the desired cursor properties, so that we only update
// them when needed.  Doing it unnecessary may result in flicker.
ساکن char_u	*آخری_سیٹ_cursor_color = تہی;
ساکن char_u	*desired_cursor_color = تہی;
ساکن صحیح	آخری_سیٹ_cursor_shape = -1;
ساکن صحیح	desired_cursor_shape = -1;
ساکن صحیح	آخری_سیٹ_cursor_blink = -1;
ساکن صحیح	desired_cursor_blink = -1;


///////////////////////////////////////
// 1. Generic code for all systems.

    ساکن صحیح
cursor_color_equal(char_u *lhs_color, char_u *rhs_color)
{
    اگر (lhs_color != تہی && rhs_color != تہی)
	واپس STRCMP(lhs_color, rhs_color) == 0;
    واپس lhs_color == تہی && rhs_color == تہی;
}

    ساکن خالی
cursor_color_copy(char_u **مقصد_color, char_u *from_color)
{
    // Avoid a free & alloc if the value is already right.
    اگر (cursor_color_equal(*مقصد_color, from_color))
	واپس;
    vim_free(*مقصد_color);
    *مقصد_color = (from_color == تہی) ? تہی : vim_strsave(from_color);
}

    ساکن char_u *
cursor_color_get(char_u *color)
{
    واپس (color == تہی) ? (char_u *)"" : color;
}


/*
 * Parse 'termwinsize' and set "rows" and "cols" for the terminal size in the
 * current window.
 * Sets "rows" and/or "cols" to zero when it should follow the window size.
 * Return TRUE if the size is the minimum size: "24*80".
 */
    ساکن صحیح
parse_termwinsize(win_T *wp, صحیح *صفیں_2, صحیح *ستون_تعداد)
{
    صحیح	minsize = FALSE;

    *صفیں_2 = 0;
    *ستون_تعداد = 0;

    اگر (*wp->w_p_tws == NUL)
	واپس FALSE;

    char_u *p = vim_strchr(wp->w_p_tws, 'x');

    // Syntax of value was already checked when it's set.
    اگر (p == تہی)
    {
	minsize = TRUE;
	p = vim_strchr(wp->w_p_tws, '*');
    }
    *صفیں_2 = atoi((حرف *)wp->w_p_tws);
    *ستون_تعداد = atoi((حرف *)p + 1);
    اگر (*صفیں_2 > VTERM_MAX_صفیں)
	*صفیں_2 = VTERM_MAX_صفیں;
    اگر (*ستون_تعداد > VTERM_MAX_ستون_تعداد)
	*ستون_تعداد = VTERM_MAX_ستون_تعداد;
    واپس minsize;
}

/*
 * Determine the terminal size from 'termwinsize' and the current window.
 */
    ساکن خالی
سیٹ_رکن_and_win_سائز(رکن_T *رکن, jobopt_T *opt)
{
    صحیح صفیں_2, ستون_تعداد;
    صحیح	minsize;

#اگر_تعریف_شدہ FEAT_GUI
    اگر (رکن->tl_system)
    {
	// Use the entire screen (excluding the tabpanel area) for the system
	// commands.  However, it will start at the command line and scroll up
	// as needed, using tl_toprow.
	رکن->tl_صفیں = صفیں;
	رکن->tl_ستون_تعداد = topframe->fr_چوڑائی;
	واپس;
    }
#اگر_ختم
    رکن->tl_صفیں = curwin->w_اونچائی;
    رکن->tl_ستون_تعداد = curwin->w_چوڑائی;

    minsize = parse_termwinsize(curwin, &صفیں_2, &ستون_تعداد);
    اگر (minsize)
    {
	اگر (رکن->tl_صفیں < صفیں_2)
	    رکن->tl_صفیں = صفیں_2;
	اگر (رکن->tl_ستون_تعداد < ستون_تعداد)
	    رکن->tl_ستون_تعداد = ستون_تعداد;
    }
    اگر ((opt->jo_set2 & JO2_رکن_صفیں))
	رکن->tl_صفیں = opt->jo_رکن_صفیں;
    ورنہ اگر (صفیں_2 != 0)
	رکن->tl_صفیں = صفیں_2;
    اگر ((opt->jo_set2 & JO2_رکن_ستون_تعداد))
	رکن->tl_ستون_تعداد = opt->jo_رکن_ستون_تعداد;
    ورنہ اگر (ستون_تعداد != 0)
	رکن->tl_ستون_تعداد = ستون_تعداد;

    اگر (!opt->jo_hidden)
    {
	اگر (رکن->tl_صفیں != curwin->w_اونچائی)
	    win_setheight_win(رکن->tl_صفیں, curwin);
	اگر (رکن->tl_ستون_تعداد != curwin->w_چوڑائی)
	    win_setwidth_win(رکن->tl_ستون_تعداد, curwin);

	// Set 'winsize' now to avoid a resize at the next redraw.
	اگر (!minsize && *curwin->w_p_tws != NUL)
	{
	    char_u buf[100];

	    vim_snprintf((حرف *)buf, 100, "%dx%d",
						 رکن->tl_صفیں, رکن->tl_ستون_تعداد);
	    سیٹ_option_قدر_give_err((char_u *)"termwinsize",
							   0L, buf, OPT_LOCAL);
	}
    }
}

/*
 * Initialize job options for a terminal job.
 * Caller may overrule some of them.
 */
    خالی
init_کام_options(jobopt_T *opt)
{
    clear_کام_options(opt);

    opt->jo_منوال = CH_منوال_RAW;
    opt->jo_برآمد_منوال = CH_منوال_RAW;
    opt->jo_err_منوال = CH_منوال_RAW;
    opt->jo_سیٹ = JO_منوال | JO_برآمد_منوال | JO_ERR_منوال;
}

/*
 * Set job options mandatory for a terminal job.
 */
    ساکن خالی
تیاری_کام_options(jobopt_T *opt, صحیح صفیں_2, صحیح ستون_تعداد)
{
#اگر_غیر_تعریف_شدہ MSWIN
    // Win32: Redirecting the job output won't work, thus always connect stdout
    // here.
    اگر (!(opt->jo_سیٹ & JO_برآمد_IO))
#اگر_ختم
    {
	// Connect stdout to the terminal.
	opt->jo_io[PART_برآمد] = JIO_بفر;
	opt->jo_io_buf[PART_برآمد] = curbuf->b_fnum;
	opt->jo_modifiable[PART_برآمد] = 0;
	opt->jo_سیٹ |= JO_برآمد_IO + JO_برآمد_BUF + JO_برآمد_MODIFIABLE;
    }

#اگر_غیر_تعریف_شدہ MSWIN
    // Win32: Redirecting the job output won't work, thus always connect stderr
    // here.
    اگر (!(opt->jo_سیٹ & JO_ERR_IO))
#اگر_ختم
    {
	// Connect stderr to the terminal.
	opt->jo_io[PART_ERR] = JIO_بفر;
	opt->jo_io_buf[PART_ERR] = curbuf->b_fnum;
	opt->jo_modifiable[PART_ERR] = 0;
	opt->jo_سیٹ |= JO_ERR_IO + JO_ERR_BUF + JO_ERR_MODIFIABLE;
    }

    opt->jo_pty = TRUE;
    اگر ((opt->jo_set2 & JO2_رکن_صفیں) == 0)
	opt->jo_رکن_صفیں = صفیں_2;
    اگر ((opt->jo_set2 & JO2_رکن_ستون_تعداد) == 0)
	opt->jo_رکن_ستون_تعداد = ستون_تعداد;
}

/*
 * Flush messages on channels.
 */
    ساکن خالی
رکن_flush_messages(خالی)
{
    mch_check_messages();
    parse_queued_messages();
}

/*
 * Close a terminal buffer (and its window).  Used when creating the terminal
 * fails.
 */
    ساکن خالی
رکن_close_بفر(buf_T *buf, buf_T *old_curbuf)
{
    free_terminal(buf);
    اگر (old_curbuf != تہی)
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
رکن_آغاز(
	typval_T    *argvar,
	حرف	    **argv,
	jobopt_T    *opt,
	صحیح	    flags)
{
    exarg_T	بانٹو_ea;
    win_T	*old_curwin = curwin;
    رکن_T	*رکن;
    buf_T	*old_curbuf = تہی;
    صحیح		res;
    buf_T	*newbuf;
    صحیح		vertical = opt->jo_vertical || (cmdmod.cmod_بانٹو & WSP_VERT);
    jobopt_T	orig_opt;  // only partly filled
    pos_T	save_cursor;

    اگر (check_restricted() || check_secure())
	واپس تہی;
    اگر (cmdwin_type != 0)
    {
	emsg(_(e_cannot_open_terminal_from_command_سطر_کھڑکی));
	واپس تہی;
    }

    اگر ((opt->jo_سیٹ & (JO_اندر_IO + JO_برآمد_IO + JO_ERR_IO))
					 == (JO_اندر_IO + JO_برآمد_IO + JO_ERR_IO)
	|| (!(opt->jo_سیٹ & JO_برآمد_IO) && (opt->jo_سیٹ & JO_برآمد_BUF))
	|| (!(opt->jo_سیٹ & JO_ERR_IO) && (opt->jo_سیٹ & JO_ERR_BUF))
	|| (argvar != تہی
	    && argvar->v_type == VAR_LIST
	    && argvar->vval.v_list != تہی
	    && argvar->vval.v_list->lv_پہلا == &range_list_شے))
    {
	emsg(_(e_invalid_دلیل));
	واپس تہی;
    }

    رکن = ALLOC_CLEAR_ONE(رکن_T);
    اگر (رکن == تہی)
	واپس تہی;
    رکن->tl_dirty_صف_انتہا = MAX_صف;
    رکن->tl_cursor_visible = TRUE;
    رکن->tl_cursor_shape = VTERM_PROP_CURSORSHAPE_بلاک;
    رکن->tl_اختتام = opt->jo_رکن_اختتام;
#اگر_تعریف_شدہ FEAT_GUI
    رکن->tl_system = (flags & رکن_آغاز_SYSTEM);
#اگر_ختم
    ga_init2(&رکن->tl_scrollback, حجم(sb_سطر_T), 300);
    ga_init2(&رکن->tl_scrollback_postponed, حجم(sb_سطر_T), 300);
    ga_init2(&رکن->tl_osc_buf, حجم(حرف), 300);

    setpcmark();
    CLEAR_میدان(بانٹو_ea);
    اگر (opt->jo_curwin)
    {
	// Create a new buffer in the current window.
	اگر (!can_abandon(curbuf, flags & رکن_آغاز_FORCEIT))
	{
	    no_write_پیغام();
	    vim_free(رکن);
	    واپس تہی;
	}
	اگر (do_ecmd(0, تہی, تہی, &بانٹو_ea, ECMD_ONE,
		      (buf_hide(curwin->w_بفر) ? ECMD_HIDE : 0)
			  + ((flags & رکن_آغاز_FORCEIT) ? ECMD_FORCEIT : 0),
							       curwin) == FAIL)
	{
	    vim_free(رکن);
	    واپس تہی;
	}
    }
    ورنہ اگر (opt->jo_hidden || (flags & رکن_آغاز_SYSTEM))
    {
	buf_T *buf;

	// Create a new buffer without a window. Make it the current buffer for
	// a moment to be able to do the initializations.
	buf = buflist_new((char_u *)"", تہی, (linenr_T)0,
							 BLN_NEW | BLN_LISTED);
	اگر (buf == تہی || ml_open(buf) == FAIL)
	{
	    vim_free(رکن);
	    واپس تہی;
	}
	old_curbuf = curbuf;
	--curbuf->b_nwindows;
	curbuf = buf;
	save_cursor = curwin->w_cursor;
	curwin->w_بفر = buf;
	++curbuf->b_nwindows;
    }
    ورنہ
    {
	// Open a new window or tab.
	بانٹو_ea.cmdidx = CMD_new;
	بانٹو_ea.cmd = (char_u *)"new";
	بانٹو_ea.arg = (char_u *)"";
	اگر (opt->jo_رکن_صفیں > 0 && !vertical)
	{
	    بانٹو_ea.line2 = opt->jo_رکن_صفیں;
	    بانٹو_ea.addr_گنتی = 1;
	}
	اگر (opt->jo_رکن_ستون_تعداد > 0 && vertical)
	{
	    بانٹو_ea.line2 = opt->jo_رکن_ستون_تعداد;
	    بانٹو_ea.addr_گنتی = 1;
	}

	صحیح cmod_بانٹو_modified = FALSE;
	اگر (vertical)
	{
	    اگر (!(cmdmod.cmod_بانٹو & WSP_VERT))
		cmod_بانٹو_modified = TRUE;
	    cmdmod.cmod_بانٹو |= WSP_VERT;
	}
	ex_splitview(&بانٹو_ea);
	اگر (cmod_بانٹو_modified)
	    cmdmod.cmod_بانٹو &= ~WSP_VERT;
	اگر (curwin == old_curwin)
	{
	    // split failed
	    vim_free(رکن);
	    واپس تہی;
	}
    }
    رکن->tl_بفر = curbuf;
    curbuf->b_رکن = رکن;

    اگر (!opt->jo_hidden)
    {
	// Only one size was taken care of with :new, do the other one.  With
	// "curwin" both need to be done.
	اگر (opt->jo_رکن_صفیں > 0 && (opt->jo_curwin || vertical))
	    win_setheight(opt->jo_رکن_صفیں);
	اگر (opt->jo_رکن_ستون_تعداد > 0 && (opt->jo_curwin || !vertical))
	    win_setwidth(opt->jo_رکن_ستون_تعداد);
    }

    // Link the new terminal in the list of active terminals.
    رکن->tl_اگلا = پہلا_رکن;
    پہلا_رکن = رکن;

    apply_autocmds(EVENT_BUFFILEPRE, تہی, تہی, FALSE, curbuf);

    اگر (opt->jo_رکن_name != تہی)
    {
	vim_free(curbuf->b_ffname);
	curbuf->b_ffname = vim_strsave(opt->jo_رکن_name);
    }
    ورنہ اگر (argv != تہی)
    {
	vim_free(curbuf->b_ffname);
	curbuf->b_ffname = vim_strsave((char_u *)"!system");
    }
    ورنہ
    {
	صحیح	i;
	size_t	len;
	char_u	*cmd, *p;

	اگر (argvar->v_type == VAR_STRING)
	{
	    cmd = argvar->vval.v_string;
	    اگر (cmd == تہی)
		cmd = (char_u *)"";
	    ورنہ اگر (STRCMP(cmd, "NONE") == 0)
		cmd = (char_u *)"pty";
	}
	ورنہ اگر (argvar->v_type != VAR_LIST
		|| argvar->vval.v_list == تہی
		|| argvar->vval.v_list->lv_len == 0
		|| (cmd = tv_get_string_chk(
			       &argvar->vval.v_list->lv_پہلا->li_tv)) == تہی)
	    cmd = (char_u*)"";

	len = STRLEN(cmd) + 10;
	p = alloc(len);

	برائے (i = 0; p != تہی; ++i)
	{
	    // Prepend a ! to the command name to avoid the buffer name equals
	    // the executable, otherwise ":w!" would overwrite it.
	    اگر (i == 0)
		vim_snprintf((حرف *)p, len, "!%s", cmd);
	    ورنہ
		vim_snprintf((حرف *)p, len, "!%s (%d)", cmd, i);
	    اگر (buflist_findname(p) == تہی)
	    {
		vim_free(curbuf->b_ffname);
		curbuf->b_ffname = p;
		توڑو;
	    }
	}
    }
    vim_free(curbuf->b_sfname);
    curbuf->b_sfname = vim_strsave(curbuf->b_ffname);
    curbuf->b_fname = curbuf->b_ffname;

    apply_autocmds(EVENT_BUFFILEPOST, تہی, تہی, FALSE, curbuf);

    اگر (opt->jo_رکن_opencmd != تہی)
	رکن->tl_opencmd = vim_strsave(opt->jo_رکن_opencmd);

    اگر (opt->jo_eof_chars != تہی)
	رکن->tl_eof_chars = vim_strsave(opt->jo_eof_chars);

    سیٹ_string_option_براہ_راست((char_u *)"buftype", -1,
				  (char_u *)"terminal", OPT_FREE|OPT_LOCAL, 0);
    // Avoid that 'buftype' is reset when this buffer is entered.
    curbuf->b_p_initialized = true;

    // Mark the buffer as not modifiable. It can only be made modifiable after
    // the job finished.
    curbuf->b_p_ma = FALSE;

    سیٹ_رکن_and_win_سائز(رکن, opt);
#اگر_تعریف_شدہ MSWIN
    mch_memmove(orig_opt.jo_io, opt->jo_io, حجم(orig_opt.jo_io));
#اگر_ختم
    تیاری_کام_options(opt, رکن->tl_صفیں, رکن->tl_ستون_تعداد);

    اگر (flags & رکن_آغاز_NOJOB)
	واپس curbuf;

#اگر defined(FEAT_SESSION)
    // Remember the command for the session file.
    اگر (opt->jo_رکن_norestore || argv != تہی)
	رکن->tl_command = vim_strsave((char_u *)"NONE");
    ورنہ اگر (argvar->v_type == VAR_STRING)
    {
	char_u	*cmd = argvar->vval.v_string;

	اگر (cmd != تہی && STRCMP(cmd, p_sh) != 0)
	    رکن->tl_command = vim_strsave(cmd);
    }
    ورنہ اگر (argvar->v_type == VAR_LIST
	    && argvar->vval.v_list != تہی
	    && argvar->vval.v_list->lv_len > 0)
    {
	garray_T	ga;
	listitem_T	*شے;

	ga_init2(&ga, 1, 100);
	FOR_ALL_LIST_اشیاء(argvar->vval.v_list, شے)
	{
	    char_u *s = tv_get_string_chk(&شے->li_tv);
	    char_u *p;

	    اگر (s == تہی)
		توڑو;
	    p = vim_strsave_fnameescape(s, VSE_NONE);
	    اگر (p == تہی)
		توڑو;
	    ga_concat(&ga, p);
	    vim_free(p);
	    ga_append(&ga, ' ');
	}
	اگر (شے == تہی)
	{
	    ga_append(&ga, NUL);
	    رکن->tl_command = ga.ga_ڈیٹا;
	}
	ورنہ
	    ga_clear(&ga);
    }
#اگر_ختم

    اگر (opt->jo_رکن_kill != تہی)
    {
	char_u *p = skiptowhite(opt->jo_رکن_kill);

	رکن->tl_kill = vim_strnsave(opt->jo_رکن_kill, p - opt->jo_رکن_kill);
    }

    اگر (opt->jo_رکن_api != تہی)
    {
	char_u *p = skiptowhite(opt->jo_رکن_api);

	رکن->tl_api = vim_strnsave(opt->jo_رکن_api, p - opt->jo_رکن_api);
    }
    ورنہ
	رکن->tl_api = vim_strsave((char_u *)"Tapi_");

    اگر (opt->jo_set2 & JO2_رکن_HIGHLIGHT)
	رکن->tl_highlight_name = vim_strsave(opt->jo_رکن_highlight);

#اگر defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    // Save the user-defined palette, it is only used in GUI (or 'tgc' is on).
    اگر (opt->jo_set2 & JO2_ANSI_COLORS)
    {
	رکن->tl_palette = ALLOC_MULT(long_u, 16);
	اگر (رکن->tl_palette == تہی)
	{
	    vim_free(رکن);
	    واپس تہی;
	}
	حافظہ_نقل(رکن->tl_palette, opt->jo_ansi_colors, حجم(long_u) * 16);
    }
#اگر_ختم

    // System dependent: setup the vterm and maybe start the job in it.
    اگر (argv == تہی
	    && argvar->v_type == VAR_STRING
	    && argvar->vval.v_string != تہی
	    && STRCMP(argvar->vval.v_string, "NONE") == 0)
	res = create_pty_only(رکن, opt);
    ورنہ
	res = رکن_and_کام_init(رکن, argvar, argv, opt, &orig_opt);

    newbuf = curbuf;
    اگر (res == OK)
    {
	// Get and remember the size we ended up with.  Update the pty.
	vterm_get_سائز(رکن->tl_vterm, &رکن->tl_صفیں, &رکن->tl_ستون_تعداد);
	رکن_report_winsize(رکن, رکن->tl_صفیں, رکن->tl_ستون_تعداد);
#اگر_تعریف_شدہ FEAT_GUI
	اگر (رکن->tl_system)
	{
	    // display first line below typed command
	    رکن->tl_toprow = پیغام_صف + 1;
	    رکن->tl_dirty_صف_انتہا = 0;
	}
#اگر_ختم

	// Make sure we don't get stuck on sending keys to the job, it leads to
	// a deadlock if the job is waiting for Vim to read.
	channel_سیٹ_nonblock(رکن->tl_کام->jv_channel, PART_اندر);

	اگر (old_curbuf != تہی)
	{
	    --curbuf->b_nwindows;
	    curbuf = old_curbuf;
	    curwin->w_بفر = curbuf;
	    curwin->w_cursor = save_cursor;
	    ++curbuf->b_nwindows;
	}
	ورنہ اگر (vgetc_busy
#اگر_تعریف_شدہ FEAT_TIMERS
		|| timer_busy
#اگر_ختم
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
    ورنہ
    {
	رکن_close_بفر(curbuf, old_curbuf);
	واپس تہی;
    }

    apply_autocmds(EVENT_TERMINALOPEN, تہی, تہی, FALSE, newbuf);
    اگر (!opt->jo_hidden && !(flags & رکن_آغاز_SYSTEM))
	apply_autocmds(EVENT_TERMINALWINOPEN, تہی, تہی, FALSE, newbuf);
    واپس newbuf;
}

/*
 * ":terminal": open a terminal window and execute a job in it.
 */
    خالی
ex_terminal(exarg_T *eap)
{
    typval_T	argvar[2];
    jobopt_T	opt;
    صحیح		opt_shell = FALSE;
    char_u	*cmd;
    char_u	*tofree = تہی;
    صحیح		scroll_save = پیغام_scroll;

    پیغام_scroll = FALSE;		// don't scroll here
    autowrite_all();
    پیغام_scroll = scroll_save;

    init_کام_options(&opt);

    cmd = eap->arg;
    جب (*cmd == '+' && *(cmd + 1) == '+')
    {
	char_u  *p, *ep;

	cmd += 2;
	p = skiptowhite(cmd);
	ep = vim_strchr(cmd, '=');
	اگر (ep != تہی)
	{
	    اگر (ep < p)
		p = ep;
	    ورنہ
		ep = تہی;
	}

	// Note: Keep this in sync with get_terminalopt_name.

#تعریف_کرو OPTARG_HAS(name) ((صحیح)(p - cmd) == حجم(name) - 1 \
				 && STRNICMP(cmd, name, حجم(name) - 1) == 0)
	اگر (OPTARG_HAS("close"))
	    opt.jo_رکن_اختتام = 'c';
	ورنہ اگر (OPTARG_HAS("noclose"))
	    opt.jo_رکن_اختتام = 'n';
	ورنہ اگر (OPTARG_HAS("open"))
	    opt.jo_رکن_اختتام = 'o';
	ورنہ اگر (OPTARG_HAS("curwin"))
	    opt.jo_curwin = 1;
	ورنہ اگر (OPTARG_HAS("hidden"))
	    opt.jo_hidden = 1;
	ورنہ اگر (OPTARG_HAS("norestore"))
	    opt.jo_رکن_norestore = 1;
	ورنہ اگر (OPTARG_HAS("shell"))
	    opt_shell = TRUE;
	ورنہ اگر (OPTARG_HAS("kill") && ep != تہی)
	{
	    opt.jo_set2 |= JO2_رکن_KILL;
	    opt.jo_رکن_kill = ep + 1;
	    p = skiptowhite(cmd);
	}
	ورنہ اگر (OPTARG_HAS("api"))
	{
	    opt.jo_set2 |= JO2_رکن_API;
	    اگر (ep != تہی)
	    {
		opt.jo_رکن_api = ep + 1;
		p = skiptowhite(cmd);
	    }
	    ورنہ
		opt.jo_رکن_api = تہی;
	}
	ورنہ اگر (OPTARG_HAS("rows") && ep != تہی && محفوظ_isdigit(ep[1]))
	{
	    opt.jo_set2 |= JO2_رکن_صفیں;
	    opt.jo_رکن_صفیں = atoi((حرف *)ep + 1);
	    p = skiptowhite(cmd);
	}
	ورنہ اگر (OPTARG_HAS("cols") && ep != تہی && محفوظ_isdigit(ep[1]))
	{
	    opt.jo_set2 |= JO2_رکن_ستون_تعداد;
	    opt.jo_رکن_ستون_تعداد = atoi((حرف *)ep + 1);
	    p = skiptowhite(cmd);
	}
	ورنہ اگر (OPTARG_HAS("eof") && ep != تہی)
	{
	    char_u *buf = تہی;
	    char_u *کلیدیں;

	    vim_free(opt.jo_eof_chars);
	    p = skiptowhite(cmd);
	    *p = NUL;
	    کلیدیں = replace_termcodes(ep + 1, &buf, 0,
		    REPTERM_FROM_PART | REPTERM_DO_LT | REPTERM_SPECIAL, تہی);
	    opt.jo_set2 |= JO2_EOF_CHARS;
	    opt.jo_eof_chars = vim_strsave(کلیدیں);
	    vim_free(buf);
	    *p = ' ';
	}
#اگر_تعریف_شدہ MSWIN
	ورنہ اگر ((صحیح)(p - cmd) == 4 && STRNICMP(cmd, "type", 4) == 0
								 && ep != تہی)
	{
	    صحیح tty_type = NUL;

	    p = skiptowhite(cmd);
	    اگر (STRNICMP(ep + 1, "winpty", p - (ep + 1)) == 0)
		tty_type = 'w';
	    ورنہ اگر (STRNICMP(ep + 1, "conpty", p - (ep + 1)) == 0)
		tty_type = 'c';
	    ورنہ
	    {
		semsg(_(e_invalid_قدر_for_دلیل_str), "type");
		جاؤ theend;
	    }
	    opt.jo_set2 |= JO2_TTY_TYPE;
	    opt.jo_tty_type = tty_type;
	}
#اگر_ختم
	ورنہ
	{
	    اگر (*p)
		*p = NUL;
	    semsg(_(e_invalid_attribute_str), cmd);
	    جاؤ theend;
	}
#تعریف_منسوخ OPTARG_HAS
	cmd = skipwhite(p);
    }
    اگر (*cmd == NUL)
    {
	// Make a copy of 'shell', an autocommand may change the option.
	tofree = cmd = vim_strsave(p_sh);

	// default to close when the shell exits
	اگر (opt.jo_رکن_اختتام == NUL)
	    opt.jo_رکن_اختتام = TL_اختتام_CLOSE;
    }

    اگر (eap->addr_گنتی > 0)
    {
	// Write lines from current buffer to the job.
	opt.jo_سیٹ |= JO_اندر_IO | JO_اندر_BUF | JO_اندر_چوٹی | JO_اندر_BOT;
	opt.jo_io[PART_اندر] = JIO_بفر;
	opt.jo_io_buf[PART_اندر] = curbuf->b_fnum;
	opt.jo_اندر_چوٹی = eap->line1;
	opt.jo_اندر_bot = eap->line2;
    }

    اگر (opt_shell && tofree == تہی)
    {
#اگر_تعریف_شدہ UNIX
	حرف	**argv = تہی;
	char_u	*tofree1 = تہی;
	char_u	*tofree2 = تہی;

	// :term ++shell command
	اگر (unix_build_argv(cmd, &argv, &tofree1, &tofree2) == OK)
	    رکن_آغاز(تہی, argv, &opt, eap->forceit ? رکن_آغاز_FORCEIT : 0);
	vim_free(argv);
	vim_free(tofree1);
	vim_free(tofree2);
	جاؤ theend;
#ورنہ
# ifdef MSWIN
	long_u	    cmdlen = STRLEN(p_sh) + STRLEN(p_shcf) + STRLEN(cmd) + 10;
	char_u	    *newcmd;

	newcmd = alloc(cmdlen);
	اگر (newcmd == تہی)
	    جاؤ theend;
	tofree = newcmd;
	vim_snprintf((حرف *)newcmd, cmdlen, "%s %s %s", p_sh, p_shcf, cmd);
	cmd = newcmd;
# ورنہ
	emsg(_(e_sorry_plusplusshell_not_supported_on_this_system));
	جاؤ theend;
# endif
#اگر_ختم
    }
    argvar[0].v_type = VAR_STRING;
    argvar[0].vval.v_string = cmd;
    argvar[1].v_type = VAR_UNKNOWN;
    رکن_آغاز(argvar, تہی, &opt, eap->forceit ? رکن_آغاز_FORCEIT : 0);

theend:
    vim_free(tofree);
    vim_free(opt.jo_eof_chars);
}

    ساکن char_u *
get_terminalopt_name(expand_T *xp غیر_مستعمل, صحیح idx)
{
    // Note: Keep this in sync with ex_terminal.
    ساکن حرف *(p_termopt_اقدار[]) =
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

    اگر (idx < (صحیح)ARRAY_لمبائی(p_termopt_اقدار))
	واپس (char_u*)p_termopt_اقدار[idx];
    واپس تہی;
}

    ساکن char_u *
get_termkill_name(expand_T *xp غیر_مستعمل, صحیح idx)
{
    // These are platform-specific values used for job_stop(). They are defined
    // in each platform's mch_signal_job(). Just use a unified auto-complete
    // list for simplicity.
    ساکن حرف *(p_termkill_اقدار[]) =
    {
	"term",
	"hup",
	"quit",
	"int",
	"kill",
	"winch",
    };

    اگر (idx < (صحیح)ARRAY_لمبائی(p_termkill_اقدار))
	واپس (char_u*)p_termkill_اقدار[idx];
    واپس تہی;
}

/*
 * Command-line expansion for :terminal [options]
 */
    صحیح
expand_terminal_opt(
	char_u	    *pat,
	expand_T    *xp,
	regmatch_T  *rmp,
	char_u	    ***matches,
	صحیح	    *عدد_Matches)
{
    اگر (xp->xp_نمونہ > xp->xp_سطر && *(xp->xp_نمونہ-1) == '=')
    {
	char_u *(*cb)(expand_T *, صحیح) = تہی;

	char_u *name_انتہا = xp->xp_نمونہ - 1;
	اگر (name_انتہا - xp->xp_سطر >= 4
		&& STRNCMP(name_انتہا - 4, "kill", 4) == 0)
	    cb = get_termkill_name;

	اگر (cb != تہی)
	{
	    واپس ExpandGeneric(
		    pat,
		    xp,
		    rmp,
		    matches,
		    عدد_Matches,
		    cb,
		    FALSE);
	}
	واپس FAIL;
    }
    واپس ExpandGeneric(
	    pat,
	    xp,
	    rmp,
	    matches,
	    عدد_Matches,
	    get_terminalopt_name,
	    FALSE);
}

#اگر defined(FEAT_SESSION)
/*
 * Write a :terminal command to the session file to restore the terminal in
 * window "wp".
 * Return FAIL if writing fails.
 */
    صحیح
رکن_write_session(FILE *fd, win_T *wp, hashtab_T *terminal_bufs)
{
    ثابت صحیح	bufnr = wp->w_بفر->b_fnum;
    رکن_T	*رکن = wp->w_بفر->b_رکن;

    اگر (terminal_bufs != تہی && wp->w_بفر->b_nwindows > 1)
    {
	// There are multiple views into this terminal buffer. We don't want to
	// create the terminal multiple times. If it's the first time, create,
	// otherwise link to the first buffer.
	حرف	    id_as_str[NUMBUFLEN];
	hashitem_T  *اندراج_2;

	vim_snprintf(id_as_str, حجم(id_as_str), "%d", bufnr);

	اندراج_2 = hash_ڈھونڈو(terminal_bufs, (char_u *)id_as_str);
	اگر (!HASHITEM_خالی_خانے(اندراج_2))
	{
	    // we've already opened this terminal buffer
	    اگر (fprintf(fd, "execute 'buffer ' .. term_buf_%d", bufnr) < 0)
		واپس FAIL;
	    واپس put_eol(fd);
	}
    }

    // Create the terminal and run the command.  This is not without
    // risk, but let's assume the user only creates a session when this
    // will be OK.
    اگر (fprintf(fd, "exe ':terminal ++curwin"
		" ++cols=' .. ((&columns * %d + %ld) / %ld)"
		" .. ' ++rows=' .. ((&lines * %d + %ld) / %ld) ",
		رکن->tl_ستون_تعداد, Columns / 2, Columns,
		رکن->tl_صفیں, صفیں / 2, صفیں) < 0)
	واپس FAIL;
# ifdef MSWIN
    اگر (fprintf(fd, ".. ' ++type=%s' ", رکن->tl_کام->jv_tty_type) < 0)
	واپس FAIL;
# endif
    اگر (رکن->tl_command != تہی)
    {
	char_u *quoted_command = string_quote(رکن->tl_command, FALSE);
	اگر (quoted_command == تہی)
	    واپس FAIL;

	صحیح ret = fputs(".. ' ' .. ", fd) < 0
		    || fputs((حرف *)quoted_command, fd) < 0;
	vim_free(quoted_command);
	اگر (ret)
	    واپس FAIL;
    }
    اگر (put_eol(fd) != OK)
	واپس FAIL;

    اگر (fprintf(fd, "var term_buf_%d: number = bufnr()", bufnr) < 0)
	واپس FAIL;

    اگر (terminal_bufs != تہی && wp->w_بفر->b_nwindows > 1)
    {
	حرف *hash_کلید = alloc(NUMBUFLEN);

	vim_snprintf(hash_کلید, NUMBUFLEN, "%d", bufnr);
	hash_جوڑو(terminal_bufs, (char_u *)hash_کلید, "terminal session");
    }

    واپس put_eol(fd);
}

/*
 * Return TRUE if "buf" has a terminal that should be restored.
 */
    صحیح
رکن_should_restore(buf_T *buf)
{
    رکن_T	*رکن = buf->b_رکن;

    واپس رکن != تہی && (رکن->tl_command == تہی
				     || STRCMP(رکن->tl_command, "NONE") != 0);
}
#اگر_ختم

/*
 * Free the scrollback buffer for "term".
 */
    ساکن خالی
free_scrollback(رکن_T *رکن)
{
    صحیح i;

    برائے (i = 0; i < رکن->tl_scrollback.ga_len; ++i)
	vim_free(((sb_سطر_T *)رکن->tl_scrollback.ga_ڈیٹا + i)->sb_cells);
    ga_clear(&رکن->tl_scrollback);
    برائے (i = 0; i < رکن->tl_scrollback_postponed.ga_len; ++i)
	vim_free(((sb_سطر_T *)رکن->tl_scrollback_postponed.ga_ڈیٹا + i)->sb_cells);
    ga_clear(&رکن->tl_scrollback_postponed);
}


// Terminals that need to be freed soon.
ساکن رکن_T	*terminals_مقصد_free = تہی;

/*
 * Free a terminal and everything it refers to.
 * Kills the job if there is one.
 * Called when wiping out a buffer.
 * The actual terminal structure is freed later in free_unused_terminals(),
 * because callbacks may wipe out a buffer while the terminal is still
 * referenced.
 */
    خالی
free_terminal(buf_T *buf)
{
    رکن_T	*رکن = buf->b_رکن;
    رکن_T	*tp;

    اگر (رکن == تہی)
	واپس;

    // Unlink the terminal form the list of terminals.
    اگر (پہلا_رکن == رکن)
	پہلا_رکن = رکن->tl_اگلا;
    ورنہ
	برائے (tp = پہلا_رکن; tp->tl_اگلا != تہی; tp = tp->tl_اگلا)
	    اگر (tp->tl_اگلا == رکن)
	    {
		tp->tl_اگلا = رکن->tl_اگلا;
		توڑو;
	    }

    اگر (رکن->tl_کام != تہی)
    {
	اگر (رکن->tl_کام->jv_حالت != کام_ENDED
		&& رکن->tl_کام->jv_حالت != کام_ختم_شدہ
		&& رکن->tl_کام->jv_حالت != کام_FAILED)
	    کام_stop(رکن->tl_کام, تہی, "kill");
	کام_unref(رکن->tl_کام);
    }
    رکن->tl_اگلا = terminals_مقصد_free;
    terminals_مقصد_free = رکن;

    buf->b_رکن = تہی;
    اگر (اندر_terminal_loop == رکن)
	اندر_terminal_loop = تہی;
}

    خالی
free_غیر_مستعمل_terminals(خالی)
{
    جب (terminals_مقصد_free != تہی)
    {
	رکن_T	    *رکن = terminals_مقصد_free;

	terminals_مقصد_free = رکن->tl_اگلا;

	free_scrollback(رکن);
	ga_clear(&رکن->tl_osc_buf);

	رکن_free_vterm(رکن);
	vim_free(رکن->tl_api);
	vim_free(رکن->tl_title);
#اگر_تعریف_شدہ FEAT_SESSION
	vim_free(رکن->tl_command);
#اگر_ختم
	vim_free(رکن->tl_kill);
	vim_free(رکن->tl_حالت_متن);
	vim_free(رکن->tl_opencmd);
	vim_free(رکن->tl_eof_chars);
	vim_free(رکن->tl_arg0_cmd);
#اگر_تعریف_شدہ MSWIN
	اگر (رکن->tl_برآمد_fd != تہی)
	    fclose(رکن->tl_برآمد_fd);
#اگر_ختم
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
    ساکن ch_part_T
get_tty_part(رکن_T *رکن غیر_مستعمل)
{
#اگر_تعریف_شدہ UNIX
    ch_part_T	parts[3] = {PART_اندر, PART_برآمد, PART_ERR};
    صحیح		i;

    برائے (i = 0; i < 3; ++i)
    {
	صحیح fd = رکن->tl_کام->jv_channel->ch_part[parts[i]].ch_fd;

	اگر (mch_isatty(fd))
	    واپس parts[i];
    }
#اگر_ختم
    واپس PART_اندر;
}

/*
 * Read any vterm output and send it on the channel.
 */
    ساکن خالی
رکن_forward_output(رکن_T *رکن)
{
    VTerm *vterm = رکن->tl_vterm;
    حرف   buf[کلید_BUF_LEN];
    size_t curlen = vterm_output_read(vterm, buf, کلید_BUF_LEN);

    اگر (curlen > 0)
	channel_send(رکن->tl_کام->jv_channel, get_tty_part(رکن),
					     (char_u *)buf, (صحیح)curlen, تہی);
}

/*
 * Write job output "msg[len]" to the vterm.
 */
    ساکن خالی
رکن_write_کام_output(رکن_T *رکن, char_u *پیغام_arg, size_t len_arg)
{
    char_u	*پیغام = پیغام_arg;
    size_t	len = len_arg;
    VTerm	*vterm = رکن->tl_vterm;
    size_t	prevlen = vterm_output_get_بفر_current(vterm);
    size_t	حد_بندی = رکن->tl_بفر->b_p_twsl * رکن->tl_ستون_تعداد * 3;

    // Limit the length to 'termwinscroll' * cols * 3 bytes.  Keep the text at
    // the end.
    اگر (len > حد_بندی)
    {
	char_u *p = پیغام + len - حد_بندی;

	p -= (*mb_سر_off)(پیغام, p);
	len -= p - پیغام;
	پیغام = p;
    }

    vterm_داخلہ_write(vterm, (حرف *)پیغام, len);

    // flush vterm buffer when vterm responded to control sequence
    اگر (prevlen != vterm_output_get_بفر_current(vterm))
	رکن_forward_output(رکن);

    // this invokes the damage callbacks
    vterm_screen_flush_damage(vterm_obtain_screen(vterm));
}

    ساکن خالی
position_cursor(win_T *wp, VTermPos *pos)
{
    wp->w_wrow = MIN(pos->صف, MAX(0, wp->w_اونچائی - 1));
    wp->w_wcol = MIN(pos->ستون, MAX(0, wp->w_چوڑائی - 1));
#اگر_تعریف_شدہ FEAT_PROP_POPUP
    اگر (popup_is_popup(wp))
    {
	wp->w_wrow += popup_چوٹی_extra(wp);
	wp->w_wcol += popup_بایاں_extra(wp);
	wp->w_flags |= WFLAG_WCOL_OFF_ADDED | WFLAG_WROW_OFF_ADDED;
    }
    ورنہ
	wp->w_flags &= ~(WFLAG_WCOL_OFF_ADDED | WFLAG_WROW_OFF_ADDED);
#اگر_ختم
    wp->w_valid |= (VALID_WCOL|VALID_WROW);
}

    ساکن خالی
update_cursor(رکن_T *رکن, صحیح redraw)
{
    اگر (رکن->tl_معمول_منوال)
	واپس;
#اگر_تعریف_شدہ FEAT_GUI
    اگر (رکن->tl_system)
	windgoto(رکن->tl_cursor_pos.صف + رکن->tl_toprow,
		firstwin->w_wincol + رکن->tl_cursor_pos.ستون);
    ورنہ
#اگر_ختم
    اگر (!رکن_کام_running(رکن))
	// avoid the cursor positioned below the last used line
	setcursor();
    ورنہ
    {
	// do not use the window cursor position
	position_cursor(curwin, &curbuf->b_رکن->tl_cursor_pos);
	windgoto(W_WINROW(curwin) + curwin->w_wrow,
		 curwin->w_wincol + curwin->w_wcol);
    }
    اگر (redraw)
    {
	aco_save_T	aco;

	اگر (رکن->tl_بفر == curbuf && رکن->tl_cursor_visible)
	    cursor_on();
	برآمد_flush();
#اگر_تعریف_شدہ FEAT_GUI
	اگر (gui.اندر_use)
	{
	    gui_update_cursor(FALSE, FALSE);
	    gui_mch_flush();
	}
#اگر_ختم
	// Make sure an invoked autocmd doesn't delete the buffer (and the
	// terminal) under our fingers.
	++رکن->tl_بفر->b_locked;

	// save and restore curwin and curbuf, in case the autocmd changes them
	aucmd_prepbuf(&aco, curbuf);
	apply_autocmds(EVENT_TEXTCHANGEDT, تہی, تہی, FALSE, رکن->tl_بفر);
	aucmd_restbuf(&aco);

	--رکن->tl_بفر->b_locked;
    }
}

/*
 * Find the location of a scrollbackline in the buffer
 */
    ساکن خالی
scrollbackline_pos_اندر_buf(رکن_T *رکن, صحیح صف, linenr_T *lnum, صحیح *آغاز_ستون, size_t *آغاز_pos)
{
    sb_سطر_T	*سطریں = (sb_سطر_T *)رکن->tl_scrollback.ga_ڈیٹا;
    linenr_T    calc_lnum = رکن->tl_بفر_scrolled;
    size_t      calc_pos = 0;
    صحیح         calc_ستون = 0;
    صحیح         i;

    اگر (صف < 0 || صف >= رکن->tl_scrollback.ga_len)
	واپس;

    اگر (صف > رکن->tl_scrollback_scrolled)
    {
	// Lookback how far along in the top line we are
	برائے (i = رکن->tl_scrollback_scrolled + 1; i > 0 && سطریں[i].continuation; --i)
	{
	    calc_pos += سطریں[i - 1].sb_بائٹیں;
	    calc_ستون += سطریں[i - 1].sb_ستون_تعداد;
	}
	i = رکن->tl_scrollback_scrolled + 1;
	calc_lnum = رکن->tl_بفر_scrolled + 1;
	// Do not count this line's bytes/cols twice
	اگر (سطریں[i].continuation)
	    ++i;
    }
    ورنہ
    {
	i = 1;
	calc_lnum = 1;
    }

    برائے (; i <= صف; ++i)
    {
	اگر (!سطریں[i].continuation)
	{
	    ++calc_lnum;
	    calc_pos = 0;
	    calc_ستون = 0;
	}
	ورنہ
	{
	    calc_pos += سطریں[i - 1].sb_بائٹیں;
	    calc_ستون += سطریں[i - 1].sb_ستون_تعداد;
	}
    }

    *lnum = calc_lnum;
    اگر (آغاز_ستون)
	*آغاز_ستون = calc_ستون;
    اگر (آغاز_pos)
	*آغاز_pos = calc_pos;
}

/*
 * Find the location of a buffer line in the scrollback
 */
    ساکن خالی
bufline_pos_اندر_scrollback(رکن_T *رکن, linenr_T lnum, صحیح ستون, صحیح *صف, صحیح *wrapped_ستون)
{
    buf_T	*buf = رکن->tl_بفر;
    sb_سطر_T	*سطریں = (sb_سطر_T *)رکن->tl_scrollback.ga_ڈیٹا;
    linenr_T    calc_صف = رکن->tl_scrollback_scrolled;
    صحیح         calc_ستون = ستون;
    linenr_T    l;

    اگر (lnum > buf->b_ml.ml_سطر_گنتی)
	واپس;

    اگر (lnum > رکن->tl_بفر_scrolled)
    {
	calc_صف = رکن->tl_scrollback_scrolled;
	l = رکن->tl_بفر_scrolled + 1;

	جب (calc_صف < رکن->tl_scrollback.ga_len && سطریں[calc_صف].continuation)
	    ++calc_صف;
    }
    ورنہ
    {
	calc_صف = 0;
	l = 1;
    }

    جب (calc_صف < رکن->tl_scrollback.ga_len && l < lnum)
    {
	++calc_صف;
	اگر (!سطریں[calc_صف].continuation)
	    ++l;
    }

    جب (calc_صف + 1 < رکن->tl_scrollback.ga_len && سطریں[calc_صف + 1].continuation
	    && calc_ستون >= سطریں[calc_صف].sb_ستون_تعداد)
    {
	calc_ستون -= سطریں[calc_صف].sb_ستون_تعداد;
	++calc_صف;
    }

    اگر (صف)
	*صف = calc_صف;
    اگر (wrapped_ستون)
	*wrapped_ستون = calc_ستون;
}

/*
 * Invoked when "msg" output from a job was received.  Write it to the terminal
 * of "buffer".
 */
    خالی
write_مقصد_رکن(buf_T *بفر, char_u *پیغام, channel_T *channel)
{
    size_t	len = STRLEN(پیغام);
    رکن_T	*رکن = بفر->b_رکن;

#اگر_تعریف_شدہ MSWIN
    // Win32: Cannot redirect output of the job, intercept it here and write to
    // the file.
    اگر (رکن->tl_برآمد_fd != تہی)
    {
	ch_log(channel, "Writing %d bytes to output file", (صحیح)len);
	fwrite(پیغام, len, 1, رکن->tl_برآمد_fd);
	واپس;
    }
#اگر_ختم

    اگر (رکن->tl_vterm == تہی)
    {
	ch_log(channel, "NOT writing %d bytes to terminal", (صحیح)len);
	واپس;
    }
    ch_log(channel, "writing %d bytes to terminal", (صحیح)len);
    cursor_off();
    رکن_write_کام_output(رکن, پیغام, len);

#اگر_تعریف_شدہ FEAT_GUI
    اگر (رکن->tl_system)
    {
	// show system output, scrolling up the screen as needed
	update_system_رکن(رکن);
	update_cursor(رکن, TRUE);
    }
    ورنہ
#اگر_ختم
    // In Terminal-Normal mode we are displaying the buffer, not the terminal
    // contents, thus no screen update is needed.
    اگر (!رکن->tl_معمول_منوال)
    {
	// Don't use update_screen() when editing the command line, it gets
	// cleared.
	// TODO: only update once in a while.
	ch_log(رکن->tl_کام->jv_channel, "updating screen");
	اگر (بفر == curbuf && (State & منوال_CMDLINE) == 0)
	{
	    update_screen(UPD_VALID_NO_UPDATE);
#اگر defined(FEAT_TABPANEL)
	    اگر (redraw_tabpanel)
		draw_tabpanel();
#اگر_ختم
	    // update_screen() can be slow, check the terminal wasn't closed
	    // already
	    اگر (بفر == curbuf && curbuf->b_رکن != تہی)
		update_cursor(curbuf->b_رکن, TRUE);
	}
	ورنہ
	    redraw_after_callback(TRUE, FALSE);
    }
}

/*
 * Send a mouse position and click to the vterm
 */
    ساکن صحیح
رکن_send_mouse(VTerm *vterm, صحیح button, صحیح pressed)
{
    VTermModifier   mod = VTERM_MOD_NONE;
    صحیح		    صف = mouse_صف - W_WINROW(curwin);
    صحیح		    ستون = mouse_ستون - curwin->w_wincol;

#اگر_تعریف_شدہ FEAT_PROP_POPUP
    اگر (popup_is_popup(curwin))
    {
	صف -= popup_چوٹی_extra(curwin);
	ستون -= popup_بایاں_extra(curwin);
    }
#اگر_ختم
    vterm_mouse_move(vterm, صف, ستون, mod);
    اگر (button != 0)
	vterm_mouse_button(vterm, button, pressed, mod);
    واپس TRUE;
}

ساکن صحیح enter_mouse_ستون = -1;
ساکن صحیح enter_mouse_صف = -1;

/*
 * Handle a mouse click, drag or release.
 * Return TRUE when a mouse event is sent to the terminal.
 */
    ساکن صحیح
رکن_mouse_click(VTerm *vterm, صحیح کلید)
{
#اگر defined(FEAT_CLIPBOARD)
    // For modeless selection mouse drag and release events are ignored, unless
    // they are preceded with a mouse down event
    ساکن صحیح	    ignore_drag_release = TRUE;
    VTermState	    *state;
    VTermMouseState mouse_state;


    state = vterm_obtain_state(vterm);
    اگر (state == تہی)
	واپس FALSE;

    vterm_state_get_mousestate(state, &mouse_state);
    اگر (mouse_state.flags == 0)
    {
	// Terminal is not using the mouse, use modeless selection.
	انتخاب (کلید)
	{
	حالت K_LEFTDRAG:
	حالت K_LEFTRELEASE:
	حالت K_RIGHTDRAG:
	حالت K_RIGHTRELEASE:
		// Ignore drag and release events when the button-down wasn't
		// seen before.
		اگر (ignore_drag_release)
		{
		    صحیح save_mouse_ستون, save_mouse_صف;

		    اگر (enter_mouse_ستون < 0)
			توڑو;

		    // mouse click in the window gave us focus, handle that
		    // click now
		    save_mouse_ستون = mouse_ستون;
		    save_mouse_صف = mouse_صف;
		    mouse_ستون = enter_mouse_ستون;
		    mouse_صف = enter_mouse_صف;
		    clip_modeless(MOUSE_بایاں, TRUE, FALSE);
		    mouse_ستون = save_mouse_ستون;
		    mouse_صف = save_mouse_صف;
		}
		// FALLTHROUGH
	حالت K_LEFTMOUSE:
	حالت K_RIGHTMOUSE:
		اگر (کلید == K_LEFTRELEASE || کلید == K_RIGHTRELEASE)
		    ignore_drag_release = TRUE;
		ورنہ
		    ignore_drag_release = FALSE;
		// Should we call mouse_has() here?
		اگر (clip_ستارہ.دستیاب)
		{
		    صحیح	    button, is_click, is_drag;

		    button = get_mouse_button(KEY2TERMCAP1(کلید),
							 &is_click, &is_drag);
		    اگر (mouse_model_popup() && button == MOUSE_بایاں
					       && (mod_نقاب & MOD_نقاب_سرکاؤ))
		    {
			// Translate shift-left to right button.
			button = MOUSE_دایاں;
			mod_نقاب &= ~MOD_نقاب_سرکاؤ;
		    }
		    clip_modeless(button, is_click, is_drag);
		}
		توڑو;

	حالت K_MIDDLEMOUSE:
		اگر (clip_ستارہ.دستیاب)
		    داخل_کرو_reg('*', TRUE);
		توڑو;
	}
	enter_mouse_ستون = -1;
	واپس FALSE;
    }
#اگر_ختم
    enter_mouse_ستون = -1;

    انتخاب (کلید)
    {
	حالت K_LEFTMOUSE:
	حالت K_LEFTMOUSE_NM:	رکن_send_mouse(vterm, 1, 1); توڑو;
	حالت K_LEFTDRAG:	رکن_send_mouse(vterm, 1, 1); توڑو;
	حالت K_LEFTRELEASE:
	حالت K_LEFTRELEASE_NM:	رکن_send_mouse(vterm, 1, 0); توڑو;
	حالت K_MOUSEMOVE:	رکن_send_mouse(vterm, 0, 0); توڑو;
	حالت K_MIDDLEMOUSE:	رکن_send_mouse(vterm, 2, 1); توڑو;
	حالت K_MIDDLEDRAG:	رکن_send_mouse(vterm, 2, 1); توڑو;
	حالت K_MIDDLERELEASE:	رکن_send_mouse(vterm, 2, 0); توڑو;
	حالت K_RIGHTMOUSE:	رکن_send_mouse(vterm, 3, 1); توڑو;
	حالت K_RIGHTDRAG:	رکن_send_mouse(vterm, 3, 1); توڑو;
	حالت K_RIGHTRELEASE:	رکن_send_mouse(vterm, 3, 0); توڑو;
    }
    واپس TRUE;
}

/*
 * Convert typed key "c" with modifiers "modmask" into bytes to send to the
 * job.
 * Return the number of bytes in "buf".
 */
    ساکن صحیح
رکن_convert_کلید(رکن_T *رکن, صحیح c, صحیح modmask, حرف *buf)
{
    VTerm	    *vterm = رکن->tl_vterm;
    VTerm_کلید	    کلید = VTERM_کلید_NONE;
    VTermModifier   mod = VTERM_MOD_NONE;
    صحیح		    other = FALSE;

    انتخاب (c)
    {
	// don't use VTERM_KEY_ENTER, it may do an unwanted conversion

	حالت K_BS:
#اگر_تعریف_شدہ MSWIN
	    // In ConPTY, we must use VTERM_KEY_BACKSPACE, otherwise it will
	    // delete one word, equivalent to Alt+Backspace.
	    اگر (رکن->tl_conpty)
		کلید = VTERM_کلید_BACKSPACE;
	    ورنہ
#اگر_ختم
		// don't use VTERM_KEY_BACKSPACE, it always becomes 0x7f DEL
		c = رکن_backspace_char;
	    توڑو;

#اگر_تعریف_شدہ MSWIN
	حالت BS:
	    // In ConPTY, we must use VTERM_KEY_BACKSPACE, otherwise it will
	    // delete one word, equivalent to Alt+Backspace.
	    اگر (رکن->tl_conpty)
		کلید = VTERM_کلید_BACKSPACE;
	    توڑو;
#اگر_ختم

	حالت ESC:		کلید = VTERM_کلید_ESCAPE; توڑو;
	حالت K_DEL:		کلید = VTERM_کلید_DEL; توڑو;
	حالت K_نیچے_کو:		کلید = VTERM_کلید_نیچے_کو; توڑو;
	حالت K_S_نیچے_کو:		mod = VTERM_MOD_سرکاؤ;
				کلید = VTERM_کلید_نیچے_کو; توڑو;
	حالت K_انتہا:		کلید = VTERM_کلید_انتہا; توڑو;
	حالت K_S_انتہا:		mod = VTERM_MOD_سرکاؤ;
				کلید = VTERM_کلید_انتہا; توڑو;
	حالت K_C_انتہا:		mod = VTERM_MOD_CTRL;
				کلید = VTERM_کلید_انتہا; توڑو;
	حالت K_F10:		کلید = VTERM_کلید_FUNCTION(10); توڑو;
	حالت K_F11:		کلید = VTERM_کلید_FUNCTION(11); توڑو;
	حالت K_F12:		کلید = VTERM_کلید_FUNCTION(12); توڑو;
	حالت K_F1:		کلید = VTERM_کلید_FUNCTION(1); توڑو;
	حالت K_F2:		کلید = VTERM_کلید_FUNCTION(2); توڑو;
	حالت K_F3:		کلید = VTERM_کلید_FUNCTION(3); توڑو;
	حالت K_F4:		کلید = VTERM_کلید_FUNCTION(4); توڑو;
	حالت K_F5:		کلید = VTERM_کلید_FUNCTION(5); توڑو;
	حالت K_F6:		کلید = VTERM_کلید_FUNCTION(6); توڑو;
	حالت K_F7:		کلید = VTERM_کلید_FUNCTION(7); توڑو;
	حالت K_F8:		کلید = VTERM_کلید_FUNCTION(8); توڑو;
	حالت K_F9:		کلید = VTERM_کلید_FUNCTION(9); توڑو;
	حالت K_HOME:		کلید = VTERM_کلید_HOME; توڑو;
	حالت K_S_HOME:		mod = VTERM_MOD_سرکاؤ;
				کلید = VTERM_کلید_HOME; توڑو;
	حالت K_C_HOME:		mod = VTERM_MOD_CTRL;
				کلید = VTERM_کلید_HOME; توڑو;
	حالت K_INS:		کلید = VTERM_کلید_INS; توڑو;
	حالت K_K0:		کلید = VTERM_کلید_KP_0; توڑو;
	حالت K_K1:		کلید = VTERM_کلید_KP_1; توڑو;
	حالت K_K2:		کلید = VTERM_کلید_KP_2; توڑو;
	حالت K_K3:		کلید = VTERM_کلید_KP_3; توڑو;
	حالت K_K4:		کلید = VTERM_کلید_KP_4; توڑو;
	حالت K_K5:		کلید = VTERM_کلید_KP_5; توڑو;
	حالت K_K6:		کلید = VTERM_کلید_KP_6; توڑو;
	حالت K_K7:		کلید = VTERM_کلید_KP_7; توڑو;
	حالت K_K8:		کلید = VTERM_کلید_KP_8; توڑو;
	حالت K_K9:		کلید = VTERM_کلید_KP_9; توڑو;
	حالت K_KDEL:		کلید = VTERM_کلید_DEL; توڑو; // TODO
	حالت K_KDIVIDE:		کلید = VTERM_کلید_KP_DIVIDE; توڑو;
	حالت K_KEND:		کلید = VTERM_کلید_KP_1; توڑو; // TODO
	حالت K_KENTER:		کلید = VTERM_کلید_KP_ENTER; توڑو;
	حالت K_KHOME:		کلید = VTERM_کلید_KP_7; توڑو; // TODO
	حالت K_KINS:		کلید = VTERM_کلید_KP_0; توڑو; // TODO
	حالت K_KMINUS:		کلید = VTERM_کلید_KP_MINUS; توڑو;
	حالت K_KMULTIPLY:	کلید = VTERM_کلید_KP_MULT; توڑو;
	حالت K_KPAGEDOWN:	کلید = VTERM_کلید_KP_3; توڑو; // TODO
	حالت K_KPAGEUP:		کلید = VTERM_کلید_KP_9; توڑو; // TODO
	حالت K_KPLUS:		کلید = VTERM_کلید_KP_PLUS; توڑو;
	حالت K_KPOINT:		کلید = VTERM_کلید_KP_PERIOD; توڑو;
	حالت K_بایاں:		کلید = VTERM_کلید_بایاں; توڑو;
	حالت K_S_بایاں:		mod = VTERM_MOD_سرکاؤ;
				کلید = VTERM_کلید_بایاں; توڑو;
	حالت K_C_بایاں:		mod = VTERM_MOD_CTRL;
				کلید = VTERM_کلید_بایاں; توڑو;
	حالت K_PAGEDOWN:	کلید = VTERM_کلید_PAGEDOWN; توڑو;
	حالت K_PAGEUP:		کلید = VTERM_کلید_PAGEUP; توڑو;
	حالت K_دایاں:		کلید = VTERM_کلید_دایاں; توڑو;
	حالت K_S_دایاں:		mod = VTERM_MOD_سرکاؤ;
				کلید = VTERM_کلید_دایاں; توڑو;
	حالت K_C_دایاں:		mod = VTERM_MOD_CTRL;
				کلید = VTERM_کلید_دایاں; توڑو;
	حالت K_اوپر:		کلید = VTERM_کلید_اوپر; توڑو;
	حالت K_S_اوپر:		mod = VTERM_MOD_سرکاؤ;
				کلید = VTERM_کلید_اوپر; توڑو;
	حالت TAB:		کلید = VTERM_کلید_TAB; توڑو;
	حالت K_S_TAB:		mod = VTERM_MOD_سرکاؤ;
				کلید = VTERM_کلید_TAB; توڑو;

	حالت K_MOUSEUP:		other = رکن_send_mouse(vterm, 5, 1); توڑو;
	حالت K_MOUSEDOWN:	other = رکن_send_mouse(vterm, 4, 1); توڑو;
	حالت K_MOUSELEFT:	other = رکن_send_mouse(vterm, 7, 1); توڑو;
	حالت K_MOUSERIGHT:	other = رکن_send_mouse(vterm, 6, 1); توڑو;

	حالت K_LEFTMOUSE:
	حالت K_LEFTMOUSE_NM:
	حالت K_LEFTDRAG:
	حالت K_LEFTRELEASE:
	حالت K_LEFTRELEASE_NM:
	حالت K_MOUSEMOVE:
	حالت K_MIDDLEMOUSE:
	حالت K_MIDDLEDRAG:
	حالت K_MIDDLERELEASE:
	حالت K_RIGHTMOUSE:
	حالت K_RIGHTDRAG:
	حالت K_RIGHTRELEASE:	اگر (!رکن_mouse_click(vterm, c))
				    واپس 0;
				other = TRUE;
				توڑو;

	حالت K_X1MOUSE:		/* TODO */ واپس 0;
	حالت K_X1DRAG:		/* TODO */ واپس 0;
	حالت K_X1RELEASE:	/* TODO */ واپس 0;
	حالت K_X2MOUSE:		/* TODO */ واپس 0;
	حالت K_X2DRAG:		/* TODO */ واپس 0;
	حالت K_X2RELEASE:	/* TODO */ واپس 0;

	حالت K_IGNORE:		واپس 0;
	حالت K_NOP:		واپس 0;
	حالت K_UNDO:		واپس 0;
	حالت K_HELP:		واپس 0;
	حالت K_XF1:		کلید = VTERM_کلید_FUNCTION(1); توڑو;
	حالت K_XF2:		کلید = VTERM_کلید_FUNCTION(2); توڑو;
	حالت K_XF3:		کلید = VTERM_کلید_FUNCTION(3); توڑو;
	حالت K_XF4:		کلید = VTERM_کلید_FUNCTION(4); توڑو;
	حالت K_SELECT:		واپس 0;
#اگر_تعریف_شدہ FEAT_GUI
	حالت K_VER_SCROLLBAR:	واپس 0;
	حالت K_HOR_SCROLLBAR:	واپس 0;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_GUI_TABLINE
	حالت K_TABLINE:		واپس 0;
	حالت K_TABMENU:		واپس 0;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_NETBEANS_INTG
	حالت K_F21:		کلید = VTERM_کلید_FUNCTION(21); توڑو;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_DND
	حالت K_DROP:		واپس 0;
#اگر_ختم
	حالت K_CURSORHOLD:	واپس 0;
	حالت K_PS:		vterm_keyboard_آغاز_paste(vterm);
				other = TRUE;
				توڑو;
	حالت K_PE:		vterm_keyboard_انتہا_paste(vterm);
				other = TRUE;
				توڑو;
    }

    // add modifiers for the typed key
    اگر (modmask & MOD_نقاب_سرکاؤ)
	mod |= VTERM_MOD_سرکاؤ;
    اگر (modmask & MOD_نقاب_CTRL)
	mod |= VTERM_MOD_CTRL;
    اگر (modmask & (MOD_نقاب_ALT | MOD_نقاب_META))
	mod |= VTERM_MOD_ALT;

    // Ctrl-Shift-i may have the key "I" instead of "i", but for the kitty
    // keyboard protocol should use "i".  Applies to all ascii letters.
    اگر (ASCII_ISUPPER(c)
	    && vterm_is_kitty_keyboard(vterm)
	    && mod == (VTERM_MOD_CTRL | VTERM_MOD_سرکاؤ))
	c = TOLOWER_ASC(c);

    /*
     * Convert special keys to vterm keys:
     * - Write keys to vterm: vterm_keyboard_key()
     * - Write output to channel.
     */
    اگر (کلید != VTERM_کلید_NONE)
	// Special key, let vterm convert it.
	vterm_keyboard_کلید(vterm, کلید, mod);
    ورنہ اگر (!other)
	// Normal character, let vterm convert it.
	vterm_keyboard_unichar(vterm, c, mod);

    // Read back the converted escape sequence.
    واپس (صحیح)vterm_output_read(vterm, buf, کلید_BUF_LEN);
}

/*
 * Return TRUE if the job for "term" is still running.
 * If "check_job_status" is TRUE update the job status.
 * NOTE: "term" may be freed by callbacks.
 */
    ساکن صحیح
رکن_کام_running_check(رکن_T *رکن, صحیح check_کام_حالت)
{
    // Also consider the job finished when the channel is closed, to avoid a
    // race condition when updating the title.
    اگر (رکن == تہی
	|| رکن->tl_کام == تہی
	|| !channel_is_open(رکن->tl_کام->jv_channel))
	واپس FALSE;

    کام_T *کام = رکن->tl_کام;

    // Careful: Checking the job status may invoke callbacks, which close
    // the buffer and terminate "term".  However, "job" will not be freed
    // yet.
    اگر (check_کام_حالت)
	کام_حالت(کام);
    واپس (کام->jv_حالت == کام_STARTED
	    || (کام->jv_channel != تہی && کام->jv_channel->ch_keep_open));
}

/*
 * Return TRUE if the job for "term" is still running.
 */
    صحیح
رکن_کام_running(رکن_T *رکن)
{
    واپس رکن_کام_running_check(رکن, FALSE);
}

/*
 * Return TRUE if the job for "term" is still running, ignoring the job was
 * "NONE".
 */
    صحیح
رکن_کام_running_not_none(رکن_T *رکن)
{
    واپس رکن_کام_running(رکن) && !رکن_none_open(رکن);
}

/*
 * Return TRUE if "term" has an active channel and used ":term NONE".
 */
    صحیح
رکن_none_open(رکن_T *رکن)
{
    // Also consider the job finished when the channel is closed, to avoid a
    // race condition when updating the title.
    واپس رکن != تہی
	&& رکن->tl_کام != تہی
	&& channel_is_open(رکن->tl_کام->jv_channel)
	&& رکن->tl_کام->jv_channel->ch_keep_open;
}

//
// Used to confirm whether we would like to kill a terminal.
// Return OK when the user confirms to kill it.
// Return FAIL if the user selects otherwise.
//
    صحیح
رکن_confirm_stop(buf_T *buf)
{
    char_u	buff[DIALOG_پیغام_سائز];
    صحیح	ret;

    dialog_پیغام(buff, _("Kill job in \"%s\"?"), buf_get_fname(buf));
    ret = vim_dialog_yesno(VIM_QUESTION, تہی, buff, 1);
    اگر (ret == VIM_YES)
	واپس OK;
    ورنہ
	واپس FAIL;
}

/*
 * Used when exiting: kill the job in "buf" if so desired.
 * Return OK when the job finished.
 * Return FAIL when the job is still running.
 */
    صحیح
رکن_try_stop_کام(buf_T *buf)
{
    صحیح	    گنتی;
    حرف    *how = (حرف *)buf->b_رکن->tl_kill;

#اگر defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
    اگر ((how == تہی || *how == NUL)
			  && (p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)))
    {
	اگر (رکن_confirm_stop(buf) == OK)
	    how = "kill";
	ورنہ
	    واپس FAIL;
    }
#اگر_ختم
    اگر (how == تہی || *how == NUL)
	واپس FAIL;

    کام_stop(buf->b_رکن->tl_کام, تہی, how);

    // wait for up to a second for the job to die
    برائے (گنتی = 0; گنتی < 100; ++گنتی)
    {
	کام_T *کام;

	// buffer, terminal and job may be cleaned up while waiting
	اگر (!buf_valid(buf)
		|| buf->b_رکن == تہی
		|| buf->b_رکن->tl_کام == تہی)
	    واپس OK;
	کام = buf->b_رکن->tl_کام;

	// Call job_status() to update jv_status. It may cause the job to be
	// cleaned up but it won't be freed.
	کام_حالت(کام);
	اگر (کام->jv_حالت >= کام_ENDED)
	    واپس OK;

	ui_delay(10L, TRUE);
	رکن_flush_messages();
    }
    واپس FAIL;
}

/*
 * Add the last line of the scrollback buffer to the buffer in the window.
 */
    ساکن خالی
جوڑو_scrollback_سطر_مقصد_بفر(رکن_T *رکن, char_u *متن, صحیح len, صحیح append)
{
    buf_T	*buf = رکن->tl_بفر;
    صحیح		خالی_خانے_2 = (buf->b_ml.ml_flags & ML_خالی_خانے);
    linenr_T	lnum = buf->b_ml.ml_سطر_گنتی;

#اگر_تعریف_شدہ MSWIN
    char_u *tmp = متن;
    اگر (!enc_utf8 && enc_codepage > 0)
    {
	WCHAR   *ret = تہی;
	صحیح	لمبائی = 0;

	Multi_Byte_مقصد_Wide_Char_alloc(CP_UTF8, 0, (حرف*)متن, len + 1,
							   &ret, &لمبائی);
	اگر (ret != تہی)
	{
	    Wide_Char_مقصد_Multi_Byte_alloc(enc_codepage, 0,
				      ret, لمبائی, (حرف **)&متن, &len, 0, 0);
	    vim_free(ret);
	}
    }
#اگر_ختم

    اگر (append)
    {
	char_u *پچھلا_متن = ml_get_buf(buf, lnum, FALSE);
	size_t پچھلا_len = STRLEN(پچھلا_متن);

	char_u *both = alloc(len + پچھلا_len + 2);
	اگر (both == تہی)
	    واپس;
	vim_strncpy(both, پچھلا_متن, پچھلا_len + 1);
	vim_strncpy(both + پچھلا_len, متن, len + 1);

	curbuf = buf;
	ml_replace(lnum, both, FALSE);
	curbuf = curwin->w_بفر;
    }
    ورنہ
	ml_append_buf(buf, lnum, متن, len + 1, FALSE);

#اگر_تعریف_شدہ MSWIN
    اگر (tmp != متن)
	vim_free(متن);
#اگر_ختم
    اگر (خالی_خانے_2)
    {
	// Delete the empty line that was in the empty buffer.
	curbuf = buf;
	ml_delete(1);
	curbuf = curwin->w_بفر;
    }
}

    ساکن خالی
cell2cellattr(ثابت VTermScreenCell *cell, cellattr_T *attr)
{
    attr->چوڑائی = cell->چوڑائی;
    attr->attrs = cell->attrs;
    attr->fg = cell->fg;
    attr->bg = cell->bg;
}

    ساکن صحیح
equal_celattr(cellattr_T *a, cellattr_T *b)
{
    // We only compare the RGB colors, ignoring the ANSI index and type.
    // Thus black set explicitly is equal the background black.
    واپس a->fg.red == b->fg.red
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
    ساکن صحیح
جوڑو_خالی_خانے_scrollback(رکن_T *رکن, cellattr_T *fill_attr, صحیح lnum)
{
    اگر (ga_grow(&رکن->tl_scrollback, 1) == FAIL)
	واپس FALSE;

    sb_سطر_T *سطر = (sb_سطر_T *)رکن->tl_scrollback.ga_ڈیٹا
	+ رکن->tl_scrollback.ga_len;

    اگر (lnum > 0)
    {
	صحیح i;

	برائے (i = 0; i < رکن->tl_scrollback.ga_len - lnum; ++i)
	{
	    *سطر = *(سطر - 1);
	    --سطر;
	}
    }
    سطر->sb_ستون_تعداد = 0;
    سطر->sb_بائٹیں = 0;
    سطر->sb_cells = تہی;
    سطر->sb_fill_attr = *fill_attr;
    ++رکن->tl_scrollback.ga_len;
    واپس OK;
}

/*
 * Remove the terminal contents from the scrollback and the buffer.
 * Used before adding a new scrollback line or updating the buffer for lines
 * displayed in the terminal.
 */
    ساکن خالی
cleanup_scrollback(رکن_T *رکن)
{
    sb_سطر_T	*سطر;
    garray_T	*gap;
    char_u	*bufline;
    size_t      bufline_لمبائی;

    curbuf = رکن->tl_بفر;
    gap = &رکن->tl_scrollback;
    bufline = ml_get_buf(curbuf, curbuf->b_ml.ml_سطر_گنتی, FALSE);
    bufline_لمبائی = STRLEN(bufline);
    جب (رکن->tl_scrollback_snapshot && gap->ga_len > 0)
    {
	سطر = (sb_سطر_T *)gap->ga_ڈیٹا + gap->ga_len - 1;
	اگر (سطر->sb_بائٹیں < 0 || (size_t)سطر->sb_بائٹیں > bufline_لمبائی)
	    توڑو;
	bufline_لمبائی -= سطر->sb_بائٹیں;
	اگر (!bufline_لمبائی)
	{
	    ml_delete(curbuf->b_ml.ml_سطر_گنتی);
	    bufline = ml_get_buf(curbuf, curbuf->b_ml.ml_سطر_گنتی, FALSE);
	    bufline_لمبائی = STRLEN(bufline);
	}
	vim_free(سطر->sb_cells);
	--gap->ga_len;
	--رکن->tl_scrollback_snapshot;
    }
    اگر (bufline_لمبائی < STRLEN(bufline))
    {
	char_u *shortened = vim_strnsave(bufline, bufline_لمبائی);
	ml_replace(curbuf->b_ml.ml_سطر_گنتی, shortened, FALSE);
    }
    curbuf = curwin->w_بفر;
    اگر (curbuf == رکن->tl_بفر)
	check_cursor();
}

/*
 * Add the current lines of the terminal to scrollback and to the buffer.
 */
    ساکن خالی
update_snapshot(رکن_T *رکن)
{
    VTermScreen	    *screen;
    VTermState	    *state;
    صحیح		    len;
    صحیح		    سطریں_skipped = 0;
    VTermPos	    pos;
    VTermScreenCell cell;
    cellattr_T	    fill_attr, new_fill_attr;
    cellattr_T	    *p;

    ch_log(رکن->tl_کام == تہی ? تہی : رکن->tl_کام->jv_channel,
				  "Adding terminal window snapshot to buffer");

    // First remove the lines that were appended before, they might be
    // outdated.
    cleanup_scrollback(رکن);

    screen = vterm_obtain_screen(رکن->tl_vterm);
    state = vterm_obtain_state(رکن->tl_vterm);
    fill_attr = new_fill_attr = رکن->tl_default_color;
    برائے (pos.صف = 0; pos.صف < رکن->tl_صفیں; ++pos.صف)
    {
	len = 0;
	برائے (pos.ستون = 0; pos.ستون < رکن->tl_ستون_تعداد; ++pos.ستون)
	    اگر (vterm_screen_get_cell(screen, pos, &cell) != 0
						       && cell.chars[0] != NUL)
	    {
		len = pos.ستون + 1;
		new_fill_attr = رکن->tl_default_color;
	    }
	    ورنہ
		// Assume the last attr is the filler attr.
		cell2cellattr(&cell, &new_fill_attr);

	اگر (len == 0 && equal_celattr(&new_fill_attr, &fill_attr))
	    ++سطریں_skipped;
	ورنہ
	{
	    جب (سطریں_skipped > 0)
	    {
		// Line was skipped, add an empty line.
		--سطریں_skipped;
		اگر (جوڑو_خالی_خانے_scrollback(رکن, &fill_attr, 0) == OK)
		{
		    جوڑو_scrollback_سطر_مقصد_بفر(رکن, (char_u *)"", 0, 0);
		    ++رکن->tl_scrollback_snapshot;
		}
	    }

	    اگر (len == 0)
		p = تہی;
	    ورنہ
		p = ALLOC_MULT(cellattr_T, len);
	    اگر ((p != تہی || len == 0)
				     && ga_grow(&رکن->tl_scrollback, 1) == OK)
	    {
		garray_T    ga;
		صحیح	    چوڑائی;
		ثابت VTerm_سطر_Info *lineinfo;
		sb_سطر_T   *سطر = (sb_سطر_T *)رکن->tl_scrollback.ga_ڈیٹا
						  + رکن->tl_scrollback.ga_len;

		lineinfo = vterm_state_get_lineinfo(state, pos.صف);

		ga_init2(&ga, 1, 100);
		برائے (pos.ستون = 0; pos.ستون < len; pos.ستون += چوڑائی)
		{
		    اگر (vterm_screen_get_cell(screen, pos, &cell) == 0)
		    {
			چوڑائی = 1;
			CLEAR_اشارہ_گر(p + pos.ستون);
			اگر (ga_grow(&ga, 1) == OK)
			    ga.ga_len += utf_char2bytes(' ',
					     (char_u *)ga.ga_ڈیٹا + ga.ga_len);
		    }
		    ورنہ
		    {
			چوڑائی = cell.چوڑائی;

			cell2cellattr(&cell, &p[pos.ستون]);
			اگر (چوڑائی == 2)
			    // second cell of double-width character has the
			    // same attributes.
			    p[pos.ستون + 1] = p[pos.ستون];

			// Each character can be up to 6 bytes.
			اگر (ga_grow(&ga, VTERM_MAX_CHARS_فی_CELL * 6) == OK)
			{
			    صحیح	    i;
			    صحیح	    c;

			    برائے (i = 0; i < VTERM_MAX_CHARS_فی_CELL &&
				    ((c = cell.chars[i]) > 0 || i == 0); ++i)
				ga.ga_len += utf_char2bytes(c == NUL ? ' ' : c,
					     (char_u *)ga.ga_ڈیٹا + ga.ga_len);
			}
		    }
		}
		سطر->sb_ستون_تعداد = len;
		سطر->sb_بائٹیں = ga.ga_len;
		سطر->sb_cells = p;
		سطر->sb_fill_attr = new_fill_attr;
		سطر->continuation = (char_u)lineinfo->continuation;
		fill_attr = new_fill_attr;
		++رکن->tl_scrollback.ga_len;
		++رکن->tl_scrollback_snapshot;

		اگر (ga_grow(&ga, 1) == FAIL)
		    جوڑو_scrollback_سطر_مقصد_بفر(رکن, (char_u *)"", 0, 0);
		ورنہ
		{
		    *((char_u *)ga.ga_ڈیٹا + ga.ga_len) = NUL;
		    جوڑو_scrollback_سطر_مقصد_بفر(رکن, ga.ga_ڈیٹا, ga.ga_len,
						  lineinfo->continuation);
		}
		ga_clear(&ga);
	    }
	    ورنہ
		vim_free(p);
	}
    }

    // Add trailing empty lines.
    برائے (pos.صف = رکن->tl_scrollback.ga_len;
	    pos.صف < رکن->tl_scrollback_scrolled + رکن->tl_cursor_pos.صف;
	    ++pos.صف)
    {
	اگر (جوڑو_خالی_خانے_scrollback(رکن, &fill_attr, 0) == OK)
	{
	    جوڑو_scrollback_سطر_مقصد_بفر(رکن, (char_u *)"", 0, 0);
	    ++رکن->tl_scrollback_snapshot;
	}
    }

    رکن->tl_dirty_snapshot = FALSE;
#اگر_تعریف_شدہ FEAT_TIMERS
    رکن->tl_timer_سیٹ = FALSE;
#اگر_ختم
}

/*
 * Loop over all windows in the current tab, and also curwin, which is not
 * encountered when using a terminal in a popup window.
 * Return TRUE if "*wp" was set to the next window.
 */
    ساکن صحیح
for_all_windows_and_curwin(win_T **wp, صحیح *did_curwin)
{
    اگر (*wp == تہی)
	*wp = firstwin;
    ورنہ اگر ((*wp)->w_اگلا != تہی)
	*wp = (*wp)->w_اگلا;
    ورنہ اگر (!*did_curwin)
	*wp = curwin;
    ورنہ
	واپس FALSE;
    اگر (*wp == curwin)
	*did_curwin = TRUE;
    واپس TRUE;
}

/*
 * If needed, add the current lines of the terminal to scrollback and to the
 * buffer.  Called after the job has ended and when switching to
 * Terminal-Normal mode.
 * When "redraw" is TRUE redraw the windows that show the terminal.
 */
    خالی
may_move_terminal_مقصد_بفر(رکن_T *رکن, صحیح redraw)
{
    اگر (رکن->tl_vterm == تہی)
	واپس;

    // Update the snapshot only if something changes or the buffer does not
    // have all the lines.
    اگر (رکن->tl_dirty_snapshot || رکن->tl_بفر->b_ml.ml_سطر_گنتی
					       <= رکن->tl_بفر_scrolled)
	update_snapshot(رکن);

    اگر (redraw)
    {
	win_T	    *wp = تہی;
	صحیح	    did_curwin = FALSE;

	جب (for_all_windows_and_curwin(&wp, &did_curwin))
	{
	    اگر (wp->w_بفر == رکن->tl_بفر)
	    {
		wp->w_cursor.lnum = رکن->tl_بفر->b_ml.ml_سطر_گنتی;
		wp->w_cursor.ستون = 0;
		wp->w_valid = 0;
		اگر (wp->w_cursor.lnum >= wp->w_اونچائی)
		{
		    linenr_T min_topline = wp->w_cursor.lnum - wp->w_اونچائی + 1;

		    اگر (wp->w_topline < min_topline)
			wp->w_topline = min_topline;
		}
		redraw_win_later(wp, UPD_NOT_VALID);
	    }
	}
    }
}

#اگر defined(FEAT_TIMERS)
/*
 * Check if any terminal timer expired.  If so, copy text from the terminal to
 * the buffer.
 * Return the time until the next timer will expire.
 */
    صحیح
رکن_check_timers(صحیح اگلا_due_arg, proftime_T *now)
{
    رکن_T  *رکن;
    صحیح	    اگلا_due = اگلا_due_arg;

    FOR_ALL_TERMS(رکن)
    {
	اگر (رکن->tl_timer_سیٹ && !رکن->tl_معمول_منوال)
	{
	    طویل    this_due = proftime_وقت_بایاں(&رکن->tl_timer_due, now);

	    اگر (this_due <= 1)
	    {
		رکن->tl_timer_سیٹ = FALSE;
		may_move_terminal_مقصد_بفر(رکن, FALSE);
	    }
	    ورنہ اگر (اگلا_due == -1 || اگلا_due > this_due)
		اگلا_due = this_due;
	}
    }

    واپس اگلا_due;
}
#اگر_ختم

/*
 * When "normal_mode" is TRUE set the terminal to Terminal-Normal mode,
 * otherwise end it.
 */
    ساکن خالی
سیٹ_terminal_منوال(رکن_T *رکن, صحیح معمول_منوال)
{
    رکن->tl_معمول_منوال = معمول_منوال;
    may_trigger_modechanged();
    اگر (!معمول_منوال)
	handle_postponed_scrollback(رکن);
    VIM_CLEAR(رکن->tl_حالت_متن);
    اگر (رکن->tl_بفر == curbuf)
	maketitle();
}

/*
 * Called after the job is finished and Terminal mode is not active:
 * Move the vterm contents into the scrollback buffer and free the vterm.
 */
    ساکن خالی
cleanup_vterm(رکن_T *رکن)
{
    سیٹ_terminal_منوال(رکن, FALSE);
    اگر (رکن->tl_اختتام != TL_اختتام_CLOSE)
	may_move_terminal_مقصد_بفر(رکن, TRUE);
    رکن_free_vterm(رکن);
}

/*
 * Switch from Terminal-Job mode to Terminal-Normal mode.
 * Suspends updating the terminal window.
 */
    ساکن خالی
رکن_enter_معمول_منوال(خالی)
{
    رکن_T     *رکن = curbuf->b_رکن;
    linenr_T   lnum;
    صحیح        ستون;

    سیٹ_terminal_منوال(رکن, TRUE);

    // Append the current terminal contents to the buffer.
    may_move_terminal_مقصد_بفر(رکن, TRUE);

    // Move the window cursor to the position of the cursor in the
    // terminal.
    lnum = رکن->tl_بفر_scrolled + 1 + رکن->tl_cursor_pos.صف;
    ستون = رکن->tl_cursor_pos.ستون;
    scrollbackline_pos_اندر_buf(رکن, رکن->tl_cursor_pos.صف + رکن->tl_scrollback_scrolled, &lnum, &ستون, تہی);

    curwin->w_cursor.lnum = lnum;
    check_cursor();
    اگر (coladvance(ستون) == FAIL)
	coladvance(MAXCOL);
    curwin->w_سیٹ_curswant = true;
    curwin->w_topline = رکن->tl_بفر_scrolled + 1;
}

/*
 * Returns TRUE if the current window contains a terminal and we are in
 * Terminal-Normal mode.
 */
    صحیح
رکن_اندر_معمول_منوال(buf_T *buf)
{
    رکن_T *رکن = buf->b_رکن;

    واپس رکن != تہی && رکن->tl_معمول_منوال;
}

/*
 * Switch from Terminal-Normal mode to Terminal-Job mode.
 * Restores updating the terminal window.
 */
    خالی
رکن_enter_کام_منوال(خالی)
{
    رکن_T	*رکن = curbuf->b_رکن;

    سیٹ_terminal_منوال(رکن, FALSE);

    اگر (رکن->tl_channel_closed)
	cleanup_vterm(رکن);
    redraw_buf_and_حالت_later(curbuf, UPD_NOT_VALID);
#اگر_تعریف_شدہ FEAT_PROP_POPUP
    اگر (WIN_IS_POPUP(curwin))
	redraw_later(UPD_NOT_VALID);
#اگر_ختم
}

/*
 * When "modify_other_keys" is set then vgetc() should not reduce a key with
 * modifiers into a basic key.  However, we may only find out after calling
 * vgetc().  Therefore vgetorpeek() will call check_no_reduce_keys() to update
 * "no_reduce_keys" before using it.
 */
قسمتعریف شمار {
    NRKS_NONE,	    // initial value
    NRKS_CHECK,	    // modify_other_keys was off before calling vgetc()
    NRKS_سیٹ,	    // no_reduce_keys was incremented in term_vgetc() or
		    // check_no_reduce_keys(), must be decremented.
} reduce_کلید_state_T;

ساکن reduce_کلید_state_T  no_reduce_کلید_state = NRKS_NONE;

/*
 * Return TRUE if the term is using modifyOtherKeys level 2 or the kitty
 * keyboard protocol.
 */
    ساکن صحیح
vterm_using_کلید_protocol(خالی)
{
    واپس curbuf->b_رکن != تہی
	&& curbuf->b_رکن->tl_vterm != تہی
	&& (vterm_is_modify_other_کلیدیں(curbuf->b_رکن->tl_vterm)
		|| vterm_is_kitty_keyboard(curbuf->b_رکن->tl_vterm));
}

    خالی
check_no_reduce_کلیدیں(خالی)
{
    اگر (no_reduce_کلید_state != NRKS_CHECK
	    || no_reduce_کلیدیں >= 1
	    || curbuf->b_رکن == تہی
	    || curbuf->b_رکن->tl_vterm == تہی)
	واپس;

    اگر (vterm_using_کلید_protocol())
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
    ساکن صحیح
رکن_vgetc(خالی)
{
    صحیح c;
    صحیح save_State = State;

    State = منوال_TERMINAL;
    got_int = FALSE;
#اگر_تعریف_شدہ MSWIN
    ctrl_break_was_pressed = FALSE;
#اگر_ختم

    اگر (vterm_using_کلید_protocol())
    {
	++no_reduce_کلیدیں;
	no_reduce_کلید_state = NRKS_سیٹ;
    }
    ورنہ
    {
	no_reduce_کلید_state = NRKS_CHECK;
    }

    c = vgetc();
    got_int = FALSE;
    State = save_State;

    اگر (no_reduce_کلید_state == NRKS_سیٹ)
	--no_reduce_کلیدیں;
    no_reduce_کلید_state = NRKS_NONE;

    واپس c;
}

ساکن صحیح	mouse_was_outside = FALSE;

/*
 * Send key "c" with modifiers "modmask" to terminal.
 * Return FAIL when the key needs to be handled in Normal mode.
 * Return OK when the key was dropped or sent to the terminal.
 */
    صحیح
send_کلیدیں_مقصد_رکن(رکن_T *رکن, صحیح c, صحیح modmask, صحیح typed)
{
    حرف	پیغام[کلید_BUF_LEN];
    size_t	len;
    صحیح		dragging_outside = FALSE;

    // Catch keys that need to be handled as in Normal mode.
    انتخاب (c)
    {
	حالت NUL:
	حالت K_ZERO:
	    اگر (typed)
		stuffcharReadbuff(c);
	    واپس FAIL;

	حالت K_TABLINE:
	    stuffcharReadbuff(c);
	    واپس FAIL;

	حالت K_IGNORE:
	حالت K_CANCEL:  // used for :normal when running out of chars
	    واپس FAIL;

	حالت K_LEFTDRAG:
	حالت K_MIDDLEDRAG:
	حالت K_RIGHTDRAG:
	حالت K_X1DRAG:
	حالت K_X2DRAG:
	    dragging_outside = mouse_was_outside;
	    // FALLTHROUGH
	حالت K_LEFTMOUSE:
	حالت K_LEFTMOUSE_NM:
	حالت K_LEFTRELEASE:
	حالت K_LEFTRELEASE_NM:
	حالت K_MOUSEMOVE:
	حالت K_MIDDLEMOUSE:
	حالت K_MIDDLERELEASE:
	حالت K_RIGHTMOUSE:
	حالت K_RIGHTRELEASE:
	حالت K_X1MOUSE:
	حالت K_X1RELEASE:
	حالت K_X2MOUSE:
	حالت K_X2RELEASE:

	حالت K_MOUSEUP:
	حالت K_MOUSEDOWN:
	حالت K_MOUSELEFT:
	حالت K_MOUSERIGHT:
	    {
		صحیح	صف = mouse_صف;
		صحیح	ستون = mouse_ستون;

#اگر_تعریف_شدہ FEAT_PROP_POPUP
		اگر (popup_is_popup(curwin))
		{
		    صف -= popup_چوٹی_extra(curwin);
		    ستون -= popup_بایاں_extra(curwin);
		}
#اگر_ختم
		اگر (صف < W_WINROW(curwin)
			|| صف >= (W_WINROW(curwin) + curwin->w_اونچائی)
			|| ستون < curwin->w_wincol
			|| ستون >= W_ENDCOL(curwin)
			|| dragging_outside)
		{
		    // click or scroll outside the current window or on status
		    // line or vertical separator
		    اگر (typed)
		    {
			stuffcharReadbuff(c);
			mouse_was_outside = TRUE;
		    }
		    واپس FAIL;
		}
	    }
	    توڑو;

	حالت K_COMMAND:
	حالت K_SCRIPT_COMMAND:
	    واپس do_cmdkey_command(c, 0);
    }
    اگر (typed)
	mouse_was_outside = FALSE;

    // Convert the typed key to a sequence of bytes for the job.
    len = رکن_convert_کلید(رکن, c, modmask, پیغام);
    اگر (len > 0)
	// TODO: if FAIL is returned, stop?
	channel_send(رکن->tl_کام->jv_channel, get_tty_part(رکن),
						(char_u *)پیغام, (صحیح)len, تہی);

    واپس OK;
}

/*
 * Handle CTRL-W "": send register contents to the job.
 */
    ساکن خالی
رکن_paste_رجسٹر(صحیح پچھلا_c غیر_مستعمل)
{
    صحیح		c;
    list_T	*l;
    listitem_T	*شے;
    طویل	reglen = 0;
    صحیح		type;

    اگر (جوڑو_مقصد_showcmd(پچھلا_c))
    اگر (جوڑو_مقصد_showcmd('"'))
	برآمد_flush();

    c = رکن_vgetc();
    clear_showcmd();

    اگر (!رکن_use_loop())
	// job finished while waiting for a character
	واپس;

    // CTRL-W "= prompt for expression to evaluate.
    اگر (c == '=' && get_اظہاریہ_رجسٹر() != '=')
	واپس;
    اگر (!رکن_use_loop())
	// job finished while waiting for a character
	واپس;

    l = (list_T *)get_reg_contents(c, GREG_LIST);
    اگر (l == تہی)
	واپس;

    type = get_reg_type(c, &reglen);
    FOR_ALL_LIST_اشیاء(l, شے)
    {
	char_u *s = tv_get_string(&شے->li_tv);
#اگر_تعریف_شدہ MSWIN
	char_u *tmp = s;

	اگر (!enc_utf8 && enc_codepage > 0)
	{
	    WCHAR   *ret = تہی;
	    صحیح	لمبائی = 0;

	    Multi_Byte_مقصد_Wide_Char_alloc(enc_codepage, 0, (حرف *)s,
		    (صحیح)STRLEN(s), &ret, &لمبائی);
	    اگر (ret != تہی)
	    {
		Wide_Char_مقصد_Multi_Byte_alloc(CP_UTF8, 0,
			ret, لمبائی, (حرف **)&s, &لمبائی, 0, 0);
		vim_free(ret);
	    }
	}
#اگر_ختم
	channel_send(curbuf->b_رکن->tl_کام->jv_channel, PART_اندر,
		s, (صحیح)STRLEN(s), تہی);
#اگر_تعریف_شدہ MSWIN
	اگر (tmp != s)
	    vim_free(s);
#اگر_ختم

	اگر (شے->li_اگلا != تہی || type == MLINE)
	    channel_send(curbuf->b_رکن->tl_کام->jv_channel, PART_اندر,
		    (char_u *)"\r", 1, تہی);
    }
    list_free(l);
}

/*
 * Return TRUE when waiting for a character in the terminal, the cursor of the
 * terminal should be displayed.
 */
    صحیح
terminal_is_فعال(خالی)
{
    واپس اندر_terminal_loop != تہی;
}

/*
 * Return the highlight group ID for the terminal and the window.
 */
    ساکن صحیح
رکن_get_highlight_id(رکن_T *رکن, win_T *wp)
{
    char_u *name;

    اگر (wp != تہی && wp->w_hlfwin_id != 0)
	name = syn_id2name(wp->w_hlfwin_id);
    ورنہ اگر (رکن->tl_highlight_name != تہی)
	name = رکن->tl_highlight_name;
    ورنہ
	name = (char_u*)"Terminal";

    واپس syn_name2id(name);
}

#اگر defined(FEAT_GUI)
    cursorentry_T *
رکن_get_cursor_shape(guicolor_T *fg, guicolor_T *bg)
{
    رکن_T		 *رکن = اندر_terminal_loop;
    ساکن cursorentry_T اندراج_2;
    صحیح			 id;
    guicolor_T		 رکن_fg = INVALCOLOR;
    guicolor_T		 رکن_bg = INVALCOLOR;

    CLEAR_میدان(اندراج_2);
    اندراج_2.shape = اندراج_2.mshape =
	رکن->tl_cursor_shape == VTERM_PROP_CURSORSHAPE_UNDERLINE ? SHAPE_HOR :
	رکن->tl_cursor_shape == VTERM_PROP_CURSORSHAPE_BAR_بایاں ? SHAPE_VER :
	SHAPE_بلاک;
    اندراج_2.percentage = 20;
    اگر (رکن->tl_cursor_blink)
    {
	اندراج_2.blinkwait = 700;
	اندراج_2.blinkon = 400;
	اندراج_2.blinkoff = 250;
    }

    // The highlight group overrules the defaults.
    id = رکن_get_highlight_id(رکن, curwin);
    اگر (id != 0)
	syn_id2colors(id, &رکن_fg, &رکن_bg);
    اگر (رکن_bg != INVALCOLOR)
	*fg = رکن_bg;
    ورنہ
	*fg = gui.عقب_pixel;

    اگر (رکن->tl_cursor_color == تہی)
    {
	اگر (رکن_fg != INVALCOLOR)
	    *bg = رکن_fg;
	ورنہ
	    *bg = gui.norm_pixel;
    }
    ورنہ
	*bg = color_name2handle(رکن->tl_cursor_color);
    اندراج_2.name = "n";
    اندراج_2.مستعمل_for = SHAPE_CURSOR;

    واپس &اندراج_2;
}
#اگر_ختم

    ساکن خالی
may_output_cursor_props(خالی)
{
    اگر (!cursor_color_equal(آخری_سیٹ_cursor_color, desired_cursor_color)
	    || آخری_سیٹ_cursor_shape != desired_cursor_shape
	    || آخری_سیٹ_cursor_blink != desired_cursor_blink)
    {
	cursor_color_copy(&آخری_سیٹ_cursor_color, desired_cursor_color);
	آخری_سیٹ_cursor_shape = desired_cursor_shape;
	آخری_سیٹ_cursor_blink = desired_cursor_blink;
	رکن_cursor_color(cursor_color_get(desired_cursor_color));
	اگر (desired_cursor_shape == -1 || desired_cursor_blink == -1)
	    // this will restore the initial cursor style, if possible
	    ui_cursor_shape_forced(TRUE);
	ورنہ
	    رکن_cursor_shape(desired_cursor_shape, desired_cursor_blink);
    }
}

/*
 * Set the cursor color and shape, if not last set to these.
 */
    ساکن خالی
may_سیٹ_cursor_props(رکن_T *رکن)
{
#اگر_تعریف_شدہ FEAT_GUI
    // For the GUI the cursor properties are obtained with
    // term_get_cursor_shape().
    اگر (gui.اندر_use)
	واپس;
#اگر_ختم
    اگر (اندر_terminal_loop == رکن)
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
    ساکن خالی
prepare_restore_cursor_props(خالی)
{
#اگر_تعریف_شدہ FEAT_GUI
    اگر (gui.اندر_use)
	واپس;
#اگر_ختم
    cursor_color_copy(&desired_cursor_color, تہی);
    desired_cursor_shape = -1;
    desired_cursor_blink = -1;
    may_output_cursor_props();
}

/*
 * Returns TRUE if the current window contains a terminal and we are sending
 * keys to the job.
 * If "check_job_status" is TRUE update the job status.
 */
    ساکن صحیح
رکن_use_loop_check(صحیح check_کام_حالت)
{
    رکن_T *رکن = curbuf->b_رکن;

    واپس رکن != تہی
	&& !رکن->tl_معمول_منوال
	&& رکن->tl_vterm != تہی
	&& رکن_کام_running_check(رکن, check_کام_حالت);
}

/*
 * Returns TRUE if the current window contains a terminal and we are sending
 * keys to the job.
 */
    صحیح
رکن_use_loop(خالی)
{
    واپس رکن_use_loop_check(FALSE);
}

/*
 * Called when entering a window with the mouse.  If this is a terminal window
 * we may want to change state.
 */
    خالی
رکن_win_entered(خالی)
{
    رکن_T *رکن = curbuf->b_رکن;

    اگر (رکن == تہی)
	واپس;

    اگر (رکن_use_loop_check(TRUE))
    {
	reset_VIsual_and_resel();
	اگر (State & منوال_داخل_کرو)
	    stop_داخل_کرو_منوال = TRUE;
    }
    mouse_was_outside = FALSE;
    enter_mouse_ستون = mouse_ستون;
    enter_mouse_صف = mouse_صف;
}

    خالی
رکن_focus_change(صحیح اندر_focus)
{
    رکن_T *رکن = curbuf->b_رکن;

    اگر (رکن == تہی || رکن->tl_vterm == تہی)
	واپس;

    VTermState	*state = vterm_obtain_state(رکن->tl_vterm);

    اگر (اندر_focus)
	vterm_state_focus_اندر(state);
    ورنہ
	vterm_state_focus_برآمد(state);
    رکن_forward_output(رکن);
}

/*
 * vgetc() may not include CTRL in the key when modify_other_keys is set.
 * Return the Ctrl-key value in that case.
 */
    ساکن صحیح
raw_c_مقصد_ctrl(صحیح c)
{
    اگر ((mod_نقاب & MOD_نقاب_CTRL)
	    && ((c >= '`' && c <= 0x7f) || (c >= '@' && c <= '_')))
	واپس c & 0x1f;
    واپس c;
}

/*
 * When modify_other_keys is set then do the reverse of raw_c_to_ctrl().
 * Also when the Kitty keyboard protocol is used.
 * May set "mod_mask".
 */
    ساکن صحیح
ctrl_مقصد_raw_c(صحیح c)
{
    اگر (c < 0x20 && vterm_using_کلید_protocol())
    {
	mod_نقاب |= MOD_نقاب_CTRL;
	واپس c + '@';
    }
    واپس c;
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
    صحیح
terminal_loop(صحیح blocking)
{
    صحیح		c;
    صحیح		raw_c;
    صحیح		termwinkey = 0;
    صحیح		ret;
#اگر_تعریف_شدہ UNIX
    صحیح		tty_fd = curbuf->b_رکن->tl_کام->jv_channel
				 ->ch_part[get_tty_part(curbuf->b_رکن)].ch_fd;
#اگر_ختم
    صحیح		restore_cursor = FALSE;

    // Remember the terminal we are sending keys to.  However, the terminal
    // might be closed while waiting for a character, e.g. typing "exit" in a
    // shell and ++close was used.  Therefore use curbuf->b_term instead of a
    // stored reference.
    اندر_terminal_loop = curbuf->b_رکن;

    اگر (*curwin->w_p_twk != NUL)
    {
	termwinkey = string_مقصد_کلید(curwin->w_p_twk, TRUE);
	اگر (termwinkey == Ctrl_W)
	    termwinkey = 0;
    }
    position_cursor(curwin, &curbuf->b_رکن->tl_cursor_pos);
    may_سیٹ_cursor_props(curbuf->b_رکن);

    جب (blocking || vpeekc_nomap() != NUL)
    {
#اگر_تعریف_شدہ FEAT_GUI
	اگر (curbuf->b_رکن != تہی && !curbuf->b_رکن->tl_system)
#اگر_ختم
	    // TODO: skip screen update when handling a sequence of keys.
	    // Repeat redrawing in case a message is received while redrawing.
	    جب (must_redraw != 0)
		اگر (update_screen(0) == FAIL)
		    توڑو;
	اگر (!رکن_use_loop_check(TRUE) || اندر_terminal_loop != curbuf->b_رکن)
	    // job finished while redrawing
	    توڑو;

	update_cursor(curbuf->b_رکن, FALSE);
	restore_cursor = TRUE;

	raw_c = رکن_vgetc();
	اگر (!رکن_use_loop_check(TRUE) || اندر_terminal_loop != curbuf->b_رکن)
	{
	    // Job finished while waiting for a character.  Push back the
	    // received character.
	    اگر (raw_c != K_IGNORE)
		vungetc(raw_c);
	    توڑو;
	}
	اگر (raw_c == K_IGNORE)
	    جاری;
	c = raw_c_مقصد_ctrl(raw_c);

#اگر_تعریف_شدہ UNIX
	/*
	 * The shell or another program may change the tty settings.  Getting
	 * them for every typed character is a bit of overhead, but it's needed
	 * for the first character typed, e.g. when Vim starts in a shell.
	 */
	اگر (mch_isatty(tty_fd))
	{
	    ttyinfo_T info;

	    // Get the current backspace character of the pty.
	    اگر (get_tty_info(tty_fd, &info) == OK)
		رکن_backspace_char = info.backspace;
	}
#اگر_ختم

#اگر_تعریف_شدہ MSWIN
	// On Windows winpty handles CTRL-C, don't send a CTRL_C_EVENT.
	// Use CTRL-BREAK to kill the job.
	اگر (ctrl_break_was_pressed)
	    mch_signal_کام(curbuf->b_رکن->tl_کام, (char_u *)"kill");
#اگر_ختم
	// Was either CTRL-W (termwinkey) or CTRL-\ pressed?
	// Not in a system terminal.
	اگر ((c == (termwinkey == 0 ? Ctrl_W : termwinkey) || c == Ctrl_BSL)
#اگر_تعریف_شدہ FEAT_GUI
		&& !curbuf->b_رکن->tl_system
#اگر_ختم
		)
	{
	    صحیح	    پچھلا_c = c;
	    صحیح	    پچھلا_raw_c = raw_c;
	    صحیح	    پچھلا_mod_نقاب = mod_نقاب;

	    اگر (جوڑو_مقصد_showcmd(c))
		برآمد_flush();

	    raw_c = رکن_vgetc();
	    c = raw_c_مقصد_ctrl(raw_c);

	    clear_showcmd();

	    اگر (!رکن_use_loop_check(TRUE)
					 || اندر_terminal_loop != curbuf->b_رکن)
		// job finished while waiting for a character
		توڑو;

	    اگر (پچھلا_c == Ctrl_BSL)
	    {
		اگر (c == Ctrl_N)
		{
		    // CTRL-\ CTRL-N : go to Terminal-Normal mode.
		    رکن_enter_معمول_منوال();
		    ret = FAIL;
		    جاؤ theend;
		}
		// Send both keys to the terminal, first one here, second one
		// below.
		send_کلیدیں_مقصد_رکن(curbuf->b_رکن, پچھلا_raw_c, پچھلا_mod_نقاب,
									 TRUE);
	    }
	    ورنہ اگر (c == Ctrl_C)
	    {
		// "CTRL-W CTRL-C" or 'termwinkey' CTRL-C: end the job
		mch_signal_کام(curbuf->b_رکن->tl_کام, (char_u *)"kill");
	    }
	    ورنہ اگر (c == '.')
	    {
		// "CTRL-W .": send CTRL-W to the job
		// "'termwinkey' .": send 'termwinkey' to the job
		raw_c = ctrl_مقصد_raw_c(termwinkey == 0 ? Ctrl_W : termwinkey);
	    }
	    ورنہ اگر (c == Ctrl_BSL)
	    {
		// "CTRL-W CTRL-\": send CTRL-\ to the job
		raw_c = ctrl_مقصد_raw_c(Ctrl_BSL);
	    }
	    ورنہ اگر (c == 'N')
	    {
		// CTRL-W N : go to Terminal-Normal mode.
		رکن_enter_معمول_منوال();
		ret = FAIL;
		جاؤ theend;
	    }
	    ورنہ اگر (c == '"')
	    {
		رکن_paste_رجسٹر(پچھلا_c);
		جاری;
	    }
	    ورنہ اگر (termwinkey == 0 || c != termwinkey)
	    {
		// space for CTRL-W, modifier, multi-byte char and NUL
		char_u buf[1 + 3 + MB_MAXBYTES + 1];

		// Put the command into the typeahead buffer, when using the
		// stuff buffer KeyStuffed is set and 'langmap' won't be used.
		buf[0] = Ctrl_W;
		buf[special_مقصد_buf(c, mod_نقاب, FALSE, buf + 1) + 1] = NUL;
		ins_typebuf(buf, REMAP_NONE, 0, TRUE, FALSE);
		ret = OK;
		جاؤ theend;
	    }
	}
#اگر_تعریف_شدہ MSWIN
	اگر (!enc_utf8 && has_mbyte && raw_c >= 0x80)
	{
	    WCHAR   wc;
	    char_u  mb[3];

	    mb[0] = (غیرعلامتی)raw_c >> 8;
	    mb[1] = raw_c;
	    اگر (Multi_Byte_مقصد_Wide_Char(GetACP(), 0, (حرف*)mb, 2, &wc, 1) > 0)
		raw_c = wc;
	}
#اگر_ختم
	اگر (send_کلیدیں_مقصد_رکن(curbuf->b_رکن, raw_c, mod_نقاب, TRUE) != OK)
	{
	    اگر (raw_c == K_MOUSEMOVE)
		// We are sure to come back here, don't reset the cursor color
		// and shape to avoid flickering.
		restore_cursor = FALSE;

	    ret = OK;
	    جاؤ theend;
	}
    }
    ret = FAIL;

theend:
    اندر_terminal_loop = تہی;
    اگر (restore_cursor)
	prepare_restore_cursor_props();

    // Move a snapshot of the screen contents to the buffer, so that completion
    // works in other buffers.
    اگر (curbuf->b_رکن != تہی && !curbuf->b_رکن->tl_معمول_منوال)
	may_move_terminal_مقصد_بفر(curbuf->b_رکن, FALSE);

    واپس ret;
}

    ساکن خالی
may_toggle_cursor(رکن_T *رکن)
{
    اگر (اندر_terminal_loop != رکن)
	واپس;

    اگر (رکن->tl_cursor_visible)
	cursor_on();
    ورنہ
	cursor_off();
}

/*
 * Reverse engineer the RGB value into a cterm color index.
 * First color is 1.  Return 0 if no match found (default color).
 */
    ساکن صحیح
color2index(VTermColor *color, صحیح fg, صحیح *boldp)
{
    صحیح red = color->red;
    صحیح blue = color->blue;
    صحیح green = color->green;

    *boldp = FALSE;

    اگر (VTERM_COLOR_IS_INVALID(color))
	واپس 0;

    اگر (VTERM_COLOR_IS_اشاریہ_دار(color))
    {
	// Use the color as-is if possible, give up otherwise.
	اگر (color->اشاریہ < t_colors)
	{
	    uint8_t اشاریہ = color->اشاریہ;
#اگر_تعریف_شدہ MSWIN
	    // Convert ANSI palette to cterm color index.
	    // cterm_ansi_idx is a table for the reverse conversion from cterm
	    // color index to ANSI palette. However, the current table can
	    // be used for this conversion as well.
	    // Note: If the contents of cterm_ansi_idx change in the future, a
	    // different table may be needed for this purpose.
	    اگر (اشاریہ < 16)
		اشاریہ = cterm_ansi_idx[اشاریہ];
#اگر_ختم
	    واپس اشاریہ + 1;
	}
	// 8-color terminals can actually display twice as many colors by
	// setting the high-intensity/bold bit.
	ورنہ اگر (t_colors == 8 && fg && color->اشاریہ < 16)
	{
	    *boldp = TRUE;
	    واپس (color->اشاریہ & 7) + 1;
	}
	واپس 0;
    }

    اگر (t_colors >= 256)
    {
	اگر (red == blue && red == green)
	{
	    // 24-color greyscale plus white and black
	    ساکن صحیح cutoff[23] = {
		    0x0D, 0x17, 0x21, 0x2B, 0x35, 0x3F, 0x49, 0x53, 0x5D, 0x67,
		    0x71, 0x7B, 0x85, 0x8F, 0x99, 0xA3, 0xAD, 0xB7, 0xC1, 0xCB,
		    0xD5, 0xDF, 0xE9};
	    صحیح i;

	    اگر (red < 5)
		واپس 17; // 00/00/00
	    اگر (red > 245) // ff/ff/ff
		واپس 232;
	    برائے (i = 0; i < 23; ++i)
		اگر (red < cutoff[i])
		    واپس i + 233;
	    واپس 256;
	}
	{
	    ساکن صحیح cutoff[5] = {0x2F, 0x73, 0x9B, 0xC3, 0xEB};
	    صحیح ri, gi, bi;

	    // 216-color cube
	    برائے (ri = 0; ri < 5; ++ri)
		اگر (red < cutoff[ri])
		    توڑو;
	    برائے (gi = 0; gi < 5; ++gi)
		اگر (green < cutoff[gi])
		    توڑو;
	    برائے (bi = 0; bi < 5; ++bi)
		اگر (blue < cutoff[bi])
		    توڑو;
	    واپس 17 + ri * 36 + gi * 6 + bi;
	}
    }
    واپس 0;
}

/*
 * Convert Vterm attributes to highlight flags.
 */
    ساکن صحیح
vtermAttr2hl(VTermScreenCellAttrs *cellattrs)
{
    صحیح attr = 0;

    اگر (cellattrs->bold)
	attr |= HL_BOLD;
    اگر (cellattrs->underline)
	attr |= HL_UNDERLINE;
    اگر (cellattrs->italic)
	attr |= HL_ITALIC;
    اگر (cellattrs->strike)
	attr |= HL_STRIKETHROUGH;
    اگر (cellattrs->الٹ)
	attr |= HL_INVERSE;
    واپس attr;
}

/*
 * Store Vterm attributes in "cell" from highlight flags.
 */
    ساکن خالی
hl2vtermAttr(صحیح attr, cellattr_T *cell)
{
    CLEAR_میدان(cell->attrs);
    اگر (attr & HL_BOLD)
	cell->attrs.bold = 1;
    اگر (attr & HL_UNDERLINE)
	cell->attrs.underline = 1;
    اگر (attr & HL_ITALIC)
	cell->attrs.italic = 1;
    اگر (attr & HL_STRIKETHROUGH)
	cell->attrs.strike = 1;
    اگر (attr & HL_INVERSE)
	cell->attrs.الٹ = 1;
}

/*
 * Convert the attributes of a vterm cell into an attribute index.
 */
    ساکن صحیح
cell2attr(
	رکن_T			*رکن,
	win_T			*wp,
	VTermScreenCellAttrs	*cellattrs,
	VTermColor		*cellfg,
	VTermColor		*cellbg)
{
    صحیح attr = vtermAttr2hl(cellattrs);
    VTermColor *fg = cellfg;
    VTermColor *bg = cellbg;
    صحیح is_default_fg = VTERM_COLOR_IS_DEFAULT_FG(fg);
    صحیح is_default_bg = VTERM_COLOR_IS_DEFAULT_BG(bg);

    اگر (is_default_fg || is_default_bg)
    {
	اگر (wp != تہی && wp->w_hlfwin_id != 0)
	{
	    اگر (is_default_fg)
		fg = &wp->w_رکن_hlfwin.fg;
	    اگر (is_default_bg)
		bg = &wp->w_رکن_hlfwin.bg;
	}
	ورنہ
	{
	    اگر (is_default_fg)
		fg = &رکن->tl_default_color.fg;
	    اگر (is_default_bg)
		bg = &رکن->tl_default_color.bg;
	}
    }

#اگر_تعریف_شدہ FEAT_GUI
    اگر (gui.اندر_use)
    {
	guicolor_T guifg = gui_mch_get_rgb_color(fg->red, fg->green, fg->blue);
	guicolor_T guibg = gui_mch_get_rgb_color(bg->red, bg->green, bg->blue);
	واپس get_gui_attr_idx(attr, guifg, guibg);
    }
    ورنہ
#اگر_ختم
#اگر_تعریف_شدہ FEAT_TERMGUICOLORS
    اگر (p_tgc)
    {
	guicolor_T tgcfg = VTERM_COLOR_IS_INVALID(fg)
	    ? INVALCOLOR
	    : gui_get_rgb_color_cmn(fg->red, fg->green, fg->blue);
	guicolor_T tgcbg = VTERM_COLOR_IS_INVALID(bg)
	    ? INVALCOLOR
	    : gui_get_rgb_color_cmn(bg->red, bg->green, bg->blue);
	واپس get_tgc_attr_idx(attr, tgcfg, tgcbg);
    }
    ورنہ
#اگر_ختم
    {
	صحیح bold = MAYBE;
	صحیح ctermfg = color2index(fg, TRUE, &bold);
	صحیح ctermbg = color2index(bg, FALSE, &bold);

	// with 8 colors set the bold attribute to get a bright foreground
	اگر (bold == TRUE)
	    attr |= HL_BOLD;

	واپس get_cterm_attr_idx(attr, ctermfg, ctermbg);
    }
    واپس 0;
}

    ساکن خالی
سیٹ_dirty_snapshot(رکن_T *رکن)
{
    رکن->tl_dirty_snapshot = TRUE;
#اگر_تعریف_شدہ FEAT_TIMERS
    اگر (!رکن->tl_معمول_منوال)
    {
	// Update the snapshot after 100 msec of not getting updates.
	profile_setlimit(100L, &رکن->tl_timer_due);
	رکن->tl_timer_سیٹ = TRUE;
    }
#اگر_ختم
}

    ساکن صحیح
handle_damage(VTermRect rect, خالی *user)
{
    رکن_T *رکن = (رکن_T *)user;

    رکن->tl_dirty_صف_آغاز = MIN(رکن->tl_dirty_صف_آغاز, rect.آغاز_صف);
    رکن->tl_dirty_صف_انتہا = MAX(رکن->tl_dirty_صف_انتہا, rect.انتہا_صف);
    سیٹ_dirty_snapshot(رکن);
    redraw_buf_later(رکن->tl_بفر, UPD_SOME_VALID);
    واپس 1;
}

    ساکن خالی
رکن_scroll_اوپر(رکن_T *رکن, صحیح آغاز_صف, صحیح گنتی)
{
    win_T		 *wp = تہی;
    صحیح			 did_curwin = FALSE;
    VTermColor		 fg, bg;
    VTermScreenCellAttrs attr;
    صحیح			 clear_attr;

    CLEAR_میدان(attr);

    جب (for_all_windows_and_curwin(&wp, &did_curwin))
    {
	اگر (wp->w_بفر == رکن->tl_بفر)
	{
	    // Set the color to clear lines with.
	    vterm_state_get_default_colors(vterm_obtain_state(رکن->tl_vterm),
								     &fg, &bg);
	    clear_attr = cell2attr(رکن, wp, &attr, &fg, &bg);
	    win_del_سطریں(wp, آغاز_صف, گنتی, FALSE, FALSE, clear_attr);
	}
    }
}

    ساکن صحیح
handle_moverect(VTermRect dest, VTermRect src, خالی *user)
{
    رکن_T	*رکن = (رکن_T *)user;
    صحیح		گنتی = src.آغاز_صف - dest.آغاز_صف;

    // Scrolling up is done much more efficiently by deleting lines instead of
    // redrawing the text. But avoid doing this multiple times, postpone until
    // the redraw happens.
    اگر (dest.آغاز_ستون == src.آغاز_ستون
	    && dest.انتہا_ستون == src.انتہا_ستون
	    && dest.آغاز_صف < src.آغاز_صف)
    {
	اگر (dest.آغاز_صف == 0)
	    رکن->tl_postponed_scroll += گنتی;
	ورنہ
	    رکن_scroll_اوپر(رکن, dest.آغاز_صف, گنتی);
    }

    رکن->tl_dirty_صف_آغاز = MIN(رکن->tl_dirty_صف_آغاز, dest.آغاز_صف);
    رکن->tl_dirty_صف_انتہا = MIN(رکن->tl_dirty_صف_انتہا, dest.انتہا_صف);
    سیٹ_dirty_snapshot(رکن);

    // Note sure if the scrolling will work correctly, let's do a complete
    // redraw later.
    redraw_buf_later(رکن->tl_بفر, UPD_NOT_VALID);
    واپس 1;
}

    ساکن صحیح
handle_movecursor(
	VTermPos pos,
	VTermPos oldpos غیر_مستعمل,
	صحیح visible,
	خالی *user)
{
    رکن_T	*رکن = (رکن_T *)user;
    win_T	*wp = تہی;
    صحیح		did_curwin = FALSE;

    رکن->tl_cursor_pos = pos;
    رکن->tl_cursor_visible = visible;

    جب (for_all_windows_and_curwin(&wp, &did_curwin))
    {
	اگر (wp->w_بفر == رکن->tl_بفر)
	    position_cursor(wp, &pos);
    }
    اگر (رکن->tl_بفر == curbuf && !رکن->tl_معمول_منوال)
	// Don't redraw here, it will be done after
	// vterm_input_write() is finished.
	update_cursor(رکن, FALSE);

    واپس 1;
}

    ساکن صحیح
handle_settermprop(
	VTermProp prop,
	VTerm_قدر *قدر,
	خالی *user)
{
    رکن_T	*رکن = (رکن_T *)user;
    char_u	*strval = تہی;

    انتخاب (prop)
    {
	حالت VTERM_PROP_TITLE:
	    اگر (disable_vterm_title_for_testing)
		توڑو;
	    strval = vim_strnsave((char_u *)قدر->string.str,
							    قدر->string.len);
	    اگر (strval == تہی)
		توڑو;
	    vim_free(رکن->tl_title);
	    // a blank title isn't useful, make it empty, so that "running" is
	    // displayed
	    اگر (*skipwhite(strval) == NUL)
		رکن->tl_title = تہی;
	    // Same as blank
	    ورنہ اگر (رکن->tl_arg0_cmd != تہی
		    && STRNCMP(رکن->tl_arg0_cmd, strval,
					  (صحیح)STRLEN(رکن->tl_arg0_cmd)) == 0)
		رکن->tl_title = تہی;
	    // Empty corrupted data of winpty
	    ورنہ اگر (STRNCMP("  - ", strval, 4) == 0)
		رکن->tl_title = تہی;
#اگر_تعریف_شدہ MSWIN
	    ورنہ اگر (!enc_utf8 && enc_codepage > 0)
	    {
		WCHAR   *ret = تہی;
		صحیح	لمبائی = 0;

		Multi_Byte_مقصد_Wide_Char_alloc(CP_UTF8, 0,
			(حرف*)قدر->string.str,
					(صحیح)قدر->string.len, &ret, &لمبائی);
		اگر (ret != تہی)
		{
		    Wide_Char_مقصد_Multi_Byte_alloc(enc_codepage, 0,
					ret, لمبائی, (حرف**)&رکن->tl_title,
					&لمبائی, 0, 0);
		    vim_free(ret);
		}
	    }
#اگر_ختم
	    ورنہ
	    {
		رکن->tl_title = strval;
		strval = تہی;
	    }
	    VIM_CLEAR(رکن->tl_حالت_متن);
	    اگر (رکن == curbuf->b_رکن)
	    {
		maketitle();
		curwin->w_redr_حالت = true;
	    }
	    توڑو;

	حالت VTERM_PROP_CURSORVISIBLE:
	    رکن->tl_cursor_visible = قدر->boolean;
	    may_toggle_cursor(رکن);
	    برآمد_flush();
	    توڑو;

	حالت VTERM_PROP_CURSORBLINK:
	    رکن->tl_cursor_blink = قدر->boolean;
	    may_سیٹ_cursor_props(رکن);
	    توڑو;

	حالت VTERM_PROP_CURSORSHAPE:
	    رکن->tl_cursor_shape = قدر->نمبر;
	    may_سیٹ_cursor_props(رکن);
	    توڑو;

	حالت VTERM_PROP_CURSORCOLOR:
	    strval = vim_strnsave((char_u *)قدر->string.str,
							    قدر->string.len);
	    اگر (strval == تہی)
		توڑو;
	    cursor_color_copy(&رکن->tl_cursor_color, strval);
	    may_سیٹ_cursor_props(رکن);
	    توڑو;

	حالت VTERM_PROP_ALTSCREEN:
	    // TODO: do anything else?
	    رکن->tl_using_altscreen = قدر->boolean;
	    توڑو;

	طےشدہ:
	    توڑو;
    }
    vim_free(strval);

    // Always return 1, otherwise vterm doesn't store the value internally.
    واپس 1;
}

/*
 * The job running in the terminal resized the terminal.
 */
    ساکن صحیح
handle_resize(صحیح صفیں_2, صحیح ستون_تعداد, خالی *user)
{
    رکن_T	*رکن = (رکن_T *)user;
    win_T	*wp;

    رکن->tl_صفیں = صفیں_2;
    رکن->tl_ستون_تعداد = ستون_تعداد;
    اگر (رکن->tl_vterm_سائز_بدلا)
	// Size was set by vterm_set_size(), don't set the window size.
	رکن->tl_vterm_سائز_بدلا = FALSE;
    ورنہ
    {
	FOR_ALL_WINDOWS(wp)
	{
	    اگر (wp->w_بفر == رکن->tl_بفر)
	    {
		win_setheight_win(صفیں_2, wp);
		win_setwidth_win(ستون_تعداد, wp);
	    }
	}
	redraw_buf_later(رکن->tl_بفر, UPD_NOT_VALID);
    }
    واپس 1;
}

/*
 * If the number of lines that are stored goes over 'termwinscroll' then
 * delete the first 10%.
 * "gap" points to tl_scrollback or tl_scrollback_postponed.
 * "update_buffer" is TRUE when the buffer should be updated.
 */
    ساکن خالی
حد_بندی_scrollback(رکن_T *رکن, garray_T *gap, صحیح update_بفر)
{
    اگر (gap->ga_len < رکن->tl_بفر->b_p_twsl)
	واپس;

    صحیح	todo = MAX(رکن->tl_بفر->b_p_twsl / 10,
				     gap->ga_len - رکن->tl_بفر->b_p_twsl);
    صحیح	i;
    sb_سطر_T *sb_سطریں = (sb_سطر_T *)gap->ga_ڈیٹا;

    curbuf = رکن->tl_بفر;
    برائے (i = 0; i < todo; ++i)
    {
	اگر (update_بفر && (!sb_سطریں[i].continuation || !i))
	{
	    ml_delete(1);
	    --رکن->tl_بفر_scrolled;
	}
	vim_free(sb_سطریں[i].sb_cells);
    }
    // Continue until end of wrapped line
    برائے (; todo < gap->ga_len && sb_سطریں[todo].continuation; ++todo)
	vim_free(sb_سطریں[todo].sb_cells);
    curbuf = curwin->w_بفر;

    gap->ga_len -= todo;
    mch_memmove(gap->ga_ڈیٹا,
		(sb_سطر_T *)gap->ga_ڈیٹا + todo,
		حجم(sb_سطر_T) * gap->ga_len);
    اگر (update_بفر)
    {
	win_T *curwin_save = curwin;
	win_T *wp = تہی;

	رکن->tl_scrollback_scrolled -= todo;

	FOR_ALL_WINDOWS(wp)
	{
	    اگر (wp->w_بفر == رکن->tl_بفر)
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
    ساکن صحیح
handle_pushline(صحیح ستون_تعداد, ثابت VTermScreenCell *cells, صحیح continuation, خالی *user)
{
    رکن_T	*رکن = (رکن_T *)user;
    garray_T	*gap;
    صحیح		update_بفر;

    اگر (رکن->tl_معمول_منوال)
    {
	// In Terminal-Normal mode the user interacts with the buffer, thus we
	// must not change it. Postpone adding the scrollback lines.
	gap = &رکن->tl_scrollback_postponed;
	update_بفر = FALSE;
    }
    ورنہ
    {
	// First remove the lines that were appended before, the pushed line
	// goes above it.
	cleanup_scrollback(رکن);
	gap = &رکن->tl_scrollback;
	update_بفر = TRUE;
    }

    حد_بندی_scrollback(رکن, gap, update_بفر);

    اگر (ga_grow(gap, 1) == FAIL)
	واپس 0;

    cellattr_T	*p = تہی;
    صحیح		len = 0;
    صحیح		i;
    صحیح		c;
    صحیح		ستون;
    صحیح		متن_len;
    char_u		*متن;
    sb_سطر_T	*سطر;
    garray_T	ga;
    cellattr_T	fill_attr = رکن->tl_default_color;

    // do not store empty cells at the end
    برائے (i = 0; i < ستون_تعداد; ++i)
	اگر (cells[i].chars[0] != 0)
	    len = i + 1;
	ورنہ
	    cell2cellattr(&cells[i], &fill_attr);

    ga_init2(&ga, 1, 100);
    اگر (len > 0)
	p = ALLOC_MULT(cellattr_T, len);
    اگر (p != تہی)
    {
	برائے (ستون = 0; ستون < len; ستون += cells[ستون].چوڑائی)
	{
	    اگر (ga_grow(&ga, VTERM_MAX_CHARS_فی_CELL * 4) == FAIL)
	    {
		ga.ga_len = 0;
		توڑو;
	    }
	    برائے (i = 0; i < VTERM_MAX_CHARS_فی_CELL &&
		    ((c = cells[ستون].chars[i]) > 0 || i == 0); ++i)
		ga.ga_len += utf_char2bytes(c == NUL ? ' ' : c,
			(char_u *)ga.ga_ڈیٹا + ga.ga_len);
	    cell2cellattr(&cells[ستون], &p[ستون]);
	}
    }
    اگر (ga_grow(&ga, 1) == FAIL)
    {
	اگر (update_بفر)
	    متن = (char_u *)"";
	ورنہ
	    متن = vim_strsave((char_u *)"");
	متن_len = 0;
    }
    ورنہ
    {
	متن = ga.ga_ڈیٹا;
	متن_len = ga.ga_len;
	*(متن + متن_len) = NUL;
    }
    اگر (update_بفر)
	جوڑو_scrollback_سطر_مقصد_بفر(رکن, متن, متن_len, continuation);

    سطر = (sb_سطر_T *)gap->ga_ڈیٹا + gap->ga_len;
    سطر->sb_ستون_تعداد = len;
    سطر->sb_بائٹیں = متن_len;
    سطر->sb_cells = p;
    سطر->sb_fill_attr = fill_attr;
    سطر->continuation = (char_u)continuation;
    اگر (update_بفر)
    {
	سطر->sb_متن = تہی;
	++رکن->tl_scrollback_scrolled;
	اگر (!continuation)
	    ++رکن->tl_بفر_scrolled;
	ga_clear(&ga);  // free the text
    }
    ورنہ
    {
	سطر->sb_متن = متن;
	ga_init(&ga);  // text is kept in tl_scrollback_postponed
    }
    ++gap->ga_len;
    واپس 0; // ignored
}

/*
 * Called when leaving Terminal-Normal mode: deal with any scrollback that was
 * received and stored in tl_scrollback_postponed.
 */
    ساکن خالی
handle_postponed_scrollback(رکن_T *رکن)
{
    صحیح i;

    اگر (رکن->tl_scrollback_postponed.ga_len == 0)
	واپس;
    ch_log(تہی, "Moving postponed scrollback to scrollback");

    // First remove the lines that were appended before, the pushed lines go
    // above it.
    cleanup_scrollback(رکن);

    برائے (i = 0; i < رکن->tl_scrollback_postponed.ga_len; ++i)
    {
	char_u		*متن;
	sb_سطر_T	*pp_سطر;
	sb_سطر_T	*سطر;

	اگر (ga_grow(&رکن->tl_scrollback, 1) == FAIL)
	    توڑو;
	pp_سطر = (sb_سطر_T *)رکن->tl_scrollback_postponed.ga_ڈیٹا + i;

	متن = pp_سطر->sb_متن;
	اگر (متن == تہی)
	    متن = (char_u *)"";
	جوڑو_scrollback_سطر_مقصد_بفر(رکن, متن, (صحیح)STRLEN(متن), pp_سطر->continuation);
	vim_free(pp_سطر->sb_متن);

	سطر = (sb_سطر_T *)رکن->tl_scrollback.ga_ڈیٹا
						 + رکن->tl_scrollback.ga_len;
	سطر->sb_ستون_تعداد = pp_سطر->sb_ستون_تعداد;
	سطر->sb_بائٹیں = pp_سطر->sb_بائٹیں;
	سطر->sb_cells = pp_سطر->sb_cells;
	سطر->sb_fill_attr = pp_سطر->sb_fill_attr;
	سطر->sb_متن = تہی;
	++رکن->tl_scrollback_scrolled;
	++رکن->tl_scrollback.ga_len;
	اگر (!pp_سطر->continuation)
	    ++رکن->tl_بفر_scrolled;
    }

    ga_clear(&رکن->tl_scrollback_postponed);
    حد_بندی_scrollback(رکن, &رکن->tl_scrollback, TRUE);
}

/*
 * Called when the terminal wants to ring the system bell.
 */
    ساکن صحیح
handle_bell(خالی *user غیر_مستعمل)
{
    vim_beep(BO_رکن);
    واپس 0;
}

ساکن VTermScreenCallbacks screen_callbacks = {
    handle_damage,		// damage
    handle_moverect,		// moverect
    handle_movecursor,		// movecursor
    handle_settermprop,		// settermprop
    handle_bell,		// bell
    handle_resize,		// resize
    تہی,			// sb_pushline
    تہی,			// sb_popline
    تہی,			// sb_clear
    handle_pushline	// sb_pushline4
};

/*
 * Do the work after the channel of a terminal was closed.
 * Must be called only when updating_screen is FALSE.
 * Returns TRUE when a buffer was closed (list of terminals may have changed).
 */
    ساکن صحیح
رکن_after_channel_closed(رکن_T *رکن)
{
    // Unless in Terminal-Normal mode: clear the vterm.
    اگر (!رکن->tl_معمول_منوال)
    {
	صحیح	fnum = رکن->tl_بفر->b_fnum;

	cleanup_vterm(رکن);

	اگر (رکن->tl_اختتام == TL_اختتام_CLOSE)
	{
	    aco_save_T	aco;
	    صحیح		do_سیٹ_w_locked = رکن->tl_بفر->b_nwindows == 0;
#اگر_تعریف_شدہ FEAT_PROP_POPUP
	    win_T	*pwin = تہی;

	    // If this was a terminal in a popup window, go back to the
	    // previous window.
	    اگر (popup_is_popup(curwin) && curbuf == رکن->tl_بفر)
	    {
		pwin = curwin;
		اگر (win_valid(prevwin))
		    win_enter(prevwin, FALSE);
	    }
	    ورنہ
#اگر_ختم
	    // If this is the last normal window: exit Vim.
	    اگر (رکن->tl_بفر->b_nwindows > 0 && only_one_کھڑکی())
	    {
		exarg_T ea;

		CLEAR_میدان(ea);
		ex_quit(&ea);
		واپس TRUE;
	    }

	    // ++close or term_finish == "close"
	    ch_log(تہی, "terminal job finished, closing window");
	    aucmd_prepbuf(&aco, رکن->tl_بفر);
	    اگر (curbuf == رکن->tl_بفر)
	    {
		win_T	*wp = curwin;

		// Avoid closing the window if we temporarily use it.
		اگر (is_aucmd_win(wp))
		    do_سیٹ_w_locked = TRUE;
		اگر (do_سیٹ_w_locked)
		    ++wp->w_locked;
		do_bufdel(DOBUF_WIPE, (char_u *)"", 1, fnum, fnum, FALSE);
		اگر (do_سیٹ_w_locked)
		    --wp->w_locked;
		aucmd_restbuf(&aco);
	    }
#اگر_تعریف_شدہ FEAT_PROP_POPUP
	    اگر (pwin != تہی)
		popup_close_with_retval(pwin, 0);
#اگر_ختم
	    واپس TRUE;
	}
	اگر (رکن->tl_اختتام == TL_اختتام_OPEN
				   && رکن->tl_بفر->b_nwindows == 0)
	{
	    حرف    *cmd = رکن->tl_opencmd == تہی
				? "botright sbuf %d"
				: (حرف *)رکن->tl_opencmd;
	    size_t  len = لڑی_لمبائی(cmd) + 50;
	    حرف    *buf = alloc(len);

	    اگر (buf != تہی)
	    {
		ch_log(تہی, "terminal job finished, opening window");
		vim_snprintf(buf, len, cmd, fnum);
		do_cmdline_cmd((char_u *)buf);
		vim_free(buf);
	    }
	}
	ورنہ
	    ch_log(تہی, "terminal job finished");
    }

    redraw_buf_and_حالت_later(رکن->tl_بفر, UPD_NOT_VALID);
    واپس FALSE;
}

#اگر defined(FEAT_PROP_POPUP)
/*
 * If the current window is a terminal in a popup window and the job has
 * finished, close the popup window and to back to the previous window.
 * Otherwise return FAIL.
 */
    صحیح
may_close_رکن_popup(خالی)
{
    اگر (!popup_is_popup(curwin) || curbuf->b_رکن == تہی
				 || رکن_کام_running_not_none(curbuf->b_رکن))
	واپس FAIL;

    win_T *pwin = curwin;

    اگر (win_valid(prevwin))
	win_enter(prevwin, FALSE);
    popup_close_with_retval(pwin, 0);
    واپس OK;
}
#اگر_ختم

/*
 * Called when a channel is going to be closed, before invoking the close
 * callback.
 */
    خالی
رکن_channel_closing(channel_T *ch)
{
    رکن_T *رکن;

    برائے (رکن = پہلا_رکن; رکن != تہی; رکن = رکن->tl_اگلا)
	اگر (رکن->tl_کام == ch->ch_کام && !رکن->tl_channel_closed)
	    رکن->tl_channel_closing = TRUE;
}

/*
 * Called when a channel has been closed.
 * If this was a channel for a terminal window then finish it up.
 */
    خالی
رکن_channel_closed(channel_T *ch)
{
    رکن_T *رکن;
    رکن_T *اگلا_رکن;
    صحیح	    did_one = FALSE;

    برائے (رکن = پہلا_رکن; رکن != تہی; رکن = اگلا_رکن)
    {
	اگلا_رکن = رکن->tl_اگلا;
	اگر (رکن->tl_کام == ch->ch_کام && !رکن->tl_channel_closed)
	{
	    رکن->tl_channel_closed = TRUE;
	    did_one = TRUE;

	    VIM_CLEAR(رکن->tl_title);
	    VIM_CLEAR(رکن->tl_حالت_متن);
#اگر_تعریف_شدہ MSWIN
	    اگر (رکن->tl_برآمد_fd != تہی)
	    {
		fclose(رکن->tl_برآمد_fd);
		رکن->tl_برآمد_fd = تہی;
	    }
#اگر_ختم

	    اگر (updating_screen)
	    {
		// Cannot open or close windows now.  Can happen when
		// 'lazyredraw' is set.
		رکن->tl_channel_recently_closed = TRUE;
		جاری;
	    }

	    اگر (رکن_after_channel_closed(رکن))
		اگلا_رکن = پہلا_رکن;
	}
    }

    اگر (did_one)
    {
	redraw_statuslines();

	// Need to break out of vgetc().
	ins_char_typebuf(K_IGNORE, 0);
	typebuf_was_filled = TRUE;

	رکن = curbuf->b_رکن;
	اگر (رکن != تہی)
	{
	    اگر (رکن->tl_کام == ch->ch_کام)
		maketitle();
	    update_cursor(رکن, رکن->tl_cursor_visible);
	}
    }
}

/*
 * To be called after resetting updating_screen: handle any terminal where the
 * channel was closed.
 */
    خالی
رکن_check_channel_closed_recently(خالی)
{
    رکن_T *رکن;
    رکن_T *اگلا_رکن;

    برائے (رکن = پہلا_رکن; رکن != تہی; رکن = اگلا_رکن)
    {
	اگلا_رکن = رکن->tl_اگلا;
	اگر (رکن->tl_channel_recently_closed)
	{
	    رکن->tl_channel_recently_closed = FALSE;
	    اگر (رکن_after_channel_closed(رکن))
		// start over, the list may have changed
		اگلا_رکن = پہلا_رکن;
	}
    }
}

/*
 * Fill one screen line from a line of the terminal.
 * Advances "pos" to past the last column.
 */
    ساکن خالی
رکن_line2screenline(
	رکن_T		*رکن,
	win_T		*wp,
	VTermScreen	*screen,
	VTermPos	*pos,
	صحیح		max_ستون)
{
    صحیح off = screen_get_current_سطر_off();

    برائے (pos->ستون = 0; pos->ستون < max_ستون; )
    {
	VTermScreenCell cell;
	صحیح		c;

	اگر (vterm_screen_get_cell(screen, *pos, &cell) == 0)
	    CLEAR_میدان(cell);

	c = cell.chars[0];
	اگر (c == NUL)
	{
	    Screen_سطریں[off] = ' ';
	    اگر (enc_utf8)
		Screen_سطریں_UC[off] = NUL;
	}
	ورنہ
	{
	    اگر (enc_utf8)
	    {
		صحیح i;

		// composing chars
		برائے (i = 0; i < Screen_mco
			      && i + 1 < VTERM_MAX_CHARS_فی_CELL; ++i)
		{
		    Screen_سطریں_C[i][off] = cell.chars[i + 1];
		    اگر (cell.chars[i + 1] == 0)
			توڑو;
		}
		اگر (c >= 0x80 || (Screen_mco > 0
					 && Screen_سطریں_C[0][off] != 0))
		{
		    Screen_سطریں[off] = ' ';
		    Screen_سطریں_UC[off] = c;
		}
		ورنہ
		{
		    Screen_سطریں[off] = c;
		    Screen_سطریں_UC[off] = NUL;
		}
	    }
#اگر_تعریف_شدہ MSWIN
	    ورنہ اگر (has_mbyte && c >= 0x80)
	    {
		char_u	mb[MB_MAXBYTES+1];
		WCHAR	wc = c;

		اگر (Wide_Char_مقصد_Multi_Byte(GetACP(), 0, &wc, 1,
					       (حرف*)mb, 2, 0, 0) > 1)
		{
		    Screen_سطریں[off] = mb[0];
		    Screen_سطریں[off + 1] = mb[1];
		    cell.چوڑائی = mb_ptr2cells(mb);
		}
		ورنہ
		    Screen_سطریں[off] = c;
	    }
#اگر_ختم
	    ورنہ
		// This will only store the lower byte of "c".
		Screen_سطریں[off] = c;
	}
	ScreenAttrs[off] = cell2attr(رکن, wp, &cell.attrs, &cell.fg,
								     &cell.bg);

	++pos->ستون;
	++off;
	اگر (cell.چوڑائی == 2)
	{
	    // don't set the second byte to NUL for a DBCS encoding, it
	    // has been set above
	    اگر (enc_utf8)
	    {
		Screen_سطریں_UC[off] = NUL;
		Screen_سطریں[off] = NUL;
	    }
	    ورنہ اگر (!has_mbyte)
	    {
		// Can't show a double-width character with a single-byte
		// 'encoding', just use a space.
		Screen_سطریں[off] = ' ';
		ScreenAttrs[off] = ScreenAttrs[off - 1];
	    }

	    ++pos->ستون;
	    ++off;
	}
    }
}

#اگر defined(FEAT_GUI)
    ساکن خالی
update_system_رکن(رکن_T *رکن)
{
    VTermPos	    pos;
    VTermScreen	    *screen;

    اگر (رکن->tl_vterm == تہی)
	واپس;
    screen = vterm_obtain_screen(رکن->tl_vterm);

    // Scroll up to make more room for terminal lines if needed.
    // Use the cursor position to determine how much to scroll, because
    // ConPTY may damage all rows on initialization even when most are
    // empty, which would cause unnecessary scrolling.
    جب (رکن->tl_toprow > 0
		  && (صفیں - رکن->tl_toprow) < رکن->tl_cursor_pos.صف + 1)
    {
	صحیح save_p_مزید = p_مزید;

	p_مزید = FALSE;
	پیغام_صف = صفیں - 1;
	پیغام_puts("\n");
	p_مزید = save_p_مزید;
	--رکن->tl_toprow;
    }

    برائے (pos.صف = رکن->tl_dirty_صف_آغاز; pos.صف < رکن->tl_dirty_صف_انتہا
						  && pos.صف < صفیں; ++pos.صف)
    {
	اگر (pos.صف < رکن->tl_صفیں)
	{
	    صحیح max_ستون = MIN(topframe->fr_چوڑائی, رکن->tl_ستون_تعداد);

	    رکن_line2screenline(رکن, تہی, screen, &pos, max_ستون);
	}
	ورنہ
	    pos.ستون = 0;

	screen_سطر(curwin, رکن->tl_toprow + pos.صف,
		firstwin->w_wincol, pos.ستون, topframe->fr_چوڑائی, -1, 0);
    }

    رکن->tl_dirty_صف_آغاز = MAX_صف;
    رکن->tl_dirty_صف_انتہا = 0;
}
#اگر_ختم

/*
 * Return TRUE if window "wp" is to be redrawn with term_update_window().
 * Returns FALSE when there is no terminal running in this window or it is in
 * Terminal-Normal mode.
 */
    صحیح
رکن_do_update_کھڑکی(win_T *wp)
{
    رکن_T	*رکن = wp->w_بفر->b_رکن;

    واپس رکن != تہی && رکن->tl_vterm != تہی && !رکن->tl_معمول_منوال;
}

/*
 * Called to update a window that contains an active terminal.
 */
    خالی
رکن_update_کھڑکی(win_T *wp)
{
    رکن_T	*رکن = wp->w_بفر->b_رکن;
    VTerm	*vterm;
    VTermScreen *screen;
    VTermState	*state;
    VTermPos	pos;
    صحیح		صفیں_2, ستون_تعداد;
    صحیح		newrows, newcols;
    صحیح		minsize;
    win_T	*twp;

    vterm = رکن->tl_vterm;
    screen = vterm_obtain_screen(vterm);
    state = vterm_obtain_state(vterm);

    // We use UPD_NOT_VALID on a resize or scroll, redraw everything then.
    // With UPD_SOME_VALID only redraw what was marked dirty.
    اگر (wp->w_redr_type > UPD_SOME_VALID)
    {
	رکن->tl_dirty_صف_آغاز = 0;
	رکن->tl_dirty_صف_انتہا = MAX_صف;

	اگر (رکن->tl_postponed_scroll > 0
			      && رکن->tl_postponed_scroll < رکن->tl_صفیں / 3)
	    // Scrolling is usually faster than redrawing, when there are only
	    // a few lines to scroll.
	    رکن_scroll_اوپر(رکن, 0, رکن->tl_postponed_scroll);
	رکن->tl_postponed_scroll = 0;
    }

    /*
     * If the window was resized a redraw will be triggered and we get here.
     * Adjust the size of the vterm unless 'termwinsize' specifies a fixed size.
     */
    minsize = parse_termwinsize(wp, &صفیں_2, &ستون_تعداد);

    newrows = 99999;
    newcols = 99999;
    برائے (twp = firstwin; ; twp = twp->w_اگلا)
    {
	// Always use curwin, it may be a popup window.
	win_T *wwp = twp == تہی ? curwin : twp;

	// When more than one window shows the same terminal, use the
	// smallest size.
	اگر (wwp->w_بفر == رکن->tl_بفر)
	{
	    newrows = MIN(newrows, wwp->w_اونچائی);
	    newcols = MIN(newcols, wwp->w_چوڑائی);
	}
	اگر (twp == تہی)
	    توڑو;
    }
    اگر (newrows == 99999 || newcols == 99999)
	واپس; // safety exit
    newrows = صفیں_2 == 0 ? newrows : minsize ? MAX(صفیں_2, newrows) : صفیں_2;
    newcols = ستون_تعداد == 0 ? newcols : minsize ? MAX(ستون_تعداد, newcols) : ستون_تعداد;

    // If no cell is visible there is no point in resizing.  Also, vterm can't
    // handle a zero height.
    اگر (newrows == 0 || newcols == 0)
	واپس;

    اگر (رکن->tl_صفیں != newrows || رکن->tl_ستون_تعداد != newcols)
    {
	رکن->tl_vterm_سائز_بدلا = TRUE;
	vterm_سیٹ_سائز(vterm, newrows, newcols);
	ch_log(رکن->tl_کام->jv_channel, "Resizing terminal to %d lines",
								      newrows);
	رکن_report_winsize(رکن, newrows, newcols);

	// Updating the terminal size will cause the snapshot to be cleared.
	// When not in terminal_loop() we need to restore it.
	اگر (رکن != اندر_terminal_loop)
	    may_move_terminal_مقصد_بفر(رکن, FALSE);
    }

    // The cursor may have been moved when resizing.
    vterm_state_get_cursorpos(state, &pos);
    position_cursor(wp, &pos);

    برائے (pos.صف = رکن->tl_dirty_صف_آغاز; pos.صف < رکن->tl_dirty_صف_انتہا
					  && pos.صف < wp->w_اونچائی; ++pos.صف)
    {
	اگر (pos.صف < رکن->tl_صفیں)
	{
	    صحیح max_ستون = MIN(wp->w_چوڑائی, رکن->tl_ستون_تعداد);

	    رکن_line2screenline(رکن, wp, screen, &pos, max_ستون);
	}
	ورنہ
	    pos.ستون = 0;

	screen_سطر(wp, wp->w_winrow + pos.صف
#اگر_تعریف_شدہ FEAT_MENU
				+ winbar_اونچائی(wp)
#اگر_ختم
				, wp->w_wincol, pos.ستون, wp->w_چوڑائی, -1,
#اگر_تعریف_شدہ FEAT_PROP_POPUP
				popup_is_popup(wp) ? SLF_POPUP :
#اگر_ختم
				0);
    }
}

/*
 * Called after updating all windows: may reset dirty rows.
 */
    خالی
رکن_did_update_کھڑکی(win_T *wp)
{
    رکن_T	*رکن = wp->w_بفر->b_رکن;

    اگر (رکن == تہی || رکن->tl_vterm == تہی || رکن->tl_معمول_منوال
						|| wp->w_redr_type != 0)
	واپس;

    رکن->tl_dirty_صف_آغاز = MAX_صف;
    رکن->tl_dirty_صف_انتہا = 0;
}

/*
 * Return TRUE if "wp" is a terminal window where the job has finished.
 */
    صحیح
رکن_is_ختم_شدہ(buf_T *buf)
{
    واپس buf->b_رکن != تہی && buf->b_رکن->tl_vterm == تہی;
}

/*
 * Return TRUE if "wp" is a terminal window where the job has finished or we
 * are in Terminal-Normal mode, thus we show the buffer contents.
 */
    صحیح
رکن_show_بفر(buf_T *buf)
{
    رکن_T *رکن = buf->b_رکن;

    واپس رکن != تہی && (رکن->tl_vterm == تہی || رکن->tl_معمول_منوال);
}

/*
 * The current buffer is going to be changed.  If there is terminal
 * highlighting remove it now.
 */
    خالی
رکن_change_اندر_curbuf(خالی)
{
    رکن_T *رکن = curbuf->b_رکن;

    اگر (!رکن_is_ختم_شدہ(curbuf) || رکن->tl_scrollback.ga_len <= 0)
	واپس;

    free_scrollback(رکن);
    redraw_buf_later(رکن->tl_بفر, UPD_NOT_VALID);

    // The buffer is now like a normal buffer, it cannot be easily
    // abandoned when changed.
    سیٹ_string_option_براہ_راست((char_u *)"buftype", -1,
	    (char_u *)"", OPT_FREE|OPT_LOCAL, 0);
}

/*
 * Get the screen attribute for a position in the buffer.
 * Use a negative "col" to get the filler background color.
 */
    صحیح
رکن_get_attr(win_T *wp, linenr_T lnum, صحیح ستون)
{
    buf_T	*buf = wp->w_بفر;
    رکن_T	*رکن = buf->b_رکن;
    sb_سطر_T	*سطر;
    cellattr_T	*cellattr;
    صحیح         sb_سطر = -1;
    صحیح         sb_ستون = ستون;

    اگر (رکن->tl_scrollback.ga_len)
	bufline_pos_اندر_scrollback(رکن, lnum, ستون, &sb_سطر, &sb_ستون);

    اگر (sb_سطر < 0)
	cellattr = &رکن->tl_default_color;
    ورنہ
    {
	سطر = (sb_سطر_T *)رکن->tl_scrollback.ga_ڈیٹا + sb_سطر;
	اگر (sb_ستون < 0 || sb_ستون >= سطر->sb_ستون_تعداد)
	    cellattr = &سطر->sb_fill_attr;
	ورنہ
	    cellattr = سطر->sb_cells + sb_ستون;
    }
    واپس cell2attr(رکن, wp, &cellattr->attrs, &cellattr->fg, &cellattr->bg);
}

/*
 * Convert a cterm color number 0 - 255 to RGB.
 * This is compatible with xterm.
 */
    ساکن خالی
cterm_color2vterm(صحیح nr, VTermColor *rgb)
{
    cterm_color2rgb(nr, &rgb->red, &rgb->green, &rgb->blue, &rgb->اشاریہ);
    اگر (rgb->اشاریہ == 0)
	rgb->type = VTERM_COLOR_RGB;
    ورنہ
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
    ساکن صحیح
get_vterm_color_from_synid(صحیح id, VTermColor *fg, VTermColor *bg)
{
#اگر defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    // Use the actual color for the GUI and when 'termguicolors' is set.
    اگر (0
# ifdef FEAT_GUI
	    || gui.اندر_use
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

	اگر (id > 0)
	    syn_id2colors(id, &fg_rgb, &bg_rgb);

	اگر (fg_rgb != INVALCOLOR)
	{
	    long_u rgb = GUI_MCH_GET_RGB(fg_rgb);
	    fg->red = (غیرعلامتی)(rgb >> 16);
	    fg->green = (غیرعلامتی)(rgb >> 8) & 255;
	    fg->blue = (غیرعلامتی)rgb & 255;
	    fg->type = VTERM_COLOR_RGB | VTERM_COLOR_DEFAULT_FG;
	}
	ورنہ
	    fg->type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_FG;

	اگر (bg_rgb != INVALCOLOR)
	{
	    long_u rgb = GUI_MCH_GET_RGB(bg_rgb);
	    bg->red = (غیرعلامتی)(rgb >> 16);
	    bg->green = (غیرعلامتی)(rgb >> 8) & 255;
	    bg->blue = (غیرعلامتی)rgb & 255;
	    bg->type = VTERM_COLOR_RGB | VTERM_COLOR_DEFAULT_BG;
	}
	ورنہ
	    bg->type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_BG;

	واپس TRUE;
    }
    ورنہ
#اگر_ختم
    اگر (t_colors >= 16)
    {
	صحیح cterm_fg = -1;
	صحیح cterm_bg = -1;

	اگر (id > 0)
	    syn_id2cterm_bg(id, &cterm_fg, &cterm_bg);

	اگر (cterm_fg >= 0)
	{
	    cterm_color2vterm(cterm_fg, fg);
	    fg->type |= VTERM_COLOR_DEFAULT_FG;
	}
	ورنہ
	    fg->type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_FG;

	اگر (cterm_bg >= 0)
	{
	    cterm_color2vterm(cterm_bg, bg);
	    bg->type |= VTERM_COLOR_DEFAULT_BG;
	}
	ورنہ
	    bg->type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_BG;

	واپس TRUE;
    }

    واپس FALSE;
}

    خالی
رکن_reset_hlfwin(win_T *wp)
{
    wp->w_رکن_hlfwin.fg.type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_FG;
    wp->w_رکن_hlfwin.bg.type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_BG;
}

/*
 * Cache the color of HLF_WIN.
 */
    خالی
رکن_update_hlfwin(win_T *wp)
{
    صحیح id = wp->w_hlfwin_id;

    اگر (id == 0 || !get_vterm_color_from_synid(id == -1 ? 0 : id,
		&wp->w_رکن_hlfwin.fg, &wp->w_رکن_hlfwin.bg))
	رکن_reset_hlfwin(wp);
}

/*
 * Called when option 'termguicolors' was set,
 * or when any highlight is changed.
 */
    خالی
رکن_update_hlfwin_all(خالی)
{
    win_T	 *wp = تہی;
    صحیح		 did_curwin = FALSE;

    جب (for_all_windows_and_curwin(&wp, &did_curwin))
	رکن_update_hlfwin(wp);
}

/*
 * Initialize term->tl_default_color from the environment.
 */
    خالی
رکن_init_default_colors(رکن_T *رکن)
{
    VTermColor	    *fg, *bg;
    صحیح		    fgval, bgval;
    صحیح		    id;

    CLEAR_میدان(رکن->tl_default_color.attrs);
    رکن->tl_default_color.چوڑائی = 1;
    fg = &رکن->tl_default_color.fg;
    bg = &رکن->tl_default_color.bg;

    // Vterm uses a default black background.  Set it to white when
    // 'background' is "light".
    اگر (*p_bg == 'l')
    {
	fgval = 0;
	bgval = 255;
    }
    ورنہ
    {
	fgval = 255;
	bgval = 0;
    }
    fg->red = fg->green = fg->blue = fgval;
    bg->red = bg->green = bg->blue = bgval;
    fg->type = VTERM_COLOR_RGB | VTERM_COLOR_DEFAULT_FG;
    bg->type = VTERM_COLOR_RGB | VTERM_COLOR_DEFAULT_BG;

    // The highlight group overrules the defaults.
    id = رکن_get_highlight_id(رکن, تہی);

    اگر (!get_vterm_color_from_synid(id, fg, bg))
    {
#اگر defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
	صحیح tmp;
#اگر_ختم

	// In an MS-Windows console we know the normal colors.
	اگر (cterm_معمول_fg_color > 0)
	{
	    cterm_color2vterm(cterm_معمول_fg_color - 1, fg);
#اگر defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
# ifdef VIMDLL
	    اگر (!gui.اندر_use)
# endif
	    {
		tmp = fg->red;
		fg->red = fg->blue;
		fg->blue = tmp;
	    }
#اگر_ختم
	}
#اگر_تعریف_شدہ FEAT_TERMRESPONSE
	ورنہ
	    رکن_get_fg_color(&fg->red, &fg->green, &fg->blue);
#اگر_ختم

	اگر (cterm_معمول_bg_color > 0)
	{
	    cterm_color2vterm(cterm_معمول_bg_color - 1, bg);
#اگر defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
# ifdef VIMDLL
	    اگر (!gui.اندر_use)
# endif
	    {
		tmp = fg->red;
		fg->red = fg->blue;
		fg->blue = tmp;
	    }
#اگر_ختم
	}
#اگر_تعریف_شدہ FEAT_TERMRESPONSE
	ورنہ
	    رکن_get_bg_color(&bg->red, &bg->green, &bg->blue);
#اگر_ختم
    }
}

#اگر defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Return TRUE if the user-defined palette (either g:terminal_ansi_colors or the
 * "ansi_colors" argument in term_start()) shall be applied.
 */
    ساکن صحیح
رکن_use_palette(خالی)
{
    اگر (0
# ifdef FEAT_GUI
	    || gui.اندر_use
# endif
# ifdef FEAT_TERMGUICOLORS
	    || p_tgc
# endif
       )
	واپس TRUE;
    واپس FALSE;
}

/*
 * Set the 16 ANSI colors from array of RGB values
 */
    ساکن خالی
سیٹ_vterm_palette(VTerm *vterm, long_u *rgb)
{
    صحیح		اشاریہ = 0;
    VTermState	*state = vterm_obtain_state(vterm);

    برائے (; اشاریہ < 16; اشاریہ++)
    {
	VTermColor	color;

	color.type = VTERM_COLOR_RGB;
	color.red = (غیرعلامتی)(rgb[اشاریہ] >> 16);
	color.green = (غیرعلامتی)(rgb[اشاریہ] >> 8) & 255;
	color.blue = (غیرعلامتی)rgb[اشاریہ] & 255;
	color.اشاریہ = 0;
	vterm_state_سیٹ_palette_color(state, اشاریہ, &color);
    }
}

/*
 * Set the ANSI color palette from a list of colors
 */
    ساکن صحیح
سیٹ_ansi_colors_list(VTerm *vterm, list_T *list)
{
    صحیح		n = 0;
    long_u	rgb[16];
    listitem_T	*li;

    برائے (li = list->lv_پہلا; li != تہی && n < 16; li = li->li_اگلا, n++)
    {
	char_u		*color_name;
	guicolor_T	guicolor;

	color_name = tv_get_string_chk(&li->li_tv);
	اگر (color_name == تہی)
	    واپس FAIL;

	guicolor = GUI_GET_COLOR(color_name);
	اگر (guicolor == INVALCOLOR)
	    واپس FAIL;

	rgb[n] = GUI_MCH_GET_RGB(guicolor);
    }

    اگر (n != 16 || li != تہی)
	واپس FAIL;

    سیٹ_vterm_palette(vterm, rgb);

    واپس OK;
}

/*
 * Initialize the ANSI color palette from g:terminal_ansi_colors[0:15]
 */
    ساکن خالی
init_vterm_ansi_colors(VTerm *vterm)
{
    dictitem_T	*var = ڈھونڈو_var((char_u *)"g:terminal_ansi_colors", تہی, TRUE);

    اگر (var == تہی)
	واپس;

    اگر (var->di_tv.v_type != VAR_LIST
	    || var->di_tv.vval.v_list == تہی
	    || var->di_tv.vval.v_list->lv_پہلا == &range_list_شے
	    || سیٹ_ansi_colors_list(vterm, var->di_tv.vval.v_list) == FAIL)
	semsg(_(e_invalid_دلیل_str), "g:terminal_ansi_colors");
}
#اگر_ختم

/*
 * Handles a "drop" command from the job in the terminal.
 * "item" is the file name, "item->li_next" may have options.
 */
    ساکن خالی
handle_drop_command(listitem_T *شے)
{
    char_u	*fname = tv_get_string(&شے->li_tv);
    listitem_T	*opt_شے = شے->li_اگلا;
    صحیح		bufnr;
    win_T	*wp;
    tabpage_T   *tp;
    exarg_T	ea;
    char_u	*tofree = تہی;

    bufnr = buflist_جوڑو(fname, BLN_LISTED | BLN_NOOPT);
    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	اگر (wp->w_بفر->b_fnum == bufnr)
	{
	    // buffer is in a window already, go there
	    goto_tabpage_win(tp, wp);
	    واپس;
	}
    }

    CLEAR_میدان(ea);

    اگر (opt_شے != تہی && opt_شے->li_tv.v_type == VAR_DICT
					&& opt_شے->li_tv.vval.v_dict != تہی)
    {
	dict_T *dict = opt_شے->li_tv.vval.v_dict;
	char_u *p;

	p = dict_get_string(dict, "ff", FALSE);
	اگر (p == تہی)
	    p = dict_get_string(dict, "fileformat", FALSE);
	اگر (p != تہی)
	{
	    اگر (check_ff_قدر(p) == FAIL)
		ch_log(تہی, "Invalid ff argument to drop: %s", p);
	    ورنہ
		ea.force_ff = *p;
	}
	p = dict_get_string(dict, "enc", FALSE);
	اگر (p == تہی)
	    p = dict_get_string(dict, "encoding", FALSE);
	اگر (p != تہی)
	{
	    ea.cmd = alloc(STRLEN(p) + 12);
	    اگر (ea.cmd != تہی)
	    {
		sprintf((حرف *)ea.cmd, "sbuf ++enc=%s", p);
		ea.force_enc = 11;
		tofree = ea.cmd;
	    }
	}

	p = dict_get_string(dict, "bad", FALSE);
	اگر (p != تہی)
	    get_bad_opt(p, &ea);

	اگر (dict_has_کلید(dict, "bin"))
	    ea.force_bin = FORCE_BIN;
	اگر (dict_has_کلید(dict, "binary"))
	    ea.force_bin = FORCE_BIN;
	اگر (dict_has_کلید(dict, "nobin"))
	    ea.force_bin = FORCE_NOBIN;
	اگر (dict_has_کلید(dict, "nobinary"))
	    ea.force_bin = FORCE_NOBIN;
    }

    // open in new window, like ":split fname"
    اگر (ea.cmd == تہی)
	ea.cmd = (char_u *)"split";
    ea.arg = fname;
    ea.cmdidx = CMD_بانٹو;
    ex_splitview(&ea);

    vim_free(tofree);
}

/*
 * Return TRUE if "func" starts with "pat" and "pat" isn't empty.
 */
    ساکن صحیح
is_permitted_رکن_api(char_u *func, char_u *pat)
{
    واپس pat != تہی && *pat != NUL && STRNICMP(func, pat, STRLEN(pat)) == 0;
}

/*
 * Handles a function call from the job running in a terminal.
 * "item" is the function name, "item->li_next" has the arguments.
 */
    ساکن خالی
handle_call_command(رکن_T *رکن, channel_T *channel, listitem_T *شے)
{
    char_u	*func;
    typval_T	argvars[2];
    typval_T	rettv;
    funcexe_T	funcexe;

    اگر (شے->li_اگلا == تہی)
    {
	ch_log(channel, "Missing function arguments for call");
	واپس;
    }
    func = tv_get_string(&شے->li_tv);

    اگر (!is_permitted_رکن_api(func, رکن->tl_api))
    {
	ch_log(channel, "Unpermitted function: %s", func);
	واپس;
    }

    argvars[0].v_type = VAR_نمبر;
    argvars[0].vval.v_نمبر = رکن->tl_بفر->b_fnum;
    argvars[1] = شے->li_اگلا->li_tv;
    CLEAR_میدان(funcexe);
    funcexe.fe_firstline = 1L;
    funcexe.fe_lastline = 1L;
    funcexe.fe_evaluate = TRUE;
    اگر (call_func(func, -1, &rettv, 2, argvars, &funcexe) == OK)
    {
	clear_tv(&rettv);
	ch_log(channel, "Function %s called", func);
    }
    ورنہ
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
    ساکن size_t
url_decode(ثابت حرف *src, ثابت size_t len, char_u *dst)
{
    size_t i = 0, j = 0;

    جب (i < len)
    {
	اگر (src[i] == '%' && i + 2 < len)
	{
	    dst[j] = hexhex2nr((char_u *)&src[i + 1]);
	    j++;
	    i += 3;
	}
	ورنہ
	{
	    dst[j] = src[i];
	    i++;
	    j++;
	}
    }
    dst[j] = '\0';
    واپس j;
}

/*
 * Sync terminal buffer's cwd with shell's pwd with the help of OSC 7.
 *
 * The OSC 7 sequence has the format of
 * "\033]7;file://HOSTNAME/CURRENT/DIR\033\\"
 * and what VTerm provides via VTermStringFragment is
 * "file://HOSTNAME/CURRENT/DIR"
 */
    ساکن خالی
sync_shell_dir(garray_T *gap)
{
    صحیح       انحراف = 7;  // len of "file://" is 7
    حرف      *pos = (حرف *)gap->ga_ڈیٹا + انحراف;
    char_u    *new_dir;

    // remove HOSTNAME to get PWD
    جب (انحراف < (صحیح)gap->ga_len && *pos != '/' )
    {
	++انحراف;
	++pos;
    }

    اگر (انحراف >= (صحیح)gap->ga_len)
    {
	semsg(_(e_failed_مقصد_نکالو_pwd_from_str_check_your_shell_config),
								 gap->ga_ڈیٹا);
	واپس;
    }

    new_dir = alloc(gap->ga_len - انحراف + 1);
    url_decode(pos, gap->ga_len-انحراف, new_dir);
    changedir_func(new_dir, TRUE, CDSCOPE_کھڑکی);
    vim_free(new_dir);
}

/*
 * Called by libvterm when it cannot recognize an OSC sequence.
 * We recognize a terminal API command.
 */
    ساکن صحیح
parse_osc(صحیح command, VTermStringFragment frag, خالی *user)
{
    رکن_T	*رکن = (رکن_T *)user;
    js_read_T	reader;
    typval_T	tv;
    channel_T	*channel = رکن->tl_کام == تہی ? تہی
						    : رکن->tl_کام->jv_channel;
    garray_T	*gap = &رکن->tl_osc_buf;

    // We recognize only OSC 5 1 ; {command} and OSC 7 ; {command}
    اگر (command != 51 && (command != 7 || !p_asd))
	واپس 0;

    // Concatenate what was received until the final piece is found.
    اگر (ga_grow(gap, (صحیح)frag.len + 1) == FAIL)
    {
	ga_clear(gap);
	واپس 1;
    }
    mch_memmove((حرف *)gap->ga_ڈیٹا + gap->ga_len, frag.str, frag.len);
    gap->ga_len += (صحیح)frag.len;
    اگر (!frag.final)
	واپس 1;

    ((حرف *)gap->ga_ڈیٹا)[gap->ga_len] = 0;

    اگر (command == 7)
    {
	sync_shell_dir(gap);
	ga_clear(gap);
	واپس 1;
    }

    reader.js_buf = gap->ga_ڈیٹا;
    reader.js_fill = تہی;
    reader.js_مستعمل = 0;
    اگر (json_decode(&reader, &tv, 0) == OK
	    && tv.v_type == VAR_LIST
	    && tv.vval.v_list != تہی)
    {
	listitem_T *شے = tv.vval.v_list->lv_پہلا;

	اگر (شے == تہی)
	    ch_log(channel, "Missing command");
	ورنہ
	{
	    char_u	*cmd = tv_get_string(&شے->li_tv);

	    // Make sure an invoked command doesn't delete the buffer (and the
	    // terminal) under our fingers.
	    ++رکن->tl_بفر->b_locked;

	    شے = شے->li_اگلا;
	    اگر (شے == تہی)
		ch_log(channel, "Missing argument for %s", cmd);
	    ورنہ اگر (STRCMP(cmd, "drop") == 0)
		handle_drop_command(شے);
	    ورنہ اگر (STRCMP(cmd, "call") == 0)
		handle_call_command(رکن, channel, شے);
	    ورنہ
		ch_log(channel, "Invalid command received: %s", cmd);
	    --رکن->tl_بفر->b_locked;
	}
    }
    ورنہ
	ch_log(channel, "Invalid JSON received");

    ga_clear(gap);
    clear_tv(&tv);
    واپس 1;
}

/*
 * Called by libvterm when it cannot recognize a CSI sequence.
 * We recognize the window position report.
 */
    ساکن صحیح
parse_csi(
	ثابت حرف  *leader غیر_مستعمل,
	ثابت طویل  دلائل[],
	صحیح	    argcount,
	ثابت حرف  *intermed غیر_مستعمل,
	حرف	    command,
	خالی	    *user)
{
    رکن_T	*رکن = (رکن_T *)user;
    حرف	buf[100];
    صحیح		len;
    صحیح		x = 0;
    صحیح		y = 0;
    win_T	*wp;

    // We recognize only CSI 13 t
    اگر (command != 't' || argcount != 1 || دلائل[0] != 13)
	واپس 0; // not handled

    // When getting the window position is not possible or it fails it results
    // in zero/zero.
#اگر defined(FEAT_GUI) \
	|| (defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)) \
	|| defined(MSWIN)
    (خالی)ui_get_winpos(&x, &y, (varnumber_T)100);
#اگر_ختم

    FOR_ALL_WINDOWS(wp)
	اگر (wp->w_بفر == رکن->tl_بفر)
	    توڑو;
    اگر (wp != تہی)
    {
#اگر_تعریف_شدہ FEAT_GUI
	اگر (gui.اندر_use)
	{
	    x += wp->w_wincol * gui.char_چوڑائی;
	    y += W_WINROW(wp) * gui.char_اونچائی;
	}
	ورنہ
#اگر_ختم
	{
	    // We roughly estimate the position of the terminal window inside
	    // the Vim window by assuming a 10 x 7 character cell.
	    x += wp->w_wincol * 7;
	    y += W_WINROW(wp) * 10;
	}
    }

    len = vim_snprintf(buf, 100, "\x1b[3;%d;%dt", x, y);
    channel_send(رکن->tl_کام->jv_channel, get_tty_part(رکن),
						     (char_u *)buf, len, تہی);
    واپس 1;
}

ساکن VTermStateFallbacks state_fallbacks = {
    تہی,		// control
    parse_csi,		// csi
    parse_osc,		// osc
    تہی,		// dcs
    تہی,		// apc
    تہی,		// pm
    تہی		// sos
};

/*
 * Use Vim's allocation functions for vterm so profiling works.
 */
    ساکن خالی *
vterm_malloc(size_t سائز, خالی *ڈیٹا غیر_مستعمل)
{
    // make sure that the length is not zero
    واپس alloc_clear(سائز == 0 ? 1L : سائز);
}

    ساکن خالی
vterm_memfree(خالی *ptr, خالی *ڈیٹا غیر_مستعمل)
{
    vim_free(ptr);
}

ساکن VTermAllocatorFunctions vterm_allocator = {
  &vterm_malloc,
  &vterm_memfree
};

/*
 * Create a new vterm and initialize it.
 * Return FAIL when out of memory.
 */
    ساکن صحیح
create_vterm(رکن_T *رکن, صحیح صفیں_2, صحیح ستون_تعداد)
{
    VTerm	    *vterm;
    VTermScreen	    *screen;
    VTermState	    *state;
    VTerm_قدر	    قدر;

    vterm = vterm_new_with_allocator(صفیں_2, ستون_تعداد, &vterm_allocator, تہی);
    رکن->tl_vterm = vterm;
    اگر (vterm == تہی)
	واپس FAIL;

    // Allocate screen and state here, so we can bail out if that fails.
    state = vterm_obtain_state(vterm);
    screen = vterm_obtain_screen(vterm);
    اگر (state == تہی || screen == تہی)
    {
	vterm_free(vterm);
	واپس FAIL;
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

    اگر (t_colors < 16)
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
#اگر_تعریف_شدہ MSWIN
    اگر (Get_Caret_Blink_وقت() == INFINITE)
	قدر.boolean = 0;
    ورنہ
	قدر.boolean = 1;
#ورنہ
    قدر.boolean = 0;
#اگر_ختم
    vterm_state_سیٹ_termprop(state, VTERM_PROP_CURSORBLINK, &قدر);
    vterm_state_سیٹ_unrecognised_fallbacks(state, &state_fallbacks, رکن);

    واپس OK;
}

/*
 * Reset the terminal palette to its default value.
 */
    ساکن خالی
رکن_reset_palette(VTerm *vterm)
{
    VTermState	*state = vterm_obtain_state(vterm);
    صحیح		اشاریہ;

    برائے (اشاریہ = 0; اشاریہ < 16; اشاریہ++)
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

    ساکن خالی
رکن_update_palette(رکن_T *رکن)
{
#اگر defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    اگر (رکن_use_palette()
	    && (رکن->tl_palette != تہی
		|| ڈھونڈو_var((char_u *)"g:terminal_ansi_colors", تہی, TRUE)
								      != تہی))
    {
	اگر (رکن->tl_palette != تہی)
	    سیٹ_vterm_palette(رکن->tl_vterm, رکن->tl_palette);
	ورنہ
	    init_vterm_ansi_colors(رکن->tl_vterm);
    }
    ورنہ
#اگر_ختم
	رکن_reset_palette(رکن->tl_vterm);
}

/*
 * Called when option 'termguicolors' is changed.
 */
    خالی
رکن_update_palette_all(خالی)
{
    رکن_T *رکن;

    FOR_ALL_TERMS(رکن)
    {
	اگر (رکن->tl_vterm == تہی)
	    جاری;
	رکن_update_palette(رکن);
    }
}

/*
 * Called when option 'background' or 'termguicolors' was set,
 * or when any highlight is changed.
 */
    خالی
رکن_update_colors_all(خالی)
{
    رکن_T *رکن;

    FOR_ALL_TERMS(رکن)
    {
	اگر (رکن->tl_vterm == تہی)
	    جاری;
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
    اگر (رکن->tl_حالت_متن != تہی)
	واپس رکن->tl_حالت_متن;

    char_u *txt;
    size_t len;
    char_u *fname;

    اگر (رکن->tl_معمول_منوال)
    {
	اگر (رکن_کام_running(رکن))
	    txt = (char_u *)_("Terminal");
	ورنہ
	    txt = (char_u *)_("Terminal-finished");
    }
    ورنہ اگر (رکن->tl_title != تہی)
	txt = رکن->tl_title;
    ورنہ اگر (رکن_none_open(رکن))
	txt = (char_u *)_("active");
    ورنہ اگر (رکن_کام_running(رکن))
	txt = (char_u *)_("running");
    ورنہ
	txt = (char_u *)_("finished");
    fname = buf_get_fname(رکن->tl_بفر);
    len = 9 + STRLEN(fname) + STRLEN(txt);
    رکن->tl_حالت_متن = alloc(len);
    اگر (رکن->tl_حالت_متن != تہی)
	vim_snprintf((حرف *)رکن->tl_حالت_متن, len, "%s [%s]",
		fname, txt);
    واپس رکن->tl_حالت_متن;
}

/*
 * Clear the cached value of the status text.
 */
    خالی
رکن_clear_حالت_متن(رکن_T *رکن)
{
    VIM_CLEAR(رکن->tl_حالت_متن);
}

/*
 * Mark references in jobs of terminals.
 */
    صحیح
سیٹ_ref_اندر_رکن(صحیح copyID)
{
    صحیح		abort = FALSE;
    رکن_T	*رکن;
    typval_T	tv;

    برائے (رکن = پہلا_رکن; !abort && رکن != تہی; رکن = رکن->tl_اگلا)
	اگر (رکن->tl_کام != تہی)
	{
	    tv.v_type = VAR_کام;
	    tv.vval.v_کام = رکن->tl_کام;
	    abort = abort || سیٹ_ref_اندر_شے(&tv, copyID, تہی, تہی, تہی);
	}
    واپس abort;
}

/*
 * Get the buffer from the first argument in "argvars".
 * Returns NULL when the buffer is not for a terminal window and logs a message
 * with "where".
 */
    ساکن buf_T *
رکن_get_buf(typval_T *argvars, حرف *where)
{
    buf_T *buf;

    ++emsg_off;
    buf = tv_get_buf(&argvars[0], FALSE);
    --emsg_off;
    اگر (buf == تہی || buf->b_رکن == تہی)
    {
	(خالی)tv_get_نمبر(&argvars[0]);    // issue errmsg if type error
	ch_log(تہی, "%s: invalid buffer argument", where);
	واپس تہی;
    }
    واپس buf;
}

    ساکن خالی
clear_cell(VTermScreenCell *cell)
{
    CLEAR_میدان(*cell);
    cell->fg.type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_FG;
    cell->bg.type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_BG;
}

    ساکن خالی
dump_رکن_color(FILE *fd, VTermColor *color)
{
    صحیح اشاریہ;

    اگر (VTERM_COLOR_IS_اشاریہ_دار(color))
	اشاریہ = color->اشاریہ + 1;
    ورنہ اگر (color->type == 0)
	// use RGB values
	اشاریہ = 255;
    ورنہ
	// default color
	اشاریہ = 0;
    fprintf(fd, "%02x%02x%02x%d",
	    (صحیح)color->red, (صحیح)color->green, (صحیح)color->blue,
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
    خالی
f_رکن_dumpwrite(typval_T *argvars, typval_T *rettv غیر_مستعمل)
{
    buf_T	*buf;
    رکن_T	*رکن;
    char_u	*fname;
    صحیح		max_اونچائی_2 = 0;
    صحیح		max_چوڑائی_2 = 0;
    stat_T	st;
    FILE	*fd;
    VTermPos	pos;
    VTermScreen *screen;
    VTermScreenCell پچھلا_cell;
    VTermState	*state;
    VTermPos	cursor_pos;

    اگر (check_restricted() || check_secure())
	واپس;

    اگر (اندر_vim9script()
	    && (check_for_بفر_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	واپس;

    buf = رکن_get_buf(argvars, "term_dumpwrite()");
    اگر (buf == تہی)
	واپس;
    رکن = buf->b_رکن;
    اگر (رکن->tl_vterm == تہی)
    {
	emsg(_(e_کام_already_ختم_شدہ));
	واپس;
    }

    اگر (argvars[2].v_type != VAR_UNKNOWN)
    {
	dict_T *d;

	اگر (check_for_dict_arg(argvars, 2) == FAIL)
	    واپس;
	d = argvars[2].vval.v_dict;
	اگر (d != تہی)
	{
	    max_اونچائی_2 = dict_get_نمبر(d, "rows");
	    max_چوڑائی_2 = dict_get_نمبر(d, "columns");
	}
    }

    fname = tv_get_string_chk(&argvars[1]);
    اگر (fname == تہی)
	واپس;
    اگر (mch_stat((حرف *)fname, &st) >= 0)
    {
	semsg(_(e_file_exists_str), fname);
	واپس;
    }

    اگر (*fname == NUL || (fd = mch_fopen((حرف *)fname, WRITEBIN)) == تہی)
    {
	semsg(_(e_cant_create_file_str), *fname == NUL ? (char_u *)_("<empty>") : fname);
	واپس;
    }

    clear_cell(&پچھلا_cell);

    screen = vterm_obtain_screen(رکن->tl_vterm);
    state = vterm_obtain_state(رکن->tl_vterm);
    vterm_state_get_cursorpos(state, &cursor_pos);

    برائے (pos.صف = 0; (max_اونچائی_2 == 0 || pos.صف < max_اونچائی_2)
					 && pos.صف < رکن->tl_صفیں; ++pos.صف)
    {
	صحیح		repeat = 0;

	برائے (pos.ستون = 0; (max_چوڑائی_2 == 0 || pos.ستون < max_چوڑائی_2)
					 && pos.ستون < رکن->tl_ستون_تعداد; ++pos.ستون)
	{
	    VTermScreenCell cell;
	    صحیح		    same_attr;
	    صحیح		    same_chars = TRUE;
	    صحیح		    i;
	    صحیح		    is_cursor_pos = (pos.ستون == cursor_pos.ستون
						 && pos.صف == cursor_pos.صف);

	    اگر (vterm_screen_get_cell(screen, pos, &cell) == 0)
		clear_cell(&cell);

	    برائے (i = 0; i < VTERM_MAX_CHARS_فی_CELL; ++i)
	    {
		صحیح c = cell.chars[i];
		صحیح pc = پچھلا_cell.chars[i];
		صحیح should_break = c == NUL || pc == NUL;

		// For the first character NUL is the same as space.
		اگر (i == 0)
		{
		    c = (c == NUL) ? ' ' : c;
		    pc = (pc == NUL) ? ' ' : pc;
		}
		اگر (c != pc)
		    same_chars = FALSE;
		اگر (should_break)
		    توڑو;
	    }
	    same_attr = vtermAttr2hl(&cell.attrs)
					      == vtermAttr2hl(&پچھلا_cell.attrs)
			&& vterm_color_is_equal(&cell.fg, &پچھلا_cell.fg)
			&& vterm_color_is_equal(&cell.bg, &پچھلا_cell.bg);
	    اگر (same_chars && cell.چوڑائی == پچھلا_cell.چوڑائی && same_attr
							     && !is_cursor_pos)
	    {
		++repeat;
	    }
	    ورنہ
	    {
		اگر (repeat > 0)
		{
		    fprintf(fd, "@%d", repeat);
		    repeat = 0;
		}
		fputs(is_cursor_pos ? ">" : "|", fd);

		اگر (cell.chars[0] == NUL)
		    fputs(" ", fd);
		ورنہ
		{
		    char_u	charbuf[10];
		    صحیح		len;

		    برائے (i = 0; i < VTERM_MAX_CHARS_فی_CELL
						  && cell.chars[i] != NUL; ++i)
		    {
			len = utf_char2bytes(cell.chars[i], charbuf);
			fwrite(charbuf, len, 1, fd);
		    }
		}

		// When only the characters differ we don't write anything, the
		// following "|", "@" or NL will indicate using the same
		// attributes.
		اگر (cell.چوڑائی != پچھلا_cell.چوڑائی || !same_attr)
		{
		    اگر (cell.چوڑائی == 2)
			fputs("*", fd);
		    ورنہ
			fputs("+", fd);

		    اگر (same_attr)
		    {
			fputs("&", fd);
		    }
		    ورنہ
		    {
			fprintf(fd, "%d", vtermAttr2hl(&cell.attrs));
			اگر (vterm_color_is_equal(&cell.fg, &پچھلا_cell.fg))
			    fputs("&", fd);
			ورنہ
			{
			    fputs("#", fd);
			    dump_رکن_color(fd, &cell.fg);
			}
			اگر (vterm_color_is_equal(&cell.bg, &پچھلا_cell.bg))
			    fputs("&", fd);
			ورنہ
			{
			    fputs("#", fd);
			    dump_رکن_color(fd, &cell.bg);
			}
		    }
		}

		پچھلا_cell = cell;
	    }

	    اگر (cell.چوڑائی == 2)
		++pos.ستون;
	}
	اگر (repeat > 0)
	    fprintf(fd, "@%d", repeat);
	fputs("\n", fd);
    }

    fclose(fd);
}

/*
 * Called when a dump is corrupted.  Put a breakpoint here when debugging.
 */
    ساکن خالی
dump_is_corrupt(garray_T *gap)
{
    GA_CONCAT_LITERAL(gap, "CORRUPT");
}

    ساکن خالی
append_cell(garray_T *gap, cellattr_T *cell)
{
    اگر (ga_grow(gap, 1) == FAIL)
	واپس;

    *(((cellattr_T *)gap->ga_ڈیٹا) + gap->ga_len) = *cell;
    ++gap->ga_len;
}

    ساکن خالی
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
    ساکن صحیح
read_dump_file(FILE *fd, VTermPos *cursor_pos)
{
    صحیح		    c;
    garray_T	    ga_متن;
    garray_T	    ga_cell;
    string_T	    پچھلا_char = {تہی, 0};
    صحیح		    attr = 0;
    cellattr_T	    cell;
    cellattr_T	    خالی_خانے_cell;
    رکن_T	    *رکن = curbuf->b_رکن;
    صحیح		    max_cells = 0;
    صحیح		    آغاز_صف = رکن->tl_scrollback.ga_len;

    ga_init2(&ga_متن, 1, 90);
    ga_init2(&ga_cell, حجم(cellattr_T), 90);
    clear_cellattr(&cell);
    clear_cellattr(&خالی_خانے_cell);
    cursor_pos->صف = -1;
    cursor_pos->ستون = -1;

    c = fgetc(fd);
    برائے (;;)
    {
	اگر (c == EOF)
	    توڑو;
	اگر (c == '\r')
	{
	    // DOS line endings?  Ignore.
	    c = fgetc(fd);
	}
	ورنہ اگر (c == '\n')
	{
	    // End of a line: append it to the buffer.
	    اگر (ga_متن.ga_ڈیٹا == تہی)
		dump_is_corrupt(&ga_متن);
	    اگر (ga_grow(&رکن->tl_scrollback, 1) == OK)
	    {
		sb_سطر_T   *سطر = (sb_سطر_T *)رکن->tl_scrollback.ga_ڈیٹا
						  + رکن->tl_scrollback.ga_len;

		اگر (max_cells < ga_cell.ga_len)
		    max_cells = ga_cell.ga_len;
		سطر->sb_ستون_تعداد = ga_cell.ga_len;
		سطر->sb_بائٹیں = ga_متن.ga_len;
		سطر->sb_cells = ga_cell.ga_ڈیٹا;
		سطر->sb_fill_attr = رکن->tl_default_color;
		سطر->continuation = 0;
		++رکن->tl_scrollback.ga_len;
		ga_init(&ga_cell);

		ga_append(&ga_متن, NUL);
		ml_append(curbuf->b_ml.ml_سطر_گنتی, ga_متن.ga_ڈیٹا,
							ga_متن.ga_len, FALSE);
	    }
	    ورنہ
		ga_clear(&ga_cell);
	    ga_متن.ga_len = 0;

	    c = fgetc(fd);
	}
	ورنہ اگر (c == '|' || c == '>')
	{
	    صحیح پچھلا_len = ga_متن.ga_len;

	    اگر (c == '>')
	    {
		اگر (cursor_pos->صف != -1)
		    dump_is_corrupt(&ga_متن);	// duplicate cursor
		cursor_pos->صف = رکن->tl_scrollback.ga_len - آغاز_صف;
		cursor_pos->ستون = ga_cell.ga_len;
	    }

	    // normal character(s) followed by "+", "*", "|", "@" or NL
	    c = fgetc(fd);
	    اگر (c != EOF)
		ga_append(&ga_متن, c);
	    برائے (;;)
	    {
		c = fgetc(fd);
		اگر (c == '+' || c == '*' || c == '|' || c == '>' || c == '@'
						      || c == EOF || c == '\n')
		    توڑو;
		ga_append(&ga_متن, c);
	    }

	    // save the character for repeating it
	    VIM_CLEAR_STRING(پچھلا_char);
	    اگر (ga_متن.ga_ڈیٹا != تہی)
	    {
		پچھلا_char.لمبائی = (size_t)(ga_متن.ga_len - پچھلا_len);
		پچھلا_char.string = vim_strnsave(
		    ((char_u *)ga_متن.ga_ڈیٹا) + پچھلا_len,
		    پچھلا_char.لمبائی);
		اگر (پچھلا_char.string == تہی)
		    پچھلا_char.لمبائی = 0;
	    }

	    اگر (c == '@' || c == '|' || c == '>' || c == '\n')
	    {
		// use all attributes from previous cell
	    }
	    ورنہ اگر (c == '+' || c == '*')
	    {
		صحیح is_bg;

		cell.چوڑائی = c == '+' ? 1 : 2;

		c = fgetc(fd);
		اگر (c == '&')
		{
		    // use same attr as previous cell
		    c = fgetc(fd);
		}
		ورنہ اگر (محفوظ_isdigit(c))
		{
		    // get the decimal attribute
		    attr = 0;
		    جب (محفوظ_isdigit(c))
		    {
			attr = attr * 10 + (c - '0');
			c = fgetc(fd);
		    }
		    hl2vtermAttr(attr, &cell);

		    // is_bg == 0: fg, is_bg == 1: bg
		    برائے (is_bg = 0; is_bg <= 1; ++is_bg)
		    {
			اگر (c == '&')
			{
			    // use same color as previous cell
			    c = fgetc(fd);
			}
			ورنہ اگر (c == '#')
			{
			    صحیح red, green, blue, اشاریہ = 0, type;

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
			    اگر (!محفوظ_isdigit(c))
				dump_is_corrupt(&ga_متن);
			    جب (محفوظ_isdigit(c))
			    {
				اشاریہ = اشاریہ * 10 + (c - '0');
				c = fgetc(fd);
			    }
			    اگر (اشاریہ == 0 || اشاریہ == 255)
			    {
				type = VTERM_COLOR_RGB;
				اگر (اشاریہ == 0)
				{
				    اگر (is_bg)
					type |= VTERM_COLOR_DEFAULT_BG;
				    ورنہ
					type |= VTERM_COLOR_DEFAULT_FG;
				}
			    }
			    ورنہ
			    {
				type = VTERM_COLOR_اشاریہ_دار;
				اشاریہ -= 1;
			    }
			    اگر (is_bg)
			    {
				cell.bg.type = type;
				cell.bg.red = red;
				cell.bg.green = green;
				cell.bg.blue = blue;
				cell.bg.اشاریہ = اشاریہ;
			    }
			    ورنہ
			    {
				cell.fg.type = type;
				cell.fg.red = red;
				cell.fg.green = green;
				cell.fg.blue = blue;
				cell.fg.اشاریہ = اشاریہ;
			    }
			}
			ورنہ
			    dump_is_corrupt(&ga_متن);
		    }
		}
		ورنہ
		    dump_is_corrupt(&ga_متن);
	    }
	    ورنہ
		dump_is_corrupt(&ga_متن);

	    append_cell(&ga_cell, &cell);
	    اگر (cell.چوڑائی == 2)
		append_cell(&ga_cell, &خالی_خانے_cell);
	}
	ورنہ اگر (c == '@')
	{
	    اگر (پچھلا_char.string == تہی)
		dump_is_corrupt(&ga_متن);
	    ورنہ
	    {
		صحیح گنتی = 0;

		// repeat previous character, get the count
		برائے (;;)
		{
		    c = fgetc(fd);
		    اگر (!محفوظ_isdigit(c))
			توڑو;
		    گنتی = گنتی * 10 + (c - '0');
		}

		جب (گنتی-- > 0)
		{
		    ga_concat_len(&ga_متن, پچھلا_char.string, پچھلا_char.لمبائی);
		    append_cell(&ga_cell, &cell);
		}
	    }
	}
	ورنہ
	{
	    dump_is_corrupt(&ga_متن);
	    c = fgetc(fd);
	}
    }

    اگر (ga_متن.ga_len > 0)
    {
	// trailing characters after last NL
	dump_is_corrupt(&ga_متن);
	ga_append(&ga_متن, NUL);
	ml_append(curbuf->b_ml.ml_سطر_گنتی, ga_متن.ga_ڈیٹا,
							ga_متن.ga_len, FALSE);
    }

    ga_clear(&ga_متن);
    ga_clear(&ga_cell);
    vim_free(پچھلا_char.string);

    واپس max_cells;
}

/*
 * Return an allocated string with at least "text_width" "=" characters and
 * "fname" inserted in the middle.
 */
    ساکن char_u *
get_separator(صحیح متن_چوڑائی, char_u *fname)
{
    صحیح	    چوڑائی = MAX(متن_چوڑائی, curwin->w_چوڑائی);
    char_u  *textline;
    صحیح	    fname_سائز;
    char_u  *p = fname;
    صحیح	    i;
    size_t  off;

    textline = alloc(چوڑائی + STRLEN(fname) + 1);
    اگر (textline == تہی)
	واپس تہی;

    fname_سائز = vim_strsize(fname);
    اگر (fname_سائز < چوڑائی - 8)
    {
	// enough room, don't use the full window width
	چوڑائی = MAX(متن_چوڑائی, fname_سائز + 8);
    }
    ورنہ اگر (fname_سائز > چوڑائی - 8)
    {
	// full name doesn't fit, use only the tail
	p = gettail(fname);
	fname_سائز = vim_strsize(p);
    }
    // skip characters until the name fits
    جب (fname_سائز > چوڑائی - 8)
    {
	p += (*mb_ptr2len)(p);
	fname_سائز = vim_strsize(p);
    }

    برائے (i = 0; i < (چوڑائی - fname_سائز) / 2 - 1; ++i)
	textline[i] = '=';
    textline[i++] = ' ';

    STRCPY(textline + i, p);
    off = STRLEN(textline);
    textline[off] = ' ';
    برائے (i = 1; i < (چوڑائی - fname_سائز) / 2; ++i)
	textline[off + i] = '=';
    textline[off + i] = NUL;

    واپس textline;
}

/*
 * Common for "term_dumpdiff()" and "term_dumpload()".
 */
    ساکن خالی
رکن_بھرو_dump(typval_T *argvars, typval_T *rettv, صحیح do_diff)
{
    jobopt_T	opt;
    buf_T	*buf = تہی;
    char_u	buf1[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    char_u	*fname1;
    char_u	*fname2 = تہی;
    char_u	*fname_tofree = تہی;
    FILE	*fd1;
    FILE	*fd2 = تہی;
    char_u	*textline = تہی;

    // First open the files.  If this fails bail out.
    fname1 = tv_get_string_buf_chk(&argvars[0], buf1);
    اگر (do_diff)
	fname2 = tv_get_string_buf_chk(&argvars[1], buf2);
    اگر (fname1 == تہی || (do_diff && fname2 == تہی))
    {
	emsg(_(e_invalid_دلیل));
	واپس;
    }
    fd1 = mch_fopen((حرف *)fname1, READBIN);
    اگر (fd1 == تہی)
    {
	semsg(_(e_cant_read_file_str), fname1);
	واپس;
    }
    اگر (do_diff)
    {
	fd2 = mch_fopen((حرف *)fname2, READBIN);
	اگر (fd2 == تہی)
	{
	    fclose(fd1);
	    semsg(_(e_cant_read_file_str), fname2);
	    واپس;
	}
    }

    init_کام_options(&opt);
    اگر (argvars[do_diff ? 2 : 1].v_type != VAR_UNKNOWN
	    && get_کام_options(&argvars[do_diff ? 2 : 1], &opt, 0,
		    JO2_رکن_NAME + JO2_رکن_ستون_تعداد + JO2_رکن_صفیں
		    + JO2_VERTICAL + JO2_CURWIN + JO2_NORESTORE) == FAIL)
	جاؤ theend;

    اگر (opt.jo_رکن_name == تہی)
    {
	size_t len = STRLEN(fname1) + 12;

	fname_tofree = alloc(len);
	اگر (fname_tofree != تہی)
	{
	    vim_snprintf((حرف *)fname_tofree, len, "dump diff %s", fname1);
	    opt.jo_رکن_name = fname_tofree;
	}
    }

    اگر (opt.jo_bufnr_buf != تہی)
    {
	win_T *wp = buf_jump_open_win(opt.jo_bufnr_buf);

	// With "bufnr" argument: enter the window with this buffer and make it
	// empty.
	اگر (wp == تہی)
	    semsg(_(e_invalid_دلیل_str), "bufnr");
	ورنہ
	{
	    buf = curbuf;
	    جب (!(curbuf->b_ml.ml_flags & ML_خالی_خانے))
		ml_delete((linenr_T)1);
	    free_scrollback(curbuf->b_رکن);
	    redraw_later(UPD_NOT_VALID);
	}
    }
    ورنہ
	// Create a new terminal window.
	buf = رکن_آغاز(&argvars[0], تہی, &opt, رکن_آغاز_NOJOB);

    اگر (buf != تہی && buf->b_رکن != تہی)
    {
	صحیح		i;
	linenr_T	bot_lnum;
	linenr_T	lnum;
	رکن_T		*رکن = buf->b_رکن;
	صحیح		چوڑائی;
	صحیح		width2;
	VTermPos	cursor_pos1;
	VTermPos	cursor_pos2;

	رکن_init_default_colors(رکن);

	rettv->vval.v_نمبر = buf->b_fnum;

	// read the files, fill the buffer with the diff
	چوڑائی = read_dump_file(fd1, &cursor_pos1);

	// position the cursor
	اگر (cursor_pos1.صف >= 0)
	{
	    curwin->w_cursor.lnum = cursor_pos1.صف + 1;
	    coladvance(cursor_pos1.ستون);
	}

	// Delete the empty line that was in the empty buffer.
	ml_delete(1);

	// For term_dumpload() we are done here.
	اگر (!do_diff)
	    جاؤ theend;

	رکن->tl_چوٹی_diff_صفیں = curbuf->b_ml.ml_سطر_گنتی;

	textline = get_separator(چوڑائی, fname1);
	اگر (textline == تہی)
	    جاؤ theend;
	اگر (جوڑو_خالی_خانے_scrollback(رکن, &رکن->tl_default_color, 0) == OK)
	    ml_append(curbuf->b_ml.ml_سطر_گنتی, textline, 0, FALSE);
	vim_free(textline);

	textline = get_separator(چوڑائی, fname2);
	اگر (textline == تہی)
	    جاؤ theend;
	اگر (جوڑو_خالی_خانے_scrollback(رکن, &رکن->tl_default_color, 0) == OK)
	    ml_append(curbuf->b_ml.ml_سطر_گنتی, textline, 0, FALSE);
	textline[چوڑائی] = NUL;

	bot_lnum = curbuf->b_ml.ml_سطر_گنتی;
	width2 = read_dump_file(fd2, &cursor_pos2);
	اگر (width2 > چوڑائی)
	{
	    vim_free(textline);
	    textline = alloc(width2 + 1);
	    اگر (textline == تہی)
		جاؤ theend;
	    چوڑائی = width2;
	    textline[چوڑائی] = NUL;
	}
	رکن->tl_bot_diff_صفیں = curbuf->b_ml.ml_سطر_گنتی - bot_lnum;

	برائے (lnum = 1; lnum <= رکن->tl_چوٹی_diff_صفیں; ++lnum)
	{
	    اگر (lnum + bot_lnum > curbuf->b_ml.ml_سطر_گنتی)
	    {
		// bottom part has fewer rows, fill with "-"
		برائے (i = 0; i < چوڑائی; ++i)
		    textline[i] = '-';
	    }
	    ورنہ
	    {
		char_u *line1;
		char_u *line2;
		char_u *p1;
		char_u *p2;
		صحیح	ستون;
		sb_سطر_T   *sb_سطر = (sb_سطر_T *)رکن->tl_scrollback.ga_ڈیٹا;
		cellattr_T *cellattr1 = (sb_سطر + lnum - 1)->sb_cells;
		cellattr_T *cellattr2 = (sb_سطر + lnum + bot_lnum - 1)
								    ->sb_cells;

		// Make a copy, getting the second line will invalidate it.
		line1 = vim_strsave(ml_get(lnum));
		اگر (line1 == تہی)
		    توڑو;
		p1 = line1;

		line2 = ml_get(lnum + bot_lnum);
		p2 = line2;
		برائے (ستون = 0; ستون < چوڑائی && *p1 != NUL && *p2 != NUL; ++ستون)
		{
		    صحیح len1 = utfc_ptr2len(p1);
		    صحیح len2 = utfc_ptr2len(p2);

		    textline[ستون] = ' ';
		    اگر (len1 != len2 || STRNCMP(p1, p2, len1) != 0)
			// text differs
			textline[ستون] = 'X';
		    ورنہ اگر (lnum == cursor_pos1.صف + 1
			    && ستون == cursor_pos1.ستون
			    && (cursor_pos1.صف != cursor_pos2.صف
					|| cursor_pos1.ستون != cursor_pos2.ستون))
			// cursor in first but not in second
			textline[ستون] = '>';
		    ورنہ اگر (lnum == cursor_pos2.صف + 1
			    && ستون == cursor_pos2.ستون
			    && (cursor_pos1.صف != cursor_pos2.صف
					|| cursor_pos1.ستون != cursor_pos2.ستون))
			// cursor in second but not in first
			textline[ستون] = '<';
		    ورنہ اگر (cellattr1 != تہی && cellattr2 != تہی)
		    {
			اگر ((cellattr1 + ستون)->چوڑائی
						   != (cellattr2 + ستون)->چوڑائی)
			    textline[ستون] = 'w';
			ورنہ اگر (!vterm_color_is_equal(&(cellattr1 + ستون)->fg,
						   &(cellattr2 + ستون)->fg))
			    textline[ستون] = 'f';
			ورنہ اگر (!vterm_color_is_equal(&(cellattr1 + ستون)->bg,
						   &(cellattr2 + ستون)->bg))
			    textline[ستون] = 'b';
			ورنہ اگر (vtermAttr2hl(&(cellattr1 + ستون)->attrs)
				  != vtermAttr2hl(&((cellattr2 + ستون)->attrs)))
			    textline[ستون] = 'a';
		    }
		    p1 += len1;
		    p2 += len2;
		    // TODO: handle different width
		}

		جب (ستون < چوڑائی)
		{
		    اگر (*p1 == NUL && *p2 == NUL)
			textline[ستون] = '?';
		    ورنہ اگر (*p1 == NUL)
		    {
			textline[ستون] = '+';
			p2 += utfc_ptr2len(p2);
		    }
		    ورنہ
		    {
			textline[ستون] = '-';
			p1 += utfc_ptr2len(p1);
		    }
		    ++ستون;
		}

		vim_free(line1);
	    }
	    اگر (جوڑو_خالی_خانے_scrollback(رکن, &رکن->tl_default_color,
						 رکن->tl_چوٹی_diff_صفیں) == OK)
		ml_append(رکن->tl_چوٹی_diff_صفیں + lnum, textline, 0, FALSE);
	    ++bot_lnum;
	}

	جب (lnum + bot_lnum <= curbuf->b_ml.ml_سطر_گنتی)
	{
	    // bottom part has more rows, fill with "+"
	    برائے (i = 0; i < چوڑائی; ++i)
		textline[i] = '+';
	    اگر (جوڑو_خالی_خانے_scrollback(رکن, &رکن->tl_default_color,
						 رکن->tl_چوٹی_diff_صفیں) == OK)
		ml_append(رکن->tl_چوٹی_diff_صفیں + lnum, textline, 0, FALSE);
	    ++lnum;
	    ++bot_lnum;
	}

	رکن->tl_ستون_تعداد = چوڑائی;

	// looks better without wrapping
	curwin->w_p_wrap = 0;
    }

theend:
    vim_free(textline);
    vim_free(fname_tofree);
    fclose(fd1);
    اگر (fd2 != تہی)
	fclose(fd2);
}

/*
 * If the current buffer shows the output of term_dumpdiff(), swap the top and
 * bottom files.
 * Return FAIL when this is not possible.
 */
    صحیح
رکن_تبادلہ_diff(خالی)
{
    رکن_T	*رکن = curbuf->b_رکن;
    linenr_T	سطر_گنتی;
    linenr_T	چوٹی_صفیں;
    linenr_T	bot_صفیں;
    linenr_T	bot_آغاز;
    linenr_T	lnum;
    char_u	*p;
    sb_سطر_T	*sb_سطر;

    اگر (رکن == تہی
	    || !رکن_is_ختم_شدہ(curbuf)
	    || رکن->tl_چوٹی_diff_صفیں == 0
	    || رکن->tl_scrollback.ga_len == 0)
	واپس FAIL;

    سطر_گنتی = curbuf->b_ml.ml_سطر_گنتی;
    چوٹی_صفیں = رکن->tl_چوٹی_diff_صفیں;
    bot_صفیں = رکن->tl_bot_diff_صفیں;
    bot_آغاز = سطر_گنتی - bot_صفیں;
    sb_سطر = (sb_سطر_T *)رکن->tl_scrollback.ga_ڈیٹا;

    // move lines from top to above the bottom part
    برائے (lnum = 1; lnum <= چوٹی_صفیں; ++lnum)
    {
	p = vim_strsave(ml_get(1));
	اگر (p == تہی)
	    واپس OK;
	ml_append(bot_آغاز, p, 0, FALSE);
	ml_delete(1);
	vim_free(p);
    }

    // move lines from bottom to the top
    برائے (lnum = 1; lnum <= bot_صفیں; ++lnum)
    {
	p = vim_strsave(ml_get(bot_آغاز + lnum));
	اگر (p == تہی)
	    واپس OK;
	ml_delete(bot_آغاز + lnum);
	ml_append(lnum - 1, p, 0, FALSE);
	vim_free(p);
    }

    // move top title to bottom
    p = vim_strsave(ml_get(bot_صفیں + 1));
    اگر (p == تہی)
	واپس OK;
    ml_append(سطر_گنتی - چوٹی_صفیں - 1, p, 0, FALSE);
    ml_delete(bot_صفیں + 1);
    vim_free(p);

    // move bottom title to top
    p = vim_strsave(ml_get(سطر_گنتی - چوٹی_صفیں));
    اگر (p == تہی)
	واپس OK;
    ml_delete(سطر_گنتی - چوٹی_صفیں);
    ml_append(bot_صفیں, p, 0, FALSE);
    vim_free(p);

    اگر (چوٹی_صفیں == bot_صفیں)
    {
	// rows counts are equal, can swap cell properties
	برائے (lnum = 0; lnum < چوٹی_صفیں; ++lnum)
	{
	    sb_سطر_T	عارضی;

	    عارضی = *(sb_سطر + lnum);
	    *(sb_سطر + lnum) = *(sb_سطر + bot_آغاز + lnum);
	    *(sb_سطر + bot_آغاز + lnum) = عارضی;
	}
    }
    ورنہ
    {
	size_t		سائز = حجم(sb_سطر_T) * رکن->tl_scrollback.ga_len;
	sb_سطر_T	*عارضی = alloc(سائز);

	// need to copy cell properties into temp memory
	اگر (عارضی != تہی)
	{
	    mch_memmove(عارضی, رکن->tl_scrollback.ga_ڈیٹا, سائز);
	    mch_memmove(رکن->tl_scrollback.ga_ڈیٹا,
		    عارضی + bot_آغاز,
		    حجم(sb_سطر_T) * bot_صفیں);
	    mch_memmove((sb_سطر_T *)رکن->tl_scrollback.ga_ڈیٹا + bot_صفیں,
		    عارضی + چوٹی_صفیں,
		    حجم(sb_سطر_T) * (سطر_گنتی - چوٹی_صفیں - bot_صفیں));
	    mch_memmove((sb_سطر_T *)رکن->tl_scrollback.ga_ڈیٹا
						       + سطر_گنتی - چوٹی_صفیں,
		    عارضی,
		    حجم(sb_سطر_T) * چوٹی_صفیں);
	    vim_free(عارضی);
	}
    }

    رکن->tl_چوٹی_diff_صفیں = bot_صفیں;
    رکن->tl_bot_diff_صفیں = چوٹی_صفیں;

    update_screen(UPD_NOT_VALID);
    واپس OK;
}

/*
 * "term_dumpdiff(filename, filename, options)" function
 */
    خالی
f_رکن_dumpdiff(typval_T *argvars, typval_T *rettv)
{
    اگر (اندر_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	واپس;

    رکن_بھرو_dump(argvars, rettv, TRUE);
}

/*
 * "term_dumpload(filename, options)" function
 */
    خالی
f_رکن_dumpload(typval_T *argvars, typval_T *rettv)
{
    اگر (اندر_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	واپس;

    رکن_بھرو_dump(argvars, rettv, FALSE);
}

/*
 * "term_getaltscreen(buf)" function
 */
    خالی
f_رکن_getaltscreen(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    اگر (اندر_vim9script() && check_for_بفر_arg(argvars, 0) == FAIL)
	واپس;

    buf = رکن_get_buf(argvars, "term_getaltscreen()");
    اگر (buf == تہی)
	واپس;
    rettv->vval.v_نمبر = buf->b_رکن->tl_using_altscreen;
}

/*
 * "term_getattr(attr, name)" function
 */
    خالی
f_رکن_getattr(typval_T *argvars, typval_T *rettv)
{
    صحیح	    attr;
    size_t  i;
    char_u  *name;

    ساکن ساخت {
	حرف	    *name;
	صحیح	    attr;
    } attrs[] = {
	{"bold",      HL_BOLD},
	{"italic",    HL_ITALIC},
	{"underline", HL_UNDERLINE},
	{"strike",    HL_STRIKETHROUGH},
	{"reverse",   HL_INVERSE},
    };

    اگر (اندر_vim9script()
	    && (check_for_نمبر_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	واپس;

    attr = tv_get_نمبر(&argvars[0]);
    name = tv_get_string_chk(&argvars[1]);
    اگر (name == تہی)
	واپس;

    اگر (attr > HL_ALL)
	attr = syn_attr2attr(attr);
    برائے (i = 0; i < ARRAY_لمبائی(attrs); ++i)
	اگر (STRCMP(name, attrs[i].name) == 0)
	{
	    rettv->vval.v_نمبر = (attr & attrs[i].attr) != 0 ? 1 : 0;
	    توڑو;
	}
}

/*
 * "term_getcursor(buf)" function
 */
    خالی
f_رکن_getcursor(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    رکن_T	*رکن;
    list_T	*l;
    dict_T	*d;

    اگر (rettv_list_alloc(rettv) == FAIL)
	واپس;

    اگر (اندر_vim9script() && check_for_بفر_arg(argvars, 0) == FAIL)
	واپس;

    buf = رکن_get_buf(argvars, "term_getcursor()");
    اگر (buf == تہی)
	واپس;
    رکن = buf->b_رکن;

    l = rettv->vval.v_list;
    list_append_نمبر(l, رکن->tl_cursor_pos.صف + 1);
    list_append_نمبر(l, رکن->tl_cursor_pos.ستون + 1);

    d = dict_alloc();
    اگر (d == تہی)
	واپس;

    dict_جوڑو_نمبر(d, "visible", رکن->tl_cursor_visible);
    dict_جوڑو_نمبر(d, "blink", blink_state_is_inverted()
	    ? !رکن->tl_cursor_blink : رکن->tl_cursor_blink);
    dict_جوڑو_نمبر(d, "shape", رکن->tl_cursor_shape);
    dict_جوڑو_string(d, "color", cursor_color_get(رکن->tl_cursor_color));
    اگر (list_append_dict(l, d) == FAIL)
	dict_unref(d);
}

/*
 * "term_getjob(buf)" function
 */
    خالی
f_رکن_getjob(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    اگر (اندر_vim9script() && check_for_بفر_arg(argvars, 0) == FAIL)
	واپس;

    buf = رکن_get_buf(argvars, "term_getjob()");
    اگر (buf == تہی)
    {
	اگر (اندر_vim9script())
	{
	    rettv->v_type = VAR_کام;
	    rettv->vval.v_کام = تہی;
	}
	ورنہ
	{
	    rettv->v_type = VAR_SPECIAL;
	    rettv->vval.v_نمبر = VVAL_NULL;
	}
	واپس;
    }

    rettv->v_type = VAR_کام;
    rettv->vval.v_کام = buf->b_رکن->tl_کام;
    اگر (rettv->vval.v_کام != تہی)
	++rettv->vval.v_کام->jv_refcount;
}

    ساکن صحیح
get_صف_نمبر(typval_T *tv, رکن_T *رکن)
{
    اگر (tv->v_type == VAR_STRING
	    && tv->vval.v_string != تہی
	    && STRCMP(tv->vval.v_string, ".") == 0)
	واپس رکن->tl_cursor_pos.صف;
    واپس (صحیح)tv_get_نمبر(tv) - 1;
}

/*
 * "term_getline(buf, row)" function
 */
    خالی
f_رکن_getline(typval_T *argvars, typval_T *rettv)
{
    buf_T	    *buf;
    رکن_T	    *رکن;
    صحیح		    صف;

    rettv->v_type = VAR_STRING;

    اگر (اندر_vim9script()
	    && (check_for_بفر_arg(argvars, 0) == FAIL
		|| check_for_lnum_arg(argvars, 1) == FAIL))
	واپس;

    buf = رکن_get_buf(argvars, "term_getline()");
    اگر (buf == تہی)
	واپس;
    رکن = buf->b_رکن;
    صف = get_صف_نمبر(&argvars[1], رکن);

    اگر (رکن->tl_vterm == تہی)
    {
	linenr_T  lnum = 0;
	size_t	  انحراف = 0;
	صحیح	  sb_صف = رکن->tl_scrollback_scrolled + صف;
	sb_سطر_T *سطر;

	اگر (sb_صف < 0 || sb_صف >= رکن->tl_scrollback.ga_len)
	    واپس;
	سطر = (sb_سطر_T *)رکن->tl_scrollback.ga_ڈیٹا + sb_صف;

	scrollbackline_pos_اندر_buf(رکن, sb_صف, &lnum, تہی, &انحراف);

	// vterm is finished, get the text from the buffer
	اگر (lnum > 0 && lnum <= buf->b_ml.ml_سطر_گنتی)
	{
	    char_u *p = ml_get_buf(buf, lnum, FALSE);
	    اگر (STRLEN(p) >= انحراف + سطر->sb_بائٹیں)
		rettv->vval.v_string = vim_strnsave(p + انحراف, سطر->sb_بائٹیں);
	}
    }
    ورنہ
    {
	VTermScreen	*screen = vterm_obtain_screen(رکن->tl_vterm);
	VTermRect	rect;
	صحیح		len;
	char_u		*p;

	اگر (صف < 0 || صف >= رکن->tl_صفیں)
	    واپس;
	len = رکن->tl_ستون_تعداد * MB_MAXBYTES + 1;
	p = alloc(len);
	اگر (p == تہی)
	    واپس;
	rettv->vval.v_string = p;

	rect.آغاز_ستون = 0;
	rect.انتہا_ستون = رکن->tl_ستون_تعداد;
	rect.آغاز_صف = صف;
	rect.انتہا_صف = صف + 1;
	p[vterm_screen_get_متن(screen, (حرف *)p, len, rect)] = NUL;
    }
}

/*
 * "term_getscrolled(buf)" function
 */
    خالی
f_رکن_getscrolled(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    اگر (اندر_vim9script() && check_for_بفر_arg(argvars, 0) == FAIL)
	واپس;

    buf = رکن_get_buf(argvars, "term_getscrolled()");
    اگر (buf == تہی)
	واپس;
    rettv->vval.v_نمبر = buf->b_رکن->tl_بفر_scrolled;
}

/*
 * "term_getsize(buf)" function
 */
    خالی
f_رکن_getsize(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    list_T	*l;

    اگر (rettv_list_alloc(rettv) == FAIL)
	واپس;

    اگر (اندر_vim9script() && check_for_بفر_arg(argvars, 0) == FAIL)
	واپس;

    buf = رکن_get_buf(argvars, "term_getsize()");
    اگر (buf == تہی)
	واپس;

    l = rettv->vval.v_list;
    list_append_نمبر(l, buf->b_رکن->tl_صفیں);
    list_append_نمبر(l, buf->b_رکن->tl_ستون_تعداد);
}

/*
 * "term_setsize(buf, rows, cols)" function
 */
    خالی
f_رکن_setsize(typval_T *argvars, typval_T *rettv غیر_مستعمل)
{
    buf_T	*buf;
    رکن_T	*رکن;
    varnumber_T صفیں_2, ستون_تعداد;

    اگر (اندر_vim9script()
	    && (check_for_بفر_arg(argvars, 0) == FAIL
		|| check_for_نمبر_arg(argvars, 1) == FAIL
		|| check_for_نمبر_arg(argvars, 2) == FAIL))
	واپس;

    buf = رکن_get_buf(argvars, "term_setsize()");
    اگر (buf == تہی)
    {
	emsg(_(e_not_terminal_بفر));
	واپس;
    }
    اگر (buf->b_رکن->tl_vterm == تہی)
	واپس;
    رکن = buf->b_رکن;
    صفیں_2 = tv_get_نمبر(&argvars[1]);
    صفیں_2 = صفیں_2 <= 0 ? رکن->tl_صفیں : صفیں_2;
    ستون_تعداد = tv_get_نمبر(&argvars[2]);
    ستون_تعداد = ستون_تعداد <= 0 ? رکن->tl_ستون_تعداد : ستون_تعداد;
    vterm_سیٹ_سائز(رکن->tl_vterm, صفیں_2, ستون_تعداد);
    // handle_resize() will resize the windows

    // Get and remember the size we ended up with.  Update the pty.
    vterm_get_سائز(رکن->tl_vterm, &رکن->tl_صفیں, &رکن->tl_ستون_تعداد);
    رکن_report_winsize(رکن, رکن->tl_صفیں, رکن->tl_ستون_تعداد);
}

/*
 * "term_getstatus(buf)" function
 */
    خالی
f_رکن_getstatus(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    رکن_T	*رکن;
    char_u	val[100];

    rettv->v_type = VAR_STRING;

    اگر (اندر_vim9script() && check_for_بفر_arg(argvars, 0) == FAIL)
	واپس;

    buf = رکن_get_buf(argvars, "term_getstatus()");
    اگر (buf == تہی)
	واپس;
    رکن = buf->b_رکن;

    اگر (رکن_کام_running(رکن))
	STRCPY(val, "running");
    ورنہ
	STRCPY(val, "finished");
    اگر (رکن->tl_معمول_منوال)
	STRCAT(val, ",normal");
    rettv->vval.v_string = vim_strsave(val);
}

/*
 * "term_gettitle(buf)" function
 */
    خالی
f_رکن_gettitle(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    rettv->v_type = VAR_STRING;

    اگر (اندر_vim9script() && check_for_بفر_arg(argvars, 0) == FAIL)
	واپس;

    buf = رکن_get_buf(argvars, "term_gettitle()");
    اگر (buf == تہی)
	واپس;

    اگر (buf->b_رکن->tl_title != تہی)
	rettv->vval.v_string = vim_strsave(buf->b_رکن->tl_title);
}

/*
 * "term_gettty(buf)" function
 */
    خالی
f_رکن_gettty(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    char_u	*p = تہی;
    صحیح		عدد = 0;

    اگر (اندر_vim9script()
	    && (check_for_بفر_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	واپس;

    rettv->v_type = VAR_STRING;
    buf = رکن_get_buf(argvars, "term_gettty()");
    اگر (buf == تہی)
	واپس;
    اگر (argvars[1].v_type != VAR_UNKNOWN)
	عدد = tv_get_bool(&argvars[1]);

    انتخاب (عدد)
    {
	حالت 0:
	    اگر (buf->b_رکن->tl_کام != تہی)
		p = buf->b_رکن->tl_کام->jv_tty_برآمد;
	    توڑو;
	حالت 1:
	    اگر (buf->b_رکن->tl_کام != تہی)
		p = buf->b_رکن->tl_کام->jv_tty_اندر;
	    توڑو;
	طےشدہ:
	    semsg(_(e_invalid_دلیل_str), tv_get_string(&argvars[1]));
	    واپس;
    }
    اگر (p != تہی)
	rettv->vval.v_string = vim_strsave(p);
}

/*
 * "term_list()" function
 */
    خالی
f_رکن_list(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
    رکن_T	*tp;
    list_T	*l;

    اگر (rettv_list_alloc(rettv) == FAIL || پہلا_رکن == تہی)
	واپس;

    l = rettv->vval.v_list;
    FOR_ALL_TERMS(tp)
	اگر (tp->tl_بفر != تہی)
	    اگر (list_append_نمبر(l,
				   (varnumber_T)tp->tl_بفر->b_fnum) == FAIL)
		واپس;
}

/*
 * "term_scrape(buf, row)" function
 */
    خالی
f_رکن_scrape(typval_T *argvars, typval_T *rettv)
{
    buf_T	    *buf;
    VTermScreen	    *screen = تہی;
    VTermPos	    pos;
    list_T	    *l;
    رکن_T	    *رکن;
    char_u	    *p;
    sb_سطر_T	    *سطر;

    اگر (rettv_list_alloc(rettv) == FAIL)
	واپس;

    اگر (اندر_vim9script()
	    && (check_for_بفر_arg(argvars, 0) == FAIL
		|| check_for_lnum_arg(argvars, 1) == FAIL))
	واپس;

    buf = رکن_get_buf(argvars, "term_scrape()");
    اگر (buf == تہی)
	واپس;
    رکن = buf->b_رکن;

    l = rettv->vval.v_list;
    pos.صف = get_صف_نمبر(&argvars[1], رکن);

    اگر (رکن->tl_vterm != تہی)
    {
	screen = vterm_obtain_screen(رکن->tl_vterm);
	اگر (screen == تہی)  // can't really happen
	    واپس;
	p = تہی;
	سطر = تہی;
    }
    ورنہ
    {
	صحیح	  sb_صف = رکن->tl_scrollback_scrolled + pos.صف;
	linenr_T  lnum = 0;
	size_t	  انحراف = 0;

	scrollbackline_pos_اندر_buf(رکن, sb_صف, &lnum, تہی, &انحراف);

	اگر (sb_صف >= رکن->tl_scrollback.ga_len || lnum <= 0 || lnum > buf->b_ml.ml_سطر_گنتی)
	    واپس;

	سطر = (sb_سطر_T *)رکن->tl_scrollback.ga_ڈیٹا + sb_صف;
	p = ml_get_buf(buf, lnum, FALSE);

	اگر (STRLEN(p) < انحراف + سطر->sb_بائٹیں)
	    واپس;

	p += انحراف;
    }

    برائے (pos.ستون = 0; pos.ستون < رکن->tl_ستون_تعداد; )
    {
	dict_T		*dcell;
	صحیح		چوڑائی;
	VTermScreenCellAttrs attrs;
	VTermColor	fg, bg;
	char_u		rgb[8];
	size_t		rgblen;
	char_u		mbs[MB_MAXBYTES * VTERM_MAX_CHARS_فی_CELL + 1];
	size_t		mbslen;
	صحیح		i;

	اگر (screen == تہی)
	{
	    cellattr_T	*cellattr;

	    // vterm has finished, get the cell from scrollback
	    اگر (pos.ستون >= سطر->sb_ستون_تعداد)
		توڑو;
	    cellattr = سطر->sb_cells + pos.ستون;
	    چوڑائی = cellattr->چوڑائی;
	    attrs = cellattr->attrs;
	    fg = cellattr->fg;
	    bg = cellattr->bg;
	    mbslen = mb_ptr2len(p);
	    mch_memmove(mbs, p, mbslen);
	    mbs[mbslen] = NUL;
	    p += mbslen;
	}
	ورنہ
	{
	    VTermScreenCell cell;

	    اگر (vterm_screen_get_cell(screen, pos, &cell) == 0)
		توڑو;

	    mbslen = 0;
	    برائے (i = 0; i < VTERM_MAX_CHARS_فی_CELL; ++i)
	    {
		اگر (cell.chars[i] == 0)
		    توڑو;
		mbslen += (*utf_char2bytes)((صحیح)cell.chars[i], mbs + mbslen);
	    }
	    mbs[mbslen] = NUL;
	    چوڑائی = cell.چوڑائی;
	    attrs = cell.attrs;
	    fg = cell.fg;
	    bg = cell.bg;
	}
	dcell = dict_alloc();
	اگر (dcell == تہی)
	    توڑو;
	اگر (list_append_dict(l, dcell) == FAIL)
	{
	    dict_unref(dcell);
	    توڑو;
	}

	dict_جوڑو_string_len(dcell, "chars", mbs, (صحیح)mbslen);

	rgblen = vim_snprintf_safelen((حرف *)rgb, حجم(rgb),
	    "#%02x%02x%02x", fg.red, fg.green, fg.blue);
	dict_جوڑو_string_len(dcell, "fg", rgb, (صحیح)rgblen);
	rgblen = vim_snprintf_safelen((حرف *)rgb, حجم(rgb),
	    "#%02x%02x%02x", bg.red, bg.green, bg.blue);
	dict_جوڑو_string_len(dcell, "bg", rgb, (صحیح)rgblen);

	dict_جوڑو_نمبر(dcell, "attr",
				      cell2attr(رکن, تہی, &attrs, &fg, &bg));
	dict_جوڑو_نمبر(dcell, "width", چوڑائی);

	++pos.ستون;
	اگر (چوڑائی == 2)
	    ++pos.ستون;
    }
}

/*
 * "term_sendkeys(buf, keys)" function
 */
    خالی
f_رکن_sendkeys(typval_T *argvars, typval_T *rettv غیر_مستعمل)
{
    buf_T	*buf;
    char_u	*پیغام;
    رکن_T	*رکن;

    اگر (اندر_vim9script()
	    && (check_for_بفر_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	واپس;

    buf = رکن_get_buf(argvars, "term_sendkeys()");
    اگر (buf == تہی)
	واپس;

    پیغام = tv_get_string_chk(&argvars[1]);
    اگر (پیغام == تہی)
	واپس;
    رکن = buf->b_رکن;
    اگر (رکن->tl_vterm == تہی)
	واپس;

    جب (*پیغام != NUL)
    {
	صحیح c;

	اگر (*پیغام == K_SPECIAL && پیغام[1] != NUL && پیغام[2] != NUL)
	{
	    c = مقصد_SPECIAL(پیغام[1], پیغام[2]);
	    پیغام += 3;
	}
	ورنہ
	{
	    c = PTR2CHAR(پیغام);
	    پیغام += MB_CPTR2LEN(پیغام);
	}
	send_کلیدیں_مقصد_رکن(رکن, c, 0, FALSE);
    }
}

#اگر defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * "term_getansicolors(buf)" function
 */
    خالی
f_رکن_getansicolors(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    رکن_T	*رکن;
    VTermState	*state;
    VTermColor  color;
    char_u	hexbuf[10];
    صحیح		اشاریہ;
    list_T	*list;

    اگر (rettv_list_alloc(rettv) == FAIL)
	واپس;

    اگر (اندر_vim9script() && check_for_بفر_arg(argvars, 0) == FAIL)
	واپس;

    buf = رکن_get_buf(argvars, "term_getansicolors()");
    اگر (buf == تہی)
	واپس;
    رکن = buf->b_رکن;
    اگر (رکن->tl_vterm == تہی)
	واپس;

    list = rettv->vval.v_list;
    state = vterm_obtain_state(رکن->tl_vterm);
    برائے (اشاریہ = 0; اشاریہ < 16; اشاریہ++)
    {
	size_t	hexbuflen;

	vterm_state_get_palette_color(state, اشاریہ, &color);
	hexbuflen = vim_snprintf_safelen((حرف *)hexbuf, حجم(hexbuf),
	    "#%02x%02x%02x", color.red, color.green, color.blue);
	اگر (list_append_string(list, hexbuf, (صحیح)hexbuflen) == FAIL)
	    واپس;
    }
}

/*
 * "term_setansicolors(buf, list)" function
 */
    خالی
f_رکن_setansicolors(typval_T *argvars, typval_T *rettv غیر_مستعمل)
{
    buf_T	*buf;
    رکن_T	*رکن;
    listitem_T	*li;
    صحیح		n = 0;

    اگر (اندر_vim9script()
	    && (check_for_بفر_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL))
	واپس;

    buf = رکن_get_buf(argvars, "term_setansicolors()");
    اگر (buf == تہی)
	واپس;
    رکن = buf->b_رکن;
    اگر (رکن->tl_vterm == تہی)
	واپس;

    اگر (check_for_nonnull_list_arg(argvars, 1) == FAIL)
	واپس;

    اگر (argvars[1].vval.v_list->lv_پہلا == &range_list_شے
	    || argvars[1].vval.v_list->lv_len != 16)
    {
	semsg(_(e_invalid_قدر_for_دلیل_str), "\"colors\"");
	واپس;
    }

    اگر (رکن->tl_palette == تہی)
	رکن->tl_palette = ALLOC_MULT(long_u, 16);
    اگر (رکن->tl_palette == تہی)
	واپس;

    FOR_ALL_LIST_اشیاء(argvars[1].vval.v_list, li)
    {
	char_u		*color_name;
	guicolor_T	guicolor;

	color_name = tv_get_string_chk(&li->li_tv);
	اگر (color_name == تہی)
	    واپس;

	guicolor = GUI_GET_COLOR(color_name);
	اگر (guicolor == INVALCOLOR)
	{
	    semsg(_(e_cannot_allocate_color_str), color_name);
	    واپس;
	}

	رکن->tl_palette[n++] = GUI_MCH_GET_RGB(guicolor);
    }

    رکن_update_palette(رکن);
}
#اگر_ختم

/*
 * "term_setapi(buf, api)" function
 */
    خالی
f_رکن_setapi(typval_T *argvars, typval_T *rettv غیر_مستعمل)
{
    buf_T	*buf;
    رکن_T	*رکن;
    char_u	*api;

    اگر (اندر_vim9script()
	    && (check_for_بفر_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	واپس;

    buf = رکن_get_buf(argvars, "term_setapi()");
    اگر (buf == تہی)
	واپس;
    رکن = buf->b_رکن;
    vim_free(رکن->tl_api);
    api = tv_get_string_chk(&argvars[1]);
    اگر (api != تہی)
	رکن->tl_api = vim_strsave(api);
    ورنہ
	رکن->tl_api = تہی;
}

/*
 * "term_setrestore(buf, command)" function
 */
    خالی
f_رکن_setrestore(typval_T *argvars غیر_مستعمل, typval_T *rettv غیر_مستعمل)
{
#اگر defined(FEAT_SESSION)
    buf_T	*buf;
    رکن_T	*رکن;
    char_u	*cmd;

    اگر (اندر_vim9script()
	    && (check_for_بفر_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	واپس;

    buf = رکن_get_buf(argvars, "term_setrestore()");
    اگر (buf == تہی)
	واپس;
    رکن = buf->b_رکن;
    vim_free(رکن->tl_command);
    cmd = tv_get_string_chk(&argvars[1]);
    اگر (cmd != تہی)
	رکن->tl_command = vim_strsave(cmd);
    ورنہ
	رکن->tl_command = تہی;
#اگر_ختم
}

/*
 * "term_setkill(buf, how)" function
 */
    خالی
f_رکن_setkill(typval_T *argvars غیر_مستعمل, typval_T *rettv غیر_مستعمل)
{
    buf_T	*buf;
    رکن_T	*رکن;
    char_u	*how;

    اگر (اندر_vim9script()
	    && (check_for_بفر_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	واپس;

    buf = رکن_get_buf(argvars, "term_setkill()");
    اگر (buf == تہی)
	واپس;
    رکن = buf->b_رکن;
    vim_free(رکن->tl_kill);
    how = tv_get_string_chk(&argvars[1]);
    اگر (how != تہی)
	رکن->tl_kill = vim_strsave(how);
    ورنہ
	رکن->tl_kill = تہی;
}

/*
 * "term_start(command, options)" function
 */
    خالی
f_رکن_آغاز(typval_T *argvars, typval_T *rettv)
{
    jobopt_T	opt;
    buf_T	*buf;

    اگر (اندر_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	واپس;

    init_کام_options(&opt);
    اگر (argvars[1].v_type != VAR_UNKNOWN
	    && get_کام_options(&argvars[1], &opt,
		JO_TIMEOUT_ALL + JO_STOPONEXIT
		    + JO_CALLBACK + JO_برآمد_CALLBACK + JO_ERR_CALLBACK
		    + JO_EXIT_CB + JO_CLOSE_CALLBACK + JO_برآمد_IO,
		JO2_رکن_NAME + JO2_رکن_اختتام + JO2_HIDDEN + JO2_رکن_OPENCMD
		    + JO2_رکن_ستون_تعداد + JO2_رکن_صفیں + JO2_VERTICAL + JO2_CURWIN
		    + JO2_CWD + JO2_ENV + JO2_EOF_CHARS
		    + JO2_NORESTORE + JO2_رکن_KILL + JO2_رکن_HIGHLIGHT
		    + JO2_ANSI_COLORS + JO2_TTY_TYPE + JO2_رکن_API) == FAIL)
	واپس;

    buf = رکن_آغاز(&argvars[0], تہی, &opt, 0);

    اگر (buf != تہی && buf->b_رکن != تہی)
	rettv->vval.v_نمبر = buf->b_fnum;
}

/*
 * "term_wait" function
 */
    خالی
f_رکن_wait(typval_T *argvars, typval_T *rettv غیر_مستعمل)
{
    buf_T	*buf;

    اگر (اندر_vim9script()
	    && (check_for_بفر_arg(argvars, 0) == FAIL
		|| check_for_opt_نمبر_arg(argvars, 1) == FAIL))
	واپس;

    buf = رکن_get_buf(argvars, "term_wait()");
    اگر (buf == تہی)
	واپس;
    اگر (buf->b_رکن->tl_کام == تہی)
    {
	ch_log(تہی, "term_wait(): no job to wait for");
	واپس;
    }
    اگر (buf->b_رکن->tl_کام->jv_channel == تہی)
	// channel is closed, nothing to do
	واپس;

    // Get the job status, this will detect a job that finished.
    اگر (!buf->b_رکن->tl_کام->jv_channel->ch_keep_open
	    && STRCMP(کام_حالت(buf->b_رکن->tl_کام), "dead") == 0)
    {
	// The job is dead, keep reading channel I/O until the channel is
	// closed. buf->b_term may become NULL if the terminal was closed while
	// waiting.
	ch_log(تہی, "term_wait(): waiting for channel to close");
	جب (buf->b_رکن != تہی && !buf->b_رکن->tl_channel_closed)
	{
	    رکن_flush_messages();

	    ui_delay(10L, FALSE);
	    اگر (!buf_valid(buf))
		// If the terminal is closed when the channel is closed the
		// buffer disappears.
		توڑو;
	    اگر (buf->b_رکن == تہی || buf->b_رکن->tl_channel_closing)
		// came here from a close callback, only wait one time
		توڑو;
	}

	رکن_flush_messages();
    }
    ورنہ
    {
	طویل wait = 10L;

	رکن_flush_messages();

	// Wait for some time for any channel I/O.
	اگر (argvars[1].v_type != VAR_UNKNOWN)
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
    خالی
رکن_send_eof(channel_T *ch)
{
    رکن_T	*رکن;

    FOR_ALL_TERMS(رکن)
	اگر (رکن->tl_کام == ch->ch_کام)
	{
	    اگر (رکن->tl_eof_chars != تہی)
	    {
		channel_send(ch, PART_اندر, رکن->tl_eof_chars,
					(صحیح)STRLEN(رکن->tl_eof_chars), تہی);
		channel_send(ch, PART_اندر, (char_u *)"\r", 1, تہی);
	    }
#اگر_تعریف_شدہ MSWIN
	    ورنہ
		// Default: CTRL-D
		channel_send(ch, PART_اندر, (char_u *)"\004\r", 2, تہی);
#اگر_ختم
	}
}

#اگر defined(FEAT_GUI)
    کام_T *
رکن_getjob(رکن_T *رکن)
{
    واپس رکن != تہی ? رکن->tl_کام : تہی;
}
#اگر_ختم

#اگر defined(MSWIN)

///////////////////////////////////////
// 2. MS-Windows implementation.

HRESULT (WINAPI *pCreatePseudoConsole)(COORD, HANDLE, HANDLE, DWORD, HPCON*);
HRESULT (WINAPI *pResizePseudoConsole)(HPCON, COORD);
HRESULT (WINAPI *pClosePseudoConsole)(HPCON);
BOOL (WINAPI *pInitializeProcThreadAttributeList)(LPPROC_THREAD_ATTRIBUTE_LIST, DWORD, DWORD, PSIZE_T);
BOOL (WINAPI *pUpdateProcThreadAttribute)(LPPROC_THREAD_ATTRIBUTE_LIST, DWORD, DWORD_PTR, PVOID, سائز_T, PVOID, PSIZE_T);
خالی (WINAPI *pDeleteProcThreadAttributeList)(LPPROC_THREAD_ATTRIBUTE_LIST);

    ساکن صحیح
dyn_conpty_init(صحیح verbose)
{
    ساکن HMODULE	hKerneldll = تہی;
    صحیح			i;
    ساکن ساخت
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
	{تہی, تہی}
    };

    اگر (!has_conpty_working())
    {
	اگر (verbose)
	    emsg(_(e_conpty_is_not_دستیاب));
	واپس FAIL;
    }

    // No need to initialize twice.
    اگر (hKerneldll)
	واپس OK;

    hKerneldll = vim_بھرو_Lib("kernel32.dll");
    برائے (i = 0; conpty_اندراج[i].name != تہی
					&& conpty_اندراج[i].ptr != تہی; ++i)
    {
	اگر ((*conpty_اندراج[i].ptr = (FARPROC)Get_Proc_پتہ(hKerneldll,
						conpty_اندراج[i].name)) == تہی)
	{
	    اگر (verbose)
		semsg(_(e_could_not_بھرو_library_function_str), conpty_اندراج[i].name);
	    hKerneldll = تہی;
	    واپس FAIL;
	}
    }

    واپس OK;
}

    ساکن صحیح
conpty_رکن_and_کام_init(
	رکن_T	    *رکن,
	typval_T    *argvar,
	حرف	    **argv غیر_مستعمل,
	jobopt_T    *opt,
	jobopt_T    *orig_opt)
{
    WCHAR	    *cmd_wchar = تہی;
    WCHAR	    *cmd_wchar_copy = تہی;
    WCHAR	    *cwd_wchar = تہی;
    WCHAR	    *env_wchar = تہی;
    channel_T	    *channel = تہی;
    کام_T	    *کام = تہی;
    HANDLE	    jo = تہی;
    garray_T	    ga_cmd, ga_env;
    char_u	    *cmd = تہی;
    HRESULT	    hr;
    COORD	    consize;
    سائز_T	    breq;
    PROCESS_INFORMATION proc_info;
    HANDLE	    i_theirs = تہی;
    HANDLE	    o_theirs = تہی;
    HANDLE	    i_ours = تہی;
    HANDLE	    o_ours = تہی;
    حرف	    *errmsg = تہی;

    ga_init2(&ga_cmd, حجم(حرف*), 20);
    ga_init2(&ga_env, حجم(حرف*), 20);

    اگر (argvar->v_type == VAR_STRING)
    {
	cmd = argvar->vval.v_string;
    }
    ورنہ اگر (argvar->v_type == VAR_LIST)
    {
	اگر (win32_build_cmd(argvar->vval.v_list, &ga_cmd) == FAIL)
	    جاؤ failed;
	cmd = ga_cmd.ga_ڈیٹا;
    }
    اگر (cmd == تہی || *cmd == NUL)
    {
	emsg(_(e_invalid_دلیل));
	جاؤ failed;
    }

    رکن->tl_arg0_cmd = vim_strsave(cmd);

    cmd_wchar = enc_مقصد_utf16(cmd, تہی);

    اگر (cmd_wchar != تہی)
    {
	// Request by CreateProcessW
	breq = wcslen(cmd_wchar) + 1 + 1;	// Addition of NUL by API
	cmd_wchar_copy = ALLOC_MULT(WCHAR, breq);
	wcsncpy(cmd_wchar_copy, cmd_wchar, breq - 1);
    }

    ga_clear(&ga_cmd);
    اگر (cmd_wchar == تہی)
	جاؤ failed;
    اگر (opt->jo_cwd != تہی)
    {
	cwd_wchar = enc_مقصد_utf16(opt->jo_cwd, تہی);
	اگر (cwd_wchar == تہی)
	    جاؤ failed;
    }

    win32_build_env(opt->jo_env, &ga_env, TRUE);
    env_wchar = ga_env.ga_ڈیٹا;

    اگر (!CreatePipe(&i_theirs, &i_ours, تہی, 0))
	جاؤ failed;
    اگر (!CreatePipe(&o_ours, &o_theirs, تہی, 0))
	جاؤ failed;

    consize.X = رکن->tl_ستون_تعداد;
    consize.Y = رکن->tl_صفیں;
    hr = pCreatePseudoConsole(consize, i_theirs, o_theirs, 0,
							     &رکن->tl_conpty);
    اگر (FAILED(hr))
	جاؤ failed;

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
    pInitializeProcThreadAttributeList(تہی, 1, 0, &breq);
    رکن->tl_siex.lpAttributeList = alloc(breq);
    اگر (!رکن->tl_siex.lpAttributeList)
	جاؤ failed;
    اگر (!pInitializeProcThreadAttributeList(رکن->tl_siex.lpAttributeList, 1,
								     0, &breq))
	جاؤ failed;
    اگر (!pUpdateProcThreadAttribute(
	    رکن->tl_siex.lpAttributeList, 0,
	    PROC_THREAD_ATTRIBUTE_PSEUDOCONSOLE, رکن->tl_conpty,
	    حجم(HPCON), تہی, تہی))
	جاؤ failed;

    channel = جوڑو_channel();
    اگر (channel == تہی)
	جاؤ failed;

    کام = کام_alloc();
    اگر (کام == تہی)
	جاؤ failed;
    اگر (argvar->v_type == VAR_STRING)
    {
	صحیح argc;

	build_argv_from_string(cmd, &کام->jv_argv, &argc);
    }
    ورنہ
    {
	صحیح argc;

	build_argv_from_list(argvar->vval.v_list, &کام->jv_argv, &argc);
    }

    اگر (opt->jo_سیٹ & JO_اندر_BUF)
	کام->jv_اندر_buf = buflist_findnr(opt->jo_io_buf[PART_اندر]);

    اگر (!CreateProcessW(تہی, cmd_wchar_copy, تہی, تہی, FALSE,
	    EXTENDED_STARTUPINFO_PRESENT | CREATE_UNICODE_ENVIRONMENT
	    | CREATE_SUSPENDED | CREATE_DEFAULT_ERROR_منوال,
	    env_wchar, cwd_wchar,
	    &رکن->tl_siex.StartupInfo, &proc_info))
    {
	errmsg = GetWin32Error();
	جاؤ failed;
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

    jo = Create_کام_Object(تہی, تہی);
    اگر (jo == تہی)
	جاؤ failed;

    اگر (!تفویض_Process_مقصد_کام_Object(jo, proc_info.hProcess))
    {
	// Failed, switch the way to terminate process with TerminateProcess.
	CloseHandle(jo);
	jo = تہی;
    }

    ResumeThread(proc_info.hThread);
    CloseHandle(proc_info.hThread);

    vim_free(cmd_wchar);
    vim_free(cmd_wchar_copy);
    vim_free(cwd_wchar);
    vim_free(env_wchar);

    اگر (create_vterm(رکن, رکن->tl_صفیں, رکن->tl_ستون_تعداد) == FAIL)
	جاؤ failed;

# اگر defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    اگر (رکن_use_palette())
    {
	اگر (رکن->tl_palette != تہی)
	    سیٹ_vterm_palette(رکن->tl_vterm, رکن->tl_palette);
	ورنہ
	    init_vterm_ansi_colors(رکن->tl_vterm);
    }
# endif

    channel_سیٹ_کام(channel, کام, opt);
    کام_سیٹ_options(کام, opt);

    کام->jv_channel = channel;
    کام->jv_proc_info = proc_info;
    کام->jv_کام_object = jo;
    کام->jv_حالت = کام_STARTED;
    کام->jv_tty_type = vim_strsave((char_u *)"conpty");
    ++کام->jv_refcount;
    رکن->tl_کام = کام;

    // Redirecting stdout and stderr doesn't work at the job level.  Instead
    // open the file here and handle it in.  opt->jo_io was changed in
    // setup_job_options(), use the original flags here.
    اگر (orig_opt->jo_io[PART_برآمد] == JIO_FILE)
    {
	char_u *fname = opt->jo_io_name[PART_برآمد];

	ch_log(channel, "Opening output file %s", fname);
	رکن->tl_برآمد_fd = mch_fopen((حرف *)fname, WRITEBIN);
	اگر (رکن->tl_برآمد_fd == تہی)
	    semsg(_(e_cant_open_file_str), fname);
    }

    واپس OK;

failed:
    ga_clear(&ga_cmd);
    ga_clear(&ga_env);
    vim_free(cmd_wchar);
    vim_free(cmd_wchar_copy);
    vim_free(cwd_wchar);
    اگر (channel != تہی)
	channel_clear(channel);
    اگر (کام != تہی)
    {
	کام->jv_channel = تہی;
	کام_cleanup(کام);
    }
    رکن->tl_کام = تہی;
    اگر (jo != تہی)
	CloseHandle(jo);

    اگر (رکن->tl_siex.lpAttributeList != تہی)
    {
	pDeleteProcThreadAttributeList(رکن->tl_siex.lpAttributeList);
	vim_free(رکن->tl_siex.lpAttributeList);
    }
    رکن->tl_siex.lpAttributeList = تہی;
    اگر (o_theirs != تہی)
	CloseHandle(o_theirs);
    اگر (o_ours != تہی)
	CloseHandle(o_ours);
    اگر (i_ours != تہی)
	CloseHandle(i_ours);
    اگر (i_theirs != تہی)
	CloseHandle(i_theirs);
    اگر (رکن->tl_conpty != تہی)
	pClosePseudoConsole(رکن->tl_conpty);
    رکن->tl_conpty = تہی;
    // Propagate errors that occur in CreateProcess
    اگر (errmsg)
	semsg("CreateProcess failed: %s", errmsg);
    واپس FAIL;
}

    ساکن خالی
conpty_رکن_report_winsize(رکن_T *رکن, صحیح صفیں_2, صحیح ستون_تعداد)
{
    COORD consize;

    consize.X = ستون_تعداد;
    consize.Y = صفیں_2;
    pResizePseudoConsole(رکن->tl_conpty, consize);
}

    ساکن خالی
رکن_free_conpty(رکن_T *رکن)
{
    اگر (رکن->tl_siex.lpAttributeList != تہی)
    {
	pDeleteProcThreadAttributeList(رکن->tl_siex.lpAttributeList);
	vim_free(رکن->tl_siex.lpAttributeList);
    }
    رکن->tl_siex.lpAttributeList = تہی;
    اگر (رکن->tl_conpty != تہی)
	pClosePseudoConsole(رکن->tl_conpty);
    رکن->tl_conpty = تہی;
}

    صحیح
use_conpty(خالی)
{
    واپس has_conpty;
}

# define WINPTY_SPAWN_FLAG_AUTO_SHUTDOWN 1ul
# define WINPTY_SPAWN_FLAG_EXIT_AFTER_SHUTDOWN 2ull
# define WINPTY_MOUSE_منوال_FORCE		2

خالی* (*winpty_config_new)(UINT64, خالی*);
خالی* (*winpty_open)(خالی*, خالی*);
خالی* (*winpty_spawn_config_new)(UINT64, خالی*, LPCWSTR, خالی*, خالی*, خالی*);
BOOL (*winpty_spawn)(خالی*, خالی*, HANDLE*, HANDLE*, DWORD*, خالی*);
خالی (*winpty_config_سیٹ_mouse_منوال)(خالی*, صحیح);
خالی (*winpty_config_سیٹ_initial_سائز)(خالی*, صحیح, صحیح);
LPCWSTR (*winpty_conin_name)(خالی*);
LPCWSTR (*winpty_conout_name)(خالی*);
LPCWSTR (*winpty_conerr_name)(خالی*);
خالی (*winpty_free)(خالی*);
خالی (*winpty_config_free)(خالی*);
خالی (*winpty_spawn_config_free)(خالی*);
خالی (*winpty_error_free)(خالی*);
LPCWSTR (*winpty_error_پیغام)(خالی*);
BOOL (*winpty_سیٹ_سائز)(خالی*, صحیح, صحیح, خالی*);
HANDLE (*winpty_agent_process)(خالی*);

# define WINPTY_DLL "winpty.dll"

ساکن HINSTANCE hWinPtyDLL = تہی;

    ساکن صحیح
dyn_winpty_init(صحیح verbose)
{
    صحیح i;
    ساکن ساخت
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
				    (FARPROC*)&winpty_config_سیٹ_initial_سائز},
	{"winpty_conin_name", (FARPROC*)&winpty_conin_name},
	{"winpty_conout_name", (FARPROC*)&winpty_conout_name},
	{"winpty_error_free", (FARPROC*)&winpty_error_free},
	{"winpty_free", (FARPROC*)&winpty_free},
	{"winpty_open", (FARPROC*)&winpty_open},
	{"winpty_spawn", (FARPROC*)&winpty_spawn},
	{"winpty_spawn_config_free", (FARPROC*)&winpty_spawn_config_free},
	{"winpty_spawn_config_new", (FARPROC*)&winpty_spawn_config_new},
	{"winpty_error_msg", (FARPROC*)&winpty_error_پیغام},
	{"winpty_set_size", (FARPROC*)&winpty_سیٹ_سائز},
	{"winpty_agent_process", (FARPROC*)&winpty_agent_process},
	{تہی, تہی}
    };

    // No need to initialize twice.
    اگر (hWinPtyDLL)
	واپس OK;
    // Load winpty.dll, prefer using the 'winptydll' option, fall back to just
    // winpty.dll.
    اگر (*p_winptydll != NUL)
	hWinPtyDLL = vim_بھرو_Lib((حرف *)p_winptydll);
    اگر (!hWinPtyDLL)
	hWinPtyDLL = vim_بھرو_Lib(WINPTY_DLL);
    اگر (!hWinPtyDLL)
    {
	اگر (verbose)
	    semsg(_(e_could_not_بھرو_library_str_str),
		    (*p_winptydll != NUL ? p_winptydll : (char_u *)WINPTY_DLL),
		    GetWin32Error());
	واپس FAIL;
    }
    برائے (i = 0; winpty_اندراج[i].name != تہی
					 && winpty_اندراج[i].ptr != تہی; ++i)
    {
	اگر ((*winpty_اندراج[i].ptr = (FARPROC)Get_Proc_پتہ(hWinPtyDLL,
					      winpty_اندراج[i].name)) == تہی)
	{
	    اگر (verbose)
		semsg(_(e_could_not_بھرو_library_function_str), winpty_اندراج[i].name);
	    hWinPtyDLL = تہی;
	    واپس FAIL;
	}
    }

    واپس OK;
}

    ساکن صحیح
winpty_رکن_and_کام_init(
	رکن_T	    *رکن,
	typval_T    *argvar,
	حرف	    **argv غیر_مستعمل,
	jobopt_T    *opt,
	jobopt_T    *orig_opt)
{
    WCHAR	    *cmd_wchar = تہی;
    WCHAR	    *cwd_wchar = تہی;
    WCHAR	    *env_wchar = تہی;
    channel_T	    *channel = تہی;
    کام_T	    *کام = تہی;
    DWORD	    error;
    HANDLE	    jo = تہی;
    HANDLE	    بچہ_process_handle;
    HANDLE	    بچہ_thread_handle;
    خالی	    *winpty_err = تہی;
    خالی	    *spawn_config = تہی;
    garray_T	    ga_cmd, ga_env;
    char_u	    *cmd = تہی;

    ga_init2(&ga_cmd, حجم(حرف*), 20);
    ga_init2(&ga_env, حجم(حرف*), 20);

    اگر (argvar->v_type == VAR_STRING)
    {
	cmd = argvar->vval.v_string;
    }
    ورنہ اگر (argvar->v_type == VAR_LIST)
    {
	اگر (win32_build_cmd(argvar->vval.v_list, &ga_cmd) == FAIL)
	    جاؤ failed;
	cmd = ga_cmd.ga_ڈیٹا;
    }
    اگر (cmd == تہی || *cmd == NUL)
    {
	emsg(_(e_invalid_دلیل));
	جاؤ failed;
    }

    رکن->tl_arg0_cmd = vim_strsave(cmd);

    cmd_wchar = enc_مقصد_utf16(cmd, تہی);
    ga_clear(&ga_cmd);
    اگر (cmd_wchar == تہی)
	جاؤ failed;
    اگر (opt->jo_cwd != تہی)
    {
	cwd_wchar = enc_مقصد_utf16(opt->jo_cwd, تہی);
	اگر (cwd_wchar == تہی)
	    جاؤ failed;
    }

    win32_build_env(opt->jo_env, &ga_env, TRUE);
    env_wchar = ga_env.ga_ڈیٹا;

    رکن->tl_winpty_config = winpty_config_new(0, &winpty_err);
    اگر (رکن->tl_winpty_config == تہی)
	جاؤ failed;

    winpty_config_سیٹ_mouse_منوال(رکن->tl_winpty_config,
						    WINPTY_MOUSE_منوال_FORCE);
    winpty_config_سیٹ_initial_سائز(رکن->tl_winpty_config,
						 رکن->tl_ستون_تعداد, رکن->tl_صفیں);
    رکن->tl_winpty = winpty_open(رکن->tl_winpty_config, &winpty_err);
    اگر (رکن->tl_winpty == تہی)
	جاؤ failed;

    spawn_config = winpty_spawn_config_new(
	    WINPTY_SPAWN_FLAG_AUTO_SHUTDOWN |
		WINPTY_SPAWN_FLAG_EXIT_AFTER_SHUTDOWN,
	    تہی,
	    cmd_wchar,
	    cwd_wchar,
	    env_wchar,
	    &winpty_err);
    اگر (spawn_config == تہی)
	جاؤ failed;

    channel = جوڑو_channel();
    اگر (channel == تہی)
	جاؤ failed;

    کام = کام_alloc();
    اگر (کام == تہی)
	جاؤ failed;
    اگر (argvar->v_type == VAR_STRING)
    {
	صحیح argc;

	build_argv_from_string(cmd, &کام->jv_argv, &argc);
    }
    ورنہ
    {
	صحیح argc;

	build_argv_from_list(argvar->vval.v_list, &کام->jv_argv, &argc);
    }

    اگر (opt->jo_سیٹ & JO_اندر_BUF)
	کام->jv_اندر_buf = buflist_findnr(opt->jo_io_buf[PART_اندر]);

    اگر (!winpty_spawn(رکن->tl_winpty, spawn_config, &بچہ_process_handle,
	    &بچہ_thread_handle, &error, &winpty_err))
	جاؤ failed;

    channel_سیٹ_pipes(channel,
	(sock_T)CreateFileW(
	    winpty_conin_name(رکن->tl_winpty),
	    GENERIC_WRITE, 0, تہی,
	    OPEN_EXISTING, 0, تہی),
	(sock_T)CreateFileW(
	    winpty_conout_name(رکن->tl_winpty),
	    GENERIC_READ, 0, تہی,
	    OPEN_EXISTING, 0, تہی),
	(sock_T)CreateFileW(
	    winpty_conerr_name(رکن->tl_winpty),
	    GENERIC_READ, 0, تہی,
	    OPEN_EXISTING, 0, تہی));

    // Write lines with CR instead of NL.
    channel->ch_write_متن_منوال = TRUE;

    jo = Create_کام_Object(تہی, تہی);
    اگر (jo == تہی)
	جاؤ failed;

    اگر (!تفویض_Process_مقصد_کام_Object(jo, بچہ_process_handle))
    {
	// Failed, switch the way to terminate process with TerminateProcess.
	CloseHandle(jo);
	jo = تہی;
    }

    winpty_spawn_config_free(spawn_config);
    vim_free(cmd_wchar);
    vim_free(cwd_wchar);
    vim_free(env_wchar);

    اگر (create_vterm(رکن, رکن->tl_صفیں, رکن->tl_ستون_تعداد) == FAIL)
	جاؤ failed;

# اگر defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    اگر (رکن_use_palette())
    {
	اگر (رکن->tl_palette != تہی)
	    سیٹ_vterm_palette(رکن->tl_vterm, رکن->tl_palette);
	ورنہ
	    init_vterm_ansi_colors(رکن->tl_vterm);
    }
# endif

    channel_سیٹ_کام(channel, کام, opt);
    کام_سیٹ_options(کام, opt);

    کام->jv_channel = channel;
    کام->jv_proc_info.hProcess = بچہ_process_handle;
    کام->jv_proc_info.dwProcessId = GetProcessId(بچہ_process_handle);
    کام->jv_کام_object = jo;
    کام->jv_حالت = کام_STARTED;
    کام->jv_tty_اندر = utf16_مقصد_enc(
	    (short_u *)winpty_conin_name(رکن->tl_winpty), تہی);
    کام->jv_tty_برآمد = utf16_مقصد_enc(
	    (short_u *)winpty_conout_name(رکن->tl_winpty), تہی);
    کام->jv_tty_type = vim_strsave((char_u *)"winpty");
    ++کام->jv_refcount;
    رکن->tl_کام = کام;

    // Redirecting stdout and stderr doesn't work at the job level.  Instead
    // open the file here and handle it in.  opt->jo_io was changed in
    // setup_job_options(), use the original flags here.
    اگر (orig_opt->jo_io[PART_برآمد] == JIO_FILE)
    {
	char_u *fname = opt->jo_io_name[PART_برآمد];

	ch_log(channel, "Opening output file %s", fname);
	رکن->tl_برآمد_fd = mch_fopen((حرف *)fname, WRITEBIN);
	اگر (رکن->tl_برآمد_fd == تہی)
	    semsg(_(e_cant_open_file_str), fname);
    }

    واپس OK;

failed:
    ga_clear(&ga_cmd);
    ga_clear(&ga_env);
    vim_free(cmd_wchar);
    vim_free(cwd_wchar);
    اگر (spawn_config != تہی)
	winpty_spawn_config_free(spawn_config);
    اگر (channel != تہی)
	channel_clear(channel);
    اگر (کام != تہی)
    {
	کام->jv_channel = تہی;
	کام_cleanup(کام);
    }
    رکن->tl_کام = تہی;
    اگر (jo != تہی)
	CloseHandle(jo);
    اگر (رکن->tl_winpty != تہی)
	winpty_free(رکن->tl_winpty);
    رکن->tl_winpty = تہی;
    اگر (رکن->tl_winpty_config != تہی)
	winpty_config_free(رکن->tl_winpty_config);
    رکن->tl_winpty_config = تہی;
    اگر (winpty_err != تہی)
    {
	حرف *پیغام = (حرف *)utf16_مقصد_enc(
				(short_u *)winpty_error_پیغام(winpty_err), تہی);

	اگر (پیغام != تہی)
	{
	    emsg(پیغام);
	    vim_free(پیغام);
	}
	winpty_error_free(winpty_err);
    }
    واپس FAIL;
}

/*
 * Create a new terminal of "rows" by "cols" cells.
 * Store a reference in "term".
 * Return OK or FAIL.
 */
    ساکن صحیح
رکن_and_کام_init(
	رکن_T	    *رکن,
	typval_T    *argvar,
	حرف	    **argv,
	jobopt_T    *opt,
	jobopt_T    *orig_opt)
{
    صحیح		    use_winpty = FALSE;
    صحیح		    use_conpty = FALSE;
    صحیح		    tty_type = *p_twt;

    has_winpty = dyn_winpty_init(FALSE) != FAIL ? TRUE : FALSE;
    has_conpty = dyn_conpty_init(FALSE) != FAIL ? TRUE : FALSE;

    اگر (!has_winpty && !has_conpty)
	// If neither is available give the errors for winpty, since when
	// conpty is not available it can't be installed either.
	واپس dyn_winpty_init(TRUE);

    اگر (opt->jo_tty_type != NUL)
	tty_type = opt->jo_tty_type;

    اگر (tty_type == NUL)
    {
	اگر (has_conpty && (is_conpty_stable() || !has_winpty))
	    use_conpty = TRUE;
	ورنہ اگر (has_winpty)
	    use_winpty = TRUE;
	// else: error
    }
    ورنہ اگر (tty_type == 'w')	// winpty
    {
	اگر (has_winpty)
	    use_winpty = TRUE;
    }
    ورنہ اگر (tty_type == 'c')	// conpty
    {
	اگر (has_conpty)
	    use_conpty = TRUE;
	ورنہ
	    واپس dyn_conpty_init(TRUE);
    }

    اگر (use_conpty)
	واپس conpty_رکن_and_کام_init(رکن, argvar, argv, opt, orig_opt);

    اگر (use_winpty)
	واپس winpty_رکن_and_کام_init(رکن, argvar, argv, opt, orig_opt);

    // error
    واپس dyn_winpty_init(TRUE);
}

    ساکن صحیح
create_pty_only(رکن_T *رکن, jobopt_T *options)
{
    HANDLE	    h_Pipe_اندر = INVALID_HANDLE_قدر;
    HANDLE	    h_Pipe_برآمد = INVALID_HANDLE_قدر;
    حرف	    اندر_name[80], برآمد_name[80];
    channel_T	    *channel = تہی;

    اگر (create_vterm(رکن, رکن->tl_صفیں, رکن->tl_ستون_تعداد) == FAIL)
	واپس FAIL;

    vim_snprintf(اندر_name, حجم(اندر_name), "\\\\.\\pipe\\vim-%d-in-%d",
	    GetCurrentProcessId(),
	    curbuf->b_fnum);
    h_Pipe_اندر = CreateNamedPipe(اندر_name, PIPE_ACCESS_OUTBOUND,
	    PIPE_TYPE_پیغام | PIPE_NOWAIT,
	    PIPE_UNLIMITED_INSTANCES,
	    0, 0, NMPWAIT_NOWAIT, تہی);
    اگر (h_Pipe_اندر == INVALID_HANDLE_قدر)
	جاؤ failed;

    vim_snprintf(برآمد_name, حجم(برآمد_name), "\\\\.\\pipe\\vim-%d-out-%d",
	    GetCurrentProcessId(),
	    curbuf->b_fnum);
    h_Pipe_برآمد = CreateNamedPipe(برآمد_name, PIPE_ACCESS_INBOUND,
	    PIPE_TYPE_پیغام | PIPE_NOWAIT,
	    PIPE_UNLIMITED_INSTANCES,
	    0, 0, 0, تہی);
    اگر (h_Pipe_برآمد == INVALID_HANDLE_قدر)
	جاؤ failed;

    ConnectNamedPipe(h_Pipe_اندر, تہی);
    ConnectNamedPipe(h_Pipe_برآمد, تہی);

    رکن->tl_کام = کام_alloc();
    اگر (رکن->tl_کام == تہی)
	جاؤ failed;
    ++رکن->tl_کام->jv_refcount;

    // behave like the job is already finished
    رکن->tl_کام->jv_حالت = کام_ختم_شدہ;

    channel = جوڑو_channel();
    اگر (channel == تہی)
	جاؤ failed;
    رکن->tl_کام->jv_channel = channel;
    channel->ch_keep_open = TRUE;
    channel->ch_named_pipe = TRUE;

    channel_سیٹ_pipes(channel,
	(sock_T)h_Pipe_اندر,
	(sock_T)h_Pipe_برآمد,
	(sock_T)h_Pipe_برآمد);
    channel_سیٹ_کام(channel, رکن->tl_کام, options);
    رکن->tl_کام->jv_tty_اندر = vim_strsave((char_u*)اندر_name);
    رکن->tl_کام->jv_tty_برآمد = vim_strsave((char_u*)برآمد_name);

    واپس OK;

failed:
    اگر (h_Pipe_اندر != تہی)
	CloseHandle(h_Pipe_اندر);
    اگر (h_Pipe_برآمد != تہی)
	CloseHandle(h_Pipe_برآمد);
    واپس FAIL;
}

/*
 * Free the terminal emulator part of "term".
 */
    ساکن خالی
رکن_free_vterm(رکن_T *رکن)
{
    رکن_free_conpty(رکن);
    اگر (رکن->tl_winpty != تہی)
	winpty_free(رکن->tl_winpty);
    رکن->tl_winpty = تہی;
    اگر (رکن->tl_winpty_config != تہی)
	winpty_config_free(رکن->tl_winpty_config);
    رکن->tl_winpty_config = تہی;
    اگر (رکن->tl_vterm != تہی)
	vterm_free(رکن->tl_vterm);
    رکن->tl_vterm = تہی;
}

/*
 * Report the size to the terminal.
 */
    ساکن خالی
رکن_report_winsize(رکن_T *رکن, صحیح صفیں_2, صحیح ستون_تعداد)
{
    اگر (رکن->tl_conpty)
	conpty_رکن_report_winsize(رکن, صفیں_2, ستون_تعداد);
    اگر (رکن->tl_winpty)
	winpty_سیٹ_سائز(رکن->tl_winpty, ستون_تعداد, صفیں_2, تہی);
}

    صحیح
terminal_enabled(خالی)
{
    واپس dyn_winpty_init(FALSE) == OK || dyn_conpty_init(FALSE) == OK;
}

#ورنہ

///////////////////////////////////////
// 3. Unix-like implementation.

/*
 * Create a new terminal of "rows" by "cols" cells.
 * Start job for "cmd".
 * Store the pointers in "term".
 * When "argv" is not NULL then "argvar" is not used.
 * Return OK or FAIL.
 */
    ساکن صحیح
رکن_and_کام_init(
	رکن_T	    *رکن,
	typval_T    *argvar,
	حرف	    **argv,
	jobopt_T    *opt,
	jobopt_T    *orig_opt غیر_مستعمل)
{
    رکن->tl_arg0_cmd = تہی;

    اگر (create_vterm(رکن, رکن->tl_صفیں, رکن->tl_ستون_تعداد) == FAIL)
	واپس FAIL;

# اگر defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    اگر (رکن_use_palette())
    {
	اگر (رکن->tl_palette != تہی)
	    سیٹ_vterm_palette(رکن->tl_vterm, رکن->tl_palette);
	ورنہ
	    init_vterm_ansi_colors(رکن->tl_vterm);
    }
# endif

    // This may change a string in "argvar".
    رکن->tl_کام = کام_آغاز(argvar, argv, opt, &رکن->tl_کام);
    اگر (رکن->tl_کام != تہی)
	++رکن->tl_کام->jv_refcount;

    واپس رکن->tl_کام != تہی
	&& رکن->tl_کام->jv_channel != تہی
	&& رکن->tl_کام->jv_حالت != کام_FAILED ? OK : FAIL;
}

    ساکن صحیح
create_pty_only(رکن_T *رکن, jobopt_T *opt)
{
    اگر (create_vterm(رکن, رکن->tl_صفیں, رکن->tl_ستون_تعداد) == FAIL)
	واپس FAIL;

    رکن->tl_کام = کام_alloc();
    اگر (رکن->tl_کام == تہی)
	واپس FAIL;
    ++رکن->tl_کام->jv_refcount;

    // behave like the job is already finished
    رکن->tl_کام->jv_حالت = کام_ختم_شدہ;

    واپس mch_create_pty_channel(رکن->tl_کام, opt);
}

/*
 * Free the terminal emulator part of "term".
 */
    ساکن خالی
رکن_free_vterm(رکن_T *رکن)
{
    اگر (رکن->tl_vterm != تہی)
	vterm_free(رکن->tl_vterm);
    رکن->tl_vterm = تہی;
}

/*
 * Report the size to the terminal.
 */
    ساکن خالی
رکن_report_winsize(رکن_T *رکن, صحیح صفیں_2, صحیح ستون_تعداد)
{
    // Use an ioctl() to report the new window size to the job.
    اگر (رکن->tl_کام == تہی || رکن->tl_کام->jv_channel == تہی)
	واپس;

    صحیح fd = -1;
    صحیح part;

    برائے (part = PART_برآمد; part < PART_گنتی; ++part)
    {
	fd = رکن->tl_کام->jv_channel->ch_part[part].ch_fd;
	اگر (mch_isatty(fd))
	    توڑو;
    }
    اگر (part < PART_گنتی && mch_report_winsize(fd, صفیں_2, ستون_تعداد) == OK)
	mch_signal_کام(رکن->tl_کام, (char_u *)"winch");
}

#اگر_ختم

#اگر_ختم // FEAT_TERMINAL
