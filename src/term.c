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
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ tgetstr tgetstr_defined_wrong

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_TGETENT
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
# ਜੇ defined(VMS) || defined(AMIGA)
#  define TPUTSFUNCAST (ਖਾਲੀ (*)(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ))
# ਨਹੀਂਤਾਂ
#  ifdef HAVE_OUTFUNTYPE
#   define TPUTSFUNCAST (outfuntype)
#  ਨਹੀਂਤਾਂ
#   define TPUTSFUNCAST (ਪੂਰਨ (*)(ਪੂਰਨ))
#  endif
# endif
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ tgetstr

// start of keys that are not directly used by Vim but can be mapped
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BT_EXTRA_ਕੁੰਜੀਆਂ	0x101

ਸਥਿਰ ਖਾਲੀ parse_builtin_tcap(char_u *s);
ਸਥਿਰ ਖਾਲੀ gather_termleader(ਖਾਲੀ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMRESPONSE
ਸਥਿਰ ਖਾਲੀ req_codes_from_ਪਦ(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ req_ਹੋਰ_codes_from_ਪਦ(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ got_ਕੋਡ_from_ਪਦ(char_u *ਕੋਡ, ਪੂਰਨ len);
ਸਥਿਰ ਖਾਲੀ check_for_codes_from_ਪਦ(ਖਾਲੀ);
#ਜੇ_ਅੰਤ
ਸਥਿਰ ਖਾਲੀ del_termcode_idx(ਪੂਰਨ idx);
ਸਥਿਰ ਪੂਰਨ ਲੱਭੋ_ਪਦ_bykeys(char_u *src, ਪੂਰਨ *len);
ਸਥਿਰ ਪੂਰਨ ਪਦ_is_builtin(char_u *name);
ਸਥਿਰ ਪੂਰਨ ਪਦ_7to8bit(char_u *p);
ਸਥਿਰ ਖਾਲੀ accept_modifiers_for_function_ਕੁੰਜੀਆਂ(ਖਾਲੀ);

#ਜੇ 0  // Change to 1 to enable ch_log() calls for termresponse debugging.
# define DEBUG_TERMRESPONSE
# define LOG_TR1(str) \
		ch_log(ਨਿਰਾ, "TermResp: %s " str, \
			must_redraw == UPD_NOT_VALID ? "NV" \
			: must_redraw == UPD_CLEAR ? "CL" : "  ")
# define LOG_TRN(fmt,...) \
		ch_log(ਨਿਰਾ, "TermResp: %s " fmt, \
			must_redraw == UPD_NOT_VALID ? "NV" \
			: must_redraw == UPD_CLEAR ? "CL" : "  ", __VA_ARGS__)
#ਨਹੀਂਤਾਂ
# define LOG_TR1(str) ਕਰੋ { /**/ } ਜਦੋਂ (0)
# define LOG_TRN(fmt,...) ਕਰੋ { /**/ } ਜਦੋਂ (0)
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_TGETENT
ਸਥਿਰ ਅੱਖਰ *invoke_tgetent(char_u *, char_u *);

/*
 * Here is our own prototype for tgetstr(), any prototypes from the include
 * files have been disabled by the define at the start of this file.
 */
ਅੱਖਰ		*tgetstr(ਅੱਖਰ *, ਅੱਖਰ **);
#ਜੇ_ਅੰਤ

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
    ਸਥਿਤੀ_GET,		// send request when switching to RAW mode
    ਸਥਿਤੀ_ਭੇਜਿਆ,	// did send request, checking for response
    ਸਥਿਤੀ_GOT,		// received response
    ਸਥਿਤੀ_FAIL		// timed out
} request_ਤਰੱਕੀ_T;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    request_ਤਰੱਕੀ_T	    tr_ਤਰੱਕੀ;
    time_t		    tr_ਸ਼ੁਰੂ;	// when request was sent, -1 for never
} termrequest_T;

// Holds state for current OSC response.
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    ਪੂਰਨ		    processing;	// If we are in the middle of an OSC response
    char_u	    ਸ਼ੁਰੂ_char;	// First char in the OSC response
    garray_T	    buf;	// Buffer holding the OSC response, to be
				// placed in the "v:termosc" vim var.

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ELAPSED_FUNC
    elapsed_T	    ਸ਼ੁਰੂ_tv;	// Set at the beginning of an OSC response.
				// Used to timeout after a set amount of
				// time.
#ਜੇ_ਅੰਤ
} oscstate_T;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TERMREQUEST_INIT {ਸਥਿਤੀ_GET, -1}

// Request Terminal Version status:
ਸਥਿਰ termrequest_T crv_ਸਥਿਤੀ = TERMREQUEST_INIT;

// Request Cursor position report:
ਸਥਿਰ termrequest_T u7_ਸਥਿਤੀ = TERMREQUEST_INIT;

// Request xterm compatibility check:
ਸਥਿਰ termrequest_T xcc_ਸਥਿਤੀ = TERMREQUEST_INIT;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMRESPONSE
# ifdef FEAT_TERMINAL
// Request foreground color report:
ਸਥਿਰ termrequest_T rfg_ਸਥਿਤੀ = TERMREQUEST_INIT;
ਸਥਿਰ ਪੂਰਨ fg_r = 0;
ਸਥਿਰ ਪੂਰਨ fg_g = 0;
ਸਥਿਰ ਪੂਰਨ fg_b = 0;
# endif
// Background color values from the OSC 11 response, also used by the
// image backend to flatten RGBA alpha onto the actual terminal background.
ਸਥਿਰ ਪੂਰਨ bg_r = 255;
ਸਥਿਰ ਪੂਰਨ bg_g = 255;
ਸਥਿਰ ਪੂਰਨ bg_b = 255;

// Request background color report:
ਸਥਿਰ termrequest_T rbg_ਸਥਿਤੀ = TERMREQUEST_INIT;

// Request cursor blinking mode report:
ਸਥਿਰ termrequest_T rbm_ਸਥਿਤੀ = TERMREQUEST_INIT;

// Request cursor style report:
ਸਥਿਰ termrequest_T rcs_ਸਥਿਤੀ = TERMREQUEST_INIT;

// Request window's position report:
ਸਥਿਰ termrequest_T winpos_ਸਥਿਤੀ = TERMREQUEST_INIT;

// Request DECRQM (DEC mode) report:
ਸਥਿਰ termrequest_T decrqm_ਸਥਿਤੀ = TERMREQUEST_INIT;

ਸਥਿਰ termrequest_T *all_termrequests[] = {
    &crv_ਸਥਿਤੀ,
    &u7_ਸਥਿਤੀ,
    &xcc_ਸਥਿਤੀ,
# ifdef FEAT_TERMINAL
    &rfg_ਸਥਿਤੀ,
# endif
    &rbg_ਸਥਿਤੀ,
    &rbm_ਸਥਿਤੀ,
    &rcs_ਸਥਿਤੀ,
    &winpos_ਸਥਿਤੀ,
    &decrqm_ਸਥਿਤੀ,
    ਨਿਰਾ
};

// The t_8u code may default to a value but get reset when the term response is
// received.  To avoid redrawing too often, only redraw when t_8u is not reset
// and it was supposed to be written.  Unless t_8u was set explicitly.
// FALSE -> don't output t_8u yet
// MAYBE -> tried outputting t_8u while FALSE
// OK    -> can write t_8u
ਪੂਰਨ write_t_8u_state = FALSE;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_TGETENT
/*
 * Don't declare these variables if termcap.h contains them.
 * Autoconf checks if these variables should be declared extern (not all
 * systems have them).
 * Some versions define ospeed to be speed_t, but that is incompatible with
 * BSD, where ospeed is short and speed_t is long.
 */
# ifndef HAVE_OSPEED
#  ifdef OSPEED_EXTERN
ਬਾਹਰੀ ਛੋਟਾ ospeed;
#  ਨਹੀਂਤਾਂ
ਛੋਟਾ ospeed;
#  endif
# endif
# ifndef HAVE_ਉੱਪਰ_BC_PC
#  ifdef ਉੱਪਰ_BC_PC_EXTERN
ਬਾਹਰੀ ਅੱਖਰ *ਉੱਪਰ_2, *BC, PC;
#  ਨਹੀਂਤਾਂ
ਅੱਖਰ *ਉੱਪਰ_2, *BC, PC;
#  endif
# endif

# define TGETSTR(s, p)	vim_tgetstr((s), (p))
# define TGETENT(b, t)	tgetent((ਅੱਖਰ *)(b), (ਅੱਖਰ *)(t))
ਸਥਿਰ char_u *vim_tgetstr(ਅੱਖਰ *s, char_u **pp);
#ਜੇ_ਅੰਤ // HAVE_TGETENT

ਸਥਿਰ ਪੂਰਨ  detected_8bit = FALSE;	// detected 8-bit terminal

#ਜੇ (defined(UNIX) || defined(VMS))
ਸਥਿਰ ਪੂਰਨ focus_state = MAYBE; // TRUE if the Vim window has focus
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMRESPONSE
// When the cursor shape was detected these values are used:
// 1: block, 2: underline, 3: vertical bar
ਸਥਿਰ ਪੂਰਨ initial_cursor_shape = 0;

// The blink flag from the style response may be inverted from the actual
// blinking state, xterm XORs the flags.
ਸਥਿਰ ਪੂਰਨ initial_cursor_shape_blink = FALSE;

// The blink flag from the blinking-cursor mode response
ਸਥਿਰ ਪੂਰਨ initial_cursor_blink = FALSE;
#ਜੇ_ਅੰਤ

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
ਸਥਿਰ ਪੂਰਨ sync_output_setting = 0;

// > 0: Currently batching output
// == 0: No synchronized output
ਸਥਿਰ ਪੂਰਨ sync_output_state = 0;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
// DEC mode 2048 (in-band window resize events)
// https://gist.github.com/rockorager/e695fb2924d36b2bcf1fff4a3704bd83
ਸਥਿਰ ਪੂਰਨ win_resize_setting = 0;
ਸਥਿਰ bool win_resize_enabled = false;
#ਜੇ_ਅੰਤ

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
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    ਪੂਰਨ		bt_ਇੰਦਰਾਜ;   // either a KS_xxx code (>= 0), or a K_xxx code.
    ਅੱਖਰ	*bt_string; //ਮੁੱਲ
} tcap_ਇੰਦਰਾਜ_T;

/*
 * Standard ANSI terminal, default for Unix.
 */
ਸਥਿਰ tcap_ਇੰਦਰਾਜ_T builtin_ansi[] = {
    {(ਪੂਰਨ)KS_CE,	"\033[K"},
    {(ਪੂਰਨ)KS_AL,	"\033[L"},
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TERMINFO
    {(ਪੂਰਨ)KS_CAL,	"\033[%p1%dL"},
#ਨਹੀਂਤਾਂ
    {(ਪੂਰਨ)KS_CAL,	"\033[%dL"},
#ਜੇ_ਅੰਤ
    {(ਪੂਰਨ)KS_DL,	"\033[M"},
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TERMINFO
    {(ਪੂਰਨ)KS_CDL,	"\033[%p1%dM"},
#ਨਹੀਂਤਾਂ
    {(ਪੂਰਨ)KS_CDL,	"\033[%dM"},
#ਜੇ_ਅੰਤ
    {(ਪੂਰਨ)KS_CL,	"\033[H\033[2J"},
    {(ਪੂਰਨ)KS_ME,	"\033[0m"},
    {(ਪੂਰਨ)KS_MR,	"\033[7m"},
    {(ਪੂਰਨ)KS_MS,	"y"},
    {(ਪੂਰਨ)KS_UT,	"y"},		// guessed
    {(ਪੂਰਨ)KS_LE,	"\b"},
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TERMINFO
    {(ਪੂਰਨ)KS_CM,	"\033[%i%p1%d;%p2%dH"},
#ਨਹੀਂਤਾਂ
    {(ਪੂਰਨ)KS_CM,	"\033[%i%d;%dH"},
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TERMINFO
    {(ਪੂਰਨ)KS_CRI,	"\033[%p1%dC"},
#ਨਹੀਂਤਾਂ
    {(ਪੂਰਨ)KS_CRI,	"\033[%dC"},
#ਜੇ_ਅੰਤ

    {(ਪੂਰਨ)KS_NAME,	ਨਿਰਾ}  // end marker
};

/*
 * VT320 is working as an ANSI terminal compatible DEC terminal.
 * (it covers VT1x0, VT2x0 and VT3x0 up to VT320 on VMS as well)
 * TODO:- rewrite ESC[ codes to CSI
 *      - keyboard languages (CSI ? 26 n)
 */
ਸਥਿਰ tcap_ਇੰਦਰਾਜ_T builtin_vt320[] = {
    {(ਪੂਰਨ)KS_CE,	"\033[K"},
    {(ਪੂਰਨ)KS_AL,	"\033[L"},
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TERMINFO
    {(ਪੂਰਨ)KS_CAL,	"\033[%p1%dL"},
#ਨਹੀਂਤਾਂ
    {(ਪੂਰਨ)KS_CAL,	"\033[%dL"},
#ਜੇ_ਅੰਤ
    {(ਪੂਰਨ)KS_DL,	"\033[M"},
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TERMINFO
    {(ਪੂਰਨ)KS_CDL,	"\033[%p1%dM"},
#ਨਹੀਂਤਾਂ
    {(ਪੂਰਨ)KS_CDL,	"\033[%dM"},
#ਜੇ_ਅੰਤ
    {(ਪੂਰਨ)KS_CL,	"\033[H\033[2J"},
    {(ਪੂਰਨ)KS_CD,	"\033[J"},
    {(ਪੂਰਨ)KS_CCO,	"8"},			// allow 8 colors
    {(ਪੂਰਨ)KS_ME,	"\033[0m"},
    {(ਪੂਰਨ)KS_MR,	"\033[7m"},
    {(ਪੂਰਨ)KS_MD,	"\033[1m"},  // bold mode
    {(ਪੂਰਨ)KS_SE,	"\033[22m"},// normal mode
    {(ਪੂਰਨ)KS_UE,	"\033[24m"},// exit underscore mode
    {(ਪੂਰਨ)KS_US,	"\033[4m"},  // underscore mode
    {(ਪੂਰਨ)KS_CZH,	"\033[34;43m"},  // italic mode: blue text on yellow
    {(ਪੂਰਨ)KS_CZR,	"\033[0m"},	    // italic mode end
    {(ਪੂਰਨ)KS_CAB,	"\033[4%dm"},	    // set background color (ANSI)
    {(ਪੂਰਨ)KS_CAF,	"\033[3%dm"},	    // set foreground color (ANSI)
    {(ਪੂਰਨ)KS_CSB,	"\033[102;%dm"},	// set screen background color
    {(ਪੂਰਨ)KS_CSF,	"\033[101;%dm"},	// set screen foreground color
    {(ਪੂਰਨ)KS_MS,	"y"},
    {(ਪੂਰਨ)KS_UT,	"y"},
    {(ਪੂਰਨ)KS_XN,	"y"},
    {(ਪੂਰਨ)KS_LE,	"\b"},
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TERMINFO
    {(ਪੂਰਨ)KS_CM,	"\033[%i%p1%d;%p2%dH"},
#ਨਹੀਂਤਾਂ
    {(ਪੂਰਨ)KS_CM,	"\033[%i%d;%dH"},
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TERMINFO
    {(ਪੂਰਨ)KS_CRI,	"\033[%p1%dC"},
#ਨਹੀਂਤਾਂ
    {(ਪੂਰਨ)KS_CRI,	"\033[%dC"},
#ਜੇ_ਅੰਤ
    {K_ਉੱਪਰ,		"\033[A"},
    {K_ਥੱਲੇ,		"\033[B"},
    {K_ਸੱਜਾ,		"\033[C"},
    {K_ਖੱਬਾ,		"\033[D"},
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
    {K_ਸਿਰਾ,		"\033[4~"},
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

    {(ਪੂਰਨ)KS_NAME,	ਨਿਰਾ}  // end marker
};

/*
 * Ordinary vt52
 */
ਸਥਿਰ tcap_ਇੰਦਰਾਜ_T builtin_vt52[] = {
    {(ਪੂਰਨ)KS_CE,	"\033K"},
    {(ਪੂਰਨ)KS_CD,	"\033J"},
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TERMINFO
    {(ਪੂਰਨ)KS_CM,	"\033Y%p1%' '%+%c%p2%' '%+%c"},
#ਨਹੀਂਤਾਂ
    {(ਪੂਰਨ)KS_CM,	"\033Y%+ %+ "},
#ਜੇ_ਅੰਤ
    {(ਪੂਰਨ)KS_LE,	"\b"},
    {(ਪੂਰਨ)KS_SR,	"\033I"},
    {(ਪੂਰਨ)KS_AL,	"\033L"},
    {(ਪੂਰਨ)KS_DL,	"\033M"},
    {K_ਉੱਪਰ,		"\033A"},
    {K_ਥੱਲੇ,		"\033B"},
    {K_ਖੱਬਾ,		"\033D"},
    {K_ਸੱਜਾ,		"\033C"},
    {K_F1,		"\033P"},
    {K_F2,		"\033Q"},
    {K_F3,		"\033R"},
    {(ਪੂਰਨ)KS_CL,	"\033H\033J"},
    {(ਪੂਰਨ)KS_MS,	"y"},

    {(ਪੂਰਨ)KS_NAME,	ਨਿਰਾ}  // end marker
};

/*
 * Builtin xterm with Vim-specific entries.
 */
ਸਥਿਰ tcap_ਇੰਦਰਾਜ_T builtin_xterm[] = {
    {(ਪੂਰਨ)KS_CE,	"\033[K"},
    {(ਪੂਰਨ)KS_AL,	"\033[L"},
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TERMINFO
    {(ਪੂਰਨ)KS_CAL,	"\033[%p1%dL"},
#ਨਹੀਂਤਾਂ
    {(ਪੂਰਨ)KS_CAL,	"\033[%dL"},
#ਜੇ_ਅੰਤ
    {(ਪੂਰਨ)KS_DL,	"\033[M"},
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TERMINFO
    {(ਪੂਰਨ)KS_CDL,	"\033[%p1%dM"},
#ਨਹੀਂਤਾਂ
    {(ਪੂਰਨ)KS_CDL,	"\033[%dM"},
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TERMINFO
    {(ਪੂਰਨ)KS_CS,	"\033[%i%p1%d;%p2%dr"},
#ਨਹੀਂਤਾਂ
    {(ਪੂਰਨ)KS_CS,	"\033[%i%d;%dr"},
#ਜੇ_ਅੰਤ
    {(ਪੂਰਨ)KS_CL,	"\033[H\033[2J"},
    {(ਪੂਰਨ)KS_CD,	"\033[J"},
    {(ਪੂਰਨ)KS_ME,	"\033[m"},
    {(ਪੂਰਨ)KS_MR,	"\033[7m"},
    {(ਪੂਰਨ)KS_MD,	"\033[1m"},
    {(ਪੂਰਨ)KS_UE,	"\033[m"},
    {(ਪੂਰਨ)KS_US,	"\033[4m"},
    {(ਪੂਰਨ)KS_STE,	"\033[29m"},
    {(ਪੂਰਨ)KS_STS,	"\033[9m"},
    {(ਪੂਰਨ)KS_MS,	"y"},
    {(ਪੂਰਨ)KS_UT,	"y"},
    {(ਪੂਰਨ)KS_LE,	"\b"},
    {(ਪੂਰਨ)KS_VI,	"\033[?25l"},
    {(ਪੂਰਨ)KS_VE,	"\033[?25h"},
    {(ਪੂਰਨ)KS_VS,	"\033[?12h"},
    {(ਪੂਰਨ)KS_CVS,	"\033[?12l"},
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TERMINFO
    {(ਪੂਰਨ)KS_CSH,	"\033[%p1%d q"},
#ਨਹੀਂਤਾਂ
    {(ਪੂਰਨ)KS_CSH,	"\033[%d q"},
#ਜੇ_ਅੰਤ
    {(ਪੂਰਨ)KS_ਚੱਕਰੀ_ਜਾਂਚ,	"\033[?12$p"},
    {(ਪੂਰਨ)KS_CRS,	"\033P$q q\033\\"},
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TERMINFO
    {(ਪੂਰਨ)KS_CM,	"\033[%i%p1%d;%p2%dH"},
#ਨਹੀਂਤਾਂ
    {(ਪੂਰਨ)KS_CM,	"\033[%i%d;%dH"},
#ਜੇ_ਅੰਤ
    {(ਪੂਰਨ)KS_SR,	"\033M"},
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TERMINFO
    {(ਪੂਰਨ)KS_CRI,	"\033[%p1%dC"},
#ਨਹੀਂਤਾਂ
    {(ਪੂਰਨ)KS_CRI,	"\033[%dC"},
#ਜੇ_ਅੰਤ
    {(ਪੂਰਨ)KS_KS,	"\033[?1h\033="},
    {(ਪੂਰਨ)KS_KE,	"\033[?1l\033>"},
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_XTERM_SAVE
    {(ਪੂਰਨ)KS_TI,	"\0337\033[?47h"},
    {(ਪੂਰਨ)KS_TE,	"\033[?47l\0338"},
#ਜੇ_ਅੰਤ
    // These are now under control of the 'keyprotocol' option, see
    // "builtin_mok2".
    // {(int)KS_CTI,	"\033[>4;2m"},
    // {(int)KS_CRK,	"\033[?4m"},
    // {(int)KS_CTE,	"\033[>4;m"},
    {(ਪੂਰਨ)KS_CIS,	"\033]1;"},
    {(ਪੂਰਨ)KS_CIE,	"\007"},
    {(ਪੂਰਨ)KS_TS,	"\033]2;"},
    {(ਪੂਰਨ)KS_FS,	"\007"},
    {(ਪੂਰਨ)KS_CSC,	"\033]12;"},
    {(ਪੂਰਨ)KS_CEC,	"\007"},
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TERMINFO
    {(ਪੂਰਨ)KS_CWS,	"\033[8;%p1%d;%p2%dt"},
    {(ਪੂਰਨ)KS_CWP,	"\033[3;%p1%d;%p2%dt"},
    {(ਪੂਰਨ)KS_CGP,	"\033[13t"},
#ਨਹੀਂਤਾਂ
    {(ਪੂਰਨ)KS_CWS,	"\033[8;%d;%dt"},
    {(ਪੂਰਨ)KS_CWP,	"\033[3;%d;%dt"},
    {(ਪੂਰਨ)KS_CGP,	"\033[13t"},
#ਜੇ_ਅੰਤ
    {(ਪੂਰਨ)KS_CRV,	"\033[>c"},
    {(ਪੂਰਨ)KS_CXM,	"\033[?1006;1000%?%p1%{1}%=%th%el%;"},
    {(ਪੂਰਨ)KS_RFG,	"\033]10;?\007"},
    {(ਪੂਰਨ)KS_RBG,	"\033]11;?\007"},
    {(ਪੂਰਨ)KS_U7,	"\033[6n"},
    {(ਪੂਰਨ)KS_CAU,	"\033[58;5;%dm"},
    {(ਪੂਰਨ)KS_CBE,	"\033[?2004h"},
    {(ਪੂਰਨ)KS_CBD,	"\033[?2004l"},
    {(ਪੂਰਨ)KS_CST,	"\033[22;2t"},
    {(ਪੂਰਨ)KS_CRT,	"\033[23;2t"},
    {(ਪੂਰਨ)KS_SSI,	"\033[22;1t"},
    {(ਪੂਰਨ)KS_SRI,	"\033[23;1t"},
#ਜੇ (defined(UNIX) || defined(VMS))
    {(ਪੂਰਨ)KS_FD,	"\033[?1004l"},
    {(ਪੂਰਨ)KS_FE,	"\033[?1004h"},
#ਜੇ_ਅੰਤ

    {K_ਉੱਪਰ,		"\033O*A"},
    {K_ਥੱਲੇ,		"\033O*B"},
    {K_ਸੱਜਾ,		"\033O*C"},
    {K_ਖੱਬਾ,		"\033O*D"},
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
    {K_ਸਿਰਾ,		"\033[@;*F"},	// Esc [ F or Esc 1 ; F
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

    {BT_EXTRA_ਕੁੰਜੀਆਂ,   ""},
    {TERMCAP2_ਕੁੰਜੀ('k', '0'), "\033[10;*~"}, // F0
    {TERMCAP2_ਕੁੰਜੀ('F', '3'), "\033[25;*~"}, // F13
    // F14 and F15 are missing, because they send the same codes as the undo
    // and help key, although they don't work on all keyboards.
    {TERMCAP2_ਕੁੰਜੀ('F', '6'), "\033[29;*~"}, // F16
    {TERMCAP2_ਕੁੰਜੀ('F', '7'), "\033[31;*~"}, // F17
    {TERMCAP2_ਕੁੰਜੀ('F', '8'), "\033[32;*~"}, // F18
    {TERMCAP2_ਕੁੰਜੀ('F', '9'), "\033[33;*~"}, // F19
    {TERMCAP2_ਕੁੰਜੀ('F', 'A'), "\033[34;*~"}, // F20

    {TERMCAP2_ਕੁੰਜੀ('F', 'B'), "\033[42;*~"}, // F21
    {TERMCAP2_ਕੁੰਜੀ('F', 'C'), "\033[43;*~"}, // F22
    {TERMCAP2_ਕੁੰਜੀ('F', 'D'), "\033[44;*~"}, // F23
    {TERMCAP2_ਕੁੰਜੀ('F', 'E'), "\033[45;*~"}, // F24
    {TERMCAP2_ਕੁੰਜੀ('F', 'F'), "\033[46;*~"}, // F25
    {TERMCAP2_ਕੁੰਜੀ('F', 'G'), "\033[47;*~"}, // F26
    {TERMCAP2_ਕੁੰਜੀ('F', 'H'), "\033[48;*~"}, // F27
    {TERMCAP2_ਕੁੰਜੀ('F', 'I'), "\033[49;*~"}, // F28
    {TERMCAP2_ਕੁੰਜੀ('F', 'J'), "\033[50;*~"}, // F29
    {TERMCAP2_ਕੁੰਜੀ('F', 'K'), "\033[51;*~"}, // F30

    {TERMCAP2_ਕੁੰਜੀ('F', 'L'), "\033[52;*~"}, // F31
    {TERMCAP2_ਕੁੰਜੀ('F', 'M'), "\033[53;*~"}, // F32
    {TERMCAP2_ਕੁੰਜੀ('F', 'N'), "\033[54;*~"}, // F33
    {TERMCAP2_ਕੁੰਜੀ('F', 'O'), "\033[55;*~"}, // F34
    {TERMCAP2_ਕੁੰਜੀ('F', 'P'), "\033[56;*~"}, // F35
    {TERMCAP2_ਕੁੰਜੀ('F', 'Q'), "\033[57;*~"}, // F36
    {TERMCAP2_ਕੁੰਜੀ('F', 'R'), "\033[58;*~"}, // F37

    {(ਪੂਰਨ)KS_NAME,	ਨਿਰਾ}  // end marker
};

/*
 * Additions for using modifyOtherKeys level 2.  Same as what is used for
 * xterm.
 */
ਸਥਿਰ tcap_ਇੰਦਰਾਜ_T builtin_mok2[] = {
    // t_TI enables modifyOtherKeys level 2
    {(ਪੂਰਨ)KS_CTI,	"\033[>4;2m"},

    // XTQMODKEYS was added in xterm version 377: "CSI ? 4 m" which should
    // return "{lead} > 4 ; Pv m".  Before version 377 we expect it to have no
    // effect.
    {(ਪੂਰਨ)KS_CRK,	"\033[?4m"},

    // t_TE disables modifyOtherKeys
    {(ਪੂਰਨ)KS_CTE,	"\033[>4;m"},

    {(ਪੂਰਨ)KS_NAME,	ਨਿਰਾ}  // end marker
};

/*
 * Additions for using the Kitty keyboard protocol.
 */
ਸਥਿਰ tcap_ਇੰਦਰਾਜ_T builtin_kitty[] = {
    // t_TI enables the kitty keyboard protocol.
    {(ਪੂਰਨ)KS_CTI,	"\033[=1;1u"},

    // t_RK requests the kitty keyboard protocol state
    {(ਪੂਰਨ)KS_CRK,	"\033[?u"},

    // t_TE also disables modifyOtherKeys, because t_TI from xterm may already
    // have been used.
    {(ਪੂਰਨ)KS_CTE,	"\033[>4;m\033[=0;1u"},

    // t_RF request terminal foreground color
    {(ਪੂਰਨ)KS_RFG,	"\033]10;?\033\\"},

    // t_RB request terminal background color
    {(ਪੂਰਨ)KS_RBG,	"\033]11;?\033\\"},

    {(ਪੂਰਨ)KS_NAME,	ਨਿਰਾ}  // end marker
};

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_TGETENT
/*
 * Additions for enabling/disabling synchronized output mode for terminal.
 */
ਸਥਿਰ tcap_ਇੰਦਰਾਜ_T builtin_sync_output[] = {
    {(ਪੂਰਨ)KS_BSU,	"\033[?2026h"},
    {(ਪੂਰਨ)KS_ESU,	"\033[?2026l"},
    {(ਪੂਰਨ)KS_NAME,	ਨਿਰਾ}  // end marker
};
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMRESPONSE
/*
 * List of DECRQM modes that Vim supports
 */
ਸਥਿਰ ਅਚਲ ਪੂਰਨ dec_modes[] = {
    2026,   // Synchronized output
# ifdef UNIX
    2048    // In-band terminal resize events
# endif
};
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMGUICOLORS
/*
 * Additions for using the RGB colors and terminal font
 */
ਸਥਿਰ tcap_ਇੰਦਰਾਜ_T builtin_rgb[] = {
    // These are printf strings, not terminal codes.
    {(ਪੂਰਨ)KS_8F,	"\033[38;2;%lu;%lu;%lum"},
    {(ਪੂਰਨ)KS_8B,	"\033[48;2;%lu;%lu;%lum"},
    {(ਪੂਰਨ)KS_8U,	"\033[58;2;%lu;%lu;%lum"},

    {(ਪੂਰਨ)KS_NAME,	ਨਿਰਾ}  // end marker
};
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_TGETENT
ਸਥਿਰ tcap_ਇੰਦਰਾਜ_T special_ਪਦ[] = {
    // These are printf strings, not terminal codes.
    {(ਪੂਰਨ)KS_CF,	"\033[%dm"},
    {(ਪੂਰਨ)KS_NAME,	ਨਿਰਾ}  // end marker
};
#ਜੇ_ਅੰਤ

/*
 * iris-ansi for Silicon Graphics machines.
 */
ਸਥਿਰ tcap_ਇੰਦਰਾਜ_T builtin_iris_ansi[] = {
    {(ਪੂਰਨ)KS_CE,	"\033[K"},
    {(ਪੂਰਨ)KS_CD,	"\033[J"},
    {(ਪੂਰਨ)KS_AL,	"\033[L"},
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TERMINFO
    {(ਪੂਰਨ)KS_CAL,	"\033[%p1%dL"},
#ਨਹੀਂਤਾਂ
    {(ਪੂਰਨ)KS_CAL,	"\033[%dL"},
#ਜੇ_ਅੰਤ
    {(ਪੂਰਨ)KS_DL,	"\033[M"},
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TERMINFO
    {(ਪੂਰਨ)KS_CDL,	"\033[%p1%dM"},
#ਨਹੀਂਤਾਂ
    {(ਪੂਰਨ)KS_CDL,	"\033[%dM"},
#ਜੇ_ਅੰਤ
#ਜੇ 0	// The scroll region is not working as Vim expects.
# ifdef TERMINFO
    {(ਪੂਰਨ)KS_CS,	"\033[%i%p1%d;%p2%dr"},
# ਨਹੀਂਤਾਂ
    {(ਪੂਰਨ)KS_CS,	"\033[%i%d;%dr"},
# endif
#ਜੇ_ਅੰਤ
    {(ਪੂਰਨ)KS_CL,	"\033[H\033[2J"},
    {(ਪੂਰਨ)KS_VE,	"\033[9/y\033[12/y"},	// These aren't documented
    {(ਪੂਰਨ)KS_VS,	"\033[10/y\033[=1h\033[=2l"}, // These aren't documented
    {(ਪੂਰਨ)KS_TI,	"\033[=6h"},
    {(ਪੂਰਨ)KS_TE,	"\033[=6l"},
    {(ਪੂਰਨ)KS_SE,	"\033[21;27m"},
    {(ਪੂਰਨ)KS_SO,	"\033[1;7m"},
    {(ਪੂਰਨ)KS_ME,	"\033[m"},
    {(ਪੂਰਨ)KS_MR,	"\033[7m"},
    {(ਪੂਰਨ)KS_MD,	"\033[1m"},
    {(ਪੂਰਨ)KS_CCO,	"8"},			// allow 8 colors
    {(ਪੂਰਨ)KS_CZH,	"\033[3m"},		// italic mode on
    {(ਪੂਰਨ)KS_CZR,	"\033[23m"},		// italic mode off
    {(ਪੂਰਨ)KS_US,	"\033[4m"},		// underline on
    {(ਪੂਰਨ)KS_UE,	"\033[24m"},		// underline off
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TERMINFO
    {(ਪੂਰਨ)KS_CAB,	"\033[4%p1%dm"},    // set background color (ANSI)
    {(ਪੂਰਨ)KS_CAF,	"\033[3%p1%dm"},    // set foreground color (ANSI)
    {(ਪੂਰਨ)KS_CSB,	"\033[102;%p1%dm"}, // set screen background color
    {(ਪੂਰਨ)KS_CSF,	"\033[101;%p1%dm"}, // set screen foreground color
#ਨਹੀਂਤਾਂ
    {(ਪੂਰਨ)KS_CAB,	"\033[4%dm"},	    // set background color (ANSI)
    {(ਪੂਰਨ)KS_CAF,	"\033[3%dm"},	    // set foreground color (ANSI)
    {(ਪੂਰਨ)KS_CSB,	"\033[102;%dm"},    // set screen background color
    {(ਪੂਰਨ)KS_CSF,	"\033[101;%dm"},    // set screen foreground color
#ਜੇ_ਅੰਤ
    {(ਪੂਰਨ)KS_MS,	"y"},		// guessed
    {(ਪੂਰਨ)KS_UT,	"y"},		// guessed
    {(ਪੂਰਨ)KS_LE,	"\b"},
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TERMINFO
    {(ਪੂਰਨ)KS_CM,	"\033[%i%p1%d;%p2%dH"},
#ਨਹੀਂਤਾਂ
    {(ਪੂਰਨ)KS_CM,	"\033[%i%d;%dH"},
#ਜੇ_ਅੰਤ
    {(ਪੂਰਨ)KS_SR,	"\033M"},
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TERMINFO
    {(ਪੂਰਨ)KS_CRI,	"\033[%p1%dC"},
#ਨਹੀਂਤਾਂ
    {(ਪੂਰਨ)KS_CRI,	"\033[%dC"},
#ਜੇ_ਅੰਤ
    {(ਪੂਰਨ)KS_CIS,	"\033P3.y"},
    {(ਪੂਰਨ)KS_CIE,	"\234"},    // ST "String Terminator"
    {(ਪੂਰਨ)KS_TS,	"\033P1.y"},
    {(ਪੂਰਨ)KS_FS,	"\234"},    // ST "String Terminator"
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TERMINFO
    {(ਪੂਰਨ)KS_CWS,	"\033[203;%p1%d;%p2%d/y"},
    {(ਪੂਰਨ)KS_CWP,	"\033[205;%p1%d;%p2%d/y"},
#ਨਹੀਂਤਾਂ
    {(ਪੂਰਨ)KS_CWS,	"\033[203;%d;%d/y"},
    {(ਪੂਰਨ)KS_CWP,	"\033[205;%d;%d/y"},
#ਜੇ_ਅੰਤ
    {K_ਉੱਪਰ,		"\033[A"},
    {K_ਥੱਲੇ,		"\033[B"},
    {K_ਖੱਬਾ,		"\033[D"},
    {K_ਸੱਜਾ,		"\033[C"},
    {K_S_ਉੱਪਰ,		"\033[161q"},
    {K_S_ਥੱਲੇ,		"\033[164q"},
    {K_S_ਖੱਬਾ,		"\033[158q"},
    {K_S_ਸੱਜਾ,		"\033[167q"},
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
    {K_ਸਿਰਾ,		"\033[146q"},
    {K_PAGEUP,		"\033[150q"},
    {K_PAGEDOWN,	"\033[154q"},

    {(ਪੂਰਨ)KS_NAME,	ਨਿਰਾ}  // end marker
};

/*
 * These codes are valid when nansi.sys or equivalent has been installed.
 * Function keys on a PC are preceded with a NUL. These are converted into
 * K_NUL '\316' in mch_inchar(), because we cannot handle NULs in key codes.
 * CTRL-arrow is used instead of SHIFT-arrow.
 */
ਸਥਿਰ tcap_ਇੰਦਰਾਜ_T builtin_pcansi[] = {
    {(ਪੂਰਨ)KS_DL,	"\033[M"},
    {(ਪੂਰਨ)KS_AL,	"\033[L"},
    {(ਪੂਰਨ)KS_CE,	"\033[K"},
    {(ਪੂਰਨ)KS_CL,	"\033[2J"},
    {(ਪੂਰਨ)KS_ME,	"\033[0m"},
    {(ਪੂਰਨ)KS_MR,	"\033[5m"},	// reverse: black on lightgrey
    {(ਪੂਰਨ)KS_MD,	"\033[1m"},	// bold: white text
    {(ਪੂਰਨ)KS_SE,	"\033[0m"},	// standout end
    {(ਪੂਰਨ)KS_SO,	"\033[31m"},	// standout: white on blue
    {(ਪੂਰਨ)KS_CZH,	"\033[34;43m"},	// italic mode: blue text on yellow
    {(ਪੂਰਨ)KS_CZR,	"\033[0m"},	// italic mode end
    {(ਪੂਰਨ)KS_US,	"\033[36;41m"},	// underscore mode: cyan text on red
    {(ਪੂਰਨ)KS_UE,	"\033[0m"},	// underscore mode end
    {(ਪੂਰਨ)KS_CCO,	"8"},		// allow 8 colors
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TERMINFO
    {(ਪੂਰਨ)KS_CAB,	"\033[4%p1%dm"},// set background color
    {(ਪੂਰਨ)KS_CAF,	"\033[3%p1%dm"},// set foreground color
#ਨਹੀਂਤਾਂ
    {(ਪੂਰਨ)KS_CAB,	"\033[4%dm"},	// set background color
    {(ਪੂਰਨ)KS_CAF,	"\033[3%dm"},	// set foreground color
#ਜੇ_ਅੰਤ
    {(ਪੂਰਨ)KS_OP,	"\033[0m"},	// reset colors
    {(ਪੂਰਨ)KS_MS,	"y"},
    {(ਪੂਰਨ)KS_UT,	"y"},		// guessed
    {(ਪੂਰਨ)KS_LE,	"\b"},
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TERMINFO
    {(ਪੂਰਨ)KS_CM,	"\033[%i%p1%d;%p2%dH"},
#ਨਹੀਂਤਾਂ
    {(ਪੂਰਨ)KS_CM,	"\033[%i%d;%dH"},
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TERMINFO
    {(ਪੂਰਨ)KS_CRI,	"\033[%p1%dC"},
#ਨਹੀਂਤਾਂ
    {(ਪੂਰਨ)KS_CRI,	"\033[%dC"},
#ਜੇ_ਅੰਤ
    {K_ਉੱਪਰ,		"\316H"},
    {K_ਥੱਲੇ,		"\316P"},
    {K_ਖੱਬਾ,		"\316K"},
    {K_ਸੱਜਾ,		"\316M"},
    {K_S_ਖੱਬਾ,		"\316s"},
    {K_S_ਸੱਜਾ,		"\316t"},
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
    {K_ਸਿਰਾ,		"\316O"},
    {K_PAGEDOWN,	"\316Q"},
    {K_PAGEUP,		"\316I"},

    {(ਪੂਰਨ)KS_NAME,	ਨਿਰਾ}  // end marker
};

/*
 * These codes are valid for the Win32 Console.  The entries that start with
 * ESC | are translated into console calls in os_win32.c.  The function keys
 * are also translated in os_win32.c.
 */
ਸਥਿਰ tcap_ਇੰਦਰਾਜ_T builtin_win32[] = {
    {(ਪੂਰਨ)KS_CE,	"\033|K"},	// clear to end of line
    {(ਪੂਰਨ)KS_AL,	"\033|L"},	// add new blank line
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TERMINFO
    {(ਪੂਰਨ)KS_CAL,	"\033|%p1%dL"},	// add number of new blank lines
#ਨਹੀਂਤਾਂ
    {(ਪੂਰਨ)KS_CAL,	"\033|%dL"},	// add number of new blank lines
#ਜੇ_ਅੰਤ
    {(ਪੂਰਨ)KS_DL,	"\033|M"},	// delete line
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TERMINFO
    {(ਪੂਰਨ)KS_CDL,	"\033|%p1%dM"},	// delete number of lines
    {(ਪੂਰਨ)KS_CSV,	"\033|%p1%d;%p2%dV"},
#ਨਹੀਂਤਾਂ
    {(ਪੂਰਨ)KS_CDL,	"\033|%dM"},	// delete number of lines
    {(ਪੂਰਨ)KS_CSV,	"\033|%d;%dV"},
#ਜੇ_ਅੰਤ
    {(ਪੂਰਨ)KS_CL,	"\033|J"},	// clear screen
    {(ਪੂਰਨ)KS_CD,	"\033|j"},	// clear to end of display
    {(ਪੂਰਨ)KS_VI,	"\033|v"},	// cursor invisible
    {(ਪੂਰਨ)KS_VE,	"\033|V"},	// cursor visible

    {(ਪੂਰਨ)KS_ME,	"\033|0m"},	//ਸਾਧਾਰਨ
    {(ਪੂਰਨ)KS_MR,	"\033|112m"},	// reverse: black on lightgray
    {(ਪੂਰਨ)KS_MD,	"\033|15m"},	// bold: white on black
#ਜੇ 1
    {(ਪੂਰਨ)KS_SO,	"\033|31m"},	// standout: white on blue
    {(ਪੂਰਨ)KS_SE,	"\033|0m"},	// standout end
#ਨਹੀਂਤਾਂ
    {(ਪੂਰਨ)KS_SO,	"\033|F"},	// standout: high intensity
    {(ਪੂਰਨ)KS_SE,	"\033|f"},	// standout end
#ਜੇ_ਅੰਤ
    {(ਪੂਰਨ)KS_CZH,	"\033|225m"},	// italic: blue text on yellow
    {(ਪੂਰਨ)KS_CZR,	"\033|0m"},	// italic end
    {(ਪੂਰਨ)KS_US,	"\033|67m"},	// underscore: cyan text on red
    {(ਪੂਰਨ)KS_UE,	"\033|0m"},	// underscore end
    {(ਪੂਰਨ)KS_CCO,	"16"},		// allow 16 colors
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TERMINFO
    {(ਪੂਰਨ)KS_CAB,	"\033|%p1%db"},	// set background color
    {(ਪੂਰਨ)KS_CAF,	"\033|%p1%df"},	// set foreground color
#ਨਹੀਂਤਾਂ
    {(ਪੂਰਨ)KS_CAB,	"\033|%db"},	// set background color
    {(ਪੂਰਨ)KS_CAF,	"\033|%df"},	// set foreground color
#ਜੇ_ਅੰਤ

    {(ਪੂਰਨ)KS_MS,	"y"},		// save to move cur in reverse mode
    {(ਪੂਰਨ)KS_UT,	"y"},
    {(ਪੂਰਨ)KS_XN,	"y"},
    {(ਪੂਰਨ)KS_LE,	"\b"},
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TERMINFO
    {(ਪੂਰਨ)KS_CM,	"\033|%i%p1%d;%p2%dH"}, // cursor motion
#ਨਹੀਂਤਾਂ
    {(ਪੂਰਨ)KS_CM,	"\033|%i%d;%dH"}, // cursor motion
#ਜੇ_ਅੰਤ
    {(ਪੂਰਨ)KS_VB,	"\033|B"},	// visual bell
    {(ਪੂਰਨ)KS_TI,	"\033|S"},	// put terminal in termcap mode
    {(ਪੂਰਨ)KS_TE,	"\033|E"},	// out of termcap mode
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TERMINFO
    {(ਪੂਰਨ)KS_CS,	"\033|%i%p1%d;%p2%dr"}, // scroll region
#ਨਹੀਂਤਾਂ
    {(ਪੂਰਨ)KS_CS,	"\033|%i%d;%dr"}, // scroll region
#ਜੇ_ਅੰਤ

    {K_ਉੱਪਰ,		"\316H"},
    {K_ਥੱਲੇ,		"\316P"},
    {K_ਖੱਬਾ,		"\316K"},
    {K_ਸੱਜਾ,		"\316M"},
    {K_S_ਉੱਪਰ,		"\316\304"},
    {K_S_ਥੱਲੇ,		"\316\317"},
    {K_S_ਖੱਬਾ,		"\316\311"},
    {K_C_ਖੱਬਾ,		"\316s"},
    {K_S_ਸੱਜਾ,		"\316\313"},
    {K_C_ਸੱਜਾ,		"\316t"},
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
    {K_ਸਿਰਾ,		"\316O"},
    {K_S_ਸਿਰਾ,		"\316\315"},
    {K_C_ਸਿਰਾ,		"\316u"},
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

    {(ਪੂਰਨ)KS_NAME,	ਨਿਰਾ}  // end marker
};

#ਜੇ defined(FEAT_GUI)
/*
 * GUI uses made-up codes, only used inside Vim.
 */
ਸਥਿਰ tcap_ਇੰਦਰਾਜ_T builtin_gui[] = {
    {(ਪੂਰਨ)KS_CE,	"\033|$"},
    {(ਪੂਰਨ)KS_AL,	"\033|i"},
# ifdef TERMINFO
    {(ਪੂਰਨ)KS_CAL,	"\033|%p1%dI"},
# ਨਹੀਂਤਾਂ
    {(ਪੂਰਨ)KS_CAL,	"\033|%dI"},
# endif
    {(ਪੂਰਨ)KS_DL,	"\033|d"},
# ifdef TERMINFO
    {(ਪੂਰਨ)KS_CDL,	"\033|%p1%dD"},
    {(ਪੂਰਨ)KS_CS,	"\033|%p1%d;%p2%dR"},
    {(ਪੂਰਨ)KS_CSV,	"\033|%p1%d;%p2%dV"},
# ਨਹੀਂਤਾਂ
    {(ਪੂਰਨ)KS_CDL,	"\033|%dD"},
    {(ਪੂਰਨ)KS_CS,	"\033|%d;%dR"},
    {(ਪੂਰਨ)KS_CSV,	"\033|%d;%dV"},
# endif
    {(ਪੂਰਨ)KS_CL,	"\033|C"},
			// attributes switched on with 'h', off with * 'H'
    {(ਪੂਰਨ)KS_ME,	"\033|31H"}, // HL_ALL
    {(ਪੂਰਨ)KS_MR,	"\033|1h"},   // HL_INVERSE
    {(ਪੂਰਨ)KS_MD,	"\033|2h"},   // HL_BOLD
    {(ਪੂਰਨ)KS_SE,	"\033|16H"}, // HL_STANDOUT
    {(ਪੂਰਨ)KS_SO,	"\033|16h"}, // HL_STANDOUT
    {(ਪੂਰਨ)KS_UE,	"\033|8H"},   // HL_UNDERLINE
    {(ਪੂਰਨ)KS_US,	"\033|8h"},   // HL_UNDERLINE
    {(ਪੂਰਨ)KS_UCE,	"\033|8C"},   // HL_UNDERCURL
    {(ਪੂਰਨ)KS_UCS,	"\033|8c"},   // HL_UNDERCURL
    {(ਪੂਰਨ)KS_STE,	"\033|4C"},   // HL_STRIKETHROUGH
    {(ਪੂਰਨ)KS_STS,	"\033|4c"},   // HL_STRIKETHROUGH
    {(ਪੂਰਨ)KS_CZR,	"\033|4H"},   // HL_ITALIC
    {(ਪੂਰਨ)KS_CZH,	"\033|4h"},   // HL_ITALIC
    {(ਪੂਰਨ)KS_VB,	"\033|f"},
    {(ਪੂਰਨ)KS_MS,	"y"},
    {(ਪੂਰਨ)KS_UT,	"y"},
    {(ਪੂਰਨ)KS_XN,	"y"},
    {(ਪੂਰਨ)KS_LE,	"\b"},		// cursor-left = BS
    {(ਪੂਰਨ)KS_ND,	"\014"},	// cursor-right = CTRL-L
# ifdef TERMINFO
    {(ਪੂਰਨ)KS_CM,	"\033|%p1%d;%p2%dM"},
# ਨਹੀਂਤਾਂ
    {(ਪੂਰਨ)KS_CM,	"\033|%d;%dM"},
# endif
	// there are no key sequences here, the GUI sequences are recognized
	// in check_termcode()

    {(ਪੂਰਨ)KS_NAME,	ਨਿਰਾ}  // end marker
};
#ਜੇ_ਅੰਤ

/*
 * Amiga console window, default for Amiga.
 */
ਸਥਿਰ tcap_ਇੰਦਰਾਜ_T builtin_amiga[] = {
    {(ਪੂਰਨ)KS_CE,	"\033[K"},
    {(ਪੂਰਨ)KS_CD,	"\033[J"},
    {(ਪੂਰਨ)KS_AL,	"\033[L"},
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TERMINFO
    {(ਪੂਰਨ)KS_CAL,	"\033[%p1%dL"},
#ਨਹੀਂਤਾਂ
    {(ਪੂਰਨ)KS_CAL,	"\033[%dL"},
#ਜੇ_ਅੰਤ
    {(ਪੂਰਨ)KS_DL,	"\033[M"},
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TERMINFO
    {(ਪੂਰਨ)KS_CDL,	"\033[%p1%dM"},
#ਨਹੀਂਤਾਂ
    {(ਪੂਰਨ)KS_CDL,	"\033[%dM"},
#ਜੇ_ਅੰਤ
    {(ਪੂਰਨ)KS_CL,	"\014"},
    {(ਪੂਰਨ)KS_VI,	"\033[0 p"},
    {(ਪੂਰਨ)KS_VE,	"\033[1 p"},
    {(ਪੂਰਨ)KS_ME,	"\033[0m"},
    {(ਪੂਰਨ)KS_MR,	"\033[7m"},
    {(ਪੂਰਨ)KS_MD,	"\033[1m"},
    {(ਪੂਰਨ)KS_SE,	"\033[0m"},
    {(ਪੂਰਨ)KS_SO,	"\033[33m"},
    {(ਪੂਰਨ)KS_US,	"\033[4m"},
    {(ਪੂਰਨ)KS_UE,	"\033[0m"},
    {(ਪੂਰਨ)KS_CZH,	"\033[3m"},
    {(ਪੂਰਨ)KS_CZR,	"\033[0m"},
#ਜੇ defined(__amigaos4__) || defined(__MORPHOS__) || defined(__AROS__)
    {(ਪੂਰਨ)KS_CCO,	"8"},		// allow 8 colors
# ifdef TERMINFO
    {(ਪੂਰਨ)KS_CAB,	"\033[4%p1%dm"},// set background color
    {(ਪੂਰਨ)KS_CAF,	"\033[3%p1%dm"},// set foreground color
# ਨਹੀਂਤਾਂ
    {(ਪੂਰਨ)KS_CAB,	"\033[4%dm"},	// set background color
    {(ਪੂਰਨ)KS_CAF,	"\033[3%dm"},	// set foreground color
# endif
    {(ਪੂਰਨ)KS_OP,	"\033[m"},	// reset colors
#ਜੇ_ਅੰਤ
    {(ਪੂਰਨ)KS_MS,	"y"},
    {(ਪੂਰਨ)KS_UT,	"y"},		// guessed
    {(ਪੂਰਨ)KS_LE,	"\b"},
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TERMINFO
    {(ਪੂਰਨ)KS_CM,	"\033[%i%p1%d;%p2%dH"},
#ਨਹੀਂਤਾਂ
    {(ਪੂਰਨ)KS_CM,	"\033[%i%d;%dH"},
#ਜੇ_ਅੰਤ
#ਜੇ defined(__MORPHOS__)
    {(ਪੂਰਨ)KS_SR,	"\033M"},
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TERMINFO
    {(ਪੂਰਨ)KS_CRI,	"\033[%p1%dC"},
#ਨਹੀਂਤਾਂ
    {(ਪੂਰਨ)KS_CRI,	"\033[%dC"},
#ਜੇ_ਅੰਤ
    {K_ਉੱਪਰ,		"\233A"},
    {K_ਥੱਲੇ,		"\233B"},
    {K_ਖੱਬਾ,		"\233D"},
    {K_ਸੱਜਾ,		"\233C"},
    {K_S_ਉੱਪਰ,		"\233T"},
    {K_S_ਥੱਲੇ,		"\233S"},
    {K_S_ਖੱਬਾ,		"\233 A"},
    {K_S_ਸੱਜਾ,		"\233 @"},
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
    {K_ਸਿਰਾ,		"\233\064\065~"},	// 101 key keyboard

    {BT_EXTRA_ਕੁੰਜੀਆਂ,	""},
    {TERMCAP2_ਕੁੰਜੀ('#', '2'), "\233\065\064~"},	// shifted home key
    {TERMCAP2_ਕੁੰਜੀ('#', '3'), "\233\065\060~"},	// shifted insert key
    {TERMCAP2_ਕੁੰਜੀ('*', '7'), "\233\065\065~"},	// shifted end key

    {(ਪੂਰਨ)KS_NAME,	ਨਿਰਾ}  // end marker
};

/*
 * The most minimal terminal: only clear screen and cursor positioning.
 */
ਸਥਿਰ tcap_ਇੰਦਰਾਜ_T builtin_dumb[] = {
    {(ਪੂਰਨ)KS_CL,	"\014"},
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TERMINFO
    {(ਪੂਰਨ)KS_CM,	"\033[%i%p1%d;%p2%dH"},
#ਨਹੀਂਤਾਂ
    {(ਪੂਰਨ)KS_CM,	"\033[%i%d;%dH"},
#ਜੇ_ਅੰਤ

    {(ਪੂਰਨ)KS_NAME,	ਨਿਰਾ}  // end marker
};

/*
 * Terminal used for debugging.
 */
ਸਥਿਰ tcap_ਇੰਦਰਾਜ_T builtin_debug[] = {
    {(ਪੂਰਨ)KS_CE,	"[CE]"},
    {(ਪੂਰਨ)KS_CD,	"[CD]"},
    {(ਪੂਰਨ)KS_AL,	"[AL]"},
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TERMINFO
    {(ਪੂਰਨ)KS_CAL,	"[CAL%p1%d]"},
#ਨਹੀਂਤਾਂ
    {(ਪੂਰਨ)KS_CAL,	"[CAL%d]"},
#ਜੇ_ਅੰਤ
    {(ਪੂਰਨ)KS_DL,	"[DL]"},
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TERMINFO
    {(ਪੂਰਨ)KS_CDL,	"[CDL%p1%d]"},
#ਨਹੀਂਤਾਂ
    {(ਪੂਰਨ)KS_CDL,	"[CDL%d]"},
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TERMINFO
    {(ਪੂਰਨ)KS_CS,	"[%p1%dCS%p2%d]"},
#ਨਹੀਂਤਾਂ
    {(ਪੂਰਨ)KS_CS,	"[%dCS%d]"},
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TERMINFO
    {(ਪੂਰਨ)KS_CSV,	"[%p1%dCSV%p2%d]"},
#ਨਹੀਂਤਾਂ
    {(ਪੂਰਨ)KS_CSV,	"[%dCSV%d]"},
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TERMINFO
    {(ਪੂਰਨ)KS_CAB,	"[CAB%p1%d]"},
    {(ਪੂਰਨ)KS_CAF,	"[CAF%p1%d]"},
    {(ਪੂਰਨ)KS_CSB,	"[CSB%p1%d]"},
    {(ਪੂਰਨ)KS_CSF,	"[CSF%p1%d]"},
#ਨਹੀਂਤਾਂ
    {(ਪੂਰਨ)KS_CAB,	"[CAB%d]"},
    {(ਪੂਰਨ)KS_CAF,	"[CAF%d]"},
    {(ਪੂਰਨ)KS_CSB,	"[CSB%d]"},
    {(ਪੂਰਨ)KS_CSF,	"[CSF%d]"},
#ਜੇ_ਅੰਤ
    {(ਪੂਰਨ)KS_CAU,	"[CAU%d]"},
    {(ਪੂਰਨ)KS_OP,	"[OP]"},
    {(ਪੂਰਨ)KS_LE,	"[LE]"},
    {(ਪੂਰਨ)KS_CL,	"[CL]"},
    {(ਪੂਰਨ)KS_VI,	"[VI]"},
    {(ਪੂਰਨ)KS_VE,	"[VE]"},
    {(ਪੂਰਨ)KS_VS,	"[VS]"},
    {(ਪੂਰਨ)KS_ME,	"[ME]"},
    {(ਪੂਰਨ)KS_MR,	"[MR]"},
    {(ਪੂਰਨ)KS_MB,	"[MB]"},
    {(ਪੂਰਨ)KS_MD,	"[MD]"},
    {(ਪੂਰਨ)KS_SE,	"[SE]"},
    {(ਪੂਰਨ)KS_SO,	"[SO]"},
    {(ਪੂਰਨ)KS_UE,	"[UE]"},
    {(ਪੂਰਨ)KS_US,	"[US]"},
    {(ਪੂਰਨ)KS_UCE,	"[UCE]"},
    {(ਪੂਰਨ)KS_UCS,	"[UCS]"},
    {(ਪੂਰਨ)KS_USS,	"[USS]"},
    {(ਪੂਰਨ)KS_DS,	"[DS]"},
    {(ਪੂਰਨ)KS_CDS,	"[CDS]"},
    {(ਪੂਰਨ)KS_STE,	"[STE]"},
    {(ਪੂਰਨ)KS_STS,	"[STS]"},
    {(ਪੂਰਨ)KS_MS,	"[MS]"},
    {(ਪੂਰਨ)KS_UT,	"[UT]"},
    {(ਪੂਰਨ)KS_XN,	"[XN]"},
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TERMINFO
    {(ਪੂਰਨ)KS_CM,	"[%p1%dCM%p2%d]"},
#ਨਹੀਂਤਾਂ
    {(ਪੂਰਨ)KS_CM,	"[%dCM%d]"},
#ਜੇ_ਅੰਤ
    {(ਪੂਰਨ)KS_SR,	"[SR]"},
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TERMINFO
    {(ਪੂਰਨ)KS_CRI,	"[CRI%p1%d]"},
#ਨਹੀਂਤਾਂ
    {(ਪੂਰਨ)KS_CRI,	"[CRI%d]"},
#ਜੇ_ਅੰਤ
    {(ਪੂਰਨ)KS_VB,	"[VB]"},
    {(ਪੂਰਨ)KS_KS,	"[KS]"},
    {(ਪੂਰਨ)KS_KE,	"[KE]"},
    {(ਪੂਰਨ)KS_TI,	"[TI]"},
    {(ਪੂਰਨ)KS_TE,	"[TE]"},
    {(ਪੂਰਨ)KS_CIS,	"[CIS]"},
    {(ਪੂਰਨ)KS_CIE,	"[CIE]"},
    {(ਪੂਰਨ)KS_CSC,	"[CSC]"},
    {(ਪੂਰਨ)KS_CEC,	"[CEC]"},
    {(ਪੂਰਨ)KS_TS,	"[TS]"},
    {(ਪੂਰਨ)KS_FS,	"[FS]"},
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TERMINFO
    {(ਪੂਰਨ)KS_CWS,	"[%p1%dCWS%p2%d]"},
    {(ਪੂਰਨ)KS_CWP,	"[%p1%dCWP%p2%d]"},
#ਨਹੀਂਤਾਂ
    {(ਪੂਰਨ)KS_CWS,	"[%dCWS%d]"},
    {(ਪੂਰਨ)KS_CWP,	"[%dCWP%d]"},
#ਜੇ_ਅੰਤ
    {(ਪੂਰਨ)KS_CRV,	"[CRV]"},
    {(ਪੂਰਨ)KS_CXM,	"[CXM]"},
    {(ਪੂਰਨ)KS_U7,	"[U7]"},
    {(ਪੂਰਨ)KS_RFG,	"[RFG]"},
    {(ਪੂਰਨ)KS_RBG,	"[RBG]"},
    {(ਪੂਰਨ)KS_CF,	"[CF%d]"},
    {K_ਉੱਪਰ,		"[KU]"},
    {K_ਥੱਲੇ,		"[KD]"},
    {K_ਖੱਬਾ,		"[KL]"},
    {K_ਸੱਜਾ,		"[KR]"},
    {K_XUP,		"[xKU]"},
    {K_XDOWN,		"[xKD]"},
    {K_XLEFT,		"[xKL]"},
    {K_XRIGHT,		"[xKR]"},
    {K_S_ਉੱਪਰ,		"[S-KU]"},
    {K_S_ਥੱਲੇ,		"[S-KD]"},
    {K_S_ਖੱਬਾ,		"[S-KL]"},
    {K_C_ਖੱਬਾ,		"[C-KL]"},
    {K_S_ਸੱਜਾ,		"[S-KR]"},
    {K_C_ਸੱਜਾ,		"[C-KR]"},
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
    {K_ਸਿਰਾ,		"[END]"},
    {K_S_ਸਿਰਾ,		"[C-END]"},
    {K_C_ਸਿਰਾ,		"[C-END]"},
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

    {(ਪੂਰਨ)KS_NAME,	ਨਿਰਾ}  // end marker
};

/*
 * List of builtin terminals.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    ਅੱਖਰ	    *bitc_name;	    // name, such as "xterm"
    tcap_ਇੰਦਰਾਜ_T    *bitc_ਸਾਰਣੀ;    // table with entries for bitc_name
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
#ਜੇ defined(FEAT_GUI)
    {"gui",	    builtin_gui},
#ਜੇ_ਅੰਤ
    {"amiga",	    builtin_amiga},
    {"dumb",	    builtin_dumb},
    {"debug",	    builtin_debug},

    {ਨਿਰਾ,	    ਨਿਰਾ},  // end marker
};

#ਜੇ defined(FEAT_TERMGUICOLORS)
    ਸਥਿਰ guicolor_T
termgui_mch_get_color(char_u *name)
{
    ਵਾਪਸ gui_get_color_cmn(name);
}

    guicolor_T
termgui_get_color(char_u *name)
{
    guicolor_T	t;

    ਜੇ (*name == NUL)
	ਵਾਪਸ INVALCOLOR;
    t = termgui_mch_get_color(name);

    ਜੇ (t == INVALCOLOR)
	semsg(_(e_cannot_allocate_color_str), name);
    ਵਾਪਸ t;
}

    guicolor_T
termgui_mch_get_rgb(guicolor_T color)
{
    ਵਾਪਸ color;
}
#ਜੇ_ਅੰਤ

/*
 * DEFAULT_TERM is used, when no terminal is specified with -T option or $TERM.
 */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ AMIGA
# define DEFAULT_ਪਦ	(char_u *)"amiga"
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
# define DEFAULT_ਪਦ	(char_u *)"win32"
#ਜੇ_ਅੰਤ

#ਜੇ defined(UNIX)
# define DEFAULT_ਪਦ	(char_u *)"ansi"
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ VMS
# define DEFAULT_ਪਦ	(char_u *)"vt320"
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __HAIKU__
# undef DEFAULT_ਪਦ
# define DEFAULT_ਪਦ	(char_u *)"xterm"
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ DEFAULT_ਪਦ
# define DEFAULT_ਪਦ	(char_u *)"dumb"
#ਜੇ_ਅੰਤ

/*
 * Term_strings contains currently used terminal output strings.
 * It is initialized with the default values by parse_builtin_tcap().
 * The values can be changed by setting the option with the same name.
 */
char_u *(ਪਦ_ਲੜੀਆਂ[(ਪੂਰਨ)KS_ਆਖਰੀ + 1]);

ਸਥਿਰ ਪੂਰਨ	need_gather = FALSE;	    // need to fill termleader[]
ਸਥਿਰ char_u	termleader[256 + 1];	    // for check_termcode()
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMRESPONSE
ਸਥਿਰ ਪੂਰਨ	check_for_codes = FALSE;    // check for key code response
#ਜੇ_ਅੰਤ

/*
 * Structure and table to store terminal features that can be detected by
 * querying the terminal.  Either by inspecting the termresponse or a more
 * specific request.  Besides this there are:
 * t_colors - number of colors supported
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    ਅੱਖਰ    *tpr_name;
    ਪੂਰਨ	    tpr_ਸੈੱਟ_by_termresponse;
    ਪੂਰਨ	    tpr_ਸਥਿਤੀ;
} termprop_T;

// Values for tpr_status.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TPR_UNKNOWN	    'u'
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TPR_YES		    'y'
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TPR_NO		    'n'
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TPR_MOUSE_XTERM     'x'	// use "xterm" for 'ttymouse'
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TPR_MOUSE_XTERM2    '2'	// use "xterm2" for 'ttymouse'
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TPR_MOUSE_SGR	    's'	// use "sgr" for 'ttymouse'

// can request the cursor style without messing up the display
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TPR_CURSOR_STYLE	    0
// can request the cursor blink mode without messing up the display
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TPR_CURSOR_BLINK	    1
// can set the underline color with t_8u without resetting other colors
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TPR_UNDERLINE_RGB	    2
// mouse support - TPR_MOUSE_XTERM, TPR_MOUSE_XTERM2 or TPR_MOUSE_SGR
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TPR_MOUSE		    3
// term response indicates kitty
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TPR_KITTY		    4
// can send DECRQM requests to terminal
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TPR_DECRQM		    5
// true color, from the color count of the terminal entry
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TPR_RGB			    6
// table size
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TPR_ਗਿਣਤੀ		    7

ਸਥਿਰ termprop_T ਪਦ_props[TPR_ਗਿਣਤੀ];

/*
 * Set the "rgb" terminal property from the color count.  The entry of a
 * direct color terminal, such as xterm-direct, has 16777216 colors.
 */
    ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_rgb_ਪਦ_prop(ਖਾਲੀ)
{
    ਪਦ_props[TPR_RGB].tpr_ਸਥਿਤੀ = t_colors == 0x1000000
						       ? TPR_YES : TPR_UNKNOWN;
}

/*
 * Initialize the term_props table.
 * When "all" is FALSE only set those that are detected from the version
 * response.
 */
    ਖਾਲੀ
init_ਪਦ_props(ਪੂਰਨ all)
{
    ਪੂਰਨ i;

    ਪਦ_props[TPR_CURSOR_STYLE].tpr_name = "cursor_style";
    ਪਦ_props[TPR_CURSOR_STYLE].tpr_ਸੈੱਟ_by_termresponse = FALSE;
    ਪਦ_props[TPR_CURSOR_BLINK].tpr_name = "cursor_blink_mode";
    ਪਦ_props[TPR_CURSOR_BLINK].tpr_ਸੈੱਟ_by_termresponse = FALSE;
    ਪਦ_props[TPR_UNDERLINE_RGB].tpr_name = "underline_rgb";
    ਪਦ_props[TPR_UNDERLINE_RGB].tpr_ਸੈੱਟ_by_termresponse = TRUE;
    ਪਦ_props[TPR_MOUSE].tpr_name = "mouse";
    ਪਦ_props[TPR_MOUSE].tpr_ਸੈੱਟ_by_termresponse = TRUE;
    ਪਦ_props[TPR_KITTY].tpr_name = "kitty";
    ਪਦ_props[TPR_KITTY].tpr_ਸੈੱਟ_by_termresponse = FALSE;
    ਪਦ_props[TPR_DECRQM].tpr_name = "decrqm";
    ਪਦ_props[TPR_DECRQM].tpr_ਸੈੱਟ_by_termresponse = TRUE;
    ਪਦ_props[TPR_RGB].tpr_name = "rgb";
    ਪਦ_props[TPR_RGB].tpr_ਸੈੱਟ_by_termresponse = FALSE;

    ਲਈ (i = 0; i < TPR_ਗਿਣਤੀ; ++i)
	ਜੇ (all || ਪਦ_props[i].tpr_ਸੈੱਟ_by_termresponse)
	    ਪਦ_props[i].tpr_ਸਥਿਤੀ = TPR_UNKNOWN;

    // Derived from the color count, not reset.
    ਸੈੱਟ_rgb_ਪਦ_prop();
}

#ਜੇ defined(FEAT_EVAL)
    ਖਾਲੀ
f_terminalprops(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
# ifdef FEAT_TERMRESPONSE
    ਪੂਰਨ i;
# endif

    ਜੇ (rettv_dict_alloc(rettv) == FAIL)
	ਵਾਪਸ;
# ifdef FEAT_TERMRESPONSE
    ਲਈ (i = 0; i < TPR_ਗਿਣਤੀ; ++i)
    {
	char_u	ਮੁੱਲ[2];

	ਮੁੱਲ[0] = ਪਦ_props[i].tpr_ਸਥਿਤੀ;
	ਮੁੱਲ[1] = NUL;
	dict_ਜੋੜੋ_string_len(rettv->vval.v_dict, ਪਦ_props[i].tpr_name,
	    ਮੁੱਲ, (ਮੁੱਲ[0] == NUL) ? 0 : 1);
    }
# endif
}
#ਜੇ_ਅੰਤ

/*
 * Find the builtin termcap entries for "term".
 * This also recognizes similar names.  E.g. "xterm-256color" finds the "xterm"
 * entry.
 * Returns NULL when "term" is not found.
 */
    ਸਥਿਰ tcap_ਇੰਦਰਾਜ_T *
ਲੱਭੋ_builtin_ਪਦ(char_u *ਪਦ)
{
    ਲਈ (ਪੂਰਨ i = 0; ; ++i)
    {
	char_u *name = (char_u *)builtin_terminals[i].bitc_name;
	ਜੇ (name == ਨਿਰਾ)  // end marker
	    ਰੋਕੋ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
	ਜੇ (STRCMP(name, "iris-ansi") == 0 && vim_is_iris(ਪਦ))
	    ਵਾਪਸ builtin_terminals[i].bitc_ਸਾਰਣੀ;
	ਜੇ (STRCMP(name, "xterm") == 0 && vim_is_xterm(ਪਦ))
	    ਵਾਪਸ builtin_terminals[i].bitc_ਸਾਰਣੀ;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ VMS
	ਜੇ (STRCMP(name, "vt320") == 0 && vim_is_vt300(ਪਦ))
	    ਵਾਪਸ builtin_terminals[i].bitc_ਸਾਰਣੀ;
#ਜੇ_ਅੰਤ
	ਜੇ (STRCMP(ਪਦ, name) == 0)
	    ਵਾਪਸ builtin_terminals[i].bitc_ਸਾਰਣੀ;
    }
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Apply entries from a builtin termcap.
 */
    ਸਥਿਰ ਖਾਲੀ
apply_builtin_tcap(char_u *ਪਦ, tcap_ਇੰਦਰਾਜ_T *entries, ਪੂਰਨ overwrite)
{
    ਪੂਰਨ ਪਦ_8bit = ਪਦ_is_8bit(ਪਦ);

    ਲਈ (tcap_ਇੰਦਰਾਜ_T *p = entries;
	      p->bt_ਇੰਦਰਾਜ != (ਪੂਰਨ)KS_NAME && p->bt_ਇੰਦਰਾਜ != BT_EXTRA_ਕੁੰਜੀਆਂ; ++p)
    {
	ਜੇ ((ਪੂਰਨ)p->bt_ਇੰਦਰਾਜ >= 0)	// KS_xx entry
	{
	    // Only set the value if it wasn't set yet or "overwrite" is TRUE.
	    ਜੇ (ਪਦ_ਲੜੀਆਂ[p->bt_ਇੰਦਰਾਜ] == ਨਿਰਾ
		    || ਪਦ_ਲੜੀਆਂ[p->bt_ਇੰਦਰਾਜ] == ਖਾਲੀ_ਥਾਵਾਂ_option
		    || overwrite)
	    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
		ਪੂਰਨ opt_idx = -1;
#ਜੇ_ਅੰਤ
		// 8bit terminal: use CSI instead of <Esc>[
		ਜੇ (ਪਦ_8bit && ਪਦ_7to8bit((char_u *)p->bt_string) != 0)
		{
		    char_u  *s, *t;

		    s = vim_strsave((char_u *)p->bt_string);
		    ਜੇ (s != ਨਿਰਾ)
		    {
			ਲਈ (t = s; *t; ++t)
			    ਜੇ (ਪਦ_7to8bit(t))
			    {
				*t = ਪਦ_7to8bit(t);
				STRMOVE(t + 1, t + 2);
			    }
			ਪਦ_ਲੜੀਆਂ[p->bt_ਇੰਦਰਾਜ] = s;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
			opt_idx =
#ਜੇ_ਅੰਤ
				  ਸੈੱਟ_ਪਦ_option_alloced(
						   &ਪਦ_ਲੜੀਆਂ[p->bt_ਇੰਦਰਾਜ]);
		    }
		}
		ਨਹੀਂਤਾਂ
		{
		    ਪਦ_ਲੜੀਆਂ[p->bt_ਇੰਦਰਾਜ] = (char_u *)p->bt_string;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
		    opt_idx = get_ਪਦ_opt_idx(&ਪਦ_ਲੜੀਆਂ[p->bt_ਇੰਦਰਾਜ]);
#ਜੇ_ਅੰਤ
		}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
		ਸੈੱਟ_ਪਦ_option_sctx_idx(ਨਿਰਾ, opt_idx);
#ਜੇ_ਅੰਤ
	    }
	}
	ਨਹੀਂਤਾਂ
	{
	    char_u  name[2];
	    name[0] = KEY2TERMCAP0((ਪੂਰਨ)p->bt_ਇੰਦਰਾਜ);
	    name[1] = KEY2TERMCAP1((ਪੂਰਨ)p->bt_ਇੰਦਰਾਜ);
	    ਜੇ (ਲੱਭੋ_termcode(name) == ਨਿਰਾ || overwrite)
		ਜੋੜੋ_termcode(name, (char_u *)p->bt_string, ਪਦ_8bit);
	}
    }
}

/*
 * Apply builtin termcap entries for a given keyprotocol.
 */
    ਖਾਲੀ
apply_keyprotocol(char_u *ਪਦ, keyprot_T prot)
{
    ਜੇ (prot == KEYPROTOCOL_KITTY)
	apply_builtin_tcap(ਪਦ, builtin_kitty, TRUE);
    ਜੇ (prot == KEYPROTOCOL_MOK2)
	apply_builtin_tcap(ਪਦ, builtin_mok2, TRUE);

    ਜੇ (prot != KEYPROTOCOL_NONE)
	// Some function keys may accept modifiers even though the
	// terminfo/termcap entry does not indicate this.
	accept_modifiers_for_function_ਕੁੰਜੀਆਂ();
}

/*
 * Parsing of the builtin termcap entries.
 * Caller should check if "term" is a valid builtin terminal name.
 * The terminal's name is not set, as this is already done in termcapinit().
 */
    ਸਥਿਰ ਖਾਲੀ
parse_builtin_tcap(char_u *ਪਦ)
{
    tcap_ਇੰਦਰਾਜ_T *entries = ਲੱਭੋ_builtin_ਪਦ(ਪਦ);
    ਜੇ (entries != ਨਿਰਾ)
	apply_builtin_tcap(ਪਦ, entries, FALSE);
}

/*
 * Set number of colors.
 * Store it as a number in t_colors.
 * Store it as a string in T_CCO (using nr_colors[]).
 */
    ਖਾਲੀ
ਸੈੱਟ_color_ਗਿਣਤੀ(ਪੂਰਨ nr)
{
    char_u	nr_colors[20];		// string for number of colors

    t_colors = nr;
    ਜੇ (t_colors > 1)
	sprintf((ਅੱਖਰ *)nr_colors, "%d", t_colors);
    ਨਹੀਂਤਾਂ
	*nr_colors = NUL;
#ਜੇ 0
# ifdef FEAT_TERMGUICOLORS
    // xterm-direct, enable termguicolors, when it wasn't set yet
    ਜੇ (t_colors == 0x1000000 && !p_tgc_ਸੈੱਟ)
	ਸੈੱਟ_option_ਮੁੱਲ((char_u *)"termguicolors", 1L, ਨਿਰਾ, 0);
# endif
#ਜੇ_ਅੰਤ
    ਸੈੱਟ_string_option_ਸਿੱਧਾ((char_u *)"t_Co", -1, nr_colors, OPT_FREE, 0);
}

/*
 * Set the color count to "val" and redraw if it changed.
 */
    ਸਥਿਰ ਖਾਲੀ
may_adjust_color_ਗਿਣਤੀ(ਪੂਰਨ val)
{
    ਜੇ (val == t_colors)
	ਵਾਪਸ;

    // Nr of colors changed, initialize highlighting and redraw everything.
    // This causes a redraw, which usually clears the message.  Try keeping
    // the message if it might work.
    ਸੈੱਟ_keep_ਸੁਨੇਹਾ_from_hist();
    ਸੈੱਟ_color_ਗਿਣਤੀ(val);
    init_highlight(TRUE, FALSE);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG_TERMRESPONSE
    {
	ਪੂਰਨ r = redraw_asap(UPD_CLEAR);

	LOG_TRN("Received t_Co, redraw_asap(): %d", r);
    }
#ਨਹੀਂਤਾਂ
    redraw_asap(UPD_CLEAR);
#ਜੇ_ਅੰਤ
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_TGETENT
ਸਥਿਰ ਅੱਖਰ *(ਕੁੰਜੀ_ਨਾਂ[]) =
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
    ਨਿਰਾ
};
#ਜੇ_ਅੰਤ

#ਜੇ defined(HAVE_TGETENT) || defined(FEAT_TERMGUICOLORS)
/*
 * Return TRUE if "term_strings[idx]" was not set.
 */
    ਸਥਿਰ ਪੂਰਨ
ਪਦ_ਲੜੀਆਂ_not_ਸੈੱਟ(ਗਿਣਤੀ Special_ਕੁੰਜੀ idx)
{
    ਵਾਪਸ ਪਦ_STR(idx) == ਨਿਰਾ || ਪਦ_STR(idx) == ਖਾਲੀ_ਥਾਵਾਂ_option;
}
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_TGETENT
/*
 * Get the termcap entries we need with tgetstr(), tgetflag() and tgetnum().
 * "invoke_tgetent()" must have been called before.
 * If "*height" or "*width" are not zero then use the "li" and "col" entries to
 * get their value.
 */
    ਸਥਿਰ ਖਾਲੀ
get_ਪਦ_entries(ਪੂਰਨ *ਉਚਾਈ, ਪੂਰਨ *ਚੌੜਾਈ)
{
    ਸਥਿਰ ਬਣਤਰ {
		    ਗਿਣਤੀ Special_ਕੁੰਜੀ dest; // index in term_strings[]
		    ਅੱਖਰ *name;		  // termcap name for string
		  } string_ਨਾਂ[] =
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
			{(ਗਿਣਤੀ Special_ਕੁੰਜੀ)0, ਨਿਰਾ}
		    };
    ਪੂਰਨ		    i;
    ਸਥਿਰ char_u   tstrbuf[TBUFSZ];
    char_u	    *tp = tstrbuf;

    /*
     * get output strings
     */
    ਲਈ (i = 0; string_ਨਾਂ[i].name != ਨਿਰਾ; ++i)
    {
	ਜੇ (ਪਦ_ਲੜੀਆਂ_not_ਸੈੱਟ(string_ਨਾਂ[i].dest))
	{
	    ਪਦ_STR(string_ਨਾਂ[i].dest) = TGETSTR(string_ਨਾਂ[i].name, &tp);
# ifdef FEAT_EVAL
	    ਸੈੱਟ_ਪਦ_option_sctx_idx(string_ਨਾਂ[i].name, -1);
# endif
	}
    }

    // tgetflag() returns 1 if the flag is present, 0 if not and
    // possibly -1 if the flag doesn't exist.
    ਜੇ ((T_MS == ਨਿਰਾ || T_MS == ਖਾਲੀ_ਥਾਵਾਂ_option) && tgetflag("ms") > 0)
	T_MS = (char_u *)"y";
    ਜੇ ((T_XS == ਨਿਰਾ || T_XS == ਖਾਲੀ_ਥਾਵਾਂ_option) && tgetflag("xs") > 0)
	T_XS = (char_u *)"y";
    ਜੇ ((T_XN == ਨਿਰਾ || T_XN == ਖਾਲੀ_ਥਾਵਾਂ_option) && tgetflag("xn") > 0)
	T_XN = (char_u *)"y";
    ਜੇ ((T_DB == ਨਿਰਾ || T_DB == ਖਾਲੀ_ਥਾਵਾਂ_option) && tgetflag("db") > 0)
	T_DB = (char_u *)"y";
    ਜੇ ((T_DA == ਨਿਰਾ || T_DA == ਖਾਲੀ_ਥਾਵਾਂ_option) && tgetflag("da") > 0)
	T_DA = (char_u *)"y";
    ਜੇ ((T_UT == ਨਿਰਾ || T_UT == ਖਾਲੀ_ਥਾਵਾਂ_option) && tgetflag("ut") > 0)
	T_UT = (char_u *)"y";
    ਜੇ ((T_XON == ਨਿਰਾ || T_XON == ਖਾਲੀ_ਥਾਵਾਂ_option) && tgetflag("xo") > 0)
	T_XON = (char_u *)"y";

    /*
     * get key codes
     */
    ਲਈ (i = 0; ਕੁੰਜੀ_ਨਾਂ[i] != ਨਿਰਾ; ++i)
	ਜੇ (ਲੱਭੋ_termcode((char_u *)ਕੁੰਜੀ_ਨਾਂ[i]) == ਨਿਰਾ)
	{
	    char_u *p = TGETSTR(ਕੁੰਜੀ_ਨਾਂ[i], &tp);

	    // if cursor-left == backspace, ignore it (televideo 925)
	    ਜੇ (p != ਨਿਰਾ
		    && (*p != Ctrl_H
			|| ਕੁੰਜੀ_ਨਾਂ[i][0] != 'k'
			|| ਕੁੰਜੀ_ਨਾਂ[i][1] != 'l'))
		ਜੋੜੋ_termcode((char_u *)ਕੁੰਜੀ_ਨਾਂ[i], p, FALSE);
	}

    ਜੇ (*ਉਚਾਈ == 0)
	*ਉਚਾਈ = tgetnum("li");
    ਜੇ (*ਚੌੜਾਈ == 0)
	*ਚੌੜਾਈ = tgetnum("co");

    /*
     * Get number of colors (if not done already).
     */
    ਜੇ (ਪਦ_ਲੜੀਆਂ_not_ਸੈੱਟ(KS_CCO))
    {
	ਸੈੱਟ_color_ਗਿਣਤੀ(tgetnum("Co"));
# ifdef FEAT_EVAL
	ਸੈੱਟ_ਪਦ_option_sctx_idx("Co", -1);
# endif
    }

# ifndef hpux
    BC = (ਅੱਖਰ *)TGETSTR("bc", &tp);
    ਉੱਪਰ_2 = (ਅੱਖਰ *)TGETSTR("up", &tp);
    char_u *p = TGETSTR("pc", &tp);
    ਜੇ (p != ਨਿਰਾ)
	PC = *p;
# endif
}
#ਜੇ_ਅੰਤ

/*
 * Report "term" is not found and list the ones we do know about.
 */
    ਸਥਿਰ ਖਾਲੀ
report_ਪਦ_error(ਅੱਖਰ *error_ਸੁਨੇਹਾ, char_u *ਪਦ)
{
    mch_errmsg("\r\n");
    ਜੇ (error_ਸੁਨੇਹਾ != ਨਿਰਾ)
    {
	mch_errmsg(error_ਸੁਨੇਹਾ);
	mch_errmsg("\r\n");
    }
    mch_errmsg("'");
    mch_errmsg((ਅੱਖਰ *)ਪਦ);
    mch_errmsg(_("' not known. Available builtin terminals are:"));
    mch_errmsg("\r\n");

    ਲਈ (ਪੂਰਨ i = 0; ; ++i)
    {
	ਅੱਖਰ *name = builtin_terminals[i].bitc_name;
	ਜੇ (name == ਨਿਰਾ)  // end marker
	    ਰੋਕੋ;
	// Do not mention the "gui" entry, the user won't need to type it.
	ਜੇ (STRCMP(name, "gui") != 0)
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_TGETENT
	    mch_errmsg("    builtin_");
#ਨਹੀਂਤਾਂ
	    mch_errmsg("    ");
#ਜੇ_ਅੰਤ
	    mch_errmsg(name);
	    mch_errmsg("\r\n");
	}
    }
    // Output extra 'cmdheight' line breaks to avoid that the following error
    // message overwrites the last terminal name.
    ਲਈ (ਪੂਰਨ i = 1; i < p_ch; ++i)
	mch_errmsg("\r\n");
}

    ਸਥਿਰ ਖਾਲੀ
report_default_ਪਦ(char_u *ਪਦ)
{
    mch_errmsg(_("defaulting to '"));
    mch_errmsg((ਅੱਖਰ *)ਪਦ);
    mch_errmsg("'\r\n");
    ਜੇ (emsg_silent == 0 && !ਵਿੱਚ_assert_fails)
    {
	screen_ਸ਼ੁਰੂ();	// don't know where cursor is now
	ਬਾਹਰਲਾ_flush();
	ਜੇ (!is_not_a_ਪਦ())
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
ਮੇਲ_keyprotocol(char_u *ਪਦ)
{
    ਪੂਰਨ		len = (ਪੂਰਨ)STRLEN(p_kpc) + 1;
    char_u	*buf = alloc(len);
    ਜੇ (buf == ਨਿਰਾ)
	ਵਾਪਸ KEYPROTOCOL_FAIL;

    keyprot_T	ret = KEYPROTOCOL_FAIL;
    char_u	*p = p_kpc;
    ਜਦੋਂ (*p != NUL)
    {
	// Isolate one comma separated item.
	(ਖਾਲੀ)copy_option_part(&p, buf, len, ",");
	char_u *colon = vim_strchr(buf, ':');
	ਜੇ (colon == ਨਿਰਾ || colon == buf || colon[1] == NUL)
	    ਜਾਓ theend;
	*colon = NUL;

	keyprot_T prot;
	// Note: Keep this in sync with p_kpc_protocol_values.
	ਜੇ (STRCMP(colon + 1, "none") == 0)
	    prot = KEYPROTOCOL_NONE;
	ਨਹੀਂਤਾਂ ਜੇ (STRCMP(colon + 1, "mok2") == 0)
	    prot = KEYPROTOCOL_MOK2;
	ਨਹੀਂਤਾਂ ਜੇ (STRCMP(colon + 1, "kitty") == 0)
	    prot = KEYPROTOCOL_KITTY;
	ਨਹੀਂਤਾਂ
	    ਜਾਓ theend;

	regmatch_T regmatch;
	CLEAR_ਖੇਤਰ(regmatch);
	regmatch.rm_ic = TRUE;
	regmatch.regprog = vim_regcomp(buf, RE_MAGIC);
	ਜੇ (regmatch.regprog == ਨਿਰਾ)
	    ਜਾਓ theend;

	ਪੂਰਨ ਮੇਲ = ਪਦ != ਨਿਰਾ && vim_regexec(&regmatch, ਪਦ, (colnr_T)0);
	vim_regfree(regmatch.regprog);
	ਜੇ (ਮੇਲ)
	{
	    ret = prot;
	    ਜਾਓ theend;
	}

    }

    // No match found, use "none".
    ret = KEYPROTOCOL_NONE;

theend:
    vim_free(buf);
    ਵਾਪਸ ret;
}

/*
 * Set terminal options for terminal "term".
 * Return OK if terminal 'term' was found in a termcap, FAIL otherwise.
 *
 * While doing this, until ttest(), some options may be NULL, be careful.
 */
    ਪੂਰਨ
ਸੈੱਟ_termname(char_u *ਪਦ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_TGETENT
    ਪੂਰਨ		builtin_ਪਹਿਲਾ = p_tbi;
    ਪੂਰਨ		try;
    ਪੂਰਨ		termcap_cleared = FALSE;
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		ਚੌੜਾਈ = 0, ਉਚਾਈ = 0;
    ਅੱਖਰ	*error_ਸੁਨੇਹਾ = ਨਿਰਾ;
    char_u	*bs_p, *del_p;

    // In silect mode (ex -s) we don't use the 'term' option.
    ਜੇ (silent_ਬਹੁਲਕ)
	ਵਾਪਸ OK;

    detected_8bit = FALSE;		// reset 8-bit detection

    ਜੇ (ਪਦ_is_builtin(ਪਦ))
    {
	ਪਦ += 8;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_TGETENT
	builtin_ਪਹਿਲਾ = 1;
#ਜੇ_ਅੰਤ
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
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_TGETENT
    ਲਈ (try = builtin_ਪਹਿਲਾ ? 0 : 1; try < 3; ++try)
    {
	/*
	 * Use external termcap
	 */
	ਜੇ (try == 1)
	{
	    char_u	    tbuf[TBUFSZ];

	    /*
	     * If the external termcap does not have a matching entry, try the
	     * builtin ones.
	     */
	    ਜੇ ((error_ਸੁਨੇਹਾ = invoke_tgetent(tbuf, ਪਦ)) == ਨਿਰਾ)
	    {
		ਜੇ (!termcap_cleared)
		{
		    clear_termoptions();	// clear old options
		    termcap_cleared = TRUE;
		}

		get_ਪਦ_entries(&ਉਚਾਈ, &ਚੌੜਾਈ);
	    }
	}
	ਨਹੀਂਤਾਂ	    // try == 0 || try == 2
#ਜੇ_ਅੰਤ // HAVE_TGETENT
	/*
	 * Use builtin termcap
	 */
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_TGETENT
	    /*
	     * If builtin termcap was already used, there is no need to search
	     * for the builtin termcap again, quit now.
	     */
	    ਜੇ (try == 2 && builtin_ਪਹਿਲਾ && termcap_cleared)
		ਰੋਕੋ;
#ਜੇ_ਅੰਤ
	    /*
	     * Search for 'term' in builtin_terminals[].
	     */
	    tcap_ਇੰਦਰਾਜ_T *termp = ਲੱਭੋ_builtin_ਪਦ(ਪਦ);
	    ਜੇ (termp == ਨਿਰਾ)	// did not find it
	    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_TGETENT
		/*
		 * If try == 0, first try the external termcap. If that is not
		 * found we'll get back here with try == 2.
		 * If termcap_cleared is set we used the external termcap,
		 * don't complain about not finding the term in the builtin
		 * termcap.
		 */
		ਜੇ (try == 0)			// try external one
		    ਜਾਰੀ;
		ਜੇ (termcap_cleared)		// found in external termcap
		    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
		report_ਪਦ_error(error_ਸੁਨੇਹਾ, ਪਦ);

		// when user typed :set term=xxx, quit here
		ਜੇ (starting != NO_SCREEN)
		{
		    screen_ਸ਼ੁਰੂ();	// don't know where cursor is now
		    wait_return(TRUE);
		    ਵਾਪਸ FAIL;
		}
		ਪਦ = DEFAULT_ਪਦ;
		report_default_ਪਦ(ਪਦ);
		ਸੈੱਟ_string_option_ਸਿੱਧਾ((char_u *)"term", -1, ਪਦ,
								 OPT_FREE, 0);
		display_errors();
	    }
	    ਬਾਹਰਲਾ_flush();
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_TGETENT
	    ਜੇ (!termcap_cleared)
	    {
#ਜੇ_ਅੰਤ
		clear_termoptions();	    // clear old options
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_TGETENT
		termcap_cleared = TRUE;
	    }
#ਜੇ_ਅੰਤ
	    parse_builtin_tcap(ਪਦ);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
	    ਜੇ (ਪਦ_is_gui(ਪਦ))
	    {
		ਬਾਹਰਲਾ_flush();
		gui_init();
		// If starting the GUI failed, don't do any of the other
		// things for this terminal
		ਜੇ (!gui.ਵਿੱਚ_use)
		    ਵਾਪਸ FAIL;
# ifdef HAVE_TGETENT
		ਰੋਕੋ;		// don't try using external termcap
# endif
	    }
#ਜੇ_ਅੰਤ // FEAT_GUI
	}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_TGETENT
    }
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    ਜੇ (!gui.ਵਿੱਚ_use)
#ਜੇ_ਅੰਤ
    {
	// Use the 'keyprotocol' option to adjust the t_TE and t_TI
	// termcap entries if there is an entry matching "term".
	keyprot_T kpc = ਮੇਲ_keyprotocol(ਪਦ);
	apply_keyprotocol(ਪਦ, kpc);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMGUICOLORS
	// There is no good way to detect that the terminal supports RGB
	// colors.  Since these termcap entries are non-standard anyway and
	// only used when the user sets 'termguicolors' we might as well add
	// them.  But not when one of them was already set.
	ਜੇ (ਪਦ_ਲੜੀਆਂ_not_ਸੈੱਟ(KS_8F)
		&& ਪਦ_ਲੜੀਆਂ_not_ਸੈੱਟ(KS_8B)
		&& ਪਦ_ਲੜੀਆਂ_not_ਸੈੱਟ(KS_8U))
	    apply_builtin_tcap(ਪਦ, builtin_rgb, TRUE);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_TGETENT
	ਜੇ (ਪਦ_ਲੜੀਆਂ_not_ਸੈੱਟ(KS_CF))
	    apply_builtin_tcap(ਪਦ, special_ਪਦ, TRUE);
	ਜੇ (ਪਦ_ਲੜੀਆਂ_not_ਸੈੱਟ(KS_BSU) && ਪਦ_ਲੜੀਆਂ_not_ਸੈੱਟ(KS_ESU))
	    apply_builtin_tcap(ਪਦ, builtin_sync_output, TRUE);
#ਜੇ_ਅੰਤ
    }

/*
 * special: There is no info in the termcap about whether the cursor
 * positioning is relative to the start of the screen or to the start of the
 * scrolling region.  We just guess here. Only msdos pcterm is known to do it
 * relative.
 */
    ਜੇ (STRCMP(ਪਦ, "pcterm") == 0)
	T_CCS = (char_u *)"yes";
    ਨਹੀਂਤਾਂ
	T_CCS = ਖਾਲੀ_ਥਾਵਾਂ_option;

    // Special case: "kitty" may not have a "RV" entry in terminfo, but we need
    // to request the version for several other things to work.
    ਜੇ (ਲੜੀ_ਖੋਜ((ਅੱਖਰ *)ਪਦ, "kitty") != ਨਿਰਾ
					   && (T_CRV == ਨਿਰਾ || *T_CRV == NUL))
	T_CRV = (char_u *)"\033[>c";

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
/*
 * Any "stty" settings override the default for t_kb from the termcap.
 * This is in os_unix.c, because it depends a lot on the version of unix that
 * is being used.
 * Don't do this when the GUI is active, it uses "t_kb" and "t_kD" directly.
 */
# ifdef FEAT_GUI
    ਜੇ (!gui.ਵਿੱਚ_use)
# endif
	get_stty();
#ਜੇ_ਅੰਤ

/*
 * If the termcap has no entry for 'bs' and/or 'del' and the ioctl() also
 * didn't work, use the default CTRL-H
 * The default for t_kD is DEL, unless t_kb is DEL.
 * The vim_strsave'd strings are probably lost forever, well it's only two
 * bytes.  Don't do this when the GUI is active, it uses "t_kb" and "t_kD"
 * directly.
 */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    ਜੇ (!gui.ਵਿੱਚ_use)
#ਜੇ_ਅੰਤ
    {
	bs_p = ਲੱਭੋ_termcode((char_u *)"kb");
	del_p = ਲੱਭੋ_termcode((char_u *)"kD");
	ਜੇ (bs_p == ਨਿਰਾ || *bs_p == NUL)
	    ਜੋੜੋ_termcode((char_u *)"kb", (bs_p = (char_u *)CTRL_H_STR), FALSE);
	ਜੇ ((del_p == ਨਿਰਾ || *del_p == NUL) &&
					    (bs_p == ਨਿਰਾ || *bs_p != DEL))
	    ਜੋੜੋ_termcode((char_u *)"kD", (char_u *)DEL_STR, FALSE);
    }

#ਜੇ defined(UNIX) || defined(VMS)
    ਪਦ_is_xterm = vim_is_xterm(ਪਦ);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMRESPONSE
    // Reset terminal properties that are set based on the termresponse, which
    // will be sent out soon.
    init_ਪਦ_props(FALSE);
#ਜੇ_ਅੰਤ

#ਜੇ defined(UNIX) || defined(VMS)
    // If the first number in t_XM is 1006 then the terminal will support SGR
    // mouse reporting.
    ਪੂਰਨ did_ਸੈੱਟ_ttym = FALSE;
    ਜੇ (T_CXM != ਨਿਰਾ && *T_CXM != NUL && !option_was_ਸੈੱਟ((char_u *)"ttym"))
    {
	char_u *p = T_CXM;

	ਜਦੋਂ (*p != NUL && !VIM_ISDIGIT(*p))
	    ++p;
	ਜੇ (getdigits(&p) == 1006)
	{
	    did_ਸੈੱਟ_ttym = TRUE;
	    ਸੈੱਟ_option_ਮੁੱਲ_give_err((char_u *)"ttym", 0L, (char_u *)"sgr", 0);
	}
    }

    /*
     * For Unix, set the 'ttymouse' option to the type of mouse to be used.
     * The termcode for the mouse is added as a side effect in option.c.
     */
    {
	char_u	*p = (char_u *)"";

# ifdef FEAT_MOUSE_XTERM
	ਜੇ (use_xterm_like_mouse(ਪਦ))
	{
	    ਜੇ (use_xterm_mouse())
		p = ਨਿਰਾ;	// keep existing value, might be "xterm2"
	    ਨਹੀਂਤਾਂ
		p = (char_u *)"xterm";
	}
# endif
	ਜੇ (p != ਨਿਰਾ && !did_ਸੈੱਟ_ttym)
	{
	    ਸੈੱਟ_option_ਮੁੱਲ_give_err((char_u *)"ttym", 0L, p, 0);
	    // Reset the WAS_SET flag, 'ttymouse' can be set to "sgr" or
	    // "xterm2" in check_termcode().
	    reset_option_was_ਸੈੱਟ((char_u *)"ttym");
	}
	ਜੇ (p == ਨਿਰਾ
# ifdef FEAT_GUI
		|| gui.ਵਿੱਚ_use
# endif
		)
	    check_mouse_termcode();	// set mouse termcode anyway
    }
#ਨਹੀਂਤਾਂ
    ਸੈੱਟ_mouse_termcode(KS_MOUSE, (char_u *)"\233M");
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MOUSE_XTERM
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
	ਜੋੜੋ_termcode(name, (char_u *)"\033[I", FALSE);

	// handle focus out event
	name[1] = KE_FOCUSLOST;
	ਜੋੜੋ_termcode(name, (char_u *)"\033[O", FALSE);

	need_gather = TRUE;
    }
#ਜੇ_ਅੰਤ
#ਜੇ (defined(UNIX) || defined(VMS))
    // First time after setting 'term' a focus event is always reported.
    focus_state = MAYBE;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ਪਦ_CONSOLE
    // DEFAULT_TERM indicates that it is the machine console.
    ਜੇ (STRCMP(ਪਦ, DEFAULT_ਪਦ) != 0)
	ਪਦ_console = FALSE;
    ਨਹੀਂਤਾਂ
    {
	ਪਦ_console = TRUE;
# ifdef AMIGA
	win_resize_on();	// enable window resizing reports
# endif
    }
#ਜੇ_ਅੰਤ

    ttest(TRUE);	// make sure we have a valid set of terminal codes

    ਭਰਿਆ_screen = TRUE;		// we can use termcap codes from now on
    ਸੈੱਟ_ਪਦ_defaults();	// use current values as defaults
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMRESPONSE
    LOG_TR1("setting crv_status to STATUS_GET");
    crv_ਸਥਿਤੀ.tr_ਤਰੱਕੀ = ਸਥਿਤੀ_GET;	// Get terminal version later
    write_t_8u_state = FALSE;
#ਜੇ_ਅੰਤ

    /*
     * Initialize the terminal with the appropriate termcap codes.
     * Set the mouse and window title if possible.
     * Don't do this when starting, need to parse the .vimrc first, because it
     * may redefine t_TI etc.
     */
    ਜੇ (starting != NO_SCREEN)
    {
	starttermcap();		// may change terminal mode
	setmouse();		// may start using the mouse
	maketitle();		// may display window title
    }

	// display initial screen after ttest() checking. jw.
    ਜੇ (ਚੌੜਾਈ <= 0 || ਉਚਾਈ <= 0)
    {
	// termcap failed to report size
	// set defaults, in case ui_get_shellsize() also fails
	ਚੌੜਾਈ = 80;
#ਜੇ defined(MSWIN)
	ਉਚਾਈ = 25;	    // console is often 25 lines
#ਨਹੀਂਤਾਂ
	ਉਚਾਈ = 24;	    // most terminals are 24 lines
#ਜੇ_ਅੰਤ
    }
    ਸੈੱਟ_shellsize(ਚੌੜਾਈ, ਉਚਾਈ, FALSE);	// may change Rows
    ਜੇ (starting != NO_SCREEN)
    {
	ਜੇ (scroll_region)
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
		ਜੇ (curbuf->b_ml.ml_mfp != ਨਿਰਾ)
		{
		    aucmd_prepbuf(&aco, buf);
		    ਜੇ (curbuf == buf)
		    {
			apply_autocmds(EVENT_TERMCHANGED, ਨਿਰਾ, ਨਿਰਾ, FALSE,
								      curbuf);
			// restore curwin/curbuf and a few other things
			aucmd_restbuf(&aco);
		    }
		}
	    }
	}
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMRESPONSE
    may_req_termresponse();
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_IMAGE
    (ਖਾਲੀ)update_image_backend();
#ਜੇ_ਅੰਤ

    ਵਾਪਸ OK;
}

#ਜੇ defined(EXITFREE)

# ifdef HAVE_DEL_CURTERM
#  include <term.h>	    // declares cur_term
# endif

/*
 * If supported, delete "cur_term", which caches terminal related entries.
 * Avoids that valgrind reports possibly lost memory.
 */
    ਖਾਲੀ
free_cur_ਪਦ(ਖਾਲੀ)
{
# ifdef HAVE_DEL_CURTERM
    ਜੇ (cur_ਪਦ)
	del_curterm(cur_ਪਦ);
# endif
}

#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_TGETENT
/*
 * Call tgetent()
 * Return error message if it fails, NULL if it's OK.
 */
    ਸਥਿਰ ਅੱਖਰ *
invoke_tgetent(char_u *tbuf, char_u *ਪਦ)
{
    ਪੂਰਨ	    i;

    // Note: Valgrind may report a leak here, because the library keeps one
    // buffer around that we can't ever free.
    i = TGETENT(tbuf, ਪਦ);
    ਜੇ (i < 0		    // -1 is always an error
# ifdef TGETENT_ZERO_ERR
	    || i == 0	    // sometimes zero is also an error
# endif
       )
    {
	// On FreeBSD tputs() gets a SEGV after a tgetent() which fails.  Call
	// tgetent() with the always existing "dumb" entry to avoid a crash or
	// hang.
	(ਖਾਲੀ)TGETENT(tbuf, "dumb");

	ਜੇ (i < 0)
# ifdef TGETENT_ZERO_ERR
	    ਵਾਪਸ _(e_cannot_open_termcap_file);
	ਜੇ (i == 0)
# endif
# ifdef TERMINFO
	    ਵਾਪਸ _(e_terminal_ਇੰਦਰਾਜ_not_ਮਿਲਿਆ_ਵਿੱਚ_terminfo);
# ਨਹੀਂਤਾਂ
	    ਵਾਪਸ _(e_terminal_ਇੰਦਰਾਜ_not_ਮਿਲਿਆ_ਵਿੱਚ_termcap);
# endif
    }
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Some versions of tgetstr() have been reported to return -1 instead of NULL.
 * Fix that here.
 */
    ਸਥਿਰ char_u *
vim_tgetstr(ਅੱਖਰ *s, char_u **pp)
{
    ਅੱਖਰ	*p;

    p = tgetstr(s, (ਅੱਖਰ **)pp);
    ਜੇ (p == (ਅੱਖਰ *)-1)
	p = ਨਿਰਾ;
    ਵਾਪਸ (char_u *)p;
}
#ਜੇ_ਅੰਤ // HAVE_TGETENT

#ਜੇ defined(HAVE_TGETENT) && (defined(UNIX) || defined(VMS) || defined(MACOS_X))
/*
 * Get Columns and Rows from the termcap. Used after a window signal if the
 * ioctl() fails. It doesn't make sense to call tgetent each time if the "co"
 * and "li" entries never change. But on some systems this works.
 * Errors while getting the entries are ignored.
 */
    ਖਾਲੀ
getlinecol(
    ਲੰਮਾ	*cp,	// pointer to columns
    ਲੰਮਾ	*rp)	// pointer to rows
{
    char_u	tbuf[TBUFSZ];

    ਜੇ (T_NAME == ਨਿਰਾ || *T_NAME == NUL
	    || invoke_tgetent(tbuf, T_NAME) != ਨਿਰਾ)
	ਵਾਪਸ;

    ਜੇ (*cp == 0)
	*cp = tgetnum("co");
    ਜੇ (*rp == 0)
	*rp = tgetnum("li");
}
#ਜੇ_ਅੰਤ // defined(HAVE_TGETENT) && defined(UNIX)

/*
 * Get a string entry from the termcap and add it to the list of termcodes.
 * Used for <t_xx> special keys.
 * Give an error message for failure when not sourcing.
 * If force given, replace an existing entry.
 * Return FAIL if the entry was not found, OK if the entry was added.
 */
    ਪੂਰਨ
ਜੋੜੋ_termcap_ਇੰਦਰਾਜ(char_u *name, ਪੂਰਨ force)
{
    char_u  *ਪਦ;
    ਪੂਰਨ	    ਕੁੰਜੀ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_TGETENT
    char_u  *string;
    ਪੂਰਨ	    i;
    ਪੂਰਨ	    builtin_ਪਹਿਲਾ;
    char_u  tbuf[TBUFSZ];
    char_u  tstrbuf[TBUFSZ];
    char_u  *tp = tstrbuf;
    ਅੱਖਰ    *error_ਸੁਨੇਹਾ = ਨਿਰਾ;
#ਜੇ_ਅੰਤ

/*
 * If the GUI is running or will start in a moment, we only support the keys
 * that the GUI can produce.
 */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    ਜੇ (gui.ਵਿੱਚ_use || gui.starting)
	ਵਾਪਸ gui_mch_haskey(name);
#ਜੇ_ਅੰਤ

    ਜੇ (!force && ਲੱਭੋ_termcode(name) != ਨਿਰਾ)	    // it's already there
	ਵਾਪਸ OK;

    ਪਦ = T_NAME;
    ਜੇ (ਪਦ == ਨਿਰਾ || *ਪਦ == NUL)	    // 'term' not defined yet
	ਵਾਪਸ FAIL;

    ਜੇ (ਪਦ_is_builtin(ਪਦ))		    // name starts with "builtin_"
    {
	ਪਦ += 8;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_TGETENT
	builtin_ਪਹਿਲਾ = TRUE;
#ਜੇ_ਅੰਤ
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_TGETENT
    ਨਹੀਂਤਾਂ
	builtin_ਪਹਿਲਾ = p_tbi;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_TGETENT
/*
 * We can get the entry from the builtin termcap and from the external one.
 * If 'ttybuiltin' is on or the terminal name starts with "builtin_", try
 * builtin termcap first.
 * If 'ttybuiltin' is off, try external termcap first.
 */
    ਲਈ (i = 0; i < 2; ++i)
    {
	ਜੇ ((!builtin_ਪਹਿਲਾ) == i)
#ਜੇ_ਅੰਤ
	/*
	 * Search in builtin termcaps
	 */
	{
	    tcap_ਇੰਦਰਾਜ_T *termp = ਲੱਭੋ_builtin_ਪਦ(ਪਦ);
	    ਜੇ (termp != ਨਿਰਾ)	// found it
	    {
		ਕੁੰਜੀ = TERMCAP2_ਕੁੰਜੀ(name[0], name[1]);
		++termp;
		ਜਦੋਂ (termp->bt_ਇੰਦਰਾਜ != (ਪੂਰਨ)KS_NAME)
		{
		    ਜੇ ((ਪੂਰਨ)termp->bt_ਇੰਦਰਾਜ == ਕੁੰਜੀ)
		    {
			ਜੋੜੋ_termcode(name, (char_u *)termp->bt_string,
							  ਪਦ_is_8bit(ਪਦ));
			ਵਾਪਸ OK;
		    }
		    ++termp;
		}
	    }
	}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_TGETENT
	ਨਹੀਂਤਾਂ
	/*
	 * Search in external termcap
	 */
	{
	    error_ਸੁਨੇਹਾ = invoke_tgetent(tbuf, ਪਦ);
	    ਜੇ (error_ਸੁਨੇਹਾ == ਨਿਰਾ)
	    {
		string = TGETSTR((ਅੱਖਰ *)name, &tp);
		ਜੇ (string != ਨਿਰਾ && *string != NUL)
		{
		    ਜੋੜੋ_termcode(name, string, FALSE);
		    ਵਾਪਸ OK;
		}
	    }
	}
    }
#ਜੇ_ਅੰਤ

    ਜੇ (SOURCING_NAME == ਨਿਰਾ)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_TGETENT
	ਜੇ (error_ਸੁਨੇਹਾ != ਨਿਰਾ)
	    emsg(error_ਸੁਨੇਹਾ);
	ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	    semsg(_(e_no_str_ਇੰਦਰਾਜ_ਵਿੱਚ_termcap), name);
    }
    ਵਾਪਸ FAIL;
}

    ਸਥਿਰ ਪੂਰਨ
ਪਦ_is_builtin(char_u *name)
{
    ਵਾਪਸ (STRNCMP(name, "builtin_", (size_t)8) == 0);
}

/*
 * Return TRUE if terminal "name" uses CSI instead of <Esc>[.
 * Assume that the terminal is using 8-bit controls when the name contains
 * "8bit", like in "xterm-8bit".
 */
    ਪੂਰਨ
ਪਦ_is_8bit(char_u *name)
{
    ਵਾਪਸ (detected_8bit || ਲੜੀ_ਖੋਜ((ਅੱਖਰ *)name, "8bit") != ਨਿਰਾ);
}

/*
 * Translate terminal control chars from 7-bit to 8-bit:
 * <Esc>[ -> CSI  <M_C_[>
 * <Esc>] -> OSC  <M-C-]>
 * <Esc>O -> <M-C-O>
 */
    ਸਥਿਰ ਪੂਰਨ
ਪਦ_7to8bit(char_u *p)
{
    ਜੇ (*p != ESC)
	ਵਾਪਸ 0;

    ਜੇ (p[1] == '[')
	ਵਾਪਸ CSI;
    ਨਹੀਂਤਾਂ ਜੇ (p[1] == ']')
	ਵਾਪਸ OSC;
    ਨਹੀਂਤਾਂ ਜੇ (p[1] == 'O')
	ਵਾਪਸ 0x8f;
    ਵਾਪਸ 0;
}

#ਜੇ defined(FEAT_GUI)
    ਪੂਰਨ
ਪਦ_is_gui(char_u *name)
{
    ਵਾਪਸ (STRCMP(name, "builtin_gui") == 0 || STRCMP(name, "gui") == 0);
}
#ਜੇ_ਅੰਤ

#ਜੇ !defined(HAVE_TGETENT) || defined(AMIGA)

    char_u *
tltoa(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ i)
{
    ਸਥਿਰ char_u buf[16];
    char_u	*p;

    p = buf + 15;
    *p = '\0';
    ਕਰੋ
    {
	--p;
	*p = (char_u) (i % 10 + '0');
	i /= 10;
    }
    ਜਦੋਂ (i > 0 && p > buf);
    ਵਾਪਸ p;
}
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ HAVE_TGETENT

/*
 * minimal tgoto() implementation.
 * no padding and we only parse for %i %d and %+char
 */
    ਸਥਿਰ ਅੱਖਰ *
tgoto(ਅੱਖਰ *cm, ਪੂਰਨ x, ਪੂਰਨ y)
{
    ਸਥਿਰ ਅੱਖਰ buf[30];
    ਅੱਖਰ *p, *s, *e;

    ਜੇ (!cm)
	ਵਾਪਸ "OOPS";
    e = buf + 29;
    ਲਈ (s = buf; s < e && *cm; cm++)
    {
	ਜੇ (*cm != '%')
	{
	    *s++ = *cm;
	    ਜਾਰੀ;
	}
	ਚੋਣ (*++cm)
	{
	ਹਾਲਤ 'd':
	    p = (ਅੱਖਰ *)tltoa((ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)y);
	    y = x;
	    ਜਦੋਂ (*p)
		*s++ = *p++;
	    ਰੋਕੋ;
	ਹਾਲਤ 'i':
	    x++;
	    y++;
	    ਰੋਕੋ;
	ਹਾਲਤ '+':
	    *s++ = (ਅੱਖਰ)(*++cm + y);
	    y = x;
	    ਰੋਕੋ;
	ਹਾਲਤ '%':
	    *s++ = *cm;
	    ਰੋਕੋ;
	ਮੂਲ:
	    ਵਾਪਸ "OOPS";
	}
    }
    *s = '\0';
    ਵਾਪਸ buf;
}

#ਜੇ_ਅੰਤ // HAVE_TGETENT

/*
 * Set the terminal name and initialize the terminal options.
 * If "name" is NULL or empty, get the terminal name from the environment.
 * If that fails, use the default terminal name.
 */
    ਖਾਲੀ
termcapinit(char_u *name)
{
    char_u	*ਪਦ = name;

    ਜੇ (ਪਦ != ਨਿਰਾ && *ਪਦ == NUL)
	ਪਦ = ਨਿਰਾ;	    // empty name is equal to no name

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ MSWIN
    ਜੇ (ਪਦ == ਨਿਰਾ)
	ਪਦ = mch_getenv((char_u *)"TERM");
#ਜੇ_ਅੰਤ
    ਜੇ (ਪਦ == ਨਿਰਾ || *ਪਦ == NUL)
	ਪਦ = DEFAULT_ਪਦ;
    ਸੈੱਟ_string_option_ਸਿੱਧਾ((char_u *)"term", -1, ਪਦ, OPT_FREE, 0);

    // Set the default terminal name.
    ਸੈੱਟ_string_default("term", ਪਦ);
    ਸੈੱਟ_string_default("ttytype", ਪਦ);

    // Avoid using "term" here, because the next mch_getenv() may overwrite it.
    ਸੈੱਟ_termname(T_NAME != ਨਿਰਾ ? T_NAME : ਪਦ);
}

/*
 * The number of calls to ui_write is reduced by using "out_buf".
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਬਾਹਰਲਾ_ਮਾਪ	8191

// add one to allow mch_write() in os_win32.c to append a NUL
ਸਥਿਰ char_u		ਬਾਹਰਲਾ_buf[ਬਾਹਰਲਾ_ਮਾਪ + 1];

ਸਥਿਰ ਪੂਰਨ		ਬਾਹਰਲਾ_pos = 0;	// number of chars in out_buf

// Since the maximum number of SGR parameters shown as a normal value range is
// 16, the escape sequence length can be 4 * 16 + lead + tail.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_ESC_SEQ_LEN	80

/*
 * out_flush(): flush the output buffer
 */
    ਖਾਲੀ
ਬਾਹਰਲਾ_flush(ਖਾਲੀ)
{
    ਪੂਰਨ	    len;

    ਜੇ (ਬਾਹਰਲਾ_pos == 0)
	ਵਾਪਸ;

    // set out_pos to 0 before ui_write, to avoid recursiveness
    len = ਬਾਹਰਲਾ_pos;
    ਬਾਹਰਲਾ_pos = 0;
    ui_write(ਬਾਹਰਲਾ_buf, len, FALSE);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਜੇ (ch_log_output != FALSE)
    {
	ਬਾਹਰਲਾ_buf[len] = NUL;
	ch_log(ਨਿਰਾ, "raw %s output: \"%s\"",
# ifdef FEAT_GUI
		(gui.ਵਿੱਚ_use && !gui.dying && !gui.starting) ? "GUI" :
# endif
		"terminal",
		ਬਾਹਰਲਾ_buf);
	ਜੇ (ch_log_output == TRUE)
	    ch_log_output = FALSE;  // only log once
    }
#ਜੇ_ਅੰਤ
}

/*
 * out_flush_cursor(): flush the output buffer and redraw the cursor.
 * Does not flush recursively in the GUI to avoid slow drawing.
 */
    ਖਾਲੀ
ਬਾਹਰਲਾ_flush_cursor(
    ਪੂਰਨ	    force ਅਣਵਰਤਿਆ,   // when TRUE, update cursor even when not moved
    ਪੂਰਨ	    clear_selection ਅਣਵਰਤਿਆ) // clear selection under cursor
{
    mch_disable_flush();
    ਬਾਹਰਲਾ_flush();
    mch_enable_flush();
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    ਜੇ (gui.ਵਿੱਚ_use)
    {
	gui_update_cursor(force, clear_selection);
	gui_may_flush();
    }
#ਜੇ_ਅੰਤ
}


/*
 * Sometimes a byte out of a multi-byte character is written with out_char().
 * To avoid flushing half of the character, call this function first.
 */
    ਖਾਲੀ
ਬਾਹਰਲਾ_flush_check(ਖਾਲੀ)
{
    ਜੇ (enc_dbcs != 0 && ਬਾਹਰਲਾ_pos >= ਬਾਹਰਲਾ_ਮਾਪ - MB_MAXBYTES)
	ਬਾਹਰਲਾ_flush();
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
/*
 * out_trash(): Throw away the contents of the output buffer
 */
    ਖਾਲੀ
ਬਾਹਰਲਾ_trash(ਖਾਲੀ)
{
    ਬਾਹਰਲਾ_pos = 0;
}
#ਜੇ_ਅੰਤ

/*
 * out_char(c): put a byte into the output buffer.
 *		Flush it if it becomes full.
 * This should not be used for outputting text on the screen (use functions
 * like msg_puts() and screen_putchar() for that).
 */
    ਖਾਲੀ
ਬਾਹਰਲਾ_char(ਬਿਨਾਨਿਸ਼ਾਨੀ c)
{
#ਜੇ defined(UNIX) || defined(VMS) || defined(AMIGA) || defined(MACOS_X)
    ਜੇ (c == '\n')	// turn LF into CR-LF (CRMOD doesn't seem to do this)
	ਬਾਹਰਲਾ_char('\r');
#ਜੇ_ਅੰਤ

    ਬਾਹਰਲਾ_buf[ਬਾਹਰਲਾ_pos++] = c;

    // For testing we flush each time.
    ਜੇ (ਬਾਹਰਲਾ_pos >= ਬਾਹਰਲਾ_ਮਾਪ || p_wd)
	ਬਾਹਰਲਾ_flush();
}

/*
 * Output "c" like out_char(), but don't flush when p_wd is set.
 */
    ਸਥਿਰ ਪੂਰਨ
ਬਾਹਰਲਾ_char_nf(ਪੂਰਨ c)
{
    ਬਾਹਰਲਾ_buf[ਬਾਹਰਲਾ_pos++] = (ਬਿਨਾਨਿਸ਼ਾਨੀ)c;

    ਜੇ (ਬਾਹਰਲਾ_pos >= ਬਾਹਰਲਾ_ਮਾਪ)
	ਬਾਹਰਲਾ_flush();
    ਵਾਪਸ (ਬਿਨਾਨਿਸ਼ਾਨੀ)c;
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
    ਖਾਲੀ
ਬਾਹਰਲਾ_str_nf(char_u *s)
{
    // avoid terminal strings being split up
    ਜੇ (ਬਾਹਰਲਾ_pos > ਬਾਹਰਲਾ_ਮਾਪ - MAX_ESC_SEQ_LEN)
	ਬਾਹਰਲਾ_flush();

    ਲਈ (char_u *p = s; *p != NUL; ++p)
	ਬਾਹਰਲਾ_char_nf(*p);

    // For testing we write one string at a time.
    ਜੇ (p_wd)
	ਬਾਹਰਲਾ_flush();
}

/*
 * A conditional-flushing out_str, mainly for visualbell.
 * Handles a delay internally, because termlib may not respect the delay or do
 * it at the wrong time.
 * Note: Only for terminal strings.
 */
    ਖਾਲੀ
ਬਾਹਰਲਾ_str_cf(char_u *s)
{
    ਜੇ (s == ਨਿਰਾ || *s == NUL)
	ਵਾਪਸ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_TGETENT
    char_u *p;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    // Don't use tputs() when GUI is used, ncurses crashes.
    ਜੇ (gui.ਵਿੱਚ_use)
    {
	ਬਾਹਰਲਾ_str_nf(s);
	ਵਾਪਸ;
    }
#ਜੇ_ਅੰਤ
    ਜੇ (ਬਾਹਰਲਾ_pos > ਬਾਹਰਲਾ_ਮਾਪ - MAX_ESC_SEQ_LEN)
	ਬਾਹਰਲਾ_flush();
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_TGETENT
    ਲਈ (p = s; *s; ++s)
    {
	// flush just before delay command
	ਜੇ (*s == '$' && *(s + 1) == '<')
	{
	    char_u save_c = *s;
	    ਪੂਰਨ duration = atoi((ਅੱਖਰ *)s + 2);

	    *s = NUL;
	    tputs((ਅੱਖਰ *)p, 1, TPUTSFUNCAST ਬਾਹਰਲਾ_char_nf);
	    *s = save_c;
	    ਬਾਹਰਲਾ_flush();
# ifdef ELAPSED_FUNC
	    // Only sleep here if we can limit this happening in
	    // vim_beep().
	    p = vim_strchr(s, '>');
	    ਜੇ (p == ਨਿਰਾ || duration <= 0)
	    {
		// can't parse the time, don't sleep here
		p = s;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		++p;
		do_sleep(duration, FALSE);
	    }
# ਨਹੀਂਤਾਂ
	    // Rely on the terminal library to sleep.
	    p = s;
# endif
	    ਰੋਕੋ;
	}
    }
    tputs((ਅੱਖਰ *)p, 1, TPUTSFUNCAST ਬਾਹਰਲਾ_char_nf);
#ਨਹੀਂਤਾਂ
    ਜਦੋਂ (*s)
	ਬਾਹਰਲਾ_char_nf(*s++);
#ਜੇ_ਅੰਤ

    // For testing we write one string at a time.
    ਜੇ (p_wd)
	ਬਾਹਰਲਾ_flush();
}

/*
 * out_str(s): Put a character string a byte at a time into the output buffer.
 * If HAVE_TGETENT is defined use tputs(), the termcap parser. (jw)
 * This should only be used for writing terminal codes, not for outputting
 * normal text (use functions like msg_puts() and screen_putchar() for that).
 */
    ਖਾਲੀ
ਬਾਹਰਲਾ_str(char_u *s)
{
    ਜੇ (s == ਨਿਰਾ || *s == NUL)
	ਵਾਪਸ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    // Don't use tputs() when GUI is used, ncurses crashes.
    ਜੇ (gui.ਵਿੱਚ_use)
    {
	ਬਾਹਰਲਾ_str_nf(s);
	ਵਾਪਸ;
    }
#ਜੇ_ਅੰਤ
    // avoid terminal strings being split up
    ਜੇ (ਬਾਹਰਲਾ_pos > ਬਾਹਰਲਾ_ਮਾਪ - MAX_ESC_SEQ_LEN)
	ਬਾਹਰਲਾ_flush();
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_TGETENT
    tputs((ਅੱਖਰ *)s, 1, TPUTSFUNCAST ਬਾਹਰਲਾ_char_nf);
#ਨਹੀਂਤਾਂ
    ਜਦੋਂ (*s)
	ਬਾਹਰਲਾ_char_nf(*s++);
#ਜੇ_ਅੰਤ

    // For testing we write one string at a time.
    ਜੇ (p_wd)
	ਬਾਹਰਲਾ_flush();
}

/*
 * cursor positioning using termcap parser. (jw)
 */
    ਖਾਲੀ
ਪਦ_windgoto(ਪੂਰਨ ਪੰਕਤੀ, ਪੂਰਨ ਖਾਨਾ)
{
    ਬਾਹਰਲਾ_STR(tgoto((ਅੱਖਰ *)T_CM, ਖਾਨਾ, ਪੰਕਤੀ));
}

    ਖਾਲੀ
ਪਦ_cursor_ਸੱਜਾ(ਪੂਰਨ i)
{
    ਬਾਹਰਲਾ_STR(tgoto((ਅੱਖਰ *)T_CRI, 0, i));
}

    ਖਾਲੀ
ਪਦ_append_ਸਤਰਾਂ(ਪੂਰਨ ਸਤਰ_ਗਿਣਤੀ)
{
    ਬਾਹਰਲਾ_STR(tgoto((ਅੱਖਰ *)T_CAL, 0, ਸਤਰ_ਗਿਣਤੀ));
}

    ਖਾਲੀ
ਪਦ_delete_ਸਤਰਾਂ(ਪੂਰਨ ਸਤਰ_ਗਿਣਤੀ)
{
    ਬਾਹਰਲਾ_STR(tgoto((ਅੱਖਰ *)T_CDL, 0, ਸਤਰ_ਗਿਣਤੀ));
}

#ਜੇ defined(UNIX) || defined(VMS)
    ਖਾਲੀ
ਪਦ_enable_mouse(ਪੂਰਨ enable)
{
    ਪੂਰਨ on = enable ? 1 : 0;
    ਬਾਹਰਲਾ_STR(tgoto((ਅੱਖਰ *)T_CXM, 0, on));
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(HAVE_TGETENT)
    ਖਾਲੀ
ਪਦ_ਸੈੱਟ_winpos(ਪੂਰਨ x, ਪੂਰਨ y)
{
    // Can't handle a negative value here
    ਜੇ (x < 0)
	x = 0;
    ਜੇ (y < 0)
	y = 0;
    ਬਾਹਰਲਾ_STR(tgoto((ਅੱਖਰ *)T_CWP, y, x));
}

# ਜੇ defined(FEAT_TERMRESPONSE)
/*
 * Return TRUE if we can request the terminal for a response.
 */
    ਸਥਿਰ ਪੂਰਨ
can_get_termresponse(ਖਾਲੀ)
{
    ਵਾਪਸ cur_tmode == TMODE_RAW
	    && termcap_ਸਰਗਰਮ
#  ifdef UNIX
	    && (is_not_a_ਪਦ() || (isatty(1) && isatty(read_cmd_fd)))
#  endif
	    && p_ek;
}

/*
 * Set "status" to STATUS_SENT.
 */
    ਸਥਿਰ ਖਾਲੀ
termrequest_ਭੇਜਿਆ(termrequest_T *ਸਥਿਤੀ)
{
    ਸਥਿਤੀ->tr_ਤਰੱਕੀ = ਸਥਿਤੀ_ਭੇਜਿਆ;
    ਸਥਿਤੀ->tr_ਸ਼ੁਰੂ = ਸਮਾਂ(ਨਿਰਾ);
}

/*
 * Return TRUE if any of the requests are in STATUS_SENT.
 */
    ਸਥਿਰ ਪੂਰਨ
termrequest_any_pending(ਖਾਲੀ)
{
    ਪੂਰਨ	    i;
    time_t  now = ਸਮਾਂ(ਨਿਰਾ);

    ਲਈ (i = 0; all_termrequests[i] != ਨਿਰਾ; ++i)
    {
	ਜੇ (all_termrequests[i]->tr_ਤਰੱਕੀ == ਸਥਿਤੀ_ਭੇਜਿਆ)
	{
	    ਜੇ (all_termrequests[i]->tr_ਸ਼ੁਰੂ > 0 && now > 0
				    && all_termrequests[i]->tr_ਸ਼ੁਰੂ + 2 < now)
		// Sent the request more than 2 seconds ago and didn't get a
		// response, assume it failed.
		all_termrequests[i]->tr_ਤਰੱਕੀ = ਸਥਿਤੀ_FAIL;
	    ਨਹੀਂਤਾਂ
		ਵਾਪਸ TRUE;
	}
    }
    ਵਾਪਸ FALSE;
}

ਸਥਿਰ ਪੂਰਨ winpos_x = -1;
ਸਥਿਰ ਪੂਰਨ winpos_y = -1;
ਸਥਿਰ ਪੂਰਨ did_request_winpos = 0;

#  ਜੇ defined(FEAT_EVAL) || defined(FEAT_TERMINAL)
/*
 * Try getting the Vim window position from the terminal.
 * Returns OK or FAIL.
 */
    ਪੂਰਨ
ਪਦ_get_winpos(ਪੂਰਨ *x, ਪੂਰਨ *y, varnumber_T timeout)
{
    ਪੂਰਨ ਗਿਣਤੀ_2 = 0;
    ਪੂਰਨ prev_winpos_x = winpos_x;
    ਪੂਰਨ prev_winpos_y = winpos_y;

    ਜੇ (*T_CGP == NUL || !can_get_termresponse())
	ਵਾਪਸ FAIL;
    winpos_x = -1;
    winpos_y = -1;
    ++did_request_winpos;
    termrequest_ਭੇਜਿਆ(&winpos_ਸਥਿਤੀ);
    ਬਾਹਰਲਾ_STR(T_CGP);
    ਬਾਹਰਲਾ_flush();

    // Try reading the result for "timeout" msec.
    ਜਦੋਂ (ਗਿਣਤੀ_2++ <= timeout / 10 && !got_int)
    {
	(ਖਾਲੀ)vpeekc_nomap();
	ਜੇ (winpos_x >= 0 && winpos_y >= 0)
	{
	    *x = winpos_x;
	    *y = winpos_y;
	    ਵਾਪਸ OK;
	}
	ui_delay(10L, FALSE);
    }
    // Do not reset "did_request_winpos", if we timed out the response might
    // still come later and we must consume it.

    winpos_x = prev_winpos_x;
    winpos_y = prev_winpos_y;
    ਜੇ (timeout < 10 && prev_winpos_y >= 0 && prev_winpos_x >= 0)
    {
	// Polling: return previous values if we have them.
	*x = winpos_x;
	*y = winpos_y;
	ਵਾਪਸ OK;
    }

    ਵਾਪਸ FALSE;
}
#  endif
# endif

    ਖਾਲੀ
ਪਦ_ਸੈੱਟ_winsize(ਪੂਰਨ ਉਚਾਈ, ਪੂਰਨ ਚੌੜਾਈ)
{
    ਬਾਹਰਲਾ_STR(tgoto((ਅੱਖਰ *)T_CWS, ਚੌੜਾਈ, ਉਚਾਈ));
}
#ਜੇ_ਅੰਤ

    ਖਾਲੀ
ਪਦ_font(ਪੂਰਨ n)
{
    ਜੇ (*T_CFO)
    {
	ਅੱਖਰ buf[20];
	sprintf(buf, (ਅੱਖਰ *)T_CFO, 9 + n);
	ਬਾਹਰਲਾ_STR(buf);
    }
}

    ਸਥਿਰ ਖਾਲੀ
ਪਦ_color(char_u *s, ਪੂਰਨ n)
{
    ਅੱਖਰ	buf[20];
    ਪੂਰਨ		i = *s == CSI ? 1 : 2;
		// index in s[] just after <Esc>[ or CSI

    // Special handling of 16 colors, because termcap can't handle it
    // Also accept "\e[3%dm" for TERMINFO, it is sometimes used
    // Also accept CSI instead of <Esc>[
    ਜੇ (n >= 8 && t_colors >= 16
	      && ((s[0] == ESC && s[1] == '[')
#ਜੇ defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
		  || (s[0] == ESC && s[1] == '|')
#ਜੇ_ਅੰਤ
		  || (s[0] == CSI && (i = 1) == 1))
	      && s[i] != NUL
	      && (STRCMP(s + i + 1, "%p1%dm") == 0
		  || STRCMP(s + i + 1, "%dm") == 0)
	      && (s[i] == '3' || s[i] == '4'))
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TERMINFO
	ਅੱਖਰ *format = "%s%s%%p1%%dm";
#ਨਹੀਂਤਾਂ
	ਅੱਖਰ *format = "%s%s%%dm";
#ਜੇ_ਅੰਤ
	ਅੱਖਰ *lead = i == 2 ? (
#ਜੇ defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
		    s[1] == '|' ? "\033|" :
#ਜੇ_ਅੰਤ
		    "\033[") : "\233";
	ਅੱਖਰ *ਪੂਛ = s[i] == '3' ? (n >= 16 ? "38;5;" : "9")
				 : (n >= 16 ? "48;5;" : "10");

	sprintf(buf, format, lead, ਪੂਛ);
	ਬਾਹਰਲਾ_STR(tgoto(buf, 0, n >= 16 ? n : n - 8));
    }
    ਨਹੀਂਤਾਂ
	ਬਾਹਰਲਾ_STR(tgoto((ਅੱਖਰ *)s, 0, n));
}

    ਖਾਲੀ
ਪਦ_fg_color(ਪੂਰਨ n)
{
    // Use "AF" termcap entry if present, "Sf" entry otherwise
    ਜੇ (*T_CAF)
	ਪਦ_color(T_CAF, n);
    ਨਹੀਂਤਾਂ ਜੇ (*T_CSF)
	ਪਦ_color(T_CSF, n);
}

    ਖਾਲੀ
ਪਦ_bg_color(ਪੂਰਨ n)
{
    // Use "AB" termcap entry if present, "Sb" entry otherwise
    ਜੇ (*T_CAB)
	ਪਦ_color(T_CAB, n);
    ਨਹੀਂਤਾਂ ਜੇ (*T_CSB)
	ਪਦ_color(T_CSB, n);
}

    ਖਾਲੀ
ਪਦ_ul_color(ਪੂਰਨ n)
{
    ਜੇ (*T_CAU)
	ਪਦ_color(T_CAU, n);
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
ਪਦ_bg_default(ਖਾਲੀ)
{
#ਜੇ defined(MSWIN)
    // DOS console is nearly always black
    ਵਾਪਸ (char_u *)"dark";
#ਨਹੀਂਤਾਂ
    char_u	*p;

    ਜੇ (STRCMP(T_NAME, "linux") == 0
	    || STRCMP(T_NAME, "screen.linux") == 0
	    || STRNCMP(T_NAME, "cygwin", 6) == 0
	    || STRNCMP(T_NAME, "putty", 5) == 0
	    || ((p = mch_getenv((char_u *)"COLORFGBG")) != ਨਿਰਾ
		&& (p = vim_strrchr(p, ';')) != ਨਿਰਾ
		&& ((p[1] >= '0' && p[1] <= '6') || p[1] == '8')
		&& p[2] == NUL))
	ਵਾਪਸ (char_u *)"dark";
    ਵਾਪਸ (char_u *)"light";
#ਜੇ_ਅੰਤ
}

#ਜੇ defined(FEAT_TERMGUICOLORS)

# define RED(rgb)   (((long_u)(rgb) >> 16) & 0xFF)
# define GREEN(rgb) (((long_u)(rgb) >>  8) & 0xFF)
# define BLUE(rgb)  (((long_u)(rgb)      ) & 0xFF)

    ਸਥਿਰ ਖਾਲੀ
ਪਦ_rgb_color(char_u *s, guicolor_T rgb)
{
# define MAX_COLOR_STR_LEN 100
    ਅੱਖਰ	buf[MAX_COLOR_STR_LEN];

    ਜੇ (*s == NUL)
	ਵਾਪਸ;
    vim_snprintf(buf, MAX_COLOR_STR_LEN,
				  (ਅੱਖਰ *)s, RED(rgb), GREEN(rgb), BLUE(rgb));
# ifdef FEAT_VTP
    ਜੇ (use_vtp() && (p_tgc || t_colors >= 256))
    {
	ਬਾਹਰਲਾ_flush();
	buf[1] = '[';
	vtp_printf(buf);
    }
    ਨਹੀਂਤਾਂ
# endif
	ਬਾਹਰਲਾ_STR(buf);
}

    ਖਾਲੀ
ਪਦ_fg_rgb_color(guicolor_T rgb)
{
    ਜੇ (rgb != INVALCOLOR)
	ਪਦ_rgb_color(T_8F, rgb);
}

    ਖਾਲੀ
ਪਦ_bg_rgb_color(guicolor_T rgb)
{
    ਜੇ (rgb != INVALCOLOR)
	ਪਦ_rgb_color(T_8B, rgb);
}

    ਖਾਲੀ
ਪਦ_ul_rgb_color(guicolor_T rgb)
{
# ifdef FEAT_TERMRESPONSE
    // If the user explicitly sets t_8u then use it.  Otherwise wait for
    // termresponse to be received, which is when t_8u would be set and a
    // redraw is needed if it was used.
    ਜੇ (!option_was_ਸੈੱਟ((char_u *)"t_8u") && write_t_8u_state != OK)
	write_t_8u_state = MAYBE;
    ਨਹੀਂਤਾਂ
# endif
	ਪਦ_rgb_color(T_8U, rgb);
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(UNIX) || defined(VMS) || defined(MACOS_X)
/*
 * Generic function to set window title, using t_ts and t_fs.
 */
    ਖਾਲੀ
ਪਦ_settitle(char_u *title)
{
    MAY_WANT_ਮੰਜ਼ਿਲ_LOG_THIS;

    // t_ts takes one argument: column in status line
    ਬਾਹਰਲਾ_STR(tgoto((ਅੱਖਰ *)T_TS, 0, 0));	// set title start
    ਬਾਹਰਲਾ_str_nf(title);
    ਬਾਹਰਲਾ_str(T_FS);			// set title end
    ਬਾਹਰਲਾ_flush();
}

/*
 * Tell the terminal to push (save) the title and/or icon, so that it can be
 * popped (restored) later.
 */
    ਖਾਲੀ
ਪਦ_ਧੱਕੋ_title(ਪੂਰਨ which)
{
    ਜੇ ((which & SAVE_RESTORE_TITLE) && T_CST != ਨਿਰਾ && *T_CST != NUL)
    {
	ਬਾਹਰਲਾ_STR(T_CST);
	ਬਾਹਰਲਾ_flush();
    }

    ਜੇ ((which & SAVE_RESTORE_ICON) && T_SSI != ਨਿਰਾ && *T_SSI != NUL)
    {
	ਬਾਹਰਲਾ_STR(T_SSI);
	ਬਾਹਰਲਾ_flush();
    }
}

/*
 * Tell the terminal to pop the title and/or icon.
 */
    ਖਾਲੀ
ਪਦ_ਕੱਢੋ_title(ਪੂਰਨ which)
{
    ਜੇ ((which & SAVE_RESTORE_TITLE) && T_CRT != ਨਿਰਾ && *T_CRT != NUL)
    {
	ਬਾਹਰਲਾ_STR(T_CRT);
	ਬਾਹਰਲਾ_flush();
    }

    ਜੇ ((which & SAVE_RESTORE_ICON) && T_SRI != ਨਿਰਾ && *T_SRI != NUL)
    {
	ਬਾਹਰਲਾ_STR(T_SRI);
	ਬਾਹਰਲਾ_flush();
    }
}
#ਜੇ_ਅੰਤ

/*
 * Make sure we have a valid set or terminal options.
 * Replace all entries that are NULL by empty_option
 */
    ਖਾਲੀ
ttest(ਪੂਰਨ ਜੋੜੇ)
{
    char_u *env_colors;

    check_options();		    // make sure no options are NULL

    /*
     * MUST have "cm": cursor motion.
     */
    ਜੇ (*T_CM == NUL)
	emsg(_(e_terminal_capability_cm_required));

    /*
     * if "cs" defined, use a scroll region, it's faster.
     */
    ਜੇ (*T_CS != NUL)
	scroll_region = TRUE;
    ਨਹੀਂਤਾਂ
	scroll_region = FALSE;

    ਜੇ (ਜੋੜੇ)
    {
	/*
	 * optional pairs
	 */
	// TP goes to normal mode for TI (invert) and TB (bold)
	ਜੇ (*T_ME == NUL)
	    T_ME = T_MR = T_MD = T_MB = ਖਾਲੀ_ਥਾਵਾਂ_option;
	ਜੇ (*T_SO == NUL || *T_SE == NUL)
	    T_SO = T_SE = ਖਾਲੀ_ਥਾਵਾਂ_option;
	ਜੇ (*T_US == NUL || *T_UE == NUL)
	    T_US = T_UE = ਖਾਲੀ_ਥਾਵਾਂ_option;
	ਜੇ (*T_CZH == NUL || *T_CZR == NUL)
	    T_CZH = T_CZR = ਖਾਲੀ_ਥਾਵਾਂ_option;

	// T_VE is needed even though T_VI is not defined
	ਜੇ (*T_VE == NUL)
	    T_VI = ਖਾਲੀ_ਥਾਵਾਂ_option;

	// if 'mr' or 'me' is not defined use 'so' and 'se'
	ਜੇ (*T_ME == NUL)
	{
	    T_ME = T_SE;
	    T_MR = T_SO;
	    T_MD = T_SO;
	}

	// if 'so' or 'se' is not defined use 'mr' and 'me'
	ਜੇ (*T_SO == NUL)
	{
	    T_SE = T_ME;
	    ਜੇ (*T_MR == NUL)
		T_SO = T_MD;
	    ਨਹੀਂਤਾਂ
		T_SO = T_MR;
	}

	// if 'ZH' or 'ZR' is not defined use 'mr' and 'me'
	ਜੇ (*T_CZH == NUL)
	{
	    T_CZR = T_ME;
	    ਜੇ (*T_MR == NUL)
		T_CZH = T_MD;
	    ਨਹੀਂਤਾਂ
		T_CZH = T_MR;
	}

	// "Sb" and "Sf" come in pairs
	ਜੇ (*T_CSB == NUL || *T_CSF == NUL)
	{
	    T_CSB = ਖਾਲੀ_ਥਾਵਾਂ_option;
	    T_CSF = ਖਾਲੀ_ਥਾਵਾਂ_option;
	}

	// "AB" and "AF" come in pairs
	ਜੇ (*T_CAB == NUL || *T_CAF == NUL)
	{
	    T_CAB = ਖਾਲੀ_ਥਾਵਾਂ_option;
	    T_CAF = ਖਾਲੀ_ਥਾਵਾਂ_option;
	}

	// if 'Sb' and 'AB' are not defined, reset "Co"
	ਜੇ (*T_CSB == NUL && *T_CAB == NUL)
	    free_one_termoption(T_CCO);

	// Set 'weirdinvert' according to value of 't_xs'
	p_wiv = (*T_XS != NUL);
    }
    need_gather = TRUE;

    // Set t_colors to the value of $COLORS or t_Co.  Ignore $COLORS in the
    // GUI.
    t_colors = atoi((ਅੱਖਰ *)T_CCO);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    ਜੇ (!gui.ਵਿੱਚ_use)
#ਜੇ_ਅੰਤ
    {
	env_colors = mch_getenv((char_u *)"COLORS");
	ਜੇ (env_colors != ਨਿਰਾ && ਸੁਰੱਖਿਅਤ_isdigit(*env_colors))
	{
	    ਪੂਰਨ colors = atoi((ਅੱਖਰ *)env_colors);

	    ਜੇ (colors != t_colors)
		ਸੈੱਟ_color_ਗਿਣਤੀ(colors);
	}
    }
    ਸੈੱਟ_rgb_ਪਦ_prop();
}

#ਜੇ defined(FEAT_GUI) && (defined(FEAT_MENU) || !defined(USE_ON_FLY_SCROLL))
/*
 * Represent the given long_u as individual bytes, with the most significant
 * byte first, and store them in dst.
 */
    ਖਾਲੀ
ਜੋੜੋ_long_ਮੰਜ਼ਿਲ_buf(long_u val, char_u *dst)
{
    ਪੂਰਨ	    i;
    ਪੂਰਨ	    ਖਿਸਕਾਓ;

    ਲਈ (i = 1; i <= (ਪੂਰਨ)ਆਕਾਰ(long_u); i++)
    {
	ਖਿਸਕਾਓ = 8 * (ਆਕਾਰ(long_u) - i);
	dst[i - 1] = (char_u) ((val >> ਖਿਸਕਾਓ) & 0xff);
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
    ਸਥਿਰ ਪੂਰਨ
get_long_from_buf(char_u *buf, long_u *val)
{
    ਪੂਰਨ	    len;
    char_u  ਬਾਈਟਾਂ[ਆਕਾਰ(long_u)];
    ਪੂਰਨ	    i;
    ਪੂਰਨ	    ਖਿਸਕਾਓ;

    *val = 0;
    len = get_ਬਾਈਟਾਂ_from_buf(buf, ਬਾਈਟਾਂ, (ਪੂਰਨ)ਆਕਾਰ(long_u));
    ਜੇ (len == -1)
	ਵਾਪਸ -1;
    ਲਈ (i = 0; i < (ਪੂਰਨ)ਆਕਾਰ(long_u); i++)
    {
	ਖਿਸਕਾਓ = 8 * (ਆਕਾਰ(long_u) - 1 - i);
	*val += (long_u)ਬਾਈਟਾਂ[i] << ਖਿਸਕਾਓ;
    }
    ਵਾਪਸ len;
}
#ਜੇ_ਅੰਤ

/*
 * Read the next num_bytes bytes from buf, and store them in bytes.  Assume
 * that buf has been through inchar().	Returns the actual number of bytes used
 * from buf (between num_bytes and num_bytes*2), or -1 if not enough bytes were
 * available.
 */
    ਪੂਰਨ
get_ਬਾਈਟਾਂ_from_buf(char_u *buf, char_u *ਬਾਈਟਾਂ, ਪੂਰਨ ਅੰਕ_ਬਾਈਟਾਂ)
{
    ਪੂਰਨ	    len = 0;
    ਪੂਰਨ	    i;
    char_u  c;

    ਲਈ (i = 0; i < ਅੰਕ_ਬਾਈਟਾਂ; i++)
    {
	ਜੇ ((c = buf[len++]) == NUL)
	    ਵਾਪਸ -1;
	ਜੇ (c == K_SPECIAL)
	{
	    ਜੇ (buf[len] == NUL || buf[len + 1] == NUL)	    // cannot happen?
		ਵਾਪਸ -1;
	    ਜੇ (buf[len++] == (ਪੂਰਨ)KS_ZERO)
		c = NUL;
	    // else it should be KS_SPECIAL; when followed by KE_FILLER c is
	    // K_SPECIAL, or followed by KE_CSI and c must be CSI.
	    ਜੇ (buf[len++] == (ਪੂਰਨ)KE_CSI)
		c = CSI;
	}
	ਨਹੀਂਤਾਂ ਜੇ (c == CSI && buf[len] == KS_EXTRA
					       && buf[len + 1] == (ਪੂਰਨ)KE_CSI)
	    // CSI is stored as CSI KS_SPECIAL KE_CSI to avoid confusion with
	    // the start of a special key, see add_to_input_buf_csi().
	    len += 2;
	ਬਾਈਟਾਂ[i] = c;
    }
    ਵਾਪਸ len;
}

/*
 * Check if the new shell size is valid, correct it if it's too small or way
 * too big.
 */
    ਖਾਲੀ
check_shellsize(ਖਾਲੀ)
{
    // need room for one window and command line
    ਜੇ (ਪੰਕਤੀਆਂ < min_ਪੰਕਤੀਆਂ_for_all_tabpages())
	ਪੰਕਤੀਆਂ = min_ਪੰਕਤੀਆਂ_for_all_tabpages();
    ਹੱਦਬੰਦੀ_screen_ਮਾਪ();

    // make sure these values are not invalid
    ਜੇ (cmdline_ਪੰਕਤੀ >= ਪੰਕਤੀਆਂ)
	cmdline_ਪੰਕਤੀ = ਪੰਕਤੀਆਂ - 1;
    ਜੇ (ਸੁਨੇਹਾ_ਪੰਕਤੀ >= ਪੰਕਤੀਆਂ)
	ਸੁਨੇਹਾ_ਪੰਕਤੀ = ਪੰਕਤੀਆਂ - 1;
}

/*
 * Limit Rows and Columns to avoid an overflow in Rows * Columns.
 */
    ਖਾਲੀ
ਹੱਦਬੰਦੀ_screen_ਮਾਪ(ਖਾਲੀ)
{
    ਜੇ (Columns < MIN_COLUMNS)
	Columns = MIN_COLUMNS;
    ਨਹੀਂਤਾਂ ਜੇ (Columns > 10000)
	Columns = 10000;
    ਜੇ (ਪੰਕਤੀਆਂ > 1000)
	ਪੰਕਤੀਆਂ = 1000;
}

/*
 * Invoked just before the screen structures are going to be (re)allocated.
 */
    ਖਾਲੀ
win_new_shellsize(ਖਾਲੀ)
{
    ਸਥਿਰ ਪੂਰਨ	old_ਪੰਕਤੀਆਂ_2 = 0;
    ਸਥਿਰ ਪੂਰਨ	old_Columns = 0;
    ਸਥਿਰ ਪੂਰਨ	old_coloff = 0;

    ਜੇ (old_ਪੰਕਤੀਆਂ_2 != ਪੰਕਤੀਆਂ || old_Columns != COLUMNS_WITHOUT_TPL()
	    || old_coloff != TPL_LCOL())
	ui_new_shellsize();
    ਜੇ (old_Columns != COLUMNS_WITHOUT_TPL() || old_coloff != TPL_LCOL())
    {
	old_Columns = COLUMNS_WITHOUT_TPL();
	old_coloff = TPL_LCOL();

	shell_new_columns();
    }
    ਜੇ (old_ਪੰਕਤੀਆਂ_2 != ਪੰਕਤੀਆਂ)
    {
	// If 'window' uses the whole screen, keep it using that.
	// Don't change it when set with "-w size" on the command line.
	ਜੇ (p_ਖਿੜਕੀ == old_ਪੰਕਤੀਆਂ_2 - 1
		    || (old_ਪੰਕਤੀਆਂ_2 == 0 && !option_was_ਸੈੱਟ((char_u *)"window")))
	    p_ਖਿੜਕੀ = ਪੰਕਤੀਆਂ - 1;
	old_ਪੰਕਤੀਆਂ_2 = ਪੰਕਤੀਆਂ;
	shell_new_ਪੰਕਤੀਆਂ();	// update window sizes
    }
}

#ਜੇ defined(FEAT_IMAGE) || defined(FEAT_EVAL)
    ਖਾਲੀ
update_cell_ਮਾਪ(ਖਾਲੀ)
{
# ifdef FEAT_GUI
    ਜੇ (gui.ਵਿੱਚ_use)
    {
	cell_ਚੌੜਾਈ = gui.char_ਚੌੜਾਈ;
	cell_ਉਚਾਈ = gui.char_ਉਚਾਈ;
    }
    ਨਹੀਂਤਾਂ
# endif
    {
	ਬਣਤਰ cellsize cell_sz;

	mch_calc_cell_ਮਾਪ(&cell_sz);
	ਜੇ (cell_sz.cs_xpixel <= 0 || cell_sz.cs_ypixel <= 0)
	{
	    cell_ਚੌੜਾਈ = 8;
	    cell_ਉਚਾਈ = 16;
	}
	ਨਹੀਂਤਾਂ
	{
	    cell_ਚੌੜਾਈ = cell_sz.cs_xpixel;
	    cell_ਉਚਾਈ = cell_sz.cs_ypixel;
	}
    }
}
#ਜੇ_ਅੰਤ

/*
 * Call this function when the Vim shell has been resized in any way.
 * Will obtain the current size and redraw (also when size didn't change).
 */
    ਖਾਲੀ
shell_resized(ਖਾਲੀ)
{
#ਜੇ defined(FEAT_IMAGE) || defined(FEAT_EVAL)
    update_cell_ਮਾਪ();
#ਜੇ_ਅੰਤ
    ਸੈੱਟ_shellsize(0, 0, FALSE);
}

/*
 * Check if the shell size changed.  Handle a resize.
 * When the size didn't change, nothing happens.
 */
    ਖਾਲੀ
shell_resized_check(ਖਾਲੀ)
{
    ਪੂਰਨ		old_ਪੰਕਤੀਆਂ_2 = ਪੰਕਤੀਆਂ;
    ਪੂਰਨ		old_Columns = Columns;

    ਜੇ (exiting
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
	    // Do not get the size when executing a shell command during
	    // startup.
	    || gui.starting
#ਜੇ_ਅੰਤ
	    )
	ਵਾਪਸ;

    (ਖਾਲੀ)ui_get_shellsize();
    check_shellsize();
    ਜੇ (old_ਪੰਕਤੀਆਂ_2 != ਪੰਕਤੀਆਂ || old_Columns != Columns)
	shell_resized();
}

/*
 * Set size of the Vim shell.
 * If 'mustset' is TRUE, we must set Rows and Columns, do not get the real
 * window size (this is used for the :win command).
 * If 'mustset' is FALSE, we may try to get the real window size and if
 * it fails use 'width' and 'height'.
 */
    ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_shellsize_inner(ਪੂਰਨ ਚੌੜਾਈ, ਪੂਰਨ ਉਚਾਈ, ਪੂਰਨ mustset)
{
    ਜੇ (updating_screen)
	// resizing while in update_screen() may cause a crash
	ਵਾਪਸ;

    // curwin->w_buffer can be NULL when we are closing a window and the
    // buffer (or window) has already been closed and removing a scrollbar
    // causes a resize event. Don't resize then, it will happen after entering
    // another buffer.
    ਜੇ (curwin->w_ਬਫਰ == ਨਿਰਾ || curwin->w_ਸਤਰਾਂ == ਨਿਰਾ)
	ਵਾਪਸ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ AMIGA
    ਬਾਹਰਲਾ_flush();	    // must do this before mch_get_shellsize() for
			    // some obscure reason
#ਜੇ_ਅੰਤ

    ਜੇ (mustset || (ui_get_shellsize() == FAIL && ਉਚਾਈ != 0))
    {
	ਪੰਕਤੀਆਂ = ਉਚਾਈ;
	Columns = ਚੌੜਾਈ;
	check_shellsize();
	ui_ਸੈੱਟ_shellsize(mustset);
    }
    ਨਹੀਂਤਾਂ
	check_shellsize();

    // The window layout used to be adjusted here, but it now happens in
    // screenalloc() (also invoked from screenclear()).  That is because the
    // "busy" check above may skip this, but not screenalloc().

    ਜੇ (State != ਬਹੁਲਕ_ASKMORE && State != ਬਹੁਲਕ_EXTERNCMD
						      && State != ਬਹੁਲਕ_CONFIRM)
	screenclear();
    ਨਹੀਂਤਾਂ
	screen_ਸ਼ੁਰੂ();	    // don't know where cursor is now

    ਜੇ (starting != NO_SCREEN)
    {
	maketitle();

	ਬਦਲਿਆ_ਸਤਰ_abv_curs();
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
	ਜੇ (State == ਬਹੁਲਕ_ASKMORE || State == ਬਹੁਲਕ_EXTERNCMD
				     || State == ਬਹੁਲਕ_CONFIRM || exmode_ਸਰਗਰਮ)
	{
	    screenalloc(FALSE);
	    repeat_ਸੁਨੇਹਾ();
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (curwin->w_p_scb)
		do_check_scrollbind(TRUE);
	    ਜੇ (State & ਬਹੁਲਕ_CMDLINE)
	    {
		update_screen(UPD_NOT_VALID);
		redrawcmdline();
	    }
	    ਨਹੀਂਤਾਂ
	    {
		update_topline();
		ਜੇ (pum_visible())
		{
		    redraw_later(UPD_NOT_VALID);
		    ins_compl_show_pum();
		}
		update_screen(UPD_NOT_VALID);
		ਜੇ (redrawing())
		    setcursor();
	    }
	}
	cursor_on();	    // redrawing may have switched it off
    }
    ਬਾਹਰਲਾ_flush();
}

    ਖਾਲੀ
ਸੈੱਟ_shellsize(ਪੂਰਨ ਚੌੜਾਈ, ਪੂਰਨ ਉਚਾਈ, ਪੂਰਨ mustset)
{
    ਸਥਿਰ ਪੂਰਨ	busy = FALSE;
    ਸਥਿਰ ਪੂਰਨ	do_ਲੜੀਵਾਰ = FALSE;

    ਜੇ (ਚੌੜਾਈ < 0 || ਉਚਾਈ < 0)    // just checking...
	ਵਾਪਸ;

    ਜੇ (State == ਬਹੁਲਕ_HITRETURN || State == ਬਹੁਲਕ_SETWSIZE)
    {
	// postpone the resizing
	State = ਬਹੁਲਕ_SETWSIZE;
	ਵਾਪਸ;
    }

    // Avoid recursiveness.  This can happen when setting the window size
    // causes another window-changed signal or when two SIGWINCH signals come
    // very close together.  There needs to be another run then after the
    // current one is done to pick up the latest size.
    do_ਲੜੀਵਾਰ = TRUE;
    ਜੇ (busy)
	ਵਾਪਸ;

    ਜਦੋਂ (do_ਲੜੀਵਾਰ)
    {
	do_ਲੜੀਵਾਰ = FALSE;
	busy = TRUE;
	ਸੈੱਟ_shellsize_inner(ਚੌੜਾਈ, ਉਚਾਈ, mustset);
	busy = FALSE;
    }
}

/*
 * Output T_CTE, the t_TE termcap entry, and handle expected effects.
 * The code possibly disables modifyOtherKeys and the Kitty keyboard protocol.
 */
    ਖਾਲੀ
ਬਾਹਰਲਾ_str_t_TE(ਖਾਲੀ)
{
    ਬਾਹਰਲਾ_str(T_CTE);

    // The seenModifyOtherKeys flag is not reset here.  We do expect t_TE to
    // disable modifyOtherKeys, but until Xterm version 377 there is no way to
    // detect it's enabled again after the following t_TI.  We assume that when
    // seenModifyOtherKeys was set before it will still be valid.

    // When the modifyOtherKeys level is detected to be 2 we expect t_TE to
    // disable it.  Remembering that it was detected to be enabled is useful in
    // some situations.
    // The following t_TI is expected to request the state and then
    // modify_otherkeys_state will be set again.
    ਜੇ (modify_otherkeys_state == MOKS_ENABLED
	    || modify_otherkeys_state == MOKS_DISABLED)
	modify_otherkeys_state = MOKS_DISABLED;
    ਨਹੀਂਤਾਂ ਜੇ (modify_otherkeys_state != MOKS_INITIAL)
	modify_otherkeys_state = MOKS_AFTER_T_TE;

    // When the kitty keyboard protocol is enabled we expect t_TE to disable
    // it.  Remembering that it was detected to be enabled is useful in some
    // situations.
    // The following t_TI is expected to request the state and then
    // kitty_protocol_state will be set again.
    ਜੇ (kitty_protocol_state == KKPS_ENABLED
	    || kitty_protocol_state == KKPS_DISABLED)
	kitty_protocol_state = KKPS_DISABLED;
    ਨਹੀਂਤਾਂ
	kitty_protocol_state = KKPS_AFTER_T_TE;
}

ਸਥਿਰ ਪੂਰਨ send_t_RK = FALSE;

/*
 * Output T_TI and setup for what follows.
 */
    ਖਾਲੀ
ਬਾਹਰਲਾ_str_t_TI(ਖਾਲੀ)
{
    ਬਾਹਰਲਾ_str(T_CTI);

    // Send t_RK when there is no more work to do.
    send_t_RK = TRUE;
}

/*
 * Output T_BE, but only when t_PS and t_PE are set.
 */
    ਖਾਲੀ
ਬਾਹਰਲਾ_str_t_BE(ਖਾਲੀ)
{
    char_u *p;

    ਜੇ (T_BE == ਨਿਰਾ || *T_BE == NUL
	    || (p = ਲੱਭੋ_termcode((char_u *)"PS")) == ਨਿਰਾ || *p == NUL
	    || (p = ਲੱਭੋ_termcode((char_u *)"PE")) == ਨਿਰਾ || *p == NUL)
	ਵਾਪਸ;
    ਬਾਹਰਲਾ_str(T_BE);
}

/*
 * If t_TI was recently sent and there is no typeahead or work to do, now send
 * t_RK.  This is postponed to avoid the response arriving in a shell command
 * or after Vim exits.
 */
    ਖਾਲੀ
may_send_t_RK(ਖਾਲੀ)
{
    ਜੇ (send_t_RK
	    && !work_pending()
	    && !ex_ਸਾਧਾਰਨ_busy
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	    && !ਵਿੱਚ_feedkeys
#ਜੇ_ਅੰਤ
	    && !exiting)
    {
	send_t_RK = FALSE;
	ਬਾਹਰਲਾ_str(T_CRK);
	ਬਾਹਰਲਾ_flush();
    }
}

/*
 * Set the terminal to TMODE_RAW (for Normal mode) or TMODE_COOK (for external
 * commands and Ex mode).
 */
    ਖਾਲੀ
settmode(tmode_T tmode)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    // don't set the term where gvim was started to any mode
    ਜੇ (gui.ਵਿੱਚ_use)
	ਵਾਪਸ;
#ਜੇ_ਅੰਤ

    ਜੇ (!ਭਰਿਆ_screen)
	ਵਾਪਸ;

    /*
     * When returning after calling a shell cur_tmode is TMODE_UNKNOWN,
     * set the terminal to raw mode, even though we think it already is,
     * because the shell program may have reset the terminal mode.
     * When we think the terminal is normal, don't try to set it to
     * normal again, because that causes problems (logout!) on some
     * machines.
     */
    ਜੇ (tmode != cur_tmode)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMRESPONSE
# ifdef FEAT_GUI
	ਜੇ (!gui.ਵਿੱਚ_use && !gui.starting)
# endif
	{
	    // May need to check for T_CRV response and termcodes, it
	    // doesn't work in Cooked mode, an external program may get
	    // them.
	    ਜੇ (tmode != TMODE_RAW && termrequest_any_pending())
		(ਖਾਲੀ)vpeekc_nomap();
	    check_for_codes_from_ਪਦ();
	}
#ਜੇ_ਅੰਤ
	ਜੇ (tmode != TMODE_RAW)
	    mch_setmouse(FALSE);	// switch mouse off

	// Disable bracketed paste and modifyOtherKeys in cooked mode.
	// Avoid doing this too often, on some terminals the codes are not
	// handled properly.
	ਜੇ (termcap_ਸਰਗਰਮ && tmode != TMODE_SLEEP
		&& cur_tmode != TMODE_SLEEP)
	{
	    MAY_WANT_ਮੰਜ਼ਿਲ_LOG_THIS;

	    ਜੇ (tmode != TMODE_RAW)
	    {
		ਬਾਹਰਲਾ_str(T_BD);	// disable bracketed paste mode
		ਬਾਹਰਲਾ_str_t_TE();	// possibly disables modifyOtherKeys
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ਬਾਹਰਲਾ_str_t_BE();	// enable bracketed paste mode (should
				// be before mch_settmode().
		ਬਾਹਰਲਾ_str_t_TI();	// possibly enables modifyOtherKeys
	    }
	}
	ਬਾਹਰਲਾ_flush();
	mch_settmode(tmode);	// machine specific function
	cur_tmode = tmode;
	ਜੇ (tmode == TMODE_RAW)
	    setmouse();		// may switch mouse on
	ਬਾਹਰਲਾ_flush();
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMRESPONSE
    may_req_termresponse();
#ਜੇ_ਅੰਤ
}

    ਖਾਲੀ
starttermcap(ਖਾਲੀ)
{
    ਜੇ (!ਭਰਿਆ_screen || termcap_ਸਰਗਰਮ)
	ਵਾਪਸ;

    MAY_WANT_ਮੰਜ਼ਿਲ_LOG_THIS;

    ਬਾਹਰਲਾ_str(T_TI);			// start termcap mode
    ਬਾਹਰਲਾ_str_t_TI();			// start "raw" mode
    ਬਾਹਰਲਾ_str(T_KS);			// start "keypad transmit" mode
    ਬਾਹਰਲਾ_str_t_BE();			// enable bracketed paste mode

#ਜੇ defined(UNIX) || defined(VMS)
    // Enable xterm's focus reporting mode when 'esckeys' is set.
    ਜੇ (p_ek && *T_FE != NUL)
	ਬਾਹਰਲਾ_str(T_FE);
#ਜੇ_ਅੰਤ

    ਬਾਹਰਲਾ_flush();
    termcap_ਸਰਗਰਮ = TRUE;
    screen_ਸ਼ੁਰੂ();			// don't know where cursor is now
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMRESPONSE
# ifdef FEAT_GUI
    ਜੇ (!gui.ਵਿੱਚ_use && !gui.starting)
# endif
    {
	may_req_termresponse();
	// Immediately check for a response.  If t_Co changes, we don't
	// want to redraw with wrong colors first.
	ਜੇ (crv_ਸਥਿਤੀ.tr_ਤਰੱਕੀ == ਸਥਿਤੀ_ਭੇਜਿਆ)
	    check_for_codes_from_ਪਦ();
    }
#ਜੇ_ਅੰਤ
}

    ਖਾਲੀ
stoptermcap(ਖਾਲੀ)
{
    screen_stop_highlight();
    reset_cterm_colors();

    ਜੇ (!termcap_ਸਰਗਰਮ)
	ਵਾਪਸ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMRESPONSE
# ifdef FEAT_GUI
    ਜੇ (!gui.ਵਿੱਚ_use && !gui.starting)
# endif
    {
	// May need to discard T_CRV, T_U7 or T_RBG response.
	ਜੇ (termrequest_any_pending())
	{
# ifdef UNIX
	    // Give the terminal a chance to respond.
	    mch_delay(100L, 0);
# endif
# ifdef TCIFLUSH
	    // Discard data received but not read.
	    ਜੇ (exiting)
		tcflush(fileno(stdin), TCIFLUSH);
# endif
	}
	// Check for termcodes first, otherwise an external program may
	// get them.
	check_for_codes_from_ਪਦ();
    }
#ਜੇ_ਅੰਤ
    MAY_WANT_ਮੰਜ਼ਿਲ_LOG_THIS;

#ਜੇ defined(UNIX) || defined(VMS)
    // Disable xterm's focus reporting mode if 'esckeys' is set.
    ਜੇ (p_ek && *T_FD != NUL)
	ਬਾਹਰਲਾ_str(T_FD);
#ਜੇ_ਅੰਤ

    ਬਾਹਰਲਾ_str(T_BD);			// disable bracketed paste mode
    ਬਾਹਰਲਾ_str(T_KE);			// stop "keypad transmit" mode
    ਬਾਹਰਲਾ_flush();
    termcap_ਸਰਗਰਮ = FALSE;

    // Output t_te before t_TE, t_te may switch between main and alternate
    // screen and following codes may work on the active screen only.
    //
    // When using the Kitty keyboard protocol the main and alternate screen
    // use a separate state.  If we are (or were) using the Kitty keyboard
    // protocol and t_te is not empty (possibly switching screens) then
    // output t_TE both before and after outputting t_te.
    ਜੇ (*T_TE != NUL && (kitty_protocol_state == KKPS_ENABLED
		|| kitty_protocol_state == KKPS_DISABLED))
	ਬਾਹਰਲਾ_str_t_TE();		// probably disables the kitty keyboard
				// protocol

    ਬਾਹਰਲਾ_str(T_TE);			// stop termcap mode
    cursor_on();			// just in case it is still off
    ਬਾਹਰਲਾ_str_t_TE();			// stop "raw" mode, modifyOtherKeys and
					// Kitty keyboard protocol
    screen_ਸ਼ੁਰੂ();			// don't know where cursor is now
    ਬਾਹਰਲਾ_flush();
}

#ਜੇ defined(FEAT_TERMRESPONSE)
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
    ਖਾਲੀ
may_req_termresponse(ਖਾਲੀ)
{
    ਜੇ (crv_ਸਥਿਤੀ.tr_ਤਰੱਕੀ == ਸਥਿਤੀ_GET
	    && can_get_termresponse()
	    && starting == 0
	    && *T_CRV != NUL)
    {
	MAY_WANT_ਮੰਜ਼ਿਲ_LOG_THIS;
	LOG_TR1("Sending CRV request");
	ਬਾਹਰਲਾ_str(T_CRV);
	termrequest_ਭੇਜਿਆ(&crv_ਸਥਿਤੀ);
	// check for the characters now, otherwise they might be eaten by
	// get_keystroke()
	ਬਾਹਰਲਾ_flush();
	(ਖਾਲੀ)vpeekc_nomap();
    }
}

/*
 * Send sequences to the terminal and check with t_u7 how the cursor moves, to
 * find out properties of the terminal.
 * Note that this goes out before T_CRV, so that the result can be used when
 * the termresponse arrives.
 */
    ਖਾਲੀ
check_terminal_behavior(ਖਾਲੀ)
{
    ਪੂਰਨ	    did_send = FALSE;

    ਜੇ (!can_get_termresponse() || starting != 0 || *T_U7 == NUL)
	ਵਾਪਸ;

    ਜੇ (u7_ਸਥਿਤੀ.tr_ਤਰੱਕੀ == ਸਥਿਤੀ_GET
	    && !option_was_ਸੈੱਟ((char_u *)"ambiwidth"))
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
	MAY_WANT_ਮੰਜ਼ਿਲ_LOG_THIS;
	LOG_TR1("Sending request for ambiwidth check");
	// Do this in the second row.  In the first row the returned sequence
	// may be CSI 1;2R, which is the same as <S-F3>.
	ਪਦ_windgoto(1, 0);
	buf[mb_char2bytes(0x25bd, buf)] = NUL;
	ਬਾਹਰਲਾ_str(buf);
	ਬਾਹਰਲਾ_str(T_U7);
	termrequest_ਭੇਜਿਆ(&u7_ਸਥਿਤੀ);
	ਬਾਹਰਲਾ_flush();
	did_send = TRUE;

	// This overwrites a few characters on the screen, a redraw is needed
	// after this. Clear them out for now.
	screen_stop_highlight();
	ਪਦ_windgoto(1, 0);
	ਬਾਹਰਲਾ_str((char_u *)"  ");
	ਸਤਰ_was_clobbered(1);
    }

    ਜੇ (xcc_ਸਥਿਤੀ.tr_ਤਰੱਕੀ == ਸਥਿਤੀ_GET && ਪੰਕਤੀਆਂ > 2)
    {
	// 2. Check compatibility with xterm.
	// We move the cursor to (2, 0), print a test sequence and then query
	// the current cursor position.  If the terminal properly handles
	// unknown DCS string and CSI sequence with intermediate byte, the test
	// sequence is ignored and the cursor does not move.  If the terminal
	// handles test sequence incorrectly, a garbage string is displayed and
	// the cursor does move.
	MAY_WANT_ਮੰਜ਼ਿਲ_LOG_THIS;
	LOG_TR1("Sending xterm compatibility test sequence.");
	// Do this in the third row.  Second row is used by ambiguous
	// character width check.
	ਪਦ_windgoto(2, 0);
	// send the test DCS string.
	ਬਾਹਰਲਾ_str((char_u *)"\033Pzz\033\\");
	// send the test CSI sequence with intermediate byte.
	ਬਾਹਰਲਾ_str((char_u *)"\033[0%m");
	ਬਾਹਰਲਾ_str(T_U7);
	termrequest_ਭੇਜਿਆ(&xcc_ਸਥਿਤੀ);
	ਬਾਹਰਲਾ_flush();
	did_send = TRUE;

	// If the terminal handles test sequence incorrectly, garbage text is
	// displayed. Clear them out for now.
	screen_stop_highlight();
	ਪਦ_windgoto(2, 0);
	ਬਾਹਰਲਾ_str((char_u *)"           ");
	ਸਤਰ_was_clobbered(2);
    }

    ਜੇ (did_send)
    {
	ਪਦ_windgoto(0, 0);

	// Need to reset the known cursor position.
	screen_ਸ਼ੁਰੂ();

	// check for the characters now, otherwise they might be eaten by
	// get_keystroke()
	ਬਾਹਰਲਾ_flush();
	(ਖਾਲੀ)vpeekc_nomap();
    }
}

/*
 * Similar to requesting the version string: Request the terminal background
 * color when it is the right moment.
 */
    ਖਾਲੀ
may_req_bg_color(ਖਾਲੀ)
{
    ਜੇ (can_get_termresponse() && starting == 0)
    {
	ਪੂਰਨ didit = FALSE;

# ifdef FEAT_TERMINAL
	// Only request foreground if t_RF is set.
	ਜੇ (rfg_ਸਥਿਤੀ.tr_ਤਰੱਕੀ == ਸਥਿਤੀ_GET && *T_RFG != NUL)
	{
	    MAY_WANT_ਮੰਜ਼ਿਲ_LOG_THIS;
	    LOG_TR1("Sending FG request");
	    ਬਾਹਰਲਾ_str(T_RFG);
	    termrequest_ਭੇਜਿਆ(&rfg_ਸਥਿਤੀ);
	    didit = TRUE;
	}
# endif

	// Only request background if t_RB is set.
	ਜੇ (rbg_ਸਥਿਤੀ.tr_ਤਰੱਕੀ == ਸਥਿਤੀ_GET && *T_RBG != NUL)
	{
	    MAY_WANT_ਮੰਜ਼ਿਲ_LOG_THIS;
	    LOG_TR1("Sending BG request");
	    ਬਾਹਰਲਾ_str(T_RBG);
	    termrequest_ਭੇਜਿਆ(&rbg_ਸਥਿਤੀ);
	    didit = TRUE;
	}

	ਜੇ (didit)
	{
	    // check for the characters now, otherwise they might be eaten by
	    // get_keystroke()
	    ਬਾਹਰਲਾ_flush();
	    (ਖਾਲੀ)vpeekc_nomap();
	}
    }
}

#ਜੇ_ਅੰਤ

/*
 * Return TRUE when saving and restoring the screen.
 */
    ਪੂਰਨ
swapping_screen(ਖਾਲੀ)
{
    ਵਾਪਸ (ਭਰਿਆ_screen && *T_TI != NUL);
}

/*
 * By outputting the 'cursor very visible' termcap code, for some windowed
 * terminals this makes the screen scrolled to the correct position.
 * Used when starting Vim or returning from a shell.
 */
    ਖਾਲੀ
scroll_ਸ਼ੁਰੂ(ਖਾਲੀ)
{
    ਜੇ (*T_VS == NUL || *T_CVS == NUL)
	ਵਾਪਸ;

    MAY_WANT_ਮੰਜ਼ਿਲ_LOG_THIS;
    ਬਾਹਰਲਾ_str(T_VS);
    ਬਾਹਰਲਾ_str(T_CVS);
    screen_ਸ਼ੁਰੂ();		// don't know where cursor is now
}

// True if cursor is not visible
ਸਥਿਰ ਪੂਰਨ cursor_is_off = FALSE;

// True if cursor is not visible due to an ongoing cursor-less sleep
ਸਥਿਰ ਪੂਰਨ cursor_is_asleep = FALSE;

/*
 * Enable the cursor without checking if it's already enabled.
 */
    ਖਾਲੀ
cursor_on_force(ਖਾਲੀ)
{
    ਬਾਹਰਲਾ_str(T_VE);
    cursor_is_off = FALSE;
    cursor_is_asleep = FALSE;
}

/*
 * Enable the cursor if it's currently off.
 */
    ਖਾਲੀ
cursor_on(ਖਾਲੀ)
{
    ਜੇ (cursor_is_off && !cursor_is_asleep)
	cursor_on_force();
}

/*
 * Disable the cursor.
 */
    ਖਾਲੀ
cursor_off(ਖਾਲੀ)
{
    ਜੇ (ਭਰਿਆ_screen && !cursor_is_off)
    {
	ਬਾਹਰਲਾ_str(T_VI);	    // disable cursor
	cursor_is_off = TRUE;
    }
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
/*
 * Check whether the cursor is invisible due to an ongoing cursor-less sleep
 */
    ਪੂਰਨ
cursor_is_sleeping(ਖਾਲੀ)
{
    ਵਾਪਸ cursor_is_asleep;
}
#ਜੇ_ਅੰਤ

/*
 * Disable the cursor and mark it disabled by cursor-less sleep
 */
    ਖਾਲੀ
cursor_sleep(ਖਾਲੀ)
{
    cursor_is_asleep = TRUE;
    cursor_off();
}

/*
 * Enable the cursor and mark it not disabled by cursor-less sleep
 */
    ਖਾਲੀ
cursor_unsleep(ਖਾਲੀ)
{
    cursor_is_asleep = FALSE;
    cursor_on();
}

#ਜੇ defined(CURSOR_SHAPE)
/*
 * Set cursor shape to match Insert or Replace mode.
 */
    ਖਾਲੀ
ਪਦ_cursor_ਬਹੁਲਕ(ਪੂਰਨ forced)
{
    ਸਥਿਰ ਪੂਰਨ showing_ਬਹੁਲਕ = -1;
    char_u *p;

    // Only do something when redrawing the screen and we can restore the
    // mode.
    ਜੇ (!ਭਰਿਆ_screen || *T_CEI == NUL)
    {
# ifdef FEAT_TERMRESPONSE
	ਜੇ (forced && initial_cursor_shape > 0)
	    // Restore to initial values.
	    ਪਦ_cursor_shape(initial_cursor_shape, initial_cursor_blink);
# endif
	ਵਾਪਸ;
    }

    ਜੇ ((State & ਬਹੁਲਕ_REPLACE) == ਬਹੁਲਕ_REPLACE)
    {
	ਜੇ (forced || showing_ਬਹੁਲਕ != ਬਹੁਲਕ_REPLACE)
	{
	    ਜੇ (*T_CSR != NUL)
		p = T_CSR;	// Replace mode cursor
	    ਨਹੀਂਤਾਂ
		p = T_CSI;	// fall back to Insert mode cursor
	    ਜੇ (*p != NUL)
	    {
		ਬਾਹਰਲਾ_str(p);
		showing_ਬਹੁਲਕ = ਬਹੁਲਕ_REPLACE;
	    }
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (State & ਬਹੁਲਕ_ਪਾਓ)
    {
	ਜੇ ((forced || showing_ਬਹੁਲਕ != ਬਹੁਲਕ_ਪਾਓ) && *T_CSI != NUL)
	{
	    ਬਾਹਰਲਾ_str(T_CSI);	    // Insert mode cursor
	    showing_ਬਹੁਲਕ = ਬਹੁਲਕ_ਪਾਓ;
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (forced || showing_ਬਹੁਲਕ != ਬਹੁਲਕ_ਸਾਧਾਰਨ)
    {
	ਬਾਹਰਲਾ_str(T_CEI);		    // non-Insert mode cursor
	showing_ਬਹੁਲਕ = ਬਹੁਲਕ_ਸਾਧਾਰਨ;
    }
}

# ਜੇ defined(FEAT_TERMINAL)
    ਖਾਲੀ
ਪਦ_cursor_color(char_u *color)
{
    ਜੇ (*T_CSC == NUL)
	ਵਾਪਸ;

    ਬਾਹਰਲਾ_str(T_CSC);		// set cursor color start
    ਬਾਹਰਲਾ_str_nf(color);
    ਬਾਹਰਲਾ_str(T_CEC);		// set cursor color end
    ਬਾਹਰਲਾ_flush();
}
# endif

    ਪੂਰਨ
blink_state_is_inverted(ਖਾਲੀ)
{
# ifdef FEAT_TERMRESPONSE
    ਵਾਪਸ rbm_ਸਥਿਤੀ.tr_ਤਰੱਕੀ == ਸਥਿਤੀ_GOT
	&& rcs_ਸਥਿਤੀ.tr_ਤਰੱਕੀ == ਸਥਿਤੀ_GOT
		&& initial_cursor_blink != initial_cursor_shape_blink;
# ਨਹੀਂਤਾਂ
    ਵਾਪਸ FALSE;
# endif
}

/*
 * "shape": 1 = block, 2 = underline, 3 = vertical bar
 */
    ਖਾਲੀ
ਪਦ_cursor_shape(ਪੂਰਨ shape, ਪੂਰਨ blink)
{
    ਜੇ (*T_CSH != NUL)
    {
	ਬਾਹਰਲਾ_STR(tgoto((ਅੱਖਰ *)T_CSH, 0, shape * 2 - blink));
	ਬਾਹਰਲਾ_flush();
    }
    ਨਹੀਂਤਾਂ
    {
	ਪੂਰਨ do_blink = blink;

	// t_SH is empty: try setting just the blink state.
	// The blink flags are XORed together, if the initial blinking from
	// style and shape differs, we need to invert the flag here.
	ਜੇ (blink_state_is_inverted())
	    do_blink = !blink;

	ਜੇ (do_blink && *T_VS != NUL)
	{
	    ਬਾਹਰਲਾ_str(T_VS);
	    ਬਾਹਰਲਾ_flush();
	}
	ਨਹੀਂਤਾਂ ਜੇ (!do_blink && *T_CVS != NUL)
	{
	    ਬਾਹਰਲਾ_str(T_CVS);
	    ਬਾਹਰਲਾ_flush();
	}
    }
}
#ਜੇ_ਅੰਤ

/*
 * Set scrolling region for window 'wp'.
 * The region starts 'off' lines from the start of the window.
 * Also set the vertical scroll region for a vertically split window.  Always
 * the full width of the window, excluding the vertical separator.
 */
    ਖਾਲੀ
scroll_region_ਸੈੱਟ(win_T *wp, ਪੂਰਨ off)
{
    ਬਾਹਰਲਾ_STR(tgoto((ਅੱਖਰ *)T_CS, W_WINROW(wp) + wp->w_ਉਚਾਈ - 1,
							 W_WINROW(wp) + off));
    ਜੇ (*T_CSV != NUL && wp->w_ਚੌੜਾਈ != Columns)
	ਬਾਹਰਲਾ_STR(tgoto((ਅੱਖਰ *)T_CSV, wp->w_wincol + wp->w_ਚੌੜਾਈ - 1,
							       wp->w_wincol));
    screen_ਸ਼ੁਰੂ();		    // don't know where cursor is now
}

/*
 * Reset scrolling region to the whole screen.
 */
    ਖਾਲੀ
scroll_region_reset(ਖਾਲੀ)
{
    ਬਾਹਰਲਾ_STR(tgoto((ਅੱਖਰ *)T_CS, (ਪੂਰਨ)ਪੰਕਤੀਆਂ - 1, 0));
    ਜੇ (*T_CSV != NUL)
	ਬਾਹਰਲਾ_STR(tgoto((ਅੱਖਰ *)T_CSV, (ਪੂਰਨ)Columns - 1, 0));
    screen_ਸ਼ੁਰੂ();		    // don't know where cursor is now
}


/*
 * List of terminal codes that are currently recognized.
 */

ਸਥਿਰ ਬਣਤਰ termcode
{
    char_u  name[2];	    // termcap name of entry
    char_u  *ਕੋਡ;	    // terminal code (in allocated memory)
    ਪੂਰਨ	    len;	    // STRLEN(code)
    ਪੂਰਨ	    modlen;	    // length of part before ";*~".
} *termcodes = ਨਿਰਾ;

ਸਥਿਰ ਪੂਰਨ  tc_max_len = 0; // number of entries that termcodes[] can hold
ਸਥਿਰ ਪੂਰਨ  tc_len = 0;	    // current number of entries in termcodes[]

ਸਥਿਰ ਪੂਰਨ termcode_ਤਾਰਾ(char_u *ਕੋਡ, ਪੂਰਨ len);

    ਖਾਲੀ
clear_termcodes(ਖਾਲੀ)
{
    ਜਦੋਂ (tc_len > 0)
	vim_free(termcodes[--tc_len].ਕੋਡ);
    VIM_CLEAR(termcodes);
    tc_max_len = 0;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_TGETENT
    BC = (ਅੱਖਰ *)ਖਾਲੀ_ਥਾਵਾਂ_option;
    ਉੱਪਰ_2 = (ਅੱਖਰ *)ਖਾਲੀ_ਥਾਵਾਂ_option;
    PC = NUL;			// set pad character to NUL
    ospeed = 0;
#ਜੇ_ਅੰਤ

    need_gather = TRUE;		// need to fill termleader[]
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ATC_FROM_ਪਦ 55

/*
 * For xterm we recognize special codes like "ESC[42;*X" and "ESC O*X" that
 * accept modifiers.
 * Set "termcodes[idx].modlen".
 */
    ਸਥਿਰ ਖਾਲੀ
adjust_modlen(ਪੂਰਨ idx)
{
    termcodes[idx].modlen = 0;
    ਪੂਰਨ j = termcode_ਤਾਰਾ(termcodes[idx].ਕੋਡ, termcodes[idx].len);
    ਜੇ (j <= 0)
	ਵਾਪਸ;

    termcodes[idx].modlen = termcodes[idx].len - 1 - j;
    // For "CSI[@;X" the "@" is not included in "modlen".
    ਜੇ (termcodes[idx].ਕੋਡ[termcodes[idx].modlen - 1] == '@')
	--termcodes[idx].modlen;
}

/*
 * Add a new entry for "name[2]" to the list of terminal codes.
 * Note that "name" may not have a terminating NUL.
 * The list is kept alphabetical for ":set termcap"
 * "flags" is TRUE when replacing 7-bit by 8-bit controls is desired.
 * "flags" can also be ATC_FROM_TERM for got_code_from_term().
 */
    ਖਾਲੀ
ਜੋੜੋ_termcode(char_u *name, char_u *string, ਪੂਰਨ flags)
{
    ਬਣਤਰ termcode *new_tc;
    ਪੂਰਨ		    i, j;
    char_u	    *s;
    ਪੂਰਨ		    len;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਅੱਖਰ	    *action = "Setting";
#ਜੇ_ਅੰਤ

    ਜੇ (string == ਨਿਰਾ || *string == NUL)
    {
	del_termcode(name);
	ਵਾਪਸ;
    }

#ਜੇ defined(MSWIN) && !defined(FEAT_GUI)
    s = vim_strnsave(string, STRLEN(string) + 1);
#ਨਹੀਂਤਾਂ
# ifdef VIMDLL
    ਜੇ (!gui.ਵਿੱਚ_use)
	s = vim_strnsave(string, STRLEN(string) + 1);
    ਨਹੀਂਤਾਂ
# endif
	s = vim_strsave(string);
#ਜੇ_ਅੰਤ
    ਜੇ (s == ਨਿਰਾ)
	ਵਾਪਸ;

    // Change leading <Esc>[ to CSI, change <Esc>O to <M-O>.
    ਜੇ (flags != 0 && flags != ATC_FROM_ਪਦ && ਪਦ_7to8bit(string) != 0)
    {
	STRMOVE(s, s + 1);
	s[0] = ਪਦ_7to8bit(string);
    }

#ਜੇ defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
# ifdef VIMDLL
    ਜੇ (!gui.ਵਿੱਚ_use)
# endif
    {
	ਜੇ (s[0] == K_NUL)
	{
	    STRMOVE(s + 1, s);
	    s[1] = 3;
	}
    }
#ਜੇ_ਅੰਤ

    len = (ਪੂਰਨ)STRLEN(s);

    need_gather = TRUE;		// need to fill termleader[]

    /*
     * need to make space for more entries
     */
    ਜੇ (tc_len == tc_max_len)
    {
	tc_max_len += 20;
	new_tc = ALLOC_MULT(ਬਣਤਰ termcode, tc_max_len);
	ਜੇ (new_tc == ਨਿਰਾ)
	{
	    tc_max_len -= 20;
	    vim_free(s);
	    ਵਾਪਸ;
	}
	ਲਈ (i = 0; i < tc_len; ++i)
	    new_tc[i] = termcodes[i];
	vim_free(termcodes);
	termcodes = new_tc;
    }

    /*
     * Look for existing entry with the same name, it is replaced.
     * Look for an existing entry that is alphabetical higher, the new entry
     * is inserted in front of it.
     */
    ਲਈ (i = 0; i < tc_len; ++i)
    {
	ਜੇ (termcodes[i].name[0] < name[0])
	    ਜਾਰੀ;
	ਜੇ (termcodes[i].name[0] == name[0])
	{
	    ਜੇ (termcodes[i].name[1] < name[1])
		ਜਾਰੀ;
	    /*
	     * Exact match: May replace old code.
	     */
	    ਜੇ (termcodes[i].name[1] == name[1])
	    {
		ਜੇ (flags == ATC_FROM_ਪਦ && (j = termcode_ਤਾਰਾ(
				    termcodes[i].ਕੋਡ, termcodes[i].len)) > 0)
		{
		    // Don't replace ESC[123;*X or ESC O*X with another when
		    // invoked from got_code_from_term().
		    ਜੇ (len == termcodes[i].len - j
			    && STRNCMP(s, termcodes[i].ਕੋਡ, len - 1) == 0
			    && s[len - 1]
				   == termcodes[i].ਕੋਡ[termcodes[i].len - 1])
		    {
			// They are equal but for the ";*": don't add it.
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
			ch_log(ਨਿਰਾ, "Termcap entry %c%c did not change",
							     name[0], name[1]);
#ਜੇ_ਅੰਤ
			vim_free(s);
			ਵਾਪਸ;
		    }
		}
		ਨਹੀਂਤਾਂ
		{
		    // Replace old code.
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
		    ch_log(ਨਿਰਾ, "Termcap entry %c%c was: %s",
					  name[0], name[1], termcodes[i].ਕੋਡ);
#ਜੇ_ਅੰਤ
		    vim_free(termcodes[i].ਕੋਡ);
		    --tc_len;
		    ਰੋਕੋ;
		}
	    }
	}
	/*
	 * Found alphabetical larger entry, move rest to insert new entry
	 */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	action = "Adding";
#ਜੇ_ਅੰਤ
	ਲਈ (j = tc_len; j > i; --j)
	    termcodes[j] = termcodes[j - 1];
	ਰੋਕੋ;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ch_log(ਨਿਰਾ, "%s termcap entry %c%c to %s", action, name[0], name[1], s);
#ਜੇ_ਅੰਤ
    termcodes[i].name[0] = name[0];
    termcodes[i].name[1] = name[1];
    termcodes[i].ਕੋਡ = s;
    termcodes[i].len = len;
    adjust_modlen(i);

    ++tc_len;
}

/*
 * Some function keys may include modifiers, but the terminfo/termcap entries
 * do not indicate that.  Insert ";*" where we expect modifiers might appear.
 */
    ਸਥਿਰ ਖਾਲੀ
accept_modifiers_for_function_ਕੁੰਜੀਆਂ(ਖਾਲੀ)
{
    regmatch_T regmatch;
    CLEAR_ਖੇਤਰ(regmatch);
    regmatch.rm_ic = TRUE;
    regmatch.regprog = vim_regcomp((char_u *)"^\033[\\d\\+\\~$", RE_MAGIC);

    ਲਈ (ਪੂਰਨ i = 0; i < tc_len; ++i)
    {
	ਜੇ (regmatch.regprog == ਨਿਰਾ)
	    ਵਾਪਸ;

	// skip PasteStart and PasteEnd
	ਜੇ (termcodes[i].name[0] == 'P'
		&& (termcodes[i].name[1] == 'S' || termcodes[i].name[1] == 'E'))
	    ਜਾਰੀ;

	char_u *s = termcodes[i].ਕੋਡ;
	ਜੇ (s != ਨਿਰਾ && vim_regexec(&regmatch, s, (colnr_T)0))
	{
	    size_t len = STRLEN(s);
	    char_u *ns = alloc(len + 3);
	    ਜੇ (ns != ਨਿਰਾ)
	    {
		mch_memmove(ns, s, len - 1);
		mch_memmove(ns + len - 1, ";*~", 4);
		vim_free(s);
		termcodes[i].ਕੋਡ = ns;
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
    ਸਥਿਰ ਪੂਰਨ
termcode_ਤਾਰਾ(char_u *ਕੋਡ, ਪੂਰਨ len)
{
    // Shortest is <M-O>*X.  With ; shortest is <CSI>@;*X
    ਜੇ (len >= 3 && ਕੋਡ[len - 2] == '*')
    {
	ਜੇ (len >= 5 && ਕੋਡ[len - 3] == ';')
	    ਵਾਪਸ 2;
	ਨਹੀਂਤਾਂ
	    ਵਾਪਸ 1;
    }
    ਵਾਪਸ 0;
}

    char_u  *
ਲੱਭੋ_termcode(char_u *name)
{
    ਪੂਰਨ	    i;

    ਲਈ (i = 0; i < tc_len; ++i)
	ਜੇ (termcodes[i].name[0] == name[0] && termcodes[i].name[1] == name[1])
	    ਵਾਪਸ termcodes[i].ਕੋਡ;
    ਵਾਪਸ ਨਿਰਾ;
}

    char_u *
get_termcode(ਪੂਰਨ i)
{
    ਜੇ (i >= tc_len)
	ਵਾਪਸ ਨਿਰਾ;
    ਵਾਪਸ &termcodes[i].name[0];
}

/*
 * Returns the length of the terminal code at index 'idx'.
 */
    ਪੂਰਨ
get_termcode_len(ਪੂਰਨ idx)
{
    ਵਾਪਸ termcodes[idx].len;
}

    ਖਾਲੀ
del_termcode(char_u *name)
{
    ਪੂਰਨ	    i;

    ਜੇ (termcodes == ਨਿਰਾ)	// nothing there yet
	ਵਾਪਸ;

    need_gather = TRUE;		// need to fill termleader[]

    ਲਈ (i = 0; i < tc_len; ++i)
	ਜੇ (termcodes[i].name[0] == name[0] && termcodes[i].name[1] == name[1])
	{
	    del_termcode_idx(i);
	    ਵਾਪਸ;
	}
    // not found. Give error message?
}

    ਸਥਿਰ ਖਾਲੀ
del_termcode_idx(ਪੂਰਨ idx)
{
    ਪੂਰਨ		i;

    vim_free(termcodes[idx].ਕੋਡ);
    --tc_len;
    ਲਈ (i = idx; i < tc_len; ++i)
	termcodes[i] = termcodes[i + 1];
}

/*
 * Called when detected that the terminal sends 8-bit codes.
 * Convert all 7-bit codes to their 8-bit equivalent.
 */
    ਸਥਿਰ ਖਾਲੀ
switch_ਮੰਜ਼ਿਲ_8bit(ਖਾਲੀ)
{
    ਪੂਰਨ		i;
    ਪੂਰਨ		c;

    // Only need to do something when not already using 8-bit codes.
    ਜੇ (!ਪਦ_is_8bit(T_NAME))
    {
	ਲਈ (i = 0; i < tc_len; ++i)
	{
	    c = ਪਦ_7to8bit(termcodes[i].ਕੋਡ);
	    ਜੇ (c != 0)
	    {
		STRMOVE(termcodes[i].ਕੋਡ + 1, termcodes[i].ਕੋਡ + 2);
		termcodes[i].ਕੋਡ[0] = c;
	    }
	}
	need_gather = TRUE;		// need to fill termleader[]
    }
    detected_8bit = TRUE;
    LOG_TR1("Switching to 8 bit");
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_DOUBLE_CLICK
ਸਥਿਰ linenr_T orig_topline = 0;
# ifdef FEAT_DIFF
ਸਥਿਰ ਪੂਰਨ orig_topfill = 0;
# endif
#ਜੇ_ਅੰਤ
#ਜੇ defined(CHECK_DOUBLE_CLICK)
/*
 * Checking for double-clicks ourselves.
 * "orig_topline" is used to avoid detecting a double-click when the window
 * contents scrolled (e.g., when 'scrolloff' is non-zero).
 */
/*
 * Set orig_topline.  Used when jumping to another window, so that a double
 * click still works.
 */
    ਖਾਲੀ
ਸੈੱਟ_mouse_topline(win_T *wp)
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
    ਪੂਰਨ
is_mouse_topline(win_T *wp)
{
    ਵਾਪਸ orig_topline == wp->w_topline
# ifdef FEAT_DIFF
	&& orig_topfill == wp->w_topfill
# endif
	;
}
#ਜੇ_ਅੰਤ

/*
 * If "buf" is NULL put "string[new_slen]" in typebuf; "buflen" is not used.
 * If "buf" is not NULL put "string[new_slen]" in "buf[bufsize]" and adjust
 * "buflen".
 * Remove "slen" bytes.
 * Returns FAIL for error.
 */
    ਪੂਰਨ
put_string_ਵਿੱਚ_typebuf(
	ਪੂਰਨ	ਵਿੱਥ,
	ਪੂਰਨ	slen,
	char_u	*string,
	ਪੂਰਨ	new_slen,
	char_u	*buf,
	ਪੂਰਨ	bufsize,
	ਪੂਰਨ	*buflen)
{
    ਪੂਰਨ		extra = new_slen - slen;

    string[new_slen] = NUL;
    ਜੇ (buf == ਨਿਰਾ)
    {
	ਜੇ (extra < 0)
	    // remove matched chars, taking care of noremap
	    del_typebuf(-extra, ਵਿੱਥ);
	ਨਹੀਂਤਾਂ ਜੇ (extra > 0)
	    // insert the extra space we need
	    ਜੇ (ins_typebuf(string + slen, REMAP_YES, ਵਿੱਥ, FALSE, FALSE)
								       == FAIL)
		ਵਾਪਸ FAIL;

	// Careful: del_typebuf() and ins_typebuf() may have reallocated
	// typebuf.tb_buf[]!
	mch_memmove(typebuf.tb_buf + typebuf.tb_off + ਵਿੱਥ, string,
							     (size_t)new_slen);
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (extra < 0)
	    // remove matched characters
	    mch_memmove(buf + ਵਿੱਥ, buf + ਵਿੱਥ - extra,
					   (size_t)(*buflen + ਵਿੱਥ + extra));
	ਨਹੀਂਤਾਂ ਜੇ (extra > 0)
	{
	    // Insert the extra space we need.  If there is insufficient
	    // space return -1.
	    ਜੇ (*buflen + extra + new_slen >= bufsize)
		ਵਾਪਸ FAIL;
	    mch_memmove(buf + ਵਿੱਥ + extra, buf + ਵਿੱਥ,
						   (size_t)(*buflen - ਵਿੱਥ));
	}
	mch_memmove(buf + ਵਿੱਥ, string, (size_t)new_slen);
	*buflen = *buflen + extra + new_slen;
    }
    ਵਾਪਸ OK;
}

/*
 * Decode a modifier number as xterm provides it into MOD_MASK bits.
 */
    ਪੂਰਨ
decode_modifiers(ਪੂਰਨ n)
{
    ਪੂਰਨ	    ਕੋਡ = n - 1;
    ਪੂਰਨ	    modifiers = 0;

    ਜੇ (ਕੋਡ & 1)
	modifiers |= MOD_ਨਕਾਬ_ਖਿਸਕਾਓ;
    ਜੇ (ਕੋਡ & 2)
	modifiers |= MOD_ਨਕਾਬ_ALT;
    ਜੇ (ਕੋਡ & 4)
	modifiers |= MOD_ਨਕਾਬ_CTRL;
    ਜੇ (ਕੋਡ & 8)
	modifiers |= MOD_ਨਕਾਬ_META;
    // Any further modifiers are silently dropped.

    ਵਾਪਸ modifiers;
}

    ਸਥਿਰ ਪੂਰਨ
modifiers2keycode(ਪੂਰਨ modifiers, ਪੂਰਨ *ਕੁੰਜੀ, char_u *string)
{
    ਪੂਰਨ new_slen = 0;

    ਜੇ (modifiers == 0)
	ਵਾਪਸ 0;

    // Some keys have the modifier included.  Need to handle that here to
    // make mappings work.  This may result in a special key, such as
    // K_S_TAB.
    *ਕੁੰਜੀ = simplify_ਕੁੰਜੀ(*ਕੁੰਜੀ, &modifiers);
    ਜੇ (modifiers != 0)
    {
	string[new_slen++] = K_SPECIAL;
	string[new_slen++] = (ਪੂਰਨ)KS_MODIFIER;
	string[new_slen++] = modifiers;
    }
    ਵਾਪਸ new_slen;
}

/*
 * Handle a cursor position report.
 */
    ਸਥਿਰ ਖਾਲੀ
handle_u7_response(ਪੂਰਨ *arg, char_u *tp ਅਣਵਰਤਿਆ, ਪੂਰਨ csi_len ਅਣਵਰਤਿਆ)
{
    ਜੇ (arg[0] == 2 && arg[1] >= 2)
    {
	ਅੱਖਰ *aw = ਨਿਰਾ;

	LOG_TRN("Received U7 status: %s", tp);
	u7_ਸਥਿਤੀ.tr_ਤਰੱਕੀ = ਸਥਿਤੀ_GOT;
	did_cursorhold = TRUE;
	ਜੇ (arg[1] == 2)
	    aw = "single";
	ਨਹੀਂਤਾਂ ਜੇ (arg[1] == 3)
	    aw = "double";
	ਜੇ (aw != ਨਿਰਾ && STRCMP(aw, p_ambw) != 0)
	{
	    // Setting the option causes a screen redraw. Do
	    // that right away if possible, keeping any
	    // messages.
	    ਸੈੱਟ_option_ਮੁੱਲ_give_err((char_u *)"ambw", 0L, (char_u *)aw, 0);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG_TERMRESPONSE
	    {
		ਪੂਰਨ r = redraw_asap(UPD_CLEAR);

		LOG_TRN("set 'ambiwidth', redraw_asap(): %d", r);
	    }
#ਨਹੀਂਤਾਂ
	    redraw_asap(UPD_CLEAR);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	    ਸੈੱਟ_vim_var_string(VV_TERMU7RESP, tp, csi_len);
#ਜੇ_ਅੰਤ
	    apply_autocmds(EVENT_TERMRESPONSEALL,
					(char_u *)"ambiguouswidth", ਨਿਰਾ, FALSE, curbuf);
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (arg[0] == 3)
    {
	ਪੂਰਨ ਮੁੱਲ;

	LOG_TRN("Received compatibility test result: %s", tp);
	xcc_ਸਥਿਤੀ.tr_ਤਰੱਕੀ = ਸਥਿਤੀ_GOT;

	// Third row: xterm compatibility test.
	// If the cursor is on the first column then the terminal can handle
	// the request for cursor style and blinking.
	ਮੁੱਲ = arg[1] == 1 ? TPR_YES : TPR_NO;
	ਪਦ_props[TPR_CURSOR_STYLE].tpr_ਸਥਿਤੀ = ਮੁੱਲ;
	ਪਦ_props[TPR_CURSOR_BLINK].tpr_ਸਥਿਤੀ = ਮੁੱਲ;
    }
}



/*
 * Handle a response to T_CRV: {lead}{first}{x};{vers};{y}c
 * Xterm and alike use '>' for {first}.
 * Rxvt sends "{lead}?1;2c".
 */
    ਸਥਿਰ ਖਾਲੀ
handle_ਸੰਸਕਰਣ_response(ਪੂਰਨ ਪਹਿਲਾ, ਪੂਰਨ *arg, ਪੂਰਨ argc, char_u *tp)
{
    // The xterm version.  It is set to zero when it can't be an actual xterm
    // version.
    ਪੂਰਨ ਸੰਸਕਰਣ_2 = arg[1];

    LOG_TRN("Received CRV response: %s", tp);
    crv_ਸਥਿਤੀ.tr_ਤਰੱਕੀ = ਸਥਿਤੀ_GOT;
    did_cursorhold = TRUE;

    // Reset terminal properties that are set based on the termresponse.
    // Mainly useful for tests that send the termresponse multiple times.
    // For testing all props can be reset.
    init_ਪਦ_props(
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	    reset_ਪਦ_props_on_termresponse
#ਨਹੀਂਤਾਂ
	    FALSE
#ਜੇ_ਅੰਤ
	    );

    // If this code starts with CSI, you can bet that the
    // terminal uses 8-bit codes.
    ਜੇ (tp[0] == CSI)
	switch_ਮੰਜ਼ਿਲ_8bit();

    // Screen sends 40500.
    // rxvt sends its version number: "20703" is 2.7.3.
    // Ignore it for when the user has set 'term' to xterm,
    // even though it's an rxvt.
    ਜੇ (ਸੰਸਕਰਣ_2 > 20000)
	ਸੰਸਕਰਣ_2 = 0;

    // Figure out more if the response is CSI > 99 ; 99 ; 99 c
    ਜੇ (ਪਹਿਲਾ == '>' && argc == 3)
    {
	// mintty 2.9.5 sends 77;20905;0c.
	// (77 is ASCII 'M' for mintty.)
	ਜੇ (arg[0] == 77)
	{
	    // mintty can do SGR mouse reporting
	    ਪਦ_props[TPR_MOUSE].tpr_ਸਥਿਤੀ = TPR_MOUSE_SGR;
	}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMRESPONSE
	// If xterm version >= 141 try to get termcap codes.  For other
	// terminals the request should be ignored.
	ਜੇ (ਸੰਸਕਰਣ_2 >= 141 && p_xtermcodes)
	{
	    LOG_TR1("Enable checking for XT codes");
	    check_for_codes = TRUE;
	    need_gather = TRUE;
	    req_codes_from_ਪਦ();
	}
#ਜੇ_ਅੰਤ

	// libvterm sends 0;100;0
	// Konsole sends 0;115;0 and works the same way
	ਜੇ ((ਸੰਸਕਰਣ_2 == 100 || ਸੰਸਕਰਣ_2 == 115) && arg[0] == 0 && arg[2] == 0)
	{
	    // If run from Vim $COLORS is set to the number of
	    // colors the terminal supports.  Otherwise assume
	    // 256, libvterm supports even more.
	    ਜੇ (mch_getenv((char_u *)"COLORS") == ਨਿਰਾ)
		may_adjust_color_ਗਿਣਤੀ(256);
	    // Libvterm can handle SGR mouse reporting.
	    ਪਦ_props[TPR_MOUSE].tpr_ਸਥਿਤੀ = TPR_MOUSE_SGR;
	}

	ਜੇ (ਸੰਸਕਰਣ_2 == 95)
	{
	    // Mac Terminal.app sends 1;95;0
	    //
	    // Terminal.app doesn't seem to handle DECRQM sequences
	    // properly, see issue #19852.
	    ਜੇ (arg[0] == 1 && arg[2] == 0)
	    {
		ਪਦ_props[TPR_UNDERLINE_RGB].tpr_ਸਥਿਤੀ = TPR_YES;
		ਪਦ_props[TPR_MOUSE].tpr_ਸਥਿਤੀ = TPR_MOUSE_SGR;
		ਪਦ_props[TPR_DECRQM].tpr_ਸਥਿਤੀ = TPR_NO;
	    }
	    // iTerm2 sends 0;95;0
	    ਨਹੀਂਤਾਂ ਜੇ (arg[0] == 0 && arg[2] == 0)
	    {
		// iTerm2 can do SGR mouse reporting
		ਪਦ_props[TPR_MOUSE].tpr_ਸਥਿਤੀ = TPR_MOUSE_SGR;
		ਪਦ_props[TPR_DECRQM].tpr_ਸਥਿਤੀ = TPR_YES;
	    }
	    // old iTerm2 sends 0;95;
	    ਨਹੀਂਤਾਂ ਜੇ (arg[0] == 0 && arg[2] == -1)
		ਪਦ_props[TPR_UNDERLINE_RGB].tpr_ਸਥਿਤੀ = TPR_YES;
	}

	// screen sends 83;40500;0 83 is 'S' in ASCII.
	ਜੇ (arg[0] == 83)
	{
	    // screen supports SGR mouse codes since 4.7.0
	    ਜੇ (arg[1] >= 40700)
		ਪਦ_props[TPR_MOUSE].tpr_ਸਥਿਤੀ = TPR_MOUSE_SGR;
	    ਨਹੀਂਤਾਂ
		ਪਦ_props[TPR_MOUSE].tpr_ਸਥਿਤੀ = TPR_MOUSE_XTERM;
	}

	// If no recognized terminal has set mouse behavior, assume xterm.
	ਜੇ (ਪਦ_props[TPR_MOUSE].tpr_ਸਥਿਤੀ == TPR_UNKNOWN)
	{
	    // Xterm version 277 supports SGR.
	    // Xterm version >= 95 supports mouse dragging.
	    ਜੇ (ਸੰਸਕਰਣ_2 >= 277)
		ਪਦ_props[TPR_MOUSE].tpr_ਸਥਿਤੀ = TPR_MOUSE_SGR;
	    ਨਹੀਂਤਾਂ ਜੇ (ਸੰਸਕਰਣ_2 >= 95)
		ਪਦ_props[TPR_MOUSE].tpr_ਸਥਿਤੀ = TPR_MOUSE_XTERM2;
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
	ਜੇ (arg[1] >= 2500)
	{
	    ਪਦ_props[TPR_UNDERLINE_RGB].tpr_ਸਥਿਤੀ = TPR_YES;
	}

	ਨਹੀਂਤਾਂ ਜੇ (ਸੰਸਕਰਣ_2 == 136 && arg[2] == 0)
	{
	    ਪਦ_props[TPR_UNDERLINE_RGB].tpr_ਸਥਿਤੀ = TPR_YES;

	    // PuTTY sends 0;136;0
	    ਜੇ (arg[0] == 0)
	    {
		// supports sgr-like mouse reporting.
		ਪਦ_props[TPR_MOUSE].tpr_ਸਥਿਤੀ = TPR_MOUSE_SGR;
	    }
	    // vandyke SecureCRT sends 1;136;0
	}

	// Konsole sends 0;115;0 - but t_u8 does not actually work, therefore
	// commented out.
	// else if (version == 115 && arg[0] == 0 && arg[2] == 0)
	//     term_props[TPR_UNDERLINE_RGB].tpr_status = TPR_YES;

	// Kitty up to 9.x sends 1;400{version};{secondary-version}
	ਜੇ (arg[0] == 1 && arg[1] >= 4000 && arg[1] <= 4009)
	{
	    ਪਦ_props[TPR_KITTY].tpr_ਸਥਿਤੀ = TPR_YES;
	    ਪਦ_props[TPR_KITTY].tpr_ਸੈੱਟ_by_termresponse = TRUE;

	    // Kitty can handle SGR mouse reporting.
	    ਪਦ_props[TPR_MOUSE].tpr_ਸਥਿਤੀ = TPR_MOUSE_SGR;
	    ਪਦ_props[TPR_DECRQM].tpr_ਸਥਿਤੀ = TPR_YES;
	}

	// foot terminal sends 1;12700;0
	ਜੇ (arg[0] == 1 && ਸੰਸਕਰਣ_2 == 12700 && arg[2] == 0)
	{
	    ਪਦ_props[TPR_MOUSE].tpr_ਸਥਿਤੀ = TPR_MOUSE_SGR;
	    ਪਦ_props[TPR_DECRQM].tpr_ਸਥਿਤੀ = TPR_YES;
	}

	// GNU screen sends 83;30600;0, 83;40500;0, etc.
	// 30600/40500 is a version number of GNU screen. DA2 support is added
	// on 3.6.  DCS string has a special meaning to GNU screen, but xterm
	// compatibility checking does not detect GNU screen.
	ਜੇ (arg[0] == 83 && arg[1] >= 30600)
	{
	    ਪਦ_props[TPR_CURSOR_STYLE].tpr_ਸਥਿਤੀ = TPR_NO;
	    ਪਦ_props[TPR_CURSOR_BLINK].tpr_ਸਥਿਤੀ = TPR_NO;
	    ਪਦ_props[TPR_DECRQM].tpr_ਸਥਿਤੀ = TPR_NO; // screen doesn't seem
							// to handle DECRQM
							// sequences
	}

	// Xterm first responded to this request at patch level
	// 95, so assume anything below 95 is not xterm and hopefully supports
	// the underline RGB color sequence.
	ਜੇ (ਸੰਸਕਰਣ_2 < 95)
	    ਪਦ_props[TPR_UNDERLINE_RGB].tpr_ਸਥਿਤੀ = TPR_YES;

	// Getting the cursor style is only supported properly by xterm since
	// version 279 (otherwise it returns 0x18).
	ਜੇ (ਸੰਸਕਰਣ_2 < 279)
	    ਪਦ_props[TPR_CURSOR_STYLE].tpr_ਸਥਿਤੀ = TPR_NO;

	/*
	 * Take action on the detected properties.
	 */

	// Unless the underline RGB color is expected to work, disable "t_8u".
	// It does not work for the real Xterm, it resets the background color.
	// This may cause some flicker.  Alternative would be to set "t_8u"
	// here if the terminal is expected to support it, but that might
	// conflict with what was set in the .vimrc.
	ਜੇ (ਪਦ_props[TPR_UNDERLINE_RGB].tpr_ਸਥਿਤੀ != TPR_YES
			&& *T_8U != NUL
			&& !option_was_ਸੈੱਟ((char_u *)"t_8u"))
	{
	    ਸੈੱਟ_string_option_ਸਿੱਧਾ((char_u *)"t_8u", -1, (char_u *)"",
								  OPT_FREE, 0);
	}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMRESPONSE
	ਜੇ (*T_8U != NUL && write_t_8u_state == MAYBE)
	    // Did skip writing t_8u, a complete redraw is needed.
	    redraw_later_clear();
	write_t_8u_state = OK;  // can output t_8u now
#ਜੇ_ਅੰਤ

	// Only set 'ttymouse' automatically if it was not set
	// by the user already.
	ਜੇ (!option_was_ਸੈੱਟ((char_u *)"ttym")
		&& (ਪਦ_props[TPR_MOUSE].tpr_ਸਥਿਤੀ == TPR_MOUSE_XTERM2
		    || ਪਦ_props[TPR_MOUSE].tpr_ਸਥਿਤੀ == TPR_MOUSE_SGR))
	{
	    ਸੈੱਟ_option_ਮੁੱਲ_give_err((char_u *)"ttym", 0L,
		    ਪਦ_props[TPR_MOUSE].tpr_ਸਥਿਤੀ == TPR_MOUSE_SGR
				    ? (char_u *)"sgr" : (char_u *)"xterm2", 0);
	}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMRESPONSE
	ਪੂਰਨ need_flush = FALSE;

	// Only request the cursor style if t_SH and t_RS are
	// set. Only supported properly by xterm since version
	// 279 (otherwise it returns 0x18).
	// Only when getting the cursor style was detected to work.
	// Not for Terminal.app, it can't handle t_RS, it
	// echoes the characters to the screen.
	ਜੇ (rcs_ਸਥਿਤੀ.tr_ਤਰੱਕੀ == ਸਥਿਤੀ_GET
		&& ਪਦ_props[TPR_CURSOR_STYLE].tpr_ਸਥਿਤੀ == TPR_YES
		&& *T_CSH != NUL
		&& *T_CRS != NUL)
	{
	    MAY_WANT_ਮੰਜ਼ਿਲ_LOG_THIS;
	    LOG_TR1("Sending cursor style request");
	    ਬਾਹਰਲਾ_str(T_CRS);
	    termrequest_ਭੇਜਿਆ(&rcs_ਸਥਿਤੀ);
	    need_flush = TRUE;
	}

	// Only request the cursor blink mode if t_RC set. Not
	// for Gnome terminal, it can't handle t_RC, it
	// echoes the characters to the screen.
	// Only when getting the cursor style was detected to work.
	ਜੇ (rbm_ਸਥਿਤੀ.tr_ਤਰੱਕੀ == ਸਥਿਤੀ_GET
		&& ਪਦ_props[TPR_CURSOR_BLINK].tpr_ਸਥਿਤੀ == TPR_YES
		&& *T_ਚੱਕਰੀ_ਜਾਂਚ != NUL)
	{
	    MAY_WANT_ਮੰਜ਼ਿਲ_LOG_THIS;
	    LOG_TR1("Sending cursor blink mode request");
	    ਬਾਹਰਲਾ_str(T_ਚੱਕਰੀ_ਜਾਂਚ);
	    termrequest_ਭੇਜਿਆ(&rbm_ਸਥਿਤੀ);
	    need_flush = TRUE;
	}

	// Only request DEC modes via DECRQM when the terminal is known to
	// handle it.  Not for Apple Terminal.app or GNU screen, they echo
	// the trailing "p" to the screen.  See issue #19852.
	ਜੇ (decrqm_ਸਥਿਤੀ.tr_ਤਰੱਕੀ == ਸਥਿਤੀ_GET
		&& ਪਦ_props[TPR_DECRQM].tpr_ਸਥਿਤੀ == TPR_YES)
	{
	    MAY_WANT_ਮੰਜ਼ਿਲ_LOG_THIS;
	    LOG_TR1("Sending DECRQM requests");
	    ਲਈ (ਪੂਰਨ i = 0; i < (ਪੂਰਨ)ARRAY_ਲੰਮਾਈ(dec_modes); i++)
	    {
		vim_snprintf((ਅੱਖਰ *)IObuff, IOSIZE, "\033[?%d$p", dec_modes[i]);
		ਬਾਹਰਲਾ_str(IObuff);
	    }
	    termrequest_ਭੇਜਿਆ(&decrqm_ਸਥਿਤੀ);
	    need_flush = TRUE;

	}

	ਜੇ (need_flush)
	    ਬਾਹਰਲਾ_flush();
#ਜੇ_ਅੰਤ
    }
}

/*
 * Add "key" to "buf" and return the number of bytes used.
 * Handles special keys and multi-byte characters.
 */
    ਸਥਿਰ ਪੂਰਨ
ਜੋੜੋ_ਕੁੰਜੀ_ਮੰਜ਼ਿਲ_buf(ਪੂਰਨ ਕੁੰਜੀ, char_u *buf)
{
    ਪੂਰਨ idx = 0;

    ਜੇ (IS_SPECIAL(ਕੁੰਜੀ))
    {
	buf[idx++] = K_SPECIAL;
	buf[idx++] = KEY2TERMCAP0(ਕੁੰਜੀ);
	buf[idx++] = KEY2TERMCAP1(ਕੁੰਜੀ);
    }
    ਨਹੀਂਤਾਂ ਜੇ (has_mbyte)
	idx += (*mb_char2bytes)(ਕੁੰਜੀ, buf + idx);
    ਨਹੀਂਤਾਂ
	buf[idx++] = ਕੁੰਜੀ;
    ਵਾਪਸ idx;
}

/*
 * Shared between handle_key_with_modifier() and handle_csi_function_key().
 */
    ਸਥਿਰ ਪੂਰਨ
put_ਕੁੰਜੀ_modifiers_ਵਿੱਚ_typebuf(
	ਪੂਰਨ	ਕੁੰਜੀ_arg,
	ਪੂਰਨ	modifiers_arg,
	ਪੂਰਨ	csi_len,
	ਪੂਰਨ	ਵਿੱਥ,
	char_u	*buf,
	ਪੂਰਨ	bufsize,
	ਪੂਰਨ	*buflen)
{
    ਪੂਰਨ ਕੁੰਜੀ = ਕੁੰਜੀ_arg;
    ਪੂਰਨ modifiers = modifiers_arg;

    // Some keys need adjustment when the Ctrl modifier is used.
    ਕੁੰਜੀ = may_adjust_ਕੁੰਜੀ_for_ctrl(modifiers, ਕੁੰਜੀ);

    // May remove the shift modifier if it's already included in the key.
    modifiers = may_remove_ਖਿਸਕਾਓ_modifier(modifiers, ਕੁੰਜੀ);

    // Produce modifiers with K_SPECIAL KS_MODIFIER {mod}
    // worst-case: 3-byte modifier + 4 byte multi-char key + NUL
    char_u string[MAX_ਕੁੰਜੀ_ਕੋਡ_LEN + 2];
    ਪੂਰਨ new_slen = modifiers2keycode(modifiers, &ਕੁੰਜੀ, string);

    // reject overlong key that would overflow string
    ਜੇ (ਕੁੰਜੀ > 0x10FFFF)
	ਵਾਪਸ -1;

    // Add the bytes for the key.
    new_slen += ਜੋੜੋ_ਕੁੰਜੀ_ਮੰਜ਼ਿਲ_buf(ਕੁੰਜੀ, string + new_slen);

    string[new_slen] = NUL;
    ਜੇ (put_string_ਵਿੱਚ_typebuf(ਵਿੱਥ, csi_len, string, new_slen,
						 buf, bufsize, buflen) == FAIL)
	ਵਾਪਸ -1;
    ਵਾਪਸ new_slen - csi_len + ਵਿੱਥ;
}

/*
 * Parse the number from a CSI numbered sequence for an F1-F12 key:
 *	ESC [ {number} ~
 * Returns the key
 */
    ਸਥਿਰ ਪੂਰਨ
parse_csi_f_ਕੁੰਜੀਆਂ(ਪੂਰਨ arg)
{
    char_u ਕੁੰਜੀ_name[2] = { 0, 0 };

    ਚੋਣ (arg)
    {
	ਹਾਲਤ 11: ਕੁੰਜੀ_name[0] = 'k'; ਕੁੰਜੀ_name[1] = '1'; ਰੋਕੋ;  // K_F1
	ਹਾਲਤ 12: ਕੁੰਜੀ_name[0] = 'k'; ਕੁੰਜੀ_name[1] = '2'; ਰੋਕੋ;  // K_F2
	ਹਾਲਤ 13: ਕੁੰਜੀ_name[0] = 'k'; ਕੁੰਜੀ_name[1] = '3'; ਰੋਕੋ;  // K_F3
	ਹਾਲਤ 14: ਕੁੰਜੀ_name[0] = 'k'; ਕੁੰਜੀ_name[1] = '4'; ਰੋਕੋ;  // K_F4
	ਹਾਲਤ 15: ਕੁੰਜੀ_name[0] = 'k'; ਕੁੰਜੀ_name[1] = '5'; ਰੋਕੋ;  // K_F5
	ਹਾਲਤ 17: ਕੁੰਜੀ_name[0] = 'k'; ਕੁੰਜੀ_name[1] = '6'; ਰੋਕੋ;  // K_F6
	ਹਾਲਤ 18: ਕੁੰਜੀ_name[0] = 'k'; ਕੁੰਜੀ_name[1] = '7'; ਰੋਕੋ;  // K_F7
	ਹਾਲਤ 19: ਕੁੰਜੀ_name[0] = 'k'; ਕੁੰਜੀ_name[1] = '8'; ਰੋਕੋ;  // K_F8
	ਹਾਲਤ 20: ਕੁੰਜੀ_name[0] = 'k'; ਕੁੰਜੀ_name[1] = '9'; ਰੋਕੋ;  // K_F9
	ਹਾਲਤ 21: ਕੁੰਜੀ_name[0] = 'F'; ਕੁੰਜੀ_name[1] = ';'; ਰੋਕੋ;  // K_F10
	ਹਾਲਤ 23: ਕੁੰਜੀ_name[0] = 'F'; ਕੁੰਜੀ_name[1] = '1'; ਰੋਕੋ;  // K_F11
	ਹਾਲਤ 24: ਕੁੰਜੀ_name[0] = 'F'; ਕੁੰਜੀ_name[1] = '2'; ਰੋਕੋ;  // K_F12
    }
    ਜੇ (ਕੁੰਜੀ_name[0])
	ਵਾਪਸ TERMCAP2_ਕੁੰਜੀ(ਕੁੰਜੀ_name[0], ਕੁੰਜੀ_name[1]);
    // shouldn't happen
    ਵਾਪਸ arg;
}

/*
 * Handle a sequence with key and modifier, one of:
 *	{lead}27;{modifier};{key}~
 *	{lead}{key};{modifier}u
 * Returns the difference in length.
 */
    ਸਥਿਰ ਪੂਰਨ
handle_ਕੁੰਜੀ_with_modifier(
	ਪੂਰਨ	*arg,
	ਪੂਰਨ	csi_len,
	ਪੂਰਨ	ਵਿੱਥ,
	char_u	*buf,
	ਪੂਰਨ	bufsize,
	ਪੂਰਨ	*buflen,
	ਪੂਰਨ	iskitty,
	ਪੂਰਨ	trail)
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
    ਜੇ (!iskitty
	    && (kitty_protocol_state == KKPS_INITIAL
		|| kitty_protocol_state == KKPS_OFF
		|| kitty_protocol_state == KKPS_AFTER_T_TE)
	    && ਪਦ_props[TPR_KITTY].tpr_ਸਥਿਤੀ != TPR_YES)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	ch_log(ਨਿਰਾ, "setting seenModifyOtherKeys to TRUE");
#ਜੇ_ਅੰਤ
	ਵੇਖਿਆ_Modify_Other_ਕੁੰਜੀਆਂ = TRUE;
    }

    ਪੂਰਨ ਕੁੰਜੀ = iskitty ? arg[0] : arg[2];
    ਪੂਰਨ modifiers = decode_modifiers(arg[1]);

    // Some terminals do not apply the Shift modifier to the key.  To make
    // mappings consistent we do it here.  TODO: support more keys.
    ਜੇ ((modifiers & MOD_ਨਕਾਬ_ਖਿਸਕਾਓ) && ਕੁੰਜੀ >= 'a' && ਕੁੰਜੀ <= 'z')
	ਕੁੰਜੀ += 'A' - 'a';

    // Putting Esc in the buffer creates ambiguity, it can be the start of an
    // escape sequence.  Use K_ESC to avoid that.
    ਜੇ (ਕੁੰਜੀ == ESC)
	ਕੁੰਜੀ = K_ESC;

    ਨਹੀਂਤਾਂ ਜੇ (arg[0] >= 11 && arg[0] <= 24 && trail == '~')
	ਕੁੰਜੀ = parse_csi_f_ਕੁੰਜੀਆਂ(arg[0]);

    ਵਾਪਸ put_ਕੁੰਜੀ_modifiers_ਵਿੱਚ_typebuf(ਕੁੰਜੀ, modifiers, csi_len, ਵਿੱਥ, buf,
	    bufsize, buflen);
}

/*
 * Handle a sequence with key without a modifier:
 *	{lead}{key}u
 *	{lead}{key}~
 * Returns the difference in length.
 */
    ਸਥਿਰ ਪੂਰਨ
handle_ਕੁੰਜੀ_without_modifier(
	ਪੂਰਨ	*arg,
	ਪੂਰਨ	csi_len,
	ਪੂਰਨ	ਵਿੱਥ,
	char_u	*buf,
	ਪੂਰਨ	bufsize,
	ਪੂਰਨ	*buflen,
	ਪੂਰਨ	trail)
{
    char_u  string[MAX_ਕੁੰਜੀ_ਕੋਡ_LEN + 1];
    ਪੂਰਨ	    new_slen;

    ਜੇ (arg[0] == ESC)
    {
	// Putting Esc in the buffer creates ambiguity, it can be the start of
	// an escape sequence.  Use K_ESC to avoid that.
	string[0] = K_SPECIAL;
	string[1] = KS_EXTRA;
	string[2] = KE_ESC;
	new_slen = 3;
    }
    ਨਹੀਂਤਾਂ ਜੇ (arg[0] >= 11 && arg[0] <= 24 && trail == '~')
    {
	ਪੂਰਨ ਕੁੰਜੀ = parse_csi_f_ਕੁੰਜੀਆਂ(arg[0]);
	string[0] = K_SPECIAL;
	string[1] = KEY2TERMCAP0(ਕੁੰਜੀ);
	string[2] = KEY2TERMCAP1(ਕੁੰਜੀ);
	new_slen = 3;
    }
    ਨਹੀਂਤਾਂ
	new_slen = ਜੋੜੋ_ਕੁੰਜੀ_ਮੰਜ਼ਿਲ_buf(arg[0], string);

    ਜੇ (put_string_ਵਿੱਚ_typebuf(ਵਿੱਥ, csi_len, string, new_slen,
						 buf, bufsize, buflen) == FAIL)
	ਵਾਪਸ -1;
    ਵਾਪਸ new_slen - csi_len + ਵਿੱਥ;
}

/*
 * CSI function key without or with modifiers:
 *	{lead}[ABCDEFHPQRS]
 *	{lead}1;{modifier}[ABCDEFHPQRS]
 * Returns zero when nog recognized, a positive number when recognized.
 */
    ਸਥਿਰ ਪੂਰਨ
handle_csi_function_ਕੁੰਜੀ(
	ਪੂਰਨ	argc,
	ਪੂਰਨ	*arg,
	ਪੂਰਨ	trail,
	ਪੂਰਨ	csi_len,
	char_u	*ਕੁੰਜੀ_name,
	ਪੂਰਨ	ਵਿੱਥ,
	char_u  *buf,
	ਪੂਰਨ	bufsize,
	ਪੂਰਨ	*buflen)
{
    ਕੁੰਜੀ_name[0] = 'k';
    ਚੋਣ (trail)
    {
	ਹਾਲਤ 'A': ਕੁੰਜੀ_name[1] = 'u'; ਰੋਕੋ;  // K_UP
	ਹਾਲਤ 'B': ਕੁੰਜੀ_name[1] = 'd'; ਰੋਕੋ;  // K_DOWN
	ਹਾਲਤ 'C': ਕੁੰਜੀ_name[1] = 'r'; ਰੋਕੋ;  // K_RIGHT
	ਹਾਲਤ 'D': ਕੁੰਜੀ_name[1] = 'l'; ਰੋਕੋ;  // K_LEFT

	// case 'E': keypad BEGIN - not supported
	ਹਾਲਤ 'F': ਕੁੰਜੀ_name[0] = '@'; ਕੁੰਜੀ_name[1] = '7'; ਰੋਕੋ;  // K_END
	ਹਾਲਤ 'H': ਕੁੰਜੀ_name[1] = 'h'; ਰੋਕੋ;  // K_HOME

	ਹਾਲਤ 'P': ਕੁੰਜੀ_name[1] = '1'; ਰੋਕੋ;  // K_F1
	ਹਾਲਤ 'Q': ਕੁੰਜੀ_name[1] = '2'; ਰੋਕੋ;  // K_F2
	ਹਾਲਤ 'R': ਕੁੰਜੀ_name[1] = '3'; ਰੋਕੋ;  // K_F3
	ਹਾਲਤ 'S': ਕੁੰਜੀ_name[1] = '4'; ਰੋਕੋ;  // K_F4

	ਮੂਲ: ਵਾਪਸ 0;  // not recognized
    }

    ਪੂਰਨ ਕੁੰਜੀ = TERMCAP2_ਕੁੰਜੀ(ਕੁੰਜੀ_name[0], ਕੁੰਜੀ_name[1]);
    ਪੂਰਨ modifiers = argc == 2 ? decode_modifiers(arg[1]) : 0;
    put_ਕੁੰਜੀ_modifiers_ਵਿੱਚ_typebuf(ਕੁੰਜੀ, modifiers,
					csi_len, ਵਿੱਥ, buf, bufsize, buflen);
    ਵਾਪਸ csi_len;
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
    ਸਥਿਰ ਪੂਰਨ
handle_csi(
	char_u	*tp,
	ਪੂਰਨ	len,
	char_u	*argp,
	ਪੂਰਨ	ਵਿੱਥ,
	char_u  *buf,
	ਪੂਰਨ	bufsize,
	ਪੂਰਨ	*buflen,
	char_u	*ਕੁੰਜੀ_name,
	ਪੂਰਨ	*slen)
{
    ਪੂਰਨ		ਪਹਿਲਾ = -1;  // optional char right after {lead}
    ਪੂਰਨ		trail;	     // char that ends CSI sequence
    ਪੂਰਨ		arg[3] = {-1, -1, -1};	// argument numbers
    ਪੂਰਨ		argc = 0;		// number of arguments
    char_u	*ap = argp;
    ਪੂਰਨ		csi_len;

    // Check for non-digit after CSI.
    ਜੇ (!VIM_ISDIGIT(*ap))
	ਪਹਿਲਾ = *ap++;

    ਜੇ (ਪਹਿਲਾ >= 'A' && ਪਹਿਲਾ <= 'Z')
    {
	// If "first" is in [ABCDEFHPQRS] then it is actually the "trail" and
	// no argument follows.
	trail = ਪਹਿਲਾ;
	ਪਹਿਲਾ = -1;
	--ap;
    }
    ਨਹੀਂਤਾਂ
    {
	// Find up to three argument numbers.
	ਲਈ (argc = 0; argc < 3; )
	{
	    ਜੇ (ap >= tp + len)
		ਵਾਪਸ -1;
	    ਜੇ (*ap == ';')
		arg[argc++] = -1;  // omitted number
	    ਨਹੀਂਤਾਂ ਜੇ (VIM_ISDIGIT(*ap))
	    {
		arg[argc] = 0;
		ਲਈ (;;)
		{
		    ਜੇ (ap >= tp + len)
			ਵਾਪਸ -1;
		    ਜੇ (!VIM_ISDIGIT(*ap))
			ਰੋਕੋ;
		    // avoid overflow
		    ਜੇ (arg[argc] <= (ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ - 9) / 10)
			arg[argc] = arg[argc] * 10 + (*ap - '0');
		    ++ap;
		}
		++argc;
	    }
	    ਜੇ (*ap == ';')
		++ap;
	    ਨਹੀਂਤਾਂ
		ਰੋਕੋ;
	}

	// mrxvt has been reported to have "+" in the version. Assume
	// the escape sequence ends with a letter or one of "{|}~".
	ਜਦੋਂ (ap < tp + len
		&& !(*ap >= '{' && *ap <= '~')
		&& !ASCII_ISALPHA(*ap))
	    ++ap;
	ਜੇ (ap >= tp + len)
	    ਵਾਪਸ -1;
	trail = *ap;
    }

    csi_len = (ਪੂਰਨ)(ap - tp) + 1;

    // Response to XTQMODKEYS: "CSI > 4 ; Pv m" where Pv indicates the
    // modifyOtherKeys level.  Drop similar responses.
    ਜੇ (ਪਹਿਲਾ == '>' && (argc == 1 || argc == 2) && trail == 'm')
    {
	ਜੇ (arg[0] == 4 && argc == 2)
	    modify_otherkeys_state = arg[1] == 2 ? MOKS_ENABLED : MOKS_OFF;

	ਕੁੰਜੀ_name[0] = (ਪੂਰਨ)KS_EXTRA;
	ਕੁੰਜੀ_name[1] = (ਪੂਰਨ)KE_IGNORE;
	*slen = csi_len;
    }

    // Function key starting with CSI:
    //	{lead}[ABCDEFHPQRS]
    //	{lead}1;{modifier}[ABCDEFHPQRS]
    ਨਹੀਂਤਾਂ ਜੇ (ਪਹਿਲਾ == -1 && ASCII_ISUPPER(trail)
	    && (argc == 0 || (argc == 2 && arg[0] == 1)))
    {
	ਪੂਰਨ res = handle_csi_function_ਕੁੰਜੀ(argc, arg, trail,
			      csi_len, ਕੁੰਜੀ_name, ਵਿੱਥ, buf, bufsize, buflen);
	ਵਾਪਸ res <= 0 ? res : len + res;
    }

    // Cursor position report: {lead}{row};{col}R
    // Eat it when there are 2 arguments and it ends in 'R'.
    // Also when u7_status is not "sent", it may be from a previous Vim that
    // just exited.  But not for <S-F3>, it sends something similar, check for
    // row and column to make sense.
    ਨਹੀਂਤਾਂ ਜੇ (ਪਹਿਲਾ == -1 && argc == 2 && trail == 'R')
    {
	handle_u7_response(arg, tp, csi_len);

	ਕੁੰਜੀ_name[0] = (ਪੂਰਨ)KS_EXTRA;
	ਕੁੰਜੀ_name[1] = (ਪੂਰਨ)KE_IGNORE;
	*slen = csi_len;
    }

#ਜੇ (defined(UNIX) || defined(MSWIN)) \
    && (defined(FEAT_IMAGE) || defined(FEAT_EVAL))
    // Response to CSI 14 t or CSI 16 t
    ਨਹੀਂਤਾਂ ਜੇ (ਪਹਿਲਾ == -1 && argc >= 3
	    && (arg[0] == 4 || arg[0] == 6) && trail == 't')
    {
	ਜੇ (arg[0] == 4)
	{
	    LOG_TRN("Received CSI 14 t response: %s", tp);
	    cell_ਚੌੜਾਈ = arg[2] / Columns;
	    cell_ਉਚਾਈ = arg[1] / ਪੰਕਤੀਆਂ;
	}
	ਨਹੀਂਤਾਂ
	{
	    LOG_TRN("Received CSI 16 t response: %s", tp);
	    cell_ਚੌੜਾਈ = arg[2];
	    cell_ਉਚਾਈ = arg[1];
	}

	ਕੁੰਜੀ_name[0] = (ਪੂਰਨ)KS_EXTRA;
	ਕੁੰਜੀ_name[1] = (ਪੂਰਨ)KE_IGNORE;
	*slen = csi_len;
    }
#ਜੇ_ਅੰਤ

    // Primary device attributes (DA1) response
    ਨਹੀਂਤਾਂ ਜੇ (ਪਹਿਲਾ == '?' && trail == 'c')
    {
	LOG_TRN("Received DA1 response: %s", tp);

	*slen = csi_len;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	ਸੈੱਟ_vim_var_string(VV_TERMDA1, tp, *slen);
#ਜੇ_ਅੰਤ
	apply_autocmds(EVENT_TERMRESPONSEALL,
					(char_u *)"da1", ਨਿਰਾ, FALSE, curbuf);

	ਕੁੰਜੀ_name[0] = (ਪੂਰਨ)KS_EXTRA;
	ਕੁੰਜੀ_name[1] = (ਪੂਰਨ)KE_IGNORE;
	*slen = csi_len;
    }

    // DECRPM mode 2026 or 2048.
    ਨਹੀਂਤਾਂ ਜੇ (ਪਹਿਲਾ == '?' && trail == 'y' && argc == 2
	    && (arg[0] == 2026 || arg[0] == 2048))
    {
	ਪੂਰਨ setting = arg[1];

	*slen = csi_len;
	ਕੁੰਜੀ_name[0] = (ਪੂਰਨ)KS_EXTRA;
	ਕੁੰਜੀ_name[1] = (ਪੂਰਨ)KE_IGNORE;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMRESPONSE
	// Mark the DECRQM request as answered so it is not sent again and
	// stoptermcap() does not wait for it.
	ਜੇ (decrqm_ਸਥਿਤੀ.tr_ਤਰੱਕੀ == ਸਥਿਤੀ_ਭੇਜਿਆ)
	    decrqm_ਸਥਿਤੀ.tr_ਤਰੱਕੀ = ਸਥਿਤੀ_GOT;
#ਜੇ_ਅੰਤ

	ਜੇ (setting >= 0 && setting <= 4)
	{
	    LOG_TRN("Received DECRPM mode %d: %s", arg[0], tp);

	    ਚੋਣ (arg[0])
	    {
		ਹਾਲਤ 2026:
		    sync_output_setting = setting;
		    ਸੈੱਟ_option_ਮੁੱਲ_give_err((char_u *)"termsync",
			    setting == 1 || setting == 2, ਨਿਰਾ, 0);
		    ਰੋਕੋ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
		ਹਾਲਤ 2048:
		    win_resize_setting = setting;

		    ਪਦ_ਸੈੱਟ_win_resize(true);
		    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
	    }
	}
	ਨਹੀਂਤਾਂ
	    LOG_TRN("Unknown DECRPM mode %d setting %d", arg[0], setting);
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
    // In-band window resize event
    ਨਹੀਂਤਾਂ ਜੇ (win_resize_enabled && argc >= 3 && arg[0] == 48)
    {
	ਪੂਰਨ ਉਚਾਈ = arg[1], ਚੌੜਾਈ = arg[2];

	*slen = csi_len;
	ਕੁੰਜੀ_name[0] = (ਪੂਰਨ)KS_EXTRA;
	ਕੁੰਜੀ_name[1] = (ਪੂਰਨ)KE_IGNORE;

	// Only update if needed. Avoids intro message from disappearing on
	// startup (due to initial event).
	ਜੇ (ਉਚਾਈ != ਪੰਕਤੀਆਂ || ਚੌੜਾਈ != Columns)
	    ਸੈੱਟ_shellsize(ਚੌੜਾਈ, ਉਚਾਈ, true);
    }
#ਜੇ_ਅੰਤ

    // Version string: Eat it when there is at least one digit and
    // it ends in 'c'
    ਨਹੀਂਤਾਂ ਜੇ (*T_CRV != NUL && ap > argp + 1 && trail == 'c')
    {
	handle_ਸੰਸਕਰਣ_response(ਪਹਿਲਾ, arg, argc, tp);

	*slen = csi_len;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	ਸੈੱਟ_vim_var_string(VV_TERMRESPONSE, tp, *slen);
#ਜੇ_ਅੰਤ
	apply_autocmds(EVENT_TERMRESPONSE,
					ਨਿਰਾ, ਨਿਰਾ, FALSE, curbuf);
	apply_autocmds(EVENT_TERMRESPONSEALL,
					(char_u *)"version", ਨਿਰਾ, FALSE, curbuf);
	ਕੁੰਜੀ_name[0] = (ਪੂਰਨ)KS_EXTRA;
	ਕੁੰਜੀ_name[1] = (ਪੂਰਨ)KE_IGNORE;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMRESPONSE
    // Check blinking cursor from xterm:
    // {lead}?12;1$y       set
    // {lead}?12;2$y       not set
    //
    // {lead} can be <Esc>[ or CSI
    ਨਹੀਂਤਾਂ ਜੇ (rbm_ਸਥਿਤੀ.tr_ਤਰੱਕੀ == ਸਥਿਤੀ_ਭੇਜਿਆ
	    && ਪਹਿਲਾ == '?'
	    && ap == argp + 6
	    && arg[0] == 12
	    && ap[-1] == '$'
	    && trail == 'y')
    {
	initial_cursor_blink = (arg[1] == '1');
	rbm_ਸਥਿਤੀ.tr_ਤਰੱਕੀ = ਸਥਿਤੀ_GOT;
	LOG_TRN("Received cursor blinking mode response: %s", tp);
	ਕੁੰਜੀ_name[0] = (ਪੂਰਨ)KS_EXTRA;
	ਕੁੰਜੀ_name[1] = (ਪੂਰਨ)KE_IGNORE;
	*slen = csi_len;
# ifdef FEAT_EVAL
	ਸੈੱਟ_vim_var_string(VV_TERMBLINKRESP, tp, *slen);
# endif
	apply_autocmds(EVENT_TERMRESPONSEALL,
					(char_u *)"cursorblink", ਨਿਰਾ, FALSE, curbuf);
    }
#ਜੇ_ਅੰਤ

    // Kitty keyboard protocol status response: CSI ? flags u
    ਨਹੀਂਤਾਂ ਜੇ (ਪਹਿਲਾ == '?' && argc == 1 && trail == 'u')
    {
	// The protocol has various "progressive enhancement flags" values, but
	// we only check for zero and non-zero here.
	ਜੇ (arg[0] == '0')
	{
	    kitty_protocol_state = KKPS_OFF;
	}
	ਨਹੀਂਤਾਂ
	{
	    kitty_protocol_state = KKPS_ENABLED;

	    // Reset seenModifyOtherKeys just in case some key combination has
	    // been seen that set it before we get the status response.
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	    ch_log(ਨਿਰਾ, "setting seenModifyOtherKeys to FALSE");
#ਜੇ_ਅੰਤ
	    ਵੇਖਿਆ_Modify_Other_ਕੁੰਜੀਆਂ = FALSE;
	}

	ਕੁੰਜੀ_name[0] = (ਪੂਰਨ)KS_EXTRA;
	ਕੁੰਜੀ_name[1] = (ਪੂਰਨ)KE_IGNORE;
	*slen = csi_len;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMRESPONSE
    // Check for a window position response from the terminal:
    //       {lead}3;{x};{y}t
    ਨਹੀਂਤਾਂ ਜੇ (did_request_winpos && argc == 3 && arg[0] == 3
						   && trail == 't')
    {
	winpos_x = arg[1];
	winpos_y = arg[2];
	// got finished code: consume it
	ਕੁੰਜੀ_name[0] = (ਪੂਰਨ)KS_EXTRA;
	ਕੁੰਜੀ_name[1] = (ਪੂਰਨ)KE_IGNORE;
	*slen = csi_len;

	ਜੇ (--did_request_winpos <= 0)
	    winpos_ਸਥਿਤੀ.tr_ਤਰੱਕੀ = ਸਥਿਤੀ_GOT;
    }
#ਜੇ_ਅੰਤ

    // Key with modifier:
    //	{lead}27;{modifier};{key}~
    //	{lead}{key};{modifier}u
    //	{lead}{key};{modifier}~
    // Even though we only handle four modifiers and the {modifier} value
    // should be 16 or lower, we accept all modifier values to avoid the raw
    // sequence to be passed through.
    ਨਹੀਂਤਾਂ ਜੇ ((arg[0] == 27 && argc == 3 && trail == '~')
		|| (argc == 2 && (trail == 'u' || trail == '~')))
    {
	ਪੂਰਨ iskitty = argc == 2 && (trail == 'u' || trail == '~');
	ਵਾਪਸ len + handle_ਕੁੰਜੀ_with_modifier(arg, csi_len, ਵਿੱਥ, buf,
		bufsize, buflen, iskitty, trail);
    }

    // Key without modifier (Kitty sends this for Esc or F3):
    //	{lead}{key}u
    //	{lead}{key}~
    ਨਹੀਂਤਾਂ ਜੇ (argc == 1 && (trail == 'u' || trail == '~'))
    {
	ਵਾਪਸ len + handle_ਕੁੰਜੀ_without_modifier(arg, csi_len, ਵਿੱਥ, buf,
		bufsize, buflen, trail);
    }

    // else: Unknown CSI sequence.  We could drop it, but then the
    // user can't create a map for it.
    ਵਾਪਸ 0;
}

ਸਥਿਰ ਖਾਲੀ
check_for_color_response(char_u *resp, ਪੂਰਨ len)
{
    ਪੂਰਨ		i, j;
    char_u	*argp;

    j = 1 + (resp[0] == ESC);
    argp = resp + j;

    ਜੇ (len >= j + 3 && (argp[0] != '1'
			     || (argp[1] != '1' && argp[1] != '0')
			     || argp[2] != ';'))
	i = 0; // no match
    ਨਹੀਂਤਾਂ
    {
	ਲਈ (i = j; i < len; ++i)
	    ਜੇ (resp[i] == '\007' || (resp[0] == OSC ? resp[i] == STERM
			: (resp[i] == ESC && i + 1 < len && resp[i + 1] == '\\')))
	    {
		ਪੂਰਨ is_bg = argp[1] == '1';
		ਪੂਰਨ is_4digit = i - j >= 21 && resp[j + 11] == '/'
						  && resp[j + 16] == '/';

		ਜੇ (i - j >= 15 && STRNCMP(resp + j + 3, "rgb:", 4) == 0
			    && (is_4digit
				   || (resp[j + 9] == '/' && resp[j + 12] == '/')))
		{
		    char_u *tp_r = resp + j + 7;
		    char_u *tp_g = resp + j + (is_4digit ? 12 : 10);
		    char_u *tp_b = resp + j + (is_4digit ? 17 : 13);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMRESPONSE
		    ਪੂਰਨ rval, gval, bval;

		    rval = hexhex2nr(tp_r);
		    gval = hexhex2nr(tp_g);
		    bval = hexhex2nr(tp_b);
#ਜੇ_ਅੰਤ
		    ਜੇ (is_bg)
		    {
			ਅੱਖਰ *new_bg_val = (3 * '6' < *tp_r + *tp_g +
					     *tp_b) ? "light" : "dark";

			LOG_TRN("Received RBG response: r=%d g=%d b=%d", rval, gval, bval);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMRESPONSE
			rbg_ਸਥਿਤੀ.tr_ਤਰੱਕੀ = ਸਥਿਤੀ_GOT;
			bg_r = rval;
			bg_g = gval;
			bg_b = bval;
#ਜੇ_ਅੰਤ
			ਜੇ (!option_was_ਸੈੱਟ((char_u *)"bg")
				      && STRCMP(p_bg, new_bg_val) != 0)
			{
			    // value differs, apply it
			    ਸੈੱਟ_option_ਮੁੱਲ_give_err((char_u *)"bg",
						  0L, (char_u *)new_bg_val, 0);
			    reset_option_was_ਸੈੱਟ((char_u *)"bg");
			    redraw_asap(UPD_CLEAR);
			}
		    }
#ਜੇ defined(FEAT_TERMRESPONSE) && defined(FEAT_TERMINAL)
		    ਨਹੀਂਤਾਂ
		    {
			LOG_TRN("Received RFG response: r=%d g=%d b=%d", rval, gval, bval);
			rfg_ਸਥਿਤੀ.tr_ਤਰੱਕੀ = ਸਥਿਤੀ_GOT;
			fg_r = rval;
			fg_g = gval;
			fg_b = bval;
		    }
#ਜੇ_ਅੰਤ
		}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
		ਸੈੱਟ_vim_var_string(is_bg ? VV_TERMRBGRESP
						  : VV_TERMRFGRESP, resp, len);
#ਜੇ_ਅੰਤ
		apply_autocmds(EVENT_TERMRESPONSEALL,
			is_bg ? (char_u *)"background" : (char_u *)"foreground",
			ਨਿਰਾ, FALSE, curbuf);
		ਰੋਕੋ;
	    }
    }
    ਜੇ (i == len)
	LOG_TR1("not enough characters for RB");
}

ਸਥਿਰ oscstate_T osc_state;

/*
 * Return true if currently receiving an OSC response.
 */
    bool
ਵਿੱਚ_osc_sequence(ਖਾਲੀ)
{
    ਵਾਪਸ osc_state.processing;
}

/*
 * Handles any OSC sequence and places the result in "v:termosc". Returns OK on
 * success and FAIL on failure.
 */
    ਸਥਿਰ ਪੂਰਨ
handle_osc(char_u *tp, ਪੂਰਨ len, char_u *ਕੁੰਜੀ_name, ਪੂਰਨ *slen)
{
    char_u ਆਖਰੀ_char;

    ਜੇ (!osc_state.processing)
    {
	ਪੂਰਨ cur;

	LOG_TRN("Received OSC response: %s", (ਅੱਖਰ*)tp);
	// Check if it is a valid OSC sequence, and consume it. OSC format
	// consists of:
	// <identifier><data><terminator>
	// <identifier> is either <Esc>] or an OSC character
	// <terminator> can be '\007', <Esc>\ or STERM.
	cur = 1 + (tp[0] == ESC);

	ਜੇ (len < cur + 1 + (tp[0] != OSC)) // Include terminator as well
	    ਵਾਪਸ FAIL;

	// The whole OSC response may be larger than the typeahead buffer.
	// To handle this, keep reading data in and out of the typeahead
	// buffer until we read an OSC terminator or timeout.

	// We can't use the previous buffer since we transferred ownership of it
	// to the vim var.
	ga_init2(&osc_state.buf, 1, 1024);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ELAPSED_FUNC
	ELAPSED_INIT(osc_state.ਸ਼ੁਰੂ_tv);
#ਜੇ_ਅੰਤ
	osc_state.processing = TRUE;
	osc_state.ਸ਼ੁਰੂ_char = tp[0];
	ਆਖਰੀ_char = 0;
    }
    ਨਹੀਂਤਾਂ
	ਆਖਰੀ_char = ((char_u *)osc_state.buf.ga_ਅੰਕੜੇ)[osc_state.buf.ga_len - 1];

    ਕੁੰਜੀ_name[0] = (ਪੂਰਨ)KS_EXTRA;

    // Read data and append to buffer. If we reach a terminator, then
    // finally set the vim var.
    ਲਈ (ਪੂਰਨ i = 0; i < len; i++)
	ਜੇ (tp[i] == '\007' || (osc_state.ਸ਼ੁਰੂ_char == OSC ? tp[i] == STERM
		    : ((tp[i] == ESC && i + 1 < len && tp[i + 1] == '\\')
			|| (i == 0 && tp[i] == '\\' && ਆਖਰੀ_char == ESC)
			)))
	{
	    osc_state.processing = FALSE;

	    ਕੁੰਜੀ_name[1] = (ਪੂਰਨ)KE_OSC;

	    ga_concat_len(&osc_state.buf, tp, i + 1 + (tp[i] == ESC));
	    ga_append(&osc_state.buf, NUL);
	    *slen = i + 1 + (tp[i] == ESC);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	    ਸੈੱਟ_vim_var_string_ਸਿੱਧਾ(VV_TERMOSC, osc_state.buf.ga_ਅੰਕੜੇ);
#ਜੇ_ਅੰਤ
	    // Check for background/foreground colour response
	    check_for_color_response(osc_state.buf.ga_ਅੰਕੜੇ, osc_state.buf.ga_len - 1);

	    char_u savebg = *p_bg;
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"osc",
		    ਨਿਰਾ, FALSE, curbuf);
	    ਜੇ (*p_bg != savebg)
		redraw_asap(UPD_CLEAR);
	    ਵਾਪਸ OK;
	}

    ਕੁੰਜੀ_name[1] = (ਪੂਰਨ)KE_IGNORE;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ELAPSED_FUNC
    ਜੇ (ELAPSED_FUNC(osc_state.ਸ਼ੁਰੂ_tv) >= p_ost)
    {
	semsg(_(e_osc_response_timed_ਬਾਹਰਲਾ), osc_state.buf.ga_len,
		osc_state.buf.ga_ਅੰਕੜੇ);

	ga_clear(&osc_state.buf);
	osc_state.processing = FALSE;
	ਵਾਪਸ FAIL;
    }
#ਜੇ_ਅੰਤ

    ga_concat(&osc_state.buf, tp);
    *slen = len; // Consume everything

    ਵਾਪਸ OK;
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
    ਸਥਿਰ ਪੂਰਨ
handle_dcs(char_u *tp, char_u *argp, ਪੂਰਨ len, char_u *ਕੁੰਜੀ_name, ਪੂਰਨ *slen)
{
    ਪੂਰਨ i, j;

    LOG_TRN("Received DCS response: %s", (ਅੱਖਰ*)tp);
    j = 1 + (tp[0] == ESC);
    ਜੇ (len < j + 3)
	i = len; // need more chars
    ਨਹੀਂਤਾਂ ਜੇ ((argp[1] != '+' && argp[1] != '$')
	    || (argp[2] != 'r' && argp[2] != 'R'))
	i = 0; // no match
    ਨਹੀਂਤਾਂ ਜੇ (argp[1] == '+')
	// key code response
	ਲਈ (i = j; i < len; ++i)
	{
	    ਜੇ ((tp[i] == ESC && i + 1 < len && tp[i + 1] == '\\')
		    || tp[i] == STERM)
	    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMRESPONSE
		// handle a key code response, drop a resource response
		ਜੇ (i - j >= 3 && argp[2] == 'r')
		    got_ਕੋਡ_from_ਪਦ(tp + j, i);
#ਜੇ_ਅੰਤ
		ਕੁੰਜੀ_name[0] = (ਪੂਰਨ)KS_EXTRA;
		ਕੁੰਜੀ_name[1] = (ਪੂਰਨ)KE_IGNORE;
		*slen = i + 1 + (tp[i] == ESC);
		ਰੋਕੋ;
	    }
	}
    ਨਹੀਂਤਾਂ
    {
	// Probably the cursor shape response.  Make sure that "i"
	// is equal to "len" when there are not sufficient
	// characters.
	ਲਈ (i = j + 3; i < len; ++i)
	{
	    ਜੇ (i - j == 3 && !ਸੁਰੱਖਿਅਤ_isdigit(tp[i]))
		ਰੋਕੋ;
	    ਜੇ (i - j == 4 && tp[i] != ' ')
		ਰੋਕੋ;
	    ਜੇ (i - j == 5 && tp[i] != 'q')
		ਰੋਕੋ;
	    ਜੇ (i - j == 6 && tp[i] != ESC && tp[i] != STERM)
		ਰੋਕੋ;
	    ਜੇ ((i - j == 6 && tp[i] == STERM)
		    || (i - j == 7 && tp[i] == '\\'))
	    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMRESPONSE
		ਪੂਰਨ ਸੰਖਿਆ = argp[3] - '0';

		// 0, 1 = block blink, 2 = block
		// 3 = underline blink, 4 = underline
		// 5 = vertical bar blink, 6 = vertical bar
		ਸੰਖਿਆ = ਸੰਖਿਆ == 0 ? 1 : ਸੰਖਿਆ;
		initial_cursor_shape = (ਸੰਖਿਆ + 1) / 2;
		// The blink flag is actually inverted, compared to
		// the value set with T_SH.
		initial_cursor_shape_blink =
				       (ਸੰਖਿਆ & 1) ? FALSE : TRUE;
		rcs_ਸਥਿਤੀ.tr_ਤਰੱਕੀ = ਸਥਿਤੀ_GOT;
#ਜੇ_ਅੰਤ
		LOG_TRN("Received cursor shape response: %s", tp);

		ਕੁੰਜੀ_name[0] = (ਪੂਰਨ)KS_EXTRA;
		ਕੁੰਜੀ_name[1] = (ਪੂਰਨ)KE_IGNORE;
		*slen = i + 1;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
		ਸੈੱਟ_vim_var_string(VV_TERMSTYLERESP, tp, *slen);
#ਜੇ_ਅੰਤ
		apply_autocmds(EVENT_TERMRESPONSEALL,
					(char_u *)"cursorshape", ਨਿਰਾ, FALSE, curbuf);
		ਰੋਕੋ;
	    }
	}
    }

    ਜੇ (i == len)
    {
	// These codes arrive many together, each code can be
	// truncated at any point.
	LOG_TR1("not enough characters for XT");
	ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
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
    ਪੂਰਨ
check_termcode(
    ਪੂਰਨ		max_ਵਿੱਥ,
    char_u	*buf,
    ਪੂਰਨ		bufsize,
    ਪੂਰਨ		*buflen)
{
    char_u	*tp;
    char_u	*p;
    ਪੂਰਨ		slen = 0;	// init for GCC
    ਪੂਰਨ		modslen;
    ਪੂਰਨ		len;
    ਪੂਰਨ		retval = 0;
    ਪੂਰਨ		ਵਿੱਥ;
    char_u	ਕੁੰਜੀ_name[2];
    ਪੂਰਨ		modifiers;
    char_u	*modifiers_ਸ਼ੁਰੂ = ਨਿਰਾ;
    ਪੂਰਨ		ਕੁੰਜੀ;
    ਪੂਰਨ		new_slen;   // Length of what will replace the termcode
    char_u	string[MAX_ਕੁੰਜੀ_ਕੋਡ_LEN + 1];
    ਪੂਰਨ		i, j;
    ਪੂਰਨ		idx = 0;
    ਪੂਰਨ		cpo_koffset;

    cpo_koffset = (vim_strchr(p_cpo, CPO_KOFFSET) != ਨਿਰਾ);

    /*
     * Speed up the checks for terminal codes by gathering all first bytes
     * used in termleader[].  Often this is just a single <Esc>.
     */
    ਜੇ (need_gather)
	gather_termleader();

    /*
     * Check at several positions in typebuf.tb_buf[], to catch something like
     * "x<Up>" that can be mapped. Stop at max_offset, because characters
     * after that cannot be used for mapping, and with @r commands
     * typebuf.tb_buf[] can become very long.
     * This is used often, KEEP IT FAST!
     */
    ਲਈ (ਵਿੱਥ = 0; ਵਿੱਥ < max_ਵਿੱਥ; ++ਵਿੱਥ)
    {
	ਜੇ (buf == ਨਿਰਾ)
	{
	    ਜੇ (ਵਿੱਥ >= typebuf.tb_len)
		ਰੋਕੋ;
	    tp = typebuf.tb_buf + typebuf.tb_off + ਵਿੱਥ;
	    len = typebuf.tb_len - ਵਿੱਥ;	// length of the input
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (ਵਿੱਥ >= *buflen)
		ਰੋਕੋ;
	    tp = buf + ਵਿੱਥ;
	    len = *buflen - ਵਿੱਥ;
	}

	/*
	 * Don't check characters after K_SPECIAL, those are already
	 * translated terminal chars (avoid translating ~@^Hx).
	 */
	ਜੇ (*tp == K_SPECIAL)
	{
	    ਵਿੱਥ += 2;	// there are always 2 extra characters
	    ਜਾਰੀ;
	}

	ਜੇ (osc_state.processing)
	{
	    // Still processing OSC response data, go straight to handler
	    // function.
	    tp[len] = NUL;
	    ਕੁੰਜੀ_name[0] = NUL;
	    ਕੁੰਜੀ_name[1] = NUL;
	    modifiers = 0;
	    ਜਾਓ handle_osc;
	}

	/*
	 * Skip this position if the character does not appear as the first
	 * character in term_strings. This speeds up a lot, since most
	 * termcodes start with the same character (ESC or CSI).
	 */
	i = *tp;
	ਲਈ (p = termleader; *p && *p != i; ++p)
	    ;
	ਜੇ (*p == NUL)
	    ਜਾਰੀ;

	/*
	 * Skip this position if p_ek is not set and tp[0] is an ESC and we
	 * are in Insert mode.
	 */
	ਜੇ (*tp == ESC && !p_ek && (State & ਬਹੁਲਕ_ਪਾਓ))
	    ਜਾਰੀ;

	tp[len] = NUL;
	ਕੁੰਜੀ_name[0] = NUL;	// no key name found yet
	ਕੁੰਜੀ_name[1] = NUL;	// no key name found yet
	modifiers = 0;		// no modifiers yet

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
	ਜੇ (gui.ਵਿੱਚ_use)
	{
	    /*
	     * GUI special key codes are all of the form [CSI xx].
	     */
	    ਜੇ (*tp == CSI)	    // Special key from GUI
	    {
		ਜੇ (len < 3)
		    ਵਾਪਸ -1;	    // Shouldn't happen
		slen = 3;
		ਕੁੰਜੀ_name[0] = tp[1];
		ਕੁੰਜੀ_name[1] = tp[2];
	    }
	}
	ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ // FEAT_GUI
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
	    ਜੇ (len >= 3 && tp[0] == CSI && tp[1] == KS_EXTRA
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
	    ਕੁੰਜੀ_name[0] = tp[1];
	    ਕੁੰਜੀ_name[1] = tp[2];
	}
	ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	{
	    ਪੂਰਨ  mouse_ਸੂਚਕਾਂਕ_ਮਿਲਿਆ = -1;
	    ਪੂਰਨ keypad_ਸੂਚਕਾਂਕ_ਮਿਲਿਆ = -1;
	    ਪੂਰਨ keypad_slen_ਮਿਲਿਆ = 0;

	    ਲਈ (idx = 0; idx < tc_len; ++idx)
	    {
		ਪੂਰਨ     is_keypad = FALSE;
		/*
		 * Ignore the entry if we are not at the start of
		 * typebuf.tb_buf[]
		 * and there are not enough characters to make a match.
		 * But only when the 'K' flag is in 'cpoptions'.
		 */
		slen = termcodes[idx].len;
		modifiers_ਸ਼ੁਰੂ = ਨਿਰਾ;
		ਜੇ (cpo_koffset && ਵਿੱਥ && len < slen)
		    ਜਾਰੀ;
		ਜੇ (STRNCMP(termcodes[idx].ਕੋਡ, tp,
				     (size_t)(slen > len ? len : slen)) == 0)
		{
		    ਪੂਰਨ	    looks_like_mouse_ਸ਼ੁਰੂ = FALSE;

		    ਜੇ (len < slen)		// got a partial sequence
			ਵਾਪਸ -1;		// need to get more chars

		    /*
		     * When found a keypad key, check if there is another key
		     * that matches and use that one.  This makes <Home> to be
		     * found instead of <kHome> when they produce the same
		     * key code.
		     */
		    ਜੇ (termcodes[idx].name[0] == 'K'
				       && (VIM_ISDIGIT(termcodes[idx].name[1])
						|| ASCII_ISUPPER(termcodes[idx].name[1])))
		    {
			is_keypad = TRUE;
			// Only use it when there is no other match.
			ਜੇ (keypad_ਸੂਚਕਾਂਕ_ਮਿਲਿਆ < 0)
			{
			    keypad_ਸੂਚਕਾਂਕ_ਮਿਲਿਆ = idx;
			    keypad_slen_ਮਿਲਿਆ = slen;
			}
		    }

		    ਜੇ (slen == 2 && len > 2
			    && termcodes[idx].ਕੋਡ[0] == ESC
			    && termcodes[idx].ਕੋਡ[1] == '[')
		    {
			// The mouse termcode "ESC [" is also the prefix of
			// "ESC [ I" (focus gained) and other keys.  Check some
			// more bytes to find out.
			ਜੇ (!ਸੁਰੱਖਿਅਤ_isdigit(tp[2]))
			{
			    // ESC [ without number following: Only use it when
			    // there is no other match.
			    looks_like_mouse_ਸ਼ੁਰੂ = TRUE;
			}
			ਨਹੀਂਤਾਂ ਜੇ (termcodes[idx].name[0] == KS_DEC_MOUSE)
			{
			    char_u  *nr = tp + 2;
			    ਪੂਰਨ	    ਗਿਣਤੀ_2 = 0;

			    // If a digit is following it could be a key with
			    // modifier, e.g., ESC [ 1;2P.  Can be confused
			    // with DEC_MOUSE, which requires four numbers
			    // following.  If not then it can't be a DEC_MOUSE
			    // code.
			    ਲਈ (;;)
			    {
				++ਗਿਣਤੀ_2;
				(ਖਾਲੀ)getdigits(&nr);
				ਜੇ (nr >= tp + len)
				    ਵਾਪਸ -1;	// partial sequence
				ਜੇ (*nr != ';')
				    ਰੋਕੋ;
				++nr;
				ਜੇ (nr >= tp + len)
				    ਵਾਪਸ -1;	// partial sequence
			    }
			    ਜੇ (ਗਿਣਤੀ_2 < 4)
				ਜਾਰੀ;	// no match
			}
		    }
		    ਜੇ (looks_like_mouse_ਸ਼ੁਰੂ)
		    {
			// Only use it when there is no other match.
			ਜੇ (mouse_ਸੂਚਕਾਂਕ_ਮਿਲਿਆ < 0)
			    mouse_ਸੂਚਕਾਂਕ_ਮਿਲਿਆ = idx;
		    }
		    ਨਹੀਂਤਾਂ ਜੇ (!is_keypad)
		    {
			ਕੁੰਜੀ_name[0] = termcodes[idx].name[0];
			ਕੁੰਜੀ_name[1] = termcodes[idx].name[1];
			ਰੋਕੋ;
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
		ਜੇ (termcodes[idx].modlen > 0 && mouse_ਸੂਚਕਾਂਕ_ਮਿਲਿਆ < 0)
		{
		    modslen = termcodes[idx].modlen;
		    ਜੇ (cpo_koffset && ਵਿੱਥ && len < modslen)
			ਜਾਰੀ;
		    ਜੇ (STRNCMP(termcodes[idx].ਕੋਡ, tp,
				(size_t)(modslen > len ? len : modslen)) == 0)
		    {
			ਪੂਰਨ	    n;

			ਜੇ (len <= modslen)	// got a partial sequence
			    ਵਾਪਸ -1;		// need to get more chars

			ਜੇ (tp[modslen] == termcodes[idx].ਕੋਡ[slen - 1])
			    // no modifiers
			    slen = modslen + 1;
			ਨਹੀਂਤਾਂ ਜੇ (tp[modslen] != ';' && modslen == slen - 3)
			    // no match for "code;*X" with "code;"
			    ਜਾਰੀ;
			ਨਹੀਂਤਾਂ ਜੇ (termcodes[idx].ਕੋਡ[modslen] == '@'
					 && (tp[modslen] != '1'
						    || tp[modslen + 1] != ';'))
			    // no match for "<Esc>[@" with "<Esc>[1;"
			    ਜਾਰੀ;
			ਨਹੀਂਤਾਂ
			{
			    // Skip over the digits, the final char must
			    // follow. URXVT can use a negative value, thus
			    // also accept '-'.
			    ਲਈ (j = slen - 2; j < len && (ਸੁਰੱਖਿਅਤ_isdigit(tp[j])
				       || tp[j] == '-' || tp[j] == ';'); ++j)
				;
			    ++j;
			    ਜੇ (len < j)	// got a partial sequence
				ਵਾਪਸ -1;	// need to get more chars
			    ਜੇ (tp[j - 1] != termcodes[idx].ਕੋਡ[slen - 1])
				ਜਾਰੀ;	// no match

			    modifiers_ਸ਼ੁਰੂ = tp + slen - 2;

			    // Match!  Convert modifier bits.
			    n = atoi((ਅੱਖਰ *)modifiers_ਸ਼ੁਰੂ);
			    modifiers |= decode_modifiers(n);

			    slen = j;
			}
			ਜੇ (termcodes[idx].name[0] == 'K'
					   && (VIM_ISDIGIT(termcodes[idx].name[1])
						    || ASCII_ISUPPER(termcodes[idx].name[1])))
			{
			    is_keypad = TRUE;
			    ਜੇ (keypad_ਸੂਚਕਾਂਕ_ਮਿਲਿਆ < 0)
			    {
				keypad_ਸੂਚਕਾਂਕ_ਮਿਲਿਆ = idx;
				keypad_slen_ਮਿਲਿਆ = slen;
			    }
			}
			ਜੇ (!is_keypad)
			{
			    ਕੁੰਜੀ_name[0] = termcodes[idx].name[0];
			    ਕੁੰਜੀ_name[1] = termcodes[idx].name[1];
			    ਰੋਕੋ;
			}
		    }
		}
	    }
	    ਜੇ (idx == tc_len && keypad_ਸੂਚਕਾਂਕ_ਮਿਲਿਆ >= 0)
	    {
		ਕੁੰਜੀ_name[0] = termcodes[keypad_ਸੂਚਕਾਂਕ_ਮਿਲਿਆ].name[0];
		ਕੁੰਜੀ_name[1] = termcodes[keypad_ਸੂਚਕਾਂਕ_ਮਿਲਿਆ].name[1];
		slen = keypad_slen_ਮਿਲਿਆ;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (idx == tc_len && mouse_ਸੂਚਕਾਂਕ_ਮਿਲਿਆ >= 0)
	    {
		ਕੁੰਜੀ_name[0] = termcodes[mouse_ਸੂਚਕਾਂਕ_ਮਿਲਿਆ].name[0];
		ਕੁੰਜੀ_name[1] = termcodes[mouse_ਸੂਚਕਾਂਕ_ਮਿਲਿਆ].name[1];
	    }
	}

	ਜੇ (ਕੁੰਜੀ_name[0] == NUL
	    // Mouse codes of DEC and pterm start with <ESC>[.  When
	    // detecting the start of these mouse codes they might as well be
	    // another key code or terminal response.
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MOUSE_DEC
	    || ਕੁੰਜੀ_name[0] == KS_DEC_MOUSE
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MOUSE_PTERM
	    || ਕੁੰਜੀ_name[0] == KS_PTERM_MOUSE
#ਜੇ_ਅੰਤ
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
	    ਜੇ (((tp[0] == ESC && len >= 3 && tp[1] == '[')
			|| (tp[0] == CSI && len >= 2))
		    && vim_strchr((char_u *)"0123456789>?ABCDEFHPQRS",
								*argp) != ਨਿਰਾ)
	    {
		ਪੂਰਨ resp = handle_csi(tp, len, argp, ਵਿੱਥ, buf,
					     bufsize, buflen, ਕੁੰਜੀ_name, &slen);
		ਜੇ (resp != 0)
		{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG_TERMRESPONSE
		    ਜੇ (resp == -1)
			LOG_TR1("Not enough characters for CSI sequence");
#ਜੇ_ਅੰਤ
		    ਵਾਪਸ resp;
		}
	    }

	    // Check for OSC responses from terminal
	    ਨਹੀਂਤਾਂ ਜੇ ((tp[0] == ESC && len >= 2 && tp[1] == ']') || tp[0] == OSC)
	    {
handle_osc:
		ਜੇ (handle_osc(tp, len, ਕੁੰਜੀ_name, &slen) == FAIL)
		    ਵਾਪਸ -1;
	    }

	    // Check for key code response from xterm,
	    // starting with <Esc>P or DCS
	    // It would only be needed with this condition:
	    //	    (check_for_codes || rcs_status.tr_progress == STATUS_SENT)
	    // Now this is always done so that DCS codes don't mess up things.
	    ਨਹੀਂਤਾਂ ਜੇ ((tp[0] == ESC && len >= 2 && tp[1] == 'P') || tp[0] == DCS)
	    {
		ਜੇ (handle_dcs(tp, argp, len, ਕੁੰਜੀ_name, &slen) == FAIL)
		    ਵਾਪਸ -1;
	    }
	}

	ਜੇ (ਕੁੰਜੀ_name[0] == NUL)
	    ਜਾਰੀ;	    // No match at this position, try next one

	// We only get here when we have a complete termcode match

#ਜੇ defined(FEAT_GUI) || defined(MSWIN)
	/*
	 * For scroll events from the GUI or MS-Windows console, fetch the
	 * pointer coordinates so that we know which window to scroll later.
	 */
	ਜੇ (TRUE
# ਜੇ defined(FEAT_GUI) && !defined(MSWIN)
		&& gui.ਵਿੱਚ_use
# endif
		&& ਕੁੰਜੀ_name[0] == (ਪੂਰਨ)KS_EXTRA
		&& (ਕੁੰਜੀ_name[1] == (ਪੂਰਨ)KE_X1MOUSE
		    || ਕੁੰਜੀ_name[1] == (ਪੂਰਨ)KE_X2MOUSE
		    || ਕੁੰਜੀ_name[1] == (ਪੂਰਨ)KE_MOUSEMOVE_XY
		    || ਕੁੰਜੀ_name[1] == (ਪੂਰਨ)KE_MOUSELEFT
		    || ਕੁੰਜੀ_name[1] == (ਪੂਰਨ)KE_MOUSERIGHT
		    || ਕੁੰਜੀ_name[1] == (ਪੂਰਨ)KE_MOUSEDOWN
		    || ਕੁੰਜੀ_name[1] == (ਪੂਰਨ)KE_MOUSEUP))
	{
	    char_u	ਬਾਈਟਾਂ[6];
	    ਪੂਰਨ		ਅੰਕ_ਬਾਈਟਾਂ = get_ਬਾਈਟਾਂ_from_buf(tp + slen, ਬਾਈਟਾਂ, 4);

	    ਜੇ (ਅੰਕ_ਬਾਈਟਾਂ == -1)	// not enough coordinates
		ਵਾਪਸ -1;
	    mouse_ਖਾਨਾ = 128 * (ਬਾਈਟਾਂ[0] - ' ' - 1) + ਬਾਈਟਾਂ[1] - ' ' - 1;
	    mouse_ਪੰਕਤੀ = 128 * (ਬਾਈਟਾਂ[2] - ' ' - 1) + ਬਾਈਟਾਂ[3] - ' ' - 1;
	    slen += ਅੰਕ_ਬਾਈਟਾਂ;
	    // equal to K_MOUSEMOVE
	    ਜੇ (ਕੁੰਜੀ_name[1] == (ਪੂਰਨ)KE_MOUSEMOVE_XY)
		ਕੁੰਜੀ_name[1] = (ਪੂਰਨ)KE_MOUSEMOVE;
	}
	ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	/*
	 * If it is a mouse click, get the coordinates.
	 */
	ਜੇ (ਕੁੰਜੀ_name[0] == KS_MOUSE
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MOUSE_GPM
		|| ਕੁੰਜੀ_name[0] == KS_GPM_MOUSE
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MOUSE_JSB
		|| ਕੁੰਜੀ_name[0] == KS_JSBTERM_MOUSE
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MOUSE_NET
		|| ਕੁੰਜੀ_name[0] == KS_NETTERM_MOUSE
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MOUSE_DEC
		|| ਕੁੰਜੀ_name[0] == KS_DEC_MOUSE
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MOUSE_PTERM
		|| ਕੁੰਜੀ_name[0] == KS_PTERM_MOUSE
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MOUSE_URXVT
		|| ਕੁੰਜੀ_name[0] == KS_URXVT_MOUSE
#ਜੇ_ਅੰਤ
		|| ਕੁੰਜੀ_name[0] == KS_SGR_MOUSE
		|| ਕੁੰਜੀ_name[0] == KS_SGR_MOUSE_RELEASE)
	{
	    ਜੇ (check_termcode_mouse(tp, &slen, ਕੁੰਜੀ_name, modifiers_ਸ਼ੁਰੂ, idx,
							     &modifiers) == -1)
		ਵਾਪਸ -1;
	}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
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
	ਨਹੀਂਤਾਂ ਜੇ (ਕੁੰਜੀ_name[0] == (ਪੂਰਨ)KS_MENU)
	{
	    long_u	val;
	    ਪੂਰਨ		ਅੰਕ_ਬਾਈਟਾਂ = get_long_from_buf(tp + slen, &val);

	    ਜੇ (ਅੰਕ_ਬਾਈਟਾਂ == -1)
		ਵਾਪਸ -1;
	    current_menu = (vimmenu_T *)val;
	    slen += ਅੰਕ_ਬਾਈਟਾਂ;

	    // The menu may have been deleted right after it was used, check
	    // for that.
	    ਜੇ (check_menu_ਸੂਚਕ(ਜੜ੍ਹ_menu, current_menu) == FAIL)
	    {
		ਕੁੰਜੀ_name[0] = KS_EXTRA;
		ਕੁੰਜੀ_name[1] = (ਪੂਰਨ)KE_IGNORE;
	    }
	}
# endif
# ifdef FEAT_GUI_TABLINE
	ਨਹੀਂਤਾਂ ਜੇ (ਕੁੰਜੀ_name[0] == (ਪੂਰਨ)KS_TABLINE)
	{
	    // Selecting tabline tab or using its menu.
	    char_u	ਬਾਈਟਾਂ[6];
	    ਪੂਰਨ		ਅੰਕ_ਬਾਈਟਾਂ = get_ਬਾਈਟਾਂ_from_buf(tp + slen, ਬਾਈਟਾਂ, 1);

	    ਜੇ (ਅੰਕ_ਬਾਈਟਾਂ == -1)
		ਵਾਪਸ -1;
	    current_tab = (ਪੂਰਨ)ਬਾਈਟਾਂ[0];
	    ਜੇ (current_tab == 255)	// -1 in a byte gives 255
		current_tab = -1;
	    slen += ਅੰਕ_ਬਾਈਟਾਂ;
	}
	ਨਹੀਂਤਾਂ ਜੇ (ਕੁੰਜੀ_name[0] == (ਪੂਰਨ)KS_TABMENU)
	{
	    // Selecting tabline tab or using its menu.
	    char_u	ਬਾਈਟਾਂ[6];
	    ਪੂਰਨ		ਅੰਕ_ਬਾਈਟਾਂ = get_ਬਾਈਟਾਂ_from_buf(tp + slen, ਬਾਈਟਾਂ, 2);

	    ਜੇ (ਅੰਕ_ਬਾਈਟਾਂ == -1)
		ਵਾਪਸ -1;
	    current_tab = (ਪੂਰਨ)ਬਾਈਟਾਂ[0];
	    current_tabmenu = (ਪੂਰਨ)ਬਾਈਟਾਂ[1];
	    slen += ਅੰਕ_ਬਾਈਟਾਂ;
	}
# endif
# ifndef USE_ON_FLY_SCROLL
	ਨਹੀਂਤਾਂ ਜੇ (ਕੁੰਜੀ_name[0] == (ਪੂਰਨ)KS_VER_SCROLLBAR)
	{
	    long_u	val;
	    char_u	ਬਾਈਟਾਂ[6];
	    ਪੂਰਨ		ਅੰਕ_ਬਾਈਟਾਂ;

	    // Get the last scrollbar event in the queue of the same type
	    j = 0;
	    ਲਈ (i = 0; tp[j] == CSI && tp[j + 1] == KS_VER_SCROLLBAR
						     && tp[j + 2] != NUL; ++i)
	    {
		j += 3;
		ਅੰਕ_ਬਾਈਟਾਂ = get_ਬਾਈਟਾਂ_from_buf(tp + j, ਬਾਈਟਾਂ, 1);
		ਜੇ (ਅੰਕ_ਬਾਈਟਾਂ == -1)
		    ਰੋਕੋ;
		ਜੇ (i == 0)
		    current_scrollbar = (ਪੂਰਨ)ਬਾਈਟਾਂ[0];
		ਨਹੀਂਤਾਂ ਜੇ (current_scrollbar != (ਪੂਰਨ)ਬਾਈਟਾਂ[0])
		    ਰੋਕੋ;
		j += ਅੰਕ_ਬਾਈਟਾਂ;
		ਅੰਕ_ਬਾਈਟਾਂ = get_long_from_buf(tp + j, &val);
		ਜੇ (ਅੰਕ_ਬਾਈਟਾਂ == -1)
		    ਰੋਕੋ;
		scrollbar_ਮੁੱਲ = val;
		j += ਅੰਕ_ਬਾਈਟਾਂ;
		slen = j;
	    }
	    ਜੇ (i == 0)		// not enough characters to make one
		ਵਾਪਸ -1;
	}
	ਨਹੀਂਤਾਂ ਜੇ (ਕੁੰਜੀ_name[0] == (ਪੂਰਨ)KS_HOR_SCROLLBAR)
	{
	    long_u	val;
	    ਪੂਰਨ		ਅੰਕ_ਬਾਈਟਾਂ;

	    // Get the last horiz. scrollbar event in the queue
	    j = 0;
	    ਲਈ (i = 0; tp[j] == CSI && tp[j + 1] == KS_HOR_SCROLLBAR
						     && tp[j + 2] != NUL; ++i)
	    {
		j += 3;
		ਅੰਕ_ਬਾਈਟਾਂ = get_long_from_buf(tp + j, &val);
		ਜੇ (ਅੰਕ_ਬਾਈਟਾਂ == -1)
		    ਰੋਕੋ;
		scrollbar_ਮੁੱਲ = val;
		j += ਅੰਕ_ਬਾਈਟਾਂ;
		slen = j;
	    }
	    ਜੇ (i == 0)		// not enough characters to make one
		ਵਾਪਸ -1;
	}
# endif // !USE_ON_FLY_SCROLL
#ਜੇ_ਅੰਤ // FEAT_GUI

#ਜੇ defined(UNIX) || defined(VMS)
	/*
	 * Handle FocusIn/FocusOut event sequences reported by XTerm.
	 * (CSI I/CSI O)
	 */
	ਜੇ (ਕੁੰਜੀ_name[0] == KS_EXTRA
# ifdef FEAT_GUI
		&& !gui.ਵਿੱਚ_use
# endif
	    )
	{
	    ਜੇ (ਕੁੰਜੀ_name[1] == KE_FOCUSGAINED)
	    {
		ਜੇ (!focus_state)
		{
		    ui_focus_change(TRUE);
		    did_cursorhold = TRUE;
		    focus_state = TRUE;
		}
		ਕੁੰਜੀ_name[1] = (ਪੂਰਨ)KE_IGNORE;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (ਕੁੰਜੀ_name[1] == KE_FOCUSLOST)
	    {
		ਜੇ (focus_state)
		{
		    ui_focus_change(FALSE);
		    did_cursorhold = TRUE;
		    focus_state = FALSE;
		}
		ਕੁੰਜੀ_name[1] = (ਪੂਰਨ)KE_IGNORE;
	    }
	}
#ਜੇ_ਅੰਤ

	/*
	 * Change <xHome> to <Home>, <xUp> to <Up>, etc.
	 */
	ਕੁੰਜੀ = handle_x_ਕੁੰਜੀਆਂ(TERMCAP2_ਕੁੰਜੀ(ਕੁੰਜੀ_name[0], ਕੁੰਜੀ_name[1]));

	// Add any modifier codes to our string.
	new_slen = modifiers2keycode(modifiers, &ਕੁੰਜੀ, string);

	// Finally, add the special key code to our string
	ਕੁੰਜੀ_name[0] = KEY2TERMCAP0(ਕੁੰਜੀ);
	ਕੁੰਜੀ_name[1] = KEY2TERMCAP1(ਕੁੰਜੀ);
	ਜੇ (ਕੁੰਜੀ_name[0] == KS_ਕੁੰਜੀ)
	{
	    // from ":set <M-b>=xx"
	    ਜੇ (has_mbyte)
		new_slen += (*mb_char2bytes)(ਕੁੰਜੀ_name[1], string + new_slen);
	    ਨਹੀਂਤਾਂ
		string[new_slen++] = ਕੁੰਜੀ_name[1];
	}
	ਨਹੀਂਤਾਂ ਜੇ (osc_state.processing ||
		(new_slen == 0
		 && ਕੁੰਜੀ_name[0] == KS_EXTRA
		 && ਕੁੰਜੀ_name[1] == KE_IGNORE))
	{
	    // Do not put K_IGNORE into the buffer, do return KEYLEN_REMOVED
	    // to indicate what happened.
	    retval = KEYLEN_REMOVED;
	}
	ਨਹੀਂਤਾਂ
	{
	    string[new_slen++] = K_SPECIAL;
	    string[new_slen++] = ਕੁੰਜੀ_name[0];
	    string[new_slen++] = ਕੁੰਜੀ_name[1];
	}
	ਜੇ (put_string_ਵਿੱਚ_typebuf(ਵਿੱਥ, slen, string, new_slen,
						 buf, bufsize, buflen) == FAIL)
	    ਵਾਪਸ -1;
	ਵਾਪਸ retval == 0 ? (len + new_slen - slen + ਵਿੱਥ) : retval;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMRESPONSE
    LOG_TR1("normal character");
#ਜੇ_ਅੰਤ

    ਵਾਪਸ 0;			    // no match found
}

#ਜੇ defined(FEAT_TERMINAL) && defined(FEAT_TERMRESPONSE)
/*
 * Get the text foreground color, if known.
 */
    ਖਾਲੀ
ਪਦ_get_fg_color(char_u *r, char_u *g, char_u *b)
{
    ਜੇ (rfg_ਸਥਿਤੀ.tr_ਤਰੱਕੀ != ਸਥਿਤੀ_GOT)
	ਵਾਪਸ;

    *r = fg_r;
    *g = fg_g;
    *b = fg_b;
}
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMRESPONSE
/*
 * Get the text background color, if known.  Returns OK when the OSC 11
 * response has been parsed and *r, *g, *b are populated; FAIL otherwise.
 */
    ਪੂਰਨ
ਪਦ_get_bg_color(char_u *r, char_u *g, char_u *b)
{
    ਜੇ (rbg_ਸਥਿਤੀ.tr_ਤਰੱਕੀ != ਸਥਿਤੀ_GOT)
	ਵਾਪਸ FAIL;

    *r = bg_r;
    *g = bg_g;
    *b = bg_b;
    ਵਾਪਸ OK;
}
#ਜੇ_ਅੰਤ

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
    scid_T	sid_arg ਅਣਵਰਤਿਆ,	// script ID to use for <SID>,
				// or 0 to use current_sctx
    ਪੂਰਨ		flags,
    ਪੂਰਨ		*did_simplify)
{
    ਪੂਰਨ		i;
    ਪੂਰਨ		slen;
    ਪੂਰਨ		ਕੁੰਜੀ;
    size_t	dlen = 0;
    char_u	*src;
    ਪੂਰਨ		do_backslash;	// backslash is a special character
    ਪੂਰਨ		do_special;	// recognize <> key codes
    ਪੂਰਨ		do_ਕੁੰਜੀ_ਕੋਡ;	// recognize raw key codes
    char_u	*ਨਤੀਜਾ;	// buffer for resulting string
    garray_T	ga;

    do_backslash = (vim_strchr(p_cpo, CPO_BSLASH) == ਨਿਰਾ);
    do_special = (vim_strchr(p_cpo, CPO_SPECI) == ਨਿਰਾ)
						  || (flags & REPTERM_SPECIAL);
    do_ਕੁੰਜੀ_ਕੋਡ = (vim_strchr(p_cpo, CPO_KEYCODE) == ਨਿਰਾ);
    src = from;

    /*
     * Allocate space for the translation.  Worst case a single character is
     * replaced by 6 bytes (shifted special key), plus a NUL at the end.
     * In the rare case more might be needed ga_grow() must be called again.
     */
    ga_init2(&ga, 1L, 100);
    ਜੇ (ga_grow(&ga, (ਪੂਰਨ)(STRLEN(src) * 6 + 1)) == FAIL) // out of memory
    {
	*bufp = ਨਿਰਾ;
	ਵਾਪਸ from;
    }
    ਨਤੀਜਾ = ga.ga_ਅੰਕੜੇ;

    /*
     * Check for #n at start only: function key n
     */
    ਜੇ ((flags & REPTERM_FROM_PART) && src[0] == '#' && VIM_ISDIGIT(src[1]))
    {
	ਨਤੀਜਾ[dlen++] = K_SPECIAL;
	ਨਤੀਜਾ[dlen++] = 'k';
	ਜੇ (src[1] == '0')
	    ਨਤੀਜਾ[dlen++] = ';';	// #0 is F10 is "k;"
	ਨਹੀਂਤਾਂ
	    ਨਤੀਜਾ[dlen++] = src[1];	// #3 is F3 is "k3"
	src += 2;
    }

    /*
     * Copy each byte from *from to result[dlen]
     */
    ਜਦੋਂ (*src != NUL)
    {
	/*
	 * If 'cpoptions' does not contain '<', check for special key codes,
	 * like "<C-S-LeftMouse>"
	 */
	ਜੇ (do_special && ((flags & REPTERM_DO_LT)
					      || STRNCMP(src, "<lt>", 4) != 0))
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	    /*
	     * Change <SID>Func to K_SNR <script-nr> _Func.  This name is used
	     * for script-local user functions.
	     * (room: 5 * 6 = 30 bytes; needed: 3 + <nr> + 1 <= 14)
	     * Also change <SID>name.Func to K_SNR <import-script-nr> _Func.
	     * Only if "name" is recognized as an import.
	     */
	    ਜੇ (STRNICMP(src, "<SID>", 5) == 0)
	    {
		ਜੇ (sid_arg < 0 || (sid_arg == 0 && current_sctx.sc_sid <= 0))
		    emsg(_(e_using_sid_not_ਵਿੱਚ_script_context));
		ਨਹੀਂਤਾਂ
		{
		    char_u  *dot;
		    ਲੰਮਾ    sid = sid_arg != 0 ? sid_arg : current_sctx.sc_sid;

		    src += 5;
		    ਜੇ (ਵਿੱਚ_vim9script()
				       && (dot = vim_strchr(src, '.')) != ਨਿਰਾ)
		    {
			imported_T *imp = ਲੱਭੋ_imported(src, dot - src, FALSE);

			ਜੇ (imp != ਨਿਰਾ)
			{
			    scriptitem_T    *si = SCRIPT_ਵਸਤੂ(imp->imp_sid);
			    size_t	    len;

			    src = dot + 1;
			    ਜੇ (si->sn_autoload_ਅਗੇਤਰ != ਨਿਰਾ)
			    {
				// Turn "<SID>name.Func"
				// into "scriptname#Func".
				len = STRLEN(si->sn_autoload_ਅਗੇਤਰ);
				ਜੇ (ga_grow(&ga,
				     (ਪੂਰਨ)(STRLEN(src) * 6 + len + 1)) == FAIL)
				{
				    ga_clear(&ga);
				    *bufp = ਨਿਰਾ;
				    ਵਾਪਸ from;
				}
				ਨਤੀਜਾ = ga.ga_ਅੰਕੜੇ;
				STRCPY(ਨਤੀਜਾ + dlen, si->sn_autoload_ਅਗੇਤਰ);
				dlen += len;
				ਜਾਰੀ;
			    }
			    sid = imp->imp_sid;
			}
		    }

		    ਨਤੀਜਾ[dlen++] = K_SPECIAL;
		    ਨਤੀਜਾ[dlen++] = (ਪੂਰਨ)KS_EXTRA;
		    ਨਤੀਜਾ[dlen++] = (ਪੂਰਨ)KE_SNR;
		    sprintf((ਅੱਖਰ *)ਨਤੀਜਾ + dlen, "%ld", sid);
		    dlen += STRLEN(ਨਤੀਜਾ + dlen);
		    ਨਤੀਜਾ[dlen++] = '_';
		    ਜਾਰੀ;
		}
	    }
#ਜੇ_ਅੰਤ
	    ਪੂਰਨ fsk_flags = FSK_KEYCODE
			| ((flags & REPTERM_NO_SIMPLIFY) ? 0 : FSK_SIMPLIFY)
			| ((flags & REPTERM_FROM_PART) ? FSK_FROM_PART : 0);
	    slen = trans_special(&src, ਨਤੀਜਾ + dlen, fsk_flags,
							   TRUE, did_simplify);
	    ਜੇ (slen > 0)
	    {
		dlen += slen;
		ਜਾਰੀ;
	    }
	}

	/*
	 * If 'cpoptions' does not contain 'k', see if it's an actual key-code.
	 * Note that this is also checked after replacing the <> form.
	 * Single character codes are NOT replaced (e.g. ^H or DEL), because
	 * it could be a character in the file.
	 */
	ਜੇ (do_ਕੁੰਜੀ_ਕੋਡ)
	{
	    ਪੂਰਨ len;
	    i = ਲੱਭੋ_ਪਦ_bykeys(src, &len);
	    ਜੇ (i >= 0)
	    {
		ਨਤੀਜਾ[dlen++] = K_SPECIAL;
		ਨਤੀਜਾ[dlen++] = termcodes[i].name[0];
		ਨਤੀਜਾ[dlen++] = termcodes[i].name[1];
		src += len;
		// If terminal code matched, continue after it.
		ਜਾਰੀ;
	    }
	}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	ਜੇ (do_special)
	{
	    char_u	*p, *s, len;

	    /*
	     * Replace <Leader> by the value of "mapleader".
	     * Replace <LocalLeader> by the value of "maplocalleader".
	     * If "mapleader" or "maplocalleader" isn't set use a backslash.
	     */
	    ਜੇ (STRNICMP(src, "<Leader>", 8) == 0)
	    {
		len = 8;
		p = get_var_ਮੁੱਲ((char_u *)"g:mapleader");
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (STRNICMP(src, "<LocalLeader>", 13) == 0)
	    {
		len = 13;
		p = get_var_ਮੁੱਲ((char_u *)"g:maplocalleader");
	    }
	    ਨਹੀਂਤਾਂ
	    {
		len = 0;
		p = ਨਿਰਾ;
	    }
	    ਜੇ (len != 0)
	    {
		// Allow up to 8 * 6 characters for "mapleader".
		ਜੇ (p == ਨਿਰਾ || *p == NUL || STRLEN(p) > 8 * 6)
		    s = (char_u *)"\\";
		ਨਹੀਂਤਾਂ
		    s = p;
		ਜਦੋਂ (*s != NUL)
		    ਨਤੀਜਾ[dlen++] = *s++;
		src += len;
		ਜਾਰੀ;
	    }
	}
#ਜੇ_ਅੰਤ

	/*
	 * Remove CTRL-V and ignore the next character.
	 * For "from" side the CTRL-V at the end is included, for the "to"
	 * part it is removed.
	 * If 'cpoptions' does not contain 'B', also accept a backslash.
	 */
	ਕੁੰਜੀ = *src;
	ਜੇ (ਕੁੰਜੀ == Ctrl_V || (do_backslash && ਕੁੰਜੀ == '\\'))
	{
	    ++src;				// skip CTRL-V or backslash
	    ਜੇ (*src == NUL)
	    {
		ਜੇ (flags & REPTERM_FROM_PART)
		    ਨਤੀਜਾ[dlen++] = ਕੁੰਜੀ;
		ਰੋਕੋ;
	    }
	}

	// skip multibyte char correctly
	ਲਈ (i = (*mb_ptr2len)(src); i > 0; --i)
	{
	    /*
	     * If the character is K_SPECIAL, replace it with K_SPECIAL
	     * KS_SPECIAL KE_FILLER.
	     * If compiled with the GUI replace CSI with K_CSI.
	     */
	    ਜੇ (*src == K_SPECIAL)
	    {
		ਨਤੀਜਾ[dlen++] = K_SPECIAL;
		ਨਤੀਜਾ[dlen++] = KS_SPECIAL;
		ਨਤੀਜਾ[dlen++] = KE_FILLER;
	    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
	    ਨਹੀਂਤਾਂ ਜੇ (*src == CSI)
	    {
		ਨਤੀਜਾ[dlen++] = K_SPECIAL;
		ਨਤੀਜਾ[dlen++] = KS_EXTRA;
		ਨਤੀਜਾ[dlen++] = (ਪੂਰਨ)KE_CSI;
	    }
#ਜੇ_ਅੰਤ
	    ਨਹੀਂਤਾਂ
		ਨਤੀਜਾ[dlen++] = *src;
	    ++src;
	}
    }
    ਨਤੀਜਾ[dlen] = NUL;

    /*
     * Copy the new string to allocated memory.
     * If this fails, just return from.
     */
    ਜੇ ((*bufp = vim_strsave(ਨਤੀਜਾ)) != ਨਿਰਾ)
	from = *bufp;
    vim_free(ਨਤੀਜਾ);
    ਵਾਪਸ from;
}

/*
 * Find a termcode with keys 'src' (must be NUL terminated).
 * Return the index in termcodes[], or -1 if not found.
 */
    ਸਥਿਰ ਪੂਰਨ
ਲੱਭੋ_ਪਦ_bykeys(char_u *src, ਪੂਰਨ *matchlen)
{
    ਪੂਰਨ		i, j;
    ਪੂਰਨ		len = (ਪੂਰਨ)STRLEN(src);
    ਪੂਰਨ         ਮਿਲਿਆ = -1;
    // Don't return a match for a single character
    ਪੂਰਨ         foundlen = 1;
    ਪੂਰਨ         slen, modslen;
    ਪੂਰਨ         thislen;

    // Most input bytes cannot start a terminal code.  Reuse the same leader
    // table as check_termcode() to avoid scanning termcodes[] unnecessarily.
    ਜੇ (need_gather)
	gather_termleader();
    ਜੇ (*src == NUL || vim_strchr(termleader, *src) == ਨਿਰਾ)
	ਵਾਪਸ -1;

    // find longest match
    // borrows part of check_termcode
    ਲਈ (i = 0; i < tc_len; ++i)
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
	ਜੇ (modslen > 0)
	{
	    ਜੇ (len > modslen
			&& STRNCMP(termcodes[i].ਕੋਡ, src, (size_t)modslen) == 0)
	    {
		thislen = 0;

		ਜੇ (src[modslen] == termcodes[i].ਕੋਡ[slen - 1])
		    // no modifiers
		    thislen = modslen + 1;
		ਨਹੀਂਤਾਂ ਜੇ (src[modslen] != ';' && modslen == slen - 3)
		    // no match for "code;*X" with "code;"
		    ਜਾਰੀ;
		ਨਹੀਂਤਾਂ ਜੇ (termcodes[i].ਕੋਡ[modslen] == '@'
				&& (src[modslen] != '1'
					    || src[modslen + 1] != ';'))
		    // no match for "<Esc>[@" with "<Esc>[1;"
		    ਜਾਰੀ;
		ਨਹੀਂਤਾਂ
		{
		    // Skip over the digits, the final char must
		    // follow. URXVT can use a negative value, thus
		    // also accept '-'.
		    ਲਈ (j = slen - 2; j < len && (ਸੁਰੱਖਿਅਤ_isdigit(src[j])
			       || src[j] == '-' || src[j] == ';'); ++j)
			;
		    ++j;
		    ਜੇ (len < j)	// got a partial sequence
			ਜਾਰੀ;
		    ਜੇ (src[j - 1] != termcodes[i].ਕੋਡ[slen - 1])
			ਜਾਰੀ;	// no match

		    thislen = j;
		}

		ਜੇ (thislen > foundlen)
		{
		    ਮਿਲਿਆ = i;
		    foundlen = thislen;
		}
	    }
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (slen > foundlen && len >= slen
			&& STRNCMP(termcodes[i].ਕੋਡ, src, (size_t)slen) == 0)
	    {
		ਮਿਲਿਆ = i;
		foundlen = slen;
	    }
	}
    }

    ਜੇ (matchlen != ਨਿਰਾ && ਮਿਲਿਆ >= 0)
	*matchlen = foundlen;

    ਵਾਪਸ ਮਿਲਿਆ;
}

/*
 * Gather the first characters in the terminal key codes into a string.
 * Used to speed up check_termcode().
 */
    ਸਥਿਰ ਖਾਲੀ
gather_termleader(ਖਾਲੀ)
{
    ਪੂਰਨ	    i;
    ਪੂਰਨ	    len = 0;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    ਜੇ (gui.ਵਿੱਚ_use)
	termleader[len++] = CSI;    // the GUI codes are not in termcodes[]
#ਜੇ_ਅੰਤ

    // Querying cell size uses CSI 14 t
#ਜੇ defined(MSWIN) && (defined(FEAT_IMAGE) || defined(FEAT_EVAL))
# ifdef FEAT_GUI
    ਜੇ (!gui.ਵਿੱਚ_use)
# endif
    {
	termleader[len++] = ESC;
# ifdef FEAT_GUI
	ਜੇ (!gui.ਵਿੱਚ_use)
# endif
	    termleader[len++] = CSI;
    }
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMRESPONSE
    ਜੇ (check_for_codes || *T_CRS != NUL)
	termleader[len++] = DCS;    // the termcode response starts with DCS
				    // in 8-bit mode
#ਜੇ_ਅੰਤ
    termleader[len] = NUL;

    ਲਈ (i = 0; i < tc_len; ++i)
	ਜੇ (vim_strchr(termleader, termcodes[i].ਕੋਡ[0]) == ਨਿਰਾ)
	{
	    termleader[len++] = termcodes[i].ਕੋਡ[0];
	    termleader[len] = NUL;
	}

    need_gather = FALSE;
}

/*
 * Show all termcodes (for ":set termcap")
 * This code looks a lot like showoptions(), but is different.
 * "flags" can have OPT_ONECOLUMN.
 */
    ਖਾਲੀ
show_termcodes(ਪੂਰਨ flags)
{
    ਪੂਰਨ		ਖਾਨਾ;
    ਪੂਰਨ		*ਚੀਜ਼ਾਂ;
    ਪੂਰਨ		ਵਸਤੂ_ਗਿਣਤੀ;
    ਪੂਰਨ		ਲੜੀਵਾਰ;
    ਪੂਰਨ		ਪੰਕਤੀ, ਪੰਕਤੀਆਂ_2;
    ਪੂਰਨ		ਖਾਨੇ_ਗਿਣਤੀ;
    ਪੂਰਨ		i;
    ਪੂਰਨ		len;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ INC3 27	    // try to make three columns
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ INC2 40	    // try to make two columns
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GAP 2	    // spaces between columns

    ਜੇ (tc_len == 0)	    // no terminal codes (must be GUI)
	ਵਾਪਸ;
    ਚੀਜ਼ਾਂ = ALLOC_MULT(ਪੂਰਨ, tc_len);
    ਜੇ (ਚੀਜ਼ਾਂ == ਨਿਰਾ)
	ਵਾਪਸ;

    // Highlight title
    ਸੁਨੇਹਾ_puts_title(_("\n--- Terminal keys ---"));

    /*
     * Do the loop three times:
     * 1. display the short items (non-strings and short strings)
     * 2. display the medium items (medium length strings)
     * 3. display the long items (remaining strings)
     * When "flags" has OPT_ONECOLUMN do everything in 3.
     */
    ਲਈ (ਲੜੀਵਾਰ = (flags & OPT_ONECOLUMN) ? 3 : 1; ਲੜੀਵਾਰ <= 3 && !got_int; ++ਲੜੀਵਾਰ)
    {
	/*
	 * collect the items in items[]
	 */
	ਵਸਤੂ_ਗਿਣਤੀ = 0;
	ਲਈ (i = 0; i < tc_len; i++)
	{
	    len = show_one_termcode(termcodes[i].name,
						    termcodes[i].ਕੋਡ, FALSE);
	    ਜੇ ((flags & OPT_ONECOLUMN) ||
		    (len <= INC3 - GAP ? ਲੜੀਵਾਰ == 1
			: len <= INC2 - GAP ? ਲੜੀਵਾਰ == 2
			: ਲੜੀਵਾਰ == 3))
		ਚੀਜ਼ਾਂ[ਵਸਤੂ_ਗਿਣਤੀ++] = i;
	}

	/*
	 * display the items
	 */
	ਜੇ (ਲੜੀਵਾਰ <= 2)
	{
	    ਖਾਨੇ_ਗਿਣਤੀ = (Columns + GAP) / (ਲੜੀਵਾਰ == 1 ? INC3 : INC2);
	    ਜੇ (ਖਾਨੇ_ਗਿਣਤੀ == 0)
		ਖਾਨੇ_ਗਿਣਤੀ = 1;
	    ਪੰਕਤੀਆਂ_2 = (ਵਸਤੂ_ਗਿਣਤੀ + ਖਾਨੇ_ਗਿਣਤੀ - 1) / ਖਾਨੇ_ਗਿਣਤੀ;
	}
	ਨਹੀਂਤਾਂ	// run == 3
	    ਪੰਕਤੀਆਂ_2 = ਵਸਤੂ_ਗਿਣਤੀ;
	ਲਈ (ਪੰਕਤੀ = 0; ਪੰਕਤੀ < ਪੰਕਤੀਆਂ_2 && !got_int; ++ਪੰਕਤੀ)
	{
	    ਸੁਨੇਹਾ_putchar('\n');			// go to next line
	    ਜੇ (got_int)			// 'q' typed in more
		ਰੋਕੋ;
	    ਖਾਨਾ = 0;
	    ਲਈ (i = ਪੰਕਤੀ; i < ਵਸਤੂ_ਗਿਣਤੀ; i += ਪੰਕਤੀਆਂ_2)
	    {
		ਸੁਨੇਹਾ_ਖਾਨਾ = ਖਾਨਾ;			// make columns
		show_one_termcode(termcodes[ਚੀਜ਼ਾਂ[i]].name,
					      termcodes[ਚੀਜ਼ਾਂ[i]].ਕੋਡ, TRUE);
		ਜੇ (ਲੜੀਵਾਰ == 2)
		    ਖਾਨਾ += INC2;
		ਨਹੀਂਤਾਂ
		    ਖਾਨਾ += INC3;
	    }
	    ਬਾਹਰਲਾ_flush();
	    ui_breakcheck();
	}
    }
    vim_free(ਚੀਜ਼ਾਂ);
}

/*
 * Show one termcode entry.
 * Output goes into IObuff[]
 */
    ਪੂਰਨ
show_one_termcode(char_u *name, char_u *ਕੋਡ, ਪੂਰਨ printit)
{
    char_u	*p;
    ਪੂਰਨ		len;

    ਜੇ (name[0] > '~')
    {
	IObuff[0] = ' ';
	IObuff[1] = ' ';
	IObuff[2] = ' ';
	IObuff[3] = ' ';
    }
    ਨਹੀਂਤਾਂ
    {
	IObuff[0] = 't';
	IObuff[1] = '_';
	IObuff[2] = name[0];
	IObuff[3] = name[1];
    }
    IObuff[4] = ' ';

    p = get_special_ਕੁੰਜੀ_name(TERMCAP2_ਕੁੰਜੀ(name[0], name[1]), 0);
    ਜੇ (p[1] != 't')
	STRCPY(IObuff + 5, p);
    ਨਹੀਂਤਾਂ
	IObuff[5] = NUL;
    len = (ਪੂਰਨ)STRLEN(IObuff);
    ਕਰੋ
	IObuff[len++] = ' ';
    ਜਦੋਂ (len < 17);
    IObuff[len] = NUL;
    ਜੇ (ਕੋਡ == ਨਿਰਾ)
	len += 4;
    ਨਹੀਂਤਾਂ
	len += vim_strsize(ਕੋਡ);

    ਜੇ (printit)
    {
	ਸੁਨੇਹਾ_puts((ਅੱਖਰ *)IObuff);
	ਜੇ (ਕੋਡ == ਨਿਰਾ)
	    ਸੁਨੇਹਾ_puts("NULL");
	ਨਹੀਂਤਾਂ
	    ਸੁਨੇਹਾ_outtrans(ਕੋਡ);
    }
    ਵਾਪਸ len;
}

#ਜੇ defined(FEAT_TERMRESPONSE)
/*
 * For Xterm >= 140 compiled with OPT_TCAP_QUERY: Obtain the actually used
 * termcap codes from the terminal itself.
 * We get them one by one to avoid a very long response string.
 */
ਸਥਿਰ ਪੂਰਨ xt_ਸੂਚਕਾਂਕ_ਵਿੱਚ = 0;
ਸਥਿਰ ਪੂਰਨ xt_ਸੂਚਕਾਂਕ_ਬਾਹਰਲਾ = 0;

    ਸਥਿਰ ਖਾਲੀ
req_codes_from_ਪਦ(ਖਾਲੀ)
{
    xt_ਸੂਚਕਾਂਕ_ਬਾਹਰਲਾ = 0;
    xt_ਸੂਚਕਾਂਕ_ਵਿੱਚ = 0;
    req_ਹੋਰ_codes_from_ਪਦ();
}

    ਸਥਿਰ ਖਾਲੀ
req_ਹੋਰ_codes_from_ਪਦ(ਖਾਲੀ)
{
    ਅੱਖਰ	buf[32];  // extra size to shut up LGTM
    ਪੂਰਨ		old_idx = xt_ਸੂਚਕਾਂਕ_ਬਾਹਰਲਾ;

    // Don't do anything when going to exit.
    ਜੇ (exiting)
	ਵਾਪਸ;

    // Send up to 10 more requests out than we received.  Avoid sending too
    // many, there can be a buffer overflow somewhere.
    ਜਦੋਂ (xt_ਸੂਚਕਾਂਕ_ਬਾਹਰਲਾ < xt_ਸੂਚਕਾਂਕ_ਵਿੱਚ + 10 && ਕੁੰਜੀ_ਨਾਂ[xt_ਸੂਚਕਾਂਕ_ਬਾਹਰਲਾ] != ਨਿਰਾ)
    {
	ਅੱਖਰ *ਕੁੰਜੀ_name = ਕੁੰਜੀ_ਨਾਂ[xt_ਸੂਚਕਾਂਕ_ਬਾਹਰਲਾ];

	MAY_WANT_ਮੰਜ਼ਿਲ_LOG_THIS;
	LOG_TRN("Requesting XT %d: %s", xt_ਸੂਚਕਾਂਕ_ਬਾਹਰਲਾ, ਕੁੰਜੀ_name);
	ਜੇ (ਕੁੰਜੀ_name[2] != NUL)
	    sprintf(buf, "\033P+q%02x%02x%02x\033\\", ਕੁੰਜੀ_name[0], ਕੁੰਜੀ_name[1], ਕੁੰਜੀ_name[2]);
	ਨਹੀਂਤਾਂ
	    sprintf(buf, "\033P+q%02x%02x\033\\", ਕੁੰਜੀ_name[0], ਕੁੰਜੀ_name[1]);
	ਬਾਹਰਲਾ_str_nf((char_u *)buf);
	++xt_ਸੂਚਕਾਂਕ_ਬਾਹਰਲਾ;
    }

    // Send the codes out right away.
    ਜੇ (xt_ਸੂਚਕਾਂਕ_ਬਾਹਰਲਾ != old_idx)
	ਬਾਹਰਲਾ_flush();
}

/*
 * Decode key code response from xterm:
 * '<Esc>P1+r<name>=<string><Esc>\' if it is enabled/supported
 * '<Esc>P0+r<Esc>\'                if it not enabled
 * A "0" instead of the "1" indicates a code that isn't supported.
 * Both <name> and <string> are encoded in hex.
 * "code" points to the "0" or "1".
 */
    ਸਥਿਰ ਖਾਲੀ
got_ਕੋਡ_from_ਪਦ(char_u *ਕੋਡ, ਪੂਰਨ len)
{
# define XT_LEN 100
    char_u	name[4];
    char_u	str[XT_LEN];
    ਪੂਰਨ		i;
    ਪੂਰਨ		j = 0;
    ਪੂਰਨ		c;

    // A '1' means the code is supported, a '0' means it isn't.
    // If it is supported, there must be a '=' following
    // When half the length is > XT_LEN we can't use it.
    ਜੇ (ਕੋਡ[0] == '1' && (ਕੋਡ[7] == '=' || ਕੋਡ[9] == '=') && len / 2 < XT_LEN)
    {
	// Get the name from the response and find it in the table.
	name[0] = hexhex2nr(ਕੋਡ + 3);
	name[1] = hexhex2nr(ਕੋਡ + 5);
	ਜੇ (ਕੋਡ[9] == '=')
	    name[2] = hexhex2nr(ਕੋਡ + 7);
	ਨਹੀਂਤਾਂ
	    name[2] = NUL;
	name[3] = NUL;
	ਲਈ (i = 0; ਕੁੰਜੀ_ਨਾਂ[i] != ਨਿਰਾ; ++i)
	{
	    ਜੇ (STRCMP(ਕੁੰਜੀ_ਨਾਂ[i], name) == 0)
	    {
		xt_ਸੂਚਕਾਂਕ_ਵਿੱਚ = i;
		ਰੋਕੋ;
	    }
	}

	LOG_TRN("Received XT %d: %s", xt_ਸੂਚਕਾਂਕ_ਵਿੱਚ, (ਅੱਖਰ *)name);

	ਜੇ (ਕੁੰਜੀ_ਨਾਂ[i] != ਨਿਰਾ)
	{
	    i = (ਕੋਡ[7] == '=') ? 8 : 10;
	    ਲਈ (; (c = hexhex2nr(ਕੋਡ + i)) >= 0; i += 2)
		str[j++] = c;
	    str[j] = NUL;
	    ਜੇ (name[0] == 'C' && name[1] == 'o')
	    {
		// Color count is not a key code.
		ਪੂਰਨ val = atoi((ਅੱਖਰ *)str);
# ਜੇ defined(FEAT_EVAL)
		ਜੇ (val == t_colors)
		    ch_log(ਨਿਰਾ, "got_code_from_term(Co): no change (%d)", val);
		ਨਹੀਂਤਾਂ
		    ch_log(ਨਿਰਾ,
			       "got_code_from_term(Co): changed from %d to %d",
								t_colors, val);
# endif
		may_adjust_color_ਗਿਣਤੀ(val);
	    }
# ਜੇ 0
#  ifdef FEAT_TERMGUICOLORS
	    // when RGB result comes back, it is supported when the result contains an '='
	    ਨਹੀਂਤਾਂ ਜੇ (name[0] == 'R' && name[1] == 'G' && name[2] == 'B' && ਕੋਡ[9] == '=')
	    {
		ਪੂਰਨ val = atoi((ਅੱਖਰ *)str);
		// only enable it, if termguicolors hasn't been set yet and
		// there are 8 bits per color channel
		ਜੇ (val == 8 && !p_tgc_ਸੈੱਟ)
		{
#   ifdef FEAT_EVAL
		    ch_log(ਨਿਰਾ, "got_code_from_term(RGB): xterm-direct colors detected");
#   endif
		    // RGB capability set, enable termguicolors
		    ਸੈੱਟ_option_ਮੁੱਲ((char_u *)"termguicolors", 1L, ਨਿਰਾ, 0);
		}
	    }
#  endif
# endif
	    ਨਹੀਂਤਾਂ
	    {
		i = ਲੱਭੋ_ਪਦ_bykeys(str, ਨਿਰਾ);
		ਜੇ (i >= 0 && name[0] == termcodes[i].name[0]
					    && name[1] == termcodes[i].name[1])
		{
		    // Existing entry with the same name and code - skip.
# ifdef FEAT_EVAL
		    ch_log(ਨਿਰਾ, "got_code_from_term(): Entry %c%c did not change",
							     name[0], name[1]);
# endif
		}
		ਨਹੀਂਤਾਂ ਜੇ (i >= 0 && name[0] == 'K' && VIM_ISDIGIT(name[1]))
		{
		    // Would replace existing entry with keypad key - skip.
# ifdef FEAT_EVAL
		    ch_log(ਨਿਰਾ, "got_code_from_term(): Skipping entry %c%c in favor of %c%c with matching keys %s",
			    name[0], name[1], termcodes[i].name[0], termcodes[i].name[1], str);
# endif
		}
		ਨਹੀਂਤਾਂ
		{
		    ਜੇ (i >= 0)
		    {
			// Delete an existing entry using the same code.
# ifdef FEAT_EVAL
			ch_log(ਨਿਰਾ, "got_code_from_term(): Deleting entry %c%c with matching keys %s",
			      termcodes[i].name[0], termcodes[i].name[1], str);
# endif
			del_termcode_idx(i);
		    }
# ifdef FEAT_EVAL
		    ਨਹੀਂਤਾਂ
			ch_log(ਨਿਰਾ, "got_code_from_term(): Adding entry %c%c with keys %s",
							name[0], name[1], str);
# endif
		    ਜੋੜੋ_termcode(name, str, ATC_FROM_ਪਦ);
		}
	    }
	}
    }

    // May request more codes now that we received one.
    ++xt_ਸੂਚਕਾਂਕ_ਵਿੱਚ;
    req_ਹੋਰ_codes_from_ਪਦ();
}

/*
 * Check if there are any unanswered requests and deal with them.
 * This is called before starting an external program or getting direct
 * keyboard input.  We don't want responses to be send to that program or
 * handled as typed text.
 */
    ਸਥਿਰ ਖਾਲੀ
check_for_codes_from_ਪਦ(ਖਾਲੀ)
{
    ਪੂਰਨ		c;

    // If no codes requested or all are answered, no need to wait.
    ਜੇ (xt_ਸੂਚਕਾਂਕ_ਬਾਹਰਲਾ == 0 || xt_ਸੂਚਕਾਂਕ_ਬਾਹਰਲਾ == xt_ਸੂਚਕਾਂਕ_ਵਿੱਚ)
	ਵਾਪਸ;

    // Vgetc() will check for and handle any response.
    // Keep calling vpeekc() until we don't get any responses.
    ++no_mapping;
    ++allow_ਕੁੰਜੀਆਂ;
    ਲਈ (;;)
    {
	c = vpeekc();
	ਜੇ (c == NUL)	    // nothing available
	    ਰੋਕੋ;

	// If a response is recognized it's replaced with K_IGNORE, must read
	// it from the input stream.  If there is no K_IGNORE we can't do
	// anything, break here (there might be some responses further on, but
	// we don't want to throw away any typed chars).
	ਜੇ (c != K_SPECIAL && c != K_IGNORE)
	    ਰੋਕੋ;
	c = vgetc();
	ਜੇ (c != K_IGNORE)
	{
	    vungetc(c);
	    ਰੋਕੋ;
	}
    }
    --no_mapping;
    --allow_ਕੁੰਜੀਆਂ;
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
ਸਥਿਰ ਅੱਖਰ ksme_str[20];
ਸਥਿਰ ਅੱਖਰ ksmr_str[20];
ਸਥਿਰ ਅੱਖਰ ksmd_str[20];

/*
 * For Win32 console: update termcap codes for existing console attributes.
 */
    ਖਾਲੀ
update_tcap(ਪੂਰਨ attr)
{
    sprintf(ksme_str, "\033|%dm", attr);
    sprintf(ksmd_str, "\033|%dm", attr | 0x08);  // FOREGROUND_INTENSITY
    sprintf(ksmr_str, "\033|%dm", ((attr & 0x0F) << 4) | ((attr & 0xF0) >> 4));

    tcap_ਇੰਦਰਾਜ_T *p = ਲੱਭੋ_builtin_ਪਦ(DEFAULT_ਪਦ);
    ਜੇ (p == ਨਿਰਾ)  // did not find it
	ਵਾਪਸ;
    ਜਦੋਂ (p->bt_string != ਨਿਰਾ)
    {
	ਜੇ (p->bt_ਇੰਦਰਾਜ == (ਪੂਰਨ)KS_ME)
	    p->bt_string = &ksme_str[0];
	ਨਹੀਂਤਾਂ ਜੇ (p->bt_ਇੰਦਰਾਜ == (ਪੂਰਨ)KS_MR)
	    p->bt_string = &ksmr_str[0];
	ਨਹੀਂਤਾਂ ਜੇ (p->bt_ਇੰਦਰਾਜ == (ਪੂਰਨ)KS_MD)
	    p->bt_string = &ksmd_str[0];
	++p;
    }
}

# ifdef FEAT_TERMGUICOLORS
#  define KSSIZE 20

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ
{
    CMODE_ਸੂਚਕਾਂਕਿਤ = 0,	// Use cmd.exe 4bit palette.
    CMODE_RGB,		// Use 24bit RGB colors using VTP.
    CMODE_256_ਖਾਨਾ,	// Emulate xterm's 256-color palette using VTP.
    CMODE_ਆਖਰੀ,
} cmode_T;

ਬਣਤਰ ks_tbl_S
{
    ਪੂਰਨ  ਕੋਡ;				// value of KS_
    ਅੱਖਰ *vtp;				// code in RGB mode
    ਅੱਖਰ *vtp2;				// code in 256color mode
    ਅੱਖਰ buf[CMODE_ਆਖਰੀ][KSSIZE];	// real buffer
};

ਸਥਿਰ ਬਣਤਰ ks_tbl_S ks_tbl[] =
{
    {(ਪੂਰਨ)KS_ME,  "\033|0m",  "\033|0m", {""}},   //ਸਾਧਾਰਨ
    {(ਪੂਰਨ)KS_MR,  "\033|7m",  "\033|7m", {""}},   //ਉਲਟ
    {(ਪੂਰਨ)KS_MD,  "\033|1m",  "\033|1m", {""}},   // bold
    {(ਪੂਰਨ)KS_SO,  "\033|91m", "\033|91m", {""}},  // standout: bright red text
    {(ਪੂਰਨ)KS_SE,  "\033|39m", "\033|39m", {""}},  // standout end: default color
    {(ਪੂਰਨ)KS_CZH, "\033|3m",  "\033|3m", {""}},   // italic
    {(ਪੂਰਨ)KS_CZR, "\033|0m",  "\033|0m", {""}},   // italic end
    {(ਪੂਰਨ)KS_US,  "\033|4m",  "\033|4m", {""}},   // underscore
    {(ਪੂਰਨ)KS_UE,  "\033|24m", "\033|24m", {""}},  // underscore end
#  ifdef TERMINFO
    {(ਪੂਰਨ)KS_CAB, "\033|%p1%db", "\033|%p14%dm", {""}}, // set background color
    {(ਪੂਰਨ)KS_CAF, "\033|%p1%df", "\033|%p13%dm", {""}}, // set foreground color
    {(ਪੂਰਨ)KS_CS,  "\033|%p1%d;%p2%dR", "\033|%p1%d;%p2%dR", {""}},
    {(ਪੂਰਨ)KS_CSV, "\033|%p1%d;%p2%dV", "\033|%p1%d;%p2%dV", {""}},
#  ਨਹੀਂਤਾਂ
    {(ਪੂਰਨ)KS_CAB, "\033|%db", "\033|4%dm", {""}}, // set background color
    {(ਪੂਰਨ)KS_CAF, "\033|%df", "\033|3%dm", {""}}, // set foreground color
    {(ਪੂਰਨ)KS_CS,  "\033|%d;%dR", "\033|%d;%dR", {""}},
    {(ਪੂਰਨ)KS_CSV, "\033|%d;%dV", "\033|%d;%dV", {""}},
#  endif
    {(ਪੂਰਨ)KS_CCO, "256", "256", {""}},	    // colors
    {(ਪੂਰਨ)KS_NAME, ਨਿਰਾ, ਨਿਰਾ, {""}}			    // terminator
};

/*
 * Find the first entry for "code" in the builtin termcap for "name".
 * Returns NULL when not found.
 */
    ਸਥਿਰ tcap_ਇੰਦਰਾਜ_T *
ਲੱਭੋ_ਪਹਿਲਾ_tcap(
    char_u *name,
    ਪੂਰਨ	    ਕੋਡ)
{
    tcap_ਇੰਦਰਾਜ_T *p = ਲੱਭੋ_builtin_ਪਦ(name);
    ਜੇ (p == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    ਜਦੋਂ (p->bt_string != ਨਿਰਾ)
    {
	ਜੇ (p->bt_ਇੰਦਰਾਜ == ਕੋਡ)
	    ਵਾਪਸ p;
	++p;
    }
    ਵਾਪਸ ਨਿਰਾ;
}
# endif

/*
 * For Win32 console: replace the sequence immediately after termguicolors.
 */
    ਖਾਲੀ
ਅਦਲਾ_ਬਦਲੀ_tcap(ਖਾਲੀ)
{
# ifdef FEAT_TERMGUICOLORS
    ਸਥਿਰ ਪੂਰਨ		init_ਪੂਰਾ_2 = FALSE;
    ਸਥਿਰ cmode_T	curr_ਬਹੁਲਕ;
    ਬਣਤਰ ks_tbl_S	*ks;
    cmode_T		ਬਹੁਲਕ;

    ਜੇ (!init_ਪੂਰਾ_2)
    {
	ਲਈ (ks = ks_tbl; ks->ਕੋਡ != (ਪੂਰਨ)KS_NAME; ks++)
	{
	    tcap_ਇੰਦਰਾਜ_T *bt = ਲੱਭੋ_ਪਹਿਲਾ_tcap(DEFAULT_ਪਦ, ks->ਕੋਡ);
	    ਜੇ (bt != ਨਿਰਾ)
	    {
		// Preserve the original value.
		STRNCPY(ks->buf[CMODE_ਸੂਚਕਾਂਕਿਤ], bt->bt_string, KSSIZE);
		STRNCPY(ks->buf[CMODE_RGB], ks->vtp, KSSIZE);
		STRNCPY(ks->buf[CMODE_256_ਖਾਨਾ], ks->vtp2, KSSIZE);

		bt->bt_string = ks->buf[CMODE_ਸੂਚਕਾਂਕਿਤ];
	    }
	}
	init_ਪੂਰਾ_2 = TRUE;
	curr_ਬਹੁਲਕ = CMODE_ਸੂਚਕਾਂਕਿਤ;
    }

    ਜੇ (p_tgc)
	ਬਹੁਲਕ = CMODE_RGB;
    ਨਹੀਂਤਾਂ ਜੇ (t_colors >= 256)
	ਬਹੁਲਕ = CMODE_256_ਖਾਨਾ;
    ਨਹੀਂਤਾਂ
	ਬਹੁਲਕ = CMODE_ਸੂਚਕਾਂਕਿਤ;

    ਜੇ (ਬਹੁਲਕ == curr_ਬਹੁਲਕ)
	ਵਾਪਸ;

    ਲਈ (ks = ks_tbl; ks->ਕੋਡ != (ਪੂਰਨ)KS_NAME; ks++)
    {
	tcap_ਇੰਦਰਾਜ_T *bt = ਲੱਭੋ_ਪਹਿਲਾ_tcap(DEFAULT_ਪਦ, ks->ਕੋਡ);
	ਜੇ (bt != ਨਿਰਾ)
	    bt->bt_string = ks->buf[ਬਹੁਲਕ];
    }

    curr_ਬਹੁਲਕ = ਬਹੁਲਕ;
# endif
}

#ਜੇ_ਅੰਤ


#ਜੇ (defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))) || defined(FEAT_TERMINAL)
ਸਥਿਰ ਅਚਲ ਪੂਰਨ cube_ਮੁੱਲ[] = {
    0x00, 0x5F, 0x87, 0xAF, 0xD7, 0xFF
};

ਸਥਿਰ ਅਚਲ ਪੂਰਨ grey_ramp[] = {
    0x08, 0x12, 0x1C, 0x26, 0x30, 0x3A, 0x44, 0x4E, 0x58, 0x62, 0x6C, 0x76,
    0x80, 0x8A, 0x94, 0x9E, 0xA8, 0xB2, 0xBC, 0xC6, 0xD0, 0xDA, 0xE4, 0xEE
};

ਸਥਿਰ ਅਚਲ char_u ansi_ਸਾਰਣੀ[16][3] = {
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

# ਜੇ defined(MSWIN)
// Mapping between cterm indices < 16 and their counterpart in the ANSI palette.
ਅਚਲ char_u cterm_ansi_idx[] = {
    0, 4, 2, 6, 1, 5, 3, 7, 8, 12, 10, 14, 9, 13, 11, 15
};
# endif

# define ANSI_ਸੂਚਕਾਂਕ_NONE 0

    ਖਾਲੀ
ansi_color2rgb(ਪੂਰਨ nr, char_u *r, char_u *g, char_u *b, char_u *ansi_idx)
{
    ਜੇ (nr < 16)
    {
	*r = ansi_ਸਾਰਣੀ[nr][0];
	*g = ansi_ਸਾਰਣੀ[nr][1];
	*b = ansi_ਸਾਰਣੀ[nr][2];
	*ansi_idx = nr + 1;
    }
    ਨਹੀਂਤਾਂ
    {
	*r = 0;
	*g = 0;
	*b = 0;
	*ansi_idx = ANSI_ਸੂਚਕਾਂਕ_NONE;
    }
}

    ਖਾਲੀ
cterm_color2rgb(ਪੂਰਨ nr, char_u *r, char_u *g, char_u *b, char_u *ansi_idx)
{
    ਪੂਰਨ idx;

    ਜੇ (nr < 16)
    {
# ਜੇ defined(MSWIN)
	idx = cterm_ansi_idx[nr];
# ਨਹੀਂਤਾਂ
	idx = nr;
# endif
	*r = ansi_ਸਾਰਣੀ[idx][0];
	*g = ansi_ਸਾਰਣੀ[idx][1];
	*b = ansi_ਸਾਰਣੀ[idx][2];
	*ansi_idx = idx + 1;
    }
    ਨਹੀਂਤਾਂ ਜੇ (nr < 232)
    {
	// 216 color cube
	idx = nr - 16;
	*r = cube_ਮੁੱਲ[idx / 36 % 6];
	*g = cube_ਮੁੱਲ[idx / 6  % 6];
	*b = cube_ਮੁੱਲ[idx      % 6];
	*ansi_idx = ANSI_ਸੂਚਕਾਂਕ_NONE;
    }
    ਨਹੀਂਤਾਂ ਜੇ (nr < 256)
    {
	// 24 grey scale ramp
	idx = nr - 232;
	*r = grey_ramp[idx];
	*g = grey_ramp[idx];
	*b = grey_ramp[idx];
	*ansi_idx = ANSI_ਸੂਚਕਾਂਕ_NONE;
    }
    ਨਹੀਂਤਾਂ
    {
	*r = 0;
	*g = 0;
	*b = 0;
	*ansi_idx = ANSI_ਸੂਚਕਾਂਕ_NONE;
    }
}
#ਜੇ_ਅੰਤ

/*
 * Replace K_BS by <BS> and K_DEL by <DEL>.
 * Include any modifiers into the key and drop them.
 * Returns "len" adjusted for replaced codes.
 */
    ਪੂਰਨ
ਪਦ_replace_keycodes(char_u *ta_buf, ਪੂਰਨ ta_len, ਪੂਰਨ len_arg)
{
    ਪੂਰਨ		len = len_arg;
    ਪੂਰਨ		i;
    ਪੂਰਨ		c;

    ਲਈ (i = ta_len; i < ta_len + len; ++i)
    {
	ਜੇ (ta_buf[i] == CSI && len - i > 3 && ta_buf[i + 1] == KS_MODIFIER)
	{
	    ਪੂਰਨ modifiers = ta_buf[i + 2];
	    ਪੂਰਨ ਕੁੰਜੀ = ta_buf[i + 3];

	    // Try to use the modifier to modify the key.  In any case drop the
	    // modifier.
	    mch_memmove(ta_buf + i + 1, ta_buf + i + 4, (size_t)(len - i - 3));
	    len -= 3;
	    ਜੇ (ਕੁੰਜੀ < 0x80)
		ਕੁੰਜੀ = ਮਿਲਾਓ_modify_Other_ਕੁੰਜੀਆਂ(ਕੁੰਜੀ, &modifiers);
	    ta_buf[i] = ਕੁੰਜੀ;
	}
	ਨਹੀਂਤਾਂ ਜੇ (ta_buf[i] == CSI && len - i > 2)
	{
	    c = TERMCAP2_ਕੁੰਜੀ(ta_buf[i + 1], ta_buf[i + 2]);
	    ਜੇ (c == K_DEL || c == K_KDEL || c == K_BS)
	    {
		mch_memmove(ta_buf + i + 1, ta_buf + i + 3,
							(size_t)(len - i - 2));
		ਜੇ (c == K_DEL || c == K_KDEL)
		    ta_buf[i] = DEL;
		ਨਹੀਂਤਾਂ
		    ta_buf[i] = Ctrl_H;
		len -= 2;
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (ta_buf[i] == '\r')
	    ta_buf[i] = '\n';
	ਜੇ (has_mbyte)
	    i += (*mb_ptr2len_len)(ta_buf + i, ta_len + len - i) - 1;
    }
    ਵਾਪਸ len;
}

/*
 * Should be called when cleaning up terminal state.
 */
    ਖਾਲੀ
ਪਦ_disable_dec(ਖਾਲੀ)
{
    ਪਦ_ਸੈੱਟ_sync_output(ਪਦ_SYNC_OUTPUT_OFF);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
    ਪਦ_ਸੈੱਟ_win_resize(false);
#ਜੇ_ਅੰਤ
    // Make sure to always flush the output buffer, because this may be called
    // before starting the GUI
    ਬਾਹਰਲਾ_flush();
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
/*
 * Enable or disable receiving in-band window resize events from the terminal.
 * If "state" is true, then if the terminal supports DEC mode 2048 and
 * 'termresize' is "" or "inband", then enable it and disable the SIGWINCH
 * signal handling. Otherwise disable the mode if it is enabled and reinstall
 * the SIGWINCH handler.
 */
    ਖਾਲੀ
ਪਦ_ਸੈੱਟ_win_resize(bool state)
{
# ifdef FEAT_GUI
    bool    ਵਿੱਚ_gui = gui.ਵਿੱਚ_use;

    ਜੇ (state && ਵਿੱਚ_gui)
	ਵਾਪਸ;
# endif

    ਜੇ (!state || win_resize_setting == 0 || win_resize_setting == 4)
    {
	// Make sure it update internal window size if DEC mode 2048 is
	// unavailable now.
	ਜੇ (win_resize_enabled)
	{
	    ਸੈੱਟ_shellsize(0, 0, false);
	    ਸੈੱਟ_sigwinch_handler();
	    ਬਾਹਰਲਾ_str((char_u *)"\033[?2048l");
	}
	win_resize_enabled = false;
    }
    ਨਹੀਂਤਾਂ ਜੇ ((*p_trz == NUL || STRCMP(p_trz, "inband") == 0)
	    && !win_resize_enabled)
    {
	ਜੇ (win_resize_setting == 2)
	    ਬਾਹਰਲਾ_str((char_u *)"\033[?2048h");
# ifdef SIGWINCH
	mch_signal(SIGWINCH, SIG_DFL);
# endif
	win_resize_enabled = true;
    }
}
#ਜੇ_ਅੰਤ

    ਪੂਰਨ
sync_output_ਸਰਗਰਮ(ਖਾਲੀ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    ਜੇ (gui.ਵਿੱਚ_use)
	ਵਾਪਸ TRUE;
#ਜੇ_ਅੰਤ
    ਵਾਪਸ p_tsy && (sync_output_setting == 1 || sync_output_setting == 2)
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
    ਖਾਲੀ
ਪਦ_ਸੈੱਟ_sync_output(ਪੂਰਨ flags)
{
    bool    allowed;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    bool    ਵਿੱਚ_gui = gui.ਵਿੱਚ_use;
#ਨਹੀਂਤਾਂ
    bool    ਵਿੱਚ_gui = false;
#ਜੇ_ਅੰਤ

    allowed = p_tsy && (sync_output_setting == 1 || sync_output_setting == 2);

    ਜੇ (flags & ਪਦ_SYNC_OUTPUT_FLUSH)
    {
	// Tell terminal to display screen contents, then resume batching.
	ਜੇ (allowed && !ਵਿੱਚ_gui && sync_output_state > 0 && *T_ESU != NUL &&
		*T_BSU != NUL)
	{
	    ਬਾਹਰਲਾ_str_nf(T_ESU);
	    ਬਾਹਰਲਾ_str_nf(T_BSU);
	    ਬਾਹਰਲਾ_flush();
	}
	ਵਾਪਸ;
    }

    // Forcibly turn off synchronized output (e.g. 'notermsync')
    ਜੇ (flags & ਪਦ_SYNC_OUTPUT_OFF)
    {
	ਜੇ (sync_output_state > 0 && *T_ESU != NUL)
	{
	    ਬਾਹਰਲਾ_str_nf(T_ESU);
	    ਬਾਹਰਲਾ_flush();
	    sync_output_state = 0;
	}
	ਵਾਪਸ;
    }

    ਜੇ (!allowed || ਵਿੱਚ_gui || *T_BSU == NUL || *T_ESU == NUL)
	ਵਾਪਸ;

    // Only enable if we aren't already, and only disable if we have reached
    // zero.
    ਜੇ (flags & ਪਦ_SYNC_OUTPUT_ENABLE)
    {
	ਜੇ (sync_output_state++ > 0)
	    ਵਾਪਸ;
	ਬਾਹਰਲਾ_str_nf(T_BSU);
    }
    ਨਹੀਂਤਾਂ ਜੇ (flags & ਪਦ_SYNC_OUTPUT_DISABLE)
    {
	ਜੇ (sync_output_state == 0 || --sync_output_state > 0)
	    ਵਾਪਸ;
	// Flush the output buffer before ending the sync batch so that
	// all drawing output is sent to the terminal within the
	// BSU..ESU window.  Without this, the drawing data remaining in
	// out_buf would be sent after ESU, outside the sync batch.
	ਬਾਹਰਲਾ_str_nf(T_ESU);
	ਬਾਹਰਲਾ_flush();
    }
    ਨਹੀਂਤਾਂ
    {
	siemsg("Unknown sync output value %d", flags);
	ਵਾਪਸ;
    }
}
