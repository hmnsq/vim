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

#ಅನ್ತರ್ಭಾವ "vim.h"

#ಆದರೆ defined(FEAT_TERMINAL)

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ MIN
# define MIN(x,y) ((x) < (y) ? (x) : (y))
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ MAX
# define MAX(x,y) ((x) > (y) ? (x) : (y))
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ "libvterm/include/vterm.h"

// This is VTermScreenCell without the characters, thus much smaller.
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    VTermScreenCellAttrs	attrs;
    ಅಕ್ಷರ			ಅಗಲ;
    VTermColor			fg;
    VTermColor			bg;
} cellattr_T;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ sb_ಸಾಲು_S {
    ಪೂರ್ಣಾಂಕ		sb_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;	// can differ per line
    ಪೂರ್ಣಾಂಕ		sb_ಬೈಟುಗಳು;	// length in bytes of text
    cellattr_T	*sb_cells;	// allocated
    cellattr_T	sb_fill_attr;	// for short line
    char_u	*sb_ಪಠ್ಯ;	// for tl_scrollback_postponed
    char_u	continuation;
} sb_ಸಾಲು_T;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
# ifndef HPCON
#  define HPCON VOID*
# endif
# ifndef EXTENDED_STARTUPINFO_PRESENT
#  define EXTENDED_STARTUPINFO_PRESENT 0x00080000
# endif
# ifndef PROC_THREAD_ATTRIBUTE_PSEUDOCONSOLE
#  define PROC_THREAD_ATTRIBUTE_PSEUDOCONSOLE 0x00020016
# endif
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ _DYN_STARTUPINFOEXW
{
    STARTUPINFOW StartupInfo;
    LPPROC_THREAD_ATTRIBUTE_LIST lpAttributeList;
} DYN_STARTUPINFOEXW, *PDYN_STARTUPINFOEXW;
#ಆದರೆ_ಅಂತ್ಯ

// typedef term_T in structs.h
ರಚನೆ terminal_S {
    ಪದ_T	*tl_ಮುಂದಿನ;

    VTerm	*tl_vterm;
    ಕೆಲಸ_T	*tl_ಕೆಲಸ;
    buf_T	*tl_ಬಫರ್;
#ಆದರೆ defined(FEAT_GUI)
    ಪೂರ್ಣಾಂಕ		tl_system;	// when non-zero used for :!cmd output
    ಪೂರ್ಣಾಂಕ		tl_toprow;	// row with first line of system terminal
#ಆದರೆ_ಅಂತ್ಯ

    // Set when setting the size of a vterm, reset after redrawing.
    ಪೂರ್ಣಾಂಕ		tl_vterm_ಗಾತ್ರ_ಬದಲಾಯಿತು;

    ಪೂರ್ಣಾಂಕ		tl_ಸಾಮಾನ್ಯ_ಬಹುಲಕ; // TRUE: Terminal-Normal mode
    ಪೂರ್ಣಾಂಕ		tl_channel_closing;
    ಪೂರ್ಣಾಂಕ		tl_channel_closed;
    ಪೂರ್ಣಾಂಕ		tl_channel_recently_closed; // still need to handle tl_finish

    ಪೂರ್ಣಾಂಕ		tl_ಮುಕ್ತಾಯ;
#ವ್ಯಾಖ್ಯಾನಿಸು TL_ಮುಕ್ತಾಯ_UNSET	    NUL
#ವ್ಯಾಖ್ಯಾನಿಸು TL_ಮುಕ್ತಾಯ_CLOSE	    'c'	// ++close or :terminal without argument
#ವ್ಯಾಖ್ಯಾನಿಸು TL_ಮುಕ್ತಾಯ_NOCLOSE   'n'	// ++noclose
#ವ್ಯಾಖ್ಯಾನಿಸು TL_ಮುಕ್ತಾಯ_OPEN	    'o'	// ++open
    char_u	*tl_opencmd;
    char_u	*tl_eof_chars;
    char_u	*tl_api;	// prefix for terminal API function

    char_u	*tl_arg0_cmd;	// To format the status bar

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
    ಶೂನ್ಯ	*tl_winpty_config;
    ಶೂನ್ಯ	*tl_winpty;

    HPCON	tl_conpty;
    DYN_STARTUPINFOEXW tl_siex;	// Structure that always needs to be hold

    FILE	*tl_ಹೊರಹೋಗು_fd;
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_SESSION)
    char_u	*tl_command;
#ಆದರೆ_ಅಂತ್ಯ
    char_u	*tl_kill;

    // last known vterm size
    ಪೂರ್ಣಾಂಕ		tl_ಅಡ್ಡಸಾಲುಗಳು;
    ಪೂರ್ಣಾಂಕ		tl_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;

    char_u	*tl_title; // NULL or allocated
    char_u	*tl_ಸ್ಥಿತಿ_ಪಠ್ಯ; // NULL or allocated

    // Range of screen rows to update.  Zero based.
    ಪೂರ್ಣಾಂಕ		tl_dirty_ಅಡ್ಡಸಾಲು_ಆರಂಭ; // MAX_ROW if nothing dirty
    ಪೂರ್ಣಾಂಕ		tl_dirty_ಅಡ್ಡಸಾಲು_ಕೊನೆ;   // row below last one to update
    ಪೂರ್ಣಾಂಕ		tl_dirty_snapshot;  // text updated after making snapshot
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TIMERS
    ಪೂರ್ಣಾಂಕ		tl_timer_ಗಣ;
    proftime_T	tl_timer_due;
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		tl_postponed_scroll;	// to be scrolled up

    garray_T	tl_scrollback;
    ಪೂರ್ಣಾಂಕ		tl_scrollback_scrolled;
    garray_T	tl_scrollback_postponed;
    ಪೂರ್ಣಾಂಕ		tl_scrollback_snapshot;
    ಪೂರ್ಣಾಂಕ		tl_ಬಫರ್_scrolled;

    char_u	*tl_highlight_name; // replaces "Terminal"; allocated

    cellattr_T	tl_default_color;

    linenr_T	tl_ತುದಿ_diff_ಅಡ್ಡಸಾಲುಗಳು;   // rows of top diff file or zero
    linenr_T	tl_bot_diff_ಅಡ್ಡಸಾಲುಗಳು;   // rows of bottom diff file

    VTermPos	tl_cursor_pos;
    ಪೂರ್ಣಾಂಕ		tl_cursor_visible;
    ಪೂರ್ಣಾಂಕ		tl_cursor_blink;
    ಪೂರ್ಣಾಂಕ		tl_cursor_shape;  // 1: block, 2: underline, 3: bar
    char_u	*tl_cursor_color; // NULL or allocated

    long_u	*tl_palette; // array of 16 colors specified by term_start, can
			     // be NULL
    ಪೂರ್ಣಾಂಕ		tl_using_altscreen;
    garray_T	tl_osc_buf;	    // incomplete OSC string
};

#ವ್ಯಾಖ್ಯಾನಿಸು TMODE_ONCE 1	    // CTRL-\ CTRL-N used
#ವ್ಯಾಖ್ಯಾನಿಸು TMODE_LOOP 2	    // CTRL-W N used

/*
 * List of all active terminals.
 */
ಸ್ಥಿರ ಪದ_T *ಮೊದಲ_ಪದ = ಖಾಲಿ;

// Terminal active in terminal_loop().
ಸ್ಥಿರ ಪದ_T *ಒಳ_terminal_loop = ಖಾಲಿ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
ಸ್ಥಿರ BOOL has_winpty = FALSE;
ಸ್ಥಿರ BOOL has_conpty = FALSE;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು MAX_ಅಡ್ಡಸಾಲು 999999	    // used for tl_dirty_row_end to update all rows
#ವ್ಯಾಖ್ಯಾನಿಸು ಕೀಲಿ_BUF_LEN 200

#ವ್ಯಾಖ್ಯಾನಿಸು FOR_ALL_TERMS(ಪದ)	\
    ಗಾಗಿ ((ಪದ) = ಮೊದಲ_ಪದ; (ಪದ) != ಖಾಲಿ; (ಪದ) = (ಪದ)->tl_ಮುಂದಿನ)

/*
 * Functions with separate implementation for MS-Windows and Unix-like systems.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಪದ_and_ಕೆಲಸ_init(ಪದ_T *ಪದ, typval_T *argvar, ಅಕ್ಷರ **argv, jobopt_T *opt, jobopt_T *orig_opt);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ create_pty_only(ಪದ_T *ಪದ, jobopt_T *opt);
ಸ್ಥಿರ ಶೂನ್ಯ ಪದ_report_winsize(ಪದ_T *ಪದ, ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲುಗಳು_2, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ);
ಸ್ಥಿರ ಶೂನ್ಯ ಪದ_free_vterm(ಪದ_T *ಪದ);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
ಸ್ಥಿರ ಶೂನ್ಯ update_system_ಪದ(ಪದ_T *ಪದ);
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಶೂನ್ಯ handle_postponed_scrollback(ಪದ_T *ಪದ);

// The character that we know (or assume) that the terminal expects for the
// backspace key.
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಪದ_backspace_char = BS;

// Store the last set and the desired cursor properties, so that we only update
// them when needed.  Doing it unnecessary may result in flicker.
ಸ್ಥಿರ char_u	*ಕೊನೆಯ_ಗಣ_cursor_color = ಖಾಲಿ;
ಸ್ಥಿರ char_u	*desired_cursor_color = ಖಾಲಿ;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	ಕೊನೆಯ_ಗಣ_cursor_shape = -1;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	desired_cursor_shape = -1;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	ಕೊನೆಯ_ಗಣ_cursor_blink = -1;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	desired_cursor_blink = -1;


///////////////////////////////////////
// 1. Generic code for all systems.

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
cursor_color_equal(char_u *lhs_color, char_u *rhs_color)
{
    ಆದರೆ (lhs_color != ಖಾಲಿ && rhs_color != ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ STRCMP(lhs_color, rhs_color) == 0;
    ಹಿಂತಿರುಗಿ lhs_color == ಖಾಲಿ && rhs_color == ಖಾಲಿ;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
cursor_color_copy(char_u **ಗಮ್ಯ_color, char_u *from_color)
{
    // Avoid a free & alloc if the value is already right.
    ಆದರೆ (cursor_color_equal(*ಗಮ್ಯ_color, from_color))
	ಹಿಂತಿರುಗಿ;
    vim_free(*ಗಮ್ಯ_color);
    *ಗಮ್ಯ_color = (from_color == ಖಾಲಿ) ? ಖಾಲಿ : vim_strsave(from_color);
}

    ಸ್ಥಿರ char_u *
cursor_color_get(char_u *color)
{
    ಹಿಂತಿರುಗಿ (color == ಖಾಲಿ) ? (char_u *)"" : color;
}


/*
 * Parse 'termwinsize' and set "rows" and "cols" for the terminal size in the
 * current window.
 * Sets "rows" and/or "cols" to zero when it should follow the window size.
 * Return TRUE if the size is the minimum size: "24*80".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
parse_termwinsize(win_T *wp, ಪೂರ್ಣಾಂಕ *ಅಡ್ಡಸಾಲುಗಳು_2, ಪೂರ್ಣಾಂಕ *ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ)
{
    ಪೂರ್ಣಾಂಕ	minsize = FALSE;

    *ಅಡ್ಡಸಾಲುಗಳು_2 = 0;
    *ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = 0;

    ಆದರೆ (*wp->w_p_tws == NUL)
	ಹಿಂತಿರುಗಿ FALSE;

    char_u *p = vim_strchr(wp->w_p_tws, 'x');

    // Syntax of value was already checked when it's set.
    ಆದರೆ (p == ಖಾಲಿ)
    {
	minsize = TRUE;
	p = vim_strchr(wp->w_p_tws, '*');
    }
    *ಅಡ್ಡಸಾಲುಗಳು_2 = atoi((ಅಕ್ಷರ *)wp->w_p_tws);
    *ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = atoi((ಅಕ್ಷರ *)p + 1);
    ಆದರೆ (*ಅಡ್ಡಸಾಲುಗಳು_2 > VTERM_MAX_ಅಡ್ಡಸಾಲುಗಳು)
	*ಅಡ್ಡಸಾಲುಗಳು_2 = VTERM_MAX_ಅಡ್ಡಸಾಲುಗಳು;
    ಆದರೆ (*ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ > VTERM_MAX_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ)
	*ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = VTERM_MAX_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;
    ಹಿಂತಿರುಗಿ minsize;
}

/*
 * Determine the terminal size from 'termwinsize' and the current window.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಗಣ_ಪದ_and_win_ಗಾತ್ರ(ಪದ_T *ಪದ, jobopt_T *opt)
{
    ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲುಗಳು_2, ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;
    ಪೂರ್ಣಾಂಕ	minsize;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    ಆದರೆ (ಪದ->tl_system)
    {
	// Use the entire screen (excluding the tabpanel area) for the system
	// commands.  However, it will start at the command line and scroll up
	// as needed, using tl_toprow.
	ಪದ->tl_ಅಡ್ಡಸಾಲುಗಳು = ಅಡ್ಡಸಾಲುಗಳು;
	ಪದ->tl_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = topframe->fr_ಅಗಲ;
	ಹಿಂತಿರುಗಿ;
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಪದ->tl_ಅಡ್ಡಸಾಲುಗಳು = curwin->w_ಎತ್ತರ;
    ಪದ->tl_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = curwin->w_ಅಗಲ;

    minsize = parse_termwinsize(curwin, &ಅಡ್ಡಸಾಲುಗಳು_2, &ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ);
    ಆದರೆ (minsize)
    {
	ಆದರೆ (ಪದ->tl_ಅಡ್ಡಸಾಲುಗಳು < ಅಡ್ಡಸಾಲುಗಳು_2)
	    ಪದ->tl_ಅಡ್ಡಸಾಲುಗಳು = ಅಡ್ಡಸಾಲುಗಳು_2;
	ಆದರೆ (ಪದ->tl_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ < ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ)
	    ಪದ->tl_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;
    }
    ಆದರೆ ((opt->jo_set2 & JO2_ಪದ_ಅಡ್ಡಸಾಲುಗಳು))
	ಪದ->tl_ಅಡ್ಡಸಾಲುಗಳು = opt->jo_ಪದ_ಅಡ್ಡಸಾಲುಗಳು;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಅಡ್ಡಸಾಲುಗಳು_2 != 0)
	ಪದ->tl_ಅಡ್ಡಸಾಲುಗಳು = ಅಡ್ಡಸಾಲುಗಳು_2;
    ಆದರೆ ((opt->jo_set2 & JO2_ಪದ_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ))
	ಪದ->tl_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = opt->jo_ಪದ_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ != 0)
	ಪದ->tl_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;

    ಆದರೆ (!opt->jo_hidden)
    {
	ಆದರೆ (ಪದ->tl_ಅಡ್ಡಸಾಲುಗಳು != curwin->w_ಎತ್ತರ)
	    win_setheight_win(ಪದ->tl_ಅಡ್ಡಸಾಲುಗಳು, curwin);
	ಆದರೆ (ಪದ->tl_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ != curwin->w_ಅಗಲ)
	    win_setwidth_win(ಪದ->tl_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ, curwin);

	// Set 'winsize' now to avoid a resize at the next redraw.
	ಆದರೆ (!minsize && *curwin->w_p_tws != NUL)
	{
	    char_u buf[100];

	    vim_snprintf((ಅಕ್ಷರ *)buf, 100, "%dx%d",
						 ಪದ->tl_ಅಡ್ಡಸಾಲುಗಳು, ಪದ->tl_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ);
	    ಗಣ_option_ಮೌಲ್ಯ_give_err((char_u *)"termwinsize",
							   0L, buf, OPT_LOCAL);
	}
    }
}

/*
 * Initialize job options for a terminal job.
 * Caller may overrule some of them.
 */
    ಶೂನ್ಯ
init_ಕೆಲಸ_options(jobopt_T *opt)
{
    clear_ಕೆಲಸ_options(opt);

    opt->jo_ಬಹುಲಕ = CH_ಬಹುಲಕ_RAW;
    opt->jo_ಹೊರಹೋಗು_ಬಹುಲಕ = CH_ಬಹುಲಕ_RAW;
    opt->jo_err_ಬಹುಲಕ = CH_ಬಹುಲಕ_RAW;
    opt->jo_ಗಣ = JO_ಬಹುಲಕ | JO_ಹೊರಹೋಗು_ಬಹುಲಕ | JO_ERR_ಬಹುಲಕ;
}

/*
 * Set job options mandatory for a terminal job.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಸಿದ್ಧತೆ_ಕೆಲಸ_options(jobopt_T *opt, ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲುಗಳು_2, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ MSWIN
    // Win32: Redirecting the job output won't work, thus always connect stdout
    // here.
    ಆದರೆ (!(opt->jo_ಗಣ & JO_ಹೊರಹೋಗು_IO))
#ಆದರೆ_ಅಂತ್ಯ
    {
	// Connect stdout to the terminal.
	opt->jo_io[PART_ಹೊರಹೋಗು] = JIO_ಬಫರ್;
	opt->jo_io_buf[PART_ಹೊರಹೋಗು] = curbuf->b_fnum;
	opt->jo_modifiable[PART_ಹೊರಹೋಗು] = 0;
	opt->jo_ಗಣ |= JO_ಹೊರಹೋಗು_IO + JO_ಹೊರಹೋಗು_BUF + JO_ಹೊರಹೋಗು_MODIFIABLE;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ MSWIN
    // Win32: Redirecting the job output won't work, thus always connect stderr
    // here.
    ಆದರೆ (!(opt->jo_ಗಣ & JO_ERR_IO))
#ಆದರೆ_ಅಂತ್ಯ
    {
	// Connect stderr to the terminal.
	opt->jo_io[PART_ERR] = JIO_ಬಫರ್;
	opt->jo_io_buf[PART_ERR] = curbuf->b_fnum;
	opt->jo_modifiable[PART_ERR] = 0;
	opt->jo_ಗಣ |= JO_ERR_IO + JO_ERR_BUF + JO_ERR_MODIFIABLE;
    }

    opt->jo_pty = TRUE;
    ಆದರೆ ((opt->jo_set2 & JO2_ಪದ_ಅಡ್ಡಸಾಲುಗಳು) == 0)
	opt->jo_ಪದ_ಅಡ್ಡಸಾಲುಗಳು = ಅಡ್ಡಸಾಲುಗಳು_2;
    ಆದರೆ ((opt->jo_set2 & JO2_ಪದ_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ) == 0)
	opt->jo_ಪದ_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;
}

/*
 * Flush messages on channels.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಪದ_flush_messages(ಶೂನ್ಯ)
{
    mch_check_messages();
    parse_queued_messages();
}

/*
 * Close a terminal buffer (and its window).  Used when creating the terminal
 * fails.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಪದ_close_ಬಫರ್(buf_T *buf, buf_T *old_curbuf)
{
    free_terminal(buf);
    ಆದರೆ (old_curbuf != ಖಾಲಿ)
    {
	--curbuf->b_nwindows;
	curbuf = old_curbuf;
	curwin->w_ಬಫರ್ = curbuf;
	++curbuf->b_nwindows;
    }
    CHECK_CURBUF;

    // Wiping out the buffer will also close the window and call
    // free_terminal().
    do_ಬಫರ್(DOBUF_WIPE, DOBUF_ಮೊದಲ, FORWARD, buf->b_fnum, TRUE);
}

/*
 * Start a terminal window and return its buffer.
 * Use either "argvar" or "argv", the other must be NULL.
 * When "flags" has TERM_START_NOJOB only create the buffer, b_term and open
 * the window.
 * Returns NULL when failed.
 */
    buf_T *
ಪದ_ಆರಂಭ(
	typval_T    *argvar,
	ಅಕ್ಷರ	    **argv,
	jobopt_T    *opt,
	ಪೂರ್ಣಾಂಕ	    flags)
{
    exarg_T	ವಿಭಜಿಸು_ea;
    win_T	*old_curwin = curwin;
    ಪದ_T	*ಪದ;
    buf_T	*old_curbuf = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		res;
    buf_T	*newbuf;
    ಪೂರ್ಣಾಂಕ		vertical = opt->jo_vertical || (cmdmod.cmod_ವಿಭಜಿಸು & WSP_VERT);
    jobopt_T	orig_opt;  // only partly filled
    pos_T	save_cursor;

    ಆದರೆ (check_restricted() || check_secure())
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಆದರೆ (cmdwin_type != 0)
    {
	emsg(_(e_cannot_open_terminal_from_command_ಸಾಲು_ಕಿಟಕಿ));
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಆದರೆ ((opt->jo_ಗಣ & (JO_ಒಳ_IO + JO_ಹೊರಹೋಗು_IO + JO_ERR_IO))
					 == (JO_ಒಳ_IO + JO_ಹೊರಹೋಗು_IO + JO_ERR_IO)
	|| (!(opt->jo_ಗಣ & JO_ಹೊರಹೋಗು_IO) && (opt->jo_ಗಣ & JO_ಹೊರಹೋಗು_BUF))
	|| (!(opt->jo_ಗಣ & JO_ERR_IO) && (opt->jo_ಗಣ & JO_ERR_BUF))
	|| (argvar != ಖಾಲಿ
	    && argvar->v_type == VAR_LIST
	    && argvar->vval.v_list != ಖಾಲಿ
	    && argvar->vval.v_list->lv_ಮೊದಲ == &range_list_ಅಂಶ))
    {
	emsg(_(e_invalid_ವಾದ));
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಪದ = ALLOC_CLEAR_ONE(ಪದ_T);
    ಆದರೆ (ಪದ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಪದ->tl_dirty_ಅಡ್ಡಸಾಲು_ಕೊನೆ = MAX_ಅಡ್ಡಸಾಲು;
    ಪದ->tl_cursor_visible = TRUE;
    ಪದ->tl_cursor_shape = VTERM_PROP_CURSORSHAPE_ಖಂಡ;
    ಪದ->tl_ಮುಕ್ತಾಯ = opt->jo_ಪದ_ಮುಕ್ತಾಯ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    ಪದ->tl_system = (flags & ಪದ_ಆರಂಭ_SYSTEM);
#ಆದರೆ_ಅಂತ್ಯ
    ga_init2(&ಪದ->tl_scrollback, ಗಾತ್ರ(sb_ಸಾಲು_T), 300);
    ga_init2(&ಪದ->tl_scrollback_postponed, ಗಾತ್ರ(sb_ಸಾಲು_T), 300);
    ga_init2(&ಪದ->tl_osc_buf, ಗಾತ್ರ(ಅಕ್ಷರ), 300);

    setpcmark();
    CLEAR_ಕ್ಷೇತ್ರ(ವಿಭಜಿಸು_ea);
    ಆದರೆ (opt->jo_curwin)
    {
	// Create a new buffer in the current window.
	ಆದರೆ (!can_abandon(curbuf, flags & ಪದ_ಆರಂಭ_FORCEIT))
	{
	    no_write_ಸಂದೇಶ();
	    vim_free(ಪದ);
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
	ಆದರೆ (do_ecmd(0, ಖಾಲಿ, ಖಾಲಿ, &ವಿಭಜಿಸು_ea, ECMD_ONE,
		      (buf_hide(curwin->w_ಬಫರ್) ? ECMD_HIDE : 0)
			  + ((flags & ಪದ_ಆರಂಭ_FORCEIT) ? ECMD_FORCEIT : 0),
							       curwin) == FAIL)
	{
	    vim_free(ಪದ);
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (opt->jo_hidden || (flags & ಪದ_ಆರಂಭ_SYSTEM))
    {
	buf_T *buf;

	// Create a new buffer without a window. Make it the current buffer for
	// a moment to be able to do the initializations.
	buf = buflist_new((char_u *)"", ಖಾಲಿ, (linenr_T)0,
							 BLN_NEW | BLN_LISTED);
	ಆದರೆ (buf == ಖಾಲಿ || ml_open(buf) == FAIL)
	{
	    vim_free(ಪದ);
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
	old_curbuf = curbuf;
	--curbuf->b_nwindows;
	curbuf = buf;
	save_cursor = curwin->w_cursor;
	curwin->w_ಬಫರ್ = buf;
	++curbuf->b_nwindows;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Open a new window or tab.
	ವಿಭಜಿಸು_ea.cmdidx = CMD_new;
	ವಿಭಜಿಸು_ea.cmd = (char_u *)"new";
	ವಿಭಜಿಸು_ea.arg = (char_u *)"";
	ಆದರೆ (opt->jo_ಪದ_ಅಡ್ಡಸಾಲುಗಳು > 0 && !vertical)
	{
	    ವಿಭಜಿಸು_ea.line2 = opt->jo_ಪದ_ಅಡ್ಡಸಾಲುಗಳು;
	    ವಿಭಜಿಸು_ea.addr_ಎಣಿಕೆ = 1;
	}
	ಆದರೆ (opt->jo_ಪದ_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ > 0 && vertical)
	{
	    ವಿಭಜಿಸು_ea.line2 = opt->jo_ಪದ_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;
	    ವಿಭಜಿಸು_ea.addr_ಎಣಿಕೆ = 1;
	}

	ಪೂರ್ಣಾಂಕ cmod_ವಿಭಜಿಸು_modified = FALSE;
	ಆದರೆ (vertical)
	{
	    ಆದರೆ (!(cmdmod.cmod_ವಿಭಜಿಸು & WSP_VERT))
		cmod_ವಿಭಜಿಸು_modified = TRUE;
	    cmdmod.cmod_ವಿಭಜಿಸು |= WSP_VERT;
	}
	ex_splitview(&ವಿಭಜಿಸು_ea);
	ಆದರೆ (cmod_ವಿಭಜಿಸು_modified)
	    cmdmod.cmod_ವಿಭಜಿಸು &= ~WSP_VERT;
	ಆದರೆ (curwin == old_curwin)
	{
	    // split failed
	    vim_free(ಪದ);
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
    }
    ಪದ->tl_ಬಫರ್ = curbuf;
    curbuf->b_ಪದ = ಪದ;

    ಆದರೆ (!opt->jo_hidden)
    {
	// Only one size was taken care of with :new, do the other one.  With
	// "curwin" both need to be done.
	ಆದರೆ (opt->jo_ಪದ_ಅಡ್ಡಸಾಲುಗಳು > 0 && (opt->jo_curwin || vertical))
	    win_setheight(opt->jo_ಪದ_ಅಡ್ಡಸಾಲುಗಳು);
	ಆದರೆ (opt->jo_ಪದ_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ > 0 && (opt->jo_curwin || !vertical))
	    win_setwidth(opt->jo_ಪದ_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ);
    }

    // Link the new terminal in the list of active terminals.
    ಪದ->tl_ಮುಂದಿನ = ಮೊದಲ_ಪದ;
    ಮೊದಲ_ಪದ = ಪದ;

    apply_autocmds(EVENT_BUFFILEPRE, ಖಾಲಿ, ಖಾಲಿ, FALSE, curbuf);

    ಆದರೆ (opt->jo_ಪದ_name != ಖಾಲಿ)
    {
	vim_free(curbuf->b_ffname);
	curbuf->b_ffname = vim_strsave(opt->jo_ಪದ_name);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argv != ಖಾಲಿ)
    {
	vim_free(curbuf->b_ffname);
	curbuf->b_ffname = vim_strsave((char_u *)"!system");
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಪೂರ್ಣಾಂಕ	i;
	size_t	len;
	char_u	*cmd, *p;

	ಆದರೆ (argvar->v_type == VAR_STRING)
	{
	    cmd = argvar->vval.v_string;
	    ಆದರೆ (cmd == ಖಾಲಿ)
		cmd = (char_u *)"";
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(cmd, "NONE") == 0)
		cmd = (char_u *)"pty";
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argvar->v_type != VAR_LIST
		|| argvar->vval.v_list == ಖಾಲಿ
		|| argvar->vval.v_list->lv_len == 0
		|| (cmd = tv_get_string_chk(
			       &argvar->vval.v_list->lv_ಮೊದಲ->li_tv)) == ಖಾಲಿ)
	    cmd = (char_u*)"";

	len = STRLEN(cmd) + 10;
	p = alloc(len);

	ಗಾಗಿ (i = 0; p != ಖಾಲಿ; ++i)
	{
	    // Prepend a ! to the command name to avoid the buffer name equals
	    // the executable, otherwise ":w!" would overwrite it.
	    ಆದರೆ (i == 0)
		vim_snprintf((ಅಕ್ಷರ *)p, len, "!%s", cmd);
	    ಇಲ್ಲದಿದ್ದರೆ
		vim_snprintf((ಅಕ್ಷರ *)p, len, "!%s (%d)", cmd, i);
	    ಆದರೆ (buflist_findname(p) == ಖಾಲಿ)
	    {
		vim_free(curbuf->b_ffname);
		curbuf->b_ffname = p;
		ನಿಲ್ಲಿಸು;
	    }
	}
    }
    vim_free(curbuf->b_sfname);
    curbuf->b_sfname = vim_strsave(curbuf->b_ffname);
    curbuf->b_fname = curbuf->b_ffname;

    apply_autocmds(EVENT_BUFFILEPOST, ಖಾಲಿ, ಖಾಲಿ, FALSE, curbuf);

    ಆದರೆ (opt->jo_ಪದ_opencmd != ಖಾಲಿ)
	ಪದ->tl_opencmd = vim_strsave(opt->jo_ಪದ_opencmd);

    ಆದರೆ (opt->jo_eof_chars != ಖಾಲಿ)
	ಪದ->tl_eof_chars = vim_strsave(opt->jo_eof_chars);

    ಗಣ_string_option_ನೇರ((char_u *)"buftype", -1,
				  (char_u *)"terminal", OPT_FREE|OPT_LOCAL, 0);
    // Avoid that 'buftype' is reset when this buffer is entered.
    curbuf->b_p_initialized = true;

    // Mark the buffer as not modifiable. It can only be made modifiable after
    // the job finished.
    curbuf->b_p_ma = FALSE;

    ಗಣ_ಪದ_and_win_ಗಾತ್ರ(ಪದ, opt);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
    mch_memmove(orig_opt.jo_io, opt->jo_io, ಗಾತ್ರ(orig_opt.jo_io));
#ಆದರೆ_ಅಂತ್ಯ
    ಸಿದ್ಧತೆ_ಕೆಲಸ_options(opt, ಪದ->tl_ಅಡ್ಡಸಾಲುಗಳು, ಪದ->tl_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ);

    ಆದರೆ (flags & ಪದ_ಆರಂಭ_NOJOB)
	ಹಿಂತಿರುಗಿ curbuf;

#ಆದರೆ defined(FEAT_SESSION)
    // Remember the command for the session file.
    ಆದರೆ (opt->jo_ಪದ_norestore || argv != ಖಾಲಿ)
	ಪದ->tl_command = vim_strsave((char_u *)"NONE");
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argvar->v_type == VAR_STRING)
    {
	char_u	*cmd = argvar->vval.v_string;

	ಆದರೆ (cmd != ಖಾಲಿ && STRCMP(cmd, p_sh) != 0)
	    ಪದ->tl_command = vim_strsave(cmd);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argvar->v_type == VAR_LIST
	    && argvar->vval.v_list != ಖಾಲಿ
	    && argvar->vval.v_list->lv_len > 0)
    {
	garray_T	ga;
	listitem_T	*ಅಂಶ;

	ga_init2(&ga, 1, 100);
	FOR_ALL_LIST_ವಸ್ತುಗಳು(argvar->vval.v_list, ಅಂಶ)
	{
	    char_u *s = tv_get_string_chk(&ಅಂಶ->li_tv);
	    char_u *p;

	    ಆದರೆ (s == ಖಾಲಿ)
		ನಿಲ್ಲಿಸು;
	    p = vim_strsave_fnameescape(s, VSE_NONE);
	    ಆದರೆ (p == ಖಾಲಿ)
		ನಿಲ್ಲಿಸು;
	    ga_concat(&ga, p);
	    vim_free(p);
	    ga_append(&ga, ' ');
	}
	ಆದರೆ (ಅಂಶ == ಖಾಲಿ)
	{
	    ga_append(&ga, NUL);
	    ಪದ->tl_command = ga.ga_ದತ್ತಾಂಶ;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ga_clear(&ga);
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (opt->jo_ಪದ_kill != ಖಾಲಿ)
    {
	char_u *p = skiptowhite(opt->jo_ಪದ_kill);

	ಪದ->tl_kill = vim_strnsave(opt->jo_ಪದ_kill, p - opt->jo_ಪದ_kill);
    }

    ಆದರೆ (opt->jo_ಪದ_api != ಖಾಲಿ)
    {
	char_u *p = skiptowhite(opt->jo_ಪದ_api);

	ಪದ->tl_api = vim_strnsave(opt->jo_ಪದ_api, p - opt->jo_ಪದ_api);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	ಪದ->tl_api = vim_strsave((char_u *)"Tapi_");

    ಆದರೆ (opt->jo_set2 & JO2_ಪದ_HIGHLIGHT)
	ಪದ->tl_highlight_name = vim_strsave(opt->jo_ಪದ_highlight);

#ಆದರೆ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    // Save the user-defined palette, it is only used in GUI (or 'tgc' is on).
    ಆದರೆ (opt->jo_set2 & JO2_ANSI_COLORS)
    {
	ಪದ->tl_palette = ALLOC_MULT(long_u, 16);
	ಆದರೆ (ಪದ->tl_palette == ಖಾಲಿ)
	{
	    vim_free(ಪದ);
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
	ಸ್ಮರಣೆ_ನಕಲು(ಪದ->tl_palette, opt->jo_ansi_colors, ಗಾತ್ರ(long_u) * 16);
    }
#ಆದರೆ_ಅಂತ್ಯ

    // System dependent: setup the vterm and maybe start the job in it.
    ಆದರೆ (argv == ಖಾಲಿ
	    && argvar->v_type == VAR_STRING
	    && argvar->vval.v_string != ಖಾಲಿ
	    && STRCMP(argvar->vval.v_string, "NONE") == 0)
	res = create_pty_only(ಪದ, opt);
    ಇಲ್ಲದಿದ್ದರೆ
	res = ಪದ_and_ಕೆಲಸ_init(ಪದ, argvar, argv, opt, &orig_opt);

    newbuf = curbuf;
    ಆದರೆ (res == OK)
    {
	// Get and remember the size we ended up with.  Update the pty.
	vterm_get_ಗಾತ್ರ(ಪದ->tl_vterm, &ಪದ->tl_ಅಡ್ಡಸಾಲುಗಳು, &ಪದ->tl_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ);
	ಪದ_report_winsize(ಪದ, ಪದ->tl_ಅಡ್ಡಸಾಲುಗಳು, ಪದ->tl_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
	ಆದರೆ (ಪದ->tl_system)
	{
	    // display first line below typed command
	    ಪದ->tl_toprow = ಸಂದೇಶ_ಅಡ್ಡಸಾಲು + 1;
	    ಪದ->tl_dirty_ಅಡ್ಡಸಾಲು_ಕೊನೆ = 0;
	}
#ಆದರೆ_ಅಂತ್ಯ

	// Make sure we don't get stuck on sending keys to the job, it leads to
	// a deadlock if the job is waiting for Vim to read.
	channel_ಗಣ_nonblock(ಪದ->tl_ಕೆಲಸ->jv_channel, PART_ಒಳ);

	ಆದರೆ (old_curbuf != ಖಾಲಿ)
	{
	    --curbuf->b_nwindows;
	    curbuf = old_curbuf;
	    curwin->w_ಬಫರ್ = curbuf;
	    curwin->w_cursor = save_cursor;
	    ++curbuf->b_nwindows;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (vgetc_busy
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TIMERS
		|| timer_busy
#ಆದರೆ_ಅಂತ್ಯ
		|| ಒಳಹರಿವು_busy)
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
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಪದ_close_ಬಫರ್(curbuf, old_curbuf);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    apply_autocmds(EVENT_TERMINALOPEN, ಖಾಲಿ, ಖಾಲಿ, FALSE, newbuf);
    ಆದರೆ (!opt->jo_hidden && !(flags & ಪದ_ಆರಂಭ_SYSTEM))
	apply_autocmds(EVENT_TERMINALWINOPEN, ಖಾಲಿ, ಖಾಲಿ, FALSE, newbuf);
    ಹಿಂತಿರುಗಿ newbuf;
}

/*
 * ":terminal": open a terminal window and execute a job in it.
 */
    ಶೂನ್ಯ
ex_terminal(exarg_T *eap)
{
    typval_T	argvar[2];
    jobopt_T	opt;
    ಪೂರ್ಣಾಂಕ		opt_shell = FALSE;
    char_u	*cmd;
    char_u	*tofree = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		scroll_save = ಸಂದೇಶ_scroll;

    ಸಂದೇಶ_scroll = FALSE;		// don't scroll here
    autowrite_all();
    ಸಂದೇಶ_scroll = scroll_save;

    init_ಕೆಲಸ_options(&opt);

    cmd = eap->arg;
    ಆಗ (*cmd == '+' && *(cmd + 1) == '+')
    {
	char_u  *p, *ep;

	cmd += 2;
	p = skiptowhite(cmd);
	ep = vim_strchr(cmd, '=');
	ಆದರೆ (ep != ಖಾಲಿ)
	{
	    ಆದರೆ (ep < p)
		p = ep;
	    ಇಲ್ಲದಿದ್ದರೆ
		ep = ಖಾಲಿ;
	}

	// Note: Keep this in sync with get_terminalopt_name.

#ವ್ಯಾಖ್ಯಾನಿಸು OPTARG_HAS(name) ((ಪೂರ್ಣಾಂಕ)(p - cmd) == ಗಾತ್ರ(name) - 1 \
				 && STRNICMP(cmd, name, ಗಾತ್ರ(name) - 1) == 0)
	ಆದರೆ (OPTARG_HAS("close"))
	    opt.jo_ಪದ_ಮುಕ್ತಾಯ = 'c';
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (OPTARG_HAS("noclose"))
	    opt.jo_ಪದ_ಮುಕ್ತಾಯ = 'n';
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (OPTARG_HAS("open"))
	    opt.jo_ಪದ_ಮುಕ್ತಾಯ = 'o';
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (OPTARG_HAS("curwin"))
	    opt.jo_curwin = 1;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (OPTARG_HAS("hidden"))
	    opt.jo_hidden = 1;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (OPTARG_HAS("norestore"))
	    opt.jo_ಪದ_norestore = 1;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (OPTARG_HAS("shell"))
	    opt_shell = TRUE;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (OPTARG_HAS("kill") && ep != ಖಾಲಿ)
	{
	    opt.jo_set2 |= JO2_ಪದ_KILL;
	    opt.jo_ಪದ_kill = ep + 1;
	    p = skiptowhite(cmd);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (OPTARG_HAS("api"))
	{
	    opt.jo_set2 |= JO2_ಪದ_API;
	    ಆದರೆ (ep != ಖಾಲಿ)
	    {
		opt.jo_ಪದ_api = ep + 1;
		p = skiptowhite(cmd);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		opt.jo_ಪದ_api = ಖಾಲಿ;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (OPTARG_HAS("rows") && ep != ಖಾಲಿ && ಸುರಕ್ಷಿತ_isdigit(ep[1]))
	{
	    opt.jo_set2 |= JO2_ಪದ_ಅಡ್ಡಸಾಲುಗಳು;
	    opt.jo_ಪದ_ಅಡ್ಡಸಾಲುಗಳು = atoi((ಅಕ್ಷರ *)ep + 1);
	    p = skiptowhite(cmd);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (OPTARG_HAS("cols") && ep != ಖಾಲಿ && ಸುರಕ್ಷಿತ_isdigit(ep[1]))
	{
	    opt.jo_set2 |= JO2_ಪದ_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;
	    opt.jo_ಪದ_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = atoi((ಅಕ್ಷರ *)ep + 1);
	    p = skiptowhite(cmd);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (OPTARG_HAS("eof") && ep != ಖಾಲಿ)
	{
	    char_u *buf = ಖಾಲಿ;
	    char_u *ಕೀಲಿಗಳು;

	    vim_free(opt.jo_eof_chars);
	    p = skiptowhite(cmd);
	    *p = NUL;
	    ಕೀಲಿಗಳು = replace_termcodes(ep + 1, &buf, 0,
		    REPTERM_FROM_PART | REPTERM_DO_LT | REPTERM_SPECIAL, ಖಾಲಿ);
	    opt.jo_set2 |= JO2_EOF_CHARS;
	    opt.jo_eof_chars = vim_strsave(ಕೀಲಿಗಳು);
	    vim_free(buf);
	    *p = ' ';
	}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((ಪೂರ್ಣಾಂಕ)(p - cmd) == 4 && STRNICMP(cmd, "type", 4) == 0
								 && ep != ಖಾಲಿ)
	{
	    ಪೂರ್ಣಾಂಕ tty_type = NUL;

	    p = skiptowhite(cmd);
	    ಆದರೆ (STRNICMP(ep + 1, "winpty", p - (ep + 1)) == 0)
		tty_type = 'w';
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNICMP(ep + 1, "conpty", p - (ep + 1)) == 0)
		tty_type = 'c';
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		semsg(_(e_invalid_ಮೌಲ್ಯ_for_ವಾದ_str), "type");
		ಹೋಗು theend;
	    }
	    opt.jo_set2 |= JO2_TTY_TYPE;
	    opt.jo_tty_type = tty_type;
	}
#ಆದರೆ_ಅಂತ್ಯ
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (*p)
		*p = NUL;
	    semsg(_(e_invalid_attribute_str), cmd);
	    ಹೋಗು theend;
	}
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು OPTARG_HAS
	cmd = skipwhite(p);
    }
    ಆದರೆ (*cmd == NUL)
    {
	// Make a copy of 'shell', an autocommand may change the option.
	tofree = cmd = vim_strsave(p_sh);

	// default to close when the shell exits
	ಆದರೆ (opt.jo_ಪದ_ಮುಕ್ತಾಯ == NUL)
	    opt.jo_ಪದ_ಮುಕ್ತಾಯ = TL_ಮುಕ್ತಾಯ_CLOSE;
    }

    ಆದರೆ (eap->addr_ಎಣಿಕೆ > 0)
    {
	// Write lines from current buffer to the job.
	opt.jo_ಗಣ |= JO_ಒಳ_IO | JO_ಒಳ_BUF | JO_ಒಳ_ತುದಿ | JO_ಒಳ_BOT;
	opt.jo_io[PART_ಒಳ] = JIO_ಬಫರ್;
	opt.jo_io_buf[PART_ಒಳ] = curbuf->b_fnum;
	opt.jo_ಒಳ_ತುದಿ = eap->line1;
	opt.jo_ಒಳ_bot = eap->line2;
    }

    ಆದರೆ (opt_shell && tofree == ಖಾಲಿ)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
	ಅಕ್ಷರ	**argv = ಖಾಲಿ;
	char_u	*tofree1 = ಖಾಲಿ;
	char_u	*tofree2 = ಖಾಲಿ;

	// :term ++shell command
	ಆದರೆ (unix_build_argv(cmd, &argv, &tofree1, &tofree2) == OK)
	    ಪದ_ಆರಂಭ(ಖಾಲಿ, argv, &opt, eap->forceit ? ಪದ_ಆರಂಭ_FORCEIT : 0);
	vim_free(argv);
	vim_free(tofree1);
	vim_free(tofree2);
	ಹೋಗು theend;
#ಇಲ್ಲದಿದ್ದರೆ
# ifdef MSWIN
	long_u	    cmdlen = STRLEN(p_sh) + STRLEN(p_shcf) + STRLEN(cmd) + 10;
	char_u	    *newcmd;

	newcmd = alloc(cmdlen);
	ಆದರೆ (newcmd == ಖಾಲಿ)
	    ಹೋಗು theend;
	tofree = newcmd;
	vim_snprintf((ಅಕ್ಷರ *)newcmd, cmdlen, "%s %s %s", p_sh, p_shcf, cmd);
	cmd = newcmd;
# ಇಲ್ಲದಿದ್ದರೆ
	emsg(_(e_sorry_plusplusshell_not_supported_on_this_system));
	ಹೋಗು theend;
# endif
#ಆದರೆ_ಅಂತ್ಯ
    }
    argvar[0].v_type = VAR_STRING;
    argvar[0].vval.v_string = cmd;
    argvar[1].v_type = VAR_UNKNOWN;
    ಪದ_ಆರಂಭ(argvar, ಖಾಲಿ, &opt, eap->forceit ? ಪದ_ಆರಂಭ_FORCEIT : 0);

theend:
    vim_free(tofree);
    vim_free(opt.jo_eof_chars);
}

    ಸ್ಥಿರ char_u *
get_terminalopt_name(expand_T *xp ಬಳಸದ, ಪೂರ್ಣಾಂಕ idx)
{
    // Note: Keep this in sync with ex_terminal.
    ಸ್ಥಿರ ಅಕ್ಷರ *(p_termopt_ಮೌಲ್ಯಗಳು[]) =
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

    ಆದರೆ (idx < (ಪೂರ್ಣಾಂಕ)ARRAY_ಉದ್ದಳತೆ(p_termopt_ಮೌಲ್ಯಗಳು))
	ಹಿಂತಿರುಗಿ (char_u*)p_termopt_ಮೌಲ್ಯಗಳು[idx];
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

    ಸ್ಥಿರ char_u *
get_termkill_name(expand_T *xp ಬಳಸದ, ಪೂರ್ಣಾಂಕ idx)
{
    // These are platform-specific values used for job_stop(). They are defined
    // in each platform's mch_signal_job(). Just use a unified auto-complete
    // list for simplicity.
    ಸ್ಥಿರ ಅಕ್ಷರ *(p_termkill_ಮೌಲ್ಯಗಳು[]) =
    {
	"term",
	"hup",
	"quit",
	"int",
	"kill",
	"winch",
    };

    ಆದರೆ (idx < (ಪೂರ್ಣಾಂಕ)ARRAY_ಉದ್ದಳತೆ(p_termkill_ಮೌಲ್ಯಗಳು))
	ಹಿಂತಿರುಗಿ (char_u*)p_termkill_ಮೌಲ್ಯಗಳು[idx];
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Command-line expansion for :terminal [options]
 */
    ಪೂರ್ಣಾಂಕ
expand_terminal_opt(
	char_u	    *pat,
	expand_T    *xp,
	regmatch_T  *rmp,
	char_u	    ***matches,
	ಪೂರ್ಣಾಂಕ	    *ಅಂಕೆ_Matches)
{
    ಆದರೆ (xp->xp_ನಮೂನೆ > xp->xp_ಸಾಲು && *(xp->xp_ನಮೂನೆ-1) == '=')
    {
	char_u *(*cb)(expand_T *, ಪೂರ್ಣಾಂಕ) = ಖಾಲಿ;

	char_u *name_ಕೊನೆ = xp->xp_ನಮೂನೆ - 1;
	ಆದರೆ (name_ಕೊನೆ - xp->xp_ಸಾಲು >= 4
		&& STRNCMP(name_ಕೊನೆ - 4, "kill", 4) == 0)
	    cb = get_termkill_name;

	ಆದರೆ (cb != ಖಾಲಿ)
	{
	    ಹಿಂತಿರುಗಿ ExpandGeneric(
		    pat,
		    xp,
		    rmp,
		    matches,
		    ಅಂಕೆ_Matches,
		    cb,
		    FALSE);
	}
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ ExpandGeneric(
	    pat,
	    xp,
	    rmp,
	    matches,
	    ಅಂಕೆ_Matches,
	    get_terminalopt_name,
	    FALSE);
}

#ಆದರೆ defined(FEAT_SESSION)
/*
 * Write a :terminal command to the session file to restore the terminal in
 * window "wp".
 * Return FAIL if writing fails.
 */
    ಪೂರ್ಣಾಂಕ
ಪದ_write_session(FILE *fd, win_T *wp, hashtab_T *terminal_bufs)
{
    ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ	bufnr = wp->w_ಬಫರ್->b_fnum;
    ಪದ_T	*ಪದ = wp->w_ಬಫರ್->b_ಪದ;

    ಆದರೆ (terminal_bufs != ಖಾಲಿ && wp->w_ಬಫರ್->b_nwindows > 1)
    {
	// There are multiple views into this terminal buffer. We don't want to
	// create the terminal multiple times. If it's the first time, create,
	// otherwise link to the first buffer.
	ಅಕ್ಷರ	    id_as_str[NUMBUFLEN];
	hashitem_T  *ನಮೂದು;

	vim_snprintf(id_as_str, ಗಾತ್ರ(id_as_str), "%d", bufnr);

	ನಮೂದು = hash_ಕಂಡುಹಿಡಿ(terminal_bufs, (char_u *)id_as_str);
	ಆದರೆ (!HASHITEM_ಖಾಲಿಗಳು(ನಮೂದು))
	{
	    // we've already opened this terminal buffer
	    ಆದರೆ (fprintf(fd, "execute 'buffer ' .. term_buf_%d", bufnr) < 0)
		ಹಿಂತಿರುಗಿ FAIL;
	    ಹಿಂತಿರುಗಿ put_eol(fd);
	}
    }

    // Create the terminal and run the command.  This is not without
    // risk, but let's assume the user only creates a session when this
    // will be OK.
    ಆದರೆ (fprintf(fd, "exe ':terminal ++curwin"
		" ++cols=' .. ((&columns * %d + %ld) / %ld)"
		" .. ' ++rows=' .. ((&lines * %d + %ld) / %ld) ",
		ಪದ->tl_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ, Columns / 2, Columns,
		ಪದ->tl_ಅಡ್ಡಸಾಲುಗಳು, ಅಡ್ಡಸಾಲುಗಳು / 2, ಅಡ್ಡಸಾಲುಗಳು) < 0)
	ಹಿಂತಿರುಗಿ FAIL;
# ifdef MSWIN
    ಆದರೆ (fprintf(fd, ".. ' ++type=%s' ", ಪದ->tl_ಕೆಲಸ->jv_tty_type) < 0)
	ಹಿಂತಿರುಗಿ FAIL;
# endif
    ಆದರೆ (ಪದ->tl_command != ಖಾಲಿ)
    {
	char_u *quoted_command = string_quote(ಪದ->tl_command, FALSE);
	ಆದರೆ (quoted_command == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FAIL;

	ಪೂರ್ಣಾಂಕ ret = fputs(".. ' ' .. ", fd) < 0
		    || fputs((ಅಕ್ಷರ *)quoted_command, fd) < 0;
	vim_free(quoted_command);
	ಆದರೆ (ret)
	    ಹಿಂತಿರುಗಿ FAIL;
    }
    ಆದರೆ (put_eol(fd) != OK)
	ಹಿಂತಿರುಗಿ FAIL;

    ಆದರೆ (fprintf(fd, "var term_buf_%d: number = bufnr()", bufnr) < 0)
	ಹಿಂತಿರುಗಿ FAIL;

    ಆದರೆ (terminal_bufs != ಖಾಲಿ && wp->w_ಬಫರ್->b_nwindows > 1)
    {
	ಅಕ್ಷರ *hash_ಕೀಲಿ = alloc(NUMBUFLEN);

	vim_snprintf(hash_ಕೀಲಿ, NUMBUFLEN, "%d", bufnr);
	hash_ಕೂಡಿಸು(terminal_bufs, (char_u *)hash_ಕೀಲಿ, "terminal session");
    }

    ಹಿಂತಿರುಗಿ put_eol(fd);
}

/*
 * Return TRUE if "buf" has a terminal that should be restored.
 */
    ಪೂರ್ಣಾಂಕ
ಪದ_should_restore(buf_T *buf)
{
    ಪದ_T	*ಪದ = buf->b_ಪದ;

    ಹಿಂತಿರುಗಿ ಪದ != ಖಾಲಿ && (ಪದ->tl_command == ಖಾಲಿ
				     || STRCMP(ಪದ->tl_command, "NONE") != 0);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Free the scrollback buffer for "term".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
free_scrollback(ಪದ_T *ಪದ)
{
    ಪೂರ್ಣಾಂಕ i;

    ಗಾಗಿ (i = 0; i < ಪದ->tl_scrollback.ga_len; ++i)
	vim_free(((sb_ಸಾಲು_T *)ಪದ->tl_scrollback.ga_ದತ್ತಾಂಶ + i)->sb_cells);
    ga_clear(&ಪದ->tl_scrollback);
    ಗಾಗಿ (i = 0; i < ಪದ->tl_scrollback_postponed.ga_len; ++i)
	vim_free(((sb_ಸಾಲು_T *)ಪದ->tl_scrollback_postponed.ga_ದತ್ತಾಂಶ + i)->sb_cells);
    ga_clear(&ಪದ->tl_scrollback_postponed);
}


// Terminals that need to be freed soon.
ಸ್ಥಿರ ಪದ_T	*terminals_ಗಮ್ಯ_free = ಖಾಲಿ;

/*
 * Free a terminal and everything it refers to.
 * Kills the job if there is one.
 * Called when wiping out a buffer.
 * The actual terminal structure is freed later in free_unused_terminals(),
 * because callbacks may wipe out a buffer while the terminal is still
 * referenced.
 */
    ಶೂನ್ಯ
free_terminal(buf_T *buf)
{
    ಪದ_T	*ಪದ = buf->b_ಪದ;
    ಪದ_T	*tp;

    ಆದರೆ (ಪದ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    // Unlink the terminal form the list of terminals.
    ಆದರೆ (ಮೊದಲ_ಪದ == ಪದ)
	ಮೊದಲ_ಪದ = ಪದ->tl_ಮುಂದಿನ;
    ಇಲ್ಲದಿದ್ದರೆ
	ಗಾಗಿ (tp = ಮೊದಲ_ಪದ; tp->tl_ಮುಂದಿನ != ಖಾಲಿ; tp = tp->tl_ಮುಂದಿನ)
	    ಆದರೆ (tp->tl_ಮುಂದಿನ == ಪದ)
	    {
		tp->tl_ಮುಂದಿನ = ಪದ->tl_ಮುಂದಿನ;
		ನಿಲ್ಲಿಸು;
	    }

    ಆದರೆ (ಪದ->tl_ಕೆಲಸ != ಖಾಲಿ)
    {
	ಆದರೆ (ಪದ->tl_ಕೆಲಸ->jv_ಸ್ಥಿತಿ != ಕೆಲಸ_ENDED
		&& ಪದ->tl_ಕೆಲಸ->jv_ಸ್ಥಿತಿ != ಕೆಲಸ_ಪೂರ್ಣಗೊಂಡ
		&& ಪದ->tl_ಕೆಲಸ->jv_ಸ್ಥಿತಿ != ಕೆಲಸ_FAILED)
	    ಕೆಲಸ_stop(ಪದ->tl_ಕೆಲಸ, ಖಾಲಿ, "kill");
	ಕೆಲಸ_unref(ಪದ->tl_ಕೆಲಸ);
    }
    ಪದ->tl_ಮುಂದಿನ = terminals_ಗಮ್ಯ_free;
    terminals_ಗಮ್ಯ_free = ಪದ;

    buf->b_ಪದ = ಖಾಲಿ;
    ಆದರೆ (ಒಳ_terminal_loop == ಪದ)
	ಒಳ_terminal_loop = ಖಾಲಿ;
}

    ಶೂನ್ಯ
free_ಬಳಸದ_terminals(ಶೂನ್ಯ)
{
    ಆಗ (terminals_ಗಮ್ಯ_free != ಖಾಲಿ)
    {
	ಪದ_T	    *ಪದ = terminals_ಗಮ್ಯ_free;

	terminals_ಗಮ್ಯ_free = ಪದ->tl_ಮುಂದಿನ;

	free_scrollback(ಪದ);
	ga_clear(&ಪದ->tl_osc_buf);

	ಪದ_free_vterm(ಪದ);
	vim_free(ಪದ->tl_api);
	vim_free(ಪದ->tl_title);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SESSION
	vim_free(ಪದ->tl_command);
#ಆದರೆ_ಅಂತ್ಯ
	vim_free(ಪದ->tl_kill);
	vim_free(ಪದ->tl_ಸ್ಥಿತಿ_ಪಠ್ಯ);
	vim_free(ಪದ->tl_opencmd);
	vim_free(ಪದ->tl_eof_chars);
	vim_free(ಪದ->tl_arg0_cmd);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
	ಆದರೆ (ಪದ->tl_ಹೊರಹೋಗು_fd != ಖಾಲಿ)
	    fclose(ಪದ->tl_ಹೊರಹೋಗು_fd);
#ಆದರೆ_ಅಂತ್ಯ
	vim_free(ಪದ->tl_highlight_name);
	vim_free(ಪದ->tl_cursor_color);
	vim_free(ಪದ->tl_palette);
	vim_free(ಪದ);
    }
}

/*
 * Get the part that is connected to the tty. Normally this is PART_IN, but
 * when writing buffer lines to the job it can be another.  This makes it
 * possible to do "1,5term vim -".
 */
    ಸ್ಥಿರ ch_part_T
get_tty_part(ಪದ_T *ಪದ ಬಳಸದ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
    ch_part_T	parts[3] = {PART_ಒಳ, PART_ಹೊರಹೋಗು, PART_ERR};
    ಪೂರ್ಣಾಂಕ		i;

    ಗಾಗಿ (i = 0; i < 3; ++i)
    {
	ಪೂರ್ಣಾಂಕ fd = ಪದ->tl_ಕೆಲಸ->jv_channel->ch_part[parts[i]].ch_fd;

	ಆದರೆ (mch_isatty(fd))
	    ಹಿಂತಿರುಗಿ parts[i];
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ PART_ಒಳ;
}

/*
 * Read any vterm output and send it on the channel.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಪದ_forward_output(ಪದ_T *ಪದ)
{
    VTerm *vterm = ಪದ->tl_vterm;
    ಅಕ್ಷರ   buf[ಕೀಲಿ_BUF_LEN];
    size_t curlen = vterm_output_read(vterm, buf, ಕೀಲಿ_BUF_LEN);

    ಆದರೆ (curlen > 0)
	channel_send(ಪದ->tl_ಕೆಲಸ->jv_channel, get_tty_part(ಪದ),
					     (char_u *)buf, (ಪೂರ್ಣಾಂಕ)curlen, ಖಾಲಿ);
}

/*
 * Write job output "msg[len]" to the vterm.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಪದ_write_ಕೆಲಸ_output(ಪದ_T *ಪದ, char_u *ಸಂದೇಶ_arg, size_t len_arg)
{
    char_u	*ಸಂದೇಶ = ಸಂದೇಶ_arg;
    size_t	len = len_arg;
    VTerm	*vterm = ಪದ->tl_vterm;
    size_t	prevlen = vterm_output_get_ಬಫರ್_current(vterm);
    size_t	ಎಲ್ಲೆ = ಪದ->tl_ಬಫರ್->b_p_twsl * ಪದ->tl_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ * 3;

    // Limit the length to 'termwinscroll' * cols * 3 bytes.  Keep the text at
    // the end.
    ಆದರೆ (len > ಎಲ್ಲೆ)
    {
	char_u *p = ಸಂದೇಶ + len - ಎಲ್ಲೆ;

	p -= (*mb_ತಲೆ_off)(ಸಂದೇಶ, p);
	len -= p - ಸಂದೇಶ;
	ಸಂದೇಶ = p;
    }

    vterm_ಒಳಹರಿವು_write(vterm, (ಅಕ್ಷರ *)ಸಂದೇಶ, len);

    // flush vterm buffer when vterm responded to control sequence
    ಆದರೆ (prevlen != vterm_output_get_ಬಫರ್_current(vterm))
	ಪದ_forward_output(ಪದ);

    // this invokes the damage callbacks
    vterm_screen_flush_damage(vterm_obtain_screen(vterm));
}

    ಸ್ಥಿರ ಶೂನ್ಯ
position_cursor(win_T *wp, VTermPos *pos)
{
    wp->w_wrow = MIN(pos->ಅಡ್ಡಸಾಲು, MAX(0, wp->w_ಎತ್ತರ - 1));
    wp->w_wcol = MIN(pos->ಲಂಬಸಾಲು, MAX(0, wp->w_ಅಗಲ - 1));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
    ಆದರೆ (popup_is_popup(wp))
    {
	wp->w_wrow += popup_ತುದಿ_extra(wp);
	wp->w_wcol += popup_ಎಡ_extra(wp);
	wp->w_flags |= WFLAG_WCOL_OFF_ADDED | WFLAG_WROW_OFF_ADDED;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	wp->w_flags &= ~(WFLAG_WCOL_OFF_ADDED | WFLAG_WROW_OFF_ADDED);
#ಆದರೆ_ಅಂತ್ಯ
    wp->w_valid |= (VALID_WCOL|VALID_WROW);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
update_cursor(ಪದ_T *ಪದ, ಪೂರ್ಣಾಂಕ redraw)
{
    ಆದರೆ (ಪದ->tl_ಸಾಮಾನ್ಯ_ಬಹುಲಕ)
	ಹಿಂತಿರುಗಿ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    ಆದರೆ (ಪದ->tl_system)
	windgoto(ಪದ->tl_cursor_pos.ಅಡ್ಡಸಾಲು + ಪದ->tl_toprow,
		firstwin->w_wincol + ಪದ->tl_cursor_pos.ಲಂಬಸಾಲು);
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (!ಪದ_ಕೆಲಸ_running(ಪದ))
	// avoid the cursor positioned below the last used line
	setcursor();
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// do not use the window cursor position
	position_cursor(curwin, &curbuf->b_ಪದ->tl_cursor_pos);
	windgoto(W_WINROW(curwin) + curwin->w_wrow,
		 curwin->w_wincol + curwin->w_wcol);
    }
    ಆದರೆ (redraw)
    {
	aco_save_T	aco;

	ಆದರೆ (ಪದ->tl_ಬಫರ್ == curbuf && ಪದ->tl_cursor_visible)
	    cursor_on();
	ಹೊರಹೋಗು_flush();
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
	ಆದರೆ (gui.ಒಳ_use)
	{
	    gui_update_cursor(FALSE, FALSE);
	    gui_mch_flush();
	}
#ಆದರೆ_ಅಂತ್ಯ
	// Make sure an invoked autocmd doesn't delete the buffer (and the
	// terminal) under our fingers.
	++ಪದ->tl_ಬಫರ್->b_locked;

	// save and restore curwin and curbuf, in case the autocmd changes them
	aucmd_prepbuf(&aco, curbuf);
	apply_autocmds(EVENT_TEXTCHANGEDT, ಖಾಲಿ, ಖಾಲಿ, FALSE, ಪದ->tl_ಬಫರ್);
	aucmd_restbuf(&aco);

	--ಪದ->tl_ಬಫರ್->b_locked;
    }
}

/*
 * Find the location of a scrollbackline in the buffer
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
scrollbackline_pos_ಒಳ_buf(ಪದ_T *ಪದ, ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, linenr_T *lnum, ಪೂರ್ಣಾಂಕ *ಆರಂಭ_ಲಂಬಸಾಲು, size_t *ಆರಂಭ_pos)
{
    sb_ಸಾಲು_T	*ಸಾಲುಗಳು = (sb_ಸಾಲು_T *)ಪದ->tl_scrollback.ga_ದತ್ತಾಂಶ;
    linenr_T    calc_lnum = ಪದ->tl_ಬಫರ್_scrolled;
    size_t      calc_pos = 0;
    ಪೂರ್ಣಾಂಕ         calc_ಲಂಬಸಾಲು = 0;
    ಪೂರ್ಣಾಂಕ         i;

    ಆದರೆ (ಅಡ್ಡಸಾಲು < 0 || ಅಡ್ಡಸಾಲು >= ಪದ->tl_scrollback.ga_len)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (ಅಡ್ಡಸಾಲು > ಪದ->tl_scrollback_scrolled)
    {
	// Lookback how far along in the top line we are
	ಗಾಗಿ (i = ಪದ->tl_scrollback_scrolled + 1; i > 0 && ಸಾಲುಗಳು[i].continuation; --i)
	{
	    calc_pos += ಸಾಲುಗಳು[i - 1].sb_ಬೈಟುಗಳು;
	    calc_ಲಂಬಸಾಲು += ಸಾಲುಗಳು[i - 1].sb_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;
	}
	i = ಪದ->tl_scrollback_scrolled + 1;
	calc_lnum = ಪದ->tl_ಬಫರ್_scrolled + 1;
	// Do not count this line's bytes/cols twice
	ಆದರೆ (ಸಾಲುಗಳು[i].continuation)
	    ++i;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	i = 1;
	calc_lnum = 1;
    }

    ಗಾಗಿ (; i <= ಅಡ್ಡಸಾಲು; ++i)
    {
	ಆದರೆ (!ಸಾಲುಗಳು[i].continuation)
	{
	    ++calc_lnum;
	    calc_pos = 0;
	    calc_ಲಂಬಸಾಲು = 0;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    calc_pos += ಸಾಲುಗಳು[i - 1].sb_ಬೈಟುಗಳು;
	    calc_ಲಂಬಸಾಲು += ಸಾಲುಗಳು[i - 1].sb_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;
	}
    }

    *lnum = calc_lnum;
    ಆದರೆ (ಆರಂಭ_ಲಂಬಸಾಲು)
	*ಆರಂಭ_ಲಂಬಸಾಲು = calc_ಲಂಬಸಾಲು;
    ಆದರೆ (ಆರಂಭ_pos)
	*ಆರಂಭ_pos = calc_pos;
}

/*
 * Find the location of a buffer line in the scrollback
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
bufline_pos_ಒಳ_scrollback(ಪದ_T *ಪದ, linenr_T lnum, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು, ಪೂರ್ಣಾಂಕ *ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ *wrapped_ಲಂಬಸಾಲು)
{
    buf_T	*buf = ಪದ->tl_ಬಫರ್;
    sb_ಸಾಲು_T	*ಸಾಲುಗಳು = (sb_ಸಾಲು_T *)ಪದ->tl_scrollback.ga_ದತ್ತಾಂಶ;
    linenr_T    calc_ಅಡ್ಡಸಾಲು = ಪದ->tl_scrollback_scrolled;
    ಪೂರ್ಣಾಂಕ         calc_ಲಂಬಸಾಲು = ಲಂಬಸಾಲು;
    linenr_T    l;

    ಆದರೆ (lnum > buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (lnum > ಪದ->tl_ಬಫರ್_scrolled)
    {
	calc_ಅಡ್ಡಸಾಲು = ಪದ->tl_scrollback_scrolled;
	l = ಪದ->tl_ಬಫರ್_scrolled + 1;

	ಆಗ (calc_ಅಡ್ಡಸಾಲು < ಪದ->tl_scrollback.ga_len && ಸಾಲುಗಳು[calc_ಅಡ್ಡಸಾಲು].continuation)
	    ++calc_ಅಡ್ಡಸಾಲು;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	calc_ಅಡ್ಡಸಾಲು = 0;
	l = 1;
    }

    ಆಗ (calc_ಅಡ್ಡಸಾಲು < ಪದ->tl_scrollback.ga_len && l < lnum)
    {
	++calc_ಅಡ್ಡಸಾಲು;
	ಆದರೆ (!ಸಾಲುಗಳು[calc_ಅಡ್ಡಸಾಲು].continuation)
	    ++l;
    }

    ಆಗ (calc_ಅಡ್ಡಸಾಲು + 1 < ಪದ->tl_scrollback.ga_len && ಸಾಲುಗಳು[calc_ಅಡ್ಡಸಾಲು + 1].continuation
	    && calc_ಲಂಬಸಾಲು >= ಸಾಲುಗಳು[calc_ಅಡ್ಡಸಾಲು].sb_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ)
    {
	calc_ಲಂಬಸಾಲು -= ಸಾಲುಗಳು[calc_ಅಡ್ಡಸಾಲು].sb_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;
	++calc_ಅಡ್ಡಸಾಲು;
    }

    ಆದರೆ (ಅಡ್ಡಸಾಲು)
	*ಅಡ್ಡಸಾಲು = calc_ಅಡ್ಡಸಾಲು;
    ಆದರೆ (wrapped_ಲಂಬಸಾಲು)
	*wrapped_ಲಂಬಸಾಲು = calc_ಲಂಬಸಾಲು;
}

/*
 * Invoked when "msg" output from a job was received.  Write it to the terminal
 * of "buffer".
 */
    ಶೂನ್ಯ
write_ಗಮ್ಯ_ಪದ(buf_T *ಬಫರ್, char_u *ಸಂದೇಶ, channel_T *channel)
{
    size_t	len = STRLEN(ಸಂದೇಶ);
    ಪದ_T	*ಪದ = ಬಫರ್->b_ಪದ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
    // Win32: Cannot redirect output of the job, intercept it here and write to
    // the file.
    ಆದರೆ (ಪದ->tl_ಹೊರಹೋಗು_fd != ಖಾಲಿ)
    {
	ch_log(channel, "Writing %d bytes to output file", (ಪೂರ್ಣಾಂಕ)len);
	fwrite(ಸಂದೇಶ, len, 1, ಪದ->tl_ಹೊರಹೋಗು_fd);
	ಹಿಂತಿರುಗಿ;
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (ಪದ->tl_vterm == ಖಾಲಿ)
    {
	ch_log(channel, "NOT writing %d bytes to terminal", (ಪೂರ್ಣಾಂಕ)len);
	ಹಿಂತಿರುಗಿ;
    }
    ch_log(channel, "writing %d bytes to terminal", (ಪೂರ್ಣಾಂಕ)len);
    cursor_off();
    ಪದ_write_ಕೆಲಸ_output(ಪದ, ಸಂದೇಶ, len);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    ಆದರೆ (ಪದ->tl_system)
    {
	// show system output, scrolling up the screen as needed
	update_system_ಪದ(ಪದ);
	update_cursor(ಪದ, TRUE);
    }
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
    // In Terminal-Normal mode we are displaying the buffer, not the terminal
    // contents, thus no screen update is needed.
    ಆದರೆ (!ಪದ->tl_ಸಾಮಾನ್ಯ_ಬಹುಲಕ)
    {
	// Don't use update_screen() when editing the command line, it gets
	// cleared.
	// TODO: only update once in a while.
	ch_log(ಪದ->tl_ಕೆಲಸ->jv_channel, "updating screen");
	ಆದರೆ (ಬಫರ್ == curbuf && (State & ಬಹುಲಕ_CMDLINE) == 0)
	{
	    update_screen(UPD_VALID_NO_UPDATE);
#ಆದರೆ defined(FEAT_TABPANEL)
	    ಆದರೆ (redraw_tabpanel)
		draw_tabpanel();
#ಆದರೆ_ಅಂತ್ಯ
	    // update_screen() can be slow, check the terminal wasn't closed
	    // already
	    ಆದರೆ (ಬಫರ್ == curbuf && curbuf->b_ಪದ != ಖಾಲಿ)
		update_cursor(curbuf->b_ಪದ, TRUE);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    redraw_after_callback(TRUE, FALSE);
    }
}

/*
 * Send a mouse position and click to the vterm
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಪದ_send_mouse(VTerm *vterm, ಪೂರ್ಣಾಂಕ button, ಪೂರ್ಣಾಂಕ pressed)
{
    VTermModifier   mod = VTERM_MOD_NONE;
    ಪೂರ್ಣಾಂಕ		    ಅಡ್ಡಸಾಲು = mouse_ಅಡ್ಡಸಾಲು - W_WINROW(curwin);
    ಪೂರ್ಣಾಂಕ		    ಲಂಬಸಾಲು = mouse_ಲಂಬಸಾಲು - curwin->w_wincol;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
    ಆದರೆ (popup_is_popup(curwin))
    {
	ಅಡ್ಡಸಾಲು -= popup_ತುದಿ_extra(curwin);
	ಲಂಬಸಾಲು -= popup_ಎಡ_extra(curwin);
    }
#ಆದರೆ_ಅಂತ್ಯ
    vterm_mouse_move(vterm, ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು, mod);
    ಆದರೆ (button != 0)
	vterm_mouse_button(vterm, button, pressed, mod);
    ಹಿಂತಿರುಗಿ TRUE;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ enter_mouse_ಲಂಬಸಾಲು = -1;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ enter_mouse_ಅಡ್ಡಸಾಲು = -1;

/*
 * Handle a mouse click, drag or release.
 * Return TRUE when a mouse event is sent to the terminal.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಪದ_mouse_click(VTerm *vterm, ಪೂರ್ಣಾಂಕ ಕೀಲಿ)
{
#ಆದರೆ defined(FEAT_CLIPBOARD)
    // For modeless selection mouse drag and release events are ignored, unless
    // they are preceded with a mouse down event
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	    ignore_drag_release = TRUE;
    VTermState	    *state;
    VTermMouseState mouse_state;


    state = vterm_obtain_state(vterm);
    ಆದರೆ (state == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FALSE;

    vterm_state_get_mousestate(state, &mouse_state);
    ಆದರೆ (mouse_state.flags == 0)
    {
	// Terminal is not using the mouse, use modeless selection.
	ಆಯ್ಕೆ (ಕೀಲಿ)
	{
	ಸ್ಥಿತಿ K_LEFTDRAG:
	ಸ್ಥಿತಿ K_LEFTRELEASE:
	ಸ್ಥಿತಿ K_RIGHTDRAG:
	ಸ್ಥಿತಿ K_RIGHTRELEASE:
		// Ignore drag and release events when the button-down wasn't
		// seen before.
		ಆದರೆ (ignore_drag_release)
		{
		    ಪೂರ್ಣಾಂಕ save_mouse_ಲಂಬಸಾಲು, save_mouse_ಅಡ್ಡಸಾಲು;

		    ಆದರೆ (enter_mouse_ಲಂಬಸಾಲು < 0)
			ನಿಲ್ಲಿಸು;

		    // mouse click in the window gave us focus, handle that
		    // click now
		    save_mouse_ಲಂಬಸಾಲು = mouse_ಲಂಬಸಾಲು;
		    save_mouse_ಅಡ್ಡಸಾಲು = mouse_ಅಡ್ಡಸಾಲು;
		    mouse_ಲಂಬಸಾಲು = enter_mouse_ಲಂಬಸಾಲು;
		    mouse_ಅಡ್ಡಸಾಲು = enter_mouse_ಅಡ್ಡಸಾಲು;
		    clip_modeless(MOUSE_ಎಡ, TRUE, FALSE);
		    mouse_ಲಂಬಸಾಲು = save_mouse_ಲಂಬಸಾಲು;
		    mouse_ಅಡ್ಡಸಾಲು = save_mouse_ಅಡ್ಡಸಾಲು;
		}
		// FALLTHROUGH
	ಸ್ಥಿತಿ K_LEFTMOUSE:
	ಸ್ಥಿತಿ K_RIGHTMOUSE:
		ಆದರೆ (ಕೀಲಿ == K_LEFTRELEASE || ಕೀಲಿ == K_RIGHTRELEASE)
		    ignore_drag_release = TRUE;
		ಇಲ್ಲದಿದ್ದರೆ
		    ignore_drag_release = FALSE;
		// Should we call mouse_has() here?
		ಆದರೆ (clip_ನಕ್ಷತ್ರ.ಲಭ್ಯ)
		{
		    ಪೂರ್ಣಾಂಕ	    button, is_click, is_drag;

		    button = get_mouse_button(KEY2TERMCAP1(ಕೀಲಿ),
							 &is_click, &is_drag);
		    ಆದರೆ (mouse_model_popup() && button == MOUSE_ಎಡ
					       && (mod_ಮುಖವಾಡ & MOD_ಮುಖವಾಡ_ಸರಿಸು))
		    {
			// Translate shift-left to right button.
			button = MOUSE_ಬಲ;
			mod_ಮುಖವಾಡ &= ~MOD_ಮುಖವಾಡ_ಸರಿಸು;
		    }
		    clip_modeless(button, is_click, is_drag);
		}
		ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ K_MIDDLEMOUSE:
		ಆದರೆ (clip_ನಕ್ಷತ್ರ.ಲಭ್ಯ)
		    ಸೇರಿಸು_reg('*', TRUE);
		ನಿಲ್ಲಿಸು;
	}
	enter_mouse_ಲಂಬಸಾಲು = -1;
	ಹಿಂತಿರುಗಿ FALSE;
    }
#ಆದರೆ_ಅಂತ್ಯ
    enter_mouse_ಲಂಬಸಾಲು = -1;

    ಆಯ್ಕೆ (ಕೀಲಿ)
    {
	ಸ್ಥಿತಿ K_LEFTMOUSE:
	ಸ್ಥಿತಿ K_LEFTMOUSE_NM:	ಪದ_send_mouse(vterm, 1, 1); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_LEFTDRAG:	ಪದ_send_mouse(vterm, 1, 1); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_LEFTRELEASE:
	ಸ್ಥಿತಿ K_LEFTRELEASE_NM:	ಪದ_send_mouse(vterm, 1, 0); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_MOUSEMOVE:	ಪದ_send_mouse(vterm, 0, 0); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_MIDDLEMOUSE:	ಪದ_send_mouse(vterm, 2, 1); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_MIDDLEDRAG:	ಪದ_send_mouse(vterm, 2, 1); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_MIDDLERELEASE:	ಪದ_send_mouse(vterm, 2, 0); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_RIGHTMOUSE:	ಪದ_send_mouse(vterm, 3, 1); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_RIGHTDRAG:	ಪದ_send_mouse(vterm, 3, 1); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_RIGHTRELEASE:	ಪದ_send_mouse(vterm, 3, 0); ನಿಲ್ಲಿಸು;
    }
    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * Convert typed key "c" with modifiers "modmask" into bytes to send to the
 * job.
 * Return the number of bytes in "buf".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಪದ_convert_ಕೀಲಿ(ಪದ_T *ಪದ, ಪೂರ್ಣಾಂಕ c, ಪೂರ್ಣಾಂಕ modmask, ಅಕ್ಷರ *buf)
{
    VTerm	    *vterm = ಪದ->tl_vterm;
    VTerm_ಕೀಲಿ	    ಕೀಲಿ = VTERM_ಕೀಲಿ_NONE;
    VTermModifier   mod = VTERM_MOD_NONE;
    ಪೂರ್ಣಾಂಕ		    other = FALSE;

    ಆಯ್ಕೆ (c)
    {
	// don't use VTERM_KEY_ENTER, it may do an unwanted conversion

	ಸ್ಥಿತಿ K_BS:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
	    // In ConPTY, we must use VTERM_KEY_BACKSPACE, otherwise it will
	    // delete one word, equivalent to Alt+Backspace.
	    ಆದರೆ (ಪದ->tl_conpty)
		ಕೀಲಿ = VTERM_ಕೀಲಿ_BACKSPACE;
	    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
		// don't use VTERM_KEY_BACKSPACE, it always becomes 0x7f DEL
		c = ಪದ_backspace_char;
	    ನಿಲ್ಲಿಸು;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
	ಸ್ಥಿತಿ BS:
	    // In ConPTY, we must use VTERM_KEY_BACKSPACE, otherwise it will
	    // delete one word, equivalent to Alt+Backspace.
	    ಆದರೆ (ಪದ->tl_conpty)
		ಕೀಲಿ = VTERM_ಕೀಲಿ_BACKSPACE;
	    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ

	ಸ್ಥಿತಿ ESC:		ಕೀಲಿ = VTERM_ಕೀಲಿ_ESCAPE; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_DEL:		ಕೀಲಿ = VTERM_ಕೀಲಿ_DEL; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_ಕೆಳಕ್ಕೆ:		ಕೀಲಿ = VTERM_ಕೀಲಿ_ಕೆಳಕ್ಕೆ; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_S_ಕೆಳಕ್ಕೆ:		mod = VTERM_MOD_ಸರಿಸು;
				ಕೀಲಿ = VTERM_ಕೀಲಿ_ಕೆಳಕ್ಕೆ; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_ಕೊನೆ:		ಕೀಲಿ = VTERM_ಕೀಲಿ_ಕೊನೆ; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_S_ಕೊನೆ:		mod = VTERM_MOD_ಸರಿಸು;
				ಕೀಲಿ = VTERM_ಕೀಲಿ_ಕೊನೆ; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_C_ಕೊನೆ:		mod = VTERM_MOD_CTRL;
				ಕೀಲಿ = VTERM_ಕೀಲಿ_ಕೊನೆ; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_F10:		ಕೀಲಿ = VTERM_ಕೀಲಿ_FUNCTION(10); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_F11:		ಕೀಲಿ = VTERM_ಕೀಲಿ_FUNCTION(11); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_F12:		ಕೀಲಿ = VTERM_ಕೀಲಿ_FUNCTION(12); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_F1:		ಕೀಲಿ = VTERM_ಕೀಲಿ_FUNCTION(1); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_F2:		ಕೀಲಿ = VTERM_ಕೀಲಿ_FUNCTION(2); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_F3:		ಕೀಲಿ = VTERM_ಕೀಲಿ_FUNCTION(3); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_F4:		ಕೀಲಿ = VTERM_ಕೀಲಿ_FUNCTION(4); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_F5:		ಕೀಲಿ = VTERM_ಕೀಲಿ_FUNCTION(5); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_F6:		ಕೀಲಿ = VTERM_ಕೀಲಿ_FUNCTION(6); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_F7:		ಕೀಲಿ = VTERM_ಕೀಲಿ_FUNCTION(7); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_F8:		ಕೀಲಿ = VTERM_ಕೀಲಿ_FUNCTION(8); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_F9:		ಕೀಲಿ = VTERM_ಕೀಲಿ_FUNCTION(9); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_HOME:		ಕೀಲಿ = VTERM_ಕೀಲಿ_HOME; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_S_HOME:		mod = VTERM_MOD_ಸರಿಸು;
				ಕೀಲಿ = VTERM_ಕೀಲಿ_HOME; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_C_HOME:		mod = VTERM_MOD_CTRL;
				ಕೀಲಿ = VTERM_ಕೀಲಿ_HOME; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_INS:		ಕೀಲಿ = VTERM_ಕೀಲಿ_INS; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_K0:		ಕೀಲಿ = VTERM_ಕೀಲಿ_KP_0; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_K1:		ಕೀಲಿ = VTERM_ಕೀಲಿ_KP_1; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_K2:		ಕೀಲಿ = VTERM_ಕೀಲಿ_KP_2; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_K3:		ಕೀಲಿ = VTERM_ಕೀಲಿ_KP_3; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_K4:		ಕೀಲಿ = VTERM_ಕೀಲಿ_KP_4; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_K5:		ಕೀಲಿ = VTERM_ಕೀಲಿ_KP_5; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_K6:		ಕೀಲಿ = VTERM_ಕೀಲಿ_KP_6; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_K7:		ಕೀಲಿ = VTERM_ಕೀಲಿ_KP_7; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_K8:		ಕೀಲಿ = VTERM_ಕೀಲಿ_KP_8; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_K9:		ಕೀಲಿ = VTERM_ಕೀಲಿ_KP_9; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_KDEL:		ಕೀಲಿ = VTERM_ಕೀಲಿ_DEL; ನಿಲ್ಲಿಸು; // TODO
	ಸ್ಥಿತಿ K_KDIVIDE:		ಕೀಲಿ = VTERM_ಕೀಲಿ_KP_DIVIDE; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_KEND:		ಕೀಲಿ = VTERM_ಕೀಲಿ_KP_1; ನಿಲ್ಲಿಸು; // TODO
	ಸ್ಥಿತಿ K_KENTER:		ಕೀಲಿ = VTERM_ಕೀಲಿ_KP_ENTER; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_KHOME:		ಕೀಲಿ = VTERM_ಕೀಲಿ_KP_7; ನಿಲ್ಲಿಸು; // TODO
	ಸ್ಥಿತಿ K_KINS:		ಕೀಲಿ = VTERM_ಕೀಲಿ_KP_0; ನಿಲ್ಲಿಸು; // TODO
	ಸ್ಥಿತಿ K_KMINUS:		ಕೀಲಿ = VTERM_ಕೀಲಿ_KP_MINUS; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_KMULTIPLY:	ಕೀಲಿ = VTERM_ಕೀಲಿ_KP_MULT; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_KPAGEDOWN:	ಕೀಲಿ = VTERM_ಕೀಲಿ_KP_3; ನಿಲ್ಲಿಸು; // TODO
	ಸ್ಥಿತಿ K_KPAGEUP:		ಕೀಲಿ = VTERM_ಕೀಲಿ_KP_9; ನಿಲ್ಲಿಸು; // TODO
	ಸ್ಥಿತಿ K_KPLUS:		ಕೀಲಿ = VTERM_ಕೀಲಿ_KP_PLUS; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_KPOINT:		ಕೀಲಿ = VTERM_ಕೀಲಿ_KP_PERIOD; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_ಎಡ:		ಕೀಲಿ = VTERM_ಕೀಲಿ_ಎಡ; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_S_ಎಡ:		mod = VTERM_MOD_ಸರಿಸು;
				ಕೀಲಿ = VTERM_ಕೀಲಿ_ಎಡ; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_C_ಎಡ:		mod = VTERM_MOD_CTRL;
				ಕೀಲಿ = VTERM_ಕೀಲಿ_ಎಡ; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_PAGEDOWN:	ಕೀಲಿ = VTERM_ಕೀಲಿ_PAGEDOWN; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_PAGEUP:		ಕೀಲಿ = VTERM_ಕೀಲಿ_PAGEUP; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_ಬಲ:		ಕೀಲಿ = VTERM_ಕೀಲಿ_ಬಲ; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_S_ಬಲ:		mod = VTERM_MOD_ಸರಿಸು;
				ಕೀಲಿ = VTERM_ಕೀಲಿ_ಬಲ; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_C_ಬಲ:		mod = VTERM_MOD_CTRL;
				ಕೀಲಿ = VTERM_ಕೀಲಿ_ಬಲ; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_ಮೇಲೆ:		ಕೀಲಿ = VTERM_ಕೀಲಿ_ಮೇಲೆ; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_S_ಮೇಲೆ:		mod = VTERM_MOD_ಸರಿಸು;
				ಕೀಲಿ = VTERM_ಕೀಲಿ_ಮೇಲೆ; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ TAB:		ಕೀಲಿ = VTERM_ಕೀಲಿ_TAB; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_S_TAB:		mod = VTERM_MOD_ಸರಿಸು;
				ಕೀಲಿ = VTERM_ಕೀಲಿ_TAB; ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ K_MOUSEUP:		other = ಪದ_send_mouse(vterm, 5, 1); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_MOUSEDOWN:	other = ಪದ_send_mouse(vterm, 4, 1); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_MOUSELEFT:	other = ಪದ_send_mouse(vterm, 7, 1); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_MOUSERIGHT:	other = ಪದ_send_mouse(vterm, 6, 1); ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ K_LEFTMOUSE:
	ಸ್ಥಿತಿ K_LEFTMOUSE_NM:
	ಸ್ಥಿತಿ K_LEFTDRAG:
	ಸ್ಥಿತಿ K_LEFTRELEASE:
	ಸ್ಥಿತಿ K_LEFTRELEASE_NM:
	ಸ್ಥಿತಿ K_MOUSEMOVE:
	ಸ್ಥಿತಿ K_MIDDLEMOUSE:
	ಸ್ಥಿತಿ K_MIDDLEDRAG:
	ಸ್ಥಿತಿ K_MIDDLERELEASE:
	ಸ್ಥಿತಿ K_RIGHTMOUSE:
	ಸ್ಥಿತಿ K_RIGHTDRAG:
	ಸ್ಥಿತಿ K_RIGHTRELEASE:	ಆದರೆ (!ಪದ_mouse_click(vterm, c))
				    ಹಿಂತಿರುಗಿ 0;
				other = TRUE;
				ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ K_X1MOUSE:		/* TODO */ ಹಿಂತಿರುಗಿ 0;
	ಸ್ಥಿತಿ K_X1DRAG:		/* TODO */ ಹಿಂತಿರುಗಿ 0;
	ಸ್ಥಿತಿ K_X1RELEASE:	/* TODO */ ಹಿಂತಿರುಗಿ 0;
	ಸ್ಥಿತಿ K_X2MOUSE:		/* TODO */ ಹಿಂತಿರುಗಿ 0;
	ಸ್ಥಿತಿ K_X2DRAG:		/* TODO */ ಹಿಂತಿರುಗಿ 0;
	ಸ್ಥಿತಿ K_X2RELEASE:	/* TODO */ ಹಿಂತಿರುಗಿ 0;

	ಸ್ಥಿತಿ K_IGNORE:		ಹಿಂತಿರುಗಿ 0;
	ಸ್ಥಿತಿ K_NOP:		ಹಿಂತಿರುಗಿ 0;
	ಸ್ಥಿತಿ K_UNDO:		ಹಿಂತಿರುಗಿ 0;
	ಸ್ಥಿತಿ K_HELP:		ಹಿಂತಿರುಗಿ 0;
	ಸ್ಥಿತಿ K_XF1:		ಕೀಲಿ = VTERM_ಕೀಲಿ_FUNCTION(1); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_XF2:		ಕೀಲಿ = VTERM_ಕೀಲಿ_FUNCTION(2); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_XF3:		ಕೀಲಿ = VTERM_ಕೀಲಿ_FUNCTION(3); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_XF4:		ಕೀಲಿ = VTERM_ಕೀಲಿ_FUNCTION(4); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_SELECT:		ಹಿಂತಿರುಗಿ 0;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
	ಸ್ಥಿತಿ K_VER_SCROLLBAR:	ಹಿಂತಿರುಗಿ 0;
	ಸ್ಥಿತಿ K_HOR_SCROLLBAR:	ಹಿಂತಿರುಗಿ 0;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_TABLINE
	ಸ್ಥಿತಿ K_TABLINE:		ಹಿಂತಿರುಗಿ 0;
	ಸ್ಥಿತಿ K_TABMENU:		ಹಿಂತಿರುಗಿ 0;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_NETBEANS_INTG
	ಸ್ಥಿತಿ K_F21:		ಕೀಲಿ = VTERM_ಕೀಲಿ_FUNCTION(21); ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DND
	ಸ್ಥಿತಿ K_DROP:		ಹಿಂತಿರುಗಿ 0;
#ಆದರೆ_ಅಂತ್ಯ
	ಸ್ಥಿತಿ K_CURSORHOLD:	ಹಿಂತಿರುಗಿ 0;
	ಸ್ಥಿತಿ K_PS:		vterm_keyboard_ಆರಂಭ_paste(vterm);
				other = TRUE;
				ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_PE:		vterm_keyboard_ಕೊನೆ_paste(vterm);
				other = TRUE;
				ನಿಲ್ಲಿಸು;
    }

    // add modifiers for the typed key
    ಆದರೆ (modmask & MOD_ಮುಖವಾಡ_ಸರಿಸು)
	mod |= VTERM_MOD_ಸರಿಸು;
    ಆದರೆ (modmask & MOD_ಮುಖವಾಡ_CTRL)
	mod |= VTERM_MOD_CTRL;
    ಆದರೆ (modmask & (MOD_ಮುಖವಾಡ_ALT | MOD_ಮುಖವಾಡ_META))
	mod |= VTERM_MOD_ALT;

    // Ctrl-Shift-i may have the key "I" instead of "i", but for the kitty
    // keyboard protocol should use "i".  Applies to all ascii letters.
    ಆದರೆ (ASCII_ISUPPER(c)
	    && vterm_is_kitty_keyboard(vterm)
	    && mod == (VTERM_MOD_CTRL | VTERM_MOD_ಸರಿಸು))
	c = TOLOWER_ASC(c);

    /*
     * Convert special keys to vterm keys:
     * - Write keys to vterm: vterm_keyboard_key()
     * - Write output to channel.
     */
    ಆದರೆ (ಕೀಲಿ != VTERM_ಕೀಲಿ_NONE)
	// Special key, let vterm convert it.
	vterm_keyboard_ಕೀಲಿ(vterm, ಕೀಲಿ, mod);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!other)
	// Normal character, let vterm convert it.
	vterm_keyboard_unichar(vterm, c, mod);

    // Read back the converted escape sequence.
    ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ)vterm_output_read(vterm, buf, ಕೀಲಿ_BUF_LEN);
}

/*
 * Return TRUE if the job for "term" is still running.
 * If "check_job_status" is TRUE update the job status.
 * NOTE: "term" may be freed by callbacks.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಪದ_ಕೆಲಸ_running_check(ಪದ_T *ಪದ, ಪೂರ್ಣಾಂಕ check_ಕೆಲಸ_ಸ್ಥಿತಿ)
{
    // Also consider the job finished when the channel is closed, to avoid a
    // race condition when updating the title.
    ಆದರೆ (ಪದ == ಖಾಲಿ
	|| ಪದ->tl_ಕೆಲಸ == ಖಾಲಿ
	|| !channel_is_open(ಪದ->tl_ಕೆಲಸ->jv_channel))
	ಹಿಂತಿರುಗಿ FALSE;

    ಕೆಲಸ_T *ಕೆಲಸ = ಪದ->tl_ಕೆಲಸ;

    // Careful: Checking the job status may invoke callbacks, which close
    // the buffer and terminate "term".  However, "job" will not be freed
    // yet.
    ಆದರೆ (check_ಕೆಲಸ_ಸ್ಥಿತಿ)
	ಕೆಲಸ_ಸ್ಥಿತಿ(ಕೆಲಸ);
    ಹಿಂತಿರುಗಿ (ಕೆಲಸ->jv_ಸ್ಥಿತಿ == ಕೆಲಸ_STARTED
	    || (ಕೆಲಸ->jv_channel != ಖಾಲಿ && ಕೆಲಸ->jv_channel->ch_keep_open));
}

/*
 * Return TRUE if the job for "term" is still running.
 */
    ಪೂರ್ಣಾಂಕ
ಪದ_ಕೆಲಸ_running(ಪದ_T *ಪದ)
{
    ಹಿಂತಿರುಗಿ ಪದ_ಕೆಲಸ_running_check(ಪದ, FALSE);
}

/*
 * Return TRUE if the job for "term" is still running, ignoring the job was
 * "NONE".
 */
    ಪೂರ್ಣಾಂಕ
ಪದ_ಕೆಲಸ_running_not_none(ಪದ_T *ಪದ)
{
    ಹಿಂತಿರುಗಿ ಪದ_ಕೆಲಸ_running(ಪದ) && !ಪದ_none_open(ಪದ);
}

/*
 * Return TRUE if "term" has an active channel and used ":term NONE".
 */
    ಪೂರ್ಣಾಂಕ
ಪದ_none_open(ಪದ_T *ಪದ)
{
    // Also consider the job finished when the channel is closed, to avoid a
    // race condition when updating the title.
    ಹಿಂತಿರುಗಿ ಪದ != ಖಾಲಿ
	&& ಪದ->tl_ಕೆಲಸ != ಖಾಲಿ
	&& channel_is_open(ಪದ->tl_ಕೆಲಸ->jv_channel)
	&& ಪದ->tl_ಕೆಲಸ->jv_channel->ch_keep_open;
}

//
// Used to confirm whether we would like to kill a terminal.
// Return OK when the user confirms to kill it.
// Return FAIL if the user selects otherwise.
//
    ಪೂರ್ಣಾಂಕ
ಪದ_confirm_stop(buf_T *buf)
{
    char_u	buff[DIALOG_ಸಂದೇಶ_ಗಾತ್ರ];
    ಪೂರ್ಣಾಂಕ	ret;

    dialog_ಸಂದೇಶ(buff, _("Kill job in \"%s\"?"), buf_get_fname(buf));
    ret = vim_dialog_yesno(VIM_QUESTION, ಖಾಲಿ, buff, 1);
    ಆದರೆ (ret == VIM_YES)
	ಹಿಂತಿರುಗಿ OK;
    ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ FAIL;
}

/*
 * Used when exiting: kill the job in "buf" if so desired.
 * Return OK when the job finished.
 * Return FAIL when the job is still running.
 */
    ಪೂರ್ಣಾಂಕ
ಪದ_try_stop_ಕೆಲಸ(buf_T *buf)
{
    ಪೂರ್ಣಾಂಕ	    ಎಣಿಕೆ_2;
    ಅಕ್ಷರ    *how = (ಅಕ್ಷರ *)buf->b_ಪದ->tl_kill;

#ಆದರೆ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
    ಆದರೆ ((how == ಖಾಲಿ || *how == NUL)
			  && (p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)))
    {
	ಆದರೆ (ಪದ_confirm_stop(buf) == OK)
	    how = "kill";
	ಇಲ್ಲದಿದ್ದರೆ
	    ಹಿಂತಿರುಗಿ FAIL;
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (how == ಖಾಲಿ || *how == NUL)
	ಹಿಂತಿರುಗಿ FAIL;

    ಕೆಲಸ_stop(buf->b_ಪದ->tl_ಕೆಲಸ, ಖಾಲಿ, how);

    // wait for up to a second for the job to die
    ಗಾಗಿ (ಎಣಿಕೆ_2 = 0; ಎಣಿಕೆ_2 < 100; ++ಎಣಿಕೆ_2)
    {
	ಕೆಲಸ_T *ಕೆಲಸ;

	// buffer, terminal and job may be cleaned up while waiting
	ಆದರೆ (!buf_valid(buf)
		|| buf->b_ಪದ == ಖಾಲಿ
		|| buf->b_ಪದ->tl_ಕೆಲಸ == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ OK;
	ಕೆಲಸ = buf->b_ಪದ->tl_ಕೆಲಸ;

	// Call job_status() to update jv_status. It may cause the job to be
	// cleaned up but it won't be freed.
	ಕೆಲಸ_ಸ್ಥಿತಿ(ಕೆಲಸ);
	ಆದರೆ (ಕೆಲಸ->jv_ಸ್ಥಿತಿ >= ಕೆಲಸ_ENDED)
	    ಹಿಂತಿರುಗಿ OK;

	ui_delay(10L, TRUE);
	ಪದ_flush_messages();
    }
    ಹಿಂತಿರುಗಿ FAIL;
}

/*
 * Add the last line of the scrollback buffer to the buffer in the window.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಕೂಡಿಸು_scrollback_ಸಾಲು_ಗಮ್ಯ_ಬಫರ್(ಪದ_T *ಪದ, char_u *ಪಠ್ಯ, ಪೂರ್ಣಾಂಕ len, ಪೂರ್ಣಾಂಕ append)
{
    buf_T	*buf = ಪದ->tl_ಬಫರ್;
    ಪೂರ್ಣಾಂಕ		ಖಾಲಿಗಳು_2 = (buf->b_ml.ml_flags & ML_ಖಾಲಿಗಳು);
    linenr_T	lnum = buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
    char_u *tmp = ಪಠ್ಯ;
    ಆದರೆ (!enc_utf8 && enc_codepage > 0)
    {
	WCHAR   *ret = ಖಾಲಿ;
	ಪೂರ್ಣಾಂಕ	ಉದ್ದಳತೆ = 0;

	Multi_Byte_ಗಮ್ಯ_Wide_Char_alloc(CP_UTF8, 0, (ಅಕ್ಷರ*)ಪಠ್ಯ, len + 1,
							   &ret, &ಉದ್ದಳತೆ);
	ಆದರೆ (ret != ಖಾಲಿ)
	{
	    Wide_Char_ಗಮ್ಯ_Multi_Byte_alloc(enc_codepage, 0,
				      ret, ಉದ್ದಳತೆ, (ಅಕ್ಷರ **)&ಪಠ್ಯ, &len, 0, 0);
	    vim_free(ret);
	}
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (append)
    {
	char_u *prev_ಪಠ್ಯ = ml_get_buf(buf, lnum, FALSE);
	size_t prev_len = STRLEN(prev_ಪಠ್ಯ);

	char_u *both = alloc(len + prev_len + 2);
	ಆದರೆ (both == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ;
	vim_strncpy(both, prev_ಪಠ್ಯ, prev_len + 1);
	vim_strncpy(both + prev_len, ಪಠ್ಯ, len + 1);

	curbuf = buf;
	ml_replace(lnum, both, FALSE);
	curbuf = curwin->w_ಬಫರ್;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	ml_append_buf(buf, lnum, ಪಠ್ಯ, len + 1, FALSE);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
    ಆದರೆ (tmp != ಪಠ್ಯ)
	vim_free(ಪಠ್ಯ);
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (ಖಾಲಿಗಳು_2)
    {
	// Delete the empty line that was in the empty buffer.
	curbuf = buf;
	ml_delete(1);
	curbuf = curwin->w_ಬಫರ್;
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
cell2cellattr(ಸ್ಥಿರಾಂಕ VTermScreenCell *cell, cellattr_T *attr)
{
    attr->ಅಗಲ = cell->ಅಗಲ;
    attr->attrs = cell->attrs;
    attr->fg = cell->fg;
    attr->bg = cell->bg;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
equal_celattr(cellattr_T *a, cellattr_T *b)
{
    // We only compare the RGB colors, ignoring the ANSI index and type.
    // Thus black set explicitly is equal the background black.
    ಹಿಂತಿರುಗಿ a->fg.red == b->fg.red
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಕೂಡಿಸು_ಖಾಲಿಗಳು_scrollback(ಪದ_T *ಪದ, cellattr_T *fill_attr, ಪೂರ್ಣಾಂಕ lnum)
{
    ಆದರೆ (ga_grow(&ಪದ->tl_scrollback, 1) == FAIL)
	ಹಿಂತಿರುಗಿ FALSE;

    sb_ಸಾಲು_T *ಸಾಲು = (sb_ಸಾಲು_T *)ಪದ->tl_scrollback.ga_ದತ್ತಾಂಶ
	+ ಪದ->tl_scrollback.ga_len;

    ಆದರೆ (lnum > 0)
    {
	ಪೂರ್ಣಾಂಕ i;

	ಗಾಗಿ (i = 0; i < ಪದ->tl_scrollback.ga_len - lnum; ++i)
	{
	    *ಸಾಲು = *(ಸಾಲು - 1);
	    --ಸಾಲು;
	}
    }
    ಸಾಲು->sb_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = 0;
    ಸಾಲು->sb_ಬೈಟುಗಳು = 0;
    ಸಾಲು->sb_cells = ಖಾಲಿ;
    ಸಾಲು->sb_fill_attr = *fill_attr;
    ++ಪದ->tl_scrollback.ga_len;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Remove the terminal contents from the scrollback and the buffer.
 * Used before adding a new scrollback line or updating the buffer for lines
 * displayed in the terminal.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
cleanup_scrollback(ಪದ_T *ಪದ)
{
    sb_ಸಾಲು_T	*ಸಾಲು;
    garray_T	*gap;
    char_u	*bufline;
    size_t      bufline_ಉದ್ದಳತೆ;

    curbuf = ಪದ->tl_ಬಫರ್;
    gap = &ಪದ->tl_scrollback;
    bufline = ml_get_buf(curbuf, curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ, FALSE);
    bufline_ಉದ್ದಳತೆ = STRLEN(bufline);
    ಆಗ (ಪದ->tl_scrollback_snapshot && gap->ga_len > 0)
    {
	ಸಾಲು = (sb_ಸಾಲು_T *)gap->ga_ದತ್ತಾಂಶ + gap->ga_len - 1;
	ಆದರೆ (ಸಾಲು->sb_ಬೈಟುಗಳು < 0 || (size_t)ಸಾಲು->sb_ಬೈಟುಗಳು > bufline_ಉದ್ದಳತೆ)
	    ನಿಲ್ಲಿಸು;
	bufline_ಉದ್ದಳತೆ -= ಸಾಲು->sb_ಬೈಟುಗಳು;
	ಆದರೆ (!bufline_ಉದ್ದಳತೆ)
	{
	    ml_delete(curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ);
	    bufline = ml_get_buf(curbuf, curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ, FALSE);
	    bufline_ಉದ್ದಳತೆ = STRLEN(bufline);
	}
	vim_free(ಸಾಲು->sb_cells);
	--gap->ga_len;
	--ಪದ->tl_scrollback_snapshot;
    }
    ಆದರೆ (bufline_ಉದ್ದಳತೆ < STRLEN(bufline))
    {
	char_u *shortened = vim_strnsave(bufline, bufline_ಉದ್ದಳತೆ);
	ml_replace(curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ, shortened, FALSE);
    }
    curbuf = curwin->w_ಬಫರ್;
    ಆದರೆ (curbuf == ಪದ->tl_ಬಫರ್)
	check_cursor();
}

/*
 * Add the current lines of the terminal to scrollback and to the buffer.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
update_snapshot(ಪದ_T *ಪದ)
{
    VTermScreen	    *screen;
    VTermState	    *state;
    ಪೂರ್ಣಾಂಕ		    len;
    ಪೂರ್ಣಾಂಕ		    ಸಾಲುಗಳು_skipped = 0;
    VTermPos	    pos;
    VTermScreenCell cell;
    cellattr_T	    fill_attr, new_fill_attr;
    cellattr_T	    *p;

    ch_log(ಪದ->tl_ಕೆಲಸ == ಖಾಲಿ ? ಖಾಲಿ : ಪದ->tl_ಕೆಲಸ->jv_channel,
				  "Adding terminal window snapshot to buffer");

    // First remove the lines that were appended before, they might be
    // outdated.
    cleanup_scrollback(ಪದ);

    screen = vterm_obtain_screen(ಪದ->tl_vterm);
    state = vterm_obtain_state(ಪದ->tl_vterm);
    fill_attr = new_fill_attr = ಪದ->tl_default_color;
    ಗಾಗಿ (pos.ಅಡ್ಡಸಾಲು = 0; pos.ಅಡ್ಡಸಾಲು < ಪದ->tl_ಅಡ್ಡಸಾಲುಗಳು; ++pos.ಅಡ್ಡಸಾಲು)
    {
	len = 0;
	ಗಾಗಿ (pos.ಲಂಬಸಾಲು = 0; pos.ಲಂಬಸಾಲು < ಪದ->tl_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ; ++pos.ಲಂಬಸಾಲು)
	    ಆದರೆ (vterm_screen_get_cell(screen, pos, &cell) != 0
						       && cell.chars[0] != NUL)
	    {
		len = pos.ಲಂಬಸಾಲು + 1;
		new_fill_attr = ಪದ->tl_default_color;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		// Assume the last attr is the filler attr.
		cell2cellattr(&cell, &new_fill_attr);

	ಆದರೆ (len == 0 && equal_celattr(&new_fill_attr, &fill_attr))
	    ++ಸಾಲುಗಳು_skipped;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆಗ (ಸಾಲುಗಳು_skipped > 0)
	    {
		// Line was skipped, add an empty line.
		--ಸಾಲುಗಳು_skipped;
		ಆದರೆ (ಕೂಡಿಸು_ಖಾಲಿಗಳು_scrollback(ಪದ, &fill_attr, 0) == OK)
		{
		    ಕೂಡಿಸು_scrollback_ಸಾಲು_ಗಮ್ಯ_ಬಫರ್(ಪದ, (char_u *)"", 0, 0);
		    ++ಪದ->tl_scrollback_snapshot;
		}
	    }

	    ಆದರೆ (len == 0)
		p = ಖಾಲಿ;
	    ಇಲ್ಲದಿದ್ದರೆ
		p = ALLOC_MULT(cellattr_T, len);
	    ಆದರೆ ((p != ಖಾಲಿ || len == 0)
				     && ga_grow(&ಪದ->tl_scrollback, 1) == OK)
	    {
		garray_T    ga;
		ಪೂರ್ಣಾಂಕ	    ಅಗಲ;
		ಸ್ಥಿರಾಂಕ VTerm_ಸಾಲು_Info *lineinfo;
		sb_ಸಾಲು_T   *ಸಾಲು = (sb_ಸಾಲು_T *)ಪದ->tl_scrollback.ga_ದತ್ತಾಂಶ
						  + ಪದ->tl_scrollback.ga_len;

		lineinfo = vterm_state_get_lineinfo(state, pos.ಅಡ್ಡಸಾಲು);

		ga_init2(&ga, 1, 100);
		ಗಾಗಿ (pos.ಲಂಬಸಾಲು = 0; pos.ಲಂಬಸಾಲು < len; pos.ಲಂಬಸಾಲು += ಅಗಲ)
		{
		    ಆದರೆ (vterm_screen_get_cell(screen, pos, &cell) == 0)
		    {
			ಅಗಲ = 1;
			CLEAR_ಸೂಚಕ(p + pos.ಲಂಬಸಾಲು);
			ಆದರೆ (ga_grow(&ga, 1) == OK)
			    ga.ga_len += utf_char2bytes(' ',
					     (char_u *)ga.ga_ದತ್ತಾಂಶ + ga.ga_len);
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			ಅಗಲ = cell.ಅಗಲ;

			cell2cellattr(&cell, &p[pos.ಲಂಬಸಾಲು]);
			ಆದರೆ (ಅಗಲ == 2)
			    // second cell of double-width character has the
			    // same attributes.
			    p[pos.ಲಂಬಸಾಲು + 1] = p[pos.ಲಂಬಸಾಲು];

			// Each character can be up to 6 bytes.
			ಆದರೆ (ga_grow(&ga, VTERM_MAX_CHARS_ಪ್ರತಿ_CELL * 6) == OK)
			{
			    ಪೂರ್ಣಾಂಕ	    i;
			    ಪೂರ್ಣಾಂಕ	    c;

			    ಗಾಗಿ (i = 0; i < VTERM_MAX_CHARS_ಪ್ರತಿ_CELL &&
				    ((c = cell.chars[i]) > 0 || i == 0); ++i)
				ga.ga_len += utf_char2bytes(c == NUL ? ' ' : c,
					     (char_u *)ga.ga_ದತ್ತಾಂಶ + ga.ga_len);
			}
		    }
		}
		ಸಾಲು->sb_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = len;
		ಸಾಲು->sb_ಬೈಟುಗಳು = ga.ga_len;
		ಸಾಲು->sb_cells = p;
		ಸಾಲು->sb_fill_attr = new_fill_attr;
		ಸಾಲು->continuation = (char_u)lineinfo->continuation;
		fill_attr = new_fill_attr;
		++ಪದ->tl_scrollback.ga_len;
		++ಪದ->tl_scrollback_snapshot;

		ಆದರೆ (ga_grow(&ga, 1) == FAIL)
		    ಕೂಡಿಸು_scrollback_ಸಾಲು_ಗಮ್ಯ_ಬಫರ್(ಪದ, (char_u *)"", 0, 0);
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    *((char_u *)ga.ga_ದತ್ತಾಂಶ + ga.ga_len) = NUL;
		    ಕೂಡಿಸು_scrollback_ಸಾಲು_ಗಮ್ಯ_ಬಫರ್(ಪದ, ga.ga_ದತ್ತಾಂಶ, ga.ga_len,
						  lineinfo->continuation);
		}
		ga_clear(&ga);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		vim_free(p);
	}
    }

    // Add trailing empty lines.
    ಗಾಗಿ (pos.ಅಡ್ಡಸಾಲು = ಪದ->tl_scrollback.ga_len;
	    pos.ಅಡ್ಡಸಾಲು < ಪದ->tl_scrollback_scrolled + ಪದ->tl_cursor_pos.ಅಡ್ಡಸಾಲು;
	    ++pos.ಅಡ್ಡಸಾಲು)
    {
	ಆದರೆ (ಕೂಡಿಸು_ಖಾಲಿಗಳು_scrollback(ಪದ, &fill_attr, 0) == OK)
	{
	    ಕೂಡಿಸು_scrollback_ಸಾಲು_ಗಮ್ಯ_ಬಫರ್(ಪದ, (char_u *)"", 0, 0);
	    ++ಪದ->tl_scrollback_snapshot;
	}
    }

    ಪದ->tl_dirty_snapshot = FALSE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TIMERS
    ಪದ->tl_timer_ಗಣ = FALSE;
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Loop over all windows in the current tab, and also curwin, which is not
 * encountered when using a terminal in a popup window.
 * Return TRUE if "*wp" was set to the next window.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
for_all_windows_and_curwin(win_T **wp, ಪೂರ್ಣಾಂಕ *did_curwin)
{
    ಆದರೆ (*wp == ಖಾಲಿ)
	*wp = firstwin;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((*wp)->w_ಮುಂದಿನ != ಖಾಲಿ)
	*wp = (*wp)->w_ಮುಂದಿನ;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!*did_curwin)
	*wp = curwin;
    ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ FALSE;
    ಆದರೆ (*wp == curwin)
	*did_curwin = TRUE;
    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * If needed, add the current lines of the terminal to scrollback and to the
 * buffer.  Called after the job has ended and when switching to
 * Terminal-Normal mode.
 * When "redraw" is TRUE redraw the windows that show the terminal.
 */
    ಶೂನ್ಯ
may_move_terminal_ಗಮ್ಯ_ಬಫರ್(ಪದ_T *ಪದ, ಪೂರ್ಣಾಂಕ redraw)
{
    ಆದರೆ (ಪದ->tl_vterm == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    // Update the snapshot only if something changes or the buffer does not
    // have all the lines.
    ಆದರೆ (ಪದ->tl_dirty_snapshot || ಪದ->tl_ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ
					       <= ಪದ->tl_ಬಫರ್_scrolled)
	update_snapshot(ಪದ);

    ಆದರೆ (redraw)
    {
	win_T	    *wp = ಖಾಲಿ;
	ಪೂರ್ಣಾಂಕ	    did_curwin = FALSE;

	ಆಗ (for_all_windows_and_curwin(&wp, &did_curwin))
	{
	    ಆದರೆ (wp->w_ಬಫರ್ == ಪದ->tl_ಬಫರ್)
	    {
		wp->w_cursor.lnum = ಪದ->tl_ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
		wp->w_cursor.ಲಂಬಸಾಲು = 0;
		wp->w_valid = 0;
		ಆದರೆ (wp->w_cursor.lnum >= wp->w_ಎತ್ತರ)
		{
		    linenr_T min_topline = wp->w_cursor.lnum - wp->w_ಎತ್ತರ + 1;

		    ಆದರೆ (wp->w_topline < min_topline)
			wp->w_topline = min_topline;
		}
		redraw_win_later(wp, UPD_NOT_VALID);
	    }
	}
    }
}

#ಆದರೆ defined(FEAT_TIMERS)
/*
 * Check if any terminal timer expired.  If so, copy text from the terminal to
 * the buffer.
 * Return the time until the next timer will expire.
 */
    ಪೂರ್ಣಾಂಕ
ಪದ_check_timers(ಪೂರ್ಣಾಂಕ ಮುಂದಿನ_due_arg, proftime_T *now)
{
    ಪದ_T  *ಪದ;
    ಪೂರ್ಣಾಂಕ	    ಮುಂದಿನ_due = ಮುಂದಿನ_due_arg;

    FOR_ALL_TERMS(ಪದ)
    {
	ಆದರೆ (ಪದ->tl_timer_ಗಣ && !ಪದ->tl_ಸಾಮಾನ್ಯ_ಬಹುಲಕ)
	{
	    ದೀರ್ಘ    this_due = proftime_ಸಮಯ_ಎಡ(&ಪದ->tl_timer_due, now);

	    ಆದರೆ (this_due <= 1)
	    {
		ಪದ->tl_timer_ಗಣ = FALSE;
		may_move_terminal_ಗಮ್ಯ_ಬಫರ್(ಪದ, FALSE);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಮುಂದಿನ_due == -1 || ಮುಂದಿನ_due > this_due)
		ಮುಂದಿನ_due = this_due;
	}
    }

    ಹಿಂತಿರುಗಿ ಮುಂದಿನ_due;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * When "normal_mode" is TRUE set the terminal to Terminal-Normal mode,
 * otherwise end it.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಗಣ_terminal_ಬಹುಲಕ(ಪದ_T *ಪದ, ಪೂರ್ಣಾಂಕ ಸಾಮಾನ್ಯ_ಬಹುಲಕ)
{
    ಪದ->tl_ಸಾಮಾನ್ಯ_ಬಹುಲಕ = ಸಾಮಾನ್ಯ_ಬಹುಲಕ;
    may_trigger_modechanged();
    ಆದರೆ (!ಸಾಮಾನ್ಯ_ಬಹುಲಕ)
	handle_postponed_scrollback(ಪದ);
    VIM_CLEAR(ಪದ->tl_ಸ್ಥಿತಿ_ಪಠ್ಯ);
    ಆದರೆ (ಪದ->tl_ಬಫರ್ == curbuf)
	maketitle();
}

/*
 * Called after the job is finished and Terminal mode is not active:
 * Move the vterm contents into the scrollback buffer and free the vterm.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
cleanup_vterm(ಪದ_T *ಪದ)
{
    ಗಣ_terminal_ಬಹುಲಕ(ಪದ, FALSE);
    ಆದರೆ (ಪದ->tl_ಮುಕ್ತಾಯ != TL_ಮುಕ್ತಾಯ_CLOSE)
	may_move_terminal_ಗಮ್ಯ_ಬಫರ್(ಪದ, TRUE);
    ಪದ_free_vterm(ಪದ);
}

/*
 * Switch from Terminal-Job mode to Terminal-Normal mode.
 * Suspends updating the terminal window.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಪದ_enter_ಸಾಮಾನ್ಯ_ಬಹುಲಕ(ಶೂನ್ಯ)
{
    ಪದ_T     *ಪದ = curbuf->b_ಪದ;
    linenr_T   lnum;
    ಪೂರ್ಣಾಂಕ        ಲಂಬಸಾಲು;

    ಗಣ_terminal_ಬಹುಲಕ(ಪದ, TRUE);

    // Append the current terminal contents to the buffer.
    may_move_terminal_ಗಮ್ಯ_ಬಫರ್(ಪದ, TRUE);

    // Move the window cursor to the position of the cursor in the
    // terminal.
    lnum = ಪದ->tl_ಬಫರ್_scrolled + 1 + ಪದ->tl_cursor_pos.ಅಡ್ಡಸಾಲು;
    ಲಂಬಸಾಲು = ಪದ->tl_cursor_pos.ಲಂಬಸಾಲು;
    scrollbackline_pos_ಒಳ_buf(ಪದ, ಪದ->tl_cursor_pos.ಅಡ್ಡಸಾಲು + ಪದ->tl_scrollback_scrolled, &lnum, &ಲಂಬಸಾಲು, ಖಾಲಿ);

    curwin->w_cursor.lnum = lnum;
    check_cursor();
    ಆದರೆ (coladvance(ಲಂಬಸಾಲು) == FAIL)
	coladvance(MAXCOL);
    curwin->w_ಗಣ_curswant = true;
    curwin->w_topline = ಪದ->tl_ಬಫರ್_scrolled + 1;
}

/*
 * Returns TRUE if the current window contains a terminal and we are in
 * Terminal-Normal mode.
 */
    ಪೂರ್ಣಾಂಕ
ಪದ_ಒಳ_ಸಾಮಾನ್ಯ_ಬಹುಲಕ(buf_T *buf)
{
    ಪದ_T *ಪದ = buf->b_ಪದ;

    ಹಿಂತಿರುಗಿ ಪದ != ಖಾಲಿ && ಪದ->tl_ಸಾಮಾನ್ಯ_ಬಹುಲಕ;
}

/*
 * Switch from Terminal-Normal mode to Terminal-Job mode.
 * Restores updating the terminal window.
 */
    ಶೂನ್ಯ
ಪದ_enter_ಕೆಲಸ_ಬಹುಲಕ(ಶೂನ್ಯ)
{
    ಪದ_T	*ಪದ = curbuf->b_ಪದ;

    ಗಣ_terminal_ಬಹುಲಕ(ಪದ, FALSE);

    ಆದರೆ (ಪದ->tl_channel_closed)
	cleanup_vterm(ಪದ);
    redraw_buf_and_ಸ್ಥಿತಿ_later(curbuf, UPD_NOT_VALID);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
    ಆದರೆ (WIN_IS_POPUP(curwin))
	redraw_later(UPD_NOT_VALID);
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * When "modify_other_keys" is set then vgetc() should not reduce a key with
 * modifiers into a basic key.  However, we may only find out after calling
 * vgetc().  Therefore vgetorpeek() will call check_no_reduce_keys() to update
 * "no_reduce_keys" before using it.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
    NRKS_NONE,	    // initial value
    NRKS_CHECK,	    // modify_other_keys was off before calling vgetc()
    NRKS_ಗಣ,	    // no_reduce_keys was incremented in term_vgetc() or
		    // check_no_reduce_keys(), must be decremented.
} reduce_ಕೀಲಿ_state_T;

ಸ್ಥಿರ reduce_ಕೀಲಿ_state_T  no_reduce_ಕೀಲಿ_state = NRKS_NONE;

/*
 * Return TRUE if the term is using modifyOtherKeys level 2 or the kitty
 * keyboard protocol.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
vterm_using_ಕೀಲಿ_protocol(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ curbuf->b_ಪದ != ಖಾಲಿ
	&& curbuf->b_ಪದ->tl_vterm != ಖಾಲಿ
	&& (vterm_is_modify_other_ಕೀಲಿಗಳು(curbuf->b_ಪದ->tl_vterm)
		|| vterm_is_kitty_keyboard(curbuf->b_ಪದ->tl_vterm));
}

    ಶೂನ್ಯ
check_no_reduce_ಕೀಲಿಗಳು(ಶೂನ್ಯ)
{
    ಆದರೆ (no_reduce_ಕೀಲಿ_state != NRKS_CHECK
	    || no_reduce_ಕೀಲಿಗಳು >= 1
	    || curbuf->b_ಪದ == ಖಾಲಿ
	    || curbuf->b_ಪದ->tl_vterm == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (vterm_using_ಕೀಲಿ_protocol())
    {
	// "modify_other_keys" or kitty keyboard protocol was enabled while
	// waiting.
	no_reduce_ಕೀಲಿ_state = NRKS_ಗಣ;
	++no_reduce_ಕೀಲಿಗಳು;
    }
}

/*
 * Get a key from the user with terminal mode mappings.
 * Note: while waiting a terminal may be closed and freed if the channel is
 * closed and ++close was used.  This may even happen before we get here.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಪದ_vgetc(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ c;
    ಪೂರ್ಣಾಂಕ save_State = State;

    State = ಬಹುಲಕ_TERMINAL;
    got_int = FALSE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
    ctrl_break_was_pressed = FALSE;
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (vterm_using_ಕೀಲಿ_protocol())
    {
	++no_reduce_ಕೀಲಿಗಳು;
	no_reduce_ಕೀಲಿ_state = NRKS_ಗಣ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	no_reduce_ಕೀಲಿ_state = NRKS_CHECK;
    }

    c = vgetc();
    got_int = FALSE;
    State = save_State;

    ಆದರೆ (no_reduce_ಕೀಲಿ_state == NRKS_ಗಣ)
	--no_reduce_ಕೀಲಿಗಳು;
    no_reduce_ಕೀಲಿ_state = NRKS_NONE;

    ಹಿಂತಿರುಗಿ c;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	mouse_was_outside = FALSE;

/*
 * Send key "c" with modifiers "modmask" to terminal.
 * Return FAIL when the key needs to be handled in Normal mode.
 * Return OK when the key was dropped or sent to the terminal.
 */
    ಪೂರ್ಣಾಂಕ
send_ಕೀಲಿಗಳು_ಗಮ್ಯ_ಪದ(ಪದ_T *ಪದ, ಪೂರ್ಣಾಂಕ c, ಪೂರ್ಣಾಂಕ modmask, ಪೂರ್ಣಾಂಕ typed)
{
    ಅಕ್ಷರ	ಸಂದೇಶ[ಕೀಲಿ_BUF_LEN];
    size_t	len;
    ಪೂರ್ಣಾಂಕ		dragging_outside = FALSE;

    // Catch keys that need to be handled as in Normal mode.
    ಆಯ್ಕೆ (c)
    {
	ಸ್ಥಿತಿ NUL:
	ಸ್ಥಿತಿ K_ZERO:
	    ಆದರೆ (typed)
		stuffcharReadbuff(c);
	    ಹಿಂತಿರುಗಿ FAIL;

	ಸ್ಥಿತಿ K_TABLINE:
	    stuffcharReadbuff(c);
	    ಹಿಂತಿರುಗಿ FAIL;

	ಸ್ಥಿತಿ K_IGNORE:
	ಸ್ಥಿತಿ K_CANCEL:  // used for :normal when running out of chars
	    ಹಿಂತಿರುಗಿ FAIL;

	ಸ್ಥಿತಿ K_LEFTDRAG:
	ಸ್ಥಿತಿ K_MIDDLEDRAG:
	ಸ್ಥಿತಿ K_RIGHTDRAG:
	ಸ್ಥಿತಿ K_X1DRAG:
	ಸ್ಥಿತಿ K_X2DRAG:
	    dragging_outside = mouse_was_outside;
	    // FALLTHROUGH
	ಸ್ಥಿತಿ K_LEFTMOUSE:
	ಸ್ಥಿತಿ K_LEFTMOUSE_NM:
	ಸ್ಥಿತಿ K_LEFTRELEASE:
	ಸ್ಥಿತಿ K_LEFTRELEASE_NM:
	ಸ್ಥಿತಿ K_MOUSEMOVE:
	ಸ್ಥಿತಿ K_MIDDLEMOUSE:
	ಸ್ಥಿತಿ K_MIDDLERELEASE:
	ಸ್ಥಿತಿ K_RIGHTMOUSE:
	ಸ್ಥಿತಿ K_RIGHTRELEASE:
	ಸ್ಥಿತಿ K_X1MOUSE:
	ಸ್ಥಿತಿ K_X1RELEASE:
	ಸ್ಥಿತಿ K_X2MOUSE:
	ಸ್ಥಿತಿ K_X2RELEASE:

	ಸ್ಥಿತಿ K_MOUSEUP:
	ಸ್ಥಿತಿ K_MOUSEDOWN:
	ಸ್ಥಿತಿ K_MOUSELEFT:
	ಸ್ಥಿತಿ K_MOUSERIGHT:
	    {
		ಪೂರ್ಣಾಂಕ	ಅಡ್ಡಸಾಲು = mouse_ಅಡ್ಡಸಾಲು;
		ಪೂರ್ಣಾಂಕ	ಲಂಬಸಾಲು = mouse_ಲಂಬಸಾಲು;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
		ಆದರೆ (popup_is_popup(curwin))
		{
		    ಅಡ್ಡಸಾಲು -= popup_ತುದಿ_extra(curwin);
		    ಲಂಬಸಾಲು -= popup_ಎಡ_extra(curwin);
		}
#ಆದರೆ_ಅಂತ್ಯ
		ಆದರೆ (ಅಡ್ಡಸಾಲು < W_WINROW(curwin)
			|| ಅಡ್ಡಸಾಲು >= (W_WINROW(curwin) + curwin->w_ಎತ್ತರ)
			|| ಲಂಬಸಾಲು < curwin->w_wincol
			|| ಲಂಬಸಾಲು >= W_ENDCOL(curwin)
			|| dragging_outside)
		{
		    // click or scroll outside the current window or on status
		    // line or vertical separator
		    ಆದರೆ (typed)
		    {
			stuffcharReadbuff(c);
			mouse_was_outside = TRUE;
		    }
		    ಹಿಂತಿರುಗಿ FAIL;
		}
	    }
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ K_COMMAND:
	ಸ್ಥಿತಿ K_SCRIPT_COMMAND:
	    ಹಿಂತಿರುಗಿ do_cmdkey_command(c, 0);
    }
    ಆದರೆ (typed)
	mouse_was_outside = FALSE;

    // Convert the typed key to a sequence of bytes for the job.
    len = ಪದ_convert_ಕೀಲಿ(ಪದ, c, modmask, ಸಂದೇಶ);
    ಆದರೆ (len > 0)
	// TODO: if FAIL is returned, stop?
	channel_send(ಪದ->tl_ಕೆಲಸ->jv_channel, get_tty_part(ಪದ),
						(char_u *)ಸಂದೇಶ, (ಪೂರ್ಣಾಂಕ)len, ಖಾಲಿ);

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Handle CTRL-W "": send register contents to the job.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಪದ_paste_ರಿಜಿಸ್ಟರ್(ಪೂರ್ಣಾಂಕ prev_c ಬಳಸದ)
{
    ಪೂರ್ಣಾಂಕ		c;
    list_T	*l;
    listitem_T	*ಅಂಶ;
    ದೀರ್ಘ	reglen = 0;
    ಪೂರ್ಣಾಂಕ		type;

    ಆದರೆ (ಕೂಡಿಸು_ಗಮ್ಯ_showcmd(prev_c))
    ಆದರೆ (ಕೂಡಿಸು_ಗಮ್ಯ_showcmd('"'))
	ಹೊರಹೋಗು_flush();

    c = ಪದ_vgetc();
    clear_showcmd();

    ಆದರೆ (!ಪದ_use_loop())
	// job finished while waiting for a character
	ಹಿಂತಿರುಗಿ;

    // CTRL-W "= prompt for expression to evaluate.
    ಆದರೆ (c == '=' && get_ಅಭಿವ್ಯಕ್ತಿ_ರಿಜಿಸ್ಟರ್() != '=')
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (!ಪದ_use_loop())
	// job finished while waiting for a character
	ಹಿಂತಿರುಗಿ;

    l = (list_T *)get_reg_contents(c, GREG_LIST);
    ಆದರೆ (l == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    type = get_reg_type(c, &reglen);
    FOR_ALL_LIST_ವಸ್ತುಗಳು(l, ಅಂಶ)
    {
	char_u *s = tv_get_string(&ಅಂಶ->li_tv);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
	char_u *tmp = s;

	ಆದರೆ (!enc_utf8 && enc_codepage > 0)
	{
	    WCHAR   *ret = ಖಾಲಿ;
	    ಪೂರ್ಣಾಂಕ	ಉದ್ದಳತೆ = 0;

	    Multi_Byte_ಗಮ್ಯ_Wide_Char_alloc(enc_codepage, 0, (ಅಕ್ಷರ *)s,
		    (ಪೂರ್ಣಾಂಕ)STRLEN(s), &ret, &ಉದ್ದಳತೆ);
	    ಆದರೆ (ret != ಖಾಲಿ)
	    {
		Wide_Char_ಗಮ್ಯ_Multi_Byte_alloc(CP_UTF8, 0,
			ret, ಉದ್ದಳತೆ, (ಅಕ್ಷರ **)&s, &ಉದ್ದಳತೆ, 0, 0);
		vim_free(ret);
	    }
	}
#ಆದರೆ_ಅಂತ್ಯ
	channel_send(curbuf->b_ಪದ->tl_ಕೆಲಸ->jv_channel, PART_ಒಳ,
		s, (ಪೂರ್ಣಾಂಕ)STRLEN(s), ಖಾಲಿ);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
	ಆದರೆ (tmp != s)
	    vim_free(s);
#ಆದರೆ_ಅಂತ್ಯ

	ಆದರೆ (ಅಂಶ->li_ಮುಂದಿನ != ಖಾಲಿ || type == MLINE)
	    channel_send(curbuf->b_ಪದ->tl_ಕೆಲಸ->jv_channel, PART_ಒಳ,
		    (char_u *)"\r", 1, ಖಾಲಿ);
    }
    list_free(l);
}

/*
 * Return TRUE when waiting for a character in the terminal, the cursor of the
 * terminal should be displayed.
 */
    ಪೂರ್ಣಾಂಕ
terminal_is_ಸಕ್ರಿಯ(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ ಒಳ_terminal_loop != ಖಾಲಿ;
}

/*
 * Return the highlight group ID for the terminal and the window.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಪದ_get_highlight_id(ಪದ_T *ಪದ, win_T *wp)
{
    char_u *name;

    ಆದರೆ (wp != ಖಾಲಿ && wp->w_hlfwin_id != 0)
	name = syn_id2name(wp->w_hlfwin_id);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಪದ->tl_highlight_name != ಖಾಲಿ)
	name = ಪದ->tl_highlight_name;
    ಇಲ್ಲದಿದ್ದರೆ
	name = (char_u*)"Terminal";

    ಹಿಂತಿರುಗಿ syn_name2id(name);
}

#ಆದರೆ defined(FEAT_GUI)
    cursorentry_T *
ಪದ_get_cursor_shape(guicolor_T *fg, guicolor_T *bg)
{
    ಪದ_T		 *ಪದ = ಒಳ_terminal_loop;
    ಸ್ಥಿರ cursorentry_T ನಮೂದು;
    ಪೂರ್ಣಾಂಕ			 id;
    guicolor_T		 ಪದ_fg = INVALCOLOR;
    guicolor_T		 ಪದ_bg = INVALCOLOR;

    CLEAR_ಕ್ಷೇತ್ರ(ನಮೂದು);
    ನಮೂದು.shape = ನಮೂದು.mshape =
	ಪದ->tl_cursor_shape == VTERM_PROP_CURSORSHAPE_UNDERLINE ? SHAPE_HOR :
	ಪದ->tl_cursor_shape == VTERM_PROP_CURSORSHAPE_BAR_ಎಡ ? SHAPE_VER :
	SHAPE_ಖಂಡ;
    ನಮೂದು.percentage = 20;
    ಆದರೆ (ಪದ->tl_cursor_blink)
    {
	ನಮೂದು.blinkwait = 700;
	ನಮೂದು.blinkon = 400;
	ನಮೂದು.blinkoff = 250;
    }

    // The highlight group overrules the defaults.
    id = ಪದ_get_highlight_id(ಪದ, curwin);
    ಆದರೆ (id != 0)
	syn_id2colors(id, &ಪದ_fg, &ಪದ_bg);
    ಆದರೆ (ಪದ_bg != INVALCOLOR)
	*fg = ಪದ_bg;
    ಇಲ್ಲದಿದ್ದರೆ
	*fg = gui.ಹಿಂಭಾಗ_pixel;

    ಆದರೆ (ಪದ->tl_cursor_color == ಖಾಲಿ)
    {
	ಆದರೆ (ಪದ_fg != INVALCOLOR)
	    *bg = ಪದ_fg;
	ಇಲ್ಲದಿದ್ದರೆ
	    *bg = gui.norm_pixel;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	*bg = color_name2handle(ಪದ->tl_cursor_color);
    ನಮೂದು.name = "n";
    ನಮೂದು.ಬಳಸಿದ_for = SHAPE_CURSOR;

    ಹಿಂತಿರುಗಿ &ನಮೂದು;
}
#ಆದರೆ_ಅಂತ್ಯ

    ಸ್ಥಿರ ಶೂನ್ಯ
may_output_cursor_props(ಶೂನ್ಯ)
{
    ಆದರೆ (!cursor_color_equal(ಕೊನೆಯ_ಗಣ_cursor_color, desired_cursor_color)
	    || ಕೊನೆಯ_ಗಣ_cursor_shape != desired_cursor_shape
	    || ಕೊನೆಯ_ಗಣ_cursor_blink != desired_cursor_blink)
    {
	cursor_color_copy(&ಕೊನೆಯ_ಗಣ_cursor_color, desired_cursor_color);
	ಕೊನೆಯ_ಗಣ_cursor_shape = desired_cursor_shape;
	ಕೊನೆಯ_ಗಣ_cursor_blink = desired_cursor_blink;
	ಪದ_cursor_color(cursor_color_get(desired_cursor_color));
	ಆದರೆ (desired_cursor_shape == -1 || desired_cursor_blink == -1)
	    // this will restore the initial cursor style, if possible
	    ui_cursor_shape_forced(TRUE);
	ಇಲ್ಲದಿದ್ದರೆ
	    ಪದ_cursor_shape(desired_cursor_shape, desired_cursor_blink);
    }
}

/*
 * Set the cursor color and shape, if not last set to these.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
may_ಗಣ_cursor_props(ಪದ_T *ಪದ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    // For the GUI the cursor properties are obtained with
    // term_get_cursor_shape().
    ಆದರೆ (gui.ಒಳ_use)
	ಹಿಂತಿರುಗಿ;
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (ಒಳ_terminal_loop == ಪದ)
    {
	cursor_color_copy(&desired_cursor_color, ಪದ->tl_cursor_color);
	desired_cursor_shape = ಪದ->tl_cursor_shape;
	desired_cursor_blink = ಪದ->tl_cursor_blink;
	may_output_cursor_props();
    }
}

/*
 * Reset the desired cursor properties and restore them when needed.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
prepare_restore_cursor_props(ಶೂನ್ಯ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    ಆದರೆ (gui.ಒಳ_use)
	ಹಿಂತಿರುಗಿ;
#ಆದರೆ_ಅಂತ್ಯ
    cursor_color_copy(&desired_cursor_color, ಖಾಲಿ);
    desired_cursor_shape = -1;
    desired_cursor_blink = -1;
    may_output_cursor_props();
}

/*
 * Returns TRUE if the current window contains a terminal and we are sending
 * keys to the job.
 * If "check_job_status" is TRUE update the job status.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಪದ_use_loop_check(ಪೂರ್ಣಾಂಕ check_ಕೆಲಸ_ಸ್ಥಿತಿ)
{
    ಪದ_T *ಪದ = curbuf->b_ಪದ;

    ಹಿಂತಿರುಗಿ ಪದ != ಖಾಲಿ
	&& !ಪದ->tl_ಸಾಮಾನ್ಯ_ಬಹುಲಕ
	&& ಪದ->tl_vterm != ಖಾಲಿ
	&& ಪದ_ಕೆಲಸ_running_check(ಪದ, check_ಕೆಲಸ_ಸ್ಥಿತಿ);
}

/*
 * Returns TRUE if the current window contains a terminal and we are sending
 * keys to the job.
 */
    ಪೂರ್ಣಾಂಕ
ಪದ_use_loop(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ ಪದ_use_loop_check(FALSE);
}

/*
 * Called when entering a window with the mouse.  If this is a terminal window
 * we may want to change state.
 */
    ಶೂನ್ಯ
ಪದ_win_entered(ಶೂನ್ಯ)
{
    ಪದ_T *ಪದ = curbuf->b_ಪದ;

    ಆದರೆ (ಪದ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (ಪದ_use_loop_check(TRUE))
    {
	reset_VIsual_and_resel();
	ಆದರೆ (State & ಬಹುಲಕ_ಸೇರಿಸು)
	    stop_ಸೇರಿಸು_ಬಹುಲಕ = TRUE;
    }
    mouse_was_outside = FALSE;
    enter_mouse_ಲಂಬಸಾಲು = mouse_ಲಂಬಸಾಲು;
    enter_mouse_ಅಡ್ಡಸಾಲು = mouse_ಅಡ್ಡಸಾಲು;
}

    ಶೂನ್ಯ
ಪದ_focus_change(ಪೂರ್ಣಾಂಕ ಒಳ_focus)
{
    ಪದ_T *ಪದ = curbuf->b_ಪದ;

    ಆದರೆ (ಪದ == ಖಾಲಿ || ಪದ->tl_vterm == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    VTermState	*state = vterm_obtain_state(ಪದ->tl_vterm);

    ಆದರೆ (ಒಳ_focus)
	vterm_state_focus_ಒಳ(state);
    ಇಲ್ಲದಿದ್ದರೆ
	vterm_state_focus_ಹೊರಹೋಗು(state);
    ಪದ_forward_output(ಪದ);
}

/*
 * vgetc() may not include CTRL in the key when modify_other_keys is set.
 * Return the Ctrl-key value in that case.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
raw_c_ಗಮ್ಯ_ctrl(ಪೂರ್ಣಾಂಕ c)
{
    ಆದರೆ ((mod_ಮುಖವಾಡ & MOD_ಮುಖವಾಡ_CTRL)
	    && ((c >= '`' && c <= 0x7f) || (c >= '@' && c <= '_')))
	ಹಿಂತಿರುಗಿ c & 0x1f;
    ಹಿಂತಿರುಗಿ c;
}

/*
 * When modify_other_keys is set then do the reverse of raw_c_to_ctrl().
 * Also when the Kitty keyboard protocol is used.
 * May set "mod_mask".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ctrl_ಗಮ್ಯ_raw_c(ಪೂರ್ಣಾಂಕ c)
{
    ಆದರೆ (c < 0x20 && vterm_using_ಕೀಲಿ_protocol())
    {
	mod_ಮುಖವಾಡ |= MOD_ಮುಖವಾಡ_CTRL;
	ಹಿಂತಿರುಗಿ c + '@';
    }
    ಹಿಂತಿರುಗಿ c;
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
    ಪೂರ್ಣಾಂಕ
terminal_loop(ಪೂರ್ಣಾಂಕ blocking)
{
    ಪೂರ್ಣಾಂಕ		c;
    ಪೂರ್ಣಾಂಕ		raw_c;
    ಪೂರ್ಣಾಂಕ		termwinkey = 0;
    ಪೂರ್ಣಾಂಕ		ret;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
    ಪೂರ್ಣಾಂಕ		tty_fd = curbuf->b_ಪದ->tl_ಕೆಲಸ->jv_channel
				 ->ch_part[get_tty_part(curbuf->b_ಪದ)].ch_fd;
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		restore_cursor = FALSE;

    // Remember the terminal we are sending keys to.  However, the terminal
    // might be closed while waiting for a character, e.g. typing "exit" in a
    // shell and ++close was used.  Therefore use curbuf->b_term instead of a
    // stored reference.
    ಒಳ_terminal_loop = curbuf->b_ಪದ;

    ಆದರೆ (*curwin->w_p_twk != NUL)
    {
	termwinkey = string_ಗಮ್ಯ_ಕೀಲಿ(curwin->w_p_twk, TRUE);
	ಆದರೆ (termwinkey == Ctrl_W)
	    termwinkey = 0;
    }
    position_cursor(curwin, &curbuf->b_ಪದ->tl_cursor_pos);
    may_ಗಣ_cursor_props(curbuf->b_ಪದ);

    ಆಗ (blocking || vpeekc_nomap() != NUL)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
	ಆದರೆ (curbuf->b_ಪದ != ಖಾಲಿ && !curbuf->b_ಪದ->tl_system)
#ಆದರೆ_ಅಂತ್ಯ
	    // TODO: skip screen update when handling a sequence of keys.
	    // Repeat redrawing in case a message is received while redrawing.
	    ಆಗ (must_redraw != 0)
		ಆದರೆ (update_screen(0) == FAIL)
		    ನಿಲ್ಲಿಸು;
	ಆದರೆ (!ಪದ_use_loop_check(TRUE) || ಒಳ_terminal_loop != curbuf->b_ಪದ)
	    // job finished while redrawing
	    ನಿಲ್ಲಿಸು;

	update_cursor(curbuf->b_ಪದ, FALSE);
	restore_cursor = TRUE;

	raw_c = ಪದ_vgetc();
	ಆದರೆ (!ಪದ_use_loop_check(TRUE) || ಒಳ_terminal_loop != curbuf->b_ಪದ)
	{
	    // Job finished while waiting for a character.  Push back the
	    // received character.
	    ಆದರೆ (raw_c != K_IGNORE)
		vungetc(raw_c);
	    ನಿಲ್ಲಿಸು;
	}
	ಆದರೆ (raw_c == K_IGNORE)
	    ಮುಂದುವರಿ;
	c = raw_c_ಗಮ್ಯ_ctrl(raw_c);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
	/*
	 * The shell or another program may change the tty settings.  Getting
	 * them for every typed character is a bit of overhead, but it's needed
	 * for the first character typed, e.g. when Vim starts in a shell.
	 */
	ಆದರೆ (mch_isatty(tty_fd))
	{
	    ttyinfo_T info;

	    // Get the current backspace character of the pty.
	    ಆದರೆ (get_tty_info(tty_fd, &info) == OK)
		ಪದ_backspace_char = info.backspace;
	}
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
	// On Windows winpty handles CTRL-C, don't send a CTRL_C_EVENT.
	// Use CTRL-BREAK to kill the job.
	ಆದರೆ (ctrl_break_was_pressed)
	    mch_signal_ಕೆಲಸ(curbuf->b_ಪದ->tl_ಕೆಲಸ, (char_u *)"kill");
#ಆದರೆ_ಅಂತ್ಯ
	// Was either CTRL-W (termwinkey) or CTRL-\ pressed?
	// Not in a system terminal.
	ಆದರೆ ((c == (termwinkey == 0 ? Ctrl_W : termwinkey) || c == Ctrl_BSL)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
		&& !curbuf->b_ಪದ->tl_system
#ಆದರೆ_ಅಂತ್ಯ
		)
	{
	    ಪೂರ್ಣಾಂಕ	    prev_c = c;
	    ಪೂರ್ಣಾಂಕ	    prev_raw_c = raw_c;
	    ಪೂರ್ಣಾಂಕ	    prev_mod_ಮುಖವಾಡ = mod_ಮುಖವಾಡ;

	    ಆದರೆ (ಕೂಡಿಸು_ಗಮ್ಯ_showcmd(c))
		ಹೊರಹೋಗು_flush();

	    raw_c = ಪದ_vgetc();
	    c = raw_c_ಗಮ್ಯ_ctrl(raw_c);

	    clear_showcmd();

	    ಆದರೆ (!ಪದ_use_loop_check(TRUE)
					 || ಒಳ_terminal_loop != curbuf->b_ಪದ)
		// job finished while waiting for a character
		ನಿಲ್ಲಿಸು;

	    ಆದರೆ (prev_c == Ctrl_BSL)
	    {
		ಆದರೆ (c == Ctrl_N)
		{
		    // CTRL-\ CTRL-N : go to Terminal-Normal mode.
		    ಪದ_enter_ಸಾಮಾನ್ಯ_ಬಹುಲಕ();
		    ret = FAIL;
		    ಹೋಗು theend;
		}
		// Send both keys to the terminal, first one here, second one
		// below.
		send_ಕೀಲಿಗಳು_ಗಮ್ಯ_ಪದ(curbuf->b_ಪದ, prev_raw_c, prev_mod_ಮುಖವಾಡ,
									 TRUE);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == Ctrl_C)
	    {
		// "CTRL-W CTRL-C" or 'termwinkey' CTRL-C: end the job
		mch_signal_ಕೆಲಸ(curbuf->b_ಪದ->tl_ಕೆಲಸ, (char_u *)"kill");
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == '.')
	    {
		// "CTRL-W .": send CTRL-W to the job
		// "'termwinkey' .": send 'termwinkey' to the job
		raw_c = ctrl_ಗಮ್ಯ_raw_c(termwinkey == 0 ? Ctrl_W : termwinkey);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == Ctrl_BSL)
	    {
		// "CTRL-W CTRL-\": send CTRL-\ to the job
		raw_c = ctrl_ಗಮ್ಯ_raw_c(Ctrl_BSL);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == 'N')
	    {
		// CTRL-W N : go to Terminal-Normal mode.
		ಪದ_enter_ಸಾಮಾನ್ಯ_ಬಹುಲಕ();
		ret = FAIL;
		ಹೋಗು theend;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == '"')
	    {
		ಪದ_paste_ರಿಜಿಸ್ಟರ್(prev_c);
		ಮುಂದುವರಿ;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (termwinkey == 0 || c != termwinkey)
	    {
		// space for CTRL-W, modifier, multi-byte char and NUL
		char_u buf[1 + 3 + MB_MAXBYTES + 1];

		// Put the command into the typeahead buffer, when using the
		// stuff buffer KeyStuffed is set and 'langmap' won't be used.
		buf[0] = Ctrl_W;
		buf[special_ಗಮ್ಯ_buf(c, mod_ಮುಖವಾಡ, FALSE, buf + 1) + 1] = NUL;
		ins_typebuf(buf, REMAP_NONE, 0, TRUE, FALSE);
		ret = OK;
		ಹೋಗು theend;
	    }
	}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
	ಆದರೆ (!enc_utf8 && has_mbyte && raw_c >= 0x80)
	{
	    WCHAR   wc;
	    char_u  mb[3];

	    mb[0] = (ಚಿಹ್ನೆರಹಿತ)raw_c >> 8;
	    mb[1] = raw_c;
	    ಆದರೆ (Multi_Byte_ಗಮ್ಯ_Wide_Char(GetACP(), 0, (ಅಕ್ಷರ*)mb, 2, &wc, 1) > 0)
		raw_c = wc;
	}
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ (send_ಕೀಲಿಗಳು_ಗಮ್ಯ_ಪದ(curbuf->b_ಪದ, raw_c, mod_ಮುಖವಾಡ, TRUE) != OK)
	{
	    ಆದರೆ (raw_c == K_MOUSEMOVE)
		// We are sure to come back here, don't reset the cursor color
		// and shape to avoid flickering.
		restore_cursor = FALSE;

	    ret = OK;
	    ಹೋಗು theend;
	}
    }
    ret = FAIL;

theend:
    ಒಳ_terminal_loop = ಖಾಲಿ;
    ಆದರೆ (restore_cursor)
	prepare_restore_cursor_props();

    // Move a snapshot of the screen contents to the buffer, so that completion
    // works in other buffers.
    ಆದರೆ (curbuf->b_ಪದ != ಖಾಲಿ && !curbuf->b_ಪದ->tl_ಸಾಮಾನ್ಯ_ಬಹುಲಕ)
	may_move_terminal_ಗಮ್ಯ_ಬಫರ್(curbuf->b_ಪದ, FALSE);

    ಹಿಂತಿರುಗಿ ret;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
may_toggle_cursor(ಪದ_T *ಪದ)
{
    ಆದರೆ (ಒಳ_terminal_loop != ಪದ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (ಪದ->tl_cursor_visible)
	cursor_on();
    ಇಲ್ಲದಿದ್ದರೆ
	cursor_off();
}

/*
 * Reverse engineer the RGB value into a cterm color index.
 * First color is 1.  Return 0 if no match found (default color).
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
color2index(VTermColor *color, ಪೂರ್ಣಾಂಕ fg, ಪೂರ್ಣಾಂಕ *boldp)
{
    ಪೂರ್ಣಾಂಕ red = color->red;
    ಪೂರ್ಣಾಂಕ blue = color->blue;
    ಪೂರ್ಣಾಂಕ green = color->green;

    *boldp = FALSE;

    ಆದರೆ (VTERM_COLOR_IS_INVALID(color))
	ಹಿಂತಿರುಗಿ 0;

    ಆದರೆ (VTERM_COLOR_IS_ಸೂಚ್ಯಂಕಿತ(color))
    {
	// Use the color as-is if possible, give up otherwise.
	ಆದರೆ (color->ಸೂಚ್ಯಂಕ < t_colors)
	{
	    uint8_t ಸೂಚ್ಯಂಕ = color->ಸೂಚ್ಯಂಕ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
	    // Convert ANSI palette to cterm color index.
	    // cterm_ansi_idx is a table for the reverse conversion from cterm
	    // color index to ANSI palette. However, the current table can
	    // be used for this conversion as well.
	    // Note: If the contents of cterm_ansi_idx change in the future, a
	    // different table may be needed for this purpose.
	    ಆದರೆ (ಸೂಚ್ಯಂಕ < 16)
		ಸೂಚ್ಯಂಕ = cterm_ansi_idx[ಸೂಚ್ಯಂಕ];
#ಆದರೆ_ಅಂತ್ಯ
	    ಹಿಂತಿರುಗಿ ಸೂಚ್ಯಂಕ + 1;
	}
	// 8-color terminals can actually display twice as many colors by
	// setting the high-intensity/bold bit.
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (t_colors == 8 && fg && color->ಸೂಚ್ಯಂಕ < 16)
	{
	    *boldp = TRUE;
	    ಹಿಂತಿರುಗಿ (color->ಸೂಚ್ಯಂಕ & 7) + 1;
	}
	ಹಿಂತಿರುಗಿ 0;
    }

    ಆದರೆ (t_colors >= 256)
    {
	ಆದರೆ (red == blue && red == green)
	{
	    // 24-color greyscale plus white and black
	    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ cutoff[23] = {
		    0x0D, 0x17, 0x21, 0x2B, 0x35, 0x3F, 0x49, 0x53, 0x5D, 0x67,
		    0x71, 0x7B, 0x85, 0x8F, 0x99, 0xA3, 0xAD, 0xB7, 0xC1, 0xCB,
		    0xD5, 0xDF, 0xE9};
	    ಪೂರ್ಣಾಂಕ i;

	    ಆದರೆ (red < 5)
		ಹಿಂತಿರುಗಿ 17; // 00/00/00
	    ಆದರೆ (red > 245) // ff/ff/ff
		ಹಿಂತಿರುಗಿ 232;
	    ಗಾಗಿ (i = 0; i < 23; ++i)
		ಆದರೆ (red < cutoff[i])
		    ಹಿಂತಿರುಗಿ i + 233;
	    ಹಿಂತಿರುಗಿ 256;
	}
	{
	    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ cutoff[5] = {0x2F, 0x73, 0x9B, 0xC3, 0xEB};
	    ಪೂರ್ಣಾಂಕ ri, gi, bi;

	    // 216-color cube
	    ಗಾಗಿ (ri = 0; ri < 5; ++ri)
		ಆದರೆ (red < cutoff[ri])
		    ನಿಲ್ಲಿಸು;
	    ಗಾಗಿ (gi = 0; gi < 5; ++gi)
		ಆದರೆ (green < cutoff[gi])
		    ನಿಲ್ಲಿಸು;
	    ಗಾಗಿ (bi = 0; bi < 5; ++bi)
		ಆದರೆ (blue < cutoff[bi])
		    ನಿಲ್ಲಿಸು;
	    ಹಿಂತಿರುಗಿ 17 + ri * 36 + gi * 6 + bi;
	}
    }
    ಹಿಂತಿರುಗಿ 0;
}

/*
 * Convert Vterm attributes to highlight flags.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
vtermAttr2hl(VTermScreenCellAttrs *cellattrs)
{
    ಪೂರ್ಣಾಂಕ attr = 0;

    ಆದರೆ (cellattrs->bold)
	attr |= HL_BOLD;
    ಆದರೆ (cellattrs->underline)
	attr |= HL_UNDERLINE;
    ಆದರೆ (cellattrs->italic)
	attr |= HL_ITALIC;
    ಆದರೆ (cellattrs->strike)
	attr |= HL_STRIKETHROUGH;
    ಆದರೆ (cellattrs->ಹಿಮ್ಮುಖ)
	attr |= HL_INVERSE;
    ಹಿಂತಿರುಗಿ attr;
}

/*
 * Store Vterm attributes in "cell" from highlight flags.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
hl2vtermAttr(ಪೂರ್ಣಾಂಕ attr, cellattr_T *cell)
{
    CLEAR_ಕ್ಷೇತ್ರ(cell->attrs);
    ಆದರೆ (attr & HL_BOLD)
	cell->attrs.bold = 1;
    ಆದರೆ (attr & HL_UNDERLINE)
	cell->attrs.underline = 1;
    ಆದರೆ (attr & HL_ITALIC)
	cell->attrs.italic = 1;
    ಆದರೆ (attr & HL_STRIKETHROUGH)
	cell->attrs.strike = 1;
    ಆದರೆ (attr & HL_INVERSE)
	cell->attrs.ಹಿಮ್ಮುಖ = 1;
}

/*
 * Convert the attributes of a vterm cell into an attribute index.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
cell2attr(
	ಪದ_T			*ಪದ,
	win_T			*wp,
	VTermScreenCellAttrs	*cellattrs,
	VTermColor		*cellfg,
	VTermColor		*cellbg)
{
    ಪೂರ್ಣಾಂಕ attr = vtermAttr2hl(cellattrs);
    VTermColor *fg = cellfg;
    VTermColor *bg = cellbg;
    ಪೂರ್ಣಾಂಕ is_default_fg = VTERM_COLOR_IS_DEFAULT_FG(fg);
    ಪೂರ್ಣಾಂಕ is_default_bg = VTERM_COLOR_IS_DEFAULT_BG(bg);

    ಆದರೆ (is_default_fg || is_default_bg)
    {
	ಆದರೆ (wp != ಖಾಲಿ && wp->w_hlfwin_id != 0)
	{
	    ಆದರೆ (is_default_fg)
		fg = &wp->w_ಪದ_hlfwin.fg;
	    ಆದರೆ (is_default_bg)
		bg = &wp->w_ಪದ_hlfwin.bg;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (is_default_fg)
		fg = &ಪದ->tl_default_color.fg;
	    ಆದರೆ (is_default_bg)
		bg = &ಪದ->tl_default_color.bg;
	}
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    ಆದರೆ (gui.ಒಳ_use)
    {
	guicolor_T guifg = gui_mch_get_rgb_color(fg->red, fg->green, fg->blue);
	guicolor_T guibg = gui_mch_get_rgb_color(bg->red, bg->green, bg->blue);
	ಹಿಂತಿರುಗಿ get_gui_attr_idx(attr, guifg, guibg);
    }
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMGUICOLORS
    ಆದರೆ (p_tgc)
    {
	guicolor_T tgcfg = VTERM_COLOR_IS_INVALID(fg)
	    ? INVALCOLOR
	    : gui_get_rgb_color_cmn(fg->red, fg->green, fg->blue);
	guicolor_T tgcbg = VTERM_COLOR_IS_INVALID(bg)
	    ? INVALCOLOR
	    : gui_get_rgb_color_cmn(bg->red, bg->green, bg->blue);
	ಹಿಂತಿರುಗಿ get_tgc_attr_idx(attr, tgcfg, tgcbg);
    }
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
    {
	ಪೂರ್ಣಾಂಕ bold = MAYBE;
	ಪೂರ್ಣಾಂಕ ctermfg = color2index(fg, TRUE, &bold);
	ಪೂರ್ಣಾಂಕ ctermbg = color2index(bg, FALSE, &bold);

	// with 8 colors set the bold attribute to get a bright foreground
	ಆದರೆ (bold == TRUE)
	    attr |= HL_BOLD;

	ಹಿಂತಿರುಗಿ get_cterm_attr_idx(attr, ctermfg, ctermbg);
    }
    ಹಿಂತಿರುಗಿ 0;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ಗಣ_dirty_snapshot(ಪದ_T *ಪದ)
{
    ಪದ->tl_dirty_snapshot = TRUE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TIMERS
    ಆದರೆ (!ಪದ->tl_ಸಾಮಾನ್ಯ_ಬಹುಲಕ)
    {
	// Update the snapshot after 100 msec of not getting updates.
	profile_setlimit(100L, &ಪದ->tl_timer_due);
	ಪದ->tl_timer_ಗಣ = TRUE;
    }
#ಆದರೆ_ಅಂತ್ಯ
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
handle_damage(VTermRect rect, ಶೂನ್ಯ *user)
{
    ಪದ_T *ಪದ = (ಪದ_T *)user;

    ಪದ->tl_dirty_ಅಡ್ಡಸಾಲು_ಆರಂಭ = MIN(ಪದ->tl_dirty_ಅಡ್ಡಸಾಲು_ಆರಂಭ, rect.ಆರಂಭ_ಅಡ್ಡಸಾಲು);
    ಪದ->tl_dirty_ಅಡ್ಡಸಾಲು_ಕೊನೆ = MAX(ಪದ->tl_dirty_ಅಡ್ಡಸಾಲು_ಕೊನೆ, rect.ಕೊನೆ_ಅಡ್ಡಸಾಲು);
    ಗಣ_dirty_snapshot(ಪದ);
    redraw_buf_later(ಪದ->tl_ಬಫರ್, UPD_SOME_VALID);
    ಹಿಂತಿರುಗಿ 1;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ಪದ_scroll_ಮೇಲೆ(ಪದ_T *ಪದ, ಪೂರ್ಣಾಂಕ ಆರಂಭ_ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2)
{
    win_T		 *wp = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ			 did_curwin = FALSE;
    VTermColor		 fg, bg;
    VTermScreenCellAttrs attr;
    ಪೂರ್ಣಾಂಕ			 clear_attr;

    CLEAR_ಕ್ಷೇತ್ರ(attr);

    ಆಗ (for_all_windows_and_curwin(&wp, &did_curwin))
    {
	ಆದರೆ (wp->w_ಬಫರ್ == ಪದ->tl_ಬಫರ್)
	{
	    // Set the color to clear lines with.
	    vterm_state_get_default_colors(vterm_obtain_state(ಪದ->tl_vterm),
								     &fg, &bg);
	    clear_attr = cell2attr(ಪದ, wp, &attr, &fg, &bg);
	    win_del_ಸಾಲುಗಳು(wp, ಆರಂಭ_ಅಡ್ಡಸಾಲು, ಎಣಿಕೆ_2, FALSE, FALSE, clear_attr);
	}
    }
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
handle_moverect(VTermRect dest, VTermRect src, ಶೂನ್ಯ *user)
{
    ಪದ_T	*ಪದ = (ಪದ_T *)user;
    ಪೂರ್ಣಾಂಕ		ಎಣಿಕೆ_2 = src.ಆರಂಭ_ಅಡ್ಡಸಾಲು - dest.ಆರಂಭ_ಅಡ್ಡಸಾಲು;

    // Scrolling up is done much more efficiently by deleting lines instead of
    // redrawing the text. But avoid doing this multiple times, postpone until
    // the redraw happens.
    ಆದರೆ (dest.ಆರಂಭ_ಲಂಬಸಾಲು == src.ಆರಂಭ_ಲಂಬಸಾಲು
	    && dest.ಕೊನೆ_ಲಂಬಸಾಲು == src.ಕೊನೆ_ಲಂಬಸಾಲು
	    && dest.ಆರಂಭ_ಅಡ್ಡಸಾಲು < src.ಆರಂಭ_ಅಡ್ಡಸಾಲು)
    {
	ಆದರೆ (dest.ಆರಂಭ_ಅಡ್ಡಸಾಲು == 0)
	    ಪದ->tl_postponed_scroll += ಎಣಿಕೆ_2;
	ಇಲ್ಲದಿದ್ದರೆ
	    ಪದ_scroll_ಮೇಲೆ(ಪದ, dest.ಆರಂಭ_ಅಡ್ಡಸಾಲು, ಎಣಿಕೆ_2);
    }

    ಪದ->tl_dirty_ಅಡ್ಡಸಾಲು_ಆರಂಭ = MIN(ಪದ->tl_dirty_ಅಡ್ಡಸಾಲು_ಆರಂಭ, dest.ಆರಂಭ_ಅಡ್ಡಸಾಲು);
    ಪದ->tl_dirty_ಅಡ್ಡಸಾಲು_ಕೊನೆ = MIN(ಪದ->tl_dirty_ಅಡ್ಡಸಾಲು_ಕೊನೆ, dest.ಕೊನೆ_ಅಡ್ಡಸಾಲು);
    ಗಣ_dirty_snapshot(ಪದ);

    // Note sure if the scrolling will work correctly, let's do a complete
    // redraw later.
    redraw_buf_later(ಪದ->tl_ಬಫರ್, UPD_NOT_VALID);
    ಹಿಂತಿರುಗಿ 1;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
handle_movecursor(
	VTermPos pos,
	VTermPos oldpos ಬಳಸದ,
	ಪೂರ್ಣಾಂಕ visible,
	ಶೂನ್ಯ *user)
{
    ಪದ_T	*ಪದ = (ಪದ_T *)user;
    win_T	*wp = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		did_curwin = FALSE;

    ಪದ->tl_cursor_pos = pos;
    ಪದ->tl_cursor_visible = visible;

    ಆಗ (for_all_windows_and_curwin(&wp, &did_curwin))
    {
	ಆದರೆ (wp->w_ಬಫರ್ == ಪದ->tl_ಬಫರ್)
	    position_cursor(wp, &pos);
    }
    ಆದರೆ (ಪದ->tl_ಬಫರ್ == curbuf && !ಪದ->tl_ಸಾಮಾನ್ಯ_ಬಹುಲಕ)
	// Don't redraw here, it will be done after
	// vterm_input_write() is finished.
	update_cursor(ಪದ, FALSE);

    ಹಿಂತಿರುಗಿ 1;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
handle_settermprop(
	VTermProp prop,
	VTerm_ಮೌಲ್ಯ *ಮೌಲ್ಯ,
	ಶೂನ್ಯ *user)
{
    ಪದ_T	*ಪದ = (ಪದ_T *)user;
    char_u	*strval = ಖಾಲಿ;

    ಆಯ್ಕೆ (prop)
    {
	ಸ್ಥಿತಿ VTERM_PROP_TITLE:
	    ಆದರೆ (disable_vterm_title_for_testing)
		ನಿಲ್ಲಿಸು;
	    strval = vim_strnsave((char_u *)ಮೌಲ್ಯ->string.str,
							    ಮೌಲ್ಯ->string.len);
	    ಆದರೆ (strval == ಖಾಲಿ)
		ನಿಲ್ಲಿಸು;
	    vim_free(ಪದ->tl_title);
	    // a blank title isn't useful, make it empty, so that "running" is
	    // displayed
	    ಆದರೆ (*skipwhite(strval) == NUL)
		ಪದ->tl_title = ಖಾಲಿ;
	    // Same as blank
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಪದ->tl_arg0_cmd != ಖಾಲಿ
		    && STRNCMP(ಪದ->tl_arg0_cmd, strval,
					  (ಪೂರ್ಣಾಂಕ)STRLEN(ಪದ->tl_arg0_cmd)) == 0)
		ಪದ->tl_title = ಖಾಲಿ;
	    // Empty corrupted data of winpty
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP("  - ", strval, 4) == 0)
		ಪದ->tl_title = ಖಾಲಿ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!enc_utf8 && enc_codepage > 0)
	    {
		WCHAR   *ret = ಖಾಲಿ;
		ಪೂರ್ಣಾಂಕ	ಉದ್ದಳತೆ = 0;

		Multi_Byte_ಗಮ್ಯ_Wide_Char_alloc(CP_UTF8, 0,
			(ಅಕ್ಷರ*)ಮೌಲ್ಯ->string.str,
					(ಪೂರ್ಣಾಂಕ)ಮೌಲ್ಯ->string.len, &ret, &ಉದ್ದಳತೆ);
		ಆದರೆ (ret != ಖಾಲಿ)
		{
		    Wide_Char_ಗಮ್ಯ_Multi_Byte_alloc(enc_codepage, 0,
					ret, ಉದ್ದಳತೆ, (ಅಕ್ಷರ**)&ಪದ->tl_title,
					&ಉದ್ದಳತೆ, 0, 0);
		    vim_free(ret);
		}
	    }
#ಆದರೆ_ಅಂತ್ಯ
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಪದ->tl_title = strval;
		strval = ಖಾಲಿ;
	    }
	    VIM_CLEAR(ಪದ->tl_ಸ್ಥಿತಿ_ಪಠ್ಯ);
	    ಆದರೆ (ಪದ == curbuf->b_ಪದ)
	    {
		maketitle();
		curwin->w_redr_ಸ್ಥಿತಿ = true;
	    }
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ VTERM_PROP_CURSORVISIBLE:
	    ಪದ->tl_cursor_visible = ಮೌಲ್ಯ->boolean;
	    may_toggle_cursor(ಪದ);
	    ಹೊರಹೋಗು_flush();
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ VTERM_PROP_CURSORBLINK:
	    ಪದ->tl_cursor_blink = ಮೌಲ್ಯ->boolean;
	    may_ಗಣ_cursor_props(ಪದ);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ VTERM_PROP_CURSORSHAPE:
	    ಪದ->tl_cursor_shape = ಮೌಲ್ಯ->ಸಂಖ್ಯೆ;
	    may_ಗಣ_cursor_props(ಪದ);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ VTERM_PROP_CURSORCOLOR:
	    strval = vim_strnsave((char_u *)ಮೌಲ್ಯ->string.str,
							    ಮೌಲ್ಯ->string.len);
	    ಆದರೆ (strval == ಖಾಲಿ)
		ನಿಲ್ಲಿಸು;
	    cursor_color_copy(&ಪದ->tl_cursor_color, strval);
	    may_ಗಣ_cursor_props(ಪದ);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ VTERM_PROP_ALTSCREEN:
	    // TODO: do anything else?
	    ಪದ->tl_using_altscreen = ಮೌಲ್ಯ->boolean;
	    ನಿಲ್ಲಿಸು;

	ಪೂರ್ವನಿಯೋಜಿತ:
	    ನಿಲ್ಲಿಸು;
    }
    vim_free(strval);

    // Always return 1, otherwise vterm doesn't store the value internally.
    ಹಿಂತಿರುಗಿ 1;
}

/*
 * The job running in the terminal resized the terminal.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
handle_resize(ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲುಗಳು_2, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ, ಶೂನ್ಯ *user)
{
    ಪದ_T	*ಪದ = (ಪದ_T *)user;
    win_T	*wp;

    ಪದ->tl_ಅಡ್ಡಸಾಲುಗಳು = ಅಡ್ಡಸಾಲುಗಳು_2;
    ಪದ->tl_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;
    ಆದರೆ (ಪದ->tl_vterm_ಗಾತ್ರ_ಬದಲಾಯಿತು)
	// Size was set by vterm_set_size(), don't set the window size.
	ಪದ->tl_vterm_ಗಾತ್ರ_ಬದಲಾಯಿತು = FALSE;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	FOR_ALL_WINDOWS(wp)
	{
	    ಆದರೆ (wp->w_ಬಫರ್ == ಪದ->tl_ಬಫರ್)
	    {
		win_setheight_win(ಅಡ್ಡಸಾಲುಗಳು_2, wp);
		win_setwidth_win(ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ, wp);
	    }
	}
	redraw_buf_later(ಪದ->tl_ಬಫರ್, UPD_NOT_VALID);
    }
    ಹಿಂತಿರುಗಿ 1;
}

/*
 * If the number of lines that are stored goes over 'termwinscroll' then
 * delete the first 10%.
 * "gap" points to tl_scrollback or tl_scrollback_postponed.
 * "update_buffer" is TRUE when the buffer should be updated.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಎಲ್ಲೆ_scrollback(ಪದ_T *ಪದ, garray_T *gap, ಪೂರ್ಣಾಂಕ update_ಬಫರ್)
{
    ಆದರೆ (gap->ga_len < ಪದ->tl_ಬಫರ್->b_p_twsl)
	ಹಿಂತಿರುಗಿ;

    ಪೂರ್ಣಾಂಕ	todo = MAX(ಪದ->tl_ಬಫರ್->b_p_twsl / 10,
				     gap->ga_len - ಪದ->tl_ಬಫರ್->b_p_twsl);
    ಪೂರ್ಣಾಂಕ	i;
    sb_ಸಾಲು_T *sb_ಸಾಲುಗಳು = (sb_ಸಾಲು_T *)gap->ga_ದತ್ತಾಂಶ;

    curbuf = ಪದ->tl_ಬಫರ್;
    ಗಾಗಿ (i = 0; i < todo; ++i)
    {
	ಆದರೆ (update_ಬಫರ್ && (!sb_ಸಾಲುಗಳು[i].continuation || !i))
	{
	    ml_delete(1);
	    --ಪದ->tl_ಬಫರ್_scrolled;
	}
	vim_free(sb_ಸಾಲುಗಳು[i].sb_cells);
    }
    // Continue until end of wrapped line
    ಗಾಗಿ (; todo < gap->ga_len && sb_ಸಾಲುಗಳು[todo].continuation; ++todo)
	vim_free(sb_ಸಾಲುಗಳು[todo].sb_cells);
    curbuf = curwin->w_ಬಫರ್;

    gap->ga_len -= todo;
    mch_memmove(gap->ga_ದತ್ತಾಂಶ,
		(sb_ಸಾಲು_T *)gap->ga_ದತ್ತಾಂಶ + todo,
		ಗಾತ್ರ(sb_ಸಾಲು_T) * gap->ga_len);
    ಆದರೆ (update_ಬಫರ್)
    {
	win_T *curwin_save = curwin;
	win_T *wp = ಖಾಲಿ;

	ಪದ->tl_scrollback_scrolled -= todo;

	FOR_ALL_WINDOWS(wp)
	{
	    ಆದರೆ (wp->w_ಬಫರ್ == ಪದ->tl_ಬಫರ್)
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
handle_pushline(ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ, ಸ್ಥಿರಾಂಕ VTermScreenCell *cells, ಪೂರ್ಣಾಂಕ continuation, ಶೂನ್ಯ *user)
{
    ಪದ_T	*ಪದ = (ಪದ_T *)user;
    garray_T	*gap;
    ಪೂರ್ಣಾಂಕ		update_ಬಫರ್;

    ಆದರೆ (ಪದ->tl_ಸಾಮಾನ್ಯ_ಬಹುಲಕ)
    {
	// In Terminal-Normal mode the user interacts with the buffer, thus we
	// must not change it. Postpone adding the scrollback lines.
	gap = &ಪದ->tl_scrollback_postponed;
	update_ಬಫರ್ = FALSE;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// First remove the lines that were appended before, the pushed line
	// goes above it.
	cleanup_scrollback(ಪದ);
	gap = &ಪದ->tl_scrollback;
	update_ಬಫರ್ = TRUE;
    }

    ಎಲ್ಲೆ_scrollback(ಪದ, gap, update_ಬಫರ್);

    ಆದರೆ (ga_grow(gap, 1) == FAIL)
	ಹಿಂತಿರುಗಿ 0;

    cellattr_T	*p = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		len = 0;
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		c;
    ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು;
    ಪೂರ್ಣಾಂಕ		ಪಠ್ಯ_len;
    char_u		*ಪಠ್ಯ;
    sb_ಸಾಲು_T	*ಸಾಲು;
    garray_T	ga;
    cellattr_T	fill_attr = ಪದ->tl_default_color;

    // do not store empty cells at the end
    ಗಾಗಿ (i = 0; i < ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ; ++i)
	ಆದರೆ (cells[i].chars[0] != 0)
	    len = i + 1;
	ಇಲ್ಲದಿದ್ದರೆ
	    cell2cellattr(&cells[i], &fill_attr);

    ga_init2(&ga, 1, 100);
    ಆದರೆ (len > 0)
	p = ALLOC_MULT(cellattr_T, len);
    ಆದರೆ (p != ಖಾಲಿ)
    {
	ಗಾಗಿ (ಲಂಬಸಾಲು = 0; ಲಂಬಸಾಲು < len; ಲಂಬಸಾಲು += cells[ಲಂಬಸಾಲು].ಅಗಲ)
	{
	    ಆದರೆ (ga_grow(&ga, VTERM_MAX_CHARS_ಪ್ರತಿ_CELL * 4) == FAIL)
	    {
		ga.ga_len = 0;
		ನಿಲ್ಲಿಸು;
	    }
	    ಗಾಗಿ (i = 0; i < VTERM_MAX_CHARS_ಪ್ರತಿ_CELL &&
		    ((c = cells[ಲಂಬಸಾಲು].chars[i]) > 0 || i == 0); ++i)
		ga.ga_len += utf_char2bytes(c == NUL ? ' ' : c,
			(char_u *)ga.ga_ದತ್ತಾಂಶ + ga.ga_len);
	    cell2cellattr(&cells[ಲಂಬಸಾಲು], &p[ಲಂಬಸಾಲು]);
	}
    }
    ಆದರೆ (ga_grow(&ga, 1) == FAIL)
    {
	ಆದರೆ (update_ಬಫರ್)
	    ಪಠ್ಯ = (char_u *)"";
	ಇಲ್ಲದಿದ್ದರೆ
	    ಪಠ್ಯ = vim_strsave((char_u *)"");
	ಪಠ್ಯ_len = 0;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಪಠ್ಯ = ga.ga_ದತ್ತಾಂಶ;
	ಪಠ್ಯ_len = ga.ga_len;
	*(ಪಠ್ಯ + ಪಠ್ಯ_len) = NUL;
    }
    ಆದರೆ (update_ಬಫರ್)
	ಕೂಡಿಸು_scrollback_ಸಾಲು_ಗಮ್ಯ_ಬಫರ್(ಪದ, ಪಠ್ಯ, ಪಠ್ಯ_len, continuation);

    ಸಾಲು = (sb_ಸಾಲು_T *)gap->ga_ದತ್ತಾಂಶ + gap->ga_len;
    ಸಾಲು->sb_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = len;
    ಸಾಲು->sb_ಬೈಟುಗಳು = ಪಠ್ಯ_len;
    ಸಾಲು->sb_cells = p;
    ಸಾಲು->sb_fill_attr = fill_attr;
    ಸಾಲು->continuation = (char_u)continuation;
    ಆದರೆ (update_ಬಫರ್)
    {
	ಸಾಲು->sb_ಪಠ್ಯ = ಖಾಲಿ;
	++ಪದ->tl_scrollback_scrolled;
	ಆದರೆ (!continuation)
	    ++ಪದ->tl_ಬಫರ್_scrolled;
	ga_clear(&ga);  // free the text
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಸಾಲು->sb_ಪಠ್ಯ = ಪಠ್ಯ;
	ga_init(&ga);  // text is kept in tl_scrollback_postponed
    }
    ++gap->ga_len;
    ಹಿಂತಿರುಗಿ 0; // ignored
}

/*
 * Called when leaving Terminal-Normal mode: deal with any scrollback that was
 * received and stored in tl_scrollback_postponed.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
handle_postponed_scrollback(ಪದ_T *ಪದ)
{
    ಪೂರ್ಣಾಂಕ i;

    ಆದರೆ (ಪದ->tl_scrollback_postponed.ga_len == 0)
	ಹಿಂತಿರುಗಿ;
    ch_log(ಖಾಲಿ, "Moving postponed scrollback to scrollback");

    // First remove the lines that were appended before, the pushed lines go
    // above it.
    cleanup_scrollback(ಪದ);

    ಗಾಗಿ (i = 0; i < ಪದ->tl_scrollback_postponed.ga_len; ++i)
    {
	char_u		*ಪಠ್ಯ;
	sb_ಸಾಲು_T	*pp_ಸಾಲು;
	sb_ಸಾಲು_T	*ಸಾಲು;

	ಆದರೆ (ga_grow(&ಪದ->tl_scrollback, 1) == FAIL)
	    ನಿಲ್ಲಿಸು;
	pp_ಸಾಲು = (sb_ಸಾಲು_T *)ಪದ->tl_scrollback_postponed.ga_ದತ್ತಾಂಶ + i;

	ಪಠ್ಯ = pp_ಸಾಲು->sb_ಪಠ್ಯ;
	ಆದರೆ (ಪಠ್ಯ == ಖಾಲಿ)
	    ಪಠ್ಯ = (char_u *)"";
	ಕೂಡಿಸು_scrollback_ಸಾಲು_ಗಮ್ಯ_ಬಫರ್(ಪದ, ಪಠ್ಯ, (ಪೂರ್ಣಾಂಕ)STRLEN(ಪಠ್ಯ), pp_ಸಾಲು->continuation);
	vim_free(pp_ಸಾಲು->sb_ಪಠ್ಯ);

	ಸಾಲು = (sb_ಸಾಲು_T *)ಪದ->tl_scrollback.ga_ದತ್ತಾಂಶ
						 + ಪದ->tl_scrollback.ga_len;
	ಸಾಲು->sb_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = pp_ಸಾಲು->sb_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;
	ಸಾಲು->sb_ಬೈಟುಗಳು = pp_ಸಾಲು->sb_ಬೈಟುಗಳು;
	ಸಾಲು->sb_cells = pp_ಸಾಲು->sb_cells;
	ಸಾಲು->sb_fill_attr = pp_ಸಾಲು->sb_fill_attr;
	ಸಾಲು->sb_ಪಠ್ಯ = ಖಾಲಿ;
	++ಪದ->tl_scrollback_scrolled;
	++ಪದ->tl_scrollback.ga_len;
	ಆದರೆ (!pp_ಸಾಲು->continuation)
	    ++ಪದ->tl_ಬಫರ್_scrolled;
    }

    ga_clear(&ಪದ->tl_scrollback_postponed);
    ಎಲ್ಲೆ_scrollback(ಪದ, &ಪದ->tl_scrollback, TRUE);
}

/*
 * Called when the terminal wants to ring the system bell.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
handle_bell(ಶೂನ್ಯ *user ಬಳಸದ)
{
    vim_beep(BO_ಪದ);
    ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ VTermScreenCallbacks screen_callbacks = {
    handle_damage,		// damage
    handle_moverect,		// moverect
    handle_movecursor,		// movecursor
    handle_settermprop,		// settermprop
    handle_bell,		// bell
    handle_resize,		// resize
    ಖಾಲಿ,			// sb_pushline
    ಖಾಲಿ,			// sb_popline
    ಖಾಲಿ,			// sb_clear
    handle_pushline	// sb_pushline4
};

/*
 * Do the work after the channel of a terminal was closed.
 * Must be called only when updating_screen is FALSE.
 * Returns TRUE when a buffer was closed (list of terminals may have changed).
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಪದ_after_channel_closed(ಪದ_T *ಪದ)
{
    // Unless in Terminal-Normal mode: clear the vterm.
    ಆದರೆ (!ಪದ->tl_ಸಾಮಾನ್ಯ_ಬಹುಲಕ)
    {
	ಪೂರ್ಣಾಂಕ	fnum = ಪದ->tl_ಬಫರ್->b_fnum;

	cleanup_vterm(ಪದ);

	ಆದರೆ (ಪದ->tl_ಮುಕ್ತಾಯ == TL_ಮುಕ್ತಾಯ_CLOSE)
	{
	    aco_save_T	aco;
	    ಪೂರ್ಣಾಂಕ		do_ಗಣ_w_locked = ಪದ->tl_ಬಫರ್->b_nwindows == 0;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
	    win_T	*pwin = ಖಾಲಿ;

	    // If this was a terminal in a popup window, go back to the
	    // previous window.
	    ಆದರೆ (popup_is_popup(curwin) && curbuf == ಪದ->tl_ಬಫರ್)
	    {
		pwin = curwin;
		ಆದರೆ (win_valid(prevwin))
		    win_enter(prevwin, FALSE);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	    // If this is the last normal window: exit Vim.
	    ಆದರೆ (ಪದ->tl_ಬಫರ್->b_nwindows > 0 && only_one_ಕಿಟಕಿ())
	    {
		exarg_T ea;

		CLEAR_ಕ್ಷೇತ್ರ(ea);
		ex_quit(&ea);
		ಹಿಂತಿರುಗಿ TRUE;
	    }

	    // ++close or term_finish == "close"
	    ch_log(ಖಾಲಿ, "terminal job finished, closing window");
	    aucmd_prepbuf(&aco, ಪದ->tl_ಬಫರ್);
	    ಆದರೆ (curbuf == ಪದ->tl_ಬಫರ್)
	    {
		win_T	*wp = curwin;

		// Avoid closing the window if we temporarily use it.
		ಆದರೆ (is_aucmd_win(wp))
		    do_ಗಣ_w_locked = TRUE;
		ಆದರೆ (do_ಗಣ_w_locked)
		    ++wp->w_locked;
		do_bufdel(DOBUF_WIPE, (char_u *)"", 1, fnum, fnum, FALSE);
		ಆದರೆ (do_ಗಣ_w_locked)
		    --wp->w_locked;
		aucmd_restbuf(&aco);
	    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
	    ಆದರೆ (pwin != ಖಾಲಿ)
		popup_close_with_retval(pwin, 0);
#ಆದರೆ_ಅಂತ್ಯ
	    ಹಿಂತಿರುಗಿ TRUE;
	}
	ಆದರೆ (ಪದ->tl_ಮುಕ್ತಾಯ == TL_ಮುಕ್ತಾಯ_OPEN
				   && ಪದ->tl_ಬಫರ್->b_nwindows == 0)
	{
	    ಅಕ್ಷರ    *cmd = ಪದ->tl_opencmd == ಖಾಲಿ
				? "botright sbuf %d"
				: (ಅಕ್ಷರ *)ಪದ->tl_opencmd;
	    size_t  len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(cmd) + 50;
	    ಅಕ್ಷರ    *buf = alloc(len);

	    ಆದರೆ (buf != ಖಾಲಿ)
	    {
		ch_log(ಖಾಲಿ, "terminal job finished, opening window");
		vim_snprintf(buf, len, cmd, fnum);
		do_cmdline_cmd((char_u *)buf);
		vim_free(buf);
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ch_log(ಖಾಲಿ, "terminal job finished");
    }

    redraw_buf_and_ಸ್ಥಿತಿ_later(ಪದ->tl_ಬಫರ್, UPD_NOT_VALID);
    ಹಿಂತಿರುಗಿ FALSE;
}

#ಆದರೆ defined(FEAT_PROP_POPUP)
/*
 * If the current window is a terminal in a popup window and the job has
 * finished, close the popup window and to back to the previous window.
 * Otherwise return FAIL.
 */
    ಪೂರ್ಣಾಂಕ
may_close_ಪದ_popup(ಶೂನ್ಯ)
{
    ಆದರೆ (!popup_is_popup(curwin) || curbuf->b_ಪದ == ಖಾಲಿ
				 || ಪದ_ಕೆಲಸ_running_not_none(curbuf->b_ಪದ))
	ಹಿಂತಿರುಗಿ FAIL;

    win_T *pwin = curwin;

    ಆದರೆ (win_valid(prevwin))
	win_enter(prevwin, FALSE);
    popup_close_with_retval(pwin, 0);
    ಹಿಂತಿರುಗಿ OK;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Called when a channel is going to be closed, before invoking the close
 * callback.
 */
    ಶೂನ್ಯ
ಪದ_channel_closing(channel_T *ch)
{
    ಪದ_T *ಪದ;

    ಗಾಗಿ (ಪದ = ಮೊದಲ_ಪದ; ಪದ != ಖಾಲಿ; ಪದ = ಪದ->tl_ಮುಂದಿನ)
	ಆದರೆ (ಪದ->tl_ಕೆಲಸ == ch->ch_ಕೆಲಸ && !ಪದ->tl_channel_closed)
	    ಪದ->tl_channel_closing = TRUE;
}

/*
 * Called when a channel has been closed.
 * If this was a channel for a terminal window then finish it up.
 */
    ಶೂನ್ಯ
ಪದ_channel_closed(channel_T *ch)
{
    ಪದ_T *ಪದ;
    ಪದ_T *ಮುಂದಿನ_ಪದ;
    ಪೂರ್ಣಾಂಕ	    did_one = FALSE;

    ಗಾಗಿ (ಪದ = ಮೊದಲ_ಪದ; ಪದ != ಖಾಲಿ; ಪದ = ಮುಂದಿನ_ಪದ)
    {
	ಮುಂದಿನ_ಪದ = ಪದ->tl_ಮುಂದಿನ;
	ಆದರೆ (ಪದ->tl_ಕೆಲಸ == ch->ch_ಕೆಲಸ && !ಪದ->tl_channel_closed)
	{
	    ಪದ->tl_channel_closed = TRUE;
	    did_one = TRUE;

	    VIM_CLEAR(ಪದ->tl_title);
	    VIM_CLEAR(ಪದ->tl_ಸ್ಥಿತಿ_ಪಠ್ಯ);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
	    ಆದರೆ (ಪದ->tl_ಹೊರಹೋಗು_fd != ಖಾಲಿ)
	    {
		fclose(ಪದ->tl_ಹೊರಹೋಗು_fd);
		ಪದ->tl_ಹೊರಹೋಗು_fd = ಖಾಲಿ;
	    }
#ಆದರೆ_ಅಂತ್ಯ

	    ಆದರೆ (updating_screen)
	    {
		// Cannot open or close windows now.  Can happen when
		// 'lazyredraw' is set.
		ಪದ->tl_channel_recently_closed = TRUE;
		ಮುಂದುವರಿ;
	    }

	    ಆದರೆ (ಪದ_after_channel_closed(ಪದ))
		ಮುಂದಿನ_ಪದ = ಮೊದಲ_ಪದ;
	}
    }

    ಆದರೆ (did_one)
    {
	redraw_statuslines();

	// Need to break out of vgetc().
	ins_char_typebuf(K_IGNORE, 0);
	typebuf_was_filled = TRUE;

	ಪದ = curbuf->b_ಪದ;
	ಆದರೆ (ಪದ != ಖಾಲಿ)
	{
	    ಆದರೆ (ಪದ->tl_ಕೆಲಸ == ch->ch_ಕೆಲಸ)
		maketitle();
	    update_cursor(ಪದ, ಪದ->tl_cursor_visible);
	}
    }
}

/*
 * To be called after resetting updating_screen: handle any terminal where the
 * channel was closed.
 */
    ಶೂನ್ಯ
ಪದ_check_channel_closed_recently(ಶೂನ್ಯ)
{
    ಪದ_T *ಪದ;
    ಪದ_T *ಮುಂದಿನ_ಪದ;

    ಗಾಗಿ (ಪದ = ಮೊದಲ_ಪದ; ಪದ != ಖಾಲಿ; ಪದ = ಮುಂದಿನ_ಪದ)
    {
	ಮುಂದಿನ_ಪದ = ಪದ->tl_ಮುಂದಿನ;
	ಆದರೆ (ಪದ->tl_channel_recently_closed)
	{
	    ಪದ->tl_channel_recently_closed = FALSE;
	    ಆದರೆ (ಪದ_after_channel_closed(ಪದ))
		// start over, the list may have changed
		ಮುಂದಿನ_ಪದ = ಮೊದಲ_ಪದ;
	}
    }
}

/*
 * Fill one screen line from a line of the terminal.
 * Advances "pos" to past the last column.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಪದ_line2screenline(
	ಪದ_T		*ಪದ,
	win_T		*wp,
	VTermScreen	*screen,
	VTermPos	*pos,
	ಪೂರ್ಣಾಂಕ		max_ಲಂಬಸಾಲು)
{
    ಪೂರ್ಣಾಂಕ off = screen_get_current_ಸಾಲು_off();

    ಗಾಗಿ (pos->ಲಂಬಸಾಲು = 0; pos->ಲಂಬಸಾಲು < max_ಲಂಬಸಾಲು; )
    {
	VTermScreenCell cell;
	ಪೂರ್ಣಾಂಕ		c;

	ಆದರೆ (vterm_screen_get_cell(screen, *pos, &cell) == 0)
	    CLEAR_ಕ್ಷೇತ್ರ(cell);

	c = cell.chars[0];
	ಆದರೆ (c == NUL)
	{
	    Screen_ಸಾಲುಗಳು[off] = ' ';
	    ಆದರೆ (enc_utf8)
		Screen_ಸಾಲುಗಳು_UC[off] = NUL;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (enc_utf8)
	    {
		ಪೂರ್ಣಾಂಕ i;

		// composing chars
		ಗಾಗಿ (i = 0; i < Screen_mco
			      && i + 1 < VTERM_MAX_CHARS_ಪ್ರತಿ_CELL; ++i)
		{
		    Screen_ಸಾಲುಗಳು_C[i][off] = cell.chars[i + 1];
		    ಆದರೆ (cell.chars[i + 1] == 0)
			ನಿಲ್ಲಿಸು;
		}
		ಆದರೆ (c >= 0x80 || (Screen_mco > 0
					 && Screen_ಸಾಲುಗಳು_C[0][off] != 0))
		{
		    Screen_ಸಾಲುಗಳು[off] = ' ';
		    Screen_ಸಾಲುಗಳು_UC[off] = c;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    Screen_ಸಾಲುಗಳು[off] = c;
		    Screen_ಸಾಲುಗಳು_UC[off] = NUL;
		}
	    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (has_mbyte && c >= 0x80)
	    {
		char_u	mb[MB_MAXBYTES+1];
		WCHAR	wc = c;

		ಆದರೆ (Wide_Char_ಗಮ್ಯ_Multi_Byte(GetACP(), 0, &wc, 1,
					       (ಅಕ್ಷರ*)mb, 2, 0, 0) > 1)
		{
		    Screen_ಸಾಲುಗಳು[off] = mb[0];
		    Screen_ಸಾಲುಗಳು[off + 1] = mb[1];
		    cell.ಅಗಲ = mb_ptr2cells(mb);
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    Screen_ಸಾಲುಗಳು[off] = c;
	    }
#ಆದರೆ_ಅಂತ್ಯ
	    ಇಲ್ಲದಿದ್ದರೆ
		// This will only store the lower byte of "c".
		Screen_ಸಾಲುಗಳು[off] = c;
	}
	ScreenAttrs[off] = cell2attr(ಪದ, wp, &cell.attrs, &cell.fg,
								     &cell.bg);

	++pos->ಲಂಬಸಾಲು;
	++off;
	ಆದರೆ (cell.ಅಗಲ == 2)
	{
	    // don't set the second byte to NUL for a DBCS encoding, it
	    // has been set above
	    ಆದರೆ (enc_utf8)
	    {
		Screen_ಸಾಲುಗಳು_UC[off] = NUL;
		Screen_ಸಾಲುಗಳು[off] = NUL;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!has_mbyte)
	    {
		// Can't show a double-width character with a single-byte
		// 'encoding', just use a space.
		Screen_ಸಾಲುಗಳು[off] = ' ';
		ScreenAttrs[off] = ScreenAttrs[off - 1];
	    }

	    ++pos->ಲಂಬಸಾಲು;
	    ++off;
	}
    }
}

#ಆದರೆ defined(FEAT_GUI)
    ಸ್ಥಿರ ಶೂನ್ಯ
update_system_ಪದ(ಪದ_T *ಪದ)
{
    VTermPos	    pos;
    VTermScreen	    *screen;

    ಆದರೆ (ಪದ->tl_vterm == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    screen = vterm_obtain_screen(ಪದ->tl_vterm);

    // Scroll up to make more room for terminal lines if needed.
    // Use the cursor position to determine how much to scroll, because
    // ConPTY may damage all rows on initialization even when most are
    // empty, which would cause unnecessary scrolling.
    ಆಗ (ಪದ->tl_toprow > 0
		  && (ಅಡ್ಡಸಾಲುಗಳು - ಪದ->tl_toprow) < ಪದ->tl_cursor_pos.ಅಡ್ಡಸಾಲು + 1)
    {
	ಪೂರ್ಣಾಂಕ save_p_ಇನ್ನಷ್ಟು = p_ಇನ್ನಷ್ಟು;

	p_ಇನ್ನಷ್ಟು = FALSE;
	ಸಂದೇಶ_ಅಡ್ಡಸಾಲು = ಅಡ್ಡಸಾಲುಗಳು - 1;
	ಸಂದೇಶ_puts("\n");
	p_ಇನ್ನಷ್ಟು = save_p_ಇನ್ನಷ್ಟು;
	--ಪದ->tl_toprow;
    }

    ಗಾಗಿ (pos.ಅಡ್ಡಸಾಲು = ಪದ->tl_dirty_ಅಡ್ಡಸಾಲು_ಆರಂಭ; pos.ಅಡ್ಡಸಾಲು < ಪದ->tl_dirty_ಅಡ್ಡಸಾಲು_ಕೊನೆ
						  && pos.ಅಡ್ಡಸಾಲು < ಅಡ್ಡಸಾಲುಗಳು; ++pos.ಅಡ್ಡಸಾಲು)
    {
	ಆದರೆ (pos.ಅಡ್ಡಸಾಲು < ಪದ->tl_ಅಡ್ಡಸಾಲುಗಳು)
	{
	    ಪೂರ್ಣಾಂಕ max_ಲಂಬಸಾಲು = MIN(topframe->fr_ಅಗಲ, ಪದ->tl_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ);

	    ಪದ_line2screenline(ಪದ, ಖಾಲಿ, screen, &pos, max_ಲಂಬಸಾಲು);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    pos.ಲಂಬಸಾಲು = 0;

	screen_ಸಾಲು(curwin, ಪದ->tl_toprow + pos.ಅಡ್ಡಸಾಲು,
		firstwin->w_wincol, pos.ಲಂಬಸಾಲು, topframe->fr_ಅಗಲ, -1, 0);
    }

    ಪದ->tl_dirty_ಅಡ್ಡಸಾಲು_ಆರಂಭ = MAX_ಅಡ್ಡಸಾಲು;
    ಪದ->tl_dirty_ಅಡ್ಡಸಾಲು_ಕೊನೆ = 0;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Return TRUE if window "wp" is to be redrawn with term_update_window().
 * Returns FALSE when there is no terminal running in this window or it is in
 * Terminal-Normal mode.
 */
    ಪೂರ್ಣಾಂಕ
ಪದ_do_update_ಕಿಟಕಿ(win_T *wp)
{
    ಪದ_T	*ಪದ = wp->w_ಬಫರ್->b_ಪದ;

    ಹಿಂತಿರುಗಿ ಪದ != ಖಾಲಿ && ಪದ->tl_vterm != ಖಾಲಿ && !ಪದ->tl_ಸಾಮಾನ್ಯ_ಬಹುಲಕ;
}

/*
 * Called to update a window that contains an active terminal.
 */
    ಶೂನ್ಯ
ಪದ_update_ಕಿಟಕಿ(win_T *wp)
{
    ಪದ_T	*ಪದ = wp->w_ಬಫರ್->b_ಪದ;
    VTerm	*vterm;
    VTermScreen *screen;
    VTermState	*state;
    VTermPos	pos;
    ಪೂರ್ಣಾಂಕ		ಅಡ್ಡಸಾಲುಗಳು_2, ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;
    ಪೂರ್ಣಾಂಕ		newrows, newcols;
    ಪೂರ್ಣಾಂಕ		minsize;
    win_T	*twp;

    vterm = ಪದ->tl_vterm;
    screen = vterm_obtain_screen(vterm);
    state = vterm_obtain_state(vterm);

    // We use UPD_NOT_VALID on a resize or scroll, redraw everything then.
    // With UPD_SOME_VALID only redraw what was marked dirty.
    ಆದರೆ (wp->w_redr_type > UPD_SOME_VALID)
    {
	ಪದ->tl_dirty_ಅಡ್ಡಸಾಲು_ಆರಂಭ = 0;
	ಪದ->tl_dirty_ಅಡ್ಡಸಾಲು_ಕೊನೆ = MAX_ಅಡ್ಡಸಾಲು;

	ಆದರೆ (ಪದ->tl_postponed_scroll > 0
			      && ಪದ->tl_postponed_scroll < ಪದ->tl_ಅಡ್ಡಸಾಲುಗಳು / 3)
	    // Scrolling is usually faster than redrawing, when there are only
	    // a few lines to scroll.
	    ಪದ_scroll_ಮೇಲೆ(ಪದ, 0, ಪದ->tl_postponed_scroll);
	ಪದ->tl_postponed_scroll = 0;
    }

    /*
     * If the window was resized a redraw will be triggered and we get here.
     * Adjust the size of the vterm unless 'termwinsize' specifies a fixed size.
     */
    minsize = parse_termwinsize(wp, &ಅಡ್ಡಸಾಲುಗಳು_2, &ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ);

    newrows = 99999;
    newcols = 99999;
    ಗಾಗಿ (twp = firstwin; ; twp = twp->w_ಮುಂದಿನ)
    {
	// Always use curwin, it may be a popup window.
	win_T *wwp = twp == ಖಾಲಿ ? curwin : twp;

	// When more than one window shows the same terminal, use the
	// smallest size.
	ಆದರೆ (wwp->w_ಬಫರ್ == ಪದ->tl_ಬಫರ್)
	{
	    newrows = MIN(newrows, wwp->w_ಎತ್ತರ);
	    newcols = MIN(newcols, wwp->w_ಅಗಲ);
	}
	ಆದರೆ (twp == ಖಾಲಿ)
	    ನಿಲ್ಲಿಸು;
    }
    ಆದರೆ (newrows == 99999 || newcols == 99999)
	ಹಿಂತಿರುಗಿ; // safety exit
    newrows = ಅಡ್ಡಸಾಲುಗಳು_2 == 0 ? newrows : minsize ? MAX(ಅಡ್ಡಸಾಲುಗಳು_2, newrows) : ಅಡ್ಡಸಾಲುಗಳು_2;
    newcols = ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ == 0 ? newcols : minsize ? MAX(ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ, newcols) : ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;

    // If no cell is visible there is no point in resizing.  Also, vterm can't
    // handle a zero height.
    ಆದರೆ (newrows == 0 || newcols == 0)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (ಪದ->tl_ಅಡ್ಡಸಾಲುಗಳು != newrows || ಪದ->tl_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ != newcols)
    {
	ಪದ->tl_vterm_ಗಾತ್ರ_ಬದಲಾಯಿತು = TRUE;
	vterm_ಗಣ_ಗಾತ್ರ(vterm, newrows, newcols);
	ch_log(ಪದ->tl_ಕೆಲಸ->jv_channel, "Resizing terminal to %d lines",
								      newrows);
	ಪದ_report_winsize(ಪದ, newrows, newcols);

	// Updating the terminal size will cause the snapshot to be cleared.
	// When not in terminal_loop() we need to restore it.
	ಆದರೆ (ಪದ != ಒಳ_terminal_loop)
	    may_move_terminal_ಗಮ್ಯ_ಬಫರ್(ಪದ, FALSE);
    }

    // The cursor may have been moved when resizing.
    vterm_state_get_cursorpos(state, &pos);
    position_cursor(wp, &pos);

    ಗಾಗಿ (pos.ಅಡ್ಡಸಾಲು = ಪದ->tl_dirty_ಅಡ್ಡಸಾಲು_ಆರಂಭ; pos.ಅಡ್ಡಸಾಲು < ಪದ->tl_dirty_ಅಡ್ಡಸಾಲು_ಕೊನೆ
					  && pos.ಅಡ್ಡಸಾಲು < wp->w_ಎತ್ತರ; ++pos.ಅಡ್ಡಸಾಲು)
    {
	ಆದರೆ (pos.ಅಡ್ಡಸಾಲು < ಪದ->tl_ಅಡ್ಡಸಾಲುಗಳು)
	{
	    ಪೂರ್ಣಾಂಕ max_ಲಂಬಸಾಲು = MIN(wp->w_ಅಗಲ, ಪದ->tl_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ);

	    ಪದ_line2screenline(ಪದ, wp, screen, &pos, max_ಲಂಬಸಾಲು);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    pos.ಲಂಬಸಾಲು = 0;

	screen_ಸಾಲು(wp, wp->w_winrow + pos.ಅಡ್ಡಸಾಲು
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU
				+ winbar_ಎತ್ತರ(wp)
#ಆದರೆ_ಅಂತ್ಯ
				, wp->w_wincol, pos.ಲಂಬಸಾಲು, wp->w_ಅಗಲ, -1,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
				popup_is_popup(wp) ? SLF_POPUP :
#ಆದರೆ_ಅಂತ್ಯ
				0);
    }
}

/*
 * Called after updating all windows: may reset dirty rows.
 */
    ಶೂನ್ಯ
ಪದ_did_update_ಕಿಟಕಿ(win_T *wp)
{
    ಪದ_T	*ಪದ = wp->w_ಬಫರ್->b_ಪದ;

    ಆದರೆ (ಪದ == ಖಾಲಿ || ಪದ->tl_vterm == ಖಾಲಿ || ಪದ->tl_ಸಾಮಾನ್ಯ_ಬಹುಲಕ
						|| wp->w_redr_type != 0)
	ಹಿಂತಿರುಗಿ;

    ಪದ->tl_dirty_ಅಡ್ಡಸಾಲು_ಆರಂಭ = MAX_ಅಡ್ಡಸಾಲು;
    ಪದ->tl_dirty_ಅಡ್ಡಸಾಲು_ಕೊನೆ = 0;
}

/*
 * Return TRUE if "wp" is a terminal window where the job has finished.
 */
    ಪೂರ್ಣಾಂಕ
ಪದ_is_ಪೂರ್ಣಗೊಂಡ(buf_T *buf)
{
    ಹಿಂತಿರುಗಿ buf->b_ಪದ != ಖಾಲಿ && buf->b_ಪದ->tl_vterm == ಖಾಲಿ;
}

/*
 * Return TRUE if "wp" is a terminal window where the job has finished or we
 * are in Terminal-Normal mode, thus we show the buffer contents.
 */
    ಪೂರ್ಣಾಂಕ
ಪದ_show_ಬಫರ್(buf_T *buf)
{
    ಪದ_T *ಪದ = buf->b_ಪದ;

    ಹಿಂತಿರುಗಿ ಪದ != ಖಾಲಿ && (ಪದ->tl_vterm == ಖಾಲಿ || ಪದ->tl_ಸಾಮಾನ್ಯ_ಬಹುಲಕ);
}

/*
 * The current buffer is going to be changed.  If there is terminal
 * highlighting remove it now.
 */
    ಶೂನ್ಯ
ಪದ_change_ಒಳ_curbuf(ಶೂನ್ಯ)
{
    ಪದ_T *ಪದ = curbuf->b_ಪದ;

    ಆದರೆ (!ಪದ_is_ಪೂರ್ಣಗೊಂಡ(curbuf) || ಪದ->tl_scrollback.ga_len <= 0)
	ಹಿಂತಿರುಗಿ;

    free_scrollback(ಪದ);
    redraw_buf_later(ಪದ->tl_ಬಫರ್, UPD_NOT_VALID);

    // The buffer is now like a normal buffer, it cannot be easily
    // abandoned when changed.
    ಗಣ_string_option_ನೇರ((char_u *)"buftype", -1,
	    (char_u *)"", OPT_FREE|OPT_LOCAL, 0);
}

/*
 * Get the screen attribute for a position in the buffer.
 * Use a negative "col" to get the filler background color.
 */
    ಪೂರ್ಣಾಂಕ
ಪದ_get_attr(win_T *wp, linenr_T lnum, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು)
{
    buf_T	*buf = wp->w_ಬಫರ್;
    ಪದ_T	*ಪದ = buf->b_ಪದ;
    sb_ಸಾಲು_T	*ಸಾಲು;
    cellattr_T	*cellattr;
    ಪೂರ್ಣಾಂಕ         sb_ಸಾಲು = -1;
    ಪೂರ್ಣಾಂಕ         sb_ಲಂಬಸಾಲು = ಲಂಬಸಾಲು;

    ಆದರೆ (ಪದ->tl_scrollback.ga_len)
	bufline_pos_ಒಳ_scrollback(ಪದ, lnum, ಲಂಬಸಾಲು, &sb_ಸಾಲು, &sb_ಲಂಬಸಾಲು);

    ಆದರೆ (sb_ಸಾಲು < 0)
	cellattr = &ಪದ->tl_default_color;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಸಾಲು = (sb_ಸಾಲು_T *)ಪದ->tl_scrollback.ga_ದತ್ತಾಂಶ + sb_ಸಾಲು;
	ಆದರೆ (sb_ಲಂಬಸಾಲು < 0 || sb_ಲಂಬಸಾಲು >= ಸಾಲು->sb_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ)
	    cellattr = &ಸಾಲು->sb_fill_attr;
	ಇಲ್ಲದಿದ್ದರೆ
	    cellattr = ಸಾಲು->sb_cells + sb_ಲಂಬಸಾಲು;
    }
    ಹಿಂತಿರುಗಿ cell2attr(ಪದ, wp, &cellattr->attrs, &cellattr->fg, &cellattr->bg);
}

/*
 * Convert a cterm color number 0 - 255 to RGB.
 * This is compatible with xterm.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
cterm_color2vterm(ಪೂರ್ಣಾಂಕ nr, VTermColor *rgb)
{
    cterm_color2rgb(nr, &rgb->red, &rgb->green, &rgb->blue, &rgb->ಸೂಚ್ಯಂಕ);
    ಆದರೆ (rgb->ಸೂಚ್ಯಂಕ == 0)
	rgb->type = VTERM_COLOR_RGB;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	rgb->type = VTERM_COLOR_ಸೂಚ್ಯಂಕಿತ;
	--rgb->ಸೂಚ್ಯಂಕ;
    }
}

/*
 * Initialize vterm color from the synID.
 * Returns TRUE if color is set to "fg" and "bg".
 * Otherwise returns FALSE.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
get_vterm_color_from_synid(ಪೂರ್ಣಾಂಕ id, VTermColor *fg, VTermColor *bg)
{
#ಆದರೆ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    // Use the actual color for the GUI and when 'termguicolors' is set.
    ಆದರೆ (0
# ifdef FEAT_GUI
	    || gui.ಒಳ_use
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

	ಆದರೆ (id > 0)
	    syn_id2colors(id, &fg_rgb, &bg_rgb);

	ಆದರೆ (fg_rgb != INVALCOLOR)
	{
	    long_u rgb = GUI_MCH_GET_RGB(fg_rgb);
	    fg->red = (ಚಿಹ್ನೆರಹಿತ)(rgb >> 16);
	    fg->green = (ಚಿಹ್ನೆರಹಿತ)(rgb >> 8) & 255;
	    fg->blue = (ಚಿಹ್ನೆರಹಿತ)rgb & 255;
	    fg->type = VTERM_COLOR_RGB | VTERM_COLOR_DEFAULT_FG;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    fg->type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_FG;

	ಆದರೆ (bg_rgb != INVALCOLOR)
	{
	    long_u rgb = GUI_MCH_GET_RGB(bg_rgb);
	    bg->red = (ಚಿಹ್ನೆರಹಿತ)(rgb >> 16);
	    bg->green = (ಚಿಹ್ನೆರಹಿತ)(rgb >> 8) & 255;
	    bg->blue = (ಚಿಹ್ನೆರಹಿತ)rgb & 255;
	    bg->type = VTERM_COLOR_RGB | VTERM_COLOR_DEFAULT_BG;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    bg->type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_BG;

	ಹಿಂತಿರುಗಿ TRUE;
    }
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (t_colors >= 16)
    {
	ಪೂರ್ಣಾಂಕ cterm_fg = -1;
	ಪೂರ್ಣಾಂಕ cterm_bg = -1;

	ಆದರೆ (id > 0)
	    syn_id2cterm_bg(id, &cterm_fg, &cterm_bg);

	ಆದರೆ (cterm_fg >= 0)
	{
	    cterm_color2vterm(cterm_fg, fg);
	    fg->type |= VTERM_COLOR_DEFAULT_FG;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    fg->type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_FG;

	ಆದರೆ (cterm_bg >= 0)
	{
	    cterm_color2vterm(cterm_bg, bg);
	    bg->type |= VTERM_COLOR_DEFAULT_BG;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    bg->type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_BG;

	ಹಿಂತಿರುಗಿ TRUE;
    }

    ಹಿಂತಿರುಗಿ FALSE;
}

    ಶೂನ್ಯ
ಪದ_reset_hlfwin(win_T *wp)
{
    wp->w_ಪದ_hlfwin.fg.type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_FG;
    wp->w_ಪದ_hlfwin.bg.type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_BG;
}

/*
 * Cache the color of HLF_WIN.
 */
    ಶೂನ್ಯ
ಪದ_update_hlfwin(win_T *wp)
{
    ಪೂರ್ಣಾಂಕ id = wp->w_hlfwin_id;

    ಆದರೆ (id == 0 || !get_vterm_color_from_synid(id == -1 ? 0 : id,
		&wp->w_ಪದ_hlfwin.fg, &wp->w_ಪದ_hlfwin.bg))
	ಪದ_reset_hlfwin(wp);
}

/*
 * Called when option 'termguicolors' was set,
 * or when any highlight is changed.
 */
    ಶೂನ್ಯ
ಪದ_update_hlfwin_all(ಶೂನ್ಯ)
{
    win_T	 *wp = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		 did_curwin = FALSE;

    ಆಗ (for_all_windows_and_curwin(&wp, &did_curwin))
	ಪದ_update_hlfwin(wp);
}

/*
 * Initialize term->tl_default_color from the environment.
 */
    ಶೂನ್ಯ
ಪದ_init_default_colors(ಪದ_T *ಪದ)
{
    VTermColor	    *fg, *bg;
    ಪೂರ್ಣಾಂಕ		    fgval, bgval;
    ಪೂರ್ಣಾಂಕ		    id;

    CLEAR_ಕ್ಷೇತ್ರ(ಪದ->tl_default_color.attrs);
    ಪದ->tl_default_color.ಅಗಲ = 1;
    fg = &ಪದ->tl_default_color.fg;
    bg = &ಪದ->tl_default_color.bg;

    // Vterm uses a default black background.  Set it to white when
    // 'background' is "light".
    ಆದರೆ (*p_bg == 'l')
    {
	fgval = 0;
	bgval = 255;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	fgval = 255;
	bgval = 0;
    }
    fg->red = fg->green = fg->blue = fgval;
    bg->red = bg->green = bg->blue = bgval;
    fg->type = VTERM_COLOR_RGB | VTERM_COLOR_DEFAULT_FG;
    bg->type = VTERM_COLOR_RGB | VTERM_COLOR_DEFAULT_BG;

    // The highlight group overrules the defaults.
    id = ಪದ_get_highlight_id(ಪದ, ಖಾಲಿ);

    ಆದರೆ (!get_vterm_color_from_synid(id, fg, bg))
    {
#ಆದರೆ defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
	ಪೂರ್ಣಾಂಕ tmp;
#ಆದರೆ_ಅಂತ್ಯ

	// In an MS-Windows console we know the normal colors.
	ಆದರೆ (cterm_ಸಾಮಾನ್ಯ_fg_color > 0)
	{
	    cterm_color2vterm(cterm_ಸಾಮಾನ್ಯ_fg_color - 1, fg);
#ಆದರೆ defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
# ifdef VIMDLL
	    ಆದರೆ (!gui.ಒಳ_use)
# endif
	    {
		tmp = fg->red;
		fg->red = fg->blue;
		fg->blue = tmp;
	    }
#ಆದರೆ_ಅಂತ್ಯ
	}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMRESPONSE
	ಇಲ್ಲದಿದ್ದರೆ
	    ಪದ_get_fg_color(&fg->red, &fg->green, &fg->blue);
#ಆದರೆ_ಅಂತ್ಯ

	ಆದರೆ (cterm_ಸಾಮಾನ್ಯ_bg_color > 0)
	{
	    cterm_color2vterm(cterm_ಸಾಮಾನ್ಯ_bg_color - 1, bg);
#ಆದರೆ defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
# ifdef VIMDLL
	    ಆದರೆ (!gui.ಒಳ_use)
# endif
	    {
		tmp = fg->red;
		fg->red = fg->blue;
		fg->blue = tmp;
	    }
#ಆದರೆ_ಅಂತ್ಯ
	}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMRESPONSE
	ಇಲ್ಲದಿದ್ದರೆ
	    ಪದ_get_bg_color(&bg->red, &bg->green, &bg->blue);
#ಆದರೆ_ಅಂತ್ಯ
    }
}

#ಆದರೆ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * Return TRUE if the user-defined palette (either g:terminal_ansi_colors or the
 * "ansi_colors" argument in term_start()) shall be applied.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಪದ_use_palette(ಶೂನ್ಯ)
{
    ಆದರೆ (0
# ifdef FEAT_GUI
	    || gui.ಒಳ_use
# endif
# ifdef FEAT_TERMGUICOLORS
	    || p_tgc
# endif
       )
	ಹಿಂತಿರುಗಿ TRUE;
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Set the 16 ANSI colors from array of RGB values
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಗಣ_vterm_palette(VTerm *vterm, long_u *rgb)
{
    ಪೂರ್ಣಾಂಕ		ಸೂಚ್ಯಂಕ = 0;
    VTermState	*state = vterm_obtain_state(vterm);

    ಗಾಗಿ (; ಸೂಚ್ಯಂಕ < 16; ಸೂಚ್ಯಂಕ++)
    {
	VTermColor	color;

	color.type = VTERM_COLOR_RGB;
	color.red = (ಚಿಹ್ನೆರಹಿತ)(rgb[ಸೂಚ್ಯಂಕ] >> 16);
	color.green = (ಚಿಹ್ನೆರಹಿತ)(rgb[ಸೂಚ್ಯಂಕ] >> 8) & 255;
	color.blue = (ಚಿಹ್ನೆರಹಿತ)rgb[ಸೂಚ್ಯಂಕ] & 255;
	color.ಸೂಚ್ಯಂಕ = 0;
	vterm_state_ಗಣ_palette_color(state, ಸೂಚ್ಯಂಕ, &color);
    }
}

/*
 * Set the ANSI color palette from a list of colors
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಗಣ_ansi_colors_list(VTerm *vterm, list_T *list)
{
    ಪೂರ್ಣಾಂಕ		n = 0;
    long_u	rgb[16];
    listitem_T	*li;

    ಗಾಗಿ (li = list->lv_ಮೊದಲ; li != ಖಾಲಿ && n < 16; li = li->li_ಮುಂದಿನ, n++)
    {
	char_u		*color_name;
	guicolor_T	guicolor;

	color_name = tv_get_string_chk(&li->li_tv);
	ಆದರೆ (color_name == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FAIL;

	guicolor = GUI_GET_COLOR(color_name);
	ಆದರೆ (guicolor == INVALCOLOR)
	    ಹಿಂತಿರುಗಿ FAIL;

	rgb[n] = GUI_MCH_GET_RGB(guicolor);
    }

    ಆದರೆ (n != 16 || li != ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    ಗಣ_vterm_palette(vterm, rgb);

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Initialize the ANSI color palette from g:terminal_ansi_colors[0:15]
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
init_vterm_ansi_colors(VTerm *vterm)
{
    dictitem_T	*var = ಕಂಡುಹಿಡಿ_var((char_u *)"g:terminal_ansi_colors", ಖಾಲಿ, TRUE);

    ಆದರೆ (var == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (var->di_tv.v_type != VAR_LIST
	    || var->di_tv.vval.v_list == ಖಾಲಿ
	    || var->di_tv.vval.v_list->lv_ಮೊದಲ == &range_list_ಅಂಶ
	    || ಗಣ_ansi_colors_list(vterm, var->di_tv.vval.v_list) == FAIL)
	semsg(_(e_invalid_ವಾದ_str), "g:terminal_ansi_colors");
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Handles a "drop" command from the job in the terminal.
 * "item" is the file name, "item->li_next" may have options.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
handle_drop_command(listitem_T *ಅಂಶ)
{
    char_u	*fname = tv_get_string(&ಅಂಶ->li_tv);
    listitem_T	*opt_ಅಂಶ = ಅಂಶ->li_ಮುಂದಿನ;
    ಪೂರ್ಣಾಂಕ		bufnr;
    win_T	*wp;
    tabpage_T   *tp;
    exarg_T	ea;
    char_u	*tofree = ಖಾಲಿ;

    bufnr = buflist_ಕೂಡಿಸು(fname, BLN_LISTED | BLN_NOOPT);
    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	ಆದರೆ (wp->w_ಬಫರ್->b_fnum == bufnr)
	{
	    // buffer is in a window already, go there
	    goto_tabpage_win(tp, wp);
	    ಹಿಂತಿರುಗಿ;
	}
    }

    CLEAR_ಕ್ಷೇತ್ರ(ea);

    ಆದರೆ (opt_ಅಂಶ != ಖಾಲಿ && opt_ಅಂಶ->li_tv.v_type == VAR_DICT
					&& opt_ಅಂಶ->li_tv.vval.v_dict != ಖಾಲಿ)
    {
	dict_T *dict = opt_ಅಂಶ->li_tv.vval.v_dict;
	char_u *p;

	p = dict_get_string(dict, "ff", FALSE);
	ಆದರೆ (p == ಖಾಲಿ)
	    p = dict_get_string(dict, "fileformat", FALSE);
	ಆದರೆ (p != ಖಾಲಿ)
	{
	    ಆದರೆ (check_ff_ಮೌಲ್ಯ(p) == FAIL)
		ch_log(ಖಾಲಿ, "Invalid ff argument to drop: %s", p);
	    ಇಲ್ಲದಿದ್ದರೆ
		ea.force_ff = *p;
	}
	p = dict_get_string(dict, "enc", FALSE);
	ಆದರೆ (p == ಖಾಲಿ)
	    p = dict_get_string(dict, "encoding", FALSE);
	ಆದರೆ (p != ಖಾಲಿ)
	{
	    ea.cmd = alloc(STRLEN(p) + 12);
	    ಆದರೆ (ea.cmd != ಖಾಲಿ)
	    {
		sprintf((ಅಕ್ಷರ *)ea.cmd, "sbuf ++enc=%s", p);
		ea.force_enc = 11;
		tofree = ea.cmd;
	    }
	}

	p = dict_get_string(dict, "bad", FALSE);
	ಆದರೆ (p != ಖಾಲಿ)
	    get_bad_opt(p, &ea);

	ಆದರೆ (dict_has_ಕೀಲಿ(dict, "bin"))
	    ea.force_bin = FORCE_BIN;
	ಆದರೆ (dict_has_ಕೀಲಿ(dict, "binary"))
	    ea.force_bin = FORCE_BIN;
	ಆದರೆ (dict_has_ಕೀಲಿ(dict, "nobin"))
	    ea.force_bin = FORCE_NOBIN;
	ಆದರೆ (dict_has_ಕೀಲಿ(dict, "nobinary"))
	    ea.force_bin = FORCE_NOBIN;
    }

    // open in new window, like ":split fname"
    ಆದರೆ (ea.cmd == ಖಾಲಿ)
	ea.cmd = (char_u *)"split";
    ea.arg = fname;
    ea.cmdidx = CMD_ವಿಭಜಿಸು;
    ex_splitview(&ea);

    vim_free(tofree);
}

/*
 * Return TRUE if "func" starts with "pat" and "pat" isn't empty.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
is_permitted_ಪದ_api(char_u *func, char_u *pat)
{
    ಹಿಂತಿರುಗಿ pat != ಖಾಲಿ && *pat != NUL && STRNICMP(func, pat, STRLEN(pat)) == 0;
}

/*
 * Handles a function call from the job running in a terminal.
 * "item" is the function name, "item->li_next" has the arguments.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
handle_call_command(ಪದ_T *ಪದ, channel_T *channel, listitem_T *ಅಂಶ)
{
    char_u	*func;
    typval_T	argvars[2];
    typval_T	rettv;
    funcexe_T	funcexe;

    ಆದರೆ (ಅಂಶ->li_ಮುಂದಿನ == ಖಾಲಿ)
    {
	ch_log(channel, "Missing function arguments for call");
	ಹಿಂತಿರುಗಿ;
    }
    func = tv_get_string(&ಅಂಶ->li_tv);

    ಆದರೆ (!is_permitted_ಪದ_api(func, ಪದ->tl_api))
    {
	ch_log(channel, "Unpermitted function: %s", func);
	ಹಿಂತಿರುಗಿ;
    }

    argvars[0].v_type = VAR_ಸಂಖ್ಯೆ;
    argvars[0].vval.v_ಸಂಖ್ಯೆ = ಪದ->tl_ಬಫರ್->b_fnum;
    argvars[1] = ಅಂಶ->li_ಮುಂದಿನ->li_tv;
    CLEAR_ಕ್ಷೇತ್ರ(funcexe);
    funcexe.fe_firstline = 1L;
    funcexe.fe_lastline = 1L;
    funcexe.fe_evaluate = TRUE;
    ಆದರೆ (call_func(func, -1, &rettv, 2, argvars, &funcexe) == OK)
    {
	clear_tv(&rettv);
	ch_log(channel, "Function %s called", func);
    }
    ಇಲ್ಲದಿದ್ದರೆ
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
    ಸ್ಥಿರ size_t
url_decode(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *src, ಸ್ಥಿರಾಂಕ size_t len, char_u *dst)
{
    size_t i = 0, j = 0;

    ಆಗ (i < len)
    {
	ಆದರೆ (src[i] == '%' && i + 2 < len)
	{
	    dst[j] = hexhex2nr((char_u *)&src[i + 1]);
	    j++;
	    i += 3;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    dst[j] = src[i];
	    i++;
	    j++;
	}
    }
    dst[j] = '\0';
    ಹಿಂತಿರುಗಿ j;
}

/*
 * Sync terminal buffer's cwd with shell's pwd with the help of OSC 7.
 *
 * The OSC 7 sequence has the format of
 * "\033]7;file://HOSTNAME/CURRENT/DIR\033\\"
 * and what VTerm provides via VTermStringFragment is
 * "file://HOSTNAME/CURRENT/DIR"
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
sync_shell_dir(garray_T *gap)
{
    ಪೂರ್ಣಾಂಕ       ಸ್ಥಾನಾಂತರ = 7;  // len of "file://" is 7
    ಅಕ್ಷರ      *pos = (ಅಕ್ಷರ *)gap->ga_ದತ್ತಾಂಶ + ಸ್ಥಾನಾಂತರ;
    char_u    *new_dir;

    // remove HOSTNAME to get PWD
    ಆಗ (ಸ್ಥಾನಾಂತರ < (ಪೂರ್ಣಾಂಕ)gap->ga_len && *pos != '/' )
    {
	++ಸ್ಥಾನಾಂತರ;
	++pos;
    }

    ಆದರೆ (ಸ್ಥಾನಾಂತರ >= (ಪೂರ್ಣಾಂಕ)gap->ga_len)
    {
	semsg(_(e_failed_ಗಮ್ಯ_ಹೊರತೆಗೆ_pwd_from_str_check_your_shell_config),
								 gap->ga_ದತ್ತಾಂಶ);
	ಹಿಂತಿರುಗಿ;
    }

    new_dir = alloc(gap->ga_len - ಸ್ಥಾನಾಂತರ + 1);
    url_decode(pos, gap->ga_len-ಸ್ಥಾನಾಂತರ, new_dir);
    changedir_func(new_dir, TRUE, CDSCOPE_ಕಿಟಕಿ);
    vim_free(new_dir);
}

/*
 * Called by libvterm when it cannot recognize an OSC sequence.
 * We recognize a terminal API command.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
parse_osc(ಪೂರ್ಣಾಂಕ command, VTermStringFragment frag, ಶೂನ್ಯ *user)
{
    ಪದ_T	*ಪದ = (ಪದ_T *)user;
    js_read_T	reader;
    typval_T	tv;
    channel_T	*channel = ಪದ->tl_ಕೆಲಸ == ಖಾಲಿ ? ಖಾಲಿ
						    : ಪದ->tl_ಕೆಲಸ->jv_channel;
    garray_T	*gap = &ಪದ->tl_osc_buf;

    // We recognize only OSC 5 1 ; {command} and OSC 7 ; {command}
    ಆದರೆ (command != 51 && (command != 7 || !p_asd))
	ಹಿಂತಿರುಗಿ 0;

    // Concatenate what was received until the final piece is found.
    ಆದರೆ (ga_grow(gap, (ಪೂರ್ಣಾಂಕ)frag.len + 1) == FAIL)
    {
	ga_clear(gap);
	ಹಿಂತಿರುಗಿ 1;
    }
    mch_memmove((ಅಕ್ಷರ *)gap->ga_ದತ್ತಾಂಶ + gap->ga_len, frag.str, frag.len);
    gap->ga_len += (ಪೂರ್ಣಾಂಕ)frag.len;
    ಆದರೆ (!frag.final)
	ಹಿಂತಿರುಗಿ 1;

    ((ಅಕ್ಷರ *)gap->ga_ದತ್ತಾಂಶ)[gap->ga_len] = 0;

    ಆದರೆ (command == 7)
    {
	sync_shell_dir(gap);
	ga_clear(gap);
	ಹಿಂತಿರುಗಿ 1;
    }

    reader.js_buf = gap->ga_ದತ್ತಾಂಶ;
    reader.js_fill = ಖಾಲಿ;
    reader.js_ಬಳಸಿದ = 0;
    ಆದರೆ (json_decode(&reader, &tv, 0) == OK
	    && tv.v_type == VAR_LIST
	    && tv.vval.v_list != ಖಾಲಿ)
    {
	listitem_T *ಅಂಶ = tv.vval.v_list->lv_ಮೊದಲ;

	ಆದರೆ (ಅಂಶ == ಖಾಲಿ)
	    ch_log(channel, "Missing command");
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    char_u	*cmd = tv_get_string(&ಅಂಶ->li_tv);

	    // Make sure an invoked command doesn't delete the buffer (and the
	    // terminal) under our fingers.
	    ++ಪದ->tl_ಬಫರ್->b_locked;

	    ಅಂಶ = ಅಂಶ->li_ಮುಂದಿನ;
	    ಆದರೆ (ಅಂಶ == ಖಾಲಿ)
		ch_log(channel, "Missing argument for %s", cmd);
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(cmd, "drop") == 0)
		handle_drop_command(ಅಂಶ);
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(cmd, "call") == 0)
		handle_call_command(ಪದ, channel, ಅಂಶ);
	    ಇಲ್ಲದಿದ್ದರೆ
		ch_log(channel, "Invalid command received: %s", cmd);
	    --ಪದ->tl_ಬಫರ್->b_locked;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
	ch_log(channel, "Invalid JSON received");

    ga_clear(gap);
    clear_tv(&tv);
    ಹಿಂತಿರುಗಿ 1;
}

/*
 * Called by libvterm when it cannot recognize a CSI sequence.
 * We recognize the window position report.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
parse_csi(
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ  *leader ಬಳಸದ,
	ಸ್ಥಿರಾಂಕ ದೀರ್ಘ  ವಾದಗಳು[],
	ಪೂರ್ಣಾಂಕ	    argcount,
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ  *intermed ಬಳಸದ,
	ಅಕ್ಷರ	    command,
	ಶೂನ್ಯ	    *user)
{
    ಪದ_T	*ಪದ = (ಪದ_T *)user;
    ಅಕ್ಷರ	buf[100];
    ಪೂರ್ಣಾಂಕ		len;
    ಪೂರ್ಣಾಂಕ		x = 0;
    ಪೂರ್ಣಾಂಕ		y = 0;
    win_T	*wp;

    // We recognize only CSI 13 t
    ಆದರೆ (command != 't' || argcount != 1 || ವಾದಗಳು[0] != 13)
	ಹಿಂತಿರುಗಿ 0; // not handled

    // When getting the window position is not possible or it fails it results
    // in zero/zero.
#ಆದರೆ defined(FEAT_GUI) \
	|| (defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)) \
	|| defined(MSWIN)
    (ಶೂನ್ಯ)ui_get_winpos(&x, &y, (varnumber_T)100);
#ಆದರೆ_ಅಂತ್ಯ

    FOR_ALL_WINDOWS(wp)
	ಆದರೆ (wp->w_ಬಫರ್ == ಪದ->tl_ಬಫರ್)
	    ನಿಲ್ಲಿಸು;
    ಆದರೆ (wp != ಖಾಲಿ)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
	ಆದರೆ (gui.ಒಳ_use)
	{
	    x += wp->w_wincol * gui.char_ಅಗಲ;
	    y += W_WINROW(wp) * gui.char_ಎತ್ತರ;
	}
	ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	{
	    // We roughly estimate the position of the terminal window inside
	    // the Vim window by assuming a 10 x 7 character cell.
	    x += wp->w_wincol * 7;
	    y += W_WINROW(wp) * 10;
	}
    }

    len = vim_snprintf(buf, 100, "\x1b[3;%d;%dt", x, y);
    channel_send(ಪದ->tl_ಕೆಲಸ->jv_channel, get_tty_part(ಪದ),
						     (char_u *)buf, len, ಖಾಲಿ);
    ಹಿಂತಿರುಗಿ 1;
}

ಸ್ಥಿರ VTermStateFallbacks state_fallbacks = {
    ಖಾಲಿ,		// control
    parse_csi,		// csi
    parse_osc,		// osc
    ಖಾಲಿ,		// dcs
    ಖಾಲಿ,		// apc
    ಖಾಲಿ,		// pm
    ಖಾಲಿ		// sos
};

/*
 * Use Vim's allocation functions for vterm so profiling works.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ *
vterm_malloc(size_t ಗಾತ್ರ_2, ಶೂನ್ಯ *ದತ್ತಾಂಶ ಬಳಸದ)
{
    // make sure that the length is not zero
    ಹಿಂತಿರುಗಿ alloc_clear(ಗಾತ್ರ_2 == 0 ? 1L : ಗಾತ್ರ_2);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
vterm_memfree(ಶೂನ್ಯ *ptr, ಶೂನ್ಯ *ದತ್ತಾಂಶ ಬಳಸದ)
{
    vim_free(ptr);
}

ಸ್ಥಿರ VTermAllocatorFunctions vterm_allocator = {
  &vterm_malloc,
  &vterm_memfree
};

/*
 * Create a new vterm and initialize it.
 * Return FAIL when out of memory.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
create_vterm(ಪದ_T *ಪದ, ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲುಗಳು_2, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ)
{
    VTerm	    *vterm;
    VTermScreen	    *screen;
    VTermState	    *state;
    VTerm_ಮೌಲ್ಯ	    ಮೌಲ್ಯ;

    vterm = vterm_new_with_allocator(ಅಡ್ಡಸಾಲುಗಳು_2, ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ, &vterm_allocator, ಖಾಲಿ);
    ಪದ->tl_vterm = vterm;
    ಆದರೆ (vterm == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    // Allocate screen and state here, so we can bail out if that fails.
    state = vterm_obtain_state(vterm);
    screen = vterm_obtain_screen(vterm);
    ಆದರೆ (state == ಖಾಲಿ || screen == ಖಾಲಿ)
    {
	vterm_free(vterm);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    vterm_screen_ಗಣ_callbacks(screen, &screen_callbacks, ಪದ);
    vterm_screen_ಗಣ_damage_ವಿಲೀನ(screen, VTERM_DAMAGE_SCROLL);
    vterm_screen_callbacks_has_pushline4(screen);
    // TODO: depends on 'encoding'.
    vterm_ಗಣ_utf8(vterm, 1);

    ಪದ_init_default_colors(ಪದ);

    vterm_state_ಗಣ_default_colors(
	    state,
	    &ಪದ->tl_default_color.fg,
	    &ಪದ->tl_default_color.bg);

    ಆದರೆ (t_colors < 16)
	// Less than 16 colors: assume that bold means using a bright color for
	// the foreground color.
	vterm_state_ಗಣ_bold_highbright(vterm_obtain_state(vterm), 1);

    // Required to initialize most things.
    vterm_screen_reset(screen, 1 /* hard */);

    // Allow using alternate screen.
    vterm_screen_enable_altscreen(screen, 1);

    // For unix do not use a blinking cursor.  In an xterm this causes the
    // cursor to blink if it's blinking in the xterm.
    // For Windows we respect the system wide setting.
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
    ಆದರೆ (Get_Caret_Blink_ಸಮಯ() == INFINITE)
	ಮೌಲ್ಯ.boolean = 0;
    ಇಲ್ಲದಿದ್ದರೆ
	ಮೌಲ್ಯ.boolean = 1;
#ಇಲ್ಲದಿದ್ದರೆ
    ಮೌಲ್ಯ.boolean = 0;
#ಆದರೆ_ಅಂತ್ಯ
    vterm_state_ಗಣ_termprop(state, VTERM_PROP_CURSORBLINK, &ಮೌಲ್ಯ);
    vterm_state_ಗಣ_unrecognised_fallbacks(state, &state_fallbacks, ಪದ);

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Reset the terminal palette to its default value.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಪದ_reset_palette(VTerm *vterm)
{
    VTermState	*state = vterm_obtain_state(vterm);
    ಪೂರ್ಣಾಂಕ		ಸೂಚ್ಯಂಕ;

    ಗಾಗಿ (ಸೂಚ್ಯಂಕ = 0; ಸೂಚ್ಯಂಕ < 16; ಸೂಚ್ಯಂಕ++)
    {
	VTermColor	color;

	color.type = VTERM_COLOR_ಸೂಚ್ಯಂಕಿತ;
	ansi_color2rgb(ಸೂಚ್ಯಂಕ, &color.red, &color.green, &color.blue,
		&color.ಸೂಚ್ಯಂಕ);
	// The first valid index starts at 1.
	color.ಸೂಚ್ಯಂಕ -= 1;

	vterm_state_ಗಣ_palette_color(state, ಸೂಚ್ಯಂಕ, &color);
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ಪದ_update_palette(ಪದ_T *ಪದ)
{
#ಆದರೆ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    ಆದರೆ (ಪದ_use_palette()
	    && (ಪದ->tl_palette != ಖಾಲಿ
		|| ಕಂಡುಹಿಡಿ_var((char_u *)"g:terminal_ansi_colors", ಖಾಲಿ, TRUE)
								      != ಖಾಲಿ))
    {
	ಆದರೆ (ಪದ->tl_palette != ಖಾಲಿ)
	    ಗಣ_vterm_palette(ಪದ->tl_vterm, ಪದ->tl_palette);
	ಇಲ್ಲದಿದ್ದರೆ
	    init_vterm_ansi_colors(ಪದ->tl_vterm);
    }
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	ಪದ_reset_palette(ಪದ->tl_vterm);
}

/*
 * Called when option 'termguicolors' is changed.
 */
    ಶೂನ್ಯ
ಪದ_update_palette_all(ಶೂನ್ಯ)
{
    ಪದ_T *ಪದ;

    FOR_ALL_TERMS(ಪದ)
    {
	ಆದರೆ (ಪದ->tl_vterm == ಖಾಲಿ)
	    ಮುಂದುವರಿ;
	ಪದ_update_palette(ಪದ);
    }
}

/*
 * Called when option 'background' or 'termguicolors' was set,
 * or when any highlight is changed.
 */
    ಶೂನ್ಯ
ಪದ_update_colors_all(ಶೂನ್ಯ)
{
    ಪದ_T *ಪದ;

    FOR_ALL_TERMS(ಪದ)
    {
	ಆದರೆ (ಪದ->tl_vterm == ಖಾಲಿ)
	    ಮುಂದುವರಿ;
	ಪದ_init_default_colors(ಪದ);
	vterm_state_ಗಣ_default_colors(
		vterm_obtain_state(ಪದ->tl_vterm),
		&ಪದ->tl_default_color.fg,
		&ಪದ->tl_default_color.bg);
    }
}

/*
 * Return the text to show for the buffer name and status.
 */
    char_u *
ಪದ_get_ಸ್ಥಿತಿ_ಪಠ್ಯ(ಪದ_T *ಪದ)
{
    ಆದರೆ (ಪದ->tl_ಸ್ಥಿತಿ_ಪಠ್ಯ != ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಪದ->tl_ಸ್ಥಿತಿ_ಪಠ್ಯ;

    char_u *txt;
    size_t len;
    char_u *fname;

    ಆದರೆ (ಪದ->tl_ಸಾಮಾನ್ಯ_ಬಹುಲಕ)
    {
	ಆದರೆ (ಪದ_ಕೆಲಸ_running(ಪದ))
	    txt = (char_u *)_("Terminal");
	ಇಲ್ಲದಿದ್ದರೆ
	    txt = (char_u *)_("Terminal-finished");
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಪದ->tl_title != ಖಾಲಿ)
	txt = ಪದ->tl_title;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಪದ_none_open(ಪದ))
	txt = (char_u *)_("active");
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಪದ_ಕೆಲಸ_running(ಪದ))
	txt = (char_u *)_("running");
    ಇಲ್ಲದಿದ್ದರೆ
	txt = (char_u *)_("finished");
    fname = buf_get_fname(ಪದ->tl_ಬಫರ್);
    len = 9 + STRLEN(fname) + STRLEN(txt);
    ಪದ->tl_ಸ್ಥಿತಿ_ಪಠ್ಯ = alloc(len);
    ಆದರೆ (ಪದ->tl_ಸ್ಥಿತಿ_ಪಠ್ಯ != ಖಾಲಿ)
	vim_snprintf((ಅಕ್ಷರ *)ಪದ->tl_ಸ್ಥಿತಿ_ಪಠ್ಯ, len, "%s [%s]",
		fname, txt);
    ಹಿಂತಿರುಗಿ ಪದ->tl_ಸ್ಥಿತಿ_ಪಠ್ಯ;
}

/*
 * Clear the cached value of the status text.
 */
    ಶೂನ್ಯ
ಪದ_clear_ಸ್ಥಿತಿ_ಪಠ್ಯ(ಪದ_T *ಪದ)
{
    VIM_CLEAR(ಪದ->tl_ಸ್ಥಿತಿ_ಪಠ್ಯ);
}

/*
 * Mark references in jobs of terminals.
 */
    ಪೂರ್ಣಾಂಕ
ಗಣ_ref_ಒಳ_ಪದ(ಪೂರ್ಣಾಂಕ copyID)
{
    ಪೂರ್ಣಾಂಕ		abort = FALSE;
    ಪದ_T	*ಪದ;
    typval_T	tv;

    ಗಾಗಿ (ಪದ = ಮೊದಲ_ಪದ; !abort && ಪದ != ಖಾಲಿ; ಪದ = ಪದ->tl_ಮುಂದಿನ)
	ಆದರೆ (ಪದ->tl_ಕೆಲಸ != ಖಾಲಿ)
	{
	    tv.v_type = VAR_ಕೆಲಸ;
	    tv.vval.v_ಕೆಲಸ = ಪದ->tl_ಕೆಲಸ;
	    abort = abort || ಗಣ_ref_ಒಳ_ಅಂಶ(&tv, copyID, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ);
	}
    ಹಿಂತಿರುಗಿ abort;
}

/*
 * Get the buffer from the first argument in "argvars".
 * Returns NULL when the buffer is not for a terminal window and logs a message
 * with "where".
 */
    ಸ್ಥಿರ buf_T *
ಪದ_get_buf(typval_T *argvars, ಅಕ್ಷರ *where)
{
    buf_T *buf;

    ++emsg_off;
    buf = tv_get_buf(&argvars[0], FALSE);
    --emsg_off;
    ಆದರೆ (buf == ಖಾಲಿ || buf->b_ಪದ == ಖಾಲಿ)
    {
	(ಶೂನ್ಯ)tv_get_ಸಂಖ್ಯೆ(&argvars[0]);    // issue errmsg if type error
	ch_log(ಖಾಲಿ, "%s: invalid buffer argument", where);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಹಿಂತಿರುಗಿ buf;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
clear_cell(VTermScreenCell *cell)
{
    CLEAR_ಕ್ಷೇತ್ರ(*cell);
    cell->fg.type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_FG;
    cell->bg.type = VTERM_COLOR_INVALID | VTERM_COLOR_DEFAULT_BG;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
dump_ಪದ_color(FILE *fd, VTermColor *color)
{
    ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ;

    ಆದರೆ (VTERM_COLOR_IS_ಸೂಚ್ಯಂಕಿತ(color))
	ಸೂಚ್ಯಂಕ = color->ಸೂಚ್ಯಂಕ + 1;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (color->type == 0)
	// use RGB values
	ಸೂಚ್ಯಂಕ = 255;
    ಇಲ್ಲದಿದ್ದರೆ
	// default color
	ಸೂಚ್ಯಂಕ = 0;
    fprintf(fd, "%02x%02x%02x%d",
	    (ಪೂರ್ಣಾಂಕ)color->red, (ಪೂರ್ಣಾಂಕ)color->green, (ಪೂರ್ಣಾಂಕ)color->blue,
	    ಸೂಚ್ಯಂಕ);
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
    ಶೂನ್ಯ
f_ಪದ_dumpwrite(typval_T *argvars, typval_T *rettv ಬಳಸದ)
{
    buf_T	*buf;
    ಪದ_T	*ಪದ;
    char_u	*fname;
    ಪೂರ್ಣಾಂಕ		max_ಎತ್ತರ_2 = 0;
    ಪೂರ್ಣಾಂಕ		max_ಅಗಲ_2 = 0;
    stat_T	st;
    FILE	*fd;
    VTermPos	pos;
    VTermScreen *screen;
    VTermScreenCell prev_cell;
    VTermState	*state;
    VTermPos	cursor_pos;

    ಆದರೆ (check_restricted() || check_secure())
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_ಬಫರ್_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	ಹಿಂತಿರುಗಿ;

    buf = ಪದ_get_buf(argvars, "term_dumpwrite()");
    ಆದರೆ (buf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    ಪದ = buf->b_ಪದ;
    ಆದರೆ (ಪದ->tl_vterm == ಖಾಲಿ)
    {
	emsg(_(e_ಕೆಲಸ_already_ಪೂರ್ಣಗೊಂಡ));
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (argvars[2].v_type != VAR_UNKNOWN)
    {
	dict_T *d;

	ಆದರೆ (check_for_dict_arg(argvars, 2) == FAIL)
	    ಹಿಂತಿರುಗಿ;
	d = argvars[2].vval.v_dict;
	ಆದರೆ (d != ಖಾಲಿ)
	{
	    max_ಎತ್ತರ_2 = dict_get_ಸಂಖ್ಯೆ(d, "rows");
	    max_ಅಗಲ_2 = dict_get_ಸಂಖ್ಯೆ(d, "columns");
	}
    }

    fname = tv_get_string_chk(&argvars[1]);
    ಆದರೆ (fname == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (mch_stat((ಅಕ್ಷರ *)fname, &st) >= 0)
    {
	semsg(_(e_file_exists_str), fname);
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (*fname == NUL || (fd = mch_fopen((ಅಕ್ಷರ *)fname, WRITEBIN)) == ಖಾಲಿ)
    {
	semsg(_(e_cant_create_file_str), *fname == NUL ? (char_u *)_("<empty>") : fname);
	ಹಿಂತಿರುಗಿ;
    }

    clear_cell(&prev_cell);

    screen = vterm_obtain_screen(ಪದ->tl_vterm);
    state = vterm_obtain_state(ಪದ->tl_vterm);
    vterm_state_get_cursorpos(state, &cursor_pos);

    ಗಾಗಿ (pos.ಅಡ್ಡಸಾಲು = 0; (max_ಎತ್ತರ_2 == 0 || pos.ಅಡ್ಡಸಾಲು < max_ಎತ್ತರ_2)
					 && pos.ಅಡ್ಡಸಾಲು < ಪದ->tl_ಅಡ್ಡಸಾಲುಗಳು; ++pos.ಅಡ್ಡಸಾಲು)
    {
	ಪೂರ್ಣಾಂಕ		repeat = 0;

	ಗಾಗಿ (pos.ಲಂಬಸಾಲು = 0; (max_ಅಗಲ_2 == 0 || pos.ಲಂಬಸಾಲು < max_ಅಗಲ_2)
					 && pos.ಲಂಬಸಾಲು < ಪದ->tl_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ; ++pos.ಲಂಬಸಾಲು)
	{
	    VTermScreenCell cell;
	    ಪೂರ್ಣಾಂಕ		    same_attr;
	    ಪೂರ್ಣಾಂಕ		    same_chars = TRUE;
	    ಪೂರ್ಣಾಂಕ		    i;
	    ಪೂರ್ಣಾಂಕ		    is_cursor_pos = (pos.ಲಂಬಸಾಲು == cursor_pos.ಲಂಬಸಾಲು
						 && pos.ಅಡ್ಡಸಾಲು == cursor_pos.ಅಡ್ಡಸಾಲು);

	    ಆದರೆ (vterm_screen_get_cell(screen, pos, &cell) == 0)
		clear_cell(&cell);

	    ಗಾಗಿ (i = 0; i < VTERM_MAX_CHARS_ಪ್ರತಿ_CELL; ++i)
	    {
		ಪೂರ್ಣಾಂಕ c = cell.chars[i];
		ಪೂರ್ಣಾಂಕ pc = prev_cell.chars[i];
		ಪೂರ್ಣಾಂಕ should_break = c == NUL || pc == NUL;

		// For the first character NUL is the same as space.
		ಆದರೆ (i == 0)
		{
		    c = (c == NUL) ? ' ' : c;
		    pc = (pc == NUL) ? ' ' : pc;
		}
		ಆದರೆ (c != pc)
		    same_chars = FALSE;
		ಆದರೆ (should_break)
		    ನಿಲ್ಲಿಸು;
	    }
	    same_attr = vtermAttr2hl(&cell.attrs)
					      == vtermAttr2hl(&prev_cell.attrs)
			&& vterm_color_is_equal(&cell.fg, &prev_cell.fg)
			&& vterm_color_is_equal(&cell.bg, &prev_cell.bg);
	    ಆದರೆ (same_chars && cell.ಅಗಲ == prev_cell.ಅಗಲ && same_attr
							     && !is_cursor_pos)
	    {
		++repeat;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಆದರೆ (repeat > 0)
		{
		    fprintf(fd, "@%d", repeat);
		    repeat = 0;
		}
		fputs(is_cursor_pos ? ">" : "|", fd);

		ಆದರೆ (cell.chars[0] == NUL)
		    fputs(" ", fd);
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    char_u	charbuf[10];
		    ಪೂರ್ಣಾಂಕ		len;

		    ಗಾಗಿ (i = 0; i < VTERM_MAX_CHARS_ಪ್ರತಿ_CELL
						  && cell.chars[i] != NUL; ++i)
		    {
			len = utf_char2bytes(cell.chars[i], charbuf);
			fwrite(charbuf, len, 1, fd);
		    }
		}

		// When only the characters differ we don't write anything, the
		// following "|", "@" or NL will indicate using the same
		// attributes.
		ಆದರೆ (cell.ಅಗಲ != prev_cell.ಅಗಲ || !same_attr)
		{
		    ಆದರೆ (cell.ಅಗಲ == 2)
			fputs("*", fd);
		    ಇಲ್ಲದಿದ್ದರೆ
			fputs("+", fd);

		    ಆದರೆ (same_attr)
		    {
			fputs("&", fd);
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			fprintf(fd, "%d", vtermAttr2hl(&cell.attrs));
			ಆದರೆ (vterm_color_is_equal(&cell.fg, &prev_cell.fg))
			    fputs("&", fd);
			ಇಲ್ಲದಿದ್ದರೆ
			{
			    fputs("#", fd);
			    dump_ಪದ_color(fd, &cell.fg);
			}
			ಆದರೆ (vterm_color_is_equal(&cell.bg, &prev_cell.bg))
			    fputs("&", fd);
			ಇಲ್ಲದಿದ್ದರೆ
			{
			    fputs("#", fd);
			    dump_ಪದ_color(fd, &cell.bg);
			}
		    }
		}

		prev_cell = cell;
	    }

	    ಆದರೆ (cell.ಅಗಲ == 2)
		++pos.ಲಂಬಸಾಲು;
	}
	ಆದರೆ (repeat > 0)
	    fprintf(fd, "@%d", repeat);
	fputs("\n", fd);
    }

    fclose(fd);
}

/*
 * Called when a dump is corrupted.  Put a breakpoint here when debugging.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
dump_is_corrupt(garray_T *gap)
{
    GA_CONCAT_LITERAL(gap, "CORRUPT");
}

    ಸ್ಥಿರ ಶೂನ್ಯ
append_cell(garray_T *gap, cellattr_T *cell)
{
    ಆದರೆ (ga_grow(gap, 1) == FAIL)
	ಹಿಂತಿರುಗಿ;

    *(((cellattr_T *)gap->ga_ದತ್ತಾಂಶ) + gap->ga_len) = *cell;
    ++gap->ga_len;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
clear_cellattr(cellattr_T *cell)
{
    CLEAR_ಕ್ಷೇತ್ರ(*cell);
    cell->fg.type = VTERM_COLOR_DEFAULT_FG;
    cell->bg.type = VTERM_COLOR_DEFAULT_BG;
}

/*
 * Read the dump file from "fd" and append lines to the current buffer.
 * Return the cell width of the longest line.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
read_dump_file(FILE *fd, VTermPos *cursor_pos)
{
    ಪೂರ್ಣಾಂಕ		    c;
    garray_T	    ga_ಪಠ್ಯ;
    garray_T	    ga_cell;
    string_T	    prev_char = {ಖಾಲಿ, 0};
    ಪೂರ್ಣಾಂಕ		    attr = 0;
    cellattr_T	    cell;
    cellattr_T	    ಖಾಲಿಗಳು_cell;
    ಪದ_T	    *ಪದ = curbuf->b_ಪದ;
    ಪೂರ್ಣಾಂಕ		    max_cells = 0;
    ಪೂರ್ಣಾಂಕ		    ಆರಂಭ_ಅಡ್ಡಸಾಲು = ಪದ->tl_scrollback.ga_len;

    ga_init2(&ga_ಪಠ್ಯ, 1, 90);
    ga_init2(&ga_cell, ಗಾತ್ರ(cellattr_T), 90);
    clear_cellattr(&cell);
    clear_cellattr(&ಖಾಲಿಗಳು_cell);
    cursor_pos->ಅಡ್ಡಸಾಲು = -1;
    cursor_pos->ಲಂಬಸಾಲು = -1;

    c = fgetc(fd);
    ಗಾಗಿ (;;)
    {
	ಆದರೆ (c == EOF)
	    ನಿಲ್ಲಿಸು;
	ಆದರೆ (c == '\r')
	{
	    // DOS line endings?  Ignore.
	    c = fgetc(fd);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == '\n')
	{
	    // End of a line: append it to the buffer.
	    ಆದರೆ (ga_ಪಠ್ಯ.ga_ದತ್ತಾಂಶ == ಖಾಲಿ)
		dump_is_corrupt(&ga_ಪಠ್ಯ);
	    ಆದರೆ (ga_grow(&ಪದ->tl_scrollback, 1) == OK)
	    {
		sb_ಸಾಲು_T   *ಸಾಲು = (sb_ಸಾಲು_T *)ಪದ->tl_scrollback.ga_ದತ್ತಾಂಶ
						  + ಪದ->tl_scrollback.ga_len;

		ಆದರೆ (max_cells < ga_cell.ga_len)
		    max_cells = ga_cell.ga_len;
		ಸಾಲು->sb_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = ga_cell.ga_len;
		ಸಾಲು->sb_ಬೈಟುಗಳು = ga_ಪಠ್ಯ.ga_len;
		ಸಾಲು->sb_cells = ga_cell.ga_ದತ್ತಾಂಶ;
		ಸಾಲು->sb_fill_attr = ಪದ->tl_default_color;
		ಸಾಲು->continuation = 0;
		++ಪದ->tl_scrollback.ga_len;
		ga_init(&ga_cell);

		ga_append(&ga_ಪಠ್ಯ, NUL);
		ml_append(curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ, ga_ಪಠ್ಯ.ga_ದತ್ತಾಂಶ,
							ga_ಪಠ್ಯ.ga_len, FALSE);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		ga_clear(&ga_cell);
	    ga_ಪಠ್ಯ.ga_len = 0;

	    c = fgetc(fd);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == '|' || c == '>')
	{
	    ಪೂರ್ಣಾಂಕ prev_len = ga_ಪಠ್ಯ.ga_len;

	    ಆದರೆ (c == '>')
	    {
		ಆದರೆ (cursor_pos->ಅಡ್ಡಸಾಲು != -1)
		    dump_is_corrupt(&ga_ಪಠ್ಯ);	// duplicate cursor
		cursor_pos->ಅಡ್ಡಸಾಲು = ಪದ->tl_scrollback.ga_len - ಆರಂಭ_ಅಡ್ಡಸಾಲು;
		cursor_pos->ಲಂಬಸಾಲು = ga_cell.ga_len;
	    }

	    // normal character(s) followed by "+", "*", "|", "@" or NL
	    c = fgetc(fd);
	    ಆದರೆ (c != EOF)
		ga_append(&ga_ಪಠ್ಯ, c);
	    ಗಾಗಿ (;;)
	    {
		c = fgetc(fd);
		ಆದರೆ (c == '+' || c == '*' || c == '|' || c == '>' || c == '@'
						      || c == EOF || c == '\n')
		    ನಿಲ್ಲಿಸು;
		ga_append(&ga_ಪಠ್ಯ, c);
	    }

	    // save the character for repeating it
	    VIM_CLEAR_STRING(prev_char);
	    ಆದರೆ (ga_ಪಠ್ಯ.ga_ದತ್ತಾಂಶ != ಖಾಲಿ)
	    {
		prev_char.ಉದ್ದಳತೆ = (size_t)(ga_ಪಠ್ಯ.ga_len - prev_len);
		prev_char.string = vim_strnsave(
		    ((char_u *)ga_ಪಠ್ಯ.ga_ದತ್ತಾಂಶ) + prev_len,
		    prev_char.ಉದ್ದಳತೆ);
		ಆದರೆ (prev_char.string == ಖಾಲಿ)
		    prev_char.ಉದ್ದಳತೆ = 0;
	    }

	    ಆದರೆ (c == '@' || c == '|' || c == '>' || c == '\n')
	    {
		// use all attributes from previous cell
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == '+' || c == '*')
	    {
		ಪೂರ್ಣಾಂಕ is_bg;

		cell.ಅಗಲ = c == '+' ? 1 : 2;

		c = fgetc(fd);
		ಆದರೆ (c == '&')
		{
		    // use same attr as previous cell
		    c = fgetc(fd);
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಸುರಕ್ಷಿತ_isdigit(c))
		{
		    // get the decimal attribute
		    attr = 0;
		    ಆಗ (ಸುರಕ್ಷಿತ_isdigit(c))
		    {
			attr = attr * 10 + (c - '0');
			c = fgetc(fd);
		    }
		    hl2vtermAttr(attr, &cell);

		    // is_bg == 0: fg, is_bg == 1: bg
		    ಗಾಗಿ (is_bg = 0; is_bg <= 1; ++is_bg)
		    {
			ಆದರೆ (c == '&')
			{
			    // use same color as previous cell
			    c = fgetc(fd);
			}
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == '#')
			{
			    ಪೂರ್ಣಾಂಕ red, green, blue, ಸೂಚ್ಯಂಕ = 0, type;

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
			    ಆದರೆ (!ಸುರಕ್ಷಿತ_isdigit(c))
				dump_is_corrupt(&ga_ಪಠ್ಯ);
			    ಆಗ (ಸುರಕ್ಷಿತ_isdigit(c))
			    {
				ಸೂಚ್ಯಂಕ = ಸೂಚ್ಯಂಕ * 10 + (c - '0');
				c = fgetc(fd);
			    }
			    ಆದರೆ (ಸೂಚ್ಯಂಕ == 0 || ಸೂಚ್ಯಂಕ == 255)
			    {
				type = VTERM_COLOR_RGB;
				ಆದರೆ (ಸೂಚ್ಯಂಕ == 0)
				{
				    ಆದರೆ (is_bg)
					type |= VTERM_COLOR_DEFAULT_BG;
				    ಇಲ್ಲದಿದ್ದರೆ
					type |= VTERM_COLOR_DEFAULT_FG;
				}
			    }
			    ಇಲ್ಲದಿದ್ದರೆ
			    {
				type = VTERM_COLOR_ಸೂಚ್ಯಂಕಿತ;
				ಸೂಚ್ಯಂಕ -= 1;
			    }
			    ಆದರೆ (is_bg)
			    {
				cell.bg.type = type;
				cell.bg.red = red;
				cell.bg.green = green;
				cell.bg.blue = blue;
				cell.bg.ಸೂಚ್ಯಂಕ = ಸೂಚ್ಯಂಕ;
			    }
			    ಇಲ್ಲದಿದ್ದರೆ
			    {
				cell.fg.type = type;
				cell.fg.red = red;
				cell.fg.green = green;
				cell.fg.blue = blue;
				cell.fg.ಸೂಚ್ಯಂಕ = ಸೂಚ್ಯಂಕ;
			    }
			}
			ಇಲ್ಲದಿದ್ದರೆ
			    dump_is_corrupt(&ga_ಪಠ್ಯ);
		    }
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    dump_is_corrupt(&ga_ಪಠ್ಯ);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		dump_is_corrupt(&ga_ಪಠ್ಯ);

	    append_cell(&ga_cell, &cell);
	    ಆದರೆ (cell.ಅಗಲ == 2)
		append_cell(&ga_cell, &ಖಾಲಿಗಳು_cell);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == '@')
	{
	    ಆದರೆ (prev_char.string == ಖಾಲಿ)
		dump_is_corrupt(&ga_ಪಠ್ಯ);
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2 = 0;

		// repeat previous character, get the count
		ಗಾಗಿ (;;)
		{
		    c = fgetc(fd);
		    ಆದರೆ (!ಸುರಕ್ಷಿತ_isdigit(c))
			ನಿಲ್ಲಿಸು;
		    ಎಣಿಕೆ_2 = ಎಣಿಕೆ_2 * 10 + (c - '0');
		}

		ಆಗ (ಎಣಿಕೆ_2-- > 0)
		{
		    ga_concat_len(&ga_ಪಠ್ಯ, prev_char.string, prev_char.ಉದ್ದಳತೆ);
		    append_cell(&ga_cell, &cell);
		}
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    dump_is_corrupt(&ga_ಪಠ್ಯ);
	    c = fgetc(fd);
	}
    }

    ಆದರೆ (ga_ಪಠ್ಯ.ga_len > 0)
    {
	// trailing characters after last NL
	dump_is_corrupt(&ga_ಪಠ್ಯ);
	ga_append(&ga_ಪಠ್ಯ, NUL);
	ml_append(curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ, ga_ಪಠ್ಯ.ga_ದತ್ತಾಂಶ,
							ga_ಪಠ್ಯ.ga_len, FALSE);
    }

    ga_clear(&ga_ಪಠ್ಯ);
    ga_clear(&ga_cell);
    vim_free(prev_char.string);

    ಹಿಂತಿರುಗಿ max_cells;
}

/*
 * Return an allocated string with at least "text_width" "=" characters and
 * "fname" inserted in the middle.
 */
    ಸ್ಥಿರ char_u *
get_separator(ಪೂರ್ಣಾಂಕ ಪಠ್ಯ_ಅಗಲ, char_u *fname)
{
    ಪೂರ್ಣಾಂಕ	    ಅಗಲ = MAX(ಪಠ್ಯ_ಅಗಲ, curwin->w_ಅಗಲ);
    char_u  *textline;
    ಪೂರ್ಣಾಂಕ	    fname_ಗಾತ್ರ;
    char_u  *p = fname;
    ಪೂರ್ಣಾಂಕ	    i;
    size_t  off;

    textline = alloc(ಅಗಲ + STRLEN(fname) + 1);
    ಆದರೆ (textline == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    fname_ಗಾತ್ರ = vim_strsize(fname);
    ಆದರೆ (fname_ಗಾತ್ರ < ಅಗಲ - 8)
    {
	// enough room, don't use the full window width
	ಅಗಲ = MAX(ಪಠ್ಯ_ಅಗಲ, fname_ಗಾತ್ರ + 8);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fname_ಗಾತ್ರ > ಅಗಲ - 8)
    {
	// full name doesn't fit, use only the tail
	p = gettail(fname);
	fname_ಗಾತ್ರ = vim_strsize(p);
    }
    // skip characters until the name fits
    ಆಗ (fname_ಗಾತ್ರ > ಅಗಲ - 8)
    {
	p += (*mb_ptr2len)(p);
	fname_ಗಾತ್ರ = vim_strsize(p);
    }

    ಗಾಗಿ (i = 0; i < (ಅಗಲ - fname_ಗಾತ್ರ) / 2 - 1; ++i)
	textline[i] = '=';
    textline[i++] = ' ';

    STRCPY(textline + i, p);
    off = STRLEN(textline);
    textline[off] = ' ';
    ಗಾಗಿ (i = 1; i < (ಅಗಲ - fname_ಗಾತ್ರ) / 2; ++i)
	textline[off + i] = '=';
    textline[off + i] = NUL;

    ಹಿಂತಿರುಗಿ textline;
}

/*
 * Common for "term_dumpdiff()" and "term_dumpload()".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಪದ_ತುಂಬು_dump(typval_T *argvars, typval_T *rettv, ಪೂರ್ಣಾಂಕ do_diff)
{
    jobopt_T	opt;
    buf_T	*buf = ಖಾಲಿ;
    char_u	buf1[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    char_u	*fname1;
    char_u	*fname2 = ಖಾಲಿ;
    char_u	*fname_tofree = ಖಾಲಿ;
    FILE	*fd1;
    FILE	*fd2 = ಖಾಲಿ;
    char_u	*textline = ಖಾಲಿ;

    // First open the files.  If this fails bail out.
    fname1 = tv_get_string_buf_chk(&argvars[0], buf1);
    ಆದರೆ (do_diff)
	fname2 = tv_get_string_buf_chk(&argvars[1], buf2);
    ಆದರೆ (fname1 == ಖಾಲಿ || (do_diff && fname2 == ಖಾಲಿ))
    {
	emsg(_(e_invalid_ವಾದ));
	ಹಿಂತಿರುಗಿ;
    }
    fd1 = mch_fopen((ಅಕ್ಷರ *)fname1, READBIN);
    ಆದರೆ (fd1 == ಖಾಲಿ)
    {
	semsg(_(e_cant_read_file_str), fname1);
	ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (do_diff)
    {
	fd2 = mch_fopen((ಅಕ್ಷರ *)fname2, READBIN);
	ಆದರೆ (fd2 == ಖಾಲಿ)
	{
	    fclose(fd1);
	    semsg(_(e_cant_read_file_str), fname2);
	    ಹಿಂತಿರುಗಿ;
	}
    }

    init_ಕೆಲಸ_options(&opt);
    ಆದರೆ (argvars[do_diff ? 2 : 1].v_type != VAR_UNKNOWN
	    && get_ಕೆಲಸ_options(&argvars[do_diff ? 2 : 1], &opt, 0,
		    JO2_ಪದ_NAME + JO2_ಪದ_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ + JO2_ಪದ_ಅಡ್ಡಸಾಲುಗಳು
		    + JO2_VERTICAL + JO2_CURWIN + JO2_NORESTORE) == FAIL)
	ಹೋಗು theend;

    ಆದರೆ (opt.jo_ಪದ_name == ಖಾಲಿ)
    {
	size_t len = STRLEN(fname1) + 12;

	fname_tofree = alloc(len);
	ಆದರೆ (fname_tofree != ಖಾಲಿ)
	{
	    vim_snprintf((ಅಕ್ಷರ *)fname_tofree, len, "dump diff %s", fname1);
	    opt.jo_ಪದ_name = fname_tofree;
	}
    }

    ಆದರೆ (opt.jo_bufnr_buf != ಖಾಲಿ)
    {
	win_T *wp = buf_jump_open_win(opt.jo_bufnr_buf);

	// With "bufnr" argument: enter the window with this buffer and make it
	// empty.
	ಆದರೆ (wp == ಖಾಲಿ)
	    semsg(_(e_invalid_ವಾದ_str), "bufnr");
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    buf = curbuf;
	    ಆಗ (!(curbuf->b_ml.ml_flags & ML_ಖಾಲಿಗಳು))
		ml_delete((linenr_T)1);
	    free_scrollback(curbuf->b_ಪದ);
	    redraw_later(UPD_NOT_VALID);
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
	// Create a new terminal window.
	buf = ಪದ_ಆರಂಭ(&argvars[0], ಖಾಲಿ, &opt, ಪದ_ಆರಂಭ_NOJOB);

    ಆದರೆ (buf != ಖಾಲಿ && buf->b_ಪದ != ಖಾಲಿ)
    {
	ಪೂರ್ಣಾಂಕ		i;
	linenr_T	bot_lnum;
	linenr_T	lnum;
	ಪದ_T		*ಪದ = buf->b_ಪದ;
	ಪೂರ್ಣಾಂಕ		ಅಗಲ;
	ಪೂರ್ಣಾಂಕ		width2;
	VTermPos	cursor_pos1;
	VTermPos	cursor_pos2;

	ಪದ_init_default_colors(ಪದ);

	rettv->vval.v_ಸಂಖ್ಯೆ = buf->b_fnum;

	// read the files, fill the buffer with the diff
	ಅಗಲ = read_dump_file(fd1, &cursor_pos1);

	// position the cursor
	ಆದರೆ (cursor_pos1.ಅಡ್ಡಸಾಲು >= 0)
	{
	    curwin->w_cursor.lnum = cursor_pos1.ಅಡ್ಡಸಾಲು + 1;
	    coladvance(cursor_pos1.ಲಂಬಸಾಲು);
	}

	// Delete the empty line that was in the empty buffer.
	ml_delete(1);

	// For term_dumpload() we are done here.
	ಆದರೆ (!do_diff)
	    ಹೋಗು theend;

	ಪದ->tl_ತುದಿ_diff_ಅಡ್ಡಸಾಲುಗಳು = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;

	textline = get_separator(ಅಗಲ, fname1);
	ಆದರೆ (textline == ಖಾಲಿ)
	    ಹೋಗು theend;
	ಆದರೆ (ಕೂಡಿಸು_ಖಾಲಿಗಳು_scrollback(ಪದ, &ಪದ->tl_default_color, 0) == OK)
	    ml_append(curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ, textline, 0, FALSE);
	vim_free(textline);

	textline = get_separator(ಅಗಲ, fname2);
	ಆದರೆ (textline == ಖಾಲಿ)
	    ಹೋಗು theend;
	ಆದರೆ (ಕೂಡಿಸು_ಖಾಲಿಗಳು_scrollback(ಪದ, &ಪದ->tl_default_color, 0) == OK)
	    ml_append(curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ, textline, 0, FALSE);
	textline[ಅಗಲ] = NUL;

	bot_lnum = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
	width2 = read_dump_file(fd2, &cursor_pos2);
	ಆದರೆ (width2 > ಅಗಲ)
	{
	    vim_free(textline);
	    textline = alloc(width2 + 1);
	    ಆದರೆ (textline == ಖಾಲಿ)
		ಹೋಗು theend;
	    ಅಗಲ = width2;
	    textline[ಅಗಲ] = NUL;
	}
	ಪದ->tl_bot_diff_ಅಡ್ಡಸಾಲುಗಳು = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ - bot_lnum;

	ಗಾಗಿ (lnum = 1; lnum <= ಪದ->tl_ತುದಿ_diff_ಅಡ್ಡಸಾಲುಗಳು; ++lnum)
	{
	    ಆದರೆ (lnum + bot_lnum > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	    {
		// bottom part has fewer rows, fill with "-"
		ಗಾಗಿ (i = 0; i < ಅಗಲ; ++i)
		    textline[i] = '-';
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		char_u *line1;
		char_u *line2;
		char_u *p1;
		char_u *p2;
		ಪೂರ್ಣಾಂಕ	ಲಂಬಸಾಲು;
		sb_ಸಾಲು_T   *sb_ಸಾಲು = (sb_ಸಾಲು_T *)ಪದ->tl_scrollback.ga_ದತ್ತಾಂಶ;
		cellattr_T *cellattr1 = (sb_ಸಾಲು + lnum - 1)->sb_cells;
		cellattr_T *cellattr2 = (sb_ಸಾಲು + lnum + bot_lnum - 1)
								    ->sb_cells;

		// Make a copy, getting the second line will invalidate it.
		line1 = vim_strsave(ml_get(lnum));
		ಆದರೆ (line1 == ಖಾಲಿ)
		    ನಿಲ್ಲಿಸು;
		p1 = line1;

		line2 = ml_get(lnum + bot_lnum);
		p2 = line2;
		ಗಾಗಿ (ಲಂಬಸಾಲು = 0; ಲಂಬಸಾಲು < ಅಗಲ && *p1 != NUL && *p2 != NUL; ++ಲಂಬಸಾಲು)
		{
		    ಪೂರ್ಣಾಂಕ len1 = utfc_ptr2len(p1);
		    ಪೂರ್ಣಾಂಕ len2 = utfc_ptr2len(p2);

		    textline[ಲಂಬಸಾಲು] = ' ';
		    ಆದರೆ (len1 != len2 || STRNCMP(p1, p2, len1) != 0)
			// text differs
			textline[ಲಂಬಸಾಲು] = 'X';
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (lnum == cursor_pos1.ಅಡ್ಡಸಾಲು + 1
			    && ಲಂಬಸಾಲು == cursor_pos1.ಲಂಬಸಾಲು
			    && (cursor_pos1.ಅಡ್ಡಸಾಲು != cursor_pos2.ಅಡ್ಡಸಾಲು
					|| cursor_pos1.ಲಂಬಸಾಲು != cursor_pos2.ಲಂಬಸಾಲು))
			// cursor in first but not in second
			textline[ಲಂಬಸಾಲು] = '>';
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (lnum == cursor_pos2.ಅಡ್ಡಸಾಲು + 1
			    && ಲಂಬಸಾಲು == cursor_pos2.ಲಂಬಸಾಲು
			    && (cursor_pos1.ಅಡ್ಡಸಾಲು != cursor_pos2.ಅಡ್ಡಸಾಲು
					|| cursor_pos1.ಲಂಬಸಾಲು != cursor_pos2.ಲಂಬಸಾಲು))
			// cursor in second but not in first
			textline[ಲಂಬಸಾಲು] = '<';
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cellattr1 != ಖಾಲಿ && cellattr2 != ಖಾಲಿ)
		    {
			ಆದರೆ ((cellattr1 + ಲಂಬಸಾಲು)->ಅಗಲ
						   != (cellattr2 + ಲಂಬಸಾಲು)->ಅಗಲ)
			    textline[ಲಂಬಸಾಲು] = 'w';
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!vterm_color_is_equal(&(cellattr1 + ಲಂಬಸಾಲು)->fg,
						   &(cellattr2 + ಲಂಬಸಾಲು)->fg))
			    textline[ಲಂಬಸಾಲು] = 'f';
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!vterm_color_is_equal(&(cellattr1 + ಲಂಬಸಾಲು)->bg,
						   &(cellattr2 + ಲಂಬಸಾಲು)->bg))
			    textline[ಲಂಬಸಾಲು] = 'b';
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (vtermAttr2hl(&(cellattr1 + ಲಂಬಸಾಲು)->attrs)
				  != vtermAttr2hl(&((cellattr2 + ಲಂಬಸಾಲು)->attrs)))
			    textline[ಲಂಬಸಾಲು] = 'a';
		    }
		    p1 += len1;
		    p2 += len2;
		    // TODO: handle different width
		}

		ಆಗ (ಲಂಬಸಾಲು < ಅಗಲ)
		{
		    ಆದರೆ (*p1 == NUL && *p2 == NUL)
			textline[ಲಂಬಸಾಲು] = '?';
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p1 == NUL)
		    {
			textline[ಲಂಬಸಾಲು] = '+';
			p2 += utfc_ptr2len(p2);
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			textline[ಲಂಬಸಾಲು] = '-';
			p1 += utfc_ptr2len(p1);
		    }
		    ++ಲಂಬಸಾಲು;
		}

		vim_free(line1);
	    }
	    ಆದರೆ (ಕೂಡಿಸು_ಖಾಲಿಗಳು_scrollback(ಪದ, &ಪದ->tl_default_color,
						 ಪದ->tl_ತುದಿ_diff_ಅಡ್ಡಸಾಲುಗಳು) == OK)
		ml_append(ಪದ->tl_ತುದಿ_diff_ಅಡ್ಡಸಾಲುಗಳು + lnum, textline, 0, FALSE);
	    ++bot_lnum;
	}

	ಆಗ (lnum + bot_lnum <= curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	{
	    // bottom part has more rows, fill with "+"
	    ಗಾಗಿ (i = 0; i < ಅಗಲ; ++i)
		textline[i] = '+';
	    ಆದರೆ (ಕೂಡಿಸು_ಖಾಲಿಗಳು_scrollback(ಪದ, &ಪದ->tl_default_color,
						 ಪದ->tl_ತುದಿ_diff_ಅಡ್ಡಸಾಲುಗಳು) == OK)
		ml_append(ಪದ->tl_ತುದಿ_diff_ಅಡ್ಡಸಾಲುಗಳು + lnum, textline, 0, FALSE);
	    ++lnum;
	    ++bot_lnum;
	}

	ಪದ->tl_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = ಅಗಲ;

	// looks better without wrapping
	curwin->w_p_wrap = 0;
    }

theend:
    vim_free(textline);
    vim_free(fname_tofree);
    fclose(fd1);
    ಆದರೆ (fd2 != ಖಾಲಿ)
	fclose(fd2);
}

/*
 * If the current buffer shows the output of term_dumpdiff(), swap the top and
 * bottom files.
 * Return FAIL when this is not possible.
 */
    ಪೂರ್ಣಾಂಕ
ಪದ_ಅದಲುಬದಲು_diff(ಶೂನ್ಯ)
{
    ಪದ_T	*ಪದ = curbuf->b_ಪದ;
    linenr_T	ಸಾಲು_ಎಣಿಕೆ;
    linenr_T	ತುದಿ_ಅಡ್ಡಸಾಲುಗಳು;
    linenr_T	bot_ಅಡ್ಡಸಾಲುಗಳು;
    linenr_T	bot_ಆರಂಭ;
    linenr_T	lnum;
    char_u	*p;
    sb_ಸಾಲು_T	*sb_ಸಾಲು;

    ಆದರೆ (ಪದ == ಖಾಲಿ
	    || !ಪದ_is_ಪೂರ್ಣಗೊಂಡ(curbuf)
	    || ಪದ->tl_ತುದಿ_diff_ಅಡ್ಡಸಾಲುಗಳು == 0
	    || ಪದ->tl_scrollback.ga_len == 0)
	ಹಿಂತಿರುಗಿ FAIL;

    ಸಾಲು_ಎಣಿಕೆ = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
    ತುದಿ_ಅಡ್ಡಸಾಲುಗಳು = ಪದ->tl_ತುದಿ_diff_ಅಡ್ಡಸಾಲುಗಳು;
    bot_ಅಡ್ಡಸಾಲುಗಳು = ಪದ->tl_bot_diff_ಅಡ್ಡಸಾಲುಗಳು;
    bot_ಆರಂಭ = ಸಾಲು_ಎಣಿಕೆ - bot_ಅಡ್ಡಸಾಲುಗಳು;
    sb_ಸಾಲು = (sb_ಸಾಲು_T *)ಪದ->tl_scrollback.ga_ದತ್ತಾಂಶ;

    // move lines from top to above the bottom part
    ಗಾಗಿ (lnum = 1; lnum <= ತುದಿ_ಅಡ್ಡಸಾಲುಗಳು; ++lnum)
    {
	p = vim_strsave(ml_get(1));
	ಆದರೆ (p == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ OK;
	ml_append(bot_ಆರಂಭ, p, 0, FALSE);
	ml_delete(1);
	vim_free(p);
    }

    // move lines from bottom to the top
    ಗಾಗಿ (lnum = 1; lnum <= bot_ಅಡ್ಡಸಾಲುಗಳು; ++lnum)
    {
	p = vim_strsave(ml_get(bot_ಆರಂಭ + lnum));
	ಆದರೆ (p == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ OK;
	ml_delete(bot_ಆರಂಭ + lnum);
	ml_append(lnum - 1, p, 0, FALSE);
	vim_free(p);
    }

    // move top title to bottom
    p = vim_strsave(ml_get(bot_ಅಡ್ಡಸಾಲುಗಳು + 1));
    ಆದರೆ (p == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ OK;
    ml_append(ಸಾಲು_ಎಣಿಕೆ - ತುದಿ_ಅಡ್ಡಸಾಲುಗಳು - 1, p, 0, FALSE);
    ml_delete(bot_ಅಡ್ಡಸಾಲುಗಳು + 1);
    vim_free(p);

    // move bottom title to top
    p = vim_strsave(ml_get(ಸಾಲು_ಎಣಿಕೆ - ತುದಿ_ಅಡ್ಡಸಾಲುಗಳು));
    ಆದರೆ (p == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ OK;
    ml_delete(ಸಾಲು_ಎಣಿಕೆ - ತುದಿ_ಅಡ್ಡಸಾಲುಗಳು);
    ml_append(bot_ಅಡ್ಡಸಾಲುಗಳು, p, 0, FALSE);
    vim_free(p);

    ಆದರೆ (ತುದಿ_ಅಡ್ಡಸಾಲುಗಳು == bot_ಅಡ್ಡಸಾಲುಗಳು)
    {
	// rows counts are equal, can swap cell properties
	ಗಾಗಿ (lnum = 0; lnum < ತುದಿ_ಅಡ್ಡಸಾಲುಗಳು; ++lnum)
	{
	    sb_ಸಾಲು_T	ತಾತ್ಕಾಲಿಕ;

	    ತಾತ್ಕಾಲಿಕ = *(sb_ಸಾಲು + lnum);
	    *(sb_ಸಾಲು + lnum) = *(sb_ಸಾಲು + bot_ಆರಂಭ + lnum);
	    *(sb_ಸಾಲು + bot_ಆರಂಭ + lnum) = ತಾತ್ಕಾಲಿಕ;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	size_t		ಗಾತ್ರ_2 = ಗಾತ್ರ(sb_ಸಾಲು_T) * ಪದ->tl_scrollback.ga_len;
	sb_ಸಾಲು_T	*ತಾತ್ಕಾಲಿಕ = alloc(ಗಾತ್ರ_2);

	// need to copy cell properties into temp memory
	ಆದರೆ (ತಾತ್ಕಾಲಿಕ != ಖಾಲಿ)
	{
	    mch_memmove(ತಾತ್ಕಾಲಿಕ, ಪದ->tl_scrollback.ga_ದತ್ತಾಂಶ, ಗಾತ್ರ_2);
	    mch_memmove(ಪದ->tl_scrollback.ga_ದತ್ತಾಂಶ,
		    ತಾತ್ಕಾಲಿಕ + bot_ಆರಂಭ,
		    ಗಾತ್ರ(sb_ಸಾಲು_T) * bot_ಅಡ್ಡಸಾಲುಗಳು);
	    mch_memmove((sb_ಸಾಲು_T *)ಪದ->tl_scrollback.ga_ದತ್ತಾಂಶ + bot_ಅಡ್ಡಸಾಲುಗಳು,
		    ತಾತ್ಕಾಲಿಕ + ತುದಿ_ಅಡ್ಡಸಾಲುಗಳು,
		    ಗಾತ್ರ(sb_ಸಾಲು_T) * (ಸಾಲು_ಎಣಿಕೆ - ತುದಿ_ಅಡ್ಡಸಾಲುಗಳು - bot_ಅಡ್ಡಸಾಲುಗಳು));
	    mch_memmove((sb_ಸಾಲು_T *)ಪದ->tl_scrollback.ga_ದತ್ತಾಂಶ
						       + ಸಾಲು_ಎಣಿಕೆ - ತುದಿ_ಅಡ್ಡಸಾಲುಗಳು,
		    ತಾತ್ಕಾಲಿಕ,
		    ಗಾತ್ರ(sb_ಸಾಲು_T) * ತುದಿ_ಅಡ್ಡಸಾಲುಗಳು);
	    vim_free(ತಾತ್ಕಾಲಿಕ);
	}
    }

    ಪದ->tl_ತುದಿ_diff_ಅಡ್ಡಸಾಲುಗಳು = bot_ಅಡ್ಡಸಾಲುಗಳು;
    ಪದ->tl_bot_diff_ಅಡ್ಡಸಾಲುಗಳು = ತುದಿ_ಅಡ್ಡಸಾಲುಗಳು;

    update_screen(UPD_NOT_VALID);
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * "term_dumpdiff(filename, filename, options)" function
 */
    ಶೂನ್ಯ
f_ಪದ_dumpdiff(typval_T *argvars, typval_T *rettv)
{
    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	ಹಿಂತಿರುಗಿ;

    ಪದ_ತುಂಬು_dump(argvars, rettv, TRUE);
}

/*
 * "term_dumpload(filename, options)" function
 */
    ಶೂನ್ಯ
f_ಪದ_dumpload(typval_T *argvars, typval_T *rettv)
{
    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    ಪದ_ತುಂಬು_dump(argvars, rettv, FALSE);
}

/*
 * "term_getaltscreen(buf)" function
 */
    ಶೂನ್ಯ
f_ಪದ_getaltscreen(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    ಆದರೆ (ಒಳ_vim9script() && check_for_ಬಫರ್_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    buf = ಪದ_get_buf(argvars, "term_getaltscreen()");
    ಆದರೆ (buf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    rettv->vval.v_ಸಂಖ್ಯೆ = buf->b_ಪದ->tl_using_altscreen;
}

/*
 * "term_getattr(attr, name)" function
 */
    ಶೂನ್ಯ
f_ಪದ_getattr(typval_T *argvars, typval_T *rettv)
{
    ಪೂರ್ಣಾಂಕ	    attr;
    size_t  i;
    char_u  *name;

    ಸ್ಥಿರ ರಚನೆ {
	ಅಕ್ಷರ	    *name;
	ಪೂರ್ಣಾಂಕ	    attr;
    } attrs[] = {
	{"bold",      HL_BOLD},
	{"italic",    HL_ITALIC},
	{"underline", HL_UNDERLINE},
	{"strike",    HL_STRIKETHROUGH},
	{"reverse",   HL_INVERSE},
    };

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    attr = tv_get_ಸಂಖ್ಯೆ(&argvars[0]);
    name = tv_get_string_chk(&argvars[1]);
    ಆದರೆ (name == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (attr > HL_ALL)
	attr = syn_attr2attr(attr);
    ಗಾಗಿ (i = 0; i < ARRAY_ಉದ್ದಳತೆ(attrs); ++i)
	ಆದರೆ (STRCMP(name, attrs[i].name) == 0)
	{
	    rettv->vval.v_ಸಂಖ್ಯೆ = (attr & attrs[i].attr) != 0 ? 1 : 0;
	    ನಿಲ್ಲಿಸು;
	}
}

/*
 * "term_getcursor(buf)" function
 */
    ಶೂನ್ಯ
f_ಪದ_getcursor(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    ಪದ_T	*ಪದ;
    list_T	*l;
    dict_T	*d;

    ಆದರೆ (rettv_list_alloc(rettv) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (ಒಳ_vim9script() && check_for_ಬಫರ್_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    buf = ಪದ_get_buf(argvars, "term_getcursor()");
    ಆದರೆ (buf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    ಪದ = buf->b_ಪದ;

    l = rettv->vval.v_list;
    list_append_ಸಂಖ್ಯೆ(l, ಪದ->tl_cursor_pos.ಅಡ್ಡಸಾಲು + 1);
    list_append_ಸಂಖ್ಯೆ(l, ಪದ->tl_cursor_pos.ಲಂಬಸಾಲು + 1);

    d = dict_alloc();
    ಆದರೆ (d == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(d, "visible", ಪದ->tl_cursor_visible);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(d, "blink", blink_state_is_inverted()
	    ? !ಪದ->tl_cursor_blink : ಪದ->tl_cursor_blink);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(d, "shape", ಪದ->tl_cursor_shape);
    dict_ಕೂಡಿಸು_string(d, "color", cursor_color_get(ಪದ->tl_cursor_color));
    ಆದರೆ (list_append_dict(l, d) == FAIL)
	dict_unref(d);
}

/*
 * "term_getjob(buf)" function
 */
    ಶೂನ್ಯ
f_ಪದ_getjob(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    ಆದರೆ (ಒಳ_vim9script() && check_for_ಬಫರ್_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    buf = ಪದ_get_buf(argvars, "term_getjob()");
    ಆದರೆ (buf == ಖಾಲಿ)
    {
	ಆದರೆ (ಒಳ_vim9script())
	{
	    rettv->v_type = VAR_ಕೆಲಸ;
	    rettv->vval.v_ಕೆಲಸ = ಖಾಲಿ;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    rettv->v_type = VAR_SPECIAL;
	    rettv->vval.v_ಸಂಖ್ಯೆ = VVAL_NULL;
	}
	ಹಿಂತಿರುಗಿ;
    }

    rettv->v_type = VAR_ಕೆಲಸ;
    rettv->vval.v_ಕೆಲಸ = buf->b_ಪದ->tl_ಕೆಲಸ;
    ಆದರೆ (rettv->vval.v_ಕೆಲಸ != ಖಾಲಿ)
	++rettv->vval.v_ಕೆಲಸ->jv_refcount;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
get_ಅಡ್ಡಸಾಲು_ಸಂಖ್ಯೆ(typval_T *tv, ಪದ_T *ಪದ)
{
    ಆದರೆ (tv->v_type == VAR_STRING
	    && tv->vval.v_string != ಖಾಲಿ
	    && STRCMP(tv->vval.v_string, ".") == 0)
	ಹಿಂತಿರುಗಿ ಪದ->tl_cursor_pos.ಅಡ್ಡಸಾಲು;
    ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ)tv_get_ಸಂಖ್ಯೆ(tv) - 1;
}

/*
 * "term_getline(buf, row)" function
 */
    ಶೂನ್ಯ
f_ಪದ_getline(typval_T *argvars, typval_T *rettv)
{
    buf_T	    *buf;
    ಪದ_T	    *ಪದ;
    ಪೂರ್ಣಾಂಕ		    ಅಡ್ಡಸಾಲು;

    rettv->v_type = VAR_STRING;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_ಬಫರ್_arg(argvars, 0) == FAIL
		|| check_for_lnum_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    buf = ಪದ_get_buf(argvars, "term_getline()");
    ಆದರೆ (buf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    ಪದ = buf->b_ಪದ;
    ಅಡ್ಡಸಾಲು = get_ಅಡ್ಡಸಾಲು_ಸಂಖ್ಯೆ(&argvars[1], ಪದ);

    ಆದರೆ (ಪದ->tl_vterm == ಖಾಲಿ)
    {
	linenr_T  lnum = 0;
	size_t	  ಸ್ಥಾನಾಂತರ = 0;
	ಪೂರ್ಣಾಂಕ	  sb_ಅಡ್ಡಸಾಲು = ಪದ->tl_scrollback_scrolled + ಅಡ್ಡಸಾಲು;
	sb_ಸಾಲು_T *ಸಾಲು;

	ಆದರೆ (sb_ಅಡ್ಡಸಾಲು < 0 || sb_ಅಡ್ಡಸಾಲು >= ಪದ->tl_scrollback.ga_len)
	    ಹಿಂತಿರುಗಿ;
	ಸಾಲು = (sb_ಸಾಲು_T *)ಪದ->tl_scrollback.ga_ದತ್ತಾಂಶ + sb_ಅಡ್ಡಸಾಲು;

	scrollbackline_pos_ಒಳ_buf(ಪದ, sb_ಅಡ್ಡಸಾಲು, &lnum, ಖಾಲಿ, &ಸ್ಥಾನಾಂತರ);

	// vterm is finished, get the text from the buffer
	ಆದರೆ (lnum > 0 && lnum <= buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	{
	    char_u *p = ml_get_buf(buf, lnum, FALSE);
	    ಆದರೆ (STRLEN(p) >= ಸ್ಥಾನಾಂತರ + ಸಾಲು->sb_ಬೈಟುಗಳು)
		rettv->vval.v_string = vim_strnsave(p + ಸ್ಥಾನಾಂತರ, ಸಾಲು->sb_ಬೈಟುಗಳು);
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	VTermScreen	*screen = vterm_obtain_screen(ಪದ->tl_vterm);
	VTermRect	rect;
	ಪೂರ್ಣಾಂಕ		len;
	char_u		*p;

	ಆದರೆ (ಅಡ್ಡಸಾಲು < 0 || ಅಡ್ಡಸಾಲು >= ಪದ->tl_ಅಡ್ಡಸಾಲುಗಳು)
	    ಹಿಂತಿರುಗಿ;
	len = ಪದ->tl_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ * MB_MAXBYTES + 1;
	p = alloc(len);
	ಆದರೆ (p == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ;
	rettv->vval.v_string = p;

	rect.ಆರಂಭ_ಲಂಬಸಾಲು = 0;
	rect.ಕೊನೆ_ಲಂಬಸಾಲು = ಪದ->tl_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;
	rect.ಆರಂಭ_ಅಡ್ಡಸಾಲು = ಅಡ್ಡಸಾಲು;
	rect.ಕೊನೆ_ಅಡ್ಡಸಾಲು = ಅಡ್ಡಸಾಲು + 1;
	p[vterm_screen_get_ಪಠ್ಯ(screen, (ಅಕ್ಷರ *)p, len, rect)] = NUL;
    }
}

/*
 * "term_getscrolled(buf)" function
 */
    ಶೂನ್ಯ
f_ಪದ_getscrolled(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    ಆದರೆ (ಒಳ_vim9script() && check_for_ಬಫರ್_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    buf = ಪದ_get_buf(argvars, "term_getscrolled()");
    ಆದರೆ (buf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    rettv->vval.v_ಸಂಖ್ಯೆ = buf->b_ಪದ->tl_ಬಫರ್_scrolled;
}

/*
 * "term_getsize(buf)" function
 */
    ಶೂನ್ಯ
f_ಪದ_getsize(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    list_T	*l;

    ಆದರೆ (rettv_list_alloc(rettv) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (ಒಳ_vim9script() && check_for_ಬಫರ್_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    buf = ಪದ_get_buf(argvars, "term_getsize()");
    ಆದರೆ (buf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    l = rettv->vval.v_list;
    list_append_ಸಂಖ್ಯೆ(l, buf->b_ಪದ->tl_ಅಡ್ಡಸಾಲುಗಳು);
    list_append_ಸಂಖ್ಯೆ(l, buf->b_ಪದ->tl_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ);
}

/*
 * "term_setsize(buf, rows, cols)" function
 */
    ಶೂನ್ಯ
f_ಪದ_setsize(typval_T *argvars, typval_T *rettv ಬಳಸದ)
{
    buf_T	*buf;
    ಪದ_T	*ಪದ;
    varnumber_T ಅಡ್ಡಸಾಲುಗಳು_2, ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_ಬಫರ್_arg(argvars, 0) == FAIL
		|| check_for_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL
		|| check_for_ಸಂಖ್ಯೆ_arg(argvars, 2) == FAIL))
	ಹಿಂತಿರುಗಿ;

    buf = ಪದ_get_buf(argvars, "term_setsize()");
    ಆದರೆ (buf == ಖಾಲಿ)
    {
	emsg(_(e_not_terminal_ಬಫರ್));
	ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (buf->b_ಪದ->tl_vterm == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    ಪದ = buf->b_ಪದ;
    ಅಡ್ಡಸಾಲುಗಳು_2 = tv_get_ಸಂಖ್ಯೆ(&argvars[1]);
    ಅಡ್ಡಸಾಲುಗಳು_2 = ಅಡ್ಡಸಾಲುಗಳು_2 <= 0 ? ಪದ->tl_ಅಡ್ಡಸಾಲುಗಳು : ಅಡ್ಡಸಾಲುಗಳು_2;
    ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = tv_get_ಸಂಖ್ಯೆ(&argvars[2]);
    ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ <= 0 ? ಪದ->tl_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ : ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;
    vterm_ಗಣ_ಗಾತ್ರ(ಪದ->tl_vterm, ಅಡ್ಡಸಾಲುಗಳು_2, ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ);
    // handle_resize() will resize the windows

    // Get and remember the size we ended up with.  Update the pty.
    vterm_get_ಗಾತ್ರ(ಪದ->tl_vterm, &ಪದ->tl_ಅಡ್ಡಸಾಲುಗಳು, &ಪದ->tl_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ);
    ಪದ_report_winsize(ಪದ, ಪದ->tl_ಅಡ್ಡಸಾಲುಗಳು, ಪದ->tl_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ);
}

/*
 * "term_getstatus(buf)" function
 */
    ಶೂನ್ಯ
f_ಪದ_getstatus(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    ಪದ_T	*ಪದ;
    char_u	val[100];

    rettv->v_type = VAR_STRING;

    ಆದರೆ (ಒಳ_vim9script() && check_for_ಬಫರ್_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    buf = ಪದ_get_buf(argvars, "term_getstatus()");
    ಆದರೆ (buf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    ಪದ = buf->b_ಪದ;

    ಆದರೆ (ಪದ_ಕೆಲಸ_running(ಪದ))
	STRCPY(val, "running");
    ಇಲ್ಲದಿದ್ದರೆ
	STRCPY(val, "finished");
    ಆದರೆ (ಪದ->tl_ಸಾಮಾನ್ಯ_ಬಹುಲಕ)
	STRCAT(val, ",normal");
    rettv->vval.v_string = vim_strsave(val);
}

/*
 * "term_gettitle(buf)" function
 */
    ಶೂನ್ಯ
f_ಪದ_gettitle(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    rettv->v_type = VAR_STRING;

    ಆದರೆ (ಒಳ_vim9script() && check_for_ಬಫರ್_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    buf = ಪದ_get_buf(argvars, "term_gettitle()");
    ಆದರೆ (buf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (buf->b_ಪದ->tl_title != ಖಾಲಿ)
	rettv->vval.v_string = vim_strsave(buf->b_ಪದ->tl_title);
}

/*
 * "term_gettty(buf)" function
 */
    ಶೂನ್ಯ
f_ಪದ_gettty(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    char_u	*p = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		ಅಂಕೆ = 0;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_ಬಫರ್_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    rettv->v_type = VAR_STRING;
    buf = ಪದ_get_buf(argvars, "term_gettty()");
    ಆದರೆ (buf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (argvars[1].v_type != VAR_UNKNOWN)
	ಅಂಕೆ = tv_get_bool(&argvars[1]);

    ಆಯ್ಕೆ (ಅಂಕೆ)
    {
	ಸ್ಥಿತಿ 0:
	    ಆದರೆ (buf->b_ಪದ->tl_ಕೆಲಸ != ಖಾಲಿ)
		p = buf->b_ಪದ->tl_ಕೆಲಸ->jv_tty_ಹೊರಹೋಗು;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 1:
	    ಆದರೆ (buf->b_ಪದ->tl_ಕೆಲಸ != ಖಾಲಿ)
		p = buf->b_ಪದ->tl_ಕೆಲಸ->jv_tty_ಒಳ;
	    ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:
	    semsg(_(e_invalid_ವಾದ_str), tv_get_string(&argvars[1]));
	    ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (p != ಖಾಲಿ)
	rettv->vval.v_string = vim_strsave(p);
}

/*
 * "term_list()" function
 */
    ಶೂನ್ಯ
f_ಪದ_list(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
    ಪದ_T	*tp;
    list_T	*l;

    ಆದರೆ (rettv_list_alloc(rettv) == FAIL || ಮೊದಲ_ಪದ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    l = rettv->vval.v_list;
    FOR_ALL_TERMS(tp)
	ಆದರೆ (tp->tl_ಬಫರ್ != ಖಾಲಿ)
	    ಆದರೆ (list_append_ಸಂಖ್ಯೆ(l,
				   (varnumber_T)tp->tl_ಬಫರ್->b_fnum) == FAIL)
		ಹಿಂತಿರುಗಿ;
}

/*
 * "term_scrape(buf, row)" function
 */
    ಶೂನ್ಯ
f_ಪದ_scrape(typval_T *argvars, typval_T *rettv)
{
    buf_T	    *buf;
    VTermScreen	    *screen = ಖಾಲಿ;
    VTermPos	    pos;
    list_T	    *l;
    ಪದ_T	    *ಪದ;
    char_u	    *p;
    sb_ಸಾಲು_T	    *ಸಾಲು;

    ಆದರೆ (rettv_list_alloc(rettv) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_ಬಫರ್_arg(argvars, 0) == FAIL
		|| check_for_lnum_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    buf = ಪದ_get_buf(argvars, "term_scrape()");
    ಆದರೆ (buf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    ಪದ = buf->b_ಪದ;

    l = rettv->vval.v_list;
    pos.ಅಡ್ಡಸಾಲು = get_ಅಡ್ಡಸಾಲು_ಸಂಖ್ಯೆ(&argvars[1], ಪದ);

    ಆದರೆ (ಪದ->tl_vterm != ಖಾಲಿ)
    {
	screen = vterm_obtain_screen(ಪದ->tl_vterm);
	ಆದರೆ (screen == ಖಾಲಿ)  // can't really happen
	    ಹಿಂತಿರುಗಿ;
	p = ಖಾಲಿ;
	ಸಾಲು = ಖಾಲಿ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಪೂರ್ಣಾಂಕ	  sb_ಅಡ್ಡಸಾಲು = ಪದ->tl_scrollback_scrolled + pos.ಅಡ್ಡಸಾಲು;
	linenr_T  lnum = 0;
	size_t	  ಸ್ಥಾನಾಂತರ = 0;

	scrollbackline_pos_ಒಳ_buf(ಪದ, sb_ಅಡ್ಡಸಾಲು, &lnum, ಖಾಲಿ, &ಸ್ಥಾನಾಂತರ);

	ಆದರೆ (sb_ಅಡ್ಡಸಾಲು >= ಪದ->tl_scrollback.ga_len || lnum <= 0 || lnum > buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	    ಹಿಂತಿರುಗಿ;

	ಸಾಲು = (sb_ಸಾಲು_T *)ಪದ->tl_scrollback.ga_ದತ್ತಾಂಶ + sb_ಅಡ್ಡಸಾಲು;
	p = ml_get_buf(buf, lnum, FALSE);

	ಆದರೆ (STRLEN(p) < ಸ್ಥಾನಾಂತರ + ಸಾಲು->sb_ಬೈಟುಗಳು)
	    ಹಿಂತಿರುಗಿ;

	p += ಸ್ಥಾನಾಂತರ;
    }

    ಗಾಗಿ (pos.ಲಂಬಸಾಲು = 0; pos.ಲಂಬಸಾಲು < ಪದ->tl_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ; )
    {
	dict_T		*dcell;
	ಪೂರ್ಣಾಂಕ		ಅಗಲ;
	VTermScreenCellAttrs attrs;
	VTermColor	fg, bg;
	char_u		rgb[8];
	size_t		rgblen;
	char_u		mbs[MB_MAXBYTES * VTERM_MAX_CHARS_ಪ್ರತಿ_CELL + 1];
	size_t		mbslen;
	ಪೂರ್ಣಾಂಕ		i;

	ಆದರೆ (screen == ಖಾಲಿ)
	{
	    cellattr_T	*cellattr;

	    // vterm has finished, get the cell from scrollback
	    ಆದರೆ (pos.ಲಂಬಸಾಲು >= ಸಾಲು->sb_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ)
		ನಿಲ್ಲಿಸು;
	    cellattr = ಸಾಲು->sb_cells + pos.ಲಂಬಸಾಲು;
	    ಅಗಲ = cellattr->ಅಗಲ;
	    attrs = cellattr->attrs;
	    fg = cellattr->fg;
	    bg = cellattr->bg;
	    mbslen = mb_ptr2len(p);
	    mch_memmove(mbs, p, mbslen);
	    mbs[mbslen] = NUL;
	    p += mbslen;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    VTermScreenCell cell;

	    ಆದರೆ (vterm_screen_get_cell(screen, pos, &cell) == 0)
		ನಿಲ್ಲಿಸು;

	    mbslen = 0;
	    ಗಾಗಿ (i = 0; i < VTERM_MAX_CHARS_ಪ್ರತಿ_CELL; ++i)
	    {
		ಆದರೆ (cell.chars[i] == 0)
		    ನಿಲ್ಲಿಸು;
		mbslen += (*utf_char2bytes)((ಪೂರ್ಣಾಂಕ)cell.chars[i], mbs + mbslen);
	    }
	    mbs[mbslen] = NUL;
	    ಅಗಲ = cell.ಅಗಲ;
	    attrs = cell.attrs;
	    fg = cell.fg;
	    bg = cell.bg;
	}
	dcell = dict_alloc();
	ಆದರೆ (dcell == ಖಾಲಿ)
	    ನಿಲ್ಲಿಸು;
	ಆದರೆ (list_append_dict(l, dcell) == FAIL)
	{
	    dict_unref(dcell);
	    ನಿಲ್ಲಿಸು;
	}

	dict_ಕೂಡಿಸು_string_len(dcell, "chars", mbs, (ಪೂರ್ಣಾಂಕ)mbslen);

	rgblen = vim_snprintf_safelen((ಅಕ್ಷರ *)rgb, ಗಾತ್ರ(rgb),
	    "#%02x%02x%02x", fg.red, fg.green, fg.blue);
	dict_ಕೂಡಿಸು_string_len(dcell, "fg", rgb, (ಪೂರ್ಣಾಂಕ)rgblen);
	rgblen = vim_snprintf_safelen((ಅಕ್ಷರ *)rgb, ಗಾತ್ರ(rgb),
	    "#%02x%02x%02x", bg.red, bg.green, bg.blue);
	dict_ಕೂಡಿಸು_string_len(dcell, "bg", rgb, (ಪೂರ್ಣಾಂಕ)rgblen);

	dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dcell, "attr",
				      cell2attr(ಪದ, ಖಾಲಿ, &attrs, &fg, &bg));
	dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dcell, "width", ಅಗಲ);

	++pos.ಲಂಬಸಾಲು;
	ಆದರೆ (ಅಗಲ == 2)
	    ++pos.ಲಂಬಸಾಲು;
    }
}

/*
 * "term_sendkeys(buf, keys)" function
 */
    ಶೂನ್ಯ
f_ಪದ_sendkeys(typval_T *argvars, typval_T *rettv ಬಳಸದ)
{
    buf_T	*buf;
    char_u	*ಸಂದೇಶ;
    ಪದ_T	*ಪದ;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_ಬಫರ್_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    buf = ಪದ_get_buf(argvars, "term_sendkeys()");
    ಆದರೆ (buf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಸಂದೇಶ = tv_get_string_chk(&argvars[1]);
    ಆದರೆ (ಸಂದೇಶ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    ಪದ = buf->b_ಪದ;
    ಆದರೆ (ಪದ->tl_vterm == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆಗ (*ಸಂದೇಶ != NUL)
    {
	ಪೂರ್ಣಾಂಕ c;

	ಆದರೆ (*ಸಂದೇಶ == K_SPECIAL && ಸಂದೇಶ[1] != NUL && ಸಂದೇಶ[2] != NUL)
	{
	    c = ಗಮ್ಯ_SPECIAL(ಸಂದೇಶ[1], ಸಂದೇಶ[2]);
	    ಸಂದೇಶ += 3;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    c = PTR2CHAR(ಸಂದೇಶ);
	    ಸಂದೇಶ += MB_CPTR2LEN(ಸಂದೇಶ);
	}
	send_ಕೀಲಿಗಳು_ಗಮ್ಯ_ಪದ(ಪದ, c, 0, FALSE);
    }
}

#ಆದರೆ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
/*
 * "term_getansicolors(buf)" function
 */
    ಶೂನ್ಯ
f_ಪದ_getansicolors(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    ಪದ_T	*ಪದ;
    VTermState	*state;
    VTermColor  color;
    char_u	hexbuf[10];
    ಪೂರ್ಣಾಂಕ		ಸೂಚ್ಯಂಕ;
    list_T	*list;

    ಆದರೆ (rettv_list_alloc(rettv) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (ಒಳ_vim9script() && check_for_ಬಫರ್_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    buf = ಪದ_get_buf(argvars, "term_getansicolors()");
    ಆದರೆ (buf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    ಪದ = buf->b_ಪದ;
    ಆದರೆ (ಪದ->tl_vterm == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    list = rettv->vval.v_list;
    state = vterm_obtain_state(ಪದ->tl_vterm);
    ಗಾಗಿ (ಸೂಚ್ಯಂಕ = 0; ಸೂಚ್ಯಂಕ < 16; ಸೂಚ್ಯಂಕ++)
    {
	size_t	hexbuflen;

	vterm_state_get_palette_color(state, ಸೂಚ್ಯಂಕ, &color);
	hexbuflen = vim_snprintf_safelen((ಅಕ್ಷರ *)hexbuf, ಗಾತ್ರ(hexbuf),
	    "#%02x%02x%02x", color.red, color.green, color.blue);
	ಆದರೆ (list_append_string(list, hexbuf, (ಪೂರ್ಣಾಂಕ)hexbuflen) == FAIL)
	    ಹಿಂತಿರುಗಿ;
    }
}

/*
 * "term_setansicolors(buf, list)" function
 */
    ಶೂನ್ಯ
f_ಪದ_setansicolors(typval_T *argvars, typval_T *rettv ಬಳಸದ)
{
    buf_T	*buf;
    ಪದ_T	*ಪದ;
    listitem_T	*li;
    ಪೂರ್ಣಾಂಕ		n = 0;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_ಬಫರ್_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    buf = ಪದ_get_buf(argvars, "term_setansicolors()");
    ಆದರೆ (buf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    ಪದ = buf->b_ಪದ;
    ಆದರೆ (ಪದ->tl_vterm == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (check_for_nonnull_list_arg(argvars, 1) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (argvars[1].vval.v_list->lv_ಮೊದಲ == &range_list_ಅಂಶ
	    || argvars[1].vval.v_list->lv_len != 16)
    {
	semsg(_(e_invalid_ಮೌಲ್ಯ_for_ವಾದ_str), "\"colors\"");
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (ಪದ->tl_palette == ಖಾಲಿ)
	ಪದ->tl_palette = ALLOC_MULT(long_u, 16);
    ಆದರೆ (ಪದ->tl_palette == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    FOR_ALL_LIST_ವಸ್ತುಗಳು(argvars[1].vval.v_list, li)
    {
	char_u		*color_name;
	guicolor_T	guicolor;

	color_name = tv_get_string_chk(&li->li_tv);
	ಆದರೆ (color_name == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ;

	guicolor = GUI_GET_COLOR(color_name);
	ಆದರೆ (guicolor == INVALCOLOR)
	{
	    semsg(_(e_cannot_allocate_color_str), color_name);
	    ಹಿಂತಿರುಗಿ;
	}

	ಪದ->tl_palette[n++] = GUI_MCH_GET_RGB(guicolor);
    }

    ಪದ_update_palette(ಪದ);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * "term_setapi(buf, api)" function
 */
    ಶೂನ್ಯ
f_ಪದ_setapi(typval_T *argvars, typval_T *rettv ಬಳಸದ)
{
    buf_T	*buf;
    ಪದ_T	*ಪದ;
    char_u	*api;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_ಬಫರ್_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    buf = ಪದ_get_buf(argvars, "term_setapi()");
    ಆದರೆ (buf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    ಪದ = buf->b_ಪದ;
    vim_free(ಪದ->tl_api);
    api = tv_get_string_chk(&argvars[1]);
    ಆದರೆ (api != ಖಾಲಿ)
	ಪದ->tl_api = vim_strsave(api);
    ಇಲ್ಲದಿದ್ದರೆ
	ಪದ->tl_api = ಖಾಲಿ;
}

/*
 * "term_setrestore(buf, command)" function
 */
    ಶೂನ್ಯ
f_ಪದ_setrestore(typval_T *argvars ಬಳಸದ, typval_T *rettv ಬಳಸದ)
{
#ಆದರೆ defined(FEAT_SESSION)
    buf_T	*buf;
    ಪದ_T	*ಪದ;
    char_u	*cmd;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_ಬಫರ್_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    buf = ಪದ_get_buf(argvars, "term_setrestore()");
    ಆದರೆ (buf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    ಪದ = buf->b_ಪದ;
    vim_free(ಪದ->tl_command);
    cmd = tv_get_string_chk(&argvars[1]);
    ಆದರೆ (cmd != ಖಾಲಿ)
	ಪದ->tl_command = vim_strsave(cmd);
    ಇಲ್ಲದಿದ್ದರೆ
	ಪದ->tl_command = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * "term_setkill(buf, how)" function
 */
    ಶೂನ್ಯ
f_ಪದ_setkill(typval_T *argvars ಬಳಸದ, typval_T *rettv ಬಳಸದ)
{
    buf_T	*buf;
    ಪದ_T	*ಪದ;
    char_u	*how;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_ಬಫರ್_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    buf = ಪದ_get_buf(argvars, "term_setkill()");
    ಆದರೆ (buf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    ಪದ = buf->b_ಪದ;
    vim_free(ಪದ->tl_kill);
    how = tv_get_string_chk(&argvars[1]);
    ಆದರೆ (how != ಖಾಲಿ)
	ಪದ->tl_kill = vim_strsave(how);
    ಇಲ್ಲದಿದ್ದರೆ
	ಪದ->tl_kill = ಖಾಲಿ;
}

/*
 * "term_start(command, options)" function
 */
    ಶೂನ್ಯ
f_ಪದ_ಆರಂಭ(typval_T *argvars, typval_T *rettv)
{
    jobopt_T	opt;
    buf_T	*buf;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    init_ಕೆಲಸ_options(&opt);
    ಆದರೆ (argvars[1].v_type != VAR_UNKNOWN
	    && get_ಕೆಲಸ_options(&argvars[1], &opt,
		JO_TIMEOUT_ALL + JO_STOPONEXIT
		    + JO_CALLBACK + JO_ಹೊರಹೋಗು_CALLBACK + JO_ERR_CALLBACK
		    + JO_EXIT_CB + JO_CLOSE_CALLBACK + JO_ಹೊರಹೋಗು_IO,
		JO2_ಪದ_NAME + JO2_ಪದ_ಮುಕ್ತಾಯ + JO2_HIDDEN + JO2_ಪದ_OPENCMD
		    + JO2_ಪದ_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ + JO2_ಪದ_ಅಡ್ಡಸಾಲುಗಳು + JO2_VERTICAL + JO2_CURWIN
		    + JO2_CWD + JO2_ENV + JO2_EOF_CHARS
		    + JO2_NORESTORE + JO2_ಪದ_KILL + JO2_ಪದ_HIGHLIGHT
		    + JO2_ANSI_COLORS + JO2_TTY_TYPE + JO2_ಪದ_API) == FAIL)
	ಹಿಂತಿರುಗಿ;

    buf = ಪದ_ಆರಂಭ(&argvars[0], ಖಾಲಿ, &opt, 0);

    ಆದರೆ (buf != ಖಾಲಿ && buf->b_ಪದ != ಖಾಲಿ)
	rettv->vval.v_ಸಂಖ್ಯೆ = buf->b_fnum;
}

/*
 * "term_wait" function
 */
    ಶೂನ್ಯ
f_ಪದ_wait(typval_T *argvars, typval_T *rettv ಬಳಸದ)
{
    buf_T	*buf;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_ಬಫರ್_arg(argvars, 0) == FAIL
		|| check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    buf = ಪದ_get_buf(argvars, "term_wait()");
    ಆದರೆ (buf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (buf->b_ಪದ->tl_ಕೆಲಸ == ಖಾಲಿ)
    {
	ch_log(ಖಾಲಿ, "term_wait(): no job to wait for");
	ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (buf->b_ಪದ->tl_ಕೆಲಸ->jv_channel == ಖಾಲಿ)
	// channel is closed, nothing to do
	ಹಿಂತಿರುಗಿ;

    // Get the job status, this will detect a job that finished.
    ಆದರೆ (!buf->b_ಪದ->tl_ಕೆಲಸ->jv_channel->ch_keep_open
	    && STRCMP(ಕೆಲಸ_ಸ್ಥಿತಿ(buf->b_ಪದ->tl_ಕೆಲಸ), "dead") == 0)
    {
	// The job is dead, keep reading channel I/O until the channel is
	// closed. buf->b_term may become NULL if the terminal was closed while
	// waiting.
	ch_log(ಖಾಲಿ, "term_wait(): waiting for channel to close");
	ಆಗ (buf->b_ಪದ != ಖಾಲಿ && !buf->b_ಪದ->tl_channel_closed)
	{
	    ಪದ_flush_messages();

	    ui_delay(10L, FALSE);
	    ಆದರೆ (!buf_valid(buf))
		// If the terminal is closed when the channel is closed the
		// buffer disappears.
		ನಿಲ್ಲಿಸು;
	    ಆದರೆ (buf->b_ಪದ == ಖಾಲಿ || buf->b_ಪದ->tl_channel_closing)
		// came here from a close callback, only wait one time
		ನಿಲ್ಲಿಸು;
	}

	ಪದ_flush_messages();
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ದೀರ್ಘ wait = 10L;

	ಪದ_flush_messages();

	// Wait for some time for any channel I/O.
	ಆದರೆ (argvars[1].v_type != VAR_UNKNOWN)
	    wait = tv_get_ಸಂಖ್ಯೆ(&argvars[1]);
	ui_delay(wait, TRUE);

	// Flushing messages on channels is hopefully sufficient.
	// TODO: is there a better way?
	ಪದ_flush_messages();
    }
}

/*
 * Called when a channel has sent all the lines to a terminal.
 * Send a CTRL-D to mark the end of the text.
 */
    ಶೂನ್ಯ
ಪದ_send_eof(channel_T *ch)
{
    ಪದ_T	*ಪದ;

    FOR_ALL_TERMS(ಪದ)
	ಆದರೆ (ಪದ->tl_ಕೆಲಸ == ch->ch_ಕೆಲಸ)
	{
	    ಆದರೆ (ಪದ->tl_eof_chars != ಖಾಲಿ)
	    {
		channel_send(ch, PART_ಒಳ, ಪದ->tl_eof_chars,
					(ಪೂರ್ಣಾಂಕ)STRLEN(ಪದ->tl_eof_chars), ಖಾಲಿ);
		channel_send(ch, PART_ಒಳ, (char_u *)"\r", 1, ಖಾಲಿ);
	    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
	    ಇಲ್ಲದಿದ್ದರೆ
		// Default: CTRL-D
		channel_send(ch, PART_ಒಳ, (char_u *)"\004\r", 2, ಖಾಲಿ);
#ಆದರೆ_ಅಂತ್ಯ
	}
}

#ಆದರೆ defined(FEAT_GUI)
    ಕೆಲಸ_T *
ಪದ_getjob(ಪದ_T *ಪದ)
{
    ಹಿಂತಿರುಗಿ ಪದ != ಖಾಲಿ ? ಪದ->tl_ಕೆಲಸ : ಖಾಲಿ;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(MSWIN)

///////////////////////////////////////
// 2. MS-Windows implementation.

HRESULT (WINAPI *pCreatePseudoConsole)(COORD, HANDLE, HANDLE, DWORD, HPCON*);
HRESULT (WINAPI *pResizePseudoConsole)(HPCON, COORD);
HRESULT (WINAPI *pClosePseudoConsole)(HPCON);
BOOL (WINAPI *pInitializeProcThreadAttributeList)(LPPROC_THREAD_ATTRIBUTE_LIST, DWORD, DWORD, PSIZE_T);
BOOL (WINAPI *pUpdateProcThreadAttribute)(LPPROC_THREAD_ATTRIBUTE_LIST, DWORD, DWORD_PTR, PVOID, ಗಾತ್ರ_T, PVOID, PSIZE_T);
ಶೂನ್ಯ (WINAPI *pDeleteProcThreadAttributeList)(LPPROC_THREAD_ATTRIBUTE_LIST);

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
dyn_conpty_init(ಪೂರ್ಣಾಂಕ verbose)
{
    ಸ್ಥಿರ HMODULE	hKerneldll = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ			i;
    ಸ್ಥಿರ ರಚನೆ
    {
	ಅಕ್ಷರ	*name;
	FARPROC	*ptr;
    } conpty_ನಮೂದು[] =
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
	{ಖಾಲಿ, ಖಾಲಿ}
    };

    ಆದರೆ (!has_conpty_working())
    {
	ಆದರೆ (verbose)
	    emsg(_(e_conpty_is_not_ಲಭ್ಯ));
	ಹಿಂತಿರುಗಿ FAIL;
    }

    // No need to initialize twice.
    ಆದರೆ (hKerneldll)
	ಹಿಂತಿರುಗಿ OK;

    hKerneldll = vim_ತುಂಬು_Lib("kernel32.dll");
    ಗಾಗಿ (i = 0; conpty_ನಮೂದು[i].name != ಖಾಲಿ
					&& conpty_ನಮೂದು[i].ptr != ಖಾಲಿ; ++i)
    {
	ಆದರೆ ((*conpty_ನಮೂದು[i].ptr = (FARPROC)Get_Proc_ವಿಳಾಸ(hKerneldll,
						conpty_ನಮೂದು[i].name)) == ಖಾಲಿ)
	{
	    ಆದರೆ (verbose)
		semsg(_(e_could_not_ತುಂಬು_library_function_str), conpty_ನಮೂದು[i].name);
	    hKerneldll = ಖಾಲಿ;
	    ಹಿಂತಿರುಗಿ FAIL;
	}
    }

    ಹಿಂತಿರುಗಿ OK;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
conpty_ಪದ_and_ಕೆಲಸ_init(
	ಪದ_T	    *ಪದ,
	typval_T    *argvar,
	ಅಕ್ಷರ	    **argv ಬಳಸದ,
	jobopt_T    *opt,
	jobopt_T    *orig_opt)
{
    WCHAR	    *cmd_wchar = ಖಾಲಿ;
    WCHAR	    *cmd_wchar_copy = ಖಾಲಿ;
    WCHAR	    *cwd_wchar = ಖಾಲಿ;
    WCHAR	    *env_wchar = ಖಾಲಿ;
    channel_T	    *channel = ಖಾಲಿ;
    ಕೆಲಸ_T	    *ಕೆಲಸ = ಖಾಲಿ;
    HANDLE	    jo = ಖಾಲಿ;
    garray_T	    ga_cmd, ga_env;
    char_u	    *cmd = ಖಾಲಿ;
    HRESULT	    hr;
    COORD	    consize;
    ಗಾತ್ರ_T	    breq;
    PROCESS_INFORMATION proc_info;
    HANDLE	    i_theirs = ಖಾಲಿ;
    HANDLE	    o_theirs = ಖಾಲಿ;
    HANDLE	    i_ours = ಖಾಲಿ;
    HANDLE	    o_ours = ಖಾಲಿ;
    ಅಕ್ಷರ	    *errmsg = ಖಾಲಿ;

    ga_init2(&ga_cmd, ಗಾತ್ರ(ಅಕ್ಷರ*), 20);
    ga_init2(&ga_env, ಗಾತ್ರ(ಅಕ್ಷರ*), 20);

    ಆದರೆ (argvar->v_type == VAR_STRING)
    {
	cmd = argvar->vval.v_string;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argvar->v_type == VAR_LIST)
    {
	ಆದರೆ (win32_build_cmd(argvar->vval.v_list, &ga_cmd) == FAIL)
	    ಹೋಗು failed;
	cmd = ga_cmd.ga_ದತ್ತಾಂಶ;
    }
    ಆದರೆ (cmd == ಖಾಲಿ || *cmd == NUL)
    {
	emsg(_(e_invalid_ವಾದ));
	ಹೋಗು failed;
    }

    ಪದ->tl_arg0_cmd = vim_strsave(cmd);

    cmd_wchar = enc_ಗಮ್ಯ_utf16(cmd, ಖಾಲಿ);

    ಆದರೆ (cmd_wchar != ಖಾಲಿ)
    {
	// Request by CreateProcessW
	breq = wcslen(cmd_wchar) + 1 + 1;	// Addition of NUL by API
	cmd_wchar_copy = ALLOC_MULT(WCHAR, breq);
	wcsncpy(cmd_wchar_copy, cmd_wchar, breq - 1);
    }

    ga_clear(&ga_cmd);
    ಆದರೆ (cmd_wchar == ಖಾಲಿ)
	ಹೋಗು failed;
    ಆದರೆ (opt->jo_cwd != ಖಾಲಿ)
    {
	cwd_wchar = enc_ಗಮ್ಯ_utf16(opt->jo_cwd, ಖಾಲಿ);
	ಆದರೆ (cwd_wchar == ಖಾಲಿ)
	    ಹೋಗು failed;
    }

    win32_build_env(opt->jo_env, &ga_env, TRUE);
    env_wchar = ga_env.ga_ದತ್ತಾಂಶ;

    ಆದರೆ (!CreatePipe(&i_theirs, &i_ours, ಖಾಲಿ, 0))
	ಹೋಗು failed;
    ಆದರೆ (!CreatePipe(&o_ours, &o_theirs, ಖಾಲಿ, 0))
	ಹೋಗು failed;

    consize.X = ಪದ->tl_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;
    consize.Y = ಪದ->tl_ಅಡ್ಡಸಾಲುಗಳು;
    hr = pCreatePseudoConsole(consize, i_theirs, o_theirs, 0,
							     &ಪದ->tl_conpty);
    ಆದರೆ (FAILED(hr))
	ಹೋಗು failed;

    ಪದ->tl_siex.StartupInfo.cb = ಗಾತ್ರ(ಪದ->tl_siex);

    // Explicitly invalidate std handles to prevent inheritance of
    // the debugger's stdout (e.g., in Visual Studio debugging sessions),
    // which could cause job output to go to the debugger instead of
    // the intended ConPTY, even with bInheritHandles set to FALSE in CreateProcess.
    ಪದ->tl_siex.StartupInfo.dwFlags = STARTF_USESTDHANDLES;
    ಪದ->tl_siex.StartupInfo.h_Std_ಒಳಹರಿವು = INVALID_HANDLE_ಮೌಲ್ಯ;
    ಪದ->tl_siex.StartupInfo.hStdOutput = INVALID_HANDLE_ಮೌಲ್ಯ;
    ಪದ->tl_siex.StartupInfo.hStdError = INVALID_HANDLE_ಮೌಲ್ಯ;

    // Set up pipe inheritance safely: Vista or later.
    pInitializeProcThreadAttributeList(ಖಾಲಿ, 1, 0, &breq);
    ಪದ->tl_siex.lpAttributeList = alloc(breq);
    ಆದರೆ (!ಪದ->tl_siex.lpAttributeList)
	ಹೋಗು failed;
    ಆದರೆ (!pInitializeProcThreadAttributeList(ಪದ->tl_siex.lpAttributeList, 1,
								     0, &breq))
	ಹೋಗು failed;
    ಆದರೆ (!pUpdateProcThreadAttribute(
	    ಪದ->tl_siex.lpAttributeList, 0,
	    PROC_THREAD_ATTRIBUTE_PSEUDOCONSOLE, ಪದ->tl_conpty,
	    ಗಾತ್ರ(HPCON), ಖಾಲಿ, ಖಾಲಿ))
	ಹೋಗು failed;

    channel = ಕೂಡಿಸು_channel();
    ಆದರೆ (channel == ಖಾಲಿ)
	ಹೋಗು failed;

    ಕೆಲಸ = ಕೆಲಸ_alloc();
    ಆದರೆ (ಕೆಲಸ == ಖಾಲಿ)
	ಹೋಗು failed;
    ಆದರೆ (argvar->v_type == VAR_STRING)
    {
	ಪೂರ್ಣಾಂಕ argc;

	build_argv_from_string(cmd, &ಕೆಲಸ->jv_argv, &argc);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಪೂರ್ಣಾಂಕ argc;

	build_argv_from_list(argvar->vval.v_list, &ಕೆಲಸ->jv_argv, &argc);
    }

    ಆದರೆ (opt->jo_ಗಣ & JO_ಒಳ_BUF)
	ಕೆಲಸ->jv_ಒಳ_buf = buflist_findnr(opt->jo_io_buf[PART_ಒಳ]);

    ಆದರೆ (!CreateProcessW(ಖಾಲಿ, cmd_wchar_copy, ಖಾಲಿ, ಖಾಲಿ, FALSE,
	    EXTENDED_STARTUPINFO_PRESENT | CREATE_UNICODE_ENVIRONMENT
	    | CREATE_SUSPENDED | CREATE_DEFAULT_ERROR_ಬಹುಲಕ,
	    env_wchar, cwd_wchar,
	    &ಪದ->tl_siex.StartupInfo, &proc_info))
    {
	errmsg = GetWin32Error();
	ಹೋಗು failed;
    }

    CloseHandle(i_theirs);
    CloseHandle(o_theirs);

    channel_ಗಣ_pipes(channel,
	    (sock_T)i_ours,
	    (sock_T)o_ours,
	    (sock_T)o_ours);

    // Write lines with CR instead of NL.
    channel->ch_write_ಪಠ್ಯ_ಬಹುಲಕ = TRUE;

    // Use to explicitly delete anonymous pipe handle.
    // In addition, it is used to prevent the pipe from being closed when input
    // from a buffer etc. is finished.
    channel->ch_anonymous_pipe = TRUE;

    jo = Create_ಕೆಲಸ_Object(ಖಾಲಿ, ಖಾಲಿ);
    ಆದರೆ (jo == ಖಾಲಿ)
	ಹೋಗು failed;

    ಆದರೆ (!ನಿಯೋಜಿಸು_Process_ಗಮ್ಯ_ಕೆಲಸ_Object(jo, proc_info.hProcess))
    {
	// Failed, switch the way to terminate process with TerminateProcess.
	CloseHandle(jo);
	jo = ಖಾಲಿ;
    }

    ResumeThread(proc_info.hThread);
    CloseHandle(proc_info.hThread);

    vim_free(cmd_wchar);
    vim_free(cmd_wchar_copy);
    vim_free(cwd_wchar);
    vim_free(env_wchar);

    ಆದರೆ (create_vterm(ಪದ, ಪದ->tl_ಅಡ್ಡಸಾಲುಗಳು, ಪದ->tl_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ) == FAIL)
	ಹೋಗು failed;

# ಆದರೆ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    ಆದರೆ (ಪದ_use_palette())
    {
	ಆದರೆ (ಪದ->tl_palette != ಖಾಲಿ)
	    ಗಣ_vterm_palette(ಪದ->tl_vterm, ಪದ->tl_palette);
	ಇಲ್ಲದಿದ್ದರೆ
	    init_vterm_ansi_colors(ಪದ->tl_vterm);
    }
# endif

    channel_ಗಣ_ಕೆಲಸ(channel, ಕೆಲಸ, opt);
    ಕೆಲಸ_ಗಣ_options(ಕೆಲಸ, opt);

    ಕೆಲಸ->jv_channel = channel;
    ಕೆಲಸ->jv_proc_info = proc_info;
    ಕೆಲಸ->jv_ಕೆಲಸ_object = jo;
    ಕೆಲಸ->jv_ಸ್ಥಿತಿ = ಕೆಲಸ_STARTED;
    ಕೆಲಸ->jv_tty_type = vim_strsave((char_u *)"conpty");
    ++ಕೆಲಸ->jv_refcount;
    ಪದ->tl_ಕೆಲಸ = ಕೆಲಸ;

    // Redirecting stdout and stderr doesn't work at the job level.  Instead
    // open the file here and handle it in.  opt->jo_io was changed in
    // setup_job_options(), use the original flags here.
    ಆದರೆ (orig_opt->jo_io[PART_ಹೊರಹೋಗು] == JIO_FILE)
    {
	char_u *fname = opt->jo_io_name[PART_ಹೊರಹೋಗು];

	ch_log(channel, "Opening output file %s", fname);
	ಪದ->tl_ಹೊರಹೋಗು_fd = mch_fopen((ಅಕ್ಷರ *)fname, WRITEBIN);
	ಆದರೆ (ಪದ->tl_ಹೊರಹೋಗು_fd == ಖಾಲಿ)
	    semsg(_(e_cant_open_file_str), fname);
    }

    ಹಿಂತಿರುಗಿ OK;

failed:
    ga_clear(&ga_cmd);
    ga_clear(&ga_env);
    vim_free(cmd_wchar);
    vim_free(cmd_wchar_copy);
    vim_free(cwd_wchar);
    ಆದರೆ (channel != ಖಾಲಿ)
	channel_clear(channel);
    ಆದರೆ (ಕೆಲಸ != ಖಾಲಿ)
    {
	ಕೆಲಸ->jv_channel = ಖಾಲಿ;
	ಕೆಲಸ_cleanup(ಕೆಲಸ);
    }
    ಪದ->tl_ಕೆಲಸ = ಖಾಲಿ;
    ಆದರೆ (jo != ಖಾಲಿ)
	CloseHandle(jo);

    ಆದರೆ (ಪದ->tl_siex.lpAttributeList != ಖಾಲಿ)
    {
	pDeleteProcThreadAttributeList(ಪದ->tl_siex.lpAttributeList);
	vim_free(ಪದ->tl_siex.lpAttributeList);
    }
    ಪದ->tl_siex.lpAttributeList = ಖಾಲಿ;
    ಆದರೆ (o_theirs != ಖಾಲಿ)
	CloseHandle(o_theirs);
    ಆದರೆ (o_ours != ಖಾಲಿ)
	CloseHandle(o_ours);
    ಆದರೆ (i_ours != ಖಾಲಿ)
	CloseHandle(i_ours);
    ಆದರೆ (i_theirs != ಖಾಲಿ)
	CloseHandle(i_theirs);
    ಆದರೆ (ಪದ->tl_conpty != ಖಾಲಿ)
	pClosePseudoConsole(ಪದ->tl_conpty);
    ಪದ->tl_conpty = ಖಾಲಿ;
    // Propagate errors that occur in CreateProcess
    ಆದರೆ (errmsg)
	semsg("CreateProcess failed: %s", errmsg);
    ಹಿಂತಿರುಗಿ FAIL;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
conpty_ಪದ_report_winsize(ಪದ_T *ಪದ, ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲುಗಳು_2, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ)
{
    COORD consize;

    consize.X = ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;
    consize.Y = ಅಡ್ಡಸಾಲುಗಳು_2;
    pResizePseudoConsole(ಪದ->tl_conpty, consize);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ಪದ_free_conpty(ಪದ_T *ಪದ)
{
    ಆದರೆ (ಪದ->tl_siex.lpAttributeList != ಖಾಲಿ)
    {
	pDeleteProcThreadAttributeList(ಪದ->tl_siex.lpAttributeList);
	vim_free(ಪದ->tl_siex.lpAttributeList);
    }
    ಪದ->tl_siex.lpAttributeList = ಖಾಲಿ;
    ಆದರೆ (ಪದ->tl_conpty != ಖಾಲಿ)
	pClosePseudoConsole(ಪದ->tl_conpty);
    ಪದ->tl_conpty = ಖಾಲಿ;
}

    ಪೂರ್ಣಾಂಕ
use_conpty(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ has_conpty;
}

# define WINPTY_SPAWN_FLAG_AUTO_SHUTDOWN 1ul
# define WINPTY_SPAWN_FLAG_EXIT_AFTER_SHUTDOWN 2ull
# define WINPTY_MOUSE_ಬಹುಲಕ_FORCE		2

ಶೂನ್ಯ* (*winpty_config_new)(UINT64, ಶೂನ್ಯ*);
ಶೂನ್ಯ* (*winpty_open)(ಶೂನ್ಯ*, ಶೂನ್ಯ*);
ಶೂನ್ಯ* (*winpty_spawn_config_new)(UINT64, ಶೂನ್ಯ*, LPCWSTR, ಶೂನ್ಯ*, ಶೂನ್ಯ*, ಶೂನ್ಯ*);
BOOL (*winpty_spawn)(ಶೂನ್ಯ*, ಶೂನ್ಯ*, HANDLE*, HANDLE*, DWORD*, ಶೂನ್ಯ*);
ಶೂನ್ಯ (*winpty_config_ಗಣ_mouse_ಬಹುಲಕ)(ಶೂನ್ಯ*, ಪೂರ್ಣಾಂಕ);
ಶೂನ್ಯ (*winpty_config_ಗಣ_initial_ಗಾತ್ರ)(ಶೂನ್ಯ*, ಪೂರ್ಣಾಂಕ, ಪೂರ್ಣಾಂಕ);
LPCWSTR (*winpty_conin_name)(ಶೂನ್ಯ*);
LPCWSTR (*winpty_conout_name)(ಶೂನ್ಯ*);
LPCWSTR (*winpty_conerr_name)(ಶೂನ್ಯ*);
ಶೂನ್ಯ (*winpty_free)(ಶೂನ್ಯ*);
ಶೂನ್ಯ (*winpty_config_free)(ಶೂನ್ಯ*);
ಶೂನ್ಯ (*winpty_spawn_config_free)(ಶೂನ್ಯ*);
ಶೂನ್ಯ (*winpty_error_free)(ಶೂನ್ಯ*);
LPCWSTR (*winpty_error_ಸಂದೇಶ)(ಶೂನ್ಯ*);
BOOL (*winpty_ಗಣ_ಗಾತ್ರ)(ಶೂನ್ಯ*, ಪೂರ್ಣಾಂಕ, ಪೂರ್ಣಾಂಕ, ಶೂನ್ಯ*);
HANDLE (*winpty_agent_process)(ಶೂನ್ಯ*);

# define WINPTY_DLL "winpty.dll"

ಸ್ಥಿರ HINSTANCE hWinPtyDLL = ಖಾಲಿ;

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
dyn_winpty_init(ಪೂರ್ಣಾಂಕ verbose)
{
    ಪೂರ್ಣಾಂಕ i;
    ಸ್ಥಿರ ರಚನೆ
    {
	ಅಕ್ಷರ	    *name;
	FARPROC	    *ptr;
    } winpty_ನಮೂದು[] =
    {
	{"winpty_conerr_name", (FARPROC*)&winpty_conerr_name},
	{"winpty_config_free", (FARPROC*)&winpty_config_free},
	{"winpty_config_new", (FARPROC*)&winpty_config_new},
	{"winpty_config_set_mouse_mode",
				      (FARPROC*)&winpty_config_ಗಣ_mouse_ಬಹುಲಕ},
	{"winpty_config_set_initial_size",
				    (FARPROC*)&winpty_config_ಗಣ_initial_ಗಾತ್ರ},
	{"winpty_conin_name", (FARPROC*)&winpty_conin_name},
	{"winpty_conout_name", (FARPROC*)&winpty_conout_name},
	{"winpty_error_free", (FARPROC*)&winpty_error_free},
	{"winpty_free", (FARPROC*)&winpty_free},
	{"winpty_open", (FARPROC*)&winpty_open},
	{"winpty_spawn", (FARPROC*)&winpty_spawn},
	{"winpty_spawn_config_free", (FARPROC*)&winpty_spawn_config_free},
	{"winpty_spawn_config_new", (FARPROC*)&winpty_spawn_config_new},
	{"winpty_error_msg", (FARPROC*)&winpty_error_ಸಂದೇಶ},
	{"winpty_set_size", (FARPROC*)&winpty_ಗಣ_ಗಾತ್ರ},
	{"winpty_agent_process", (FARPROC*)&winpty_agent_process},
	{ಖಾಲಿ, ಖಾಲಿ}
    };

    // No need to initialize twice.
    ಆದರೆ (hWinPtyDLL)
	ಹಿಂತಿರುಗಿ OK;
    // Load winpty.dll, prefer using the 'winptydll' option, fall back to just
    // winpty.dll.
    ಆದರೆ (*p_winptydll != NUL)
	hWinPtyDLL = vim_ತುಂಬು_Lib((ಅಕ್ಷರ *)p_winptydll);
    ಆದರೆ (!hWinPtyDLL)
	hWinPtyDLL = vim_ತುಂಬು_Lib(WINPTY_DLL);
    ಆದರೆ (!hWinPtyDLL)
    {
	ಆದರೆ (verbose)
	    semsg(_(e_could_not_ತುಂಬು_library_str_str),
		    (*p_winptydll != NUL ? p_winptydll : (char_u *)WINPTY_DLL),
		    GetWin32Error());
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಗಾಗಿ (i = 0; winpty_ನಮೂದು[i].name != ಖಾಲಿ
					 && winpty_ನಮೂದು[i].ptr != ಖಾಲಿ; ++i)
    {
	ಆದರೆ ((*winpty_ನಮೂದು[i].ptr = (FARPROC)Get_Proc_ವಿಳಾಸ(hWinPtyDLL,
					      winpty_ನಮೂದು[i].name)) == ಖಾಲಿ)
	{
	    ಆದರೆ (verbose)
		semsg(_(e_could_not_ತುಂಬು_library_function_str), winpty_ನಮೂದು[i].name);
	    hWinPtyDLL = ಖಾಲಿ;
	    ಹಿಂತಿರುಗಿ FAIL;
	}
    }

    ಹಿಂತಿರುಗಿ OK;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
winpty_ಪದ_and_ಕೆಲಸ_init(
	ಪದ_T	    *ಪದ,
	typval_T    *argvar,
	ಅಕ್ಷರ	    **argv ಬಳಸದ,
	jobopt_T    *opt,
	jobopt_T    *orig_opt)
{
    WCHAR	    *cmd_wchar = ಖಾಲಿ;
    WCHAR	    *cwd_wchar = ಖಾಲಿ;
    WCHAR	    *env_wchar = ಖಾಲಿ;
    channel_T	    *channel = ಖಾಲಿ;
    ಕೆಲಸ_T	    *ಕೆಲಸ = ಖಾಲಿ;
    DWORD	    error;
    HANDLE	    jo = ಖಾಲಿ;
    HANDLE	    ಮಗು_process_handle;
    HANDLE	    ಮಗು_thread_handle;
    ಶೂನ್ಯ	    *winpty_err = ಖಾಲಿ;
    ಶೂನ್ಯ	    *spawn_config = ಖಾಲಿ;
    garray_T	    ga_cmd, ga_env;
    char_u	    *cmd = ಖಾಲಿ;

    ga_init2(&ga_cmd, ಗಾತ್ರ(ಅಕ್ಷರ*), 20);
    ga_init2(&ga_env, ಗಾತ್ರ(ಅಕ್ಷರ*), 20);

    ಆದರೆ (argvar->v_type == VAR_STRING)
    {
	cmd = argvar->vval.v_string;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argvar->v_type == VAR_LIST)
    {
	ಆದರೆ (win32_build_cmd(argvar->vval.v_list, &ga_cmd) == FAIL)
	    ಹೋಗು failed;
	cmd = ga_cmd.ga_ದತ್ತಾಂಶ;
    }
    ಆದರೆ (cmd == ಖಾಲಿ || *cmd == NUL)
    {
	emsg(_(e_invalid_ವಾದ));
	ಹೋಗು failed;
    }

    ಪದ->tl_arg0_cmd = vim_strsave(cmd);

    cmd_wchar = enc_ಗಮ್ಯ_utf16(cmd, ಖಾಲಿ);
    ga_clear(&ga_cmd);
    ಆದರೆ (cmd_wchar == ಖಾಲಿ)
	ಹೋಗು failed;
    ಆದರೆ (opt->jo_cwd != ಖಾಲಿ)
    {
	cwd_wchar = enc_ಗಮ್ಯ_utf16(opt->jo_cwd, ಖಾಲಿ);
	ಆದರೆ (cwd_wchar == ಖಾಲಿ)
	    ಹೋಗು failed;
    }

    win32_build_env(opt->jo_env, &ga_env, TRUE);
    env_wchar = ga_env.ga_ದತ್ತಾಂಶ;

    ಪದ->tl_winpty_config = winpty_config_new(0, &winpty_err);
    ಆದರೆ (ಪದ->tl_winpty_config == ಖಾಲಿ)
	ಹೋಗು failed;

    winpty_config_ಗಣ_mouse_ಬಹುಲಕ(ಪದ->tl_winpty_config,
						    WINPTY_MOUSE_ಬಹುಲಕ_FORCE);
    winpty_config_ಗಣ_initial_ಗಾತ್ರ(ಪದ->tl_winpty_config,
						 ಪದ->tl_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ, ಪದ->tl_ಅಡ್ಡಸಾಲುಗಳು);
    ಪದ->tl_winpty = winpty_open(ಪದ->tl_winpty_config, &winpty_err);
    ಆದರೆ (ಪದ->tl_winpty == ಖಾಲಿ)
	ಹೋಗು failed;

    spawn_config = winpty_spawn_config_new(
	    WINPTY_SPAWN_FLAG_AUTO_SHUTDOWN |
		WINPTY_SPAWN_FLAG_EXIT_AFTER_SHUTDOWN,
	    ಖಾಲಿ,
	    cmd_wchar,
	    cwd_wchar,
	    env_wchar,
	    &winpty_err);
    ಆದರೆ (spawn_config == ಖಾಲಿ)
	ಹೋಗು failed;

    channel = ಕೂಡಿಸು_channel();
    ಆದರೆ (channel == ಖಾಲಿ)
	ಹೋಗು failed;

    ಕೆಲಸ = ಕೆಲಸ_alloc();
    ಆದರೆ (ಕೆಲಸ == ಖಾಲಿ)
	ಹೋಗು failed;
    ಆದರೆ (argvar->v_type == VAR_STRING)
    {
	ಪೂರ್ಣಾಂಕ argc;

	build_argv_from_string(cmd, &ಕೆಲಸ->jv_argv, &argc);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಪೂರ್ಣಾಂಕ argc;

	build_argv_from_list(argvar->vval.v_list, &ಕೆಲಸ->jv_argv, &argc);
    }

    ಆದರೆ (opt->jo_ಗಣ & JO_ಒಳ_BUF)
	ಕೆಲಸ->jv_ಒಳ_buf = buflist_findnr(opt->jo_io_buf[PART_ಒಳ]);

    ಆದರೆ (!winpty_spawn(ಪದ->tl_winpty, spawn_config, &ಮಗು_process_handle,
	    &ಮಗು_thread_handle, &error, &winpty_err))
	ಹೋಗು failed;

    channel_ಗಣ_pipes(channel,
	(sock_T)CreateFileW(
	    winpty_conin_name(ಪದ->tl_winpty),
	    GENERIC_WRITE, 0, ಖಾಲಿ,
	    OPEN_EXISTING, 0, ಖಾಲಿ),
	(sock_T)CreateFileW(
	    winpty_conout_name(ಪದ->tl_winpty),
	    GENERIC_READ, 0, ಖಾಲಿ,
	    OPEN_EXISTING, 0, ಖಾಲಿ),
	(sock_T)CreateFileW(
	    winpty_conerr_name(ಪದ->tl_winpty),
	    GENERIC_READ, 0, ಖಾಲಿ,
	    OPEN_EXISTING, 0, ಖಾಲಿ));

    // Write lines with CR instead of NL.
    channel->ch_write_ಪಠ್ಯ_ಬಹುಲಕ = TRUE;

    jo = Create_ಕೆಲಸ_Object(ಖಾಲಿ, ಖಾಲಿ);
    ಆದರೆ (jo == ಖಾಲಿ)
	ಹೋಗು failed;

    ಆದರೆ (!ನಿಯೋಜಿಸು_Process_ಗಮ್ಯ_ಕೆಲಸ_Object(jo, ಮಗು_process_handle))
    {
	// Failed, switch the way to terminate process with TerminateProcess.
	CloseHandle(jo);
	jo = ಖಾಲಿ;
    }

    winpty_spawn_config_free(spawn_config);
    vim_free(cmd_wchar);
    vim_free(cwd_wchar);
    vim_free(env_wchar);

    ಆದರೆ (create_vterm(ಪದ, ಪದ->tl_ಅಡ್ಡಸಾಲುಗಳು, ಪದ->tl_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ) == FAIL)
	ಹೋಗು failed;

# ಆದರೆ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    ಆದರೆ (ಪದ_use_palette())
    {
	ಆದರೆ (ಪದ->tl_palette != ಖಾಲಿ)
	    ಗಣ_vterm_palette(ಪದ->tl_vterm, ಪದ->tl_palette);
	ಇಲ್ಲದಿದ್ದರೆ
	    init_vterm_ansi_colors(ಪದ->tl_vterm);
    }
# endif

    channel_ಗಣ_ಕೆಲಸ(channel, ಕೆಲಸ, opt);
    ಕೆಲಸ_ಗಣ_options(ಕೆಲಸ, opt);

    ಕೆಲಸ->jv_channel = channel;
    ಕೆಲಸ->jv_proc_info.hProcess = ಮಗು_process_handle;
    ಕೆಲಸ->jv_proc_info.dwProcessId = GetProcessId(ಮಗು_process_handle);
    ಕೆಲಸ->jv_ಕೆಲಸ_object = jo;
    ಕೆಲಸ->jv_ಸ್ಥಿತಿ = ಕೆಲಸ_STARTED;
    ಕೆಲಸ->jv_tty_ಒಳ = utf16_ಗಮ್ಯ_enc(
	    (short_u *)winpty_conin_name(ಪದ->tl_winpty), ಖಾಲಿ);
    ಕೆಲಸ->jv_tty_ಹೊರಹೋಗು = utf16_ಗಮ್ಯ_enc(
	    (short_u *)winpty_conout_name(ಪದ->tl_winpty), ಖಾಲಿ);
    ಕೆಲಸ->jv_tty_type = vim_strsave((char_u *)"winpty");
    ++ಕೆಲಸ->jv_refcount;
    ಪದ->tl_ಕೆಲಸ = ಕೆಲಸ;

    // Redirecting stdout and stderr doesn't work at the job level.  Instead
    // open the file here and handle it in.  opt->jo_io was changed in
    // setup_job_options(), use the original flags here.
    ಆದರೆ (orig_opt->jo_io[PART_ಹೊರಹೋಗು] == JIO_FILE)
    {
	char_u *fname = opt->jo_io_name[PART_ಹೊರಹೋಗು];

	ch_log(channel, "Opening output file %s", fname);
	ಪದ->tl_ಹೊರಹೋಗು_fd = mch_fopen((ಅಕ್ಷರ *)fname, WRITEBIN);
	ಆದರೆ (ಪದ->tl_ಹೊರಹೋಗು_fd == ಖಾಲಿ)
	    semsg(_(e_cant_open_file_str), fname);
    }

    ಹಿಂತಿರುಗಿ OK;

failed:
    ga_clear(&ga_cmd);
    ga_clear(&ga_env);
    vim_free(cmd_wchar);
    vim_free(cwd_wchar);
    ಆದರೆ (spawn_config != ಖಾಲಿ)
	winpty_spawn_config_free(spawn_config);
    ಆದರೆ (channel != ಖಾಲಿ)
	channel_clear(channel);
    ಆದರೆ (ಕೆಲಸ != ಖಾಲಿ)
    {
	ಕೆಲಸ->jv_channel = ಖಾಲಿ;
	ಕೆಲಸ_cleanup(ಕೆಲಸ);
    }
    ಪದ->tl_ಕೆಲಸ = ಖಾಲಿ;
    ಆದರೆ (jo != ಖಾಲಿ)
	CloseHandle(jo);
    ಆದರೆ (ಪದ->tl_winpty != ಖಾಲಿ)
	winpty_free(ಪದ->tl_winpty);
    ಪದ->tl_winpty = ಖಾಲಿ;
    ಆದರೆ (ಪದ->tl_winpty_config != ಖಾಲಿ)
	winpty_config_free(ಪದ->tl_winpty_config);
    ಪದ->tl_winpty_config = ಖಾಲಿ;
    ಆದರೆ (winpty_err != ಖಾಲಿ)
    {
	ಅಕ್ಷರ *ಸಂದೇಶ = (ಅಕ್ಷರ *)utf16_ಗಮ್ಯ_enc(
				(short_u *)winpty_error_ಸಂದೇಶ(winpty_err), ಖಾಲಿ);

	ಆದರೆ (ಸಂದೇಶ != ಖಾಲಿ)
	{
	    emsg(ಸಂದೇಶ);
	    vim_free(ಸಂದೇಶ);
	}
	winpty_error_free(winpty_err);
    }
    ಹಿಂತಿರುಗಿ FAIL;
}

/*
 * Create a new terminal of "rows" by "cols" cells.
 * Store a reference in "term".
 * Return OK or FAIL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಪದ_and_ಕೆಲಸ_init(
	ಪದ_T	    *ಪದ,
	typval_T    *argvar,
	ಅಕ್ಷರ	    **argv,
	jobopt_T    *opt,
	jobopt_T    *orig_opt)
{
    ಪೂರ್ಣಾಂಕ		    use_winpty = FALSE;
    ಪೂರ್ಣಾಂಕ		    use_conpty = FALSE;
    ಪೂರ್ಣಾಂಕ		    tty_type = *p_twt;

    has_winpty = dyn_winpty_init(FALSE) != FAIL ? TRUE : FALSE;
    has_conpty = dyn_conpty_init(FALSE) != FAIL ? TRUE : FALSE;

    ಆದರೆ (!has_winpty && !has_conpty)
	// If neither is available give the errors for winpty, since when
	// conpty is not available it can't be installed either.
	ಹಿಂತಿರುಗಿ dyn_winpty_init(TRUE);

    ಆದರೆ (opt->jo_tty_type != NUL)
	tty_type = opt->jo_tty_type;

    ಆದರೆ (tty_type == NUL)
    {
	ಆದರೆ (has_conpty && (is_conpty_stable() || !has_winpty))
	    use_conpty = TRUE;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (has_winpty)
	    use_winpty = TRUE;
	// else: error
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (tty_type == 'w')	// winpty
    {
	ಆದರೆ (has_winpty)
	    use_winpty = TRUE;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (tty_type == 'c')	// conpty
    {
	ಆದರೆ (has_conpty)
	    use_conpty = TRUE;
	ಇಲ್ಲದಿದ್ದರೆ
	    ಹಿಂತಿರುಗಿ dyn_conpty_init(TRUE);
    }

    ಆದರೆ (use_conpty)
	ಹಿಂತಿರುಗಿ conpty_ಪದ_and_ಕೆಲಸ_init(ಪದ, argvar, argv, opt, orig_opt);

    ಆದರೆ (use_winpty)
	ಹಿಂತಿರುಗಿ winpty_ಪದ_and_ಕೆಲಸ_init(ಪದ, argvar, argv, opt, orig_opt);

    // error
    ಹಿಂತಿರುಗಿ dyn_winpty_init(TRUE);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
create_pty_only(ಪದ_T *ಪದ, jobopt_T *options)
{
    HANDLE	    h_Pipe_ಒಳ = INVALID_HANDLE_ಮೌಲ್ಯ;
    HANDLE	    h_Pipe_ಹೊರಹೋಗು = INVALID_HANDLE_ಮೌಲ್ಯ;
    ಅಕ್ಷರ	    ಒಳ_name[80], ಹೊರಹೋಗು_name[80];
    channel_T	    *channel = ಖಾಲಿ;

    ಆದರೆ (create_vterm(ಪದ, ಪದ->tl_ಅಡ್ಡಸಾಲುಗಳು, ಪದ->tl_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    vim_snprintf(ಒಳ_name, ಗಾತ್ರ(ಒಳ_name), "\\\\.\\pipe\\vim-%d-in-%d",
	    GetCurrentProcessId(),
	    curbuf->b_fnum);
    h_Pipe_ಒಳ = CreateNamedPipe(ಒಳ_name, PIPE_ACCESS_OUTBOUND,
	    PIPE_TYPE_ಸಂದೇಶ | PIPE_NOWAIT,
	    PIPE_UNLIMITED_INSTANCES,
	    0, 0, NMPWAIT_NOWAIT, ಖಾಲಿ);
    ಆದರೆ (h_Pipe_ಒಳ == INVALID_HANDLE_ಮೌಲ್ಯ)
	ಹೋಗು failed;

    vim_snprintf(ಹೊರಹೋಗು_name, ಗಾತ್ರ(ಹೊರಹೋಗು_name), "\\\\.\\pipe\\vim-%d-out-%d",
	    GetCurrentProcessId(),
	    curbuf->b_fnum);
    h_Pipe_ಹೊರಹೋಗು = CreateNamedPipe(ಹೊರಹೋಗು_name, PIPE_ACCESS_INBOUND,
	    PIPE_TYPE_ಸಂದೇಶ | PIPE_NOWAIT,
	    PIPE_UNLIMITED_INSTANCES,
	    0, 0, 0, ಖಾಲಿ);
    ಆದರೆ (h_Pipe_ಹೊರಹೋಗು == INVALID_HANDLE_ಮೌಲ್ಯ)
	ಹೋಗು failed;

    ConnectNamedPipe(h_Pipe_ಒಳ, ಖಾಲಿ);
    ConnectNamedPipe(h_Pipe_ಹೊರಹೋಗು, ಖಾಲಿ);

    ಪದ->tl_ಕೆಲಸ = ಕೆಲಸ_alloc();
    ಆದರೆ (ಪದ->tl_ಕೆಲಸ == ಖಾಲಿ)
	ಹೋಗು failed;
    ++ಪದ->tl_ಕೆಲಸ->jv_refcount;

    // behave like the job is already finished
    ಪದ->tl_ಕೆಲಸ->jv_ಸ್ಥಿತಿ = ಕೆಲಸ_ಪೂರ್ಣಗೊಂಡ;

    channel = ಕೂಡಿಸು_channel();
    ಆದರೆ (channel == ಖಾಲಿ)
	ಹೋಗು failed;
    ಪದ->tl_ಕೆಲಸ->jv_channel = channel;
    channel->ch_keep_open = TRUE;
    channel->ch_named_pipe = TRUE;

    channel_ಗಣ_pipes(channel,
	(sock_T)h_Pipe_ಒಳ,
	(sock_T)h_Pipe_ಹೊರಹೋಗು,
	(sock_T)h_Pipe_ಹೊರಹೋಗು);
    channel_ಗಣ_ಕೆಲಸ(channel, ಪದ->tl_ಕೆಲಸ, options);
    ಪದ->tl_ಕೆಲಸ->jv_tty_ಒಳ = vim_strsave((char_u*)ಒಳ_name);
    ಪದ->tl_ಕೆಲಸ->jv_tty_ಹೊರಹೋಗು = vim_strsave((char_u*)ಹೊರಹೋಗು_name);

    ಹಿಂತಿರುಗಿ OK;

failed:
    ಆದರೆ (h_Pipe_ಒಳ != ಖಾಲಿ)
	CloseHandle(h_Pipe_ಒಳ);
    ಆದರೆ (h_Pipe_ಹೊರಹೋಗು != ಖಾಲಿ)
	CloseHandle(h_Pipe_ಹೊರಹೋಗು);
    ಹಿಂತಿರುಗಿ FAIL;
}

/*
 * Free the terminal emulator part of "term".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಪದ_free_vterm(ಪದ_T *ಪದ)
{
    ಪದ_free_conpty(ಪದ);
    ಆದರೆ (ಪದ->tl_winpty != ಖಾಲಿ)
	winpty_free(ಪದ->tl_winpty);
    ಪದ->tl_winpty = ಖಾಲಿ;
    ಆದರೆ (ಪದ->tl_winpty_config != ಖಾಲಿ)
	winpty_config_free(ಪದ->tl_winpty_config);
    ಪದ->tl_winpty_config = ಖಾಲಿ;
    ಆದರೆ (ಪದ->tl_vterm != ಖಾಲಿ)
	vterm_free(ಪದ->tl_vterm);
    ಪದ->tl_vterm = ಖಾಲಿ;
}

/*
 * Report the size to the terminal.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಪದ_report_winsize(ಪದ_T *ಪದ, ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲುಗಳು_2, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ)
{
    ಆದರೆ (ಪದ->tl_conpty)
	conpty_ಪದ_report_winsize(ಪದ, ಅಡ್ಡಸಾಲುಗಳು_2, ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ);
    ಆದರೆ (ಪದ->tl_winpty)
	winpty_ಗಣ_ಗಾತ್ರ(ಪದ->tl_winpty, ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ, ಅಡ್ಡಸಾಲುಗಳು_2, ಖಾಲಿ);
}

    ಪೂರ್ಣಾಂಕ
terminal_enabled(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ dyn_winpty_init(FALSE) == OK || dyn_conpty_init(FALSE) == OK;
}

#ಇಲ್ಲದಿದ್ದರೆ

///////////////////////////////////////
// 3. Unix-like implementation.

/*
 * Create a new terminal of "rows" by "cols" cells.
 * Start job for "cmd".
 * Store the pointers in "term".
 * When "argv" is not NULL then "argvar" is not used.
 * Return OK or FAIL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಪದ_and_ಕೆಲಸ_init(
	ಪದ_T	    *ಪದ,
	typval_T    *argvar,
	ಅಕ್ಷರ	    **argv,
	jobopt_T    *opt,
	jobopt_T    *orig_opt ಬಳಸದ)
{
    ಪದ->tl_arg0_cmd = ಖಾಲಿ;

    ಆದರೆ (create_vterm(ಪದ, ಪದ->tl_ಅಡ್ಡಸಾಲುಗಳು, ಪದ->tl_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

# ಆದರೆ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    ಆದರೆ (ಪದ_use_palette())
    {
	ಆದರೆ (ಪದ->tl_palette != ಖಾಲಿ)
	    ಗಣ_vterm_palette(ಪದ->tl_vterm, ಪದ->tl_palette);
	ಇಲ್ಲದಿದ್ದರೆ
	    init_vterm_ansi_colors(ಪದ->tl_vterm);
    }
# endif

    // This may change a string in "argvar".
    ಪದ->tl_ಕೆಲಸ = ಕೆಲಸ_ಆರಂಭ(argvar, argv, opt, &ಪದ->tl_ಕೆಲಸ);
    ಆದರೆ (ಪದ->tl_ಕೆಲಸ != ಖಾಲಿ)
	++ಪದ->tl_ಕೆಲಸ->jv_refcount;

    ಹಿಂತಿರುಗಿ ಪದ->tl_ಕೆಲಸ != ಖಾಲಿ
	&& ಪದ->tl_ಕೆಲಸ->jv_channel != ಖಾಲಿ
	&& ಪದ->tl_ಕೆಲಸ->jv_ಸ್ಥಿತಿ != ಕೆಲಸ_FAILED ? OK : FAIL;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
create_pty_only(ಪದ_T *ಪದ, jobopt_T *opt)
{
    ಆದರೆ (create_vterm(ಪದ, ಪದ->tl_ಅಡ್ಡಸಾಲುಗಳು, ಪದ->tl_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    ಪದ->tl_ಕೆಲಸ = ಕೆಲಸ_alloc();
    ಆದರೆ (ಪದ->tl_ಕೆಲಸ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    ++ಪದ->tl_ಕೆಲಸ->jv_refcount;

    // behave like the job is already finished
    ಪದ->tl_ಕೆಲಸ->jv_ಸ್ಥಿತಿ = ಕೆಲಸ_ಪೂರ್ಣಗೊಂಡ;

    ಹಿಂತಿರುಗಿ mch_create_pty_channel(ಪದ->tl_ಕೆಲಸ, opt);
}

/*
 * Free the terminal emulator part of "term".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಪದ_free_vterm(ಪದ_T *ಪದ)
{
    ಆದರೆ (ಪದ->tl_vterm != ಖಾಲಿ)
	vterm_free(ಪದ->tl_vterm);
    ಪದ->tl_vterm = ಖಾಲಿ;
}

/*
 * Report the size to the terminal.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಪದ_report_winsize(ಪದ_T *ಪದ, ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲುಗಳು_2, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ)
{
    // Use an ioctl() to report the new window size to the job.
    ಆದರೆ (ಪದ->tl_ಕೆಲಸ == ಖಾಲಿ || ಪದ->tl_ಕೆಲಸ->jv_channel == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಪೂರ್ಣಾಂಕ fd = -1;
    ಪೂರ್ಣಾಂಕ part;

    ಗಾಗಿ (part = PART_ಹೊರಹೋಗು; part < PART_ಎಣಿಕೆ; ++part)
    {
	fd = ಪದ->tl_ಕೆಲಸ->jv_channel->ch_part[part].ch_fd;
	ಆದರೆ (mch_isatty(fd))
	    ನಿಲ್ಲಿಸು;
    }
    ಆದರೆ (part < PART_ಎಣಿಕೆ && mch_report_winsize(fd, ಅಡ್ಡಸಾಲುಗಳು_2, ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ) == OK)
	mch_signal_ಕೆಲಸ(ಪದ->tl_ಕೆಲಸ, (char_u *)"winch");
}

#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ // FEAT_TERMINAL
