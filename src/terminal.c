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

#اگر_تعریف_نشده MIN
# define MIN(x,y) ((x) < (y) ? (x) : (y))
#پایان_اگر
#اگر_تعریف_نشده MAX
# define MAX(x,y) ((x) > (y) ? (x) : (y))
#پایان_اگر

#ضمن "libvterm/include/vterm.h"

// This is VTermScreenCell without the characters, thus much smaller.
تعريفنوع بنية {
    VTermScreenCellAttrs	attrs;
    حرف			عرض;
    VTermColor			fg;
    VTermColor			bg;
} cellattr_T;

تعريفنوع بنية sb_سطر_S {
    صحيح		sb_تعداد_ستون;	// can differ per line
    صحيح		sb_بایتها;	// length in bytes of text
    cellattr_T	*sb_cells;	// allocated
    cellattr_T	sb_fill_attr;	// for short line
    char_u	*sb_متن;	// for tl_scrollback_postponed
    char_u	continuation;
} sb_سطر_T;

#اگر_تعریف_شده MSWIN
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
#پایان_اگر

// typedef term_T in structs.h
بنية terminal_S {
    جمله_T	*tl_بعدی;

    VTerm	*tl_vterm;
    کار_T	*tl_کار;
    buf_T	*tl_میانگیر;
#إذا defined(FEAT_GUI)
    صحيح		tl_system;	// when non-zero used for :!cmd output
    صحيح		tl_toprow;	// row with first line of system terminal
#پایان_اگر

    // Set when setting the size of a vterm, reset after redrawing.
    صحيح		tl_vterm_اندازه_تغییریافته;

    صحيح		tl_نرمال_نما; // TRUE: Terminal-Normal mode
    صحيح		tl_channel_closing;
    صحيح		tl_channel_closed;
    صحيح		tl_channel_recently_closed; // still need to handle tl_finish

    صحيح		tl_پایان;
#تعریف_کن TL_پایان_UNSET	    NUL
#تعریف_کن TL_پایان_CLOSE	    'c'	// ++close or :terminal without argument
#تعریف_کن TL_پایان_NOCLOSE   'n'	// ++noclose
#تعریف_کن TL_پایان_OPEN	    'o'	// ++open
    char_u	*tl_opencmd;
    char_u	*tl_eof_chars;
    char_u	*tl_api;	// prefix for terminal API function

    char_u	*tl_arg0_cmd;	// To format the status bar

#اگر_تعریف_شده MSWIN
    فراغ	*tl_winpty_config;
    فراغ	*tl_winpty;

    HPCON	tl_conpty;
    DYN_STARTUPINFOEXW tl_siex;	// Structure that always needs to be hold

    FILE	*tl_بیرون_fd;
#پایان_اگر
#إذا defined(FEAT_SESSION)
    char_u	*tl_command;
#پایان_اگر
    char_u	*tl_kill;

    // last known vterm size
    صحيح		tl_ردیفها;
    صحيح		tl_تعداد_ستون;

    char_u	*tl_title; // NULL or allocated
    char_u	*tl_وضعیت_متن; // NULL or allocated

    // Range of screen rows to update.  Zero based.
    صحيح		tl_dirty_ردیف_شروع; // MAX_ROW if nothing dirty
    صحيح		tl_dirty_ردیف_انتها;   // row below last one to update
    صحيح		tl_dirty_snapshot;  // text updated after making snapshot
#اگر_تعریف_شده FEAT_TIMERS
    صحيح		tl_timer_مجموعه;
    proftime_T	tl_timer_due;
#پایان_اگر
    صحيح		tl_postponed_scroll;	// to be scrolled up

    garray_T	tl_scrollback;
    صحيح		tl_scrollback_scrolled;
    garray_T	tl_scrollback_postponed;
    صحيح		tl_scrollback_snapshot;
    صحيح		tl_میانگیر_scrolled;

    char_u	*tl_highlight_name; // replaces "Terminal"; allocated

    cellattr_T	tl_default_color;

    linenr_T	tl_بالاترین_diff_ردیفها;   // rows of top diff file or zero
    linenr_T	tl_bot_diff_ردیفها;   // rows of bottom diff file

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

#تعریف_کن TMODE_ONCE 1	    // CTRL-\ CTRL-N used
#تعریف_کن TMODE_LOOP 2	    // CTRL-W N used

/*
 * List of all active terminals.
 */
ساكن جمله_T *اولی_جمله = عدم;

// Terminal active in terminal_loop().
ساكن جمله_T *درون_terminal_loop = عدم;

#اگر_تعریف_شده MSWIN
ساكن BOOL has_winpty = FALSE;
ساكن BOOL has_conpty = FALSE;
#پایان_اگر

#تعریف_کن MAX_ردیف 999999	    // used for tl_dirty_row_end to update all rows
#تعریف_کن کلید_BUF_LEN 200

#تعریف_کن FOR_ALL_TERMS(جمله)	\
    لكل ((جمله) = اولی_جمله; (جمله) != عدم; (جمله) = (جمله)->tl_بعدی)

/*
 * Functions with separate implementation for MS-Windows and Unix-like systems.
 */
ساكن صحيح جمله_and_کار_init(جمله_T *جمله, typval_T *argvar, حرف **argv, jobopt_T *opt, jobopt_T *orig_opt);
ساكن صحيح create_pty_only(جمله_T *جمله, jobopt_T *opt);
ساكن فراغ جمله_report_winsize(جمله_T *جمله, صحيح ردیفها_2, صحيح تعداد_ستون);
ساكن فراغ جمله_free_vterm(جمله_T *جمله);
#اگر_تعریف_شده FEAT_GUI
ساكن فراغ update_system_جمله(جمله_T *جمله);
#پایان_اگر

ساكن فراغ handle_postponed_scrollback(جمله_T *جمله);

// The character that we know (or assume) that the terminal expects for the
// backspace key.
ساكن صحيح جمله_backspace_char = BS;

// Store the last set and the desired cursor properties, so that we only update
// them when needed.  Doing it unnecessary may result in flicker.
ساكن char_u	*آخرین_مجموعه_cursor_color = عدم;
ساكن char_u	*desired_cursor_color = عدم;
ساكن صحيح	آخرین_مجموعه_cursor_shape = -1;
ساكن صحيح	desired_cursor_shape = -1;
ساكن صحيح	آخرین_مجموعه_cursor_blink = -1;
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
cursor_color_copy(char_u **مقصد_color, char_u *from_color)
{
    // Avoid a free & alloc if the value is already right.
    إذا (cursor_color_equal(*مقصد_color, from_color))
	أرجع;
    vim_free(*مقصد_color);
    *مقصد_color = (from_color == عدم) ? عدم : vim_strsave(from_color);
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
parse_termwinsize(win_T *wp, صحيح *ردیفها_2, صحيح *تعداد_ستون)
{
    صحيح	minsize = FALSE;

    *ردیفها_2 = 0;
    *تعداد_ستون = 0;

    إذا (*wp->w_p_tws == NUL)
	أرجع FALSE;

    char_u *p = vim_strchr(wp->w_p_tws, 'x');

    // Syntax of value was already checked when it's set.
    إذا (p == عدم)
    {
	minsize = TRUE;
	p = vim_strchr(wp->w_p_tws, '*');
    }
    *ردیفها_2 = atoi((حرف *)wp->w_p_tws);
    *تعداد_ستون = atoi((حرف *)p + 1);
    إذا (*ردیفها_2 > VTERM_MAX_ردیفها)
	*ردیفها_2 = VTERM_MAX_ردیفها;
    إذا (*تعداد_ستون > VTERM_MAX_تعداد_ستون)
	*تعداد_ستون = VTERM_MAX_تعداد_ستون;
    أرجع minsize;
}

/*
 * Determine the terminal size from 'termwinsize' and the current window.
 */
    ساكن فراغ
مجموعه_جمله_and_win_اندازه(جمله_T *جمله, jobopt_T *opt)
{
    صحيح ردیفها_2, تعداد_ستون;
    صحيح	minsize;

#اگر_تعریف_شده FEAT_GUI
    إذا (جمله->tl_system)
    {
	// Use the entire screen (excluding the tabpanel area) for the system
	// commands.  However, it will start at the command line and scroll up
	// as needed, using tl_toprow.
	جمله->tl_ردیفها = ردیفها;
	جمله->tl_تعداد_ستون = topframe->fr_عرض;
	أرجع;
    }
#پایان_اگر
    جمله->tl_ردیفها = curwin->w_ارتفاع;
    جمله->tl_تعداد_ستون = curwin->w_عرض;

    minsize = parse_termwinsize(curwin, &ردیفها_2, &تعداد_ستون);
    إذا (minsize)
    {
	إذا (جمله->tl_ردیفها < ردیفها_2)
	    جمله->tl_ردیفها = ردیفها_2;
	إذا (جمله->tl_تعداد_ستون < تعداد_ستون)
	    جمله->tl_تعداد_ستون = تعداد_ستون;
    }
    إذا ((opt->jo_set2 & JO2_جمله_ردیفها))
	جمله->tl_ردیفها = opt->jo_جمله_ردیفها;
    وإلا إذا (ردیفها_2 != 0)
	جمله->tl_ردیفها = ردیفها_2;
    إذا ((opt->jo_set2 & JO2_جمله_تعداد_ستون))
	جمله->tl_تعداد_ستون = opt->jo_جمله_تعداد_ستون;
    وإلا إذا (تعداد_ستون != 0)
	جمله->tl_تعداد_ستون = تعداد_ستون;

    إذا (!opt->jo_hidden)
    {
	إذا (جمله->tl_ردیفها != curwin->w_ارتفاع)
	    win_setheight_win(جمله->tl_ردیفها, curwin);
	إذا (جمله->tl_تعداد_ستون != curwin->w_عرض)
	    win_setwidth_win(جمله->tl_تعداد_ستون, curwin);

	// Set 'winsize' now to avoid a resize at the next redraw.
	إذا (!minsize && *curwin->w_p_tws != NUL)
	{
	    char_u buf[100];

	    vim_snprintf((حرف *)buf, 100, "%dx%d",
						 جمله->tl_ردیفها, جمله->tl_تعداد_ستون);
	    مجموعه_option_مقدار_give_err((char_u *)"termwinsize",
							   0L, buf, OPT_LOCAL);
	}
    }
}

/*
 * Initialize job options for a terminal job.
 * Caller may overrule some of them.
 */
    فراغ
init_کار_options(jobopt_T *opt)
{
    clear_کار_options(opt);

    opt->jo_نما = CH_نما_RAW;
    opt->jo_بیرون_نما = CH_نما_RAW;
    opt->jo_err_نما = CH_نما_RAW;
    opt->jo_مجموعه = JO_نما | JO_بیرون_نما | JO_ERR_نما;
}

/*
 * Set job options mandatory for a terminal job.
 */
    ساكن فراغ
راهاندازی_کار_options(jobopt_T *opt, صحيح ردیفها_2, صحيح تعداد_ستون)
{
#اگر_تعریف_نشده MSWIN
    // Win32: Redirecting the job output won't work, thus always connect stdout
    // here.
    إذا (!(opt->jo_مجموعه & JO_بیرون_IO))
#پایان_اگر
    {
	// Connect stdout to the terminal.
	opt->jo_io[PART_بیرون] = JIO_میانگیر;
	opt->jo_io_buf[PART_بیرون] = curbuf->b_fnum;
	opt->jo_modifiable[PART_بیرون] = 0;
	opt->jo_مجموعه |= JO_بیرون_IO + JO_بیرون_BUF + JO_بیرون_MODIFIABLE;
    }

#اگر_تعریف_نشده MSWIN
    // Win32: Redirecting the job output won't work, thus always connect stderr
    // here.
    إذا (!(opt->jo_مجموعه & JO_ERR_IO))
#پایان_اگر
    {
	// Connect stderr to the terminal.
	opt->jo_io[PART_ERR] = JIO_میانگیر;
	opt->jo_io_buf[PART_ERR] = curbuf->b_fnum;
	opt->jo_modifiable[PART_ERR] = 0;
	opt->jo_مجموعه |= JO_ERR_IO + JO_ERR_BUF + JO_ERR_MODIFIABLE;
    }

    opt->jo_pty = TRUE;
    إذا ((opt->jo_set2 & JO2_جمله_ردیفها) == 0)
	opt->jo_جمله_ردیفها = ردیفها_2;
    إذا ((opt->jo_set2 & JO2_جمله_تعداد_ستون) == 0)
	opt->jo_جمله_تعداد_ستون = تعداد_ستون;
}

/*
 * Flush messages on channels.
 */
    ساكن فراغ
جمله_flush_messages(فراغ)
{
    mch_check_messages();
    parse_queued_messages();
}

/*
 * Close a terminal buffer (and its window).  Used when creating the terminal
 * fails.
 */
    ساكن فراغ
جمله_close_میانگیر(buf_T *buf, buf_T *old_curbuf)
{
    free_terminal(buf);
    إذا (old_curbuf != عدم)
    {
	--curbuf->b_nwindows;
	curbuf = old_curbuf;
	curwin->w_میانگیر = curbuf;
	++curbuf->b_nwindows;
    }
    CHECK_CURBUF;

    // Wiping out the buffer will also close the window and call
    // free_terminal().
    do_میانگیر(DOBUF_WIPE, DOBUF_اولی, FORWARD, buf->b_fnum, TRUE);
}

/*
 * Start a terminal window and return its buffer.
 * Use either "argvar" or "argv", the other must be NULL.
 * When "flags" has TERM_START_NOJOB only create the buffer, b_term and open
 * the window.
 * Returns NULL when failed.
 */
    buf_T *
جمله_شروع(
	typval_T    *argvar,
	حرف	    **argv,
	jobopt_T    *opt,
	صحيح	    flags)
{
    exarg_T	تقسیم_ea;
    win_T	*old_curwin = curwin;
    جمله_T	*جمله;
    buf_T	*old_curbuf = عدم;
    صحيح		res;
    buf_T	*newbuf;
    صحيح		vertical = opt->jo_vertical || (cmdmod.cmod_تقسیم & WSP_VERT);
    jobopt_T	orig_opt;  // only partly filled
    pos_T	save_cursor;

    إذا (check_restricted() || check_secure())
	أرجع عدم;
    إذا (cmdwin_type != 0)
    {
	emsg(_(e_cannot_open_terminal_from_command_سطر_پنجره));
	أرجع عدم;
    }

    إذا ((opt->jo_مجموعه & (JO_درون_IO + JO_بیرون_IO + JO_ERR_IO))
					 == (JO_درون_IO + JO_بیرون_IO + JO_ERR_IO)
	|| (!(opt->jo_مجموعه & JO_بیرون_IO) && (opt->jo_مجموعه & JO_بیرون_BUF))
	|| (!(opt->jo_مجموعه & JO_ERR_IO) && (opt->jo_مجموعه & JO_ERR_BUF))
	|| (argvar != عدم
	    && argvar->v_type == VAR_LIST
	    && argvar->vval.v_list != عدم
	    && argvar->vval.v_list->lv_اولی == &range_list_قلم))
    {
	emsg(_(e_invalid_آرگومان));
	أرجع عدم;
    }

    جمله = ALLOC_CLEAR_ONE(جمله_T);
    إذا (جمله == عدم)
	أرجع عدم;
    جمله->tl_dirty_ردیف_انتها = MAX_ردیف;
    جمله->tl_cursor_visible = TRUE;
    جمله->tl_cursor_shape = VTERM_PROP_CURSORSHAPE_بلوک;
    جمله->tl_پایان = opt->jo_جمله_پایان;
#اگر_تعریف_شده FEAT_GUI
    جمله->tl_system = (flags & جمله_شروع_SYSTEM);
#پایان_اگر
    ga_init2(&جمله->tl_scrollback, حجم(sb_سطر_T), 300);
    ga_init2(&جمله->tl_scrollback_postponed, حجم(sb_سطر_T), 300);
    ga_init2(&جمله->tl_osc_buf, حجم(حرف), 300);

    setpcmark();
    CLEAR_فیلد(تقسیم_ea);
    إذا (opt->jo_curwin)
    {
	// Create a new buffer in the current window.
	إذا (!can_abandon(curbuf, flags & جمله_شروع_FORCEIT))
	{
	    no_write_پیام();
	    vim_free(جمله);
	    أرجع عدم;
	}
	إذا (do_ecmd(0, عدم, عدم, &تقسیم_ea, ECMD_ONE,
		      (buf_hide(curwin->w_میانگیر) ? ECMD_HIDE : 0)
			  + ((flags & جمله_شروع_FORCEIT) ? ECMD_FORCEIT : 0),
							       curwin) == FAIL)
	{
	    vim_free(جمله);
	    أرجع عدم;
	}
    }
    وإلا إذا (opt->jo_hidden || (flags & جمله_شروع_SYSTEM))
    {
	buf_T *buf;

	// Create a new buffer without a window. Make it the current buffer for
	// a moment to be able to do the initializations.
	buf = buflist_new((char_u *)"", عدم, (linenr_T)0,
							 BLN_NEW | BLN_LISTED);
	إذا (buf == عدم || ml_open(buf) == FAIL)
	{
	    vim_free(جمله);
	    أرجع عدم;
	}
	old_curbuf = curbuf;
	--curbuf->b_nwindows;
	curbuf = buf;
	save_cursor = curwin->w_cursor;
	curwin->w_میانگیر = buf;
	++curbuf->b_nwindows;
    }
    وإلا
    {
	// Open a new window or tab.
	تقسیم_ea.cmdidx = CMD_new;
	تقسیم_ea.cmd = (char_u *)"new";
	تقسیم_ea.arg = (char_u *)"";
	إذا (opt->jo_جمله_ردیفها > 0 && !vertical)
	{
	    تقسیم_ea.line2 = opt->jo_جمله_ردیفها;
	    تقسیم_ea.addr_شمار = 1;
	}
	إذا (opt->jo_جمله_تعداد_ستون > 0 && vertical)
	{
	    تقسیم_ea.line2 = opt->jo_جمله_تعداد_ستون;
	    تقسیم_ea.addr_شمار = 1;
	}

	صحيح cmod_تقسیم_modified = FALSE;
	إذا (vertical)
	{
	    إذا (!(cmdmod.cmod_تقسیم & WSP_VERT))
		cmod_تقسیم_modified = TRUE;
	    cmdmod.cmod_تقسیم |= WSP_VERT;
	}
	ex_splitview(&تقسیم_ea);
	إذا (cmod_تقسیم_modified)
	    cmdmod.cmod_تقسیم &= ~WSP_VERT;
	إذا (curwin == old_curwin)
	{
	    // split failed
	    vim_free(جمله);
	    أرجع عدم;
	}
    }
    جمله->tl_میانگیر = curbuf;
    curbuf->b_جمله = جمله;

    إذا (!opt->jo_hidden)
    {
	// Only one size was taken care of with :new, do the other one.  With
	// "curwin" both need to be done.
	إذا (opt->jo_جمله_ردیفها > 0 && (opt->jo_curwin || vertical))
	    win_setheight(opt->jo_جمله_ردیفها);
	إذا (opt->jo_جمله_تعداد_ستون > 0 && (opt->jo_curwin || !vertical))
	    win_setwidth(opt->jo_جمله_تعداد_ستون);
    }

    // Link the new terminal in the list of active terminals.
    جمله->tl_بعدی = اولی_جمله;
    اولی_جمله = جمله;

    apply_autocmds(EVENT_BUFFILEPRE, عدم, عدم, FALSE, curbuf);

    إذا (opt->jo_جمله_name != عدم)
    {
	vim_free(curbuf->b_ffname);
	curbuf->b_ffname = vim_strsave(opt->jo_جمله_name);
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
			       &argvar->vval.v_list->lv_اولی->li_tv)) == عدم)
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

    إذا (opt->jo_جمله_opencmd != عدم)
	جمله->tl_opencmd = vim_strsave(opt->jo_جمله_opencmd);

    إذا (opt->jo_eof_chars != عدم)
	جمله->tl_eof_chars = vim_strsave(opt->jo_eof_chars);

    مجموعه_string_option_مستقیم((char_u *)"buftype", -1,
				  (char_u *)"terminal", OPT_FREE|OPT_LOCAL, 0);
    // Avoid that 'buftype' is reset when this buffer is entered.
    curbuf->b_p_initialized = true;

    // Mark the buffer as not modifiable. It can only be made modifiable after
    // the job finished.
    curbuf->b_p_ma = FALSE;

    مجموعه_جمله_and_win_اندازه(جمله, opt);
#اگر_تعریف_شده MSWIN
    mch_memmove(orig_opt.jo_io, opt->jo_io, حجم(orig_opt.jo_io));
#پایان_اگر
    راهاندازی_کار_options(opt, جمله->tl_ردیفها, جمله->tl_تعداد_ستون);

    إذا (flags & جمله_شروع_NOJOB)
	أرجع curbuf;

#إذا defined(FEAT_SESSION)
    // Remember the command for the session file.
    إذا (opt->jo_جمله_norestore || argv != عدم)
	جمله->tl_command = vim_strsave((char_u *)"NONE");
    وإلا إذا (argvar->v_type == VAR_STRING)
    {
	char_u	*cmd = argvar->vval.v_string;

	إذا (cmd != عدم && STRCMP(cmd, p_sh) != 0)
	    جمله->tl_command = vim_strsave(cmd);
    }
    وإلا إذا (argvar->v_type == VAR_LIST
	    && argvar->vval.v_list != عدم
	    && argvar->vval.v_list->lv_len > 0)
    {
	garray_T	ga;
	listitem_T	*قلم;

	ga_init2(&ga, 1, 100);
	FOR_ALL_LIST_اقلام(argvar->vval.v_list, قلم)
	{
	    char_u *s = tv_get_string_chk(&قلم->li_tv);
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
	إذا (قلم == عدم)
	{
	    ga_append(&ga, NUL);
	    جمله->tl_command = ga.ga_داده;
	}
	وإلا
	    ga_clear(&ga);
    }
#پایان_اگر

    إذا (opt->jo_جمله_kill != عدم)
    {
	char_u *p = skiptowhite(opt->jo_جمله_kill);

	جمله->tl_kill = vim_strnsave(opt->jo_جمله_kill, p - opt->jo_جمله_kill);
    }

    إذا (opt->jo_جمله_api != عدم)
    {
	char_u *p = skiptowhite(opt->jo_جمله_api);

	جمله->tl_api = vim_strnsave(opt->jo_جمله_api, p - opt->jo_جمله_api);
    }
    وإلا
	جمله->tl_api = vim_strsave((char_u *)"Tapi_");

    إذا (opt->jo_set2 & JO2_جمله_HIGHLIGHT)
	جمله->tl_highlight_name = vim_strsave(opt->jo_جمله_highlight);

#إذا defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    // Save the user-defined palette, it is only used in GUI (or 'tgc' is on).
    إذا (opt->jo_set2 & JO2_ANSI_COLORS)
    {
	جمله->tl_palette = ALLOC_MULT(long_u, 16);
	إذا (جمله->tl_palette == عدم)
	{
	    vim_free(جمله);
	    أرجع عدم;
	}
	کپی_حافظه(جمله->tl_palette, opt->jo_ansi_colors, حجم(long_u) * 16);
    }
#پایان_اگر

    // System dependent: setup the vterm and maybe start the job in it.
    إذا (argv == عدم
	    && argvar->v_type == VAR_STRING
	    && argvar->vval.v_string != عدم
	    && STRCMP(argvar->vval.v_string, "NONE") == 0)
	res = create_pty_only(جمله, opt);
    وإلا
	res = جمله_and_کار_init(جمله, argvar, argv, opt, &orig_opt);

    newbuf = curbuf;
    إذا (res == OK)
    {
	// Get and remember the size we ended up with.  Update the pty.
	vterm_get_اندازه(جمله->tl_vterm, &جمله->tl_ردیفها, &جمله->tl_تعداد_ستون);
	جمله_report_winsize(جمله, جمله->tl_ردیفها, جمله->tl_تعداد_ستون);
#اگر_تعریف_شده FEAT_GUI
	إذا (جمله->tl_system)
	{
	    // display first line below typed command
	    جمله->tl_toprow = پیام_ردیف + 1;
	    جمله->tl_dirty_ردیف_انتها = 0;
	}
#پایان_اگر

	// Make sure we don't get stuck on sending keys to the job, it leads to
	// a deadlock if the job is waiting for Vim to read.
	channel_مجموعه_nonblock(جمله->tl_کار->jv_channel, PART_درون);

	إذا (old_curbuf != عدم)
	{
	    --curbuf->b_nwindows;
	    curbuf = old_curbuf;
	    curwin->w_میانگیر = curbuf;
	    curwin->w_cursor = save_cursor;
	    ++curbuf->b_nwindows;
	}
	وإلا إذا (vgetc_busy
#اگر_تعریف_شده FEAT_TIMERS
		|| timer_busy
#پایان_اگر
		|| درون_داد_busy)
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
	جمله_close_میانگیر(curbuf, old_curbuf);
	أرجع عدم;
    }

    apply_autocmds(EVENT_TERMINALOPEN, عدم, عدم, FALSE, newbuf);
    إذا (!opt->jo_hidden && !(flags & جمله_شروع_SYSTEM))
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
    صحيح		scroll_save = پیام_scroll;

    پیام_scroll = FALSE;		// don't scroll here
    autowrite_all();
    پیام_scroll = scroll_save;

    init_کار_options(&opt);

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

#تعریف_کن OPTARG_HAS(name) ((صحيح)(p - cmd) == حجم(name) - 1 \
				 && STRNICMP(cmd, name, حجم(name) - 1) == 0)
	إذا (OPTARG_HAS("close"))
	    opt.jo_جمله_پایان = 'c';
	وإلا إذا (OPTARG_HAS("noclose"))
	    opt.jo_جمله_پایان = 'n';
	وإلا إذا (OPTARG_HAS("open"))
	    opt.jo_جمله_پایان = 'o';
	وإلا إذا (OPTARG_HAS("curwin"))
	    opt.jo_curwin = 1;
	وإلا إذا (OPTARG_HAS("hidden"))
	    opt.jo_hidden = 1;
	وإلا إذا (OPTARG_HAS("norestore"))
	    opt.jo_جمله_norestore = 1;
	وإلا إذا (OPTARG_HAS("shell"))
	    opt_shell = TRUE;
	وإلا إذا (OPTARG_HAS("kill") && ep != عدم)
	{
	    opt.jo_set2 |= JO2_جمله_KILL;
	    opt.jo_جمله_kill = ep + 1;
	    p = skiptowhite(cmd);
	}
	وإلا إذا (OPTARG_HAS("api"))
	{
	    opt.jo_set2 |= JO2_جمله_API;
	    إذا (ep != عدم)
	    {
		opt.jo_جمله_api = ep + 1;
		p = skiptowhite(cmd);
	    }
	    وإلا
		opt.jo_جمله_api = عدم;
	}
	وإلا إذا (OPTARG_HAS("rows") && ep != عدم && امن_isdigit(ep[1]))
	{
	    opt.jo_set2 |= JO2_جمله_ردیفها;
	    opt.jo_جمله_ردیفها = atoi((حرف *)ep + 1);
	    p = skiptowhite(cmd);
	}
	وإلا إذا (OPTARG_HAS("cols") && ep != عدم && امن_isdigit(ep[1]))
	{
	    opt.jo_set2 |= JO2_جمله_تعداد_ستون;
	    opt.jo_جمله_تعداد_ستون = atoi((حرف *)ep + 1);
	    p = skiptowhite(cmd);
	}
	وإلا إذا (OPTARG_HAS("eof") && ep != عدم)
	{
	    char_u *buf = عدم;
	    char_u *کلیدها;

	    vim_free(opt.jo_eof_chars);
	    p = skiptowhite(cmd);
	    *p = NUL;
	    کلیدها = replace_termcodes(ep + 1, &buf, 0,
		    REPTERM_FROM_PART | REPTERM_DO_LT | REPTERM_SPECIAL, عدم);
	    opt.jo_set2 |= JO2_EOF_CHARS;
	    opt.jo_eof_chars = vim_strsave(کلیدها);
	    vim_free(buf);
	    *p = ' ';
	}
#اگر_تعریف_شده MSWIN
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
		semsg(_(e_invalid_مقدار_for_آرگومان_str), "type");
		اذهب theend;
	    }
	    opt.jo_set2 |= JO2_TTY_TYPE;
	    opt.jo_tty_type = tty_type;
	}
#پایان_اگر
	وإلا
	{
	    إذا (*p)
		*p = NUL;
	    semsg(_(e_invalid_attribute_str), cmd);
	    اذهب theend;
	}
#لغو_تعریف OPTARG_HAS
	cmd = skipwhite(p);
    }
    إذا (*cmd == NUL)
    {
	// Make a copy of 'shell', an autocommand may change the option.
	tofree = cmd = vim_strsave(p_sh);

	// default to close when the shell exits
	إذا (opt.jo_جمله_پایان == NUL)
	    opt.jo_جمله_پایان = TL_پایان_CLOSE;
    }

    إذا (eap->addr_شمار > 0)
    {
	// Write lines from current buffer to the job.
	opt.jo_مجموعه |= JO_درون_IO | JO_درون_BUF | JO_درون_بالاترین | JO_درون_BOT;
	opt.jo_io[PART_درون] = JIO_میانگیر;
	opt.jo_io_buf[PART_درون] = curbuf->b_fnum;
	opt.jo_درون_بالاترین = eap->line1;
	opt.jo_درون_bot = eap->line2;
    }

    إذا (opt_shell && tofree == عدم)
    {
#اگر_تعریف_شده UNIX
	حرف	**argv = عدم;
	char_u	*tofree1 = عدم;
	char_u	*tofree2 = عدم;

	// :term ++shell command
	إذا (unix_build_argv(cmd, &argv, &tofree1, &tofree2) == OK)
	    جمله_شروع(عدم, argv, &opt, eap->forceit ? جمله_شروع_FORCEIT : 0);
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
#پایان_اگر
    }
    argvar[0].v_type = VAR_STRING;
    argvar[0].vval.v_string = cmd;
    argvar[1].v_type = VAR_UNKNOWN;
    جمله_شروع(argvar, عدم, &opt, eap->forceit ? جمله_شروع_FORCEIT : 0);

theend:
    vim_free(tofree);
    vim_free(opt.jo_eof_chars);
}

    ساكن char_u *
get_terminalopt_name(expand_T *xp استفادهنشده, صحيح idx)
{
    // Note: Keep this in sync with ex_terminal.
    ساكن حرف *(p_termopt_مقادیر[]) =
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

    إذا (idx < (صحيح)ARRAY_درازی(p_termopt_مقادیر))
	أرجع (char_u*)p_termopt_مقادیر[idx];
    أرجع عدم;
}

    ساكن char_u *
get_termkill_name(expand_T *xp استفادهنشده, صحيح idx)
{
    // These are platform-specific values used for job_stop(). They are defined
    // in each platform's mch_signal_job(). Just use a unified auto-complete
    // list for simplicity.
    ساكن حرف *(p_termkill_مقادیر[]) =
    {
	"term",
	"hup",
	"quit",
	"int",
	"kill",
	"winch",
    };

    إذا (idx < (صحيح)ARRAY_درازی(p_termkill_مقادیر))
	أرجع (char_u*)p_termkill_مقادیر[idx];
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
	صحيح	    *رقم_Matches)
{
    إذا (xp->xp_الگو > xp->xp_سطر && *(xp->xp_الگو-1) == '=')
    {
	char_u *(*cb)(expand_T *, صحيح) = عدم;

	char_u *name_انتها = xp->xp_الگو - 1;
	إذا (name_انتها - xp->xp_سطر >= 4
		&& STRNCMP(name_انتها - 4, "kill", 4) == 0)
	    cb = get_termkill_name;

	إذا (cb != عدم)
	{
	    أرجع ExpandGeneric(
		    pat,
		    xp,
		    rmp,
		    matches,
		    رقم_Matches,
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
	    رقم_Matches,
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
جمله_write_session(FILE *fd, win_T *wp, hashtab_T *terminal_bufs)
{
    ثابت صحيح	bufnr = wp->w_میانگیر->b_fnum;
    جمله_T	*جمله = wp->w_میانگیر->b_جمله;

    إذا (terminal_bufs != عدم && wp->w_میانگیر->b_nwindows > 1)
    {
	// There are multiple views into this terminal buffer. We don't want to
	// create the terminal multiple times. If it's the first time, create,
	// otherwise link to the first buffer.
	حرف	    id_as_str[NUMBUFLEN];
	hashitem_T  *مدخل;

	vim_snprintf(id_as_str, حجم(id_as_str), "%d", bufnr);

	مدخل = hash_پیدا_کن(terminal_bufs, (char_u *)id_as_str);
	إذا (!HASHITEM_خالی(مدخل))
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
		جمله->tl_تعداد_ستون, Columns / 2, Columns,
		جمله->tl_ردیفها, ردیفها / 2, ردیفها) < 0)
	أرجع FAIL;
# ifdef MSWIN
    إذا (fprintf(fd, ".. ' ++type=%s' ", جمله->tl_کار->jv_tty_type) < 0)
	أرجع FAIL;
# endif
    إذا (جمله->tl_command != عدم)
    {
	char_u *quoted_command = string_quote(جمله->tl_command, FALSE);
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

    إذا (terminal_bufs != عدم && wp->w_میانگیر->b_nwindows > 1)
    {
	حرف *hash_کلید = alloc(NUMBUFLEN);

	vim_snprintf(hash_کلید, NUMBUFLEN, "%d", bufnr);
	hash_جمع_کن(terminal_bufs, (char_u *)hash_کلید, "terminal session");
    }

    أرجع put_eol(fd);
}

/*
 * Return TRUE if "buf" has a terminal that should be restored.
 */
    صحيح
جمله_should_restore(buf_T *buf)
{
    جمله_T	*جمله = buf->b_جمله;

    أرجع جمله != عدم && (جمله->tl_command == عدم
				     || STRCMP(جمله->tl_command, "NONE") != 0);
}
#پایان_اگر

/*
 * Free the scrollback buffer for "term".
 */
    ساكن فراغ
free_scrollback(جمله_T *جمله)
{
    صحيح i;

    لكل (i = 0; i < جمله->tl_scrollback.ga_len; ++i)
	vim_free(((sb_سطر_T *)جمله->tl_scrollback.ga_داده + i)->sb_cells);
    ga_clear(&جمله->tl_scrollback);
    لكل (i = 0; i < جمله->tl_scrollback_postponed.ga_len; ++i)
	vim_free(((sb_سطر_T *)جمله->tl_scrollback_postponed.ga_داده + i)->sb_cells);
    ga_clear(&جمله->tl_scrollback_postponed);
}


// Terminals that need to be freed soon.
ساكن جمله_T	*terminals_مقصد_free = عدم;

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
    جمله_T	*جمله = buf->b_جمله;
    جمله_T	*tp;

    إذا (جمله == عدم)
	أرجع;

    // Unlink the terminal form the list of terminals.
    إذا (اولی_جمله == جمله)
	اولی_جمله = جمله->tl_بعدی;
    وإلا
	لكل (tp = اولی_جمله; tp->tl_بعدی != عدم; tp = tp->tl_بعدی)
	    إذا (tp->tl_بعدی == جمله)
	    {
		tp->tl_بعدی = جمله->tl_بعدی;
		اكسر;
	    }

    إذا (جمله->tl_کار != عدم)
    {
	إذا (جمله->tl_کار->jv_وضعیت != کار_ENDED
		&& جمله->tl_کار->jv_وضعیت != کار_پایانیافته
		&& جمله->tl_کار->jv_وضعیت != کار_FAILED)
	    کار_stop(جمله->tl_کار, عدم, "kill");
	کار_unref(جمله->tl_کار);
    }
    جمله->tl_بعدی = terminals_مقصد_free;
    terminals_مقصد_free = جمله;

    buf->b_جمله = عدم;
    إذا (درون_terminal_loop == جمله)
	درون_terminal_loop = عدم;
}

    فراغ
free_استفادهنشده_terminals(فراغ)
{
    طالما (terminals_مقصد_free != عدم)
    {
	جمله_T	    *جمله = terminals_مقصد_free;

	terminals_مقصد_free = جمله->tl_بعدی;

	free_scrollback(جمله);
	ga_clear(&جمله->tl_osc_buf);

	جمله_free_vterm(جمله);
	vim_free(جمله->tl_api);
	vim_free(جمله->tl_title);
#اگر_تعریف_شده FEAT_SESSION
	vim_free(جمله->tl_command);
#پایان_اگر
	vim_free(جمله->tl_kill);
	vim_free(جمله->tl_وضعیت_متن);
	vim_free(جمله->tl_opencmd);
	vim_free(جمله->tl_eof_chars);
	vim_free(جمله->tl_arg0_cmd);
#اگر_تعریف_شده MSWIN
	إذا (جمله->tl_بیرون_fd != عدم)
	    fclose(جمله->tl_بیرون_fd);
#پایان_اگر
	vim_free(جمله->tl_highlight_name);
	vim_free(جمله->tl_cursor_color);
	vim_free(جمله->tl_palette);
	vim_free(جمله);
    }
}

/*
 * Get the part that is connected to the tty. Normally this is PART_IN, but
 * when writing buffer lines to the job it can be another.  This makes it
 * possible to do "1,5term vim -".
 */
    ساكن ch_part_T
get_tty_part(جمله_T *جمله استفادهنشده)
{
#اگر_تعریف_شده UNIX
    ch_part_T	parts[3] = {PART_درون, PART_بیرون, PART_ERR};
    صحيح		i;

    لكل (i = 0; i < 3; ++i)
    {
	صحيح fd = جمله->tl_کار->jv_channel->ch_part[parts[i]].ch_fd;

	إذا (mch_isatty(fd))
	    أرجع parts[i];
    }
#پایان_اگر
    أرجع PART_درون;
}

/*
 * Read any vterm output and send it on the channel.
 */
    ساكن فراغ
جمله_forward_output(جمله_T *جمله)
{
    VTerm *vterm = جمله->tl_vterm;
    حرف   buf[کلید_BUF_LEN];
    size_t curlen = vterm_output_read(vterm, buf, کلید_BUF_LEN);

    إذا (curlen > 0)
	channel_send(جمله->tl_کار->jv_channel, get_tty_part(جمله),
					     (char_u *)buf, (صحيح)curlen, عدم);
}

/*
 * Write job output "msg[len]" to the vterm.
 */
    ساكن فراغ
جمله_write_کار_output(جمله_T *جمله, char_u *پیام_arg, size_t len_arg)
{
    char_u	*پیام = پیام_arg;
    size_t	len = len_arg;
    VTerm	*vterm = جمله->tl_vterm;
    size_t	prevlen = vterm_output_get_میانگیر_current(vterm);
    size_t	حد = جمله->tl_میانگیر->b_p_twsl * جمله->tl_تعداد_ستون * 3;

    // Limit the length to 'termwinscroll' * cols * 3 bytes.  Keep the text at
    // the end.
    إذا (len > حد)
    {
	char_u *p = پیام + len - حد;

	p -= (*mb_سر_off)(پیام, p);
	len -= p - پیام;
	پیام = p;
    }

    vterm_درون_داد_write(vterm, (حرف *)پیام, len);

    // flush vterm buffer when vterm responded to control sequence
    إذا (prevlen != vterm_output_get_میانگیر_current(vterm))
	جمله_forward_output(جمله);

    // this invokes the damage callbacks
    vterm_screen_flush_damage(vterm_obtain_screen(vterm));
}

    ساكن فراغ
position_cursor(win_T *wp, VTermPos *pos)
{
    wp->w_wrow = MIN(pos->ردیف, MAX(0, wp->w_ارتفاع - 1));
    wp->w_wcol = MIN(pos->ستون, MAX(0, wp->w_عرض - 1));
#اگر_تعریف_شده FEAT_PROP_POPUP
    إذا (popup_is_popup(wp))
    {
	wp->w_wrow += popup_بالاترین_extra(wp);
	wp->w_wcol += popup_چپ_extra(wp);
	wp->w_flags |= WFLAG_WCOL_OFF_ADDED | WFLAG_WROW_OFF_ADDED;
    }
    وإلا
	wp->w_flags &= ~(WFLAG_WCOL_OFF_ADDED | WFLAG_WROW_OFF_ADDED);
#پایان_اگر
    wp->w_valid |= (VALID_WCOL|VALID_WROW);
}

    ساكن فراغ
update_cursor(جمله_T *جمله, صحيح redraw)
{
    إذا (جمله->tl_نرمال_نما)
	أرجع;
#اگر_تعریف_شده FEAT_GUI
    إذا (جمله->tl_system)
	windgoto(جمله->tl_cursor_pos.ردیف + جمله->tl_toprow,
		firstwin->w_wincol + جمله->tl_cursor_pos.ستون);
    وإلا
#پایان_اگر
    إذا (!جمله_کار_running(جمله))
	// avoid the cursor positioned below the last used line
	setcursor();
    وإلا
    {
	// do not use the window cursor position
	position_cursor(curwin, &curbuf->b_جمله->tl_cursor_pos);
	windgoto(W_WINROW(curwin) + curwin->w_wrow,
		 curwin->w_wincol + curwin->w_wcol);
    }
    إذا (redraw)
    {
	aco_save_T	aco;

	إذا (جمله->tl_میانگیر == curbuf && جمله->tl_cursor_visible)
	    cursor_on();
	بیرون_flush();
#اگر_تعریف_شده FEAT_GUI
	إذا (gui.درون_use)
	{
	    gui_update_cursor(FALSE, FALSE);
	    gui_mch_flush();
	}
#پایان_اگر
	// Make sure an invoked autocmd doesn't delete the buffer (and the
	// terminal) under our fingers.
	++جمله->tl_میانگیر->b_locked;

	// save and restore curwin and curbuf, in case the autocmd changes them
	aucmd_prepbuf(&aco, curbuf);
	apply_autocmds(EVENT_TEXTCHANGEDT, عدم, عدم, FALSE, جمله->tl_میانگیر);
	aucmd_restbuf(&aco);

	--جمله->tl_میانگیر->b_locked;
    }
}

/*
 * Find the location of a scrollbackline in the buffer
 */
    ساكن فراغ
scrollbackline_pos_درون_buf(جمله_T *جمله, صحيح ردیف, linenr_T *lnum, صحيح *شروع_ستون, size_t *شروع_pos)
{
    sb_سطر_T	*سطرها = (sb_سطر_T *)جمله->tl_scrollback.ga_داده;
    linenr_T    calc_lnum = جمله->tl_میانگیر_scrolled;
    size_t      calc_pos = 0;
    صحيح         calc_ستون = 0;
    صحيح         i;

    إذا (ردیف < 0 || ردیف >= جمله->tl_scrollback.ga_len)
	أرجع;

    إذا (ردیف > جمله->tl_scrollback_scrolled)
    {
	// Lookback how far along in the top line we are
	لكل (i = جمله->tl_scrollback_scrolled + 1; i > 0 && سطرها[i].continuation; --i)
	{
	    calc_pos += سطرها[i - 1].sb_بایتها;
	    calc_ستون += سطرها[i - 1].sb_تعداد_ستون;
	}
	i = جمله->tl_scrollback_scrolled + 1;
	calc_lnum = جمله->tl_میانگیر_scrolled + 1;
	// Do not count this line's bytes/cols twice
	إذا (سطرها[i].continuation)
	    ++i;
    }
    وإلا
    {
	i = 1;
	calc_lnum = 1;
    }

    لكل (; i <= ردیف; ++i)
    {
	إذا (!سطرها[i].continuation)
	{
	    ++calc_lnum;
	    calc_pos = 0;
	    calc_ستون = 0;
	}
	وإلا
	{
	    calc_pos += سطرها[i - 1].sb_بایتها;
	    calc_ستون += سطرها[i - 1].sb_تعداد_ستون;
	}
    }

    *lnum = calc_lnum;
    إذا (شروع_ستون)
	*شروع_ستون = calc_ستون;
    إذا (شروع_pos)
	*شروع_pos = calc_pos;
}

/*
 * Find the location of a buffer line in the scrollback
 */
    ساكن فراغ
bufline_pos_درون_scrollback(جمله_T *جمله, linenr_T lnum, صحيح ستون, صحيح *ردیف, صحيح *wrapped_ستون)
{
    buf_T	*buf = جمله->tl_میانگیر;
    sb_سطر_T	*سطرها = (sb_سطر_T *)جمله->tl_scrollback.ga_داده;
    linenr_T    calc_ردیف = جمله->tl_scrollback_scrolled;
    صحيح         calc_ستون = ستون;
    linenr_T    l;

    إذا (lnum > buf->b_ml.ml_سطر_شمار)
	أرجع;

    إذا (lnum > جمله->tl_میانگیر_scrolled)
    {
	calc_ردیف = جمله->tl_scrollback_scrolled;
	l = جمله->tl_میانگیر_scrolled + 1;

	طالما (calc_ردیف < جمله->tl_scrollback.ga_len && سطرها[calc_ردیف].continuation)
	    ++calc_ردیف;
    }
    وإلا
    {
	calc_ردیف = 0;
	l = 1;
    }

    طالما (calc_ردیف < جمله->tl_scrollback.ga_len && l < lnum)
    {
	++calc_ردیف;
	إذا (!سطرها[calc_ردیف].continuation)
	    ++l;
    }

    طالما (calc_ردیف + 1 < جمله->tl_scrollback.ga_len && سطرها[calc_ردیف + 1].continuation
	    && calc_ستون >= سطرها[calc_ردیف].sb_تعداد_ستون)
    {
	calc_ستون -= سطرها[calc_ردیف].sb_تعداد_ستون;
	++calc_ردیف;
    }

    إذا (ردیف)
	*ردیف = calc_ردیف;
    إذا (wrapped_ستون)
	*wrapped_ستون = calc_ستون;
}

/*
 * Invoked when "msg" output from a job was received.  Write it to the terminal
 * of "buffer".
 */
    فراغ
write_مقصد_جمله(buf_T *میانگیر, char_u *پیام, channel_T *channel)
{
    size_t	len = STRLEN(پیام);
    جمله_T	*جمله = میانگیر->b_جمله;

#اگر_تعریف_شده MSWIN
    // Win32: Cannot redirect output of the job, intercept it here and write to
    // the file.
    إذا (جمله->tl_بیرون_fd != عدم)
    {
	ch_log(channel, "Writing %d bytes to output file", (صحيح)len);
	fwrite(پیام, len, 1, جمله->tl_بیرون_fd);
	أرجع;
    }
#پایان_اگر

    إذا (جمله->tl_vterm == عدم)
    {
	ch_log(channel, "NOT writing %d bytes to terminal", (صحيح)len);
	أرجع;
    }
    ch_log(channel, "writing %d bytes to terminal", (صحيح)len);
    cursor_off();
    جمله_write_کار_output(جمله, پیام, len);

#اگر_تعریف_شده FEAT_GUI
    إذا (جمله->tl_system)
    {
	// show system output, scrolling up the screen as needed
	update_system_جمله(جمله);
	update_cursor(جمله, TRUE);
    }
    وإلا
#پایان_اگر
    // In Terminal-Normal mode we are displaying the buffer, not the terminal
    // contents, thus no screen update is needed.
    إذا (!جمله->tl_نرمال_نما)
    {
	// Don't use update_screen() when editing the command line, it gets
	// cleared.
	// TODO: only update once in a while.
	ch_log(جمله->tl_کار->jv_channel, "updating screen");
	إذا (میانگیر == curbuf && (State & نما_CMDLINE) == 0)
	{
	    update_screen(UPD_VALID_NO_UPDATE);
#إذا defined(FEAT_TABPANEL)
	    إذا (redraw_tabpanel)
		draw_tabpanel();
#پایان_اگر
	    // update_screen() can be slow, check the terminal wasn't closed
	    // already
	    إذا (میانگیر == curbuf && curbuf->b_جمله != عدم)
		update_cursor(curbuf->b_جمله, TRUE);
	}
	وإلا
	    redraw_after_callback(TRUE, FALSE);
    }
}

/*
 * Send a mouse position and click to the vterm
 */
    ساكن صحيح
جمله_send_mouse(VTerm *vterm, صحيح button, صحيح pressed)
{
    VTermModifier   mod = VTERM_MOD_NONE;
    صحيح		    ردیف = mouse_ردیف - W_WINROW(curwin);
    صحيح		    ستون = mouse_ستون - curwin->w_wincol;

#اگر_تعریف_شده FEAT_PROP_POPUP
    إذا (popup_is_popup(curwin))
    {
	ردیف -= popup_بالاترین_extra(curwin);
	ستون -= popup_چپ_extra(curwin);
    }
#پایان_اگر
    vterm_mouse_move(vterm, ردیف, ستون, mod);
    إذا (button != 0)
	vterm_mouse_button(vterm, button, pressed, mod);
    أرجع TRUE;
}

ساكن صحيح enter_mouse_ستون = -1;
ساكن صحيح enter_mouse_ردیف = -1;

/*
 * Handle a mouse click, drag or release.
 * Return TRUE when a mouse event is sent to the terminal.
 */
    ساكن صحيح
جمله_mouse_click(VTerm *vterm, صحيح کلید)
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
		    صحيح save_mouse_ستون, save_mouse_ردیف;

		    إذا (enter_mouse_ستون < 0)
			اكسر;

		    // mouse click in the window gave us focus, handle that
		    // click now
		    save_mouse_ستون = mouse_ستون;
		    save_mouse_ردیف = mouse_ردیف;
		    mouse_ستون = enter_mouse_ستون;
		    mouse_ردیف = enter_mouse_ردیف;
		    clip_modeless(MOUSE_چپ, TRUE, FALSE);
		    mouse_ستون = save_mouse_ستون;
		    mouse_ردیف = save_mouse_ردیف;
		}
		// FALLTHROUGH
	حالة K_LEFTMOUSE:
	حالة K_RIGHTMOUSE:
		إذا (کلید == K_LEFTRELEASE || کلید == K_RIGHTRELEASE)
		    ignore_drag_release = TRUE;
		وإلا
		    ignore_drag_release = FALSE;
		// Should we call mouse_has() here?
		إذا (clip_ستاره.موجود)
		{
		    صحيح	    button, is_click, is_drag;

		    button = get_mouse_button(KEY2TERMCAP1(کلید),
							 &is_click, &is_drag);
		    إذا (mouse_model_popup() && button == MOUSE_چپ
					       && (mod_ماسک & MOD_ماسک_لغزش))
		    {
			// Translate shift-left to right button.
			button = MOUSE_راست;
			mod_ماسک &= ~MOD_ماسک_لغزش;
		    }
		    clip_modeless(button, is_click, is_drag);
		}
		اكسر;

	حالة K_MIDDLEMOUSE:
		إذا (clip_ستاره.موجود)
		    درج_reg('*', TRUE);
		اكسر;
	}
	enter_mouse_ستون = -1;
	أرجع FALSE;
    }
#پایان_اگر
    enter_mouse_ستون = -1;

    بدل (کلید)
    {
	حالة K_LEFTMOUSE:
	حالة K_LEFTMOUSE_NM:	جمله_send_mouse(vterm, 1, 1); اكسر;
	حالة K_LEFTDRAG:	جمله_send_mouse(vterm, 1, 1); اكسر;
	حالة K_LEFTRELEASE:
	حالة K_LEFTRELEASE_NM:	جمله_send_mouse(vterm, 1, 0); اكسر;
	حالة K_MOUSEMOVE:	جمله_send_mouse(vterm, 0, 0); اكسر;
	حالة K_MIDDLEMOUSE:	جمله_send_mouse(vterm, 2, 1); اكسر;
	حالة K_MIDDLEDRAG:	جمله_send_mouse(vterm, 2, 1); اكسر;
	حالة K_MIDDLERELEASE:	جمله_send_mouse(vterm, 2, 0); اكسر;
	حالة K_RIGHTMOUSE:	جمله_send_mouse(vterm, 3, 1); اكسر;
	حالة K_RIGHTDRAG:	جمله_send_mouse(vterm, 3, 1); اكسر;
	حالة K_RIGHTRELEASE:	جمله_send_mouse(vterm, 3, 0); اكسر;
    }
    أرجع TRUE;
}

/*
 * Convert typed key "c" with modifiers "modmask" into bytes to send to the
 * job.
 * Return the number of bytes in "buf".
 */
    ساكن صحيح
جمله_convert_کلید(جمله_T *جمله, صحيح c, صحيح modmask, حرف *buf)
{
    VTerm	    *vterm = جمله->tl_vterm;
    VTerm_کلید	    کلید = VTERM_کلید_NONE;
    VTermModifier   mod = VTERM_MOD_NONE;
    صحيح		    other = FALSE;

    بدل (c)
    {
	// don't use VTERM_KEY_ENTER, it may do an unwanted conversion

	حالة K_BS:
#اگر_تعریف_شده MSWIN
	    // In ConPTY, we must use VTERM_KEY_BACKSPACE, otherwise it will
	    // delete one word, equivalent to Alt+Backspace.
	    إذا (جمله->tl_conpty)
		کلید = VTERM_کلید_BACKSPACE;
	    وإلا
#پایان_اگر
		// don't use VTERM_KEY_BACKSPACE, it always becomes 0x7f DEL
		c = جمله_backspace_char;
	    اكسر;

#اگر_تعریف_شده MSWIN
	حالة BS:
	    // In ConPTY, we must use VTERM_KEY_BACKSPACE, otherwise it will
	    // delete one word, equivalent to Alt+Backspace.
	    إذا (جمله->tl_conpty)
		کلید = VTERM_کلید_BACKSPACE;
	    اكسر;
#پایان_اگر

	حالة ESC:		کلید = VTERM_کلید_ESCAPE; اكسر;
	حالة K_DEL:		کلید = VTERM_کلید_DEL; اكسر;
	حالة K_پایین:		کلید = VTERM_کلید_پایین; اكسر;
	حالة K_S_پایین:		mod = VTERM_MOD_لغزش;
				کلید = VTERM_کلید_پایین; اكسر;
	حالة K_انتها:		کلید = VTERM_کلید_انتها; اكسر;
	حالة K_S_انتها:		mod = VTERM_MOD_لغزش;
				کلید = VTERM_کلید_انتها; اكسر;
	حالة K_C_انتها:		mod = VTERM_MOD_CTRL;
				کلید = VTERM_کلید_انتها; اكسر;
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
	حالة K_S_HOME:		mod = VTERM_MOD_لغزش;
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
	حالة K_چپ:		کلید = VTERM_کلید_چپ; اكسر;
	حالة K_S_چپ:		mod = VTERM_MOD_لغزش;
				کلید = VTERM_کلید_چپ; اكسر;
	حالة K_C_چپ:		mod = VTERM_MOD_CTRL;
				کلید = VTERM_کلید_چپ; اكسر;
	حالة K_PAGEDOWN:	کلید = VTERM_کلید_PAGEDOWN; اكسر;
	حالة K_PAGEUP:		کلید = VTERM_کلید_PAGEUP; اكسر;
	حالة K_راست:		کلید = VTERM_کلید_راست; اكسر;
	حالة K_S_راست:		mod = VTERM_MOD_لغزش;
				کلید = VTERM_کلید_راست; اكسر;
	حالة K_C_راست:		mod = VTERM_MOD_CTRL;
				کلید = VTERM_کلید_راست; اكسر;
	حالة K_به_بالا:		کلید = VTERM_کلید_به_بالا; اكسر;
	حالة K_S_به_بالا:		mod = VTERM_MOD_لغزش;
				کلید = VTERM_کلید_به_بالا; اكسر;
	حالة TAB:		کلید = VTERM_کلید_TAB; اكسر;
	حالة K_S_TAB:		mod = VTERM_MOD_لغزش;
				کلید = VTERM_کلید_TAB; اكسر;

	حالة K_MOUSEUP:		other = جمله_send_mouse(vterm, 5, 1); اكسر;
	حالة K_MOUSEDOWN:	other = جمله_send_mouse(vterm, 4, 1); اكسر;
	حالة K_MOUSELEFT:	other = جمله_send_mouse(vterm, 7, 1); اكسر;
	حالة K_MOUSERIGHT:	other = جمله_send_mouse(vterm, 6, 1); اكسر;

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
	حالة K_RIGHTRELEASE:	إذا (!جمله_mouse_click(vterm, c))
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
#اگر_تعریف_شده FEAT_GUI
	حالة K_VER_SCROLLBAR:	أرجع 0;
	حالة K_HOR_SCROLLBAR:	أرجع 0;
#پایان_اگر
#اگر_تعریف_شده FEAT_GUI_TABLINE
	حالة K_TABLINE:		أرجع 0;
	حالة K_TABMENU:		أرجع 0;
#پایان_اگر
#اگر_تعریف_شده FEAT_NETBEANS_INTG
	حالة K_F21:		کلید = VTERM_کلید_FUNCTION(21); اكسر;
#پایان_اگر
#اگر_تعریف_شده FEAT_DND
	حالة K_DROP:		أرجع 0;
#پایان_اگر
	حالة K_CURSORHOLD:	أرجع 0;
	حالة K_PS:		vterm_keyboard_شروع_paste(vterm);
				other = TRUE;
				اكسر;
	حالة K_PE:		vterm_keyboard_انتها_paste(vterm);
				other = TRUE;
				اكسر;
    }

    // add modifiers for the typed key
    إذا (modmask & MOD_ماسک_لغزش)
	mod |= VTERM_MOD_لغزش;
    إذا (modmask & MOD_ماسک_CTRL)
	mod |= VTERM_MOD_CTRL;
    إذا (modmask & (MOD_ماسک_ALT | MOD_ماسک_META))
	mod |= VTERM_MOD_ALT;

    // Ctrl-Shift-i may have the key "I" instead of "i", but for the kitty
    // keyboard protocol should use "i".  Applies to all ascii letters.
    إذا (ASCII_ISUPPER(c)
	    && vterm_is_kitty_keyboard(vterm)
	    && mod == (VTERM_MOD_CTRL | VTERM_MOD_لغزش))
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
جمله_کار_running_check(جمله_T *جمله, صحيح check_کار_وضعیت)
{
    // Also consider the job finished when the channel is closed, to avoid a
    // race condition when updating the title.
    إذا (جمله == عدم
	|| جمله->tl_کار == عدم
	|| !channel_is_open(جمله->tl_کار->jv_channel))
	أرجع FALSE;

    کار_T *کار = جمله->tl_کار;

    // Careful: Checking the job status may invoke callbacks, which close
    // the buffer and terminate "term".  However, "job" will not be freed
    // yet.
    إذا (check_کار_وضعیت)
	کار_وضعیت(کار);
    أرجع (کار->jv_وضعیت == کار_STARTED
	    || (کار->jv_channel != عدم && کار->jv_channel->ch_keep_open));
}

/*
 * Return TRUE if the job for "term" is still running.
 */
    صحيح
جمله_کار_running(جمله_T *جمله)
{
    أرجع جمله_کار_running_check(جمله, FALSE);
}

/*
 * Return TRUE if the job for "term" is still running, ignoring the job was
 * "NONE".
 */
    صحيح
جمله_کار_running_not_none(جمله_T *جمله)
{
    أرجع جمله_کار_running(جمله) && !جمله_none_open(جمله);
}

/*
 * Return TRUE if "term" has an active channel and used ":term NONE".
 */
    صحيح
جمله_none_open(جمله_T *جمله)
{
    // Also consider the job finished when the channel is closed, to avoid a
    // race condition when updating the title.
    أرجع جمله != عدم
	&& جمله->tl_کار != عدم
	&& channel_is_open(جمله->tl_کار->jv_channel)
	&& جمله->tl_کار->jv_channel->ch_keep_open;
}

//
// Used to confirm whether we would like to kill a terminal.
// Return OK when the user confirms to kill it.
// Return FAIL if the user selects otherwise.
//
    صحيح
جمله_confirm_stop(buf_T *buf)
{
    char_u	buff[DIALOG_پیام_اندازه];
    صحيح	ret;

    dialog_پیام(buff, _("Kill job in \"%s\"?"), buf_get_fname(buf));
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
جمله_try_stop_کار(buf_T *buf)
{
    صحيح	    شمار;
    حرف    *how = (حرف *)buf->b_جمله->tl_kill;

#إذا defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
    إذا ((how == عدم || *how == NUL)
			  && (p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)))
    {
	إذا (جمله_confirm_stop(buf) == OK)
	    how = "kill";
	وإلا
	    أرجع FAIL;
    }
#پایان_اگر
    إذا (how == عدم || *how == NUL)
	أرجع FAIL;

    کار_stop(buf->b_جمله->tl_کار, عدم, how);

    // wait for up to a second for the job to die
    لكل (شمار = 0; شمار < 100; ++شمار)
    {
	کار_T *کار;

	// buffer, terminal and job may be cleaned up while waiting
	إذا (!buf_valid(buf)
		|| buf->b_جمله == عدم
		|| buf->b_جمله->tl_کار == عدم)
	    أرجع OK;
	کار = buf->b_جمله->tl_کار;

	// Call job_status() to update jv_status. It may cause the job to be
	// cleaned up but it won't be freed.
	کار_وضعیت(کار);
	إذا (کار->jv_وضعیت >= کار_ENDED)
	    أرجع OK;

	ui_delay(10L, TRUE);
	جمله_flush_messages();
    }
    أرجع FAIL;
}

/*
 * Add the last line of the scrollback buffer to the buffer in the window.
 */
    ساكن فراغ
جمع_کن_scrollback_سطر_مقصد_میانگیر(جمله_T *جمله, char_u *متن, صحيح len, صحيح append)
{
    buf_T	*buf = جمله->tl_میانگیر;
    صحيح		خالی_2 = (buf->b_ml.ml_flags & ML_خالی);
    linenr_T	lnum = buf->b_ml.ml_سطر_شمار;

#اگر_تعریف_شده MSWIN
    char_u *tmp = متن;
    إذا (!enc_utf8 && enc_codepage > 0)
    {
	WCHAR   *ret = عدم;
	صحيح	درازی = 0;

	Multi_Byte_مقصد_Wide_Char_alloc(CP_UTF8, 0, (حرف*)متن, len + 1,
							   &ret, &درازی);
	إذا (ret != عدم)
	{
	    Wide_Char_مقصد_Multi_Byte_alloc(enc_codepage, 0,
				      ret, درازی, (حرف **)&متن, &len, 0, 0);
	    vim_free(ret);
	}
    }
#پایان_اگر

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
	curbuf = curwin->w_میانگیر;
    }
    وإلا
	ml_append_buf(buf, lnum, متن, len + 1, FALSE);

#اگر_تعریف_شده MSWIN
    إذا (tmp != متن)
	vim_free(متن);
#پایان_اگر
    إذا (خالی_2)
    {
	// Delete the empty line that was in the empty buffer.
	curbuf = buf;
	ml_delete(1);
	curbuf = curwin->w_میانگیر;
    }
}

    ساكن فراغ
cell2cellattr(ثابت VTermScreenCell *cell, cellattr_T *attr)
{
    attr->عرض = cell->عرض;
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
جمع_کن_خالی_scrollback(جمله_T *جمله, cellattr_T *fill_attr, صحيح lnum)
{
    إذا (ga_grow(&جمله->tl_scrollback, 1) == FAIL)
	أرجع FALSE;

    sb_سطر_T *سطر = (sb_سطر_T *)جمله->tl_scrollback.ga_داده
	+ جمله->tl_scrollback.ga_len;

    إذا (lnum > 0)
    {
	صحيح i;

	لكل (i = 0; i < جمله->tl_scrollback.ga_len - lnum; ++i)
	{
	    *سطر = *(سطر - 1);
	    --سطر;
	}
    }
    سطر->sb_تعداد_ستون = 0;
    سطر->sb_بایتها = 0;
    سطر->sb_cells = عدم;
    سطر->sb_fill_attr = *fill_attr;
    ++جمله->tl_scrollback.ga_len;
    أرجع OK;
}

/*
 * Remove the terminal contents from the scrollback and the buffer.
 * Used before adding a new scrollback line or updating the buffer for lines
 * displayed in the terminal.
 */
    ساكن فراغ
cleanup_scrollback(جمله_T *جمله)
{
    sb_سطر_T	*سطر;
    garray_T	*gap;
    char_u	*bufline;
    size_t      bufline_درازی;

    curbuf = جمله->tl_میانگیر;
    gap = &جمله->tl_scrollback;
    bufline = ml_get_buf(curbuf, curbuf->b_ml.ml_سطر_شمار, FALSE);
    bufline_درازی = STRLEN(bufline);
    طالما (جمله->tl_scrollback_snapshot && gap->ga_len > 0)
    {
	سطر = (sb_سطر_T *)gap->ga_داده + gap->ga_len - 1;
	إذا (سطر->sb_بایتها < 0 || (size_t)سطر->sb_بایتها > bufline_درازی)
	    اكسر;
	bufline_درازی -= سطر->sb_بایتها;
	إذا (!bufline_درازی)
	{
	    ml_delete(curbuf->b_ml.ml_سطر_شمار);
	    bufline = ml_get_buf(curbuf, curbuf->b_ml.ml_سطر_شمار, FALSE);
	    bufline_درازی = STRLEN(bufline);
	}
	vim_free(سطر->sb_cells);
	--gap->ga_len;
	--جمله->tl_scrollback_snapshot;
    }
    إذا (bufline_درازی < STRLEN(bufline))
    {
	char_u *shortened = vim_strnsave(bufline, bufline_درازی);
	ml_replace(curbuf->b_ml.ml_سطر_شمار, shortened, FALSE);
    }
    curbuf = curwin->w_میانگیر;
    إذا (curbuf == جمله->tl_میانگیر)
	check_cursor();
}

/*
 * Add the current lines of the terminal to scrollback and to the buffer.
 */
    ساكن فراغ
update_snapshot(جمله_T *جمله)
{
    VTermScreen	    *screen;
    VTermState	    *state;
    صحيح		    len;
    صحيح		    سطرها_skipped = 0;
    VTermPos	    pos;
    VTermScreenCell cell;
    cellattr_T	    fill_attr, new_fill_attr;
    cellattr_T	    *p;

    ch_log(جمله->tl_کار == عدم ? عدم : جمله->tl_کار->jv_channel,
				  "Adding terminal window snapshot to buffer");

    // First remove the lines that were appended before, they might be
    // outdated.
    cleanup_scrollback(جمله);

    screen = vterm_obtain_screen(جمله->tl_vterm);
    state = vterm_obtain_state(جمله->tl_vterm);
    fill_attr = new_fill_attr = جمله->tl_default_color;
    لكل (pos.ردیف = 0; pos.ردیف < جمله->tl_ردیفها; ++pos.ردیف)
    {
	len = 0;
	لكل (pos.ستون = 0; pos.ستون < جمله->tl_تعداد_ستون; ++pos.ستون)
	    إذا (vterm_screen_get_cell(screen, pos, &cell) != 0
						       && cell.chars[0] != NUL)
	    {
		len = pos.ستون + 1;
		new_fill_attr = جمله->tl_default_color;
	    }
	    وإلا
		// Assume the last attr is the filler attr.
		cell2cellattr(&cell, &new_fill_attr);

	إذا (len == 0 && equal_celattr(&new_fill_attr, &fill_attr))
	    ++سطرها_skipped;
	وإلا
	{
	    طالما (سطرها_skipped > 0)
	    {
		// Line was skipped, add an empty line.
		--سطرها_skipped;
		إذا (جمع_کن_خالی_scrollback(جمله, &fill_attr, 0) == OK)
		{
		    جمع_کن_scrollback_سطر_مقصد_میانگیر(جمله, (char_u *)"", 0, 0);
		    ++جمله->tl_scrollback_snapshot;
		}
	    }

	    إذا (len == 0)
		p = عدم;
	    وإلا
		p = ALLOC_MULT(cellattr_T, len);
	    إذا ((p != عدم || len == 0)
				     && ga_grow(&جمله->tl_scrollback, 1) == OK)
	    {
		garray_T    ga;
		صحيح	    عرض;
		ثابت VTerm_سطر_Info *lineinfo;
		sb_سطر_T   *سطر = (sb_سطر_T *)جمله->tl_scrollback.ga_داده
						  + جمله->tl_scrollback.ga_len;

		lineinfo = vterm_state_get_lineinfo(state, pos.ردیف);

		ga_init2(&ga, 1, 100);
		لكل (pos.ستون = 0; pos.ستون < len; pos.ستون += عرض)
		{
		    إذا (vterm_screen_get_cell(screen, pos, &cell) == 0)
		    {
			عرض = 1;
			CLEAR_اشاره_گر(p + pos.ستون);
			إذا (ga_grow(&ga, 1) == OK)
			    ga.ga_len += utf_char2bytes(' ',
					     (char_u *)ga.ga_داده + ga.ga_len);
		    }
		    وإلا
		    {
			عرض = cell.عرض;

			cell2cellattr(&cell, &p[pos.ستون]);
			إذا (عرض == 2)
			    // second cell of double-width character has the
			    // same attributes.
			    p[pos.ستون + 1] = p[pos.ستون];

			// Each character can be up to 6 bytes.
			إذا (ga_grow(&ga, VTERM_MAX_CHARS_در_هر_CELL * 6) == OK)
			{
			    صحيح	    i;
			    صحيح	    c;

			    لكل (i = 0; i < VTERM_MAX_CHARS_در_هر_CELL &&
				    ((c = cell.chars[i]) > 0 || i == 0); ++i)
				ga.ga_len += utf_char2bytes(c == NUL ? ' ' : c,
					     (char_u *)ga.ga_داده + ga.ga_len);
			}
		    }
		}
		سطر->sb_تعداد_ستون = len;
		سطر->sb_بایتها = ga.ga_len;
		سطر->sb_cells = p;
		سطر->sb_fill_attr = new_fill_attr;
		سطر->continuation = (char_u)lineinfo->continuation;
		fill_attr = new_fill_attr;
		++جمله->tl_scrollback.ga_len;
		++جمله->tl_scrollback_snapshot;

		إذا (ga_grow(&ga, 1) == FAIL)
		    جمع_کن_scrollback_سطر_مقصد_میانگیر(جمله, (char_u *)"", 0, 0);
		وإلا
		{
		    *((char_u *)ga.ga_داده + ga.ga_len) = NUL;
		    جمع_کن_scrollback_سطر_مقصد_میانگیر(جمله, ga.ga_داده, ga.ga_len,
						  lineinfo->continuation);
		}
		ga_clear(&ga);
	    }
	    وإلا
		vim_free(p);
	}
    }

    // Add trailing empty lines.
    لكل (pos.ردیف = جمله->tl_scrollback.ga_len;
	    pos.ردیف < جمله->tl_scrollback_scrolled + جمله->tl_cursor_pos.ردیف;
	    ++pos.ردیف)
    {
	إذا (جمع_کن_خالی_scrollback(جمله, &fill_attr, 0) == OK)
	{
	    جمع_کن_scrollback_سطر_مقصد_میانگیر(جمله, (char_u *)"", 0, 0);
	    ++جمله->tl_scrollback_snapshot;
	}
    }

    جمله->tl_dirty_snapshot = FALSE;
#اگر_تعریف_شده FEAT_TIMERS
    جمله->tl_timer_مجموعه = FALSE;
#پایان_اگر
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
    وإلا إذا ((*wp)->w_بعدی != عدم)
	*wp = (*wp)->w_بعدی;
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
may_move_terminal_مقصد_میانگیر(جمله_T *جمله, صحيح redraw)
{
    إذا (جمله->tl_vterm == عدم)
	أرجع;

    // Update the snapshot only if something changes or the buffer does not
    // have all the lines.
    إذا (جمله->tl_dirty_snapshot || جمله->tl_میانگیر->b_ml.ml_سطر_شمار
					       <= جمله->tl_میانگیر_scrolled)
	update_snapshot(جمله);

    إذا (redraw)
    {
	win_T	    *wp = عدم;
	صحيح	    did_curwin = FALSE;

	طالما (for_all_windows_and_curwin(&wp, &did_curwin))
	{
	    إذا (wp->w_میانگیر == جمله->tl_میانگیر)
	    {
		wp->w_cursor.lnum = جمله->tl_میانگیر->b_ml.ml_سطر_شمار;
		wp->w_cursor.ستون = 0;
		wp->w_valid = 0;
		إذا (wp->w_cursor.lnum >= wp->w_ارتفاع)
		{
		    linenr_T min_topline = wp->w_cursor.lnum - wp->w_ارتفاع + 1;

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
جمله_check_timers(صحيح بعدی_due_arg, proftime_T *now)
{
    جمله_T  *جمله;
    صحيح	    بعدی_due = بعدی_due_arg;

    FOR_ALL_TERMS(جمله)
    {
	إذا (جمله->tl_timer_مجموعه && !جمله->tl_نرمال_نما)
	{
	    دراز    this_due = proftime_زمان_چپ(&جمله->tl_timer_due, now);

	    إذا (this_due <= 1)
	    {
		جمله->tl_timer_مجموعه = FALSE;
		may_move_terminal_مقصد_میانگیر(جمله, FALSE);
	    }
	    وإلا إذا (بعدی_due == -1 || بعدی_due > this_due)
		بعدی_due = this_due;
	}
    }

    أرجع بعدی_due;
}
#پایان_اگر

/*
 * When "normal_mode" is TRUE set the terminal to Terminal-Normal mode,
 * otherwise end it.
 */
    ساكن فراغ
مجموعه_terminal_نما(جمله_T *جمله, صحيح نرمال_نما)
{
    جمله->tl_نرمال_نما = نرمال_نما;
    may_trigger_modechanged();
    إذا (!نرمال_نما)
	handle_postponed_scrollback(جمله);
    VIM_CLEAR(جمله->tl_وضعیت_متن);
    إذا (جمله->tl_میانگیر == curbuf)
	maketitle();
}

/*
 * Called after the job is finished and Terminal mode is not active:
 * Move the vterm contents into the scrollback buffer and free the vterm.
 */
    ساكن فراغ
cleanup_vterm(جمله_T *جمله)
{
    مجموعه_terminal_نما(جمله, FALSE);
    إذا (جمله->tl_پایان != TL_پایان_CLOSE)
	may_move_terminal_مقصد_میانگیر(جمله, TRUE);
    جمله_free_vterm(جمله);
}

/*
 * Switch from Terminal-Job mode to Terminal-Normal mode.
 * Suspends updating the terminal window.
 */
    ساكن فراغ
جمله_enter_نرمال_نما(فراغ)
{
    جمله_T     *جمله = curbuf->b_جمله;
    linenr_T   lnum;
    صحيح        ستون;

    مجموعه_terminal_نما(جمله, TRUE);

    // Append the current terminal contents to the buffer.
    may_move_terminal_مقصد_میانگیر(جمله, TRUE);

    // Move the window cursor to the position of the cursor in the
    // terminal.
    lnum = جمله->tl_میانگیر_scrolled + 1 + جمله->tl_cursor_pos.ردیف;
    ستون = جمله->tl_cursor_pos.ستون;
    scrollbackline_pos_درون_buf(جمله, جمله->tl_cursor_pos.ردیف + جمله->tl_scrollback_scrolled, &lnum, &ستون, عدم);

    curwin->w_cursor.lnum = lnum;
    check_cursor();
    إذا (coladvance(ستون) == FAIL)
	coladvance(MAXCOL);
    curwin->w_مجموعه_curswant = true;
    curwin->w_topline = جمله->tl_میانگیر_scrolled + 1;
}

/*
 * Returns TRUE if the current window contains a terminal and we are in
 * Terminal-Normal mode.
 */
    صحيح
جمله_درون_نرمال_نما(buf_T *buf)
{
    جمله_T *جمله = buf->b_جمله;

    أرجع جمله != عدم && جمله->tl_نرمال_نما;
}

/*
 * Switch from Terminal-Normal mode to Terminal-Job mode.
 * Restores updating the terminal window.
 */
    فراغ
جمله_enter_کار_نما(فراغ)
{
    جمله_T	*جمله = curbuf->b_جمله;

    مجموعه_terminal_نما(جمله, FALSE);

    إذا (جمله->tl_channel_closed)
	cleanup_vterm(جمله);
    redraw_buf_and_وضعیت_later(curbuf, UPD_NOT_VALID);
#اگر_تعریف_شده FEAT_PROP_POPUP
    إذا (WIN_IS_POPUP(curwin))
	redraw_later(UPD_NOT_VALID);
#پایان_اگر
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
    NRKS_مجموعه,	    // no_reduce_keys was incremented in term_vgetc() or
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
    أرجع curbuf->b_جمله != عدم
	&& curbuf->b_جمله->tl_vterm != عدم
	&& (vterm_is_modify_other_کلیدها(curbuf->b_جمله->tl_vterm)
		|| vterm_is_kitty_keyboard(curbuf->b_جمله->tl_vterm));
}

    فراغ
check_no_reduce_کلیدها(فراغ)
{
    إذا (no_reduce_کلید_state != NRKS_CHECK
	    || no_reduce_کلیدها >= 1
	    || curbuf->b_جمله == عدم
	    || curbuf->b_جمله->tl_vterm == عدم)
	أرجع;

    إذا (vterm_using_کلید_protocol())
    {
	// "modify_other_keys" or kitty keyboard protocol was enabled while
	// waiting.
	no_reduce_کلید_state = NRKS_مجموعه;
	++no_reduce_کلیدها;
    }
}

/*
 * Get a key from the user with terminal mode mappings.
 * Note: while waiting a terminal may be closed and freed if the channel is
 * closed and ++close was used.  This may even happen before we get here.
 */
    ساكن صحيح
جمله_vgetc(فراغ)
{
    صحيح c;
    صحيح save_State = State;

    State = نما_TERMINAL;
    got_int = FALSE;
#اگر_تعریف_شده MSWIN
    ctrl_break_was_pressed = FALSE;
#پایان_اگر

    إذا (vterm_using_کلید_protocol())
    {
	++no_reduce_کلیدها;
	no_reduce_کلید_state = NRKS_مجموعه;
    }
    وإلا
    {
	no_reduce_کلید_state = NRKS_CHECK;
    }

    c = vgetc();
    got_int = FALSE;
    State = save_State;

    إذا (no_reduce_کلید_state == NRKS_مجموعه)
	--no_reduce_کلیدها;
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
send_کلیدها_مقصد_جمله(جمله_T *جمله, صحيح c, صحيح modmask, صحيح typed)
{
    حرف	پیام[کلید_BUF_LEN];
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
		صحيح	ردیف = mouse_ردیف;
		صحيح	ستون = mouse_ستون;

#اگر_تعریف_شده FEAT_PROP_POPUP
		إذا (popup_is_popup(curwin))
		{
		    ردیف -= popup_بالاترین_extra(curwin);
		    ستون -= popup_چپ_extra(curwin);
		}
#پایان_اگر
		إذا (ردیف < W_WINROW(curwin)
			|| ردیف >= (W_WINROW(curwin) + curwin->w_ارتفاع)
			|| ستون < curwin->w_wincol
			|| ستون >= W_ENDCOL(curwin)
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
    len = جمله_convert_کلید(جمله, c, modmask, پیام);
    إذا (len > 0)
	// TODO: if FAIL is returned, stop?
	channel_send(جمله->tl_کار->jv_channel, get_tty_part(جمله),
						(char_u *)پیام, (صحيح)len, عدم);

    أرجع OK;
}

/*
 * Handle CTRL-W "": send register contents to the job.
 */
    ساكن فراغ
جمله_paste_ثبات(صحيح prev_c استفادهنشده)
{
    صحيح		c;
    list_T	*l;
    listitem_T	*قلم;
    دراز	reglen = 0;
    صحيح		type;

    إذا (جمع_کن_مقصد_showcmd(prev_c))
    إذا (جمع_کن_مقصد_showcmd('"'))
	بیرون_flush();

    c = جمله_vgetc();
    clear_showcmd();

    إذا (!جمله_use_loop())
	// job finished while waiting for a character
	أرجع;

    // CTRL-W "= prompt for expression to evaluate.
    إذا (c == '=' && get_عبارت_ثبات() != '=')
	أرجع;
    إذا (!جمله_use_loop())
	// job finished while waiting for a character
	أرجع;

    l = (list_T *)get_reg_contents(c, GREG_LIST);
    إذا (l == عدم)
	أرجع;

    type = get_reg_type(c, &reglen);
    FOR_ALL_LIST_اقلام(l, قلم)
    {
	char_u *s = tv_get_string(&قلم->li_tv);
#اگر_تعریف_شده MSWIN
	char_u *tmp = s;

	إذا (!enc_utf8 && enc_codepage > 0)
	{
	    WCHAR   *ret = عدم;
	    صحيح	درازی = 0;

	    Multi_Byte_مقصد_Wide_Char_alloc(enc_codepage, 0, (حرف *)s,
		    (صحيح)STRLEN(s), &ret, &درازی);
	    إذا (ret != عدم)
	    {
		Wide_Char_مقصد_Multi_Byte_alloc(CP_UTF8, 0,
			ret, درازی, (حرف **)&s, &درازی, 0, 0);
		vim_free(ret);
	    }
	}
#پایان_اگر
	channel_send(curbuf->b_جمله->tl_کار->jv_channel, PART_درون,
		s, (صحيح)STRLEN(s), عدم);
#اگر_تعریف_شده MSWIN
	إذا (tmp != s)
	    vim_free(s);
#پایان_اگر

	إذا (قلم->li_بعدی != عدم || type == MLINE)
	    channel_send(curbuf->b_جمله->tl_کار->jv_channel, PART_درون,
		    (char_u *)"\r", 1, عدم);
    }
    list_free(l);
}

/*
 * Return TRUE when waiting for a character in the terminal, the cursor of the
 * terminal should be displayed.
 */
    صحيح
terminal_is_فعال(فراغ)
{
    أرجع درون_terminal_loop != عدم;
}

/*
 * Return the highlight group ID for the terminal and the window.
 */
    ساكن صحيح
جمله_get_highlight_id(جمله_T *جمله, win_T *wp)
{
    char_u *name;

    إذا (wp != عدم && wp->w_hlfwin_id != 0)
	name = syn_id2name(wp->w_hlfwin_id);
    وإلا إذا (جمله->tl_highlight_name != عدم)
	name = جمله->tl_highlight_name;
    وإلا
	name = (char_u*)"Terminal";

    أرجع syn_name2id(name);
}

#إذا defined(FEAT_GUI)
    cursorentry_T *
جمله_get_cursor_shape(guicolor_T *fg, guicolor_T *bg)
{
    جمله_T		 *جمله = درون_terminal_loop;
    ساكن cursorentry_T مدخل;
    صحيح			 id;
    guicolor_T		 جمله_fg = INVALCOLOR;
    guicolor_T		 جمله_bg = INVALCOLOR;

    CLEAR_فیلد(مدخل);
    مدخل.shape = مدخل.mshape =
	جمله->tl_cursor_shape == VTERM_PROP_CURSORSHAPE_UNDERLINE ? SHAPE_HOR :
	جمله->tl_cursor_shape == VTERM_PROP_CURSORSHAPE_BAR_چپ ? SHAPE_VER :
	SHAPE_بلوک;
    مدخل.percentage = 20;
    إذا (جمله->tl_cursor_blink)
    {
	مدخل.blinkwait = 700;
	مدخل.blinkon = 400;
	مدخل.blinkoff = 250;
    }

    // The highlight group overrules the defaults.
    id = جمله_get_highlight_id(جمله, curwin);
    إذا (id != 0)
	syn_id2colors(id, &جمله_fg, &جمله_bg);
    إذا (جمله_bg != INVALCOLOR)
	*fg = جمله_bg;
    وإلا
	*fg = gui.پشت_pixel;

    إذا (جمله->tl_cursor_color == عدم)
    {
	إذا (جمله_fg != INVALCOLOR)
	    *bg = جمله_fg;
	وإلا
	    *bg = gui.norm_pixel;
    }
    وإلا
	*bg = color_name2handle(جمله->tl_cursor_color);
    مدخل.name = "n";
    مدخل.استفاده_شده_for = SHAPE_CURSOR;

    أرجع &مدخل;
}
#پایان_اگر

    ساكن فراغ
may_output_cursor_props(فراغ)
{
    إذا (!cursor_color_equal(آخرین_مجموعه_cursor_color, desired_cursor_color)
	    || آخرین_مجموعه_cursor_shape != desired_cursor_shape
	    || آخرین_مجموعه_cursor_blink != desired_cursor_blink)
    {
	cursor_color_copy(&آخرین_مجموعه_cursor_color, desired_cursor_color);
	آخرین_مجموعه_cursor_shape = desired_cursor_shape;
	آخرین_مجموعه_cursor_blink = desired_cursor_blink;
	جمله_cursor_color(cursor_color_get(desired_cursor_color));
	إذا (desired_cursor_shape == -1 || desired_cursor_blink == -1)
	    // this will restore the initial cursor style, if possible
	    ui_cursor_shape_forced(TRUE);
	وإلا
	    جمله_cursor_shape(desired_cursor_shape, desired_cursor_blink);
    }
}

/*
 * Set the cursor color and shape, if not last set to these.
 */
    ساكن فراغ
may_مجموعه_cursor_props(جمله_T *جمله)
{
#اگر_تعریف_شده FEAT_GUI
    // For the GUI the cursor properties are obtained with
    // term_get_cursor_shape().
    إذا (gui.درون_use)
	أرجع;
#پایان_اگر
    إذا (درون_terminal_loop == جمله)
    {
	cursor_color_copy(&desired_cursor_color, جمله->tl_cursor_color);
	desired_cursor_shape = جمله->tl_cursor_shape;
	desired_cursor_blink = جمله->tl_cursor_blink;
	may_output_cursor_props();
    }
}

/*
 * Reset the desired cursor properties and restore them when needed.
 */
    ساكن فراغ
prepare_restore_cursor_props(فراغ)
{
#اگر_تعریف_شده FEAT_GUI
    إذا (gui.درون_use)
	أرجع;
#پایان_اگر
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
جمله_use_loop_check(صحيح check_کار_وضعیت)
{
    جمله_T *جمله = curbuf->b_جمله;

    أرجع جمله != عدم
	&& !جمله->tl_نرمال_نما
	&& جمله->tl_vterm != عدم
	&& جمله_کار_running_check(جمله, check_کار_وضعیت);
}

/*
 * Returns TRUE if the current window contains a terminal and we are sending
 * keys to the job.
 */
    صحيح
جمله_use_loop(فراغ)
{
    أرجع جمله_use_loop_check(FALSE);
}

/*
 * Called when entering a window with the mouse.  If this is a terminal window
 * we may want to change state.
 */
    فراغ
جمله_win_entered(فراغ)
{
    جمله_T *جمله = curbuf->b_جمله;

    إذا (جمله == عدم)
	أرجع;

    إذا (جمله_use_loop_check(TRUE))
    {
	reset_VIsual_and_resel();
	إذا (State & نما_درج)
	    stop_درج_نما = TRUE;
    }
    mouse_was_outside = FALSE;
    enter_mouse_ستون = mouse_ستون;
    enter_mouse_ردیف = mouse_ردیف;
}

    فراغ
جمله_focus_change(صحيح درون_focus)
{
    جمله_T *جمله = curbuf->b_جمله;

    إذا (جمله == عدم || جمله->tl_vterm == عدم)
	أرجع;

    VTermState	*state = vterm_obtain_state(جمله->tl_vterm);

    إذا (درون_focus)
	vterm_state_focus_درون(state);
    وإلا
	vterm_state_focus_بیرون(state);
    جمله_forward_output(جمله);
}

/*
 * vgetc() may not include CTRL in the key when modify_other_keys is set.
 * Return the Ctrl-key value in that case.
 */
    ساكن صحيح
raw_c_مقصد_ctrl(صحيح c)
{
    إذا ((mod_ماسک & MOD_ماسک_CTRL)
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
ctrl_مقصد_raw_c(صحيح c)
{
    إذا (c < 0x20 && vterm_using_کلید_protocol())
    {
	mod_ماسک |= MOD_ماسک_CTRL;
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
#اگر_تعریف_شده UNIX
    صحيح		tty_fd = curbuf->b_جمله->tl_کار->jv_channel
				 ->ch_part[get_tty_part(curbuf->b_جمله)].ch_fd;
#پایان_اگر
    صحيح		restore_cursor = FALSE;

    // Remember the terminal we are sending keys to.  However, the terminal
    // might be closed while waiting for a character, e.g. typing "exit" in a
    // shell and ++close was used.  Therefore use curbuf->b_term instead of a
    // stored reference.
    درون_terminal_loop = curbuf->b_جمله;

    إذا (*curwin->w_p_twk != NUL)
    {
	termwinkey = string_مقصد_کلید(curwin->w_p_twk, TRUE);
	إذا (termwinkey == Ctrl_W)
	    termwinkey = 0;
    }
    position_cursor(curwin, &curbuf->b_جمله->tl_cursor_pos);
    may_مجموعه_cursor_props(curbuf->b_جمله);

    طالما (blocking || vpeekc_nomap() != NUL)
    {
#اگر_تعریف_شده FEAT_GUI
	إذا (curbuf->b_جمله != عدم && !curbuf->b_جمله->tl_system)
#پایان_اگر
	    // TODO: skip screen update when handling a sequence of keys.
	    // Repeat redrawing in case a message is received while redrawing.
	    طالما (must_redraw != 0)
		إذا (update_screen(0) == FAIL)
		    اكسر;
	إذا (!جمله_use_loop_check(TRUE) || درون_terminal_loop != curbuf->b_جمله)
	    // job finished while redrawing
	    اكسر;

	update_cursor(curbuf->b_جمله, FALSE);
	restore_cursor = TRUE;

	raw_c = جمله_vgetc();
	إذا (!جمله_use_loop_check(TRUE) || درون_terminal_loop != curbuf->b_جمله)
	{
	    // Job finished while waiting for a character.  Push back the
	    // received character.
	    إذا (raw_c != K_IGNORE)
		vungetc(raw_c);
	    اكسر;
	}
	إذا (raw_c == K_IGNORE)
	    استمر;
	c = raw_c_مقصد_ctrl(raw_c);

#اگر_تعریف_شده UNIX
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
		جمله_backspace_char = info.backspace;
	}
#پایان_اگر

#اگر_تعریف_شده MSWIN
	// On Windows winpty handles CTRL-C, don't send a CTRL_C_EVENT.
	// Use CTRL-BREAK to kill the job.
	إذا (ctrl_break_was_pressed)
	    mch_signal_کار(curbuf->b_جمله->tl_کار, (char_u *)"kill");
#پایان_اگر
	// Was either CTRL-W (termwinkey) or CTRL-\ pressed?
	// Not in a system terminal.
	إذا ((c == (termwinkey == 0 ? Ctrl_W : termwinkey) || c == Ctrl_BSL)
#اگر_تعریف_شده FEAT_GUI
		&& !curbuf->b_جمله->tl_system
#پایان_اگر
		)
	{
	    صحيح	    prev_c = c;
	    صحيح	    prev_raw_c = raw_c;
	    صحيح	    prev_mod_ماسک = mod_ماسک;

	    إذا (جمع_کن_مقصد_showcmd(c))
		بیرون_flush();

	    raw_c = جمله_vgetc();
	    c = raw_c_مقصد_ctrl(raw_c);

	    clear_showcmd();

	    إذا (!جمله_use_loop_check(TRUE)
					 || درون_terminal_loop != curbuf->b_جمله)
		// job finished while waiting for a character
		اكسر;

	    إذا (prev_c == Ctrl_BSL)
	    {
		إذا (c == Ctrl_N)
		{
		    // CTRL-\ CTRL-N : go to Terminal-Normal mode.
		    جمله_enter_نرمال_نما();
		    ret = FAIL;
		    اذهب theend;
		}
		// Send both keys to the terminal, first one here, second one
		// below.
		send_کلیدها_مقصد_جمله(curbuf->b_جمله, prev_raw_c, prev_mod_ماسک,
									 TRUE);
	    }
	    وإلا إذا (c == Ctrl_C)
	    {
		// "CTRL-W CTRL-C" or 'termwinkey' CTRL-C: end the job
		mch_signal_کار(curbuf->b_جمله->tl_کار, (char_u *)"kill");
	    }
	    وإلا إذا (c == '.')
	    {
		// "CTRL-W .": send CTRL-W to the job
		// "'termwinkey' .": send 'termwinkey' to the job
		raw_c = ctrl_مقصد_raw_c(termwinkey == 0 ? Ctrl_W : termwinkey);
	    }
	    وإلا إذا (c == Ctrl_BSL)
	    {
		// "CTRL-W CTRL-\": send CTRL-\ to the job
		raw_c = ctrl_مقصد_raw_c(Ctrl_BSL);
	    }
	    وإلا إذا (c == 'N')
	    {
		// CTRL-W N : go to Terminal-Normal mode.
		جمله_enter_نرمال_نما();
		ret = FAIL;
		اذهب theend;
	    }
	    وإلا إذا (c == '"')
	    {
		جمله_paste_ثبات(prev_c);
		استمر;
	    }
	    وإلا إذا (termwinkey == 0 || c != termwinkey)
	    {
		// space for CTRL-W, modifier, multi-byte char and NUL
		char_u buf[1 + 3 + MB_MAXBYTES + 1];

		// Put the command into the typeahead buffer, when using the
		// stuff buffer KeyStuffed is set and 'langmap' won't be used.
		buf[0] = Ctrl_W;
		buf[special_مقصد_buf(c, mod_ماسک, FALSE, buf + 1) + 1] = NUL;
		ins_typebuf(buf, REMAP_NONE, 0, TRUE, FALSE);
		ret = OK;
		اذهب theend;
	    }
	}
#اگر_تعریف_شده MSWIN
	إذا (!enc_utf8 && has_mbyte && raw_c >= 0x80)
	{
	    WCHAR   wc;
	    char_u  mb[3];

	    mb[0] = (بیعلامت)raw_c >> 8;
	    mb[1] = raw_c;
	    إذا (Multi_Byte_مقصد_Wide_Char(GetACP(), 0, (حرف*)mb, 2, &wc, 1) > 0)
		raw_c = wc;
	}
#پایان_اگر
	إذا (send_کلیدها_مقصد_جمله(curbuf->b_جمله, raw_c, mod_ماسک, TRUE) != OK)
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
    درون_terminal_loop = عدم;
    إذا (restore_cursor)
	prepare_restore_cursor_props();

    // Move a snapshot of the screen contents to the buffer, so that completion
    // works in other buffers.
    إذا (curbuf->b_جمله != عدم && !curbuf->b_جمله->tl_نرمال_نما)
	may_move_terminal_مقصد_میانگیر(curbuf->b_جمله, FALSE);

    أرجع ret;
}

    ساكن فراغ
may_toggle_cursor(جمله_T *جمله)
{
    إذا (درون_terminal_loop != جمله)
	أرجع;

    إذا (جمله->tl_cursor_visible)
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

    إذا (VTERM_COLOR_IS_نمایهای(color))
    {
	// Use the color as-is if possible, give up otherwise.
	إذا (color->شاخص < t_colors)
	{
	    uint8_t شاخص = color->شاخص;
#اگر_تعریف_شده MSWIN
	    // Convert ANSI palette to cterm color index.
	    // cterm_ansi_idx is a table for the reverse conversion from cterm
	    // color index to ANSI palette. However, the current table can
	    // be used for this conversion as well.
	    // Note: If the contents of cterm_ansi_idx change in the future, a
	    // different table may be needed for this purpose.
	    إذا (شاخص < 16)
		شاخص = cterm_ansi_idx[شاخص];
#پایان_اگر
	    أرجع شاخص + 1;
	}
	// 8-color terminals can actually display twice as many colors by
	// setting the high-intensity/bold bit.
	وإلا إذا (t_colors == 8 && fg && color->شاخص < 16)
	{
	    *boldp = TRUE;
	    أرجع (color->شاخص & 7) + 1;
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
    إذا (cellattrs->معکوس)
	attr |= HL_INVERSE;
    أرجع attr;
}

/*
 * Store Vterm attributes in "cell" from highlight flags.
 */
    ساكن فراغ
hl2vtermAttr(صحيح attr, cellattr_T *cell)
{
    CLEAR_فیلد(cell->attrs);
    إذا (attr & HL_BOLD)
	cell->attrs.bold = 1;
    إذا (attr & HL_UNDERLINE)
	cell->attrs.underline = 1;
    إذا (attr & HL_ITALIC)
	cell->attrs.italic = 1;
    إذا (attr & HL_STRIKETHROUGH)
	cell->attrs.strike = 1;
    إذا (attr & HL_INVERSE)
	cell->attrs.معکوس = 1;
}

/*
 * Convert the attributes of a vterm cell into an attribute index.
 */
    ساكن صحيح
cell2attr(
	جمله_T			*جمله,
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
		fg = &wp->w_جمله_hlfwin.fg;
	    إذا (is_default_bg)
		bg = &wp->w_جمله_hlfwin.bg;
	}
	وإلا
	{
	    إذا (is_default_fg)
		fg = &جمله->tl_default_color.fg;
	    إذا (is_default_bg)
		bg = &جمله->tl_default_color.bg;
	}
    }

#اگر_تعریف_شده FEAT_GUI
    إذا (gui.درون_use)
    {
	guicolor_T guifg = gui_mch_get_rgb_color(fg->red, fg->green, fg->blue);
	guicolor_T guibg = gui_mch_get_rgb_color(bg->red, bg->green, bg->blue);
	أرجع get_gui_attr_idx(attr, guifg, guibg);
    }
    وإلا
#پایان_اگر
#اگر_تعریف_شده FEAT_TERMGUICOLORS
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
#پایان_اگر
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
مجموعه_dirty_snapshot(جمله_T *جمله)
{
    جمله->tl_dirty_snapshot = TRUE;
#اگر_تعریف_شده FEAT_TIMERS
    إذا (!جمله->tl_نرمال_نما)
    {
	// Update the snapshot after 100 msec of not getting updates.
	profile_setlimit(100L, &جمله->tl_timer_due);
	جمله->tl_timer_مجموعه = TRUE;
    }
#پایان_اگر
}

    ساكن صحيح
handle_damage(VTermRect rect, فراغ *user)
{
    جمله_T *جمله = (جمله_T *)user;

    جمله->tl_dirty_ردیف_شروع = MIN(جمله->tl_dirty_ردیف_شروع, rect.شروع_ردیف);
    جمله->tl_dirty_ردیف_انتها = MAX(جمله->tl_dirty_ردیف_انتها, rect.انتها_ردیف);
    مجموعه_dirty_snapshot(جمله);
    redraw_buf_later(جمله->tl_میانگیر, UPD_SOME_VALID);
    أرجع 1;
}

    ساكن فراغ
جمله_scroll_به_بالا(جمله_T *جمله, صحيح شروع_ردیف, صحيح شمار)
{
    win_T		 *wp = عدم;
    صحيح			 did_curwin = FALSE;
    VTermColor		 fg, bg;
    VTermScreenCellAttrs attr;
    صحيح			 clear_attr;

    CLEAR_فیلد(attr);

    طالما (for_all_windows_and_curwin(&wp, &did_curwin))
    {
	إذا (wp->w_میانگیر == جمله->tl_میانگیر)
	{
	    // Set the color to clear lines with.
	    vterm_state_get_default_colors(vterm_obtain_state(جمله->tl_vterm),
								     &fg, &bg);
	    clear_attr = cell2attr(جمله, wp, &attr, &fg, &bg);
	    win_del_سطرها(wp, شروع_ردیف, شمار, FALSE, FALSE, clear_attr);
	}
    }
}

    ساكن صحيح
handle_moverect(VTermRect dest, VTermRect src, فراغ *user)
{
    جمله_T	*جمله = (جمله_T *)user;
    صحيح		شمار = src.شروع_ردیف - dest.شروع_ردیف;

    // Scrolling up is done much more efficiently by deleting lines instead of
    // redrawing the text. But avoid doing this multiple times, postpone until
    // the redraw happens.
    إذا (dest.شروع_ستون == src.شروع_ستون
	    && dest.انتها_ستون == src.انتها_ستون
	    && dest.شروع_ردیف < src.شروع_ردیف)
    {
	إذا (dest.شروع_ردیف == 0)
	    جمله->tl_postponed_scroll += شمار;
	وإلا
	    جمله_scroll_به_بالا(جمله, dest.شروع_ردیف, شمار);
    }

    جمله->tl_dirty_ردیف_شروع = MIN(جمله->tl_dirty_ردیف_شروع, dest.شروع_ردیف);
    جمله->tl_dirty_ردیف_انتها = MIN(جمله->tl_dirty_ردیف_انتها, dest.انتها_ردیف);
    مجموعه_dirty_snapshot(جمله);

    // Note sure if the scrolling will work correctly, let's do a complete
    // redraw later.
    redraw_buf_later(جمله->tl_میانگیر, UPD_NOT_VALID);
    أرجع 1;
}

    ساكن صحيح
handle_movecursor(
	VTermPos pos,
	VTermPos oldpos استفادهنشده,
	صحيح visible,
	فراغ *user)
{
    جمله_T	*جمله = (جمله_T *)user;
    win_T	*wp = عدم;
    صحيح		did_curwin = FALSE;

    جمله->tl_cursor_pos = pos;
    جمله->tl_cursor_visible = visible;

    طالما (for_all_windows_and_curwin(&wp, &did_curwin))
    {
	إذا (wp->w_میانگیر == جمله->tl_میانگیر)
	    position_cursor(wp, &pos);
    }
    إذا (جمله->tl_میانگیر == curbuf && !جمله->tl_نرمال_نما)
	// Don't redraw here, it will be done after
	// vterm_input_write() is finished.
	update_cursor(جمله, FALSE);

    أرجع 1;
}

    ساكن صحيح
handle_settermprop(
	VTermProp prop,
	VTerm_مقدار *مقدار,
	فراغ *user)
{
    جمله_T	*جمله = (جمله_T *)user;
    char_u	*strval = عدم;

    بدل (prop)
    {
	حالة VTERM_PROP_TITLE:
	    إذا (disable_vterm_title_for_testing)
		اكسر;
	    strval = vim_strnsave((char_u *)مقدار->string.str,
							    مقدار->string.len);
	    إذا (strval == عدم)
		اكسر;
	    vim_free(جمله->tl_title);
	    // a blank title isn't useful, make it empty, so that "running" is
	    // displayed
	    إذا (*skipwhite(strval) == NUL)
		جمله->tl_title = عدم;
	    // Same as blank
	    وإلا إذا (جمله->tl_arg0_cmd != عدم
		    && STRNCMP(جمله->tl_arg0_cmd, strval,
					  (صحيح)STRLEN(جمله->tl_arg0_cmd)) == 0)
		جمله->tl_title = عدم;
	    // Empty corrupted data of winpty
	    وإلا إذا (STRNCMP("  - ", strval, 4) == 0)
		جمله->tl_title = عدم;
#اگر_تعریف_شده MSWIN
	    وإلا إذا (!enc_utf8 && enc_codepage > 0)
	    {
		WCHAR   *ret = عدم;
		صحيح	درازی = 0;

		Multi_Byte_مقصد_Wide_Char_alloc(CP_UTF8, 0,
			(حرف*)مقدار->string.str,
					(صحيح)مقدار->string.len, &ret, &درازی);
		إذا (ret != عدم)
		{
		    Wide_Char_مقصد_Multi_Byte_alloc(enc_codepage, 0,
					ret, درازی, (حرف**)&جمله->tl_title,
					&درازی, 0, 0);
		    vim_free(ret);
		}
	    }
#پایان_اگر
	    وإلا
	    {
		جمله->tl_title = strval;
		strval = عدم;
	    }
	    VIM_CLEAR(جمله->tl_وضعیت_متن);
	    إذا (جمله == curbuf->b_جمله)
	    {
		maketitle();
		curwin->w_redr_وضعیت = true;
	    }
	    اكسر;

	حالة VTERM_PROP_CURSORVISIBLE:
	    جمله->tl_cursor_visible = مقدار->boolean;
	    may_toggle_cursor(جمله);
	    بیرون_flush();
	    اكسر;

	حالة VTERM_PROP_CURSORBLINK:
	    جمله->tl_cursor_blink = مقدار->boolean;
	    may_مجموعه_cursor_props(جمله);
	    اكسر;

	حالة VTERM_PROP_CURSORSHAPE:
	    جمله->tl_cursor_shape = مقدار->عدد;
	    may_مجموعه_cursor_props(جمله);
	    اكسر;

	حالة VTERM_PROP_CURSORCOLOR:
	    strval = vim_strnsave((char_u *)مقدار->string.str,
							    مقدار->string.len);
	    إذا (strval == عدم)
		اكسر;
	    cursor_color_copy(&جمله->tl_cursor_color, strval);
	    may_مجموعه_cursor_props(جمله);
	    اكسر;

	حالة VTERM_PROP_ALTSCREEN:
	    // TODO: do anything else?
	    جمله->tl_using_altscreen = مقدار->boolean;
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
handle_resize(صحيح ردیفها_2, صحيح تعداد_ستون, فراغ *user)
{
    جمله_T	*جمله = (جمله_T *)user;
    win_T	*wp;

    جمله->tl_ردیفها = ردیفها_2;
    جمله->tl_تعداد_ستون = تعداد_ستون;
    إذا (جمله->tl_vterm_اندازه_تغییریافته)
	// Size was set by vterm_set_size(), don't set the window size.
	جمله->tl_vterm_اندازه_تغییریافته = FALSE;
    وإلا
    {
	FOR_ALL_WINDOWS(wp)
	{
	    إذا (wp->w_میانگیر == جمله->tl_میانگیر)
	    {
		win_setheight_win(ردیفها_2, wp);
		win_setwidth_win(تعداد_ستون, wp);
	    }
	}
	redraw_buf_later(جمله->tl_میانگیر, UPD_NOT_VALID);
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
حد_scrollback(جمله_T *جمله, garray_T *gap, صحيح update_میانگیر)
{
    إذا (gap->ga_len < جمله->tl_میانگیر->b_p_twsl)
	أرجع;

    صحيح	todo = MAX(جمله->tl_میانگیر->b_p_twsl / 10,
				     gap->ga_len - جمله->tl_میانگیر->b_p_twsl);
    صحيح	i;
    sb_سطر_T *sb_سطرها = (sb_سطر_T *)gap->ga_داده;

    curbuf = جمله->tl_میانگیر;
    لكل (i = 0; i < todo; ++i)
    {
	إذا (update_میانگیر && (!sb_سطرها[i].continuation || !i))
	{
	    ml_delete(1);
	    --جمله->tl_میانگیر_scrolled;
	}
	vim_free(sb_سطرها[i].sb_cells);
    }
    // Continue until end of wrapped line
    لكل (; todo < gap->ga_len && sb_سطرها[todo].continuation; ++todo)
	vim_free(sb_سطرها[todo].sb_cells);
    curbuf = curwin->w_میانگیر;

    gap->ga_len -= todo;
    mch_memmove(gap->ga_داده,
		(sb_سطر_T *)gap->ga_داده + todo,
		حجم(sb_سطر_T) * gap->ga_len);
    إذا (update_میانگیر)
    {
	win_T *curwin_save = curwin;
	win_T *wp = عدم;

	جمله->tl_scrollback_scrolled -= todo;

	FOR_ALL_WINDOWS(wp)
	{
	    إذا (wp->w_میانگیر == جمله->tl_میانگیر)
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
handle_pushline(صحيح تعداد_ستون, ثابت VTermScreenCell *cells, صحيح continuation, فراغ *user)
{
    جمله_T	*جمله = (جمله_T *)user;
    garray_T	*gap;
    صحيح		update_میانگیر;

    إذا (جمله->tl_نرمال_نما)
    {
	// In Terminal-Normal mode the user interacts with the buffer, thus we
	// must not change it. Postpone adding the scrollback lines.
	gap = &جمله->tl_scrollback_postponed;
	update_میانگیر = FALSE;
    }
    وإلا
    {
	// First remove the lines that were appended before, the pushed line
	// goes above it.
	cleanup_scrollback(جمله);
	gap = &جمله->tl_scrollback;
	update_میانگیر = TRUE;
    }

    حد_scrollback(جمله, gap, update_میانگیر);

    إذا (ga_grow(gap, 1) == FAIL)
	أرجع 0;

    cellattr_T	*p = عدم;
    صحيح		len = 0;
    صحيح		i;
    صحيح		c;
    صحيح		ستون;
    صحيح		متن_len;
    char_u		*متن;
    sb_سطر_T	*سطر;
    garray_T	ga;
    cellattr_T	fill_attr = جمله->tl_default_color;

    // do not store empty cells at the end
    لكل (i = 0; i < تعداد_ستون; ++i)
	إذا (cells[i].chars[0] != 0)
	    len = i + 1;
	وإلا
	    cell2cellattr(&cells[i], &fill_attr);

    ga_init2(&ga, 1, 100);
    إذا (len > 0)
	p = ALLOC_MULT(cellattr_T, len);
    إذا (p != عدم)
    {
	لكل (ستون = 0; ستون < len; ستون += cells[ستون].عرض)
	{
	    إذا (ga_grow(&ga, VTERM_MAX_CHARS_در_هر_CELL * 4) == FAIL)
	    {
		ga.ga_len = 0;
		اكسر;
	    }
	    لكل (i = 0; i < VTERM_MAX_CHARS_در_هر_CELL &&
		    ((c = cells[ستون].chars[i]) > 0 || i == 0); ++i)
		ga.ga_len += utf_char2bytes(c == NUL ? ' ' : c,
			(char_u *)ga.ga_داده + ga.ga_len);
	    cell2cellattr(&cells[ستون], &p[ستون]);
	}
    }
    إذا (ga_grow(&ga, 1) == FAIL)
    {
	إذا (update_میانگیر)
	    متن = (char_u *)"";
	وإلا
	    متن = vim_strsave((char_u *)"");
	متن_len = 0;
    }
    وإلا
    {
	متن = ga.ga_داده;
	متن_len = ga.ga_len;
	*(متن + متن_len) = NUL;
    }
    إذا (update_میانگیر)
	جمع_کن_scrollback_سطر_مقصد_میانگیر(جمله, متن, متن_len, continuation);

    سطر = (sb_سطر_T *)gap->ga_داده + gap->ga_len;
    سطر->sb_تعداد_ستون = len;
    سطر->sb_بایتها = متن_len;
    سطر->sb_cells = p;
    سطر->sb_fill_attr = fill_attr;
    سطر->continuation = (char_u)continuation;
    إذا (update_میانگیر)
    {
	سطر->sb_متن = عدم;
	++جمله->tl_scrollback_scrolled;
	إذا (!continuation)
	    ++جمله->tl_میانگیر_scrolled;
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
handle_postponed_scrollback(جمله_T *جمله)
{
    صحيح i;

    إذا (جمله->tl_scrollback_postponed.ga_len == 0)
	أرجع;
    ch_log(عدم, "Moving postponed scrollback to scrollback");

    // First remove the lines that were appended before, the pushed lines go
    // above it.
    cleanup_scrollback(جمله);

    لكل (i = 0; i < جمله->tl_scrollback_postponed.ga_len; ++i)
    {
	char_u		*متن;
	sb_سطر_T	*pp_سطر;
	sb_سطر_T	*سطر;

	إذا (ga_grow(&جمله->tl_scrollback, 1) == FAIL)
	    اكسر;
	pp_سطر = (sb_سطر_T *)جمله->tl_scrollback_postponed.ga_داده + i;

	متن = pp_سطر->sb_متن;
	إذا (متن == عدم)
	    متن = (char_u *)"";
	جمع_کن_scrollback_سطر_مقصد_میانگیر(جمله, متن, (صحيح)STRLEN(متن), pp_سطر->continuation);
	vim_free(pp_سطر->sb_متن);

	سطر = (sb_سطر_T *)جمله->tl_scrollback.ga_داده
						 + جمله->tl_scrollback.ga_len;
	سطر->sb_تعداد_ستون = pp_سطر->sb_تعداد_ستون;
	سطر->sb_بایتها = pp_سطر->sb_بایتها;
	سطر->sb_cells = pp_سطر->sb_cells;
	سطر->sb_fill_attr = pp_سطر->sb_fill_attr;
	سطر->sb_متن = عدم;
	++جمله->tl_scrollback_scrolled;
	++جمله->tl_scrollback.ga_len;
	إذا (!pp_سطر->continuation)
	    ++جمله->tl_میانگیر_scrolled;
    }

    ga_clear(&جمله->tl_scrollback_postponed);
    حد_scrollback(جمله, &جمله->tl_scrollback, TRUE);
}

/*
 * Called when the terminal wants to ring the system bell.
 */
    ساكن صحيح
handle_bell(فراغ *user استفادهنشده)
{
    vim_beep(BO_جمله);
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
جمله_after_channel_closed(جمله_T *جمله)
{
    // Unless in Terminal-Normal mode: clear the vterm.
    إذا (!جمله->tl_نرمال_نما)
    {
	صحيح	fnum = جمله->tl_میانگیر->b_fnum;

	cleanup_vterm(جمله);

	إذا (جمله->tl_پایان == TL_پایان_CLOSE)
	{
	    aco_save_T	aco;
	    صحيح		do_مجموعه_w_locked = جمله->tl_میانگیر->b_nwindows == 0;
#اگر_تعریف_شده FEAT_PROP_POPUP
	    win_T	*pwin = عدم;

	    // If this was a terminal in a popup window, go back to the
	    // previous window.
	    إذا (popup_is_popup(curwin) && curbuf == جمله->tl_میانگیر)
	    {
		pwin = curwin;
		إذا (win_valid(prevwin))
		    win_enter(prevwin, FALSE);
	    }
	    وإلا
#پایان_اگر
	    // If this is the last normal window: exit Vim.
	    إذا (جمله->tl_میانگیر->b_nwindows > 0 && only_one_پنجره())
	    {
		exarg_T ea;

		CLEAR_فیلد(ea);
		ex_quit(&ea);
		أرجع TRUE;
	    }

	    // ++close or term_finish == "close"
	    ch_log(عدم, "terminal job finished, closing window");
	    aucmd_prepbuf(&aco, جمله->tl_میانگیر);
	    إذا (curbuf == جمله->tl_میانگیر)
	    {
		win_T	*wp = curwin;

		// Avoid closing the window if we temporarily use it.
		إذا (is_aucmd_win(wp))
		    do_مجموعه_w_locked = TRUE;
		إذا (do_مجموعه_w_locked)
		    ++wp->w_locked;
		do_bufdel(DOBUF_WIPE, (char_u *)"", 1, fnum, fnum, FALSE);
		إذا (do_مجموعه_w_locked)
		    --wp->w_locked;
		aucmd_restbuf(&aco);
	    }
#اگر_تعریف_شده FEAT_PROP_POPUP
	    إذا (pwin != عدم)
		popup_close_with_retval(pwin, 0);
#پایان_اگر
	    أرجع TRUE;
	}
	إذا (جمله->tl_پایان == TL_پایان_OPEN
				   && جمله->tl_میانگیر->b_nwindows == 0)
	{
	    حرف    *cmd = جمله->tl_opencmd == عدم
				? "botright sbuf %d"
				: (حرف *)جمله->tl_opencmd;
	    size_t  len = طول_رشته(cmd) + 50;
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

    redraw_buf_and_وضعیت_later(جمله->tl_میانگیر, UPD_NOT_VALID);
    أرجع FALSE;
}

#إذا defined(FEAT_PROP_POPUP)
/*
 * If the current window is a terminal in a popup window and the job has
 * finished, close the popup window and to back to the previous window.
 * Otherwise return FAIL.
 */
    صحيح
may_close_جمله_popup(فراغ)
{
    إذا (!popup_is_popup(curwin) || curbuf->b_جمله == عدم
				 || جمله_کار_running_not_none(curbuf->b_جمله))
	أرجع FAIL;

    win_T *pwin = curwin;

    إذا (win_valid(prevwin))
	win_enter(prevwin, FALSE);
    popup_close_with_retval(pwin, 0);
    أرجع OK;
}
#پایان_اگر

/*
 * Called when a channel is going to be closed, before invoking the close
 * callback.
 */
    فراغ
جمله_channel_closing(channel_T *ch)
{
    جمله_T *جمله;

    لكل (جمله = اولی_جمله; جمله != عدم; جمله = جمله->tl_بعدی)
	إذا (جمله->tl_کار == ch->ch_کار && !جمله->tl_channel_closed)
	    جمله->tl_channel_closing = TRUE;
}

/*
 * Called when a channel has been closed.
 * If this was a channel for a terminal window then finish it up.
 */
    فراغ
جمله_channel_closed(channel_T *ch)
{
    جمله_T *جمله;
    جمله_T *بعدی_جمله;
    صحيح	    did_one = FALSE;

    لكل (جمله = اولی_جمله; جمله != عدم; جمله = بعدی_جمله)
    {
	بعدی_جمله = جمله->tl_بعدی;
	إذا (جمله->tl_کار == ch->ch_کار && !جمله->tl_channel_closed)
	{
	    جمله->tl_channel_closed = TRUE;
	    did_one = TRUE;

	    VIM_CLEAR(جمله->tl_title);
	    VIM_CLEAR(جمله->tl_وضعیت_متن);
#اگر_تعریف_شده MSWIN
	    إذا (جمله->tl_بیرون_fd != عدم)
	    {
		fclose(جمله->tl_بیرون_fd);
		جمله->tl_بیرون_fd = عدم;
	    }
#پایان_اگر

	    إذا (updating_screen)
	    {
		// Cannot open or close windows now.  Can happen when
		// 'lazyredraw' is set.
		جمله->tl_channel_recently_closed = TRUE;
		استمر;
	    }

	    إذا (جمله_after_channel_closed(جمله))
		بعدی_جمله = اولی_جمله;
	}
    }

    إذا (did_one)
    {
	redraw_statuslines();

	// Need to break out of vgetc().
	ins_char_typebuf(K_IGNORE, 0);
	typebuf_was_filled = TRUE;

	جمله = curbuf->b_جمله;
	إذا (جمله != عدم)
	{
	    إذا (جمله->tl_کار == ch->ch_کار)
		maketitle();
	    update_cursor(جمله, جمله->tl_cursor_visible);
	}
    }
}

/*
 * To be called after resetting updating_screen: handle any terminal where the
 * channel was closed.
 */
    فراغ
جمله_check_channel_closed_recently(فراغ)
{
    جمله_T *جمله;
    جمله_T *بعدی_جمله;

    لكل (جمله = اولی_جمله; جمله != عدم; جمله = بعدی_جمله)
    {
	بعدی_جمله = جمله->tl_بعدی;
	إذا (جمله->tl_channel_recently_closed)
	{
	    جمله->tl_channel_recently_closed = FALSE;
	    إذا (جمله_after_channel_closed(جمله))
		// start over, the list may have changed
		بعدی_جمله = اولی_جمله;
	}
    }
}

/*
 * Fill one screen line from a line of the terminal.
 * Advances "pos" to past the last column.
 */
    ساكن فراغ
جمله_line2screenline(
	جمله_T		*جمله,
	win_T		*wp,
	VTermScreen	*screen,
	VTermPos	*pos,
	صحيح		max_ستون)
{
    صحيح off = screen_get_current_سطر_off();

    لكل (pos->ستون = 0; pos->ستون < max_ستون; )
    {
	VTermScreenCell cell;
	صحيح		c;

	إذا (vterm_screen_get_cell(screen, *pos, &cell) == 0)
	    CLEAR_فیلد(cell);

	c = cell.chars[0];
	إذا (c == NUL)
	{
	    Screen_سطرها[off] = ' ';
	    إذا (enc_utf8)
		Screen_سطرها_UC[off] = NUL;
	}
	وإلا
	{
	    إذا (enc_utf8)
	    {
		صحيح i;

		// composing chars
		لكل (i = 0; i < Screen_mco
			      && i + 1 < VTERM_MAX_CHARS_در_هر_CELL; ++i)
		{
		    Screen_سطرها_C[i][off] = cell.chars[i + 1];
		    إذا (cell.chars[i + 1] == 0)
			اكسر;
		}
		إذا (c >= 0x80 || (Screen_mco > 0
					 && Screen_سطرها_C[0][off] != 0))
		{
		    Screen_سطرها[off] = ' ';
		    Screen_سطرها_UC[off] = c;
		}
		وإلا
		{
		    Screen_سطرها[off] = c;
		    Screen_سطرها_UC[off] = NUL;
		}
	    }
#اگر_تعریف_شده MSWIN
	    وإلا إذا (has_mbyte && c >= 0x80)
	    {
		char_u	mb[MB_MAXBYTES+1];
		WCHAR	wc = c;

		إذا (Wide_Char_مقصد_Multi_Byte(GetACP(), 0, &wc, 1,
					       (حرف*)mb, 2, 0, 0) > 1)
		{
		    Screen_سطرها[off] = mb[0];
		    Screen_سطرها[off + 1] = mb[1];
		    cell.عرض = mb_ptr2cells(mb);
		}
		وإلا
		    Screen_سطرها[off] = c;
	    }
#پایان_اگر
	    وإلا
		// This will only store the lower byte of "c".
		Screen_سطرها[off] = c;
	}
	ScreenAttrs[off] = cell2attr(جمله, wp, &cell.attrs, &cell.fg,
								     &cell.bg);

	++pos->ستون;
	++off;
	إذا (cell.عرض == 2)
	{
	    // don't set the second byte to NUL for a DBCS encoding, it
	    // has been set above
	    إذا (enc_utf8)
	    {
		Screen_سطرها_UC[off] = NUL;
		Screen_سطرها[off] = NUL;
	    }
	    وإلا إذا (!has_mbyte)
	    {
		// Can't show a double-width character with a single-byte
		// 'encoding', just use a space.
		Screen_سطرها[off] = ' ';
		ScreenAttrs[off] = ScreenAttrs[off - 1];
	    }

	    ++pos->ستون;
	    ++off;
	}
    }
}

#إذا defined(FEAT_GUI)
    ساكن فراغ
update_system_جمله(جمله_T *جمله)
{
    VTermPos	    pos;
    VTermScreen	    *screen;

    إذا (جمله->tl_vterm == عدم)
	أرجع;
    screen = vterm_obtain_screen(جمله->tl_vterm);

    // Scroll up to make more room for terminal lines if needed.
    // Use the cursor position to determine how much to scroll, because
    // ConPTY may damage all rows on initialization even when most are
    // empty, which would cause unnecessary scrolling.
    طالما (جمله->tl_toprow > 0
		  && (ردیفها - جمله->tl_toprow) < جمله->tl_cursor_pos.ردیف + 1)
    {
	صحيح save_p_بیشتر = p_بیشتر;

	p_بیشتر = FALSE;
	پیام_ردیف = ردیفها - 1;
	پیام_puts("\n");
	p_بیشتر = save_p_بیشتر;
	--جمله->tl_toprow;
    }

    لكل (pos.ردیف = جمله->tl_dirty_ردیف_شروع; pos.ردیف < جمله->tl_dirty_ردیف_انتها
						  && pos.ردیف < ردیفها; ++pos.ردیف)
    {
	إذا (pos.ردیف < جمله->tl_ردیفها)
	{
	    صحيح max_ستون = MIN(topframe->fr_عرض, جمله->tl_تعداد_ستون);

	    جمله_line2screenline(جمله, عدم, screen, &pos, max_ستون);
	}
	وإلا
	    pos.ستون = 0;

	screen_سطر(curwin, جمله->tl_toprow + pos.ردیف,
		firstwin->w_wincol, pos.ستون, topframe->fr_عرض, -1, 0);
    }

    جمله->tl_dirty_ردیف_شروع = MAX_ردیف;
    جمله->tl_dirty_ردیف_انتها = 0;
}
#پایان_اگر

/*
 * Return TRUE if window "wp" is to be redrawn with term_update_window().
 * Returns FALSE when there is no terminal running in this window or it is in
 * Terminal-Normal mode.
 */
    صحيح
جمله_do_update_پنجره(win_T *wp)
{
    جمله_T	*جمله = wp->w_میانگیر->b_جمله;

    أرجع جمله != عدم && جمله->tl_vterm != عدم && !جمله->tl_نرمال_نما;
}

/*
 * Called to update a window that contains an active terminal.
 */
    فراغ
جمله_update_پنجره(win_T *wp)
{
    جمله_T	*جمله = wp->w_میانگیر->b_جمله;
    VTerm	*vterm;
    VTermScreen *screen;
    VTermState	*state;
    VTermPos	pos;
    صحيح		ردیفها_2, تعداد_ستون;
    صحيح		newrows, newcols;
    صحيح		minsize;
    win_T	*twp;

    vterm = جمله->tl_vterm;
    screen = vterm_obtain_screen(vterm);
    state = vterm_obtain_state(vterm);

    // We use UPD_NOT_VALID on a resize or scroll, redraw everything then.
    // With UPD_SOME_VALID only redraw what was marked dirty.
    إذا (wp->w_redr_type > UPD_SOME_VALID)
    {
	جمله->tl_dirty_ردیف_شروع = 0;
	جمله->tl_dirty_ردیف_انتها = MAX_ردیف;

	إذا (جمله->tl_postponed_scroll > 0
			      && جمله->tl_postponed_scroll < جمله->tl_ردیفها / 3)
	    // Scrolling is usually faster than redrawing, when there are only
	    // a few lines to scroll.
	    جمله_scroll_به_بالا(جمله, 0, جمله->tl_postponed_scroll);
	جمله->tl_postponed_scroll = 0;
    }

    /*
     * If the window was resized a redraw will be triggered and we get here.
     * Adjust the size of the vterm unless 'termwinsize' specifies a fixed size.
     */
    minsize = parse_termwinsize(wp, &ردیفها_2, &تعداد_ستون);

    newrows = 99999;
    newcols = 99999;
    لكل (twp = firstwin; ; twp = twp->w_بعدی)
    {
	// Always use curwin, it may be a popup window.
	win_T *wwp = twp == عدم ? curwin : twp;

	// When more than one window shows the same terminal, use the
	// smallest size.
	إذا (wwp->w_میانگیر == جمله->tl_میانگیر)
	{
	    newrows = MIN(newrows, wwp->w_ارتفاع);
	    newcols = MIN(newcols, wwp->w_عرض);
	}
	إذا (twp == عدم)
	    اكسر;
    }
    إذا (newrows == 99999 || newcols == 99999)
	أرجع; // safety exit
    newrows = ردیفها_2 == 0 ? newrows : minsize ? MAX(ردیفها_2, newrows) : ردیفها_2;
    newcols = تعداد_ستون == 0 ? newcols : minsize ? MAX(تعداد_ستون, newcols) : تعداد_ستون;

    // If no cell is visible there is no point in resizing.  Also, vterm can't
    // handle a zero height.
    إذا (newrows == 0 || newcols == 0)
	أرجع;

    إذا (جمله->tl_ردیفها != newrows || جمله->tl_تعداد_ستون != newcols)
    {
	جمله->tl_vterm_اندازه_تغییریافته = TRUE;
	vterm_مجموعه_اندازه(vterm, newrows, newcols);
	ch_log(جمله->tl_کار->jv_channel, "Resizing terminal to %d lines",
								      newrows);
	جمله_report_winsize(جمله, newrows, newcols);

	// Updating the terminal size will cause the snapshot to be cleared.
	// When not in terminal_loop() we need to restore it.
	إذا (جمله != درون_terminal_loop)
	    may_move_terminal_مقصد_میانگیر(جمله, FALSE);
    }

    // The cursor may have been moved when resizing.
    vterm_state_get_cursorpos(state, &pos);
    position_cursor(wp, &pos);

    لكل (pos.ردیف = جمله->tl_dirty_ردیف_شروع; pos.ردیف < جمله->tl_dirty_ردیف_انتها
					  && pos.ردیف < wp->w_ارتفاع; ++pos.ردیف)
    {
	إذا (pos.ردیف < جمله->tl_ردیفها)
	{
	    صحيح max_ستون = MIN(wp->w_عرض, جمله->tl_تعداد_ستون);

	    جمله_line2screenline(جمله, wp, screen, &pos, max_ستون);
	}
	وإلا
	    pos.ستون = 0;

	screen_سطر(wp, wp->w_winrow + pos.ردیف
#اگر_تعریف_شده FEAT_MENU
				+ winbar_ارتفاع(wp)
#پایان_اگر
				, wp->w_wincol, pos.ستون, wp->w_عرض, -1,
#اگر_تعریف_شده FEAT_PROP_POPUP
				popup_is_popup(wp) ? SLF_POPUP :
#پایان_اگر
				0);
    }
}

/*
 * Called after updating all windows: may reset dirty rows.
 */
    فراغ
جمله_did_update_پنجره(win_T *wp)
{
    جمله_T	*جمله = wp->w_میانگیر->b_جمله;

    إذا (جمله == عدم || جمله->tl_vterm == عدم || جمله->tl_نرمال_نما
						|| wp->w_redr_type != 0)
	أرجع;

    جمله->tl_dirty_ردیف_شروع = MAX_ردیف;
    جمله->tl_dirty_ردیف_انتها = 0;
}

/*
 * Return TRUE if "wp" is a terminal window where the job has finished.
 */
    صحيح
جمله_is_پایانیافته(buf_T *buf)
{
    أرجع buf->b_جمله != عدم && buf->b_جمله->tl_vterm == عدم;
}

/*
 * Return TRUE if "wp" is a terminal window where the job has finished or we
 * are in Terminal-Normal mode, thus we show the buffer contents.
 */
    صحيح
جمله_show_میانگیر(buf_T *buf)
{
    جمله_T *جمله = buf->b_جمله;

    أرجع جمله != عدم && (جمله->tl_vterm == عدم || جمله->tl_نرمال_نما);
}

/*
 * The current buffer is going to be changed.  If there is terminal
 * highlighting remove it now.
 */
    فراغ
جمله_change_درون_curbuf(فراغ)
{
    جمله_T *جمله = curbuf->b_جمله;

    إذا (!جمله_is_پایانیافته(curbuf) || جمله->tl_scrollback.ga_len <= 0)
	أرجع;

    free_scrollback(جمله);
    redraw_buf_later(جمله->tl_میانگیر, UPD_NOT_VALID);

    // The buffer is now like a normal buffer, it cannot be easily
    // abandoned when changed.
    مجموعه_string_option_مستقیم((char_u *)"buftype", -1,
	    (char_u *)"", OPT_FREE|OPT_LOCAL, 0);
}

/*
 * Get the screen attribute for a position in the buffer.
 * Use a negative "col" to get the filler background color.
 */
    صحيح
جمله_get_attr(win_T *wp, linenr_T lnum, صحيح ستون)
{
    buf_T	*buf = wp->w_میانگیر;
    جمله_T	*جمله = buf->b_جمله;
    sb_سطر_T	*سطر;
    cellattr_T	*cellattr;
    صحيح         sb_سطر = -1;
    صحيح         sb_ستون = ستون;

    إذا (جمله->tl_scrollback.ga_len)
	bufline_pos_درون_scrollback(جمله, lnum, ستون, &sb_سطر, &sb_ستون);

    إذا (sb_سطر < 0)
	cellattr = &جمله->tl_default_color;
    وإلا
    {
	سطر = (sb_سطر_T *)جمله->tl_scrollback.ga_داده + sb_سطر;
	إذا (sb_ستون < 0 || sb_ستون >= سطر->sb_تعداد_ستون)
	    cellattr = &سطر->sb_fill_attr;
	وإلا
	    cellattr = سطر->sb_cells + sb_ستون;
    }
    أرجع cell2attr(جمله, wp, &cellattr->attrs, &cellattr->fg, &cellattr->bg);
}

/*
 * Convert a cterm color number 0 - 255 to RGB.
 * This is compatible with xterm.
 */
    ساكن فراغ
cterm_color2vterm(صحيح nr, VTermColor *rgb)
{
    cterm_color2rgb(nr, &rgb->red, &rgb->green, &rgb->blue, &rgb->شاخص);
    إذا (rgb->شاخص == 0)
	rgb->type = VTERM_COLOR_RGB;
    وإلا
    {
	rgb->type = VTERM_COLOR_نمایهای;
	--rgb->شاخص;
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
	    || gui.درون_use
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
	    fg->red = (بیعلامت)(rgb >> 16);
	    fg->green = (بیعلامت)(rgb >> 8) & 255;
	    fg->blue = (بیعلامت)rgb & 255;
	    fg->type = VTERM_COLOR_RGB | VTERM_COLOR_DEFAULT_FG;
	}
	وإلا
	    fg->type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_FG;

	إذا (bg_rgb != INVALCOLOR)
	{
	    long_u rgb = GUI_MCH_GET_RGB(bg_rgb);
	    bg->red = (بیعلامت)(rgb >> 16);
	    bg->green = (بیعلامت)(rgb >> 8) & 255;
	    bg->blue = (بیعلامت)rgb & 255;
	    bg->type = VTERM_COLOR_RGB | VTERM_COLOR_DEFAULT_BG;
	}
	وإلا
	    bg->type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_BG;

	أرجع TRUE;
    }
    وإلا
#پایان_اگر
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
جمله_reset_hlfwin(win_T *wp)
{
    wp->w_جمله_hlfwin.fg.type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_FG;
    wp->w_جمله_hlfwin.bg.type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_BG;
}

/*
 * Cache the color of HLF_WIN.
 */
    فراغ
جمله_update_hlfwin(win_T *wp)
{
    صحيح id = wp->w_hlfwin_id;

    إذا (id == 0 || !get_vterm_color_from_synid(id == -1 ? 0 : id,
		&wp->w_جمله_hlfwin.fg, &wp->w_جمله_hlfwin.bg))
	جمله_reset_hlfwin(wp);
}

/*
 * Called when option 'termguicolors' was set,
 * or when any highlight is changed.
 */
    فراغ
جمله_update_hlfwin_all(فراغ)
{
    win_T	 *wp = عدم;
    صحيح		 did_curwin = FALSE;

    طالما (for_all_windows_and_curwin(&wp, &did_curwin))
	جمله_update_hlfwin(wp);
}

/*
 * Initialize term->tl_default_color from the environment.
 */
    فراغ
جمله_init_default_colors(جمله_T *جمله)
{
    VTermColor	    *fg, *bg;
    صحيح		    fgval, bgval;
    صحيح		    id;

    CLEAR_فیلد(جمله->tl_default_color.attrs);
    جمله->tl_default_color.عرض = 1;
    fg = &جمله->tl_default_color.fg;
    bg = &جمله->tl_default_color.bg;

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
    id = جمله_get_highlight_id(جمله, عدم);

    إذا (!get_vterm_color_from_synid(id, fg, bg))
    {
#إذا defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
	صحيح tmp;
#پایان_اگر

	// In an MS-Windows console we know the normal colors.
	إذا (cterm_نرمال_fg_color > 0)
	{
	    cterm_color2vterm(cterm_نرمال_fg_color - 1, fg);
#إذا defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
# ifdef VIMDLL
	    إذا (!gui.درون_use)
# endif
	    {
		tmp = fg->red;
		fg->red = fg->blue;
		fg->blue = tmp;
	    }
#پایان_اگر
	}
#اگر_تعریف_شده FEAT_TERMRESPONSE
	وإلا
	    جمله_get_fg_color(&fg->red, &fg->green, &fg->blue);
#پایان_اگر

	إذا (cterm_نرمال_bg_color > 0)
	{
	    cterm_color2vterm(cterm_نرمال_bg_color - 1, bg);
#إذا defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
# ifdef VIMDLL
	    إذا (!gui.درون_use)
# endif
	    {
		tmp = fg->red;
		fg->red = fg->blue;
		fg->blue = tmp;
	    }
#پایان_اگر
	}
#اگر_تعریف_شده FEAT_TERMRESPONSE
	وإلا
	    جمله_get_bg_color(&bg->red, &bg->green, &bg->blue);
#پایان_اگر
    }
}

#إذا defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Return TRUE if the user-defined palette (either g:terminal_ansi_colors or the
 * "ansi_colors" argument in term_start()) shall be applied.
 */
    ساكن صحيح
جمله_use_palette(فراغ)
{
    إذا (0
# ifdef FEAT_GUI
	    || gui.درون_use
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
مجموعه_vterm_palette(VTerm *vterm, long_u *rgb)
{
    صحيح		شاخص = 0;
    VTermState	*state = vterm_obtain_state(vterm);

    لكل (; شاخص < 16; شاخص++)
    {
	VTermColor	color;

	color.type = VTERM_COLOR_RGB;
	color.red = (بیعلامت)(rgb[شاخص] >> 16);
	color.green = (بیعلامت)(rgb[شاخص] >> 8) & 255;
	color.blue = (بیعلامت)rgb[شاخص] & 255;
	color.شاخص = 0;
	vterm_state_مجموعه_palette_color(state, شاخص, &color);
    }
}

/*
 * Set the ANSI color palette from a list of colors
 */
    ساكن صحيح
مجموعه_ansi_colors_list(VTerm *vterm, list_T *list)
{
    صحيح		n = 0;
    long_u	rgb[16];
    listitem_T	*li;

    لكل (li = list->lv_اولی; li != عدم && n < 16; li = li->li_بعدی, n++)
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

    مجموعه_vterm_palette(vterm, rgb);

    أرجع OK;
}

/*
 * Initialize the ANSI color palette from g:terminal_ansi_colors[0:15]
 */
    ساكن فراغ
init_vterm_ansi_colors(VTerm *vterm)
{
    dictitem_T	*var = پیدا_کن_var((char_u *)"g:terminal_ansi_colors", عدم, TRUE);

    إذا (var == عدم)
	أرجع;

    إذا (var->di_tv.v_type != VAR_LIST
	    || var->di_tv.vval.v_list == عدم
	    || var->di_tv.vval.v_list->lv_اولی == &range_list_قلم
	    || مجموعه_ansi_colors_list(vterm, var->di_tv.vval.v_list) == FAIL)
	semsg(_(e_invalid_آرگومان_str), "g:terminal_ansi_colors");
}
#پایان_اگر

/*
 * Handles a "drop" command from the job in the terminal.
 * "item" is the file name, "item->li_next" may have options.
 */
    ساكن فراغ
handle_drop_command(listitem_T *قلم)
{
    char_u	*fname = tv_get_string(&قلم->li_tv);
    listitem_T	*opt_قلم = قلم->li_بعدی;
    صحيح		bufnr;
    win_T	*wp;
    tabpage_T   *tp;
    exarg_T	ea;
    char_u	*tofree = عدم;

    bufnr = buflist_جمع_کن(fname, BLN_LISTED | BLN_NOOPT);
    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	إذا (wp->w_میانگیر->b_fnum == bufnr)
	{
	    // buffer is in a window already, go there
	    goto_tabpage_win(tp, wp);
	    أرجع;
	}
    }

    CLEAR_فیلد(ea);

    إذا (opt_قلم != عدم && opt_قلم->li_tv.v_type == VAR_DICT
					&& opt_قلم->li_tv.vval.v_dict != عدم)
    {
	dict_T *dict = opt_قلم->li_tv.vval.v_dict;
	char_u *p;

	p = dict_get_string(dict, "ff", FALSE);
	إذا (p == عدم)
	    p = dict_get_string(dict, "fileformat", FALSE);
	إذا (p != عدم)
	{
	    إذا (check_ff_مقدار(p) == FAIL)
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
    ea.cmdidx = CMD_تقسیم;
    ex_splitview(&ea);

    vim_free(tofree);
}

/*
 * Return TRUE if "func" starts with "pat" and "pat" isn't empty.
 */
    ساكن صحيح
is_permitted_جمله_api(char_u *func, char_u *pat)
{
    أرجع pat != عدم && *pat != NUL && STRNICMP(func, pat, STRLEN(pat)) == 0;
}

/*
 * Handles a function call from the job running in a terminal.
 * "item" is the function name, "item->li_next" has the arguments.
 */
    ساكن فراغ
handle_call_command(جمله_T *جمله, channel_T *channel, listitem_T *قلم)
{
    char_u	*func;
    typval_T	argvars[2];
    typval_T	rettv;
    funcexe_T	funcexe;

    إذا (قلم->li_بعدی == عدم)
    {
	ch_log(channel, "Missing function arguments for call");
	أرجع;
    }
    func = tv_get_string(&قلم->li_tv);

    إذا (!is_permitted_جمله_api(func, جمله->tl_api))
    {
	ch_log(channel, "Unpermitted function: %s", func);
	أرجع;
    }

    argvars[0].v_type = VAR_عدد;
    argvars[0].vval.v_عدد = جمله->tl_میانگیر->b_fnum;
    argvars[1] = قلم->li_بعدی->li_tv;
    CLEAR_فیلد(funcexe);
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
    صحيح       جابه_جایی = 7;  // len of "file://" is 7
    حرف      *pos = (حرف *)gap->ga_داده + جابه_جایی;
    char_u    *new_dir;

    // remove HOSTNAME to get PWD
    طالما (جابه_جایی < (صحيح)gap->ga_len && *pos != '/' )
    {
	++جابه_جایی;
	++pos;
    }

    إذا (جابه_جایی >= (صحيح)gap->ga_len)
    {
	semsg(_(e_failed_مقصد_استخراج_pwd_from_str_check_your_shell_config),
								 gap->ga_داده);
	أرجع;
    }

    new_dir = alloc(gap->ga_len - جابه_جایی + 1);
    url_decode(pos, gap->ga_len-جابه_جایی, new_dir);
    changedir_func(new_dir, TRUE, CDSCOPE_پنجره);
    vim_free(new_dir);
}

/*
 * Called by libvterm when it cannot recognize an OSC sequence.
 * We recognize a terminal API command.
 */
    ساكن صحيح
parse_osc(صحيح command, VTermStringFragment frag, فراغ *user)
{
    جمله_T	*جمله = (جمله_T *)user;
    js_read_T	reader;
    typval_T	tv;
    channel_T	*channel = جمله->tl_کار == عدم ? عدم
						    : جمله->tl_کار->jv_channel;
    garray_T	*gap = &جمله->tl_osc_buf;

    // We recognize only OSC 5 1 ; {command} and OSC 7 ; {command}
    إذا (command != 51 && (command != 7 || !p_asd))
	أرجع 0;

    // Concatenate what was received until the final piece is found.
    إذا (ga_grow(gap, (صحيح)frag.len + 1) == FAIL)
    {
	ga_clear(gap);
	أرجع 1;
    }
    mch_memmove((حرف *)gap->ga_داده + gap->ga_len, frag.str, frag.len);
    gap->ga_len += (صحيح)frag.len;
    إذا (!frag.final)
	أرجع 1;

    ((حرف *)gap->ga_داده)[gap->ga_len] = 0;

    إذا (command == 7)
    {
	sync_shell_dir(gap);
	ga_clear(gap);
	أرجع 1;
    }

    reader.js_buf = gap->ga_داده;
    reader.js_fill = عدم;
    reader.js_استفاده_شده = 0;
    إذا (json_decode(&reader, &tv, 0) == OK
	    && tv.v_type == VAR_LIST
	    && tv.vval.v_list != عدم)
    {
	listitem_T *قلم = tv.vval.v_list->lv_اولی;

	إذا (قلم == عدم)
	    ch_log(channel, "Missing command");
	وإلا
	{
	    char_u	*cmd = tv_get_string(&قلم->li_tv);

	    // Make sure an invoked command doesn't delete the buffer (and the
	    // terminal) under our fingers.
	    ++جمله->tl_میانگیر->b_locked;

	    قلم = قلم->li_بعدی;
	    إذا (قلم == عدم)
		ch_log(channel, "Missing argument for %s", cmd);
	    وإلا إذا (STRCMP(cmd, "drop") == 0)
		handle_drop_command(قلم);
	    وإلا إذا (STRCMP(cmd, "call") == 0)
		handle_call_command(جمله, channel, قلم);
	    وإلا
		ch_log(channel, "Invalid command received: %s", cmd);
	    --جمله->tl_میانگیر->b_locked;
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
	ثابت حرف  *leader استفادهنشده,
	ثابت دراز  آرگومانها[],
	صحيح	    argcount,
	ثابت حرف  *intermed استفادهنشده,
	حرف	    command,
	فراغ	    *user)
{
    جمله_T	*جمله = (جمله_T *)user;
    حرف	buf[100];
    صحيح		len;
    صحيح		x = 0;
    صحيح		y = 0;
    win_T	*wp;

    // We recognize only CSI 13 t
    إذا (command != 't' || argcount != 1 || آرگومانها[0] != 13)
	أرجع 0; // not handled

    // When getting the window position is not possible or it fails it results
    // in zero/zero.
#إذا defined(FEAT_GUI) \
	|| (defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)) \
	|| defined(MSWIN)
    (فراغ)ui_get_winpos(&x, &y, (varnumber_T)100);
#پایان_اگر

    FOR_ALL_WINDOWS(wp)
	إذا (wp->w_میانگیر == جمله->tl_میانگیر)
	    اكسر;
    إذا (wp != عدم)
    {
#اگر_تعریف_شده FEAT_GUI
	إذا (gui.درون_use)
	{
	    x += wp->w_wincol * gui.char_عرض;
	    y += W_WINROW(wp) * gui.char_ارتفاع;
	}
	وإلا
#پایان_اگر
	{
	    // We roughly estimate the position of the terminal window inside
	    // the Vim window by assuming a 10 x 7 character cell.
	    x += wp->w_wincol * 7;
	    y += W_WINROW(wp) * 10;
	}
    }

    len = vim_snprintf(buf, 100, "\x1b[3;%d;%dt", x, y);
    channel_send(جمله->tl_کار->jv_channel, get_tty_part(جمله),
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
vterm_malloc(size_t اندازه, فراغ *داده استفادهنشده)
{
    // make sure that the length is not zero
    أرجع alloc_clear(اندازه == 0 ? 1L : اندازه);
}

    ساكن فراغ
vterm_memfree(فراغ *ptr, فراغ *داده استفادهنشده)
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
create_vterm(جمله_T *جمله, صحيح ردیفها_2, صحيح تعداد_ستون)
{
    VTerm	    *vterm;
    VTermScreen	    *screen;
    VTermState	    *state;
    VTerm_مقدار	    مقدار;

    vterm = vterm_new_with_allocator(ردیفها_2, تعداد_ستون, &vterm_allocator, عدم);
    جمله->tl_vterm = vterm;
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

    vterm_screen_مجموعه_callbacks(screen, &screen_callbacks, جمله);
    vterm_screen_مجموعه_damage_ادغام(screen, VTERM_DAMAGE_SCROLL);
    vterm_screen_callbacks_has_pushline4(screen);
    // TODO: depends on 'encoding'.
    vterm_مجموعه_utf8(vterm, 1);

    جمله_init_default_colors(جمله);

    vterm_state_مجموعه_default_colors(
	    state,
	    &جمله->tl_default_color.fg,
	    &جمله->tl_default_color.bg);

    إذا (t_colors < 16)
	// Less than 16 colors: assume that bold means using a bright color for
	// the foreground color.
	vterm_state_مجموعه_bold_highbright(vterm_obtain_state(vterm), 1);

    // Required to initialize most things.
    vterm_screen_reset(screen, 1 /* hard */);

    // Allow using alternate screen.
    vterm_screen_enable_altscreen(screen, 1);

    // For unix do not use a blinking cursor.  In an xterm this causes the
    // cursor to blink if it's blinking in the xterm.
    // For Windows we respect the system wide setting.
#اگر_تعریف_شده MSWIN
    إذا (Get_Caret_Blink_زمان() == INFINITE)
	مقدار.boolean = 0;
    وإلا
	مقدار.boolean = 1;
#وإلا
    مقدار.boolean = 0;
#پایان_اگر
    vterm_state_مجموعه_termprop(state, VTERM_PROP_CURSORBLINK, &مقدار);
    vterm_state_مجموعه_unrecognised_fallbacks(state, &state_fallbacks, جمله);

    أرجع OK;
}

/*
 * Reset the terminal palette to its default value.
 */
    ساكن فراغ
جمله_reset_palette(VTerm *vterm)
{
    VTermState	*state = vterm_obtain_state(vterm);
    صحيح		شاخص;

    لكل (شاخص = 0; شاخص < 16; شاخص++)
    {
	VTermColor	color;

	color.type = VTERM_COLOR_نمایهای;
	ansi_color2rgb(شاخص, &color.red, &color.green, &color.blue,
		&color.شاخص);
	// The first valid index starts at 1.
	color.شاخص -= 1;

	vterm_state_مجموعه_palette_color(state, شاخص, &color);
    }
}

    ساكن فراغ
جمله_update_palette(جمله_T *جمله)
{
#إذا defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    إذا (جمله_use_palette()
	    && (جمله->tl_palette != عدم
		|| پیدا_کن_var((char_u *)"g:terminal_ansi_colors", عدم, TRUE)
								      != عدم))
    {
	إذا (جمله->tl_palette != عدم)
	    مجموعه_vterm_palette(جمله->tl_vterm, جمله->tl_palette);
	وإلا
	    init_vterm_ansi_colors(جمله->tl_vterm);
    }
    وإلا
#پایان_اگر
	جمله_reset_palette(جمله->tl_vterm);
}

/*
 * Called when option 'termguicolors' is changed.
 */
    فراغ
جمله_update_palette_all(فراغ)
{
    جمله_T *جمله;

    FOR_ALL_TERMS(جمله)
    {
	إذا (جمله->tl_vterm == عدم)
	    استمر;
	جمله_update_palette(جمله);
    }
}

/*
 * Called when option 'background' or 'termguicolors' was set,
 * or when any highlight is changed.
 */
    فراغ
جمله_update_colors_all(فراغ)
{
    جمله_T *جمله;

    FOR_ALL_TERMS(جمله)
    {
	إذا (جمله->tl_vterm == عدم)
	    استمر;
	جمله_init_default_colors(جمله);
	vterm_state_مجموعه_default_colors(
		vterm_obtain_state(جمله->tl_vterm),
		&جمله->tl_default_color.fg,
		&جمله->tl_default_color.bg);
    }
}

/*
 * Return the text to show for the buffer name and status.
 */
    char_u *
جمله_get_وضعیت_متن(جمله_T *جمله)
{
    إذا (جمله->tl_وضعیت_متن != عدم)
	أرجع جمله->tl_وضعیت_متن;

    char_u *txt;
    size_t len;
    char_u *fname;

    إذا (جمله->tl_نرمال_نما)
    {
	إذا (جمله_کار_running(جمله))
	    txt = (char_u *)_("Terminal");
	وإلا
	    txt = (char_u *)_("Terminal-finished");
    }
    وإلا إذا (جمله->tl_title != عدم)
	txt = جمله->tl_title;
    وإلا إذا (جمله_none_open(جمله))
	txt = (char_u *)_("active");
    وإلا إذا (جمله_کار_running(جمله))
	txt = (char_u *)_("running");
    وإلا
	txt = (char_u *)_("finished");
    fname = buf_get_fname(جمله->tl_میانگیر);
    len = 9 + STRLEN(fname) + STRLEN(txt);
    جمله->tl_وضعیت_متن = alloc(len);
    إذا (جمله->tl_وضعیت_متن != عدم)
	vim_snprintf((حرف *)جمله->tl_وضعیت_متن, len, "%s [%s]",
		fname, txt);
    أرجع جمله->tl_وضعیت_متن;
}

/*
 * Clear the cached value of the status text.
 */
    فراغ
جمله_clear_وضعیت_متن(جمله_T *جمله)
{
    VIM_CLEAR(جمله->tl_وضعیت_متن);
}

/*
 * Mark references in jobs of terminals.
 */
    صحيح
مجموعه_ref_درون_جمله(صحيح copyID)
{
    صحيح		abort = FALSE;
    جمله_T	*جمله;
    typval_T	tv;

    لكل (جمله = اولی_جمله; !abort && جمله != عدم; جمله = جمله->tl_بعدی)
	إذا (جمله->tl_کار != عدم)
	{
	    tv.v_type = VAR_کار;
	    tv.vval.v_کار = جمله->tl_کار;
	    abort = abort || مجموعه_ref_درون_قلم(&tv, copyID, عدم, عدم, عدم);
	}
    أرجع abort;
}

/*
 * Get the buffer from the first argument in "argvars".
 * Returns NULL when the buffer is not for a terminal window and logs a message
 * with "where".
 */
    ساكن buf_T *
جمله_get_buf(typval_T *argvars, حرف *where)
{
    buf_T *buf;

    ++emsg_off;
    buf = tv_get_buf(&argvars[0], FALSE);
    --emsg_off;
    إذا (buf == عدم || buf->b_جمله == عدم)
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
    CLEAR_فیلد(*cell);
    cell->fg.type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_FG;
    cell->bg.type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_BG;
}

    ساكن فراغ
dump_جمله_color(FILE *fd, VTermColor *color)
{
    صحيح شاخص;

    إذا (VTERM_COLOR_IS_نمایهای(color))
	شاخص = color->شاخص + 1;
    وإلا إذا (color->type == 0)
	// use RGB values
	شاخص = 255;
    وإلا
	// default color
	شاخص = 0;
    fprintf(fd, "%02x%02x%02x%d",
	    (صحيح)color->red, (صحيح)color->green, (صحيح)color->blue,
	    شاخص);
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
f_جمله_dumpwrite(typval_T *argvars, typval_T *rettv استفادهنشده)
{
    buf_T	*buf;
    جمله_T	*جمله;
    char_u	*fname;
    صحيح		max_ارتفاع_2 = 0;
    صحيح		max_عرض_2 = 0;
    stat_T	st;
    FILE	*fd;
    VTermPos	pos;
    VTermScreen *screen;
    VTermScreenCell prev_cell;
    VTermState	*state;
    VTermPos	cursor_pos;

    إذا (check_restricted() || check_secure())
	أرجع;

    إذا (درون_vim9script()
	    && (check_for_میانگیر_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	أرجع;

    buf = جمله_get_buf(argvars, "term_dumpwrite()");
    إذا (buf == عدم)
	أرجع;
    جمله = buf->b_جمله;
    إذا (جمله->tl_vterm == عدم)
    {
	emsg(_(e_کار_already_پایانیافته));
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
	    max_ارتفاع_2 = dict_get_عدد(d, "rows");
	    max_عرض_2 = dict_get_عدد(d, "columns");
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

    screen = vterm_obtain_screen(جمله->tl_vterm);
    state = vterm_obtain_state(جمله->tl_vterm);
    vterm_state_get_cursorpos(state, &cursor_pos);

    لكل (pos.ردیف = 0; (max_ارتفاع_2 == 0 || pos.ردیف < max_ارتفاع_2)
					 && pos.ردیف < جمله->tl_ردیفها; ++pos.ردیف)
    {
	صحيح		repeat = 0;

	لكل (pos.ستون = 0; (max_عرض_2 == 0 || pos.ستون < max_عرض_2)
					 && pos.ستون < جمله->tl_تعداد_ستون; ++pos.ستون)
	{
	    VTermScreenCell cell;
	    صحيح		    same_attr;
	    صحيح		    same_chars = TRUE;
	    صحيح		    i;
	    صحيح		    is_cursor_pos = (pos.ستون == cursor_pos.ستون
						 && pos.ردیف == cursor_pos.ردیف);

	    إذا (vterm_screen_get_cell(screen, pos, &cell) == 0)
		clear_cell(&cell);

	    لكل (i = 0; i < VTERM_MAX_CHARS_در_هر_CELL; ++i)
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
	    إذا (same_chars && cell.عرض == prev_cell.عرض && same_attr
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

		    لكل (i = 0; i < VTERM_MAX_CHARS_در_هر_CELL
						  && cell.chars[i] != NUL; ++i)
		    {
			len = utf_char2bytes(cell.chars[i], charbuf);
			fwrite(charbuf, len, 1, fd);
		    }
		}

		// When only the characters differ we don't write anything, the
		// following "|", "@" or NL will indicate using the same
		// attributes.
		إذا (cell.عرض != prev_cell.عرض || !same_attr)
		{
		    إذا (cell.عرض == 2)
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
			    dump_جمله_color(fd, &cell.fg);
			}
			إذا (vterm_color_is_equal(&cell.bg, &prev_cell.bg))
			    fputs("&", fd);
			وإلا
			{
			    fputs("#", fd);
			    dump_جمله_color(fd, &cell.bg);
			}
		    }
		}

		prev_cell = cell;
	    }

	    إذا (cell.عرض == 2)
		++pos.ستون;
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

    *(((cellattr_T *)gap->ga_داده) + gap->ga_len) = *cell;
    ++gap->ga_len;
}

    ساكن فراغ
clear_cellattr(cellattr_T *cell)
{
    CLEAR_فیلد(*cell);
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
    cellattr_T	    خالی_cell;
    جمله_T	    *جمله = curbuf->b_جمله;
    صحيح		    max_cells = 0;
    صحيح		    شروع_ردیف = جمله->tl_scrollback.ga_len;

    ga_init2(&ga_متن, 1, 90);
    ga_init2(&ga_cell, حجم(cellattr_T), 90);
    clear_cellattr(&cell);
    clear_cellattr(&خالی_cell);
    cursor_pos->ردیف = -1;
    cursor_pos->ستون = -1;

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
	    إذا (ga_متن.ga_داده == عدم)
		dump_is_corrupt(&ga_متن);
	    إذا (ga_grow(&جمله->tl_scrollback, 1) == OK)
	    {
		sb_سطر_T   *سطر = (sb_سطر_T *)جمله->tl_scrollback.ga_داده
						  + جمله->tl_scrollback.ga_len;

		إذا (max_cells < ga_cell.ga_len)
		    max_cells = ga_cell.ga_len;
		سطر->sb_تعداد_ستون = ga_cell.ga_len;
		سطر->sb_بایتها = ga_متن.ga_len;
		سطر->sb_cells = ga_cell.ga_داده;
		سطر->sb_fill_attr = جمله->tl_default_color;
		سطر->continuation = 0;
		++جمله->tl_scrollback.ga_len;
		ga_init(&ga_cell);

		ga_append(&ga_متن, NUL);
		ml_append(curbuf->b_ml.ml_سطر_شمار, ga_متن.ga_داده,
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
		إذا (cursor_pos->ردیف != -1)
		    dump_is_corrupt(&ga_متن);	// duplicate cursor
		cursor_pos->ردیف = جمله->tl_scrollback.ga_len - شروع_ردیف;
		cursor_pos->ستون = ga_cell.ga_len;
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
	    إذا (ga_متن.ga_داده != عدم)
	    {
		prev_char.درازی = (size_t)(ga_متن.ga_len - prev_len);
		prev_char.string = vim_strnsave(
		    ((char_u *)ga_متن.ga_داده) + prev_len,
		    prev_char.درازی);
		إذا (prev_char.string == عدم)
		    prev_char.درازی = 0;
	    }

	    إذا (c == '@' || c == '|' || c == '>' || c == '\n')
	    {
		// use all attributes from previous cell
	    }
	    وإلا إذا (c == '+' || c == '*')
	    {
		صحيح is_bg;

		cell.عرض = c == '+' ? 1 : 2;

		c = fgetc(fd);
		إذا (c == '&')
		{
		    // use same attr as previous cell
		    c = fgetc(fd);
		}
		وإلا إذا (امن_isdigit(c))
		{
		    // get the decimal attribute
		    attr = 0;
		    طالما (امن_isdigit(c))
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
			    صحيح red, green, blue, شاخص = 0, type;

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
			    إذا (!امن_isdigit(c))
				dump_is_corrupt(&ga_متن);
			    طالما (امن_isdigit(c))
			    {
				شاخص = شاخص * 10 + (c - '0');
				c = fgetc(fd);
			    }
			    إذا (شاخص == 0 || شاخص == 255)
			    {
				type = VTERM_COLOR_RGB;
				إذا (شاخص == 0)
				{
				    إذا (is_bg)
					type |= VTERM_COLOR_DEFAULT_BG;
				    وإلا
					type |= VTERM_COLOR_DEFAULT_FG;
				}
			    }
			    وإلا
			    {
				type = VTERM_COLOR_نمایهای;
				شاخص -= 1;
			    }
			    إذا (is_bg)
			    {
				cell.bg.type = type;
				cell.bg.red = red;
				cell.bg.green = green;
				cell.bg.blue = blue;
				cell.bg.شاخص = شاخص;
			    }
			    وإلا
			    {
				cell.fg.type = type;
				cell.fg.red = red;
				cell.fg.green = green;
				cell.fg.blue = blue;
				cell.fg.شاخص = شاخص;
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
	    إذا (cell.عرض == 2)
		append_cell(&ga_cell, &خالی_cell);
	}
	وإلا إذا (c == '@')
	{
	    إذا (prev_char.string == عدم)
		dump_is_corrupt(&ga_متن);
	    وإلا
	    {
		صحيح شمار = 0;

		// repeat previous character, get the count
		لكل (;;)
		{
		    c = fgetc(fd);
		    إذا (!امن_isdigit(c))
			اكسر;
		    شمار = شمار * 10 + (c - '0');
		}

		طالما (شمار-- > 0)
		{
		    ga_concat_len(&ga_متن, prev_char.string, prev_char.درازی);
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
	ml_append(curbuf->b_ml.ml_سطر_شمار, ga_متن.ga_داده,
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
get_separator(صحيح متن_عرض, char_u *fname)
{
    صحيح	    عرض = MAX(متن_عرض, curwin->w_عرض);
    char_u  *textline;
    صحيح	    fname_اندازه;
    char_u  *p = fname;
    صحيح	    i;
    size_t  off;

    textline = alloc(عرض + STRLEN(fname) + 1);
    إذا (textline == عدم)
	أرجع عدم;

    fname_اندازه = vim_strsize(fname);
    إذا (fname_اندازه < عرض - 8)
    {
	// enough room, don't use the full window width
	عرض = MAX(متن_عرض, fname_اندازه + 8);
    }
    وإلا إذا (fname_اندازه > عرض - 8)
    {
	// full name doesn't fit, use only the tail
	p = gettail(fname);
	fname_اندازه = vim_strsize(p);
    }
    // skip characters until the name fits
    طالما (fname_اندازه > عرض - 8)
    {
	p += (*mb_ptr2len)(p);
	fname_اندازه = vim_strsize(p);
    }

    لكل (i = 0; i < (عرض - fname_اندازه) / 2 - 1; ++i)
	textline[i] = '=';
    textline[i++] = ' ';

    STRCPY(textline + i, p);
    off = STRLEN(textline);
    textline[off] = ' ';
    لكل (i = 1; i < (عرض - fname_اندازه) / 2; ++i)
	textline[off + i] = '=';
    textline[off + i] = NUL;

    أرجع textline;
}

/*
 * Common for "term_dumpdiff()" and "term_dumpload()".
 */
    ساكن فراغ
جمله_بارگیری_dump(typval_T *argvars, typval_T *rettv, صحيح do_diff)
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
	emsg(_(e_invalid_آرگومان));
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

    init_کار_options(&opt);
    إذا (argvars[do_diff ? 2 : 1].v_type != VAR_UNKNOWN
	    && get_کار_options(&argvars[do_diff ? 2 : 1], &opt, 0,
		    JO2_جمله_NAME + JO2_جمله_تعداد_ستون + JO2_جمله_ردیفها
		    + JO2_VERTICAL + JO2_CURWIN + JO2_NORESTORE) == FAIL)
	اذهب theend;

    إذا (opt.jo_جمله_name == عدم)
    {
	size_t len = STRLEN(fname1) + 12;

	fname_tofree = alloc(len);
	إذا (fname_tofree != عدم)
	{
	    vim_snprintf((حرف *)fname_tofree, len, "dump diff %s", fname1);
	    opt.jo_جمله_name = fname_tofree;
	}
    }

    إذا (opt.jo_bufnr_buf != عدم)
    {
	win_T *wp = buf_jump_open_win(opt.jo_bufnr_buf);

	// With "bufnr" argument: enter the window with this buffer and make it
	// empty.
	إذا (wp == عدم)
	    semsg(_(e_invalid_آرگومان_str), "bufnr");
	وإلا
	{
	    buf = curbuf;
	    طالما (!(curbuf->b_ml.ml_flags & ML_خالی))
		ml_delete((linenr_T)1);
	    free_scrollback(curbuf->b_جمله);
	    redraw_later(UPD_NOT_VALID);
	}
    }
    وإلا
	// Create a new terminal window.
	buf = جمله_شروع(&argvars[0], عدم, &opt, جمله_شروع_NOJOB);

    إذا (buf != عدم && buf->b_جمله != عدم)
    {
	صحيح		i;
	linenr_T	bot_lnum;
	linenr_T	lnum;
	جمله_T		*جمله = buf->b_جمله;
	صحيح		عرض;
	صحيح		width2;
	VTermPos	cursor_pos1;
	VTermPos	cursor_pos2;

	جمله_init_default_colors(جمله);

	rettv->vval.v_عدد = buf->b_fnum;

	// read the files, fill the buffer with the diff
	عرض = read_dump_file(fd1, &cursor_pos1);

	// position the cursor
	إذا (cursor_pos1.ردیف >= 0)
	{
	    curwin->w_cursor.lnum = cursor_pos1.ردیف + 1;
	    coladvance(cursor_pos1.ستون);
	}

	// Delete the empty line that was in the empty buffer.
	ml_delete(1);

	// For term_dumpload() we are done here.
	إذا (!do_diff)
	    اذهب theend;

	جمله->tl_بالاترین_diff_ردیفها = curbuf->b_ml.ml_سطر_شمار;

	textline = get_separator(عرض, fname1);
	إذا (textline == عدم)
	    اذهب theend;
	إذا (جمع_کن_خالی_scrollback(جمله, &جمله->tl_default_color, 0) == OK)
	    ml_append(curbuf->b_ml.ml_سطر_شمار, textline, 0, FALSE);
	vim_free(textline);

	textline = get_separator(عرض, fname2);
	إذا (textline == عدم)
	    اذهب theend;
	إذا (جمع_کن_خالی_scrollback(جمله, &جمله->tl_default_color, 0) == OK)
	    ml_append(curbuf->b_ml.ml_سطر_شمار, textline, 0, FALSE);
	textline[عرض] = NUL;

	bot_lnum = curbuf->b_ml.ml_سطر_شمار;
	width2 = read_dump_file(fd2, &cursor_pos2);
	إذا (width2 > عرض)
	{
	    vim_free(textline);
	    textline = alloc(width2 + 1);
	    إذا (textline == عدم)
		اذهب theend;
	    عرض = width2;
	    textline[عرض] = NUL;
	}
	جمله->tl_bot_diff_ردیفها = curbuf->b_ml.ml_سطر_شمار - bot_lnum;

	لكل (lnum = 1; lnum <= جمله->tl_بالاترین_diff_ردیفها; ++lnum)
	{
	    إذا (lnum + bot_lnum > curbuf->b_ml.ml_سطر_شمار)
	    {
		// bottom part has fewer rows, fill with "-"
		لكل (i = 0; i < عرض; ++i)
		    textline[i] = '-';
	    }
	    وإلا
	    {
		char_u *line1;
		char_u *line2;
		char_u *p1;
		char_u *p2;
		صحيح	ستون;
		sb_سطر_T   *sb_سطر = (sb_سطر_T *)جمله->tl_scrollback.ga_داده;
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
		لكل (ستون = 0; ستون < عرض && *p1 != NUL && *p2 != NUL; ++ستون)
		{
		    صحيح len1 = utfc_ptr2len(p1);
		    صحيح len2 = utfc_ptr2len(p2);

		    textline[ستون] = ' ';
		    إذا (len1 != len2 || STRNCMP(p1, p2, len1) != 0)
			// text differs
			textline[ستون] = 'X';
		    وإلا إذا (lnum == cursor_pos1.ردیف + 1
			    && ستون == cursor_pos1.ستون
			    && (cursor_pos1.ردیف != cursor_pos2.ردیف
					|| cursor_pos1.ستون != cursor_pos2.ستون))
			// cursor in first but not in second
			textline[ستون] = '>';
		    وإلا إذا (lnum == cursor_pos2.ردیف + 1
			    && ستون == cursor_pos2.ستون
			    && (cursor_pos1.ردیف != cursor_pos2.ردیف
					|| cursor_pos1.ستون != cursor_pos2.ستون))
			// cursor in second but not in first
			textline[ستون] = '<';
		    وإلا إذا (cellattr1 != عدم && cellattr2 != عدم)
		    {
			إذا ((cellattr1 + ستون)->عرض
						   != (cellattr2 + ستون)->عرض)
			    textline[ستون] = 'w';
			وإلا إذا (!vterm_color_is_equal(&(cellattr1 + ستون)->fg,
						   &(cellattr2 + ستون)->fg))
			    textline[ستون] = 'f';
			وإلا إذا (!vterm_color_is_equal(&(cellattr1 + ستون)->bg,
						   &(cellattr2 + ستون)->bg))
			    textline[ستون] = 'b';
			وإلا إذا (vtermAttr2hl(&(cellattr1 + ستون)->attrs)
				  != vtermAttr2hl(&((cellattr2 + ستون)->attrs)))
			    textline[ستون] = 'a';
		    }
		    p1 += len1;
		    p2 += len2;
		    // TODO: handle different width
		}

		طالما (ستون < عرض)
		{
		    إذا (*p1 == NUL && *p2 == NUL)
			textline[ستون] = '?';
		    وإلا إذا (*p1 == NUL)
		    {
			textline[ستون] = '+';
			p2 += utfc_ptr2len(p2);
		    }
		    وإلا
		    {
			textline[ستون] = '-';
			p1 += utfc_ptr2len(p1);
		    }
		    ++ستون;
		}

		vim_free(line1);
	    }
	    إذا (جمع_کن_خالی_scrollback(جمله, &جمله->tl_default_color,
						 جمله->tl_بالاترین_diff_ردیفها) == OK)
		ml_append(جمله->tl_بالاترین_diff_ردیفها + lnum, textline, 0, FALSE);
	    ++bot_lnum;
	}

	طالما (lnum + bot_lnum <= curbuf->b_ml.ml_سطر_شمار)
	{
	    // bottom part has more rows, fill with "+"
	    لكل (i = 0; i < عرض; ++i)
		textline[i] = '+';
	    إذا (جمع_کن_خالی_scrollback(جمله, &جمله->tl_default_color,
						 جمله->tl_بالاترین_diff_ردیفها) == OK)
		ml_append(جمله->tl_بالاترین_diff_ردیفها + lnum, textline, 0, FALSE);
	    ++lnum;
	    ++bot_lnum;
	}

	جمله->tl_تعداد_ستون = عرض;

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
جمله_جابجا_diff(فراغ)
{
    جمله_T	*جمله = curbuf->b_جمله;
    linenr_T	سطر_شمار;
    linenr_T	بالاترین_ردیفها;
    linenr_T	bot_ردیفها;
    linenr_T	bot_شروع;
    linenr_T	lnum;
    char_u	*p;
    sb_سطر_T	*sb_سطر;

    إذا (جمله == عدم
	    || !جمله_is_پایانیافته(curbuf)
	    || جمله->tl_بالاترین_diff_ردیفها == 0
	    || جمله->tl_scrollback.ga_len == 0)
	أرجع FAIL;

    سطر_شمار = curbuf->b_ml.ml_سطر_شمار;
    بالاترین_ردیفها = جمله->tl_بالاترین_diff_ردیفها;
    bot_ردیفها = جمله->tl_bot_diff_ردیفها;
    bot_شروع = سطر_شمار - bot_ردیفها;
    sb_سطر = (sb_سطر_T *)جمله->tl_scrollback.ga_داده;

    // move lines from top to above the bottom part
    لكل (lnum = 1; lnum <= بالاترین_ردیفها; ++lnum)
    {
	p = vim_strsave(ml_get(1));
	إذا (p == عدم)
	    أرجع OK;
	ml_append(bot_شروع, p, 0, FALSE);
	ml_delete(1);
	vim_free(p);
    }

    // move lines from bottom to the top
    لكل (lnum = 1; lnum <= bot_ردیفها; ++lnum)
    {
	p = vim_strsave(ml_get(bot_شروع + lnum));
	إذا (p == عدم)
	    أرجع OK;
	ml_delete(bot_شروع + lnum);
	ml_append(lnum - 1, p, 0, FALSE);
	vim_free(p);
    }

    // move top title to bottom
    p = vim_strsave(ml_get(bot_ردیفها + 1));
    إذا (p == عدم)
	أرجع OK;
    ml_append(سطر_شمار - بالاترین_ردیفها - 1, p, 0, FALSE);
    ml_delete(bot_ردیفها + 1);
    vim_free(p);

    // move bottom title to top
    p = vim_strsave(ml_get(سطر_شمار - بالاترین_ردیفها));
    إذا (p == عدم)
	أرجع OK;
    ml_delete(سطر_شمار - بالاترین_ردیفها);
    ml_append(bot_ردیفها, p, 0, FALSE);
    vim_free(p);

    إذا (بالاترین_ردیفها == bot_ردیفها)
    {
	// rows counts are equal, can swap cell properties
	لكل (lnum = 0; lnum < بالاترین_ردیفها; ++lnum)
	{
	    sb_سطر_T	موقت;

	    موقت = *(sb_سطر + lnum);
	    *(sb_سطر + lnum) = *(sb_سطر + bot_شروع + lnum);
	    *(sb_سطر + bot_شروع + lnum) = موقت;
	}
    }
    وإلا
    {
	size_t		اندازه = حجم(sb_سطر_T) * جمله->tl_scrollback.ga_len;
	sb_سطر_T	*موقت = alloc(اندازه);

	// need to copy cell properties into temp memory
	إذا (موقت != عدم)
	{
	    mch_memmove(موقت, جمله->tl_scrollback.ga_داده, اندازه);
	    mch_memmove(جمله->tl_scrollback.ga_داده,
		    موقت + bot_شروع,
		    حجم(sb_سطر_T) * bot_ردیفها);
	    mch_memmove((sb_سطر_T *)جمله->tl_scrollback.ga_داده + bot_ردیفها,
		    موقت + بالاترین_ردیفها,
		    حجم(sb_سطر_T) * (سطر_شمار - بالاترین_ردیفها - bot_ردیفها));
	    mch_memmove((sb_سطر_T *)جمله->tl_scrollback.ga_داده
						       + سطر_شمار - بالاترین_ردیفها,
		    موقت,
		    حجم(sb_سطر_T) * بالاترین_ردیفها);
	    vim_free(موقت);
	}
    }

    جمله->tl_بالاترین_diff_ردیفها = bot_ردیفها;
    جمله->tl_bot_diff_ردیفها = بالاترین_ردیفها;

    update_screen(UPD_NOT_VALID);
    أرجع OK;
}

/*
 * "term_dumpdiff(filename, filename, options)" function
 */
    فراغ
f_جمله_dumpdiff(typval_T *argvars, typval_T *rettv)
{
    إذا (درون_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	أرجع;

    جمله_بارگیری_dump(argvars, rettv, TRUE);
}

/*
 * "term_dumpload(filename, options)" function
 */
    فراغ
f_جمله_dumpload(typval_T *argvars, typval_T *rettv)
{
    إذا (درون_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	أرجع;

    جمله_بارگیری_dump(argvars, rettv, FALSE);
}

/*
 * "term_getaltscreen(buf)" function
 */
    فراغ
f_جمله_getaltscreen(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    إذا (درون_vim9script() && check_for_میانگیر_arg(argvars, 0) == FAIL)
	أرجع;

    buf = جمله_get_buf(argvars, "term_getaltscreen()");
    إذا (buf == عدم)
	أرجع;
    rettv->vval.v_عدد = buf->b_جمله->tl_using_altscreen;
}

/*
 * "term_getattr(attr, name)" function
 */
    فراغ
f_جمله_getattr(typval_T *argvars, typval_T *rettv)
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

    إذا (درون_vim9script()
	    && (check_for_عدد_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	أرجع;

    attr = tv_get_عدد(&argvars[0]);
    name = tv_get_string_chk(&argvars[1]);
    إذا (name == عدم)
	أرجع;

    إذا (attr > HL_ALL)
	attr = syn_attr2attr(attr);
    لكل (i = 0; i < ARRAY_درازی(attrs); ++i)
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
f_جمله_getcursor(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    جمله_T	*جمله;
    list_T	*l;
    dict_T	*d;

    إذا (rettv_list_alloc(rettv) == FAIL)
	أرجع;

    إذا (درون_vim9script() && check_for_میانگیر_arg(argvars, 0) == FAIL)
	أرجع;

    buf = جمله_get_buf(argvars, "term_getcursor()");
    إذا (buf == عدم)
	أرجع;
    جمله = buf->b_جمله;

    l = rettv->vval.v_list;
    list_append_عدد(l, جمله->tl_cursor_pos.ردیف + 1);
    list_append_عدد(l, جمله->tl_cursor_pos.ستون + 1);

    d = dict_alloc();
    إذا (d == عدم)
	أرجع;

    dict_جمع_کن_عدد(d, "visible", جمله->tl_cursor_visible);
    dict_جمع_کن_عدد(d, "blink", blink_state_is_inverted()
	    ? !جمله->tl_cursor_blink : جمله->tl_cursor_blink);
    dict_جمع_کن_عدد(d, "shape", جمله->tl_cursor_shape);
    dict_جمع_کن_string(d, "color", cursor_color_get(جمله->tl_cursor_color));
    إذا (list_append_dict(l, d) == FAIL)
	dict_unref(d);
}

/*
 * "term_getjob(buf)" function
 */
    فراغ
f_جمله_getjob(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    إذا (درون_vim9script() && check_for_میانگیر_arg(argvars, 0) == FAIL)
	أرجع;

    buf = جمله_get_buf(argvars, "term_getjob()");
    إذا (buf == عدم)
    {
	إذا (درون_vim9script())
	{
	    rettv->v_type = VAR_کار;
	    rettv->vval.v_کار = عدم;
	}
	وإلا
	{
	    rettv->v_type = VAR_SPECIAL;
	    rettv->vval.v_عدد = VVAL_NULL;
	}
	أرجع;
    }

    rettv->v_type = VAR_کار;
    rettv->vval.v_کار = buf->b_جمله->tl_کار;
    إذا (rettv->vval.v_کار != عدم)
	++rettv->vval.v_کار->jv_refcount;
}

    ساكن صحيح
get_ردیف_عدد(typval_T *tv, جمله_T *جمله)
{
    إذا (tv->v_type == VAR_STRING
	    && tv->vval.v_string != عدم
	    && STRCMP(tv->vval.v_string, ".") == 0)
	أرجع جمله->tl_cursor_pos.ردیف;
    أرجع (صحيح)tv_get_عدد(tv) - 1;
}

/*
 * "term_getline(buf, row)" function
 */
    فراغ
f_جمله_getline(typval_T *argvars, typval_T *rettv)
{
    buf_T	    *buf;
    جمله_T	    *جمله;
    صحيح		    ردیف;

    rettv->v_type = VAR_STRING;

    إذا (درون_vim9script()
	    && (check_for_میانگیر_arg(argvars, 0) == FAIL
		|| check_for_lnum_arg(argvars, 1) == FAIL))
	أرجع;

    buf = جمله_get_buf(argvars, "term_getline()");
    إذا (buf == عدم)
	أرجع;
    جمله = buf->b_جمله;
    ردیف = get_ردیف_عدد(&argvars[1], جمله);

    إذا (جمله->tl_vterm == عدم)
    {
	linenr_T  lnum = 0;
	size_t	  جابه_جایی = 0;
	صحيح	  sb_ردیف = جمله->tl_scrollback_scrolled + ردیف;
	sb_سطر_T *سطر;

	إذا (sb_ردیف < 0 || sb_ردیف >= جمله->tl_scrollback.ga_len)
	    أرجع;
	سطر = (sb_سطر_T *)جمله->tl_scrollback.ga_داده + sb_ردیف;

	scrollbackline_pos_درون_buf(جمله, sb_ردیف, &lnum, عدم, &جابه_جایی);

	// vterm is finished, get the text from the buffer
	إذا (lnum > 0 && lnum <= buf->b_ml.ml_سطر_شمار)
	{
	    char_u *p = ml_get_buf(buf, lnum, FALSE);
	    إذا (STRLEN(p) >= جابه_جایی + سطر->sb_بایتها)
		rettv->vval.v_string = vim_strnsave(p + جابه_جایی, سطر->sb_بایتها);
	}
    }
    وإلا
    {
	VTermScreen	*screen = vterm_obtain_screen(جمله->tl_vterm);
	VTermRect	rect;
	صحيح		len;
	char_u		*p;

	إذا (ردیف < 0 || ردیف >= جمله->tl_ردیفها)
	    أرجع;
	len = جمله->tl_تعداد_ستون * MB_MAXBYTES + 1;
	p = alloc(len);
	إذا (p == عدم)
	    أرجع;
	rettv->vval.v_string = p;

	rect.شروع_ستون = 0;
	rect.انتها_ستون = جمله->tl_تعداد_ستون;
	rect.شروع_ردیف = ردیف;
	rect.انتها_ردیف = ردیف + 1;
	p[vterm_screen_get_متن(screen, (حرف *)p, len, rect)] = NUL;
    }
}

/*
 * "term_getscrolled(buf)" function
 */
    فراغ
f_جمله_getscrolled(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    إذا (درون_vim9script() && check_for_میانگیر_arg(argvars, 0) == FAIL)
	أرجع;

    buf = جمله_get_buf(argvars, "term_getscrolled()");
    إذا (buf == عدم)
	أرجع;
    rettv->vval.v_عدد = buf->b_جمله->tl_میانگیر_scrolled;
}

/*
 * "term_getsize(buf)" function
 */
    فراغ
f_جمله_getsize(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    list_T	*l;

    إذا (rettv_list_alloc(rettv) == FAIL)
	أرجع;

    إذا (درون_vim9script() && check_for_میانگیر_arg(argvars, 0) == FAIL)
	أرجع;

    buf = جمله_get_buf(argvars, "term_getsize()");
    إذا (buf == عدم)
	أرجع;

    l = rettv->vval.v_list;
    list_append_عدد(l, buf->b_جمله->tl_ردیفها);
    list_append_عدد(l, buf->b_جمله->tl_تعداد_ستون);
}

/*
 * "term_setsize(buf, rows, cols)" function
 */
    فراغ
f_جمله_setsize(typval_T *argvars, typval_T *rettv استفادهنشده)
{
    buf_T	*buf;
    جمله_T	*جمله;
    varnumber_T ردیفها_2, تعداد_ستون;

    إذا (درون_vim9script()
	    && (check_for_میانگیر_arg(argvars, 0) == FAIL
		|| check_for_عدد_arg(argvars, 1) == FAIL
		|| check_for_عدد_arg(argvars, 2) == FAIL))
	أرجع;

    buf = جمله_get_buf(argvars, "term_setsize()");
    إذا (buf == عدم)
    {
	emsg(_(e_not_terminal_میانگیر));
	أرجع;
    }
    إذا (buf->b_جمله->tl_vterm == عدم)
	أرجع;
    جمله = buf->b_جمله;
    ردیفها_2 = tv_get_عدد(&argvars[1]);
    ردیفها_2 = ردیفها_2 <= 0 ? جمله->tl_ردیفها : ردیفها_2;
    تعداد_ستون = tv_get_عدد(&argvars[2]);
    تعداد_ستون = تعداد_ستون <= 0 ? جمله->tl_تعداد_ستون : تعداد_ستون;
    vterm_مجموعه_اندازه(جمله->tl_vterm, ردیفها_2, تعداد_ستون);
    // handle_resize() will resize the windows

    // Get and remember the size we ended up with.  Update the pty.
    vterm_get_اندازه(جمله->tl_vterm, &جمله->tl_ردیفها, &جمله->tl_تعداد_ستون);
    جمله_report_winsize(جمله, جمله->tl_ردیفها, جمله->tl_تعداد_ستون);
}

/*
 * "term_getstatus(buf)" function
 */
    فراغ
f_جمله_getstatus(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    جمله_T	*جمله;
    char_u	val[100];

    rettv->v_type = VAR_STRING;

    إذا (درون_vim9script() && check_for_میانگیر_arg(argvars, 0) == FAIL)
	أرجع;

    buf = جمله_get_buf(argvars, "term_getstatus()");
    إذا (buf == عدم)
	أرجع;
    جمله = buf->b_جمله;

    إذا (جمله_کار_running(جمله))
	STRCPY(val, "running");
    وإلا
	STRCPY(val, "finished");
    إذا (جمله->tl_نرمال_نما)
	STRCAT(val, ",normal");
    rettv->vval.v_string = vim_strsave(val);
}

/*
 * "term_gettitle(buf)" function
 */
    فراغ
f_جمله_gettitle(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    rettv->v_type = VAR_STRING;

    إذا (درون_vim9script() && check_for_میانگیر_arg(argvars, 0) == FAIL)
	أرجع;

    buf = جمله_get_buf(argvars, "term_gettitle()");
    إذا (buf == عدم)
	أرجع;

    إذا (buf->b_جمله->tl_title != عدم)
	rettv->vval.v_string = vim_strsave(buf->b_جمله->tl_title);
}

/*
 * "term_gettty(buf)" function
 */
    فراغ
f_جمله_gettty(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    char_u	*p = عدم;
    صحيح		رقم = 0;

    إذا (درون_vim9script()
	    && (check_for_میانگیر_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	أرجع;

    rettv->v_type = VAR_STRING;
    buf = جمله_get_buf(argvars, "term_gettty()");
    إذا (buf == عدم)
	أرجع;
    إذا (argvars[1].v_type != VAR_UNKNOWN)
	رقم = tv_get_bool(&argvars[1]);

    بدل (رقم)
    {
	حالة 0:
	    إذا (buf->b_جمله->tl_کار != عدم)
		p = buf->b_جمله->tl_کار->jv_tty_بیرون;
	    اكسر;
	حالة 1:
	    إذا (buf->b_جمله->tl_کار != عدم)
		p = buf->b_جمله->tl_کار->jv_tty_درون;
	    اكسر;
	افتراضي:
	    semsg(_(e_invalid_آرگومان_str), tv_get_string(&argvars[1]));
	    أرجع;
    }
    إذا (p != عدم)
	rettv->vval.v_string = vim_strsave(p);
}

/*
 * "term_list()" function
 */
    فراغ
f_جمله_list(typval_T *argvars استفادهنشده, typval_T *rettv)
{
    جمله_T	*tp;
    list_T	*l;

    إذا (rettv_list_alloc(rettv) == FAIL || اولی_جمله == عدم)
	أرجع;

    l = rettv->vval.v_list;
    FOR_ALL_TERMS(tp)
	إذا (tp->tl_میانگیر != عدم)
	    إذا (list_append_عدد(l,
				   (varnumber_T)tp->tl_میانگیر->b_fnum) == FAIL)
		أرجع;
}

/*
 * "term_scrape(buf, row)" function
 */
    فراغ
f_جمله_scrape(typval_T *argvars, typval_T *rettv)
{
    buf_T	    *buf;
    VTermScreen	    *screen = عدم;
    VTermPos	    pos;
    list_T	    *l;
    جمله_T	    *جمله;
    char_u	    *p;
    sb_سطر_T	    *سطر;

    إذا (rettv_list_alloc(rettv) == FAIL)
	أرجع;

    إذا (درون_vim9script()
	    && (check_for_میانگیر_arg(argvars, 0) == FAIL
		|| check_for_lnum_arg(argvars, 1) == FAIL))
	أرجع;

    buf = جمله_get_buf(argvars, "term_scrape()");
    إذا (buf == عدم)
	أرجع;
    جمله = buf->b_جمله;

    l = rettv->vval.v_list;
    pos.ردیف = get_ردیف_عدد(&argvars[1], جمله);

    إذا (جمله->tl_vterm != عدم)
    {
	screen = vterm_obtain_screen(جمله->tl_vterm);
	إذا (screen == عدم)  // can't really happen
	    أرجع;
	p = عدم;
	سطر = عدم;
    }
    وإلا
    {
	صحيح	  sb_ردیف = جمله->tl_scrollback_scrolled + pos.ردیف;
	linenr_T  lnum = 0;
	size_t	  جابه_جایی = 0;

	scrollbackline_pos_درون_buf(جمله, sb_ردیف, &lnum, عدم, &جابه_جایی);

	إذا (sb_ردیف >= جمله->tl_scrollback.ga_len || lnum <= 0 || lnum > buf->b_ml.ml_سطر_شمار)
	    أرجع;

	سطر = (sb_سطر_T *)جمله->tl_scrollback.ga_داده + sb_ردیف;
	p = ml_get_buf(buf, lnum, FALSE);

	إذا (STRLEN(p) < جابه_جایی + سطر->sb_بایتها)
	    أرجع;

	p += جابه_جایی;
    }

    لكل (pos.ستون = 0; pos.ستون < جمله->tl_تعداد_ستون; )
    {
	dict_T		*dcell;
	صحيح		عرض;
	VTermScreenCellAttrs attrs;
	VTermColor	fg, bg;
	char_u		rgb[8];
	size_t		rgblen;
	char_u		mbs[MB_MAXBYTES * VTERM_MAX_CHARS_در_هر_CELL + 1];
	size_t		mbslen;
	صحيح		i;

	إذا (screen == عدم)
	{
	    cellattr_T	*cellattr;

	    // vterm has finished, get the cell from scrollback
	    إذا (pos.ستون >= سطر->sb_تعداد_ستون)
		اكسر;
	    cellattr = سطر->sb_cells + pos.ستون;
	    عرض = cellattr->عرض;
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
	    لكل (i = 0; i < VTERM_MAX_CHARS_در_هر_CELL; ++i)
	    {
		إذا (cell.chars[i] == 0)
		    اكسر;
		mbslen += (*utf_char2bytes)((صحيح)cell.chars[i], mbs + mbslen);
	    }
	    mbs[mbslen] = NUL;
	    عرض = cell.عرض;
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

	dict_جمع_کن_string_len(dcell, "chars", mbs, (صحيح)mbslen);

	rgblen = vim_snprintf_safelen((حرف *)rgb, حجم(rgb),
	    "#%02x%02x%02x", fg.red, fg.green, fg.blue);
	dict_جمع_کن_string_len(dcell, "fg", rgb, (صحيح)rgblen);
	rgblen = vim_snprintf_safelen((حرف *)rgb, حجم(rgb),
	    "#%02x%02x%02x", bg.red, bg.green, bg.blue);
	dict_جمع_کن_string_len(dcell, "bg", rgb, (صحيح)rgblen);

	dict_جمع_کن_عدد(dcell, "attr",
				      cell2attr(جمله, عدم, &attrs, &fg, &bg));
	dict_جمع_کن_عدد(dcell, "width", عرض);

	++pos.ستون;
	إذا (عرض == 2)
	    ++pos.ستون;
    }
}

/*
 * "term_sendkeys(buf, keys)" function
 */
    فراغ
f_جمله_sendkeys(typval_T *argvars, typval_T *rettv استفادهنشده)
{
    buf_T	*buf;
    char_u	*پیام;
    جمله_T	*جمله;

    إذا (درون_vim9script()
	    && (check_for_میانگیر_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	أرجع;

    buf = جمله_get_buf(argvars, "term_sendkeys()");
    إذا (buf == عدم)
	أرجع;

    پیام = tv_get_string_chk(&argvars[1]);
    إذا (پیام == عدم)
	أرجع;
    جمله = buf->b_جمله;
    إذا (جمله->tl_vterm == عدم)
	أرجع;

    طالما (*پیام != NUL)
    {
	صحيح c;

	إذا (*پیام == K_SPECIAL && پیام[1] != NUL && پیام[2] != NUL)
	{
	    c = مقصد_SPECIAL(پیام[1], پیام[2]);
	    پیام += 3;
	}
	وإلا
	{
	    c = PTR2CHAR(پیام);
	    پیام += MB_CPTR2LEN(پیام);
	}
	send_کلیدها_مقصد_جمله(جمله, c, 0, FALSE);
    }
}

#إذا defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * "term_getansicolors(buf)" function
 */
    فراغ
f_جمله_getansicolors(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    جمله_T	*جمله;
    VTermState	*state;
    VTermColor  color;
    char_u	hexbuf[10];
    صحيح		شاخص;
    list_T	*list;

    إذا (rettv_list_alloc(rettv) == FAIL)
	أرجع;

    إذا (درون_vim9script() && check_for_میانگیر_arg(argvars, 0) == FAIL)
	أرجع;

    buf = جمله_get_buf(argvars, "term_getansicolors()");
    إذا (buf == عدم)
	أرجع;
    جمله = buf->b_جمله;
    إذا (جمله->tl_vterm == عدم)
	أرجع;

    list = rettv->vval.v_list;
    state = vterm_obtain_state(جمله->tl_vterm);
    لكل (شاخص = 0; شاخص < 16; شاخص++)
    {
	size_t	hexbuflen;

	vterm_state_get_palette_color(state, شاخص, &color);
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
f_جمله_setansicolors(typval_T *argvars, typval_T *rettv استفادهنشده)
{
    buf_T	*buf;
    جمله_T	*جمله;
    listitem_T	*li;
    صحيح		n = 0;

    إذا (درون_vim9script()
	    && (check_for_میانگیر_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL))
	أرجع;

    buf = جمله_get_buf(argvars, "term_setansicolors()");
    إذا (buf == عدم)
	أرجع;
    جمله = buf->b_جمله;
    إذا (جمله->tl_vterm == عدم)
	أرجع;

    إذا (check_for_nonnull_list_arg(argvars, 1) == FAIL)
	أرجع;

    إذا (argvars[1].vval.v_list->lv_اولی == &range_list_قلم
	    || argvars[1].vval.v_list->lv_len != 16)
    {
	semsg(_(e_invalid_مقدار_for_آرگومان_str), "\"colors\"");
	أرجع;
    }

    إذا (جمله->tl_palette == عدم)
	جمله->tl_palette = ALLOC_MULT(long_u, 16);
    إذا (جمله->tl_palette == عدم)
	أرجع;

    FOR_ALL_LIST_اقلام(argvars[1].vval.v_list, li)
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

	جمله->tl_palette[n++] = GUI_MCH_GET_RGB(guicolor);
    }

    جمله_update_palette(جمله);
}
#پایان_اگر

/*
 * "term_setapi(buf, api)" function
 */
    فراغ
f_جمله_setapi(typval_T *argvars, typval_T *rettv استفادهنشده)
{
    buf_T	*buf;
    جمله_T	*جمله;
    char_u	*api;

    إذا (درون_vim9script()
	    && (check_for_میانگیر_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	أرجع;

    buf = جمله_get_buf(argvars, "term_setapi()");
    إذا (buf == عدم)
	أرجع;
    جمله = buf->b_جمله;
    vim_free(جمله->tl_api);
    api = tv_get_string_chk(&argvars[1]);
    إذا (api != عدم)
	جمله->tl_api = vim_strsave(api);
    وإلا
	جمله->tl_api = عدم;
}

/*
 * "term_setrestore(buf, command)" function
 */
    فراغ
f_جمله_setrestore(typval_T *argvars استفادهنشده, typval_T *rettv استفادهنشده)
{
#إذا defined(FEAT_SESSION)
    buf_T	*buf;
    جمله_T	*جمله;
    char_u	*cmd;

    إذا (درون_vim9script()
	    && (check_for_میانگیر_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	أرجع;

    buf = جمله_get_buf(argvars, "term_setrestore()");
    إذا (buf == عدم)
	أرجع;
    جمله = buf->b_جمله;
    vim_free(جمله->tl_command);
    cmd = tv_get_string_chk(&argvars[1]);
    إذا (cmd != عدم)
	جمله->tl_command = vim_strsave(cmd);
    وإلا
	جمله->tl_command = عدم;
#پایان_اگر
}

/*
 * "term_setkill(buf, how)" function
 */
    فراغ
f_جمله_setkill(typval_T *argvars استفادهنشده, typval_T *rettv استفادهنشده)
{
    buf_T	*buf;
    جمله_T	*جمله;
    char_u	*how;

    إذا (درون_vim9script()
	    && (check_for_میانگیر_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	أرجع;

    buf = جمله_get_buf(argvars, "term_setkill()");
    إذا (buf == عدم)
	أرجع;
    جمله = buf->b_جمله;
    vim_free(جمله->tl_kill);
    how = tv_get_string_chk(&argvars[1]);
    إذا (how != عدم)
	جمله->tl_kill = vim_strsave(how);
    وإلا
	جمله->tl_kill = عدم;
}

/*
 * "term_start(command, options)" function
 */
    فراغ
f_جمله_شروع(typval_T *argvars, typval_T *rettv)
{
    jobopt_T	opt;
    buf_T	*buf;

    إذا (درون_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	أرجع;

    init_کار_options(&opt);
    إذا (argvars[1].v_type != VAR_UNKNOWN
	    && get_کار_options(&argvars[1], &opt,
		JO_TIMEOUT_ALL + JO_STOPONEXIT
		    + JO_CALLBACK + JO_بیرون_CALLBACK + JO_ERR_CALLBACK
		    + JO_EXIT_CB + JO_CLOSE_CALLBACK + JO_بیرون_IO,
		JO2_جمله_NAME + JO2_جمله_پایان + JO2_HIDDEN + JO2_جمله_OPENCMD
		    + JO2_جمله_تعداد_ستون + JO2_جمله_ردیفها + JO2_VERTICAL + JO2_CURWIN
		    + JO2_CWD + JO2_ENV + JO2_EOF_CHARS
		    + JO2_NORESTORE + JO2_جمله_KILL + JO2_جمله_HIGHLIGHT
		    + JO2_ANSI_COLORS + JO2_TTY_TYPE + JO2_جمله_API) == FAIL)
	أرجع;

    buf = جمله_شروع(&argvars[0], عدم, &opt, 0);

    إذا (buf != عدم && buf->b_جمله != عدم)
	rettv->vval.v_عدد = buf->b_fnum;
}

/*
 * "term_wait" function
 */
    فراغ
f_جمله_wait(typval_T *argvars, typval_T *rettv استفادهنشده)
{
    buf_T	*buf;

    إذا (درون_vim9script()
	    && (check_for_میانگیر_arg(argvars, 0) == FAIL
		|| check_for_opt_عدد_arg(argvars, 1) == FAIL))
	أرجع;

    buf = جمله_get_buf(argvars, "term_wait()");
    إذا (buf == عدم)
	أرجع;
    إذا (buf->b_جمله->tl_کار == عدم)
    {
	ch_log(عدم, "term_wait(): no job to wait for");
	أرجع;
    }
    إذا (buf->b_جمله->tl_کار->jv_channel == عدم)
	// channel is closed, nothing to do
	أرجع;

    // Get the job status, this will detect a job that finished.
    إذا (!buf->b_جمله->tl_کار->jv_channel->ch_keep_open
	    && STRCMP(کار_وضعیت(buf->b_جمله->tl_کار), "dead") == 0)
    {
	// The job is dead, keep reading channel I/O until the channel is
	// closed. buf->b_term may become NULL if the terminal was closed while
	// waiting.
	ch_log(عدم, "term_wait(): waiting for channel to close");
	طالما (buf->b_جمله != عدم && !buf->b_جمله->tl_channel_closed)
	{
	    جمله_flush_messages();

	    ui_delay(10L, FALSE);
	    إذا (!buf_valid(buf))
		// If the terminal is closed when the channel is closed the
		// buffer disappears.
		اكسر;
	    إذا (buf->b_جمله == عدم || buf->b_جمله->tl_channel_closing)
		// came here from a close callback, only wait one time
		اكسر;
	}

	جمله_flush_messages();
    }
    وإلا
    {
	دراز wait = 10L;

	جمله_flush_messages();

	// Wait for some time for any channel I/O.
	إذا (argvars[1].v_type != VAR_UNKNOWN)
	    wait = tv_get_عدد(&argvars[1]);
	ui_delay(wait, TRUE);

	// Flushing messages on channels is hopefully sufficient.
	// TODO: is there a better way?
	جمله_flush_messages();
    }
}

/*
 * Called when a channel has sent all the lines to a terminal.
 * Send a CTRL-D to mark the end of the text.
 */
    فراغ
جمله_send_eof(channel_T *ch)
{
    جمله_T	*جمله;

    FOR_ALL_TERMS(جمله)
	إذا (جمله->tl_کار == ch->ch_کار)
	{
	    إذا (جمله->tl_eof_chars != عدم)
	    {
		channel_send(ch, PART_درون, جمله->tl_eof_chars,
					(صحيح)STRLEN(جمله->tl_eof_chars), عدم);
		channel_send(ch, PART_درون, (char_u *)"\r", 1, عدم);
	    }
#اگر_تعریف_شده MSWIN
	    وإلا
		// Default: CTRL-D
		channel_send(ch, PART_درون, (char_u *)"\004\r", 2, عدم);
#پایان_اگر
	}
}

#إذا defined(FEAT_GUI)
    کار_T *
جمله_getjob(جمله_T *جمله)
{
    أرجع جمله != عدم ? جمله->tl_کار : عدم;
}
#پایان_اگر

#إذا defined(MSWIN)

///////////////////////////////////////
// 2. MS-Windows implementation.

HRESULT (WINAPI *pCreatePseudoConsole)(COORD, HANDLE, HANDLE, DWORD, HPCON*);
HRESULT (WINAPI *pResizePseudoConsole)(HPCON, COORD);
HRESULT (WINAPI *pClosePseudoConsole)(HPCON);
BOOL (WINAPI *pInitializeProcThreadAttributeList)(LPPROC_THREAD_ATTRIBUTE_LIST, DWORD, DWORD, PSIZE_T);
BOOL (WINAPI *pUpdateProcThreadAttribute)(LPPROC_THREAD_ATTRIBUTE_LIST, DWORD, DWORD_PTR, PVOID, اندازه_T, PVOID, PSIZE_T);
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
    } conpty_مدخل[] =
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

    hKerneldll = vim_بارگیری_Lib("kernel32.dll");
    لكل (i = 0; conpty_مدخل[i].name != عدم
					&& conpty_مدخل[i].ptr != عدم; ++i)
    {
	إذا ((*conpty_مدخل[i].ptr = (FARPROC)Get_Proc_آدرس(hKerneldll,
						conpty_مدخل[i].name)) == عدم)
	{
	    إذا (verbose)
		semsg(_(e_could_not_بارگیری_library_function_str), conpty_مدخل[i].name);
	    hKerneldll = عدم;
	    أرجع FAIL;
	}
    }

    أرجع OK;
}

    ساكن صحيح
conpty_جمله_and_کار_init(
	جمله_T	    *جمله,
	typval_T    *argvar,
	حرف	    **argv استفادهنشده,
	jobopt_T    *opt,
	jobopt_T    *orig_opt)
{
    WCHAR	    *cmd_wchar = عدم;
    WCHAR	    *cmd_wchar_copy = عدم;
    WCHAR	    *cwd_wchar = عدم;
    WCHAR	    *env_wchar = عدم;
    channel_T	    *channel = عدم;
    کار_T	    *کار = عدم;
    HANDLE	    jo = عدم;
    garray_T	    ga_cmd, ga_env;
    char_u	    *cmd = عدم;
    HRESULT	    hr;
    COORD	    consize;
    اندازه_T	    breq;
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
	cmd = ga_cmd.ga_داده;
    }
    إذا (cmd == عدم || *cmd == NUL)
    {
	emsg(_(e_invalid_آرگومان));
	اذهب failed;
    }

    جمله->tl_arg0_cmd = vim_strsave(cmd);

    cmd_wchar = enc_مقصد_utf16(cmd, عدم);

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
	cwd_wchar = enc_مقصد_utf16(opt->jo_cwd, عدم);
	إذا (cwd_wchar == عدم)
	    اذهب failed;
    }

    win32_build_env(opt->jo_env, &ga_env, TRUE);
    env_wchar = ga_env.ga_داده;

    إذا (!CreatePipe(&i_theirs, &i_ours, عدم, 0))
	اذهب failed;
    إذا (!CreatePipe(&o_ours, &o_theirs, عدم, 0))
	اذهب failed;

    consize.X = جمله->tl_تعداد_ستون;
    consize.Y = جمله->tl_ردیفها;
    hr = pCreatePseudoConsole(consize, i_theirs, o_theirs, 0,
							     &جمله->tl_conpty);
    إذا (FAILED(hr))
	اذهب failed;

    جمله->tl_siex.StartupInfo.cb = حجم(جمله->tl_siex);

    // Explicitly invalidate std handles to prevent inheritance of
    // the debugger's stdout (e.g., in Visual Studio debugging sessions),
    // which could cause job output to go to the debugger instead of
    // the intended ConPTY, even with bInheritHandles set to FALSE in CreateProcess.
    جمله->tl_siex.StartupInfo.dwFlags = STARTF_USESTDHANDLES;
    جمله->tl_siex.StartupInfo.h_Std_درون_داد = INVALID_HANDLE_مقدار;
    جمله->tl_siex.StartupInfo.hStdOutput = INVALID_HANDLE_مقدار;
    جمله->tl_siex.StartupInfo.hStdError = INVALID_HANDLE_مقدار;

    // Set up pipe inheritance safely: Vista or later.
    pInitializeProcThreadAttributeList(عدم, 1, 0, &breq);
    جمله->tl_siex.lpAttributeList = alloc(breq);
    إذا (!جمله->tl_siex.lpAttributeList)
	اذهب failed;
    إذا (!pInitializeProcThreadAttributeList(جمله->tl_siex.lpAttributeList, 1,
								     0, &breq))
	اذهب failed;
    إذا (!pUpdateProcThreadAttribute(
	    جمله->tl_siex.lpAttributeList, 0,
	    PROC_THREAD_ATTRIBUTE_PSEUDOCONSOLE, جمله->tl_conpty,
	    حجم(HPCON), عدم, عدم))
	اذهب failed;

    channel = جمع_کن_channel();
    إذا (channel == عدم)
	اذهب failed;

    کار = کار_alloc();
    إذا (کار == عدم)
	اذهب failed;
    إذا (argvar->v_type == VAR_STRING)
    {
	صحيح argc;

	build_argv_from_string(cmd, &کار->jv_argv, &argc);
    }
    وإلا
    {
	صحيح argc;

	build_argv_from_list(argvar->vval.v_list, &کار->jv_argv, &argc);
    }

    إذا (opt->jo_مجموعه & JO_درون_BUF)
	کار->jv_درون_buf = buflist_findnr(opt->jo_io_buf[PART_درون]);

    إذا (!CreateProcessW(عدم, cmd_wchar_copy, عدم, عدم, FALSE,
	    EXTENDED_STARTUPINFO_PRESENT | CREATE_UNICODE_ENVIRONMENT
	    | CREATE_SUSPENDED | CREATE_DEFAULT_ERROR_نما,
	    env_wchar, cwd_wchar,
	    &جمله->tl_siex.StartupInfo, &proc_info))
    {
	errmsg = GetWin32Error();
	اذهب failed;
    }

    CloseHandle(i_theirs);
    CloseHandle(o_theirs);

    channel_مجموعه_pipes(channel,
	    (sock_T)i_ours,
	    (sock_T)o_ours,
	    (sock_T)o_ours);

    // Write lines with CR instead of NL.
    channel->ch_write_متن_نما = TRUE;

    // Use to explicitly delete anonymous pipe handle.
    // In addition, it is used to prevent the pipe from being closed when input
    // from a buffer etc. is finished.
    channel->ch_anonymous_pipe = TRUE;

    jo = Create_کار_Object(عدم, عدم);
    إذا (jo == عدم)
	اذهب failed;

    إذا (!تخصیص_بده_Process_مقصد_کار_Object(jo, proc_info.hProcess))
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

    إذا (create_vterm(جمله, جمله->tl_ردیفها, جمله->tl_تعداد_ستون) == FAIL)
	اذهب failed;

# إذا defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    إذا (جمله_use_palette())
    {
	إذا (جمله->tl_palette != عدم)
	    مجموعه_vterm_palette(جمله->tl_vterm, جمله->tl_palette);
	وإلا
	    init_vterm_ansi_colors(جمله->tl_vterm);
    }
# endif

    channel_مجموعه_کار(channel, کار, opt);
    کار_مجموعه_options(کار, opt);

    کار->jv_channel = channel;
    کار->jv_proc_info = proc_info;
    کار->jv_کار_object = jo;
    کار->jv_وضعیت = کار_STARTED;
    کار->jv_tty_type = vim_strsave((char_u *)"conpty");
    ++کار->jv_refcount;
    جمله->tl_کار = کار;

    // Redirecting stdout and stderr doesn't work at the job level.  Instead
    // open the file here and handle it in.  opt->jo_io was changed in
    // setup_job_options(), use the original flags here.
    إذا (orig_opt->jo_io[PART_بیرون] == JIO_FILE)
    {
	char_u *fname = opt->jo_io_name[PART_بیرون];

	ch_log(channel, "Opening output file %s", fname);
	جمله->tl_بیرون_fd = mch_fopen((حرف *)fname, WRITEBIN);
	إذا (جمله->tl_بیرون_fd == عدم)
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
    إذا (کار != عدم)
    {
	کار->jv_channel = عدم;
	کار_cleanup(کار);
    }
    جمله->tl_کار = عدم;
    إذا (jo != عدم)
	CloseHandle(jo);

    إذا (جمله->tl_siex.lpAttributeList != عدم)
    {
	pDeleteProcThreadAttributeList(جمله->tl_siex.lpAttributeList);
	vim_free(جمله->tl_siex.lpAttributeList);
    }
    جمله->tl_siex.lpAttributeList = عدم;
    إذا (o_theirs != عدم)
	CloseHandle(o_theirs);
    إذا (o_ours != عدم)
	CloseHandle(o_ours);
    إذا (i_ours != عدم)
	CloseHandle(i_ours);
    إذا (i_theirs != عدم)
	CloseHandle(i_theirs);
    إذا (جمله->tl_conpty != عدم)
	pClosePseudoConsole(جمله->tl_conpty);
    جمله->tl_conpty = عدم;
    // Propagate errors that occur in CreateProcess
    إذا (errmsg)
	semsg("CreateProcess failed: %s", errmsg);
    أرجع FAIL;
}

    ساكن فراغ
conpty_جمله_report_winsize(جمله_T *جمله, صحيح ردیفها_2, صحيح تعداد_ستون)
{
    COORD consize;

    consize.X = تعداد_ستون;
    consize.Y = ردیفها_2;
    pResizePseudoConsole(جمله->tl_conpty, consize);
}

    ساكن فراغ
جمله_free_conpty(جمله_T *جمله)
{
    إذا (جمله->tl_siex.lpAttributeList != عدم)
    {
	pDeleteProcThreadAttributeList(جمله->tl_siex.lpAttributeList);
	vim_free(جمله->tl_siex.lpAttributeList);
    }
    جمله->tl_siex.lpAttributeList = عدم;
    إذا (جمله->tl_conpty != عدم)
	pClosePseudoConsole(جمله->tl_conpty);
    جمله->tl_conpty = عدم;
}

    صحيح
use_conpty(فراغ)
{
    أرجع has_conpty;
}

# define WINPTY_SPAWN_FLAG_AUTO_SHUTDOWN 1ul
# define WINPTY_SPAWN_FLAG_EXIT_AFTER_SHUTDOWN 2ull
# define WINPTY_MOUSE_نما_FORCE		2

فراغ* (*winpty_config_new)(UINT64, فراغ*);
فراغ* (*winpty_open)(فراغ*, فراغ*);
فراغ* (*winpty_spawn_config_new)(UINT64, فراغ*, LPCWSTR, فراغ*, فراغ*, فراغ*);
BOOL (*winpty_spawn)(فراغ*, فراغ*, HANDLE*, HANDLE*, DWORD*, فراغ*);
فراغ (*winpty_config_مجموعه_mouse_نما)(فراغ*, صحيح);
فراغ (*winpty_config_مجموعه_initial_اندازه)(فراغ*, صحيح, صحيح);
LPCWSTR (*winpty_conin_name)(فراغ*);
LPCWSTR (*winpty_conout_name)(فراغ*);
LPCWSTR (*winpty_conerr_name)(فراغ*);
فراغ (*winpty_free)(فراغ*);
فراغ (*winpty_config_free)(فراغ*);
فراغ (*winpty_spawn_config_free)(فراغ*);
فراغ (*winpty_error_free)(فراغ*);
LPCWSTR (*winpty_error_پیام)(فراغ*);
BOOL (*winpty_مجموعه_اندازه)(فراغ*, صحيح, صحيح, فراغ*);
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
    } winpty_مدخل[] =
    {
	{"winpty_conerr_name", (FARPROC*)&winpty_conerr_name},
	{"winpty_config_free", (FARPROC*)&winpty_config_free},
	{"winpty_config_new", (FARPROC*)&winpty_config_new},
	{"winpty_config_set_mouse_mode",
				      (FARPROC*)&winpty_config_مجموعه_mouse_نما},
	{"winpty_config_set_initial_size",
				    (FARPROC*)&winpty_config_مجموعه_initial_اندازه},
	{"winpty_conin_name", (FARPROC*)&winpty_conin_name},
	{"winpty_conout_name", (FARPROC*)&winpty_conout_name},
	{"winpty_error_free", (FARPROC*)&winpty_error_free},
	{"winpty_free", (FARPROC*)&winpty_free},
	{"winpty_open", (FARPROC*)&winpty_open},
	{"winpty_spawn", (FARPROC*)&winpty_spawn},
	{"winpty_spawn_config_free", (FARPROC*)&winpty_spawn_config_free},
	{"winpty_spawn_config_new", (FARPROC*)&winpty_spawn_config_new},
	{"winpty_error_msg", (FARPROC*)&winpty_error_پیام},
	{"winpty_set_size", (FARPROC*)&winpty_مجموعه_اندازه},
	{"winpty_agent_process", (FARPROC*)&winpty_agent_process},
	{عدم, عدم}
    };

    // No need to initialize twice.
    إذا (hWinPtyDLL)
	أرجع OK;
    // Load winpty.dll, prefer using the 'winptydll' option, fall back to just
    // winpty.dll.
    إذا (*p_winptydll != NUL)
	hWinPtyDLL = vim_بارگیری_Lib((حرف *)p_winptydll);
    إذا (!hWinPtyDLL)
	hWinPtyDLL = vim_بارگیری_Lib(WINPTY_DLL);
    إذا (!hWinPtyDLL)
    {
	إذا (verbose)
	    semsg(_(e_could_not_بارگیری_library_str_str),
		    (*p_winptydll != NUL ? p_winptydll : (char_u *)WINPTY_DLL),
		    GetWin32Error());
	أرجع FAIL;
    }
    لكل (i = 0; winpty_مدخل[i].name != عدم
					 && winpty_مدخل[i].ptr != عدم; ++i)
    {
	إذا ((*winpty_مدخل[i].ptr = (FARPROC)Get_Proc_آدرس(hWinPtyDLL,
					      winpty_مدخل[i].name)) == عدم)
	{
	    إذا (verbose)
		semsg(_(e_could_not_بارگیری_library_function_str), winpty_مدخل[i].name);
	    hWinPtyDLL = عدم;
	    أرجع FAIL;
	}
    }

    أرجع OK;
}

    ساكن صحيح
winpty_جمله_and_کار_init(
	جمله_T	    *جمله,
	typval_T    *argvar,
	حرف	    **argv استفادهنشده,
	jobopt_T    *opt,
	jobopt_T    *orig_opt)
{
    WCHAR	    *cmd_wchar = عدم;
    WCHAR	    *cwd_wchar = عدم;
    WCHAR	    *env_wchar = عدم;
    channel_T	    *channel = عدم;
    کار_T	    *کار = عدم;
    DWORD	    error;
    HANDLE	    jo = عدم;
    HANDLE	    فرزند_process_handle;
    HANDLE	    فرزند_thread_handle;
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
	cmd = ga_cmd.ga_داده;
    }
    إذا (cmd == عدم || *cmd == NUL)
    {
	emsg(_(e_invalid_آرگومان));
	اذهب failed;
    }

    جمله->tl_arg0_cmd = vim_strsave(cmd);

    cmd_wchar = enc_مقصد_utf16(cmd, عدم);
    ga_clear(&ga_cmd);
    إذا (cmd_wchar == عدم)
	اذهب failed;
    إذا (opt->jo_cwd != عدم)
    {
	cwd_wchar = enc_مقصد_utf16(opt->jo_cwd, عدم);
	إذا (cwd_wchar == عدم)
	    اذهب failed;
    }

    win32_build_env(opt->jo_env, &ga_env, TRUE);
    env_wchar = ga_env.ga_داده;

    جمله->tl_winpty_config = winpty_config_new(0, &winpty_err);
    إذا (جمله->tl_winpty_config == عدم)
	اذهب failed;

    winpty_config_مجموعه_mouse_نما(جمله->tl_winpty_config,
						    WINPTY_MOUSE_نما_FORCE);
    winpty_config_مجموعه_initial_اندازه(جمله->tl_winpty_config,
						 جمله->tl_تعداد_ستون, جمله->tl_ردیفها);
    جمله->tl_winpty = winpty_open(جمله->tl_winpty_config, &winpty_err);
    إذا (جمله->tl_winpty == عدم)
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

    channel = جمع_کن_channel();
    إذا (channel == عدم)
	اذهب failed;

    کار = کار_alloc();
    إذا (کار == عدم)
	اذهب failed;
    إذا (argvar->v_type == VAR_STRING)
    {
	صحيح argc;

	build_argv_from_string(cmd, &کار->jv_argv, &argc);
    }
    وإلا
    {
	صحيح argc;

	build_argv_from_list(argvar->vval.v_list, &کار->jv_argv, &argc);
    }

    إذا (opt->jo_مجموعه & JO_درون_BUF)
	کار->jv_درون_buf = buflist_findnr(opt->jo_io_buf[PART_درون]);

    إذا (!winpty_spawn(جمله->tl_winpty, spawn_config, &فرزند_process_handle,
	    &فرزند_thread_handle, &error, &winpty_err))
	اذهب failed;

    channel_مجموعه_pipes(channel,
	(sock_T)CreateFileW(
	    winpty_conin_name(جمله->tl_winpty),
	    GENERIC_WRITE, 0, عدم,
	    OPEN_EXISTING, 0, عدم),
	(sock_T)CreateFileW(
	    winpty_conout_name(جمله->tl_winpty),
	    GENERIC_READ, 0, عدم,
	    OPEN_EXISTING, 0, عدم),
	(sock_T)CreateFileW(
	    winpty_conerr_name(جمله->tl_winpty),
	    GENERIC_READ, 0, عدم,
	    OPEN_EXISTING, 0, عدم));

    // Write lines with CR instead of NL.
    channel->ch_write_متن_نما = TRUE;

    jo = Create_کار_Object(عدم, عدم);
    إذا (jo == عدم)
	اذهب failed;

    إذا (!تخصیص_بده_Process_مقصد_کار_Object(jo, فرزند_process_handle))
    {
	// Failed, switch the way to terminate process with TerminateProcess.
	CloseHandle(jo);
	jo = عدم;
    }

    winpty_spawn_config_free(spawn_config);
    vim_free(cmd_wchar);
    vim_free(cwd_wchar);
    vim_free(env_wchar);

    إذا (create_vterm(جمله, جمله->tl_ردیفها, جمله->tl_تعداد_ستون) == FAIL)
	اذهب failed;

# إذا defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    إذا (جمله_use_palette())
    {
	إذا (جمله->tl_palette != عدم)
	    مجموعه_vterm_palette(جمله->tl_vterm, جمله->tl_palette);
	وإلا
	    init_vterm_ansi_colors(جمله->tl_vterm);
    }
# endif

    channel_مجموعه_کار(channel, کار, opt);
    کار_مجموعه_options(کار, opt);

    کار->jv_channel = channel;
    کار->jv_proc_info.hProcess = فرزند_process_handle;
    کار->jv_proc_info.dwProcessId = GetProcessId(فرزند_process_handle);
    کار->jv_کار_object = jo;
    کار->jv_وضعیت = کار_STARTED;
    کار->jv_tty_درون = utf16_مقصد_enc(
	    (short_u *)winpty_conin_name(جمله->tl_winpty), عدم);
    کار->jv_tty_بیرون = utf16_مقصد_enc(
	    (short_u *)winpty_conout_name(جمله->tl_winpty), عدم);
    کار->jv_tty_type = vim_strsave((char_u *)"winpty");
    ++کار->jv_refcount;
    جمله->tl_کار = کار;

    // Redirecting stdout and stderr doesn't work at the job level.  Instead
    // open the file here and handle it in.  opt->jo_io was changed in
    // setup_job_options(), use the original flags here.
    إذا (orig_opt->jo_io[PART_بیرون] == JIO_FILE)
    {
	char_u *fname = opt->jo_io_name[PART_بیرون];

	ch_log(channel, "Opening output file %s", fname);
	جمله->tl_بیرون_fd = mch_fopen((حرف *)fname, WRITEBIN);
	إذا (جمله->tl_بیرون_fd == عدم)
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
    إذا (کار != عدم)
    {
	کار->jv_channel = عدم;
	کار_cleanup(کار);
    }
    جمله->tl_کار = عدم;
    إذا (jo != عدم)
	CloseHandle(jo);
    إذا (جمله->tl_winpty != عدم)
	winpty_free(جمله->tl_winpty);
    جمله->tl_winpty = عدم;
    إذا (جمله->tl_winpty_config != عدم)
	winpty_config_free(جمله->tl_winpty_config);
    جمله->tl_winpty_config = عدم;
    إذا (winpty_err != عدم)
    {
	حرف *پیام = (حرف *)utf16_مقصد_enc(
				(short_u *)winpty_error_پیام(winpty_err), عدم);

	إذا (پیام != عدم)
	{
	    emsg(پیام);
	    vim_free(پیام);
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
جمله_and_کار_init(
	جمله_T	    *جمله,
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
	أرجع conpty_جمله_and_کار_init(جمله, argvar, argv, opt, orig_opt);

    إذا (use_winpty)
	أرجع winpty_جمله_and_کار_init(جمله, argvar, argv, opt, orig_opt);

    // error
    أرجع dyn_winpty_init(TRUE);
}

    ساكن صحيح
create_pty_only(جمله_T *جمله, jobopt_T *options)
{
    HANDLE	    h_Pipe_درون = INVALID_HANDLE_مقدار;
    HANDLE	    h_Pipe_بیرون = INVALID_HANDLE_مقدار;
    حرف	    درون_name[80], بیرون_name[80];
    channel_T	    *channel = عدم;

    إذا (create_vterm(جمله, جمله->tl_ردیفها, جمله->tl_تعداد_ستون) == FAIL)
	أرجع FAIL;

    vim_snprintf(درون_name, حجم(درون_name), "\\\\.\\pipe\\vim-%d-in-%d",
	    GetCurrentProcessId(),
	    curbuf->b_fnum);
    h_Pipe_درون = CreateNamedPipe(درون_name, PIPE_ACCESS_OUTBOUND,
	    PIPE_TYPE_پیام | PIPE_NOWAIT,
	    PIPE_UNLIMITED_INSTANCES,
	    0, 0, NMPWAIT_NOWAIT, عدم);
    إذا (h_Pipe_درون == INVALID_HANDLE_مقدار)
	اذهب failed;

    vim_snprintf(بیرون_name, حجم(بیرون_name), "\\\\.\\pipe\\vim-%d-out-%d",
	    GetCurrentProcessId(),
	    curbuf->b_fnum);
    h_Pipe_بیرون = CreateNamedPipe(بیرون_name, PIPE_ACCESS_INBOUND,
	    PIPE_TYPE_پیام | PIPE_NOWAIT,
	    PIPE_UNLIMITED_INSTANCES,
	    0, 0, 0, عدم);
    إذا (h_Pipe_بیرون == INVALID_HANDLE_مقدار)
	اذهب failed;

    ConnectNamedPipe(h_Pipe_درون, عدم);
    ConnectNamedPipe(h_Pipe_بیرون, عدم);

    جمله->tl_کار = کار_alloc();
    إذا (جمله->tl_کار == عدم)
	اذهب failed;
    ++جمله->tl_کار->jv_refcount;

    // behave like the job is already finished
    جمله->tl_کار->jv_وضعیت = کار_پایانیافته;

    channel = جمع_کن_channel();
    إذا (channel == عدم)
	اذهب failed;
    جمله->tl_کار->jv_channel = channel;
    channel->ch_keep_open = TRUE;
    channel->ch_named_pipe = TRUE;

    channel_مجموعه_pipes(channel,
	(sock_T)h_Pipe_درون,
	(sock_T)h_Pipe_بیرون,
	(sock_T)h_Pipe_بیرون);
    channel_مجموعه_کار(channel, جمله->tl_کار, options);
    جمله->tl_کار->jv_tty_درون = vim_strsave((char_u*)درون_name);
    جمله->tl_کار->jv_tty_بیرون = vim_strsave((char_u*)بیرون_name);

    أرجع OK;

failed:
    إذا (h_Pipe_درون != عدم)
	CloseHandle(h_Pipe_درون);
    إذا (h_Pipe_بیرون != عدم)
	CloseHandle(h_Pipe_بیرون);
    أرجع FAIL;
}

/*
 * Free the terminal emulator part of "term".
 */
    ساكن فراغ
جمله_free_vterm(جمله_T *جمله)
{
    جمله_free_conpty(جمله);
    إذا (جمله->tl_winpty != عدم)
	winpty_free(جمله->tl_winpty);
    جمله->tl_winpty = عدم;
    إذا (جمله->tl_winpty_config != عدم)
	winpty_config_free(جمله->tl_winpty_config);
    جمله->tl_winpty_config = عدم;
    إذا (جمله->tl_vterm != عدم)
	vterm_free(جمله->tl_vterm);
    جمله->tl_vterm = عدم;
}

/*
 * Report the size to the terminal.
 */
    ساكن فراغ
جمله_report_winsize(جمله_T *جمله, صحيح ردیفها_2, صحيح تعداد_ستون)
{
    إذا (جمله->tl_conpty)
	conpty_جمله_report_winsize(جمله, ردیفها_2, تعداد_ستون);
    إذا (جمله->tl_winpty)
	winpty_مجموعه_اندازه(جمله->tl_winpty, تعداد_ستون, ردیفها_2, عدم);
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
جمله_and_کار_init(
	جمله_T	    *جمله,
	typval_T    *argvar,
	حرف	    **argv,
	jobopt_T    *opt,
	jobopt_T    *orig_opt استفادهنشده)
{
    جمله->tl_arg0_cmd = عدم;

    إذا (create_vterm(جمله, جمله->tl_ردیفها, جمله->tl_تعداد_ستون) == FAIL)
	أرجع FAIL;

# إذا defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    إذا (جمله_use_palette())
    {
	إذا (جمله->tl_palette != عدم)
	    مجموعه_vterm_palette(جمله->tl_vterm, جمله->tl_palette);
	وإلا
	    init_vterm_ansi_colors(جمله->tl_vterm);
    }
# endif

    // This may change a string in "argvar".
    جمله->tl_کار = کار_شروع(argvar, argv, opt, &جمله->tl_کار);
    إذا (جمله->tl_کار != عدم)
	++جمله->tl_کار->jv_refcount;

    أرجع جمله->tl_کار != عدم
	&& جمله->tl_کار->jv_channel != عدم
	&& جمله->tl_کار->jv_وضعیت != کار_FAILED ? OK : FAIL;
}

    ساكن صحيح
create_pty_only(جمله_T *جمله, jobopt_T *opt)
{
    إذا (create_vterm(جمله, جمله->tl_ردیفها, جمله->tl_تعداد_ستون) == FAIL)
	أرجع FAIL;

    جمله->tl_کار = کار_alloc();
    إذا (جمله->tl_کار == عدم)
	أرجع FAIL;
    ++جمله->tl_کار->jv_refcount;

    // behave like the job is already finished
    جمله->tl_کار->jv_وضعیت = کار_پایانیافته;

    أرجع mch_create_pty_channel(جمله->tl_کار, opt);
}

/*
 * Free the terminal emulator part of "term".
 */
    ساكن فراغ
جمله_free_vterm(جمله_T *جمله)
{
    إذا (جمله->tl_vterm != عدم)
	vterm_free(جمله->tl_vterm);
    جمله->tl_vterm = عدم;
}

/*
 * Report the size to the terminal.
 */
    ساكن فراغ
جمله_report_winsize(جمله_T *جمله, صحيح ردیفها_2, صحيح تعداد_ستون)
{
    // Use an ioctl() to report the new window size to the job.
    إذا (جمله->tl_کار == عدم || جمله->tl_کار->jv_channel == عدم)
	أرجع;

    صحيح fd = -1;
    صحيح part;

    لكل (part = PART_بیرون; part < PART_شمار; ++part)
    {
	fd = جمله->tl_کار->jv_channel->ch_part[part].ch_fd;
	إذا (mch_isatty(fd))
	    اكسر;
    }
    إذا (part < PART_شمار && mch_report_winsize(fd, ردیفها_2, تعداد_ستون) == OK)
	mch_signal_کار(جمله->tl_کار, (char_u *)"winch");
}

#پایان_اگر

#پایان_اگر // FEAT_TERMINAL
