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
#تعریف_کن tgetstr tgetstr_defined_wrong

#ضمن "vim.h"

#اگر_تعریف_شده HAVE_TGETENT
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
# إذا defined(VMS) || defined(AMIGA)
#  define TPUTSFUNCAST (فراغ (*)(بیعلامت صحيح))
# وإلا
#  ifdef HAVE_OUTFUNTYPE
#   define TPUTSFUNCAST (outfuntype)
#  وإلا
#   define TPUTSFUNCAST (صحيح (*)(صحيح))
#  endif
# endif
#پایان_اگر

#لغو_تعریف tgetstr

// start of keys that are not directly used by Vim but can be mapped
#تعریف_کن BT_EXTRA_کلیدها	0x101

ساكن فراغ parse_builtin_tcap(char_u *s);
ساكن فراغ gather_termleader(فراغ);
#اگر_تعریف_شده FEAT_TERMRESPONSE
ساكن فراغ req_codes_from_جمله(فراغ);
ساكن فراغ req_بیشتر_codes_from_جمله(فراغ);
ساكن فراغ got_کد_from_جمله(char_u *کد, صحيح len);
ساكن فراغ check_for_codes_from_جمله(فراغ);
#پایان_اگر
ساكن فراغ del_termcode_idx(صحيح idx);
ساكن صحيح پیدا_کن_جمله_bykeys(char_u *src, صحيح *len);
ساكن صحيح جمله_is_builtin(char_u *name);
ساكن صحيح جمله_7to8bit(char_u *p);
ساكن فراغ accept_modifiers_for_function_کلیدها(فراغ);

#إذا 0  // Change to 1 to enable ch_log() calls for termresponse debugging.
# define DEBUG_TERMRESPONSE
# define LOG_TR1(str) \
		ch_log(عدم, "TermResp: %s " str, \
			must_redraw == UPD_NOT_VALID ? "NV" \
			: must_redraw == UPD_CLEAR ? "CL" : "  ")
# define LOG_TRN(fmt,...) \
		ch_log(عدم, "TermResp: %s " fmt, \
			must_redraw == UPD_NOT_VALID ? "NV" \
			: must_redraw == UPD_CLEAR ? "CL" : "  ", __VA_ARGS__)
#وإلا
# define LOG_TR1(str) افعل { /**/ } طالما (0)
# define LOG_TRN(fmt,...) افعل { /**/ } طالما (0)
#پایان_اگر

#اگر_تعریف_شده HAVE_TGETENT
ساكن حرف *invoke_tgetent(char_u *, char_u *);

/*
 * Here is our own prototype for tgetstr(), any prototypes from the include
 * files have been disabled by the define at the start of this file.
 */
حرف		*tgetstr(حرف *, حرف **);
#پایان_اگر

تعريفنوع تعداد {
    وضعیت_GET,		// send request when switching to RAW mode
    وضعیت_فرستاده,	// did send request, checking for response
    وضعیت_GOT,		// received response
    وضعیت_FAIL		// timed out
} request_پیشرفت_T;

تعريفنوع بنية {
    request_پیشرفت_T	    tr_پیشرفت;
    time_t		    tr_شروع;	// when request was sent, -1 for never
} termrequest_T;

// Holds state for current OSC response.
تعريفنوع بنية
{
    صحيح		    processing;	// If we are in the middle of an OSC response
    char_u	    شروع_char;	// First char in the OSC response
    garray_T	    buf;	// Buffer holding the OSC response, to be
				// placed in the "v:termosc" vim var.

#اگر_تعریف_شده ELAPSED_FUNC
    elapsed_T	    شروع_tv;	// Set at the beginning of an OSC response.
				// Used to timeout after a set amount of
				// time.
#پایان_اگر
} oscstate_T;

#تعریف_کن TERMREQUEST_INIT {وضعیت_GET, -1}

// Request Terminal Version status:
ساكن termrequest_T crv_وضعیت = TERMREQUEST_INIT;

// Request Cursor position report:
ساكن termrequest_T u7_وضعیت = TERMREQUEST_INIT;

// Request xterm compatibility check:
ساكن termrequest_T xcc_وضعیت = TERMREQUEST_INIT;

#اگر_تعریف_شده FEAT_TERMRESPONSE
# ifdef FEAT_TERMINAL
// Request foreground color report:
ساكن termrequest_T rfg_وضعیت = TERMREQUEST_INIT;
ساكن صحيح fg_r = 0;
ساكن صحيح fg_g = 0;
ساكن صحيح fg_b = 0;
# endif
// Background color values from the OSC 11 response, also used by the
// image backend to flatten RGBA alpha onto the actual terminal background.
ساكن صحيح bg_r = 255;
ساكن صحيح bg_g = 255;
ساكن صحيح bg_b = 255;

// Request background color report:
ساكن termrequest_T rbg_وضعیت = TERMREQUEST_INIT;

// Request cursor blinking mode report:
ساكن termrequest_T rbm_وضعیت = TERMREQUEST_INIT;

// Request cursor style report:
ساكن termrequest_T rcs_وضعیت = TERMREQUEST_INIT;

// Request window's position report:
ساكن termrequest_T winpos_وضعیت = TERMREQUEST_INIT;

// Request DECRQM (DEC mode) report:
ساكن termrequest_T decrqm_وضعیت = TERMREQUEST_INIT;

ساكن termrequest_T *all_termrequests[] = {
    &crv_وضعیت,
    &u7_وضعیت,
    &xcc_وضعیت,
# ifdef FEAT_TERMINAL
    &rfg_وضعیت,
# endif
    &rbg_وضعیت,
    &rbm_وضعیت,
    &rcs_وضعیت,
    &winpos_وضعیت,
    &decrqm_وضعیت,
    عدم
};

// The t_8u code may default to a value but get reset when the term response is
// received.  To avoid redrawing too often, only redraw when t_8u is not reset
// and it was supposed to be written.  Unless t_8u was set explicitly.
// FALSE -> don't output t_8u yet
// MAYBE -> tried outputting t_8u while FALSE
// OK    -> can write t_8u
صحيح write_t_8u_state = FALSE;
#پایان_اگر

#اگر_تعریف_شده HAVE_TGETENT
/*
 * Don't declare these variables if termcap.h contains them.
 * Autoconf checks if these variables should be declared extern (not all
 * systems have them).
 * Some versions define ospeed to be speed_t, but that is incompatible with
 * BSD, where ospeed is short and speed_t is long.
 */
# ifndef HAVE_OSPEED
#  ifdef OSPEED_EXTERN
خارجي کوتاه ospeed;
#  وإلا
کوتاه ospeed;
#  endif
# endif
# ifndef HAVE_به_بالا_BC_PC
#  ifdef به_بالا_BC_PC_EXTERN
خارجي حرف *به_بالا_2, *BC, PC;
#  وإلا
حرف *به_بالا_2, *BC, PC;
#  endif
# endif

# define TGETSTR(s, p)	vim_tgetstr((s), (p))
# define TGETENT(b, t)	tgetent((حرف *)(b), (حرف *)(t))
ساكن char_u *vim_tgetstr(حرف *s, char_u **pp);
#پایان_اگر // HAVE_TGETENT

ساكن صحيح  detected_8bit = FALSE;	// detected 8-bit terminal

#إذا (defined(UNIX) || defined(VMS))
ساكن صحيح focus_state = MAYBE; // TRUE if the Vim window has focus
#پایان_اگر

#اگر_تعریف_شده FEAT_TERMRESPONSE
// When the cursor shape was detected these values are used:
// 1: block, 2: underline, 3: vertical bar
ساكن صحيح initial_cursor_shape = 0;

// The blink flag from the style response may be inverted from the actual
// blinking state, xterm XORs the flags.
ساكن صحيح initial_cursor_shape_blink = FALSE;

// The blink flag from the blinking-cursor mode response
ساكن صحيح initial_cursor_blink = FALSE;
#پایان_اگر

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
ساكن صحيح sync_output_setting = 0;

// > 0: Currently batching output
// == 0: No synchronized output
ساكن صحيح sync_output_state = 0;

#اگر_تعریف_شده UNIX
// DEC mode 2048 (in-band window resize events)
// https://gist.github.com/rockorager/e695fb2924d36b2bcf1fff4a3704bd83
ساكن صحيح win_resize_setting = 0;
ساكن bool win_resize_enabled = false;
#پایان_اگر

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
تعريفنوع بنية
{
    صحيح		bt_مدخل;   // either a KS_xxx code (>= 0), or a K_xxx code.
    حرف	*bt_string; //مقدار
} tcap_مدخل_T;

/*
 * Standard ANSI terminal, default for Unix.
 */
ساكن tcap_مدخل_T builtin_ansi[] = {
    {(صحيح)KS_CE,	"\033[K"},
    {(صحيح)KS_AL,	"\033[L"},
#اگر_تعریف_شده TERMINFO
    {(صحيح)KS_CAL,	"\033[%p1%dL"},
#وإلا
    {(صحيح)KS_CAL,	"\033[%dL"},
#پایان_اگر
    {(صحيح)KS_DL,	"\033[M"},
#اگر_تعریف_شده TERMINFO
    {(صحيح)KS_CDL,	"\033[%p1%dM"},
#وإلا
    {(صحيح)KS_CDL,	"\033[%dM"},
#پایان_اگر
    {(صحيح)KS_CL,	"\033[H\033[2J"},
    {(صحيح)KS_ME,	"\033[0m"},
    {(صحيح)KS_MR,	"\033[7m"},
    {(صحيح)KS_MS,	"y"},
    {(صحيح)KS_UT,	"y"},		// guessed
    {(صحيح)KS_LE,	"\b"},
#اگر_تعریف_شده TERMINFO
    {(صحيح)KS_CM,	"\033[%i%p1%d;%p2%dH"},
#وإلا
    {(صحيح)KS_CM,	"\033[%i%d;%dH"},
#پایان_اگر
#اگر_تعریف_شده TERMINFO
    {(صحيح)KS_CRI,	"\033[%p1%dC"},
#وإلا
    {(صحيح)KS_CRI,	"\033[%dC"},
#پایان_اگر

    {(صحيح)KS_NAME,	عدم}  // end marker
};

/*
 * VT320 is working as an ANSI terminal compatible DEC terminal.
 * (it covers VT1x0, VT2x0 and VT3x0 up to VT320 on VMS as well)
 * TODO:- rewrite ESC[ codes to CSI
 *      - keyboard languages (CSI ? 26 n)
 */
ساكن tcap_مدخل_T builtin_vt320[] = {
    {(صحيح)KS_CE,	"\033[K"},
    {(صحيح)KS_AL,	"\033[L"},
#اگر_تعریف_شده TERMINFO
    {(صحيح)KS_CAL,	"\033[%p1%dL"},
#وإلا
    {(صحيح)KS_CAL,	"\033[%dL"},
#پایان_اگر
    {(صحيح)KS_DL,	"\033[M"},
#اگر_تعریف_شده TERMINFO
    {(صحيح)KS_CDL,	"\033[%p1%dM"},
#وإلا
    {(صحيح)KS_CDL,	"\033[%dM"},
#پایان_اگر
    {(صحيح)KS_CL,	"\033[H\033[2J"},
    {(صحيح)KS_CD,	"\033[J"},
    {(صحيح)KS_CCO,	"8"},			// allow 8 colors
    {(صحيح)KS_ME,	"\033[0m"},
    {(صحيح)KS_MR,	"\033[7m"},
    {(صحيح)KS_MD,	"\033[1m"},  // bold mode
    {(صحيح)KS_SE,	"\033[22m"},// normal mode
    {(صحيح)KS_UE,	"\033[24m"},// exit underscore mode
    {(صحيح)KS_US,	"\033[4m"},  // underscore mode
    {(صحيح)KS_CZH,	"\033[34;43m"},  // italic mode: blue text on yellow
    {(صحيح)KS_CZR,	"\033[0m"},	    // italic mode end
    {(صحيح)KS_CAB,	"\033[4%dm"},	    // set background color (ANSI)
    {(صحيح)KS_CAF,	"\033[3%dm"},	    // set foreground color (ANSI)
    {(صحيح)KS_CSB,	"\033[102;%dm"},	// set screen background color
    {(صحيح)KS_CSF,	"\033[101;%dm"},	// set screen foreground color
    {(صحيح)KS_MS,	"y"},
    {(صحيح)KS_UT,	"y"},
    {(صحيح)KS_XN,	"y"},
    {(صحيح)KS_LE,	"\b"},
#اگر_تعریف_شده TERMINFO
    {(صحيح)KS_CM,	"\033[%i%p1%d;%p2%dH"},
#وإلا
    {(صحيح)KS_CM,	"\033[%i%d;%dH"},
#پایان_اگر
#اگر_تعریف_شده TERMINFO
    {(صحيح)KS_CRI,	"\033[%p1%dC"},
#وإلا
    {(صحيح)KS_CRI,	"\033[%dC"},
#پایان_اگر
    {K_به_بالا,		"\033[A"},
    {K_پایین,		"\033[B"},
    {K_راست,		"\033[C"},
    {K_چپ,		"\033[D"},
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
    {K_انتها,		"\033[4~"},
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

    {(صحيح)KS_NAME,	عدم}  // end marker
};

/*
 * Ordinary vt52
 */
ساكن tcap_مدخل_T builtin_vt52[] = {
    {(صحيح)KS_CE,	"\033K"},
    {(صحيح)KS_CD,	"\033J"},
#اگر_تعریف_شده TERMINFO
    {(صحيح)KS_CM,	"\033Y%p1%' '%+%c%p2%' '%+%c"},
#وإلا
    {(صحيح)KS_CM,	"\033Y%+ %+ "},
#پایان_اگر
    {(صحيح)KS_LE,	"\b"},
    {(صحيح)KS_SR,	"\033I"},
    {(صحيح)KS_AL,	"\033L"},
    {(صحيح)KS_DL,	"\033M"},
    {K_به_بالا,		"\033A"},
    {K_پایین,		"\033B"},
    {K_چپ,		"\033D"},
    {K_راست,		"\033C"},
    {K_F1,		"\033P"},
    {K_F2,		"\033Q"},
    {K_F3,		"\033R"},
    {(صحيح)KS_CL,	"\033H\033J"},
    {(صحيح)KS_MS,	"y"},

    {(صحيح)KS_NAME,	عدم}  // end marker
};

/*
 * Builtin xterm with Vim-specific entries.
 */
ساكن tcap_مدخل_T builtin_xterm[] = {
    {(صحيح)KS_CE,	"\033[K"},
    {(صحيح)KS_AL,	"\033[L"},
#اگر_تعریف_شده TERMINFO
    {(صحيح)KS_CAL,	"\033[%p1%dL"},
#وإلا
    {(صحيح)KS_CAL,	"\033[%dL"},
#پایان_اگر
    {(صحيح)KS_DL,	"\033[M"},
#اگر_تعریف_شده TERMINFO
    {(صحيح)KS_CDL,	"\033[%p1%dM"},
#وإلا
    {(صحيح)KS_CDL,	"\033[%dM"},
#پایان_اگر
#اگر_تعریف_شده TERMINFO
    {(صحيح)KS_CS,	"\033[%i%p1%d;%p2%dr"},
#وإلا
    {(صحيح)KS_CS,	"\033[%i%d;%dr"},
#پایان_اگر
    {(صحيح)KS_CL,	"\033[H\033[2J"},
    {(صحيح)KS_CD,	"\033[J"},
    {(صحيح)KS_ME,	"\033[m"},
    {(صحيح)KS_MR,	"\033[7m"},
    {(صحيح)KS_MD,	"\033[1m"},
    {(صحيح)KS_UE,	"\033[m"},
    {(صحيح)KS_US,	"\033[4m"},
    {(صحيح)KS_STE,	"\033[29m"},
    {(صحيح)KS_STS,	"\033[9m"},
    {(صحيح)KS_MS,	"y"},
    {(صحيح)KS_UT,	"y"},
    {(صحيح)KS_LE,	"\b"},
    {(صحيح)KS_VI,	"\033[?25l"},
    {(صحيح)KS_VE,	"\033[?25h"},
    {(صحيح)KS_VS,	"\033[?12h"},
    {(صحيح)KS_CVS,	"\033[?12l"},
#اگر_تعریف_شده TERMINFO
    {(صحيح)KS_CSH,	"\033[%p1%d q"},
#وإلا
    {(صحيح)KS_CSH,	"\033[%d q"},
#پایان_اگر
    {(صحيح)KS_بررسی_چرخهای,	"\033[?12$p"},
    {(صحيح)KS_CRS,	"\033P$q q\033\\"},
#اگر_تعریف_شده TERMINFO
    {(صحيح)KS_CM,	"\033[%i%p1%d;%p2%dH"},
#وإلا
    {(صحيح)KS_CM,	"\033[%i%d;%dH"},
#پایان_اگر
    {(صحيح)KS_SR,	"\033M"},
#اگر_تعریف_شده TERMINFO
    {(صحيح)KS_CRI,	"\033[%p1%dC"},
#وإلا
    {(صحيح)KS_CRI,	"\033[%dC"},
#پایان_اگر
    {(صحيح)KS_KS,	"\033[?1h\033="},
    {(صحيح)KS_KE,	"\033[?1l\033>"},
#اگر_تعریف_شده FEAT_XTERM_SAVE
    {(صحيح)KS_TI,	"\0337\033[?47h"},
    {(صحيح)KS_TE,	"\033[?47l\0338"},
#پایان_اگر
    // These are now under control of the 'keyprotocol' option, see
    // "builtin_mok2".
    // {(int)KS_CTI,	"\033[>4;2m"},
    // {(int)KS_CRK,	"\033[?4m"},
    // {(int)KS_CTE,	"\033[>4;m"},
    {(صحيح)KS_CIS,	"\033]1;"},
    {(صحيح)KS_CIE,	"\007"},
    {(صحيح)KS_TS,	"\033]2;"},
    {(صحيح)KS_FS,	"\007"},
    {(صحيح)KS_CSC,	"\033]12;"},
    {(صحيح)KS_CEC,	"\007"},
#اگر_تعریف_شده TERMINFO
    {(صحيح)KS_CWS,	"\033[8;%p1%d;%p2%dt"},
    {(صحيح)KS_CWP,	"\033[3;%p1%d;%p2%dt"},
    {(صحيح)KS_CGP,	"\033[13t"},
#وإلا
    {(صحيح)KS_CWS,	"\033[8;%d;%dt"},
    {(صحيح)KS_CWP,	"\033[3;%d;%dt"},
    {(صحيح)KS_CGP,	"\033[13t"},
#پایان_اگر
    {(صحيح)KS_CRV,	"\033[>c"},
    {(صحيح)KS_CXM,	"\033[?1006;1000%?%p1%{1}%=%th%el%;"},
    {(صحيح)KS_RFG,	"\033]10;?\007"},
    {(صحيح)KS_RBG,	"\033]11;?\007"},
    {(صحيح)KS_U7,	"\033[6n"},
    {(صحيح)KS_CAU,	"\033[58;5;%dm"},
    {(صحيح)KS_CBE,	"\033[?2004h"},
    {(صحيح)KS_CBD,	"\033[?2004l"},
    {(صحيح)KS_CST,	"\033[22;2t"},
    {(صحيح)KS_CRT,	"\033[23;2t"},
    {(صحيح)KS_SSI,	"\033[22;1t"},
    {(صحيح)KS_SRI,	"\033[23;1t"},
#إذا (defined(UNIX) || defined(VMS))
    {(صحيح)KS_FD,	"\033[?1004l"},
    {(صحيح)KS_FE,	"\033[?1004h"},
#پایان_اگر

    {K_به_بالا,		"\033O*A"},
    {K_پایین,		"\033O*B"},
    {K_راست,		"\033O*C"},
    {K_چپ,		"\033O*D"},
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
    {K_انتها,		"\033[@;*F"},	// Esc [ F or Esc 1 ; F
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

    {BT_EXTRA_کلیدها,   ""},
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

    {(صحيح)KS_NAME,	عدم}  // end marker
};

/*
 * Additions for using modifyOtherKeys level 2.  Same as what is used for
 * xterm.
 */
ساكن tcap_مدخل_T builtin_mok2[] = {
    // t_TI enables modifyOtherKeys level 2
    {(صحيح)KS_CTI,	"\033[>4;2m"},

    // XTQMODKEYS was added in xterm version 377: "CSI ? 4 m" which should
    // return "{lead} > 4 ; Pv m".  Before version 377 we expect it to have no
    // effect.
    {(صحيح)KS_CRK,	"\033[?4m"},

    // t_TE disables modifyOtherKeys
    {(صحيح)KS_CTE,	"\033[>4;m"},

    {(صحيح)KS_NAME,	عدم}  // end marker
};

/*
 * Additions for using the Kitty keyboard protocol.
 */
ساكن tcap_مدخل_T builtin_kitty[] = {
    // t_TI enables the kitty keyboard protocol.
    {(صحيح)KS_CTI,	"\033[=1;1u"},

    // t_RK requests the kitty keyboard protocol state
    {(صحيح)KS_CRK,	"\033[?u"},

    // t_TE also disables modifyOtherKeys, because t_TI from xterm may already
    // have been used.
    {(صحيح)KS_CTE,	"\033[>4;m\033[=0;1u"},

    // t_RF request terminal foreground color
    {(صحيح)KS_RFG,	"\033]10;?\033\\"},

    // t_RB request terminal background color
    {(صحيح)KS_RBG,	"\033]11;?\033\\"},

    {(صحيح)KS_NAME,	عدم}  // end marker
};

#اگر_تعریف_شده HAVE_TGETENT
/*
 * Additions for enabling/disabling synchronized output mode for terminal.
 */
ساكن tcap_مدخل_T builtin_sync_output[] = {
    {(صحيح)KS_BSU,	"\033[?2026h"},
    {(صحيح)KS_ESU,	"\033[?2026l"},
    {(صحيح)KS_NAME,	عدم}  // end marker
};
#پایان_اگر

#اگر_تعریف_شده FEAT_TERMRESPONSE
/*
 * List of DECRQM modes that Vim supports
 */
ساكن ثابت صحيح dec_modes[] = {
    2026,   // Synchronized output
# ifdef UNIX
    2048    // In-band terminal resize events
# endif
};
#پایان_اگر

#اگر_تعریف_شده FEAT_TERMGUICOLORS
/*
 * Additions for using the RGB colors and terminal font
 */
ساكن tcap_مدخل_T builtin_rgb[] = {
    // These are printf strings, not terminal codes.
    {(صحيح)KS_8F,	"\033[38;2;%lu;%lu;%lum"},
    {(صحيح)KS_8B,	"\033[48;2;%lu;%lu;%lum"},
    {(صحيح)KS_8U,	"\033[58;2;%lu;%lu;%lum"},

    {(صحيح)KS_NAME,	عدم}  // end marker
};
#پایان_اگر

#اگر_تعریف_شده HAVE_TGETENT
ساكن tcap_مدخل_T special_جمله[] = {
    // These are printf strings, not terminal codes.
    {(صحيح)KS_CF,	"\033[%dm"},
    {(صحيح)KS_NAME,	عدم}  // end marker
};
#پایان_اگر

/*
 * iris-ansi for Silicon Graphics machines.
 */
ساكن tcap_مدخل_T builtin_iris_ansi[] = {
    {(صحيح)KS_CE,	"\033[K"},
    {(صحيح)KS_CD,	"\033[J"},
    {(صحيح)KS_AL,	"\033[L"},
#اگر_تعریف_شده TERMINFO
    {(صحيح)KS_CAL,	"\033[%p1%dL"},
#وإلا
    {(صحيح)KS_CAL,	"\033[%dL"},
#پایان_اگر
    {(صحيح)KS_DL,	"\033[M"},
#اگر_تعریف_شده TERMINFO
    {(صحيح)KS_CDL,	"\033[%p1%dM"},
#وإلا
    {(صحيح)KS_CDL,	"\033[%dM"},
#پایان_اگر
#إذا 0	// The scroll region is not working as Vim expects.
# ifdef TERMINFO
    {(صحيح)KS_CS,	"\033[%i%p1%d;%p2%dr"},
# وإلا
    {(صحيح)KS_CS,	"\033[%i%d;%dr"},
# endif
#پایان_اگر
    {(صحيح)KS_CL,	"\033[H\033[2J"},
    {(صحيح)KS_VE,	"\033[9/y\033[12/y"},	// These aren't documented
    {(صحيح)KS_VS,	"\033[10/y\033[=1h\033[=2l"}, // These aren't documented
    {(صحيح)KS_TI,	"\033[=6h"},
    {(صحيح)KS_TE,	"\033[=6l"},
    {(صحيح)KS_SE,	"\033[21;27m"},
    {(صحيح)KS_SO,	"\033[1;7m"},
    {(صحيح)KS_ME,	"\033[m"},
    {(صحيح)KS_MR,	"\033[7m"},
    {(صحيح)KS_MD,	"\033[1m"},
    {(صحيح)KS_CCO,	"8"},			// allow 8 colors
    {(صحيح)KS_CZH,	"\033[3m"},		// italic mode on
    {(صحيح)KS_CZR,	"\033[23m"},		// italic mode off
    {(صحيح)KS_US,	"\033[4m"},		// underline on
    {(صحيح)KS_UE,	"\033[24m"},		// underline off
#اگر_تعریف_شده TERMINFO
    {(صحيح)KS_CAB,	"\033[4%p1%dm"},    // set background color (ANSI)
    {(صحيح)KS_CAF,	"\033[3%p1%dm"},    // set foreground color (ANSI)
    {(صحيح)KS_CSB,	"\033[102;%p1%dm"}, // set screen background color
    {(صحيح)KS_CSF,	"\033[101;%p1%dm"}, // set screen foreground color
#وإلا
    {(صحيح)KS_CAB,	"\033[4%dm"},	    // set background color (ANSI)
    {(صحيح)KS_CAF,	"\033[3%dm"},	    // set foreground color (ANSI)
    {(صحيح)KS_CSB,	"\033[102;%dm"},    // set screen background color
    {(صحيح)KS_CSF,	"\033[101;%dm"},    // set screen foreground color
#پایان_اگر
    {(صحيح)KS_MS,	"y"},		// guessed
    {(صحيح)KS_UT,	"y"},		// guessed
    {(صحيح)KS_LE,	"\b"},
#اگر_تعریف_شده TERMINFO
    {(صحيح)KS_CM,	"\033[%i%p1%d;%p2%dH"},
#وإلا
    {(صحيح)KS_CM,	"\033[%i%d;%dH"},
#پایان_اگر
    {(صحيح)KS_SR,	"\033M"},
#اگر_تعریف_شده TERMINFO
    {(صحيح)KS_CRI,	"\033[%p1%dC"},
#وإلا
    {(صحيح)KS_CRI,	"\033[%dC"},
#پایان_اگر
    {(صحيح)KS_CIS,	"\033P3.y"},
    {(صحيح)KS_CIE,	"\234"},    // ST "String Terminator"
    {(صحيح)KS_TS,	"\033P1.y"},
    {(صحيح)KS_FS,	"\234"},    // ST "String Terminator"
#اگر_تعریف_شده TERMINFO
    {(صحيح)KS_CWS,	"\033[203;%p1%d;%p2%d/y"},
    {(صحيح)KS_CWP,	"\033[205;%p1%d;%p2%d/y"},
#وإلا
    {(صحيح)KS_CWS,	"\033[203;%d;%d/y"},
    {(صحيح)KS_CWP,	"\033[205;%d;%d/y"},
#پایان_اگر
    {K_به_بالا,		"\033[A"},
    {K_پایین,		"\033[B"},
    {K_چپ,		"\033[D"},
    {K_راست,		"\033[C"},
    {K_S_به_بالا,		"\033[161q"},
    {K_S_پایین,		"\033[164q"},
    {K_S_چپ,		"\033[158q"},
    {K_S_راست,		"\033[167q"},
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
    {K_انتها,		"\033[146q"},
    {K_PAGEUP,		"\033[150q"},
    {K_PAGEDOWN,	"\033[154q"},

    {(صحيح)KS_NAME,	عدم}  // end marker
};

/*
 * These codes are valid when nansi.sys or equivalent has been installed.
 * Function keys on a PC are preceded with a NUL. These are converted into
 * K_NUL '\316' in mch_inchar(), because we cannot handle NULs in key codes.
 * CTRL-arrow is used instead of SHIFT-arrow.
 */
ساكن tcap_مدخل_T builtin_pcansi[] = {
    {(صحيح)KS_DL,	"\033[M"},
    {(صحيح)KS_AL,	"\033[L"},
    {(صحيح)KS_CE,	"\033[K"},
    {(صحيح)KS_CL,	"\033[2J"},
    {(صحيح)KS_ME,	"\033[0m"},
    {(صحيح)KS_MR,	"\033[5m"},	// reverse: black on lightgrey
    {(صحيح)KS_MD,	"\033[1m"},	// bold: white text
    {(صحيح)KS_SE,	"\033[0m"},	// standout end
    {(صحيح)KS_SO,	"\033[31m"},	// standout: white on blue
    {(صحيح)KS_CZH,	"\033[34;43m"},	// italic mode: blue text on yellow
    {(صحيح)KS_CZR,	"\033[0m"},	// italic mode end
    {(صحيح)KS_US,	"\033[36;41m"},	// underscore mode: cyan text on red
    {(صحيح)KS_UE,	"\033[0m"},	// underscore mode end
    {(صحيح)KS_CCO,	"8"},		// allow 8 colors
#اگر_تعریف_شده TERMINFO
    {(صحيح)KS_CAB,	"\033[4%p1%dm"},// set background color
    {(صحيح)KS_CAF,	"\033[3%p1%dm"},// set foreground color
#وإلا
    {(صحيح)KS_CAB,	"\033[4%dm"},	// set background color
    {(صحيح)KS_CAF,	"\033[3%dm"},	// set foreground color
#پایان_اگر
    {(صحيح)KS_OP,	"\033[0m"},	// reset colors
    {(صحيح)KS_MS,	"y"},
    {(صحيح)KS_UT,	"y"},		// guessed
    {(صحيح)KS_LE,	"\b"},
#اگر_تعریف_شده TERMINFO
    {(صحيح)KS_CM,	"\033[%i%p1%d;%p2%dH"},
#وإلا
    {(صحيح)KS_CM,	"\033[%i%d;%dH"},
#پایان_اگر
#اگر_تعریف_شده TERMINFO
    {(صحيح)KS_CRI,	"\033[%p1%dC"},
#وإلا
    {(صحيح)KS_CRI,	"\033[%dC"},
#پایان_اگر
    {K_به_بالا,		"\316H"},
    {K_پایین,		"\316P"},
    {K_چپ,		"\316K"},
    {K_راست,		"\316M"},
    {K_S_چپ,		"\316s"},
    {K_S_راست,		"\316t"},
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
    {K_انتها,		"\316O"},
    {K_PAGEDOWN,	"\316Q"},
    {K_PAGEUP,		"\316I"},

    {(صحيح)KS_NAME,	عدم}  // end marker
};

/*
 * These codes are valid for the Win32 Console.  The entries that start with
 * ESC | are translated into console calls in os_win32.c.  The function keys
 * are also translated in os_win32.c.
 */
ساكن tcap_مدخل_T builtin_win32[] = {
    {(صحيح)KS_CE,	"\033|K"},	// clear to end of line
    {(صحيح)KS_AL,	"\033|L"},	// add new blank line
#اگر_تعریف_شده TERMINFO
    {(صحيح)KS_CAL,	"\033|%p1%dL"},	// add number of new blank lines
#وإلا
    {(صحيح)KS_CAL,	"\033|%dL"},	// add number of new blank lines
#پایان_اگر
    {(صحيح)KS_DL,	"\033|M"},	// delete line
#اگر_تعریف_شده TERMINFO
    {(صحيح)KS_CDL,	"\033|%p1%dM"},	// delete number of lines
    {(صحيح)KS_CSV,	"\033|%p1%d;%p2%dV"},
#وإلا
    {(صحيح)KS_CDL,	"\033|%dM"},	// delete number of lines
    {(صحيح)KS_CSV,	"\033|%d;%dV"},
#پایان_اگر
    {(صحيح)KS_CL,	"\033|J"},	// clear screen
    {(صحيح)KS_CD,	"\033|j"},	// clear to end of display
    {(صحيح)KS_VI,	"\033|v"},	// cursor invisible
    {(صحيح)KS_VE,	"\033|V"},	// cursor visible

    {(صحيح)KS_ME,	"\033|0m"},	//نرمال
    {(صحيح)KS_MR,	"\033|112m"},	// reverse: black on lightgray
    {(صحيح)KS_MD,	"\033|15m"},	// bold: white on black
#إذا 1
    {(صحيح)KS_SO,	"\033|31m"},	// standout: white on blue
    {(صحيح)KS_SE,	"\033|0m"},	// standout end
#وإلا
    {(صحيح)KS_SO,	"\033|F"},	// standout: high intensity
    {(صحيح)KS_SE,	"\033|f"},	// standout end
#پایان_اگر
    {(صحيح)KS_CZH,	"\033|225m"},	// italic: blue text on yellow
    {(صحيح)KS_CZR,	"\033|0m"},	// italic end
    {(صحيح)KS_US,	"\033|67m"},	// underscore: cyan text on red
    {(صحيح)KS_UE,	"\033|0m"},	// underscore end
    {(صحيح)KS_CCO,	"16"},		// allow 16 colors
#اگر_تعریف_شده TERMINFO
    {(صحيح)KS_CAB,	"\033|%p1%db"},	// set background color
    {(صحيح)KS_CAF,	"\033|%p1%df"},	// set foreground color
#وإلا
    {(صحيح)KS_CAB,	"\033|%db"},	// set background color
    {(صحيح)KS_CAF,	"\033|%df"},	// set foreground color
#پایان_اگر

    {(صحيح)KS_MS,	"y"},		// save to move cur in reverse mode
    {(صحيح)KS_UT,	"y"},
    {(صحيح)KS_XN,	"y"},
    {(صحيح)KS_LE,	"\b"},
#اگر_تعریف_شده TERMINFO
    {(صحيح)KS_CM,	"\033|%i%p1%d;%p2%dH"}, // cursor motion
#وإلا
    {(صحيح)KS_CM,	"\033|%i%d;%dH"}, // cursor motion
#پایان_اگر
    {(صحيح)KS_VB,	"\033|B"},	// visual bell
    {(صحيح)KS_TI,	"\033|S"},	// put terminal in termcap mode
    {(صحيح)KS_TE,	"\033|E"},	// out of termcap mode
#اگر_تعریف_شده TERMINFO
    {(صحيح)KS_CS,	"\033|%i%p1%d;%p2%dr"}, // scroll region
#وإلا
    {(صحيح)KS_CS,	"\033|%i%d;%dr"}, // scroll region
#پایان_اگر

    {K_به_بالا,		"\316H"},
    {K_پایین,		"\316P"},
    {K_چپ,		"\316K"},
    {K_راست,		"\316M"},
    {K_S_به_بالا,		"\316\304"},
    {K_S_پایین,		"\316\317"},
    {K_S_چپ,		"\316\311"},
    {K_C_چپ,		"\316s"},
    {K_S_راست,		"\316\313"},
    {K_C_راست,		"\316t"},
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
    {K_انتها,		"\316O"},
    {K_S_انتها,		"\316\315"},
    {K_C_انتها,		"\316u"},
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

    {(صحيح)KS_NAME,	عدم}  // end marker
};

#إذا defined(FEAT_GUI)
/*
 * GUI uses made-up codes, only used inside Vim.
 */
ساكن tcap_مدخل_T builtin_gui[] = {
    {(صحيح)KS_CE,	"\033|$"},
    {(صحيح)KS_AL,	"\033|i"},
# ifdef TERMINFO
    {(صحيح)KS_CAL,	"\033|%p1%dI"},
# وإلا
    {(صحيح)KS_CAL,	"\033|%dI"},
# endif
    {(صحيح)KS_DL,	"\033|d"},
# ifdef TERMINFO
    {(صحيح)KS_CDL,	"\033|%p1%dD"},
    {(صحيح)KS_CS,	"\033|%p1%d;%p2%dR"},
    {(صحيح)KS_CSV,	"\033|%p1%d;%p2%dV"},
# وإلا
    {(صحيح)KS_CDL,	"\033|%dD"},
    {(صحيح)KS_CS,	"\033|%d;%dR"},
    {(صحيح)KS_CSV,	"\033|%d;%dV"},
# endif
    {(صحيح)KS_CL,	"\033|C"},
			// attributes switched on with 'h', off with * 'H'
    {(صحيح)KS_ME,	"\033|31H"}, // HL_ALL
    {(صحيح)KS_MR,	"\033|1h"},   // HL_INVERSE
    {(صحيح)KS_MD,	"\033|2h"},   // HL_BOLD
    {(صحيح)KS_SE,	"\033|16H"}, // HL_STANDOUT
    {(صحيح)KS_SO,	"\033|16h"}, // HL_STANDOUT
    {(صحيح)KS_UE,	"\033|8H"},   // HL_UNDERLINE
    {(صحيح)KS_US,	"\033|8h"},   // HL_UNDERLINE
    {(صحيح)KS_UCE,	"\033|8C"},   // HL_UNDERCURL
    {(صحيح)KS_UCS,	"\033|8c"},   // HL_UNDERCURL
    {(صحيح)KS_STE,	"\033|4C"},   // HL_STRIKETHROUGH
    {(صحيح)KS_STS,	"\033|4c"},   // HL_STRIKETHROUGH
    {(صحيح)KS_CZR,	"\033|4H"},   // HL_ITALIC
    {(صحيح)KS_CZH,	"\033|4h"},   // HL_ITALIC
    {(صحيح)KS_VB,	"\033|f"},
    {(صحيح)KS_MS,	"y"},
    {(صحيح)KS_UT,	"y"},
    {(صحيح)KS_XN,	"y"},
    {(صحيح)KS_LE,	"\b"},		// cursor-left = BS
    {(صحيح)KS_ND,	"\014"},	// cursor-right = CTRL-L
# ifdef TERMINFO
    {(صحيح)KS_CM,	"\033|%p1%d;%p2%dM"},
# وإلا
    {(صحيح)KS_CM,	"\033|%d;%dM"},
# endif
	// there are no key sequences here, the GUI sequences are recognized
	// in check_termcode()

    {(صحيح)KS_NAME,	عدم}  // end marker
};
#پایان_اگر

/*
 * Amiga console window, default for Amiga.
 */
ساكن tcap_مدخل_T builtin_amiga[] = {
    {(صحيح)KS_CE,	"\033[K"},
    {(صحيح)KS_CD,	"\033[J"},
    {(صحيح)KS_AL,	"\033[L"},
#اگر_تعریف_شده TERMINFO
    {(صحيح)KS_CAL,	"\033[%p1%dL"},
#وإلا
    {(صحيح)KS_CAL,	"\033[%dL"},
#پایان_اگر
    {(صحيح)KS_DL,	"\033[M"},
#اگر_تعریف_شده TERMINFO
    {(صحيح)KS_CDL,	"\033[%p1%dM"},
#وإلا
    {(صحيح)KS_CDL,	"\033[%dM"},
#پایان_اگر
    {(صحيح)KS_CL,	"\014"},
    {(صحيح)KS_VI,	"\033[0 p"},
    {(صحيح)KS_VE,	"\033[1 p"},
    {(صحيح)KS_ME,	"\033[0m"},
    {(صحيح)KS_MR,	"\033[7m"},
    {(صحيح)KS_MD,	"\033[1m"},
    {(صحيح)KS_SE,	"\033[0m"},
    {(صحيح)KS_SO,	"\033[33m"},
    {(صحيح)KS_US,	"\033[4m"},
    {(صحيح)KS_UE,	"\033[0m"},
    {(صحيح)KS_CZH,	"\033[3m"},
    {(صحيح)KS_CZR,	"\033[0m"},
#إذا defined(__amigaos4__) || defined(__MORPHOS__) || defined(__AROS__)
    {(صحيح)KS_CCO,	"8"},		// allow 8 colors
# ifdef TERMINFO
    {(صحيح)KS_CAB,	"\033[4%p1%dm"},// set background color
    {(صحيح)KS_CAF,	"\033[3%p1%dm"},// set foreground color
# وإلا
    {(صحيح)KS_CAB,	"\033[4%dm"},	// set background color
    {(صحيح)KS_CAF,	"\033[3%dm"},	// set foreground color
# endif
    {(صحيح)KS_OP,	"\033[m"},	// reset colors
#پایان_اگر
    {(صحيح)KS_MS,	"y"},
    {(صحيح)KS_UT,	"y"},		// guessed
    {(صحيح)KS_LE,	"\b"},
#اگر_تعریف_شده TERMINFO
    {(صحيح)KS_CM,	"\033[%i%p1%d;%p2%dH"},
#وإلا
    {(صحيح)KS_CM,	"\033[%i%d;%dH"},
#پایان_اگر
#إذا defined(__MORPHOS__)
    {(صحيح)KS_SR,	"\033M"},
#پایان_اگر
#اگر_تعریف_شده TERMINFO
    {(صحيح)KS_CRI,	"\033[%p1%dC"},
#وإلا
    {(صحيح)KS_CRI,	"\033[%dC"},
#پایان_اگر
    {K_به_بالا,		"\233A"},
    {K_پایین,		"\233B"},
    {K_چپ,		"\233D"},
    {K_راست,		"\233C"},
    {K_S_به_بالا,		"\233T"},
    {K_S_پایین,		"\233S"},
    {K_S_چپ,		"\233 A"},
    {K_S_راست,		"\233 @"},
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
    {K_انتها,		"\233\064\065~"},	// 101 key keyboard

    {BT_EXTRA_کلیدها,	""},
    {TERMCAP2_کلید('#', '2'), "\233\065\064~"},	// shifted home key
    {TERMCAP2_کلید('#', '3'), "\233\065\060~"},	// shifted insert key
    {TERMCAP2_کلید('*', '7'), "\233\065\065~"},	// shifted end key

    {(صحيح)KS_NAME,	عدم}  // end marker
};

/*
 * The most minimal terminal: only clear screen and cursor positioning.
 */
ساكن tcap_مدخل_T builtin_dumb[] = {
    {(صحيح)KS_CL,	"\014"},
#اگر_تعریف_شده TERMINFO
    {(صحيح)KS_CM,	"\033[%i%p1%d;%p2%dH"},
#وإلا
    {(صحيح)KS_CM,	"\033[%i%d;%dH"},
#پایان_اگر

    {(صحيح)KS_NAME,	عدم}  // end marker
};

/*
 * Terminal used for debugging.
 */
ساكن tcap_مدخل_T builtin_debug[] = {
    {(صحيح)KS_CE,	"[CE]"},
    {(صحيح)KS_CD,	"[CD]"},
    {(صحيح)KS_AL,	"[AL]"},
#اگر_تعریف_شده TERMINFO
    {(صحيح)KS_CAL,	"[CAL%p1%d]"},
#وإلا
    {(صحيح)KS_CAL,	"[CAL%d]"},
#پایان_اگر
    {(صحيح)KS_DL,	"[DL]"},
#اگر_تعریف_شده TERMINFO
    {(صحيح)KS_CDL,	"[CDL%p1%d]"},
#وإلا
    {(صحيح)KS_CDL,	"[CDL%d]"},
#پایان_اگر
#اگر_تعریف_شده TERMINFO
    {(صحيح)KS_CS,	"[%p1%dCS%p2%d]"},
#وإلا
    {(صحيح)KS_CS,	"[%dCS%d]"},
#پایان_اگر
#اگر_تعریف_شده TERMINFO
    {(صحيح)KS_CSV,	"[%p1%dCSV%p2%d]"},
#وإلا
    {(صحيح)KS_CSV,	"[%dCSV%d]"},
#پایان_اگر
#اگر_تعریف_شده TERMINFO
    {(صحيح)KS_CAB,	"[CAB%p1%d]"},
    {(صحيح)KS_CAF,	"[CAF%p1%d]"},
    {(صحيح)KS_CSB,	"[CSB%p1%d]"},
    {(صحيح)KS_CSF,	"[CSF%p1%d]"},
#وإلا
    {(صحيح)KS_CAB,	"[CAB%d]"},
    {(صحيح)KS_CAF,	"[CAF%d]"},
    {(صحيح)KS_CSB,	"[CSB%d]"},
    {(صحيح)KS_CSF,	"[CSF%d]"},
#پایان_اگر
    {(صحيح)KS_CAU,	"[CAU%d]"},
    {(صحيح)KS_OP,	"[OP]"},
    {(صحيح)KS_LE,	"[LE]"},
    {(صحيح)KS_CL,	"[CL]"},
    {(صحيح)KS_VI,	"[VI]"},
    {(صحيح)KS_VE,	"[VE]"},
    {(صحيح)KS_VS,	"[VS]"},
    {(صحيح)KS_ME,	"[ME]"},
    {(صحيح)KS_MR,	"[MR]"},
    {(صحيح)KS_MB,	"[MB]"},
    {(صحيح)KS_MD,	"[MD]"},
    {(صحيح)KS_SE,	"[SE]"},
    {(صحيح)KS_SO,	"[SO]"},
    {(صحيح)KS_UE,	"[UE]"},
    {(صحيح)KS_US,	"[US]"},
    {(صحيح)KS_UCE,	"[UCE]"},
    {(صحيح)KS_UCS,	"[UCS]"},
    {(صحيح)KS_USS,	"[USS]"},
    {(صحيح)KS_DS,	"[DS]"},
    {(صحيح)KS_CDS,	"[CDS]"},
    {(صحيح)KS_STE,	"[STE]"},
    {(صحيح)KS_STS,	"[STS]"},
    {(صحيح)KS_MS,	"[MS]"},
    {(صحيح)KS_UT,	"[UT]"},
    {(صحيح)KS_XN,	"[XN]"},
#اگر_تعریف_شده TERMINFO
    {(صحيح)KS_CM,	"[%p1%dCM%p2%d]"},
#وإلا
    {(صحيح)KS_CM,	"[%dCM%d]"},
#پایان_اگر
    {(صحيح)KS_SR,	"[SR]"},
#اگر_تعریف_شده TERMINFO
    {(صحيح)KS_CRI,	"[CRI%p1%d]"},
#وإلا
    {(صحيح)KS_CRI,	"[CRI%d]"},
#پایان_اگر
    {(صحيح)KS_VB,	"[VB]"},
    {(صحيح)KS_KS,	"[KS]"},
    {(صحيح)KS_KE,	"[KE]"},
    {(صحيح)KS_TI,	"[TI]"},
    {(صحيح)KS_TE,	"[TE]"},
    {(صحيح)KS_CIS,	"[CIS]"},
    {(صحيح)KS_CIE,	"[CIE]"},
    {(صحيح)KS_CSC,	"[CSC]"},
    {(صحيح)KS_CEC,	"[CEC]"},
    {(صحيح)KS_TS,	"[TS]"},
    {(صحيح)KS_FS,	"[FS]"},
#اگر_تعریف_شده TERMINFO
    {(صحيح)KS_CWS,	"[%p1%dCWS%p2%d]"},
    {(صحيح)KS_CWP,	"[%p1%dCWP%p2%d]"},
#وإلا
    {(صحيح)KS_CWS,	"[%dCWS%d]"},
    {(صحيح)KS_CWP,	"[%dCWP%d]"},
#پایان_اگر
    {(صحيح)KS_CRV,	"[CRV]"},
    {(صحيح)KS_CXM,	"[CXM]"},
    {(صحيح)KS_U7,	"[U7]"},
    {(صحيح)KS_RFG,	"[RFG]"},
    {(صحيح)KS_RBG,	"[RBG]"},
    {(صحيح)KS_CF,	"[CF%d]"},
    {K_به_بالا,		"[KU]"},
    {K_پایین,		"[KD]"},
    {K_چپ,		"[KL]"},
    {K_راست,		"[KR]"},
    {K_XUP,		"[xKU]"},
    {K_XDOWN,		"[xKD]"},
    {K_XLEFT,		"[xKL]"},
    {K_XRIGHT,		"[xKR]"},
    {K_S_به_بالا,		"[S-KU]"},
    {K_S_پایین,		"[S-KD]"},
    {K_S_چپ,		"[S-KL]"},
    {K_C_چپ,		"[C-KL]"},
    {K_S_راست,		"[S-KR]"},
    {K_C_راست,		"[C-KR]"},
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
    {K_انتها,		"[END]"},
    {K_S_انتها,		"[C-END]"},
    {K_C_انتها,		"[C-END]"},
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

    {(صحيح)KS_NAME,	عدم}  // end marker
};

/*
 * List of builtin terminals.
 */
تعريفنوع بنية {
    حرف	    *bitc_name;	    // name, such as "xterm"
    tcap_مدخل_T    *bitc_جدول;    // table with entries for bitc_name
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
#إذا defined(FEAT_GUI)
    {"gui",	    builtin_gui},
#پایان_اگر
    {"amiga",	    builtin_amiga},
    {"dumb",	    builtin_dumb},
    {"debug",	    builtin_debug},

    {عدم,	    عدم},  // end marker
};

#إذا defined(FEAT_TERMGUICOLORS)
    ساكن guicolor_T
termgui_mch_get_color(char_u *name)
{
    أرجع gui_get_color_cmn(name);
}

    guicolor_T
termgui_get_color(char_u *name)
{
    guicolor_T	t;

    إذا (*name == NUL)
	أرجع INVALCOLOR;
    t = termgui_mch_get_color(name);

    إذا (t == INVALCOLOR)
	semsg(_(e_cannot_allocate_color_str), name);
    أرجع t;
}

    guicolor_T
termgui_mch_get_rgb(guicolor_T color)
{
    أرجع color;
}
#پایان_اگر

/*
 * DEFAULT_TERM is used, when no terminal is specified with -T option or $TERM.
 */
#اگر_تعریف_شده AMIGA
# define DEFAULT_جمله	(char_u *)"amiga"
#پایان_اگر

#اگر_تعریف_شده MSWIN
# define DEFAULT_جمله	(char_u *)"win32"
#پایان_اگر

#إذا defined(UNIX)
# define DEFAULT_جمله	(char_u *)"ansi"
#پایان_اگر

#اگر_تعریف_شده VMS
# define DEFAULT_جمله	(char_u *)"vt320"
#پایان_اگر

#اگر_تعریف_شده __HAIKU__
# undef DEFAULT_جمله
# define DEFAULT_جمله	(char_u *)"xterm"
#پایان_اگر

#اگر_تعریف_نشده DEFAULT_جمله
# define DEFAULT_جمله	(char_u *)"dumb"
#پایان_اگر

/*
 * Term_strings contains currently used terminal output strings.
 * It is initialized with the default values by parse_builtin_tcap().
 * The values can be changed by setting the option with the same name.
 */
char_u *(جمله_رشتهها[(صحيح)KS_آخرین + 1]);

ساكن صحيح	need_gather = FALSE;	    // need to fill termleader[]
ساكن char_u	termleader[256 + 1];	    // for check_termcode()
#اگر_تعریف_شده FEAT_TERMRESPONSE
ساكن صحيح	check_for_codes = FALSE;    // check for key code response
#پایان_اگر

/*
 * Structure and table to store terminal features that can be detected by
 * querying the terminal.  Either by inspecting the termresponse or a more
 * specific request.  Besides this there are:
 * t_colors - number of colors supported
 */
تعريفنوع بنية {
    حرف    *tpr_name;
    صحيح	    tpr_مجموعه_by_termresponse;
    صحيح	    tpr_وضعیت;
} termprop_T;

// Values for tpr_status.
#تعریف_کن TPR_UNKNOWN	    'u'
#تعریف_کن TPR_YES		    'y'
#تعریف_کن TPR_NO		    'n'
#تعریف_کن TPR_MOUSE_XTERM     'x'	// use "xterm" for 'ttymouse'
#تعریف_کن TPR_MOUSE_XTERM2    '2'	// use "xterm2" for 'ttymouse'
#تعریف_کن TPR_MOUSE_SGR	    's'	// use "sgr" for 'ttymouse'

// can request the cursor style without messing up the display
#تعریف_کن TPR_CURSOR_STYLE	    0
// can request the cursor blink mode without messing up the display
#تعریف_کن TPR_CURSOR_BLINK	    1
// can set the underline color with t_8u without resetting other colors
#تعریف_کن TPR_UNDERLINE_RGB	    2
// mouse support - TPR_MOUSE_XTERM, TPR_MOUSE_XTERM2 or TPR_MOUSE_SGR
#تعریف_کن TPR_MOUSE		    3
// term response indicates kitty
#تعریف_کن TPR_KITTY		    4
// can send DECRQM requests to terminal
#تعریف_کن TPR_DECRQM		    5
// true color, from the color count of the terminal entry
#تعریف_کن TPR_RGB			    6
// table size
#تعریف_کن TPR_شمار		    7

ساكن termprop_T جمله_props[TPR_شمار];

/*
 * Set the "rgb" terminal property from the color count.  The entry of a
 * direct color terminal, such as xterm-direct, has 16777216 colors.
 */
    ساكن فراغ
مجموعه_rgb_جمله_prop(فراغ)
{
    جمله_props[TPR_RGB].tpr_وضعیت = t_colors == 0x1000000
						       ? TPR_YES : TPR_UNKNOWN;
}

/*
 * Initialize the term_props table.
 * When "all" is FALSE only set those that are detected from the version
 * response.
 */
    فراغ
init_جمله_props(صحيح all)
{
    صحيح i;

    جمله_props[TPR_CURSOR_STYLE].tpr_name = "cursor_style";
    جمله_props[TPR_CURSOR_STYLE].tpr_مجموعه_by_termresponse = FALSE;
    جمله_props[TPR_CURSOR_BLINK].tpr_name = "cursor_blink_mode";
    جمله_props[TPR_CURSOR_BLINK].tpr_مجموعه_by_termresponse = FALSE;
    جمله_props[TPR_UNDERLINE_RGB].tpr_name = "underline_rgb";
    جمله_props[TPR_UNDERLINE_RGB].tpr_مجموعه_by_termresponse = TRUE;
    جمله_props[TPR_MOUSE].tpr_name = "mouse";
    جمله_props[TPR_MOUSE].tpr_مجموعه_by_termresponse = TRUE;
    جمله_props[TPR_KITTY].tpr_name = "kitty";
    جمله_props[TPR_KITTY].tpr_مجموعه_by_termresponse = FALSE;
    جمله_props[TPR_DECRQM].tpr_name = "decrqm";
    جمله_props[TPR_DECRQM].tpr_مجموعه_by_termresponse = TRUE;
    جمله_props[TPR_RGB].tpr_name = "rgb";
    جمله_props[TPR_RGB].tpr_مجموعه_by_termresponse = FALSE;

    لكل (i = 0; i < TPR_شمار; ++i)
	إذا (all || جمله_props[i].tpr_مجموعه_by_termresponse)
	    جمله_props[i].tpr_وضعیت = TPR_UNKNOWN;

    // Derived from the color count, not reset.
    مجموعه_rgb_جمله_prop();
}

#إذا defined(FEAT_EVAL)
    فراغ
f_terminalprops(typval_T *argvars استفادهنشده, typval_T *rettv)
{
# ifdef FEAT_TERMRESPONSE
    صحيح i;
# endif

    إذا (rettv_dict_alloc(rettv) == FAIL)
	أرجع;
# ifdef FEAT_TERMRESPONSE
    لكل (i = 0; i < TPR_شمار; ++i)
    {
	char_u	مقدار[2];

	مقدار[0] = جمله_props[i].tpr_وضعیت;
	مقدار[1] = NUL;
	dict_جمع_کن_string_len(rettv->vval.v_dict, جمله_props[i].tpr_name,
	    مقدار, (مقدار[0] == NUL) ? 0 : 1);
    }
# endif
}
#پایان_اگر

/*
 * Find the builtin termcap entries for "term".
 * This also recognizes similar names.  E.g. "xterm-256color" finds the "xterm"
 * entry.
 * Returns NULL when "term" is not found.
 */
    ساكن tcap_مدخل_T *
پیدا_کن_builtin_جمله(char_u *جمله)
{
    لكل (صحيح i = 0; ; ++i)
    {
	char_u *name = (char_u *)builtin_terminals[i].bitc_name;
	إذا (name == عدم)  // end marker
	    اكسر;
#اگر_تعریف_شده UNIX
	إذا (STRCMP(name, "iris-ansi") == 0 && vim_is_iris(جمله))
	    أرجع builtin_terminals[i].bitc_جدول;
	إذا (STRCMP(name, "xterm") == 0 && vim_is_xterm(جمله))
	    أرجع builtin_terminals[i].bitc_جدول;
#پایان_اگر
#اگر_تعریف_شده VMS
	إذا (STRCMP(name, "vt320") == 0 && vim_is_vt300(جمله))
	    أرجع builtin_terminals[i].bitc_جدول;
#پایان_اگر
	إذا (STRCMP(جمله, name) == 0)
	    أرجع builtin_terminals[i].bitc_جدول;
    }
    أرجع عدم;
}

/*
 * Apply entries from a builtin termcap.
 */
    ساكن فراغ
apply_builtin_tcap(char_u *جمله, tcap_مدخل_T *entries, صحيح overwrite)
{
    صحيح جمله_8bit = جمله_is_8bit(جمله);

    لكل (tcap_مدخل_T *p = entries;
	      p->bt_مدخل != (صحيح)KS_NAME && p->bt_مدخل != BT_EXTRA_کلیدها; ++p)
    {
	إذا ((صحيح)p->bt_مدخل >= 0)	// KS_xx entry
	{
	    // Only set the value if it wasn't set yet or "overwrite" is TRUE.
	    إذا (جمله_رشتهها[p->bt_مدخل] == عدم
		    || جمله_رشتهها[p->bt_مدخل] == خالی_option
		    || overwrite)
	    {
#اگر_تعریف_شده FEAT_EVAL
		صحيح opt_idx = -1;
#پایان_اگر
		// 8bit terminal: use CSI instead of <Esc>[
		إذا (جمله_8bit && جمله_7to8bit((char_u *)p->bt_string) != 0)
		{
		    char_u  *s, *t;

		    s = vim_strsave((char_u *)p->bt_string);
		    إذا (s != عدم)
		    {
			لكل (t = s; *t; ++t)
			    إذا (جمله_7to8bit(t))
			    {
				*t = جمله_7to8bit(t);
				STRMOVE(t + 1, t + 2);
			    }
			جمله_رشتهها[p->bt_مدخل] = s;
#اگر_تعریف_شده FEAT_EVAL
			opt_idx =
#پایان_اگر
				  مجموعه_جمله_option_alloced(
						   &جمله_رشتهها[p->bt_مدخل]);
		    }
		}
		وإلا
		{
		    جمله_رشتهها[p->bt_مدخل] = (char_u *)p->bt_string;
#اگر_تعریف_شده FEAT_EVAL
		    opt_idx = get_جمله_opt_idx(&جمله_رشتهها[p->bt_مدخل]);
#پایان_اگر
		}
#اگر_تعریف_شده FEAT_EVAL
		مجموعه_جمله_option_sctx_idx(عدم, opt_idx);
#پایان_اگر
	    }
	}
	وإلا
	{
	    char_u  name[2];
	    name[0] = KEY2TERMCAP0((صحيح)p->bt_مدخل);
	    name[1] = KEY2TERMCAP1((صحيح)p->bt_مدخل);
	    إذا (پیدا_کن_termcode(name) == عدم || overwrite)
		جمع_کن_termcode(name, (char_u *)p->bt_string, جمله_8bit);
	}
    }
}

/*
 * Apply builtin termcap entries for a given keyprotocol.
 */
    فراغ
apply_keyprotocol(char_u *جمله, keyprot_T prot)
{
    إذا (prot == KEYPROTOCOL_KITTY)
	apply_builtin_tcap(جمله, builtin_kitty, TRUE);
    إذا (prot == KEYPROTOCOL_MOK2)
	apply_builtin_tcap(جمله, builtin_mok2, TRUE);

    إذا (prot != KEYPROTOCOL_NONE)
	// Some function keys may accept modifiers even though the
	// terminfo/termcap entry does not indicate this.
	accept_modifiers_for_function_کلیدها();
}

/*
 * Parsing of the builtin termcap entries.
 * Caller should check if "term" is a valid builtin terminal name.
 * The terminal's name is not set, as this is already done in termcapinit().
 */
    ساكن فراغ
parse_builtin_tcap(char_u *جمله)
{
    tcap_مدخل_T *entries = پیدا_کن_builtin_جمله(جمله);
    إذا (entries != عدم)
	apply_builtin_tcap(جمله, entries, FALSE);
}

/*
 * Set number of colors.
 * Store it as a number in t_colors.
 * Store it as a string in T_CCO (using nr_colors[]).
 */
    فراغ
مجموعه_color_شمار(صحيح nr)
{
    char_u	nr_colors[20];		// string for number of colors

    t_colors = nr;
    إذا (t_colors > 1)
	sprintf((حرف *)nr_colors, "%d", t_colors);
    وإلا
	*nr_colors = NUL;
#إذا 0
# ifdef FEAT_TERMGUICOLORS
    // xterm-direct, enable termguicolors, when it wasn't set yet
    إذا (t_colors == 0x1000000 && !p_tgc_مجموعه)
	مجموعه_option_مقدار((char_u *)"termguicolors", 1L, عدم, 0);
# endif
#پایان_اگر
    مجموعه_string_option_مستقیم((char_u *)"t_Co", -1, nr_colors, OPT_FREE, 0);
}

/*
 * Set the color count to "val" and redraw if it changed.
 */
    ساكن فراغ
may_adjust_color_شمار(صحيح val)
{
    إذا (val == t_colors)
	أرجع;

    // Nr of colors changed, initialize highlighting and redraw everything.
    // This causes a redraw, which usually clears the message.  Try keeping
    // the message if it might work.
    مجموعه_keep_پیام_from_hist();
    مجموعه_color_شمار(val);
    init_highlight(TRUE, FALSE);
#اگر_تعریف_شده DEBUG_TERMRESPONSE
    {
	صحيح r = redraw_asap(UPD_CLEAR);

	LOG_TRN("Received t_Co, redraw_asap(): %d", r);
    }
#وإلا
    redraw_asap(UPD_CLEAR);
#پایان_اگر
}

#اگر_تعریف_شده HAVE_TGETENT
ساكن حرف *(کلید_نامها[]) =
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
    عدم
};
#پایان_اگر

#إذا defined(HAVE_TGETENT) || defined(FEAT_TERMGUICOLORS)
/*
 * Return TRUE if "term_strings[idx]" was not set.
 */
    ساكن صحيح
جمله_رشتهها_not_مجموعه(تعداد Special_کلید idx)
{
    أرجع جمله_STR(idx) == عدم || جمله_STR(idx) == خالی_option;
}
#پایان_اگر

#اگر_تعریف_شده HAVE_TGETENT
/*
 * Get the termcap entries we need with tgetstr(), tgetflag() and tgetnum().
 * "invoke_tgetent()" must have been called before.
 * If "*height" or "*width" are not zero then use the "li" and "col" entries to
 * get their value.
 */
    ساكن فراغ
get_جمله_entries(صحيح *ارتفاع, صحيح *عرض)
{
    ساكن بنية {
		    تعداد Special_کلید dest; // index in term_strings[]
		    حرف *name;		  // termcap name for string
		  } string_نامها[] =
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
			{(تعداد Special_کلید)0, عدم}
		    };
    صحيح		    i;
    ساكن char_u   tstrbuf[TBUFSZ];
    char_u	    *tp = tstrbuf;

    /*
     * get output strings
     */
    لكل (i = 0; string_نامها[i].name != عدم; ++i)
    {
	إذا (جمله_رشتهها_not_مجموعه(string_نامها[i].dest))
	{
	    جمله_STR(string_نامها[i].dest) = TGETSTR(string_نامها[i].name, &tp);
# ifdef FEAT_EVAL
	    مجموعه_جمله_option_sctx_idx(string_نامها[i].name, -1);
# endif
	}
    }

    // tgetflag() returns 1 if the flag is present, 0 if not and
    // possibly -1 if the flag doesn't exist.
    إذا ((T_MS == عدم || T_MS == خالی_option) && tgetflag("ms") > 0)
	T_MS = (char_u *)"y";
    إذا ((T_XS == عدم || T_XS == خالی_option) && tgetflag("xs") > 0)
	T_XS = (char_u *)"y";
    إذا ((T_XN == عدم || T_XN == خالی_option) && tgetflag("xn") > 0)
	T_XN = (char_u *)"y";
    إذا ((T_DB == عدم || T_DB == خالی_option) && tgetflag("db") > 0)
	T_DB = (char_u *)"y";
    إذا ((T_DA == عدم || T_DA == خالی_option) && tgetflag("da") > 0)
	T_DA = (char_u *)"y";
    إذا ((T_UT == عدم || T_UT == خالی_option) && tgetflag("ut") > 0)
	T_UT = (char_u *)"y";
    إذا ((T_XON == عدم || T_XON == خالی_option) && tgetflag("xo") > 0)
	T_XON = (char_u *)"y";

    /*
     * get key codes
     */
    لكل (i = 0; کلید_نامها[i] != عدم; ++i)
	إذا (پیدا_کن_termcode((char_u *)کلید_نامها[i]) == عدم)
	{
	    char_u *p = TGETSTR(کلید_نامها[i], &tp);

	    // if cursor-left == backspace, ignore it (televideo 925)
	    إذا (p != عدم
		    && (*p != Ctrl_H
			|| کلید_نامها[i][0] != 'k'
			|| کلید_نامها[i][1] != 'l'))
		جمع_کن_termcode((char_u *)کلید_نامها[i], p, FALSE);
	}

    إذا (*ارتفاع == 0)
	*ارتفاع = tgetnum("li");
    إذا (*عرض == 0)
	*عرض = tgetnum("co");

    /*
     * Get number of colors (if not done already).
     */
    إذا (جمله_رشتهها_not_مجموعه(KS_CCO))
    {
	مجموعه_color_شمار(tgetnum("Co"));
# ifdef FEAT_EVAL
	مجموعه_جمله_option_sctx_idx("Co", -1);
# endif
    }

# ifndef hpux
    BC = (حرف *)TGETSTR("bc", &tp);
    به_بالا_2 = (حرف *)TGETSTR("up", &tp);
    char_u *p = TGETSTR("pc", &tp);
    إذا (p != عدم)
	PC = *p;
# endif
}
#پایان_اگر

/*
 * Report "term" is not found and list the ones we do know about.
 */
    ساكن فراغ
report_جمله_error(حرف *error_پیام, char_u *جمله)
{
    mch_errmsg("\r\n");
    إذا (error_پیام != عدم)
    {
	mch_errmsg(error_پیام);
	mch_errmsg("\r\n");
    }
    mch_errmsg("'");
    mch_errmsg((حرف *)جمله);
    mch_errmsg(_("' not known. Available builtin terminals are:"));
    mch_errmsg("\r\n");

    لكل (صحيح i = 0; ; ++i)
    {
	حرف *name = builtin_terminals[i].bitc_name;
	إذا (name == عدم)  // end marker
	    اكسر;
	// Do not mention the "gui" entry, the user won't need to type it.
	إذا (STRCMP(name, "gui") != 0)
	{
#اگر_تعریف_شده HAVE_TGETENT
	    mch_errmsg("    builtin_");
#وإلا
	    mch_errmsg("    ");
#پایان_اگر
	    mch_errmsg(name);
	    mch_errmsg("\r\n");
	}
    }
    // Output extra 'cmdheight' line breaks to avoid that the following error
    // message overwrites the last terminal name.
    لكل (صحيح i = 1; i < p_ch; ++i)
	mch_errmsg("\r\n");
}

    ساكن فراغ
report_default_جمله(char_u *جمله)
{
    mch_errmsg(_("defaulting to '"));
    mch_errmsg((حرف *)جمله);
    mch_errmsg("'\r\n");
    إذا (emsg_silent == 0 && !درون_assert_fails)
    {
	screen_شروع();	// don't know where cursor is now
	بیرون_flush();
	إذا (!is_not_a_جمله())
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
تطابق_keyprotocol(char_u *جمله)
{
    صحيح		len = (صحيح)STRLEN(p_kpc) + 1;
    char_u	*buf = alloc(len);
    إذا (buf == عدم)
	أرجع KEYPROTOCOL_FAIL;

    keyprot_T	ret = KEYPROTOCOL_FAIL;
    char_u	*p = p_kpc;
    طالما (*p != NUL)
    {
	// Isolate one comma separated item.
	(فراغ)copy_option_part(&p, buf, len, ",");
	char_u *colon = vim_strchr(buf, ':');
	إذا (colon == عدم || colon == buf || colon[1] == NUL)
	    اذهب theend;
	*colon = NUL;

	keyprot_T prot;
	// Note: Keep this in sync with p_kpc_protocol_values.
	إذا (STRCMP(colon + 1, "none") == 0)
	    prot = KEYPROTOCOL_NONE;
	وإلا إذا (STRCMP(colon + 1, "mok2") == 0)
	    prot = KEYPROTOCOL_MOK2;
	وإلا إذا (STRCMP(colon + 1, "kitty") == 0)
	    prot = KEYPROTOCOL_KITTY;
	وإلا
	    اذهب theend;

	regmatch_T regmatch;
	CLEAR_فیلد(regmatch);
	regmatch.rm_ic = TRUE;
	regmatch.regprog = vim_regcomp(buf, RE_MAGIC);
	إذا (regmatch.regprog == عدم)
	    اذهب theend;

	صحيح تطابق = جمله != عدم && vim_regexec(&regmatch, جمله, (colnr_T)0);
	vim_regfree(regmatch.regprog);
	إذا (تطابق)
	{
	    ret = prot;
	    اذهب theend;
	}

    }

    // No match found, use "none".
    ret = KEYPROTOCOL_NONE;

theend:
    vim_free(buf);
    أرجع ret;
}

/*
 * Set terminal options for terminal "term".
 * Return OK if terminal 'term' was found in a termcap, FAIL otherwise.
 *
 * While doing this, until ttest(), some options may be NULL, be careful.
 */
    صحيح
مجموعه_termname(char_u *جمله)
{
#اگر_تعریف_شده HAVE_TGETENT
    صحيح		builtin_اولی = p_tbi;
    صحيح		try;
    صحيح		termcap_cleared = FALSE;
#پایان_اگر
    صحيح		عرض = 0, ارتفاع = 0;
    حرف	*error_پیام = عدم;
    char_u	*bs_p, *del_p;

    // In silect mode (ex -s) we don't use the 'term' option.
    إذا (silent_نما)
	أرجع OK;

    detected_8bit = FALSE;		// reset 8-bit detection

    إذا (جمله_is_builtin(جمله))
    {
	جمله += 8;
#اگر_تعریف_شده HAVE_TGETENT
	builtin_اولی = 1;
#پایان_اگر
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
#اگر_تعریف_شده HAVE_TGETENT
    لكل (try = builtin_اولی ? 0 : 1; try < 3; ++try)
    {
	/*
	 * Use external termcap
	 */
	إذا (try == 1)
	{
	    char_u	    tbuf[TBUFSZ];

	    /*
	     * If the external termcap does not have a matching entry, try the
	     * builtin ones.
	     */
	    إذا ((error_پیام = invoke_tgetent(tbuf, جمله)) == عدم)
	    {
		إذا (!termcap_cleared)
		{
		    clear_termoptions();	// clear old options
		    termcap_cleared = TRUE;
		}

		get_جمله_entries(&ارتفاع, &عرض);
	    }
	}
	وإلا	    // try == 0 || try == 2
#پایان_اگر // HAVE_TGETENT
	/*
	 * Use builtin termcap
	 */
	{
#اگر_تعریف_شده HAVE_TGETENT
	    /*
	     * If builtin termcap was already used, there is no need to search
	     * for the builtin termcap again, quit now.
	     */
	    إذا (try == 2 && builtin_اولی && termcap_cleared)
		اكسر;
#پایان_اگر
	    /*
	     * Search for 'term' in builtin_terminals[].
	     */
	    tcap_مدخل_T *termp = پیدا_کن_builtin_جمله(جمله);
	    إذا (termp == عدم)	// did not find it
	    {
#اگر_تعریف_شده HAVE_TGETENT
		/*
		 * If try == 0, first try the external termcap. If that is not
		 * found we'll get back here with try == 2.
		 * If termcap_cleared is set we used the external termcap,
		 * don't complain about not finding the term in the builtin
		 * termcap.
		 */
		إذا (try == 0)			// try external one
		    استمر;
		إذا (termcap_cleared)		// found in external termcap
		    اكسر;
#پایان_اگر
		report_جمله_error(error_پیام, جمله);

		// when user typed :set term=xxx, quit here
		إذا (starting != NO_SCREEN)
		{
		    screen_شروع();	// don't know where cursor is now
		    wait_return(TRUE);
		    أرجع FAIL;
		}
		جمله = DEFAULT_جمله;
		report_default_جمله(جمله);
		مجموعه_string_option_مستقیم((char_u *)"term", -1, جمله,
								 OPT_FREE, 0);
		display_errors();
	    }
	    بیرون_flush();
#اگر_تعریف_شده HAVE_TGETENT
	    إذا (!termcap_cleared)
	    {
#پایان_اگر
		clear_termoptions();	    // clear old options
#اگر_تعریف_شده HAVE_TGETENT
		termcap_cleared = TRUE;
	    }
#پایان_اگر
	    parse_builtin_tcap(جمله);

#اگر_تعریف_شده FEAT_GUI
	    إذا (جمله_is_gui(جمله))
	    {
		بیرون_flush();
		gui_init();
		// If starting the GUI failed, don't do any of the other
		// things for this terminal
		إذا (!gui.درون_use)
		    أرجع FAIL;
# ifdef HAVE_TGETENT
		اكسر;		// don't try using external termcap
# endif
	    }
#پایان_اگر // FEAT_GUI
	}
#اگر_تعریف_شده HAVE_TGETENT
    }
#پایان_اگر

#اگر_تعریف_شده FEAT_GUI
    إذا (!gui.درون_use)
#پایان_اگر
    {
	// Use the 'keyprotocol' option to adjust the t_TE and t_TI
	// termcap entries if there is an entry matching "term".
	keyprot_T kpc = تطابق_keyprotocol(جمله);
	apply_keyprotocol(جمله, kpc);

#اگر_تعریف_شده FEAT_TERMGUICOLORS
	// There is no good way to detect that the terminal supports RGB
	// colors.  Since these termcap entries are non-standard anyway and
	// only used when the user sets 'termguicolors' we might as well add
	// them.  But not when one of them was already set.
	إذا (جمله_رشتهها_not_مجموعه(KS_8F)
		&& جمله_رشتهها_not_مجموعه(KS_8B)
		&& جمله_رشتهها_not_مجموعه(KS_8U))
	    apply_builtin_tcap(جمله, builtin_rgb, TRUE);
#پایان_اگر
#اگر_تعریف_شده HAVE_TGETENT
	إذا (جمله_رشتهها_not_مجموعه(KS_CF))
	    apply_builtin_tcap(جمله, special_جمله, TRUE);
	إذا (جمله_رشتهها_not_مجموعه(KS_BSU) && جمله_رشتهها_not_مجموعه(KS_ESU))
	    apply_builtin_tcap(جمله, builtin_sync_output, TRUE);
#پایان_اگر
    }

/*
 * special: There is no info in the termcap about whether the cursor
 * positioning is relative to the start of the screen or to the start of the
 * scrolling region.  We just guess here. Only msdos pcterm is known to do it
 * relative.
 */
    إذا (STRCMP(جمله, "pcterm") == 0)
	T_CCS = (char_u *)"yes";
    وإلا
	T_CCS = خالی_option;

    // Special case: "kitty" may not have a "RV" entry in terminfo, but we need
    // to request the version for several other things to work.
    إذا (جستجوی_رشته((حرف *)جمله, "kitty") != عدم
					   && (T_CRV == عدم || *T_CRV == NUL))
	T_CRV = (char_u *)"\033[>c";

#اگر_تعریف_شده UNIX
/*
 * Any "stty" settings override the default for t_kb from the termcap.
 * This is in os_unix.c, because it depends a lot on the version of unix that
 * is being used.
 * Don't do this when the GUI is active, it uses "t_kb" and "t_kD" directly.
 */
# ifdef FEAT_GUI
    إذا (!gui.درون_use)
# endif
	get_stty();
#پایان_اگر

/*
 * If the termcap has no entry for 'bs' and/or 'del' and the ioctl() also
 * didn't work, use the default CTRL-H
 * The default for t_kD is DEL, unless t_kb is DEL.
 * The vim_strsave'd strings are probably lost forever, well it's only two
 * bytes.  Don't do this when the GUI is active, it uses "t_kb" and "t_kD"
 * directly.
 */
#اگر_تعریف_شده FEAT_GUI
    إذا (!gui.درون_use)
#پایان_اگر
    {
	bs_p = پیدا_کن_termcode((char_u *)"kb");
	del_p = پیدا_کن_termcode((char_u *)"kD");
	إذا (bs_p == عدم || *bs_p == NUL)
	    جمع_کن_termcode((char_u *)"kb", (bs_p = (char_u *)CTRL_H_STR), FALSE);
	إذا ((del_p == عدم || *del_p == NUL) &&
					    (bs_p == عدم || *bs_p != DEL))
	    جمع_کن_termcode((char_u *)"kD", (char_u *)DEL_STR, FALSE);
    }

#إذا defined(UNIX) || defined(VMS)
    جمله_is_xterm = vim_is_xterm(جمله);
#پایان_اگر
#اگر_تعریف_شده FEAT_TERMRESPONSE
    // Reset terminal properties that are set based on the termresponse, which
    // will be sent out soon.
    init_جمله_props(FALSE);
#پایان_اگر

#إذا defined(UNIX) || defined(VMS)
    // If the first number in t_XM is 1006 then the terminal will support SGR
    // mouse reporting.
    صحيح did_مجموعه_ttym = FALSE;
    إذا (T_CXM != عدم && *T_CXM != NUL && !option_was_مجموعه((char_u *)"ttym"))
    {
	char_u *p = T_CXM;

	طالما (*p != NUL && !VIM_ISDIGIT(*p))
	    ++p;
	إذا (getdigits(&p) == 1006)
	{
	    did_مجموعه_ttym = TRUE;
	    مجموعه_option_مقدار_give_err((char_u *)"ttym", 0L, (char_u *)"sgr", 0);
	}
    }

    /*
     * For Unix, set the 'ttymouse' option to the type of mouse to be used.
     * The termcode for the mouse is added as a side effect in option.c.
     */
    {
	char_u	*p = (char_u *)"";

# ifdef FEAT_MOUSE_XTERM
	إذا (use_xterm_like_mouse(جمله))
	{
	    إذا (use_xterm_mouse())
		p = عدم;	// keep existing value, might be "xterm2"
	    وإلا
		p = (char_u *)"xterm";
	}
# endif
	إذا (p != عدم && !did_مجموعه_ttym)
	{
	    مجموعه_option_مقدار_give_err((char_u *)"ttym", 0L, p, 0);
	    // Reset the WAS_SET flag, 'ttymouse' can be set to "sgr" or
	    // "xterm2" in check_termcode().
	    reset_option_was_مجموعه((char_u *)"ttym");
	}
	إذا (p == عدم
# ifdef FEAT_GUI
		|| gui.درون_use
# endif
		)
	    check_mouse_termcode();	// set mouse termcode anyway
    }
#وإلا
    مجموعه_mouse_termcode(KS_MOUSE, (char_u *)"\233M");
#پایان_اگر

#اگر_تعریف_شده FEAT_MOUSE_XTERM
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
	جمع_کن_termcode(name, (char_u *)"\033[I", FALSE);

	// handle focus out event
	name[1] = KE_FOCUSLOST;
	جمع_کن_termcode(name, (char_u *)"\033[O", FALSE);

	need_gather = TRUE;
    }
#پایان_اگر
#إذا (defined(UNIX) || defined(VMS))
    // First time after setting 'term' a focus event is always reported.
    focus_state = MAYBE;
#پایان_اگر

#اگر_تعریف_شده USE_جمله_CONSOLE
    // DEFAULT_TERM indicates that it is the machine console.
    إذا (STRCMP(جمله, DEFAULT_جمله) != 0)
	جمله_console = FALSE;
    وإلا
    {
	جمله_console = TRUE;
# ifdef AMIGA
	win_resize_on();	// enable window resizing reports
# endif
    }
#پایان_اگر

    ttest(TRUE);	// make sure we have a valid set of terminal codes

    پر_screen = TRUE;		// we can use termcap codes from now on
    مجموعه_جمله_defaults();	// use current values as defaults
#اگر_تعریف_شده FEAT_TERMRESPONSE
    LOG_TR1("setting crv_status to STATUS_GET");
    crv_وضعیت.tr_پیشرفت = وضعیت_GET;	// Get terminal version later
    write_t_8u_state = FALSE;
#پایان_اگر

    /*
     * Initialize the terminal with the appropriate termcap codes.
     * Set the mouse and window title if possible.
     * Don't do this when starting, need to parse the .vimrc first, because it
     * may redefine t_TI etc.
     */
    إذا (starting != NO_SCREEN)
    {
	starttermcap();		// may change terminal mode
	setmouse();		// may start using the mouse
	maketitle();		// may display window title
    }

	// display initial screen after ttest() checking. jw.
    إذا (عرض <= 0 || ارتفاع <= 0)
    {
	// termcap failed to report size
	// set defaults, in case ui_get_shellsize() also fails
	عرض = 80;
#إذا defined(MSWIN)
	ارتفاع = 25;	    // console is often 25 lines
#وإلا
	ارتفاع = 24;	    // most terminals are 24 lines
#پایان_اگر
    }
    مجموعه_shellsize(عرض, ارتفاع, FALSE);	// may change Rows
    إذا (starting != NO_SCREEN)
    {
	إذا (scroll_region)
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
		إذا (curbuf->b_ml.ml_mfp != عدم)
		{
		    aucmd_prepbuf(&aco, buf);
		    إذا (curbuf == buf)
		    {
			apply_autocmds(EVENT_TERMCHANGED, عدم, عدم, FALSE,
								      curbuf);
			// restore curwin/curbuf and a few other things
			aucmd_restbuf(&aco);
		    }
		}
	    }
	}
    }

#اگر_تعریف_شده FEAT_TERMRESPONSE
    may_req_termresponse();
#پایان_اگر

#اگر_تعریف_شده FEAT_IMAGE
    (فراغ)update_image_backend();
#پایان_اگر

    أرجع OK;
}

#إذا defined(EXITFREE)

# ifdef HAVE_DEL_CURTERM
#  include <term.h>	    // declares cur_term
# endif

/*
 * If supported, delete "cur_term", which caches terminal related entries.
 * Avoids that valgrind reports possibly lost memory.
 */
    فراغ
free_cur_جمله(فراغ)
{
# ifdef HAVE_DEL_CURTERM
    إذا (cur_جمله)
	del_curterm(cur_جمله);
# endif
}

#پایان_اگر

#اگر_تعریف_شده HAVE_TGETENT
/*
 * Call tgetent()
 * Return error message if it fails, NULL if it's OK.
 */
    ساكن حرف *
invoke_tgetent(char_u *tbuf, char_u *جمله)
{
    صحيح	    i;

    // Note: Valgrind may report a leak here, because the library keeps one
    // buffer around that we can't ever free.
    i = TGETENT(tbuf, جمله);
    إذا (i < 0		    // -1 is always an error
# ifdef TGETENT_ZERO_ERR
	    || i == 0	    // sometimes zero is also an error
# endif
       )
    {
	// On FreeBSD tputs() gets a SEGV after a tgetent() which fails.  Call
	// tgetent() with the always existing "dumb" entry to avoid a crash or
	// hang.
	(فراغ)TGETENT(tbuf, "dumb");

	إذا (i < 0)
# ifdef TGETENT_ZERO_ERR
	    أرجع _(e_cannot_open_termcap_file);
	إذا (i == 0)
# endif
# ifdef TERMINFO
	    أرجع _(e_terminal_مدخل_not_یافت_درون_terminfo);
# وإلا
	    أرجع _(e_terminal_مدخل_not_یافت_درون_termcap);
# endif
    }
    أرجع عدم;
}

/*
 * Some versions of tgetstr() have been reported to return -1 instead of NULL.
 * Fix that here.
 */
    ساكن char_u *
vim_tgetstr(حرف *s, char_u **pp)
{
    حرف	*p;

    p = tgetstr(s, (حرف **)pp);
    إذا (p == (حرف *)-1)
	p = عدم;
    أرجع (char_u *)p;
}
#پایان_اگر // HAVE_TGETENT

#إذا defined(HAVE_TGETENT) && (defined(UNIX) || defined(VMS) || defined(MACOS_X))
/*
 * Get Columns and Rows from the termcap. Used after a window signal if the
 * ioctl() fails. It doesn't make sense to call tgetent each time if the "co"
 * and "li" entries never change. But on some systems this works.
 * Errors while getting the entries are ignored.
 */
    فراغ
getlinecol(
    دراز	*cp,	// pointer to columns
    دراز	*rp)	// pointer to rows
{
    char_u	tbuf[TBUFSZ];

    إذا (T_NAME == عدم || *T_NAME == NUL
	    || invoke_tgetent(tbuf, T_NAME) != عدم)
	أرجع;

    إذا (*cp == 0)
	*cp = tgetnum("co");
    إذا (*rp == 0)
	*rp = tgetnum("li");
}
#پایان_اگر // defined(HAVE_TGETENT) && defined(UNIX)

/*
 * Get a string entry from the termcap and add it to the list of termcodes.
 * Used for <t_xx> special keys.
 * Give an error message for failure when not sourcing.
 * If force given, replace an existing entry.
 * Return FAIL if the entry was not found, OK if the entry was added.
 */
    صحيح
جمع_کن_termcap_مدخل(char_u *name, صحيح force)
{
    char_u  *جمله;
    صحيح	    کلید;
#اگر_تعریف_شده HAVE_TGETENT
    char_u  *string;
    صحيح	    i;
    صحيح	    builtin_اولی;
    char_u  tbuf[TBUFSZ];
    char_u  tstrbuf[TBUFSZ];
    char_u  *tp = tstrbuf;
    حرف    *error_پیام = عدم;
#پایان_اگر

/*
 * If the GUI is running or will start in a moment, we only support the keys
 * that the GUI can produce.
 */
#اگر_تعریف_شده FEAT_GUI
    إذا (gui.درون_use || gui.starting)
	أرجع gui_mch_haskey(name);
#پایان_اگر

    إذا (!force && پیدا_کن_termcode(name) != عدم)	    // it's already there
	أرجع OK;

    جمله = T_NAME;
    إذا (جمله == عدم || *جمله == NUL)	    // 'term' not defined yet
	أرجع FAIL;

    إذا (جمله_is_builtin(جمله))		    // name starts with "builtin_"
    {
	جمله += 8;
#اگر_تعریف_شده HAVE_TGETENT
	builtin_اولی = TRUE;
#پایان_اگر
    }
#اگر_تعریف_شده HAVE_TGETENT
    وإلا
	builtin_اولی = p_tbi;
#پایان_اگر

#اگر_تعریف_شده HAVE_TGETENT
/*
 * We can get the entry from the builtin termcap and from the external one.
 * If 'ttybuiltin' is on or the terminal name starts with "builtin_", try
 * builtin termcap first.
 * If 'ttybuiltin' is off, try external termcap first.
 */
    لكل (i = 0; i < 2; ++i)
    {
	إذا ((!builtin_اولی) == i)
#پایان_اگر
	/*
	 * Search in builtin termcaps
	 */
	{
	    tcap_مدخل_T *termp = پیدا_کن_builtin_جمله(جمله);
	    إذا (termp != عدم)	// found it
	    {
		کلید = TERMCAP2_کلید(name[0], name[1]);
		++termp;
		طالما (termp->bt_مدخل != (صحيح)KS_NAME)
		{
		    إذا ((صحيح)termp->bt_مدخل == کلید)
		    {
			جمع_کن_termcode(name, (char_u *)termp->bt_string,
							  جمله_is_8bit(جمله));
			أرجع OK;
		    }
		    ++termp;
		}
	    }
	}
#اگر_تعریف_شده HAVE_TGETENT
	وإلا
	/*
	 * Search in external termcap
	 */
	{
	    error_پیام = invoke_tgetent(tbuf, جمله);
	    إذا (error_پیام == عدم)
	    {
		string = TGETSTR((حرف *)name, &tp);
		إذا (string != عدم && *string != NUL)
		{
		    جمع_کن_termcode(name, string, FALSE);
		    أرجع OK;
		}
	    }
	}
    }
#پایان_اگر

    إذا (SOURCING_NAME == عدم)
    {
#اگر_تعریف_شده HAVE_TGETENT
	إذا (error_پیام != عدم)
	    emsg(error_پیام);
	وإلا
#پایان_اگر
	    semsg(_(e_no_str_مدخل_درون_termcap), name);
    }
    أرجع FAIL;
}

    ساكن صحيح
جمله_is_builtin(char_u *name)
{
    أرجع (STRNCMP(name, "builtin_", (size_t)8) == 0);
}

/*
 * Return TRUE if terminal "name" uses CSI instead of <Esc>[.
 * Assume that the terminal is using 8-bit controls when the name contains
 * "8bit", like in "xterm-8bit".
 */
    صحيح
جمله_is_8bit(char_u *name)
{
    أرجع (detected_8bit || جستجوی_رشته((حرف *)name, "8bit") != عدم);
}

/*
 * Translate terminal control chars from 7-bit to 8-bit:
 * <Esc>[ -> CSI  <M_C_[>
 * <Esc>] -> OSC  <M-C-]>
 * <Esc>O -> <M-C-O>
 */
    ساكن صحيح
جمله_7to8bit(char_u *p)
{
    إذا (*p != ESC)
	أرجع 0;

    إذا (p[1] == '[')
	أرجع CSI;
    وإلا إذا (p[1] == ']')
	أرجع OSC;
    وإلا إذا (p[1] == 'O')
	أرجع 0x8f;
    أرجع 0;
}

#إذا defined(FEAT_GUI)
    صحيح
جمله_is_gui(char_u *name)
{
    أرجع (STRCMP(name, "builtin_gui") == 0 || STRCMP(name, "gui") == 0);
}
#پایان_اگر

#إذا !defined(HAVE_TGETENT) || defined(AMIGA)

    char_u *
tltoa(بیعلامت دراز i)
{
    ساكن char_u buf[16];
    char_u	*p;

    p = buf + 15;
    *p = '\0';
    افعل
    {
	--p;
	*p = (char_u) (i % 10 + '0');
	i /= 10;
    }
    طالما (i > 0 && p > buf);
    أرجع p;
}
#پایان_اگر

#اگر_تعریف_نشده HAVE_TGETENT

/*
 * minimal tgoto() implementation.
 * no padding and we only parse for %i %d and %+char
 */
    ساكن حرف *
tgoto(حرف *cm, صحيح x, صحيح y)
{
    ساكن حرف buf[30];
    حرف *p, *s, *e;

    إذا (!cm)
	أرجع "OOPS";
    e = buf + 29;
    لكل (s = buf; s < e && *cm; cm++)
    {
	إذا (*cm != '%')
	{
	    *s++ = *cm;
	    استمر;
	}
	بدل (*++cm)
	{
	حالة 'd':
	    p = (حرف *)tltoa((بیعلامت دراز)y);
	    y = x;
	    طالما (*p)
		*s++ = *p++;
	    اكسر;
	حالة 'i':
	    x++;
	    y++;
	    اكسر;
	حالة '+':
	    *s++ = (حرف)(*++cm + y);
	    y = x;
	    اكسر;
	حالة '%':
	    *s++ = *cm;
	    اكسر;
	افتراضي:
	    أرجع "OOPS";
	}
    }
    *s = '\0';
    أرجع buf;
}

#پایان_اگر // HAVE_TGETENT

/*
 * Set the terminal name and initialize the terminal options.
 * If "name" is NULL or empty, get the terminal name from the environment.
 * If that fails, use the default terminal name.
 */
    فراغ
termcapinit(char_u *name)
{
    char_u	*جمله = name;

    إذا (جمله != عدم && *جمله == NUL)
	جمله = عدم;	    // empty name is equal to no name

#اگر_تعریف_نشده MSWIN
    إذا (جمله == عدم)
	جمله = mch_getenv((char_u *)"TERM");
#پایان_اگر
    إذا (جمله == عدم || *جمله == NUL)
	جمله = DEFAULT_جمله;
    مجموعه_string_option_مستقیم((char_u *)"term", -1, جمله, OPT_FREE, 0);

    // Set the default terminal name.
    مجموعه_string_default("term", جمله);
    مجموعه_string_default("ttytype", جمله);

    // Avoid using "term" here, because the next mch_getenv() may overwrite it.
    مجموعه_termname(T_NAME != عدم ? T_NAME : جمله);
}

/*
 * The number of calls to ui_write is reduced by using "out_buf".
 */
#تعریف_کن بیرون_اندازه	8191

// add one to allow mch_write() in os_win32.c to append a NUL
ساكن char_u		بیرون_buf[بیرون_اندازه + 1];

ساكن صحيح		بیرون_pos = 0;	// number of chars in out_buf

// Since the maximum number of SGR parameters shown as a normal value range is
// 16, the escape sequence length can be 4 * 16 + lead + tail.
#تعریف_کن MAX_ESC_SEQ_LEN	80

/*
 * out_flush(): flush the output buffer
 */
    فراغ
بیرون_flush(فراغ)
{
    صحيح	    len;

    إذا (بیرون_pos == 0)
	أرجع;

    // set out_pos to 0 before ui_write, to avoid recursiveness
    len = بیرون_pos;
    بیرون_pos = 0;
    ui_write(بیرون_buf, len, FALSE);
#اگر_تعریف_شده FEAT_EVAL
    إذا (ch_log_output != FALSE)
    {
	بیرون_buf[len] = NUL;
	ch_log(عدم, "raw %s output: \"%s\"",
# ifdef FEAT_GUI
		(gui.درون_use && !gui.dying && !gui.starting) ? "GUI" :
# endif
		"terminal",
		بیرون_buf);
	إذا (ch_log_output == TRUE)
	    ch_log_output = FALSE;  // only log once
    }
#پایان_اگر
}

/*
 * out_flush_cursor(): flush the output buffer and redraw the cursor.
 * Does not flush recursively in the GUI to avoid slow drawing.
 */
    فراغ
بیرون_flush_cursor(
    صحيح	    force استفادهنشده,   // when TRUE, update cursor even when not moved
    صحيح	    clear_selection استفادهنشده) // clear selection under cursor
{
    mch_disable_flush();
    بیرون_flush();
    mch_enable_flush();
#اگر_تعریف_شده FEAT_GUI
    إذا (gui.درون_use)
    {
	gui_update_cursor(force, clear_selection);
	gui_may_flush();
    }
#پایان_اگر
}


/*
 * Sometimes a byte out of a multi-byte character is written with out_char().
 * To avoid flushing half of the character, call this function first.
 */
    فراغ
بیرون_flush_check(فراغ)
{
    إذا (enc_dbcs != 0 && بیرون_pos >= بیرون_اندازه - MB_MAXBYTES)
	بیرون_flush();
}

#اگر_تعریف_شده FEAT_GUI
/*
 * out_trash(): Throw away the contents of the output buffer
 */
    فراغ
بیرون_trash(فراغ)
{
    بیرون_pos = 0;
}
#پایان_اگر

/*
 * out_char(c): put a byte into the output buffer.
 *		Flush it if it becomes full.
 * This should not be used for outputting text on the screen (use functions
 * like msg_puts() and screen_putchar() for that).
 */
    فراغ
بیرون_char(بیعلامت c)
{
#إذا defined(UNIX) || defined(VMS) || defined(AMIGA) || defined(MACOS_X)
    إذا (c == '\n')	// turn LF into CR-LF (CRMOD doesn't seem to do this)
	بیرون_char('\r');
#پایان_اگر

    بیرون_buf[بیرون_pos++] = c;

    // For testing we flush each time.
    إذا (بیرون_pos >= بیرون_اندازه || p_wd)
	بیرون_flush();
}

/*
 * Output "c" like out_char(), but don't flush when p_wd is set.
 */
    ساكن صحيح
بیرون_char_nf(صحيح c)
{
    بیرون_buf[بیرون_pos++] = (بیعلامت)c;

    إذا (بیرون_pos >= بیرون_اندازه)
	بیرون_flush();
    أرجع (بیعلامت)c;
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
    فراغ
بیرون_str_nf(char_u *s)
{
    // avoid terminal strings being split up
    إذا (بیرون_pos > بیرون_اندازه - MAX_ESC_SEQ_LEN)
	بیرون_flush();

    لكل (char_u *p = s; *p != NUL; ++p)
	بیرون_char_nf(*p);

    // For testing we write one string at a time.
    إذا (p_wd)
	بیرون_flush();
}

/*
 * A conditional-flushing out_str, mainly for visualbell.
 * Handles a delay internally, because termlib may not respect the delay or do
 * it at the wrong time.
 * Note: Only for terminal strings.
 */
    فراغ
بیرون_str_cf(char_u *s)
{
    إذا (s == عدم || *s == NUL)
	أرجع;

#اگر_تعریف_شده HAVE_TGETENT
    char_u *p;
#پایان_اگر

#اگر_تعریف_شده FEAT_GUI
    // Don't use tputs() when GUI is used, ncurses crashes.
    إذا (gui.درون_use)
    {
	بیرون_str_nf(s);
	أرجع;
    }
#پایان_اگر
    إذا (بیرون_pos > بیرون_اندازه - MAX_ESC_SEQ_LEN)
	بیرون_flush();
#اگر_تعریف_شده HAVE_TGETENT
    لكل (p = s; *s; ++s)
    {
	// flush just before delay command
	إذا (*s == '$' && *(s + 1) == '<')
	{
	    char_u save_c = *s;
	    صحيح duration = atoi((حرف *)s + 2);

	    *s = NUL;
	    tputs((حرف *)p, 1, TPUTSFUNCAST بیرون_char_nf);
	    *s = save_c;
	    بیرون_flush();
# ifdef ELAPSED_FUNC
	    // Only sleep here if we can limit this happening in
	    // vim_beep().
	    p = vim_strchr(s, '>');
	    إذا (p == عدم || duration <= 0)
	    {
		// can't parse the time, don't sleep here
		p = s;
	    }
	    وإلا
	    {
		++p;
		do_sleep(duration, FALSE);
	    }
# وإلا
	    // Rely on the terminal library to sleep.
	    p = s;
# endif
	    اكسر;
	}
    }
    tputs((حرف *)p, 1, TPUTSFUNCAST بیرون_char_nf);
#وإلا
    طالما (*s)
	بیرون_char_nf(*s++);
#پایان_اگر

    // For testing we write one string at a time.
    إذا (p_wd)
	بیرون_flush();
}

/*
 * out_str(s): Put a character string a byte at a time into the output buffer.
 * If HAVE_TGETENT is defined use tputs(), the termcap parser. (jw)
 * This should only be used for writing terminal codes, not for outputting
 * normal text (use functions like msg_puts() and screen_putchar() for that).
 */
    فراغ
بیرون_str(char_u *s)
{
    إذا (s == عدم || *s == NUL)
	أرجع;

#اگر_تعریف_شده FEAT_GUI
    // Don't use tputs() when GUI is used, ncurses crashes.
    إذا (gui.درون_use)
    {
	بیرون_str_nf(s);
	أرجع;
    }
#پایان_اگر
    // avoid terminal strings being split up
    إذا (بیرون_pos > بیرون_اندازه - MAX_ESC_SEQ_LEN)
	بیرون_flush();
#اگر_تعریف_شده HAVE_TGETENT
    tputs((حرف *)s, 1, TPUTSFUNCAST بیرون_char_nf);
#وإلا
    طالما (*s)
	بیرون_char_nf(*s++);
#پایان_اگر

    // For testing we write one string at a time.
    إذا (p_wd)
	بیرون_flush();
}

/*
 * cursor positioning using termcap parser. (jw)
 */
    فراغ
جمله_windgoto(صحيح ردیف, صحيح ستون)
{
    بیرون_STR(tgoto((حرف *)T_CM, ستون, ردیف));
}

    فراغ
جمله_cursor_راست(صحيح i)
{
    بیرون_STR(tgoto((حرف *)T_CRI, 0, i));
}

    فراغ
جمله_append_سطرها(صحيح سطر_شمار)
{
    بیرون_STR(tgoto((حرف *)T_CAL, 0, سطر_شمار));
}

    فراغ
جمله_delete_سطرها(صحيح سطر_شمار)
{
    بیرون_STR(tgoto((حرف *)T_CDL, 0, سطر_شمار));
}

#إذا defined(UNIX) || defined(VMS)
    فراغ
جمله_enable_mouse(صحيح enable)
{
    صحيح on = enable ? 1 : 0;
    بیرون_STR(tgoto((حرف *)T_CXM, 0, on));
}
#پایان_اگر

#إذا defined(HAVE_TGETENT)
    فراغ
جمله_مجموعه_winpos(صحيح x, صحيح y)
{
    // Can't handle a negative value here
    إذا (x < 0)
	x = 0;
    إذا (y < 0)
	y = 0;
    بیرون_STR(tgoto((حرف *)T_CWP, y, x));
}

# إذا defined(FEAT_TERMRESPONSE)
/*
 * Return TRUE if we can request the terminal for a response.
 */
    ساكن صحيح
can_get_termresponse(فراغ)
{
    أرجع cur_tmode == TMODE_RAW
	    && termcap_فعال
#  ifdef UNIX
	    && (is_not_a_جمله() || (isatty(1) && isatty(read_cmd_fd)))
#  endif
	    && p_ek;
}

/*
 * Set "status" to STATUS_SENT.
 */
    ساكن فراغ
termrequest_فرستاده(termrequest_T *وضعیت)
{
    وضعیت->tr_پیشرفت = وضعیت_فرستاده;
    وضعیت->tr_شروع = زمان(عدم);
}

/*
 * Return TRUE if any of the requests are in STATUS_SENT.
 */
    ساكن صحيح
termrequest_any_pending(فراغ)
{
    صحيح	    i;
    time_t  now = زمان(عدم);

    لكل (i = 0; all_termrequests[i] != عدم; ++i)
    {
	إذا (all_termrequests[i]->tr_پیشرفت == وضعیت_فرستاده)
	{
	    إذا (all_termrequests[i]->tr_شروع > 0 && now > 0
				    && all_termrequests[i]->tr_شروع + 2 < now)
		// Sent the request more than 2 seconds ago and didn't get a
		// response, assume it failed.
		all_termrequests[i]->tr_پیشرفت = وضعیت_FAIL;
	    وإلا
		أرجع TRUE;
	}
    }
    أرجع FALSE;
}

ساكن صحيح winpos_x = -1;
ساكن صحيح winpos_y = -1;
ساكن صحيح did_request_winpos = 0;

#  إذا defined(FEAT_EVAL) || defined(FEAT_TERMINAL)
/*
 * Try getting the Vim window position from the terminal.
 * Returns OK or FAIL.
 */
    صحيح
جمله_get_winpos(صحيح *x, صحيح *y, varnumber_T timeout)
{
    صحيح شمار = 0;
    صحيح prev_winpos_x = winpos_x;
    صحيح prev_winpos_y = winpos_y;

    إذا (*T_CGP == NUL || !can_get_termresponse())
	أرجع FAIL;
    winpos_x = -1;
    winpos_y = -1;
    ++did_request_winpos;
    termrequest_فرستاده(&winpos_وضعیت);
    بیرون_STR(T_CGP);
    بیرون_flush();

    // Try reading the result for "timeout" msec.
    طالما (شمار++ <= timeout / 10 && !got_int)
    {
	(فراغ)vpeekc_nomap();
	إذا (winpos_x >= 0 && winpos_y >= 0)
	{
	    *x = winpos_x;
	    *y = winpos_y;
	    أرجع OK;
	}
	ui_delay(10L, FALSE);
    }
    // Do not reset "did_request_winpos", if we timed out the response might
    // still come later and we must consume it.

    winpos_x = prev_winpos_x;
    winpos_y = prev_winpos_y;
    إذا (timeout < 10 && prev_winpos_y >= 0 && prev_winpos_x >= 0)
    {
	// Polling: return previous values if we have them.
	*x = winpos_x;
	*y = winpos_y;
	أرجع OK;
    }

    أرجع FALSE;
}
#  endif
# endif

    فراغ
جمله_مجموعه_winsize(صحيح ارتفاع, صحيح عرض)
{
    بیرون_STR(tgoto((حرف *)T_CWS, عرض, ارتفاع));
}
#پایان_اگر

    فراغ
جمله_font(صحيح n)
{
    إذا (*T_CFO)
    {
	حرف buf[20];
	sprintf(buf, (حرف *)T_CFO, 9 + n);
	بیرون_STR(buf);
    }
}

    ساكن فراغ
جمله_color(char_u *s, صحيح n)
{
    حرف	buf[20];
    صحيح		i = *s == CSI ? 1 : 2;
		// index in s[] just after <Esc>[ or CSI

    // Special handling of 16 colors, because termcap can't handle it
    // Also accept "\e[3%dm" for TERMINFO, it is sometimes used
    // Also accept CSI instead of <Esc>[
    إذا (n >= 8 && t_colors >= 16
	      && ((s[0] == ESC && s[1] == '[')
#إذا defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
		  || (s[0] == ESC && s[1] == '|')
#پایان_اگر
		  || (s[0] == CSI && (i = 1) == 1))
	      && s[i] != NUL
	      && (STRCMP(s + i + 1, "%p1%dm") == 0
		  || STRCMP(s + i + 1, "%dm") == 0)
	      && (s[i] == '3' || s[i] == '4'))
    {
#اگر_تعریف_شده TERMINFO
	حرف *format = "%s%s%%p1%%dm";
#وإلا
	حرف *format = "%s%s%%dm";
#پایان_اگر
	حرف *lead = i == 2 ? (
#إذا defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
		    s[1] == '|' ? "\033|" :
#پایان_اگر
		    "\033[") : "\233";
	حرف *دم = s[i] == '3' ? (n >= 16 ? "38;5;" : "9")
				 : (n >= 16 ? "48;5;" : "10");

	sprintf(buf, format, lead, دم);
	بیرون_STR(tgoto(buf, 0, n >= 16 ? n : n - 8));
    }
    وإلا
	بیرون_STR(tgoto((حرف *)s, 0, n));
}

    فراغ
جمله_fg_color(صحيح n)
{
    // Use "AF" termcap entry if present, "Sf" entry otherwise
    إذا (*T_CAF)
	جمله_color(T_CAF, n);
    وإلا إذا (*T_CSF)
	جمله_color(T_CSF, n);
}

    فراغ
جمله_bg_color(صحيح n)
{
    // Use "AB" termcap entry if present, "Sb" entry otherwise
    إذا (*T_CAB)
	جمله_color(T_CAB, n);
    وإلا إذا (*T_CSB)
	جمله_color(T_CSB, n);
}

    فراغ
جمله_ul_color(صحيح n)
{
    إذا (*T_CAU)
	جمله_color(T_CAU, n);
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
جمله_bg_default(فراغ)
{
#إذا defined(MSWIN)
    // DOS console is nearly always black
    أرجع (char_u *)"dark";
#وإلا
    char_u	*p;

    إذا (STRCMP(T_NAME, "linux") == 0
	    || STRCMP(T_NAME, "screen.linux") == 0
	    || STRNCMP(T_NAME, "cygwin", 6) == 0
	    || STRNCMP(T_NAME, "putty", 5) == 0
	    || ((p = mch_getenv((char_u *)"COLORFGBG")) != عدم
		&& (p = vim_strrchr(p, ';')) != عدم
		&& ((p[1] >= '0' && p[1] <= '6') || p[1] == '8')
		&& p[2] == NUL))
	أرجع (char_u *)"dark";
    أرجع (char_u *)"light";
#پایان_اگر
}

#إذا defined(FEAT_TERMGUICOLORS)

# define RED(rgb)   (((long_u)(rgb) >> 16) & 0xFF)
# define GREEN(rgb) (((long_u)(rgb) >>  8) & 0xFF)
# define BLUE(rgb)  (((long_u)(rgb)      ) & 0xFF)

    ساكن فراغ
جمله_rgb_color(char_u *s, guicolor_T rgb)
{
# define MAX_COLOR_STR_LEN 100
    حرف	buf[MAX_COLOR_STR_LEN];

    إذا (*s == NUL)
	أرجع;
    vim_snprintf(buf, MAX_COLOR_STR_LEN,
				  (حرف *)s, RED(rgb), GREEN(rgb), BLUE(rgb));
# ifdef FEAT_VTP
    إذا (use_vtp() && (p_tgc || t_colors >= 256))
    {
	بیرون_flush();
	buf[1] = '[';
	vtp_printf(buf);
    }
    وإلا
# endif
	بیرون_STR(buf);
}

    فراغ
جمله_fg_rgb_color(guicolor_T rgb)
{
    إذا (rgb != INVALCOLOR)
	جمله_rgb_color(T_8F, rgb);
}

    فراغ
جمله_bg_rgb_color(guicolor_T rgb)
{
    إذا (rgb != INVALCOLOR)
	جمله_rgb_color(T_8B, rgb);
}

    فراغ
جمله_ul_rgb_color(guicolor_T rgb)
{
# ifdef FEAT_TERMRESPONSE
    // If the user explicitly sets t_8u then use it.  Otherwise wait for
    // termresponse to be received, which is when t_8u would be set and a
    // redraw is needed if it was used.
    إذا (!option_was_مجموعه((char_u *)"t_8u") && write_t_8u_state != OK)
	write_t_8u_state = MAYBE;
    وإلا
# endif
	جمله_rgb_color(T_8U, rgb);
}
#پایان_اگر

#إذا defined(UNIX) || defined(VMS) || defined(MACOS_X)
/*
 * Generic function to set window title, using t_ts and t_fs.
 */
    فراغ
جمله_settitle(char_u *title)
{
    MAY_WANT_مقصد_LOG_THIS;

    // t_ts takes one argument: column in status line
    بیرون_STR(tgoto((حرف *)T_TS, 0, 0));	// set title start
    بیرون_str_nf(title);
    بیرون_str(T_FS);			// set title end
    بیرون_flush();
}

/*
 * Tell the terminal to push (save) the title and/or icon, so that it can be
 * popped (restored) later.
 */
    فراغ
جمله_افزودن_title(صحيح which)
{
    إذا ((which & SAVE_RESTORE_TITLE) && T_CST != عدم && *T_CST != NUL)
    {
	بیرون_STR(T_CST);
	بیرون_flush();
    }

    إذا ((which & SAVE_RESTORE_ICON) && T_SSI != عدم && *T_SSI != NUL)
    {
	بیرون_STR(T_SSI);
	بیرون_flush();
    }
}

/*
 * Tell the terminal to pop the title and/or icon.
 */
    فراغ
جمله_برداشت_title(صحيح which)
{
    إذا ((which & SAVE_RESTORE_TITLE) && T_CRT != عدم && *T_CRT != NUL)
    {
	بیرون_STR(T_CRT);
	بیرون_flush();
    }

    إذا ((which & SAVE_RESTORE_ICON) && T_SRI != عدم && *T_SRI != NUL)
    {
	بیرون_STR(T_SRI);
	بیرون_flush();
    }
}
#پایان_اگر

/*
 * Make sure we have a valid set or terminal options.
 * Replace all entries that are NULL by empty_option
 */
    فراغ
ttest(صحيح جفتها)
{
    char_u *env_colors;

    check_options();		    // make sure no options are NULL

    /*
     * MUST have "cm": cursor motion.
     */
    إذا (*T_CM == NUL)
	emsg(_(e_terminal_capability_cm_required));

    /*
     * if "cs" defined, use a scroll region, it's faster.
     */
    إذا (*T_CS != NUL)
	scroll_region = TRUE;
    وإلا
	scroll_region = FALSE;

    إذا (جفتها)
    {
	/*
	 * optional pairs
	 */
	// TP goes to normal mode for TI (invert) and TB (bold)
	إذا (*T_ME == NUL)
	    T_ME = T_MR = T_MD = T_MB = خالی_option;
	إذا (*T_SO == NUL || *T_SE == NUL)
	    T_SO = T_SE = خالی_option;
	إذا (*T_US == NUL || *T_UE == NUL)
	    T_US = T_UE = خالی_option;
	إذا (*T_CZH == NUL || *T_CZR == NUL)
	    T_CZH = T_CZR = خالی_option;

	// T_VE is needed even though T_VI is not defined
	إذا (*T_VE == NUL)
	    T_VI = خالی_option;

	// if 'mr' or 'me' is not defined use 'so' and 'se'
	إذا (*T_ME == NUL)
	{
	    T_ME = T_SE;
	    T_MR = T_SO;
	    T_MD = T_SO;
	}

	// if 'so' or 'se' is not defined use 'mr' and 'me'
	إذا (*T_SO == NUL)
	{
	    T_SE = T_ME;
	    إذا (*T_MR == NUL)
		T_SO = T_MD;
	    وإلا
		T_SO = T_MR;
	}

	// if 'ZH' or 'ZR' is not defined use 'mr' and 'me'
	إذا (*T_CZH == NUL)
	{
	    T_CZR = T_ME;
	    إذا (*T_MR == NUL)
		T_CZH = T_MD;
	    وإلا
		T_CZH = T_MR;
	}

	// "Sb" and "Sf" come in pairs
	إذا (*T_CSB == NUL || *T_CSF == NUL)
	{
	    T_CSB = خالی_option;
	    T_CSF = خالی_option;
	}

	// "AB" and "AF" come in pairs
	إذا (*T_CAB == NUL || *T_CAF == NUL)
	{
	    T_CAB = خالی_option;
	    T_CAF = خالی_option;
	}

	// if 'Sb' and 'AB' are not defined, reset "Co"
	إذا (*T_CSB == NUL && *T_CAB == NUL)
	    free_one_termoption(T_CCO);

	// Set 'weirdinvert' according to value of 't_xs'
	p_wiv = (*T_XS != NUL);
    }
    need_gather = TRUE;

    // Set t_colors to the value of $COLORS or t_Co.  Ignore $COLORS in the
    // GUI.
    t_colors = atoi((حرف *)T_CCO);
#اگر_تعریف_شده FEAT_GUI
    إذا (!gui.درون_use)
#پایان_اگر
    {
	env_colors = mch_getenv((char_u *)"COLORS");
	إذا (env_colors != عدم && امن_isdigit(*env_colors))
	{
	    صحيح colors = atoi((حرف *)env_colors);

	    إذا (colors != t_colors)
		مجموعه_color_شمار(colors);
	}
    }
    مجموعه_rgb_جمله_prop();
}

#إذا defined(FEAT_GUI) && (defined(FEAT_MENU) || !defined(USE_ON_FLY_SCROLL))
/*
 * Represent the given long_u as individual bytes, with the most significant
 * byte first, and store them in dst.
 */
    فراغ
جمع_کن_long_مقصد_buf(long_u val, char_u *dst)
{
    صحيح	    i;
    صحيح	    لغزش;

    لكل (i = 1; i <= (صحيح)حجم(long_u); i++)
    {
	لغزش = 8 * (حجم(long_u) - i);
	dst[i - 1] = (char_u) ((val >> لغزش) & 0xff);
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
    ساكن صحيح
get_long_from_buf(char_u *buf, long_u *val)
{
    صحيح	    len;
    char_u  بایتها[حجم(long_u)];
    صحيح	    i;
    صحيح	    لغزش;

    *val = 0;
    len = get_بایتها_from_buf(buf, بایتها, (صحيح)حجم(long_u));
    إذا (len == -1)
	أرجع -1;
    لكل (i = 0; i < (صحيح)حجم(long_u); i++)
    {
	لغزش = 8 * (حجم(long_u) - 1 - i);
	*val += (long_u)بایتها[i] << لغزش;
    }
    أرجع len;
}
#پایان_اگر

/*
 * Read the next num_bytes bytes from buf, and store them in bytes.  Assume
 * that buf has been through inchar().	Returns the actual number of bytes used
 * from buf (between num_bytes and num_bytes*2), or -1 if not enough bytes were
 * available.
 */
    صحيح
get_بایتها_from_buf(char_u *buf, char_u *بایتها, صحيح رقم_بایتها)
{
    صحيح	    len = 0;
    صحيح	    i;
    char_u  c;

    لكل (i = 0; i < رقم_بایتها; i++)
    {
	إذا ((c = buf[len++]) == NUL)
	    أرجع -1;
	إذا (c == K_SPECIAL)
	{
	    إذا (buf[len] == NUL || buf[len + 1] == NUL)	    // cannot happen?
		أرجع -1;
	    إذا (buf[len++] == (صحيح)KS_ZERO)
		c = NUL;
	    // else it should be KS_SPECIAL; when followed by KE_FILLER c is
	    // K_SPECIAL, or followed by KE_CSI and c must be CSI.
	    إذا (buf[len++] == (صحيح)KE_CSI)
		c = CSI;
	}
	وإلا إذا (c == CSI && buf[len] == KS_EXTRA
					       && buf[len + 1] == (صحيح)KE_CSI)
	    // CSI is stored as CSI KS_SPECIAL KE_CSI to avoid confusion with
	    // the start of a special key, see add_to_input_buf_csi().
	    len += 2;
	بایتها[i] = c;
    }
    أرجع len;
}

/*
 * Check if the new shell size is valid, correct it if it's too small or way
 * too big.
 */
    فراغ
check_shellsize(فراغ)
{
    // need room for one window and command line
    إذا (ردیفها < min_ردیفها_for_all_tabpages())
	ردیفها = min_ردیفها_for_all_tabpages();
    حد_screen_اندازه();

    // make sure these values are not invalid
    إذا (cmdline_ردیف >= ردیفها)
	cmdline_ردیف = ردیفها - 1;
    إذا (پیام_ردیف >= ردیفها)
	پیام_ردیف = ردیفها - 1;
}

/*
 * Limit Rows and Columns to avoid an overflow in Rows * Columns.
 */
    فراغ
حد_screen_اندازه(فراغ)
{
    إذا (Columns < MIN_COLUMNS)
	Columns = MIN_COLUMNS;
    وإلا إذا (Columns > 10000)
	Columns = 10000;
    إذا (ردیفها > 1000)
	ردیفها = 1000;
}

/*
 * Invoked just before the screen structures are going to be (re)allocated.
 */
    فراغ
win_new_shellsize(فراغ)
{
    ساكن صحيح	old_ردیفها_2 = 0;
    ساكن صحيح	old_Columns = 0;
    ساكن صحيح	old_coloff = 0;

    إذا (old_ردیفها_2 != ردیفها || old_Columns != COLUMNS_WITHOUT_TPL()
	    || old_coloff != TPL_LCOL())
	ui_new_shellsize();
    إذا (old_Columns != COLUMNS_WITHOUT_TPL() || old_coloff != TPL_LCOL())
    {
	old_Columns = COLUMNS_WITHOUT_TPL();
	old_coloff = TPL_LCOL();

	shell_new_columns();
    }
    إذا (old_ردیفها_2 != ردیفها)
    {
	// If 'window' uses the whole screen, keep it using that.
	// Don't change it when set with "-w size" on the command line.
	إذا (p_پنجره == old_ردیفها_2 - 1
		    || (old_ردیفها_2 == 0 && !option_was_مجموعه((char_u *)"window")))
	    p_پنجره = ردیفها - 1;
	old_ردیفها_2 = ردیفها;
	shell_new_ردیفها();	// update window sizes
    }
}

#إذا defined(FEAT_IMAGE) || defined(FEAT_EVAL)
    فراغ
update_cell_اندازه(فراغ)
{
# ifdef FEAT_GUI
    إذا (gui.درون_use)
    {
	cell_عرض = gui.char_عرض;
	cell_ارتفاع = gui.char_ارتفاع;
    }
    وإلا
# endif
    {
	بنية cellsize cell_sz;

	mch_calc_cell_اندازه(&cell_sz);
	إذا (cell_sz.cs_xpixel <= 0 || cell_sz.cs_ypixel <= 0)
	{
	    cell_عرض = 8;
	    cell_ارتفاع = 16;
	}
	وإلا
	{
	    cell_عرض = cell_sz.cs_xpixel;
	    cell_ارتفاع = cell_sz.cs_ypixel;
	}
    }
}
#پایان_اگر

/*
 * Call this function when the Vim shell has been resized in any way.
 * Will obtain the current size and redraw (also when size didn't change).
 */
    فراغ
shell_resized(فراغ)
{
#إذا defined(FEAT_IMAGE) || defined(FEAT_EVAL)
    update_cell_اندازه();
#پایان_اگر
    مجموعه_shellsize(0, 0, FALSE);
}

/*
 * Check if the shell size changed.  Handle a resize.
 * When the size didn't change, nothing happens.
 */
    فراغ
shell_resized_check(فراغ)
{
    صحيح		old_ردیفها_2 = ردیفها;
    صحيح		old_Columns = Columns;

    إذا (exiting
#اگر_تعریف_شده FEAT_GUI
	    // Do not get the size when executing a shell command during
	    // startup.
	    || gui.starting
#پایان_اگر
	    )
	أرجع;

    (فراغ)ui_get_shellsize();
    check_shellsize();
    إذا (old_ردیفها_2 != ردیفها || old_Columns != Columns)
	shell_resized();
}

/*
 * Set size of the Vim shell.
 * If 'mustset' is TRUE, we must set Rows and Columns, do not get the real
 * window size (this is used for the :win command).
 * If 'mustset' is FALSE, we may try to get the real window size and if
 * it fails use 'width' and 'height'.
 */
    ساكن فراغ
مجموعه_shellsize_inner(صحيح عرض, صحيح ارتفاع, صحيح mustset)
{
    إذا (updating_screen)
	// resizing while in update_screen() may cause a crash
	أرجع;

    // curwin->w_buffer can be NULL when we are closing a window and the
    // buffer (or window) has already been closed and removing a scrollbar
    // causes a resize event. Don't resize then, it will happen after entering
    // another buffer.
    إذا (curwin->w_میانگیر == عدم || curwin->w_سطرها == عدم)
	أرجع;

#اگر_تعریف_شده AMIGA
    بیرون_flush();	    // must do this before mch_get_shellsize() for
			    // some obscure reason
#پایان_اگر

    إذا (mustset || (ui_get_shellsize() == FAIL && ارتفاع != 0))
    {
	ردیفها = ارتفاع;
	Columns = عرض;
	check_shellsize();
	ui_مجموعه_shellsize(mustset);
    }
    وإلا
	check_shellsize();

    // The window layout used to be adjusted here, but it now happens in
    // screenalloc() (also invoked from screenclear()).  That is because the
    // "busy" check above may skip this, but not screenalloc().

    إذا (State != نما_ASKMORE && State != نما_EXTERNCMD
						      && State != نما_CONFIRM)
	screenclear();
    وإلا
	screen_شروع();	    // don't know where cursor is now

    إذا (starting != NO_SCREEN)
    {
	maketitle();

	تغییریافته_سطر_abv_curs();
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
	إذا (State == نما_ASKMORE || State == نما_EXTERNCMD
				     || State == نما_CONFIRM || exmode_فعال)
	{
	    screenalloc(FALSE);
	    repeat_پیام();
	}
	وإلا
	{
	    إذا (curwin->w_p_scb)
		do_check_scrollbind(TRUE);
	    إذا (State & نما_CMDLINE)
	    {
		update_screen(UPD_NOT_VALID);
		redrawcmdline();
	    }
	    وإلا
	    {
		update_topline();
		إذا (pum_visible())
		{
		    redraw_later(UPD_NOT_VALID);
		    ins_compl_show_pum();
		}
		update_screen(UPD_NOT_VALID);
		إذا (redrawing())
		    setcursor();
	    }
	}
	cursor_on();	    // redrawing may have switched it off
    }
    بیرون_flush();
}

    فراغ
مجموعه_shellsize(صحيح عرض, صحيح ارتفاع, صحيح mustset)
{
    ساكن صحيح	busy = FALSE;
    ساكن صحيح	do_پیاپی = FALSE;

    إذا (عرض < 0 || ارتفاع < 0)    // just checking...
	أرجع;

    إذا (State == نما_HITRETURN || State == نما_SETWSIZE)
    {
	// postpone the resizing
	State = نما_SETWSIZE;
	أرجع;
    }

    // Avoid recursiveness.  This can happen when setting the window size
    // causes another window-changed signal or when two SIGWINCH signals come
    // very close together.  There needs to be another run then after the
    // current one is done to pick up the latest size.
    do_پیاپی = TRUE;
    إذا (busy)
	أرجع;

    طالما (do_پیاپی)
    {
	do_پیاپی = FALSE;
	busy = TRUE;
	مجموعه_shellsize_inner(عرض, ارتفاع, mustset);
	busy = FALSE;
    }
}

/*
 * Output T_CTE, the t_TE termcap entry, and handle expected effects.
 * The code possibly disables modifyOtherKeys and the Kitty keyboard protocol.
 */
    فراغ
بیرون_str_t_TE(فراغ)
{
    بیرون_str(T_CTE);

    // The seenModifyOtherKeys flag is not reset here.  We do expect t_TE to
    // disable modifyOtherKeys, but until Xterm version 377 there is no way to
    // detect it's enabled again after the following t_TI.  We assume that when
    // seenModifyOtherKeys was set before it will still be valid.

    // When the modifyOtherKeys level is detected to be 2 we expect t_TE to
    // disable it.  Remembering that it was detected to be enabled is useful in
    // some situations.
    // The following t_TI is expected to request the state and then
    // modify_otherkeys_state will be set again.
    إذا (modify_otherkeys_state == MOKS_ENABLED
	    || modify_otherkeys_state == MOKS_DISABLED)
	modify_otherkeys_state = MOKS_DISABLED;
    وإلا إذا (modify_otherkeys_state != MOKS_INITIAL)
	modify_otherkeys_state = MOKS_AFTER_T_TE;

    // When the kitty keyboard protocol is enabled we expect t_TE to disable
    // it.  Remembering that it was detected to be enabled is useful in some
    // situations.
    // The following t_TI is expected to request the state and then
    // kitty_protocol_state will be set again.
    إذا (kitty_protocol_state == KKPS_ENABLED
	    || kitty_protocol_state == KKPS_DISABLED)
	kitty_protocol_state = KKPS_DISABLED;
    وإلا
	kitty_protocol_state = KKPS_AFTER_T_TE;
}

ساكن صحيح send_t_RK = FALSE;

/*
 * Output T_TI and setup for what follows.
 */
    فراغ
بیرون_str_t_TI(فراغ)
{
    بیرون_str(T_CTI);

    // Send t_RK when there is no more work to do.
    send_t_RK = TRUE;
}

/*
 * Output T_BE, but only when t_PS and t_PE are set.
 */
    فراغ
بیرون_str_t_BE(فراغ)
{
    char_u *p;

    إذا (T_BE == عدم || *T_BE == NUL
	    || (p = پیدا_کن_termcode((char_u *)"PS")) == عدم || *p == NUL
	    || (p = پیدا_کن_termcode((char_u *)"PE")) == عدم || *p == NUL)
	أرجع;
    بیرون_str(T_BE);
}

/*
 * If t_TI was recently sent and there is no typeahead or work to do, now send
 * t_RK.  This is postponed to avoid the response arriving in a shell command
 * or after Vim exits.
 */
    فراغ
may_send_t_RK(فراغ)
{
    إذا (send_t_RK
	    && !work_pending()
	    && !ex_نرمال_busy
#اگر_تعریف_شده FEAT_EVAL
	    && !درون_feedkeys
#پایان_اگر
	    && !exiting)
    {
	send_t_RK = FALSE;
	بیرون_str(T_CRK);
	بیرون_flush();
    }
}

/*
 * Set the terminal to TMODE_RAW (for Normal mode) or TMODE_COOK (for external
 * commands and Ex mode).
 */
    فراغ
settmode(tmode_T tmode)
{
#اگر_تعریف_شده FEAT_GUI
    // don't set the term where gvim was started to any mode
    إذا (gui.درون_use)
	أرجع;
#پایان_اگر

    إذا (!پر_screen)
	أرجع;

    /*
     * When returning after calling a shell cur_tmode is TMODE_UNKNOWN,
     * set the terminal to raw mode, even though we think it already is,
     * because the shell program may have reset the terminal mode.
     * When we think the terminal is normal, don't try to set it to
     * normal again, because that causes problems (logout!) on some
     * machines.
     */
    إذا (tmode != cur_tmode)
    {
#اگر_تعریف_شده FEAT_TERMRESPONSE
# ifdef FEAT_GUI
	إذا (!gui.درون_use && !gui.starting)
# endif
	{
	    // May need to check for T_CRV response and termcodes, it
	    // doesn't work in Cooked mode, an external program may get
	    // them.
	    إذا (tmode != TMODE_RAW && termrequest_any_pending())
		(فراغ)vpeekc_nomap();
	    check_for_codes_from_جمله();
	}
#پایان_اگر
	إذا (tmode != TMODE_RAW)
	    mch_setmouse(FALSE);	// switch mouse off

	// Disable bracketed paste and modifyOtherKeys in cooked mode.
	// Avoid doing this too often, on some terminals the codes are not
	// handled properly.
	إذا (termcap_فعال && tmode != TMODE_SLEEP
		&& cur_tmode != TMODE_SLEEP)
	{
	    MAY_WANT_مقصد_LOG_THIS;

	    إذا (tmode != TMODE_RAW)
	    {
		بیرون_str(T_BD);	// disable bracketed paste mode
		بیرون_str_t_TE();	// possibly disables modifyOtherKeys
	    }
	    وإلا
	    {
		بیرون_str_t_BE();	// enable bracketed paste mode (should
				// be before mch_settmode().
		بیرون_str_t_TI();	// possibly enables modifyOtherKeys
	    }
	}
	بیرون_flush();
	mch_settmode(tmode);	// machine specific function
	cur_tmode = tmode;
	إذا (tmode == TMODE_RAW)
	    setmouse();		// may switch mouse on
	بیرون_flush();
    }
#اگر_تعریف_شده FEAT_TERMRESPONSE
    may_req_termresponse();
#پایان_اگر
}

    فراغ
starttermcap(فراغ)
{
    إذا (!پر_screen || termcap_فعال)
	أرجع;

    MAY_WANT_مقصد_LOG_THIS;

    بیرون_str(T_TI);			// start termcap mode
    بیرون_str_t_TI();			// start "raw" mode
    بیرون_str(T_KS);			// start "keypad transmit" mode
    بیرون_str_t_BE();			// enable bracketed paste mode

#إذا defined(UNIX) || defined(VMS)
    // Enable xterm's focus reporting mode when 'esckeys' is set.
    إذا (p_ek && *T_FE != NUL)
	بیرون_str(T_FE);
#پایان_اگر

    بیرون_flush();
    termcap_فعال = TRUE;
    screen_شروع();			// don't know where cursor is now
#اگر_تعریف_شده FEAT_TERMRESPONSE
# ifdef FEAT_GUI
    إذا (!gui.درون_use && !gui.starting)
# endif
    {
	may_req_termresponse();
	// Immediately check for a response.  If t_Co changes, we don't
	// want to redraw with wrong colors first.
	إذا (crv_وضعیت.tr_پیشرفت == وضعیت_فرستاده)
	    check_for_codes_from_جمله();
    }
#پایان_اگر
}

    فراغ
stoptermcap(فراغ)
{
    screen_stop_highlight();
    reset_cterm_colors();

    إذا (!termcap_فعال)
	أرجع;

#اگر_تعریف_شده FEAT_TERMRESPONSE
# ifdef FEAT_GUI
    إذا (!gui.درون_use && !gui.starting)
# endif
    {
	// May need to discard T_CRV, T_U7 or T_RBG response.
	إذا (termrequest_any_pending())
	{
# ifdef UNIX
	    // Give the terminal a chance to respond.
	    mch_delay(100L, 0);
# endif
# ifdef TCIFLUSH
	    // Discard data received but not read.
	    إذا (exiting)
		tcflush(fileno(stdin), TCIFLUSH);
# endif
	}
	// Check for termcodes first, otherwise an external program may
	// get them.
	check_for_codes_from_جمله();
    }
#پایان_اگر
    MAY_WANT_مقصد_LOG_THIS;

#إذا defined(UNIX) || defined(VMS)
    // Disable xterm's focus reporting mode if 'esckeys' is set.
    إذا (p_ek && *T_FD != NUL)
	بیرون_str(T_FD);
#پایان_اگر

    بیرون_str(T_BD);			// disable bracketed paste mode
    بیرون_str(T_KE);			// stop "keypad transmit" mode
    بیرون_flush();
    termcap_فعال = FALSE;

    // Output t_te before t_TE, t_te may switch between main and alternate
    // screen and following codes may work on the active screen only.
    //
    // When using the Kitty keyboard protocol the main and alternate screen
    // use a separate state.  If we are (or were) using the Kitty keyboard
    // protocol and t_te is not empty (possibly switching screens) then
    // output t_TE both before and after outputting t_te.
    إذا (*T_TE != NUL && (kitty_protocol_state == KKPS_ENABLED
		|| kitty_protocol_state == KKPS_DISABLED))
	بیرون_str_t_TE();		// probably disables the kitty keyboard
				// protocol

    بیرون_str(T_TE);			// stop termcap mode
    cursor_on();			// just in case it is still off
    بیرون_str_t_TE();			// stop "raw" mode, modifyOtherKeys and
					// Kitty keyboard protocol
    screen_شروع();			// don't know where cursor is now
    بیرون_flush();
}

#إذا defined(FEAT_TERMRESPONSE)
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
    فراغ
may_req_termresponse(فراغ)
{
    إذا (crv_وضعیت.tr_پیشرفت == وضعیت_GET
	    && can_get_termresponse()
	    && starting == 0
	    && *T_CRV != NUL)
    {
	MAY_WANT_مقصد_LOG_THIS;
	LOG_TR1("Sending CRV request");
	بیرون_str(T_CRV);
	termrequest_فرستاده(&crv_وضعیت);
	// check for the characters now, otherwise they might be eaten by
	// get_keystroke()
	بیرون_flush();
	(فراغ)vpeekc_nomap();
    }
}

/*
 * Send sequences to the terminal and check with t_u7 how the cursor moves, to
 * find out properties of the terminal.
 * Note that this goes out before T_CRV, so that the result can be used when
 * the termresponse arrives.
 */
    فراغ
check_terminal_behavior(فراغ)
{
    صحيح	    did_send = FALSE;

    إذا (!can_get_termresponse() || starting != 0 || *T_U7 == NUL)
	أرجع;

    إذا (u7_وضعیت.tr_پیشرفت == وضعیت_GET
	    && !option_was_مجموعه((char_u *)"ambiwidth"))
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
	جمله_windgoto(1, 0);
	buf[mb_char2bytes(0x25bd, buf)] = NUL;
	بیرون_str(buf);
	بیرون_str(T_U7);
	termrequest_فرستاده(&u7_وضعیت);
	بیرون_flush();
	did_send = TRUE;

	// This overwrites a few characters on the screen, a redraw is needed
	// after this. Clear them out for now.
	screen_stop_highlight();
	جمله_windgoto(1, 0);
	بیرون_str((char_u *)"  ");
	سطر_was_clobbered(1);
    }

    إذا (xcc_وضعیت.tr_پیشرفت == وضعیت_GET && ردیفها > 2)
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
	جمله_windgoto(2, 0);
	// send the test DCS string.
	بیرون_str((char_u *)"\033Pzz\033\\");
	// send the test CSI sequence with intermediate byte.
	بیرون_str((char_u *)"\033[0%m");
	بیرون_str(T_U7);
	termrequest_فرستاده(&xcc_وضعیت);
	بیرون_flush();
	did_send = TRUE;

	// If the terminal handles test sequence incorrectly, garbage text is
	// displayed. Clear them out for now.
	screen_stop_highlight();
	جمله_windgoto(2, 0);
	بیرون_str((char_u *)"           ");
	سطر_was_clobbered(2);
    }

    إذا (did_send)
    {
	جمله_windgoto(0, 0);

	// Need to reset the known cursor position.
	screen_شروع();

	// check for the characters now, otherwise they might be eaten by
	// get_keystroke()
	بیرون_flush();
	(فراغ)vpeekc_nomap();
    }
}

/*
 * Similar to requesting the version string: Request the terminal background
 * color when it is the right moment.
 */
    فراغ
may_req_bg_color(فراغ)
{
    إذا (can_get_termresponse() && starting == 0)
    {
	صحيح didit = FALSE;

# ifdef FEAT_TERMINAL
	// Only request foreground if t_RF is set.
	إذا (rfg_وضعیت.tr_پیشرفت == وضعیت_GET && *T_RFG != NUL)
	{
	    MAY_WANT_مقصد_LOG_THIS;
	    LOG_TR1("Sending FG request");
	    بیرون_str(T_RFG);
	    termrequest_فرستاده(&rfg_وضعیت);
	    didit = TRUE;
	}
# endif

	// Only request background if t_RB is set.
	إذا (rbg_وضعیت.tr_پیشرفت == وضعیت_GET && *T_RBG != NUL)
	{
	    MAY_WANT_مقصد_LOG_THIS;
	    LOG_TR1("Sending BG request");
	    بیرون_str(T_RBG);
	    termrequest_فرستاده(&rbg_وضعیت);
	    didit = TRUE;
	}

	إذا (didit)
	{
	    // check for the characters now, otherwise they might be eaten by
	    // get_keystroke()
	    بیرون_flush();
	    (فراغ)vpeekc_nomap();
	}
    }
}

#پایان_اگر

/*
 * Return TRUE when saving and restoring the screen.
 */
    صحيح
swapping_screen(فراغ)
{
    أرجع (پر_screen && *T_TI != NUL);
}

/*
 * By outputting the 'cursor very visible' termcap code, for some windowed
 * terminals this makes the screen scrolled to the correct position.
 * Used when starting Vim or returning from a shell.
 */
    فراغ
scroll_شروع(فراغ)
{
    إذا (*T_VS == NUL || *T_CVS == NUL)
	أرجع;

    MAY_WANT_مقصد_LOG_THIS;
    بیرون_str(T_VS);
    بیرون_str(T_CVS);
    screen_شروع();		// don't know where cursor is now
}

// True if cursor is not visible
ساكن صحيح cursor_is_off = FALSE;

// True if cursor is not visible due to an ongoing cursor-less sleep
ساكن صحيح cursor_is_asleep = FALSE;

/*
 * Enable the cursor without checking if it's already enabled.
 */
    فراغ
cursor_on_force(فراغ)
{
    بیرون_str(T_VE);
    cursor_is_off = FALSE;
    cursor_is_asleep = FALSE;
}

/*
 * Enable the cursor if it's currently off.
 */
    فراغ
cursor_on(فراغ)
{
    إذا (cursor_is_off && !cursor_is_asleep)
	cursor_on_force();
}

/*
 * Disable the cursor.
 */
    فراغ
cursor_off(فراغ)
{
    إذا (پر_screen && !cursor_is_off)
    {
	بیرون_str(T_VI);	    // disable cursor
	cursor_is_off = TRUE;
    }
}

#اگر_تعریف_شده FEAT_GUI
/*
 * Check whether the cursor is invisible due to an ongoing cursor-less sleep
 */
    صحيح
cursor_is_sleeping(فراغ)
{
    أرجع cursor_is_asleep;
}
#پایان_اگر

/*
 * Disable the cursor and mark it disabled by cursor-less sleep
 */
    فراغ
cursor_sleep(فراغ)
{
    cursor_is_asleep = TRUE;
    cursor_off();
}

/*
 * Enable the cursor and mark it not disabled by cursor-less sleep
 */
    فراغ
cursor_unsleep(فراغ)
{
    cursor_is_asleep = FALSE;
    cursor_on();
}

#إذا defined(CURSOR_SHAPE)
/*
 * Set cursor shape to match Insert or Replace mode.
 */
    فراغ
جمله_cursor_نما(صحيح forced)
{
    ساكن صحيح showing_نما = -1;
    char_u *p;

    // Only do something when redrawing the screen and we can restore the
    // mode.
    إذا (!پر_screen || *T_CEI == NUL)
    {
# ifdef FEAT_TERMRESPONSE
	إذا (forced && initial_cursor_shape > 0)
	    // Restore to initial values.
	    جمله_cursor_shape(initial_cursor_shape, initial_cursor_blink);
# endif
	أرجع;
    }

    إذا ((State & نما_REPLACE) == نما_REPLACE)
    {
	إذا (forced || showing_نما != نما_REPLACE)
	{
	    إذا (*T_CSR != NUL)
		p = T_CSR;	// Replace mode cursor
	    وإلا
		p = T_CSI;	// fall back to Insert mode cursor
	    إذا (*p != NUL)
	    {
		بیرون_str(p);
		showing_نما = نما_REPLACE;
	    }
	}
    }
    وإلا إذا (State & نما_درج)
    {
	إذا ((forced || showing_نما != نما_درج) && *T_CSI != NUL)
	{
	    بیرون_str(T_CSI);	    // Insert mode cursor
	    showing_نما = نما_درج;
	}
    }
    وإلا إذا (forced || showing_نما != نما_نرمال)
    {
	بیرون_str(T_CEI);		    // non-Insert mode cursor
	showing_نما = نما_نرمال;
    }
}

# إذا defined(FEAT_TERMINAL)
    فراغ
جمله_cursor_color(char_u *color)
{
    إذا (*T_CSC == NUL)
	أرجع;

    بیرون_str(T_CSC);		// set cursor color start
    بیرون_str_nf(color);
    بیرون_str(T_CEC);		// set cursor color end
    بیرون_flush();
}
# endif

    صحيح
blink_state_is_inverted(فراغ)
{
# ifdef FEAT_TERMRESPONSE
    أرجع rbm_وضعیت.tr_پیشرفت == وضعیت_GOT
	&& rcs_وضعیت.tr_پیشرفت == وضعیت_GOT
		&& initial_cursor_blink != initial_cursor_shape_blink;
# وإلا
    أرجع FALSE;
# endif
}

/*
 * "shape": 1 = block, 2 = underline, 3 = vertical bar
 */
    فراغ
جمله_cursor_shape(صحيح shape, صحيح blink)
{
    إذا (*T_CSH != NUL)
    {
	بیرون_STR(tgoto((حرف *)T_CSH, 0, shape * 2 - blink));
	بیرون_flush();
    }
    وإلا
    {
	صحيح do_blink = blink;

	// t_SH is empty: try setting just the blink state.
	// The blink flags are XORed together, if the initial blinking from
	// style and shape differs, we need to invert the flag here.
	إذا (blink_state_is_inverted())
	    do_blink = !blink;

	إذا (do_blink && *T_VS != NUL)
	{
	    بیرون_str(T_VS);
	    بیرون_flush();
	}
	وإلا إذا (!do_blink && *T_CVS != NUL)
	{
	    بیرون_str(T_CVS);
	    بیرون_flush();
	}
    }
}
#پایان_اگر

/*
 * Set scrolling region for window 'wp'.
 * The region starts 'off' lines from the start of the window.
 * Also set the vertical scroll region for a vertically split window.  Always
 * the full width of the window, excluding the vertical separator.
 */
    فراغ
scroll_region_مجموعه(win_T *wp, صحيح off)
{
    بیرون_STR(tgoto((حرف *)T_CS, W_WINROW(wp) + wp->w_ارتفاع - 1,
							 W_WINROW(wp) + off));
    إذا (*T_CSV != NUL && wp->w_عرض != Columns)
	بیرون_STR(tgoto((حرف *)T_CSV, wp->w_wincol + wp->w_عرض - 1,
							       wp->w_wincol));
    screen_شروع();		    // don't know where cursor is now
}

/*
 * Reset scrolling region to the whole screen.
 */
    فراغ
scroll_region_reset(فراغ)
{
    بیرون_STR(tgoto((حرف *)T_CS, (صحيح)ردیفها - 1, 0));
    إذا (*T_CSV != NUL)
	بیرون_STR(tgoto((حرف *)T_CSV, (صحيح)Columns - 1, 0));
    screen_شروع();		    // don't know where cursor is now
}


/*
 * List of terminal codes that are currently recognized.
 */

ساكن بنية termcode
{
    char_u  name[2];	    // termcap name of entry
    char_u  *کد;	    // terminal code (in allocated memory)
    صحيح	    len;	    // STRLEN(code)
    صحيح	    modlen;	    // length of part before ";*~".
} *termcodes = عدم;

ساكن صحيح  tc_max_len = 0; // number of entries that termcodes[] can hold
ساكن صحيح  tc_len = 0;	    // current number of entries in termcodes[]

ساكن صحيح termcode_ستاره(char_u *کد, صحيح len);

    فراغ
clear_termcodes(فراغ)
{
    طالما (tc_len > 0)
	vim_free(termcodes[--tc_len].کد);
    VIM_CLEAR(termcodes);
    tc_max_len = 0;

#اگر_تعریف_شده HAVE_TGETENT
    BC = (حرف *)خالی_option;
    به_بالا_2 = (حرف *)خالی_option;
    PC = NUL;			// set pad character to NUL
    ospeed = 0;
#پایان_اگر

    need_gather = TRUE;		// need to fill termleader[]
}

#تعریف_کن ATC_FROM_جمله 55

/*
 * For xterm we recognize special codes like "ESC[42;*X" and "ESC O*X" that
 * accept modifiers.
 * Set "termcodes[idx].modlen".
 */
    ساكن فراغ
adjust_modlen(صحيح idx)
{
    termcodes[idx].modlen = 0;
    صحيح j = termcode_ستاره(termcodes[idx].کد, termcodes[idx].len);
    إذا (j <= 0)
	أرجع;

    termcodes[idx].modlen = termcodes[idx].len - 1 - j;
    // For "CSI[@;X" the "@" is not included in "modlen".
    إذا (termcodes[idx].کد[termcodes[idx].modlen - 1] == '@')
	--termcodes[idx].modlen;
}

/*
 * Add a new entry for "name[2]" to the list of terminal codes.
 * Note that "name" may not have a terminating NUL.
 * The list is kept alphabetical for ":set termcap"
 * "flags" is TRUE when replacing 7-bit by 8-bit controls is desired.
 * "flags" can also be ATC_FROM_TERM for got_code_from_term().
 */
    فراغ
جمع_کن_termcode(char_u *name, char_u *string, صحيح flags)
{
    بنية termcode *new_tc;
    صحيح		    i, j;
    char_u	    *s;
    صحيح		    len;
#اگر_تعریف_شده FEAT_EVAL
    حرف	    *action = "Setting";
#پایان_اگر

    إذا (string == عدم || *string == NUL)
    {
	del_termcode(name);
	أرجع;
    }

#إذا defined(MSWIN) && !defined(FEAT_GUI)
    s = vim_strnsave(string, STRLEN(string) + 1);
#وإلا
# ifdef VIMDLL
    إذا (!gui.درون_use)
	s = vim_strnsave(string, STRLEN(string) + 1);
    وإلا
# endif
	s = vim_strsave(string);
#پایان_اگر
    إذا (s == عدم)
	أرجع;

    // Change leading <Esc>[ to CSI, change <Esc>O to <M-O>.
    إذا (flags != 0 && flags != ATC_FROM_جمله && جمله_7to8bit(string) != 0)
    {
	STRMOVE(s, s + 1);
	s[0] = جمله_7to8bit(string);
    }

#إذا defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
# ifdef VIMDLL
    إذا (!gui.درون_use)
# endif
    {
	إذا (s[0] == K_NUL)
	{
	    STRMOVE(s + 1, s);
	    s[1] = 3;
	}
    }
#پایان_اگر

    len = (صحيح)STRLEN(s);

    need_gather = TRUE;		// need to fill termleader[]

    /*
     * need to make space for more entries
     */
    إذا (tc_len == tc_max_len)
    {
	tc_max_len += 20;
	new_tc = ALLOC_MULT(بنية termcode, tc_max_len);
	إذا (new_tc == عدم)
	{
	    tc_max_len -= 20;
	    vim_free(s);
	    أرجع;
	}
	لكل (i = 0; i < tc_len; ++i)
	    new_tc[i] = termcodes[i];
	vim_free(termcodes);
	termcodes = new_tc;
    }

    /*
     * Look for existing entry with the same name, it is replaced.
     * Look for an existing entry that is alphabetical higher, the new entry
     * is inserted in front of it.
     */
    لكل (i = 0; i < tc_len; ++i)
    {
	إذا (termcodes[i].name[0] < name[0])
	    استمر;
	إذا (termcodes[i].name[0] == name[0])
	{
	    إذا (termcodes[i].name[1] < name[1])
		استمر;
	    /*
	     * Exact match: May replace old code.
	     */
	    إذا (termcodes[i].name[1] == name[1])
	    {
		إذا (flags == ATC_FROM_جمله && (j = termcode_ستاره(
				    termcodes[i].کد, termcodes[i].len)) > 0)
		{
		    // Don't replace ESC[123;*X or ESC O*X with another when
		    // invoked from got_code_from_term().
		    إذا (len == termcodes[i].len - j
			    && STRNCMP(s, termcodes[i].کد, len - 1) == 0
			    && s[len - 1]
				   == termcodes[i].کد[termcodes[i].len - 1])
		    {
			// They are equal but for the ";*": don't add it.
#اگر_تعریف_شده FEAT_EVAL
			ch_log(عدم, "Termcap entry %c%c did not change",
							     name[0], name[1]);
#پایان_اگر
			vim_free(s);
			أرجع;
		    }
		}
		وإلا
		{
		    // Replace old code.
#اگر_تعریف_شده FEAT_EVAL
		    ch_log(عدم, "Termcap entry %c%c was: %s",
					  name[0], name[1], termcodes[i].کد);
#پایان_اگر
		    vim_free(termcodes[i].کد);
		    --tc_len;
		    اكسر;
		}
	    }
	}
	/*
	 * Found alphabetical larger entry, move rest to insert new entry
	 */
#اگر_تعریف_شده FEAT_EVAL
	action = "Adding";
#پایان_اگر
	لكل (j = tc_len; j > i; --j)
	    termcodes[j] = termcodes[j - 1];
	اكسر;
    }

#اگر_تعریف_شده FEAT_EVAL
    ch_log(عدم, "%s termcap entry %c%c to %s", action, name[0], name[1], s);
#پایان_اگر
    termcodes[i].name[0] = name[0];
    termcodes[i].name[1] = name[1];
    termcodes[i].کد = s;
    termcodes[i].len = len;
    adjust_modlen(i);

    ++tc_len;
}

/*
 * Some function keys may include modifiers, but the terminfo/termcap entries
 * do not indicate that.  Insert ";*" where we expect modifiers might appear.
 */
    ساكن فراغ
accept_modifiers_for_function_کلیدها(فراغ)
{
    regmatch_T regmatch;
    CLEAR_فیلد(regmatch);
    regmatch.rm_ic = TRUE;
    regmatch.regprog = vim_regcomp((char_u *)"^\033[\\d\\+\\~$", RE_MAGIC);

    لكل (صحيح i = 0; i < tc_len; ++i)
    {
	إذا (regmatch.regprog == عدم)
	    أرجع;

	// skip PasteStart and PasteEnd
	إذا (termcodes[i].name[0] == 'P'
		&& (termcodes[i].name[1] == 'S' || termcodes[i].name[1] == 'E'))
	    استمر;

	char_u *s = termcodes[i].کد;
	إذا (s != عدم && vim_regexec(&regmatch, s, (colnr_T)0))
	{
	    size_t len = STRLEN(s);
	    char_u *ns = alloc(len + 3);
	    إذا (ns != عدم)
	    {
		mch_memmove(ns, s, len - 1);
		mch_memmove(ns + len - 1, ";*~", 4);
		vim_free(s);
		termcodes[i].کد = ns;
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
    ساكن صحيح
termcode_ستاره(char_u *کد, صحيح len)
{
    // Shortest is <M-O>*X.  With ; shortest is <CSI>@;*X
    إذا (len >= 3 && کد[len - 2] == '*')
    {
	إذا (len >= 5 && کد[len - 3] == ';')
	    أرجع 2;
	وإلا
	    أرجع 1;
    }
    أرجع 0;
}

    char_u  *
پیدا_کن_termcode(char_u *name)
{
    صحيح	    i;

    لكل (i = 0; i < tc_len; ++i)
	إذا (termcodes[i].name[0] == name[0] && termcodes[i].name[1] == name[1])
	    أرجع termcodes[i].کد;
    أرجع عدم;
}

    char_u *
get_termcode(صحيح i)
{
    إذا (i >= tc_len)
	أرجع عدم;
    أرجع &termcodes[i].name[0];
}

/*
 * Returns the length of the terminal code at index 'idx'.
 */
    صحيح
get_termcode_len(صحيح idx)
{
    أرجع termcodes[idx].len;
}

    فراغ
del_termcode(char_u *name)
{
    صحيح	    i;

    إذا (termcodes == عدم)	// nothing there yet
	أرجع;

    need_gather = TRUE;		// need to fill termleader[]

    لكل (i = 0; i < tc_len; ++i)
	إذا (termcodes[i].name[0] == name[0] && termcodes[i].name[1] == name[1])
	{
	    del_termcode_idx(i);
	    أرجع;
	}
    // not found. Give error message?
}

    ساكن فراغ
del_termcode_idx(صحيح idx)
{
    صحيح		i;

    vim_free(termcodes[idx].کد);
    --tc_len;
    لكل (i = idx; i < tc_len; ++i)
	termcodes[i] = termcodes[i + 1];
}

/*
 * Called when detected that the terminal sends 8-bit codes.
 * Convert all 7-bit codes to their 8-bit equivalent.
 */
    ساكن فراغ
switch_مقصد_8bit(فراغ)
{
    صحيح		i;
    صحيح		c;

    // Only need to do something when not already using 8-bit codes.
    إذا (!جمله_is_8bit(T_NAME))
    {
	لكل (i = 0; i < tc_len; ++i)
	{
	    c = جمله_7to8bit(termcodes[i].کد);
	    إذا (c != 0)
	    {
		STRMOVE(termcodes[i].کد + 1, termcodes[i].کد + 2);
		termcodes[i].کد[0] = c;
	    }
	}
	need_gather = TRUE;		// need to fill termleader[]
    }
    detected_8bit = TRUE;
    LOG_TR1("Switching to 8 bit");
}

#اگر_تعریف_شده CHECK_DOUBLE_CLICK
ساكن linenr_T orig_topline = 0;
# ifdef FEAT_DIFF
ساكن صحيح orig_topfill = 0;
# endif
#پایان_اگر
#إذا defined(CHECK_DOUBLE_CLICK)
/*
 * Checking for double-clicks ourselves.
 * "orig_topline" is used to avoid detecting a double-click when the window
 * contents scrolled (e.g., when 'scrolloff' is non-zero).
 */
/*
 * Set orig_topline.  Used when jumping to another window, so that a double
 * click still works.
 */
    فراغ
مجموعه_mouse_topline(win_T *wp)
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
    صحيح
is_mouse_topline(win_T *wp)
{
    أرجع orig_topline == wp->w_topline
# ifdef FEAT_DIFF
	&& orig_topfill == wp->w_topfill
# endif
	;
}
#پایان_اگر

/*
 * If "buf" is NULL put "string[new_slen]" in typebuf; "buflen" is not used.
 * If "buf" is not NULL put "string[new_slen]" in "buf[bufsize]" and adjust
 * "buflen".
 * Remove "slen" bytes.
 * Returns FAIL for error.
 */
    صحيح
put_string_درون_typebuf(
	صحيح	جابه_جایی,
	صحيح	slen,
	char_u	*string,
	صحيح	new_slen,
	char_u	*buf,
	صحيح	bufsize,
	صحيح	*buflen)
{
    صحيح		extra = new_slen - slen;

    string[new_slen] = NUL;
    إذا (buf == عدم)
    {
	إذا (extra < 0)
	    // remove matched chars, taking care of noremap
	    del_typebuf(-extra, جابه_جایی);
	وإلا إذا (extra > 0)
	    // insert the extra space we need
	    إذا (ins_typebuf(string + slen, REMAP_YES, جابه_جایی, FALSE, FALSE)
								       == FAIL)
		أرجع FAIL;

	// Careful: del_typebuf() and ins_typebuf() may have reallocated
	// typebuf.tb_buf[]!
	mch_memmove(typebuf.tb_buf + typebuf.tb_off + جابه_جایی, string,
							     (size_t)new_slen);
    }
    وإلا
    {
	إذا (extra < 0)
	    // remove matched characters
	    mch_memmove(buf + جابه_جایی, buf + جابه_جایی - extra,
					   (size_t)(*buflen + جابه_جایی + extra));
	وإلا إذا (extra > 0)
	{
	    // Insert the extra space we need.  If there is insufficient
	    // space return -1.
	    إذا (*buflen + extra + new_slen >= bufsize)
		أرجع FAIL;
	    mch_memmove(buf + جابه_جایی + extra, buf + جابه_جایی,
						   (size_t)(*buflen - جابه_جایی));
	}
	mch_memmove(buf + جابه_جایی, string, (size_t)new_slen);
	*buflen = *buflen + extra + new_slen;
    }
    أرجع OK;
}

/*
 * Decode a modifier number as xterm provides it into MOD_MASK bits.
 */
    صحيح
decode_modifiers(صحيح n)
{
    صحيح	    کد = n - 1;
    صحيح	    modifiers = 0;

    إذا (کد & 1)
	modifiers |= MOD_ماسک_لغزش;
    إذا (کد & 2)
	modifiers |= MOD_ماسک_ALT;
    إذا (کد & 4)
	modifiers |= MOD_ماسک_CTRL;
    إذا (کد & 8)
	modifiers |= MOD_ماسک_META;
    // Any further modifiers are silently dropped.

    أرجع modifiers;
}

    ساكن صحيح
modifiers2keycode(صحيح modifiers, صحيح *کلید, char_u *string)
{
    صحيح new_slen = 0;

    إذا (modifiers == 0)
	أرجع 0;

    // Some keys have the modifier included.  Need to handle that here to
    // make mappings work.  This may result in a special key, such as
    // K_S_TAB.
    *کلید = simplify_کلید(*کلید, &modifiers);
    إذا (modifiers != 0)
    {
	string[new_slen++] = K_SPECIAL;
	string[new_slen++] = (صحيح)KS_MODIFIER;
	string[new_slen++] = modifiers;
    }
    أرجع new_slen;
}

/*
 * Handle a cursor position report.
 */
    ساكن فراغ
handle_u7_response(صحيح *arg, char_u *tp استفادهنشده, صحيح csi_len استفادهنشده)
{
    إذا (arg[0] == 2 && arg[1] >= 2)
    {
	حرف *aw = عدم;

	LOG_TRN("Received U7 status: %s", tp);
	u7_وضعیت.tr_پیشرفت = وضعیت_GOT;
	did_cursorhold = TRUE;
	إذا (arg[1] == 2)
	    aw = "single";
	وإلا إذا (arg[1] == 3)
	    aw = "double";
	إذا (aw != عدم && STRCMP(aw, p_ambw) != 0)
	{
	    // Setting the option causes a screen redraw. Do
	    // that right away if possible, keeping any
	    // messages.
	    مجموعه_option_مقدار_give_err((char_u *)"ambw", 0L, (char_u *)aw, 0);
#اگر_تعریف_شده DEBUG_TERMRESPONSE
	    {
		صحيح r = redraw_asap(UPD_CLEAR);

		LOG_TRN("set 'ambiwidth', redraw_asap(): %d", r);
	    }
#وإلا
	    redraw_asap(UPD_CLEAR);
#پایان_اگر
#اگر_تعریف_شده FEAT_EVAL
	    مجموعه_vim_var_string(VV_TERMU7RESP, tp, csi_len);
#پایان_اگر
	    apply_autocmds(EVENT_TERMRESPONSEALL,
					(char_u *)"ambiguouswidth", عدم, FALSE, curbuf);
	}
    }
    وإلا إذا (arg[0] == 3)
    {
	صحيح مقدار;

	LOG_TRN("Received compatibility test result: %s", tp);
	xcc_وضعیت.tr_پیشرفت = وضعیت_GOT;

	// Third row: xterm compatibility test.
	// If the cursor is on the first column then the terminal can handle
	// the request for cursor style and blinking.
	مقدار = arg[1] == 1 ? TPR_YES : TPR_NO;
	جمله_props[TPR_CURSOR_STYLE].tpr_وضعیت = مقدار;
	جمله_props[TPR_CURSOR_BLINK].tpr_وضعیت = مقدار;
    }
}



/*
 * Handle a response to T_CRV: {lead}{first}{x};{vers};{y}c
 * Xterm and alike use '>' for {first}.
 * Rxvt sends "{lead}?1;2c".
 */
    ساكن فراغ
handle_نسخه_response(صحيح اولی, صحيح *arg, صحيح argc, char_u *tp)
{
    // The xterm version.  It is set to zero when it can't be an actual xterm
    // version.
    صحيح نسخه_2 = arg[1];

    LOG_TRN("Received CRV response: %s", tp);
    crv_وضعیت.tr_پیشرفت = وضعیت_GOT;
    did_cursorhold = TRUE;

    // Reset terminal properties that are set based on the termresponse.
    // Mainly useful for tests that send the termresponse multiple times.
    // For testing all props can be reset.
    init_جمله_props(
#اگر_تعریف_شده FEAT_EVAL
	    reset_جمله_props_on_termresponse
#وإلا
	    FALSE
#پایان_اگر
	    );

    // If this code starts with CSI, you can bet that the
    // terminal uses 8-bit codes.
    إذا (tp[0] == CSI)
	switch_مقصد_8bit();

    // Screen sends 40500.
    // rxvt sends its version number: "20703" is 2.7.3.
    // Ignore it for when the user has set 'term' to xterm,
    // even though it's an rxvt.
    إذا (نسخه_2 > 20000)
	نسخه_2 = 0;

    // Figure out more if the response is CSI > 99 ; 99 ; 99 c
    إذا (اولی == '>' && argc == 3)
    {
	// mintty 2.9.5 sends 77;20905;0c.
	// (77 is ASCII 'M' for mintty.)
	إذا (arg[0] == 77)
	{
	    // mintty can do SGR mouse reporting
	    جمله_props[TPR_MOUSE].tpr_وضعیت = TPR_MOUSE_SGR;
	}

#اگر_تعریف_شده FEAT_TERMRESPONSE
	// If xterm version >= 141 try to get termcap codes.  For other
	// terminals the request should be ignored.
	إذا (نسخه_2 >= 141 && p_xtermcodes)
	{
	    LOG_TR1("Enable checking for XT codes");
	    check_for_codes = TRUE;
	    need_gather = TRUE;
	    req_codes_from_جمله();
	}
#پایان_اگر

	// libvterm sends 0;100;0
	// Konsole sends 0;115;0 and works the same way
	إذا ((نسخه_2 == 100 || نسخه_2 == 115) && arg[0] == 0 && arg[2] == 0)
	{
	    // If run from Vim $COLORS is set to the number of
	    // colors the terminal supports.  Otherwise assume
	    // 256, libvterm supports even more.
	    إذا (mch_getenv((char_u *)"COLORS") == عدم)
		may_adjust_color_شمار(256);
	    // Libvterm can handle SGR mouse reporting.
	    جمله_props[TPR_MOUSE].tpr_وضعیت = TPR_MOUSE_SGR;
	}

	إذا (نسخه_2 == 95)
	{
	    // Mac Terminal.app sends 1;95;0
	    //
	    // Terminal.app doesn't seem to handle DECRQM sequences
	    // properly, see issue #19852.
	    إذا (arg[0] == 1 && arg[2] == 0)
	    {
		جمله_props[TPR_UNDERLINE_RGB].tpr_وضعیت = TPR_YES;
		جمله_props[TPR_MOUSE].tpr_وضعیت = TPR_MOUSE_SGR;
		جمله_props[TPR_DECRQM].tpr_وضعیت = TPR_NO;
	    }
	    // iTerm2 sends 0;95;0
	    وإلا إذا (arg[0] == 0 && arg[2] == 0)
	    {
		// iTerm2 can do SGR mouse reporting
		جمله_props[TPR_MOUSE].tpr_وضعیت = TPR_MOUSE_SGR;
		جمله_props[TPR_DECRQM].tpr_وضعیت = TPR_YES;
	    }
	    // old iTerm2 sends 0;95;
	    وإلا إذا (arg[0] == 0 && arg[2] == -1)
		جمله_props[TPR_UNDERLINE_RGB].tpr_وضعیت = TPR_YES;
	}

	// screen sends 83;40500;0 83 is 'S' in ASCII.
	إذا (arg[0] == 83)
	{
	    // screen supports SGR mouse codes since 4.7.0
	    إذا (arg[1] >= 40700)
		جمله_props[TPR_MOUSE].tpr_وضعیت = TPR_MOUSE_SGR;
	    وإلا
		جمله_props[TPR_MOUSE].tpr_وضعیت = TPR_MOUSE_XTERM;
	}

	// If no recognized terminal has set mouse behavior, assume xterm.
	إذا (جمله_props[TPR_MOUSE].tpr_وضعیت == TPR_UNKNOWN)
	{
	    // Xterm version 277 supports SGR.
	    // Xterm version >= 95 supports mouse dragging.
	    إذا (نسخه_2 >= 277)
		جمله_props[TPR_MOUSE].tpr_وضعیت = TPR_MOUSE_SGR;
	    وإلا إذا (نسخه_2 >= 95)
		جمله_props[TPR_MOUSE].tpr_وضعیت = TPR_MOUSE_XTERM2;
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
	إذا (arg[1] >= 2500)
	{
	    جمله_props[TPR_UNDERLINE_RGB].tpr_وضعیت = TPR_YES;
	}

	وإلا إذا (نسخه_2 == 136 && arg[2] == 0)
	{
	    جمله_props[TPR_UNDERLINE_RGB].tpr_وضعیت = TPR_YES;

	    // PuTTY sends 0;136;0
	    إذا (arg[0] == 0)
	    {
		// supports sgr-like mouse reporting.
		جمله_props[TPR_MOUSE].tpr_وضعیت = TPR_MOUSE_SGR;
	    }
	    // vandyke SecureCRT sends 1;136;0
	}

	// Konsole sends 0;115;0 - but t_u8 does not actually work, therefore
	// commented out.
	// else if (version == 115 && arg[0] == 0 && arg[2] == 0)
	//     term_props[TPR_UNDERLINE_RGB].tpr_status = TPR_YES;

	// Kitty up to 9.x sends 1;400{version};{secondary-version}
	إذا (arg[0] == 1 && arg[1] >= 4000 && arg[1] <= 4009)
	{
	    جمله_props[TPR_KITTY].tpr_وضعیت = TPR_YES;
	    جمله_props[TPR_KITTY].tpr_مجموعه_by_termresponse = TRUE;

	    // Kitty can handle SGR mouse reporting.
	    جمله_props[TPR_MOUSE].tpr_وضعیت = TPR_MOUSE_SGR;
	    جمله_props[TPR_DECRQM].tpr_وضعیت = TPR_YES;
	}

	// foot terminal sends 1;12700;0
	إذا (arg[0] == 1 && نسخه_2 == 12700 && arg[2] == 0)
	{
	    جمله_props[TPR_MOUSE].tpr_وضعیت = TPR_MOUSE_SGR;
	    جمله_props[TPR_DECRQM].tpr_وضعیت = TPR_YES;
	}

	// GNU screen sends 83;30600;0, 83;40500;0, etc.
	// 30600/40500 is a version number of GNU screen. DA2 support is added
	// on 3.6.  DCS string has a special meaning to GNU screen, but xterm
	// compatibility checking does not detect GNU screen.
	إذا (arg[0] == 83 && arg[1] >= 30600)
	{
	    جمله_props[TPR_CURSOR_STYLE].tpr_وضعیت = TPR_NO;
	    جمله_props[TPR_CURSOR_BLINK].tpr_وضعیت = TPR_NO;
	    جمله_props[TPR_DECRQM].tpr_وضعیت = TPR_NO; // screen doesn't seem
							// to handle DECRQM
							// sequences
	}

	// Xterm first responded to this request at patch level
	// 95, so assume anything below 95 is not xterm and hopefully supports
	// the underline RGB color sequence.
	إذا (نسخه_2 < 95)
	    جمله_props[TPR_UNDERLINE_RGB].tpr_وضعیت = TPR_YES;

	// Getting the cursor style is only supported properly by xterm since
	// version 279 (otherwise it returns 0x18).
	إذا (نسخه_2 < 279)
	    جمله_props[TPR_CURSOR_STYLE].tpr_وضعیت = TPR_NO;

	/*
	 * Take action on the detected properties.
	 */

	// Unless the underline RGB color is expected to work, disable "t_8u".
	// It does not work for the real Xterm, it resets the background color.
	// This may cause some flicker.  Alternative would be to set "t_8u"
	// here if the terminal is expected to support it, but that might
	// conflict with what was set in the .vimrc.
	إذا (جمله_props[TPR_UNDERLINE_RGB].tpr_وضعیت != TPR_YES
			&& *T_8U != NUL
			&& !option_was_مجموعه((char_u *)"t_8u"))
	{
	    مجموعه_string_option_مستقیم((char_u *)"t_8u", -1, (char_u *)"",
								  OPT_FREE, 0);
	}
#اگر_تعریف_شده FEAT_TERMRESPONSE
	إذا (*T_8U != NUL && write_t_8u_state == MAYBE)
	    // Did skip writing t_8u, a complete redraw is needed.
	    redraw_later_clear();
	write_t_8u_state = OK;  // can output t_8u now
#پایان_اگر

	// Only set 'ttymouse' automatically if it was not set
	// by the user already.
	إذا (!option_was_مجموعه((char_u *)"ttym")
		&& (جمله_props[TPR_MOUSE].tpr_وضعیت == TPR_MOUSE_XTERM2
		    || جمله_props[TPR_MOUSE].tpr_وضعیت == TPR_MOUSE_SGR))
	{
	    مجموعه_option_مقدار_give_err((char_u *)"ttym", 0L,
		    جمله_props[TPR_MOUSE].tpr_وضعیت == TPR_MOUSE_SGR
				    ? (char_u *)"sgr" : (char_u *)"xterm2", 0);
	}

#اگر_تعریف_شده FEAT_TERMRESPONSE
	صحيح need_flush = FALSE;

	// Only request the cursor style if t_SH and t_RS are
	// set. Only supported properly by xterm since version
	// 279 (otherwise it returns 0x18).
	// Only when getting the cursor style was detected to work.
	// Not for Terminal.app, it can't handle t_RS, it
	// echoes the characters to the screen.
	إذا (rcs_وضعیت.tr_پیشرفت == وضعیت_GET
		&& جمله_props[TPR_CURSOR_STYLE].tpr_وضعیت == TPR_YES
		&& *T_CSH != NUL
		&& *T_CRS != NUL)
	{
	    MAY_WANT_مقصد_LOG_THIS;
	    LOG_TR1("Sending cursor style request");
	    بیرون_str(T_CRS);
	    termrequest_فرستاده(&rcs_وضعیت);
	    need_flush = TRUE;
	}

	// Only request the cursor blink mode if t_RC set. Not
	// for Gnome terminal, it can't handle t_RC, it
	// echoes the characters to the screen.
	// Only when getting the cursor style was detected to work.
	إذا (rbm_وضعیت.tr_پیشرفت == وضعیت_GET
		&& جمله_props[TPR_CURSOR_BLINK].tpr_وضعیت == TPR_YES
		&& *T_بررسی_چرخهای != NUL)
	{
	    MAY_WANT_مقصد_LOG_THIS;
	    LOG_TR1("Sending cursor blink mode request");
	    بیرون_str(T_بررسی_چرخهای);
	    termrequest_فرستاده(&rbm_وضعیت);
	    need_flush = TRUE;
	}

	// Only request DEC modes via DECRQM when the terminal is known to
	// handle it.  Not for Apple Terminal.app or GNU screen, they echo
	// the trailing "p" to the screen.  See issue #19852.
	إذا (decrqm_وضعیت.tr_پیشرفت == وضعیت_GET
		&& جمله_props[TPR_DECRQM].tpr_وضعیت == TPR_YES)
	{
	    MAY_WANT_مقصد_LOG_THIS;
	    LOG_TR1("Sending DECRQM requests");
	    لكل (صحيح i = 0; i < (صحيح)ARRAY_درازی(dec_modes); i++)
	    {
		vim_snprintf((حرف *)IObuff, IOSIZE, "\033[?%d$p", dec_modes[i]);
		بیرون_str(IObuff);
	    }
	    termrequest_فرستاده(&decrqm_وضعیت);
	    need_flush = TRUE;

	}

	إذا (need_flush)
	    بیرون_flush();
#پایان_اگر
    }
}

/*
 * Add "key" to "buf" and return the number of bytes used.
 * Handles special keys and multi-byte characters.
 */
    ساكن صحيح
جمع_کن_کلید_مقصد_buf(صحيح کلید, char_u *buf)
{
    صحيح idx = 0;

    إذا (IS_SPECIAL(کلید))
    {
	buf[idx++] = K_SPECIAL;
	buf[idx++] = KEY2TERMCAP0(کلید);
	buf[idx++] = KEY2TERMCAP1(کلید);
    }
    وإلا إذا (has_mbyte)
	idx += (*mb_char2bytes)(کلید, buf + idx);
    وإلا
	buf[idx++] = کلید;
    أرجع idx;
}

/*
 * Shared between handle_key_with_modifier() and handle_csi_function_key().
 */
    ساكن صحيح
put_کلید_modifiers_درون_typebuf(
	صحيح	کلید_arg,
	صحيح	modifiers_arg,
	صحيح	csi_len,
	صحيح	جابه_جایی,
	char_u	*buf,
	صحيح	bufsize,
	صحيح	*buflen)
{
    صحيح کلید = کلید_arg;
    صحيح modifiers = modifiers_arg;

    // Some keys need adjustment when the Ctrl modifier is used.
    کلید = may_adjust_کلید_for_ctrl(modifiers, کلید);

    // May remove the shift modifier if it's already included in the key.
    modifiers = may_remove_لغزش_modifier(modifiers, کلید);

    // Produce modifiers with K_SPECIAL KS_MODIFIER {mod}
    // worst-case: 3-byte modifier + 4 byte multi-char key + NUL
    char_u string[MAX_کلید_کد_LEN + 2];
    صحيح new_slen = modifiers2keycode(modifiers, &کلید, string);

    // reject overlong key that would overflow string
    إذا (کلید > 0x10FFFF)
	أرجع -1;

    // Add the bytes for the key.
    new_slen += جمع_کن_کلید_مقصد_buf(کلید, string + new_slen);

    string[new_slen] = NUL;
    إذا (put_string_درون_typebuf(جابه_جایی, csi_len, string, new_slen,
						 buf, bufsize, buflen) == FAIL)
	أرجع -1;
    أرجع new_slen - csi_len + جابه_جایی;
}

/*
 * Parse the number from a CSI numbered sequence for an F1-F12 key:
 *	ESC [ {number} ~
 * Returns the key
 */
    ساكن صحيح
parse_csi_f_کلیدها(صحيح arg)
{
    char_u کلید_name[2] = { 0, 0 };

    بدل (arg)
    {
	حالة 11: کلید_name[0] = 'k'; کلید_name[1] = '1'; اكسر;  // K_F1
	حالة 12: کلید_name[0] = 'k'; کلید_name[1] = '2'; اكسر;  // K_F2
	حالة 13: کلید_name[0] = 'k'; کلید_name[1] = '3'; اكسر;  // K_F3
	حالة 14: کلید_name[0] = 'k'; کلید_name[1] = '4'; اكسر;  // K_F4
	حالة 15: کلید_name[0] = 'k'; کلید_name[1] = '5'; اكسر;  // K_F5
	حالة 17: کلید_name[0] = 'k'; کلید_name[1] = '6'; اكسر;  // K_F6
	حالة 18: کلید_name[0] = 'k'; کلید_name[1] = '7'; اكسر;  // K_F7
	حالة 19: کلید_name[0] = 'k'; کلید_name[1] = '8'; اكسر;  // K_F8
	حالة 20: کلید_name[0] = 'k'; کلید_name[1] = '9'; اكسر;  // K_F9
	حالة 21: کلید_name[0] = 'F'; کلید_name[1] = ';'; اكسر;  // K_F10
	حالة 23: کلید_name[0] = 'F'; کلید_name[1] = '1'; اكسر;  // K_F11
	حالة 24: کلید_name[0] = 'F'; کلید_name[1] = '2'; اكسر;  // K_F12
    }
    إذا (کلید_name[0])
	أرجع TERMCAP2_کلید(کلید_name[0], کلید_name[1]);
    // shouldn't happen
    أرجع arg;
}

/*
 * Handle a sequence with key and modifier, one of:
 *	{lead}27;{modifier};{key}~
 *	{lead}{key};{modifier}u
 * Returns the difference in length.
 */
    ساكن صحيح
handle_کلید_with_modifier(
	صحيح	*arg,
	صحيح	csi_len,
	صحيح	جابه_جایی,
	char_u	*buf,
	صحيح	bufsize,
	صحيح	*buflen,
	صحيح	iskitty,
	صحيح	trail)
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
    إذا (!iskitty
	    && (kitty_protocol_state == KKPS_INITIAL
		|| kitty_protocol_state == KKPS_OFF
		|| kitty_protocol_state == KKPS_AFTER_T_TE)
	    && جمله_props[TPR_KITTY].tpr_وضعیت != TPR_YES)
    {
#اگر_تعریف_شده FEAT_EVAL
	ch_log(عدم, "setting seenModifyOtherKeys to TRUE");
#پایان_اگر
	دیده_Modify_Other_کلیدها = TRUE;
    }

    صحيح کلید = iskitty ? arg[0] : arg[2];
    صحيح modifiers = decode_modifiers(arg[1]);

    // Some terminals do not apply the Shift modifier to the key.  To make
    // mappings consistent we do it here.  TODO: support more keys.
    إذا ((modifiers & MOD_ماسک_لغزش) && کلید >= 'a' && کلید <= 'z')
	کلید += 'A' - 'a';

    // Putting Esc in the buffer creates ambiguity, it can be the start of an
    // escape sequence.  Use K_ESC to avoid that.
    إذا (کلید == ESC)
	کلید = K_ESC;

    وإلا إذا (arg[0] >= 11 && arg[0] <= 24 && trail == '~')
	کلید = parse_csi_f_کلیدها(arg[0]);

    أرجع put_کلید_modifiers_درون_typebuf(کلید, modifiers, csi_len, جابه_جایی, buf,
	    bufsize, buflen);
}

/*
 * Handle a sequence with key without a modifier:
 *	{lead}{key}u
 *	{lead}{key}~
 * Returns the difference in length.
 */
    ساكن صحيح
handle_کلید_without_modifier(
	صحيح	*arg,
	صحيح	csi_len,
	صحيح	جابه_جایی,
	char_u	*buf,
	صحيح	bufsize,
	صحيح	*buflen,
	صحيح	trail)
{
    char_u  string[MAX_کلید_کد_LEN + 1];
    صحيح	    new_slen;

    إذا (arg[0] == ESC)
    {
	// Putting Esc in the buffer creates ambiguity, it can be the start of
	// an escape sequence.  Use K_ESC to avoid that.
	string[0] = K_SPECIAL;
	string[1] = KS_EXTRA;
	string[2] = KE_ESC;
	new_slen = 3;
    }
    وإلا إذا (arg[0] >= 11 && arg[0] <= 24 && trail == '~')
    {
	صحيح کلید = parse_csi_f_کلیدها(arg[0]);
	string[0] = K_SPECIAL;
	string[1] = KEY2TERMCAP0(کلید);
	string[2] = KEY2TERMCAP1(کلید);
	new_slen = 3;
    }
    وإلا
	new_slen = جمع_کن_کلید_مقصد_buf(arg[0], string);

    إذا (put_string_درون_typebuf(جابه_جایی, csi_len, string, new_slen,
						 buf, bufsize, buflen) == FAIL)
	أرجع -1;
    أرجع new_slen - csi_len + جابه_جایی;
}

/*
 * CSI function key without or with modifiers:
 *	{lead}[ABCDEFHPQRS]
 *	{lead}1;{modifier}[ABCDEFHPQRS]
 * Returns zero when nog recognized, a positive number when recognized.
 */
    ساكن صحيح
handle_csi_function_کلید(
	صحيح	argc,
	صحيح	*arg,
	صحيح	trail,
	صحيح	csi_len,
	char_u	*کلید_name,
	صحيح	جابه_جایی,
	char_u  *buf,
	صحيح	bufsize,
	صحيح	*buflen)
{
    کلید_name[0] = 'k';
    بدل (trail)
    {
	حالة 'A': کلید_name[1] = 'u'; اكسر;  // K_UP
	حالة 'B': کلید_name[1] = 'd'; اكسر;  // K_DOWN
	حالة 'C': کلید_name[1] = 'r'; اكسر;  // K_RIGHT
	حالة 'D': کلید_name[1] = 'l'; اكسر;  // K_LEFT

	// case 'E': keypad BEGIN - not supported
	حالة 'F': کلید_name[0] = '@'; کلید_name[1] = '7'; اكسر;  // K_END
	حالة 'H': کلید_name[1] = 'h'; اكسر;  // K_HOME

	حالة 'P': کلید_name[1] = '1'; اكسر;  // K_F1
	حالة 'Q': کلید_name[1] = '2'; اكسر;  // K_F2
	حالة 'R': کلید_name[1] = '3'; اكسر;  // K_F3
	حالة 'S': کلید_name[1] = '4'; اكسر;  // K_F4

	افتراضي: أرجع 0;  // not recognized
    }

    صحيح کلید = TERMCAP2_کلید(کلید_name[0], کلید_name[1]);
    صحيح modifiers = argc == 2 ? decode_modifiers(arg[1]) : 0;
    put_کلید_modifiers_درون_typebuf(کلید, modifiers,
					csi_len, جابه_جایی, buf, bufsize, buflen);
    أرجع csi_len;
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
    ساكن صحيح
handle_csi(
	char_u	*tp,
	صحيح	len,
	char_u	*argp,
	صحيح	جابه_جایی,
	char_u  *buf,
	صحيح	bufsize,
	صحيح	*buflen,
	char_u	*کلید_name,
	صحيح	*slen)
{
    صحيح		اولی = -1;  // optional char right after {lead}
    صحيح		trail;	     // char that ends CSI sequence
    صحيح		arg[3] = {-1, -1, -1};	// argument numbers
    صحيح		argc = 0;		// number of arguments
    char_u	*ap = argp;
    صحيح		csi_len;

    // Check for non-digit after CSI.
    إذا (!VIM_ISDIGIT(*ap))
	اولی = *ap++;

    إذا (اولی >= 'A' && اولی <= 'Z')
    {
	// If "first" is in [ABCDEFHPQRS] then it is actually the "trail" and
	// no argument follows.
	trail = اولی;
	اولی = -1;
	--ap;
    }
    وإلا
    {
	// Find up to three argument numbers.
	لكل (argc = 0; argc < 3; )
	{
	    إذا (ap >= tp + len)
		أرجع -1;
	    إذا (*ap == ';')
		arg[argc++] = -1;  // omitted number
	    وإلا إذا (VIM_ISDIGIT(*ap))
	    {
		arg[argc] = 0;
		لكل (;;)
		{
		    إذا (ap >= tp + len)
			أرجع -1;
		    إذا (!VIM_ISDIGIT(*ap))
			اكسر;
		    // avoid overflow
		    إذا (arg[argc] <= (بیشترین_صحیح - 9) / 10)
			arg[argc] = arg[argc] * 10 + (*ap - '0');
		    ++ap;
		}
		++argc;
	    }
	    إذا (*ap == ';')
		++ap;
	    وإلا
		اكسر;
	}

	// mrxvt has been reported to have "+" in the version. Assume
	// the escape sequence ends with a letter or one of "{|}~".
	طالما (ap < tp + len
		&& !(*ap >= '{' && *ap <= '~')
		&& !ASCII_ISALPHA(*ap))
	    ++ap;
	إذا (ap >= tp + len)
	    أرجع -1;
	trail = *ap;
    }

    csi_len = (صحيح)(ap - tp) + 1;

    // Response to XTQMODKEYS: "CSI > 4 ; Pv m" where Pv indicates the
    // modifyOtherKeys level.  Drop similar responses.
    إذا (اولی == '>' && (argc == 1 || argc == 2) && trail == 'm')
    {
	إذا (arg[0] == 4 && argc == 2)
	    modify_otherkeys_state = arg[1] == 2 ? MOKS_ENABLED : MOKS_OFF;

	کلید_name[0] = (صحيح)KS_EXTRA;
	کلید_name[1] = (صحيح)KE_IGNORE;
	*slen = csi_len;
    }

    // Function key starting with CSI:
    //	{lead}[ABCDEFHPQRS]
    //	{lead}1;{modifier}[ABCDEFHPQRS]
    وإلا إذا (اولی == -1 && ASCII_ISUPPER(trail)
	    && (argc == 0 || (argc == 2 && arg[0] == 1)))
    {
	صحيح res = handle_csi_function_کلید(argc, arg, trail,
			      csi_len, کلید_name, جابه_جایی, buf, bufsize, buflen);
	أرجع res <= 0 ? res : len + res;
    }

    // Cursor position report: {lead}{row};{col}R
    // Eat it when there are 2 arguments and it ends in 'R'.
    // Also when u7_status is not "sent", it may be from a previous Vim that
    // just exited.  But not for <S-F3>, it sends something similar, check for
    // row and column to make sense.
    وإلا إذا (اولی == -1 && argc == 2 && trail == 'R')
    {
	handle_u7_response(arg, tp, csi_len);

	کلید_name[0] = (صحيح)KS_EXTRA;
	کلید_name[1] = (صحيح)KE_IGNORE;
	*slen = csi_len;
    }

#إذا (defined(UNIX) || defined(MSWIN)) \
    && (defined(FEAT_IMAGE) || defined(FEAT_EVAL))
    // Response to CSI 14 t or CSI 16 t
    وإلا إذا (اولی == -1 && argc >= 3
	    && (arg[0] == 4 || arg[0] == 6) && trail == 't')
    {
	إذا (arg[0] == 4)
	{
	    LOG_TRN("Received CSI 14 t response: %s", tp);
	    cell_عرض = arg[2] / Columns;
	    cell_ارتفاع = arg[1] / ردیفها;
	}
	وإلا
	{
	    LOG_TRN("Received CSI 16 t response: %s", tp);
	    cell_عرض = arg[2];
	    cell_ارتفاع = arg[1];
	}

	کلید_name[0] = (صحيح)KS_EXTRA;
	کلید_name[1] = (صحيح)KE_IGNORE;
	*slen = csi_len;
    }
#پایان_اگر

    // Primary device attributes (DA1) response
    وإلا إذا (اولی == '?' && trail == 'c')
    {
	LOG_TRN("Received DA1 response: %s", tp);

	*slen = csi_len;
#اگر_تعریف_شده FEAT_EVAL
	مجموعه_vim_var_string(VV_TERMDA1, tp, *slen);
#پایان_اگر
	apply_autocmds(EVENT_TERMRESPONSEALL,
					(char_u *)"da1", عدم, FALSE, curbuf);

	کلید_name[0] = (صحيح)KS_EXTRA;
	کلید_name[1] = (صحيح)KE_IGNORE;
	*slen = csi_len;
    }

    // DECRPM mode 2026 or 2048.
    وإلا إذا (اولی == '?' && trail == 'y' && argc == 2
	    && (arg[0] == 2026 || arg[0] == 2048))
    {
	صحيح setting = arg[1];

	*slen = csi_len;
	کلید_name[0] = (صحيح)KS_EXTRA;
	کلید_name[1] = (صحيح)KE_IGNORE;

#اگر_تعریف_شده FEAT_TERMRESPONSE
	// Mark the DECRQM request as answered so it is not sent again and
	// stoptermcap() does not wait for it.
	إذا (decrqm_وضعیت.tr_پیشرفت == وضعیت_فرستاده)
	    decrqm_وضعیت.tr_پیشرفت = وضعیت_GOT;
#پایان_اگر

	إذا (setting >= 0 && setting <= 4)
	{
	    LOG_TRN("Received DECRPM mode %d: %s", arg[0], tp);

	    بدل (arg[0])
	    {
		حالة 2026:
		    sync_output_setting = setting;
		    مجموعه_option_مقدار_give_err((char_u *)"termsync",
			    setting == 1 || setting == 2, عدم, 0);
		    اكسر;
#اگر_تعریف_شده UNIX
		حالة 2048:
		    win_resize_setting = setting;

		    جمله_مجموعه_win_resize(true);
		    اكسر;
#پایان_اگر
	    }
	}
	وإلا
	    LOG_TRN("Unknown DECRPM mode %d setting %d", arg[0], setting);
    }

#اگر_تعریف_شده UNIX
    // In-band window resize event
    وإلا إذا (win_resize_enabled && argc >= 3 && arg[0] == 48)
    {
	صحيح ارتفاع = arg[1], عرض = arg[2];

	*slen = csi_len;
	کلید_name[0] = (صحيح)KS_EXTRA;
	کلید_name[1] = (صحيح)KE_IGNORE;

	// Only update if needed. Avoids intro message from disappearing on
	// startup (due to initial event).
	إذا (ارتفاع != ردیفها || عرض != Columns)
	    مجموعه_shellsize(عرض, ارتفاع, true);
    }
#پایان_اگر

    // Version string: Eat it when there is at least one digit and
    // it ends in 'c'
    وإلا إذا (*T_CRV != NUL && ap > argp + 1 && trail == 'c')
    {
	handle_نسخه_response(اولی, arg, argc, tp);

	*slen = csi_len;
#اگر_تعریف_شده FEAT_EVAL
	مجموعه_vim_var_string(VV_TERMRESPONSE, tp, *slen);
#پایان_اگر
	apply_autocmds(EVENT_TERMRESPONSE,
					عدم, عدم, FALSE, curbuf);
	apply_autocmds(EVENT_TERMRESPONSEALL,
					(char_u *)"version", عدم, FALSE, curbuf);
	کلید_name[0] = (صحيح)KS_EXTRA;
	کلید_name[1] = (صحيح)KE_IGNORE;
    }

#اگر_تعریف_شده FEAT_TERMRESPONSE
    // Check blinking cursor from xterm:
    // {lead}?12;1$y       set
    // {lead}?12;2$y       not set
    //
    // {lead} can be <Esc>[ or CSI
    وإلا إذا (rbm_وضعیت.tr_پیشرفت == وضعیت_فرستاده
	    && اولی == '?'
	    && ap == argp + 6
	    && arg[0] == 12
	    && ap[-1] == '$'
	    && trail == 'y')
    {
	initial_cursor_blink = (arg[1] == '1');
	rbm_وضعیت.tr_پیشرفت = وضعیت_GOT;
	LOG_TRN("Received cursor blinking mode response: %s", tp);
	کلید_name[0] = (صحيح)KS_EXTRA;
	کلید_name[1] = (صحيح)KE_IGNORE;
	*slen = csi_len;
# ifdef FEAT_EVAL
	مجموعه_vim_var_string(VV_TERMBLINKRESP, tp, *slen);
# endif
	apply_autocmds(EVENT_TERMRESPONSEALL,
					(char_u *)"cursorblink", عدم, FALSE, curbuf);
    }
#پایان_اگر

    // Kitty keyboard protocol status response: CSI ? flags u
    وإلا إذا (اولی == '?' && argc == 1 && trail == 'u')
    {
	// The protocol has various "progressive enhancement flags" values, but
	// we only check for zero and non-zero here.
	إذا (arg[0] == '0')
	{
	    kitty_protocol_state = KKPS_OFF;
	}
	وإلا
	{
	    kitty_protocol_state = KKPS_ENABLED;

	    // Reset seenModifyOtherKeys just in case some key combination has
	    // been seen that set it before we get the status response.
#اگر_تعریف_شده FEAT_EVAL
	    ch_log(عدم, "setting seenModifyOtherKeys to FALSE");
#پایان_اگر
	    دیده_Modify_Other_کلیدها = FALSE;
	}

	کلید_name[0] = (صحيح)KS_EXTRA;
	کلید_name[1] = (صحيح)KE_IGNORE;
	*slen = csi_len;
    }

#اگر_تعریف_شده FEAT_TERMRESPONSE
    // Check for a window position response from the terminal:
    //       {lead}3;{x};{y}t
    وإلا إذا (did_request_winpos && argc == 3 && arg[0] == 3
						   && trail == 't')
    {
	winpos_x = arg[1];
	winpos_y = arg[2];
	// got finished code: consume it
	کلید_name[0] = (صحيح)KS_EXTRA;
	کلید_name[1] = (صحيح)KE_IGNORE;
	*slen = csi_len;

	إذا (--did_request_winpos <= 0)
	    winpos_وضعیت.tr_پیشرفت = وضعیت_GOT;
    }
#پایان_اگر

    // Key with modifier:
    //	{lead}27;{modifier};{key}~
    //	{lead}{key};{modifier}u
    //	{lead}{key};{modifier}~
    // Even though we only handle four modifiers and the {modifier} value
    // should be 16 or lower, we accept all modifier values to avoid the raw
    // sequence to be passed through.
    وإلا إذا ((arg[0] == 27 && argc == 3 && trail == '~')
		|| (argc == 2 && (trail == 'u' || trail == '~')))
    {
	صحيح iskitty = argc == 2 && (trail == 'u' || trail == '~');
	أرجع len + handle_کلید_with_modifier(arg, csi_len, جابه_جایی, buf,
		bufsize, buflen, iskitty, trail);
    }

    // Key without modifier (Kitty sends this for Esc or F3):
    //	{lead}{key}u
    //	{lead}{key}~
    وإلا إذا (argc == 1 && (trail == 'u' || trail == '~'))
    {
	أرجع len + handle_کلید_without_modifier(arg, csi_len, جابه_جایی, buf,
		bufsize, buflen, trail);
    }

    // else: Unknown CSI sequence.  We could drop it, but then the
    // user can't create a map for it.
    أرجع 0;
}

ساكن فراغ
check_for_color_response(char_u *resp, صحيح len)
{
    صحيح		i, j;
    char_u	*argp;

    j = 1 + (resp[0] == ESC);
    argp = resp + j;

    إذا (len >= j + 3 && (argp[0] != '1'
			     || (argp[1] != '1' && argp[1] != '0')
			     || argp[2] != ';'))
	i = 0; // no match
    وإلا
    {
	لكل (i = j; i < len; ++i)
	    إذا (resp[i] == '\007' || (resp[0] == OSC ? resp[i] == STERM
			: (resp[i] == ESC && i + 1 < len && resp[i + 1] == '\\')))
	    {
		صحيح is_bg = argp[1] == '1';
		صحيح is_4digit = i - j >= 21 && resp[j + 11] == '/'
						  && resp[j + 16] == '/';

		إذا (i - j >= 15 && STRNCMP(resp + j + 3, "rgb:", 4) == 0
			    && (is_4digit
				   || (resp[j + 9] == '/' && resp[j + 12] == '/')))
		{
		    char_u *tp_r = resp + j + 7;
		    char_u *tp_g = resp + j + (is_4digit ? 12 : 10);
		    char_u *tp_b = resp + j + (is_4digit ? 17 : 13);
#اگر_تعریف_شده FEAT_TERMRESPONSE
		    صحيح rval, gval, bval;

		    rval = hexhex2nr(tp_r);
		    gval = hexhex2nr(tp_g);
		    bval = hexhex2nr(tp_b);
#پایان_اگر
		    إذا (is_bg)
		    {
			حرف *new_bg_val = (3 * '6' < *tp_r + *tp_g +
					     *tp_b) ? "light" : "dark";

			LOG_TRN("Received RBG response: r=%d g=%d b=%d", rval, gval, bval);
#اگر_تعریف_شده FEAT_TERMRESPONSE
			rbg_وضعیت.tr_پیشرفت = وضعیت_GOT;
			bg_r = rval;
			bg_g = gval;
			bg_b = bval;
#پایان_اگر
			إذا (!option_was_مجموعه((char_u *)"bg")
				      && STRCMP(p_bg, new_bg_val) != 0)
			{
			    // value differs, apply it
			    مجموعه_option_مقدار_give_err((char_u *)"bg",
						  0L, (char_u *)new_bg_val, 0);
			    reset_option_was_مجموعه((char_u *)"bg");
			    redraw_asap(UPD_CLEAR);
			}
		    }
#إذا defined(FEAT_TERMRESPONSE) && defined(FEAT_TERMINAL)
		    وإلا
		    {
			LOG_TRN("Received RFG response: r=%d g=%d b=%d", rval, gval, bval);
			rfg_وضعیت.tr_پیشرفت = وضعیت_GOT;
			fg_r = rval;
			fg_g = gval;
			fg_b = bval;
		    }
#پایان_اگر
		}

#اگر_تعریف_شده FEAT_EVAL
		مجموعه_vim_var_string(is_bg ? VV_TERMRBGRESP
						  : VV_TERMRFGRESP, resp, len);
#پایان_اگر
		apply_autocmds(EVENT_TERMRESPONSEALL,
			is_bg ? (char_u *)"background" : (char_u *)"foreground",
			عدم, FALSE, curbuf);
		اكسر;
	    }
    }
    إذا (i == len)
	LOG_TR1("not enough characters for RB");
}

ساكن oscstate_T osc_state;

/*
 * Return true if currently receiving an OSC response.
 */
    bool
درون_osc_sequence(فراغ)
{
    أرجع osc_state.processing;
}

/*
 * Handles any OSC sequence and places the result in "v:termosc". Returns OK on
 * success and FAIL on failure.
 */
    ساكن صحيح
handle_osc(char_u *tp, صحيح len, char_u *کلید_name, صحيح *slen)
{
    char_u آخرین_char;

    إذا (!osc_state.processing)
    {
	صحيح cur;

	LOG_TRN("Received OSC response: %s", (حرف*)tp);
	// Check if it is a valid OSC sequence, and consume it. OSC format
	// consists of:
	// <identifier><data><terminator>
	// <identifier> is either <Esc>] or an OSC character
	// <terminator> can be '\007', <Esc>\ or STERM.
	cur = 1 + (tp[0] == ESC);

	إذا (len < cur + 1 + (tp[0] != OSC)) // Include terminator as well
	    أرجع FAIL;

	// The whole OSC response may be larger than the typeahead buffer.
	// To handle this, keep reading data in and out of the typeahead
	// buffer until we read an OSC terminator or timeout.

	// We can't use the previous buffer since we transferred ownership of it
	// to the vim var.
	ga_init2(&osc_state.buf, 1, 1024);
#اگر_تعریف_شده ELAPSED_FUNC
	ELAPSED_INIT(osc_state.شروع_tv);
#پایان_اگر
	osc_state.processing = TRUE;
	osc_state.شروع_char = tp[0];
	آخرین_char = 0;
    }
    وإلا
	آخرین_char = ((char_u *)osc_state.buf.ga_داده)[osc_state.buf.ga_len - 1];

    کلید_name[0] = (صحيح)KS_EXTRA;

    // Read data and append to buffer. If we reach a terminator, then
    // finally set the vim var.
    لكل (صحيح i = 0; i < len; i++)
	إذا (tp[i] == '\007' || (osc_state.شروع_char == OSC ? tp[i] == STERM
		    : ((tp[i] == ESC && i + 1 < len && tp[i + 1] == '\\')
			|| (i == 0 && tp[i] == '\\' && آخرین_char == ESC)
			)))
	{
	    osc_state.processing = FALSE;

	    کلید_name[1] = (صحيح)KE_OSC;

	    ga_concat_len(&osc_state.buf, tp, i + 1 + (tp[i] == ESC));
	    ga_append(&osc_state.buf, NUL);
	    *slen = i + 1 + (tp[i] == ESC);
#اگر_تعریف_شده FEAT_EVAL
	    مجموعه_vim_var_string_مستقیم(VV_TERMOSC, osc_state.buf.ga_داده);
#پایان_اگر
	    // Check for background/foreground colour response
	    check_for_color_response(osc_state.buf.ga_داده, osc_state.buf.ga_len - 1);

	    char_u savebg = *p_bg;
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"osc",
		    عدم, FALSE, curbuf);
	    إذا (*p_bg != savebg)
		redraw_asap(UPD_CLEAR);
	    أرجع OK;
	}

    کلید_name[1] = (صحيح)KE_IGNORE;

#اگر_تعریف_شده ELAPSED_FUNC
    إذا (ELAPSED_FUNC(osc_state.شروع_tv) >= p_ost)
    {
	semsg(_(e_osc_response_timed_بیرون), osc_state.buf.ga_len,
		osc_state.buf.ga_داده);

	ga_clear(&osc_state.buf);
	osc_state.processing = FALSE;
	أرجع FAIL;
    }
#پایان_اگر

    ga_concat(&osc_state.buf, tp);
    *slen = len; // Consume everything

    أرجع OK;
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
    ساكن صحيح
handle_dcs(char_u *tp, char_u *argp, صحيح len, char_u *کلید_name, صحيح *slen)
{
    صحيح i, j;

    LOG_TRN("Received DCS response: %s", (حرف*)tp);
    j = 1 + (tp[0] == ESC);
    إذا (len < j + 3)
	i = len; // need more chars
    وإلا إذا ((argp[1] != '+' && argp[1] != '$')
	    || (argp[2] != 'r' && argp[2] != 'R'))
	i = 0; // no match
    وإلا إذا (argp[1] == '+')
	// key code response
	لكل (i = j; i < len; ++i)
	{
	    إذا ((tp[i] == ESC && i + 1 < len && tp[i + 1] == '\\')
		    || tp[i] == STERM)
	    {
#اگر_تعریف_شده FEAT_TERMRESPONSE
		// handle a key code response, drop a resource response
		إذا (i - j >= 3 && argp[2] == 'r')
		    got_کد_from_جمله(tp + j, i);
#پایان_اگر
		کلید_name[0] = (صحيح)KS_EXTRA;
		کلید_name[1] = (صحيح)KE_IGNORE;
		*slen = i + 1 + (tp[i] == ESC);
		اكسر;
	    }
	}
    وإلا
    {
	// Probably the cursor shape response.  Make sure that "i"
	// is equal to "len" when there are not sufficient
	// characters.
	لكل (i = j + 3; i < len; ++i)
	{
	    إذا (i - j == 3 && !امن_isdigit(tp[i]))
		اكسر;
	    إذا (i - j == 4 && tp[i] != ' ')
		اكسر;
	    إذا (i - j == 5 && tp[i] != 'q')
		اكسر;
	    إذا (i - j == 6 && tp[i] != ESC && tp[i] != STERM)
		اكسر;
	    إذا ((i - j == 6 && tp[i] == STERM)
		    || (i - j == 7 && tp[i] == '\\'))
	    {
#اگر_تعریف_شده FEAT_TERMRESPONSE
		صحيح عدد = argp[3] - '0';

		// 0, 1 = block blink, 2 = block
		// 3 = underline blink, 4 = underline
		// 5 = vertical bar blink, 6 = vertical bar
		عدد = عدد == 0 ? 1 : عدد;
		initial_cursor_shape = (عدد + 1) / 2;
		// The blink flag is actually inverted, compared to
		// the value set with T_SH.
		initial_cursor_shape_blink =
				       (عدد & 1) ? FALSE : TRUE;
		rcs_وضعیت.tr_پیشرفت = وضعیت_GOT;
#پایان_اگر
		LOG_TRN("Received cursor shape response: %s", tp);

		کلید_name[0] = (صحيح)KS_EXTRA;
		کلید_name[1] = (صحيح)KE_IGNORE;
		*slen = i + 1;
#اگر_تعریف_شده FEAT_EVAL
		مجموعه_vim_var_string(VV_TERMSTYLERESP, tp, *slen);
#پایان_اگر
		apply_autocmds(EVENT_TERMRESPONSEALL,
					(char_u *)"cursorshape", عدم, FALSE, curbuf);
		اكسر;
	    }
	}
    }

    إذا (i == len)
    {
	// These codes arrive many together, each code can be
	// truncated at any point.
	LOG_TR1("not enough characters for XT");
	أرجع FAIL;
    }
    أرجع OK;
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
    صحيح
check_termcode(
    صحيح		max_جابه_جایی,
    char_u	*buf,
    صحيح		bufsize,
    صحيح		*buflen)
{
    char_u	*tp;
    char_u	*p;
    صحيح		slen = 0;	// init for GCC
    صحيح		modslen;
    صحيح		len;
    صحيح		retval = 0;
    صحيح		جابه_جایی;
    char_u	کلید_name[2];
    صحيح		modifiers;
    char_u	*modifiers_شروع = عدم;
    صحيح		کلید;
    صحيح		new_slen;   // Length of what will replace the termcode
    char_u	string[MAX_کلید_کد_LEN + 1];
    صحيح		i, j;
    صحيح		idx = 0;
    صحيح		cpo_koffset;

    cpo_koffset = (vim_strchr(p_cpo, CPO_KOFFSET) != عدم);

    /*
     * Speed up the checks for terminal codes by gathering all first bytes
     * used in termleader[].  Often this is just a single <Esc>.
     */
    إذا (need_gather)
	gather_termleader();

    /*
     * Check at several positions in typebuf.tb_buf[], to catch something like
     * "x<Up>" that can be mapped. Stop at max_offset, because characters
     * after that cannot be used for mapping, and with @r commands
     * typebuf.tb_buf[] can become very long.
     * This is used often, KEEP IT FAST!
     */
    لكل (جابه_جایی = 0; جابه_جایی < max_جابه_جایی; ++جابه_جایی)
    {
	إذا (buf == عدم)
	{
	    إذا (جابه_جایی >= typebuf.tb_len)
		اكسر;
	    tp = typebuf.tb_buf + typebuf.tb_off + جابه_جایی;
	    len = typebuf.tb_len - جابه_جایی;	// length of the input
	}
	وإلا
	{
	    إذا (جابه_جایی >= *buflen)
		اكسر;
	    tp = buf + جابه_جایی;
	    len = *buflen - جابه_جایی;
	}

	/*
	 * Don't check characters after K_SPECIAL, those are already
	 * translated terminal chars (avoid translating ~@^Hx).
	 */
	إذا (*tp == K_SPECIAL)
	{
	    جابه_جایی += 2;	// there are always 2 extra characters
	    استمر;
	}

	إذا (osc_state.processing)
	{
	    // Still processing OSC response data, go straight to handler
	    // function.
	    tp[len] = NUL;
	    کلید_name[0] = NUL;
	    کلید_name[1] = NUL;
	    modifiers = 0;
	    اذهب handle_osc;
	}

	/*
	 * Skip this position if the character does not appear as the first
	 * character in term_strings. This speeds up a lot, since most
	 * termcodes start with the same character (ESC or CSI).
	 */
	i = *tp;
	لكل (p = termleader; *p && *p != i; ++p)
	    ;
	إذا (*p == NUL)
	    استمر;

	/*
	 * Skip this position if p_ek is not set and tp[0] is an ESC and we
	 * are in Insert mode.
	 */
	إذا (*tp == ESC && !p_ek && (State & نما_درج))
	    استمر;

	tp[len] = NUL;
	کلید_name[0] = NUL;	// no key name found yet
	کلید_name[1] = NUL;	// no key name found yet
	modifiers = 0;		// no modifiers yet

#اگر_تعریف_شده FEAT_GUI
	إذا (gui.درون_use)
	{
	    /*
	     * GUI special key codes are all of the form [CSI xx].
	     */
	    إذا (*tp == CSI)	    // Special key from GUI
	    {
		إذا (len < 3)
		    أرجع -1;	    // Shouldn't happen
		slen = 3;
		کلید_name[0] = tp[1];
		کلید_name[1] = tp[2];
	    }
	}
	وإلا
#پایان_اگر // FEAT_GUI
#اگر_تعریف_شده MSWIN
	    إذا (len >= 3 && tp[0] == CSI && tp[1] == KS_EXTRA
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
	وإلا
#پایان_اگر
	{
	    صحيح  mouse_شاخص_یافت = -1;
	    صحيح keypad_شاخص_یافت = -1;
	    صحيح keypad_slen_یافت = 0;

	    لكل (idx = 0; idx < tc_len; ++idx)
	    {
		صحيح     is_keypad = FALSE;
		/*
		 * Ignore the entry if we are not at the start of
		 * typebuf.tb_buf[]
		 * and there are not enough characters to make a match.
		 * But only when the 'K' flag is in 'cpoptions'.
		 */
		slen = termcodes[idx].len;
		modifiers_شروع = عدم;
		إذا (cpo_koffset && جابه_جایی && len < slen)
		    استمر;
		إذا (STRNCMP(termcodes[idx].کد, tp,
				     (size_t)(slen > len ? len : slen)) == 0)
		{
		    صحيح	    looks_like_mouse_شروع = FALSE;

		    إذا (len < slen)		// got a partial sequence
			أرجع -1;		// need to get more chars

		    /*
		     * When found a keypad key, check if there is another key
		     * that matches and use that one.  This makes <Home> to be
		     * found instead of <kHome> when they produce the same
		     * key code.
		     */
		    إذا (termcodes[idx].name[0] == 'K'
				       && (VIM_ISDIGIT(termcodes[idx].name[1])
						|| ASCII_ISUPPER(termcodes[idx].name[1])))
		    {
			is_keypad = TRUE;
			// Only use it when there is no other match.
			إذا (keypad_شاخص_یافت < 0)
			{
			    keypad_شاخص_یافت = idx;
			    keypad_slen_یافت = slen;
			}
		    }

		    إذا (slen == 2 && len > 2
			    && termcodes[idx].کد[0] == ESC
			    && termcodes[idx].کد[1] == '[')
		    {
			// The mouse termcode "ESC [" is also the prefix of
			// "ESC [ I" (focus gained) and other keys.  Check some
			// more bytes to find out.
			إذا (!امن_isdigit(tp[2]))
			{
			    // ESC [ without number following: Only use it when
			    // there is no other match.
			    looks_like_mouse_شروع = TRUE;
			}
			وإلا إذا (termcodes[idx].name[0] == KS_DEC_MOUSE)
			{
			    char_u  *nr = tp + 2;
			    صحيح	    شمار = 0;

			    // If a digit is following it could be a key with
			    // modifier, e.g., ESC [ 1;2P.  Can be confused
			    // with DEC_MOUSE, which requires four numbers
			    // following.  If not then it can't be a DEC_MOUSE
			    // code.
			    لكل (;;)
			    {
				++شمار;
				(فراغ)getdigits(&nr);
				إذا (nr >= tp + len)
				    أرجع -1;	// partial sequence
				إذا (*nr != ';')
				    اكسر;
				++nr;
				إذا (nr >= tp + len)
				    أرجع -1;	// partial sequence
			    }
			    إذا (شمار < 4)
				استمر;	// no match
			}
		    }
		    إذا (looks_like_mouse_شروع)
		    {
			// Only use it when there is no other match.
			إذا (mouse_شاخص_یافت < 0)
			    mouse_شاخص_یافت = idx;
		    }
		    وإلا إذا (!is_keypad)
		    {
			کلید_name[0] = termcodes[idx].name[0];
			کلید_name[1] = termcodes[idx].name[1];
			اكسر;
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
		إذا (termcodes[idx].modlen > 0 && mouse_شاخص_یافت < 0)
		{
		    modslen = termcodes[idx].modlen;
		    إذا (cpo_koffset && جابه_جایی && len < modslen)
			استمر;
		    إذا (STRNCMP(termcodes[idx].کد, tp,
				(size_t)(modslen > len ? len : modslen)) == 0)
		    {
			صحيح	    n;

			إذا (len <= modslen)	// got a partial sequence
			    أرجع -1;		// need to get more chars

			إذا (tp[modslen] == termcodes[idx].کد[slen - 1])
			    // no modifiers
			    slen = modslen + 1;
			وإلا إذا (tp[modslen] != ';' && modslen == slen - 3)
			    // no match for "code;*X" with "code;"
			    استمر;
			وإلا إذا (termcodes[idx].کد[modslen] == '@'
					 && (tp[modslen] != '1'
						    || tp[modslen + 1] != ';'))
			    // no match for "<Esc>[@" with "<Esc>[1;"
			    استمر;
			وإلا
			{
			    // Skip over the digits, the final char must
			    // follow. URXVT can use a negative value, thus
			    // also accept '-'.
			    لكل (j = slen - 2; j < len && (امن_isdigit(tp[j])
				       || tp[j] == '-' || tp[j] == ';'); ++j)
				;
			    ++j;
			    إذا (len < j)	// got a partial sequence
				أرجع -1;	// need to get more chars
			    إذا (tp[j - 1] != termcodes[idx].کد[slen - 1])
				استمر;	// no match

			    modifiers_شروع = tp + slen - 2;

			    // Match!  Convert modifier bits.
			    n = atoi((حرف *)modifiers_شروع);
			    modifiers |= decode_modifiers(n);

			    slen = j;
			}
			إذا (termcodes[idx].name[0] == 'K'
					   && (VIM_ISDIGIT(termcodes[idx].name[1])
						    || ASCII_ISUPPER(termcodes[idx].name[1])))
			{
			    is_keypad = TRUE;
			    إذا (keypad_شاخص_یافت < 0)
			    {
				keypad_شاخص_یافت = idx;
				keypad_slen_یافت = slen;
			    }
			}
			إذا (!is_keypad)
			{
			    کلید_name[0] = termcodes[idx].name[0];
			    کلید_name[1] = termcodes[idx].name[1];
			    اكسر;
			}
		    }
		}
	    }
	    إذا (idx == tc_len && keypad_شاخص_یافت >= 0)
	    {
		کلید_name[0] = termcodes[keypad_شاخص_یافت].name[0];
		کلید_name[1] = termcodes[keypad_شاخص_یافت].name[1];
		slen = keypad_slen_یافت;
	    }
	    وإلا إذا (idx == tc_len && mouse_شاخص_یافت >= 0)
	    {
		کلید_name[0] = termcodes[mouse_شاخص_یافت].name[0];
		کلید_name[1] = termcodes[mouse_شاخص_یافت].name[1];
	    }
	}

	إذا (کلید_name[0] == NUL
	    // Mouse codes of DEC and pterm start with <ESC>[.  When
	    // detecting the start of these mouse codes they might as well be
	    // another key code or terminal response.
#اگر_تعریف_شده FEAT_MOUSE_DEC
	    || کلید_name[0] == KS_DEC_MOUSE
#پایان_اگر
#اگر_تعریف_شده FEAT_MOUSE_PTERM
	    || کلید_name[0] == KS_PTERM_MOUSE
#پایان_اگر
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
	    إذا (((tp[0] == ESC && len >= 3 && tp[1] == '[')
			|| (tp[0] == CSI && len >= 2))
		    && vim_strchr((char_u *)"0123456789>?ABCDEFHPQRS",
								*argp) != عدم)
	    {
		صحيح resp = handle_csi(tp, len, argp, جابه_جایی, buf,
					     bufsize, buflen, کلید_name, &slen);
		إذا (resp != 0)
		{
#اگر_تعریف_شده DEBUG_TERMRESPONSE
		    إذا (resp == -1)
			LOG_TR1("Not enough characters for CSI sequence");
#پایان_اگر
		    أرجع resp;
		}
	    }

	    // Check for OSC responses from terminal
	    وإلا إذا ((tp[0] == ESC && len >= 2 && tp[1] == ']') || tp[0] == OSC)
	    {
handle_osc:
		إذا (handle_osc(tp, len, کلید_name, &slen) == FAIL)
		    أرجع -1;
	    }

	    // Check for key code response from xterm,
	    // starting with <Esc>P or DCS
	    // It would only be needed with this condition:
	    //	    (check_for_codes || rcs_status.tr_progress == STATUS_SENT)
	    // Now this is always done so that DCS codes don't mess up things.
	    وإلا إذا ((tp[0] == ESC && len >= 2 && tp[1] == 'P') || tp[0] == DCS)
	    {
		إذا (handle_dcs(tp, argp, len, کلید_name, &slen) == FAIL)
		    أرجع -1;
	    }
	}

	إذا (کلید_name[0] == NUL)
	    استمر;	    // No match at this position, try next one

	// We only get here when we have a complete termcode match

#إذا defined(FEAT_GUI) || defined(MSWIN)
	/*
	 * For scroll events from the GUI or MS-Windows console, fetch the
	 * pointer coordinates so that we know which window to scroll later.
	 */
	إذا (TRUE
# إذا defined(FEAT_GUI) && !defined(MSWIN)
		&& gui.درون_use
# endif
		&& کلید_name[0] == (صحيح)KS_EXTRA
		&& (کلید_name[1] == (صحيح)KE_X1MOUSE
		    || کلید_name[1] == (صحيح)KE_X2MOUSE
		    || کلید_name[1] == (صحيح)KE_MOUSEMOVE_XY
		    || کلید_name[1] == (صحيح)KE_MOUSELEFT
		    || کلید_name[1] == (صحيح)KE_MOUSERIGHT
		    || کلید_name[1] == (صحيح)KE_MOUSEDOWN
		    || کلید_name[1] == (صحيح)KE_MOUSEUP))
	{
	    char_u	بایتها[6];
	    صحيح		رقم_بایتها = get_بایتها_from_buf(tp + slen, بایتها, 4);

	    إذا (رقم_بایتها == -1)	// not enough coordinates
		أرجع -1;
	    mouse_ستون = 128 * (بایتها[0] - ' ' - 1) + بایتها[1] - ' ' - 1;
	    mouse_ردیف = 128 * (بایتها[2] - ' ' - 1) + بایتها[3] - ' ' - 1;
	    slen += رقم_بایتها;
	    // equal to K_MOUSEMOVE
	    إذا (کلید_name[1] == (صحيح)KE_MOUSEMOVE_XY)
		کلید_name[1] = (صحيح)KE_MOUSEMOVE;
	}
	وإلا
#پایان_اگر
	/*
	 * If it is a mouse click, get the coordinates.
	 */
	إذا (کلید_name[0] == KS_MOUSE
#اگر_تعریف_شده FEAT_MOUSE_GPM
		|| کلید_name[0] == KS_GPM_MOUSE
#پایان_اگر
#اگر_تعریف_شده FEAT_MOUSE_JSB
		|| کلید_name[0] == KS_JSBTERM_MOUSE
#پایان_اگر
#اگر_تعریف_شده FEAT_MOUSE_NET
		|| کلید_name[0] == KS_NETTERM_MOUSE
#پایان_اگر
#اگر_تعریف_شده FEAT_MOUSE_DEC
		|| کلید_name[0] == KS_DEC_MOUSE
#پایان_اگر
#اگر_تعریف_شده FEAT_MOUSE_PTERM
		|| کلید_name[0] == KS_PTERM_MOUSE
#پایان_اگر
#اگر_تعریف_شده FEAT_MOUSE_URXVT
		|| کلید_name[0] == KS_URXVT_MOUSE
#پایان_اگر
		|| کلید_name[0] == KS_SGR_MOUSE
		|| کلید_name[0] == KS_SGR_MOUSE_RELEASE)
	{
	    إذا (check_termcode_mouse(tp, &slen, کلید_name, modifiers_شروع, idx,
							     &modifiers) == -1)
		أرجع -1;
	}

#اگر_تعریف_شده FEAT_GUI
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
	وإلا إذا (کلید_name[0] == (صحيح)KS_MENU)
	{
	    long_u	val;
	    صحيح		رقم_بایتها = get_long_from_buf(tp + slen, &val);

	    إذا (رقم_بایتها == -1)
		أرجع -1;
	    current_menu = (vimmenu_T *)val;
	    slen += رقم_بایتها;

	    // The menu may have been deleted right after it was used, check
	    // for that.
	    إذا (check_menu_اشاره_گر(ریشه_menu, current_menu) == FAIL)
	    {
		کلید_name[0] = KS_EXTRA;
		کلید_name[1] = (صحيح)KE_IGNORE;
	    }
	}
# endif
# ifdef FEAT_GUI_TABLINE
	وإلا إذا (کلید_name[0] == (صحيح)KS_TABLINE)
	{
	    // Selecting tabline tab or using its menu.
	    char_u	بایتها[6];
	    صحيح		رقم_بایتها = get_بایتها_from_buf(tp + slen, بایتها, 1);

	    إذا (رقم_بایتها == -1)
		أرجع -1;
	    current_tab = (صحيح)بایتها[0];
	    إذا (current_tab == 255)	// -1 in a byte gives 255
		current_tab = -1;
	    slen += رقم_بایتها;
	}
	وإلا إذا (کلید_name[0] == (صحيح)KS_TABMENU)
	{
	    // Selecting tabline tab or using its menu.
	    char_u	بایتها[6];
	    صحيح		رقم_بایتها = get_بایتها_from_buf(tp + slen, بایتها, 2);

	    إذا (رقم_بایتها == -1)
		أرجع -1;
	    current_tab = (صحيح)بایتها[0];
	    current_tabmenu = (صحيح)بایتها[1];
	    slen += رقم_بایتها;
	}
# endif
# ifndef USE_ON_FLY_SCROLL
	وإلا إذا (کلید_name[0] == (صحيح)KS_VER_SCROLLBAR)
	{
	    long_u	val;
	    char_u	بایتها[6];
	    صحيح		رقم_بایتها;

	    // Get the last scrollbar event in the queue of the same type
	    j = 0;
	    لكل (i = 0; tp[j] == CSI && tp[j + 1] == KS_VER_SCROLLBAR
						     && tp[j + 2] != NUL; ++i)
	    {
		j += 3;
		رقم_بایتها = get_بایتها_from_buf(tp + j, بایتها, 1);
		إذا (رقم_بایتها == -1)
		    اكسر;
		إذا (i == 0)
		    current_scrollbar = (صحيح)بایتها[0];
		وإلا إذا (current_scrollbar != (صحيح)بایتها[0])
		    اكسر;
		j += رقم_بایتها;
		رقم_بایتها = get_long_from_buf(tp + j, &val);
		إذا (رقم_بایتها == -1)
		    اكسر;
		scrollbar_مقدار = val;
		j += رقم_بایتها;
		slen = j;
	    }
	    إذا (i == 0)		// not enough characters to make one
		أرجع -1;
	}
	وإلا إذا (کلید_name[0] == (صحيح)KS_HOR_SCROLLBAR)
	{
	    long_u	val;
	    صحيح		رقم_بایتها;

	    // Get the last horiz. scrollbar event in the queue
	    j = 0;
	    لكل (i = 0; tp[j] == CSI && tp[j + 1] == KS_HOR_SCROLLBAR
						     && tp[j + 2] != NUL; ++i)
	    {
		j += 3;
		رقم_بایتها = get_long_from_buf(tp + j, &val);
		إذا (رقم_بایتها == -1)
		    اكسر;
		scrollbar_مقدار = val;
		j += رقم_بایتها;
		slen = j;
	    }
	    إذا (i == 0)		// not enough characters to make one
		أرجع -1;
	}
# endif // !USE_ON_FLY_SCROLL
#پایان_اگر // FEAT_GUI

#إذا defined(UNIX) || defined(VMS)
	/*
	 * Handle FocusIn/FocusOut event sequences reported by XTerm.
	 * (CSI I/CSI O)
	 */
	إذا (کلید_name[0] == KS_EXTRA
# ifdef FEAT_GUI
		&& !gui.درون_use
# endif
	    )
	{
	    إذا (کلید_name[1] == KE_FOCUSGAINED)
	    {
		إذا (!focus_state)
		{
		    ui_focus_change(TRUE);
		    did_cursorhold = TRUE;
		    focus_state = TRUE;
		}
		کلید_name[1] = (صحيح)KE_IGNORE;
	    }
	    وإلا إذا (کلید_name[1] == KE_FOCUSLOST)
	    {
		إذا (focus_state)
		{
		    ui_focus_change(FALSE);
		    did_cursorhold = TRUE;
		    focus_state = FALSE;
		}
		کلید_name[1] = (صحيح)KE_IGNORE;
	    }
	}
#پایان_اگر

	/*
	 * Change <xHome> to <Home>, <xUp> to <Up>, etc.
	 */
	کلید = handle_x_کلیدها(TERMCAP2_کلید(کلید_name[0], کلید_name[1]));

	// Add any modifier codes to our string.
	new_slen = modifiers2keycode(modifiers, &کلید, string);

	// Finally, add the special key code to our string
	کلید_name[0] = KEY2TERMCAP0(کلید);
	کلید_name[1] = KEY2TERMCAP1(کلید);
	إذا (کلید_name[0] == KS_کلید)
	{
	    // from ":set <M-b>=xx"
	    إذا (has_mbyte)
		new_slen += (*mb_char2bytes)(کلید_name[1], string + new_slen);
	    وإلا
		string[new_slen++] = کلید_name[1];
	}
	وإلا إذا (osc_state.processing ||
		(new_slen == 0
		 && کلید_name[0] == KS_EXTRA
		 && کلید_name[1] == KE_IGNORE))
	{
	    // Do not put K_IGNORE into the buffer, do return KEYLEN_REMOVED
	    // to indicate what happened.
	    retval = KEYLEN_REMOVED;
	}
	وإلا
	{
	    string[new_slen++] = K_SPECIAL;
	    string[new_slen++] = کلید_name[0];
	    string[new_slen++] = کلید_name[1];
	}
	إذا (put_string_درون_typebuf(جابه_جایی, slen, string, new_slen,
						 buf, bufsize, buflen) == FAIL)
	    أرجع -1;
	أرجع retval == 0 ? (len + new_slen - slen + جابه_جایی) : retval;
    }

#اگر_تعریف_شده FEAT_TERMRESPONSE
    LOG_TR1("normal character");
#پایان_اگر

    أرجع 0;			    // no match found
}

#إذا defined(FEAT_TERMINAL) && defined(FEAT_TERMRESPONSE)
/*
 * Get the text foreground color, if known.
 */
    فراغ
جمله_get_fg_color(char_u *r, char_u *g, char_u *b)
{
    إذا (rfg_وضعیت.tr_پیشرفت != وضعیت_GOT)
	أرجع;

    *r = fg_r;
    *g = fg_g;
    *b = fg_b;
}
#پایان_اگر

#اگر_تعریف_شده FEAT_TERMRESPONSE
/*
 * Get the text background color, if known.  Returns OK when the OSC 11
 * response has been parsed and *r, *g, *b are populated; FAIL otherwise.
 */
    صحيح
جمله_get_bg_color(char_u *r, char_u *g, char_u *b)
{
    إذا (rbg_وضعیت.tr_پیشرفت != وضعیت_GOT)
	أرجع FAIL;

    *r = bg_r;
    *g = bg_g;
    *b = bg_b;
    أرجع OK;
}
#پایان_اگر

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
    scid_T	sid_arg استفادهنشده,	// script ID to use for <SID>,
				// or 0 to use current_sctx
    صحيح		flags,
    صحيح		*did_simplify)
{
    صحيح		i;
    صحيح		slen;
    صحيح		کلید;
    size_t	dlen = 0;
    char_u	*src;
    صحيح		do_backslash;	// backslash is a special character
    صحيح		do_special;	// recognize <> key codes
    صحيح		do_کلید_کد;	// recognize raw key codes
    char_u	*نتیجه;	// buffer for resulting string
    garray_T	ga;

    do_backslash = (vim_strchr(p_cpo, CPO_BSLASH) == عدم);
    do_special = (vim_strchr(p_cpo, CPO_SPECI) == عدم)
						  || (flags & REPTERM_SPECIAL);
    do_کلید_کد = (vim_strchr(p_cpo, CPO_KEYCODE) == عدم);
    src = from;

    /*
     * Allocate space for the translation.  Worst case a single character is
     * replaced by 6 bytes (shifted special key), plus a NUL at the end.
     * In the rare case more might be needed ga_grow() must be called again.
     */
    ga_init2(&ga, 1L, 100);
    إذا (ga_grow(&ga, (صحيح)(STRLEN(src) * 6 + 1)) == FAIL) // out of memory
    {
	*bufp = عدم;
	أرجع from;
    }
    نتیجه = ga.ga_داده;

    /*
     * Check for #n at start only: function key n
     */
    إذا ((flags & REPTERM_FROM_PART) && src[0] == '#' && VIM_ISDIGIT(src[1]))
    {
	نتیجه[dlen++] = K_SPECIAL;
	نتیجه[dlen++] = 'k';
	إذا (src[1] == '0')
	    نتیجه[dlen++] = ';';	// #0 is F10 is "k;"
	وإلا
	    نتیجه[dlen++] = src[1];	// #3 is F3 is "k3"
	src += 2;
    }

    /*
     * Copy each byte from *from to result[dlen]
     */
    طالما (*src != NUL)
    {
	/*
	 * If 'cpoptions' does not contain '<', check for special key codes,
	 * like "<C-S-LeftMouse>"
	 */
	إذا (do_special && ((flags & REPTERM_DO_LT)
					      || STRNCMP(src, "<lt>", 4) != 0))
	{
#اگر_تعریف_شده FEAT_EVAL
	    /*
	     * Change <SID>Func to K_SNR <script-nr> _Func.  This name is used
	     * for script-local user functions.
	     * (room: 5 * 6 = 30 bytes; needed: 3 + <nr> + 1 <= 14)
	     * Also change <SID>name.Func to K_SNR <import-script-nr> _Func.
	     * Only if "name" is recognized as an import.
	     */
	    إذا (STRNICMP(src, "<SID>", 5) == 0)
	    {
		إذا (sid_arg < 0 || (sid_arg == 0 && current_sctx.sc_sid <= 0))
		    emsg(_(e_using_sid_not_درون_script_context));
		وإلا
		{
		    char_u  *dot;
		    دراز    sid = sid_arg != 0 ? sid_arg : current_sctx.sc_sid;

		    src += 5;
		    إذا (درون_vim9script()
				       && (dot = vim_strchr(src, '.')) != عدم)
		    {
			imported_T *imp = پیدا_کن_imported(src, dot - src, FALSE);

			إذا (imp != عدم)
			{
			    scriptitem_T    *si = SCRIPT_قلم(imp->imp_sid);
			    size_t	    len;

			    src = dot + 1;
			    إذا (si->sn_autoload_پیشوند != عدم)
			    {
				// Turn "<SID>name.Func"
				// into "scriptname#Func".
				len = STRLEN(si->sn_autoload_پیشوند);
				إذا (ga_grow(&ga,
				     (صحيح)(STRLEN(src) * 6 + len + 1)) == FAIL)
				{
				    ga_clear(&ga);
				    *bufp = عدم;
				    أرجع from;
				}
				نتیجه = ga.ga_داده;
				STRCPY(نتیجه + dlen, si->sn_autoload_پیشوند);
				dlen += len;
				استمر;
			    }
			    sid = imp->imp_sid;
			}
		    }

		    نتیجه[dlen++] = K_SPECIAL;
		    نتیجه[dlen++] = (صحيح)KS_EXTRA;
		    نتیجه[dlen++] = (صحيح)KE_SNR;
		    sprintf((حرف *)نتیجه + dlen, "%ld", sid);
		    dlen += STRLEN(نتیجه + dlen);
		    نتیجه[dlen++] = '_';
		    استمر;
		}
	    }
#پایان_اگر
	    صحيح fsk_flags = FSK_KEYCODE
			| ((flags & REPTERM_NO_SIMPLIFY) ? 0 : FSK_SIMPLIFY)
			| ((flags & REPTERM_FROM_PART) ? FSK_FROM_PART : 0);
	    slen = trans_special(&src, نتیجه + dlen, fsk_flags,
							   TRUE, did_simplify);
	    إذا (slen > 0)
	    {
		dlen += slen;
		استمر;
	    }
	}

	/*
	 * If 'cpoptions' does not contain 'k', see if it's an actual key-code.
	 * Note that this is also checked after replacing the <> form.
	 * Single character codes are NOT replaced (e.g. ^H or DEL), because
	 * it could be a character in the file.
	 */
	إذا (do_کلید_کد)
	{
	    صحيح len;
	    i = پیدا_کن_جمله_bykeys(src, &len);
	    إذا (i >= 0)
	    {
		نتیجه[dlen++] = K_SPECIAL;
		نتیجه[dlen++] = termcodes[i].name[0];
		نتیجه[dlen++] = termcodes[i].name[1];
		src += len;
		// If terminal code matched, continue after it.
		استمر;
	    }
	}

#اگر_تعریف_شده FEAT_EVAL
	إذا (do_special)
	{
	    char_u	*p, *s, len;

	    /*
	     * Replace <Leader> by the value of "mapleader".
	     * Replace <LocalLeader> by the value of "maplocalleader".
	     * If "mapleader" or "maplocalleader" isn't set use a backslash.
	     */
	    إذا (STRNICMP(src, "<Leader>", 8) == 0)
	    {
		len = 8;
		p = get_var_مقدار((char_u *)"g:mapleader");
	    }
	    وإلا إذا (STRNICMP(src, "<LocalLeader>", 13) == 0)
	    {
		len = 13;
		p = get_var_مقدار((char_u *)"g:maplocalleader");
	    }
	    وإلا
	    {
		len = 0;
		p = عدم;
	    }
	    إذا (len != 0)
	    {
		// Allow up to 8 * 6 characters for "mapleader".
		إذا (p == عدم || *p == NUL || STRLEN(p) > 8 * 6)
		    s = (char_u *)"\\";
		وإلا
		    s = p;
		طالما (*s != NUL)
		    نتیجه[dlen++] = *s++;
		src += len;
		استمر;
	    }
	}
#پایان_اگر

	/*
	 * Remove CTRL-V and ignore the next character.
	 * For "from" side the CTRL-V at the end is included, for the "to"
	 * part it is removed.
	 * If 'cpoptions' does not contain 'B', also accept a backslash.
	 */
	کلید = *src;
	إذا (کلید == Ctrl_V || (do_backslash && کلید == '\\'))
	{
	    ++src;				// skip CTRL-V or backslash
	    إذا (*src == NUL)
	    {
		إذا (flags & REPTERM_FROM_PART)
		    نتیجه[dlen++] = کلید;
		اكسر;
	    }
	}

	// skip multibyte char correctly
	لكل (i = (*mb_ptr2len)(src); i > 0; --i)
	{
	    /*
	     * If the character is K_SPECIAL, replace it with K_SPECIAL
	     * KS_SPECIAL KE_FILLER.
	     * If compiled with the GUI replace CSI with K_CSI.
	     */
	    إذا (*src == K_SPECIAL)
	    {
		نتیجه[dlen++] = K_SPECIAL;
		نتیجه[dlen++] = KS_SPECIAL;
		نتیجه[dlen++] = KE_FILLER;
	    }
#اگر_تعریف_شده FEAT_GUI
	    وإلا إذا (*src == CSI)
	    {
		نتیجه[dlen++] = K_SPECIAL;
		نتیجه[dlen++] = KS_EXTRA;
		نتیجه[dlen++] = (صحيح)KE_CSI;
	    }
#پایان_اگر
	    وإلا
		نتیجه[dlen++] = *src;
	    ++src;
	}
    }
    نتیجه[dlen] = NUL;

    /*
     * Copy the new string to allocated memory.
     * If this fails, just return from.
     */
    إذا ((*bufp = vim_strsave(نتیجه)) != عدم)
	from = *bufp;
    vim_free(نتیجه);
    أرجع from;
}

/*
 * Find a termcode with keys 'src' (must be NUL terminated).
 * Return the index in termcodes[], or -1 if not found.
 */
    ساكن صحيح
پیدا_کن_جمله_bykeys(char_u *src, صحيح *matchlen)
{
    صحيح		i, j;
    صحيح		len = (صحيح)STRLEN(src);
    صحيح         یافت = -1;
    // Don't return a match for a single character
    صحيح         foundlen = 1;
    صحيح         slen, modslen;
    صحيح         thislen;

    // Most input bytes cannot start a terminal code.  Reuse the same leader
    // table as check_termcode() to avoid scanning termcodes[] unnecessarily.
    إذا (need_gather)
	gather_termleader();
    إذا (*src == NUL || vim_strchr(termleader, *src) == عدم)
	أرجع -1;

    // find longest match
    // borrows part of check_termcode
    لكل (i = 0; i < tc_len; ++i)
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
	إذا (modslen > 0)
	{
	    إذا (len > modslen
			&& STRNCMP(termcodes[i].کد, src, (size_t)modslen) == 0)
	    {
		thislen = 0;

		إذا (src[modslen] == termcodes[i].کد[slen - 1])
		    // no modifiers
		    thislen = modslen + 1;
		وإلا إذا (src[modslen] != ';' && modslen == slen - 3)
		    // no match for "code;*X" with "code;"
		    استمر;
		وإلا إذا (termcodes[i].کد[modslen] == '@'
				&& (src[modslen] != '1'
					    || src[modslen + 1] != ';'))
		    // no match for "<Esc>[@" with "<Esc>[1;"
		    استمر;
		وإلا
		{
		    // Skip over the digits, the final char must
		    // follow. URXVT can use a negative value, thus
		    // also accept '-'.
		    لكل (j = slen - 2; j < len && (امن_isdigit(src[j])
			       || src[j] == '-' || src[j] == ';'); ++j)
			;
		    ++j;
		    إذا (len < j)	// got a partial sequence
			استمر;
		    إذا (src[j - 1] != termcodes[i].کد[slen - 1])
			استمر;	// no match

		    thislen = j;
		}

		إذا (thislen > foundlen)
		{
		    یافت = i;
		    foundlen = thislen;
		}
	    }
	}
	وإلا
	{
	    إذا (slen > foundlen && len >= slen
			&& STRNCMP(termcodes[i].کد, src, (size_t)slen) == 0)
	    {
		یافت = i;
		foundlen = slen;
	    }
	}
    }

    إذا (matchlen != عدم && یافت >= 0)
	*matchlen = foundlen;

    أرجع یافت;
}

/*
 * Gather the first characters in the terminal key codes into a string.
 * Used to speed up check_termcode().
 */
    ساكن فراغ
gather_termleader(فراغ)
{
    صحيح	    i;
    صحيح	    len = 0;

#اگر_تعریف_شده FEAT_GUI
    إذا (gui.درون_use)
	termleader[len++] = CSI;    // the GUI codes are not in termcodes[]
#پایان_اگر

    // Querying cell size uses CSI 14 t
#إذا defined(MSWIN) && (defined(FEAT_IMAGE) || defined(FEAT_EVAL))
# ifdef FEAT_GUI
    إذا (!gui.درون_use)
# endif
    {
	termleader[len++] = ESC;
# ifdef FEAT_GUI
	إذا (!gui.درون_use)
# endif
	    termleader[len++] = CSI;
    }
#پایان_اگر

#اگر_تعریف_شده FEAT_TERMRESPONSE
    إذا (check_for_codes || *T_CRS != NUL)
	termleader[len++] = DCS;    // the termcode response starts with DCS
				    // in 8-bit mode
#پایان_اگر
    termleader[len] = NUL;

    لكل (i = 0; i < tc_len; ++i)
	إذا (vim_strchr(termleader, termcodes[i].کد[0]) == عدم)
	{
	    termleader[len++] = termcodes[i].کد[0];
	    termleader[len] = NUL;
	}

    need_gather = FALSE;
}

/*
 * Show all termcodes (for ":set termcap")
 * This code looks a lot like showoptions(), but is different.
 * "flags" can have OPT_ONECOLUMN.
 */
    فراغ
show_termcodes(صحيح flags)
{
    صحيح		ستون;
    صحيح		*اقلام;
    صحيح		قلم_شمار;
    صحيح		پیاپی;
    صحيح		ردیف, ردیفها_2;
    صحيح		تعداد_ستون;
    صحيح		i;
    صحيح		len;

#تعریف_کن INC3 27	    // try to make three columns
#تعریف_کن INC2 40	    // try to make two columns
#تعریف_کن GAP 2	    // spaces between columns

    إذا (tc_len == 0)	    // no terminal codes (must be GUI)
	أرجع;
    اقلام = ALLOC_MULT(صحيح, tc_len);
    إذا (اقلام == عدم)
	أرجع;

    // Highlight title
    پیام_puts_title(_("\n--- Terminal keys ---"));

    /*
     * Do the loop three times:
     * 1. display the short items (non-strings and short strings)
     * 2. display the medium items (medium length strings)
     * 3. display the long items (remaining strings)
     * When "flags" has OPT_ONECOLUMN do everything in 3.
     */
    لكل (پیاپی = (flags & OPT_ONECOLUMN) ? 3 : 1; پیاپی <= 3 && !got_int; ++پیاپی)
    {
	/*
	 * collect the items in items[]
	 */
	قلم_شمار = 0;
	لكل (i = 0; i < tc_len; i++)
	{
	    len = show_one_termcode(termcodes[i].name,
						    termcodes[i].کد, FALSE);
	    إذا ((flags & OPT_ONECOLUMN) ||
		    (len <= INC3 - GAP ? پیاپی == 1
			: len <= INC2 - GAP ? پیاپی == 2
			: پیاپی == 3))
		اقلام[قلم_شمار++] = i;
	}

	/*
	 * display the items
	 */
	إذا (پیاپی <= 2)
	{
	    تعداد_ستون = (Columns + GAP) / (پیاپی == 1 ? INC3 : INC2);
	    إذا (تعداد_ستون == 0)
		تعداد_ستون = 1;
	    ردیفها_2 = (قلم_شمار + تعداد_ستون - 1) / تعداد_ستون;
	}
	وإلا	// run == 3
	    ردیفها_2 = قلم_شمار;
	لكل (ردیف = 0; ردیف < ردیفها_2 && !got_int; ++ردیف)
	{
	    پیام_putchar('\n');			// go to next line
	    إذا (got_int)			// 'q' typed in more
		اكسر;
	    ستون = 0;
	    لكل (i = ردیف; i < قلم_شمار; i += ردیفها_2)
	    {
		پیام_ستون = ستون;			// make columns
		show_one_termcode(termcodes[اقلام[i]].name,
					      termcodes[اقلام[i]].کد, TRUE);
		إذا (پیاپی == 2)
		    ستون += INC2;
		وإلا
		    ستون += INC3;
	    }
	    بیرون_flush();
	    ui_breakcheck();
	}
    }
    vim_free(اقلام);
}

/*
 * Show one termcode entry.
 * Output goes into IObuff[]
 */
    صحيح
show_one_termcode(char_u *name, char_u *کد, صحيح printit)
{
    char_u	*p;
    صحيح		len;

    إذا (name[0] > '~')
    {
	IObuff[0] = ' ';
	IObuff[1] = ' ';
	IObuff[2] = ' ';
	IObuff[3] = ' ';
    }
    وإلا
    {
	IObuff[0] = 't';
	IObuff[1] = '_';
	IObuff[2] = name[0];
	IObuff[3] = name[1];
    }
    IObuff[4] = ' ';

    p = get_special_کلید_name(TERMCAP2_کلید(name[0], name[1]), 0);
    إذا (p[1] != 't')
	STRCPY(IObuff + 5, p);
    وإلا
	IObuff[5] = NUL;
    len = (صحيح)STRLEN(IObuff);
    افعل
	IObuff[len++] = ' ';
    طالما (len < 17);
    IObuff[len] = NUL;
    إذا (کد == عدم)
	len += 4;
    وإلا
	len += vim_strsize(کد);

    إذا (printit)
    {
	پیام_puts((حرف *)IObuff);
	إذا (کد == عدم)
	    پیام_puts("NULL");
	وإلا
	    پیام_outtrans(کد);
    }
    أرجع len;
}

#إذا defined(FEAT_TERMRESPONSE)
/*
 * For Xterm >= 140 compiled with OPT_TCAP_QUERY: Obtain the actually used
 * termcap codes from the terminal itself.
 * We get them one by one to avoid a very long response string.
 */
ساكن صحيح xt_شاخص_درون = 0;
ساكن صحيح xt_شاخص_بیرون = 0;

    ساكن فراغ
req_codes_from_جمله(فراغ)
{
    xt_شاخص_بیرون = 0;
    xt_شاخص_درون = 0;
    req_بیشتر_codes_from_جمله();
}

    ساكن فراغ
req_بیشتر_codes_from_جمله(فراغ)
{
    حرف	buf[32];  // extra size to shut up LGTM
    صحيح		old_idx = xt_شاخص_بیرون;

    // Don't do anything when going to exit.
    إذا (exiting)
	أرجع;

    // Send up to 10 more requests out than we received.  Avoid sending too
    // many, there can be a buffer overflow somewhere.
    طالما (xt_شاخص_بیرون < xt_شاخص_درون + 10 && کلید_نامها[xt_شاخص_بیرون] != عدم)
    {
	حرف *کلید_name = کلید_نامها[xt_شاخص_بیرون];

	MAY_WANT_مقصد_LOG_THIS;
	LOG_TRN("Requesting XT %d: %s", xt_شاخص_بیرون, کلید_name);
	إذا (کلید_name[2] != NUL)
	    sprintf(buf, "\033P+q%02x%02x%02x\033\\", کلید_name[0], کلید_name[1], کلید_name[2]);
	وإلا
	    sprintf(buf, "\033P+q%02x%02x\033\\", کلید_name[0], کلید_name[1]);
	بیرون_str_nf((char_u *)buf);
	++xt_شاخص_بیرون;
    }

    // Send the codes out right away.
    إذا (xt_شاخص_بیرون != old_idx)
	بیرون_flush();
}

/*
 * Decode key code response from xterm:
 * '<Esc>P1+r<name>=<string><Esc>\' if it is enabled/supported
 * '<Esc>P0+r<Esc>\'                if it not enabled
 * A "0" instead of the "1" indicates a code that isn't supported.
 * Both <name> and <string> are encoded in hex.
 * "code" points to the "0" or "1".
 */
    ساكن فراغ
got_کد_from_جمله(char_u *کد, صحيح len)
{
# define XT_LEN 100
    char_u	name[4];
    char_u	str[XT_LEN];
    صحيح		i;
    صحيح		j = 0;
    صحيح		c;

    // A '1' means the code is supported, a '0' means it isn't.
    // If it is supported, there must be a '=' following
    // When half the length is > XT_LEN we can't use it.
    إذا (کد[0] == '1' && (کد[7] == '=' || کد[9] == '=') && len / 2 < XT_LEN)
    {
	// Get the name from the response and find it in the table.
	name[0] = hexhex2nr(کد + 3);
	name[1] = hexhex2nr(کد + 5);
	إذا (کد[9] == '=')
	    name[2] = hexhex2nr(کد + 7);
	وإلا
	    name[2] = NUL;
	name[3] = NUL;
	لكل (i = 0; کلید_نامها[i] != عدم; ++i)
	{
	    إذا (STRCMP(کلید_نامها[i], name) == 0)
	    {
		xt_شاخص_درون = i;
		اكسر;
	    }
	}

	LOG_TRN("Received XT %d: %s", xt_شاخص_درون, (حرف *)name);

	إذا (کلید_نامها[i] != عدم)
	{
	    i = (کد[7] == '=') ? 8 : 10;
	    لكل (; (c = hexhex2nr(کد + i)) >= 0; i += 2)
		str[j++] = c;
	    str[j] = NUL;
	    إذا (name[0] == 'C' && name[1] == 'o')
	    {
		// Color count is not a key code.
		صحيح val = atoi((حرف *)str);
# إذا defined(FEAT_EVAL)
		إذا (val == t_colors)
		    ch_log(عدم, "got_code_from_term(Co): no change (%d)", val);
		وإلا
		    ch_log(عدم,
			       "got_code_from_term(Co): changed from %d to %d",
								t_colors, val);
# endif
		may_adjust_color_شمار(val);
	    }
# إذا 0
#  ifdef FEAT_TERMGUICOLORS
	    // when RGB result comes back, it is supported when the result contains an '='
	    وإلا إذا (name[0] == 'R' && name[1] == 'G' && name[2] == 'B' && کد[9] == '=')
	    {
		صحيح val = atoi((حرف *)str);
		// only enable it, if termguicolors hasn't been set yet and
		// there are 8 bits per color channel
		إذا (val == 8 && !p_tgc_مجموعه)
		{
#   ifdef FEAT_EVAL
		    ch_log(عدم, "got_code_from_term(RGB): xterm-direct colors detected");
#   endif
		    // RGB capability set, enable termguicolors
		    مجموعه_option_مقدار((char_u *)"termguicolors", 1L, عدم, 0);
		}
	    }
#  endif
# endif
	    وإلا
	    {
		i = پیدا_کن_جمله_bykeys(str, عدم);
		إذا (i >= 0 && name[0] == termcodes[i].name[0]
					    && name[1] == termcodes[i].name[1])
		{
		    // Existing entry with the same name and code - skip.
# ifdef FEAT_EVAL
		    ch_log(عدم, "got_code_from_term(): Entry %c%c did not change",
							     name[0], name[1]);
# endif
		}
		وإلا إذا (i >= 0 && name[0] == 'K' && VIM_ISDIGIT(name[1]))
		{
		    // Would replace existing entry with keypad key - skip.
# ifdef FEAT_EVAL
		    ch_log(عدم, "got_code_from_term(): Skipping entry %c%c in favor of %c%c with matching keys %s",
			    name[0], name[1], termcodes[i].name[0], termcodes[i].name[1], str);
# endif
		}
		وإلا
		{
		    إذا (i >= 0)
		    {
			// Delete an existing entry using the same code.
# ifdef FEAT_EVAL
			ch_log(عدم, "got_code_from_term(): Deleting entry %c%c with matching keys %s",
			      termcodes[i].name[0], termcodes[i].name[1], str);
# endif
			del_termcode_idx(i);
		    }
# ifdef FEAT_EVAL
		    وإلا
			ch_log(عدم, "got_code_from_term(): Adding entry %c%c with keys %s",
							name[0], name[1], str);
# endif
		    جمع_کن_termcode(name, str, ATC_FROM_جمله);
		}
	    }
	}
    }

    // May request more codes now that we received one.
    ++xt_شاخص_درون;
    req_بیشتر_codes_from_جمله();
}

/*
 * Check if there are any unanswered requests and deal with them.
 * This is called before starting an external program or getting direct
 * keyboard input.  We don't want responses to be send to that program or
 * handled as typed text.
 */
    ساكن فراغ
check_for_codes_from_جمله(فراغ)
{
    صحيح		c;

    // If no codes requested or all are answered, no need to wait.
    إذا (xt_شاخص_بیرون == 0 || xt_شاخص_بیرون == xt_شاخص_درون)
	أرجع;

    // Vgetc() will check for and handle any response.
    // Keep calling vpeekc() until we don't get any responses.
    ++no_mapping;
    ++allow_کلیدها;
    لكل (;;)
    {
	c = vpeekc();
	إذا (c == NUL)	    // nothing available
	    اكسر;

	// If a response is recognized it's replaced with K_IGNORE, must read
	// it from the input stream.  If there is no K_IGNORE we can't do
	// anything, break here (there might be some responses further on, but
	// we don't want to throw away any typed chars).
	إذا (c != K_SPECIAL && c != K_IGNORE)
	    اكسر;
	c = vgetc();
	إذا (c != K_IGNORE)
	{
	    vungetc(c);
	    اكسر;
	}
    }
    --no_mapping;
    --allow_کلیدها;
}
#پایان_اگر

#إذا defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
ساكن حرف ksme_str[20];
ساكن حرف ksmr_str[20];
ساكن حرف ksmd_str[20];

/*
 * For Win32 console: update termcap codes for existing console attributes.
 */
    فراغ
update_tcap(صحيح attr)
{
    sprintf(ksme_str, "\033|%dm", attr);
    sprintf(ksmd_str, "\033|%dm", attr | 0x08);  // FOREGROUND_INTENSITY
    sprintf(ksmr_str, "\033|%dm", ((attr & 0x0F) << 4) | ((attr & 0xF0) >> 4));

    tcap_مدخل_T *p = پیدا_کن_builtin_جمله(DEFAULT_جمله);
    إذا (p == عدم)  // did not find it
	أرجع;
    طالما (p->bt_string != عدم)
    {
	إذا (p->bt_مدخل == (صحيح)KS_ME)
	    p->bt_string = &ksme_str[0];
	وإلا إذا (p->bt_مدخل == (صحيح)KS_MR)
	    p->bt_string = &ksmr_str[0];
	وإلا إذا (p->bt_مدخل == (صحيح)KS_MD)
	    p->bt_string = &ksmd_str[0];
	++p;
    }
}

# ifdef FEAT_TERMGUICOLORS
#  define KSSIZE 20

تعريفنوع تعداد
{
    CMODE_نمایهای = 0,	// Use cmd.exe 4bit palette.
    CMODE_RGB,		// Use 24bit RGB colors using VTP.
    CMODE_256_ستون,	// Emulate xterm's 256-color palette using VTP.
    CMODE_آخرین,
} cmode_T;

بنية ks_tbl_S
{
    صحيح  کد;				// value of KS_
    حرف *vtp;				// code in RGB mode
    حرف *vtp2;				// code in 256color mode
    حرف buf[CMODE_آخرین][KSSIZE];	// real buffer
};

ساكن بنية ks_tbl_S ks_tbl[] =
{
    {(صحيح)KS_ME,  "\033|0m",  "\033|0m", {""}},   //نرمال
    {(صحيح)KS_MR,  "\033|7m",  "\033|7m", {""}},   //معکوس
    {(صحيح)KS_MD,  "\033|1m",  "\033|1m", {""}},   // bold
    {(صحيح)KS_SO,  "\033|91m", "\033|91m", {""}},  // standout: bright red text
    {(صحيح)KS_SE,  "\033|39m", "\033|39m", {""}},  // standout end: default color
    {(صحيح)KS_CZH, "\033|3m",  "\033|3m", {""}},   // italic
    {(صحيح)KS_CZR, "\033|0m",  "\033|0m", {""}},   // italic end
    {(صحيح)KS_US,  "\033|4m",  "\033|4m", {""}},   // underscore
    {(صحيح)KS_UE,  "\033|24m", "\033|24m", {""}},  // underscore end
#  ifdef TERMINFO
    {(صحيح)KS_CAB, "\033|%p1%db", "\033|%p14%dm", {""}}, // set background color
    {(صحيح)KS_CAF, "\033|%p1%df", "\033|%p13%dm", {""}}, // set foreground color
    {(صحيح)KS_CS,  "\033|%p1%d;%p2%dR", "\033|%p1%d;%p2%dR", {""}},
    {(صحيح)KS_CSV, "\033|%p1%d;%p2%dV", "\033|%p1%d;%p2%dV", {""}},
#  وإلا
    {(صحيح)KS_CAB, "\033|%db", "\033|4%dm", {""}}, // set background color
    {(صحيح)KS_CAF, "\033|%df", "\033|3%dm", {""}}, // set foreground color
    {(صحيح)KS_CS,  "\033|%d;%dR", "\033|%d;%dR", {""}},
    {(صحيح)KS_CSV, "\033|%d;%dV", "\033|%d;%dV", {""}},
#  endif
    {(صحيح)KS_CCO, "256", "256", {""}},	    // colors
    {(صحيح)KS_NAME, عدم, عدم, {""}}			    // terminator
};

/*
 * Find the first entry for "code" in the builtin termcap for "name".
 * Returns NULL when not found.
 */
    ساكن tcap_مدخل_T *
پیدا_کن_اولی_tcap(
    char_u *name,
    صحيح	    کد)
{
    tcap_مدخل_T *p = پیدا_کن_builtin_جمله(name);
    إذا (p == عدم)
	أرجع عدم;
    طالما (p->bt_string != عدم)
    {
	إذا (p->bt_مدخل == کد)
	    أرجع p;
	++p;
    }
    أرجع عدم;
}
# endif

/*
 * For Win32 console: replace the sequence immediately after termguicolors.
 */
    فراغ
جابجا_tcap(فراغ)
{
# ifdef FEAT_TERMGUICOLORS
    ساكن صحيح		init_انجامشده_2 = FALSE;
    ساكن cmode_T	curr_نما;
    بنية ks_tbl_S	*ks;
    cmode_T		نما;

    إذا (!init_انجامشده_2)
    {
	لكل (ks = ks_tbl; ks->کد != (صحيح)KS_NAME; ks++)
	{
	    tcap_مدخل_T *bt = پیدا_کن_اولی_tcap(DEFAULT_جمله, ks->کد);
	    إذا (bt != عدم)
	    {
		// Preserve the original value.
		STRNCPY(ks->buf[CMODE_نمایهای], bt->bt_string, KSSIZE);
		STRNCPY(ks->buf[CMODE_RGB], ks->vtp, KSSIZE);
		STRNCPY(ks->buf[CMODE_256_ستون], ks->vtp2, KSSIZE);

		bt->bt_string = ks->buf[CMODE_نمایهای];
	    }
	}
	init_انجامشده_2 = TRUE;
	curr_نما = CMODE_نمایهای;
    }

    إذا (p_tgc)
	نما = CMODE_RGB;
    وإلا إذا (t_colors >= 256)
	نما = CMODE_256_ستون;
    وإلا
	نما = CMODE_نمایهای;

    إذا (نما == curr_نما)
	أرجع;

    لكل (ks = ks_tbl; ks->کد != (صحيح)KS_NAME; ks++)
    {
	tcap_مدخل_T *bt = پیدا_کن_اولی_tcap(DEFAULT_جمله, ks->کد);
	إذا (bt != عدم)
	    bt->bt_string = ks->buf[نما];
    }

    curr_نما = نما;
# endif
}

#پایان_اگر


#إذا (defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))) || defined(FEAT_TERMINAL)
ساكن ثابت صحيح cube_مقدار[] = {
    0x00, 0x5F, 0x87, 0xAF, 0xD7, 0xFF
};

ساكن ثابت صحيح grey_ramp[] = {
    0x08, 0x12, 0x1C, 0x26, 0x30, 0x3A, 0x44, 0x4E, 0x58, 0x62, 0x6C, 0x76,
    0x80, 0x8A, 0x94, 0x9E, 0xA8, 0xB2, 0xBC, 0xC6, 0xD0, 0xDA, 0xE4, 0xEE
};

ساكن ثابت char_u ansi_جدول[16][3] = {
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

# إذا defined(MSWIN)
// Mapping between cterm indices < 16 and their counterpart in the ANSI palette.
ثابت char_u cterm_ansi_idx[] = {
    0, 4, 2, 6, 1, 5, 3, 7, 8, 12, 10, 14, 9, 13, 11, 15
};
# endif

# define ANSI_شاخص_NONE 0

    فراغ
ansi_color2rgb(صحيح nr, char_u *r, char_u *g, char_u *b, char_u *ansi_idx)
{
    إذا (nr < 16)
    {
	*r = ansi_جدول[nr][0];
	*g = ansi_جدول[nr][1];
	*b = ansi_جدول[nr][2];
	*ansi_idx = nr + 1;
    }
    وإلا
    {
	*r = 0;
	*g = 0;
	*b = 0;
	*ansi_idx = ANSI_شاخص_NONE;
    }
}

    فراغ
cterm_color2rgb(صحيح nr, char_u *r, char_u *g, char_u *b, char_u *ansi_idx)
{
    صحيح idx;

    إذا (nr < 16)
    {
# إذا defined(MSWIN)
	idx = cterm_ansi_idx[nr];
# وإلا
	idx = nr;
# endif
	*r = ansi_جدول[idx][0];
	*g = ansi_جدول[idx][1];
	*b = ansi_جدول[idx][2];
	*ansi_idx = idx + 1;
    }
    وإلا إذا (nr < 232)
    {
	// 216 color cube
	idx = nr - 16;
	*r = cube_مقدار[idx / 36 % 6];
	*g = cube_مقدار[idx / 6  % 6];
	*b = cube_مقدار[idx      % 6];
	*ansi_idx = ANSI_شاخص_NONE;
    }
    وإلا إذا (nr < 256)
    {
	// 24 grey scale ramp
	idx = nr - 232;
	*r = grey_ramp[idx];
	*g = grey_ramp[idx];
	*b = grey_ramp[idx];
	*ansi_idx = ANSI_شاخص_NONE;
    }
    وإلا
    {
	*r = 0;
	*g = 0;
	*b = 0;
	*ansi_idx = ANSI_شاخص_NONE;
    }
}
#پایان_اگر

/*
 * Replace K_BS by <BS> and K_DEL by <DEL>.
 * Include any modifiers into the key and drop them.
 * Returns "len" adjusted for replaced codes.
 */
    صحيح
جمله_replace_keycodes(char_u *ta_buf, صحيح ta_len, صحيح len_arg)
{
    صحيح		len = len_arg;
    صحيح		i;
    صحيح		c;

    لكل (i = ta_len; i < ta_len + len; ++i)
    {
	إذا (ta_buf[i] == CSI && len - i > 3 && ta_buf[i + 1] == KS_MODIFIER)
	{
	    صحيح modifiers = ta_buf[i + 2];
	    صحيح کلید = ta_buf[i + 3];

	    // Try to use the modifier to modify the key.  In any case drop the
	    // modifier.
	    mch_memmove(ta_buf + i + 1, ta_buf + i + 4, (size_t)(len - i - 3));
	    len -= 3;
	    إذا (کلید < 0x80)
		کلید = ادغام_modify_Other_کلیدها(کلید, &modifiers);
	    ta_buf[i] = کلید;
	}
	وإلا إذا (ta_buf[i] == CSI && len - i > 2)
	{
	    c = TERMCAP2_کلید(ta_buf[i + 1], ta_buf[i + 2]);
	    إذا (c == K_DEL || c == K_KDEL || c == K_BS)
	    {
		mch_memmove(ta_buf + i + 1, ta_buf + i + 3,
							(size_t)(len - i - 2));
		إذا (c == K_DEL || c == K_KDEL)
		    ta_buf[i] = DEL;
		وإلا
		    ta_buf[i] = Ctrl_H;
		len -= 2;
	    }
	}
	وإلا إذا (ta_buf[i] == '\r')
	    ta_buf[i] = '\n';
	إذا (has_mbyte)
	    i += (*mb_ptr2len_len)(ta_buf + i, ta_len + len - i) - 1;
    }
    أرجع len;
}

/*
 * Should be called when cleaning up terminal state.
 */
    فراغ
جمله_disable_dec(فراغ)
{
    جمله_مجموعه_sync_output(جمله_SYNC_OUTPUT_OFF);
#اگر_تعریف_شده UNIX
    جمله_مجموعه_win_resize(false);
#پایان_اگر
    // Make sure to always flush the output buffer, because this may be called
    // before starting the GUI
    بیرون_flush();
}

#اگر_تعریف_شده UNIX
/*
 * Enable or disable receiving in-band window resize events from the terminal.
 * If "state" is true, then if the terminal supports DEC mode 2048 and
 * 'termresize' is "" or "inband", then enable it and disable the SIGWINCH
 * signal handling. Otherwise disable the mode if it is enabled and reinstall
 * the SIGWINCH handler.
 */
    فراغ
جمله_مجموعه_win_resize(bool state)
{
# ifdef FEAT_GUI
    bool    درون_gui = gui.درون_use;

    إذا (state && درون_gui)
	أرجع;
# endif

    إذا (!state || win_resize_setting == 0 || win_resize_setting == 4)
    {
	// Make sure it update internal window size if DEC mode 2048 is
	// unavailable now.
	إذا (win_resize_enabled)
	{
	    مجموعه_shellsize(0, 0, false);
	    مجموعه_sigwinch_handler();
	    بیرون_str((char_u *)"\033[?2048l");
	}
	win_resize_enabled = false;
    }
    وإلا إذا ((*p_trz == NUL || STRCMP(p_trz, "inband") == 0)
	    && !win_resize_enabled)
    {
	إذا (win_resize_setting == 2)
	    بیرون_str((char_u *)"\033[?2048h");
# ifdef SIGWINCH
	mch_signal(SIGWINCH, SIG_DFL);
# endif
	win_resize_enabled = true;
    }
}
#پایان_اگر

    صحيح
sync_output_فعال(فراغ)
{
#اگر_تعریف_شده FEAT_GUI
    إذا (gui.درون_use)
	أرجع TRUE;
#پایان_اگر
    أرجع p_tsy && (sync_output_setting == 1 || sync_output_setting == 2)
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
    فراغ
جمله_مجموعه_sync_output(صحيح flags)
{
    bool    allowed;
#اگر_تعریف_شده FEAT_GUI
    bool    درون_gui = gui.درون_use;
#وإلا
    bool    درون_gui = false;
#پایان_اگر

    allowed = p_tsy && (sync_output_setting == 1 || sync_output_setting == 2);

    إذا (flags & جمله_SYNC_OUTPUT_FLUSH)
    {
	// Tell terminal to display screen contents, then resume batching.
	إذا (allowed && !درون_gui && sync_output_state > 0 && *T_ESU != NUL &&
		*T_BSU != NUL)
	{
	    بیرون_str_nf(T_ESU);
	    بیرون_str_nf(T_BSU);
	    بیرون_flush();
	}
	أرجع;
    }

    // Forcibly turn off synchronized output (e.g. 'notermsync')
    إذا (flags & جمله_SYNC_OUTPUT_OFF)
    {
	إذا (sync_output_state > 0 && *T_ESU != NUL)
	{
	    بیرون_str_nf(T_ESU);
	    بیرون_flush();
	    sync_output_state = 0;
	}
	أرجع;
    }

    إذا (!allowed || درون_gui || *T_BSU == NUL || *T_ESU == NUL)
	أرجع;

    // Only enable if we aren't already, and only disable if we have reached
    // zero.
    إذا (flags & جمله_SYNC_OUTPUT_ENABLE)
    {
	إذا (sync_output_state++ > 0)
	    أرجع;
	بیرون_str_nf(T_BSU);
    }
    وإلا إذا (flags & جمله_SYNC_OUTPUT_DISABLE)
    {
	إذا (sync_output_state == 0 || --sync_output_state > 0)
	    أرجع;
	// Flush the output buffer before ending the sync batch so that
	// all drawing output is sent to the terminal within the
	// BSU..ESU window.  Without this, the drawing data remaining in
	// out_buf would be sent after ESU, outside the sync batch.
	بیرون_str_nf(T_ESU);
	بیرون_flush();
    }
    وإلا
    {
	siemsg("Unknown sync output value %d", flags);
	أرجع;
    }
}
