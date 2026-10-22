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

#که_نه_تعریف_شوی MIN
# define MIN(x,y) ((x) < (y) ? (x) : (y))
#د_که_پای
#که_نه_تعریف_شوی MAX
# define MAX(x,y) ((x) > (y) ? (x) : (y))
#د_که_پای

#ضمن "libvterm/include/vterm.h"

// This is VTermScreenCell without the characters, thus much smaller.
تعريفنوع بنية {
    VTermScreenCellAttrs	attrs;
    حرف			پلنوالی;
    VTermColor			fg;
    VTermColor			bg;
} cellattr_T;

تعريفنوع بنية sb_کرښه_S {
    صحيح		sb_ستون_شمېر;	// can differ per line
    صحيح		sb_بایټونه;	// length in bytes of text
    cellattr_T	*sb_cells;	// allocated
    cellattr_T	sb_fill_attr;	// for short line
    char_u	*sb_متن;	// for tl_scrollback_postponed
    char_u	continuation;
} sb_کرښه_T;

#که_تعریف_شوی MSWIN
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
#د_که_پای

// typedef term_T in structs.h
بنية terminal_S {
    غړی_T	*tl_بل;

    VTerm	*tl_vterm;
    دنده_T	*tl_دنده;
    buf_T	*tl_بفر;
#إذا defined(FEAT_GUI)
    صحيح		tl_system;	// when non-zero used for :!cmd output
    صحيح		tl_toprow;	// row with first line of system terminal
#د_که_پای

    // Set when setting the size of a vterm, reset after redrawing.
    صحيح		tl_vterm_اندازه_بدل_شوی;

    صحيح		tl_نورمال_ډېرپېښ; // TRUE: Terminal-Normal mode
    صحيح		tl_channel_closing;
    صحيح		tl_channel_closed;
    صحيح		tl_channel_recently_closed; // still need to handle tl_finish

    صحيح		tl_پای;
#تعریف_کړه TL_پای_UNSET	    NUL
#تعریف_کړه TL_پای_CLOSE	    'c'	// ++close or :terminal without argument
#تعریف_کړه TL_پای_NOCLOSE   'n'	// ++noclose
#تعریف_کړه TL_پای_OPEN	    'o'	// ++open
    char_u	*tl_opencmd;
    char_u	*tl_eof_chars;
    char_u	*tl_api;	// prefix for terminal API function

    char_u	*tl_arg0_cmd;	// To format the status bar

#که_تعریف_شوی MSWIN
    فراغ	*tl_winpty_config;
    فراغ	*tl_winpty;

    HPCON	tl_conpty;
    DYN_STARTUPINFOEXW tl_siex;	// Structure that always needs to be hold

    FILE	*tl_بهر_fd;
#د_که_پای
#إذا defined(FEAT_SESSION)
    char_u	*tl_command;
#د_که_پای
    char_u	*tl_kill;

    // last known vterm size
    صحيح		tl_لیکې;
    صحيح		tl_ستون_شمېر;

    char_u	*tl_title; // NULL or allocated
    char_u	*tl_حالت_متن; // NULL or allocated

    // Range of screen rows to update.  Zero based.
    صحيح		tl_dirty_لیکه_پیل; // MAX_ROW if nothing dirty
    صحيح		tl_dirty_لیکه_پای_ټکی;   // row below last one to update
    صحيح		tl_dirty_snapshot;  // text updated after making snapshot
#که_تعریف_شوی FEAT_TIMERS
    صحيح		tl_timer_سېټ;
    proftime_T	tl_timer_due;
#د_که_پای
    صحيح		tl_postponed_scroll;	// to be scrolled up

    garray_T	tl_scrollback;
    صحيح		tl_scrollback_scrolled;
    garray_T	tl_scrollback_postponed;
    صحيح		tl_scrollback_snapshot;
    صحيح		tl_بفر_scrolled;

    char_u	*tl_highlight_name; // replaces "Terminal"; allocated

    cellattr_T	tl_default_color;

    linenr_T	tl_سر_diff_لیکې;   // rows of top diff file or zero
    linenr_T	tl_bot_diff_لیکې;   // rows of bottom diff file

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

#تعریف_کړه TMODE_ONCE 1	    // CTRL-\ CTRL-N used
#تعریف_کړه TMODE_LOOP 2	    // CTRL-W N used

/*
 * List of all active terminals.
 */
ساكن غړی_T *لومړی_غړی = عدم;

// Terminal active in terminal_loop().
ساكن غړی_T *دننه_terminal_loop = عدم;

#که_تعریف_شوی MSWIN
ساكن BOOL has_winpty = FALSE;
ساكن BOOL has_conpty = FALSE;
#د_که_پای

#تعریف_کړه MAX_لیکه 999999	    // used for tl_dirty_row_end to update all rows
#تعریف_کړه کلی_BUF_LEN 200

#تعریف_کړه FOR_ALL_TERMS(غړی)	\
    لكل ((غړی) = لومړی_غړی; (غړی) != عدم; (غړی) = (غړی)->tl_بل)

/*
 * Functions with separate implementation for MS-Windows and Unix-like systems.
 */
ساكن صحيح غړی_and_دنده_init(غړی_T *غړی, typval_T *argvar, حرف **argv, jobopt_T *opt, jobopt_T *orig_opt);
ساكن صحيح create_pty_only(غړی_T *غړی, jobopt_T *opt);
ساكن فراغ غړی_report_winsize(غړی_T *غړی, صحيح لیکې_2, صحيح ستون_شمېر);
ساكن فراغ غړی_free_vterm(غړی_T *غړی);
#که_تعریف_شوی FEAT_GUI
ساكن فراغ update_system_غړی(غړی_T *غړی);
#د_که_پای

ساكن فراغ handle_postponed_scrollback(غړی_T *غړی);

// The character that we know (or assume) that the terminal expects for the
// backspace key.
ساكن صحيح غړی_backspace_char = BS;

// Store the last set and the desired cursor properties, so that we only update
// them when needed.  Doing it unnecessary may result in flicker.
ساكن char_u	*وروستی_سېټ_cursor_color = عدم;
ساكن char_u	*desired_cursor_color = عدم;
ساكن صحيح	وروستی_سېټ_cursor_shape = -1;
ساكن صحيح	desired_cursor_shape = -1;
ساكن صحيح	وروستی_سېټ_cursor_blink = -1;
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
parse_termwinsize(win_T *wp, صحيح *لیکې_2, صحيح *ستون_شمېر)
{
    صحيح	minsize = FALSE;

    *لیکې_2 = 0;
    *ستون_شمېر = 0;

    إذا (*wp->w_p_tws == NUL)
	أرجع FALSE;

    char_u *p = vim_strchr(wp->w_p_tws, 'x');

    // Syntax of value was already checked when it's set.
    إذا (p == عدم)
    {
	minsize = TRUE;
	p = vim_strchr(wp->w_p_tws, '*');
    }
    *لیکې_2 = atoi((حرف *)wp->w_p_tws);
    *ستون_شمېر = atoi((حرف *)p + 1);
    إذا (*لیکې_2 > VTERM_MAX_لیکې)
	*لیکې_2 = VTERM_MAX_لیکې;
    إذا (*ستون_شمېر > VTERM_MAX_ستون_شمېر)
	*ستون_شمېر = VTERM_MAX_ستون_شمېر;
    أرجع minsize;
}

/*
 * Determine the terminal size from 'termwinsize' and the current window.
 */
    ساكن فراغ
سېټ_غړی_and_win_اندازه(غړی_T *غړی, jobopt_T *opt)
{
    صحيح لیکې_2, ستون_شمېر;
    صحيح	minsize;

#که_تعریف_شوی FEAT_GUI
    إذا (غړی->tl_system)
    {
	// Use the entire screen (excluding the tabpanel area) for the system
	// commands.  However, it will start at the command line and scroll up
	// as needed, using tl_toprow.
	غړی->tl_لیکې = لیکې;
	غړی->tl_ستون_شمېر = topframe->fr_پلنوالی;
	أرجع;
    }
#د_که_پای
    غړی->tl_لیکې = curwin->w_لوړوالی;
    غړی->tl_ستون_شمېر = curwin->w_پلنوالی;

    minsize = parse_termwinsize(curwin, &لیکې_2, &ستون_شمېر);
    إذا (minsize)
    {
	إذا (غړی->tl_لیکې < لیکې_2)
	    غړی->tl_لیکې = لیکې_2;
	إذا (غړی->tl_ستون_شمېر < ستون_شمېر)
	    غړی->tl_ستون_شمېر = ستون_شمېر;
    }
    إذا ((opt->jo_set2 & JO2_غړی_لیکې))
	غړی->tl_لیکې = opt->jo_غړی_لیکې;
    وإلا إذا (لیکې_2 != 0)
	غړی->tl_لیکې = لیکې_2;
    إذا ((opt->jo_set2 & JO2_غړی_ستون_شمېر))
	غړی->tl_ستون_شمېر = opt->jo_غړی_ستون_شمېر;
    وإلا إذا (ستون_شمېر != 0)
	غړی->tl_ستون_شمېر = ستون_شمېر;

    إذا (!opt->jo_hidden)
    {
	إذا (غړی->tl_لیکې != curwin->w_لوړوالی)
	    win_setheight_win(غړی->tl_لیکې, curwin);
	إذا (غړی->tl_ستون_شمېر != curwin->w_پلنوالی)
	    win_setwidth_win(غړی->tl_ستون_شمېر, curwin);

	// Set 'winsize' now to avoid a resize at the next redraw.
	إذا (!minsize && *curwin->w_p_tws != NUL)
	{
	    char_u buf[100];

	    vim_snprintf((حرف *)buf, 100, "%dx%d",
						 غړی->tl_لیکې, غړی->tl_ستون_شمېر);
	    سېټ_option_ارزښت_give_err((char_u *)"termwinsize",
							   0L, buf, OPT_LOCAL);
	}
    }
}

/*
 * Initialize job options for a terminal job.
 * Caller may overrule some of them.
 */
    فراغ
init_دنده_options(jobopt_T *opt)
{
    clear_دنده_options(opt);

    opt->jo_ډېرپېښ = CH_ډېرپېښ_RAW;
    opt->jo_بهر_ډېرپېښ = CH_ډېرپېښ_RAW;
    opt->jo_err_ډېرپېښ = CH_ډېرپېښ_RAW;
    opt->jo_سېټ = JO_ډېرپېښ | JO_بهر_ډېرپېښ | JO_ERR_ډېرپېښ;
}

/*
 * Set job options mandatory for a terminal job.
 */
    ساكن فراغ
تیاری_دنده_options(jobopt_T *opt, صحيح لیکې_2, صحيح ستون_شمېر)
{
#که_نه_تعریف_شوی MSWIN
    // Win32: Redirecting the job output won't work, thus always connect stdout
    // here.
    إذا (!(opt->jo_سېټ & JO_بهر_IO))
#د_که_پای
    {
	// Connect stdout to the terminal.
	opt->jo_io[PART_بهر] = JIO_بفر;
	opt->jo_io_buf[PART_بهر] = curbuf->b_fnum;
	opt->jo_modifiable[PART_بهر] = 0;
	opt->jo_سېټ |= JO_بهر_IO + JO_بهر_BUF + JO_بهر_MODIFIABLE;
    }

#که_نه_تعریف_شوی MSWIN
    // Win32: Redirecting the job output won't work, thus always connect stderr
    // here.
    إذا (!(opt->jo_سېټ & JO_ERR_IO))
#د_که_پای
    {
	// Connect stderr to the terminal.
	opt->jo_io[PART_ERR] = JIO_بفر;
	opt->jo_io_buf[PART_ERR] = curbuf->b_fnum;
	opt->jo_modifiable[PART_ERR] = 0;
	opt->jo_سېټ |= JO_ERR_IO + JO_ERR_BUF + JO_ERR_MODIFIABLE;
    }

    opt->jo_pty = TRUE;
    إذا ((opt->jo_set2 & JO2_غړی_لیکې) == 0)
	opt->jo_غړی_لیکې = لیکې_2;
    إذا ((opt->jo_set2 & JO2_غړی_ستون_شمېر) == 0)
	opt->jo_غړی_ستون_شمېر = ستون_شمېر;
}

/*
 * Flush messages on channels.
 */
    ساكن فراغ
غړی_flush_messages(فراغ)
{
    mch_check_messages();
    parse_queued_messages();
}

/*
 * Close a terminal buffer (and its window).  Used when creating the terminal
 * fails.
 */
    ساكن فراغ
غړی_close_بفر(buf_T *buf, buf_T *old_curbuf)
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
    do_بفر(DOBUF_WIPE, DOBUF_لومړی, FORWARD, buf->b_fnum, TRUE);
}

/*
 * Start a terminal window and return its buffer.
 * Use either "argvar" or "argv", the other must be NULL.
 * When "flags" has TERM_START_NOJOB only create the buffer, b_term and open
 * the window.
 * Returns NULL when failed.
 */
    buf_T *
غړی_پیل(
	typval_T    *argvar,
	حرف	    **argv,
	jobopt_T    *opt,
	صحيح	    flags)
{
    exarg_T	وېشل_ea;
    win_T	*old_curwin = curwin;
    غړی_T	*غړی;
    buf_T	*old_curbuf = عدم;
    صحيح		res;
    buf_T	*newbuf;
    صحيح		vertical = opt->jo_vertical || (cmdmod.cmod_وېشل & WSP_VERT);
    jobopt_T	orig_opt;  // only partly filled
    pos_T	save_cursor;

    إذا (check_restricted() || check_secure())
	أرجع عدم;
    إذا (cmdwin_type != 0)
    {
	emsg(_(e_cannot_open_terminal_from_command_کرښه_کړکۍ));
	أرجع عدم;
    }

    إذا ((opt->jo_سېټ & (JO_دننه_IO + JO_بهر_IO + JO_ERR_IO))
					 == (JO_دننه_IO + JO_بهر_IO + JO_ERR_IO)
	|| (!(opt->jo_سېټ & JO_بهر_IO) && (opt->jo_سېټ & JO_بهر_BUF))
	|| (!(opt->jo_سېټ & JO_ERR_IO) && (opt->jo_سېټ & JO_ERR_BUF))
	|| (argvar != عدم
	    && argvar->v_type == VAR_LIST
	    && argvar->vval.v_list != عدم
	    && argvar->vval.v_list->lv_لومړی == &range_list_توکی))
    {
	emsg(_(e_invalid_دلیل));
	أرجع عدم;
    }

    غړی = ALLOC_CLEAR_ONE(غړی_T);
    إذا (غړی == عدم)
	أرجع عدم;
    غړی->tl_dirty_لیکه_پای_ټکی = MAX_لیکه;
    غړی->tl_cursor_visible = TRUE;
    غړی->tl_cursor_shape = VTERM_PROP_CURSORSHAPE_بلاک;
    غړی->tl_پای = opt->jo_غړی_پای;
#که_تعریف_شوی FEAT_GUI
    غړی->tl_system = (flags & غړی_پیل_SYSTEM);
#د_که_پای
    ga_init2(&غړی->tl_scrollback, حجم(sb_کرښه_T), 300);
    ga_init2(&غړی->tl_scrollback_postponed, حجم(sb_کرښه_T), 300);
    ga_init2(&غړی->tl_osc_buf, حجم(حرف), 300);

    setpcmark();
    CLEAR_ډګر(وېشل_ea);
    إذا (opt->jo_curwin)
    {
	// Create a new buffer in the current window.
	إذا (!can_abandon(curbuf, flags & غړی_پیل_FORCEIT))
	{
	    no_write_پیغام();
	    vim_free(غړی);
	    أرجع عدم;
	}
	إذا (do_ecmd(0, عدم, عدم, &وېشل_ea, ECMD_ONE,
		      (buf_hide(curwin->w_بفر) ? ECMD_HIDE : 0)
			  + ((flags & غړی_پیل_FORCEIT) ? ECMD_FORCEIT : 0),
							       curwin) == FAIL)
	{
	    vim_free(غړی);
	    أرجع عدم;
	}
    }
    وإلا إذا (opt->jo_hidden || (flags & غړی_پیل_SYSTEM))
    {
	buf_T *buf;

	// Create a new buffer without a window. Make it the current buffer for
	// a moment to be able to do the initializations.
	buf = buflist_new((char_u *)"", عدم, (linenr_T)0,
							 BLN_NEW | BLN_LISTED);
	إذا (buf == عدم || ml_open(buf) == FAIL)
	{
	    vim_free(غړی);
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
	وېشل_ea.cmdidx = CMD_new;
	وېشل_ea.cmd = (char_u *)"new";
	وېشل_ea.arg = (char_u *)"";
	إذا (opt->jo_غړی_لیکې > 0 && !vertical)
	{
	    وېشل_ea.line2 = opt->jo_غړی_لیکې;
	    وېشل_ea.addr_شمېر = 1;
	}
	إذا (opt->jo_غړی_ستون_شمېر > 0 && vertical)
	{
	    وېشل_ea.line2 = opt->jo_غړی_ستون_شمېر;
	    وېشل_ea.addr_شمېر = 1;
	}

	صحيح cmod_وېشل_modified = FALSE;
	إذا (vertical)
	{
	    إذا (!(cmdmod.cmod_وېشل & WSP_VERT))
		cmod_وېشل_modified = TRUE;
	    cmdmod.cmod_وېشل |= WSP_VERT;
	}
	ex_splitview(&وېشل_ea);
	إذا (cmod_وېشل_modified)
	    cmdmod.cmod_وېشل &= ~WSP_VERT;
	إذا (curwin == old_curwin)
	{
	    // split failed
	    vim_free(غړی);
	    أرجع عدم;
	}
    }
    غړی->tl_بفر = curbuf;
    curbuf->b_غړی = غړی;

    إذا (!opt->jo_hidden)
    {
	// Only one size was taken care of with :new, do the other one.  With
	// "curwin" both need to be done.
	إذا (opt->jo_غړی_لیکې > 0 && (opt->jo_curwin || vertical))
	    win_setheight(opt->jo_غړی_لیکې);
	إذا (opt->jo_غړی_ستون_شمېر > 0 && (opt->jo_curwin || !vertical))
	    win_setwidth(opt->jo_غړی_ستون_شمېر);
    }

    // Link the new terminal in the list of active terminals.
    غړی->tl_بل = لومړی_غړی;
    لومړی_غړی = غړی;

    apply_autocmds(EVENT_BUFFILEPRE, عدم, عدم, FALSE, curbuf);

    إذا (opt->jo_غړی_name != عدم)
    {
	vim_free(curbuf->b_ffname);
	curbuf->b_ffname = vim_strsave(opt->jo_غړی_name);
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
			       &argvar->vval.v_list->lv_لومړی->li_tv)) == عدم)
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

    إذا (opt->jo_غړی_opencmd != عدم)
	غړی->tl_opencmd = vim_strsave(opt->jo_غړی_opencmd);

    إذا (opt->jo_eof_chars != عدم)
	غړی->tl_eof_chars = vim_strsave(opt->jo_eof_chars);

    سېټ_string_option_مستقیم((char_u *)"buftype", -1,
				  (char_u *)"terminal", OPT_FREE|OPT_LOCAL, 0);
    // Avoid that 'buftype' is reset when this buffer is entered.
    curbuf->b_p_initialized = true;

    // Mark the buffer as not modifiable. It can only be made modifiable after
    // the job finished.
    curbuf->b_p_ma = FALSE;

    سېټ_غړی_and_win_اندازه(غړی, opt);
#که_تعریف_شوی MSWIN
    mch_memmove(orig_opt.jo_io, opt->jo_io, حجم(orig_opt.jo_io));
#د_که_پای
    تیاری_دنده_options(opt, غړی->tl_لیکې, غړی->tl_ستون_شمېر);

    إذا (flags & غړی_پیل_NOJOB)
	أرجع curbuf;

#إذا defined(FEAT_SESSION)
    // Remember the command for the session file.
    إذا (opt->jo_غړی_norestore || argv != عدم)
	غړی->tl_command = vim_strsave((char_u *)"NONE");
    وإلا إذا (argvar->v_type == VAR_STRING)
    {
	char_u	*cmd = argvar->vval.v_string;

	إذا (cmd != عدم && STRCMP(cmd, p_sh) != 0)
	    غړی->tl_command = vim_strsave(cmd);
    }
    وإلا إذا (argvar->v_type == VAR_LIST
	    && argvar->vval.v_list != عدم
	    && argvar->vval.v_list->lv_len > 0)
    {
	garray_T	ga;
	listitem_T	*توکی;

	ga_init2(&ga, 1, 100);
	FOR_ALL_LIST_توکي(argvar->vval.v_list, توکی)
	{
	    char_u *s = tv_get_string_chk(&توکی->li_tv);
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
	إذا (توکی == عدم)
	{
	    ga_append(&ga, NUL);
	    غړی->tl_command = ga.ga_معلومات;
	}
	وإلا
	    ga_clear(&ga);
    }
#د_که_پای

    إذا (opt->jo_غړی_kill != عدم)
    {
	char_u *p = skiptowhite(opt->jo_غړی_kill);

	غړی->tl_kill = vim_strnsave(opt->jo_غړی_kill, p - opt->jo_غړی_kill);
    }

    إذا (opt->jo_غړی_api != عدم)
    {
	char_u *p = skiptowhite(opt->jo_غړی_api);

	غړی->tl_api = vim_strnsave(opt->jo_غړی_api, p - opt->jo_غړی_api);
    }
    وإلا
	غړی->tl_api = vim_strsave((char_u *)"Tapi_");

    إذا (opt->jo_set2 & JO2_غړی_HIGHLIGHT)
	غړی->tl_highlight_name = vim_strsave(opt->jo_غړی_highlight);

#إذا defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    // Save the user-defined palette, it is only used in GUI (or 'tgc' is on).
    إذا (opt->jo_set2 & JO2_ANSI_COLORS)
    {
	غړی->tl_palette = ALLOC_MULT(long_u, 16);
	إذا (غړی->tl_palette == عدم)
	{
	    vim_free(غړی);
	    أرجع عدم;
	}
	حافظه_نقل(غړی->tl_palette, opt->jo_ansi_colors, حجم(long_u) * 16);
    }
#د_که_پای

    // System dependent: setup the vterm and maybe start the job in it.
    إذا (argv == عدم
	    && argvar->v_type == VAR_STRING
	    && argvar->vval.v_string != عدم
	    && STRCMP(argvar->vval.v_string, "NONE") == 0)
	res = create_pty_only(غړی, opt);
    وإلا
	res = غړی_and_دنده_init(غړی, argvar, argv, opt, &orig_opt);

    newbuf = curbuf;
    إذا (res == OK)
    {
	// Get and remember the size we ended up with.  Update the pty.
	vterm_get_اندازه(غړی->tl_vterm, &غړی->tl_لیکې, &غړی->tl_ستون_شمېر);
	غړی_report_winsize(غړی, غړی->tl_لیکې, غړی->tl_ستون_شمېر);
#که_تعریف_شوی FEAT_GUI
	إذا (غړی->tl_system)
	{
	    // display first line below typed command
	    غړی->tl_toprow = پیغام_لیکه + 1;
	    غړی->tl_dirty_لیکه_پای_ټکی = 0;
	}
#د_که_پای

	// Make sure we don't get stuck on sending keys to the job, it leads to
	// a deadlock if the job is waiting for Vim to read.
	channel_سېټ_nonblock(غړی->tl_دنده->jv_channel, PART_دننه);

	إذا (old_curbuf != عدم)
	{
	    --curbuf->b_nwindows;
	    curbuf = old_curbuf;
	    curwin->w_بفر = curbuf;
	    curwin->w_cursor = save_cursor;
	    ++curbuf->b_nwindows;
	}
	وإلا إذا (vgetc_busy
#که_تعریف_شوی FEAT_TIMERS
		|| timer_busy
#د_که_پای
		|| ننوت_busy)
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
	غړی_close_بفر(curbuf, old_curbuf);
	أرجع عدم;
    }

    apply_autocmds(EVENT_TERMINALOPEN, عدم, عدم, FALSE, newbuf);
    إذا (!opt->jo_hidden && !(flags & غړی_پیل_SYSTEM))
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

    init_دنده_options(&opt);

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

#تعریف_کړه OPTARG_HAS(name) ((صحيح)(p - cmd) == حجم(name) - 1 \
				 && STRNICMP(cmd, name, حجم(name) - 1) == 0)
	إذا (OPTARG_HAS("close"))
	    opt.jo_غړی_پای = 'c';
	وإلا إذا (OPTARG_HAS("noclose"))
	    opt.jo_غړی_پای = 'n';
	وإلا إذا (OPTARG_HAS("open"))
	    opt.jo_غړی_پای = 'o';
	وإلا إذا (OPTARG_HAS("curwin"))
	    opt.jo_curwin = 1;
	وإلا إذا (OPTARG_HAS("hidden"))
	    opt.jo_hidden = 1;
	وإلا إذا (OPTARG_HAS("norestore"))
	    opt.jo_غړی_norestore = 1;
	وإلا إذا (OPTARG_HAS("shell"))
	    opt_shell = TRUE;
	وإلا إذا (OPTARG_HAS("kill") && ep != عدم)
	{
	    opt.jo_set2 |= JO2_غړی_KILL;
	    opt.jo_غړی_kill = ep + 1;
	    p = skiptowhite(cmd);
	}
	وإلا إذا (OPTARG_HAS("api"))
	{
	    opt.jo_set2 |= JO2_غړی_API;
	    إذا (ep != عدم)
	    {
		opt.jo_غړی_api = ep + 1;
		p = skiptowhite(cmd);
	    }
	    وإلا
		opt.jo_غړی_api = عدم;
	}
	وإلا إذا (OPTARG_HAS("rows") && ep != عدم && خوندي_isdigit(ep[1]))
	{
	    opt.jo_set2 |= JO2_غړی_لیکې;
	    opt.jo_غړی_لیکې = atoi((حرف *)ep + 1);
	    p = skiptowhite(cmd);
	}
	وإلا إذا (OPTARG_HAS("cols") && ep != عدم && خوندي_isdigit(ep[1]))
	{
	    opt.jo_set2 |= JO2_غړی_ستون_شمېر;
	    opt.jo_غړی_ستون_شمېر = atoi((حرف *)ep + 1);
	    p = skiptowhite(cmd);
	}
	وإلا إذا (OPTARG_HAS("eof") && ep != عدم)
	{
	    char_u *buf = عدم;
	    char_u *کلیګانې;

	    vim_free(opt.jo_eof_chars);
	    p = skiptowhite(cmd);
	    *p = NUL;
	    کلیګانې = replace_termcodes(ep + 1, &buf, 0,
		    REPTERM_FROM_PART | REPTERM_DO_LT | REPTERM_SPECIAL, عدم);
	    opt.jo_set2 |= JO2_EOF_CHARS;
	    opt.jo_eof_chars = vim_strsave(کلیګانې);
	    vim_free(buf);
	    *p = ' ';
	}
#که_تعریف_شوی MSWIN
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
		semsg(_(e_invalid_ارزښت_for_دلیل_str), "type");
		اذهب theend;
	    }
	    opt.jo_set2 |= JO2_TTY_TYPE;
	    opt.jo_tty_type = tty_type;
	}
#د_که_پای
	وإلا
	{
	    إذا (*p)
		*p = NUL;
	    semsg(_(e_invalid_attribute_str), cmd);
	    اذهب theend;
	}
#تعریف_لغوه OPTARG_HAS
	cmd = skipwhite(p);
    }
    إذا (*cmd == NUL)
    {
	// Make a copy of 'shell', an autocommand may change the option.
	tofree = cmd = vim_strsave(p_sh);

	// default to close when the shell exits
	إذا (opt.jo_غړی_پای == NUL)
	    opt.jo_غړی_پای = TL_پای_CLOSE;
    }

    إذا (eap->addr_شمېر > 0)
    {
	// Write lines from current buffer to the job.
	opt.jo_سېټ |= JO_دننه_IO | JO_دننه_BUF | JO_دننه_سر | JO_دننه_BOT;
	opt.jo_io[PART_دننه] = JIO_بفر;
	opt.jo_io_buf[PART_دننه] = curbuf->b_fnum;
	opt.jo_دننه_سر = eap->line1;
	opt.jo_دننه_bot = eap->line2;
    }

    إذا (opt_shell && tofree == عدم)
    {
#که_تعریف_شوی UNIX
	حرف	**argv = عدم;
	char_u	*tofree1 = عدم;
	char_u	*tofree2 = عدم;

	// :term ++shell command
	إذا (unix_build_argv(cmd, &argv, &tofree1, &tofree2) == OK)
	    غړی_پیل(عدم, argv, &opt, eap->forceit ? غړی_پیل_FORCEIT : 0);
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
#د_که_پای
    }
    argvar[0].v_type = VAR_STRING;
    argvar[0].vval.v_string = cmd;
    argvar[1].v_type = VAR_UNKNOWN;
    غړی_پیل(argvar, عدم, &opt, eap->forceit ? غړی_پیل_FORCEIT : 0);

theend:
    vim_free(tofree);
    vim_free(opt.jo_eof_chars);
}

    ساكن char_u *
get_terminalopt_name(expand_T *xp ناکارول_شوی, صحيح idx)
{
    // Note: Keep this in sync with ex_terminal.
    ساكن حرف *(p_termopt_ارزښتونه[]) =
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

    إذا (idx < (صحيح)ARRAY_اوږدتیا(p_termopt_ارزښتونه))
	أرجع (char_u*)p_termopt_ارزښتونه[idx];
    أرجع عدم;
}

    ساكن char_u *
get_termkill_name(expand_T *xp ناکارول_شوی, صحيح idx)
{
    // These are platform-specific values used for job_stop(). They are defined
    // in each platform's mch_signal_job(). Just use a unified auto-complete
    // list for simplicity.
    ساكن حرف *(p_termkill_ارزښتونه[]) =
    {
	"term",
	"hup",
	"quit",
	"int",
	"kill",
	"winch",
    };

    إذا (idx < (صحيح)ARRAY_اوږدتیا(p_termkill_ارزښتونه))
	أرجع (char_u*)p_termkill_ارزښتونه[idx];
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
    إذا (xp->xp_نمونه > xp->xp_کرښه && *(xp->xp_نمونه-1) == '=')
    {
	char_u *(*cb)(expand_T *, صحيح) = عدم;

	char_u *name_پای_ټکی = xp->xp_نمونه - 1;
	إذا (name_پای_ټکی - xp->xp_کرښه >= 4
		&& STRNCMP(name_پای_ټکی - 4, "kill", 4) == 0)
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
غړی_write_session(FILE *fd, win_T *wp, hashtab_T *terminal_bufs)
{
    ثابت صحيح	bufnr = wp->w_بفر->b_fnum;
    غړی_T	*غړی = wp->w_بفر->b_غړی;

    إذا (terminal_bufs != عدم && wp->w_بفر->b_nwindows > 1)
    {
	// There are multiple views into this terminal buffer. We don't want to
	// create the terminal multiple times. If it's the first time, create,
	// otherwise link to the first buffer.
	حرف	    id_as_str[NUMBUFLEN];
	hashitem_T  *ثبت;

	vim_snprintf(id_as_str, حجم(id_as_str), "%d", bufnr);

	ثبت = hash_پیدا_کړه(terminal_bufs, (char_u *)id_as_str);
	إذا (!HASHITEM_تشه(ثبت))
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
		غړی->tl_ستون_شمېر, Columns / 2, Columns,
		غړی->tl_لیکې, لیکې / 2, لیکې) < 0)
	أرجع FAIL;
# ifdef MSWIN
    إذا (fprintf(fd, ".. ' ++type=%s' ", غړی->tl_دنده->jv_tty_type) < 0)
	أرجع FAIL;
# endif
    إذا (غړی->tl_command != عدم)
    {
	char_u *quoted_command = string_quote(غړی->tl_command, FALSE);
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
	حرف *hash_کلی = alloc(NUMBUFLEN);

	vim_snprintf(hash_کلی, NUMBUFLEN, "%d", bufnr);
	hash_جمع_کړه(terminal_bufs, (char_u *)hash_کلی, "terminal session");
    }

    أرجع put_eol(fd);
}

/*
 * Return TRUE if "buf" has a terminal that should be restored.
 */
    صحيح
غړی_should_restore(buf_T *buf)
{
    غړی_T	*غړی = buf->b_غړی;

    أرجع غړی != عدم && (غړی->tl_command == عدم
				     || STRCMP(غړی->tl_command, "NONE") != 0);
}
#د_که_پای

/*
 * Free the scrollback buffer for "term".
 */
    ساكن فراغ
free_scrollback(غړی_T *غړی)
{
    صحيح i;

    لكل (i = 0; i < غړی->tl_scrollback.ga_len; ++i)
	vim_free(((sb_کرښه_T *)غړی->tl_scrollback.ga_معلومات + i)->sb_cells);
    ga_clear(&غړی->tl_scrollback);
    لكل (i = 0; i < غړی->tl_scrollback_postponed.ga_len; ++i)
	vim_free(((sb_کرښه_T *)غړی->tl_scrollback_postponed.ga_معلومات + i)->sb_cells);
    ga_clear(&غړی->tl_scrollback_postponed);
}


// Terminals that need to be freed soon.
ساكن غړی_T	*terminals_منزل_free = عدم;

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
    غړی_T	*غړی = buf->b_غړی;
    غړی_T	*tp;

    إذا (غړی == عدم)
	أرجع;

    // Unlink the terminal form the list of terminals.
    إذا (لومړی_غړی == غړی)
	لومړی_غړی = غړی->tl_بل;
    وإلا
	لكل (tp = لومړی_غړی; tp->tl_بل != عدم; tp = tp->tl_بل)
	    إذا (tp->tl_بل == غړی)
	    {
		tp->tl_بل = غړی->tl_بل;
		اكسر;
	    }

    إذا (غړی->tl_دنده != عدم)
    {
	إذا (غړی->tl_دنده->jv_حالت != دنده_ENDED
		&& غړی->tl_دنده->jv_حالت != دنده_پای_ته_رسېدلی
		&& غړی->tl_دنده->jv_حالت != دنده_FAILED)
	    دنده_stop(غړی->tl_دنده, عدم, "kill");
	دنده_unref(غړی->tl_دنده);
    }
    غړی->tl_بل = terminals_منزل_free;
    terminals_منزل_free = غړی;

    buf->b_غړی = عدم;
    إذا (دننه_terminal_loop == غړی)
	دننه_terminal_loop = عدم;
}

    فراغ
free_ناکارول_شوی_terminals(فراغ)
{
    طالما (terminals_منزل_free != عدم)
    {
	غړی_T	    *غړی = terminals_منزل_free;

	terminals_منزل_free = غړی->tl_بل;

	free_scrollback(غړی);
	ga_clear(&غړی->tl_osc_buf);

	غړی_free_vterm(غړی);
	vim_free(غړی->tl_api);
	vim_free(غړی->tl_title);
#که_تعریف_شوی FEAT_SESSION
	vim_free(غړی->tl_command);
#د_که_پای
	vim_free(غړی->tl_kill);
	vim_free(غړی->tl_حالت_متن);
	vim_free(غړی->tl_opencmd);
	vim_free(غړی->tl_eof_chars);
	vim_free(غړی->tl_arg0_cmd);
#که_تعریف_شوی MSWIN
	إذا (غړی->tl_بهر_fd != عدم)
	    fclose(غړی->tl_بهر_fd);
#د_که_پای
	vim_free(غړی->tl_highlight_name);
	vim_free(غړی->tl_cursor_color);
	vim_free(غړی->tl_palette);
	vim_free(غړی);
    }
}

/*
 * Get the part that is connected to the tty. Normally this is PART_IN, but
 * when writing buffer lines to the job it can be another.  This makes it
 * possible to do "1,5term vim -".
 */
    ساكن ch_part_T
get_tty_part(غړی_T *غړی ناکارول_شوی)
{
#که_تعریف_شوی UNIX
    ch_part_T	parts[3] = {PART_دننه, PART_بهر, PART_ERR};
    صحيح		i;

    لكل (i = 0; i < 3; ++i)
    {
	صحيح fd = غړی->tl_دنده->jv_channel->ch_part[parts[i]].ch_fd;

	إذا (mch_isatty(fd))
	    أرجع parts[i];
    }
#د_که_پای
    أرجع PART_دننه;
}

/*
 * Read any vterm output and send it on the channel.
 */
    ساكن فراغ
غړی_forward_output(غړی_T *غړی)
{
    VTerm *vterm = غړی->tl_vterm;
    حرف   buf[کلی_BUF_LEN];
    size_t curlen = vterm_output_read(vterm, buf, کلی_BUF_LEN);

    إذا (curlen > 0)
	channel_send(غړی->tl_دنده->jv_channel, get_tty_part(غړی),
					     (char_u *)buf, (صحيح)curlen, عدم);
}

/*
 * Write job output "msg[len]" to the vterm.
 */
    ساكن فراغ
غړی_write_دنده_output(غړی_T *غړی, char_u *پیغام_arg, size_t len_arg)
{
    char_u	*پیغام = پیغام_arg;
    size_t	len = len_arg;
    VTerm	*vterm = غړی->tl_vterm;
    size_t	prevlen = vterm_output_get_بفر_current(vterm);
    size_t	حد = غړی->tl_بفر->b_p_twsl * غړی->tl_ستون_شمېر * 3;

    // Limit the length to 'termwinscroll' * cols * 3 bytes.  Keep the text at
    // the end.
    إذا (len > حد)
    {
	char_u *p = پیغام + len - حد;

	p -= (*mb_سر_off)(پیغام, p);
	len -= p - پیغام;
	پیغام = p;
    }

    vterm_ننوت_write(vterm, (حرف *)پیغام, len);

    // flush vterm buffer when vterm responded to control sequence
    إذا (prevlen != vterm_output_get_بفر_current(vterm))
	غړی_forward_output(غړی);

    // this invokes the damage callbacks
    vterm_screen_flush_damage(vterm_obtain_screen(vterm));
}

    ساكن فراغ
position_cursor(win_T *wp, VTermPos *pos)
{
    wp->w_wrow = MIN(pos->لیکه, MAX(0, wp->w_لوړوالی - 1));
    wp->w_wcol = MIN(pos->ستون, MAX(0, wp->w_پلنوالی - 1));
#که_تعریف_شوی FEAT_PROP_POPUP
    إذا (popup_is_popup(wp))
    {
	wp->w_wrow += popup_سر_extra(wp);
	wp->w_wcol += popup_کیڼ_extra(wp);
	wp->w_flags |= WFLAG_WCOL_OFF_ADDED | WFLAG_WROW_OFF_ADDED;
    }
    وإلا
	wp->w_flags &= ~(WFLAG_WCOL_OFF_ADDED | WFLAG_WROW_OFF_ADDED);
#د_که_پای
    wp->w_valid |= (VALID_WCOL|VALID_WROW);
}

    ساكن فراغ
update_cursor(غړی_T *غړی, صحيح redraw)
{
    إذا (غړی->tl_نورمال_ډېرپېښ)
	أرجع;
#که_تعریف_شوی FEAT_GUI
    إذا (غړی->tl_system)
	windgoto(غړی->tl_cursor_pos.لیکه + غړی->tl_toprow,
		firstwin->w_wincol + غړی->tl_cursor_pos.ستون);
    وإلا
#د_که_پای
    إذا (!غړی_دنده_running(غړی))
	// avoid the cursor positioned below the last used line
	setcursor();
    وإلا
    {
	// do not use the window cursor position
	position_cursor(curwin, &curbuf->b_غړی->tl_cursor_pos);
	windgoto(W_WINROW(curwin) + curwin->w_wrow,
		 curwin->w_wincol + curwin->w_wcol);
    }
    إذا (redraw)
    {
	aco_save_T	aco;

	إذا (غړی->tl_بفر == curbuf && غړی->tl_cursor_visible)
	    cursor_on();
	بهر_flush();
#که_تعریف_شوی FEAT_GUI
	إذا (gui.دننه_use)
	{
	    gui_update_cursor(FALSE, FALSE);
	    gui_mch_flush();
	}
#د_که_پای
	// Make sure an invoked autocmd doesn't delete the buffer (and the
	// terminal) under our fingers.
	++غړی->tl_بفر->b_locked;

	// save and restore curwin and curbuf, in case the autocmd changes them
	aucmd_prepbuf(&aco, curbuf);
	apply_autocmds(EVENT_TEXTCHANGEDT, عدم, عدم, FALSE, غړی->tl_بفر);
	aucmd_restbuf(&aco);

	--غړی->tl_بفر->b_locked;
    }
}

/*
 * Find the location of a scrollbackline in the buffer
 */
    ساكن فراغ
scrollbackline_pos_دننه_buf(غړی_T *غړی, صحيح لیکه, linenr_T *lnum, صحيح *پیل_ستون, size_t *پیل_pos)
{
    sb_کرښه_T	*کرښې = (sb_کرښه_T *)غړی->tl_scrollback.ga_معلومات;
    linenr_T    calc_lnum = غړی->tl_بفر_scrolled;
    size_t      calc_pos = 0;
    صحيح         calc_ستون = 0;
    صحيح         i;

    إذا (لیکه < 0 || لیکه >= غړی->tl_scrollback.ga_len)
	أرجع;

    إذا (لیکه > غړی->tl_scrollback_scrolled)
    {
	// Lookback how far along in the top line we are
	لكل (i = غړی->tl_scrollback_scrolled + 1; i > 0 && کرښې[i].continuation; --i)
	{
	    calc_pos += کرښې[i - 1].sb_بایټونه;
	    calc_ستون += کرښې[i - 1].sb_ستون_شمېر;
	}
	i = غړی->tl_scrollback_scrolled + 1;
	calc_lnum = غړی->tl_بفر_scrolled + 1;
	// Do not count this line's bytes/cols twice
	إذا (کرښې[i].continuation)
	    ++i;
    }
    وإلا
    {
	i = 1;
	calc_lnum = 1;
    }

    لكل (; i <= لیکه; ++i)
    {
	إذا (!کرښې[i].continuation)
	{
	    ++calc_lnum;
	    calc_pos = 0;
	    calc_ستون = 0;
	}
	وإلا
	{
	    calc_pos += کرښې[i - 1].sb_بایټونه;
	    calc_ستون += کرښې[i - 1].sb_ستون_شمېر;
	}
    }

    *lnum = calc_lnum;
    إذا (پیل_ستون)
	*پیل_ستون = calc_ستون;
    إذا (پیل_pos)
	*پیل_pos = calc_pos;
}

/*
 * Find the location of a buffer line in the scrollback
 */
    ساكن فراغ
bufline_pos_دننه_scrollback(غړی_T *غړی, linenr_T lnum, صحيح ستون, صحيح *لیکه, صحيح *wrapped_ستون)
{
    buf_T	*buf = غړی->tl_بفر;
    sb_کرښه_T	*کرښې = (sb_کرښه_T *)غړی->tl_scrollback.ga_معلومات;
    linenr_T    calc_لیکه = غړی->tl_scrollback_scrolled;
    صحيح         calc_ستون = ستون;
    linenr_T    l;

    إذا (lnum > buf->b_ml.ml_کرښه_شمېر)
	أرجع;

    إذا (lnum > غړی->tl_بفر_scrolled)
    {
	calc_لیکه = غړی->tl_scrollback_scrolled;
	l = غړی->tl_بفر_scrolled + 1;

	طالما (calc_لیکه < غړی->tl_scrollback.ga_len && کرښې[calc_لیکه].continuation)
	    ++calc_لیکه;
    }
    وإلا
    {
	calc_لیکه = 0;
	l = 1;
    }

    طالما (calc_لیکه < غړی->tl_scrollback.ga_len && l < lnum)
    {
	++calc_لیکه;
	إذا (!کرښې[calc_لیکه].continuation)
	    ++l;
    }

    طالما (calc_لیکه + 1 < غړی->tl_scrollback.ga_len && کرښې[calc_لیکه + 1].continuation
	    && calc_ستون >= کرښې[calc_لیکه].sb_ستون_شمېر)
    {
	calc_ستون -= کرښې[calc_لیکه].sb_ستون_شمېر;
	++calc_لیکه;
    }

    إذا (لیکه)
	*لیکه = calc_لیکه;
    إذا (wrapped_ستون)
	*wrapped_ستون = calc_ستون;
}

/*
 * Invoked when "msg" output from a job was received.  Write it to the terminal
 * of "buffer".
 */
    فراغ
write_منزل_غړی(buf_T *بفر, char_u *پیغام, channel_T *channel)
{
    size_t	len = STRLEN(پیغام);
    غړی_T	*غړی = بفر->b_غړی;

#که_تعریف_شوی MSWIN
    // Win32: Cannot redirect output of the job, intercept it here and write to
    // the file.
    إذا (غړی->tl_بهر_fd != عدم)
    {
	ch_log(channel, "Writing %d bytes to output file", (صحيح)len);
	fwrite(پیغام, len, 1, غړی->tl_بهر_fd);
	أرجع;
    }
#د_که_پای

    إذا (غړی->tl_vterm == عدم)
    {
	ch_log(channel, "NOT writing %d bytes to terminal", (صحيح)len);
	أرجع;
    }
    ch_log(channel, "writing %d bytes to terminal", (صحيح)len);
    cursor_off();
    غړی_write_دنده_output(غړی, پیغام, len);

#که_تعریف_شوی FEAT_GUI
    إذا (غړی->tl_system)
    {
	// show system output, scrolling up the screen as needed
	update_system_غړی(غړی);
	update_cursor(غړی, TRUE);
    }
    وإلا
#د_که_پای
    // In Terminal-Normal mode we are displaying the buffer, not the terminal
    // contents, thus no screen update is needed.
    إذا (!غړی->tl_نورمال_ډېرپېښ)
    {
	// Don't use update_screen() when editing the command line, it gets
	// cleared.
	// TODO: only update once in a while.
	ch_log(غړی->tl_دنده->jv_channel, "updating screen");
	إذا (بفر == curbuf && (State & ډېرپېښ_CMDLINE) == 0)
	{
	    update_screen(UPD_VALID_NO_UPDATE);
#إذا defined(FEAT_TABPANEL)
	    إذا (redraw_tabpanel)
		draw_tabpanel();
#د_که_پای
	    // update_screen() can be slow, check the terminal wasn't closed
	    // already
	    إذا (بفر == curbuf && curbuf->b_غړی != عدم)
		update_cursor(curbuf->b_غړی, TRUE);
	}
	وإلا
	    redraw_after_callback(TRUE, FALSE);
    }
}

/*
 * Send a mouse position and click to the vterm
 */
    ساكن صحيح
غړی_send_mouse(VTerm *vterm, صحيح button, صحيح pressed)
{
    VTermModifier   mod = VTERM_MOD_NONE;
    صحيح		    لیکه = mouse_لیکه - W_WINROW(curwin);
    صحيح		    ستون = mouse_ستون - curwin->w_wincol;

#که_تعریف_شوی FEAT_PROP_POPUP
    إذا (popup_is_popup(curwin))
    {
	لیکه -= popup_سر_extra(curwin);
	ستون -= popup_کیڼ_extra(curwin);
    }
#د_که_پای
    vterm_mouse_move(vterm, لیکه, ستون, mod);
    إذا (button != 0)
	vterm_mouse_button(vterm, button, pressed, mod);
    أرجع TRUE;
}

ساكن صحيح enter_mouse_ستون = -1;
ساكن صحيح enter_mouse_لیکه = -1;

/*
 * Handle a mouse click, drag or release.
 * Return TRUE when a mouse event is sent to the terminal.
 */
    ساكن صحيح
غړی_mouse_click(VTerm *vterm, صحيح کلی)
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
	بدل (کلی)
	{
	حالة K_LEFTDRAG:
	حالة K_LEFTRELEASE:
	حالة K_RIGHTDRAG:
	حالة K_RIGHTRELEASE:
		// Ignore drag and release events when the button-down wasn't
		// seen before.
		إذا (ignore_drag_release)
		{
		    صحيح save_mouse_ستون, save_mouse_لیکه;

		    إذا (enter_mouse_ستون < 0)
			اكسر;

		    // mouse click in the window gave us focus, handle that
		    // click now
		    save_mouse_ستون = mouse_ستون;
		    save_mouse_لیکه = mouse_لیکه;
		    mouse_ستون = enter_mouse_ستون;
		    mouse_لیکه = enter_mouse_لیکه;
		    clip_modeless(MOUSE_کیڼ, TRUE, FALSE);
		    mouse_ستون = save_mouse_ستون;
		    mouse_لیکه = save_mouse_لیکه;
		}
		// FALLTHROUGH
	حالة K_LEFTMOUSE:
	حالة K_RIGHTMOUSE:
		إذا (کلی == K_LEFTRELEASE || کلی == K_RIGHTRELEASE)
		    ignore_drag_release = TRUE;
		وإلا
		    ignore_drag_release = FALSE;
		// Should we call mouse_has() here?
		إذا (clip_ستوری.شته)
		{
		    صحيح	    button, is_click, is_drag;

		    button = get_mouse_button(KEY2TERMCAP1(کلی),
							 &is_click, &is_drag);
		    إذا (mouse_model_popup() && button == MOUSE_کیڼ
					       && (mod_ماسک & MOD_ماسک_ښويدنه))
		    {
			// Translate shift-left to right button.
			button = MOUSE_ښی;
			mod_ماسک &= ~MOD_ماسک_ښويدنه;
		    }
		    clip_modeless(button, is_click, is_drag);
		}
		اكسر;

	حالة K_MIDDLEMOUSE:
		إذا (clip_ستوری.شته)
		    ورننباسه_reg('*', TRUE);
		اكسر;
	}
	enter_mouse_ستون = -1;
	أرجع FALSE;
    }
#د_که_پای
    enter_mouse_ستون = -1;

    بدل (کلی)
    {
	حالة K_LEFTMOUSE:
	حالة K_LEFTMOUSE_NM:	غړی_send_mouse(vterm, 1, 1); اكسر;
	حالة K_LEFTDRAG:	غړی_send_mouse(vterm, 1, 1); اكسر;
	حالة K_LEFTRELEASE:
	حالة K_LEFTRELEASE_NM:	غړی_send_mouse(vterm, 1, 0); اكسر;
	حالة K_MOUSEMOVE:	غړی_send_mouse(vterm, 0, 0); اكسر;
	حالة K_MIDDLEMOUSE:	غړی_send_mouse(vterm, 2, 1); اكسر;
	حالة K_MIDDLEDRAG:	غړی_send_mouse(vterm, 2, 1); اكسر;
	حالة K_MIDDLERELEASE:	غړی_send_mouse(vterm, 2, 0); اكسر;
	حالة K_RIGHTMOUSE:	غړی_send_mouse(vterm, 3, 1); اكسر;
	حالة K_RIGHTDRAG:	غړی_send_mouse(vterm, 3, 1); اكسر;
	حالة K_RIGHTRELEASE:	غړی_send_mouse(vterm, 3, 0); اكسر;
    }
    أرجع TRUE;
}

/*
 * Convert typed key "c" with modifiers "modmask" into bytes to send to the
 * job.
 * Return the number of bytes in "buf".
 */
    ساكن صحيح
غړی_convert_کلی(غړی_T *غړی, صحيح c, صحيح modmask, حرف *buf)
{
    VTerm	    *vterm = غړی->tl_vterm;
    VTerm_کلی	    کلی = VTERM_کلی_NONE;
    VTermModifier   mod = VTERM_MOD_NONE;
    صحيح		    other = FALSE;

    بدل (c)
    {
	// don't use VTERM_KEY_ENTER, it may do an unwanted conversion

	حالة K_BS:
#که_تعریف_شوی MSWIN
	    // In ConPTY, we must use VTERM_KEY_BACKSPACE, otherwise it will
	    // delete one word, equivalent to Alt+Backspace.
	    إذا (غړی->tl_conpty)
		کلی = VTERM_کلی_BACKSPACE;
	    وإلا
#د_که_پای
		// don't use VTERM_KEY_BACKSPACE, it always becomes 0x7f DEL
		c = غړی_backspace_char;
	    اكسر;

#که_تعریف_شوی MSWIN
	حالة BS:
	    // In ConPTY, we must use VTERM_KEY_BACKSPACE, otherwise it will
	    // delete one word, equivalent to Alt+Backspace.
	    إذا (غړی->tl_conpty)
		کلی = VTERM_کلی_BACKSPACE;
	    اكسر;
#د_که_پای

	حالة ESC:		کلی = VTERM_کلی_ESCAPE; اكسر;
	حالة K_DEL:		کلی = VTERM_کلی_DEL; اكسر;
	حالة K_ښکته:		کلی = VTERM_کلی_ښکته; اكسر;
	حالة K_S_ښکته:		mod = VTERM_MOD_ښويدنه;
				کلی = VTERM_کلی_ښکته; اكسر;
	حالة K_پای_ټکی:		کلی = VTERM_کلی_پای_ټکی; اكسر;
	حالة K_S_پای_ټکی:		mod = VTERM_MOD_ښويدنه;
				کلی = VTERM_کلی_پای_ټکی; اكسر;
	حالة K_C_پای_ټکی:		mod = VTERM_MOD_CTRL;
				کلی = VTERM_کلی_پای_ټکی; اكسر;
	حالة K_F10:		کلی = VTERM_کلی_FUNCTION(10); اكسر;
	حالة K_F11:		کلی = VTERM_کلی_FUNCTION(11); اكسر;
	حالة K_F12:		کلی = VTERM_کلی_FUNCTION(12); اكسر;
	حالة K_F1:		کلی = VTERM_کلی_FUNCTION(1); اكسر;
	حالة K_F2:		کلی = VTERM_کلی_FUNCTION(2); اكسر;
	حالة K_F3:		کلی = VTERM_کلی_FUNCTION(3); اكسر;
	حالة K_F4:		کلی = VTERM_کلی_FUNCTION(4); اكسر;
	حالة K_F5:		کلی = VTERM_کلی_FUNCTION(5); اكسر;
	حالة K_F6:		کلی = VTERM_کلی_FUNCTION(6); اكسر;
	حالة K_F7:		کلی = VTERM_کلی_FUNCTION(7); اكسر;
	حالة K_F8:		کلی = VTERM_کلی_FUNCTION(8); اكسر;
	حالة K_F9:		کلی = VTERM_کلی_FUNCTION(9); اكسر;
	حالة K_HOME:		کلی = VTERM_کلی_HOME; اكسر;
	حالة K_S_HOME:		mod = VTERM_MOD_ښويدنه;
				کلی = VTERM_کلی_HOME; اكسر;
	حالة K_C_HOME:		mod = VTERM_MOD_CTRL;
				کلی = VTERM_کلی_HOME; اكسر;
	حالة K_INS:		کلی = VTERM_کلی_INS; اكسر;
	حالة K_K0:		کلی = VTERM_کلی_KP_0; اكسر;
	حالة K_K1:		کلی = VTERM_کلی_KP_1; اكسر;
	حالة K_K2:		کلی = VTERM_کلی_KP_2; اكسر;
	حالة K_K3:		کلی = VTERM_کلی_KP_3; اكسر;
	حالة K_K4:		کلی = VTERM_کلی_KP_4; اكسر;
	حالة K_K5:		کلی = VTERM_کلی_KP_5; اكسر;
	حالة K_K6:		کلی = VTERM_کلی_KP_6; اكسر;
	حالة K_K7:		کلی = VTERM_کلی_KP_7; اكسر;
	حالة K_K8:		کلی = VTERM_کلی_KP_8; اكسر;
	حالة K_K9:		کلی = VTERM_کلی_KP_9; اكسر;
	حالة K_KDEL:		کلی = VTERM_کلی_DEL; اكسر; // TODO
	حالة K_KDIVIDE:		کلی = VTERM_کلی_KP_DIVIDE; اكسر;
	حالة K_KEND:		کلی = VTERM_کلی_KP_1; اكسر; // TODO
	حالة K_KENTER:		کلی = VTERM_کلی_KP_ENTER; اكسر;
	حالة K_KHOME:		کلی = VTERM_کلی_KP_7; اكسر; // TODO
	حالة K_KINS:		کلی = VTERM_کلی_KP_0; اكسر; // TODO
	حالة K_KMINUS:		کلی = VTERM_کلی_KP_MINUS; اكسر;
	حالة K_KMULTIPLY:	کلی = VTERM_کلی_KP_MULT; اكسر;
	حالة K_KPAGEDOWN:	کلی = VTERM_کلی_KP_3; اكسر; // TODO
	حالة K_KPAGEUP:		کلی = VTERM_کلی_KP_9; اكسر; // TODO
	حالة K_KPLUS:		کلی = VTERM_کلی_KP_PLUS; اكسر;
	حالة K_KPOINT:		کلی = VTERM_کلی_KP_PERIOD; اكسر;
	حالة K_کیڼ:		کلی = VTERM_کلی_کیڼ; اكسر;
	حالة K_S_کیڼ:		mod = VTERM_MOD_ښويدنه;
				کلی = VTERM_کلی_کیڼ; اكسر;
	حالة K_C_کیڼ:		mod = VTERM_MOD_CTRL;
				کلی = VTERM_کلی_کیڼ; اكسر;
	حالة K_PAGEDOWN:	کلی = VTERM_کلی_PAGEDOWN; اكسر;
	حالة K_PAGEUP:		کلی = VTERM_کلی_PAGEUP; اكسر;
	حالة K_ښی:		کلی = VTERM_کلی_ښی; اكسر;
	حالة K_S_ښی:		mod = VTERM_MOD_ښويدنه;
				کلی = VTERM_کلی_ښی; اكسر;
	حالة K_C_ښی:		mod = VTERM_MOD_CTRL;
				کلی = VTERM_کلی_ښی; اكسر;
	حالة K_پورته:		کلی = VTERM_کلی_پورته; اكسر;
	حالة K_S_پورته:		mod = VTERM_MOD_ښويدنه;
				کلی = VTERM_کلی_پورته; اكسر;
	حالة TAB:		کلی = VTERM_کلی_TAB; اكسر;
	حالة K_S_TAB:		mod = VTERM_MOD_ښويدنه;
				کلی = VTERM_کلی_TAB; اكسر;

	حالة K_MOUSEUP:		other = غړی_send_mouse(vterm, 5, 1); اكسر;
	حالة K_MOUSEDOWN:	other = غړی_send_mouse(vterm, 4, 1); اكسر;
	حالة K_MOUSELEFT:	other = غړی_send_mouse(vterm, 7, 1); اكسر;
	حالة K_MOUSERIGHT:	other = غړی_send_mouse(vterm, 6, 1); اكسر;

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
	حالة K_RIGHTRELEASE:	إذا (!غړی_mouse_click(vterm, c))
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
	حالة K_XF1:		کلی = VTERM_کلی_FUNCTION(1); اكسر;
	حالة K_XF2:		کلی = VTERM_کلی_FUNCTION(2); اكسر;
	حالة K_XF3:		کلی = VTERM_کلی_FUNCTION(3); اكسر;
	حالة K_XF4:		کلی = VTERM_کلی_FUNCTION(4); اكسر;
	حالة K_SELECT:		أرجع 0;
#که_تعریف_شوی FEAT_GUI
	حالة K_VER_SCROLLBAR:	أرجع 0;
	حالة K_HOR_SCROLLBAR:	أرجع 0;
#د_که_پای
#که_تعریف_شوی FEAT_GUI_TABLINE
	حالة K_TABLINE:		أرجع 0;
	حالة K_TABMENU:		أرجع 0;
#د_که_پای
#که_تعریف_شوی FEAT_NETBEANS_INTG
	حالة K_F21:		کلی = VTERM_کلی_FUNCTION(21); اكسر;
#د_که_پای
#که_تعریف_شوی FEAT_DND
	حالة K_DROP:		أرجع 0;
#د_که_پای
	حالة K_CURSORHOLD:	أرجع 0;
	حالة K_PS:		vterm_keyboard_پیل_paste(vterm);
				other = TRUE;
				اكسر;
	حالة K_PE:		vterm_keyboard_پای_ټکی_paste(vterm);
				other = TRUE;
				اكسر;
    }

    // add modifiers for the typed key
    إذا (modmask & MOD_ماسک_ښويدنه)
	mod |= VTERM_MOD_ښويدنه;
    إذا (modmask & MOD_ماسک_CTRL)
	mod |= VTERM_MOD_CTRL;
    إذا (modmask & (MOD_ماسک_ALT | MOD_ماسک_META))
	mod |= VTERM_MOD_ALT;

    // Ctrl-Shift-i may have the key "I" instead of "i", but for the kitty
    // keyboard protocol should use "i".  Applies to all ascii letters.
    إذا (ASCII_ISUPPER(c)
	    && vterm_is_kitty_keyboard(vterm)
	    && mod == (VTERM_MOD_CTRL | VTERM_MOD_ښويدنه))
	c = TOLOWER_ASC(c);

    /*
     * Convert special keys to vterm keys:
     * - Write keys to vterm: vterm_keyboard_key()
     * - Write output to channel.
     */
    إذا (کلی != VTERM_کلی_NONE)
	// Special key, let vterm convert it.
	vterm_keyboard_کلی(vterm, کلی, mod);
    وإلا إذا (!other)
	// Normal character, let vterm convert it.
	vterm_keyboard_unichar(vterm, c, mod);

    // Read back the converted escape sequence.
    أرجع (صحيح)vterm_output_read(vterm, buf, کلی_BUF_LEN);
}

/*
 * Return TRUE if the job for "term" is still running.
 * If "check_job_status" is TRUE update the job status.
 * NOTE: "term" may be freed by callbacks.
 */
    ساكن صحيح
غړی_دنده_running_check(غړی_T *غړی, صحيح check_دنده_حالت)
{
    // Also consider the job finished when the channel is closed, to avoid a
    // race condition when updating the title.
    إذا (غړی == عدم
	|| غړی->tl_دنده == عدم
	|| !channel_is_open(غړی->tl_دنده->jv_channel))
	أرجع FALSE;

    دنده_T *دنده = غړی->tl_دنده;

    // Careful: Checking the job status may invoke callbacks, which close
    // the buffer and terminate "term".  However, "job" will not be freed
    // yet.
    إذا (check_دنده_حالت)
	دنده_حالت(دنده);
    أرجع (دنده->jv_حالت == دنده_STARTED
	    || (دنده->jv_channel != عدم && دنده->jv_channel->ch_keep_open));
}

/*
 * Return TRUE if the job for "term" is still running.
 */
    صحيح
غړی_دنده_running(غړی_T *غړی)
{
    أرجع غړی_دنده_running_check(غړی, FALSE);
}

/*
 * Return TRUE if the job for "term" is still running, ignoring the job was
 * "NONE".
 */
    صحيح
غړی_دنده_running_not_none(غړی_T *غړی)
{
    أرجع غړی_دنده_running(غړی) && !غړی_none_open(غړی);
}

/*
 * Return TRUE if "term" has an active channel and used ":term NONE".
 */
    صحيح
غړی_none_open(غړی_T *غړی)
{
    // Also consider the job finished when the channel is closed, to avoid a
    // race condition when updating the title.
    أرجع غړی != عدم
	&& غړی->tl_دنده != عدم
	&& channel_is_open(غړی->tl_دنده->jv_channel)
	&& غړی->tl_دنده->jv_channel->ch_keep_open;
}

//
// Used to confirm whether we would like to kill a terminal.
// Return OK when the user confirms to kill it.
// Return FAIL if the user selects otherwise.
//
    صحيح
غړی_confirm_stop(buf_T *buf)
{
    char_u	buff[DIALOG_پیغام_اندازه];
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
غړی_try_stop_دنده(buf_T *buf)
{
    صحيح	    شمېر;
    حرف    *how = (حرف *)buf->b_غړی->tl_kill;

#إذا defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
    إذا ((how == عدم || *how == NUL)
			  && (p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)))
    {
	إذا (غړی_confirm_stop(buf) == OK)
	    how = "kill";
	وإلا
	    أرجع FAIL;
    }
#د_که_پای
    إذا (how == عدم || *how == NUL)
	أرجع FAIL;

    دنده_stop(buf->b_غړی->tl_دنده, عدم, how);

    // wait for up to a second for the job to die
    لكل (شمېر = 0; شمېر < 100; ++شمېر)
    {
	دنده_T *دنده;

	// buffer, terminal and job may be cleaned up while waiting
	إذا (!buf_valid(buf)
		|| buf->b_غړی == عدم
		|| buf->b_غړی->tl_دنده == عدم)
	    أرجع OK;
	دنده = buf->b_غړی->tl_دنده;

	// Call job_status() to update jv_status. It may cause the job to be
	// cleaned up but it won't be freed.
	دنده_حالت(دنده);
	إذا (دنده->jv_حالت >= دنده_ENDED)
	    أرجع OK;

	ui_delay(10L, TRUE);
	غړی_flush_messages();
    }
    أرجع FAIL;
}

/*
 * Add the last line of the scrollback buffer to the buffer in the window.
 */
    ساكن فراغ
جمع_کړه_scrollback_کرښه_منزل_بفر(غړی_T *غړی, char_u *متن, صحيح len, صحيح append)
{
    buf_T	*buf = غړی->tl_بفر;
    صحيح		تشه_2 = (buf->b_ml.ml_flags & ML_تشه);
    linenr_T	lnum = buf->b_ml.ml_کرښه_شمېر;

#که_تعریف_شوی MSWIN
    char_u *tmp = متن;
    إذا (!enc_utf8 && enc_codepage > 0)
    {
	WCHAR   *ret = عدم;
	صحيح	اوږدتیا = 0;

	Multi_Byte_منزل_Wide_Char_alloc(CP_UTF8, 0, (حرف*)متن, len + 1,
							   &ret, &اوږدتیا);
	إذا (ret != عدم)
	{
	    Wide_Char_منزل_Multi_Byte_alloc(enc_codepage, 0,
				      ret, اوږدتیا, (حرف **)&متن, &len, 0, 0);
	    vim_free(ret);
	}
    }
#د_که_پای

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

#که_تعریف_شوی MSWIN
    إذا (tmp != متن)
	vim_free(متن);
#د_که_پای
    إذا (تشه_2)
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
    attr->پلنوالی = cell->پلنوالی;
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
جمع_کړه_تشه_scrollback(غړی_T *غړی, cellattr_T *fill_attr, صحيح lnum)
{
    إذا (ga_grow(&غړی->tl_scrollback, 1) == FAIL)
	أرجع FALSE;

    sb_کرښه_T *کرښه = (sb_کرښه_T *)غړی->tl_scrollback.ga_معلومات
	+ غړی->tl_scrollback.ga_len;

    إذا (lnum > 0)
    {
	صحيح i;

	لكل (i = 0; i < غړی->tl_scrollback.ga_len - lnum; ++i)
	{
	    *کرښه = *(کرښه - 1);
	    --کرښه;
	}
    }
    کرښه->sb_ستون_شمېر = 0;
    کرښه->sb_بایټونه = 0;
    کرښه->sb_cells = عدم;
    کرښه->sb_fill_attr = *fill_attr;
    ++غړی->tl_scrollback.ga_len;
    أرجع OK;
}

/*
 * Remove the terminal contents from the scrollback and the buffer.
 * Used before adding a new scrollback line or updating the buffer for lines
 * displayed in the terminal.
 */
    ساكن فراغ
cleanup_scrollback(غړی_T *غړی)
{
    sb_کرښه_T	*کرښه;
    garray_T	*gap;
    char_u	*bufline;
    size_t      bufline_اوږدتیا;

    curbuf = غړی->tl_بفر;
    gap = &غړی->tl_scrollback;
    bufline = ml_get_buf(curbuf, curbuf->b_ml.ml_کرښه_شمېر, FALSE);
    bufline_اوږدتیا = STRLEN(bufline);
    طالما (غړی->tl_scrollback_snapshot && gap->ga_len > 0)
    {
	کرښه = (sb_کرښه_T *)gap->ga_معلومات + gap->ga_len - 1;
	إذا (کرښه->sb_بایټونه < 0 || (size_t)کرښه->sb_بایټونه > bufline_اوږدتیا)
	    اكسر;
	bufline_اوږدتیا -= کرښه->sb_بایټونه;
	إذا (!bufline_اوږدتیا)
	{
	    ml_delete(curbuf->b_ml.ml_کرښه_شمېر);
	    bufline = ml_get_buf(curbuf, curbuf->b_ml.ml_کرښه_شمېر, FALSE);
	    bufline_اوږدتیا = STRLEN(bufline);
	}
	vim_free(کرښه->sb_cells);
	--gap->ga_len;
	--غړی->tl_scrollback_snapshot;
    }
    إذا (bufline_اوږدتیا < STRLEN(bufline))
    {
	char_u *shortened = vim_strnsave(bufline, bufline_اوږدتیا);
	ml_replace(curbuf->b_ml.ml_کرښه_شمېر, shortened, FALSE);
    }
    curbuf = curwin->w_بفر;
    إذا (curbuf == غړی->tl_بفر)
	check_cursor();
}

/*
 * Add the current lines of the terminal to scrollback and to the buffer.
 */
    ساكن فراغ
update_snapshot(غړی_T *غړی)
{
    VTermScreen	    *screen;
    VTermState	    *state;
    صحيح		    len;
    صحيح		    کرښې_skipped = 0;
    VTermPos	    pos;
    VTermScreenCell cell;
    cellattr_T	    fill_attr, new_fill_attr;
    cellattr_T	    *p;

    ch_log(غړی->tl_دنده == عدم ? عدم : غړی->tl_دنده->jv_channel,
				  "Adding terminal window snapshot to buffer");

    // First remove the lines that were appended before, they might be
    // outdated.
    cleanup_scrollback(غړی);

    screen = vterm_obtain_screen(غړی->tl_vterm);
    state = vterm_obtain_state(غړی->tl_vterm);
    fill_attr = new_fill_attr = غړی->tl_default_color;
    لكل (pos.لیکه = 0; pos.لیکه < غړی->tl_لیکې; ++pos.لیکه)
    {
	len = 0;
	لكل (pos.ستون = 0; pos.ستون < غړی->tl_ستون_شمېر; ++pos.ستون)
	    إذا (vterm_screen_get_cell(screen, pos, &cell) != 0
						       && cell.chars[0] != NUL)
	    {
		len = pos.ستون + 1;
		new_fill_attr = غړی->tl_default_color;
	    }
	    وإلا
		// Assume the last attr is the filler attr.
		cell2cellattr(&cell, &new_fill_attr);

	إذا (len == 0 && equal_celattr(&new_fill_attr, &fill_attr))
	    ++کرښې_skipped;
	وإلا
	{
	    طالما (کرښې_skipped > 0)
	    {
		// Line was skipped, add an empty line.
		--کرښې_skipped;
		إذا (جمع_کړه_تشه_scrollback(غړی, &fill_attr, 0) == OK)
		{
		    جمع_کړه_scrollback_کرښه_منزل_بفر(غړی, (char_u *)"", 0, 0);
		    ++غړی->tl_scrollback_snapshot;
		}
	    }

	    إذا (len == 0)
		p = عدم;
	    وإلا
		p = ALLOC_MULT(cellattr_T, len);
	    إذا ((p != عدم || len == 0)
				     && ga_grow(&غړی->tl_scrollback, 1) == OK)
	    {
		garray_T    ga;
		صحيح	    پلنوالی;
		ثابت VTerm_کرښه_Info *lineinfo;
		sb_کرښه_T   *کرښه = (sb_کرښه_T *)غړی->tl_scrollback.ga_معلومات
						  + غړی->tl_scrollback.ga_len;

		lineinfo = vterm_state_get_lineinfo(state, pos.لیکه);

		ga_init2(&ga, 1, 100);
		لكل (pos.ستون = 0; pos.ستون < len; pos.ستون += پلنوالی)
		{
		    إذا (vterm_screen_get_cell(screen, pos, &cell) == 0)
		    {
			پلنوالی = 1;
			CLEAR_اشاره_ګر(p + pos.ستون);
			إذا (ga_grow(&ga, 1) == OK)
			    ga.ga_len += utf_char2bytes(' ',
					     (char_u *)ga.ga_معلومات + ga.ga_len);
		    }
		    وإلا
		    {
			پلنوالی = cell.پلنوالی;

			cell2cellattr(&cell, &p[pos.ستون]);
			إذا (پلنوالی == 2)
			    // second cell of double-width character has the
			    // same attributes.
			    p[pos.ستون + 1] = p[pos.ستون];

			// Each character can be up to 6 bytes.
			إذا (ga_grow(&ga, VTERM_MAX_CHARS_فی_CELL * 6) == OK)
			{
			    صحيح	    i;
			    صحيح	    c;

			    لكل (i = 0; i < VTERM_MAX_CHARS_فی_CELL &&
				    ((c = cell.chars[i]) > 0 || i == 0); ++i)
				ga.ga_len += utf_char2bytes(c == NUL ? ' ' : c,
					     (char_u *)ga.ga_معلومات + ga.ga_len);
			}
		    }
		}
		کرښه->sb_ستون_شمېر = len;
		کرښه->sb_بایټونه = ga.ga_len;
		کرښه->sb_cells = p;
		کرښه->sb_fill_attr = new_fill_attr;
		کرښه->continuation = (char_u)lineinfo->continuation;
		fill_attr = new_fill_attr;
		++غړی->tl_scrollback.ga_len;
		++غړی->tl_scrollback_snapshot;

		إذا (ga_grow(&ga, 1) == FAIL)
		    جمع_کړه_scrollback_کرښه_منزل_بفر(غړی, (char_u *)"", 0, 0);
		وإلا
		{
		    *((char_u *)ga.ga_معلومات + ga.ga_len) = NUL;
		    جمع_کړه_scrollback_کرښه_منزل_بفر(غړی, ga.ga_معلومات, ga.ga_len,
						  lineinfo->continuation);
		}
		ga_clear(&ga);
	    }
	    وإلا
		vim_free(p);
	}
    }

    // Add trailing empty lines.
    لكل (pos.لیکه = غړی->tl_scrollback.ga_len;
	    pos.لیکه < غړی->tl_scrollback_scrolled + غړی->tl_cursor_pos.لیکه;
	    ++pos.لیکه)
    {
	إذا (جمع_کړه_تشه_scrollback(غړی, &fill_attr, 0) == OK)
	{
	    جمع_کړه_scrollback_کرښه_منزل_بفر(غړی, (char_u *)"", 0, 0);
	    ++غړی->tl_scrollback_snapshot;
	}
    }

    غړی->tl_dirty_snapshot = FALSE;
#که_تعریف_شوی FEAT_TIMERS
    غړی->tl_timer_سېټ = FALSE;
#د_که_پای
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
    وإلا إذا ((*wp)->w_بل != عدم)
	*wp = (*wp)->w_بل;
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
may_move_terminal_منزل_بفر(غړی_T *غړی, صحيح redraw)
{
    إذا (غړی->tl_vterm == عدم)
	أرجع;

    // Update the snapshot only if something changes or the buffer does not
    // have all the lines.
    إذا (غړی->tl_dirty_snapshot || غړی->tl_بفر->b_ml.ml_کرښه_شمېر
					       <= غړی->tl_بفر_scrolled)
	update_snapshot(غړی);

    إذا (redraw)
    {
	win_T	    *wp = عدم;
	صحيح	    did_curwin = FALSE;

	طالما (for_all_windows_and_curwin(&wp, &did_curwin))
	{
	    إذا (wp->w_بفر == غړی->tl_بفر)
	    {
		wp->w_cursor.lnum = غړی->tl_بفر->b_ml.ml_کرښه_شمېر;
		wp->w_cursor.ستون = 0;
		wp->w_valid = 0;
		إذا (wp->w_cursor.lnum >= wp->w_لوړوالی)
		{
		    linenr_T min_topline = wp->w_cursor.lnum - wp->w_لوړوالی + 1;

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
غړی_check_timers(صحيح بل_due_arg, proftime_T *now)
{
    غړی_T  *غړی;
    صحيح	    بل_due = بل_due_arg;

    FOR_ALL_TERMS(غړی)
    {
	إذا (غړی->tl_timer_سېټ && !غړی->tl_نورمال_ډېرپېښ)
	{
	    اوږد    this_due = proftime_وخت_کیڼ(&غړی->tl_timer_due, now);

	    إذا (this_due <= 1)
	    {
		غړی->tl_timer_سېټ = FALSE;
		may_move_terminal_منزل_بفر(غړی, FALSE);
	    }
	    وإلا إذا (بل_due == -1 || بل_due > this_due)
		بل_due = this_due;
	}
    }

    أرجع بل_due;
}
#د_که_پای

/*
 * When "normal_mode" is TRUE set the terminal to Terminal-Normal mode,
 * otherwise end it.
 */
    ساكن فراغ
سېټ_terminal_ډېرپېښ(غړی_T *غړی, صحيح نورمال_ډېرپېښ)
{
    غړی->tl_نورمال_ډېرپېښ = نورمال_ډېرپېښ;
    may_trigger_modechanged();
    إذا (!نورمال_ډېرپېښ)
	handle_postponed_scrollback(غړی);
    VIM_CLEAR(غړی->tl_حالت_متن);
    إذا (غړی->tl_بفر == curbuf)
	maketitle();
}

/*
 * Called after the job is finished and Terminal mode is not active:
 * Move the vterm contents into the scrollback buffer and free the vterm.
 */
    ساكن فراغ
cleanup_vterm(غړی_T *غړی)
{
    سېټ_terminal_ډېرپېښ(غړی, FALSE);
    إذا (غړی->tl_پای != TL_پای_CLOSE)
	may_move_terminal_منزل_بفر(غړی, TRUE);
    غړی_free_vterm(غړی);
}

/*
 * Switch from Terminal-Job mode to Terminal-Normal mode.
 * Suspends updating the terminal window.
 */
    ساكن فراغ
غړی_enter_نورمال_ډېرپېښ(فراغ)
{
    غړی_T     *غړی = curbuf->b_غړی;
    linenr_T   lnum;
    صحيح        ستون;

    سېټ_terminal_ډېرپېښ(غړی, TRUE);

    // Append the current terminal contents to the buffer.
    may_move_terminal_منزل_بفر(غړی, TRUE);

    // Move the window cursor to the position of the cursor in the
    // terminal.
    lnum = غړی->tl_بفر_scrolled + 1 + غړی->tl_cursor_pos.لیکه;
    ستون = غړی->tl_cursor_pos.ستون;
    scrollbackline_pos_دننه_buf(غړی, غړی->tl_cursor_pos.لیکه + غړی->tl_scrollback_scrolled, &lnum, &ستون, عدم);

    curwin->w_cursor.lnum = lnum;
    check_cursor();
    إذا (coladvance(ستون) == FAIL)
	coladvance(MAXCOL);
    curwin->w_سېټ_curswant = true;
    curwin->w_topline = غړی->tl_بفر_scrolled + 1;
}

/*
 * Returns TRUE if the current window contains a terminal and we are in
 * Terminal-Normal mode.
 */
    صحيح
غړی_دننه_نورمال_ډېرپېښ(buf_T *buf)
{
    غړی_T *غړی = buf->b_غړی;

    أرجع غړی != عدم && غړی->tl_نورمال_ډېرپېښ;
}

/*
 * Switch from Terminal-Normal mode to Terminal-Job mode.
 * Restores updating the terminal window.
 */
    فراغ
غړی_enter_دنده_ډېرپېښ(فراغ)
{
    غړی_T	*غړی = curbuf->b_غړی;

    سېټ_terminal_ډېرپېښ(غړی, FALSE);

    إذا (غړی->tl_channel_closed)
	cleanup_vterm(غړی);
    redraw_buf_and_حالت_later(curbuf, UPD_NOT_VALID);
#که_تعریف_شوی FEAT_PROP_POPUP
    إذا (WIN_IS_POPUP(curwin))
	redraw_later(UPD_NOT_VALID);
#د_که_پای
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
    NRKS_سېټ,	    // no_reduce_keys was incremented in term_vgetc() or
		    // check_no_reduce_keys(), must be decremented.
} reduce_کلی_state_T;

ساكن reduce_کلی_state_T  no_reduce_کلی_state = NRKS_NONE;

/*
 * Return TRUE if the term is using modifyOtherKeys level 2 or the kitty
 * keyboard protocol.
 */
    ساكن صحيح
vterm_using_کلی_protocol(فراغ)
{
    أرجع curbuf->b_غړی != عدم
	&& curbuf->b_غړی->tl_vterm != عدم
	&& (vterm_is_modify_other_کلیګانې(curbuf->b_غړی->tl_vterm)
		|| vterm_is_kitty_keyboard(curbuf->b_غړی->tl_vterm));
}

    فراغ
check_no_reduce_کلیګانې(فراغ)
{
    إذا (no_reduce_کلی_state != NRKS_CHECK
	    || no_reduce_کلیګانې >= 1
	    || curbuf->b_غړی == عدم
	    || curbuf->b_غړی->tl_vterm == عدم)
	أرجع;

    إذا (vterm_using_کلی_protocol())
    {
	// "modify_other_keys" or kitty keyboard protocol was enabled while
	// waiting.
	no_reduce_کلی_state = NRKS_سېټ;
	++no_reduce_کلیګانې;
    }
}

/*
 * Get a key from the user with terminal mode mappings.
 * Note: while waiting a terminal may be closed and freed if the channel is
 * closed and ++close was used.  This may even happen before we get here.
 */
    ساكن صحيح
غړی_vgetc(فراغ)
{
    صحيح c;
    صحيح save_State = State;

    State = ډېرپېښ_TERMINAL;
    got_int = FALSE;
#که_تعریف_شوی MSWIN
    ctrl_break_was_pressed = FALSE;
#د_که_پای

    إذا (vterm_using_کلی_protocol())
    {
	++no_reduce_کلیګانې;
	no_reduce_کلی_state = NRKS_سېټ;
    }
    وإلا
    {
	no_reduce_کلی_state = NRKS_CHECK;
    }

    c = vgetc();
    got_int = FALSE;
    State = save_State;

    إذا (no_reduce_کلی_state == NRKS_سېټ)
	--no_reduce_کلیګانې;
    no_reduce_کلی_state = NRKS_NONE;

    أرجع c;
}

ساكن صحيح	mouse_was_outside = FALSE;

/*
 * Send key "c" with modifiers "modmask" to terminal.
 * Return FAIL when the key needs to be handled in Normal mode.
 * Return OK when the key was dropped or sent to the terminal.
 */
    صحيح
send_کلیګانې_منزل_غړی(غړی_T *غړی, صحيح c, صحيح modmask, صحيح typed)
{
    حرف	پیغام[کلی_BUF_LEN];
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
		صحيح	لیکه = mouse_لیکه;
		صحيح	ستون = mouse_ستون;

#که_تعریف_شوی FEAT_PROP_POPUP
		إذا (popup_is_popup(curwin))
		{
		    لیکه -= popup_سر_extra(curwin);
		    ستون -= popup_کیڼ_extra(curwin);
		}
#د_که_پای
		إذا (لیکه < W_WINROW(curwin)
			|| لیکه >= (W_WINROW(curwin) + curwin->w_لوړوالی)
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
    len = غړی_convert_کلی(غړی, c, modmask, پیغام);
    إذا (len > 0)
	// TODO: if FAIL is returned, stop?
	channel_send(غړی->tl_دنده->jv_channel, get_tty_part(غړی),
						(char_u *)پیغام, (صحيح)len, عدم);

    أرجع OK;
}

/*
 * Handle CTRL-W "": send register contents to the job.
 */
    ساكن فراغ
غړی_paste_ثبتوونکی(صحيح prev_c ناکارول_شوی)
{
    صحيح		c;
    list_T	*l;
    listitem_T	*توکی;
    اوږد	reglen = 0;
    صحيح		type;

    إذا (جمع_کړه_منزل_showcmd(prev_c))
    إذا (جمع_کړه_منزل_showcmd('"'))
	بهر_flush();

    c = غړی_vgetc();
    clear_showcmd();

    إذا (!غړی_use_loop())
	// job finished while waiting for a character
	أرجع;

    // CTRL-W "= prompt for expression to evaluate.
    إذا (c == '=' && get_بیان_ثبتوونکی() != '=')
	أرجع;
    إذا (!غړی_use_loop())
	// job finished while waiting for a character
	أرجع;

    l = (list_T *)get_reg_contents(c, GREG_LIST);
    إذا (l == عدم)
	أرجع;

    type = get_reg_type(c, &reglen);
    FOR_ALL_LIST_توکي(l, توکی)
    {
	char_u *s = tv_get_string(&توکی->li_tv);
#که_تعریف_شوی MSWIN
	char_u *tmp = s;

	إذا (!enc_utf8 && enc_codepage > 0)
	{
	    WCHAR   *ret = عدم;
	    صحيح	اوږدتیا = 0;

	    Multi_Byte_منزل_Wide_Char_alloc(enc_codepage, 0, (حرف *)s,
		    (صحيح)STRLEN(s), &ret, &اوږدتیا);
	    إذا (ret != عدم)
	    {
		Wide_Char_منزل_Multi_Byte_alloc(CP_UTF8, 0,
			ret, اوږدتیا, (حرف **)&s, &اوږدتیا, 0, 0);
		vim_free(ret);
	    }
	}
#د_که_پای
	channel_send(curbuf->b_غړی->tl_دنده->jv_channel, PART_دننه,
		s, (صحيح)STRLEN(s), عدم);
#که_تعریف_شوی MSWIN
	إذا (tmp != s)
	    vim_free(s);
#د_که_پای

	إذا (توکی->li_بل != عدم || type == MLINE)
	    channel_send(curbuf->b_غړی->tl_دنده->jv_channel, PART_دننه,
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
    أرجع دننه_terminal_loop != عدم;
}

/*
 * Return the highlight group ID for the terminal and the window.
 */
    ساكن صحيح
غړی_get_highlight_id(غړی_T *غړی, win_T *wp)
{
    char_u *name;

    إذا (wp != عدم && wp->w_hlfwin_id != 0)
	name = syn_id2name(wp->w_hlfwin_id);
    وإلا إذا (غړی->tl_highlight_name != عدم)
	name = غړی->tl_highlight_name;
    وإلا
	name = (char_u*)"Terminal";

    أرجع syn_name2id(name);
}

#إذا defined(FEAT_GUI)
    cursorentry_T *
غړی_get_cursor_shape(guicolor_T *fg, guicolor_T *bg)
{
    غړی_T		 *غړی = دننه_terminal_loop;
    ساكن cursorentry_T ثبت;
    صحيح			 id;
    guicolor_T		 غړی_fg = INVALCOLOR;
    guicolor_T		 غړی_bg = INVALCOLOR;

    CLEAR_ډګر(ثبت);
    ثبت.shape = ثبت.mshape =
	غړی->tl_cursor_shape == VTERM_PROP_CURSORSHAPE_UNDERLINE ? SHAPE_HOR :
	غړی->tl_cursor_shape == VTERM_PROP_CURSORSHAPE_BAR_کیڼ ? SHAPE_VER :
	SHAPE_بلاک;
    ثبت.percentage = 20;
    إذا (غړی->tl_cursor_blink)
    {
	ثبت.blinkwait = 700;
	ثبت.blinkon = 400;
	ثبت.blinkoff = 250;
    }

    // The highlight group overrules the defaults.
    id = غړی_get_highlight_id(غړی, curwin);
    إذا (id != 0)
	syn_id2colors(id, &غړی_fg, &غړی_bg);
    إذا (غړی_bg != INVALCOLOR)
	*fg = غړی_bg;
    وإلا
	*fg = gui.شا_pixel;

    إذا (غړی->tl_cursor_color == عدم)
    {
	إذا (غړی_fg != INVALCOLOR)
	    *bg = غړی_fg;
	وإلا
	    *bg = gui.norm_pixel;
    }
    وإلا
	*bg = color_name2handle(غړی->tl_cursor_color);
    ثبت.name = "n";
    ثبت.کارول_شوی_for = SHAPE_CURSOR;

    أرجع &ثبت;
}
#د_که_پای

    ساكن فراغ
may_output_cursor_props(فراغ)
{
    إذا (!cursor_color_equal(وروستی_سېټ_cursor_color, desired_cursor_color)
	    || وروستی_سېټ_cursor_shape != desired_cursor_shape
	    || وروستی_سېټ_cursor_blink != desired_cursor_blink)
    {
	cursor_color_copy(&وروستی_سېټ_cursor_color, desired_cursor_color);
	وروستی_سېټ_cursor_shape = desired_cursor_shape;
	وروستی_سېټ_cursor_blink = desired_cursor_blink;
	غړی_cursor_color(cursor_color_get(desired_cursor_color));
	إذا (desired_cursor_shape == -1 || desired_cursor_blink == -1)
	    // this will restore the initial cursor style, if possible
	    ui_cursor_shape_forced(TRUE);
	وإلا
	    غړی_cursor_shape(desired_cursor_shape, desired_cursor_blink);
    }
}

/*
 * Set the cursor color and shape, if not last set to these.
 */
    ساكن فراغ
may_سېټ_cursor_props(غړی_T *غړی)
{
#که_تعریف_شوی FEAT_GUI
    // For the GUI the cursor properties are obtained with
    // term_get_cursor_shape().
    إذا (gui.دننه_use)
	أرجع;
#د_که_پای
    إذا (دننه_terminal_loop == غړی)
    {
	cursor_color_copy(&desired_cursor_color, غړی->tl_cursor_color);
	desired_cursor_shape = غړی->tl_cursor_shape;
	desired_cursor_blink = غړی->tl_cursor_blink;
	may_output_cursor_props();
    }
}

/*
 * Reset the desired cursor properties and restore them when needed.
 */
    ساكن فراغ
prepare_restore_cursor_props(فراغ)
{
#که_تعریف_شوی FEAT_GUI
    إذا (gui.دننه_use)
	أرجع;
#د_که_پای
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
غړی_use_loop_check(صحيح check_دنده_حالت)
{
    غړی_T *غړی = curbuf->b_غړی;

    أرجع غړی != عدم
	&& !غړی->tl_نورمال_ډېرپېښ
	&& غړی->tl_vterm != عدم
	&& غړی_دنده_running_check(غړی, check_دنده_حالت);
}

/*
 * Returns TRUE if the current window contains a terminal and we are sending
 * keys to the job.
 */
    صحيح
غړی_use_loop(فراغ)
{
    أرجع غړی_use_loop_check(FALSE);
}

/*
 * Called when entering a window with the mouse.  If this is a terminal window
 * we may want to change state.
 */
    فراغ
غړی_win_entered(فراغ)
{
    غړی_T *غړی = curbuf->b_غړی;

    إذا (غړی == عدم)
	أرجع;

    إذا (غړی_use_loop_check(TRUE))
    {
	reset_VIsual_and_resel();
	إذا (State & ډېرپېښ_ورننباسه)
	    stop_ورننباسه_ډېرپېښ = TRUE;
    }
    mouse_was_outside = FALSE;
    enter_mouse_ستون = mouse_ستون;
    enter_mouse_لیکه = mouse_لیکه;
}

    فراغ
غړی_focus_change(صحيح دننه_focus)
{
    غړی_T *غړی = curbuf->b_غړی;

    إذا (غړی == عدم || غړی->tl_vterm == عدم)
	أرجع;

    VTermState	*state = vterm_obtain_state(غړی->tl_vterm);

    إذا (دننه_focus)
	vterm_state_focus_دننه(state);
    وإلا
	vterm_state_focus_بهر(state);
    غړی_forward_output(غړی);
}

/*
 * vgetc() may not include CTRL in the key when modify_other_keys is set.
 * Return the Ctrl-key value in that case.
 */
    ساكن صحيح
raw_c_منزل_ctrl(صحيح c)
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
ctrl_منزل_raw_c(صحيح c)
{
    إذا (c < 0x20 && vterm_using_کلی_protocol())
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
#که_تعریف_شوی UNIX
    صحيح		tty_fd = curbuf->b_غړی->tl_دنده->jv_channel
				 ->ch_part[get_tty_part(curbuf->b_غړی)].ch_fd;
#د_که_پای
    صحيح		restore_cursor = FALSE;

    // Remember the terminal we are sending keys to.  However, the terminal
    // might be closed while waiting for a character, e.g. typing "exit" in a
    // shell and ++close was used.  Therefore use curbuf->b_term instead of a
    // stored reference.
    دننه_terminal_loop = curbuf->b_غړی;

    إذا (*curwin->w_p_twk != NUL)
    {
	termwinkey = string_منزل_کلی(curwin->w_p_twk, TRUE);
	إذا (termwinkey == Ctrl_W)
	    termwinkey = 0;
    }
    position_cursor(curwin, &curbuf->b_غړی->tl_cursor_pos);
    may_سېټ_cursor_props(curbuf->b_غړی);

    طالما (blocking || vpeekc_nomap() != NUL)
    {
#که_تعریف_شوی FEAT_GUI
	إذا (curbuf->b_غړی != عدم && !curbuf->b_غړی->tl_system)
#د_که_پای
	    // TODO: skip screen update when handling a sequence of keys.
	    // Repeat redrawing in case a message is received while redrawing.
	    طالما (must_redraw != 0)
		إذا (update_screen(0) == FAIL)
		    اكسر;
	إذا (!غړی_use_loop_check(TRUE) || دننه_terminal_loop != curbuf->b_غړی)
	    // job finished while redrawing
	    اكسر;

	update_cursor(curbuf->b_غړی, FALSE);
	restore_cursor = TRUE;

	raw_c = غړی_vgetc();
	إذا (!غړی_use_loop_check(TRUE) || دننه_terminal_loop != curbuf->b_غړی)
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

#که_تعریف_شوی UNIX
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
		غړی_backspace_char = info.backspace;
	}
#د_که_پای

#که_تعریف_شوی MSWIN
	// On Windows winpty handles CTRL-C, don't send a CTRL_C_EVENT.
	// Use CTRL-BREAK to kill the job.
	إذا (ctrl_break_was_pressed)
	    mch_signal_دنده(curbuf->b_غړی->tl_دنده, (char_u *)"kill");
#د_که_پای
	// Was either CTRL-W (termwinkey) or CTRL-\ pressed?
	// Not in a system terminal.
	إذا ((c == (termwinkey == 0 ? Ctrl_W : termwinkey) || c == Ctrl_BSL)
#که_تعریف_شوی FEAT_GUI
		&& !curbuf->b_غړی->tl_system
#د_که_پای
		)
	{
	    صحيح	    prev_c = c;
	    صحيح	    prev_raw_c = raw_c;
	    صحيح	    prev_mod_ماسک = mod_ماسک;

	    إذا (جمع_کړه_منزل_showcmd(c))
		بهر_flush();

	    raw_c = غړی_vgetc();
	    c = raw_c_منزل_ctrl(raw_c);

	    clear_showcmd();

	    إذا (!غړی_use_loop_check(TRUE)
					 || دننه_terminal_loop != curbuf->b_غړی)
		// job finished while waiting for a character
		اكسر;

	    إذا (prev_c == Ctrl_BSL)
	    {
		إذا (c == Ctrl_N)
		{
		    // CTRL-\ CTRL-N : go to Terminal-Normal mode.
		    غړی_enter_نورمال_ډېرپېښ();
		    ret = FAIL;
		    اذهب theend;
		}
		// Send both keys to the terminal, first one here, second one
		// below.
		send_کلیګانې_منزل_غړی(curbuf->b_غړی, prev_raw_c, prev_mod_ماسک,
									 TRUE);
	    }
	    وإلا إذا (c == Ctrl_C)
	    {
		// "CTRL-W CTRL-C" or 'termwinkey' CTRL-C: end the job
		mch_signal_دنده(curbuf->b_غړی->tl_دنده, (char_u *)"kill");
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
		غړی_enter_نورمال_ډېرپېښ();
		ret = FAIL;
		اذهب theend;
	    }
	    وإلا إذا (c == '"')
	    {
		غړی_paste_ثبتوونکی(prev_c);
		استمر;
	    }
	    وإلا إذا (termwinkey == 0 || c != termwinkey)
	    {
		// space for CTRL-W, modifier, multi-byte char and NUL
		char_u buf[1 + 3 + MB_MAXBYTES + 1];

		// Put the command into the typeahead buffer, when using the
		// stuff buffer KeyStuffed is set and 'langmap' won't be used.
		buf[0] = Ctrl_W;
		buf[special_منزل_buf(c, mod_ماسک, FALSE, buf + 1) + 1] = NUL;
		ins_typebuf(buf, REMAP_NONE, 0, TRUE, FALSE);
		ret = OK;
		اذهب theend;
	    }
	}
#که_تعریف_شوی MSWIN
	إذا (!enc_utf8 && has_mbyte && raw_c >= 0x80)
	{
	    WCHAR   wc;
	    char_u  mb[3];

	    mb[0] = (بینښه)raw_c >> 8;
	    mb[1] = raw_c;
	    إذا (Multi_Byte_منزل_Wide_Char(GetACP(), 0, (حرف*)mb, 2, &wc, 1) > 0)
		raw_c = wc;
	}
#د_که_پای
	إذا (send_کلیګانې_منزل_غړی(curbuf->b_غړی, raw_c, mod_ماسک, TRUE) != OK)
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
    دننه_terminal_loop = عدم;
    إذا (restore_cursor)
	prepare_restore_cursor_props();

    // Move a snapshot of the screen contents to the buffer, so that completion
    // works in other buffers.
    إذا (curbuf->b_غړی != عدم && !curbuf->b_غړی->tl_نورمال_ډېرپېښ)
	may_move_terminal_منزل_بفر(curbuf->b_غړی, FALSE);

    أرجع ret;
}

    ساكن فراغ
may_toggle_cursor(غړی_T *غړی)
{
    إذا (دننه_terminal_loop != غړی)
	أرجع;

    إذا (غړی->tl_cursor_visible)
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

    إذا (VTERM_COLOR_IS_شاخصي(color))
    {
	// Use the color as-is if possible, give up otherwise.
	إذا (color->شاخص < t_colors)
	{
	    uint8_t شاخص = color->شاخص;
#که_تعریف_شوی MSWIN
	    // Convert ANSI palette to cterm color index.
	    // cterm_ansi_idx is a table for the reverse conversion from cterm
	    // color index to ANSI palette. However, the current table can
	    // be used for this conversion as well.
	    // Note: If the contents of cterm_ansi_idx change in the future, a
	    // different table may be needed for this purpose.
	    إذا (شاخص < 16)
		شاخص = cterm_ansi_idx[شاخص];
#د_که_پای
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
    إذا (cellattrs->سرچپه)
	attr |= HL_INVERSE;
    أرجع attr;
}

/*
 * Store Vterm attributes in "cell" from highlight flags.
 */
    ساكن فراغ
hl2vtermAttr(صحيح attr, cellattr_T *cell)
{
    CLEAR_ډګر(cell->attrs);
    إذا (attr & HL_BOLD)
	cell->attrs.bold = 1;
    إذا (attr & HL_UNDERLINE)
	cell->attrs.underline = 1;
    إذا (attr & HL_ITALIC)
	cell->attrs.italic = 1;
    إذا (attr & HL_STRIKETHROUGH)
	cell->attrs.strike = 1;
    إذا (attr & HL_INVERSE)
	cell->attrs.سرچپه = 1;
}

/*
 * Convert the attributes of a vterm cell into an attribute index.
 */
    ساكن صحيح
cell2attr(
	غړی_T			*غړی,
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
		fg = &wp->w_غړی_hlfwin.fg;
	    إذا (is_default_bg)
		bg = &wp->w_غړی_hlfwin.bg;
	}
	وإلا
	{
	    إذا (is_default_fg)
		fg = &غړی->tl_default_color.fg;
	    إذا (is_default_bg)
		bg = &غړی->tl_default_color.bg;
	}
    }

#که_تعریف_شوی FEAT_GUI
    إذا (gui.دننه_use)
    {
	guicolor_T guifg = gui_mch_get_rgb_color(fg->red, fg->green, fg->blue);
	guicolor_T guibg = gui_mch_get_rgb_color(bg->red, bg->green, bg->blue);
	أرجع get_gui_attr_idx(attr, guifg, guibg);
    }
    وإلا
#د_که_پای
#که_تعریف_شوی FEAT_TERMGUICOLORS
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
#د_که_پای
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
سېټ_dirty_snapshot(غړی_T *غړی)
{
    غړی->tl_dirty_snapshot = TRUE;
#که_تعریف_شوی FEAT_TIMERS
    إذا (!غړی->tl_نورمال_ډېرپېښ)
    {
	// Update the snapshot after 100 msec of not getting updates.
	profile_setlimit(100L, &غړی->tl_timer_due);
	غړی->tl_timer_سېټ = TRUE;
    }
#د_که_پای
}

    ساكن صحيح
handle_damage(VTermRect rect, فراغ *user)
{
    غړی_T *غړی = (غړی_T *)user;

    غړی->tl_dirty_لیکه_پیل = MIN(غړی->tl_dirty_لیکه_پیل, rect.پیل_لیکه);
    غړی->tl_dirty_لیکه_پای_ټکی = MAX(غړی->tl_dirty_لیکه_پای_ټکی, rect.پای_ټکی_لیکه);
    سېټ_dirty_snapshot(غړی);
    redraw_buf_later(غړی->tl_بفر, UPD_SOME_VALID);
    أرجع 1;
}

    ساكن فراغ
غړی_scroll_پورته(غړی_T *غړی, صحيح پیل_لیکه, صحيح شمېر)
{
    win_T		 *wp = عدم;
    صحيح			 did_curwin = FALSE;
    VTermColor		 fg, bg;
    VTermScreenCellAttrs attr;
    صحيح			 clear_attr;

    CLEAR_ډګر(attr);

    طالما (for_all_windows_and_curwin(&wp, &did_curwin))
    {
	إذا (wp->w_بفر == غړی->tl_بفر)
	{
	    // Set the color to clear lines with.
	    vterm_state_get_default_colors(vterm_obtain_state(غړی->tl_vterm),
								     &fg, &bg);
	    clear_attr = cell2attr(غړی, wp, &attr, &fg, &bg);
	    win_del_کرښې(wp, پیل_لیکه, شمېر, FALSE, FALSE, clear_attr);
	}
    }
}

    ساكن صحيح
handle_moverect(VTermRect dest, VTermRect src, فراغ *user)
{
    غړی_T	*غړی = (غړی_T *)user;
    صحيح		شمېر = src.پیل_لیکه - dest.پیل_لیکه;

    // Scrolling up is done much more efficiently by deleting lines instead of
    // redrawing the text. But avoid doing this multiple times, postpone until
    // the redraw happens.
    إذا (dest.پیل_ستون == src.پیل_ستون
	    && dest.پای_ټکی_ستون == src.پای_ټکی_ستون
	    && dest.پیل_لیکه < src.پیل_لیکه)
    {
	إذا (dest.پیل_لیکه == 0)
	    غړی->tl_postponed_scroll += شمېر;
	وإلا
	    غړی_scroll_پورته(غړی, dest.پیل_لیکه, شمېر);
    }

    غړی->tl_dirty_لیکه_پیل = MIN(غړی->tl_dirty_لیکه_پیل, dest.پیل_لیکه);
    غړی->tl_dirty_لیکه_پای_ټکی = MIN(غړی->tl_dirty_لیکه_پای_ټکی, dest.پای_ټکی_لیکه);
    سېټ_dirty_snapshot(غړی);

    // Note sure if the scrolling will work correctly, let's do a complete
    // redraw later.
    redraw_buf_later(غړی->tl_بفر, UPD_NOT_VALID);
    أرجع 1;
}

    ساكن صحيح
handle_movecursor(
	VTermPos pos,
	VTermPos oldpos ناکارول_شوی,
	صحيح visible,
	فراغ *user)
{
    غړی_T	*غړی = (غړی_T *)user;
    win_T	*wp = عدم;
    صحيح		did_curwin = FALSE;

    غړی->tl_cursor_pos = pos;
    غړی->tl_cursor_visible = visible;

    طالما (for_all_windows_and_curwin(&wp, &did_curwin))
    {
	إذا (wp->w_بفر == غړی->tl_بفر)
	    position_cursor(wp, &pos);
    }
    إذا (غړی->tl_بفر == curbuf && !غړی->tl_نورمال_ډېرپېښ)
	// Don't redraw here, it will be done after
	// vterm_input_write() is finished.
	update_cursor(غړی, FALSE);

    أرجع 1;
}

    ساكن صحيح
handle_settermprop(
	VTermProp prop,
	VTerm_ارزښت *ارزښت,
	فراغ *user)
{
    غړی_T	*غړی = (غړی_T *)user;
    char_u	*strval = عدم;

    بدل (prop)
    {
	حالة VTERM_PROP_TITLE:
	    إذا (disable_vterm_title_for_testing)
		اكسر;
	    strval = vim_strnsave((char_u *)ارزښت->string.str,
							    ارزښت->string.len);
	    إذا (strval == عدم)
		اكسر;
	    vim_free(غړی->tl_title);
	    // a blank title isn't useful, make it empty, so that "running" is
	    // displayed
	    إذا (*skipwhite(strval) == NUL)
		غړی->tl_title = عدم;
	    // Same as blank
	    وإلا إذا (غړی->tl_arg0_cmd != عدم
		    && STRNCMP(غړی->tl_arg0_cmd, strval,
					  (صحيح)STRLEN(غړی->tl_arg0_cmd)) == 0)
		غړی->tl_title = عدم;
	    // Empty corrupted data of winpty
	    وإلا إذا (STRNCMP("  - ", strval, 4) == 0)
		غړی->tl_title = عدم;
#که_تعریف_شوی MSWIN
	    وإلا إذا (!enc_utf8 && enc_codepage > 0)
	    {
		WCHAR   *ret = عدم;
		صحيح	اوږدتیا = 0;

		Multi_Byte_منزل_Wide_Char_alloc(CP_UTF8, 0,
			(حرف*)ارزښت->string.str,
					(صحيح)ارزښت->string.len, &ret, &اوږدتیا);
		إذا (ret != عدم)
		{
		    Wide_Char_منزل_Multi_Byte_alloc(enc_codepage, 0,
					ret, اوږدتیا, (حرف**)&غړی->tl_title,
					&اوږدتیا, 0, 0);
		    vim_free(ret);
		}
	    }
#د_که_پای
	    وإلا
	    {
		غړی->tl_title = strval;
		strval = عدم;
	    }
	    VIM_CLEAR(غړی->tl_حالت_متن);
	    إذا (غړی == curbuf->b_غړی)
	    {
		maketitle();
		curwin->w_redr_حالت = true;
	    }
	    اكسر;

	حالة VTERM_PROP_CURSORVISIBLE:
	    غړی->tl_cursor_visible = ارزښت->boolean;
	    may_toggle_cursor(غړی);
	    بهر_flush();
	    اكسر;

	حالة VTERM_PROP_CURSORBLINK:
	    غړی->tl_cursor_blink = ارزښت->boolean;
	    may_سېټ_cursor_props(غړی);
	    اكسر;

	حالة VTERM_PROP_CURSORSHAPE:
	    غړی->tl_cursor_shape = ارزښت->شمېره;
	    may_سېټ_cursor_props(غړی);
	    اكسر;

	حالة VTERM_PROP_CURSORCOLOR:
	    strval = vim_strnsave((char_u *)ارزښت->string.str,
							    ارزښت->string.len);
	    إذا (strval == عدم)
		اكسر;
	    cursor_color_copy(&غړی->tl_cursor_color, strval);
	    may_سېټ_cursor_props(غړی);
	    اكسر;

	حالة VTERM_PROP_ALTSCREEN:
	    // TODO: do anything else?
	    غړی->tl_using_altscreen = ارزښت->boolean;
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
handle_resize(صحيح لیکې_2, صحيح ستون_شمېر, فراغ *user)
{
    غړی_T	*غړی = (غړی_T *)user;
    win_T	*wp;

    غړی->tl_لیکې = لیکې_2;
    غړی->tl_ستون_شمېر = ستون_شمېر;
    إذا (غړی->tl_vterm_اندازه_بدل_شوی)
	// Size was set by vterm_set_size(), don't set the window size.
	غړی->tl_vterm_اندازه_بدل_شوی = FALSE;
    وإلا
    {
	FOR_ALL_WINDOWS(wp)
	{
	    إذا (wp->w_بفر == غړی->tl_بفر)
	    {
		win_setheight_win(لیکې_2, wp);
		win_setwidth_win(ستون_شمېر, wp);
	    }
	}
	redraw_buf_later(غړی->tl_بفر, UPD_NOT_VALID);
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
حد_scrollback(غړی_T *غړی, garray_T *gap, صحيح update_بفر)
{
    إذا (gap->ga_len < غړی->tl_بفر->b_p_twsl)
	أرجع;

    صحيح	todo = MAX(غړی->tl_بفر->b_p_twsl / 10,
				     gap->ga_len - غړی->tl_بفر->b_p_twsl);
    صحيح	i;
    sb_کرښه_T *sb_کرښې = (sb_کرښه_T *)gap->ga_معلومات;

    curbuf = غړی->tl_بفر;
    لكل (i = 0; i < todo; ++i)
    {
	إذا (update_بفر && (!sb_کرښې[i].continuation || !i))
	{
	    ml_delete(1);
	    --غړی->tl_بفر_scrolled;
	}
	vim_free(sb_کرښې[i].sb_cells);
    }
    // Continue until end of wrapped line
    لكل (; todo < gap->ga_len && sb_کرښې[todo].continuation; ++todo)
	vim_free(sb_کرښې[todo].sb_cells);
    curbuf = curwin->w_بفر;

    gap->ga_len -= todo;
    mch_memmove(gap->ga_معلومات,
		(sb_کرښه_T *)gap->ga_معلومات + todo,
		حجم(sb_کرښه_T) * gap->ga_len);
    إذا (update_بفر)
    {
	win_T *curwin_save = curwin;
	win_T *wp = عدم;

	غړی->tl_scrollback_scrolled -= todo;

	FOR_ALL_WINDOWS(wp)
	{
	    إذا (wp->w_بفر == غړی->tl_بفر)
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
handle_pushline(صحيح ستون_شمېر, ثابت VTermScreenCell *cells, صحيح continuation, فراغ *user)
{
    غړی_T	*غړی = (غړی_T *)user;
    garray_T	*gap;
    صحيح		update_بفر;

    إذا (غړی->tl_نورمال_ډېرپېښ)
    {
	// In Terminal-Normal mode the user interacts with the buffer, thus we
	// must not change it. Postpone adding the scrollback lines.
	gap = &غړی->tl_scrollback_postponed;
	update_بفر = FALSE;
    }
    وإلا
    {
	// First remove the lines that were appended before, the pushed line
	// goes above it.
	cleanup_scrollback(غړی);
	gap = &غړی->tl_scrollback;
	update_بفر = TRUE;
    }

    حد_scrollback(غړی, gap, update_بفر);

    إذا (ga_grow(gap, 1) == FAIL)
	أرجع 0;

    cellattr_T	*p = عدم;
    صحيح		len = 0;
    صحيح		i;
    صحيح		c;
    صحيح		ستون;
    صحيح		متن_len;
    char_u		*متن;
    sb_کرښه_T	*کرښه;
    garray_T	ga;
    cellattr_T	fill_attr = غړی->tl_default_color;

    // do not store empty cells at the end
    لكل (i = 0; i < ستون_شمېر; ++i)
	إذا (cells[i].chars[0] != 0)
	    len = i + 1;
	وإلا
	    cell2cellattr(&cells[i], &fill_attr);

    ga_init2(&ga, 1, 100);
    إذا (len > 0)
	p = ALLOC_MULT(cellattr_T, len);
    إذا (p != عدم)
    {
	لكل (ستون = 0; ستون < len; ستون += cells[ستون].پلنوالی)
	{
	    إذا (ga_grow(&ga, VTERM_MAX_CHARS_فی_CELL * 4) == FAIL)
	    {
		ga.ga_len = 0;
		اكسر;
	    }
	    لكل (i = 0; i < VTERM_MAX_CHARS_فی_CELL &&
		    ((c = cells[ستون].chars[i]) > 0 || i == 0); ++i)
		ga.ga_len += utf_char2bytes(c == NUL ? ' ' : c,
			(char_u *)ga.ga_معلومات + ga.ga_len);
	    cell2cellattr(&cells[ستون], &p[ستون]);
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
	جمع_کړه_scrollback_کرښه_منزل_بفر(غړی, متن, متن_len, continuation);

    کرښه = (sb_کرښه_T *)gap->ga_معلومات + gap->ga_len;
    کرښه->sb_ستون_شمېر = len;
    کرښه->sb_بایټونه = متن_len;
    کرښه->sb_cells = p;
    کرښه->sb_fill_attr = fill_attr;
    کرښه->continuation = (char_u)continuation;
    إذا (update_بفر)
    {
	کرښه->sb_متن = عدم;
	++غړی->tl_scrollback_scrolled;
	إذا (!continuation)
	    ++غړی->tl_بفر_scrolled;
	ga_clear(&ga);  // free the text
    }
    وإلا
    {
	کرښه->sb_متن = متن;
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
handle_postponed_scrollback(غړی_T *غړی)
{
    صحيح i;

    إذا (غړی->tl_scrollback_postponed.ga_len == 0)
	أرجع;
    ch_log(عدم, "Moving postponed scrollback to scrollback");

    // First remove the lines that were appended before, the pushed lines go
    // above it.
    cleanup_scrollback(غړی);

    لكل (i = 0; i < غړی->tl_scrollback_postponed.ga_len; ++i)
    {
	char_u		*متن;
	sb_کرښه_T	*pp_کرښه;
	sb_کرښه_T	*کرښه;

	إذا (ga_grow(&غړی->tl_scrollback, 1) == FAIL)
	    اكسر;
	pp_کرښه = (sb_کرښه_T *)غړی->tl_scrollback_postponed.ga_معلومات + i;

	متن = pp_کرښه->sb_متن;
	إذا (متن == عدم)
	    متن = (char_u *)"";
	جمع_کړه_scrollback_کرښه_منزل_بفر(غړی, متن, (صحيح)STRLEN(متن), pp_کرښه->continuation);
	vim_free(pp_کرښه->sb_متن);

	کرښه = (sb_کرښه_T *)غړی->tl_scrollback.ga_معلومات
						 + غړی->tl_scrollback.ga_len;
	کرښه->sb_ستون_شمېر = pp_کرښه->sb_ستون_شمېر;
	کرښه->sb_بایټونه = pp_کرښه->sb_بایټونه;
	کرښه->sb_cells = pp_کرښه->sb_cells;
	کرښه->sb_fill_attr = pp_کرښه->sb_fill_attr;
	کرښه->sb_متن = عدم;
	++غړی->tl_scrollback_scrolled;
	++غړی->tl_scrollback.ga_len;
	إذا (!pp_کرښه->continuation)
	    ++غړی->tl_بفر_scrolled;
    }

    ga_clear(&غړی->tl_scrollback_postponed);
    حد_scrollback(غړی, &غړی->tl_scrollback, TRUE);
}

/*
 * Called when the terminal wants to ring the system bell.
 */
    ساكن صحيح
handle_bell(فراغ *user ناکارول_شوی)
{
    vim_beep(BO_غړی);
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
غړی_after_channel_closed(غړی_T *غړی)
{
    // Unless in Terminal-Normal mode: clear the vterm.
    إذا (!غړی->tl_نورمال_ډېرپېښ)
    {
	صحيح	fnum = غړی->tl_بفر->b_fnum;

	cleanup_vterm(غړی);

	إذا (غړی->tl_پای == TL_پای_CLOSE)
	{
	    aco_save_T	aco;
	    صحيح		do_سېټ_w_locked = غړی->tl_بفر->b_nwindows == 0;
#که_تعریف_شوی FEAT_PROP_POPUP
	    win_T	*pwin = عدم;

	    // If this was a terminal in a popup window, go back to the
	    // previous window.
	    إذا (popup_is_popup(curwin) && curbuf == غړی->tl_بفر)
	    {
		pwin = curwin;
		إذا (win_valid(prevwin))
		    win_enter(prevwin, FALSE);
	    }
	    وإلا
#د_که_پای
	    // If this is the last normal window: exit Vim.
	    إذا (غړی->tl_بفر->b_nwindows > 0 && only_one_کړکۍ())
	    {
		exarg_T ea;

		CLEAR_ډګر(ea);
		ex_quit(&ea);
		أرجع TRUE;
	    }

	    // ++close or term_finish == "close"
	    ch_log(عدم, "terminal job finished, closing window");
	    aucmd_prepbuf(&aco, غړی->tl_بفر);
	    إذا (curbuf == غړی->tl_بفر)
	    {
		win_T	*wp = curwin;

		// Avoid closing the window if we temporarily use it.
		إذا (is_aucmd_win(wp))
		    do_سېټ_w_locked = TRUE;
		إذا (do_سېټ_w_locked)
		    ++wp->w_locked;
		do_bufdel(DOBUF_WIPE, (char_u *)"", 1, fnum, fnum, FALSE);
		إذا (do_سېټ_w_locked)
		    --wp->w_locked;
		aucmd_restbuf(&aco);
	    }
#که_تعریف_شوی FEAT_PROP_POPUP
	    إذا (pwin != عدم)
		popup_close_with_retval(pwin, 0);
#د_که_پای
	    أرجع TRUE;
	}
	إذا (غړی->tl_پای == TL_پای_OPEN
				   && غړی->tl_بفر->b_nwindows == 0)
	{
	    حرف    *cmd = غړی->tl_opencmd == عدم
				? "botright sbuf %d"
				: (حرف *)غړی->tl_opencmd;
	    size_t  len = مزي_اوږدوالی(cmd) + 50;
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

    redraw_buf_and_حالت_later(غړی->tl_بفر, UPD_NOT_VALID);
    أرجع FALSE;
}

#إذا defined(FEAT_PROP_POPUP)
/*
 * If the current window is a terminal in a popup window and the job has
 * finished, close the popup window and to back to the previous window.
 * Otherwise return FAIL.
 */
    صحيح
may_close_غړی_popup(فراغ)
{
    إذا (!popup_is_popup(curwin) || curbuf->b_غړی == عدم
				 || غړی_دنده_running_not_none(curbuf->b_غړی))
	أرجع FAIL;

    win_T *pwin = curwin;

    إذا (win_valid(prevwin))
	win_enter(prevwin, FALSE);
    popup_close_with_retval(pwin, 0);
    أرجع OK;
}
#د_که_پای

/*
 * Called when a channel is going to be closed, before invoking the close
 * callback.
 */
    فراغ
غړی_channel_closing(channel_T *ch)
{
    غړی_T *غړی;

    لكل (غړی = لومړی_غړی; غړی != عدم; غړی = غړی->tl_بل)
	إذا (غړی->tl_دنده == ch->ch_دنده && !غړی->tl_channel_closed)
	    غړی->tl_channel_closing = TRUE;
}

/*
 * Called when a channel has been closed.
 * If this was a channel for a terminal window then finish it up.
 */
    فراغ
غړی_channel_closed(channel_T *ch)
{
    غړی_T *غړی;
    غړی_T *بل_غړی;
    صحيح	    did_one = FALSE;

    لكل (غړی = لومړی_غړی; غړی != عدم; غړی = بل_غړی)
    {
	بل_غړی = غړی->tl_بل;
	إذا (غړی->tl_دنده == ch->ch_دنده && !غړی->tl_channel_closed)
	{
	    غړی->tl_channel_closed = TRUE;
	    did_one = TRUE;

	    VIM_CLEAR(غړی->tl_title);
	    VIM_CLEAR(غړی->tl_حالت_متن);
#که_تعریف_شوی MSWIN
	    إذا (غړی->tl_بهر_fd != عدم)
	    {
		fclose(غړی->tl_بهر_fd);
		غړی->tl_بهر_fd = عدم;
	    }
#د_که_پای

	    إذا (updating_screen)
	    {
		// Cannot open or close windows now.  Can happen when
		// 'lazyredraw' is set.
		غړی->tl_channel_recently_closed = TRUE;
		استمر;
	    }

	    إذا (غړی_after_channel_closed(غړی))
		بل_غړی = لومړی_غړی;
	}
    }

    إذا (did_one)
    {
	redraw_statuslines();

	// Need to break out of vgetc().
	ins_char_typebuf(K_IGNORE, 0);
	typebuf_was_filled = TRUE;

	غړی = curbuf->b_غړی;
	إذا (غړی != عدم)
	{
	    إذا (غړی->tl_دنده == ch->ch_دنده)
		maketitle();
	    update_cursor(غړی, غړی->tl_cursor_visible);
	}
    }
}

/*
 * To be called after resetting updating_screen: handle any terminal where the
 * channel was closed.
 */
    فراغ
غړی_check_channel_closed_recently(فراغ)
{
    غړی_T *غړی;
    غړی_T *بل_غړی;

    لكل (غړی = لومړی_غړی; غړی != عدم; غړی = بل_غړی)
    {
	بل_غړی = غړی->tl_بل;
	إذا (غړی->tl_channel_recently_closed)
	{
	    غړی->tl_channel_recently_closed = FALSE;
	    إذا (غړی_after_channel_closed(غړی))
		// start over, the list may have changed
		بل_غړی = لومړی_غړی;
	}
    }
}

/*
 * Fill one screen line from a line of the terminal.
 * Advances "pos" to past the last column.
 */
    ساكن فراغ
غړی_line2screenline(
	غړی_T		*غړی,
	win_T		*wp,
	VTermScreen	*screen,
	VTermPos	*pos,
	صحيح		max_ستون)
{
    صحيح off = screen_get_current_کرښه_off();

    لكل (pos->ستون = 0; pos->ستون < max_ستون; )
    {
	VTermScreenCell cell;
	صحيح		c;

	إذا (vterm_screen_get_cell(screen, *pos, &cell) == 0)
	    CLEAR_ډګر(cell);

	c = cell.chars[0];
	إذا (c == NUL)
	{
	    Screen_کرښې[off] = ' ';
	    إذا (enc_utf8)
		Screen_کرښې_UC[off] = NUL;
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
		    Screen_کرښې_C[i][off] = cell.chars[i + 1];
		    إذا (cell.chars[i + 1] == 0)
			اكسر;
		}
		إذا (c >= 0x80 || (Screen_mco > 0
					 && Screen_کرښې_C[0][off] != 0))
		{
		    Screen_کرښې[off] = ' ';
		    Screen_کرښې_UC[off] = c;
		}
		وإلا
		{
		    Screen_کرښې[off] = c;
		    Screen_کرښې_UC[off] = NUL;
		}
	    }
#که_تعریف_شوی MSWIN
	    وإلا إذا (has_mbyte && c >= 0x80)
	    {
		char_u	mb[MB_MAXBYTES+1];
		WCHAR	wc = c;

		إذا (Wide_Char_منزل_Multi_Byte(GetACP(), 0, &wc, 1,
					       (حرف*)mb, 2, 0, 0) > 1)
		{
		    Screen_کرښې[off] = mb[0];
		    Screen_کرښې[off + 1] = mb[1];
		    cell.پلنوالی = mb_ptr2cells(mb);
		}
		وإلا
		    Screen_کرښې[off] = c;
	    }
#د_که_پای
	    وإلا
		// This will only store the lower byte of "c".
		Screen_کرښې[off] = c;
	}
	ScreenAttrs[off] = cell2attr(غړی, wp, &cell.attrs, &cell.fg,
								     &cell.bg);

	++pos->ستون;
	++off;
	إذا (cell.پلنوالی == 2)
	{
	    // don't set the second byte to NUL for a DBCS encoding, it
	    // has been set above
	    إذا (enc_utf8)
	    {
		Screen_کرښې_UC[off] = NUL;
		Screen_کرښې[off] = NUL;
	    }
	    وإلا إذا (!has_mbyte)
	    {
		// Can't show a double-width character with a single-byte
		// 'encoding', just use a space.
		Screen_کرښې[off] = ' ';
		ScreenAttrs[off] = ScreenAttrs[off - 1];
	    }

	    ++pos->ستون;
	    ++off;
	}
    }
}

#إذا defined(FEAT_GUI)
    ساكن فراغ
update_system_غړی(غړی_T *غړی)
{
    VTermPos	    pos;
    VTermScreen	    *screen;

    إذا (غړی->tl_vterm == عدم)
	أرجع;
    screen = vterm_obtain_screen(غړی->tl_vterm);

    // Scroll up to make more room for terminal lines if needed.
    // Use the cursor position to determine how much to scroll, because
    // ConPTY may damage all rows on initialization even when most are
    // empty, which would cause unnecessary scrolling.
    طالما (غړی->tl_toprow > 0
		  && (لیکې - غړی->tl_toprow) < غړی->tl_cursor_pos.لیکه + 1)
    {
	صحيح save_p_نور = p_نور;

	p_نور = FALSE;
	پیغام_لیکه = لیکې - 1;
	پیغام_puts("\n");
	p_نور = save_p_نور;
	--غړی->tl_toprow;
    }

    لكل (pos.لیکه = غړی->tl_dirty_لیکه_پیل; pos.لیکه < غړی->tl_dirty_لیکه_پای_ټکی
						  && pos.لیکه < لیکې; ++pos.لیکه)
    {
	إذا (pos.لیکه < غړی->tl_لیکې)
	{
	    صحيح max_ستون = MIN(topframe->fr_پلنوالی, غړی->tl_ستون_شمېر);

	    غړی_line2screenline(غړی, عدم, screen, &pos, max_ستون);
	}
	وإلا
	    pos.ستون = 0;

	screen_کرښه(curwin, غړی->tl_toprow + pos.لیکه,
		firstwin->w_wincol, pos.ستون, topframe->fr_پلنوالی, -1, 0);
    }

    غړی->tl_dirty_لیکه_پیل = MAX_لیکه;
    غړی->tl_dirty_لیکه_پای_ټکی = 0;
}
#د_که_پای

/*
 * Return TRUE if window "wp" is to be redrawn with term_update_window().
 * Returns FALSE when there is no terminal running in this window or it is in
 * Terminal-Normal mode.
 */
    صحيح
غړی_do_update_کړکۍ(win_T *wp)
{
    غړی_T	*غړی = wp->w_بفر->b_غړی;

    أرجع غړی != عدم && غړی->tl_vterm != عدم && !غړی->tl_نورمال_ډېرپېښ;
}

/*
 * Called to update a window that contains an active terminal.
 */
    فراغ
غړی_update_کړکۍ(win_T *wp)
{
    غړی_T	*غړی = wp->w_بفر->b_غړی;
    VTerm	*vterm;
    VTermScreen *screen;
    VTermState	*state;
    VTermPos	pos;
    صحيح		لیکې_2, ستون_شمېر;
    صحيح		newrows, newcols;
    صحيح		minsize;
    win_T	*twp;

    vterm = غړی->tl_vterm;
    screen = vterm_obtain_screen(vterm);
    state = vterm_obtain_state(vterm);

    // We use UPD_NOT_VALID on a resize or scroll, redraw everything then.
    // With UPD_SOME_VALID only redraw what was marked dirty.
    إذا (wp->w_redr_type > UPD_SOME_VALID)
    {
	غړی->tl_dirty_لیکه_پیل = 0;
	غړی->tl_dirty_لیکه_پای_ټکی = MAX_لیکه;

	إذا (غړی->tl_postponed_scroll > 0
			      && غړی->tl_postponed_scroll < غړی->tl_لیکې / 3)
	    // Scrolling is usually faster than redrawing, when there are only
	    // a few lines to scroll.
	    غړی_scroll_پورته(غړی, 0, غړی->tl_postponed_scroll);
	غړی->tl_postponed_scroll = 0;
    }

    /*
     * If the window was resized a redraw will be triggered and we get here.
     * Adjust the size of the vterm unless 'termwinsize' specifies a fixed size.
     */
    minsize = parse_termwinsize(wp, &لیکې_2, &ستون_شمېر);

    newrows = 99999;
    newcols = 99999;
    لكل (twp = firstwin; ; twp = twp->w_بل)
    {
	// Always use curwin, it may be a popup window.
	win_T *wwp = twp == عدم ? curwin : twp;

	// When more than one window shows the same terminal, use the
	// smallest size.
	إذا (wwp->w_بفر == غړی->tl_بفر)
	{
	    newrows = MIN(newrows, wwp->w_لوړوالی);
	    newcols = MIN(newcols, wwp->w_پلنوالی);
	}
	إذا (twp == عدم)
	    اكسر;
    }
    إذا (newrows == 99999 || newcols == 99999)
	أرجع; // safety exit
    newrows = لیکې_2 == 0 ? newrows : minsize ? MAX(لیکې_2, newrows) : لیکې_2;
    newcols = ستون_شمېر == 0 ? newcols : minsize ? MAX(ستون_شمېر, newcols) : ستون_شمېر;

    // If no cell is visible there is no point in resizing.  Also, vterm can't
    // handle a zero height.
    إذا (newrows == 0 || newcols == 0)
	أرجع;

    إذا (غړی->tl_لیکې != newrows || غړی->tl_ستون_شمېر != newcols)
    {
	غړی->tl_vterm_اندازه_بدل_شوی = TRUE;
	vterm_سېټ_اندازه(vterm, newrows, newcols);
	ch_log(غړی->tl_دنده->jv_channel, "Resizing terminal to %d lines",
								      newrows);
	غړی_report_winsize(غړی, newrows, newcols);

	// Updating the terminal size will cause the snapshot to be cleared.
	// When not in terminal_loop() we need to restore it.
	إذا (غړی != دننه_terminal_loop)
	    may_move_terminal_منزل_بفر(غړی, FALSE);
    }

    // The cursor may have been moved when resizing.
    vterm_state_get_cursorpos(state, &pos);
    position_cursor(wp, &pos);

    لكل (pos.لیکه = غړی->tl_dirty_لیکه_پیل; pos.لیکه < غړی->tl_dirty_لیکه_پای_ټکی
					  && pos.لیکه < wp->w_لوړوالی; ++pos.لیکه)
    {
	إذا (pos.لیکه < غړی->tl_لیکې)
	{
	    صحيح max_ستون = MIN(wp->w_پلنوالی, غړی->tl_ستون_شمېر);

	    غړی_line2screenline(غړی, wp, screen, &pos, max_ستون);
	}
	وإلا
	    pos.ستون = 0;

	screen_کرښه(wp, wp->w_winrow + pos.لیکه
#که_تعریف_شوی FEAT_MENU
				+ winbar_لوړوالی(wp)
#د_که_پای
				, wp->w_wincol, pos.ستون, wp->w_پلنوالی, -1,
#که_تعریف_شوی FEAT_PROP_POPUP
				popup_is_popup(wp) ? SLF_POPUP :
#د_که_پای
				0);
    }
}

/*
 * Called after updating all windows: may reset dirty rows.
 */
    فراغ
غړی_did_update_کړکۍ(win_T *wp)
{
    غړی_T	*غړی = wp->w_بفر->b_غړی;

    إذا (غړی == عدم || غړی->tl_vterm == عدم || غړی->tl_نورمال_ډېرپېښ
						|| wp->w_redr_type != 0)
	أرجع;

    غړی->tl_dirty_لیکه_پیل = MAX_لیکه;
    غړی->tl_dirty_لیکه_پای_ټکی = 0;
}

/*
 * Return TRUE if "wp" is a terminal window where the job has finished.
 */
    صحيح
غړی_is_پای_ته_رسېدلی(buf_T *buf)
{
    أرجع buf->b_غړی != عدم && buf->b_غړی->tl_vterm == عدم;
}

/*
 * Return TRUE if "wp" is a terminal window where the job has finished or we
 * are in Terminal-Normal mode, thus we show the buffer contents.
 */
    صحيح
غړی_show_بفر(buf_T *buf)
{
    غړی_T *غړی = buf->b_غړی;

    أرجع غړی != عدم && (غړی->tl_vterm == عدم || غړی->tl_نورمال_ډېرپېښ);
}

/*
 * The current buffer is going to be changed.  If there is terminal
 * highlighting remove it now.
 */
    فراغ
غړی_change_دننه_curbuf(فراغ)
{
    غړی_T *غړی = curbuf->b_غړی;

    إذا (!غړی_is_پای_ته_رسېدلی(curbuf) || غړی->tl_scrollback.ga_len <= 0)
	أرجع;

    free_scrollback(غړی);
    redraw_buf_later(غړی->tl_بفر, UPD_NOT_VALID);

    // The buffer is now like a normal buffer, it cannot be easily
    // abandoned when changed.
    سېټ_string_option_مستقیم((char_u *)"buftype", -1,
	    (char_u *)"", OPT_FREE|OPT_LOCAL, 0);
}

/*
 * Get the screen attribute for a position in the buffer.
 * Use a negative "col" to get the filler background color.
 */
    صحيح
غړی_get_attr(win_T *wp, linenr_T lnum, صحيح ستون)
{
    buf_T	*buf = wp->w_بفر;
    غړی_T	*غړی = buf->b_غړی;
    sb_کرښه_T	*کرښه;
    cellattr_T	*cellattr;
    صحيح         sb_کرښه = -1;
    صحيح         sb_ستون = ستون;

    إذا (غړی->tl_scrollback.ga_len)
	bufline_pos_دننه_scrollback(غړی, lnum, ستون, &sb_کرښه, &sb_ستون);

    إذا (sb_کرښه < 0)
	cellattr = &غړی->tl_default_color;
    وإلا
    {
	کرښه = (sb_کرښه_T *)غړی->tl_scrollback.ga_معلومات + sb_کرښه;
	إذا (sb_ستون < 0 || sb_ستون >= کرښه->sb_ستون_شمېر)
	    cellattr = &کرښه->sb_fill_attr;
	وإلا
	    cellattr = کرښه->sb_cells + sb_ستون;
    }
    أرجع cell2attr(غړی, wp, &cellattr->attrs, &cellattr->fg, &cellattr->bg);
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
	rgb->type = VTERM_COLOR_شاخصي;
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
	    || gui.دننه_use
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
	    fg->red = (بینښه)(rgb >> 16);
	    fg->green = (بینښه)(rgb >> 8) & 255;
	    fg->blue = (بینښه)rgb & 255;
	    fg->type = VTERM_COLOR_RGB | VTERM_COLOR_DEFAULT_FG;
	}
	وإلا
	    fg->type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_FG;

	إذا (bg_rgb != INVALCOLOR)
	{
	    long_u rgb = GUI_MCH_GET_RGB(bg_rgb);
	    bg->red = (بینښه)(rgb >> 16);
	    bg->green = (بینښه)(rgb >> 8) & 255;
	    bg->blue = (بینښه)rgb & 255;
	    bg->type = VTERM_COLOR_RGB | VTERM_COLOR_DEFAULT_BG;
	}
	وإلا
	    bg->type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_BG;

	أرجع TRUE;
    }
    وإلا
#د_که_پای
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
غړی_reset_hlfwin(win_T *wp)
{
    wp->w_غړی_hlfwin.fg.type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_FG;
    wp->w_غړی_hlfwin.bg.type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_BG;
}

/*
 * Cache the color of HLF_WIN.
 */
    فراغ
غړی_update_hlfwin(win_T *wp)
{
    صحيح id = wp->w_hlfwin_id;

    إذا (id == 0 || !get_vterm_color_from_synid(id == -1 ? 0 : id,
		&wp->w_غړی_hlfwin.fg, &wp->w_غړی_hlfwin.bg))
	غړی_reset_hlfwin(wp);
}

/*
 * Called when option 'termguicolors' was set,
 * or when any highlight is changed.
 */
    فراغ
غړی_update_hlfwin_all(فراغ)
{
    win_T	 *wp = عدم;
    صحيح		 did_curwin = FALSE;

    طالما (for_all_windows_and_curwin(&wp, &did_curwin))
	غړی_update_hlfwin(wp);
}

/*
 * Initialize term->tl_default_color from the environment.
 */
    فراغ
غړی_init_default_colors(غړی_T *غړی)
{
    VTermColor	    *fg, *bg;
    صحيح		    fgval, bgval;
    صحيح		    id;

    CLEAR_ډګر(غړی->tl_default_color.attrs);
    غړی->tl_default_color.پلنوالی = 1;
    fg = &غړی->tl_default_color.fg;
    bg = &غړی->tl_default_color.bg;

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
    id = غړی_get_highlight_id(غړی, عدم);

    إذا (!get_vterm_color_from_synid(id, fg, bg))
    {
#إذا defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
	صحيح tmp;
#د_که_پای

	// In an MS-Windows console we know the normal colors.
	إذا (cterm_نورمال_fg_color > 0)
	{
	    cterm_color2vterm(cterm_نورمال_fg_color - 1, fg);
#إذا defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
# ifdef VIMDLL
	    إذا (!gui.دننه_use)
# endif
	    {
		tmp = fg->red;
		fg->red = fg->blue;
		fg->blue = tmp;
	    }
#د_که_پای
	}
#که_تعریف_شوی FEAT_TERMRESPONSE
	وإلا
	    غړی_get_fg_color(&fg->red, &fg->green, &fg->blue);
#د_که_پای

	إذا (cterm_نورمال_bg_color > 0)
	{
	    cterm_color2vterm(cterm_نورمال_bg_color - 1, bg);
#إذا defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
# ifdef VIMDLL
	    إذا (!gui.دننه_use)
# endif
	    {
		tmp = fg->red;
		fg->red = fg->blue;
		fg->blue = tmp;
	    }
#د_که_پای
	}
#که_تعریف_شوی FEAT_TERMRESPONSE
	وإلا
	    غړی_get_bg_color(&bg->red, &bg->green, &bg->blue);
#د_که_پای
    }
}

#إذا defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Return TRUE if the user-defined palette (either g:terminal_ansi_colors or the
 * "ansi_colors" argument in term_start()) shall be applied.
 */
    ساكن صحيح
غړی_use_palette(فراغ)
{
    إذا (0
# ifdef FEAT_GUI
	    || gui.دننه_use
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
سېټ_vterm_palette(VTerm *vterm, long_u *rgb)
{
    صحيح		شاخص = 0;
    VTermState	*state = vterm_obtain_state(vterm);

    لكل (; شاخص < 16; شاخص++)
    {
	VTermColor	color;

	color.type = VTERM_COLOR_RGB;
	color.red = (بینښه)(rgb[شاخص] >> 16);
	color.green = (بینښه)(rgb[شاخص] >> 8) & 255;
	color.blue = (بینښه)rgb[شاخص] & 255;
	color.شاخص = 0;
	vterm_state_سېټ_palette_color(state, شاخص, &color);
    }
}

/*
 * Set the ANSI color palette from a list of colors
 */
    ساكن صحيح
سېټ_ansi_colors_list(VTerm *vterm, list_T *list)
{
    صحيح		n = 0;
    long_u	rgb[16];
    listitem_T	*li;

    لكل (li = list->lv_لومړی; li != عدم && n < 16; li = li->li_بل, n++)
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

    سېټ_vterm_palette(vterm, rgb);

    أرجع OK;
}

/*
 * Initialize the ANSI color palette from g:terminal_ansi_colors[0:15]
 */
    ساكن فراغ
init_vterm_ansi_colors(VTerm *vterm)
{
    dictitem_T	*var = پیدا_کړه_var((char_u *)"g:terminal_ansi_colors", عدم, TRUE);

    إذا (var == عدم)
	أرجع;

    إذا (var->di_tv.v_type != VAR_LIST
	    || var->di_tv.vval.v_list == عدم
	    || var->di_tv.vval.v_list->lv_لومړی == &range_list_توکی
	    || سېټ_ansi_colors_list(vterm, var->di_tv.vval.v_list) == FAIL)
	semsg(_(e_invalid_دلیل_str), "g:terminal_ansi_colors");
}
#د_که_پای

/*
 * Handles a "drop" command from the job in the terminal.
 * "item" is the file name, "item->li_next" may have options.
 */
    ساكن فراغ
handle_drop_command(listitem_T *توکی)
{
    char_u	*fname = tv_get_string(&توکی->li_tv);
    listitem_T	*opt_توکی = توکی->li_بل;
    صحيح		bufnr;
    win_T	*wp;
    tabpage_T   *tp;
    exarg_T	ea;
    char_u	*tofree = عدم;

    bufnr = buflist_جمع_کړه(fname, BLN_LISTED | BLN_NOOPT);
    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	إذا (wp->w_بفر->b_fnum == bufnr)
	{
	    // buffer is in a window already, go there
	    goto_tabpage_win(tp, wp);
	    أرجع;
	}
    }

    CLEAR_ډګر(ea);

    إذا (opt_توکی != عدم && opt_توکی->li_tv.v_type == VAR_DICT
					&& opt_توکی->li_tv.vval.v_dict != عدم)
    {
	dict_T *dict = opt_توکی->li_tv.vval.v_dict;
	char_u *p;

	p = dict_get_string(dict, "ff", FALSE);
	إذا (p == عدم)
	    p = dict_get_string(dict, "fileformat", FALSE);
	إذا (p != عدم)
	{
	    إذا (check_ff_ارزښت(p) == FAIL)
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

	إذا (dict_has_کلی(dict, "bin"))
	    ea.force_bin = FORCE_BIN;
	إذا (dict_has_کلی(dict, "binary"))
	    ea.force_bin = FORCE_BIN;
	إذا (dict_has_کلی(dict, "nobin"))
	    ea.force_bin = FORCE_NOBIN;
	إذا (dict_has_کلی(dict, "nobinary"))
	    ea.force_bin = FORCE_NOBIN;
    }

    // open in new window, like ":split fname"
    إذا (ea.cmd == عدم)
	ea.cmd = (char_u *)"split";
    ea.arg = fname;
    ea.cmdidx = CMD_وېشل;
    ex_splitview(&ea);

    vim_free(tofree);
}

/*
 * Return TRUE if "func" starts with "pat" and "pat" isn't empty.
 */
    ساكن صحيح
is_permitted_غړی_api(char_u *func, char_u *pat)
{
    أرجع pat != عدم && *pat != NUL && STRNICMP(func, pat, STRLEN(pat)) == 0;
}

/*
 * Handles a function call from the job running in a terminal.
 * "item" is the function name, "item->li_next" has the arguments.
 */
    ساكن فراغ
handle_call_command(غړی_T *غړی, channel_T *channel, listitem_T *توکی)
{
    char_u	*func;
    typval_T	argvars[2];
    typval_T	rettv;
    funcexe_T	funcexe;

    إذا (توکی->li_بل == عدم)
    {
	ch_log(channel, "Missing function arguments for call");
	أرجع;
    }
    func = tv_get_string(&توکی->li_tv);

    إذا (!is_permitted_غړی_api(func, غړی->tl_api))
    {
	ch_log(channel, "Unpermitted function: %s", func);
	أرجع;
    }

    argvars[0].v_type = VAR_شمېره;
    argvars[0].vval.v_شمېره = غړی->tl_بفر->b_fnum;
    argvars[1] = توکی->li_بل->li_tv;
    CLEAR_ډګر(funcexe);
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
    صحيح       دننه_واټن = 7;  // len of "file://" is 7
    حرف      *pos = (حرف *)gap->ga_معلومات + دننه_واټن;
    char_u    *new_dir;

    // remove HOSTNAME to get PWD
    طالما (دننه_واټن < (صحيح)gap->ga_len && *pos != '/' )
    {
	++دننه_واټن;
	++pos;
    }

    إذا (دننه_واټن >= (صحيح)gap->ga_len)
    {
	semsg(_(e_failed_منزل_ایستل_pwd_from_str_check_your_shell_config),
								 gap->ga_معلومات);
	أرجع;
    }

    new_dir = alloc(gap->ga_len - دننه_واټن + 1);
    url_decode(pos, gap->ga_len-دننه_واټن, new_dir);
    changedir_func(new_dir, TRUE, CDSCOPE_کړکۍ);
    vim_free(new_dir);
}

/*
 * Called by libvterm when it cannot recognize an OSC sequence.
 * We recognize a terminal API command.
 */
    ساكن صحيح
parse_osc(صحيح command, VTermStringFragment frag, فراغ *user)
{
    غړی_T	*غړی = (غړی_T *)user;
    js_read_T	reader;
    typval_T	tv;
    channel_T	*channel = غړی->tl_دنده == عدم ? عدم
						    : غړی->tl_دنده->jv_channel;
    garray_T	*gap = &غړی->tl_osc_buf;

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
    reader.js_کارول_شوی = 0;
    إذا (json_decode(&reader, &tv, 0) == OK
	    && tv.v_type == VAR_LIST
	    && tv.vval.v_list != عدم)
    {
	listitem_T *توکی = tv.vval.v_list->lv_لومړی;

	إذا (توکی == عدم)
	    ch_log(channel, "Missing command");
	وإلا
	{
	    char_u	*cmd = tv_get_string(&توکی->li_tv);

	    // Make sure an invoked command doesn't delete the buffer (and the
	    // terminal) under our fingers.
	    ++غړی->tl_بفر->b_locked;

	    توکی = توکی->li_بل;
	    إذا (توکی == عدم)
		ch_log(channel, "Missing argument for %s", cmd);
	    وإلا إذا (STRCMP(cmd, "drop") == 0)
		handle_drop_command(توکی);
	    وإلا إذا (STRCMP(cmd, "call") == 0)
		handle_call_command(غړی, channel, توکی);
	    وإلا
		ch_log(channel, "Invalid command received: %s", cmd);
	    --غړی->tl_بفر->b_locked;
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
	ثابت حرف  *leader ناکارول_شوی,
	ثابت اوږد  دلیلونه[],
	صحيح	    argcount,
	ثابت حرف  *intermed ناکارول_شوی,
	حرف	    command,
	فراغ	    *user)
{
    غړی_T	*غړی = (غړی_T *)user;
    حرف	buf[100];
    صحيح		len;
    صحيح		x = 0;
    صحيح		y = 0;
    win_T	*wp;

    // We recognize only CSI 13 t
    إذا (command != 't' || argcount != 1 || دلیلونه[0] != 13)
	أرجع 0; // not handled

    // When getting the window position is not possible or it fails it results
    // in zero/zero.
#إذا defined(FEAT_GUI) \
	|| (defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)) \
	|| defined(MSWIN)
    (فراغ)ui_get_winpos(&x, &y, (varnumber_T)100);
#د_که_پای

    FOR_ALL_WINDOWS(wp)
	إذا (wp->w_بفر == غړی->tl_بفر)
	    اكسر;
    إذا (wp != عدم)
    {
#که_تعریف_شوی FEAT_GUI
	إذا (gui.دننه_use)
	{
	    x += wp->w_wincol * gui.char_پلنوالی;
	    y += W_WINROW(wp) * gui.char_لوړوالی;
	}
	وإلا
#د_که_پای
	{
	    // We roughly estimate the position of the terminal window inside
	    // the Vim window by assuming a 10 x 7 character cell.
	    x += wp->w_wincol * 7;
	    y += W_WINROW(wp) * 10;
	}
    }

    len = vim_snprintf(buf, 100, "\x1b[3;%d;%dt", x, y);
    channel_send(غړی->tl_دنده->jv_channel, get_tty_part(غړی),
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
vterm_malloc(size_t اندازه, فراغ *معلومات ناکارول_شوی)
{
    // make sure that the length is not zero
    أرجع alloc_clear(اندازه == 0 ? 1L : اندازه);
}

    ساكن فراغ
vterm_memfree(فراغ *ptr, فراغ *معلومات ناکارول_شوی)
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
create_vterm(غړی_T *غړی, صحيح لیکې_2, صحيح ستون_شمېر)
{
    VTerm	    *vterm;
    VTermScreen	    *screen;
    VTermState	    *state;
    VTerm_ارزښت	    ارزښت;

    vterm = vterm_new_with_allocator(لیکې_2, ستون_شمېر, &vterm_allocator, عدم);
    غړی->tl_vterm = vterm;
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

    vterm_screen_سېټ_callbacks(screen, &screen_callbacks, غړی);
    vterm_screen_سېټ_damage_یوځای(screen, VTERM_DAMAGE_SCROLL);
    vterm_screen_callbacks_has_pushline4(screen);
    // TODO: depends on 'encoding'.
    vterm_سېټ_utf8(vterm, 1);

    غړی_init_default_colors(غړی);

    vterm_state_سېټ_default_colors(
	    state,
	    &غړی->tl_default_color.fg,
	    &غړی->tl_default_color.bg);

    إذا (t_colors < 16)
	// Less than 16 colors: assume that bold means using a bright color for
	// the foreground color.
	vterm_state_سېټ_bold_highbright(vterm_obtain_state(vterm), 1);

    // Required to initialize most things.
    vterm_screen_reset(screen, 1 /* hard */);

    // Allow using alternate screen.
    vterm_screen_enable_altscreen(screen, 1);

    // For unix do not use a blinking cursor.  In an xterm this causes the
    // cursor to blink if it's blinking in the xterm.
    // For Windows we respect the system wide setting.
#که_تعریف_شوی MSWIN
    إذا (Get_Caret_Blink_وخت() == INFINITE)
	ارزښت.boolean = 0;
    وإلا
	ارزښت.boolean = 1;
#وإلا
    ارزښت.boolean = 0;
#د_که_پای
    vterm_state_سېټ_termprop(state, VTERM_PROP_CURSORBLINK, &ارزښت);
    vterm_state_سېټ_unrecognised_fallbacks(state, &state_fallbacks, غړی);

    أرجع OK;
}

/*
 * Reset the terminal palette to its default value.
 */
    ساكن فراغ
غړی_reset_palette(VTerm *vterm)
{
    VTermState	*state = vterm_obtain_state(vterm);
    صحيح		شاخص;

    لكل (شاخص = 0; شاخص < 16; شاخص++)
    {
	VTermColor	color;

	color.type = VTERM_COLOR_شاخصي;
	ansi_color2rgb(شاخص, &color.red, &color.green, &color.blue,
		&color.شاخص);
	// The first valid index starts at 1.
	color.شاخص -= 1;

	vterm_state_سېټ_palette_color(state, شاخص, &color);
    }
}

    ساكن فراغ
غړی_update_palette(غړی_T *غړی)
{
#إذا defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    إذا (غړی_use_palette()
	    && (غړی->tl_palette != عدم
		|| پیدا_کړه_var((char_u *)"g:terminal_ansi_colors", عدم, TRUE)
								      != عدم))
    {
	إذا (غړی->tl_palette != عدم)
	    سېټ_vterm_palette(غړی->tl_vterm, غړی->tl_palette);
	وإلا
	    init_vterm_ansi_colors(غړی->tl_vterm);
    }
    وإلا
#د_که_پای
	غړی_reset_palette(غړی->tl_vterm);
}

/*
 * Called when option 'termguicolors' is changed.
 */
    فراغ
غړی_update_palette_all(فراغ)
{
    غړی_T *غړی;

    FOR_ALL_TERMS(غړی)
    {
	إذا (غړی->tl_vterm == عدم)
	    استمر;
	غړی_update_palette(غړی);
    }
}

/*
 * Called when option 'background' or 'termguicolors' was set,
 * or when any highlight is changed.
 */
    فراغ
غړی_update_colors_all(فراغ)
{
    غړی_T *غړی;

    FOR_ALL_TERMS(غړی)
    {
	إذا (غړی->tl_vterm == عدم)
	    استمر;
	غړی_init_default_colors(غړی);
	vterm_state_سېټ_default_colors(
		vterm_obtain_state(غړی->tl_vterm),
		&غړی->tl_default_color.fg,
		&غړی->tl_default_color.bg);
    }
}

/*
 * Return the text to show for the buffer name and status.
 */
    char_u *
غړی_get_حالت_متن(غړی_T *غړی)
{
    إذا (غړی->tl_حالت_متن != عدم)
	أرجع غړی->tl_حالت_متن;

    char_u *txt;
    size_t len;
    char_u *fname;

    إذا (غړی->tl_نورمال_ډېرپېښ)
    {
	إذا (غړی_دنده_running(غړی))
	    txt = (char_u *)_("Terminal");
	وإلا
	    txt = (char_u *)_("Terminal-finished");
    }
    وإلا إذا (غړی->tl_title != عدم)
	txt = غړی->tl_title;
    وإلا إذا (غړی_none_open(غړی))
	txt = (char_u *)_("active");
    وإلا إذا (غړی_دنده_running(غړی))
	txt = (char_u *)_("running");
    وإلا
	txt = (char_u *)_("finished");
    fname = buf_get_fname(غړی->tl_بفر);
    len = 9 + STRLEN(fname) + STRLEN(txt);
    غړی->tl_حالت_متن = alloc(len);
    إذا (غړی->tl_حالت_متن != عدم)
	vim_snprintf((حرف *)غړی->tl_حالت_متن, len, "%s [%s]",
		fname, txt);
    أرجع غړی->tl_حالت_متن;
}

/*
 * Clear the cached value of the status text.
 */
    فراغ
غړی_clear_حالت_متن(غړی_T *غړی)
{
    VIM_CLEAR(غړی->tl_حالت_متن);
}

/*
 * Mark references in jobs of terminals.
 */
    صحيح
سېټ_ref_دننه_غړی(صحيح copyID)
{
    صحيح		abort = FALSE;
    غړی_T	*غړی;
    typval_T	tv;

    لكل (غړی = لومړی_غړی; !abort && غړی != عدم; غړی = غړی->tl_بل)
	إذا (غړی->tl_دنده != عدم)
	{
	    tv.v_type = VAR_دنده;
	    tv.vval.v_دنده = غړی->tl_دنده;
	    abort = abort || سېټ_ref_دننه_توکی(&tv, copyID, عدم, عدم, عدم);
	}
    أرجع abort;
}

/*
 * Get the buffer from the first argument in "argvars".
 * Returns NULL when the buffer is not for a terminal window and logs a message
 * with "where".
 */
    ساكن buf_T *
غړی_get_buf(typval_T *argvars, حرف *where)
{
    buf_T *buf;

    ++emsg_off;
    buf = tv_get_buf(&argvars[0], FALSE);
    --emsg_off;
    إذا (buf == عدم || buf->b_غړی == عدم)
    {
	(فراغ)tv_get_شمېره(&argvars[0]);    // issue errmsg if type error
	ch_log(عدم, "%s: invalid buffer argument", where);
	أرجع عدم;
    }
    أرجع buf;
}

    ساكن فراغ
clear_cell(VTermScreenCell *cell)
{
    CLEAR_ډګر(*cell);
    cell->fg.type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_FG;
    cell->bg.type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_BG;
}

    ساكن فراغ
dump_غړی_color(FILE *fd, VTermColor *color)
{
    صحيح شاخص;

    إذا (VTERM_COLOR_IS_شاخصي(color))
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
f_غړی_dumpwrite(typval_T *argvars, typval_T *rettv ناکارول_شوی)
{
    buf_T	*buf;
    غړی_T	*غړی;
    char_u	*fname;
    صحيح		max_لوړوالی_2 = 0;
    صحيح		max_پلنوالی_2 = 0;
    stat_T	st;
    FILE	*fd;
    VTermPos	pos;
    VTermScreen *screen;
    VTermScreenCell prev_cell;
    VTermState	*state;
    VTermPos	cursor_pos;

    إذا (check_restricted() || check_secure())
	أرجع;

    إذا (دننه_vim9script()
	    && (check_for_بفر_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	أرجع;

    buf = غړی_get_buf(argvars, "term_dumpwrite()");
    إذا (buf == عدم)
	أرجع;
    غړی = buf->b_غړی;
    إذا (غړی->tl_vterm == عدم)
    {
	emsg(_(e_دنده_already_پای_ته_رسېدلی));
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
	    max_لوړوالی_2 = dict_get_شمېره(d, "rows");
	    max_پلنوالی_2 = dict_get_شمېره(d, "columns");
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

    screen = vterm_obtain_screen(غړی->tl_vterm);
    state = vterm_obtain_state(غړی->tl_vterm);
    vterm_state_get_cursorpos(state, &cursor_pos);

    لكل (pos.لیکه = 0; (max_لوړوالی_2 == 0 || pos.لیکه < max_لوړوالی_2)
					 && pos.لیکه < غړی->tl_لیکې; ++pos.لیکه)
    {
	صحيح		repeat = 0;

	لكل (pos.ستون = 0; (max_پلنوالی_2 == 0 || pos.ستون < max_پلنوالی_2)
					 && pos.ستون < غړی->tl_ستون_شمېر; ++pos.ستون)
	{
	    VTermScreenCell cell;
	    صحيح		    same_attr;
	    صحيح		    same_chars = TRUE;
	    صحيح		    i;
	    صحيح		    is_cursor_pos = (pos.ستون == cursor_pos.ستون
						 && pos.لیکه == cursor_pos.لیکه);

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
	    إذا (same_chars && cell.پلنوالی == prev_cell.پلنوالی && same_attr
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
		إذا (cell.پلنوالی != prev_cell.پلنوالی || !same_attr)
		{
		    إذا (cell.پلنوالی == 2)
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
			    dump_غړی_color(fd, &cell.fg);
			}
			إذا (vterm_color_is_equal(&cell.bg, &prev_cell.bg))
			    fputs("&", fd);
			وإلا
			{
			    fputs("#", fd);
			    dump_غړی_color(fd, &cell.bg);
			}
		    }
		}

		prev_cell = cell;
	    }

	    إذا (cell.پلنوالی == 2)
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

    *(((cellattr_T *)gap->ga_معلومات) + gap->ga_len) = *cell;
    ++gap->ga_len;
}

    ساكن فراغ
clear_cellattr(cellattr_T *cell)
{
    CLEAR_ډګر(*cell);
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
    cellattr_T	    تشه_cell;
    غړی_T	    *غړی = curbuf->b_غړی;
    صحيح		    max_cells = 0;
    صحيح		    پیل_لیکه = غړی->tl_scrollback.ga_len;

    ga_init2(&ga_متن, 1, 90);
    ga_init2(&ga_cell, حجم(cellattr_T), 90);
    clear_cellattr(&cell);
    clear_cellattr(&تشه_cell);
    cursor_pos->لیکه = -1;
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
	    إذا (ga_متن.ga_معلومات == عدم)
		dump_is_corrupt(&ga_متن);
	    إذا (ga_grow(&غړی->tl_scrollback, 1) == OK)
	    {
		sb_کرښه_T   *کرښه = (sb_کرښه_T *)غړی->tl_scrollback.ga_معلومات
						  + غړی->tl_scrollback.ga_len;

		إذا (max_cells < ga_cell.ga_len)
		    max_cells = ga_cell.ga_len;
		کرښه->sb_ستون_شمېر = ga_cell.ga_len;
		کرښه->sb_بایټونه = ga_متن.ga_len;
		کرښه->sb_cells = ga_cell.ga_معلومات;
		کرښه->sb_fill_attr = غړی->tl_default_color;
		کرښه->continuation = 0;
		++غړی->tl_scrollback.ga_len;
		ga_init(&ga_cell);

		ga_append(&ga_متن, NUL);
		ml_append(curbuf->b_ml.ml_کرښه_شمېر, ga_متن.ga_معلومات,
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
		إذا (cursor_pos->لیکه != -1)
		    dump_is_corrupt(&ga_متن);	// duplicate cursor
		cursor_pos->لیکه = غړی->tl_scrollback.ga_len - پیل_لیکه;
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
	    إذا (ga_متن.ga_معلومات != عدم)
	    {
		prev_char.اوږدتیا = (size_t)(ga_متن.ga_len - prev_len);
		prev_char.string = vim_strnsave(
		    ((char_u *)ga_متن.ga_معلومات) + prev_len,
		    prev_char.اوږدتیا);
		إذا (prev_char.string == عدم)
		    prev_char.اوږدتیا = 0;
	    }

	    إذا (c == '@' || c == '|' || c == '>' || c == '\n')
	    {
		// use all attributes from previous cell
	    }
	    وإلا إذا (c == '+' || c == '*')
	    {
		صحيح is_bg;

		cell.پلنوالی = c == '+' ? 1 : 2;

		c = fgetc(fd);
		إذا (c == '&')
		{
		    // use same attr as previous cell
		    c = fgetc(fd);
		}
		وإلا إذا (خوندي_isdigit(c))
		{
		    // get the decimal attribute
		    attr = 0;
		    طالما (خوندي_isdigit(c))
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
			    إذا (!خوندي_isdigit(c))
				dump_is_corrupt(&ga_متن);
			    طالما (خوندي_isdigit(c))
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
				type = VTERM_COLOR_شاخصي;
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
	    إذا (cell.پلنوالی == 2)
		append_cell(&ga_cell, &تشه_cell);
	}
	وإلا إذا (c == '@')
	{
	    إذا (prev_char.string == عدم)
		dump_is_corrupt(&ga_متن);
	    وإلا
	    {
		صحيح شمېر = 0;

		// repeat previous character, get the count
		لكل (;;)
		{
		    c = fgetc(fd);
		    إذا (!خوندي_isdigit(c))
			اكسر;
		    شمېر = شمېر * 10 + (c - '0');
		}

		طالما (شمېر-- > 0)
		{
		    ga_concat_len(&ga_متن, prev_char.string, prev_char.اوږدتیا);
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
	ml_append(curbuf->b_ml.ml_کرښه_شمېر, ga_متن.ga_معلومات,
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
get_separator(صحيح متن_پلنوالی, char_u *fname)
{
    صحيح	    پلنوالی = MAX(متن_پلنوالی, curwin->w_پلنوالی);
    char_u  *textline;
    صحيح	    fname_اندازه;
    char_u  *p = fname;
    صحيح	    i;
    size_t  off;

    textline = alloc(پلنوالی + STRLEN(fname) + 1);
    إذا (textline == عدم)
	أرجع عدم;

    fname_اندازه = vim_strsize(fname);
    إذا (fname_اندازه < پلنوالی - 8)
    {
	// enough room, don't use the full window width
	پلنوالی = MAX(متن_پلنوالی, fname_اندازه + 8);
    }
    وإلا إذا (fname_اندازه > پلنوالی - 8)
    {
	// full name doesn't fit, use only the tail
	p = gettail(fname);
	fname_اندازه = vim_strsize(p);
    }
    // skip characters until the name fits
    طالما (fname_اندازه > پلنوالی - 8)
    {
	p += (*mb_ptr2len)(p);
	fname_اندازه = vim_strsize(p);
    }

    لكل (i = 0; i < (پلنوالی - fname_اندازه) / 2 - 1; ++i)
	textline[i] = '=';
    textline[i++] = ' ';

    STRCPY(textline + i, p);
    off = STRLEN(textline);
    textline[off] = ' ';
    لكل (i = 1; i < (پلنوالی - fname_اندازه) / 2; ++i)
	textline[off + i] = '=';
    textline[off + i] = NUL;

    أرجع textline;
}

/*
 * Common for "term_dumpdiff()" and "term_dumpload()".
 */
    ساكن فراغ
غړی_بارول_dump(typval_T *argvars, typval_T *rettv, صحيح do_diff)
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

    init_دنده_options(&opt);
    إذا (argvars[do_diff ? 2 : 1].v_type != VAR_UNKNOWN
	    && get_دنده_options(&argvars[do_diff ? 2 : 1], &opt, 0,
		    JO2_غړی_NAME + JO2_غړی_ستون_شمېر + JO2_غړی_لیکې
		    + JO2_VERTICAL + JO2_CURWIN + JO2_NORESTORE) == FAIL)
	اذهب theend;

    إذا (opt.jo_غړی_name == عدم)
    {
	size_t len = STRLEN(fname1) + 12;

	fname_tofree = alloc(len);
	إذا (fname_tofree != عدم)
	{
	    vim_snprintf((حرف *)fname_tofree, len, "dump diff %s", fname1);
	    opt.jo_غړی_name = fname_tofree;
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
	    طالما (!(curbuf->b_ml.ml_flags & ML_تشه))
		ml_delete((linenr_T)1);
	    free_scrollback(curbuf->b_غړی);
	    redraw_later(UPD_NOT_VALID);
	}
    }
    وإلا
	// Create a new terminal window.
	buf = غړی_پیل(&argvars[0], عدم, &opt, غړی_پیل_NOJOB);

    إذا (buf != عدم && buf->b_غړی != عدم)
    {
	صحيح		i;
	linenr_T	bot_lnum;
	linenr_T	lnum;
	غړی_T		*غړی = buf->b_غړی;
	صحيح		پلنوالی;
	صحيح		width2;
	VTermPos	cursor_pos1;
	VTermPos	cursor_pos2;

	غړی_init_default_colors(غړی);

	rettv->vval.v_شمېره = buf->b_fnum;

	// read the files, fill the buffer with the diff
	پلنوالی = read_dump_file(fd1, &cursor_pos1);

	// position the cursor
	إذا (cursor_pos1.لیکه >= 0)
	{
	    curwin->w_cursor.lnum = cursor_pos1.لیکه + 1;
	    coladvance(cursor_pos1.ستون);
	}

	// Delete the empty line that was in the empty buffer.
	ml_delete(1);

	// For term_dumpload() we are done here.
	إذا (!do_diff)
	    اذهب theend;

	غړی->tl_سر_diff_لیکې = curbuf->b_ml.ml_کرښه_شمېر;

	textline = get_separator(پلنوالی, fname1);
	إذا (textline == عدم)
	    اذهب theend;
	إذا (جمع_کړه_تشه_scrollback(غړی, &غړی->tl_default_color, 0) == OK)
	    ml_append(curbuf->b_ml.ml_کرښه_شمېر, textline, 0, FALSE);
	vim_free(textline);

	textline = get_separator(پلنوالی, fname2);
	إذا (textline == عدم)
	    اذهب theend;
	إذا (جمع_کړه_تشه_scrollback(غړی, &غړی->tl_default_color, 0) == OK)
	    ml_append(curbuf->b_ml.ml_کرښه_شمېر, textline, 0, FALSE);
	textline[پلنوالی] = NUL;

	bot_lnum = curbuf->b_ml.ml_کرښه_شمېر;
	width2 = read_dump_file(fd2, &cursor_pos2);
	إذا (width2 > پلنوالی)
	{
	    vim_free(textline);
	    textline = alloc(width2 + 1);
	    إذا (textline == عدم)
		اذهب theend;
	    پلنوالی = width2;
	    textline[پلنوالی] = NUL;
	}
	غړی->tl_bot_diff_لیکې = curbuf->b_ml.ml_کرښه_شمېر - bot_lnum;

	لكل (lnum = 1; lnum <= غړی->tl_سر_diff_لیکې; ++lnum)
	{
	    إذا (lnum + bot_lnum > curbuf->b_ml.ml_کرښه_شمېر)
	    {
		// bottom part has fewer rows, fill with "-"
		لكل (i = 0; i < پلنوالی; ++i)
		    textline[i] = '-';
	    }
	    وإلا
	    {
		char_u *line1;
		char_u *line2;
		char_u *p1;
		char_u *p2;
		صحيح	ستون;
		sb_کرښه_T   *sb_کرښه = (sb_کرښه_T *)غړی->tl_scrollback.ga_معلومات;
		cellattr_T *cellattr1 = (sb_کرښه + lnum - 1)->sb_cells;
		cellattr_T *cellattr2 = (sb_کرښه + lnum + bot_lnum - 1)
								    ->sb_cells;

		// Make a copy, getting the second line will invalidate it.
		line1 = vim_strsave(ml_get(lnum));
		إذا (line1 == عدم)
		    اكسر;
		p1 = line1;

		line2 = ml_get(lnum + bot_lnum);
		p2 = line2;
		لكل (ستون = 0; ستون < پلنوالی && *p1 != NUL && *p2 != NUL; ++ستون)
		{
		    صحيح len1 = utfc_ptr2len(p1);
		    صحيح len2 = utfc_ptr2len(p2);

		    textline[ستون] = ' ';
		    إذا (len1 != len2 || STRNCMP(p1, p2, len1) != 0)
			// text differs
			textline[ستون] = 'X';
		    وإلا إذا (lnum == cursor_pos1.لیکه + 1
			    && ستون == cursor_pos1.ستون
			    && (cursor_pos1.لیکه != cursor_pos2.لیکه
					|| cursor_pos1.ستون != cursor_pos2.ستون))
			// cursor in first but not in second
			textline[ستون] = '>';
		    وإلا إذا (lnum == cursor_pos2.لیکه + 1
			    && ستون == cursor_pos2.ستون
			    && (cursor_pos1.لیکه != cursor_pos2.لیکه
					|| cursor_pos1.ستون != cursor_pos2.ستون))
			// cursor in second but not in first
			textline[ستون] = '<';
		    وإلا إذا (cellattr1 != عدم && cellattr2 != عدم)
		    {
			إذا ((cellattr1 + ستون)->پلنوالی
						   != (cellattr2 + ستون)->پلنوالی)
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

		طالما (ستون < پلنوالی)
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
	    إذا (جمع_کړه_تشه_scrollback(غړی, &غړی->tl_default_color,
						 غړی->tl_سر_diff_لیکې) == OK)
		ml_append(غړی->tl_سر_diff_لیکې + lnum, textline, 0, FALSE);
	    ++bot_lnum;
	}

	طالما (lnum + bot_lnum <= curbuf->b_ml.ml_کرښه_شمېر)
	{
	    // bottom part has more rows, fill with "+"
	    لكل (i = 0; i < پلنوالی; ++i)
		textline[i] = '+';
	    إذا (جمع_کړه_تشه_scrollback(غړی, &غړی->tl_default_color,
						 غړی->tl_سر_diff_لیکې) == OK)
		ml_append(غړی->tl_سر_diff_لیکې + lnum, textline, 0, FALSE);
	    ++lnum;
	    ++bot_lnum;
	}

	غړی->tl_ستون_شمېر = پلنوالی;

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
غړی_بدلون_diff(فراغ)
{
    غړی_T	*غړی = curbuf->b_غړی;
    linenr_T	کرښه_شمېر;
    linenr_T	سر_لیکې;
    linenr_T	bot_لیکې;
    linenr_T	bot_پیل;
    linenr_T	lnum;
    char_u	*p;
    sb_کرښه_T	*sb_کرښه;

    إذا (غړی == عدم
	    || !غړی_is_پای_ته_رسېدلی(curbuf)
	    || غړی->tl_سر_diff_لیکې == 0
	    || غړی->tl_scrollback.ga_len == 0)
	أرجع FAIL;

    کرښه_شمېر = curbuf->b_ml.ml_کرښه_شمېر;
    سر_لیکې = غړی->tl_سر_diff_لیکې;
    bot_لیکې = غړی->tl_bot_diff_لیکې;
    bot_پیل = کرښه_شمېر - bot_لیکې;
    sb_کرښه = (sb_کرښه_T *)غړی->tl_scrollback.ga_معلومات;

    // move lines from top to above the bottom part
    لكل (lnum = 1; lnum <= سر_لیکې; ++lnum)
    {
	p = vim_strsave(ml_get(1));
	إذا (p == عدم)
	    أرجع OK;
	ml_append(bot_پیل, p, 0, FALSE);
	ml_delete(1);
	vim_free(p);
    }

    // move lines from bottom to the top
    لكل (lnum = 1; lnum <= bot_لیکې; ++lnum)
    {
	p = vim_strsave(ml_get(bot_پیل + lnum));
	إذا (p == عدم)
	    أرجع OK;
	ml_delete(bot_پیل + lnum);
	ml_append(lnum - 1, p, 0, FALSE);
	vim_free(p);
    }

    // move top title to bottom
    p = vim_strsave(ml_get(bot_لیکې + 1));
    إذا (p == عدم)
	أرجع OK;
    ml_append(کرښه_شمېر - سر_لیکې - 1, p, 0, FALSE);
    ml_delete(bot_لیکې + 1);
    vim_free(p);

    // move bottom title to top
    p = vim_strsave(ml_get(کرښه_شمېر - سر_لیکې));
    إذا (p == عدم)
	أرجع OK;
    ml_delete(کرښه_شمېر - سر_لیکې);
    ml_append(bot_لیکې, p, 0, FALSE);
    vim_free(p);

    إذا (سر_لیکې == bot_لیکې)
    {
	// rows counts are equal, can swap cell properties
	لكل (lnum = 0; lnum < سر_لیکې; ++lnum)
	{
	    sb_کرښه_T	لنډمهاله;

	    لنډمهاله = *(sb_کرښه + lnum);
	    *(sb_کرښه + lnum) = *(sb_کرښه + bot_پیل + lnum);
	    *(sb_کرښه + bot_پیل + lnum) = لنډمهاله;
	}
    }
    وإلا
    {
	size_t		اندازه = حجم(sb_کرښه_T) * غړی->tl_scrollback.ga_len;
	sb_کرښه_T	*لنډمهاله = alloc(اندازه);

	// need to copy cell properties into temp memory
	إذا (لنډمهاله != عدم)
	{
	    mch_memmove(لنډمهاله, غړی->tl_scrollback.ga_معلومات, اندازه);
	    mch_memmove(غړی->tl_scrollback.ga_معلومات,
		    لنډمهاله + bot_پیل,
		    حجم(sb_کرښه_T) * bot_لیکې);
	    mch_memmove((sb_کرښه_T *)غړی->tl_scrollback.ga_معلومات + bot_لیکې,
		    لنډمهاله + سر_لیکې,
		    حجم(sb_کرښه_T) * (کرښه_شمېر - سر_لیکې - bot_لیکې));
	    mch_memmove((sb_کرښه_T *)غړی->tl_scrollback.ga_معلومات
						       + کرښه_شمېر - سر_لیکې,
		    لنډمهاله,
		    حجم(sb_کرښه_T) * سر_لیکې);
	    vim_free(لنډمهاله);
	}
    }

    غړی->tl_سر_diff_لیکې = bot_لیکې;
    غړی->tl_bot_diff_لیکې = سر_لیکې;

    update_screen(UPD_NOT_VALID);
    أرجع OK;
}

/*
 * "term_dumpdiff(filename, filename, options)" function
 */
    فراغ
f_غړی_dumpdiff(typval_T *argvars, typval_T *rettv)
{
    إذا (دننه_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	أرجع;

    غړی_بارول_dump(argvars, rettv, TRUE);
}

/*
 * "term_dumpload(filename, options)" function
 */
    فراغ
f_غړی_dumpload(typval_T *argvars, typval_T *rettv)
{
    إذا (دننه_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	أرجع;

    غړی_بارول_dump(argvars, rettv, FALSE);
}

/*
 * "term_getaltscreen(buf)" function
 */
    فراغ
f_غړی_getaltscreen(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    إذا (دننه_vim9script() && check_for_بفر_arg(argvars, 0) == FAIL)
	أرجع;

    buf = غړی_get_buf(argvars, "term_getaltscreen()");
    إذا (buf == عدم)
	أرجع;
    rettv->vval.v_شمېره = buf->b_غړی->tl_using_altscreen;
}

/*
 * "term_getattr(attr, name)" function
 */
    فراغ
f_غړی_getattr(typval_T *argvars, typval_T *rettv)
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

    إذا (دننه_vim9script()
	    && (check_for_شمېره_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	أرجع;

    attr = tv_get_شمېره(&argvars[0]);
    name = tv_get_string_chk(&argvars[1]);
    إذا (name == عدم)
	أرجع;

    إذا (attr > HL_ALL)
	attr = syn_attr2attr(attr);
    لكل (i = 0; i < ARRAY_اوږدتیا(attrs); ++i)
	إذا (STRCMP(name, attrs[i].name) == 0)
	{
	    rettv->vval.v_شمېره = (attr & attrs[i].attr) != 0 ? 1 : 0;
	    اكسر;
	}
}

/*
 * "term_getcursor(buf)" function
 */
    فراغ
f_غړی_getcursor(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    غړی_T	*غړی;
    list_T	*l;
    dict_T	*d;

    إذا (rettv_list_alloc(rettv) == FAIL)
	أرجع;

    إذا (دننه_vim9script() && check_for_بفر_arg(argvars, 0) == FAIL)
	أرجع;

    buf = غړی_get_buf(argvars, "term_getcursor()");
    إذا (buf == عدم)
	أرجع;
    غړی = buf->b_غړی;

    l = rettv->vval.v_list;
    list_append_شمېره(l, غړی->tl_cursor_pos.لیکه + 1);
    list_append_شمېره(l, غړی->tl_cursor_pos.ستون + 1);

    d = dict_alloc();
    إذا (d == عدم)
	أرجع;

    dict_جمع_کړه_شمېره(d, "visible", غړی->tl_cursor_visible);
    dict_جمع_کړه_شمېره(d, "blink", blink_state_is_inverted()
	    ? !غړی->tl_cursor_blink : غړی->tl_cursor_blink);
    dict_جمع_کړه_شمېره(d, "shape", غړی->tl_cursor_shape);
    dict_جمع_کړه_string(d, "color", cursor_color_get(غړی->tl_cursor_color));
    إذا (list_append_dict(l, d) == FAIL)
	dict_unref(d);
}

/*
 * "term_getjob(buf)" function
 */
    فراغ
f_غړی_getjob(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    إذا (دننه_vim9script() && check_for_بفر_arg(argvars, 0) == FAIL)
	أرجع;

    buf = غړی_get_buf(argvars, "term_getjob()");
    إذا (buf == عدم)
    {
	إذا (دننه_vim9script())
	{
	    rettv->v_type = VAR_دنده;
	    rettv->vval.v_دنده = عدم;
	}
	وإلا
	{
	    rettv->v_type = VAR_SPECIAL;
	    rettv->vval.v_شمېره = VVAL_NULL;
	}
	أرجع;
    }

    rettv->v_type = VAR_دنده;
    rettv->vval.v_دنده = buf->b_غړی->tl_دنده;
    إذا (rettv->vval.v_دنده != عدم)
	++rettv->vval.v_دنده->jv_refcount;
}

    ساكن صحيح
get_لیکه_شمېره(typval_T *tv, غړی_T *غړی)
{
    إذا (tv->v_type == VAR_STRING
	    && tv->vval.v_string != عدم
	    && STRCMP(tv->vval.v_string, ".") == 0)
	أرجع غړی->tl_cursor_pos.لیکه;
    أرجع (صحيح)tv_get_شمېره(tv) - 1;
}

/*
 * "term_getline(buf, row)" function
 */
    فراغ
f_غړی_getline(typval_T *argvars, typval_T *rettv)
{
    buf_T	    *buf;
    غړی_T	    *غړی;
    صحيح		    لیکه;

    rettv->v_type = VAR_STRING;

    إذا (دننه_vim9script()
	    && (check_for_بفر_arg(argvars, 0) == FAIL
		|| check_for_lnum_arg(argvars, 1) == FAIL))
	أرجع;

    buf = غړی_get_buf(argvars, "term_getline()");
    إذا (buf == عدم)
	أرجع;
    غړی = buf->b_غړی;
    لیکه = get_لیکه_شمېره(&argvars[1], غړی);

    إذا (غړی->tl_vterm == عدم)
    {
	linenr_T  lnum = 0;
	size_t	  دننه_واټن = 0;
	صحيح	  sb_لیکه = غړی->tl_scrollback_scrolled + لیکه;
	sb_کرښه_T *کرښه;

	إذا (sb_لیکه < 0 || sb_لیکه >= غړی->tl_scrollback.ga_len)
	    أرجع;
	کرښه = (sb_کرښه_T *)غړی->tl_scrollback.ga_معلومات + sb_لیکه;

	scrollbackline_pos_دننه_buf(غړی, sb_لیکه, &lnum, عدم, &دننه_واټن);

	// vterm is finished, get the text from the buffer
	إذا (lnum > 0 && lnum <= buf->b_ml.ml_کرښه_شمېر)
	{
	    char_u *p = ml_get_buf(buf, lnum, FALSE);
	    إذا (STRLEN(p) >= دننه_واټن + کرښه->sb_بایټونه)
		rettv->vval.v_string = vim_strnsave(p + دننه_واټن, کرښه->sb_بایټونه);
	}
    }
    وإلا
    {
	VTermScreen	*screen = vterm_obtain_screen(غړی->tl_vterm);
	VTermRect	rect;
	صحيح		len;
	char_u		*p;

	إذا (لیکه < 0 || لیکه >= غړی->tl_لیکې)
	    أرجع;
	len = غړی->tl_ستون_شمېر * MB_MAXBYTES + 1;
	p = alloc(len);
	إذا (p == عدم)
	    أرجع;
	rettv->vval.v_string = p;

	rect.پیل_ستون = 0;
	rect.پای_ټکی_ستون = غړی->tl_ستون_شمېر;
	rect.پیل_لیکه = لیکه;
	rect.پای_ټکی_لیکه = لیکه + 1;
	p[vterm_screen_get_متن(screen, (حرف *)p, len, rect)] = NUL;
    }
}

/*
 * "term_getscrolled(buf)" function
 */
    فراغ
f_غړی_getscrolled(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    إذا (دننه_vim9script() && check_for_بفر_arg(argvars, 0) == FAIL)
	أرجع;

    buf = غړی_get_buf(argvars, "term_getscrolled()");
    إذا (buf == عدم)
	أرجع;
    rettv->vval.v_شمېره = buf->b_غړی->tl_بفر_scrolled;
}

/*
 * "term_getsize(buf)" function
 */
    فراغ
f_غړی_getsize(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    list_T	*l;

    إذا (rettv_list_alloc(rettv) == FAIL)
	أرجع;

    إذا (دننه_vim9script() && check_for_بفر_arg(argvars, 0) == FAIL)
	أرجع;

    buf = غړی_get_buf(argvars, "term_getsize()");
    إذا (buf == عدم)
	أرجع;

    l = rettv->vval.v_list;
    list_append_شمېره(l, buf->b_غړی->tl_لیکې);
    list_append_شمېره(l, buf->b_غړی->tl_ستون_شمېر);
}

/*
 * "term_setsize(buf, rows, cols)" function
 */
    فراغ
f_غړی_setsize(typval_T *argvars, typval_T *rettv ناکارول_شوی)
{
    buf_T	*buf;
    غړی_T	*غړی;
    varnumber_T لیکې_2, ستون_شمېر;

    إذا (دننه_vim9script()
	    && (check_for_بفر_arg(argvars, 0) == FAIL
		|| check_for_شمېره_arg(argvars, 1) == FAIL
		|| check_for_شمېره_arg(argvars, 2) == FAIL))
	أرجع;

    buf = غړی_get_buf(argvars, "term_setsize()");
    إذا (buf == عدم)
    {
	emsg(_(e_not_terminal_بفر));
	أرجع;
    }
    إذا (buf->b_غړی->tl_vterm == عدم)
	أرجع;
    غړی = buf->b_غړی;
    لیکې_2 = tv_get_شمېره(&argvars[1]);
    لیکې_2 = لیکې_2 <= 0 ? غړی->tl_لیکې : لیکې_2;
    ستون_شمېر = tv_get_شمېره(&argvars[2]);
    ستون_شمېر = ستون_شمېر <= 0 ? غړی->tl_ستون_شمېر : ستون_شمېر;
    vterm_سېټ_اندازه(غړی->tl_vterm, لیکې_2, ستون_شمېر);
    // handle_resize() will resize the windows

    // Get and remember the size we ended up with.  Update the pty.
    vterm_get_اندازه(غړی->tl_vterm, &غړی->tl_لیکې, &غړی->tl_ستون_شمېر);
    غړی_report_winsize(غړی, غړی->tl_لیکې, غړی->tl_ستون_شمېر);
}

/*
 * "term_getstatus(buf)" function
 */
    فراغ
f_غړی_getstatus(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    غړی_T	*غړی;
    char_u	val[100];

    rettv->v_type = VAR_STRING;

    إذا (دننه_vim9script() && check_for_بفر_arg(argvars, 0) == FAIL)
	أرجع;

    buf = غړی_get_buf(argvars, "term_getstatus()");
    إذا (buf == عدم)
	أرجع;
    غړی = buf->b_غړی;

    إذا (غړی_دنده_running(غړی))
	STRCPY(val, "running");
    وإلا
	STRCPY(val, "finished");
    إذا (غړی->tl_نورمال_ډېرپېښ)
	STRCAT(val, ",normal");
    rettv->vval.v_string = vim_strsave(val);
}

/*
 * "term_gettitle(buf)" function
 */
    فراغ
f_غړی_gettitle(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    rettv->v_type = VAR_STRING;

    إذا (دننه_vim9script() && check_for_بفر_arg(argvars, 0) == FAIL)
	أرجع;

    buf = غړی_get_buf(argvars, "term_gettitle()");
    إذا (buf == عدم)
	أرجع;

    إذا (buf->b_غړی->tl_title != عدم)
	rettv->vval.v_string = vim_strsave(buf->b_غړی->tl_title);
}

/*
 * "term_gettty(buf)" function
 */
    فراغ
f_غړی_gettty(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    char_u	*p = عدم;
    صحيح		عدد = 0;

    إذا (دننه_vim9script()
	    && (check_for_بفر_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	أرجع;

    rettv->v_type = VAR_STRING;
    buf = غړی_get_buf(argvars, "term_gettty()");
    إذا (buf == عدم)
	أرجع;
    إذا (argvars[1].v_type != VAR_UNKNOWN)
	عدد = tv_get_bool(&argvars[1]);

    بدل (عدد)
    {
	حالة 0:
	    إذا (buf->b_غړی->tl_دنده != عدم)
		p = buf->b_غړی->tl_دنده->jv_tty_بهر;
	    اكسر;
	حالة 1:
	    إذا (buf->b_غړی->tl_دنده != عدم)
		p = buf->b_غړی->tl_دنده->jv_tty_دننه;
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
f_غړی_list(typval_T *argvars ناکارول_شوی, typval_T *rettv)
{
    غړی_T	*tp;
    list_T	*l;

    إذا (rettv_list_alloc(rettv) == FAIL || لومړی_غړی == عدم)
	أرجع;

    l = rettv->vval.v_list;
    FOR_ALL_TERMS(tp)
	إذا (tp->tl_بفر != عدم)
	    إذا (list_append_شمېره(l,
				   (varnumber_T)tp->tl_بفر->b_fnum) == FAIL)
		أرجع;
}

/*
 * "term_scrape(buf, row)" function
 */
    فراغ
f_غړی_scrape(typval_T *argvars, typval_T *rettv)
{
    buf_T	    *buf;
    VTermScreen	    *screen = عدم;
    VTermPos	    pos;
    list_T	    *l;
    غړی_T	    *غړی;
    char_u	    *p;
    sb_کرښه_T	    *کرښه;

    إذا (rettv_list_alloc(rettv) == FAIL)
	أرجع;

    إذا (دننه_vim9script()
	    && (check_for_بفر_arg(argvars, 0) == FAIL
		|| check_for_lnum_arg(argvars, 1) == FAIL))
	أرجع;

    buf = غړی_get_buf(argvars, "term_scrape()");
    إذا (buf == عدم)
	أرجع;
    غړی = buf->b_غړی;

    l = rettv->vval.v_list;
    pos.لیکه = get_لیکه_شمېره(&argvars[1], غړی);

    إذا (غړی->tl_vterm != عدم)
    {
	screen = vterm_obtain_screen(غړی->tl_vterm);
	إذا (screen == عدم)  // can't really happen
	    أرجع;
	p = عدم;
	کرښه = عدم;
    }
    وإلا
    {
	صحيح	  sb_لیکه = غړی->tl_scrollback_scrolled + pos.لیکه;
	linenr_T  lnum = 0;
	size_t	  دننه_واټن = 0;

	scrollbackline_pos_دننه_buf(غړی, sb_لیکه, &lnum, عدم, &دننه_واټن);

	إذا (sb_لیکه >= غړی->tl_scrollback.ga_len || lnum <= 0 || lnum > buf->b_ml.ml_کرښه_شمېر)
	    أرجع;

	کرښه = (sb_کرښه_T *)غړی->tl_scrollback.ga_معلومات + sb_لیکه;
	p = ml_get_buf(buf, lnum, FALSE);

	إذا (STRLEN(p) < دننه_واټن + کرښه->sb_بایټونه)
	    أرجع;

	p += دننه_واټن;
    }

    لكل (pos.ستون = 0; pos.ستون < غړی->tl_ستون_شمېر; )
    {
	dict_T		*dcell;
	صحيح		پلنوالی;
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
	    إذا (pos.ستون >= کرښه->sb_ستون_شمېر)
		اكسر;
	    cellattr = کرښه->sb_cells + pos.ستون;
	    پلنوالی = cellattr->پلنوالی;
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
	    پلنوالی = cell.پلنوالی;
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

	dict_جمع_کړه_string_len(dcell, "chars", mbs, (صحيح)mbslen);

	rgblen = vim_snprintf_safelen((حرف *)rgb, حجم(rgb),
	    "#%02x%02x%02x", fg.red, fg.green, fg.blue);
	dict_جمع_کړه_string_len(dcell, "fg", rgb, (صحيح)rgblen);
	rgblen = vim_snprintf_safelen((حرف *)rgb, حجم(rgb),
	    "#%02x%02x%02x", bg.red, bg.green, bg.blue);
	dict_جمع_کړه_string_len(dcell, "bg", rgb, (صحيح)rgblen);

	dict_جمع_کړه_شمېره(dcell, "attr",
				      cell2attr(غړی, عدم, &attrs, &fg, &bg));
	dict_جمع_کړه_شمېره(dcell, "width", پلنوالی);

	++pos.ستون;
	إذا (پلنوالی == 2)
	    ++pos.ستون;
    }
}

/*
 * "term_sendkeys(buf, keys)" function
 */
    فراغ
f_غړی_sendkeys(typval_T *argvars, typval_T *rettv ناکارول_شوی)
{
    buf_T	*buf;
    char_u	*پیغام;
    غړی_T	*غړی;

    إذا (دننه_vim9script()
	    && (check_for_بفر_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	أرجع;

    buf = غړی_get_buf(argvars, "term_sendkeys()");
    إذا (buf == عدم)
	أرجع;

    پیغام = tv_get_string_chk(&argvars[1]);
    إذا (پیغام == عدم)
	أرجع;
    غړی = buf->b_غړی;
    إذا (غړی->tl_vterm == عدم)
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
	send_کلیګانې_منزل_غړی(غړی, c, 0, FALSE);
    }
}

#إذا defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * "term_getansicolors(buf)" function
 */
    فراغ
f_غړی_getansicolors(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    غړی_T	*غړی;
    VTermState	*state;
    VTermColor  color;
    char_u	hexbuf[10];
    صحيح		شاخص;
    list_T	*list;

    إذا (rettv_list_alloc(rettv) == FAIL)
	أرجع;

    إذا (دننه_vim9script() && check_for_بفر_arg(argvars, 0) == FAIL)
	أرجع;

    buf = غړی_get_buf(argvars, "term_getansicolors()");
    إذا (buf == عدم)
	أرجع;
    غړی = buf->b_غړی;
    إذا (غړی->tl_vterm == عدم)
	أرجع;

    list = rettv->vval.v_list;
    state = vterm_obtain_state(غړی->tl_vterm);
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
f_غړی_setansicolors(typval_T *argvars, typval_T *rettv ناکارول_شوی)
{
    buf_T	*buf;
    غړی_T	*غړی;
    listitem_T	*li;
    صحيح		n = 0;

    إذا (دننه_vim9script()
	    && (check_for_بفر_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL))
	أرجع;

    buf = غړی_get_buf(argvars, "term_setansicolors()");
    إذا (buf == عدم)
	أرجع;
    غړی = buf->b_غړی;
    إذا (غړی->tl_vterm == عدم)
	أرجع;

    إذا (check_for_nonnull_list_arg(argvars, 1) == FAIL)
	أرجع;

    إذا (argvars[1].vval.v_list->lv_لومړی == &range_list_توکی
	    || argvars[1].vval.v_list->lv_len != 16)
    {
	semsg(_(e_invalid_ارزښت_for_دلیل_str), "\"colors\"");
	أرجع;
    }

    إذا (غړی->tl_palette == عدم)
	غړی->tl_palette = ALLOC_MULT(long_u, 16);
    إذا (غړی->tl_palette == عدم)
	أرجع;

    FOR_ALL_LIST_توکي(argvars[1].vval.v_list, li)
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

	غړی->tl_palette[n++] = GUI_MCH_GET_RGB(guicolor);
    }

    غړی_update_palette(غړی);
}
#د_که_پای

/*
 * "term_setapi(buf, api)" function
 */
    فراغ
f_غړی_setapi(typval_T *argvars, typval_T *rettv ناکارول_شوی)
{
    buf_T	*buf;
    غړی_T	*غړی;
    char_u	*api;

    إذا (دننه_vim9script()
	    && (check_for_بفر_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	أرجع;

    buf = غړی_get_buf(argvars, "term_setapi()");
    إذا (buf == عدم)
	أرجع;
    غړی = buf->b_غړی;
    vim_free(غړی->tl_api);
    api = tv_get_string_chk(&argvars[1]);
    إذا (api != عدم)
	غړی->tl_api = vim_strsave(api);
    وإلا
	غړی->tl_api = عدم;
}

/*
 * "term_setrestore(buf, command)" function
 */
    فراغ
f_غړی_setrestore(typval_T *argvars ناکارول_شوی, typval_T *rettv ناکارول_شوی)
{
#إذا defined(FEAT_SESSION)
    buf_T	*buf;
    غړی_T	*غړی;
    char_u	*cmd;

    إذا (دننه_vim9script()
	    && (check_for_بفر_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	أرجع;

    buf = غړی_get_buf(argvars, "term_setrestore()");
    إذا (buf == عدم)
	أرجع;
    غړی = buf->b_غړی;
    vim_free(غړی->tl_command);
    cmd = tv_get_string_chk(&argvars[1]);
    إذا (cmd != عدم)
	غړی->tl_command = vim_strsave(cmd);
    وإلا
	غړی->tl_command = عدم;
#د_که_پای
}

/*
 * "term_setkill(buf, how)" function
 */
    فراغ
f_غړی_setkill(typval_T *argvars ناکارول_شوی, typval_T *rettv ناکارول_شوی)
{
    buf_T	*buf;
    غړی_T	*غړی;
    char_u	*how;

    إذا (دننه_vim9script()
	    && (check_for_بفر_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	أرجع;

    buf = غړی_get_buf(argvars, "term_setkill()");
    إذا (buf == عدم)
	أرجع;
    غړی = buf->b_غړی;
    vim_free(غړی->tl_kill);
    how = tv_get_string_chk(&argvars[1]);
    إذا (how != عدم)
	غړی->tl_kill = vim_strsave(how);
    وإلا
	غړی->tl_kill = عدم;
}

/*
 * "term_start(command, options)" function
 */
    فراغ
f_غړی_پیل(typval_T *argvars, typval_T *rettv)
{
    jobopt_T	opt;
    buf_T	*buf;

    إذا (دننه_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	أرجع;

    init_دنده_options(&opt);
    إذا (argvars[1].v_type != VAR_UNKNOWN
	    && get_دنده_options(&argvars[1], &opt,
		JO_TIMEOUT_ALL + JO_STOPONEXIT
		    + JO_CALLBACK + JO_بهر_CALLBACK + JO_ERR_CALLBACK
		    + JO_EXIT_CB + JO_CLOSE_CALLBACK + JO_بهر_IO,
		JO2_غړی_NAME + JO2_غړی_پای + JO2_HIDDEN + JO2_غړی_OPENCMD
		    + JO2_غړی_ستون_شمېر + JO2_غړی_لیکې + JO2_VERTICAL + JO2_CURWIN
		    + JO2_CWD + JO2_ENV + JO2_EOF_CHARS
		    + JO2_NORESTORE + JO2_غړی_KILL + JO2_غړی_HIGHLIGHT
		    + JO2_ANSI_COLORS + JO2_TTY_TYPE + JO2_غړی_API) == FAIL)
	أرجع;

    buf = غړی_پیل(&argvars[0], عدم, &opt, 0);

    إذا (buf != عدم && buf->b_غړی != عدم)
	rettv->vval.v_شمېره = buf->b_fnum;
}

/*
 * "term_wait" function
 */
    فراغ
f_غړی_wait(typval_T *argvars, typval_T *rettv ناکارول_شوی)
{
    buf_T	*buf;

    إذا (دننه_vim9script()
	    && (check_for_بفر_arg(argvars, 0) == FAIL
		|| check_for_opt_شمېره_arg(argvars, 1) == FAIL))
	أرجع;

    buf = غړی_get_buf(argvars, "term_wait()");
    إذا (buf == عدم)
	أرجع;
    إذا (buf->b_غړی->tl_دنده == عدم)
    {
	ch_log(عدم, "term_wait(): no job to wait for");
	أرجع;
    }
    إذا (buf->b_غړی->tl_دنده->jv_channel == عدم)
	// channel is closed, nothing to do
	أرجع;

    // Get the job status, this will detect a job that finished.
    إذا (!buf->b_غړی->tl_دنده->jv_channel->ch_keep_open
	    && STRCMP(دنده_حالت(buf->b_غړی->tl_دنده), "dead") == 0)
    {
	// The job is dead, keep reading channel I/O until the channel is
	// closed. buf->b_term may become NULL if the terminal was closed while
	// waiting.
	ch_log(عدم, "term_wait(): waiting for channel to close");
	طالما (buf->b_غړی != عدم && !buf->b_غړی->tl_channel_closed)
	{
	    غړی_flush_messages();

	    ui_delay(10L, FALSE);
	    إذا (!buf_valid(buf))
		// If the terminal is closed when the channel is closed the
		// buffer disappears.
		اكسر;
	    إذا (buf->b_غړی == عدم || buf->b_غړی->tl_channel_closing)
		// came here from a close callback, only wait one time
		اكسر;
	}

	غړی_flush_messages();
    }
    وإلا
    {
	اوږد wait = 10L;

	غړی_flush_messages();

	// Wait for some time for any channel I/O.
	إذا (argvars[1].v_type != VAR_UNKNOWN)
	    wait = tv_get_شمېره(&argvars[1]);
	ui_delay(wait, TRUE);

	// Flushing messages on channels is hopefully sufficient.
	// TODO: is there a better way?
	غړی_flush_messages();
    }
}

/*
 * Called when a channel has sent all the lines to a terminal.
 * Send a CTRL-D to mark the end of the text.
 */
    فراغ
غړی_send_eof(channel_T *ch)
{
    غړی_T	*غړی;

    FOR_ALL_TERMS(غړی)
	إذا (غړی->tl_دنده == ch->ch_دنده)
	{
	    إذا (غړی->tl_eof_chars != عدم)
	    {
		channel_send(ch, PART_دننه, غړی->tl_eof_chars,
					(صحيح)STRLEN(غړی->tl_eof_chars), عدم);
		channel_send(ch, PART_دننه, (char_u *)"\r", 1, عدم);
	    }
#که_تعریف_شوی MSWIN
	    وإلا
		// Default: CTRL-D
		channel_send(ch, PART_دننه, (char_u *)"\004\r", 2, عدم);
#د_که_پای
	}
}

#إذا defined(FEAT_GUI)
    دنده_T *
غړی_getjob(غړی_T *غړی)
{
    أرجع غړی != عدم ? غړی->tl_دنده : عدم;
}
#د_که_پای

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
    } conpty_ثبت[] =
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
	    emsg(_(e_conpty_is_not_شته));
	أرجع FAIL;
    }

    // No need to initialize twice.
    إذا (hKerneldll)
	أرجع OK;

    hKerneldll = vim_بارول_Lib("kernel32.dll");
    لكل (i = 0; conpty_ثبت[i].name != عدم
					&& conpty_ثبت[i].ptr != عدم; ++i)
    {
	إذا ((*conpty_ثبت[i].ptr = (FARPROC)Get_Proc_پته(hKerneldll,
						conpty_ثبت[i].name)) == عدم)
	{
	    إذا (verbose)
		semsg(_(e_could_not_بارول_library_function_str), conpty_ثبت[i].name);
	    hKerneldll = عدم;
	    أرجع FAIL;
	}
    }

    أرجع OK;
}

    ساكن صحيح
conpty_غړی_and_دنده_init(
	غړی_T	    *غړی,
	typval_T    *argvar,
	حرف	    **argv ناکارول_شوی,
	jobopt_T    *opt,
	jobopt_T    *orig_opt)
{
    WCHAR	    *cmd_wchar = عدم;
    WCHAR	    *cmd_wchar_copy = عدم;
    WCHAR	    *cwd_wchar = عدم;
    WCHAR	    *env_wchar = عدم;
    channel_T	    *channel = عدم;
    دنده_T	    *دنده = عدم;
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
	cmd = ga_cmd.ga_معلومات;
    }
    إذا (cmd == عدم || *cmd == NUL)
    {
	emsg(_(e_invalid_دلیل));
	اذهب failed;
    }

    غړی->tl_arg0_cmd = vim_strsave(cmd);

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

    consize.X = غړی->tl_ستون_شمېر;
    consize.Y = غړی->tl_لیکې;
    hr = pCreatePseudoConsole(consize, i_theirs, o_theirs, 0,
							     &غړی->tl_conpty);
    إذا (FAILED(hr))
	اذهب failed;

    غړی->tl_siex.StartupInfo.cb = حجم(غړی->tl_siex);

    // Explicitly invalidate std handles to prevent inheritance of
    // the debugger's stdout (e.g., in Visual Studio debugging sessions),
    // which could cause job output to go to the debugger instead of
    // the intended ConPTY, even with bInheritHandles set to FALSE in CreateProcess.
    غړی->tl_siex.StartupInfo.dwFlags = STARTF_USESTDHANDLES;
    غړی->tl_siex.StartupInfo.h_Std_ننوت = INVALID_HANDLE_ارزښت;
    غړی->tl_siex.StartupInfo.hStdOutput = INVALID_HANDLE_ارزښت;
    غړی->tl_siex.StartupInfo.hStdError = INVALID_HANDLE_ارزښت;

    // Set up pipe inheritance safely: Vista or later.
    pInitializeProcThreadAttributeList(عدم, 1, 0, &breq);
    غړی->tl_siex.lpAttributeList = alloc(breq);
    إذا (!غړی->tl_siex.lpAttributeList)
	اذهب failed;
    إذا (!pInitializeProcThreadAttributeList(غړی->tl_siex.lpAttributeList, 1,
								     0, &breq))
	اذهب failed;
    إذا (!pUpdateProcThreadAttribute(
	    غړی->tl_siex.lpAttributeList, 0,
	    PROC_THREAD_ATTRIBUTE_PSEUDOCONSOLE, غړی->tl_conpty,
	    حجم(HPCON), عدم, عدم))
	اذهب failed;

    channel = جمع_کړه_channel();
    إذا (channel == عدم)
	اذهب failed;

    دنده = دنده_alloc();
    إذا (دنده == عدم)
	اذهب failed;
    إذا (argvar->v_type == VAR_STRING)
    {
	صحيح argc;

	build_argv_from_string(cmd, &دنده->jv_argv, &argc);
    }
    وإلا
    {
	صحيح argc;

	build_argv_from_list(argvar->vval.v_list, &دنده->jv_argv, &argc);
    }

    إذا (opt->jo_سېټ & JO_دننه_BUF)
	دنده->jv_دننه_buf = buflist_findnr(opt->jo_io_buf[PART_دننه]);

    إذا (!CreateProcessW(عدم, cmd_wchar_copy, عدم, عدم, FALSE,
	    EXTENDED_STARTUPINFO_PRESENT | CREATE_UNICODE_ENVIRONMENT
	    | CREATE_SUSPENDED | CREATE_DEFAULT_ERROR_ډېرپېښ,
	    env_wchar, cwd_wchar,
	    &غړی->tl_siex.StartupInfo, &proc_info))
    {
	errmsg = GetWin32Error();
	اذهب failed;
    }

    CloseHandle(i_theirs);
    CloseHandle(o_theirs);

    channel_سېټ_pipes(channel,
	    (sock_T)i_ours,
	    (sock_T)o_ours,
	    (sock_T)o_ours);

    // Write lines with CR instead of NL.
    channel->ch_write_متن_ډېرپېښ = TRUE;

    // Use to explicitly delete anonymous pipe handle.
    // In addition, it is used to prevent the pipe from being closed when input
    // from a buffer etc. is finished.
    channel->ch_anonymous_pipe = TRUE;

    jo = Create_دنده_Object(عدم, عدم);
    إذا (jo == عدم)
	اذهب failed;

    إذا (!ټاکل_Process_منزل_دنده_Object(jo, proc_info.hProcess))
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

    إذا (create_vterm(غړی, غړی->tl_لیکې, غړی->tl_ستون_شمېر) == FAIL)
	اذهب failed;

# إذا defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    إذا (غړی_use_palette())
    {
	إذا (غړی->tl_palette != عدم)
	    سېټ_vterm_palette(غړی->tl_vterm, غړی->tl_palette);
	وإلا
	    init_vterm_ansi_colors(غړی->tl_vterm);
    }
# endif

    channel_سېټ_دنده(channel, دنده, opt);
    دنده_سېټ_options(دنده, opt);

    دنده->jv_channel = channel;
    دنده->jv_proc_info = proc_info;
    دنده->jv_دنده_object = jo;
    دنده->jv_حالت = دنده_STARTED;
    دنده->jv_tty_type = vim_strsave((char_u *)"conpty");
    ++دنده->jv_refcount;
    غړی->tl_دنده = دنده;

    // Redirecting stdout and stderr doesn't work at the job level.  Instead
    // open the file here and handle it in.  opt->jo_io was changed in
    // setup_job_options(), use the original flags here.
    إذا (orig_opt->jo_io[PART_بهر] == JIO_FILE)
    {
	char_u *fname = opt->jo_io_name[PART_بهر];

	ch_log(channel, "Opening output file %s", fname);
	غړی->tl_بهر_fd = mch_fopen((حرف *)fname, WRITEBIN);
	إذا (غړی->tl_بهر_fd == عدم)
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
    إذا (دنده != عدم)
    {
	دنده->jv_channel = عدم;
	دنده_cleanup(دنده);
    }
    غړی->tl_دنده = عدم;
    إذا (jo != عدم)
	CloseHandle(jo);

    إذا (غړی->tl_siex.lpAttributeList != عدم)
    {
	pDeleteProcThreadAttributeList(غړی->tl_siex.lpAttributeList);
	vim_free(غړی->tl_siex.lpAttributeList);
    }
    غړی->tl_siex.lpAttributeList = عدم;
    إذا (o_theirs != عدم)
	CloseHandle(o_theirs);
    إذا (o_ours != عدم)
	CloseHandle(o_ours);
    إذا (i_ours != عدم)
	CloseHandle(i_ours);
    إذا (i_theirs != عدم)
	CloseHandle(i_theirs);
    إذا (غړی->tl_conpty != عدم)
	pClosePseudoConsole(غړی->tl_conpty);
    غړی->tl_conpty = عدم;
    // Propagate errors that occur in CreateProcess
    إذا (errmsg)
	semsg("CreateProcess failed: %s", errmsg);
    أرجع FAIL;
}

    ساكن فراغ
conpty_غړی_report_winsize(غړی_T *غړی, صحيح لیکې_2, صحيح ستون_شمېر)
{
    COORD consize;

    consize.X = ستون_شمېر;
    consize.Y = لیکې_2;
    pResizePseudoConsole(غړی->tl_conpty, consize);
}

    ساكن فراغ
غړی_free_conpty(غړی_T *غړی)
{
    إذا (غړی->tl_siex.lpAttributeList != عدم)
    {
	pDeleteProcThreadAttributeList(غړی->tl_siex.lpAttributeList);
	vim_free(غړی->tl_siex.lpAttributeList);
    }
    غړی->tl_siex.lpAttributeList = عدم;
    إذا (غړی->tl_conpty != عدم)
	pClosePseudoConsole(غړی->tl_conpty);
    غړی->tl_conpty = عدم;
}

    صحيح
use_conpty(فراغ)
{
    أرجع has_conpty;
}

# define WINPTY_SPAWN_FLAG_AUTO_SHUTDOWN 1ul
# define WINPTY_SPAWN_FLAG_EXIT_AFTER_SHUTDOWN 2ull
# define WINPTY_MOUSE_ډېرپېښ_FORCE		2

فراغ* (*winpty_config_new)(UINT64, فراغ*);
فراغ* (*winpty_open)(فراغ*, فراغ*);
فراغ* (*winpty_spawn_config_new)(UINT64, فراغ*, LPCWSTR, فراغ*, فراغ*, فراغ*);
BOOL (*winpty_spawn)(فراغ*, فراغ*, HANDLE*, HANDLE*, DWORD*, فراغ*);
فراغ (*winpty_config_سېټ_mouse_ډېرپېښ)(فراغ*, صحيح);
فراغ (*winpty_config_سېټ_initial_اندازه)(فراغ*, صحيح, صحيح);
LPCWSTR (*winpty_conin_name)(فراغ*);
LPCWSTR (*winpty_conout_name)(فراغ*);
LPCWSTR (*winpty_conerr_name)(فراغ*);
فراغ (*winpty_free)(فراغ*);
فراغ (*winpty_config_free)(فراغ*);
فراغ (*winpty_spawn_config_free)(فراغ*);
فراغ (*winpty_error_free)(فراغ*);
LPCWSTR (*winpty_error_پیغام)(فراغ*);
BOOL (*winpty_سېټ_اندازه)(فراغ*, صحيح, صحيح, فراغ*);
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
    } winpty_ثبت[] =
    {
	{"winpty_conerr_name", (FARPROC*)&winpty_conerr_name},
	{"winpty_config_free", (FARPROC*)&winpty_config_free},
	{"winpty_config_new", (FARPROC*)&winpty_config_new},
	{"winpty_config_set_mouse_mode",
				      (FARPROC*)&winpty_config_سېټ_mouse_ډېرپېښ},
	{"winpty_config_set_initial_size",
				    (FARPROC*)&winpty_config_سېټ_initial_اندازه},
	{"winpty_conin_name", (FARPROC*)&winpty_conin_name},
	{"winpty_conout_name", (FARPROC*)&winpty_conout_name},
	{"winpty_error_free", (FARPROC*)&winpty_error_free},
	{"winpty_free", (FARPROC*)&winpty_free},
	{"winpty_open", (FARPROC*)&winpty_open},
	{"winpty_spawn", (FARPROC*)&winpty_spawn},
	{"winpty_spawn_config_free", (FARPROC*)&winpty_spawn_config_free},
	{"winpty_spawn_config_new", (FARPROC*)&winpty_spawn_config_new},
	{"winpty_error_msg", (FARPROC*)&winpty_error_پیغام},
	{"winpty_set_size", (FARPROC*)&winpty_سېټ_اندازه},
	{"winpty_agent_process", (FARPROC*)&winpty_agent_process},
	{عدم, عدم}
    };

    // No need to initialize twice.
    إذا (hWinPtyDLL)
	أرجع OK;
    // Load winpty.dll, prefer using the 'winptydll' option, fall back to just
    // winpty.dll.
    إذا (*p_winptydll != NUL)
	hWinPtyDLL = vim_بارول_Lib((حرف *)p_winptydll);
    إذا (!hWinPtyDLL)
	hWinPtyDLL = vim_بارول_Lib(WINPTY_DLL);
    إذا (!hWinPtyDLL)
    {
	إذا (verbose)
	    semsg(_(e_could_not_بارول_library_str_str),
		    (*p_winptydll != NUL ? p_winptydll : (char_u *)WINPTY_DLL),
		    GetWin32Error());
	أرجع FAIL;
    }
    لكل (i = 0; winpty_ثبت[i].name != عدم
					 && winpty_ثبت[i].ptr != عدم; ++i)
    {
	إذا ((*winpty_ثبت[i].ptr = (FARPROC)Get_Proc_پته(hWinPtyDLL,
					      winpty_ثبت[i].name)) == عدم)
	{
	    إذا (verbose)
		semsg(_(e_could_not_بارول_library_function_str), winpty_ثبت[i].name);
	    hWinPtyDLL = عدم;
	    أرجع FAIL;
	}
    }

    أرجع OK;
}

    ساكن صحيح
winpty_غړی_and_دنده_init(
	غړی_T	    *غړی,
	typval_T    *argvar,
	حرف	    **argv ناکارول_شوی,
	jobopt_T    *opt,
	jobopt_T    *orig_opt)
{
    WCHAR	    *cmd_wchar = عدم;
    WCHAR	    *cwd_wchar = عدم;
    WCHAR	    *env_wchar = عدم;
    channel_T	    *channel = عدم;
    دنده_T	    *دنده = عدم;
    DWORD	    error;
    HANDLE	    jo = عدم;
    HANDLE	    ماشوم_process_handle;
    HANDLE	    ماشوم_thread_handle;
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
	emsg(_(e_invalid_دلیل));
	اذهب failed;
    }

    غړی->tl_arg0_cmd = vim_strsave(cmd);

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

    غړی->tl_winpty_config = winpty_config_new(0, &winpty_err);
    إذا (غړی->tl_winpty_config == عدم)
	اذهب failed;

    winpty_config_سېټ_mouse_ډېرپېښ(غړی->tl_winpty_config,
						    WINPTY_MOUSE_ډېرپېښ_FORCE);
    winpty_config_سېټ_initial_اندازه(غړی->tl_winpty_config,
						 غړی->tl_ستون_شمېر, غړی->tl_لیکې);
    غړی->tl_winpty = winpty_open(غړی->tl_winpty_config, &winpty_err);
    إذا (غړی->tl_winpty == عدم)
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

    channel = جمع_کړه_channel();
    إذا (channel == عدم)
	اذهب failed;

    دنده = دنده_alloc();
    إذا (دنده == عدم)
	اذهب failed;
    إذا (argvar->v_type == VAR_STRING)
    {
	صحيح argc;

	build_argv_from_string(cmd, &دنده->jv_argv, &argc);
    }
    وإلا
    {
	صحيح argc;

	build_argv_from_list(argvar->vval.v_list, &دنده->jv_argv, &argc);
    }

    إذا (opt->jo_سېټ & JO_دننه_BUF)
	دنده->jv_دننه_buf = buflist_findnr(opt->jo_io_buf[PART_دننه]);

    إذا (!winpty_spawn(غړی->tl_winpty, spawn_config, &ماشوم_process_handle,
	    &ماشوم_thread_handle, &error, &winpty_err))
	اذهب failed;

    channel_سېټ_pipes(channel,
	(sock_T)CreateFileW(
	    winpty_conin_name(غړی->tl_winpty),
	    GENERIC_WRITE, 0, عدم,
	    OPEN_EXISTING, 0, عدم),
	(sock_T)CreateFileW(
	    winpty_conout_name(غړی->tl_winpty),
	    GENERIC_READ, 0, عدم,
	    OPEN_EXISTING, 0, عدم),
	(sock_T)CreateFileW(
	    winpty_conerr_name(غړی->tl_winpty),
	    GENERIC_READ, 0, عدم,
	    OPEN_EXISTING, 0, عدم));

    // Write lines with CR instead of NL.
    channel->ch_write_متن_ډېرپېښ = TRUE;

    jo = Create_دنده_Object(عدم, عدم);
    إذا (jo == عدم)
	اذهب failed;

    إذا (!ټاکل_Process_منزل_دنده_Object(jo, ماشوم_process_handle))
    {
	// Failed, switch the way to terminate process with TerminateProcess.
	CloseHandle(jo);
	jo = عدم;
    }

    winpty_spawn_config_free(spawn_config);
    vim_free(cmd_wchar);
    vim_free(cwd_wchar);
    vim_free(env_wchar);

    إذا (create_vterm(غړی, غړی->tl_لیکې, غړی->tl_ستون_شمېر) == FAIL)
	اذهب failed;

# إذا defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    إذا (غړی_use_palette())
    {
	إذا (غړی->tl_palette != عدم)
	    سېټ_vterm_palette(غړی->tl_vterm, غړی->tl_palette);
	وإلا
	    init_vterm_ansi_colors(غړی->tl_vterm);
    }
# endif

    channel_سېټ_دنده(channel, دنده, opt);
    دنده_سېټ_options(دنده, opt);

    دنده->jv_channel = channel;
    دنده->jv_proc_info.hProcess = ماشوم_process_handle;
    دنده->jv_proc_info.dwProcessId = GetProcessId(ماشوم_process_handle);
    دنده->jv_دنده_object = jo;
    دنده->jv_حالت = دنده_STARTED;
    دنده->jv_tty_دننه = utf16_منزل_enc(
	    (short_u *)winpty_conin_name(غړی->tl_winpty), عدم);
    دنده->jv_tty_بهر = utf16_منزل_enc(
	    (short_u *)winpty_conout_name(غړی->tl_winpty), عدم);
    دنده->jv_tty_type = vim_strsave((char_u *)"winpty");
    ++دنده->jv_refcount;
    غړی->tl_دنده = دنده;

    // Redirecting stdout and stderr doesn't work at the job level.  Instead
    // open the file here and handle it in.  opt->jo_io was changed in
    // setup_job_options(), use the original flags here.
    إذا (orig_opt->jo_io[PART_بهر] == JIO_FILE)
    {
	char_u *fname = opt->jo_io_name[PART_بهر];

	ch_log(channel, "Opening output file %s", fname);
	غړی->tl_بهر_fd = mch_fopen((حرف *)fname, WRITEBIN);
	إذا (غړی->tl_بهر_fd == عدم)
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
    إذا (دنده != عدم)
    {
	دنده->jv_channel = عدم;
	دنده_cleanup(دنده);
    }
    غړی->tl_دنده = عدم;
    إذا (jo != عدم)
	CloseHandle(jo);
    إذا (غړی->tl_winpty != عدم)
	winpty_free(غړی->tl_winpty);
    غړی->tl_winpty = عدم;
    إذا (غړی->tl_winpty_config != عدم)
	winpty_config_free(غړی->tl_winpty_config);
    غړی->tl_winpty_config = عدم;
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
غړی_and_دنده_init(
	غړی_T	    *غړی,
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
	أرجع conpty_غړی_and_دنده_init(غړی, argvar, argv, opt, orig_opt);

    إذا (use_winpty)
	أرجع winpty_غړی_and_دنده_init(غړی, argvar, argv, opt, orig_opt);

    // error
    أرجع dyn_winpty_init(TRUE);
}

    ساكن صحيح
create_pty_only(غړی_T *غړی, jobopt_T *options)
{
    HANDLE	    h_Pipe_دننه = INVALID_HANDLE_ارزښت;
    HANDLE	    h_Pipe_بهر = INVALID_HANDLE_ارزښت;
    حرف	    دننه_name[80], بهر_name[80];
    channel_T	    *channel = عدم;

    إذا (create_vterm(غړی, غړی->tl_لیکې, غړی->tl_ستون_شمېر) == FAIL)
	أرجع FAIL;

    vim_snprintf(دننه_name, حجم(دننه_name), "\\\\.\\pipe\\vim-%d-in-%d",
	    GetCurrentProcessId(),
	    curbuf->b_fnum);
    h_Pipe_دننه = CreateNamedPipe(دننه_name, PIPE_ACCESS_OUTBOUND,
	    PIPE_TYPE_پیغام | PIPE_NOWAIT,
	    PIPE_UNLIMITED_INSTANCES,
	    0, 0, NMPWAIT_NOWAIT, عدم);
    إذا (h_Pipe_دننه == INVALID_HANDLE_ارزښت)
	اذهب failed;

    vim_snprintf(بهر_name, حجم(بهر_name), "\\\\.\\pipe\\vim-%d-out-%d",
	    GetCurrentProcessId(),
	    curbuf->b_fnum);
    h_Pipe_بهر = CreateNamedPipe(بهر_name, PIPE_ACCESS_INBOUND,
	    PIPE_TYPE_پیغام | PIPE_NOWAIT,
	    PIPE_UNLIMITED_INSTANCES,
	    0, 0, 0, عدم);
    إذا (h_Pipe_بهر == INVALID_HANDLE_ارزښت)
	اذهب failed;

    ConnectNamedPipe(h_Pipe_دننه, عدم);
    ConnectNamedPipe(h_Pipe_بهر, عدم);

    غړی->tl_دنده = دنده_alloc();
    إذا (غړی->tl_دنده == عدم)
	اذهب failed;
    ++غړی->tl_دنده->jv_refcount;

    // behave like the job is already finished
    غړی->tl_دنده->jv_حالت = دنده_پای_ته_رسېدلی;

    channel = جمع_کړه_channel();
    إذا (channel == عدم)
	اذهب failed;
    غړی->tl_دنده->jv_channel = channel;
    channel->ch_keep_open = TRUE;
    channel->ch_named_pipe = TRUE;

    channel_سېټ_pipes(channel,
	(sock_T)h_Pipe_دننه,
	(sock_T)h_Pipe_بهر,
	(sock_T)h_Pipe_بهر);
    channel_سېټ_دنده(channel, غړی->tl_دنده, options);
    غړی->tl_دنده->jv_tty_دننه = vim_strsave((char_u*)دننه_name);
    غړی->tl_دنده->jv_tty_بهر = vim_strsave((char_u*)بهر_name);

    أرجع OK;

failed:
    إذا (h_Pipe_دننه != عدم)
	CloseHandle(h_Pipe_دننه);
    إذا (h_Pipe_بهر != عدم)
	CloseHandle(h_Pipe_بهر);
    أرجع FAIL;
}

/*
 * Free the terminal emulator part of "term".
 */
    ساكن فراغ
غړی_free_vterm(غړی_T *غړی)
{
    غړی_free_conpty(غړی);
    إذا (غړی->tl_winpty != عدم)
	winpty_free(غړی->tl_winpty);
    غړی->tl_winpty = عدم;
    إذا (غړی->tl_winpty_config != عدم)
	winpty_config_free(غړی->tl_winpty_config);
    غړی->tl_winpty_config = عدم;
    إذا (غړی->tl_vterm != عدم)
	vterm_free(غړی->tl_vterm);
    غړی->tl_vterm = عدم;
}

/*
 * Report the size to the terminal.
 */
    ساكن فراغ
غړی_report_winsize(غړی_T *غړی, صحيح لیکې_2, صحيح ستون_شمېر)
{
    إذا (غړی->tl_conpty)
	conpty_غړی_report_winsize(غړی, لیکې_2, ستون_شمېر);
    إذا (غړی->tl_winpty)
	winpty_سېټ_اندازه(غړی->tl_winpty, ستون_شمېر, لیکې_2, عدم);
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
غړی_and_دنده_init(
	غړی_T	    *غړی,
	typval_T    *argvar,
	حرف	    **argv,
	jobopt_T    *opt,
	jobopt_T    *orig_opt ناکارول_شوی)
{
    غړی->tl_arg0_cmd = عدم;

    إذا (create_vterm(غړی, غړی->tl_لیکې, غړی->tl_ستون_شمېر) == FAIL)
	أرجع FAIL;

# إذا defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    إذا (غړی_use_palette())
    {
	إذا (غړی->tl_palette != عدم)
	    سېټ_vterm_palette(غړی->tl_vterm, غړی->tl_palette);
	وإلا
	    init_vterm_ansi_colors(غړی->tl_vterm);
    }
# endif

    // This may change a string in "argvar".
    غړی->tl_دنده = دنده_پیل(argvar, argv, opt, &غړی->tl_دنده);
    إذا (غړی->tl_دنده != عدم)
	++غړی->tl_دنده->jv_refcount;

    أرجع غړی->tl_دنده != عدم
	&& غړی->tl_دنده->jv_channel != عدم
	&& غړی->tl_دنده->jv_حالت != دنده_FAILED ? OK : FAIL;
}

    ساكن صحيح
create_pty_only(غړی_T *غړی, jobopt_T *opt)
{
    إذا (create_vterm(غړی, غړی->tl_لیکې, غړی->tl_ستون_شمېر) == FAIL)
	أرجع FAIL;

    غړی->tl_دنده = دنده_alloc();
    إذا (غړی->tl_دنده == عدم)
	أرجع FAIL;
    ++غړی->tl_دنده->jv_refcount;

    // behave like the job is already finished
    غړی->tl_دنده->jv_حالت = دنده_پای_ته_رسېدلی;

    أرجع mch_create_pty_channel(غړی->tl_دنده, opt);
}

/*
 * Free the terminal emulator part of "term".
 */
    ساكن فراغ
غړی_free_vterm(غړی_T *غړی)
{
    إذا (غړی->tl_vterm != عدم)
	vterm_free(غړی->tl_vterm);
    غړی->tl_vterm = عدم;
}

/*
 * Report the size to the terminal.
 */
    ساكن فراغ
غړی_report_winsize(غړی_T *غړی, صحيح لیکې_2, صحيح ستون_شمېر)
{
    // Use an ioctl() to report the new window size to the job.
    إذا (غړی->tl_دنده == عدم || غړی->tl_دنده->jv_channel == عدم)
	أرجع;

    صحيح fd = -1;
    صحيح part;

    لكل (part = PART_بهر; part < PART_شمېر; ++part)
    {
	fd = غړی->tl_دنده->jv_channel->ch_part[part].ch_fd;
	إذا (mch_isatty(fd))
	    اكسر;
    }
    إذا (part < PART_شمېر && mch_report_winsize(fd, لیکې_2, ستون_شمېر) == OK)
	mch_signal_دنده(غړی->tl_دنده, (char_u *)"winch");
}

#د_که_پای

#د_که_پای // FEAT_TERMINAL
