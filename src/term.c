/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */
/*
 *
 * term.c: functions for controlling the terminal
 *
 * primitive termcap support for Amiga and Win32 included
 *
 * NOTE: padding and variable substitution is not performed,
 * when compiling without HAVE_TGETENT, we use tputs() and tgoto() dummies.
 */

/*
 * Some systems have a prototype for tgetstr() with (char *) instead of
 * (char **). This define removes that prototype. We include our own prototype
 * below.
 */
#define tgetstr tgetstr_defined_wrong

#ಅನ್ತರ್ಭಾವ "vim.h"

#ifdef HAVE_TGETENT
# ifdef HAVE_TERMIOS_H
#  include <termios.h>	    // seems to be required for some Linux
# endif
# ifdef HAVE_TERMCAP_H
#  include <termcap.h>
# endif

/*
 * A few linux systems define outfuntype in termcap.h to be used as the third
 * argument for tputs().
 */
# ಯದಿ defined(VMS) || defined(AMIGA)
#  define TPUTSFUNCAST (ಶೂನ್ಯ (*)(unsigned ಪೂರ್ಣಾಙ್ಕ))
# ಅನ್ಯಥಾ
#  ifdef HAVE_OUTFUNTYPE
#   define TPUTSFUNCAST (outfuntype)
#  ಅನ್ಯಥಾ
#   define TPUTSFUNCAST (ಪೂರ್ಣಾಙ್ಕ (*)(ಪೂರ್ಣಾಙ್ಕ))
#  endif
# endif
#endif

#undef tgetstr

// start of keys that are not directly used by Vim but can be mapped
#define BT_EXTRA_ಕೀಲಿಗಳು	0x101

ಸ್ಥಿರ ಶೂನ್ಯ parse_builtin_tcap(char_u *s);
ಸ್ಥಿರ ಶೂನ್ಯ gather_termleader(ಶೂನ್ಯ);
#ifdef FEAT_TERMRESPONSE
ಸ್ಥಿರ ಶೂನ್ಯ req_codes_from_term(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ req_more_codes_from_term(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ got_code_from_term(char_u *code, ಪೂರ್ಣಾಙ್ಕ len);
ಸ್ಥಿರ ಶೂನ್ಯ check_for_codes_from_term(ಶೂನ್ಯ);
#endif
ಸ್ಥಿರ ಶೂನ್ಯ del_termcode_idx(ಪೂರ್ಣಾಙ್ಕ idx);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ find_term_bykeys(char_u *src, ಪೂರ್ಣಾಙ್ಕ *len);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ term_is_builtin(char_u *name);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ term_7to8bit(char_u *p);
ಸ್ಥಿರ ಶೂನ್ಯ accept_modifiers_for_function_ಕೀಲಿಗಳು(ಶೂನ್ಯ);

#ಯದಿ 0  // Change to 1 to enable ch_log() calls for termresponse debugging.
# define DEBUG_TERMRESPONSE
# define LOG_TR1(str) \
		ch_log(ರಿಕ್ತ, "TermResp: %s " str, \
			must_redraw == UPD_NOT_VALID ? "NV" \
			: must_redraw == UPD_CLEAR ? "CL" : "  ")
# define LOG_TRN(fmt,...) \
		ch_log(ರಿಕ್ತ, "TermResp: %s " fmt, \
			must_redraw == UPD_NOT_VALID ? "NV" \
			: must_redraw == UPD_CLEAR ? "CL" : "  ", __VA_ARGS__)
#ಅನ್ಯಥಾ
# define LOG_TR1(str) ಕುರು { /**/ } ಯಾವತ್ (0)
# define LOG_TRN(fmt,...) ಕುರು { /**/ } ಯಾವತ್ (0)
#endif

#ifdef HAVE_TGETENT
ಸ್ಥಿರ ವರ್ಣ *invoke_tgetent(char_u *, char_u *);

/*
 * Here is our own prototype for tgetstr(), any prototypes from the include
 * files have been disabled by the define at the start of this file.
 */
ವರ್ಣ		*tgetstr(ವರ್ಣ *, ವರ್ಣ **);
#endif

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ {
    STATUS_GET,		// send request when switching to RAW mode
    STATUS_SENT,	// did send request, checking for response
    STATUS_GOT,		// received response
    STATUS_FAIL		// timed out
} request_progress_T;

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
    request_progress_T	    tr_progress;
    time_t		    tr_start;	// when request was sent, -1 for never
} termrequest_T;

// Holds state for current OSC response.
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ
{
    ಪೂರ್ಣಾಙ್ಕ		    processing;	// If we are in the middle of an OSC response
    char_u	    start_char;	// First char in the OSC response
    garray_T	    buf;	// Buffer holding the OSC response, to be
				// placed in the "v:termosc" vim var.

#ifdef ELAPSED_FUNC
    elapsed_T	    start_tv;	// Set at the beginning of an OSC response.
				// Used to timeout after a set amount of
				// time.
#endif
} oscstate_T;

#define TERMREQUEST_INIT {STATUS_GET, -1}

// Request Terminal Version status:
ಸ್ಥಿರ termrequest_T crv_status = TERMREQUEST_INIT;

// Request Cursor position report:
ಸ್ಥಿರ termrequest_T u7_status = TERMREQUEST_INIT;

// Request xterm compatibility check:
ಸ್ಥಿರ termrequest_T xcc_status = TERMREQUEST_INIT;

#ifdef FEAT_TERMRESPONSE
# ifdef FEAT_TERMINAL
// Request foreground color report:
ಸ್ಥಿರ termrequest_T rfg_status = TERMREQUEST_INIT;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ fg_r = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ fg_g = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ fg_b = 0;
# endif
// Background color values from the OSC 11 response, also used by the
// image backend to flatten RGBA alpha onto the actual terminal background.
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ bg_r = 255;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ bg_g = 255;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ bg_b = 255;

// Request background color report:
ಸ್ಥಿರ termrequest_T rbg_status = TERMREQUEST_INIT;

// Request cursor blinking mode report:
ಸ್ಥಿರ termrequest_T rbm_status = TERMREQUEST_INIT;

// Request cursor style report:
ಸ್ಥಿರ termrequest_T rcs_status = TERMREQUEST_INIT;

// Request window's position report:
ಸ್ಥಿರ termrequest_T winpos_status = TERMREQUEST_INIT;

// Request DECRQM (DEC mode) report:
ಸ್ಥಿರ termrequest_T decrqm_status = TERMREQUEST_INIT;

ಸ್ಥಿರ termrequest_T *all_termrequests[] = {
    &crv_status,
    &u7_status,
    &xcc_status,
# ifdef FEAT_TERMINAL
    &rfg_status,
# endif
    &rbg_status,
    &rbm_status,
    &rcs_status,
    &winpos_status,
    &decrqm_status,
    ರಿಕ್ತ
};

// The t_8u code may default to a value but get reset when the term response is
// received.  To avoid redrawing too often, only redraw when t_8u is not reset
// and it was supposed to be written.  Unless t_8u was set explicitly.
// FALSE -> don't output t_8u yet
// MAYBE -> tried outputting t_8u while FALSE
// OK    -> can write t_8u
ಪೂರ್ಣಾಙ್ಕ write_t_8u_state = FALSE;
#endif

#ifdef HAVE_TGETENT
/*
 * Don't declare these variables if termcap.h contains them.
 * Autoconf checks if these variables should be declared extern (not all
 * systems have them).
 * Some versions define ospeed to be speed_t, but that is incompatible with
 * BSD, where ospeed is short and speed_t is long.
 */
# ifndef HAVE_OSPEED
#  ifdef OSPEED_EXTERN
ಬಾಹ್ಯ short ospeed;
#  ಅನ್ಯಥಾ
short ospeed;
#  endif
# endif
# ifndef HAVE_UP_BC_PC
#  ifdef UP_BC_PC_EXTERN
ಬಾಹ್ಯ ವರ್ಣ *UP, *BC, PC;
#  ಅನ್ಯಥಾ
ವರ್ಣ *UP, *BC, PC;
#  endif
# endif

# define TGETSTR(s, p)	vim_tgetstr((s), (p))
# define TGETENT(b, t)	tgetent((ವರ್ಣ *)(b), (ವರ್ಣ *)(t))
ಸ್ಥಿರ char_u *vim_tgetstr(ವರ್ಣ *s, char_u **pp);
#endif // HAVE_TGETENT

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ  detected_8bit = FALSE;	// detected 8-bit terminal

#ಯದಿ (defined(UNIX) || defined(VMS))
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ focus_state = MAYBE; // TRUE if the Vim window has focus
#endif

#ifdef FEAT_TERMRESPONSE
// When the cursor shape was detected these values are used:
// 1: block, 2: underline, 3: vertical bar
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ initial_cursor_shape = 0;

// The blink flag from the style response may be inverted from the actual
// blinking state, xterm XORs the flags.
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ initial_cursor_shape_blink = FALSE;

// The blink flag from the blinking-cursor mode response
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ initial_cursor_blink = FALSE;
#endif

// 0	Mode is not recognized	not supported
//
// 1	Set			supported and screen updates are not shown to
//				the user until mode is disabled
//
// 2	Reset			supported and screen updates are shown as usual
//				(e.g. as soon as they arrive)
//
// 3	Permanently set		undefined
//
// 4	Permanently reset	not supported
//
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sync_output_setting = 0;

// > 0: Currently batching output
// == 0: No synchronized output
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sync_output_state = 0;

#ifdef UNIX
// DEC mode 2048 (in-band window resize events)
// https://gist.github.com/rockorager/e695fb2924d36b2bcf1fff4a3704bd83
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ win_resize_setting = 0;
ಸ್ಥಿರ bool win_resize_enabled = false;
#endif

/*
 * The builtin termcap entries.
 *
 * The entries are also included when HAVE_TGETENT is defined, the system
 * termcap may be incomplete and a few Vim-specific entries are added.
 *
 * When HAVE_TGETENT is defined, the builtin entries can be accessed with
 * "builtin_amiga", "builtin_ansi", "builtin_debug", etc.
 *
 * Each termcap is a list of tcap_entry_T.  See parse_builtin_tcap() for all
 * details.
 *
 * Entries marked with "guessed" may be wrong.
 */
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ
{
    ಪೂರ್ಣಾಙ್ಕ		bt_entry;   // either a KS_xxx code (>= 0), or a K_xxx code.
    ವರ್ಣ	*bt_string; //ಮೌಲ್ಯ
} tcap_entry_T;

/*
 * Standard ANSI terminal, default for Unix.
 */
ಸ್ಥಿರ tcap_entry_T builtin_ansi[] = {
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CE,	"\033[K"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_AL,	"\033[L"},
#ifdef TERMINFO
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CAL,	"\033[%p1%dL"},
#ಅನ್ಯಥಾ
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CAL,	"\033[%dL"},
#endif
    {(ಪೂರ್ಣಾಙ್ಕ)KS_DL,	"\033[M"},
#ifdef TERMINFO
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CDL,	"\033[%p1%dM"},
#ಅನ್ಯಥಾ
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CDL,	"\033[%dM"},
#endif
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CL,	"\033[H\033[2J"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_ME,	"\033[0m"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_MR,	"\033[7m"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_MS,	"y"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_UT,	"y"},		// guessed
    {(ಪೂರ್ಣಾಙ್ಕ)KS_LE,	"\b"},
#ifdef TERMINFO
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CM,	"\033[%i%p1%d;%p2%dH"},
#ಅನ್ಯಥಾ
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CM,	"\033[%i%d;%dH"},
#endif
#ifdef TERMINFO
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CRI,	"\033[%p1%dC"},
#ಅನ್ಯಥಾ
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CRI,	"\033[%dC"},
#endif

    {(ಪೂರ್ಣಾಙ್ಕ)KS_NAME,	ರಿಕ್ತ}  // end marker
};

/*
 * VT320 is working as an ANSI terminal compatible DEC terminal.
 * (it covers VT1x0, VT2x0 and VT3x0 up to VT320 on VMS as well)
 * TODO:- rewrite ESC[ codes to CSI
 *      - keyboard languages (CSI ? 26 n)
 */
ಸ್ಥಿರ tcap_entry_T builtin_vt320[] = {
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CE,	"\033[K"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_AL,	"\033[L"},
#ifdef TERMINFO
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CAL,	"\033[%p1%dL"},
#ಅನ್ಯಥಾ
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CAL,	"\033[%dL"},
#endif
    {(ಪೂರ್ಣಾಙ್ಕ)KS_DL,	"\033[M"},
#ifdef TERMINFO
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CDL,	"\033[%p1%dM"},
#ಅನ್ಯಥಾ
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CDL,	"\033[%dM"},
#endif
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CL,	"\033[H\033[2J"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CD,	"\033[J"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CCO,	"8"},			// allow 8 colors
    {(ಪೂರ್ಣಾಙ್ಕ)KS_ME,	"\033[0m"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_MR,	"\033[7m"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_MD,	"\033[1m"},  // bold mode
    {(ಪೂರ್ಣಾಙ್ಕ)KS_SE,	"\033[22m"},// normal mode
    {(ಪೂರ್ಣಾಙ್ಕ)KS_UE,	"\033[24m"},// exit underscore mode
    {(ಪೂರ್ಣಾಙ್ಕ)KS_US,	"\033[4m"},  // underscore mode
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CZH,	"\033[34;43m"},  // italic mode: blue text on yellow
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CZR,	"\033[0m"},	    // italic mode end
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CAB,	"\033[4%dm"},	    // set background color (ANSI)
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CAF,	"\033[3%dm"},	    // set foreground color (ANSI)
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CSB,	"\033[102;%dm"},	// set screen background color
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CSF,	"\033[101;%dm"},	// set screen foreground color
    {(ಪೂರ್ಣಾಙ್ಕ)KS_MS,	"y"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_UT,	"y"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_XN,	"y"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_LE,	"\b"},
#ifdef TERMINFO
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CM,	"\033[%i%p1%d;%p2%dH"},
#ಅನ್ಯಥಾ
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CM,	"\033[%i%d;%dH"},
#endif
#ifdef TERMINFO
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CRI,	"\033[%p1%dC"},
#ಅನ್ಯಥಾ
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CRI,	"\033[%dC"},
#endif
    {K_UP,		"\033[A"},
    {K_DOWN,		"\033[B"},
    {K_ಬಲ,		"\033[C"},
    {K_ಎಡ,		"\033[D"},
    // Note: cursor key sequences for application cursor mode are omitted,
    // because they interfere with typed commands: <Esc>OA.
    {K_F1,		"\033[11~"},
    {K_F2,		"\033[12~"},
    {K_F3,		"\033[13~"},
    {K_F4,		"\033[14~"},
    {K_F5,		"\033[15~"},
    {K_F6,		"\033[17~"},
    {K_F7,		"\033[18~"},
    {K_F8,		"\033[19~"},
    {K_F9,		"\033[20~"},
    {K_F10,		"\033[21~"},
    {K_F11,		"\033[23~"},
    {K_F12,		"\033[24~"},
    {K_F13,		"\033[25~"},
    {K_F14,		"\033[26~"},
    {K_F15,		"\033[28~"},	// Help
    {K_F16,		"\033[29~"},	// Select
    {K_F17,		"\033[31~"},
    {K_F18,		"\033[32~"},
    {K_F19,		"\033[33~"},
    {K_F20,		"\033[34~"},
    {K_INS,		"\033[2~"},
    {K_DEL,		"\033[3~"},
    {K_HOME,		"\033[1~"},
    {K_END,		"\033[4~"},
    {K_PAGEUP,		"\033[5~"},
    {K_PAGEDOWN,	"\033[6~"},
    // These sequences starting with <Esc> O may interfere with what the user
    // is typing.  Remove these if that bothers you.
    {K_KPLUS,		"\033Ok"},	// keypad plus
    {K_KMINUS,		"\033Om"},	// keypad minus
    {K_KDIVIDE,		"\033Oo"},	// keypad /
    {K_KMULTIPLY,	"\033Oj"},	// keypad *
    {K_KENTER,		"\033OM"},	// keypad Enter
    {K_K0,		"\033Op"},	// keypad 0
    {K_K1,		"\033Oq"},	// keypad 1
    {K_K2,		"\033Or"},	// keypad 2
    {K_K3,		"\033Os"},	// keypad 3
    {K_K4,		"\033Ot"},	// keypad 4
    {K_K5,		"\033Ou"},	// keypad 5
    {K_K6,		"\033Ov"},	// keypad 6
    {K_K7,		"\033Ow"},	// keypad 7
    {K_K8,		"\033Ox"},	// keypad 8
    {K_K9,		"\033Oy"},	// keypad 9
    {K_BS,		"\x7f"},	// for some reason 0177 doesn't work

    {(ಪೂರ್ಣಾಙ್ಕ)KS_NAME,	ರಿಕ್ತ}  // end marker
};

/*
 * Ordinary vt52
 */
ಸ್ಥಿರ tcap_entry_T builtin_vt52[] = {
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CE,	"\033K"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CD,	"\033J"},
#ifdef TERMINFO
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CM,	"\033Y%p1%' '%+%c%p2%' '%+%c"},
#ಅನ್ಯಥಾ
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CM,	"\033Y%+ %+ "},
#endif
    {(ಪೂರ್ಣಾಙ್ಕ)KS_LE,	"\b"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_SR,	"\033I"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_AL,	"\033L"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_DL,	"\033M"},
    {K_UP,		"\033A"},
    {K_DOWN,		"\033B"},
    {K_ಎಡ,		"\033D"},
    {K_ಬಲ,		"\033C"},
    {K_F1,		"\033P"},
    {K_F2,		"\033Q"},
    {K_F3,		"\033R"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CL,	"\033H\033J"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_MS,	"y"},

    {(ಪೂರ್ಣಾಙ್ಕ)KS_NAME,	ರಿಕ್ತ}  // end marker
};

/*
 * Builtin xterm with Vim-specific entries.
 */
ಸ್ಥಿರ tcap_entry_T builtin_xterm[] = {
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CE,	"\033[K"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_AL,	"\033[L"},
#ifdef TERMINFO
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CAL,	"\033[%p1%dL"},
#ಅನ್ಯಥಾ
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CAL,	"\033[%dL"},
#endif
    {(ಪೂರ್ಣಾಙ್ಕ)KS_DL,	"\033[M"},
#ifdef TERMINFO
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CDL,	"\033[%p1%dM"},
#ಅನ್ಯಥಾ
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CDL,	"\033[%dM"},
#endif
#ifdef TERMINFO
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CS,	"\033[%i%p1%d;%p2%dr"},
#ಅನ್ಯಥಾ
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CS,	"\033[%i%d;%dr"},
#endif
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CL,	"\033[H\033[2J"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CD,	"\033[J"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_ME,	"\033[m"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_MR,	"\033[7m"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_MD,	"\033[1m"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_UE,	"\033[m"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_US,	"\033[4m"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_STE,	"\033[29m"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_STS,	"\033[9m"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_MS,	"y"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_UT,	"y"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_LE,	"\b"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_VI,	"\033[?25l"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_VE,	"\033[?25h"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_VS,	"\033[?12h"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CVS,	"\033[?12l"},
#ifdef TERMINFO
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CSH,	"\033[%p1%d q"},
#ಅನ್ಯಥಾ
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CSH,	"\033[%d q"},
#endif
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CRC,	"\033[?12$p"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CRS,	"\033P$q q\033\\"},
#ifdef TERMINFO
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CM,	"\033[%i%p1%d;%p2%dH"},
#ಅನ್ಯಥಾ
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CM,	"\033[%i%d;%dH"},
#endif
    {(ಪೂರ್ಣಾಙ್ಕ)KS_SR,	"\033M"},
#ifdef TERMINFO
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CRI,	"\033[%p1%dC"},
#ಅನ್ಯಥಾ
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CRI,	"\033[%dC"},
#endif
    {(ಪೂರ್ಣಾಙ್ಕ)KS_KS,	"\033[?1h\033="},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_KE,	"\033[?1l\033>"},
#ifdef FEAT_XTERM_SAVE
    {(ಪೂರ್ಣಾಙ್ಕ)KS_TI,	"\0337\033[?47h"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_TE,	"\033[?47l\0338"},
#endif
    // These are now under control of the 'keyprotocol' option, see
    // "builtin_mok2".
    // {(int)KS_CTI,	"\033[>4;2m"},
    // {(int)KS_CRK,	"\033[?4m"},
    // {(int)KS_CTE,	"\033[>4;m"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CIS,	"\033]1;"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CIE,	"\007"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_TS,	"\033]2;"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_FS,	"\007"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CSC,	"\033]12;"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CEC,	"\007"},
#ifdef TERMINFO
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CWS,	"\033[8;%p1%d;%p2%dt"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CWP,	"\033[3;%p1%d;%p2%dt"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CGP,	"\033[13t"},
#ಅನ್ಯಥಾ
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CWS,	"\033[8;%d;%dt"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CWP,	"\033[3;%d;%dt"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CGP,	"\033[13t"},
#endif
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CRV,	"\033[>c"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CXM,	"\033[?1006;1000%?%p1%{1}%=%th%el%;"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_RFG,	"\033]10;?\007"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_RBG,	"\033]11;?\007"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_U7,	"\033[6n"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CAU,	"\033[58;5;%dm"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CBE,	"\033[?2004h"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CBD,	"\033[?2004l"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CST,	"\033[22;2t"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CRT,	"\033[23;2t"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_SSI,	"\033[22;1t"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_SRI,	"\033[23;1t"},
#ಯದಿ (defined(UNIX) || defined(VMS))
    {(ಪೂರ್ಣಾಙ್ಕ)KS_FD,	"\033[?1004l"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_FE,	"\033[?1004h"},
#endif

    {K_UP,		"\033O*A"},
    {K_DOWN,		"\033O*B"},
    {K_ಬಲ,		"\033O*C"},
    {K_ಎಡ,		"\033O*D"},
    // An extra set of cursor keys for vt100 mode
    {K_XUP,		"\033[@;*A"},	// Esc [ A or Esc [ 1 ; A
    {K_XDOWN,		"\033[@;*B"},	// Esc [ B or Esc [ 1 ; B
    {K_XRIGHT,		"\033[@;*C"},	// Esc [ C or Esc [ 1 ; C
    {K_XLEFT,		"\033[@;*D"},	// Esc [ D or Esc [ 1 ; D
    // An extra set of function keys for vt100 mode
    {K_XF1,		"\033O*P"},
    {K_XF2,		"\033O*Q"},
    {K_XF3,		"\033O*R"},
    {K_XF4,		"\033O*S"},
    {K_F1,		"\033[11;*~"},
    {K_F2,		"\033[12;*~"},
    {K_F3,		"\033[13;*~"},
    {K_F4,		"\033[14;*~"},
    {K_F5,		"\033[15;*~"},
    {K_F6,		"\033[17;*~"},
    {K_F7,		"\033[18;*~"},
    {K_F8,		"\033[19;*~"},
    {K_F9,		"\033[20;*~"},
    {K_F10,		"\033[21;*~"},
    {K_F11,		"\033[23;*~"},
    {K_F12,		"\033[24;*~"},
    {K_S_TAB,		"\033[Z"},
    {K_HELP,		"\033[28;*~"},
    {K_UNDO,		"\033[26;*~"},
    {K_INS,		"\033[2;*~"},
    {K_HOME,		"\033[@;*H"},    // Esc [ H  or Esc 1 ; H
    // {K_S_HOME,		"\033O2H"},
    // {K_C_HOME,		"\033O5H"},
    {K_KHOME,		"\033[1;*~"},
    {K_XHOME,		"\033O*H"},	// other Home
    {K_ZHOME,		"\033[7;*~"},	// other Home
    {K_END,		"\033[@;*F"},	// Esc [ F or Esc 1 ; F
    // {K_S_END,		"\033O2F"},
    // {K_C_END,		"\033O5F"},
    {K_KEND,		"\033[4;*~"},
    {K_XEND,		"\033O*F"},	// other End
    {K_ZEND,		"\033[8;*~"},
    {K_PAGEUP,		"\033[5;*~"},
    {K_PAGEDOWN,	"\033[6;*~"},
    {K_KPLUS,		"\033O*k"},     // keypad plus
    {K_KMINUS,		"\033O*m"},     // keypad minus
    {K_KDIVIDE,		"\033O*o"},     // keypad /
    {K_KMULTIPLY,	"\033O*j"},     // keypad *
    {K_KENTER,		"\033O*M"},     // keypad Enter
    {K_KPOINT,		"\033O*n"},     // keypad .
    {K_K0,		"\033O*p"},     // keypad 0
    {K_K1,		"\033O*q"},     // keypad 1
    {K_K2,		"\033O*r"},     // keypad 2
    {K_K3,		"\033O*s"},     // keypad 3
    {K_K4,		"\033O*t"},     // keypad 4
    {K_K5,		"\033O*u"},     // keypad 5
    {K_K6,		"\033O*v"},     // keypad 6
    {K_K7,		"\033O*w"},     // keypad 7
    {K_K8,		"\033O*x"},     // keypad 8
    {K_K9,		"\033O*y"},     // keypad 9
    {K_KDEL,		"\033[3;*~"}, // keypad Del
    {K_PS,		"\033[200~"}, // paste start
    {K_PE,		"\033[201~"}, // paste end

    {BT_EXTRA_ಕೀಲಿಗಳು,   ""},
    {TERMCAP2_ಕೀಲಿ('k', '0'), "\033[10;*~"}, // F0
    {TERMCAP2_ಕೀಲಿ('F', '3'), "\033[25;*~"}, // F13
    // F14 and F15 are missing, because they send the same codes as the undo
    // and help key, although they don't work on all keyboards.
    {TERMCAP2_ಕೀಲಿ('F', '6'), "\033[29;*~"}, // F16
    {TERMCAP2_ಕೀಲಿ('F', '7'), "\033[31;*~"}, // F17
    {TERMCAP2_ಕೀಲಿ('F', '8'), "\033[32;*~"}, // F18
    {TERMCAP2_ಕೀಲಿ('F', '9'), "\033[33;*~"}, // F19
    {TERMCAP2_ಕೀಲಿ('F', 'A'), "\033[34;*~"}, // F20

    {TERMCAP2_ಕೀಲಿ('F', 'B'), "\033[42;*~"}, // F21
    {TERMCAP2_ಕೀಲಿ('F', 'C'), "\033[43;*~"}, // F22
    {TERMCAP2_ಕೀಲಿ('F', 'D'), "\033[44;*~"}, // F23
    {TERMCAP2_ಕೀಲಿ('F', 'E'), "\033[45;*~"}, // F24
    {TERMCAP2_ಕೀಲಿ('F', 'F'), "\033[46;*~"}, // F25
    {TERMCAP2_ಕೀಲಿ('F', 'G'), "\033[47;*~"}, // F26
    {TERMCAP2_ಕೀಲಿ('F', 'H'), "\033[48;*~"}, // F27
    {TERMCAP2_ಕೀಲಿ('F', 'I'), "\033[49;*~"}, // F28
    {TERMCAP2_ಕೀಲಿ('F', 'J'), "\033[50;*~"}, // F29
    {TERMCAP2_ಕೀಲಿ('F', 'K'), "\033[51;*~"}, // F30

    {TERMCAP2_ಕೀಲಿ('F', 'L'), "\033[52;*~"}, // F31
    {TERMCAP2_ಕೀಲಿ('F', 'M'), "\033[53;*~"}, // F32
    {TERMCAP2_ಕೀಲಿ('F', 'N'), "\033[54;*~"}, // F33
    {TERMCAP2_ಕೀಲಿ('F', 'O'), "\033[55;*~"}, // F34
    {TERMCAP2_ಕೀಲಿ('F', 'P'), "\033[56;*~"}, // F35
    {TERMCAP2_ಕೀಲಿ('F', 'Q'), "\033[57;*~"}, // F36
    {TERMCAP2_ಕೀಲಿ('F', 'R'), "\033[58;*~"}, // F37

    {(ಪೂರ್ಣಾಙ್ಕ)KS_NAME,	ರಿಕ್ತ}  // end marker
};

/*
 * Additions for using modifyOtherKeys level 2.  Same as what is used for
 * xterm.
 */
ಸ್ಥಿರ tcap_entry_T builtin_mok2[] = {
    // t_TI enables modifyOtherKeys level 2
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CTI,	"\033[>4;2m"},

    // XTQMODKEYS was added in xterm version 377: "CSI ? 4 m" which should
    // return "{lead} > 4 ; Pv m".  Before version 377 we expect it to have no
    // effect.
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CRK,	"\033[?4m"},

    // t_TE disables modifyOtherKeys
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CTE,	"\033[>4;m"},

    {(ಪೂರ್ಣಾಙ್ಕ)KS_NAME,	ರಿಕ್ತ}  // end marker
};

/*
 * Additions for using the Kitty keyboard protocol.
 */
ಸ್ಥಿರ tcap_entry_T builtin_kitty[] = {
    // t_TI enables the kitty keyboard protocol.
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CTI,	"\033[=1;1u"},

    // t_RK requests the kitty keyboard protocol state
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CRK,	"\033[?u"},

    // t_TE also disables modifyOtherKeys, because t_TI from xterm may already
    // have been used.
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CTE,	"\033[>4;m\033[=0;1u"},

    // t_RF request terminal foreground color
    {(ಪೂರ್ಣಾಙ್ಕ)KS_RFG,	"\033]10;?\033\\"},

    // t_RB request terminal background color
    {(ಪೂರ್ಣಾಙ್ಕ)KS_RBG,	"\033]11;?\033\\"},

    {(ಪೂರ್ಣಾಙ್ಕ)KS_NAME,	ರಿಕ್ತ}  // end marker
};

#ifdef HAVE_TGETENT
/*
 * Additions for enabling/disabling synchronized output mode for terminal.
 */
ಸ್ಥಿರ tcap_entry_T builtin_sync_output[] = {
    {(ಪೂರ್ಣಾಙ್ಕ)KS_BSU,	"\033[?2026h"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_ESU,	"\033[?2026l"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_NAME,	ರಿಕ್ತ}  // end marker
};
#endif

#ifdef FEAT_TERMRESPONSE
/*
 * List of DECRQM modes that Vim supports
 */
ಸ್ಥಿರ ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ dec_modes[] = {
    2026,   // Synchronized output
# ifdef UNIX
    2048    // In-band terminal resize events
# endif
};
#endif

#ifdef FEAT_TERMGUICOLORS
/*
 * Additions for using the RGB colors and terminal font
 */
ಸ್ಥಿರ tcap_entry_T builtin_rgb[] = {
    // These are printf strings, not terminal codes.
    {(ಪೂರ್ಣಾಙ್ಕ)KS_8F,	"\033[38;2;%lu;%lu;%lum"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_8B,	"\033[48;2;%lu;%lu;%lum"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_8U,	"\033[58;2;%lu;%lu;%lum"},

    {(ಪೂರ್ಣಾಙ್ಕ)KS_NAME,	ರಿಕ್ತ}  // end marker
};
#endif

#ifdef HAVE_TGETENT
ಸ್ಥಿರ tcap_entry_T special_term[] = {
    // These are printf strings, not terminal codes.
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CF,	"\033[%dm"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_NAME,	ರಿಕ್ತ}  // end marker
};
#endif

/*
 * iris-ansi for Silicon Graphics machines.
 */
ಸ್ಥಿರ tcap_entry_T builtin_iris_ansi[] = {
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CE,	"\033[K"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CD,	"\033[J"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_AL,	"\033[L"},
#ifdef TERMINFO
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CAL,	"\033[%p1%dL"},
#ಅನ್ಯಥಾ
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CAL,	"\033[%dL"},
#endif
    {(ಪೂರ್ಣಾಙ್ಕ)KS_DL,	"\033[M"},
#ifdef TERMINFO
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CDL,	"\033[%p1%dM"},
#ಅನ್ಯಥಾ
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CDL,	"\033[%dM"},
#endif
#ಯದಿ 0	// The scroll region is not working as Vim expects.
# ifdef TERMINFO
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CS,	"\033[%i%p1%d;%p2%dr"},
# ಅನ್ಯಥಾ
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CS,	"\033[%i%d;%dr"},
# endif
#endif
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CL,	"\033[H\033[2J"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_VE,	"\033[9/y\033[12/y"},	// These aren't documented
    {(ಪೂರ್ಣಾಙ್ಕ)KS_VS,	"\033[10/y\033[=1h\033[=2l"}, // These aren't documented
    {(ಪೂರ್ಣಾಙ್ಕ)KS_TI,	"\033[=6h"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_TE,	"\033[=6l"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_SE,	"\033[21;27m"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_SO,	"\033[1;7m"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_ME,	"\033[m"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_MR,	"\033[7m"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_MD,	"\033[1m"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CCO,	"8"},			// allow 8 colors
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CZH,	"\033[3m"},		// italic mode on
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CZR,	"\033[23m"},		// italic mode off
    {(ಪೂರ್ಣಾಙ್ಕ)KS_US,	"\033[4m"},		// underline on
    {(ಪೂರ್ಣಾಙ್ಕ)KS_UE,	"\033[24m"},		// underline off
#ifdef TERMINFO
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CAB,	"\033[4%p1%dm"},    // set background color (ANSI)
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CAF,	"\033[3%p1%dm"},    // set foreground color (ANSI)
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CSB,	"\033[102;%p1%dm"}, // set screen background color
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CSF,	"\033[101;%p1%dm"}, // set screen foreground color
#ಅನ್ಯಥಾ
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CAB,	"\033[4%dm"},	    // set background color (ANSI)
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CAF,	"\033[3%dm"},	    // set foreground color (ANSI)
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CSB,	"\033[102;%dm"},    // set screen background color
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CSF,	"\033[101;%dm"},    // set screen foreground color
#endif
    {(ಪೂರ್ಣಾಙ್ಕ)KS_MS,	"y"},		// guessed
    {(ಪೂರ್ಣಾಙ್ಕ)KS_UT,	"y"},		// guessed
    {(ಪೂರ್ಣಾಙ್ಕ)KS_LE,	"\b"},
#ifdef TERMINFO
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CM,	"\033[%i%p1%d;%p2%dH"},
#ಅನ್ಯಥಾ
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CM,	"\033[%i%d;%dH"},
#endif
    {(ಪೂರ್ಣಾಙ್ಕ)KS_SR,	"\033M"},
#ifdef TERMINFO
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CRI,	"\033[%p1%dC"},
#ಅನ್ಯಥಾ
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CRI,	"\033[%dC"},
#endif
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CIS,	"\033P3.y"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CIE,	"\234"},    // ST "String Terminator"
    {(ಪೂರ್ಣಾಙ್ಕ)KS_TS,	"\033P1.y"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_FS,	"\234"},    // ST "String Terminator"
#ifdef TERMINFO
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CWS,	"\033[203;%p1%d;%p2%d/y"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CWP,	"\033[205;%p1%d;%p2%d/y"},
#ಅನ್ಯಥಾ
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CWS,	"\033[203;%d;%d/y"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CWP,	"\033[205;%d;%d/y"},
#endif
    {K_UP,		"\033[A"},
    {K_DOWN,		"\033[B"},
    {K_ಎಡ,		"\033[D"},
    {K_ಬಲ,		"\033[C"},
    {K_S_UP,		"\033[161q"},
    {K_S_DOWN,		"\033[164q"},
    {K_S_ಎಡ,		"\033[158q"},
    {K_S_ಬಲ,		"\033[167q"},
    {K_F1,		"\033[001q"},
    {K_F2,		"\033[002q"},
    {K_F3,		"\033[003q"},
    {K_F4,		"\033[004q"},
    {K_F5,		"\033[005q"},
    {K_F6,		"\033[006q"},
    {K_F7,		"\033[007q"},
    {K_F8,		"\033[008q"},
    {K_F9,		"\033[009q"},
    {K_F10,		"\033[010q"},
    {K_F11,		"\033[011q"},
    {K_F12,		"\033[012q"},
    {K_S_F1,		"\033[013q"},
    {K_S_F2,		"\033[014q"},
    {K_S_F3,		"\033[015q"},
    {K_S_F4,		"\033[016q"},
    {K_S_F5,		"\033[017q"},
    {K_S_F6,		"\033[018q"},
    {K_S_F7,		"\033[019q"},
    {K_S_F8,		"\033[020q"},
    {K_S_F9,		"\033[021q"},
    {K_S_F10,		"\033[022q"},
    {K_S_F11,		"\033[023q"},
    {K_S_F12,		"\033[024q"},
    {K_INS,		"\033[139q"},
    {K_HOME,		"\033[H"},
    {K_END,		"\033[146q"},
    {K_PAGEUP,		"\033[150q"},
    {K_PAGEDOWN,	"\033[154q"},

    {(ಪೂರ್ಣಾಙ್ಕ)KS_NAME,	ರಿಕ್ತ}  // end marker
};

/*
 * These codes are valid when nansi.sys or equivalent has been installed.
 * Function keys on a PC are preceded with a NUL. These are converted into
 * K_NUL '\316' in mch_inchar(), because we cannot handle NULs in key codes.
 * CTRL-arrow is used instead of SHIFT-arrow.
 */
ಸ್ಥಿರ tcap_entry_T builtin_pcansi[] = {
    {(ಪೂರ್ಣಾಙ್ಕ)KS_DL,	"\033[M"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_AL,	"\033[L"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CE,	"\033[K"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CL,	"\033[2J"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_ME,	"\033[0m"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_MR,	"\033[5m"},	// reverse: black on lightgrey
    {(ಪೂರ್ಣಾಙ್ಕ)KS_MD,	"\033[1m"},	// bold: white text
    {(ಪೂರ್ಣಾಙ್ಕ)KS_SE,	"\033[0m"},	// standout end
    {(ಪೂರ್ಣಾಙ್ಕ)KS_SO,	"\033[31m"},	// standout: white on blue
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CZH,	"\033[34;43m"},	// italic mode: blue text on yellow
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CZR,	"\033[0m"},	// italic mode end
    {(ಪೂರ್ಣಾಙ್ಕ)KS_US,	"\033[36;41m"},	// underscore mode: cyan text on red
    {(ಪೂರ್ಣಾಙ್ಕ)KS_UE,	"\033[0m"},	// underscore mode end
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CCO,	"8"},		// allow 8 colors
#ifdef TERMINFO
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CAB,	"\033[4%p1%dm"},// set background color
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CAF,	"\033[3%p1%dm"},// set foreground color
#ಅನ್ಯಥಾ
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CAB,	"\033[4%dm"},	// set background color
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CAF,	"\033[3%dm"},	// set foreground color
#endif
    {(ಪೂರ್ಣಾಙ್ಕ)KS_OP,	"\033[0m"},	// reset colors
    {(ಪೂರ್ಣಾಙ್ಕ)KS_MS,	"y"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_UT,	"y"},		// guessed
    {(ಪೂರ್ಣಾಙ್ಕ)KS_LE,	"\b"},
#ifdef TERMINFO
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CM,	"\033[%i%p1%d;%p2%dH"},
#ಅನ್ಯಥಾ
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CM,	"\033[%i%d;%dH"},
#endif
#ifdef TERMINFO
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CRI,	"\033[%p1%dC"},
#ಅನ್ಯಥಾ
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CRI,	"\033[%dC"},
#endif
    {K_UP,		"\316H"},
    {K_DOWN,		"\316P"},
    {K_ಎಡ,		"\316K"},
    {K_ಬಲ,		"\316M"},
    {K_S_ಎಡ,		"\316s"},
    {K_S_ಬಲ,		"\316t"},
    {K_F1,		"\316;"},
    {K_F2,		"\316<"},
    {K_F3,		"\316="},
    {K_F4,		"\316>"},
    {K_F5,		"\316?"},
    {K_F6,		"\316@"},
    {K_F7,		"\316A"},
    {K_F8,		"\316B"},
    {K_F9,		"\316C"},
    {K_F10,		"\316D"},
    {K_F11,		"\316\205"},	// guessed
    {K_F12,		"\316\206"},	// guessed
    {K_S_F1,		"\316T"},
    {K_S_F2,		"\316U"},
    {K_S_F3,		"\316V"},
    {K_S_F4,		"\316W"},
    {K_S_F5,		"\316X"},
    {K_S_F6,		"\316Y"},
    {K_S_F7,		"\316Z"},
    {K_S_F8,		"\316["},
    {K_S_F9,		"\316\\"},
    {K_S_F10,		"\316]"},
    {K_S_F11,		"\316\207"},	// guessed
    {K_S_F12,		"\316\210"},	// guessed
    {K_INS,		"\316R"},
    {K_DEL,		"\316S"},
    {K_HOME,		"\316G"},
    {K_END,		"\316O"},
    {K_PAGEDOWN,	"\316Q"},
    {K_PAGEUP,		"\316I"},

    {(ಪೂರ್ಣಾಙ್ಕ)KS_NAME,	ರಿಕ್ತ}  // end marker
};

/*
 * These codes are valid for the Win32 Console.  The entries that start with
 * ESC | are translated into console calls in os_win32.c.  The function keys
 * are also translated in os_win32.c.
 */
ಸ್ಥಿರ tcap_entry_T builtin_win32[] = {
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CE,	"\033|K"},	// clear to end of line
    {(ಪೂರ್ಣಾಙ್ಕ)KS_AL,	"\033|L"},	// add new blank line
#ifdef TERMINFO
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CAL,	"\033|%p1%dL"},	// add number of new blank lines
#ಅನ್ಯಥಾ
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CAL,	"\033|%dL"},	// add number of new blank lines
#endif
    {(ಪೂರ್ಣಾಙ್ಕ)KS_DL,	"\033|M"},	// delete line
#ifdef TERMINFO
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CDL,	"\033|%p1%dM"},	// delete number of lines
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CSV,	"\033|%p1%d;%p2%dV"},
#ಅನ್ಯಥಾ
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CDL,	"\033|%dM"},	// delete number of lines
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CSV,	"\033|%d;%dV"},
#endif
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CL,	"\033|J"},	// clear screen
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CD,	"\033|j"},	// clear to end of display
    {(ಪೂರ್ಣಾಙ್ಕ)KS_VI,	"\033|v"},	// cursor invisible
    {(ಪೂರ್ಣಾಙ್ಕ)KS_VE,	"\033|V"},	// cursor visible

    {(ಪೂರ್ಣಾಙ್ಕ)KS_ME,	"\033|0m"},	// normal
    {(ಪೂರ್ಣಾಙ್ಕ)KS_MR,	"\033|112m"},	// reverse: black on lightgray
    {(ಪೂರ್ಣಾಙ್ಕ)KS_MD,	"\033|15m"},	// bold: white on black
#ಯದಿ 1
    {(ಪೂರ್ಣಾಙ್ಕ)KS_SO,	"\033|31m"},	// standout: white on blue
    {(ಪೂರ್ಣಾಙ್ಕ)KS_SE,	"\033|0m"},	// standout end
#ಅನ್ಯಥಾ
    {(ಪೂರ್ಣಾಙ್ಕ)KS_SO,	"\033|F"},	// standout: high intensity
    {(ಪೂರ್ಣಾಙ್ಕ)KS_SE,	"\033|f"},	// standout end
#endif
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CZH,	"\033|225m"},	// italic: blue text on yellow
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CZR,	"\033|0m"},	// italic end
    {(ಪೂರ್ಣಾಙ್ಕ)KS_US,	"\033|67m"},	// underscore: cyan text on red
    {(ಪೂರ್ಣಾಙ್ಕ)KS_UE,	"\033|0m"},	// underscore end
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CCO,	"16"},		// allow 16 colors
#ifdef TERMINFO
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CAB,	"\033|%p1%db"},	// set background color
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CAF,	"\033|%p1%df"},	// set foreground color
#ಅನ್ಯಥಾ
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CAB,	"\033|%db"},	// set background color
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CAF,	"\033|%df"},	// set foreground color
#endif

    {(ಪೂರ್ಣಾಙ್ಕ)KS_MS,	"y"},		// save to move cur in reverse mode
    {(ಪೂರ್ಣಾಙ್ಕ)KS_UT,	"y"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_XN,	"y"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_LE,	"\b"},
#ifdef TERMINFO
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CM,	"\033|%i%p1%d;%p2%dH"}, // cursor motion
#ಅನ್ಯಥಾ
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CM,	"\033|%i%d;%dH"}, // cursor motion
#endif
    {(ಪೂರ್ಣಾಙ್ಕ)KS_VB,	"\033|B"},	// visual bell
    {(ಪೂರ್ಣಾಙ್ಕ)KS_TI,	"\033|S"},	// put terminal in termcap mode
    {(ಪೂರ್ಣಾಙ್ಕ)KS_TE,	"\033|E"},	// out of termcap mode
#ifdef TERMINFO
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CS,	"\033|%i%p1%d;%p2%dr"}, // scroll region
#ಅನ್ಯಥಾ
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CS,	"\033|%i%d;%dr"}, // scroll region
#endif

    {K_UP,		"\316H"},
    {K_DOWN,		"\316P"},
    {K_ಎಡ,		"\316K"},
    {K_ಬಲ,		"\316M"},
    {K_S_UP,		"\316\304"},
    {K_S_DOWN,		"\316\317"},
    {K_S_ಎಡ,		"\316\311"},
    {K_C_ಎಡ,		"\316s"},
    {K_S_ಬಲ,		"\316\313"},
    {K_C_ಬಲ,		"\316t"},
    {K_S_TAB,		"\316\017"},
    {K_F1,		"\316;"},
    {K_F2,		"\316<"},
    {K_F3,		"\316="},
    {K_F4,		"\316>"},
    {K_F5,		"\316?"},
    {K_F6,		"\316@"},
    {K_F7,		"\316A"},
    {K_F8,		"\316B"},
    {K_F9,		"\316C"},
    {K_F10,		"\316D"},
    {K_F11,		"\316\205"},
    {K_F12,		"\316\206"},
    {K_S_F1,		"\316T"},
    {K_S_F2,		"\316U"},
    {K_S_F3,		"\316V"},
    {K_S_F4,		"\316W"},
    {K_S_F5,		"\316X"},
    {K_S_F6,		"\316Y"},
    {K_S_F7,		"\316Z"},
    {K_S_F8,		"\316["},
    {K_S_F9,		"\316\\"},
    {K_S_F10,		"\316]"},
    {K_S_F11,		"\316\207"},
    {K_S_F12,		"\316\210"},
    {K_INS,		"\316R"},
    {K_DEL,		"\316S"},
    {K_HOME,		"\316G"},
    {K_S_HOME,		"\316\302"},
    {K_C_HOME,		"\316w"},
    {K_END,		"\316O"},
    {K_S_END,		"\316\315"},
    {K_C_END,		"\316u"},
    {K_PAGEDOWN,	"\316Q"},
    {K_PAGEUP,		"\316I"},
    {K_KPLUS,		"\316N"},
    {K_KMINUS,		"\316J"},
    {K_KMULTIPLY,	"\316\067"},
    {K_K0,		"\316\332"},
    {K_K1,		"\316\336"},
    {K_K2,		"\316\342"},
    {K_K3,		"\316\346"},
    {K_K4,		"\316\352"},
    {K_K5,		"\316\356"},
    {K_K6,		"\316\362"},
    {K_K7,		"\316\366"},
    {K_K8,		"\316\372"},
    {K_K9,		"\316\376"},
    {K_BS,		"\316x"},
    {K_S_BS,		"\316y"},

    {(ಪೂರ್ಣಾಙ್ಕ)KS_NAME,	ರಿಕ್ತ}  // end marker
};

#ಯದಿ defined(FEAT_GUI)
/*
 * GUI uses made-up codes, only used inside Vim.
 */
ಸ್ಥಿರ tcap_entry_T builtin_gui[] = {
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CE,	"\033|$"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_AL,	"\033|i"},
# ifdef TERMINFO
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CAL,	"\033|%p1%dI"},
# ಅನ್ಯಥಾ
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CAL,	"\033|%dI"},
# endif
    {(ಪೂರ್ಣಾಙ್ಕ)KS_DL,	"\033|d"},
# ifdef TERMINFO
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CDL,	"\033|%p1%dD"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CS,	"\033|%p1%d;%p2%dR"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CSV,	"\033|%p1%d;%p2%dV"},
# ಅನ್ಯಥಾ
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CDL,	"\033|%dD"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CS,	"\033|%d;%dR"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CSV,	"\033|%d;%dV"},
# endif
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CL,	"\033|C"},
			// attributes switched on with 'h', off with * 'H'
    {(ಪೂರ್ಣಾಙ್ಕ)KS_ME,	"\033|31H"}, // HL_ALL
    {(ಪೂರ್ಣಾಙ್ಕ)KS_MR,	"\033|1h"},   // HL_INVERSE
    {(ಪೂರ್ಣಾಙ್ಕ)KS_MD,	"\033|2h"},   // HL_BOLD
    {(ಪೂರ್ಣಾಙ್ಕ)KS_SE,	"\033|16H"}, // HL_STANDOUT
    {(ಪೂರ್ಣಾಙ್ಕ)KS_SO,	"\033|16h"}, // HL_STANDOUT
    {(ಪೂರ್ಣಾಙ್ಕ)KS_UE,	"\033|8H"},   // HL_UNDERLINE
    {(ಪೂರ್ಣಾಙ್ಕ)KS_US,	"\033|8h"},   // HL_UNDERLINE
    {(ಪೂರ್ಣಾಙ್ಕ)KS_UCE,	"\033|8C"},   // HL_UNDERCURL
    {(ಪೂರ್ಣಾಙ್ಕ)KS_UCS,	"\033|8c"},   // HL_UNDERCURL
    {(ಪೂರ್ಣಾಙ್ಕ)KS_STE,	"\033|4C"},   // HL_STRIKETHROUGH
    {(ಪೂರ್ಣಾಙ್ಕ)KS_STS,	"\033|4c"},   // HL_STRIKETHROUGH
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CZR,	"\033|4H"},   // HL_ITALIC
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CZH,	"\033|4h"},   // HL_ITALIC
    {(ಪೂರ್ಣಾಙ್ಕ)KS_VB,	"\033|f"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_MS,	"y"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_UT,	"y"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_XN,	"y"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_LE,	"\b"},		// cursor-left = BS
    {(ಪೂರ್ಣಾಙ್ಕ)KS_ND,	"\014"},	// cursor-right = CTRL-L
# ifdef TERMINFO
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CM,	"\033|%p1%d;%p2%dM"},
# ಅನ್ಯಥಾ
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CM,	"\033|%d;%dM"},
# endif
	// there are no key sequences here, the GUI sequences are recognized
	// in check_termcode()

    {(ಪೂರ್ಣಾಙ್ಕ)KS_NAME,	ರಿಕ್ತ}  // end marker
};
#endif

/*
 * Amiga console window, default for Amiga.
 */
ಸ್ಥಿರ tcap_entry_T builtin_amiga[] = {
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CE,	"\033[K"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CD,	"\033[J"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_AL,	"\033[L"},
#ifdef TERMINFO
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CAL,	"\033[%p1%dL"},
#ಅನ್ಯಥಾ
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CAL,	"\033[%dL"},
#endif
    {(ಪೂರ್ಣಾಙ್ಕ)KS_DL,	"\033[M"},
#ifdef TERMINFO
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CDL,	"\033[%p1%dM"},
#ಅನ್ಯಥಾ
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CDL,	"\033[%dM"},
#endif
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CL,	"\014"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_VI,	"\033[0 p"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_VE,	"\033[1 p"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_ME,	"\033[0m"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_MR,	"\033[7m"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_MD,	"\033[1m"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_SE,	"\033[0m"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_SO,	"\033[33m"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_US,	"\033[4m"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_UE,	"\033[0m"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CZH,	"\033[3m"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CZR,	"\033[0m"},
#ಯದಿ defined(__amigaos4__) || defined(__MORPHOS__) || defined(__AROS__)
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CCO,	"8"},		// allow 8 colors
# ifdef TERMINFO
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CAB,	"\033[4%p1%dm"},// set background color
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CAF,	"\033[3%p1%dm"},// set foreground color
# ಅನ್ಯಥಾ
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CAB,	"\033[4%dm"},	// set background color
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CAF,	"\033[3%dm"},	// set foreground color
# endif
    {(ಪೂರ್ಣಾಙ್ಕ)KS_OP,	"\033[m"},	// reset colors
#endif
    {(ಪೂರ್ಣಾಙ್ಕ)KS_MS,	"y"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_UT,	"y"},		// guessed
    {(ಪೂರ್ಣಾಙ್ಕ)KS_LE,	"\b"},
#ifdef TERMINFO
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CM,	"\033[%i%p1%d;%p2%dH"},
#ಅನ್ಯಥಾ
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CM,	"\033[%i%d;%dH"},
#endif
#ಯದಿ defined(__MORPHOS__)
    {(ಪೂರ್ಣಾಙ್ಕ)KS_SR,	"\033M"},
#endif
#ifdef TERMINFO
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CRI,	"\033[%p1%dC"},
#ಅನ್ಯಥಾ
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CRI,	"\033[%dC"},
#endif
    {K_UP,		"\233A"},
    {K_DOWN,		"\233B"},
    {K_ಎಡ,		"\233D"},
    {K_ಬಲ,		"\233C"},
    {K_S_UP,		"\233T"},
    {K_S_DOWN,		"\233S"},
    {K_S_ಎಡ,		"\233 A"},
    {K_S_ಬಲ,		"\233 @"},
    {K_S_TAB,		"\233Z"},
    {K_F1,		"\233\060~"},// some compilers don't dig "\2330"
    {K_F2,		"\233\061~"},
    {K_F3,		"\233\062~"},
    {K_F4,		"\233\063~"},
    {K_F5,		"\233\064~"},
    {K_F6,		"\233\065~"},
    {K_F7,		"\233\066~"},
    {K_F8,		"\233\067~"},
    {K_F9,		"\233\070~"},
    {K_F10,		"\233\071~"},
    {K_S_F1,		"\233\061\060~"},
    {K_S_F2,		"\233\061\061~"},
    {K_S_F3,		"\233\061\062~"},
    {K_S_F4,		"\233\061\063~"},
    {K_S_F5,		"\233\061\064~"},
    {K_S_F6,		"\233\061\065~"},
    {K_S_F7,		"\233\061\066~"},
    {K_S_F8,		"\233\061\067~"},
    {K_S_F9,		"\233\061\070~"},
    {K_S_F10,		"\233\061\071~"},
    {K_HELP,		"\233?~"},
    {K_INS,		"\233\064\060~"},	// 101 key keyboard
    {K_PAGEUP,		"\233\064\061~"},	// 101 key keyboard
    {K_PAGEDOWN,	"\233\064\062~"},	// 101 key keyboard
    {K_HOME,		"\233\064\064~"},	// 101 key keyboard
    {K_END,		"\233\064\065~"},	// 101 key keyboard

    {BT_EXTRA_ಕೀಲಿಗಳು,	""},
    {TERMCAP2_ಕೀಲಿ('#', '2'), "\233\065\064~"},	// shifted home key
    {TERMCAP2_ಕೀಲಿ('#', '3'), "\233\065\060~"},	// shifted insert key
    {TERMCAP2_ಕೀಲಿ('*', '7'), "\233\065\065~"},	// shifted end key

    {(ಪೂರ್ಣಾಙ್ಕ)KS_NAME,	ರಿಕ್ತ}  // end marker
};

/*
 * The most minimal terminal: only clear screen and cursor positioning.
 */
ಸ್ಥಿರ tcap_entry_T builtin_dumb[] = {
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CL,	"\014"},
#ifdef TERMINFO
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CM,	"\033[%i%p1%d;%p2%dH"},
#ಅನ್ಯಥಾ
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CM,	"\033[%i%d;%dH"},
#endif

    {(ಪೂರ್ಣಾಙ್ಕ)KS_NAME,	ರಿಕ್ತ}  // end marker
};

/*
 * Terminal used for debugging.
 */
ಸ್ಥಿರ tcap_entry_T builtin_debug[] = {
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CE,	"[CE]"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CD,	"[CD]"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_AL,	"[AL]"},
#ifdef TERMINFO
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CAL,	"[CAL%p1%d]"},
#ಅನ್ಯಥಾ
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CAL,	"[CAL%d]"},
#endif
    {(ಪೂರ್ಣಾಙ್ಕ)KS_DL,	"[DL]"},
#ifdef TERMINFO
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CDL,	"[CDL%p1%d]"},
#ಅನ್ಯಥಾ
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CDL,	"[CDL%d]"},
#endif
#ifdef TERMINFO
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CS,	"[%p1%dCS%p2%d]"},
#ಅನ್ಯಥಾ
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CS,	"[%dCS%d]"},
#endif
#ifdef TERMINFO
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CSV,	"[%p1%dCSV%p2%d]"},
#ಅನ್ಯಥಾ
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CSV,	"[%dCSV%d]"},
#endif
#ifdef TERMINFO
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CAB,	"[CAB%p1%d]"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CAF,	"[CAF%p1%d]"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CSB,	"[CSB%p1%d]"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CSF,	"[CSF%p1%d]"},
#ಅನ್ಯಥಾ
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CAB,	"[CAB%d]"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CAF,	"[CAF%d]"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CSB,	"[CSB%d]"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CSF,	"[CSF%d]"},
#endif
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CAU,	"[CAU%d]"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_OP,	"[OP]"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_LE,	"[LE]"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CL,	"[CL]"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_VI,	"[VI]"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_VE,	"[VE]"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_VS,	"[VS]"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_ME,	"[ME]"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_MR,	"[MR]"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_MB,	"[MB]"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_MD,	"[MD]"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_SE,	"[SE]"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_SO,	"[SO]"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_UE,	"[UE]"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_US,	"[US]"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_UCE,	"[UCE]"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_UCS,	"[UCS]"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_USS,	"[USS]"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_DS,	"[DS]"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CDS,	"[CDS]"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_STE,	"[STE]"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_STS,	"[STS]"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_MS,	"[MS]"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_UT,	"[UT]"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_XN,	"[XN]"},
#ifdef TERMINFO
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CM,	"[%p1%dCM%p2%d]"},
#ಅನ್ಯಥಾ
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CM,	"[%dCM%d]"},
#endif
    {(ಪೂರ್ಣಾಙ್ಕ)KS_SR,	"[SR]"},
#ifdef TERMINFO
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CRI,	"[CRI%p1%d]"},
#ಅನ್ಯಥಾ
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CRI,	"[CRI%d]"},
#endif
    {(ಪೂರ್ಣಾಙ್ಕ)KS_VB,	"[VB]"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_KS,	"[KS]"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_KE,	"[KE]"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_TI,	"[TI]"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_TE,	"[TE]"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CIS,	"[CIS]"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CIE,	"[CIE]"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CSC,	"[CSC]"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CEC,	"[CEC]"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_TS,	"[TS]"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_FS,	"[FS]"},
#ifdef TERMINFO
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CWS,	"[%p1%dCWS%p2%d]"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CWP,	"[%p1%dCWP%p2%d]"},
#ಅನ್ಯಥಾ
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CWS,	"[%dCWS%d]"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CWP,	"[%dCWP%d]"},
#endif
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CRV,	"[CRV]"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CXM,	"[CXM]"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_U7,	"[U7]"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_RFG,	"[RFG]"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_RBG,	"[RBG]"},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CF,	"[CF%d]"},
    {K_UP,		"[KU]"},
    {K_DOWN,		"[KD]"},
    {K_ಎಡ,		"[KL]"},
    {K_ಬಲ,		"[KR]"},
    {K_XUP,		"[xKU]"},
    {K_XDOWN,		"[xKD]"},
    {K_XLEFT,		"[xKL]"},
    {K_XRIGHT,		"[xKR]"},
    {K_S_UP,		"[S-KU]"},
    {K_S_DOWN,		"[S-KD]"},
    {K_S_ಎಡ,		"[S-KL]"},
    {K_C_ಎಡ,		"[C-KL]"},
    {K_S_ಬಲ,		"[S-KR]"},
    {K_C_ಬಲ,		"[C-KR]"},
    {K_F1,		"[F1]"},
    {K_XF1,		"[xF1]"},
    {K_F2,		"[F2]"},
    {K_XF2,		"[xF2]"},
    {K_F3,		"[F3]"},
    {K_XF3,		"[xF3]"},
    {K_F4,		"[F4]"},
    {K_XF4,		"[xF4]"},
    {K_F5,		"[F5]"},
    {K_F6,		"[F6]"},
    {K_F7,		"[F7]"},
    {K_F8,		"[F8]"},
    {K_F9,		"[F9]"},
    {K_F10,		"[F10]"},
    {K_F11,		"[F11]"},
    {K_F12,		"[F12]"},
    {K_S_F1,		"[S-F1]"},
    {K_S_XF1,		"[S-xF1]"},
    {K_S_F2,		"[S-F2]"},
    {K_S_XF2,		"[S-xF2]"},
    {K_S_F3,		"[S-F3]"},
    {K_S_XF3,		"[S-xF3]"},
    {K_S_F4,		"[S-F4]"},
    {K_S_XF4,		"[S-xF4]"},
    {K_S_F5,		"[S-F5]"},
    {K_S_F6,		"[S-F6]"},
    {K_S_F7,		"[S-F7]"},
    {K_S_F8,		"[S-F8]"},
    {K_S_F9,		"[S-F9]"},
    {K_S_F10,		"[S-F10]"},
    {K_S_F11,		"[S-F11]"},
    {K_S_F12,		"[S-F12]"},
    {K_HELP,		"[HELP]"},
    {K_UNDO,		"[UNDO]"},
    {K_BS,		"[BS]"},
    {K_INS,		"[INS]"},
    {K_KINS,		"[KINS]"},
    {K_DEL,		"[DEL]"},
    {K_KDEL,		"[KDEL]"},
    {K_HOME,		"[HOME]"},
    {K_S_HOME,		"[C-HOME]"},
    {K_C_HOME,		"[C-HOME]"},
    {K_KHOME,		"[KHOME]"},
    {K_XHOME,		"[XHOME]"},
    {K_ZHOME,		"[ZHOME]"},
    {K_END,		"[END]"},
    {K_S_END,		"[C-END]"},
    {K_C_END,		"[C-END]"},
    {K_KEND,		"[KEND]"},
    {K_XEND,		"[XEND]"},
    {K_ZEND,		"[ZEND]"},
    {K_PAGEUP,		"[PAGEUP]"},
    {K_PAGEDOWN,	"[PAGEDOWN]"},
    {K_KPAGEUP,		"[KPAGEUP]"},
    {K_KPAGEDOWN,	"[KPAGEDOWN]"},
    {K_MOUSE,		"[MOUSE]"},
    {K_KPLUS,		"[KPLUS]"},
    {K_KMINUS,		"[KMINUS]"},
    {K_KDIVIDE,		"[KDIVIDE]"},
    {K_KMULTIPLY,	"[KMULTIPLY]"},
    {K_KENTER,		"[KENTER]"},
    {K_KPOINT,		"[KPOINT]"},
    {K_PS,		"[PASTE-START]"},
    {K_PE,		"[PASTE-END]"},
    {K_K0,		"[K0]"},
    {K_K1,		"[K1]"},
    {K_K2,		"[K2]"},
    {K_K3,		"[K3]"},
    {K_K4,		"[K4]"},
    {K_K5,		"[K5]"},
    {K_K6,		"[K6]"},
    {K_K7,		"[K7]"},
    {K_K8,		"[K8]"},
    {K_K9,		"[K9]"},

    {(ಪೂರ್ಣಾಙ್ಕ)KS_NAME,	ರಿಕ್ತ}  // end marker
};

/*
 * List of builtin terminals.
 */
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
    ವರ್ಣ	    *bitc_name;	    // name, such as "xterm"
    tcap_entry_T    *bitc_table;    // table with entries for bitc_name
} builtin_tcap_T;

builtin_tcap_T builtin_terminals[] = {
    // Unix and Generic
    {"ansi",	    builtin_ansi},
    {"vt320",	    builtin_vt320},
    {"vt52",	    builtin_vt52},
    {"xterm",	    builtin_xterm},
    {"iris-ansi",   builtin_iris_ansi},

    // MS-Windows
    {"pcansi",	    builtin_pcansi},
    {"win32",	    builtin_win32},

    // Other systems
#ಯದಿ defined(FEAT_GUI)
    {"gui",	    builtin_gui},
#endif
    {"amiga",	    builtin_amiga},
    {"dumb",	    builtin_dumb},
    {"debug",	    builtin_debug},

    {ರಿಕ್ತ,	    ರಿಕ್ತ},  // end marker
};

#ಯದಿ defined(FEAT_TERMGUICOLORS)
    ಸ್ಥಿರ guicolor_T
termgui_mch_get_color(char_u *name)
{
    ಪ್ರತ್ಯಾಗಮ gui_get_color_cmn(name);
}

    guicolor_T
termgui_get_color(char_u *name)
{
    guicolor_T	t;

    ಯದಿ (*name == NUL)
	ಪ್ರತ್ಯಾಗಮ INVALCOLOR;
    t = termgui_mch_get_color(name);

    ಯದಿ (t == INVALCOLOR)
	semsg(_(e_cannot_allocate_color_str), name);
    ಪ್ರತ್ಯಾಗಮ t;
}

    guicolor_T
termgui_mch_get_rgb(guicolor_T color)
{
    ಪ್ರತ್ಯಾಗಮ color;
}
#endif

/*
 * DEFAULT_TERM is used, when no terminal is specified with -T option or $TERM.
 */
#ifdef AMIGA
# define DEFAULT_TERM	(char_u *)"amiga"
#endif

#ifdef MSWIN
# define DEFAULT_TERM	(char_u *)"win32"
#endif

#ಯದಿ defined(UNIX)
# define DEFAULT_TERM	(char_u *)"ansi"
#endif

#ifdef VMS
# define DEFAULT_TERM	(char_u *)"vt320"
#endif

#ifdef __HAIKU__
# undef DEFAULT_TERM
# define DEFAULT_TERM	(char_u *)"xterm"
#endif

#ifndef DEFAULT_TERM
# define DEFAULT_TERM	(char_u *)"dumb"
#endif

/*
 * Term_strings contains currently used terminal output strings.
 * It is initialized with the default values by parse_builtin_tcap().
 * The values can be changed by setting the option with the same name.
 */
char_u *(term_strings[(ಪೂರ್ಣಾಙ್ಕ)KS_LAST + 1]);

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	need_gather = FALSE;	    // need to fill termleader[]
ಸ್ಥಿರ char_u	termleader[256 + 1];	    // for check_termcode()
#ifdef FEAT_TERMRESPONSE
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	check_for_codes = FALSE;    // check for key code response
#endif

/*
 * Structure and table to store terminal features that can be detected by
 * querying the terminal.  Either by inspecting the termresponse or a more
 * specific request.  Besides this there are:
 * t_colors - number of colors supported
 */
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
    ವರ್ಣ    *tpr_name;
    ಪೂರ್ಣಾಙ್ಕ	    tpr_set_by_termresponse;
    ಪೂರ್ಣಾಙ್ಕ	    tpr_status;
} termprop_T;

// Values for tpr_status.
#define TPR_UNKNOWN	    'u'
#define TPR_YES		    'y'
#define TPR_NO		    'n'
#define TPR_MOUSE_XTERM     'x'	// use "xterm" for 'ttymouse'
#define TPR_MOUSE_XTERM2    '2'	// use "xterm2" for 'ttymouse'
#define TPR_MOUSE_SGR	    's'	// use "sgr" for 'ttymouse'

// can request the cursor style without messing up the display
#define TPR_CURSOR_STYLE	    0
// can request the cursor blink mode without messing up the display
#define TPR_CURSOR_BLINK	    1
// can set the underline color with t_8u without resetting other colors
#define TPR_UNDERLINE_RGB	    2
// mouse support - TPR_MOUSE_XTERM, TPR_MOUSE_XTERM2 or TPR_MOUSE_SGR
#define TPR_MOUSE		    3
// term response indicates kitty
#define TPR_KITTY		    4
// can send DECRQM requests to terminal
#define TPR_DECRQM		    5
// true color, from the color count of the terminal entry
#define TPR_RGB			    6
// table size
#define TPR_ಎಣಿಕೆ		    7

ಸ್ಥಿರ termprop_T term_props[TPR_ಎಣಿಕೆ];

/*
 * Set the "rgb" terminal property from the color count.  The entry of a
 * direct color terminal, such as xterm-direct, has 16777216 colors.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
set_rgb_term_prop(ಶೂನ್ಯ)
{
    term_props[TPR_RGB].tpr_status = t_colors == 0x1000000
						       ? TPR_YES : TPR_UNKNOWN;
}

/*
 * Initialize the term_props table.
 * When "all" is FALSE only set those that are detected from the version
 * response.
 */
    ಶೂನ್ಯ
init_term_props(ಪೂರ್ಣಾಙ್ಕ all)
{
    ಪೂರ್ಣಾಙ್ಕ i;

    term_props[TPR_CURSOR_STYLE].tpr_name = "cursor_style";
    term_props[TPR_CURSOR_STYLE].tpr_set_by_termresponse = FALSE;
    term_props[TPR_CURSOR_BLINK].tpr_name = "cursor_blink_mode";
    term_props[TPR_CURSOR_BLINK].tpr_set_by_termresponse = FALSE;
    term_props[TPR_UNDERLINE_RGB].tpr_name = "underline_rgb";
    term_props[TPR_UNDERLINE_RGB].tpr_set_by_termresponse = TRUE;
    term_props[TPR_MOUSE].tpr_name = "mouse";
    term_props[TPR_MOUSE].tpr_set_by_termresponse = TRUE;
    term_props[TPR_KITTY].tpr_name = "kitty";
    term_props[TPR_KITTY].tpr_set_by_termresponse = FALSE;
    term_props[TPR_DECRQM].tpr_name = "decrqm";
    term_props[TPR_DECRQM].tpr_set_by_termresponse = TRUE;
    term_props[TPR_RGB].tpr_name = "rgb";
    term_props[TPR_RGB].tpr_set_by_termresponse = FALSE;

    ಪ್ರತಿ (i = 0; i < TPR_ಎಣಿಕೆ; ++i)
	ಯದಿ (all || term_props[i].tpr_set_by_termresponse)
	    term_props[i].tpr_status = TPR_UNKNOWN;

    // Derived from the color count, not reset.
    set_rgb_term_prop();
}

#ಯದಿ defined(FEAT_EVAL)
    ಶೂನ್ಯ
f_terminalprops(typval_T *argvars UNUSED, typval_T *rettv)
{
# ifdef FEAT_TERMRESPONSE
    ಪೂರ್ಣಾಙ್ಕ i;
# endif

    ಯದಿ (rettv_dict_alloc(rettv) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;
# ifdef FEAT_TERMRESPONSE
    ಪ್ರತಿ (i = 0; i < TPR_ಎಣಿಕೆ; ++i)
    {
	char_u	ಮೌಲ್ಯ[2];

	ಮೌಲ್ಯ[0] = term_props[i].tpr_status;
	ಮೌಲ್ಯ[1] = NUL;
	dict_add_string_len(rettv->vval.v_dict, term_props[i].tpr_name,
	    ಮೌಲ್ಯ, (ಮೌಲ್ಯ[0] == NUL) ? 0 : 1);
    }
# endif
}
#endif

/*
 * Find the builtin termcap entries for "term".
 * This also recognizes similar names.  E.g. "xterm-256color" finds the "xterm"
 * entry.
 * Returns NULL when "term" is not found.
 */
    ಸ್ಥಿರ tcap_entry_T *
find_builtin_term(char_u *term)
{
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; ; ++i)
    {
	char_u *name = (char_u *)builtin_terminals[i].bitc_name;
	ಯದಿ (name == ರಿಕ್ತ)  // end marker
	    ಭಙ್ಗ;
#ifdef UNIX
	ಯದಿ (STRCMP(name, "iris-ansi") == 0 && vim_is_iris(term))
	    ಪ್ರತ್ಯಾಗಮ builtin_terminals[i].bitc_table;
	ಯದಿ (STRCMP(name, "xterm") == 0 && vim_is_xterm(term))
	    ಪ್ರತ್ಯಾಗಮ builtin_terminals[i].bitc_table;
#endif
#ifdef VMS
	ಯದಿ (STRCMP(name, "vt320") == 0 && vim_is_vt300(term))
	    ಪ್ರತ್ಯಾಗಮ builtin_terminals[i].bitc_table;
#endif
	ಯದಿ (STRCMP(term, name) == 0)
	    ಪ್ರತ್ಯಾಗಮ builtin_terminals[i].bitc_table;
    }
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Apply entries from a builtin termcap.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
apply_builtin_tcap(char_u *term, tcap_entry_T *entries, ಪೂರ್ಣಾಙ್ಕ overwrite)
{
    ಪೂರ್ಣಾಙ್ಕ term_8bit = term_is_8bit(term);

    ಪ್ರತಿ (tcap_entry_T *p = entries;
	      p->bt_entry != (ಪೂರ್ಣಾಙ್ಕ)KS_NAME && p->bt_entry != BT_EXTRA_ಕೀಲಿಗಳು; ++p)
    {
	ಯದಿ ((ಪೂರ್ಣಾಙ್ಕ)p->bt_entry >= 0)	// KS_xx entry
	{
	    // Only set the value if it wasn't set yet or "overwrite" is TRUE.
	    ಯದಿ (term_strings[p->bt_entry] == ರಿಕ್ತ
		    || term_strings[p->bt_entry] == empty_option
		    || overwrite)
	    {
#ifdef FEAT_EVAL
		ಪೂರ್ಣಾಙ್ಕ opt_idx = -1;
#endif
		// 8bit terminal: use CSI instead of <Esc>[
		ಯದಿ (term_8bit && term_7to8bit((char_u *)p->bt_string) != 0)
		{
		    char_u  *s, *t;

		    s = vim_strsave((char_u *)p->bt_string);
		    ಯದಿ (s != ರಿಕ್ತ)
		    {
			ಪ್ರತಿ (t = s; *t; ++t)
			    ಯದಿ (term_7to8bit(t))
			    {
				*t = term_7to8bit(t);
				STRMOVE(t + 1, t + 2);
			    }
			term_strings[p->bt_entry] = s;
#ifdef FEAT_EVAL
			opt_idx =
#endif
				  set_term_option_alloced(
						   &term_strings[p->bt_entry]);
		    }
		}
		ಅನ್ಯಥಾ
		{
		    term_strings[p->bt_entry] = (char_u *)p->bt_string;
#ifdef FEAT_EVAL
		    opt_idx = get_term_opt_idx(&term_strings[p->bt_entry]);
#endif
		}
#ifdef FEAT_EVAL
		set_term_option_sctx_idx(ರಿಕ್ತ, opt_idx);
#endif
	    }
	}
	ಅನ್ಯಥಾ
	{
	    char_u  name[2];
	    name[0] = KEY2TERMCAP0((ಪೂರ್ಣಾಙ್ಕ)p->bt_entry);
	    name[1] = KEY2TERMCAP1((ಪೂರ್ಣಾಙ್ಕ)p->bt_entry);
	    ಯದಿ (find_termcode(name) == ರಿಕ್ತ || overwrite)
		add_termcode(name, (char_u *)p->bt_string, term_8bit);
	}
    }
}

/*
 * Apply builtin termcap entries for a given keyprotocol.
 */
    ಶೂನ್ಯ
apply_keyprotocol(char_u *term, keyprot_T prot)
{
    ಯದಿ (prot == KEYPROTOCOL_KITTY)
	apply_builtin_tcap(term, builtin_kitty, TRUE);
    ಯದಿ (prot == KEYPROTOCOL_MOK2)
	apply_builtin_tcap(term, builtin_mok2, TRUE);

    ಯದಿ (prot != KEYPROTOCOL_NONE)
	// Some function keys may accept modifiers even though the
	// terminfo/termcap entry does not indicate this.
	accept_modifiers_for_function_ಕೀಲಿಗಳು();
}

/*
 * Parsing of the builtin termcap entries.
 * Caller should check if "term" is a valid builtin terminal name.
 * The terminal's name is not set, as this is already done in termcapinit().
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
parse_builtin_tcap(char_u *term)
{
    tcap_entry_T *entries = find_builtin_term(term);
    ಯದಿ (entries != ರಿಕ್ತ)
	apply_builtin_tcap(term, entries, FALSE);
}

/*
 * Set number of colors.
 * Store it as a number in t_colors.
 * Store it as a string in T_CCO (using nr_colors[]).
 */
    ಶೂನ್ಯ
set_color_ಎಣಿಕೆ(ಪೂರ್ಣಾಙ್ಕ nr)
{
    char_u	nr_colors[20];		// string for number of colors

    t_colors = nr;
    ಯದಿ (t_colors > 1)
	sprintf((ವರ್ಣ *)nr_colors, "%d", t_colors);
    ಅನ್ಯಥಾ
	*nr_colors = NUL;
#ಯದಿ 0
# ifdef FEAT_TERMGUICOLORS
    // xterm-direct, enable termguicolors, when it wasn't set yet
    ಯದಿ (t_colors == 0x1000000 && !p_tgc_set)
	set_option_ಮೌಲ್ಯ((char_u *)"termguicolors", 1L, ರಿಕ್ತ, 0);
# endif
#endif
    set_string_option_direct((char_u *)"t_Co", -1, nr_colors, OPT_FREE, 0);
}

/*
 * Set the color count to "val" and redraw if it changed.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
may_adjust_color_ಎಣಿಕೆ(ಪೂರ್ಣಾಙ್ಕ val)
{
    ಯದಿ (val == t_colors)
	ಪ್ರತ್ಯಾಗಮ;

    // Nr of colors changed, initialize highlighting and redraw everything.
    // This causes a redraw, which usually clears the message.  Try keeping
    // the message if it might work.
    set_keep_ಸಂದೇಶ_from_hist();
    set_color_ಎಣಿಕೆ(val);
    init_highlight(TRUE, FALSE);
#ifdef DEBUG_TERMRESPONSE
    {
	ಪೂರ್ಣಾಙ್ಕ r = redraw_asap(UPD_CLEAR);

	LOG_TRN("Received t_Co, redraw_asap(): %d", r);
    }
#ಅನ್ಯಥಾ
    redraw_asap(UPD_CLEAR);
#endif
}

#ifdef HAVE_TGETENT
ಸ್ಥಿರ ವರ್ಣ *(ಕೀಲಿ_names[]) =
{
# ifdef FEAT_TERMRESPONSE
    // Do those ones first, both may cause a screen redraw.
    "Co",
    // disabled, because it switches termguicolors, but that
    // is noticeable and confuses users
    // "RGB",
# endif
    "ku", "kd", "kr", "kl",
    "#2", "#4", "%i", "*7",
    "k1", "k2", "k3", "k4", "k5", "k6",
    "k7", "k8", "k9", "k;", "F1", "F2",
    "%1", "&8", "kb", "kI", "kD", "kh",
    "@7", "kP", "kN", "K1", "K3", "K4", "K5", "kB",
    "PS", "PE",
    ರಿಕ್ತ
};
#endif

#ಯದಿ defined(HAVE_TGETENT) || defined(FEAT_TERMGUICOLORS)
/*
 * Return TRUE if "term_strings[idx]" was not set.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
term_strings_not_set(ಗಣನಾ Special_ಕೀಲಿ idx)
{
    ಪ್ರತ್ಯಾಗಮ TERM_STR(idx) == ರಿಕ್ತ || TERM_STR(idx) == empty_option;
}
#endif

#ifdef HAVE_TGETENT
/*
 * Get the termcap entries we need with tgetstr(), tgetflag() and tgetnum().
 * "invoke_tgetent()" must have been called before.
 * If "*height" or "*width" are not zero then use the "li" and "col" entries to
 * get their value.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
get_term_entries(ಪೂರ್ಣಾಙ್ಕ *height, ಪೂರ್ಣಾಙ್ಕ *width)
{
    ಸ್ಥಿರ ಸಂರಚನಾ {
		    ಗಣನಾ Special_ಕೀಲಿ dest; // index in term_strings[]
		    ವರ್ಣ *name;		  // termcap name for string
		  } string_names[] =
		    {	{KS_CE, "ce"}, {KS_AL, "al"}, {KS_CAL,"AL"},
			{KS_DL, "dl"}, {KS_CDL,"DL"}, {KS_CS, "cs"},
			{KS_CL, "cl"}, {KS_CD, "cd"},
			{KS_VI, "vi"}, {KS_VE, "ve"}, {KS_MB, "mb"},
			{KS_ME, "me"}, {KS_MR, "mr"},
			{KS_MD, "md"}, {KS_SE, "se"}, {KS_SO, "so"},
			{KS_CZH,"ZH"}, {KS_CZR,"ZR"}, {KS_UE, "ue"},
			{KS_US, "us"}, {KS_UCE, "Ce"}, {KS_UCS, "Cs"},
			{KS_USS, "Us"}, {KS_DS, "ds"}, {KS_CDS, "Ds"},
			{KS_STE,"Te"}, {KS_STS,"Ts"},
			{KS_CM, "cm"}, {KS_SR, "sr"},
			{KS_CRI,"RI"}, {KS_VB, "vb"}, {KS_KS, "ks"},
			{KS_KE, "ke"}, {KS_TI, "ti"}, {KS_TE, "te"},
			{KS_CTI, "TI"}, {KS_CRK, "RK"}, {KS_CTE, "TE"},
			{KS_BC, "bc"}, {KS_CSB,"Sb"}, {KS_CSF,"Sf"},
			{KS_CAB,"AB"}, {KS_CAF,"AF"}, {KS_CAU,"AU"},
			{KS_LE, "le"},
			{KS_ND, "nd"}, {KS_OP, "op"},
			{KS_CRV, "RV"}, {KS_CXM, "XM"},
			{KS_VS, "vs"}, {KS_CVS, "VS"},
			{KS_CIS, "IS"}, {KS_CIE, "IE"},
			{KS_CSC, "SC"}, {KS_CEC, "EC"},
			{KS_TS, "ts"}, {KS_FS, "fs"},
			{KS_CWP, "WP"}, {KS_CWS, "WS"},
			{KS_CSI, "SI"}, {KS_CEI, "EI"},
			{KS_U7, "u7"}, {KS_RFG, "RF"}, {KS_RBG, "RB"},
			{KS_8F, "8f"}, {KS_8B, "8b"}, {KS_8U, "8u"},
			{KS_CBE, "BE"}, {KS_CBD, "BD"},
			{KS_CST, "ST"}, {KS_CRT, "RT"},
			{KS_SSI, "Si"}, {KS_SRI, "Ri"},
			{KS_CF, "CF"},
			{(ಗಣನಾ Special_ಕೀಲಿ)0, ರಿಕ್ತ}
		    };
    ಪೂರ್ಣಾಙ್ಕ		    i;
    ಸ್ಥಿರ char_u   tstrbuf[TBUFSZ];
    char_u	    *tp = tstrbuf;

    /*
     * get output strings
     */
    ಪ್ರತಿ (i = 0; string_names[i].name != ರಿಕ್ತ; ++i)
    {
	ಯದಿ (term_strings_not_set(string_names[i].dest))
	{
	    TERM_STR(string_names[i].dest) = TGETSTR(string_names[i].name, &tp);
# ifdef FEAT_EVAL
	    set_term_option_sctx_idx(string_names[i].name, -1);
# endif
	}
    }

    // tgetflag() returns 1 if the flag is present, 0 if not and
    // possibly -1 if the flag doesn't exist.
    ಯದಿ ((T_MS == ರಿಕ್ತ || T_MS == empty_option) && tgetflag("ms") > 0)
	T_MS = (char_u *)"y";
    ಯದಿ ((T_XS == ರಿಕ್ತ || T_XS == empty_option) && tgetflag("xs") > 0)
	T_XS = (char_u *)"y";
    ಯದಿ ((T_XN == ರಿಕ್ತ || T_XN == empty_option) && tgetflag("xn") > 0)
	T_XN = (char_u *)"y";
    ಯದಿ ((T_DB == ರಿಕ್ತ || T_DB == empty_option) && tgetflag("db") > 0)
	T_DB = (char_u *)"y";
    ಯದಿ ((T_DA == ರಿಕ್ತ || T_DA == empty_option) && tgetflag("da") > 0)
	T_DA = (char_u *)"y";
    ಯದಿ ((T_UT == ರಿಕ್ತ || T_UT == empty_option) && tgetflag("ut") > 0)
	T_UT = (char_u *)"y";
    ಯದಿ ((T_XON == ರಿಕ್ತ || T_XON == empty_option) && tgetflag("xo") > 0)
	T_XON = (char_u *)"y";

    /*
     * get key codes
     */
    ಪ್ರತಿ (i = 0; ಕೀಲಿ_names[i] != ರಿಕ್ತ; ++i)
	ಯದಿ (find_termcode((char_u *)ಕೀಲಿ_names[i]) == ರಿಕ್ತ)
	{
	    char_u *p = TGETSTR(ಕೀಲಿ_names[i], &tp);

	    // if cursor-left == backspace, ignore it (televideo 925)
	    ಯದಿ (p != ರಿಕ್ತ
		    && (*p != Ctrl_H
			|| ಕೀಲಿ_names[i][0] != 'k'
			|| ಕೀಲಿ_names[i][1] != 'l'))
		add_termcode((char_u *)ಕೀಲಿ_names[i], p, FALSE);
	}

    ಯದಿ (*height == 0)
	*height = tgetnum("li");
    ಯದಿ (*width == 0)
	*width = tgetnum("co");

    /*
     * Get number of colors (if not done already).
     */
    ಯದಿ (term_strings_not_set(KS_CCO))
    {
	set_color_ಎಣಿಕೆ(tgetnum("Co"));
# ifdef FEAT_EVAL
	set_term_option_sctx_idx("Co", -1);
# endif
    }

# ifndef hpux
    BC = (ವರ್ಣ *)TGETSTR("bc", &tp);
    UP = (ವರ್ಣ *)TGETSTR("up", &tp);
    char_u *p = TGETSTR("pc", &tp);
    ಯದಿ (p != ರಿಕ್ತ)
	PC = *p;
# endif
}
#endif

/*
 * Report "term" is not found and list the ones we do know about.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
report_term_error(ವರ್ಣ *error_ಸಂದೇಶ, char_u *term)
{
    mch_errmsg("\r\n");
    ಯದಿ (error_ಸಂದೇಶ != ರಿಕ್ತ)
    {
	mch_errmsg(error_ಸಂದೇಶ);
	mch_errmsg("\r\n");
    }
    mch_errmsg("'");
    mch_errmsg((ವರ್ಣ *)term);
    mch_errmsg(_("' not known. Available builtin terminals are:"));
    mch_errmsg("\r\n");

    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; ; ++i)
    {
	ವರ್ಣ *name = builtin_terminals[i].bitc_name;
	ಯದಿ (name == ರಿಕ್ತ)  // end marker
	    ಭಙ್ಗ;
	// Do not mention the "gui" entry, the user won't need to type it.
	ಯದಿ (STRCMP(name, "gui") != 0)
	{
#ifdef HAVE_TGETENT
	    mch_errmsg("    builtin_");
#ಅನ್ಯಥಾ
	    mch_errmsg("    ");
#endif
	    mch_errmsg(name);
	    mch_errmsg("\r\n");
	}
    }
    // Output extra 'cmdheight' line breaks to avoid that the following error
    // message overwrites the last terminal name.
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 1; i < p_ch; ++i)
	mch_errmsg("\r\n");
}

    ಸ್ಥಿರ ಶೂನ್ಯ
report_default_term(char_u *term)
{
    mch_errmsg(_("defaulting to '"));
    mch_errmsg((ವರ್ಣ *)term);
    mch_errmsg("'\r\n");
    ಯದಿ (emsg_silent == 0 && !in_assert_fails)
    {
	screen_start();	// don't know where cursor is now
	out_flush();
	ಯದಿ (!is_not_a_term())
	    ui_delay(2007L, TRUE);
    }
}

/*
 * Parse the 'keyprotocol' option, match against "term" and return the protocol
 * for the first matching entry.
 * When "term" is NULL then compile all patterns to check for any errors.
 * Returns KEYPROTOCOL_FAIL if a pattern cannot be compiled.
 * Returns KEYPROTOCOL_NONE if there is no match.
 */
    keyprot_T
match_keyprotocol(char_u *term)
{
    ಪೂರ್ಣಾಙ್ಕ		len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(p_kpc) + 1;
    char_u	*buf = alloc(len);
    ಯದಿ (buf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ KEYPROTOCOL_FAIL;

    keyprot_T	ret = KEYPROTOCOL_FAIL;
    char_u	*p = p_kpc;
    ಯಾವತ್ (*p != NUL)
    {
	// Isolate one comma separated item.
	(ಶೂನ್ಯ)copy_option_part(&p, buf, len, ",");
	char_u *colon = vim_strchr(buf, ':');
	ಯದಿ (colon == ರಿಕ್ತ || colon == buf || colon[1] == NUL)
	    ಗಚ್ಛ theend;
	*colon = NUL;

	keyprot_T prot;
	// Note: Keep this in sync with p_kpc_protocol_values.
	ಯದಿ (STRCMP(colon + 1, "none") == 0)
	    prot = KEYPROTOCOL_NONE;
	ಅನ್ಯಥಾ ಯದಿ (STRCMP(colon + 1, "mok2") == 0)
	    prot = KEYPROTOCOL_MOK2;
	ಅನ್ಯಥಾ ಯದಿ (STRCMP(colon + 1, "kitty") == 0)
	    prot = KEYPROTOCOL_KITTY;
	ಅನ್ಯಥಾ
	    ಗಚ್ಛ theend;

	regmatch_T regmatch;
	CLEAR_FIELD(regmatch);
	regmatch.rm_ic = TRUE;
	regmatch.regprog = vim_regcomp(buf, RE_MAGIC);
	ಯದಿ (regmatch.regprog == ರಿಕ್ತ)
	    ಗಚ್ಛ theend;

	ಪೂರ್ಣಾಙ್ಕ match = term != ರಿಕ್ತ && vim_regexec(&regmatch, term, (colnr_T)0);
	vim_regfree(regmatch.regprog);
	ಯದಿ (match)
	{
	    ret = prot;
	    ಗಚ್ಛ theend;
	}

    }

    // No match found, use "none".
    ret = KEYPROTOCOL_NONE;

theend:
    vim_free(buf);
    ಪ್ರತ್ಯಾಗಮ ret;
}

/*
 * Set terminal options for terminal "term".
 * Return OK if terminal 'term' was found in a termcap, FAIL otherwise.
 *
 * While doing this, until ttest(), some options may be NULL, be careful.
 */
    ಪೂರ್ಣಾಙ್ಕ
set_termname(char_u *term)
{
#ifdef HAVE_TGETENT
    ಪೂರ್ಣಾಙ್ಕ		builtin_first = p_tbi;
    ಪೂರ್ಣಾಙ್ಕ		try;
    ಪೂರ್ಣಾಙ್ಕ		termcap_cleared = FALSE;
#endif
    ಪೂರ್ಣಾಙ್ಕ		width = 0, height = 0;
    ವರ್ಣ	*error_ಸಂದೇಶ = ರಿಕ್ತ;
    char_u	*bs_p, *del_p;

    // In silect mode (ex -s) we don't use the 'term' option.
    ಯದಿ (silent_mode)
	ಪ್ರತ್ಯಾಗಮ OK;

    detected_8bit = FALSE;		// reset 8-bit detection

    ಯದಿ (term_is_builtin(term))
    {
	term += 8;
#ifdef HAVE_TGETENT
	builtin_first = 1;
#endif
    }

/*
 * If HAVE_TGETENT is not defined, only the builtin termcap is used, otherwise:
 *   If builtin_first is TRUE:
 *     0. try builtin termcap
 *     1. try external termcap
 *     2. if both fail default to a builtin terminal
 *   If builtin_first is FALSE:
 *     1. try external termcap
 *     2. try builtin termcap, if both fail default to a builtin terminal
 */
#ifdef HAVE_TGETENT
    ಪ್ರತಿ (try = builtin_first ? 0 : 1; try < 3; ++try)
    {
	/*
	 * Use external termcap
	 */
	ಯದಿ (try == 1)
	{
	    char_u	    tbuf[TBUFSZ];

	    /*
	     * If the external termcap does not have a matching entry, try the
	     * builtin ones.
	     */
	    ಯದಿ ((error_ಸಂದೇಶ = invoke_tgetent(tbuf, term)) == ರಿಕ್ತ)
	    {
		ಯದಿ (!termcap_cleared)
		{
		    clear_termoptions();	// clear old options
		    termcap_cleared = TRUE;
		}

		get_term_entries(&height, &width);
	    }
	}
	ಅನ್ಯಥಾ	    // try == 0 || try == 2
#endif // HAVE_TGETENT
	/*
	 * Use builtin termcap
	 */
	{
#ifdef HAVE_TGETENT
	    /*
	     * If builtin termcap was already used, there is no need to search
	     * for the builtin termcap again, quit now.
	     */
	    ಯದಿ (try == 2 && builtin_first && termcap_cleared)
		ಭಙ್ಗ;
#endif
	    /*
	     * Search for 'term' in builtin_terminals[].
	     */
	    tcap_entry_T *termp = find_builtin_term(term);
	    ಯದಿ (termp == ರಿಕ್ತ)	// did not find it
	    {
#ifdef HAVE_TGETENT
		/*
		 * If try == 0, first try the external termcap. If that is not
		 * found we'll get back here with try == 2.
		 * If termcap_cleared is set we used the external termcap,
		 * don't complain about not finding the term in the builtin
		 * termcap.
		 */
		ಯದಿ (try == 0)			// try external one
		    ಅನುವರ್ತ;
		ಯದಿ (termcap_cleared)		// found in external termcap
		    ಭಙ್ಗ;
#endif
		report_term_error(error_ಸಂದೇಶ, term);

		// when user typed :set term=xxx, quit here
		ಯದಿ (starting != NO_SCREEN)
		{
		    screen_start();	// don't know where cursor is now
		    wait_return(TRUE);
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		}
		term = DEFAULT_TERM;
		report_default_term(term);
		set_string_option_direct((char_u *)"term", -1, term,
								 OPT_FREE, 0);
		display_errors();
	    }
	    out_flush();
#ifdef HAVE_TGETENT
	    ಯದಿ (!termcap_cleared)
	    {
#endif
		clear_termoptions();	    // clear old options
#ifdef HAVE_TGETENT
		termcap_cleared = TRUE;
	    }
#endif
	    parse_builtin_tcap(term);

#ifdef FEAT_GUI
	    ಯದಿ (term_is_gui(term))
	    {
		out_flush();
		gui_init();
		// If starting the GUI failed, don't do any of the other
		// things for this terminal
		ಯದಿ (!gui.in_use)
		    ಪ್ರತ್ಯಾಗಮ FAIL;
# ifdef HAVE_TGETENT
		ಭಙ್ಗ;		// don't try using external termcap
# endif
	    }
#endif // FEAT_GUI
	}
#ifdef HAVE_TGETENT
    }
#endif

#ifdef FEAT_GUI
    ಯದಿ (!gui.in_use)
#endif
    {
	// Use the 'keyprotocol' option to adjust the t_TE and t_TI
	// termcap entries if there is an entry matching "term".
	keyprot_T kpc = match_keyprotocol(term);
	apply_keyprotocol(term, kpc);

#ifdef FEAT_TERMGUICOLORS
	// There is no good way to detect that the terminal supports RGB
	// colors.  Since these termcap entries are non-standard anyway and
	// only used when the user sets 'termguicolors' we might as well add
	// them.  But not when one of them was already set.
	ಯದಿ (term_strings_not_set(KS_8F)
		&& term_strings_not_set(KS_8B)
		&& term_strings_not_set(KS_8U))
	    apply_builtin_tcap(term, builtin_rgb, TRUE);
#endif
#ifdef HAVE_TGETENT
	ಯದಿ (term_strings_not_set(KS_CF))
	    apply_builtin_tcap(term, special_term, TRUE);
	ಯದಿ (term_strings_not_set(KS_BSU) && term_strings_not_set(KS_ESU))
	    apply_builtin_tcap(term, builtin_sync_output, TRUE);
#endif
    }

/*
 * special: There is no info in the termcap about whether the cursor
 * positioning is relative to the start of the screen or to the start of the
 * scrolling region.  We just guess here. Only msdos pcterm is known to do it
 * relative.
 */
    ಯದಿ (STRCMP(term, "pcterm") == 0)
	T_CCS = (char_u *)"yes";
    ಅನ್ಯಥಾ
	T_CCS = empty_option;

    // Special case: "kitty" may not have a "RV" entry in terminfo, but we need
    // to request the version for several other things to work.
    ಯದಿ (strstr((ವರ್ಣ *)term, "kitty") != ರಿಕ್ತ
					   && (T_CRV == ರಿಕ್ತ || *T_CRV == NUL))
	T_CRV = (char_u *)"\033[>c";

#ifdef UNIX
/*
 * Any "stty" settings override the default for t_kb from the termcap.
 * This is in os_unix.c, because it depends a lot on the version of unix that
 * is being used.
 * Don't do this when the GUI is active, it uses "t_kb" and "t_kD" directly.
 */
# ifdef FEAT_GUI
    ಯದಿ (!gui.in_use)
# endif
	get_stty();
#endif

/*
 * If the termcap has no entry for 'bs' and/or 'del' and the ioctl() also
 * didn't work, use the default CTRL-H
 * The default for t_kD is DEL, unless t_kb is DEL.
 * The vim_strsave'd strings are probably lost forever, well it's only two
 * bytes.  Don't do this when the GUI is active, it uses "t_kb" and "t_kD"
 * directly.
 */
#ifdef FEAT_GUI
    ಯದಿ (!gui.in_use)
#endif
    {
	bs_p = find_termcode((char_u *)"kb");
	del_p = find_termcode((char_u *)"kD");
	ಯದಿ (bs_p == ರಿಕ್ತ || *bs_p == NUL)
	    add_termcode((char_u *)"kb", (bs_p = (char_u *)CTRL_H_STR), FALSE);
	ಯದಿ ((del_p == ರಿಕ್ತ || *del_p == NUL) &&
					    (bs_p == ರಿಕ್ತ || *bs_p != DEL))
	    add_termcode((char_u *)"kD", (char_u *)DEL_STR, FALSE);
    }

#ಯದಿ defined(UNIX) || defined(VMS)
    term_is_xterm = vim_is_xterm(term);
#endif
#ifdef FEAT_TERMRESPONSE
    // Reset terminal properties that are set based on the termresponse, which
    // will be sent out soon.
    init_term_props(FALSE);
#endif

#ಯದಿ defined(UNIX) || defined(VMS)
    // If the first number in t_XM is 1006 then the terminal will support SGR
    // mouse reporting.
    ಪೂರ್ಣಾಙ್ಕ did_set_ttym = FALSE;
    ಯದಿ (T_CXM != ರಿಕ್ತ && *T_CXM != NUL && !option_was_set((char_u *)"ttym"))
    {
	char_u *p = T_CXM;

	ಯಾವತ್ (*p != NUL && !VIM_ISDIGIT(*p))
	    ++p;
	ಯದಿ (getdigits(&p) == 1006)
	{
	    did_set_ttym = TRUE;
	    set_option_ಮೌಲ್ಯ_give_err((char_u *)"ttym", 0L, (char_u *)"sgr", 0);
	}
    }

    /*
     * For Unix, set the 'ttymouse' option to the type of mouse to be used.
     * The termcode for the mouse is added as a side effect in option.c.
     */
    {
	char_u	*p = (char_u *)"";

# ifdef FEAT_MOUSE_XTERM
	ಯದಿ (use_xterm_like_mouse(term))
	{
	    ಯದಿ (use_xterm_mouse())
		p = ರಿಕ್ತ;	// keep existing value, might be "xterm2"
	    ಅನ್ಯಥಾ
		p = (char_u *)"xterm";
	}
# endif
	ಯದಿ (p != ರಿಕ್ತ && !did_set_ttym)
	{
	    set_option_ಮೌಲ್ಯ_give_err((char_u *)"ttym", 0L, p, 0);
	    // Reset the WAS_SET flag, 'ttymouse' can be set to "sgr" or
	    // "xterm2" in check_termcode().
	    reset_option_was_set((char_u *)"ttym");
	}
	ಯದಿ (p == ರಿಕ್ತ
# ifdef FEAT_GUI
		|| gui.in_use
# endif
		)
	    check_mouse_termcode();	// set mouse termcode anyway
    }
#ಅನ್ಯಥಾ
    set_mouse_termcode(KS_MOUSE, (char_u *)"\233M");
#endif

#ifdef FEAT_MOUSE_XTERM
    // Focus reporting is supported by xterm compatible terminals and tmux.
    // We hard-code the received escape sequences here.  There are the terminfo
    // entries kxIN and kxOUT, but they are rarely used and do hot have a
    // two-letter termcap name.
    // This used to be done only for xterm-like terminals, but some others also
    // may produce these codes.  Always recognize them, as the chance of them
    // being used for something else is very small.
    {
	char_u name[3];

	// handle focus in event
	name[0] = KS_EXTRA;
	name[1] = KE_FOCUSGAINED;
	name[2] = NUL;
	add_termcode(name, (char_u *)"\033[I", FALSE);

	// handle focus out event
	name[1] = KE_FOCUSLOST;
	add_termcode(name, (char_u *)"\033[O", FALSE);

	need_gather = TRUE;
    }
#endif
#ಯದಿ (defined(UNIX) || defined(VMS))
    // First time after setting 'term' a focus event is always reported.
    focus_state = MAYBE;
#endif

#ifdef USE_TERM_CONSOLE
    // DEFAULT_TERM indicates that it is the machine console.
    ಯದಿ (STRCMP(term, DEFAULT_TERM) != 0)
	term_console = FALSE;
    ಅನ್ಯಥಾ
    {
	term_console = TRUE;
# ifdef AMIGA
	win_resize_on();	// enable window resizing reports
# endif
    }
#endif

    ttest(TRUE);	// make sure we have a valid set of terminal codes

    full_screen = TRUE;		// we can use termcap codes from now on
    set_term_defaults();	// use current values as defaults
#ifdef FEAT_TERMRESPONSE
    LOG_TR1("setting crv_status to STATUS_GET");
    crv_status.tr_progress = STATUS_GET;	// Get terminal version later
    write_t_8u_state = FALSE;
#endif

    /*
     * Initialize the terminal with the appropriate termcap codes.
     * Set the mouse and window title if possible.
     * Don't do this when starting, need to parse the .vimrc first, because it
     * may redefine t_TI etc.
     */
    ಯದಿ (starting != NO_SCREEN)
    {
	starttermcap();		// may change terminal mode
	setmouse();		// may start using the mouse
	maketitle();		// may display window title
    }

	// display initial screen after ttest() checking. jw.
    ಯದಿ (width <= 0 || height <= 0)
    {
	// termcap failed to report size
	// set defaults, in case ui_get_shellsize() also fails
	width = 80;
#ಯದಿ defined(MSWIN)
	height = 25;	    // console is often 25 lines
#ಅನ್ಯಥಾ
	height = 24;	    // most terminals are 24 lines
#endif
    }
    set_shellsize(width, height, FALSE);	// may change Rows
    ಯದಿ (starting != NO_SCREEN)
    {
	ಯದಿ (scroll_region)
	    scroll_region_reset();		// In case Rows changed
	check_map_keycodes();	// check mappings for terminal codes used

	{
	    buf_T	*buf;
	    aco_save_T	aco;

	    /*
	     * Execute the TermChanged autocommands for each buffer that is
	     * loaded.
	     */
	    FOR_ALL_BUFFERS(buf)
	    {
		ಯದಿ (curbuf->b_ml.ml_mfp != ರಿಕ್ತ)
		{
		    aucmd_prepbuf(&aco, buf);
		    ಯದಿ (curbuf == buf)
		    {
			apply_autocmds(EVENT_TERMCHANGED, ರಿಕ್ತ, ರಿಕ್ತ, FALSE,
								      curbuf);
			// restore curwin/curbuf and a few other things
			aucmd_restbuf(&aco);
		    }
		}
	    }
	}
    }

#ifdef FEAT_TERMRESPONSE
    may_req_termresponse();
#endif

#ifdef FEAT_IMAGE
    (ಶೂನ್ಯ)update_image_backend();
#endif

    ಪ್ರತ್ಯಾಗಮ OK;
}

#ಯದಿ defined(EXITFREE)

# ifdef HAVE_DEL_CURTERM
#  include <term.h>	    // declares cur_term
# endif

/*
 * If supported, delete "cur_term", which caches terminal related entries.
 * Avoids that valgrind reports possibly lost memory.
 */
    ಶೂನ್ಯ
free_cur_term(ಶೂನ್ಯ)
{
# ifdef HAVE_DEL_CURTERM
    ಯದಿ (cur_term)
	del_curterm(cur_term);
# endif
}

#endif

#ifdef HAVE_TGETENT
/*
 * Call tgetent()
 * Return error message if it fails, NULL if it's OK.
 */
    ಸ್ಥಿರ ವರ್ಣ *
invoke_tgetent(char_u *tbuf, char_u *term)
{
    ಪೂರ್ಣಾಙ್ಕ	    i;

    // Note: Valgrind may report a leak here, because the library keeps one
    // buffer around that we can't ever free.
    i = TGETENT(tbuf, term);
    ಯದಿ (i < 0		    // -1 is always an error
# ifdef TGETENT_ZERO_ERR
	    || i == 0	    // sometimes zero is also an error
# endif
       )
    {
	// On FreeBSD tputs() gets a SEGV after a tgetent() which fails.  Call
	// tgetent() with the always existing "dumb" entry to avoid a crash or
	// hang.
	(ಶೂನ್ಯ)TGETENT(tbuf, "dumb");

	ಯದಿ (i < 0)
# ifdef TGETENT_ZERO_ERR
	    ಪ್ರತ್ಯಾಗಮ _(e_cannot_open_termcap_file);
	ಯದಿ (i == 0)
# endif
# ifdef TERMINFO
	    ಪ್ರತ್ಯಾಗಮ _(e_terminal_entry_not_found_in_terminfo);
# ಅನ್ಯಥಾ
	    ಪ್ರತ್ಯಾಗಮ _(e_terminal_entry_not_found_in_termcap);
# endif
    }
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Some versions of tgetstr() have been reported to return -1 instead of NULL.
 * Fix that here.
 */
    ಸ್ಥಿರ char_u *
vim_tgetstr(ವರ್ಣ *s, char_u **pp)
{
    ವರ್ಣ	*p;

    p = tgetstr(s, (ವರ್ಣ **)pp);
    ಯದಿ (p == (ವರ್ಣ *)-1)
	p = ರಿಕ್ತ;
    ಪ್ರತ್ಯಾಗಮ (char_u *)p;
}
#endif // HAVE_TGETENT

#ಯದಿ defined(HAVE_TGETENT) && (defined(UNIX) || defined(VMS) || defined(MACOS_X))
/*
 * Get Columns and Rows from the termcap. Used after a window signal if the
 * ioctl() fails. It doesn't make sense to call tgetent each time if the "co"
 * and "li" entries never change. But on some systems this works.
 * Errors while getting the entries are ignored.
 */
    ಶೂನ್ಯ
getlinecol(
    long	*cp,	// pointer to columns
    long	*rp)	// pointer to rows
{
    char_u	tbuf[TBUFSZ];

    ಯದಿ (T_NAME == ರಿಕ್ತ || *T_NAME == NUL
	    || invoke_tgetent(tbuf, T_NAME) != ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (*cp == 0)
	*cp = tgetnum("co");
    ಯದಿ (*rp == 0)
	*rp = tgetnum("li");
}
#endif // defined(HAVE_TGETENT) && defined(UNIX)

/*
 * Get a string entry from the termcap and add it to the list of termcodes.
 * Used for <t_xx> special keys.
 * Give an error message for failure when not sourcing.
 * If force given, replace an existing entry.
 * Return FAIL if the entry was not found, OK if the entry was added.
 */
    ಪೂರ್ಣಾಙ್ಕ
add_termcap_entry(char_u *name, ಪೂರ್ಣಾಙ್ಕ force)
{
    char_u  *term;
    ಪೂರ್ಣಾಙ್ಕ	    ಕೀಲಿ;
#ifdef HAVE_TGETENT
    char_u  *string;
    ಪೂರ್ಣಾಙ್ಕ	    i;
    ಪೂರ್ಣಾಙ್ಕ	    builtin_first;
    char_u  tbuf[TBUFSZ];
    char_u  tstrbuf[TBUFSZ];
    char_u  *tp = tstrbuf;
    ವರ್ಣ    *error_ಸಂದೇಶ = ರಿಕ್ತ;
#endif

/*
 * If the GUI is running or will start in a moment, we only support the keys
 * that the GUI can produce.
 */
#ifdef FEAT_GUI
    ಯದಿ (gui.in_use || gui.starting)
	ಪ್ರತ್ಯಾಗಮ gui_mch_haskey(name);
#endif

    ಯದಿ (!force && find_termcode(name) != ರಿಕ್ತ)	    // it's already there
	ಪ್ರತ್ಯಾಗಮ OK;

    term = T_NAME;
    ಯದಿ (term == ರಿಕ್ತ || *term == NUL)	    // 'term' not defined yet
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಯದಿ (term_is_builtin(term))		    // name starts with "builtin_"
    {
	term += 8;
#ifdef HAVE_TGETENT
	builtin_first = TRUE;
#endif
    }
#ifdef HAVE_TGETENT
    ಅನ್ಯಥಾ
	builtin_first = p_tbi;
#endif

#ifdef HAVE_TGETENT
/*
 * We can get the entry from the builtin termcap and from the external one.
 * If 'ttybuiltin' is on or the terminal name starts with "builtin_", try
 * builtin termcap first.
 * If 'ttybuiltin' is off, try external termcap first.
 */
    ಪ್ರತಿ (i = 0; i < 2; ++i)
    {
	ಯದಿ ((!builtin_first) == i)
#endif
	/*
	 * Search in builtin termcaps
	 */
	{
	    tcap_entry_T *termp = find_builtin_term(term);
	    ಯದಿ (termp != ರಿಕ್ತ)	// found it
	    {
		ಕೀಲಿ = TERMCAP2_ಕೀಲಿ(name[0], name[1]);
		++termp;
		ಯಾವತ್ (termp->bt_entry != (ಪೂರ್ಣಾಙ್ಕ)KS_NAME)
		{
		    ಯದಿ ((ಪೂರ್ಣಾಙ್ಕ)termp->bt_entry == ಕೀಲಿ)
		    {
			add_termcode(name, (char_u *)termp->bt_string,
							  term_is_8bit(term));
			ಪ್ರತ್ಯಾಗಮ OK;
		    }
		    ++termp;
		}
	    }
	}
#ifdef HAVE_TGETENT
	ಅನ್ಯಥಾ
	/*
	 * Search in external termcap
	 */
	{
	    error_ಸಂದೇಶ = invoke_tgetent(tbuf, term);
	    ಯದಿ (error_ಸಂದೇಶ == ರಿಕ್ತ)
	    {
		string = TGETSTR((ವರ್ಣ *)name, &tp);
		ಯದಿ (string != ರಿಕ್ತ && *string != NUL)
		{
		    add_termcode(name, string, FALSE);
		    ಪ್ರತ್ಯಾಗಮ OK;
		}
	    }
	}
    }
#endif

    ಯದಿ (SOURCING_NAME == ರಿಕ್ತ)
    {
#ifdef HAVE_TGETENT
	ಯದಿ (error_ಸಂದೇಶ != ರಿಕ್ತ)
	    emsg(error_ಸಂದೇಶ);
	ಅನ್ಯಥಾ
#endif
	    semsg(_(e_no_str_entry_in_termcap), name);
    }
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
term_is_builtin(char_u *name)
{
    ಪ್ರತ್ಯಾಗಮ (STRNCMP(name, "builtin_", (size_t)8) == 0);
}

/*
 * Return TRUE if terminal "name" uses CSI instead of <Esc>[.
 * Assume that the terminal is using 8-bit controls when the name contains
 * "8bit", like in "xterm-8bit".
 */
    ಪೂರ್ಣಾಙ್ಕ
term_is_8bit(char_u *name)
{
    ಪ್ರತ್ಯಾಗಮ (detected_8bit || strstr((ವರ್ಣ *)name, "8bit") != ರಿಕ್ತ);
}

/*
 * Translate terminal control chars from 7-bit to 8-bit:
 * <Esc>[ -> CSI  <M_C_[>
 * <Esc>] -> OSC  <M-C-]>
 * <Esc>O -> <M-C-O>
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
term_7to8bit(char_u *p)
{
    ಯದಿ (*p != ESC)
	ಪ್ರತ್ಯಾಗಮ 0;

    ಯದಿ (p[1] == '[')
	ಪ್ರತ್ಯಾಗಮ CSI;
    ಅನ್ಯಥಾ ಯದಿ (p[1] == ']')
	ಪ್ರತ್ಯಾಗಮ OSC;
    ಅನ್ಯಥಾ ಯದಿ (p[1] == 'O')
	ಪ್ರತ್ಯಾಗಮ 0x8f;
    ಪ್ರತ್ಯಾಗಮ 0;
}

#ಯದಿ defined(FEAT_GUI)
    ಪೂರ್ಣಾಙ್ಕ
term_is_gui(char_u *name)
{
    ಪ್ರತ್ಯಾಗಮ (STRCMP(name, "builtin_gui") == 0 || STRCMP(name, "gui") == 0);
}
#endif

#ಯದಿ !defined(HAVE_TGETENT) || defined(AMIGA)

    char_u *
tltoa(unsigned long i)
{
    ಸ್ಥಿರ char_u buf[16];
    char_u	*p;

    p = buf + 15;
    *p = '\0';
    ಕುರು
    {
	--p;
	*p = (char_u) (i % 10 + '0');
	i /= 10;
    }
    ಯಾವತ್ (i > 0 && p > buf);
    ಪ್ರತ್ಯಾಗಮ p;
}
#endif

#ifndef HAVE_TGETENT

/*
 * minimal tgoto() implementation.
 * no padding and we only parse for %i %d and %+char
 */
    ಸ್ಥಿರ ವರ್ಣ *
tgoto(ವರ್ಣ *cm, ಪೂರ್ಣಾಙ್ಕ x, ಪೂರ್ಣಾಙ್ಕ y)
{
    ಸ್ಥಿರ ವರ್ಣ buf[30];
    ವರ್ಣ *p, *s, *e;

    ಯದಿ (!cm)
	ಪ್ರತ್ಯಾಗಮ "OOPS";
    e = buf + 29;
    ಪ್ರತಿ (s = buf; s < e && *cm; cm++)
    {
	ಯದಿ (*cm != '%')
	{
	    *s++ = *cm;
	    ಅನುವರ್ತ;
	}
	ವಿಕಲ್ಪ (*++cm)
	{
	ಪಕ್ಷ 'd':
	    p = (ವರ್ಣ *)tltoa((unsigned long)y);
	    y = x;
	    ಯಾವತ್ (*p)
		*s++ = *p++;
	    ಭಙ್ಗ;
	ಪಕ್ಷ 'i':
	    x++;
	    y++;
	    ಭಙ್ಗ;
	ಪಕ್ಷ '+':
	    *s++ = (ವರ್ಣ)(*++cm + y);
	    y = x;
	    ಭಙ್ಗ;
	ಪಕ್ಷ '%':
	    *s++ = *cm;
	    ಭಙ್ಗ;
	ಸ್ವಭಾವ:
	    ಪ್ರತ್ಯಾಗಮ "OOPS";
	}
    }
    *s = '\0';
    ಪ್ರತ್ಯಾಗಮ buf;
}

#endif // HAVE_TGETENT

/*
 * Set the terminal name and initialize the terminal options.
 * If "name" is NULL or empty, get the terminal name from the environment.
 * If that fails, use the default terminal name.
 */
    ಶೂನ್ಯ
termcapinit(char_u *name)
{
    char_u	*term = name;

    ಯದಿ (term != ರಿಕ್ತ && *term == NUL)
	term = ರಿಕ್ತ;	    // empty name is equal to no name

#ifndef MSWIN
    ಯದಿ (term == ರಿಕ್ತ)
	term = mch_getenv((char_u *)"TERM");
#endif
    ಯದಿ (term == ರಿಕ್ತ || *term == NUL)
	term = DEFAULT_TERM;
    set_string_option_direct((char_u *)"term", -1, term, OPT_FREE, 0);

    // Set the default terminal name.
    set_string_default("term", term);
    set_string_default("ttytype", term);

    // Avoid using "term" here, because the next mch_getenv() may overwrite it.
    set_termname(T_NAME != ರಿಕ್ತ ? T_NAME : term);
}

/*
 * The number of calls to ui_write is reduced by using "out_buf".
 */
#define OUT_SIZE	8191

// add one to allow mch_write() in os_win32.c to append a NUL
ಸ್ಥಿರ char_u		out_buf[OUT_SIZE + 1];

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ		out_pos = 0;	// number of chars in out_buf

// Since the maximum number of SGR parameters shown as a normal value range is
// 16, the escape sequence length can be 4 * 16 + lead + tail.
#define MAX_ESC_SEQ_LEN	80

/*
 * out_flush(): flush the output buffer
 */
    ಶೂನ್ಯ
out_flush(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ	    len;

    ಯದಿ (out_pos == 0)
	ಪ್ರತ್ಯಾಗಮ;

    // set out_pos to 0 before ui_write, to avoid recursiveness
    len = out_pos;
    out_pos = 0;
    ui_write(out_buf, len, FALSE);
#ifdef FEAT_EVAL
    ಯದಿ (ch_log_output != FALSE)
    {
	out_buf[len] = NUL;
	ch_log(ರಿಕ್ತ, "raw %s output: \"%s\"",
# ifdef FEAT_GUI
		(gui.in_use && !gui.dying && !gui.starting) ? "GUI" :
# endif
		"terminal",
		out_buf);
	ಯದಿ (ch_log_output == TRUE)
	    ch_log_output = FALSE;  // only log once
    }
#endif
}

/*
 * out_flush_cursor(): flush the output buffer and redraw the cursor.
 * Does not flush recursively in the GUI to avoid slow drawing.
 */
    ಶೂನ್ಯ
out_flush_cursor(
    ಪೂರ್ಣಾಙ್ಕ	    force UNUSED,   // when TRUE, update cursor even when not moved
    ಪೂರ್ಣಾಙ್ಕ	    clear_selection UNUSED) // clear selection under cursor
{
    mch_disable_flush();
    out_flush();
    mch_enable_flush();
#ifdef FEAT_GUI
    ಯದಿ (gui.in_use)
    {
	gui_update_cursor(force, clear_selection);
	gui_may_flush();
    }
#endif
}


/*
 * Sometimes a byte out of a multi-byte character is written with out_char().
 * To avoid flushing half of the character, call this function first.
 */
    ಶೂನ್ಯ
out_flush_check(ಶೂನ್ಯ)
{
    ಯದಿ (enc_dbcs != 0 && out_pos >= OUT_SIZE - MB_MAXBYTES)
	out_flush();
}

#ifdef FEAT_GUI
/*
 * out_trash(): Throw away the contents of the output buffer
 */
    ಶೂನ್ಯ
out_trash(ಶೂನ್ಯ)
{
    out_pos = 0;
}
#endif

/*
 * out_char(c): put a byte into the output buffer.
 *		Flush it if it becomes full.
 * This should not be used for outputting text on the screen (use functions
 * like msg_puts() and screen_putchar() for that).
 */
    ಶೂನ್ಯ
out_char(unsigned c)
{
#ಯದಿ defined(UNIX) || defined(VMS) || defined(AMIGA) || defined(MACOS_X)
    ಯದಿ (c == '\n')	// turn LF into CR-LF (CRMOD doesn't seem to do this)
	out_char('\r');
#endif

    out_buf[out_pos++] = c;

    // For testing we flush each time.
    ಯದಿ (out_pos >= OUT_SIZE || p_wd)
	out_flush();
}

/*
 * Output "c" like out_char(), but don't flush when p_wd is set.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
out_char_nf(ಪೂರ್ಣಾಙ್ಕ c)
{
    out_buf[out_pos++] = (unsigned)c;

    ಯದಿ (out_pos >= OUT_SIZE)
	out_flush();
    ಪ್ರತ್ಯಾಗಮ (unsigned)c;
}

/*
 * A never-padding out_str().
 * Use this whenever you don't want to run the string through tputs().
 * tputs() above is harmless, but tputs() from the termcap library
 * is likely to strip off leading digits, that it mistakes for padding
 * information, and "%i", "%d", etc.
 * This should only be used for writing terminal codes, not for outputting
 * normal text (use functions like msg_puts() and screen_putchar() for that).
 */
    ಶೂನ್ಯ
out_str_nf(char_u *s)
{
    // avoid terminal strings being split up
    ಯದಿ (out_pos > OUT_SIZE - MAX_ESC_SEQ_LEN)
	out_flush();

    ಪ್ರತಿ (char_u *p = s; *p != NUL; ++p)
	out_char_nf(*p);

    // For testing we write one string at a time.
    ಯದಿ (p_wd)
	out_flush();
}

/*
 * A conditional-flushing out_str, mainly for visualbell.
 * Handles a delay internally, because termlib may not respect the delay or do
 * it at the wrong time.
 * Note: Only for terminal strings.
 */
    ಶೂನ್ಯ
out_str_cf(char_u *s)
{
    ಯದಿ (s == ರಿಕ್ತ || *s == NUL)
	ಪ್ರತ್ಯಾಗಮ;

#ifdef HAVE_TGETENT
    char_u *p;
#endif

#ifdef FEAT_GUI
    // Don't use tputs() when GUI is used, ncurses crashes.
    ಯದಿ (gui.in_use)
    {
	out_str_nf(s);
	ಪ್ರತ್ಯಾಗಮ;
    }
#endif
    ಯದಿ (out_pos > OUT_SIZE - MAX_ESC_SEQ_LEN)
	out_flush();
#ifdef HAVE_TGETENT
    ಪ್ರತಿ (p = s; *s; ++s)
    {
	// flush just before delay command
	ಯದಿ (*s == '$' && *(s + 1) == '<')
	{
	    char_u save_c = *s;
	    ಪೂರ್ಣಾಙ್ಕ duration = atoi((ವರ್ಣ *)s + 2);

	    *s = NUL;
	    tputs((ವರ್ಣ *)p, 1, TPUTSFUNCAST out_char_nf);
	    *s = save_c;
	    out_flush();
# ifdef ELAPSED_FUNC
	    // Only sleep here if we can limit this happening in
	    // vim_beep().
	    p = vim_strchr(s, '>');
	    ಯದಿ (p == ರಿಕ್ತ || duration <= 0)
	    {
		// can't parse the time, don't sleep here
		p = s;
	    }
	    ಅನ್ಯಥಾ
	    {
		++p;
		do_sleep(duration, FALSE);
	    }
# ಅನ್ಯಥಾ
	    // Rely on the terminal library to sleep.
	    p = s;
# endif
	    ಭಙ್ಗ;
	}
    }
    tputs((ವರ್ಣ *)p, 1, TPUTSFUNCAST out_char_nf);
#ಅನ್ಯಥಾ
    ಯಾವತ್ (*s)
	out_char_nf(*s++);
#endif

    // For testing we write one string at a time.
    ಯದಿ (p_wd)
	out_flush();
}

/*
 * out_str(s): Put a character string a byte at a time into the output buffer.
 * If HAVE_TGETENT is defined use tputs(), the termcap parser. (jw)
 * This should only be used for writing terminal codes, not for outputting
 * normal text (use functions like msg_puts() and screen_putchar() for that).
 */
    ಶೂನ್ಯ
out_str(char_u *s)
{
    ಯದಿ (s == ರಿಕ್ತ || *s == NUL)
	ಪ್ರತ್ಯಾಗಮ;

#ifdef FEAT_GUI
    // Don't use tputs() when GUI is used, ncurses crashes.
    ಯದಿ (gui.in_use)
    {
	out_str_nf(s);
	ಪ್ರತ್ಯಾಗಮ;
    }
#endif
    // avoid terminal strings being split up
    ಯದಿ (out_pos > OUT_SIZE - MAX_ESC_SEQ_LEN)
	out_flush();
#ifdef HAVE_TGETENT
    tputs((ವರ್ಣ *)s, 1, TPUTSFUNCAST out_char_nf);
#ಅನ್ಯಥಾ
    ಯಾವತ್ (*s)
	out_char_nf(*s++);
#endif

    // For testing we write one string at a time.
    ಯದಿ (p_wd)
	out_flush();
}

/*
 * cursor positioning using termcap parser. (jw)
 */
    ಶೂನ್ಯ
term_windgoto(ಪೂರ್ಣಾಙ್ಕ row, ಪೂರ್ಣಾಙ್ಕ col)
{
    OUT_STR(tgoto((ವರ್ಣ *)T_CM, col, row));
}

    ಶೂನ್ಯ
term_cursor_ಬಲ(ಪೂರ್ಣಾಙ್ಕ i)
{
    OUT_STR(tgoto((ವರ್ಣ *)T_CRI, 0, i));
}

    ಶೂನ್ಯ
term_append_ಸಾಲುಗಳು(ಪೂರ್ಣಾಙ್ಕ ಸಾಲು_ಎಣಿಕೆ)
{
    OUT_STR(tgoto((ವರ್ಣ *)T_CAL, 0, ಸಾಲು_ಎಣಿಕೆ));
}

    ಶೂನ್ಯ
term_delete_ಸಾಲುಗಳು(ಪೂರ್ಣಾಙ್ಕ ಸಾಲು_ಎಣಿಕೆ)
{
    OUT_STR(tgoto((ವರ್ಣ *)T_CDL, 0, ಸಾಲು_ಎಣಿಕೆ));
}

#ಯದಿ defined(UNIX) || defined(VMS)
    ಶೂನ್ಯ
term_enable_mouse(ಪೂರ್ಣಾಙ್ಕ enable)
{
    ಪೂರ್ಣಾಙ್ಕ on = enable ? 1 : 0;
    OUT_STR(tgoto((ವರ್ಣ *)T_CXM, 0, on));
}
#endif

#ಯದಿ defined(HAVE_TGETENT)
    ಶೂನ್ಯ
term_set_winpos(ಪೂರ್ಣಾಙ್ಕ x, ಪೂರ್ಣಾಙ್ಕ y)
{
    // Can't handle a negative value here
    ಯದಿ (x < 0)
	x = 0;
    ಯದಿ (y < 0)
	y = 0;
    OUT_STR(tgoto((ವರ್ಣ *)T_CWP, y, x));
}

# ಯದಿ defined(FEAT_TERMRESPONSE)
/*
 * Return TRUE if we can request the terminal for a response.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
can_get_termresponse(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ cur_tmode == TMODE_RAW
	    && termcap_active
#  ifdef UNIX
	    && (is_not_a_term() || (isatty(1) && isatty(read_cmd_fd)))
#  endif
	    && p_ek;
}

/*
 * Set "status" to STATUS_SENT.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
termrequest_sent(termrequest_T *status)
{
    status->tr_progress = STATUS_SENT;
    status->tr_start = time(ರಿಕ್ತ);
}

/*
 * Return TRUE if any of the requests are in STATUS_SENT.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
termrequest_any_pending(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ	    i;
    time_t  now = time(ರಿಕ್ತ);

    ಪ್ರತಿ (i = 0; all_termrequests[i] != ರಿಕ್ತ; ++i)
    {
	ಯದಿ (all_termrequests[i]->tr_progress == STATUS_SENT)
	{
	    ಯದಿ (all_termrequests[i]->tr_start > 0 && now > 0
				    && all_termrequests[i]->tr_start + 2 < now)
		// Sent the request more than 2 seconds ago and didn't get a
		// response, assume it failed.
		all_termrequests[i]->tr_progress = STATUS_FAIL;
	    ಅನ್ಯಥಾ
		ಪ್ರತ್ಯಾಗಮ TRUE;
	}
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ winpos_x = -1;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ winpos_y = -1;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ did_request_winpos = 0;

#  ಯದಿ defined(FEAT_EVAL) || defined(FEAT_TERMINAL)
/*
 * Try getting the Vim window position from the terminal.
 * Returns OK or FAIL.
 */
    ಪೂರ್ಣಾಙ್ಕ
term_get_winpos(ಪೂರ್ಣಾಙ್ಕ *x, ಪೂರ್ಣಾಙ್ಕ *y, varnumber_T timeout)
{
    ಪೂರ್ಣಾಙ್ಕ ಎಣಿಕೆ = 0;
    ಪೂರ್ಣಾಙ್ಕ prev_winpos_x = winpos_x;
    ಪೂರ್ಣಾಙ್ಕ prev_winpos_y = winpos_y;

    ಯದಿ (*T_CGP == NUL || !can_get_termresponse())
	ಪ್ರತ್ಯಾಗಮ FAIL;
    winpos_x = -1;
    winpos_y = -1;
    ++did_request_winpos;
    termrequest_sent(&winpos_status);
    OUT_STR(T_CGP);
    out_flush();

    // Try reading the result for "timeout" msec.
    ಯಾವತ್ (ಎಣಿಕೆ++ <= timeout / 10 && !got_int)
    {
	(ಶೂನ್ಯ)vpeekc_nomap();
	ಯದಿ (winpos_x >= 0 && winpos_y >= 0)
	{
	    *x = winpos_x;
	    *y = winpos_y;
	    ಪ್ರತ್ಯಾಗಮ OK;
	}
	ui_delay(10L, FALSE);
    }
    // Do not reset "did_request_winpos", if we timed out the response might
    // still come later and we must consume it.

    winpos_x = prev_winpos_x;
    winpos_y = prev_winpos_y;
    ಯದಿ (timeout < 10 && prev_winpos_y >= 0 && prev_winpos_x >= 0)
    {
	// Polling: return previous values if we have them.
	*x = winpos_x;
	*y = winpos_y;
	ಪ್ರತ್ಯಾಗಮ OK;
    }

    ಪ್ರತ್ಯಾಗಮ FALSE;
}
#  endif
# endif

    ಶೂನ್ಯ
term_set_winsize(ಪೂರ್ಣಾಙ್ಕ height, ಪೂರ್ಣಾಙ್ಕ width)
{
    OUT_STR(tgoto((ವರ್ಣ *)T_CWS, width, height));
}
#endif

    ಶೂನ್ಯ
term_font(ಪೂರ್ಣಾಙ್ಕ n)
{
    ಯದಿ (*T_CFO)
    {
	ವರ್ಣ buf[20];
	sprintf(buf, (ವರ್ಣ *)T_CFO, 9 + n);
	OUT_STR(buf);
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
term_color(char_u *s, ಪೂರ್ಣಾಙ್ಕ n)
{
    ವರ್ಣ	buf[20];
    ಪೂರ್ಣಾಙ್ಕ		i = *s == CSI ? 1 : 2;
		// index in s[] just after <Esc>[ or CSI

    // Special handling of 16 colors, because termcap can't handle it
    // Also accept "\e[3%dm" for TERMINFO, it is sometimes used
    // Also accept CSI instead of <Esc>[
    ಯದಿ (n >= 8 && t_colors >= 16
	      && ((s[0] == ESC && s[1] == '[')
#ಯದಿ defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
		  || (s[0] == ESC && s[1] == '|')
#endif
		  || (s[0] == CSI && (i = 1) == 1))
	      && s[i] != NUL
	      && (STRCMP(s + i + 1, "%p1%dm") == 0
		  || STRCMP(s + i + 1, "%dm") == 0)
	      && (s[i] == '3' || s[i] == '4'))
    {
#ifdef TERMINFO
	ವರ್ಣ *format = "%s%s%%p1%%dm";
#ಅನ್ಯಥಾ
	ವರ್ಣ *format = "%s%s%%dm";
#endif
	ವರ್ಣ *lead = i == 2 ? (
#ಯದಿ defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
		    s[1] == '|' ? "\033|" :
#endif
		    "\033[") : "\233";
	ವರ್ಣ *tail = s[i] == '3' ? (n >= 16 ? "38;5;" : "9")
				 : (n >= 16 ? "48;5;" : "10");

	sprintf(buf, format, lead, tail);
	OUT_STR(tgoto(buf, 0, n >= 16 ? n : n - 8));
    }
    ಅನ್ಯಥಾ
	OUT_STR(tgoto((ವರ್ಣ *)s, 0, n));
}

    ಶೂನ್ಯ
term_fg_color(ಪೂರ್ಣಾಙ್ಕ n)
{
    // Use "AF" termcap entry if present, "Sf" entry otherwise
    ಯದಿ (*T_CAF)
	term_color(T_CAF, n);
    ಅನ್ಯಥಾ ಯದಿ (*T_CSF)
	term_color(T_CSF, n);
}

    ಶೂನ್ಯ
term_bg_color(ಪೂರ್ಣಾಙ್ಕ n)
{
    // Use "AB" termcap entry if present, "Sb" entry otherwise
    ಯದಿ (*T_CAB)
	term_color(T_CAB, n);
    ಅನ್ಯಥಾ ಯದಿ (*T_CSB)
	term_color(T_CSB, n);
}

    ಶೂನ್ಯ
term_ul_color(ಪೂರ್ಣಾಙ್ಕ n)
{
    ಯದಿ (*T_CAU)
	term_color(T_CAU, n);
}

/*
 * Return "dark" or "light" depending on the kind of terminal.
 * This is just guessing!  Recognized are:
 * "linux"	    Linux console
 * "screen.linux"   Linux console with screen
 * "cygwin.*"	    Cygwin shell
 * "putty.*"	    Putty program
 * We also check the COLORFGBG environment variable, which is set by
 * rxvt and derivatives. This variable contains either two or three
 * values separated by semicolons; we want the last value in either
 * case. If this value is 0-6 or 8, our background is dark.
 */
    char_u *
term_bg_default(ಶೂನ್ಯ)
{
#ಯದಿ defined(MSWIN)
    // DOS console is nearly always black
    ಪ್ರತ್ಯಾಗಮ (char_u *)"dark";
#ಅನ್ಯಥಾ
    char_u	*p;

    ಯದಿ (STRCMP(T_NAME, "linux") == 0
	    || STRCMP(T_NAME, "screen.linux") == 0
	    || STRNCMP(T_NAME, "cygwin", 6) == 0
	    || STRNCMP(T_NAME, "putty", 5) == 0
	    || ((p = mch_getenv((char_u *)"COLORFGBG")) != ರಿಕ್ತ
		&& (p = vim_strrchr(p, ';')) != ರಿಕ್ತ
		&& ((p[1] >= '0' && p[1] <= '6') || p[1] == '8')
		&& p[2] == NUL))
	ಪ್ರತ್ಯಾಗಮ (char_u *)"dark";
    ಪ್ರತ್ಯಾಗಮ (char_u *)"light";
#endif
}

#ಯದಿ defined(FEAT_TERMGUICOLORS)

# define RED(rgb)   (((long_u)(rgb) >> 16) & 0xFF)
# define GREEN(rgb) (((long_u)(rgb) >>  8) & 0xFF)
# define BLUE(rgb)  (((long_u)(rgb)      ) & 0xFF)

    ಸ್ಥಿರ ಶೂನ್ಯ
term_rgb_color(char_u *s, guicolor_T rgb)
{
# define MAX_COLOR_STR_LEN 100
    ವರ್ಣ	buf[MAX_COLOR_STR_LEN];

    ಯದಿ (*s == NUL)
	ಪ್ರತ್ಯಾಗಮ;
    vim_snprintf(buf, MAX_COLOR_STR_LEN,
				  (ವರ್ಣ *)s, RED(rgb), GREEN(rgb), BLUE(rgb));
# ifdef FEAT_VTP
    ಯದಿ (use_vtp() && (p_tgc || t_colors >= 256))
    {
	out_flush();
	buf[1] = '[';
	vtp_printf(buf);
    }
    ಅನ್ಯಥಾ
# endif
	OUT_STR(buf);
}

    ಶೂನ್ಯ
term_fg_rgb_color(guicolor_T rgb)
{
    ಯದಿ (rgb != INVALCOLOR)
	term_rgb_color(T_8F, rgb);
}

    ಶೂನ್ಯ
term_bg_rgb_color(guicolor_T rgb)
{
    ಯದಿ (rgb != INVALCOLOR)
	term_rgb_color(T_8B, rgb);
}

    ಶೂನ್ಯ
term_ul_rgb_color(guicolor_T rgb)
{
# ifdef FEAT_TERMRESPONSE
    // If the user explicitly sets t_8u then use it.  Otherwise wait for
    // termresponse to be received, which is when t_8u would be set and a
    // redraw is needed if it was used.
    ಯದಿ (!option_was_set((char_u *)"t_8u") && write_t_8u_state != OK)
	write_t_8u_state = MAYBE;
    ಅನ್ಯಥಾ
# endif
	term_rgb_color(T_8U, rgb);
}
#endif

#ಯದಿ defined(UNIX) || defined(VMS) || defined(MACOS_X)
/*
 * Generic function to set window title, using t_ts and t_fs.
 */
    ಶೂನ್ಯ
term_settitle(char_u *title)
{
    MAY_WANT_TO_LOG_THIS;

    // t_ts takes one argument: column in status line
    OUT_STR(tgoto((ವರ್ಣ *)T_TS, 0, 0));	// set title start
    out_str_nf(title);
    out_str(T_FS);			// set title end
    out_flush();
}

/*
 * Tell the terminal to push (save) the title and/or icon, so that it can be
 * popped (restored) later.
 */
    ಶೂನ್ಯ
term_ತಳ್ಳು_title(ಪೂರ್ಣಾಙ್ಕ which)
{
    ಯದಿ ((which & SAVE_RESTORE_TITLE) && T_CST != ರಿಕ್ತ && *T_CST != NUL)
    {
	OUT_STR(T_CST);
	out_flush();
    }

    ಯದಿ ((which & SAVE_RESTORE_ICON) && T_SSI != ರಿಕ್ತ && *T_SSI != NUL)
    {
	OUT_STR(T_SSI);
	out_flush();
    }
}

/*
 * Tell the terminal to pop the title and/or icon.
 */
    ಶೂನ್ಯ
term_ತೆಗೆ_title(ಪೂರ್ಣಾಙ್ಕ which)
{
    ಯದಿ ((which & SAVE_RESTORE_TITLE) && T_CRT != ರಿಕ್ತ && *T_CRT != NUL)
    {
	OUT_STR(T_CRT);
	out_flush();
    }

    ಯದಿ ((which & SAVE_RESTORE_ICON) && T_SRI != ರಿಕ್ತ && *T_SRI != NUL)
    {
	OUT_STR(T_SRI);
	out_flush();
    }
}
#endif

/*
 * Make sure we have a valid set or terminal options.
 * Replace all entries that are NULL by empty_option
 */
    ಶೂನ್ಯ
ttest(ಪೂರ್ಣಾಙ್ಕ pairs)
{
    char_u *env_colors;

    check_options();		    // make sure no options are NULL

    /*
     * MUST have "cm": cursor motion.
     */
    ಯದಿ (*T_CM == NUL)
	emsg(_(e_terminal_capability_cm_required));

    /*
     * if "cs" defined, use a scroll region, it's faster.
     */
    ಯದಿ (*T_CS != NUL)
	scroll_region = TRUE;
    ಅನ್ಯಥಾ
	scroll_region = FALSE;

    ಯದಿ (pairs)
    {
	/*
	 * optional pairs
	 */
	// TP goes to normal mode for TI (invert) and TB (bold)
	ಯದಿ (*T_ME == NUL)
	    T_ME = T_MR = T_MD = T_MB = empty_option;
	ಯದಿ (*T_SO == NUL || *T_SE == NUL)
	    T_SO = T_SE = empty_option;
	ಯದಿ (*T_US == NUL || *T_UE == NUL)
	    T_US = T_UE = empty_option;
	ಯದಿ (*T_CZH == NUL || *T_CZR == NUL)
	    T_CZH = T_CZR = empty_option;

	// T_VE is needed even though T_VI is not defined
	ಯದಿ (*T_VE == NUL)
	    T_VI = empty_option;

	// if 'mr' or 'me' is not defined use 'so' and 'se'
	ಯದಿ (*T_ME == NUL)
	{
	    T_ME = T_SE;
	    T_MR = T_SO;
	    T_MD = T_SO;
	}

	// if 'so' or 'se' is not defined use 'mr' and 'me'
	ಯದಿ (*T_SO == NUL)
	{
	    T_SE = T_ME;
	    ಯದಿ (*T_MR == NUL)
		T_SO = T_MD;
	    ಅನ್ಯಥಾ
		T_SO = T_MR;
	}

	// if 'ZH' or 'ZR' is not defined use 'mr' and 'me'
	ಯದಿ (*T_CZH == NUL)
	{
	    T_CZR = T_ME;
	    ಯದಿ (*T_MR == NUL)
		T_CZH = T_MD;
	    ಅನ್ಯಥಾ
		T_CZH = T_MR;
	}

	// "Sb" and "Sf" come in pairs
	ಯದಿ (*T_CSB == NUL || *T_CSF == NUL)
	{
	    T_CSB = empty_option;
	    T_CSF = empty_option;
	}

	// "AB" and "AF" come in pairs
	ಯದಿ (*T_CAB == NUL || *T_CAF == NUL)
	{
	    T_CAB = empty_option;
	    T_CAF = empty_option;
	}

	// if 'Sb' and 'AB' are not defined, reset "Co"
	ಯದಿ (*T_CSB == NUL && *T_CAB == NUL)
	    free_one_termoption(T_CCO);

	// Set 'weirdinvert' according to value of 't_xs'
	p_wiv = (*T_XS != NUL);
    }
    need_gather = TRUE;

    // Set t_colors to the value of $COLORS or t_Co.  Ignore $COLORS in the
    // GUI.
    t_colors = atoi((ವರ್ಣ *)T_CCO);
#ifdef FEAT_GUI
    ಯದಿ (!gui.in_use)
#endif
    {
	env_colors = mch_getenv((char_u *)"COLORS");
	ಯದಿ (env_colors != ರಿಕ್ತ && SAFE_isdigit(*env_colors))
	{
	    ಪೂರ್ಣಾಙ್ಕ colors = atoi((ವರ್ಣ *)env_colors);

	    ಯದಿ (colors != t_colors)
		set_color_ಎಣಿಕೆ(colors);
	}
    }
    set_rgb_term_prop();
}

#ಯದಿ defined(FEAT_GUI) && (defined(FEAT_MENU) || !defined(USE_ON_FLY_SCROLL))
/*
 * Represent the given long_u as individual bytes, with the most significant
 * byte first, and store them in dst.
 */
    ಶೂನ್ಯ
add_long_to_buf(long_u val, char_u *dst)
{
    ಪೂರ್ಣಾಙ್ಕ	    i;
    ಪೂರ್ಣಾಙ್ಕ	    shift;

    ಪ್ರತಿ (i = 1; i <= (ಪೂರ್ಣಾಙ್ಕ)ಪರಿಮಾಣ(long_u); i++)
    {
	shift = 8 * (ಪರಿಮಾಣ(long_u) - i);
	dst[i - 1] = (char_u) ((val >> shift) & 0xff);
    }
}

/*
 * Interpret the next string of bytes in buf as a long integer, with the most
 * significant byte first.  Note that it is assumed that buf has been through
 * inchar(), so that NUL and K_SPECIAL will be represented as three bytes each.
 * Puts result in val, and returns the number of bytes read from buf
 * (between sizeof(long_u) and 2 * sizeof(long_u)), or -1 if not enough bytes
 * were present.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
get_long_from_buf(char_u *buf, long_u *val)
{
    ಪೂರ್ಣಾಙ್ಕ	    len;
    char_u  bytes[ಪರಿಮಾಣ(long_u)];
    ಪೂರ್ಣಾಙ್ಕ	    i;
    ಪೂರ್ಣಾಙ್ಕ	    shift;

    *val = 0;
    len = get_bytes_from_buf(buf, bytes, (ಪೂರ್ಣಾಙ್ಕ)ಪರಿಮಾಣ(long_u));
    ಯದಿ (len == -1)
	ಪ್ರತ್ಯಾಗಮ -1;
    ಪ್ರತಿ (i = 0; i < (ಪೂರ್ಣಾಙ್ಕ)ಪರಿಮಾಣ(long_u); i++)
    {
	shift = 8 * (ಪರಿಮಾಣ(long_u) - 1 - i);
	*val += (long_u)bytes[i] << shift;
    }
    ಪ್ರತ್ಯಾಗಮ len;
}
#endif

/*
 * Read the next num_bytes bytes from buf, and store them in bytes.  Assume
 * that buf has been through inchar().	Returns the actual number of bytes used
 * from buf (between num_bytes and num_bytes*2), or -1 if not enough bytes were
 * available.
 */
    ಪೂರ್ಣಾಙ್ಕ
get_bytes_from_buf(char_u *buf, char_u *bytes, ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_bytes)
{
    ಪೂರ್ಣಾಙ್ಕ	    len = 0;
    ಪೂರ್ಣಾಙ್ಕ	    i;
    char_u  c;

    ಪ್ರತಿ (i = 0; i < ಅಂಕೆ_bytes; i++)
    {
	ಯದಿ ((c = buf[len++]) == NUL)
	    ಪ್ರತ್ಯಾಗಮ -1;
	ಯದಿ (c == K_SPECIAL)
	{
	    ಯದಿ (buf[len] == NUL || buf[len + 1] == NUL)	    // cannot happen?
		ಪ್ರತ್ಯಾಗಮ -1;
	    ಯದಿ (buf[len++] == (ಪೂರ್ಣಾಙ್ಕ)KS_ZERO)
		c = NUL;
	    // else it should be KS_SPECIAL; when followed by KE_FILLER c is
	    // K_SPECIAL, or followed by KE_CSI and c must be CSI.
	    ಯದಿ (buf[len++] == (ಪೂರ್ಣಾಙ್ಕ)KE_CSI)
		c = CSI;
	}
	ಅನ್ಯಥಾ ಯದಿ (c == CSI && buf[len] == KS_EXTRA
					       && buf[len + 1] == (ಪೂರ್ಣಾಙ್ಕ)KE_CSI)
	    // CSI is stored as CSI KS_SPECIAL KE_CSI to avoid confusion with
	    // the start of a special key, see add_to_input_buf_csi().
	    len += 2;
	bytes[i] = c;
    }
    ಪ್ರತ್ಯಾಗಮ len;
}

/*
 * Check if the new shell size is valid, correct it if it's too small or way
 * too big.
 */
    ಶೂನ್ಯ
check_shellsize(ಶೂನ್ಯ)
{
    // need room for one window and command line
    ಯದಿ (Rows < min_rows_for_all_tabpages())
	Rows = min_rows_for_all_tabpages();
    limit_screen_size();

    // make sure these values are not invalid
    ಯದಿ (cmdline_row >= Rows)
	cmdline_row = Rows - 1;
    ಯದಿ (ಸಂದೇಶ_row >= Rows)
	ಸಂದೇಶ_row = Rows - 1;
}

/*
 * Limit Rows and Columns to avoid an overflow in Rows * Columns.
 */
    ಶೂನ್ಯ
limit_screen_size(ಶೂನ್ಯ)
{
    ಯದಿ (Columns < MIN_COLUMNS)
	Columns = MIN_COLUMNS;
    ಅನ್ಯಥಾ ಯದಿ (Columns > 10000)
	Columns = 10000;
    ಯದಿ (Rows > 1000)
	Rows = 1000;
}

/*
 * Invoked just before the screen structures are going to be (re)allocated.
 */
    ಶೂನ್ಯ
win_new_shellsize(ಶೂನ್ಯ)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	old_Rows = 0;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	old_Columns = 0;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	old_coloff = 0;

    ಯದಿ (old_Rows != Rows || old_Columns != COLUMNS_WITHOUT_TPL()
	    || old_coloff != TPL_LCOL())
	ui_new_shellsize();
    ಯದಿ (old_Columns != COLUMNS_WITHOUT_TPL() || old_coloff != TPL_LCOL())
    {
	old_Columns = COLUMNS_WITHOUT_TPL();
	old_coloff = TPL_LCOL();

	shell_new_columns();
    }
    ಯದಿ (old_Rows != Rows)
    {
	// If 'window' uses the whole screen, keep it using that.
	// Don't change it when set with "-w size" on the command line.
	ಯದಿ (p_window == old_Rows - 1
		    || (old_Rows == 0 && !option_was_set((char_u *)"window")))
	    p_window = Rows - 1;
	old_Rows = Rows;
	shell_new_rows();	// update window sizes
    }
}

#ಯದಿ defined(FEAT_IMAGE) || defined(FEAT_EVAL)
    ಶೂನ್ಯ
update_cell_size(ಶೂನ್ಯ)
{
# ifdef FEAT_GUI
    ಯದಿ (gui.in_use)
    {
	cell_width = gui.char_width;
	cell_height = gui.char_height;
    }
    ಅನ್ಯಥಾ
# endif
    {
	ಸಂರಚನಾ cellsize cell_sz;

	mch_calc_cell_size(&cell_sz);
	ಯದಿ (cell_sz.cs_xpixel <= 0 || cell_sz.cs_ypixel <= 0)
	{
	    cell_width = 8;
	    cell_height = 16;
	}
	ಅನ್ಯಥಾ
	{
	    cell_width = cell_sz.cs_xpixel;
	    cell_height = cell_sz.cs_ypixel;
	}
    }
}
#endif

/*
 * Call this function when the Vim shell has been resized in any way.
 * Will obtain the current size and redraw (also when size didn't change).
 */
    ಶೂನ್ಯ
shell_resized(ಶೂನ್ಯ)
{
#ಯದಿ defined(FEAT_IMAGE) || defined(FEAT_EVAL)
    update_cell_size();
#endif
    set_shellsize(0, 0, FALSE);
}

/*
 * Check if the shell size changed.  Handle a resize.
 * When the size didn't change, nothing happens.
 */
    ಶೂನ್ಯ
shell_resized_check(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		old_Rows = Rows;
    ಪೂರ್ಣಾಙ್ಕ		old_Columns = Columns;

    ಯದಿ (exiting
#ifdef FEAT_GUI
	    // Do not get the size when executing a shell command during
	    // startup.
	    || gui.starting
#endif
	    )
	ಪ್ರತ್ಯಾಗಮ;

    (ಶೂನ್ಯ)ui_get_shellsize();
    check_shellsize();
    ಯದಿ (old_Rows != Rows || old_Columns != Columns)
	shell_resized();
}

/*
 * Set size of the Vim shell.
 * If 'mustset' is TRUE, we must set Rows and Columns, do not get the real
 * window size (this is used for the :win command).
 * If 'mustset' is FALSE, we may try to get the real window size and if
 * it fails use 'width' and 'height'.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
set_shellsize_inner(ಪೂರ್ಣಾಙ್ಕ width, ಪೂರ್ಣಾಙ್ಕ height, ಪೂರ್ಣಾಙ್ಕ mustset)
{
    ಯದಿ (updating_screen)
	// resizing while in update_screen() may cause a crash
	ಪ್ರತ್ಯಾಗಮ;

    // curwin->w_buffer can be NULL when we are closing a window and the
    // buffer (or window) has already been closed and removing a scrollbar
    // causes a resize event. Don't resize then, it will happen after entering
    // another buffer.
    ಯದಿ (curwin->w_buffer == ರಿಕ್ತ || curwin->w_ಸಾಲುಗಳು == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

#ifdef AMIGA
    out_flush();	    // must do this before mch_get_shellsize() for
			    // some obscure reason
#endif

    ಯದಿ (mustset || (ui_get_shellsize() == FAIL && height != 0))
    {
	Rows = height;
	Columns = width;
	check_shellsize();
	ui_set_shellsize(mustset);
    }
    ಅನ್ಯಥಾ
	check_shellsize();

    // The window layout used to be adjusted here, but it now happens in
    // screenalloc() (also invoked from screenclear()).  That is because the
    // "busy" check above may skip this, but not screenalloc().

    ಯದಿ (State != MODE_ASKMORE && State != MODE_EXTERNCMD
						      && State != MODE_CONFIRM)
	screenclear();
    ಅನ್ಯಥಾ
	screen_start();	    // don't know where cursor is now

    ಯದಿ (starting != NO_SCREEN)
    {
	maketitle();

	changed_ಸಾಲು_abv_curs();
	invalidate_botline();

	/*
	 * We only redraw when it's needed:
	 * - While at the more prompt or executing an external command, don't
	 *   redraw, but position the cursor.
	 * - While editing the command line, only redraw that.
	 * - in Ex mode, don't redraw anything.
	 * - Otherwise, redraw right now, and position the cursor.
	 * Always need to call update_screen() or screenalloc(), to make
	 * sure Rows/Columns and the size of ScreenLines[] is correct!
	 */
	ಯದಿ (State == MODE_ASKMORE || State == MODE_EXTERNCMD
				     || State == MODE_CONFIRM || exmode_active)
	{
	    screenalloc(FALSE);
	    repeat_message();
	}
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (curwin->w_p_scb)
		do_check_scrollbind(TRUE);
	    ಯದಿ (State & MODE_CMDLINE)
	    {
		update_screen(UPD_NOT_VALID);
		redrawcmdline();
	    }
	    ಅನ್ಯಥಾ
	    {
		update_topline();
		ಯದಿ (pum_visible())
		{
		    redraw_later(UPD_NOT_VALID);
		    ins_compl_show_pum();
		}
		update_screen(UPD_NOT_VALID);
		ಯದಿ (redrawing())
		    setcursor();
	    }
	}
	cursor_on();	    // redrawing may have switched it off
    }
    out_flush();
}

    ಶೂನ್ಯ
set_shellsize(ಪೂರ್ಣಾಙ್ಕ width, ಪೂರ್ಣಾಙ್ಕ height, ಪೂರ್ಣಾಙ್ಕ mustset)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	busy = FALSE;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	do_run = FALSE;

    ಯದಿ (width < 0 || height < 0)    // just checking...
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (State == MODE_HITRETURN || State == MODE_SETWSIZE)
    {
	// postpone the resizing
	State = MODE_SETWSIZE;
	ಪ್ರತ್ಯಾಗಮ;
    }

    // Avoid recursiveness.  This can happen when setting the window size
    // causes another window-changed signal or when two SIGWINCH signals come
    // very close together.  There needs to be another run then after the
    // current one is done to pick up the latest size.
    do_run = TRUE;
    ಯದಿ (busy)
	ಪ್ರತ್ಯಾಗಮ;

    ಯಾವತ್ (do_run)
    {
	do_run = FALSE;
	busy = TRUE;
	set_shellsize_inner(width, height, mustset);
	busy = FALSE;
    }
}

/*
 * Output T_CTE, the t_TE termcap entry, and handle expected effects.
 * The code possibly disables modifyOtherKeys and the Kitty keyboard protocol.
 */
    ಶೂನ್ಯ
out_str_t_TE(ಶೂನ್ಯ)
{
    out_str(T_CTE);

    // The seenModifyOtherKeys flag is not reset here.  We do expect t_TE to
    // disable modifyOtherKeys, but until Xterm version 377 there is no way to
    // detect it's enabled again after the following t_TI.  We assume that when
    // seenModifyOtherKeys was set before it will still be valid.

    // When the modifyOtherKeys level is detected to be 2 we expect t_TE to
    // disable it.  Remembering that it was detected to be enabled is useful in
    // some situations.
    // The following t_TI is expected to request the state and then
    // modify_otherkeys_state will be set again.
    ಯದಿ (modify_otherkeys_state == MOKS_ENABLED
	    || modify_otherkeys_state == MOKS_DISABLED)
	modify_otherkeys_state = MOKS_DISABLED;
    ಅನ್ಯಥಾ ಯದಿ (modify_otherkeys_state != MOKS_INITIAL)
	modify_otherkeys_state = MOKS_AFTER_T_TE;

    // When the kitty keyboard protocol is enabled we expect t_TE to disable
    // it.  Remembering that it was detected to be enabled is useful in some
    // situations.
    // The following t_TI is expected to request the state and then
    // kitty_protocol_state will be set again.
    ಯದಿ (kitty_protocol_state == KKPS_ENABLED
	    || kitty_protocol_state == KKPS_DISABLED)
	kitty_protocol_state = KKPS_DISABLED;
    ಅನ್ಯಥಾ
	kitty_protocol_state = KKPS_AFTER_T_TE;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ send_t_RK = FALSE;

/*
 * Output T_TI and setup for what follows.
 */
    ಶೂನ್ಯ
out_str_t_TI(ಶೂನ್ಯ)
{
    out_str(T_CTI);

    // Send t_RK when there is no more work to do.
    send_t_RK = TRUE;
}

/*
 * Output T_BE, but only when t_PS and t_PE are set.
 */
    ಶೂನ್ಯ
out_str_t_BE(ಶೂನ್ಯ)
{
    char_u *p;

    ಯದಿ (T_BE == ರಿಕ್ತ || *T_BE == NUL
	    || (p = find_termcode((char_u *)"PS")) == ರಿಕ್ತ || *p == NUL
	    || (p = find_termcode((char_u *)"PE")) == ರಿಕ್ತ || *p == NUL)
	ಪ್ರತ್ಯಾಗಮ;
    out_str(T_BE);
}

/*
 * If t_TI was recently sent and there is no typeahead or work to do, now send
 * t_RK.  This is postponed to avoid the response arriving in a shell command
 * or after Vim exits.
 */
    ಶೂನ್ಯ
may_send_t_RK(ಶೂನ್ಯ)
{
    ಯದಿ (send_t_RK
	    && !work_pending()
	    && !ex_normal_busy
#ifdef FEAT_EVAL
	    && !in_feedkeys
#endif
	    && !exiting)
    {
	send_t_RK = FALSE;
	out_str(T_CRK);
	out_flush();
    }
}

/*
 * Set the terminal to TMODE_RAW (for Normal mode) or TMODE_COOK (for external
 * commands and Ex mode).
 */
    ಶೂನ್ಯ
settmode(tmode_T tmode)
{
#ifdef FEAT_GUI
    // don't set the term where gvim was started to any mode
    ಯದಿ (gui.in_use)
	ಪ್ರತ್ಯಾಗಮ;
#endif

    ಯದಿ (!full_screen)
	ಪ್ರತ್ಯಾಗಮ;

    /*
     * When returning after calling a shell cur_tmode is TMODE_UNKNOWN,
     * set the terminal to raw mode, even though we think it already is,
     * because the shell program may have reset the terminal mode.
     * When we think the terminal is normal, don't try to set it to
     * normal again, because that causes problems (logout!) on some
     * machines.
     */
    ಯದಿ (tmode != cur_tmode)
    {
#ifdef FEAT_TERMRESPONSE
# ifdef FEAT_GUI
	ಯದಿ (!gui.in_use && !gui.starting)
# endif
	{
	    // May need to check for T_CRV response and termcodes, it
	    // doesn't work in Cooked mode, an external program may get
	    // them.
	    ಯದಿ (tmode != TMODE_RAW && termrequest_any_pending())
		(ಶೂನ್ಯ)vpeekc_nomap();
	    check_for_codes_from_term();
	}
#endif
	ಯದಿ (tmode != TMODE_RAW)
	    mch_setmouse(FALSE);	// switch mouse off

	// Disable bracketed paste and modifyOtherKeys in cooked mode.
	// Avoid doing this too often, on some terminals the codes are not
	// handled properly.
	ಯದಿ (termcap_active && tmode != TMODE_SLEEP
		&& cur_tmode != TMODE_SLEEP)
	{
	    MAY_WANT_TO_LOG_THIS;

	    ಯದಿ (tmode != TMODE_RAW)
	    {
		out_str(T_BD);	// disable bracketed paste mode
		out_str_t_TE();	// possibly disables modifyOtherKeys
	    }
	    ಅನ್ಯಥಾ
	    {
		out_str_t_BE();	// enable bracketed paste mode (should
				// be before mch_settmode().
		out_str_t_TI();	// possibly enables modifyOtherKeys
	    }
	}
	out_flush();
	mch_settmode(tmode);	// machine specific function
	cur_tmode = tmode;
	ಯದಿ (tmode == TMODE_RAW)
	    setmouse();		// may switch mouse on
	out_flush();
    }
#ifdef FEAT_TERMRESPONSE
    may_req_termresponse();
#endif
}

    ಶೂನ್ಯ
starttermcap(ಶೂನ್ಯ)
{
    ಯದಿ (!full_screen || termcap_active)
	ಪ್ರತ್ಯಾಗಮ;

    MAY_WANT_TO_LOG_THIS;

    out_str(T_TI);			// start termcap mode
    out_str_t_TI();			// start "raw" mode
    out_str(T_KS);			// start "keypad transmit" mode
    out_str_t_BE();			// enable bracketed paste mode

#ಯದಿ defined(UNIX) || defined(VMS)
    // Enable xterm's focus reporting mode when 'esckeys' is set.
    ಯದಿ (p_ek && *T_FE != NUL)
	out_str(T_FE);
#endif

    out_flush();
    termcap_active = TRUE;
    screen_start();			// don't know where cursor is now
#ifdef FEAT_TERMRESPONSE
# ifdef FEAT_GUI
    ಯದಿ (!gui.in_use && !gui.starting)
# endif
    {
	may_req_termresponse();
	// Immediately check for a response.  If t_Co changes, we don't
	// want to redraw with wrong colors first.
	ಯದಿ (crv_status.tr_progress == STATUS_SENT)
	    check_for_codes_from_term();
    }
#endif
}

    ಶೂನ್ಯ
stoptermcap(ಶೂನ್ಯ)
{
    screen_stop_highlight();
    reset_cterm_colors();

    ಯದಿ (!termcap_active)
	ಪ್ರತ್ಯಾಗಮ;

#ifdef FEAT_TERMRESPONSE
# ifdef FEAT_GUI
    ಯದಿ (!gui.in_use && !gui.starting)
# endif
    {
	// May need to discard T_CRV, T_U7 or T_RBG response.
	ಯದಿ (termrequest_any_pending())
	{
# ifdef UNIX
	    // Give the terminal a chance to respond.
	    mch_delay(100L, 0);
# endif
# ifdef TCIFLUSH
	    // Discard data received but not read.
	    ಯದಿ (exiting)
		tcflush(fileno(stdin), TCIFLUSH);
# endif
	}
	// Check for termcodes first, otherwise an external program may
	// get them.
	check_for_codes_from_term();
    }
#endif
    MAY_WANT_TO_LOG_THIS;

#ಯದಿ defined(UNIX) || defined(VMS)
    // Disable xterm's focus reporting mode if 'esckeys' is set.
    ಯದಿ (p_ek && *T_FD != NUL)
	out_str(T_FD);
#endif

    out_str(T_BD);			// disable bracketed paste mode
    out_str(T_KE);			// stop "keypad transmit" mode
    out_flush();
    termcap_active = FALSE;

    // Output t_te before t_TE, t_te may switch between main and alternate
    // screen and following codes may work on the active screen only.
    //
    // When using the Kitty keyboard protocol the main and alternate screen
    // use a separate state.  If we are (or were) using the Kitty keyboard
    // protocol and t_te is not empty (possibly switching screens) then
    // output t_TE both before and after outputting t_te.
    ಯದಿ (*T_TE != NUL && (kitty_protocol_state == KKPS_ENABLED
		|| kitty_protocol_state == KKPS_DISABLED))
	out_str_t_TE();		// probably disables the kitty keyboard
				// protocol

    out_str(T_TE);			// stop termcap mode
    cursor_on();			// just in case it is still off
    out_str_t_TE();			// stop "raw" mode, modifyOtherKeys and
					// Kitty keyboard protocol
    screen_start();			// don't know where cursor is now
    out_flush();
}

#ಯದಿ defined(FEAT_TERMRESPONSE)
/*
 * Request version string (for xterm) when needed.
 * Only do this after switching to raw mode, otherwise the result will be
 * echoed.
 * Only do this after startup has finished, to avoid that the response comes
 * while executing "-c !cmd" or even after "-c quit".
 * Only do this after termcap mode has been started, otherwise the codes for
 * the cursor keys may be wrong.
 * Only do this when 'esckeys' is on, otherwise the response causes trouble in
 * Insert mode.
 * On Unix only do it when both output and input are a tty (avoid writing
 * request to terminal while reading from a file).
 * The result is caught in check_termcode().
 */
    ಶೂನ್ಯ
may_req_termresponse(ಶೂನ್ಯ)
{
    ಯದಿ (crv_status.tr_progress == STATUS_GET
	    && can_get_termresponse()
	    && starting == 0
	    && *T_CRV != NUL)
    {
	MAY_WANT_TO_LOG_THIS;
	LOG_TR1("Sending CRV request");
	out_str(T_CRV);
	termrequest_sent(&crv_status);
	// check for the characters now, otherwise they might be eaten by
	// get_keystroke()
	out_flush();
	(ಶೂನ್ಯ)vpeekc_nomap();
    }
}

/*
 * Send sequences to the terminal and check with t_u7 how the cursor moves, to
 * find out properties of the terminal.
 * Note that this goes out before T_CRV, so that the result can be used when
 * the termresponse arrives.
 */
    ಶೂನ್ಯ
check_terminal_behavior(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ	    did_send = FALSE;

    ಯದಿ (!can_get_termresponse() || starting != 0 || *T_U7 == NUL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (u7_status.tr_progress == STATUS_GET
	    && !option_was_set((char_u *)"ambiwidth"))
    {
	char_u	buf[16];

	// Ambiguous width check.
	// Check how the terminal treats ambiguous character width (UAX #11).
	// First, we move the cursor to (1, 0) and print a test ambiguous
	// character \u25bd (WHITE DOWN-POINTING TRIANGLE) and then query
	// the current cursor position.  If the terminal treats \u25bd as
	// single width, the position is (1, 1), or if it is treated as double
	// width, that will be (1, 2).  This function has the side effect that
	// changes cursor position, so it must be called immediately after
	// entering termcap mode.
	MAY_WANT_TO_LOG_THIS;
	LOG_TR1("Sending request for ambiwidth check");
	// Do this in the second row.  In the first row the returned sequence
	// may be CSI 1;2R, which is the same as <S-F3>.
	term_windgoto(1, 0);
	buf[mb_char2bytes(0x25bd, buf)] = NUL;
	out_str(buf);
	out_str(T_U7);
	termrequest_sent(&u7_status);
	out_flush();
	did_send = TRUE;

	// This overwrites a few characters on the screen, a redraw is needed
	// after this. Clear them out for now.
	screen_stop_highlight();
	term_windgoto(1, 0);
	out_str((char_u *)"  ");
	ಸಾಲು_was_clobbered(1);
    }

    ಯದಿ (xcc_status.tr_progress == STATUS_GET && Rows > 2)
    {
	// 2. Check compatibility with xterm.
	// We move the cursor to (2, 0), print a test sequence and then query
	// the current cursor position.  If the terminal properly handles
	// unknown DCS string and CSI sequence with intermediate byte, the test
	// sequence is ignored and the cursor does not move.  If the terminal
	// handles test sequence incorrectly, a garbage string is displayed and
	// the cursor does move.
	MAY_WANT_TO_LOG_THIS;
	LOG_TR1("Sending xterm compatibility test sequence.");
	// Do this in the third row.  Second row is used by ambiguous
	// character width check.
	term_windgoto(2, 0);
	// send the test DCS string.
	out_str((char_u *)"\033Pzz\033\\");
	// send the test CSI sequence with intermediate byte.
	out_str((char_u *)"\033[0%m");
	out_str(T_U7);
	termrequest_sent(&xcc_status);
	out_flush();
	did_send = TRUE;

	// If the terminal handles test sequence incorrectly, garbage text is
	// displayed. Clear them out for now.
	screen_stop_highlight();
	term_windgoto(2, 0);
	out_str((char_u *)"           ");
	ಸಾಲು_was_clobbered(2);
    }

    ಯದಿ (did_send)
    {
	term_windgoto(0, 0);

	// Need to reset the known cursor position.
	screen_start();

	// check for the characters now, otherwise they might be eaten by
	// get_keystroke()
	out_flush();
	(ಶೂನ್ಯ)vpeekc_nomap();
    }
}

/*
 * Similar to requesting the version string: Request the terminal background
 * color when it is the right moment.
 */
    ಶೂನ್ಯ
may_req_bg_color(ಶೂನ್ಯ)
{
    ಯದಿ (can_get_termresponse() && starting == 0)
    {
	ಪೂರ್ಣಾಙ್ಕ didit = FALSE;

# ifdef FEAT_TERMINAL
	// Only request foreground if t_RF is set.
	ಯದಿ (rfg_status.tr_progress == STATUS_GET && *T_RFG != NUL)
	{
	    MAY_WANT_TO_LOG_THIS;
	    LOG_TR1("Sending FG request");
	    out_str(T_RFG);
	    termrequest_sent(&rfg_status);
	    didit = TRUE;
	}
# endif

	// Only request background if t_RB is set.
	ಯದಿ (rbg_status.tr_progress == STATUS_GET && *T_RBG != NUL)
	{
	    MAY_WANT_TO_LOG_THIS;
	    LOG_TR1("Sending BG request");
	    out_str(T_RBG);
	    termrequest_sent(&rbg_status);
	    didit = TRUE;
	}

	ಯದಿ (didit)
	{
	    // check for the characters now, otherwise they might be eaten by
	    // get_keystroke()
	    out_flush();
	    (ಶೂನ್ಯ)vpeekc_nomap();
	}
    }
}

#endif

/*
 * Return TRUE when saving and restoring the screen.
 */
    ಪೂರ್ಣಾಙ್ಕ
swapping_screen(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ (full_screen && *T_TI != NUL);
}

/*
 * By outputting the 'cursor very visible' termcap code, for some windowed
 * terminals this makes the screen scrolled to the correct position.
 * Used when starting Vim or returning from a shell.
 */
    ಶೂನ್ಯ
scroll_start(ಶೂನ್ಯ)
{
    ಯದಿ (*T_VS == NUL || *T_CVS == NUL)
	ಪ್ರತ್ಯಾಗಮ;

    MAY_WANT_TO_LOG_THIS;
    out_str(T_VS);
    out_str(T_CVS);
    screen_start();		// don't know where cursor is now
}

// True if cursor is not visible
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ cursor_is_off = FALSE;

// True if cursor is not visible due to an ongoing cursor-less sleep
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ cursor_is_asleep = FALSE;

/*
 * Enable the cursor without checking if it's already enabled.
 */
    ಶೂನ್ಯ
cursor_on_force(ಶೂನ್ಯ)
{
    out_str(T_VE);
    cursor_is_off = FALSE;
    cursor_is_asleep = FALSE;
}

/*
 * Enable the cursor if it's currently off.
 */
    ಶೂನ್ಯ
cursor_on(ಶೂನ್ಯ)
{
    ಯದಿ (cursor_is_off && !cursor_is_asleep)
	cursor_on_force();
}

/*
 * Disable the cursor.
 */
    ಶೂನ್ಯ
cursor_off(ಶೂನ್ಯ)
{
    ಯದಿ (full_screen && !cursor_is_off)
    {
	out_str(T_VI);	    // disable cursor
	cursor_is_off = TRUE;
    }
}

#ifdef FEAT_GUI
/*
 * Check whether the cursor is invisible due to an ongoing cursor-less sleep
 */
    ಪೂರ್ಣಾಙ್ಕ
cursor_is_sleeping(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ cursor_is_asleep;
}
#endif

/*
 * Disable the cursor and mark it disabled by cursor-less sleep
 */
    ಶೂನ್ಯ
cursor_sleep(ಶೂನ್ಯ)
{
    cursor_is_asleep = TRUE;
    cursor_off();
}

/*
 * Enable the cursor and mark it not disabled by cursor-less sleep
 */
    ಶೂನ್ಯ
cursor_unsleep(ಶೂನ್ಯ)
{
    cursor_is_asleep = FALSE;
    cursor_on();
}

#ಯದಿ defined(CURSOR_SHAPE)
/*
 * Set cursor shape to match Insert or Replace mode.
 */
    ಶೂನ್ಯ
term_cursor_mode(ಪೂರ್ಣಾಙ್ಕ forced)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ showing_mode = -1;
    char_u *p;

    // Only do something when redrawing the screen and we can restore the
    // mode.
    ಯದಿ (!full_screen || *T_CEI == NUL)
    {
# ifdef FEAT_TERMRESPONSE
	ಯದಿ (forced && initial_cursor_shape > 0)
	    // Restore to initial values.
	    term_cursor_shape(initial_cursor_shape, initial_cursor_blink);
# endif
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ ((State & MODE_REPLACE) == MODE_REPLACE)
    {
	ಯದಿ (forced || showing_mode != MODE_REPLACE)
	{
	    ಯದಿ (*T_CSR != NUL)
		p = T_CSR;	// Replace mode cursor
	    ಅನ್ಯಥಾ
		p = T_CSI;	// fall back to Insert mode cursor
	    ಯದಿ (*p != NUL)
	    {
		out_str(p);
		showing_mode = MODE_REPLACE;
	    }
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (State & MODE_ಸೇರಿಸು)
    {
	ಯದಿ ((forced || showing_mode != MODE_ಸೇರಿಸು) && *T_CSI != NUL)
	{
	    out_str(T_CSI);	    // Insert mode cursor
	    showing_mode = MODE_ಸೇರಿಸು;
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (forced || showing_mode != MODE_NORMAL)
    {
	out_str(T_CEI);		    // non-Insert mode cursor
	showing_mode = MODE_NORMAL;
    }
}

# ಯದಿ defined(FEAT_TERMINAL)
    ಶೂನ್ಯ
term_cursor_color(char_u *color)
{
    ಯದಿ (*T_CSC == NUL)
	ಪ್ರತ್ಯಾಗಮ;

    out_str(T_CSC);		// set cursor color start
    out_str_nf(color);
    out_str(T_CEC);		// set cursor color end
    out_flush();
}
# endif

    ಪೂರ್ಣಾಙ್ಕ
blink_state_is_inverted(ಶೂನ್ಯ)
{
# ifdef FEAT_TERMRESPONSE
    ಪ್ರತ್ಯಾಗಮ rbm_status.tr_progress == STATUS_GOT
	&& rcs_status.tr_progress == STATUS_GOT
		&& initial_cursor_blink != initial_cursor_shape_blink;
# ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ FALSE;
# endif
}

/*
 * "shape": 1 = block, 2 = underline, 3 = vertical bar
 */
    ಶೂನ್ಯ
term_cursor_shape(ಪೂರ್ಣಾಙ್ಕ shape, ಪೂರ್ಣಾಙ್ಕ blink)
{
    ಯದಿ (*T_CSH != NUL)
    {
	OUT_STR(tgoto((ವರ್ಣ *)T_CSH, 0, shape * 2 - blink));
	out_flush();
    }
    ಅನ್ಯಥಾ
    {
	ಪೂರ್ಣಾಙ್ಕ do_blink = blink;

	// t_SH is empty: try setting just the blink state.
	// The blink flags are XORed together, if the initial blinking from
	// style and shape differs, we need to invert the flag here.
	ಯದಿ (blink_state_is_inverted())
	    do_blink = !blink;

	ಯದಿ (do_blink && *T_VS != NUL)
	{
	    out_str(T_VS);
	    out_flush();
	}
	ಅನ್ಯಥಾ ಯದಿ (!do_blink && *T_CVS != NUL)
	{
	    out_str(T_CVS);
	    out_flush();
	}
    }
}
#endif

/*
 * Set scrolling region for window 'wp'.
 * The region starts 'off' lines from the start of the window.
 * Also set the vertical scroll region for a vertically split window.  Always
 * the full width of the window, excluding the vertical separator.
 */
    ಶೂನ್ಯ
scroll_region_set(win_T *wp, ಪೂರ್ಣಾಙ್ಕ off)
{
    OUT_STR(tgoto((ವರ್ಣ *)T_CS, W_WINROW(wp) + wp->w_height - 1,
							 W_WINROW(wp) + off));
    ಯದಿ (*T_CSV != NUL && wp->w_width != Columns)
	OUT_STR(tgoto((ವರ್ಣ *)T_CSV, wp->w_wincol + wp->w_width - 1,
							       wp->w_wincol));
    screen_start();		    // don't know where cursor is now
}

/*
 * Reset scrolling region to the whole screen.
 */
    ಶೂನ್ಯ
scroll_region_reset(ಶೂನ್ಯ)
{
    OUT_STR(tgoto((ವರ್ಣ *)T_CS, (ಪೂರ್ಣಾಙ್ಕ)Rows - 1, 0));
    ಯದಿ (*T_CSV != NUL)
	OUT_STR(tgoto((ವರ್ಣ *)T_CSV, (ಪೂರ್ಣಾಙ್ಕ)Columns - 1, 0));
    screen_start();		    // don't know where cursor is now
}


/*
 * List of terminal codes that are currently recognized.
 */

ಸ್ಥಿರ ಸಂರಚನಾ termcode
{
    char_u  name[2];	    // termcap name of entry
    char_u  *code;	    // terminal code (in allocated memory)
    ಪೂರ್ಣಾಙ್ಕ	    len;	    // STRLEN(code)
    ಪೂರ್ಣಾಙ್ಕ	    modlen;	    // length of part before ";*~".
} *termcodes = ರಿಕ್ತ;

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ  tc_max_len = 0; // number of entries that termcodes[] can hold
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ  tc_len = 0;	    // current number of entries in termcodes[]

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ termcode_star(char_u *code, ಪೂರ್ಣಾಙ್ಕ len);

    ಶೂನ್ಯ
clear_termcodes(ಶೂನ್ಯ)
{
    ಯಾವತ್ (tc_len > 0)
	vim_free(termcodes[--tc_len].code);
    VIM_CLEAR(termcodes);
    tc_max_len = 0;

#ifdef HAVE_TGETENT
    BC = (ವರ್ಣ *)empty_option;
    UP = (ವರ್ಣ *)empty_option;
    PC = NUL;			// set pad character to NUL
    ospeed = 0;
#endif

    need_gather = TRUE;		// need to fill termleader[]
}

#define ATC_FROM_TERM 55

/*
 * For xterm we recognize special codes like "ESC[42;*X" and "ESC O*X" that
 * accept modifiers.
 * Set "termcodes[idx].modlen".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
adjust_modlen(ಪೂರ್ಣಾಙ್ಕ idx)
{
    termcodes[idx].modlen = 0;
    ಪೂರ್ಣಾಙ್ಕ j = termcode_star(termcodes[idx].code, termcodes[idx].len);
    ಯದಿ (j <= 0)
	ಪ್ರತ್ಯಾಗಮ;

    termcodes[idx].modlen = termcodes[idx].len - 1 - j;
    // For "CSI[@;X" the "@" is not included in "modlen".
    ಯದಿ (termcodes[idx].code[termcodes[idx].modlen - 1] == '@')
	--termcodes[idx].modlen;
}

/*
 * Add a new entry for "name[2]" to the list of terminal codes.
 * Note that "name" may not have a terminating NUL.
 * The list is kept alphabetical for ":set termcap"
 * "flags" is TRUE when replacing 7-bit by 8-bit controls is desired.
 * "flags" can also be ATC_FROM_TERM for got_code_from_term().
 */
    ಶೂನ್ಯ
add_termcode(char_u *name, char_u *string, ಪೂರ್ಣಾಙ್ಕ flags)
{
    ಸಂರಚನಾ termcode *new_tc;
    ಪೂರ್ಣಾಙ್ಕ		    i, j;
    char_u	    *s;
    ಪೂರ್ಣಾಙ್ಕ		    len;
#ifdef FEAT_EVAL
    ವರ್ಣ	    *action = "Setting";
#endif

    ಯದಿ (string == ರಿಕ್ತ || *string == NUL)
    {
	del_termcode(name);
	ಪ್ರತ್ಯಾಗಮ;
    }

#ಯದಿ defined(MSWIN) && !defined(FEAT_GUI)
    s = vim_strnsave(string, STRLEN(string) + 1);
#ಅನ್ಯಥಾ
# ifdef VIMDLL
    ಯದಿ (!gui.in_use)
	s = vim_strnsave(string, STRLEN(string) + 1);
    ಅನ್ಯಥಾ
# endif
	s = vim_strsave(string);
#endif
    ಯದಿ (s == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    // Change leading <Esc>[ to CSI, change <Esc>O to <M-O>.
    ಯದಿ (flags != 0 && flags != ATC_FROM_TERM && term_7to8bit(string) != 0)
    {
	STRMOVE(s, s + 1);
	s[0] = term_7to8bit(string);
    }

#ಯದಿ defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
# ifdef VIMDLL
    ಯದಿ (!gui.in_use)
# endif
    {
	ಯದಿ (s[0] == K_NUL)
	{
	    STRMOVE(s + 1, s);
	    s[1] = 3;
	}
    }
#endif

    len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(s);

    need_gather = TRUE;		// need to fill termleader[]

    /*
     * need to make space for more entries
     */
    ಯದಿ (tc_len == tc_max_len)
    {
	tc_max_len += 20;
	new_tc = ALLOC_MULT(ಸಂರಚನಾ termcode, tc_max_len);
	ಯದಿ (new_tc == ರಿಕ್ತ)
	{
	    tc_max_len -= 20;
	    vim_free(s);
	    ಪ್ರತ್ಯಾಗಮ;
	}
	ಪ್ರತಿ (i = 0; i < tc_len; ++i)
	    new_tc[i] = termcodes[i];
	vim_free(termcodes);
	termcodes = new_tc;
    }

    /*
     * Look for existing entry with the same name, it is replaced.
     * Look for an existing entry that is alphabetical higher, the new entry
     * is inserted in front of it.
     */
    ಪ್ರತಿ (i = 0; i < tc_len; ++i)
    {
	ಯದಿ (termcodes[i].name[0] < name[0])
	    ಅನುವರ್ತ;
	ಯದಿ (termcodes[i].name[0] == name[0])
	{
	    ಯದಿ (termcodes[i].name[1] < name[1])
		ಅನುವರ್ತ;
	    /*
	     * Exact match: May replace old code.
	     */
	    ಯದಿ (termcodes[i].name[1] == name[1])
	    {
		ಯದಿ (flags == ATC_FROM_TERM && (j = termcode_star(
				    termcodes[i].code, termcodes[i].len)) > 0)
		{
		    // Don't replace ESC[123;*X or ESC O*X with another when
		    // invoked from got_code_from_term().
		    ಯದಿ (len == termcodes[i].len - j
			    && STRNCMP(s, termcodes[i].code, len - 1) == 0
			    && s[len - 1]
				   == termcodes[i].code[termcodes[i].len - 1])
		    {
			// They are equal but for the ";*": don't add it.
#ifdef FEAT_EVAL
			ch_log(ರಿಕ್ತ, "Termcap entry %c%c did not change",
							     name[0], name[1]);
#endif
			vim_free(s);
			ಪ್ರತ್ಯಾಗಮ;
		    }
		}
		ಅನ್ಯಥಾ
		{
		    // Replace old code.
#ifdef FEAT_EVAL
		    ch_log(ರಿಕ್ತ, "Termcap entry %c%c was: %s",
					  name[0], name[1], termcodes[i].code);
#endif
		    vim_free(termcodes[i].code);
		    --tc_len;
		    ಭಙ್ಗ;
		}
	    }
	}
	/*
	 * Found alphabetical larger entry, move rest to insert new entry
	 */
#ifdef FEAT_EVAL
	action = "Adding";
#endif
	ಪ್ರತಿ (j = tc_len; j > i; --j)
	    termcodes[j] = termcodes[j - 1];
	ಭಙ್ಗ;
    }

#ifdef FEAT_EVAL
    ch_log(ರಿಕ್ತ, "%s termcap entry %c%c to %s", action, name[0], name[1], s);
#endif
    termcodes[i].name[0] = name[0];
    termcodes[i].name[1] = name[1];
    termcodes[i].code = s;
    termcodes[i].len = len;
    adjust_modlen(i);

    ++tc_len;
}

/*
 * Some function keys may include modifiers, but the terminfo/termcap entries
 * do not indicate that.  Insert ";*" where we expect modifiers might appear.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
accept_modifiers_for_function_ಕೀಲಿಗಳು(ಶೂನ್ಯ)
{
    regmatch_T regmatch;
    CLEAR_FIELD(regmatch);
    regmatch.rm_ic = TRUE;
    regmatch.regprog = vim_regcomp((char_u *)"^\033[\\d\\+\\~$", RE_MAGIC);

    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < tc_len; ++i)
    {
	ಯದಿ (regmatch.regprog == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ;

	// skip PasteStart and PasteEnd
	ಯದಿ (termcodes[i].name[0] == 'P'
		&& (termcodes[i].name[1] == 'S' || termcodes[i].name[1] == 'E'))
	    ಅನುವರ್ತ;

	char_u *s = termcodes[i].code;
	ಯದಿ (s != ರಿಕ್ತ && vim_regexec(&regmatch, s, (colnr_T)0))
	{
	    size_t len = STRLEN(s);
	    char_u *ns = alloc(len + 3);
	    ಯದಿ (ns != ರಿಕ್ತ)
	    {
		mch_memmove(ns, s, len - 1);
		mch_memmove(ns + len - 1, ";*~", 4);
		vim_free(s);
		termcodes[i].code = ns;
		termcodes[i].len += 2;
		adjust_modlen(i);
	    }
	}
    }

    vim_regfree(regmatch.regprog);
}

/*
 * Check termcode "code[len]" for ending in ;*X or *X.
 * The "X" can be any character.
 * Return 0 if not found, 2 for ;*X and 1 for *X.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
termcode_star(char_u *code, ಪೂರ್ಣಾಙ್ಕ len)
{
    // Shortest is <M-O>*X.  With ; shortest is <CSI>@;*X
    ಯದಿ (len >= 3 && code[len - 2] == '*')
    {
	ಯದಿ (len >= 5 && code[len - 3] == ';')
	    ಪ್ರತ್ಯಾಗಮ 2;
	ಅನ್ಯಥಾ
	    ಪ್ರತ್ಯಾಗಮ 1;
    }
    ಪ್ರತ್ಯಾಗಮ 0;
}

    char_u  *
find_termcode(char_u *name)
{
    ಪೂರ್ಣಾಙ್ಕ	    i;

    ಪ್ರತಿ (i = 0; i < tc_len; ++i)
	ಯದಿ (termcodes[i].name[0] == name[0] && termcodes[i].name[1] == name[1])
	    ಪ್ರತ್ಯಾಗಮ termcodes[i].code;
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

    char_u *
get_termcode(ಪೂರ್ಣಾಙ್ಕ i)
{
    ಯದಿ (i >= tc_len)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ಪ್ರತ್ಯಾಗಮ &termcodes[i].name[0];
}

/*
 * Returns the length of the terminal code at index 'idx'.
 */
    ಪೂರ್ಣಾಙ್ಕ
get_termcode_len(ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಪ್ರತ್ಯಾಗಮ termcodes[idx].len;
}

    ಶೂನ್ಯ
del_termcode(char_u *name)
{
    ಪೂರ್ಣಾಙ್ಕ	    i;

    ಯದಿ (termcodes == ರಿಕ್ತ)	// nothing there yet
	ಪ್ರತ್ಯಾಗಮ;

    need_gather = TRUE;		// need to fill termleader[]

    ಪ್ರತಿ (i = 0; i < tc_len; ++i)
	ಯದಿ (termcodes[i].name[0] == name[0] && termcodes[i].name[1] == name[1])
	{
	    del_termcode_idx(i);
	    ಪ್ರತ್ಯಾಗಮ;
	}
    // not found. Give error message?
}

    ಸ್ಥಿರ ಶೂನ್ಯ
del_termcode_idx(ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಪೂರ್ಣಾಙ್ಕ		i;

    vim_free(termcodes[idx].code);
    --tc_len;
    ಪ್ರತಿ (i = idx; i < tc_len; ++i)
	termcodes[i] = termcodes[i + 1];
}

/*
 * Called when detected that the terminal sends 8-bit codes.
 * Convert all 7-bit codes to their 8-bit equivalent.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
switch_to_8bit(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		c;

    // Only need to do something when not already using 8-bit codes.
    ಯದಿ (!term_is_8bit(T_NAME))
    {
	ಪ್ರತಿ (i = 0; i < tc_len; ++i)
	{
	    c = term_7to8bit(termcodes[i].code);
	    ಯದಿ (c != 0)
	    {
		STRMOVE(termcodes[i].code + 1, termcodes[i].code + 2);
		termcodes[i].code[0] = c;
	    }
	}
	need_gather = TRUE;		// need to fill termleader[]
    }
    detected_8bit = TRUE;
    LOG_TR1("Switching to 8 bit");
}

#ifdef CHECK_DOUBLE_CLICK
ಸ್ಥಿರ linenr_T orig_topline = 0;
# ifdef FEAT_DIFF
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ orig_topfill = 0;
# endif
#endif
#ಯದಿ defined(CHECK_DOUBLE_CLICK)
/*
 * Checking for double-clicks ourselves.
 * "orig_topline" is used to avoid detecting a double-click when the window
 * contents scrolled (e.g., when 'scrolloff' is non-zero).
 */
/*
 * Set orig_topline.  Used when jumping to another window, so that a double
 * click still works.
 */
    ಶೂನ್ಯ
set_mouse_topline(win_T *wp)
{
    orig_topline = wp->w_topline;
# ifdef FEAT_DIFF
    orig_topfill = wp->w_topfill;
# endif
}

/*
 * Returns TRUE if the top line and top fill of window 'wp' matches the saved
 * topline and topfill.
 */
    ಪೂರ್ಣಾಙ್ಕ
is_mouse_topline(win_T *wp)
{
    ಪ್ರತ್ಯಾಗಮ orig_topline == wp->w_topline
# ifdef FEAT_DIFF
	&& orig_topfill == wp->w_topfill
# endif
	;
}
#endif

/*
 * If "buf" is NULL put "string[new_slen]" in typebuf; "buflen" is not used.
 * If "buf" is not NULL put "string[new_slen]" in "buf[bufsize]" and adjust
 * "buflen".
 * Remove "slen" bytes.
 * Returns FAIL for error.
 */
    ಪೂರ್ಣಾಙ್ಕ
put_string_in_typebuf(
	ಪೂರ್ಣಾಙ್ಕ	offset,
	ಪೂರ್ಣಾಙ್ಕ	slen,
	char_u	*string,
	ಪೂರ್ಣಾಙ್ಕ	new_slen,
	char_u	*buf,
	ಪೂರ್ಣಾಙ್ಕ	bufsize,
	ಪೂರ್ಣಾಙ್ಕ	*buflen)
{
    ಪೂರ್ಣಾಙ್ಕ		extra = new_slen - slen;

    string[new_slen] = NUL;
    ಯದಿ (buf == ರಿಕ್ತ)
    {
	ಯದಿ (extra < 0)
	    // remove matched chars, taking care of noremap
	    del_typebuf(-extra, offset);
	ಅನ್ಯಥಾ ಯದಿ (extra > 0)
	    // insert the extra space we need
	    ಯದಿ (ins_typebuf(string + slen, REMAP_YES, offset, FALSE, FALSE)
								       == FAIL)
		ಪ್ರತ್ಯಾಗಮ FAIL;

	// Careful: del_typebuf() and ins_typebuf() may have reallocated
	// typebuf.tb_buf[]!
	mch_memmove(typebuf.tb_buf + typebuf.tb_off + offset, string,
							     (size_t)new_slen);
    }
    ಅನ್ಯಥಾ
    {
	ಯದಿ (extra < 0)
	    // remove matched characters
	    mch_memmove(buf + offset, buf + offset - extra,
					   (size_t)(*buflen + offset + extra));
	ಅನ್ಯಥಾ ಯದಿ (extra > 0)
	{
	    // Insert the extra space we need.  If there is insufficient
	    // space return -1.
	    ಯದಿ (*buflen + extra + new_slen >= bufsize)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    mch_memmove(buf + offset + extra, buf + offset,
						   (size_t)(*buflen - offset));
	}
	mch_memmove(buf + offset, string, (size_t)new_slen);
	*buflen = *buflen + extra + new_slen;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Decode a modifier number as xterm provides it into MOD_MASK bits.
 */
    ಪೂರ್ಣಾಙ್ಕ
decode_modifiers(ಪೂರ್ಣಾಙ್ಕ n)
{
    ಪೂರ್ಣಾಙ್ಕ	    code = n - 1;
    ಪೂರ್ಣಾಙ್ಕ	    modifiers = 0;

    ಯದಿ (code & 1)
	modifiers |= MOD_MASK_SHIFT;
    ಯದಿ (code & 2)
	modifiers |= MOD_MASK_ALT;
    ಯದಿ (code & 4)
	modifiers |= MOD_MASK_CTRL;
    ಯದಿ (code & 8)
	modifiers |= MOD_MASK_META;
    // Any further modifiers are silently dropped.

    ಪ್ರತ್ಯಾಗಮ modifiers;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
modifiers2keycode(ಪೂರ್ಣಾಙ್ಕ modifiers, ಪೂರ್ಣಾಙ್ಕ *ಕೀಲಿ, char_u *string)
{
    ಪೂರ್ಣಾಙ್ಕ new_slen = 0;

    ಯದಿ (modifiers == 0)
	ಪ್ರತ್ಯಾಗಮ 0;

    // Some keys have the modifier included.  Need to handle that here to
    // make mappings work.  This may result in a special key, such as
    // K_S_TAB.
    *ಕೀಲಿ = simplify_ಕೀಲಿ(*ಕೀಲಿ, &modifiers);
    ಯದಿ (modifiers != 0)
    {
	string[new_slen++] = K_SPECIAL;
	string[new_slen++] = (ಪೂರ್ಣಾಙ್ಕ)KS_MODIFIER;
	string[new_slen++] = modifiers;
    }
    ಪ್ರತ್ಯಾಗಮ new_slen;
}

/*
 * Handle a cursor position report.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
handle_u7_response(ಪೂರ್ಣಾಙ್ಕ *arg, char_u *tp UNUSED, ಪೂರ್ಣಾಙ್ಕ csi_len UNUSED)
{
    ಯದಿ (arg[0] == 2 && arg[1] >= 2)
    {
	ವರ್ಣ *aw = ರಿಕ್ತ;

	LOG_TRN("Received U7 status: %s", tp);
	u7_status.tr_progress = STATUS_GOT;
	did_cursorhold = TRUE;
	ಯದಿ (arg[1] == 2)
	    aw = "single";
	ಅನ್ಯಥಾ ಯದಿ (arg[1] == 3)
	    aw = "double";
	ಯದಿ (aw != ರಿಕ್ತ && STRCMP(aw, p_ambw) != 0)
	{
	    // Setting the option causes a screen redraw. Do
	    // that right away if possible, keeping any
	    // messages.
	    set_option_ಮೌಲ್ಯ_give_err((char_u *)"ambw", 0L, (char_u *)aw, 0);
#ifdef DEBUG_TERMRESPONSE
	    {
		ಪೂರ್ಣಾಙ್ಕ r = redraw_asap(UPD_CLEAR);

		LOG_TRN("set 'ambiwidth', redraw_asap(): %d", r);
	    }
#ಅನ್ಯಥಾ
	    redraw_asap(UPD_CLEAR);
#endif
#ifdef FEAT_EVAL
	    set_vim_var_string(VV_TERMU7RESP, tp, csi_len);
#endif
	    apply_autocmds(EVENT_TERMRESPONSEALL,
					(char_u *)"ambiguouswidth", ರಿಕ್ತ, FALSE, curbuf);
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (arg[0] == 3)
    {
	ಪೂರ್ಣಾಙ್ಕ ಮೌಲ್ಯ;

	LOG_TRN("Received compatibility test result: %s", tp);
	xcc_status.tr_progress = STATUS_GOT;

	// Third row: xterm compatibility test.
	// If the cursor is on the first column then the terminal can handle
	// the request for cursor style and blinking.
	ಮೌಲ್ಯ = arg[1] == 1 ? TPR_YES : TPR_NO;
	term_props[TPR_CURSOR_STYLE].tpr_status = ಮೌಲ್ಯ;
	term_props[TPR_CURSOR_BLINK].tpr_status = ಮೌಲ್ಯ;
    }
}



/*
 * Handle a response to T_CRV: {lead}{first}{x};{vers};{y}c
 * Xterm and alike use '>' for {first}.
 * Rxvt sends "{lead}?1;2c".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
handle_version_response(ಪೂರ್ಣಾಙ್ಕ first, ಪೂರ್ಣಾಙ್ಕ *arg, ಪೂರ್ಣಾಙ್ಕ argc, char_u *tp)
{
    // The xterm version.  It is set to zero when it can't be an actual xterm
    // version.
    ಪೂರ್ಣಾಙ್ಕ version = arg[1];

    LOG_TRN("Received CRV response: %s", tp);
    crv_status.tr_progress = STATUS_GOT;
    did_cursorhold = TRUE;

    // Reset terminal properties that are set based on the termresponse.
    // Mainly useful for tests that send the termresponse multiple times.
    // For testing all props can be reset.
    init_term_props(
#ifdef FEAT_EVAL
	    reset_term_props_on_termresponse
#ಅನ್ಯಥಾ
	    FALSE
#endif
	    );

    // If this code starts with CSI, you can bet that the
    // terminal uses 8-bit codes.
    ಯದಿ (tp[0] == CSI)
	switch_to_8bit();

    // Screen sends 40500.
    // rxvt sends its version number: "20703" is 2.7.3.
    // Ignore it for when the user has set 'term' to xterm,
    // even though it's an rxvt.
    ಯದಿ (version > 20000)
	version = 0;

    // Figure out more if the response is CSI > 99 ; 99 ; 99 c
    ಯದಿ (first == '>' && argc == 3)
    {
	// mintty 2.9.5 sends 77;20905;0c.
	// (77 is ASCII 'M' for mintty.)
	ಯದಿ (arg[0] == 77)
	{
	    // mintty can do SGR mouse reporting
	    term_props[TPR_MOUSE].tpr_status = TPR_MOUSE_SGR;
	}

#ifdef FEAT_TERMRESPONSE
	// If xterm version >= 141 try to get termcap codes.  For other
	// terminals the request should be ignored.
	ಯದಿ (version >= 141 && p_xtermcodes)
	{
	    LOG_TR1("Enable checking for XT codes");
	    check_for_codes = TRUE;
	    need_gather = TRUE;
	    req_codes_from_term();
	}
#endif

	// libvterm sends 0;100;0
	// Konsole sends 0;115;0 and works the same way
	ಯದಿ ((version == 100 || version == 115) && arg[0] == 0 && arg[2] == 0)
	{
	    // If run from Vim $COLORS is set to the number of
	    // colors the terminal supports.  Otherwise assume
	    // 256, libvterm supports even more.
	    ಯದಿ (mch_getenv((char_u *)"COLORS") == ರಿಕ್ತ)
		may_adjust_color_ಎಣಿಕೆ(256);
	    // Libvterm can handle SGR mouse reporting.
	    term_props[TPR_MOUSE].tpr_status = TPR_MOUSE_SGR;
	}

	ಯದಿ (version == 95)
	{
	    // Mac Terminal.app sends 1;95;0
	    //
	    // Terminal.app doesn't seem to handle DECRQM sequences
	    // properly, see issue #19852.
	    ಯದಿ (arg[0] == 1 && arg[2] == 0)
	    {
		term_props[TPR_UNDERLINE_RGB].tpr_status = TPR_YES;
		term_props[TPR_MOUSE].tpr_status = TPR_MOUSE_SGR;
		term_props[TPR_DECRQM].tpr_status = TPR_NO;
	    }
	    // iTerm2 sends 0;95;0
	    ಅನ್ಯಥಾ ಯದಿ (arg[0] == 0 && arg[2] == 0)
	    {
		// iTerm2 can do SGR mouse reporting
		term_props[TPR_MOUSE].tpr_status = TPR_MOUSE_SGR;
		term_props[TPR_DECRQM].tpr_status = TPR_YES;
	    }
	    // old iTerm2 sends 0;95;
	    ಅನ್ಯಥಾ ಯದಿ (arg[0] == 0 && arg[2] == -1)
		term_props[TPR_UNDERLINE_RGB].tpr_status = TPR_YES;
	}

	// screen sends 83;40500;0 83 is 'S' in ASCII.
	ಯದಿ (arg[0] == 83)
	{
	    // screen supports SGR mouse codes since 4.7.0
	    ಯದಿ (arg[1] >= 40700)
		term_props[TPR_MOUSE].tpr_status = TPR_MOUSE_SGR;
	    ಅನ್ಯಥಾ
		term_props[TPR_MOUSE].tpr_status = TPR_MOUSE_XTERM;
	}

	// If no recognized terminal has set mouse behavior, assume xterm.
	ಯದಿ (term_props[TPR_MOUSE].tpr_status == TPR_UNKNOWN)
	{
	    // Xterm version 277 supports SGR.
	    // Xterm version >= 95 supports mouse dragging.
	    ಯದಿ (version >= 277)
		term_props[TPR_MOUSE].tpr_status = TPR_MOUSE_SGR;
	    ಅನ್ಯಥಾ ಯದಿ (version >= 95)
		term_props[TPR_MOUSE].tpr_status = TPR_MOUSE_XTERM2;
	}

	// Detect terminals that set $TERM to something like
	// "xterm-256color" but are not fully xterm compatible.
	//
	// Gnome terminal sends 1;3801;0, 1;4402;0 or 1;2501;0.
	// Newer Gnome-terminal sends 65;6001;1.
	// xfce4-terminal sends 1;2802;0.
	// screen sends 83;40500;0
	// Assuming any version number over 2500 is not an
	// xterm (without the limit for rxvt and screen).
	ಯದಿ (arg[1] >= 2500)
	{
	    term_props[TPR_UNDERLINE_RGB].tpr_status = TPR_YES;
	}

	ಅನ್ಯಥಾ ಯದಿ (version == 136 && arg[2] == 0)
	{
	    term_props[TPR_UNDERLINE_RGB].tpr_status = TPR_YES;

	    // PuTTY sends 0;136;0
	    ಯದಿ (arg[0] == 0)
	    {
		// supports sgr-like mouse reporting.
		term_props[TPR_MOUSE].tpr_status = TPR_MOUSE_SGR;
	    }
	    // vandyke SecureCRT sends 1;136;0
	}

	// Konsole sends 0;115;0 - but t_u8 does not actually work, therefore
	// commented out.
	// else if (version == 115 && arg[0] == 0 && arg[2] == 0)
	//     term_props[TPR_UNDERLINE_RGB].tpr_status = TPR_YES;

	// Kitty up to 9.x sends 1;400{version};{secondary-version}
	ಯದಿ (arg[0] == 1 && arg[1] >= 4000 && arg[1] <= 4009)
	{
	    term_props[TPR_KITTY].tpr_status = TPR_YES;
	    term_props[TPR_KITTY].tpr_set_by_termresponse = TRUE;

	    // Kitty can handle SGR mouse reporting.
	    term_props[TPR_MOUSE].tpr_status = TPR_MOUSE_SGR;
	    term_props[TPR_DECRQM].tpr_status = TPR_YES;
	}

	// foot terminal sends 1;12700;0
	ಯದಿ (arg[0] == 1 && version == 12700 && arg[2] == 0)
	{
	    term_props[TPR_MOUSE].tpr_status = TPR_MOUSE_SGR;
	    term_props[TPR_DECRQM].tpr_status = TPR_YES;
	}

	// GNU screen sends 83;30600;0, 83;40500;0, etc.
	// 30600/40500 is a version number of GNU screen. DA2 support is added
	// on 3.6.  DCS string has a special meaning to GNU screen, but xterm
	// compatibility checking does not detect GNU screen.
	ಯದಿ (arg[0] == 83 && arg[1] >= 30600)
	{
	    term_props[TPR_CURSOR_STYLE].tpr_status = TPR_NO;
	    term_props[TPR_CURSOR_BLINK].tpr_status = TPR_NO;
	    term_props[TPR_DECRQM].tpr_status = TPR_NO; // screen doesn't seem
							// to handle DECRQM
							// sequences
	}

	// Xterm first responded to this request at patch level
	// 95, so assume anything below 95 is not xterm and hopefully supports
	// the underline RGB color sequence.
	ಯದಿ (version < 95)
	    term_props[TPR_UNDERLINE_RGB].tpr_status = TPR_YES;

	// Getting the cursor style is only supported properly by xterm since
	// version 279 (otherwise it returns 0x18).
	ಯದಿ (version < 279)
	    term_props[TPR_CURSOR_STYLE].tpr_status = TPR_NO;

	/*
	 * Take action on the detected properties.
	 */

	// Unless the underline RGB color is expected to work, disable "t_8u".
	// It does not work for the real Xterm, it resets the background color.
	// This may cause some flicker.  Alternative would be to set "t_8u"
	// here if the terminal is expected to support it, but that might
	// conflict with what was set in the .vimrc.
	ಯದಿ (term_props[TPR_UNDERLINE_RGB].tpr_status != TPR_YES
			&& *T_8U != NUL
			&& !option_was_set((char_u *)"t_8u"))
	{
	    set_string_option_direct((char_u *)"t_8u", -1, (char_u *)"",
								  OPT_FREE, 0);
	}
#ifdef FEAT_TERMRESPONSE
	ಯದಿ (*T_8U != NUL && write_t_8u_state == MAYBE)
	    // Did skip writing t_8u, a complete redraw is needed.
	    redraw_later_clear();
	write_t_8u_state = OK;  // can output t_8u now
#endif

	// Only set 'ttymouse' automatically if it was not set
	// by the user already.
	ಯದಿ (!option_was_set((char_u *)"ttym")
		&& (term_props[TPR_MOUSE].tpr_status == TPR_MOUSE_XTERM2
		    || term_props[TPR_MOUSE].tpr_status == TPR_MOUSE_SGR))
	{
	    set_option_ಮೌಲ್ಯ_give_err((char_u *)"ttym", 0L,
		    term_props[TPR_MOUSE].tpr_status == TPR_MOUSE_SGR
				    ? (char_u *)"sgr" : (char_u *)"xterm2", 0);
	}

#ifdef FEAT_TERMRESPONSE
	ಪೂರ್ಣಾಙ್ಕ need_flush = FALSE;

	// Only request the cursor style if t_SH and t_RS are
	// set. Only supported properly by xterm since version
	// 279 (otherwise it returns 0x18).
	// Only when getting the cursor style was detected to work.
	// Not for Terminal.app, it can't handle t_RS, it
	// echoes the characters to the screen.
	ಯದಿ (rcs_status.tr_progress == STATUS_GET
		&& term_props[TPR_CURSOR_STYLE].tpr_status == TPR_YES
		&& *T_CSH != NUL
		&& *T_CRS != NUL)
	{
	    MAY_WANT_TO_LOG_THIS;
	    LOG_TR1("Sending cursor style request");
	    out_str(T_CRS);
	    termrequest_sent(&rcs_status);
	    need_flush = TRUE;
	}

	// Only request the cursor blink mode if t_RC set. Not
	// for Gnome terminal, it can't handle t_RC, it
	// echoes the characters to the screen.
	// Only when getting the cursor style was detected to work.
	ಯದಿ (rbm_status.tr_progress == STATUS_GET
		&& term_props[TPR_CURSOR_BLINK].tpr_status == TPR_YES
		&& *T_CRC != NUL)
	{
	    MAY_WANT_TO_LOG_THIS;
	    LOG_TR1("Sending cursor blink mode request");
	    out_str(T_CRC);
	    termrequest_sent(&rbm_status);
	    need_flush = TRUE;
	}

	// Only request DEC modes via DECRQM when the terminal is known to
	// handle it.  Not for Apple Terminal.app or GNU screen, they echo
	// the trailing "p" to the screen.  See issue #19852.
	ಯದಿ (decrqm_status.tr_progress == STATUS_GET
		&& term_props[TPR_DECRQM].tpr_status == TPR_YES)
	{
	    MAY_WANT_TO_LOG_THIS;
	    LOG_TR1("Sending DECRQM requests");
	    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < (ಪೂರ್ಣಾಙ್ಕ)ARRAY_LENGTH(dec_modes); i++)
	    {
		vim_snprintf((ವರ್ಣ *)IObuff, IOSIZE, "\033[?%d$p", dec_modes[i]);
		out_str(IObuff);
	    }
	    termrequest_sent(&decrqm_status);
	    need_flush = TRUE;

	}

	ಯದಿ (need_flush)
	    out_flush();
#endif
    }
}

/*
 * Add "key" to "buf" and return the number of bytes used.
 * Handles special keys and multi-byte characters.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
add_ಕೀಲಿ_to_buf(ಪೂರ್ಣಾಙ್ಕ ಕೀಲಿ, char_u *buf)
{
    ಪೂರ್ಣಾಙ್ಕ idx = 0;

    ಯದಿ (IS_SPECIAL(ಕೀಲಿ))
    {
	buf[idx++] = K_SPECIAL;
	buf[idx++] = KEY2TERMCAP0(ಕೀಲಿ);
	buf[idx++] = KEY2TERMCAP1(ಕೀಲಿ);
    }
    ಅನ್ಯಥಾ ಯದಿ (has_mbyte)
	idx += (*mb_char2bytes)(ಕೀಲಿ, buf + idx);
    ಅನ್ಯಥಾ
	buf[idx++] = ಕೀಲಿ;
    ಪ್ರತ್ಯಾಗಮ idx;
}

/*
 * Shared between handle_key_with_modifier() and handle_csi_function_key().
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
put_ಕೀಲಿ_modifiers_in_typebuf(
	ಪೂರ್ಣಾಙ್ಕ	ಕೀಲಿ_arg,
	ಪೂರ್ಣಾಙ್ಕ	modifiers_arg,
	ಪೂರ್ಣಾಙ್ಕ	csi_len,
	ಪೂರ್ಣಾಙ್ಕ	offset,
	char_u	*buf,
	ಪೂರ್ಣಾಙ್ಕ	bufsize,
	ಪೂರ್ಣಾಙ್ಕ	*buflen)
{
    ಪೂರ್ಣಾಙ್ಕ ಕೀಲಿ = ಕೀಲಿ_arg;
    ಪೂರ್ಣಾಙ್ಕ modifiers = modifiers_arg;

    // Some keys need adjustment when the Ctrl modifier is used.
    ಕೀಲಿ = may_adjust_ಕೀಲಿ_for_ctrl(modifiers, ಕೀಲಿ);

    // May remove the shift modifier if it's already included in the key.
    modifiers = may_remove_shift_modifier(modifiers, ಕೀಲಿ);

    // Produce modifiers with K_SPECIAL KS_MODIFIER {mod}
    // worst-case: 3-byte modifier + 4 byte multi-char key + NUL
    char_u string[MAX_ಕೀಲಿ_CODE_LEN + 2];
    ಪೂರ್ಣಾಙ್ಕ new_slen = modifiers2keycode(modifiers, &ಕೀಲಿ, string);

    // reject overlong key that would overflow string
    ಯದಿ (ಕೀಲಿ > 0x10FFFF)
	ಪ್ರತ್ಯಾಗಮ -1;

    // Add the bytes for the key.
    new_slen += add_ಕೀಲಿ_to_buf(ಕೀಲಿ, string + new_slen);

    string[new_slen] = NUL;
    ಯದಿ (put_string_in_typebuf(offset, csi_len, string, new_slen,
						 buf, bufsize, buflen) == FAIL)
	ಪ್ರತ್ಯಾಗಮ -1;
    ಪ್ರತ್ಯಾಗಮ new_slen - csi_len + offset;
}

/*
 * Parse the number from a CSI numbered sequence for an F1-F12 key:
 *	ESC [ {number} ~
 * Returns the key
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
parse_csi_f_ಕೀಲಿಗಳು(ಪೂರ್ಣಾಙ್ಕ arg)
{
    char_u ಕೀಲಿ_name[2] = { 0, 0 };

    ವಿಕಲ್ಪ (arg)
    {
	ಪಕ್ಷ 11: ಕೀಲಿ_name[0] = 'k'; ಕೀಲಿ_name[1] = '1'; ಭಙ್ಗ;  // K_F1
	ಪಕ್ಷ 12: ಕೀಲಿ_name[0] = 'k'; ಕೀಲಿ_name[1] = '2'; ಭಙ್ಗ;  // K_F2
	ಪಕ್ಷ 13: ಕೀಲಿ_name[0] = 'k'; ಕೀಲಿ_name[1] = '3'; ಭಙ್ಗ;  // K_F3
	ಪಕ್ಷ 14: ಕೀಲಿ_name[0] = 'k'; ಕೀಲಿ_name[1] = '4'; ಭಙ್ಗ;  // K_F4
	ಪಕ್ಷ 15: ಕೀಲಿ_name[0] = 'k'; ಕೀಲಿ_name[1] = '5'; ಭಙ್ಗ;  // K_F5
	ಪಕ್ಷ 17: ಕೀಲಿ_name[0] = 'k'; ಕೀಲಿ_name[1] = '6'; ಭಙ್ಗ;  // K_F6
	ಪಕ್ಷ 18: ಕೀಲಿ_name[0] = 'k'; ಕೀಲಿ_name[1] = '7'; ಭಙ್ಗ;  // K_F7
	ಪಕ್ಷ 19: ಕೀಲಿ_name[0] = 'k'; ಕೀಲಿ_name[1] = '8'; ಭಙ್ಗ;  // K_F8
	ಪಕ್ಷ 20: ಕೀಲಿ_name[0] = 'k'; ಕೀಲಿ_name[1] = '9'; ಭಙ್ಗ;  // K_F9
	ಪಕ್ಷ 21: ಕೀಲಿ_name[0] = 'F'; ಕೀಲಿ_name[1] = ';'; ಭಙ್ಗ;  // K_F10
	ಪಕ್ಷ 23: ಕೀಲಿ_name[0] = 'F'; ಕೀಲಿ_name[1] = '1'; ಭಙ್ಗ;  // K_F11
	ಪಕ್ಷ 24: ಕೀಲಿ_name[0] = 'F'; ಕೀಲಿ_name[1] = '2'; ಭಙ್ಗ;  // K_F12
    }
    ಯದಿ (ಕೀಲಿ_name[0])
	ಪ್ರತ್ಯಾಗಮ TERMCAP2_ಕೀಲಿ(ಕೀಲಿ_name[0], ಕೀಲಿ_name[1]);
    // shouldn't happen
    ಪ್ರತ್ಯಾಗಮ arg;
}

/*
 * Handle a sequence with key and modifier, one of:
 *	{lead}27;{modifier};{key}~
 *	{lead}{key};{modifier}u
 * Returns the difference in length.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
handle_ಕೀಲಿ_with_modifier(
	ಪೂರ್ಣಾಙ್ಕ	*arg,
	ಪೂರ್ಣಾಙ್ಕ	csi_len,
	ಪೂರ್ಣಾಙ್ಕ	offset,
	char_u	*buf,
	ಪೂರ್ಣಾಙ್ಕ	bufsize,
	ಪೂರ್ಣಾಙ್ಕ	*buflen,
	ಪೂರ್ಣಾಙ್ಕ	iskitty,
	ಪೂರ್ಣಾಙ್ಕ	trail)
{
    // Only set seenModifyOtherKeys for the "{lead}27;" code to avoid setting
    // it for terminals using the kitty keyboard protocol.  Xterm sends
    // the form ending in "u" when the formatOtherKeys resource is set.  We do
    // not support this.
    //
    // Do not set seenModifyOtherKeys if there was a positive response at any
    // time from requesting the kitty keyboard protocol state, these are not
    // expected to support modifyOtherKeys level 2.
    //
    // Do not set seenModifyOtherKeys for kitty, it does send some sequences
    // like this but does not have the modifyOtherKeys feature.
    ಯದಿ (!iskitty
	    && (kitty_protocol_state == KKPS_INITIAL
		|| kitty_protocol_state == KKPS_OFF
		|| kitty_protocol_state == KKPS_AFTER_T_TE)
	    && term_props[TPR_KITTY].tpr_status != TPR_YES)
    {
#ifdef FEAT_EVAL
	ch_log(ರಿಕ್ತ, "setting seenModifyOtherKeys to TRUE");
#endif
	seen_Modify_Other_ಕೀಲಿಗಳು = TRUE;
    }

    ಪೂರ್ಣಾಙ್ಕ ಕೀಲಿ = iskitty ? arg[0] : arg[2];
    ಪೂರ್ಣಾಙ್ಕ modifiers = decode_modifiers(arg[1]);

    // Some terminals do not apply the Shift modifier to the key.  To make
    // mappings consistent we do it here.  TODO: support more keys.
    ಯದಿ ((modifiers & MOD_MASK_SHIFT) && ಕೀಲಿ >= 'a' && ಕೀಲಿ <= 'z')
	ಕೀಲಿ += 'A' - 'a';

    // Putting Esc in the buffer creates ambiguity, it can be the start of an
    // escape sequence.  Use K_ESC to avoid that.
    ಯದಿ (ಕೀಲಿ == ESC)
	ಕೀಲಿ = K_ESC;

    ಅನ್ಯಥಾ ಯದಿ (arg[0] >= 11 && arg[0] <= 24 && trail == '~')
	ಕೀಲಿ = parse_csi_f_ಕೀಲಿಗಳು(arg[0]);

    ಪ್ರತ್ಯಾಗಮ put_ಕೀಲಿ_modifiers_in_typebuf(ಕೀಲಿ, modifiers, csi_len, offset, buf,
	    bufsize, buflen);
}

/*
 * Handle a sequence with key without a modifier:
 *	{lead}{key}u
 *	{lead}{key}~
 * Returns the difference in length.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
handle_ಕೀಲಿ_without_modifier(
	ಪೂರ್ಣಾಙ್ಕ	*arg,
	ಪೂರ್ಣಾಙ್ಕ	csi_len,
	ಪೂರ್ಣಾಙ್ಕ	offset,
	char_u	*buf,
	ಪೂರ್ಣಾಙ್ಕ	bufsize,
	ಪೂರ್ಣಾಙ್ಕ	*buflen,
	ಪೂರ್ಣಾಙ್ಕ	trail)
{
    char_u  string[MAX_ಕೀಲಿ_CODE_LEN + 1];
    ಪೂರ್ಣಾಙ್ಕ	    new_slen;

    ಯದಿ (arg[0] == ESC)
    {
	// Putting Esc in the buffer creates ambiguity, it can be the start of
	// an escape sequence.  Use K_ESC to avoid that.
	string[0] = K_SPECIAL;
	string[1] = KS_EXTRA;
	string[2] = KE_ESC;
	new_slen = 3;
    }
    ಅನ್ಯಥಾ ಯದಿ (arg[0] >= 11 && arg[0] <= 24 && trail == '~')
    {
	ಪೂರ್ಣಾಙ್ಕ ಕೀಲಿ = parse_csi_f_ಕೀಲಿಗಳು(arg[0]);
	string[0] = K_SPECIAL;
	string[1] = KEY2TERMCAP0(ಕೀಲಿ);
	string[2] = KEY2TERMCAP1(ಕೀಲಿ);
	new_slen = 3;
    }
    ಅನ್ಯಥಾ
	new_slen = add_ಕೀಲಿ_to_buf(arg[0], string);

    ಯದಿ (put_string_in_typebuf(offset, csi_len, string, new_slen,
						 buf, bufsize, buflen) == FAIL)
	ಪ್ರತ್ಯಾಗಮ -1;
    ಪ್ರತ್ಯಾಗಮ new_slen - csi_len + offset;
}

/*
 * CSI function key without or with modifiers:
 *	{lead}[ABCDEFHPQRS]
 *	{lead}1;{modifier}[ABCDEFHPQRS]
 * Returns zero when nog recognized, a positive number when recognized.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
handle_csi_function_ಕೀಲಿ(
	ಪೂರ್ಣಾಙ್ಕ	argc,
	ಪೂರ್ಣಾಙ್ಕ	*arg,
	ಪೂರ್ಣಾಙ್ಕ	trail,
	ಪೂರ್ಣಾಙ್ಕ	csi_len,
	char_u	*ಕೀಲಿ_name,
	ಪೂರ್ಣಾಙ್ಕ	offset,
	char_u  *buf,
	ಪೂರ್ಣಾಙ್ಕ	bufsize,
	ಪೂರ್ಣಾಙ್ಕ	*buflen)
{
    ಕೀಲಿ_name[0] = 'k';
    ವಿಕಲ್ಪ (trail)
    {
	ಪಕ್ಷ 'A': ಕೀಲಿ_name[1] = 'u'; ಭಙ್ಗ;  // K_UP
	ಪಕ್ಷ 'B': ಕೀಲಿ_name[1] = 'd'; ಭಙ್ಗ;  // K_DOWN
	ಪಕ್ಷ 'C': ಕೀಲಿ_name[1] = 'r'; ಭಙ್ಗ;  // K_RIGHT
	ಪಕ್ಷ 'D': ಕೀಲಿ_name[1] = 'l'; ಭಙ್ಗ;  // K_LEFT

	// case 'E': keypad BEGIN - not supported
	ಪಕ್ಷ 'F': ಕೀಲಿ_name[0] = '@'; ಕೀಲಿ_name[1] = '7'; ಭಙ್ಗ;  // K_END
	ಪಕ್ಷ 'H': ಕೀಲಿ_name[1] = 'h'; ಭಙ್ಗ;  // K_HOME

	ಪಕ್ಷ 'P': ಕೀಲಿ_name[1] = '1'; ಭಙ್ಗ;  // K_F1
	ಪಕ್ಷ 'Q': ಕೀಲಿ_name[1] = '2'; ಭಙ್ಗ;  // K_F2
	ಪಕ್ಷ 'R': ಕೀಲಿ_name[1] = '3'; ಭಙ್ಗ;  // K_F3
	ಪಕ್ಷ 'S': ಕೀಲಿ_name[1] = '4'; ಭಙ್ಗ;  // K_F4

	ಸ್ವಭಾವ: ಪ್ರತ್ಯಾಗಮ 0;  // not recognized
    }

    ಪೂರ್ಣಾಙ್ಕ ಕೀಲಿ = TERMCAP2_ಕೀಲಿ(ಕೀಲಿ_name[0], ಕೀಲಿ_name[1]);
    ಪೂರ್ಣಾಙ್ಕ modifiers = argc == 2 ? decode_modifiers(arg[1]) : 0;
    put_ಕೀಲಿ_modifiers_in_typebuf(ಕೀಲಿ, modifiers,
					csi_len, offset, buf, bufsize, buflen);
    ಪ್ರತ್ಯಾಗಮ csi_len;
}

/*
 * Handle a CSI escape sequence.
 * - Xterm version string.
 *
 * - Response to XTQMODKEYS: "{lead} > 4 ; Pv m".
 *
 * - Cursor position report: {lead}{row};{col}R
 *   The final byte must be 'R'. It is used for checking the
 *   ambiguous-width character state.
 *
 * - window position reply: {lead}3;{x};{y}t
 *
 * - key with modifiers when modifyOtherKeys is enabled or the Kitty keyboard
 *   protocol is used:
 *	    {lead}27;{modifier};{key}~
 *	    {lead}{key};{modifier}u
 *
 * - function key with or without modifiers:
 *	{lead}[ABCDEFHPQRS]
 *	{lead}1;{modifier}[ABCDEFHPQRS]
 *
 * - DA1 query response: {lead}?...;c
 *
 * - DECRPM response: {lead}?2026;{mode}$y
 *
 * Return 0 for no match, -1 for partial match, > 0 for full match.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
handle_csi(
	char_u	*tp,
	ಪೂರ್ಣಾಙ್ಕ	len,
	char_u	*argp,
	ಪೂರ್ಣಾಙ್ಕ	offset,
	char_u  *buf,
	ಪೂರ್ಣಾಙ್ಕ	bufsize,
	ಪೂರ್ಣಾಙ್ಕ	*buflen,
	char_u	*ಕೀಲಿ_name,
	ಪೂರ್ಣಾಙ್ಕ	*slen)
{
    ಪೂರ್ಣಾಙ್ಕ		first = -1;  // optional char right after {lead}
    ಪೂರ್ಣಾಙ್ಕ		trail;	     // char that ends CSI sequence
    ಪೂರ್ಣಾಙ್ಕ		arg[3] = {-1, -1, -1};	// argument numbers
    ಪೂರ್ಣಾಙ್ಕ		argc = 0;		// number of arguments
    char_u	*ap = argp;
    ಪೂರ್ಣಾಙ್ಕ		csi_len;

    // Check for non-digit after CSI.
    ಯದಿ (!VIM_ISDIGIT(*ap))
	first = *ap++;

    ಯದಿ (first >= 'A' && first <= 'Z')
    {
	// If "first" is in [ABCDEFHPQRS] then it is actually the "trail" and
	// no argument follows.
	trail = first;
	first = -1;
	--ap;
    }
    ಅನ್ಯಥಾ
    {
	// Find up to three argument numbers.
	ಪ್ರತಿ (argc = 0; argc < 3; )
	{
	    ಯದಿ (ap >= tp + len)
		ಪ್ರತ್ಯಾಗಮ -1;
	    ಯದಿ (*ap == ';')
		arg[argc++] = -1;  // omitted number
	    ಅನ್ಯಥಾ ಯದಿ (VIM_ISDIGIT(*ap))
	    {
		arg[argc] = 0;
		ಪ್ರತಿ (;;)
		{
		    ಯದಿ (ap >= tp + len)
			ಪ್ರತ್ಯಾಗಮ -1;
		    ಯದಿ (!VIM_ISDIGIT(*ap))
			ಭಙ್ಗ;
		    // avoid overflow
		    ಯದಿ (arg[argc] <= (INT_MAX - 9) / 10)
			arg[argc] = arg[argc] * 10 + (*ap - '0');
		    ++ap;
		}
		++argc;
	    }
	    ಯದಿ (*ap == ';')
		++ap;
	    ಅನ್ಯಥಾ
		ಭಙ್ಗ;
	}

	// mrxvt has been reported to have "+" in the version. Assume
	// the escape sequence ends with a letter or one of "{|}~".
	ಯಾವತ್ (ap < tp + len
		&& !(*ap >= '{' && *ap <= '~')
		&& !ASCII_ISALPHA(*ap))
	    ++ap;
	ಯದಿ (ap >= tp + len)
	    ಪ್ರತ್ಯಾಗಮ -1;
	trail = *ap;
    }

    csi_len = (ಪೂರ್ಣಾಙ್ಕ)(ap - tp) + 1;

    // Response to XTQMODKEYS: "CSI > 4 ; Pv m" where Pv indicates the
    // modifyOtherKeys level.  Drop similar responses.
    ಯದಿ (first == '>' && (argc == 1 || argc == 2) && trail == 'm')
    {
	ಯದಿ (arg[0] == 4 && argc == 2)
	    modify_otherkeys_state = arg[1] == 2 ? MOKS_ENABLED : MOKS_OFF;

	ಕೀಲಿ_name[0] = (ಪೂರ್ಣಾಙ್ಕ)KS_EXTRA;
	ಕೀಲಿ_name[1] = (ಪೂರ್ಣಾಙ್ಕ)KE_IGNORE;
	*slen = csi_len;
    }

    // Function key starting with CSI:
    //	{lead}[ABCDEFHPQRS]
    //	{lead}1;{modifier}[ABCDEFHPQRS]
    ಅನ್ಯಥಾ ಯದಿ (first == -1 && ASCII_ISUPPER(trail)
	    && (argc == 0 || (argc == 2 && arg[0] == 1)))
    {
	ಪೂರ್ಣಾಙ್ಕ res = handle_csi_function_ಕೀಲಿ(argc, arg, trail,
			      csi_len, ಕೀಲಿ_name, offset, buf, bufsize, buflen);
	ಪ್ರತ್ಯಾಗಮ res <= 0 ? res : len + res;
    }

    // Cursor position report: {lead}{row};{col}R
    // Eat it when there are 2 arguments and it ends in 'R'.
    // Also when u7_status is not "sent", it may be from a previous Vim that
    // just exited.  But not for <S-F3>, it sends something similar, check for
    // row and column to make sense.
    ಅನ್ಯಥಾ ಯದಿ (first == -1 && argc == 2 && trail == 'R')
    {
	handle_u7_response(arg, tp, csi_len);

	ಕೀಲಿ_name[0] = (ಪೂರ್ಣಾಙ್ಕ)KS_EXTRA;
	ಕೀಲಿ_name[1] = (ಪೂರ್ಣಾಙ್ಕ)KE_IGNORE;
	*slen = csi_len;
    }

#ಯದಿ (defined(UNIX) || defined(MSWIN)) \
    && (defined(FEAT_IMAGE) || defined(FEAT_EVAL))
    // Response to CSI 14 t or CSI 16 t
    ಅನ್ಯಥಾ ಯದಿ (first == -1 && argc >= 3
	    && (arg[0] == 4 || arg[0] == 6) && trail == 't')
    {
	ಯದಿ (arg[0] == 4)
	{
	    LOG_TRN("Received CSI 14 t response: %s", tp);
	    cell_width = arg[2] / Columns;
	    cell_height = arg[1] / Rows;
	}
	ಅನ್ಯಥಾ
	{
	    LOG_TRN("Received CSI 16 t response: %s", tp);
	    cell_width = arg[2];
	    cell_height = arg[1];
	}

	ಕೀಲಿ_name[0] = (ಪೂರ್ಣಾಙ್ಕ)KS_EXTRA;
	ಕೀಲಿ_name[1] = (ಪೂರ್ಣಾಙ್ಕ)KE_IGNORE;
	*slen = csi_len;
    }
#endif

    // Primary device attributes (DA1) response
    ಅನ್ಯಥಾ ಯದಿ (first == '?' && trail == 'c')
    {
	LOG_TRN("Received DA1 response: %s", tp);

	*slen = csi_len;
#ifdef FEAT_EVAL
	set_vim_var_string(VV_TERMDA1, tp, *slen);
#endif
	apply_autocmds(EVENT_TERMRESPONSEALL,
					(char_u *)"da1", ರಿಕ್ತ, FALSE, curbuf);

	ಕೀಲಿ_name[0] = (ಪೂರ್ಣಾಙ್ಕ)KS_EXTRA;
	ಕೀಲಿ_name[1] = (ಪೂರ್ಣಾಙ್ಕ)KE_IGNORE;
	*slen = csi_len;
    }

    // DECRPM mode 2026 or 2048.
    ಅನ್ಯಥಾ ಯದಿ (first == '?' && trail == 'y' && argc == 2
	    && (arg[0] == 2026 || arg[0] == 2048))
    {
	ಪೂರ್ಣಾಙ್ಕ setting = arg[1];

	*slen = csi_len;
	ಕೀಲಿ_name[0] = (ಪೂರ್ಣಾಙ್ಕ)KS_EXTRA;
	ಕೀಲಿ_name[1] = (ಪೂರ್ಣಾಙ್ಕ)KE_IGNORE;

#ifdef FEAT_TERMRESPONSE
	// Mark the DECRQM request as answered so it is not sent again and
	// stoptermcap() does not wait for it.
	ಯದಿ (decrqm_status.tr_progress == STATUS_SENT)
	    decrqm_status.tr_progress = STATUS_GOT;
#endif

	ಯದಿ (setting >= 0 && setting <= 4)
	{
	    LOG_TRN("Received DECRPM mode %d: %s", arg[0], tp);

	    ವಿಕಲ್ಪ (arg[0])
	    {
		ಪಕ್ಷ 2026:
		    sync_output_setting = setting;
		    set_option_ಮೌಲ್ಯ_give_err((char_u *)"termsync",
			    setting == 1 || setting == 2, ರಿಕ್ತ, 0);
		    ಭಙ್ಗ;
#ifdef UNIX
		ಪಕ್ಷ 2048:
		    win_resize_setting = setting;

		    term_set_win_resize(true);
		    ಭಙ್ಗ;
#endif
	    }
	}
	ಅನ್ಯಥಾ
	    LOG_TRN("Unknown DECRPM mode %d setting %d", arg[0], setting);
    }

#ifdef UNIX
    // In-band window resize event
    ಅನ್ಯಥಾ ಯದಿ (win_resize_enabled && argc >= 3 && arg[0] == 48)
    {
	ಪೂರ್ಣಾಙ್ಕ height = arg[1], width = arg[2];

	*slen = csi_len;
	ಕೀಲಿ_name[0] = (ಪೂರ್ಣಾಙ್ಕ)KS_EXTRA;
	ಕೀಲಿ_name[1] = (ಪೂರ್ಣಾಙ್ಕ)KE_IGNORE;

	// Only update if needed. Avoids intro message from disappearing on
	// startup (due to initial event).
	ಯದಿ (height != Rows || width != Columns)
	    set_shellsize(width, height, true);
    }
#endif

    // Version string: Eat it when there is at least one digit and
    // it ends in 'c'
    ಅನ್ಯಥಾ ಯದಿ (*T_CRV != NUL && ap > argp + 1 && trail == 'c')
    {
	handle_version_response(first, arg, argc, tp);

	*slen = csi_len;
#ifdef FEAT_EVAL
	set_vim_var_string(VV_TERMRESPONSE, tp, *slen);
#endif
	apply_autocmds(EVENT_TERMRESPONSE,
					ರಿಕ್ತ, ರಿಕ್ತ, FALSE, curbuf);
	apply_autocmds(EVENT_TERMRESPONSEALL,
					(char_u *)"version", ರಿಕ್ತ, FALSE, curbuf);
	ಕೀಲಿ_name[0] = (ಪೂರ್ಣಾಙ್ಕ)KS_EXTRA;
	ಕೀಲಿ_name[1] = (ಪೂರ್ಣಾಙ್ಕ)KE_IGNORE;
    }

#ifdef FEAT_TERMRESPONSE
    // Check blinking cursor from xterm:
    // {lead}?12;1$y       set
    // {lead}?12;2$y       not set
    //
    // {lead} can be <Esc>[ or CSI
    ಅನ್ಯಥಾ ಯದಿ (rbm_status.tr_progress == STATUS_SENT
	    && first == '?'
	    && ap == argp + 6
	    && arg[0] == 12
	    && ap[-1] == '$'
	    && trail == 'y')
    {
	initial_cursor_blink = (arg[1] == '1');
	rbm_status.tr_progress = STATUS_GOT;
	LOG_TRN("Received cursor blinking mode response: %s", tp);
	ಕೀಲಿ_name[0] = (ಪೂರ್ಣಾಙ್ಕ)KS_EXTRA;
	ಕೀಲಿ_name[1] = (ಪೂರ್ಣಾಙ್ಕ)KE_IGNORE;
	*slen = csi_len;
# ifdef FEAT_EVAL
	set_vim_var_string(VV_TERMBLINKRESP, tp, *slen);
# endif
	apply_autocmds(EVENT_TERMRESPONSEALL,
					(char_u *)"cursorblink", ರಿಕ್ತ, FALSE, curbuf);
    }
#endif

    // Kitty keyboard protocol status response: CSI ? flags u
    ಅನ್ಯಥಾ ಯದಿ (first == '?' && argc == 1 && trail == 'u')
    {
	// The protocol has various "progressive enhancement flags" values, but
	// we only check for zero and non-zero here.
	ಯದಿ (arg[0] == '0')
	{
	    kitty_protocol_state = KKPS_OFF;
	}
	ಅನ್ಯಥಾ
	{
	    kitty_protocol_state = KKPS_ENABLED;

	    // Reset seenModifyOtherKeys just in case some key combination has
	    // been seen that set it before we get the status response.
#ifdef FEAT_EVAL
	    ch_log(ರಿಕ್ತ, "setting seenModifyOtherKeys to FALSE");
#endif
	    seen_Modify_Other_ಕೀಲಿಗಳು = FALSE;
	}

	ಕೀಲಿ_name[0] = (ಪೂರ್ಣಾಙ್ಕ)KS_EXTRA;
	ಕೀಲಿ_name[1] = (ಪೂರ್ಣಾಙ್ಕ)KE_IGNORE;
	*slen = csi_len;
    }

#ifdef FEAT_TERMRESPONSE
    // Check for a window position response from the terminal:
    //       {lead}3;{x};{y}t
    ಅನ್ಯಥಾ ಯದಿ (did_request_winpos && argc == 3 && arg[0] == 3
						   && trail == 't')
    {
	winpos_x = arg[1];
	winpos_y = arg[2];
	// got finished code: consume it
	ಕೀಲಿ_name[0] = (ಪೂರ್ಣಾಙ್ಕ)KS_EXTRA;
	ಕೀಲಿ_name[1] = (ಪೂರ್ಣಾಙ್ಕ)KE_IGNORE;
	*slen = csi_len;

	ಯದಿ (--did_request_winpos <= 0)
	    winpos_status.tr_progress = STATUS_GOT;
    }
#endif

    // Key with modifier:
    //	{lead}27;{modifier};{key}~
    //	{lead}{key};{modifier}u
    //	{lead}{key};{modifier}~
    // Even though we only handle four modifiers and the {modifier} value
    // should be 16 or lower, we accept all modifier values to avoid the raw
    // sequence to be passed through.
    ಅನ್ಯಥಾ ಯದಿ ((arg[0] == 27 && argc == 3 && trail == '~')
		|| (argc == 2 && (trail == 'u' || trail == '~')))
    {
	ಪೂರ್ಣಾಙ್ಕ iskitty = argc == 2 && (trail == 'u' || trail == '~');
	ಪ್ರತ್ಯಾಗಮ len + handle_ಕೀಲಿ_with_modifier(arg, csi_len, offset, buf,
		bufsize, buflen, iskitty, trail);
    }

    // Key without modifier (Kitty sends this for Esc or F3):
    //	{lead}{key}u
    //	{lead}{key}~
    ಅನ್ಯಥಾ ಯದಿ (argc == 1 && (trail == 'u' || trail == '~'))
    {
	ಪ್ರತ್ಯಾಗಮ len + handle_ಕೀಲಿ_without_modifier(arg, csi_len, offset, buf,
		bufsize, buflen, trail);
    }

    // else: Unknown CSI sequence.  We could drop it, but then the
    // user can't create a map for it.
    ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ
check_for_color_response(char_u *resp, ಪೂರ್ಣಾಙ್ಕ len)
{
    ಪೂರ್ಣಾಙ್ಕ		i, j;
    char_u	*argp;

    j = 1 + (resp[0] == ESC);
    argp = resp + j;

    ಯದಿ (len >= j + 3 && (argp[0] != '1'
			     || (argp[1] != '1' && argp[1] != '0')
			     || argp[2] != ';'))
	i = 0; // no match
    ಅನ್ಯಥಾ
    {
	ಪ್ರತಿ (i = j; i < len; ++i)
	    ಯದಿ (resp[i] == '\007' || (resp[0] == OSC ? resp[i] == STERM
			: (resp[i] == ESC && i + 1 < len && resp[i + 1] == '\\')))
	    {
		ಪೂರ್ಣಾಙ್ಕ is_bg = argp[1] == '1';
		ಪೂರ್ಣಾಙ್ಕ is_4digit = i - j >= 21 && resp[j + 11] == '/'
						  && resp[j + 16] == '/';

		ಯದಿ (i - j >= 15 && STRNCMP(resp + j + 3, "rgb:", 4) == 0
			    && (is_4digit
				   || (resp[j + 9] == '/' && resp[j + 12] == '/')))
		{
		    char_u *tp_r = resp + j + 7;
		    char_u *tp_g = resp + j + (is_4digit ? 12 : 10);
		    char_u *tp_b = resp + j + (is_4digit ? 17 : 13);
#ifdef FEAT_TERMRESPONSE
		    ಪೂರ್ಣಾಙ್ಕ rval, gval, bval;

		    rval = hexhex2nr(tp_r);
		    gval = hexhex2nr(tp_g);
		    bval = hexhex2nr(tp_b);
#endif
		    ಯದಿ (is_bg)
		    {
			ವರ್ಣ *new_bg_val = (3 * '6' < *tp_r + *tp_g +
					     *tp_b) ? "light" : "dark";

			LOG_TRN("Received RBG response: r=%d g=%d b=%d", rval, gval, bval);
#ifdef FEAT_TERMRESPONSE
			rbg_status.tr_progress = STATUS_GOT;
			bg_r = rval;
			bg_g = gval;
			bg_b = bval;
#endif
			ಯದಿ (!option_was_set((char_u *)"bg")
				      && STRCMP(p_bg, new_bg_val) != 0)
			{
			    // value differs, apply it
			    set_option_ಮೌಲ್ಯ_give_err((char_u *)"bg",
						  0L, (char_u *)new_bg_val, 0);
			    reset_option_was_set((char_u *)"bg");
			    redraw_asap(UPD_CLEAR);
			}
		    }
#ಯದಿ defined(FEAT_TERMRESPONSE) && defined(FEAT_TERMINAL)
		    ಅನ್ಯಥಾ
		    {
			LOG_TRN("Received RFG response: r=%d g=%d b=%d", rval, gval, bval);
			rfg_status.tr_progress = STATUS_GOT;
			fg_r = rval;
			fg_g = gval;
			fg_b = bval;
		    }
#endif
		}

#ifdef FEAT_EVAL
		set_vim_var_string(is_bg ? VV_TERMRBGRESP
						  : VV_TERMRFGRESP, resp, len);
#endif
		apply_autocmds(EVENT_TERMRESPONSEALL,
			is_bg ? (char_u *)"background" : (char_u *)"foreground",
			ರಿಕ್ತ, FALSE, curbuf);
		ಭಙ್ಗ;
	    }
    }
    ಯದಿ (i == len)
	LOG_TR1("not enough characters for RB");
}

ಸ್ಥಿರ oscstate_T osc_state;

/*
 * Return true if currently receiving an OSC response.
 */
    bool
in_osc_sequence(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ osc_state.processing;
}

/*
 * Handles any OSC sequence and places the result in "v:termosc". Returns OK on
 * success and FAIL on failure.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
handle_osc(char_u *tp, ಪೂರ್ಣಾಙ್ಕ len, char_u *ಕೀಲಿ_name, ಪೂರ್ಣಾಙ್ಕ *slen)
{
    char_u last_char;

    ಯದಿ (!osc_state.processing)
    {
	ಪೂರ್ಣಾಙ್ಕ cur;

	LOG_TRN("Received OSC response: %s", (ವರ್ಣ*)tp);
	// Check if it is a valid OSC sequence, and consume it. OSC format
	// consists of:
	// <identifier><data><terminator>
	// <identifier> is either <Esc>] or an OSC character
	// <terminator> can be '\007', <Esc>\ or STERM.
	cur = 1 + (tp[0] == ESC);

	ಯದಿ (len < cur + 1 + (tp[0] != OSC)) // Include terminator as well
	    ಪ್ರತ್ಯಾಗಮ FAIL;

	// The whole OSC response may be larger than the typeahead buffer.
	// To handle this, keep reading data in and out of the typeahead
	// buffer until we read an OSC terminator or timeout.

	// We can't use the previous buffer since we transferred ownership of it
	// to the vim var.
	ga_init2(&osc_state.buf, 1, 1024);
#ifdef ELAPSED_FUNC
	ELAPSED_INIT(osc_state.start_tv);
#endif
	osc_state.processing = TRUE;
	osc_state.start_char = tp[0];
	last_char = 0;
    }
    ಅನ್ಯಥಾ
	last_char = ((char_u *)osc_state.buf.ga_data)[osc_state.buf.ga_len - 1];

    ಕೀಲಿ_name[0] = (ಪೂರ್ಣಾಙ್ಕ)KS_EXTRA;

    // Read data and append to buffer. If we reach a terminator, then
    // finally set the vim var.
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < len; i++)
	ಯದಿ (tp[i] == '\007' || (osc_state.start_char == OSC ? tp[i] == STERM
		    : ((tp[i] == ESC && i + 1 < len && tp[i + 1] == '\\')
			|| (i == 0 && tp[i] == '\\' && last_char == ESC)
			)))
	{
	    osc_state.processing = FALSE;

	    ಕೀಲಿ_name[1] = (ಪೂರ್ಣಾಙ್ಕ)KE_OSC;

	    ga_concat_len(&osc_state.buf, tp, i + 1 + (tp[i] == ESC));
	    ga_append(&osc_state.buf, NUL);
	    *slen = i + 1 + (tp[i] == ESC);
#ifdef FEAT_EVAL
	    set_vim_var_string_direct(VV_TERMOSC, osc_state.buf.ga_data);
#endif
	    // Check for background/foreground colour response
	    check_for_color_response(osc_state.buf.ga_data, osc_state.buf.ga_len - 1);

	    char_u savebg = *p_bg;
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"osc",
		    ರಿಕ್ತ, FALSE, curbuf);
	    ಯದಿ (*p_bg != savebg)
		redraw_asap(UPD_CLEAR);
	    ಪ್ರತ್ಯಾಗಮ OK;
	}

    ಕೀಲಿ_name[1] = (ಪೂರ್ಣಾಙ್ಕ)KE_IGNORE;

#ifdef ELAPSED_FUNC
    ಯದಿ (ELAPSED_FUNC(osc_state.start_tv) >= p_ost)
    {
	semsg(_(e_osc_response_timed_out), osc_state.buf.ga_len,
		osc_state.buf.ga_data);

	ga_clear(&osc_state.buf);
	osc_state.processing = FALSE;
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
#endif

    ga_concat(&osc_state.buf, tp);
    *slen = len; // Consume everything

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Check for key code response from xterm:
 * {lead}{flag}+r<hex bytes><{tail}
 *
 * {lead} can be <Esc>P or DCS
 * {flag} can be '0' or '1'
 * {tail} can be Esc>\ or STERM
 *
 * Check for resource response from xterm (and drop it):
 * {lead}{flag}+R<hex bytes>=<value>{tail}
 *
 * Check for cursor shape response from xterm:
 * {lead}1$r<digit> q{tail}
 *
 * {lead} can be <Esc>P or DCS
 * {tail} can be <Esc>\ or STERM
 *
 * Consume any code that starts with "{lead}.+r" or "{lead}.$r".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
handle_dcs(char_u *tp, char_u *argp, ಪೂರ್ಣಾಙ್ಕ len, char_u *ಕೀಲಿ_name, ಪೂರ್ಣಾಙ್ಕ *slen)
{
    ಪೂರ್ಣಾಙ್ಕ i, j;

    LOG_TRN("Received DCS response: %s", (ವರ್ಣ*)tp);
    j = 1 + (tp[0] == ESC);
    ಯದಿ (len < j + 3)
	i = len; // need more chars
    ಅನ್ಯಥಾ ಯದಿ ((argp[1] != '+' && argp[1] != '$')
	    || (argp[2] != 'r' && argp[2] != 'R'))
	i = 0; // no match
    ಅನ್ಯಥಾ ಯದಿ (argp[1] == '+')
	// key code response
	ಪ್ರತಿ (i = j; i < len; ++i)
	{
	    ಯದಿ ((tp[i] == ESC && i + 1 < len && tp[i + 1] == '\\')
		    || tp[i] == STERM)
	    {
#ifdef FEAT_TERMRESPONSE
		// handle a key code response, drop a resource response
		ಯದಿ (i - j >= 3 && argp[2] == 'r')
		    got_code_from_term(tp + j, i);
#endif
		ಕೀಲಿ_name[0] = (ಪೂರ್ಣಾಙ್ಕ)KS_EXTRA;
		ಕೀಲಿ_name[1] = (ಪೂರ್ಣಾಙ್ಕ)KE_IGNORE;
		*slen = i + 1 + (tp[i] == ESC);
		ಭಙ್ಗ;
	    }
	}
    ಅನ್ಯಥಾ
    {
	// Probably the cursor shape response.  Make sure that "i"
	// is equal to "len" when there are not sufficient
	// characters.
	ಪ್ರತಿ (i = j + 3; i < len; ++i)
	{
	    ಯದಿ (i - j == 3 && !SAFE_isdigit(tp[i]))
		ಭಙ್ಗ;
	    ಯದಿ (i - j == 4 && tp[i] != ' ')
		ಭಙ್ಗ;
	    ಯದಿ (i - j == 5 && tp[i] != 'q')
		ಭಙ್ಗ;
	    ಯದಿ (i - j == 6 && tp[i] != ESC && tp[i] != STERM)
		ಭಙ್ಗ;
	    ಯದಿ ((i - j == 6 && tp[i] == STERM)
		    || (i - j == 7 && tp[i] == '\\'))
	    {
#ifdef FEAT_TERMRESPONSE
		ಪೂರ್ಣಾಙ್ಕ ಸಂಖ್ಯೆ = argp[3] - '0';

		// 0, 1 = block blink, 2 = block
		// 3 = underline blink, 4 = underline
		// 5 = vertical bar blink, 6 = vertical bar
		ಸಂಖ್ಯೆ = ಸಂಖ್ಯೆ == 0 ? 1 : ಸಂಖ್ಯೆ;
		initial_cursor_shape = (ಸಂಖ್ಯೆ + 1) / 2;
		// The blink flag is actually inverted, compared to
		// the value set with T_SH.
		initial_cursor_shape_blink =
				       (ಸಂಖ್ಯೆ & 1) ? FALSE : TRUE;
		rcs_status.tr_progress = STATUS_GOT;
#endif
		LOG_TRN("Received cursor shape response: %s", tp);

		ಕೀಲಿ_name[0] = (ಪೂರ್ಣಾಙ್ಕ)KS_EXTRA;
		ಕೀಲಿ_name[1] = (ಪೂರ್ಣಾಙ್ಕ)KE_IGNORE;
		*slen = i + 1;
#ifdef FEAT_EVAL
		set_vim_var_string(VV_TERMSTYLERESP, tp, *slen);
#endif
		apply_autocmds(EVENT_TERMRESPONSEALL,
					(char_u *)"cursorshape", ರಿಕ್ತ, FALSE, curbuf);
		ಭಙ್ಗ;
	    }
	}
    }

    ಯದಿ (i == len)
    {
	// These codes arrive many together, each code can be
	// truncated at any point.
	LOG_TR1("not enough characters for XT");
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Check if typebuf.tb_buf[] contains a terminal key code.
 * Check from typebuf.tb_buf[typebuf.tb_off] to typebuf.tb_buf[typebuf.tb_off
 * + "max_offset"].
 * Return 0 for no match, -1 for partial match, > 0 for full match.
 * Return KEYLEN_REMOVED when a key code was deleted.
 * With a match, the match is removed, the replacement code is inserted in
 * typebuf.tb_buf[] and the number of characters in typebuf.tb_buf[] is
 * returned.
 * When "buf" is not NULL, buf[bufsize] is used instead of typebuf.tb_buf[].
 * "buflen" is then the length of the string in buf[] and is updated for
 * inserts and deletes.
 */
    ಪೂರ್ಣಾಙ್ಕ
check_termcode(
    ಪೂರ್ಣಾಙ್ಕ		max_offset,
    char_u	*buf,
    ಪೂರ್ಣಾಙ್ಕ		bufsize,
    ಪೂರ್ಣಾಙ್ಕ		*buflen)
{
    char_u	*tp;
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		slen = 0;	// init for GCC
    ಪೂರ್ಣಾಙ್ಕ		modslen;
    ಪೂರ್ಣಾಙ್ಕ		len;
    ಪೂರ್ಣಾಙ್ಕ		retval = 0;
    ಪೂರ್ಣಾಙ್ಕ		offset;
    char_u	ಕೀಲಿ_name[2];
    ಪೂರ್ಣಾಙ್ಕ		modifiers;
    char_u	*modifiers_start = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		ಕೀಲಿ;
    ಪೂರ್ಣಾಙ್ಕ		new_slen;   // Length of what will replace the termcode
    char_u	string[MAX_ಕೀಲಿ_CODE_LEN + 1];
    ಪೂರ್ಣಾಙ್ಕ		i, j;
    ಪೂರ್ಣಾಙ್ಕ		idx = 0;
    ಪೂರ್ಣಾಙ್ಕ		cpo_koffset;

    cpo_koffset = (vim_strchr(p_cpo, CPO_KOFFSET) != ರಿಕ್ತ);

    /*
     * Speed up the checks for terminal codes by gathering all first bytes
     * used in termleader[].  Often this is just a single <Esc>.
     */
    ಯದಿ (need_gather)
	gather_termleader();

    /*
     * Check at several positions in typebuf.tb_buf[], to catch something like
     * "x<Up>" that can be mapped. Stop at max_offset, because characters
     * after that cannot be used for mapping, and with @r commands
     * typebuf.tb_buf[] can become very long.
     * This is used often, KEEP IT FAST!
     */
    ಪ್ರತಿ (offset = 0; offset < max_offset; ++offset)
    {
	ಯದಿ (buf == ರಿಕ್ತ)
	{
	    ಯದಿ (offset >= typebuf.tb_len)
		ಭಙ್ಗ;
	    tp = typebuf.tb_buf + typebuf.tb_off + offset;
	    len = typebuf.tb_len - offset;	// length of the input
	}
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (offset >= *buflen)
		ಭಙ್ಗ;
	    tp = buf + offset;
	    len = *buflen - offset;
	}

	/*
	 * Don't check characters after K_SPECIAL, those are already
	 * translated terminal chars (avoid translating ~@^Hx).
	 */
	ಯದಿ (*tp == K_SPECIAL)
	{
	    offset += 2;	// there are always 2 extra characters
	    ಅನುವರ್ತ;
	}

	ಯದಿ (osc_state.processing)
	{
	    // Still processing OSC response data, go straight to handler
	    // function.
	    tp[len] = NUL;
	    ಕೀಲಿ_name[0] = NUL;
	    ಕೀಲಿ_name[1] = NUL;
	    modifiers = 0;
	    ಗಚ್ಛ handle_osc;
	}

	/*
	 * Skip this position if the character does not appear as the first
	 * character in term_strings. This speeds up a lot, since most
	 * termcodes start with the same character (ESC or CSI).
	 */
	i = *tp;
	ಪ್ರತಿ (p = termleader; *p && *p != i; ++p)
	    ;
	ಯದಿ (*p == NUL)
	    ಅನುವರ್ತ;

	/*
	 * Skip this position if p_ek is not set and tp[0] is an ESC and we
	 * are in Insert mode.
	 */
	ಯದಿ (*tp == ESC && !p_ek && (State & MODE_ಸೇರಿಸು))
	    ಅನುವರ್ತ;

	tp[len] = NUL;
	ಕೀಲಿ_name[0] = NUL;	// no key name found yet
	ಕೀಲಿ_name[1] = NUL;	// no key name found yet
	modifiers = 0;		// no modifiers yet

#ifdef FEAT_GUI
	ಯದಿ (gui.in_use)
	{
	    /*
	     * GUI special key codes are all of the form [CSI xx].
	     */
	    ಯದಿ (*tp == CSI)	    // Special key from GUI
	    {
		ಯದಿ (len < 3)
		    ಪ್ರತ್ಯಾಗಮ -1;	    // Shouldn't happen
		slen = 3;
		ಕೀಲಿ_name[0] = tp[1];
		ಕೀಲಿ_name[1] = tp[2];
	    }
	}
	ಅನ್ಯಥಾ
#endif // FEAT_GUI
#ifdef MSWIN
	    ಯದಿ (len >= 3 && tp[0] == CSI && tp[1] == KS_EXTRA
		    && (tp[2] == KE_MOUSEUP
			|| tp[2] == KE_MOUSEDOWN
			|| tp[2] == KE_MOUSELEFT
			|| tp[2] == KE_MOUSERIGHT))
	{
	    // MS-Windows console sends mouse scroll events encoded:
	    // - CSI
	    // - KS_EXTRA
	    // - {KE_MOUSE[UP|DOWN|LEFT|RIGHT]}
	    slen = 3;
	    ಕೀಲಿ_name[0] = tp[1];
	    ಕೀಲಿ_name[1] = tp[2];
	}
	ಅನ್ಯಥಾ
#endif
	{
	    ಪೂರ್ಣಾಙ್ಕ  mouse_index_found = -1;
	    ಪೂರ್ಣಾಙ್ಕ keypad_index_found = -1;
	    ಪೂರ್ಣಾಙ್ಕ keypad_slen_found = 0;

	    ಪ್ರತಿ (idx = 0; idx < tc_len; ++idx)
	    {
		ಪೂರ್ಣಾಙ್ಕ     is_keypad = FALSE;
		/*
		 * Ignore the entry if we are not at the start of
		 * typebuf.tb_buf[]
		 * and there are not enough characters to make a match.
		 * But only when the 'K' flag is in 'cpoptions'.
		 */
		slen = termcodes[idx].len;
		modifiers_start = ರಿಕ್ತ;
		ಯದಿ (cpo_koffset && offset && len < slen)
		    ಅನುವರ್ತ;
		ಯದಿ (STRNCMP(termcodes[idx].code, tp,
				     (size_t)(slen > len ? len : slen)) == 0)
		{
		    ಪೂರ್ಣಾಙ್ಕ	    looks_like_mouse_start = FALSE;

		    ಯದಿ (len < slen)		// got a partial sequence
			ಪ್ರತ್ಯಾಗಮ -1;		// need to get more chars

		    /*
		     * When found a keypad key, check if there is another key
		     * that matches and use that one.  This makes <Home> to be
		     * found instead of <kHome> when they produce the same
		     * key code.
		     */
		    ಯದಿ (termcodes[idx].name[0] == 'K'
				       && (VIM_ISDIGIT(termcodes[idx].name[1])
						|| ASCII_ISUPPER(termcodes[idx].name[1])))
		    {
			is_keypad = TRUE;
			// Only use it when there is no other match.
			ಯದಿ (keypad_index_found < 0)
			{
			    keypad_index_found = idx;
			    keypad_slen_found = slen;
			}
		    }

		    ಯದಿ (slen == 2 && len > 2
			    && termcodes[idx].code[0] == ESC
			    && termcodes[idx].code[1] == '[')
		    {
			// The mouse termcode "ESC [" is also the prefix of
			// "ESC [ I" (focus gained) and other keys.  Check some
			// more bytes to find out.
			ಯದಿ (!SAFE_isdigit(tp[2]))
			{
			    // ESC [ without number following: Only use it when
			    // there is no other match.
			    looks_like_mouse_start = TRUE;
			}
			ಅನ್ಯಥಾ ಯದಿ (termcodes[idx].name[0] == KS_DEC_MOUSE)
			{
			    char_u  *nr = tp + 2;
			    ಪೂರ್ಣಾಙ್ಕ	    ಎಣಿಕೆ = 0;

			    // If a digit is following it could be a key with
			    // modifier, e.g., ESC [ 1;2P.  Can be confused
			    // with DEC_MOUSE, which requires four numbers
			    // following.  If not then it can't be a DEC_MOUSE
			    // code.
			    ಪ್ರತಿ (;;)
			    {
				++ಎಣಿಕೆ;
				(ಶೂನ್ಯ)getdigits(&nr);
				ಯದಿ (nr >= tp + len)
				    ಪ್ರತ್ಯಾಗಮ -1;	// partial sequence
				ಯದಿ (*nr != ';')
				    ಭಙ್ಗ;
				++nr;
				ಯದಿ (nr >= tp + len)
				    ಪ್ರತ್ಯಾಗಮ -1;	// partial sequence
			    }
			    ಯದಿ (ಎಣಿಕೆ < 4)
				ಅನುವರ್ತ;	// no match
			}
		    }
		    ಯದಿ (looks_like_mouse_start)
		    {
			// Only use it when there is no other match.
			ಯದಿ (mouse_index_found < 0)
			    mouse_index_found = idx;
		    }
		    ಅನ್ಯಥಾ ಯದಿ (!is_keypad)
		    {
			ಕೀಲಿ_name[0] = termcodes[idx].name[0];
			ಕೀಲಿ_name[1] = termcodes[idx].name[1];
			ಭಙ್ಗ;
		    }
		}

		/*
		 * Check for code with modifier, like xterm uses:
		 * <Esc>[123;*X  (modslen == slen - 3)
		 * <Esc>[@;*X    (matches <Esc>[X and <Esc>[1;9X )
		 * Also <Esc>O*X and <M-O>*X (modslen == slen - 2).
		 * When there is a modifier the * matches a number.
		 * When there is no modifier the ;* or * is omitted.
		 */
		ಯದಿ (termcodes[idx].modlen > 0 && mouse_index_found < 0)
		{
		    modslen = termcodes[idx].modlen;
		    ಯದಿ (cpo_koffset && offset && len < modslen)
			ಅನುವರ್ತ;
		    ಯದಿ (STRNCMP(termcodes[idx].code, tp,
				(size_t)(modslen > len ? len : modslen)) == 0)
		    {
			ಪೂರ್ಣಾಙ್ಕ	    n;

			ಯದಿ (len <= modslen)	// got a partial sequence
			    ಪ್ರತ್ಯಾಗಮ -1;		// need to get more chars

			ಯದಿ (tp[modslen] == termcodes[idx].code[slen - 1])
			    // no modifiers
			    slen = modslen + 1;
			ಅನ್ಯಥಾ ಯದಿ (tp[modslen] != ';' && modslen == slen - 3)
			    // no match for "code;*X" with "code;"
			    ಅನುವರ್ತ;
			ಅನ್ಯಥಾ ಯದಿ (termcodes[idx].code[modslen] == '@'
					 && (tp[modslen] != '1'
						    || tp[modslen + 1] != ';'))
			    // no match for "<Esc>[@" with "<Esc>[1;"
			    ಅನುವರ್ತ;
			ಅನ್ಯಥಾ
			{
			    // Skip over the digits, the final char must
			    // follow. URXVT can use a negative value, thus
			    // also accept '-'.
			    ಪ್ರತಿ (j = slen - 2; j < len && (SAFE_isdigit(tp[j])
				       || tp[j] == '-' || tp[j] == ';'); ++j)
				;
			    ++j;
			    ಯದಿ (len < j)	// got a partial sequence
				ಪ್ರತ್ಯಾಗಮ -1;	// need to get more chars
			    ಯದಿ (tp[j - 1] != termcodes[idx].code[slen - 1])
				ಅನುವರ್ತ;	// no match

			    modifiers_start = tp + slen - 2;

			    // Match!  Convert modifier bits.
			    n = atoi((ವರ್ಣ *)modifiers_start);
			    modifiers |= decode_modifiers(n);

			    slen = j;
			}
			ಯದಿ (termcodes[idx].name[0] == 'K'
					   && (VIM_ISDIGIT(termcodes[idx].name[1])
						    || ASCII_ISUPPER(termcodes[idx].name[1])))
			{
			    is_keypad = TRUE;
			    ಯದಿ (keypad_index_found < 0)
			    {
				keypad_index_found = idx;
				keypad_slen_found = slen;
			    }
			}
			ಯದಿ (!is_keypad)
			{
			    ಕೀಲಿ_name[0] = termcodes[idx].name[0];
			    ಕೀಲಿ_name[1] = termcodes[idx].name[1];
			    ಭಙ್ಗ;
			}
		    }
		}
	    }
	    ಯದಿ (idx == tc_len && keypad_index_found >= 0)
	    {
		ಕೀಲಿ_name[0] = termcodes[keypad_index_found].name[0];
		ಕೀಲಿ_name[1] = termcodes[keypad_index_found].name[1];
		slen = keypad_slen_found;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (idx == tc_len && mouse_index_found >= 0)
	    {
		ಕೀಲಿ_name[0] = termcodes[mouse_index_found].name[0];
		ಕೀಲಿ_name[1] = termcodes[mouse_index_found].name[1];
	    }
	}

	ಯದಿ (ಕೀಲಿ_name[0] == NUL
	    // Mouse codes of DEC and pterm start with <ESC>[.  When
	    // detecting the start of these mouse codes they might as well be
	    // another key code or terminal response.
#ifdef FEAT_MOUSE_DEC
	    || ಕೀಲಿ_name[0] == KS_DEC_MOUSE
#endif
#ifdef FEAT_MOUSE_PTERM
	    || ಕೀಲಿ_name[0] == KS_PTERM_MOUSE
#endif
	   )
	{
	    char_u *argp = tp[0] == ESC ? tp + 2 : tp + 1;

	    /*
	     * Check for responses from the terminal starting with {lead}:
	     * "<Esc>[" or CSI followed by [0-9>?].
	     * Also for function keys without a modifier:
	     * "<Esc>[" or CSI followed by [ABCDEFHPQRS].
	     *
	     * - Xterm version string: {lead}>{x};{vers};{y}c
	     *   Also eat other possible responses to t_RV, rxvt returns
	     *   "{lead}?1;2c".
	     *
	     * - Response to XTQMODKEYS: "{lead} > 4 ; Pv m".
	     *
	     * - Cursor position report: {lead}{row};{col}R
	     *   The final byte must be 'R'. It is used for checking the
	     *   ambiguous-width character state.
	     *
	     * - window position reply: {lead}3;{x};{y}t
	     *
	     * - key with modifiers when modifyOtherKeys is enabled:
	     *	    {lead}27;{modifier};{key}~
	     *	    {lead}{key};{modifier}u
	     */
	    ಯದಿ (((tp[0] == ESC && len >= 3 && tp[1] == '[')
			|| (tp[0] == CSI && len >= 2))
		    && vim_strchr((char_u *)"0123456789>?ABCDEFHPQRS",
								*argp) != ರಿಕ್ತ)
	    {
		ಪೂರ್ಣಾಙ್ಕ resp = handle_csi(tp, len, argp, offset, buf,
					     bufsize, buflen, ಕೀಲಿ_name, &slen);
		ಯದಿ (resp != 0)
		{
#ifdef DEBUG_TERMRESPONSE
		    ಯದಿ (resp == -1)
			LOG_TR1("Not enough characters for CSI sequence");
#endif
		    ಪ್ರತ್ಯಾಗಮ resp;
		}
	    }

	    // Check for OSC responses from terminal
	    ಅನ್ಯಥಾ ಯದಿ ((tp[0] == ESC && len >= 2 && tp[1] == ']') || tp[0] == OSC)
	    {
handle_osc:
		ಯದಿ (handle_osc(tp, len, ಕೀಲಿ_name, &slen) == FAIL)
		    ಪ್ರತ್ಯಾಗಮ -1;
	    }

	    // Check for key code response from xterm,
	    // starting with <Esc>P or DCS
	    // It would only be needed with this condition:
	    //	    (check_for_codes || rcs_status.tr_progress == STATUS_SENT)
	    // Now this is always done so that DCS codes don't mess up things.
	    ಅನ್ಯಥಾ ಯದಿ ((tp[0] == ESC && len >= 2 && tp[1] == 'P') || tp[0] == DCS)
	    {
		ಯದಿ (handle_dcs(tp, argp, len, ಕೀಲಿ_name, &slen) == FAIL)
		    ಪ್ರತ್ಯಾಗಮ -1;
	    }
	}

	ಯದಿ (ಕೀಲಿ_name[0] == NUL)
	    ಅನುವರ್ತ;	    // No match at this position, try next one

	// We only get here when we have a complete termcode match

#ಯದಿ defined(FEAT_GUI) || defined(MSWIN)
	/*
	 * For scroll events from the GUI or MS-Windows console, fetch the
	 * pointer coordinates so that we know which window to scroll later.
	 */
	ಯದಿ (TRUE
# ಯದಿ defined(FEAT_GUI) && !defined(MSWIN)
		&& gui.in_use
# endif
		&& ಕೀಲಿ_name[0] == (ಪೂರ್ಣಾಙ್ಕ)KS_EXTRA
		&& (ಕೀಲಿ_name[1] == (ಪೂರ್ಣಾಙ್ಕ)KE_X1MOUSE
		    || ಕೀಲಿ_name[1] == (ಪೂರ್ಣಾಙ್ಕ)KE_X2MOUSE
		    || ಕೀಲಿ_name[1] == (ಪೂರ್ಣಾಙ್ಕ)KE_MOUSEMOVE_XY
		    || ಕೀಲಿ_name[1] == (ಪೂರ್ಣಾಙ್ಕ)KE_MOUSELEFT
		    || ಕೀಲಿ_name[1] == (ಪೂರ್ಣಾಙ್ಕ)KE_MOUSERIGHT
		    || ಕೀಲಿ_name[1] == (ಪೂರ್ಣಾಙ್ಕ)KE_MOUSEDOWN
		    || ಕೀಲಿ_name[1] == (ಪೂರ್ಣಾಙ್ಕ)KE_MOUSEUP))
	{
	    char_u	bytes[6];
	    ಪೂರ್ಣಾಙ್ಕ		ಅಂಕೆ_bytes = get_bytes_from_buf(tp + slen, bytes, 4);

	    ಯದಿ (ಅಂಕೆ_bytes == -1)	// not enough coordinates
		ಪ್ರತ್ಯಾಗಮ -1;
	    mouse_col = 128 * (bytes[0] - ' ' - 1) + bytes[1] - ' ' - 1;
	    mouse_row = 128 * (bytes[2] - ' ' - 1) + bytes[3] - ' ' - 1;
	    slen += ಅಂಕೆ_bytes;
	    // equal to K_MOUSEMOVE
	    ಯದಿ (ಕೀಲಿ_name[1] == (ಪೂರ್ಣಾಙ್ಕ)KE_MOUSEMOVE_XY)
		ಕೀಲಿ_name[1] = (ಪೂರ್ಣಾಙ್ಕ)KE_MOUSEMOVE;
	}
	ಅನ್ಯಥಾ
#endif
	/*
	 * If it is a mouse click, get the coordinates.
	 */
	ಯದಿ (ಕೀಲಿ_name[0] == KS_MOUSE
#ifdef FEAT_MOUSE_GPM
		|| ಕೀಲಿ_name[0] == KS_GPM_MOUSE
#endif
#ifdef FEAT_MOUSE_JSB
		|| ಕೀಲಿ_name[0] == KS_JSBTERM_MOUSE
#endif
#ifdef FEAT_MOUSE_NET
		|| ಕೀಲಿ_name[0] == KS_NETTERM_MOUSE
#endif
#ifdef FEAT_MOUSE_DEC
		|| ಕೀಲಿ_name[0] == KS_DEC_MOUSE
#endif
#ifdef FEAT_MOUSE_PTERM
		|| ಕೀಲಿ_name[0] == KS_PTERM_MOUSE
#endif
#ifdef FEAT_MOUSE_URXVT
		|| ಕೀಲಿ_name[0] == KS_URXVT_MOUSE
#endif
		|| ಕೀಲಿ_name[0] == KS_SGR_MOUSE
		|| ಕೀಲಿ_name[0] == KS_SGR_MOUSE_RELEASE)
	{
	    ಯದಿ (check_termcode_mouse(tp, &slen, ಕೀಲಿ_name, modifiers_start, idx,
							     &modifiers) == -1)
		ಪ್ರತ್ಯಾಗಮ -1;
	}

#ifdef FEAT_GUI
	/*
	 * If using the GUI, then we get menu and scrollbar events.
	 *
	 * A menu event is encoded as K_SPECIAL, KS_MENU, KE_FILLER followed by
	 * four bytes which are to be taken as a pointer to the vimmenu_T
	 * structure.
	 *
	 * A tab line event is encoded as K_SPECIAL KS_TABLINE nr, where "nr"
	 * is one byte with the tab index.
	 *
	 * A scrollbar event is K_SPECIAL, KS_VER_SCROLLBAR, KE_FILLER followed
	 * by one byte representing the scrollbar number, and then four bytes
	 * representing a long_u which is the new value of the scrollbar.
	 *
	 * A horizontal scrollbar event is K_SPECIAL, KS_HOR_SCROLLBAR,
	 * KE_FILLER followed by four bytes representing a long_u which is the
	 * new value of the scrollbar.
	 */
# ifdef FEAT_MENU
	ಅನ್ಯಥಾ ಯದಿ (ಕೀಲಿ_name[0] == (ಪೂರ್ಣಾಙ್ಕ)KS_MENU)
	{
	    long_u	val;
	    ಪೂರ್ಣಾಙ್ಕ		ಅಂಕೆ_bytes = get_long_from_buf(tp + slen, &val);

	    ಯದಿ (ಅಂಕೆ_bytes == -1)
		ಪ್ರತ್ಯಾಗಮ -1;
	    current_menu = (vimmenu_T *)val;
	    slen += ಅಂಕೆ_bytes;

	    // The menu may have been deleted right after it was used, check
	    // for that.
	    ಯದಿ (check_menu_pointer(ಬೇರು_menu, current_menu) == FAIL)
	    {
		ಕೀಲಿ_name[0] = KS_EXTRA;
		ಕೀಲಿ_name[1] = (ಪೂರ್ಣಾಙ್ಕ)KE_IGNORE;
	    }
	}
# endif
# ifdef FEAT_GUI_TABLINE
	ಅನ್ಯಥಾ ಯದಿ (ಕೀಲಿ_name[0] == (ಪೂರ್ಣಾಙ್ಕ)KS_TABLINE)
	{
	    // Selecting tabline tab or using its menu.
	    char_u	bytes[6];
	    ಪೂರ್ಣಾಙ್ಕ		ಅಂಕೆ_bytes = get_bytes_from_buf(tp + slen, bytes, 1);

	    ಯದಿ (ಅಂಕೆ_bytes == -1)
		ಪ್ರತ್ಯಾಗಮ -1;
	    current_tab = (ಪೂರ್ಣಾಙ್ಕ)bytes[0];
	    ಯದಿ (current_tab == 255)	// -1 in a byte gives 255
		current_tab = -1;
	    slen += ಅಂಕೆ_bytes;
	}
	ಅನ್ಯಥಾ ಯದಿ (ಕೀಲಿ_name[0] == (ಪೂರ್ಣಾಙ್ಕ)KS_TABMENU)
	{
	    // Selecting tabline tab or using its menu.
	    char_u	bytes[6];
	    ಪೂರ್ಣಾಙ್ಕ		ಅಂಕೆ_bytes = get_bytes_from_buf(tp + slen, bytes, 2);

	    ಯದಿ (ಅಂಕೆ_bytes == -1)
		ಪ್ರತ್ಯಾಗಮ -1;
	    current_tab = (ಪೂರ್ಣಾಙ್ಕ)bytes[0];
	    current_tabmenu = (ಪೂರ್ಣಾಙ್ಕ)bytes[1];
	    slen += ಅಂಕೆ_bytes;
	}
# endif
# ifndef USE_ON_FLY_SCROLL
	ಅನ್ಯಥಾ ಯದಿ (ಕೀಲಿ_name[0] == (ಪೂರ್ಣಾಙ್ಕ)KS_VER_SCROLLBAR)
	{
	    long_u	val;
	    char_u	bytes[6];
	    ಪೂರ್ಣಾಙ್ಕ		ಅಂಕೆ_bytes;

	    // Get the last scrollbar event in the queue of the same type
	    j = 0;
	    ಪ್ರತಿ (i = 0; tp[j] == CSI && tp[j + 1] == KS_VER_SCROLLBAR
						     && tp[j + 2] != NUL; ++i)
	    {
		j += 3;
		ಅಂಕೆ_bytes = get_bytes_from_buf(tp + j, bytes, 1);
		ಯದಿ (ಅಂಕೆ_bytes == -1)
		    ಭಙ್ಗ;
		ಯದಿ (i == 0)
		    current_scrollbar = (ಪೂರ್ಣಾಙ್ಕ)bytes[0];
		ಅನ್ಯಥಾ ಯದಿ (current_scrollbar != (ಪೂರ್ಣಾಙ್ಕ)bytes[0])
		    ಭಙ್ಗ;
		j += ಅಂಕೆ_bytes;
		ಅಂಕೆ_bytes = get_long_from_buf(tp + j, &val);
		ಯದಿ (ಅಂಕೆ_bytes == -1)
		    ಭಙ್ಗ;
		scrollbar_ಮೌಲ್ಯ = val;
		j += ಅಂಕೆ_bytes;
		slen = j;
	    }
	    ಯದಿ (i == 0)		// not enough characters to make one
		ಪ್ರತ್ಯಾಗಮ -1;
	}
	ಅನ್ಯಥಾ ಯದಿ (ಕೀಲಿ_name[0] == (ಪೂರ್ಣಾಙ್ಕ)KS_HOR_SCROLLBAR)
	{
	    long_u	val;
	    ಪೂರ್ಣಾಙ್ಕ		ಅಂಕೆ_bytes;

	    // Get the last horiz. scrollbar event in the queue
	    j = 0;
	    ಪ್ರತಿ (i = 0; tp[j] == CSI && tp[j + 1] == KS_HOR_SCROLLBAR
						     && tp[j + 2] != NUL; ++i)
	    {
		j += 3;
		ಅಂಕೆ_bytes = get_long_from_buf(tp + j, &val);
		ಯದಿ (ಅಂಕೆ_bytes == -1)
		    ಭಙ್ಗ;
		scrollbar_ಮೌಲ್ಯ = val;
		j += ಅಂಕೆ_bytes;
		slen = j;
	    }
	    ಯದಿ (i == 0)		// not enough characters to make one
		ಪ್ರತ್ಯಾಗಮ -1;
	}
# endif // !USE_ON_FLY_SCROLL
#endif // FEAT_GUI

#ಯದಿ defined(UNIX) || defined(VMS)
	/*
	 * Handle FocusIn/FocusOut event sequences reported by XTerm.
	 * (CSI I/CSI O)
	 */
	ಯದಿ (ಕೀಲಿ_name[0] == KS_EXTRA
# ifdef FEAT_GUI
		&& !gui.in_use
# endif
	    )
	{
	    ಯದಿ (ಕೀಲಿ_name[1] == KE_FOCUSGAINED)
	    {
		ಯದಿ (!focus_state)
		{
		    ui_focus_change(TRUE);
		    did_cursorhold = TRUE;
		    focus_state = TRUE;
		}
		ಕೀಲಿ_name[1] = (ಪೂರ್ಣಾಙ್ಕ)KE_IGNORE;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (ಕೀಲಿ_name[1] == KE_FOCUSLOST)
	    {
		ಯದಿ (focus_state)
		{
		    ui_focus_change(FALSE);
		    did_cursorhold = TRUE;
		    focus_state = FALSE;
		}
		ಕೀಲಿ_name[1] = (ಪೂರ್ಣಾಙ್ಕ)KE_IGNORE;
	    }
	}
#endif

	/*
	 * Change <xHome> to <Home>, <xUp> to <Up>, etc.
	 */
	ಕೀಲಿ = handle_x_ಕೀಲಿಗಳು(TERMCAP2_ಕೀಲಿ(ಕೀಲಿ_name[0], ಕೀಲಿ_name[1]));

	// Add any modifier codes to our string.
	new_slen = modifiers2keycode(modifiers, &ಕೀಲಿ, string);

	// Finally, add the special key code to our string
	ಕೀಲಿ_name[0] = KEY2TERMCAP0(ಕೀಲಿ);
	ಕೀಲಿ_name[1] = KEY2TERMCAP1(ಕೀಲಿ);
	ಯದಿ (ಕೀಲಿ_name[0] == KS_ಕೀಲಿ)
	{
	    // from ":set <M-b>=xx"
	    ಯದಿ (has_mbyte)
		new_slen += (*mb_char2bytes)(ಕೀಲಿ_name[1], string + new_slen);
	    ಅನ್ಯಥಾ
		string[new_slen++] = ಕೀಲಿ_name[1];
	}
	ಅನ್ಯಥಾ ಯದಿ (osc_state.processing ||
		(new_slen == 0
		 && ಕೀಲಿ_name[0] == KS_EXTRA
		 && ಕೀಲಿ_name[1] == KE_IGNORE))
	{
	    // Do not put K_IGNORE into the buffer, do return KEYLEN_REMOVED
	    // to indicate what happened.
	    retval = KEYLEN_REMOVED;
	}
	ಅನ್ಯಥಾ
	{
	    string[new_slen++] = K_SPECIAL;
	    string[new_slen++] = ಕೀಲಿ_name[0];
	    string[new_slen++] = ಕೀಲಿ_name[1];
	}
	ಯದಿ (put_string_in_typebuf(offset, slen, string, new_slen,
						 buf, bufsize, buflen) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ -1;
	ಪ್ರತ್ಯಾಗಮ retval == 0 ? (len + new_slen - slen + offset) : retval;
    }

#ifdef FEAT_TERMRESPONSE
    LOG_TR1("normal character");
#endif

    ಪ್ರತ್ಯಾಗಮ 0;			    // no match found
}

#ಯದಿ defined(FEAT_TERMINAL) && defined(FEAT_TERMRESPONSE)
/*
 * Get the text foreground color, if known.
 */
    ಶೂನ್ಯ
term_get_fg_color(char_u *r, char_u *g, char_u *b)
{
    ಯದಿ (rfg_status.tr_progress != STATUS_GOT)
	ಪ್ರತ್ಯಾಗಮ;

    *r = fg_r;
    *g = fg_g;
    *b = fg_b;
}
#endif

#ifdef FEAT_TERMRESPONSE
/*
 * Get the text background color, if known.  Returns OK when the OSC 11
 * response has been parsed and *r, *g, *b are populated; FAIL otherwise.
 */
    ಪೂರ್ಣಾಙ್ಕ
term_get_bg_color(char_u *r, char_u *g, char_u *b)
{
    ಯದಿ (rbg_status.tr_progress != STATUS_GOT)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    *r = bg_r;
    *g = bg_g;
    *b = bg_b;
    ಪ್ರತ್ಯಾಗಮ OK;
}
#endif

/*
 * Replace any terminal code strings in from[] with the equivalent internal
 * vim representation.	This is used for the "from" and "to" part of a
 * mapping, and the "to" part of a menu command.
 * Any strings like "<C-UP>" are also replaced, unless 'cpoptions' contains
 * '<'.
 * K_SPECIAL by itself is replaced by K_SPECIAL KS_SPECIAL KE_FILLER.
 *
 * The replacement is done in result[] and finally copied into allocated
 * memory. If this all works well *bufp is set to the allocated memory and a
 * pointer to it is returned. If something fails *bufp is set to NULL and from
 * is returned.
 *
 * CTRL-V characters are removed.  When "flags" has REPTERM_FROM_PART, a
 * trailing CTRL-V is included, otherwise it is removed (for ":map xx ^V", maps
 * xx to nothing).  When 'cpoptions' does not contain 'B', a backslash can be
 * used instead of a CTRL-V.
 *
 * Flags:
 *  REPTERM_FROM_PART	see above
 *  REPTERM_DO_LT	also translate <lt>
 *  REPTERM_SPECIAL	always accept <key> notation
 *  REPTERM_NO_SIMPLIFY	do not simplify <C-H> to 0x08 and set 8th bit for <A-x>
 *
 * "did_simplify" is set when some <C-H> or <A-x> code was simplified, unless
 * it is NULL.
 */
    char_u *
replace_termcodes(
    char_u	*from,
    char_u	**bufp,
    scid_T	sid_arg UNUSED,	// script ID to use for <SID>,
				// or 0 to use current_sctx
    ಪೂರ್ಣಾಙ್ಕ		flags,
    ಪೂರ್ಣಾಙ್ಕ		*did_simplify)
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		slen;
    ಪೂರ್ಣಾಙ್ಕ		ಕೀಲಿ;
    size_t	dlen = 0;
    char_u	*src;
    ಪೂರ್ಣಾಙ್ಕ		do_backslash;	// backslash is a special character
    ಪೂರ್ಣಾಙ್ಕ		do_special;	// recognize <> key codes
    ಪೂರ್ಣಾಙ್ಕ		do_ಕೀಲಿ_code;	// recognize raw key codes
    char_u	*result;	// buffer for resulting string
    garray_T	ga;

    do_backslash = (vim_strchr(p_cpo, CPO_BSLASH) == ರಿಕ್ತ);
    do_special = (vim_strchr(p_cpo, CPO_SPECI) == ರಿಕ್ತ)
						  || (flags & REPTERM_SPECIAL);
    do_ಕೀಲಿ_code = (vim_strchr(p_cpo, CPO_KEYCODE) == ರಿಕ್ತ);
    src = from;

    /*
     * Allocate space for the translation.  Worst case a single character is
     * replaced by 6 bytes (shifted special key), plus a NUL at the end.
     * In the rare case more might be needed ga_grow() must be called again.
     */
    ga_init2(&ga, 1L, 100);
    ಯದಿ (ga_grow(&ga, (ಪೂರ್ಣಾಙ್ಕ)(STRLEN(src) * 6 + 1)) == FAIL) // out of memory
    {
	*bufp = ರಿಕ್ತ;
	ಪ್ರತ್ಯಾಗಮ from;
    }
    result = ga.ga_data;

    /*
     * Check for #n at start only: function key n
     */
    ಯದಿ ((flags & REPTERM_FROM_PART) && src[0] == '#' && VIM_ISDIGIT(src[1]))
    {
	result[dlen++] = K_SPECIAL;
	result[dlen++] = 'k';
	ಯದಿ (src[1] == '0')
	    result[dlen++] = ';';	// #0 is F10 is "k;"
	ಅನ್ಯಥಾ
	    result[dlen++] = src[1];	// #3 is F3 is "k3"
	src += 2;
    }

    /*
     * Copy each byte from *from to result[dlen]
     */
    ಯಾವತ್ (*src != NUL)
    {
	/*
	 * If 'cpoptions' does not contain '<', check for special key codes,
	 * like "<C-S-LeftMouse>"
	 */
	ಯದಿ (do_special && ((flags & REPTERM_DO_LT)
					      || STRNCMP(src, "<lt>", 4) != 0))
	{
#ifdef FEAT_EVAL
	    /*
	     * Change <SID>Func to K_SNR <script-nr> _Func.  This name is used
	     * for script-local user functions.
	     * (room: 5 * 6 = 30 bytes; needed: 3 + <nr> + 1 <= 14)
	     * Also change <SID>name.Func to K_SNR <import-script-nr> _Func.
	     * Only if "name" is recognized as an import.
	     */
	    ಯದಿ (STRNICMP(src, "<SID>", 5) == 0)
	    {
		ಯದಿ (sid_arg < 0 || (sid_arg == 0 && current_sctx.sc_sid <= 0))
		    emsg(_(e_using_sid_not_in_script_context));
		ಅನ್ಯಥಾ
		{
		    char_u  *dot;
		    long    sid = sid_arg != 0 ? sid_arg : current_sctx.sc_sid;

		    src += 5;
		    ಯದಿ (in_vim9script()
				       && (dot = vim_strchr(src, '.')) != ರಿಕ್ತ)
		    {
			imported_T *imp = find_imported(src, dot - src, FALSE);

			ಯದಿ (imp != ರಿಕ್ತ)
			{
			    scriptitem_T    *si = SCRIPT_ITEM(imp->imp_sid);
			    size_t	    len;

			    src = dot + 1;
			    ಯದಿ (si->sn_autoload_prefix != ರಿಕ್ತ)
			    {
				// Turn "<SID>name.Func"
				// into "scriptname#Func".
				len = STRLEN(si->sn_autoload_prefix);
				ಯದಿ (ga_grow(&ga,
				     (ಪೂರ್ಣಾಙ್ಕ)(STRLEN(src) * 6 + len + 1)) == FAIL)
				{
				    ga_clear(&ga);
				    *bufp = ರಿಕ್ತ;
				    ಪ್ರತ್ಯಾಗಮ from;
				}
				result = ga.ga_data;
				STRCPY(result + dlen, si->sn_autoload_prefix);
				dlen += len;
				ಅನುವರ್ತ;
			    }
			    sid = imp->imp_sid;
			}
		    }

		    result[dlen++] = K_SPECIAL;
		    result[dlen++] = (ಪೂರ್ಣಾಙ್ಕ)KS_EXTRA;
		    result[dlen++] = (ಪೂರ್ಣಾಙ್ಕ)KE_SNR;
		    sprintf((ವರ್ಣ *)result + dlen, "%ld", sid);
		    dlen += STRLEN(result + dlen);
		    result[dlen++] = '_';
		    ಅನುವರ್ತ;
		}
	    }
#endif
	    ಪೂರ್ಣಾಙ್ಕ fsk_flags = FSK_KEYCODE
			| ((flags & REPTERM_NO_SIMPLIFY) ? 0 : FSK_SIMPLIFY)
			| ((flags & REPTERM_FROM_PART) ? FSK_FROM_PART : 0);
	    slen = trans_special(&src, result + dlen, fsk_flags,
							   TRUE, did_simplify);
	    ಯದಿ (slen > 0)
	    {
		dlen += slen;
		ಅನುವರ್ತ;
	    }
	}

	/*
	 * If 'cpoptions' does not contain 'k', see if it's an actual key-code.
	 * Note that this is also checked after replacing the <> form.
	 * Single character codes are NOT replaced (e.g. ^H or DEL), because
	 * it could be a character in the file.
	 */
	ಯದಿ (do_ಕೀಲಿ_code)
	{
	    ಪೂರ್ಣಾಙ್ಕ len;
	    i = find_term_bykeys(src, &len);
	    ಯದಿ (i >= 0)
	    {
		result[dlen++] = K_SPECIAL;
		result[dlen++] = termcodes[i].name[0];
		result[dlen++] = termcodes[i].name[1];
		src += len;
		// If terminal code matched, continue after it.
		ಅನುವರ್ತ;
	    }
	}

#ifdef FEAT_EVAL
	ಯದಿ (do_special)
	{
	    char_u	*p, *s, len;

	    /*
	     * Replace <Leader> by the value of "mapleader".
	     * Replace <LocalLeader> by the value of "maplocalleader".
	     * If "mapleader" or "maplocalleader" isn't set use a backslash.
	     */
	    ಯದಿ (STRNICMP(src, "<Leader>", 8) == 0)
	    {
		len = 8;
		p = get_var_ಮೌಲ್ಯ((char_u *)"g:mapleader");
	    }
	    ಅನ್ಯಥಾ ಯದಿ (STRNICMP(src, "<LocalLeader>", 13) == 0)
	    {
		len = 13;
		p = get_var_ಮೌಲ್ಯ((char_u *)"g:maplocalleader");
	    }
	    ಅನ್ಯಥಾ
	    {
		len = 0;
		p = ರಿಕ್ತ;
	    }
	    ಯದಿ (len != 0)
	    {
		// Allow up to 8 * 6 characters for "mapleader".
		ಯದಿ (p == ರಿಕ್ತ || *p == NUL || STRLEN(p) > 8 * 6)
		    s = (char_u *)"\\";
		ಅನ್ಯಥಾ
		    s = p;
		ಯಾವತ್ (*s != NUL)
		    result[dlen++] = *s++;
		src += len;
		ಅನುವರ್ತ;
	    }
	}
#endif

	/*
	 * Remove CTRL-V and ignore the next character.
	 * For "from" side the CTRL-V at the end is included, for the "to"
	 * part it is removed.
	 * If 'cpoptions' does not contain 'B', also accept a backslash.
	 */
	ಕೀಲಿ = *src;
	ಯದಿ (ಕೀಲಿ == Ctrl_V || (do_backslash && ಕೀಲಿ == '\\'))
	{
	    ++src;				// skip CTRL-V or backslash
	    ಯದಿ (*src == NUL)
	    {
		ಯದಿ (flags & REPTERM_FROM_PART)
		    result[dlen++] = ಕೀಲಿ;
		ಭಙ್ಗ;
	    }
	}

	// skip multibyte char correctly
	ಪ್ರತಿ (i = (*mb_ptr2len)(src); i > 0; --i)
	{
	    /*
	     * If the character is K_SPECIAL, replace it with K_SPECIAL
	     * KS_SPECIAL KE_FILLER.
	     * If compiled with the GUI replace CSI with K_CSI.
	     */
	    ಯದಿ (*src == K_SPECIAL)
	    {
		result[dlen++] = K_SPECIAL;
		result[dlen++] = KS_SPECIAL;
		result[dlen++] = KE_FILLER;
	    }
#ifdef FEAT_GUI
	    ಅನ್ಯಥಾ ಯದಿ (*src == CSI)
	    {
		result[dlen++] = K_SPECIAL;
		result[dlen++] = KS_EXTRA;
		result[dlen++] = (ಪೂರ್ಣಾಙ್ಕ)KE_CSI;
	    }
#endif
	    ಅನ್ಯಥಾ
		result[dlen++] = *src;
	    ++src;
	}
    }
    result[dlen] = NUL;

    /*
     * Copy the new string to allocated memory.
     * If this fails, just return from.
     */
    ಯದಿ ((*bufp = vim_strsave(result)) != ರಿಕ್ತ)
	from = *bufp;
    vim_free(result);
    ಪ್ರತ್ಯಾಗಮ from;
}

/*
 * Find a termcode with keys 'src' (must be NUL terminated).
 * Return the index in termcodes[], or -1 if not found.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
find_term_bykeys(char_u *src, ಪೂರ್ಣಾಙ್ಕ *matchlen)
{
    ಪೂರ್ಣಾಙ್ಕ		i, j;
    ಪೂರ್ಣಾಙ್ಕ		len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(src);
    ಪೂರ್ಣಾಙ್ಕ         found = -1;
    // Don't return a match for a single character
    ಪೂರ್ಣಾಙ್ಕ         foundlen = 1;
    ಪೂರ್ಣಾಙ್ಕ         slen, modslen;
    ಪೂರ್ಣಾಙ್ಕ         thislen;

    // Most input bytes cannot start a terminal code.  Reuse the same leader
    // table as check_termcode() to avoid scanning termcodes[] unnecessarily.
    ಯದಿ (need_gather)
	gather_termleader();
    ಯದಿ (*src == NUL || vim_strchr(termleader, *src) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ -1;

    // find longest match
    // borrows part of check_termcode
    ಪ್ರತಿ (i = 0; i < tc_len; ++i)
    {
	slen = termcodes[i].len;
	modslen = termcodes[i].modlen;

	/*
	 * Check for code with modifier, like xterm uses:
	 * <Esc>[123;*X  (modslen == slen - 3)
	 * <Esc>[@;*X    (matches <Esc>[X and <Esc>[1;9X )
	 * Also <Esc>O*X and <M-O>*X (modslen == slen - 2).
	 * When there is a modifier the * matches a number.
	 * When there is no modifier the ;* or * is omitted.
	 */
	ಯದಿ (modslen > 0)
	{
	    ಯದಿ (len > modslen
			&& STRNCMP(termcodes[i].code, src, (size_t)modslen) == 0)
	    {
		thislen = 0;

		ಯದಿ (src[modslen] == termcodes[i].code[slen - 1])
		    // no modifiers
		    thislen = modslen + 1;
		ಅನ್ಯಥಾ ಯದಿ (src[modslen] != ';' && modslen == slen - 3)
		    // no match for "code;*X" with "code;"
		    ಅನುವರ್ತ;
		ಅನ್ಯಥಾ ಯದಿ (termcodes[i].code[modslen] == '@'
				&& (src[modslen] != '1'
					    || src[modslen + 1] != ';'))
		    // no match for "<Esc>[@" with "<Esc>[1;"
		    ಅನುವರ್ತ;
		ಅನ್ಯಥಾ
		{
		    // Skip over the digits, the final char must
		    // follow. URXVT can use a negative value, thus
		    // also accept '-'.
		    ಪ್ರತಿ (j = slen - 2; j < len && (SAFE_isdigit(src[j])
			       || src[j] == '-' || src[j] == ';'); ++j)
			;
		    ++j;
		    ಯದಿ (len < j)	// got a partial sequence
			ಅನುವರ್ತ;
		    ಯದಿ (src[j - 1] != termcodes[i].code[slen - 1])
			ಅನುವರ್ತ;	// no match

		    thislen = j;
		}

		ಯದಿ (thislen > foundlen)
		{
		    found = i;
		    foundlen = thislen;
		}
	    }
	}
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (slen > foundlen && len >= slen
			&& STRNCMP(termcodes[i].code, src, (size_t)slen) == 0)
	    {
		found = i;
		foundlen = slen;
	    }
	}
    }

    ಯದಿ (matchlen != ರಿಕ್ತ && found >= 0)
	*matchlen = foundlen;

    ಪ್ರತ್ಯಾಗಮ found;
}

/*
 * Gather the first characters in the terminal key codes into a string.
 * Used to speed up check_termcode().
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
gather_termleader(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ	    i;
    ಪೂರ್ಣಾಙ್ಕ	    len = 0;

#ifdef FEAT_GUI
    ಯದಿ (gui.in_use)
	termleader[len++] = CSI;    // the GUI codes are not in termcodes[]
#endif

    // Querying cell size uses CSI 14 t
#ಯದಿ defined(MSWIN) && (defined(FEAT_IMAGE) || defined(FEAT_EVAL))
# ifdef FEAT_GUI
    ಯದಿ (!gui.in_use)
# endif
    {
	termleader[len++] = ESC;
# ifdef FEAT_GUI
	ಯದಿ (!gui.in_use)
# endif
	    termleader[len++] = CSI;
    }
#endif

#ifdef FEAT_TERMRESPONSE
    ಯದಿ (check_for_codes || *T_CRS != NUL)
	termleader[len++] = DCS;    // the termcode response starts with DCS
				    // in 8-bit mode
#endif
    termleader[len] = NUL;

    ಪ್ರತಿ (i = 0; i < tc_len; ++i)
	ಯದಿ (vim_strchr(termleader, termcodes[i].code[0]) == ರಿಕ್ತ)
	{
	    termleader[len++] = termcodes[i].code[0];
	    termleader[len] = NUL;
	}

    need_gather = FALSE;
}

/*
 * Show all termcodes (for ":set termcap")
 * This code looks a lot like showoptions(), but is different.
 * "flags" can have OPT_ONECOLUMN.
 */
    ಶೂನ್ಯ
show_termcodes(ಪೂರ್ಣಾಙ್ಕ flags)
{
    ಪೂರ್ಣಾಙ್ಕ		col;
    ಪೂರ್ಣಾಙ್ಕ		*ವಸ್ತುಗಳು;
    ಪೂರ್ಣಾಙ್ಕ		item_ಎಣಿಕೆ;
    ಪೂರ್ಣಾಙ್ಕ		run;
    ಪೂರ್ಣಾಙ್ಕ		row, rows;
    ಪೂರ್ಣಾಙ್ಕ		cols;
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		len;

#define INC3 27	    // try to make three columns
#define INC2 40	    // try to make two columns
#define GAP 2	    // spaces between columns

    ಯದಿ (tc_len == 0)	    // no terminal codes (must be GUI)
	ಪ್ರತ್ಯಾಗಮ;
    ವಸ್ತುಗಳು = ALLOC_MULT(ಪೂರ್ಣಾಙ್ಕ, tc_len);
    ಯದಿ (ವಸ್ತುಗಳು == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    // Highlight title
    ಸಂದೇಶ_puts_title(_("\n--- Terminal keys ---"));

    /*
     * Do the loop three times:
     * 1. display the short items (non-strings and short strings)
     * 2. display the medium items (medium length strings)
     * 3. display the long items (remaining strings)
     * When "flags" has OPT_ONECOLUMN do everything in 3.
     */
    ಪ್ರತಿ (run = (flags & OPT_ONECOLUMN) ? 3 : 1; run <= 3 && !got_int; ++run)
    {
	/*
	 * collect the items in items[]
	 */
	item_ಎಣಿಕೆ = 0;
	ಪ್ರತಿ (i = 0; i < tc_len; i++)
	{
	    len = show_one_termcode(termcodes[i].name,
						    termcodes[i].code, FALSE);
	    ಯದಿ ((flags & OPT_ONECOLUMN) ||
		    (len <= INC3 - GAP ? run == 1
			: len <= INC2 - GAP ? run == 2
			: run == 3))
		ವಸ್ತುಗಳು[item_ಎಣಿಕೆ++] = i;
	}

	/*
	 * display the items
	 */
	ಯದಿ (run <= 2)
	{
	    cols = (Columns + GAP) / (run == 1 ? INC3 : INC2);
	    ಯದಿ (cols == 0)
		cols = 1;
	    rows = (item_ಎಣಿಕೆ + cols - 1) / cols;
	}
	ಅನ್ಯಥಾ	// run == 3
	    rows = item_ಎಣಿಕೆ;
	ಪ್ರತಿ (row = 0; row < rows && !got_int; ++row)
	{
	    ಸಂದೇಶ_putchar('\n');			// go to next line
	    ಯದಿ (got_int)			// 'q' typed in more
		ಭಙ್ಗ;
	    col = 0;
	    ಪ್ರತಿ (i = row; i < item_ಎಣಿಕೆ; i += rows)
	    {
		ಸಂದೇಶ_col = col;			// make columns
		show_one_termcode(termcodes[ವಸ್ತುಗಳು[i]].name,
					      termcodes[ವಸ್ತುಗಳು[i]].code, TRUE);
		ಯದಿ (run == 2)
		    col += INC2;
		ಅನ್ಯಥಾ
		    col += INC3;
	    }
	    out_flush();
	    ui_breakcheck();
	}
    }
    vim_free(ವಸ್ತುಗಳು);
}

/*
 * Show one termcode entry.
 * Output goes into IObuff[]
 */
    ಪೂರ್ಣಾಙ್ಕ
show_one_termcode(char_u *name, char_u *code, ಪೂರ್ಣಾಙ್ಕ printit)
{
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		len;

    ಯದಿ (name[0] > '~')
    {
	IObuff[0] = ' ';
	IObuff[1] = ' ';
	IObuff[2] = ' ';
	IObuff[3] = ' ';
    }
    ಅನ್ಯಥಾ
    {
	IObuff[0] = 't';
	IObuff[1] = '_';
	IObuff[2] = name[0];
	IObuff[3] = name[1];
    }
    IObuff[4] = ' ';

    p = get_special_ಕೀಲಿ_name(TERMCAP2_ಕೀಲಿ(name[0], name[1]), 0);
    ಯದಿ (p[1] != 't')
	STRCPY(IObuff + 5, p);
    ಅನ್ಯಥಾ
	IObuff[5] = NUL;
    len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(IObuff);
    ಕುರು
	IObuff[len++] = ' ';
    ಯಾವತ್ (len < 17);
    IObuff[len] = NUL;
    ಯದಿ (code == ರಿಕ್ತ)
	len += 4;
    ಅನ್ಯಥಾ
	len += vim_strsize(code);

    ಯದಿ (printit)
    {
	ಸಂದೇಶ_puts((ವರ್ಣ *)IObuff);
	ಯದಿ (code == ರಿಕ್ತ)
	    ಸಂದೇಶ_puts("NULL");
	ಅನ್ಯಥಾ
	    ಸಂದೇಶ_outtrans(code);
    }
    ಪ್ರತ್ಯಾಗಮ len;
}

#ಯದಿ defined(FEAT_TERMRESPONSE)
/*
 * For Xterm >= 140 compiled with OPT_TCAP_QUERY: Obtain the actually used
 * termcap codes from the terminal itself.
 * We get them one by one to avoid a very long response string.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ xt_index_in = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ xt_index_out = 0;

    ಸ್ಥಿರ ಶೂನ್ಯ
req_codes_from_term(ಶೂನ್ಯ)
{
    xt_index_out = 0;
    xt_index_in = 0;
    req_more_codes_from_term();
}

    ಸ್ಥಿರ ಶೂನ್ಯ
req_more_codes_from_term(ಶೂನ್ಯ)
{
    ವರ್ಣ	buf[32];  // extra size to shut up LGTM
    ಪೂರ್ಣಾಙ್ಕ		old_idx = xt_index_out;

    // Don't do anything when going to exit.
    ಯದಿ (exiting)
	ಪ್ರತ್ಯಾಗಮ;

    // Send up to 10 more requests out than we received.  Avoid sending too
    // many, there can be a buffer overflow somewhere.
    ಯಾವತ್ (xt_index_out < xt_index_in + 10 && ಕೀಲಿ_names[xt_index_out] != ರಿಕ್ತ)
    {
	ವರ್ಣ *ಕೀಲಿ_name = ಕೀಲಿ_names[xt_index_out];

	MAY_WANT_TO_LOG_THIS;
	LOG_TRN("Requesting XT %d: %s", xt_index_out, ಕೀಲಿ_name);
	ಯದಿ (ಕೀಲಿ_name[2] != NUL)
	    sprintf(buf, "\033P+q%02x%02x%02x\033\\", ಕೀಲಿ_name[0], ಕೀಲಿ_name[1], ಕೀಲಿ_name[2]);
	ಅನ್ಯಥಾ
	    sprintf(buf, "\033P+q%02x%02x\033\\", ಕೀಲಿ_name[0], ಕೀಲಿ_name[1]);
	out_str_nf((char_u *)buf);
	++xt_index_out;
    }

    // Send the codes out right away.
    ಯದಿ (xt_index_out != old_idx)
	out_flush();
}

/*
 * Decode key code response from xterm:
 * '<Esc>P1+r<name>=<string><Esc>\' if it is enabled/supported
 * '<Esc>P0+r<Esc>\'                if it not enabled
 * A "0" instead of the "1" indicates a code that isn't supported.
 * Both <name> and <string> are encoded in hex.
 * "code" points to the "0" or "1".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
got_code_from_term(char_u *code, ಪೂರ್ಣಾಙ್ಕ len)
{
# define XT_LEN 100
    char_u	name[4];
    char_u	str[XT_LEN];
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		j = 0;
    ಪೂರ್ಣಾಙ್ಕ		c;

    // A '1' means the code is supported, a '0' means it isn't.
    // If it is supported, there must be a '=' following
    // When half the length is > XT_LEN we can't use it.
    ಯದಿ (code[0] == '1' && (code[7] == '=' || code[9] == '=') && len / 2 < XT_LEN)
    {
	// Get the name from the response and find it in the table.
	name[0] = hexhex2nr(code + 3);
	name[1] = hexhex2nr(code + 5);
	ಯದಿ (code[9] == '=')
	    name[2] = hexhex2nr(code + 7);
	ಅನ್ಯಥಾ
	    name[2] = NUL;
	name[3] = NUL;
	ಪ್ರತಿ (i = 0; ಕೀಲಿ_names[i] != ರಿಕ್ತ; ++i)
	{
	    ಯದಿ (STRCMP(ಕೀಲಿ_names[i], name) == 0)
	    {
		xt_index_in = i;
		ಭಙ್ಗ;
	    }
	}

	LOG_TRN("Received XT %d: %s", xt_index_in, (ವರ್ಣ *)name);

	ಯದಿ (ಕೀಲಿ_names[i] != ರಿಕ್ತ)
	{
	    i = (code[7] == '=') ? 8 : 10;
	    ಪ್ರತಿ (; (c = hexhex2nr(code + i)) >= 0; i += 2)
		str[j++] = c;
	    str[j] = NUL;
	    ಯದಿ (name[0] == 'C' && name[1] == 'o')
	    {
		// Color count is not a key code.
		ಪೂರ್ಣಾಙ್ಕ val = atoi((ವರ್ಣ *)str);
# ಯದಿ defined(FEAT_EVAL)
		ಯದಿ (val == t_colors)
		    ch_log(ರಿಕ್ತ, "got_code_from_term(Co): no change (%d)", val);
		ಅನ್ಯಥಾ
		    ch_log(ರಿಕ್ತ,
			       "got_code_from_term(Co): changed from %d to %d",
								t_colors, val);
# endif
		may_adjust_color_ಎಣಿಕೆ(val);
	    }
# ಯದಿ 0
#  ifdef FEAT_TERMGUICOLORS
	    // when RGB result comes back, it is supported when the result contains an '='
	    ಅನ್ಯಥಾ ಯದಿ (name[0] == 'R' && name[1] == 'G' && name[2] == 'B' && code[9] == '=')
	    {
		ಪೂರ್ಣಾಙ್ಕ val = atoi((ವರ್ಣ *)str);
		// only enable it, if termguicolors hasn't been set yet and
		// there are 8 bits per color channel
		ಯದಿ (val == 8 && !p_tgc_set)
		{
#   ifdef FEAT_EVAL
		    ch_log(ರಿಕ್ತ, "got_code_from_term(RGB): xterm-direct colors detected");
#   endif
		    // RGB capability set, enable termguicolors
		    set_option_ಮೌಲ್ಯ((char_u *)"termguicolors", 1L, ರಿಕ್ತ, 0);
		}
	    }
#  endif
# endif
	    ಅನ್ಯಥಾ
	    {
		i = find_term_bykeys(str, ರಿಕ್ತ);
		ಯದಿ (i >= 0 && name[0] == termcodes[i].name[0]
					    && name[1] == termcodes[i].name[1])
		{
		    // Existing entry with the same name and code - skip.
# ifdef FEAT_EVAL
		    ch_log(ರಿಕ್ತ, "got_code_from_term(): Entry %c%c did not change",
							     name[0], name[1]);
# endif
		}
		ಅನ್ಯಥಾ ಯದಿ (i >= 0 && name[0] == 'K' && VIM_ISDIGIT(name[1]))
		{
		    // Would replace existing entry with keypad key - skip.
# ifdef FEAT_EVAL
		    ch_log(ರಿಕ್ತ, "got_code_from_term(): Skipping entry %c%c in favor of %c%c with matching keys %s",
			    name[0], name[1], termcodes[i].name[0], termcodes[i].name[1], str);
# endif
		}
		ಅನ್ಯಥಾ
		{
		    ಯದಿ (i >= 0)
		    {
			// Delete an existing entry using the same code.
# ifdef FEAT_EVAL
			ch_log(ರಿಕ್ತ, "got_code_from_term(): Deleting entry %c%c with matching keys %s",
			      termcodes[i].name[0], termcodes[i].name[1], str);
# endif
			del_termcode_idx(i);
		    }
# ifdef FEAT_EVAL
		    ಅನ್ಯಥಾ
			ch_log(ರಿಕ್ತ, "got_code_from_term(): Adding entry %c%c with keys %s",
							name[0], name[1], str);
# endif
		    add_termcode(name, str, ATC_FROM_TERM);
		}
	    }
	}
    }

    // May request more codes now that we received one.
    ++xt_index_in;
    req_more_codes_from_term();
}

/*
 * Check if there are any unanswered requests and deal with them.
 * This is called before starting an external program or getting direct
 * keyboard input.  We don't want responses to be send to that program or
 * handled as typed text.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
check_for_codes_from_term(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		c;

    // If no codes requested or all are answered, no need to wait.
    ಯದಿ (xt_index_out == 0 || xt_index_out == xt_index_in)
	ಪ್ರತ್ಯಾಗಮ;

    // Vgetc() will check for and handle any response.
    // Keep calling vpeekc() until we don't get any responses.
    ++no_mapping;
    ++allow_ಕೀಲಿಗಳು;
    ಪ್ರತಿ (;;)
    {
	c = vpeekc();
	ಯದಿ (c == NUL)	    // nothing available
	    ಭಙ್ಗ;

	// If a response is recognized it's replaced with K_IGNORE, must read
	// it from the input stream.  If there is no K_IGNORE we can't do
	// anything, break here (there might be some responses further on, but
	// we don't want to throw away any typed chars).
	ಯದಿ (c != K_SPECIAL && c != K_IGNORE)
	    ಭಙ್ಗ;
	c = vgetc();
	ಯದಿ (c != K_IGNORE)
	{
	    vungetc(c);
	    ಭಙ್ಗ;
	}
    }
    --no_mapping;
    --allow_ಕೀಲಿಗಳು;
}
#endif

#ಯದಿ defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
ಸ್ಥಿರ ವರ್ಣ ksme_str[20];
ಸ್ಥಿರ ವರ್ಣ ksmr_str[20];
ಸ್ಥಿರ ವರ್ಣ ksmd_str[20];

/*
 * For Win32 console: update termcap codes for existing console attributes.
 */
    ಶೂನ್ಯ
update_tcap(ಪೂರ್ಣಾಙ್ಕ attr)
{
    sprintf(ksme_str, "\033|%dm", attr);
    sprintf(ksmd_str, "\033|%dm", attr | 0x08);  // FOREGROUND_INTENSITY
    sprintf(ksmr_str, "\033|%dm", ((attr & 0x0F) << 4) | ((attr & 0xF0) >> 4));

    tcap_entry_T *p = find_builtin_term(DEFAULT_TERM);
    ಯದಿ (p == ರಿಕ್ತ)  // did not find it
	ಪ್ರತ್ಯಾಗಮ;
    ಯಾವತ್ (p->bt_string != ರಿಕ್ತ)
    {
	ಯದಿ (p->bt_entry == (ಪೂರ್ಣಾಙ್ಕ)KS_ME)
	    p->bt_string = &ksme_str[0];
	ಅನ್ಯಥಾ ಯದಿ (p->bt_entry == (ಪೂರ್ಣಾಙ್ಕ)KS_MR)
	    p->bt_string = &ksmr_str[0];
	ಅನ್ಯಥಾ ಯದಿ (p->bt_entry == (ಪೂರ್ಣಾಙ್ಕ)KS_MD)
	    p->bt_string = &ksmd_str[0];
	++p;
    }
}

# ifdef FEAT_TERMGUICOLORS
#  define KSSIZE 20

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ
{
    CMODE_INDEXED = 0,	// Use cmd.exe 4bit palette.
    CMODE_RGB,		// Use 24bit RGB colors using VTP.
    CMODE_256COL,	// Emulate xterm's 256-color palette using VTP.
    CMODE_LAST,
} cmode_T;

ಸಂರಚನಾ ks_tbl_S
{
    ಪೂರ್ಣಾಙ್ಕ  code;				// value of KS_
    ವರ್ಣ *vtp;				// code in RGB mode
    ವರ್ಣ *vtp2;				// code in 256color mode
    ವರ್ಣ buf[CMODE_LAST][KSSIZE];	// real buffer
};

ಸ್ಥಿರ ಸಂರಚನಾ ks_tbl_S ks_tbl[] =
{
    {(ಪೂರ್ಣಾಙ್ಕ)KS_ME,  "\033|0m",  "\033|0m", {""}},   // normal
    {(ಪೂರ್ಣಾಙ್ಕ)KS_MR,  "\033|7m",  "\033|7m", {""}},   // reverse
    {(ಪೂರ್ಣಾಙ್ಕ)KS_MD,  "\033|1m",  "\033|1m", {""}},   // bold
    {(ಪೂರ್ಣಾಙ್ಕ)KS_SO,  "\033|91m", "\033|91m", {""}},  // standout: bright red text
    {(ಪೂರ್ಣಾಙ್ಕ)KS_SE,  "\033|39m", "\033|39m", {""}},  // standout end: default color
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CZH, "\033|3m",  "\033|3m", {""}},   // italic
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CZR, "\033|0m",  "\033|0m", {""}},   // italic end
    {(ಪೂರ್ಣಾಙ್ಕ)KS_US,  "\033|4m",  "\033|4m", {""}},   // underscore
    {(ಪೂರ್ಣಾಙ್ಕ)KS_UE,  "\033|24m", "\033|24m", {""}},  // underscore end
#  ifdef TERMINFO
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CAB, "\033|%p1%db", "\033|%p14%dm", {""}}, // set background color
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CAF, "\033|%p1%df", "\033|%p13%dm", {""}}, // set foreground color
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CS,  "\033|%p1%d;%p2%dR", "\033|%p1%d;%p2%dR", {""}},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CSV, "\033|%p1%d;%p2%dV", "\033|%p1%d;%p2%dV", {""}},
#  ಅನ್ಯಥಾ
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CAB, "\033|%db", "\033|4%dm", {""}}, // set background color
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CAF, "\033|%df", "\033|3%dm", {""}}, // set foreground color
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CS,  "\033|%d;%dR", "\033|%d;%dR", {""}},
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CSV, "\033|%d;%dV", "\033|%d;%dV", {""}},
#  endif
    {(ಪೂರ್ಣಾಙ್ಕ)KS_CCO, "256", "256", {""}},	    // colors
    {(ಪೂರ್ಣಾಙ್ಕ)KS_NAME, ರಿಕ್ತ, ರಿಕ್ತ, {""}}			    // terminator
};

/*
 * Find the first entry for "code" in the builtin termcap for "name".
 * Returns NULL when not found.
 */
    ಸ್ಥಿರ tcap_entry_T *
find_first_tcap(
    char_u *name,
    ಪೂರ್ಣಾಙ್ಕ	    code)
{
    tcap_entry_T *p = find_builtin_term(name);
    ಯದಿ (p == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ಯಾವತ್ (p->bt_string != ರಿಕ್ತ)
    {
	ಯದಿ (p->bt_entry == code)
	    ಪ್ರತ್ಯಾಗಮ p;
	++p;
    }
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}
# endif

/*
 * For Win32 console: replace the sequence immediately after termguicolors.
 */
    ಶೂನ್ಯ
swap_tcap(ಶೂನ್ಯ)
{
# ifdef FEAT_TERMGUICOLORS
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ		init_done = FALSE;
    ಸ್ಥಿರ cmode_T	curr_mode;
    ಸಂರಚನಾ ks_tbl_S	*ks;
    cmode_T		mode;

    ಯದಿ (!init_done)
    {
	ಪ್ರತಿ (ks = ks_tbl; ks->code != (ಪೂರ್ಣಾಙ್ಕ)KS_NAME; ks++)
	{
	    tcap_entry_T *bt = find_first_tcap(DEFAULT_TERM, ks->code);
	    ಯದಿ (bt != ರಿಕ್ತ)
	    {
		// Preserve the original value.
		STRNCPY(ks->buf[CMODE_INDEXED], bt->bt_string, KSSIZE);
		STRNCPY(ks->buf[CMODE_RGB], ks->vtp, KSSIZE);
		STRNCPY(ks->buf[CMODE_256COL], ks->vtp2, KSSIZE);

		bt->bt_string = ks->buf[CMODE_INDEXED];
	    }
	}
	init_done = TRUE;
	curr_mode = CMODE_INDEXED;
    }

    ಯದಿ (p_tgc)
	mode = CMODE_RGB;
    ಅನ್ಯಥಾ ಯದಿ (t_colors >= 256)
	mode = CMODE_256COL;
    ಅನ್ಯಥಾ
	mode = CMODE_INDEXED;

    ಯದಿ (mode == curr_mode)
	ಪ್ರತ್ಯಾಗಮ;

    ಪ್ರತಿ (ks = ks_tbl; ks->code != (ಪೂರ್ಣಾಙ್ಕ)KS_NAME; ks++)
    {
	tcap_entry_T *bt = find_first_tcap(DEFAULT_TERM, ks->code);
	ಯದಿ (bt != ರಿಕ್ತ)
	    bt->bt_string = ks->buf[mode];
    }

    curr_mode = mode;
# endif
}

#endif


#ಯದಿ (defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))) || defined(FEAT_TERMINAL)
ಸ್ಥಿರ ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ cube_ಮೌಲ್ಯ[] = {
    0x00, 0x5F, 0x87, 0xAF, 0xD7, 0xFF
};

ಸ್ಥಿರ ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ grey_ramp[] = {
    0x08, 0x12, 0x1C, 0x26, 0x30, 0x3A, 0x44, 0x4E, 0x58, 0x62, 0x6C, 0x76,
    0x80, 0x8A, 0x94, 0x9E, 0xA8, 0xB2, 0xBC, 0xC6, 0xD0, 0xDA, 0xE4, 0xEE
};

ಸ್ಥಿರ ಧ್ರುವ char_u ansi_table[16][3] = {
//   R    G    B
  {  0,   0,   0}, // black
  {224,   0,   0}, // dark red
  {  0, 224,   0}, // dark green
  {224, 224,   0}, // dark yellow / brown
  {  0,   0, 224}, // dark blue
  {224,   0, 224}, // dark magenta
  {  0, 224, 224}, // dark cyan
  {224, 224, 224}, // light grey

  {128, 128, 128}, // dark grey
  {255,  64,  64}, // light red
  { 64, 255,  64}, // light green
  {255, 255,  64}, // yellow
  { 64,  64, 255}, // light blue
  {255,  64, 255}, // light magenta
  { 64, 255, 255}, // light cyan
  {255, 255, 255}, // white
};

# ಯದಿ defined(MSWIN)
// Mapping between cterm indices < 16 and their counterpart in the ANSI palette.
ಧ್ರುವ char_u cterm_ansi_idx[] = {
    0, 4, 2, 6, 1, 5, 3, 7, 8, 12, 10, 14, 9, 13, 11, 15
};
# endif

# define ANSI_INDEX_NONE 0

    ಶೂನ್ಯ
ansi_color2rgb(ಪೂರ್ಣಾಙ್ಕ nr, char_u *r, char_u *g, char_u *b, char_u *ansi_idx)
{
    ಯದಿ (nr < 16)
    {
	*r = ansi_table[nr][0];
	*g = ansi_table[nr][1];
	*b = ansi_table[nr][2];
	*ansi_idx = nr + 1;
    }
    ಅನ್ಯಥಾ
    {
	*r = 0;
	*g = 0;
	*b = 0;
	*ansi_idx = ANSI_INDEX_NONE;
    }
}

    ಶೂನ್ಯ
cterm_color2rgb(ಪೂರ್ಣಾಙ್ಕ nr, char_u *r, char_u *g, char_u *b, char_u *ansi_idx)
{
    ಪೂರ್ಣಾಙ್ಕ idx;

    ಯದಿ (nr < 16)
    {
# ಯದಿ defined(MSWIN)
	idx = cterm_ansi_idx[nr];
# ಅನ್ಯಥಾ
	idx = nr;
# endif
	*r = ansi_table[idx][0];
	*g = ansi_table[idx][1];
	*b = ansi_table[idx][2];
	*ansi_idx = idx + 1;
    }
    ಅನ್ಯಥಾ ಯದಿ (nr < 232)
    {
	// 216 color cube
	idx = nr - 16;
	*r = cube_ಮೌಲ್ಯ[idx / 36 % 6];
	*g = cube_ಮೌಲ್ಯ[idx / 6  % 6];
	*b = cube_ಮೌಲ್ಯ[idx      % 6];
	*ansi_idx = ANSI_INDEX_NONE;
    }
    ಅನ್ಯಥಾ ಯದಿ (nr < 256)
    {
	// 24 grey scale ramp
	idx = nr - 232;
	*r = grey_ramp[idx];
	*g = grey_ramp[idx];
	*b = grey_ramp[idx];
	*ansi_idx = ANSI_INDEX_NONE;
    }
    ಅನ್ಯಥಾ
    {
	*r = 0;
	*g = 0;
	*b = 0;
	*ansi_idx = ANSI_INDEX_NONE;
    }
}
#endif

/*
 * Replace K_BS by <BS> and K_DEL by <DEL>.
 * Include any modifiers into the key and drop them.
 * Returns "len" adjusted for replaced codes.
 */
    ಪೂರ್ಣಾಙ್ಕ
term_replace_keycodes(char_u *ta_buf, ಪೂರ್ಣಾಙ್ಕ ta_len, ಪೂರ್ಣಾಙ್ಕ len_arg)
{
    ಪೂರ್ಣಾಙ್ಕ		len = len_arg;
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		c;

    ಪ್ರತಿ (i = ta_len; i < ta_len + len; ++i)
    {
	ಯದಿ (ta_buf[i] == CSI && len - i > 3 && ta_buf[i + 1] == KS_MODIFIER)
	{
	    ಪೂರ್ಣಾಙ್ಕ modifiers = ta_buf[i + 2];
	    ಪೂರ್ಣಾಙ್ಕ ಕೀಲಿ = ta_buf[i + 3];

	    // Try to use the modifier to modify the key.  In any case drop the
	    // modifier.
	    mch_memmove(ta_buf + i + 1, ta_buf + i + 4, (size_t)(len - i - 3));
	    len -= 3;
	    ಯದಿ (ಕೀಲಿ < 0x80)
		ಕೀಲಿ = merge_modify_Other_ಕೀಲಿಗಳು(ಕೀಲಿ, &modifiers);
	    ta_buf[i] = ಕೀಲಿ;
	}
	ಅನ್ಯಥಾ ಯದಿ (ta_buf[i] == CSI && len - i > 2)
	{
	    c = TERMCAP2_ಕೀಲಿ(ta_buf[i + 1], ta_buf[i + 2]);
	    ಯದಿ (c == K_DEL || c == K_KDEL || c == K_BS)
	    {
		mch_memmove(ta_buf + i + 1, ta_buf + i + 3,
							(size_t)(len - i - 2));
		ಯದಿ (c == K_DEL || c == K_KDEL)
		    ta_buf[i] = DEL;
		ಅನ್ಯಥಾ
		    ta_buf[i] = Ctrl_H;
		len -= 2;
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (ta_buf[i] == '\r')
	    ta_buf[i] = '\n';
	ಯದಿ (has_mbyte)
	    i += (*mb_ptr2len_len)(ta_buf + i, ta_len + len - i) - 1;
    }
    ಪ್ರತ್ಯಾಗಮ len;
}

/*
 * Should be called when cleaning up terminal state.
 */
    ಶೂನ್ಯ
term_disable_dec(ಶೂನ್ಯ)
{
    term_set_sync_output(TERM_SYNC_OUTPUT_OFF);
#ifdef UNIX
    term_set_win_resize(false);
#endif
    // Make sure to always flush the output buffer, because this may be called
    // before starting the GUI
    out_flush();
}

#ifdef UNIX
/*
 * Enable or disable receiving in-band window resize events from the terminal.
 * If "state" is true, then if the terminal supports DEC mode 2048 and
 * 'termresize' is "" or "inband", then enable it and disable the SIGWINCH
 * signal handling. Otherwise disable the mode if it is enabled and reinstall
 * the SIGWINCH handler.
 */
    ಶೂನ್ಯ
term_set_win_resize(bool state)
{
# ifdef FEAT_GUI
    bool    in_gui = gui.in_use;

    ಯದಿ (state && in_gui)
	ಪ್ರತ್ಯಾಗಮ;
# endif

    ಯದಿ (!state || win_resize_setting == 0 || win_resize_setting == 4)
    {
	// Make sure it update internal window size if DEC mode 2048 is
	// unavailable now.
	ಯದಿ (win_resize_enabled)
	{
	    set_shellsize(0, 0, false);
	    set_sigwinch_handler();
	    out_str((char_u *)"\033[?2048l");
	}
	win_resize_enabled = false;
    }
    ಅನ್ಯಥಾ ಯದಿ ((*p_trz == NUL || STRCMP(p_trz, "inband") == 0)
	    && !win_resize_enabled)
    {
	ಯದಿ (win_resize_setting == 2)
	    out_str((char_u *)"\033[?2048h");
# ifdef SIGWINCH
	mch_signal(SIGWINCH, SIG_DFL);
# endif
	win_resize_enabled = true;
    }
}
#endif

    ಪೂರ್ಣಾಙ್ಕ
sync_output_active(ಶೂನ್ಯ)
{
#ifdef FEAT_GUI
    ಯದಿ (gui.in_use)
	ಪ್ರತ್ಯಾಗಮ TRUE;
#endif
    ಪ್ರತ್ಯಾಗಮ p_tsy && (sync_output_setting == 1 || sync_output_setting == 2)
	&& *T_BSU != NUL && *T_ESU != NUL;
}

/*
 * Enable or disable synchronized output if possible. Specification can be found
 * here:
 * https://github.com/contour-terminal/vt-extensions/blob/master/synchronized-output.md
 *
 * BSU/ESU do not need to bypass out_buf and go straight to ui_write().
 * What matters is the order in which they reach the terminal:
 * - BSU must be emitted before the batched redraw bytes.
 * - ESU must be emitted after those redraw bytes.
 * - A FLUSH must emit ESU and BSU together, then flush immediately.
 * As long as out_flush() is done at those boundaries, putting BSU/ESU in
 * out_buf reduces small writes without changing the observable protocol.
 */
    ಶೂನ್ಯ
term_set_sync_output(ಪೂರ್ಣಾಙ್ಕ flags)
{
    bool    allowed;
#ifdef FEAT_GUI
    bool    in_gui = gui.in_use;
#ಅನ್ಯಥಾ
    bool    in_gui = false;
#endif

    allowed = p_tsy && (sync_output_setting == 1 || sync_output_setting == 2);

    ಯದಿ (flags & TERM_SYNC_OUTPUT_FLUSH)
    {
	// Tell terminal to display screen contents, then resume batching.
	ಯದಿ (allowed && !in_gui && sync_output_state > 0 && *T_ESU != NUL &&
		*T_BSU != NUL)
	{
	    out_str_nf(T_ESU);
	    out_str_nf(T_BSU);
	    out_flush();
	}
	ಪ್ರತ್ಯಾಗಮ;
    }

    // Forcibly turn off synchronized output (e.g. 'notermsync')
    ಯದಿ (flags & TERM_SYNC_OUTPUT_OFF)
    {
	ಯದಿ (sync_output_state > 0 && *T_ESU != NUL)
	{
	    out_str_nf(T_ESU);
	    out_flush();
	    sync_output_state = 0;
	}
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (!allowed || in_gui || *T_BSU == NUL || *T_ESU == NUL)
	ಪ್ರತ್ಯಾಗಮ;

    // Only enable if we aren't already, and only disable if we have reached
    // zero.
    ಯದಿ (flags & TERM_SYNC_OUTPUT_ENABLE)
    {
	ಯದಿ (sync_output_state++ > 0)
	    ಪ್ರತ್ಯಾಗಮ;
	out_str_nf(T_BSU);
    }
    ಅನ್ಯಥಾ ಯದಿ (flags & TERM_SYNC_OUTPUT_DISABLE)
    {
	ಯದಿ (sync_output_state == 0 || --sync_output_state > 0)
	    ಪ್ರತ್ಯಾಗಮ;
	// Flush the output buffer before ending the sync batch so that
	// all drawing output is sent to the terminal within the
	// BSU..ESU window.  Without this, the drawing data remaining in
	// out_buf would be sent after ESU, outside the sync batch.
	out_str_nf(T_ESU);
	out_flush();
    }
    ಅನ್ಯಥಾ
    {
	siemsg("Unknown sync output value %d", flags);
	ಪ್ರತ್ಯಾಗಮ;
    }
}
