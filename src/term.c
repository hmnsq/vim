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
#تعریف_کرو tgetstr tgetstr_defined_wrong

#شامل "vim.h"

#اگر_تعریف_شدہ HAVE_TGETENT
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
# اگر defined(VMS) || defined(AMIGA)
#  define TPUTSFUNCAST (خالی (*)(غیرعلامتی صحیح))
# ورنہ
#  ifdef HAVE_OUTFUNTYPE
#   define TPUTSFUNCAST (outfuntype)
#  ورنہ
#   define TPUTSFUNCAST (صحیح (*)(صحیح))
#  endif
# endif
#اگر_ختم

#تعریف_منسوخ tgetstr

// start of keys that are not directly used by Vim but can be mapped
#تعریف_کرو BT_EXTRA_کلیدیں	0x101

ساکن خالی parse_builtin_tcap(char_u *s);
ساکن خالی gather_termleader(خالی);
#اگر_تعریف_شدہ FEAT_TERMRESPONSE
ساکن خالی req_codes_from_رکن(خالی);
ساکن خالی req_مزید_codes_from_رکن(خالی);
ساکن خالی got_رمز_from_رکن(char_u *رمز, صحیح len);
ساکن خالی check_for_codes_from_رکن(خالی);
#اگر_ختم
ساکن خالی del_termcode_idx(صحیح idx);
ساکن صحیح ڈھونڈو_رکن_bykeys(char_u *src, صحیح *len);
ساکن صحیح رکن_is_builtin(char_u *name);
ساکن صحیح رکن_7to8bit(char_u *p);
ساکن خالی accept_modifiers_for_function_کلیدیں(خالی);

#اگر 0  // Change to 1 to enable ch_log() calls for termresponse debugging.
# define DEBUG_TERMRESPONSE
# define LOG_TR1(str) \
		ch_log(تہی, "TermResp: %s " str, \
			must_redraw == UPD_NOT_VALID ? "NV" \
			: must_redraw == UPD_CLEAR ? "CL" : "  ")
# define LOG_TRN(fmt,...) \
		ch_log(تہی, "TermResp: %s " fmt, \
			must_redraw == UPD_NOT_VALID ? "NV" \
			: must_redraw == UPD_CLEAR ? "CL" : "  ", __VA_ARGS__)
#ورنہ
# define LOG_TR1(str) کرو { /**/ } جب (0)
# define LOG_TRN(fmt,...) کرو { /**/ } جب (0)
#اگر_ختم

#اگر_تعریف_شدہ HAVE_TGETENT
ساکن حرف *invoke_tgetent(char_u *, char_u *);

/*
 * Here is our own prototype for tgetstr(), any prototypes from the include
 * files have been disabled by the define at the start of this file.
 */
حرف		*tgetstr(حرف *, حرف **);
#اگر_ختم

قسمتعریف شمار {
    حالت_GET,		// send request when switching to RAW mode
    حالت_بھیجا,	// did send request, checking for response
    حالت_GOT,		// received response
    حالت_FAIL		// timed out
} request_پیش_رفت_T;

قسمتعریف ساخت {
    request_پیش_رفت_T	    tr_پیش_رفت;
    time_t		    tr_آغاز;	// when request was sent, -1 for never
} termrequest_T;

// Holds state for current OSC response.
قسمتعریف ساخت
{
    صحیح		    processing;	// If we are in the middle of an OSC response
    char_u	    آغاز_char;	// First char in the OSC response
    garray_T	    buf;	// Buffer holding the OSC response, to be
				// placed in the "v:termosc" vim var.

#اگر_تعریف_شدہ ELAPSED_FUNC
    elapsed_T	    آغاز_tv;	// Set at the beginning of an OSC response.
				// Used to timeout after a set amount of
				// time.
#اگر_ختم
} oscstate_T;

#تعریف_کرو TERMREQUEST_INIT {حالت_GET, -1}

// Request Terminal Version status:
ساکن termrequest_T crv_حالت = TERMREQUEST_INIT;

// Request Cursor position report:
ساکن termrequest_T u7_حالت = TERMREQUEST_INIT;

// Request xterm compatibility check:
ساکن termrequest_T xcc_حالت = TERMREQUEST_INIT;

#اگر_تعریف_شدہ FEAT_TERMRESPONSE
# ifdef FEAT_TERMINAL
// Request foreground color report:
ساکن termrequest_T rfg_حالت = TERMREQUEST_INIT;
ساکن صحیح fg_r = 0;
ساکن صحیح fg_g = 0;
ساکن صحیح fg_b = 0;
# endif
// Background color values from the OSC 11 response, also used by the
// image backend to flatten RGBA alpha onto the actual terminal background.
ساکن صحیح bg_r = 255;
ساکن صحیح bg_g = 255;
ساکن صحیح bg_b = 255;

// Request background color report:
ساکن termrequest_T rbg_حالت = TERMREQUEST_INIT;

// Request cursor blinking mode report:
ساکن termrequest_T rbm_حالت = TERMREQUEST_INIT;

// Request cursor style report:
ساکن termrequest_T rcs_حالت = TERMREQUEST_INIT;

// Request window's position report:
ساکن termrequest_T winpos_حالت = TERMREQUEST_INIT;

// Request DECRQM (DEC mode) report:
ساکن termrequest_T decrqm_حالت = TERMREQUEST_INIT;

ساکن termrequest_T *all_termrequests[] = {
    &crv_حالت,
    &u7_حالت,
    &xcc_حالت,
# ifdef FEAT_TERMINAL
    &rfg_حالت,
# endif
    &rbg_حالت,
    &rbm_حالت,
    &rcs_حالت,
    &winpos_حالت,
    &decrqm_حالت,
    تہی
};

// The t_8u code may default to a value but get reset when the term response is
// received.  To avoid redrawing too often, only redraw when t_8u is not reset
// and it was supposed to be written.  Unless t_8u was set explicitly.
// FALSE -> don't output t_8u yet
// MAYBE -> tried outputting t_8u while FALSE
// OK    -> can write t_8u
صحیح write_t_8u_state = FALSE;
#اگر_ختم

#اگر_تعریف_شدہ HAVE_TGETENT
/*
 * Don't declare these variables if termcap.h contains them.
 * Autoconf checks if these variables should be declared extern (not all
 * systems have them).
 * Some versions define ospeed to be speed_t, but that is incompatible with
 * BSD, where ospeed is short and speed_t is long.
 */
# ifndef HAVE_OSPEED
#  ifdef OSPEED_EXTERN
خارجی مختصر ospeed;
#  ورنہ
مختصر ospeed;
#  endif
# endif
# ifndef HAVE_اوپر_BC_PC
#  ifdef اوپر_BC_PC_EXTERN
خارجی حرف *اوپر_2, *BC, PC;
#  ورنہ
حرف *اوپر_2, *BC, PC;
#  endif
# endif

# define TGETSTR(s, p)	vim_tgetstr((s), (p))
# define TGETENT(b, t)	tgetent((حرف *)(b), (حرف *)(t))
ساکن char_u *vim_tgetstr(حرف *s, char_u **pp);
#اگر_ختم // HAVE_TGETENT

ساکن صحیح  detected_8bit = FALSE;	// detected 8-bit terminal

#اگر (defined(UNIX) || defined(VMS))
ساکن صحیح focus_state = MAYBE; // TRUE if the Vim window has focus
#اگر_ختم

#اگر_تعریف_شدہ FEAT_TERMRESPONSE
// When the cursor shape was detected these values are used:
// 1: block, 2: underline, 3: vertical bar
ساکن صحیح initial_cursor_shape = 0;

// The blink flag from the style response may be inverted from the actual
// blinking state, xterm XORs the flags.
ساکن صحیح initial_cursor_shape_blink = FALSE;

// The blink flag from the blinking-cursor mode response
ساکن صحیح initial_cursor_blink = FALSE;
#اگر_ختم

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
ساکن صحیح sync_output_setting = 0;

// > 0: Currently batching output
// == 0: No synchronized output
ساکن صحیح sync_output_state = 0;

#اگر_تعریف_شدہ UNIX
// DEC mode 2048 (in-band window resize events)
// https://gist.github.com/rockorager/e695fb2924d36b2bcf1fff4a3704bd83
ساکن صحیح win_resize_setting = 0;
ساکن bool win_resize_enabled = false;
#اگر_ختم

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
قسمتعریف ساخت
{
    صحیح		bt_اندراج;   // either a KS_xxx code (>= 0), or a K_xxx code.
    حرف	*bt_string; //قدر
} tcap_اندراج_T;

/*
 * Standard ANSI terminal, default for Unix.
 */
ساکن tcap_اندراج_T builtin_ansi[] = {
    {(صحیح)KS_CE,	"\033[K"},
    {(صحیح)KS_AL,	"\033[L"},
#اگر_تعریف_شدہ TERMINFO
    {(صحیح)KS_CAL,	"\033[%p1%dL"},
#ورنہ
    {(صحیح)KS_CAL,	"\033[%dL"},
#اگر_ختم
    {(صحیح)KS_DL,	"\033[M"},
#اگر_تعریف_شدہ TERMINFO
    {(صحیح)KS_CDL,	"\033[%p1%dM"},
#ورنہ
    {(صحیح)KS_CDL,	"\033[%dM"},
#اگر_ختم
    {(صحیح)KS_CL,	"\033[H\033[2J"},
    {(صحیح)KS_ME,	"\033[0m"},
    {(صحیح)KS_MR,	"\033[7m"},
    {(صحیح)KS_MS,	"y"},
    {(صحیح)KS_UT,	"y"},		// guessed
    {(صحیح)KS_LE,	"\b"},
#اگر_تعریف_شدہ TERMINFO
    {(صحیح)KS_CM,	"\033[%i%p1%d;%p2%dH"},
#ورنہ
    {(صحیح)KS_CM,	"\033[%i%d;%dH"},
#اگر_ختم
#اگر_تعریف_شدہ TERMINFO
    {(صحیح)KS_CRI,	"\033[%p1%dC"},
#ورنہ
    {(صحیح)KS_CRI,	"\033[%dC"},
#اگر_ختم

    {(صحیح)KS_NAME,	تہی}  // end marker
};

/*
 * VT320 is working as an ANSI terminal compatible DEC terminal.
 * (it covers VT1x0, VT2x0 and VT3x0 up to VT320 on VMS as well)
 * TODO:- rewrite ESC[ codes to CSI
 *      - keyboard languages (CSI ? 26 n)
 */
ساکن tcap_اندراج_T builtin_vt320[] = {
    {(صحیح)KS_CE,	"\033[K"},
    {(صحیح)KS_AL,	"\033[L"},
#اگر_تعریف_شدہ TERMINFO
    {(صحیح)KS_CAL,	"\033[%p1%dL"},
#ورنہ
    {(صحیح)KS_CAL,	"\033[%dL"},
#اگر_ختم
    {(صحیح)KS_DL,	"\033[M"},
#اگر_تعریف_شدہ TERMINFO
    {(صحیح)KS_CDL,	"\033[%p1%dM"},
#ورنہ
    {(صحیح)KS_CDL,	"\033[%dM"},
#اگر_ختم
    {(صحیح)KS_CL,	"\033[H\033[2J"},
    {(صحیح)KS_CD,	"\033[J"},
    {(صحیح)KS_CCO,	"8"},			// allow 8 colors
    {(صحیح)KS_ME,	"\033[0m"},
    {(صحیح)KS_MR,	"\033[7m"},
    {(صحیح)KS_MD,	"\033[1m"},  // bold mode
    {(صحیح)KS_SE,	"\033[22m"},// normal mode
    {(صحیح)KS_UE,	"\033[24m"},// exit underscore mode
    {(صحیح)KS_US,	"\033[4m"},  // underscore mode
    {(صحیح)KS_CZH,	"\033[34;43m"},  // italic mode: blue text on yellow
    {(صحیح)KS_CZR,	"\033[0m"},	    // italic mode end
    {(صحیح)KS_CAB,	"\033[4%dm"},	    // set background color (ANSI)
    {(صحیح)KS_CAF,	"\033[3%dm"},	    // set foreground color (ANSI)
    {(صحیح)KS_CSB,	"\033[102;%dm"},	// set screen background color
    {(صحیح)KS_CSF,	"\033[101;%dm"},	// set screen foreground color
    {(صحیح)KS_MS,	"y"},
    {(صحیح)KS_UT,	"y"},
    {(صحیح)KS_XN,	"y"},
    {(صحیح)KS_LE,	"\b"},
#اگر_تعریف_شدہ TERMINFO
    {(صحیح)KS_CM,	"\033[%i%p1%d;%p2%dH"},
#ورنہ
    {(صحیح)KS_CM,	"\033[%i%d;%dH"},
#اگر_ختم
#اگر_تعریف_شدہ TERMINFO
    {(صحیح)KS_CRI,	"\033[%p1%dC"},
#ورنہ
    {(صحیح)KS_CRI,	"\033[%dC"},
#اگر_ختم
    {K_اوپر,		"\033[A"},
    {K_نیچے_کو,		"\033[B"},
    {K_دایاں,		"\033[C"},
    {K_بایاں,		"\033[D"},
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
    {K_انتہا,		"\033[4~"},
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

    {(صحیح)KS_NAME,	تہی}  // end marker
};

/*
 * Ordinary vt52
 */
ساکن tcap_اندراج_T builtin_vt52[] = {
    {(صحیح)KS_CE,	"\033K"},
    {(صحیح)KS_CD,	"\033J"},
#اگر_تعریف_شدہ TERMINFO
    {(صحیح)KS_CM,	"\033Y%p1%' '%+%c%p2%' '%+%c"},
#ورنہ
    {(صحیح)KS_CM,	"\033Y%+ %+ "},
#اگر_ختم
    {(صحیح)KS_LE,	"\b"},
    {(صحیح)KS_SR,	"\033I"},
    {(صحیح)KS_AL,	"\033L"},
    {(صحیح)KS_DL,	"\033M"},
    {K_اوپر,		"\033A"},
    {K_نیچے_کو,		"\033B"},
    {K_بایاں,		"\033D"},
    {K_دایاں,		"\033C"},
    {K_F1,		"\033P"},
    {K_F2,		"\033Q"},
    {K_F3,		"\033R"},
    {(صحیح)KS_CL,	"\033H\033J"},
    {(صحیح)KS_MS,	"y"},

    {(صحیح)KS_NAME,	تہی}  // end marker
};

/*
 * Builtin xterm with Vim-specific entries.
 */
ساکن tcap_اندراج_T builtin_xterm[] = {
    {(صحیح)KS_CE,	"\033[K"},
    {(صحیح)KS_AL,	"\033[L"},
#اگر_تعریف_شدہ TERMINFO
    {(صحیح)KS_CAL,	"\033[%p1%dL"},
#ورنہ
    {(صحیح)KS_CAL,	"\033[%dL"},
#اگر_ختم
    {(صحیح)KS_DL,	"\033[M"},
#اگر_تعریف_شدہ TERMINFO
    {(صحیح)KS_CDL,	"\033[%p1%dM"},
#ورنہ
    {(صحیح)KS_CDL,	"\033[%dM"},
#اگر_ختم
#اگر_تعریف_شدہ TERMINFO
    {(صحیح)KS_CS,	"\033[%i%p1%d;%p2%dr"},
#ورنہ
    {(صحیح)KS_CS,	"\033[%i%d;%dr"},
#اگر_ختم
    {(صحیح)KS_CL,	"\033[H\033[2J"},
    {(صحیح)KS_CD,	"\033[J"},
    {(صحیح)KS_ME,	"\033[m"},
    {(صحیح)KS_MR,	"\033[7m"},
    {(صحیح)KS_MD,	"\033[1m"},
    {(صحیح)KS_UE,	"\033[m"},
    {(صحیح)KS_US,	"\033[4m"},
    {(صحیح)KS_STE,	"\033[29m"},
    {(صحیح)KS_STS,	"\033[9m"},
    {(صحیح)KS_MS,	"y"},
    {(صحیح)KS_UT,	"y"},
    {(صحیح)KS_LE,	"\b"},
    {(صحیح)KS_VI,	"\033[?25l"},
    {(صحیح)KS_VE,	"\033[?25h"},
    {(صحیح)KS_VS,	"\033[?12h"},
    {(صحیح)KS_CVS,	"\033[?12l"},
#اگر_تعریف_شدہ TERMINFO
    {(صحیح)KS_CSH,	"\033[%p1%d q"},
#ورنہ
    {(صحیح)KS_CSH,	"\033[%d q"},
#اگر_ختم
    {(صحیح)KS_چکری_جانچ,	"\033[?12$p"},
    {(صحیح)KS_CRS,	"\033P$q q\033\\"},
#اگر_تعریف_شدہ TERMINFO
    {(صحیح)KS_CM,	"\033[%i%p1%d;%p2%dH"},
#ورنہ
    {(صحیح)KS_CM,	"\033[%i%d;%dH"},
#اگر_ختم
    {(صحیح)KS_SR,	"\033M"},
#اگر_تعریف_شدہ TERMINFO
    {(صحیح)KS_CRI,	"\033[%p1%dC"},
#ورنہ
    {(صحیح)KS_CRI,	"\033[%dC"},
#اگر_ختم
    {(صحیح)KS_KS,	"\033[?1h\033="},
    {(صحیح)KS_KE,	"\033[?1l\033>"},
#اگر_تعریف_شدہ FEAT_XTERM_SAVE
    {(صحیح)KS_TI,	"\0337\033[?47h"},
    {(صحیح)KS_TE,	"\033[?47l\0338"},
#اگر_ختم
    // These are now under control of the 'keyprotocol' option, see
    // "builtin_mok2".
    // {(int)KS_CTI,	"\033[>4;2m"},
    // {(int)KS_CRK,	"\033[?4m"},
    // {(int)KS_CTE,	"\033[>4;m"},
    {(صحیح)KS_CIS,	"\033]1;"},
    {(صحیح)KS_CIE,	"\007"},
    {(صحیح)KS_TS,	"\033]2;"},
    {(صحیح)KS_FS,	"\007"},
    {(صحیح)KS_CSC,	"\033]12;"},
    {(صحیح)KS_CEC,	"\007"},
#اگر_تعریف_شدہ TERMINFO
    {(صحیح)KS_CWS,	"\033[8;%p1%d;%p2%dt"},
    {(صحیح)KS_CWP,	"\033[3;%p1%d;%p2%dt"},
    {(صحیح)KS_CGP,	"\033[13t"},
#ورنہ
    {(صحیح)KS_CWS,	"\033[8;%d;%dt"},
    {(صحیح)KS_CWP,	"\033[3;%d;%dt"},
    {(صحیح)KS_CGP,	"\033[13t"},
#اگر_ختم
    {(صحیح)KS_CRV,	"\033[>c"},
    {(صحیح)KS_CXM,	"\033[?1006;1000%?%p1%{1}%=%th%el%;"},
    {(صحیح)KS_RFG,	"\033]10;?\007"},
    {(صحیح)KS_RBG,	"\033]11;?\007"},
    {(صحیح)KS_U7,	"\033[6n"},
    {(صحیح)KS_CAU,	"\033[58;5;%dm"},
    {(صحیح)KS_CBE,	"\033[?2004h"},
    {(صحیح)KS_CBD,	"\033[?2004l"},
    {(صحیح)KS_CST,	"\033[22;2t"},
    {(صحیح)KS_CRT,	"\033[23;2t"},
    {(صحیح)KS_SSI,	"\033[22;1t"},
    {(صحیح)KS_SRI,	"\033[23;1t"},
#اگر (defined(UNIX) || defined(VMS))
    {(صحیح)KS_FD,	"\033[?1004l"},
    {(صحیح)KS_FE,	"\033[?1004h"},
#اگر_ختم

    {K_اوپر,		"\033O*A"},
    {K_نیچے_کو,		"\033O*B"},
    {K_دایاں,		"\033O*C"},
    {K_بایاں,		"\033O*D"},
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
    {K_انتہا,		"\033[@;*F"},	// Esc [ F or Esc 1 ; F
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

    {BT_EXTRA_کلیدیں,   ""},
    {TERMCAP2_کلید('k', '0'), "\033[10;*~"}, // F0
    {TERMCAP2_کلید('F', '3'), "\033[25;*~"}, // F13
    // F14 and F15 are missing, because they send the same codes as the undo
    // and help key, although they don't work on all keyboards.
    {TERMCAP2_کلید('F', '6'), "\033[29;*~"}, // F16
    {TERMCAP2_کلید('F', '7'), "\033[31;*~"}, // F17
    {TERMCAP2_کلید('F', '8'), "\033[32;*~"}, // F18
    {TERMCAP2_کلید('F', '9'), "\033[33;*~"}, // F19
    {TERMCAP2_کلید('F', 'A'), "\033[34;*~"}, // F20

    {TERMCAP2_کلید('F', 'B'), "\033[42;*~"}, // F21
    {TERMCAP2_کلید('F', 'C'), "\033[43;*~"}, // F22
    {TERMCAP2_کلید('F', 'D'), "\033[44;*~"}, // F23
    {TERMCAP2_کلید('F', 'E'), "\033[45;*~"}, // F24
    {TERMCAP2_کلید('F', 'F'), "\033[46;*~"}, // F25
    {TERMCAP2_کلید('F', 'G'), "\033[47;*~"}, // F26
    {TERMCAP2_کلید('F', 'H'), "\033[48;*~"}, // F27
    {TERMCAP2_کلید('F', 'I'), "\033[49;*~"}, // F28
    {TERMCAP2_کلید('F', 'J'), "\033[50;*~"}, // F29
    {TERMCAP2_کلید('F', 'K'), "\033[51;*~"}, // F30

    {TERMCAP2_کلید('F', 'L'), "\033[52;*~"}, // F31
    {TERMCAP2_کلید('F', 'M'), "\033[53;*~"}, // F32
    {TERMCAP2_کلید('F', 'N'), "\033[54;*~"}, // F33
    {TERMCAP2_کلید('F', 'O'), "\033[55;*~"}, // F34
    {TERMCAP2_کلید('F', 'P'), "\033[56;*~"}, // F35
    {TERMCAP2_کلید('F', 'Q'), "\033[57;*~"}, // F36
    {TERMCAP2_کلید('F', 'R'), "\033[58;*~"}, // F37

    {(صحیح)KS_NAME,	تہی}  // end marker
};

/*
 * Additions for using modifyOtherKeys level 2.  Same as what is used for
 * xterm.
 */
ساکن tcap_اندراج_T builtin_mok2[] = {
    // t_TI enables modifyOtherKeys level 2
    {(صحیح)KS_CTI,	"\033[>4;2m"},

    // XTQMODKEYS was added in xterm version 377: "CSI ? 4 m" which should
    // return "{lead} > 4 ; Pv m".  Before version 377 we expect it to have no
    // effect.
    {(صحیح)KS_CRK,	"\033[?4m"},

    // t_TE disables modifyOtherKeys
    {(صحیح)KS_CTE,	"\033[>4;m"},

    {(صحیح)KS_NAME,	تہی}  // end marker
};

/*
 * Additions for using the Kitty keyboard protocol.
 */
ساکن tcap_اندراج_T builtin_kitty[] = {
    // t_TI enables the kitty keyboard protocol.
    {(صحیح)KS_CTI,	"\033[=1;1u"},

    // t_RK requests the kitty keyboard protocol state
    {(صحیح)KS_CRK,	"\033[?u"},

    // t_TE also disables modifyOtherKeys, because t_TI from xterm may already
    // have been used.
    {(صحیح)KS_CTE,	"\033[>4;m\033[=0;1u"},

    // t_RF request terminal foreground color
    {(صحیح)KS_RFG,	"\033]10;?\033\\"},

    // t_RB request terminal background color
    {(صحیح)KS_RBG,	"\033]11;?\033\\"},

    {(صحیح)KS_NAME,	تہی}  // end marker
};

#اگر_تعریف_شدہ HAVE_TGETENT
/*
 * Additions for enabling/disabling synchronized output mode for terminal.
 */
ساکن tcap_اندراج_T builtin_sync_output[] = {
    {(صحیح)KS_BSU,	"\033[?2026h"},
    {(صحیح)KS_ESU,	"\033[?2026l"},
    {(صحیح)KS_NAME,	تہی}  // end marker
};
#اگر_ختم

#اگر_تعریف_شدہ FEAT_TERMRESPONSE
/*
 * List of DECRQM modes that Vim supports
 */
ساکن ثابت صحیح dec_modes[] = {
    2026,   // Synchronized output
# ifdef UNIX
    2048    // In-band terminal resize events
# endif
};
#اگر_ختم

#اگر_تعریف_شدہ FEAT_TERMGUICOLORS
/*
 * Additions for using the RGB colors and terminal font
 */
ساکن tcap_اندراج_T builtin_rgb[] = {
    // These are printf strings, not terminal codes.
    {(صحیح)KS_8F,	"\033[38;2;%lu;%lu;%lum"},
    {(صحیح)KS_8B,	"\033[48;2;%lu;%lu;%lum"},
    {(صحیح)KS_8U,	"\033[58;2;%lu;%lu;%lum"},

    {(صحیح)KS_NAME,	تہی}  // end marker
};
#اگر_ختم

#اگر_تعریف_شدہ HAVE_TGETENT
ساکن tcap_اندراج_T special_رکن[] = {
    // These are printf strings, not terminal codes.
    {(صحیح)KS_CF,	"\033[%dm"},
    {(صحیح)KS_NAME,	تہی}  // end marker
};
#اگر_ختم

/*
 * iris-ansi for Silicon Graphics machines.
 */
ساکن tcap_اندراج_T builtin_iris_ansi[] = {
    {(صحیح)KS_CE,	"\033[K"},
    {(صحیح)KS_CD,	"\033[J"},
    {(صحیح)KS_AL,	"\033[L"},
#اگر_تعریف_شدہ TERMINFO
    {(صحیح)KS_CAL,	"\033[%p1%dL"},
#ورنہ
    {(صحیح)KS_CAL,	"\033[%dL"},
#اگر_ختم
    {(صحیح)KS_DL,	"\033[M"},
#اگر_تعریف_شدہ TERMINFO
    {(صحیح)KS_CDL,	"\033[%p1%dM"},
#ورنہ
    {(صحیح)KS_CDL,	"\033[%dM"},
#اگر_ختم
#اگر 0	// The scroll region is not working as Vim expects.
# ifdef TERMINFO
    {(صحیح)KS_CS,	"\033[%i%p1%d;%p2%dr"},
# ورنہ
    {(صحیح)KS_CS,	"\033[%i%d;%dr"},
# endif
#اگر_ختم
    {(صحیح)KS_CL,	"\033[H\033[2J"},
    {(صحیح)KS_VE,	"\033[9/y\033[12/y"},	// These aren't documented
    {(صحیح)KS_VS,	"\033[10/y\033[=1h\033[=2l"}, // These aren't documented
    {(صحیح)KS_TI,	"\033[=6h"},
    {(صحیح)KS_TE,	"\033[=6l"},
    {(صحیح)KS_SE,	"\033[21;27m"},
    {(صحیح)KS_SO,	"\033[1;7m"},
    {(صحیح)KS_ME,	"\033[m"},
    {(صحیح)KS_MR,	"\033[7m"},
    {(صحیح)KS_MD,	"\033[1m"},
    {(صحیح)KS_CCO,	"8"},			// allow 8 colors
    {(صحیح)KS_CZH,	"\033[3m"},		// italic mode on
    {(صحیح)KS_CZR,	"\033[23m"},		// italic mode off
    {(صحیح)KS_US,	"\033[4m"},		// underline on
    {(صحیح)KS_UE,	"\033[24m"},		// underline off
#اگر_تعریف_شدہ TERMINFO
    {(صحیح)KS_CAB,	"\033[4%p1%dm"},    // set background color (ANSI)
    {(صحیح)KS_CAF,	"\033[3%p1%dm"},    // set foreground color (ANSI)
    {(صحیح)KS_CSB,	"\033[102;%p1%dm"}, // set screen background color
    {(صحیح)KS_CSF,	"\033[101;%p1%dm"}, // set screen foreground color
#ورنہ
    {(صحیح)KS_CAB,	"\033[4%dm"},	    // set background color (ANSI)
    {(صحیح)KS_CAF,	"\033[3%dm"},	    // set foreground color (ANSI)
    {(صحیح)KS_CSB,	"\033[102;%dm"},    // set screen background color
    {(صحیح)KS_CSF,	"\033[101;%dm"},    // set screen foreground color
#اگر_ختم
    {(صحیح)KS_MS,	"y"},		// guessed
    {(صحیح)KS_UT,	"y"},		// guessed
    {(صحیح)KS_LE,	"\b"},
#اگر_تعریف_شدہ TERMINFO
    {(صحیح)KS_CM,	"\033[%i%p1%d;%p2%dH"},
#ورنہ
    {(صحیح)KS_CM,	"\033[%i%d;%dH"},
#اگر_ختم
    {(صحیح)KS_SR,	"\033M"},
#اگر_تعریف_شدہ TERMINFO
    {(صحیح)KS_CRI,	"\033[%p1%dC"},
#ورنہ
    {(صحیح)KS_CRI,	"\033[%dC"},
#اگر_ختم
    {(صحیح)KS_CIS,	"\033P3.y"},
    {(صحیح)KS_CIE,	"\234"},    // ST "String Terminator"
    {(صحیح)KS_TS,	"\033P1.y"},
    {(صحیح)KS_FS,	"\234"},    // ST "String Terminator"
#اگر_تعریف_شدہ TERMINFO
    {(صحیح)KS_CWS,	"\033[203;%p1%d;%p2%d/y"},
    {(صحیح)KS_CWP,	"\033[205;%p1%d;%p2%d/y"},
#ورنہ
    {(صحیح)KS_CWS,	"\033[203;%d;%d/y"},
    {(صحیح)KS_CWP,	"\033[205;%d;%d/y"},
#اگر_ختم
    {K_اوپر,		"\033[A"},
    {K_نیچے_کو,		"\033[B"},
    {K_بایاں,		"\033[D"},
    {K_دایاں,		"\033[C"},
    {K_S_اوپر,		"\033[161q"},
    {K_S_نیچے_کو,		"\033[164q"},
    {K_S_بایاں,		"\033[158q"},
    {K_S_دایاں,		"\033[167q"},
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
    {K_انتہا,		"\033[146q"},
    {K_PAGEUP,		"\033[150q"},
    {K_PAGEDOWN,	"\033[154q"},

    {(صحیح)KS_NAME,	تہی}  // end marker
};

/*
 * These codes are valid when nansi.sys or equivalent has been installed.
 * Function keys on a PC are preceded with a NUL. These are converted into
 * K_NUL '\316' in mch_inchar(), because we cannot handle NULs in key codes.
 * CTRL-arrow is used instead of SHIFT-arrow.
 */
ساکن tcap_اندراج_T builtin_pcansi[] = {
    {(صحیح)KS_DL,	"\033[M"},
    {(صحیح)KS_AL,	"\033[L"},
    {(صحیح)KS_CE,	"\033[K"},
    {(صحیح)KS_CL,	"\033[2J"},
    {(صحیح)KS_ME,	"\033[0m"},
    {(صحیح)KS_MR,	"\033[5m"},	// reverse: black on lightgrey
    {(صحیح)KS_MD,	"\033[1m"},	// bold: white text
    {(صحیح)KS_SE,	"\033[0m"},	// standout end
    {(صحیح)KS_SO,	"\033[31m"},	// standout: white on blue
    {(صحیح)KS_CZH,	"\033[34;43m"},	// italic mode: blue text on yellow
    {(صحیح)KS_CZR,	"\033[0m"},	// italic mode end
    {(صحیح)KS_US,	"\033[36;41m"},	// underscore mode: cyan text on red
    {(صحیح)KS_UE,	"\033[0m"},	// underscore mode end
    {(صحیح)KS_CCO,	"8"},		// allow 8 colors
#اگر_تعریف_شدہ TERMINFO
    {(صحیح)KS_CAB,	"\033[4%p1%dm"},// set background color
    {(صحیح)KS_CAF,	"\033[3%p1%dm"},// set foreground color
#ورنہ
    {(صحیح)KS_CAB,	"\033[4%dm"},	// set background color
    {(صحیح)KS_CAF,	"\033[3%dm"},	// set foreground color
#اگر_ختم
    {(صحیح)KS_OP,	"\033[0m"},	// reset colors
    {(صحیح)KS_MS,	"y"},
    {(صحیح)KS_UT,	"y"},		// guessed
    {(صحیح)KS_LE,	"\b"},
#اگر_تعریف_شدہ TERMINFO
    {(صحیح)KS_CM,	"\033[%i%p1%d;%p2%dH"},
#ورنہ
    {(صحیح)KS_CM,	"\033[%i%d;%dH"},
#اگر_ختم
#اگر_تعریف_شدہ TERMINFO
    {(صحیح)KS_CRI,	"\033[%p1%dC"},
#ورنہ
    {(صحیح)KS_CRI,	"\033[%dC"},
#اگر_ختم
    {K_اوپر,		"\316H"},
    {K_نیچے_کو,		"\316P"},
    {K_بایاں,		"\316K"},
    {K_دایاں,		"\316M"},
    {K_S_بایاں,		"\316s"},
    {K_S_دایاں,		"\316t"},
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
    {K_انتہا,		"\316O"},
    {K_PAGEDOWN,	"\316Q"},
    {K_PAGEUP,		"\316I"},

    {(صحیح)KS_NAME,	تہی}  // end marker
};

/*
 * These codes are valid for the Win32 Console.  The entries that start with
 * ESC | are translated into console calls in os_win32.c.  The function keys
 * are also translated in os_win32.c.
 */
ساکن tcap_اندراج_T builtin_win32[] = {
    {(صحیح)KS_CE,	"\033|K"},	// clear to end of line
    {(صحیح)KS_AL,	"\033|L"},	// add new blank line
#اگر_تعریف_شدہ TERMINFO
    {(صحیح)KS_CAL,	"\033|%p1%dL"},	// add number of new blank lines
#ورنہ
    {(صحیح)KS_CAL,	"\033|%dL"},	// add number of new blank lines
#اگر_ختم
    {(صحیح)KS_DL,	"\033|M"},	// delete line
#اگر_تعریف_شدہ TERMINFO
    {(صحیح)KS_CDL,	"\033|%p1%dM"},	// delete number of lines
    {(صحیح)KS_CSV,	"\033|%p1%d;%p2%dV"},
#ورنہ
    {(صحیح)KS_CDL,	"\033|%dM"},	// delete number of lines
    {(صحیح)KS_CSV,	"\033|%d;%dV"},
#اگر_ختم
    {(صحیح)KS_CL,	"\033|J"},	// clear screen
    {(صحیح)KS_CD,	"\033|j"},	// clear to end of display
    {(صحیح)KS_VI,	"\033|v"},	// cursor invisible
    {(صحیح)KS_VE,	"\033|V"},	// cursor visible

    {(صحیح)KS_ME,	"\033|0m"},	//معمول
    {(صحیح)KS_MR,	"\033|112m"},	// reverse: black on lightgray
    {(صحیح)KS_MD,	"\033|15m"},	// bold: white on black
#اگر 1
    {(صحیح)KS_SO,	"\033|31m"},	// standout: white on blue
    {(صحیح)KS_SE,	"\033|0m"},	// standout end
#ورنہ
    {(صحیح)KS_SO,	"\033|F"},	// standout: high intensity
    {(صحیح)KS_SE,	"\033|f"},	// standout end
#اگر_ختم
    {(صحیح)KS_CZH,	"\033|225m"},	// italic: blue text on yellow
    {(صحیح)KS_CZR,	"\033|0m"},	// italic end
    {(صحیح)KS_US,	"\033|67m"},	// underscore: cyan text on red
    {(صحیح)KS_UE,	"\033|0m"},	// underscore end
    {(صحیح)KS_CCO,	"16"},		// allow 16 colors
#اگر_تعریف_شدہ TERMINFO
    {(صحیح)KS_CAB,	"\033|%p1%db"},	// set background color
    {(صحیح)KS_CAF,	"\033|%p1%df"},	// set foreground color
#ورنہ
    {(صحیح)KS_CAB,	"\033|%db"},	// set background color
    {(صحیح)KS_CAF,	"\033|%df"},	// set foreground color
#اگر_ختم

    {(صحیح)KS_MS,	"y"},		// save to move cur in reverse mode
    {(صحیح)KS_UT,	"y"},
    {(صحیح)KS_XN,	"y"},
    {(صحیح)KS_LE,	"\b"},
#اگر_تعریف_شدہ TERMINFO
    {(صحیح)KS_CM,	"\033|%i%p1%d;%p2%dH"}, // cursor motion
#ورنہ
    {(صحیح)KS_CM,	"\033|%i%d;%dH"}, // cursor motion
#اگر_ختم
    {(صحیح)KS_VB,	"\033|B"},	// visual bell
    {(صحیح)KS_TI,	"\033|S"},	// put terminal in termcap mode
    {(صحیح)KS_TE,	"\033|E"},	// out of termcap mode
#اگر_تعریف_شدہ TERMINFO
    {(صحیح)KS_CS,	"\033|%i%p1%d;%p2%dr"}, // scroll region
#ورنہ
    {(صحیح)KS_CS,	"\033|%i%d;%dr"}, // scroll region
#اگر_ختم

    {K_اوپر,		"\316H"},
    {K_نیچے_کو,		"\316P"},
    {K_بایاں,		"\316K"},
    {K_دایاں,		"\316M"},
    {K_S_اوپر,		"\316\304"},
    {K_S_نیچے_کو,		"\316\317"},
    {K_S_بایاں,		"\316\311"},
    {K_C_بایاں,		"\316s"},
    {K_S_دایاں,		"\316\313"},
    {K_C_دایاں,		"\316t"},
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
    {K_انتہا,		"\316O"},
    {K_S_انتہا,		"\316\315"},
    {K_C_انتہا,		"\316u"},
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

    {(صحیح)KS_NAME,	تہی}  // end marker
};

#اگر defined(FEAT_GUI)
/*
 * GUI uses made-up codes, only used inside Vim.
 */
ساکن tcap_اندراج_T builtin_gui[] = {
    {(صحیح)KS_CE,	"\033|$"},
    {(صحیح)KS_AL,	"\033|i"},
# ifdef TERMINFO
    {(صحیح)KS_CAL,	"\033|%p1%dI"},
# ورنہ
    {(صحیح)KS_CAL,	"\033|%dI"},
# endif
    {(صحیح)KS_DL,	"\033|d"},
# ifdef TERMINFO
    {(صحیح)KS_CDL,	"\033|%p1%dD"},
    {(صحیح)KS_CS,	"\033|%p1%d;%p2%dR"},
    {(صحیح)KS_CSV,	"\033|%p1%d;%p2%dV"},
# ورنہ
    {(صحیح)KS_CDL,	"\033|%dD"},
    {(صحیح)KS_CS,	"\033|%d;%dR"},
    {(صحیح)KS_CSV,	"\033|%d;%dV"},
# endif
    {(صحیح)KS_CL,	"\033|C"},
			// attributes switched on with 'h', off with * 'H'
    {(صحیح)KS_ME,	"\033|31H"}, // HL_ALL
    {(صحیح)KS_MR,	"\033|1h"},   // HL_INVERSE
    {(صحیح)KS_MD,	"\033|2h"},   // HL_BOLD
    {(صحیح)KS_SE,	"\033|16H"}, // HL_STANDOUT
    {(صحیح)KS_SO,	"\033|16h"}, // HL_STANDOUT
    {(صحیح)KS_UE,	"\033|8H"},   // HL_UNDERLINE
    {(صحیح)KS_US,	"\033|8h"},   // HL_UNDERLINE
    {(صحیح)KS_UCE,	"\033|8C"},   // HL_UNDERCURL
    {(صحیح)KS_UCS,	"\033|8c"},   // HL_UNDERCURL
    {(صحیح)KS_STE,	"\033|4C"},   // HL_STRIKETHROUGH
    {(صحیح)KS_STS,	"\033|4c"},   // HL_STRIKETHROUGH
    {(صحیح)KS_CZR,	"\033|4H"},   // HL_ITALIC
    {(صحیح)KS_CZH,	"\033|4h"},   // HL_ITALIC
    {(صحیح)KS_VB,	"\033|f"},
    {(صحیح)KS_MS,	"y"},
    {(صحیح)KS_UT,	"y"},
    {(صحیح)KS_XN,	"y"},
    {(صحیح)KS_LE,	"\b"},		// cursor-left = BS
    {(صحیح)KS_ND,	"\014"},	// cursor-right = CTRL-L
# ifdef TERMINFO
    {(صحیح)KS_CM,	"\033|%p1%d;%p2%dM"},
# ورنہ
    {(صحیح)KS_CM,	"\033|%d;%dM"},
# endif
	// there are no key sequences here, the GUI sequences are recognized
	// in check_termcode()

    {(صحیح)KS_NAME,	تہی}  // end marker
};
#اگر_ختم

/*
 * Amiga console window, default for Amiga.
 */
ساکن tcap_اندراج_T builtin_amiga[] = {
    {(صحیح)KS_CE,	"\033[K"},
    {(صحیح)KS_CD,	"\033[J"},
    {(صحیح)KS_AL,	"\033[L"},
#اگر_تعریف_شدہ TERMINFO
    {(صحیح)KS_CAL,	"\033[%p1%dL"},
#ورنہ
    {(صحیح)KS_CAL,	"\033[%dL"},
#اگر_ختم
    {(صحیح)KS_DL,	"\033[M"},
#اگر_تعریف_شدہ TERMINFO
    {(صحیح)KS_CDL,	"\033[%p1%dM"},
#ورنہ
    {(صحیح)KS_CDL,	"\033[%dM"},
#اگر_ختم
    {(صحیح)KS_CL,	"\014"},
    {(صحیح)KS_VI,	"\033[0 p"},
    {(صحیح)KS_VE,	"\033[1 p"},
    {(صحیح)KS_ME,	"\033[0m"},
    {(صحیح)KS_MR,	"\033[7m"},
    {(صحیح)KS_MD,	"\033[1m"},
    {(صحیح)KS_SE,	"\033[0m"},
    {(صحیح)KS_SO,	"\033[33m"},
    {(صحیح)KS_US,	"\033[4m"},
    {(صحیح)KS_UE,	"\033[0m"},
    {(صحیح)KS_CZH,	"\033[3m"},
    {(صحیح)KS_CZR,	"\033[0m"},
#اگر defined(__amigaos4__) || defined(__MORPHOS__) || defined(__AROS__)
    {(صحیح)KS_CCO,	"8"},		// allow 8 colors
# ifdef TERMINFO
    {(صحیح)KS_CAB,	"\033[4%p1%dm"},// set background color
    {(صحیح)KS_CAF,	"\033[3%p1%dm"},// set foreground color
# ورنہ
    {(صحیح)KS_CAB,	"\033[4%dm"},	// set background color
    {(صحیح)KS_CAF,	"\033[3%dm"},	// set foreground color
# endif
    {(صحیح)KS_OP,	"\033[m"},	// reset colors
#اگر_ختم
    {(صحیح)KS_MS,	"y"},
    {(صحیح)KS_UT,	"y"},		// guessed
    {(صحیح)KS_LE,	"\b"},
#اگر_تعریف_شدہ TERMINFO
    {(صحیح)KS_CM,	"\033[%i%p1%d;%p2%dH"},
#ورنہ
    {(صحیح)KS_CM,	"\033[%i%d;%dH"},
#اگر_ختم
#اگر defined(__MORPHOS__)
    {(صحیح)KS_SR,	"\033M"},
#اگر_ختم
#اگر_تعریف_شدہ TERMINFO
    {(صحیح)KS_CRI,	"\033[%p1%dC"},
#ورنہ
    {(صحیح)KS_CRI,	"\033[%dC"},
#اگر_ختم
    {K_اوپر,		"\233A"},
    {K_نیچے_کو,		"\233B"},
    {K_بایاں,		"\233D"},
    {K_دایاں,		"\233C"},
    {K_S_اوپر,		"\233T"},
    {K_S_نیچے_کو,		"\233S"},
    {K_S_بایاں,		"\233 A"},
    {K_S_دایاں,		"\233 @"},
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
    {K_انتہا,		"\233\064\065~"},	// 101 key keyboard

    {BT_EXTRA_کلیدیں,	""},
    {TERMCAP2_کلید('#', '2'), "\233\065\064~"},	// shifted home key
    {TERMCAP2_کلید('#', '3'), "\233\065\060~"},	// shifted insert key
    {TERMCAP2_کلید('*', '7'), "\233\065\065~"},	// shifted end key

    {(صحیح)KS_NAME,	تہی}  // end marker
};

/*
 * The most minimal terminal: only clear screen and cursor positioning.
 */
ساکن tcap_اندراج_T builtin_dumb[] = {
    {(صحیح)KS_CL,	"\014"},
#اگر_تعریف_شدہ TERMINFO
    {(صحیح)KS_CM,	"\033[%i%p1%d;%p2%dH"},
#ورنہ
    {(صحیح)KS_CM,	"\033[%i%d;%dH"},
#اگر_ختم

    {(صحیح)KS_NAME,	تہی}  // end marker
};

/*
 * Terminal used for debugging.
 */
ساکن tcap_اندراج_T builtin_debug[] = {
    {(صحیح)KS_CE,	"[CE]"},
    {(صحیح)KS_CD,	"[CD]"},
    {(صحیح)KS_AL,	"[AL]"},
#اگر_تعریف_شدہ TERMINFO
    {(صحیح)KS_CAL,	"[CAL%p1%d]"},
#ورنہ
    {(صحیح)KS_CAL,	"[CAL%d]"},
#اگر_ختم
    {(صحیح)KS_DL,	"[DL]"},
#اگر_تعریف_شدہ TERMINFO
    {(صحیح)KS_CDL,	"[CDL%p1%d]"},
#ورنہ
    {(صحیح)KS_CDL,	"[CDL%d]"},
#اگر_ختم
#اگر_تعریف_شدہ TERMINFO
    {(صحیح)KS_CS,	"[%p1%dCS%p2%d]"},
#ورنہ
    {(صحیح)KS_CS,	"[%dCS%d]"},
#اگر_ختم
#اگر_تعریف_شدہ TERMINFO
    {(صحیح)KS_CSV,	"[%p1%dCSV%p2%d]"},
#ورنہ
    {(صحیح)KS_CSV,	"[%dCSV%d]"},
#اگر_ختم
#اگر_تعریف_شدہ TERMINFO
    {(صحیح)KS_CAB,	"[CAB%p1%d]"},
    {(صحیح)KS_CAF,	"[CAF%p1%d]"},
    {(صحیح)KS_CSB,	"[CSB%p1%d]"},
    {(صحیح)KS_CSF,	"[CSF%p1%d]"},
#ورنہ
    {(صحیح)KS_CAB,	"[CAB%d]"},
    {(صحیح)KS_CAF,	"[CAF%d]"},
    {(صحیح)KS_CSB,	"[CSB%d]"},
    {(صحیح)KS_CSF,	"[CSF%d]"},
#اگر_ختم
    {(صحیح)KS_CAU,	"[CAU%d]"},
    {(صحیح)KS_OP,	"[OP]"},
    {(صحیح)KS_LE,	"[LE]"},
    {(صحیح)KS_CL,	"[CL]"},
    {(صحیح)KS_VI,	"[VI]"},
    {(صحیح)KS_VE,	"[VE]"},
    {(صحیح)KS_VS,	"[VS]"},
    {(صحیح)KS_ME,	"[ME]"},
    {(صحیح)KS_MR,	"[MR]"},
    {(صحیح)KS_MB,	"[MB]"},
    {(صحیح)KS_MD,	"[MD]"},
    {(صحیح)KS_SE,	"[SE]"},
    {(صحیح)KS_SO,	"[SO]"},
    {(صحیح)KS_UE,	"[UE]"},
    {(صحیح)KS_US,	"[US]"},
    {(صحیح)KS_UCE,	"[UCE]"},
    {(صحیح)KS_UCS,	"[UCS]"},
    {(صحیح)KS_USS,	"[USS]"},
    {(صحیح)KS_DS,	"[DS]"},
    {(صحیح)KS_CDS,	"[CDS]"},
    {(صحیح)KS_STE,	"[STE]"},
    {(صحیح)KS_STS,	"[STS]"},
    {(صحیح)KS_MS,	"[MS]"},
    {(صحیح)KS_UT,	"[UT]"},
    {(صحیح)KS_XN,	"[XN]"},
#اگر_تعریف_شدہ TERMINFO
    {(صحیح)KS_CM,	"[%p1%dCM%p2%d]"},
#ورنہ
    {(صحیح)KS_CM,	"[%dCM%d]"},
#اگر_ختم
    {(صحیح)KS_SR,	"[SR]"},
#اگر_تعریف_شدہ TERMINFO
    {(صحیح)KS_CRI,	"[CRI%p1%d]"},
#ورنہ
    {(صحیح)KS_CRI,	"[CRI%d]"},
#اگر_ختم
    {(صحیح)KS_VB,	"[VB]"},
    {(صحیح)KS_KS,	"[KS]"},
    {(صحیح)KS_KE,	"[KE]"},
    {(صحیح)KS_TI,	"[TI]"},
    {(صحیح)KS_TE,	"[TE]"},
    {(صحیح)KS_CIS,	"[CIS]"},
    {(صحیح)KS_CIE,	"[CIE]"},
    {(صحیح)KS_CSC,	"[CSC]"},
    {(صحیح)KS_CEC,	"[CEC]"},
    {(صحیح)KS_TS,	"[TS]"},
    {(صحیح)KS_FS,	"[FS]"},
#اگر_تعریف_شدہ TERMINFO
    {(صحیح)KS_CWS,	"[%p1%dCWS%p2%d]"},
    {(صحیح)KS_CWP,	"[%p1%dCWP%p2%d]"},
#ورنہ
    {(صحیح)KS_CWS,	"[%dCWS%d]"},
    {(صحیح)KS_CWP,	"[%dCWP%d]"},
#اگر_ختم
    {(صحیح)KS_CRV,	"[CRV]"},
    {(صحیح)KS_CXM,	"[CXM]"},
    {(صحیح)KS_U7,	"[U7]"},
    {(صحیح)KS_RFG,	"[RFG]"},
    {(صحیح)KS_RBG,	"[RBG]"},
    {(صحیح)KS_CF,	"[CF%d]"},
    {K_اوپر,		"[KU]"},
    {K_نیچے_کو,		"[KD]"},
    {K_بایاں,		"[KL]"},
    {K_دایاں,		"[KR]"},
    {K_XUP,		"[xKU]"},
    {K_XDOWN,		"[xKD]"},
    {K_XLEFT,		"[xKL]"},
    {K_XRIGHT,		"[xKR]"},
    {K_S_اوپر,		"[S-KU]"},
    {K_S_نیچے_کو,		"[S-KD]"},
    {K_S_بایاں,		"[S-KL]"},
    {K_C_بایاں,		"[C-KL]"},
    {K_S_دایاں,		"[S-KR]"},
    {K_C_دایاں,		"[C-KR]"},
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
    {K_انتہا,		"[END]"},
    {K_S_انتہا,		"[C-END]"},
    {K_C_انتہا,		"[C-END]"},
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

    {(صحیح)KS_NAME,	تہی}  // end marker
};

/*
 * List of builtin terminals.
 */
قسمتعریف ساخت {
    حرف	    *bitc_name;	    // name, such as "xterm"
    tcap_اندراج_T    *bitc_جدول;    // table with entries for bitc_name
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
#اگر defined(FEAT_GUI)
    {"gui",	    builtin_gui},
#اگر_ختم
    {"amiga",	    builtin_amiga},
    {"dumb",	    builtin_dumb},
    {"debug",	    builtin_debug},

    {تہی,	    تہی},  // end marker
};

#اگر defined(FEAT_TERMGUICOLORS)
    ساکن guicolor_T
termgui_mch_get_color(char_u *name)
{
    واپس gui_get_color_cmn(name);
}

    guicolor_T
termgui_get_color(char_u *name)
{
    guicolor_T	t;

    اگر (*name == NUL)
	واپس INVALCOLOR;
    t = termgui_mch_get_color(name);

    اگر (t == INVALCOLOR)
	semsg(_(e_cannot_allocate_color_str), name);
    واپس t;
}

    guicolor_T
termgui_mch_get_rgb(guicolor_T color)
{
    واپس color;
}
#اگر_ختم

/*
 * DEFAULT_TERM is used, when no terminal is specified with -T option or $TERM.
 */
#اگر_تعریف_شدہ AMIGA
# define DEFAULT_رکن	(char_u *)"amiga"
#اگر_ختم

#اگر_تعریف_شدہ MSWIN
# define DEFAULT_رکن	(char_u *)"win32"
#اگر_ختم

#اگر defined(UNIX)
# define DEFAULT_رکن	(char_u *)"ansi"
#اگر_ختم

#اگر_تعریف_شدہ VMS
# define DEFAULT_رکن	(char_u *)"vt320"
#اگر_ختم

#اگر_تعریف_شدہ __HAIKU__
# undef DEFAULT_رکن
# define DEFAULT_رکن	(char_u *)"xterm"
#اگر_ختم

#اگر_غیر_تعریف_شدہ DEFAULT_رکن
# define DEFAULT_رکن	(char_u *)"dumb"
#اگر_ختم

/*
 * Term_strings contains currently used terminal output strings.
 * It is initialized with the default values by parse_builtin_tcap().
 * The values can be changed by setting the option with the same name.
 */
char_u *(رکن_لڑیاں[(صحیح)KS_آخری + 1]);

ساکن صحیح	need_gather = FALSE;	    // need to fill termleader[]
ساکن char_u	termleader[256 + 1];	    // for check_termcode()
#اگر_تعریف_شدہ FEAT_TERMRESPONSE
ساکن صحیح	check_for_codes = FALSE;    // check for key code response
#اگر_ختم

/*
 * Structure and table to store terminal features that can be detected by
 * querying the terminal.  Either by inspecting the termresponse or a more
 * specific request.  Besides this there are:
 * t_colors - number of colors supported
 */
قسمتعریف ساخت {
    حرف    *tpr_name;
    صحیح	    tpr_سیٹ_by_termresponse;
    صحیح	    tpr_حالت;
} termprop_T;

// Values for tpr_status.
#تعریف_کرو TPR_UNKNOWN	    'u'
#تعریف_کرو TPR_YES		    'y'
#تعریف_کرو TPR_NO		    'n'
#تعریف_کرو TPR_MOUSE_XTERM     'x'	// use "xterm" for 'ttymouse'
#تعریف_کرو TPR_MOUSE_XTERM2    '2'	// use "xterm2" for 'ttymouse'
#تعریف_کرو TPR_MOUSE_SGR	    's'	// use "sgr" for 'ttymouse'

// can request the cursor style without messing up the display
#تعریف_کرو TPR_CURSOR_STYLE	    0
// can request the cursor blink mode without messing up the display
#تعریف_کرو TPR_CURSOR_BLINK	    1
// can set the underline color with t_8u without resetting other colors
#تعریف_کرو TPR_UNDERLINE_RGB	    2
// mouse support - TPR_MOUSE_XTERM, TPR_MOUSE_XTERM2 or TPR_MOUSE_SGR
#تعریف_کرو TPR_MOUSE		    3
// term response indicates kitty
#تعریف_کرو TPR_KITTY		    4
// can send DECRQM requests to terminal
#تعریف_کرو TPR_DECRQM		    5
// true color, from the color count of the terminal entry
#تعریف_کرو TPR_RGB			    6
// table size
#تعریف_کرو TPR_گنتی		    7

ساکن termprop_T رکن_props[TPR_گنتی];

/*
 * Set the "rgb" terminal property from the color count.  The entry of a
 * direct color terminal, such as xterm-direct, has 16777216 colors.
 */
    ساکن خالی
سیٹ_rgb_رکن_prop(خالی)
{
    رکن_props[TPR_RGB].tpr_حالت = t_colors == 0x1000000
						       ? TPR_YES : TPR_UNKNOWN;
}

/*
 * Initialize the term_props table.
 * When "all" is FALSE only set those that are detected from the version
 * response.
 */
    خالی
init_رکن_props(صحیح all)
{
    صحیح i;

    رکن_props[TPR_CURSOR_STYLE].tpr_name = "cursor_style";
    رکن_props[TPR_CURSOR_STYLE].tpr_سیٹ_by_termresponse = FALSE;
    رکن_props[TPR_CURSOR_BLINK].tpr_name = "cursor_blink_mode";
    رکن_props[TPR_CURSOR_BLINK].tpr_سیٹ_by_termresponse = FALSE;
    رکن_props[TPR_UNDERLINE_RGB].tpr_name = "underline_rgb";
    رکن_props[TPR_UNDERLINE_RGB].tpr_سیٹ_by_termresponse = TRUE;
    رکن_props[TPR_MOUSE].tpr_name = "mouse";
    رکن_props[TPR_MOUSE].tpr_سیٹ_by_termresponse = TRUE;
    رکن_props[TPR_KITTY].tpr_name = "kitty";
    رکن_props[TPR_KITTY].tpr_سیٹ_by_termresponse = FALSE;
    رکن_props[TPR_DECRQM].tpr_name = "decrqm";
    رکن_props[TPR_DECRQM].tpr_سیٹ_by_termresponse = TRUE;
    رکن_props[TPR_RGB].tpr_name = "rgb";
    رکن_props[TPR_RGB].tpr_سیٹ_by_termresponse = FALSE;

    برائے (i = 0; i < TPR_گنتی; ++i)
	اگر (all || رکن_props[i].tpr_سیٹ_by_termresponse)
	    رکن_props[i].tpr_حالت = TPR_UNKNOWN;

    // Derived from the color count, not reset.
    سیٹ_rgb_رکن_prop();
}

#اگر defined(FEAT_EVAL)
    خالی
f_terminalprops(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
# ifdef FEAT_TERMRESPONSE
    صحیح i;
# endif

    اگر (rettv_dict_alloc(rettv) == FAIL)
	واپس;
# ifdef FEAT_TERMRESPONSE
    برائے (i = 0; i < TPR_گنتی; ++i)
    {
	char_u	قدر[2];

	قدر[0] = رکن_props[i].tpr_حالت;
	قدر[1] = NUL;
	dict_جوڑو_string_len(rettv->vval.v_dict, رکن_props[i].tpr_name,
	    قدر, (قدر[0] == NUL) ? 0 : 1);
    }
# endif
}
#اگر_ختم

/*
 * Find the builtin termcap entries for "term".
 * This also recognizes similar names.  E.g. "xterm-256color" finds the "xterm"
 * entry.
 * Returns NULL when "term" is not found.
 */
    ساکن tcap_اندراج_T *
ڈھونڈو_builtin_رکن(char_u *رکن)
{
    برائے (صحیح i = 0; ; ++i)
    {
	char_u *name = (char_u *)builtin_terminals[i].bitc_name;
	اگر (name == تہی)  // end marker
	    توڑو;
#اگر_تعریف_شدہ UNIX
	اگر (STRCMP(name, "iris-ansi") == 0 && vim_is_iris(رکن))
	    واپس builtin_terminals[i].bitc_جدول;
	اگر (STRCMP(name, "xterm") == 0 && vim_is_xterm(رکن))
	    واپس builtin_terminals[i].bitc_جدول;
#اگر_ختم
#اگر_تعریف_شدہ VMS
	اگر (STRCMP(name, "vt320") == 0 && vim_is_vt300(رکن))
	    واپس builtin_terminals[i].bitc_جدول;
#اگر_ختم
	اگر (STRCMP(رکن, name) == 0)
	    واپس builtin_terminals[i].bitc_جدول;
    }
    واپس تہی;
}

/*
 * Apply entries from a builtin termcap.
 */
    ساکن خالی
apply_builtin_tcap(char_u *رکن, tcap_اندراج_T *entries, صحیح overwrite)
{
    صحیح رکن_8bit = رکن_is_8bit(رکن);

    برائے (tcap_اندراج_T *p = entries;
	      p->bt_اندراج != (صحیح)KS_NAME && p->bt_اندراج != BT_EXTRA_کلیدیں; ++p)
    {
	اگر ((صحیح)p->bt_اندراج >= 0)	// KS_xx entry
	{
	    // Only set the value if it wasn't set yet or "overwrite" is TRUE.
	    اگر (رکن_لڑیاں[p->bt_اندراج] == تہی
		    || رکن_لڑیاں[p->bt_اندراج] == خالی_خانے_option
		    || overwrite)
	    {
#اگر_تعریف_شدہ FEAT_EVAL
		صحیح opt_idx = -1;
#اگر_ختم
		// 8bit terminal: use CSI instead of <Esc>[
		اگر (رکن_8bit && رکن_7to8bit((char_u *)p->bt_string) != 0)
		{
		    char_u  *s, *t;

		    s = vim_strsave((char_u *)p->bt_string);
		    اگر (s != تہی)
		    {
			برائے (t = s; *t; ++t)
			    اگر (رکن_7to8bit(t))
			    {
				*t = رکن_7to8bit(t);
				STRMOVE(t + 1, t + 2);
			    }
			رکن_لڑیاں[p->bt_اندراج] = s;
#اگر_تعریف_شدہ FEAT_EVAL
			opt_idx =
#اگر_ختم
				  سیٹ_رکن_option_alloced(
						   &رکن_لڑیاں[p->bt_اندراج]);
		    }
		}
		ورنہ
		{
		    رکن_لڑیاں[p->bt_اندراج] = (char_u *)p->bt_string;
#اگر_تعریف_شدہ FEAT_EVAL
		    opt_idx = get_رکن_opt_idx(&رکن_لڑیاں[p->bt_اندراج]);
#اگر_ختم
		}
#اگر_تعریف_شدہ FEAT_EVAL
		سیٹ_رکن_option_sctx_idx(تہی, opt_idx);
#اگر_ختم
	    }
	}
	ورنہ
	{
	    char_u  name[2];
	    name[0] = KEY2TERMCAP0((صحیح)p->bt_اندراج);
	    name[1] = KEY2TERMCAP1((صحیح)p->bt_اندراج);
	    اگر (ڈھونڈو_termcode(name) == تہی || overwrite)
		جوڑو_termcode(name, (char_u *)p->bt_string, رکن_8bit);
	}
    }
}

/*
 * Apply builtin termcap entries for a given keyprotocol.
 */
    خالی
apply_keyprotocol(char_u *رکن, keyprot_T prot)
{
    اگر (prot == KEYPROTOCOL_KITTY)
	apply_builtin_tcap(رکن, builtin_kitty, TRUE);
    اگر (prot == KEYPROTOCOL_MOK2)
	apply_builtin_tcap(رکن, builtin_mok2, TRUE);

    اگر (prot != KEYPROTOCOL_NONE)
	// Some function keys may accept modifiers even though the
	// terminfo/termcap entry does not indicate this.
	accept_modifiers_for_function_کلیدیں();
}

/*
 * Parsing of the builtin termcap entries.
 * Caller should check if "term" is a valid builtin terminal name.
 * The terminal's name is not set, as this is already done in termcapinit().
 */
    ساکن خالی
parse_builtin_tcap(char_u *رکن)
{
    tcap_اندراج_T *entries = ڈھونڈو_builtin_رکن(رکن);
    اگر (entries != تہی)
	apply_builtin_tcap(رکن, entries, FALSE);
}

/*
 * Set number of colors.
 * Store it as a number in t_colors.
 * Store it as a string in T_CCO (using nr_colors[]).
 */
    خالی
سیٹ_color_گنتی(صحیح nr)
{
    char_u	nr_colors[20];		// string for number of colors

    t_colors = nr;
    اگر (t_colors > 1)
	sprintf((حرف *)nr_colors, "%d", t_colors);
    ورنہ
	*nr_colors = NUL;
#اگر 0
# ifdef FEAT_TERMGUICOLORS
    // xterm-direct, enable termguicolors, when it wasn't set yet
    اگر (t_colors == 0x1000000 && !p_tgc_سیٹ)
	سیٹ_option_قدر((char_u *)"termguicolors", 1L, تہی, 0);
# endif
#اگر_ختم
    سیٹ_string_option_براہ_راست((char_u *)"t_Co", -1, nr_colors, OPT_FREE, 0);
}

/*
 * Set the color count to "val" and redraw if it changed.
 */
    ساکن خالی
may_adjust_color_گنتی(صحیح val)
{
    اگر (val == t_colors)
	واپس;

    // Nr of colors changed, initialize highlighting and redraw everything.
    // This causes a redraw, which usually clears the message.  Try keeping
    // the message if it might work.
    سیٹ_keep_پیغام_from_hist();
    سیٹ_color_گنتی(val);
    init_highlight(TRUE, FALSE);
#اگر_تعریف_شدہ DEBUG_TERMRESPONSE
    {
	صحیح r = redraw_asap(UPD_CLEAR);

	LOG_TRN("Received t_Co, redraw_asap(): %d", r);
    }
#ورنہ
    redraw_asap(UPD_CLEAR);
#اگر_ختم
}

#اگر_تعریف_شدہ HAVE_TGETENT
ساکن حرف *(کلید_نام[]) =
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
    تہی
};
#اگر_ختم

#اگر defined(HAVE_TGETENT) || defined(FEAT_TERMGUICOLORS)
/*
 * Return TRUE if "term_strings[idx]" was not set.
 */
    ساکن صحیح
رکن_لڑیاں_not_سیٹ(شمار Special_کلید idx)
{
    واپس رکن_STR(idx) == تہی || رکن_STR(idx) == خالی_خانے_option;
}
#اگر_ختم

#اگر_تعریف_شدہ HAVE_TGETENT
/*
 * Get the termcap entries we need with tgetstr(), tgetflag() and tgetnum().
 * "invoke_tgetent()" must have been called before.
 * If "*height" or "*width" are not zero then use the "li" and "col" entries to
 * get their value.
 */
    ساکن خالی
get_رکن_entries(صحیح *اونچائی, صحیح *چوڑائی)
{
    ساکن ساخت {
		    شمار Special_کلید dest; // index in term_strings[]
		    حرف *name;		  // termcap name for string
		  } string_نام[] =
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
			{(شمار Special_کلید)0, تہی}
		    };
    صحیح		    i;
    ساکن char_u   tstrbuf[TBUFSZ];
    char_u	    *tp = tstrbuf;

    /*
     * get output strings
     */
    برائے (i = 0; string_نام[i].name != تہی; ++i)
    {
	اگر (رکن_لڑیاں_not_سیٹ(string_نام[i].dest))
	{
	    رکن_STR(string_نام[i].dest) = TGETSTR(string_نام[i].name, &tp);
# ifdef FEAT_EVAL
	    سیٹ_رکن_option_sctx_idx(string_نام[i].name, -1);
# endif
	}
    }

    // tgetflag() returns 1 if the flag is present, 0 if not and
    // possibly -1 if the flag doesn't exist.
    اگر ((T_MS == تہی || T_MS == خالی_خانے_option) && tgetflag("ms") > 0)
	T_MS = (char_u *)"y";
    اگر ((T_XS == تہی || T_XS == خالی_خانے_option) && tgetflag("xs") > 0)
	T_XS = (char_u *)"y";
    اگر ((T_XN == تہی || T_XN == خالی_خانے_option) && tgetflag("xn") > 0)
	T_XN = (char_u *)"y";
    اگر ((T_DB == تہی || T_DB == خالی_خانے_option) && tgetflag("db") > 0)
	T_DB = (char_u *)"y";
    اگر ((T_DA == تہی || T_DA == خالی_خانے_option) && tgetflag("da") > 0)
	T_DA = (char_u *)"y";
    اگر ((T_UT == تہی || T_UT == خالی_خانے_option) && tgetflag("ut") > 0)
	T_UT = (char_u *)"y";
    اگر ((T_XON == تہی || T_XON == خالی_خانے_option) && tgetflag("xo") > 0)
	T_XON = (char_u *)"y";

    /*
     * get key codes
     */
    برائے (i = 0; کلید_نام[i] != تہی; ++i)
	اگر (ڈھونڈو_termcode((char_u *)کلید_نام[i]) == تہی)
	{
	    char_u *p = TGETSTR(کلید_نام[i], &tp);

	    // if cursor-left == backspace, ignore it (televideo 925)
	    اگر (p != تہی
		    && (*p != Ctrl_H
			|| کلید_نام[i][0] != 'k'
			|| کلید_نام[i][1] != 'l'))
		جوڑو_termcode((char_u *)کلید_نام[i], p, FALSE);
	}

    اگر (*اونچائی == 0)
	*اونچائی = tgetnum("li");
    اگر (*چوڑائی == 0)
	*چوڑائی = tgetnum("co");

    /*
     * Get number of colors (if not done already).
     */
    اگر (رکن_لڑیاں_not_سیٹ(KS_CCO))
    {
	سیٹ_color_گنتی(tgetnum("Co"));
# ifdef FEAT_EVAL
	سیٹ_رکن_option_sctx_idx("Co", -1);
# endif
    }

# ifndef hpux
    BC = (حرف *)TGETSTR("bc", &tp);
    اوپر_2 = (حرف *)TGETSTR("up", &tp);
    char_u *p = TGETSTR("pc", &tp);
    اگر (p != تہی)
	PC = *p;
# endif
}
#اگر_ختم

/*
 * Report "term" is not found and list the ones we do know about.
 */
    ساکن خالی
report_رکن_error(حرف *error_پیغام, char_u *رکن)
{
    mch_errmsg("\r\n");
    اگر (error_پیغام != تہی)
    {
	mch_errmsg(error_پیغام);
	mch_errmsg("\r\n");
    }
    mch_errmsg("'");
    mch_errmsg((حرف *)رکن);
    mch_errmsg(_("' not known. Available builtin terminals are:"));
    mch_errmsg("\r\n");

    برائے (صحیح i = 0; ; ++i)
    {
	حرف *name = builtin_terminals[i].bitc_name;
	اگر (name == تہی)  // end marker
	    توڑو;
	// Do not mention the "gui" entry, the user won't need to type it.
	اگر (STRCMP(name, "gui") != 0)
	{
#اگر_تعریف_شدہ HAVE_TGETENT
	    mch_errmsg("    builtin_");
#ورنہ
	    mch_errmsg("    ");
#اگر_ختم
	    mch_errmsg(name);
	    mch_errmsg("\r\n");
	}
    }
    // Output extra 'cmdheight' line breaks to avoid that the following error
    // message overwrites the last terminal name.
    برائے (صحیح i = 1; i < p_ch; ++i)
	mch_errmsg("\r\n");
}

    ساکن خالی
report_default_رکن(char_u *رکن)
{
    mch_errmsg(_("defaulting to '"));
    mch_errmsg((حرف *)رکن);
    mch_errmsg("'\r\n");
    اگر (emsg_silent == 0 && !اندر_assert_fails)
    {
	screen_آغاز();	// don't know where cursor is now
	برآمد_flush();
	اگر (!is_not_a_رکن())
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
مطابقت_keyprotocol(char_u *رکن)
{
    صحیح		len = (صحیح)STRLEN(p_kpc) + 1;
    char_u	*buf = alloc(len);
    اگر (buf == تہی)
	واپس KEYPROTOCOL_FAIL;

    keyprot_T	ret = KEYPROTOCOL_FAIL;
    char_u	*p = p_kpc;
    جب (*p != NUL)
    {
	// Isolate one comma separated item.
	(خالی)copy_option_part(&p, buf, len, ",");
	char_u *colon = vim_strchr(buf, ':');
	اگر (colon == تہی || colon == buf || colon[1] == NUL)
	    جاؤ theend;
	*colon = NUL;

	keyprot_T prot;
	// Note: Keep this in sync with p_kpc_protocol_values.
	اگر (STRCMP(colon + 1, "none") == 0)
	    prot = KEYPROTOCOL_NONE;
	ورنہ اگر (STRCMP(colon + 1, "mok2") == 0)
	    prot = KEYPROTOCOL_MOK2;
	ورنہ اگر (STRCMP(colon + 1, "kitty") == 0)
	    prot = KEYPROTOCOL_KITTY;
	ورنہ
	    جاؤ theend;

	regmatch_T regmatch;
	CLEAR_میدان(regmatch);
	regmatch.rm_ic = TRUE;
	regmatch.regprog = vim_regcomp(buf, RE_MAGIC);
	اگر (regmatch.regprog == تہی)
	    جاؤ theend;

	صحیح مطابقت = رکن != تہی && vim_regexec(&regmatch, رکن, (colnr_T)0);
	vim_regfree(regmatch.regprog);
	اگر (مطابقت)
	{
	    ret = prot;
	    جاؤ theend;
	}

    }

    // No match found, use "none".
    ret = KEYPROTOCOL_NONE;

theend:
    vim_free(buf);
    واپس ret;
}

/*
 * Set terminal options for terminal "term".
 * Return OK if terminal 'term' was found in a termcap, FAIL otherwise.
 *
 * While doing this, until ttest(), some options may be NULL, be careful.
 */
    صحیح
سیٹ_termname(char_u *رکن)
{
#اگر_تعریف_شدہ HAVE_TGETENT
    صحیح		builtin_پہلا = p_tbi;
    صحیح		try;
    صحیح		termcap_cleared = FALSE;
#اگر_ختم
    صحیح		چوڑائی = 0, اونچائی = 0;
    حرف	*error_پیغام = تہی;
    char_u	*bs_p, *del_p;

    // In silect mode (ex -s) we don't use the 'term' option.
    اگر (silent_منوال)
	واپس OK;

    detected_8bit = FALSE;		// reset 8-bit detection

    اگر (رکن_is_builtin(رکن))
    {
	رکن += 8;
#اگر_تعریف_شدہ HAVE_TGETENT
	builtin_پہلا = 1;
#اگر_ختم
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
#اگر_تعریف_شدہ HAVE_TGETENT
    برائے (try = builtin_پہلا ? 0 : 1; try < 3; ++try)
    {
	/*
	 * Use external termcap
	 */
	اگر (try == 1)
	{
	    char_u	    tbuf[TBUFSZ];

	    /*
	     * If the external termcap does not have a matching entry, try the
	     * builtin ones.
	     */
	    اگر ((error_پیغام = invoke_tgetent(tbuf, رکن)) == تہی)
	    {
		اگر (!termcap_cleared)
		{
		    clear_termoptions();	// clear old options
		    termcap_cleared = TRUE;
		}

		get_رکن_entries(&اونچائی, &چوڑائی);
	    }
	}
	ورنہ	    // try == 0 || try == 2
#اگر_ختم // HAVE_TGETENT
	/*
	 * Use builtin termcap
	 */
	{
#اگر_تعریف_شدہ HAVE_TGETENT
	    /*
	     * If builtin termcap was already used, there is no need to search
	     * for the builtin termcap again, quit now.
	     */
	    اگر (try == 2 && builtin_پہلا && termcap_cleared)
		توڑو;
#اگر_ختم
	    /*
	     * Search for 'term' in builtin_terminals[].
	     */
	    tcap_اندراج_T *termp = ڈھونڈو_builtin_رکن(رکن);
	    اگر (termp == تہی)	// did not find it
	    {
#اگر_تعریف_شدہ HAVE_TGETENT
		/*
		 * If try == 0, first try the external termcap. If that is not
		 * found we'll get back here with try == 2.
		 * If termcap_cleared is set we used the external termcap,
		 * don't complain about not finding the term in the builtin
		 * termcap.
		 */
		اگر (try == 0)			// try external one
		    جاری;
		اگر (termcap_cleared)		// found in external termcap
		    توڑو;
#اگر_ختم
		report_رکن_error(error_پیغام, رکن);

		// when user typed :set term=xxx, quit here
		اگر (starting != NO_SCREEN)
		{
		    screen_آغاز();	// don't know where cursor is now
		    wait_return(TRUE);
		    واپس FAIL;
		}
		رکن = DEFAULT_رکن;
		report_default_رکن(رکن);
		سیٹ_string_option_براہ_راست((char_u *)"term", -1, رکن,
								 OPT_FREE, 0);
		display_errors();
	    }
	    برآمد_flush();
#اگر_تعریف_شدہ HAVE_TGETENT
	    اگر (!termcap_cleared)
	    {
#اگر_ختم
		clear_termoptions();	    // clear old options
#اگر_تعریف_شدہ HAVE_TGETENT
		termcap_cleared = TRUE;
	    }
#اگر_ختم
	    parse_builtin_tcap(رکن);

#اگر_تعریف_شدہ FEAT_GUI
	    اگر (رکن_is_gui(رکن))
	    {
		برآمد_flush();
		gui_init();
		// If starting the GUI failed, don't do any of the other
		// things for this terminal
		اگر (!gui.اندر_use)
		    واپس FAIL;
# ifdef HAVE_TGETENT
		توڑو;		// don't try using external termcap
# endif
	    }
#اگر_ختم // FEAT_GUI
	}
#اگر_تعریف_شدہ HAVE_TGETENT
    }
#اگر_ختم

#اگر_تعریف_شدہ FEAT_GUI
    اگر (!gui.اندر_use)
#اگر_ختم
    {
	// Use the 'keyprotocol' option to adjust the t_TE and t_TI
	// termcap entries if there is an entry matching "term".
	keyprot_T kpc = مطابقت_keyprotocol(رکن);
	apply_keyprotocol(رکن, kpc);

#اگر_تعریف_شدہ FEAT_TERMGUICOLORS
	// There is no good way to detect that the terminal supports RGB
	// colors.  Since these termcap entries are non-standard anyway and
	// only used when the user sets 'termguicolors' we might as well add
	// them.  But not when one of them was already set.
	اگر (رکن_لڑیاں_not_سیٹ(KS_8F)
		&& رکن_لڑیاں_not_سیٹ(KS_8B)
		&& رکن_لڑیاں_not_سیٹ(KS_8U))
	    apply_builtin_tcap(رکن, builtin_rgb, TRUE);
#اگر_ختم
#اگر_تعریف_شدہ HAVE_TGETENT
	اگر (رکن_لڑیاں_not_سیٹ(KS_CF))
	    apply_builtin_tcap(رکن, special_رکن, TRUE);
	اگر (رکن_لڑیاں_not_سیٹ(KS_BSU) && رکن_لڑیاں_not_سیٹ(KS_ESU))
	    apply_builtin_tcap(رکن, builtin_sync_output, TRUE);
#اگر_ختم
    }

/*
 * special: There is no info in the termcap about whether the cursor
 * positioning is relative to the start of the screen or to the start of the
 * scrolling region.  We just guess here. Only msdos pcterm is known to do it
 * relative.
 */
    اگر (STRCMP(رکن, "pcterm") == 0)
	T_CCS = (char_u *)"yes";
    ورنہ
	T_CCS = خالی_خانے_option;

    // Special case: "kitty" may not have a "RV" entry in terminfo, but we need
    // to request the version for several other things to work.
    اگر (لڑی_تلاش((حرف *)رکن, "kitty") != تہی
					   && (T_CRV == تہی || *T_CRV == NUL))
	T_CRV = (char_u *)"\033[>c";

#اگر_تعریف_شدہ UNIX
/*
 * Any "stty" settings override the default for t_kb from the termcap.
 * This is in os_unix.c, because it depends a lot on the version of unix that
 * is being used.
 * Don't do this when the GUI is active, it uses "t_kb" and "t_kD" directly.
 */
# ifdef FEAT_GUI
    اگر (!gui.اندر_use)
# endif
	get_stty();
#اگر_ختم

/*
 * If the termcap has no entry for 'bs' and/or 'del' and the ioctl() also
 * didn't work, use the default CTRL-H
 * The default for t_kD is DEL, unless t_kb is DEL.
 * The vim_strsave'd strings are probably lost forever, well it's only two
 * bytes.  Don't do this when the GUI is active, it uses "t_kb" and "t_kD"
 * directly.
 */
#اگر_تعریف_شدہ FEAT_GUI
    اگر (!gui.اندر_use)
#اگر_ختم
    {
	bs_p = ڈھونڈو_termcode((char_u *)"kb");
	del_p = ڈھونڈو_termcode((char_u *)"kD");
	اگر (bs_p == تہی || *bs_p == NUL)
	    جوڑو_termcode((char_u *)"kb", (bs_p = (char_u *)CTRL_H_STR), FALSE);
	اگر ((del_p == تہی || *del_p == NUL) &&
					    (bs_p == تہی || *bs_p != DEL))
	    جوڑو_termcode((char_u *)"kD", (char_u *)DEL_STR, FALSE);
    }

#اگر defined(UNIX) || defined(VMS)
    رکن_is_xterm = vim_is_xterm(رکن);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_TERMRESPONSE
    // Reset terminal properties that are set based on the termresponse, which
    // will be sent out soon.
    init_رکن_props(FALSE);
#اگر_ختم

#اگر defined(UNIX) || defined(VMS)
    // If the first number in t_XM is 1006 then the terminal will support SGR
    // mouse reporting.
    صحیح did_سیٹ_ttym = FALSE;
    اگر (T_CXM != تہی && *T_CXM != NUL && !option_was_سیٹ((char_u *)"ttym"))
    {
	char_u *p = T_CXM;

	جب (*p != NUL && !VIM_ISDIGIT(*p))
	    ++p;
	اگر (getdigits(&p) == 1006)
	{
	    did_سیٹ_ttym = TRUE;
	    سیٹ_option_قدر_give_err((char_u *)"ttym", 0L, (char_u *)"sgr", 0);
	}
    }

    /*
     * For Unix, set the 'ttymouse' option to the type of mouse to be used.
     * The termcode for the mouse is added as a side effect in option.c.
     */
    {
	char_u	*p = (char_u *)"";

# ifdef FEAT_MOUSE_XTERM
	اگر (use_xterm_like_mouse(رکن))
	{
	    اگر (use_xterm_mouse())
		p = تہی;	// keep existing value, might be "xterm2"
	    ورنہ
		p = (char_u *)"xterm";
	}
# endif
	اگر (p != تہی && !did_سیٹ_ttym)
	{
	    سیٹ_option_قدر_give_err((char_u *)"ttym", 0L, p, 0);
	    // Reset the WAS_SET flag, 'ttymouse' can be set to "sgr" or
	    // "xterm2" in check_termcode().
	    reset_option_was_سیٹ((char_u *)"ttym");
	}
	اگر (p == تہی
# ifdef FEAT_GUI
		|| gui.اندر_use
# endif
		)
	    check_mouse_termcode();	// set mouse termcode anyway
    }
#ورنہ
    سیٹ_mouse_termcode(KS_MOUSE, (char_u *)"\233M");
#اگر_ختم

#اگر_تعریف_شدہ FEAT_MOUSE_XTERM
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
	جوڑو_termcode(name, (char_u *)"\033[I", FALSE);

	// handle focus out event
	name[1] = KE_FOCUSLOST;
	جوڑو_termcode(name, (char_u *)"\033[O", FALSE);

	need_gather = TRUE;
    }
#اگر_ختم
#اگر (defined(UNIX) || defined(VMS))
    // First time after setting 'term' a focus event is always reported.
    focus_state = MAYBE;
#اگر_ختم

#اگر_تعریف_شدہ USE_رکن_CONSOLE
    // DEFAULT_TERM indicates that it is the machine console.
    اگر (STRCMP(رکن, DEFAULT_رکن) != 0)
	رکن_console = FALSE;
    ورنہ
    {
	رکن_console = TRUE;
# ifdef AMIGA
	win_resize_on();	// enable window resizing reports
# endif
    }
#اگر_ختم

    ttest(TRUE);	// make sure we have a valid set of terminal codes

    بھرا_screen = TRUE;		// we can use termcap codes from now on
    سیٹ_رکن_defaults();	// use current values as defaults
#اگر_تعریف_شدہ FEAT_TERMRESPONSE
    LOG_TR1("setting crv_status to STATUS_GET");
    crv_حالت.tr_پیش_رفت = حالت_GET;	// Get terminal version later
    write_t_8u_state = FALSE;
#اگر_ختم

    /*
     * Initialize the terminal with the appropriate termcap codes.
     * Set the mouse and window title if possible.
     * Don't do this when starting, need to parse the .vimrc first, because it
     * may redefine t_TI etc.
     */
    اگر (starting != NO_SCREEN)
    {
	starttermcap();		// may change terminal mode
	setmouse();		// may start using the mouse
	maketitle();		// may display window title
    }

	// display initial screen after ttest() checking. jw.
    اگر (چوڑائی <= 0 || اونچائی <= 0)
    {
	// termcap failed to report size
	// set defaults, in case ui_get_shellsize() also fails
	چوڑائی = 80;
#اگر defined(MSWIN)
	اونچائی = 25;	    // console is often 25 lines
#ورنہ
	اونچائی = 24;	    // most terminals are 24 lines
#اگر_ختم
    }
    سیٹ_shellsize(چوڑائی, اونچائی, FALSE);	// may change Rows
    اگر (starting != NO_SCREEN)
    {
	اگر (scroll_region)
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
		اگر (curbuf->b_ml.ml_mfp != تہی)
		{
		    aucmd_prepbuf(&aco, buf);
		    اگر (curbuf == buf)
		    {
			apply_autocmds(EVENT_TERMCHANGED, تہی, تہی, FALSE,
								      curbuf);
			// restore curwin/curbuf and a few other things
			aucmd_restbuf(&aco);
		    }
		}
	    }
	}
    }

#اگر_تعریف_شدہ FEAT_TERMRESPONSE
    may_req_termresponse();
#اگر_ختم

#اگر_تعریف_شدہ FEAT_IMAGE
    (خالی)update_image_backend();
#اگر_ختم

    واپس OK;
}

#اگر defined(EXITFREE)

# ifdef HAVE_DEL_CURTERM
#  include <term.h>	    // declares cur_term
# endif

/*
 * If supported, delete "cur_term", which caches terminal related entries.
 * Avoids that valgrind reports possibly lost memory.
 */
    خالی
free_cur_رکن(خالی)
{
# ifdef HAVE_DEL_CURTERM
    اگر (cur_رکن)
	del_curterm(cur_رکن);
# endif
}

#اگر_ختم

#اگر_تعریف_شدہ HAVE_TGETENT
/*
 * Call tgetent()
 * Return error message if it fails, NULL if it's OK.
 */
    ساکن حرف *
invoke_tgetent(char_u *tbuf, char_u *رکن)
{
    صحیح	    i;

    // Note: Valgrind may report a leak here, because the library keeps one
    // buffer around that we can't ever free.
    i = TGETENT(tbuf, رکن);
    اگر (i < 0		    // -1 is always an error
# ifdef TGETENT_ZERO_ERR
	    || i == 0	    // sometimes zero is also an error
# endif
       )
    {
	// On FreeBSD tputs() gets a SEGV after a tgetent() which fails.  Call
	// tgetent() with the always existing "dumb" entry to avoid a crash or
	// hang.
	(خالی)TGETENT(tbuf, "dumb");

	اگر (i < 0)
# ifdef TGETENT_ZERO_ERR
	    واپس _(e_cannot_open_termcap_file);
	اگر (i == 0)
# endif
# ifdef TERMINFO
	    واپس _(e_terminal_اندراج_not_ملا_اندر_terminfo);
# ورنہ
	    واپس _(e_terminal_اندراج_not_ملا_اندر_termcap);
# endif
    }
    واپس تہی;
}

/*
 * Some versions of tgetstr() have been reported to return -1 instead of NULL.
 * Fix that here.
 */
    ساکن char_u *
vim_tgetstr(حرف *s, char_u **pp)
{
    حرف	*p;

    p = tgetstr(s, (حرف **)pp);
    اگر (p == (حرف *)-1)
	p = تہی;
    واپس (char_u *)p;
}
#اگر_ختم // HAVE_TGETENT

#اگر defined(HAVE_TGETENT) && (defined(UNIX) || defined(VMS) || defined(MACOS_X))
/*
 * Get Columns and Rows from the termcap. Used after a window signal if the
 * ioctl() fails. It doesn't make sense to call tgetent each time if the "co"
 * and "li" entries never change. But on some systems this works.
 * Errors while getting the entries are ignored.
 */
    خالی
getlinecol(
    طویل	*cp,	// pointer to columns
    طویل	*rp)	// pointer to rows
{
    char_u	tbuf[TBUFSZ];

    اگر (T_NAME == تہی || *T_NAME == NUL
	    || invoke_tgetent(tbuf, T_NAME) != تہی)
	واپس;

    اگر (*cp == 0)
	*cp = tgetnum("co");
    اگر (*rp == 0)
	*rp = tgetnum("li");
}
#اگر_ختم // defined(HAVE_TGETENT) && defined(UNIX)

/*
 * Get a string entry from the termcap and add it to the list of termcodes.
 * Used for <t_xx> special keys.
 * Give an error message for failure when not sourcing.
 * If force given, replace an existing entry.
 * Return FAIL if the entry was not found, OK if the entry was added.
 */
    صحیح
جوڑو_termcap_اندراج(char_u *name, صحیح force)
{
    char_u  *رکن;
    صحیح	    کلید;
#اگر_تعریف_شدہ HAVE_TGETENT
    char_u  *string;
    صحیح	    i;
    صحیح	    builtin_پہلا;
    char_u  tbuf[TBUFSZ];
    char_u  tstrbuf[TBUFSZ];
    char_u  *tp = tstrbuf;
    حرف    *error_پیغام = تہی;
#اگر_ختم

/*
 * If the GUI is running or will start in a moment, we only support the keys
 * that the GUI can produce.
 */
#اگر_تعریف_شدہ FEAT_GUI
    اگر (gui.اندر_use || gui.starting)
	واپس gui_mch_haskey(name);
#اگر_ختم

    اگر (!force && ڈھونڈو_termcode(name) != تہی)	    // it's already there
	واپس OK;

    رکن = T_NAME;
    اگر (رکن == تہی || *رکن == NUL)	    // 'term' not defined yet
	واپس FAIL;

    اگر (رکن_is_builtin(رکن))		    // name starts with "builtin_"
    {
	رکن += 8;
#اگر_تعریف_شدہ HAVE_TGETENT
	builtin_پہلا = TRUE;
#اگر_ختم
    }
#اگر_تعریف_شدہ HAVE_TGETENT
    ورنہ
	builtin_پہلا = p_tbi;
#اگر_ختم

#اگر_تعریف_شدہ HAVE_TGETENT
/*
 * We can get the entry from the builtin termcap and from the external one.
 * If 'ttybuiltin' is on or the terminal name starts with "builtin_", try
 * builtin termcap first.
 * If 'ttybuiltin' is off, try external termcap first.
 */
    برائے (i = 0; i < 2; ++i)
    {
	اگر ((!builtin_پہلا) == i)
#اگر_ختم
	/*
	 * Search in builtin termcaps
	 */
	{
	    tcap_اندراج_T *termp = ڈھونڈو_builtin_رکن(رکن);
	    اگر (termp != تہی)	// found it
	    {
		کلید = TERMCAP2_کلید(name[0], name[1]);
		++termp;
		جب (termp->bt_اندراج != (صحیح)KS_NAME)
		{
		    اگر ((صحیح)termp->bt_اندراج == کلید)
		    {
			جوڑو_termcode(name, (char_u *)termp->bt_string,
							  رکن_is_8bit(رکن));
			واپس OK;
		    }
		    ++termp;
		}
	    }
	}
#اگر_تعریف_شدہ HAVE_TGETENT
	ورنہ
	/*
	 * Search in external termcap
	 */
	{
	    error_پیغام = invoke_tgetent(tbuf, رکن);
	    اگر (error_پیغام == تہی)
	    {
		string = TGETSTR((حرف *)name, &tp);
		اگر (string != تہی && *string != NUL)
		{
		    جوڑو_termcode(name, string, FALSE);
		    واپس OK;
		}
	    }
	}
    }
#اگر_ختم

    اگر (SOURCING_NAME == تہی)
    {
#اگر_تعریف_شدہ HAVE_TGETENT
	اگر (error_پیغام != تہی)
	    emsg(error_پیغام);
	ورنہ
#اگر_ختم
	    semsg(_(e_no_str_اندراج_اندر_termcap), name);
    }
    واپس FAIL;
}

    ساکن صحیح
رکن_is_builtin(char_u *name)
{
    واپس (STRNCMP(name, "builtin_", (size_t)8) == 0);
}

/*
 * Return TRUE if terminal "name" uses CSI instead of <Esc>[.
 * Assume that the terminal is using 8-bit controls when the name contains
 * "8bit", like in "xterm-8bit".
 */
    صحیح
رکن_is_8bit(char_u *name)
{
    واپس (detected_8bit || لڑی_تلاش((حرف *)name, "8bit") != تہی);
}

/*
 * Translate terminal control chars from 7-bit to 8-bit:
 * <Esc>[ -> CSI  <M_C_[>
 * <Esc>] -> OSC  <M-C-]>
 * <Esc>O -> <M-C-O>
 */
    ساکن صحیح
رکن_7to8bit(char_u *p)
{
    اگر (*p != ESC)
	واپس 0;

    اگر (p[1] == '[')
	واپس CSI;
    ورنہ اگر (p[1] == ']')
	واپس OSC;
    ورنہ اگر (p[1] == 'O')
	واپس 0x8f;
    واپس 0;
}

#اگر defined(FEAT_GUI)
    صحیح
رکن_is_gui(char_u *name)
{
    واپس (STRCMP(name, "builtin_gui") == 0 || STRCMP(name, "gui") == 0);
}
#اگر_ختم

#اگر !defined(HAVE_TGETENT) || defined(AMIGA)

    char_u *
tltoa(غیرعلامتی طویل i)
{
    ساکن char_u buf[16];
    char_u	*p;

    p = buf + 15;
    *p = '\0';
    کرو
    {
	--p;
	*p = (char_u) (i % 10 + '0');
	i /= 10;
    }
    جب (i > 0 && p > buf);
    واپس p;
}
#اگر_ختم

#اگر_غیر_تعریف_شدہ HAVE_TGETENT

/*
 * minimal tgoto() implementation.
 * no padding and we only parse for %i %d and %+char
 */
    ساکن حرف *
tgoto(حرف *cm, صحیح x, صحیح y)
{
    ساکن حرف buf[30];
    حرف *p, *s, *e;

    اگر (!cm)
	واپس "OOPS";
    e = buf + 29;
    برائے (s = buf; s < e && *cm; cm++)
    {
	اگر (*cm != '%')
	{
	    *s++ = *cm;
	    جاری;
	}
	انتخاب (*++cm)
	{
	حالت 'd':
	    p = (حرف *)tltoa((غیرعلامتی طویل)y);
	    y = x;
	    جب (*p)
		*s++ = *p++;
	    توڑو;
	حالت 'i':
	    x++;
	    y++;
	    توڑو;
	حالت '+':
	    *s++ = (حرف)(*++cm + y);
	    y = x;
	    توڑو;
	حالت '%':
	    *s++ = *cm;
	    توڑو;
	طےشدہ:
	    واپس "OOPS";
	}
    }
    *s = '\0';
    واپس buf;
}

#اگر_ختم // HAVE_TGETENT

/*
 * Set the terminal name and initialize the terminal options.
 * If "name" is NULL or empty, get the terminal name from the environment.
 * If that fails, use the default terminal name.
 */
    خالی
termcapinit(char_u *name)
{
    char_u	*رکن = name;

    اگر (رکن != تہی && *رکن == NUL)
	رکن = تہی;	    // empty name is equal to no name

#اگر_غیر_تعریف_شدہ MSWIN
    اگر (رکن == تہی)
	رکن = mch_getenv((char_u *)"TERM");
#اگر_ختم
    اگر (رکن == تہی || *رکن == NUL)
	رکن = DEFAULT_رکن;
    سیٹ_string_option_براہ_راست((char_u *)"term", -1, رکن, OPT_FREE, 0);

    // Set the default terminal name.
    سیٹ_string_default("term", رکن);
    سیٹ_string_default("ttytype", رکن);

    // Avoid using "term" here, because the next mch_getenv() may overwrite it.
    سیٹ_termname(T_NAME != تہی ? T_NAME : رکن);
}

/*
 * The number of calls to ui_write is reduced by using "out_buf".
 */
#تعریف_کرو برآمد_سائز	8191

// add one to allow mch_write() in os_win32.c to append a NUL
ساکن char_u		برآمد_buf[برآمد_سائز + 1];

ساکن صحیح		برآمد_pos = 0;	// number of chars in out_buf

// Since the maximum number of SGR parameters shown as a normal value range is
// 16, the escape sequence length can be 4 * 16 + lead + tail.
#تعریف_کرو MAX_ESC_SEQ_LEN	80

/*
 * out_flush(): flush the output buffer
 */
    خالی
برآمد_flush(خالی)
{
    صحیح	    len;

    اگر (برآمد_pos == 0)
	واپس;

    // set out_pos to 0 before ui_write, to avoid recursiveness
    len = برآمد_pos;
    برآمد_pos = 0;
    ui_write(برآمد_buf, len, FALSE);
#اگر_تعریف_شدہ FEAT_EVAL
    اگر (ch_log_output != FALSE)
    {
	برآمد_buf[len] = NUL;
	ch_log(تہی, "raw %s output: \"%s\"",
# ifdef FEAT_GUI
		(gui.اندر_use && !gui.dying && !gui.starting) ? "GUI" :
# endif
		"terminal",
		برآمد_buf);
	اگر (ch_log_output == TRUE)
	    ch_log_output = FALSE;  // only log once
    }
#اگر_ختم
}

/*
 * out_flush_cursor(): flush the output buffer and redraw the cursor.
 * Does not flush recursively in the GUI to avoid slow drawing.
 */
    خالی
برآمد_flush_cursor(
    صحیح	    force غیر_مستعمل,   // when TRUE, update cursor even when not moved
    صحیح	    clear_selection غیر_مستعمل) // clear selection under cursor
{
    mch_disable_flush();
    برآمد_flush();
    mch_enable_flush();
#اگر_تعریف_شدہ FEAT_GUI
    اگر (gui.اندر_use)
    {
	gui_update_cursor(force, clear_selection);
	gui_may_flush();
    }
#اگر_ختم
}


/*
 * Sometimes a byte out of a multi-byte character is written with out_char().
 * To avoid flushing half of the character, call this function first.
 */
    خالی
برآمد_flush_check(خالی)
{
    اگر (enc_dbcs != 0 && برآمد_pos >= برآمد_سائز - MB_MAXBYTES)
	برآمد_flush();
}

#اگر_تعریف_شدہ FEAT_GUI
/*
 * out_trash(): Throw away the contents of the output buffer
 */
    خالی
برآمد_trash(خالی)
{
    برآمد_pos = 0;
}
#اگر_ختم

/*
 * out_char(c): put a byte into the output buffer.
 *		Flush it if it becomes full.
 * This should not be used for outputting text on the screen (use functions
 * like msg_puts() and screen_putchar() for that).
 */
    خالی
برآمد_char(غیرعلامتی c)
{
#اگر defined(UNIX) || defined(VMS) || defined(AMIGA) || defined(MACOS_X)
    اگر (c == '\n')	// turn LF into CR-LF (CRMOD doesn't seem to do this)
	برآمد_char('\r');
#اگر_ختم

    برآمد_buf[برآمد_pos++] = c;

    // For testing we flush each time.
    اگر (برآمد_pos >= برآمد_سائز || p_wd)
	برآمد_flush();
}

/*
 * Output "c" like out_char(), but don't flush when p_wd is set.
 */
    ساکن صحیح
برآمد_char_nf(صحیح c)
{
    برآمد_buf[برآمد_pos++] = (غیرعلامتی)c;

    اگر (برآمد_pos >= برآمد_سائز)
	برآمد_flush();
    واپس (غیرعلامتی)c;
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
    خالی
برآمد_str_nf(char_u *s)
{
    // avoid terminal strings being split up
    اگر (برآمد_pos > برآمد_سائز - MAX_ESC_SEQ_LEN)
	برآمد_flush();

    برائے (char_u *p = s; *p != NUL; ++p)
	برآمد_char_nf(*p);

    // For testing we write one string at a time.
    اگر (p_wd)
	برآمد_flush();
}

/*
 * A conditional-flushing out_str, mainly for visualbell.
 * Handles a delay internally, because termlib may not respect the delay or do
 * it at the wrong time.
 * Note: Only for terminal strings.
 */
    خالی
برآمد_str_cf(char_u *s)
{
    اگر (s == تہی || *s == NUL)
	واپس;

#اگر_تعریف_شدہ HAVE_TGETENT
    char_u *p;
#اگر_ختم

#اگر_تعریف_شدہ FEAT_GUI
    // Don't use tputs() when GUI is used, ncurses crashes.
    اگر (gui.اندر_use)
    {
	برآمد_str_nf(s);
	واپس;
    }
#اگر_ختم
    اگر (برآمد_pos > برآمد_سائز - MAX_ESC_SEQ_LEN)
	برآمد_flush();
#اگر_تعریف_شدہ HAVE_TGETENT
    برائے (p = s; *s; ++s)
    {
	// flush just before delay command
	اگر (*s == '$' && *(s + 1) == '<')
	{
	    char_u save_c = *s;
	    صحیح duration = atoi((حرف *)s + 2);

	    *s = NUL;
	    tputs((حرف *)p, 1, TPUTSFUNCAST برآمد_char_nf);
	    *s = save_c;
	    برآمد_flush();
# ifdef ELAPSED_FUNC
	    // Only sleep here if we can limit this happening in
	    // vim_beep().
	    p = vim_strchr(s, '>');
	    اگر (p == تہی || duration <= 0)
	    {
		// can't parse the time, don't sleep here
		p = s;
	    }
	    ورنہ
	    {
		++p;
		do_sleep(duration, FALSE);
	    }
# ورنہ
	    // Rely on the terminal library to sleep.
	    p = s;
# endif
	    توڑو;
	}
    }
    tputs((حرف *)p, 1, TPUTSFUNCAST برآمد_char_nf);
#ورنہ
    جب (*s)
	برآمد_char_nf(*s++);
#اگر_ختم

    // For testing we write one string at a time.
    اگر (p_wd)
	برآمد_flush();
}

/*
 * out_str(s): Put a character string a byte at a time into the output buffer.
 * If HAVE_TGETENT is defined use tputs(), the termcap parser. (jw)
 * This should only be used for writing terminal codes, not for outputting
 * normal text (use functions like msg_puts() and screen_putchar() for that).
 */
    خالی
برآمد_str(char_u *s)
{
    اگر (s == تہی || *s == NUL)
	واپس;

#اگر_تعریف_شدہ FEAT_GUI
    // Don't use tputs() when GUI is used, ncurses crashes.
    اگر (gui.اندر_use)
    {
	برآمد_str_nf(s);
	واپس;
    }
#اگر_ختم
    // avoid terminal strings being split up
    اگر (برآمد_pos > برآمد_سائز - MAX_ESC_SEQ_LEN)
	برآمد_flush();
#اگر_تعریف_شدہ HAVE_TGETENT
    tputs((حرف *)s, 1, TPUTSFUNCAST برآمد_char_nf);
#ورنہ
    جب (*s)
	برآمد_char_nf(*s++);
#اگر_ختم

    // For testing we write one string at a time.
    اگر (p_wd)
	برآمد_flush();
}

/*
 * cursor positioning using termcap parser. (jw)
 */
    خالی
رکن_windgoto(صحیح صف, صحیح ستون)
{
    برآمد_STR(tgoto((حرف *)T_CM, ستون, صف));
}

    خالی
رکن_cursor_دایاں(صحیح i)
{
    برآمد_STR(tgoto((حرف *)T_CRI, 0, i));
}

    خالی
رکن_append_سطریں(صحیح سطر_گنتی)
{
    برآمد_STR(tgoto((حرف *)T_CAL, 0, سطر_گنتی));
}

    خالی
رکن_delete_سطریں(صحیح سطر_گنتی)
{
    برآمد_STR(tgoto((حرف *)T_CDL, 0, سطر_گنتی));
}

#اگر defined(UNIX) || defined(VMS)
    خالی
رکن_enable_mouse(صحیح enable)
{
    صحیح on = enable ? 1 : 0;
    برآمد_STR(tgoto((حرف *)T_CXM, 0, on));
}
#اگر_ختم

#اگر defined(HAVE_TGETENT)
    خالی
رکن_سیٹ_winpos(صحیح x, صحیح y)
{
    // Can't handle a negative value here
    اگر (x < 0)
	x = 0;
    اگر (y < 0)
	y = 0;
    برآمد_STR(tgoto((حرف *)T_CWP, y, x));
}

# اگر defined(FEAT_TERMRESPONSE)
/*
 * Return TRUE if we can request the terminal for a response.
 */
    ساکن صحیح
can_get_termresponse(خالی)
{
    واپس cur_tmode == TMODE_RAW
	    && termcap_فعال
#  ifdef UNIX
	    && (is_not_a_رکن() || (isatty(1) && isatty(read_cmd_fd)))
#  endif
	    && p_ek;
}

/*
 * Set "status" to STATUS_SENT.
 */
    ساکن خالی
termrequest_بھیجا(termrequest_T *حالت_2)
{
    حالت_2->tr_پیش_رفت = حالت_بھیجا;
    حالت_2->tr_آغاز = وقت(تہی);
}

/*
 * Return TRUE if any of the requests are in STATUS_SENT.
 */
    ساکن صحیح
termrequest_any_pending(خالی)
{
    صحیح	    i;
    time_t  now = وقت(تہی);

    برائے (i = 0; all_termrequests[i] != تہی; ++i)
    {
	اگر (all_termrequests[i]->tr_پیش_رفت == حالت_بھیجا)
	{
	    اگر (all_termrequests[i]->tr_آغاز > 0 && now > 0
				    && all_termrequests[i]->tr_آغاز + 2 < now)
		// Sent the request more than 2 seconds ago and didn't get a
		// response, assume it failed.
		all_termrequests[i]->tr_پیش_رفت = حالت_FAIL;
	    ورنہ
		واپس TRUE;
	}
    }
    واپس FALSE;
}

ساکن صحیح winpos_x = -1;
ساکن صحیح winpos_y = -1;
ساکن صحیح did_request_winpos = 0;

#  اگر defined(FEAT_EVAL) || defined(FEAT_TERMINAL)
/*
 * Try getting the Vim window position from the terminal.
 * Returns OK or FAIL.
 */
    صحیح
رکن_get_winpos(صحیح *x, صحیح *y, varnumber_T timeout)
{
    صحیح گنتی = 0;
    صحیح پچھلا_winpos_x = winpos_x;
    صحیح پچھلا_winpos_y = winpos_y;

    اگر (*T_CGP == NUL || !can_get_termresponse())
	واپس FAIL;
    winpos_x = -1;
    winpos_y = -1;
    ++did_request_winpos;
    termrequest_بھیجا(&winpos_حالت);
    برآمد_STR(T_CGP);
    برآمد_flush();

    // Try reading the result for "timeout" msec.
    جب (گنتی++ <= timeout / 10 && !got_int)
    {
	(خالی)vpeekc_nomap();
	اگر (winpos_x >= 0 && winpos_y >= 0)
	{
	    *x = winpos_x;
	    *y = winpos_y;
	    واپس OK;
	}
	ui_delay(10L, FALSE);
    }
    // Do not reset "did_request_winpos", if we timed out the response might
    // still come later and we must consume it.

    winpos_x = پچھلا_winpos_x;
    winpos_y = پچھلا_winpos_y;
    اگر (timeout < 10 && پچھلا_winpos_y >= 0 && پچھلا_winpos_x >= 0)
    {
	// Polling: return previous values if we have them.
	*x = winpos_x;
	*y = winpos_y;
	واپس OK;
    }

    واپس FALSE;
}
#  endif
# endif

    خالی
رکن_سیٹ_winsize(صحیح اونچائی, صحیح چوڑائی)
{
    برآمد_STR(tgoto((حرف *)T_CWS, چوڑائی, اونچائی));
}
#اگر_ختم

    خالی
رکن_font(صحیح n)
{
    اگر (*T_CFO)
    {
	حرف buf[20];
	sprintf(buf, (حرف *)T_CFO, 9 + n);
	برآمد_STR(buf);
    }
}

    ساکن خالی
رکن_color(char_u *s, صحیح n)
{
    حرف	buf[20];
    صحیح		i = *s == CSI ? 1 : 2;
		// index in s[] just after <Esc>[ or CSI

    // Special handling of 16 colors, because termcap can't handle it
    // Also accept "\e[3%dm" for TERMINFO, it is sometimes used
    // Also accept CSI instead of <Esc>[
    اگر (n >= 8 && t_colors >= 16
	      && ((s[0] == ESC && s[1] == '[')
#اگر defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
		  || (s[0] == ESC && s[1] == '|')
#اگر_ختم
		  || (s[0] == CSI && (i = 1) == 1))
	      && s[i] != NUL
	      && (STRCMP(s + i + 1, "%p1%dm") == 0
		  || STRCMP(s + i + 1, "%dm") == 0)
	      && (s[i] == '3' || s[i] == '4'))
    {
#اگر_تعریف_شدہ TERMINFO
	حرف *format = "%s%s%%p1%%dm";
#ورنہ
	حرف *format = "%s%s%%dm";
#اگر_ختم
	حرف *lead = i == 2 ? (
#اگر defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
		    s[1] == '|' ? "\033|" :
#اگر_ختم
		    "\033[") : "\233";
	حرف *دم = s[i] == '3' ? (n >= 16 ? "38;5;" : "9")
				 : (n >= 16 ? "48;5;" : "10");

	sprintf(buf, format, lead, دم);
	برآمد_STR(tgoto(buf, 0, n >= 16 ? n : n - 8));
    }
    ورنہ
	برآمد_STR(tgoto((حرف *)s, 0, n));
}

    خالی
رکن_fg_color(صحیح n)
{
    // Use "AF" termcap entry if present, "Sf" entry otherwise
    اگر (*T_CAF)
	رکن_color(T_CAF, n);
    ورنہ اگر (*T_CSF)
	رکن_color(T_CSF, n);
}

    خالی
رکن_bg_color(صحیح n)
{
    // Use "AB" termcap entry if present, "Sb" entry otherwise
    اگر (*T_CAB)
	رکن_color(T_CAB, n);
    ورنہ اگر (*T_CSB)
	رکن_color(T_CSB, n);
}

    خالی
رکن_ul_color(صحیح n)
{
    اگر (*T_CAU)
	رکن_color(T_CAU, n);
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
رکن_bg_default(خالی)
{
#اگر defined(MSWIN)
    // DOS console is nearly always black
    واپس (char_u *)"dark";
#ورنہ
    char_u	*p;

    اگر (STRCMP(T_NAME, "linux") == 0
	    || STRCMP(T_NAME, "screen.linux") == 0
	    || STRNCMP(T_NAME, "cygwin", 6) == 0
	    || STRNCMP(T_NAME, "putty", 5) == 0
	    || ((p = mch_getenv((char_u *)"COLORFGBG")) != تہی
		&& (p = vim_strrchr(p, ';')) != تہی
		&& ((p[1] >= '0' && p[1] <= '6') || p[1] == '8')
		&& p[2] == NUL))
	واپس (char_u *)"dark";
    واپس (char_u *)"light";
#اگر_ختم
}

#اگر defined(FEAT_TERMGUICOLORS)

# define RED(rgb)   (((long_u)(rgb) >> 16) & 0xFF)
# define GREEN(rgb) (((long_u)(rgb) >>  8) & 0xFF)
# define BLUE(rgb)  (((long_u)(rgb)      ) & 0xFF)

    ساکن خالی
رکن_rgb_color(char_u *s, guicolor_T rgb)
{
# define MAX_COLOR_STR_LEN 100
    حرف	buf[MAX_COLOR_STR_LEN];

    اگر (*s == NUL)
	واپس;
    vim_snprintf(buf, MAX_COLOR_STR_LEN,
				  (حرف *)s, RED(rgb), GREEN(rgb), BLUE(rgb));
# ifdef FEAT_VTP
    اگر (use_vtp() && (p_tgc || t_colors >= 256))
    {
	برآمد_flush();
	buf[1] = '[';
	vtp_printf(buf);
    }
    ورنہ
# endif
	برآمد_STR(buf);
}

    خالی
رکن_fg_rgb_color(guicolor_T rgb)
{
    اگر (rgb != INVALCOLOR)
	رکن_rgb_color(T_8F, rgb);
}

    خالی
رکن_bg_rgb_color(guicolor_T rgb)
{
    اگر (rgb != INVALCOLOR)
	رکن_rgb_color(T_8B, rgb);
}

    خالی
رکن_ul_rgb_color(guicolor_T rgb)
{
# ifdef FEAT_TERMRESPONSE
    // If the user explicitly sets t_8u then use it.  Otherwise wait for
    // termresponse to be received, which is when t_8u would be set and a
    // redraw is needed if it was used.
    اگر (!option_was_سیٹ((char_u *)"t_8u") && write_t_8u_state != OK)
	write_t_8u_state = MAYBE;
    ورنہ
# endif
	رکن_rgb_color(T_8U, rgb);
}
#اگر_ختم

#اگر defined(UNIX) || defined(VMS) || defined(MACOS_X)
/*
 * Generic function to set window title, using t_ts and t_fs.
 */
    خالی
رکن_settitle(char_u *title)
{
    MAY_WANT_مقصد_LOG_THIS;

    // t_ts takes one argument: column in status line
    برآمد_STR(tgoto((حرف *)T_TS, 0, 0));	// set title start
    برآمد_str_nf(title);
    برآمد_str(T_FS);			// set title end
    برآمد_flush();
}

/*
 * Tell the terminal to push (save) the title and/or icon, so that it can be
 * popped (restored) later.
 */
    خالی
رکن_ڈالو_title(صحیح which)
{
    اگر ((which & SAVE_RESTORE_TITLE) && T_CST != تہی && *T_CST != NUL)
    {
	برآمد_STR(T_CST);
	برآمد_flush();
    }

    اگر ((which & SAVE_RESTORE_ICON) && T_SSI != تہی && *T_SSI != NUL)
    {
	برآمد_STR(T_SSI);
	برآمد_flush();
    }
}

/*
 * Tell the terminal to pop the title and/or icon.
 */
    خالی
رکن_نکالو_title(صحیح which)
{
    اگر ((which & SAVE_RESTORE_TITLE) && T_CRT != تہی && *T_CRT != NUL)
    {
	برآمد_STR(T_CRT);
	برآمد_flush();
    }

    اگر ((which & SAVE_RESTORE_ICON) && T_SRI != تہی && *T_SRI != NUL)
    {
	برآمد_STR(T_SRI);
	برآمد_flush();
    }
}
#اگر_ختم

/*
 * Make sure we have a valid set or terminal options.
 * Replace all entries that are NULL by empty_option
 */
    خالی
ttest(صحیح جوڑے)
{
    char_u *env_colors;

    check_options();		    // make sure no options are NULL

    /*
     * MUST have "cm": cursor motion.
     */
    اگر (*T_CM == NUL)
	emsg(_(e_terminal_capability_cm_required));

    /*
     * if "cs" defined, use a scroll region, it's faster.
     */
    اگر (*T_CS != NUL)
	scroll_region = TRUE;
    ورنہ
	scroll_region = FALSE;

    اگر (جوڑے)
    {
	/*
	 * optional pairs
	 */
	// TP goes to normal mode for TI (invert) and TB (bold)
	اگر (*T_ME == NUL)
	    T_ME = T_MR = T_MD = T_MB = خالی_خانے_option;
	اگر (*T_SO == NUL || *T_SE == NUL)
	    T_SO = T_SE = خالی_خانے_option;
	اگر (*T_US == NUL || *T_UE == NUL)
	    T_US = T_UE = خالی_خانے_option;
	اگر (*T_CZH == NUL || *T_CZR == NUL)
	    T_CZH = T_CZR = خالی_خانے_option;

	// T_VE is needed even though T_VI is not defined
	اگر (*T_VE == NUL)
	    T_VI = خالی_خانے_option;

	// if 'mr' or 'me' is not defined use 'so' and 'se'
	اگر (*T_ME == NUL)
	{
	    T_ME = T_SE;
	    T_MR = T_SO;
	    T_MD = T_SO;
	}

	// if 'so' or 'se' is not defined use 'mr' and 'me'
	اگر (*T_SO == NUL)
	{
	    T_SE = T_ME;
	    اگر (*T_MR == NUL)
		T_SO = T_MD;
	    ورنہ
		T_SO = T_MR;
	}

	// if 'ZH' or 'ZR' is not defined use 'mr' and 'me'
	اگر (*T_CZH == NUL)
	{
	    T_CZR = T_ME;
	    اگر (*T_MR == NUL)
		T_CZH = T_MD;
	    ورنہ
		T_CZH = T_MR;
	}

	// "Sb" and "Sf" come in pairs
	اگر (*T_CSB == NUL || *T_CSF == NUL)
	{
	    T_CSB = خالی_خانے_option;
	    T_CSF = خالی_خانے_option;
	}

	// "AB" and "AF" come in pairs
	اگر (*T_CAB == NUL || *T_CAF == NUL)
	{
	    T_CAB = خالی_خانے_option;
	    T_CAF = خالی_خانے_option;
	}

	// if 'Sb' and 'AB' are not defined, reset "Co"
	اگر (*T_CSB == NUL && *T_CAB == NUL)
	    free_one_termoption(T_CCO);

	// Set 'weirdinvert' according to value of 't_xs'
	p_wiv = (*T_XS != NUL);
    }
    need_gather = TRUE;

    // Set t_colors to the value of $COLORS or t_Co.  Ignore $COLORS in the
    // GUI.
    t_colors = atoi((حرف *)T_CCO);
#اگر_تعریف_شدہ FEAT_GUI
    اگر (!gui.اندر_use)
#اگر_ختم
    {
	env_colors = mch_getenv((char_u *)"COLORS");
	اگر (env_colors != تہی && محفوظ_isdigit(*env_colors))
	{
	    صحیح colors = atoi((حرف *)env_colors);

	    اگر (colors != t_colors)
		سیٹ_color_گنتی(colors);
	}
    }
    سیٹ_rgb_رکن_prop();
}

#اگر defined(FEAT_GUI) && (defined(FEAT_MENU) || !defined(USE_ON_FLY_SCROLL))
/*
 * Represent the given long_u as individual bytes, with the most significant
 * byte first, and store them in dst.
 */
    خالی
جوڑو_long_مقصد_buf(long_u val, char_u *dst)
{
    صحیح	    i;
    صحیح	    سرکاؤ;

    برائے (i = 1; i <= (صحیح)حجم(long_u); i++)
    {
	سرکاؤ = 8 * (حجم(long_u) - i);
	dst[i - 1] = (char_u) ((val >> سرکاؤ) & 0xff);
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
    ساکن صحیح
get_long_from_buf(char_u *buf, long_u *val)
{
    صحیح	    len;
    char_u  بائٹیں[حجم(long_u)];
    صحیح	    i;
    صحیح	    سرکاؤ;

    *val = 0;
    len = get_بائٹیں_from_buf(buf, بائٹیں, (صحیح)حجم(long_u));
    اگر (len == -1)
	واپس -1;
    برائے (i = 0; i < (صحیح)حجم(long_u); i++)
    {
	سرکاؤ = 8 * (حجم(long_u) - 1 - i);
	*val += (long_u)بائٹیں[i] << سرکاؤ;
    }
    واپس len;
}
#اگر_ختم

/*
 * Read the next num_bytes bytes from buf, and store them in bytes.  Assume
 * that buf has been through inchar().	Returns the actual number of bytes used
 * from buf (between num_bytes and num_bytes*2), or -1 if not enough bytes were
 * available.
 */
    صحیح
get_بائٹیں_from_buf(char_u *buf, char_u *بائٹیں, صحیح عدد_بائٹیں)
{
    صحیح	    len = 0;
    صحیح	    i;
    char_u  c;

    برائے (i = 0; i < عدد_بائٹیں; i++)
    {
	اگر ((c = buf[len++]) == NUL)
	    واپس -1;
	اگر (c == K_SPECIAL)
	{
	    اگر (buf[len] == NUL || buf[len + 1] == NUL)	    // cannot happen?
		واپس -1;
	    اگر (buf[len++] == (صحیح)KS_ZERO)
		c = NUL;
	    // else it should be KS_SPECIAL; when followed by KE_FILLER c is
	    // K_SPECIAL, or followed by KE_CSI and c must be CSI.
	    اگر (buf[len++] == (صحیح)KE_CSI)
		c = CSI;
	}
	ورنہ اگر (c == CSI && buf[len] == KS_EXTRA
					       && buf[len + 1] == (صحیح)KE_CSI)
	    // CSI is stored as CSI KS_SPECIAL KE_CSI to avoid confusion with
	    // the start of a special key, see add_to_input_buf_csi().
	    len += 2;
	بائٹیں[i] = c;
    }
    واپس len;
}

/*
 * Check if the new shell size is valid, correct it if it's too small or way
 * too big.
 */
    خالی
check_shellsize(خالی)
{
    // need room for one window and command line
    اگر (صفیں < min_صفیں_for_all_tabpages())
	صفیں = min_صفیں_for_all_tabpages();
    حد_بندی_screen_سائز();

    // make sure these values are not invalid
    اگر (cmdline_صف >= صفیں)
	cmdline_صف = صفیں - 1;
    اگر (پیغام_صف >= صفیں)
	پیغام_صف = صفیں - 1;
}

/*
 * Limit Rows and Columns to avoid an overflow in Rows * Columns.
 */
    خالی
حد_بندی_screen_سائز(خالی)
{
    اگر (Columns < MIN_COLUMNS)
	Columns = MIN_COLUMNS;
    ورنہ اگر (Columns > 10000)
	Columns = 10000;
    اگر (صفیں > 1000)
	صفیں = 1000;
}

/*
 * Invoked just before the screen structures are going to be (re)allocated.
 */
    خالی
win_new_shellsize(خالی)
{
    ساکن صحیح	old_صفیں_2 = 0;
    ساکن صحیح	old_Columns = 0;
    ساکن صحیح	old_coloff = 0;

    اگر (old_صفیں_2 != صفیں || old_Columns != COLUMNS_WITHOUT_TPL()
	    || old_coloff != TPL_LCOL())
	ui_new_shellsize();
    اگر (old_Columns != COLUMNS_WITHOUT_TPL() || old_coloff != TPL_LCOL())
    {
	old_Columns = COLUMNS_WITHOUT_TPL();
	old_coloff = TPL_LCOL();

	shell_new_columns();
    }
    اگر (old_صفیں_2 != صفیں)
    {
	// If 'window' uses the whole screen, keep it using that.
	// Don't change it when set with "-w size" on the command line.
	اگر (p_کھڑکی == old_صفیں_2 - 1
		    || (old_صفیں_2 == 0 && !option_was_سیٹ((char_u *)"window")))
	    p_کھڑکی = صفیں - 1;
	old_صفیں_2 = صفیں;
	shell_new_صفیں();	// update window sizes
    }
}

#اگر defined(FEAT_IMAGE) || defined(FEAT_EVAL)
    خالی
update_cell_سائز(خالی)
{
# ifdef FEAT_GUI
    اگر (gui.اندر_use)
    {
	cell_چوڑائی = gui.char_چوڑائی;
	cell_اونچائی = gui.char_اونچائی;
    }
    ورنہ
# endif
    {
	ساخت cellsize cell_sz;

	mch_calc_cell_سائز(&cell_sz);
	اگر (cell_sz.cs_xpixel <= 0 || cell_sz.cs_ypixel <= 0)
	{
	    cell_چوڑائی = 8;
	    cell_اونچائی = 16;
	}
	ورنہ
	{
	    cell_چوڑائی = cell_sz.cs_xpixel;
	    cell_اونچائی = cell_sz.cs_ypixel;
	}
    }
}
#اگر_ختم

/*
 * Call this function when the Vim shell has been resized in any way.
 * Will obtain the current size and redraw (also when size didn't change).
 */
    خالی
shell_resized(خالی)
{
#اگر defined(FEAT_IMAGE) || defined(FEAT_EVAL)
    update_cell_سائز();
#اگر_ختم
    سیٹ_shellsize(0, 0, FALSE);
}

/*
 * Check if the shell size changed.  Handle a resize.
 * When the size didn't change, nothing happens.
 */
    خالی
shell_resized_check(خالی)
{
    صحیح		old_صفیں_2 = صفیں;
    صحیح		old_Columns = Columns;

    اگر (exiting
#اگر_تعریف_شدہ FEAT_GUI
	    // Do not get the size when executing a shell command during
	    // startup.
	    || gui.starting
#اگر_ختم
	    )
	واپس;

    (خالی)ui_get_shellsize();
    check_shellsize();
    اگر (old_صفیں_2 != صفیں || old_Columns != Columns)
	shell_resized();
}

/*
 * Set size of the Vim shell.
 * If 'mustset' is TRUE, we must set Rows and Columns, do not get the real
 * window size (this is used for the :win command).
 * If 'mustset' is FALSE, we may try to get the real window size and if
 * it fails use 'width' and 'height'.
 */
    ساکن خالی
سیٹ_shellsize_inner(صحیح چوڑائی, صحیح اونچائی, صحیح mustset)
{
    اگر (updating_screen)
	// resizing while in update_screen() may cause a crash
	واپس;

    // curwin->w_buffer can be NULL when we are closing a window and the
    // buffer (or window) has already been closed and removing a scrollbar
    // causes a resize event. Don't resize then, it will happen after entering
    // another buffer.
    اگر (curwin->w_بفر == تہی || curwin->w_سطریں == تہی)
	واپس;

#اگر_تعریف_شدہ AMIGA
    برآمد_flush();	    // must do this before mch_get_shellsize() for
			    // some obscure reason
#اگر_ختم

    اگر (mustset || (ui_get_shellsize() == FAIL && اونچائی != 0))
    {
	صفیں = اونچائی;
	Columns = چوڑائی;
	check_shellsize();
	ui_سیٹ_shellsize(mustset);
    }
    ورنہ
	check_shellsize();

    // The window layout used to be adjusted here, but it now happens in
    // screenalloc() (also invoked from screenclear()).  That is because the
    // "busy" check above may skip this, but not screenalloc().

    اگر (State != منوال_ASKMORE && State != منوال_EXTERNCMD
						      && State != منوال_CONFIRM)
	screenclear();
    ورنہ
	screen_آغاز();	    // don't know where cursor is now

    اگر (starting != NO_SCREEN)
    {
	maketitle();

	بدلا_سطر_abv_curs();
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
	اگر (State == منوال_ASKMORE || State == منوال_EXTERNCMD
				     || State == منوال_CONFIRM || exmode_فعال)
	{
	    screenalloc(FALSE);
	    repeat_پیغام();
	}
	ورنہ
	{
	    اگر (curwin->w_p_scb)
		do_check_scrollbind(TRUE);
	    اگر (State & منوال_CMDLINE)
	    {
		update_screen(UPD_NOT_VALID);
		redrawcmdline();
	    }
	    ورنہ
	    {
		update_topline();
		اگر (pum_visible())
		{
		    redraw_later(UPD_NOT_VALID);
		    ins_compl_show_pum();
		}
		update_screen(UPD_NOT_VALID);
		اگر (redrawing())
		    setcursor();
	    }
	}
	cursor_on();	    // redrawing may have switched it off
    }
    برآمد_flush();
}

    خالی
سیٹ_shellsize(صحیح چوڑائی, صحیح اونچائی, صحیح mustset)
{
    ساکن صحیح	busy = FALSE;
    ساکن صحیح	do_تسلسل = FALSE;

    اگر (چوڑائی < 0 || اونچائی < 0)    // just checking...
	واپس;

    اگر (State == منوال_HITRETURN || State == منوال_SETWSIZE)
    {
	// postpone the resizing
	State = منوال_SETWSIZE;
	واپس;
    }

    // Avoid recursiveness.  This can happen when setting the window size
    // causes another window-changed signal or when two SIGWINCH signals come
    // very close together.  There needs to be another run then after the
    // current one is done to pick up the latest size.
    do_تسلسل = TRUE;
    اگر (busy)
	واپس;

    جب (do_تسلسل)
    {
	do_تسلسل = FALSE;
	busy = TRUE;
	سیٹ_shellsize_inner(چوڑائی, اونچائی, mustset);
	busy = FALSE;
    }
}

/*
 * Output T_CTE, the t_TE termcap entry, and handle expected effects.
 * The code possibly disables modifyOtherKeys and the Kitty keyboard protocol.
 */
    خالی
برآمد_str_t_TE(خالی)
{
    برآمد_str(T_CTE);

    // The seenModifyOtherKeys flag is not reset here.  We do expect t_TE to
    // disable modifyOtherKeys, but until Xterm version 377 there is no way to
    // detect it's enabled again after the following t_TI.  We assume that when
    // seenModifyOtherKeys was set before it will still be valid.

    // When the modifyOtherKeys level is detected to be 2 we expect t_TE to
    // disable it.  Remembering that it was detected to be enabled is useful in
    // some situations.
    // The following t_TI is expected to request the state and then
    // modify_otherkeys_state will be set again.
    اگر (modify_otherkeys_state == MOKS_ENABLED
	    || modify_otherkeys_state == MOKS_DISABLED)
	modify_otherkeys_state = MOKS_DISABLED;
    ورنہ اگر (modify_otherkeys_state != MOKS_INITIAL)
	modify_otherkeys_state = MOKS_AFTER_T_TE;

    // When the kitty keyboard protocol is enabled we expect t_TE to disable
    // it.  Remembering that it was detected to be enabled is useful in some
    // situations.
    // The following t_TI is expected to request the state and then
    // kitty_protocol_state will be set again.
    اگر (kitty_protocol_state == KKPS_ENABLED
	    || kitty_protocol_state == KKPS_DISABLED)
	kitty_protocol_state = KKPS_DISABLED;
    ورنہ
	kitty_protocol_state = KKPS_AFTER_T_TE;
}

ساکن صحیح send_t_RK = FALSE;

/*
 * Output T_TI and setup for what follows.
 */
    خالی
برآمد_str_t_TI(خالی)
{
    برآمد_str(T_CTI);

    // Send t_RK when there is no more work to do.
    send_t_RK = TRUE;
}

/*
 * Output T_BE, but only when t_PS and t_PE are set.
 */
    خالی
برآمد_str_t_BE(خالی)
{
    char_u *p;

    اگر (T_BE == تہی || *T_BE == NUL
	    || (p = ڈھونڈو_termcode((char_u *)"PS")) == تہی || *p == NUL
	    || (p = ڈھونڈو_termcode((char_u *)"PE")) == تہی || *p == NUL)
	واپس;
    برآمد_str(T_BE);
}

/*
 * If t_TI was recently sent and there is no typeahead or work to do, now send
 * t_RK.  This is postponed to avoid the response arriving in a shell command
 * or after Vim exits.
 */
    خالی
may_send_t_RK(خالی)
{
    اگر (send_t_RK
	    && !work_pending()
	    && !ex_معمول_busy
#اگر_تعریف_شدہ FEAT_EVAL
	    && !اندر_feedkeys
#اگر_ختم
	    && !exiting)
    {
	send_t_RK = FALSE;
	برآمد_str(T_CRK);
	برآمد_flush();
    }
}

/*
 * Set the terminal to TMODE_RAW (for Normal mode) or TMODE_COOK (for external
 * commands and Ex mode).
 */
    خالی
settmode(tmode_T tmode)
{
#اگر_تعریف_شدہ FEAT_GUI
    // don't set the term where gvim was started to any mode
    اگر (gui.اندر_use)
	واپس;
#اگر_ختم

    اگر (!بھرا_screen)
	واپس;

    /*
     * When returning after calling a shell cur_tmode is TMODE_UNKNOWN,
     * set the terminal to raw mode, even though we think it already is,
     * because the shell program may have reset the terminal mode.
     * When we think the terminal is normal, don't try to set it to
     * normal again, because that causes problems (logout!) on some
     * machines.
     */
    اگر (tmode != cur_tmode)
    {
#اگر_تعریف_شدہ FEAT_TERMRESPONSE
# ifdef FEAT_GUI
	اگر (!gui.اندر_use && !gui.starting)
# endif
	{
	    // May need to check for T_CRV response and termcodes, it
	    // doesn't work in Cooked mode, an external program may get
	    // them.
	    اگر (tmode != TMODE_RAW && termrequest_any_pending())
		(خالی)vpeekc_nomap();
	    check_for_codes_from_رکن();
	}
#اگر_ختم
	اگر (tmode != TMODE_RAW)
	    mch_setmouse(FALSE);	// switch mouse off

	// Disable bracketed paste and modifyOtherKeys in cooked mode.
	// Avoid doing this too often, on some terminals the codes are not
	// handled properly.
	اگر (termcap_فعال && tmode != TMODE_SLEEP
		&& cur_tmode != TMODE_SLEEP)
	{
	    MAY_WANT_مقصد_LOG_THIS;

	    اگر (tmode != TMODE_RAW)
	    {
		برآمد_str(T_BD);	// disable bracketed paste mode
		برآمد_str_t_TE();	// possibly disables modifyOtherKeys
	    }
	    ورنہ
	    {
		برآمد_str_t_BE();	// enable bracketed paste mode (should
				// be before mch_settmode().
		برآمد_str_t_TI();	// possibly enables modifyOtherKeys
	    }
	}
	برآمد_flush();
	mch_settmode(tmode);	// machine specific function
	cur_tmode = tmode;
	اگر (tmode == TMODE_RAW)
	    setmouse();		// may switch mouse on
	برآمد_flush();
    }
#اگر_تعریف_شدہ FEAT_TERMRESPONSE
    may_req_termresponse();
#اگر_ختم
}

    خالی
starttermcap(خالی)
{
    اگر (!بھرا_screen || termcap_فعال)
	واپس;

    MAY_WANT_مقصد_LOG_THIS;

    برآمد_str(T_TI);			// start termcap mode
    برآمد_str_t_TI();			// start "raw" mode
    برآمد_str(T_KS);			// start "keypad transmit" mode
    برآمد_str_t_BE();			// enable bracketed paste mode

#اگر defined(UNIX) || defined(VMS)
    // Enable xterm's focus reporting mode when 'esckeys' is set.
    اگر (p_ek && *T_FE != NUL)
	برآمد_str(T_FE);
#اگر_ختم

    برآمد_flush();
    termcap_فعال = TRUE;
    screen_آغاز();			// don't know where cursor is now
#اگر_تعریف_شدہ FEAT_TERMRESPONSE
# ifdef FEAT_GUI
    اگر (!gui.اندر_use && !gui.starting)
# endif
    {
	may_req_termresponse();
	// Immediately check for a response.  If t_Co changes, we don't
	// want to redraw with wrong colors first.
	اگر (crv_حالت.tr_پیش_رفت == حالت_بھیجا)
	    check_for_codes_from_رکن();
    }
#اگر_ختم
}

    خالی
stoptermcap(خالی)
{
    screen_stop_highlight();
    reset_cterm_colors();

    اگر (!termcap_فعال)
	واپس;

#اگر_تعریف_شدہ FEAT_TERMRESPONSE
# ifdef FEAT_GUI
    اگر (!gui.اندر_use && !gui.starting)
# endif
    {
	// May need to discard T_CRV, T_U7 or T_RBG response.
	اگر (termrequest_any_pending())
	{
# ifdef UNIX
	    // Give the terminal a chance to respond.
	    mch_delay(100L, 0);
# endif
# ifdef TCIFLUSH
	    // Discard data received but not read.
	    اگر (exiting)
		tcflush(fileno(stdin), TCIFLUSH);
# endif
	}
	// Check for termcodes first, otherwise an external program may
	// get them.
	check_for_codes_from_رکن();
    }
#اگر_ختم
    MAY_WANT_مقصد_LOG_THIS;

#اگر defined(UNIX) || defined(VMS)
    // Disable xterm's focus reporting mode if 'esckeys' is set.
    اگر (p_ek && *T_FD != NUL)
	برآمد_str(T_FD);
#اگر_ختم

    برآمد_str(T_BD);			// disable bracketed paste mode
    برآمد_str(T_KE);			// stop "keypad transmit" mode
    برآمد_flush();
    termcap_فعال = FALSE;

    // Output t_te before t_TE, t_te may switch between main and alternate
    // screen and following codes may work on the active screen only.
    //
    // When using the Kitty keyboard protocol the main and alternate screen
    // use a separate state.  If we are (or were) using the Kitty keyboard
    // protocol and t_te is not empty (possibly switching screens) then
    // output t_TE both before and after outputting t_te.
    اگر (*T_TE != NUL && (kitty_protocol_state == KKPS_ENABLED
		|| kitty_protocol_state == KKPS_DISABLED))
	برآمد_str_t_TE();		// probably disables the kitty keyboard
				// protocol

    برآمد_str(T_TE);			// stop termcap mode
    cursor_on();			// just in case it is still off
    برآمد_str_t_TE();			// stop "raw" mode, modifyOtherKeys and
					// Kitty keyboard protocol
    screen_آغاز();			// don't know where cursor is now
    برآمد_flush();
}

#اگر defined(FEAT_TERMRESPONSE)
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
    خالی
may_req_termresponse(خالی)
{
    اگر (crv_حالت.tr_پیش_رفت == حالت_GET
	    && can_get_termresponse()
	    && starting == 0
	    && *T_CRV != NUL)
    {
	MAY_WANT_مقصد_LOG_THIS;
	LOG_TR1("Sending CRV request");
	برآمد_str(T_CRV);
	termrequest_بھیجا(&crv_حالت);
	// check for the characters now, otherwise they might be eaten by
	// get_keystroke()
	برآمد_flush();
	(خالی)vpeekc_nomap();
    }
}

/*
 * Send sequences to the terminal and check with t_u7 how the cursor moves, to
 * find out properties of the terminal.
 * Note that this goes out before T_CRV, so that the result can be used when
 * the termresponse arrives.
 */
    خالی
check_terminal_behavior(خالی)
{
    صحیح	    did_send = FALSE;

    اگر (!can_get_termresponse() || starting != 0 || *T_U7 == NUL)
	واپس;

    اگر (u7_حالت.tr_پیش_رفت == حالت_GET
	    && !option_was_سیٹ((char_u *)"ambiwidth"))
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
	MAY_WANT_مقصد_LOG_THIS;
	LOG_TR1("Sending request for ambiwidth check");
	// Do this in the second row.  In the first row the returned sequence
	// may be CSI 1;2R, which is the same as <S-F3>.
	رکن_windgoto(1, 0);
	buf[mb_char2bytes(0x25bd, buf)] = NUL;
	برآمد_str(buf);
	برآمد_str(T_U7);
	termrequest_بھیجا(&u7_حالت);
	برآمد_flush();
	did_send = TRUE;

	// This overwrites a few characters on the screen, a redraw is needed
	// after this. Clear them out for now.
	screen_stop_highlight();
	رکن_windgoto(1, 0);
	برآمد_str((char_u *)"  ");
	سطر_was_clobbered(1);
    }

    اگر (xcc_حالت.tr_پیش_رفت == حالت_GET && صفیں > 2)
    {
	// 2. Check compatibility with xterm.
	// We move the cursor to (2, 0), print a test sequence and then query
	// the current cursor position.  If the terminal properly handles
	// unknown DCS string and CSI sequence with intermediate byte, the test
	// sequence is ignored and the cursor does not move.  If the terminal
	// handles test sequence incorrectly, a garbage string is displayed and
	// the cursor does move.
	MAY_WANT_مقصد_LOG_THIS;
	LOG_TR1("Sending xterm compatibility test sequence.");
	// Do this in the third row.  Second row is used by ambiguous
	// character width check.
	رکن_windgoto(2, 0);
	// send the test DCS string.
	برآمد_str((char_u *)"\033Pzz\033\\");
	// send the test CSI sequence with intermediate byte.
	برآمد_str((char_u *)"\033[0%m");
	برآمد_str(T_U7);
	termrequest_بھیجا(&xcc_حالت);
	برآمد_flush();
	did_send = TRUE;

	// If the terminal handles test sequence incorrectly, garbage text is
	// displayed. Clear them out for now.
	screen_stop_highlight();
	رکن_windgoto(2, 0);
	برآمد_str((char_u *)"           ");
	سطر_was_clobbered(2);
    }

    اگر (did_send)
    {
	رکن_windgoto(0, 0);

	// Need to reset the known cursor position.
	screen_آغاز();

	// check for the characters now, otherwise they might be eaten by
	// get_keystroke()
	برآمد_flush();
	(خالی)vpeekc_nomap();
    }
}

/*
 * Similar to requesting the version string: Request the terminal background
 * color when it is the right moment.
 */
    خالی
may_req_bg_color(خالی)
{
    اگر (can_get_termresponse() && starting == 0)
    {
	صحیح didit = FALSE;

# ifdef FEAT_TERMINAL
	// Only request foreground if t_RF is set.
	اگر (rfg_حالت.tr_پیش_رفت == حالت_GET && *T_RFG != NUL)
	{
	    MAY_WANT_مقصد_LOG_THIS;
	    LOG_TR1("Sending FG request");
	    برآمد_str(T_RFG);
	    termrequest_بھیجا(&rfg_حالت);
	    didit = TRUE;
	}
# endif

	// Only request background if t_RB is set.
	اگر (rbg_حالت.tr_پیش_رفت == حالت_GET && *T_RBG != NUL)
	{
	    MAY_WANT_مقصد_LOG_THIS;
	    LOG_TR1("Sending BG request");
	    برآمد_str(T_RBG);
	    termrequest_بھیجا(&rbg_حالت);
	    didit = TRUE;
	}

	اگر (didit)
	{
	    // check for the characters now, otherwise they might be eaten by
	    // get_keystroke()
	    برآمد_flush();
	    (خالی)vpeekc_nomap();
	}
    }
}

#اگر_ختم

/*
 * Return TRUE when saving and restoring the screen.
 */
    صحیح
swapping_screen(خالی)
{
    واپس (بھرا_screen && *T_TI != NUL);
}

/*
 * By outputting the 'cursor very visible' termcap code, for some windowed
 * terminals this makes the screen scrolled to the correct position.
 * Used when starting Vim or returning from a shell.
 */
    خالی
scroll_آغاز(خالی)
{
    اگر (*T_VS == NUL || *T_CVS == NUL)
	واپس;

    MAY_WANT_مقصد_LOG_THIS;
    برآمد_str(T_VS);
    برآمد_str(T_CVS);
    screen_آغاز();		// don't know where cursor is now
}

// True if cursor is not visible
ساکن صحیح cursor_is_off = FALSE;

// True if cursor is not visible due to an ongoing cursor-less sleep
ساکن صحیح cursor_is_asleep = FALSE;

/*
 * Enable the cursor without checking if it's already enabled.
 */
    خالی
cursor_on_force(خالی)
{
    برآمد_str(T_VE);
    cursor_is_off = FALSE;
    cursor_is_asleep = FALSE;
}

/*
 * Enable the cursor if it's currently off.
 */
    خالی
cursor_on(خالی)
{
    اگر (cursor_is_off && !cursor_is_asleep)
	cursor_on_force();
}

/*
 * Disable the cursor.
 */
    خالی
cursor_off(خالی)
{
    اگر (بھرا_screen && !cursor_is_off)
    {
	برآمد_str(T_VI);	    // disable cursor
	cursor_is_off = TRUE;
    }
}

#اگر_تعریف_شدہ FEAT_GUI
/*
 * Check whether the cursor is invisible due to an ongoing cursor-less sleep
 */
    صحیح
cursor_is_sleeping(خالی)
{
    واپس cursor_is_asleep;
}
#اگر_ختم

/*
 * Disable the cursor and mark it disabled by cursor-less sleep
 */
    خالی
cursor_sleep(خالی)
{
    cursor_is_asleep = TRUE;
    cursor_off();
}

/*
 * Enable the cursor and mark it not disabled by cursor-less sleep
 */
    خالی
cursor_unsleep(خالی)
{
    cursor_is_asleep = FALSE;
    cursor_on();
}

#اگر defined(CURSOR_SHAPE)
/*
 * Set cursor shape to match Insert or Replace mode.
 */
    خالی
رکن_cursor_منوال(صحیح forced)
{
    ساکن صحیح showing_منوال = -1;
    char_u *p;

    // Only do something when redrawing the screen and we can restore the
    // mode.
    اگر (!بھرا_screen || *T_CEI == NUL)
    {
# ifdef FEAT_TERMRESPONSE
	اگر (forced && initial_cursor_shape > 0)
	    // Restore to initial values.
	    رکن_cursor_shape(initial_cursor_shape, initial_cursor_blink);
# endif
	واپس;
    }

    اگر ((State & منوال_REPLACE) == منوال_REPLACE)
    {
	اگر (forced || showing_منوال != منوال_REPLACE)
	{
	    اگر (*T_CSR != NUL)
		p = T_CSR;	// Replace mode cursor
	    ورنہ
		p = T_CSI;	// fall back to Insert mode cursor
	    اگر (*p != NUL)
	    {
		برآمد_str(p);
		showing_منوال = منوال_REPLACE;
	    }
	}
    }
    ورنہ اگر (State & منوال_داخل_کرو)
    {
	اگر ((forced || showing_منوال != منوال_داخل_کرو) && *T_CSI != NUL)
	{
	    برآمد_str(T_CSI);	    // Insert mode cursor
	    showing_منوال = منوال_داخل_کرو;
	}
    }
    ورنہ اگر (forced || showing_منوال != منوال_معمول)
    {
	برآمد_str(T_CEI);		    // non-Insert mode cursor
	showing_منوال = منوال_معمول;
    }
}

# اگر defined(FEAT_TERMINAL)
    خالی
رکن_cursor_color(char_u *color)
{
    اگر (*T_CSC == NUL)
	واپس;

    برآمد_str(T_CSC);		// set cursor color start
    برآمد_str_nf(color);
    برآمد_str(T_CEC);		// set cursor color end
    برآمد_flush();
}
# endif

    صحیح
blink_state_is_inverted(خالی)
{
# ifdef FEAT_TERMRESPONSE
    واپس rbm_حالت.tr_پیش_رفت == حالت_GOT
	&& rcs_حالت.tr_پیش_رفت == حالت_GOT
		&& initial_cursor_blink != initial_cursor_shape_blink;
# ورنہ
    واپس FALSE;
# endif
}

/*
 * "shape": 1 = block, 2 = underline, 3 = vertical bar
 */
    خالی
رکن_cursor_shape(صحیح shape, صحیح blink)
{
    اگر (*T_CSH != NUL)
    {
	برآمد_STR(tgoto((حرف *)T_CSH, 0, shape * 2 - blink));
	برآمد_flush();
    }
    ورنہ
    {
	صحیح do_blink = blink;

	// t_SH is empty: try setting just the blink state.
	// The blink flags are XORed together, if the initial blinking from
	// style and shape differs, we need to invert the flag here.
	اگر (blink_state_is_inverted())
	    do_blink = !blink;

	اگر (do_blink && *T_VS != NUL)
	{
	    برآمد_str(T_VS);
	    برآمد_flush();
	}
	ورنہ اگر (!do_blink && *T_CVS != NUL)
	{
	    برآمد_str(T_CVS);
	    برآمد_flush();
	}
    }
}
#اگر_ختم

/*
 * Set scrolling region for window 'wp'.
 * The region starts 'off' lines from the start of the window.
 * Also set the vertical scroll region for a vertically split window.  Always
 * the full width of the window, excluding the vertical separator.
 */
    خالی
scroll_region_سیٹ(win_T *wp, صحیح off)
{
    برآمد_STR(tgoto((حرف *)T_CS, W_WINROW(wp) + wp->w_اونچائی - 1,
							 W_WINROW(wp) + off));
    اگر (*T_CSV != NUL && wp->w_چوڑائی != Columns)
	برآمد_STR(tgoto((حرف *)T_CSV, wp->w_wincol + wp->w_چوڑائی - 1,
							       wp->w_wincol));
    screen_آغاز();		    // don't know where cursor is now
}

/*
 * Reset scrolling region to the whole screen.
 */
    خالی
scroll_region_reset(خالی)
{
    برآمد_STR(tgoto((حرف *)T_CS, (صحیح)صفیں - 1, 0));
    اگر (*T_CSV != NUL)
	برآمد_STR(tgoto((حرف *)T_CSV, (صحیح)Columns - 1, 0));
    screen_آغاز();		    // don't know where cursor is now
}


/*
 * List of terminal codes that are currently recognized.
 */

ساکن ساخت termcode
{
    char_u  name[2];	    // termcap name of entry
    char_u  *رمز;	    // terminal code (in allocated memory)
    صحیح	    len;	    // STRLEN(code)
    صحیح	    modlen;	    // length of part before ";*~".
} *termcodes = تہی;

ساکن صحیح  tc_max_len = 0; // number of entries that termcodes[] can hold
ساکن صحیح  tc_len = 0;	    // current number of entries in termcodes[]

ساکن صحیح termcode_ستارہ(char_u *رمز, صحیح len);

    خالی
clear_termcodes(خالی)
{
    جب (tc_len > 0)
	vim_free(termcodes[--tc_len].رمز);
    VIM_CLEAR(termcodes);
    tc_max_len = 0;

#اگر_تعریف_شدہ HAVE_TGETENT
    BC = (حرف *)خالی_خانے_option;
    اوپر_2 = (حرف *)خالی_خانے_option;
    PC = NUL;			// set pad character to NUL
    ospeed = 0;
#اگر_ختم

    need_gather = TRUE;		// need to fill termleader[]
}

#تعریف_کرو ATC_FROM_رکن 55

/*
 * For xterm we recognize special codes like "ESC[42;*X" and "ESC O*X" that
 * accept modifiers.
 * Set "termcodes[idx].modlen".
 */
    ساکن خالی
adjust_modlen(صحیح idx)
{
    termcodes[idx].modlen = 0;
    صحیح j = termcode_ستارہ(termcodes[idx].رمز, termcodes[idx].len);
    اگر (j <= 0)
	واپس;

    termcodes[idx].modlen = termcodes[idx].len - 1 - j;
    // For "CSI[@;X" the "@" is not included in "modlen".
    اگر (termcodes[idx].رمز[termcodes[idx].modlen - 1] == '@')
	--termcodes[idx].modlen;
}

/*
 * Add a new entry for "name[2]" to the list of terminal codes.
 * Note that "name" may not have a terminating NUL.
 * The list is kept alphabetical for ":set termcap"
 * "flags" is TRUE when replacing 7-bit by 8-bit controls is desired.
 * "flags" can also be ATC_FROM_TERM for got_code_from_term().
 */
    خالی
جوڑو_termcode(char_u *name, char_u *string, صحیح flags)
{
    ساخت termcode *new_tc;
    صحیح		    i, j;
    char_u	    *s;
    صحیح		    len;
#اگر_تعریف_شدہ FEAT_EVAL
    حرف	    *action = "Setting";
#اگر_ختم

    اگر (string == تہی || *string == NUL)
    {
	del_termcode(name);
	واپس;
    }

#اگر defined(MSWIN) && !defined(FEAT_GUI)
    s = vim_strnsave(string, STRLEN(string) + 1);
#ورنہ
# ifdef VIMDLL
    اگر (!gui.اندر_use)
	s = vim_strnsave(string, STRLEN(string) + 1);
    ورنہ
# endif
	s = vim_strsave(string);
#اگر_ختم
    اگر (s == تہی)
	واپس;

    // Change leading <Esc>[ to CSI, change <Esc>O to <M-O>.
    اگر (flags != 0 && flags != ATC_FROM_رکن && رکن_7to8bit(string) != 0)
    {
	STRMOVE(s, s + 1);
	s[0] = رکن_7to8bit(string);
    }

#اگر defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
# ifdef VIMDLL
    اگر (!gui.اندر_use)
# endif
    {
	اگر (s[0] == K_NUL)
	{
	    STRMOVE(s + 1, s);
	    s[1] = 3;
	}
    }
#اگر_ختم

    len = (صحیح)STRLEN(s);

    need_gather = TRUE;		// need to fill termleader[]

    /*
     * need to make space for more entries
     */
    اگر (tc_len == tc_max_len)
    {
	tc_max_len += 20;
	new_tc = ALLOC_MULT(ساخت termcode, tc_max_len);
	اگر (new_tc == تہی)
	{
	    tc_max_len -= 20;
	    vim_free(s);
	    واپس;
	}
	برائے (i = 0; i < tc_len; ++i)
	    new_tc[i] = termcodes[i];
	vim_free(termcodes);
	termcodes = new_tc;
    }

    /*
     * Look for existing entry with the same name, it is replaced.
     * Look for an existing entry that is alphabetical higher, the new entry
     * is inserted in front of it.
     */
    برائے (i = 0; i < tc_len; ++i)
    {
	اگر (termcodes[i].name[0] < name[0])
	    جاری;
	اگر (termcodes[i].name[0] == name[0])
	{
	    اگر (termcodes[i].name[1] < name[1])
		جاری;
	    /*
	     * Exact match: May replace old code.
	     */
	    اگر (termcodes[i].name[1] == name[1])
	    {
		اگر (flags == ATC_FROM_رکن && (j = termcode_ستارہ(
				    termcodes[i].رمز, termcodes[i].len)) > 0)
		{
		    // Don't replace ESC[123;*X or ESC O*X with another when
		    // invoked from got_code_from_term().
		    اگر (len == termcodes[i].len - j
			    && STRNCMP(s, termcodes[i].رمز, len - 1) == 0
			    && s[len - 1]
				   == termcodes[i].رمز[termcodes[i].len - 1])
		    {
			// They are equal but for the ";*": don't add it.
#اگر_تعریف_شدہ FEAT_EVAL
			ch_log(تہی, "Termcap entry %c%c did not change",
							     name[0], name[1]);
#اگر_ختم
			vim_free(s);
			واپس;
		    }
		}
		ورنہ
		{
		    // Replace old code.
#اگر_تعریف_شدہ FEAT_EVAL
		    ch_log(تہی, "Termcap entry %c%c was: %s",
					  name[0], name[1], termcodes[i].رمز);
#اگر_ختم
		    vim_free(termcodes[i].رمز);
		    --tc_len;
		    توڑو;
		}
	    }
	}
	/*
	 * Found alphabetical larger entry, move rest to insert new entry
	 */
#اگر_تعریف_شدہ FEAT_EVAL
	action = "Adding";
#اگر_ختم
	برائے (j = tc_len; j > i; --j)
	    termcodes[j] = termcodes[j - 1];
	توڑو;
    }

#اگر_تعریف_شدہ FEAT_EVAL
    ch_log(تہی, "%s termcap entry %c%c to %s", action, name[0], name[1], s);
#اگر_ختم
    termcodes[i].name[0] = name[0];
    termcodes[i].name[1] = name[1];
    termcodes[i].رمز = s;
    termcodes[i].len = len;
    adjust_modlen(i);

    ++tc_len;
}

/*
 * Some function keys may include modifiers, but the terminfo/termcap entries
 * do not indicate that.  Insert ";*" where we expect modifiers might appear.
 */
    ساکن خالی
accept_modifiers_for_function_کلیدیں(خالی)
{
    regmatch_T regmatch;
    CLEAR_میدان(regmatch);
    regmatch.rm_ic = TRUE;
    regmatch.regprog = vim_regcomp((char_u *)"^\033[\\d\\+\\~$", RE_MAGIC);

    برائے (صحیح i = 0; i < tc_len; ++i)
    {
	اگر (regmatch.regprog == تہی)
	    واپس;

	// skip PasteStart and PasteEnd
	اگر (termcodes[i].name[0] == 'P'
		&& (termcodes[i].name[1] == 'S' || termcodes[i].name[1] == 'E'))
	    جاری;

	char_u *s = termcodes[i].رمز;
	اگر (s != تہی && vim_regexec(&regmatch, s, (colnr_T)0))
	{
	    size_t len = STRLEN(s);
	    char_u *ns = alloc(len + 3);
	    اگر (ns != تہی)
	    {
		mch_memmove(ns, s, len - 1);
		mch_memmove(ns + len - 1, ";*~", 4);
		vim_free(s);
		termcodes[i].رمز = ns;
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
    ساکن صحیح
termcode_ستارہ(char_u *رمز, صحیح len)
{
    // Shortest is <M-O>*X.  With ; shortest is <CSI>@;*X
    اگر (len >= 3 && رمز[len - 2] == '*')
    {
	اگر (len >= 5 && رمز[len - 3] == ';')
	    واپس 2;
	ورنہ
	    واپس 1;
    }
    واپس 0;
}

    char_u  *
ڈھونڈو_termcode(char_u *name)
{
    صحیح	    i;

    برائے (i = 0; i < tc_len; ++i)
	اگر (termcodes[i].name[0] == name[0] && termcodes[i].name[1] == name[1])
	    واپس termcodes[i].رمز;
    واپس تہی;
}

    char_u *
get_termcode(صحیح i)
{
    اگر (i >= tc_len)
	واپس تہی;
    واپس &termcodes[i].name[0];
}

/*
 * Returns the length of the terminal code at index 'idx'.
 */
    صحیح
get_termcode_len(صحیح idx)
{
    واپس termcodes[idx].len;
}

    خالی
del_termcode(char_u *name)
{
    صحیح	    i;

    اگر (termcodes == تہی)	// nothing there yet
	واپس;

    need_gather = TRUE;		// need to fill termleader[]

    برائے (i = 0; i < tc_len; ++i)
	اگر (termcodes[i].name[0] == name[0] && termcodes[i].name[1] == name[1])
	{
	    del_termcode_idx(i);
	    واپس;
	}
    // not found. Give error message?
}

    ساکن خالی
del_termcode_idx(صحیح idx)
{
    صحیح		i;

    vim_free(termcodes[idx].رمز);
    --tc_len;
    برائے (i = idx; i < tc_len; ++i)
	termcodes[i] = termcodes[i + 1];
}

/*
 * Called when detected that the terminal sends 8-bit codes.
 * Convert all 7-bit codes to their 8-bit equivalent.
 */
    ساکن خالی
switch_مقصد_8bit(خالی)
{
    صحیح		i;
    صحیح		c;

    // Only need to do something when not already using 8-bit codes.
    اگر (!رکن_is_8bit(T_NAME))
    {
	برائے (i = 0; i < tc_len; ++i)
	{
	    c = رکن_7to8bit(termcodes[i].رمز);
	    اگر (c != 0)
	    {
		STRMOVE(termcodes[i].رمز + 1, termcodes[i].رمز + 2);
		termcodes[i].رمز[0] = c;
	    }
	}
	need_gather = TRUE;		// need to fill termleader[]
    }
    detected_8bit = TRUE;
    LOG_TR1("Switching to 8 bit");
}

#اگر_تعریف_شدہ CHECK_DOUBLE_CLICK
ساکن linenr_T orig_topline = 0;
# ifdef FEAT_DIFF
ساکن صحیح orig_topfill = 0;
# endif
#اگر_ختم
#اگر defined(CHECK_DOUBLE_CLICK)
/*
 * Checking for double-clicks ourselves.
 * "orig_topline" is used to avoid detecting a double-click when the window
 * contents scrolled (e.g., when 'scrolloff' is non-zero).
 */
/*
 * Set orig_topline.  Used when jumping to another window, so that a double
 * click still works.
 */
    خالی
سیٹ_mouse_topline(win_T *wp)
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
    صحیح
is_mouse_topline(win_T *wp)
{
    واپس orig_topline == wp->w_topline
# ifdef FEAT_DIFF
	&& orig_topfill == wp->w_topfill
# endif
	;
}
#اگر_ختم

/*
 * If "buf" is NULL put "string[new_slen]" in typebuf; "buflen" is not used.
 * If "buf" is not NULL put "string[new_slen]" in "buf[bufsize]" and adjust
 * "buflen".
 * Remove "slen" bytes.
 * Returns FAIL for error.
 */
    صحیح
put_string_اندر_typebuf(
	صحیح	انحراف,
	صحیح	slen,
	char_u	*string,
	صحیح	new_slen,
	char_u	*buf,
	صحیح	bufsize,
	صحیح	*buflen)
{
    صحیح		extra = new_slen - slen;

    string[new_slen] = NUL;
    اگر (buf == تہی)
    {
	اگر (extra < 0)
	    // remove matched chars, taking care of noremap
	    del_typebuf(-extra, انحراف);
	ورنہ اگر (extra > 0)
	    // insert the extra space we need
	    اگر (ins_typebuf(string + slen, REMAP_YES, انحراف, FALSE, FALSE)
								       == FAIL)
		واپس FAIL;

	// Careful: del_typebuf() and ins_typebuf() may have reallocated
	// typebuf.tb_buf[]!
	mch_memmove(typebuf.tb_buf + typebuf.tb_off + انحراف, string,
							     (size_t)new_slen);
    }
    ورنہ
    {
	اگر (extra < 0)
	    // remove matched characters
	    mch_memmove(buf + انحراف, buf + انحراف - extra,
					   (size_t)(*buflen + انحراف + extra));
	ورنہ اگر (extra > 0)
	{
	    // Insert the extra space we need.  If there is insufficient
	    // space return -1.
	    اگر (*buflen + extra + new_slen >= bufsize)
		واپس FAIL;
	    mch_memmove(buf + انحراف + extra, buf + انحراف,
						   (size_t)(*buflen - انحراف));
	}
	mch_memmove(buf + انحراف, string, (size_t)new_slen);
	*buflen = *buflen + extra + new_slen;
    }
    واپس OK;
}

/*
 * Decode a modifier number as xterm provides it into MOD_MASK bits.
 */
    صحیح
decode_modifiers(صحیح n)
{
    صحیح	    رمز = n - 1;
    صحیح	    modifiers = 0;

    اگر (رمز & 1)
	modifiers |= MOD_نقاب_سرکاؤ;
    اگر (رمز & 2)
	modifiers |= MOD_نقاب_ALT;
    اگر (رمز & 4)
	modifiers |= MOD_نقاب_CTRL;
    اگر (رمز & 8)
	modifiers |= MOD_نقاب_META;
    // Any further modifiers are silently dropped.

    واپس modifiers;
}

    ساکن صحیح
modifiers2keycode(صحیح modifiers, صحیح *کلید, char_u *string)
{
    صحیح new_slen = 0;

    اگر (modifiers == 0)
	واپس 0;

    // Some keys have the modifier included.  Need to handle that here to
    // make mappings work.  This may result in a special key, such as
    // K_S_TAB.
    *کلید = simplify_کلید(*کلید, &modifiers);
    اگر (modifiers != 0)
    {
	string[new_slen++] = K_SPECIAL;
	string[new_slen++] = (صحیح)KS_MODIFIER;
	string[new_slen++] = modifiers;
    }
    واپس new_slen;
}

/*
 * Handle a cursor position report.
 */
    ساکن خالی
handle_u7_response(صحیح *arg, char_u *tp غیر_مستعمل, صحیح csi_len غیر_مستعمل)
{
    اگر (arg[0] == 2 && arg[1] >= 2)
    {
	حرف *aw = تہی;

	LOG_TRN("Received U7 status: %s", tp);
	u7_حالت.tr_پیش_رفت = حالت_GOT;
	did_cursorhold = TRUE;
	اگر (arg[1] == 2)
	    aw = "single";
	ورنہ اگر (arg[1] == 3)
	    aw = "double";
	اگر (aw != تہی && STRCMP(aw, p_ambw) != 0)
	{
	    // Setting the option causes a screen redraw. Do
	    // that right away if possible, keeping any
	    // messages.
	    سیٹ_option_قدر_give_err((char_u *)"ambw", 0L, (char_u *)aw, 0);
#اگر_تعریف_شدہ DEBUG_TERMRESPONSE
	    {
		صحیح r = redraw_asap(UPD_CLEAR);

		LOG_TRN("set 'ambiwidth', redraw_asap(): %d", r);
	    }
#ورنہ
	    redraw_asap(UPD_CLEAR);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_EVAL
	    سیٹ_vim_var_string(VV_TERMU7RESP, tp, csi_len);
#اگر_ختم
	    apply_autocmds(EVENT_TERMRESPONSEALL,
					(char_u *)"ambiguouswidth", تہی, FALSE, curbuf);
	}
    }
    ورنہ اگر (arg[0] == 3)
    {
	صحیح قدر;

	LOG_TRN("Received compatibility test result: %s", tp);
	xcc_حالت.tr_پیش_رفت = حالت_GOT;

	// Third row: xterm compatibility test.
	// If the cursor is on the first column then the terminal can handle
	// the request for cursor style and blinking.
	قدر = arg[1] == 1 ? TPR_YES : TPR_NO;
	رکن_props[TPR_CURSOR_STYLE].tpr_حالت = قدر;
	رکن_props[TPR_CURSOR_BLINK].tpr_حالت = قدر;
    }
}



/*
 * Handle a response to T_CRV: {lead}{first}{x};{vers};{y}c
 * Xterm and alike use '>' for {first}.
 * Rxvt sends "{lead}?1;2c".
 */
    ساکن خالی
handle_نسخہ_response(صحیح پہلا, صحیح *arg, صحیح argc, char_u *tp)
{
    // The xterm version.  It is set to zero when it can't be an actual xterm
    // version.
    صحیح نسخہ_2 = arg[1];

    LOG_TRN("Received CRV response: %s", tp);
    crv_حالت.tr_پیش_رفت = حالت_GOT;
    did_cursorhold = TRUE;

    // Reset terminal properties that are set based on the termresponse.
    // Mainly useful for tests that send the termresponse multiple times.
    // For testing all props can be reset.
    init_رکن_props(
#اگر_تعریف_شدہ FEAT_EVAL
	    reset_رکن_props_on_termresponse
#ورنہ
	    FALSE
#اگر_ختم
	    );

    // If this code starts with CSI, you can bet that the
    // terminal uses 8-bit codes.
    اگر (tp[0] == CSI)
	switch_مقصد_8bit();

    // Screen sends 40500.
    // rxvt sends its version number: "20703" is 2.7.3.
    // Ignore it for when the user has set 'term' to xterm,
    // even though it's an rxvt.
    اگر (نسخہ_2 > 20000)
	نسخہ_2 = 0;

    // Figure out more if the response is CSI > 99 ; 99 ; 99 c
    اگر (پہلا == '>' && argc == 3)
    {
	// mintty 2.9.5 sends 77;20905;0c.
	// (77 is ASCII 'M' for mintty.)
	اگر (arg[0] == 77)
	{
	    // mintty can do SGR mouse reporting
	    رکن_props[TPR_MOUSE].tpr_حالت = TPR_MOUSE_SGR;
	}

#اگر_تعریف_شدہ FEAT_TERMRESPONSE
	// If xterm version >= 141 try to get termcap codes.  For other
	// terminals the request should be ignored.
	اگر (نسخہ_2 >= 141 && p_xtermcodes)
	{
	    LOG_TR1("Enable checking for XT codes");
	    check_for_codes = TRUE;
	    need_gather = TRUE;
	    req_codes_from_رکن();
	}
#اگر_ختم

	// libvterm sends 0;100;0
	// Konsole sends 0;115;0 and works the same way
	اگر ((نسخہ_2 == 100 || نسخہ_2 == 115) && arg[0] == 0 && arg[2] == 0)
	{
	    // If run from Vim $COLORS is set to the number of
	    // colors the terminal supports.  Otherwise assume
	    // 256, libvterm supports even more.
	    اگر (mch_getenv((char_u *)"COLORS") == تہی)
		may_adjust_color_گنتی(256);
	    // Libvterm can handle SGR mouse reporting.
	    رکن_props[TPR_MOUSE].tpr_حالت = TPR_MOUSE_SGR;
	}

	اگر (نسخہ_2 == 95)
	{
	    // Mac Terminal.app sends 1;95;0
	    //
	    // Terminal.app doesn't seem to handle DECRQM sequences
	    // properly, see issue #19852.
	    اگر (arg[0] == 1 && arg[2] == 0)
	    {
		رکن_props[TPR_UNDERLINE_RGB].tpr_حالت = TPR_YES;
		رکن_props[TPR_MOUSE].tpr_حالت = TPR_MOUSE_SGR;
		رکن_props[TPR_DECRQM].tpr_حالت = TPR_NO;
	    }
	    // iTerm2 sends 0;95;0
	    ورنہ اگر (arg[0] == 0 && arg[2] == 0)
	    {
		// iTerm2 can do SGR mouse reporting
		رکن_props[TPR_MOUSE].tpr_حالت = TPR_MOUSE_SGR;
		رکن_props[TPR_DECRQM].tpr_حالت = TPR_YES;
	    }
	    // old iTerm2 sends 0;95;
	    ورنہ اگر (arg[0] == 0 && arg[2] == -1)
		رکن_props[TPR_UNDERLINE_RGB].tpr_حالت = TPR_YES;
	}

	// screen sends 83;40500;0 83 is 'S' in ASCII.
	اگر (arg[0] == 83)
	{
	    // screen supports SGR mouse codes since 4.7.0
	    اگر (arg[1] >= 40700)
		رکن_props[TPR_MOUSE].tpr_حالت = TPR_MOUSE_SGR;
	    ورنہ
		رکن_props[TPR_MOUSE].tpr_حالت = TPR_MOUSE_XTERM;
	}

	// If no recognized terminal has set mouse behavior, assume xterm.
	اگر (رکن_props[TPR_MOUSE].tpr_حالت == TPR_UNKNOWN)
	{
	    // Xterm version 277 supports SGR.
	    // Xterm version >= 95 supports mouse dragging.
	    اگر (نسخہ_2 >= 277)
		رکن_props[TPR_MOUSE].tpr_حالت = TPR_MOUSE_SGR;
	    ورنہ اگر (نسخہ_2 >= 95)
		رکن_props[TPR_MOUSE].tpr_حالت = TPR_MOUSE_XTERM2;
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
	اگر (arg[1] >= 2500)
	{
	    رکن_props[TPR_UNDERLINE_RGB].tpr_حالت = TPR_YES;
	}

	ورنہ اگر (نسخہ_2 == 136 && arg[2] == 0)
	{
	    رکن_props[TPR_UNDERLINE_RGB].tpr_حالت = TPR_YES;

	    // PuTTY sends 0;136;0
	    اگر (arg[0] == 0)
	    {
		// supports sgr-like mouse reporting.
		رکن_props[TPR_MOUSE].tpr_حالت = TPR_MOUSE_SGR;
	    }
	    // vandyke SecureCRT sends 1;136;0
	}

	// Konsole sends 0;115;0 - but t_u8 does not actually work, therefore
	// commented out.
	// else if (version == 115 && arg[0] == 0 && arg[2] == 0)
	//     term_props[TPR_UNDERLINE_RGB].tpr_status = TPR_YES;

	// Kitty up to 9.x sends 1;400{version};{secondary-version}
	اگر (arg[0] == 1 && arg[1] >= 4000 && arg[1] <= 4009)
	{
	    رکن_props[TPR_KITTY].tpr_حالت = TPR_YES;
	    رکن_props[TPR_KITTY].tpr_سیٹ_by_termresponse = TRUE;

	    // Kitty can handle SGR mouse reporting.
	    رکن_props[TPR_MOUSE].tpr_حالت = TPR_MOUSE_SGR;
	    رکن_props[TPR_DECRQM].tpr_حالت = TPR_YES;
	}

	// foot terminal sends 1;12700;0
	اگر (arg[0] == 1 && نسخہ_2 == 12700 && arg[2] == 0)
	{
	    رکن_props[TPR_MOUSE].tpr_حالت = TPR_MOUSE_SGR;
	    رکن_props[TPR_DECRQM].tpr_حالت = TPR_YES;
	}

	// GNU screen sends 83;30600;0, 83;40500;0, etc.
	// 30600/40500 is a version number of GNU screen. DA2 support is added
	// on 3.6.  DCS string has a special meaning to GNU screen, but xterm
	// compatibility checking does not detect GNU screen.
	اگر (arg[0] == 83 && arg[1] >= 30600)
	{
	    رکن_props[TPR_CURSOR_STYLE].tpr_حالت = TPR_NO;
	    رکن_props[TPR_CURSOR_BLINK].tpr_حالت = TPR_NO;
	    رکن_props[TPR_DECRQM].tpr_حالت = TPR_NO; // screen doesn't seem
							// to handle DECRQM
							// sequences
	}

	// Xterm first responded to this request at patch level
	// 95, so assume anything below 95 is not xterm and hopefully supports
	// the underline RGB color sequence.
	اگر (نسخہ_2 < 95)
	    رکن_props[TPR_UNDERLINE_RGB].tpr_حالت = TPR_YES;

	// Getting the cursor style is only supported properly by xterm since
	// version 279 (otherwise it returns 0x18).
	اگر (نسخہ_2 < 279)
	    رکن_props[TPR_CURSOR_STYLE].tpr_حالت = TPR_NO;

	/*
	 * Take action on the detected properties.
	 */

	// Unless the underline RGB color is expected to work, disable "t_8u".
	// It does not work for the real Xterm, it resets the background color.
	// This may cause some flicker.  Alternative would be to set "t_8u"
	// here if the terminal is expected to support it, but that might
	// conflict with what was set in the .vimrc.
	اگر (رکن_props[TPR_UNDERLINE_RGB].tpr_حالت != TPR_YES
			&& *T_8U != NUL
			&& !option_was_سیٹ((char_u *)"t_8u"))
	{
	    سیٹ_string_option_براہ_راست((char_u *)"t_8u", -1, (char_u *)"",
								  OPT_FREE, 0);
	}
#اگر_تعریف_شدہ FEAT_TERMRESPONSE
	اگر (*T_8U != NUL && write_t_8u_state == MAYBE)
	    // Did skip writing t_8u, a complete redraw is needed.
	    redraw_later_clear();
	write_t_8u_state = OK;  // can output t_8u now
#اگر_ختم

	// Only set 'ttymouse' automatically if it was not set
	// by the user already.
	اگر (!option_was_سیٹ((char_u *)"ttym")
		&& (رکن_props[TPR_MOUSE].tpr_حالت == TPR_MOUSE_XTERM2
		    || رکن_props[TPR_MOUSE].tpr_حالت == TPR_MOUSE_SGR))
	{
	    سیٹ_option_قدر_give_err((char_u *)"ttym", 0L,
		    رکن_props[TPR_MOUSE].tpr_حالت == TPR_MOUSE_SGR
				    ? (char_u *)"sgr" : (char_u *)"xterm2", 0);
	}

#اگر_تعریف_شدہ FEAT_TERMRESPONSE
	صحیح need_flush = FALSE;

	// Only request the cursor style if t_SH and t_RS are
	// set. Only supported properly by xterm since version
	// 279 (otherwise it returns 0x18).
	// Only when getting the cursor style was detected to work.
	// Not for Terminal.app, it can't handle t_RS, it
	// echoes the characters to the screen.
	اگر (rcs_حالت.tr_پیش_رفت == حالت_GET
		&& رکن_props[TPR_CURSOR_STYLE].tpr_حالت == TPR_YES
		&& *T_CSH != NUL
		&& *T_CRS != NUL)
	{
	    MAY_WANT_مقصد_LOG_THIS;
	    LOG_TR1("Sending cursor style request");
	    برآمد_str(T_CRS);
	    termrequest_بھیجا(&rcs_حالت);
	    need_flush = TRUE;
	}

	// Only request the cursor blink mode if t_RC set. Not
	// for Gnome terminal, it can't handle t_RC, it
	// echoes the characters to the screen.
	// Only when getting the cursor style was detected to work.
	اگر (rbm_حالت.tr_پیش_رفت == حالت_GET
		&& رکن_props[TPR_CURSOR_BLINK].tpr_حالت == TPR_YES
		&& *T_چکری_جانچ != NUL)
	{
	    MAY_WANT_مقصد_LOG_THIS;
	    LOG_TR1("Sending cursor blink mode request");
	    برآمد_str(T_چکری_جانچ);
	    termrequest_بھیجا(&rbm_حالت);
	    need_flush = TRUE;
	}

	// Only request DEC modes via DECRQM when the terminal is known to
	// handle it.  Not for Apple Terminal.app or GNU screen, they echo
	// the trailing "p" to the screen.  See issue #19852.
	اگر (decrqm_حالت.tr_پیش_رفت == حالت_GET
		&& رکن_props[TPR_DECRQM].tpr_حالت == TPR_YES)
	{
	    MAY_WANT_مقصد_LOG_THIS;
	    LOG_TR1("Sending DECRQM requests");
	    برائے (صحیح i = 0; i < (صحیح)ARRAY_لمبائی(dec_modes); i++)
	    {
		vim_snprintf((حرف *)IObuff, IOSIZE, "\033[?%d$p", dec_modes[i]);
		برآمد_str(IObuff);
	    }
	    termrequest_بھیجا(&decrqm_حالت);
	    need_flush = TRUE;

	}

	اگر (need_flush)
	    برآمد_flush();
#اگر_ختم
    }
}

/*
 * Add "key" to "buf" and return the number of bytes used.
 * Handles special keys and multi-byte characters.
 */
    ساکن صحیح
جوڑو_کلید_مقصد_buf(صحیح کلید, char_u *buf)
{
    صحیح idx = 0;

    اگر (IS_SPECIAL(کلید))
    {
	buf[idx++] = K_SPECIAL;
	buf[idx++] = KEY2TERMCAP0(کلید);
	buf[idx++] = KEY2TERMCAP1(کلید);
    }
    ورنہ اگر (has_mbyte)
	idx += (*mb_char2bytes)(کلید, buf + idx);
    ورنہ
	buf[idx++] = کلید;
    واپس idx;
}

/*
 * Shared between handle_key_with_modifier() and handle_csi_function_key().
 */
    ساکن صحیح
put_کلید_modifiers_اندر_typebuf(
	صحیح	کلید_arg,
	صحیح	modifiers_arg,
	صحیح	csi_len,
	صحیح	انحراف,
	char_u	*buf,
	صحیح	bufsize,
	صحیح	*buflen)
{
    صحیح کلید = کلید_arg;
    صحیح modifiers = modifiers_arg;

    // Some keys need adjustment when the Ctrl modifier is used.
    کلید = may_adjust_کلید_for_ctrl(modifiers, کلید);

    // May remove the shift modifier if it's already included in the key.
    modifiers = may_remove_سرکاؤ_modifier(modifiers, کلید);

    // Produce modifiers with K_SPECIAL KS_MODIFIER {mod}
    // worst-case: 3-byte modifier + 4 byte multi-char key + NUL
    char_u string[MAX_کلید_رمز_LEN + 2];
    صحیح new_slen = modifiers2keycode(modifiers, &کلید, string);

    // reject overlong key that would overflow string
    اگر (کلید > 0x10FFFF)
	واپس -1;

    // Add the bytes for the key.
    new_slen += جوڑو_کلید_مقصد_buf(کلید, string + new_slen);

    string[new_slen] = NUL;
    اگر (put_string_اندر_typebuf(انحراف, csi_len, string, new_slen,
						 buf, bufsize, buflen) == FAIL)
	واپس -1;
    واپس new_slen - csi_len + انحراف;
}

/*
 * Parse the number from a CSI numbered sequence for an F1-F12 key:
 *	ESC [ {number} ~
 * Returns the key
 */
    ساکن صحیح
parse_csi_f_کلیدیں(صحیح arg)
{
    char_u کلید_name[2] = { 0, 0 };

    انتخاب (arg)
    {
	حالت 11: کلید_name[0] = 'k'; کلید_name[1] = '1'; توڑو;  // K_F1
	حالت 12: کلید_name[0] = 'k'; کلید_name[1] = '2'; توڑو;  // K_F2
	حالت 13: کلید_name[0] = 'k'; کلید_name[1] = '3'; توڑو;  // K_F3
	حالت 14: کلید_name[0] = 'k'; کلید_name[1] = '4'; توڑو;  // K_F4
	حالت 15: کلید_name[0] = 'k'; کلید_name[1] = '5'; توڑو;  // K_F5
	حالت 17: کلید_name[0] = 'k'; کلید_name[1] = '6'; توڑو;  // K_F6
	حالت 18: کلید_name[0] = 'k'; کلید_name[1] = '7'; توڑو;  // K_F7
	حالت 19: کلید_name[0] = 'k'; کلید_name[1] = '8'; توڑو;  // K_F8
	حالت 20: کلید_name[0] = 'k'; کلید_name[1] = '9'; توڑو;  // K_F9
	حالت 21: کلید_name[0] = 'F'; کلید_name[1] = ';'; توڑو;  // K_F10
	حالت 23: کلید_name[0] = 'F'; کلید_name[1] = '1'; توڑو;  // K_F11
	حالت 24: کلید_name[0] = 'F'; کلید_name[1] = '2'; توڑو;  // K_F12
    }
    اگر (کلید_name[0])
	واپس TERMCAP2_کلید(کلید_name[0], کلید_name[1]);
    // shouldn't happen
    واپس arg;
}

/*
 * Handle a sequence with key and modifier, one of:
 *	{lead}27;{modifier};{key}~
 *	{lead}{key};{modifier}u
 * Returns the difference in length.
 */
    ساکن صحیح
handle_کلید_with_modifier(
	صحیح	*arg,
	صحیح	csi_len,
	صحیح	انحراف,
	char_u	*buf,
	صحیح	bufsize,
	صحیح	*buflen,
	صحیح	iskitty,
	صحیح	trail)
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
    اگر (!iskitty
	    && (kitty_protocol_state == KKPS_INITIAL
		|| kitty_protocol_state == KKPS_OFF
		|| kitty_protocol_state == KKPS_AFTER_T_TE)
	    && رکن_props[TPR_KITTY].tpr_حالت != TPR_YES)
    {
#اگر_تعریف_شدہ FEAT_EVAL
	ch_log(تہی, "setting seenModifyOtherKeys to TRUE");
#اگر_ختم
	نظر_آیا_Modify_Other_کلیدیں = TRUE;
    }

    صحیح کلید = iskitty ? arg[0] : arg[2];
    صحیح modifiers = decode_modifiers(arg[1]);

    // Some terminals do not apply the Shift modifier to the key.  To make
    // mappings consistent we do it here.  TODO: support more keys.
    اگر ((modifiers & MOD_نقاب_سرکاؤ) && کلید >= 'a' && کلید <= 'z')
	کلید += 'A' - 'a';

    // Putting Esc in the buffer creates ambiguity, it can be the start of an
    // escape sequence.  Use K_ESC to avoid that.
    اگر (کلید == ESC)
	کلید = K_ESC;

    ورنہ اگر (arg[0] >= 11 && arg[0] <= 24 && trail == '~')
	کلید = parse_csi_f_کلیدیں(arg[0]);

    واپس put_کلید_modifiers_اندر_typebuf(کلید, modifiers, csi_len, انحراف, buf,
	    bufsize, buflen);
}

/*
 * Handle a sequence with key without a modifier:
 *	{lead}{key}u
 *	{lead}{key}~
 * Returns the difference in length.
 */
    ساکن صحیح
handle_کلید_without_modifier(
	صحیح	*arg,
	صحیح	csi_len,
	صحیح	انحراف,
	char_u	*buf,
	صحیح	bufsize,
	صحیح	*buflen,
	صحیح	trail)
{
    char_u  string[MAX_کلید_رمز_LEN + 1];
    صحیح	    new_slen;

    اگر (arg[0] == ESC)
    {
	// Putting Esc in the buffer creates ambiguity, it can be the start of
	// an escape sequence.  Use K_ESC to avoid that.
	string[0] = K_SPECIAL;
	string[1] = KS_EXTRA;
	string[2] = KE_ESC;
	new_slen = 3;
    }
    ورنہ اگر (arg[0] >= 11 && arg[0] <= 24 && trail == '~')
    {
	صحیح کلید = parse_csi_f_کلیدیں(arg[0]);
	string[0] = K_SPECIAL;
	string[1] = KEY2TERMCAP0(کلید);
	string[2] = KEY2TERMCAP1(کلید);
	new_slen = 3;
    }
    ورنہ
	new_slen = جوڑو_کلید_مقصد_buf(arg[0], string);

    اگر (put_string_اندر_typebuf(انحراف, csi_len, string, new_slen,
						 buf, bufsize, buflen) == FAIL)
	واپس -1;
    واپس new_slen - csi_len + انحراف;
}

/*
 * CSI function key without or with modifiers:
 *	{lead}[ABCDEFHPQRS]
 *	{lead}1;{modifier}[ABCDEFHPQRS]
 * Returns zero when nog recognized, a positive number when recognized.
 */
    ساکن صحیح
handle_csi_function_کلید(
	صحیح	argc,
	صحیح	*arg,
	صحیح	trail,
	صحیح	csi_len,
	char_u	*کلید_name,
	صحیح	انحراف,
	char_u  *buf,
	صحیح	bufsize,
	صحیح	*buflen)
{
    کلید_name[0] = 'k';
    انتخاب (trail)
    {
	حالت 'A': کلید_name[1] = 'u'; توڑو;  // K_UP
	حالت 'B': کلید_name[1] = 'd'; توڑو;  // K_DOWN
	حالت 'C': کلید_name[1] = 'r'; توڑو;  // K_RIGHT
	حالت 'D': کلید_name[1] = 'l'; توڑو;  // K_LEFT

	// case 'E': keypad BEGIN - not supported
	حالت 'F': کلید_name[0] = '@'; کلید_name[1] = '7'; توڑو;  // K_END
	حالت 'H': کلید_name[1] = 'h'; توڑو;  // K_HOME

	حالت 'P': کلید_name[1] = '1'; توڑو;  // K_F1
	حالت 'Q': کلید_name[1] = '2'; توڑو;  // K_F2
	حالت 'R': کلید_name[1] = '3'; توڑو;  // K_F3
	حالت 'S': کلید_name[1] = '4'; توڑو;  // K_F4

	طےشدہ: واپس 0;  // not recognized
    }

    صحیح کلید = TERMCAP2_کلید(کلید_name[0], کلید_name[1]);
    صحیح modifiers = argc == 2 ? decode_modifiers(arg[1]) : 0;
    put_کلید_modifiers_اندر_typebuf(کلید, modifiers,
					csi_len, انحراف, buf, bufsize, buflen);
    واپس csi_len;
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
    ساکن صحیح
handle_csi(
	char_u	*tp,
	صحیح	len,
	char_u	*argp,
	صحیح	انحراف,
	char_u  *buf,
	صحیح	bufsize,
	صحیح	*buflen,
	char_u	*کلید_name,
	صحیح	*slen)
{
    صحیح		پہلا = -1;  // optional char right after {lead}
    صحیح		trail;	     // char that ends CSI sequence
    صحیح		arg[3] = {-1, -1, -1};	// argument numbers
    صحیح		argc = 0;		// number of arguments
    char_u	*ap = argp;
    صحیح		csi_len;

    // Check for non-digit after CSI.
    اگر (!VIM_ISDIGIT(*ap))
	پہلا = *ap++;

    اگر (پہلا >= 'A' && پہلا <= 'Z')
    {
	// If "first" is in [ABCDEFHPQRS] then it is actually the "trail" and
	// no argument follows.
	trail = پہلا;
	پہلا = -1;
	--ap;
    }
    ورنہ
    {
	// Find up to three argument numbers.
	برائے (argc = 0; argc < 3; )
	{
	    اگر (ap >= tp + len)
		واپس -1;
	    اگر (*ap == ';')
		arg[argc++] = -1;  // omitted number
	    ورنہ اگر (VIM_ISDIGIT(*ap))
	    {
		arg[argc] = 0;
		برائے (;;)
		{
		    اگر (ap >= tp + len)
			واپس -1;
		    اگر (!VIM_ISDIGIT(*ap))
			توڑو;
		    // avoid overflow
		    اگر (arg[argc] <= (زیادہ_سے_زیادہ_عدد - 9) / 10)
			arg[argc] = arg[argc] * 10 + (*ap - '0');
		    ++ap;
		}
		++argc;
	    }
	    اگر (*ap == ';')
		++ap;
	    ورنہ
		توڑو;
	}

	// mrxvt has been reported to have "+" in the version. Assume
	// the escape sequence ends with a letter or one of "{|}~".
	جب (ap < tp + len
		&& !(*ap >= '{' && *ap <= '~')
		&& !ASCII_ISALPHA(*ap))
	    ++ap;
	اگر (ap >= tp + len)
	    واپس -1;
	trail = *ap;
    }

    csi_len = (صحیح)(ap - tp) + 1;

    // Response to XTQMODKEYS: "CSI > 4 ; Pv m" where Pv indicates the
    // modifyOtherKeys level.  Drop similar responses.
    اگر (پہلا == '>' && (argc == 1 || argc == 2) && trail == 'm')
    {
	اگر (arg[0] == 4 && argc == 2)
	    modify_otherkeys_state = arg[1] == 2 ? MOKS_ENABLED : MOKS_OFF;

	کلید_name[0] = (صحیح)KS_EXTRA;
	کلید_name[1] = (صحیح)KE_IGNORE;
	*slen = csi_len;
    }

    // Function key starting with CSI:
    //	{lead}[ABCDEFHPQRS]
    //	{lead}1;{modifier}[ABCDEFHPQRS]
    ورنہ اگر (پہلا == -1 && ASCII_ISUPPER(trail)
	    && (argc == 0 || (argc == 2 && arg[0] == 1)))
    {
	صحیح res = handle_csi_function_کلید(argc, arg, trail,
			      csi_len, کلید_name, انحراف, buf, bufsize, buflen);
	واپس res <= 0 ? res : len + res;
    }

    // Cursor position report: {lead}{row};{col}R
    // Eat it when there are 2 arguments and it ends in 'R'.
    // Also when u7_status is not "sent", it may be from a previous Vim that
    // just exited.  But not for <S-F3>, it sends something similar, check for
    // row and column to make sense.
    ورنہ اگر (پہلا == -1 && argc == 2 && trail == 'R')
    {
	handle_u7_response(arg, tp, csi_len);

	کلید_name[0] = (صحیح)KS_EXTRA;
	کلید_name[1] = (صحیح)KE_IGNORE;
	*slen = csi_len;
    }

#اگر (defined(UNIX) || defined(MSWIN)) \
    && (defined(FEAT_IMAGE) || defined(FEAT_EVAL))
    // Response to CSI 14 t or CSI 16 t
    ورنہ اگر (پہلا == -1 && argc >= 3
	    && (arg[0] == 4 || arg[0] == 6) && trail == 't')
    {
	اگر (arg[0] == 4)
	{
	    LOG_TRN("Received CSI 14 t response: %s", tp);
	    cell_چوڑائی = arg[2] / Columns;
	    cell_اونچائی = arg[1] / صفیں;
	}
	ورنہ
	{
	    LOG_TRN("Received CSI 16 t response: %s", tp);
	    cell_چوڑائی = arg[2];
	    cell_اونچائی = arg[1];
	}

	کلید_name[0] = (صحیح)KS_EXTRA;
	کلید_name[1] = (صحیح)KE_IGNORE;
	*slen = csi_len;
    }
#اگر_ختم

    // Primary device attributes (DA1) response
    ورنہ اگر (پہلا == '?' && trail == 'c')
    {
	LOG_TRN("Received DA1 response: %s", tp);

	*slen = csi_len;
#اگر_تعریف_شدہ FEAT_EVAL
	سیٹ_vim_var_string(VV_TERMDA1, tp, *slen);
#اگر_ختم
	apply_autocmds(EVENT_TERMRESPONSEALL,
					(char_u *)"da1", تہی, FALSE, curbuf);

	کلید_name[0] = (صحیح)KS_EXTRA;
	کلید_name[1] = (صحیح)KE_IGNORE;
	*slen = csi_len;
    }

    // DECRPM mode 2026 or 2048.
    ورنہ اگر (پہلا == '?' && trail == 'y' && argc == 2
	    && (arg[0] == 2026 || arg[0] == 2048))
    {
	صحیح setting = arg[1];

	*slen = csi_len;
	کلید_name[0] = (صحیح)KS_EXTRA;
	کلید_name[1] = (صحیح)KE_IGNORE;

#اگر_تعریف_شدہ FEAT_TERMRESPONSE
	// Mark the DECRQM request as answered so it is not sent again and
	// stoptermcap() does not wait for it.
	اگر (decrqm_حالت.tr_پیش_رفت == حالت_بھیجا)
	    decrqm_حالت.tr_پیش_رفت = حالت_GOT;
#اگر_ختم

	اگر (setting >= 0 && setting <= 4)
	{
	    LOG_TRN("Received DECRPM mode %d: %s", arg[0], tp);

	    انتخاب (arg[0])
	    {
		حالت 2026:
		    sync_output_setting = setting;
		    سیٹ_option_قدر_give_err((char_u *)"termsync",
			    setting == 1 || setting == 2, تہی, 0);
		    توڑو;
#اگر_تعریف_شدہ UNIX
		حالت 2048:
		    win_resize_setting = setting;

		    رکن_سیٹ_win_resize(true);
		    توڑو;
#اگر_ختم
	    }
	}
	ورنہ
	    LOG_TRN("Unknown DECRPM mode %d setting %d", arg[0], setting);
    }

#اگر_تعریف_شدہ UNIX
    // In-band window resize event
    ورنہ اگر (win_resize_enabled && argc >= 3 && arg[0] == 48)
    {
	صحیح اونچائی = arg[1], چوڑائی = arg[2];

	*slen = csi_len;
	کلید_name[0] = (صحیح)KS_EXTRA;
	کلید_name[1] = (صحیح)KE_IGNORE;

	// Only update if needed. Avoids intro message from disappearing on
	// startup (due to initial event).
	اگر (اونچائی != صفیں || چوڑائی != Columns)
	    سیٹ_shellsize(چوڑائی, اونچائی, true);
    }
#اگر_ختم

    // Version string: Eat it when there is at least one digit and
    // it ends in 'c'
    ورنہ اگر (*T_CRV != NUL && ap > argp + 1 && trail == 'c')
    {
	handle_نسخہ_response(پہلا, arg, argc, tp);

	*slen = csi_len;
#اگر_تعریف_شدہ FEAT_EVAL
	سیٹ_vim_var_string(VV_TERMRESPONSE, tp, *slen);
#اگر_ختم
	apply_autocmds(EVENT_TERMRESPONSE,
					تہی, تہی, FALSE, curbuf);
	apply_autocmds(EVENT_TERMRESPONSEALL,
					(char_u *)"version", تہی, FALSE, curbuf);
	کلید_name[0] = (صحیح)KS_EXTRA;
	کلید_name[1] = (صحیح)KE_IGNORE;
    }

#اگر_تعریف_شدہ FEAT_TERMRESPONSE
    // Check blinking cursor from xterm:
    // {lead}?12;1$y       set
    // {lead}?12;2$y       not set
    //
    // {lead} can be <Esc>[ or CSI
    ورنہ اگر (rbm_حالت.tr_پیش_رفت == حالت_بھیجا
	    && پہلا == '?'
	    && ap == argp + 6
	    && arg[0] == 12
	    && ap[-1] == '$'
	    && trail == 'y')
    {
	initial_cursor_blink = (arg[1] == '1');
	rbm_حالت.tr_پیش_رفت = حالت_GOT;
	LOG_TRN("Received cursor blinking mode response: %s", tp);
	کلید_name[0] = (صحیح)KS_EXTRA;
	کلید_name[1] = (صحیح)KE_IGNORE;
	*slen = csi_len;
# ifdef FEAT_EVAL
	سیٹ_vim_var_string(VV_TERMBLINKRESP, tp, *slen);
# endif
	apply_autocmds(EVENT_TERMRESPONSEALL,
					(char_u *)"cursorblink", تہی, FALSE, curbuf);
    }
#اگر_ختم

    // Kitty keyboard protocol status response: CSI ? flags u
    ورنہ اگر (پہلا == '?' && argc == 1 && trail == 'u')
    {
	// The protocol has various "progressive enhancement flags" values, but
	// we only check for zero and non-zero here.
	اگر (arg[0] == '0')
	{
	    kitty_protocol_state = KKPS_OFF;
	}
	ورنہ
	{
	    kitty_protocol_state = KKPS_ENABLED;

	    // Reset seenModifyOtherKeys just in case some key combination has
	    // been seen that set it before we get the status response.
#اگر_تعریف_شدہ FEAT_EVAL
	    ch_log(تہی, "setting seenModifyOtherKeys to FALSE");
#اگر_ختم
	    نظر_آیا_Modify_Other_کلیدیں = FALSE;
	}

	کلید_name[0] = (صحیح)KS_EXTRA;
	کلید_name[1] = (صحیح)KE_IGNORE;
	*slen = csi_len;
    }

#اگر_تعریف_شدہ FEAT_TERMRESPONSE
    // Check for a window position response from the terminal:
    //       {lead}3;{x};{y}t
    ورنہ اگر (did_request_winpos && argc == 3 && arg[0] == 3
						   && trail == 't')
    {
	winpos_x = arg[1];
	winpos_y = arg[2];
	// got finished code: consume it
	کلید_name[0] = (صحیح)KS_EXTRA;
	کلید_name[1] = (صحیح)KE_IGNORE;
	*slen = csi_len;

	اگر (--did_request_winpos <= 0)
	    winpos_حالت.tr_پیش_رفت = حالت_GOT;
    }
#اگر_ختم

    // Key with modifier:
    //	{lead}27;{modifier};{key}~
    //	{lead}{key};{modifier}u
    //	{lead}{key};{modifier}~
    // Even though we only handle four modifiers and the {modifier} value
    // should be 16 or lower, we accept all modifier values to avoid the raw
    // sequence to be passed through.
    ورنہ اگر ((arg[0] == 27 && argc == 3 && trail == '~')
		|| (argc == 2 && (trail == 'u' || trail == '~')))
    {
	صحیح iskitty = argc == 2 && (trail == 'u' || trail == '~');
	واپس len + handle_کلید_with_modifier(arg, csi_len, انحراف, buf,
		bufsize, buflen, iskitty, trail);
    }

    // Key without modifier (Kitty sends this for Esc or F3):
    //	{lead}{key}u
    //	{lead}{key}~
    ورنہ اگر (argc == 1 && (trail == 'u' || trail == '~'))
    {
	واپس len + handle_کلید_without_modifier(arg, csi_len, انحراف, buf,
		bufsize, buflen, trail);
    }

    // else: Unknown CSI sequence.  We could drop it, but then the
    // user can't create a map for it.
    واپس 0;
}

ساکن خالی
check_for_color_response(char_u *resp, صحیح len)
{
    صحیح		i, j;
    char_u	*argp;

    j = 1 + (resp[0] == ESC);
    argp = resp + j;

    اگر (len >= j + 3 && (argp[0] != '1'
			     || (argp[1] != '1' && argp[1] != '0')
			     || argp[2] != ';'))
	i = 0; // no match
    ورنہ
    {
	برائے (i = j; i < len; ++i)
	    اگر (resp[i] == '\007' || (resp[0] == OSC ? resp[i] == STERM
			: (resp[i] == ESC && i + 1 < len && resp[i + 1] == '\\')))
	    {
		صحیح is_bg = argp[1] == '1';
		صحیح is_4digit = i - j >= 21 && resp[j + 11] == '/'
						  && resp[j + 16] == '/';

		اگر (i - j >= 15 && STRNCMP(resp + j + 3, "rgb:", 4) == 0
			    && (is_4digit
				   || (resp[j + 9] == '/' && resp[j + 12] == '/')))
		{
		    char_u *tp_r = resp + j + 7;
		    char_u *tp_g = resp + j + (is_4digit ? 12 : 10);
		    char_u *tp_b = resp + j + (is_4digit ? 17 : 13);
#اگر_تعریف_شدہ FEAT_TERMRESPONSE
		    صحیح rval, gval, bval;

		    rval = hexhex2nr(tp_r);
		    gval = hexhex2nr(tp_g);
		    bval = hexhex2nr(tp_b);
#اگر_ختم
		    اگر (is_bg)
		    {
			حرف *new_bg_val = (3 * '6' < *tp_r + *tp_g +
					     *tp_b) ? "light" : "dark";

			LOG_TRN("Received RBG response: r=%d g=%d b=%d", rval, gval, bval);
#اگر_تعریف_شدہ FEAT_TERMRESPONSE
			rbg_حالت.tr_پیش_رفت = حالت_GOT;
			bg_r = rval;
			bg_g = gval;
			bg_b = bval;
#اگر_ختم
			اگر (!option_was_سیٹ((char_u *)"bg")
				      && STRCMP(p_bg, new_bg_val) != 0)
			{
			    // value differs, apply it
			    سیٹ_option_قدر_give_err((char_u *)"bg",
						  0L, (char_u *)new_bg_val, 0);
			    reset_option_was_سیٹ((char_u *)"bg");
			    redraw_asap(UPD_CLEAR);
			}
		    }
#اگر defined(FEAT_TERMRESPONSE) && defined(FEAT_TERMINAL)
		    ورنہ
		    {
			LOG_TRN("Received RFG response: r=%d g=%d b=%d", rval, gval, bval);
			rfg_حالت.tr_پیش_رفت = حالت_GOT;
			fg_r = rval;
			fg_g = gval;
			fg_b = bval;
		    }
#اگر_ختم
		}

#اگر_تعریف_شدہ FEAT_EVAL
		سیٹ_vim_var_string(is_bg ? VV_TERMRBGRESP
						  : VV_TERMRFGRESP, resp, len);
#اگر_ختم
		apply_autocmds(EVENT_TERMRESPONSEALL,
			is_bg ? (char_u *)"background" : (char_u *)"foreground",
			تہی, FALSE, curbuf);
		توڑو;
	    }
    }
    اگر (i == len)
	LOG_TR1("not enough characters for RB");
}

ساکن oscstate_T osc_state;

/*
 * Return true if currently receiving an OSC response.
 */
    bool
اندر_osc_sequence(خالی)
{
    واپس osc_state.processing;
}

/*
 * Handles any OSC sequence and places the result in "v:termosc". Returns OK on
 * success and FAIL on failure.
 */
    ساکن صحیح
handle_osc(char_u *tp, صحیح len, char_u *کلید_name, صحیح *slen)
{
    char_u آخری_char;

    اگر (!osc_state.processing)
    {
	صحیح cur;

	LOG_TRN("Received OSC response: %s", (حرف*)tp);
	// Check if it is a valid OSC sequence, and consume it. OSC format
	// consists of:
	// <identifier><data><terminator>
	// <identifier> is either <Esc>] or an OSC character
	// <terminator> can be '\007', <Esc>\ or STERM.
	cur = 1 + (tp[0] == ESC);

	اگر (len < cur + 1 + (tp[0] != OSC)) // Include terminator as well
	    واپس FAIL;

	// The whole OSC response may be larger than the typeahead buffer.
	// To handle this, keep reading data in and out of the typeahead
	// buffer until we read an OSC terminator or timeout.

	// We can't use the previous buffer since we transferred ownership of it
	// to the vim var.
	ga_init2(&osc_state.buf, 1, 1024);
#اگر_تعریف_شدہ ELAPSED_FUNC
	ELAPSED_INIT(osc_state.آغاز_tv);
#اگر_ختم
	osc_state.processing = TRUE;
	osc_state.آغاز_char = tp[0];
	آخری_char = 0;
    }
    ورنہ
	آخری_char = ((char_u *)osc_state.buf.ga_ڈیٹا)[osc_state.buf.ga_len - 1];

    کلید_name[0] = (صحیح)KS_EXTRA;

    // Read data and append to buffer. If we reach a terminator, then
    // finally set the vim var.
    برائے (صحیح i = 0; i < len; i++)
	اگر (tp[i] == '\007' || (osc_state.آغاز_char == OSC ? tp[i] == STERM
		    : ((tp[i] == ESC && i + 1 < len && tp[i + 1] == '\\')
			|| (i == 0 && tp[i] == '\\' && آخری_char == ESC)
			)))
	{
	    osc_state.processing = FALSE;

	    کلید_name[1] = (صحیح)KE_OSC;

	    ga_concat_len(&osc_state.buf, tp, i + 1 + (tp[i] == ESC));
	    ga_append(&osc_state.buf, NUL);
	    *slen = i + 1 + (tp[i] == ESC);
#اگر_تعریف_شدہ FEAT_EVAL
	    سیٹ_vim_var_string_براہ_راست(VV_TERMOSC, osc_state.buf.ga_ڈیٹا);
#اگر_ختم
	    // Check for background/foreground colour response
	    check_for_color_response(osc_state.buf.ga_ڈیٹا, osc_state.buf.ga_len - 1);

	    char_u savebg = *p_bg;
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"osc",
		    تہی, FALSE, curbuf);
	    اگر (*p_bg != savebg)
		redraw_asap(UPD_CLEAR);
	    واپس OK;
	}

    کلید_name[1] = (صحیح)KE_IGNORE;

#اگر_تعریف_شدہ ELAPSED_FUNC
    اگر (ELAPSED_FUNC(osc_state.آغاز_tv) >= p_ost)
    {
	semsg(_(e_osc_response_timed_برآمد), osc_state.buf.ga_len,
		osc_state.buf.ga_ڈیٹا);

	ga_clear(&osc_state.buf);
	osc_state.processing = FALSE;
	واپس FAIL;
    }
#اگر_ختم

    ga_concat(&osc_state.buf, tp);
    *slen = len; // Consume everything

    واپس OK;
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
    ساکن صحیح
handle_dcs(char_u *tp, char_u *argp, صحیح len, char_u *کلید_name, صحیح *slen)
{
    صحیح i, j;

    LOG_TRN("Received DCS response: %s", (حرف*)tp);
    j = 1 + (tp[0] == ESC);
    اگر (len < j + 3)
	i = len; // need more chars
    ورنہ اگر ((argp[1] != '+' && argp[1] != '$')
	    || (argp[2] != 'r' && argp[2] != 'R'))
	i = 0; // no match
    ورنہ اگر (argp[1] == '+')
	// key code response
	برائے (i = j; i < len; ++i)
	{
	    اگر ((tp[i] == ESC && i + 1 < len && tp[i + 1] == '\\')
		    || tp[i] == STERM)
	    {
#اگر_تعریف_شدہ FEAT_TERMRESPONSE
		// handle a key code response, drop a resource response
		اگر (i - j >= 3 && argp[2] == 'r')
		    got_رمز_from_رکن(tp + j, i);
#اگر_ختم
		کلید_name[0] = (صحیح)KS_EXTRA;
		کلید_name[1] = (صحیح)KE_IGNORE;
		*slen = i + 1 + (tp[i] == ESC);
		توڑو;
	    }
	}
    ورنہ
    {
	// Probably the cursor shape response.  Make sure that "i"
	// is equal to "len" when there are not sufficient
	// characters.
	برائے (i = j + 3; i < len; ++i)
	{
	    اگر (i - j == 3 && !محفوظ_isdigit(tp[i]))
		توڑو;
	    اگر (i - j == 4 && tp[i] != ' ')
		توڑو;
	    اگر (i - j == 5 && tp[i] != 'q')
		توڑو;
	    اگر (i - j == 6 && tp[i] != ESC && tp[i] != STERM)
		توڑو;
	    اگر ((i - j == 6 && tp[i] == STERM)
		    || (i - j == 7 && tp[i] == '\\'))
	    {
#اگر_تعریف_شدہ FEAT_TERMRESPONSE
		صحیح نمبر = argp[3] - '0';

		// 0, 1 = block blink, 2 = block
		// 3 = underline blink, 4 = underline
		// 5 = vertical bar blink, 6 = vertical bar
		نمبر = نمبر == 0 ? 1 : نمبر;
		initial_cursor_shape = (نمبر + 1) / 2;
		// The blink flag is actually inverted, compared to
		// the value set with T_SH.
		initial_cursor_shape_blink =
				       (نمبر & 1) ? FALSE : TRUE;
		rcs_حالت.tr_پیش_رفت = حالت_GOT;
#اگر_ختم
		LOG_TRN("Received cursor shape response: %s", tp);

		کلید_name[0] = (صحیح)KS_EXTRA;
		کلید_name[1] = (صحیح)KE_IGNORE;
		*slen = i + 1;
#اگر_تعریف_شدہ FEAT_EVAL
		سیٹ_vim_var_string(VV_TERMSTYLERESP, tp, *slen);
#اگر_ختم
		apply_autocmds(EVENT_TERMRESPONSEALL,
					(char_u *)"cursorshape", تہی, FALSE, curbuf);
		توڑو;
	    }
	}
    }

    اگر (i == len)
    {
	// These codes arrive many together, each code can be
	// truncated at any point.
	LOG_TR1("not enough characters for XT");
	واپس FAIL;
    }
    واپس OK;
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
    صحیح
check_termcode(
    صحیح		max_انحراف,
    char_u	*buf,
    صحیح		bufsize,
    صحیح		*buflen)
{
    char_u	*tp;
    char_u	*p;
    صحیح		slen = 0;	// init for GCC
    صحیح		modslen;
    صحیح		len;
    صحیح		retval = 0;
    صحیح		انحراف;
    char_u	کلید_name[2];
    صحیح		modifiers;
    char_u	*modifiers_آغاز = تہی;
    صحیح		کلید;
    صحیح		new_slen;   // Length of what will replace the termcode
    char_u	string[MAX_کلید_رمز_LEN + 1];
    صحیح		i, j;
    صحیح		idx = 0;
    صحیح		cpo_koffset;

    cpo_koffset = (vim_strchr(p_cpo, CPO_KOFFSET) != تہی);

    /*
     * Speed up the checks for terminal codes by gathering all first bytes
     * used in termleader[].  Often this is just a single <Esc>.
     */
    اگر (need_gather)
	gather_termleader();

    /*
     * Check at several positions in typebuf.tb_buf[], to catch something like
     * "x<Up>" that can be mapped. Stop at max_offset, because characters
     * after that cannot be used for mapping, and with @r commands
     * typebuf.tb_buf[] can become very long.
     * This is used often, KEEP IT FAST!
     */
    برائے (انحراف = 0; انحراف < max_انحراف; ++انحراف)
    {
	اگر (buf == تہی)
	{
	    اگر (انحراف >= typebuf.tb_len)
		توڑو;
	    tp = typebuf.tb_buf + typebuf.tb_off + انحراف;
	    len = typebuf.tb_len - انحراف;	// length of the input
	}
	ورنہ
	{
	    اگر (انحراف >= *buflen)
		توڑو;
	    tp = buf + انحراف;
	    len = *buflen - انحراف;
	}

	/*
	 * Don't check characters after K_SPECIAL, those are already
	 * translated terminal chars (avoid translating ~@^Hx).
	 */
	اگر (*tp == K_SPECIAL)
	{
	    انحراف += 2;	// there are always 2 extra characters
	    جاری;
	}

	اگر (osc_state.processing)
	{
	    // Still processing OSC response data, go straight to handler
	    // function.
	    tp[len] = NUL;
	    کلید_name[0] = NUL;
	    کلید_name[1] = NUL;
	    modifiers = 0;
	    جاؤ handle_osc;
	}

	/*
	 * Skip this position if the character does not appear as the first
	 * character in term_strings. This speeds up a lot, since most
	 * termcodes start with the same character (ESC or CSI).
	 */
	i = *tp;
	برائے (p = termleader; *p && *p != i; ++p)
	    ;
	اگر (*p == NUL)
	    جاری;

	/*
	 * Skip this position if p_ek is not set and tp[0] is an ESC and we
	 * are in Insert mode.
	 */
	اگر (*tp == ESC && !p_ek && (State & منوال_داخل_کرو))
	    جاری;

	tp[len] = NUL;
	کلید_name[0] = NUL;	// no key name found yet
	کلید_name[1] = NUL;	// no key name found yet
	modifiers = 0;		// no modifiers yet

#اگر_تعریف_شدہ FEAT_GUI
	اگر (gui.اندر_use)
	{
	    /*
	     * GUI special key codes are all of the form [CSI xx].
	     */
	    اگر (*tp == CSI)	    // Special key from GUI
	    {
		اگر (len < 3)
		    واپس -1;	    // Shouldn't happen
		slen = 3;
		کلید_name[0] = tp[1];
		کلید_name[1] = tp[2];
	    }
	}
	ورنہ
#اگر_ختم // FEAT_GUI
#اگر_تعریف_شدہ MSWIN
	    اگر (len >= 3 && tp[0] == CSI && tp[1] == KS_EXTRA
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
	    کلید_name[0] = tp[1];
	    کلید_name[1] = tp[2];
	}
	ورنہ
#اگر_ختم
	{
	    صحیح  mouse_اشاریہ_ملا = -1;
	    صحیح keypad_اشاریہ_ملا = -1;
	    صحیح keypad_slen_ملا = 0;

	    برائے (idx = 0; idx < tc_len; ++idx)
	    {
		صحیح     is_keypad = FALSE;
		/*
		 * Ignore the entry if we are not at the start of
		 * typebuf.tb_buf[]
		 * and there are not enough characters to make a match.
		 * But only when the 'K' flag is in 'cpoptions'.
		 */
		slen = termcodes[idx].len;
		modifiers_آغاز = تہی;
		اگر (cpo_koffset && انحراف && len < slen)
		    جاری;
		اگر (STRNCMP(termcodes[idx].رمز, tp,
				     (size_t)(slen > len ? len : slen)) == 0)
		{
		    صحیح	    looks_like_mouse_آغاز = FALSE;

		    اگر (len < slen)		// got a partial sequence
			واپس -1;		// need to get more chars

		    /*
		     * When found a keypad key, check if there is another key
		     * that matches and use that one.  This makes <Home> to be
		     * found instead of <kHome> when they produce the same
		     * key code.
		     */
		    اگر (termcodes[idx].name[0] == 'K'
				       && (VIM_ISDIGIT(termcodes[idx].name[1])
						|| ASCII_ISUPPER(termcodes[idx].name[1])))
		    {
			is_keypad = TRUE;
			// Only use it when there is no other match.
			اگر (keypad_اشاریہ_ملا < 0)
			{
			    keypad_اشاریہ_ملا = idx;
			    keypad_slen_ملا = slen;
			}
		    }

		    اگر (slen == 2 && len > 2
			    && termcodes[idx].رمز[0] == ESC
			    && termcodes[idx].رمز[1] == '[')
		    {
			// The mouse termcode "ESC [" is also the prefix of
			// "ESC [ I" (focus gained) and other keys.  Check some
			// more bytes to find out.
			اگر (!محفوظ_isdigit(tp[2]))
			{
			    // ESC [ without number following: Only use it when
			    // there is no other match.
			    looks_like_mouse_آغاز = TRUE;
			}
			ورنہ اگر (termcodes[idx].name[0] == KS_DEC_MOUSE)
			{
			    char_u  *nr = tp + 2;
			    صحیح	    گنتی = 0;

			    // If a digit is following it could be a key with
			    // modifier, e.g., ESC [ 1;2P.  Can be confused
			    // with DEC_MOUSE, which requires four numbers
			    // following.  If not then it can't be a DEC_MOUSE
			    // code.
			    برائے (;;)
			    {
				++گنتی;
				(خالی)getdigits(&nr);
				اگر (nr >= tp + len)
				    واپس -1;	// partial sequence
				اگر (*nr != ';')
				    توڑو;
				++nr;
				اگر (nr >= tp + len)
				    واپس -1;	// partial sequence
			    }
			    اگر (گنتی < 4)
				جاری;	// no match
			}
		    }
		    اگر (looks_like_mouse_آغاز)
		    {
			// Only use it when there is no other match.
			اگر (mouse_اشاریہ_ملا < 0)
			    mouse_اشاریہ_ملا = idx;
		    }
		    ورنہ اگر (!is_keypad)
		    {
			کلید_name[0] = termcodes[idx].name[0];
			کلید_name[1] = termcodes[idx].name[1];
			توڑو;
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
		اگر (termcodes[idx].modlen > 0 && mouse_اشاریہ_ملا < 0)
		{
		    modslen = termcodes[idx].modlen;
		    اگر (cpo_koffset && انحراف && len < modslen)
			جاری;
		    اگر (STRNCMP(termcodes[idx].رمز, tp,
				(size_t)(modslen > len ? len : modslen)) == 0)
		    {
			صحیح	    n;

			اگر (len <= modslen)	// got a partial sequence
			    واپس -1;		// need to get more chars

			اگر (tp[modslen] == termcodes[idx].رمز[slen - 1])
			    // no modifiers
			    slen = modslen + 1;
			ورنہ اگر (tp[modslen] != ';' && modslen == slen - 3)
			    // no match for "code;*X" with "code;"
			    جاری;
			ورنہ اگر (termcodes[idx].رمز[modslen] == '@'
					 && (tp[modslen] != '1'
						    || tp[modslen + 1] != ';'))
			    // no match for "<Esc>[@" with "<Esc>[1;"
			    جاری;
			ورنہ
			{
			    // Skip over the digits, the final char must
			    // follow. URXVT can use a negative value, thus
			    // also accept '-'.
			    برائے (j = slen - 2; j < len && (محفوظ_isdigit(tp[j])
				       || tp[j] == '-' || tp[j] == ';'); ++j)
				;
			    ++j;
			    اگر (len < j)	// got a partial sequence
				واپس -1;	// need to get more chars
			    اگر (tp[j - 1] != termcodes[idx].رمز[slen - 1])
				جاری;	// no match

			    modifiers_آغاز = tp + slen - 2;

			    // Match!  Convert modifier bits.
			    n = atoi((حرف *)modifiers_آغاز);
			    modifiers |= decode_modifiers(n);

			    slen = j;
			}
			اگر (termcodes[idx].name[0] == 'K'
					   && (VIM_ISDIGIT(termcodes[idx].name[1])
						    || ASCII_ISUPPER(termcodes[idx].name[1])))
			{
			    is_keypad = TRUE;
			    اگر (keypad_اشاریہ_ملا < 0)
			    {
				keypad_اشاریہ_ملا = idx;
				keypad_slen_ملا = slen;
			    }
			}
			اگر (!is_keypad)
			{
			    کلید_name[0] = termcodes[idx].name[0];
			    کلید_name[1] = termcodes[idx].name[1];
			    توڑو;
			}
		    }
		}
	    }
	    اگر (idx == tc_len && keypad_اشاریہ_ملا >= 0)
	    {
		کلید_name[0] = termcodes[keypad_اشاریہ_ملا].name[0];
		کلید_name[1] = termcodes[keypad_اشاریہ_ملا].name[1];
		slen = keypad_slen_ملا;
	    }
	    ورنہ اگر (idx == tc_len && mouse_اشاریہ_ملا >= 0)
	    {
		کلید_name[0] = termcodes[mouse_اشاریہ_ملا].name[0];
		کلید_name[1] = termcodes[mouse_اشاریہ_ملا].name[1];
	    }
	}

	اگر (کلید_name[0] == NUL
	    // Mouse codes of DEC and pterm start with <ESC>[.  When
	    // detecting the start of these mouse codes they might as well be
	    // another key code or terminal response.
#اگر_تعریف_شدہ FEAT_MOUSE_DEC
	    || کلید_name[0] == KS_DEC_MOUSE
#اگر_ختم
#اگر_تعریف_شدہ FEAT_MOUSE_PTERM
	    || کلید_name[0] == KS_PTERM_MOUSE
#اگر_ختم
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
	    اگر (((tp[0] == ESC && len >= 3 && tp[1] == '[')
			|| (tp[0] == CSI && len >= 2))
		    && vim_strchr((char_u *)"0123456789>?ABCDEFHPQRS",
								*argp) != تہی)
	    {
		صحیح resp = handle_csi(tp, len, argp, انحراف, buf,
					     bufsize, buflen, کلید_name, &slen);
		اگر (resp != 0)
		{
#اگر_تعریف_شدہ DEBUG_TERMRESPONSE
		    اگر (resp == -1)
			LOG_TR1("Not enough characters for CSI sequence");
#اگر_ختم
		    واپس resp;
		}
	    }

	    // Check for OSC responses from terminal
	    ورنہ اگر ((tp[0] == ESC && len >= 2 && tp[1] == ']') || tp[0] == OSC)
	    {
handle_osc:
		اگر (handle_osc(tp, len, کلید_name, &slen) == FAIL)
		    واپس -1;
	    }

	    // Check for key code response from xterm,
	    // starting with <Esc>P or DCS
	    // It would only be needed with this condition:
	    //	    (check_for_codes || rcs_status.tr_progress == STATUS_SENT)
	    // Now this is always done so that DCS codes don't mess up things.
	    ورنہ اگر ((tp[0] == ESC && len >= 2 && tp[1] == 'P') || tp[0] == DCS)
	    {
		اگر (handle_dcs(tp, argp, len, کلید_name, &slen) == FAIL)
		    واپس -1;
	    }
	}

	اگر (کلید_name[0] == NUL)
	    جاری;	    // No match at this position, try next one

	// We only get here when we have a complete termcode match

#اگر defined(FEAT_GUI) || defined(MSWIN)
	/*
	 * For scroll events from the GUI or MS-Windows console, fetch the
	 * pointer coordinates so that we know which window to scroll later.
	 */
	اگر (TRUE
# اگر defined(FEAT_GUI) && !defined(MSWIN)
		&& gui.اندر_use
# endif
		&& کلید_name[0] == (صحیح)KS_EXTRA
		&& (کلید_name[1] == (صحیح)KE_X1MOUSE
		    || کلید_name[1] == (صحیح)KE_X2MOUSE
		    || کلید_name[1] == (صحیح)KE_MOUSEMOVE_XY
		    || کلید_name[1] == (صحیح)KE_MOUSELEFT
		    || کلید_name[1] == (صحیح)KE_MOUSERIGHT
		    || کلید_name[1] == (صحیح)KE_MOUSEDOWN
		    || کلید_name[1] == (صحیح)KE_MOUSEUP))
	{
	    char_u	بائٹیں[6];
	    صحیح		عدد_بائٹیں = get_بائٹیں_from_buf(tp + slen, بائٹیں, 4);

	    اگر (عدد_بائٹیں == -1)	// not enough coordinates
		واپس -1;
	    mouse_ستون = 128 * (بائٹیں[0] - ' ' - 1) + بائٹیں[1] - ' ' - 1;
	    mouse_صف = 128 * (بائٹیں[2] - ' ' - 1) + بائٹیں[3] - ' ' - 1;
	    slen += عدد_بائٹیں;
	    // equal to K_MOUSEMOVE
	    اگر (کلید_name[1] == (صحیح)KE_MOUSEMOVE_XY)
		کلید_name[1] = (صحیح)KE_MOUSEMOVE;
	}
	ورنہ
#اگر_ختم
	/*
	 * If it is a mouse click, get the coordinates.
	 */
	اگر (کلید_name[0] == KS_MOUSE
#اگر_تعریف_شدہ FEAT_MOUSE_GPM
		|| کلید_name[0] == KS_GPM_MOUSE
#اگر_ختم
#اگر_تعریف_شدہ FEAT_MOUSE_JSB
		|| کلید_name[0] == KS_JSBTERM_MOUSE
#اگر_ختم
#اگر_تعریف_شدہ FEAT_MOUSE_NET
		|| کلید_name[0] == KS_NETTERM_MOUSE
#اگر_ختم
#اگر_تعریف_شدہ FEAT_MOUSE_DEC
		|| کلید_name[0] == KS_DEC_MOUSE
#اگر_ختم
#اگر_تعریف_شدہ FEAT_MOUSE_PTERM
		|| کلید_name[0] == KS_PTERM_MOUSE
#اگر_ختم
#اگر_تعریف_شدہ FEAT_MOUSE_URXVT
		|| کلید_name[0] == KS_URXVT_MOUSE
#اگر_ختم
		|| کلید_name[0] == KS_SGR_MOUSE
		|| کلید_name[0] == KS_SGR_MOUSE_RELEASE)
	{
	    اگر (check_termcode_mouse(tp, &slen, کلید_name, modifiers_آغاز, idx,
							     &modifiers) == -1)
		واپس -1;
	}

#اگر_تعریف_شدہ FEAT_GUI
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
	ورنہ اگر (کلید_name[0] == (صحیح)KS_MENU)
	{
	    long_u	val;
	    صحیح		عدد_بائٹیں = get_long_from_buf(tp + slen, &val);

	    اگر (عدد_بائٹیں == -1)
		واپس -1;
	    current_menu = (vimmenu_T *)val;
	    slen += عدد_بائٹیں;

	    // The menu may have been deleted right after it was used, check
	    // for that.
	    اگر (check_menu_اشارہ_گر(جڑ_menu, current_menu) == FAIL)
	    {
		کلید_name[0] = KS_EXTRA;
		کلید_name[1] = (صحیح)KE_IGNORE;
	    }
	}
# endif
# ifdef FEAT_GUI_TABLINE
	ورنہ اگر (کلید_name[0] == (صحیح)KS_TABLINE)
	{
	    // Selecting tabline tab or using its menu.
	    char_u	بائٹیں[6];
	    صحیح		عدد_بائٹیں = get_بائٹیں_from_buf(tp + slen, بائٹیں, 1);

	    اگر (عدد_بائٹیں == -1)
		واپس -1;
	    current_tab = (صحیح)بائٹیں[0];
	    اگر (current_tab == 255)	// -1 in a byte gives 255
		current_tab = -1;
	    slen += عدد_بائٹیں;
	}
	ورنہ اگر (کلید_name[0] == (صحیح)KS_TABMENU)
	{
	    // Selecting tabline tab or using its menu.
	    char_u	بائٹیں[6];
	    صحیح		عدد_بائٹیں = get_بائٹیں_from_buf(tp + slen, بائٹیں, 2);

	    اگر (عدد_بائٹیں == -1)
		واپس -1;
	    current_tab = (صحیح)بائٹیں[0];
	    current_tabmenu = (صحیح)بائٹیں[1];
	    slen += عدد_بائٹیں;
	}
# endif
# ifndef USE_ON_FLY_SCROLL
	ورنہ اگر (کلید_name[0] == (صحیح)KS_VER_SCROLLBAR)
	{
	    long_u	val;
	    char_u	بائٹیں[6];
	    صحیح		عدد_بائٹیں;

	    // Get the last scrollbar event in the queue of the same type
	    j = 0;
	    برائے (i = 0; tp[j] == CSI && tp[j + 1] == KS_VER_SCROLLBAR
						     && tp[j + 2] != NUL; ++i)
	    {
		j += 3;
		عدد_بائٹیں = get_بائٹیں_from_buf(tp + j, بائٹیں, 1);
		اگر (عدد_بائٹیں == -1)
		    توڑو;
		اگر (i == 0)
		    current_scrollbar = (صحیح)بائٹیں[0];
		ورنہ اگر (current_scrollbar != (صحیح)بائٹیں[0])
		    توڑو;
		j += عدد_بائٹیں;
		عدد_بائٹیں = get_long_from_buf(tp + j, &val);
		اگر (عدد_بائٹیں == -1)
		    توڑو;
		scrollbar_قدر = val;
		j += عدد_بائٹیں;
		slen = j;
	    }
	    اگر (i == 0)		// not enough characters to make one
		واپس -1;
	}
	ورنہ اگر (کلید_name[0] == (صحیح)KS_HOR_SCROLLBAR)
	{
	    long_u	val;
	    صحیح		عدد_بائٹیں;

	    // Get the last horiz. scrollbar event in the queue
	    j = 0;
	    برائے (i = 0; tp[j] == CSI && tp[j + 1] == KS_HOR_SCROLLBAR
						     && tp[j + 2] != NUL; ++i)
	    {
		j += 3;
		عدد_بائٹیں = get_long_from_buf(tp + j, &val);
		اگر (عدد_بائٹیں == -1)
		    توڑو;
		scrollbar_قدر = val;
		j += عدد_بائٹیں;
		slen = j;
	    }
	    اگر (i == 0)		// not enough characters to make one
		واپس -1;
	}
# endif // !USE_ON_FLY_SCROLL
#اگر_ختم // FEAT_GUI

#اگر defined(UNIX) || defined(VMS)
	/*
	 * Handle FocusIn/FocusOut event sequences reported by XTerm.
	 * (CSI I/CSI O)
	 */
	اگر (کلید_name[0] == KS_EXTRA
# ifdef FEAT_GUI
		&& !gui.اندر_use
# endif
	    )
	{
	    اگر (کلید_name[1] == KE_FOCUSGAINED)
	    {
		اگر (!focus_state)
		{
		    ui_focus_change(TRUE);
		    did_cursorhold = TRUE;
		    focus_state = TRUE;
		}
		کلید_name[1] = (صحیح)KE_IGNORE;
	    }
	    ورنہ اگر (کلید_name[1] == KE_FOCUSLOST)
	    {
		اگر (focus_state)
		{
		    ui_focus_change(FALSE);
		    did_cursorhold = TRUE;
		    focus_state = FALSE;
		}
		کلید_name[1] = (صحیح)KE_IGNORE;
	    }
	}
#اگر_ختم

	/*
	 * Change <xHome> to <Home>, <xUp> to <Up>, etc.
	 */
	کلید = handle_x_کلیدیں(TERMCAP2_کلید(کلید_name[0], کلید_name[1]));

	// Add any modifier codes to our string.
	new_slen = modifiers2keycode(modifiers, &کلید, string);

	// Finally, add the special key code to our string
	کلید_name[0] = KEY2TERMCAP0(کلید);
	کلید_name[1] = KEY2TERMCAP1(کلید);
	اگر (کلید_name[0] == KS_کلید)
	{
	    // from ":set <M-b>=xx"
	    اگر (has_mbyte)
		new_slen += (*mb_char2bytes)(کلید_name[1], string + new_slen);
	    ورنہ
		string[new_slen++] = کلید_name[1];
	}
	ورنہ اگر (osc_state.processing ||
		(new_slen == 0
		 && کلید_name[0] == KS_EXTRA
		 && کلید_name[1] == KE_IGNORE))
	{
	    // Do not put K_IGNORE into the buffer, do return KEYLEN_REMOVED
	    // to indicate what happened.
	    retval = KEYLEN_REMOVED;
	}
	ورنہ
	{
	    string[new_slen++] = K_SPECIAL;
	    string[new_slen++] = کلید_name[0];
	    string[new_slen++] = کلید_name[1];
	}
	اگر (put_string_اندر_typebuf(انحراف, slen, string, new_slen,
						 buf, bufsize, buflen) == FAIL)
	    واپس -1;
	واپس retval == 0 ? (len + new_slen - slen + انحراف) : retval;
    }

#اگر_تعریف_شدہ FEAT_TERMRESPONSE
    LOG_TR1("normal character");
#اگر_ختم

    واپس 0;			    // no match found
}

#اگر defined(FEAT_TERMINAL) && defined(FEAT_TERMRESPONSE)
/*
 * Get the text foreground color, if known.
 */
    خالی
رکن_get_fg_color(char_u *r, char_u *g, char_u *b)
{
    اگر (rfg_حالت.tr_پیش_رفت != حالت_GOT)
	واپس;

    *r = fg_r;
    *g = fg_g;
    *b = fg_b;
}
#اگر_ختم

#اگر_تعریف_شدہ FEAT_TERMRESPONSE
/*
 * Get the text background color, if known.  Returns OK when the OSC 11
 * response has been parsed and *r, *g, *b are populated; FAIL otherwise.
 */
    صحیح
رکن_get_bg_color(char_u *r, char_u *g, char_u *b)
{
    اگر (rbg_حالت.tr_پیش_رفت != حالت_GOT)
	واپس FAIL;

    *r = bg_r;
    *g = bg_g;
    *b = bg_b;
    واپس OK;
}
#اگر_ختم

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
    scid_T	sid_arg غیر_مستعمل,	// script ID to use for <SID>,
				// or 0 to use current_sctx
    صحیح		flags,
    صحیح		*did_simplify)
{
    صحیح		i;
    صحیح		slen;
    صحیح		کلید;
    size_t	dlen = 0;
    char_u	*src;
    صحیح		do_backslash;	// backslash is a special character
    صحیح		do_special;	// recognize <> key codes
    صحیح		do_کلید_رمز;	// recognize raw key codes
    char_u	*نتیجہ;	// buffer for resulting string
    garray_T	ga;

    do_backslash = (vim_strchr(p_cpo, CPO_BSLASH) == تہی);
    do_special = (vim_strchr(p_cpo, CPO_SPECI) == تہی)
						  || (flags & REPTERM_SPECIAL);
    do_کلید_رمز = (vim_strchr(p_cpo, CPO_KEYCODE) == تہی);
    src = from;

    /*
     * Allocate space for the translation.  Worst case a single character is
     * replaced by 6 bytes (shifted special key), plus a NUL at the end.
     * In the rare case more might be needed ga_grow() must be called again.
     */
    ga_init2(&ga, 1L, 100);
    اگر (ga_grow(&ga, (صحیح)(STRLEN(src) * 6 + 1)) == FAIL) // out of memory
    {
	*bufp = تہی;
	واپس from;
    }
    نتیجہ = ga.ga_ڈیٹا;

    /*
     * Check for #n at start only: function key n
     */
    اگر ((flags & REPTERM_FROM_PART) && src[0] == '#' && VIM_ISDIGIT(src[1]))
    {
	نتیجہ[dlen++] = K_SPECIAL;
	نتیجہ[dlen++] = 'k';
	اگر (src[1] == '0')
	    نتیجہ[dlen++] = ';';	// #0 is F10 is "k;"
	ورنہ
	    نتیجہ[dlen++] = src[1];	// #3 is F3 is "k3"
	src += 2;
    }

    /*
     * Copy each byte from *from to result[dlen]
     */
    جب (*src != NUL)
    {
	/*
	 * If 'cpoptions' does not contain '<', check for special key codes,
	 * like "<C-S-LeftMouse>"
	 */
	اگر (do_special && ((flags & REPTERM_DO_LT)
					      || STRNCMP(src, "<lt>", 4) != 0))
	{
#اگر_تعریف_شدہ FEAT_EVAL
	    /*
	     * Change <SID>Func to K_SNR <script-nr> _Func.  This name is used
	     * for script-local user functions.
	     * (room: 5 * 6 = 30 bytes; needed: 3 + <nr> + 1 <= 14)
	     * Also change <SID>name.Func to K_SNR <import-script-nr> _Func.
	     * Only if "name" is recognized as an import.
	     */
	    اگر (STRNICMP(src, "<SID>", 5) == 0)
	    {
		اگر (sid_arg < 0 || (sid_arg == 0 && current_sctx.sc_sid <= 0))
		    emsg(_(e_using_sid_not_اندر_script_context));
		ورنہ
		{
		    char_u  *dot;
		    طویل    sid = sid_arg != 0 ? sid_arg : current_sctx.sc_sid;

		    src += 5;
		    اگر (اندر_vim9script()
				       && (dot = vim_strchr(src, '.')) != تہی)
		    {
			imported_T *imp = ڈھونڈو_imported(src, dot - src, FALSE);

			اگر (imp != تہی)
			{
			    scriptitem_T    *si = SCRIPT_شے(imp->imp_sid);
			    size_t	    len;

			    src = dot + 1;
			    اگر (si->sn_autoload_سابقہ != تہی)
			    {
				// Turn "<SID>name.Func"
				// into "scriptname#Func".
				len = STRLEN(si->sn_autoload_سابقہ);
				اگر (ga_grow(&ga,
				     (صحیح)(STRLEN(src) * 6 + len + 1)) == FAIL)
				{
				    ga_clear(&ga);
				    *bufp = تہی;
				    واپس from;
				}
				نتیجہ = ga.ga_ڈیٹا;
				STRCPY(نتیجہ + dlen, si->sn_autoload_سابقہ);
				dlen += len;
				جاری;
			    }
			    sid = imp->imp_sid;
			}
		    }

		    نتیجہ[dlen++] = K_SPECIAL;
		    نتیجہ[dlen++] = (صحیح)KS_EXTRA;
		    نتیجہ[dlen++] = (صحیح)KE_SNR;
		    sprintf((حرف *)نتیجہ + dlen, "%ld", sid);
		    dlen += STRLEN(نتیجہ + dlen);
		    نتیجہ[dlen++] = '_';
		    جاری;
		}
	    }
#اگر_ختم
	    صحیح fsk_flags = FSK_KEYCODE
			| ((flags & REPTERM_NO_SIMPLIFY) ? 0 : FSK_SIMPLIFY)
			| ((flags & REPTERM_FROM_PART) ? FSK_FROM_PART : 0);
	    slen = trans_special(&src, نتیجہ + dlen, fsk_flags,
							   TRUE, did_simplify);
	    اگر (slen > 0)
	    {
		dlen += slen;
		جاری;
	    }
	}

	/*
	 * If 'cpoptions' does not contain 'k', see if it's an actual key-code.
	 * Note that this is also checked after replacing the <> form.
	 * Single character codes are NOT replaced (e.g. ^H or DEL), because
	 * it could be a character in the file.
	 */
	اگر (do_کلید_رمز)
	{
	    صحیح len;
	    i = ڈھونڈو_رکن_bykeys(src, &len);
	    اگر (i >= 0)
	    {
		نتیجہ[dlen++] = K_SPECIAL;
		نتیجہ[dlen++] = termcodes[i].name[0];
		نتیجہ[dlen++] = termcodes[i].name[1];
		src += len;
		// If terminal code matched, continue after it.
		جاری;
	    }
	}

#اگر_تعریف_شدہ FEAT_EVAL
	اگر (do_special)
	{
	    char_u	*p, *s, len;

	    /*
	     * Replace <Leader> by the value of "mapleader".
	     * Replace <LocalLeader> by the value of "maplocalleader".
	     * If "mapleader" or "maplocalleader" isn't set use a backslash.
	     */
	    اگر (STRNICMP(src, "<Leader>", 8) == 0)
	    {
		len = 8;
		p = get_var_قدر((char_u *)"g:mapleader");
	    }
	    ورنہ اگر (STRNICMP(src, "<LocalLeader>", 13) == 0)
	    {
		len = 13;
		p = get_var_قدر((char_u *)"g:maplocalleader");
	    }
	    ورنہ
	    {
		len = 0;
		p = تہی;
	    }
	    اگر (len != 0)
	    {
		// Allow up to 8 * 6 characters for "mapleader".
		اگر (p == تہی || *p == NUL || STRLEN(p) > 8 * 6)
		    s = (char_u *)"\\";
		ورنہ
		    s = p;
		جب (*s != NUL)
		    نتیجہ[dlen++] = *s++;
		src += len;
		جاری;
	    }
	}
#اگر_ختم

	/*
	 * Remove CTRL-V and ignore the next character.
	 * For "from" side the CTRL-V at the end is included, for the "to"
	 * part it is removed.
	 * If 'cpoptions' does not contain 'B', also accept a backslash.
	 */
	کلید = *src;
	اگر (کلید == Ctrl_V || (do_backslash && کلید == '\\'))
	{
	    ++src;				// skip CTRL-V or backslash
	    اگر (*src == NUL)
	    {
		اگر (flags & REPTERM_FROM_PART)
		    نتیجہ[dlen++] = کلید;
		توڑو;
	    }
	}

	// skip multibyte char correctly
	برائے (i = (*mb_ptr2len)(src); i > 0; --i)
	{
	    /*
	     * If the character is K_SPECIAL, replace it with K_SPECIAL
	     * KS_SPECIAL KE_FILLER.
	     * If compiled with the GUI replace CSI with K_CSI.
	     */
	    اگر (*src == K_SPECIAL)
	    {
		نتیجہ[dlen++] = K_SPECIAL;
		نتیجہ[dlen++] = KS_SPECIAL;
		نتیجہ[dlen++] = KE_FILLER;
	    }
#اگر_تعریف_شدہ FEAT_GUI
	    ورنہ اگر (*src == CSI)
	    {
		نتیجہ[dlen++] = K_SPECIAL;
		نتیجہ[dlen++] = KS_EXTRA;
		نتیجہ[dlen++] = (صحیح)KE_CSI;
	    }
#اگر_ختم
	    ورنہ
		نتیجہ[dlen++] = *src;
	    ++src;
	}
    }
    نتیجہ[dlen] = NUL;

    /*
     * Copy the new string to allocated memory.
     * If this fails, just return from.
     */
    اگر ((*bufp = vim_strsave(نتیجہ)) != تہی)
	from = *bufp;
    vim_free(نتیجہ);
    واپس from;
}

/*
 * Find a termcode with keys 'src' (must be NUL terminated).
 * Return the index in termcodes[], or -1 if not found.
 */
    ساکن صحیح
ڈھونڈو_رکن_bykeys(char_u *src, صحیح *matchlen)
{
    صحیح		i, j;
    صحیح		len = (صحیح)STRLEN(src);
    صحیح         ملا = -1;
    // Don't return a match for a single character
    صحیح         foundlen = 1;
    صحیح         slen, modslen;
    صحیح         thislen;

    // Most input bytes cannot start a terminal code.  Reuse the same leader
    // table as check_termcode() to avoid scanning termcodes[] unnecessarily.
    اگر (need_gather)
	gather_termleader();
    اگر (*src == NUL || vim_strchr(termleader, *src) == تہی)
	واپس -1;

    // find longest match
    // borrows part of check_termcode
    برائے (i = 0; i < tc_len; ++i)
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
	اگر (modslen > 0)
	{
	    اگر (len > modslen
			&& STRNCMP(termcodes[i].رمز, src, (size_t)modslen) == 0)
	    {
		thislen = 0;

		اگر (src[modslen] == termcodes[i].رمز[slen - 1])
		    // no modifiers
		    thislen = modslen + 1;
		ورنہ اگر (src[modslen] != ';' && modslen == slen - 3)
		    // no match for "code;*X" with "code;"
		    جاری;
		ورنہ اگر (termcodes[i].رمز[modslen] == '@'
				&& (src[modslen] != '1'
					    || src[modslen + 1] != ';'))
		    // no match for "<Esc>[@" with "<Esc>[1;"
		    جاری;
		ورنہ
		{
		    // Skip over the digits, the final char must
		    // follow. URXVT can use a negative value, thus
		    // also accept '-'.
		    برائے (j = slen - 2; j < len && (محفوظ_isdigit(src[j])
			       || src[j] == '-' || src[j] == ';'); ++j)
			;
		    ++j;
		    اگر (len < j)	// got a partial sequence
			جاری;
		    اگر (src[j - 1] != termcodes[i].رمز[slen - 1])
			جاری;	// no match

		    thislen = j;
		}

		اگر (thislen > foundlen)
		{
		    ملا = i;
		    foundlen = thislen;
		}
	    }
	}
	ورنہ
	{
	    اگر (slen > foundlen && len >= slen
			&& STRNCMP(termcodes[i].رمز, src, (size_t)slen) == 0)
	    {
		ملا = i;
		foundlen = slen;
	    }
	}
    }

    اگر (matchlen != تہی && ملا >= 0)
	*matchlen = foundlen;

    واپس ملا;
}

/*
 * Gather the first characters in the terminal key codes into a string.
 * Used to speed up check_termcode().
 */
    ساکن خالی
gather_termleader(خالی)
{
    صحیح	    i;
    صحیح	    len = 0;

#اگر_تعریف_شدہ FEAT_GUI
    اگر (gui.اندر_use)
	termleader[len++] = CSI;    // the GUI codes are not in termcodes[]
#اگر_ختم

    // Querying cell size uses CSI 14 t
#اگر defined(MSWIN) && (defined(FEAT_IMAGE) || defined(FEAT_EVAL))
# ifdef FEAT_GUI
    اگر (!gui.اندر_use)
# endif
    {
	termleader[len++] = ESC;
# ifdef FEAT_GUI
	اگر (!gui.اندر_use)
# endif
	    termleader[len++] = CSI;
    }
#اگر_ختم

#اگر_تعریف_شدہ FEAT_TERMRESPONSE
    اگر (check_for_codes || *T_CRS != NUL)
	termleader[len++] = DCS;    // the termcode response starts with DCS
				    // in 8-bit mode
#اگر_ختم
    termleader[len] = NUL;

    برائے (i = 0; i < tc_len; ++i)
	اگر (vim_strchr(termleader, termcodes[i].رمز[0]) == تہی)
	{
	    termleader[len++] = termcodes[i].رمز[0];
	    termleader[len] = NUL;
	}

    need_gather = FALSE;
}

/*
 * Show all termcodes (for ":set termcap")
 * This code looks a lot like showoptions(), but is different.
 * "flags" can have OPT_ONECOLUMN.
 */
    خالی
show_termcodes(صحیح flags)
{
    صحیح		ستون;
    صحیح		*اشیاء;
    صحیح		شے_گنتی;
    صحیح		تسلسل;
    صحیح		صف, صفیں_2;
    صحیح		ستون_تعداد;
    صحیح		i;
    صحیح		len;

#تعریف_کرو INC3 27	    // try to make three columns
#تعریف_کرو INC2 40	    // try to make two columns
#تعریف_کرو GAP 2	    // spaces between columns

    اگر (tc_len == 0)	    // no terminal codes (must be GUI)
	واپس;
    اشیاء = ALLOC_MULT(صحیح, tc_len);
    اگر (اشیاء == تہی)
	واپس;

    // Highlight title
    پیغام_puts_title(_("\n--- Terminal keys ---"));

    /*
     * Do the loop three times:
     * 1. display the short items (non-strings and short strings)
     * 2. display the medium items (medium length strings)
     * 3. display the long items (remaining strings)
     * When "flags" has OPT_ONECOLUMN do everything in 3.
     */
    برائے (تسلسل = (flags & OPT_ONECOLUMN) ? 3 : 1; تسلسل <= 3 && !got_int; ++تسلسل)
    {
	/*
	 * collect the items in items[]
	 */
	شے_گنتی = 0;
	برائے (i = 0; i < tc_len; i++)
	{
	    len = show_one_termcode(termcodes[i].name,
						    termcodes[i].رمز, FALSE);
	    اگر ((flags & OPT_ONECOLUMN) ||
		    (len <= INC3 - GAP ? تسلسل == 1
			: len <= INC2 - GAP ? تسلسل == 2
			: تسلسل == 3))
		اشیاء[شے_گنتی++] = i;
	}

	/*
	 * display the items
	 */
	اگر (تسلسل <= 2)
	{
	    ستون_تعداد = (Columns + GAP) / (تسلسل == 1 ? INC3 : INC2);
	    اگر (ستون_تعداد == 0)
		ستون_تعداد = 1;
	    صفیں_2 = (شے_گنتی + ستون_تعداد - 1) / ستون_تعداد;
	}
	ورنہ	// run == 3
	    صفیں_2 = شے_گنتی;
	برائے (صف = 0; صف < صفیں_2 && !got_int; ++صف)
	{
	    پیغام_putchar('\n');			// go to next line
	    اگر (got_int)			// 'q' typed in more
		توڑو;
	    ستون = 0;
	    برائے (i = صف; i < شے_گنتی; i += صفیں_2)
	    {
		پیغام_ستون = ستون;			// make columns
		show_one_termcode(termcodes[اشیاء[i]].name,
					      termcodes[اشیاء[i]].رمز, TRUE);
		اگر (تسلسل == 2)
		    ستون += INC2;
		ورنہ
		    ستون += INC3;
	    }
	    برآمد_flush();
	    ui_breakcheck();
	}
    }
    vim_free(اشیاء);
}

/*
 * Show one termcode entry.
 * Output goes into IObuff[]
 */
    صحیح
show_one_termcode(char_u *name, char_u *رمز, صحیح printit)
{
    char_u	*p;
    صحیح		len;

    اگر (name[0] > '~')
    {
	IObuff[0] = ' ';
	IObuff[1] = ' ';
	IObuff[2] = ' ';
	IObuff[3] = ' ';
    }
    ورنہ
    {
	IObuff[0] = 't';
	IObuff[1] = '_';
	IObuff[2] = name[0];
	IObuff[3] = name[1];
    }
    IObuff[4] = ' ';

    p = get_special_کلید_name(TERMCAP2_کلید(name[0], name[1]), 0);
    اگر (p[1] != 't')
	STRCPY(IObuff + 5, p);
    ورنہ
	IObuff[5] = NUL;
    len = (صحیح)STRLEN(IObuff);
    کرو
	IObuff[len++] = ' ';
    جب (len < 17);
    IObuff[len] = NUL;
    اگر (رمز == تہی)
	len += 4;
    ورنہ
	len += vim_strsize(رمز);

    اگر (printit)
    {
	پیغام_puts((حرف *)IObuff);
	اگر (رمز == تہی)
	    پیغام_puts("NULL");
	ورنہ
	    پیغام_outtrans(رمز);
    }
    واپس len;
}

#اگر defined(FEAT_TERMRESPONSE)
/*
 * For Xterm >= 140 compiled with OPT_TCAP_QUERY: Obtain the actually used
 * termcap codes from the terminal itself.
 * We get them one by one to avoid a very long response string.
 */
ساکن صحیح xt_اشاریہ_اندر = 0;
ساکن صحیح xt_اشاریہ_برآمد = 0;

    ساکن خالی
req_codes_from_رکن(خالی)
{
    xt_اشاریہ_برآمد = 0;
    xt_اشاریہ_اندر = 0;
    req_مزید_codes_from_رکن();
}

    ساکن خالی
req_مزید_codes_from_رکن(خالی)
{
    حرف	buf[32];  // extra size to shut up LGTM
    صحیح		old_idx = xt_اشاریہ_برآمد;

    // Don't do anything when going to exit.
    اگر (exiting)
	واپس;

    // Send up to 10 more requests out than we received.  Avoid sending too
    // many, there can be a buffer overflow somewhere.
    جب (xt_اشاریہ_برآمد < xt_اشاریہ_اندر + 10 && کلید_نام[xt_اشاریہ_برآمد] != تہی)
    {
	حرف *کلید_name = کلید_نام[xt_اشاریہ_برآمد];

	MAY_WANT_مقصد_LOG_THIS;
	LOG_TRN("Requesting XT %d: %s", xt_اشاریہ_برآمد, کلید_name);
	اگر (کلید_name[2] != NUL)
	    sprintf(buf, "\033P+q%02x%02x%02x\033\\", کلید_name[0], کلید_name[1], کلید_name[2]);
	ورنہ
	    sprintf(buf, "\033P+q%02x%02x\033\\", کلید_name[0], کلید_name[1]);
	برآمد_str_nf((char_u *)buf);
	++xt_اشاریہ_برآمد;
    }

    // Send the codes out right away.
    اگر (xt_اشاریہ_برآمد != old_idx)
	برآمد_flush();
}

/*
 * Decode key code response from xterm:
 * '<Esc>P1+r<name>=<string><Esc>\' if it is enabled/supported
 * '<Esc>P0+r<Esc>\'                if it not enabled
 * A "0" instead of the "1" indicates a code that isn't supported.
 * Both <name> and <string> are encoded in hex.
 * "code" points to the "0" or "1".
 */
    ساکن خالی
got_رمز_from_رکن(char_u *رمز, صحیح len)
{
# define XT_LEN 100
    char_u	name[4];
    char_u	str[XT_LEN];
    صحیح		i;
    صحیح		j = 0;
    صحیح		c;

    // A '1' means the code is supported, a '0' means it isn't.
    // If it is supported, there must be a '=' following
    // When half the length is > XT_LEN we can't use it.
    اگر (رمز[0] == '1' && (رمز[7] == '=' || رمز[9] == '=') && len / 2 < XT_LEN)
    {
	// Get the name from the response and find it in the table.
	name[0] = hexhex2nr(رمز + 3);
	name[1] = hexhex2nr(رمز + 5);
	اگر (رمز[9] == '=')
	    name[2] = hexhex2nr(رمز + 7);
	ورنہ
	    name[2] = NUL;
	name[3] = NUL;
	برائے (i = 0; کلید_نام[i] != تہی; ++i)
	{
	    اگر (STRCMP(کلید_نام[i], name) == 0)
	    {
		xt_اشاریہ_اندر = i;
		توڑو;
	    }
	}

	LOG_TRN("Received XT %d: %s", xt_اشاریہ_اندر, (حرف *)name);

	اگر (کلید_نام[i] != تہی)
	{
	    i = (رمز[7] == '=') ? 8 : 10;
	    برائے (; (c = hexhex2nr(رمز + i)) >= 0; i += 2)
		str[j++] = c;
	    str[j] = NUL;
	    اگر (name[0] == 'C' && name[1] == 'o')
	    {
		// Color count is not a key code.
		صحیح val = atoi((حرف *)str);
# اگر defined(FEAT_EVAL)
		اگر (val == t_colors)
		    ch_log(تہی, "got_code_from_term(Co): no change (%d)", val);
		ورنہ
		    ch_log(تہی,
			       "got_code_from_term(Co): changed from %d to %d",
								t_colors, val);
# endif
		may_adjust_color_گنتی(val);
	    }
# اگر 0
#  ifdef FEAT_TERMGUICOLORS
	    // when RGB result comes back, it is supported when the result contains an '='
	    ورنہ اگر (name[0] == 'R' && name[1] == 'G' && name[2] == 'B' && رمز[9] == '=')
	    {
		صحیح val = atoi((حرف *)str);
		// only enable it, if termguicolors hasn't been set yet and
		// there are 8 bits per color channel
		اگر (val == 8 && !p_tgc_سیٹ)
		{
#   ifdef FEAT_EVAL
		    ch_log(تہی, "got_code_from_term(RGB): xterm-direct colors detected");
#   endif
		    // RGB capability set, enable termguicolors
		    سیٹ_option_قدر((char_u *)"termguicolors", 1L, تہی, 0);
		}
	    }
#  endif
# endif
	    ورنہ
	    {
		i = ڈھونڈو_رکن_bykeys(str, تہی);
		اگر (i >= 0 && name[0] == termcodes[i].name[0]
					    && name[1] == termcodes[i].name[1])
		{
		    // Existing entry with the same name and code - skip.
# ifdef FEAT_EVAL
		    ch_log(تہی, "got_code_from_term(): Entry %c%c did not change",
							     name[0], name[1]);
# endif
		}
		ورنہ اگر (i >= 0 && name[0] == 'K' && VIM_ISDIGIT(name[1]))
		{
		    // Would replace existing entry with keypad key - skip.
# ifdef FEAT_EVAL
		    ch_log(تہی, "got_code_from_term(): Skipping entry %c%c in favor of %c%c with matching keys %s",
			    name[0], name[1], termcodes[i].name[0], termcodes[i].name[1], str);
# endif
		}
		ورنہ
		{
		    اگر (i >= 0)
		    {
			// Delete an existing entry using the same code.
# ifdef FEAT_EVAL
			ch_log(تہی, "got_code_from_term(): Deleting entry %c%c with matching keys %s",
			      termcodes[i].name[0], termcodes[i].name[1], str);
# endif
			del_termcode_idx(i);
		    }
# ifdef FEAT_EVAL
		    ورنہ
			ch_log(تہی, "got_code_from_term(): Adding entry %c%c with keys %s",
							name[0], name[1], str);
# endif
		    جوڑو_termcode(name, str, ATC_FROM_رکن);
		}
	    }
	}
    }

    // May request more codes now that we received one.
    ++xt_اشاریہ_اندر;
    req_مزید_codes_from_رکن();
}

/*
 * Check if there are any unanswered requests and deal with them.
 * This is called before starting an external program or getting direct
 * keyboard input.  We don't want responses to be send to that program or
 * handled as typed text.
 */
    ساکن خالی
check_for_codes_from_رکن(خالی)
{
    صحیح		c;

    // If no codes requested or all are answered, no need to wait.
    اگر (xt_اشاریہ_برآمد == 0 || xt_اشاریہ_برآمد == xt_اشاریہ_اندر)
	واپس;

    // Vgetc() will check for and handle any response.
    // Keep calling vpeekc() until we don't get any responses.
    ++no_mapping;
    ++allow_کلیدیں;
    برائے (;;)
    {
	c = vpeekc();
	اگر (c == NUL)	    // nothing available
	    توڑو;

	// If a response is recognized it's replaced with K_IGNORE, must read
	// it from the input stream.  If there is no K_IGNORE we can't do
	// anything, break here (there might be some responses further on, but
	// we don't want to throw away any typed chars).
	اگر (c != K_SPECIAL && c != K_IGNORE)
	    توڑو;
	c = vgetc();
	اگر (c != K_IGNORE)
	{
	    vungetc(c);
	    توڑو;
	}
    }
    --no_mapping;
    --allow_کلیدیں;
}
#اگر_ختم

#اگر defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
ساکن حرف ksme_str[20];
ساکن حرف ksmr_str[20];
ساکن حرف ksmd_str[20];

/*
 * For Win32 console: update termcap codes for existing console attributes.
 */
    خالی
update_tcap(صحیح attr)
{
    sprintf(ksme_str, "\033|%dm", attr);
    sprintf(ksmd_str, "\033|%dm", attr | 0x08);  // FOREGROUND_INTENSITY
    sprintf(ksmr_str, "\033|%dm", ((attr & 0x0F) << 4) | ((attr & 0xF0) >> 4));

    tcap_اندراج_T *p = ڈھونڈو_builtin_رکن(DEFAULT_رکن);
    اگر (p == تہی)  // did not find it
	واپس;
    جب (p->bt_string != تہی)
    {
	اگر (p->bt_اندراج == (صحیح)KS_ME)
	    p->bt_string = &ksme_str[0];
	ورنہ اگر (p->bt_اندراج == (صحیح)KS_MR)
	    p->bt_string = &ksmr_str[0];
	ورنہ اگر (p->bt_اندراج == (صحیح)KS_MD)
	    p->bt_string = &ksmd_str[0];
	++p;
    }
}

# ifdef FEAT_TERMGUICOLORS
#  define KSSIZE 20

قسمتعریف شمار
{
    CMODE_اشاریہ_دار = 0,	// Use cmd.exe 4bit palette.
    CMODE_RGB,		// Use 24bit RGB colors using VTP.
    CMODE_256_ستون,	// Emulate xterm's 256-color palette using VTP.
    CMODE_آخری,
} cmode_T;

ساخت ks_tbl_S
{
    صحیح  رمز;				// value of KS_
    حرف *vtp;				// code in RGB mode
    حرف *vtp2;				// code in 256color mode
    حرف buf[CMODE_آخری][KSSIZE];	// real buffer
};

ساکن ساخت ks_tbl_S ks_tbl[] =
{
    {(صحیح)KS_ME,  "\033|0m",  "\033|0m", {""}},   //معمول
    {(صحیح)KS_MR,  "\033|7m",  "\033|7m", {""}},   //الٹ
    {(صحیح)KS_MD,  "\033|1m",  "\033|1m", {""}},   // bold
    {(صحیح)KS_SO,  "\033|91m", "\033|91m", {""}},  // standout: bright red text
    {(صحیح)KS_SE,  "\033|39m", "\033|39m", {""}},  // standout end: default color
    {(صحیح)KS_CZH, "\033|3m",  "\033|3m", {""}},   // italic
    {(صحیح)KS_CZR, "\033|0m",  "\033|0m", {""}},   // italic end
    {(صحیح)KS_US,  "\033|4m",  "\033|4m", {""}},   // underscore
    {(صحیح)KS_UE,  "\033|24m", "\033|24m", {""}},  // underscore end
#  ifdef TERMINFO
    {(صحیح)KS_CAB, "\033|%p1%db", "\033|%p14%dm", {""}}, // set background color
    {(صحیح)KS_CAF, "\033|%p1%df", "\033|%p13%dm", {""}}, // set foreground color
    {(صحیح)KS_CS,  "\033|%p1%d;%p2%dR", "\033|%p1%d;%p2%dR", {""}},
    {(صحیح)KS_CSV, "\033|%p1%d;%p2%dV", "\033|%p1%d;%p2%dV", {""}},
#  ورنہ
    {(صحیح)KS_CAB, "\033|%db", "\033|4%dm", {""}}, // set background color
    {(صحیح)KS_CAF, "\033|%df", "\033|3%dm", {""}}, // set foreground color
    {(صحیح)KS_CS,  "\033|%d;%dR", "\033|%d;%dR", {""}},
    {(صحیح)KS_CSV, "\033|%d;%dV", "\033|%d;%dV", {""}},
#  endif
    {(صحیح)KS_CCO, "256", "256", {""}},	    // colors
    {(صحیح)KS_NAME, تہی, تہی, {""}}			    // terminator
};

/*
 * Find the first entry for "code" in the builtin termcap for "name".
 * Returns NULL when not found.
 */
    ساکن tcap_اندراج_T *
ڈھونڈو_پہلا_tcap(
    char_u *name,
    صحیح	    رمز)
{
    tcap_اندراج_T *p = ڈھونڈو_builtin_رکن(name);
    اگر (p == تہی)
	واپس تہی;
    جب (p->bt_string != تہی)
    {
	اگر (p->bt_اندراج == رمز)
	    واپس p;
	++p;
    }
    واپس تہی;
}
# endif

/*
 * For Win32 console: replace the sequence immediately after termguicolors.
 */
    خالی
تبادلہ_tcap(خالی)
{
# ifdef FEAT_TERMGUICOLORS
    ساکن صحیح		init_مکمل_2 = FALSE;
    ساکن cmode_T	curr_منوال;
    ساخت ks_tbl_S	*ks;
    cmode_T		منوال;

    اگر (!init_مکمل_2)
    {
	برائے (ks = ks_tbl; ks->رمز != (صحیح)KS_NAME; ks++)
	{
	    tcap_اندراج_T *bt = ڈھونڈو_پہلا_tcap(DEFAULT_رکن, ks->رمز);
	    اگر (bt != تہی)
	    {
		// Preserve the original value.
		STRNCPY(ks->buf[CMODE_اشاریہ_دار], bt->bt_string, KSSIZE);
		STRNCPY(ks->buf[CMODE_RGB], ks->vtp, KSSIZE);
		STRNCPY(ks->buf[CMODE_256_ستون], ks->vtp2, KSSIZE);

		bt->bt_string = ks->buf[CMODE_اشاریہ_دار];
	    }
	}
	init_مکمل_2 = TRUE;
	curr_منوال = CMODE_اشاریہ_دار;
    }

    اگر (p_tgc)
	منوال = CMODE_RGB;
    ورنہ اگر (t_colors >= 256)
	منوال = CMODE_256_ستون;
    ورنہ
	منوال = CMODE_اشاریہ_دار;

    اگر (منوال == curr_منوال)
	واپس;

    برائے (ks = ks_tbl; ks->رمز != (صحیح)KS_NAME; ks++)
    {
	tcap_اندراج_T *bt = ڈھونڈو_پہلا_tcap(DEFAULT_رکن, ks->رمز);
	اگر (bt != تہی)
	    bt->bt_string = ks->buf[منوال];
    }

    curr_منوال = منوال;
# endif
}

#اگر_ختم


#اگر (defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))) || defined(FEAT_TERMINAL)
ساکن ثابت صحیح cube_قدر[] = {
    0x00, 0x5F, 0x87, 0xAF, 0xD7, 0xFF
};

ساکن ثابت صحیح grey_ramp[] = {
    0x08, 0x12, 0x1C, 0x26, 0x30, 0x3A, 0x44, 0x4E, 0x58, 0x62, 0x6C, 0x76,
    0x80, 0x8A, 0x94, 0x9E, 0xA8, 0xB2, 0xBC, 0xC6, 0xD0, 0xDA, 0xE4, 0xEE
};

ساکن ثابت char_u ansi_جدول[16][3] = {
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

# اگر defined(MSWIN)
// Mapping between cterm indices < 16 and their counterpart in the ANSI palette.
ثابت char_u cterm_ansi_idx[] = {
    0, 4, 2, 6, 1, 5, 3, 7, 8, 12, 10, 14, 9, 13, 11, 15
};
# endif

# define ANSI_اشاریہ_NONE 0

    خالی
ansi_color2rgb(صحیح nr, char_u *r, char_u *g, char_u *b, char_u *ansi_idx)
{
    اگر (nr < 16)
    {
	*r = ansi_جدول[nr][0];
	*g = ansi_جدول[nr][1];
	*b = ansi_جدول[nr][2];
	*ansi_idx = nr + 1;
    }
    ورنہ
    {
	*r = 0;
	*g = 0;
	*b = 0;
	*ansi_idx = ANSI_اشاریہ_NONE;
    }
}

    خالی
cterm_color2rgb(صحیح nr, char_u *r, char_u *g, char_u *b, char_u *ansi_idx)
{
    صحیح idx;

    اگر (nr < 16)
    {
# اگر defined(MSWIN)
	idx = cterm_ansi_idx[nr];
# ورنہ
	idx = nr;
# endif
	*r = ansi_جدول[idx][0];
	*g = ansi_جدول[idx][1];
	*b = ansi_جدول[idx][2];
	*ansi_idx = idx + 1;
    }
    ورنہ اگر (nr < 232)
    {
	// 216 color cube
	idx = nr - 16;
	*r = cube_قدر[idx / 36 % 6];
	*g = cube_قدر[idx / 6  % 6];
	*b = cube_قدر[idx      % 6];
	*ansi_idx = ANSI_اشاریہ_NONE;
    }
    ورنہ اگر (nr < 256)
    {
	// 24 grey scale ramp
	idx = nr - 232;
	*r = grey_ramp[idx];
	*g = grey_ramp[idx];
	*b = grey_ramp[idx];
	*ansi_idx = ANSI_اشاریہ_NONE;
    }
    ورنہ
    {
	*r = 0;
	*g = 0;
	*b = 0;
	*ansi_idx = ANSI_اشاریہ_NONE;
    }
}
#اگر_ختم

/*
 * Replace K_BS by <BS> and K_DEL by <DEL>.
 * Include any modifiers into the key and drop them.
 * Returns "len" adjusted for replaced codes.
 */
    صحیح
رکن_replace_keycodes(char_u *ta_buf, صحیح ta_len, صحیح len_arg)
{
    صحیح		len = len_arg;
    صحیح		i;
    صحیح		c;

    برائے (i = ta_len; i < ta_len + len; ++i)
    {
	اگر (ta_buf[i] == CSI && len - i > 3 && ta_buf[i + 1] == KS_MODIFIER)
	{
	    صحیح modifiers = ta_buf[i + 2];
	    صحیح کلید = ta_buf[i + 3];

	    // Try to use the modifier to modify the key.  In any case drop the
	    // modifier.
	    mch_memmove(ta_buf + i + 1, ta_buf + i + 4, (size_t)(len - i - 3));
	    len -= 3;
	    اگر (کلید < 0x80)
		کلید = ملاؤ_modify_Other_کلیدیں(کلید, &modifiers);
	    ta_buf[i] = کلید;
	}
	ورنہ اگر (ta_buf[i] == CSI && len - i > 2)
	{
	    c = TERMCAP2_کلید(ta_buf[i + 1], ta_buf[i + 2]);
	    اگر (c == K_DEL || c == K_KDEL || c == K_BS)
	    {
		mch_memmove(ta_buf + i + 1, ta_buf + i + 3,
							(size_t)(len - i - 2));
		اگر (c == K_DEL || c == K_KDEL)
		    ta_buf[i] = DEL;
		ورنہ
		    ta_buf[i] = Ctrl_H;
		len -= 2;
	    }
	}
	ورنہ اگر (ta_buf[i] == '\r')
	    ta_buf[i] = '\n';
	اگر (has_mbyte)
	    i += (*mb_ptr2len_len)(ta_buf + i, ta_len + len - i) - 1;
    }
    واپس len;
}

/*
 * Should be called when cleaning up terminal state.
 */
    خالی
رکن_disable_dec(خالی)
{
    رکن_سیٹ_sync_output(رکن_SYNC_OUTPUT_OFF);
#اگر_تعریف_شدہ UNIX
    رکن_سیٹ_win_resize(false);
#اگر_ختم
    // Make sure to always flush the output buffer, because this may be called
    // before starting the GUI
    برآمد_flush();
}

#اگر_تعریف_شدہ UNIX
/*
 * Enable or disable receiving in-band window resize events from the terminal.
 * If "state" is true, then if the terminal supports DEC mode 2048 and
 * 'termresize' is "" or "inband", then enable it and disable the SIGWINCH
 * signal handling. Otherwise disable the mode if it is enabled and reinstall
 * the SIGWINCH handler.
 */
    خالی
رکن_سیٹ_win_resize(bool state)
{
# ifdef FEAT_GUI
    bool    اندر_gui = gui.اندر_use;

    اگر (state && اندر_gui)
	واپس;
# endif

    اگر (!state || win_resize_setting == 0 || win_resize_setting == 4)
    {
	// Make sure it update internal window size if DEC mode 2048 is
	// unavailable now.
	اگر (win_resize_enabled)
	{
	    سیٹ_shellsize(0, 0, false);
	    سیٹ_sigwinch_handler();
	    برآمد_str((char_u *)"\033[?2048l");
	}
	win_resize_enabled = false;
    }
    ورنہ اگر ((*p_trz == NUL || STRCMP(p_trz, "inband") == 0)
	    && !win_resize_enabled)
    {
	اگر (win_resize_setting == 2)
	    برآمد_str((char_u *)"\033[?2048h");
# ifdef SIGWINCH
	mch_signal(SIGWINCH, SIG_DFL);
# endif
	win_resize_enabled = true;
    }
}
#اگر_ختم

    صحیح
sync_output_فعال(خالی)
{
#اگر_تعریف_شدہ FEAT_GUI
    اگر (gui.اندر_use)
	واپس TRUE;
#اگر_ختم
    واپس p_tsy && (sync_output_setting == 1 || sync_output_setting == 2)
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
    خالی
رکن_سیٹ_sync_output(صحیح flags)
{
    bool    allowed;
#اگر_تعریف_شدہ FEAT_GUI
    bool    اندر_gui = gui.اندر_use;
#ورنہ
    bool    اندر_gui = false;
#اگر_ختم

    allowed = p_tsy && (sync_output_setting == 1 || sync_output_setting == 2);

    اگر (flags & رکن_SYNC_OUTPUT_FLUSH)
    {
	// Tell terminal to display screen contents, then resume batching.
	اگر (allowed && !اندر_gui && sync_output_state > 0 && *T_ESU != NUL &&
		*T_BSU != NUL)
	{
	    برآمد_str_nf(T_ESU);
	    برآمد_str_nf(T_BSU);
	    برآمد_flush();
	}
	واپس;
    }

    // Forcibly turn off synchronized output (e.g. 'notermsync')
    اگر (flags & رکن_SYNC_OUTPUT_OFF)
    {
	اگر (sync_output_state > 0 && *T_ESU != NUL)
	{
	    برآمد_str_nf(T_ESU);
	    برآمد_flush();
	    sync_output_state = 0;
	}
	واپس;
    }

    اگر (!allowed || اندر_gui || *T_BSU == NUL || *T_ESU == NUL)
	واپس;

    // Only enable if we aren't already, and only disable if we have reached
    // zero.
    اگر (flags & رکن_SYNC_OUTPUT_ENABLE)
    {
	اگر (sync_output_state++ > 0)
	    واپس;
	برآمد_str_nf(T_BSU);
    }
    ورنہ اگر (flags & رکن_SYNC_OUTPUT_DISABLE)
    {
	اگر (sync_output_state == 0 || --sync_output_state > 0)
	    واپس;
	// Flush the output buffer before ending the sync batch so that
	// all drawing output is sent to the terminal within the
	// BSU..ESU window.  Without this, the drawing data remaining in
	// out_buf would be sent after ESU, outside the sync batch.
	برآمد_str_nf(T_ESU);
	برآمد_flush();
    }
    ورنہ
    {
	siemsg("Unknown sync output value %d", flags);
	واپس;
    }
}
