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
#ವ್ಯಾಖ್ಯಾನಿಸು tgetstr tgetstr_defined_wrong

#ಅನ್ತರ್ಭಾವ "vim.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_TGETENT
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
# ಆದರೆ defined(VMS) || defined(AMIGA)
#  define TPUTSFUNCAST (ಶೂನ್ಯ (*)(ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ))
# ಇಲ್ಲದಿದ್ದರೆ
#  ifdef HAVE_OUTFUNTYPE
#   define TPUTSFUNCAST (outfuntype)
#  ಇಲ್ಲದಿದ್ದರೆ
#   define TPUTSFUNCAST (ಪೂರ್ಣಾಂಕ (*)(ಪೂರ್ಣಾಂಕ))
#  endif
# endif
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು tgetstr

// start of keys that are not directly used by Vim but can be mapped
#ವ್ಯಾಖ್ಯಾನಿಸು BT_EXTRA_ಕೀಲಿಗಳು	0x101

ಸ್ಥಿರ ಶೂನ್ಯ parse_builtin_tcap(char_u *s);
ಸ್ಥಿರ ಶೂನ್ಯ gather_termleader(ಶೂನ್ಯ);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMRESPONSE
ಸ್ಥಿರ ಶೂನ್ಯ req_codes_from_ಪದ(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ req_ಇನ್ನಷ್ಟು_codes_from_ಪದ(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ got_ಸಂಕೇತ_from_ಪದ(char_u *ಸಂಕೇತ_2, ಪೂರ್ಣಾಂಕ len);
ಸ್ಥಿರ ಶೂನ್ಯ check_for_codes_from_ಪದ(ಶೂನ್ಯ);
#ಆದರೆ_ಅಂತ್ಯ
ಸ್ಥಿರ ಶೂನ್ಯ del_termcode_idx(ಪೂರ್ಣಾಂಕ idx);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಕಂಡುಹಿಡಿ_ಪದ_bykeys(char_u *src, ಪೂರ್ಣಾಂಕ *len);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಪದ_is_builtin(char_u *name);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಪದ_7to8bit(char_u *p);
ಸ್ಥಿರ ಶೂನ್ಯ accept_modifiers_for_function_ಕೀಲಿಗಳು(ಶೂನ್ಯ);

#ಆದರೆ 0  // Change to 1 to enable ch_log() calls for termresponse debugging.
# define DEBUG_TERMRESPONSE
# define LOG_TR1(str) \
		ch_log(ಖಾಲಿ, "TermResp: %s " str, \
			must_redraw == UPD_NOT_VALID ? "NV" \
			: must_redraw == UPD_CLEAR ? "CL" : "  ")
# define LOG_TRN(fmt,...) \
		ch_log(ಖಾಲಿ, "TermResp: %s " fmt, \
			must_redraw == UPD_NOT_VALID ? "NV" \
			: must_redraw == UPD_CLEAR ? "CL" : "  ", __VA_ARGS__)
#ಇಲ್ಲದಿದ್ದರೆ
# define LOG_TR1(str) ಮಾಡು { /**/ } ಆಗ (0)
# define LOG_TRN(fmt,...) ಮಾಡು { /**/ } ಆಗ (0)
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_TGETENT
ಸ್ಥಿರ ಅಕ್ಷರ *invoke_tgetent(char_u *, char_u *);

/*
 * Here is our own prototype for tgetstr(), any prototypes from the include
 * files have been disabled by the define at the start of this file.
 */
ಅಕ್ಷರ		*tgetstr(ಅಕ್ಷರ *, ಅಕ್ಷರ **);
#ಆದರೆ_ಅಂತ್ಯ

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
    ಸ್ಥಿತಿ_GET,		// send request when switching to RAW mode
    ಸ್ಥಿತಿ_ಕಳುಹಿಸಿದ,	// did send request, checking for response
    ಸ್ಥಿತಿ_GOT,		// received response
    ಸ್ಥಿತಿ_FAIL		// timed out
} request_ಪ್ರಗತಿ_T;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    request_ಪ್ರಗತಿ_T	    tr_ಪ್ರಗತಿ;
    time_t		    tr_ಆರಂಭ;	// when request was sent, -1 for never
} termrequest_T;

// Holds state for current OSC response.
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    ಪೂರ್ಣಾಂಕ		    processing;	// If we are in the middle of an OSC response
    char_u	    ಆರಂಭ_char;	// First char in the OSC response
    garray_T	    buf;	// Buffer holding the OSC response, to be
				// placed in the "v:termosc" vim var.

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ELAPSED_FUNC
    elapsed_T	    ಆರಂಭ_tv;	// Set at the beginning of an OSC response.
				// Used to timeout after a set amount of
				// time.
#ಆದರೆ_ಅಂತ್ಯ
} oscstate_T;

#ವ್ಯಾಖ್ಯಾನಿಸು TERMREQUEST_INIT {ಸ್ಥಿತಿ_GET, -1}

// Request Terminal Version status:
ಸ್ಥಿರ termrequest_T crv_ಸ್ಥಿತಿ = TERMREQUEST_INIT;

// Request Cursor position report:
ಸ್ಥಿರ termrequest_T u7_ಸ್ಥಿತಿ = TERMREQUEST_INIT;

// Request xterm compatibility check:
ಸ್ಥಿರ termrequest_T xcc_ಸ್ಥಿತಿ = TERMREQUEST_INIT;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMRESPONSE
# ifdef FEAT_TERMINAL
// Request foreground color report:
ಸ್ಥಿರ termrequest_T rfg_ಸ್ಥಿತಿ = TERMREQUEST_INIT;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ fg_r = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ fg_g = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ fg_b = 0;
# endif
// Background color values from the OSC 11 response, also used by the
// image backend to flatten RGBA alpha onto the actual terminal background.
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ bg_r = 255;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ bg_g = 255;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ bg_b = 255;

// Request background color report:
ಸ್ಥಿರ termrequest_T rbg_ಸ್ಥಿತಿ = TERMREQUEST_INIT;

// Request cursor blinking mode report:
ಸ್ಥಿರ termrequest_T rbm_ಸ್ಥಿತಿ = TERMREQUEST_INIT;

// Request cursor style report:
ಸ್ಥಿರ termrequest_T rcs_ಸ್ಥಿತಿ = TERMREQUEST_INIT;

// Request window's position report:
ಸ್ಥಿರ termrequest_T winpos_ಸ್ಥಿತಿ = TERMREQUEST_INIT;

// Request DECRQM (DEC mode) report:
ಸ್ಥಿರ termrequest_T decrqm_ಸ್ಥಿತಿ = TERMREQUEST_INIT;

ಸ್ಥಿರ termrequest_T *all_termrequests[] = {
    &crv_ಸ್ಥಿತಿ,
    &u7_ಸ್ಥಿತಿ,
    &xcc_ಸ್ಥಿತಿ,
# ifdef FEAT_TERMINAL
    &rfg_ಸ್ಥಿತಿ,
# endif
    &rbg_ಸ್ಥಿತಿ,
    &rbm_ಸ್ಥಿತಿ,
    &rcs_ಸ್ಥಿತಿ,
    &winpos_ಸ್ಥಿತಿ,
    &decrqm_ಸ್ಥಿತಿ,
    ಖಾಲಿ
};

// The t_8u code may default to a value but get reset when the term response is
// received.  To avoid redrawing too often, only redraw when t_8u is not reset
// and it was supposed to be written.  Unless t_8u was set explicitly.
// FALSE -> don't output t_8u yet
// MAYBE -> tried outputting t_8u while FALSE
// OK    -> can write t_8u
ಪೂರ್ಣಾಂಕ write_t_8u_state = FALSE;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_TGETENT
/*
 * Don't declare these variables if termcap.h contains them.
 * Autoconf checks if these variables should be declared extern (not all
 * systems have them).
 * Some versions define ospeed to be speed_t, but that is incompatible with
 * BSD, where ospeed is short and speed_t is long.
 */
# ifndef HAVE_OSPEED
#  ifdef OSPEED_EXTERN
ಬಾಹ್ಯ ಗಿಡ್ಡ ospeed;
#  ಇಲ್ಲದಿದ್ದರೆ
ಗಿಡ್ಡ ospeed;
#  endif
# endif
# ifndef HAVE_ಮೇಲೆ_BC_PC
#  ifdef ಮೇಲೆ_BC_PC_EXTERN
ಬಾಹ್ಯ ಅಕ್ಷರ *ಮೇಲೆ_2, *BC, PC;
#  ಇಲ್ಲದಿದ್ದರೆ
ಅಕ್ಷರ *ಮೇಲೆ_2, *BC, PC;
#  endif
# endif

# define TGETSTR(s, p)	vim_tgetstr((s), (p))
# define TGETENT(b, t)	tgetent((ಅಕ್ಷರ *)(b), (ಅಕ್ಷರ *)(t))
ಸ್ಥಿರ char_u *vim_tgetstr(ಅಕ್ಷರ *s, char_u **pp);
#ಆದರೆ_ಅಂತ್ಯ // HAVE_TGETENT

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ  detected_8bit = FALSE;	// detected 8-bit terminal

#ಆದರೆ (defined(UNIX) || defined(VMS))
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ focus_state = MAYBE; // TRUE if the Vim window has focus
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMRESPONSE
// When the cursor shape was detected these values are used:
// 1: block, 2: underline, 3: vertical bar
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ initial_cursor_shape = 0;

// The blink flag from the style response may be inverted from the actual
// blinking state, xterm XORs the flags.
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ initial_cursor_shape_blink = FALSE;

// The blink flag from the blinking-cursor mode response
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ initial_cursor_blink = FALSE;
#ಆದರೆ_ಅಂತ್ಯ

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
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sync_output_setting = 0;

// > 0: Currently batching output
// == 0: No synchronized output
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sync_output_state = 0;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
// DEC mode 2048 (in-band window resize events)
// https://gist.github.com/rockorager/e695fb2924d36b2bcf1fff4a3704bd83
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ win_resize_setting = 0;
ಸ್ಥಿರ bool win_resize_enabled = false;
#ಆದರೆ_ಅಂತ್ಯ

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
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    ಪೂರ್ಣಾಂಕ		bt_ನಮೂದು;   // either a KS_xxx code (>= 0), or a K_xxx code.
    ಅಕ್ಷರ	*bt_string; //ಮೌಲ್ಯ
} tcap_ನಮೂದು_T;

/*
 * Standard ANSI terminal, default for Unix.
 */
ಸ್ಥಿರ tcap_ನಮೂದು_T builtin_ansi[] = {
    {(ಪೂರ್ಣಾಂಕ)KS_CE,	"\033[K"},
    {(ಪೂರ್ಣಾಂಕ)KS_AL,	"\033[L"},
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TERMINFO
    {(ಪೂರ್ಣಾಂಕ)KS_CAL,	"\033[%p1%dL"},
#ಇಲ್ಲದಿದ್ದರೆ
    {(ಪೂರ್ಣಾಂಕ)KS_CAL,	"\033[%dL"},
#ಆದರೆ_ಅಂತ್ಯ
    {(ಪೂರ್ಣಾಂಕ)KS_DL,	"\033[M"},
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TERMINFO
    {(ಪೂರ್ಣಾಂಕ)KS_CDL,	"\033[%p1%dM"},
#ಇಲ್ಲದಿದ್ದರೆ
    {(ಪೂರ್ಣಾಂಕ)KS_CDL,	"\033[%dM"},
#ಆದರೆ_ಅಂತ್ಯ
    {(ಪೂರ್ಣಾಂಕ)KS_CL,	"\033[H\033[2J"},
    {(ಪೂರ್ಣಾಂಕ)KS_ME,	"\033[0m"},
    {(ಪೂರ್ಣಾಂಕ)KS_MR,	"\033[7m"},
    {(ಪೂರ್ಣಾಂಕ)KS_MS,	"y"},
    {(ಪೂರ್ಣಾಂಕ)KS_UT,	"y"},		// guessed
    {(ಪೂರ್ಣಾಂಕ)KS_LE,	"\b"},
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TERMINFO
    {(ಪೂರ್ಣಾಂಕ)KS_CM,	"\033[%i%p1%d;%p2%dH"},
#ಇಲ್ಲದಿದ್ದರೆ
    {(ಪೂರ್ಣಾಂಕ)KS_CM,	"\033[%i%d;%dH"},
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TERMINFO
    {(ಪೂರ್ಣಾಂಕ)KS_CRI,	"\033[%p1%dC"},
#ಇಲ್ಲದಿದ್ದರೆ
    {(ಪೂರ್ಣಾಂಕ)KS_CRI,	"\033[%dC"},
#ಆದರೆ_ಅಂತ್ಯ

    {(ಪೂರ್ಣಾಂಕ)KS_NAME,	ಖಾಲಿ}  // end marker
};

/*
 * VT320 is working as an ANSI terminal compatible DEC terminal.
 * (it covers VT1x0, VT2x0 and VT3x0 up to VT320 on VMS as well)
 * TODO:- rewrite ESC[ codes to CSI
 *      - keyboard languages (CSI ? 26 n)
 */
ಸ್ಥಿರ tcap_ನಮೂದು_T builtin_vt320[] = {
    {(ಪೂರ್ಣಾಂಕ)KS_CE,	"\033[K"},
    {(ಪೂರ್ಣಾಂಕ)KS_AL,	"\033[L"},
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TERMINFO
    {(ಪೂರ್ಣಾಂಕ)KS_CAL,	"\033[%p1%dL"},
#ಇಲ್ಲದಿದ್ದರೆ
    {(ಪೂರ್ಣಾಂಕ)KS_CAL,	"\033[%dL"},
#ಆದರೆ_ಅಂತ್ಯ
    {(ಪೂರ್ಣಾಂಕ)KS_DL,	"\033[M"},
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TERMINFO
    {(ಪೂರ್ಣಾಂಕ)KS_CDL,	"\033[%p1%dM"},
#ಇಲ್ಲದಿದ್ದರೆ
    {(ಪೂರ್ಣಾಂಕ)KS_CDL,	"\033[%dM"},
#ಆದರೆ_ಅಂತ್ಯ
    {(ಪೂರ್ಣಾಂಕ)KS_CL,	"\033[H\033[2J"},
    {(ಪೂರ್ಣಾಂಕ)KS_CD,	"\033[J"},
    {(ಪೂರ್ಣಾಂಕ)KS_CCO,	"8"},			// allow 8 colors
    {(ಪೂರ್ಣಾಂಕ)KS_ME,	"\033[0m"},
    {(ಪೂರ್ಣಾಂಕ)KS_MR,	"\033[7m"},
    {(ಪೂರ್ಣಾಂಕ)KS_MD,	"\033[1m"},  // bold mode
    {(ಪೂರ್ಣಾಂಕ)KS_SE,	"\033[22m"},// normal mode
    {(ಪೂರ್ಣಾಂಕ)KS_UE,	"\033[24m"},// exit underscore mode
    {(ಪೂರ್ಣಾಂಕ)KS_US,	"\033[4m"},  // underscore mode
    {(ಪೂರ್ಣಾಂಕ)KS_CZH,	"\033[34;43m"},  // italic mode: blue text on yellow
    {(ಪೂರ್ಣಾಂಕ)KS_CZR,	"\033[0m"},	    // italic mode end
    {(ಪೂರ್ಣಾಂಕ)KS_CAB,	"\033[4%dm"},	    // set background color (ANSI)
    {(ಪೂರ್ಣಾಂಕ)KS_CAF,	"\033[3%dm"},	    // set foreground color (ANSI)
    {(ಪೂರ್ಣಾಂಕ)KS_CSB,	"\033[102;%dm"},	// set screen background color
    {(ಪೂರ್ಣಾಂಕ)KS_CSF,	"\033[101;%dm"},	// set screen foreground color
    {(ಪೂರ್ಣಾಂಕ)KS_MS,	"y"},
    {(ಪೂರ್ಣಾಂಕ)KS_UT,	"y"},
    {(ಪೂರ್ಣಾಂಕ)KS_XN,	"y"},
    {(ಪೂರ್ಣಾಂಕ)KS_LE,	"\b"},
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TERMINFO
    {(ಪೂರ್ಣಾಂಕ)KS_CM,	"\033[%i%p1%d;%p2%dH"},
#ಇಲ್ಲದಿದ್ದರೆ
    {(ಪೂರ್ಣಾಂಕ)KS_CM,	"\033[%i%d;%dH"},
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TERMINFO
    {(ಪೂರ್ಣಾಂಕ)KS_CRI,	"\033[%p1%dC"},
#ಇಲ್ಲದಿದ್ದರೆ
    {(ಪೂರ್ಣಾಂಕ)KS_CRI,	"\033[%dC"},
#ಆದರೆ_ಅಂತ್ಯ
    {K_ಮೇಲೆ,		"\033[A"},
    {K_ಕೆಳಕ್ಕೆ,		"\033[B"},
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
    {K_ಕೊನೆ,		"\033[4~"},
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

    {(ಪೂರ್ಣಾಂಕ)KS_NAME,	ಖಾಲಿ}  // end marker
};

/*
 * Ordinary vt52
 */
ಸ್ಥಿರ tcap_ನಮೂದು_T builtin_vt52[] = {
    {(ಪೂರ್ಣಾಂಕ)KS_CE,	"\033K"},
    {(ಪೂರ್ಣಾಂಕ)KS_CD,	"\033J"},
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TERMINFO
    {(ಪೂರ್ಣಾಂಕ)KS_CM,	"\033Y%p1%' '%+%c%p2%' '%+%c"},
#ಇಲ್ಲದಿದ್ದರೆ
    {(ಪೂರ್ಣಾಂಕ)KS_CM,	"\033Y%+ %+ "},
#ಆದರೆ_ಅಂತ್ಯ
    {(ಪೂರ್ಣಾಂಕ)KS_LE,	"\b"},
    {(ಪೂರ್ಣಾಂಕ)KS_SR,	"\033I"},
    {(ಪೂರ್ಣಾಂಕ)KS_AL,	"\033L"},
    {(ಪೂರ್ಣಾಂಕ)KS_DL,	"\033M"},
    {K_ಮೇಲೆ,		"\033A"},
    {K_ಕೆಳಕ್ಕೆ,		"\033B"},
    {K_ಎಡ,		"\033D"},
    {K_ಬಲ,		"\033C"},
    {K_F1,		"\033P"},
    {K_F2,		"\033Q"},
    {K_F3,		"\033R"},
    {(ಪೂರ್ಣಾಂಕ)KS_CL,	"\033H\033J"},
    {(ಪೂರ್ಣಾಂಕ)KS_MS,	"y"},

    {(ಪೂರ್ಣಾಂಕ)KS_NAME,	ಖಾಲಿ}  // end marker
};

/*
 * Builtin xterm with Vim-specific entries.
 */
ಸ್ಥಿರ tcap_ನಮೂದು_T builtin_xterm[] = {
    {(ಪೂರ್ಣಾಂಕ)KS_CE,	"\033[K"},
    {(ಪೂರ್ಣಾಂಕ)KS_AL,	"\033[L"},
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TERMINFO
    {(ಪೂರ್ಣಾಂಕ)KS_CAL,	"\033[%p1%dL"},
#ಇಲ್ಲದಿದ್ದರೆ
    {(ಪೂರ್ಣಾಂಕ)KS_CAL,	"\033[%dL"},
#ಆದರೆ_ಅಂತ್ಯ
    {(ಪೂರ್ಣಾಂಕ)KS_DL,	"\033[M"},
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TERMINFO
    {(ಪೂರ್ಣಾಂಕ)KS_CDL,	"\033[%p1%dM"},
#ಇಲ್ಲದಿದ್ದರೆ
    {(ಪೂರ್ಣಾಂಕ)KS_CDL,	"\033[%dM"},
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TERMINFO
    {(ಪೂರ್ಣಾಂಕ)KS_CS,	"\033[%i%p1%d;%p2%dr"},
#ಇಲ್ಲದಿದ್ದರೆ
    {(ಪೂರ್ಣಾಂಕ)KS_CS,	"\033[%i%d;%dr"},
#ಆದರೆ_ಅಂತ್ಯ
    {(ಪೂರ್ಣಾಂಕ)KS_CL,	"\033[H\033[2J"},
    {(ಪೂರ್ಣಾಂಕ)KS_CD,	"\033[J"},
    {(ಪೂರ್ಣಾಂಕ)KS_ME,	"\033[m"},
    {(ಪೂರ್ಣಾಂಕ)KS_MR,	"\033[7m"},
    {(ಪೂರ್ಣಾಂಕ)KS_MD,	"\033[1m"},
    {(ಪೂರ್ಣಾಂಕ)KS_UE,	"\033[m"},
    {(ಪೂರ್ಣಾಂಕ)KS_US,	"\033[4m"},
    {(ಪೂರ್ಣಾಂಕ)KS_STE,	"\033[29m"},
    {(ಪೂರ್ಣಾಂಕ)KS_STS,	"\033[9m"},
    {(ಪೂರ್ಣಾಂಕ)KS_MS,	"y"},
    {(ಪೂರ್ಣಾಂಕ)KS_UT,	"y"},
    {(ಪೂರ್ಣಾಂಕ)KS_LE,	"\b"},
    {(ಪೂರ್ಣಾಂಕ)KS_VI,	"\033[?25l"},
    {(ಪೂರ್ಣಾಂಕ)KS_VE,	"\033[?25h"},
    {(ಪೂರ್ಣಾಂಕ)KS_VS,	"\033[?12h"},
    {(ಪೂರ್ಣಾಂಕ)KS_CVS,	"\033[?12l"},
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TERMINFO
    {(ಪೂರ್ಣಾಂಕ)KS_CSH,	"\033[%p1%d q"},
#ಇಲ್ಲದಿದ್ದರೆ
    {(ಪೂರ್ಣಾಂಕ)KS_CSH,	"\033[%d q"},
#ಆದರೆ_ಅಂತ್ಯ
    {(ಪೂರ್ಣಾಂಕ)KS_ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ,	"\033[?12$p"},
    {(ಪೂರ್ಣಾಂಕ)KS_CRS,	"\033P$q q\033\\"},
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TERMINFO
    {(ಪೂರ್ಣಾಂಕ)KS_CM,	"\033[%i%p1%d;%p2%dH"},
#ಇಲ್ಲದಿದ್ದರೆ
    {(ಪೂರ್ಣಾಂಕ)KS_CM,	"\033[%i%d;%dH"},
#ಆದರೆ_ಅಂತ್ಯ
    {(ಪೂರ್ಣಾಂಕ)KS_SR,	"\033M"},
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TERMINFO
    {(ಪೂರ್ಣಾಂಕ)KS_CRI,	"\033[%p1%dC"},
#ಇಲ್ಲದಿದ್ದರೆ
    {(ಪೂರ್ಣಾಂಕ)KS_CRI,	"\033[%dC"},
#ಆದರೆ_ಅಂತ್ಯ
    {(ಪೂರ್ಣಾಂಕ)KS_KS,	"\033[?1h\033="},
    {(ಪೂರ್ಣಾಂಕ)KS_KE,	"\033[?1l\033>"},
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_XTERM_SAVE
    {(ಪೂರ್ಣಾಂಕ)KS_TI,	"\0337\033[?47h"},
    {(ಪೂರ್ಣಾಂಕ)KS_TE,	"\033[?47l\0338"},
#ಆದರೆ_ಅಂತ್ಯ
    // These are now under control of the 'keyprotocol' option, see
    // "builtin_mok2".
    // {(int)KS_CTI,	"\033[>4;2m"},
    // {(int)KS_CRK,	"\033[?4m"},
    // {(int)KS_CTE,	"\033[>4;m"},
    {(ಪೂರ್ಣಾಂಕ)KS_CIS,	"\033]1;"},
    {(ಪೂರ್ಣಾಂಕ)KS_CIE,	"\007"},
    {(ಪೂರ್ಣಾಂಕ)KS_TS,	"\033]2;"},
    {(ಪೂರ್ಣಾಂಕ)KS_FS,	"\007"},
    {(ಪೂರ್ಣಾಂಕ)KS_CSC,	"\033]12;"},
    {(ಪೂರ್ಣಾಂಕ)KS_CEC,	"\007"},
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TERMINFO
    {(ಪೂರ್ಣಾಂಕ)KS_CWS,	"\033[8;%p1%d;%p2%dt"},
    {(ಪೂರ್ಣಾಂಕ)KS_CWP,	"\033[3;%p1%d;%p2%dt"},
    {(ಪೂರ್ಣಾಂಕ)KS_CGP,	"\033[13t"},
#ಇಲ್ಲದಿದ್ದರೆ
    {(ಪೂರ್ಣಾಂಕ)KS_CWS,	"\033[8;%d;%dt"},
    {(ಪೂರ್ಣಾಂಕ)KS_CWP,	"\033[3;%d;%dt"},
    {(ಪೂರ್ಣಾಂಕ)KS_CGP,	"\033[13t"},
#ಆದರೆ_ಅಂತ್ಯ
    {(ಪೂರ್ಣಾಂಕ)KS_CRV,	"\033[>c"},
    {(ಪೂರ್ಣಾಂಕ)KS_CXM,	"\033[?1006;1000%?%p1%{1}%=%th%el%;"},
    {(ಪೂರ್ಣಾಂಕ)KS_RFG,	"\033]10;?\007"},
    {(ಪೂರ್ಣಾಂಕ)KS_RBG,	"\033]11;?\007"},
    {(ಪೂರ್ಣಾಂಕ)KS_U7,	"\033[6n"},
    {(ಪೂರ್ಣಾಂಕ)KS_CAU,	"\033[58;5;%dm"},
    {(ಪೂರ್ಣಾಂಕ)KS_CBE,	"\033[?2004h"},
    {(ಪೂರ್ಣಾಂಕ)KS_CBD,	"\033[?2004l"},
    {(ಪೂರ್ಣಾಂಕ)KS_CST,	"\033[22;2t"},
    {(ಪೂರ್ಣಾಂಕ)KS_CRT,	"\033[23;2t"},
    {(ಪೂರ್ಣಾಂಕ)KS_SSI,	"\033[22;1t"},
    {(ಪೂರ್ಣಾಂಕ)KS_SRI,	"\033[23;1t"},
#ಆದರೆ (defined(UNIX) || defined(VMS))
    {(ಪೂರ್ಣಾಂಕ)KS_FD,	"\033[?1004l"},
    {(ಪೂರ್ಣಾಂಕ)KS_FE,	"\033[?1004h"},
#ಆದರೆ_ಅಂತ್ಯ

    {K_ಮೇಲೆ,		"\033O*A"},
    {K_ಕೆಳಕ್ಕೆ,		"\033O*B"},
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
    {K_ಕೊನೆ,		"\033[@;*F"},	// Esc [ F or Esc 1 ; F
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

    {(ಪೂರ್ಣಾಂಕ)KS_NAME,	ಖಾಲಿ}  // end marker
};

/*
 * Additions for using modifyOtherKeys level 2.  Same as what is used for
 * xterm.
 */
ಸ್ಥಿರ tcap_ನಮೂದು_T builtin_mok2[] = {
    // t_TI enables modifyOtherKeys level 2
    {(ಪೂರ್ಣಾಂಕ)KS_CTI,	"\033[>4;2m"},

    // XTQMODKEYS was added in xterm version 377: "CSI ? 4 m" which should
    // return "{lead} > 4 ; Pv m".  Before version 377 we expect it to have no
    // effect.
    {(ಪೂರ್ಣಾಂಕ)KS_CRK,	"\033[?4m"},

    // t_TE disables modifyOtherKeys
    {(ಪೂರ್ಣಾಂಕ)KS_CTE,	"\033[>4;m"},

    {(ಪೂರ್ಣಾಂಕ)KS_NAME,	ಖಾಲಿ}  // end marker
};

/*
 * Additions for using the Kitty keyboard protocol.
 */
ಸ್ಥಿರ tcap_ನಮೂದು_T builtin_kitty[] = {
    // t_TI enables the kitty keyboard protocol.
    {(ಪೂರ್ಣಾಂಕ)KS_CTI,	"\033[=1;1u"},

    // t_RK requests the kitty keyboard protocol state
    {(ಪೂರ್ಣಾಂಕ)KS_CRK,	"\033[?u"},

    // t_TE also disables modifyOtherKeys, because t_TI from xterm may already
    // have been used.
    {(ಪೂರ್ಣಾಂಕ)KS_CTE,	"\033[>4;m\033[=0;1u"},

    // t_RF request terminal foreground color
    {(ಪೂರ್ಣಾಂಕ)KS_RFG,	"\033]10;?\033\\"},

    // t_RB request terminal background color
    {(ಪೂರ್ಣಾಂಕ)KS_RBG,	"\033]11;?\033\\"},

    {(ಪೂರ್ಣಾಂಕ)KS_NAME,	ಖಾಲಿ}  // end marker
};

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_TGETENT
/*
 * Additions for enabling/disabling synchronized output mode for terminal.
 */
ಸ್ಥಿರ tcap_ನಮೂದು_T builtin_sync_output[] = {
    {(ಪೂರ್ಣಾಂಕ)KS_BSU,	"\033[?2026h"},
    {(ಪೂರ್ಣಾಂಕ)KS_ESU,	"\033[?2026l"},
    {(ಪೂರ್ಣಾಂಕ)KS_NAME,	ಖಾಲಿ}  // end marker
};
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMRESPONSE
/*
 * List of DECRQM modes that Vim supports
 */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ dec_modes[] = {
    2026,   // Synchronized output
# ifdef UNIX
    2048    // In-band terminal resize events
# endif
};
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMGUICOLORS
/*
 * Additions for using the RGB colors and terminal font
 */
ಸ್ಥಿರ tcap_ನಮೂದು_T builtin_rgb[] = {
    // These are printf strings, not terminal codes.
    {(ಪೂರ್ಣಾಂಕ)KS_8F,	"\033[38;2;%lu;%lu;%lum"},
    {(ಪೂರ್ಣಾಂಕ)KS_8B,	"\033[48;2;%lu;%lu;%lum"},
    {(ಪೂರ್ಣಾಂಕ)KS_8U,	"\033[58;2;%lu;%lu;%lum"},

    {(ಪೂರ್ಣಾಂಕ)KS_NAME,	ಖಾಲಿ}  // end marker
};
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_TGETENT
ಸ್ಥಿರ tcap_ನಮೂದು_T special_ಪದ[] = {
    // These are printf strings, not terminal codes.
    {(ಪೂರ್ಣಾಂಕ)KS_CF,	"\033[%dm"},
    {(ಪೂರ್ಣಾಂಕ)KS_NAME,	ಖಾಲಿ}  // end marker
};
#ಆದರೆ_ಅಂತ್ಯ

/*
 * iris-ansi for Silicon Graphics machines.
 */
ಸ್ಥಿರ tcap_ನಮೂದು_T builtin_iris_ansi[] = {
    {(ಪೂರ್ಣಾಂಕ)KS_CE,	"\033[K"},
    {(ಪೂರ್ಣಾಂಕ)KS_CD,	"\033[J"},
    {(ಪೂರ್ಣಾಂಕ)KS_AL,	"\033[L"},
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TERMINFO
    {(ಪೂರ್ಣಾಂಕ)KS_CAL,	"\033[%p1%dL"},
#ಇಲ್ಲದಿದ್ದರೆ
    {(ಪೂರ್ಣಾಂಕ)KS_CAL,	"\033[%dL"},
#ಆದರೆ_ಅಂತ್ಯ
    {(ಪೂರ್ಣಾಂಕ)KS_DL,	"\033[M"},
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TERMINFO
    {(ಪೂರ್ಣಾಂಕ)KS_CDL,	"\033[%p1%dM"},
#ಇಲ್ಲದಿದ್ದರೆ
    {(ಪೂರ್ಣಾಂಕ)KS_CDL,	"\033[%dM"},
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ 0	// The scroll region is not working as Vim expects.
# ifdef TERMINFO
    {(ಪೂರ್ಣಾಂಕ)KS_CS,	"\033[%i%p1%d;%p2%dr"},
# ಇಲ್ಲದಿದ್ದರೆ
    {(ಪೂರ್ಣಾಂಕ)KS_CS,	"\033[%i%d;%dr"},
# endif
#ಆದರೆ_ಅಂತ್ಯ
    {(ಪೂರ್ಣಾಂಕ)KS_CL,	"\033[H\033[2J"},
    {(ಪೂರ್ಣಾಂಕ)KS_VE,	"\033[9/y\033[12/y"},	// These aren't documented
    {(ಪೂರ್ಣಾಂಕ)KS_VS,	"\033[10/y\033[=1h\033[=2l"}, // These aren't documented
    {(ಪೂರ್ಣಾಂಕ)KS_TI,	"\033[=6h"},
    {(ಪೂರ್ಣಾಂಕ)KS_TE,	"\033[=6l"},
    {(ಪೂರ್ಣಾಂಕ)KS_SE,	"\033[21;27m"},
    {(ಪೂರ್ಣಾಂಕ)KS_SO,	"\033[1;7m"},
    {(ಪೂರ್ಣಾಂಕ)KS_ME,	"\033[m"},
    {(ಪೂರ್ಣಾಂಕ)KS_MR,	"\033[7m"},
    {(ಪೂರ್ಣಾಂಕ)KS_MD,	"\033[1m"},
    {(ಪೂರ್ಣಾಂಕ)KS_CCO,	"8"},			// allow 8 colors
    {(ಪೂರ್ಣಾಂಕ)KS_CZH,	"\033[3m"},		// italic mode on
    {(ಪೂರ್ಣಾಂಕ)KS_CZR,	"\033[23m"},		// italic mode off
    {(ಪೂರ್ಣಾಂಕ)KS_US,	"\033[4m"},		// underline on
    {(ಪೂರ್ಣಾಂಕ)KS_UE,	"\033[24m"},		// underline off
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TERMINFO
    {(ಪೂರ್ಣಾಂಕ)KS_CAB,	"\033[4%p1%dm"},    // set background color (ANSI)
    {(ಪೂರ್ಣಾಂಕ)KS_CAF,	"\033[3%p1%dm"},    // set foreground color (ANSI)
    {(ಪೂರ್ಣಾಂಕ)KS_CSB,	"\033[102;%p1%dm"}, // set screen background color
    {(ಪೂರ್ಣಾಂಕ)KS_CSF,	"\033[101;%p1%dm"}, // set screen foreground color
#ಇಲ್ಲದಿದ್ದರೆ
    {(ಪೂರ್ಣಾಂಕ)KS_CAB,	"\033[4%dm"},	    // set background color (ANSI)
    {(ಪೂರ್ಣಾಂಕ)KS_CAF,	"\033[3%dm"},	    // set foreground color (ANSI)
    {(ಪೂರ್ಣಾಂಕ)KS_CSB,	"\033[102;%dm"},    // set screen background color
    {(ಪೂರ್ಣಾಂಕ)KS_CSF,	"\033[101;%dm"},    // set screen foreground color
#ಆದರೆ_ಅಂತ್ಯ
    {(ಪೂರ್ಣಾಂಕ)KS_MS,	"y"},		// guessed
    {(ಪೂರ್ಣಾಂಕ)KS_UT,	"y"},		// guessed
    {(ಪೂರ್ಣಾಂಕ)KS_LE,	"\b"},
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TERMINFO
    {(ಪೂರ್ಣಾಂಕ)KS_CM,	"\033[%i%p1%d;%p2%dH"},
#ಇಲ್ಲದಿದ್ದರೆ
    {(ಪೂರ್ಣಾಂಕ)KS_CM,	"\033[%i%d;%dH"},
#ಆದರೆ_ಅಂತ್ಯ
    {(ಪೂರ್ಣಾಂಕ)KS_SR,	"\033M"},
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TERMINFO
    {(ಪೂರ್ಣಾಂಕ)KS_CRI,	"\033[%p1%dC"},
#ಇಲ್ಲದಿದ್ದರೆ
    {(ಪೂರ್ಣಾಂಕ)KS_CRI,	"\033[%dC"},
#ಆದರೆ_ಅಂತ್ಯ
    {(ಪೂರ್ಣಾಂಕ)KS_CIS,	"\033P3.y"},
    {(ಪೂರ್ಣಾಂಕ)KS_CIE,	"\234"},    // ST "String Terminator"
    {(ಪೂರ್ಣಾಂಕ)KS_TS,	"\033P1.y"},
    {(ಪೂರ್ಣಾಂಕ)KS_FS,	"\234"},    // ST "String Terminator"
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TERMINFO
    {(ಪೂರ್ಣಾಂಕ)KS_CWS,	"\033[203;%p1%d;%p2%d/y"},
    {(ಪೂರ್ಣಾಂಕ)KS_CWP,	"\033[205;%p1%d;%p2%d/y"},
#ಇಲ್ಲದಿದ್ದರೆ
    {(ಪೂರ್ಣಾಂಕ)KS_CWS,	"\033[203;%d;%d/y"},
    {(ಪೂರ್ಣಾಂಕ)KS_CWP,	"\033[205;%d;%d/y"},
#ಆದರೆ_ಅಂತ್ಯ
    {K_ಮೇಲೆ,		"\033[A"},
    {K_ಕೆಳಕ್ಕೆ,		"\033[B"},
    {K_ಎಡ,		"\033[D"},
    {K_ಬಲ,		"\033[C"},
    {K_S_ಮೇಲೆ,		"\033[161q"},
    {K_S_ಕೆಳಕ್ಕೆ,		"\033[164q"},
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
    {K_ಕೊನೆ,		"\033[146q"},
    {K_PAGEUP,		"\033[150q"},
    {K_PAGEDOWN,	"\033[154q"},

    {(ಪೂರ್ಣಾಂಕ)KS_NAME,	ಖಾಲಿ}  // end marker
};

/*
 * These codes are valid when nansi.sys or equivalent has been installed.
 * Function keys on a PC are preceded with a NUL. These are converted into
 * K_NUL '\316' in mch_inchar(), because we cannot handle NULs in key codes.
 * CTRL-arrow is used instead of SHIFT-arrow.
 */
ಸ್ಥಿರ tcap_ನಮೂದು_T builtin_pcansi[] = {
    {(ಪೂರ್ಣಾಂಕ)KS_DL,	"\033[M"},
    {(ಪೂರ್ಣಾಂಕ)KS_AL,	"\033[L"},
    {(ಪೂರ್ಣಾಂಕ)KS_CE,	"\033[K"},
    {(ಪೂರ್ಣಾಂಕ)KS_CL,	"\033[2J"},
    {(ಪೂರ್ಣಾಂಕ)KS_ME,	"\033[0m"},
    {(ಪೂರ್ಣಾಂಕ)KS_MR,	"\033[5m"},	// reverse: black on lightgrey
    {(ಪೂರ್ಣಾಂಕ)KS_MD,	"\033[1m"},	// bold: white text
    {(ಪೂರ್ಣಾಂಕ)KS_SE,	"\033[0m"},	// standout end
    {(ಪೂರ್ಣಾಂಕ)KS_SO,	"\033[31m"},	// standout: white on blue
    {(ಪೂರ್ಣಾಂಕ)KS_CZH,	"\033[34;43m"},	// italic mode: blue text on yellow
    {(ಪೂರ್ಣಾಂಕ)KS_CZR,	"\033[0m"},	// italic mode end
    {(ಪೂರ್ಣಾಂಕ)KS_US,	"\033[36;41m"},	// underscore mode: cyan text on red
    {(ಪೂರ್ಣಾಂಕ)KS_UE,	"\033[0m"},	// underscore mode end
    {(ಪೂರ್ಣಾಂಕ)KS_CCO,	"8"},		// allow 8 colors
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TERMINFO
    {(ಪೂರ್ಣಾಂಕ)KS_CAB,	"\033[4%p1%dm"},// set background color
    {(ಪೂರ್ಣಾಂಕ)KS_CAF,	"\033[3%p1%dm"},// set foreground color
#ಇಲ್ಲದಿದ್ದರೆ
    {(ಪೂರ್ಣಾಂಕ)KS_CAB,	"\033[4%dm"},	// set background color
    {(ಪೂರ್ಣಾಂಕ)KS_CAF,	"\033[3%dm"},	// set foreground color
#ಆದರೆ_ಅಂತ್ಯ
    {(ಪೂರ್ಣಾಂಕ)KS_OP,	"\033[0m"},	// reset colors
    {(ಪೂರ್ಣಾಂಕ)KS_MS,	"y"},
    {(ಪೂರ್ಣಾಂಕ)KS_UT,	"y"},		// guessed
    {(ಪೂರ್ಣಾಂಕ)KS_LE,	"\b"},
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TERMINFO
    {(ಪೂರ್ಣಾಂಕ)KS_CM,	"\033[%i%p1%d;%p2%dH"},
#ಇಲ್ಲದಿದ್ದರೆ
    {(ಪೂರ್ಣಾಂಕ)KS_CM,	"\033[%i%d;%dH"},
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TERMINFO
    {(ಪೂರ್ಣಾಂಕ)KS_CRI,	"\033[%p1%dC"},
#ಇಲ್ಲದಿದ್ದರೆ
    {(ಪೂರ್ಣಾಂಕ)KS_CRI,	"\033[%dC"},
#ಆದರೆ_ಅಂತ್ಯ
    {K_ಮೇಲೆ,		"\316H"},
    {K_ಕೆಳಕ್ಕೆ,		"\316P"},
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
    {K_ಕೊನೆ,		"\316O"},
    {K_PAGEDOWN,	"\316Q"},
    {K_PAGEUP,		"\316I"},

    {(ಪೂರ್ಣಾಂಕ)KS_NAME,	ಖಾಲಿ}  // end marker
};

/*
 * These codes are valid for the Win32 Console.  The entries that start with
 * ESC | are translated into console calls in os_win32.c.  The function keys
 * are also translated in os_win32.c.
 */
ಸ್ಥಿರ tcap_ನಮೂದು_T builtin_win32[] = {
    {(ಪೂರ್ಣಾಂಕ)KS_CE,	"\033|K"},	// clear to end of line
    {(ಪೂರ್ಣಾಂಕ)KS_AL,	"\033|L"},	// add new blank line
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TERMINFO
    {(ಪೂರ್ಣಾಂಕ)KS_CAL,	"\033|%p1%dL"},	// add number of new blank lines
#ಇಲ್ಲದಿದ್ದರೆ
    {(ಪೂರ್ಣಾಂಕ)KS_CAL,	"\033|%dL"},	// add number of new blank lines
#ಆದರೆ_ಅಂತ್ಯ
    {(ಪೂರ್ಣಾಂಕ)KS_DL,	"\033|M"},	// delete line
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TERMINFO
    {(ಪೂರ್ಣಾಂಕ)KS_CDL,	"\033|%p1%dM"},	// delete number of lines
    {(ಪೂರ್ಣಾಂಕ)KS_CSV,	"\033|%p1%d;%p2%dV"},
#ಇಲ್ಲದಿದ್ದರೆ
    {(ಪೂರ್ಣಾಂಕ)KS_CDL,	"\033|%dM"},	// delete number of lines
    {(ಪೂರ್ಣಾಂಕ)KS_CSV,	"\033|%d;%dV"},
#ಆದರೆ_ಅಂತ್ಯ
    {(ಪೂರ್ಣಾಂಕ)KS_CL,	"\033|J"},	// clear screen
    {(ಪೂರ್ಣಾಂಕ)KS_CD,	"\033|j"},	// clear to end of display
    {(ಪೂರ್ಣಾಂಕ)KS_VI,	"\033|v"},	// cursor invisible
    {(ಪೂರ್ಣಾಂಕ)KS_VE,	"\033|V"},	// cursor visible

    {(ಪೂರ್ಣಾಂಕ)KS_ME,	"\033|0m"},	//ಸಾಮಾನ್ಯ
    {(ಪೂರ್ಣಾಂಕ)KS_MR,	"\033|112m"},	// reverse: black on lightgray
    {(ಪೂರ್ಣಾಂಕ)KS_MD,	"\033|15m"},	// bold: white on black
#ಆದರೆ 1
    {(ಪೂರ್ಣಾಂಕ)KS_SO,	"\033|31m"},	// standout: white on blue
    {(ಪೂರ್ಣಾಂಕ)KS_SE,	"\033|0m"},	// standout end
#ಇಲ್ಲದಿದ್ದರೆ
    {(ಪೂರ್ಣಾಂಕ)KS_SO,	"\033|F"},	// standout: high intensity
    {(ಪೂರ್ಣಾಂಕ)KS_SE,	"\033|f"},	// standout end
#ಆದರೆ_ಅಂತ್ಯ
    {(ಪೂರ್ಣಾಂಕ)KS_CZH,	"\033|225m"},	// italic: blue text on yellow
    {(ಪೂರ್ಣಾಂಕ)KS_CZR,	"\033|0m"},	// italic end
    {(ಪೂರ್ಣಾಂಕ)KS_US,	"\033|67m"},	// underscore: cyan text on red
    {(ಪೂರ್ಣಾಂಕ)KS_UE,	"\033|0m"},	// underscore end
    {(ಪೂರ್ಣಾಂಕ)KS_CCO,	"16"},		// allow 16 colors
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TERMINFO
    {(ಪೂರ್ಣಾಂಕ)KS_CAB,	"\033|%p1%db"},	// set background color
    {(ಪೂರ್ಣಾಂಕ)KS_CAF,	"\033|%p1%df"},	// set foreground color
#ಇಲ್ಲದಿದ್ದರೆ
    {(ಪೂರ್ಣಾಂಕ)KS_CAB,	"\033|%db"},	// set background color
    {(ಪೂರ್ಣಾಂಕ)KS_CAF,	"\033|%df"},	// set foreground color
#ಆದರೆ_ಅಂತ್ಯ

    {(ಪೂರ್ಣಾಂಕ)KS_MS,	"y"},		// save to move cur in reverse mode
    {(ಪೂರ್ಣಾಂಕ)KS_UT,	"y"},
    {(ಪೂರ್ಣಾಂಕ)KS_XN,	"y"},
    {(ಪೂರ್ಣಾಂಕ)KS_LE,	"\b"},
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TERMINFO
    {(ಪೂರ್ಣಾಂಕ)KS_CM,	"\033|%i%p1%d;%p2%dH"}, // cursor motion
#ಇಲ್ಲದಿದ್ದರೆ
    {(ಪೂರ್ಣಾಂಕ)KS_CM,	"\033|%i%d;%dH"}, // cursor motion
#ಆದರೆ_ಅಂತ್ಯ
    {(ಪೂರ್ಣಾಂಕ)KS_VB,	"\033|B"},	// visual bell
    {(ಪೂರ್ಣಾಂಕ)KS_TI,	"\033|S"},	// put terminal in termcap mode
    {(ಪೂರ್ಣಾಂಕ)KS_TE,	"\033|E"},	// out of termcap mode
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TERMINFO
    {(ಪೂರ್ಣಾಂಕ)KS_CS,	"\033|%i%p1%d;%p2%dr"}, // scroll region
#ಇಲ್ಲದಿದ್ದರೆ
    {(ಪೂರ್ಣಾಂಕ)KS_CS,	"\033|%i%d;%dr"}, // scroll region
#ಆದರೆ_ಅಂತ್ಯ

    {K_ಮೇಲೆ,		"\316H"},
    {K_ಕೆಳಕ್ಕೆ,		"\316P"},
    {K_ಎಡ,		"\316K"},
    {K_ಬಲ,		"\316M"},
    {K_S_ಮೇಲೆ,		"\316\304"},
    {K_S_ಕೆಳಕ್ಕೆ,		"\316\317"},
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
    {K_ಕೊನೆ,		"\316O"},
    {K_S_ಕೊನೆ,		"\316\315"},
    {K_C_ಕೊನೆ,		"\316u"},
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

    {(ಪೂರ್ಣಾಂಕ)KS_NAME,	ಖಾಲಿ}  // end marker
};

#ಆದರೆ defined(FEAT_GUI)
/*
 * GUI uses made-up codes, only used inside Vim.
 */
ಸ್ಥಿರ tcap_ನಮೂದು_T builtin_gui[] = {
    {(ಪೂರ್ಣಾಂಕ)KS_CE,	"\033|$"},
    {(ಪೂರ್ಣಾಂಕ)KS_AL,	"\033|i"},
# ifdef TERMINFO
    {(ಪೂರ್ಣಾಂಕ)KS_CAL,	"\033|%p1%dI"},
# ಇಲ್ಲದಿದ್ದರೆ
    {(ಪೂರ್ಣಾಂಕ)KS_CAL,	"\033|%dI"},
# endif
    {(ಪೂರ್ಣಾಂಕ)KS_DL,	"\033|d"},
# ifdef TERMINFO
    {(ಪೂರ್ಣಾಂಕ)KS_CDL,	"\033|%p1%dD"},
    {(ಪೂರ್ಣಾಂಕ)KS_CS,	"\033|%p1%d;%p2%dR"},
    {(ಪೂರ್ಣಾಂಕ)KS_CSV,	"\033|%p1%d;%p2%dV"},
# ಇಲ್ಲದಿದ್ದರೆ
    {(ಪೂರ್ಣಾಂಕ)KS_CDL,	"\033|%dD"},
    {(ಪೂರ್ಣಾಂಕ)KS_CS,	"\033|%d;%dR"},
    {(ಪೂರ್ಣಾಂಕ)KS_CSV,	"\033|%d;%dV"},
# endif
    {(ಪೂರ್ಣಾಂಕ)KS_CL,	"\033|C"},
			// attributes switched on with 'h', off with * 'H'
    {(ಪೂರ್ಣಾಂಕ)KS_ME,	"\033|31H"}, // HL_ALL
    {(ಪೂರ್ಣಾಂಕ)KS_MR,	"\033|1h"},   // HL_INVERSE
    {(ಪೂರ್ಣಾಂಕ)KS_MD,	"\033|2h"},   // HL_BOLD
    {(ಪೂರ್ಣಾಂಕ)KS_SE,	"\033|16H"}, // HL_STANDOUT
    {(ಪೂರ್ಣಾಂಕ)KS_SO,	"\033|16h"}, // HL_STANDOUT
    {(ಪೂರ್ಣಾಂಕ)KS_UE,	"\033|8H"},   // HL_UNDERLINE
    {(ಪೂರ್ಣಾಂಕ)KS_US,	"\033|8h"},   // HL_UNDERLINE
    {(ಪೂರ್ಣಾಂಕ)KS_UCE,	"\033|8C"},   // HL_UNDERCURL
    {(ಪೂರ್ಣಾಂಕ)KS_UCS,	"\033|8c"},   // HL_UNDERCURL
    {(ಪೂರ್ಣಾಂಕ)KS_STE,	"\033|4C"},   // HL_STRIKETHROUGH
    {(ಪೂರ್ಣಾಂಕ)KS_STS,	"\033|4c"},   // HL_STRIKETHROUGH
    {(ಪೂರ್ಣಾಂಕ)KS_CZR,	"\033|4H"},   // HL_ITALIC
    {(ಪೂರ್ಣಾಂಕ)KS_CZH,	"\033|4h"},   // HL_ITALIC
    {(ಪೂರ್ಣಾಂಕ)KS_VB,	"\033|f"},
    {(ಪೂರ್ಣಾಂಕ)KS_MS,	"y"},
    {(ಪೂರ್ಣಾಂಕ)KS_UT,	"y"},
    {(ಪೂರ್ಣಾಂಕ)KS_XN,	"y"},
    {(ಪೂರ್ಣಾಂಕ)KS_LE,	"\b"},		// cursor-left = BS
    {(ಪೂರ್ಣಾಂಕ)KS_ND,	"\014"},	// cursor-right = CTRL-L
# ifdef TERMINFO
    {(ಪೂರ್ಣಾಂಕ)KS_CM,	"\033|%p1%d;%p2%dM"},
# ಇಲ್ಲದಿದ್ದರೆ
    {(ಪೂರ್ಣಾಂಕ)KS_CM,	"\033|%d;%dM"},
# endif
	// there are no key sequences here, the GUI sequences are recognized
	// in check_termcode()

    {(ಪೂರ್ಣಾಂಕ)KS_NAME,	ಖಾಲಿ}  // end marker
};
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Amiga console window, default for Amiga.
 */
ಸ್ಥಿರ tcap_ನಮೂದು_T builtin_amiga[] = {
    {(ಪೂರ್ಣಾಂಕ)KS_CE,	"\033[K"},
    {(ಪೂರ್ಣಾಂಕ)KS_CD,	"\033[J"},
    {(ಪೂರ್ಣಾಂಕ)KS_AL,	"\033[L"},
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TERMINFO
    {(ಪೂರ್ಣಾಂಕ)KS_CAL,	"\033[%p1%dL"},
#ಇಲ್ಲದಿದ್ದರೆ
    {(ಪೂರ್ಣಾಂಕ)KS_CAL,	"\033[%dL"},
#ಆದರೆ_ಅಂತ್ಯ
    {(ಪೂರ್ಣಾಂಕ)KS_DL,	"\033[M"},
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TERMINFO
    {(ಪೂರ್ಣಾಂಕ)KS_CDL,	"\033[%p1%dM"},
#ಇಲ್ಲದಿದ್ದರೆ
    {(ಪೂರ್ಣಾಂಕ)KS_CDL,	"\033[%dM"},
#ಆದರೆ_ಅಂತ್ಯ
    {(ಪೂರ್ಣಾಂಕ)KS_CL,	"\014"},
    {(ಪೂರ್ಣಾಂಕ)KS_VI,	"\033[0 p"},
    {(ಪೂರ್ಣಾಂಕ)KS_VE,	"\033[1 p"},
    {(ಪೂರ್ಣಾಂಕ)KS_ME,	"\033[0m"},
    {(ಪೂರ್ಣಾಂಕ)KS_MR,	"\033[7m"},
    {(ಪೂರ್ಣಾಂಕ)KS_MD,	"\033[1m"},
    {(ಪೂರ್ಣಾಂಕ)KS_SE,	"\033[0m"},
    {(ಪೂರ್ಣಾಂಕ)KS_SO,	"\033[33m"},
    {(ಪೂರ್ಣಾಂಕ)KS_US,	"\033[4m"},
    {(ಪೂರ್ಣಾಂಕ)KS_UE,	"\033[0m"},
    {(ಪೂರ್ಣಾಂಕ)KS_CZH,	"\033[3m"},
    {(ಪೂರ್ಣಾಂಕ)KS_CZR,	"\033[0m"},
#ಆದರೆ defined(__amigaos4__) || defined(__MORPHOS__) || defined(__AROS__)
    {(ಪೂರ್ಣಾಂಕ)KS_CCO,	"8"},		// allow 8 colors
# ifdef TERMINFO
    {(ಪೂರ್ಣಾಂಕ)KS_CAB,	"\033[4%p1%dm"},// set background color
    {(ಪೂರ್ಣಾಂಕ)KS_CAF,	"\033[3%p1%dm"},// set foreground color
# ಇಲ್ಲದಿದ್ದರೆ
    {(ಪೂರ್ಣಾಂಕ)KS_CAB,	"\033[4%dm"},	// set background color
    {(ಪೂರ್ಣಾಂಕ)KS_CAF,	"\033[3%dm"},	// set foreground color
# endif
    {(ಪೂರ್ಣಾಂಕ)KS_OP,	"\033[m"},	// reset colors
#ಆದರೆ_ಅಂತ್ಯ
    {(ಪೂರ್ಣಾಂಕ)KS_MS,	"y"},
    {(ಪೂರ್ಣಾಂಕ)KS_UT,	"y"},		// guessed
    {(ಪೂರ್ಣಾಂಕ)KS_LE,	"\b"},
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TERMINFO
    {(ಪೂರ್ಣಾಂಕ)KS_CM,	"\033[%i%p1%d;%p2%dH"},
#ಇಲ್ಲದಿದ್ದರೆ
    {(ಪೂರ್ಣಾಂಕ)KS_CM,	"\033[%i%d;%dH"},
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(__MORPHOS__)
    {(ಪೂರ್ಣಾಂಕ)KS_SR,	"\033M"},
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TERMINFO
    {(ಪೂರ್ಣಾಂಕ)KS_CRI,	"\033[%p1%dC"},
#ಇಲ್ಲದಿದ್ದರೆ
    {(ಪೂರ್ಣಾಂಕ)KS_CRI,	"\033[%dC"},
#ಆದರೆ_ಅಂತ್ಯ
    {K_ಮೇಲೆ,		"\233A"},
    {K_ಕೆಳಕ್ಕೆ,		"\233B"},
    {K_ಎಡ,		"\233D"},
    {K_ಬಲ,		"\233C"},
    {K_S_ಮೇಲೆ,		"\233T"},
    {K_S_ಕೆಳಕ್ಕೆ,		"\233S"},
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
    {K_ಕೊನೆ,		"\233\064\065~"},	// 101 key keyboard

    {BT_EXTRA_ಕೀಲಿಗಳು,	""},
    {TERMCAP2_ಕೀಲಿ('#', '2'), "\233\065\064~"},	// shifted home key
    {TERMCAP2_ಕೀಲಿ('#', '3'), "\233\065\060~"},	// shifted insert key
    {TERMCAP2_ಕೀಲಿ('*', '7'), "\233\065\065~"},	// shifted end key

    {(ಪೂರ್ಣಾಂಕ)KS_NAME,	ಖಾಲಿ}  // end marker
};

/*
 * The most minimal terminal: only clear screen and cursor positioning.
 */
ಸ್ಥಿರ tcap_ನಮೂದು_T builtin_dumb[] = {
    {(ಪೂರ್ಣಾಂಕ)KS_CL,	"\014"},
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TERMINFO
    {(ಪೂರ್ಣಾಂಕ)KS_CM,	"\033[%i%p1%d;%p2%dH"},
#ಇಲ್ಲದಿದ್ದರೆ
    {(ಪೂರ್ಣಾಂಕ)KS_CM,	"\033[%i%d;%dH"},
#ಆದರೆ_ಅಂತ್ಯ

    {(ಪೂರ್ಣಾಂಕ)KS_NAME,	ಖಾಲಿ}  // end marker
};

/*
 * Terminal used for debugging.
 */
ಸ್ಥಿರ tcap_ನಮೂದು_T builtin_debug[] = {
    {(ಪೂರ್ಣಾಂಕ)KS_CE,	"[CE]"},
    {(ಪೂರ್ಣಾಂಕ)KS_CD,	"[CD]"},
    {(ಪೂರ್ಣಾಂಕ)KS_AL,	"[AL]"},
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TERMINFO
    {(ಪೂರ್ಣಾಂಕ)KS_CAL,	"[CAL%p1%d]"},
#ಇಲ್ಲದಿದ್ದರೆ
    {(ಪೂರ್ಣಾಂಕ)KS_CAL,	"[CAL%d]"},
#ಆದರೆ_ಅಂತ್ಯ
    {(ಪೂರ್ಣಾಂಕ)KS_DL,	"[DL]"},
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TERMINFO
    {(ಪೂರ್ಣಾಂಕ)KS_CDL,	"[CDL%p1%d]"},
#ಇಲ್ಲದಿದ್ದರೆ
    {(ಪೂರ್ಣಾಂಕ)KS_CDL,	"[CDL%d]"},
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TERMINFO
    {(ಪೂರ್ಣಾಂಕ)KS_CS,	"[%p1%dCS%p2%d]"},
#ಇಲ್ಲದಿದ್ದರೆ
    {(ಪೂರ್ಣಾಂಕ)KS_CS,	"[%dCS%d]"},
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TERMINFO
    {(ಪೂರ್ಣಾಂಕ)KS_CSV,	"[%p1%dCSV%p2%d]"},
#ಇಲ್ಲದಿದ್ದರೆ
    {(ಪೂರ್ಣಾಂಕ)KS_CSV,	"[%dCSV%d]"},
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TERMINFO
    {(ಪೂರ್ಣಾಂಕ)KS_CAB,	"[CAB%p1%d]"},
    {(ಪೂರ್ಣಾಂಕ)KS_CAF,	"[CAF%p1%d]"},
    {(ಪೂರ್ಣಾಂಕ)KS_CSB,	"[CSB%p1%d]"},
    {(ಪೂರ್ಣಾಂಕ)KS_CSF,	"[CSF%p1%d]"},
#ಇಲ್ಲದಿದ್ದರೆ
    {(ಪೂರ್ಣಾಂಕ)KS_CAB,	"[CAB%d]"},
    {(ಪೂರ್ಣಾಂಕ)KS_CAF,	"[CAF%d]"},
    {(ಪೂರ್ಣಾಂಕ)KS_CSB,	"[CSB%d]"},
    {(ಪೂರ್ಣಾಂಕ)KS_CSF,	"[CSF%d]"},
#ಆದರೆ_ಅಂತ್ಯ
    {(ಪೂರ್ಣಾಂಕ)KS_CAU,	"[CAU%d]"},
    {(ಪೂರ್ಣಾಂಕ)KS_OP,	"[OP]"},
    {(ಪೂರ್ಣಾಂಕ)KS_LE,	"[LE]"},
    {(ಪೂರ್ಣಾಂಕ)KS_CL,	"[CL]"},
    {(ಪೂರ್ಣಾಂಕ)KS_VI,	"[VI]"},
    {(ಪೂರ್ಣಾಂಕ)KS_VE,	"[VE]"},
    {(ಪೂರ್ಣಾಂಕ)KS_VS,	"[VS]"},
    {(ಪೂರ್ಣಾಂಕ)KS_ME,	"[ME]"},
    {(ಪೂರ್ಣಾಂಕ)KS_MR,	"[MR]"},
    {(ಪೂರ್ಣಾಂಕ)KS_MB,	"[MB]"},
    {(ಪೂರ್ಣಾಂಕ)KS_MD,	"[MD]"},
    {(ಪೂರ್ಣಾಂಕ)KS_SE,	"[SE]"},
    {(ಪೂರ್ಣಾಂಕ)KS_SO,	"[SO]"},
    {(ಪೂರ್ಣಾಂಕ)KS_UE,	"[UE]"},
    {(ಪೂರ್ಣಾಂಕ)KS_US,	"[US]"},
    {(ಪೂರ್ಣಾಂಕ)KS_UCE,	"[UCE]"},
    {(ಪೂರ್ಣಾಂಕ)KS_UCS,	"[UCS]"},
    {(ಪೂರ್ಣಾಂಕ)KS_USS,	"[USS]"},
    {(ಪೂರ್ಣಾಂಕ)KS_DS,	"[DS]"},
    {(ಪೂರ್ಣಾಂಕ)KS_CDS,	"[CDS]"},
    {(ಪೂರ್ಣಾಂಕ)KS_STE,	"[STE]"},
    {(ಪೂರ್ಣಾಂಕ)KS_STS,	"[STS]"},
    {(ಪೂರ್ಣಾಂಕ)KS_MS,	"[MS]"},
    {(ಪೂರ್ಣಾಂಕ)KS_UT,	"[UT]"},
    {(ಪೂರ್ಣಾಂಕ)KS_XN,	"[XN]"},
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TERMINFO
    {(ಪೂರ್ಣಾಂಕ)KS_CM,	"[%p1%dCM%p2%d]"},
#ಇಲ್ಲದಿದ್ದರೆ
    {(ಪೂರ್ಣಾಂಕ)KS_CM,	"[%dCM%d]"},
#ಆದರೆ_ಅಂತ್ಯ
    {(ಪೂರ್ಣಾಂಕ)KS_SR,	"[SR]"},
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TERMINFO
    {(ಪೂರ್ಣಾಂಕ)KS_CRI,	"[CRI%p1%d]"},
#ಇಲ್ಲದಿದ್ದರೆ
    {(ಪೂರ್ಣಾಂಕ)KS_CRI,	"[CRI%d]"},
#ಆದರೆ_ಅಂತ್ಯ
    {(ಪೂರ್ಣಾಂಕ)KS_VB,	"[VB]"},
    {(ಪೂರ್ಣಾಂಕ)KS_KS,	"[KS]"},
    {(ಪೂರ್ಣಾಂಕ)KS_KE,	"[KE]"},
    {(ಪೂರ್ಣಾಂಕ)KS_TI,	"[TI]"},
    {(ಪೂರ್ಣಾಂಕ)KS_TE,	"[TE]"},
    {(ಪೂರ್ಣಾಂಕ)KS_CIS,	"[CIS]"},
    {(ಪೂರ್ಣಾಂಕ)KS_CIE,	"[CIE]"},
    {(ಪೂರ್ಣಾಂಕ)KS_CSC,	"[CSC]"},
    {(ಪೂರ್ಣಾಂಕ)KS_CEC,	"[CEC]"},
    {(ಪೂರ್ಣಾಂಕ)KS_TS,	"[TS]"},
    {(ಪೂರ್ಣಾಂಕ)KS_FS,	"[FS]"},
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TERMINFO
    {(ಪೂರ್ಣಾಂಕ)KS_CWS,	"[%p1%dCWS%p2%d]"},
    {(ಪೂರ್ಣಾಂಕ)KS_CWP,	"[%p1%dCWP%p2%d]"},
#ಇಲ್ಲದಿದ್ದರೆ
    {(ಪೂರ್ಣಾಂಕ)KS_CWS,	"[%dCWS%d]"},
    {(ಪೂರ್ಣಾಂಕ)KS_CWP,	"[%dCWP%d]"},
#ಆದರೆ_ಅಂತ್ಯ
    {(ಪೂರ್ಣಾಂಕ)KS_CRV,	"[CRV]"},
    {(ಪೂರ್ಣಾಂಕ)KS_CXM,	"[CXM]"},
    {(ಪೂರ್ಣಾಂಕ)KS_U7,	"[U7]"},
    {(ಪೂರ್ಣಾಂಕ)KS_RFG,	"[RFG]"},
    {(ಪೂರ್ಣಾಂಕ)KS_RBG,	"[RBG]"},
    {(ಪೂರ್ಣಾಂಕ)KS_CF,	"[CF%d]"},
    {K_ಮೇಲೆ,		"[KU]"},
    {K_ಕೆಳಕ್ಕೆ,		"[KD]"},
    {K_ಎಡ,		"[KL]"},
    {K_ಬಲ,		"[KR]"},
    {K_XUP,		"[xKU]"},
    {K_XDOWN,		"[xKD]"},
    {K_XLEFT,		"[xKL]"},
    {K_XRIGHT,		"[xKR]"},
    {K_S_ಮೇಲೆ,		"[S-KU]"},
    {K_S_ಕೆಳಕ್ಕೆ,		"[S-KD]"},
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
    {K_ಕೊನೆ,		"[END]"},
    {K_S_ಕೊನೆ,		"[C-END]"},
    {K_C_ಕೊನೆ,		"[C-END]"},
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

    {(ಪೂರ್ಣಾಂಕ)KS_NAME,	ಖಾಲಿ}  // end marker
};

/*
 * List of builtin terminals.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಅಕ್ಷರ	    *bitc_name;	    // name, such as "xterm"
    tcap_ನಮೂದು_T    *bitc_ಕೋಷ್ಟಕ;    // table with entries for bitc_name
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
#ಆದರೆ defined(FEAT_GUI)
    {"gui",	    builtin_gui},
#ಆದರೆ_ಅಂತ್ಯ
    {"amiga",	    builtin_amiga},
    {"dumb",	    builtin_dumb},
    {"debug",	    builtin_debug},

    {ಖಾಲಿ,	    ಖಾಲಿ},  // end marker
};

#ಆದರೆ defined(FEAT_TERMGUICOLORS)
    ಸ್ಥಿರ guicolor_T
termgui_mch_get_color(char_u *name)
{
    ಹಿಂತಿರುಗಿ gui_get_color_cmn(name);
}

    guicolor_T
termgui_get_color(char_u *name)
{
    guicolor_T	t;

    ಆದರೆ (*name == NUL)
	ಹಿಂತಿರುಗಿ INVALCOLOR;
    t = termgui_mch_get_color(name);

    ಆದರೆ (t == INVALCOLOR)
	semsg(_(e_cannot_allocate_color_str), name);
    ಹಿಂತಿರುಗಿ t;
}

    guicolor_T
termgui_mch_get_rgb(guicolor_T color)
{
    ಹಿಂತಿರುಗಿ color;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * DEFAULT_TERM is used, when no terminal is specified with -T option or $TERM.
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ AMIGA
# define DEFAULT_ಪದ	(char_u *)"amiga"
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
# define DEFAULT_ಪದ	(char_u *)"win32"
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(UNIX)
# define DEFAULT_ಪದ	(char_u *)"ansi"
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ VMS
# define DEFAULT_ಪದ	(char_u *)"vt320"
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __HAIKU__
# undef DEFAULT_ಪದ
# define DEFAULT_ಪದ	(char_u *)"xterm"
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ DEFAULT_ಪದ
# define DEFAULT_ಪದ	(char_u *)"dumb"
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Term_strings contains currently used terminal output strings.
 * It is initialized with the default values by parse_builtin_tcap().
 * The values can be changed by setting the option with the same name.
 */
char_u *(ಪದ_ಅಕ್ಷರಸರಣಿಗಳು[(ಪೂರ್ಣಾಂಕ)KS_ಕೊನೆಯ + 1]);

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	need_gather = FALSE;	    // need to fill termleader[]
ಸ್ಥಿರ char_u	termleader[256 + 1];	    // for check_termcode()
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMRESPONSE
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	check_for_codes = FALSE;    // check for key code response
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Structure and table to store terminal features that can be detected by
 * querying the terminal.  Either by inspecting the termresponse or a more
 * specific request.  Besides this there are:
 * t_colors - number of colors supported
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಅಕ್ಷರ    *tpr_name;
    ಪೂರ್ಣಾಂಕ	    tpr_ಗಣ_by_termresponse;
    ಪೂರ್ಣಾಂಕ	    tpr_ಸ್ಥಿತಿ;
} termprop_T;

// Values for tpr_status.
#ವ್ಯಾಖ್ಯಾನಿಸು TPR_UNKNOWN	    'u'
#ವ್ಯಾಖ್ಯಾನಿಸು TPR_YES		    'y'
#ವ್ಯಾಖ್ಯಾನಿಸು TPR_NO		    'n'
#ವ್ಯಾಖ್ಯಾನಿಸು TPR_MOUSE_XTERM     'x'	// use "xterm" for 'ttymouse'
#ವ್ಯಾಖ್ಯಾನಿಸು TPR_MOUSE_XTERM2    '2'	// use "xterm2" for 'ttymouse'
#ವ್ಯಾಖ್ಯಾನಿಸು TPR_MOUSE_SGR	    's'	// use "sgr" for 'ttymouse'

// can request the cursor style without messing up the display
#ವ್ಯಾಖ್ಯಾನಿಸು TPR_CURSOR_STYLE	    0
// can request the cursor blink mode without messing up the display
#ವ್ಯಾಖ್ಯಾನಿಸು TPR_CURSOR_BLINK	    1
// can set the underline color with t_8u without resetting other colors
#ವ್ಯಾಖ್ಯಾನಿಸು TPR_UNDERLINE_RGB	    2
// mouse support - TPR_MOUSE_XTERM, TPR_MOUSE_XTERM2 or TPR_MOUSE_SGR
#ವ್ಯಾಖ್ಯಾನಿಸು TPR_MOUSE		    3
// term response indicates kitty
#ವ್ಯಾಖ್ಯಾನಿಸು TPR_KITTY		    4
// can send DECRQM requests to terminal
#ವ್ಯಾಖ್ಯಾನಿಸು TPR_DECRQM		    5
// true color, from the color count of the terminal entry
#ವ್ಯಾಖ್ಯಾನಿಸು TPR_RGB			    6
// table size
#ವ್ಯಾಖ್ಯಾನಿಸು TPR_ಎಣಿಕೆ		    7

ಸ್ಥಿರ termprop_T ಪದ_props[TPR_ಎಣಿಕೆ];

/*
 * Set the "rgb" terminal property from the color count.  The entry of a
 * direct color terminal, such as xterm-direct, has 16777216 colors.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಗಣ_rgb_ಪದ_prop(ಶೂನ್ಯ)
{
    ಪದ_props[TPR_RGB].tpr_ಸ್ಥಿತಿ = t_colors == 0x1000000
						       ? TPR_YES : TPR_UNKNOWN;
}

/*
 * Initialize the term_props table.
 * When "all" is FALSE only set those that are detected from the version
 * response.
 */
    ಶೂನ್ಯ
init_ಪದ_props(ಪೂರ್ಣಾಂಕ all)
{
    ಪೂರ್ಣಾಂಕ i;

    ಪದ_props[TPR_CURSOR_STYLE].tpr_name = "cursor_style";
    ಪದ_props[TPR_CURSOR_STYLE].tpr_ಗಣ_by_termresponse = FALSE;
    ಪದ_props[TPR_CURSOR_BLINK].tpr_name = "cursor_blink_mode";
    ಪದ_props[TPR_CURSOR_BLINK].tpr_ಗಣ_by_termresponse = FALSE;
    ಪದ_props[TPR_UNDERLINE_RGB].tpr_name = "underline_rgb";
    ಪದ_props[TPR_UNDERLINE_RGB].tpr_ಗಣ_by_termresponse = TRUE;
    ಪದ_props[TPR_MOUSE].tpr_name = "mouse";
    ಪದ_props[TPR_MOUSE].tpr_ಗಣ_by_termresponse = TRUE;
    ಪದ_props[TPR_KITTY].tpr_name = "kitty";
    ಪದ_props[TPR_KITTY].tpr_ಗಣ_by_termresponse = FALSE;
    ಪದ_props[TPR_DECRQM].tpr_name = "decrqm";
    ಪದ_props[TPR_DECRQM].tpr_ಗಣ_by_termresponse = TRUE;
    ಪದ_props[TPR_RGB].tpr_name = "rgb";
    ಪದ_props[TPR_RGB].tpr_ಗಣ_by_termresponse = FALSE;

    ಗಾಗಿ (i = 0; i < TPR_ಎಣಿಕೆ; ++i)
	ಆದರೆ (all || ಪದ_props[i].tpr_ಗಣ_by_termresponse)
	    ಪದ_props[i].tpr_ಸ್ಥಿತಿ = TPR_UNKNOWN;

    // Derived from the color count, not reset.
    ಗಣ_rgb_ಪದ_prop();
}

#ಆದರೆ defined(FEAT_EVAL)
    ಶೂನ್ಯ
f_terminalprops(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
# ifdef FEAT_TERMRESPONSE
    ಪೂರ್ಣಾಂಕ i;
# endif

    ಆದರೆ (rettv_dict_alloc(rettv) == FAIL)
	ಹಿಂತಿರುಗಿ;
# ifdef FEAT_TERMRESPONSE
    ಗಾಗಿ (i = 0; i < TPR_ಎಣಿಕೆ; ++i)
    {
	char_u	ಮೌಲ್ಯ[2];

	ಮೌಲ್ಯ[0] = ಪದ_props[i].tpr_ಸ್ಥಿತಿ;
	ಮೌಲ್ಯ[1] = NUL;
	dict_ಕೂಡಿಸು_string_len(rettv->vval.v_dict, ಪದ_props[i].tpr_name,
	    ಮೌಲ್ಯ, (ಮೌಲ್ಯ[0] == NUL) ? 0 : 1);
    }
# endif
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Find the builtin termcap entries for "term".
 * This also recognizes similar names.  E.g. "xterm-256color" finds the "xterm"
 * entry.
 * Returns NULL when "term" is not found.
 */
    ಸ್ಥಿರ tcap_ನಮೂದು_T *
ಕಂಡುಹಿಡಿ_builtin_ಪದ(char_u *ಪದ)
{
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; ; ++i)
    {
	char_u *name = (char_u *)builtin_terminals[i].bitc_name;
	ಆದರೆ (name == ಖಾಲಿ)  // end marker
	    ನಿಲ್ಲಿಸು;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
	ಆದರೆ (STRCMP(name, "iris-ansi") == 0 && vim_is_iris(ಪದ))
	    ಹಿಂತಿರುಗಿ builtin_terminals[i].bitc_ಕೋಷ್ಟಕ;
	ಆದರೆ (STRCMP(name, "xterm") == 0 && vim_is_xterm(ಪದ))
	    ಹಿಂತಿರುಗಿ builtin_terminals[i].bitc_ಕೋಷ್ಟಕ;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ VMS
	ಆದರೆ (STRCMP(name, "vt320") == 0 && vim_is_vt300(ಪದ))
	    ಹಿಂತಿರುಗಿ builtin_terminals[i].bitc_ಕೋಷ್ಟಕ;
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ (STRCMP(ಪದ, name) == 0)
	    ಹಿಂತಿರುಗಿ builtin_terminals[i].bitc_ಕೋಷ್ಟಕ;
    }
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Apply entries from a builtin termcap.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
apply_builtin_tcap(char_u *ಪದ, tcap_ನಮೂದು_T *entries, ಪೂರ್ಣಾಂಕ overwrite)
{
    ಪೂರ್ಣಾಂಕ ಪದ_8bit = ಪದ_is_8bit(ಪದ);

    ಗಾಗಿ (tcap_ನಮೂದು_T *p = entries;
	      p->bt_ನಮೂದು != (ಪೂರ್ಣಾಂಕ)KS_NAME && p->bt_ನಮೂದು != BT_EXTRA_ಕೀಲಿಗಳು; ++p)
    {
	ಆದರೆ ((ಪೂರ್ಣಾಂಕ)p->bt_ನಮೂದು >= 0)	// KS_xx entry
	{
	    // Only set the value if it wasn't set yet or "overwrite" is TRUE.
	    ಆದರೆ (ಪದ_ಅಕ್ಷರಸರಣಿಗಳು[p->bt_ನಮೂದು] == ಖಾಲಿ
		    || ಪದ_ಅಕ್ಷರಸರಣಿಗಳು[p->bt_ನಮೂದು] == ಖಾಲಿಗಳು_option
		    || overwrite)
	    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
		ಪೂರ್ಣಾಂಕ opt_idx = -1;
#ಆದರೆ_ಅಂತ್ಯ
		// 8bit terminal: use CSI instead of <Esc>[
		ಆದರೆ (ಪದ_8bit && ಪದ_7to8bit((char_u *)p->bt_string) != 0)
		{
		    char_u  *s, *t;

		    s = vim_strsave((char_u *)p->bt_string);
		    ಆದರೆ (s != ಖಾಲಿ)
		    {
			ಗಾಗಿ (t = s; *t; ++t)
			    ಆದರೆ (ಪದ_7to8bit(t))
			    {
				*t = ಪದ_7to8bit(t);
				STRMOVE(t + 1, t + 2);
			    }
			ಪದ_ಅಕ್ಷರಸರಣಿಗಳು[p->bt_ನಮೂದು] = s;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
			opt_idx =
#ಆದರೆ_ಅಂತ್ಯ
				  ಗಣ_ಪದ_option_alloced(
						   &ಪದ_ಅಕ್ಷರಸರಣಿಗಳು[p->bt_ನಮೂದು]);
		    }
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ಪದ_ಅಕ್ಷರಸರಣಿಗಳು[p->bt_ನಮೂದು] = (char_u *)p->bt_string;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
		    opt_idx = get_ಪದ_opt_idx(&ಪದ_ಅಕ್ಷರಸರಣಿಗಳು[p->bt_ನಮೂದು]);
#ಆದರೆ_ಅಂತ್ಯ
		}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
		ಗಣ_ಪದ_option_sctx_idx(ಖಾಲಿ, opt_idx);
#ಆದರೆ_ಅಂತ್ಯ
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    char_u  name[2];
	    name[0] = KEY2TERMCAP0((ಪೂರ್ಣಾಂಕ)p->bt_ನಮೂದು);
	    name[1] = KEY2TERMCAP1((ಪೂರ್ಣಾಂಕ)p->bt_ನಮೂದು);
	    ಆದರೆ (ಕಂಡುಹಿಡಿ_termcode(name) == ಖಾಲಿ || overwrite)
		ಕೂಡಿಸು_termcode(name, (char_u *)p->bt_string, ಪದ_8bit);
	}
    }
}

/*
 * Apply builtin termcap entries for a given keyprotocol.
 */
    ಶೂನ್ಯ
apply_keyprotocol(char_u *ಪದ, keyprot_T prot)
{
    ಆದರೆ (prot == KEYPROTOCOL_KITTY)
	apply_builtin_tcap(ಪದ, builtin_kitty, TRUE);
    ಆದರೆ (prot == KEYPROTOCOL_MOK2)
	apply_builtin_tcap(ಪದ, builtin_mok2, TRUE);

    ಆದರೆ (prot != KEYPROTOCOL_NONE)
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
parse_builtin_tcap(char_u *ಪದ)
{
    tcap_ನಮೂದು_T *entries = ಕಂಡುಹಿಡಿ_builtin_ಪದ(ಪದ);
    ಆದರೆ (entries != ಖಾಲಿ)
	apply_builtin_tcap(ಪದ, entries, FALSE);
}

/*
 * Set number of colors.
 * Store it as a number in t_colors.
 * Store it as a string in T_CCO (using nr_colors[]).
 */
    ಶೂನ್ಯ
ಗಣ_color_ಎಣಿಕೆ(ಪೂರ್ಣಾಂಕ nr)
{
    char_u	nr_colors[20];		// string for number of colors

    t_colors = nr;
    ಆದರೆ (t_colors > 1)
	sprintf((ಅಕ್ಷರ *)nr_colors, "%d", t_colors);
    ಇಲ್ಲದಿದ್ದರೆ
	*nr_colors = NUL;
#ಆದರೆ 0
# ifdef FEAT_TERMGUICOLORS
    // xterm-direct, enable termguicolors, when it wasn't set yet
    ಆದರೆ (t_colors == 0x1000000 && !p_tgc_ಗಣ)
	ಗಣ_option_ಮೌಲ್ಯ((char_u *)"termguicolors", 1L, ಖಾಲಿ, 0);
# endif
#ಆದರೆ_ಅಂತ್ಯ
    ಗಣ_string_option_ನೇರ((char_u *)"t_Co", -1, nr_colors, OPT_FREE, 0);
}

/*
 * Set the color count to "val" and redraw if it changed.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
may_adjust_color_ಎಣಿಕೆ(ಪೂರ್ಣಾಂಕ val)
{
    ಆದರೆ (val == t_colors)
	ಹಿಂತಿರುಗಿ;

    // Nr of colors changed, initialize highlighting and redraw everything.
    // This causes a redraw, which usually clears the message.  Try keeping
    // the message if it might work.
    ಗಣ_keep_ಸಂದೇಶ_from_hist();
    ಗಣ_color_ಎಣಿಕೆ(val);
    init_highlight(TRUE, FALSE);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG_TERMRESPONSE
    {
	ಪೂರ್ಣಾಂಕ r = redraw_asap(UPD_CLEAR);

	LOG_TRN("Received t_Co, redraw_asap(): %d", r);
    }
#ಇಲ್ಲದಿದ್ದರೆ
    redraw_asap(UPD_CLEAR);
#ಆದರೆ_ಅಂತ್ಯ
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_TGETENT
ಸ್ಥಿರ ಅಕ್ಷರ *(ಕೀಲಿ_ಹೆಸರುಗಳು[]) =
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
    ಖಾಲಿ
};
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(HAVE_TGETENT) || defined(FEAT_TERMGUICOLORS)
/*
 * Return TRUE if "term_strings[idx]" was not set.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಪದ_ಅಕ್ಷರಸರಣಿಗಳು_not_ಗಣ(ಎಣಿಕೆ Special_ಕೀಲಿ idx)
{
    ಹಿಂತಿರುಗಿ ಪದ_STR(idx) == ಖಾಲಿ || ಪದ_STR(idx) == ಖಾಲಿಗಳು_option;
}
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_TGETENT
/*
 * Get the termcap entries we need with tgetstr(), tgetflag() and tgetnum().
 * "invoke_tgetent()" must have been called before.
 * If "*height" or "*width" are not zero then use the "li" and "col" entries to
 * get their value.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
get_ಪದ_entries(ಪೂರ್ಣಾಂಕ *ಎತ್ತರ, ಪೂರ್ಣಾಂಕ *ಅಗಲ)
{
    ಸ್ಥಿರ ರಚನೆ {
		    ಎಣಿಕೆ Special_ಕೀಲಿ dest; // index in term_strings[]
		    ಅಕ್ಷರ *name;		  // termcap name for string
		  } string_ಹೆಸರುಗಳು[] =
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
			{(ಎಣಿಕೆ Special_ಕೀಲಿ)0, ಖಾಲಿ}
		    };
    ಪೂರ್ಣಾಂಕ		    i;
    ಸ್ಥಿರ char_u   tstrbuf[TBUFSZ];
    char_u	    *tp = tstrbuf;

    /*
     * get output strings
     */
    ಗಾಗಿ (i = 0; string_ಹೆಸರುಗಳು[i].name != ಖಾಲಿ; ++i)
    {
	ಆದರೆ (ಪದ_ಅಕ್ಷರಸರಣಿಗಳು_not_ಗಣ(string_ಹೆಸರುಗಳು[i].dest))
	{
	    ಪದ_STR(string_ಹೆಸರುಗಳು[i].dest) = TGETSTR(string_ಹೆಸರುಗಳು[i].name, &tp);
# ifdef FEAT_EVAL
	    ಗಣ_ಪದ_option_sctx_idx(string_ಹೆಸರುಗಳು[i].name, -1);
# endif
	}
    }

    // tgetflag() returns 1 if the flag is present, 0 if not and
    // possibly -1 if the flag doesn't exist.
    ಆದರೆ ((T_MS == ಖಾಲಿ || T_MS == ಖಾಲಿಗಳು_option) && tgetflag("ms") > 0)
	T_MS = (char_u *)"y";
    ಆದರೆ ((T_XS == ಖಾಲಿ || T_XS == ಖಾಲಿಗಳು_option) && tgetflag("xs") > 0)
	T_XS = (char_u *)"y";
    ಆದರೆ ((T_XN == ಖಾಲಿ || T_XN == ಖಾಲಿಗಳು_option) && tgetflag("xn") > 0)
	T_XN = (char_u *)"y";
    ಆದರೆ ((T_DB == ಖಾಲಿ || T_DB == ಖಾಲಿಗಳು_option) && tgetflag("db") > 0)
	T_DB = (char_u *)"y";
    ಆದರೆ ((T_DA == ಖಾಲಿ || T_DA == ಖಾಲಿಗಳು_option) && tgetflag("da") > 0)
	T_DA = (char_u *)"y";
    ಆದರೆ ((T_UT == ಖಾಲಿ || T_UT == ಖಾಲಿಗಳು_option) && tgetflag("ut") > 0)
	T_UT = (char_u *)"y";
    ಆದರೆ ((T_XON == ಖಾಲಿ || T_XON == ಖಾಲಿಗಳು_option) && tgetflag("xo") > 0)
	T_XON = (char_u *)"y";

    /*
     * get key codes
     */
    ಗಾಗಿ (i = 0; ಕೀಲಿ_ಹೆಸರುಗಳು[i] != ಖಾಲಿ; ++i)
	ಆದರೆ (ಕಂಡುಹಿಡಿ_termcode((char_u *)ಕೀಲಿ_ಹೆಸರುಗಳು[i]) == ಖಾಲಿ)
	{
	    char_u *p = TGETSTR(ಕೀಲಿ_ಹೆಸರುಗಳು[i], &tp);

	    // if cursor-left == backspace, ignore it (televideo 925)
	    ಆದರೆ (p != ಖಾಲಿ
		    && (*p != Ctrl_H
			|| ಕೀಲಿ_ಹೆಸರುಗಳು[i][0] != 'k'
			|| ಕೀಲಿ_ಹೆಸರುಗಳು[i][1] != 'l'))
		ಕೂಡಿಸು_termcode((char_u *)ಕೀಲಿ_ಹೆಸರುಗಳು[i], p, FALSE);
	}

    ಆದರೆ (*ಎತ್ತರ == 0)
	*ಎತ್ತರ = tgetnum("li");
    ಆದರೆ (*ಅಗಲ == 0)
	*ಅಗಲ = tgetnum("co");

    /*
     * Get number of colors (if not done already).
     */
    ಆದರೆ (ಪದ_ಅಕ್ಷರಸರಣಿಗಳು_not_ಗಣ(KS_CCO))
    {
	ಗಣ_color_ಎಣಿಕೆ(tgetnum("Co"));
# ifdef FEAT_EVAL
	ಗಣ_ಪದ_option_sctx_idx("Co", -1);
# endif
    }

# ifndef hpux
    BC = (ಅಕ್ಷರ *)TGETSTR("bc", &tp);
    ಮೇಲೆ_2 = (ಅಕ್ಷರ *)TGETSTR("up", &tp);
    char_u *p = TGETSTR("pc", &tp);
    ಆದರೆ (p != ಖಾಲಿ)
	PC = *p;
# endif
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Report "term" is not found and list the ones we do know about.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
report_ಪದ_error(ಅಕ್ಷರ *error_ಸಂದೇಶ, char_u *ಪದ)
{
    mch_errmsg("\r\n");
    ಆದರೆ (error_ಸಂದೇಶ != ಖಾಲಿ)
    {
	mch_errmsg(error_ಸಂದೇಶ);
	mch_errmsg("\r\n");
    }
    mch_errmsg("'");
    mch_errmsg((ಅಕ್ಷರ *)ಪದ);
    mch_errmsg(_("' not known. Available builtin terminals are:"));
    mch_errmsg("\r\n");

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; ; ++i)
    {
	ಅಕ್ಷರ *name = builtin_terminals[i].bitc_name;
	ಆದರೆ (name == ಖಾಲಿ)  // end marker
	    ನಿಲ್ಲಿಸು;
	// Do not mention the "gui" entry, the user won't need to type it.
	ಆದರೆ (STRCMP(name, "gui") != 0)
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_TGETENT
	    mch_errmsg("    builtin_");
#ಇಲ್ಲದಿದ್ದರೆ
	    mch_errmsg("    ");
#ಆದರೆ_ಅಂತ್ಯ
	    mch_errmsg(name);
	    mch_errmsg("\r\n");
	}
    }
    // Output extra 'cmdheight' line breaks to avoid that the following error
    // message overwrites the last terminal name.
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 1; i < p_ch; ++i)
	mch_errmsg("\r\n");
}

    ಸ್ಥಿರ ಶೂನ್ಯ
report_default_ಪದ(char_u *ಪದ)
{
    mch_errmsg(_("defaulting to '"));
    mch_errmsg((ಅಕ್ಷರ *)ಪದ);
    mch_errmsg("'\r\n");
    ಆದರೆ (emsg_silent == 0 && !ಒಳ_assert_fails)
    {
	screen_ಆರಂಭ();	// don't know where cursor is now
	ಹೊರಹೋಗು_flush();
	ಆದರೆ (!is_not_a_ಪದ())
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
ಹೊಂದಿಕೆ_keyprotocol(char_u *ಪದ)
{
    ಪೂರ್ಣಾಂಕ		len = (ಪೂರ್ಣಾಂಕ)STRLEN(p_kpc) + 1;
    char_u	*buf = alloc(len);
    ಆದರೆ (buf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ KEYPROTOCOL_FAIL;

    keyprot_T	ret = KEYPROTOCOL_FAIL;
    char_u	*p = p_kpc;
    ಆಗ (*p != NUL)
    {
	// Isolate one comma separated item.
	(ಶೂನ್ಯ)copy_option_part(&p, buf, len, ",");
	char_u *colon = vim_strchr(buf, ':');
	ಆದರೆ (colon == ಖಾಲಿ || colon == buf || colon[1] == NUL)
	    ಹೋಗು theend;
	*colon = NUL;

	keyprot_T prot;
	// Note: Keep this in sync with p_kpc_protocol_values.
	ಆದರೆ (STRCMP(colon + 1, "none") == 0)
	    prot = KEYPROTOCOL_NONE;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(colon + 1, "mok2") == 0)
	    prot = KEYPROTOCOL_MOK2;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(colon + 1, "kitty") == 0)
	    prot = KEYPROTOCOL_KITTY;
	ಇಲ್ಲದಿದ್ದರೆ
	    ಹೋಗು theend;

	regmatch_T regmatch;
	CLEAR_ಕ್ಷೇತ್ರ(regmatch);
	regmatch.rm_ic = TRUE;
	regmatch.regprog = vim_regcomp(buf, RE_MAGIC);
	ಆದರೆ (regmatch.regprog == ಖಾಲಿ)
	    ಹೋಗು theend;

	ಪೂರ್ಣಾಂಕ ಹೊಂದಿಕೆ = ಪದ != ಖಾಲಿ && vim_regexec(&regmatch, ಪದ, (colnr_T)0);
	vim_regfree(regmatch.regprog);
	ಆದರೆ (ಹೊಂದಿಕೆ)
	{
	    ret = prot;
	    ಹೋಗು theend;
	}

    }

    // No match found, use "none".
    ret = KEYPROTOCOL_NONE;

theend:
    vim_free(buf);
    ಹಿಂತಿರುಗಿ ret;
}

/*
 * Set terminal options for terminal "term".
 * Return OK if terminal 'term' was found in a termcap, FAIL otherwise.
 *
 * While doing this, until ttest(), some options may be NULL, be careful.
 */
    ಪೂರ್ಣಾಂಕ
ಗಣ_termname(char_u *ಪದ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_TGETENT
    ಪೂರ್ಣಾಂಕ		builtin_ಮೊದಲ = p_tbi;
    ಪೂರ್ಣಾಂಕ		try;
    ಪೂರ್ಣಾಂಕ		termcap_cleared = FALSE;
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		ಅಗಲ = 0, ಎತ್ತರ = 0;
    ಅಕ್ಷರ	*error_ಸಂದೇಶ = ಖಾಲಿ;
    char_u	*bs_p, *del_p;

    // In silect mode (ex -s) we don't use the 'term' option.
    ಆದರೆ (silent_ಬಹುಲಕ)
	ಹಿಂತಿರುಗಿ OK;

    detected_8bit = FALSE;		// reset 8-bit detection

    ಆದರೆ (ಪದ_is_builtin(ಪದ))
    {
	ಪದ += 8;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_TGETENT
	builtin_ಮೊದಲ = 1;
#ಆದರೆ_ಅಂತ್ಯ
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
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_TGETENT
    ಗಾಗಿ (try = builtin_ಮೊದಲ ? 0 : 1; try < 3; ++try)
    {
	/*
	 * Use external termcap
	 */
	ಆದರೆ (try == 1)
	{
	    char_u	    tbuf[TBUFSZ];

	    /*
	     * If the external termcap does not have a matching entry, try the
	     * builtin ones.
	     */
	    ಆದರೆ ((error_ಸಂದೇಶ = invoke_tgetent(tbuf, ಪದ)) == ಖಾಲಿ)
	    {
		ಆದರೆ (!termcap_cleared)
		{
		    clear_termoptions();	// clear old options
		    termcap_cleared = TRUE;
		}

		get_ಪದ_entries(&ಎತ್ತರ, &ಅಗಲ);
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ	    // try == 0 || try == 2
#ಆದರೆ_ಅಂತ್ಯ // HAVE_TGETENT
	/*
	 * Use builtin termcap
	 */
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_TGETENT
	    /*
	     * If builtin termcap was already used, there is no need to search
	     * for the builtin termcap again, quit now.
	     */
	    ಆದರೆ (try == 2 && builtin_ಮೊದಲ && termcap_cleared)
		ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
	    /*
	     * Search for 'term' in builtin_terminals[].
	     */
	    tcap_ನಮೂದು_T *termp = ಕಂಡುಹಿಡಿ_builtin_ಪದ(ಪದ);
	    ಆದರೆ (termp == ಖಾಲಿ)	// did not find it
	    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_TGETENT
		/*
		 * If try == 0, first try the external termcap. If that is not
		 * found we'll get back here with try == 2.
		 * If termcap_cleared is set we used the external termcap,
		 * don't complain about not finding the term in the builtin
		 * termcap.
		 */
		ಆದರೆ (try == 0)			// try external one
		    ಮುಂದುವರಿ;
		ಆದರೆ (termcap_cleared)		// found in external termcap
		    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
		report_ಪದ_error(error_ಸಂದೇಶ, ಪದ);

		// when user typed :set term=xxx, quit here
		ಆದರೆ (starting != NO_SCREEN)
		{
		    screen_ಆರಂಭ();	// don't know where cursor is now
		    wait_return(TRUE);
		    ಹಿಂತಿರುಗಿ FAIL;
		}
		ಪದ = DEFAULT_ಪದ;
		report_default_ಪದ(ಪದ);
		ಗಣ_string_option_ನೇರ((char_u *)"term", -1, ಪದ,
								 OPT_FREE, 0);
		display_errors();
	    }
	    ಹೊರಹೋಗು_flush();
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_TGETENT
	    ಆದರೆ (!termcap_cleared)
	    {
#ಆದರೆ_ಅಂತ್ಯ
		clear_termoptions();	    // clear old options
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_TGETENT
		termcap_cleared = TRUE;
	    }
#ಆದರೆ_ಅಂತ್ಯ
	    parse_builtin_tcap(ಪದ);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
	    ಆದರೆ (ಪದ_is_gui(ಪದ))
	    {
		ಹೊರಹೋಗು_flush();
		gui_init();
		// If starting the GUI failed, don't do any of the other
		// things for this terminal
		ಆದರೆ (!gui.ಒಳ_use)
		    ಹಿಂತಿರುಗಿ FAIL;
# ifdef HAVE_TGETENT
		ನಿಲ್ಲಿಸು;		// don't try using external termcap
# endif
	    }
#ಆದರೆ_ಅಂತ್ಯ // FEAT_GUI
	}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_TGETENT
    }
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    ಆದರೆ (!gui.ಒಳ_use)
#ಆದರೆ_ಅಂತ್ಯ
    {
	// Use the 'keyprotocol' option to adjust the t_TE and t_TI
	// termcap entries if there is an entry matching "term".
	keyprot_T kpc = ಹೊಂದಿಕೆ_keyprotocol(ಪದ);
	apply_keyprotocol(ಪದ, kpc);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMGUICOLORS
	// There is no good way to detect that the terminal supports RGB
	// colors.  Since these termcap entries are non-standard anyway and
	// only used when the user sets 'termguicolors' we might as well add
	// them.  But not when one of them was already set.
	ಆದರೆ (ಪದ_ಅಕ್ಷರಸರಣಿಗಳು_not_ಗಣ(KS_8F)
		&& ಪದ_ಅಕ್ಷರಸರಣಿಗಳು_not_ಗಣ(KS_8B)
		&& ಪದ_ಅಕ್ಷರಸರಣಿಗಳು_not_ಗಣ(KS_8U))
	    apply_builtin_tcap(ಪದ, builtin_rgb, TRUE);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_TGETENT
	ಆದರೆ (ಪದ_ಅಕ್ಷರಸರಣಿಗಳು_not_ಗಣ(KS_CF))
	    apply_builtin_tcap(ಪದ, special_ಪದ, TRUE);
	ಆದರೆ (ಪದ_ಅಕ್ಷರಸರಣಿಗಳು_not_ಗಣ(KS_BSU) && ಪದ_ಅಕ್ಷರಸರಣಿಗಳು_not_ಗಣ(KS_ESU))
	    apply_builtin_tcap(ಪದ, builtin_sync_output, TRUE);
#ಆದರೆ_ಅಂತ್ಯ
    }

/*
 * special: There is no info in the termcap about whether the cursor
 * positioning is relative to the start of the screen or to the start of the
 * scrolling region.  We just guess here. Only msdos pcterm is known to do it
 * relative.
 */
    ಆದರೆ (STRCMP(ಪದ, "pcterm") == 0)
	T_CCS = (char_u *)"yes";
    ಇಲ್ಲದಿದ್ದರೆ
	T_CCS = ಖಾಲಿಗಳು_option;

    // Special case: "kitty" may not have a "RV" entry in terminfo, but we need
    // to request the version for several other things to work.
    ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು((ಅಕ್ಷರ *)ಪದ, "kitty") != ಖಾಲಿ
					   && (T_CRV == ಖಾಲಿ || *T_CRV == NUL))
	T_CRV = (char_u *)"\033[>c";

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
/*
 * Any "stty" settings override the default for t_kb from the termcap.
 * This is in os_unix.c, because it depends a lot on the version of unix that
 * is being used.
 * Don't do this when the GUI is active, it uses "t_kb" and "t_kD" directly.
 */
# ifdef FEAT_GUI
    ಆದರೆ (!gui.ಒಳ_use)
# endif
	get_stty();
#ಆದರೆ_ಅಂತ್ಯ

/*
 * If the termcap has no entry for 'bs' and/or 'del' and the ioctl() also
 * didn't work, use the default CTRL-H
 * The default for t_kD is DEL, unless t_kb is DEL.
 * The vim_strsave'd strings are probably lost forever, well it's only two
 * bytes.  Don't do this when the GUI is active, it uses "t_kb" and "t_kD"
 * directly.
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    ಆದರೆ (!gui.ಒಳ_use)
#ಆದರೆ_ಅಂತ್ಯ
    {
	bs_p = ಕಂಡುಹಿಡಿ_termcode((char_u *)"kb");
	del_p = ಕಂಡುಹಿಡಿ_termcode((char_u *)"kD");
	ಆದರೆ (bs_p == ಖಾಲಿ || *bs_p == NUL)
	    ಕೂಡಿಸು_termcode((char_u *)"kb", (bs_p = (char_u *)CTRL_H_STR), FALSE);
	ಆದರೆ ((del_p == ಖಾಲಿ || *del_p == NUL) &&
					    (bs_p == ಖಾಲಿ || *bs_p != DEL))
	    ಕೂಡಿಸು_termcode((char_u *)"kD", (char_u *)DEL_STR, FALSE);
    }

#ಆದರೆ defined(UNIX) || defined(VMS)
    ಪದ_is_xterm = vim_is_xterm(ಪದ);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMRESPONSE
    // Reset terminal properties that are set based on the termresponse, which
    // will be sent out soon.
    init_ಪದ_props(FALSE);
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(UNIX) || defined(VMS)
    // If the first number in t_XM is 1006 then the terminal will support SGR
    // mouse reporting.
    ಪೂರ್ಣಾಂಕ did_ಗಣ_ttym = FALSE;
    ಆದರೆ (T_CXM != ಖಾಲಿ && *T_CXM != NUL && !option_was_ಗಣ((char_u *)"ttym"))
    {
	char_u *p = T_CXM;

	ಆಗ (*p != NUL && !VIM_ISDIGIT(*p))
	    ++p;
	ಆದರೆ (getdigits(&p) == 1006)
	{
	    did_ಗಣ_ttym = TRUE;
	    ಗಣ_option_ಮೌಲ್ಯ_give_err((char_u *)"ttym", 0L, (char_u *)"sgr", 0);
	}
    }

    /*
     * For Unix, set the 'ttymouse' option to the type of mouse to be used.
     * The termcode for the mouse is added as a side effect in option.c.
     */
    {
	char_u	*p = (char_u *)"";

# ifdef FEAT_MOUSE_XTERM
	ಆದರೆ (use_xterm_like_mouse(ಪದ))
	{
	    ಆದರೆ (use_xterm_mouse())
		p = ಖಾಲಿ;	// keep existing value, might be "xterm2"
	    ಇಲ್ಲದಿದ್ದರೆ
		p = (char_u *)"xterm";
	}
# endif
	ಆದರೆ (p != ಖಾಲಿ && !did_ಗಣ_ttym)
	{
	    ಗಣ_option_ಮೌಲ್ಯ_give_err((char_u *)"ttym", 0L, p, 0);
	    // Reset the WAS_SET flag, 'ttymouse' can be set to "sgr" or
	    // "xterm2" in check_termcode().
	    reset_option_was_ಗಣ((char_u *)"ttym");
	}
	ಆದರೆ (p == ಖಾಲಿ
# ifdef FEAT_GUI
		|| gui.ಒಳ_use
# endif
		)
	    check_mouse_termcode();	// set mouse termcode anyway
    }
#ಇಲ್ಲದಿದ್ದರೆ
    ಗಣ_mouse_termcode(KS_MOUSE, (char_u *)"\233M");
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MOUSE_XTERM
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
	ಕೂಡಿಸು_termcode(name, (char_u *)"\033[I", FALSE);

	// handle focus out event
	name[1] = KE_FOCUSLOST;
	ಕೂಡಿಸು_termcode(name, (char_u *)"\033[O", FALSE);

	need_gather = TRUE;
    }
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ (defined(UNIX) || defined(VMS))
    // First time after setting 'term' a focus event is always reported.
    focus_state = MAYBE;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ಪದ_CONSOLE
    // DEFAULT_TERM indicates that it is the machine console.
    ಆದರೆ (STRCMP(ಪದ, DEFAULT_ಪದ) != 0)
	ಪದ_console = FALSE;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಪದ_console = TRUE;
# ifdef AMIGA
	win_resize_on();	// enable window resizing reports
# endif
    }
#ಆದರೆ_ಅಂತ್ಯ

    ttest(TRUE);	// make sure we have a valid set of terminal codes

    ತುಂಬಿದ_screen = TRUE;		// we can use termcap codes from now on
    ಗಣ_ಪದ_defaults();	// use current values as defaults
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMRESPONSE
    LOG_TR1("setting crv_status to STATUS_GET");
    crv_ಸ್ಥಿತಿ.tr_ಪ್ರಗತಿ = ಸ್ಥಿತಿ_GET;	// Get terminal version later
    write_t_8u_state = FALSE;
#ಆದರೆ_ಅಂತ್ಯ

    /*
     * Initialize the terminal with the appropriate termcap codes.
     * Set the mouse and window title if possible.
     * Don't do this when starting, need to parse the .vimrc first, because it
     * may redefine t_TI etc.
     */
    ಆದರೆ (starting != NO_SCREEN)
    {
	starttermcap();		// may change terminal mode
	setmouse();		// may start using the mouse
	maketitle();		// may display window title
    }

	// display initial screen after ttest() checking. jw.
    ಆದರೆ (ಅಗಲ <= 0 || ಎತ್ತರ <= 0)
    {
	// termcap failed to report size
	// set defaults, in case ui_get_shellsize() also fails
	ಅಗಲ = 80;
#ಆದರೆ defined(MSWIN)
	ಎತ್ತರ = 25;	    // console is often 25 lines
#ಇಲ್ಲದಿದ್ದರೆ
	ಎತ್ತರ = 24;	    // most terminals are 24 lines
#ಆದರೆ_ಅಂತ್ಯ
    }
    ಗಣ_shellsize(ಅಗಲ, ಎತ್ತರ, FALSE);	// may change Rows
    ಆದರೆ (starting != NO_SCREEN)
    {
	ಆದರೆ (scroll_region)
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
		ಆದರೆ (curbuf->b_ml.ml_mfp != ಖಾಲಿ)
		{
		    aucmd_prepbuf(&aco, buf);
		    ಆದರೆ (curbuf == buf)
		    {
			apply_autocmds(EVENT_TERMCHANGED, ಖಾಲಿ, ಖಾಲಿ, FALSE,
								      curbuf);
			// restore curwin/curbuf and a few other things
			aucmd_restbuf(&aco);
		    }
		}
	    }
	}
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMRESPONSE
    may_req_termresponse();
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_IMAGE
    (ಶೂನ್ಯ)update_image_backend();
#ಆದರೆ_ಅಂತ್ಯ

    ಹಿಂತಿರುಗಿ OK;
}

#ಆದರೆ defined(EXITFREE)

# ifdef HAVE_DEL_CURTERM
#  include <term.h>	    // declares cur_term
# endif

/*
 * If supported, delete "cur_term", which caches terminal related entries.
 * Avoids that valgrind reports possibly lost memory.
 */
    ಶೂನ್ಯ
free_cur_ಪದ(ಶೂನ್ಯ)
{
# ifdef HAVE_DEL_CURTERM
    ಆದರೆ (cur_ಪದ)
	del_curterm(cur_ಪದ);
# endif
}

#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_TGETENT
/*
 * Call tgetent()
 * Return error message if it fails, NULL if it's OK.
 */
    ಸ್ಥಿರ ಅಕ್ಷರ *
invoke_tgetent(char_u *tbuf, char_u *ಪದ)
{
    ಪೂರ್ಣಾಂಕ	    i;

    // Note: Valgrind may report a leak here, because the library keeps one
    // buffer around that we can't ever free.
    i = TGETENT(tbuf, ಪದ);
    ಆದರೆ (i < 0		    // -1 is always an error
# ifdef TGETENT_ZERO_ERR
	    || i == 0	    // sometimes zero is also an error
# endif
       )
    {
	// On FreeBSD tputs() gets a SEGV after a tgetent() which fails.  Call
	// tgetent() with the always existing "dumb" entry to avoid a crash or
	// hang.
	(ಶೂನ್ಯ)TGETENT(tbuf, "dumb");

	ಆದರೆ (i < 0)
# ifdef TGETENT_ZERO_ERR
	    ಹಿಂತಿರುಗಿ _(e_cannot_open_termcap_file);
	ಆದರೆ (i == 0)
# endif
# ifdef TERMINFO
	    ಹಿಂತಿರುಗಿ _(e_terminal_ನಮೂದು_not_ಸಿಕ್ಕಿತು_ಒಳ_terminfo);
# ಇಲ್ಲದಿದ್ದರೆ
	    ಹಿಂತಿರುಗಿ _(e_terminal_ನಮೂದು_not_ಸಿಕ್ಕಿತು_ಒಳ_termcap);
# endif
    }
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Some versions of tgetstr() have been reported to return -1 instead of NULL.
 * Fix that here.
 */
    ಸ್ಥಿರ char_u *
vim_tgetstr(ಅಕ್ಷರ *s, char_u **pp)
{
    ಅಕ್ಷರ	*p;

    p = tgetstr(s, (ಅಕ್ಷರ **)pp);
    ಆದರೆ (p == (ಅಕ್ಷರ *)-1)
	p = ಖಾಲಿ;
    ಹಿಂತಿರುಗಿ (char_u *)p;
}
#ಆದರೆ_ಅಂತ್ಯ // HAVE_TGETENT

#ಆದರೆ defined(HAVE_TGETENT) && (defined(UNIX) || defined(VMS) || defined(MACOS_X))
/*
 * Get Columns and Rows from the termcap. Used after a window signal if the
 * ioctl() fails. It doesn't make sense to call tgetent each time if the "co"
 * and "li" entries never change. But on some systems this works.
 * Errors while getting the entries are ignored.
 */
    ಶೂನ್ಯ
getlinecol(
    ದೀರ್ಘ	*cp,	// pointer to columns
    ದೀರ್ಘ	*rp)	// pointer to rows
{
    char_u	tbuf[TBUFSZ];

    ಆದರೆ (T_NAME == ಖಾಲಿ || *T_NAME == NUL
	    || invoke_tgetent(tbuf, T_NAME) != ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (*cp == 0)
	*cp = tgetnum("co");
    ಆದರೆ (*rp == 0)
	*rp = tgetnum("li");
}
#ಆದರೆ_ಅಂತ್ಯ // defined(HAVE_TGETENT) && defined(UNIX)

/*
 * Get a string entry from the termcap and add it to the list of termcodes.
 * Used for <t_xx> special keys.
 * Give an error message for failure when not sourcing.
 * If force given, replace an existing entry.
 * Return FAIL if the entry was not found, OK if the entry was added.
 */
    ಪೂರ್ಣಾಂಕ
ಕೂಡಿಸು_termcap_ನಮೂದು(char_u *name, ಪೂರ್ಣಾಂಕ force)
{
    char_u  *ಪದ;
    ಪೂರ್ಣಾಂಕ	    ಕೀಲಿ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_TGETENT
    char_u  *string;
    ಪೂರ್ಣಾಂಕ	    i;
    ಪೂರ್ಣಾಂಕ	    builtin_ಮೊದಲ;
    char_u  tbuf[TBUFSZ];
    char_u  tstrbuf[TBUFSZ];
    char_u  *tp = tstrbuf;
    ಅಕ್ಷರ    *error_ಸಂದೇಶ = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ

/*
 * If the GUI is running or will start in a moment, we only support the keys
 * that the GUI can produce.
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    ಆದರೆ (gui.ಒಳ_use || gui.starting)
	ಹಿಂತಿರುಗಿ gui_mch_haskey(name);
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (!force && ಕಂಡುಹಿಡಿ_termcode(name) != ಖಾಲಿ)	    // it's already there
	ಹಿಂತಿರುಗಿ OK;

    ಪದ = T_NAME;
    ಆದರೆ (ಪದ == ಖಾಲಿ || *ಪದ == NUL)	    // 'term' not defined yet
	ಹಿಂತಿರುಗಿ FAIL;

    ಆದರೆ (ಪದ_is_builtin(ಪದ))		    // name starts with "builtin_"
    {
	ಪದ += 8;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_TGETENT
	builtin_ಮೊದಲ = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_TGETENT
    ಇಲ್ಲದಿದ್ದರೆ
	builtin_ಮೊದಲ = p_tbi;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_TGETENT
/*
 * We can get the entry from the builtin termcap and from the external one.
 * If 'ttybuiltin' is on or the terminal name starts with "builtin_", try
 * builtin termcap first.
 * If 'ttybuiltin' is off, try external termcap first.
 */
    ಗಾಗಿ (i = 0; i < 2; ++i)
    {
	ಆದರೆ ((!builtin_ಮೊದಲ) == i)
#ಆದರೆ_ಅಂತ್ಯ
	/*
	 * Search in builtin termcaps
	 */
	{
	    tcap_ನಮೂದು_T *termp = ಕಂಡುಹಿಡಿ_builtin_ಪದ(ಪದ);
	    ಆದರೆ (termp != ಖಾಲಿ)	// found it
	    {
		ಕೀಲಿ = TERMCAP2_ಕೀಲಿ(name[0], name[1]);
		++termp;
		ಆಗ (termp->bt_ನಮೂದು != (ಪೂರ್ಣಾಂಕ)KS_NAME)
		{
		    ಆದರೆ ((ಪೂರ್ಣಾಂಕ)termp->bt_ನಮೂದು == ಕೀಲಿ)
		    {
			ಕೂಡಿಸು_termcode(name, (char_u *)termp->bt_string,
							  ಪದ_is_8bit(ಪದ));
			ಹಿಂತಿರುಗಿ OK;
		    }
		    ++termp;
		}
	    }
	}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_TGETENT
	ಇಲ್ಲದಿದ್ದರೆ
	/*
	 * Search in external termcap
	 */
	{
	    error_ಸಂದೇಶ = invoke_tgetent(tbuf, ಪದ);
	    ಆದರೆ (error_ಸಂದೇಶ == ಖಾಲಿ)
	    {
		string = TGETSTR((ಅಕ್ಷರ *)name, &tp);
		ಆದರೆ (string != ಖಾಲಿ && *string != NUL)
		{
		    ಕೂಡಿಸು_termcode(name, string, FALSE);
		    ಹಿಂತಿರುಗಿ OK;
		}
	    }
	}
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (SOURCING_NAME == ಖಾಲಿ)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_TGETENT
	ಆದರೆ (error_ಸಂದೇಶ != ಖಾಲಿ)
	    emsg(error_ಸಂದೇಶ);
	ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	    semsg(_(e_no_str_ನಮೂದು_ಒಳ_termcap), name);
    }
    ಹಿಂತಿರುಗಿ FAIL;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಪದ_is_builtin(char_u *name)
{
    ಹಿಂತಿರುಗಿ (STRNCMP(name, "builtin_", (size_t)8) == 0);
}

/*
 * Return TRUE if terminal "name" uses CSI instead of <Esc>[.
 * Assume that the terminal is using 8-bit controls when the name contains
 * "8bit", like in "xterm-8bit".
 */
    ಪೂರ್ಣಾಂಕ
ಪದ_is_8bit(char_u *name)
{
    ಹಿಂತಿರುಗಿ (detected_8bit || ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು((ಅಕ್ಷರ *)name, "8bit") != ಖಾಲಿ);
}

/*
 * Translate terminal control chars from 7-bit to 8-bit:
 * <Esc>[ -> CSI  <M_C_[>
 * <Esc>] -> OSC  <M-C-]>
 * <Esc>O -> <M-C-O>
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಪದ_7to8bit(char_u *p)
{
    ಆದರೆ (*p != ESC)
	ಹಿಂತಿರುಗಿ 0;

    ಆದರೆ (p[1] == '[')
	ಹಿಂತಿರುಗಿ CSI;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p[1] == ']')
	ಹಿಂತಿರುಗಿ OSC;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p[1] == 'O')
	ಹಿಂತಿರುಗಿ 0x8f;
    ಹಿಂತಿರುಗಿ 0;
}

#ಆದರೆ defined(FEAT_GUI)
    ಪೂರ್ಣಾಂಕ
ಪದ_is_gui(char_u *name)
{
    ಹಿಂತಿರುಗಿ (STRCMP(name, "builtin_gui") == 0 || STRCMP(name, "gui") == 0);
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ !defined(HAVE_TGETENT) || defined(AMIGA)

    char_u *
tltoa(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ i)
{
    ಸ್ಥಿರ char_u buf[16];
    char_u	*p;

    p = buf + 15;
    *p = '\0';
    ಮಾಡು
    {
	--p;
	*p = (char_u) (i % 10 + '0');
	i /= 10;
    }
    ಆಗ (i > 0 && p > buf);
    ಹಿಂತಿರುಗಿ p;
}
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ HAVE_TGETENT

/*
 * minimal tgoto() implementation.
 * no padding and we only parse for %i %d and %+char
 */
    ಸ್ಥಿರ ಅಕ್ಷರ *
tgoto(ಅಕ್ಷರ *cm, ಪೂರ್ಣಾಂಕ x, ಪೂರ್ಣಾಂಕ y)
{
    ಸ್ಥಿರ ಅಕ್ಷರ buf[30];
    ಅಕ್ಷರ *p, *s, *e;

    ಆದರೆ (!cm)
	ಹಿಂತಿರುಗಿ "OOPS";
    e = buf + 29;
    ಗಾಗಿ (s = buf; s < e && *cm; cm++)
    {
	ಆದರೆ (*cm != '%')
	{
	    *s++ = *cm;
	    ಮುಂದುವರಿ;
	}
	ಆಯ್ಕೆ (*++cm)
	{
	ಸ್ಥಿತಿ 'd':
	    p = (ಅಕ್ಷರ *)tltoa((ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)y);
	    y = x;
	    ಆಗ (*p)
		*s++ = *p++;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 'i':
	    x++;
	    y++;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ '+':
	    *s++ = (ಅಕ್ಷರ)(*++cm + y);
	    y = x;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ '%':
	    *s++ = *cm;
	    ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:
	    ಹಿಂತಿರುಗಿ "OOPS";
	}
    }
    *s = '\0';
    ಹಿಂತಿರುಗಿ buf;
}

#ಆದರೆ_ಅಂತ್ಯ // HAVE_TGETENT

/*
 * Set the terminal name and initialize the terminal options.
 * If "name" is NULL or empty, get the terminal name from the environment.
 * If that fails, use the default terminal name.
 */
    ಶೂನ್ಯ
termcapinit(char_u *name)
{
    char_u	*ಪದ = name;

    ಆದರೆ (ಪದ != ಖಾಲಿ && *ಪದ == NUL)
	ಪದ = ಖಾಲಿ;	    // empty name is equal to no name

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ MSWIN
    ಆದರೆ (ಪದ == ಖಾಲಿ)
	ಪದ = mch_getenv((char_u *)"TERM");
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (ಪದ == ಖಾಲಿ || *ಪದ == NUL)
	ಪದ = DEFAULT_ಪದ;
    ಗಣ_string_option_ನೇರ((char_u *)"term", -1, ಪದ, OPT_FREE, 0);

    // Set the default terminal name.
    ಗಣ_string_default("term", ಪದ);
    ಗಣ_string_default("ttytype", ಪದ);

    // Avoid using "term" here, because the next mch_getenv() may overwrite it.
    ಗಣ_termname(T_NAME != ಖಾಲಿ ? T_NAME : ಪದ);
}

/*
 * The number of calls to ui_write is reduced by using "out_buf".
 */
#ವ್ಯಾಖ್ಯಾನಿಸು ಹೊರಹೋಗು_ಗಾತ್ರ	8191

// add one to allow mch_write() in os_win32.c to append a NUL
ಸ್ಥಿರ char_u		ಹೊರಹೋಗು_buf[ಹೊರಹೋಗು_ಗಾತ್ರ + 1];

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ		ಹೊರಹೋಗು_pos = 0;	// number of chars in out_buf

// Since the maximum number of SGR parameters shown as a normal value range is
// 16, the escape sequence length can be 4 * 16 + lead + tail.
#ವ್ಯಾಖ್ಯಾನಿಸು MAX_ESC_SEQ_LEN	80

/*
 * out_flush(): flush the output buffer
 */
    ಶೂನ್ಯ
ಹೊರಹೋಗು_flush(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ	    len;

    ಆದರೆ (ಹೊರಹೋಗು_pos == 0)
	ಹಿಂತಿರುಗಿ;

    // set out_pos to 0 before ui_write, to avoid recursiveness
    len = ಹೊರಹೋಗು_pos;
    ಹೊರಹೋಗು_pos = 0;
    ui_write(ಹೊರಹೋಗು_buf, len, FALSE);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಆದರೆ (ch_log_output != FALSE)
    {
	ಹೊರಹೋಗು_buf[len] = NUL;
	ch_log(ಖಾಲಿ, "raw %s output: \"%s\"",
# ifdef FEAT_GUI
		(gui.ಒಳ_use && !gui.dying && !gui.starting) ? "GUI" :
# endif
		"terminal",
		ಹೊರಹೋಗು_buf);
	ಆದರೆ (ch_log_output == TRUE)
	    ch_log_output = FALSE;  // only log once
    }
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * out_flush_cursor(): flush the output buffer and redraw the cursor.
 * Does not flush recursively in the GUI to avoid slow drawing.
 */
    ಶೂನ್ಯ
ಹೊರಹೋಗು_flush_cursor(
    ಪೂರ್ಣಾಂಕ	    force ಬಳಸದ,   // when TRUE, update cursor even when not moved
    ಪೂರ್ಣಾಂಕ	    clear_selection ಬಳಸದ) // clear selection under cursor
{
    mch_disable_flush();
    ಹೊರಹೋಗು_flush();
    mch_enable_flush();
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    ಆದರೆ (gui.ಒಳ_use)
    {
	gui_update_cursor(force, clear_selection);
	gui_may_flush();
    }
#ಆದರೆ_ಅಂತ್ಯ
}


/*
 * Sometimes a byte out of a multi-byte character is written with out_char().
 * To avoid flushing half of the character, call this function first.
 */
    ಶೂನ್ಯ
ಹೊರಹೋಗು_flush_check(ಶೂನ್ಯ)
{
    ಆದರೆ (enc_dbcs != 0 && ಹೊರಹೋಗು_pos >= ಹೊರಹೋಗು_ಗಾತ್ರ - MB_MAXBYTES)
	ಹೊರಹೋಗು_flush();
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
/*
 * out_trash(): Throw away the contents of the output buffer
 */
    ಶೂನ್ಯ
ಹೊರಹೋಗು_trash(ಶೂನ್ಯ)
{
    ಹೊರಹೋಗು_pos = 0;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * out_char(c): put a byte into the output buffer.
 *		Flush it if it becomes full.
 * This should not be used for outputting text on the screen (use functions
 * like msg_puts() and screen_putchar() for that).
 */
    ಶೂನ್ಯ
ಹೊರಹೋಗು_char(ಚಿಹ್ನೆರಹಿತ c)
{
#ಆದರೆ defined(UNIX) || defined(VMS) || defined(AMIGA) || defined(MACOS_X)
    ಆದರೆ (c == '\n')	// turn LF into CR-LF (CRMOD doesn't seem to do this)
	ಹೊರಹೋಗು_char('\r');
#ಆದರೆ_ಅಂತ್ಯ

    ಹೊರಹೋಗು_buf[ಹೊರಹೋಗು_pos++] = c;

    // For testing we flush each time.
    ಆದರೆ (ಹೊರಹೋಗು_pos >= ಹೊರಹೋಗು_ಗಾತ್ರ || p_wd)
	ಹೊರಹೋಗು_flush();
}

/*
 * Output "c" like out_char(), but don't flush when p_wd is set.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಹೊರಹೋಗು_char_nf(ಪೂರ್ಣಾಂಕ c)
{
    ಹೊರಹೋಗು_buf[ಹೊರಹೋಗು_pos++] = (ಚಿಹ್ನೆರಹಿತ)c;

    ಆದರೆ (ಹೊರಹೋಗು_pos >= ಹೊರಹೋಗು_ಗಾತ್ರ)
	ಹೊರಹೋಗು_flush();
    ಹಿಂತಿರುಗಿ (ಚಿಹ್ನೆರಹಿತ)c;
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
ಹೊರಹೋಗು_str_nf(char_u *s)
{
    // avoid terminal strings being split up
    ಆದರೆ (ಹೊರಹೋಗು_pos > ಹೊರಹೋಗು_ಗಾತ್ರ - MAX_ESC_SEQ_LEN)
	ಹೊರಹೋಗು_flush();

    ಗಾಗಿ (char_u *p = s; *p != NUL; ++p)
	ಹೊರಹೋಗು_char_nf(*p);

    // For testing we write one string at a time.
    ಆದರೆ (p_wd)
	ಹೊರಹೋಗು_flush();
}

/*
 * A conditional-flushing out_str, mainly for visualbell.
 * Handles a delay internally, because termlib may not respect the delay or do
 * it at the wrong time.
 * Note: Only for terminal strings.
 */
    ಶೂನ್ಯ
ಹೊರಹೋಗು_str_cf(char_u *s)
{
    ಆದರೆ (s == ಖಾಲಿ || *s == NUL)
	ಹಿಂತಿರುಗಿ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_TGETENT
    char_u *p;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    // Don't use tputs() when GUI is used, ncurses crashes.
    ಆದರೆ (gui.ಒಳ_use)
    {
	ಹೊರಹೋಗು_str_nf(s);
	ಹಿಂತಿರುಗಿ;
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (ಹೊರಹೋಗು_pos > ಹೊರಹೋಗು_ಗಾತ್ರ - MAX_ESC_SEQ_LEN)
	ಹೊರಹೋಗು_flush();
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_TGETENT
    ಗಾಗಿ (p = s; *s; ++s)
    {
	// flush just before delay command
	ಆದರೆ (*s == '$' && *(s + 1) == '<')
	{
	    char_u save_c = *s;
	    ಪೂರ್ಣಾಂಕ duration = atoi((ಅಕ್ಷರ *)s + 2);

	    *s = NUL;
	    tputs((ಅಕ್ಷರ *)p, 1, TPUTSFUNCAST ಹೊರಹೋಗು_char_nf);
	    *s = save_c;
	    ಹೊರಹೋಗು_flush();
# ifdef ELAPSED_FUNC
	    // Only sleep here if we can limit this happening in
	    // vim_beep().
	    p = vim_strchr(s, '>');
	    ಆದರೆ (p == ಖಾಲಿ || duration <= 0)
	    {
		// can't parse the time, don't sleep here
		p = s;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		++p;
		do_sleep(duration, FALSE);
	    }
# ಇಲ್ಲದಿದ್ದರೆ
	    // Rely on the terminal library to sleep.
	    p = s;
# endif
	    ನಿಲ್ಲಿಸು;
	}
    }
    tputs((ಅಕ್ಷರ *)p, 1, TPUTSFUNCAST ಹೊರಹೋಗು_char_nf);
#ಇಲ್ಲದಿದ್ದರೆ
    ಆಗ (*s)
	ಹೊರಹೋಗು_char_nf(*s++);
#ಆದರೆ_ಅಂತ್ಯ

    // For testing we write one string at a time.
    ಆದರೆ (p_wd)
	ಹೊರಹೋಗು_flush();
}

/*
 * out_str(s): Put a character string a byte at a time into the output buffer.
 * If HAVE_TGETENT is defined use tputs(), the termcap parser. (jw)
 * This should only be used for writing terminal codes, not for outputting
 * normal text (use functions like msg_puts() and screen_putchar() for that).
 */
    ಶೂನ್ಯ
ಹೊರಹೋಗು_str(char_u *s)
{
    ಆದರೆ (s == ಖಾಲಿ || *s == NUL)
	ಹಿಂತಿರುಗಿ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    // Don't use tputs() when GUI is used, ncurses crashes.
    ಆದರೆ (gui.ಒಳ_use)
    {
	ಹೊರಹೋಗು_str_nf(s);
	ಹಿಂತಿರುಗಿ;
    }
#ಆದರೆ_ಅಂತ್ಯ
    // avoid terminal strings being split up
    ಆದರೆ (ಹೊರಹೋಗು_pos > ಹೊರಹೋಗು_ಗಾತ್ರ - MAX_ESC_SEQ_LEN)
	ಹೊರಹೋಗು_flush();
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_TGETENT
    tputs((ಅಕ್ಷರ *)s, 1, TPUTSFUNCAST ಹೊರಹೋಗು_char_nf);
#ಇಲ್ಲದಿದ್ದರೆ
    ಆಗ (*s)
	ಹೊರಹೋಗು_char_nf(*s++);
#ಆದರೆ_ಅಂತ್ಯ

    // For testing we write one string at a time.
    ಆದರೆ (p_wd)
	ಹೊರಹೋಗು_flush();
}

/*
 * cursor positioning using termcap parser. (jw)
 */
    ಶೂನ್ಯ
ಪದ_windgoto(ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು)
{
    ಹೊರಹೋಗು_STR(tgoto((ಅಕ್ಷರ *)T_CM, ಲಂಬಸಾಲು, ಅಡ್ಡಸಾಲು));
}

    ಶೂನ್ಯ
ಪದ_cursor_ಬಲ(ಪೂರ್ಣಾಂಕ i)
{
    ಹೊರಹೋಗು_STR(tgoto((ಅಕ್ಷರ *)T_CRI, 0, i));
}

    ಶೂನ್ಯ
ಪದ_append_ಸಾಲುಗಳು(ಪೂರ್ಣಾಂಕ ಸಾಲು_ಎಣಿಕೆ)
{
    ಹೊರಹೋಗು_STR(tgoto((ಅಕ್ಷರ *)T_CAL, 0, ಸಾಲು_ಎಣಿಕೆ));
}

    ಶೂನ್ಯ
ಪದ_delete_ಸಾಲುಗಳು(ಪೂರ್ಣಾಂಕ ಸಾಲು_ಎಣಿಕೆ)
{
    ಹೊರಹೋಗು_STR(tgoto((ಅಕ್ಷರ *)T_CDL, 0, ಸಾಲು_ಎಣಿಕೆ));
}

#ಆದರೆ defined(UNIX) || defined(VMS)
    ಶೂನ್ಯ
ಪದ_enable_mouse(ಪೂರ್ಣಾಂಕ enable)
{
    ಪೂರ್ಣಾಂಕ on = enable ? 1 : 0;
    ಹೊರಹೋಗು_STR(tgoto((ಅಕ್ಷರ *)T_CXM, 0, on));
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(HAVE_TGETENT)
    ಶೂನ್ಯ
ಪದ_ಗಣ_winpos(ಪೂರ್ಣಾಂಕ x, ಪೂರ್ಣಾಂಕ y)
{
    // Can't handle a negative value here
    ಆದರೆ (x < 0)
	x = 0;
    ಆದರೆ (y < 0)
	y = 0;
    ಹೊರಹೋಗು_STR(tgoto((ಅಕ್ಷರ *)T_CWP, y, x));
}

# ಆದರೆ defined(FEAT_TERMRESPONSE)
/*
 * Return TRUE if we can request the terminal for a response.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
can_get_termresponse(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ cur_tmode == TMODE_RAW
	    && termcap_ಸಕ್ರಿಯ
#  ifdef UNIX
	    && (is_not_a_ಪದ() || (isatty(1) && isatty(read_cmd_fd)))
#  endif
	    && p_ek;
}

/*
 * Set "status" to STATUS_SENT.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
termrequest_ಕಳುಹಿಸಿದ(termrequest_T *ಸ್ಥಿತಿ_2)
{
    ಸ್ಥಿತಿ_2->tr_ಪ್ರಗತಿ = ಸ್ಥಿತಿ_ಕಳುಹಿಸಿದ;
    ಸ್ಥಿತಿ_2->tr_ಆರಂಭ = ಸಮಯ(ಖಾಲಿ);
}

/*
 * Return TRUE if any of the requests are in STATUS_SENT.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
termrequest_any_pending(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ	    i;
    time_t  now = ಸಮಯ(ಖಾಲಿ);

    ಗಾಗಿ (i = 0; all_termrequests[i] != ಖಾಲಿ; ++i)
    {
	ಆದರೆ (all_termrequests[i]->tr_ಪ್ರಗತಿ == ಸ್ಥಿತಿ_ಕಳುಹಿಸಿದ)
	{
	    ಆದರೆ (all_termrequests[i]->tr_ಆರಂಭ > 0 && now > 0
				    && all_termrequests[i]->tr_ಆರಂಭ + 2 < now)
		// Sent the request more than 2 seconds ago and didn't get a
		// response, assume it failed.
		all_termrequests[i]->tr_ಪ್ರಗತಿ = ಸ್ಥಿತಿ_FAIL;
	    ಇಲ್ಲದಿದ್ದರೆ
		ಹಿಂತಿರುಗಿ TRUE;
	}
    }
    ಹಿಂತಿರುಗಿ FALSE;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ winpos_x = -1;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ winpos_y = -1;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ did_request_winpos = 0;

#  ಆದರೆ defined(FEAT_EVAL) || defined(FEAT_TERMINAL)
/*
 * Try getting the Vim window position from the terminal.
 * Returns OK or FAIL.
 */
    ಪೂರ್ಣಾಂಕ
ಪದ_get_winpos(ಪೂರ್ಣಾಂಕ *x, ಪೂರ್ಣಾಂಕ *y, varnumber_T timeout)
{
    ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2 = 0;
    ಪೂರ್ಣಾಂಕ prev_winpos_x = winpos_x;
    ಪೂರ್ಣಾಂಕ prev_winpos_y = winpos_y;

    ಆದರೆ (*T_CGP == NUL || !can_get_termresponse())
	ಹಿಂತಿರುಗಿ FAIL;
    winpos_x = -1;
    winpos_y = -1;
    ++did_request_winpos;
    termrequest_ಕಳುಹಿಸಿದ(&winpos_ಸ್ಥಿತಿ);
    ಹೊರಹೋಗು_STR(T_CGP);
    ಹೊರಹೋಗು_flush();

    // Try reading the result for "timeout" msec.
    ಆಗ (ಎಣಿಕೆ_2++ <= timeout / 10 && !got_int)
    {
	(ಶೂನ್ಯ)vpeekc_nomap();
	ಆದರೆ (winpos_x >= 0 && winpos_y >= 0)
	{
	    *x = winpos_x;
	    *y = winpos_y;
	    ಹಿಂತಿರುಗಿ OK;
	}
	ui_delay(10L, FALSE);
    }
    // Do not reset "did_request_winpos", if we timed out the response might
    // still come later and we must consume it.

    winpos_x = prev_winpos_x;
    winpos_y = prev_winpos_y;
    ಆದರೆ (timeout < 10 && prev_winpos_y >= 0 && prev_winpos_x >= 0)
    {
	// Polling: return previous values if we have them.
	*x = winpos_x;
	*y = winpos_y;
	ಹಿಂತಿರುಗಿ OK;
    }

    ಹಿಂತಿರುಗಿ FALSE;
}
#  endif
# endif

    ಶೂನ್ಯ
ಪದ_ಗಣ_winsize(ಪೂರ್ಣಾಂಕ ಎತ್ತರ, ಪೂರ್ಣಾಂಕ ಅಗಲ)
{
    ಹೊರಹೋಗು_STR(tgoto((ಅಕ್ಷರ *)T_CWS, ಅಗಲ, ಎತ್ತರ));
}
#ಆದರೆ_ಅಂತ್ಯ

    ಶೂನ್ಯ
ಪದ_font(ಪೂರ್ಣಾಂಕ n)
{
    ಆದರೆ (*T_CFO)
    {
	ಅಕ್ಷರ buf[20];
	sprintf(buf, (ಅಕ್ಷರ *)T_CFO, 9 + n);
	ಹೊರಹೋಗು_STR(buf);
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ಪದ_color(char_u *s, ಪೂರ್ಣಾಂಕ n)
{
    ಅಕ್ಷರ	buf[20];
    ಪೂರ್ಣಾಂಕ		i = *s == CSI ? 1 : 2;
		// index in s[] just after <Esc>[ or CSI

    // Special handling of 16 colors, because termcap can't handle it
    // Also accept "\e[3%dm" for TERMINFO, it is sometimes used
    // Also accept CSI instead of <Esc>[
    ಆದರೆ (n >= 8 && t_colors >= 16
	      && ((s[0] == ESC && s[1] == '[')
#ಆದರೆ defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
		  || (s[0] == ESC && s[1] == '|')
#ಆದರೆ_ಅಂತ್ಯ
		  || (s[0] == CSI && (i = 1) == 1))
	      && s[i] != NUL
	      && (STRCMP(s + i + 1, "%p1%dm") == 0
		  || STRCMP(s + i + 1, "%dm") == 0)
	      && (s[i] == '3' || s[i] == '4'))
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TERMINFO
	ಅಕ್ಷರ *format = "%s%s%%p1%%dm";
#ಇಲ್ಲದಿದ್ದರೆ
	ಅಕ್ಷರ *format = "%s%s%%dm";
#ಆದರೆ_ಅಂತ್ಯ
	ಅಕ್ಷರ *lead = i == 2 ? (
#ಆದರೆ defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
		    s[1] == '|' ? "\033|" :
#ಆದರೆ_ಅಂತ್ಯ
		    "\033[") : "\233";
	ಅಕ್ಷರ *ಬಾಲ = s[i] == '3' ? (n >= 16 ? "38;5;" : "9")
				 : (n >= 16 ? "48;5;" : "10");

	sprintf(buf, format, lead, ಬಾಲ);
	ಹೊರಹೋಗು_STR(tgoto(buf, 0, n >= 16 ? n : n - 8));
    }
    ಇಲ್ಲದಿದ್ದರೆ
	ಹೊರಹೋಗು_STR(tgoto((ಅಕ್ಷರ *)s, 0, n));
}

    ಶೂನ್ಯ
ಪದ_fg_color(ಪೂರ್ಣಾಂಕ n)
{
    // Use "AF" termcap entry if present, "Sf" entry otherwise
    ಆದರೆ (*T_CAF)
	ಪದ_color(T_CAF, n);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*T_CSF)
	ಪದ_color(T_CSF, n);
}

    ಶೂನ್ಯ
ಪದ_bg_color(ಪೂರ್ಣಾಂಕ n)
{
    // Use "AB" termcap entry if present, "Sb" entry otherwise
    ಆದರೆ (*T_CAB)
	ಪದ_color(T_CAB, n);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*T_CSB)
	ಪದ_color(T_CSB, n);
}

    ಶೂನ್ಯ
ಪದ_ul_color(ಪೂರ್ಣಾಂಕ n)
{
    ಆದರೆ (*T_CAU)
	ಪದ_color(T_CAU, n);
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
ಪದ_bg_default(ಶೂನ್ಯ)
{
#ಆದರೆ defined(MSWIN)
    // DOS console is nearly always black
    ಹಿಂತಿರುಗಿ (char_u *)"dark";
#ಇಲ್ಲದಿದ್ದರೆ
    char_u	*p;

    ಆದರೆ (STRCMP(T_NAME, "linux") == 0
	    || STRCMP(T_NAME, "screen.linux") == 0
	    || STRNCMP(T_NAME, "cygwin", 6) == 0
	    || STRNCMP(T_NAME, "putty", 5) == 0
	    || ((p = mch_getenv((char_u *)"COLORFGBG")) != ಖಾಲಿ
		&& (p = vim_strrchr(p, ';')) != ಖಾಲಿ
		&& ((p[1] >= '0' && p[1] <= '6') || p[1] == '8')
		&& p[2] == NUL))
	ಹಿಂತಿರುಗಿ (char_u *)"dark";
    ಹಿಂತಿರುಗಿ (char_u *)"light";
#ಆದರೆ_ಅಂತ್ಯ
}

#ಆದರೆ defined(FEAT_TERMGUICOLORS)

# define RED(rgb)   (((long_u)(rgb) >> 16) & 0xFF)
# define GREEN(rgb) (((long_u)(rgb) >>  8) & 0xFF)
# define BLUE(rgb)  (((long_u)(rgb)      ) & 0xFF)

    ಸ್ಥಿರ ಶೂನ್ಯ
ಪದ_rgb_color(char_u *s, guicolor_T rgb)
{
# define MAX_COLOR_STR_LEN 100
    ಅಕ್ಷರ	buf[MAX_COLOR_STR_LEN];

    ಆದರೆ (*s == NUL)
	ಹಿಂತಿರುಗಿ;
    vim_snprintf(buf, MAX_COLOR_STR_LEN,
				  (ಅಕ್ಷರ *)s, RED(rgb), GREEN(rgb), BLUE(rgb));
# ifdef FEAT_VTP
    ಆದರೆ (use_vtp() && (p_tgc || t_colors >= 256))
    {
	ಹೊರಹೋಗು_flush();
	buf[1] = '[';
	vtp_printf(buf);
    }
    ಇಲ್ಲದಿದ್ದರೆ
# endif
	ಹೊರಹೋಗು_STR(buf);
}

    ಶೂನ್ಯ
ಪದ_fg_rgb_color(guicolor_T rgb)
{
    ಆದರೆ (rgb != INVALCOLOR)
	ಪದ_rgb_color(T_8F, rgb);
}

    ಶೂನ್ಯ
ಪದ_bg_rgb_color(guicolor_T rgb)
{
    ಆದರೆ (rgb != INVALCOLOR)
	ಪದ_rgb_color(T_8B, rgb);
}

    ಶೂನ್ಯ
ಪದ_ul_rgb_color(guicolor_T rgb)
{
# ifdef FEAT_TERMRESPONSE
    // If the user explicitly sets t_8u then use it.  Otherwise wait for
    // termresponse to be received, which is when t_8u would be set and a
    // redraw is needed if it was used.
    ಆದರೆ (!option_was_ಗಣ((char_u *)"t_8u") && write_t_8u_state != OK)
	write_t_8u_state = MAYBE;
    ಇಲ್ಲದಿದ್ದರೆ
# endif
	ಪದ_rgb_color(T_8U, rgb);
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(UNIX) || defined(VMS) || defined(MACOS_X)
/*
 * Generic function to set window title, using t_ts and t_fs.
 */
    ಶೂನ್ಯ
ಪದ_settitle(char_u *title)
{
    MAY_WANT_ಗಮ್ಯ_LOG_THIS;

    // t_ts takes one argument: column in status line
    ಹೊರಹೋಗು_STR(tgoto((ಅಕ್ಷರ *)T_TS, 0, 0));	// set title start
    ಹೊರಹೋಗು_str_nf(title);
    ಹೊರಹೋಗು_str(T_FS);			// set title end
    ಹೊರಹೋಗು_flush();
}

/*
 * Tell the terminal to push (save) the title and/or icon, so that it can be
 * popped (restored) later.
 */
    ಶೂನ್ಯ
ಪದ_ತಳ್ಳು_title(ಪೂರ್ಣಾಂಕ which)
{
    ಆದರೆ ((which & SAVE_RESTORE_TITLE) && T_CST != ಖಾಲಿ && *T_CST != NUL)
    {
	ಹೊರಹೋಗು_STR(T_CST);
	ಹೊರಹೋಗು_flush();
    }

    ಆದರೆ ((which & SAVE_RESTORE_ICON) && T_SSI != ಖಾಲಿ && *T_SSI != NUL)
    {
	ಹೊರಹೋಗು_STR(T_SSI);
	ಹೊರಹೋಗು_flush();
    }
}

/*
 * Tell the terminal to pop the title and/or icon.
 */
    ಶೂನ್ಯ
ಪದ_ತೆಗೆ_title(ಪೂರ್ಣಾಂಕ which)
{
    ಆದರೆ ((which & SAVE_RESTORE_TITLE) && T_CRT != ಖಾಲಿ && *T_CRT != NUL)
    {
	ಹೊರಹೋಗು_STR(T_CRT);
	ಹೊರಹೋಗು_flush();
    }

    ಆದರೆ ((which & SAVE_RESTORE_ICON) && T_SRI != ಖಾಲಿ && *T_SRI != NUL)
    {
	ಹೊರಹೋಗು_STR(T_SRI);
	ಹೊರಹೋಗು_flush();
    }
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Make sure we have a valid set or terminal options.
 * Replace all entries that are NULL by empty_option
 */
    ಶೂನ್ಯ
ttest(ಪೂರ್ಣಾಂಕ ಜೋಡಿಗಳು)
{
    char_u *env_colors;

    check_options();		    // make sure no options are NULL

    /*
     * MUST have "cm": cursor motion.
     */
    ಆದರೆ (*T_CM == NUL)
	emsg(_(e_terminal_capability_cm_required));

    /*
     * if "cs" defined, use a scroll region, it's faster.
     */
    ಆದರೆ (*T_CS != NUL)
	scroll_region = TRUE;
    ಇಲ್ಲದಿದ್ದರೆ
	scroll_region = FALSE;

    ಆದರೆ (ಜೋಡಿಗಳು)
    {
	/*
	 * optional pairs
	 */
	// TP goes to normal mode for TI (invert) and TB (bold)
	ಆದರೆ (*T_ME == NUL)
	    T_ME = T_MR = T_MD = T_MB = ಖಾಲಿಗಳು_option;
	ಆದರೆ (*T_SO == NUL || *T_SE == NUL)
	    T_SO = T_SE = ಖಾಲಿಗಳು_option;
	ಆದರೆ (*T_US == NUL || *T_UE == NUL)
	    T_US = T_UE = ಖಾಲಿಗಳು_option;
	ಆದರೆ (*T_CZH == NUL || *T_CZR == NUL)
	    T_CZH = T_CZR = ಖಾಲಿಗಳು_option;

	// T_VE is needed even though T_VI is not defined
	ಆದರೆ (*T_VE == NUL)
	    T_VI = ಖಾಲಿಗಳು_option;

	// if 'mr' or 'me' is not defined use 'so' and 'se'
	ಆದರೆ (*T_ME == NUL)
	{
	    T_ME = T_SE;
	    T_MR = T_SO;
	    T_MD = T_SO;
	}

	// if 'so' or 'se' is not defined use 'mr' and 'me'
	ಆದರೆ (*T_SO == NUL)
	{
	    T_SE = T_ME;
	    ಆದರೆ (*T_MR == NUL)
		T_SO = T_MD;
	    ಇಲ್ಲದಿದ್ದರೆ
		T_SO = T_MR;
	}

	// if 'ZH' or 'ZR' is not defined use 'mr' and 'me'
	ಆದರೆ (*T_CZH == NUL)
	{
	    T_CZR = T_ME;
	    ಆದರೆ (*T_MR == NUL)
		T_CZH = T_MD;
	    ಇಲ್ಲದಿದ್ದರೆ
		T_CZH = T_MR;
	}

	// "Sb" and "Sf" come in pairs
	ಆದರೆ (*T_CSB == NUL || *T_CSF == NUL)
	{
	    T_CSB = ಖಾಲಿಗಳು_option;
	    T_CSF = ಖಾಲಿಗಳು_option;
	}

	// "AB" and "AF" come in pairs
	ಆದರೆ (*T_CAB == NUL || *T_CAF == NUL)
	{
	    T_CAB = ಖಾಲಿಗಳು_option;
	    T_CAF = ಖಾಲಿಗಳು_option;
	}

	// if 'Sb' and 'AB' are not defined, reset "Co"
	ಆದರೆ (*T_CSB == NUL && *T_CAB == NUL)
	    free_one_termoption(T_CCO);

	// Set 'weirdinvert' according to value of 't_xs'
	p_wiv = (*T_XS != NUL);
    }
    need_gather = TRUE;

    // Set t_colors to the value of $COLORS or t_Co.  Ignore $COLORS in the
    // GUI.
    t_colors = atoi((ಅಕ್ಷರ *)T_CCO);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    ಆದರೆ (!gui.ಒಳ_use)
#ಆದರೆ_ಅಂತ್ಯ
    {
	env_colors = mch_getenv((char_u *)"COLORS");
	ಆದರೆ (env_colors != ಖಾಲಿ && ಸುರಕ್ಷಿತ_isdigit(*env_colors))
	{
	    ಪೂರ್ಣಾಂಕ colors = atoi((ಅಕ್ಷರ *)env_colors);

	    ಆದರೆ (colors != t_colors)
		ಗಣ_color_ಎಣಿಕೆ(colors);
	}
    }
    ಗಣ_rgb_ಪದ_prop();
}

#ಆದರೆ defined(FEAT_GUI) && (defined(FEAT_MENU) || !defined(USE_ON_FLY_SCROLL))
/*
 * Represent the given long_u as individual bytes, with the most significant
 * byte first, and store them in dst.
 */
    ಶೂನ್ಯ
ಕೂಡಿಸು_long_ಗಮ್ಯ_buf(long_u val, char_u *dst)
{
    ಪೂರ್ಣಾಂಕ	    i;
    ಪೂರ್ಣಾಂಕ	    ಸರಿಸು;

    ಗಾಗಿ (i = 1; i <= (ಪೂರ್ಣಾಂಕ)ಗಾತ್ರ(long_u); i++)
    {
	ಸರಿಸು = 8 * (ಗಾತ್ರ(long_u) - i);
	dst[i - 1] = (char_u) ((val >> ಸರಿಸು) & 0xff);
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
get_long_from_buf(char_u *buf, long_u *val)
{
    ಪೂರ್ಣಾಂಕ	    len;
    char_u  ಬೈಟುಗಳು[ಗಾತ್ರ(long_u)];
    ಪೂರ್ಣಾಂಕ	    i;
    ಪೂರ್ಣಾಂಕ	    ಸರಿಸು;

    *val = 0;
    len = get_ಬೈಟುಗಳು_from_buf(buf, ಬೈಟುಗಳು, (ಪೂರ್ಣಾಂಕ)ಗಾತ್ರ(long_u));
    ಆದರೆ (len == -1)
	ಹಿಂತಿರುಗಿ -1;
    ಗಾಗಿ (i = 0; i < (ಪೂರ್ಣಾಂಕ)ಗಾತ್ರ(long_u); i++)
    {
	ಸರಿಸು = 8 * (ಗಾತ್ರ(long_u) - 1 - i);
	*val += (long_u)ಬೈಟುಗಳು[i] << ಸರಿಸು;
    }
    ಹಿಂತಿರುಗಿ len;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Read the next num_bytes bytes from buf, and store them in bytes.  Assume
 * that buf has been through inchar().	Returns the actual number of bytes used
 * from buf (between num_bytes and num_bytes*2), or -1 if not enough bytes were
 * available.
 */
    ಪೂರ್ಣಾಂಕ
get_ಬೈಟುಗಳು_from_buf(char_u *buf, char_u *ಬೈಟುಗಳು, ಪೂರ್ಣಾಂಕ ಅಂಕೆ_ಬೈಟುಗಳು)
{
    ಪೂರ್ಣಾಂಕ	    len = 0;
    ಪೂರ್ಣಾಂಕ	    i;
    char_u  c;

    ಗಾಗಿ (i = 0; i < ಅಂಕೆ_ಬೈಟುಗಳು; i++)
    {
	ಆದರೆ ((c = buf[len++]) == NUL)
	    ಹಿಂತಿರುಗಿ -1;
	ಆದರೆ (c == K_SPECIAL)
	{
	    ಆದರೆ (buf[len] == NUL || buf[len + 1] == NUL)	    // cannot happen?
		ಹಿಂತಿರುಗಿ -1;
	    ಆದರೆ (buf[len++] == (ಪೂರ್ಣಾಂಕ)KS_ZERO)
		c = NUL;
	    // else it should be KS_SPECIAL; when followed by KE_FILLER c is
	    // K_SPECIAL, or followed by KE_CSI and c must be CSI.
	    ಆದರೆ (buf[len++] == (ಪೂರ್ಣಾಂಕ)KE_CSI)
		c = CSI;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == CSI && buf[len] == KS_EXTRA
					       && buf[len + 1] == (ಪೂರ್ಣಾಂಕ)KE_CSI)
	    // CSI is stored as CSI KS_SPECIAL KE_CSI to avoid confusion with
	    // the start of a special key, see add_to_input_buf_csi().
	    len += 2;
	ಬೈಟುಗಳು[i] = c;
    }
    ಹಿಂತಿರುಗಿ len;
}

/*
 * Check if the new shell size is valid, correct it if it's too small or way
 * too big.
 */
    ಶೂನ್ಯ
check_shellsize(ಶೂನ್ಯ)
{
    // need room for one window and command line
    ಆದರೆ (ಅಡ್ಡಸಾಲುಗಳು < min_ಅಡ್ಡಸಾಲುಗಳು_for_all_tabpages())
	ಅಡ್ಡಸಾಲುಗಳು = min_ಅಡ್ಡಸಾಲುಗಳು_for_all_tabpages();
    ಎಲ್ಲೆ_screen_ಗಾತ್ರ();

    // make sure these values are not invalid
    ಆದರೆ (cmdline_ಅಡ್ಡಸಾಲು >= ಅಡ್ಡಸಾಲುಗಳು)
	cmdline_ಅಡ್ಡಸಾಲು = ಅಡ್ಡಸಾಲುಗಳು - 1;
    ಆದರೆ (ಸಂದೇಶ_ಅಡ್ಡಸಾಲು >= ಅಡ್ಡಸಾಲುಗಳು)
	ಸಂದೇಶ_ಅಡ್ಡಸಾಲು = ಅಡ್ಡಸಾಲುಗಳು - 1;
}

/*
 * Limit Rows and Columns to avoid an overflow in Rows * Columns.
 */
    ಶೂನ್ಯ
ಎಲ್ಲೆ_screen_ಗಾತ್ರ(ಶೂನ್ಯ)
{
    ಆದರೆ (Columns < MIN_COLUMNS)
	Columns = MIN_COLUMNS;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (Columns > 10000)
	Columns = 10000;
    ಆದರೆ (ಅಡ್ಡಸಾಲುಗಳು > 1000)
	ಅಡ್ಡಸಾಲುಗಳು = 1000;
}

/*
 * Invoked just before the screen structures are going to be (re)allocated.
 */
    ಶೂನ್ಯ
win_new_shellsize(ಶೂನ್ಯ)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	old_ಅಡ್ಡಸಾಲುಗಳು_2 = 0;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	old_Columns = 0;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	old_coloff = 0;

    ಆದರೆ (old_ಅಡ್ಡಸಾಲುಗಳು_2 != ಅಡ್ಡಸಾಲುಗಳು || old_Columns != COLUMNS_WITHOUT_TPL()
	    || old_coloff != TPL_LCOL())
	ui_new_shellsize();
    ಆದರೆ (old_Columns != COLUMNS_WITHOUT_TPL() || old_coloff != TPL_LCOL())
    {
	old_Columns = COLUMNS_WITHOUT_TPL();
	old_coloff = TPL_LCOL();

	shell_new_columns();
    }
    ಆದರೆ (old_ಅಡ್ಡಸಾಲುಗಳು_2 != ಅಡ್ಡಸಾಲುಗಳು)
    {
	// If 'window' uses the whole screen, keep it using that.
	// Don't change it when set with "-w size" on the command line.
	ಆದರೆ (p_ಕಿಟಕಿ == old_ಅಡ್ಡಸಾಲುಗಳು_2 - 1
		    || (old_ಅಡ್ಡಸಾಲುಗಳು_2 == 0 && !option_was_ಗಣ((char_u *)"window")))
	    p_ಕಿಟಕಿ = ಅಡ್ಡಸಾಲುಗಳು - 1;
	old_ಅಡ್ಡಸಾಲುಗಳು_2 = ಅಡ್ಡಸಾಲುಗಳು;
	shell_new_ಅಡ್ಡಸಾಲುಗಳು();	// update window sizes
    }
}

#ಆದರೆ defined(FEAT_IMAGE) || defined(FEAT_EVAL)
    ಶೂನ್ಯ
update_cell_ಗಾತ್ರ(ಶೂನ್ಯ)
{
# ifdef FEAT_GUI
    ಆದರೆ (gui.ಒಳ_use)
    {
	cell_ಅಗಲ = gui.char_ಅಗಲ;
	cell_ಎತ್ತರ = gui.char_ಎತ್ತರ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
# endif
    {
	ರಚನೆ cellsize cell_sz;

	mch_calc_cell_ಗಾತ್ರ(&cell_sz);
	ಆದರೆ (cell_sz.cs_xpixel <= 0 || cell_sz.cs_ypixel <= 0)
	{
	    cell_ಅಗಲ = 8;
	    cell_ಎತ್ತರ = 16;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    cell_ಅಗಲ = cell_sz.cs_xpixel;
	    cell_ಎತ್ತರ = cell_sz.cs_ypixel;
	}
    }
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Call this function when the Vim shell has been resized in any way.
 * Will obtain the current size and redraw (also when size didn't change).
 */
    ಶೂನ್ಯ
shell_resized(ಶೂನ್ಯ)
{
#ಆದರೆ defined(FEAT_IMAGE) || defined(FEAT_EVAL)
    update_cell_ಗಾತ್ರ();
#ಆದರೆ_ಅಂತ್ಯ
    ಗಣ_shellsize(0, 0, FALSE);
}

/*
 * Check if the shell size changed.  Handle a resize.
 * When the size didn't change, nothing happens.
 */
    ಶೂನ್ಯ
shell_resized_check(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		old_ಅಡ್ಡಸಾಲುಗಳು_2 = ಅಡ್ಡಸಾಲುಗಳು;
    ಪೂರ್ಣಾಂಕ		old_Columns = Columns;

    ಆದರೆ (exiting
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
	    // Do not get the size when executing a shell command during
	    // startup.
	    || gui.starting
#ಆದರೆ_ಅಂತ್ಯ
	    )
	ಹಿಂತಿರುಗಿ;

    (ಶೂನ್ಯ)ui_get_shellsize();
    check_shellsize();
    ಆದರೆ (old_ಅಡ್ಡಸಾಲುಗಳು_2 != ಅಡ್ಡಸಾಲುಗಳು || old_Columns != Columns)
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
ಗಣ_shellsize_inner(ಪೂರ್ಣಾಂಕ ಅಗಲ, ಪೂರ್ಣಾಂಕ ಎತ್ತರ, ಪೂರ್ಣಾಂಕ mustset)
{
    ಆದರೆ (updating_screen)
	// resizing while in update_screen() may cause a crash
	ಹಿಂತಿರುಗಿ;

    // curwin->w_buffer can be NULL when we are closing a window and the
    // buffer (or window) has already been closed and removing a scrollbar
    // causes a resize event. Don't resize then, it will happen after entering
    // another buffer.
    ಆದರೆ (curwin->w_ಬಫರ್ == ಖಾಲಿ || curwin->w_ಸಾಲುಗಳು == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ AMIGA
    ಹೊರಹೋಗು_flush();	    // must do this before mch_get_shellsize() for
			    // some obscure reason
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (mustset || (ui_get_shellsize() == FAIL && ಎತ್ತರ != 0))
    {
	ಅಡ್ಡಸಾಲುಗಳು = ಎತ್ತರ;
	Columns = ಅಗಲ;
	check_shellsize();
	ui_ಗಣ_shellsize(mustset);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	check_shellsize();

    // The window layout used to be adjusted here, but it now happens in
    // screenalloc() (also invoked from screenclear()).  That is because the
    // "busy" check above may skip this, but not screenalloc().

    ಆದರೆ (State != ಬಹುಲಕ_ASKMORE && State != ಬಹುಲಕ_EXTERNCMD
						      && State != ಬಹುಲಕ_CONFIRM)
	screenclear();
    ಇಲ್ಲದಿದ್ದರೆ
	screen_ಆರಂಭ();	    // don't know where cursor is now

    ಆದರೆ (starting != NO_SCREEN)
    {
	maketitle();

	ಬದಲಾಯಿತು_ಸಾಲು_abv_curs();
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
	ಆದರೆ (State == ಬಹುಲಕ_ASKMORE || State == ಬಹುಲಕ_EXTERNCMD
				     || State == ಬಹುಲಕ_CONFIRM || exmode_ಸಕ್ರಿಯ)
	{
	    screenalloc(FALSE);
	    repeat_ಸಂದೇಶ();
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (curwin->w_p_scb)
		do_check_scrollbind(TRUE);
	    ಆದರೆ (State & ಬಹುಲಕ_CMDLINE)
	    {
		update_screen(UPD_NOT_VALID);
		redrawcmdline();
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		update_topline();
		ಆದರೆ (pum_visible())
		{
		    redraw_later(UPD_NOT_VALID);
		    ins_compl_show_pum();
		}
		update_screen(UPD_NOT_VALID);
		ಆದರೆ (redrawing())
		    setcursor();
	    }
	}
	cursor_on();	    // redrawing may have switched it off
    }
    ಹೊರಹೋಗು_flush();
}

    ಶೂನ್ಯ
ಗಣ_shellsize(ಪೂರ್ಣಾಂಕ ಅಗಲ, ಪೂರ್ಣಾಂಕ ಎತ್ತರ, ಪೂರ್ಣಾಂಕ mustset)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	busy = FALSE;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	do_ಸತತ = FALSE;

    ಆದರೆ (ಅಗಲ < 0 || ಎತ್ತರ < 0)    // just checking...
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (State == ಬಹುಲಕ_HITRETURN || State == ಬಹುಲಕ_SETWSIZE)
    {
	// postpone the resizing
	State = ಬಹುಲಕ_SETWSIZE;
	ಹಿಂತಿರುಗಿ;
    }

    // Avoid recursiveness.  This can happen when setting the window size
    // causes another window-changed signal or when two SIGWINCH signals come
    // very close together.  There needs to be another run then after the
    // current one is done to pick up the latest size.
    do_ಸತತ = TRUE;
    ಆದರೆ (busy)
	ಹಿಂತಿರುಗಿ;

    ಆಗ (do_ಸತತ)
    {
	do_ಸತತ = FALSE;
	busy = TRUE;
	ಗಣ_shellsize_inner(ಅಗಲ, ಎತ್ತರ, mustset);
	busy = FALSE;
    }
}

/*
 * Output T_CTE, the t_TE termcap entry, and handle expected effects.
 * The code possibly disables modifyOtherKeys and the Kitty keyboard protocol.
 */
    ಶೂನ್ಯ
ಹೊರಹೋಗು_str_t_TE(ಶೂನ್ಯ)
{
    ಹೊರಹೋಗು_str(T_CTE);

    // The seenModifyOtherKeys flag is not reset here.  We do expect t_TE to
    // disable modifyOtherKeys, but until Xterm version 377 there is no way to
    // detect it's enabled again after the following t_TI.  We assume that when
    // seenModifyOtherKeys was set before it will still be valid.

    // When the modifyOtherKeys level is detected to be 2 we expect t_TE to
    // disable it.  Remembering that it was detected to be enabled is useful in
    // some situations.
    // The following t_TI is expected to request the state and then
    // modify_otherkeys_state will be set again.
    ಆದರೆ (modify_otherkeys_state == MOKS_ENABLED
	    || modify_otherkeys_state == MOKS_DISABLED)
	modify_otherkeys_state = MOKS_DISABLED;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (modify_otherkeys_state != MOKS_INITIAL)
	modify_otherkeys_state = MOKS_AFTER_T_TE;

    // When the kitty keyboard protocol is enabled we expect t_TE to disable
    // it.  Remembering that it was detected to be enabled is useful in some
    // situations.
    // The following t_TI is expected to request the state and then
    // kitty_protocol_state will be set again.
    ಆದರೆ (kitty_protocol_state == KKPS_ENABLED
	    || kitty_protocol_state == KKPS_DISABLED)
	kitty_protocol_state = KKPS_DISABLED;
    ಇಲ್ಲದಿದ್ದರೆ
	kitty_protocol_state = KKPS_AFTER_T_TE;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ send_t_RK = FALSE;

/*
 * Output T_TI and setup for what follows.
 */
    ಶೂನ್ಯ
ಹೊರಹೋಗು_str_t_TI(ಶೂನ್ಯ)
{
    ಹೊರಹೋಗು_str(T_CTI);

    // Send t_RK when there is no more work to do.
    send_t_RK = TRUE;
}

/*
 * Output T_BE, but only when t_PS and t_PE are set.
 */
    ಶೂನ್ಯ
ಹೊರಹೋಗು_str_t_BE(ಶೂನ್ಯ)
{
    char_u *p;

    ಆದರೆ (T_BE == ಖಾಲಿ || *T_BE == NUL
	    || (p = ಕಂಡುಹಿಡಿ_termcode((char_u *)"PS")) == ಖಾಲಿ || *p == NUL
	    || (p = ಕಂಡುಹಿಡಿ_termcode((char_u *)"PE")) == ಖಾಲಿ || *p == NUL)
	ಹಿಂತಿರುಗಿ;
    ಹೊರಹೋಗು_str(T_BE);
}

/*
 * If t_TI was recently sent and there is no typeahead or work to do, now send
 * t_RK.  This is postponed to avoid the response arriving in a shell command
 * or after Vim exits.
 */
    ಶೂನ್ಯ
may_send_t_RK(ಶೂನ್ಯ)
{
    ಆದರೆ (send_t_RK
	    && !work_pending()
	    && !ex_ಸಾಮಾನ್ಯ_busy
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	    && !ಒಳ_feedkeys
#ಆದರೆ_ಅಂತ್ಯ
	    && !exiting)
    {
	send_t_RK = FALSE;
	ಹೊರಹೋಗು_str(T_CRK);
	ಹೊರಹೋಗು_flush();
    }
}

/*
 * Set the terminal to TMODE_RAW (for Normal mode) or TMODE_COOK (for external
 * commands and Ex mode).
 */
    ಶೂನ್ಯ
settmode(tmode_T tmode)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    // don't set the term where gvim was started to any mode
    ಆದರೆ (gui.ಒಳ_use)
	ಹಿಂತಿರುಗಿ;
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (!ತುಂಬಿದ_screen)
	ಹಿಂತಿರುಗಿ;

    /*
     * When returning after calling a shell cur_tmode is TMODE_UNKNOWN,
     * set the terminal to raw mode, even though we think it already is,
     * because the shell program may have reset the terminal mode.
     * When we think the terminal is normal, don't try to set it to
     * normal again, because that causes problems (logout!) on some
     * machines.
     */
    ಆದರೆ (tmode != cur_tmode)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMRESPONSE
# ifdef FEAT_GUI
	ಆದರೆ (!gui.ಒಳ_use && !gui.starting)
# endif
	{
	    // May need to check for T_CRV response and termcodes, it
	    // doesn't work in Cooked mode, an external program may get
	    // them.
	    ಆದರೆ (tmode != TMODE_RAW && termrequest_any_pending())
		(ಶೂನ್ಯ)vpeekc_nomap();
	    check_for_codes_from_ಪದ();
	}
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ (tmode != TMODE_RAW)
	    mch_setmouse(FALSE);	// switch mouse off

	// Disable bracketed paste and modifyOtherKeys in cooked mode.
	// Avoid doing this too often, on some terminals the codes are not
	// handled properly.
	ಆದರೆ (termcap_ಸಕ್ರಿಯ && tmode != TMODE_SLEEP
		&& cur_tmode != TMODE_SLEEP)
	{
	    MAY_WANT_ಗಮ್ಯ_LOG_THIS;

	    ಆದರೆ (tmode != TMODE_RAW)
	    {
		ಹೊರಹೋಗು_str(T_BD);	// disable bracketed paste mode
		ಹೊರಹೋಗು_str_t_TE();	// possibly disables modifyOtherKeys
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಹೊರಹೋಗು_str_t_BE();	// enable bracketed paste mode (should
				// be before mch_settmode().
		ಹೊರಹೋಗು_str_t_TI();	// possibly enables modifyOtherKeys
	    }
	}
	ಹೊರಹೋಗು_flush();
	mch_settmode(tmode);	// machine specific function
	cur_tmode = tmode;
	ಆದರೆ (tmode == TMODE_RAW)
	    setmouse();		// may switch mouse on
	ಹೊರಹೋಗು_flush();
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMRESPONSE
    may_req_termresponse();
#ಆದರೆ_ಅಂತ್ಯ
}

    ಶೂನ್ಯ
starttermcap(ಶೂನ್ಯ)
{
    ಆದರೆ (!ತುಂಬಿದ_screen || termcap_ಸಕ್ರಿಯ)
	ಹಿಂತಿರುಗಿ;

    MAY_WANT_ಗಮ್ಯ_LOG_THIS;

    ಹೊರಹೋಗು_str(T_TI);			// start termcap mode
    ಹೊರಹೋಗು_str_t_TI();			// start "raw" mode
    ಹೊರಹೋಗು_str(T_KS);			// start "keypad transmit" mode
    ಹೊರಹೋಗು_str_t_BE();			// enable bracketed paste mode

#ಆದರೆ defined(UNIX) || defined(VMS)
    // Enable xterm's focus reporting mode when 'esckeys' is set.
    ಆದರೆ (p_ek && *T_FE != NUL)
	ಹೊರಹೋಗು_str(T_FE);
#ಆದರೆ_ಅಂತ್ಯ

    ಹೊರಹೋಗು_flush();
    termcap_ಸಕ್ರಿಯ = TRUE;
    screen_ಆರಂಭ();			// don't know where cursor is now
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMRESPONSE
# ifdef FEAT_GUI
    ಆದರೆ (!gui.ಒಳ_use && !gui.starting)
# endif
    {
	may_req_termresponse();
	// Immediately check for a response.  If t_Co changes, we don't
	// want to redraw with wrong colors first.
	ಆದರೆ (crv_ಸ್ಥಿತಿ.tr_ಪ್ರಗತಿ == ಸ್ಥಿತಿ_ಕಳುಹಿಸಿದ)
	    check_for_codes_from_ಪದ();
    }
#ಆದರೆ_ಅಂತ್ಯ
}

    ಶೂನ್ಯ
stoptermcap(ಶೂನ್ಯ)
{
    screen_stop_highlight();
    reset_cterm_colors();

    ಆದರೆ (!termcap_ಸಕ್ರಿಯ)
	ಹಿಂತಿರುಗಿ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMRESPONSE
# ifdef FEAT_GUI
    ಆದರೆ (!gui.ಒಳ_use && !gui.starting)
# endif
    {
	// May need to discard T_CRV, T_U7 or T_RBG response.
	ಆದರೆ (termrequest_any_pending())
	{
# ifdef UNIX
	    // Give the terminal a chance to respond.
	    mch_delay(100L, 0);
# endif
# ifdef TCIFLUSH
	    // Discard data received but not read.
	    ಆದರೆ (exiting)
		tcflush(fileno(stdin), TCIFLUSH);
# endif
	}
	// Check for termcodes first, otherwise an external program may
	// get them.
	check_for_codes_from_ಪದ();
    }
#ಆದರೆ_ಅಂತ್ಯ
    MAY_WANT_ಗಮ್ಯ_LOG_THIS;

#ಆದರೆ defined(UNIX) || defined(VMS)
    // Disable xterm's focus reporting mode if 'esckeys' is set.
    ಆದರೆ (p_ek && *T_FD != NUL)
	ಹೊರಹೋಗು_str(T_FD);
#ಆದರೆ_ಅಂತ್ಯ

    ಹೊರಹೋಗು_str(T_BD);			// disable bracketed paste mode
    ಹೊರಹೋಗು_str(T_KE);			// stop "keypad transmit" mode
    ಹೊರಹೋಗು_flush();
    termcap_ಸಕ್ರಿಯ = FALSE;

    // Output t_te before t_TE, t_te may switch between main and alternate
    // screen and following codes may work on the active screen only.
    //
    // When using the Kitty keyboard protocol the main and alternate screen
    // use a separate state.  If we are (or were) using the Kitty keyboard
    // protocol and t_te is not empty (possibly switching screens) then
    // output t_TE both before and after outputting t_te.
    ಆದರೆ (*T_TE != NUL && (kitty_protocol_state == KKPS_ENABLED
		|| kitty_protocol_state == KKPS_DISABLED))
	ಹೊರಹೋಗು_str_t_TE();		// probably disables the kitty keyboard
				// protocol

    ಹೊರಹೋಗು_str(T_TE);			// stop termcap mode
    cursor_on();			// just in case it is still off
    ಹೊರಹೋಗು_str_t_TE();			// stop "raw" mode, modifyOtherKeys and
					// Kitty keyboard protocol
    screen_ಆರಂಭ();			// don't know where cursor is now
    ಹೊರಹೋಗು_flush();
}

#ಆದರೆ defined(FEAT_TERMRESPONSE)
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
    ಆದರೆ (crv_ಸ್ಥಿತಿ.tr_ಪ್ರಗತಿ == ಸ್ಥಿತಿ_GET
	    && can_get_termresponse()
	    && starting == 0
	    && *T_CRV != NUL)
    {
	MAY_WANT_ಗಮ್ಯ_LOG_THIS;
	LOG_TR1("Sending CRV request");
	ಹೊರಹೋಗು_str(T_CRV);
	termrequest_ಕಳುಹಿಸಿದ(&crv_ಸ್ಥಿತಿ);
	// check for the characters now, otherwise they might be eaten by
	// get_keystroke()
	ಹೊರಹೋಗು_flush();
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
    ಪೂರ್ಣಾಂಕ	    did_send = FALSE;

    ಆದರೆ (!can_get_termresponse() || starting != 0 || *T_U7 == NUL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (u7_ಸ್ಥಿತಿ.tr_ಪ್ರಗತಿ == ಸ್ಥಿತಿ_GET
	    && !option_was_ಗಣ((char_u *)"ambiwidth"))
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
	MAY_WANT_ಗಮ್ಯ_LOG_THIS;
	LOG_TR1("Sending request for ambiwidth check");
	// Do this in the second row.  In the first row the returned sequence
	// may be CSI 1;2R, which is the same as <S-F3>.
	ಪದ_windgoto(1, 0);
	buf[mb_char2bytes(0x25bd, buf)] = NUL;
	ಹೊರಹೋಗು_str(buf);
	ಹೊರಹೋಗು_str(T_U7);
	termrequest_ಕಳುಹಿಸಿದ(&u7_ಸ್ಥಿತಿ);
	ಹೊರಹೋಗು_flush();
	did_send = TRUE;

	// This overwrites a few characters on the screen, a redraw is needed
	// after this. Clear them out for now.
	screen_stop_highlight();
	ಪದ_windgoto(1, 0);
	ಹೊರಹೋಗು_str((char_u *)"  ");
	ಸಾಲು_was_clobbered(1);
    }

    ಆದರೆ (xcc_ಸ್ಥಿತಿ.tr_ಪ್ರಗತಿ == ಸ್ಥಿತಿ_GET && ಅಡ್ಡಸಾಲುಗಳು > 2)
    {
	// 2. Check compatibility with xterm.
	// We move the cursor to (2, 0), print a test sequence and then query
	// the current cursor position.  If the terminal properly handles
	// unknown DCS string and CSI sequence with intermediate byte, the test
	// sequence is ignored and the cursor does not move.  If the terminal
	// handles test sequence incorrectly, a garbage string is displayed and
	// the cursor does move.
	MAY_WANT_ಗಮ್ಯ_LOG_THIS;
	LOG_TR1("Sending xterm compatibility test sequence.");
	// Do this in the third row.  Second row is used by ambiguous
	// character width check.
	ಪದ_windgoto(2, 0);
	// send the test DCS string.
	ಹೊರಹೋಗು_str((char_u *)"\033Pzz\033\\");
	// send the test CSI sequence with intermediate byte.
	ಹೊರಹೋಗು_str((char_u *)"\033[0%m");
	ಹೊರಹೋಗು_str(T_U7);
	termrequest_ಕಳುಹಿಸಿದ(&xcc_ಸ್ಥಿತಿ);
	ಹೊರಹೋಗು_flush();
	did_send = TRUE;

	// If the terminal handles test sequence incorrectly, garbage text is
	// displayed. Clear them out for now.
	screen_stop_highlight();
	ಪದ_windgoto(2, 0);
	ಹೊರಹೋಗು_str((char_u *)"           ");
	ಸಾಲು_was_clobbered(2);
    }

    ಆದರೆ (did_send)
    {
	ಪದ_windgoto(0, 0);

	// Need to reset the known cursor position.
	screen_ಆರಂಭ();

	// check for the characters now, otherwise they might be eaten by
	// get_keystroke()
	ಹೊರಹೋಗು_flush();
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
    ಆದರೆ (can_get_termresponse() && starting == 0)
    {
	ಪೂರ್ಣಾಂಕ didit = FALSE;

# ifdef FEAT_TERMINAL
	// Only request foreground if t_RF is set.
	ಆದರೆ (rfg_ಸ್ಥಿತಿ.tr_ಪ್ರಗತಿ == ಸ್ಥಿತಿ_GET && *T_RFG != NUL)
	{
	    MAY_WANT_ಗಮ್ಯ_LOG_THIS;
	    LOG_TR1("Sending FG request");
	    ಹೊರಹೋಗು_str(T_RFG);
	    termrequest_ಕಳುಹಿಸಿದ(&rfg_ಸ್ಥಿತಿ);
	    didit = TRUE;
	}
# endif

	// Only request background if t_RB is set.
	ಆದರೆ (rbg_ಸ್ಥಿತಿ.tr_ಪ್ರಗತಿ == ಸ್ಥಿತಿ_GET && *T_RBG != NUL)
	{
	    MAY_WANT_ಗಮ್ಯ_LOG_THIS;
	    LOG_TR1("Sending BG request");
	    ಹೊರಹೋಗು_str(T_RBG);
	    termrequest_ಕಳುಹಿಸಿದ(&rbg_ಸ್ಥಿತಿ);
	    didit = TRUE;
	}

	ಆದರೆ (didit)
	{
	    // check for the characters now, otherwise they might be eaten by
	    // get_keystroke()
	    ಹೊರಹೋಗು_flush();
	    (ಶೂನ್ಯ)vpeekc_nomap();
	}
    }
}

#ಆದರೆ_ಅಂತ್ಯ

/*
 * Return TRUE when saving and restoring the screen.
 */
    ಪೂರ್ಣಾಂಕ
swapping_screen(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ (ತುಂಬಿದ_screen && *T_TI != NUL);
}

/*
 * By outputting the 'cursor very visible' termcap code, for some windowed
 * terminals this makes the screen scrolled to the correct position.
 * Used when starting Vim or returning from a shell.
 */
    ಶೂನ್ಯ
scroll_ಆರಂಭ(ಶೂನ್ಯ)
{
    ಆದರೆ (*T_VS == NUL || *T_CVS == NUL)
	ಹಿಂತಿರುಗಿ;

    MAY_WANT_ಗಮ್ಯ_LOG_THIS;
    ಹೊರಹೋಗು_str(T_VS);
    ಹೊರಹೋಗು_str(T_CVS);
    screen_ಆರಂಭ();		// don't know where cursor is now
}

// True if cursor is not visible
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ cursor_is_off = FALSE;

// True if cursor is not visible due to an ongoing cursor-less sleep
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ cursor_is_asleep = FALSE;

/*
 * Enable the cursor without checking if it's already enabled.
 */
    ಶೂನ್ಯ
cursor_on_force(ಶೂನ್ಯ)
{
    ಹೊರಹೋಗು_str(T_VE);
    cursor_is_off = FALSE;
    cursor_is_asleep = FALSE;
}

/*
 * Enable the cursor if it's currently off.
 */
    ಶೂನ್ಯ
cursor_on(ಶೂನ್ಯ)
{
    ಆದರೆ (cursor_is_off && !cursor_is_asleep)
	cursor_on_force();
}

/*
 * Disable the cursor.
 */
    ಶೂನ್ಯ
cursor_off(ಶೂನ್ಯ)
{
    ಆದರೆ (ತುಂಬಿದ_screen && !cursor_is_off)
    {
	ಹೊರಹೋಗು_str(T_VI);	    // disable cursor
	cursor_is_off = TRUE;
    }
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
/*
 * Check whether the cursor is invisible due to an ongoing cursor-less sleep
 */
    ಪೂರ್ಣಾಂಕ
cursor_is_sleeping(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ cursor_is_asleep;
}
#ಆದರೆ_ಅಂತ್ಯ

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

#ಆದರೆ defined(CURSOR_SHAPE)
/*
 * Set cursor shape to match Insert or Replace mode.
 */
    ಶೂನ್ಯ
ಪದ_cursor_ಬಹುಲಕ(ಪೂರ್ಣಾಂಕ forced)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ showing_ಬಹುಲಕ = -1;
    char_u *p;

    // Only do something when redrawing the screen and we can restore the
    // mode.
    ಆದರೆ (!ತುಂಬಿದ_screen || *T_CEI == NUL)
    {
# ifdef FEAT_TERMRESPONSE
	ಆದರೆ (forced && initial_cursor_shape > 0)
	    // Restore to initial values.
	    ಪದ_cursor_shape(initial_cursor_shape, initial_cursor_blink);
# endif
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ ((State & ಬಹುಲಕ_REPLACE) == ಬಹುಲಕ_REPLACE)
    {
	ಆದರೆ (forced || showing_ಬಹುಲಕ != ಬಹುಲಕ_REPLACE)
	{
	    ಆದರೆ (*T_CSR != NUL)
		p = T_CSR;	// Replace mode cursor
	    ಇಲ್ಲದಿದ್ದರೆ
		p = T_CSI;	// fall back to Insert mode cursor
	    ಆದರೆ (*p != NUL)
	    {
		ಹೊರಹೋಗು_str(p);
		showing_ಬಹುಲಕ = ಬಹುಲಕ_REPLACE;
	    }
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (State & ಬಹುಲಕ_ಸೇರಿಸು)
    {
	ಆದರೆ ((forced || showing_ಬಹುಲಕ != ಬಹುಲಕ_ಸೇರಿಸು) && *T_CSI != NUL)
	{
	    ಹೊರಹೋಗು_str(T_CSI);	    // Insert mode cursor
	    showing_ಬಹುಲಕ = ಬಹುಲಕ_ಸೇರಿಸು;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (forced || showing_ಬಹುಲಕ != ಬಹುಲಕ_ಸಾಮಾನ್ಯ)
    {
	ಹೊರಹೋಗು_str(T_CEI);		    // non-Insert mode cursor
	showing_ಬಹುಲಕ = ಬಹುಲಕ_ಸಾಮಾನ್ಯ;
    }
}

# ಆದರೆ defined(FEAT_TERMINAL)
    ಶೂನ್ಯ
ಪದ_cursor_color(char_u *color)
{
    ಆದರೆ (*T_CSC == NUL)
	ಹಿಂತಿರುಗಿ;

    ಹೊರಹೋಗು_str(T_CSC);		// set cursor color start
    ಹೊರಹೋಗು_str_nf(color);
    ಹೊರಹೋಗು_str(T_CEC);		// set cursor color end
    ಹೊರಹೋಗು_flush();
}
# endif

    ಪೂರ್ಣಾಂಕ
blink_state_is_inverted(ಶೂನ್ಯ)
{
# ifdef FEAT_TERMRESPONSE
    ಹಿಂತಿರುಗಿ rbm_ಸ್ಥಿತಿ.tr_ಪ್ರಗತಿ == ಸ್ಥಿತಿ_GOT
	&& rcs_ಸ್ಥಿತಿ.tr_ಪ್ರಗತಿ == ಸ್ಥಿತಿ_GOT
		&& initial_cursor_blink != initial_cursor_shape_blink;
# ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ FALSE;
# endif
}

/*
 * "shape": 1 = block, 2 = underline, 3 = vertical bar
 */
    ಶೂನ್ಯ
ಪದ_cursor_shape(ಪೂರ್ಣಾಂಕ shape, ಪೂರ್ಣಾಂಕ blink)
{
    ಆದರೆ (*T_CSH != NUL)
    {
	ಹೊರಹೋಗು_STR(tgoto((ಅಕ್ಷರ *)T_CSH, 0, shape * 2 - blink));
	ಹೊರಹೋಗು_flush();
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಪೂರ್ಣಾಂಕ do_blink = blink;

	// t_SH is empty: try setting just the blink state.
	// The blink flags are XORed together, if the initial blinking from
	// style and shape differs, we need to invert the flag here.
	ಆದರೆ (blink_state_is_inverted())
	    do_blink = !blink;

	ಆದರೆ (do_blink && *T_VS != NUL)
	{
	    ಹೊರಹೋಗು_str(T_VS);
	    ಹೊರಹೋಗು_flush();
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!do_blink && *T_CVS != NUL)
	{
	    ಹೊರಹೋಗು_str(T_CVS);
	    ಹೊರಹೋಗು_flush();
	}
    }
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Set scrolling region for window 'wp'.
 * The region starts 'off' lines from the start of the window.
 * Also set the vertical scroll region for a vertically split window.  Always
 * the full width of the window, excluding the vertical separator.
 */
    ಶೂನ್ಯ
scroll_region_ಗಣ(win_T *wp, ಪೂರ್ಣಾಂಕ off)
{
    ಹೊರಹೋಗು_STR(tgoto((ಅಕ್ಷರ *)T_CS, W_WINROW(wp) + wp->w_ಎತ್ತರ - 1,
							 W_WINROW(wp) + off));
    ಆದರೆ (*T_CSV != NUL && wp->w_ಅಗಲ != Columns)
	ಹೊರಹೋಗು_STR(tgoto((ಅಕ್ಷರ *)T_CSV, wp->w_wincol + wp->w_ಅಗಲ - 1,
							       wp->w_wincol));
    screen_ಆರಂಭ();		    // don't know where cursor is now
}

/*
 * Reset scrolling region to the whole screen.
 */
    ಶೂನ್ಯ
scroll_region_reset(ಶೂನ್ಯ)
{
    ಹೊರಹೋಗು_STR(tgoto((ಅಕ್ಷರ *)T_CS, (ಪೂರ್ಣಾಂಕ)ಅಡ್ಡಸಾಲುಗಳು - 1, 0));
    ಆದರೆ (*T_CSV != NUL)
	ಹೊರಹೋಗು_STR(tgoto((ಅಕ್ಷರ *)T_CSV, (ಪೂರ್ಣಾಂಕ)Columns - 1, 0));
    screen_ಆರಂಭ();		    // don't know where cursor is now
}


/*
 * List of terminal codes that are currently recognized.
 */

ಸ್ಥಿರ ರಚನೆ termcode
{
    char_u  name[2];	    // termcap name of entry
    char_u  *ಸಂಕೇತ_2;	    // terminal code (in allocated memory)
    ಪೂರ್ಣಾಂಕ	    len;	    // STRLEN(code)
    ಪೂರ್ಣಾಂಕ	    modlen;	    // length of part before ";*~".
} *termcodes = ಖಾಲಿ;

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ  tc_max_len = 0; // number of entries that termcodes[] can hold
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ  tc_len = 0;	    // current number of entries in termcodes[]

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ termcode_ನಕ್ಷತ್ರ(char_u *ಸಂಕೇತ_2, ಪೂರ್ಣಾಂಕ len);

    ಶೂನ್ಯ
clear_termcodes(ಶೂನ್ಯ)
{
    ಆಗ (tc_len > 0)
	vim_free(termcodes[--tc_len].ಸಂಕೇತ_2);
    VIM_CLEAR(termcodes);
    tc_max_len = 0;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_TGETENT
    BC = (ಅಕ್ಷರ *)ಖಾಲಿಗಳು_option;
    ಮೇಲೆ_2 = (ಅಕ್ಷರ *)ಖಾಲಿಗಳು_option;
    PC = NUL;			// set pad character to NUL
    ospeed = 0;
#ಆದರೆ_ಅಂತ್ಯ

    need_gather = TRUE;		// need to fill termleader[]
}

#ವ್ಯಾಖ್ಯಾನಿಸು ATC_FROM_ಪದ 55

/*
 * For xterm we recognize special codes like "ESC[42;*X" and "ESC O*X" that
 * accept modifiers.
 * Set "termcodes[idx].modlen".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
adjust_modlen(ಪೂರ್ಣಾಂಕ idx)
{
    termcodes[idx].modlen = 0;
    ಪೂರ್ಣಾಂಕ j = termcode_ನಕ್ಷತ್ರ(termcodes[idx].ಸಂಕೇತ_2, termcodes[idx].len);
    ಆದರೆ (j <= 0)
	ಹಿಂತಿರುಗಿ;

    termcodes[idx].modlen = termcodes[idx].len - 1 - j;
    // For "CSI[@;X" the "@" is not included in "modlen".
    ಆದರೆ (termcodes[idx].ಸಂಕೇತ_2[termcodes[idx].modlen - 1] == '@')
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
ಕೂಡಿಸು_termcode(char_u *name, char_u *string, ಪೂರ್ಣಾಂಕ flags)
{
    ರಚನೆ termcode *new_tc;
    ಪೂರ್ಣಾಂಕ		    i, j;
    char_u	    *s;
    ಪೂರ್ಣಾಂಕ		    len;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಅಕ್ಷರ	    *action = "Setting";
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (string == ಖಾಲಿ || *string == NUL)
    {
	del_termcode(name);
	ಹಿಂತಿರುಗಿ;
    }

#ಆದರೆ defined(MSWIN) && !defined(FEAT_GUI)
    s = vim_strnsave(string, STRLEN(string) + 1);
#ಇಲ್ಲದಿದ್ದರೆ
# ifdef VIMDLL
    ಆದರೆ (!gui.ಒಳ_use)
	s = vim_strnsave(string, STRLEN(string) + 1);
    ಇಲ್ಲದಿದ್ದರೆ
# endif
	s = vim_strsave(string);
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (s == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    // Change leading <Esc>[ to CSI, change <Esc>O to <M-O>.
    ಆದರೆ (flags != 0 && flags != ATC_FROM_ಪದ && ಪದ_7to8bit(string) != 0)
    {
	STRMOVE(s, s + 1);
	s[0] = ಪದ_7to8bit(string);
    }

#ಆದರೆ defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
# ifdef VIMDLL
    ಆದರೆ (!gui.ಒಳ_use)
# endif
    {
	ಆದರೆ (s[0] == K_NUL)
	{
	    STRMOVE(s + 1, s);
	    s[1] = 3;
	}
    }
#ಆದರೆ_ಅಂತ್ಯ

    len = (ಪೂರ್ಣಾಂಕ)STRLEN(s);

    need_gather = TRUE;		// need to fill termleader[]

    /*
     * need to make space for more entries
     */
    ಆದರೆ (tc_len == tc_max_len)
    {
	tc_max_len += 20;
	new_tc = ALLOC_MULT(ರಚನೆ termcode, tc_max_len);
	ಆದರೆ (new_tc == ಖಾಲಿ)
	{
	    tc_max_len -= 20;
	    vim_free(s);
	    ಹಿಂತಿರುಗಿ;
	}
	ಗಾಗಿ (i = 0; i < tc_len; ++i)
	    new_tc[i] = termcodes[i];
	vim_free(termcodes);
	termcodes = new_tc;
    }

    /*
     * Look for existing entry with the same name, it is replaced.
     * Look for an existing entry that is alphabetical higher, the new entry
     * is inserted in front of it.
     */
    ಗಾಗಿ (i = 0; i < tc_len; ++i)
    {
	ಆದರೆ (termcodes[i].name[0] < name[0])
	    ಮುಂದುವರಿ;
	ಆದರೆ (termcodes[i].name[0] == name[0])
	{
	    ಆದರೆ (termcodes[i].name[1] < name[1])
		ಮುಂದುವರಿ;
	    /*
	     * Exact match: May replace old code.
	     */
	    ಆದರೆ (termcodes[i].name[1] == name[1])
	    {
		ಆದರೆ (flags == ATC_FROM_ಪದ && (j = termcode_ನಕ್ಷತ್ರ(
				    termcodes[i].ಸಂಕೇತ_2, termcodes[i].len)) > 0)
		{
		    // Don't replace ESC[123;*X or ESC O*X with another when
		    // invoked from got_code_from_term().
		    ಆದರೆ (len == termcodes[i].len - j
			    && STRNCMP(s, termcodes[i].ಸಂಕೇತ_2, len - 1) == 0
			    && s[len - 1]
				   == termcodes[i].ಸಂಕೇತ_2[termcodes[i].len - 1])
		    {
			// They are equal but for the ";*": don't add it.
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
			ch_log(ಖಾಲಿ, "Termcap entry %c%c did not change",
							     name[0], name[1]);
#ಆದರೆ_ಅಂತ್ಯ
			vim_free(s);
			ಹಿಂತಿರುಗಿ;
		    }
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    // Replace old code.
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
		    ch_log(ಖಾಲಿ, "Termcap entry %c%c was: %s",
					  name[0], name[1], termcodes[i].ಸಂಕೇತ_2);
#ಆದರೆ_ಅಂತ್ಯ
		    vim_free(termcodes[i].ಸಂಕೇತ_2);
		    --tc_len;
		    ನಿಲ್ಲಿಸು;
		}
	    }
	}
	/*
	 * Found alphabetical larger entry, move rest to insert new entry
	 */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	action = "Adding";
#ಆದರೆ_ಅಂತ್ಯ
	ಗಾಗಿ (j = tc_len; j > i; --j)
	    termcodes[j] = termcodes[j - 1];
	ನಿಲ್ಲಿಸು;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ch_log(ಖಾಲಿ, "%s termcap entry %c%c to %s", action, name[0], name[1], s);
#ಆದರೆ_ಅಂತ್ಯ
    termcodes[i].name[0] = name[0];
    termcodes[i].name[1] = name[1];
    termcodes[i].ಸಂಕೇತ_2 = s;
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
    CLEAR_ಕ್ಷೇತ್ರ(regmatch);
    regmatch.rm_ic = TRUE;
    regmatch.regprog = vim_regcomp((char_u *)"^\033[\\d\\+\\~$", RE_MAGIC);

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < tc_len; ++i)
    {
	ಆದರೆ (regmatch.regprog == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ;

	// skip PasteStart and PasteEnd
	ಆದರೆ (termcodes[i].name[0] == 'P'
		&& (termcodes[i].name[1] == 'S' || termcodes[i].name[1] == 'E'))
	    ಮುಂದುವರಿ;

	char_u *s = termcodes[i].ಸಂಕೇತ_2;
	ಆದರೆ (s != ಖಾಲಿ && vim_regexec(&regmatch, s, (colnr_T)0))
	{
	    size_t len = STRLEN(s);
	    char_u *ns = alloc(len + 3);
	    ಆದರೆ (ns != ಖಾಲಿ)
	    {
		mch_memmove(ns, s, len - 1);
		mch_memmove(ns + len - 1, ";*~", 4);
		vim_free(s);
		termcodes[i].ಸಂಕೇತ_2 = ns;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
termcode_ನಕ್ಷತ್ರ(char_u *ಸಂಕೇತ_2, ಪೂರ್ಣಾಂಕ len)
{
    // Shortest is <M-O>*X.  With ; shortest is <CSI>@;*X
    ಆದರೆ (len >= 3 && ಸಂಕೇತ_2[len - 2] == '*')
    {
	ಆದರೆ (len >= 5 && ಸಂಕೇತ_2[len - 3] == ';')
	    ಹಿಂತಿರುಗಿ 2;
	ಇಲ್ಲದಿದ್ದರೆ
	    ಹಿಂತಿರುಗಿ 1;
    }
    ಹಿಂತಿರುಗಿ 0;
}

    char_u  *
ಕಂಡುಹಿಡಿ_termcode(char_u *name)
{
    ಪೂರ್ಣಾಂಕ	    i;

    ಗಾಗಿ (i = 0; i < tc_len; ++i)
	ಆದರೆ (termcodes[i].name[0] == name[0] && termcodes[i].name[1] == name[1])
	    ಹಿಂತಿರುಗಿ termcodes[i].ಸಂಕೇತ_2;
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

    char_u *
get_termcode(ಪೂರ್ಣಾಂಕ i)
{
    ಆದರೆ (i >= tc_len)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಹಿಂತಿರುಗಿ &termcodes[i].name[0];
}

/*
 * Returns the length of the terminal code at index 'idx'.
 */
    ಪೂರ್ಣಾಂಕ
get_termcode_len(ಪೂರ್ಣಾಂಕ idx)
{
    ಹಿಂತಿರುಗಿ termcodes[idx].len;
}

    ಶೂನ್ಯ
del_termcode(char_u *name)
{
    ಪೂರ್ಣಾಂಕ	    i;

    ಆದರೆ (termcodes == ಖಾಲಿ)	// nothing there yet
	ಹಿಂತಿರುಗಿ;

    need_gather = TRUE;		// need to fill termleader[]

    ಗಾಗಿ (i = 0; i < tc_len; ++i)
	ಆದರೆ (termcodes[i].name[0] == name[0] && termcodes[i].name[1] == name[1])
	{
	    del_termcode_idx(i);
	    ಹಿಂತಿರುಗಿ;
	}
    // not found. Give error message?
}

    ಸ್ಥಿರ ಶೂನ್ಯ
del_termcode_idx(ಪೂರ್ಣಾಂಕ idx)
{
    ಪೂರ್ಣಾಂಕ		i;

    vim_free(termcodes[idx].ಸಂಕೇತ_2);
    --tc_len;
    ಗಾಗಿ (i = idx; i < tc_len; ++i)
	termcodes[i] = termcodes[i + 1];
}

/*
 * Called when detected that the terminal sends 8-bit codes.
 * Convert all 7-bit codes to their 8-bit equivalent.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
switch_ಗಮ್ಯ_8bit(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		c;

    // Only need to do something when not already using 8-bit codes.
    ಆದರೆ (!ಪದ_is_8bit(T_NAME))
    {
	ಗಾಗಿ (i = 0; i < tc_len; ++i)
	{
	    c = ಪದ_7to8bit(termcodes[i].ಸಂಕೇತ_2);
	    ಆದರೆ (c != 0)
	    {
		STRMOVE(termcodes[i].ಸಂಕೇತ_2 + 1, termcodes[i].ಸಂಕೇತ_2 + 2);
		termcodes[i].ಸಂಕೇತ_2[0] = c;
	    }
	}
	need_gather = TRUE;		// need to fill termleader[]
    }
    detected_8bit = TRUE;
    LOG_TR1("Switching to 8 bit");
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_DOUBLE_CLICK
ಸ್ಥಿರ linenr_T orig_topline = 0;
# ifdef FEAT_DIFF
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ orig_topfill = 0;
# endif
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(CHECK_DOUBLE_CLICK)
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
ಗಣ_mouse_topline(win_T *wp)
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
    ಪೂರ್ಣಾಂಕ
is_mouse_topline(win_T *wp)
{
    ಹಿಂತಿರುಗಿ orig_topline == wp->w_topline
# ifdef FEAT_DIFF
	&& orig_topfill == wp->w_topfill
# endif
	;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * If "buf" is NULL put "string[new_slen]" in typebuf; "buflen" is not used.
 * If "buf" is not NULL put "string[new_slen]" in "buf[bufsize]" and adjust
 * "buflen".
 * Remove "slen" bytes.
 * Returns FAIL for error.
 */
    ಪೂರ್ಣಾಂಕ
put_string_ಒಳ_typebuf(
	ಪೂರ್ಣಾಂಕ	ಸ್ಥಾನಾಂತರ,
	ಪೂರ್ಣಾಂಕ	slen,
	char_u	*string,
	ಪೂರ್ಣಾಂಕ	new_slen,
	char_u	*buf,
	ಪೂರ್ಣಾಂಕ	bufsize,
	ಪೂರ್ಣಾಂಕ	*buflen)
{
    ಪೂರ್ಣಾಂಕ		extra = new_slen - slen;

    string[new_slen] = NUL;
    ಆದರೆ (buf == ಖಾಲಿ)
    {
	ಆದರೆ (extra < 0)
	    // remove matched chars, taking care of noremap
	    del_typebuf(-extra, ಸ್ಥಾನಾಂತರ);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (extra > 0)
	    // insert the extra space we need
	    ಆದರೆ (ins_typebuf(string + slen, REMAP_YES, ಸ್ಥಾನಾಂತರ, FALSE, FALSE)
								       == FAIL)
		ಹಿಂತಿರುಗಿ FAIL;

	// Careful: del_typebuf() and ins_typebuf() may have reallocated
	// typebuf.tb_buf[]!
	mch_memmove(typebuf.tb_buf + typebuf.tb_off + ಸ್ಥಾನಾಂತರ, string,
							     (size_t)new_slen);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (extra < 0)
	    // remove matched characters
	    mch_memmove(buf + ಸ್ಥಾನಾಂತರ, buf + ಸ್ಥಾನಾಂತರ - extra,
					   (size_t)(*buflen + ಸ್ಥಾನಾಂತರ + extra));
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (extra > 0)
	{
	    // Insert the extra space we need.  If there is insufficient
	    // space return -1.
	    ಆದರೆ (*buflen + extra + new_slen >= bufsize)
		ಹಿಂತಿರುಗಿ FAIL;
	    mch_memmove(buf + ಸ್ಥಾನಾಂತರ + extra, buf + ಸ್ಥಾನಾಂತರ,
						   (size_t)(*buflen - ಸ್ಥಾನಾಂತರ));
	}
	mch_memmove(buf + ಸ್ಥಾನಾಂತರ, string, (size_t)new_slen);
	*buflen = *buflen + extra + new_slen;
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Decode a modifier number as xterm provides it into MOD_MASK bits.
 */
    ಪೂರ್ಣಾಂಕ
decode_modifiers(ಪೂರ್ಣಾಂಕ n)
{
    ಪೂರ್ಣಾಂಕ	    ಸಂಕೇತ_2 = n - 1;
    ಪೂರ್ಣಾಂಕ	    modifiers = 0;

    ಆದರೆ (ಸಂಕೇತ_2 & 1)
	modifiers |= MOD_ಮುಖವಾಡ_ಸರಿಸು;
    ಆದರೆ (ಸಂಕೇತ_2 & 2)
	modifiers |= MOD_ಮುಖವಾಡ_ALT;
    ಆದರೆ (ಸಂಕೇತ_2 & 4)
	modifiers |= MOD_ಮುಖವಾಡ_CTRL;
    ಆದರೆ (ಸಂಕೇತ_2 & 8)
	modifiers |= MOD_ಮುಖವಾಡ_META;
    // Any further modifiers are silently dropped.

    ಹಿಂತಿರುಗಿ modifiers;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
modifiers2keycode(ಪೂರ್ಣಾಂಕ modifiers, ಪೂರ್ಣಾಂಕ *ಕೀಲಿ, char_u *string)
{
    ಪೂರ್ಣಾಂಕ new_slen = 0;

    ಆದರೆ (modifiers == 0)
	ಹಿಂತಿರುಗಿ 0;

    // Some keys have the modifier included.  Need to handle that here to
    // make mappings work.  This may result in a special key, such as
    // K_S_TAB.
    *ಕೀಲಿ = simplify_ಕೀಲಿ(*ಕೀಲಿ, &modifiers);
    ಆದರೆ (modifiers != 0)
    {
	string[new_slen++] = K_SPECIAL;
	string[new_slen++] = (ಪೂರ್ಣಾಂಕ)KS_MODIFIER;
	string[new_slen++] = modifiers;
    }
    ಹಿಂತಿರುಗಿ new_slen;
}

/*
 * Handle a cursor position report.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
handle_u7_response(ಪೂರ್ಣಾಂಕ *arg, char_u *tp ಬಳಸದ, ಪೂರ್ಣಾಂಕ csi_len ಬಳಸದ)
{
    ಆದರೆ (arg[0] == 2 && arg[1] >= 2)
    {
	ಅಕ್ಷರ *aw = ಖಾಲಿ;

	LOG_TRN("Received U7 status: %s", tp);
	u7_ಸ್ಥಿತಿ.tr_ಪ್ರಗತಿ = ಸ್ಥಿತಿ_GOT;
	did_cursorhold = TRUE;
	ಆದರೆ (arg[1] == 2)
	    aw = "single";
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (arg[1] == 3)
	    aw = "double";
	ಆದರೆ (aw != ಖಾಲಿ && STRCMP(aw, p_ambw) != 0)
	{
	    // Setting the option causes a screen redraw. Do
	    // that right away if possible, keeping any
	    // messages.
	    ಗಣ_option_ಮೌಲ್ಯ_give_err((char_u *)"ambw", 0L, (char_u *)aw, 0);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG_TERMRESPONSE
	    {
		ಪೂರ್ಣಾಂಕ r = redraw_asap(UPD_CLEAR);

		LOG_TRN("set 'ambiwidth', redraw_asap(): %d", r);
	    }
#ಇಲ್ಲದಿದ್ದರೆ
	    redraw_asap(UPD_CLEAR);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	    ಗಣ_vim_var_string(VV_TERMU7RESP, tp, csi_len);
#ಆದರೆ_ಅಂತ್ಯ
	    apply_autocmds(EVENT_TERMRESPONSEALL,
					(char_u *)"ambiguouswidth", ಖಾಲಿ, FALSE, curbuf);
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (arg[0] == 3)
    {
	ಪೂರ್ಣಾಂಕ ಮೌಲ್ಯ;

	LOG_TRN("Received compatibility test result: %s", tp);
	xcc_ಸ್ಥಿತಿ.tr_ಪ್ರಗತಿ = ಸ್ಥಿತಿ_GOT;

	// Third row: xterm compatibility test.
	// If the cursor is on the first column then the terminal can handle
	// the request for cursor style and blinking.
	ಮೌಲ್ಯ = arg[1] == 1 ? TPR_YES : TPR_NO;
	ಪದ_props[TPR_CURSOR_STYLE].tpr_ಸ್ಥಿತಿ = ಮೌಲ್ಯ;
	ಪದ_props[TPR_CURSOR_BLINK].tpr_ಸ್ಥಿತಿ = ಮೌಲ್ಯ;
    }
}



/*
 * Handle a response to T_CRV: {lead}{first}{x};{vers};{y}c
 * Xterm and alike use '>' for {first}.
 * Rxvt sends "{lead}?1;2c".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
handle_ಆವೃತ್ತಿ_response(ಪೂರ್ಣಾಂಕ ಮೊದಲ, ಪೂರ್ಣಾಂಕ *arg, ಪೂರ್ಣಾಂಕ argc, char_u *tp)
{
    // The xterm version.  It is set to zero when it can't be an actual xterm
    // version.
    ಪೂರ್ಣಾಂಕ ಆವೃತ್ತಿ_2 = arg[1];

    LOG_TRN("Received CRV response: %s", tp);
    crv_ಸ್ಥಿತಿ.tr_ಪ್ರಗತಿ = ಸ್ಥಿತಿ_GOT;
    did_cursorhold = TRUE;

    // Reset terminal properties that are set based on the termresponse.
    // Mainly useful for tests that send the termresponse multiple times.
    // For testing all props can be reset.
    init_ಪದ_props(
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	    reset_ಪದ_props_on_termresponse
#ಇಲ್ಲದಿದ್ದರೆ
	    FALSE
#ಆದರೆ_ಅಂತ್ಯ
	    );

    // If this code starts with CSI, you can bet that the
    // terminal uses 8-bit codes.
    ಆದರೆ (tp[0] == CSI)
	switch_ಗಮ್ಯ_8bit();

    // Screen sends 40500.
    // rxvt sends its version number: "20703" is 2.7.3.
    // Ignore it for when the user has set 'term' to xterm,
    // even though it's an rxvt.
    ಆದರೆ (ಆವೃತ್ತಿ_2 > 20000)
	ಆವೃತ್ತಿ_2 = 0;

    // Figure out more if the response is CSI > 99 ; 99 ; 99 c
    ಆದರೆ (ಮೊದಲ == '>' && argc == 3)
    {
	// mintty 2.9.5 sends 77;20905;0c.
	// (77 is ASCII 'M' for mintty.)
	ಆದರೆ (arg[0] == 77)
	{
	    // mintty can do SGR mouse reporting
	    ಪದ_props[TPR_MOUSE].tpr_ಸ್ಥಿತಿ = TPR_MOUSE_SGR;
	}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMRESPONSE
	// If xterm version >= 141 try to get termcap codes.  For other
	// terminals the request should be ignored.
	ಆದರೆ (ಆವೃತ್ತಿ_2 >= 141 && p_xtermcodes)
	{
	    LOG_TR1("Enable checking for XT codes");
	    check_for_codes = TRUE;
	    need_gather = TRUE;
	    req_codes_from_ಪದ();
	}
#ಆದರೆ_ಅಂತ್ಯ

	// libvterm sends 0;100;0
	// Konsole sends 0;115;0 and works the same way
	ಆದರೆ ((ಆವೃತ್ತಿ_2 == 100 || ಆವೃತ್ತಿ_2 == 115) && arg[0] == 0 && arg[2] == 0)
	{
	    // If run from Vim $COLORS is set to the number of
	    // colors the terminal supports.  Otherwise assume
	    // 256, libvterm supports even more.
	    ಆದರೆ (mch_getenv((char_u *)"COLORS") == ಖಾಲಿ)
		may_adjust_color_ಎಣಿಕೆ(256);
	    // Libvterm can handle SGR mouse reporting.
	    ಪದ_props[TPR_MOUSE].tpr_ಸ್ಥಿತಿ = TPR_MOUSE_SGR;
	}

	ಆದರೆ (ಆವೃತ್ತಿ_2 == 95)
	{
	    // Mac Terminal.app sends 1;95;0
	    //
	    // Terminal.app doesn't seem to handle DECRQM sequences
	    // properly, see issue #19852.
	    ಆದರೆ (arg[0] == 1 && arg[2] == 0)
	    {
		ಪದ_props[TPR_UNDERLINE_RGB].tpr_ಸ್ಥಿತಿ = TPR_YES;
		ಪದ_props[TPR_MOUSE].tpr_ಸ್ಥಿತಿ = TPR_MOUSE_SGR;
		ಪದ_props[TPR_DECRQM].tpr_ಸ್ಥಿತಿ = TPR_NO;
	    }
	    // iTerm2 sends 0;95;0
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (arg[0] == 0 && arg[2] == 0)
	    {
		// iTerm2 can do SGR mouse reporting
		ಪದ_props[TPR_MOUSE].tpr_ಸ್ಥಿತಿ = TPR_MOUSE_SGR;
		ಪದ_props[TPR_DECRQM].tpr_ಸ್ಥಿತಿ = TPR_YES;
	    }
	    // old iTerm2 sends 0;95;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (arg[0] == 0 && arg[2] == -1)
		ಪದ_props[TPR_UNDERLINE_RGB].tpr_ಸ್ಥಿತಿ = TPR_YES;
	}

	// screen sends 83;40500;0 83 is 'S' in ASCII.
	ಆದರೆ (arg[0] == 83)
	{
	    // screen supports SGR mouse codes since 4.7.0
	    ಆದರೆ (arg[1] >= 40700)
		ಪದ_props[TPR_MOUSE].tpr_ಸ್ಥಿತಿ = TPR_MOUSE_SGR;
	    ಇಲ್ಲದಿದ್ದರೆ
		ಪದ_props[TPR_MOUSE].tpr_ಸ್ಥಿತಿ = TPR_MOUSE_XTERM;
	}

	// If no recognized terminal has set mouse behavior, assume xterm.
	ಆದರೆ (ಪದ_props[TPR_MOUSE].tpr_ಸ್ಥಿತಿ == TPR_UNKNOWN)
	{
	    // Xterm version 277 supports SGR.
	    // Xterm version >= 95 supports mouse dragging.
	    ಆದರೆ (ಆವೃತ್ತಿ_2 >= 277)
		ಪದ_props[TPR_MOUSE].tpr_ಸ್ಥಿತಿ = TPR_MOUSE_SGR;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಆವೃತ್ತಿ_2 >= 95)
		ಪದ_props[TPR_MOUSE].tpr_ಸ್ಥಿತಿ = TPR_MOUSE_XTERM2;
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
	ಆದರೆ (arg[1] >= 2500)
	{
	    ಪದ_props[TPR_UNDERLINE_RGB].tpr_ಸ್ಥಿತಿ = TPR_YES;
	}

	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಆವೃತ್ತಿ_2 == 136 && arg[2] == 0)
	{
	    ಪದ_props[TPR_UNDERLINE_RGB].tpr_ಸ್ಥಿತಿ = TPR_YES;

	    // PuTTY sends 0;136;0
	    ಆದರೆ (arg[0] == 0)
	    {
		// supports sgr-like mouse reporting.
		ಪದ_props[TPR_MOUSE].tpr_ಸ್ಥಿತಿ = TPR_MOUSE_SGR;
	    }
	    // vandyke SecureCRT sends 1;136;0
	}

	// Konsole sends 0;115;0 - but t_u8 does not actually work, therefore
	// commented out.
	// else if (version == 115 && arg[0] == 0 && arg[2] == 0)
	//     term_props[TPR_UNDERLINE_RGB].tpr_status = TPR_YES;

	// Kitty up to 9.x sends 1;400{version};{secondary-version}
	ಆದರೆ (arg[0] == 1 && arg[1] >= 4000 && arg[1] <= 4009)
	{
	    ಪದ_props[TPR_KITTY].tpr_ಸ್ಥಿತಿ = TPR_YES;
	    ಪದ_props[TPR_KITTY].tpr_ಗಣ_by_termresponse = TRUE;

	    // Kitty can handle SGR mouse reporting.
	    ಪದ_props[TPR_MOUSE].tpr_ಸ್ಥಿತಿ = TPR_MOUSE_SGR;
	    ಪದ_props[TPR_DECRQM].tpr_ಸ್ಥಿತಿ = TPR_YES;
	}

	// foot terminal sends 1;12700;0
	ಆದರೆ (arg[0] == 1 && ಆವೃತ್ತಿ_2 == 12700 && arg[2] == 0)
	{
	    ಪದ_props[TPR_MOUSE].tpr_ಸ್ಥಿತಿ = TPR_MOUSE_SGR;
	    ಪದ_props[TPR_DECRQM].tpr_ಸ್ಥಿತಿ = TPR_YES;
	}

	// GNU screen sends 83;30600;0, 83;40500;0, etc.
	// 30600/40500 is a version number of GNU screen. DA2 support is added
	// on 3.6.  DCS string has a special meaning to GNU screen, but xterm
	// compatibility checking does not detect GNU screen.
	ಆದರೆ (arg[0] == 83 && arg[1] >= 30600)
	{
	    ಪದ_props[TPR_CURSOR_STYLE].tpr_ಸ್ಥಿತಿ = TPR_NO;
	    ಪದ_props[TPR_CURSOR_BLINK].tpr_ಸ್ಥಿತಿ = TPR_NO;
	    ಪದ_props[TPR_DECRQM].tpr_ಸ್ಥಿತಿ = TPR_NO; // screen doesn't seem
							// to handle DECRQM
							// sequences
	}

	// Xterm first responded to this request at patch level
	// 95, so assume anything below 95 is not xterm and hopefully supports
	// the underline RGB color sequence.
	ಆದರೆ (ಆವೃತ್ತಿ_2 < 95)
	    ಪದ_props[TPR_UNDERLINE_RGB].tpr_ಸ್ಥಿತಿ = TPR_YES;

	// Getting the cursor style is only supported properly by xterm since
	// version 279 (otherwise it returns 0x18).
	ಆದರೆ (ಆವೃತ್ತಿ_2 < 279)
	    ಪದ_props[TPR_CURSOR_STYLE].tpr_ಸ್ಥಿತಿ = TPR_NO;

	/*
	 * Take action on the detected properties.
	 */

	// Unless the underline RGB color is expected to work, disable "t_8u".
	// It does not work for the real Xterm, it resets the background color.
	// This may cause some flicker.  Alternative would be to set "t_8u"
	// here if the terminal is expected to support it, but that might
	// conflict with what was set in the .vimrc.
	ಆದರೆ (ಪದ_props[TPR_UNDERLINE_RGB].tpr_ಸ್ಥಿತಿ != TPR_YES
			&& *T_8U != NUL
			&& !option_was_ಗಣ((char_u *)"t_8u"))
	{
	    ಗಣ_string_option_ನೇರ((char_u *)"t_8u", -1, (char_u *)"",
								  OPT_FREE, 0);
	}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMRESPONSE
	ಆದರೆ (*T_8U != NUL && write_t_8u_state == MAYBE)
	    // Did skip writing t_8u, a complete redraw is needed.
	    redraw_later_clear();
	write_t_8u_state = OK;  // can output t_8u now
#ಆದರೆ_ಅಂತ್ಯ

	// Only set 'ttymouse' automatically if it was not set
	// by the user already.
	ಆದರೆ (!option_was_ಗಣ((char_u *)"ttym")
		&& (ಪದ_props[TPR_MOUSE].tpr_ಸ್ಥಿತಿ == TPR_MOUSE_XTERM2
		    || ಪದ_props[TPR_MOUSE].tpr_ಸ್ಥಿತಿ == TPR_MOUSE_SGR))
	{
	    ಗಣ_option_ಮೌಲ್ಯ_give_err((char_u *)"ttym", 0L,
		    ಪದ_props[TPR_MOUSE].tpr_ಸ್ಥಿತಿ == TPR_MOUSE_SGR
				    ? (char_u *)"sgr" : (char_u *)"xterm2", 0);
	}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMRESPONSE
	ಪೂರ್ಣಾಂಕ need_flush = FALSE;

	// Only request the cursor style if t_SH and t_RS are
	// set. Only supported properly by xterm since version
	// 279 (otherwise it returns 0x18).
	// Only when getting the cursor style was detected to work.
	// Not for Terminal.app, it can't handle t_RS, it
	// echoes the characters to the screen.
	ಆದರೆ (rcs_ಸ್ಥಿತಿ.tr_ಪ್ರಗತಿ == ಸ್ಥಿತಿ_GET
		&& ಪದ_props[TPR_CURSOR_STYLE].tpr_ಸ್ಥಿತಿ == TPR_YES
		&& *T_CSH != NUL
		&& *T_CRS != NUL)
	{
	    MAY_WANT_ಗಮ್ಯ_LOG_THIS;
	    LOG_TR1("Sending cursor style request");
	    ಹೊರಹೋಗು_str(T_CRS);
	    termrequest_ಕಳುಹಿಸಿದ(&rcs_ಸ್ಥಿತಿ);
	    need_flush = TRUE;
	}

	// Only request the cursor blink mode if t_RC set. Not
	// for Gnome terminal, it can't handle t_RC, it
	// echoes the characters to the screen.
	// Only when getting the cursor style was detected to work.
	ಆದರೆ (rbm_ಸ್ಥಿತಿ.tr_ಪ್ರಗತಿ == ಸ್ಥಿತಿ_GET
		&& ಪದ_props[TPR_CURSOR_BLINK].tpr_ಸ್ಥಿತಿ == TPR_YES
		&& *T_ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ != NUL)
	{
	    MAY_WANT_ಗಮ್ಯ_LOG_THIS;
	    LOG_TR1("Sending cursor blink mode request");
	    ಹೊರಹೋಗು_str(T_ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ);
	    termrequest_ಕಳುಹಿಸಿದ(&rbm_ಸ್ಥಿತಿ);
	    need_flush = TRUE;
	}

	// Only request DEC modes via DECRQM when the terminal is known to
	// handle it.  Not for Apple Terminal.app or GNU screen, they echo
	// the trailing "p" to the screen.  See issue #19852.
	ಆದರೆ (decrqm_ಸ್ಥಿತಿ.tr_ಪ್ರಗತಿ == ಸ್ಥಿತಿ_GET
		&& ಪದ_props[TPR_DECRQM].tpr_ಸ್ಥಿತಿ == TPR_YES)
	{
	    MAY_WANT_ಗಮ್ಯ_LOG_THIS;
	    LOG_TR1("Sending DECRQM requests");
	    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < (ಪೂರ್ಣಾಂಕ)ARRAY_ಉದ್ದಳತೆ(dec_modes); i++)
	    {
		vim_snprintf((ಅಕ್ಷರ *)IObuff, IOSIZE, "\033[?%d$p", dec_modes[i]);
		ಹೊರಹೋಗು_str(IObuff);
	    }
	    termrequest_ಕಳುಹಿಸಿದ(&decrqm_ಸ್ಥಿತಿ);
	    need_flush = TRUE;

	}

	ಆದರೆ (need_flush)
	    ಹೊರಹೋಗು_flush();
#ಆದರೆ_ಅಂತ್ಯ
    }
}

/*
 * Add "key" to "buf" and return the number of bytes used.
 * Handles special keys and multi-byte characters.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಕೂಡಿಸು_ಕೀಲಿ_ಗಮ್ಯ_buf(ಪೂರ್ಣಾಂಕ ಕೀಲಿ, char_u *buf)
{
    ಪೂರ್ಣಾಂಕ idx = 0;

    ಆದರೆ (IS_SPECIAL(ಕೀಲಿ))
    {
	buf[idx++] = K_SPECIAL;
	buf[idx++] = KEY2TERMCAP0(ಕೀಲಿ);
	buf[idx++] = KEY2TERMCAP1(ಕೀಲಿ);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (has_mbyte)
	idx += (*mb_char2bytes)(ಕೀಲಿ, buf + idx);
    ಇಲ್ಲದಿದ್ದರೆ
	buf[idx++] = ಕೀಲಿ;
    ಹಿಂತಿರುಗಿ idx;
}

/*
 * Shared between handle_key_with_modifier() and handle_csi_function_key().
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
put_ಕೀಲಿ_modifiers_ಒಳ_typebuf(
	ಪೂರ್ಣಾಂಕ	ಕೀಲಿ_arg,
	ಪೂರ್ಣಾಂಕ	modifiers_arg,
	ಪೂರ್ಣಾಂಕ	csi_len,
	ಪೂರ್ಣಾಂಕ	ಸ್ಥಾನಾಂತರ,
	char_u	*buf,
	ಪೂರ್ಣಾಂಕ	bufsize,
	ಪೂರ್ಣಾಂಕ	*buflen)
{
    ಪೂರ್ಣಾಂಕ ಕೀಲಿ = ಕೀಲಿ_arg;
    ಪೂರ್ಣಾಂಕ modifiers = modifiers_arg;

    // Some keys need adjustment when the Ctrl modifier is used.
    ಕೀಲಿ = may_adjust_ಕೀಲಿ_for_ctrl(modifiers, ಕೀಲಿ);

    // May remove the shift modifier if it's already included in the key.
    modifiers = may_remove_ಸರಿಸು_modifier(modifiers, ಕೀಲಿ);

    // Produce modifiers with K_SPECIAL KS_MODIFIER {mod}
    // worst-case: 3-byte modifier + 4 byte multi-char key + NUL
    char_u string[MAX_ಕೀಲಿ_ಸಂಕೇತ_LEN + 2];
    ಪೂರ್ಣಾಂಕ new_slen = modifiers2keycode(modifiers, &ಕೀಲಿ, string);

    // reject overlong key that would overflow string
    ಆದರೆ (ಕೀಲಿ > 0x10FFFF)
	ಹಿಂತಿರುಗಿ -1;

    // Add the bytes for the key.
    new_slen += ಕೂಡಿಸು_ಕೀಲಿ_ಗಮ್ಯ_buf(ಕೀಲಿ, string + new_slen);

    string[new_slen] = NUL;
    ಆದರೆ (put_string_ಒಳ_typebuf(ಸ್ಥಾನಾಂತರ, csi_len, string, new_slen,
						 buf, bufsize, buflen) == FAIL)
	ಹಿಂತಿರುಗಿ -1;
    ಹಿಂತಿರುಗಿ new_slen - csi_len + ಸ್ಥಾನಾಂತರ;
}

/*
 * Parse the number from a CSI numbered sequence for an F1-F12 key:
 *	ESC [ {number} ~
 * Returns the key
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
parse_csi_f_ಕೀಲಿಗಳು(ಪೂರ್ಣಾಂಕ arg)
{
    char_u ಕೀಲಿ_name[2] = { 0, 0 };

    ಆಯ್ಕೆ (arg)
    {
	ಸ್ಥಿತಿ 11: ಕೀಲಿ_name[0] = 'k'; ಕೀಲಿ_name[1] = '1'; ನಿಲ್ಲಿಸು;  // K_F1
	ಸ್ಥಿತಿ 12: ಕೀಲಿ_name[0] = 'k'; ಕೀಲಿ_name[1] = '2'; ನಿಲ್ಲಿಸು;  // K_F2
	ಸ್ಥಿತಿ 13: ಕೀಲಿ_name[0] = 'k'; ಕೀಲಿ_name[1] = '3'; ನಿಲ್ಲಿಸು;  // K_F3
	ಸ್ಥಿತಿ 14: ಕೀಲಿ_name[0] = 'k'; ಕೀಲಿ_name[1] = '4'; ನಿಲ್ಲಿಸು;  // K_F4
	ಸ್ಥಿತಿ 15: ಕೀಲಿ_name[0] = 'k'; ಕೀಲಿ_name[1] = '5'; ನಿಲ್ಲಿಸು;  // K_F5
	ಸ್ಥಿತಿ 17: ಕೀಲಿ_name[0] = 'k'; ಕೀಲಿ_name[1] = '6'; ನಿಲ್ಲಿಸು;  // K_F6
	ಸ್ಥಿತಿ 18: ಕೀಲಿ_name[0] = 'k'; ಕೀಲಿ_name[1] = '7'; ನಿಲ್ಲಿಸು;  // K_F7
	ಸ್ಥಿತಿ 19: ಕೀಲಿ_name[0] = 'k'; ಕೀಲಿ_name[1] = '8'; ನಿಲ್ಲಿಸು;  // K_F8
	ಸ್ಥಿತಿ 20: ಕೀಲಿ_name[0] = 'k'; ಕೀಲಿ_name[1] = '9'; ನಿಲ್ಲಿಸು;  // K_F9
	ಸ್ಥಿತಿ 21: ಕೀಲಿ_name[0] = 'F'; ಕೀಲಿ_name[1] = ';'; ನಿಲ್ಲಿಸು;  // K_F10
	ಸ್ಥಿತಿ 23: ಕೀಲಿ_name[0] = 'F'; ಕೀಲಿ_name[1] = '1'; ನಿಲ್ಲಿಸು;  // K_F11
	ಸ್ಥಿತಿ 24: ಕೀಲಿ_name[0] = 'F'; ಕೀಲಿ_name[1] = '2'; ನಿಲ್ಲಿಸು;  // K_F12
    }
    ಆದರೆ (ಕೀಲಿ_name[0])
	ಹಿಂತಿರುಗಿ TERMCAP2_ಕೀಲಿ(ಕೀಲಿ_name[0], ಕೀಲಿ_name[1]);
    // shouldn't happen
    ಹಿಂತಿರುಗಿ arg;
}

/*
 * Handle a sequence with key and modifier, one of:
 *	{lead}27;{modifier};{key}~
 *	{lead}{key};{modifier}u
 * Returns the difference in length.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
handle_ಕೀಲಿ_with_modifier(
	ಪೂರ್ಣಾಂಕ	*arg,
	ಪೂರ್ಣಾಂಕ	csi_len,
	ಪೂರ್ಣಾಂಕ	ಸ್ಥಾನಾಂತರ,
	char_u	*buf,
	ಪೂರ್ಣಾಂಕ	bufsize,
	ಪೂರ್ಣಾಂಕ	*buflen,
	ಪೂರ್ಣಾಂಕ	iskitty,
	ಪೂರ್ಣಾಂಕ	trail)
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
    ಆದರೆ (!iskitty
	    && (kitty_protocol_state == KKPS_INITIAL
		|| kitty_protocol_state == KKPS_OFF
		|| kitty_protocol_state == KKPS_AFTER_T_TE)
	    && ಪದ_props[TPR_KITTY].tpr_ಸ್ಥಿತಿ != TPR_YES)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	ch_log(ಖಾಲಿ, "setting seenModifyOtherKeys to TRUE");
#ಆದರೆ_ಅಂತ್ಯ
	ನೋಡಿದ_Modify_Other_ಕೀಲಿಗಳು = TRUE;
    }

    ಪೂರ್ಣಾಂಕ ಕೀಲಿ = iskitty ? arg[0] : arg[2];
    ಪೂರ್ಣಾಂಕ modifiers = decode_modifiers(arg[1]);

    // Some terminals do not apply the Shift modifier to the key.  To make
    // mappings consistent we do it here.  TODO: support more keys.
    ಆದರೆ ((modifiers & MOD_ಮುಖವಾಡ_ಸರಿಸು) && ಕೀಲಿ >= 'a' && ಕೀಲಿ <= 'z')
	ಕೀಲಿ += 'A' - 'a';

    // Putting Esc in the buffer creates ambiguity, it can be the start of an
    // escape sequence.  Use K_ESC to avoid that.
    ಆದರೆ (ಕೀಲಿ == ESC)
	ಕೀಲಿ = K_ESC;

    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (arg[0] >= 11 && arg[0] <= 24 && trail == '~')
	ಕೀಲಿ = parse_csi_f_ಕೀಲಿಗಳು(arg[0]);

    ಹಿಂತಿರುಗಿ put_ಕೀಲಿ_modifiers_ಒಳ_typebuf(ಕೀಲಿ, modifiers, csi_len, ಸ್ಥಾನಾಂತರ, buf,
	    bufsize, buflen);
}

/*
 * Handle a sequence with key without a modifier:
 *	{lead}{key}u
 *	{lead}{key}~
 * Returns the difference in length.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
handle_ಕೀಲಿ_without_modifier(
	ಪೂರ್ಣಾಂಕ	*arg,
	ಪೂರ್ಣಾಂಕ	csi_len,
	ಪೂರ್ಣಾಂಕ	ಸ್ಥಾನಾಂತರ,
	char_u	*buf,
	ಪೂರ್ಣಾಂಕ	bufsize,
	ಪೂರ್ಣಾಂಕ	*buflen,
	ಪೂರ್ಣಾಂಕ	trail)
{
    char_u  string[MAX_ಕೀಲಿ_ಸಂಕೇತ_LEN + 1];
    ಪೂರ್ಣಾಂಕ	    new_slen;

    ಆದರೆ (arg[0] == ESC)
    {
	// Putting Esc in the buffer creates ambiguity, it can be the start of
	// an escape sequence.  Use K_ESC to avoid that.
	string[0] = K_SPECIAL;
	string[1] = KS_EXTRA;
	string[2] = KE_ESC;
	new_slen = 3;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (arg[0] >= 11 && arg[0] <= 24 && trail == '~')
    {
	ಪೂರ್ಣಾಂಕ ಕೀಲಿ = parse_csi_f_ಕೀಲಿಗಳು(arg[0]);
	string[0] = K_SPECIAL;
	string[1] = KEY2TERMCAP0(ಕೀಲಿ);
	string[2] = KEY2TERMCAP1(ಕೀಲಿ);
	new_slen = 3;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	new_slen = ಕೂಡಿಸು_ಕೀಲಿ_ಗಮ್ಯ_buf(arg[0], string);

    ಆದರೆ (put_string_ಒಳ_typebuf(ಸ್ಥಾನಾಂತರ, csi_len, string, new_slen,
						 buf, bufsize, buflen) == FAIL)
	ಹಿಂತಿರುಗಿ -1;
    ಹಿಂತಿರುಗಿ new_slen - csi_len + ಸ್ಥಾನಾಂತರ;
}

/*
 * CSI function key without or with modifiers:
 *	{lead}[ABCDEFHPQRS]
 *	{lead}1;{modifier}[ABCDEFHPQRS]
 * Returns zero when nog recognized, a positive number when recognized.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
handle_csi_function_ಕೀಲಿ(
	ಪೂರ್ಣಾಂಕ	argc,
	ಪೂರ್ಣಾಂಕ	*arg,
	ಪೂರ್ಣಾಂಕ	trail,
	ಪೂರ್ಣಾಂಕ	csi_len,
	char_u	*ಕೀಲಿ_name,
	ಪೂರ್ಣಾಂಕ	ಸ್ಥಾನಾಂತರ,
	char_u  *buf,
	ಪೂರ್ಣಾಂಕ	bufsize,
	ಪೂರ್ಣಾಂಕ	*buflen)
{
    ಕೀಲಿ_name[0] = 'k';
    ಆಯ್ಕೆ (trail)
    {
	ಸ್ಥಿತಿ 'A': ಕೀಲಿ_name[1] = 'u'; ನಿಲ್ಲಿಸು;  // K_UP
	ಸ್ಥಿತಿ 'B': ಕೀಲಿ_name[1] = 'd'; ನಿಲ್ಲಿಸು;  // K_DOWN
	ಸ್ಥಿತಿ 'C': ಕೀಲಿ_name[1] = 'r'; ನಿಲ್ಲಿಸು;  // K_RIGHT
	ಸ್ಥಿತಿ 'D': ಕೀಲಿ_name[1] = 'l'; ನಿಲ್ಲಿಸು;  // K_LEFT

	// case 'E': keypad BEGIN - not supported
	ಸ್ಥಿತಿ 'F': ಕೀಲಿ_name[0] = '@'; ಕೀಲಿ_name[1] = '7'; ನಿಲ್ಲಿಸು;  // K_END
	ಸ್ಥಿತಿ 'H': ಕೀಲಿ_name[1] = 'h'; ನಿಲ್ಲಿಸು;  // K_HOME

	ಸ್ಥಿತಿ 'P': ಕೀಲಿ_name[1] = '1'; ನಿಲ್ಲಿಸು;  // K_F1
	ಸ್ಥಿತಿ 'Q': ಕೀಲಿ_name[1] = '2'; ನಿಲ್ಲಿಸು;  // K_F2
	ಸ್ಥಿತಿ 'R': ಕೀಲಿ_name[1] = '3'; ನಿಲ್ಲಿಸು;  // K_F3
	ಸ್ಥಿತಿ 'S': ಕೀಲಿ_name[1] = '4'; ನಿಲ್ಲಿಸು;  // K_F4

	ಪೂರ್ವನಿಯೋಜಿತ: ಹಿಂತಿರುಗಿ 0;  // not recognized
    }

    ಪೂರ್ಣಾಂಕ ಕೀಲಿ = TERMCAP2_ಕೀಲಿ(ಕೀಲಿ_name[0], ಕೀಲಿ_name[1]);
    ಪೂರ್ಣಾಂಕ modifiers = argc == 2 ? decode_modifiers(arg[1]) : 0;
    put_ಕೀಲಿ_modifiers_ಒಳ_typebuf(ಕೀಲಿ, modifiers,
					csi_len, ಸ್ಥಾನಾಂತರ, buf, bufsize, buflen);
    ಹಿಂತಿರುಗಿ csi_len;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
handle_csi(
	char_u	*tp,
	ಪೂರ್ಣಾಂಕ	len,
	char_u	*argp,
	ಪೂರ್ಣಾಂಕ	ಸ್ಥಾನಾಂತರ,
	char_u  *buf,
	ಪೂರ್ಣಾಂಕ	bufsize,
	ಪೂರ್ಣಾಂಕ	*buflen,
	char_u	*ಕೀಲಿ_name,
	ಪೂರ್ಣಾಂಕ	*slen)
{
    ಪೂರ್ಣಾಂಕ		ಮೊದಲ = -1;  // optional char right after {lead}
    ಪೂರ್ಣಾಂಕ		trail;	     // char that ends CSI sequence
    ಪೂರ್ಣಾಂಕ		arg[3] = {-1, -1, -1};	// argument numbers
    ಪೂರ್ಣಾಂಕ		argc = 0;		// number of arguments
    char_u	*ap = argp;
    ಪೂರ್ಣಾಂಕ		csi_len;

    // Check for non-digit after CSI.
    ಆದರೆ (!VIM_ISDIGIT(*ap))
	ಮೊದಲ = *ap++;

    ಆದರೆ (ಮೊದಲ >= 'A' && ಮೊದಲ <= 'Z')
    {
	// If "first" is in [ABCDEFHPQRS] then it is actually the "trail" and
	// no argument follows.
	trail = ಮೊದಲ;
	ಮೊದಲ = -1;
	--ap;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Find up to three argument numbers.
	ಗಾಗಿ (argc = 0; argc < 3; )
	{
	    ಆದರೆ (ap >= tp + len)
		ಹಿಂತಿರುಗಿ -1;
	    ಆದರೆ (*ap == ';')
		arg[argc++] = -1;  // omitted number
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (VIM_ISDIGIT(*ap))
	    {
		arg[argc] = 0;
		ಗಾಗಿ (;;)
		{
		    ಆದರೆ (ap >= tp + len)
			ಹಿಂತಿರುಗಿ -1;
		    ಆದರೆ (!VIM_ISDIGIT(*ap))
			ನಿಲ್ಲಿಸು;
		    // avoid overflow
		    ಆದರೆ (arg[argc] <= (ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ - 9) / 10)
			arg[argc] = arg[argc] * 10 + (*ap - '0');
		    ++ap;
		}
		++argc;
	    }
	    ಆದರೆ (*ap == ';')
		++ap;
	    ಇಲ್ಲದಿದ್ದರೆ
		ನಿಲ್ಲಿಸು;
	}

	// mrxvt has been reported to have "+" in the version. Assume
	// the escape sequence ends with a letter or one of "{|}~".
	ಆಗ (ap < tp + len
		&& !(*ap >= '{' && *ap <= '~')
		&& !ASCII_ISALPHA(*ap))
	    ++ap;
	ಆದರೆ (ap >= tp + len)
	    ಹಿಂತಿರುಗಿ -1;
	trail = *ap;
    }

    csi_len = (ಪೂರ್ಣಾಂಕ)(ap - tp) + 1;

    // Response to XTQMODKEYS: "CSI > 4 ; Pv m" where Pv indicates the
    // modifyOtherKeys level.  Drop similar responses.
    ಆದರೆ (ಮೊದಲ == '>' && (argc == 1 || argc == 2) && trail == 'm')
    {
	ಆದರೆ (arg[0] == 4 && argc == 2)
	    modify_otherkeys_state = arg[1] == 2 ? MOKS_ENABLED : MOKS_OFF;

	ಕೀಲಿ_name[0] = (ಪೂರ್ಣಾಂಕ)KS_EXTRA;
	ಕೀಲಿ_name[1] = (ಪೂರ್ಣಾಂಕ)KE_IGNORE;
	*slen = csi_len;
    }

    // Function key starting with CSI:
    //	{lead}[ABCDEFHPQRS]
    //	{lead}1;{modifier}[ABCDEFHPQRS]
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಮೊದಲ == -1 && ASCII_ISUPPER(trail)
	    && (argc == 0 || (argc == 2 && arg[0] == 1)))
    {
	ಪೂರ್ಣಾಂಕ res = handle_csi_function_ಕೀಲಿ(argc, arg, trail,
			      csi_len, ಕೀಲಿ_name, ಸ್ಥಾನಾಂತರ, buf, bufsize, buflen);
	ಹಿಂತಿರುಗಿ res <= 0 ? res : len + res;
    }

    // Cursor position report: {lead}{row};{col}R
    // Eat it when there are 2 arguments and it ends in 'R'.
    // Also when u7_status is not "sent", it may be from a previous Vim that
    // just exited.  But not for <S-F3>, it sends something similar, check for
    // row and column to make sense.
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಮೊದಲ == -1 && argc == 2 && trail == 'R')
    {
	handle_u7_response(arg, tp, csi_len);

	ಕೀಲಿ_name[0] = (ಪೂರ್ಣಾಂಕ)KS_EXTRA;
	ಕೀಲಿ_name[1] = (ಪೂರ್ಣಾಂಕ)KE_IGNORE;
	*slen = csi_len;
    }

#ಆದರೆ (defined(UNIX) || defined(MSWIN)) \
    && (defined(FEAT_IMAGE) || defined(FEAT_EVAL))
    // Response to CSI 14 t or CSI 16 t
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಮೊದಲ == -1 && argc >= 3
	    && (arg[0] == 4 || arg[0] == 6) && trail == 't')
    {
	ಆದರೆ (arg[0] == 4)
	{
	    LOG_TRN("Received CSI 14 t response: %s", tp);
	    cell_ಅಗಲ = arg[2] / Columns;
	    cell_ಎತ್ತರ = arg[1] / ಅಡ್ಡಸಾಲುಗಳು;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    LOG_TRN("Received CSI 16 t response: %s", tp);
	    cell_ಅಗಲ = arg[2];
	    cell_ಎತ್ತರ = arg[1];
	}

	ಕೀಲಿ_name[0] = (ಪೂರ್ಣಾಂಕ)KS_EXTRA;
	ಕೀಲಿ_name[1] = (ಪೂರ್ಣಾಂಕ)KE_IGNORE;
	*slen = csi_len;
    }
#ಆದರೆ_ಅಂತ್ಯ

    // Primary device attributes (DA1) response
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಮೊದಲ == '?' && trail == 'c')
    {
	LOG_TRN("Received DA1 response: %s", tp);

	*slen = csi_len;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	ಗಣ_vim_var_string(VV_TERMDA1, tp, *slen);
#ಆದರೆ_ಅಂತ್ಯ
	apply_autocmds(EVENT_TERMRESPONSEALL,
					(char_u *)"da1", ಖಾಲಿ, FALSE, curbuf);

	ಕೀಲಿ_name[0] = (ಪೂರ್ಣಾಂಕ)KS_EXTRA;
	ಕೀಲಿ_name[1] = (ಪೂರ್ಣಾಂಕ)KE_IGNORE;
	*slen = csi_len;
    }

    // DECRPM mode 2026 or 2048.
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಮೊದಲ == '?' && trail == 'y' && argc == 2
	    && (arg[0] == 2026 || arg[0] == 2048))
    {
	ಪೂರ್ಣಾಂಕ setting = arg[1];

	*slen = csi_len;
	ಕೀಲಿ_name[0] = (ಪೂರ್ಣಾಂಕ)KS_EXTRA;
	ಕೀಲಿ_name[1] = (ಪೂರ್ಣಾಂಕ)KE_IGNORE;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMRESPONSE
	// Mark the DECRQM request as answered so it is not sent again and
	// stoptermcap() does not wait for it.
	ಆದರೆ (decrqm_ಸ್ಥಿತಿ.tr_ಪ್ರಗತಿ == ಸ್ಥಿತಿ_ಕಳುಹಿಸಿದ)
	    decrqm_ಸ್ಥಿತಿ.tr_ಪ್ರಗತಿ = ಸ್ಥಿತಿ_GOT;
#ಆದರೆ_ಅಂತ್ಯ

	ಆದರೆ (setting >= 0 && setting <= 4)
	{
	    LOG_TRN("Received DECRPM mode %d: %s", arg[0], tp);

	    ಆಯ್ಕೆ (arg[0])
	    {
		ಸ್ಥಿತಿ 2026:
		    sync_output_setting = setting;
		    ಗಣ_option_ಮೌಲ್ಯ_give_err((char_u *)"termsync",
			    setting == 1 || setting == 2, ಖಾಲಿ, 0);
		    ನಿಲ್ಲಿಸು;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
		ಸ್ಥಿತಿ 2048:
		    win_resize_setting = setting;

		    ಪದ_ಗಣ_win_resize(true);
		    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    LOG_TRN("Unknown DECRPM mode %d setting %d", arg[0], setting);
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
    // In-band window resize event
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (win_resize_enabled && argc >= 3 && arg[0] == 48)
    {
	ಪೂರ್ಣಾಂಕ ಎತ್ತರ = arg[1], ಅಗಲ = arg[2];

	*slen = csi_len;
	ಕೀಲಿ_name[0] = (ಪೂರ್ಣಾಂಕ)KS_EXTRA;
	ಕೀಲಿ_name[1] = (ಪೂರ್ಣಾಂಕ)KE_IGNORE;

	// Only update if needed. Avoids intro message from disappearing on
	// startup (due to initial event).
	ಆದರೆ (ಎತ್ತರ != ಅಡ್ಡಸಾಲುಗಳು || ಅಗಲ != Columns)
	    ಗಣ_shellsize(ಅಗಲ, ಎತ್ತರ, true);
    }
#ಆದರೆ_ಅಂತ್ಯ

    // Version string: Eat it when there is at least one digit and
    // it ends in 'c'
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*T_CRV != NUL && ap > argp + 1 && trail == 'c')
    {
	handle_ಆವೃತ್ತಿ_response(ಮೊದಲ, arg, argc, tp);

	*slen = csi_len;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	ಗಣ_vim_var_string(VV_TERMRESPONSE, tp, *slen);
#ಆದರೆ_ಅಂತ್ಯ
	apply_autocmds(EVENT_TERMRESPONSE,
					ಖಾಲಿ, ಖಾಲಿ, FALSE, curbuf);
	apply_autocmds(EVENT_TERMRESPONSEALL,
					(char_u *)"version", ಖಾಲಿ, FALSE, curbuf);
	ಕೀಲಿ_name[0] = (ಪೂರ್ಣಾಂಕ)KS_EXTRA;
	ಕೀಲಿ_name[1] = (ಪೂರ್ಣಾಂಕ)KE_IGNORE;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMRESPONSE
    // Check blinking cursor from xterm:
    // {lead}?12;1$y       set
    // {lead}?12;2$y       not set
    //
    // {lead} can be <Esc>[ or CSI
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (rbm_ಸ್ಥಿತಿ.tr_ಪ್ರಗತಿ == ಸ್ಥಿತಿ_ಕಳುಹಿಸಿದ
	    && ಮೊದಲ == '?'
	    && ap == argp + 6
	    && arg[0] == 12
	    && ap[-1] == '$'
	    && trail == 'y')
    {
	initial_cursor_blink = (arg[1] == '1');
	rbm_ಸ್ಥಿತಿ.tr_ಪ್ರಗತಿ = ಸ್ಥಿತಿ_GOT;
	LOG_TRN("Received cursor blinking mode response: %s", tp);
	ಕೀಲಿ_name[0] = (ಪೂರ್ಣಾಂಕ)KS_EXTRA;
	ಕೀಲಿ_name[1] = (ಪೂರ್ಣಾಂಕ)KE_IGNORE;
	*slen = csi_len;
# ifdef FEAT_EVAL
	ಗಣ_vim_var_string(VV_TERMBLINKRESP, tp, *slen);
# endif
	apply_autocmds(EVENT_TERMRESPONSEALL,
					(char_u *)"cursorblink", ಖಾಲಿ, FALSE, curbuf);
    }
#ಆದರೆ_ಅಂತ್ಯ

    // Kitty keyboard protocol status response: CSI ? flags u
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಮೊದಲ == '?' && argc == 1 && trail == 'u')
    {
	// The protocol has various "progressive enhancement flags" values, but
	// we only check for zero and non-zero here.
	ಆದರೆ (arg[0] == '0')
	{
	    kitty_protocol_state = KKPS_OFF;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    kitty_protocol_state = KKPS_ENABLED;

	    // Reset seenModifyOtherKeys just in case some key combination has
	    // been seen that set it before we get the status response.
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	    ch_log(ಖಾಲಿ, "setting seenModifyOtherKeys to FALSE");
#ಆದರೆ_ಅಂತ್ಯ
	    ನೋಡಿದ_Modify_Other_ಕೀಲಿಗಳು = FALSE;
	}

	ಕೀಲಿ_name[0] = (ಪೂರ್ಣಾಂಕ)KS_EXTRA;
	ಕೀಲಿ_name[1] = (ಪೂರ್ಣಾಂಕ)KE_IGNORE;
	*slen = csi_len;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMRESPONSE
    // Check for a window position response from the terminal:
    //       {lead}3;{x};{y}t
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (did_request_winpos && argc == 3 && arg[0] == 3
						   && trail == 't')
    {
	winpos_x = arg[1];
	winpos_y = arg[2];
	// got finished code: consume it
	ಕೀಲಿ_name[0] = (ಪೂರ್ಣಾಂಕ)KS_EXTRA;
	ಕೀಲಿ_name[1] = (ಪೂರ್ಣಾಂಕ)KE_IGNORE;
	*slen = csi_len;

	ಆದರೆ (--did_request_winpos <= 0)
	    winpos_ಸ್ಥಿತಿ.tr_ಪ್ರಗತಿ = ಸ್ಥಿತಿ_GOT;
    }
#ಆದರೆ_ಅಂತ್ಯ

    // Key with modifier:
    //	{lead}27;{modifier};{key}~
    //	{lead}{key};{modifier}u
    //	{lead}{key};{modifier}~
    // Even though we only handle four modifiers and the {modifier} value
    // should be 16 or lower, we accept all modifier values to avoid the raw
    // sequence to be passed through.
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((arg[0] == 27 && argc == 3 && trail == '~')
		|| (argc == 2 && (trail == 'u' || trail == '~')))
    {
	ಪೂರ್ಣಾಂಕ iskitty = argc == 2 && (trail == 'u' || trail == '~');
	ಹಿಂತಿರುಗಿ len + handle_ಕೀಲಿ_with_modifier(arg, csi_len, ಸ್ಥಾನಾಂತರ, buf,
		bufsize, buflen, iskitty, trail);
    }

    // Key without modifier (Kitty sends this for Esc or F3):
    //	{lead}{key}u
    //	{lead}{key}~
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argc == 1 && (trail == 'u' || trail == '~'))
    {
	ಹಿಂತಿರುಗಿ len + handle_ಕೀಲಿ_without_modifier(arg, csi_len, ಸ್ಥಾನಾಂತರ, buf,
		bufsize, buflen, trail);
    }

    // else: Unknown CSI sequence.  We could drop it, but then the
    // user can't create a map for it.
    ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ
check_for_color_response(char_u *resp, ಪೂರ್ಣಾಂಕ len)
{
    ಪೂರ್ಣಾಂಕ		i, j;
    char_u	*argp;

    j = 1 + (resp[0] == ESC);
    argp = resp + j;

    ಆದರೆ (len >= j + 3 && (argp[0] != '1'
			     || (argp[1] != '1' && argp[1] != '0')
			     || argp[2] != ';'))
	i = 0; // no match
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಗಾಗಿ (i = j; i < len; ++i)
	    ಆದರೆ (resp[i] == '\007' || (resp[0] == OSC ? resp[i] == STERM
			: (resp[i] == ESC && i + 1 < len && resp[i + 1] == '\\')))
	    {
		ಪೂರ್ಣಾಂಕ is_bg = argp[1] == '1';
		ಪೂರ್ಣಾಂಕ is_4digit = i - j >= 21 && resp[j + 11] == '/'
						  && resp[j + 16] == '/';

		ಆದರೆ (i - j >= 15 && STRNCMP(resp + j + 3, "rgb:", 4) == 0
			    && (is_4digit
				   || (resp[j + 9] == '/' && resp[j + 12] == '/')))
		{
		    char_u *tp_r = resp + j + 7;
		    char_u *tp_g = resp + j + (is_4digit ? 12 : 10);
		    char_u *tp_b = resp + j + (is_4digit ? 17 : 13);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMRESPONSE
		    ಪೂರ್ಣಾಂಕ rval, gval, bval;

		    rval = hexhex2nr(tp_r);
		    gval = hexhex2nr(tp_g);
		    bval = hexhex2nr(tp_b);
#ಆದರೆ_ಅಂತ್ಯ
		    ಆದರೆ (is_bg)
		    {
			ಅಕ್ಷರ *new_bg_val = (3 * '6' < *tp_r + *tp_g +
					     *tp_b) ? "light" : "dark";

			LOG_TRN("Received RBG response: r=%d g=%d b=%d", rval, gval, bval);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMRESPONSE
			rbg_ಸ್ಥಿತಿ.tr_ಪ್ರಗತಿ = ಸ್ಥಿತಿ_GOT;
			bg_r = rval;
			bg_g = gval;
			bg_b = bval;
#ಆದರೆ_ಅಂತ್ಯ
			ಆದರೆ (!option_was_ಗಣ((char_u *)"bg")
				      && STRCMP(p_bg, new_bg_val) != 0)
			{
			    // value differs, apply it
			    ಗಣ_option_ಮೌಲ್ಯ_give_err((char_u *)"bg",
						  0L, (char_u *)new_bg_val, 0);
			    reset_option_was_ಗಣ((char_u *)"bg");
			    redraw_asap(UPD_CLEAR);
			}
		    }
#ಆದರೆ defined(FEAT_TERMRESPONSE) && defined(FEAT_TERMINAL)
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			LOG_TRN("Received RFG response: r=%d g=%d b=%d", rval, gval, bval);
			rfg_ಸ್ಥಿತಿ.tr_ಪ್ರಗತಿ = ಸ್ಥಿತಿ_GOT;
			fg_r = rval;
			fg_g = gval;
			fg_b = bval;
		    }
#ಆದರೆ_ಅಂತ್ಯ
		}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
		ಗಣ_vim_var_string(is_bg ? VV_TERMRBGRESP
						  : VV_TERMRFGRESP, resp, len);
#ಆದರೆ_ಅಂತ್ಯ
		apply_autocmds(EVENT_TERMRESPONSEALL,
			is_bg ? (char_u *)"background" : (char_u *)"foreground",
			ಖಾಲಿ, FALSE, curbuf);
		ನಿಲ್ಲಿಸು;
	    }
    }
    ಆದರೆ (i == len)
	LOG_TR1("not enough characters for RB");
}

ಸ್ಥಿರ oscstate_T osc_state;

/*
 * Return true if currently receiving an OSC response.
 */
    bool
ಒಳ_osc_sequence(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ osc_state.processing;
}

/*
 * Handles any OSC sequence and places the result in "v:termosc". Returns OK on
 * success and FAIL on failure.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
handle_osc(char_u *tp, ಪೂರ್ಣಾಂಕ len, char_u *ಕೀಲಿ_name, ಪೂರ್ಣಾಂಕ *slen)
{
    char_u ಕೊನೆಯ_char;

    ಆದರೆ (!osc_state.processing)
    {
	ಪೂರ್ಣಾಂಕ cur;

	LOG_TRN("Received OSC response: %s", (ಅಕ್ಷರ*)tp);
	// Check if it is a valid OSC sequence, and consume it. OSC format
	// consists of:
	// <identifier><data><terminator>
	// <identifier> is either <Esc>] or an OSC character
	// <terminator> can be '\007', <Esc>\ or STERM.
	cur = 1 + (tp[0] == ESC);

	ಆದರೆ (len < cur + 1 + (tp[0] != OSC)) // Include terminator as well
	    ಹಿಂತಿರುಗಿ FAIL;

	// The whole OSC response may be larger than the typeahead buffer.
	// To handle this, keep reading data in and out of the typeahead
	// buffer until we read an OSC terminator or timeout.

	// We can't use the previous buffer since we transferred ownership of it
	// to the vim var.
	ga_init2(&osc_state.buf, 1, 1024);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ELAPSED_FUNC
	ELAPSED_INIT(osc_state.ಆರಂಭ_tv);
#ಆದರೆ_ಅಂತ್ಯ
	osc_state.processing = TRUE;
	osc_state.ಆರಂಭ_char = tp[0];
	ಕೊನೆಯ_char = 0;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	ಕೊನೆಯ_char = ((char_u *)osc_state.buf.ga_ದತ್ತಾಂಶ)[osc_state.buf.ga_len - 1];

    ಕೀಲಿ_name[0] = (ಪೂರ್ಣಾಂಕ)KS_EXTRA;

    // Read data and append to buffer. If we reach a terminator, then
    // finally set the vim var.
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < len; i++)
	ಆದರೆ (tp[i] == '\007' || (osc_state.ಆರಂಭ_char == OSC ? tp[i] == STERM
		    : ((tp[i] == ESC && i + 1 < len && tp[i + 1] == '\\')
			|| (i == 0 && tp[i] == '\\' && ಕೊನೆಯ_char == ESC)
			)))
	{
	    osc_state.processing = FALSE;

	    ಕೀಲಿ_name[1] = (ಪೂರ್ಣಾಂಕ)KE_OSC;

	    ga_concat_len(&osc_state.buf, tp, i + 1 + (tp[i] == ESC));
	    ga_append(&osc_state.buf, NUL);
	    *slen = i + 1 + (tp[i] == ESC);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	    ಗಣ_vim_var_string_ನೇರ(VV_TERMOSC, osc_state.buf.ga_ದತ್ತಾಂಶ);
#ಆದರೆ_ಅಂತ್ಯ
	    // Check for background/foreground colour response
	    check_for_color_response(osc_state.buf.ga_ದತ್ತಾಂಶ, osc_state.buf.ga_len - 1);

	    char_u savebg = *p_bg;
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"osc",
		    ಖಾಲಿ, FALSE, curbuf);
	    ಆದರೆ (*p_bg != savebg)
		redraw_asap(UPD_CLEAR);
	    ಹಿಂತಿರುಗಿ OK;
	}

    ಕೀಲಿ_name[1] = (ಪೂರ್ಣಾಂಕ)KE_IGNORE;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ELAPSED_FUNC
    ಆದರೆ (ELAPSED_FUNC(osc_state.ಆರಂಭ_tv) >= p_ost)
    {
	semsg(_(e_osc_response_timed_ಹೊರಹೋಗು), osc_state.buf.ga_len,
		osc_state.buf.ga_ದತ್ತಾಂಶ);

	ga_clear(&osc_state.buf);
	osc_state.processing = FALSE;
	ಹಿಂತಿರುಗಿ FAIL;
    }
#ಆದರೆ_ಅಂತ್ಯ

    ga_concat(&osc_state.buf, tp);
    *slen = len; // Consume everything

    ಹಿಂತಿರುಗಿ OK;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
handle_dcs(char_u *tp, char_u *argp, ಪೂರ್ಣಾಂಕ len, char_u *ಕೀಲಿ_name, ಪೂರ್ಣಾಂಕ *slen)
{
    ಪೂರ್ಣಾಂಕ i, j;

    LOG_TRN("Received DCS response: %s", (ಅಕ್ಷರ*)tp);
    j = 1 + (tp[0] == ESC);
    ಆದರೆ (len < j + 3)
	i = len; // need more chars
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((argp[1] != '+' && argp[1] != '$')
	    || (argp[2] != 'r' && argp[2] != 'R'))
	i = 0; // no match
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argp[1] == '+')
	// key code response
	ಗಾಗಿ (i = j; i < len; ++i)
	{
	    ಆದರೆ ((tp[i] == ESC && i + 1 < len && tp[i + 1] == '\\')
		    || tp[i] == STERM)
	    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMRESPONSE
		// handle a key code response, drop a resource response
		ಆದರೆ (i - j >= 3 && argp[2] == 'r')
		    got_ಸಂಕೇತ_from_ಪದ(tp + j, i);
#ಆದರೆ_ಅಂತ್ಯ
		ಕೀಲಿ_name[0] = (ಪೂರ್ಣಾಂಕ)KS_EXTRA;
		ಕೀಲಿ_name[1] = (ಪೂರ್ಣಾಂಕ)KE_IGNORE;
		*slen = i + 1 + (tp[i] == ESC);
		ನಿಲ್ಲಿಸು;
	    }
	}
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Probably the cursor shape response.  Make sure that "i"
	// is equal to "len" when there are not sufficient
	// characters.
	ಗಾಗಿ (i = j + 3; i < len; ++i)
	{
	    ಆದರೆ (i - j == 3 && !ಸುರಕ್ಷಿತ_isdigit(tp[i]))
		ನಿಲ್ಲಿಸು;
	    ಆದರೆ (i - j == 4 && tp[i] != ' ')
		ನಿಲ್ಲಿಸು;
	    ಆದರೆ (i - j == 5 && tp[i] != 'q')
		ನಿಲ್ಲಿಸು;
	    ಆದರೆ (i - j == 6 && tp[i] != ESC && tp[i] != STERM)
		ನಿಲ್ಲಿಸು;
	    ಆದರೆ ((i - j == 6 && tp[i] == STERM)
		    || (i - j == 7 && tp[i] == '\\'))
	    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMRESPONSE
		ಪೂರ್ಣಾಂಕ ಸಂಖ್ಯೆ = argp[3] - '0';

		// 0, 1 = block blink, 2 = block
		// 3 = underline blink, 4 = underline
		// 5 = vertical bar blink, 6 = vertical bar
		ಸಂಖ್ಯೆ = ಸಂಖ್ಯೆ == 0 ? 1 : ಸಂಖ್ಯೆ;
		initial_cursor_shape = (ಸಂಖ್ಯೆ + 1) / 2;
		// The blink flag is actually inverted, compared to
		// the value set with T_SH.
		initial_cursor_shape_blink =
				       (ಸಂಖ್ಯೆ & 1) ? FALSE : TRUE;
		rcs_ಸ್ಥಿತಿ.tr_ಪ್ರಗತಿ = ಸ್ಥಿತಿ_GOT;
#ಆದರೆ_ಅಂತ್ಯ
		LOG_TRN("Received cursor shape response: %s", tp);

		ಕೀಲಿ_name[0] = (ಪೂರ್ಣಾಂಕ)KS_EXTRA;
		ಕೀಲಿ_name[1] = (ಪೂರ್ಣಾಂಕ)KE_IGNORE;
		*slen = i + 1;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
		ಗಣ_vim_var_string(VV_TERMSTYLERESP, tp, *slen);
#ಆದರೆ_ಅಂತ್ಯ
		apply_autocmds(EVENT_TERMRESPONSEALL,
					(char_u *)"cursorshape", ಖಾಲಿ, FALSE, curbuf);
		ನಿಲ್ಲಿಸು;
	    }
	}
    }

    ಆದರೆ (i == len)
    {
	// These codes arrive many together, each code can be
	// truncated at any point.
	LOG_TR1("not enough characters for XT");
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
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
    ಪೂರ್ಣಾಂಕ
check_termcode(
    ಪೂರ್ಣಾಂಕ		max_ಸ್ಥಾನಾಂತರ,
    char_u	*buf,
    ಪೂರ್ಣಾಂಕ		bufsize,
    ಪೂರ್ಣಾಂಕ		*buflen)
{
    char_u	*tp;
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		slen = 0;	// init for GCC
    ಪೂರ್ಣಾಂಕ		modslen;
    ಪೂರ್ಣಾಂಕ		len;
    ಪೂರ್ಣಾಂಕ		retval = 0;
    ಪೂರ್ಣಾಂಕ		ಸ್ಥಾನಾಂತರ;
    char_u	ಕೀಲಿ_name[2];
    ಪೂರ್ಣಾಂಕ		modifiers;
    char_u	*modifiers_ಆರಂಭ = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		ಕೀಲಿ;
    ಪೂರ್ಣಾಂಕ		new_slen;   // Length of what will replace the termcode
    char_u	string[MAX_ಕೀಲಿ_ಸಂಕೇತ_LEN + 1];
    ಪೂರ್ಣಾಂಕ		i, j;
    ಪೂರ್ಣಾಂಕ		idx = 0;
    ಪೂರ್ಣಾಂಕ		cpo_koffset;

    cpo_koffset = (vim_strchr(p_cpo, CPO_KOFFSET) != ಖಾಲಿ);

    /*
     * Speed up the checks for terminal codes by gathering all first bytes
     * used in termleader[].  Often this is just a single <Esc>.
     */
    ಆದರೆ (need_gather)
	gather_termleader();

    /*
     * Check at several positions in typebuf.tb_buf[], to catch something like
     * "x<Up>" that can be mapped. Stop at max_offset, because characters
     * after that cannot be used for mapping, and with @r commands
     * typebuf.tb_buf[] can become very long.
     * This is used often, KEEP IT FAST!
     */
    ಗಾಗಿ (ಸ್ಥಾನಾಂತರ = 0; ಸ್ಥಾನಾಂತರ < max_ಸ್ಥಾನಾಂತರ; ++ಸ್ಥಾನಾಂತರ)
    {
	ಆದರೆ (buf == ಖಾಲಿ)
	{
	    ಆದರೆ (ಸ್ಥಾನಾಂತರ >= typebuf.tb_len)
		ನಿಲ್ಲಿಸು;
	    tp = typebuf.tb_buf + typebuf.tb_off + ಸ್ಥಾನಾಂತರ;
	    len = typebuf.tb_len - ಸ್ಥಾನಾಂತರ;	// length of the input
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (ಸ್ಥಾನಾಂತರ >= *buflen)
		ನಿಲ್ಲಿಸು;
	    tp = buf + ಸ್ಥಾನಾಂತರ;
	    len = *buflen - ಸ್ಥಾನಾಂತರ;
	}

	/*
	 * Don't check characters after K_SPECIAL, those are already
	 * translated terminal chars (avoid translating ~@^Hx).
	 */
	ಆದರೆ (*tp == K_SPECIAL)
	{
	    ಸ್ಥಾನಾಂತರ += 2;	// there are always 2 extra characters
	    ಮುಂದುವರಿ;
	}

	ಆದರೆ (osc_state.processing)
	{
	    // Still processing OSC response data, go straight to handler
	    // function.
	    tp[len] = NUL;
	    ಕೀಲಿ_name[0] = NUL;
	    ಕೀಲಿ_name[1] = NUL;
	    modifiers = 0;
	    ಹೋಗು handle_osc;
	}

	/*
	 * Skip this position if the character does not appear as the first
	 * character in term_strings. This speeds up a lot, since most
	 * termcodes start with the same character (ESC or CSI).
	 */
	i = *tp;
	ಗಾಗಿ (p = termleader; *p && *p != i; ++p)
	    ;
	ಆದರೆ (*p == NUL)
	    ಮುಂದುವರಿ;

	/*
	 * Skip this position if p_ek is not set and tp[0] is an ESC and we
	 * are in Insert mode.
	 */
	ಆದರೆ (*tp == ESC && !p_ek && (State & ಬಹುಲಕ_ಸೇರಿಸು))
	    ಮುಂದುವರಿ;

	tp[len] = NUL;
	ಕೀಲಿ_name[0] = NUL;	// no key name found yet
	ಕೀಲಿ_name[1] = NUL;	// no key name found yet
	modifiers = 0;		// no modifiers yet

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
	ಆದರೆ (gui.ಒಳ_use)
	{
	    /*
	     * GUI special key codes are all of the form [CSI xx].
	     */
	    ಆದರೆ (*tp == CSI)	    // Special key from GUI
	    {
		ಆದರೆ (len < 3)
		    ಹಿಂತಿರುಗಿ -1;	    // Shouldn't happen
		slen = 3;
		ಕೀಲಿ_name[0] = tp[1];
		ಕೀಲಿ_name[1] = tp[2];
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ // FEAT_GUI
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
	    ಆದರೆ (len >= 3 && tp[0] == CSI && tp[1] == KS_EXTRA
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
	ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	{
	    ಪೂರ್ಣಾಂಕ  mouse_ಸೂಚ್ಯಂಕ_ಸಿಕ್ಕಿತು = -1;
	    ಪೂರ್ಣಾಂಕ keypad_ಸೂಚ್ಯಂಕ_ಸಿಕ್ಕಿತು = -1;
	    ಪೂರ್ಣಾಂಕ keypad_slen_ಸಿಕ್ಕಿತು = 0;

	    ಗಾಗಿ (idx = 0; idx < tc_len; ++idx)
	    {
		ಪೂರ್ಣಾಂಕ     is_keypad = FALSE;
		/*
		 * Ignore the entry if we are not at the start of
		 * typebuf.tb_buf[]
		 * and there are not enough characters to make a match.
		 * But only when the 'K' flag is in 'cpoptions'.
		 */
		slen = termcodes[idx].len;
		modifiers_ಆರಂಭ = ಖಾಲಿ;
		ಆದರೆ (cpo_koffset && ಸ್ಥಾನಾಂತರ && len < slen)
		    ಮುಂದುವರಿ;
		ಆದರೆ (STRNCMP(termcodes[idx].ಸಂಕೇತ_2, tp,
				     (size_t)(slen > len ? len : slen)) == 0)
		{
		    ಪೂರ್ಣಾಂಕ	    looks_like_mouse_ಆರಂಭ = FALSE;

		    ಆದರೆ (len < slen)		// got a partial sequence
			ಹಿಂತಿರುಗಿ -1;		// need to get more chars

		    /*
		     * When found a keypad key, check if there is another key
		     * that matches and use that one.  This makes <Home> to be
		     * found instead of <kHome> when they produce the same
		     * key code.
		     */
		    ಆದರೆ (termcodes[idx].name[0] == 'K'
				       && (VIM_ISDIGIT(termcodes[idx].name[1])
						|| ASCII_ISUPPER(termcodes[idx].name[1])))
		    {
			is_keypad = TRUE;
			// Only use it when there is no other match.
			ಆದರೆ (keypad_ಸೂಚ್ಯಂಕ_ಸಿಕ್ಕಿತು < 0)
			{
			    keypad_ಸೂಚ್ಯಂಕ_ಸಿಕ್ಕಿತು = idx;
			    keypad_slen_ಸಿಕ್ಕಿತು = slen;
			}
		    }

		    ಆದರೆ (slen == 2 && len > 2
			    && termcodes[idx].ಸಂಕೇತ_2[0] == ESC
			    && termcodes[idx].ಸಂಕೇತ_2[1] == '[')
		    {
			// The mouse termcode "ESC [" is also the prefix of
			// "ESC [ I" (focus gained) and other keys.  Check some
			// more bytes to find out.
			ಆದರೆ (!ಸುರಕ್ಷಿತ_isdigit(tp[2]))
			{
			    // ESC [ without number following: Only use it when
			    // there is no other match.
			    looks_like_mouse_ಆರಂಭ = TRUE;
			}
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (termcodes[idx].name[0] == KS_DEC_MOUSE)
			{
			    char_u  *nr = tp + 2;
			    ಪೂರ್ಣಾಂಕ	    ಎಣಿಕೆ_2 = 0;

			    // If a digit is following it could be a key with
			    // modifier, e.g., ESC [ 1;2P.  Can be confused
			    // with DEC_MOUSE, which requires four numbers
			    // following.  If not then it can't be a DEC_MOUSE
			    // code.
			    ಗಾಗಿ (;;)
			    {
				++ಎಣಿಕೆ_2;
				(ಶೂನ್ಯ)getdigits(&nr);
				ಆದರೆ (nr >= tp + len)
				    ಹಿಂತಿರುಗಿ -1;	// partial sequence
				ಆದರೆ (*nr != ';')
				    ನಿಲ್ಲಿಸು;
				++nr;
				ಆದರೆ (nr >= tp + len)
				    ಹಿಂತಿರುಗಿ -1;	// partial sequence
			    }
			    ಆದರೆ (ಎಣಿಕೆ_2 < 4)
				ಮುಂದುವರಿ;	// no match
			}
		    }
		    ಆದರೆ (looks_like_mouse_ಆರಂಭ)
		    {
			// Only use it when there is no other match.
			ಆದರೆ (mouse_ಸೂಚ್ಯಂಕ_ಸಿಕ್ಕಿತು < 0)
			    mouse_ಸೂಚ್ಯಂಕ_ಸಿಕ್ಕಿತು = idx;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!is_keypad)
		    {
			ಕೀಲಿ_name[0] = termcodes[idx].name[0];
			ಕೀಲಿ_name[1] = termcodes[idx].name[1];
			ನಿಲ್ಲಿಸು;
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
		ಆದರೆ (termcodes[idx].modlen > 0 && mouse_ಸೂಚ್ಯಂಕ_ಸಿಕ್ಕಿತು < 0)
		{
		    modslen = termcodes[idx].modlen;
		    ಆದರೆ (cpo_koffset && ಸ್ಥಾನಾಂತರ && len < modslen)
			ಮುಂದುವರಿ;
		    ಆದರೆ (STRNCMP(termcodes[idx].ಸಂಕೇತ_2, tp,
				(size_t)(modslen > len ? len : modslen)) == 0)
		    {
			ಪೂರ್ಣಾಂಕ	    n;

			ಆದರೆ (len <= modslen)	// got a partial sequence
			    ಹಿಂತಿರುಗಿ -1;		// need to get more chars

			ಆದರೆ (tp[modslen] == termcodes[idx].ಸಂಕೇತ_2[slen - 1])
			    // no modifiers
			    slen = modslen + 1;
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (tp[modslen] != ';' && modslen == slen - 3)
			    // no match for "code;*X" with "code;"
			    ಮುಂದುವರಿ;
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (termcodes[idx].ಸಂಕೇತ_2[modslen] == '@'
					 && (tp[modslen] != '1'
						    || tp[modslen + 1] != ';'))
			    // no match for "<Esc>[@" with "<Esc>[1;"
			    ಮುಂದುವರಿ;
			ಇಲ್ಲದಿದ್ದರೆ
			{
			    // Skip over the digits, the final char must
			    // follow. URXVT can use a negative value, thus
			    // also accept '-'.
			    ಗಾಗಿ (j = slen - 2; j < len && (ಸುರಕ್ಷಿತ_isdigit(tp[j])
				       || tp[j] == '-' || tp[j] == ';'); ++j)
				;
			    ++j;
			    ಆದರೆ (len < j)	// got a partial sequence
				ಹಿಂತಿರುಗಿ -1;	// need to get more chars
			    ಆದರೆ (tp[j - 1] != termcodes[idx].ಸಂಕೇತ_2[slen - 1])
				ಮುಂದುವರಿ;	// no match

			    modifiers_ಆರಂಭ = tp + slen - 2;

			    // Match!  Convert modifier bits.
			    n = atoi((ಅಕ್ಷರ *)modifiers_ಆರಂಭ);
			    modifiers |= decode_modifiers(n);

			    slen = j;
			}
			ಆದರೆ (termcodes[idx].name[0] == 'K'
					   && (VIM_ISDIGIT(termcodes[idx].name[1])
						    || ASCII_ISUPPER(termcodes[idx].name[1])))
			{
			    is_keypad = TRUE;
			    ಆದರೆ (keypad_ಸೂಚ್ಯಂಕ_ಸಿಕ್ಕಿತು < 0)
			    {
				keypad_ಸೂಚ್ಯಂಕ_ಸಿಕ್ಕಿತು = idx;
				keypad_slen_ಸಿಕ್ಕಿತು = slen;
			    }
			}
			ಆದರೆ (!is_keypad)
			{
			    ಕೀಲಿ_name[0] = termcodes[idx].name[0];
			    ಕೀಲಿ_name[1] = termcodes[idx].name[1];
			    ನಿಲ್ಲಿಸು;
			}
		    }
		}
	    }
	    ಆದರೆ (idx == tc_len && keypad_ಸೂಚ್ಯಂಕ_ಸಿಕ್ಕಿತು >= 0)
	    {
		ಕೀಲಿ_name[0] = termcodes[keypad_ಸೂಚ್ಯಂಕ_ಸಿಕ್ಕಿತು].name[0];
		ಕೀಲಿ_name[1] = termcodes[keypad_ಸೂಚ್ಯಂಕ_ಸಿಕ್ಕಿತು].name[1];
		slen = keypad_slen_ಸಿಕ್ಕಿತು;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (idx == tc_len && mouse_ಸೂಚ್ಯಂಕ_ಸಿಕ್ಕಿತು >= 0)
	    {
		ಕೀಲಿ_name[0] = termcodes[mouse_ಸೂಚ್ಯಂಕ_ಸಿಕ್ಕಿತು].name[0];
		ಕೀಲಿ_name[1] = termcodes[mouse_ಸೂಚ್ಯಂಕ_ಸಿಕ್ಕಿತು].name[1];
	    }
	}

	ಆದರೆ (ಕೀಲಿ_name[0] == NUL
	    // Mouse codes of DEC and pterm start with <ESC>[.  When
	    // detecting the start of these mouse codes they might as well be
	    // another key code or terminal response.
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MOUSE_DEC
	    || ಕೀಲಿ_name[0] == KS_DEC_MOUSE
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MOUSE_PTERM
	    || ಕೀಲಿ_name[0] == KS_PTERM_MOUSE
#ಆದರೆ_ಅಂತ್ಯ
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
	    ಆದರೆ (((tp[0] == ESC && len >= 3 && tp[1] == '[')
			|| (tp[0] == CSI && len >= 2))
		    && vim_strchr((char_u *)"0123456789>?ABCDEFHPQRS",
								*argp) != ಖಾಲಿ)
	    {
		ಪೂರ್ಣಾಂಕ resp = handle_csi(tp, len, argp, ಸ್ಥಾನಾಂತರ, buf,
					     bufsize, buflen, ಕೀಲಿ_name, &slen);
		ಆದರೆ (resp != 0)
		{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG_TERMRESPONSE
		    ಆದರೆ (resp == -1)
			LOG_TR1("Not enough characters for CSI sequence");
#ಆದರೆ_ಅಂತ್ಯ
		    ಹಿಂತಿರುಗಿ resp;
		}
	    }

	    // Check for OSC responses from terminal
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((tp[0] == ESC && len >= 2 && tp[1] == ']') || tp[0] == OSC)
	    {
handle_osc:
		ಆದರೆ (handle_osc(tp, len, ಕೀಲಿ_name, &slen) == FAIL)
		    ಹಿಂತಿರುಗಿ -1;
	    }

	    // Check for key code response from xterm,
	    // starting with <Esc>P or DCS
	    // It would only be needed with this condition:
	    //	    (check_for_codes || rcs_status.tr_progress == STATUS_SENT)
	    // Now this is always done so that DCS codes don't mess up things.
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((tp[0] == ESC && len >= 2 && tp[1] == 'P') || tp[0] == DCS)
	    {
		ಆದರೆ (handle_dcs(tp, argp, len, ಕೀಲಿ_name, &slen) == FAIL)
		    ಹಿಂತಿರುಗಿ -1;
	    }
	}

	ಆದರೆ (ಕೀಲಿ_name[0] == NUL)
	    ಮುಂದುವರಿ;	    // No match at this position, try next one

	// We only get here when we have a complete termcode match

#ಆದರೆ defined(FEAT_GUI) || defined(MSWIN)
	/*
	 * For scroll events from the GUI or MS-Windows console, fetch the
	 * pointer coordinates so that we know which window to scroll later.
	 */
	ಆದರೆ (TRUE
# ಆದರೆ defined(FEAT_GUI) && !defined(MSWIN)
		&& gui.ಒಳ_use
# endif
		&& ಕೀಲಿ_name[0] == (ಪೂರ್ಣಾಂಕ)KS_EXTRA
		&& (ಕೀಲಿ_name[1] == (ಪೂರ್ಣಾಂಕ)KE_X1MOUSE
		    || ಕೀಲಿ_name[1] == (ಪೂರ್ಣಾಂಕ)KE_X2MOUSE
		    || ಕೀಲಿ_name[1] == (ಪೂರ್ಣಾಂಕ)KE_MOUSEMOVE_XY
		    || ಕೀಲಿ_name[1] == (ಪೂರ್ಣಾಂಕ)KE_MOUSELEFT
		    || ಕೀಲಿ_name[1] == (ಪೂರ್ಣಾಂಕ)KE_MOUSERIGHT
		    || ಕೀಲಿ_name[1] == (ಪೂರ್ಣಾಂಕ)KE_MOUSEDOWN
		    || ಕೀಲಿ_name[1] == (ಪೂರ್ಣಾಂಕ)KE_MOUSEUP))
	{
	    char_u	ಬೈಟುಗಳು[6];
	    ಪೂರ್ಣಾಂಕ		ಅಂಕೆ_ಬೈಟುಗಳು = get_ಬೈಟುಗಳು_from_buf(tp + slen, ಬೈಟುಗಳು, 4);

	    ಆದರೆ (ಅಂಕೆ_ಬೈಟುಗಳು == -1)	// not enough coordinates
		ಹಿಂತಿರುಗಿ -1;
	    mouse_ಲಂಬಸಾಲು = 128 * (ಬೈಟುಗಳು[0] - ' ' - 1) + ಬೈಟುಗಳು[1] - ' ' - 1;
	    mouse_ಅಡ್ಡಸಾಲು = 128 * (ಬೈಟುಗಳು[2] - ' ' - 1) + ಬೈಟುಗಳು[3] - ' ' - 1;
	    slen += ಅಂಕೆ_ಬೈಟುಗಳು;
	    // equal to K_MOUSEMOVE
	    ಆದರೆ (ಕೀಲಿ_name[1] == (ಪೂರ್ಣಾಂಕ)KE_MOUSEMOVE_XY)
		ಕೀಲಿ_name[1] = (ಪೂರ್ಣಾಂಕ)KE_MOUSEMOVE;
	}
	ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	/*
	 * If it is a mouse click, get the coordinates.
	 */
	ಆದರೆ (ಕೀಲಿ_name[0] == KS_MOUSE
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MOUSE_GPM
		|| ಕೀಲಿ_name[0] == KS_GPM_MOUSE
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MOUSE_JSB
		|| ಕೀಲಿ_name[0] == KS_JSBTERM_MOUSE
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MOUSE_NET
		|| ಕೀಲಿ_name[0] == KS_NETTERM_MOUSE
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MOUSE_DEC
		|| ಕೀಲಿ_name[0] == KS_DEC_MOUSE
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MOUSE_PTERM
		|| ಕೀಲಿ_name[0] == KS_PTERM_MOUSE
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MOUSE_URXVT
		|| ಕೀಲಿ_name[0] == KS_URXVT_MOUSE
#ಆದರೆ_ಅಂತ್ಯ
		|| ಕೀಲಿ_name[0] == KS_SGR_MOUSE
		|| ಕೀಲಿ_name[0] == KS_SGR_MOUSE_RELEASE)
	{
	    ಆದರೆ (check_termcode_mouse(tp, &slen, ಕೀಲಿ_name, modifiers_ಆರಂಭ, idx,
							     &modifiers) == -1)
		ಹಿಂತಿರುಗಿ -1;
	}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
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
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಕೀಲಿ_name[0] == (ಪೂರ್ಣಾಂಕ)KS_MENU)
	{
	    long_u	val;
	    ಪೂರ್ಣಾಂಕ		ಅಂಕೆ_ಬೈಟುಗಳು = get_long_from_buf(tp + slen, &val);

	    ಆದರೆ (ಅಂಕೆ_ಬೈಟುಗಳು == -1)
		ಹಿಂತಿರುಗಿ -1;
	    current_menu = (vimmenu_T *)val;
	    slen += ಅಂಕೆ_ಬೈಟುಗಳು;

	    // The menu may have been deleted right after it was used, check
	    // for that.
	    ಆದರೆ (check_menu_ಸೂಚಕ(ಬೇರು_menu, current_menu) == FAIL)
	    {
		ಕೀಲಿ_name[0] = KS_EXTRA;
		ಕೀಲಿ_name[1] = (ಪೂರ್ಣಾಂಕ)KE_IGNORE;
	    }
	}
# endif
# ifdef FEAT_GUI_TABLINE
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಕೀಲಿ_name[0] == (ಪೂರ್ಣಾಂಕ)KS_TABLINE)
	{
	    // Selecting tabline tab or using its menu.
	    char_u	ಬೈಟುಗಳು[6];
	    ಪೂರ್ಣಾಂಕ		ಅಂಕೆ_ಬೈಟುಗಳು = get_ಬೈಟುಗಳು_from_buf(tp + slen, ಬೈಟುಗಳು, 1);

	    ಆದರೆ (ಅಂಕೆ_ಬೈಟುಗಳು == -1)
		ಹಿಂತಿರುಗಿ -1;
	    current_tab = (ಪೂರ್ಣಾಂಕ)ಬೈಟುಗಳು[0];
	    ಆದರೆ (current_tab == 255)	// -1 in a byte gives 255
		current_tab = -1;
	    slen += ಅಂಕೆ_ಬೈಟುಗಳು;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಕೀಲಿ_name[0] == (ಪೂರ್ಣಾಂಕ)KS_TABMENU)
	{
	    // Selecting tabline tab or using its menu.
	    char_u	ಬೈಟುಗಳು[6];
	    ಪೂರ್ಣಾಂಕ		ಅಂಕೆ_ಬೈಟುಗಳು = get_ಬೈಟುಗಳು_from_buf(tp + slen, ಬೈಟುಗಳು, 2);

	    ಆದರೆ (ಅಂಕೆ_ಬೈಟುಗಳು == -1)
		ಹಿಂತಿರುಗಿ -1;
	    current_tab = (ಪೂರ್ಣಾಂಕ)ಬೈಟುಗಳು[0];
	    current_tabmenu = (ಪೂರ್ಣಾಂಕ)ಬೈಟುಗಳು[1];
	    slen += ಅಂಕೆ_ಬೈಟುಗಳು;
	}
# endif
# ifndef USE_ON_FLY_SCROLL
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಕೀಲಿ_name[0] == (ಪೂರ್ಣಾಂಕ)KS_VER_SCROLLBAR)
	{
	    long_u	val;
	    char_u	ಬೈಟುಗಳು[6];
	    ಪೂರ್ಣಾಂಕ		ಅಂಕೆ_ಬೈಟುಗಳು;

	    // Get the last scrollbar event in the queue of the same type
	    j = 0;
	    ಗಾಗಿ (i = 0; tp[j] == CSI && tp[j + 1] == KS_VER_SCROLLBAR
						     && tp[j + 2] != NUL; ++i)
	    {
		j += 3;
		ಅಂಕೆ_ಬೈಟುಗಳು = get_ಬೈಟುಗಳು_from_buf(tp + j, ಬೈಟುಗಳು, 1);
		ಆದರೆ (ಅಂಕೆ_ಬೈಟುಗಳು == -1)
		    ನಿಲ್ಲಿಸು;
		ಆದರೆ (i == 0)
		    current_scrollbar = (ಪೂರ್ಣಾಂಕ)ಬೈಟುಗಳು[0];
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (current_scrollbar != (ಪೂರ್ಣಾಂಕ)ಬೈಟುಗಳು[0])
		    ನಿಲ್ಲಿಸು;
		j += ಅಂಕೆ_ಬೈಟುಗಳು;
		ಅಂಕೆ_ಬೈಟುಗಳು = get_long_from_buf(tp + j, &val);
		ಆದರೆ (ಅಂಕೆ_ಬೈಟುಗಳು == -1)
		    ನಿಲ್ಲಿಸು;
		scrollbar_ಮೌಲ್ಯ = val;
		j += ಅಂಕೆ_ಬೈಟುಗಳು;
		slen = j;
	    }
	    ಆದರೆ (i == 0)		// not enough characters to make one
		ಹಿಂತಿರುಗಿ -1;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಕೀಲಿ_name[0] == (ಪೂರ್ಣಾಂಕ)KS_HOR_SCROLLBAR)
	{
	    long_u	val;
	    ಪೂರ್ಣಾಂಕ		ಅಂಕೆ_ಬೈಟುಗಳು;

	    // Get the last horiz. scrollbar event in the queue
	    j = 0;
	    ಗಾಗಿ (i = 0; tp[j] == CSI && tp[j + 1] == KS_HOR_SCROLLBAR
						     && tp[j + 2] != NUL; ++i)
	    {
		j += 3;
		ಅಂಕೆ_ಬೈಟುಗಳು = get_long_from_buf(tp + j, &val);
		ಆದರೆ (ಅಂಕೆ_ಬೈಟುಗಳು == -1)
		    ನಿಲ್ಲಿಸು;
		scrollbar_ಮೌಲ್ಯ = val;
		j += ಅಂಕೆ_ಬೈಟುಗಳು;
		slen = j;
	    }
	    ಆದರೆ (i == 0)		// not enough characters to make one
		ಹಿಂತಿರುಗಿ -1;
	}
# endif // !USE_ON_FLY_SCROLL
#ಆದರೆ_ಅಂತ್ಯ // FEAT_GUI

#ಆದರೆ defined(UNIX) || defined(VMS)
	/*
	 * Handle FocusIn/FocusOut event sequences reported by XTerm.
	 * (CSI I/CSI O)
	 */
	ಆದರೆ (ಕೀಲಿ_name[0] == KS_EXTRA
# ifdef FEAT_GUI
		&& !gui.ಒಳ_use
# endif
	    )
	{
	    ಆದರೆ (ಕೀಲಿ_name[1] == KE_FOCUSGAINED)
	    {
		ಆದರೆ (!focus_state)
		{
		    ui_focus_change(TRUE);
		    did_cursorhold = TRUE;
		    focus_state = TRUE;
		}
		ಕೀಲಿ_name[1] = (ಪೂರ್ಣಾಂಕ)KE_IGNORE;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಕೀಲಿ_name[1] == KE_FOCUSLOST)
	    {
		ಆದರೆ (focus_state)
		{
		    ui_focus_change(FALSE);
		    did_cursorhold = TRUE;
		    focus_state = FALSE;
		}
		ಕೀಲಿ_name[1] = (ಪೂರ್ಣಾಂಕ)KE_IGNORE;
	    }
	}
#ಆದರೆ_ಅಂತ್ಯ

	/*
	 * Change <xHome> to <Home>, <xUp> to <Up>, etc.
	 */
	ಕೀಲಿ = handle_x_ಕೀಲಿಗಳು(TERMCAP2_ಕೀಲಿ(ಕೀಲಿ_name[0], ಕೀಲಿ_name[1]));

	// Add any modifier codes to our string.
	new_slen = modifiers2keycode(modifiers, &ಕೀಲಿ, string);

	// Finally, add the special key code to our string
	ಕೀಲಿ_name[0] = KEY2TERMCAP0(ಕೀಲಿ);
	ಕೀಲಿ_name[1] = KEY2TERMCAP1(ಕೀಲಿ);
	ಆದರೆ (ಕೀಲಿ_name[0] == KS_ಕೀಲಿ)
	{
	    // from ":set <M-b>=xx"
	    ಆದರೆ (has_mbyte)
		new_slen += (*mb_char2bytes)(ಕೀಲಿ_name[1], string + new_slen);
	    ಇಲ್ಲದಿದ್ದರೆ
		string[new_slen++] = ಕೀಲಿ_name[1];
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (osc_state.processing ||
		(new_slen == 0
		 && ಕೀಲಿ_name[0] == KS_EXTRA
		 && ಕೀಲಿ_name[1] == KE_IGNORE))
	{
	    // Do not put K_IGNORE into the buffer, do return KEYLEN_REMOVED
	    // to indicate what happened.
	    retval = KEYLEN_REMOVED;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    string[new_slen++] = K_SPECIAL;
	    string[new_slen++] = ಕೀಲಿ_name[0];
	    string[new_slen++] = ಕೀಲಿ_name[1];
	}
	ಆದರೆ (put_string_ಒಳ_typebuf(ಸ್ಥಾನಾಂತರ, slen, string, new_slen,
						 buf, bufsize, buflen) == FAIL)
	    ಹಿಂತಿರುಗಿ -1;
	ಹಿಂತಿರುಗಿ retval == 0 ? (len + new_slen - slen + ಸ್ಥಾನಾಂತರ) : retval;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMRESPONSE
    LOG_TR1("normal character");
#ಆದರೆ_ಅಂತ್ಯ

    ಹಿಂತಿರುಗಿ 0;			    // no match found
}

#ಆದರೆ defined(FEAT_TERMINAL) && defined(FEAT_TERMRESPONSE)
/*
 * Get the text foreground color, if known.
 */
    ಶೂನ್ಯ
ಪದ_get_fg_color(char_u *r, char_u *g, char_u *b)
{
    ಆದರೆ (rfg_ಸ್ಥಿತಿ.tr_ಪ್ರಗತಿ != ಸ್ಥಿತಿ_GOT)
	ಹಿಂತಿರುಗಿ;

    *r = fg_r;
    *g = fg_g;
    *b = fg_b;
}
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMRESPONSE
/*
 * Get the text background color, if known.  Returns OK when the OSC 11
 * response has been parsed and *r, *g, *b are populated; FAIL otherwise.
 */
    ಪೂರ್ಣಾಂಕ
ಪದ_get_bg_color(char_u *r, char_u *g, char_u *b)
{
    ಆದರೆ (rbg_ಸ್ಥಿತಿ.tr_ಪ್ರಗತಿ != ಸ್ಥಿತಿ_GOT)
	ಹಿಂತಿರುಗಿ FAIL;

    *r = bg_r;
    *g = bg_g;
    *b = bg_b;
    ಹಿಂತಿರುಗಿ OK;
}
#ಆದರೆ_ಅಂತ್ಯ

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
    scid_T	sid_arg ಬಳಸದ,	// script ID to use for <SID>,
				// or 0 to use current_sctx
    ಪೂರ್ಣಾಂಕ		flags,
    ಪೂರ್ಣಾಂಕ		*did_simplify)
{
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		slen;
    ಪೂರ್ಣಾಂಕ		ಕೀಲಿ;
    size_t	dlen = 0;
    char_u	*src;
    ಪೂರ್ಣಾಂಕ		do_backslash;	// backslash is a special character
    ಪೂರ್ಣಾಂಕ		do_special;	// recognize <> key codes
    ಪೂರ್ಣಾಂಕ		do_ಕೀಲಿ_ಸಂಕೇತ;	// recognize raw key codes
    char_u	*ಫಲಿತಾಂಶ;	// buffer for resulting string
    garray_T	ga;

    do_backslash = (vim_strchr(p_cpo, CPO_BSLASH) == ಖಾಲಿ);
    do_special = (vim_strchr(p_cpo, CPO_SPECI) == ಖಾಲಿ)
						  || (flags & REPTERM_SPECIAL);
    do_ಕೀಲಿ_ಸಂಕೇತ = (vim_strchr(p_cpo, CPO_KEYCODE) == ಖಾಲಿ);
    src = from;

    /*
     * Allocate space for the translation.  Worst case a single character is
     * replaced by 6 bytes (shifted special key), plus a NUL at the end.
     * In the rare case more might be needed ga_grow() must be called again.
     */
    ga_init2(&ga, 1L, 100);
    ಆದರೆ (ga_grow(&ga, (ಪೂರ್ಣಾಂಕ)(STRLEN(src) * 6 + 1)) == FAIL) // out of memory
    {
	*bufp = ಖಾಲಿ;
	ಹಿಂತಿರುಗಿ from;
    }
    ಫಲಿತಾಂಶ = ga.ga_ದತ್ತಾಂಶ;

    /*
     * Check for #n at start only: function key n
     */
    ಆದರೆ ((flags & REPTERM_FROM_PART) && src[0] == '#' && VIM_ISDIGIT(src[1]))
    {
	ಫಲಿತಾಂಶ[dlen++] = K_SPECIAL;
	ಫಲಿತಾಂಶ[dlen++] = 'k';
	ಆದರೆ (src[1] == '0')
	    ಫಲಿತಾಂಶ[dlen++] = ';';	// #0 is F10 is "k;"
	ಇಲ್ಲದಿದ್ದರೆ
	    ಫಲಿತಾಂಶ[dlen++] = src[1];	// #3 is F3 is "k3"
	src += 2;
    }

    /*
     * Copy each byte from *from to result[dlen]
     */
    ಆಗ (*src != NUL)
    {
	/*
	 * If 'cpoptions' does not contain '<', check for special key codes,
	 * like "<C-S-LeftMouse>"
	 */
	ಆದರೆ (do_special && ((flags & REPTERM_DO_LT)
					      || STRNCMP(src, "<lt>", 4) != 0))
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	    /*
	     * Change <SID>Func to K_SNR <script-nr> _Func.  This name is used
	     * for script-local user functions.
	     * (room: 5 * 6 = 30 bytes; needed: 3 + <nr> + 1 <= 14)
	     * Also change <SID>name.Func to K_SNR <import-script-nr> _Func.
	     * Only if "name" is recognized as an import.
	     */
	    ಆದರೆ (STRNICMP(src, "<SID>", 5) == 0)
	    {
		ಆದರೆ (sid_arg < 0 || (sid_arg == 0 && current_sctx.sc_sid <= 0))
		    emsg(_(e_using_sid_not_ಒಳ_script_context));
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    char_u  *dot;
		    ದೀರ್ಘ    sid = sid_arg != 0 ? sid_arg : current_sctx.sc_sid;

		    src += 5;
		    ಆದರೆ (ಒಳ_vim9script()
				       && (dot = vim_strchr(src, '.')) != ಖಾಲಿ)
		    {
			imported_T *imp = ಕಂಡುಹಿಡಿ_imported(src, dot - src, FALSE);

			ಆದರೆ (imp != ಖಾಲಿ)
			{
			    scriptitem_T    *si = SCRIPT_ಅಂಶ(imp->imp_sid);
			    size_t	    len;

			    src = dot + 1;
			    ಆದರೆ (si->sn_autoload_ಪೂರ್ವಪ್ರತ್ಯಯ != ಖಾಲಿ)
			    {
				// Turn "<SID>name.Func"
				// into "scriptname#Func".
				len = STRLEN(si->sn_autoload_ಪೂರ್ವಪ್ರತ್ಯಯ);
				ಆದರೆ (ga_grow(&ga,
				     (ಪೂರ್ಣಾಂಕ)(STRLEN(src) * 6 + len + 1)) == FAIL)
				{
				    ga_clear(&ga);
				    *bufp = ಖಾಲಿ;
				    ಹಿಂತಿರುಗಿ from;
				}
				ಫಲಿತಾಂಶ = ga.ga_ದತ್ತಾಂಶ;
				STRCPY(ಫಲಿತಾಂಶ + dlen, si->sn_autoload_ಪೂರ್ವಪ್ರತ್ಯಯ);
				dlen += len;
				ಮುಂದುವರಿ;
			    }
			    sid = imp->imp_sid;
			}
		    }

		    ಫಲಿತಾಂಶ[dlen++] = K_SPECIAL;
		    ಫಲಿತಾಂಶ[dlen++] = (ಪೂರ್ಣಾಂಕ)KS_EXTRA;
		    ಫಲಿತಾಂಶ[dlen++] = (ಪೂರ್ಣಾಂಕ)KE_SNR;
		    sprintf((ಅಕ್ಷರ *)ಫಲಿತಾಂಶ + dlen, "%ld", sid);
		    dlen += STRLEN(ಫಲಿತಾಂಶ + dlen);
		    ಫಲಿತಾಂಶ[dlen++] = '_';
		    ಮುಂದುವರಿ;
		}
	    }
#ಆದರೆ_ಅಂತ್ಯ
	    ಪೂರ್ಣಾಂಕ fsk_flags = FSK_KEYCODE
			| ((flags & REPTERM_NO_SIMPLIFY) ? 0 : FSK_SIMPLIFY)
			| ((flags & REPTERM_FROM_PART) ? FSK_FROM_PART : 0);
	    slen = trans_special(&src, ಫಲಿತಾಂಶ + dlen, fsk_flags,
							   TRUE, did_simplify);
	    ಆದರೆ (slen > 0)
	    {
		dlen += slen;
		ಮುಂದುವರಿ;
	    }
	}

	/*
	 * If 'cpoptions' does not contain 'k', see if it's an actual key-code.
	 * Note that this is also checked after replacing the <> form.
	 * Single character codes are NOT replaced (e.g. ^H or DEL), because
	 * it could be a character in the file.
	 */
	ಆದರೆ (do_ಕೀಲಿ_ಸಂಕೇತ)
	{
	    ಪೂರ್ಣಾಂಕ len;
	    i = ಕಂಡುಹಿಡಿ_ಪದ_bykeys(src, &len);
	    ಆದರೆ (i >= 0)
	    {
		ಫಲಿತಾಂಶ[dlen++] = K_SPECIAL;
		ಫಲಿತಾಂಶ[dlen++] = termcodes[i].name[0];
		ಫಲಿತಾಂಶ[dlen++] = termcodes[i].name[1];
		src += len;
		// If terminal code matched, continue after it.
		ಮುಂದುವರಿ;
	    }
	}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	ಆದರೆ (do_special)
	{
	    char_u	*p, *s, len;

	    /*
	     * Replace <Leader> by the value of "mapleader".
	     * Replace <LocalLeader> by the value of "maplocalleader".
	     * If "mapleader" or "maplocalleader" isn't set use a backslash.
	     */
	    ಆದರೆ (STRNICMP(src, "<Leader>", 8) == 0)
	    {
		len = 8;
		p = get_var_ಮೌಲ್ಯ((char_u *)"g:mapleader");
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNICMP(src, "<LocalLeader>", 13) == 0)
	    {
		len = 13;
		p = get_var_ಮೌಲ್ಯ((char_u *)"g:maplocalleader");
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		len = 0;
		p = ಖಾಲಿ;
	    }
	    ಆದರೆ (len != 0)
	    {
		// Allow up to 8 * 6 characters for "mapleader".
		ಆದರೆ (p == ಖಾಲಿ || *p == NUL || STRLEN(p) > 8 * 6)
		    s = (char_u *)"\\";
		ಇಲ್ಲದಿದ್ದರೆ
		    s = p;
		ಆಗ (*s != NUL)
		    ಫಲಿತಾಂಶ[dlen++] = *s++;
		src += len;
		ಮುಂದುವರಿ;
	    }
	}
#ಆದರೆ_ಅಂತ್ಯ

	/*
	 * Remove CTRL-V and ignore the next character.
	 * For "from" side the CTRL-V at the end is included, for the "to"
	 * part it is removed.
	 * If 'cpoptions' does not contain 'B', also accept a backslash.
	 */
	ಕೀಲಿ = *src;
	ಆದರೆ (ಕೀಲಿ == Ctrl_V || (do_backslash && ಕೀಲಿ == '\\'))
	{
	    ++src;				// skip CTRL-V or backslash
	    ಆದರೆ (*src == NUL)
	    {
		ಆದರೆ (flags & REPTERM_FROM_PART)
		    ಫಲಿತಾಂಶ[dlen++] = ಕೀಲಿ;
		ನಿಲ್ಲಿಸು;
	    }
	}

	// skip multibyte char correctly
	ಗಾಗಿ (i = (*mb_ptr2len)(src); i > 0; --i)
	{
	    /*
	     * If the character is K_SPECIAL, replace it with K_SPECIAL
	     * KS_SPECIAL KE_FILLER.
	     * If compiled with the GUI replace CSI with K_CSI.
	     */
	    ಆದರೆ (*src == K_SPECIAL)
	    {
		ಫಲಿತಾಂಶ[dlen++] = K_SPECIAL;
		ಫಲಿತಾಂಶ[dlen++] = KS_SPECIAL;
		ಫಲಿತಾಂಶ[dlen++] = KE_FILLER;
	    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*src == CSI)
	    {
		ಫಲಿತಾಂಶ[dlen++] = K_SPECIAL;
		ಫಲಿತಾಂಶ[dlen++] = KS_EXTRA;
		ಫಲಿತಾಂಶ[dlen++] = (ಪೂರ್ಣಾಂಕ)KE_CSI;
	    }
#ಆದರೆ_ಅಂತ್ಯ
	    ಇಲ್ಲದಿದ್ದರೆ
		ಫಲಿತಾಂಶ[dlen++] = *src;
	    ++src;
	}
    }
    ಫಲಿತಾಂಶ[dlen] = NUL;

    /*
     * Copy the new string to allocated memory.
     * If this fails, just return from.
     */
    ಆದರೆ ((*bufp = vim_strsave(ಫಲಿತಾಂಶ)) != ಖಾಲಿ)
	from = *bufp;
    vim_free(ಫಲಿತಾಂಶ);
    ಹಿಂತಿರುಗಿ from;
}

/*
 * Find a termcode with keys 'src' (must be NUL terminated).
 * Return the index in termcodes[], or -1 if not found.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಕಂಡುಹಿಡಿ_ಪದ_bykeys(char_u *src, ಪೂರ್ಣಾಂಕ *matchlen)
{
    ಪೂರ್ಣಾಂಕ		i, j;
    ಪೂರ್ಣಾಂಕ		len = (ಪೂರ್ಣಾಂಕ)STRLEN(src);
    ಪೂರ್ಣಾಂಕ         ಸಿಕ್ಕಿತು = -1;
    // Don't return a match for a single character
    ಪೂರ್ಣಾಂಕ         foundlen = 1;
    ಪೂರ್ಣಾಂಕ         slen, modslen;
    ಪೂರ್ಣಾಂಕ         thislen;

    // Most input bytes cannot start a terminal code.  Reuse the same leader
    // table as check_termcode() to avoid scanning termcodes[] unnecessarily.
    ಆದರೆ (need_gather)
	gather_termleader();
    ಆದರೆ (*src == NUL || vim_strchr(termleader, *src) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ -1;

    // find longest match
    // borrows part of check_termcode
    ಗಾಗಿ (i = 0; i < tc_len; ++i)
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
	ಆದರೆ (modslen > 0)
	{
	    ಆದರೆ (len > modslen
			&& STRNCMP(termcodes[i].ಸಂಕೇತ_2, src, (size_t)modslen) == 0)
	    {
		thislen = 0;

		ಆದರೆ (src[modslen] == termcodes[i].ಸಂಕೇತ_2[slen - 1])
		    // no modifiers
		    thislen = modslen + 1;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (src[modslen] != ';' && modslen == slen - 3)
		    // no match for "code;*X" with "code;"
		    ಮುಂದುವರಿ;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (termcodes[i].ಸಂಕೇತ_2[modslen] == '@'
				&& (src[modslen] != '1'
					    || src[modslen + 1] != ';'))
		    // no match for "<Esc>[@" with "<Esc>[1;"
		    ಮುಂದುವರಿ;
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    // Skip over the digits, the final char must
		    // follow. URXVT can use a negative value, thus
		    // also accept '-'.
		    ಗಾಗಿ (j = slen - 2; j < len && (ಸುರಕ್ಷಿತ_isdigit(src[j])
			       || src[j] == '-' || src[j] == ';'); ++j)
			;
		    ++j;
		    ಆದರೆ (len < j)	// got a partial sequence
			ಮುಂದುವರಿ;
		    ಆದರೆ (src[j - 1] != termcodes[i].ಸಂಕೇತ_2[slen - 1])
			ಮುಂದುವರಿ;	// no match

		    thislen = j;
		}

		ಆದರೆ (thislen > foundlen)
		{
		    ಸಿಕ್ಕಿತು = i;
		    foundlen = thislen;
		}
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (slen > foundlen && len >= slen
			&& STRNCMP(termcodes[i].ಸಂಕೇತ_2, src, (size_t)slen) == 0)
	    {
		ಸಿಕ್ಕಿತು = i;
		foundlen = slen;
	    }
	}
    }

    ಆದರೆ (matchlen != ಖಾಲಿ && ಸಿಕ್ಕಿತು >= 0)
	*matchlen = foundlen;

    ಹಿಂತಿರುಗಿ ಸಿಕ್ಕಿತು;
}

/*
 * Gather the first characters in the terminal key codes into a string.
 * Used to speed up check_termcode().
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
gather_termleader(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ	    i;
    ಪೂರ್ಣಾಂಕ	    len = 0;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    ಆದರೆ (gui.ಒಳ_use)
	termleader[len++] = CSI;    // the GUI codes are not in termcodes[]
#ಆದರೆ_ಅಂತ್ಯ

    // Querying cell size uses CSI 14 t
#ಆದರೆ defined(MSWIN) && (defined(FEAT_IMAGE) || defined(FEAT_EVAL))
# ifdef FEAT_GUI
    ಆದರೆ (!gui.ಒಳ_use)
# endif
    {
	termleader[len++] = ESC;
# ifdef FEAT_GUI
	ಆದರೆ (!gui.ಒಳ_use)
# endif
	    termleader[len++] = CSI;
    }
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMRESPONSE
    ಆದರೆ (check_for_codes || *T_CRS != NUL)
	termleader[len++] = DCS;    // the termcode response starts with DCS
				    // in 8-bit mode
#ಆದರೆ_ಅಂತ್ಯ
    termleader[len] = NUL;

    ಗಾಗಿ (i = 0; i < tc_len; ++i)
	ಆದರೆ (vim_strchr(termleader, termcodes[i].ಸಂಕೇತ_2[0]) == ಖಾಲಿ)
	{
	    termleader[len++] = termcodes[i].ಸಂಕೇತ_2[0];
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
show_termcodes(ಪೂರ್ಣಾಂಕ flags)
{
    ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು;
    ಪೂರ್ಣಾಂಕ		*ವಸ್ತುಗಳು;
    ಪೂರ್ಣಾಂಕ		ಅಂಶ_ಎಣಿಕೆ;
    ಪೂರ್ಣಾಂಕ		ಸತತ;
    ಪೂರ್ಣಾಂಕ		ಅಡ್ಡಸಾಲು, ಅಡ್ಡಸಾಲುಗಳು_2;
    ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		len;

#ವ್ಯಾಖ್ಯಾನಿಸು INC3 27	    // try to make three columns
#ವ್ಯಾಖ್ಯಾನಿಸು INC2 40	    // try to make two columns
#ವ್ಯಾಖ್ಯಾನಿಸು GAP 2	    // spaces between columns

    ಆದರೆ (tc_len == 0)	    // no terminal codes (must be GUI)
	ಹಿಂತಿರುಗಿ;
    ವಸ್ತುಗಳು = ALLOC_MULT(ಪೂರ್ಣಾಂಕ, tc_len);
    ಆದರೆ (ವಸ್ತುಗಳು == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    // Highlight title
    ಸಂದೇಶ_puts_title(_("\n--- Terminal keys ---"));

    /*
     * Do the loop three times:
     * 1. display the short items (non-strings and short strings)
     * 2. display the medium items (medium length strings)
     * 3. display the long items (remaining strings)
     * When "flags" has OPT_ONECOLUMN do everything in 3.
     */
    ಗಾಗಿ (ಸತತ = (flags & OPT_ONECOLUMN) ? 3 : 1; ಸತತ <= 3 && !got_int; ++ಸತತ)
    {
	/*
	 * collect the items in items[]
	 */
	ಅಂಶ_ಎಣಿಕೆ = 0;
	ಗಾಗಿ (i = 0; i < tc_len; i++)
	{
	    len = show_one_termcode(termcodes[i].name,
						    termcodes[i].ಸಂಕೇತ_2, FALSE);
	    ಆದರೆ ((flags & OPT_ONECOLUMN) ||
		    (len <= INC3 - GAP ? ಸತತ == 1
			: len <= INC2 - GAP ? ಸತತ == 2
			: ಸತತ == 3))
		ವಸ್ತುಗಳು[ಅಂಶ_ಎಣಿಕೆ++] = i;
	}

	/*
	 * display the items
	 */
	ಆದರೆ (ಸತತ <= 2)
	{
	    ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = (Columns + GAP) / (ಸತತ == 1 ? INC3 : INC2);
	    ಆದರೆ (ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ == 0)
		ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = 1;
	    ಅಡ್ಡಸಾಲುಗಳು_2 = (ಅಂಶ_ಎಣಿಕೆ + ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ - 1) / ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;
	}
	ಇಲ್ಲದಿದ್ದರೆ	// run == 3
	    ಅಡ್ಡಸಾಲುಗಳು_2 = ಅಂಶ_ಎಣಿಕೆ;
	ಗಾಗಿ (ಅಡ್ಡಸಾಲು = 0; ಅಡ್ಡಸಾಲು < ಅಡ್ಡಸಾಲುಗಳು_2 && !got_int; ++ಅಡ್ಡಸಾಲು)
	{
	    ಸಂದೇಶ_putchar('\n');			// go to next line
	    ಆದರೆ (got_int)			// 'q' typed in more
		ನಿಲ್ಲಿಸು;
	    ಲಂಬಸಾಲು = 0;
	    ಗಾಗಿ (i = ಅಡ್ಡಸಾಲು; i < ಅಂಶ_ಎಣಿಕೆ; i += ಅಡ್ಡಸಾಲುಗಳು_2)
	    {
		ಸಂದೇಶ_ಲಂಬಸಾಲು = ಲಂಬಸಾಲು;			// make columns
		show_one_termcode(termcodes[ವಸ್ತುಗಳು[i]].name,
					      termcodes[ವಸ್ತುಗಳು[i]].ಸಂಕೇತ_2, TRUE);
		ಆದರೆ (ಸತತ == 2)
		    ಲಂಬಸಾಲು += INC2;
		ಇಲ್ಲದಿದ್ದರೆ
		    ಲಂಬಸಾಲು += INC3;
	    }
	    ಹೊರಹೋಗು_flush();
	    ui_breakcheck();
	}
    }
    vim_free(ವಸ್ತುಗಳು);
}

/*
 * Show one termcode entry.
 * Output goes into IObuff[]
 */
    ಪೂರ್ಣಾಂಕ
show_one_termcode(char_u *name, char_u *ಸಂಕೇತ_2, ಪೂರ್ಣಾಂಕ printit)
{
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		len;

    ಆದರೆ (name[0] > '~')
    {
	IObuff[0] = ' ';
	IObuff[1] = ' ';
	IObuff[2] = ' ';
	IObuff[3] = ' ';
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	IObuff[0] = 't';
	IObuff[1] = '_';
	IObuff[2] = name[0];
	IObuff[3] = name[1];
    }
    IObuff[4] = ' ';

    p = get_special_ಕೀಲಿ_name(TERMCAP2_ಕೀಲಿ(name[0], name[1]), 0);
    ಆದರೆ (p[1] != 't')
	STRCPY(IObuff + 5, p);
    ಇಲ್ಲದಿದ್ದರೆ
	IObuff[5] = NUL;
    len = (ಪೂರ್ಣಾಂಕ)STRLEN(IObuff);
    ಮಾಡು
	IObuff[len++] = ' ';
    ಆಗ (len < 17);
    IObuff[len] = NUL;
    ಆದರೆ (ಸಂಕೇತ_2 == ಖಾಲಿ)
	len += 4;
    ಇಲ್ಲದಿದ್ದರೆ
	len += vim_strsize(ಸಂಕೇತ_2);

    ಆದರೆ (printit)
    {
	ಸಂದೇಶ_puts((ಅಕ್ಷರ *)IObuff);
	ಆದರೆ (ಸಂಕೇತ_2 == ಖಾಲಿ)
	    ಸಂದೇಶ_puts("NULL");
	ಇಲ್ಲದಿದ್ದರೆ
	    ಸಂದೇಶ_outtrans(ಸಂಕೇತ_2);
    }
    ಹಿಂತಿರುಗಿ len;
}

#ಆದರೆ defined(FEAT_TERMRESPONSE)
/*
 * For Xterm >= 140 compiled with OPT_TCAP_QUERY: Obtain the actually used
 * termcap codes from the terminal itself.
 * We get them one by one to avoid a very long response string.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ xt_ಸೂಚ್ಯಂಕ_ಒಳ = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ xt_ಸೂಚ್ಯಂಕ_ಹೊರಹೋಗು = 0;

    ಸ್ಥಿರ ಶೂನ್ಯ
req_codes_from_ಪದ(ಶೂನ್ಯ)
{
    xt_ಸೂಚ್ಯಂಕ_ಹೊರಹೋಗು = 0;
    xt_ಸೂಚ್ಯಂಕ_ಒಳ = 0;
    req_ಇನ್ನಷ್ಟು_codes_from_ಪದ();
}

    ಸ್ಥಿರ ಶೂನ್ಯ
req_ಇನ್ನಷ್ಟು_codes_from_ಪದ(ಶೂನ್ಯ)
{
    ಅಕ್ಷರ	buf[32];  // extra size to shut up LGTM
    ಪೂರ್ಣಾಂಕ		old_idx = xt_ಸೂಚ್ಯಂಕ_ಹೊರಹೋಗು;

    // Don't do anything when going to exit.
    ಆದರೆ (exiting)
	ಹಿಂತಿರುಗಿ;

    // Send up to 10 more requests out than we received.  Avoid sending too
    // many, there can be a buffer overflow somewhere.
    ಆಗ (xt_ಸೂಚ್ಯಂಕ_ಹೊರಹೋಗು < xt_ಸೂಚ್ಯಂಕ_ಒಳ + 10 && ಕೀಲಿ_ಹೆಸರುಗಳು[xt_ಸೂಚ್ಯಂಕ_ಹೊರಹೋಗು] != ಖಾಲಿ)
    {
	ಅಕ್ಷರ *ಕೀಲಿ_name = ಕೀಲಿ_ಹೆಸರುಗಳು[xt_ಸೂಚ್ಯಂಕ_ಹೊರಹೋಗು];

	MAY_WANT_ಗಮ್ಯ_LOG_THIS;
	LOG_TRN("Requesting XT %d: %s", xt_ಸೂಚ್ಯಂಕ_ಹೊರಹೋಗು, ಕೀಲಿ_name);
	ಆದರೆ (ಕೀಲಿ_name[2] != NUL)
	    sprintf(buf, "\033P+q%02x%02x%02x\033\\", ಕೀಲಿ_name[0], ಕೀಲಿ_name[1], ಕೀಲಿ_name[2]);
	ಇಲ್ಲದಿದ್ದರೆ
	    sprintf(buf, "\033P+q%02x%02x\033\\", ಕೀಲಿ_name[0], ಕೀಲಿ_name[1]);
	ಹೊರಹೋಗು_str_nf((char_u *)buf);
	++xt_ಸೂಚ್ಯಂಕ_ಹೊರಹೋಗು;
    }

    // Send the codes out right away.
    ಆದರೆ (xt_ಸೂಚ್ಯಂಕ_ಹೊರಹೋಗು != old_idx)
	ಹೊರಹೋಗು_flush();
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
got_ಸಂಕೇತ_from_ಪದ(char_u *ಸಂಕೇತ_2, ಪೂರ್ಣಾಂಕ len)
{
# define XT_LEN 100
    char_u	name[4];
    char_u	str[XT_LEN];
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		j = 0;
    ಪೂರ್ಣಾಂಕ		c;

    // A '1' means the code is supported, a '0' means it isn't.
    // If it is supported, there must be a '=' following
    // When half the length is > XT_LEN we can't use it.
    ಆದರೆ (ಸಂಕೇತ_2[0] == '1' && (ಸಂಕೇತ_2[7] == '=' || ಸಂಕೇತ_2[9] == '=') && len / 2 < XT_LEN)
    {
	// Get the name from the response and find it in the table.
	name[0] = hexhex2nr(ಸಂಕೇತ_2 + 3);
	name[1] = hexhex2nr(ಸಂಕೇತ_2 + 5);
	ಆದರೆ (ಸಂಕೇತ_2[9] == '=')
	    name[2] = hexhex2nr(ಸಂಕೇತ_2 + 7);
	ಇಲ್ಲದಿದ್ದರೆ
	    name[2] = NUL;
	name[3] = NUL;
	ಗಾಗಿ (i = 0; ಕೀಲಿ_ಹೆಸರುಗಳು[i] != ಖಾಲಿ; ++i)
	{
	    ಆದರೆ (STRCMP(ಕೀಲಿ_ಹೆಸರುಗಳು[i], name) == 0)
	    {
		xt_ಸೂಚ್ಯಂಕ_ಒಳ = i;
		ನಿಲ್ಲಿಸು;
	    }
	}

	LOG_TRN("Received XT %d: %s", xt_ಸೂಚ್ಯಂಕ_ಒಳ, (ಅಕ್ಷರ *)name);

	ಆದರೆ (ಕೀಲಿ_ಹೆಸರುಗಳು[i] != ಖಾಲಿ)
	{
	    i = (ಸಂಕೇತ_2[7] == '=') ? 8 : 10;
	    ಗಾಗಿ (; (c = hexhex2nr(ಸಂಕೇತ_2 + i)) >= 0; i += 2)
		str[j++] = c;
	    str[j] = NUL;
	    ಆದರೆ (name[0] == 'C' && name[1] == 'o')
	    {
		// Color count is not a key code.
		ಪೂರ್ಣಾಂಕ val = atoi((ಅಕ್ಷರ *)str);
# ಆದರೆ defined(FEAT_EVAL)
		ಆದರೆ (val == t_colors)
		    ch_log(ಖಾಲಿ, "got_code_from_term(Co): no change (%d)", val);
		ಇಲ್ಲದಿದ್ದರೆ
		    ch_log(ಖಾಲಿ,
			       "got_code_from_term(Co): changed from %d to %d",
								t_colors, val);
# endif
		may_adjust_color_ಎಣಿಕೆ(val);
	    }
# ಆದರೆ 0
#  ifdef FEAT_TERMGUICOLORS
	    // when RGB result comes back, it is supported when the result contains an '='
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (name[0] == 'R' && name[1] == 'G' && name[2] == 'B' && ಸಂಕೇತ_2[9] == '=')
	    {
		ಪೂರ್ಣಾಂಕ val = atoi((ಅಕ್ಷರ *)str);
		// only enable it, if termguicolors hasn't been set yet and
		// there are 8 bits per color channel
		ಆದರೆ (val == 8 && !p_tgc_ಗಣ)
		{
#   ifdef FEAT_EVAL
		    ch_log(ಖಾಲಿ, "got_code_from_term(RGB): xterm-direct colors detected");
#   endif
		    // RGB capability set, enable termguicolors
		    ಗಣ_option_ಮೌಲ್ಯ((char_u *)"termguicolors", 1L, ಖಾಲಿ, 0);
		}
	    }
#  endif
# endif
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		i = ಕಂಡುಹಿಡಿ_ಪದ_bykeys(str, ಖಾಲಿ);
		ಆದರೆ (i >= 0 && name[0] == termcodes[i].name[0]
					    && name[1] == termcodes[i].name[1])
		{
		    // Existing entry with the same name and code - skip.
# ifdef FEAT_EVAL
		    ch_log(ಖಾಲಿ, "got_code_from_term(): Entry %c%c did not change",
							     name[0], name[1]);
# endif
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (i >= 0 && name[0] == 'K' && VIM_ISDIGIT(name[1]))
		{
		    // Would replace existing entry with keypad key - skip.
# ifdef FEAT_EVAL
		    ch_log(ಖಾಲಿ, "got_code_from_term(): Skipping entry %c%c in favor of %c%c with matching keys %s",
			    name[0], name[1], termcodes[i].name[0], termcodes[i].name[1], str);
# endif
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ಆದರೆ (i >= 0)
		    {
			// Delete an existing entry using the same code.
# ifdef FEAT_EVAL
			ch_log(ಖಾಲಿ, "got_code_from_term(): Deleting entry %c%c with matching keys %s",
			      termcodes[i].name[0], termcodes[i].name[1], str);
# endif
			del_termcode_idx(i);
		    }
# ifdef FEAT_EVAL
		    ಇಲ್ಲದಿದ್ದರೆ
			ch_log(ಖಾಲಿ, "got_code_from_term(): Adding entry %c%c with keys %s",
							name[0], name[1], str);
# endif
		    ಕೂಡಿಸು_termcode(name, str, ATC_FROM_ಪದ);
		}
	    }
	}
    }

    // May request more codes now that we received one.
    ++xt_ಸೂಚ್ಯಂಕ_ಒಳ;
    req_ಇನ್ನಷ್ಟು_codes_from_ಪದ();
}

/*
 * Check if there are any unanswered requests and deal with them.
 * This is called before starting an external program or getting direct
 * keyboard input.  We don't want responses to be send to that program or
 * handled as typed text.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
check_for_codes_from_ಪದ(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		c;

    // If no codes requested or all are answered, no need to wait.
    ಆದರೆ (xt_ಸೂಚ್ಯಂಕ_ಹೊರಹೋಗು == 0 || xt_ಸೂಚ್ಯಂಕ_ಹೊರಹೋಗು == xt_ಸೂಚ್ಯಂಕ_ಒಳ)
	ಹಿಂತಿರುಗಿ;

    // Vgetc() will check for and handle any response.
    // Keep calling vpeekc() until we don't get any responses.
    ++no_mapping;
    ++allow_ಕೀಲಿಗಳು;
    ಗಾಗಿ (;;)
    {
	c = vpeekc();
	ಆದರೆ (c == NUL)	    // nothing available
	    ನಿಲ್ಲಿಸು;

	// If a response is recognized it's replaced with K_IGNORE, must read
	// it from the input stream.  If there is no K_IGNORE we can't do
	// anything, break here (there might be some responses further on, but
	// we don't want to throw away any typed chars).
	ಆದರೆ (c != K_SPECIAL && c != K_IGNORE)
	    ನಿಲ್ಲಿಸು;
	c = vgetc();
	ಆದರೆ (c != K_IGNORE)
	{
	    vungetc(c);
	    ನಿಲ್ಲಿಸು;
	}
    }
    --no_mapping;
    --allow_ಕೀಲಿಗಳು;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
ಸ್ಥಿರ ಅಕ್ಷರ ksme_str[20];
ಸ್ಥಿರ ಅಕ್ಷರ ksmr_str[20];
ಸ್ಥಿರ ಅಕ್ಷರ ksmd_str[20];

/*
 * For Win32 console: update termcap codes for existing console attributes.
 */
    ಶೂನ್ಯ
update_tcap(ಪೂರ್ಣಾಂಕ attr)
{
    sprintf(ksme_str, "\033|%dm", attr);
    sprintf(ksmd_str, "\033|%dm", attr | 0x08);  // FOREGROUND_INTENSITY
    sprintf(ksmr_str, "\033|%dm", ((attr & 0x0F) << 4) | ((attr & 0xF0) >> 4));

    tcap_ನಮೂದು_T *p = ಕಂಡುಹಿಡಿ_builtin_ಪದ(DEFAULT_ಪದ);
    ಆದರೆ (p == ಖಾಲಿ)  // did not find it
	ಹಿಂತಿರುಗಿ;
    ಆಗ (p->bt_string != ಖಾಲಿ)
    {
	ಆದರೆ (p->bt_ನಮೂದು == (ಪೂರ್ಣಾಂಕ)KS_ME)
	    p->bt_string = &ksme_str[0];
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p->bt_ನಮೂದು == (ಪೂರ್ಣಾಂಕ)KS_MR)
	    p->bt_string = &ksmr_str[0];
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p->bt_ನಮೂದು == (ಪೂರ್ಣಾಂಕ)KS_MD)
	    p->bt_string = &ksmd_str[0];
	++p;
    }
}

# ifdef FEAT_TERMGUICOLORS
#  define KSSIZE 20

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ
{
    CMODE_ಸೂಚ್ಯಂಕಿತ = 0,	// Use cmd.exe 4bit palette.
    CMODE_RGB,		// Use 24bit RGB colors using VTP.
    CMODE_256_ಲಂಬಸಾಲು,	// Emulate xterm's 256-color palette using VTP.
    CMODE_ಕೊನೆಯ,
} cmode_T;

ರಚನೆ ks_tbl_S
{
    ಪೂರ್ಣಾಂಕ  ಸಂಕೇತ_2;				// value of KS_
    ಅಕ್ಷರ *vtp;				// code in RGB mode
    ಅಕ್ಷರ *vtp2;				// code in 256color mode
    ಅಕ್ಷರ buf[CMODE_ಕೊನೆಯ][KSSIZE];	// real buffer
};

ಸ್ಥಿರ ರಚನೆ ks_tbl_S ks_tbl[] =
{
    {(ಪೂರ್ಣಾಂಕ)KS_ME,  "\033|0m",  "\033|0m", {""}},   //ಸಾಮಾನ್ಯ
    {(ಪೂರ್ಣಾಂಕ)KS_MR,  "\033|7m",  "\033|7m", {""}},   //ಹಿಮ್ಮುಖ
    {(ಪೂರ್ಣಾಂಕ)KS_MD,  "\033|1m",  "\033|1m", {""}},   // bold
    {(ಪೂರ್ಣಾಂಕ)KS_SO,  "\033|91m", "\033|91m", {""}},  // standout: bright red text
    {(ಪೂರ್ಣಾಂಕ)KS_SE,  "\033|39m", "\033|39m", {""}},  // standout end: default color
    {(ಪೂರ್ಣಾಂಕ)KS_CZH, "\033|3m",  "\033|3m", {""}},   // italic
    {(ಪೂರ್ಣಾಂಕ)KS_CZR, "\033|0m",  "\033|0m", {""}},   // italic end
    {(ಪೂರ್ಣಾಂಕ)KS_US,  "\033|4m",  "\033|4m", {""}},   // underscore
    {(ಪೂರ್ಣಾಂಕ)KS_UE,  "\033|24m", "\033|24m", {""}},  // underscore end
#  ifdef TERMINFO
    {(ಪೂರ್ಣಾಂಕ)KS_CAB, "\033|%p1%db", "\033|%p14%dm", {""}}, // set background color
    {(ಪೂರ್ಣಾಂಕ)KS_CAF, "\033|%p1%df", "\033|%p13%dm", {""}}, // set foreground color
    {(ಪೂರ್ಣಾಂಕ)KS_CS,  "\033|%p1%d;%p2%dR", "\033|%p1%d;%p2%dR", {""}},
    {(ಪೂರ್ಣಾಂಕ)KS_CSV, "\033|%p1%d;%p2%dV", "\033|%p1%d;%p2%dV", {""}},
#  ಇಲ್ಲದಿದ್ದರೆ
    {(ಪೂರ್ಣಾಂಕ)KS_CAB, "\033|%db", "\033|4%dm", {""}}, // set background color
    {(ಪೂರ್ಣಾಂಕ)KS_CAF, "\033|%df", "\033|3%dm", {""}}, // set foreground color
    {(ಪೂರ್ಣಾಂಕ)KS_CS,  "\033|%d;%dR", "\033|%d;%dR", {""}},
    {(ಪೂರ್ಣಾಂಕ)KS_CSV, "\033|%d;%dV", "\033|%d;%dV", {""}},
#  endif
    {(ಪೂರ್ಣಾಂಕ)KS_CCO, "256", "256", {""}},	    // colors
    {(ಪೂರ್ಣಾಂಕ)KS_NAME, ಖಾಲಿ, ಖಾಲಿ, {""}}			    // terminator
};

/*
 * Find the first entry for "code" in the builtin termcap for "name".
 * Returns NULL when not found.
 */
    ಸ್ಥಿರ tcap_ನಮೂದು_T *
ಕಂಡುಹಿಡಿ_ಮೊದಲ_tcap(
    char_u *name,
    ಪೂರ್ಣಾಂಕ	    ಸಂಕೇತ_2)
{
    tcap_ನಮೂದು_T *p = ಕಂಡುಹಿಡಿ_builtin_ಪದ(name);
    ಆದರೆ (p == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಆಗ (p->bt_string != ಖಾಲಿ)
    {
	ಆದರೆ (p->bt_ನಮೂದು == ಸಂಕೇತ_2)
	    ಹಿಂತಿರುಗಿ p;
	++p;
    }
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}
# endif

/*
 * For Win32 console: replace the sequence immediately after termguicolors.
 */
    ಶೂನ್ಯ
ಅದಲುಬದಲು_tcap(ಶೂನ್ಯ)
{
# ifdef FEAT_TERMGUICOLORS
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ		init_ಮುಗಿದ_2 = FALSE;
    ಸ್ಥಿರ cmode_T	curr_ಬಹುಲಕ;
    ರಚನೆ ks_tbl_S	*ks;
    cmode_T		ಬಹುಲಕ;

    ಆದರೆ (!init_ಮುಗಿದ_2)
    {
	ಗಾಗಿ (ks = ks_tbl; ks->ಸಂಕೇತ_2 != (ಪೂರ್ಣಾಂಕ)KS_NAME; ks++)
	{
	    tcap_ನಮೂದು_T *bt = ಕಂಡುಹಿಡಿ_ಮೊದಲ_tcap(DEFAULT_ಪದ, ks->ಸಂಕೇತ_2);
	    ಆದರೆ (bt != ಖಾಲಿ)
	    {
		// Preserve the original value.
		STRNCPY(ks->buf[CMODE_ಸೂಚ್ಯಂಕಿತ], bt->bt_string, KSSIZE);
		STRNCPY(ks->buf[CMODE_RGB], ks->vtp, KSSIZE);
		STRNCPY(ks->buf[CMODE_256_ಲಂಬಸಾಲು], ks->vtp2, KSSIZE);

		bt->bt_string = ks->buf[CMODE_ಸೂಚ್ಯಂಕಿತ];
	    }
	}
	init_ಮುಗಿದ_2 = TRUE;
	curr_ಬಹುಲಕ = CMODE_ಸೂಚ್ಯಂಕಿತ;
    }

    ಆದರೆ (p_tgc)
	ಬಹುಲಕ = CMODE_RGB;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (t_colors >= 256)
	ಬಹುಲಕ = CMODE_256_ಲಂಬಸಾಲು;
    ಇಲ್ಲದಿದ್ದರೆ
	ಬಹುಲಕ = CMODE_ಸೂಚ್ಯಂಕಿತ;

    ಆದರೆ (ಬಹುಲಕ == curr_ಬಹುಲಕ)
	ಹಿಂತಿರುಗಿ;

    ಗಾಗಿ (ks = ks_tbl; ks->ಸಂಕೇತ_2 != (ಪೂರ್ಣಾಂಕ)KS_NAME; ks++)
    {
	tcap_ನಮೂದು_T *bt = ಕಂಡುಹಿಡಿ_ಮೊದಲ_tcap(DEFAULT_ಪದ, ks->ಸಂಕೇತ_2);
	ಆದರೆ (bt != ಖಾಲಿ)
	    bt->bt_string = ks->buf[ಬಹುಲಕ];
    }

    curr_ಬಹುಲಕ = ಬಹುಲಕ;
# endif
}

#ಆದರೆ_ಅಂತ್ಯ


#ಆದರೆ (defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))) || defined(FEAT_TERMINAL)
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ cube_ಮೌಲ್ಯ[] = {
    0x00, 0x5F, 0x87, 0xAF, 0xD7, 0xFF
};

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ grey_ramp[] = {
    0x08, 0x12, 0x1C, 0x26, 0x30, 0x3A, 0x44, 0x4E, 0x58, 0x62, 0x6C, 0x76,
    0x80, 0x8A, 0x94, 0x9E, 0xA8, 0xB2, 0xBC, 0xC6, 0xD0, 0xDA, 0xE4, 0xEE
};

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ char_u ansi_ಕೋಷ್ಟಕ[16][3] = {
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

# ಆದರೆ defined(MSWIN)
// Mapping between cterm indices < 16 and their counterpart in the ANSI palette.
ಸ್ಥಿರಾಂಕ char_u cterm_ansi_idx[] = {
    0, 4, 2, 6, 1, 5, 3, 7, 8, 12, 10, 14, 9, 13, 11, 15
};
# endif

# define ANSI_ಸೂಚ್ಯಂಕ_NONE 0

    ಶೂನ್ಯ
ansi_color2rgb(ಪೂರ್ಣಾಂಕ nr, char_u *r, char_u *g, char_u *b, char_u *ansi_idx)
{
    ಆದರೆ (nr < 16)
    {
	*r = ansi_ಕೋಷ್ಟಕ[nr][0];
	*g = ansi_ಕೋಷ್ಟಕ[nr][1];
	*b = ansi_ಕೋಷ್ಟಕ[nr][2];
	*ansi_idx = nr + 1;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	*r = 0;
	*g = 0;
	*b = 0;
	*ansi_idx = ANSI_ಸೂಚ್ಯಂಕ_NONE;
    }
}

    ಶೂನ್ಯ
cterm_color2rgb(ಪೂರ್ಣಾಂಕ nr, char_u *r, char_u *g, char_u *b, char_u *ansi_idx)
{
    ಪೂರ್ಣಾಂಕ idx;

    ಆದರೆ (nr < 16)
    {
# ಆದರೆ defined(MSWIN)
	idx = cterm_ansi_idx[nr];
# ಇಲ್ಲದಿದ್ದರೆ
	idx = nr;
# endif
	*r = ansi_ಕೋಷ್ಟಕ[idx][0];
	*g = ansi_ಕೋಷ್ಟಕ[idx][1];
	*b = ansi_ಕೋಷ್ಟಕ[idx][2];
	*ansi_idx = idx + 1;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (nr < 232)
    {
	// 216 color cube
	idx = nr - 16;
	*r = cube_ಮೌಲ್ಯ[idx / 36 % 6];
	*g = cube_ಮೌಲ್ಯ[idx / 6  % 6];
	*b = cube_ಮೌಲ್ಯ[idx      % 6];
	*ansi_idx = ANSI_ಸೂಚ್ಯಂಕ_NONE;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (nr < 256)
    {
	// 24 grey scale ramp
	idx = nr - 232;
	*r = grey_ramp[idx];
	*g = grey_ramp[idx];
	*b = grey_ramp[idx];
	*ansi_idx = ANSI_ಸೂಚ್ಯಂಕ_NONE;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	*r = 0;
	*g = 0;
	*b = 0;
	*ansi_idx = ANSI_ಸೂಚ್ಯಂಕ_NONE;
    }
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Replace K_BS by <BS> and K_DEL by <DEL>.
 * Include any modifiers into the key and drop them.
 * Returns "len" adjusted for replaced codes.
 */
    ಪೂರ್ಣಾಂಕ
ಪದ_replace_keycodes(char_u *ta_buf, ಪೂರ್ಣಾಂಕ ta_len, ಪೂರ್ಣಾಂಕ len_arg)
{
    ಪೂರ್ಣಾಂಕ		len = len_arg;
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		c;

    ಗಾಗಿ (i = ta_len; i < ta_len + len; ++i)
    {
	ಆದರೆ (ta_buf[i] == CSI && len - i > 3 && ta_buf[i + 1] == KS_MODIFIER)
	{
	    ಪೂರ್ಣಾಂಕ modifiers = ta_buf[i + 2];
	    ಪೂರ್ಣಾಂಕ ಕೀಲಿ = ta_buf[i + 3];

	    // Try to use the modifier to modify the key.  In any case drop the
	    // modifier.
	    mch_memmove(ta_buf + i + 1, ta_buf + i + 4, (size_t)(len - i - 3));
	    len -= 3;
	    ಆದರೆ (ಕೀಲಿ < 0x80)
		ಕೀಲಿ = ವಿಲೀನ_modify_Other_ಕೀಲಿಗಳು(ಕೀಲಿ, &modifiers);
	    ta_buf[i] = ಕೀಲಿ;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ta_buf[i] == CSI && len - i > 2)
	{
	    c = TERMCAP2_ಕೀಲಿ(ta_buf[i + 1], ta_buf[i + 2]);
	    ಆದರೆ (c == K_DEL || c == K_KDEL || c == K_BS)
	    {
		mch_memmove(ta_buf + i + 1, ta_buf + i + 3,
							(size_t)(len - i - 2));
		ಆದರೆ (c == K_DEL || c == K_KDEL)
		    ta_buf[i] = DEL;
		ಇಲ್ಲದಿದ್ದರೆ
		    ta_buf[i] = Ctrl_H;
		len -= 2;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ta_buf[i] == '\r')
	    ta_buf[i] = '\n';
	ಆದರೆ (has_mbyte)
	    i += (*mb_ptr2len_len)(ta_buf + i, ta_len + len - i) - 1;
    }
    ಹಿಂತಿರುಗಿ len;
}

/*
 * Should be called when cleaning up terminal state.
 */
    ಶೂನ್ಯ
ಪದ_disable_dec(ಶೂನ್ಯ)
{
    ಪದ_ಗಣ_sync_output(ಪದ_SYNC_OUTPUT_OFF);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
    ಪದ_ಗಣ_win_resize(false);
#ಆದರೆ_ಅಂತ್ಯ
    // Make sure to always flush the output buffer, because this may be called
    // before starting the GUI
    ಹೊರಹೋಗು_flush();
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
/*
 * Enable or disable receiving in-band window resize events from the terminal.
 * If "state" is true, then if the terminal supports DEC mode 2048 and
 * 'termresize' is "" or "inband", then enable it and disable the SIGWINCH
 * signal handling. Otherwise disable the mode if it is enabled and reinstall
 * the SIGWINCH handler.
 */
    ಶೂನ್ಯ
ಪದ_ಗಣ_win_resize(bool state)
{
# ifdef FEAT_GUI
    bool    ಒಳ_gui = gui.ಒಳ_use;

    ಆದರೆ (state && ಒಳ_gui)
	ಹಿಂತಿರುಗಿ;
# endif

    ಆದರೆ (!state || win_resize_setting == 0 || win_resize_setting == 4)
    {
	// Make sure it update internal window size if DEC mode 2048 is
	// unavailable now.
	ಆದರೆ (win_resize_enabled)
	{
	    ಗಣ_shellsize(0, 0, false);
	    ಗಣ_sigwinch_handler();
	    ಹೊರಹೋಗು_str((char_u *)"\033[?2048l");
	}
	win_resize_enabled = false;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((*p_trz == NUL || STRCMP(p_trz, "inband") == 0)
	    && !win_resize_enabled)
    {
	ಆದರೆ (win_resize_setting == 2)
	    ಹೊರಹೋಗು_str((char_u *)"\033[?2048h");
# ifdef SIGWINCH
	mch_signal(SIGWINCH, SIG_DFL);
# endif
	win_resize_enabled = true;
    }
}
#ಆದರೆ_ಅಂತ್ಯ

    ಪೂರ್ಣಾಂಕ
sync_output_ಸಕ್ರಿಯ(ಶೂನ್ಯ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    ಆದರೆ (gui.ಒಳ_use)
	ಹಿಂತಿರುಗಿ TRUE;
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ p_tsy && (sync_output_setting == 1 || sync_output_setting == 2)
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
ಪದ_ಗಣ_sync_output(ಪೂರ್ಣಾಂಕ flags)
{
    bool    allowed;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    bool    ಒಳ_gui = gui.ಒಳ_use;
#ಇಲ್ಲದಿದ್ದರೆ
    bool    ಒಳ_gui = false;
#ಆದರೆ_ಅಂತ್ಯ

    allowed = p_tsy && (sync_output_setting == 1 || sync_output_setting == 2);

    ಆದರೆ (flags & ಪದ_SYNC_OUTPUT_FLUSH)
    {
	// Tell terminal to display screen contents, then resume batching.
	ಆದರೆ (allowed && !ಒಳ_gui && sync_output_state > 0 && *T_ESU != NUL &&
		*T_BSU != NUL)
	{
	    ಹೊರಹೋಗು_str_nf(T_ESU);
	    ಹೊರಹೋಗು_str_nf(T_BSU);
	    ಹೊರಹೋಗು_flush();
	}
	ಹಿಂತಿರುಗಿ;
    }

    // Forcibly turn off synchronized output (e.g. 'notermsync')
    ಆದರೆ (flags & ಪದ_SYNC_OUTPUT_OFF)
    {
	ಆದರೆ (sync_output_state > 0 && *T_ESU != NUL)
	{
	    ಹೊರಹೋಗು_str_nf(T_ESU);
	    ಹೊರಹೋಗು_flush();
	    sync_output_state = 0;
	}
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (!allowed || ಒಳ_gui || *T_BSU == NUL || *T_ESU == NUL)
	ಹಿಂತಿರುಗಿ;

    // Only enable if we aren't already, and only disable if we have reached
    // zero.
    ಆದರೆ (flags & ಪದ_SYNC_OUTPUT_ENABLE)
    {
	ಆದರೆ (sync_output_state++ > 0)
	    ಹಿಂತಿರುಗಿ;
	ಹೊರಹೋಗು_str_nf(T_BSU);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (flags & ಪದ_SYNC_OUTPUT_DISABLE)
    {
	ಆದರೆ (sync_output_state == 0 || --sync_output_state > 0)
	    ಹಿಂತಿರುಗಿ;
	// Flush the output buffer before ending the sync batch so that
	// all drawing output is sent to the terminal within the
	// BSU..ESU window.  Without this, the drawing data remaining in
	// out_buf would be sent after ESU, outside the sync batch.
	ಹೊರಹೋಗು_str_nf(T_ESU);
	ಹೊರಹೋಗು_flush();
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	siemsg("Unknown sync output value %d", flags);
	ಹಿಂತಿರುಗಿ;
    }
}
