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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਜੇ defined(FEAT_TERMINAL)

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ MIN
# define MIN(x,y) ((x) < (y) ? (x) : (y))
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ MAX
# define MAX(x,y) ((x) > (y) ? (x) : (y))
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "libvterm/include/vterm.h"

// This is VTermScreenCell without the characters, thus much smaller.
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    VTermScreenCellAttrs	attrs;
    ਅੱਖਰ			ਚੌੜਾਈ;
    VTermColor			fg;
    VTermColor			bg;
} cellattr_T;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ sb_ਸਤਰ_S {
    ਪੂਰਨ		sb_ਖਾਨੇ_ਗਿਣਤੀ;	// can differ per line
    ਪੂਰਨ		sb_ਬਾਈਟਾਂ;	// length in bytes of text
    cellattr_T	*sb_cells;	// allocated
    cellattr_T	sb_fill_attr;	// for short line
    char_u	*sb_ਪਾਠ;	// for tl_scrollback_postponed
    char_u	continuation;
} sb_ਸਤਰ_T;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
# ifndef HPCON
#  define HPCON VOID*
# endif
# ifndef EXTENDED_STARTUPINFO_PRESENT
#  define EXTENDED_STARTUPINFO_PRESENT 0x00080000
# endif
# ifndef PROC_THREAD_ATTRIBUTE_PSEUDOCONSOLE
#  define PROC_THREAD_ATTRIBUTE_PSEUDOCONSOLE 0x00020016
# endif
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ _DYN_STARTUPINFOEXW
{
    STARTUPINFOW StartupInfo;
    LPPROC_THREAD_ATTRIBUTE_LIST lpAttributeList;
} DYN_STARTUPINFOEXW, *PDYN_STARTUPINFOEXW;
#ਜੇ_ਅੰਤ

// typedef term_T in structs.h
ਬਣਤਰ terminal_S {
    ਪਦ_T	*tl_ਅਗਲਾ;

    VTerm	*tl_vterm;
    ਕੰਮ_T	*tl_ਕੰਮ;
    buf_T	*tl_ਬਫਰ;
#ਜੇ defined(FEAT_GUI)
    ਪੂਰਨ		tl_system;	// when non-zero used for :!cmd output
    ਪੂਰਨ		tl_toprow;	// row with first line of system terminal
#ਜੇ_ਅੰਤ

    // Set when setting the size of a vterm, reset after redrawing.
    ਪੂਰਨ		tl_vterm_ਮਾਪ_ਬਦਲਿਆ;

    ਪੂਰਨ		tl_ਸਾਧਾਰਨ_ਬਹੁਲਕ; // TRUE: Terminal-Normal mode
    ਪੂਰਨ		tl_channel_closing;
    ਪੂਰਨ		tl_channel_closed;
    ਪੂਰਨ		tl_channel_recently_closed; // still need to handle tl_finish

    ਪੂਰਨ		tl_ਅੰਤ;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TL_ਅੰਤ_UNSET	    NUL
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TL_ਅੰਤ_CLOSE	    'c'	// ++close or :terminal without argument
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TL_ਅੰਤ_NOCLOSE   'n'	// ++noclose
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TL_ਅੰਤ_OPEN	    'o'	// ++open
    char_u	*tl_opencmd;
    char_u	*tl_eof_chars;
    char_u	*tl_api;	// prefix for terminal API function

    char_u	*tl_arg0_cmd;	// To format the status bar

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
    ਖਾਲੀ	*tl_winpty_config;
    ਖਾਲੀ	*tl_winpty;

    HPCON	tl_conpty;
    DYN_STARTUPINFOEXW tl_siex;	// Structure that always needs to be hold

    FILE	*tl_ਬਾਹਰਲਾ_fd;
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_SESSION)
    char_u	*tl_command;
#ਜੇ_ਅੰਤ
    char_u	*tl_kill;

    // last known vterm size
    ਪੂਰਨ		tl_ਪੰਕਤੀਆਂ;
    ਪੂਰਨ		tl_ਖਾਨੇ_ਗਿਣਤੀ;

    char_u	*tl_title; // NULL or allocated
    char_u	*tl_ਸਥਿਤੀ_ਪਾਠ; // NULL or allocated

    // Range of screen rows to update.  Zero based.
    ਪੂਰਨ		tl_dirty_ਪੰਕਤੀ_ਸ਼ੁਰੂ; // MAX_ROW if nothing dirty
    ਪੂਰਨ		tl_dirty_ਪੰਕਤੀ_ਸਿਰਾ;   // row below last one to update
    ਪੂਰਨ		tl_dirty_snapshot;  // text updated after making snapshot
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TIMERS
    ਪੂਰਨ		tl_timer_ਸੈੱਟ;
    proftime_T	tl_timer_due;
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		tl_postponed_scroll;	// to be scrolled up

    garray_T	tl_scrollback;
    ਪੂਰਨ		tl_scrollback_scrolled;
    garray_T	tl_scrollback_postponed;
    ਪੂਰਨ		tl_scrollback_snapshot;
    ਪੂਰਨ		tl_ਬਫਰ_scrolled;

    char_u	*tl_highlight_name; // replaces "Terminal"; allocated

    cellattr_T	tl_default_color;

    linenr_T	tl_ਸਿਖਰ_diff_ਪੰਕਤੀਆਂ;   // rows of top diff file or zero
    linenr_T	tl_bot_diff_ਪੰਕਤੀਆਂ;   // rows of bottom diff file

    VTermPos	tl_cursor_pos;
    ਪੂਰਨ		tl_cursor_visible;
    ਪੂਰਨ		tl_cursor_blink;
    ਪੂਰਨ		tl_cursor_shape;  // 1: block, 2: underline, 3: bar
    char_u	*tl_cursor_color; // NULL or allocated

    long_u	*tl_palette; // array of 16 colors specified by term_start, can
			     // be NULL
    ਪੂਰਨ		tl_using_altscreen;
    garray_T	tl_osc_buf;	    // incomplete OSC string
};

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TMODE_ONCE 1	    // CTRL-\ CTRL-N used
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TMODE_LOOP 2	    // CTRL-W N used

/*
 * List of all active terminals.
 */
ਸਥਿਰ ਪਦ_T *ਪਹਿਲਾ_ਪਦ = ਨਿਰਾ;

// Terminal active in terminal_loop().
ਸਥਿਰ ਪਦ_T *ਵਿੱਚ_terminal_loop = ਨਿਰਾ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
ਸਥਿਰ BOOL has_winpty = FALSE;
ਸਥਿਰ BOOL has_conpty = FALSE;
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_ਪੰਕਤੀ 999999	    // used for tl_dirty_row_end to update all rows
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਕੁੰਜੀ_BUF_LEN 200

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FOR_ALL_TERMS(ਪਦ)	\
    ਲਈ ((ਪਦ) = ਪਹਿਲਾ_ਪਦ; (ਪਦ) != ਨਿਰਾ; (ਪਦ) = (ਪਦ)->tl_ਅਗਲਾ)

/*
 * Functions with separate implementation for MS-Windows and Unix-like systems.
 */
ਸਥਿਰ ਪੂਰਨ ਪਦ_and_ਕੰਮ_init(ਪਦ_T *ਪਦ, typval_T *argvar, ਅੱਖਰ **argv, jobopt_T *opt, jobopt_T *orig_opt);
ਸਥਿਰ ਪੂਰਨ create_pty_only(ਪਦ_T *ਪਦ, jobopt_T *opt);
ਸਥਿਰ ਖਾਲੀ ਪਦ_report_winsize(ਪਦ_T *ਪਦ, ਪੂਰਨ ਪੰਕਤੀਆਂ_2, ਪੂਰਨ ਖਾਨੇ_ਗਿਣਤੀ);
ਸਥਿਰ ਖਾਲੀ ਪਦ_free_vterm(ਪਦ_T *ਪਦ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
ਸਥਿਰ ਖਾਲੀ update_system_ਪਦ(ਪਦ_T *ਪਦ);
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਖਾਲੀ handle_postponed_scrollback(ਪਦ_T *ਪਦ);

// The character that we know (or assume) that the terminal expects for the
// backspace key.
ਸਥਿਰ ਪੂਰਨ ਪਦ_backspace_char = BS;

// Store the last set and the desired cursor properties, so that we only update
// them when needed.  Doing it unnecessary may result in flicker.
ਸਥਿਰ char_u	*ਆਖਰੀ_ਸੈੱਟ_cursor_color = ਨਿਰਾ;
ਸਥਿਰ char_u	*desired_cursor_color = ਨਿਰਾ;
ਸਥਿਰ ਪੂਰਨ	ਆਖਰੀ_ਸੈੱਟ_cursor_shape = -1;
ਸਥਿਰ ਪੂਰਨ	desired_cursor_shape = -1;
ਸਥਿਰ ਪੂਰਨ	ਆਖਰੀ_ਸੈੱਟ_cursor_blink = -1;
ਸਥਿਰ ਪੂਰਨ	desired_cursor_blink = -1;


///////////////////////////////////////
// 1. Generic code for all systems.

    ਸਥਿਰ ਪੂਰਨ
cursor_color_equal(char_u *lhs_color, char_u *rhs_color)
{
    ਜੇ (lhs_color != ਨਿਰਾ && rhs_color != ਨਿਰਾ)
	ਵਾਪਸ STRCMP(lhs_color, rhs_color) == 0;
    ਵਾਪਸ lhs_color == ਨਿਰਾ && rhs_color == ਨਿਰਾ;
}

    ਸਥਿਰ ਖਾਲੀ
cursor_color_copy(char_u **ਮੰਜ਼ਿਲ_color, char_u *from_color)
{
    // Avoid a free & alloc if the value is already right.
    ਜੇ (cursor_color_equal(*ਮੰਜ਼ਿਲ_color, from_color))
	ਵਾਪਸ;
    vim_free(*ਮੰਜ਼ਿਲ_color);
    *ਮੰਜ਼ਿਲ_color = (from_color == ਨਿਰਾ) ? ਨਿਰਾ : vim_strsave(from_color);
}

    ਸਥਿਰ char_u *
cursor_color_get(char_u *color)
{
    ਵਾਪਸ (color == ਨਿਰਾ) ? (char_u *)"" : color;
}


/*
 * Parse 'termwinsize' and set "rows" and "cols" for the terminal size in the
 * current window.
 * Sets "rows" and/or "cols" to zero when it should follow the window size.
 * Return TRUE if the size is the minimum size: "24*80".
 */
    ਸਥਿਰ ਪੂਰਨ
parse_termwinsize(win_T *wp, ਪੂਰਨ *ਪੰਕਤੀਆਂ_2, ਪੂਰਨ *ਖਾਨੇ_ਗਿਣਤੀ)
{
    ਪੂਰਨ	minsize = FALSE;

    *ਪੰਕਤੀਆਂ_2 = 0;
    *ਖਾਨੇ_ਗਿਣਤੀ = 0;

    ਜੇ (*wp->w_p_tws == NUL)
	ਵਾਪਸ FALSE;

    char_u *p = vim_strchr(wp->w_p_tws, 'x');

    // Syntax of value was already checked when it's set.
    ਜੇ (p == ਨਿਰਾ)
    {
	minsize = TRUE;
	p = vim_strchr(wp->w_p_tws, '*');
    }
    *ਪੰਕਤੀਆਂ_2 = atoi((ਅੱਖਰ *)wp->w_p_tws);
    *ਖਾਨੇ_ਗਿਣਤੀ = atoi((ਅੱਖਰ *)p + 1);
    ਜੇ (*ਪੰਕਤੀਆਂ_2 > VTERM_MAX_ਪੰਕਤੀਆਂ)
	*ਪੰਕਤੀਆਂ_2 = VTERM_MAX_ਪੰਕਤੀਆਂ;
    ਜੇ (*ਖਾਨੇ_ਗਿਣਤੀ > VTERM_MAX_ਖਾਨੇ_ਗਿਣਤੀ)
	*ਖਾਨੇ_ਗਿਣਤੀ = VTERM_MAX_ਖਾਨੇ_ਗਿਣਤੀ;
    ਵਾਪਸ minsize;
}

/*
 * Determine the terminal size from 'termwinsize' and the current window.
 */
    ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_ਪਦ_and_win_ਮਾਪ(ਪਦ_T *ਪਦ, jobopt_T *opt)
{
    ਪੂਰਨ ਪੰਕਤੀਆਂ_2, ਖਾਨੇ_ਗਿਣਤੀ;
    ਪੂਰਨ	minsize;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    ਜੇ (ਪਦ->tl_system)
    {
	// Use the entire screen (excluding the tabpanel area) for the system
	// commands.  However, it will start at the command line and scroll up
	// as needed, using tl_toprow.
	ਪਦ->tl_ਪੰਕਤੀਆਂ = ਪੰਕਤੀਆਂ;
	ਪਦ->tl_ਖਾਨੇ_ਗਿਣਤੀ = topframe->fr_ਚੌੜਾਈ;
	ਵਾਪਸ;
    }
#ਜੇ_ਅੰਤ
    ਪਦ->tl_ਪੰਕਤੀਆਂ = curwin->w_ਉਚਾਈ;
    ਪਦ->tl_ਖਾਨੇ_ਗਿਣਤੀ = curwin->w_ਚੌੜਾਈ;

    minsize = parse_termwinsize(curwin, &ਪੰਕਤੀਆਂ_2, &ਖਾਨੇ_ਗਿਣਤੀ);
    ਜੇ (minsize)
    {
	ਜੇ (ਪਦ->tl_ਪੰਕਤੀਆਂ < ਪੰਕਤੀਆਂ_2)
	    ਪਦ->tl_ਪੰਕਤੀਆਂ = ਪੰਕਤੀਆਂ_2;
	ਜੇ (ਪਦ->tl_ਖਾਨੇ_ਗਿਣਤੀ < ਖਾਨੇ_ਗਿਣਤੀ)
	    ਪਦ->tl_ਖਾਨੇ_ਗਿਣਤੀ = ਖਾਨੇ_ਗਿਣਤੀ;
    }
    ਜੇ ((opt->jo_set2 & JO2_ਪਦ_ਪੰਕਤੀਆਂ))
	ਪਦ->tl_ਪੰਕਤੀਆਂ = opt->jo_ਪਦ_ਪੰਕਤੀਆਂ;
    ਨਹੀਂਤਾਂ ਜੇ (ਪੰਕਤੀਆਂ_2 != 0)
	ਪਦ->tl_ਪੰਕਤੀਆਂ = ਪੰਕਤੀਆਂ_2;
    ਜੇ ((opt->jo_set2 & JO2_ਪਦ_ਖਾਨੇ_ਗਿਣਤੀ))
	ਪਦ->tl_ਖਾਨੇ_ਗਿਣਤੀ = opt->jo_ਪਦ_ਖਾਨੇ_ਗਿਣਤੀ;
    ਨਹੀਂਤਾਂ ਜੇ (ਖਾਨੇ_ਗਿਣਤੀ != 0)
	ਪਦ->tl_ਖਾਨੇ_ਗਿਣਤੀ = ਖਾਨੇ_ਗਿਣਤੀ;

    ਜੇ (!opt->jo_hidden)
    {
	ਜੇ (ਪਦ->tl_ਪੰਕਤੀਆਂ != curwin->w_ਉਚਾਈ)
	    win_setheight_win(ਪਦ->tl_ਪੰਕਤੀਆਂ, curwin);
	ਜੇ (ਪਦ->tl_ਖਾਨੇ_ਗਿਣਤੀ != curwin->w_ਚੌੜਾਈ)
	    win_setwidth_win(ਪਦ->tl_ਖਾਨੇ_ਗਿਣਤੀ, curwin);

	// Set 'winsize' now to avoid a resize at the next redraw.
	ਜੇ (!minsize && *curwin->w_p_tws != NUL)
	{
	    char_u buf[100];

	    vim_snprintf((ਅੱਖਰ *)buf, 100, "%dx%d",
						 ਪਦ->tl_ਪੰਕਤੀਆਂ, ਪਦ->tl_ਖਾਨੇ_ਗਿਣਤੀ);
	    ਸੈੱਟ_option_ਮੁੱਲ_give_err((char_u *)"termwinsize",
							   0L, buf, OPT_LOCAL);
	}
    }
}

/*
 * Initialize job options for a terminal job.
 * Caller may overrule some of them.
 */
    ਖਾਲੀ
init_ਕੰਮ_options(jobopt_T *opt)
{
    clear_ਕੰਮ_options(opt);

    opt->jo_ਬਹੁਲਕ = CH_ਬਹੁਲਕ_RAW;
    opt->jo_ਬਾਹਰਲਾ_ਬਹੁਲਕ = CH_ਬਹੁਲਕ_RAW;
    opt->jo_err_ਬਹੁਲਕ = CH_ਬਹੁਲਕ_RAW;
    opt->jo_ਸੈੱਟ = JO_ਬਹੁਲਕ | JO_ਬਾਹਰਲਾ_ਬਹੁਲਕ | JO_ERR_ਬਹੁਲਕ;
}

/*
 * Set job options mandatory for a terminal job.
 */
    ਸਥਿਰ ਖਾਲੀ
ਤਿਆਰੀ_ਕੰਮ_options(jobopt_T *opt, ਪੂਰਨ ਪੰਕਤੀਆਂ_2, ਪੂਰਨ ਖਾਨੇ_ਗਿਣਤੀ)
{
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ MSWIN
    // Win32: Redirecting the job output won't work, thus always connect stdout
    // here.
    ਜੇ (!(opt->jo_ਸੈੱਟ & JO_ਬਾਹਰਲਾ_IO))
#ਜੇ_ਅੰਤ
    {
	// Connect stdout to the terminal.
	opt->jo_io[PART_ਬਾਹਰਲਾ] = JIO_ਬਫਰ;
	opt->jo_io_buf[PART_ਬਾਹਰਲਾ] = curbuf->b_fnum;
	opt->jo_modifiable[PART_ਬਾਹਰਲਾ] = 0;
	opt->jo_ਸੈੱਟ |= JO_ਬਾਹਰਲਾ_IO + JO_ਬਾਹਰਲਾ_BUF + JO_ਬਾਹਰਲਾ_MODIFIABLE;
    }

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ MSWIN
    // Win32: Redirecting the job output won't work, thus always connect stderr
    // here.
    ਜੇ (!(opt->jo_ਸੈੱਟ & JO_ERR_IO))
#ਜੇ_ਅੰਤ
    {
	// Connect stderr to the terminal.
	opt->jo_io[PART_ERR] = JIO_ਬਫਰ;
	opt->jo_io_buf[PART_ERR] = curbuf->b_fnum;
	opt->jo_modifiable[PART_ERR] = 0;
	opt->jo_ਸੈੱਟ |= JO_ERR_IO + JO_ERR_BUF + JO_ERR_MODIFIABLE;
    }

    opt->jo_pty = TRUE;
    ਜੇ ((opt->jo_set2 & JO2_ਪਦ_ਪੰਕਤੀਆਂ) == 0)
	opt->jo_ਪਦ_ਪੰਕਤੀਆਂ = ਪੰਕਤੀਆਂ_2;
    ਜੇ ((opt->jo_set2 & JO2_ਪਦ_ਖਾਨੇ_ਗਿਣਤੀ) == 0)
	opt->jo_ਪਦ_ਖਾਨੇ_ਗਿਣਤੀ = ਖਾਨੇ_ਗਿਣਤੀ;
}

/*
 * Flush messages on channels.
 */
    ਸਥਿਰ ਖਾਲੀ
ਪਦ_flush_messages(ਖਾਲੀ)
{
    mch_check_messages();
    parse_queued_messages();
}

/*
 * Close a terminal buffer (and its window).  Used when creating the terminal
 * fails.
 */
    ਸਥਿਰ ਖਾਲੀ
ਪਦ_close_ਬਫਰ(buf_T *buf, buf_T *old_curbuf)
{
    free_terminal(buf);
    ਜੇ (old_curbuf != ਨਿਰਾ)
    {
	--curbuf->b_nwindows;
	curbuf = old_curbuf;
	curwin->w_ਬਫਰ = curbuf;
	++curbuf->b_nwindows;
    }
    CHECK_CURBUF;

    // Wiping out the buffer will also close the window and call
    // free_terminal().
    do_ਬਫਰ(DOBUF_WIPE, DOBUF_ਪਹਿਲਾ, FORWARD, buf->b_fnum, TRUE);
}

/*
 * Start a terminal window and return its buffer.
 * Use either "argvar" or "argv", the other must be NULL.
 * When "flags" has TERM_START_NOJOB only create the buffer, b_term and open
 * the window.
 * Returns NULL when failed.
 */
    buf_T *
ਪਦ_ਸ਼ੁਰੂ(
	typval_T    *argvar,
	ਅੱਖਰ	    **argv,
	jobopt_T    *opt,
	ਪੂਰਨ	    flags)
{
    exarg_T	ਵੰਡੋ_ea;
    win_T	*old_curwin = curwin;
    ਪਦ_T	*ਪਦ;
    buf_T	*old_curbuf = ਨਿਰਾ;
    ਪੂਰਨ		res;
    buf_T	*newbuf;
    ਪੂਰਨ		vertical = opt->jo_vertical || (cmdmod.cmod_ਵੰਡੋ & WSP_VERT);
    jobopt_T	orig_opt;  // only partly filled
    pos_T	save_cursor;

    ਜੇ (check_restricted() || check_secure())
	ਵਾਪਸ ਨਿਰਾ;
    ਜੇ (cmdwin_type != 0)
    {
	emsg(_(e_cannot_open_terminal_from_command_ਸਤਰ_ਖਿੜਕੀ));
	ਵਾਪਸ ਨਿਰਾ;
    }

    ਜੇ ((opt->jo_ਸੈੱਟ & (JO_ਵਿੱਚ_IO + JO_ਬਾਹਰਲਾ_IO + JO_ERR_IO))
					 == (JO_ਵਿੱਚ_IO + JO_ਬਾਹਰਲਾ_IO + JO_ERR_IO)
	|| (!(opt->jo_ਸੈੱਟ & JO_ਬਾਹਰਲਾ_IO) && (opt->jo_ਸੈੱਟ & JO_ਬਾਹਰਲਾ_BUF))
	|| (!(opt->jo_ਸੈੱਟ & JO_ERR_IO) && (opt->jo_ਸੈੱਟ & JO_ERR_BUF))
	|| (argvar != ਨਿਰਾ
	    && argvar->v_type == VAR_LIST
	    && argvar->vval.v_list != ਨਿਰਾ
	    && argvar->vval.v_list->lv_ਪਹਿਲਾ == &range_list_ਵਸਤੂ))
    {
	emsg(_(e_invalid_ਦਲੀਲ));
	ਵਾਪਸ ਨਿਰਾ;
    }

    ਪਦ = ALLOC_CLEAR_ONE(ਪਦ_T);
    ਜੇ (ਪਦ == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    ਪਦ->tl_dirty_ਪੰਕਤੀ_ਸਿਰਾ = MAX_ਪੰਕਤੀ;
    ਪਦ->tl_cursor_visible = TRUE;
    ਪਦ->tl_cursor_shape = VTERM_PROP_CURSORSHAPE_ਖੰਡ;
    ਪਦ->tl_ਅੰਤ = opt->jo_ਪਦ_ਅੰਤ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    ਪਦ->tl_system = (flags & ਪਦ_ਸ਼ੁਰੂ_SYSTEM);
#ਜੇ_ਅੰਤ
    ga_init2(&ਪਦ->tl_scrollback, ਆਕਾਰ(sb_ਸਤਰ_T), 300);
    ga_init2(&ਪਦ->tl_scrollback_postponed, ਆਕਾਰ(sb_ਸਤਰ_T), 300);
    ga_init2(&ਪਦ->tl_osc_buf, ਆਕਾਰ(ਅੱਖਰ), 300);

    setpcmark();
    CLEAR_ਖੇਤਰ(ਵੰਡੋ_ea);
    ਜੇ (opt->jo_curwin)
    {
	// Create a new buffer in the current window.
	ਜੇ (!can_abandon(curbuf, flags & ਪਦ_ਸ਼ੁਰੂ_FORCEIT))
	{
	    no_write_ਸੁਨੇਹਾ();
	    vim_free(ਪਦ);
	    ਵਾਪਸ ਨਿਰਾ;
	}
	ਜੇ (do_ecmd(0, ਨਿਰਾ, ਨਿਰਾ, &ਵੰਡੋ_ea, ECMD_ONE,
		      (buf_hide(curwin->w_ਬਫਰ) ? ECMD_HIDE : 0)
			  + ((flags & ਪਦ_ਸ਼ੁਰੂ_FORCEIT) ? ECMD_FORCEIT : 0),
							       curwin) == FAIL)
	{
	    vim_free(ਪਦ);
	    ਵਾਪਸ ਨਿਰਾ;
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (opt->jo_hidden || (flags & ਪਦ_ਸ਼ੁਰੂ_SYSTEM))
    {
	buf_T *buf;

	// Create a new buffer without a window. Make it the current buffer for
	// a moment to be able to do the initializations.
	buf = buflist_new((char_u *)"", ਨਿਰਾ, (linenr_T)0,
							 BLN_NEW | BLN_LISTED);
	ਜੇ (buf == ਨਿਰਾ || ml_open(buf) == FAIL)
	{
	    vim_free(ਪਦ);
	    ਵਾਪਸ ਨਿਰਾ;
	}
	old_curbuf = curbuf;
	--curbuf->b_nwindows;
	curbuf = buf;
	save_cursor = curwin->w_cursor;
	curwin->w_ਬਫਰ = buf;
	++curbuf->b_nwindows;
    }
    ਨਹੀਂਤਾਂ
    {
	// Open a new window or tab.
	ਵੰਡੋ_ea.cmdidx = CMD_new;
	ਵੰਡੋ_ea.cmd = (char_u *)"new";
	ਵੰਡੋ_ea.arg = (char_u *)"";
	ਜੇ (opt->jo_ਪਦ_ਪੰਕਤੀਆਂ > 0 && !vertical)
	{
	    ਵੰਡੋ_ea.line2 = opt->jo_ਪਦ_ਪੰਕਤੀਆਂ;
	    ਵੰਡੋ_ea.addr_ਗਿਣਤੀ = 1;
	}
	ਜੇ (opt->jo_ਪਦ_ਖਾਨੇ_ਗਿਣਤੀ > 0 && vertical)
	{
	    ਵੰਡੋ_ea.line2 = opt->jo_ਪਦ_ਖਾਨੇ_ਗਿਣਤੀ;
	    ਵੰਡੋ_ea.addr_ਗਿਣਤੀ = 1;
	}

	ਪੂਰਨ cmod_ਵੰਡੋ_modified = FALSE;
	ਜੇ (vertical)
	{
	    ਜੇ (!(cmdmod.cmod_ਵੰਡੋ & WSP_VERT))
		cmod_ਵੰਡੋ_modified = TRUE;
	    cmdmod.cmod_ਵੰਡੋ |= WSP_VERT;
	}
	ex_splitview(&ਵੰਡੋ_ea);
	ਜੇ (cmod_ਵੰਡੋ_modified)
	    cmdmod.cmod_ਵੰਡੋ &= ~WSP_VERT;
	ਜੇ (curwin == old_curwin)
	{
	    // split failed
	    vim_free(ਪਦ);
	    ਵਾਪਸ ਨਿਰਾ;
	}
    }
    ਪਦ->tl_ਬਫਰ = curbuf;
    curbuf->b_ਪਦ = ਪਦ;

    ਜੇ (!opt->jo_hidden)
    {
	// Only one size was taken care of with :new, do the other one.  With
	// "curwin" both need to be done.
	ਜੇ (opt->jo_ਪਦ_ਪੰਕਤੀਆਂ > 0 && (opt->jo_curwin || vertical))
	    win_setheight(opt->jo_ਪਦ_ਪੰਕਤੀਆਂ);
	ਜੇ (opt->jo_ਪਦ_ਖਾਨੇ_ਗਿਣਤੀ > 0 && (opt->jo_curwin || !vertical))
	    win_setwidth(opt->jo_ਪਦ_ਖਾਨੇ_ਗਿਣਤੀ);
    }

    // Link the new terminal in the list of active terminals.
    ਪਦ->tl_ਅਗਲਾ = ਪਹਿਲਾ_ਪਦ;
    ਪਹਿਲਾ_ਪਦ = ਪਦ;

    apply_autocmds(EVENT_BUFFILEPRE, ਨਿਰਾ, ਨਿਰਾ, FALSE, curbuf);

    ਜੇ (opt->jo_ਪਦ_name != ਨਿਰਾ)
    {
	vim_free(curbuf->b_ffname);
	curbuf->b_ffname = vim_strsave(opt->jo_ਪਦ_name);
    }
    ਨਹੀਂਤਾਂ ਜੇ (argv != ਨਿਰਾ)
    {
	vim_free(curbuf->b_ffname);
	curbuf->b_ffname = vim_strsave((char_u *)"!system");
    }
    ਨਹੀਂਤਾਂ
    {
	ਪੂਰਨ	i;
	size_t	len;
	char_u	*cmd, *p;

	ਜੇ (argvar->v_type == VAR_STRING)
	{
	    cmd = argvar->vval.v_string;
	    ਜੇ (cmd == ਨਿਰਾ)
		cmd = (char_u *)"";
	    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(cmd, "NONE") == 0)
		cmd = (char_u *)"pty";
	}
	ਨਹੀਂਤਾਂ ਜੇ (argvar->v_type != VAR_LIST
		|| argvar->vval.v_list == ਨਿਰਾ
		|| argvar->vval.v_list->lv_len == 0
		|| (cmd = tv_get_string_chk(
			       &argvar->vval.v_list->lv_ਪਹਿਲਾ->li_tv)) == ਨਿਰਾ)
	    cmd = (char_u*)"";

	len = STRLEN(cmd) + 10;
	p = alloc(len);

	ਲਈ (i = 0; p != ਨਿਰਾ; ++i)
	{
	    // Prepend a ! to the command name to avoid the buffer name equals
	    // the executable, otherwise ":w!" would overwrite it.
	    ਜੇ (i == 0)
		vim_snprintf((ਅੱਖਰ *)p, len, "!%s", cmd);
	    ਨਹੀਂਤਾਂ
		vim_snprintf((ਅੱਖਰ *)p, len, "!%s (%d)", cmd, i);
	    ਜੇ (buflist_findname(p) == ਨਿਰਾ)
	    {
		vim_free(curbuf->b_ffname);
		curbuf->b_ffname = p;
		ਰੋਕੋ;
	    }
	}
    }
    vim_free(curbuf->b_sfname);
    curbuf->b_sfname = vim_strsave(curbuf->b_ffname);
    curbuf->b_fname = curbuf->b_ffname;

    apply_autocmds(EVENT_BUFFILEPOST, ਨਿਰਾ, ਨਿਰਾ, FALSE, curbuf);

    ਜੇ (opt->jo_ਪਦ_opencmd != ਨਿਰਾ)
	ਪਦ->tl_opencmd = vim_strsave(opt->jo_ਪਦ_opencmd);

    ਜੇ (opt->jo_eof_chars != ਨਿਰਾ)
	ਪਦ->tl_eof_chars = vim_strsave(opt->jo_eof_chars);

    ਸੈੱਟ_string_option_ਸਿੱਧਾ((char_u *)"buftype", -1,
				  (char_u *)"terminal", OPT_FREE|OPT_LOCAL, 0);
    // Avoid that 'buftype' is reset when this buffer is entered.
    curbuf->b_p_initialized = true;

    // Mark the buffer as not modifiable. It can only be made modifiable after
    // the job finished.
    curbuf->b_p_ma = FALSE;

    ਸੈੱਟ_ਪਦ_and_win_ਮਾਪ(ਪਦ, opt);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
    mch_memmove(orig_opt.jo_io, opt->jo_io, ਆਕਾਰ(orig_opt.jo_io));
#ਜੇ_ਅੰਤ
    ਤਿਆਰੀ_ਕੰਮ_options(opt, ਪਦ->tl_ਪੰਕਤੀਆਂ, ਪਦ->tl_ਖਾਨੇ_ਗਿਣਤੀ);

    ਜੇ (flags & ਪਦ_ਸ਼ੁਰੂ_NOJOB)
	ਵਾਪਸ curbuf;

#ਜੇ defined(FEAT_SESSION)
    // Remember the command for the session file.
    ਜੇ (opt->jo_ਪਦ_norestore || argv != ਨਿਰਾ)
	ਪਦ->tl_command = vim_strsave((char_u *)"NONE");
    ਨਹੀਂਤਾਂ ਜੇ (argvar->v_type == VAR_STRING)
    {
	char_u	*cmd = argvar->vval.v_string;

	ਜੇ (cmd != ਨਿਰਾ && STRCMP(cmd, p_sh) != 0)
	    ਪਦ->tl_command = vim_strsave(cmd);
    }
    ਨਹੀਂਤਾਂ ਜੇ (argvar->v_type == VAR_LIST
	    && argvar->vval.v_list != ਨਿਰਾ
	    && argvar->vval.v_list->lv_len > 0)
    {
	garray_T	ga;
	listitem_T	*ਵਸਤੂ;

	ga_init2(&ga, 1, 100);
	FOR_ALL_LIST_ਚੀਜ਼ਾਂ(argvar->vval.v_list, ਵਸਤੂ)
	{
	    char_u *s = tv_get_string_chk(&ਵਸਤੂ->li_tv);
	    char_u *p;

	    ਜੇ (s == ਨਿਰਾ)
		ਰੋਕੋ;
	    p = vim_strsave_fnameescape(s, VSE_NONE);
	    ਜੇ (p == ਨਿਰਾ)
		ਰੋਕੋ;
	    ga_concat(&ga, p);
	    vim_free(p);
	    ga_append(&ga, ' ');
	}
	ਜੇ (ਵਸਤੂ == ਨਿਰਾ)
	{
	    ga_append(&ga, NUL);
	    ਪਦ->tl_command = ga.ga_ਅੰਕੜੇ;
	}
	ਨਹੀਂਤਾਂ
	    ga_clear(&ga);
    }
#ਜੇ_ਅੰਤ

    ਜੇ (opt->jo_ਪਦ_kill != ਨਿਰਾ)
    {
	char_u *p = skiptowhite(opt->jo_ਪਦ_kill);

	ਪਦ->tl_kill = vim_strnsave(opt->jo_ਪਦ_kill, p - opt->jo_ਪਦ_kill);
    }

    ਜੇ (opt->jo_ਪਦ_api != ਨਿਰਾ)
    {
	char_u *p = skiptowhite(opt->jo_ਪਦ_api);

	ਪਦ->tl_api = vim_strnsave(opt->jo_ਪਦ_api, p - opt->jo_ਪਦ_api);
    }
    ਨਹੀਂਤਾਂ
	ਪਦ->tl_api = vim_strsave((char_u *)"Tapi_");

    ਜੇ (opt->jo_set2 & JO2_ਪਦ_HIGHLIGHT)
	ਪਦ->tl_highlight_name = vim_strsave(opt->jo_ਪਦ_highlight);

#ਜੇ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    // Save the user-defined palette, it is only used in GUI (or 'tgc' is on).
    ਜੇ (opt->jo_set2 & JO2_ANSI_COLORS)
    {
	ਪਦ->tl_palette = ALLOC_MULT(long_u, 16);
	ਜੇ (ਪਦ->tl_palette == ਨਿਰਾ)
	{
	    vim_free(ਪਦ);
	    ਵਾਪਸ ਨਿਰਾ;
	}
	ਸਮ੍ਰਿਤੀ_ਨਕਲ(ਪਦ->tl_palette, opt->jo_ansi_colors, ਆਕਾਰ(long_u) * 16);
    }
#ਜੇ_ਅੰਤ

    // System dependent: setup the vterm and maybe start the job in it.
    ਜੇ (argv == ਨਿਰਾ
	    && argvar->v_type == VAR_STRING
	    && argvar->vval.v_string != ਨਿਰਾ
	    && STRCMP(argvar->vval.v_string, "NONE") == 0)
	res = create_pty_only(ਪਦ, opt);
    ਨਹੀਂਤਾਂ
	res = ਪਦ_and_ਕੰਮ_init(ਪਦ, argvar, argv, opt, &orig_opt);

    newbuf = curbuf;
    ਜੇ (res == OK)
    {
	// Get and remember the size we ended up with.  Update the pty.
	vterm_get_ਮਾਪ(ਪਦ->tl_vterm, &ਪਦ->tl_ਪੰਕਤੀਆਂ, &ਪਦ->tl_ਖਾਨੇ_ਗਿਣਤੀ);
	ਪਦ_report_winsize(ਪਦ, ਪਦ->tl_ਪੰਕਤੀਆਂ, ਪਦ->tl_ਖਾਨੇ_ਗਿਣਤੀ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
	ਜੇ (ਪਦ->tl_system)
	{
	    // display first line below typed command
	    ਪਦ->tl_toprow = ਸੁਨੇਹਾ_ਪੰਕਤੀ + 1;
	    ਪਦ->tl_dirty_ਪੰਕਤੀ_ਸਿਰਾ = 0;
	}
#ਜੇ_ਅੰਤ

	// Make sure we don't get stuck on sending keys to the job, it leads to
	// a deadlock if the job is waiting for Vim to read.
	channel_ਸੈੱਟ_nonblock(ਪਦ->tl_ਕੰਮ->jv_channel, PART_ਵਿੱਚ);

	ਜੇ (old_curbuf != ਨਿਰਾ)
	{
	    --curbuf->b_nwindows;
	    curbuf = old_curbuf;
	    curwin->w_ਬਫਰ = curbuf;
	    curwin->w_cursor = save_cursor;
	    ++curbuf->b_nwindows;
	}
	ਨਹੀਂਤਾਂ ਜੇ (vgetc_busy
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TIMERS
		|| timer_busy
#ਜੇ_ਅੰਤ
		|| ਨਿਵੇਸ਼_busy)
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
    ਨਹੀਂਤਾਂ
    {
	ਪਦ_close_ਬਫਰ(curbuf, old_curbuf);
	ਵਾਪਸ ਨਿਰਾ;
    }

    apply_autocmds(EVENT_TERMINALOPEN, ਨਿਰਾ, ਨਿਰਾ, FALSE, newbuf);
    ਜੇ (!opt->jo_hidden && !(flags & ਪਦ_ਸ਼ੁਰੂ_SYSTEM))
	apply_autocmds(EVENT_TERMINALWINOPEN, ਨਿਰਾ, ਨਿਰਾ, FALSE, newbuf);
    ਵਾਪਸ newbuf;
}

/*
 * ":terminal": open a terminal window and execute a job in it.
 */
    ਖਾਲੀ
ex_terminal(exarg_T *eap)
{
    typval_T	argvar[2];
    jobopt_T	opt;
    ਪੂਰਨ		opt_shell = FALSE;
    char_u	*cmd;
    char_u	*tofree = ਨਿਰਾ;
    ਪੂਰਨ		scroll_save = ਸੁਨੇਹਾ_scroll;

    ਸੁਨੇਹਾ_scroll = FALSE;		// don't scroll here
    autowrite_all();
    ਸੁਨੇਹਾ_scroll = scroll_save;

    init_ਕੰਮ_options(&opt);

    cmd = eap->arg;
    ਜਦੋਂ (*cmd == '+' && *(cmd + 1) == '+')
    {
	char_u  *p, *ep;

	cmd += 2;
	p = skiptowhite(cmd);
	ep = vim_strchr(cmd, '=');
	ਜੇ (ep != ਨਿਰਾ)
	{
	    ਜੇ (ep < p)
		p = ep;
	    ਨਹੀਂਤਾਂ
		ep = ਨਿਰਾ;
	}

	// Note: Keep this in sync with get_terminalopt_name.

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OPTARG_HAS(name) ((ਪੂਰਨ)(p - cmd) == ਆਕਾਰ(name) - 1 \
				 && STRNICMP(cmd, name, ਆਕਾਰ(name) - 1) == 0)
	ਜੇ (OPTARG_HAS("close"))
	    opt.jo_ਪਦ_ਅੰਤ = 'c';
	ਨਹੀਂਤਾਂ ਜੇ (OPTARG_HAS("noclose"))
	    opt.jo_ਪਦ_ਅੰਤ = 'n';
	ਨਹੀਂਤਾਂ ਜੇ (OPTARG_HAS("open"))
	    opt.jo_ਪਦ_ਅੰਤ = 'o';
	ਨਹੀਂਤਾਂ ਜੇ (OPTARG_HAS("curwin"))
	    opt.jo_curwin = 1;
	ਨਹੀਂਤਾਂ ਜੇ (OPTARG_HAS("hidden"))
	    opt.jo_hidden = 1;
	ਨਹੀਂਤਾਂ ਜੇ (OPTARG_HAS("norestore"))
	    opt.jo_ਪਦ_norestore = 1;
	ਨਹੀਂਤਾਂ ਜੇ (OPTARG_HAS("shell"))
	    opt_shell = TRUE;
	ਨਹੀਂਤਾਂ ਜੇ (OPTARG_HAS("kill") && ep != ਨਿਰਾ)
	{
	    opt.jo_set2 |= JO2_ਪਦ_KILL;
	    opt.jo_ਪਦ_kill = ep + 1;
	    p = skiptowhite(cmd);
	}
	ਨਹੀਂਤਾਂ ਜੇ (OPTARG_HAS("api"))
	{
	    opt.jo_set2 |= JO2_ਪਦ_API;
	    ਜੇ (ep != ਨਿਰਾ)
	    {
		opt.jo_ਪਦ_api = ep + 1;
		p = skiptowhite(cmd);
	    }
	    ਨਹੀਂਤਾਂ
		opt.jo_ਪਦ_api = ਨਿਰਾ;
	}
	ਨਹੀਂਤਾਂ ਜੇ (OPTARG_HAS("rows") && ep != ਨਿਰਾ && ਸੁਰੱਖਿਅਤ_isdigit(ep[1]))
	{
	    opt.jo_set2 |= JO2_ਪਦ_ਪੰਕਤੀਆਂ;
	    opt.jo_ਪਦ_ਪੰਕਤੀਆਂ = atoi((ਅੱਖਰ *)ep + 1);
	    p = skiptowhite(cmd);
	}
	ਨਹੀਂਤਾਂ ਜੇ (OPTARG_HAS("cols") && ep != ਨਿਰਾ && ਸੁਰੱਖਿਅਤ_isdigit(ep[1]))
	{
	    opt.jo_set2 |= JO2_ਪਦ_ਖਾਨੇ_ਗਿਣਤੀ;
	    opt.jo_ਪਦ_ਖਾਨੇ_ਗਿਣਤੀ = atoi((ਅੱਖਰ *)ep + 1);
	    p = skiptowhite(cmd);
	}
	ਨਹੀਂਤਾਂ ਜੇ (OPTARG_HAS("eof") && ep != ਨਿਰਾ)
	{
	    char_u *buf = ਨਿਰਾ;
	    char_u *ਕੁੰਜੀਆਂ;

	    vim_free(opt.jo_eof_chars);
	    p = skiptowhite(cmd);
	    *p = NUL;
	    ਕੁੰਜੀਆਂ = replace_termcodes(ep + 1, &buf, 0,
		    REPTERM_FROM_PART | REPTERM_DO_LT | REPTERM_SPECIAL, ਨਿਰਾ);
	    opt.jo_set2 |= JO2_EOF_CHARS;
	    opt.jo_eof_chars = vim_strsave(ਕੁੰਜੀਆਂ);
	    vim_free(buf);
	    *p = ' ';
	}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
	ਨਹੀਂਤਾਂ ਜੇ ((ਪੂਰਨ)(p - cmd) == 4 && STRNICMP(cmd, "type", 4) == 0
								 && ep != ਨਿਰਾ)
	{
	    ਪੂਰਨ tty_type = NUL;

	    p = skiptowhite(cmd);
	    ਜੇ (STRNICMP(ep + 1, "winpty", p - (ep + 1)) == 0)
		tty_type = 'w';
	    ਨਹੀਂਤਾਂ ਜੇ (STRNICMP(ep + 1, "conpty", p - (ep + 1)) == 0)
		tty_type = 'c';
	    ਨਹੀਂਤਾਂ
	    {
		semsg(_(e_invalid_ਮੁੱਲ_for_ਦਲੀਲ_str), "type");
		ਜਾਓ theend;
	    }
	    opt.jo_set2 |= JO2_TTY_TYPE;
	    opt.jo_tty_type = tty_type;
	}
#ਜੇ_ਅੰਤ
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (*p)
		*p = NUL;
	    semsg(_(e_invalid_attribute_str), cmd);
	    ਜਾਓ theend;
	}
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ OPTARG_HAS
	cmd = skipwhite(p);
    }
    ਜੇ (*cmd == NUL)
    {
	// Make a copy of 'shell', an autocommand may change the option.
	tofree = cmd = vim_strsave(p_sh);

	// default to close when the shell exits
	ਜੇ (opt.jo_ਪਦ_ਅੰਤ == NUL)
	    opt.jo_ਪਦ_ਅੰਤ = TL_ਅੰਤ_CLOSE;
    }

    ਜੇ (eap->addr_ਗਿਣਤੀ > 0)
    {
	// Write lines from current buffer to the job.
	opt.jo_ਸੈੱਟ |= JO_ਵਿੱਚ_IO | JO_ਵਿੱਚ_BUF | JO_ਵਿੱਚ_ਸਿਖਰ | JO_ਵਿੱਚ_BOT;
	opt.jo_io[PART_ਵਿੱਚ] = JIO_ਬਫਰ;
	opt.jo_io_buf[PART_ਵਿੱਚ] = curbuf->b_fnum;
	opt.jo_ਵਿੱਚ_ਸਿਖਰ = eap->line1;
	opt.jo_ਵਿੱਚ_bot = eap->line2;
    }

    ਜੇ (opt_shell && tofree == ਨਿਰਾ)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
	ਅੱਖਰ	**argv = ਨਿਰਾ;
	char_u	*tofree1 = ਨਿਰਾ;
	char_u	*tofree2 = ਨਿਰਾ;

	// :term ++shell command
	ਜੇ (unix_build_argv(cmd, &argv, &tofree1, &tofree2) == OK)
	    ਪਦ_ਸ਼ੁਰੂ(ਨਿਰਾ, argv, &opt, eap->forceit ? ਪਦ_ਸ਼ੁਰੂ_FORCEIT : 0);
	vim_free(argv);
	vim_free(tofree1);
	vim_free(tofree2);
	ਜਾਓ theend;
#ਨਹੀਂਤਾਂ
# ifdef MSWIN
	long_u	    cmdlen = STRLEN(p_sh) + STRLEN(p_shcf) + STRLEN(cmd) + 10;
	char_u	    *newcmd;

	newcmd = alloc(cmdlen);
	ਜੇ (newcmd == ਨਿਰਾ)
	    ਜਾਓ theend;
	tofree = newcmd;
	vim_snprintf((ਅੱਖਰ *)newcmd, cmdlen, "%s %s %s", p_sh, p_shcf, cmd);
	cmd = newcmd;
# ਨਹੀਂਤਾਂ
	emsg(_(e_sorry_plusplusshell_not_supported_on_this_system));
	ਜਾਓ theend;
# endif
#ਜੇ_ਅੰਤ
    }
    argvar[0].v_type = VAR_STRING;
    argvar[0].vval.v_string = cmd;
    argvar[1].v_type = VAR_UNKNOWN;
    ਪਦ_ਸ਼ੁਰੂ(argvar, ਨਿਰਾ, &opt, eap->forceit ? ਪਦ_ਸ਼ੁਰੂ_FORCEIT : 0);

theend:
    vim_free(tofree);
    vim_free(opt.jo_eof_chars);
}

    ਸਥਿਰ char_u *
get_terminalopt_name(expand_T *xp ਅਣਵਰਤਿਆ, ਪੂਰਨ idx)
{
    // Note: Keep this in sync with ex_terminal.
    ਸਥਿਰ ਅੱਖਰ *(p_termopt_ਮੁੱਲਾਂ[]) =
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

    ਜੇ (idx < (ਪੂਰਨ)ARRAY_ਲੰਮਾਈ(p_termopt_ਮੁੱਲਾਂ))
	ਵਾਪਸ (char_u*)p_termopt_ਮੁੱਲਾਂ[idx];
    ਵਾਪਸ ਨਿਰਾ;
}

    ਸਥਿਰ char_u *
get_termkill_name(expand_T *xp ਅਣਵਰਤਿਆ, ਪੂਰਨ idx)
{
    // These are platform-specific values used for job_stop(). They are defined
    // in each platform's mch_signal_job(). Just use a unified auto-complete
    // list for simplicity.
    ਸਥਿਰ ਅੱਖਰ *(p_termkill_ਮੁੱਲਾਂ[]) =
    {
	"term",
	"hup",
	"quit",
	"int",
	"kill",
	"winch",
    };

    ਜੇ (idx < (ਪੂਰਨ)ARRAY_ਲੰਮਾਈ(p_termkill_ਮੁੱਲਾਂ))
	ਵਾਪਸ (char_u*)p_termkill_ਮੁੱਲਾਂ[idx];
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Command-line expansion for :terminal [options]
 */
    ਪੂਰਨ
expand_terminal_opt(
	char_u	    *pat,
	expand_T    *xp,
	regmatch_T  *rmp,
	char_u	    ***matches,
	ਪੂਰਨ	    *ਅੰਕ_Matches)
{
    ਜੇ (xp->xp_ਨਮੂਨਾ > xp->xp_ਸਤਰ && *(xp->xp_ਨਮੂਨਾ-1) == '=')
    {
	char_u *(*cb)(expand_T *, ਪੂਰਨ) = ਨਿਰਾ;

	char_u *name_ਸਿਰਾ = xp->xp_ਨਮੂਨਾ - 1;
	ਜੇ (name_ਸਿਰਾ - xp->xp_ਸਤਰ >= 4
		&& STRNCMP(name_ਸਿਰਾ - 4, "kill", 4) == 0)
	    cb = get_termkill_name;

	ਜੇ (cb != ਨਿਰਾ)
	{
	    ਵਾਪਸ ExpandGeneric(
		    pat,
		    xp,
		    rmp,
		    matches,
		    ਅੰਕ_Matches,
		    cb,
		    FALSE);
	}
	ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ ExpandGeneric(
	    pat,
	    xp,
	    rmp,
	    matches,
	    ਅੰਕ_Matches,
	    get_terminalopt_name,
	    FALSE);
}

#ਜੇ defined(FEAT_SESSION)
/*
 * Write a :terminal command to the session file to restore the terminal in
 * window "wp".
 * Return FAIL if writing fails.
 */
    ਪੂਰਨ
ਪਦ_write_session(FILE *fd, win_T *wp, hashtab_T *terminal_bufs)
{
    ਅਚਲ ਪੂਰਨ	bufnr = wp->w_ਬਫਰ->b_fnum;
    ਪਦ_T	*ਪਦ = wp->w_ਬਫਰ->b_ਪਦ;

    ਜੇ (terminal_bufs != ਨਿਰਾ && wp->w_ਬਫਰ->b_nwindows > 1)
    {
	// There are multiple views into this terminal buffer. We don't want to
	// create the terminal multiple times. If it's the first time, create,
	// otherwise link to the first buffer.
	ਅੱਖਰ	    id_as_str[NUMBUFLEN];
	hashitem_T  *ਇੰਦਰਾਜ;

	vim_snprintf(id_as_str, ਆਕਾਰ(id_as_str), "%d", bufnr);

	ਇੰਦਰਾਜ = hash_ਲੱਭੋ(terminal_bufs, (char_u *)id_as_str);
	ਜੇ (!HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(ਇੰਦਰਾਜ))
	{
	    // we've already opened this terminal buffer
	    ਜੇ (fprintf(fd, "execute 'buffer ' .. term_buf_%d", bufnr) < 0)
		ਵਾਪਸ FAIL;
	    ਵਾਪਸ put_eol(fd);
	}
    }

    // Create the terminal and run the command.  This is not without
    // risk, but let's assume the user only creates a session when this
    // will be OK.
    ਜੇ (fprintf(fd, "exe ':terminal ++curwin"
		" ++cols=' .. ((&columns * %d + %ld) / %ld)"
		" .. ' ++rows=' .. ((&lines * %d + %ld) / %ld) ",
		ਪਦ->tl_ਖਾਨੇ_ਗਿਣਤੀ, Columns / 2, Columns,
		ਪਦ->tl_ਪੰਕਤੀਆਂ, ਪੰਕਤੀਆਂ / 2, ਪੰਕਤੀਆਂ) < 0)
	ਵਾਪਸ FAIL;
# ifdef MSWIN
    ਜੇ (fprintf(fd, ".. ' ++type=%s' ", ਪਦ->tl_ਕੰਮ->jv_tty_type) < 0)
	ਵਾਪਸ FAIL;
# endif
    ਜੇ (ਪਦ->tl_command != ਨਿਰਾ)
    {
	char_u *quoted_command = string_quote(ਪਦ->tl_command, FALSE);
	ਜੇ (quoted_command == ਨਿਰਾ)
	    ਵਾਪਸ FAIL;

	ਪੂਰਨ ret = fputs(".. ' ' .. ", fd) < 0
		    || fputs((ਅੱਖਰ *)quoted_command, fd) < 0;
	vim_free(quoted_command);
	ਜੇ (ret)
	    ਵਾਪਸ FAIL;
    }
    ਜੇ (put_eol(fd) != OK)
	ਵਾਪਸ FAIL;

    ਜੇ (fprintf(fd, "var term_buf_%d: number = bufnr()", bufnr) < 0)
	ਵਾਪਸ FAIL;

    ਜੇ (terminal_bufs != ਨਿਰਾ && wp->w_ਬਫਰ->b_nwindows > 1)
    {
	ਅੱਖਰ *hash_ਕੁੰਜੀ = alloc(NUMBUFLEN);

	vim_snprintf(hash_ਕੁੰਜੀ, NUMBUFLEN, "%d", bufnr);
	hash_ਜੋੜੋ(terminal_bufs, (char_u *)hash_ਕੁੰਜੀ, "terminal session");
    }

    ਵਾਪਸ put_eol(fd);
}

/*
 * Return TRUE if "buf" has a terminal that should be restored.
 */
    ਪੂਰਨ
ਪਦ_should_restore(buf_T *buf)
{
    ਪਦ_T	*ਪਦ = buf->b_ਪਦ;

    ਵਾਪਸ ਪਦ != ਨਿਰਾ && (ਪਦ->tl_command == ਨਿਰਾ
				     || STRCMP(ਪਦ->tl_command, "NONE") != 0);
}
#ਜੇ_ਅੰਤ

/*
 * Free the scrollback buffer for "term".
 */
    ਸਥਿਰ ਖਾਲੀ
free_scrollback(ਪਦ_T *ਪਦ)
{
    ਪੂਰਨ i;

    ਲਈ (i = 0; i < ਪਦ->tl_scrollback.ga_len; ++i)
	vim_free(((sb_ਸਤਰ_T *)ਪਦ->tl_scrollback.ga_ਅੰਕੜੇ + i)->sb_cells);
    ga_clear(&ਪਦ->tl_scrollback);
    ਲਈ (i = 0; i < ਪਦ->tl_scrollback_postponed.ga_len; ++i)
	vim_free(((sb_ਸਤਰ_T *)ਪਦ->tl_scrollback_postponed.ga_ਅੰਕੜੇ + i)->sb_cells);
    ga_clear(&ਪਦ->tl_scrollback_postponed);
}


// Terminals that need to be freed soon.
ਸਥਿਰ ਪਦ_T	*terminals_ਮੰਜ਼ਿਲ_free = ਨਿਰਾ;

/*
 * Free a terminal and everything it refers to.
 * Kills the job if there is one.
 * Called when wiping out a buffer.
 * The actual terminal structure is freed later in free_unused_terminals(),
 * because callbacks may wipe out a buffer while the terminal is still
 * referenced.
 */
    ਖਾਲੀ
free_terminal(buf_T *buf)
{
    ਪਦ_T	*ਪਦ = buf->b_ਪਦ;
    ਪਦ_T	*tp;

    ਜੇ (ਪਦ == ਨਿਰਾ)
	ਵਾਪਸ;

    // Unlink the terminal form the list of terminals.
    ਜੇ (ਪਹਿਲਾ_ਪਦ == ਪਦ)
	ਪਹਿਲਾ_ਪਦ = ਪਦ->tl_ਅਗਲਾ;
    ਨਹੀਂਤਾਂ
	ਲਈ (tp = ਪਹਿਲਾ_ਪਦ; tp->tl_ਅਗਲਾ != ਨਿਰਾ; tp = tp->tl_ਅਗਲਾ)
	    ਜੇ (tp->tl_ਅਗਲਾ == ਪਦ)
	    {
		tp->tl_ਅਗਲਾ = ਪਦ->tl_ਅਗਲਾ;
		ਰੋਕੋ;
	    }

    ਜੇ (ਪਦ->tl_ਕੰਮ != ਨਿਰਾ)
    {
	ਜੇ (ਪਦ->tl_ਕੰਮ->jv_ਸਥਿਤੀ != ਕੰਮ_ENDED
		&& ਪਦ->tl_ਕੰਮ->jv_ਸਥਿਤੀ != ਕੰਮ_ਮੁਕੰਮਲ
		&& ਪਦ->tl_ਕੰਮ->jv_ਸਥਿਤੀ != ਕੰਮ_FAILED)
	    ਕੰਮ_stop(ਪਦ->tl_ਕੰਮ, ਨਿਰਾ, "kill");
	ਕੰਮ_unref(ਪਦ->tl_ਕੰਮ);
    }
    ਪਦ->tl_ਅਗਲਾ = terminals_ਮੰਜ਼ਿਲ_free;
    terminals_ਮੰਜ਼ਿਲ_free = ਪਦ;

    buf->b_ਪਦ = ਨਿਰਾ;
    ਜੇ (ਵਿੱਚ_terminal_loop == ਪਦ)
	ਵਿੱਚ_terminal_loop = ਨਿਰਾ;
}

    ਖਾਲੀ
free_ਅਣਵਰਤਿਆ_terminals(ਖਾਲੀ)
{
    ਜਦੋਂ (terminals_ਮੰਜ਼ਿਲ_free != ਨਿਰਾ)
    {
	ਪਦ_T	    *ਪਦ = terminals_ਮੰਜ਼ਿਲ_free;

	terminals_ਮੰਜ਼ਿਲ_free = ਪਦ->tl_ਅਗਲਾ;

	free_scrollback(ਪਦ);
	ga_clear(&ਪਦ->tl_osc_buf);

	ਪਦ_free_vterm(ਪਦ);
	vim_free(ਪਦ->tl_api);
	vim_free(ਪਦ->tl_title);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SESSION
	vim_free(ਪਦ->tl_command);
#ਜੇ_ਅੰਤ
	vim_free(ਪਦ->tl_kill);
	vim_free(ਪਦ->tl_ਸਥਿਤੀ_ਪਾਠ);
	vim_free(ਪਦ->tl_opencmd);
	vim_free(ਪਦ->tl_eof_chars);
	vim_free(ਪਦ->tl_arg0_cmd);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
	ਜੇ (ਪਦ->tl_ਬਾਹਰਲਾ_fd != ਨਿਰਾ)
	    fclose(ਪਦ->tl_ਬਾਹਰਲਾ_fd);
#ਜੇ_ਅੰਤ
	vim_free(ਪਦ->tl_highlight_name);
	vim_free(ਪਦ->tl_cursor_color);
	vim_free(ਪਦ->tl_palette);
	vim_free(ਪਦ);
    }
}

/*
 * Get the part that is connected to the tty. Normally this is PART_IN, but
 * when writing buffer lines to the job it can be another.  This makes it
 * possible to do "1,5term vim -".
 */
    ਸਥਿਰ ch_part_T
get_tty_part(ਪਦ_T *ਪਦ ਅਣਵਰਤਿਆ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
    ch_part_T	parts[3] = {PART_ਵਿੱਚ, PART_ਬਾਹਰਲਾ, PART_ERR};
    ਪੂਰਨ		i;

    ਲਈ (i = 0; i < 3; ++i)
    {
	ਪੂਰਨ fd = ਪਦ->tl_ਕੰਮ->jv_channel->ch_part[parts[i]].ch_fd;

	ਜੇ (mch_isatty(fd))
	    ਵਾਪਸ parts[i];
    }
#ਜੇ_ਅੰਤ
    ਵਾਪਸ PART_ਵਿੱਚ;
}

/*
 * Read any vterm output and send it on the channel.
 */
    ਸਥਿਰ ਖਾਲੀ
ਪਦ_forward_output(ਪਦ_T *ਪਦ)
{
    VTerm *vterm = ਪਦ->tl_vterm;
    ਅੱਖਰ   buf[ਕੁੰਜੀ_BUF_LEN];
    size_t curlen = vterm_output_read(vterm, buf, ਕੁੰਜੀ_BUF_LEN);

    ਜੇ (curlen > 0)
	channel_send(ਪਦ->tl_ਕੰਮ->jv_channel, get_tty_part(ਪਦ),
					     (char_u *)buf, (ਪੂਰਨ)curlen, ਨਿਰਾ);
}

/*
 * Write job output "msg[len]" to the vterm.
 */
    ਸਥਿਰ ਖਾਲੀ
ਪਦ_write_ਕੰਮ_output(ਪਦ_T *ਪਦ, char_u *ਸੁਨੇਹਾ_arg, size_t len_arg)
{
    char_u	*ਸੁਨੇਹਾ = ਸੁਨੇਹਾ_arg;
    size_t	len = len_arg;
    VTerm	*vterm = ਪਦ->tl_vterm;
    size_t	prevlen = vterm_output_get_ਬਫਰ_current(vterm);
    size_t	ਹੱਦਬੰਦੀ = ਪਦ->tl_ਬਫਰ->b_p_twsl * ਪਦ->tl_ਖਾਨੇ_ਗਿਣਤੀ * 3;

    // Limit the length to 'termwinscroll' * cols * 3 bytes.  Keep the text at
    // the end.
    ਜੇ (len > ਹੱਦਬੰਦੀ)
    {
	char_u *p = ਸੁਨੇਹਾ + len - ਹੱਦਬੰਦੀ;

	p -= (*mb_ਸਿਰ_off)(ਸੁਨੇਹਾ, p);
	len -= p - ਸੁਨੇਹਾ;
	ਸੁਨੇਹਾ = p;
    }

    vterm_ਨਿਵੇਸ਼_write(vterm, (ਅੱਖਰ *)ਸੁਨੇਹਾ, len);

    // flush vterm buffer when vterm responded to control sequence
    ਜੇ (prevlen != vterm_output_get_ਬਫਰ_current(vterm))
	ਪਦ_forward_output(ਪਦ);

    // this invokes the damage callbacks
    vterm_screen_flush_damage(vterm_obtain_screen(vterm));
}

    ਸਥਿਰ ਖਾਲੀ
position_cursor(win_T *wp, VTermPos *pos)
{
    wp->w_wrow = MIN(pos->ਪੰਕਤੀ, MAX(0, wp->w_ਉਚਾਈ - 1));
    wp->w_wcol = MIN(pos->ਖਾਨਾ, MAX(0, wp->w_ਚੌੜਾਈ - 1));
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    ਜੇ (popup_is_popup(wp))
    {
	wp->w_wrow += popup_ਸਿਖਰ_extra(wp);
	wp->w_wcol += popup_ਖੱਬਾ_extra(wp);
	wp->w_flags |= WFLAG_WCOL_OFF_ADDED | WFLAG_WROW_OFF_ADDED;
    }
    ਨਹੀਂਤਾਂ
	wp->w_flags &= ~(WFLAG_WCOL_OFF_ADDED | WFLAG_WROW_OFF_ADDED);
#ਜੇ_ਅੰਤ
    wp->w_valid |= (VALID_WCOL|VALID_WROW);
}

    ਸਥਿਰ ਖਾਲੀ
update_cursor(ਪਦ_T *ਪਦ, ਪੂਰਨ redraw)
{
    ਜੇ (ਪਦ->tl_ਸਾਧਾਰਨ_ਬਹੁਲਕ)
	ਵਾਪਸ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    ਜੇ (ਪਦ->tl_system)
	windgoto(ਪਦ->tl_cursor_pos.ਪੰਕਤੀ + ਪਦ->tl_toprow,
		firstwin->w_wincol + ਪਦ->tl_cursor_pos.ਖਾਨਾ);
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
    ਜੇ (!ਪਦ_ਕੰਮ_running(ਪਦ))
	// avoid the cursor positioned below the last used line
	setcursor();
    ਨਹੀਂਤਾਂ
    {
	// do not use the window cursor position
	position_cursor(curwin, &curbuf->b_ਪਦ->tl_cursor_pos);
	windgoto(W_WINROW(curwin) + curwin->w_wrow,
		 curwin->w_wincol + curwin->w_wcol);
    }
    ਜੇ (redraw)
    {
	aco_save_T	aco;

	ਜੇ (ਪਦ->tl_ਬਫਰ == curbuf && ਪਦ->tl_cursor_visible)
	    cursor_on();
	ਬਾਹਰਲਾ_flush();
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
	ਜੇ (gui.ਵਿੱਚ_use)
	{
	    gui_update_cursor(FALSE, FALSE);
	    gui_mch_flush();
	}
#ਜੇ_ਅੰਤ
	// Make sure an invoked autocmd doesn't delete the buffer (and the
	// terminal) under our fingers.
	++ਪਦ->tl_ਬਫਰ->b_locked;

	// save and restore curwin and curbuf, in case the autocmd changes them
	aucmd_prepbuf(&aco, curbuf);
	apply_autocmds(EVENT_TEXTCHANGEDT, ਨਿਰਾ, ਨਿਰਾ, FALSE, ਪਦ->tl_ਬਫਰ);
	aucmd_restbuf(&aco);

	--ਪਦ->tl_ਬਫਰ->b_locked;
    }
}

/*
 * Find the location of a scrollbackline in the buffer
 */
    ਸਥਿਰ ਖਾਲੀ
scrollbackline_pos_ਵਿੱਚ_buf(ਪਦ_T *ਪਦ, ਪੂਰਨ ਪੰਕਤੀ, linenr_T *lnum, ਪੂਰਨ *ਸ਼ੁਰੂ_ਖਾਨਾ, size_t *ਸ਼ੁਰੂ_pos)
{
    sb_ਸਤਰ_T	*ਸਤਰਾਂ = (sb_ਸਤਰ_T *)ਪਦ->tl_scrollback.ga_ਅੰਕੜੇ;
    linenr_T    calc_lnum = ਪਦ->tl_ਬਫਰ_scrolled;
    size_t      calc_pos = 0;
    ਪੂਰਨ         calc_ਖਾਨਾ = 0;
    ਪੂਰਨ         i;

    ਜੇ (ਪੰਕਤੀ < 0 || ਪੰਕਤੀ >= ਪਦ->tl_scrollback.ga_len)
	ਵਾਪਸ;

    ਜੇ (ਪੰਕਤੀ > ਪਦ->tl_scrollback_scrolled)
    {
	// Lookback how far along in the top line we are
	ਲਈ (i = ਪਦ->tl_scrollback_scrolled + 1; i > 0 && ਸਤਰਾਂ[i].continuation; --i)
	{
	    calc_pos += ਸਤਰਾਂ[i - 1].sb_ਬਾਈਟਾਂ;
	    calc_ਖਾਨਾ += ਸਤਰਾਂ[i - 1].sb_ਖਾਨੇ_ਗਿਣਤੀ;
	}
	i = ਪਦ->tl_scrollback_scrolled + 1;
	calc_lnum = ਪਦ->tl_ਬਫਰ_scrolled + 1;
	// Do not count this line's bytes/cols twice
	ਜੇ (ਸਤਰਾਂ[i].continuation)
	    ++i;
    }
    ਨਹੀਂਤਾਂ
    {
	i = 1;
	calc_lnum = 1;
    }

    ਲਈ (; i <= ਪੰਕਤੀ; ++i)
    {
	ਜੇ (!ਸਤਰਾਂ[i].continuation)
	{
	    ++calc_lnum;
	    calc_pos = 0;
	    calc_ਖਾਨਾ = 0;
	}
	ਨਹੀਂਤਾਂ
	{
	    calc_pos += ਸਤਰਾਂ[i - 1].sb_ਬਾਈਟਾਂ;
	    calc_ਖਾਨਾ += ਸਤਰਾਂ[i - 1].sb_ਖਾਨੇ_ਗਿਣਤੀ;
	}
    }

    *lnum = calc_lnum;
    ਜੇ (ਸ਼ੁਰੂ_ਖਾਨਾ)
	*ਸ਼ੁਰੂ_ਖਾਨਾ = calc_ਖਾਨਾ;
    ਜੇ (ਸ਼ੁਰੂ_pos)
	*ਸ਼ੁਰੂ_pos = calc_pos;
}

/*
 * Find the location of a buffer line in the scrollback
 */
    ਸਥਿਰ ਖਾਲੀ
bufline_pos_ਵਿੱਚ_scrollback(ਪਦ_T *ਪਦ, linenr_T lnum, ਪੂਰਨ ਖਾਨਾ, ਪੂਰਨ *ਪੰਕਤੀ, ਪੂਰਨ *wrapped_ਖਾਨਾ)
{
    buf_T	*buf = ਪਦ->tl_ਬਫਰ;
    sb_ਸਤਰ_T	*ਸਤਰਾਂ = (sb_ਸਤਰ_T *)ਪਦ->tl_scrollback.ga_ਅੰਕੜੇ;
    linenr_T    calc_ਪੰਕਤੀ = ਪਦ->tl_scrollback_scrolled;
    ਪੂਰਨ         calc_ਖਾਨਾ = ਖਾਨਾ;
    linenr_T    l;

    ਜੇ (lnum > buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	ਵਾਪਸ;

    ਜੇ (lnum > ਪਦ->tl_ਬਫਰ_scrolled)
    {
	calc_ਪੰਕਤੀ = ਪਦ->tl_scrollback_scrolled;
	l = ਪਦ->tl_ਬਫਰ_scrolled + 1;

	ਜਦੋਂ (calc_ਪੰਕਤੀ < ਪਦ->tl_scrollback.ga_len && ਸਤਰਾਂ[calc_ਪੰਕਤੀ].continuation)
	    ++calc_ਪੰਕਤੀ;
    }
    ਨਹੀਂਤਾਂ
    {
	calc_ਪੰਕਤੀ = 0;
	l = 1;
    }

    ਜਦੋਂ (calc_ਪੰਕਤੀ < ਪਦ->tl_scrollback.ga_len && l < lnum)
    {
	++calc_ਪੰਕਤੀ;
	ਜੇ (!ਸਤਰਾਂ[calc_ਪੰਕਤੀ].continuation)
	    ++l;
    }

    ਜਦੋਂ (calc_ਪੰਕਤੀ + 1 < ਪਦ->tl_scrollback.ga_len && ਸਤਰਾਂ[calc_ਪੰਕਤੀ + 1].continuation
	    && calc_ਖਾਨਾ >= ਸਤਰਾਂ[calc_ਪੰਕਤੀ].sb_ਖਾਨੇ_ਗਿਣਤੀ)
    {
	calc_ਖਾਨਾ -= ਸਤਰਾਂ[calc_ਪੰਕਤੀ].sb_ਖਾਨੇ_ਗਿਣਤੀ;
	++calc_ਪੰਕਤੀ;
    }

    ਜੇ (ਪੰਕਤੀ)
	*ਪੰਕਤੀ = calc_ਪੰਕਤੀ;
    ਜੇ (wrapped_ਖਾਨਾ)
	*wrapped_ਖਾਨਾ = calc_ਖਾਨਾ;
}

/*
 * Invoked when "msg" output from a job was received.  Write it to the terminal
 * of "buffer".
 */
    ਖਾਲੀ
write_ਮੰਜ਼ਿਲ_ਪਦ(buf_T *ਬਫਰ, char_u *ਸੁਨੇਹਾ, channel_T *channel)
{
    size_t	len = STRLEN(ਸੁਨੇਹਾ);
    ਪਦ_T	*ਪਦ = ਬਫਰ->b_ਪਦ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
    // Win32: Cannot redirect output of the job, intercept it here and write to
    // the file.
    ਜੇ (ਪਦ->tl_ਬਾਹਰਲਾ_fd != ਨਿਰਾ)
    {
	ch_log(channel, "Writing %d bytes to output file", (ਪੂਰਨ)len);
	fwrite(ਸੁਨੇਹਾ, len, 1, ਪਦ->tl_ਬਾਹਰਲਾ_fd);
	ਵਾਪਸ;
    }
#ਜੇ_ਅੰਤ

    ਜੇ (ਪਦ->tl_vterm == ਨਿਰਾ)
    {
	ch_log(channel, "NOT writing %d bytes to terminal", (ਪੂਰਨ)len);
	ਵਾਪਸ;
    }
    ch_log(channel, "writing %d bytes to terminal", (ਪੂਰਨ)len);
    cursor_off();
    ਪਦ_write_ਕੰਮ_output(ਪਦ, ਸੁਨੇਹਾ, len);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    ਜੇ (ਪਦ->tl_system)
    {
	// show system output, scrolling up the screen as needed
	update_system_ਪਦ(ਪਦ);
	update_cursor(ਪਦ, TRUE);
    }
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
    // In Terminal-Normal mode we are displaying the buffer, not the terminal
    // contents, thus no screen update is needed.
    ਜੇ (!ਪਦ->tl_ਸਾਧਾਰਨ_ਬਹੁਲਕ)
    {
	// Don't use update_screen() when editing the command line, it gets
	// cleared.
	// TODO: only update once in a while.
	ch_log(ਪਦ->tl_ਕੰਮ->jv_channel, "updating screen");
	ਜੇ (ਬਫਰ == curbuf && (State & ਬਹੁਲਕ_CMDLINE) == 0)
	{
	    update_screen(UPD_VALID_NO_UPDATE);
#ਜੇ defined(FEAT_TABPANEL)
	    ਜੇ (redraw_tabpanel)
		draw_tabpanel();
#ਜੇ_ਅੰਤ
	    // update_screen() can be slow, check the terminal wasn't closed
	    // already
	    ਜੇ (ਬਫਰ == curbuf && curbuf->b_ਪਦ != ਨਿਰਾ)
		update_cursor(curbuf->b_ਪਦ, TRUE);
	}
	ਨਹੀਂਤਾਂ
	    redraw_after_callback(TRUE, FALSE);
    }
}

/*
 * Send a mouse position and click to the vterm
 */
    ਸਥਿਰ ਪੂਰਨ
ਪਦ_send_mouse(VTerm *vterm, ਪੂਰਨ button, ਪੂਰਨ pressed)
{
    VTermModifier   mod = VTERM_MOD_NONE;
    ਪੂਰਨ		    ਪੰਕਤੀ = mouse_ਪੰਕਤੀ - W_WINROW(curwin);
    ਪੂਰਨ		    ਖਾਨਾ = mouse_ਖਾਨਾ - curwin->w_wincol;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    ਜੇ (popup_is_popup(curwin))
    {
	ਪੰਕਤੀ -= popup_ਸਿਖਰ_extra(curwin);
	ਖਾਨਾ -= popup_ਖੱਬਾ_extra(curwin);
    }
#ਜੇ_ਅੰਤ
    vterm_mouse_move(vterm, ਪੰਕਤੀ, ਖਾਨਾ, mod);
    ਜੇ (button != 0)
	vterm_mouse_button(vterm, button, pressed, mod);
    ਵਾਪਸ TRUE;
}

ਸਥਿਰ ਪੂਰਨ enter_mouse_ਖਾਨਾ = -1;
ਸਥਿਰ ਪੂਰਨ enter_mouse_ਪੰਕਤੀ = -1;

/*
 * Handle a mouse click, drag or release.
 * Return TRUE when a mouse event is sent to the terminal.
 */
    ਸਥਿਰ ਪੂਰਨ
ਪਦ_mouse_click(VTerm *vterm, ਪੂਰਨ ਕੁੰਜੀ)
{
#ਜੇ defined(FEAT_CLIPBOARD)
    // For modeless selection mouse drag and release events are ignored, unless
    // they are preceded with a mouse down event
    ਸਥਿਰ ਪੂਰਨ	    ignore_drag_release = TRUE;
    VTermState	    *state;
    VTermMouseState mouse_state;


    state = vterm_obtain_state(vterm);
    ਜੇ (state == ਨਿਰਾ)
	ਵਾਪਸ FALSE;

    vterm_state_get_mousestate(state, &mouse_state);
    ਜੇ (mouse_state.flags == 0)
    {
	// Terminal is not using the mouse, use modeless selection.
	ਚੋਣ (ਕੁੰਜੀ)
	{
	ਹਾਲਤ K_LEFTDRAG:
	ਹਾਲਤ K_LEFTRELEASE:
	ਹਾਲਤ K_RIGHTDRAG:
	ਹਾਲਤ K_RIGHTRELEASE:
		// Ignore drag and release events when the button-down wasn't
		// seen before.
		ਜੇ (ignore_drag_release)
		{
		    ਪੂਰਨ save_mouse_ਖਾਨਾ, save_mouse_ਪੰਕਤੀ;

		    ਜੇ (enter_mouse_ਖਾਨਾ < 0)
			ਰੋਕੋ;

		    // mouse click in the window gave us focus, handle that
		    // click now
		    save_mouse_ਖਾਨਾ = mouse_ਖਾਨਾ;
		    save_mouse_ਪੰਕਤੀ = mouse_ਪੰਕਤੀ;
		    mouse_ਖਾਨਾ = enter_mouse_ਖਾਨਾ;
		    mouse_ਪੰਕਤੀ = enter_mouse_ਪੰਕਤੀ;
		    clip_modeless(MOUSE_ਖੱਬਾ, TRUE, FALSE);
		    mouse_ਖਾਨਾ = save_mouse_ਖਾਨਾ;
		    mouse_ਪੰਕਤੀ = save_mouse_ਪੰਕਤੀ;
		}
		// FALLTHROUGH
	ਹਾਲਤ K_LEFTMOUSE:
	ਹਾਲਤ K_RIGHTMOUSE:
		ਜੇ (ਕੁੰਜੀ == K_LEFTRELEASE || ਕੁੰਜੀ == K_RIGHTRELEASE)
		    ignore_drag_release = TRUE;
		ਨਹੀਂਤਾਂ
		    ignore_drag_release = FALSE;
		// Should we call mouse_has() here?
		ਜੇ (clip_ਤਾਰਾ.ਉਪਲਬਧ)
		{
		    ਪੂਰਨ	    button, is_click, is_drag;

		    button = get_mouse_button(KEY2TERMCAP1(ਕੁੰਜੀ),
							 &is_click, &is_drag);
		    ਜੇ (mouse_model_popup() && button == MOUSE_ਖੱਬਾ
					       && (mod_ਨਕਾਬ & MOD_ਨਕਾਬ_ਖਿਸਕਾਓ))
		    {
			// Translate shift-left to right button.
			button = MOUSE_ਸੱਜਾ;
			mod_ਨਕਾਬ &= ~MOD_ਨਕਾਬ_ਖਿਸਕਾਓ;
		    }
		    clip_modeless(button, is_click, is_drag);
		}
		ਰੋਕੋ;

	ਹਾਲਤ K_MIDDLEMOUSE:
		ਜੇ (clip_ਤਾਰਾ.ਉਪਲਬਧ)
		    ਪਾਓ_reg('*', TRUE);
		ਰੋਕੋ;
	}
	enter_mouse_ਖਾਨਾ = -1;
	ਵਾਪਸ FALSE;
    }
#ਜੇ_ਅੰਤ
    enter_mouse_ਖਾਨਾ = -1;

    ਚੋਣ (ਕੁੰਜੀ)
    {
	ਹਾਲਤ K_LEFTMOUSE:
	ਹਾਲਤ K_LEFTMOUSE_NM:	ਪਦ_send_mouse(vterm, 1, 1); ਰੋਕੋ;
	ਹਾਲਤ K_LEFTDRAG:	ਪਦ_send_mouse(vterm, 1, 1); ਰੋਕੋ;
	ਹਾਲਤ K_LEFTRELEASE:
	ਹਾਲਤ K_LEFTRELEASE_NM:	ਪਦ_send_mouse(vterm, 1, 0); ਰੋਕੋ;
	ਹਾਲਤ K_MOUSEMOVE:	ਪਦ_send_mouse(vterm, 0, 0); ਰੋਕੋ;
	ਹਾਲਤ K_MIDDLEMOUSE:	ਪਦ_send_mouse(vterm, 2, 1); ਰੋਕੋ;
	ਹਾਲਤ K_MIDDLEDRAG:	ਪਦ_send_mouse(vterm, 2, 1); ਰੋਕੋ;
	ਹਾਲਤ K_MIDDLERELEASE:	ਪਦ_send_mouse(vterm, 2, 0); ਰੋਕੋ;
	ਹਾਲਤ K_RIGHTMOUSE:	ਪਦ_send_mouse(vterm, 3, 1); ਰੋਕੋ;
	ਹਾਲਤ K_RIGHTDRAG:	ਪਦ_send_mouse(vterm, 3, 1); ਰੋਕੋ;
	ਹਾਲਤ K_RIGHTRELEASE:	ਪਦ_send_mouse(vterm, 3, 0); ਰੋਕੋ;
    }
    ਵਾਪਸ TRUE;
}

/*
 * Convert typed key "c" with modifiers "modmask" into bytes to send to the
 * job.
 * Return the number of bytes in "buf".
 */
    ਸਥਿਰ ਪੂਰਨ
ਪਦ_convert_ਕੁੰਜੀ(ਪਦ_T *ਪਦ, ਪੂਰਨ c, ਪੂਰਨ modmask, ਅੱਖਰ *buf)
{
    VTerm	    *vterm = ਪਦ->tl_vterm;
    VTerm_ਕੁੰਜੀ	    ਕੁੰਜੀ = VTERM_ਕੁੰਜੀ_NONE;
    VTermModifier   mod = VTERM_MOD_NONE;
    ਪੂਰਨ		    other = FALSE;

    ਚੋਣ (c)
    {
	// don't use VTERM_KEY_ENTER, it may do an unwanted conversion

	ਹਾਲਤ K_BS:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
	    // In ConPTY, we must use VTERM_KEY_BACKSPACE, otherwise it will
	    // delete one word, equivalent to Alt+Backspace.
	    ਜੇ (ਪਦ->tl_conpty)
		ਕੁੰਜੀ = VTERM_ਕੁੰਜੀ_BACKSPACE;
	    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
		// don't use VTERM_KEY_BACKSPACE, it always becomes 0x7f DEL
		c = ਪਦ_backspace_char;
	    ਰੋਕੋ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
	ਹਾਲਤ BS:
	    // In ConPTY, we must use VTERM_KEY_BACKSPACE, otherwise it will
	    // delete one word, equivalent to Alt+Backspace.
	    ਜੇ (ਪਦ->tl_conpty)
		ਕੁੰਜੀ = VTERM_ਕੁੰਜੀ_BACKSPACE;
	    ਰੋਕੋ;
#ਜੇ_ਅੰਤ

	ਹਾਲਤ ESC:		ਕੁੰਜੀ = VTERM_ਕੁੰਜੀ_ESCAPE; ਰੋਕੋ;
	ਹਾਲਤ K_DEL:		ਕੁੰਜੀ = VTERM_ਕੁੰਜੀ_DEL; ਰੋਕੋ;
	ਹਾਲਤ K_ਥੱਲੇ:		ਕੁੰਜੀ = VTERM_ਕੁੰਜੀ_ਥੱਲੇ; ਰੋਕੋ;
	ਹਾਲਤ K_S_ਥੱਲੇ:		mod = VTERM_MOD_ਖਿਸਕਾਓ;
				ਕੁੰਜੀ = VTERM_ਕੁੰਜੀ_ਥੱਲੇ; ਰੋਕੋ;
	ਹਾਲਤ K_ਸਿਰਾ:		ਕੁੰਜੀ = VTERM_ਕੁੰਜੀ_ਸਿਰਾ; ਰੋਕੋ;
	ਹਾਲਤ K_S_ਸਿਰਾ:		mod = VTERM_MOD_ਖਿਸਕਾਓ;
				ਕੁੰਜੀ = VTERM_ਕੁੰਜੀ_ਸਿਰਾ; ਰੋਕੋ;
	ਹਾਲਤ K_C_ਸਿਰਾ:		mod = VTERM_MOD_CTRL;
				ਕੁੰਜੀ = VTERM_ਕੁੰਜੀ_ਸਿਰਾ; ਰੋਕੋ;
	ਹਾਲਤ K_F10:		ਕੁੰਜੀ = VTERM_ਕੁੰਜੀ_FUNCTION(10); ਰੋਕੋ;
	ਹਾਲਤ K_F11:		ਕੁੰਜੀ = VTERM_ਕੁੰਜੀ_FUNCTION(11); ਰੋਕੋ;
	ਹਾਲਤ K_F12:		ਕੁੰਜੀ = VTERM_ਕੁੰਜੀ_FUNCTION(12); ਰੋਕੋ;
	ਹਾਲਤ K_F1:		ਕੁੰਜੀ = VTERM_ਕੁੰਜੀ_FUNCTION(1); ਰੋਕੋ;
	ਹਾਲਤ K_F2:		ਕੁੰਜੀ = VTERM_ਕੁੰਜੀ_FUNCTION(2); ਰੋਕੋ;
	ਹਾਲਤ K_F3:		ਕੁੰਜੀ = VTERM_ਕੁੰਜੀ_FUNCTION(3); ਰੋਕੋ;
	ਹਾਲਤ K_F4:		ਕੁੰਜੀ = VTERM_ਕੁੰਜੀ_FUNCTION(4); ਰੋਕੋ;
	ਹਾਲਤ K_F5:		ਕੁੰਜੀ = VTERM_ਕੁੰਜੀ_FUNCTION(5); ਰੋਕੋ;
	ਹਾਲਤ K_F6:		ਕੁੰਜੀ = VTERM_ਕੁੰਜੀ_FUNCTION(6); ਰੋਕੋ;
	ਹਾਲਤ K_F7:		ਕੁੰਜੀ = VTERM_ਕੁੰਜੀ_FUNCTION(7); ਰੋਕੋ;
	ਹਾਲਤ K_F8:		ਕੁੰਜੀ = VTERM_ਕੁੰਜੀ_FUNCTION(8); ਰੋਕੋ;
	ਹਾਲਤ K_F9:		ਕੁੰਜੀ = VTERM_ਕੁੰਜੀ_FUNCTION(9); ਰੋਕੋ;
	ਹਾਲਤ K_HOME:		ਕੁੰਜੀ = VTERM_ਕੁੰਜੀ_HOME; ਰੋਕੋ;
	ਹਾਲਤ K_S_HOME:		mod = VTERM_MOD_ਖਿਸਕਾਓ;
				ਕੁੰਜੀ = VTERM_ਕੁੰਜੀ_HOME; ਰੋਕੋ;
	ਹਾਲਤ K_C_HOME:		mod = VTERM_MOD_CTRL;
				ਕੁੰਜੀ = VTERM_ਕੁੰਜੀ_HOME; ਰੋਕੋ;
	ਹਾਲਤ K_INS:		ਕੁੰਜੀ = VTERM_ਕੁੰਜੀ_INS; ਰੋਕੋ;
	ਹਾਲਤ K_K0:		ਕੁੰਜੀ = VTERM_ਕੁੰਜੀ_KP_0; ਰੋਕੋ;
	ਹਾਲਤ K_K1:		ਕੁੰਜੀ = VTERM_ਕੁੰਜੀ_KP_1; ਰੋਕੋ;
	ਹਾਲਤ K_K2:		ਕੁੰਜੀ = VTERM_ਕੁੰਜੀ_KP_2; ਰੋਕੋ;
	ਹਾਲਤ K_K3:		ਕੁੰਜੀ = VTERM_ਕੁੰਜੀ_KP_3; ਰੋਕੋ;
	ਹਾਲਤ K_K4:		ਕੁੰਜੀ = VTERM_ਕੁੰਜੀ_KP_4; ਰੋਕੋ;
	ਹਾਲਤ K_K5:		ਕੁੰਜੀ = VTERM_ਕੁੰਜੀ_KP_5; ਰੋਕੋ;
	ਹਾਲਤ K_K6:		ਕੁੰਜੀ = VTERM_ਕੁੰਜੀ_KP_6; ਰੋਕੋ;
	ਹਾਲਤ K_K7:		ਕੁੰਜੀ = VTERM_ਕੁੰਜੀ_KP_7; ਰੋਕੋ;
	ਹਾਲਤ K_K8:		ਕੁੰਜੀ = VTERM_ਕੁੰਜੀ_KP_8; ਰੋਕੋ;
	ਹਾਲਤ K_K9:		ਕੁੰਜੀ = VTERM_ਕੁੰਜੀ_KP_9; ਰੋਕੋ;
	ਹਾਲਤ K_KDEL:		ਕੁੰਜੀ = VTERM_ਕੁੰਜੀ_DEL; ਰੋਕੋ; // TODO
	ਹਾਲਤ K_KDIVIDE:		ਕੁੰਜੀ = VTERM_ਕੁੰਜੀ_KP_DIVIDE; ਰੋਕੋ;
	ਹਾਲਤ K_KEND:		ਕੁੰਜੀ = VTERM_ਕੁੰਜੀ_KP_1; ਰੋਕੋ; // TODO
	ਹਾਲਤ K_KENTER:		ਕੁੰਜੀ = VTERM_ਕੁੰਜੀ_KP_ENTER; ਰੋਕੋ;
	ਹਾਲਤ K_KHOME:		ਕੁੰਜੀ = VTERM_ਕੁੰਜੀ_KP_7; ਰੋਕੋ; // TODO
	ਹਾਲਤ K_KINS:		ਕੁੰਜੀ = VTERM_ਕੁੰਜੀ_KP_0; ਰੋਕੋ; // TODO
	ਹਾਲਤ K_KMINUS:		ਕੁੰਜੀ = VTERM_ਕੁੰਜੀ_KP_MINUS; ਰੋਕੋ;
	ਹਾਲਤ K_KMULTIPLY:	ਕੁੰਜੀ = VTERM_ਕੁੰਜੀ_KP_MULT; ਰੋਕੋ;
	ਹਾਲਤ K_KPAGEDOWN:	ਕੁੰਜੀ = VTERM_ਕੁੰਜੀ_KP_3; ਰੋਕੋ; // TODO
	ਹਾਲਤ K_KPAGEUP:		ਕੁੰਜੀ = VTERM_ਕੁੰਜੀ_KP_9; ਰੋਕੋ; // TODO
	ਹਾਲਤ K_KPLUS:		ਕੁੰਜੀ = VTERM_ਕੁੰਜੀ_KP_PLUS; ਰੋਕੋ;
	ਹਾਲਤ K_KPOINT:		ਕੁੰਜੀ = VTERM_ਕੁੰਜੀ_KP_PERIOD; ਰੋਕੋ;
	ਹਾਲਤ K_ਖੱਬਾ:		ਕੁੰਜੀ = VTERM_ਕੁੰਜੀ_ਖੱਬਾ; ਰੋਕੋ;
	ਹਾਲਤ K_S_ਖੱਬਾ:		mod = VTERM_MOD_ਖਿਸਕਾਓ;
				ਕੁੰਜੀ = VTERM_ਕੁੰਜੀ_ਖੱਬਾ; ਰੋਕੋ;
	ਹਾਲਤ K_C_ਖੱਬਾ:		mod = VTERM_MOD_CTRL;
				ਕੁੰਜੀ = VTERM_ਕੁੰਜੀ_ਖੱਬਾ; ਰੋਕੋ;
	ਹਾਲਤ K_PAGEDOWN:	ਕੁੰਜੀ = VTERM_ਕੁੰਜੀ_PAGEDOWN; ਰੋਕੋ;
	ਹਾਲਤ K_PAGEUP:		ਕੁੰਜੀ = VTERM_ਕੁੰਜੀ_PAGEUP; ਰੋਕੋ;
	ਹਾਲਤ K_ਸੱਜਾ:		ਕੁੰਜੀ = VTERM_ਕੁੰਜੀ_ਸੱਜਾ; ਰੋਕੋ;
	ਹਾਲਤ K_S_ਸੱਜਾ:		mod = VTERM_MOD_ਖਿਸਕਾਓ;
				ਕੁੰਜੀ = VTERM_ਕੁੰਜੀ_ਸੱਜਾ; ਰੋਕੋ;
	ਹਾਲਤ K_C_ਸੱਜਾ:		mod = VTERM_MOD_CTRL;
				ਕੁੰਜੀ = VTERM_ਕੁੰਜੀ_ਸੱਜਾ; ਰੋਕੋ;
	ਹਾਲਤ K_ਉੱਪਰ:		ਕੁੰਜੀ = VTERM_ਕੁੰਜੀ_ਉੱਪਰ; ਰੋਕੋ;
	ਹਾਲਤ K_S_ਉੱਪਰ:		mod = VTERM_MOD_ਖਿਸਕਾਓ;
				ਕੁੰਜੀ = VTERM_ਕੁੰਜੀ_ਉੱਪਰ; ਰੋਕੋ;
	ਹਾਲਤ TAB:		ਕੁੰਜੀ = VTERM_ਕੁੰਜੀ_TAB; ਰੋਕੋ;
	ਹਾਲਤ K_S_TAB:		mod = VTERM_MOD_ਖਿਸਕਾਓ;
				ਕੁੰਜੀ = VTERM_ਕੁੰਜੀ_TAB; ਰੋਕੋ;

	ਹਾਲਤ K_MOUSEUP:		other = ਪਦ_send_mouse(vterm, 5, 1); ਰੋਕੋ;
	ਹਾਲਤ K_MOUSEDOWN:	other = ਪਦ_send_mouse(vterm, 4, 1); ਰੋਕੋ;
	ਹਾਲਤ K_MOUSELEFT:	other = ਪਦ_send_mouse(vterm, 7, 1); ਰੋਕੋ;
	ਹਾਲਤ K_MOUSERIGHT:	other = ਪਦ_send_mouse(vterm, 6, 1); ਰੋਕੋ;

	ਹਾਲਤ K_LEFTMOUSE:
	ਹਾਲਤ K_LEFTMOUSE_NM:
	ਹਾਲਤ K_LEFTDRAG:
	ਹਾਲਤ K_LEFTRELEASE:
	ਹਾਲਤ K_LEFTRELEASE_NM:
	ਹਾਲਤ K_MOUSEMOVE:
	ਹਾਲਤ K_MIDDLEMOUSE:
	ਹਾਲਤ K_MIDDLEDRAG:
	ਹਾਲਤ K_MIDDLERELEASE:
	ਹਾਲਤ K_RIGHTMOUSE:
	ਹਾਲਤ K_RIGHTDRAG:
	ਹਾਲਤ K_RIGHTRELEASE:	ਜੇ (!ਪਦ_mouse_click(vterm, c))
				    ਵਾਪਸ 0;
				other = TRUE;
				ਰੋਕੋ;

	ਹਾਲਤ K_X1MOUSE:		/* TODO */ ਵਾਪਸ 0;
	ਹਾਲਤ K_X1DRAG:		/* TODO */ ਵਾਪਸ 0;
	ਹਾਲਤ K_X1RELEASE:	/* TODO */ ਵਾਪਸ 0;
	ਹਾਲਤ K_X2MOUSE:		/* TODO */ ਵਾਪਸ 0;
	ਹਾਲਤ K_X2DRAG:		/* TODO */ ਵਾਪਸ 0;
	ਹਾਲਤ K_X2RELEASE:	/* TODO */ ਵਾਪਸ 0;

	ਹਾਲਤ K_IGNORE:		ਵਾਪਸ 0;
	ਹਾਲਤ K_NOP:		ਵਾਪਸ 0;
	ਹਾਲਤ K_UNDO:		ਵਾਪਸ 0;
	ਹਾਲਤ K_HELP:		ਵਾਪਸ 0;
	ਹਾਲਤ K_XF1:		ਕੁੰਜੀ = VTERM_ਕੁੰਜੀ_FUNCTION(1); ਰੋਕੋ;
	ਹਾਲਤ K_XF2:		ਕੁੰਜੀ = VTERM_ਕੁੰਜੀ_FUNCTION(2); ਰੋਕੋ;
	ਹਾਲਤ K_XF3:		ਕੁੰਜੀ = VTERM_ਕੁੰਜੀ_FUNCTION(3); ਰੋਕੋ;
	ਹਾਲਤ K_XF4:		ਕੁੰਜੀ = VTERM_ਕੁੰਜੀ_FUNCTION(4); ਰੋਕੋ;
	ਹਾਲਤ K_SELECT:		ਵਾਪਸ 0;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
	ਹਾਲਤ K_VER_SCROLLBAR:	ਵਾਪਸ 0;
	ਹਾਲਤ K_HOR_SCROLLBAR:	ਵਾਪਸ 0;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_TABLINE
	ਹਾਲਤ K_TABLINE:		ਵਾਪਸ 0;
	ਹਾਲਤ K_TABMENU:		ਵਾਪਸ 0;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_NETBEANS_INTG
	ਹਾਲਤ K_F21:		ਕੁੰਜੀ = VTERM_ਕੁੰਜੀ_FUNCTION(21); ਰੋਕੋ;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DND
	ਹਾਲਤ K_DROP:		ਵਾਪਸ 0;
#ਜੇ_ਅੰਤ
	ਹਾਲਤ K_CURSORHOLD:	ਵਾਪਸ 0;
	ਹਾਲਤ K_PS:		vterm_keyboard_ਸ਼ੁਰੂ_paste(vterm);
				other = TRUE;
				ਰੋਕੋ;
	ਹਾਲਤ K_PE:		vterm_keyboard_ਸਿਰਾ_paste(vterm);
				other = TRUE;
				ਰੋਕੋ;
    }

    // add modifiers for the typed key
    ਜੇ (modmask & MOD_ਨਕਾਬ_ਖਿਸਕਾਓ)
	mod |= VTERM_MOD_ਖਿਸਕਾਓ;
    ਜੇ (modmask & MOD_ਨਕਾਬ_CTRL)
	mod |= VTERM_MOD_CTRL;
    ਜੇ (modmask & (MOD_ਨਕਾਬ_ALT | MOD_ਨਕਾਬ_META))
	mod |= VTERM_MOD_ALT;

    // Ctrl-Shift-i may have the key "I" instead of "i", but for the kitty
    // keyboard protocol should use "i".  Applies to all ascii letters.
    ਜੇ (ASCII_ISUPPER(c)
	    && vterm_is_kitty_keyboard(vterm)
	    && mod == (VTERM_MOD_CTRL | VTERM_MOD_ਖਿਸਕਾਓ))
	c = TOLOWER_ASC(c);

    /*
     * Convert special keys to vterm keys:
     * - Write keys to vterm: vterm_keyboard_key()
     * - Write output to channel.
     */
    ਜੇ (ਕੁੰਜੀ != VTERM_ਕੁੰਜੀ_NONE)
	// Special key, let vterm convert it.
	vterm_keyboard_ਕੁੰਜੀ(vterm, ਕੁੰਜੀ, mod);
    ਨਹੀਂਤਾਂ ਜੇ (!other)
	// Normal character, let vterm convert it.
	vterm_keyboard_unichar(vterm, c, mod);

    // Read back the converted escape sequence.
    ਵਾਪਸ (ਪੂਰਨ)vterm_output_read(vterm, buf, ਕੁੰਜੀ_BUF_LEN);
}

/*
 * Return TRUE if the job for "term" is still running.
 * If "check_job_status" is TRUE update the job status.
 * NOTE: "term" may be freed by callbacks.
 */
    ਸਥਿਰ ਪੂਰਨ
ਪਦ_ਕੰਮ_running_check(ਪਦ_T *ਪਦ, ਪੂਰਨ check_ਕੰਮ_ਸਥਿਤੀ)
{
    // Also consider the job finished when the channel is closed, to avoid a
    // race condition when updating the title.
    ਜੇ (ਪਦ == ਨਿਰਾ
	|| ਪਦ->tl_ਕੰਮ == ਨਿਰਾ
	|| !channel_is_open(ਪਦ->tl_ਕੰਮ->jv_channel))
	ਵਾਪਸ FALSE;

    ਕੰਮ_T *ਕੰਮ = ਪਦ->tl_ਕੰਮ;

    // Careful: Checking the job status may invoke callbacks, which close
    // the buffer and terminate "term".  However, "job" will not be freed
    // yet.
    ਜੇ (check_ਕੰਮ_ਸਥਿਤੀ)
	ਕੰਮ_ਸਥਿਤੀ(ਕੰਮ);
    ਵਾਪਸ (ਕੰਮ->jv_ਸਥਿਤੀ == ਕੰਮ_STARTED
	    || (ਕੰਮ->jv_channel != ਨਿਰਾ && ਕੰਮ->jv_channel->ch_keep_open));
}

/*
 * Return TRUE if the job for "term" is still running.
 */
    ਪੂਰਨ
ਪਦ_ਕੰਮ_running(ਪਦ_T *ਪਦ)
{
    ਵਾਪਸ ਪਦ_ਕੰਮ_running_check(ਪਦ, FALSE);
}

/*
 * Return TRUE if the job for "term" is still running, ignoring the job was
 * "NONE".
 */
    ਪੂਰਨ
ਪਦ_ਕੰਮ_running_not_none(ਪਦ_T *ਪਦ)
{
    ਵਾਪਸ ਪਦ_ਕੰਮ_running(ਪਦ) && !ਪਦ_none_open(ਪਦ);
}

/*
 * Return TRUE if "term" has an active channel and used ":term NONE".
 */
    ਪੂਰਨ
ਪਦ_none_open(ਪਦ_T *ਪਦ)
{
    // Also consider the job finished when the channel is closed, to avoid a
    // race condition when updating the title.
    ਵਾਪਸ ਪਦ != ਨਿਰਾ
	&& ਪਦ->tl_ਕੰਮ != ਨਿਰਾ
	&& channel_is_open(ਪਦ->tl_ਕੰਮ->jv_channel)
	&& ਪਦ->tl_ਕੰਮ->jv_channel->ch_keep_open;
}

//
// Used to confirm whether we would like to kill a terminal.
// Return OK when the user confirms to kill it.
// Return FAIL if the user selects otherwise.
//
    ਪੂਰਨ
ਪਦ_confirm_stop(buf_T *buf)
{
    char_u	buff[DIALOG_ਸੁਨੇਹਾ_ਮਾਪ];
    ਪੂਰਨ	ret;

    dialog_ਸੁਨੇਹਾ(buff, _("Kill job in \"%s\"?"), buf_get_fname(buf));
    ret = vim_dialog_yesno(VIM_QUESTION, ਨਿਰਾ, buff, 1);
    ਜੇ (ret == VIM_YES)
	ਵਾਪਸ OK;
    ਨਹੀਂਤਾਂ
	ਵਾਪਸ FAIL;
}

/*
 * Used when exiting: kill the job in "buf" if so desired.
 * Return OK when the job finished.
 * Return FAIL when the job is still running.
 */
    ਪੂਰਨ
ਪਦ_try_stop_ਕੰਮ(buf_T *buf)
{
    ਪੂਰਨ	    ਗਿਣਤੀ_2;
    ਅੱਖਰ    *how = (ਅੱਖਰ *)buf->b_ਪਦ->tl_kill;

#ਜੇ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
    ਜੇ ((how == ਨਿਰਾ || *how == NUL)
			  && (p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)))
    {
	ਜੇ (ਪਦ_confirm_stop(buf) == OK)
	    how = "kill";
	ਨਹੀਂਤਾਂ
	    ਵਾਪਸ FAIL;
    }
#ਜੇ_ਅੰਤ
    ਜੇ (how == ਨਿਰਾ || *how == NUL)
	ਵਾਪਸ FAIL;

    ਕੰਮ_stop(buf->b_ਪਦ->tl_ਕੰਮ, ਨਿਰਾ, how);

    // wait for up to a second for the job to die
    ਲਈ (ਗਿਣਤੀ_2 = 0; ਗਿਣਤੀ_2 < 100; ++ਗਿਣਤੀ_2)
    {
	ਕੰਮ_T *ਕੰਮ;

	// buffer, terminal and job may be cleaned up while waiting
	ਜੇ (!buf_valid(buf)
		|| buf->b_ਪਦ == ਨਿਰਾ
		|| buf->b_ਪਦ->tl_ਕੰਮ == ਨਿਰਾ)
	    ਵਾਪਸ OK;
	ਕੰਮ = buf->b_ਪਦ->tl_ਕੰਮ;

	// Call job_status() to update jv_status. It may cause the job to be
	// cleaned up but it won't be freed.
	ਕੰਮ_ਸਥਿਤੀ(ਕੰਮ);
	ਜੇ (ਕੰਮ->jv_ਸਥਿਤੀ >= ਕੰਮ_ENDED)
	    ਵਾਪਸ OK;

	ui_delay(10L, TRUE);
	ਪਦ_flush_messages();
    }
    ਵਾਪਸ FAIL;
}

/*
 * Add the last line of the scrollback buffer to the buffer in the window.
 */
    ਸਥਿਰ ਖਾਲੀ
ਜੋੜੋ_scrollback_ਸਤਰ_ਮੰਜ਼ਿਲ_ਬਫਰ(ਪਦ_T *ਪਦ, char_u *ਪਾਠ, ਪੂਰਨ len, ਪੂਰਨ append)
{
    buf_T	*buf = ਪਦ->tl_ਬਫਰ;
    ਪੂਰਨ		ਖਾਲੀ_ਥਾਵਾਂ_2 = (buf->b_ml.ml_flags & ML_ਖਾਲੀ_ਥਾਵਾਂ);
    linenr_T	lnum = buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
    char_u *tmp = ਪਾਠ;
    ਜੇ (!enc_utf8 && enc_codepage > 0)
    {
	WCHAR   *ret = ਨਿਰਾ;
	ਪੂਰਨ	ਲੰਮਾਈ = 0;

	Multi_Byte_ਮੰਜ਼ਿਲ_Wide_Char_alloc(CP_UTF8, 0, (ਅੱਖਰ*)ਪਾਠ, len + 1,
							   &ret, &ਲੰਮਾਈ);
	ਜੇ (ret != ਨਿਰਾ)
	{
	    Wide_Char_ਮੰਜ਼ਿਲ_Multi_Byte_alloc(enc_codepage, 0,
				      ret, ਲੰਮਾਈ, (ਅੱਖਰ **)&ਪਾਠ, &len, 0, 0);
	    vim_free(ret);
	}
    }
#ਜੇ_ਅੰਤ

    ਜੇ (append)
    {
	char_u *prev_ਪਾਠ = ml_get_buf(buf, lnum, FALSE);
	size_t prev_len = STRLEN(prev_ਪਾਠ);

	char_u *both = alloc(len + prev_len + 2);
	ਜੇ (both == ਨਿਰਾ)
	    ਵਾਪਸ;
	vim_strncpy(both, prev_ਪਾਠ, prev_len + 1);
	vim_strncpy(both + prev_len, ਪਾਠ, len + 1);

	curbuf = buf;
	ml_replace(lnum, both, FALSE);
	curbuf = curwin->w_ਬਫਰ;
    }
    ਨਹੀਂਤਾਂ
	ml_append_buf(buf, lnum, ਪਾਠ, len + 1, FALSE);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
    ਜੇ (tmp != ਪਾਠ)
	vim_free(ਪਾਠ);
#ਜੇ_ਅੰਤ
    ਜੇ (ਖਾਲੀ_ਥਾਵਾਂ_2)
    {
	// Delete the empty line that was in the empty buffer.
	curbuf = buf;
	ml_delete(1);
	curbuf = curwin->w_ਬਫਰ;
    }
}

    ਸਥਿਰ ਖਾਲੀ
cell2cellattr(ਅਚਲ VTermScreenCell *cell, cellattr_T *attr)
{
    attr->ਚੌੜਾਈ = cell->ਚੌੜਾਈ;
    attr->attrs = cell->attrs;
    attr->fg = cell->fg;
    attr->bg = cell->bg;
}

    ਸਥਿਰ ਪੂਰਨ
equal_celattr(cellattr_T *a, cellattr_T *b)
{
    // We only compare the RGB colors, ignoring the ANSI index and type.
    // Thus black set explicitly is equal the background black.
    ਵਾਪਸ a->fg.red == b->fg.red
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
    ਸਥਿਰ ਪੂਰਨ
ਜੋੜੋ_ਖਾਲੀ_ਥਾਵਾਂ_scrollback(ਪਦ_T *ਪਦ, cellattr_T *fill_attr, ਪੂਰਨ lnum)
{
    ਜੇ (ga_grow(&ਪਦ->tl_scrollback, 1) == FAIL)
	ਵਾਪਸ FALSE;

    sb_ਸਤਰ_T *ਸਤਰ = (sb_ਸਤਰ_T *)ਪਦ->tl_scrollback.ga_ਅੰਕੜੇ
	+ ਪਦ->tl_scrollback.ga_len;

    ਜੇ (lnum > 0)
    {
	ਪੂਰਨ i;

	ਲਈ (i = 0; i < ਪਦ->tl_scrollback.ga_len - lnum; ++i)
	{
	    *ਸਤਰ = *(ਸਤਰ - 1);
	    --ਸਤਰ;
	}
    }
    ਸਤਰ->sb_ਖਾਨੇ_ਗਿਣਤੀ = 0;
    ਸਤਰ->sb_ਬਾਈਟਾਂ = 0;
    ਸਤਰ->sb_cells = ਨਿਰਾ;
    ਸਤਰ->sb_fill_attr = *fill_attr;
    ++ਪਦ->tl_scrollback.ga_len;
    ਵਾਪਸ OK;
}

/*
 * Remove the terminal contents from the scrollback and the buffer.
 * Used before adding a new scrollback line or updating the buffer for lines
 * displayed in the terminal.
 */
    ਸਥਿਰ ਖਾਲੀ
cleanup_scrollback(ਪਦ_T *ਪਦ)
{
    sb_ਸਤਰ_T	*ਸਤਰ;
    garray_T	*gap;
    char_u	*bufline;
    size_t      bufline_ਲੰਮਾਈ;

    curbuf = ਪਦ->tl_ਬਫਰ;
    gap = &ਪਦ->tl_scrollback;
    bufline = ml_get_buf(curbuf, curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ, FALSE);
    bufline_ਲੰਮਾਈ = STRLEN(bufline);
    ਜਦੋਂ (ਪਦ->tl_scrollback_snapshot && gap->ga_len > 0)
    {
	ਸਤਰ = (sb_ਸਤਰ_T *)gap->ga_ਅੰਕੜੇ + gap->ga_len - 1;
	ਜੇ (ਸਤਰ->sb_ਬਾਈਟਾਂ < 0 || (size_t)ਸਤਰ->sb_ਬਾਈਟਾਂ > bufline_ਲੰਮਾਈ)
	    ਰੋਕੋ;
	bufline_ਲੰਮਾਈ -= ਸਤਰ->sb_ਬਾਈਟਾਂ;
	ਜੇ (!bufline_ਲੰਮਾਈ)
	{
	    ml_delete(curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ);
	    bufline = ml_get_buf(curbuf, curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ, FALSE);
	    bufline_ਲੰਮਾਈ = STRLEN(bufline);
	}
	vim_free(ਸਤਰ->sb_cells);
	--gap->ga_len;
	--ਪਦ->tl_scrollback_snapshot;
    }
    ਜੇ (bufline_ਲੰਮਾਈ < STRLEN(bufline))
    {
	char_u *shortened = vim_strnsave(bufline, bufline_ਲੰਮਾਈ);
	ml_replace(curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ, shortened, FALSE);
    }
    curbuf = curwin->w_ਬਫਰ;
    ਜੇ (curbuf == ਪਦ->tl_ਬਫਰ)
	check_cursor();
}

/*
 * Add the current lines of the terminal to scrollback and to the buffer.
 */
    ਸਥਿਰ ਖਾਲੀ
update_snapshot(ਪਦ_T *ਪਦ)
{
    VTermScreen	    *screen;
    VTermState	    *state;
    ਪੂਰਨ		    len;
    ਪੂਰਨ		    ਸਤਰਾਂ_skipped = 0;
    VTermPos	    pos;
    VTermScreenCell cell;
    cellattr_T	    fill_attr, new_fill_attr;
    cellattr_T	    *p;

    ch_log(ਪਦ->tl_ਕੰਮ == ਨਿਰਾ ? ਨਿਰਾ : ਪਦ->tl_ਕੰਮ->jv_channel,
				  "Adding terminal window snapshot to buffer");

    // First remove the lines that were appended before, they might be
    // outdated.
    cleanup_scrollback(ਪਦ);

    screen = vterm_obtain_screen(ਪਦ->tl_vterm);
    state = vterm_obtain_state(ਪਦ->tl_vterm);
    fill_attr = new_fill_attr = ਪਦ->tl_default_color;
    ਲਈ (pos.ਪੰਕਤੀ = 0; pos.ਪੰਕਤੀ < ਪਦ->tl_ਪੰਕਤੀਆਂ; ++pos.ਪੰਕਤੀ)
    {
	len = 0;
	ਲਈ (pos.ਖਾਨਾ = 0; pos.ਖਾਨਾ < ਪਦ->tl_ਖਾਨੇ_ਗਿਣਤੀ; ++pos.ਖਾਨਾ)
	    ਜੇ (vterm_screen_get_cell(screen, pos, &cell) != 0
						       && cell.chars[0] != NUL)
	    {
		len = pos.ਖਾਨਾ + 1;
		new_fill_attr = ਪਦ->tl_default_color;
	    }
	    ਨਹੀਂਤਾਂ
		// Assume the last attr is the filler attr.
		cell2cellattr(&cell, &new_fill_attr);

	ਜੇ (len == 0 && equal_celattr(&new_fill_attr, &fill_attr))
	    ++ਸਤਰਾਂ_skipped;
	ਨਹੀਂਤਾਂ
	{
	    ਜਦੋਂ (ਸਤਰਾਂ_skipped > 0)
	    {
		// Line was skipped, add an empty line.
		--ਸਤਰਾਂ_skipped;
		ਜੇ (ਜੋੜੋ_ਖਾਲੀ_ਥਾਵਾਂ_scrollback(ਪਦ, &fill_attr, 0) == OK)
		{
		    ਜੋੜੋ_scrollback_ਸਤਰ_ਮੰਜ਼ਿਲ_ਬਫਰ(ਪਦ, (char_u *)"", 0, 0);
		    ++ਪਦ->tl_scrollback_snapshot;
		}
	    }

	    ਜੇ (len == 0)
		p = ਨਿਰਾ;
	    ਨਹੀਂਤਾਂ
		p = ALLOC_MULT(cellattr_T, len);
	    ਜੇ ((p != ਨਿਰਾ || len == 0)
				     && ga_grow(&ਪਦ->tl_scrollback, 1) == OK)
	    {
		garray_T    ga;
		ਪੂਰਨ	    ਚੌੜਾਈ;
		ਅਚਲ VTerm_ਸਤਰ_Info *lineinfo;
		sb_ਸਤਰ_T   *ਸਤਰ = (sb_ਸਤਰ_T *)ਪਦ->tl_scrollback.ga_ਅੰਕੜੇ
						  + ਪਦ->tl_scrollback.ga_len;

		lineinfo = vterm_state_get_lineinfo(state, pos.ਪੰਕਤੀ);

		ga_init2(&ga, 1, 100);
		ਲਈ (pos.ਖਾਨਾ = 0; pos.ਖਾਨਾ < len; pos.ਖਾਨਾ += ਚੌੜਾਈ)
		{
		    ਜੇ (vterm_screen_get_cell(screen, pos, &cell) == 0)
		    {
			ਚੌੜਾਈ = 1;
			CLEAR_ਸੂਚਕ(p + pos.ਖਾਨਾ);
			ਜੇ (ga_grow(&ga, 1) == OK)
			    ga.ga_len += utf_char2bytes(' ',
					     (char_u *)ga.ga_ਅੰਕੜੇ + ga.ga_len);
		    }
		    ਨਹੀਂਤਾਂ
		    {
			ਚੌੜਾਈ = cell.ਚੌੜਾਈ;

			cell2cellattr(&cell, &p[pos.ਖਾਨਾ]);
			ਜੇ (ਚੌੜਾਈ == 2)
			    // second cell of double-width character has the
			    // same attributes.
			    p[pos.ਖਾਨਾ + 1] = p[pos.ਖਾਨਾ];

			// Each character can be up to 6 bytes.
			ਜੇ (ga_grow(&ga, VTERM_MAX_CHARS_ਪ੍ਰਤੀ_CELL * 6) == OK)
			{
			    ਪੂਰਨ	    i;
			    ਪੂਰਨ	    c;

			    ਲਈ (i = 0; i < VTERM_MAX_CHARS_ਪ੍ਰਤੀ_CELL &&
				    ((c = cell.chars[i]) > 0 || i == 0); ++i)
				ga.ga_len += utf_char2bytes(c == NUL ? ' ' : c,
					     (char_u *)ga.ga_ਅੰਕੜੇ + ga.ga_len);
			}
		    }
		}
		ਸਤਰ->sb_ਖਾਨੇ_ਗਿਣਤੀ = len;
		ਸਤਰ->sb_ਬਾਈਟਾਂ = ga.ga_len;
		ਸਤਰ->sb_cells = p;
		ਸਤਰ->sb_fill_attr = new_fill_attr;
		ਸਤਰ->continuation = (char_u)lineinfo->continuation;
		fill_attr = new_fill_attr;
		++ਪਦ->tl_scrollback.ga_len;
		++ਪਦ->tl_scrollback_snapshot;

		ਜੇ (ga_grow(&ga, 1) == FAIL)
		    ਜੋੜੋ_scrollback_ਸਤਰ_ਮੰਜ਼ਿਲ_ਬਫਰ(ਪਦ, (char_u *)"", 0, 0);
		ਨਹੀਂਤਾਂ
		{
		    *((char_u *)ga.ga_ਅੰਕੜੇ + ga.ga_len) = NUL;
		    ਜੋੜੋ_scrollback_ਸਤਰ_ਮੰਜ਼ਿਲ_ਬਫਰ(ਪਦ, ga.ga_ਅੰਕੜੇ, ga.ga_len,
						  lineinfo->continuation);
		}
		ga_clear(&ga);
	    }
	    ਨਹੀਂਤਾਂ
		vim_free(p);
	}
    }

    // Add trailing empty lines.
    ਲਈ (pos.ਪੰਕਤੀ = ਪਦ->tl_scrollback.ga_len;
	    pos.ਪੰਕਤੀ < ਪਦ->tl_scrollback_scrolled + ਪਦ->tl_cursor_pos.ਪੰਕਤੀ;
	    ++pos.ਪੰਕਤੀ)
    {
	ਜੇ (ਜੋੜੋ_ਖਾਲੀ_ਥਾਵਾਂ_scrollback(ਪਦ, &fill_attr, 0) == OK)
	{
	    ਜੋੜੋ_scrollback_ਸਤਰ_ਮੰਜ਼ਿਲ_ਬਫਰ(ਪਦ, (char_u *)"", 0, 0);
	    ++ਪਦ->tl_scrollback_snapshot;
	}
    }

    ਪਦ->tl_dirty_snapshot = FALSE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TIMERS
    ਪਦ->tl_timer_ਸੈੱਟ = FALSE;
#ਜੇ_ਅੰਤ
}

/*
 * Loop over all windows in the current tab, and also curwin, which is not
 * encountered when using a terminal in a popup window.
 * Return TRUE if "*wp" was set to the next window.
 */
    ਸਥਿਰ ਪੂਰਨ
for_all_windows_and_curwin(win_T **wp, ਪੂਰਨ *did_curwin)
{
    ਜੇ (*wp == ਨਿਰਾ)
	*wp = firstwin;
    ਨਹੀਂਤਾਂ ਜੇ ((*wp)->w_ਅਗਲਾ != ਨਿਰਾ)
	*wp = (*wp)->w_ਅਗਲਾ;
    ਨਹੀਂਤਾਂ ਜੇ (!*did_curwin)
	*wp = curwin;
    ਨਹੀਂਤਾਂ
	ਵਾਪਸ FALSE;
    ਜੇ (*wp == curwin)
	*did_curwin = TRUE;
    ਵਾਪਸ TRUE;
}

/*
 * If needed, add the current lines of the terminal to scrollback and to the
 * buffer.  Called after the job has ended and when switching to
 * Terminal-Normal mode.
 * When "redraw" is TRUE redraw the windows that show the terminal.
 */
    ਖਾਲੀ
may_move_terminal_ਮੰਜ਼ਿਲ_ਬਫਰ(ਪਦ_T *ਪਦ, ਪੂਰਨ redraw)
{
    ਜੇ (ਪਦ->tl_vterm == ਨਿਰਾ)
	ਵਾਪਸ;

    // Update the snapshot only if something changes or the buffer does not
    // have all the lines.
    ਜੇ (ਪਦ->tl_dirty_snapshot || ਪਦ->tl_ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ
					       <= ਪਦ->tl_ਬਫਰ_scrolled)
	update_snapshot(ਪਦ);

    ਜੇ (redraw)
    {
	win_T	    *wp = ਨਿਰਾ;
	ਪੂਰਨ	    did_curwin = FALSE;

	ਜਦੋਂ (for_all_windows_and_curwin(&wp, &did_curwin))
	{
	    ਜੇ (wp->w_ਬਫਰ == ਪਦ->tl_ਬਫਰ)
	    {
		wp->w_cursor.lnum = ਪਦ->tl_ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
		wp->w_cursor.ਖਾਨਾ = 0;
		wp->w_valid = 0;
		ਜੇ (wp->w_cursor.lnum >= wp->w_ਉਚਾਈ)
		{
		    linenr_T min_topline = wp->w_cursor.lnum - wp->w_ਉਚਾਈ + 1;

		    ਜੇ (wp->w_topline < min_topline)
			wp->w_topline = min_topline;
		}
		redraw_win_later(wp, UPD_NOT_VALID);
	    }
	}
    }
}

#ਜੇ defined(FEAT_TIMERS)
/*
 * Check if any terminal timer expired.  If so, copy text from the terminal to
 * the buffer.
 * Return the time until the next timer will expire.
 */
    ਪੂਰਨ
ਪਦ_check_timers(ਪੂਰਨ ਅਗਲਾ_due_arg, proftime_T *now)
{
    ਪਦ_T  *ਪਦ;
    ਪੂਰਨ	    ਅਗਲਾ_due = ਅਗਲਾ_due_arg;

    FOR_ALL_TERMS(ਪਦ)
    {
	ਜੇ (ਪਦ->tl_timer_ਸੈੱਟ && !ਪਦ->tl_ਸਾਧਾਰਨ_ਬਹੁਲਕ)
	{
	    ਲੰਮਾ    this_due = proftime_ਸਮਾਂ_ਖੱਬਾ(&ਪਦ->tl_timer_due, now);

	    ਜੇ (this_due <= 1)
	    {
		ਪਦ->tl_timer_ਸੈੱਟ = FALSE;
		may_move_terminal_ਮੰਜ਼ਿਲ_ਬਫਰ(ਪਦ, FALSE);
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (ਅਗਲਾ_due == -1 || ਅਗਲਾ_due > this_due)
		ਅਗਲਾ_due = this_due;
	}
    }

    ਵਾਪਸ ਅਗਲਾ_due;
}
#ਜੇ_ਅੰਤ

/*
 * When "normal_mode" is TRUE set the terminal to Terminal-Normal mode,
 * otherwise end it.
 */
    ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_terminal_ਬਹੁਲਕ(ਪਦ_T *ਪਦ, ਪੂਰਨ ਸਾਧਾਰਨ_ਬਹੁਲਕ)
{
    ਪਦ->tl_ਸਾਧਾਰਨ_ਬਹੁਲਕ = ਸਾਧਾਰਨ_ਬਹੁਲਕ;
    may_trigger_modechanged();
    ਜੇ (!ਸਾਧਾਰਨ_ਬਹੁਲਕ)
	handle_postponed_scrollback(ਪਦ);
    VIM_CLEAR(ਪਦ->tl_ਸਥਿਤੀ_ਪਾਠ);
    ਜੇ (ਪਦ->tl_ਬਫਰ == curbuf)
	maketitle();
}

/*
 * Called after the job is finished and Terminal mode is not active:
 * Move the vterm contents into the scrollback buffer and free the vterm.
 */
    ਸਥਿਰ ਖਾਲੀ
cleanup_vterm(ਪਦ_T *ਪਦ)
{
    ਸੈੱਟ_terminal_ਬਹੁਲਕ(ਪਦ, FALSE);
    ਜੇ (ਪਦ->tl_ਅੰਤ != TL_ਅੰਤ_CLOSE)
	may_move_terminal_ਮੰਜ਼ਿਲ_ਬਫਰ(ਪਦ, TRUE);
    ਪਦ_free_vterm(ਪਦ);
}

/*
 * Switch from Terminal-Job mode to Terminal-Normal mode.
 * Suspends updating the terminal window.
 */
    ਸਥਿਰ ਖਾਲੀ
ਪਦ_enter_ਸਾਧਾਰਨ_ਬਹੁਲਕ(ਖਾਲੀ)
{
    ਪਦ_T     *ਪਦ = curbuf->b_ਪਦ;
    linenr_T   lnum;
    ਪੂਰਨ        ਖਾਨਾ;

    ਸੈੱਟ_terminal_ਬਹੁਲਕ(ਪਦ, TRUE);

    // Append the current terminal contents to the buffer.
    may_move_terminal_ਮੰਜ਼ਿਲ_ਬਫਰ(ਪਦ, TRUE);

    // Move the window cursor to the position of the cursor in the
    // terminal.
    lnum = ਪਦ->tl_ਬਫਰ_scrolled + 1 + ਪਦ->tl_cursor_pos.ਪੰਕਤੀ;
    ਖਾਨਾ = ਪਦ->tl_cursor_pos.ਖਾਨਾ;
    scrollbackline_pos_ਵਿੱਚ_buf(ਪਦ, ਪਦ->tl_cursor_pos.ਪੰਕਤੀ + ਪਦ->tl_scrollback_scrolled, &lnum, &ਖਾਨਾ, ਨਿਰਾ);

    curwin->w_cursor.lnum = lnum;
    check_cursor();
    ਜੇ (coladvance(ਖਾਨਾ) == FAIL)
	coladvance(MAXCOL);
    curwin->w_ਸੈੱਟ_curswant = true;
    curwin->w_topline = ਪਦ->tl_ਬਫਰ_scrolled + 1;
}

/*
 * Returns TRUE if the current window contains a terminal and we are in
 * Terminal-Normal mode.
 */
    ਪੂਰਨ
ਪਦ_ਵਿੱਚ_ਸਾਧਾਰਨ_ਬਹੁਲਕ(buf_T *buf)
{
    ਪਦ_T *ਪਦ = buf->b_ਪਦ;

    ਵਾਪਸ ਪਦ != ਨਿਰਾ && ਪਦ->tl_ਸਾਧਾਰਨ_ਬਹੁਲਕ;
}

/*
 * Switch from Terminal-Normal mode to Terminal-Job mode.
 * Restores updating the terminal window.
 */
    ਖਾਲੀ
ਪਦ_enter_ਕੰਮ_ਬਹੁਲਕ(ਖਾਲੀ)
{
    ਪਦ_T	*ਪਦ = curbuf->b_ਪਦ;

    ਸੈੱਟ_terminal_ਬਹੁਲਕ(ਪਦ, FALSE);

    ਜੇ (ਪਦ->tl_channel_closed)
	cleanup_vterm(ਪਦ);
    redraw_buf_and_ਸਥਿਤੀ_later(curbuf, UPD_NOT_VALID);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    ਜੇ (WIN_IS_POPUP(curwin))
	redraw_later(UPD_NOT_VALID);
#ਜੇ_ਅੰਤ
}

/*
 * When "modify_other_keys" is set then vgetc() should not reduce a key with
 * modifiers into a basic key.  However, we may only find out after calling
 * vgetc().  Therefore vgetorpeek() will call check_no_reduce_keys() to update
 * "no_reduce_keys" before using it.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
    NRKS_NONE,	    // initial value
    NRKS_CHECK,	    // modify_other_keys was off before calling vgetc()
    NRKS_ਸੈੱਟ,	    // no_reduce_keys was incremented in term_vgetc() or
		    // check_no_reduce_keys(), must be decremented.
} reduce_ਕੁੰਜੀ_state_T;

ਸਥਿਰ reduce_ਕੁੰਜੀ_state_T  no_reduce_ਕੁੰਜੀ_state = NRKS_NONE;

/*
 * Return TRUE if the term is using modifyOtherKeys level 2 or the kitty
 * keyboard protocol.
 */
    ਸਥਿਰ ਪੂਰਨ
vterm_using_ਕੁੰਜੀ_protocol(ਖਾਲੀ)
{
    ਵਾਪਸ curbuf->b_ਪਦ != ਨਿਰਾ
	&& curbuf->b_ਪਦ->tl_vterm != ਨਿਰਾ
	&& (vterm_is_modify_other_ਕੁੰਜੀਆਂ(curbuf->b_ਪਦ->tl_vterm)
		|| vterm_is_kitty_keyboard(curbuf->b_ਪਦ->tl_vterm));
}

    ਖਾਲੀ
check_no_reduce_ਕੁੰਜੀਆਂ(ਖਾਲੀ)
{
    ਜੇ (no_reduce_ਕੁੰਜੀ_state != NRKS_CHECK
	    || no_reduce_ਕੁੰਜੀਆਂ >= 1
	    || curbuf->b_ਪਦ == ਨਿਰਾ
	    || curbuf->b_ਪਦ->tl_vterm == ਨਿਰਾ)
	ਵਾਪਸ;

    ਜੇ (vterm_using_ਕੁੰਜੀ_protocol())
    {
	// "modify_other_keys" or kitty keyboard protocol was enabled while
	// waiting.
	no_reduce_ਕੁੰਜੀ_state = NRKS_ਸੈੱਟ;
	++no_reduce_ਕੁੰਜੀਆਂ;
    }
}

/*
 * Get a key from the user with terminal mode mappings.
 * Note: while waiting a terminal may be closed and freed if the channel is
 * closed and ++close was used.  This may even happen before we get here.
 */
    ਸਥਿਰ ਪੂਰਨ
ਪਦ_vgetc(ਖਾਲੀ)
{
    ਪੂਰਨ c;
    ਪੂਰਨ save_State = State;

    State = ਬਹੁਲਕ_TERMINAL;
    got_int = FALSE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
    ctrl_break_was_pressed = FALSE;
#ਜੇ_ਅੰਤ

    ਜੇ (vterm_using_ਕੁੰਜੀ_protocol())
    {
	++no_reduce_ਕੁੰਜੀਆਂ;
	no_reduce_ਕੁੰਜੀ_state = NRKS_ਸੈੱਟ;
    }
    ਨਹੀਂਤਾਂ
    {
	no_reduce_ਕੁੰਜੀ_state = NRKS_CHECK;
    }

    c = vgetc();
    got_int = FALSE;
    State = save_State;

    ਜੇ (no_reduce_ਕੁੰਜੀ_state == NRKS_ਸੈੱਟ)
	--no_reduce_ਕੁੰਜੀਆਂ;
    no_reduce_ਕੁੰਜੀ_state = NRKS_NONE;

    ਵਾਪਸ c;
}

ਸਥਿਰ ਪੂਰਨ	mouse_was_outside = FALSE;

/*
 * Send key "c" with modifiers "modmask" to terminal.
 * Return FAIL when the key needs to be handled in Normal mode.
 * Return OK when the key was dropped or sent to the terminal.
 */
    ਪੂਰਨ
send_ਕੁੰਜੀਆਂ_ਮੰਜ਼ਿਲ_ਪਦ(ਪਦ_T *ਪਦ, ਪੂਰਨ c, ਪੂਰਨ modmask, ਪੂਰਨ typed)
{
    ਅੱਖਰ	ਸੁਨੇਹਾ[ਕੁੰਜੀ_BUF_LEN];
    size_t	len;
    ਪੂਰਨ		dragging_outside = FALSE;

    // Catch keys that need to be handled as in Normal mode.
    ਚੋਣ (c)
    {
	ਹਾਲਤ NUL:
	ਹਾਲਤ K_ZERO:
	    ਜੇ (typed)
		stuffcharReadbuff(c);
	    ਵਾਪਸ FAIL;

	ਹਾਲਤ K_TABLINE:
	    stuffcharReadbuff(c);
	    ਵਾਪਸ FAIL;

	ਹਾਲਤ K_IGNORE:
	ਹਾਲਤ K_CANCEL:  // used for :normal when running out of chars
	    ਵਾਪਸ FAIL;

	ਹਾਲਤ K_LEFTDRAG:
	ਹਾਲਤ K_MIDDLEDRAG:
	ਹਾਲਤ K_RIGHTDRAG:
	ਹਾਲਤ K_X1DRAG:
	ਹਾਲਤ K_X2DRAG:
	    dragging_outside = mouse_was_outside;
	    // FALLTHROUGH
	ਹਾਲਤ K_LEFTMOUSE:
	ਹਾਲਤ K_LEFTMOUSE_NM:
	ਹਾਲਤ K_LEFTRELEASE:
	ਹਾਲਤ K_LEFTRELEASE_NM:
	ਹਾਲਤ K_MOUSEMOVE:
	ਹਾਲਤ K_MIDDLEMOUSE:
	ਹਾਲਤ K_MIDDLERELEASE:
	ਹਾਲਤ K_RIGHTMOUSE:
	ਹਾਲਤ K_RIGHTRELEASE:
	ਹਾਲਤ K_X1MOUSE:
	ਹਾਲਤ K_X1RELEASE:
	ਹਾਲਤ K_X2MOUSE:
	ਹਾਲਤ K_X2RELEASE:

	ਹਾਲਤ K_MOUSEUP:
	ਹਾਲਤ K_MOUSEDOWN:
	ਹਾਲਤ K_MOUSELEFT:
	ਹਾਲਤ K_MOUSERIGHT:
	    {
		ਪੂਰਨ	ਪੰਕਤੀ = mouse_ਪੰਕਤੀ;
		ਪੂਰਨ	ਖਾਨਾ = mouse_ਖਾਨਾ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
		ਜੇ (popup_is_popup(curwin))
		{
		    ਪੰਕਤੀ -= popup_ਸਿਖਰ_extra(curwin);
		    ਖਾਨਾ -= popup_ਖੱਬਾ_extra(curwin);
		}
#ਜੇ_ਅੰਤ
		ਜੇ (ਪੰਕਤੀ < W_WINROW(curwin)
			|| ਪੰਕਤੀ >= (W_WINROW(curwin) + curwin->w_ਉਚਾਈ)
			|| ਖਾਨਾ < curwin->w_wincol
			|| ਖਾਨਾ >= W_ENDCOL(curwin)
			|| dragging_outside)
		{
		    // click or scroll outside the current window or on status
		    // line or vertical separator
		    ਜੇ (typed)
		    {
			stuffcharReadbuff(c);
			mouse_was_outside = TRUE;
		    }
		    ਵਾਪਸ FAIL;
		}
	    }
	    ਰੋਕੋ;

	ਹਾਲਤ K_COMMAND:
	ਹਾਲਤ K_SCRIPT_COMMAND:
	    ਵਾਪਸ do_cmdkey_command(c, 0);
    }
    ਜੇ (typed)
	mouse_was_outside = FALSE;

    // Convert the typed key to a sequence of bytes for the job.
    len = ਪਦ_convert_ਕੁੰਜੀ(ਪਦ, c, modmask, ਸੁਨੇਹਾ);
    ਜੇ (len > 0)
	// TODO: if FAIL is returned, stop?
	channel_send(ਪਦ->tl_ਕੰਮ->jv_channel, get_tty_part(ਪਦ),
						(char_u *)ਸੁਨੇਹਾ, (ਪੂਰਨ)len, ਨਿਰਾ);

    ਵਾਪਸ OK;
}

/*
 * Handle CTRL-W "": send register contents to the job.
 */
    ਸਥਿਰ ਖਾਲੀ
ਪਦ_paste_ਰਜਿਸਟਰ(ਪੂਰਨ prev_c ਅਣਵਰਤਿਆ)
{
    ਪੂਰਨ		c;
    list_T	*l;
    listitem_T	*ਵਸਤੂ;
    ਲੰਮਾ	reglen = 0;
    ਪੂਰਨ		type;

    ਜੇ (ਜੋੜੋ_ਮੰਜ਼ਿਲ_showcmd(prev_c))
    ਜੇ (ਜੋੜੋ_ਮੰਜ਼ਿਲ_showcmd('"'))
	ਬਾਹਰਲਾ_flush();

    c = ਪਦ_vgetc();
    clear_showcmd();

    ਜੇ (!ਪਦ_use_loop())
	// job finished while waiting for a character
	ਵਾਪਸ;

    // CTRL-W "= prompt for expression to evaluate.
    ਜੇ (c == '=' && get_ਵਿਅੰਜਕ_ਰਜਿਸਟਰ() != '=')
	ਵਾਪਸ;
    ਜੇ (!ਪਦ_use_loop())
	// job finished while waiting for a character
	ਵਾਪਸ;

    l = (list_T *)get_reg_contents(c, GREG_LIST);
    ਜੇ (l == ਨਿਰਾ)
	ਵਾਪਸ;

    type = get_reg_type(c, &reglen);
    FOR_ALL_LIST_ਚੀਜ਼ਾਂ(l, ਵਸਤੂ)
    {
	char_u *s = tv_get_string(&ਵਸਤੂ->li_tv);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
	char_u *tmp = s;

	ਜੇ (!enc_utf8 && enc_codepage > 0)
	{
	    WCHAR   *ret = ਨਿਰਾ;
	    ਪੂਰਨ	ਲੰਮਾਈ = 0;

	    Multi_Byte_ਮੰਜ਼ਿਲ_Wide_Char_alloc(enc_codepage, 0, (ਅੱਖਰ *)s,
		    (ਪੂਰਨ)STRLEN(s), &ret, &ਲੰਮਾਈ);
	    ਜੇ (ret != ਨਿਰਾ)
	    {
		Wide_Char_ਮੰਜ਼ਿਲ_Multi_Byte_alloc(CP_UTF8, 0,
			ret, ਲੰਮਾਈ, (ਅੱਖਰ **)&s, &ਲੰਮਾਈ, 0, 0);
		vim_free(ret);
	    }
	}
#ਜੇ_ਅੰਤ
	channel_send(curbuf->b_ਪਦ->tl_ਕੰਮ->jv_channel, PART_ਵਿੱਚ,
		s, (ਪੂਰਨ)STRLEN(s), ਨਿਰਾ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
	ਜੇ (tmp != s)
	    vim_free(s);
#ਜੇ_ਅੰਤ

	ਜੇ (ਵਸਤੂ->li_ਅਗਲਾ != ਨਿਰਾ || type == MLINE)
	    channel_send(curbuf->b_ਪਦ->tl_ਕੰਮ->jv_channel, PART_ਵਿੱਚ,
		    (char_u *)"\r", 1, ਨਿਰਾ);
    }
    list_free(l);
}

/*
 * Return TRUE when waiting for a character in the terminal, the cursor of the
 * terminal should be displayed.
 */
    ਪੂਰਨ
terminal_is_ਸਰਗਰਮ(ਖਾਲੀ)
{
    ਵਾਪਸ ਵਿੱਚ_terminal_loop != ਨਿਰਾ;
}

/*
 * Return the highlight group ID for the terminal and the window.
 */
    ਸਥਿਰ ਪੂਰਨ
ਪਦ_get_highlight_id(ਪਦ_T *ਪਦ, win_T *wp)
{
    char_u *name;

    ਜੇ (wp != ਨਿਰਾ && wp->w_hlfwin_id != 0)
	name = syn_id2name(wp->w_hlfwin_id);
    ਨਹੀਂਤਾਂ ਜੇ (ਪਦ->tl_highlight_name != ਨਿਰਾ)
	name = ਪਦ->tl_highlight_name;
    ਨਹੀਂਤਾਂ
	name = (char_u*)"Terminal";

    ਵਾਪਸ syn_name2id(name);
}

#ਜੇ defined(FEAT_GUI)
    cursorentry_T *
ਪਦ_get_cursor_shape(guicolor_T *fg, guicolor_T *bg)
{
    ਪਦ_T		 *ਪਦ = ਵਿੱਚ_terminal_loop;
    ਸਥਿਰ cursorentry_T ਇੰਦਰਾਜ;
    ਪੂਰਨ			 id;
    guicolor_T		 ਪਦ_fg = INVALCOLOR;
    guicolor_T		 ਪਦ_bg = INVALCOLOR;

    CLEAR_ਖੇਤਰ(ਇੰਦਰਾਜ);
    ਇੰਦਰਾਜ.shape = ਇੰਦਰਾਜ.mshape =
	ਪਦ->tl_cursor_shape == VTERM_PROP_CURSORSHAPE_UNDERLINE ? SHAPE_HOR :
	ਪਦ->tl_cursor_shape == VTERM_PROP_CURSORSHAPE_BAR_ਖੱਬਾ ? SHAPE_VER :
	SHAPE_ਖੰਡ;
    ਇੰਦਰਾਜ.percentage = 20;
    ਜੇ (ਪਦ->tl_cursor_blink)
    {
	ਇੰਦਰਾਜ.blinkwait = 700;
	ਇੰਦਰਾਜ.blinkon = 400;
	ਇੰਦਰਾਜ.blinkoff = 250;
    }

    // The highlight group overrules the defaults.
    id = ਪਦ_get_highlight_id(ਪਦ, curwin);
    ਜੇ (id != 0)
	syn_id2colors(id, &ਪਦ_fg, &ਪਦ_bg);
    ਜੇ (ਪਦ_bg != INVALCOLOR)
	*fg = ਪਦ_bg;
    ਨਹੀਂਤਾਂ
	*fg = gui.ਪਿੱਠ_pixel;

    ਜੇ (ਪਦ->tl_cursor_color == ਨਿਰਾ)
    {
	ਜੇ (ਪਦ_fg != INVALCOLOR)
	    *bg = ਪਦ_fg;
	ਨਹੀਂਤਾਂ
	    *bg = gui.norm_pixel;
    }
    ਨਹੀਂਤਾਂ
	*bg = color_name2handle(ਪਦ->tl_cursor_color);
    ਇੰਦਰਾਜ.name = "n";
    ਇੰਦਰਾਜ.ਵਰਤਿਆ_for = SHAPE_CURSOR;

    ਵਾਪਸ &ਇੰਦਰਾਜ;
}
#ਜੇ_ਅੰਤ

    ਸਥਿਰ ਖਾਲੀ
may_output_cursor_props(ਖਾਲੀ)
{
    ਜੇ (!cursor_color_equal(ਆਖਰੀ_ਸੈੱਟ_cursor_color, desired_cursor_color)
	    || ਆਖਰੀ_ਸੈੱਟ_cursor_shape != desired_cursor_shape
	    || ਆਖਰੀ_ਸੈੱਟ_cursor_blink != desired_cursor_blink)
    {
	cursor_color_copy(&ਆਖਰੀ_ਸੈੱਟ_cursor_color, desired_cursor_color);
	ਆਖਰੀ_ਸੈੱਟ_cursor_shape = desired_cursor_shape;
	ਆਖਰੀ_ਸੈੱਟ_cursor_blink = desired_cursor_blink;
	ਪਦ_cursor_color(cursor_color_get(desired_cursor_color));
	ਜੇ (desired_cursor_shape == -1 || desired_cursor_blink == -1)
	    // this will restore the initial cursor style, if possible
	    ui_cursor_shape_forced(TRUE);
	ਨਹੀਂਤਾਂ
	    ਪਦ_cursor_shape(desired_cursor_shape, desired_cursor_blink);
    }
}

/*
 * Set the cursor color and shape, if not last set to these.
 */
    ਸਥਿਰ ਖਾਲੀ
may_ਸੈੱਟ_cursor_props(ਪਦ_T *ਪਦ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    // For the GUI the cursor properties are obtained with
    // term_get_cursor_shape().
    ਜੇ (gui.ਵਿੱਚ_use)
	ਵਾਪਸ;
#ਜੇ_ਅੰਤ
    ਜੇ (ਵਿੱਚ_terminal_loop == ਪਦ)
    {
	cursor_color_copy(&desired_cursor_color, ਪਦ->tl_cursor_color);
	desired_cursor_shape = ਪਦ->tl_cursor_shape;
	desired_cursor_blink = ਪਦ->tl_cursor_blink;
	may_output_cursor_props();
    }
}

/*
 * Reset the desired cursor properties and restore them when needed.
 */
    ਸਥਿਰ ਖਾਲੀ
prepare_restore_cursor_props(ਖਾਲੀ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    ਜੇ (gui.ਵਿੱਚ_use)
	ਵਾਪਸ;
#ਜੇ_ਅੰਤ
    cursor_color_copy(&desired_cursor_color, ਨਿਰਾ);
    desired_cursor_shape = -1;
    desired_cursor_blink = -1;
    may_output_cursor_props();
}

/*
 * Returns TRUE if the current window contains a terminal and we are sending
 * keys to the job.
 * If "check_job_status" is TRUE update the job status.
 */
    ਸਥਿਰ ਪੂਰਨ
ਪਦ_use_loop_check(ਪੂਰਨ check_ਕੰਮ_ਸਥਿਤੀ)
{
    ਪਦ_T *ਪਦ = curbuf->b_ਪਦ;

    ਵਾਪਸ ਪਦ != ਨਿਰਾ
	&& !ਪਦ->tl_ਸਾਧਾਰਨ_ਬਹੁਲਕ
	&& ਪਦ->tl_vterm != ਨਿਰਾ
	&& ਪਦ_ਕੰਮ_running_check(ਪਦ, check_ਕੰਮ_ਸਥਿਤੀ);
}

/*
 * Returns TRUE if the current window contains a terminal and we are sending
 * keys to the job.
 */
    ਪੂਰਨ
ਪਦ_use_loop(ਖਾਲੀ)
{
    ਵਾਪਸ ਪਦ_use_loop_check(FALSE);
}

/*
 * Called when entering a window with the mouse.  If this is a terminal window
 * we may want to change state.
 */
    ਖਾਲੀ
ਪਦ_win_entered(ਖਾਲੀ)
{
    ਪਦ_T *ਪਦ = curbuf->b_ਪਦ;

    ਜੇ (ਪਦ == ਨਿਰਾ)
	ਵਾਪਸ;

    ਜੇ (ਪਦ_use_loop_check(TRUE))
    {
	reset_VIsual_and_resel();
	ਜੇ (State & ਬਹੁਲਕ_ਪਾਓ)
	    stop_ਪਾਓ_ਬਹੁਲਕ = TRUE;
    }
    mouse_was_outside = FALSE;
    enter_mouse_ਖਾਨਾ = mouse_ਖਾਨਾ;
    enter_mouse_ਪੰਕਤੀ = mouse_ਪੰਕਤੀ;
}

    ਖਾਲੀ
ਪਦ_focus_change(ਪੂਰਨ ਵਿੱਚ_focus)
{
    ਪਦ_T *ਪਦ = curbuf->b_ਪਦ;

    ਜੇ (ਪਦ == ਨਿਰਾ || ਪਦ->tl_vterm == ਨਿਰਾ)
	ਵਾਪਸ;

    VTermState	*state = vterm_obtain_state(ਪਦ->tl_vterm);

    ਜੇ (ਵਿੱਚ_focus)
	vterm_state_focus_ਵਿੱਚ(state);
    ਨਹੀਂਤਾਂ
	vterm_state_focus_ਬਾਹਰਲਾ(state);
    ਪਦ_forward_output(ਪਦ);
}

/*
 * vgetc() may not include CTRL in the key when modify_other_keys is set.
 * Return the Ctrl-key value in that case.
 */
    ਸਥਿਰ ਪੂਰਨ
raw_c_ਮੰਜ਼ਿਲ_ctrl(ਪੂਰਨ c)
{
    ਜੇ ((mod_ਨਕਾਬ & MOD_ਨਕਾਬ_CTRL)
	    && ((c >= '`' && c <= 0x7f) || (c >= '@' && c <= '_')))
	ਵਾਪਸ c & 0x1f;
    ਵਾਪਸ c;
}

/*
 * When modify_other_keys is set then do the reverse of raw_c_to_ctrl().
 * Also when the Kitty keyboard protocol is used.
 * May set "mod_mask".
 */
    ਸਥਿਰ ਪੂਰਨ
ctrl_ਮੰਜ਼ਿਲ_raw_c(ਪੂਰਨ c)
{
    ਜੇ (c < 0x20 && vterm_using_ਕੁੰਜੀ_protocol())
    {
	mod_ਨਕਾਬ |= MOD_ਨਕਾਬ_CTRL;
	ਵਾਪਸ c + '@';
    }
    ਵਾਪਸ c;
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
    ਪੂਰਨ
terminal_loop(ਪੂਰਨ blocking)
{
    ਪੂਰਨ		c;
    ਪੂਰਨ		raw_c;
    ਪੂਰਨ		termwinkey = 0;
    ਪੂਰਨ		ret;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
    ਪੂਰਨ		tty_fd = curbuf->b_ਪਦ->tl_ਕੰਮ->jv_channel
				 ->ch_part[get_tty_part(curbuf->b_ਪਦ)].ch_fd;
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		restore_cursor = FALSE;

    // Remember the terminal we are sending keys to.  However, the terminal
    // might be closed while waiting for a character, e.g. typing "exit" in a
    // shell and ++close was used.  Therefore use curbuf->b_term instead of a
    // stored reference.
    ਵਿੱਚ_terminal_loop = curbuf->b_ਪਦ;

    ਜੇ (*curwin->w_p_twk != NUL)
    {
	termwinkey = string_ਮੰਜ਼ਿਲ_ਕੁੰਜੀ(curwin->w_p_twk, TRUE);
	ਜੇ (termwinkey == Ctrl_W)
	    termwinkey = 0;
    }
    position_cursor(curwin, &curbuf->b_ਪਦ->tl_cursor_pos);
    may_ਸੈੱਟ_cursor_props(curbuf->b_ਪਦ);

    ਜਦੋਂ (blocking || vpeekc_nomap() != NUL)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
	ਜੇ (curbuf->b_ਪਦ != ਨਿਰਾ && !curbuf->b_ਪਦ->tl_system)
#ਜੇ_ਅੰਤ
	    // TODO: skip screen update when handling a sequence of keys.
	    // Repeat redrawing in case a message is received while redrawing.
	    ਜਦੋਂ (must_redraw != 0)
		ਜੇ (update_screen(0) == FAIL)
		    ਰੋਕੋ;
	ਜੇ (!ਪਦ_use_loop_check(TRUE) || ਵਿੱਚ_terminal_loop != curbuf->b_ਪਦ)
	    // job finished while redrawing
	    ਰੋਕੋ;

	update_cursor(curbuf->b_ਪਦ, FALSE);
	restore_cursor = TRUE;

	raw_c = ਪਦ_vgetc();
	ਜੇ (!ਪਦ_use_loop_check(TRUE) || ਵਿੱਚ_terminal_loop != curbuf->b_ਪਦ)
	{
	    // Job finished while waiting for a character.  Push back the
	    // received character.
	    ਜੇ (raw_c != K_IGNORE)
		vungetc(raw_c);
	    ਰੋਕੋ;
	}
	ਜੇ (raw_c == K_IGNORE)
	    ਜਾਰੀ;
	c = raw_c_ਮੰਜ਼ਿਲ_ctrl(raw_c);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
	/*
	 * The shell or another program may change the tty settings.  Getting
	 * them for every typed character is a bit of overhead, but it's needed
	 * for the first character typed, e.g. when Vim starts in a shell.
	 */
	ਜੇ (mch_isatty(tty_fd))
	{
	    ttyinfo_T info;

	    // Get the current backspace character of the pty.
	    ਜੇ (get_tty_info(tty_fd, &info) == OK)
		ਪਦ_backspace_char = info.backspace;
	}
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
	// On Windows winpty handles CTRL-C, don't send a CTRL_C_EVENT.
	// Use CTRL-BREAK to kill the job.
	ਜੇ (ctrl_break_was_pressed)
	    mch_signal_ਕੰਮ(curbuf->b_ਪਦ->tl_ਕੰਮ, (char_u *)"kill");
#ਜੇ_ਅੰਤ
	// Was either CTRL-W (termwinkey) or CTRL-\ pressed?
	// Not in a system terminal.
	ਜੇ ((c == (termwinkey == 0 ? Ctrl_W : termwinkey) || c == Ctrl_BSL)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
		&& !curbuf->b_ਪਦ->tl_system
#ਜੇ_ਅੰਤ
		)
	{
	    ਪੂਰਨ	    prev_c = c;
	    ਪੂਰਨ	    prev_raw_c = raw_c;
	    ਪੂਰਨ	    prev_mod_ਨਕਾਬ = mod_ਨਕਾਬ;

	    ਜੇ (ਜੋੜੋ_ਮੰਜ਼ਿਲ_showcmd(c))
		ਬਾਹਰਲਾ_flush();

	    raw_c = ਪਦ_vgetc();
	    c = raw_c_ਮੰਜ਼ਿਲ_ctrl(raw_c);

	    clear_showcmd();

	    ਜੇ (!ਪਦ_use_loop_check(TRUE)
					 || ਵਿੱਚ_terminal_loop != curbuf->b_ਪਦ)
		// job finished while waiting for a character
		ਰੋਕੋ;

	    ਜੇ (prev_c == Ctrl_BSL)
	    {
		ਜੇ (c == Ctrl_N)
		{
		    // CTRL-\ CTRL-N : go to Terminal-Normal mode.
		    ਪਦ_enter_ਸਾਧਾਰਨ_ਬਹੁਲਕ();
		    ret = FAIL;
		    ਜਾਓ theend;
		}
		// Send both keys to the terminal, first one here, second one
		// below.
		send_ਕੁੰਜੀਆਂ_ਮੰਜ਼ਿਲ_ਪਦ(curbuf->b_ਪਦ, prev_raw_c, prev_mod_ਨਕਾਬ,
									 TRUE);
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (c == Ctrl_C)
	    {
		// "CTRL-W CTRL-C" or 'termwinkey' CTRL-C: end the job
		mch_signal_ਕੰਮ(curbuf->b_ਪਦ->tl_ਕੰਮ, (char_u *)"kill");
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (c == '.')
	    {
		// "CTRL-W .": send CTRL-W to the job
		// "'termwinkey' .": send 'termwinkey' to the job
		raw_c = ctrl_ਮੰਜ਼ਿਲ_raw_c(termwinkey == 0 ? Ctrl_W : termwinkey);
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (c == Ctrl_BSL)
	    {
		// "CTRL-W CTRL-\": send CTRL-\ to the job
		raw_c = ctrl_ਮੰਜ਼ਿਲ_raw_c(Ctrl_BSL);
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (c == 'N')
	    {
		// CTRL-W N : go to Terminal-Normal mode.
		ਪਦ_enter_ਸਾਧਾਰਨ_ਬਹੁਲਕ();
		ret = FAIL;
		ਜਾਓ theend;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (c == '"')
	    {
		ਪਦ_paste_ਰਜਿਸਟਰ(prev_c);
		ਜਾਰੀ;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (termwinkey == 0 || c != termwinkey)
	    {
		// space for CTRL-W, modifier, multi-byte char and NUL
		char_u buf[1 + 3 + MB_MAXBYTES + 1];

		// Put the command into the typeahead buffer, when using the
		// stuff buffer KeyStuffed is set and 'langmap' won't be used.
		buf[0] = Ctrl_W;
		buf[special_ਮੰਜ਼ਿਲ_buf(c, mod_ਨਕਾਬ, FALSE, buf + 1) + 1] = NUL;
		ins_typebuf(buf, REMAP_NONE, 0, TRUE, FALSE);
		ret = OK;
		ਜਾਓ theend;
	    }
	}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
	ਜੇ (!enc_utf8 && has_mbyte && raw_c >= 0x80)
	{
	    WCHAR   wc;
	    char_u  mb[3];

	    mb[0] = (ਬਿਨਾਨਿਸ਼ਾਨੀ)raw_c >> 8;
	    mb[1] = raw_c;
	    ਜੇ (Multi_Byte_ਮੰਜ਼ਿਲ_Wide_Char(GetACP(), 0, (ਅੱਖਰ*)mb, 2, &wc, 1) > 0)
		raw_c = wc;
	}
#ਜੇ_ਅੰਤ
	ਜੇ (send_ਕੁੰਜੀਆਂ_ਮੰਜ਼ਿਲ_ਪਦ(curbuf->b_ਪਦ, raw_c, mod_ਨਕਾਬ, TRUE) != OK)
	{
	    ਜੇ (raw_c == K_MOUSEMOVE)
		// We are sure to come back here, don't reset the cursor color
		// and shape to avoid flickering.
		restore_cursor = FALSE;

	    ret = OK;
	    ਜਾਓ theend;
	}
    }
    ret = FAIL;

theend:
    ਵਿੱਚ_terminal_loop = ਨਿਰਾ;
    ਜੇ (restore_cursor)
	prepare_restore_cursor_props();

    // Move a snapshot of the screen contents to the buffer, so that completion
    // works in other buffers.
    ਜੇ (curbuf->b_ਪਦ != ਨਿਰਾ && !curbuf->b_ਪਦ->tl_ਸਾਧਾਰਨ_ਬਹੁਲਕ)
	may_move_terminal_ਮੰਜ਼ਿਲ_ਬਫਰ(curbuf->b_ਪਦ, FALSE);

    ਵਾਪਸ ret;
}

    ਸਥਿਰ ਖਾਲੀ
may_toggle_cursor(ਪਦ_T *ਪਦ)
{
    ਜੇ (ਵਿੱਚ_terminal_loop != ਪਦ)
	ਵਾਪਸ;

    ਜੇ (ਪਦ->tl_cursor_visible)
	cursor_on();
    ਨਹੀਂਤਾਂ
	cursor_off();
}

/*
 * Reverse engineer the RGB value into a cterm color index.
 * First color is 1.  Return 0 if no match found (default color).
 */
    ਸਥਿਰ ਪੂਰਨ
color2index(VTermColor *color, ਪੂਰਨ fg, ਪੂਰਨ *boldp)
{
    ਪੂਰਨ red = color->red;
    ਪੂਰਨ blue = color->blue;
    ਪੂਰਨ green = color->green;

    *boldp = FALSE;

    ਜੇ (VTERM_COLOR_IS_INVALID(color))
	ਵਾਪਸ 0;

    ਜੇ (VTERM_COLOR_IS_ਸੂਚਕਾਂਕਿਤ(color))
    {
	// Use the color as-is if possible, give up otherwise.
	ਜੇ (color->ਸੂਚਕਾਂਕ < t_colors)
	{
	    uint8_t ਸੂਚਕਾਂਕ = color->ਸੂਚਕਾਂਕ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
	    // Convert ANSI palette to cterm color index.
	    // cterm_ansi_idx is a table for the reverse conversion from cterm
	    // color index to ANSI palette. However, the current table can
	    // be used for this conversion as well.
	    // Note: If the contents of cterm_ansi_idx change in the future, a
	    // different table may be needed for this purpose.
	    ਜੇ (ਸੂਚਕਾਂਕ < 16)
		ਸੂਚਕਾਂਕ = cterm_ansi_idx[ਸੂਚਕਾਂਕ];
#ਜੇ_ਅੰਤ
	    ਵਾਪਸ ਸੂਚਕਾਂਕ + 1;
	}
	// 8-color terminals can actually display twice as many colors by
	// setting the high-intensity/bold bit.
	ਨਹੀਂਤਾਂ ਜੇ (t_colors == 8 && fg && color->ਸੂਚਕਾਂਕ < 16)
	{
	    *boldp = TRUE;
	    ਵਾਪਸ (color->ਸੂਚਕਾਂਕ & 7) + 1;
	}
	ਵਾਪਸ 0;
    }

    ਜੇ (t_colors >= 256)
    {
	ਜੇ (red == blue && red == green)
	{
	    // 24-color greyscale plus white and black
	    ਸਥਿਰ ਪੂਰਨ cutoff[23] = {
		    0x0D, 0x17, 0x21, 0x2B, 0x35, 0x3F, 0x49, 0x53, 0x5D, 0x67,
		    0x71, 0x7B, 0x85, 0x8F, 0x99, 0xA3, 0xAD, 0xB7, 0xC1, 0xCB,
		    0xD5, 0xDF, 0xE9};
	    ਪੂਰਨ i;

	    ਜੇ (red < 5)
		ਵਾਪਸ 17; // 00/00/00
	    ਜੇ (red > 245) // ff/ff/ff
		ਵਾਪਸ 232;
	    ਲਈ (i = 0; i < 23; ++i)
		ਜੇ (red < cutoff[i])
		    ਵਾਪਸ i + 233;
	    ਵਾਪਸ 256;
	}
	{
	    ਸਥਿਰ ਪੂਰਨ cutoff[5] = {0x2F, 0x73, 0x9B, 0xC3, 0xEB};
	    ਪੂਰਨ ri, gi, bi;

	    // 216-color cube
	    ਲਈ (ri = 0; ri < 5; ++ri)
		ਜੇ (red < cutoff[ri])
		    ਰੋਕੋ;
	    ਲਈ (gi = 0; gi < 5; ++gi)
		ਜੇ (green < cutoff[gi])
		    ਰੋਕੋ;
	    ਲਈ (bi = 0; bi < 5; ++bi)
		ਜੇ (blue < cutoff[bi])
		    ਰੋਕੋ;
	    ਵਾਪਸ 17 + ri * 36 + gi * 6 + bi;
	}
    }
    ਵਾਪਸ 0;
}

/*
 * Convert Vterm attributes to highlight flags.
 */
    ਸਥਿਰ ਪੂਰਨ
vtermAttr2hl(VTermScreenCellAttrs *cellattrs)
{
    ਪੂਰਨ attr = 0;

    ਜੇ (cellattrs->bold)
	attr |= HL_BOLD;
    ਜੇ (cellattrs->underline)
	attr |= HL_UNDERLINE;
    ਜੇ (cellattrs->italic)
	attr |= HL_ITALIC;
    ਜੇ (cellattrs->strike)
	attr |= HL_STRIKETHROUGH;
    ਜੇ (cellattrs->ਉਲਟ)
	attr |= HL_INVERSE;
    ਵਾਪਸ attr;
}

/*
 * Store Vterm attributes in "cell" from highlight flags.
 */
    ਸਥਿਰ ਖਾਲੀ
hl2vtermAttr(ਪੂਰਨ attr, cellattr_T *cell)
{
    CLEAR_ਖੇਤਰ(cell->attrs);
    ਜੇ (attr & HL_BOLD)
	cell->attrs.bold = 1;
    ਜੇ (attr & HL_UNDERLINE)
	cell->attrs.underline = 1;
    ਜੇ (attr & HL_ITALIC)
	cell->attrs.italic = 1;
    ਜੇ (attr & HL_STRIKETHROUGH)
	cell->attrs.strike = 1;
    ਜੇ (attr & HL_INVERSE)
	cell->attrs.ਉਲਟ = 1;
}

/*
 * Convert the attributes of a vterm cell into an attribute index.
 */
    ਸਥਿਰ ਪੂਰਨ
cell2attr(
	ਪਦ_T			*ਪਦ,
	win_T			*wp,
	VTermScreenCellAttrs	*cellattrs,
	VTermColor		*cellfg,
	VTermColor		*cellbg)
{
    ਪੂਰਨ attr = vtermAttr2hl(cellattrs);
    VTermColor *fg = cellfg;
    VTermColor *bg = cellbg;
    ਪੂਰਨ is_default_fg = VTERM_COLOR_IS_DEFAULT_FG(fg);
    ਪੂਰਨ is_default_bg = VTERM_COLOR_IS_DEFAULT_BG(bg);

    ਜੇ (is_default_fg || is_default_bg)
    {
	ਜੇ (wp != ਨਿਰਾ && wp->w_hlfwin_id != 0)
	{
	    ਜੇ (is_default_fg)
		fg = &wp->w_ਪਦ_hlfwin.fg;
	    ਜੇ (is_default_bg)
		bg = &wp->w_ਪਦ_hlfwin.bg;
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (is_default_fg)
		fg = &ਪਦ->tl_default_color.fg;
	    ਜੇ (is_default_bg)
		bg = &ਪਦ->tl_default_color.bg;
	}
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    ਜੇ (gui.ਵਿੱਚ_use)
    {
	guicolor_T guifg = gui_mch_get_rgb_color(fg->red, fg->green, fg->blue);
	guicolor_T guibg = gui_mch_get_rgb_color(bg->red, bg->green, bg->blue);
	ਵਾਪਸ get_gui_attr_idx(attr, guifg, guibg);
    }
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMGUICOLORS
    ਜੇ (p_tgc)
    {
	guicolor_T tgcfg = VTERM_COLOR_IS_INVALID(fg)
	    ? INVALCOLOR
	    : gui_get_rgb_color_cmn(fg->red, fg->green, fg->blue);
	guicolor_T tgcbg = VTERM_COLOR_IS_INVALID(bg)
	    ? INVALCOLOR
	    : gui_get_rgb_color_cmn(bg->red, bg->green, bg->blue);
	ਵਾਪਸ get_tgc_attr_idx(attr, tgcfg, tgcbg);
    }
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
    {
	ਪੂਰਨ bold = MAYBE;
	ਪੂਰਨ ctermfg = color2index(fg, TRUE, &bold);
	ਪੂਰਨ ctermbg = color2index(bg, FALSE, &bold);

	// with 8 colors set the bold attribute to get a bright foreground
	ਜੇ (bold == TRUE)
	    attr |= HL_BOLD;

	ਵਾਪਸ get_cterm_attr_idx(attr, ctermfg, ctermbg);
    }
    ਵਾਪਸ 0;
}

    ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_dirty_snapshot(ਪਦ_T *ਪਦ)
{
    ਪਦ->tl_dirty_snapshot = TRUE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TIMERS
    ਜੇ (!ਪਦ->tl_ਸਾਧਾਰਨ_ਬਹੁਲਕ)
    {
	// Update the snapshot after 100 msec of not getting updates.
	profile_setlimit(100L, &ਪਦ->tl_timer_due);
	ਪਦ->tl_timer_ਸੈੱਟ = TRUE;
    }
#ਜੇ_ਅੰਤ
}

    ਸਥਿਰ ਪੂਰਨ
handle_damage(VTermRect rect, ਖਾਲੀ *user)
{
    ਪਦ_T *ਪਦ = (ਪਦ_T *)user;

    ਪਦ->tl_dirty_ਪੰਕਤੀ_ਸ਼ੁਰੂ = MIN(ਪਦ->tl_dirty_ਪੰਕਤੀ_ਸ਼ੁਰੂ, rect.ਸ਼ੁਰੂ_ਪੰਕਤੀ);
    ਪਦ->tl_dirty_ਪੰਕਤੀ_ਸਿਰਾ = MAX(ਪਦ->tl_dirty_ਪੰਕਤੀ_ਸਿਰਾ, rect.ਸਿਰਾ_ਪੰਕਤੀ);
    ਸੈੱਟ_dirty_snapshot(ਪਦ);
    redraw_buf_later(ਪਦ->tl_ਬਫਰ, UPD_SOME_VALID);
    ਵਾਪਸ 1;
}

    ਸਥਿਰ ਖਾਲੀ
ਪਦ_scroll_ਉੱਪਰ(ਪਦ_T *ਪਦ, ਪੂਰਨ ਸ਼ੁਰੂ_ਪੰਕਤੀ, ਪੂਰਨ ਗਿਣਤੀ_2)
{
    win_T		 *wp = ਨਿਰਾ;
    ਪੂਰਨ			 did_curwin = FALSE;
    VTermColor		 fg, bg;
    VTermScreenCellAttrs attr;
    ਪੂਰਨ			 clear_attr;

    CLEAR_ਖੇਤਰ(attr);

    ਜਦੋਂ (for_all_windows_and_curwin(&wp, &did_curwin))
    {
	ਜੇ (wp->w_ਬਫਰ == ਪਦ->tl_ਬਫਰ)
	{
	    // Set the color to clear lines with.
	    vterm_state_get_default_colors(vterm_obtain_state(ਪਦ->tl_vterm),
								     &fg, &bg);
	    clear_attr = cell2attr(ਪਦ, wp, &attr, &fg, &bg);
	    win_del_ਸਤਰਾਂ(wp, ਸ਼ੁਰੂ_ਪੰਕਤੀ, ਗਿਣਤੀ_2, FALSE, FALSE, clear_attr);
	}
    }
}

    ਸਥਿਰ ਪੂਰਨ
handle_moverect(VTermRect dest, VTermRect src, ਖਾਲੀ *user)
{
    ਪਦ_T	*ਪਦ = (ਪਦ_T *)user;
    ਪੂਰਨ		ਗਿਣਤੀ_2 = src.ਸ਼ੁਰੂ_ਪੰਕਤੀ - dest.ਸ਼ੁਰੂ_ਪੰਕਤੀ;

    // Scrolling up is done much more efficiently by deleting lines instead of
    // redrawing the text. But avoid doing this multiple times, postpone until
    // the redraw happens.
    ਜੇ (dest.ਸ਼ੁਰੂ_ਖਾਨਾ == src.ਸ਼ੁਰੂ_ਖਾਨਾ
	    && dest.ਸਿਰਾ_ਖਾਨਾ == src.ਸਿਰਾ_ਖਾਨਾ
	    && dest.ਸ਼ੁਰੂ_ਪੰਕਤੀ < src.ਸ਼ੁਰੂ_ਪੰਕਤੀ)
    {
	ਜੇ (dest.ਸ਼ੁਰੂ_ਪੰਕਤੀ == 0)
	    ਪਦ->tl_postponed_scroll += ਗਿਣਤੀ_2;
	ਨਹੀਂਤਾਂ
	    ਪਦ_scroll_ਉੱਪਰ(ਪਦ, dest.ਸ਼ੁਰੂ_ਪੰਕਤੀ, ਗਿਣਤੀ_2);
    }

    ਪਦ->tl_dirty_ਪੰਕਤੀ_ਸ਼ੁਰੂ = MIN(ਪਦ->tl_dirty_ਪੰਕਤੀ_ਸ਼ੁਰੂ, dest.ਸ਼ੁਰੂ_ਪੰਕਤੀ);
    ਪਦ->tl_dirty_ਪੰਕਤੀ_ਸਿਰਾ = MIN(ਪਦ->tl_dirty_ਪੰਕਤੀ_ਸਿਰਾ, dest.ਸਿਰਾ_ਪੰਕਤੀ);
    ਸੈੱਟ_dirty_snapshot(ਪਦ);

    // Note sure if the scrolling will work correctly, let's do a complete
    // redraw later.
    redraw_buf_later(ਪਦ->tl_ਬਫਰ, UPD_NOT_VALID);
    ਵਾਪਸ 1;
}

    ਸਥਿਰ ਪੂਰਨ
handle_movecursor(
	VTermPos pos,
	VTermPos oldpos ਅਣਵਰਤਿਆ,
	ਪੂਰਨ visible,
	ਖਾਲੀ *user)
{
    ਪਦ_T	*ਪਦ = (ਪਦ_T *)user;
    win_T	*wp = ਨਿਰਾ;
    ਪੂਰਨ		did_curwin = FALSE;

    ਪਦ->tl_cursor_pos = pos;
    ਪਦ->tl_cursor_visible = visible;

    ਜਦੋਂ (for_all_windows_and_curwin(&wp, &did_curwin))
    {
	ਜੇ (wp->w_ਬਫਰ == ਪਦ->tl_ਬਫਰ)
	    position_cursor(wp, &pos);
    }
    ਜੇ (ਪਦ->tl_ਬਫਰ == curbuf && !ਪਦ->tl_ਸਾਧਾਰਨ_ਬਹੁਲਕ)
	// Don't redraw here, it will be done after
	// vterm_input_write() is finished.
	update_cursor(ਪਦ, FALSE);

    ਵਾਪਸ 1;
}

    ਸਥਿਰ ਪੂਰਨ
handle_settermprop(
	VTermProp prop,
	VTerm_ਮੁੱਲ *ਮੁੱਲ,
	ਖਾਲੀ *user)
{
    ਪਦ_T	*ਪਦ = (ਪਦ_T *)user;
    char_u	*strval = ਨਿਰਾ;

    ਚੋਣ (prop)
    {
	ਹਾਲਤ VTERM_PROP_TITLE:
	    ਜੇ (disable_vterm_title_for_testing)
		ਰੋਕੋ;
	    strval = vim_strnsave((char_u *)ਮੁੱਲ->string.str,
							    ਮੁੱਲ->string.len);
	    ਜੇ (strval == ਨਿਰਾ)
		ਰੋਕੋ;
	    vim_free(ਪਦ->tl_title);
	    // a blank title isn't useful, make it empty, so that "running" is
	    // displayed
	    ਜੇ (*skipwhite(strval) == NUL)
		ਪਦ->tl_title = ਨਿਰਾ;
	    // Same as blank
	    ਨਹੀਂਤਾਂ ਜੇ (ਪਦ->tl_arg0_cmd != ਨਿਰਾ
		    && STRNCMP(ਪਦ->tl_arg0_cmd, strval,
					  (ਪੂਰਨ)STRLEN(ਪਦ->tl_arg0_cmd)) == 0)
		ਪਦ->tl_title = ਨਿਰਾ;
	    // Empty corrupted data of winpty
	    ਨਹੀਂਤਾਂ ਜੇ (STRNCMP("  - ", strval, 4) == 0)
		ਪਦ->tl_title = ਨਿਰਾ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
	    ਨਹੀਂਤਾਂ ਜੇ (!enc_utf8 && enc_codepage > 0)
	    {
		WCHAR   *ret = ਨਿਰਾ;
		ਪੂਰਨ	ਲੰਮਾਈ = 0;

		Multi_Byte_ਮੰਜ਼ਿਲ_Wide_Char_alloc(CP_UTF8, 0,
			(ਅੱਖਰ*)ਮੁੱਲ->string.str,
					(ਪੂਰਨ)ਮੁੱਲ->string.len, &ret, &ਲੰਮਾਈ);
		ਜੇ (ret != ਨਿਰਾ)
		{
		    Wide_Char_ਮੰਜ਼ਿਲ_Multi_Byte_alloc(enc_codepage, 0,
					ret, ਲੰਮਾਈ, (ਅੱਖਰ**)&ਪਦ->tl_title,
					&ਲੰਮਾਈ, 0, 0);
		    vim_free(ret);
		}
	    }
#ਜੇ_ਅੰਤ
	    ਨਹੀਂਤਾਂ
	    {
		ਪਦ->tl_title = strval;
		strval = ਨਿਰਾ;
	    }
	    VIM_CLEAR(ਪਦ->tl_ਸਥਿਤੀ_ਪਾਠ);
	    ਜੇ (ਪਦ == curbuf->b_ਪਦ)
	    {
		maketitle();
		curwin->w_redr_ਸਥਿਤੀ = true;
	    }
	    ਰੋਕੋ;

	ਹਾਲਤ VTERM_PROP_CURSORVISIBLE:
	    ਪਦ->tl_cursor_visible = ਮੁੱਲ->boolean;
	    may_toggle_cursor(ਪਦ);
	    ਬਾਹਰਲਾ_flush();
	    ਰੋਕੋ;

	ਹਾਲਤ VTERM_PROP_CURSORBLINK:
	    ਪਦ->tl_cursor_blink = ਮੁੱਲ->boolean;
	    may_ਸੈੱਟ_cursor_props(ਪਦ);
	    ਰੋਕੋ;

	ਹਾਲਤ VTERM_PROP_CURSORSHAPE:
	    ਪਦ->tl_cursor_shape = ਮੁੱਲ->ਸੰਖਿਆ;
	    may_ਸੈੱਟ_cursor_props(ਪਦ);
	    ਰੋਕੋ;

	ਹਾਲਤ VTERM_PROP_CURSORCOLOR:
	    strval = vim_strnsave((char_u *)ਮੁੱਲ->string.str,
							    ਮੁੱਲ->string.len);
	    ਜੇ (strval == ਨਿਰਾ)
		ਰੋਕੋ;
	    cursor_color_copy(&ਪਦ->tl_cursor_color, strval);
	    may_ਸੈੱਟ_cursor_props(ਪਦ);
	    ਰੋਕੋ;

	ਹਾਲਤ VTERM_PROP_ALTSCREEN:
	    // TODO: do anything else?
	    ਪਦ->tl_using_altscreen = ਮੁੱਲ->boolean;
	    ਰੋਕੋ;

	ਮੂਲ:
	    ਰੋਕੋ;
    }
    vim_free(strval);

    // Always return 1, otherwise vterm doesn't store the value internally.
    ਵਾਪਸ 1;
}

/*
 * The job running in the terminal resized the terminal.
 */
    ਸਥਿਰ ਪੂਰਨ
handle_resize(ਪੂਰਨ ਪੰਕਤੀਆਂ_2, ਪੂਰਨ ਖਾਨੇ_ਗਿਣਤੀ, ਖਾਲੀ *user)
{
    ਪਦ_T	*ਪਦ = (ਪਦ_T *)user;
    win_T	*wp;

    ਪਦ->tl_ਪੰਕਤੀਆਂ = ਪੰਕਤੀਆਂ_2;
    ਪਦ->tl_ਖਾਨੇ_ਗਿਣਤੀ = ਖਾਨੇ_ਗਿਣਤੀ;
    ਜੇ (ਪਦ->tl_vterm_ਮਾਪ_ਬਦਲਿਆ)
	// Size was set by vterm_set_size(), don't set the window size.
	ਪਦ->tl_vterm_ਮਾਪ_ਬਦਲਿਆ = FALSE;
    ਨਹੀਂਤਾਂ
    {
	FOR_ALL_WINDOWS(wp)
	{
	    ਜੇ (wp->w_ਬਫਰ == ਪਦ->tl_ਬਫਰ)
	    {
		win_setheight_win(ਪੰਕਤੀਆਂ_2, wp);
		win_setwidth_win(ਖਾਨੇ_ਗਿਣਤੀ, wp);
	    }
	}
	redraw_buf_later(ਪਦ->tl_ਬਫਰ, UPD_NOT_VALID);
    }
    ਵਾਪਸ 1;
}

/*
 * If the number of lines that are stored goes over 'termwinscroll' then
 * delete the first 10%.
 * "gap" points to tl_scrollback or tl_scrollback_postponed.
 * "update_buffer" is TRUE when the buffer should be updated.
 */
    ਸਥਿਰ ਖਾਲੀ
ਹੱਦਬੰਦੀ_scrollback(ਪਦ_T *ਪਦ, garray_T *gap, ਪੂਰਨ update_ਬਫਰ)
{
    ਜੇ (gap->ga_len < ਪਦ->tl_ਬਫਰ->b_p_twsl)
	ਵਾਪਸ;

    ਪੂਰਨ	todo = MAX(ਪਦ->tl_ਬਫਰ->b_p_twsl / 10,
				     gap->ga_len - ਪਦ->tl_ਬਫਰ->b_p_twsl);
    ਪੂਰਨ	i;
    sb_ਸਤਰ_T *sb_ਸਤਰਾਂ = (sb_ਸਤਰ_T *)gap->ga_ਅੰਕੜੇ;

    curbuf = ਪਦ->tl_ਬਫਰ;
    ਲਈ (i = 0; i < todo; ++i)
    {
	ਜੇ (update_ਬਫਰ && (!sb_ਸਤਰਾਂ[i].continuation || !i))
	{
	    ml_delete(1);
	    --ਪਦ->tl_ਬਫਰ_scrolled;
	}
	vim_free(sb_ਸਤਰਾਂ[i].sb_cells);
    }
    // Continue until end of wrapped line
    ਲਈ (; todo < gap->ga_len && sb_ਸਤਰਾਂ[todo].continuation; ++todo)
	vim_free(sb_ਸਤਰਾਂ[todo].sb_cells);
    curbuf = curwin->w_ਬਫਰ;

    gap->ga_len -= todo;
    mch_memmove(gap->ga_ਅੰਕੜੇ,
		(sb_ਸਤਰ_T *)gap->ga_ਅੰਕੜੇ + todo,
		ਆਕਾਰ(sb_ਸਤਰ_T) * gap->ga_len);
    ਜੇ (update_ਬਫਰ)
    {
	win_T *curwin_save = curwin;
	win_T *wp = ਨਿਰਾ;

	ਪਦ->tl_scrollback_scrolled -= todo;

	FOR_ALL_WINDOWS(wp)
	{
	    ਜੇ (wp->w_ਬਫਰ == ਪਦ->tl_ਬਫਰ)
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
    ਸਥਿਰ ਪੂਰਨ
handle_pushline(ਪੂਰਨ ਖਾਨੇ_ਗਿਣਤੀ, ਅਚਲ VTermScreenCell *cells, ਪੂਰਨ continuation, ਖਾਲੀ *user)
{
    ਪਦ_T	*ਪਦ = (ਪਦ_T *)user;
    garray_T	*gap;
    ਪੂਰਨ		update_ਬਫਰ;

    ਜੇ (ਪਦ->tl_ਸਾਧਾਰਨ_ਬਹੁਲਕ)
    {
	// In Terminal-Normal mode the user interacts with the buffer, thus we
	// must not change it. Postpone adding the scrollback lines.
	gap = &ਪਦ->tl_scrollback_postponed;
	update_ਬਫਰ = FALSE;
    }
    ਨਹੀਂਤਾਂ
    {
	// First remove the lines that were appended before, the pushed line
	// goes above it.
	cleanup_scrollback(ਪਦ);
	gap = &ਪਦ->tl_scrollback;
	update_ਬਫਰ = TRUE;
    }

    ਹੱਦਬੰਦੀ_scrollback(ਪਦ, gap, update_ਬਫਰ);

    ਜੇ (ga_grow(gap, 1) == FAIL)
	ਵਾਪਸ 0;

    cellattr_T	*p = ਨਿਰਾ;
    ਪੂਰਨ		len = 0;
    ਪੂਰਨ		i;
    ਪੂਰਨ		c;
    ਪੂਰਨ		ਖਾਨਾ;
    ਪੂਰਨ		ਪਾਠ_len;
    char_u		*ਪਾਠ;
    sb_ਸਤਰ_T	*ਸਤਰ;
    garray_T	ga;
    cellattr_T	fill_attr = ਪਦ->tl_default_color;

    // do not store empty cells at the end
    ਲਈ (i = 0; i < ਖਾਨੇ_ਗਿਣਤੀ; ++i)
	ਜੇ (cells[i].chars[0] != 0)
	    len = i + 1;
	ਨਹੀਂਤਾਂ
	    cell2cellattr(&cells[i], &fill_attr);

    ga_init2(&ga, 1, 100);
    ਜੇ (len > 0)
	p = ALLOC_MULT(cellattr_T, len);
    ਜੇ (p != ਨਿਰਾ)
    {
	ਲਈ (ਖਾਨਾ = 0; ਖਾਨਾ < len; ਖਾਨਾ += cells[ਖਾਨਾ].ਚੌੜਾਈ)
	{
	    ਜੇ (ga_grow(&ga, VTERM_MAX_CHARS_ਪ੍ਰਤੀ_CELL * 4) == FAIL)
	    {
		ga.ga_len = 0;
		ਰੋਕੋ;
	    }
	    ਲਈ (i = 0; i < VTERM_MAX_CHARS_ਪ੍ਰਤੀ_CELL &&
		    ((c = cells[ਖਾਨਾ].chars[i]) > 0 || i == 0); ++i)
		ga.ga_len += utf_char2bytes(c == NUL ? ' ' : c,
			(char_u *)ga.ga_ਅੰਕੜੇ + ga.ga_len);
	    cell2cellattr(&cells[ਖਾਨਾ], &p[ਖਾਨਾ]);
	}
    }
    ਜੇ (ga_grow(&ga, 1) == FAIL)
    {
	ਜੇ (update_ਬਫਰ)
	    ਪਾਠ = (char_u *)"";
	ਨਹੀਂਤਾਂ
	    ਪਾਠ = vim_strsave((char_u *)"");
	ਪਾਠ_len = 0;
    }
    ਨਹੀਂਤਾਂ
    {
	ਪਾਠ = ga.ga_ਅੰਕੜੇ;
	ਪਾਠ_len = ga.ga_len;
	*(ਪਾਠ + ਪਾਠ_len) = NUL;
    }
    ਜੇ (update_ਬਫਰ)
	ਜੋੜੋ_scrollback_ਸਤਰ_ਮੰਜ਼ਿਲ_ਬਫਰ(ਪਦ, ਪਾਠ, ਪਾਠ_len, continuation);

    ਸਤਰ = (sb_ਸਤਰ_T *)gap->ga_ਅੰਕੜੇ + gap->ga_len;
    ਸਤਰ->sb_ਖਾਨੇ_ਗਿਣਤੀ = len;
    ਸਤਰ->sb_ਬਾਈਟਾਂ = ਪਾਠ_len;
    ਸਤਰ->sb_cells = p;
    ਸਤਰ->sb_fill_attr = fill_attr;
    ਸਤਰ->continuation = (char_u)continuation;
    ਜੇ (update_ਬਫਰ)
    {
	ਸਤਰ->sb_ਪਾਠ = ਨਿਰਾ;
	++ਪਦ->tl_scrollback_scrolled;
	ਜੇ (!continuation)
	    ++ਪਦ->tl_ਬਫਰ_scrolled;
	ga_clear(&ga);  // free the text
    }
    ਨਹੀਂਤਾਂ
    {
	ਸਤਰ->sb_ਪਾਠ = ਪਾਠ;
	ga_init(&ga);  // text is kept in tl_scrollback_postponed
    }
    ++gap->ga_len;
    ਵਾਪਸ 0; // ignored
}

/*
 * Called when leaving Terminal-Normal mode: deal with any scrollback that was
 * received and stored in tl_scrollback_postponed.
 */
    ਸਥਿਰ ਖਾਲੀ
handle_postponed_scrollback(ਪਦ_T *ਪਦ)
{
    ਪੂਰਨ i;

    ਜੇ (ਪਦ->tl_scrollback_postponed.ga_len == 0)
	ਵਾਪਸ;
    ch_log(ਨਿਰਾ, "Moving postponed scrollback to scrollback");

    // First remove the lines that were appended before, the pushed lines go
    // above it.
    cleanup_scrollback(ਪਦ);

    ਲਈ (i = 0; i < ਪਦ->tl_scrollback_postponed.ga_len; ++i)
    {
	char_u		*ਪਾਠ;
	sb_ਸਤਰ_T	*pp_ਸਤਰ;
	sb_ਸਤਰ_T	*ਸਤਰ;

	ਜੇ (ga_grow(&ਪਦ->tl_scrollback, 1) == FAIL)
	    ਰੋਕੋ;
	pp_ਸਤਰ = (sb_ਸਤਰ_T *)ਪਦ->tl_scrollback_postponed.ga_ਅੰਕੜੇ + i;

	ਪਾਠ = pp_ਸਤਰ->sb_ਪਾਠ;
	ਜੇ (ਪਾਠ == ਨਿਰਾ)
	    ਪਾਠ = (char_u *)"";
	ਜੋੜੋ_scrollback_ਸਤਰ_ਮੰਜ਼ਿਲ_ਬਫਰ(ਪਦ, ਪਾਠ, (ਪੂਰਨ)STRLEN(ਪਾਠ), pp_ਸਤਰ->continuation);
	vim_free(pp_ਸਤਰ->sb_ਪਾਠ);

	ਸਤਰ = (sb_ਸਤਰ_T *)ਪਦ->tl_scrollback.ga_ਅੰਕੜੇ
						 + ਪਦ->tl_scrollback.ga_len;
	ਸਤਰ->sb_ਖਾਨੇ_ਗਿਣਤੀ = pp_ਸਤਰ->sb_ਖਾਨੇ_ਗਿਣਤੀ;
	ਸਤਰ->sb_ਬਾਈਟਾਂ = pp_ਸਤਰ->sb_ਬਾਈਟਾਂ;
	ਸਤਰ->sb_cells = pp_ਸਤਰ->sb_cells;
	ਸਤਰ->sb_fill_attr = pp_ਸਤਰ->sb_fill_attr;
	ਸਤਰ->sb_ਪਾਠ = ਨਿਰਾ;
	++ਪਦ->tl_scrollback_scrolled;
	++ਪਦ->tl_scrollback.ga_len;
	ਜੇ (!pp_ਸਤਰ->continuation)
	    ++ਪਦ->tl_ਬਫਰ_scrolled;
    }

    ga_clear(&ਪਦ->tl_scrollback_postponed);
    ਹੱਦਬੰਦੀ_scrollback(ਪਦ, &ਪਦ->tl_scrollback, TRUE);
}

/*
 * Called when the terminal wants to ring the system bell.
 */
    ਸਥਿਰ ਪੂਰਨ
handle_bell(ਖਾਲੀ *user ਅਣਵਰਤਿਆ)
{
    vim_beep(BO_ਪਦ);
    ਵਾਪਸ 0;
}

ਸਥਿਰ VTermScreenCallbacks screen_callbacks = {
    handle_damage,		// damage
    handle_moverect,		// moverect
    handle_movecursor,		// movecursor
    handle_settermprop,		// settermprop
    handle_bell,		// bell
    handle_resize,		// resize
    ਨਿਰਾ,			// sb_pushline
    ਨਿਰਾ,			// sb_popline
    ਨਿਰਾ,			// sb_clear
    handle_pushline	// sb_pushline4
};

/*
 * Do the work after the channel of a terminal was closed.
 * Must be called only when updating_screen is FALSE.
 * Returns TRUE when a buffer was closed (list of terminals may have changed).
 */
    ਸਥਿਰ ਪੂਰਨ
ਪਦ_after_channel_closed(ਪਦ_T *ਪਦ)
{
    // Unless in Terminal-Normal mode: clear the vterm.
    ਜੇ (!ਪਦ->tl_ਸਾਧਾਰਨ_ਬਹੁਲਕ)
    {
	ਪੂਰਨ	fnum = ਪਦ->tl_ਬਫਰ->b_fnum;

	cleanup_vterm(ਪਦ);

	ਜੇ (ਪਦ->tl_ਅੰਤ == TL_ਅੰਤ_CLOSE)
	{
	    aco_save_T	aco;
	    ਪੂਰਨ		do_ਸੈੱਟ_w_locked = ਪਦ->tl_ਬਫਰ->b_nwindows == 0;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
	    win_T	*pwin = ਨਿਰਾ;

	    // If this was a terminal in a popup window, go back to the
	    // previous window.
	    ਜੇ (popup_is_popup(curwin) && curbuf == ਪਦ->tl_ਬਫਰ)
	    {
		pwin = curwin;
		ਜੇ (win_valid(prevwin))
		    win_enter(prevwin, FALSE);
	    }
	    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	    // If this is the last normal window: exit Vim.
	    ਜੇ (ਪਦ->tl_ਬਫਰ->b_nwindows > 0 && only_one_ਖਿੜਕੀ())
	    {
		exarg_T ea;

		CLEAR_ਖੇਤਰ(ea);
		ex_quit(&ea);
		ਵਾਪਸ TRUE;
	    }

	    // ++close or term_finish == "close"
	    ch_log(ਨਿਰਾ, "terminal job finished, closing window");
	    aucmd_prepbuf(&aco, ਪਦ->tl_ਬਫਰ);
	    ਜੇ (curbuf == ਪਦ->tl_ਬਫਰ)
	    {
		win_T	*wp = curwin;

		// Avoid closing the window if we temporarily use it.
		ਜੇ (is_aucmd_win(wp))
		    do_ਸੈੱਟ_w_locked = TRUE;
		ਜੇ (do_ਸੈੱਟ_w_locked)
		    ++wp->w_locked;
		do_bufdel(DOBUF_WIPE, (char_u *)"", 1, fnum, fnum, FALSE);
		ਜੇ (do_ਸੈੱਟ_w_locked)
		    --wp->w_locked;
		aucmd_restbuf(&aco);
	    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
	    ਜੇ (pwin != ਨਿਰਾ)
		popup_close_with_retval(pwin, 0);
#ਜੇ_ਅੰਤ
	    ਵਾਪਸ TRUE;
	}
	ਜੇ (ਪਦ->tl_ਅੰਤ == TL_ਅੰਤ_OPEN
				   && ਪਦ->tl_ਬਫਰ->b_nwindows == 0)
	{
	    ਅੱਖਰ    *cmd = ਪਦ->tl_opencmd == ਨਿਰਾ
				? "botright sbuf %d"
				: (ਅੱਖਰ *)ਪਦ->tl_opencmd;
	    size_t  len = ਲੜੀ_ਲੰਬਾਈ(cmd) + 50;
	    ਅੱਖਰ    *buf = alloc(len);

	    ਜੇ (buf != ਨਿਰਾ)
	    {
		ch_log(ਨਿਰਾ, "terminal job finished, opening window");
		vim_snprintf(buf, len, cmd, fnum);
		do_cmdline_cmd((char_u *)buf);
		vim_free(buf);
	    }
	}
	ਨਹੀਂਤਾਂ
	    ch_log(ਨਿਰਾ, "terminal job finished");
    }

    redraw_buf_and_ਸਥਿਤੀ_later(ਪਦ->tl_ਬਫਰ, UPD_NOT_VALID);
    ਵਾਪਸ FALSE;
}

#ਜੇ defined(FEAT_PROP_POPUP)
/*
 * If the current window is a terminal in a popup window and the job has
 * finished, close the popup window and to back to the previous window.
 * Otherwise return FAIL.
 */
    ਪੂਰਨ
may_close_ਪਦ_popup(ਖਾਲੀ)
{
    ਜੇ (!popup_is_popup(curwin) || curbuf->b_ਪਦ == ਨਿਰਾ
				 || ਪਦ_ਕੰਮ_running_not_none(curbuf->b_ਪਦ))
	ਵਾਪਸ FAIL;

    win_T *pwin = curwin;

    ਜੇ (win_valid(prevwin))
	win_enter(prevwin, FALSE);
    popup_close_with_retval(pwin, 0);
    ਵਾਪਸ OK;
}
#ਜੇ_ਅੰਤ

/*
 * Called when a channel is going to be closed, before invoking the close
 * callback.
 */
    ਖਾਲੀ
ਪਦ_channel_closing(channel_T *ch)
{
    ਪਦ_T *ਪਦ;

    ਲਈ (ਪਦ = ਪਹਿਲਾ_ਪਦ; ਪਦ != ਨਿਰਾ; ਪਦ = ਪਦ->tl_ਅਗਲਾ)
	ਜੇ (ਪਦ->tl_ਕੰਮ == ch->ch_ਕੰਮ && !ਪਦ->tl_channel_closed)
	    ਪਦ->tl_channel_closing = TRUE;
}

/*
 * Called when a channel has been closed.
 * If this was a channel for a terminal window then finish it up.
 */
    ਖਾਲੀ
ਪਦ_channel_closed(channel_T *ch)
{
    ਪਦ_T *ਪਦ;
    ਪਦ_T *ਅਗਲਾ_ਪਦ;
    ਪੂਰਨ	    did_one = FALSE;

    ਲਈ (ਪਦ = ਪਹਿਲਾ_ਪਦ; ਪਦ != ਨਿਰਾ; ਪਦ = ਅਗਲਾ_ਪਦ)
    {
	ਅਗਲਾ_ਪਦ = ਪਦ->tl_ਅਗਲਾ;
	ਜੇ (ਪਦ->tl_ਕੰਮ == ch->ch_ਕੰਮ && !ਪਦ->tl_channel_closed)
	{
	    ਪਦ->tl_channel_closed = TRUE;
	    did_one = TRUE;

	    VIM_CLEAR(ਪਦ->tl_title);
	    VIM_CLEAR(ਪਦ->tl_ਸਥਿਤੀ_ਪਾਠ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
	    ਜੇ (ਪਦ->tl_ਬਾਹਰਲਾ_fd != ਨਿਰਾ)
	    {
		fclose(ਪਦ->tl_ਬਾਹਰਲਾ_fd);
		ਪਦ->tl_ਬਾਹਰਲਾ_fd = ਨਿਰਾ;
	    }
#ਜੇ_ਅੰਤ

	    ਜੇ (updating_screen)
	    {
		// Cannot open or close windows now.  Can happen when
		// 'lazyredraw' is set.
		ਪਦ->tl_channel_recently_closed = TRUE;
		ਜਾਰੀ;
	    }

	    ਜੇ (ਪਦ_after_channel_closed(ਪਦ))
		ਅਗਲਾ_ਪਦ = ਪਹਿਲਾ_ਪਦ;
	}
    }

    ਜੇ (did_one)
    {
	redraw_statuslines();

	// Need to break out of vgetc().
	ins_char_typebuf(K_IGNORE, 0);
	typebuf_was_filled = TRUE;

	ਪਦ = curbuf->b_ਪਦ;
	ਜੇ (ਪਦ != ਨਿਰਾ)
	{
	    ਜੇ (ਪਦ->tl_ਕੰਮ == ch->ch_ਕੰਮ)
		maketitle();
	    update_cursor(ਪਦ, ਪਦ->tl_cursor_visible);
	}
    }
}

/*
 * To be called after resetting updating_screen: handle any terminal where the
 * channel was closed.
 */
    ਖਾਲੀ
ਪਦ_check_channel_closed_recently(ਖਾਲੀ)
{
    ਪਦ_T *ਪਦ;
    ਪਦ_T *ਅਗਲਾ_ਪਦ;

    ਲਈ (ਪਦ = ਪਹਿਲਾ_ਪਦ; ਪਦ != ਨਿਰਾ; ਪਦ = ਅਗਲਾ_ਪਦ)
    {
	ਅਗਲਾ_ਪਦ = ਪਦ->tl_ਅਗਲਾ;
	ਜੇ (ਪਦ->tl_channel_recently_closed)
	{
	    ਪਦ->tl_channel_recently_closed = FALSE;
	    ਜੇ (ਪਦ_after_channel_closed(ਪਦ))
		// start over, the list may have changed
		ਅਗਲਾ_ਪਦ = ਪਹਿਲਾ_ਪਦ;
	}
    }
}

/*
 * Fill one screen line from a line of the terminal.
 * Advances "pos" to past the last column.
 */
    ਸਥਿਰ ਖਾਲੀ
ਪਦ_line2screenline(
	ਪਦ_T		*ਪਦ,
	win_T		*wp,
	VTermScreen	*screen,
	VTermPos	*pos,
	ਪੂਰਨ		max_ਖਾਨਾ)
{
    ਪੂਰਨ off = screen_get_current_ਸਤਰ_off();

    ਲਈ (pos->ਖਾਨਾ = 0; pos->ਖਾਨਾ < max_ਖਾਨਾ; )
    {
	VTermScreenCell cell;
	ਪੂਰਨ		c;

	ਜੇ (vterm_screen_get_cell(screen, *pos, &cell) == 0)
	    CLEAR_ਖੇਤਰ(cell);

	c = cell.chars[0];
	ਜੇ (c == NUL)
	{
	    Screen_ਸਤਰਾਂ[off] = ' ';
	    ਜੇ (enc_utf8)
		Screen_ਸਤਰਾਂ_UC[off] = NUL;
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (enc_utf8)
	    {
		ਪੂਰਨ i;

		// composing chars
		ਲਈ (i = 0; i < Screen_mco
			      && i + 1 < VTERM_MAX_CHARS_ਪ੍ਰਤੀ_CELL; ++i)
		{
		    Screen_ਸਤਰਾਂ_C[i][off] = cell.chars[i + 1];
		    ਜੇ (cell.chars[i + 1] == 0)
			ਰੋਕੋ;
		}
		ਜੇ (c >= 0x80 || (Screen_mco > 0
					 && Screen_ਸਤਰਾਂ_C[0][off] != 0))
		{
		    Screen_ਸਤਰਾਂ[off] = ' ';
		    Screen_ਸਤਰਾਂ_UC[off] = c;
		}
		ਨਹੀਂਤਾਂ
		{
		    Screen_ਸਤਰਾਂ[off] = c;
		    Screen_ਸਤਰਾਂ_UC[off] = NUL;
		}
	    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
	    ਨਹੀਂਤਾਂ ਜੇ (has_mbyte && c >= 0x80)
	    {
		char_u	mb[MB_MAXBYTES+1];
		WCHAR	wc = c;

		ਜੇ (Wide_Char_ਮੰਜ਼ਿਲ_Multi_Byte(GetACP(), 0, &wc, 1,
					       (ਅੱਖਰ*)mb, 2, 0, 0) > 1)
		{
		    Screen_ਸਤਰਾਂ[off] = mb[0];
		    Screen_ਸਤਰਾਂ[off + 1] = mb[1];
		    cell.ਚੌੜਾਈ = mb_ptr2cells(mb);
		}
		ਨਹੀਂਤਾਂ
		    Screen_ਸਤਰਾਂ[off] = c;
	    }
#ਜੇ_ਅੰਤ
	    ਨਹੀਂਤਾਂ
		// This will only store the lower byte of "c".
		Screen_ਸਤਰਾਂ[off] = c;
	}
	ScreenAttrs[off] = cell2attr(ਪਦ, wp, &cell.attrs, &cell.fg,
								     &cell.bg);

	++pos->ਖਾਨਾ;
	++off;
	ਜੇ (cell.ਚੌੜਾਈ == 2)
	{
	    // don't set the second byte to NUL for a DBCS encoding, it
	    // has been set above
	    ਜੇ (enc_utf8)
	    {
		Screen_ਸਤਰਾਂ_UC[off] = NUL;
		Screen_ਸਤਰਾਂ[off] = NUL;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (!has_mbyte)
	    {
		// Can't show a double-width character with a single-byte
		// 'encoding', just use a space.
		Screen_ਸਤਰਾਂ[off] = ' ';
		ScreenAttrs[off] = ScreenAttrs[off - 1];
	    }

	    ++pos->ਖਾਨਾ;
	    ++off;
	}
    }
}

#ਜੇ defined(FEAT_GUI)
    ਸਥਿਰ ਖਾਲੀ
update_system_ਪਦ(ਪਦ_T *ਪਦ)
{
    VTermPos	    pos;
    VTermScreen	    *screen;

    ਜੇ (ਪਦ->tl_vterm == ਨਿਰਾ)
	ਵਾਪਸ;
    screen = vterm_obtain_screen(ਪਦ->tl_vterm);

    // Scroll up to make more room for terminal lines if needed.
    // Use the cursor position to determine how much to scroll, because
    // ConPTY may damage all rows on initialization even when most are
    // empty, which would cause unnecessary scrolling.
    ਜਦੋਂ (ਪਦ->tl_toprow > 0
		  && (ਪੰਕਤੀਆਂ - ਪਦ->tl_toprow) < ਪਦ->tl_cursor_pos.ਪੰਕਤੀ + 1)
    {
	ਪੂਰਨ save_p_ਹੋਰ = p_ਹੋਰ;

	p_ਹੋਰ = FALSE;
	ਸੁਨੇਹਾ_ਪੰਕਤੀ = ਪੰਕਤੀਆਂ - 1;
	ਸੁਨੇਹਾ_puts("\n");
	p_ਹੋਰ = save_p_ਹੋਰ;
	--ਪਦ->tl_toprow;
    }

    ਲਈ (pos.ਪੰਕਤੀ = ਪਦ->tl_dirty_ਪੰਕਤੀ_ਸ਼ੁਰੂ; pos.ਪੰਕਤੀ < ਪਦ->tl_dirty_ਪੰਕਤੀ_ਸਿਰਾ
						  && pos.ਪੰਕਤੀ < ਪੰਕਤੀਆਂ; ++pos.ਪੰਕਤੀ)
    {
	ਜੇ (pos.ਪੰਕਤੀ < ਪਦ->tl_ਪੰਕਤੀਆਂ)
	{
	    ਪੂਰਨ max_ਖਾਨਾ = MIN(topframe->fr_ਚੌੜਾਈ, ਪਦ->tl_ਖਾਨੇ_ਗਿਣਤੀ);

	    ਪਦ_line2screenline(ਪਦ, ਨਿਰਾ, screen, &pos, max_ਖਾਨਾ);
	}
	ਨਹੀਂਤਾਂ
	    pos.ਖਾਨਾ = 0;

	screen_ਸਤਰ(curwin, ਪਦ->tl_toprow + pos.ਪੰਕਤੀ,
		firstwin->w_wincol, pos.ਖਾਨਾ, topframe->fr_ਚੌੜਾਈ, -1, 0);
    }

    ਪਦ->tl_dirty_ਪੰਕਤੀ_ਸ਼ੁਰੂ = MAX_ਪੰਕਤੀ;
    ਪਦ->tl_dirty_ਪੰਕਤੀ_ਸਿਰਾ = 0;
}
#ਜੇ_ਅੰਤ

/*
 * Return TRUE if window "wp" is to be redrawn with term_update_window().
 * Returns FALSE when there is no terminal running in this window or it is in
 * Terminal-Normal mode.
 */
    ਪੂਰਨ
ਪਦ_do_update_ਖਿੜਕੀ(win_T *wp)
{
    ਪਦ_T	*ਪਦ = wp->w_ਬਫਰ->b_ਪਦ;

    ਵਾਪਸ ਪਦ != ਨਿਰਾ && ਪਦ->tl_vterm != ਨਿਰਾ && !ਪਦ->tl_ਸਾਧਾਰਨ_ਬਹੁਲਕ;
}

/*
 * Called to update a window that contains an active terminal.
 */
    ਖਾਲੀ
ਪਦ_update_ਖਿੜਕੀ(win_T *wp)
{
    ਪਦ_T	*ਪਦ = wp->w_ਬਫਰ->b_ਪਦ;
    VTerm	*vterm;
    VTermScreen *screen;
    VTermState	*state;
    VTermPos	pos;
    ਪੂਰਨ		ਪੰਕਤੀਆਂ_2, ਖਾਨੇ_ਗਿਣਤੀ;
    ਪੂਰਨ		newrows, newcols;
    ਪੂਰਨ		minsize;
    win_T	*twp;

    vterm = ਪਦ->tl_vterm;
    screen = vterm_obtain_screen(vterm);
    state = vterm_obtain_state(vterm);

    // We use UPD_NOT_VALID on a resize or scroll, redraw everything then.
    // With UPD_SOME_VALID only redraw what was marked dirty.
    ਜੇ (wp->w_redr_type > UPD_SOME_VALID)
    {
	ਪਦ->tl_dirty_ਪੰਕਤੀ_ਸ਼ੁਰੂ = 0;
	ਪਦ->tl_dirty_ਪੰਕਤੀ_ਸਿਰਾ = MAX_ਪੰਕਤੀ;

	ਜੇ (ਪਦ->tl_postponed_scroll > 0
			      && ਪਦ->tl_postponed_scroll < ਪਦ->tl_ਪੰਕਤੀਆਂ / 3)
	    // Scrolling is usually faster than redrawing, when there are only
	    // a few lines to scroll.
	    ਪਦ_scroll_ਉੱਪਰ(ਪਦ, 0, ਪਦ->tl_postponed_scroll);
	ਪਦ->tl_postponed_scroll = 0;
    }

    /*
     * If the window was resized a redraw will be triggered and we get here.
     * Adjust the size of the vterm unless 'termwinsize' specifies a fixed size.
     */
    minsize = parse_termwinsize(wp, &ਪੰਕਤੀਆਂ_2, &ਖਾਨੇ_ਗਿਣਤੀ);

    newrows = 99999;
    newcols = 99999;
    ਲਈ (twp = firstwin; ; twp = twp->w_ਅਗਲਾ)
    {
	// Always use curwin, it may be a popup window.
	win_T *wwp = twp == ਨਿਰਾ ? curwin : twp;

	// When more than one window shows the same terminal, use the
	// smallest size.
	ਜੇ (wwp->w_ਬਫਰ == ਪਦ->tl_ਬਫਰ)
	{
	    newrows = MIN(newrows, wwp->w_ਉਚਾਈ);
	    newcols = MIN(newcols, wwp->w_ਚੌੜਾਈ);
	}
	ਜੇ (twp == ਨਿਰਾ)
	    ਰੋਕੋ;
    }
    ਜੇ (newrows == 99999 || newcols == 99999)
	ਵਾਪਸ; // safety exit
    newrows = ਪੰਕਤੀਆਂ_2 == 0 ? newrows : minsize ? MAX(ਪੰਕਤੀਆਂ_2, newrows) : ਪੰਕਤੀਆਂ_2;
    newcols = ਖਾਨੇ_ਗਿਣਤੀ == 0 ? newcols : minsize ? MAX(ਖਾਨੇ_ਗਿਣਤੀ, newcols) : ਖਾਨੇ_ਗਿਣਤੀ;

    // If no cell is visible there is no point in resizing.  Also, vterm can't
    // handle a zero height.
    ਜੇ (newrows == 0 || newcols == 0)
	ਵਾਪਸ;

    ਜੇ (ਪਦ->tl_ਪੰਕਤੀਆਂ != newrows || ਪਦ->tl_ਖਾਨੇ_ਗਿਣਤੀ != newcols)
    {
	ਪਦ->tl_vterm_ਮਾਪ_ਬਦਲਿਆ = TRUE;
	vterm_ਸੈੱਟ_ਮਾਪ(vterm, newrows, newcols);
	ch_log(ਪਦ->tl_ਕੰਮ->jv_channel, "Resizing terminal to %d lines",
								      newrows);
	ਪਦ_report_winsize(ਪਦ, newrows, newcols);

	// Updating the terminal size will cause the snapshot to be cleared.
	// When not in terminal_loop() we need to restore it.
	ਜੇ (ਪਦ != ਵਿੱਚ_terminal_loop)
	    may_move_terminal_ਮੰਜ਼ਿਲ_ਬਫਰ(ਪਦ, FALSE);
    }

    // The cursor may have been moved when resizing.
    vterm_state_get_cursorpos(state, &pos);
    position_cursor(wp, &pos);

    ਲਈ (pos.ਪੰਕਤੀ = ਪਦ->tl_dirty_ਪੰਕਤੀ_ਸ਼ੁਰੂ; pos.ਪੰਕਤੀ < ਪਦ->tl_dirty_ਪੰਕਤੀ_ਸਿਰਾ
					  && pos.ਪੰਕਤੀ < wp->w_ਉਚਾਈ; ++pos.ਪੰਕਤੀ)
    {
	ਜੇ (pos.ਪੰਕਤੀ < ਪਦ->tl_ਪੰਕਤੀਆਂ)
	{
	    ਪੂਰਨ max_ਖਾਨਾ = MIN(wp->w_ਚੌੜਾਈ, ਪਦ->tl_ਖਾਨੇ_ਗਿਣਤੀ);

	    ਪਦ_line2screenline(ਪਦ, wp, screen, &pos, max_ਖਾਨਾ);
	}
	ਨਹੀਂਤਾਂ
	    pos.ਖਾਨਾ = 0;

	screen_ਸਤਰ(wp, wp->w_winrow + pos.ਪੰਕਤੀ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MENU
				+ winbar_ਉਚਾਈ(wp)
#ਜੇ_ਅੰਤ
				, wp->w_wincol, pos.ਖਾਨਾ, wp->w_ਚੌੜਾਈ, -1,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
				popup_is_popup(wp) ? SLF_POPUP :
#ਜੇ_ਅੰਤ
				0);
    }
}

/*
 * Called after updating all windows: may reset dirty rows.
 */
    ਖਾਲੀ
ਪਦ_did_update_ਖਿੜਕੀ(win_T *wp)
{
    ਪਦ_T	*ਪਦ = wp->w_ਬਫਰ->b_ਪਦ;

    ਜੇ (ਪਦ == ਨਿਰਾ || ਪਦ->tl_vterm == ਨਿਰਾ || ਪਦ->tl_ਸਾਧਾਰਨ_ਬਹੁਲਕ
						|| wp->w_redr_type != 0)
	ਵਾਪਸ;

    ਪਦ->tl_dirty_ਪੰਕਤੀ_ਸ਼ੁਰੂ = MAX_ਪੰਕਤੀ;
    ਪਦ->tl_dirty_ਪੰਕਤੀ_ਸਿਰਾ = 0;
}

/*
 * Return TRUE if "wp" is a terminal window where the job has finished.
 */
    ਪੂਰਨ
ਪਦ_is_ਮੁਕੰਮਲ(buf_T *buf)
{
    ਵਾਪਸ buf->b_ਪਦ != ਨਿਰਾ && buf->b_ਪਦ->tl_vterm == ਨਿਰਾ;
}

/*
 * Return TRUE if "wp" is a terminal window where the job has finished or we
 * are in Terminal-Normal mode, thus we show the buffer contents.
 */
    ਪੂਰਨ
ਪਦ_show_ਬਫਰ(buf_T *buf)
{
    ਪਦ_T *ਪਦ = buf->b_ਪਦ;

    ਵਾਪਸ ਪਦ != ਨਿਰਾ && (ਪਦ->tl_vterm == ਨਿਰਾ || ਪਦ->tl_ਸਾਧਾਰਨ_ਬਹੁਲਕ);
}

/*
 * The current buffer is going to be changed.  If there is terminal
 * highlighting remove it now.
 */
    ਖਾਲੀ
ਪਦ_change_ਵਿੱਚ_curbuf(ਖਾਲੀ)
{
    ਪਦ_T *ਪਦ = curbuf->b_ਪਦ;

    ਜੇ (!ਪਦ_is_ਮੁਕੰਮਲ(curbuf) || ਪਦ->tl_scrollback.ga_len <= 0)
	ਵਾਪਸ;

    free_scrollback(ਪਦ);
    redraw_buf_later(ਪਦ->tl_ਬਫਰ, UPD_NOT_VALID);

    // The buffer is now like a normal buffer, it cannot be easily
    // abandoned when changed.
    ਸੈੱਟ_string_option_ਸਿੱਧਾ((char_u *)"buftype", -1,
	    (char_u *)"", OPT_FREE|OPT_LOCAL, 0);
}

/*
 * Get the screen attribute for a position in the buffer.
 * Use a negative "col" to get the filler background color.
 */
    ਪੂਰਨ
ਪਦ_get_attr(win_T *wp, linenr_T lnum, ਪੂਰਨ ਖਾਨਾ)
{
    buf_T	*buf = wp->w_ਬਫਰ;
    ਪਦ_T	*ਪਦ = buf->b_ਪਦ;
    sb_ਸਤਰ_T	*ਸਤਰ;
    cellattr_T	*cellattr;
    ਪੂਰਨ         sb_ਸਤਰ = -1;
    ਪੂਰਨ         sb_ਖਾਨਾ = ਖਾਨਾ;

    ਜੇ (ਪਦ->tl_scrollback.ga_len)
	bufline_pos_ਵਿੱਚ_scrollback(ਪਦ, lnum, ਖਾਨਾ, &sb_ਸਤਰ, &sb_ਖਾਨਾ);

    ਜੇ (sb_ਸਤਰ < 0)
	cellattr = &ਪਦ->tl_default_color;
    ਨਹੀਂਤਾਂ
    {
	ਸਤਰ = (sb_ਸਤਰ_T *)ਪਦ->tl_scrollback.ga_ਅੰਕੜੇ + sb_ਸਤਰ;
	ਜੇ (sb_ਖਾਨਾ < 0 || sb_ਖਾਨਾ >= ਸਤਰ->sb_ਖਾਨੇ_ਗਿਣਤੀ)
	    cellattr = &ਸਤਰ->sb_fill_attr;
	ਨਹੀਂਤਾਂ
	    cellattr = ਸਤਰ->sb_cells + sb_ਖਾਨਾ;
    }
    ਵਾਪਸ cell2attr(ਪਦ, wp, &cellattr->attrs, &cellattr->fg, &cellattr->bg);
}

/*
 * Convert a cterm color number 0 - 255 to RGB.
 * This is compatible with xterm.
 */
    ਸਥਿਰ ਖਾਲੀ
cterm_color2vterm(ਪੂਰਨ nr, VTermColor *rgb)
{
    cterm_color2rgb(nr, &rgb->red, &rgb->green, &rgb->blue, &rgb->ਸੂਚਕਾਂਕ);
    ਜੇ (rgb->ਸੂਚਕਾਂਕ == 0)
	rgb->type = VTERM_COLOR_RGB;
    ਨਹੀਂਤਾਂ
    {
	rgb->type = VTERM_COLOR_ਸੂਚਕਾਂਕਿਤ;
	--rgb->ਸੂਚਕਾਂਕ;
    }
}

/*
 * Initialize vterm color from the synID.
 * Returns TRUE if color is set to "fg" and "bg".
 * Otherwise returns FALSE.
 */
    ਸਥਿਰ ਪੂਰਨ
get_vterm_color_from_synid(ਪੂਰਨ id, VTermColor *fg, VTermColor *bg)
{
#ਜੇ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    // Use the actual color for the GUI and when 'termguicolors' is set.
    ਜੇ (0
# ifdef FEAT_GUI
	    || gui.ਵਿੱਚ_use
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

	ਜੇ (id > 0)
	    syn_id2colors(id, &fg_rgb, &bg_rgb);

	ਜੇ (fg_rgb != INVALCOLOR)
	{
	    long_u rgb = GUI_MCH_GET_RGB(fg_rgb);
	    fg->red = (ਬਿਨਾਨਿਸ਼ਾਨੀ)(rgb >> 16);
	    fg->green = (ਬਿਨਾਨਿਸ਼ਾਨੀ)(rgb >> 8) & 255;
	    fg->blue = (ਬਿਨਾਨਿਸ਼ਾਨੀ)rgb & 255;
	    fg->type = VTERM_COLOR_RGB | VTERM_COLOR_DEFAULT_FG;
	}
	ਨਹੀਂਤਾਂ
	    fg->type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_FG;

	ਜੇ (bg_rgb != INVALCOLOR)
	{
	    long_u rgb = GUI_MCH_GET_RGB(bg_rgb);
	    bg->red = (ਬਿਨਾਨਿਸ਼ਾਨੀ)(rgb >> 16);
	    bg->green = (ਬਿਨਾਨਿਸ਼ਾਨੀ)(rgb >> 8) & 255;
	    bg->blue = (ਬਿਨਾਨਿਸ਼ਾਨੀ)rgb & 255;
	    bg->type = VTERM_COLOR_RGB | VTERM_COLOR_DEFAULT_BG;
	}
	ਨਹੀਂਤਾਂ
	    bg->type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_BG;

	ਵਾਪਸ TRUE;
    }
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
    ਜੇ (t_colors >= 16)
    {
	ਪੂਰਨ cterm_fg = -1;
	ਪੂਰਨ cterm_bg = -1;

	ਜੇ (id > 0)
	    syn_id2cterm_bg(id, &cterm_fg, &cterm_bg);

	ਜੇ (cterm_fg >= 0)
	{
	    cterm_color2vterm(cterm_fg, fg);
	    fg->type |= VTERM_COLOR_DEFAULT_FG;
	}
	ਨਹੀਂਤਾਂ
	    fg->type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_FG;

	ਜੇ (cterm_bg >= 0)
	{
	    cterm_color2vterm(cterm_bg, bg);
	    bg->type |= VTERM_COLOR_DEFAULT_BG;
	}
	ਨਹੀਂਤਾਂ
	    bg->type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_BG;

	ਵਾਪਸ TRUE;
    }

    ਵਾਪਸ FALSE;
}

    ਖਾਲੀ
ਪਦ_reset_hlfwin(win_T *wp)
{
    wp->w_ਪਦ_hlfwin.fg.type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_FG;
    wp->w_ਪਦ_hlfwin.bg.type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_BG;
}

/*
 * Cache the color of HLF_WIN.
 */
    ਖਾਲੀ
ਪਦ_update_hlfwin(win_T *wp)
{
    ਪੂਰਨ id = wp->w_hlfwin_id;

    ਜੇ (id == 0 || !get_vterm_color_from_synid(id == -1 ? 0 : id,
		&wp->w_ਪਦ_hlfwin.fg, &wp->w_ਪਦ_hlfwin.bg))
	ਪਦ_reset_hlfwin(wp);
}

/*
 * Called when option 'termguicolors' was set,
 * or when any highlight is changed.
 */
    ਖਾਲੀ
ਪਦ_update_hlfwin_all(ਖਾਲੀ)
{
    win_T	 *wp = ਨਿਰਾ;
    ਪੂਰਨ		 did_curwin = FALSE;

    ਜਦੋਂ (for_all_windows_and_curwin(&wp, &did_curwin))
	ਪਦ_update_hlfwin(wp);
}

/*
 * Initialize term->tl_default_color from the environment.
 */
    ਖਾਲੀ
ਪਦ_init_default_colors(ਪਦ_T *ਪਦ)
{
    VTermColor	    *fg, *bg;
    ਪੂਰਨ		    fgval, bgval;
    ਪੂਰਨ		    id;

    CLEAR_ਖੇਤਰ(ਪਦ->tl_default_color.attrs);
    ਪਦ->tl_default_color.ਚੌੜਾਈ = 1;
    fg = &ਪਦ->tl_default_color.fg;
    bg = &ਪਦ->tl_default_color.bg;

    // Vterm uses a default black background.  Set it to white when
    // 'background' is "light".
    ਜੇ (*p_bg == 'l')
    {
	fgval = 0;
	bgval = 255;
    }
    ਨਹੀਂਤਾਂ
    {
	fgval = 255;
	bgval = 0;
    }
    fg->red = fg->green = fg->blue = fgval;
    bg->red = bg->green = bg->blue = bgval;
    fg->type = VTERM_COLOR_RGB | VTERM_COLOR_DEFAULT_FG;
    bg->type = VTERM_COLOR_RGB | VTERM_COLOR_DEFAULT_BG;

    // The highlight group overrules the defaults.
    id = ਪਦ_get_highlight_id(ਪਦ, ਨਿਰਾ);

    ਜੇ (!get_vterm_color_from_synid(id, fg, bg))
    {
#ਜੇ defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
	ਪੂਰਨ tmp;
#ਜੇ_ਅੰਤ

	// In an MS-Windows console we know the normal colors.
	ਜੇ (cterm_ਸਾਧਾਰਨ_fg_color > 0)
	{
	    cterm_color2vterm(cterm_ਸਾਧਾਰਨ_fg_color - 1, fg);
#ਜੇ defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
# ifdef VIMDLL
	    ਜੇ (!gui.ਵਿੱਚ_use)
# endif
	    {
		tmp = fg->red;
		fg->red = fg->blue;
		fg->blue = tmp;
	    }
#ਜੇ_ਅੰਤ
	}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMRESPONSE
	ਨਹੀਂਤਾਂ
	    ਪਦ_get_fg_color(&fg->red, &fg->green, &fg->blue);
#ਜੇ_ਅੰਤ

	ਜੇ (cterm_ਸਾਧਾਰਨ_bg_color > 0)
	{
	    cterm_color2vterm(cterm_ਸਾਧਾਰਨ_bg_color - 1, bg);
#ਜੇ defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
# ifdef VIMDLL
	    ਜੇ (!gui.ਵਿੱਚ_use)
# endif
	    {
		tmp = fg->red;
		fg->red = fg->blue;
		fg->blue = tmp;
	    }
#ਜੇ_ਅੰਤ
	}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMRESPONSE
	ਨਹੀਂਤਾਂ
	    ਪਦ_get_bg_color(&bg->red, &bg->green, &bg->blue);
#ਜੇ_ਅੰਤ
    }
}

#ਜੇ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Return TRUE if the user-defined palette (either g:terminal_ansi_colors or the
 * "ansi_colors" argument in term_start()) shall be applied.
 */
    ਸਥਿਰ ਪੂਰਨ
ਪਦ_use_palette(ਖਾਲੀ)
{
    ਜੇ (0
# ifdef FEAT_GUI
	    || gui.ਵਿੱਚ_use
# endif
# ifdef FEAT_TERMGUICOLORS
	    || p_tgc
# endif
       )
	ਵਾਪਸ TRUE;
    ਵਾਪਸ FALSE;
}

/*
 * Set the 16 ANSI colors from array of RGB values
 */
    ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_vterm_palette(VTerm *vterm, long_u *rgb)
{
    ਪੂਰਨ		ਸੂਚਕਾਂਕ = 0;
    VTermState	*state = vterm_obtain_state(vterm);

    ਲਈ (; ਸੂਚਕਾਂਕ < 16; ਸੂਚਕਾਂਕ++)
    {
	VTermColor	color;

	color.type = VTERM_COLOR_RGB;
	color.red = (ਬਿਨਾਨਿਸ਼ਾਨੀ)(rgb[ਸੂਚਕਾਂਕ] >> 16);
	color.green = (ਬਿਨਾਨਿਸ਼ਾਨੀ)(rgb[ਸੂਚਕਾਂਕ] >> 8) & 255;
	color.blue = (ਬਿਨਾਨਿਸ਼ਾਨੀ)rgb[ਸੂਚਕਾਂਕ] & 255;
	color.ਸੂਚਕਾਂਕ = 0;
	vterm_state_ਸੈੱਟ_palette_color(state, ਸੂਚਕਾਂਕ, &color);
    }
}

/*
 * Set the ANSI color palette from a list of colors
 */
    ਸਥਿਰ ਪੂਰਨ
ਸੈੱਟ_ansi_colors_list(VTerm *vterm, list_T *list)
{
    ਪੂਰਨ		n = 0;
    long_u	rgb[16];
    listitem_T	*li;

    ਲਈ (li = list->lv_ਪਹਿਲਾ; li != ਨਿਰਾ && n < 16; li = li->li_ਅਗਲਾ, n++)
    {
	char_u		*color_name;
	guicolor_T	guicolor;

	color_name = tv_get_string_chk(&li->li_tv);
	ਜੇ (color_name == ਨਿਰਾ)
	    ਵਾਪਸ FAIL;

	guicolor = GUI_GET_COLOR(color_name);
	ਜੇ (guicolor == INVALCOLOR)
	    ਵਾਪਸ FAIL;

	rgb[n] = GUI_MCH_GET_RGB(guicolor);
    }

    ਜੇ (n != 16 || li != ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    ਸੈੱਟ_vterm_palette(vterm, rgb);

    ਵਾਪਸ OK;
}

/*
 * Initialize the ANSI color palette from g:terminal_ansi_colors[0:15]
 */
    ਸਥਿਰ ਖਾਲੀ
init_vterm_ansi_colors(VTerm *vterm)
{
    dictitem_T	*var = ਲੱਭੋ_var((char_u *)"g:terminal_ansi_colors", ਨਿਰਾ, TRUE);

    ਜੇ (var == ਨਿਰਾ)
	ਵਾਪਸ;

    ਜੇ (var->di_tv.v_type != VAR_LIST
	    || var->di_tv.vval.v_list == ਨਿਰਾ
	    || var->di_tv.vval.v_list->lv_ਪਹਿਲਾ == &range_list_ਵਸਤੂ
	    || ਸੈੱਟ_ansi_colors_list(vterm, var->di_tv.vval.v_list) == FAIL)
	semsg(_(e_invalid_ਦਲੀਲ_str), "g:terminal_ansi_colors");
}
#ਜੇ_ਅੰਤ

/*
 * Handles a "drop" command from the job in the terminal.
 * "item" is the file name, "item->li_next" may have options.
 */
    ਸਥਿਰ ਖਾਲੀ
handle_drop_command(listitem_T *ਵਸਤੂ)
{
    char_u	*fname = tv_get_string(&ਵਸਤੂ->li_tv);
    listitem_T	*opt_ਵਸਤੂ = ਵਸਤੂ->li_ਅਗਲਾ;
    ਪੂਰਨ		bufnr;
    win_T	*wp;
    tabpage_T   *tp;
    exarg_T	ea;
    char_u	*tofree = ਨਿਰਾ;

    bufnr = buflist_ਜੋੜੋ(fname, BLN_LISTED | BLN_NOOPT);
    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	ਜੇ (wp->w_ਬਫਰ->b_fnum == bufnr)
	{
	    // buffer is in a window already, go there
	    goto_tabpage_win(tp, wp);
	    ਵਾਪਸ;
	}
    }

    CLEAR_ਖੇਤਰ(ea);

    ਜੇ (opt_ਵਸਤੂ != ਨਿਰਾ && opt_ਵਸਤੂ->li_tv.v_type == VAR_DICT
					&& opt_ਵਸਤੂ->li_tv.vval.v_dict != ਨਿਰਾ)
    {
	dict_T *dict = opt_ਵਸਤੂ->li_tv.vval.v_dict;
	char_u *p;

	p = dict_get_string(dict, "ff", FALSE);
	ਜੇ (p == ਨਿਰਾ)
	    p = dict_get_string(dict, "fileformat", FALSE);
	ਜੇ (p != ਨਿਰਾ)
	{
	    ਜੇ (check_ff_ਮੁੱਲ(p) == FAIL)
		ch_log(ਨਿਰਾ, "Invalid ff argument to drop: %s", p);
	    ਨਹੀਂਤਾਂ
		ea.force_ff = *p;
	}
	p = dict_get_string(dict, "enc", FALSE);
	ਜੇ (p == ਨਿਰਾ)
	    p = dict_get_string(dict, "encoding", FALSE);
	ਜੇ (p != ਨਿਰਾ)
	{
	    ea.cmd = alloc(STRLEN(p) + 12);
	    ਜੇ (ea.cmd != ਨਿਰਾ)
	    {
		sprintf((ਅੱਖਰ *)ea.cmd, "sbuf ++enc=%s", p);
		ea.force_enc = 11;
		tofree = ea.cmd;
	    }
	}

	p = dict_get_string(dict, "bad", FALSE);
	ਜੇ (p != ਨਿਰਾ)
	    get_bad_opt(p, &ea);

	ਜੇ (dict_has_ਕੁੰਜੀ(dict, "bin"))
	    ea.force_bin = FORCE_BIN;
	ਜੇ (dict_has_ਕੁੰਜੀ(dict, "binary"))
	    ea.force_bin = FORCE_BIN;
	ਜੇ (dict_has_ਕੁੰਜੀ(dict, "nobin"))
	    ea.force_bin = FORCE_NOBIN;
	ਜੇ (dict_has_ਕੁੰਜੀ(dict, "nobinary"))
	    ea.force_bin = FORCE_NOBIN;
    }

    // open in new window, like ":split fname"
    ਜੇ (ea.cmd == ਨਿਰਾ)
	ea.cmd = (char_u *)"split";
    ea.arg = fname;
    ea.cmdidx = CMD_ਵੰਡੋ;
    ex_splitview(&ea);

    vim_free(tofree);
}

/*
 * Return TRUE if "func" starts with "pat" and "pat" isn't empty.
 */
    ਸਥਿਰ ਪੂਰਨ
is_permitted_ਪਦ_api(char_u *func, char_u *pat)
{
    ਵਾਪਸ pat != ਨਿਰਾ && *pat != NUL && STRNICMP(func, pat, STRLEN(pat)) == 0;
}

/*
 * Handles a function call from the job running in a terminal.
 * "item" is the function name, "item->li_next" has the arguments.
 */
    ਸਥਿਰ ਖਾਲੀ
handle_call_command(ਪਦ_T *ਪਦ, channel_T *channel, listitem_T *ਵਸਤੂ)
{
    char_u	*func;
    typval_T	argvars[2];
    typval_T	rettv;
    funcexe_T	funcexe;

    ਜੇ (ਵਸਤੂ->li_ਅਗਲਾ == ਨਿਰਾ)
    {
	ch_log(channel, "Missing function arguments for call");
	ਵਾਪਸ;
    }
    func = tv_get_string(&ਵਸਤੂ->li_tv);

    ਜੇ (!is_permitted_ਪਦ_api(func, ਪਦ->tl_api))
    {
	ch_log(channel, "Unpermitted function: %s", func);
	ਵਾਪਸ;
    }

    argvars[0].v_type = VAR_ਸੰਖਿਆ;
    argvars[0].vval.v_ਸੰਖਿਆ = ਪਦ->tl_ਬਫਰ->b_fnum;
    argvars[1] = ਵਸਤੂ->li_ਅਗਲਾ->li_tv;
    CLEAR_ਖੇਤਰ(funcexe);
    funcexe.fe_firstline = 1L;
    funcexe.fe_lastline = 1L;
    funcexe.fe_evaluate = TRUE;
    ਜੇ (call_func(func, -1, &rettv, 2, argvars, &funcexe) == OK)
    {
	clear_tv(&rettv);
	ch_log(channel, "Function %s called", func);
    }
    ਨਹੀਂਤਾਂ
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
    ਸਥਿਰ size_t
url_decode(ਅਚਲ ਅੱਖਰ *src, ਅਚਲ size_t len, char_u *dst)
{
    size_t i = 0, j = 0;

    ਜਦੋਂ (i < len)
    {
	ਜੇ (src[i] == '%' && i + 2 < len)
	{
	    dst[j] = hexhex2nr((char_u *)&src[i + 1]);
	    j++;
	    i += 3;
	}
	ਨਹੀਂਤਾਂ
	{
	    dst[j] = src[i];
	    i++;
	    j++;
	}
    }
    dst[j] = '\0';
    ਵਾਪਸ j;
}

/*
 * Sync terminal buffer's cwd with shell's pwd with the help of OSC 7.
 *
 * The OSC 7 sequence has the format of
 * "\033]7;file://HOSTNAME/CURRENT/DIR\033\\"
 * and what VTerm provides via VTermStringFragment is
 * "file://HOSTNAME/CURRENT/DIR"
 */
    ਸਥਿਰ ਖਾਲੀ
sync_shell_dir(garray_T *gap)
{
    ਪੂਰਨ       ਵਿੱਥ = 7;  // len of "file://" is 7
    ਅੱਖਰ      *pos = (ਅੱਖਰ *)gap->ga_ਅੰਕੜੇ + ਵਿੱਥ;
    char_u    *new_dir;

    // remove HOSTNAME to get PWD
    ਜਦੋਂ (ਵਿੱਥ < (ਪੂਰਨ)gap->ga_len && *pos != '/' )
    {
	++ਵਿੱਥ;
	++pos;
    }

    ਜੇ (ਵਿੱਥ >= (ਪੂਰਨ)gap->ga_len)
    {
	semsg(_(e_failed_ਮੰਜ਼ਿਲ_ਕੱਢੋ_pwd_from_str_check_your_shell_config),
								 gap->ga_ਅੰਕੜੇ);
	ਵਾਪਸ;
    }

    new_dir = alloc(gap->ga_len - ਵਿੱਥ + 1);
    url_decode(pos, gap->ga_len-ਵਿੱਥ, new_dir);
    changedir_func(new_dir, TRUE, CDSCOPE_ਖਿੜਕੀ);
    vim_free(new_dir);
}

/*
 * Called by libvterm when it cannot recognize an OSC sequence.
 * We recognize a terminal API command.
 */
    ਸਥਿਰ ਪੂਰਨ
parse_osc(ਪੂਰਨ command, VTermStringFragment frag, ਖਾਲੀ *user)
{
    ਪਦ_T	*ਪਦ = (ਪਦ_T *)user;
    js_read_T	reader;
    typval_T	tv;
    channel_T	*channel = ਪਦ->tl_ਕੰਮ == ਨਿਰਾ ? ਨਿਰਾ
						    : ਪਦ->tl_ਕੰਮ->jv_channel;
    garray_T	*gap = &ਪਦ->tl_osc_buf;

    // We recognize only OSC 5 1 ; {command} and OSC 7 ; {command}
    ਜੇ (command != 51 && (command != 7 || !p_asd))
	ਵਾਪਸ 0;

    // Concatenate what was received until the final piece is found.
    ਜੇ (ga_grow(gap, (ਪੂਰਨ)frag.len + 1) == FAIL)
    {
	ga_clear(gap);
	ਵਾਪਸ 1;
    }
    mch_memmove((ਅੱਖਰ *)gap->ga_ਅੰਕੜੇ + gap->ga_len, frag.str, frag.len);
    gap->ga_len += (ਪੂਰਨ)frag.len;
    ਜੇ (!frag.final)
	ਵਾਪਸ 1;

    ((ਅੱਖਰ *)gap->ga_ਅੰਕੜੇ)[gap->ga_len] = 0;

    ਜੇ (command == 7)
    {
	sync_shell_dir(gap);
	ga_clear(gap);
	ਵਾਪਸ 1;
    }

    reader.js_buf = gap->ga_ਅੰਕੜੇ;
    reader.js_fill = ਨਿਰਾ;
    reader.js_ਵਰਤਿਆ = 0;
    ਜੇ (json_decode(&reader, &tv, 0) == OK
	    && tv.v_type == VAR_LIST
	    && tv.vval.v_list != ਨਿਰਾ)
    {
	listitem_T *ਵਸਤੂ = tv.vval.v_list->lv_ਪਹਿਲਾ;

	ਜੇ (ਵਸਤੂ == ਨਿਰਾ)
	    ch_log(channel, "Missing command");
	ਨਹੀਂਤਾਂ
	{
	    char_u	*cmd = tv_get_string(&ਵਸਤੂ->li_tv);

	    // Make sure an invoked command doesn't delete the buffer (and the
	    // terminal) under our fingers.
	    ++ਪਦ->tl_ਬਫਰ->b_locked;

	    ਵਸਤੂ = ਵਸਤੂ->li_ਅਗਲਾ;
	    ਜੇ (ਵਸਤੂ == ਨਿਰਾ)
		ch_log(channel, "Missing argument for %s", cmd);
	    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(cmd, "drop") == 0)
		handle_drop_command(ਵਸਤੂ);
	    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(cmd, "call") == 0)
		handle_call_command(ਪਦ, channel, ਵਸਤੂ);
	    ਨਹੀਂਤਾਂ
		ch_log(channel, "Invalid command received: %s", cmd);
	    --ਪਦ->tl_ਬਫਰ->b_locked;
	}
    }
    ਨਹੀਂਤਾਂ
	ch_log(channel, "Invalid JSON received");

    ga_clear(gap);
    clear_tv(&tv);
    ਵਾਪਸ 1;
}

/*
 * Called by libvterm when it cannot recognize a CSI sequence.
 * We recognize the window position report.
 */
    ਸਥਿਰ ਪੂਰਨ
parse_csi(
	ਅਚਲ ਅੱਖਰ  *leader ਅਣਵਰਤਿਆ,
	ਅਚਲ ਲੰਮਾ  ਦਲੀਲਾਂ[],
	ਪੂਰਨ	    argcount,
	ਅਚਲ ਅੱਖਰ  *intermed ਅਣਵਰਤਿਆ,
	ਅੱਖਰ	    command,
	ਖਾਲੀ	    *user)
{
    ਪਦ_T	*ਪਦ = (ਪਦ_T *)user;
    ਅੱਖਰ	buf[100];
    ਪੂਰਨ		len;
    ਪੂਰਨ		x = 0;
    ਪੂਰਨ		y = 0;
    win_T	*wp;

    // We recognize only CSI 13 t
    ਜੇ (command != 't' || argcount != 1 || ਦਲੀਲਾਂ[0] != 13)
	ਵਾਪਸ 0; // not handled

    // When getting the window position is not possible or it fails it results
    // in zero/zero.
#ਜੇ defined(FEAT_GUI) \
	|| (defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)) \
	|| defined(MSWIN)
    (ਖਾਲੀ)ui_get_winpos(&x, &y, (varnumber_T)100);
#ਜੇ_ਅੰਤ

    FOR_ALL_WINDOWS(wp)
	ਜੇ (wp->w_ਬਫਰ == ਪਦ->tl_ਬਫਰ)
	    ਰੋਕੋ;
    ਜੇ (wp != ਨਿਰਾ)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
	ਜੇ (gui.ਵਿੱਚ_use)
	{
	    x += wp->w_wincol * gui.char_ਚੌੜਾਈ;
	    y += W_WINROW(wp) * gui.char_ਉਚਾਈ;
	}
	ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	{
	    // We roughly estimate the position of the terminal window inside
	    // the Vim window by assuming a 10 x 7 character cell.
	    x += wp->w_wincol * 7;
	    y += W_WINROW(wp) * 10;
	}
    }

    len = vim_snprintf(buf, 100, "\x1b[3;%d;%dt", x, y);
    channel_send(ਪਦ->tl_ਕੰਮ->jv_channel, get_tty_part(ਪਦ),
						     (char_u *)buf, len, ਨਿਰਾ);
    ਵਾਪਸ 1;
}

ਸਥਿਰ VTermStateFallbacks state_fallbacks = {
    ਨਿਰਾ,		// control
    parse_csi,		// csi
    parse_osc,		// osc
    ਨਿਰਾ,		// dcs
    ਨਿਰਾ,		// apc
    ਨਿਰਾ,		// pm
    ਨਿਰਾ		// sos
};

/*
 * Use Vim's allocation functions for vterm so profiling works.
 */
    ਸਥਿਰ ਖਾਲੀ *
vterm_malloc(size_t ਮਾਪ, ਖਾਲੀ *ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
    // make sure that the length is not zero
    ਵਾਪਸ alloc_clear(ਮਾਪ == 0 ? 1L : ਮਾਪ);
}

    ਸਥਿਰ ਖਾਲੀ
vterm_memfree(ਖਾਲੀ *ptr, ਖਾਲੀ *ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
    vim_free(ptr);
}

ਸਥਿਰ VTermAllocatorFunctions vterm_allocator = {
  &vterm_malloc,
  &vterm_memfree
};

/*
 * Create a new vterm and initialize it.
 * Return FAIL when out of memory.
 */
    ਸਥਿਰ ਪੂਰਨ
create_vterm(ਪਦ_T *ਪਦ, ਪੂਰਨ ਪੰਕਤੀਆਂ_2, ਪੂਰਨ ਖਾਨੇ_ਗਿਣਤੀ)
{
    VTerm	    *vterm;
    VTermScreen	    *screen;
    VTermState	    *state;
    VTerm_ਮੁੱਲ	    ਮੁੱਲ;

    vterm = vterm_new_with_allocator(ਪੰਕਤੀਆਂ_2, ਖਾਨੇ_ਗਿਣਤੀ, &vterm_allocator, ਨਿਰਾ);
    ਪਦ->tl_vterm = vterm;
    ਜੇ (vterm == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    // Allocate screen and state here, so we can bail out if that fails.
    state = vterm_obtain_state(vterm);
    screen = vterm_obtain_screen(vterm);
    ਜੇ (state == ਨਿਰਾ || screen == ਨਿਰਾ)
    {
	vterm_free(vterm);
	ਵਾਪਸ FAIL;
    }

    vterm_screen_ਸੈੱਟ_callbacks(screen, &screen_callbacks, ਪਦ);
    vterm_screen_ਸੈੱਟ_damage_ਮਿਲਾਓ(screen, VTERM_DAMAGE_SCROLL);
    vterm_screen_callbacks_has_pushline4(screen);
    // TODO: depends on 'encoding'.
    vterm_ਸੈੱਟ_utf8(vterm, 1);

    ਪਦ_init_default_colors(ਪਦ);

    vterm_state_ਸੈੱਟ_default_colors(
	    state,
	    &ਪਦ->tl_default_color.fg,
	    &ਪਦ->tl_default_color.bg);

    ਜੇ (t_colors < 16)
	// Less than 16 colors: assume that bold means using a bright color for
	// the foreground color.
	vterm_state_ਸੈੱਟ_bold_highbright(vterm_obtain_state(vterm), 1);

    // Required to initialize most things.
    vterm_screen_reset(screen, 1 /* hard */);

    // Allow using alternate screen.
    vterm_screen_enable_altscreen(screen, 1);

    // For unix do not use a blinking cursor.  In an xterm this causes the
    // cursor to blink if it's blinking in the xterm.
    // For Windows we respect the system wide setting.
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
    ਜੇ (Get_Caret_Blink_ਸਮਾਂ() == INFINITE)
	ਮੁੱਲ.boolean = 0;
    ਨਹੀਂਤਾਂ
	ਮੁੱਲ.boolean = 1;
#ਨਹੀਂਤਾਂ
    ਮੁੱਲ.boolean = 0;
#ਜੇ_ਅੰਤ
    vterm_state_ਸੈੱਟ_termprop(state, VTERM_PROP_CURSORBLINK, &ਮੁੱਲ);
    vterm_state_ਸੈੱਟ_unrecognised_fallbacks(state, &state_fallbacks, ਪਦ);

    ਵਾਪਸ OK;
}

/*
 * Reset the terminal palette to its default value.
 */
    ਸਥਿਰ ਖਾਲੀ
ਪਦ_reset_palette(VTerm *vterm)
{
    VTermState	*state = vterm_obtain_state(vterm);
    ਪੂਰਨ		ਸੂਚਕਾਂਕ;

    ਲਈ (ਸੂਚਕਾਂਕ = 0; ਸੂਚਕਾਂਕ < 16; ਸੂਚਕਾਂਕ++)
    {
	VTermColor	color;

	color.type = VTERM_COLOR_ਸੂਚਕਾਂਕਿਤ;
	ansi_color2rgb(ਸੂਚਕਾਂਕ, &color.red, &color.green, &color.blue,
		&color.ਸੂਚਕਾਂਕ);
	// The first valid index starts at 1.
	color.ਸੂਚਕਾਂਕ -= 1;

	vterm_state_ਸੈੱਟ_palette_color(state, ਸੂਚਕਾਂਕ, &color);
    }
}

    ਸਥਿਰ ਖਾਲੀ
ਪਦ_update_palette(ਪਦ_T *ਪਦ)
{
#ਜੇ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    ਜੇ (ਪਦ_use_palette()
	    && (ਪਦ->tl_palette != ਨਿਰਾ
		|| ਲੱਭੋ_var((char_u *)"g:terminal_ansi_colors", ਨਿਰਾ, TRUE)
								      != ਨਿਰਾ))
    {
	ਜੇ (ਪਦ->tl_palette != ਨਿਰਾ)
	    ਸੈੱਟ_vterm_palette(ਪਦ->tl_vterm, ਪਦ->tl_palette);
	ਨਹੀਂਤਾਂ
	    init_vterm_ansi_colors(ਪਦ->tl_vterm);
    }
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	ਪਦ_reset_palette(ਪਦ->tl_vterm);
}

/*
 * Called when option 'termguicolors' is changed.
 */
    ਖਾਲੀ
ਪਦ_update_palette_all(ਖਾਲੀ)
{
    ਪਦ_T *ਪਦ;

    FOR_ALL_TERMS(ਪਦ)
    {
	ਜੇ (ਪਦ->tl_vterm == ਨਿਰਾ)
	    ਜਾਰੀ;
	ਪਦ_update_palette(ਪਦ);
    }
}

/*
 * Called when option 'background' or 'termguicolors' was set,
 * or when any highlight is changed.
 */
    ਖਾਲੀ
ਪਦ_update_colors_all(ਖਾਲੀ)
{
    ਪਦ_T *ਪਦ;

    FOR_ALL_TERMS(ਪਦ)
    {
	ਜੇ (ਪਦ->tl_vterm == ਨਿਰਾ)
	    ਜਾਰੀ;
	ਪਦ_init_default_colors(ਪਦ);
	vterm_state_ਸੈੱਟ_default_colors(
		vterm_obtain_state(ਪਦ->tl_vterm),
		&ਪਦ->tl_default_color.fg,
		&ਪਦ->tl_default_color.bg);
    }
}

/*
 * Return the text to show for the buffer name and status.
 */
    char_u *
ਪਦ_get_ਸਥਿਤੀ_ਪਾਠ(ਪਦ_T *ਪਦ)
{
    ਜੇ (ਪਦ->tl_ਸਥਿਤੀ_ਪਾਠ != ਨਿਰਾ)
	ਵਾਪਸ ਪਦ->tl_ਸਥਿਤੀ_ਪਾਠ;

    char_u *txt;
    size_t len;
    char_u *fname;

    ਜੇ (ਪਦ->tl_ਸਾਧਾਰਨ_ਬਹੁਲਕ)
    {
	ਜੇ (ਪਦ_ਕੰਮ_running(ਪਦ))
	    txt = (char_u *)_("Terminal");
	ਨਹੀਂਤਾਂ
	    txt = (char_u *)_("Terminal-finished");
    }
    ਨਹੀਂਤਾਂ ਜੇ (ਪਦ->tl_title != ਨਿਰਾ)
	txt = ਪਦ->tl_title;
    ਨਹੀਂਤਾਂ ਜੇ (ਪਦ_none_open(ਪਦ))
	txt = (char_u *)_("active");
    ਨਹੀਂਤਾਂ ਜੇ (ਪਦ_ਕੰਮ_running(ਪਦ))
	txt = (char_u *)_("running");
    ਨਹੀਂਤਾਂ
	txt = (char_u *)_("finished");
    fname = buf_get_fname(ਪਦ->tl_ਬਫਰ);
    len = 9 + STRLEN(fname) + STRLEN(txt);
    ਪਦ->tl_ਸਥਿਤੀ_ਪਾਠ = alloc(len);
    ਜੇ (ਪਦ->tl_ਸਥਿਤੀ_ਪਾਠ != ਨਿਰਾ)
	vim_snprintf((ਅੱਖਰ *)ਪਦ->tl_ਸਥਿਤੀ_ਪਾਠ, len, "%s [%s]",
		fname, txt);
    ਵਾਪਸ ਪਦ->tl_ਸਥਿਤੀ_ਪਾਠ;
}

/*
 * Clear the cached value of the status text.
 */
    ਖਾਲੀ
ਪਦ_clear_ਸਥਿਤੀ_ਪਾਠ(ਪਦ_T *ਪਦ)
{
    VIM_CLEAR(ਪਦ->tl_ਸਥਿਤੀ_ਪਾਠ);
}

/*
 * Mark references in jobs of terminals.
 */
    ਪੂਰਨ
ਸੈੱਟ_ref_ਵਿੱਚ_ਪਦ(ਪੂਰਨ copyID)
{
    ਪੂਰਨ		abort = FALSE;
    ਪਦ_T	*ਪਦ;
    typval_T	tv;

    ਲਈ (ਪਦ = ਪਹਿਲਾ_ਪਦ; !abort && ਪਦ != ਨਿਰਾ; ਪਦ = ਪਦ->tl_ਅਗਲਾ)
	ਜੇ (ਪਦ->tl_ਕੰਮ != ਨਿਰਾ)
	{
	    tv.v_type = VAR_ਕੰਮ;
	    tv.vval.v_ਕੰਮ = ਪਦ->tl_ਕੰਮ;
	    abort = abort || ਸੈੱਟ_ref_ਵਿੱਚ_ਵਸਤੂ(&tv, copyID, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ);
	}
    ਵਾਪਸ abort;
}

/*
 * Get the buffer from the first argument in "argvars".
 * Returns NULL when the buffer is not for a terminal window and logs a message
 * with "where".
 */
    ਸਥਿਰ buf_T *
ਪਦ_get_buf(typval_T *argvars, ਅੱਖਰ *where)
{
    buf_T *buf;

    ++emsg_off;
    buf = tv_get_buf(&argvars[0], FALSE);
    --emsg_off;
    ਜੇ (buf == ਨਿਰਾ || buf->b_ਪਦ == ਨਿਰਾ)
    {
	(ਖਾਲੀ)tv_get_ਸੰਖਿਆ(&argvars[0]);    // issue errmsg if type error
	ch_log(ਨਿਰਾ, "%s: invalid buffer argument", where);
	ਵਾਪਸ ਨਿਰਾ;
    }
    ਵਾਪਸ buf;
}

    ਸਥਿਰ ਖਾਲੀ
clear_cell(VTermScreenCell *cell)
{
    CLEAR_ਖੇਤਰ(*cell);
    cell->fg.type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_FG;
    cell->bg.type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_BG;
}

    ਸਥਿਰ ਖਾਲੀ
dump_ਪਦ_color(FILE *fd, VTermColor *color)
{
    ਪੂਰਨ ਸੂਚਕਾਂਕ;

    ਜੇ (VTERM_COLOR_IS_ਸੂਚਕਾਂਕਿਤ(color))
	ਸੂਚਕਾਂਕ = color->ਸੂਚਕਾਂਕ + 1;
    ਨਹੀਂਤਾਂ ਜੇ (color->type == 0)
	// use RGB values
	ਸੂਚਕਾਂਕ = 255;
    ਨਹੀਂਤਾਂ
	// default color
	ਸੂਚਕਾਂਕ = 0;
    fprintf(fd, "%02x%02x%02x%d",
	    (ਪੂਰਨ)color->red, (ਪੂਰਨ)color->green, (ਪੂਰਨ)color->blue,
	    ਸੂਚਕਾਂਕ);
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
    ਖਾਲੀ
f_ਪਦ_dumpwrite(typval_T *argvars, typval_T *rettv ਅਣਵਰਤਿਆ)
{
    buf_T	*buf;
    ਪਦ_T	*ਪਦ;
    char_u	*fname;
    ਪੂਰਨ		max_ਉਚਾਈ_2 = 0;
    ਪੂਰਨ		max_ਚੌੜਾਈ_2 = 0;
    stat_T	st;
    FILE	*fd;
    VTermPos	pos;
    VTermScreen *screen;
    VTermScreenCell prev_cell;
    VTermState	*state;
    VTermPos	cursor_pos;

    ਜੇ (check_restricted() || check_secure())
	ਵਾਪਸ;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_ਬਫਰ_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	ਵਾਪਸ;

    buf = ਪਦ_get_buf(argvars, "term_dumpwrite()");
    ਜੇ (buf == ਨਿਰਾ)
	ਵਾਪਸ;
    ਪਦ = buf->b_ਪਦ;
    ਜੇ (ਪਦ->tl_vterm == ਨਿਰਾ)
    {
	emsg(_(e_ਕੰਮ_already_ਮੁਕੰਮਲ));
	ਵਾਪਸ;
    }

    ਜੇ (argvars[2].v_type != VAR_UNKNOWN)
    {
	dict_T *d;

	ਜੇ (check_for_dict_arg(argvars, 2) == FAIL)
	    ਵਾਪਸ;
	d = argvars[2].vval.v_dict;
	ਜੇ (d != ਨਿਰਾ)
	{
	    max_ਉਚਾਈ_2 = dict_get_ਸੰਖਿਆ(d, "rows");
	    max_ਚੌੜਾਈ_2 = dict_get_ਸੰਖਿਆ(d, "columns");
	}
    }

    fname = tv_get_string_chk(&argvars[1]);
    ਜੇ (fname == ਨਿਰਾ)
	ਵਾਪਸ;
    ਜੇ (mch_stat((ਅੱਖਰ *)fname, &st) >= 0)
    {
	semsg(_(e_file_exists_str), fname);
	ਵਾਪਸ;
    }

    ਜੇ (*fname == NUL || (fd = mch_fopen((ਅੱਖਰ *)fname, WRITEBIN)) == ਨਿਰਾ)
    {
	semsg(_(e_cant_create_file_str), *fname == NUL ? (char_u *)_("<empty>") : fname);
	ਵਾਪਸ;
    }

    clear_cell(&prev_cell);

    screen = vterm_obtain_screen(ਪਦ->tl_vterm);
    state = vterm_obtain_state(ਪਦ->tl_vterm);
    vterm_state_get_cursorpos(state, &cursor_pos);

    ਲਈ (pos.ਪੰਕਤੀ = 0; (max_ਉਚਾਈ_2 == 0 || pos.ਪੰਕਤੀ < max_ਉਚਾਈ_2)
					 && pos.ਪੰਕਤੀ < ਪਦ->tl_ਪੰਕਤੀਆਂ; ++pos.ਪੰਕਤੀ)
    {
	ਪੂਰਨ		repeat = 0;

	ਲਈ (pos.ਖਾਨਾ = 0; (max_ਚੌੜਾਈ_2 == 0 || pos.ਖਾਨਾ < max_ਚੌੜਾਈ_2)
					 && pos.ਖਾਨਾ < ਪਦ->tl_ਖਾਨੇ_ਗਿਣਤੀ; ++pos.ਖਾਨਾ)
	{
	    VTermScreenCell cell;
	    ਪੂਰਨ		    same_attr;
	    ਪੂਰਨ		    same_chars = TRUE;
	    ਪੂਰਨ		    i;
	    ਪੂਰਨ		    is_cursor_pos = (pos.ਖਾਨਾ == cursor_pos.ਖਾਨਾ
						 && pos.ਪੰਕਤੀ == cursor_pos.ਪੰਕਤੀ);

	    ਜੇ (vterm_screen_get_cell(screen, pos, &cell) == 0)
		clear_cell(&cell);

	    ਲਈ (i = 0; i < VTERM_MAX_CHARS_ਪ੍ਰਤੀ_CELL; ++i)
	    {
		ਪੂਰਨ c = cell.chars[i];
		ਪੂਰਨ pc = prev_cell.chars[i];
		ਪੂਰਨ should_break = c == NUL || pc == NUL;

		// For the first character NUL is the same as space.
		ਜੇ (i == 0)
		{
		    c = (c == NUL) ? ' ' : c;
		    pc = (pc == NUL) ? ' ' : pc;
		}
		ਜੇ (c != pc)
		    same_chars = FALSE;
		ਜੇ (should_break)
		    ਰੋਕੋ;
	    }
	    same_attr = vtermAttr2hl(&cell.attrs)
					      == vtermAttr2hl(&prev_cell.attrs)
			&& vterm_color_is_equal(&cell.fg, &prev_cell.fg)
			&& vterm_color_is_equal(&cell.bg, &prev_cell.bg);
	    ਜੇ (same_chars && cell.ਚੌੜਾਈ == prev_cell.ਚੌੜਾਈ && same_attr
							     && !is_cursor_pos)
	    {
		++repeat;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ਜੇ (repeat > 0)
		{
		    fprintf(fd, "@%d", repeat);
		    repeat = 0;
		}
		fputs(is_cursor_pos ? ">" : "|", fd);

		ਜੇ (cell.chars[0] == NUL)
		    fputs(" ", fd);
		ਨਹੀਂਤਾਂ
		{
		    char_u	charbuf[10];
		    ਪੂਰਨ		len;

		    ਲਈ (i = 0; i < VTERM_MAX_CHARS_ਪ੍ਰਤੀ_CELL
						  && cell.chars[i] != NUL; ++i)
		    {
			len = utf_char2bytes(cell.chars[i], charbuf);
			fwrite(charbuf, len, 1, fd);
		    }
		}

		// When only the characters differ we don't write anything, the
		// following "|", "@" or NL will indicate using the same
		// attributes.
		ਜੇ (cell.ਚੌੜਾਈ != prev_cell.ਚੌੜਾਈ || !same_attr)
		{
		    ਜੇ (cell.ਚੌੜਾਈ == 2)
			fputs("*", fd);
		    ਨਹੀਂਤਾਂ
			fputs("+", fd);

		    ਜੇ (same_attr)
		    {
			fputs("&", fd);
		    }
		    ਨਹੀਂਤਾਂ
		    {
			fprintf(fd, "%d", vtermAttr2hl(&cell.attrs));
			ਜੇ (vterm_color_is_equal(&cell.fg, &prev_cell.fg))
			    fputs("&", fd);
			ਨਹੀਂਤਾਂ
			{
			    fputs("#", fd);
			    dump_ਪਦ_color(fd, &cell.fg);
			}
			ਜੇ (vterm_color_is_equal(&cell.bg, &prev_cell.bg))
			    fputs("&", fd);
			ਨਹੀਂਤਾਂ
			{
			    fputs("#", fd);
			    dump_ਪਦ_color(fd, &cell.bg);
			}
		    }
		}

		prev_cell = cell;
	    }

	    ਜੇ (cell.ਚੌੜਾਈ == 2)
		++pos.ਖਾਨਾ;
	}
	ਜੇ (repeat > 0)
	    fprintf(fd, "@%d", repeat);
	fputs("\n", fd);
    }

    fclose(fd);
}

/*
 * Called when a dump is corrupted.  Put a breakpoint here when debugging.
 */
    ਸਥਿਰ ਖਾਲੀ
dump_is_corrupt(garray_T *gap)
{
    GA_CONCAT_LITERAL(gap, "CORRUPT");
}

    ਸਥਿਰ ਖਾਲੀ
append_cell(garray_T *gap, cellattr_T *cell)
{
    ਜੇ (ga_grow(gap, 1) == FAIL)
	ਵਾਪਸ;

    *(((cellattr_T *)gap->ga_ਅੰਕੜੇ) + gap->ga_len) = *cell;
    ++gap->ga_len;
}

    ਸਥਿਰ ਖਾਲੀ
clear_cellattr(cellattr_T *cell)
{
    CLEAR_ਖੇਤਰ(*cell);
    cell->fg.type = VTERM_COLOR_DEFAULT_FG;
    cell->bg.type = VTERM_COLOR_DEFAULT_BG;
}

/*
 * Read the dump file from "fd" and append lines to the current buffer.
 * Return the cell width of the longest line.
 */
    ਸਥਿਰ ਪੂਰਨ
read_dump_file(FILE *fd, VTermPos *cursor_pos)
{
    ਪੂਰਨ		    c;
    garray_T	    ga_ਪਾਠ;
    garray_T	    ga_cell;
    string_T	    prev_char = {ਨਿਰਾ, 0};
    ਪੂਰਨ		    attr = 0;
    cellattr_T	    cell;
    cellattr_T	    ਖਾਲੀ_ਥਾਵਾਂ_cell;
    ਪਦ_T	    *ਪਦ = curbuf->b_ਪਦ;
    ਪੂਰਨ		    max_cells = 0;
    ਪੂਰਨ		    ਸ਼ੁਰੂ_ਪੰਕਤੀ = ਪਦ->tl_scrollback.ga_len;

    ga_init2(&ga_ਪਾਠ, 1, 90);
    ga_init2(&ga_cell, ਆਕਾਰ(cellattr_T), 90);
    clear_cellattr(&cell);
    clear_cellattr(&ਖਾਲੀ_ਥਾਵਾਂ_cell);
    cursor_pos->ਪੰਕਤੀ = -1;
    cursor_pos->ਖਾਨਾ = -1;

    c = fgetc(fd);
    ਲਈ (;;)
    {
	ਜੇ (c == EOF)
	    ਰੋਕੋ;
	ਜੇ (c == '\r')
	{
	    // DOS line endings?  Ignore.
	    c = fgetc(fd);
	}
	ਨਹੀਂਤਾਂ ਜੇ (c == '\n')
	{
	    // End of a line: append it to the buffer.
	    ਜੇ (ga_ਪਾਠ.ga_ਅੰਕੜੇ == ਨਿਰਾ)
		dump_is_corrupt(&ga_ਪਾਠ);
	    ਜੇ (ga_grow(&ਪਦ->tl_scrollback, 1) == OK)
	    {
		sb_ਸਤਰ_T   *ਸਤਰ = (sb_ਸਤਰ_T *)ਪਦ->tl_scrollback.ga_ਅੰਕੜੇ
						  + ਪਦ->tl_scrollback.ga_len;

		ਜੇ (max_cells < ga_cell.ga_len)
		    max_cells = ga_cell.ga_len;
		ਸਤਰ->sb_ਖਾਨੇ_ਗਿਣਤੀ = ga_cell.ga_len;
		ਸਤਰ->sb_ਬਾਈਟਾਂ = ga_ਪਾਠ.ga_len;
		ਸਤਰ->sb_cells = ga_cell.ga_ਅੰਕੜੇ;
		ਸਤਰ->sb_fill_attr = ਪਦ->tl_default_color;
		ਸਤਰ->continuation = 0;
		++ਪਦ->tl_scrollback.ga_len;
		ga_init(&ga_cell);

		ga_append(&ga_ਪਾਠ, NUL);
		ml_append(curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ, ga_ਪਾਠ.ga_ਅੰਕੜੇ,
							ga_ਪਾਠ.ga_len, FALSE);
	    }
	    ਨਹੀਂਤਾਂ
		ga_clear(&ga_cell);
	    ga_ਪਾਠ.ga_len = 0;

	    c = fgetc(fd);
	}
	ਨਹੀਂਤਾਂ ਜੇ (c == '|' || c == '>')
	{
	    ਪੂਰਨ prev_len = ga_ਪਾਠ.ga_len;

	    ਜੇ (c == '>')
	    {
		ਜੇ (cursor_pos->ਪੰਕਤੀ != -1)
		    dump_is_corrupt(&ga_ਪਾਠ);	// duplicate cursor
		cursor_pos->ਪੰਕਤੀ = ਪਦ->tl_scrollback.ga_len - ਸ਼ੁਰੂ_ਪੰਕਤੀ;
		cursor_pos->ਖਾਨਾ = ga_cell.ga_len;
	    }

	    // normal character(s) followed by "+", "*", "|", "@" or NL
	    c = fgetc(fd);
	    ਜੇ (c != EOF)
		ga_append(&ga_ਪਾਠ, c);
	    ਲਈ (;;)
	    {
		c = fgetc(fd);
		ਜੇ (c == '+' || c == '*' || c == '|' || c == '>' || c == '@'
						      || c == EOF || c == '\n')
		    ਰੋਕੋ;
		ga_append(&ga_ਪਾਠ, c);
	    }

	    // save the character for repeating it
	    VIM_CLEAR_STRING(prev_char);
	    ਜੇ (ga_ਪਾਠ.ga_ਅੰਕੜੇ != ਨਿਰਾ)
	    {
		prev_char.ਲੰਮਾਈ = (size_t)(ga_ਪਾਠ.ga_len - prev_len);
		prev_char.string = vim_strnsave(
		    ((char_u *)ga_ਪਾਠ.ga_ਅੰਕੜੇ) + prev_len,
		    prev_char.ਲੰਮਾਈ);
		ਜੇ (prev_char.string == ਨਿਰਾ)
		    prev_char.ਲੰਮਾਈ = 0;
	    }

	    ਜੇ (c == '@' || c == '|' || c == '>' || c == '\n')
	    {
		// use all attributes from previous cell
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (c == '+' || c == '*')
	    {
		ਪੂਰਨ is_bg;

		cell.ਚੌੜਾਈ = c == '+' ? 1 : 2;

		c = fgetc(fd);
		ਜੇ (c == '&')
		{
		    // use same attr as previous cell
		    c = fgetc(fd);
		}
		ਨਹੀਂਤਾਂ ਜੇ (ਸੁਰੱਖਿਅਤ_isdigit(c))
		{
		    // get the decimal attribute
		    attr = 0;
		    ਜਦੋਂ (ਸੁਰੱਖਿਅਤ_isdigit(c))
		    {
			attr = attr * 10 + (c - '0');
			c = fgetc(fd);
		    }
		    hl2vtermAttr(attr, &cell);

		    // is_bg == 0: fg, is_bg == 1: bg
		    ਲਈ (is_bg = 0; is_bg <= 1; ++is_bg)
		    {
			ਜੇ (c == '&')
			{
			    // use same color as previous cell
			    c = fgetc(fd);
			}
			ਨਹੀਂਤਾਂ ਜੇ (c == '#')
			{
			    ਪੂਰਨ red, green, blue, ਸੂਚਕਾਂਕ = 0, type;

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
			    ਜੇ (!ਸੁਰੱਖਿਅਤ_isdigit(c))
				dump_is_corrupt(&ga_ਪਾਠ);
			    ਜਦੋਂ (ਸੁਰੱਖਿਅਤ_isdigit(c))
			    {
				ਸੂਚਕਾਂਕ = ਸੂਚਕਾਂਕ * 10 + (c - '0');
				c = fgetc(fd);
			    }
			    ਜੇ (ਸੂਚਕਾਂਕ == 0 || ਸੂਚਕਾਂਕ == 255)
			    {
				type = VTERM_COLOR_RGB;
				ਜੇ (ਸੂਚਕਾਂਕ == 0)
				{
				    ਜੇ (is_bg)
					type |= VTERM_COLOR_DEFAULT_BG;
				    ਨਹੀਂਤਾਂ
					type |= VTERM_COLOR_DEFAULT_FG;
				}
			    }
			    ਨਹੀਂਤਾਂ
			    {
				type = VTERM_COLOR_ਸੂਚਕਾਂਕਿਤ;
				ਸੂਚਕਾਂਕ -= 1;
			    }
			    ਜੇ (is_bg)
			    {
				cell.bg.type = type;
				cell.bg.red = red;
				cell.bg.green = green;
				cell.bg.blue = blue;
				cell.bg.ਸੂਚਕਾਂਕ = ਸੂਚਕਾਂਕ;
			    }
			    ਨਹੀਂਤਾਂ
			    {
				cell.fg.type = type;
				cell.fg.red = red;
				cell.fg.green = green;
				cell.fg.blue = blue;
				cell.fg.ਸੂਚਕਾਂਕ = ਸੂਚਕਾਂਕ;
			    }
			}
			ਨਹੀਂਤਾਂ
			    dump_is_corrupt(&ga_ਪਾਠ);
		    }
		}
		ਨਹੀਂਤਾਂ
		    dump_is_corrupt(&ga_ਪਾਠ);
	    }
	    ਨਹੀਂਤਾਂ
		dump_is_corrupt(&ga_ਪਾਠ);

	    append_cell(&ga_cell, &cell);
	    ਜੇ (cell.ਚੌੜਾਈ == 2)
		append_cell(&ga_cell, &ਖਾਲੀ_ਥਾਵਾਂ_cell);
	}
	ਨਹੀਂਤਾਂ ਜੇ (c == '@')
	{
	    ਜੇ (prev_char.string == ਨਿਰਾ)
		dump_is_corrupt(&ga_ਪਾਠ);
	    ਨਹੀਂਤਾਂ
	    {
		ਪੂਰਨ ਗਿਣਤੀ_2 = 0;

		// repeat previous character, get the count
		ਲਈ (;;)
		{
		    c = fgetc(fd);
		    ਜੇ (!ਸੁਰੱਖਿਅਤ_isdigit(c))
			ਰੋਕੋ;
		    ਗਿਣਤੀ_2 = ਗਿਣਤੀ_2 * 10 + (c - '0');
		}

		ਜਦੋਂ (ਗਿਣਤੀ_2-- > 0)
		{
		    ga_concat_len(&ga_ਪਾਠ, prev_char.string, prev_char.ਲੰਮਾਈ);
		    append_cell(&ga_cell, &cell);
		}
	    }
	}
	ਨਹੀਂਤਾਂ
	{
	    dump_is_corrupt(&ga_ਪਾਠ);
	    c = fgetc(fd);
	}
    }

    ਜੇ (ga_ਪਾਠ.ga_len > 0)
    {
	// trailing characters after last NL
	dump_is_corrupt(&ga_ਪਾਠ);
	ga_append(&ga_ਪਾਠ, NUL);
	ml_append(curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ, ga_ਪਾਠ.ga_ਅੰਕੜੇ,
							ga_ਪਾਠ.ga_len, FALSE);
    }

    ga_clear(&ga_ਪਾਠ);
    ga_clear(&ga_cell);
    vim_free(prev_char.string);

    ਵਾਪਸ max_cells;
}

/*
 * Return an allocated string with at least "text_width" "=" characters and
 * "fname" inserted in the middle.
 */
    ਸਥਿਰ char_u *
get_separator(ਪੂਰਨ ਪਾਠ_ਚੌੜਾਈ, char_u *fname)
{
    ਪੂਰਨ	    ਚੌੜਾਈ = MAX(ਪਾਠ_ਚੌੜਾਈ, curwin->w_ਚੌੜਾਈ);
    char_u  *textline;
    ਪੂਰਨ	    fname_ਮਾਪ;
    char_u  *p = fname;
    ਪੂਰਨ	    i;
    size_t  off;

    textline = alloc(ਚੌੜਾਈ + STRLEN(fname) + 1);
    ਜੇ (textline == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    fname_ਮਾਪ = vim_strsize(fname);
    ਜੇ (fname_ਮਾਪ < ਚੌੜਾਈ - 8)
    {
	// enough room, don't use the full window width
	ਚੌੜਾਈ = MAX(ਪਾਠ_ਚੌੜਾਈ, fname_ਮਾਪ + 8);
    }
    ਨਹੀਂਤਾਂ ਜੇ (fname_ਮਾਪ > ਚੌੜਾਈ - 8)
    {
	// full name doesn't fit, use only the tail
	p = gettail(fname);
	fname_ਮਾਪ = vim_strsize(p);
    }
    // skip characters until the name fits
    ਜਦੋਂ (fname_ਮਾਪ > ਚੌੜਾਈ - 8)
    {
	p += (*mb_ptr2len)(p);
	fname_ਮਾਪ = vim_strsize(p);
    }

    ਲਈ (i = 0; i < (ਚੌੜਾਈ - fname_ਮਾਪ) / 2 - 1; ++i)
	textline[i] = '=';
    textline[i++] = ' ';

    STRCPY(textline + i, p);
    off = STRLEN(textline);
    textline[off] = ' ';
    ਲਈ (i = 1; i < (ਚੌੜਾਈ - fname_ਮਾਪ) / 2; ++i)
	textline[off + i] = '=';
    textline[off + i] = NUL;

    ਵਾਪਸ textline;
}

/*
 * Common for "term_dumpdiff()" and "term_dumpload()".
 */
    ਸਥਿਰ ਖਾਲੀ
ਪਦ_ਲੱਦੋ_dump(typval_T *argvars, typval_T *rettv, ਪੂਰਨ do_diff)
{
    jobopt_T	opt;
    buf_T	*buf = ਨਿਰਾ;
    char_u	buf1[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    char_u	*fname1;
    char_u	*fname2 = ਨਿਰਾ;
    char_u	*fname_tofree = ਨਿਰਾ;
    FILE	*fd1;
    FILE	*fd2 = ਨਿਰਾ;
    char_u	*textline = ਨਿਰਾ;

    // First open the files.  If this fails bail out.
    fname1 = tv_get_string_buf_chk(&argvars[0], buf1);
    ਜੇ (do_diff)
	fname2 = tv_get_string_buf_chk(&argvars[1], buf2);
    ਜੇ (fname1 == ਨਿਰਾ || (do_diff && fname2 == ਨਿਰਾ))
    {
	emsg(_(e_invalid_ਦਲੀਲ));
	ਵਾਪਸ;
    }
    fd1 = mch_fopen((ਅੱਖਰ *)fname1, READBIN);
    ਜੇ (fd1 == ਨਿਰਾ)
    {
	semsg(_(e_cant_read_file_str), fname1);
	ਵਾਪਸ;
    }
    ਜੇ (do_diff)
    {
	fd2 = mch_fopen((ਅੱਖਰ *)fname2, READBIN);
	ਜੇ (fd2 == ਨਿਰਾ)
	{
	    fclose(fd1);
	    semsg(_(e_cant_read_file_str), fname2);
	    ਵਾਪਸ;
	}
    }

    init_ਕੰਮ_options(&opt);
    ਜੇ (argvars[do_diff ? 2 : 1].v_type != VAR_UNKNOWN
	    && get_ਕੰਮ_options(&argvars[do_diff ? 2 : 1], &opt, 0,
		    JO2_ਪਦ_NAME + JO2_ਪਦ_ਖਾਨੇ_ਗਿਣਤੀ + JO2_ਪਦ_ਪੰਕਤੀਆਂ
		    + JO2_VERTICAL + JO2_CURWIN + JO2_NORESTORE) == FAIL)
	ਜਾਓ theend;

    ਜੇ (opt.jo_ਪਦ_name == ਨਿਰਾ)
    {
	size_t len = STRLEN(fname1) + 12;

	fname_tofree = alloc(len);
	ਜੇ (fname_tofree != ਨਿਰਾ)
	{
	    vim_snprintf((ਅੱਖਰ *)fname_tofree, len, "dump diff %s", fname1);
	    opt.jo_ਪਦ_name = fname_tofree;
	}
    }

    ਜੇ (opt.jo_bufnr_buf != ਨਿਰਾ)
    {
	win_T *wp = buf_jump_open_win(opt.jo_bufnr_buf);

	// With "bufnr" argument: enter the window with this buffer and make it
	// empty.
	ਜੇ (wp == ਨਿਰਾ)
	    semsg(_(e_invalid_ਦਲੀਲ_str), "bufnr");
	ਨਹੀਂਤਾਂ
	{
	    buf = curbuf;
	    ਜਦੋਂ (!(curbuf->b_ml.ml_flags & ML_ਖਾਲੀ_ਥਾਵਾਂ))
		ml_delete((linenr_T)1);
	    free_scrollback(curbuf->b_ਪਦ);
	    redraw_later(UPD_NOT_VALID);
	}
    }
    ਨਹੀਂਤਾਂ
	// Create a new terminal window.
	buf = ਪਦ_ਸ਼ੁਰੂ(&argvars[0], ਨਿਰਾ, &opt, ਪਦ_ਸ਼ੁਰੂ_NOJOB);

    ਜੇ (buf != ਨਿਰਾ && buf->b_ਪਦ != ਨਿਰਾ)
    {
	ਪੂਰਨ		i;
	linenr_T	bot_lnum;
	linenr_T	lnum;
	ਪਦ_T		*ਪਦ = buf->b_ਪਦ;
	ਪੂਰਨ		ਚੌੜਾਈ;
	ਪੂਰਨ		width2;
	VTermPos	cursor_pos1;
	VTermPos	cursor_pos2;

	ਪਦ_init_default_colors(ਪਦ);

	rettv->vval.v_ਸੰਖਿਆ = buf->b_fnum;

	// read the files, fill the buffer with the diff
	ਚੌੜਾਈ = read_dump_file(fd1, &cursor_pos1);

	// position the cursor
	ਜੇ (cursor_pos1.ਪੰਕਤੀ >= 0)
	{
	    curwin->w_cursor.lnum = cursor_pos1.ਪੰਕਤੀ + 1;
	    coladvance(cursor_pos1.ਖਾਨਾ);
	}

	// Delete the empty line that was in the empty buffer.
	ml_delete(1);

	// For term_dumpload() we are done here.
	ਜੇ (!do_diff)
	    ਜਾਓ theend;

	ਪਦ->tl_ਸਿਖਰ_diff_ਪੰਕਤੀਆਂ = curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;

	textline = get_separator(ਚੌੜਾਈ, fname1);
	ਜੇ (textline == ਨਿਰਾ)
	    ਜਾਓ theend;
	ਜੇ (ਜੋੜੋ_ਖਾਲੀ_ਥਾਵਾਂ_scrollback(ਪਦ, &ਪਦ->tl_default_color, 0) == OK)
	    ml_append(curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ, textline, 0, FALSE);
	vim_free(textline);

	textline = get_separator(ਚੌੜਾਈ, fname2);
	ਜੇ (textline == ਨਿਰਾ)
	    ਜਾਓ theend;
	ਜੇ (ਜੋੜੋ_ਖਾਲੀ_ਥਾਵਾਂ_scrollback(ਪਦ, &ਪਦ->tl_default_color, 0) == OK)
	    ml_append(curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ, textline, 0, FALSE);
	textline[ਚੌੜਾਈ] = NUL;

	bot_lnum = curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
	width2 = read_dump_file(fd2, &cursor_pos2);
	ਜੇ (width2 > ਚੌੜਾਈ)
	{
	    vim_free(textline);
	    textline = alloc(width2 + 1);
	    ਜੇ (textline == ਨਿਰਾ)
		ਜਾਓ theend;
	    ਚੌੜਾਈ = width2;
	    textline[ਚੌੜਾਈ] = NUL;
	}
	ਪਦ->tl_bot_diff_ਪੰਕਤੀਆਂ = curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ - bot_lnum;

	ਲਈ (lnum = 1; lnum <= ਪਦ->tl_ਸਿਖਰ_diff_ਪੰਕਤੀਆਂ; ++lnum)
	{
	    ਜੇ (lnum + bot_lnum > curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	    {
		// bottom part has fewer rows, fill with "-"
		ਲਈ (i = 0; i < ਚੌੜਾਈ; ++i)
		    textline[i] = '-';
	    }
	    ਨਹੀਂਤਾਂ
	    {
		char_u *line1;
		char_u *line2;
		char_u *p1;
		char_u *p2;
		ਪੂਰਨ	ਖਾਨਾ;
		sb_ਸਤਰ_T   *sb_ਸਤਰ = (sb_ਸਤਰ_T *)ਪਦ->tl_scrollback.ga_ਅੰਕੜੇ;
		cellattr_T *cellattr1 = (sb_ਸਤਰ + lnum - 1)->sb_cells;
		cellattr_T *cellattr2 = (sb_ਸਤਰ + lnum + bot_lnum - 1)
								    ->sb_cells;

		// Make a copy, getting the second line will invalidate it.
		line1 = vim_strsave(ml_get(lnum));
		ਜੇ (line1 == ਨਿਰਾ)
		    ਰੋਕੋ;
		p1 = line1;

		line2 = ml_get(lnum + bot_lnum);
		p2 = line2;
		ਲਈ (ਖਾਨਾ = 0; ਖਾਨਾ < ਚੌੜਾਈ && *p1 != NUL && *p2 != NUL; ++ਖਾਨਾ)
		{
		    ਪੂਰਨ len1 = utfc_ptr2len(p1);
		    ਪੂਰਨ len2 = utfc_ptr2len(p2);

		    textline[ਖਾਨਾ] = ' ';
		    ਜੇ (len1 != len2 || STRNCMP(p1, p2, len1) != 0)
			// text differs
			textline[ਖਾਨਾ] = 'X';
		    ਨਹੀਂਤਾਂ ਜੇ (lnum == cursor_pos1.ਪੰਕਤੀ + 1
			    && ਖਾਨਾ == cursor_pos1.ਖਾਨਾ
			    && (cursor_pos1.ਪੰਕਤੀ != cursor_pos2.ਪੰਕਤੀ
					|| cursor_pos1.ਖਾਨਾ != cursor_pos2.ਖਾਨਾ))
			// cursor in first but not in second
			textline[ਖਾਨਾ] = '>';
		    ਨਹੀਂਤਾਂ ਜੇ (lnum == cursor_pos2.ਪੰਕਤੀ + 1
			    && ਖਾਨਾ == cursor_pos2.ਖਾਨਾ
			    && (cursor_pos1.ਪੰਕਤੀ != cursor_pos2.ਪੰਕਤੀ
					|| cursor_pos1.ਖਾਨਾ != cursor_pos2.ਖਾਨਾ))
			// cursor in second but not in first
			textline[ਖਾਨਾ] = '<';
		    ਨਹੀਂਤਾਂ ਜੇ (cellattr1 != ਨਿਰਾ && cellattr2 != ਨਿਰਾ)
		    {
			ਜੇ ((cellattr1 + ਖਾਨਾ)->ਚੌੜਾਈ
						   != (cellattr2 + ਖਾਨਾ)->ਚੌੜਾਈ)
			    textline[ਖਾਨਾ] = 'w';
			ਨਹੀਂਤਾਂ ਜੇ (!vterm_color_is_equal(&(cellattr1 + ਖਾਨਾ)->fg,
						   &(cellattr2 + ਖਾਨਾ)->fg))
			    textline[ਖਾਨਾ] = 'f';
			ਨਹੀਂਤਾਂ ਜੇ (!vterm_color_is_equal(&(cellattr1 + ਖਾਨਾ)->bg,
						   &(cellattr2 + ਖਾਨਾ)->bg))
			    textline[ਖਾਨਾ] = 'b';
			ਨਹੀਂਤਾਂ ਜੇ (vtermAttr2hl(&(cellattr1 + ਖਾਨਾ)->attrs)
				  != vtermAttr2hl(&((cellattr2 + ਖਾਨਾ)->attrs)))
			    textline[ਖਾਨਾ] = 'a';
		    }
		    p1 += len1;
		    p2 += len2;
		    // TODO: handle different width
		}

		ਜਦੋਂ (ਖਾਨਾ < ਚੌੜਾਈ)
		{
		    ਜੇ (*p1 == NUL && *p2 == NUL)
			textline[ਖਾਨਾ] = '?';
		    ਨਹੀਂਤਾਂ ਜੇ (*p1 == NUL)
		    {
			textline[ਖਾਨਾ] = '+';
			p2 += utfc_ptr2len(p2);
		    }
		    ਨਹੀਂਤਾਂ
		    {
			textline[ਖਾਨਾ] = '-';
			p1 += utfc_ptr2len(p1);
		    }
		    ++ਖਾਨਾ;
		}

		vim_free(line1);
	    }
	    ਜੇ (ਜੋੜੋ_ਖਾਲੀ_ਥਾਵਾਂ_scrollback(ਪਦ, &ਪਦ->tl_default_color,
						 ਪਦ->tl_ਸਿਖਰ_diff_ਪੰਕਤੀਆਂ) == OK)
		ml_append(ਪਦ->tl_ਸਿਖਰ_diff_ਪੰਕਤੀਆਂ + lnum, textline, 0, FALSE);
	    ++bot_lnum;
	}

	ਜਦੋਂ (lnum + bot_lnum <= curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	{
	    // bottom part has more rows, fill with "+"
	    ਲਈ (i = 0; i < ਚੌੜਾਈ; ++i)
		textline[i] = '+';
	    ਜੇ (ਜੋੜੋ_ਖਾਲੀ_ਥਾਵਾਂ_scrollback(ਪਦ, &ਪਦ->tl_default_color,
						 ਪਦ->tl_ਸਿਖਰ_diff_ਪੰਕਤੀਆਂ) == OK)
		ml_append(ਪਦ->tl_ਸਿਖਰ_diff_ਪੰਕਤੀਆਂ + lnum, textline, 0, FALSE);
	    ++lnum;
	    ++bot_lnum;
	}

	ਪਦ->tl_ਖਾਨੇ_ਗਿਣਤੀ = ਚੌੜਾਈ;

	// looks better without wrapping
	curwin->w_p_wrap = 0;
    }

theend:
    vim_free(textline);
    vim_free(fname_tofree);
    fclose(fd1);
    ਜੇ (fd2 != ਨਿਰਾ)
	fclose(fd2);
}

/*
 * If the current buffer shows the output of term_dumpdiff(), swap the top and
 * bottom files.
 * Return FAIL when this is not possible.
 */
    ਪੂਰਨ
ਪਦ_ਅਦਲਾ_ਬਦਲੀ_diff(ਖਾਲੀ)
{
    ਪਦ_T	*ਪਦ = curbuf->b_ਪਦ;
    linenr_T	ਸਤਰ_ਗਿਣਤੀ;
    linenr_T	ਸਿਖਰ_ਪੰਕਤੀਆਂ;
    linenr_T	bot_ਪੰਕਤੀਆਂ;
    linenr_T	bot_ਸ਼ੁਰੂ;
    linenr_T	lnum;
    char_u	*p;
    sb_ਸਤਰ_T	*sb_ਸਤਰ;

    ਜੇ (ਪਦ == ਨਿਰਾ
	    || !ਪਦ_is_ਮੁਕੰਮਲ(curbuf)
	    || ਪਦ->tl_ਸਿਖਰ_diff_ਪੰਕਤੀਆਂ == 0
	    || ਪਦ->tl_scrollback.ga_len == 0)
	ਵਾਪਸ FAIL;

    ਸਤਰ_ਗਿਣਤੀ = curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
    ਸਿਖਰ_ਪੰਕਤੀਆਂ = ਪਦ->tl_ਸਿਖਰ_diff_ਪੰਕਤੀਆਂ;
    bot_ਪੰਕਤੀਆਂ = ਪਦ->tl_bot_diff_ਪੰਕਤੀਆਂ;
    bot_ਸ਼ੁਰੂ = ਸਤਰ_ਗਿਣਤੀ - bot_ਪੰਕਤੀਆਂ;
    sb_ਸਤਰ = (sb_ਸਤਰ_T *)ਪਦ->tl_scrollback.ga_ਅੰਕੜੇ;

    // move lines from top to above the bottom part
    ਲਈ (lnum = 1; lnum <= ਸਿਖਰ_ਪੰਕਤੀਆਂ; ++lnum)
    {
	p = vim_strsave(ml_get(1));
	ਜੇ (p == ਨਿਰਾ)
	    ਵਾਪਸ OK;
	ml_append(bot_ਸ਼ੁਰੂ, p, 0, FALSE);
	ml_delete(1);
	vim_free(p);
    }

    // move lines from bottom to the top
    ਲਈ (lnum = 1; lnum <= bot_ਪੰਕਤੀਆਂ; ++lnum)
    {
	p = vim_strsave(ml_get(bot_ਸ਼ੁਰੂ + lnum));
	ਜੇ (p == ਨਿਰਾ)
	    ਵਾਪਸ OK;
	ml_delete(bot_ਸ਼ੁਰੂ + lnum);
	ml_append(lnum - 1, p, 0, FALSE);
	vim_free(p);
    }

    // move top title to bottom
    p = vim_strsave(ml_get(bot_ਪੰਕਤੀਆਂ + 1));
    ਜੇ (p == ਨਿਰਾ)
	ਵਾਪਸ OK;
    ml_append(ਸਤਰ_ਗਿਣਤੀ - ਸਿਖਰ_ਪੰਕਤੀਆਂ - 1, p, 0, FALSE);
    ml_delete(bot_ਪੰਕਤੀਆਂ + 1);
    vim_free(p);

    // move bottom title to top
    p = vim_strsave(ml_get(ਸਤਰ_ਗਿਣਤੀ - ਸਿਖਰ_ਪੰਕਤੀਆਂ));
    ਜੇ (p == ਨਿਰਾ)
	ਵਾਪਸ OK;
    ml_delete(ਸਤਰ_ਗਿਣਤੀ - ਸਿਖਰ_ਪੰਕਤੀਆਂ);
    ml_append(bot_ਪੰਕਤੀਆਂ, p, 0, FALSE);
    vim_free(p);

    ਜੇ (ਸਿਖਰ_ਪੰਕਤੀਆਂ == bot_ਪੰਕਤੀਆਂ)
    {
	// rows counts are equal, can swap cell properties
	ਲਈ (lnum = 0; lnum < ਸਿਖਰ_ਪੰਕਤੀਆਂ; ++lnum)
	{
	    sb_ਸਤਰ_T	ਆਰਜ਼ੀ;

	    ਆਰਜ਼ੀ = *(sb_ਸਤਰ + lnum);
	    *(sb_ਸਤਰ + lnum) = *(sb_ਸਤਰ + bot_ਸ਼ੁਰੂ + lnum);
	    *(sb_ਸਤਰ + bot_ਸ਼ੁਰੂ + lnum) = ਆਰਜ਼ੀ;
	}
    }
    ਨਹੀਂਤਾਂ
    {
	size_t		ਮਾਪ = ਆਕਾਰ(sb_ਸਤਰ_T) * ਪਦ->tl_scrollback.ga_len;
	sb_ਸਤਰ_T	*ਆਰਜ਼ੀ = alloc(ਮਾਪ);

	// need to copy cell properties into temp memory
	ਜੇ (ਆਰਜ਼ੀ != ਨਿਰਾ)
	{
	    mch_memmove(ਆਰਜ਼ੀ, ਪਦ->tl_scrollback.ga_ਅੰਕੜੇ, ਮਾਪ);
	    mch_memmove(ਪਦ->tl_scrollback.ga_ਅੰਕੜੇ,
		    ਆਰਜ਼ੀ + bot_ਸ਼ੁਰੂ,
		    ਆਕਾਰ(sb_ਸਤਰ_T) * bot_ਪੰਕਤੀਆਂ);
	    mch_memmove((sb_ਸਤਰ_T *)ਪਦ->tl_scrollback.ga_ਅੰਕੜੇ + bot_ਪੰਕਤੀਆਂ,
		    ਆਰਜ਼ੀ + ਸਿਖਰ_ਪੰਕਤੀਆਂ,
		    ਆਕਾਰ(sb_ਸਤਰ_T) * (ਸਤਰ_ਗਿਣਤੀ - ਸਿਖਰ_ਪੰਕਤੀਆਂ - bot_ਪੰਕਤੀਆਂ));
	    mch_memmove((sb_ਸਤਰ_T *)ਪਦ->tl_scrollback.ga_ਅੰਕੜੇ
						       + ਸਤਰ_ਗਿਣਤੀ - ਸਿਖਰ_ਪੰਕਤੀਆਂ,
		    ਆਰਜ਼ੀ,
		    ਆਕਾਰ(sb_ਸਤਰ_T) * ਸਿਖਰ_ਪੰਕਤੀਆਂ);
	    vim_free(ਆਰਜ਼ੀ);
	}
    }

    ਪਦ->tl_ਸਿਖਰ_diff_ਪੰਕਤੀਆਂ = bot_ਪੰਕਤੀਆਂ;
    ਪਦ->tl_bot_diff_ਪੰਕਤੀਆਂ = ਸਿਖਰ_ਪੰਕਤੀਆਂ;

    update_screen(UPD_NOT_VALID);
    ਵਾਪਸ OK;
}

/*
 * "term_dumpdiff(filename, filename, options)" function
 */
    ਖਾਲੀ
f_ਪਦ_dumpdiff(typval_T *argvars, typval_T *rettv)
{
    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	ਵਾਪਸ;

    ਪਦ_ਲੱਦੋ_dump(argvars, rettv, TRUE);
}

/*
 * "term_dumpload(filename, options)" function
 */
    ਖਾਲੀ
f_ਪਦ_dumpload(typval_T *argvars, typval_T *rettv)
{
    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    ਪਦ_ਲੱਦੋ_dump(argvars, rettv, FALSE);
}

/*
 * "term_getaltscreen(buf)" function
 */
    ਖਾਲੀ
f_ਪਦ_getaltscreen(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_ਬਫਰ_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    buf = ਪਦ_get_buf(argvars, "term_getaltscreen()");
    ਜੇ (buf == ਨਿਰਾ)
	ਵਾਪਸ;
    rettv->vval.v_ਸੰਖਿਆ = buf->b_ਪਦ->tl_using_altscreen;
}

/*
 * "term_getattr(attr, name)" function
 */
    ਖਾਲੀ
f_ਪਦ_getattr(typval_T *argvars, typval_T *rettv)
{
    ਪੂਰਨ	    attr;
    size_t  i;
    char_u  *name;

    ਸਥਿਰ ਬਣਤਰ {
	ਅੱਖਰ	    *name;
	ਪੂਰਨ	    attr;
    } attrs[] = {
	{"bold",      HL_BOLD},
	{"italic",    HL_ITALIC},
	{"underline", HL_UNDERLINE},
	{"strike",    HL_STRIKETHROUGH},
	{"reverse",   HL_INVERSE},
    };

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_ਸੰਖਿਆ_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    attr = tv_get_ਸੰਖਿਆ(&argvars[0]);
    name = tv_get_string_chk(&argvars[1]);
    ਜੇ (name == ਨਿਰਾ)
	ਵਾਪਸ;

    ਜੇ (attr > HL_ALL)
	attr = syn_attr2attr(attr);
    ਲਈ (i = 0; i < ARRAY_ਲੰਮਾਈ(attrs); ++i)
	ਜੇ (STRCMP(name, attrs[i].name) == 0)
	{
	    rettv->vval.v_ਸੰਖਿਆ = (attr & attrs[i].attr) != 0 ? 1 : 0;
	    ਰੋਕੋ;
	}
}

/*
 * "term_getcursor(buf)" function
 */
    ਖਾਲੀ
f_ਪਦ_getcursor(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    ਪਦ_T	*ਪਦ;
    list_T	*l;
    dict_T	*d;

    ਜੇ (rettv_list_alloc(rettv) == FAIL)
	ਵਾਪਸ;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_ਬਫਰ_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    buf = ਪਦ_get_buf(argvars, "term_getcursor()");
    ਜੇ (buf == ਨਿਰਾ)
	ਵਾਪਸ;
    ਪਦ = buf->b_ਪਦ;

    l = rettv->vval.v_list;
    list_append_ਸੰਖਿਆ(l, ਪਦ->tl_cursor_pos.ਪੰਕਤੀ + 1);
    list_append_ਸੰਖਿਆ(l, ਪਦ->tl_cursor_pos.ਖਾਨਾ + 1);

    d = dict_alloc();
    ਜੇ (d == ਨਿਰਾ)
	ਵਾਪਸ;

    dict_ਜੋੜੋ_ਸੰਖਿਆ(d, "visible", ਪਦ->tl_cursor_visible);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(d, "blink", blink_state_is_inverted()
	    ? !ਪਦ->tl_cursor_blink : ਪਦ->tl_cursor_blink);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(d, "shape", ਪਦ->tl_cursor_shape);
    dict_ਜੋੜੋ_string(d, "color", cursor_color_get(ਪਦ->tl_cursor_color));
    ਜੇ (list_append_dict(l, d) == FAIL)
	dict_unref(d);
}

/*
 * "term_getjob(buf)" function
 */
    ਖਾਲੀ
f_ਪਦ_getjob(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_ਬਫਰ_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    buf = ਪਦ_get_buf(argvars, "term_getjob()");
    ਜੇ (buf == ਨਿਰਾ)
    {
	ਜੇ (ਵਿੱਚ_vim9script())
	{
	    rettv->v_type = VAR_ਕੰਮ;
	    rettv->vval.v_ਕੰਮ = ਨਿਰਾ;
	}
	ਨਹੀਂਤਾਂ
	{
	    rettv->v_type = VAR_SPECIAL;
	    rettv->vval.v_ਸੰਖਿਆ = VVAL_NULL;
	}
	ਵਾਪਸ;
    }

    rettv->v_type = VAR_ਕੰਮ;
    rettv->vval.v_ਕੰਮ = buf->b_ਪਦ->tl_ਕੰਮ;
    ਜੇ (rettv->vval.v_ਕੰਮ != ਨਿਰਾ)
	++rettv->vval.v_ਕੰਮ->jv_refcount;
}

    ਸਥਿਰ ਪੂਰਨ
get_ਪੰਕਤੀ_ਸੰਖਿਆ(typval_T *tv, ਪਦ_T *ਪਦ)
{
    ਜੇ (tv->v_type == VAR_STRING
	    && tv->vval.v_string != ਨਿਰਾ
	    && STRCMP(tv->vval.v_string, ".") == 0)
	ਵਾਪਸ ਪਦ->tl_cursor_pos.ਪੰਕਤੀ;
    ਵਾਪਸ (ਪੂਰਨ)tv_get_ਸੰਖਿਆ(tv) - 1;
}

/*
 * "term_getline(buf, row)" function
 */
    ਖਾਲੀ
f_ਪਦ_getline(typval_T *argvars, typval_T *rettv)
{
    buf_T	    *buf;
    ਪਦ_T	    *ਪਦ;
    ਪੂਰਨ		    ਪੰਕਤੀ;

    rettv->v_type = VAR_STRING;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_ਬਫਰ_arg(argvars, 0) == FAIL
		|| check_for_lnum_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    buf = ਪਦ_get_buf(argvars, "term_getline()");
    ਜੇ (buf == ਨਿਰਾ)
	ਵਾਪਸ;
    ਪਦ = buf->b_ਪਦ;
    ਪੰਕਤੀ = get_ਪੰਕਤੀ_ਸੰਖਿਆ(&argvars[1], ਪਦ);

    ਜੇ (ਪਦ->tl_vterm == ਨਿਰਾ)
    {
	linenr_T  lnum = 0;
	size_t	  ਵਿੱਥ = 0;
	ਪੂਰਨ	  sb_ਪੰਕਤੀ = ਪਦ->tl_scrollback_scrolled + ਪੰਕਤੀ;
	sb_ਸਤਰ_T *ਸਤਰ;

	ਜੇ (sb_ਪੰਕਤੀ < 0 || sb_ਪੰਕਤੀ >= ਪਦ->tl_scrollback.ga_len)
	    ਵਾਪਸ;
	ਸਤਰ = (sb_ਸਤਰ_T *)ਪਦ->tl_scrollback.ga_ਅੰਕੜੇ + sb_ਪੰਕਤੀ;

	scrollbackline_pos_ਵਿੱਚ_buf(ਪਦ, sb_ਪੰਕਤੀ, &lnum, ਨਿਰਾ, &ਵਿੱਥ);

	// vterm is finished, get the text from the buffer
	ਜੇ (lnum > 0 && lnum <= buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	{
	    char_u *p = ml_get_buf(buf, lnum, FALSE);
	    ਜੇ (STRLEN(p) >= ਵਿੱਥ + ਸਤਰ->sb_ਬਾਈਟਾਂ)
		rettv->vval.v_string = vim_strnsave(p + ਵਿੱਥ, ਸਤਰ->sb_ਬਾਈਟਾਂ);
	}
    }
    ਨਹੀਂਤਾਂ
    {
	VTermScreen	*screen = vterm_obtain_screen(ਪਦ->tl_vterm);
	VTermRect	rect;
	ਪੂਰਨ		len;
	char_u		*p;

	ਜੇ (ਪੰਕਤੀ < 0 || ਪੰਕਤੀ >= ਪਦ->tl_ਪੰਕਤੀਆਂ)
	    ਵਾਪਸ;
	len = ਪਦ->tl_ਖਾਨੇ_ਗਿਣਤੀ * MB_MAXBYTES + 1;
	p = alloc(len);
	ਜੇ (p == ਨਿਰਾ)
	    ਵਾਪਸ;
	rettv->vval.v_string = p;

	rect.ਸ਼ੁਰੂ_ਖਾਨਾ = 0;
	rect.ਸਿਰਾ_ਖਾਨਾ = ਪਦ->tl_ਖਾਨੇ_ਗਿਣਤੀ;
	rect.ਸ਼ੁਰੂ_ਪੰਕਤੀ = ਪੰਕਤੀ;
	rect.ਸਿਰਾ_ਪੰਕਤੀ = ਪੰਕਤੀ + 1;
	p[vterm_screen_get_ਪਾਠ(screen, (ਅੱਖਰ *)p, len, rect)] = NUL;
    }
}

/*
 * "term_getscrolled(buf)" function
 */
    ਖਾਲੀ
f_ਪਦ_getscrolled(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_ਬਫਰ_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    buf = ਪਦ_get_buf(argvars, "term_getscrolled()");
    ਜੇ (buf == ਨਿਰਾ)
	ਵਾਪਸ;
    rettv->vval.v_ਸੰਖਿਆ = buf->b_ਪਦ->tl_ਬਫਰ_scrolled;
}

/*
 * "term_getsize(buf)" function
 */
    ਖਾਲੀ
f_ਪਦ_getsize(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    list_T	*l;

    ਜੇ (rettv_list_alloc(rettv) == FAIL)
	ਵਾਪਸ;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_ਬਫਰ_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    buf = ਪਦ_get_buf(argvars, "term_getsize()");
    ਜੇ (buf == ਨਿਰਾ)
	ਵਾਪਸ;

    l = rettv->vval.v_list;
    list_append_ਸੰਖਿਆ(l, buf->b_ਪਦ->tl_ਪੰਕਤੀਆਂ);
    list_append_ਸੰਖਿਆ(l, buf->b_ਪਦ->tl_ਖਾਨੇ_ਗਿਣਤੀ);
}

/*
 * "term_setsize(buf, rows, cols)" function
 */
    ਖਾਲੀ
f_ਪਦ_setsize(typval_T *argvars, typval_T *rettv ਅਣਵਰਤਿਆ)
{
    buf_T	*buf;
    ਪਦ_T	*ਪਦ;
    varnumber_T ਪੰਕਤੀਆਂ_2, ਖਾਨੇ_ਗਿਣਤੀ;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_ਬਫਰ_arg(argvars, 0) == FAIL
		|| check_for_ਸੰਖਿਆ_arg(argvars, 1) == FAIL
		|| check_for_ਸੰਖਿਆ_arg(argvars, 2) == FAIL))
	ਵਾਪਸ;

    buf = ਪਦ_get_buf(argvars, "term_setsize()");
    ਜੇ (buf == ਨਿਰਾ)
    {
	emsg(_(e_not_terminal_ਬਫਰ));
	ਵਾਪਸ;
    }
    ਜੇ (buf->b_ਪਦ->tl_vterm == ਨਿਰਾ)
	ਵਾਪਸ;
    ਪਦ = buf->b_ਪਦ;
    ਪੰਕਤੀਆਂ_2 = tv_get_ਸੰਖਿਆ(&argvars[1]);
    ਪੰਕਤੀਆਂ_2 = ਪੰਕਤੀਆਂ_2 <= 0 ? ਪਦ->tl_ਪੰਕਤੀਆਂ : ਪੰਕਤੀਆਂ_2;
    ਖਾਨੇ_ਗਿਣਤੀ = tv_get_ਸੰਖਿਆ(&argvars[2]);
    ਖਾਨੇ_ਗਿਣਤੀ = ਖਾਨੇ_ਗਿਣਤੀ <= 0 ? ਪਦ->tl_ਖਾਨੇ_ਗਿਣਤੀ : ਖਾਨੇ_ਗਿਣਤੀ;
    vterm_ਸੈੱਟ_ਮਾਪ(ਪਦ->tl_vterm, ਪੰਕਤੀਆਂ_2, ਖਾਨੇ_ਗਿਣਤੀ);
    // handle_resize() will resize the windows

    // Get and remember the size we ended up with.  Update the pty.
    vterm_get_ਮਾਪ(ਪਦ->tl_vterm, &ਪਦ->tl_ਪੰਕਤੀਆਂ, &ਪਦ->tl_ਖਾਨੇ_ਗਿਣਤੀ);
    ਪਦ_report_winsize(ਪਦ, ਪਦ->tl_ਪੰਕਤੀਆਂ, ਪਦ->tl_ਖਾਨੇ_ਗਿਣਤੀ);
}

/*
 * "term_getstatus(buf)" function
 */
    ਖਾਲੀ
f_ਪਦ_getstatus(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    ਪਦ_T	*ਪਦ;
    char_u	val[100];

    rettv->v_type = VAR_STRING;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_ਬਫਰ_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    buf = ਪਦ_get_buf(argvars, "term_getstatus()");
    ਜੇ (buf == ਨਿਰਾ)
	ਵਾਪਸ;
    ਪਦ = buf->b_ਪਦ;

    ਜੇ (ਪਦ_ਕੰਮ_running(ਪਦ))
	STRCPY(val, "running");
    ਨਹੀਂਤਾਂ
	STRCPY(val, "finished");
    ਜੇ (ਪਦ->tl_ਸਾਧਾਰਨ_ਬਹੁਲਕ)
	STRCAT(val, ",normal");
    rettv->vval.v_string = vim_strsave(val);
}

/*
 * "term_gettitle(buf)" function
 */
    ਖਾਲੀ
f_ਪਦ_gettitle(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    rettv->v_type = VAR_STRING;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_ਬਫਰ_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    buf = ਪਦ_get_buf(argvars, "term_gettitle()");
    ਜੇ (buf == ਨਿਰਾ)
	ਵਾਪਸ;

    ਜੇ (buf->b_ਪਦ->tl_title != ਨਿਰਾ)
	rettv->vval.v_string = vim_strsave(buf->b_ਪਦ->tl_title);
}

/*
 * "term_gettty(buf)" function
 */
    ਖਾਲੀ
f_ਪਦ_gettty(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    char_u	*p = ਨਿਰਾ;
    ਪੂਰਨ		ਅੰਕ = 0;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_ਬਫਰ_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    rettv->v_type = VAR_STRING;
    buf = ਪਦ_get_buf(argvars, "term_gettty()");
    ਜੇ (buf == ਨਿਰਾ)
	ਵਾਪਸ;
    ਜੇ (argvars[1].v_type != VAR_UNKNOWN)
	ਅੰਕ = tv_get_bool(&argvars[1]);

    ਚੋਣ (ਅੰਕ)
    {
	ਹਾਲਤ 0:
	    ਜੇ (buf->b_ਪਦ->tl_ਕੰਮ != ਨਿਰਾ)
		p = buf->b_ਪਦ->tl_ਕੰਮ->jv_tty_ਬਾਹਰਲਾ;
	    ਰੋਕੋ;
	ਹਾਲਤ 1:
	    ਜੇ (buf->b_ਪਦ->tl_ਕੰਮ != ਨਿਰਾ)
		p = buf->b_ਪਦ->tl_ਕੰਮ->jv_tty_ਵਿੱਚ;
	    ਰੋਕੋ;
	ਮੂਲ:
	    semsg(_(e_invalid_ਦਲੀਲ_str), tv_get_string(&argvars[1]));
	    ਵਾਪਸ;
    }
    ਜੇ (p != ਨਿਰਾ)
	rettv->vval.v_string = vim_strsave(p);
}

/*
 * "term_list()" function
 */
    ਖਾਲੀ
f_ਪਦ_list(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
    ਪਦ_T	*tp;
    list_T	*l;

    ਜੇ (rettv_list_alloc(rettv) == FAIL || ਪਹਿਲਾ_ਪਦ == ਨਿਰਾ)
	ਵਾਪਸ;

    l = rettv->vval.v_list;
    FOR_ALL_TERMS(tp)
	ਜੇ (tp->tl_ਬਫਰ != ਨਿਰਾ)
	    ਜੇ (list_append_ਸੰਖਿਆ(l,
				   (varnumber_T)tp->tl_ਬਫਰ->b_fnum) == FAIL)
		ਵਾਪਸ;
}

/*
 * "term_scrape(buf, row)" function
 */
    ਖਾਲੀ
f_ਪਦ_scrape(typval_T *argvars, typval_T *rettv)
{
    buf_T	    *buf;
    VTermScreen	    *screen = ਨਿਰਾ;
    VTermPos	    pos;
    list_T	    *l;
    ਪਦ_T	    *ਪਦ;
    char_u	    *p;
    sb_ਸਤਰ_T	    *ਸਤਰ;

    ਜੇ (rettv_list_alloc(rettv) == FAIL)
	ਵਾਪਸ;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_ਬਫਰ_arg(argvars, 0) == FAIL
		|| check_for_lnum_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    buf = ਪਦ_get_buf(argvars, "term_scrape()");
    ਜੇ (buf == ਨਿਰਾ)
	ਵਾਪਸ;
    ਪਦ = buf->b_ਪਦ;

    l = rettv->vval.v_list;
    pos.ਪੰਕਤੀ = get_ਪੰਕਤੀ_ਸੰਖਿਆ(&argvars[1], ਪਦ);

    ਜੇ (ਪਦ->tl_vterm != ਨਿਰਾ)
    {
	screen = vterm_obtain_screen(ਪਦ->tl_vterm);
	ਜੇ (screen == ਨਿਰਾ)  // can't really happen
	    ਵਾਪਸ;
	p = ਨਿਰਾ;
	ਸਤਰ = ਨਿਰਾ;
    }
    ਨਹੀਂਤਾਂ
    {
	ਪੂਰਨ	  sb_ਪੰਕਤੀ = ਪਦ->tl_scrollback_scrolled + pos.ਪੰਕਤੀ;
	linenr_T  lnum = 0;
	size_t	  ਵਿੱਥ = 0;

	scrollbackline_pos_ਵਿੱਚ_buf(ਪਦ, sb_ਪੰਕਤੀ, &lnum, ਨਿਰਾ, &ਵਿੱਥ);

	ਜੇ (sb_ਪੰਕਤੀ >= ਪਦ->tl_scrollback.ga_len || lnum <= 0 || lnum > buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	    ਵਾਪਸ;

	ਸਤਰ = (sb_ਸਤਰ_T *)ਪਦ->tl_scrollback.ga_ਅੰਕੜੇ + sb_ਪੰਕਤੀ;
	p = ml_get_buf(buf, lnum, FALSE);

	ਜੇ (STRLEN(p) < ਵਿੱਥ + ਸਤਰ->sb_ਬਾਈਟਾਂ)
	    ਵਾਪਸ;

	p += ਵਿੱਥ;
    }

    ਲਈ (pos.ਖਾਨਾ = 0; pos.ਖਾਨਾ < ਪਦ->tl_ਖਾਨੇ_ਗਿਣਤੀ; )
    {
	dict_T		*dcell;
	ਪੂਰਨ		ਚੌੜਾਈ;
	VTermScreenCellAttrs attrs;
	VTermColor	fg, bg;
	char_u		rgb[8];
	size_t		rgblen;
	char_u		mbs[MB_MAXBYTES * VTERM_MAX_CHARS_ਪ੍ਰਤੀ_CELL + 1];
	size_t		mbslen;
	ਪੂਰਨ		i;

	ਜੇ (screen == ਨਿਰਾ)
	{
	    cellattr_T	*cellattr;

	    // vterm has finished, get the cell from scrollback
	    ਜੇ (pos.ਖਾਨਾ >= ਸਤਰ->sb_ਖਾਨੇ_ਗਿਣਤੀ)
		ਰੋਕੋ;
	    cellattr = ਸਤਰ->sb_cells + pos.ਖਾਨਾ;
	    ਚੌੜਾਈ = cellattr->ਚੌੜਾਈ;
	    attrs = cellattr->attrs;
	    fg = cellattr->fg;
	    bg = cellattr->bg;
	    mbslen = mb_ptr2len(p);
	    mch_memmove(mbs, p, mbslen);
	    mbs[mbslen] = NUL;
	    p += mbslen;
	}
	ਨਹੀਂਤਾਂ
	{
	    VTermScreenCell cell;

	    ਜੇ (vterm_screen_get_cell(screen, pos, &cell) == 0)
		ਰੋਕੋ;

	    mbslen = 0;
	    ਲਈ (i = 0; i < VTERM_MAX_CHARS_ਪ੍ਰਤੀ_CELL; ++i)
	    {
		ਜੇ (cell.chars[i] == 0)
		    ਰੋਕੋ;
		mbslen += (*utf_char2bytes)((ਪੂਰਨ)cell.chars[i], mbs + mbslen);
	    }
	    mbs[mbslen] = NUL;
	    ਚੌੜਾਈ = cell.ਚੌੜਾਈ;
	    attrs = cell.attrs;
	    fg = cell.fg;
	    bg = cell.bg;
	}
	dcell = dict_alloc();
	ਜੇ (dcell == ਨਿਰਾ)
	    ਰੋਕੋ;
	ਜੇ (list_append_dict(l, dcell) == FAIL)
	{
	    dict_unref(dcell);
	    ਰੋਕੋ;
	}

	dict_ਜੋੜੋ_string_len(dcell, "chars", mbs, (ਪੂਰਨ)mbslen);

	rgblen = vim_snprintf_safelen((ਅੱਖਰ *)rgb, ਆਕਾਰ(rgb),
	    "#%02x%02x%02x", fg.red, fg.green, fg.blue);
	dict_ਜੋੜੋ_string_len(dcell, "fg", rgb, (ਪੂਰਨ)rgblen);
	rgblen = vim_snprintf_safelen((ਅੱਖਰ *)rgb, ਆਕਾਰ(rgb),
	    "#%02x%02x%02x", bg.red, bg.green, bg.blue);
	dict_ਜੋੜੋ_string_len(dcell, "bg", rgb, (ਪੂਰਨ)rgblen);

	dict_ਜੋੜੋ_ਸੰਖਿਆ(dcell, "attr",
				      cell2attr(ਪਦ, ਨਿਰਾ, &attrs, &fg, &bg));
	dict_ਜੋੜੋ_ਸੰਖਿਆ(dcell, "width", ਚੌੜਾਈ);

	++pos.ਖਾਨਾ;
	ਜੇ (ਚੌੜਾਈ == 2)
	    ++pos.ਖਾਨਾ;
    }
}

/*
 * "term_sendkeys(buf, keys)" function
 */
    ਖਾਲੀ
f_ਪਦ_sendkeys(typval_T *argvars, typval_T *rettv ਅਣਵਰਤਿਆ)
{
    buf_T	*buf;
    char_u	*ਸੁਨੇਹਾ;
    ਪਦ_T	*ਪਦ;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_ਬਫਰ_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    buf = ਪਦ_get_buf(argvars, "term_sendkeys()");
    ਜੇ (buf == ਨਿਰਾ)
	ਵਾਪਸ;

    ਸੁਨੇਹਾ = tv_get_string_chk(&argvars[1]);
    ਜੇ (ਸੁਨੇਹਾ == ਨਿਰਾ)
	ਵਾਪਸ;
    ਪਦ = buf->b_ਪਦ;
    ਜੇ (ਪਦ->tl_vterm == ਨਿਰਾ)
	ਵਾਪਸ;

    ਜਦੋਂ (*ਸੁਨੇਹਾ != NUL)
    {
	ਪੂਰਨ c;

	ਜੇ (*ਸੁਨੇਹਾ == K_SPECIAL && ਸੁਨੇਹਾ[1] != NUL && ਸੁਨੇਹਾ[2] != NUL)
	{
	    c = ਮੰਜ਼ਿਲ_SPECIAL(ਸੁਨੇਹਾ[1], ਸੁਨੇਹਾ[2]);
	    ਸੁਨੇਹਾ += 3;
	}
	ਨਹੀਂਤਾਂ
	{
	    c = PTR2CHAR(ਸੁਨੇਹਾ);
	    ਸੁਨੇਹਾ += MB_CPTR2LEN(ਸੁਨੇਹਾ);
	}
	send_ਕੁੰਜੀਆਂ_ਮੰਜ਼ਿਲ_ਪਦ(ਪਦ, c, 0, FALSE);
    }
}

#ਜੇ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * "term_getansicolors(buf)" function
 */
    ਖਾਲੀ
f_ਪਦ_getansicolors(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    ਪਦ_T	*ਪਦ;
    VTermState	*state;
    VTermColor  color;
    char_u	hexbuf[10];
    ਪੂਰਨ		ਸੂਚਕਾਂਕ;
    list_T	*list;

    ਜੇ (rettv_list_alloc(rettv) == FAIL)
	ਵਾਪਸ;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_ਬਫਰ_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    buf = ਪਦ_get_buf(argvars, "term_getansicolors()");
    ਜੇ (buf == ਨਿਰਾ)
	ਵਾਪਸ;
    ਪਦ = buf->b_ਪਦ;
    ਜੇ (ਪਦ->tl_vterm == ਨਿਰਾ)
	ਵਾਪਸ;

    list = rettv->vval.v_list;
    state = vterm_obtain_state(ਪਦ->tl_vterm);
    ਲਈ (ਸੂਚਕਾਂਕ = 0; ਸੂਚਕਾਂਕ < 16; ਸੂਚਕਾਂਕ++)
    {
	size_t	hexbuflen;

	vterm_state_get_palette_color(state, ਸੂਚਕਾਂਕ, &color);
	hexbuflen = vim_snprintf_safelen((ਅੱਖਰ *)hexbuf, ਆਕਾਰ(hexbuf),
	    "#%02x%02x%02x", color.red, color.green, color.blue);
	ਜੇ (list_append_string(list, hexbuf, (ਪੂਰਨ)hexbuflen) == FAIL)
	    ਵਾਪਸ;
    }
}

/*
 * "term_setansicolors(buf, list)" function
 */
    ਖਾਲੀ
f_ਪਦ_setansicolors(typval_T *argvars, typval_T *rettv ਅਣਵਰਤਿਆ)
{
    buf_T	*buf;
    ਪਦ_T	*ਪਦ;
    listitem_T	*li;
    ਪੂਰਨ		n = 0;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_ਬਫਰ_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    buf = ਪਦ_get_buf(argvars, "term_setansicolors()");
    ਜੇ (buf == ਨਿਰਾ)
	ਵਾਪਸ;
    ਪਦ = buf->b_ਪਦ;
    ਜੇ (ਪਦ->tl_vterm == ਨਿਰਾ)
	ਵਾਪਸ;

    ਜੇ (check_for_nonnull_list_arg(argvars, 1) == FAIL)
	ਵਾਪਸ;

    ਜੇ (argvars[1].vval.v_list->lv_ਪਹਿਲਾ == &range_list_ਵਸਤੂ
	    || argvars[1].vval.v_list->lv_len != 16)
    {
	semsg(_(e_invalid_ਮੁੱਲ_for_ਦਲੀਲ_str), "\"colors\"");
	ਵਾਪਸ;
    }

    ਜੇ (ਪਦ->tl_palette == ਨਿਰਾ)
	ਪਦ->tl_palette = ALLOC_MULT(long_u, 16);
    ਜੇ (ਪਦ->tl_palette == ਨਿਰਾ)
	ਵਾਪਸ;

    FOR_ALL_LIST_ਚੀਜ਼ਾਂ(argvars[1].vval.v_list, li)
    {
	char_u		*color_name;
	guicolor_T	guicolor;

	color_name = tv_get_string_chk(&li->li_tv);
	ਜੇ (color_name == ਨਿਰਾ)
	    ਵਾਪਸ;

	guicolor = GUI_GET_COLOR(color_name);
	ਜੇ (guicolor == INVALCOLOR)
	{
	    semsg(_(e_cannot_allocate_color_str), color_name);
	    ਵਾਪਸ;
	}

	ਪਦ->tl_palette[n++] = GUI_MCH_GET_RGB(guicolor);
    }

    ਪਦ_update_palette(ਪਦ);
}
#ਜੇ_ਅੰਤ

/*
 * "term_setapi(buf, api)" function
 */
    ਖਾਲੀ
f_ਪਦ_setapi(typval_T *argvars, typval_T *rettv ਅਣਵਰਤਿਆ)
{
    buf_T	*buf;
    ਪਦ_T	*ਪਦ;
    char_u	*api;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_ਬਫਰ_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    buf = ਪਦ_get_buf(argvars, "term_setapi()");
    ਜੇ (buf == ਨਿਰਾ)
	ਵਾਪਸ;
    ਪਦ = buf->b_ਪਦ;
    vim_free(ਪਦ->tl_api);
    api = tv_get_string_chk(&argvars[1]);
    ਜੇ (api != ਨਿਰਾ)
	ਪਦ->tl_api = vim_strsave(api);
    ਨਹੀਂਤਾਂ
	ਪਦ->tl_api = ਨਿਰਾ;
}

/*
 * "term_setrestore(buf, command)" function
 */
    ਖਾਲੀ
f_ਪਦ_setrestore(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv ਅਣਵਰਤਿਆ)
{
#ਜੇ defined(FEAT_SESSION)
    buf_T	*buf;
    ਪਦ_T	*ਪਦ;
    char_u	*cmd;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_ਬਫਰ_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    buf = ਪਦ_get_buf(argvars, "term_setrestore()");
    ਜੇ (buf == ਨਿਰਾ)
	ਵਾਪਸ;
    ਪਦ = buf->b_ਪਦ;
    vim_free(ਪਦ->tl_command);
    cmd = tv_get_string_chk(&argvars[1]);
    ਜੇ (cmd != ਨਿਰਾ)
	ਪਦ->tl_command = vim_strsave(cmd);
    ਨਹੀਂਤਾਂ
	ਪਦ->tl_command = ਨਿਰਾ;
#ਜੇ_ਅੰਤ
}

/*
 * "term_setkill(buf, how)" function
 */
    ਖਾਲੀ
f_ਪਦ_setkill(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv ਅਣਵਰਤਿਆ)
{
    buf_T	*buf;
    ਪਦ_T	*ਪਦ;
    char_u	*how;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_ਬਫਰ_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    buf = ਪਦ_get_buf(argvars, "term_setkill()");
    ਜੇ (buf == ਨਿਰਾ)
	ਵਾਪਸ;
    ਪਦ = buf->b_ਪਦ;
    vim_free(ਪਦ->tl_kill);
    how = tv_get_string_chk(&argvars[1]);
    ਜੇ (how != ਨਿਰਾ)
	ਪਦ->tl_kill = vim_strsave(how);
    ਨਹੀਂਤਾਂ
	ਪਦ->tl_kill = ਨਿਰਾ;
}

/*
 * "term_start(command, options)" function
 */
    ਖਾਲੀ
f_ਪਦ_ਸ਼ੁਰੂ(typval_T *argvars, typval_T *rettv)
{
    jobopt_T	opt;
    buf_T	*buf;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    init_ਕੰਮ_options(&opt);
    ਜੇ (argvars[1].v_type != VAR_UNKNOWN
	    && get_ਕੰਮ_options(&argvars[1], &opt,
		JO_TIMEOUT_ALL + JO_STOPONEXIT
		    + JO_CALLBACK + JO_ਬਾਹਰਲਾ_CALLBACK + JO_ERR_CALLBACK
		    + JO_EXIT_CB + JO_CLOSE_CALLBACK + JO_ਬਾਹਰਲਾ_IO,
		JO2_ਪਦ_NAME + JO2_ਪਦ_ਅੰਤ + JO2_HIDDEN + JO2_ਪਦ_OPENCMD
		    + JO2_ਪਦ_ਖਾਨੇ_ਗਿਣਤੀ + JO2_ਪਦ_ਪੰਕਤੀਆਂ + JO2_VERTICAL + JO2_CURWIN
		    + JO2_CWD + JO2_ENV + JO2_EOF_CHARS
		    + JO2_NORESTORE + JO2_ਪਦ_KILL + JO2_ਪਦ_HIGHLIGHT
		    + JO2_ANSI_COLORS + JO2_TTY_TYPE + JO2_ਪਦ_API) == FAIL)
	ਵਾਪਸ;

    buf = ਪਦ_ਸ਼ੁਰੂ(&argvars[0], ਨਿਰਾ, &opt, 0);

    ਜੇ (buf != ਨਿਰਾ && buf->b_ਪਦ != ਨਿਰਾ)
	rettv->vval.v_ਸੰਖਿਆ = buf->b_fnum;
}

/*
 * "term_wait" function
 */
    ਖਾਲੀ
f_ਪਦ_wait(typval_T *argvars, typval_T *rettv ਅਣਵਰਤਿਆ)
{
    buf_T	*buf;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_ਬਫਰ_arg(argvars, 0) == FAIL
		|| check_for_opt_ਸੰਖਿਆ_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    buf = ਪਦ_get_buf(argvars, "term_wait()");
    ਜੇ (buf == ਨਿਰਾ)
	ਵਾਪਸ;
    ਜੇ (buf->b_ਪਦ->tl_ਕੰਮ == ਨਿਰਾ)
    {
	ch_log(ਨਿਰਾ, "term_wait(): no job to wait for");
	ਵਾਪਸ;
    }
    ਜੇ (buf->b_ਪਦ->tl_ਕੰਮ->jv_channel == ਨਿਰਾ)
	// channel is closed, nothing to do
	ਵਾਪਸ;

    // Get the job status, this will detect a job that finished.
    ਜੇ (!buf->b_ਪਦ->tl_ਕੰਮ->jv_channel->ch_keep_open
	    && STRCMP(ਕੰਮ_ਸਥਿਤੀ(buf->b_ਪਦ->tl_ਕੰਮ), "dead") == 0)
    {
	// The job is dead, keep reading channel I/O until the channel is
	// closed. buf->b_term may become NULL if the terminal was closed while
	// waiting.
	ch_log(ਨਿਰਾ, "term_wait(): waiting for channel to close");
	ਜਦੋਂ (buf->b_ਪਦ != ਨਿਰਾ && !buf->b_ਪਦ->tl_channel_closed)
	{
	    ਪਦ_flush_messages();

	    ui_delay(10L, FALSE);
	    ਜੇ (!buf_valid(buf))
		// If the terminal is closed when the channel is closed the
		// buffer disappears.
		ਰੋਕੋ;
	    ਜੇ (buf->b_ਪਦ == ਨਿਰਾ || buf->b_ਪਦ->tl_channel_closing)
		// came here from a close callback, only wait one time
		ਰੋਕੋ;
	}

	ਪਦ_flush_messages();
    }
    ਨਹੀਂਤਾਂ
    {
	ਲੰਮਾ wait = 10L;

	ਪਦ_flush_messages();

	// Wait for some time for any channel I/O.
	ਜੇ (argvars[1].v_type != VAR_UNKNOWN)
	    wait = tv_get_ਸੰਖਿਆ(&argvars[1]);
	ui_delay(wait, TRUE);

	// Flushing messages on channels is hopefully sufficient.
	// TODO: is there a better way?
	ਪਦ_flush_messages();
    }
}

/*
 * Called when a channel has sent all the lines to a terminal.
 * Send a CTRL-D to mark the end of the text.
 */
    ਖਾਲੀ
ਪਦ_send_eof(channel_T *ch)
{
    ਪਦ_T	*ਪਦ;

    FOR_ALL_TERMS(ਪਦ)
	ਜੇ (ਪਦ->tl_ਕੰਮ == ch->ch_ਕੰਮ)
	{
	    ਜੇ (ਪਦ->tl_eof_chars != ਨਿਰਾ)
	    {
		channel_send(ch, PART_ਵਿੱਚ, ਪਦ->tl_eof_chars,
					(ਪੂਰਨ)STRLEN(ਪਦ->tl_eof_chars), ਨਿਰਾ);
		channel_send(ch, PART_ਵਿੱਚ, (char_u *)"\r", 1, ਨਿਰਾ);
	    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
	    ਨਹੀਂਤਾਂ
		// Default: CTRL-D
		channel_send(ch, PART_ਵਿੱਚ, (char_u *)"\004\r", 2, ਨਿਰਾ);
#ਜੇ_ਅੰਤ
	}
}

#ਜੇ defined(FEAT_GUI)
    ਕੰਮ_T *
ਪਦ_getjob(ਪਦ_T *ਪਦ)
{
    ਵਾਪਸ ਪਦ != ਨਿਰਾ ? ਪਦ->tl_ਕੰਮ : ਨਿਰਾ;
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(MSWIN)

///////////////////////////////////////
// 2. MS-Windows implementation.

HRESULT (WINAPI *pCreatePseudoConsole)(COORD, HANDLE, HANDLE, DWORD, HPCON*);
HRESULT (WINAPI *pResizePseudoConsole)(HPCON, COORD);
HRESULT (WINAPI *pClosePseudoConsole)(HPCON);
BOOL (WINAPI *pInitializeProcThreadAttributeList)(LPPROC_THREAD_ATTRIBUTE_LIST, DWORD, DWORD, PSIZE_T);
BOOL (WINAPI *pUpdateProcThreadAttribute)(LPPROC_THREAD_ATTRIBUTE_LIST, DWORD, DWORD_PTR, PVOID, ਮਾਪ_T, PVOID, PSIZE_T);
ਖਾਲੀ (WINAPI *pDeleteProcThreadAttributeList)(LPPROC_THREAD_ATTRIBUTE_LIST);

    ਸਥਿਰ ਪੂਰਨ
dyn_conpty_init(ਪੂਰਨ verbose)
{
    ਸਥਿਰ HMODULE	hKerneldll = ਨਿਰਾ;
    ਪੂਰਨ			i;
    ਸਥਿਰ ਬਣਤਰ
    {
	ਅੱਖਰ	*name;
	FARPROC	*ptr;
    } conpty_ਇੰਦਰਾਜ[] =
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
	{ਨਿਰਾ, ਨਿਰਾ}
    };

    ਜੇ (!has_conpty_working())
    {
	ਜੇ (verbose)
	    emsg(_(e_conpty_is_not_ਉਪਲਬਧ));
	ਵਾਪਸ FAIL;
    }

    // No need to initialize twice.
    ਜੇ (hKerneldll)
	ਵਾਪਸ OK;

    hKerneldll = vim_ਲੱਦੋ_Lib("kernel32.dll");
    ਲਈ (i = 0; conpty_ਇੰਦਰਾਜ[i].name != ਨਿਰਾ
					&& conpty_ਇੰਦਰਾਜ[i].ptr != ਨਿਰਾ; ++i)
    {
	ਜੇ ((*conpty_ਇੰਦਰਾਜ[i].ptr = (FARPROC)Get_Proc_ਪਤਾ(hKerneldll,
						conpty_ਇੰਦਰਾਜ[i].name)) == ਨਿਰਾ)
	{
	    ਜੇ (verbose)
		semsg(_(e_could_not_ਲੱਦੋ_library_function_str), conpty_ਇੰਦਰਾਜ[i].name);
	    hKerneldll = ਨਿਰਾ;
	    ਵਾਪਸ FAIL;
	}
    }

    ਵਾਪਸ OK;
}

    ਸਥਿਰ ਪੂਰਨ
conpty_ਪਦ_and_ਕੰਮ_init(
	ਪਦ_T	    *ਪਦ,
	typval_T    *argvar,
	ਅੱਖਰ	    **argv ਅਣਵਰਤਿਆ,
	jobopt_T    *opt,
	jobopt_T    *orig_opt)
{
    WCHAR	    *cmd_wchar = ਨਿਰਾ;
    WCHAR	    *cmd_wchar_copy = ਨਿਰਾ;
    WCHAR	    *cwd_wchar = ਨਿਰਾ;
    WCHAR	    *env_wchar = ਨਿਰਾ;
    channel_T	    *channel = ਨਿਰਾ;
    ਕੰਮ_T	    *ਕੰਮ = ਨਿਰਾ;
    HANDLE	    jo = ਨਿਰਾ;
    garray_T	    ga_cmd, ga_env;
    char_u	    *cmd = ਨਿਰਾ;
    HRESULT	    hr;
    COORD	    consize;
    ਮਾਪ_T	    breq;
    PROCESS_INFORMATION proc_info;
    HANDLE	    i_theirs = ਨਿਰਾ;
    HANDLE	    o_theirs = ਨਿਰਾ;
    HANDLE	    i_ours = ਨਿਰਾ;
    HANDLE	    o_ours = ਨਿਰਾ;
    ਅੱਖਰ	    *errmsg = ਨਿਰਾ;

    ga_init2(&ga_cmd, ਆਕਾਰ(ਅੱਖਰ*), 20);
    ga_init2(&ga_env, ਆਕਾਰ(ਅੱਖਰ*), 20);

    ਜੇ (argvar->v_type == VAR_STRING)
    {
	cmd = argvar->vval.v_string;
    }
    ਨਹੀਂਤਾਂ ਜੇ (argvar->v_type == VAR_LIST)
    {
	ਜੇ (win32_build_cmd(argvar->vval.v_list, &ga_cmd) == FAIL)
	    ਜਾਓ failed;
	cmd = ga_cmd.ga_ਅੰਕੜੇ;
    }
    ਜੇ (cmd == ਨਿਰਾ || *cmd == NUL)
    {
	emsg(_(e_invalid_ਦਲੀਲ));
	ਜਾਓ failed;
    }

    ਪਦ->tl_arg0_cmd = vim_strsave(cmd);

    cmd_wchar = enc_ਮੰਜ਼ਿਲ_utf16(cmd, ਨਿਰਾ);

    ਜੇ (cmd_wchar != ਨਿਰਾ)
    {
	// Request by CreateProcessW
	breq = wcslen(cmd_wchar) + 1 + 1;	// Addition of NUL by API
	cmd_wchar_copy = ALLOC_MULT(WCHAR, breq);
	wcsncpy(cmd_wchar_copy, cmd_wchar, breq - 1);
    }

    ga_clear(&ga_cmd);
    ਜੇ (cmd_wchar == ਨਿਰਾ)
	ਜਾਓ failed;
    ਜੇ (opt->jo_cwd != ਨਿਰਾ)
    {
	cwd_wchar = enc_ਮੰਜ਼ਿਲ_utf16(opt->jo_cwd, ਨਿਰਾ);
	ਜੇ (cwd_wchar == ਨਿਰਾ)
	    ਜਾਓ failed;
    }

    win32_build_env(opt->jo_env, &ga_env, TRUE);
    env_wchar = ga_env.ga_ਅੰਕੜੇ;

    ਜੇ (!CreatePipe(&i_theirs, &i_ours, ਨਿਰਾ, 0))
	ਜਾਓ failed;
    ਜੇ (!CreatePipe(&o_ours, &o_theirs, ਨਿਰਾ, 0))
	ਜਾਓ failed;

    consize.X = ਪਦ->tl_ਖਾਨੇ_ਗਿਣਤੀ;
    consize.Y = ਪਦ->tl_ਪੰਕਤੀਆਂ;
    hr = pCreatePseudoConsole(consize, i_theirs, o_theirs, 0,
							     &ਪਦ->tl_conpty);
    ਜੇ (FAILED(hr))
	ਜਾਓ failed;

    ਪਦ->tl_siex.StartupInfo.cb = ਆਕਾਰ(ਪਦ->tl_siex);

    // Explicitly invalidate std handles to prevent inheritance of
    // the debugger's stdout (e.g., in Visual Studio debugging sessions),
    // which could cause job output to go to the debugger instead of
    // the intended ConPTY, even with bInheritHandles set to FALSE in CreateProcess.
    ਪਦ->tl_siex.StartupInfo.dwFlags = STARTF_USESTDHANDLES;
    ਪਦ->tl_siex.StartupInfo.h_Std_ਨਿਵੇਸ਼ = INVALID_HANDLE_ਮੁੱਲ;
    ਪਦ->tl_siex.StartupInfo.hStdOutput = INVALID_HANDLE_ਮੁੱਲ;
    ਪਦ->tl_siex.StartupInfo.hStdError = INVALID_HANDLE_ਮੁੱਲ;

    // Set up pipe inheritance safely: Vista or later.
    pInitializeProcThreadAttributeList(ਨਿਰਾ, 1, 0, &breq);
    ਪਦ->tl_siex.lpAttributeList = alloc(breq);
    ਜੇ (!ਪਦ->tl_siex.lpAttributeList)
	ਜਾਓ failed;
    ਜੇ (!pInitializeProcThreadAttributeList(ਪਦ->tl_siex.lpAttributeList, 1,
								     0, &breq))
	ਜਾਓ failed;
    ਜੇ (!pUpdateProcThreadAttribute(
	    ਪਦ->tl_siex.lpAttributeList, 0,
	    PROC_THREAD_ATTRIBUTE_PSEUDOCONSOLE, ਪਦ->tl_conpty,
	    ਆਕਾਰ(HPCON), ਨਿਰਾ, ਨਿਰਾ))
	ਜਾਓ failed;

    channel = ਜੋੜੋ_channel();
    ਜੇ (channel == ਨਿਰਾ)
	ਜਾਓ failed;

    ਕੰਮ = ਕੰਮ_alloc();
    ਜੇ (ਕੰਮ == ਨਿਰਾ)
	ਜਾਓ failed;
    ਜੇ (argvar->v_type == VAR_STRING)
    {
	ਪੂਰਨ argc;

	build_argv_from_string(cmd, &ਕੰਮ->jv_argv, &argc);
    }
    ਨਹੀਂਤਾਂ
    {
	ਪੂਰਨ argc;

	build_argv_from_list(argvar->vval.v_list, &ਕੰਮ->jv_argv, &argc);
    }

    ਜੇ (opt->jo_ਸੈੱਟ & JO_ਵਿੱਚ_BUF)
	ਕੰਮ->jv_ਵਿੱਚ_buf = buflist_findnr(opt->jo_io_buf[PART_ਵਿੱਚ]);

    ਜੇ (!CreateProcessW(ਨਿਰਾ, cmd_wchar_copy, ਨਿਰਾ, ਨਿਰਾ, FALSE,
	    EXTENDED_STARTUPINFO_PRESENT | CREATE_UNICODE_ENVIRONMENT
	    | CREATE_SUSPENDED | CREATE_DEFAULT_ERROR_ਬਹੁਲਕ,
	    env_wchar, cwd_wchar,
	    &ਪਦ->tl_siex.StartupInfo, &proc_info))
    {
	errmsg = GetWin32Error();
	ਜਾਓ failed;
    }

    CloseHandle(i_theirs);
    CloseHandle(o_theirs);

    channel_ਸੈੱਟ_pipes(channel,
	    (sock_T)i_ours,
	    (sock_T)o_ours,
	    (sock_T)o_ours);

    // Write lines with CR instead of NL.
    channel->ch_write_ਪਾਠ_ਬਹੁਲਕ = TRUE;

    // Use to explicitly delete anonymous pipe handle.
    // In addition, it is used to prevent the pipe from being closed when input
    // from a buffer etc. is finished.
    channel->ch_anonymous_pipe = TRUE;

    jo = Create_ਕੰਮ_Object(ਨਿਰਾ, ਨਿਰਾ);
    ਜੇ (jo == ਨਿਰਾ)
	ਜਾਓ failed;

    ਜੇ (!ਸੌਂਪੋ_Process_ਮੰਜ਼ਿਲ_ਕੰਮ_Object(jo, proc_info.hProcess))
    {
	// Failed, switch the way to terminate process with TerminateProcess.
	CloseHandle(jo);
	jo = ਨਿਰਾ;
    }

    ResumeThread(proc_info.hThread);
    CloseHandle(proc_info.hThread);

    vim_free(cmd_wchar);
    vim_free(cmd_wchar_copy);
    vim_free(cwd_wchar);
    vim_free(env_wchar);

    ਜੇ (create_vterm(ਪਦ, ਪਦ->tl_ਪੰਕਤੀਆਂ, ਪਦ->tl_ਖਾਨੇ_ਗਿਣਤੀ) == FAIL)
	ਜਾਓ failed;

# ਜੇ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    ਜੇ (ਪਦ_use_palette())
    {
	ਜੇ (ਪਦ->tl_palette != ਨਿਰਾ)
	    ਸੈੱਟ_vterm_palette(ਪਦ->tl_vterm, ਪਦ->tl_palette);
	ਨਹੀਂਤਾਂ
	    init_vterm_ansi_colors(ਪਦ->tl_vterm);
    }
# endif

    channel_ਸੈੱਟ_ਕੰਮ(channel, ਕੰਮ, opt);
    ਕੰਮ_ਸੈੱਟ_options(ਕੰਮ, opt);

    ਕੰਮ->jv_channel = channel;
    ਕੰਮ->jv_proc_info = proc_info;
    ਕੰਮ->jv_ਕੰਮ_object = jo;
    ਕੰਮ->jv_ਸਥਿਤੀ = ਕੰਮ_STARTED;
    ਕੰਮ->jv_tty_type = vim_strsave((char_u *)"conpty");
    ++ਕੰਮ->jv_refcount;
    ਪਦ->tl_ਕੰਮ = ਕੰਮ;

    // Redirecting stdout and stderr doesn't work at the job level.  Instead
    // open the file here and handle it in.  opt->jo_io was changed in
    // setup_job_options(), use the original flags here.
    ਜੇ (orig_opt->jo_io[PART_ਬਾਹਰਲਾ] == JIO_FILE)
    {
	char_u *fname = opt->jo_io_name[PART_ਬਾਹਰਲਾ];

	ch_log(channel, "Opening output file %s", fname);
	ਪਦ->tl_ਬਾਹਰਲਾ_fd = mch_fopen((ਅੱਖਰ *)fname, WRITEBIN);
	ਜੇ (ਪਦ->tl_ਬਾਹਰਲਾ_fd == ਨਿਰਾ)
	    semsg(_(e_cant_open_file_str), fname);
    }

    ਵਾਪਸ OK;

failed:
    ga_clear(&ga_cmd);
    ga_clear(&ga_env);
    vim_free(cmd_wchar);
    vim_free(cmd_wchar_copy);
    vim_free(cwd_wchar);
    ਜੇ (channel != ਨਿਰਾ)
	channel_clear(channel);
    ਜੇ (ਕੰਮ != ਨਿਰਾ)
    {
	ਕੰਮ->jv_channel = ਨਿਰਾ;
	ਕੰਮ_cleanup(ਕੰਮ);
    }
    ਪਦ->tl_ਕੰਮ = ਨਿਰਾ;
    ਜੇ (jo != ਨਿਰਾ)
	CloseHandle(jo);

    ਜੇ (ਪਦ->tl_siex.lpAttributeList != ਨਿਰਾ)
    {
	pDeleteProcThreadAttributeList(ਪਦ->tl_siex.lpAttributeList);
	vim_free(ਪਦ->tl_siex.lpAttributeList);
    }
    ਪਦ->tl_siex.lpAttributeList = ਨਿਰਾ;
    ਜੇ (o_theirs != ਨਿਰਾ)
	CloseHandle(o_theirs);
    ਜੇ (o_ours != ਨਿਰਾ)
	CloseHandle(o_ours);
    ਜੇ (i_ours != ਨਿਰਾ)
	CloseHandle(i_ours);
    ਜੇ (i_theirs != ਨਿਰਾ)
	CloseHandle(i_theirs);
    ਜੇ (ਪਦ->tl_conpty != ਨਿਰਾ)
	pClosePseudoConsole(ਪਦ->tl_conpty);
    ਪਦ->tl_conpty = ਨਿਰਾ;
    // Propagate errors that occur in CreateProcess
    ਜੇ (errmsg)
	semsg("CreateProcess failed: %s", errmsg);
    ਵਾਪਸ FAIL;
}

    ਸਥਿਰ ਖਾਲੀ
conpty_ਪਦ_report_winsize(ਪਦ_T *ਪਦ, ਪੂਰਨ ਪੰਕਤੀਆਂ_2, ਪੂਰਨ ਖਾਨੇ_ਗਿਣਤੀ)
{
    COORD consize;

    consize.X = ਖਾਨੇ_ਗਿਣਤੀ;
    consize.Y = ਪੰਕਤੀਆਂ_2;
    pResizePseudoConsole(ਪਦ->tl_conpty, consize);
}

    ਸਥਿਰ ਖਾਲੀ
ਪਦ_free_conpty(ਪਦ_T *ਪਦ)
{
    ਜੇ (ਪਦ->tl_siex.lpAttributeList != ਨਿਰਾ)
    {
	pDeleteProcThreadAttributeList(ਪਦ->tl_siex.lpAttributeList);
	vim_free(ਪਦ->tl_siex.lpAttributeList);
    }
    ਪਦ->tl_siex.lpAttributeList = ਨਿਰਾ;
    ਜੇ (ਪਦ->tl_conpty != ਨਿਰਾ)
	pClosePseudoConsole(ਪਦ->tl_conpty);
    ਪਦ->tl_conpty = ਨਿਰਾ;
}

    ਪੂਰਨ
use_conpty(ਖਾਲੀ)
{
    ਵਾਪਸ has_conpty;
}

# define WINPTY_SPAWN_FLAG_AUTO_SHUTDOWN 1ul
# define WINPTY_SPAWN_FLAG_EXIT_AFTER_SHUTDOWN 2ull
# define WINPTY_MOUSE_ਬਹੁਲਕ_FORCE		2

ਖਾਲੀ* (*winpty_config_new)(UINT64, ਖਾਲੀ*);
ਖਾਲੀ* (*winpty_open)(ਖਾਲੀ*, ਖਾਲੀ*);
ਖਾਲੀ* (*winpty_spawn_config_new)(UINT64, ਖਾਲੀ*, LPCWSTR, ਖਾਲੀ*, ਖਾਲੀ*, ਖਾਲੀ*);
BOOL (*winpty_spawn)(ਖਾਲੀ*, ਖਾਲੀ*, HANDLE*, HANDLE*, DWORD*, ਖਾਲੀ*);
ਖਾਲੀ (*winpty_config_ਸੈੱਟ_mouse_ਬਹੁਲਕ)(ਖਾਲੀ*, ਪੂਰਨ);
ਖਾਲੀ (*winpty_config_ਸੈੱਟ_initial_ਮਾਪ)(ਖਾਲੀ*, ਪੂਰਨ, ਪੂਰਨ);
LPCWSTR (*winpty_conin_name)(ਖਾਲੀ*);
LPCWSTR (*winpty_conout_name)(ਖਾਲੀ*);
LPCWSTR (*winpty_conerr_name)(ਖਾਲੀ*);
ਖਾਲੀ (*winpty_free)(ਖਾਲੀ*);
ਖਾਲੀ (*winpty_config_free)(ਖਾਲੀ*);
ਖਾਲੀ (*winpty_spawn_config_free)(ਖਾਲੀ*);
ਖਾਲੀ (*winpty_error_free)(ਖਾਲੀ*);
LPCWSTR (*winpty_error_ਸੁਨੇਹਾ)(ਖਾਲੀ*);
BOOL (*winpty_ਸੈੱਟ_ਮਾਪ)(ਖਾਲੀ*, ਪੂਰਨ, ਪੂਰਨ, ਖਾਲੀ*);
HANDLE (*winpty_agent_process)(ਖਾਲੀ*);

# define WINPTY_DLL "winpty.dll"

ਸਥਿਰ HINSTANCE hWinPtyDLL = ਨਿਰਾ;

    ਸਥਿਰ ਪੂਰਨ
dyn_winpty_init(ਪੂਰਨ verbose)
{
    ਪੂਰਨ i;
    ਸਥਿਰ ਬਣਤਰ
    {
	ਅੱਖਰ	    *name;
	FARPROC	    *ptr;
    } winpty_ਇੰਦਰਾਜ[] =
    {
	{"winpty_conerr_name", (FARPROC*)&winpty_conerr_name},
	{"winpty_config_free", (FARPROC*)&winpty_config_free},
	{"winpty_config_new", (FARPROC*)&winpty_config_new},
	{"winpty_config_set_mouse_mode",
				      (FARPROC*)&winpty_config_ਸੈੱਟ_mouse_ਬਹੁਲਕ},
	{"winpty_config_set_initial_size",
				    (FARPROC*)&winpty_config_ਸੈੱਟ_initial_ਮਾਪ},
	{"winpty_conin_name", (FARPROC*)&winpty_conin_name},
	{"winpty_conout_name", (FARPROC*)&winpty_conout_name},
	{"winpty_error_free", (FARPROC*)&winpty_error_free},
	{"winpty_free", (FARPROC*)&winpty_free},
	{"winpty_open", (FARPROC*)&winpty_open},
	{"winpty_spawn", (FARPROC*)&winpty_spawn},
	{"winpty_spawn_config_free", (FARPROC*)&winpty_spawn_config_free},
	{"winpty_spawn_config_new", (FARPROC*)&winpty_spawn_config_new},
	{"winpty_error_msg", (FARPROC*)&winpty_error_ਸੁਨੇਹਾ},
	{"winpty_set_size", (FARPROC*)&winpty_ਸੈੱਟ_ਮਾਪ},
	{"winpty_agent_process", (FARPROC*)&winpty_agent_process},
	{ਨਿਰਾ, ਨਿਰਾ}
    };

    // No need to initialize twice.
    ਜੇ (hWinPtyDLL)
	ਵਾਪਸ OK;
    // Load winpty.dll, prefer using the 'winptydll' option, fall back to just
    // winpty.dll.
    ਜੇ (*p_winptydll != NUL)
	hWinPtyDLL = vim_ਲੱਦੋ_Lib((ਅੱਖਰ *)p_winptydll);
    ਜੇ (!hWinPtyDLL)
	hWinPtyDLL = vim_ਲੱਦੋ_Lib(WINPTY_DLL);
    ਜੇ (!hWinPtyDLL)
    {
	ਜੇ (verbose)
	    semsg(_(e_could_not_ਲੱਦੋ_library_str_str),
		    (*p_winptydll != NUL ? p_winptydll : (char_u *)WINPTY_DLL),
		    GetWin32Error());
	ਵਾਪਸ FAIL;
    }
    ਲਈ (i = 0; winpty_ਇੰਦਰਾਜ[i].name != ਨਿਰਾ
					 && winpty_ਇੰਦਰਾਜ[i].ptr != ਨਿਰਾ; ++i)
    {
	ਜੇ ((*winpty_ਇੰਦਰਾਜ[i].ptr = (FARPROC)Get_Proc_ਪਤਾ(hWinPtyDLL,
					      winpty_ਇੰਦਰਾਜ[i].name)) == ਨਿਰਾ)
	{
	    ਜੇ (verbose)
		semsg(_(e_could_not_ਲੱਦੋ_library_function_str), winpty_ਇੰਦਰਾਜ[i].name);
	    hWinPtyDLL = ਨਿਰਾ;
	    ਵਾਪਸ FAIL;
	}
    }

    ਵਾਪਸ OK;
}

    ਸਥਿਰ ਪੂਰਨ
winpty_ਪਦ_and_ਕੰਮ_init(
	ਪਦ_T	    *ਪਦ,
	typval_T    *argvar,
	ਅੱਖਰ	    **argv ਅਣਵਰਤਿਆ,
	jobopt_T    *opt,
	jobopt_T    *orig_opt)
{
    WCHAR	    *cmd_wchar = ਨਿਰਾ;
    WCHAR	    *cwd_wchar = ਨਿਰਾ;
    WCHAR	    *env_wchar = ਨਿਰਾ;
    channel_T	    *channel = ਨਿਰਾ;
    ਕੰਮ_T	    *ਕੰਮ = ਨਿਰਾ;
    DWORD	    error;
    HANDLE	    jo = ਨਿਰਾ;
    HANDLE	    ਬੱਚਾ_process_handle;
    HANDLE	    ਬੱਚਾ_thread_handle;
    ਖਾਲੀ	    *winpty_err = ਨਿਰਾ;
    ਖਾਲੀ	    *spawn_config = ਨਿਰਾ;
    garray_T	    ga_cmd, ga_env;
    char_u	    *cmd = ਨਿਰਾ;

    ga_init2(&ga_cmd, ਆਕਾਰ(ਅੱਖਰ*), 20);
    ga_init2(&ga_env, ਆਕਾਰ(ਅੱਖਰ*), 20);

    ਜੇ (argvar->v_type == VAR_STRING)
    {
	cmd = argvar->vval.v_string;
    }
    ਨਹੀਂਤਾਂ ਜੇ (argvar->v_type == VAR_LIST)
    {
	ਜੇ (win32_build_cmd(argvar->vval.v_list, &ga_cmd) == FAIL)
	    ਜਾਓ failed;
	cmd = ga_cmd.ga_ਅੰਕੜੇ;
    }
    ਜੇ (cmd == ਨਿਰਾ || *cmd == NUL)
    {
	emsg(_(e_invalid_ਦਲੀਲ));
	ਜਾਓ failed;
    }

    ਪਦ->tl_arg0_cmd = vim_strsave(cmd);

    cmd_wchar = enc_ਮੰਜ਼ਿਲ_utf16(cmd, ਨਿਰਾ);
    ga_clear(&ga_cmd);
    ਜੇ (cmd_wchar == ਨਿਰਾ)
	ਜਾਓ failed;
    ਜੇ (opt->jo_cwd != ਨਿਰਾ)
    {
	cwd_wchar = enc_ਮੰਜ਼ਿਲ_utf16(opt->jo_cwd, ਨਿਰਾ);
	ਜੇ (cwd_wchar == ਨਿਰਾ)
	    ਜਾਓ failed;
    }

    win32_build_env(opt->jo_env, &ga_env, TRUE);
    env_wchar = ga_env.ga_ਅੰਕੜੇ;

    ਪਦ->tl_winpty_config = winpty_config_new(0, &winpty_err);
    ਜੇ (ਪਦ->tl_winpty_config == ਨਿਰਾ)
	ਜਾਓ failed;

    winpty_config_ਸੈੱਟ_mouse_ਬਹੁਲਕ(ਪਦ->tl_winpty_config,
						    WINPTY_MOUSE_ਬਹੁਲਕ_FORCE);
    winpty_config_ਸੈੱਟ_initial_ਮਾਪ(ਪਦ->tl_winpty_config,
						 ਪਦ->tl_ਖਾਨੇ_ਗਿਣਤੀ, ਪਦ->tl_ਪੰਕਤੀਆਂ);
    ਪਦ->tl_winpty = winpty_open(ਪਦ->tl_winpty_config, &winpty_err);
    ਜੇ (ਪਦ->tl_winpty == ਨਿਰਾ)
	ਜਾਓ failed;

    spawn_config = winpty_spawn_config_new(
	    WINPTY_SPAWN_FLAG_AUTO_SHUTDOWN |
		WINPTY_SPAWN_FLAG_EXIT_AFTER_SHUTDOWN,
	    ਨਿਰਾ,
	    cmd_wchar,
	    cwd_wchar,
	    env_wchar,
	    &winpty_err);
    ਜੇ (spawn_config == ਨਿਰਾ)
	ਜਾਓ failed;

    channel = ਜੋੜੋ_channel();
    ਜੇ (channel == ਨਿਰਾ)
	ਜਾਓ failed;

    ਕੰਮ = ਕੰਮ_alloc();
    ਜੇ (ਕੰਮ == ਨਿਰਾ)
	ਜਾਓ failed;
    ਜੇ (argvar->v_type == VAR_STRING)
    {
	ਪੂਰਨ argc;

	build_argv_from_string(cmd, &ਕੰਮ->jv_argv, &argc);
    }
    ਨਹੀਂਤਾਂ
    {
	ਪੂਰਨ argc;

	build_argv_from_list(argvar->vval.v_list, &ਕੰਮ->jv_argv, &argc);
    }

    ਜੇ (opt->jo_ਸੈੱਟ & JO_ਵਿੱਚ_BUF)
	ਕੰਮ->jv_ਵਿੱਚ_buf = buflist_findnr(opt->jo_io_buf[PART_ਵਿੱਚ]);

    ਜੇ (!winpty_spawn(ਪਦ->tl_winpty, spawn_config, &ਬੱਚਾ_process_handle,
	    &ਬੱਚਾ_thread_handle, &error, &winpty_err))
	ਜਾਓ failed;

    channel_ਸੈੱਟ_pipes(channel,
	(sock_T)CreateFileW(
	    winpty_conin_name(ਪਦ->tl_winpty),
	    GENERIC_WRITE, 0, ਨਿਰਾ,
	    OPEN_EXISTING, 0, ਨਿਰਾ),
	(sock_T)CreateFileW(
	    winpty_conout_name(ਪਦ->tl_winpty),
	    GENERIC_READ, 0, ਨਿਰਾ,
	    OPEN_EXISTING, 0, ਨਿਰਾ),
	(sock_T)CreateFileW(
	    winpty_conerr_name(ਪਦ->tl_winpty),
	    GENERIC_READ, 0, ਨਿਰਾ,
	    OPEN_EXISTING, 0, ਨਿਰਾ));

    // Write lines with CR instead of NL.
    channel->ch_write_ਪਾਠ_ਬਹੁਲਕ = TRUE;

    jo = Create_ਕੰਮ_Object(ਨਿਰਾ, ਨਿਰਾ);
    ਜੇ (jo == ਨਿਰਾ)
	ਜਾਓ failed;

    ਜੇ (!ਸੌਂਪੋ_Process_ਮੰਜ਼ਿਲ_ਕੰਮ_Object(jo, ਬੱਚਾ_process_handle))
    {
	// Failed, switch the way to terminate process with TerminateProcess.
	CloseHandle(jo);
	jo = ਨਿਰਾ;
    }

    winpty_spawn_config_free(spawn_config);
    vim_free(cmd_wchar);
    vim_free(cwd_wchar);
    vim_free(env_wchar);

    ਜੇ (create_vterm(ਪਦ, ਪਦ->tl_ਪੰਕਤੀਆਂ, ਪਦ->tl_ਖਾਨੇ_ਗਿਣਤੀ) == FAIL)
	ਜਾਓ failed;

# ਜੇ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    ਜੇ (ਪਦ_use_palette())
    {
	ਜੇ (ਪਦ->tl_palette != ਨਿਰਾ)
	    ਸੈੱਟ_vterm_palette(ਪਦ->tl_vterm, ਪਦ->tl_palette);
	ਨਹੀਂਤਾਂ
	    init_vterm_ansi_colors(ਪਦ->tl_vterm);
    }
# endif

    channel_ਸੈੱਟ_ਕੰਮ(channel, ਕੰਮ, opt);
    ਕੰਮ_ਸੈੱਟ_options(ਕੰਮ, opt);

    ਕੰਮ->jv_channel = channel;
    ਕੰਮ->jv_proc_info.hProcess = ਬੱਚਾ_process_handle;
    ਕੰਮ->jv_proc_info.dwProcessId = GetProcessId(ਬੱਚਾ_process_handle);
    ਕੰਮ->jv_ਕੰਮ_object = jo;
    ਕੰਮ->jv_ਸਥਿਤੀ = ਕੰਮ_STARTED;
    ਕੰਮ->jv_tty_ਵਿੱਚ = utf16_ਮੰਜ਼ਿਲ_enc(
	    (short_u *)winpty_conin_name(ਪਦ->tl_winpty), ਨਿਰਾ);
    ਕੰਮ->jv_tty_ਬਾਹਰਲਾ = utf16_ਮੰਜ਼ਿਲ_enc(
	    (short_u *)winpty_conout_name(ਪਦ->tl_winpty), ਨਿਰਾ);
    ਕੰਮ->jv_tty_type = vim_strsave((char_u *)"winpty");
    ++ਕੰਮ->jv_refcount;
    ਪਦ->tl_ਕੰਮ = ਕੰਮ;

    // Redirecting stdout and stderr doesn't work at the job level.  Instead
    // open the file here and handle it in.  opt->jo_io was changed in
    // setup_job_options(), use the original flags here.
    ਜੇ (orig_opt->jo_io[PART_ਬਾਹਰਲਾ] == JIO_FILE)
    {
	char_u *fname = opt->jo_io_name[PART_ਬਾਹਰਲਾ];

	ch_log(channel, "Opening output file %s", fname);
	ਪਦ->tl_ਬਾਹਰਲਾ_fd = mch_fopen((ਅੱਖਰ *)fname, WRITEBIN);
	ਜੇ (ਪਦ->tl_ਬਾਹਰਲਾ_fd == ਨਿਰਾ)
	    semsg(_(e_cant_open_file_str), fname);
    }

    ਵਾਪਸ OK;

failed:
    ga_clear(&ga_cmd);
    ga_clear(&ga_env);
    vim_free(cmd_wchar);
    vim_free(cwd_wchar);
    ਜੇ (spawn_config != ਨਿਰਾ)
	winpty_spawn_config_free(spawn_config);
    ਜੇ (channel != ਨਿਰਾ)
	channel_clear(channel);
    ਜੇ (ਕੰਮ != ਨਿਰਾ)
    {
	ਕੰਮ->jv_channel = ਨਿਰਾ;
	ਕੰਮ_cleanup(ਕੰਮ);
    }
    ਪਦ->tl_ਕੰਮ = ਨਿਰਾ;
    ਜੇ (jo != ਨਿਰਾ)
	CloseHandle(jo);
    ਜੇ (ਪਦ->tl_winpty != ਨਿਰਾ)
	winpty_free(ਪਦ->tl_winpty);
    ਪਦ->tl_winpty = ਨਿਰਾ;
    ਜੇ (ਪਦ->tl_winpty_config != ਨਿਰਾ)
	winpty_config_free(ਪਦ->tl_winpty_config);
    ਪਦ->tl_winpty_config = ਨਿਰਾ;
    ਜੇ (winpty_err != ਨਿਰਾ)
    {
	ਅੱਖਰ *ਸੁਨੇਹਾ = (ਅੱਖਰ *)utf16_ਮੰਜ਼ਿਲ_enc(
				(short_u *)winpty_error_ਸੁਨੇਹਾ(winpty_err), ਨਿਰਾ);

	ਜੇ (ਸੁਨੇਹਾ != ਨਿਰਾ)
	{
	    emsg(ਸੁਨੇਹਾ);
	    vim_free(ਸੁਨੇਹਾ);
	}
	winpty_error_free(winpty_err);
    }
    ਵਾਪਸ FAIL;
}

/*
 * Create a new terminal of "rows" by "cols" cells.
 * Store a reference in "term".
 * Return OK or FAIL.
 */
    ਸਥਿਰ ਪੂਰਨ
ਪਦ_and_ਕੰਮ_init(
	ਪਦ_T	    *ਪਦ,
	typval_T    *argvar,
	ਅੱਖਰ	    **argv,
	jobopt_T    *opt,
	jobopt_T    *orig_opt)
{
    ਪੂਰਨ		    use_winpty = FALSE;
    ਪੂਰਨ		    use_conpty = FALSE;
    ਪੂਰਨ		    tty_type = *p_twt;

    has_winpty = dyn_winpty_init(FALSE) != FAIL ? TRUE : FALSE;
    has_conpty = dyn_conpty_init(FALSE) != FAIL ? TRUE : FALSE;

    ਜੇ (!has_winpty && !has_conpty)
	// If neither is available give the errors for winpty, since when
	// conpty is not available it can't be installed either.
	ਵਾਪਸ dyn_winpty_init(TRUE);

    ਜੇ (opt->jo_tty_type != NUL)
	tty_type = opt->jo_tty_type;

    ਜੇ (tty_type == NUL)
    {
	ਜੇ (has_conpty && (is_conpty_stable() || !has_winpty))
	    use_conpty = TRUE;
	ਨਹੀਂਤਾਂ ਜੇ (has_winpty)
	    use_winpty = TRUE;
	// else: error
    }
    ਨਹੀਂਤਾਂ ਜੇ (tty_type == 'w')	// winpty
    {
	ਜੇ (has_winpty)
	    use_winpty = TRUE;
    }
    ਨਹੀਂਤਾਂ ਜੇ (tty_type == 'c')	// conpty
    {
	ਜੇ (has_conpty)
	    use_conpty = TRUE;
	ਨਹੀਂਤਾਂ
	    ਵਾਪਸ dyn_conpty_init(TRUE);
    }

    ਜੇ (use_conpty)
	ਵਾਪਸ conpty_ਪਦ_and_ਕੰਮ_init(ਪਦ, argvar, argv, opt, orig_opt);

    ਜੇ (use_winpty)
	ਵਾਪਸ winpty_ਪਦ_and_ਕੰਮ_init(ਪਦ, argvar, argv, opt, orig_opt);

    // error
    ਵਾਪਸ dyn_winpty_init(TRUE);
}

    ਸਥਿਰ ਪੂਰਨ
create_pty_only(ਪਦ_T *ਪਦ, jobopt_T *options)
{
    HANDLE	    h_Pipe_ਵਿੱਚ = INVALID_HANDLE_ਮੁੱਲ;
    HANDLE	    h_Pipe_ਬਾਹਰਲਾ = INVALID_HANDLE_ਮੁੱਲ;
    ਅੱਖਰ	    ਵਿੱਚ_name[80], ਬਾਹਰਲਾ_name[80];
    channel_T	    *channel = ਨਿਰਾ;

    ਜੇ (create_vterm(ਪਦ, ਪਦ->tl_ਪੰਕਤੀਆਂ, ਪਦ->tl_ਖਾਨੇ_ਗਿਣਤੀ) == FAIL)
	ਵਾਪਸ FAIL;

    vim_snprintf(ਵਿੱਚ_name, ਆਕਾਰ(ਵਿੱਚ_name), "\\\\.\\pipe\\vim-%d-in-%d",
	    GetCurrentProcessId(),
	    curbuf->b_fnum);
    h_Pipe_ਵਿੱਚ = CreateNamedPipe(ਵਿੱਚ_name, PIPE_ACCESS_OUTBOUND,
	    PIPE_TYPE_ਸੁਨੇਹਾ | PIPE_NOWAIT,
	    PIPE_UNLIMITED_INSTANCES,
	    0, 0, NMPWAIT_NOWAIT, ਨਿਰਾ);
    ਜੇ (h_Pipe_ਵਿੱਚ == INVALID_HANDLE_ਮੁੱਲ)
	ਜਾਓ failed;

    vim_snprintf(ਬਾਹਰਲਾ_name, ਆਕਾਰ(ਬਾਹਰਲਾ_name), "\\\\.\\pipe\\vim-%d-out-%d",
	    GetCurrentProcessId(),
	    curbuf->b_fnum);
    h_Pipe_ਬਾਹਰਲਾ = CreateNamedPipe(ਬਾਹਰਲਾ_name, PIPE_ACCESS_INBOUND,
	    PIPE_TYPE_ਸੁਨੇਹਾ | PIPE_NOWAIT,
	    PIPE_UNLIMITED_INSTANCES,
	    0, 0, 0, ਨਿਰਾ);
    ਜੇ (h_Pipe_ਬਾਹਰਲਾ == INVALID_HANDLE_ਮੁੱਲ)
	ਜਾਓ failed;

    ConnectNamedPipe(h_Pipe_ਵਿੱਚ, ਨਿਰਾ);
    ConnectNamedPipe(h_Pipe_ਬਾਹਰਲਾ, ਨਿਰਾ);

    ਪਦ->tl_ਕੰਮ = ਕੰਮ_alloc();
    ਜੇ (ਪਦ->tl_ਕੰਮ == ਨਿਰਾ)
	ਜਾਓ failed;
    ++ਪਦ->tl_ਕੰਮ->jv_refcount;

    // behave like the job is already finished
    ਪਦ->tl_ਕੰਮ->jv_ਸਥਿਤੀ = ਕੰਮ_ਮੁਕੰਮਲ;

    channel = ਜੋੜੋ_channel();
    ਜੇ (channel == ਨਿਰਾ)
	ਜਾਓ failed;
    ਪਦ->tl_ਕੰਮ->jv_channel = channel;
    channel->ch_keep_open = TRUE;
    channel->ch_named_pipe = TRUE;

    channel_ਸੈੱਟ_pipes(channel,
	(sock_T)h_Pipe_ਵਿੱਚ,
	(sock_T)h_Pipe_ਬਾਹਰਲਾ,
	(sock_T)h_Pipe_ਬਾਹਰਲਾ);
    channel_ਸੈੱਟ_ਕੰਮ(channel, ਪਦ->tl_ਕੰਮ, options);
    ਪਦ->tl_ਕੰਮ->jv_tty_ਵਿੱਚ = vim_strsave((char_u*)ਵਿੱਚ_name);
    ਪਦ->tl_ਕੰਮ->jv_tty_ਬਾਹਰਲਾ = vim_strsave((char_u*)ਬਾਹਰਲਾ_name);

    ਵਾਪਸ OK;

failed:
    ਜੇ (h_Pipe_ਵਿੱਚ != ਨਿਰਾ)
	CloseHandle(h_Pipe_ਵਿੱਚ);
    ਜੇ (h_Pipe_ਬਾਹਰਲਾ != ਨਿਰਾ)
	CloseHandle(h_Pipe_ਬਾਹਰਲਾ);
    ਵਾਪਸ FAIL;
}

/*
 * Free the terminal emulator part of "term".
 */
    ਸਥਿਰ ਖਾਲੀ
ਪਦ_free_vterm(ਪਦ_T *ਪਦ)
{
    ਪਦ_free_conpty(ਪਦ);
    ਜੇ (ਪਦ->tl_winpty != ਨਿਰਾ)
	winpty_free(ਪਦ->tl_winpty);
    ਪਦ->tl_winpty = ਨਿਰਾ;
    ਜੇ (ਪਦ->tl_winpty_config != ਨਿਰਾ)
	winpty_config_free(ਪਦ->tl_winpty_config);
    ਪਦ->tl_winpty_config = ਨਿਰਾ;
    ਜੇ (ਪਦ->tl_vterm != ਨਿਰਾ)
	vterm_free(ਪਦ->tl_vterm);
    ਪਦ->tl_vterm = ਨਿਰਾ;
}

/*
 * Report the size to the terminal.
 */
    ਸਥਿਰ ਖਾਲੀ
ਪਦ_report_winsize(ਪਦ_T *ਪਦ, ਪੂਰਨ ਪੰਕਤੀਆਂ_2, ਪੂਰਨ ਖਾਨੇ_ਗਿਣਤੀ)
{
    ਜੇ (ਪਦ->tl_conpty)
	conpty_ਪਦ_report_winsize(ਪਦ, ਪੰਕਤੀਆਂ_2, ਖਾਨੇ_ਗਿਣਤੀ);
    ਜੇ (ਪਦ->tl_winpty)
	winpty_ਸੈੱਟ_ਮਾਪ(ਪਦ->tl_winpty, ਖਾਨੇ_ਗਿਣਤੀ, ਪੰਕਤੀਆਂ_2, ਨਿਰਾ);
}

    ਪੂਰਨ
terminal_enabled(ਖਾਲੀ)
{
    ਵਾਪਸ dyn_winpty_init(FALSE) == OK || dyn_conpty_init(FALSE) == OK;
}

#ਨਹੀਂਤਾਂ

///////////////////////////////////////
// 3. Unix-like implementation.

/*
 * Create a new terminal of "rows" by "cols" cells.
 * Start job for "cmd".
 * Store the pointers in "term".
 * When "argv" is not NULL then "argvar" is not used.
 * Return OK or FAIL.
 */
    ਸਥਿਰ ਪੂਰਨ
ਪਦ_and_ਕੰਮ_init(
	ਪਦ_T	    *ਪਦ,
	typval_T    *argvar,
	ਅੱਖਰ	    **argv,
	jobopt_T    *opt,
	jobopt_T    *orig_opt ਅਣਵਰਤਿਆ)
{
    ਪਦ->tl_arg0_cmd = ਨਿਰਾ;

    ਜੇ (create_vterm(ਪਦ, ਪਦ->tl_ਪੰਕਤੀਆਂ, ਪਦ->tl_ਖਾਨੇ_ਗਿਣਤੀ) == FAIL)
	ਵਾਪਸ FAIL;

# ਜੇ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    ਜੇ (ਪਦ_use_palette())
    {
	ਜੇ (ਪਦ->tl_palette != ਨਿਰਾ)
	    ਸੈੱਟ_vterm_palette(ਪਦ->tl_vterm, ਪਦ->tl_palette);
	ਨਹੀਂਤਾਂ
	    init_vterm_ansi_colors(ਪਦ->tl_vterm);
    }
# endif

    // This may change a string in "argvar".
    ਪਦ->tl_ਕੰਮ = ਕੰਮ_ਸ਼ੁਰੂ(argvar, argv, opt, &ਪਦ->tl_ਕੰਮ);
    ਜੇ (ਪਦ->tl_ਕੰਮ != ਨਿਰਾ)
	++ਪਦ->tl_ਕੰਮ->jv_refcount;

    ਵਾਪਸ ਪਦ->tl_ਕੰਮ != ਨਿਰਾ
	&& ਪਦ->tl_ਕੰਮ->jv_channel != ਨਿਰਾ
	&& ਪਦ->tl_ਕੰਮ->jv_ਸਥਿਤੀ != ਕੰਮ_FAILED ? OK : FAIL;
}

    ਸਥਿਰ ਪੂਰਨ
create_pty_only(ਪਦ_T *ਪਦ, jobopt_T *opt)
{
    ਜੇ (create_vterm(ਪਦ, ਪਦ->tl_ਪੰਕਤੀਆਂ, ਪਦ->tl_ਖਾਨੇ_ਗਿਣਤੀ) == FAIL)
	ਵਾਪਸ FAIL;

    ਪਦ->tl_ਕੰਮ = ਕੰਮ_alloc();
    ਜੇ (ਪਦ->tl_ਕੰਮ == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    ++ਪਦ->tl_ਕੰਮ->jv_refcount;

    // behave like the job is already finished
    ਪਦ->tl_ਕੰਮ->jv_ਸਥਿਤੀ = ਕੰਮ_ਮੁਕੰਮਲ;

    ਵਾਪਸ mch_create_pty_channel(ਪਦ->tl_ਕੰਮ, opt);
}

/*
 * Free the terminal emulator part of "term".
 */
    ਸਥਿਰ ਖਾਲੀ
ਪਦ_free_vterm(ਪਦ_T *ਪਦ)
{
    ਜੇ (ਪਦ->tl_vterm != ਨਿਰਾ)
	vterm_free(ਪਦ->tl_vterm);
    ਪਦ->tl_vterm = ਨਿਰਾ;
}

/*
 * Report the size to the terminal.
 */
    ਸਥਿਰ ਖਾਲੀ
ਪਦ_report_winsize(ਪਦ_T *ਪਦ, ਪੂਰਨ ਪੰਕਤੀਆਂ_2, ਪੂਰਨ ਖਾਨੇ_ਗਿਣਤੀ)
{
    // Use an ioctl() to report the new window size to the job.
    ਜੇ (ਪਦ->tl_ਕੰਮ == ਨਿਰਾ || ਪਦ->tl_ਕੰਮ->jv_channel == ਨਿਰਾ)
	ਵਾਪਸ;

    ਪੂਰਨ fd = -1;
    ਪੂਰਨ part;

    ਲਈ (part = PART_ਬਾਹਰਲਾ; part < PART_ਗਿਣਤੀ; ++part)
    {
	fd = ਪਦ->tl_ਕੰਮ->jv_channel->ch_part[part].ch_fd;
	ਜੇ (mch_isatty(fd))
	    ਰੋਕੋ;
    }
    ਜੇ (part < PART_ਗਿਣਤੀ && mch_report_winsize(fd, ਪੰਕਤੀਆਂ_2, ਖਾਨੇ_ਗਿਣਤੀ) == OK)
	mch_signal_ਕੰਮ(ਪਦ->tl_ਕੰਮ, (char_u *)"winch");
}

#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ // FEAT_TERMINAL
