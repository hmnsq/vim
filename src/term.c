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

#অন্তর্ভাব "vim.h"

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
# যদি defined(VMS) || defined(AMIGA)
#  define TPUTSFUNCAST (শূন্য (*)(unsigned পূর্ণাঙ্ক))
# অন্যথা
#  ifdef HAVE_OUTFUNTYPE
#   define TPUTSFUNCAST (outfuntype)
#  অন্যথা
#   define TPUTSFUNCAST (পূর্ণাঙ্ক (*)(পূর্ণাঙ্ক))
#  endif
# endif
#endif

#undef tgetstr

// start of keys that are not directly used by Vim but can be mapped
#define BT_EXTRA_চাবিগুলি	0x101

স্থির শূন্য parse_builtin_tcap(char_u *s);
স্থির শূন্য gather_termleader(শূন্য);
#ifdef FEAT_TERMRESPONSE
স্থির শূন্য req_codes_from_term(শূন্য);
স্থির শূন্য req_more_codes_from_term(শূন্য);
স্থির শূন্য got_code_from_term(char_u *code, পূর্ণাঙ্ক len);
স্থির শূন্য check_for_codes_from_term(শূন্য);
#endif
স্থির শূন্য del_termcode_idx(পূর্ণাঙ্ক idx);
স্থির পূর্ণাঙ্ক find_term_bykeys(char_u *src, পূর্ণাঙ্ক *len);
স্থির পূর্ণাঙ্ক term_is_builtin(char_u *name);
স্থির পূর্ণাঙ্ক term_7to8bit(char_u *p);
স্থির শূন্য accept_modifiers_for_function_চাবিগুলি(শূন্য);

#যদি 0  // Change to 1 to enable ch_log() calls for termresponse debugging.
# define DEBUG_TERMRESPONSE
# define LOG_TR1(str) \
		ch_log(রিক্ত, "TermResp: %s " str, \
			must_redraw == UPD_NOT_VALID ? "NV" \
			: must_redraw == UPD_CLEAR ? "CL" : "  ")
# define LOG_TRN(fmt,...) \
		ch_log(রিক্ত, "TermResp: %s " fmt, \
			must_redraw == UPD_NOT_VALID ? "NV" \
			: must_redraw == UPD_CLEAR ? "CL" : "  ", __VA_ARGS__)
#অন্যথা
# define LOG_TR1(str) কুরু { /**/ } যাবত্ (0)
# define LOG_TRN(fmt,...) কুরু { /**/ } যাবত্ (0)
#endif

#ifdef HAVE_TGETENT
স্থির বর্ণ *invoke_tgetent(char_u *, char_u *);

/*
 * Here is our own prototype for tgetstr(), any prototypes from the include
 * files have been disabled by the define at the start of this file.
 */
বর্ণ		*tgetstr(বর্ণ *, বর্ণ **);
#endif

প্রকারনির্দেশ গণনা {
    STATUS_GET,		// send request when switching to RAW mode
    STATUS_SENT,	// did send request, checking for response
    STATUS_GOT,		// received response
    STATUS_FAIL		// timed out
} request_progress_T;

প্রকারনির্দেশ সংরচনা {
    request_progress_T	    tr_progress;
    time_t		    tr_start;	// when request was sent, -1 for never
} termrequest_T;

// Holds state for current OSC response.
প্রকারনির্দেশ সংরচনা
{
    পূর্ণাঙ্ক		    processing;	// If we are in the middle of an OSC response
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
স্থির termrequest_T crv_status = TERMREQUEST_INIT;

// Request Cursor position report:
স্থির termrequest_T u7_status = TERMREQUEST_INIT;

// Request xterm compatibility check:
স্থির termrequest_T xcc_status = TERMREQUEST_INIT;

#ifdef FEAT_TERMRESPONSE
# ifdef FEAT_TERMINAL
// Request foreground color report:
স্থির termrequest_T rfg_status = TERMREQUEST_INIT;
স্থির পূর্ণাঙ্ক fg_r = 0;
স্থির পূর্ণাঙ্ক fg_g = 0;
স্থির পূর্ণাঙ্ক fg_b = 0;
# endif
// Background color values from the OSC 11 response, also used by the
// image backend to flatten RGBA alpha onto the actual terminal background.
স্থির পূর্ণাঙ্ক bg_r = 255;
স্থির পূর্ণাঙ্ক bg_g = 255;
স্থির পূর্ণাঙ্ক bg_b = 255;

// Request background color report:
স্থির termrequest_T rbg_status = TERMREQUEST_INIT;

// Request cursor blinking mode report:
স্থির termrequest_T rbm_status = TERMREQUEST_INIT;

// Request cursor style report:
স্থির termrequest_T rcs_status = TERMREQUEST_INIT;

// Request window's position report:
স্থির termrequest_T winpos_status = TERMREQUEST_INIT;

// Request DECRQM (DEC mode) report:
স্থির termrequest_T decrqm_status = TERMREQUEST_INIT;

স্থির termrequest_T *all_termrequests[] = {
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
    রিক্ত
};

// The t_8u code may default to a value but get reset when the term response is
// received.  To avoid redrawing too often, only redraw when t_8u is not reset
// and it was supposed to be written.  Unless t_8u was set explicitly.
// FALSE -> don't output t_8u yet
// MAYBE -> tried outputting t_8u while FALSE
// OK    -> can write t_8u
পূর্ণাঙ্ক write_t_8u_state = FALSE;
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
বাহ্য short ospeed;
#  অন্যথা
short ospeed;
#  endif
# endif
# ifndef HAVE_UP_BC_PC
#  ifdef UP_BC_PC_EXTERN
বাহ্য বর্ণ *UP, *BC, PC;
#  অন্যথা
বর্ণ *UP, *BC, PC;
#  endif
# endif

# define TGETSTR(s, p)	vim_tgetstr((s), (p))
# define TGETENT(b, t)	tgetent((বর্ণ *)(b), (বর্ণ *)(t))
স্থির char_u *vim_tgetstr(বর্ণ *s, char_u **pp);
#endif // HAVE_TGETENT

স্থির পূর্ণাঙ্ক  detected_8bit = FALSE;	// detected 8-bit terminal

#যদি (defined(UNIX) || defined(VMS))
স্থির পূর্ণাঙ্ক focus_state = MAYBE; // TRUE if the Vim window has focus
#endif

#ifdef FEAT_TERMRESPONSE
// When the cursor shape was detected these values are used:
// 1: block, 2: underline, 3: vertical bar
স্থির পূর্ণাঙ্ক initial_cursor_shape = 0;

// The blink flag from the style response may be inverted from the actual
// blinking state, xterm XORs the flags.
স্থির পূর্ণাঙ্ক initial_cursor_shape_blink = FALSE;

// The blink flag from the blinking-cursor mode response
স্থির পূর্ণাঙ্ক initial_cursor_blink = FALSE;
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
স্থির পূর্ণাঙ্ক sync_output_setting = 0;

// > 0: Currently batching output
// == 0: No synchronized output
স্থির পূর্ণাঙ্ক sync_output_state = 0;

#ifdef UNIX
// DEC mode 2048 (in-band window resize events)
// https://gist.github.com/rockorager/e695fb2924d36b2bcf1fff4a3704bd83
স্থির পূর্ণাঙ্ক win_resize_setting = 0;
স্থির bool win_resize_enabled = false;
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
প্রকারনির্দেশ সংরচনা
{
    পূর্ণাঙ্ক		bt_entry;   // either a KS_xxx code (>= 0), or a K_xxx code.
    বর্ণ	*bt_string; //মান
} tcap_entry_T;

/*
 * Standard ANSI terminal, default for Unix.
 */
স্থির tcap_entry_T builtin_ansi[] = {
    {(পূর্ণাঙ্ক)KS_CE,	"\033[K"},
    {(পূর্ণাঙ্ক)KS_AL,	"\033[L"},
#ifdef TERMINFO
    {(পূর্ণাঙ্ক)KS_CAL,	"\033[%p1%dL"},
#অন্যথা
    {(পূর্ণাঙ্ক)KS_CAL,	"\033[%dL"},
#endif
    {(পূর্ণাঙ্ক)KS_DL,	"\033[M"},
#ifdef TERMINFO
    {(পূর্ণাঙ্ক)KS_CDL,	"\033[%p1%dM"},
#অন্যথা
    {(পূর্ণাঙ্ক)KS_CDL,	"\033[%dM"},
#endif
    {(পূর্ণাঙ্ক)KS_CL,	"\033[H\033[2J"},
    {(পূর্ণাঙ্ক)KS_ME,	"\033[0m"},
    {(পূর্ণাঙ্ক)KS_MR,	"\033[7m"},
    {(পূর্ণাঙ্ক)KS_MS,	"y"},
    {(পূর্ণাঙ্ক)KS_UT,	"y"},		// guessed
    {(পূর্ণাঙ্ক)KS_LE,	"\b"},
#ifdef TERMINFO
    {(পূর্ণাঙ্ক)KS_CM,	"\033[%i%p1%d;%p2%dH"},
#অন্যথা
    {(পূর্ণাঙ্ক)KS_CM,	"\033[%i%d;%dH"},
#endif
#ifdef TERMINFO
    {(পূর্ণাঙ্ক)KS_CRI,	"\033[%p1%dC"},
#অন্যথা
    {(পূর্ণাঙ্ক)KS_CRI,	"\033[%dC"},
#endif

    {(পূর্ণাঙ্ক)KS_NAME,	রিক্ত}  // end marker
};

/*
 * VT320 is working as an ANSI terminal compatible DEC terminal.
 * (it covers VT1x0, VT2x0 and VT3x0 up to VT320 on VMS as well)
 * TODO:- rewrite ESC[ codes to CSI
 *      - keyboard languages (CSI ? 26 n)
 */
স্থির tcap_entry_T builtin_vt320[] = {
    {(পূর্ণাঙ্ক)KS_CE,	"\033[K"},
    {(পূর্ণাঙ্ক)KS_AL,	"\033[L"},
#ifdef TERMINFO
    {(পূর্ণাঙ্ক)KS_CAL,	"\033[%p1%dL"},
#অন্যথা
    {(পূর্ণাঙ্ক)KS_CAL,	"\033[%dL"},
#endif
    {(পূর্ণাঙ্ক)KS_DL,	"\033[M"},
#ifdef TERMINFO
    {(পূর্ণাঙ্ক)KS_CDL,	"\033[%p1%dM"},
#অন্যথা
    {(পূর্ণাঙ্ক)KS_CDL,	"\033[%dM"},
#endif
    {(পূর্ণাঙ্ক)KS_CL,	"\033[H\033[2J"},
    {(পূর্ণাঙ্ক)KS_CD,	"\033[J"},
    {(পূর্ণাঙ্ক)KS_CCO,	"8"},			// allow 8 colors
    {(পূর্ণাঙ্ক)KS_ME,	"\033[0m"},
    {(পূর্ণাঙ্ক)KS_MR,	"\033[7m"},
    {(পূর্ণাঙ্ক)KS_MD,	"\033[1m"},  // bold mode
    {(পূর্ণাঙ্ক)KS_SE,	"\033[22m"},// normal mode
    {(পূর্ণাঙ্ক)KS_UE,	"\033[24m"},// exit underscore mode
    {(পূর্ণাঙ্ক)KS_US,	"\033[4m"},  // underscore mode
    {(পূর্ণাঙ্ক)KS_CZH,	"\033[34;43m"},  // italic mode: blue text on yellow
    {(পূর্ণাঙ্ক)KS_CZR,	"\033[0m"},	    // italic mode end
    {(পূর্ণাঙ্ক)KS_CAB,	"\033[4%dm"},	    // set background color (ANSI)
    {(পূর্ণাঙ্ক)KS_CAF,	"\033[3%dm"},	    // set foreground color (ANSI)
    {(পূর্ণাঙ্ক)KS_CSB,	"\033[102;%dm"},	// set screen background color
    {(পূর্ণাঙ্ক)KS_CSF,	"\033[101;%dm"},	// set screen foreground color
    {(পূর্ণাঙ্ক)KS_MS,	"y"},
    {(পূর্ণাঙ্ক)KS_UT,	"y"},
    {(পূর্ণাঙ্ক)KS_XN,	"y"},
    {(পূর্ণাঙ্ক)KS_LE,	"\b"},
#ifdef TERMINFO
    {(পূর্ণাঙ্ক)KS_CM,	"\033[%i%p1%d;%p2%dH"},
#অন্যথা
    {(পূর্ণাঙ্ক)KS_CM,	"\033[%i%d;%dH"},
#endif
#ifdef TERMINFO
    {(পূর্ণাঙ্ক)KS_CRI,	"\033[%p1%dC"},
#অন্যথা
    {(পূর্ণাঙ্ক)KS_CRI,	"\033[%dC"},
#endif
    {K_UP,		"\033[A"},
    {K_DOWN,		"\033[B"},
    {K_ডান,		"\033[C"},
    {K_বাম,		"\033[D"},
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

    {(পূর্ণাঙ্ক)KS_NAME,	রিক্ত}  // end marker
};

/*
 * Ordinary vt52
 */
স্থির tcap_entry_T builtin_vt52[] = {
    {(পূর্ণাঙ্ক)KS_CE,	"\033K"},
    {(পূর্ণাঙ্ক)KS_CD,	"\033J"},
#ifdef TERMINFO
    {(পূর্ণাঙ্ক)KS_CM,	"\033Y%p1%' '%+%c%p2%' '%+%c"},
#অন্যথা
    {(পূর্ণাঙ্ক)KS_CM,	"\033Y%+ %+ "},
#endif
    {(পূর্ণাঙ্ক)KS_LE,	"\b"},
    {(পূর্ণাঙ্ক)KS_SR,	"\033I"},
    {(পূর্ণাঙ্ক)KS_AL,	"\033L"},
    {(পূর্ণাঙ্ক)KS_DL,	"\033M"},
    {K_UP,		"\033A"},
    {K_DOWN,		"\033B"},
    {K_বাম,		"\033D"},
    {K_ডান,		"\033C"},
    {K_F1,		"\033P"},
    {K_F2,		"\033Q"},
    {K_F3,		"\033R"},
    {(পূর্ণাঙ্ক)KS_CL,	"\033H\033J"},
    {(পূর্ণাঙ্ক)KS_MS,	"y"},

    {(পূর্ণাঙ্ক)KS_NAME,	রিক্ত}  // end marker
};

/*
 * Builtin xterm with Vim-specific entries.
 */
স্থির tcap_entry_T builtin_xterm[] = {
    {(পূর্ণাঙ্ক)KS_CE,	"\033[K"},
    {(পূর্ণাঙ্ক)KS_AL,	"\033[L"},
#ifdef TERMINFO
    {(পূর্ণাঙ্ক)KS_CAL,	"\033[%p1%dL"},
#অন্যথা
    {(পূর্ণাঙ্ক)KS_CAL,	"\033[%dL"},
#endif
    {(পূর্ণাঙ্ক)KS_DL,	"\033[M"},
#ifdef TERMINFO
    {(পূর্ণাঙ্ক)KS_CDL,	"\033[%p1%dM"},
#অন্যথা
    {(পূর্ণাঙ্ক)KS_CDL,	"\033[%dM"},
#endif
#ifdef TERMINFO
    {(পূর্ণাঙ্ক)KS_CS,	"\033[%i%p1%d;%p2%dr"},
#অন্যথা
    {(পূর্ণাঙ্ক)KS_CS,	"\033[%i%d;%dr"},
#endif
    {(পূর্ণাঙ্ক)KS_CL,	"\033[H\033[2J"},
    {(পূর্ণাঙ্ক)KS_CD,	"\033[J"},
    {(পূর্ণাঙ্ক)KS_ME,	"\033[m"},
    {(পূর্ণাঙ্ক)KS_MR,	"\033[7m"},
    {(পূর্ণাঙ্ক)KS_MD,	"\033[1m"},
    {(পূর্ণাঙ্ক)KS_UE,	"\033[m"},
    {(পূর্ণাঙ্ক)KS_US,	"\033[4m"},
    {(পূর্ণাঙ্ক)KS_STE,	"\033[29m"},
    {(পূর্ণাঙ্ক)KS_STS,	"\033[9m"},
    {(পূর্ণাঙ্ক)KS_MS,	"y"},
    {(পূর্ণাঙ্ক)KS_UT,	"y"},
    {(পূর্ণাঙ্ক)KS_LE,	"\b"},
    {(পূর্ণাঙ্ক)KS_VI,	"\033[?25l"},
    {(পূর্ণাঙ্ক)KS_VE,	"\033[?25h"},
    {(পূর্ণাঙ্ক)KS_VS,	"\033[?12h"},
    {(পূর্ণাঙ্ক)KS_CVS,	"\033[?12l"},
#ifdef TERMINFO
    {(পূর্ণাঙ্ক)KS_CSH,	"\033[%p1%d q"},
#অন্যথা
    {(পূর্ণাঙ্ক)KS_CSH,	"\033[%d q"},
#endif
    {(পূর্ণাঙ্ক)KS_CRC,	"\033[?12$p"},
    {(পূর্ণাঙ্ক)KS_CRS,	"\033P$q q\033\\"},
#ifdef TERMINFO
    {(পূর্ণাঙ্ক)KS_CM,	"\033[%i%p1%d;%p2%dH"},
#অন্যথা
    {(পূর্ণাঙ্ক)KS_CM,	"\033[%i%d;%dH"},
#endif
    {(পূর্ণাঙ্ক)KS_SR,	"\033M"},
#ifdef TERMINFO
    {(পূর্ণাঙ্ক)KS_CRI,	"\033[%p1%dC"},
#অন্যথা
    {(পূর্ণাঙ্ক)KS_CRI,	"\033[%dC"},
#endif
    {(পূর্ণাঙ্ক)KS_KS,	"\033[?1h\033="},
    {(পূর্ণাঙ্ক)KS_KE,	"\033[?1l\033>"},
#ifdef FEAT_XTERM_SAVE
    {(পূর্ণাঙ্ক)KS_TI,	"\0337\033[?47h"},
    {(পূর্ণাঙ্ক)KS_TE,	"\033[?47l\0338"},
#endif
    // These are now under control of the 'keyprotocol' option, see
    // "builtin_mok2".
    // {(int)KS_CTI,	"\033[>4;2m"},
    // {(int)KS_CRK,	"\033[?4m"},
    // {(int)KS_CTE,	"\033[>4;m"},
    {(পূর্ণাঙ্ক)KS_CIS,	"\033]1;"},
    {(পূর্ণাঙ্ক)KS_CIE,	"\007"},
    {(পূর্ণাঙ্ক)KS_TS,	"\033]2;"},
    {(পূর্ণাঙ্ক)KS_FS,	"\007"},
    {(পূর্ণাঙ্ক)KS_CSC,	"\033]12;"},
    {(পূর্ণাঙ্ক)KS_CEC,	"\007"},
#ifdef TERMINFO
    {(পূর্ণাঙ্ক)KS_CWS,	"\033[8;%p1%d;%p2%dt"},
    {(পূর্ণাঙ্ক)KS_CWP,	"\033[3;%p1%d;%p2%dt"},
    {(পূর্ণাঙ্ক)KS_CGP,	"\033[13t"},
#অন্যথা
    {(পূর্ণাঙ্ক)KS_CWS,	"\033[8;%d;%dt"},
    {(পূর্ণাঙ্ক)KS_CWP,	"\033[3;%d;%dt"},
    {(পূর্ণাঙ্ক)KS_CGP,	"\033[13t"},
#endif
    {(পূর্ণাঙ্ক)KS_CRV,	"\033[>c"},
    {(পূর্ণাঙ্ক)KS_CXM,	"\033[?1006;1000%?%p1%{1}%=%th%el%;"},
    {(পূর্ণাঙ্ক)KS_RFG,	"\033]10;?\007"},
    {(পূর্ণাঙ্ক)KS_RBG,	"\033]11;?\007"},
    {(পূর্ণাঙ্ক)KS_U7,	"\033[6n"},
    {(পূর্ণাঙ্ক)KS_CAU,	"\033[58;5;%dm"},
    {(পূর্ণাঙ্ক)KS_CBE,	"\033[?2004h"},
    {(পূর্ণাঙ্ক)KS_CBD,	"\033[?2004l"},
    {(পূর্ণাঙ্ক)KS_CST,	"\033[22;2t"},
    {(পূর্ণাঙ্ক)KS_CRT,	"\033[23;2t"},
    {(পূর্ণাঙ্ক)KS_SSI,	"\033[22;1t"},
    {(পূর্ণাঙ্ক)KS_SRI,	"\033[23;1t"},
#যদি (defined(UNIX) || defined(VMS))
    {(পূর্ণাঙ্ক)KS_FD,	"\033[?1004l"},
    {(পূর্ণাঙ্ক)KS_FE,	"\033[?1004h"},
#endif

    {K_UP,		"\033O*A"},
    {K_DOWN,		"\033O*B"},
    {K_ডান,		"\033O*C"},
    {K_বাম,		"\033O*D"},
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

    {BT_EXTRA_চাবিগুলি,   ""},
    {TERMCAP2_চাবি('k', '0'), "\033[10;*~"}, // F0
    {TERMCAP2_চাবি('F', '3'), "\033[25;*~"}, // F13
    // F14 and F15 are missing, because they send the same codes as the undo
    // and help key, although they don't work on all keyboards.
    {TERMCAP2_চাবি('F', '6'), "\033[29;*~"}, // F16
    {TERMCAP2_চাবি('F', '7'), "\033[31;*~"}, // F17
    {TERMCAP2_চাবি('F', '8'), "\033[32;*~"}, // F18
    {TERMCAP2_চাবি('F', '9'), "\033[33;*~"}, // F19
    {TERMCAP2_চাবি('F', 'A'), "\033[34;*~"}, // F20

    {TERMCAP2_চাবি('F', 'B'), "\033[42;*~"}, // F21
    {TERMCAP2_চাবি('F', 'C'), "\033[43;*~"}, // F22
    {TERMCAP2_চাবি('F', 'D'), "\033[44;*~"}, // F23
    {TERMCAP2_চাবি('F', 'E'), "\033[45;*~"}, // F24
    {TERMCAP2_চাবি('F', 'F'), "\033[46;*~"}, // F25
    {TERMCAP2_চাবি('F', 'G'), "\033[47;*~"}, // F26
    {TERMCAP2_চাবি('F', 'H'), "\033[48;*~"}, // F27
    {TERMCAP2_চাবি('F', 'I'), "\033[49;*~"}, // F28
    {TERMCAP2_চাবি('F', 'J'), "\033[50;*~"}, // F29
    {TERMCAP2_চাবি('F', 'K'), "\033[51;*~"}, // F30

    {TERMCAP2_চাবি('F', 'L'), "\033[52;*~"}, // F31
    {TERMCAP2_চাবি('F', 'M'), "\033[53;*~"}, // F32
    {TERMCAP2_চাবি('F', 'N'), "\033[54;*~"}, // F33
    {TERMCAP2_চাবি('F', 'O'), "\033[55;*~"}, // F34
    {TERMCAP2_চাবি('F', 'P'), "\033[56;*~"}, // F35
    {TERMCAP2_চাবি('F', 'Q'), "\033[57;*~"}, // F36
    {TERMCAP2_চাবি('F', 'R'), "\033[58;*~"}, // F37

    {(পূর্ণাঙ্ক)KS_NAME,	রিক্ত}  // end marker
};

/*
 * Additions for using modifyOtherKeys level 2.  Same as what is used for
 * xterm.
 */
স্থির tcap_entry_T builtin_mok2[] = {
    // t_TI enables modifyOtherKeys level 2
    {(পূর্ণাঙ্ক)KS_CTI,	"\033[>4;2m"},

    // XTQMODKEYS was added in xterm version 377: "CSI ? 4 m" which should
    // return "{lead} > 4 ; Pv m".  Before version 377 we expect it to have no
    // effect.
    {(পূর্ণাঙ্ক)KS_CRK,	"\033[?4m"},

    // t_TE disables modifyOtherKeys
    {(পূর্ণাঙ্ক)KS_CTE,	"\033[>4;m"},

    {(পূর্ণাঙ্ক)KS_NAME,	রিক্ত}  // end marker
};

/*
 * Additions for using the Kitty keyboard protocol.
 */
স্থির tcap_entry_T builtin_kitty[] = {
    // t_TI enables the kitty keyboard protocol.
    {(পূর্ণাঙ্ক)KS_CTI,	"\033[=1;1u"},

    // t_RK requests the kitty keyboard protocol state
    {(পূর্ণাঙ্ক)KS_CRK,	"\033[?u"},

    // t_TE also disables modifyOtherKeys, because t_TI from xterm may already
    // have been used.
    {(পূর্ণাঙ্ক)KS_CTE,	"\033[>4;m\033[=0;1u"},

    // t_RF request terminal foreground color
    {(পূর্ণাঙ্ক)KS_RFG,	"\033]10;?\033\\"},

    // t_RB request terminal background color
    {(পূর্ণাঙ্ক)KS_RBG,	"\033]11;?\033\\"},

    {(পূর্ণাঙ্ক)KS_NAME,	রিক্ত}  // end marker
};

#ifdef HAVE_TGETENT
/*
 * Additions for enabling/disabling synchronized output mode for terminal.
 */
স্থির tcap_entry_T builtin_sync_output[] = {
    {(পূর্ণাঙ্ক)KS_BSU,	"\033[?2026h"},
    {(পূর্ণাঙ্ক)KS_ESU,	"\033[?2026l"},
    {(পূর্ণাঙ্ক)KS_NAME,	রিক্ত}  // end marker
};
#endif

#ifdef FEAT_TERMRESPONSE
/*
 * List of DECRQM modes that Vim supports
 */
স্থির ধ্রুব পূর্ণাঙ্ক dec_modes[] = {
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
স্থির tcap_entry_T builtin_rgb[] = {
    // These are printf strings, not terminal codes.
    {(পূর্ণাঙ্ক)KS_8F,	"\033[38;2;%lu;%lu;%lum"},
    {(পূর্ণাঙ্ক)KS_8B,	"\033[48;2;%lu;%lu;%lum"},
    {(পূর্ণাঙ্ক)KS_8U,	"\033[58;2;%lu;%lu;%lum"},

    {(পূর্ণাঙ্ক)KS_NAME,	রিক্ত}  // end marker
};
#endif

#ifdef HAVE_TGETENT
স্থির tcap_entry_T special_term[] = {
    // These are printf strings, not terminal codes.
    {(পূর্ণাঙ্ক)KS_CF,	"\033[%dm"},
    {(পূর্ণাঙ্ক)KS_NAME,	রিক্ত}  // end marker
};
#endif

/*
 * iris-ansi for Silicon Graphics machines.
 */
স্থির tcap_entry_T builtin_iris_ansi[] = {
    {(পূর্ণাঙ্ক)KS_CE,	"\033[K"},
    {(পূর্ণাঙ্ক)KS_CD,	"\033[J"},
    {(পূর্ণাঙ্ক)KS_AL,	"\033[L"},
#ifdef TERMINFO
    {(পূর্ণাঙ্ক)KS_CAL,	"\033[%p1%dL"},
#অন্যথা
    {(পূর্ণাঙ্ক)KS_CAL,	"\033[%dL"},
#endif
    {(পূর্ণাঙ্ক)KS_DL,	"\033[M"},
#ifdef TERMINFO
    {(পূর্ণাঙ্ক)KS_CDL,	"\033[%p1%dM"},
#অন্যথা
    {(পূর্ণাঙ্ক)KS_CDL,	"\033[%dM"},
#endif
#যদি 0	// The scroll region is not working as Vim expects.
# ifdef TERMINFO
    {(পূর্ণাঙ্ক)KS_CS,	"\033[%i%p1%d;%p2%dr"},
# অন্যথা
    {(পূর্ণাঙ্ক)KS_CS,	"\033[%i%d;%dr"},
# endif
#endif
    {(পূর্ণাঙ্ক)KS_CL,	"\033[H\033[2J"},
    {(পূর্ণাঙ্ক)KS_VE,	"\033[9/y\033[12/y"},	// These aren't documented
    {(পূর্ণাঙ্ক)KS_VS,	"\033[10/y\033[=1h\033[=2l"}, // These aren't documented
    {(পূর্ণাঙ্ক)KS_TI,	"\033[=6h"},
    {(পূর্ণাঙ্ক)KS_TE,	"\033[=6l"},
    {(পূর্ণাঙ্ক)KS_SE,	"\033[21;27m"},
    {(পূর্ণাঙ্ক)KS_SO,	"\033[1;7m"},
    {(পূর্ণাঙ্ক)KS_ME,	"\033[m"},
    {(পূর্ণাঙ্ক)KS_MR,	"\033[7m"},
    {(পূর্ণাঙ্ক)KS_MD,	"\033[1m"},
    {(পূর্ণাঙ্ক)KS_CCO,	"8"},			// allow 8 colors
    {(পূর্ণাঙ্ক)KS_CZH,	"\033[3m"},		// italic mode on
    {(পূর্ণাঙ্ক)KS_CZR,	"\033[23m"},		// italic mode off
    {(পূর্ণাঙ্ক)KS_US,	"\033[4m"},		// underline on
    {(পূর্ণাঙ্ক)KS_UE,	"\033[24m"},		// underline off
#ifdef TERMINFO
    {(পূর্ণাঙ্ক)KS_CAB,	"\033[4%p1%dm"},    // set background color (ANSI)
    {(পূর্ণাঙ্ক)KS_CAF,	"\033[3%p1%dm"},    // set foreground color (ANSI)
    {(পূর্ণাঙ্ক)KS_CSB,	"\033[102;%p1%dm"}, // set screen background color
    {(পূর্ণাঙ্ক)KS_CSF,	"\033[101;%p1%dm"}, // set screen foreground color
#অন্যথা
    {(পূর্ণাঙ্ক)KS_CAB,	"\033[4%dm"},	    // set background color (ANSI)
    {(পূর্ণাঙ্ক)KS_CAF,	"\033[3%dm"},	    // set foreground color (ANSI)
    {(পূর্ণাঙ্ক)KS_CSB,	"\033[102;%dm"},    // set screen background color
    {(পূর্ণাঙ্ক)KS_CSF,	"\033[101;%dm"},    // set screen foreground color
#endif
    {(পূর্ণাঙ্ক)KS_MS,	"y"},		// guessed
    {(পূর্ণাঙ্ক)KS_UT,	"y"},		// guessed
    {(পূর্ণাঙ্ক)KS_LE,	"\b"},
#ifdef TERMINFO
    {(পূর্ণাঙ্ক)KS_CM,	"\033[%i%p1%d;%p2%dH"},
#অন্যথা
    {(পূর্ণাঙ্ক)KS_CM,	"\033[%i%d;%dH"},
#endif
    {(পূর্ণাঙ্ক)KS_SR,	"\033M"},
#ifdef TERMINFO
    {(পূর্ণাঙ্ক)KS_CRI,	"\033[%p1%dC"},
#অন্যথা
    {(পূর্ণাঙ্ক)KS_CRI,	"\033[%dC"},
#endif
    {(পূর্ণাঙ্ক)KS_CIS,	"\033P3.y"},
    {(পূর্ণাঙ্ক)KS_CIE,	"\234"},    // ST "String Terminator"
    {(পূর্ণাঙ্ক)KS_TS,	"\033P1.y"},
    {(পূর্ণাঙ্ক)KS_FS,	"\234"},    // ST "String Terminator"
#ifdef TERMINFO
    {(পূর্ণাঙ্ক)KS_CWS,	"\033[203;%p1%d;%p2%d/y"},
    {(পূর্ণাঙ্ক)KS_CWP,	"\033[205;%p1%d;%p2%d/y"},
#অন্যথা
    {(পূর্ণাঙ্ক)KS_CWS,	"\033[203;%d;%d/y"},
    {(পূর্ণাঙ্ক)KS_CWP,	"\033[205;%d;%d/y"},
#endif
    {K_UP,		"\033[A"},
    {K_DOWN,		"\033[B"},
    {K_বাম,		"\033[D"},
    {K_ডান,		"\033[C"},
    {K_S_UP,		"\033[161q"},
    {K_S_DOWN,		"\033[164q"},
    {K_S_বাম,		"\033[158q"},
    {K_S_ডান,		"\033[167q"},
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

    {(পূর্ণাঙ্ক)KS_NAME,	রিক্ত}  // end marker
};

/*
 * These codes are valid when nansi.sys or equivalent has been installed.
 * Function keys on a PC are preceded with a NUL. These are converted into
 * K_NUL '\316' in mch_inchar(), because we cannot handle NULs in key codes.
 * CTRL-arrow is used instead of SHIFT-arrow.
 */
স্থির tcap_entry_T builtin_pcansi[] = {
    {(পূর্ণাঙ্ক)KS_DL,	"\033[M"},
    {(পূর্ণাঙ্ক)KS_AL,	"\033[L"},
    {(পূর্ণাঙ্ক)KS_CE,	"\033[K"},
    {(পূর্ণাঙ্ক)KS_CL,	"\033[2J"},
    {(পূর্ণাঙ্ক)KS_ME,	"\033[0m"},
    {(পূর্ণাঙ্ক)KS_MR,	"\033[5m"},	// reverse: black on lightgrey
    {(পূর্ণাঙ্ক)KS_MD,	"\033[1m"},	// bold: white text
    {(পূর্ণাঙ্ক)KS_SE,	"\033[0m"},	// standout end
    {(পূর্ণাঙ্ক)KS_SO,	"\033[31m"},	// standout: white on blue
    {(পূর্ণাঙ্ক)KS_CZH,	"\033[34;43m"},	// italic mode: blue text on yellow
    {(পূর্ণাঙ্ক)KS_CZR,	"\033[0m"},	// italic mode end
    {(পূর্ণাঙ্ক)KS_US,	"\033[36;41m"},	// underscore mode: cyan text on red
    {(পূর্ণাঙ্ক)KS_UE,	"\033[0m"},	// underscore mode end
    {(পূর্ণাঙ্ক)KS_CCO,	"8"},		// allow 8 colors
#ifdef TERMINFO
    {(পূর্ণাঙ্ক)KS_CAB,	"\033[4%p1%dm"},// set background color
    {(পূর্ণাঙ্ক)KS_CAF,	"\033[3%p1%dm"},// set foreground color
#অন্যথা
    {(পূর্ণাঙ্ক)KS_CAB,	"\033[4%dm"},	// set background color
    {(পূর্ণাঙ্ক)KS_CAF,	"\033[3%dm"},	// set foreground color
#endif
    {(পূর্ণাঙ্ক)KS_OP,	"\033[0m"},	// reset colors
    {(পূর্ণাঙ্ক)KS_MS,	"y"},
    {(পূর্ণাঙ্ক)KS_UT,	"y"},		// guessed
    {(পূর্ণাঙ্ক)KS_LE,	"\b"},
#ifdef TERMINFO
    {(পূর্ণাঙ্ক)KS_CM,	"\033[%i%p1%d;%p2%dH"},
#অন্যথা
    {(পূর্ণাঙ্ক)KS_CM,	"\033[%i%d;%dH"},
#endif
#ifdef TERMINFO
    {(পূর্ণাঙ্ক)KS_CRI,	"\033[%p1%dC"},
#অন্যথা
    {(পূর্ণাঙ্ক)KS_CRI,	"\033[%dC"},
#endif
    {K_UP,		"\316H"},
    {K_DOWN,		"\316P"},
    {K_বাম,		"\316K"},
    {K_ডান,		"\316M"},
    {K_S_বাম,		"\316s"},
    {K_S_ডান,		"\316t"},
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

    {(পূর্ণাঙ্ক)KS_NAME,	রিক্ত}  // end marker
};

/*
 * These codes are valid for the Win32 Console.  The entries that start with
 * ESC | are translated into console calls in os_win32.c.  The function keys
 * are also translated in os_win32.c.
 */
স্থির tcap_entry_T builtin_win32[] = {
    {(পূর্ণাঙ্ক)KS_CE,	"\033|K"},	// clear to end of line
    {(পূর্ণাঙ্ক)KS_AL,	"\033|L"},	// add new blank line
#ifdef TERMINFO
    {(পূর্ণাঙ্ক)KS_CAL,	"\033|%p1%dL"},	// add number of new blank lines
#অন্যথা
    {(পূর্ণাঙ্ক)KS_CAL,	"\033|%dL"},	// add number of new blank lines
#endif
    {(পূর্ণাঙ্ক)KS_DL,	"\033|M"},	// delete line
#ifdef TERMINFO
    {(পূর্ণাঙ্ক)KS_CDL,	"\033|%p1%dM"},	// delete number of lines
    {(পূর্ণাঙ্ক)KS_CSV,	"\033|%p1%d;%p2%dV"},
#অন্যথা
    {(পূর্ণাঙ্ক)KS_CDL,	"\033|%dM"},	// delete number of lines
    {(পূর্ণাঙ্ক)KS_CSV,	"\033|%d;%dV"},
#endif
    {(পূর্ণাঙ্ক)KS_CL,	"\033|J"},	// clear screen
    {(পূর্ণাঙ্ক)KS_CD,	"\033|j"},	// clear to end of display
    {(পূর্ণাঙ্ক)KS_VI,	"\033|v"},	// cursor invisible
    {(পূর্ণাঙ্ক)KS_VE,	"\033|V"},	// cursor visible

    {(পূর্ণাঙ্ক)KS_ME,	"\033|0m"},	// normal
    {(পূর্ণাঙ্ক)KS_MR,	"\033|112m"},	// reverse: black on lightgray
    {(পূর্ণাঙ্ক)KS_MD,	"\033|15m"},	// bold: white on black
#যদি 1
    {(পূর্ণাঙ্ক)KS_SO,	"\033|31m"},	// standout: white on blue
    {(পূর্ণাঙ্ক)KS_SE,	"\033|0m"},	// standout end
#অন্যথা
    {(পূর্ণাঙ্ক)KS_SO,	"\033|F"},	// standout: high intensity
    {(পূর্ণাঙ্ক)KS_SE,	"\033|f"},	// standout end
#endif
    {(পূর্ণাঙ্ক)KS_CZH,	"\033|225m"},	// italic: blue text on yellow
    {(পূর্ণাঙ্ক)KS_CZR,	"\033|0m"},	// italic end
    {(পূর্ণাঙ্ক)KS_US,	"\033|67m"},	// underscore: cyan text on red
    {(পূর্ণাঙ্ক)KS_UE,	"\033|0m"},	// underscore end
    {(পূর্ণাঙ্ক)KS_CCO,	"16"},		// allow 16 colors
#ifdef TERMINFO
    {(পূর্ণাঙ্ক)KS_CAB,	"\033|%p1%db"},	// set background color
    {(পূর্ণাঙ্ক)KS_CAF,	"\033|%p1%df"},	// set foreground color
#অন্যথা
    {(পূর্ণাঙ্ক)KS_CAB,	"\033|%db"},	// set background color
    {(পূর্ণাঙ্ক)KS_CAF,	"\033|%df"},	// set foreground color
#endif

    {(পূর্ণাঙ্ক)KS_MS,	"y"},		// save to move cur in reverse mode
    {(পূর্ণাঙ্ক)KS_UT,	"y"},
    {(পূর্ণাঙ্ক)KS_XN,	"y"},
    {(পূর্ণাঙ্ক)KS_LE,	"\b"},
#ifdef TERMINFO
    {(পূর্ণাঙ্ক)KS_CM,	"\033|%i%p1%d;%p2%dH"}, // cursor motion
#অন্যথা
    {(পূর্ণাঙ্ক)KS_CM,	"\033|%i%d;%dH"}, // cursor motion
#endif
    {(পূর্ণাঙ্ক)KS_VB,	"\033|B"},	// visual bell
    {(পূর্ণাঙ্ক)KS_TI,	"\033|S"},	// put terminal in termcap mode
    {(পূর্ণাঙ্ক)KS_TE,	"\033|E"},	// out of termcap mode
#ifdef TERMINFO
    {(পূর্ণাঙ্ক)KS_CS,	"\033|%i%p1%d;%p2%dr"}, // scroll region
#অন্যথা
    {(পূর্ণাঙ্ক)KS_CS,	"\033|%i%d;%dr"}, // scroll region
#endif

    {K_UP,		"\316H"},
    {K_DOWN,		"\316P"},
    {K_বাম,		"\316K"},
    {K_ডান,		"\316M"},
    {K_S_UP,		"\316\304"},
    {K_S_DOWN,		"\316\317"},
    {K_S_বাম,		"\316\311"},
    {K_C_বাম,		"\316s"},
    {K_S_ডান,		"\316\313"},
    {K_C_ডান,		"\316t"},
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

    {(পূর্ণাঙ্ক)KS_NAME,	রিক্ত}  // end marker
};

#যদি defined(FEAT_GUI)
/*
 * GUI uses made-up codes, only used inside Vim.
 */
স্থির tcap_entry_T builtin_gui[] = {
    {(পূর্ণাঙ্ক)KS_CE,	"\033|$"},
    {(পূর্ণাঙ্ক)KS_AL,	"\033|i"},
# ifdef TERMINFO
    {(পূর্ণাঙ্ক)KS_CAL,	"\033|%p1%dI"},
# অন্যথা
    {(পূর্ণাঙ্ক)KS_CAL,	"\033|%dI"},
# endif
    {(পূর্ণাঙ্ক)KS_DL,	"\033|d"},
# ifdef TERMINFO
    {(পূর্ণাঙ্ক)KS_CDL,	"\033|%p1%dD"},
    {(পূর্ণাঙ্ক)KS_CS,	"\033|%p1%d;%p2%dR"},
    {(পূর্ণাঙ্ক)KS_CSV,	"\033|%p1%d;%p2%dV"},
# অন্যথা
    {(পূর্ণাঙ্ক)KS_CDL,	"\033|%dD"},
    {(পূর্ণাঙ্ক)KS_CS,	"\033|%d;%dR"},
    {(পূর্ণাঙ্ক)KS_CSV,	"\033|%d;%dV"},
# endif
    {(পূর্ণাঙ্ক)KS_CL,	"\033|C"},
			// attributes switched on with 'h', off with * 'H'
    {(পূর্ণাঙ্ক)KS_ME,	"\033|31H"}, // HL_ALL
    {(পূর্ণাঙ্ক)KS_MR,	"\033|1h"},   // HL_INVERSE
    {(পূর্ণাঙ্ক)KS_MD,	"\033|2h"},   // HL_BOLD
    {(পূর্ণাঙ্ক)KS_SE,	"\033|16H"}, // HL_STANDOUT
    {(পূর্ণাঙ্ক)KS_SO,	"\033|16h"}, // HL_STANDOUT
    {(পূর্ণাঙ্ক)KS_UE,	"\033|8H"},   // HL_UNDERLINE
    {(পূর্ণাঙ্ক)KS_US,	"\033|8h"},   // HL_UNDERLINE
    {(পূর্ণাঙ্ক)KS_UCE,	"\033|8C"},   // HL_UNDERCURL
    {(পূর্ণাঙ্ক)KS_UCS,	"\033|8c"},   // HL_UNDERCURL
    {(পূর্ণাঙ্ক)KS_STE,	"\033|4C"},   // HL_STRIKETHROUGH
    {(পূর্ণাঙ্ক)KS_STS,	"\033|4c"},   // HL_STRIKETHROUGH
    {(পূর্ণাঙ্ক)KS_CZR,	"\033|4H"},   // HL_ITALIC
    {(পূর্ণাঙ্ক)KS_CZH,	"\033|4h"},   // HL_ITALIC
    {(পূর্ণাঙ্ক)KS_VB,	"\033|f"},
    {(পূর্ণাঙ্ক)KS_MS,	"y"},
    {(পূর্ণাঙ্ক)KS_UT,	"y"},
    {(পূর্ণাঙ্ক)KS_XN,	"y"},
    {(পূর্ণাঙ্ক)KS_LE,	"\b"},		// cursor-left = BS
    {(পূর্ণাঙ্ক)KS_ND,	"\014"},	// cursor-right = CTRL-L
# ifdef TERMINFO
    {(পূর্ণাঙ্ক)KS_CM,	"\033|%p1%d;%p2%dM"},
# অন্যথা
    {(পূর্ণাঙ্ক)KS_CM,	"\033|%d;%dM"},
# endif
	// there are no key sequences here, the GUI sequences are recognized
	// in check_termcode()

    {(পূর্ণাঙ্ক)KS_NAME,	রিক্ত}  // end marker
};
#endif

/*
 * Amiga console window, default for Amiga.
 */
স্থির tcap_entry_T builtin_amiga[] = {
    {(পূর্ণাঙ্ক)KS_CE,	"\033[K"},
    {(পূর্ণাঙ্ক)KS_CD,	"\033[J"},
    {(পূর্ণাঙ্ক)KS_AL,	"\033[L"},
#ifdef TERMINFO
    {(পূর্ণাঙ্ক)KS_CAL,	"\033[%p1%dL"},
#অন্যথা
    {(পূর্ণাঙ্ক)KS_CAL,	"\033[%dL"},
#endif
    {(পূর্ণাঙ্ক)KS_DL,	"\033[M"},
#ifdef TERMINFO
    {(পূর্ণাঙ্ক)KS_CDL,	"\033[%p1%dM"},
#অন্যথা
    {(পূর্ণাঙ্ক)KS_CDL,	"\033[%dM"},
#endif
    {(পূর্ণাঙ্ক)KS_CL,	"\014"},
    {(পূর্ণাঙ্ক)KS_VI,	"\033[0 p"},
    {(পূর্ণাঙ্ক)KS_VE,	"\033[1 p"},
    {(পূর্ণাঙ্ক)KS_ME,	"\033[0m"},
    {(পূর্ণাঙ্ক)KS_MR,	"\033[7m"},
    {(পূর্ণাঙ্ক)KS_MD,	"\033[1m"},
    {(পূর্ণাঙ্ক)KS_SE,	"\033[0m"},
    {(পূর্ণাঙ্ক)KS_SO,	"\033[33m"},
    {(পূর্ণাঙ্ক)KS_US,	"\033[4m"},
    {(পূর্ণাঙ্ক)KS_UE,	"\033[0m"},
    {(পূর্ণাঙ্ক)KS_CZH,	"\033[3m"},
    {(পূর্ণাঙ্ক)KS_CZR,	"\033[0m"},
#যদি defined(__amigaos4__) || defined(__MORPHOS__) || defined(__AROS__)
    {(পূর্ণাঙ্ক)KS_CCO,	"8"},		// allow 8 colors
# ifdef TERMINFO
    {(পূর্ণাঙ্ক)KS_CAB,	"\033[4%p1%dm"},// set background color
    {(পূর্ণাঙ্ক)KS_CAF,	"\033[3%p1%dm"},// set foreground color
# অন্যথা
    {(পূর্ণাঙ্ক)KS_CAB,	"\033[4%dm"},	// set background color
    {(পূর্ণাঙ্ক)KS_CAF,	"\033[3%dm"},	// set foreground color
# endif
    {(পূর্ণাঙ্ক)KS_OP,	"\033[m"},	// reset colors
#endif
    {(পূর্ণাঙ্ক)KS_MS,	"y"},
    {(পূর্ণাঙ্ক)KS_UT,	"y"},		// guessed
    {(পূর্ণাঙ্ক)KS_LE,	"\b"},
#ifdef TERMINFO
    {(পূর্ণাঙ্ক)KS_CM,	"\033[%i%p1%d;%p2%dH"},
#অন্যথা
    {(পূর্ণাঙ্ক)KS_CM,	"\033[%i%d;%dH"},
#endif
#যদি defined(__MORPHOS__)
    {(পূর্ণাঙ্ক)KS_SR,	"\033M"},
#endif
#ifdef TERMINFO
    {(পূর্ণাঙ্ক)KS_CRI,	"\033[%p1%dC"},
#অন্যথা
    {(পূর্ণাঙ্ক)KS_CRI,	"\033[%dC"},
#endif
    {K_UP,		"\233A"},
    {K_DOWN,		"\233B"},
    {K_বাম,		"\233D"},
    {K_ডান,		"\233C"},
    {K_S_UP,		"\233T"},
    {K_S_DOWN,		"\233S"},
    {K_S_বাম,		"\233 A"},
    {K_S_ডান,		"\233 @"},
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

    {BT_EXTRA_চাবিগুলি,	""},
    {TERMCAP2_চাবি('#', '2'), "\233\065\064~"},	// shifted home key
    {TERMCAP2_চাবি('#', '3'), "\233\065\060~"},	// shifted insert key
    {TERMCAP2_চাবি('*', '7'), "\233\065\065~"},	// shifted end key

    {(পূর্ণাঙ্ক)KS_NAME,	রিক্ত}  // end marker
};

/*
 * The most minimal terminal: only clear screen and cursor positioning.
 */
স্থির tcap_entry_T builtin_dumb[] = {
    {(পূর্ণাঙ্ক)KS_CL,	"\014"},
#ifdef TERMINFO
    {(পূর্ণাঙ্ক)KS_CM,	"\033[%i%p1%d;%p2%dH"},
#অন্যথা
    {(পূর্ণাঙ্ক)KS_CM,	"\033[%i%d;%dH"},
#endif

    {(পূর্ণাঙ্ক)KS_NAME,	রিক্ত}  // end marker
};

/*
 * Terminal used for debugging.
 */
স্থির tcap_entry_T builtin_debug[] = {
    {(পূর্ণাঙ্ক)KS_CE,	"[CE]"},
    {(পূর্ণাঙ্ক)KS_CD,	"[CD]"},
    {(পূর্ণাঙ্ক)KS_AL,	"[AL]"},
#ifdef TERMINFO
    {(পূর্ণাঙ্ক)KS_CAL,	"[CAL%p1%d]"},
#অন্যথা
    {(পূর্ণাঙ্ক)KS_CAL,	"[CAL%d]"},
#endif
    {(পূর্ণাঙ্ক)KS_DL,	"[DL]"},
#ifdef TERMINFO
    {(পূর্ণাঙ্ক)KS_CDL,	"[CDL%p1%d]"},
#অন্যথা
    {(পূর্ণাঙ্ক)KS_CDL,	"[CDL%d]"},
#endif
#ifdef TERMINFO
    {(পূর্ণাঙ্ক)KS_CS,	"[%p1%dCS%p2%d]"},
#অন্যথা
    {(পূর্ণাঙ্ক)KS_CS,	"[%dCS%d]"},
#endif
#ifdef TERMINFO
    {(পূর্ণাঙ্ক)KS_CSV,	"[%p1%dCSV%p2%d]"},
#অন্যথা
    {(পূর্ণাঙ্ক)KS_CSV,	"[%dCSV%d]"},
#endif
#ifdef TERMINFO
    {(পূর্ণাঙ্ক)KS_CAB,	"[CAB%p1%d]"},
    {(পূর্ণাঙ্ক)KS_CAF,	"[CAF%p1%d]"},
    {(পূর্ণাঙ্ক)KS_CSB,	"[CSB%p1%d]"},
    {(পূর্ণাঙ্ক)KS_CSF,	"[CSF%p1%d]"},
#অন্যথা
    {(পূর্ণাঙ্ক)KS_CAB,	"[CAB%d]"},
    {(পূর্ণাঙ্ক)KS_CAF,	"[CAF%d]"},
    {(পূর্ণাঙ্ক)KS_CSB,	"[CSB%d]"},
    {(পূর্ণাঙ্ক)KS_CSF,	"[CSF%d]"},
#endif
    {(পূর্ণাঙ্ক)KS_CAU,	"[CAU%d]"},
    {(পূর্ণাঙ্ক)KS_OP,	"[OP]"},
    {(পূর্ণাঙ্ক)KS_LE,	"[LE]"},
    {(পূর্ণাঙ্ক)KS_CL,	"[CL]"},
    {(পূর্ণাঙ্ক)KS_VI,	"[VI]"},
    {(পূর্ণাঙ্ক)KS_VE,	"[VE]"},
    {(পূর্ণাঙ্ক)KS_VS,	"[VS]"},
    {(পূর্ণাঙ্ক)KS_ME,	"[ME]"},
    {(পূর্ণাঙ্ক)KS_MR,	"[MR]"},
    {(পূর্ণাঙ্ক)KS_MB,	"[MB]"},
    {(পূর্ণাঙ্ক)KS_MD,	"[MD]"},
    {(পূর্ণাঙ্ক)KS_SE,	"[SE]"},
    {(পূর্ণাঙ্ক)KS_SO,	"[SO]"},
    {(পূর্ণাঙ্ক)KS_UE,	"[UE]"},
    {(পূর্ণাঙ্ক)KS_US,	"[US]"},
    {(পূর্ণাঙ্ক)KS_UCE,	"[UCE]"},
    {(পূর্ণাঙ্ক)KS_UCS,	"[UCS]"},
    {(পূর্ণাঙ্ক)KS_USS,	"[USS]"},
    {(পূর্ণাঙ্ক)KS_DS,	"[DS]"},
    {(পূর্ণাঙ্ক)KS_CDS,	"[CDS]"},
    {(পূর্ণাঙ্ক)KS_STE,	"[STE]"},
    {(পূর্ণাঙ্ক)KS_STS,	"[STS]"},
    {(পূর্ণাঙ্ক)KS_MS,	"[MS]"},
    {(পূর্ণাঙ্ক)KS_UT,	"[UT]"},
    {(পূর্ণাঙ্ক)KS_XN,	"[XN]"},
#ifdef TERMINFO
    {(পূর্ণাঙ্ক)KS_CM,	"[%p1%dCM%p2%d]"},
#অন্যথা
    {(পূর্ণাঙ্ক)KS_CM,	"[%dCM%d]"},
#endif
    {(পূর্ণাঙ্ক)KS_SR,	"[SR]"},
#ifdef TERMINFO
    {(পূর্ণাঙ্ক)KS_CRI,	"[CRI%p1%d]"},
#অন্যথা
    {(পূর্ণাঙ্ক)KS_CRI,	"[CRI%d]"},
#endif
    {(পূর্ণাঙ্ক)KS_VB,	"[VB]"},
    {(পূর্ণাঙ্ক)KS_KS,	"[KS]"},
    {(পূর্ণাঙ্ক)KS_KE,	"[KE]"},
    {(পূর্ণাঙ্ক)KS_TI,	"[TI]"},
    {(পূর্ণাঙ্ক)KS_TE,	"[TE]"},
    {(পূর্ণাঙ্ক)KS_CIS,	"[CIS]"},
    {(পূর্ণাঙ্ক)KS_CIE,	"[CIE]"},
    {(পূর্ণাঙ্ক)KS_CSC,	"[CSC]"},
    {(পূর্ণাঙ্ক)KS_CEC,	"[CEC]"},
    {(পূর্ণাঙ্ক)KS_TS,	"[TS]"},
    {(পূর্ণাঙ্ক)KS_FS,	"[FS]"},
#ifdef TERMINFO
    {(পূর্ণাঙ্ক)KS_CWS,	"[%p1%dCWS%p2%d]"},
    {(পূর্ণাঙ্ক)KS_CWP,	"[%p1%dCWP%p2%d]"},
#অন্যথা
    {(পূর্ণাঙ্ক)KS_CWS,	"[%dCWS%d]"},
    {(পূর্ণাঙ্ক)KS_CWP,	"[%dCWP%d]"},
#endif
    {(পূর্ণাঙ্ক)KS_CRV,	"[CRV]"},
    {(পূর্ণাঙ্ক)KS_CXM,	"[CXM]"},
    {(পূর্ণাঙ্ক)KS_U7,	"[U7]"},
    {(পূর্ণাঙ্ক)KS_RFG,	"[RFG]"},
    {(পূর্ণাঙ্ক)KS_RBG,	"[RBG]"},
    {(পূর্ণাঙ্ক)KS_CF,	"[CF%d]"},
    {K_UP,		"[KU]"},
    {K_DOWN,		"[KD]"},
    {K_বাম,		"[KL]"},
    {K_ডান,		"[KR]"},
    {K_XUP,		"[xKU]"},
    {K_XDOWN,		"[xKD]"},
    {K_XLEFT,		"[xKL]"},
    {K_XRIGHT,		"[xKR]"},
    {K_S_UP,		"[S-KU]"},
    {K_S_DOWN,		"[S-KD]"},
    {K_S_বাম,		"[S-KL]"},
    {K_C_বাম,		"[C-KL]"},
    {K_S_ডান,		"[S-KR]"},
    {K_C_ডান,		"[C-KR]"},
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

    {(পূর্ণাঙ্ক)KS_NAME,	রিক্ত}  // end marker
};

/*
 * List of builtin terminals.
 */
প্রকারনির্দেশ সংরচনা {
    বর্ণ	    *bitc_name;	    // name, such as "xterm"
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
#যদি defined(FEAT_GUI)
    {"gui",	    builtin_gui},
#endif
    {"amiga",	    builtin_amiga},
    {"dumb",	    builtin_dumb},
    {"debug",	    builtin_debug},

    {রিক্ত,	    রিক্ত},  // end marker
};

#যদি defined(FEAT_TERMGUICOLORS)
    স্থির guicolor_T
termgui_mch_get_color(char_u *name)
{
    প্রত্যাগম gui_get_color_cmn(name);
}

    guicolor_T
termgui_get_color(char_u *name)
{
    guicolor_T	t;

    যদি (*name == NUL)
	প্রত্যাগম INVALCOLOR;
    t = termgui_mch_get_color(name);

    যদি (t == INVALCOLOR)
	semsg(_(e_cannot_allocate_color_str), name);
    প্রত্যাগম t;
}

    guicolor_T
termgui_mch_get_rgb(guicolor_T color)
{
    প্রত্যাগম color;
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

#যদি defined(UNIX)
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
char_u *(term_strings[(পূর্ণাঙ্ক)KS_LAST + 1]);

স্থির পূর্ণাঙ্ক	need_gather = FALSE;	    // need to fill termleader[]
স্থির char_u	termleader[256 + 1];	    // for check_termcode()
#ifdef FEAT_TERMRESPONSE
স্থির পূর্ণাঙ্ক	check_for_codes = FALSE;    // check for key code response
#endif

/*
 * Structure and table to store terminal features that can be detected by
 * querying the terminal.  Either by inspecting the termresponse or a more
 * specific request.  Besides this there are:
 * t_colors - number of colors supported
 */
প্রকারনির্দেশ সংরচনা {
    বর্ণ    *tpr_name;
    পূর্ণাঙ্ক	    tpr_set_by_termresponse;
    পূর্ণাঙ্ক	    tpr_status;
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
#define TPR_গুনতি		    7

স্থির termprop_T term_props[TPR_গুনতি];

/*
 * Set the "rgb" terminal property from the color count.  The entry of a
 * direct color terminal, such as xterm-direct, has 16777216 colors.
 */
    স্থির শূন্য
set_rgb_term_prop(শূন্য)
{
    term_props[TPR_RGB].tpr_status = t_colors == 0x1000000
						       ? TPR_YES : TPR_UNKNOWN;
}

/*
 * Initialize the term_props table.
 * When "all" is FALSE only set those that are detected from the version
 * response.
 */
    শূন্য
init_term_props(পূর্ণাঙ্ক all)
{
    পূর্ণাঙ্ক i;

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

    প্রতি (i = 0; i < TPR_গুনতি; ++i)
	যদি (all || term_props[i].tpr_set_by_termresponse)
	    term_props[i].tpr_status = TPR_UNKNOWN;

    // Derived from the color count, not reset.
    set_rgb_term_prop();
}

#যদি defined(FEAT_EVAL)
    শূন্য
f_terminalprops(typval_T *argvars UNUSED, typval_T *rettv)
{
# ifdef FEAT_TERMRESPONSE
    পূর্ণাঙ্ক i;
# endif

    যদি (rettv_dict_alloc(rettv) == FAIL)
	প্রত্যাগম;
# ifdef FEAT_TERMRESPONSE
    প্রতি (i = 0; i < TPR_গুনতি; ++i)
    {
	char_u	মান[2];

	মান[0] = term_props[i].tpr_status;
	মান[1] = NUL;
	dict_add_string_len(rettv->vval.v_dict, term_props[i].tpr_name,
	    মান, (মান[0] == NUL) ? 0 : 1);
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
    স্থির tcap_entry_T *
find_builtin_term(char_u *term)
{
    প্রতি (পূর্ণাঙ্ক i = 0; ; ++i)
    {
	char_u *name = (char_u *)builtin_terminals[i].bitc_name;
	যদি (name == রিক্ত)  // end marker
	    ভঙ্গ;
#ifdef UNIX
	যদি (STRCMP(name, "iris-ansi") == 0 && vim_is_iris(term))
	    প্রত্যাগম builtin_terminals[i].bitc_table;
	যদি (STRCMP(name, "xterm") == 0 && vim_is_xterm(term))
	    প্রত্যাগম builtin_terminals[i].bitc_table;
#endif
#ifdef VMS
	যদি (STRCMP(name, "vt320") == 0 && vim_is_vt300(term))
	    প্রত্যাগম builtin_terminals[i].bitc_table;
#endif
	যদি (STRCMP(term, name) == 0)
	    প্রত্যাগম builtin_terminals[i].bitc_table;
    }
    প্রত্যাগম রিক্ত;
}

/*
 * Apply entries from a builtin termcap.
 */
    স্থির শূন্য
apply_builtin_tcap(char_u *term, tcap_entry_T *entries, পূর্ণাঙ্ক overwrite)
{
    পূর্ণাঙ্ক term_8bit = term_is_8bit(term);

    প্রতি (tcap_entry_T *p = entries;
	      p->bt_entry != (পূর্ণাঙ্ক)KS_NAME && p->bt_entry != BT_EXTRA_চাবিগুলি; ++p)
    {
	যদি ((পূর্ণাঙ্ক)p->bt_entry >= 0)	// KS_xx entry
	{
	    // Only set the value if it wasn't set yet or "overwrite" is TRUE.
	    যদি (term_strings[p->bt_entry] == রিক্ত
		    || term_strings[p->bt_entry] == empty_option
		    || overwrite)
	    {
#ifdef FEAT_EVAL
		পূর্ণাঙ্ক opt_idx = -1;
#endif
		// 8bit terminal: use CSI instead of <Esc>[
		যদি (term_8bit && term_7to8bit((char_u *)p->bt_string) != 0)
		{
		    char_u  *s, *t;

		    s = vim_strsave((char_u *)p->bt_string);
		    যদি (s != রিক্ত)
		    {
			প্রতি (t = s; *t; ++t)
			    যদি (term_7to8bit(t))
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
		অন্যথা
		{
		    term_strings[p->bt_entry] = (char_u *)p->bt_string;
#ifdef FEAT_EVAL
		    opt_idx = get_term_opt_idx(&term_strings[p->bt_entry]);
#endif
		}
#ifdef FEAT_EVAL
		set_term_option_sctx_idx(রিক্ত, opt_idx);
#endif
	    }
	}
	অন্যথা
	{
	    char_u  name[2];
	    name[0] = KEY2TERMCAP0((পূর্ণাঙ্ক)p->bt_entry);
	    name[1] = KEY2TERMCAP1((পূর্ণাঙ্ক)p->bt_entry);
	    যদি (find_termcode(name) == রিক্ত || overwrite)
		add_termcode(name, (char_u *)p->bt_string, term_8bit);
	}
    }
}

/*
 * Apply builtin termcap entries for a given keyprotocol.
 */
    শূন্য
apply_keyprotocol(char_u *term, keyprot_T prot)
{
    যদি (prot == KEYPROTOCOL_KITTY)
	apply_builtin_tcap(term, builtin_kitty, TRUE);
    যদি (prot == KEYPROTOCOL_MOK2)
	apply_builtin_tcap(term, builtin_mok2, TRUE);

    যদি (prot != KEYPROTOCOL_NONE)
	// Some function keys may accept modifiers even though the
	// terminfo/termcap entry does not indicate this.
	accept_modifiers_for_function_চাবিগুলি();
}

/*
 * Parsing of the builtin termcap entries.
 * Caller should check if "term" is a valid builtin terminal name.
 * The terminal's name is not set, as this is already done in termcapinit().
 */
    স্থির শূন্য
parse_builtin_tcap(char_u *term)
{
    tcap_entry_T *entries = find_builtin_term(term);
    যদি (entries != রিক্ত)
	apply_builtin_tcap(term, entries, FALSE);
}

/*
 * Set number of colors.
 * Store it as a number in t_colors.
 * Store it as a string in T_CCO (using nr_colors[]).
 */
    শূন্য
set_color_গুনতি(পূর্ণাঙ্ক nr)
{
    char_u	nr_colors[20];		// string for number of colors

    t_colors = nr;
    যদি (t_colors > 1)
	sprintf((বর্ণ *)nr_colors, "%d", t_colors);
    অন্যথা
	*nr_colors = NUL;
#যদি 0
# ifdef FEAT_TERMGUICOLORS
    // xterm-direct, enable termguicolors, when it wasn't set yet
    যদি (t_colors == 0x1000000 && !p_tgc_set)
	set_option_মান((char_u *)"termguicolors", 1L, রিক্ত, 0);
# endif
#endif
    set_string_option_direct((char_u *)"t_Co", -1, nr_colors, OPT_FREE, 0);
}

/*
 * Set the color count to "val" and redraw if it changed.
 */
    স্থির শূন্য
may_adjust_color_গুনতি(পূর্ণাঙ্ক val)
{
    যদি (val == t_colors)
	প্রত্যাগম;

    // Nr of colors changed, initialize highlighting and redraw everything.
    // This causes a redraw, which usually clears the message.  Try keeping
    // the message if it might work.
    set_keep_বার্তা_from_hist();
    set_color_গুনতি(val);
    init_highlight(TRUE, FALSE);
#ifdef DEBUG_TERMRESPONSE
    {
	পূর্ণাঙ্ক r = redraw_asap(UPD_CLEAR);

	LOG_TRN("Received t_Co, redraw_asap(): %d", r);
    }
#অন্যথা
    redraw_asap(UPD_CLEAR);
#endif
}

#ifdef HAVE_TGETENT
স্থির বর্ণ *(চাবি_names[]) =
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
    রিক্ত
};
#endif

#যদি defined(HAVE_TGETENT) || defined(FEAT_TERMGUICOLORS)
/*
 * Return TRUE if "term_strings[idx]" was not set.
 */
    স্থির পূর্ণাঙ্ক
term_strings_not_set(গণনা Special_চাবি idx)
{
    প্রত্যাগম TERM_STR(idx) == রিক্ত || TERM_STR(idx) == empty_option;
}
#endif

#ifdef HAVE_TGETENT
/*
 * Get the termcap entries we need with tgetstr(), tgetflag() and tgetnum().
 * "invoke_tgetent()" must have been called before.
 * If "*height" or "*width" are not zero then use the "li" and "col" entries to
 * get their value.
 */
    স্থির শূন্য
get_term_entries(পূর্ণাঙ্ক *height, পূর্ণাঙ্ক *width)
{
    স্থির সংরচনা {
		    গণনা Special_চাবি dest; // index in term_strings[]
		    বর্ণ *name;		  // termcap name for string
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
			{(গণনা Special_চাবি)0, রিক্ত}
		    };
    পূর্ণাঙ্ক		    i;
    স্থির char_u   tstrbuf[TBUFSZ];
    char_u	    *tp = tstrbuf;

    /*
     * get output strings
     */
    প্রতি (i = 0; string_names[i].name != রিক্ত; ++i)
    {
	যদি (term_strings_not_set(string_names[i].dest))
	{
	    TERM_STR(string_names[i].dest) = TGETSTR(string_names[i].name, &tp);
# ifdef FEAT_EVAL
	    set_term_option_sctx_idx(string_names[i].name, -1);
# endif
	}
    }

    // tgetflag() returns 1 if the flag is present, 0 if not and
    // possibly -1 if the flag doesn't exist.
    যদি ((T_MS == রিক্ত || T_MS == empty_option) && tgetflag("ms") > 0)
	T_MS = (char_u *)"y";
    যদি ((T_XS == রিক্ত || T_XS == empty_option) && tgetflag("xs") > 0)
	T_XS = (char_u *)"y";
    যদি ((T_XN == রিক্ত || T_XN == empty_option) && tgetflag("xn") > 0)
	T_XN = (char_u *)"y";
    যদি ((T_DB == রিক্ত || T_DB == empty_option) && tgetflag("db") > 0)
	T_DB = (char_u *)"y";
    যদি ((T_DA == রিক্ত || T_DA == empty_option) && tgetflag("da") > 0)
	T_DA = (char_u *)"y";
    যদি ((T_UT == রিক্ত || T_UT == empty_option) && tgetflag("ut") > 0)
	T_UT = (char_u *)"y";
    যদি ((T_XON == রিক্ত || T_XON == empty_option) && tgetflag("xo") > 0)
	T_XON = (char_u *)"y";

    /*
     * get key codes
     */
    প্রতি (i = 0; চাবি_names[i] != রিক্ত; ++i)
	যদি (find_termcode((char_u *)চাবি_names[i]) == রিক্ত)
	{
	    char_u *p = TGETSTR(চাবি_names[i], &tp);

	    // if cursor-left == backspace, ignore it (televideo 925)
	    যদি (p != রিক্ত
		    && (*p != Ctrl_H
			|| চাবি_names[i][0] != 'k'
			|| চাবি_names[i][1] != 'l'))
		add_termcode((char_u *)চাবি_names[i], p, FALSE);
	}

    যদি (*height == 0)
	*height = tgetnum("li");
    যদি (*width == 0)
	*width = tgetnum("co");

    /*
     * Get number of colors (if not done already).
     */
    যদি (term_strings_not_set(KS_CCO))
    {
	set_color_গুনতি(tgetnum("Co"));
# ifdef FEAT_EVAL
	set_term_option_sctx_idx("Co", -1);
# endif
    }

# ifndef hpux
    BC = (বর্ণ *)TGETSTR("bc", &tp);
    UP = (বর্ণ *)TGETSTR("up", &tp);
    char_u *p = TGETSTR("pc", &tp);
    যদি (p != রিক্ত)
	PC = *p;
# endif
}
#endif

/*
 * Report "term" is not found and list the ones we do know about.
 */
    স্থির শূন্য
report_term_error(বর্ণ *error_বার্তা, char_u *term)
{
    mch_errmsg("\r\n");
    যদি (error_বার্তা != রিক্ত)
    {
	mch_errmsg(error_বার্তা);
	mch_errmsg("\r\n");
    }
    mch_errmsg("'");
    mch_errmsg((বর্ণ *)term);
    mch_errmsg(_("' not known. Available builtin terminals are:"));
    mch_errmsg("\r\n");

    প্রতি (পূর্ণাঙ্ক i = 0; ; ++i)
    {
	বর্ণ *name = builtin_terminals[i].bitc_name;
	যদি (name == রিক্ত)  // end marker
	    ভঙ্গ;
	// Do not mention the "gui" entry, the user won't need to type it.
	যদি (STRCMP(name, "gui") != 0)
	{
#ifdef HAVE_TGETENT
	    mch_errmsg("    builtin_");
#অন্যথা
	    mch_errmsg("    ");
#endif
	    mch_errmsg(name);
	    mch_errmsg("\r\n");
	}
    }
    // Output extra 'cmdheight' line breaks to avoid that the following error
    // message overwrites the last terminal name.
    প্রতি (পূর্ণাঙ্ক i = 1; i < p_ch; ++i)
	mch_errmsg("\r\n");
}

    স্থির শূন্য
report_default_term(char_u *term)
{
    mch_errmsg(_("defaulting to '"));
    mch_errmsg((বর্ণ *)term);
    mch_errmsg("'\r\n");
    যদি (emsg_silent == 0 && !in_assert_fails)
    {
	screen_start();	// don't know where cursor is now
	out_flush();
	যদি (!is_not_a_term())
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
    পূর্ণাঙ্ক		len = (পূর্ণাঙ্ক)STRLEN(p_kpc) + 1;
    char_u	*buf = alloc(len);
    যদি (buf == রিক্ত)
	প্রত্যাগম KEYPROTOCOL_FAIL;

    keyprot_T	ret = KEYPROTOCOL_FAIL;
    char_u	*p = p_kpc;
    যাবত্ (*p != NUL)
    {
	// Isolate one comma separated item.
	(শূন্য)copy_option_part(&p, buf, len, ",");
	char_u *colon = vim_strchr(buf, ':');
	যদি (colon == রিক্ত || colon == buf || colon[1] == NUL)
	    গচ্ছ theend;
	*colon = NUL;

	keyprot_T prot;
	// Note: Keep this in sync with p_kpc_protocol_values.
	যদি (STRCMP(colon + 1, "none") == 0)
	    prot = KEYPROTOCOL_NONE;
	অন্যথা যদি (STRCMP(colon + 1, "mok2") == 0)
	    prot = KEYPROTOCOL_MOK2;
	অন্যথা যদি (STRCMP(colon + 1, "kitty") == 0)
	    prot = KEYPROTOCOL_KITTY;
	অন্যথা
	    গচ্ছ theend;

	regmatch_T regmatch;
	CLEAR_FIELD(regmatch);
	regmatch.rm_ic = TRUE;
	regmatch.regprog = vim_regcomp(buf, RE_MAGIC);
	যদি (regmatch.regprog == রিক্ত)
	    গচ্ছ theend;

	পূর্ণাঙ্ক match = term != রিক্ত && vim_regexec(&regmatch, term, (colnr_T)0);
	vim_regfree(regmatch.regprog);
	যদি (match)
	{
	    ret = prot;
	    গচ্ছ theend;
	}

    }

    // No match found, use "none".
    ret = KEYPROTOCOL_NONE;

theend:
    vim_free(buf);
    প্রত্যাগম ret;
}

/*
 * Set terminal options for terminal "term".
 * Return OK if terminal 'term' was found in a termcap, FAIL otherwise.
 *
 * While doing this, until ttest(), some options may be NULL, be careful.
 */
    পূর্ণাঙ্ক
set_termname(char_u *term)
{
#ifdef HAVE_TGETENT
    পূর্ণাঙ্ক		builtin_first = p_tbi;
    পূর্ণাঙ্ক		try;
    পূর্ণাঙ্ক		termcap_cleared = FALSE;
#endif
    পূর্ণাঙ্ক		width = 0, height = 0;
    বর্ণ	*error_বার্তা = রিক্ত;
    char_u	*bs_p, *del_p;

    // In silect mode (ex -s) we don't use the 'term' option.
    যদি (silent_mode)
	প্রত্যাগম OK;

    detected_8bit = FALSE;		// reset 8-bit detection

    যদি (term_is_builtin(term))
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
    প্রতি (try = builtin_first ? 0 : 1; try < 3; ++try)
    {
	/*
	 * Use external termcap
	 */
	যদি (try == 1)
	{
	    char_u	    tbuf[TBUFSZ];

	    /*
	     * If the external termcap does not have a matching entry, try the
	     * builtin ones.
	     */
	    যদি ((error_বার্তা = invoke_tgetent(tbuf, term)) == রিক্ত)
	    {
		যদি (!termcap_cleared)
		{
		    clear_termoptions();	// clear old options
		    termcap_cleared = TRUE;
		}

		get_term_entries(&height, &width);
	    }
	}
	অন্যথা	    // try == 0 || try == 2
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
	    যদি (try == 2 && builtin_first && termcap_cleared)
		ভঙ্গ;
#endif
	    /*
	     * Search for 'term' in builtin_terminals[].
	     */
	    tcap_entry_T *termp = find_builtin_term(term);
	    যদি (termp == রিক্ত)	// did not find it
	    {
#ifdef HAVE_TGETENT
		/*
		 * If try == 0, first try the external termcap. If that is not
		 * found we'll get back here with try == 2.
		 * If termcap_cleared is set we used the external termcap,
		 * don't complain about not finding the term in the builtin
		 * termcap.
		 */
		যদি (try == 0)			// try external one
		    অনুবর্ত;
		যদি (termcap_cleared)		// found in external termcap
		    ভঙ্গ;
#endif
		report_term_error(error_বার্তা, term);

		// when user typed :set term=xxx, quit here
		যদি (starting != NO_SCREEN)
		{
		    screen_start();	// don't know where cursor is now
		    wait_return(TRUE);
		    প্রত্যাগম FAIL;
		}
		term = DEFAULT_TERM;
		report_default_term(term);
		set_string_option_direct((char_u *)"term", -1, term,
								 OPT_FREE, 0);
		display_errors();
	    }
	    out_flush();
#ifdef HAVE_TGETENT
	    যদি (!termcap_cleared)
	    {
#endif
		clear_termoptions();	    // clear old options
#ifdef HAVE_TGETENT
		termcap_cleared = TRUE;
	    }
#endif
	    parse_builtin_tcap(term);

#ifdef FEAT_GUI
	    যদি (term_is_gui(term))
	    {
		out_flush();
		gui_init();
		// If starting the GUI failed, don't do any of the other
		// things for this terminal
		যদি (!gui.in_use)
		    প্রত্যাগম FAIL;
# ifdef HAVE_TGETENT
		ভঙ্গ;		// don't try using external termcap
# endif
	    }
#endif // FEAT_GUI
	}
#ifdef HAVE_TGETENT
    }
#endif

#ifdef FEAT_GUI
    যদি (!gui.in_use)
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
	যদি (term_strings_not_set(KS_8F)
		&& term_strings_not_set(KS_8B)
		&& term_strings_not_set(KS_8U))
	    apply_builtin_tcap(term, builtin_rgb, TRUE);
#endif
#ifdef HAVE_TGETENT
	যদি (term_strings_not_set(KS_CF))
	    apply_builtin_tcap(term, special_term, TRUE);
	যদি (term_strings_not_set(KS_BSU) && term_strings_not_set(KS_ESU))
	    apply_builtin_tcap(term, builtin_sync_output, TRUE);
#endif
    }

/*
 * special: There is no info in the termcap about whether the cursor
 * positioning is relative to the start of the screen or to the start of the
 * scrolling region.  We just guess here. Only msdos pcterm is known to do it
 * relative.
 */
    যদি (STRCMP(term, "pcterm") == 0)
	T_CCS = (char_u *)"yes";
    অন্যথা
	T_CCS = empty_option;

    // Special case: "kitty" may not have a "RV" entry in terminfo, but we need
    // to request the version for several other things to work.
    যদি (strstr((বর্ণ *)term, "kitty") != রিক্ত
					   && (T_CRV == রিক্ত || *T_CRV == NUL))
	T_CRV = (char_u *)"\033[>c";

#ifdef UNIX
/*
 * Any "stty" settings override the default for t_kb from the termcap.
 * This is in os_unix.c, because it depends a lot on the version of unix that
 * is being used.
 * Don't do this when the GUI is active, it uses "t_kb" and "t_kD" directly.
 */
# ifdef FEAT_GUI
    যদি (!gui.in_use)
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
    যদি (!gui.in_use)
#endif
    {
	bs_p = find_termcode((char_u *)"kb");
	del_p = find_termcode((char_u *)"kD");
	যদি (bs_p == রিক্ত || *bs_p == NUL)
	    add_termcode((char_u *)"kb", (bs_p = (char_u *)CTRL_H_STR), FALSE);
	যদি ((del_p == রিক্ত || *del_p == NUL) &&
					    (bs_p == রিক্ত || *bs_p != DEL))
	    add_termcode((char_u *)"kD", (char_u *)DEL_STR, FALSE);
    }

#যদি defined(UNIX) || defined(VMS)
    term_is_xterm = vim_is_xterm(term);
#endif
#ifdef FEAT_TERMRESPONSE
    // Reset terminal properties that are set based on the termresponse, which
    // will be sent out soon.
    init_term_props(FALSE);
#endif

#যদি defined(UNIX) || defined(VMS)
    // If the first number in t_XM is 1006 then the terminal will support SGR
    // mouse reporting.
    পূর্ণাঙ্ক did_set_ttym = FALSE;
    যদি (T_CXM != রিক্ত && *T_CXM != NUL && !option_was_set((char_u *)"ttym"))
    {
	char_u *p = T_CXM;

	যাবত্ (*p != NUL && !VIM_ISDIGIT(*p))
	    ++p;
	যদি (getdigits(&p) == 1006)
	{
	    did_set_ttym = TRUE;
	    set_option_মান_give_err((char_u *)"ttym", 0L, (char_u *)"sgr", 0);
	}
    }

    /*
     * For Unix, set the 'ttymouse' option to the type of mouse to be used.
     * The termcode for the mouse is added as a side effect in option.c.
     */
    {
	char_u	*p = (char_u *)"";

# ifdef FEAT_MOUSE_XTERM
	যদি (use_xterm_like_mouse(term))
	{
	    যদি (use_xterm_mouse())
		p = রিক্ত;	// keep existing value, might be "xterm2"
	    অন্যথা
		p = (char_u *)"xterm";
	}
# endif
	যদি (p != রিক্ত && !did_set_ttym)
	{
	    set_option_মান_give_err((char_u *)"ttym", 0L, p, 0);
	    // Reset the WAS_SET flag, 'ttymouse' can be set to "sgr" or
	    // "xterm2" in check_termcode().
	    reset_option_was_set((char_u *)"ttym");
	}
	যদি (p == রিক্ত
# ifdef FEAT_GUI
		|| gui.in_use
# endif
		)
	    check_mouse_termcode();	// set mouse termcode anyway
    }
#অন্যথা
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
#যদি (defined(UNIX) || defined(VMS))
    // First time after setting 'term' a focus event is always reported.
    focus_state = MAYBE;
#endif

#ifdef USE_TERM_CONSOLE
    // DEFAULT_TERM indicates that it is the machine console.
    যদি (STRCMP(term, DEFAULT_TERM) != 0)
	term_console = FALSE;
    অন্যথা
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
    যদি (starting != NO_SCREEN)
    {
	starttermcap();		// may change terminal mode
	setmouse();		// may start using the mouse
	maketitle();		// may display window title
    }

	// display initial screen after ttest() checking. jw.
    যদি (width <= 0 || height <= 0)
    {
	// termcap failed to report size
	// set defaults, in case ui_get_shellsize() also fails
	width = 80;
#যদি defined(MSWIN)
	height = 25;	    // console is often 25 lines
#অন্যথা
	height = 24;	    // most terminals are 24 lines
#endif
    }
    set_shellsize(width, height, FALSE);	// may change Rows
    যদি (starting != NO_SCREEN)
    {
	যদি (scroll_region)
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
		যদি (curbuf->b_ml.ml_mfp != রিক্ত)
		{
		    aucmd_prepbuf(&aco, buf);
		    যদি (curbuf == buf)
		    {
			apply_autocmds(EVENT_TERMCHANGED, রিক্ত, রিক্ত, FALSE,
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
    (শূন্য)update_image_backend();
#endif

    প্রত্যাগম OK;
}

#যদি defined(EXITFREE)

# ifdef HAVE_DEL_CURTERM
#  include <term.h>	    // declares cur_term
# endif

/*
 * If supported, delete "cur_term", which caches terminal related entries.
 * Avoids that valgrind reports possibly lost memory.
 */
    শূন্য
free_cur_term(শূন্য)
{
# ifdef HAVE_DEL_CURTERM
    যদি (cur_term)
	del_curterm(cur_term);
# endif
}

#endif

#ifdef HAVE_TGETENT
/*
 * Call tgetent()
 * Return error message if it fails, NULL if it's OK.
 */
    স্থির বর্ণ *
invoke_tgetent(char_u *tbuf, char_u *term)
{
    পূর্ণাঙ্ক	    i;

    // Note: Valgrind may report a leak here, because the library keeps one
    // buffer around that we can't ever free.
    i = TGETENT(tbuf, term);
    যদি (i < 0		    // -1 is always an error
# ifdef TGETENT_ZERO_ERR
	    || i == 0	    // sometimes zero is also an error
# endif
       )
    {
	// On FreeBSD tputs() gets a SEGV after a tgetent() which fails.  Call
	// tgetent() with the always existing "dumb" entry to avoid a crash or
	// hang.
	(শূন্য)TGETENT(tbuf, "dumb");

	যদি (i < 0)
# ifdef TGETENT_ZERO_ERR
	    প্রত্যাগম _(e_cannot_open_termcap_file);
	যদি (i == 0)
# endif
# ifdef TERMINFO
	    প্রত্যাগম _(e_terminal_entry_not_found_in_terminfo);
# অন্যথা
	    প্রত্যাগম _(e_terminal_entry_not_found_in_termcap);
# endif
    }
    প্রত্যাগম রিক্ত;
}

/*
 * Some versions of tgetstr() have been reported to return -1 instead of NULL.
 * Fix that here.
 */
    স্থির char_u *
vim_tgetstr(বর্ণ *s, char_u **pp)
{
    বর্ণ	*p;

    p = tgetstr(s, (বর্ণ **)pp);
    যদি (p == (বর্ণ *)-1)
	p = রিক্ত;
    প্রত্যাগম (char_u *)p;
}
#endif // HAVE_TGETENT

#যদি defined(HAVE_TGETENT) && (defined(UNIX) || defined(VMS) || defined(MACOS_X))
/*
 * Get Columns and Rows from the termcap. Used after a window signal if the
 * ioctl() fails. It doesn't make sense to call tgetent each time if the "co"
 * and "li" entries never change. But on some systems this works.
 * Errors while getting the entries are ignored.
 */
    শূন্য
getlinecol(
    long	*cp,	// pointer to columns
    long	*rp)	// pointer to rows
{
    char_u	tbuf[TBUFSZ];

    যদি (T_NAME == রিক্ত || *T_NAME == NUL
	    || invoke_tgetent(tbuf, T_NAME) != রিক্ত)
	প্রত্যাগম;

    যদি (*cp == 0)
	*cp = tgetnum("co");
    যদি (*rp == 0)
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
    পূর্ণাঙ্ক
add_termcap_entry(char_u *name, পূর্ণাঙ্ক force)
{
    char_u  *term;
    পূর্ণাঙ্ক	    চাবি;
#ifdef HAVE_TGETENT
    char_u  *string;
    পূর্ণাঙ্ক	    i;
    পূর্ণাঙ্ক	    builtin_first;
    char_u  tbuf[TBUFSZ];
    char_u  tstrbuf[TBUFSZ];
    char_u  *tp = tstrbuf;
    বর্ণ    *error_বার্তা = রিক্ত;
#endif

/*
 * If the GUI is running or will start in a moment, we only support the keys
 * that the GUI can produce.
 */
#ifdef FEAT_GUI
    যদি (gui.in_use || gui.starting)
	প্রত্যাগম gui_mch_haskey(name);
#endif

    যদি (!force && find_termcode(name) != রিক্ত)	    // it's already there
	প্রত্যাগম OK;

    term = T_NAME;
    যদি (term == রিক্ত || *term == NUL)	    // 'term' not defined yet
	প্রত্যাগম FAIL;

    যদি (term_is_builtin(term))		    // name starts with "builtin_"
    {
	term += 8;
#ifdef HAVE_TGETENT
	builtin_first = TRUE;
#endif
    }
#ifdef HAVE_TGETENT
    অন্যথা
	builtin_first = p_tbi;
#endif

#ifdef HAVE_TGETENT
/*
 * We can get the entry from the builtin termcap and from the external one.
 * If 'ttybuiltin' is on or the terminal name starts with "builtin_", try
 * builtin termcap first.
 * If 'ttybuiltin' is off, try external termcap first.
 */
    প্রতি (i = 0; i < 2; ++i)
    {
	যদি ((!builtin_first) == i)
#endif
	/*
	 * Search in builtin termcaps
	 */
	{
	    tcap_entry_T *termp = find_builtin_term(term);
	    যদি (termp != রিক্ত)	// found it
	    {
		চাবি = TERMCAP2_চাবি(name[0], name[1]);
		++termp;
		যাবত্ (termp->bt_entry != (পূর্ণাঙ্ক)KS_NAME)
		{
		    যদি ((পূর্ণাঙ্ক)termp->bt_entry == চাবি)
		    {
			add_termcode(name, (char_u *)termp->bt_string,
							  term_is_8bit(term));
			প্রত্যাগম OK;
		    }
		    ++termp;
		}
	    }
	}
#ifdef HAVE_TGETENT
	অন্যথা
	/*
	 * Search in external termcap
	 */
	{
	    error_বার্তা = invoke_tgetent(tbuf, term);
	    যদি (error_বার্তা == রিক্ত)
	    {
		string = TGETSTR((বর্ণ *)name, &tp);
		যদি (string != রিক্ত && *string != NUL)
		{
		    add_termcode(name, string, FALSE);
		    প্রত্যাগম OK;
		}
	    }
	}
    }
#endif

    যদি (SOURCING_NAME == রিক্ত)
    {
#ifdef HAVE_TGETENT
	যদি (error_বার্তা != রিক্ত)
	    emsg(error_বার্তা);
	অন্যথা
#endif
	    semsg(_(e_no_str_entry_in_termcap), name);
    }
    প্রত্যাগম FAIL;
}

    স্থির পূর্ণাঙ্ক
term_is_builtin(char_u *name)
{
    প্রত্যাগম (STRNCMP(name, "builtin_", (size_t)8) == 0);
}

/*
 * Return TRUE if terminal "name" uses CSI instead of <Esc>[.
 * Assume that the terminal is using 8-bit controls when the name contains
 * "8bit", like in "xterm-8bit".
 */
    পূর্ণাঙ্ক
term_is_8bit(char_u *name)
{
    প্রত্যাগম (detected_8bit || strstr((বর্ণ *)name, "8bit") != রিক্ত);
}

/*
 * Translate terminal control chars from 7-bit to 8-bit:
 * <Esc>[ -> CSI  <M_C_[>
 * <Esc>] -> OSC  <M-C-]>
 * <Esc>O -> <M-C-O>
 */
    স্থির পূর্ণাঙ্ক
term_7to8bit(char_u *p)
{
    যদি (*p != ESC)
	প্রত্যাগম 0;

    যদি (p[1] == '[')
	প্রত্যাগম CSI;
    অন্যথা যদি (p[1] == ']')
	প্রত্যাগম OSC;
    অন্যথা যদি (p[1] == 'O')
	প্রত্যাগম 0x8f;
    প্রত্যাগম 0;
}

#যদি defined(FEAT_GUI)
    পূর্ণাঙ্ক
term_is_gui(char_u *name)
{
    প্রত্যাগম (STRCMP(name, "builtin_gui") == 0 || STRCMP(name, "gui") == 0);
}
#endif

#যদি !defined(HAVE_TGETENT) || defined(AMIGA)

    char_u *
tltoa(unsigned long i)
{
    স্থির char_u buf[16];
    char_u	*p;

    p = buf + 15;
    *p = '\0';
    কুরু
    {
	--p;
	*p = (char_u) (i % 10 + '0');
	i /= 10;
    }
    যাবত্ (i > 0 && p > buf);
    প্রত্যাগম p;
}
#endif

#ifndef HAVE_TGETENT

/*
 * minimal tgoto() implementation.
 * no padding and we only parse for %i %d and %+char
 */
    স্থির বর্ণ *
tgoto(বর্ণ *cm, পূর্ণাঙ্ক x, পূর্ণাঙ্ক y)
{
    স্থির বর্ণ buf[30];
    বর্ণ *p, *s, *e;

    যদি (!cm)
	প্রত্যাগম "OOPS";
    e = buf + 29;
    প্রতি (s = buf; s < e && *cm; cm++)
    {
	যদি (*cm != '%')
	{
	    *s++ = *cm;
	    অনুবর্ত;
	}
	বিকল্প (*++cm)
	{
	পক্ষ 'd':
	    p = (বর্ণ *)tltoa((unsigned long)y);
	    y = x;
	    যাবত্ (*p)
		*s++ = *p++;
	    ভঙ্গ;
	পক্ষ 'i':
	    x++;
	    y++;
	    ভঙ্গ;
	পক্ষ '+':
	    *s++ = (বর্ণ)(*++cm + y);
	    y = x;
	    ভঙ্গ;
	পক্ষ '%':
	    *s++ = *cm;
	    ভঙ্গ;
	স্বভাব:
	    প্রত্যাগম "OOPS";
	}
    }
    *s = '\0';
    প্রত্যাগম buf;
}

#endif // HAVE_TGETENT

/*
 * Set the terminal name and initialize the terminal options.
 * If "name" is NULL or empty, get the terminal name from the environment.
 * If that fails, use the default terminal name.
 */
    শূন্য
termcapinit(char_u *name)
{
    char_u	*term = name;

    যদি (term != রিক্ত && *term == NUL)
	term = রিক্ত;	    // empty name is equal to no name

#ifndef MSWIN
    যদি (term == রিক্ত)
	term = mch_getenv((char_u *)"TERM");
#endif
    যদি (term == রিক্ত || *term == NUL)
	term = DEFAULT_TERM;
    set_string_option_direct((char_u *)"term", -1, term, OPT_FREE, 0);

    // Set the default terminal name.
    set_string_default("term", term);
    set_string_default("ttytype", term);

    // Avoid using "term" here, because the next mch_getenv() may overwrite it.
    set_termname(T_NAME != রিক্ত ? T_NAME : term);
}

/*
 * The number of calls to ui_write is reduced by using "out_buf".
 */
#define OUT_SIZE	8191

// add one to allow mch_write() in os_win32.c to append a NUL
স্থির char_u		out_buf[OUT_SIZE + 1];

স্থির পূর্ণাঙ্ক		out_pos = 0;	// number of chars in out_buf

// Since the maximum number of SGR parameters shown as a normal value range is
// 16, the escape sequence length can be 4 * 16 + lead + tail.
#define MAX_ESC_SEQ_LEN	80

/*
 * out_flush(): flush the output buffer
 */
    শূন্য
out_flush(শূন্য)
{
    পূর্ণাঙ্ক	    len;

    যদি (out_pos == 0)
	প্রত্যাগম;

    // set out_pos to 0 before ui_write, to avoid recursiveness
    len = out_pos;
    out_pos = 0;
    ui_write(out_buf, len, FALSE);
#ifdef FEAT_EVAL
    যদি (ch_log_output != FALSE)
    {
	out_buf[len] = NUL;
	ch_log(রিক্ত, "raw %s output: \"%s\"",
# ifdef FEAT_GUI
		(gui.in_use && !gui.dying && !gui.starting) ? "GUI" :
# endif
		"terminal",
		out_buf);
	যদি (ch_log_output == TRUE)
	    ch_log_output = FALSE;  // only log once
    }
#endif
}

/*
 * out_flush_cursor(): flush the output buffer and redraw the cursor.
 * Does not flush recursively in the GUI to avoid slow drawing.
 */
    শূন্য
out_flush_cursor(
    পূর্ণাঙ্ক	    force UNUSED,   // when TRUE, update cursor even when not moved
    পূর্ণাঙ্ক	    clear_selection UNUSED) // clear selection under cursor
{
    mch_disable_flush();
    out_flush();
    mch_enable_flush();
#ifdef FEAT_GUI
    যদি (gui.in_use)
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
    শূন্য
out_flush_check(শূন্য)
{
    যদি (enc_dbcs != 0 && out_pos >= OUT_SIZE - MB_MAXBYTES)
	out_flush();
}

#ifdef FEAT_GUI
/*
 * out_trash(): Throw away the contents of the output buffer
 */
    শূন্য
out_trash(শূন্য)
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
    শূন্য
out_char(unsigned c)
{
#যদি defined(UNIX) || defined(VMS) || defined(AMIGA) || defined(MACOS_X)
    যদি (c == '\n')	// turn LF into CR-LF (CRMOD doesn't seem to do this)
	out_char('\r');
#endif

    out_buf[out_pos++] = c;

    // For testing we flush each time.
    যদি (out_pos >= OUT_SIZE || p_wd)
	out_flush();
}

/*
 * Output "c" like out_char(), but don't flush when p_wd is set.
 */
    স্থির পূর্ণাঙ্ক
out_char_nf(পূর্ণাঙ্ক c)
{
    out_buf[out_pos++] = (unsigned)c;

    যদি (out_pos >= OUT_SIZE)
	out_flush();
    প্রত্যাগম (unsigned)c;
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
    শূন্য
out_str_nf(char_u *s)
{
    // avoid terminal strings being split up
    যদি (out_pos > OUT_SIZE - MAX_ESC_SEQ_LEN)
	out_flush();

    প্রতি (char_u *p = s; *p != NUL; ++p)
	out_char_nf(*p);

    // For testing we write one string at a time.
    যদি (p_wd)
	out_flush();
}

/*
 * A conditional-flushing out_str, mainly for visualbell.
 * Handles a delay internally, because termlib may not respect the delay or do
 * it at the wrong time.
 * Note: Only for terminal strings.
 */
    শূন্য
out_str_cf(char_u *s)
{
    যদি (s == রিক্ত || *s == NUL)
	প্রত্যাগম;

#ifdef HAVE_TGETENT
    char_u *p;
#endif

#ifdef FEAT_GUI
    // Don't use tputs() when GUI is used, ncurses crashes.
    যদি (gui.in_use)
    {
	out_str_nf(s);
	প্রত্যাগম;
    }
#endif
    যদি (out_pos > OUT_SIZE - MAX_ESC_SEQ_LEN)
	out_flush();
#ifdef HAVE_TGETENT
    প্রতি (p = s; *s; ++s)
    {
	// flush just before delay command
	যদি (*s == '$' && *(s + 1) == '<')
	{
	    char_u save_c = *s;
	    পূর্ণাঙ্ক duration = atoi((বর্ণ *)s + 2);

	    *s = NUL;
	    tputs((বর্ণ *)p, 1, TPUTSFUNCAST out_char_nf);
	    *s = save_c;
	    out_flush();
# ifdef ELAPSED_FUNC
	    // Only sleep here if we can limit this happening in
	    // vim_beep().
	    p = vim_strchr(s, '>');
	    যদি (p == রিক্ত || duration <= 0)
	    {
		// can't parse the time, don't sleep here
		p = s;
	    }
	    অন্যথা
	    {
		++p;
		do_sleep(duration, FALSE);
	    }
# অন্যথা
	    // Rely on the terminal library to sleep.
	    p = s;
# endif
	    ভঙ্গ;
	}
    }
    tputs((বর্ণ *)p, 1, TPUTSFUNCAST out_char_nf);
#অন্যথা
    যাবত্ (*s)
	out_char_nf(*s++);
#endif

    // For testing we write one string at a time.
    যদি (p_wd)
	out_flush();
}

/*
 * out_str(s): Put a character string a byte at a time into the output buffer.
 * If HAVE_TGETENT is defined use tputs(), the termcap parser. (jw)
 * This should only be used for writing terminal codes, not for outputting
 * normal text (use functions like msg_puts() and screen_putchar() for that).
 */
    শূন্য
out_str(char_u *s)
{
    যদি (s == রিক্ত || *s == NUL)
	প্রত্যাগম;

#ifdef FEAT_GUI
    // Don't use tputs() when GUI is used, ncurses crashes.
    যদি (gui.in_use)
    {
	out_str_nf(s);
	প্রত্যাগম;
    }
#endif
    // avoid terminal strings being split up
    যদি (out_pos > OUT_SIZE - MAX_ESC_SEQ_LEN)
	out_flush();
#ifdef HAVE_TGETENT
    tputs((বর্ণ *)s, 1, TPUTSFUNCAST out_char_nf);
#অন্যথা
    যাবত্ (*s)
	out_char_nf(*s++);
#endif

    // For testing we write one string at a time.
    যদি (p_wd)
	out_flush();
}

/*
 * cursor positioning using termcap parser. (jw)
 */
    শূন্য
term_windgoto(পূর্ণাঙ্ক row, পূর্ণাঙ্ক col)
{
    OUT_STR(tgoto((বর্ণ *)T_CM, col, row));
}

    শূন্য
term_cursor_ডান(পূর্ণাঙ্ক i)
{
    OUT_STR(tgoto((বর্ণ *)T_CRI, 0, i));
}

    শূন্য
term_append_সারিগুলি(পূর্ণাঙ্ক সারি_গুনতি)
{
    OUT_STR(tgoto((বর্ণ *)T_CAL, 0, সারি_গুনতি));
}

    শূন্য
term_delete_সারিগুলি(পূর্ণাঙ্ক সারি_গুনতি)
{
    OUT_STR(tgoto((বর্ণ *)T_CDL, 0, সারি_গুনতি));
}

#যদি defined(UNIX) || defined(VMS)
    শূন্য
term_enable_mouse(পূর্ণাঙ্ক enable)
{
    পূর্ণাঙ্ক on = enable ? 1 : 0;
    OUT_STR(tgoto((বর্ণ *)T_CXM, 0, on));
}
#endif

#যদি defined(HAVE_TGETENT)
    শূন্য
term_set_winpos(পূর্ণাঙ্ক x, পূর্ণাঙ্ক y)
{
    // Can't handle a negative value here
    যদি (x < 0)
	x = 0;
    যদি (y < 0)
	y = 0;
    OUT_STR(tgoto((বর্ণ *)T_CWP, y, x));
}

# যদি defined(FEAT_TERMRESPONSE)
/*
 * Return TRUE if we can request the terminal for a response.
 */
    স্থির পূর্ণাঙ্ক
can_get_termresponse(শূন্য)
{
    প্রত্যাগম cur_tmode == TMODE_RAW
	    && termcap_active
#  ifdef UNIX
	    && (is_not_a_term() || (isatty(1) && isatty(read_cmd_fd)))
#  endif
	    && p_ek;
}

/*
 * Set "status" to STATUS_SENT.
 */
    স্থির শূন্য
termrequest_sent(termrequest_T *status)
{
    status->tr_progress = STATUS_SENT;
    status->tr_start = time(রিক্ত);
}

/*
 * Return TRUE if any of the requests are in STATUS_SENT.
 */
    স্থির পূর্ণাঙ্ক
termrequest_any_pending(শূন্য)
{
    পূর্ণাঙ্ক	    i;
    time_t  now = time(রিক্ত);

    প্রতি (i = 0; all_termrequests[i] != রিক্ত; ++i)
    {
	যদি (all_termrequests[i]->tr_progress == STATUS_SENT)
	{
	    যদি (all_termrequests[i]->tr_start > 0 && now > 0
				    && all_termrequests[i]->tr_start + 2 < now)
		// Sent the request more than 2 seconds ago and didn't get a
		// response, assume it failed.
		all_termrequests[i]->tr_progress = STATUS_FAIL;
	    অন্যথা
		প্রত্যাগম TRUE;
	}
    }
    প্রত্যাগম FALSE;
}

স্থির পূর্ণাঙ্ক winpos_x = -1;
স্থির পূর্ণাঙ্ক winpos_y = -1;
স্থির পূর্ণাঙ্ক did_request_winpos = 0;

#  যদি defined(FEAT_EVAL) || defined(FEAT_TERMINAL)
/*
 * Try getting the Vim window position from the terminal.
 * Returns OK or FAIL.
 */
    পূর্ণাঙ্ক
term_get_winpos(পূর্ণাঙ্ক *x, পূর্ণাঙ্ক *y, varnumber_T timeout)
{
    পূর্ণাঙ্ক গুনতি = 0;
    পূর্ণাঙ্ক prev_winpos_x = winpos_x;
    পূর্ণাঙ্ক prev_winpos_y = winpos_y;

    যদি (*T_CGP == NUL || !can_get_termresponse())
	প্রত্যাগম FAIL;
    winpos_x = -1;
    winpos_y = -1;
    ++did_request_winpos;
    termrequest_sent(&winpos_status);
    OUT_STR(T_CGP);
    out_flush();

    // Try reading the result for "timeout" msec.
    যাবত্ (গুনতি++ <= timeout / 10 && !got_int)
    {
	(শূন্য)vpeekc_nomap();
	যদি (winpos_x >= 0 && winpos_y >= 0)
	{
	    *x = winpos_x;
	    *y = winpos_y;
	    প্রত্যাগম OK;
	}
	ui_delay(10L, FALSE);
    }
    // Do not reset "did_request_winpos", if we timed out the response might
    // still come later and we must consume it.

    winpos_x = prev_winpos_x;
    winpos_y = prev_winpos_y;
    যদি (timeout < 10 && prev_winpos_y >= 0 && prev_winpos_x >= 0)
    {
	// Polling: return previous values if we have them.
	*x = winpos_x;
	*y = winpos_y;
	প্রত্যাগম OK;
    }

    প্রত্যাগম FALSE;
}
#  endif
# endif

    শূন্য
term_set_winsize(পূর্ণাঙ্ক height, পূর্ণাঙ্ক width)
{
    OUT_STR(tgoto((বর্ণ *)T_CWS, width, height));
}
#endif

    শূন্য
term_font(পূর্ণাঙ্ক n)
{
    যদি (*T_CFO)
    {
	বর্ণ buf[20];
	sprintf(buf, (বর্ণ *)T_CFO, 9 + n);
	OUT_STR(buf);
    }
}

    স্থির শূন্য
term_color(char_u *s, পূর্ণাঙ্ক n)
{
    বর্ণ	buf[20];
    পূর্ণাঙ্ক		i = *s == CSI ? 1 : 2;
		// index in s[] just after <Esc>[ or CSI

    // Special handling of 16 colors, because termcap can't handle it
    // Also accept "\e[3%dm" for TERMINFO, it is sometimes used
    // Also accept CSI instead of <Esc>[
    যদি (n >= 8 && t_colors >= 16
	      && ((s[0] == ESC && s[1] == '[')
#যদি defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
		  || (s[0] == ESC && s[1] == '|')
#endif
		  || (s[0] == CSI && (i = 1) == 1))
	      && s[i] != NUL
	      && (STRCMP(s + i + 1, "%p1%dm") == 0
		  || STRCMP(s + i + 1, "%dm") == 0)
	      && (s[i] == '3' || s[i] == '4'))
    {
#ifdef TERMINFO
	বর্ণ *format = "%s%s%%p1%%dm";
#অন্যথা
	বর্ণ *format = "%s%s%%dm";
#endif
	বর্ণ *lead = i == 2 ? (
#যদি defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
		    s[1] == '|' ? "\033|" :
#endif
		    "\033[") : "\233";
	বর্ণ *tail = s[i] == '3' ? (n >= 16 ? "38;5;" : "9")
				 : (n >= 16 ? "48;5;" : "10");

	sprintf(buf, format, lead, tail);
	OUT_STR(tgoto(buf, 0, n >= 16 ? n : n - 8));
    }
    অন্যথা
	OUT_STR(tgoto((বর্ণ *)s, 0, n));
}

    শূন্য
term_fg_color(পূর্ণাঙ্ক n)
{
    // Use "AF" termcap entry if present, "Sf" entry otherwise
    যদি (*T_CAF)
	term_color(T_CAF, n);
    অন্যথা যদি (*T_CSF)
	term_color(T_CSF, n);
}

    শূন্য
term_bg_color(পূর্ণাঙ্ক n)
{
    // Use "AB" termcap entry if present, "Sb" entry otherwise
    যদি (*T_CAB)
	term_color(T_CAB, n);
    অন্যথা যদি (*T_CSB)
	term_color(T_CSB, n);
}

    শূন্য
term_ul_color(পূর্ণাঙ্ক n)
{
    যদি (*T_CAU)
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
term_bg_default(শূন্য)
{
#যদি defined(MSWIN)
    // DOS console is nearly always black
    প্রত্যাগম (char_u *)"dark";
#অন্যথা
    char_u	*p;

    যদি (STRCMP(T_NAME, "linux") == 0
	    || STRCMP(T_NAME, "screen.linux") == 0
	    || STRNCMP(T_NAME, "cygwin", 6) == 0
	    || STRNCMP(T_NAME, "putty", 5) == 0
	    || ((p = mch_getenv((char_u *)"COLORFGBG")) != রিক্ত
		&& (p = vim_strrchr(p, ';')) != রিক্ত
		&& ((p[1] >= '0' && p[1] <= '6') || p[1] == '8')
		&& p[2] == NUL))
	প্রত্যাগম (char_u *)"dark";
    প্রত্যাগম (char_u *)"light";
#endif
}

#যদি defined(FEAT_TERMGUICOLORS)

# define RED(rgb)   (((long_u)(rgb) >> 16) & 0xFF)
# define GREEN(rgb) (((long_u)(rgb) >>  8) & 0xFF)
# define BLUE(rgb)  (((long_u)(rgb)      ) & 0xFF)

    স্থির শূন্য
term_rgb_color(char_u *s, guicolor_T rgb)
{
# define MAX_COLOR_STR_LEN 100
    বর্ণ	buf[MAX_COLOR_STR_LEN];

    যদি (*s == NUL)
	প্রত্যাগম;
    vim_snprintf(buf, MAX_COLOR_STR_LEN,
				  (বর্ণ *)s, RED(rgb), GREEN(rgb), BLUE(rgb));
# ifdef FEAT_VTP
    যদি (use_vtp() && (p_tgc || t_colors >= 256))
    {
	out_flush();
	buf[1] = '[';
	vtp_printf(buf);
    }
    অন্যথা
# endif
	OUT_STR(buf);
}

    শূন্য
term_fg_rgb_color(guicolor_T rgb)
{
    যদি (rgb != INVALCOLOR)
	term_rgb_color(T_8F, rgb);
}

    শূন্য
term_bg_rgb_color(guicolor_T rgb)
{
    যদি (rgb != INVALCOLOR)
	term_rgb_color(T_8B, rgb);
}

    শূন্য
term_ul_rgb_color(guicolor_T rgb)
{
# ifdef FEAT_TERMRESPONSE
    // If the user explicitly sets t_8u then use it.  Otherwise wait for
    // termresponse to be received, which is when t_8u would be set and a
    // redraw is needed if it was used.
    যদি (!option_was_set((char_u *)"t_8u") && write_t_8u_state != OK)
	write_t_8u_state = MAYBE;
    অন্যথা
# endif
	term_rgb_color(T_8U, rgb);
}
#endif

#যদি defined(UNIX) || defined(VMS) || defined(MACOS_X)
/*
 * Generic function to set window title, using t_ts and t_fs.
 */
    শূন্য
term_settitle(char_u *title)
{
    MAY_WANT_TO_LOG_THIS;

    // t_ts takes one argument: column in status line
    OUT_STR(tgoto((বর্ণ *)T_TS, 0, 0));	// set title start
    out_str_nf(title);
    out_str(T_FS);			// set title end
    out_flush();
}

/*
 * Tell the terminal to push (save) the title and/or icon, so that it can be
 * popped (restored) later.
 */
    শূন্য
term_ঠেলো_title(পূর্ণাঙ্ক which)
{
    যদি ((which & SAVE_RESTORE_TITLE) && T_CST != রিক্ত && *T_CST != NUL)
    {
	OUT_STR(T_CST);
	out_flush();
    }

    যদি ((which & SAVE_RESTORE_ICON) && T_SSI != রিক্ত && *T_SSI != NUL)
    {
	OUT_STR(T_SSI);
	out_flush();
    }
}

/*
 * Tell the terminal to pop the title and/or icon.
 */
    শূন্য
term_বের_করো_title(পূর্ণাঙ্ক which)
{
    যদি ((which & SAVE_RESTORE_TITLE) && T_CRT != রিক্ত && *T_CRT != NUL)
    {
	OUT_STR(T_CRT);
	out_flush();
    }

    যদি ((which & SAVE_RESTORE_ICON) && T_SRI != রিক্ত && *T_SRI != NUL)
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
    শূন্য
ttest(পূর্ণাঙ্ক pairs)
{
    char_u *env_colors;

    check_options();		    // make sure no options are NULL

    /*
     * MUST have "cm": cursor motion.
     */
    যদি (*T_CM == NUL)
	emsg(_(e_terminal_capability_cm_required));

    /*
     * if "cs" defined, use a scroll region, it's faster.
     */
    যদি (*T_CS != NUL)
	scroll_region = TRUE;
    অন্যথা
	scroll_region = FALSE;

    যদি (pairs)
    {
	/*
	 * optional pairs
	 */
	// TP goes to normal mode for TI (invert) and TB (bold)
	যদি (*T_ME == NUL)
	    T_ME = T_MR = T_MD = T_MB = empty_option;
	যদি (*T_SO == NUL || *T_SE == NUL)
	    T_SO = T_SE = empty_option;
	যদি (*T_US == NUL || *T_UE == NUL)
	    T_US = T_UE = empty_option;
	যদি (*T_CZH == NUL || *T_CZR == NUL)
	    T_CZH = T_CZR = empty_option;

	// T_VE is needed even though T_VI is not defined
	যদি (*T_VE == NUL)
	    T_VI = empty_option;

	// if 'mr' or 'me' is not defined use 'so' and 'se'
	যদি (*T_ME == NUL)
	{
	    T_ME = T_SE;
	    T_MR = T_SO;
	    T_MD = T_SO;
	}

	// if 'so' or 'se' is not defined use 'mr' and 'me'
	যদি (*T_SO == NUL)
	{
	    T_SE = T_ME;
	    যদি (*T_MR == NUL)
		T_SO = T_MD;
	    অন্যথা
		T_SO = T_MR;
	}

	// if 'ZH' or 'ZR' is not defined use 'mr' and 'me'
	যদি (*T_CZH == NUL)
	{
	    T_CZR = T_ME;
	    যদি (*T_MR == NUL)
		T_CZH = T_MD;
	    অন্যথা
		T_CZH = T_MR;
	}

	// "Sb" and "Sf" come in pairs
	যদি (*T_CSB == NUL || *T_CSF == NUL)
	{
	    T_CSB = empty_option;
	    T_CSF = empty_option;
	}

	// "AB" and "AF" come in pairs
	যদি (*T_CAB == NUL || *T_CAF == NUL)
	{
	    T_CAB = empty_option;
	    T_CAF = empty_option;
	}

	// if 'Sb' and 'AB' are not defined, reset "Co"
	যদি (*T_CSB == NUL && *T_CAB == NUL)
	    free_one_termoption(T_CCO);

	// Set 'weirdinvert' according to value of 't_xs'
	p_wiv = (*T_XS != NUL);
    }
    need_gather = TRUE;

    // Set t_colors to the value of $COLORS or t_Co.  Ignore $COLORS in the
    // GUI.
    t_colors = atoi((বর্ণ *)T_CCO);
#ifdef FEAT_GUI
    যদি (!gui.in_use)
#endif
    {
	env_colors = mch_getenv((char_u *)"COLORS");
	যদি (env_colors != রিক্ত && SAFE_isdigit(*env_colors))
	{
	    পূর্ণাঙ্ক colors = atoi((বর্ণ *)env_colors);

	    যদি (colors != t_colors)
		set_color_গুনতি(colors);
	}
    }
    set_rgb_term_prop();
}

#যদি defined(FEAT_GUI) && (defined(FEAT_MENU) || !defined(USE_ON_FLY_SCROLL))
/*
 * Represent the given long_u as individual bytes, with the most significant
 * byte first, and store them in dst.
 */
    শূন্য
add_long_to_buf(long_u val, char_u *dst)
{
    পূর্ণাঙ্ক	    i;
    পূর্ণাঙ্ক	    shift;

    প্রতি (i = 1; i <= (পূর্ণাঙ্ক)পরিমাণ(long_u); i++)
    {
	shift = 8 * (পরিমাণ(long_u) - i);
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
    স্থির পূর্ণাঙ্ক
get_long_from_buf(char_u *buf, long_u *val)
{
    পূর্ণাঙ্ক	    len;
    char_u  bytes[পরিমাণ(long_u)];
    পূর্ণাঙ্ক	    i;
    পূর্ণাঙ্ক	    shift;

    *val = 0;
    len = get_bytes_from_buf(buf, bytes, (পূর্ণাঙ্ক)পরিমাণ(long_u));
    যদি (len == -1)
	প্রত্যাগম -1;
    প্রতি (i = 0; i < (পূর্ণাঙ্ক)পরিমাণ(long_u); i++)
    {
	shift = 8 * (পরিমাণ(long_u) - 1 - i);
	*val += (long_u)bytes[i] << shift;
    }
    প্রত্যাগম len;
}
#endif

/*
 * Read the next num_bytes bytes from buf, and store them in bytes.  Assume
 * that buf has been through inchar().	Returns the actual number of bytes used
 * from buf (between num_bytes and num_bytes*2), or -1 if not enough bytes were
 * available.
 */
    পূর্ণাঙ্ক
get_bytes_from_buf(char_u *buf, char_u *bytes, পূর্ণাঙ্ক অঙ্ক_bytes)
{
    পূর্ণাঙ্ক	    len = 0;
    পূর্ণাঙ্ক	    i;
    char_u  c;

    প্রতি (i = 0; i < অঙ্ক_bytes; i++)
    {
	যদি ((c = buf[len++]) == NUL)
	    প্রত্যাগম -1;
	যদি (c == K_SPECIAL)
	{
	    যদি (buf[len] == NUL || buf[len + 1] == NUL)	    // cannot happen?
		প্রত্যাগম -1;
	    যদি (buf[len++] == (পূর্ণাঙ্ক)KS_ZERO)
		c = NUL;
	    // else it should be KS_SPECIAL; when followed by KE_FILLER c is
	    // K_SPECIAL, or followed by KE_CSI and c must be CSI.
	    যদি (buf[len++] == (পূর্ণাঙ্ক)KE_CSI)
		c = CSI;
	}
	অন্যথা যদি (c == CSI && buf[len] == KS_EXTRA
					       && buf[len + 1] == (পূর্ণাঙ্ক)KE_CSI)
	    // CSI is stored as CSI KS_SPECIAL KE_CSI to avoid confusion with
	    // the start of a special key, see add_to_input_buf_csi().
	    len += 2;
	bytes[i] = c;
    }
    প্রত্যাগম len;
}

/*
 * Check if the new shell size is valid, correct it if it's too small or way
 * too big.
 */
    শূন্য
check_shellsize(শূন্য)
{
    // need room for one window and command line
    যদি (Rows < min_rows_for_all_tabpages())
	Rows = min_rows_for_all_tabpages();
    limit_screen_size();

    // make sure these values are not invalid
    যদি (cmdline_row >= Rows)
	cmdline_row = Rows - 1;
    যদি (বার্তা_row >= Rows)
	বার্তা_row = Rows - 1;
}

/*
 * Limit Rows and Columns to avoid an overflow in Rows * Columns.
 */
    শূন্য
limit_screen_size(শূন্য)
{
    যদি (Columns < MIN_COLUMNS)
	Columns = MIN_COLUMNS;
    অন্যথা যদি (Columns > 10000)
	Columns = 10000;
    যদি (Rows > 1000)
	Rows = 1000;
}

/*
 * Invoked just before the screen structures are going to be (re)allocated.
 */
    শূন্য
win_new_shellsize(শূন্য)
{
    স্থির পূর্ণাঙ্ক	old_Rows = 0;
    স্থির পূর্ণাঙ্ক	old_Columns = 0;
    স্থির পূর্ণাঙ্ক	old_coloff = 0;

    যদি (old_Rows != Rows || old_Columns != COLUMNS_WITHOUT_TPL()
	    || old_coloff != TPL_LCOL())
	ui_new_shellsize();
    যদি (old_Columns != COLUMNS_WITHOUT_TPL() || old_coloff != TPL_LCOL())
    {
	old_Columns = COLUMNS_WITHOUT_TPL();
	old_coloff = TPL_LCOL();

	shell_new_columns();
    }
    যদি (old_Rows != Rows)
    {
	// If 'window' uses the whole screen, keep it using that.
	// Don't change it when set with "-w size" on the command line.
	যদি (p_window == old_Rows - 1
		    || (old_Rows == 0 && !option_was_set((char_u *)"window")))
	    p_window = Rows - 1;
	old_Rows = Rows;
	shell_new_rows();	// update window sizes
    }
}

#যদি defined(FEAT_IMAGE) || defined(FEAT_EVAL)
    শূন্য
update_cell_size(শূন্য)
{
# ifdef FEAT_GUI
    যদি (gui.in_use)
    {
	cell_width = gui.char_width;
	cell_height = gui.char_height;
    }
    অন্যথা
# endif
    {
	সংরচনা cellsize cell_sz;

	mch_calc_cell_size(&cell_sz);
	যদি (cell_sz.cs_xpixel <= 0 || cell_sz.cs_ypixel <= 0)
	{
	    cell_width = 8;
	    cell_height = 16;
	}
	অন্যথা
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
    শূন্য
shell_resized(শূন্য)
{
#যদি defined(FEAT_IMAGE) || defined(FEAT_EVAL)
    update_cell_size();
#endif
    set_shellsize(0, 0, FALSE);
}

/*
 * Check if the shell size changed.  Handle a resize.
 * When the size didn't change, nothing happens.
 */
    শূন্য
shell_resized_check(শূন্য)
{
    পূর্ণাঙ্ক		old_Rows = Rows;
    পূর্ণাঙ্ক		old_Columns = Columns;

    যদি (exiting
#ifdef FEAT_GUI
	    // Do not get the size when executing a shell command during
	    // startup.
	    || gui.starting
#endif
	    )
	প্রত্যাগম;

    (শূন্য)ui_get_shellsize();
    check_shellsize();
    যদি (old_Rows != Rows || old_Columns != Columns)
	shell_resized();
}

/*
 * Set size of the Vim shell.
 * If 'mustset' is TRUE, we must set Rows and Columns, do not get the real
 * window size (this is used for the :win command).
 * If 'mustset' is FALSE, we may try to get the real window size and if
 * it fails use 'width' and 'height'.
 */
    স্থির শূন্য
set_shellsize_inner(পূর্ণাঙ্ক width, পূর্ণাঙ্ক height, পূর্ণাঙ্ক mustset)
{
    যদি (updating_screen)
	// resizing while in update_screen() may cause a crash
	প্রত্যাগম;

    // curwin->w_buffer can be NULL when we are closing a window and the
    // buffer (or window) has already been closed and removing a scrollbar
    // causes a resize event. Don't resize then, it will happen after entering
    // another buffer.
    যদি (curwin->w_buffer == রিক্ত || curwin->w_সারিগুলি == রিক্ত)
	প্রত্যাগম;

#ifdef AMIGA
    out_flush();	    // must do this before mch_get_shellsize() for
			    // some obscure reason
#endif

    যদি (mustset || (ui_get_shellsize() == FAIL && height != 0))
    {
	Rows = height;
	Columns = width;
	check_shellsize();
	ui_set_shellsize(mustset);
    }
    অন্যথা
	check_shellsize();

    // The window layout used to be adjusted here, but it now happens in
    // screenalloc() (also invoked from screenclear()).  That is because the
    // "busy" check above may skip this, but not screenalloc().

    যদি (State != MODE_ASKMORE && State != MODE_EXTERNCMD
						      && State != MODE_CONFIRM)
	screenclear();
    অন্যথা
	screen_start();	    // don't know where cursor is now

    যদি (starting != NO_SCREEN)
    {
	maketitle();

	changed_সারি_abv_curs();
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
	যদি (State == MODE_ASKMORE || State == MODE_EXTERNCMD
				     || State == MODE_CONFIRM || exmode_active)
	{
	    screenalloc(FALSE);
	    repeat_message();
	}
	অন্যথা
	{
	    যদি (curwin->w_p_scb)
		do_check_scrollbind(TRUE);
	    যদি (State & MODE_CMDLINE)
	    {
		update_screen(UPD_NOT_VALID);
		redrawcmdline();
	    }
	    অন্যথা
	    {
		update_topline();
		যদি (pum_visible())
		{
		    redraw_later(UPD_NOT_VALID);
		    ins_compl_show_pum();
		}
		update_screen(UPD_NOT_VALID);
		যদি (redrawing())
		    setcursor();
	    }
	}
	cursor_on();	    // redrawing may have switched it off
    }
    out_flush();
}

    শূন্য
set_shellsize(পূর্ণাঙ্ক width, পূর্ণাঙ্ক height, পূর্ণাঙ্ক mustset)
{
    স্থির পূর্ণাঙ্ক	busy = FALSE;
    স্থির পূর্ণাঙ্ক	do_run = FALSE;

    যদি (width < 0 || height < 0)    // just checking...
	প্রত্যাগম;

    যদি (State == MODE_HITRETURN || State == MODE_SETWSIZE)
    {
	// postpone the resizing
	State = MODE_SETWSIZE;
	প্রত্যাগম;
    }

    // Avoid recursiveness.  This can happen when setting the window size
    // causes another window-changed signal or when two SIGWINCH signals come
    // very close together.  There needs to be another run then after the
    // current one is done to pick up the latest size.
    do_run = TRUE;
    যদি (busy)
	প্রত্যাগম;

    যাবত্ (do_run)
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
    শূন্য
out_str_t_TE(শূন্য)
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
    যদি (modify_otherkeys_state == MOKS_ENABLED
	    || modify_otherkeys_state == MOKS_DISABLED)
	modify_otherkeys_state = MOKS_DISABLED;
    অন্যথা যদি (modify_otherkeys_state != MOKS_INITIAL)
	modify_otherkeys_state = MOKS_AFTER_T_TE;

    // When the kitty keyboard protocol is enabled we expect t_TE to disable
    // it.  Remembering that it was detected to be enabled is useful in some
    // situations.
    // The following t_TI is expected to request the state and then
    // kitty_protocol_state will be set again.
    যদি (kitty_protocol_state == KKPS_ENABLED
	    || kitty_protocol_state == KKPS_DISABLED)
	kitty_protocol_state = KKPS_DISABLED;
    অন্যথা
	kitty_protocol_state = KKPS_AFTER_T_TE;
}

স্থির পূর্ণাঙ্ক send_t_RK = FALSE;

/*
 * Output T_TI and setup for what follows.
 */
    শূন্য
out_str_t_TI(শূন্য)
{
    out_str(T_CTI);

    // Send t_RK when there is no more work to do.
    send_t_RK = TRUE;
}

/*
 * Output T_BE, but only when t_PS and t_PE are set.
 */
    শূন্য
out_str_t_BE(শূন্য)
{
    char_u *p;

    যদি (T_BE == রিক্ত || *T_BE == NUL
	    || (p = find_termcode((char_u *)"PS")) == রিক্ত || *p == NUL
	    || (p = find_termcode((char_u *)"PE")) == রিক্ত || *p == NUL)
	প্রত্যাগম;
    out_str(T_BE);
}

/*
 * If t_TI was recently sent and there is no typeahead or work to do, now send
 * t_RK.  This is postponed to avoid the response arriving in a shell command
 * or after Vim exits.
 */
    শূন্য
may_send_t_RK(শূন্য)
{
    যদি (send_t_RK
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
    শূন্য
settmode(tmode_T tmode)
{
#ifdef FEAT_GUI
    // don't set the term where gvim was started to any mode
    যদি (gui.in_use)
	প্রত্যাগম;
#endif

    যদি (!full_screen)
	প্রত্যাগম;

    /*
     * When returning after calling a shell cur_tmode is TMODE_UNKNOWN,
     * set the terminal to raw mode, even though we think it already is,
     * because the shell program may have reset the terminal mode.
     * When we think the terminal is normal, don't try to set it to
     * normal again, because that causes problems (logout!) on some
     * machines.
     */
    যদি (tmode != cur_tmode)
    {
#ifdef FEAT_TERMRESPONSE
# ifdef FEAT_GUI
	যদি (!gui.in_use && !gui.starting)
# endif
	{
	    // May need to check for T_CRV response and termcodes, it
	    // doesn't work in Cooked mode, an external program may get
	    // them.
	    যদি (tmode != TMODE_RAW && termrequest_any_pending())
		(শূন্য)vpeekc_nomap();
	    check_for_codes_from_term();
	}
#endif
	যদি (tmode != TMODE_RAW)
	    mch_setmouse(FALSE);	// switch mouse off

	// Disable bracketed paste and modifyOtherKeys in cooked mode.
	// Avoid doing this too often, on some terminals the codes are not
	// handled properly.
	যদি (termcap_active && tmode != TMODE_SLEEP
		&& cur_tmode != TMODE_SLEEP)
	{
	    MAY_WANT_TO_LOG_THIS;

	    যদি (tmode != TMODE_RAW)
	    {
		out_str(T_BD);	// disable bracketed paste mode
		out_str_t_TE();	// possibly disables modifyOtherKeys
	    }
	    অন্যথা
	    {
		out_str_t_BE();	// enable bracketed paste mode (should
				// be before mch_settmode().
		out_str_t_TI();	// possibly enables modifyOtherKeys
	    }
	}
	out_flush();
	mch_settmode(tmode);	// machine specific function
	cur_tmode = tmode;
	যদি (tmode == TMODE_RAW)
	    setmouse();		// may switch mouse on
	out_flush();
    }
#ifdef FEAT_TERMRESPONSE
    may_req_termresponse();
#endif
}

    শূন্য
starttermcap(শূন্য)
{
    যদি (!full_screen || termcap_active)
	প্রত্যাগম;

    MAY_WANT_TO_LOG_THIS;

    out_str(T_TI);			// start termcap mode
    out_str_t_TI();			// start "raw" mode
    out_str(T_KS);			// start "keypad transmit" mode
    out_str_t_BE();			// enable bracketed paste mode

#যদি defined(UNIX) || defined(VMS)
    // Enable xterm's focus reporting mode when 'esckeys' is set.
    যদি (p_ek && *T_FE != NUL)
	out_str(T_FE);
#endif

    out_flush();
    termcap_active = TRUE;
    screen_start();			// don't know where cursor is now
#ifdef FEAT_TERMRESPONSE
# ifdef FEAT_GUI
    যদি (!gui.in_use && !gui.starting)
# endif
    {
	may_req_termresponse();
	// Immediately check for a response.  If t_Co changes, we don't
	// want to redraw with wrong colors first.
	যদি (crv_status.tr_progress == STATUS_SENT)
	    check_for_codes_from_term();
    }
#endif
}

    শূন্য
stoptermcap(শূন্য)
{
    screen_stop_highlight();
    reset_cterm_colors();

    যদি (!termcap_active)
	প্রত্যাগম;

#ifdef FEAT_TERMRESPONSE
# ifdef FEAT_GUI
    যদি (!gui.in_use && !gui.starting)
# endif
    {
	// May need to discard T_CRV, T_U7 or T_RBG response.
	যদি (termrequest_any_pending())
	{
# ifdef UNIX
	    // Give the terminal a chance to respond.
	    mch_delay(100L, 0);
# endif
# ifdef TCIFLUSH
	    // Discard data received but not read.
	    যদি (exiting)
		tcflush(fileno(stdin), TCIFLUSH);
# endif
	}
	// Check for termcodes first, otherwise an external program may
	// get them.
	check_for_codes_from_term();
    }
#endif
    MAY_WANT_TO_LOG_THIS;

#যদি defined(UNIX) || defined(VMS)
    // Disable xterm's focus reporting mode if 'esckeys' is set.
    যদি (p_ek && *T_FD != NUL)
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
    যদি (*T_TE != NUL && (kitty_protocol_state == KKPS_ENABLED
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

#যদি defined(FEAT_TERMRESPONSE)
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
    শূন্য
may_req_termresponse(শূন্য)
{
    যদি (crv_status.tr_progress == STATUS_GET
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
	(শূন্য)vpeekc_nomap();
    }
}

/*
 * Send sequences to the terminal and check with t_u7 how the cursor moves, to
 * find out properties of the terminal.
 * Note that this goes out before T_CRV, so that the result can be used when
 * the termresponse arrives.
 */
    শূন্য
check_terminal_behavior(শূন্য)
{
    পূর্ণাঙ্ক	    did_send = FALSE;

    যদি (!can_get_termresponse() || starting != 0 || *T_U7 == NUL)
	প্রত্যাগম;

    যদি (u7_status.tr_progress == STATUS_GET
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
	সারি_was_clobbered(1);
    }

    যদি (xcc_status.tr_progress == STATUS_GET && Rows > 2)
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
	সারি_was_clobbered(2);
    }

    যদি (did_send)
    {
	term_windgoto(0, 0);

	// Need to reset the known cursor position.
	screen_start();

	// check for the characters now, otherwise they might be eaten by
	// get_keystroke()
	out_flush();
	(শূন্য)vpeekc_nomap();
    }
}

/*
 * Similar to requesting the version string: Request the terminal background
 * color when it is the right moment.
 */
    শূন্য
may_req_bg_color(শূন্য)
{
    যদি (can_get_termresponse() && starting == 0)
    {
	পূর্ণাঙ্ক didit = FALSE;

# ifdef FEAT_TERMINAL
	// Only request foreground if t_RF is set.
	যদি (rfg_status.tr_progress == STATUS_GET && *T_RFG != NUL)
	{
	    MAY_WANT_TO_LOG_THIS;
	    LOG_TR1("Sending FG request");
	    out_str(T_RFG);
	    termrequest_sent(&rfg_status);
	    didit = TRUE;
	}
# endif

	// Only request background if t_RB is set.
	যদি (rbg_status.tr_progress == STATUS_GET && *T_RBG != NUL)
	{
	    MAY_WANT_TO_LOG_THIS;
	    LOG_TR1("Sending BG request");
	    out_str(T_RBG);
	    termrequest_sent(&rbg_status);
	    didit = TRUE;
	}

	যদি (didit)
	{
	    // check for the characters now, otherwise they might be eaten by
	    // get_keystroke()
	    out_flush();
	    (শূন্য)vpeekc_nomap();
	}
    }
}

#endif

/*
 * Return TRUE when saving and restoring the screen.
 */
    পূর্ণাঙ্ক
swapping_screen(শূন্য)
{
    প্রত্যাগম (full_screen && *T_TI != NUL);
}

/*
 * By outputting the 'cursor very visible' termcap code, for some windowed
 * terminals this makes the screen scrolled to the correct position.
 * Used when starting Vim or returning from a shell.
 */
    শূন্য
scroll_start(শূন্য)
{
    যদি (*T_VS == NUL || *T_CVS == NUL)
	প্রত্যাগম;

    MAY_WANT_TO_LOG_THIS;
    out_str(T_VS);
    out_str(T_CVS);
    screen_start();		// don't know where cursor is now
}

// True if cursor is not visible
স্থির পূর্ণাঙ্ক cursor_is_off = FALSE;

// True if cursor is not visible due to an ongoing cursor-less sleep
স্থির পূর্ণাঙ্ক cursor_is_asleep = FALSE;

/*
 * Enable the cursor without checking if it's already enabled.
 */
    শূন্য
cursor_on_force(শূন্য)
{
    out_str(T_VE);
    cursor_is_off = FALSE;
    cursor_is_asleep = FALSE;
}

/*
 * Enable the cursor if it's currently off.
 */
    শূন্য
cursor_on(শূন্য)
{
    যদি (cursor_is_off && !cursor_is_asleep)
	cursor_on_force();
}

/*
 * Disable the cursor.
 */
    শূন্য
cursor_off(শূন্য)
{
    যদি (full_screen && !cursor_is_off)
    {
	out_str(T_VI);	    // disable cursor
	cursor_is_off = TRUE;
    }
}

#ifdef FEAT_GUI
/*
 * Check whether the cursor is invisible due to an ongoing cursor-less sleep
 */
    পূর্ণাঙ্ক
cursor_is_sleeping(শূন্য)
{
    প্রত্যাগম cursor_is_asleep;
}
#endif

/*
 * Disable the cursor and mark it disabled by cursor-less sleep
 */
    শূন্য
cursor_sleep(শূন্য)
{
    cursor_is_asleep = TRUE;
    cursor_off();
}

/*
 * Enable the cursor and mark it not disabled by cursor-less sleep
 */
    শূন্য
cursor_unsleep(শূন্য)
{
    cursor_is_asleep = FALSE;
    cursor_on();
}

#যদি defined(CURSOR_SHAPE)
/*
 * Set cursor shape to match Insert or Replace mode.
 */
    শূন্য
term_cursor_mode(পূর্ণাঙ্ক forced)
{
    স্থির পূর্ণাঙ্ক showing_mode = -1;
    char_u *p;

    // Only do something when redrawing the screen and we can restore the
    // mode.
    যদি (!full_screen || *T_CEI == NUL)
    {
# ifdef FEAT_TERMRESPONSE
	যদি (forced && initial_cursor_shape > 0)
	    // Restore to initial values.
	    term_cursor_shape(initial_cursor_shape, initial_cursor_blink);
# endif
	প্রত্যাগম;
    }

    যদি ((State & MODE_REPLACE) == MODE_REPLACE)
    {
	যদি (forced || showing_mode != MODE_REPLACE)
	{
	    যদি (*T_CSR != NUL)
		p = T_CSR;	// Replace mode cursor
	    অন্যথা
		p = T_CSI;	// fall back to Insert mode cursor
	    যদি (*p != NUL)
	    {
		out_str(p);
		showing_mode = MODE_REPLACE;
	    }
	}
    }
    অন্যথা যদি (State & MODE_ঢোকাও)
    {
	যদি ((forced || showing_mode != MODE_ঢোকাও) && *T_CSI != NUL)
	{
	    out_str(T_CSI);	    // Insert mode cursor
	    showing_mode = MODE_ঢোকাও;
	}
    }
    অন্যথা যদি (forced || showing_mode != MODE_NORMAL)
    {
	out_str(T_CEI);		    // non-Insert mode cursor
	showing_mode = MODE_NORMAL;
    }
}

# যদি defined(FEAT_TERMINAL)
    শূন্য
term_cursor_color(char_u *color)
{
    যদি (*T_CSC == NUL)
	প্রত্যাগম;

    out_str(T_CSC);		// set cursor color start
    out_str_nf(color);
    out_str(T_CEC);		// set cursor color end
    out_flush();
}
# endif

    পূর্ণাঙ্ক
blink_state_is_inverted(শূন্য)
{
# ifdef FEAT_TERMRESPONSE
    প্রত্যাগম rbm_status.tr_progress == STATUS_GOT
	&& rcs_status.tr_progress == STATUS_GOT
		&& initial_cursor_blink != initial_cursor_shape_blink;
# অন্যথা
    প্রত্যাগম FALSE;
# endif
}

/*
 * "shape": 1 = block, 2 = underline, 3 = vertical bar
 */
    শূন্য
term_cursor_shape(পূর্ণাঙ্ক shape, পূর্ণাঙ্ক blink)
{
    যদি (*T_CSH != NUL)
    {
	OUT_STR(tgoto((বর্ণ *)T_CSH, 0, shape * 2 - blink));
	out_flush();
    }
    অন্যথা
    {
	পূর্ণাঙ্ক do_blink = blink;

	// t_SH is empty: try setting just the blink state.
	// The blink flags are XORed together, if the initial blinking from
	// style and shape differs, we need to invert the flag here.
	যদি (blink_state_is_inverted())
	    do_blink = !blink;

	যদি (do_blink && *T_VS != NUL)
	{
	    out_str(T_VS);
	    out_flush();
	}
	অন্যথা যদি (!do_blink && *T_CVS != NUL)
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
    শূন্য
scroll_region_set(win_T *wp, পূর্ণাঙ্ক off)
{
    OUT_STR(tgoto((বর্ণ *)T_CS, W_WINROW(wp) + wp->w_height - 1,
							 W_WINROW(wp) + off));
    যদি (*T_CSV != NUL && wp->w_width != Columns)
	OUT_STR(tgoto((বর্ণ *)T_CSV, wp->w_wincol + wp->w_width - 1,
							       wp->w_wincol));
    screen_start();		    // don't know where cursor is now
}

/*
 * Reset scrolling region to the whole screen.
 */
    শূন্য
scroll_region_reset(শূন্য)
{
    OUT_STR(tgoto((বর্ণ *)T_CS, (পূর্ণাঙ্ক)Rows - 1, 0));
    যদি (*T_CSV != NUL)
	OUT_STR(tgoto((বর্ণ *)T_CSV, (পূর্ণাঙ্ক)Columns - 1, 0));
    screen_start();		    // don't know where cursor is now
}


/*
 * List of terminal codes that are currently recognized.
 */

স্থির সংরচনা termcode
{
    char_u  name[2];	    // termcap name of entry
    char_u  *code;	    // terminal code (in allocated memory)
    পূর্ণাঙ্ক	    len;	    // STRLEN(code)
    পূর্ণাঙ্ক	    modlen;	    // length of part before ";*~".
} *termcodes = রিক্ত;

স্থির পূর্ণাঙ্ক  tc_max_len = 0; // number of entries that termcodes[] can hold
স্থির পূর্ণাঙ্ক  tc_len = 0;	    // current number of entries in termcodes[]

স্থির পূর্ণাঙ্ক termcode_star(char_u *code, পূর্ণাঙ্ক len);

    শূন্য
clear_termcodes(শূন্য)
{
    যাবত্ (tc_len > 0)
	vim_free(termcodes[--tc_len].code);
    VIM_CLEAR(termcodes);
    tc_max_len = 0;

#ifdef HAVE_TGETENT
    BC = (বর্ণ *)empty_option;
    UP = (বর্ণ *)empty_option;
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
    স্থির শূন্য
adjust_modlen(পূর্ণাঙ্ক idx)
{
    termcodes[idx].modlen = 0;
    পূর্ণাঙ্ক j = termcode_star(termcodes[idx].code, termcodes[idx].len);
    যদি (j <= 0)
	প্রত্যাগম;

    termcodes[idx].modlen = termcodes[idx].len - 1 - j;
    // For "CSI[@;X" the "@" is not included in "modlen".
    যদি (termcodes[idx].code[termcodes[idx].modlen - 1] == '@')
	--termcodes[idx].modlen;
}

/*
 * Add a new entry for "name[2]" to the list of terminal codes.
 * Note that "name" may not have a terminating NUL.
 * The list is kept alphabetical for ":set termcap"
 * "flags" is TRUE when replacing 7-bit by 8-bit controls is desired.
 * "flags" can also be ATC_FROM_TERM for got_code_from_term().
 */
    শূন্য
add_termcode(char_u *name, char_u *string, পূর্ণাঙ্ক flags)
{
    সংরচনা termcode *new_tc;
    পূর্ণাঙ্ক		    i, j;
    char_u	    *s;
    পূর্ণাঙ্ক		    len;
#ifdef FEAT_EVAL
    বর্ণ	    *action = "Setting";
#endif

    যদি (string == রিক্ত || *string == NUL)
    {
	del_termcode(name);
	প্রত্যাগম;
    }

#যদি defined(MSWIN) && !defined(FEAT_GUI)
    s = vim_strnsave(string, STRLEN(string) + 1);
#অন্যথা
# ifdef VIMDLL
    যদি (!gui.in_use)
	s = vim_strnsave(string, STRLEN(string) + 1);
    অন্যথা
# endif
	s = vim_strsave(string);
#endif
    যদি (s == রিক্ত)
	প্রত্যাগম;

    // Change leading <Esc>[ to CSI, change <Esc>O to <M-O>.
    যদি (flags != 0 && flags != ATC_FROM_TERM && term_7to8bit(string) != 0)
    {
	STRMOVE(s, s + 1);
	s[0] = term_7to8bit(string);
    }

#যদি defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
# ifdef VIMDLL
    যদি (!gui.in_use)
# endif
    {
	যদি (s[0] == K_NUL)
	{
	    STRMOVE(s + 1, s);
	    s[1] = 3;
	}
    }
#endif

    len = (পূর্ণাঙ্ক)STRLEN(s);

    need_gather = TRUE;		// need to fill termleader[]

    /*
     * need to make space for more entries
     */
    যদি (tc_len == tc_max_len)
    {
	tc_max_len += 20;
	new_tc = ALLOC_MULT(সংরচনা termcode, tc_max_len);
	যদি (new_tc == রিক্ত)
	{
	    tc_max_len -= 20;
	    vim_free(s);
	    প্রত্যাগম;
	}
	প্রতি (i = 0; i < tc_len; ++i)
	    new_tc[i] = termcodes[i];
	vim_free(termcodes);
	termcodes = new_tc;
    }

    /*
     * Look for existing entry with the same name, it is replaced.
     * Look for an existing entry that is alphabetical higher, the new entry
     * is inserted in front of it.
     */
    প্রতি (i = 0; i < tc_len; ++i)
    {
	যদি (termcodes[i].name[0] < name[0])
	    অনুবর্ত;
	যদি (termcodes[i].name[0] == name[0])
	{
	    যদি (termcodes[i].name[1] < name[1])
		অনুবর্ত;
	    /*
	     * Exact match: May replace old code.
	     */
	    যদি (termcodes[i].name[1] == name[1])
	    {
		যদি (flags == ATC_FROM_TERM && (j = termcode_star(
				    termcodes[i].code, termcodes[i].len)) > 0)
		{
		    // Don't replace ESC[123;*X or ESC O*X with another when
		    // invoked from got_code_from_term().
		    যদি (len == termcodes[i].len - j
			    && STRNCMP(s, termcodes[i].code, len - 1) == 0
			    && s[len - 1]
				   == termcodes[i].code[termcodes[i].len - 1])
		    {
			// They are equal but for the ";*": don't add it.
#ifdef FEAT_EVAL
			ch_log(রিক্ত, "Termcap entry %c%c did not change",
							     name[0], name[1]);
#endif
			vim_free(s);
			প্রত্যাগম;
		    }
		}
		অন্যথা
		{
		    // Replace old code.
#ifdef FEAT_EVAL
		    ch_log(রিক্ত, "Termcap entry %c%c was: %s",
					  name[0], name[1], termcodes[i].code);
#endif
		    vim_free(termcodes[i].code);
		    --tc_len;
		    ভঙ্গ;
		}
	    }
	}
	/*
	 * Found alphabetical larger entry, move rest to insert new entry
	 */
#ifdef FEAT_EVAL
	action = "Adding";
#endif
	প্রতি (j = tc_len; j > i; --j)
	    termcodes[j] = termcodes[j - 1];
	ভঙ্গ;
    }

#ifdef FEAT_EVAL
    ch_log(রিক্ত, "%s termcap entry %c%c to %s", action, name[0], name[1], s);
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
    স্থির শূন্য
accept_modifiers_for_function_চাবিগুলি(শূন্য)
{
    regmatch_T regmatch;
    CLEAR_FIELD(regmatch);
    regmatch.rm_ic = TRUE;
    regmatch.regprog = vim_regcomp((char_u *)"^\033[\\d\\+\\~$", RE_MAGIC);

    প্রতি (পূর্ণাঙ্ক i = 0; i < tc_len; ++i)
    {
	যদি (regmatch.regprog == রিক্ত)
	    প্রত্যাগম;

	// skip PasteStart and PasteEnd
	যদি (termcodes[i].name[0] == 'P'
		&& (termcodes[i].name[1] == 'S' || termcodes[i].name[1] == 'E'))
	    অনুবর্ত;

	char_u *s = termcodes[i].code;
	যদি (s != রিক্ত && vim_regexec(&regmatch, s, (colnr_T)0))
	{
	    size_t len = STRLEN(s);
	    char_u *ns = alloc(len + 3);
	    যদি (ns != রিক্ত)
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
    স্থির পূর্ণাঙ্ক
termcode_star(char_u *code, পূর্ণাঙ্ক len)
{
    // Shortest is <M-O>*X.  With ; shortest is <CSI>@;*X
    যদি (len >= 3 && code[len - 2] == '*')
    {
	যদি (len >= 5 && code[len - 3] == ';')
	    প্রত্যাগম 2;
	অন্যথা
	    প্রত্যাগম 1;
    }
    প্রত্যাগম 0;
}

    char_u  *
find_termcode(char_u *name)
{
    পূর্ণাঙ্ক	    i;

    প্রতি (i = 0; i < tc_len; ++i)
	যদি (termcodes[i].name[0] == name[0] && termcodes[i].name[1] == name[1])
	    প্রত্যাগম termcodes[i].code;
    প্রত্যাগম রিক্ত;
}

    char_u *
get_termcode(পূর্ণাঙ্ক i)
{
    যদি (i >= tc_len)
	প্রত্যাগম রিক্ত;
    প্রত্যাগম &termcodes[i].name[0];
}

/*
 * Returns the length of the terminal code at index 'idx'.
 */
    পূর্ণাঙ্ক
get_termcode_len(পূর্ণাঙ্ক idx)
{
    প্রত্যাগম termcodes[idx].len;
}

    শূন্য
del_termcode(char_u *name)
{
    পূর্ণাঙ্ক	    i;

    যদি (termcodes == রিক্ত)	// nothing there yet
	প্রত্যাগম;

    need_gather = TRUE;		// need to fill termleader[]

    প্রতি (i = 0; i < tc_len; ++i)
	যদি (termcodes[i].name[0] == name[0] && termcodes[i].name[1] == name[1])
	{
	    del_termcode_idx(i);
	    প্রত্যাগম;
	}
    // not found. Give error message?
}

    স্থির শূন্য
del_termcode_idx(পূর্ণাঙ্ক idx)
{
    পূর্ণাঙ্ক		i;

    vim_free(termcodes[idx].code);
    --tc_len;
    প্রতি (i = idx; i < tc_len; ++i)
	termcodes[i] = termcodes[i + 1];
}

/*
 * Called when detected that the terminal sends 8-bit codes.
 * Convert all 7-bit codes to their 8-bit equivalent.
 */
    স্থির শূন্য
switch_to_8bit(শূন্য)
{
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		c;

    // Only need to do something when not already using 8-bit codes.
    যদি (!term_is_8bit(T_NAME))
    {
	প্রতি (i = 0; i < tc_len; ++i)
	{
	    c = term_7to8bit(termcodes[i].code);
	    যদি (c != 0)
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
স্থির linenr_T orig_topline = 0;
# ifdef FEAT_DIFF
স্থির পূর্ণাঙ্ক orig_topfill = 0;
# endif
#endif
#যদি defined(CHECK_DOUBLE_CLICK)
/*
 * Checking for double-clicks ourselves.
 * "orig_topline" is used to avoid detecting a double-click when the window
 * contents scrolled (e.g., when 'scrolloff' is non-zero).
 */
/*
 * Set orig_topline.  Used when jumping to another window, so that a double
 * click still works.
 */
    শূন্য
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
    পূর্ণাঙ্ক
is_mouse_topline(win_T *wp)
{
    প্রত্যাগম orig_topline == wp->w_topline
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
    পূর্ণাঙ্ক
put_string_in_typebuf(
	পূর্ণাঙ্ক	offset,
	পূর্ণাঙ্ক	slen,
	char_u	*string,
	পূর্ণাঙ্ক	new_slen,
	char_u	*buf,
	পূর্ণাঙ্ক	bufsize,
	পূর্ণাঙ্ক	*buflen)
{
    পূর্ণাঙ্ক		extra = new_slen - slen;

    string[new_slen] = NUL;
    যদি (buf == রিক্ত)
    {
	যদি (extra < 0)
	    // remove matched chars, taking care of noremap
	    del_typebuf(-extra, offset);
	অন্যথা যদি (extra > 0)
	    // insert the extra space we need
	    যদি (ins_typebuf(string + slen, REMAP_YES, offset, FALSE, FALSE)
								       == FAIL)
		প্রত্যাগম FAIL;

	// Careful: del_typebuf() and ins_typebuf() may have reallocated
	// typebuf.tb_buf[]!
	mch_memmove(typebuf.tb_buf + typebuf.tb_off + offset, string,
							     (size_t)new_slen);
    }
    অন্যথা
    {
	যদি (extra < 0)
	    // remove matched characters
	    mch_memmove(buf + offset, buf + offset - extra,
					   (size_t)(*buflen + offset + extra));
	অন্যথা যদি (extra > 0)
	{
	    // Insert the extra space we need.  If there is insufficient
	    // space return -1.
	    যদি (*buflen + extra + new_slen >= bufsize)
		প্রত্যাগম FAIL;
	    mch_memmove(buf + offset + extra, buf + offset,
						   (size_t)(*buflen - offset));
	}
	mch_memmove(buf + offset, string, (size_t)new_slen);
	*buflen = *buflen + extra + new_slen;
    }
    প্রত্যাগম OK;
}

/*
 * Decode a modifier number as xterm provides it into MOD_MASK bits.
 */
    পূর্ণাঙ্ক
decode_modifiers(পূর্ণাঙ্ক n)
{
    পূর্ণাঙ্ক	    code = n - 1;
    পূর্ণাঙ্ক	    modifiers = 0;

    যদি (code & 1)
	modifiers |= MOD_MASK_SHIFT;
    যদি (code & 2)
	modifiers |= MOD_MASK_ALT;
    যদি (code & 4)
	modifiers |= MOD_MASK_CTRL;
    যদি (code & 8)
	modifiers |= MOD_MASK_META;
    // Any further modifiers are silently dropped.

    প্রত্যাগম modifiers;
}

    স্থির পূর্ণাঙ্ক
modifiers2keycode(পূর্ণাঙ্ক modifiers, পূর্ণাঙ্ক *চাবি, char_u *string)
{
    পূর্ণাঙ্ক new_slen = 0;

    যদি (modifiers == 0)
	প্রত্যাগম 0;

    // Some keys have the modifier included.  Need to handle that here to
    // make mappings work.  This may result in a special key, such as
    // K_S_TAB.
    *চাবি = simplify_চাবি(*চাবি, &modifiers);
    যদি (modifiers != 0)
    {
	string[new_slen++] = K_SPECIAL;
	string[new_slen++] = (পূর্ণাঙ্ক)KS_MODIFIER;
	string[new_slen++] = modifiers;
    }
    প্রত্যাগম new_slen;
}

/*
 * Handle a cursor position report.
 */
    স্থির শূন্য
handle_u7_response(পূর্ণাঙ্ক *arg, char_u *tp UNUSED, পূর্ণাঙ্ক csi_len UNUSED)
{
    যদি (arg[0] == 2 && arg[1] >= 2)
    {
	বর্ণ *aw = রিক্ত;

	LOG_TRN("Received U7 status: %s", tp);
	u7_status.tr_progress = STATUS_GOT;
	did_cursorhold = TRUE;
	যদি (arg[1] == 2)
	    aw = "single";
	অন্যথা যদি (arg[1] == 3)
	    aw = "double";
	যদি (aw != রিক্ত && STRCMP(aw, p_ambw) != 0)
	{
	    // Setting the option causes a screen redraw. Do
	    // that right away if possible, keeping any
	    // messages.
	    set_option_মান_give_err((char_u *)"ambw", 0L, (char_u *)aw, 0);
#ifdef DEBUG_TERMRESPONSE
	    {
		পূর্ণাঙ্ক r = redraw_asap(UPD_CLEAR);

		LOG_TRN("set 'ambiwidth', redraw_asap(): %d", r);
	    }
#অন্যথা
	    redraw_asap(UPD_CLEAR);
#endif
#ifdef FEAT_EVAL
	    set_vim_var_string(VV_TERMU7RESP, tp, csi_len);
#endif
	    apply_autocmds(EVENT_TERMRESPONSEALL,
					(char_u *)"ambiguouswidth", রিক্ত, FALSE, curbuf);
	}
    }
    অন্যথা যদি (arg[0] == 3)
    {
	পূর্ণাঙ্ক মান;

	LOG_TRN("Received compatibility test result: %s", tp);
	xcc_status.tr_progress = STATUS_GOT;

	// Third row: xterm compatibility test.
	// If the cursor is on the first column then the terminal can handle
	// the request for cursor style and blinking.
	মান = arg[1] == 1 ? TPR_YES : TPR_NO;
	term_props[TPR_CURSOR_STYLE].tpr_status = মান;
	term_props[TPR_CURSOR_BLINK].tpr_status = মান;
    }
}



/*
 * Handle a response to T_CRV: {lead}{first}{x};{vers};{y}c
 * Xterm and alike use '>' for {first}.
 * Rxvt sends "{lead}?1;2c".
 */
    স্থির শূন্য
handle_version_response(পূর্ণাঙ্ক first, পূর্ণাঙ্ক *arg, পূর্ণাঙ্ক argc, char_u *tp)
{
    // The xterm version.  It is set to zero when it can't be an actual xterm
    // version.
    পূর্ণাঙ্ক version = arg[1];

    LOG_TRN("Received CRV response: %s", tp);
    crv_status.tr_progress = STATUS_GOT;
    did_cursorhold = TRUE;

    // Reset terminal properties that are set based on the termresponse.
    // Mainly useful for tests that send the termresponse multiple times.
    // For testing all props can be reset.
    init_term_props(
#ifdef FEAT_EVAL
	    reset_term_props_on_termresponse
#অন্যথা
	    FALSE
#endif
	    );

    // If this code starts with CSI, you can bet that the
    // terminal uses 8-bit codes.
    যদি (tp[0] == CSI)
	switch_to_8bit();

    // Screen sends 40500.
    // rxvt sends its version number: "20703" is 2.7.3.
    // Ignore it for when the user has set 'term' to xterm,
    // even though it's an rxvt.
    যদি (version > 20000)
	version = 0;

    // Figure out more if the response is CSI > 99 ; 99 ; 99 c
    যদি (first == '>' && argc == 3)
    {
	// mintty 2.9.5 sends 77;20905;0c.
	// (77 is ASCII 'M' for mintty.)
	যদি (arg[0] == 77)
	{
	    // mintty can do SGR mouse reporting
	    term_props[TPR_MOUSE].tpr_status = TPR_MOUSE_SGR;
	}

#ifdef FEAT_TERMRESPONSE
	// If xterm version >= 141 try to get termcap codes.  For other
	// terminals the request should be ignored.
	যদি (version >= 141 && p_xtermcodes)
	{
	    LOG_TR1("Enable checking for XT codes");
	    check_for_codes = TRUE;
	    need_gather = TRUE;
	    req_codes_from_term();
	}
#endif

	// libvterm sends 0;100;0
	// Konsole sends 0;115;0 and works the same way
	যদি ((version == 100 || version == 115) && arg[0] == 0 && arg[2] == 0)
	{
	    // If run from Vim $COLORS is set to the number of
	    // colors the terminal supports.  Otherwise assume
	    // 256, libvterm supports even more.
	    যদি (mch_getenv((char_u *)"COLORS") == রিক্ত)
		may_adjust_color_গুনতি(256);
	    // Libvterm can handle SGR mouse reporting.
	    term_props[TPR_MOUSE].tpr_status = TPR_MOUSE_SGR;
	}

	যদি (version == 95)
	{
	    // Mac Terminal.app sends 1;95;0
	    //
	    // Terminal.app doesn't seem to handle DECRQM sequences
	    // properly, see issue #19852.
	    যদি (arg[0] == 1 && arg[2] == 0)
	    {
		term_props[TPR_UNDERLINE_RGB].tpr_status = TPR_YES;
		term_props[TPR_MOUSE].tpr_status = TPR_MOUSE_SGR;
		term_props[TPR_DECRQM].tpr_status = TPR_NO;
	    }
	    // iTerm2 sends 0;95;0
	    অন্যথা যদি (arg[0] == 0 && arg[2] == 0)
	    {
		// iTerm2 can do SGR mouse reporting
		term_props[TPR_MOUSE].tpr_status = TPR_MOUSE_SGR;
		term_props[TPR_DECRQM].tpr_status = TPR_YES;
	    }
	    // old iTerm2 sends 0;95;
	    অন্যথা যদি (arg[0] == 0 && arg[2] == -1)
		term_props[TPR_UNDERLINE_RGB].tpr_status = TPR_YES;
	}

	// screen sends 83;40500;0 83 is 'S' in ASCII.
	যদি (arg[0] == 83)
	{
	    // screen supports SGR mouse codes since 4.7.0
	    যদি (arg[1] >= 40700)
		term_props[TPR_MOUSE].tpr_status = TPR_MOUSE_SGR;
	    অন্যথা
		term_props[TPR_MOUSE].tpr_status = TPR_MOUSE_XTERM;
	}

	// If no recognized terminal has set mouse behavior, assume xterm.
	যদি (term_props[TPR_MOUSE].tpr_status == TPR_UNKNOWN)
	{
	    // Xterm version 277 supports SGR.
	    // Xterm version >= 95 supports mouse dragging.
	    যদি (version >= 277)
		term_props[TPR_MOUSE].tpr_status = TPR_MOUSE_SGR;
	    অন্যথা যদি (version >= 95)
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
	যদি (arg[1] >= 2500)
	{
	    term_props[TPR_UNDERLINE_RGB].tpr_status = TPR_YES;
	}

	অন্যথা যদি (version == 136 && arg[2] == 0)
	{
	    term_props[TPR_UNDERLINE_RGB].tpr_status = TPR_YES;

	    // PuTTY sends 0;136;0
	    যদি (arg[0] == 0)
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
	যদি (arg[0] == 1 && arg[1] >= 4000 && arg[1] <= 4009)
	{
	    term_props[TPR_KITTY].tpr_status = TPR_YES;
	    term_props[TPR_KITTY].tpr_set_by_termresponse = TRUE;

	    // Kitty can handle SGR mouse reporting.
	    term_props[TPR_MOUSE].tpr_status = TPR_MOUSE_SGR;
	    term_props[TPR_DECRQM].tpr_status = TPR_YES;
	}

	// foot terminal sends 1;12700;0
	যদি (arg[0] == 1 && version == 12700 && arg[2] == 0)
	{
	    term_props[TPR_MOUSE].tpr_status = TPR_MOUSE_SGR;
	    term_props[TPR_DECRQM].tpr_status = TPR_YES;
	}

	// GNU screen sends 83;30600;0, 83;40500;0, etc.
	// 30600/40500 is a version number of GNU screen. DA2 support is added
	// on 3.6.  DCS string has a special meaning to GNU screen, but xterm
	// compatibility checking does not detect GNU screen.
	যদি (arg[0] == 83 && arg[1] >= 30600)
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
	যদি (version < 95)
	    term_props[TPR_UNDERLINE_RGB].tpr_status = TPR_YES;

	// Getting the cursor style is only supported properly by xterm since
	// version 279 (otherwise it returns 0x18).
	যদি (version < 279)
	    term_props[TPR_CURSOR_STYLE].tpr_status = TPR_NO;

	/*
	 * Take action on the detected properties.
	 */

	// Unless the underline RGB color is expected to work, disable "t_8u".
	// It does not work for the real Xterm, it resets the background color.
	// This may cause some flicker.  Alternative would be to set "t_8u"
	// here if the terminal is expected to support it, but that might
	// conflict with what was set in the .vimrc.
	যদি (term_props[TPR_UNDERLINE_RGB].tpr_status != TPR_YES
			&& *T_8U != NUL
			&& !option_was_set((char_u *)"t_8u"))
	{
	    set_string_option_direct((char_u *)"t_8u", -1, (char_u *)"",
								  OPT_FREE, 0);
	}
#ifdef FEAT_TERMRESPONSE
	যদি (*T_8U != NUL && write_t_8u_state == MAYBE)
	    // Did skip writing t_8u, a complete redraw is needed.
	    redraw_later_clear();
	write_t_8u_state = OK;  // can output t_8u now
#endif

	// Only set 'ttymouse' automatically if it was not set
	// by the user already.
	যদি (!option_was_set((char_u *)"ttym")
		&& (term_props[TPR_MOUSE].tpr_status == TPR_MOUSE_XTERM2
		    || term_props[TPR_MOUSE].tpr_status == TPR_MOUSE_SGR))
	{
	    set_option_মান_give_err((char_u *)"ttym", 0L,
		    term_props[TPR_MOUSE].tpr_status == TPR_MOUSE_SGR
				    ? (char_u *)"sgr" : (char_u *)"xterm2", 0);
	}

#ifdef FEAT_TERMRESPONSE
	পূর্ণাঙ্ক need_flush = FALSE;

	// Only request the cursor style if t_SH and t_RS are
	// set. Only supported properly by xterm since version
	// 279 (otherwise it returns 0x18).
	// Only when getting the cursor style was detected to work.
	// Not for Terminal.app, it can't handle t_RS, it
	// echoes the characters to the screen.
	যদি (rcs_status.tr_progress == STATUS_GET
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
	যদি (rbm_status.tr_progress == STATUS_GET
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
	যদি (decrqm_status.tr_progress == STATUS_GET
		&& term_props[TPR_DECRQM].tpr_status == TPR_YES)
	{
	    MAY_WANT_TO_LOG_THIS;
	    LOG_TR1("Sending DECRQM requests");
	    প্রতি (পূর্ণাঙ্ক i = 0; i < (পূর্ণাঙ্ক)ARRAY_LENGTH(dec_modes); i++)
	    {
		vim_snprintf((বর্ণ *)IObuff, IOSIZE, "\033[?%d$p", dec_modes[i]);
		out_str(IObuff);
	    }
	    termrequest_sent(&decrqm_status);
	    need_flush = TRUE;

	}

	যদি (need_flush)
	    out_flush();
#endif
    }
}

/*
 * Add "key" to "buf" and return the number of bytes used.
 * Handles special keys and multi-byte characters.
 */
    স্থির পূর্ণাঙ্ক
add_চাবি_to_buf(পূর্ণাঙ্ক চাবি, char_u *buf)
{
    পূর্ণাঙ্ক idx = 0;

    যদি (IS_SPECIAL(চাবি))
    {
	buf[idx++] = K_SPECIAL;
	buf[idx++] = KEY2TERMCAP0(চাবি);
	buf[idx++] = KEY2TERMCAP1(চাবি);
    }
    অন্যথা যদি (has_mbyte)
	idx += (*mb_char2bytes)(চাবি, buf + idx);
    অন্যথা
	buf[idx++] = চাবি;
    প্রত্যাগম idx;
}

/*
 * Shared between handle_key_with_modifier() and handle_csi_function_key().
 */
    স্থির পূর্ণাঙ্ক
put_চাবি_modifiers_in_typebuf(
	পূর্ণাঙ্ক	চাবি_arg,
	পূর্ণাঙ্ক	modifiers_arg,
	পূর্ণাঙ্ক	csi_len,
	পূর্ণাঙ্ক	offset,
	char_u	*buf,
	পূর্ণাঙ্ক	bufsize,
	পূর্ণাঙ্ক	*buflen)
{
    পূর্ণাঙ্ক চাবি = চাবি_arg;
    পূর্ণাঙ্ক modifiers = modifiers_arg;

    // Some keys need adjustment when the Ctrl modifier is used.
    চাবি = may_adjust_চাবি_for_ctrl(modifiers, চাবি);

    // May remove the shift modifier if it's already included in the key.
    modifiers = may_remove_shift_modifier(modifiers, চাবি);

    // Produce modifiers with K_SPECIAL KS_MODIFIER {mod}
    // worst-case: 3-byte modifier + 4 byte multi-char key + NUL
    char_u string[MAX_চাবি_CODE_LEN + 2];
    পূর্ণাঙ্ক new_slen = modifiers2keycode(modifiers, &চাবি, string);

    // reject overlong key that would overflow string
    যদি (চাবি > 0x10FFFF)
	প্রত্যাগম -1;

    // Add the bytes for the key.
    new_slen += add_চাবি_to_buf(চাবি, string + new_slen);

    string[new_slen] = NUL;
    যদি (put_string_in_typebuf(offset, csi_len, string, new_slen,
						 buf, bufsize, buflen) == FAIL)
	প্রত্যাগম -1;
    প্রত্যাগম new_slen - csi_len + offset;
}

/*
 * Parse the number from a CSI numbered sequence for an F1-F12 key:
 *	ESC [ {number} ~
 * Returns the key
 */
    স্থির পূর্ণাঙ্ক
parse_csi_f_চাবিগুলি(পূর্ণাঙ্ক arg)
{
    char_u চাবি_name[2] = { 0, 0 };

    বিকল্প (arg)
    {
	পক্ষ 11: চাবি_name[0] = 'k'; চাবি_name[1] = '1'; ভঙ্গ;  // K_F1
	পক্ষ 12: চাবি_name[0] = 'k'; চাবি_name[1] = '2'; ভঙ্গ;  // K_F2
	পক্ষ 13: চাবি_name[0] = 'k'; চাবি_name[1] = '3'; ভঙ্গ;  // K_F3
	পক্ষ 14: চাবি_name[0] = 'k'; চাবি_name[1] = '4'; ভঙ্গ;  // K_F4
	পক্ষ 15: চাবি_name[0] = 'k'; চাবি_name[1] = '5'; ভঙ্গ;  // K_F5
	পক্ষ 17: চাবি_name[0] = 'k'; চাবি_name[1] = '6'; ভঙ্গ;  // K_F6
	পক্ষ 18: চাবি_name[0] = 'k'; চাবি_name[1] = '7'; ভঙ্গ;  // K_F7
	পক্ষ 19: চাবি_name[0] = 'k'; চাবি_name[1] = '8'; ভঙ্গ;  // K_F8
	পক্ষ 20: চাবি_name[0] = 'k'; চাবি_name[1] = '9'; ভঙ্গ;  // K_F9
	পক্ষ 21: চাবি_name[0] = 'F'; চাবি_name[1] = ';'; ভঙ্গ;  // K_F10
	পক্ষ 23: চাবি_name[0] = 'F'; চাবি_name[1] = '1'; ভঙ্গ;  // K_F11
	পক্ষ 24: চাবি_name[0] = 'F'; চাবি_name[1] = '2'; ভঙ্গ;  // K_F12
    }
    যদি (চাবি_name[0])
	প্রত্যাগম TERMCAP2_চাবি(চাবি_name[0], চাবি_name[1]);
    // shouldn't happen
    প্রত্যাগম arg;
}

/*
 * Handle a sequence with key and modifier, one of:
 *	{lead}27;{modifier};{key}~
 *	{lead}{key};{modifier}u
 * Returns the difference in length.
 */
    স্থির পূর্ণাঙ্ক
handle_চাবি_with_modifier(
	পূর্ণাঙ্ক	*arg,
	পূর্ণাঙ্ক	csi_len,
	পূর্ণাঙ্ক	offset,
	char_u	*buf,
	পূর্ণাঙ্ক	bufsize,
	পূর্ণাঙ্ক	*buflen,
	পূর্ণাঙ্ক	iskitty,
	পূর্ণাঙ্ক	trail)
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
    যদি (!iskitty
	    && (kitty_protocol_state == KKPS_INITIAL
		|| kitty_protocol_state == KKPS_OFF
		|| kitty_protocol_state == KKPS_AFTER_T_TE)
	    && term_props[TPR_KITTY].tpr_status != TPR_YES)
    {
#ifdef FEAT_EVAL
	ch_log(রিক্ত, "setting seenModifyOtherKeys to TRUE");
#endif
	seen_Modify_Other_চাবিগুলি = TRUE;
    }

    পূর্ণাঙ্ক চাবি = iskitty ? arg[0] : arg[2];
    পূর্ণাঙ্ক modifiers = decode_modifiers(arg[1]);

    // Some terminals do not apply the Shift modifier to the key.  To make
    // mappings consistent we do it here.  TODO: support more keys.
    যদি ((modifiers & MOD_MASK_SHIFT) && চাবি >= 'a' && চাবি <= 'z')
	চাবি += 'A' - 'a';

    // Putting Esc in the buffer creates ambiguity, it can be the start of an
    // escape sequence.  Use K_ESC to avoid that.
    যদি (চাবি == ESC)
	চাবি = K_ESC;

    অন্যথা যদি (arg[0] >= 11 && arg[0] <= 24 && trail == '~')
	চাবি = parse_csi_f_চাবিগুলি(arg[0]);

    প্রত্যাগম put_চাবি_modifiers_in_typebuf(চাবি, modifiers, csi_len, offset, buf,
	    bufsize, buflen);
}

/*
 * Handle a sequence with key without a modifier:
 *	{lead}{key}u
 *	{lead}{key}~
 * Returns the difference in length.
 */
    স্থির পূর্ণাঙ্ক
handle_চাবি_without_modifier(
	পূর্ণাঙ্ক	*arg,
	পূর্ণাঙ্ক	csi_len,
	পূর্ণাঙ্ক	offset,
	char_u	*buf,
	পূর্ণাঙ্ক	bufsize,
	পূর্ণাঙ্ক	*buflen,
	পূর্ণাঙ্ক	trail)
{
    char_u  string[MAX_চাবি_CODE_LEN + 1];
    পূর্ণাঙ্ক	    new_slen;

    যদি (arg[0] == ESC)
    {
	// Putting Esc in the buffer creates ambiguity, it can be the start of
	// an escape sequence.  Use K_ESC to avoid that.
	string[0] = K_SPECIAL;
	string[1] = KS_EXTRA;
	string[2] = KE_ESC;
	new_slen = 3;
    }
    অন্যথা যদি (arg[0] >= 11 && arg[0] <= 24 && trail == '~')
    {
	পূর্ণাঙ্ক চাবি = parse_csi_f_চাবিগুলি(arg[0]);
	string[0] = K_SPECIAL;
	string[1] = KEY2TERMCAP0(চাবি);
	string[2] = KEY2TERMCAP1(চাবি);
	new_slen = 3;
    }
    অন্যথা
	new_slen = add_চাবি_to_buf(arg[0], string);

    যদি (put_string_in_typebuf(offset, csi_len, string, new_slen,
						 buf, bufsize, buflen) == FAIL)
	প্রত্যাগম -1;
    প্রত্যাগম new_slen - csi_len + offset;
}

/*
 * CSI function key without or with modifiers:
 *	{lead}[ABCDEFHPQRS]
 *	{lead}1;{modifier}[ABCDEFHPQRS]
 * Returns zero when nog recognized, a positive number when recognized.
 */
    স্থির পূর্ণাঙ্ক
handle_csi_function_চাবি(
	পূর্ণাঙ্ক	argc,
	পূর্ণাঙ্ক	*arg,
	পূর্ণাঙ্ক	trail,
	পূর্ণাঙ্ক	csi_len,
	char_u	*চাবি_name,
	পূর্ণাঙ্ক	offset,
	char_u  *buf,
	পূর্ণাঙ্ক	bufsize,
	পূর্ণাঙ্ক	*buflen)
{
    চাবি_name[0] = 'k';
    বিকল্প (trail)
    {
	পক্ষ 'A': চাবি_name[1] = 'u'; ভঙ্গ;  // K_UP
	পক্ষ 'B': চাবি_name[1] = 'd'; ভঙ্গ;  // K_DOWN
	পক্ষ 'C': চাবি_name[1] = 'r'; ভঙ্গ;  // K_RIGHT
	পক্ষ 'D': চাবি_name[1] = 'l'; ভঙ্গ;  // K_LEFT

	// case 'E': keypad BEGIN - not supported
	পক্ষ 'F': চাবি_name[0] = '@'; চাবি_name[1] = '7'; ভঙ্গ;  // K_END
	পক্ষ 'H': চাবি_name[1] = 'h'; ভঙ্গ;  // K_HOME

	পক্ষ 'P': চাবি_name[1] = '1'; ভঙ্গ;  // K_F1
	পক্ষ 'Q': চাবি_name[1] = '2'; ভঙ্গ;  // K_F2
	পক্ষ 'R': চাবি_name[1] = '3'; ভঙ্গ;  // K_F3
	পক্ষ 'S': চাবি_name[1] = '4'; ভঙ্গ;  // K_F4

	স্বভাব: প্রত্যাগম 0;  // not recognized
    }

    পূর্ণাঙ্ক চাবি = TERMCAP2_চাবি(চাবি_name[0], চাবি_name[1]);
    পূর্ণাঙ্ক modifiers = argc == 2 ? decode_modifiers(arg[1]) : 0;
    put_চাবি_modifiers_in_typebuf(চাবি, modifiers,
					csi_len, offset, buf, bufsize, buflen);
    প্রত্যাগম csi_len;
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
    স্থির পূর্ণাঙ্ক
handle_csi(
	char_u	*tp,
	পূর্ণাঙ্ক	len,
	char_u	*argp,
	পূর্ণাঙ্ক	offset,
	char_u  *buf,
	পূর্ণাঙ্ক	bufsize,
	পূর্ণাঙ্ক	*buflen,
	char_u	*চাবি_name,
	পূর্ণাঙ্ক	*slen)
{
    পূর্ণাঙ্ক		first = -1;  // optional char right after {lead}
    পূর্ণাঙ্ক		trail;	     // char that ends CSI sequence
    পূর্ণাঙ্ক		arg[3] = {-1, -1, -1};	// argument numbers
    পূর্ণাঙ্ক		argc = 0;		// number of arguments
    char_u	*ap = argp;
    পূর্ণাঙ্ক		csi_len;

    // Check for non-digit after CSI.
    যদি (!VIM_ISDIGIT(*ap))
	first = *ap++;

    যদি (first >= 'A' && first <= 'Z')
    {
	// If "first" is in [ABCDEFHPQRS] then it is actually the "trail" and
	// no argument follows.
	trail = first;
	first = -1;
	--ap;
    }
    অন্যথা
    {
	// Find up to three argument numbers.
	প্রতি (argc = 0; argc < 3; )
	{
	    যদি (ap >= tp + len)
		প্রত্যাগম -1;
	    যদি (*ap == ';')
		arg[argc++] = -1;  // omitted number
	    অন্যথা যদি (VIM_ISDIGIT(*ap))
	    {
		arg[argc] = 0;
		প্রতি (;;)
		{
		    যদি (ap >= tp + len)
			প্রত্যাগম -1;
		    যদি (!VIM_ISDIGIT(*ap))
			ভঙ্গ;
		    // avoid overflow
		    যদি (arg[argc] <= (INT_MAX - 9) / 10)
			arg[argc] = arg[argc] * 10 + (*ap - '0');
		    ++ap;
		}
		++argc;
	    }
	    যদি (*ap == ';')
		++ap;
	    অন্যথা
		ভঙ্গ;
	}

	// mrxvt has been reported to have "+" in the version. Assume
	// the escape sequence ends with a letter or one of "{|}~".
	যাবত্ (ap < tp + len
		&& !(*ap >= '{' && *ap <= '~')
		&& !ASCII_ISALPHA(*ap))
	    ++ap;
	যদি (ap >= tp + len)
	    প্রত্যাগম -1;
	trail = *ap;
    }

    csi_len = (পূর্ণাঙ্ক)(ap - tp) + 1;

    // Response to XTQMODKEYS: "CSI > 4 ; Pv m" where Pv indicates the
    // modifyOtherKeys level.  Drop similar responses.
    যদি (first == '>' && (argc == 1 || argc == 2) && trail == 'm')
    {
	যদি (arg[0] == 4 && argc == 2)
	    modify_otherkeys_state = arg[1] == 2 ? MOKS_ENABLED : MOKS_OFF;

	চাবি_name[0] = (পূর্ণাঙ্ক)KS_EXTRA;
	চাবি_name[1] = (পূর্ণাঙ্ক)KE_IGNORE;
	*slen = csi_len;
    }

    // Function key starting with CSI:
    //	{lead}[ABCDEFHPQRS]
    //	{lead}1;{modifier}[ABCDEFHPQRS]
    অন্যথা যদি (first == -1 && ASCII_ISUPPER(trail)
	    && (argc == 0 || (argc == 2 && arg[0] == 1)))
    {
	পূর্ণাঙ্ক res = handle_csi_function_চাবি(argc, arg, trail,
			      csi_len, চাবি_name, offset, buf, bufsize, buflen);
	প্রত্যাগম res <= 0 ? res : len + res;
    }

    // Cursor position report: {lead}{row};{col}R
    // Eat it when there are 2 arguments and it ends in 'R'.
    // Also when u7_status is not "sent", it may be from a previous Vim that
    // just exited.  But not for <S-F3>, it sends something similar, check for
    // row and column to make sense.
    অন্যথা যদি (first == -1 && argc == 2 && trail == 'R')
    {
	handle_u7_response(arg, tp, csi_len);

	চাবি_name[0] = (পূর্ণাঙ্ক)KS_EXTRA;
	চাবি_name[1] = (পূর্ণাঙ্ক)KE_IGNORE;
	*slen = csi_len;
    }

#যদি (defined(UNIX) || defined(MSWIN)) \
    && (defined(FEAT_IMAGE) || defined(FEAT_EVAL))
    // Response to CSI 14 t or CSI 16 t
    অন্যথা যদি (first == -1 && argc >= 3
	    && (arg[0] == 4 || arg[0] == 6) && trail == 't')
    {
	যদি (arg[0] == 4)
	{
	    LOG_TRN("Received CSI 14 t response: %s", tp);
	    cell_width = arg[2] / Columns;
	    cell_height = arg[1] / Rows;
	}
	অন্যথা
	{
	    LOG_TRN("Received CSI 16 t response: %s", tp);
	    cell_width = arg[2];
	    cell_height = arg[1];
	}

	চাবি_name[0] = (পূর্ণাঙ্ক)KS_EXTRA;
	চাবি_name[1] = (পূর্ণাঙ্ক)KE_IGNORE;
	*slen = csi_len;
    }
#endif

    // Primary device attributes (DA1) response
    অন্যথা যদি (first == '?' && trail == 'c')
    {
	LOG_TRN("Received DA1 response: %s", tp);

	*slen = csi_len;
#ifdef FEAT_EVAL
	set_vim_var_string(VV_TERMDA1, tp, *slen);
#endif
	apply_autocmds(EVENT_TERMRESPONSEALL,
					(char_u *)"da1", রিক্ত, FALSE, curbuf);

	চাবি_name[0] = (পূর্ণাঙ্ক)KS_EXTRA;
	চাবি_name[1] = (পূর্ণাঙ্ক)KE_IGNORE;
	*slen = csi_len;
    }

    // DECRPM mode 2026 or 2048.
    অন্যথা যদি (first == '?' && trail == 'y' && argc == 2
	    && (arg[0] == 2026 || arg[0] == 2048))
    {
	পূর্ণাঙ্ক setting = arg[1];

	*slen = csi_len;
	চাবি_name[0] = (পূর্ণাঙ্ক)KS_EXTRA;
	চাবি_name[1] = (পূর্ণাঙ্ক)KE_IGNORE;

#ifdef FEAT_TERMRESPONSE
	// Mark the DECRQM request as answered so it is not sent again and
	// stoptermcap() does not wait for it.
	যদি (decrqm_status.tr_progress == STATUS_SENT)
	    decrqm_status.tr_progress = STATUS_GOT;
#endif

	যদি (setting >= 0 && setting <= 4)
	{
	    LOG_TRN("Received DECRPM mode %d: %s", arg[0], tp);

	    বিকল্প (arg[0])
	    {
		পক্ষ 2026:
		    sync_output_setting = setting;
		    set_option_মান_give_err((char_u *)"termsync",
			    setting == 1 || setting == 2, রিক্ত, 0);
		    ভঙ্গ;
#ifdef UNIX
		পক্ষ 2048:
		    win_resize_setting = setting;

		    term_set_win_resize(true);
		    ভঙ্গ;
#endif
	    }
	}
	অন্যথা
	    LOG_TRN("Unknown DECRPM mode %d setting %d", arg[0], setting);
    }

#ifdef UNIX
    // In-band window resize event
    অন্যথা যদি (win_resize_enabled && argc >= 3 && arg[0] == 48)
    {
	পূর্ণাঙ্ক height = arg[1], width = arg[2];

	*slen = csi_len;
	চাবি_name[0] = (পূর্ণাঙ্ক)KS_EXTRA;
	চাবি_name[1] = (পূর্ণাঙ্ক)KE_IGNORE;

	// Only update if needed. Avoids intro message from disappearing on
	// startup (due to initial event).
	যদি (height != Rows || width != Columns)
	    set_shellsize(width, height, true);
    }
#endif

    // Version string: Eat it when there is at least one digit and
    // it ends in 'c'
    অন্যথা যদি (*T_CRV != NUL && ap > argp + 1 && trail == 'c')
    {
	handle_version_response(first, arg, argc, tp);

	*slen = csi_len;
#ifdef FEAT_EVAL
	set_vim_var_string(VV_TERMRESPONSE, tp, *slen);
#endif
	apply_autocmds(EVENT_TERMRESPONSE,
					রিক্ত, রিক্ত, FALSE, curbuf);
	apply_autocmds(EVENT_TERMRESPONSEALL,
					(char_u *)"version", রিক্ত, FALSE, curbuf);
	চাবি_name[0] = (পূর্ণাঙ্ক)KS_EXTRA;
	চাবি_name[1] = (পূর্ণাঙ্ক)KE_IGNORE;
    }

#ifdef FEAT_TERMRESPONSE
    // Check blinking cursor from xterm:
    // {lead}?12;1$y       set
    // {lead}?12;2$y       not set
    //
    // {lead} can be <Esc>[ or CSI
    অন্যথা যদি (rbm_status.tr_progress == STATUS_SENT
	    && first == '?'
	    && ap == argp + 6
	    && arg[0] == 12
	    && ap[-1] == '$'
	    && trail == 'y')
    {
	initial_cursor_blink = (arg[1] == '1');
	rbm_status.tr_progress = STATUS_GOT;
	LOG_TRN("Received cursor blinking mode response: %s", tp);
	চাবি_name[0] = (পূর্ণাঙ্ক)KS_EXTRA;
	চাবি_name[1] = (পূর্ণাঙ্ক)KE_IGNORE;
	*slen = csi_len;
# ifdef FEAT_EVAL
	set_vim_var_string(VV_TERMBLINKRESP, tp, *slen);
# endif
	apply_autocmds(EVENT_TERMRESPONSEALL,
					(char_u *)"cursorblink", রিক্ত, FALSE, curbuf);
    }
#endif

    // Kitty keyboard protocol status response: CSI ? flags u
    অন্যথা যদি (first == '?' && argc == 1 && trail == 'u')
    {
	// The protocol has various "progressive enhancement flags" values, but
	// we only check for zero and non-zero here.
	যদি (arg[0] == '0')
	{
	    kitty_protocol_state = KKPS_OFF;
	}
	অন্যথা
	{
	    kitty_protocol_state = KKPS_ENABLED;

	    // Reset seenModifyOtherKeys just in case some key combination has
	    // been seen that set it before we get the status response.
#ifdef FEAT_EVAL
	    ch_log(রিক্ত, "setting seenModifyOtherKeys to FALSE");
#endif
	    seen_Modify_Other_চাবিগুলি = FALSE;
	}

	চাবি_name[0] = (পূর্ণাঙ্ক)KS_EXTRA;
	চাবি_name[1] = (পূর্ণাঙ্ক)KE_IGNORE;
	*slen = csi_len;
    }

#ifdef FEAT_TERMRESPONSE
    // Check for a window position response from the terminal:
    //       {lead}3;{x};{y}t
    অন্যথা যদি (did_request_winpos && argc == 3 && arg[0] == 3
						   && trail == 't')
    {
	winpos_x = arg[1];
	winpos_y = arg[2];
	// got finished code: consume it
	চাবি_name[0] = (পূর্ণাঙ্ক)KS_EXTRA;
	চাবি_name[1] = (পূর্ণাঙ্ক)KE_IGNORE;
	*slen = csi_len;

	যদি (--did_request_winpos <= 0)
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
    অন্যথা যদি ((arg[0] == 27 && argc == 3 && trail == '~')
		|| (argc == 2 && (trail == 'u' || trail == '~')))
    {
	পূর্ণাঙ্ক iskitty = argc == 2 && (trail == 'u' || trail == '~');
	প্রত্যাগম len + handle_চাবি_with_modifier(arg, csi_len, offset, buf,
		bufsize, buflen, iskitty, trail);
    }

    // Key without modifier (Kitty sends this for Esc or F3):
    //	{lead}{key}u
    //	{lead}{key}~
    অন্যথা যদি (argc == 1 && (trail == 'u' || trail == '~'))
    {
	প্রত্যাগম len + handle_চাবি_without_modifier(arg, csi_len, offset, buf,
		bufsize, buflen, trail);
    }

    // else: Unknown CSI sequence.  We could drop it, but then the
    // user can't create a map for it.
    প্রত্যাগম 0;
}

স্থির শূন্য
check_for_color_response(char_u *resp, পূর্ণাঙ্ক len)
{
    পূর্ণাঙ্ক		i, j;
    char_u	*argp;

    j = 1 + (resp[0] == ESC);
    argp = resp + j;

    যদি (len >= j + 3 && (argp[0] != '1'
			     || (argp[1] != '1' && argp[1] != '0')
			     || argp[2] != ';'))
	i = 0; // no match
    অন্যথা
    {
	প্রতি (i = j; i < len; ++i)
	    যদি (resp[i] == '\007' || (resp[0] == OSC ? resp[i] == STERM
			: (resp[i] == ESC && i + 1 < len && resp[i + 1] == '\\')))
	    {
		পূর্ণাঙ্ক is_bg = argp[1] == '1';
		পূর্ণাঙ্ক is_4digit = i - j >= 21 && resp[j + 11] == '/'
						  && resp[j + 16] == '/';

		যদি (i - j >= 15 && STRNCMP(resp + j + 3, "rgb:", 4) == 0
			    && (is_4digit
				   || (resp[j + 9] == '/' && resp[j + 12] == '/')))
		{
		    char_u *tp_r = resp + j + 7;
		    char_u *tp_g = resp + j + (is_4digit ? 12 : 10);
		    char_u *tp_b = resp + j + (is_4digit ? 17 : 13);
#ifdef FEAT_TERMRESPONSE
		    পূর্ণাঙ্ক rval, gval, bval;

		    rval = hexhex2nr(tp_r);
		    gval = hexhex2nr(tp_g);
		    bval = hexhex2nr(tp_b);
#endif
		    যদি (is_bg)
		    {
			বর্ণ *new_bg_val = (3 * '6' < *tp_r + *tp_g +
					     *tp_b) ? "light" : "dark";

			LOG_TRN("Received RBG response: r=%d g=%d b=%d", rval, gval, bval);
#ifdef FEAT_TERMRESPONSE
			rbg_status.tr_progress = STATUS_GOT;
			bg_r = rval;
			bg_g = gval;
			bg_b = bval;
#endif
			যদি (!option_was_set((char_u *)"bg")
				      && STRCMP(p_bg, new_bg_val) != 0)
			{
			    // value differs, apply it
			    set_option_মান_give_err((char_u *)"bg",
						  0L, (char_u *)new_bg_val, 0);
			    reset_option_was_set((char_u *)"bg");
			    redraw_asap(UPD_CLEAR);
			}
		    }
#যদি defined(FEAT_TERMRESPONSE) && defined(FEAT_TERMINAL)
		    অন্যথা
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
			রিক্ত, FALSE, curbuf);
		ভঙ্গ;
	    }
    }
    যদি (i == len)
	LOG_TR1("not enough characters for RB");
}

স্থির oscstate_T osc_state;

/*
 * Return true if currently receiving an OSC response.
 */
    bool
in_osc_sequence(শূন্য)
{
    প্রত্যাগম osc_state.processing;
}

/*
 * Handles any OSC sequence and places the result in "v:termosc". Returns OK on
 * success and FAIL on failure.
 */
    স্থির পূর্ণাঙ্ক
handle_osc(char_u *tp, পূর্ণাঙ্ক len, char_u *চাবি_name, পূর্ণাঙ্ক *slen)
{
    char_u last_char;

    যদি (!osc_state.processing)
    {
	পূর্ণাঙ্ক cur;

	LOG_TRN("Received OSC response: %s", (বর্ণ*)tp);
	// Check if it is a valid OSC sequence, and consume it. OSC format
	// consists of:
	// <identifier><data><terminator>
	// <identifier> is either <Esc>] or an OSC character
	// <terminator> can be '\007', <Esc>\ or STERM.
	cur = 1 + (tp[0] == ESC);

	যদি (len < cur + 1 + (tp[0] != OSC)) // Include terminator as well
	    প্রত্যাগম FAIL;

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
    অন্যথা
	last_char = ((char_u *)osc_state.buf.ga_data)[osc_state.buf.ga_len - 1];

    চাবি_name[0] = (পূর্ণাঙ্ক)KS_EXTRA;

    // Read data and append to buffer. If we reach a terminator, then
    // finally set the vim var.
    প্রতি (পূর্ণাঙ্ক i = 0; i < len; i++)
	যদি (tp[i] == '\007' || (osc_state.start_char == OSC ? tp[i] == STERM
		    : ((tp[i] == ESC && i + 1 < len && tp[i + 1] == '\\')
			|| (i == 0 && tp[i] == '\\' && last_char == ESC)
			)))
	{
	    osc_state.processing = FALSE;

	    চাবি_name[1] = (পূর্ণাঙ্ক)KE_OSC;

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
		    রিক্ত, FALSE, curbuf);
	    যদি (*p_bg != savebg)
		redraw_asap(UPD_CLEAR);
	    প্রত্যাগম OK;
	}

    চাবি_name[1] = (পূর্ণাঙ্ক)KE_IGNORE;

#ifdef ELAPSED_FUNC
    যদি (ELAPSED_FUNC(osc_state.start_tv) >= p_ost)
    {
	semsg(_(e_osc_response_timed_out), osc_state.buf.ga_len,
		osc_state.buf.ga_data);

	ga_clear(&osc_state.buf);
	osc_state.processing = FALSE;
	প্রত্যাগম FAIL;
    }
#endif

    ga_concat(&osc_state.buf, tp);
    *slen = len; // Consume everything

    প্রত্যাগম OK;
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
    স্থির পূর্ণাঙ্ক
handle_dcs(char_u *tp, char_u *argp, পূর্ণাঙ্ক len, char_u *চাবি_name, পূর্ণাঙ্ক *slen)
{
    পূর্ণাঙ্ক i, j;

    LOG_TRN("Received DCS response: %s", (বর্ণ*)tp);
    j = 1 + (tp[0] == ESC);
    যদি (len < j + 3)
	i = len; // need more chars
    অন্যথা যদি ((argp[1] != '+' && argp[1] != '$')
	    || (argp[2] != 'r' && argp[2] != 'R'))
	i = 0; // no match
    অন্যথা যদি (argp[1] == '+')
	// key code response
	প্রতি (i = j; i < len; ++i)
	{
	    যদি ((tp[i] == ESC && i + 1 < len && tp[i + 1] == '\\')
		    || tp[i] == STERM)
	    {
#ifdef FEAT_TERMRESPONSE
		// handle a key code response, drop a resource response
		যদি (i - j >= 3 && argp[2] == 'r')
		    got_code_from_term(tp + j, i);
#endif
		চাবি_name[0] = (পূর্ণাঙ্ক)KS_EXTRA;
		চাবি_name[1] = (পূর্ণাঙ্ক)KE_IGNORE;
		*slen = i + 1 + (tp[i] == ESC);
		ভঙ্গ;
	    }
	}
    অন্যথা
    {
	// Probably the cursor shape response.  Make sure that "i"
	// is equal to "len" when there are not sufficient
	// characters.
	প্রতি (i = j + 3; i < len; ++i)
	{
	    যদি (i - j == 3 && !SAFE_isdigit(tp[i]))
		ভঙ্গ;
	    যদি (i - j == 4 && tp[i] != ' ')
		ভঙ্গ;
	    যদি (i - j == 5 && tp[i] != 'q')
		ভঙ্গ;
	    যদি (i - j == 6 && tp[i] != ESC && tp[i] != STERM)
		ভঙ্গ;
	    যদি ((i - j == 6 && tp[i] == STERM)
		    || (i - j == 7 && tp[i] == '\\'))
	    {
#ifdef FEAT_TERMRESPONSE
		পূর্ণাঙ্ক সংখ্যা = argp[3] - '0';

		// 0, 1 = block blink, 2 = block
		// 3 = underline blink, 4 = underline
		// 5 = vertical bar blink, 6 = vertical bar
		সংখ্যা = সংখ্যা == 0 ? 1 : সংখ্যা;
		initial_cursor_shape = (সংখ্যা + 1) / 2;
		// The blink flag is actually inverted, compared to
		// the value set with T_SH.
		initial_cursor_shape_blink =
				       (সংখ্যা & 1) ? FALSE : TRUE;
		rcs_status.tr_progress = STATUS_GOT;
#endif
		LOG_TRN("Received cursor shape response: %s", tp);

		চাবি_name[0] = (পূর্ণাঙ্ক)KS_EXTRA;
		চাবি_name[1] = (পূর্ণাঙ্ক)KE_IGNORE;
		*slen = i + 1;
#ifdef FEAT_EVAL
		set_vim_var_string(VV_TERMSTYLERESP, tp, *slen);
#endif
		apply_autocmds(EVENT_TERMRESPONSEALL,
					(char_u *)"cursorshape", রিক্ত, FALSE, curbuf);
		ভঙ্গ;
	    }
	}
    }

    যদি (i == len)
    {
	// These codes arrive many together, each code can be
	// truncated at any point.
	LOG_TR1("not enough characters for XT");
	প্রত্যাগম FAIL;
    }
    প্রত্যাগম OK;
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
    পূর্ণাঙ্ক
check_termcode(
    পূর্ণাঙ্ক		max_offset,
    char_u	*buf,
    পূর্ণাঙ্ক		bufsize,
    পূর্ণাঙ্ক		*buflen)
{
    char_u	*tp;
    char_u	*p;
    পূর্ণাঙ্ক		slen = 0;	// init for GCC
    পূর্ণাঙ্ক		modslen;
    পূর্ণাঙ্ক		len;
    পূর্ণাঙ্ক		retval = 0;
    পূর্ণাঙ্ক		offset;
    char_u	চাবি_name[2];
    পূর্ণাঙ্ক		modifiers;
    char_u	*modifiers_start = রিক্ত;
    পূর্ণাঙ্ক		চাবি;
    পূর্ণাঙ্ক		new_slen;   // Length of what will replace the termcode
    char_u	string[MAX_চাবি_CODE_LEN + 1];
    পূর্ণাঙ্ক		i, j;
    পূর্ণাঙ্ক		idx = 0;
    পূর্ণাঙ্ক		cpo_koffset;

    cpo_koffset = (vim_strchr(p_cpo, CPO_KOFFSET) != রিক্ত);

    /*
     * Speed up the checks for terminal codes by gathering all first bytes
     * used in termleader[].  Often this is just a single <Esc>.
     */
    যদি (need_gather)
	gather_termleader();

    /*
     * Check at several positions in typebuf.tb_buf[], to catch something like
     * "x<Up>" that can be mapped. Stop at max_offset, because characters
     * after that cannot be used for mapping, and with @r commands
     * typebuf.tb_buf[] can become very long.
     * This is used often, KEEP IT FAST!
     */
    প্রতি (offset = 0; offset < max_offset; ++offset)
    {
	যদি (buf == রিক্ত)
	{
	    যদি (offset >= typebuf.tb_len)
		ভঙ্গ;
	    tp = typebuf.tb_buf + typebuf.tb_off + offset;
	    len = typebuf.tb_len - offset;	// length of the input
	}
	অন্যথা
	{
	    যদি (offset >= *buflen)
		ভঙ্গ;
	    tp = buf + offset;
	    len = *buflen - offset;
	}

	/*
	 * Don't check characters after K_SPECIAL, those are already
	 * translated terminal chars (avoid translating ~@^Hx).
	 */
	যদি (*tp == K_SPECIAL)
	{
	    offset += 2;	// there are always 2 extra characters
	    অনুবর্ত;
	}

	যদি (osc_state.processing)
	{
	    // Still processing OSC response data, go straight to handler
	    // function.
	    tp[len] = NUL;
	    চাবি_name[0] = NUL;
	    চাবি_name[1] = NUL;
	    modifiers = 0;
	    গচ্ছ handle_osc;
	}

	/*
	 * Skip this position if the character does not appear as the first
	 * character in term_strings. This speeds up a lot, since most
	 * termcodes start with the same character (ESC or CSI).
	 */
	i = *tp;
	প্রতি (p = termleader; *p && *p != i; ++p)
	    ;
	যদি (*p == NUL)
	    অনুবর্ত;

	/*
	 * Skip this position if p_ek is not set and tp[0] is an ESC and we
	 * are in Insert mode.
	 */
	যদি (*tp == ESC && !p_ek && (State & MODE_ঢোকাও))
	    অনুবর্ত;

	tp[len] = NUL;
	চাবি_name[0] = NUL;	// no key name found yet
	চাবি_name[1] = NUL;	// no key name found yet
	modifiers = 0;		// no modifiers yet

#ifdef FEAT_GUI
	যদি (gui.in_use)
	{
	    /*
	     * GUI special key codes are all of the form [CSI xx].
	     */
	    যদি (*tp == CSI)	    // Special key from GUI
	    {
		যদি (len < 3)
		    প্রত্যাগম -1;	    // Shouldn't happen
		slen = 3;
		চাবি_name[0] = tp[1];
		চাবি_name[1] = tp[2];
	    }
	}
	অন্যথা
#endif // FEAT_GUI
#ifdef MSWIN
	    যদি (len >= 3 && tp[0] == CSI && tp[1] == KS_EXTRA
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
	    চাবি_name[0] = tp[1];
	    চাবি_name[1] = tp[2];
	}
	অন্যথা
#endif
	{
	    পূর্ণাঙ্ক  mouse_index_found = -1;
	    পূর্ণাঙ্ক keypad_index_found = -1;
	    পূর্ণাঙ্ক keypad_slen_found = 0;

	    প্রতি (idx = 0; idx < tc_len; ++idx)
	    {
		পূর্ণাঙ্ক     is_keypad = FALSE;
		/*
		 * Ignore the entry if we are not at the start of
		 * typebuf.tb_buf[]
		 * and there are not enough characters to make a match.
		 * But only when the 'K' flag is in 'cpoptions'.
		 */
		slen = termcodes[idx].len;
		modifiers_start = রিক্ত;
		যদি (cpo_koffset && offset && len < slen)
		    অনুবর্ত;
		যদি (STRNCMP(termcodes[idx].code, tp,
				     (size_t)(slen > len ? len : slen)) == 0)
		{
		    পূর্ণাঙ্ক	    looks_like_mouse_start = FALSE;

		    যদি (len < slen)		// got a partial sequence
			প্রত্যাগম -1;		// need to get more chars

		    /*
		     * When found a keypad key, check if there is another key
		     * that matches and use that one.  This makes <Home> to be
		     * found instead of <kHome> when they produce the same
		     * key code.
		     */
		    যদি (termcodes[idx].name[0] == 'K'
				       && (VIM_ISDIGIT(termcodes[idx].name[1])
						|| ASCII_ISUPPER(termcodes[idx].name[1])))
		    {
			is_keypad = TRUE;
			// Only use it when there is no other match.
			যদি (keypad_index_found < 0)
			{
			    keypad_index_found = idx;
			    keypad_slen_found = slen;
			}
		    }

		    যদি (slen == 2 && len > 2
			    && termcodes[idx].code[0] == ESC
			    && termcodes[idx].code[1] == '[')
		    {
			// The mouse termcode "ESC [" is also the prefix of
			// "ESC [ I" (focus gained) and other keys.  Check some
			// more bytes to find out.
			যদি (!SAFE_isdigit(tp[2]))
			{
			    // ESC [ without number following: Only use it when
			    // there is no other match.
			    looks_like_mouse_start = TRUE;
			}
			অন্যথা যদি (termcodes[idx].name[0] == KS_DEC_MOUSE)
			{
			    char_u  *nr = tp + 2;
			    পূর্ণাঙ্ক	    গুনতি = 0;

			    // If a digit is following it could be a key with
			    // modifier, e.g., ESC [ 1;2P.  Can be confused
			    // with DEC_MOUSE, which requires four numbers
			    // following.  If not then it can't be a DEC_MOUSE
			    // code.
			    প্রতি (;;)
			    {
				++গুনতি;
				(শূন্য)getdigits(&nr);
				যদি (nr >= tp + len)
				    প্রত্যাগম -1;	// partial sequence
				যদি (*nr != ';')
				    ভঙ্গ;
				++nr;
				যদি (nr >= tp + len)
				    প্রত্যাগম -1;	// partial sequence
			    }
			    যদি (গুনতি < 4)
				অনুবর্ত;	// no match
			}
		    }
		    যদি (looks_like_mouse_start)
		    {
			// Only use it when there is no other match.
			যদি (mouse_index_found < 0)
			    mouse_index_found = idx;
		    }
		    অন্যথা যদি (!is_keypad)
		    {
			চাবি_name[0] = termcodes[idx].name[0];
			চাবি_name[1] = termcodes[idx].name[1];
			ভঙ্গ;
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
		যদি (termcodes[idx].modlen > 0 && mouse_index_found < 0)
		{
		    modslen = termcodes[idx].modlen;
		    যদি (cpo_koffset && offset && len < modslen)
			অনুবর্ত;
		    যদি (STRNCMP(termcodes[idx].code, tp,
				(size_t)(modslen > len ? len : modslen)) == 0)
		    {
			পূর্ণাঙ্ক	    n;

			যদি (len <= modslen)	// got a partial sequence
			    প্রত্যাগম -1;		// need to get more chars

			যদি (tp[modslen] == termcodes[idx].code[slen - 1])
			    // no modifiers
			    slen = modslen + 1;
			অন্যথা যদি (tp[modslen] != ';' && modslen == slen - 3)
			    // no match for "code;*X" with "code;"
			    অনুবর্ত;
			অন্যথা যদি (termcodes[idx].code[modslen] == '@'
					 && (tp[modslen] != '1'
						    || tp[modslen + 1] != ';'))
			    // no match for "<Esc>[@" with "<Esc>[1;"
			    অনুবর্ত;
			অন্যথা
			{
			    // Skip over the digits, the final char must
			    // follow. URXVT can use a negative value, thus
			    // also accept '-'.
			    প্রতি (j = slen - 2; j < len && (SAFE_isdigit(tp[j])
				       || tp[j] == '-' || tp[j] == ';'); ++j)
				;
			    ++j;
			    যদি (len < j)	// got a partial sequence
				প্রত্যাগম -1;	// need to get more chars
			    যদি (tp[j - 1] != termcodes[idx].code[slen - 1])
				অনুবর্ত;	// no match

			    modifiers_start = tp + slen - 2;

			    // Match!  Convert modifier bits.
			    n = atoi((বর্ণ *)modifiers_start);
			    modifiers |= decode_modifiers(n);

			    slen = j;
			}
			যদি (termcodes[idx].name[0] == 'K'
					   && (VIM_ISDIGIT(termcodes[idx].name[1])
						    || ASCII_ISUPPER(termcodes[idx].name[1])))
			{
			    is_keypad = TRUE;
			    যদি (keypad_index_found < 0)
			    {
				keypad_index_found = idx;
				keypad_slen_found = slen;
			    }
			}
			যদি (!is_keypad)
			{
			    চাবি_name[0] = termcodes[idx].name[0];
			    চাবি_name[1] = termcodes[idx].name[1];
			    ভঙ্গ;
			}
		    }
		}
	    }
	    যদি (idx == tc_len && keypad_index_found >= 0)
	    {
		চাবি_name[0] = termcodes[keypad_index_found].name[0];
		চাবি_name[1] = termcodes[keypad_index_found].name[1];
		slen = keypad_slen_found;
	    }
	    অন্যথা যদি (idx == tc_len && mouse_index_found >= 0)
	    {
		চাবি_name[0] = termcodes[mouse_index_found].name[0];
		চাবি_name[1] = termcodes[mouse_index_found].name[1];
	    }
	}

	যদি (চাবি_name[0] == NUL
	    // Mouse codes of DEC and pterm start with <ESC>[.  When
	    // detecting the start of these mouse codes they might as well be
	    // another key code or terminal response.
#ifdef FEAT_MOUSE_DEC
	    || চাবি_name[0] == KS_DEC_MOUSE
#endif
#ifdef FEAT_MOUSE_PTERM
	    || চাবি_name[0] == KS_PTERM_MOUSE
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
	    যদি (((tp[0] == ESC && len >= 3 && tp[1] == '[')
			|| (tp[0] == CSI && len >= 2))
		    && vim_strchr((char_u *)"0123456789>?ABCDEFHPQRS",
								*argp) != রিক্ত)
	    {
		পূর্ণাঙ্ক resp = handle_csi(tp, len, argp, offset, buf,
					     bufsize, buflen, চাবি_name, &slen);
		যদি (resp != 0)
		{
#ifdef DEBUG_TERMRESPONSE
		    যদি (resp == -1)
			LOG_TR1("Not enough characters for CSI sequence");
#endif
		    প্রত্যাগম resp;
		}
	    }

	    // Check for OSC responses from terminal
	    অন্যথা যদি ((tp[0] == ESC && len >= 2 && tp[1] == ']') || tp[0] == OSC)
	    {
handle_osc:
		যদি (handle_osc(tp, len, চাবি_name, &slen) == FAIL)
		    প্রত্যাগম -1;
	    }

	    // Check for key code response from xterm,
	    // starting with <Esc>P or DCS
	    // It would only be needed with this condition:
	    //	    (check_for_codes || rcs_status.tr_progress == STATUS_SENT)
	    // Now this is always done so that DCS codes don't mess up things.
	    অন্যথা যদি ((tp[0] == ESC && len >= 2 && tp[1] == 'P') || tp[0] == DCS)
	    {
		যদি (handle_dcs(tp, argp, len, চাবি_name, &slen) == FAIL)
		    প্রত্যাগম -1;
	    }
	}

	যদি (চাবি_name[0] == NUL)
	    অনুবর্ত;	    // No match at this position, try next one

	// We only get here when we have a complete termcode match

#যদি defined(FEAT_GUI) || defined(MSWIN)
	/*
	 * For scroll events from the GUI or MS-Windows console, fetch the
	 * pointer coordinates so that we know which window to scroll later.
	 */
	যদি (TRUE
# যদি defined(FEAT_GUI) && !defined(MSWIN)
		&& gui.in_use
# endif
		&& চাবি_name[0] == (পূর্ণাঙ্ক)KS_EXTRA
		&& (চাবি_name[1] == (পূর্ণাঙ্ক)KE_X1MOUSE
		    || চাবি_name[1] == (পূর্ণাঙ্ক)KE_X2MOUSE
		    || চাবি_name[1] == (পূর্ণাঙ্ক)KE_MOUSEMOVE_XY
		    || চাবি_name[1] == (পূর্ণাঙ্ক)KE_MOUSELEFT
		    || চাবি_name[1] == (পূর্ণাঙ্ক)KE_MOUSERIGHT
		    || চাবি_name[1] == (পূর্ণাঙ্ক)KE_MOUSEDOWN
		    || চাবি_name[1] == (পূর্ণাঙ্ক)KE_MOUSEUP))
	{
	    char_u	bytes[6];
	    পূর্ণাঙ্ক		অঙ্ক_bytes = get_bytes_from_buf(tp + slen, bytes, 4);

	    যদি (অঙ্ক_bytes == -1)	// not enough coordinates
		প্রত্যাগম -1;
	    mouse_col = 128 * (bytes[0] - ' ' - 1) + bytes[1] - ' ' - 1;
	    mouse_row = 128 * (bytes[2] - ' ' - 1) + bytes[3] - ' ' - 1;
	    slen += অঙ্ক_bytes;
	    // equal to K_MOUSEMOVE
	    যদি (চাবি_name[1] == (পূর্ণাঙ্ক)KE_MOUSEMOVE_XY)
		চাবি_name[1] = (পূর্ণাঙ্ক)KE_MOUSEMOVE;
	}
	অন্যথা
#endif
	/*
	 * If it is a mouse click, get the coordinates.
	 */
	যদি (চাবি_name[0] == KS_MOUSE
#ifdef FEAT_MOUSE_GPM
		|| চাবি_name[0] == KS_GPM_MOUSE
#endif
#ifdef FEAT_MOUSE_JSB
		|| চাবি_name[0] == KS_JSBTERM_MOUSE
#endif
#ifdef FEAT_MOUSE_NET
		|| চাবি_name[0] == KS_NETTERM_MOUSE
#endif
#ifdef FEAT_MOUSE_DEC
		|| চাবি_name[0] == KS_DEC_MOUSE
#endif
#ifdef FEAT_MOUSE_PTERM
		|| চাবি_name[0] == KS_PTERM_MOUSE
#endif
#ifdef FEAT_MOUSE_URXVT
		|| চাবি_name[0] == KS_URXVT_MOUSE
#endif
		|| চাবি_name[0] == KS_SGR_MOUSE
		|| চাবি_name[0] == KS_SGR_MOUSE_RELEASE)
	{
	    যদি (check_termcode_mouse(tp, &slen, চাবি_name, modifiers_start, idx,
							     &modifiers) == -1)
		প্রত্যাগম -1;
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
	অন্যথা যদি (চাবি_name[0] == (পূর্ণাঙ্ক)KS_MENU)
	{
	    long_u	val;
	    পূর্ণাঙ্ক		অঙ্ক_bytes = get_long_from_buf(tp + slen, &val);

	    যদি (অঙ্ক_bytes == -1)
		প্রত্যাগম -1;
	    current_menu = (vimmenu_T *)val;
	    slen += অঙ্ক_bytes;

	    // The menu may have been deleted right after it was used, check
	    // for that.
	    যদি (check_menu_pointer(মূল_menu, current_menu) == FAIL)
	    {
		চাবি_name[0] = KS_EXTRA;
		চাবি_name[1] = (পূর্ণাঙ্ক)KE_IGNORE;
	    }
	}
# endif
# ifdef FEAT_GUI_TABLINE
	অন্যথা যদি (চাবি_name[0] == (পূর্ণাঙ্ক)KS_TABLINE)
	{
	    // Selecting tabline tab or using its menu.
	    char_u	bytes[6];
	    পূর্ণাঙ্ক		অঙ্ক_bytes = get_bytes_from_buf(tp + slen, bytes, 1);

	    যদি (অঙ্ক_bytes == -1)
		প্রত্যাগম -1;
	    current_tab = (পূর্ণাঙ্ক)bytes[0];
	    যদি (current_tab == 255)	// -1 in a byte gives 255
		current_tab = -1;
	    slen += অঙ্ক_bytes;
	}
	অন্যথা যদি (চাবি_name[0] == (পূর্ণাঙ্ক)KS_TABMENU)
	{
	    // Selecting tabline tab or using its menu.
	    char_u	bytes[6];
	    পূর্ণাঙ্ক		অঙ্ক_bytes = get_bytes_from_buf(tp + slen, bytes, 2);

	    যদি (অঙ্ক_bytes == -1)
		প্রত্যাগম -1;
	    current_tab = (পূর্ণাঙ্ক)bytes[0];
	    current_tabmenu = (পূর্ণাঙ্ক)bytes[1];
	    slen += অঙ্ক_bytes;
	}
# endif
# ifndef USE_ON_FLY_SCROLL
	অন্যথা যদি (চাবি_name[0] == (পূর্ণাঙ্ক)KS_VER_SCROLLBAR)
	{
	    long_u	val;
	    char_u	bytes[6];
	    পূর্ণাঙ্ক		অঙ্ক_bytes;

	    // Get the last scrollbar event in the queue of the same type
	    j = 0;
	    প্রতি (i = 0; tp[j] == CSI && tp[j + 1] == KS_VER_SCROLLBAR
						     && tp[j + 2] != NUL; ++i)
	    {
		j += 3;
		অঙ্ক_bytes = get_bytes_from_buf(tp + j, bytes, 1);
		যদি (অঙ্ক_bytes == -1)
		    ভঙ্গ;
		যদি (i == 0)
		    current_scrollbar = (পূর্ণাঙ্ক)bytes[0];
		অন্যথা যদি (current_scrollbar != (পূর্ণাঙ্ক)bytes[0])
		    ভঙ্গ;
		j += অঙ্ক_bytes;
		অঙ্ক_bytes = get_long_from_buf(tp + j, &val);
		যদি (অঙ্ক_bytes == -1)
		    ভঙ্গ;
		scrollbar_মান = val;
		j += অঙ্ক_bytes;
		slen = j;
	    }
	    যদি (i == 0)		// not enough characters to make one
		প্রত্যাগম -1;
	}
	অন্যথা যদি (চাবি_name[0] == (পূর্ণাঙ্ক)KS_HOR_SCROLLBAR)
	{
	    long_u	val;
	    পূর্ণাঙ্ক		অঙ্ক_bytes;

	    // Get the last horiz. scrollbar event in the queue
	    j = 0;
	    প্রতি (i = 0; tp[j] == CSI && tp[j + 1] == KS_HOR_SCROLLBAR
						     && tp[j + 2] != NUL; ++i)
	    {
		j += 3;
		অঙ্ক_bytes = get_long_from_buf(tp + j, &val);
		যদি (অঙ্ক_bytes == -1)
		    ভঙ্গ;
		scrollbar_মান = val;
		j += অঙ্ক_bytes;
		slen = j;
	    }
	    যদি (i == 0)		// not enough characters to make one
		প্রত্যাগম -1;
	}
# endif // !USE_ON_FLY_SCROLL
#endif // FEAT_GUI

#যদি defined(UNIX) || defined(VMS)
	/*
	 * Handle FocusIn/FocusOut event sequences reported by XTerm.
	 * (CSI I/CSI O)
	 */
	যদি (চাবি_name[0] == KS_EXTRA
# ifdef FEAT_GUI
		&& !gui.in_use
# endif
	    )
	{
	    যদি (চাবি_name[1] == KE_FOCUSGAINED)
	    {
		যদি (!focus_state)
		{
		    ui_focus_change(TRUE);
		    did_cursorhold = TRUE;
		    focus_state = TRUE;
		}
		চাবি_name[1] = (পূর্ণাঙ্ক)KE_IGNORE;
	    }
	    অন্যথা যদি (চাবি_name[1] == KE_FOCUSLOST)
	    {
		যদি (focus_state)
		{
		    ui_focus_change(FALSE);
		    did_cursorhold = TRUE;
		    focus_state = FALSE;
		}
		চাবি_name[1] = (পূর্ণাঙ্ক)KE_IGNORE;
	    }
	}
#endif

	/*
	 * Change <xHome> to <Home>, <xUp> to <Up>, etc.
	 */
	চাবি = handle_x_চাবিগুলি(TERMCAP2_চাবি(চাবি_name[0], চাবি_name[1]));

	// Add any modifier codes to our string.
	new_slen = modifiers2keycode(modifiers, &চাবি, string);

	// Finally, add the special key code to our string
	চাবি_name[0] = KEY2TERMCAP0(চাবি);
	চাবি_name[1] = KEY2TERMCAP1(চাবি);
	যদি (চাবি_name[0] == KS_চাবি)
	{
	    // from ":set <M-b>=xx"
	    যদি (has_mbyte)
		new_slen += (*mb_char2bytes)(চাবি_name[1], string + new_slen);
	    অন্যথা
		string[new_slen++] = চাবি_name[1];
	}
	অন্যথা যদি (osc_state.processing ||
		(new_slen == 0
		 && চাবি_name[0] == KS_EXTRA
		 && চাবি_name[1] == KE_IGNORE))
	{
	    // Do not put K_IGNORE into the buffer, do return KEYLEN_REMOVED
	    // to indicate what happened.
	    retval = KEYLEN_REMOVED;
	}
	অন্যথা
	{
	    string[new_slen++] = K_SPECIAL;
	    string[new_slen++] = চাবি_name[0];
	    string[new_slen++] = চাবি_name[1];
	}
	যদি (put_string_in_typebuf(offset, slen, string, new_slen,
						 buf, bufsize, buflen) == FAIL)
	    প্রত্যাগম -1;
	প্রত্যাগম retval == 0 ? (len + new_slen - slen + offset) : retval;
    }

#ifdef FEAT_TERMRESPONSE
    LOG_TR1("normal character");
#endif

    প্রত্যাগম 0;			    // no match found
}

#যদি defined(FEAT_TERMINAL) && defined(FEAT_TERMRESPONSE)
/*
 * Get the text foreground color, if known.
 */
    শূন্য
term_get_fg_color(char_u *r, char_u *g, char_u *b)
{
    যদি (rfg_status.tr_progress != STATUS_GOT)
	প্রত্যাগম;

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
    পূর্ণাঙ্ক
term_get_bg_color(char_u *r, char_u *g, char_u *b)
{
    যদি (rbg_status.tr_progress != STATUS_GOT)
	প্রত্যাগম FAIL;

    *r = bg_r;
    *g = bg_g;
    *b = bg_b;
    প্রত্যাগম OK;
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
    পূর্ণাঙ্ক		flags,
    পূর্ণাঙ্ক		*did_simplify)
{
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		slen;
    পূর্ণাঙ্ক		চাবি;
    size_t	dlen = 0;
    char_u	*src;
    পূর্ণাঙ্ক		do_backslash;	// backslash is a special character
    পূর্ণাঙ্ক		do_special;	// recognize <> key codes
    পূর্ণাঙ্ক		do_চাবি_code;	// recognize raw key codes
    char_u	*result;	// buffer for resulting string
    garray_T	ga;

    do_backslash = (vim_strchr(p_cpo, CPO_BSLASH) == রিক্ত);
    do_special = (vim_strchr(p_cpo, CPO_SPECI) == রিক্ত)
						  || (flags & REPTERM_SPECIAL);
    do_চাবি_code = (vim_strchr(p_cpo, CPO_KEYCODE) == রিক্ত);
    src = from;

    /*
     * Allocate space for the translation.  Worst case a single character is
     * replaced by 6 bytes (shifted special key), plus a NUL at the end.
     * In the rare case more might be needed ga_grow() must be called again.
     */
    ga_init2(&ga, 1L, 100);
    যদি (ga_grow(&ga, (পূর্ণাঙ্ক)(STRLEN(src) * 6 + 1)) == FAIL) // out of memory
    {
	*bufp = রিক্ত;
	প্রত্যাগম from;
    }
    result = ga.ga_data;

    /*
     * Check for #n at start only: function key n
     */
    যদি ((flags & REPTERM_FROM_PART) && src[0] == '#' && VIM_ISDIGIT(src[1]))
    {
	result[dlen++] = K_SPECIAL;
	result[dlen++] = 'k';
	যদি (src[1] == '0')
	    result[dlen++] = ';';	// #0 is F10 is "k;"
	অন্যথা
	    result[dlen++] = src[1];	// #3 is F3 is "k3"
	src += 2;
    }

    /*
     * Copy each byte from *from to result[dlen]
     */
    যাবত্ (*src != NUL)
    {
	/*
	 * If 'cpoptions' does not contain '<', check for special key codes,
	 * like "<C-S-LeftMouse>"
	 */
	যদি (do_special && ((flags & REPTERM_DO_LT)
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
	    যদি (STRNICMP(src, "<SID>", 5) == 0)
	    {
		যদি (sid_arg < 0 || (sid_arg == 0 && current_sctx.sc_sid <= 0))
		    emsg(_(e_using_sid_not_in_script_context));
		অন্যথা
		{
		    char_u  *dot;
		    long    sid = sid_arg != 0 ? sid_arg : current_sctx.sc_sid;

		    src += 5;
		    যদি (in_vim9script()
				       && (dot = vim_strchr(src, '.')) != রিক্ত)
		    {
			imported_T *imp = find_imported(src, dot - src, FALSE);

			যদি (imp != রিক্ত)
			{
			    scriptitem_T    *si = SCRIPT_ITEM(imp->imp_sid);
			    size_t	    len;

			    src = dot + 1;
			    যদি (si->sn_autoload_prefix != রিক্ত)
			    {
				// Turn "<SID>name.Func"
				// into "scriptname#Func".
				len = STRLEN(si->sn_autoload_prefix);
				যদি (ga_grow(&ga,
				     (পূর্ণাঙ্ক)(STRLEN(src) * 6 + len + 1)) == FAIL)
				{
				    ga_clear(&ga);
				    *bufp = রিক্ত;
				    প্রত্যাগম from;
				}
				result = ga.ga_data;
				STRCPY(result + dlen, si->sn_autoload_prefix);
				dlen += len;
				অনুবর্ত;
			    }
			    sid = imp->imp_sid;
			}
		    }

		    result[dlen++] = K_SPECIAL;
		    result[dlen++] = (পূর্ণাঙ্ক)KS_EXTRA;
		    result[dlen++] = (পূর্ণাঙ্ক)KE_SNR;
		    sprintf((বর্ণ *)result + dlen, "%ld", sid);
		    dlen += STRLEN(result + dlen);
		    result[dlen++] = '_';
		    অনুবর্ত;
		}
	    }
#endif
	    পূর্ণাঙ্ক fsk_flags = FSK_KEYCODE
			| ((flags & REPTERM_NO_SIMPLIFY) ? 0 : FSK_SIMPLIFY)
			| ((flags & REPTERM_FROM_PART) ? FSK_FROM_PART : 0);
	    slen = trans_special(&src, result + dlen, fsk_flags,
							   TRUE, did_simplify);
	    যদি (slen > 0)
	    {
		dlen += slen;
		অনুবর্ত;
	    }
	}

	/*
	 * If 'cpoptions' does not contain 'k', see if it's an actual key-code.
	 * Note that this is also checked after replacing the <> form.
	 * Single character codes are NOT replaced (e.g. ^H or DEL), because
	 * it could be a character in the file.
	 */
	যদি (do_চাবি_code)
	{
	    পূর্ণাঙ্ক len;
	    i = find_term_bykeys(src, &len);
	    যদি (i >= 0)
	    {
		result[dlen++] = K_SPECIAL;
		result[dlen++] = termcodes[i].name[0];
		result[dlen++] = termcodes[i].name[1];
		src += len;
		// If terminal code matched, continue after it.
		অনুবর্ত;
	    }
	}

#ifdef FEAT_EVAL
	যদি (do_special)
	{
	    char_u	*p, *s, len;

	    /*
	     * Replace <Leader> by the value of "mapleader".
	     * Replace <LocalLeader> by the value of "maplocalleader".
	     * If "mapleader" or "maplocalleader" isn't set use a backslash.
	     */
	    যদি (STRNICMP(src, "<Leader>", 8) == 0)
	    {
		len = 8;
		p = get_var_মান((char_u *)"g:mapleader");
	    }
	    অন্যথা যদি (STRNICMP(src, "<LocalLeader>", 13) == 0)
	    {
		len = 13;
		p = get_var_মান((char_u *)"g:maplocalleader");
	    }
	    অন্যথা
	    {
		len = 0;
		p = রিক্ত;
	    }
	    যদি (len != 0)
	    {
		// Allow up to 8 * 6 characters for "mapleader".
		যদি (p == রিক্ত || *p == NUL || STRLEN(p) > 8 * 6)
		    s = (char_u *)"\\";
		অন্যথা
		    s = p;
		যাবত্ (*s != NUL)
		    result[dlen++] = *s++;
		src += len;
		অনুবর্ত;
	    }
	}
#endif

	/*
	 * Remove CTRL-V and ignore the next character.
	 * For "from" side the CTRL-V at the end is included, for the "to"
	 * part it is removed.
	 * If 'cpoptions' does not contain 'B', also accept a backslash.
	 */
	চাবি = *src;
	যদি (চাবি == Ctrl_V || (do_backslash && চাবি == '\\'))
	{
	    ++src;				// skip CTRL-V or backslash
	    যদি (*src == NUL)
	    {
		যদি (flags & REPTERM_FROM_PART)
		    result[dlen++] = চাবি;
		ভঙ্গ;
	    }
	}

	// skip multibyte char correctly
	প্রতি (i = (*mb_ptr2len)(src); i > 0; --i)
	{
	    /*
	     * If the character is K_SPECIAL, replace it with K_SPECIAL
	     * KS_SPECIAL KE_FILLER.
	     * If compiled with the GUI replace CSI with K_CSI.
	     */
	    যদি (*src == K_SPECIAL)
	    {
		result[dlen++] = K_SPECIAL;
		result[dlen++] = KS_SPECIAL;
		result[dlen++] = KE_FILLER;
	    }
#ifdef FEAT_GUI
	    অন্যথা যদি (*src == CSI)
	    {
		result[dlen++] = K_SPECIAL;
		result[dlen++] = KS_EXTRA;
		result[dlen++] = (পূর্ণাঙ্ক)KE_CSI;
	    }
#endif
	    অন্যথা
		result[dlen++] = *src;
	    ++src;
	}
    }
    result[dlen] = NUL;

    /*
     * Copy the new string to allocated memory.
     * If this fails, just return from.
     */
    যদি ((*bufp = vim_strsave(result)) != রিক্ত)
	from = *bufp;
    vim_free(result);
    প্রত্যাগম from;
}

/*
 * Find a termcode with keys 'src' (must be NUL terminated).
 * Return the index in termcodes[], or -1 if not found.
 */
    স্থির পূর্ণাঙ্ক
find_term_bykeys(char_u *src, পূর্ণাঙ্ক *matchlen)
{
    পূর্ণাঙ্ক		i, j;
    পূর্ণাঙ্ক		len = (পূর্ণাঙ্ক)STRLEN(src);
    পূর্ণাঙ্ক         found = -1;
    // Don't return a match for a single character
    পূর্ণাঙ্ক         foundlen = 1;
    পূর্ণাঙ্ক         slen, modslen;
    পূর্ণাঙ্ক         thislen;

    // Most input bytes cannot start a terminal code.  Reuse the same leader
    // table as check_termcode() to avoid scanning termcodes[] unnecessarily.
    যদি (need_gather)
	gather_termleader();
    যদি (*src == NUL || vim_strchr(termleader, *src) == রিক্ত)
	প্রত্যাগম -1;

    // find longest match
    // borrows part of check_termcode
    প্রতি (i = 0; i < tc_len; ++i)
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
	যদি (modslen > 0)
	{
	    যদি (len > modslen
			&& STRNCMP(termcodes[i].code, src, (size_t)modslen) == 0)
	    {
		thislen = 0;

		যদি (src[modslen] == termcodes[i].code[slen - 1])
		    // no modifiers
		    thislen = modslen + 1;
		অন্যথা যদি (src[modslen] != ';' && modslen == slen - 3)
		    // no match for "code;*X" with "code;"
		    অনুবর্ত;
		অন্যথা যদি (termcodes[i].code[modslen] == '@'
				&& (src[modslen] != '1'
					    || src[modslen + 1] != ';'))
		    // no match for "<Esc>[@" with "<Esc>[1;"
		    অনুবর্ত;
		অন্যথা
		{
		    // Skip over the digits, the final char must
		    // follow. URXVT can use a negative value, thus
		    // also accept '-'.
		    প্রতি (j = slen - 2; j < len && (SAFE_isdigit(src[j])
			       || src[j] == '-' || src[j] == ';'); ++j)
			;
		    ++j;
		    যদি (len < j)	// got a partial sequence
			অনুবর্ত;
		    যদি (src[j - 1] != termcodes[i].code[slen - 1])
			অনুবর্ত;	// no match

		    thislen = j;
		}

		যদি (thislen > foundlen)
		{
		    found = i;
		    foundlen = thislen;
		}
	    }
	}
	অন্যথা
	{
	    যদি (slen > foundlen && len >= slen
			&& STRNCMP(termcodes[i].code, src, (size_t)slen) == 0)
	    {
		found = i;
		foundlen = slen;
	    }
	}
    }

    যদি (matchlen != রিক্ত && found >= 0)
	*matchlen = foundlen;

    প্রত্যাগম found;
}

/*
 * Gather the first characters in the terminal key codes into a string.
 * Used to speed up check_termcode().
 */
    স্থির শূন্য
gather_termleader(শূন্য)
{
    পূর্ণাঙ্ক	    i;
    পূর্ণাঙ্ক	    len = 0;

#ifdef FEAT_GUI
    যদি (gui.in_use)
	termleader[len++] = CSI;    // the GUI codes are not in termcodes[]
#endif

    // Querying cell size uses CSI 14 t
#যদি defined(MSWIN) && (defined(FEAT_IMAGE) || defined(FEAT_EVAL))
# ifdef FEAT_GUI
    যদি (!gui.in_use)
# endif
    {
	termleader[len++] = ESC;
# ifdef FEAT_GUI
	যদি (!gui.in_use)
# endif
	    termleader[len++] = CSI;
    }
#endif

#ifdef FEAT_TERMRESPONSE
    যদি (check_for_codes || *T_CRS != NUL)
	termleader[len++] = DCS;    // the termcode response starts with DCS
				    // in 8-bit mode
#endif
    termleader[len] = NUL;

    প্রতি (i = 0; i < tc_len; ++i)
	যদি (vim_strchr(termleader, termcodes[i].code[0]) == রিক্ত)
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
    শূন্য
show_termcodes(পূর্ণাঙ্ক flags)
{
    পূর্ণাঙ্ক		col;
    পূর্ণাঙ্ক		*বস্তুসমূহ;
    পূর্ণাঙ্ক		item_গুনতি;
    পূর্ণাঙ্ক		run;
    পূর্ণাঙ্ক		row, rows;
    পূর্ণাঙ্ক		cols;
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		len;

#define INC3 27	    // try to make three columns
#define INC2 40	    // try to make two columns
#define GAP 2	    // spaces between columns

    যদি (tc_len == 0)	    // no terminal codes (must be GUI)
	প্রত্যাগম;
    বস্তুসমূহ = ALLOC_MULT(পূর্ণাঙ্ক, tc_len);
    যদি (বস্তুসমূহ == রিক্ত)
	প্রত্যাগম;

    // Highlight title
    বার্তা_puts_title(_("\n--- Terminal keys ---"));

    /*
     * Do the loop three times:
     * 1. display the short items (non-strings and short strings)
     * 2. display the medium items (medium length strings)
     * 3. display the long items (remaining strings)
     * When "flags" has OPT_ONECOLUMN do everything in 3.
     */
    প্রতি (run = (flags & OPT_ONECOLUMN) ? 3 : 1; run <= 3 && !got_int; ++run)
    {
	/*
	 * collect the items in items[]
	 */
	item_গুনতি = 0;
	প্রতি (i = 0; i < tc_len; i++)
	{
	    len = show_one_termcode(termcodes[i].name,
						    termcodes[i].code, FALSE);
	    যদি ((flags & OPT_ONECOLUMN) ||
		    (len <= INC3 - GAP ? run == 1
			: len <= INC2 - GAP ? run == 2
			: run == 3))
		বস্তুসমূহ[item_গুনতি++] = i;
	}

	/*
	 * display the items
	 */
	যদি (run <= 2)
	{
	    cols = (Columns + GAP) / (run == 1 ? INC3 : INC2);
	    যদি (cols == 0)
		cols = 1;
	    rows = (item_গুনতি + cols - 1) / cols;
	}
	অন্যথা	// run == 3
	    rows = item_গুনতি;
	প্রতি (row = 0; row < rows && !got_int; ++row)
	{
	    বার্তা_putchar('\n');			// go to next line
	    যদি (got_int)			// 'q' typed in more
		ভঙ্গ;
	    col = 0;
	    প্রতি (i = row; i < item_গুনতি; i += rows)
	    {
		বার্তা_col = col;			// make columns
		show_one_termcode(termcodes[বস্তুসমূহ[i]].name,
					      termcodes[বস্তুসমূহ[i]].code, TRUE);
		যদি (run == 2)
		    col += INC2;
		অন্যথা
		    col += INC3;
	    }
	    out_flush();
	    ui_breakcheck();
	}
    }
    vim_free(বস্তুসমূহ);
}

/*
 * Show one termcode entry.
 * Output goes into IObuff[]
 */
    পূর্ণাঙ্ক
show_one_termcode(char_u *name, char_u *code, পূর্ণাঙ্ক printit)
{
    char_u	*p;
    পূর্ণাঙ্ক		len;

    যদি (name[0] > '~')
    {
	IObuff[0] = ' ';
	IObuff[1] = ' ';
	IObuff[2] = ' ';
	IObuff[3] = ' ';
    }
    অন্যথা
    {
	IObuff[0] = 't';
	IObuff[1] = '_';
	IObuff[2] = name[0];
	IObuff[3] = name[1];
    }
    IObuff[4] = ' ';

    p = get_special_চাবি_name(TERMCAP2_চাবি(name[0], name[1]), 0);
    যদি (p[1] != 't')
	STRCPY(IObuff + 5, p);
    অন্যথা
	IObuff[5] = NUL;
    len = (পূর্ণাঙ্ক)STRLEN(IObuff);
    কুরু
	IObuff[len++] = ' ';
    যাবত্ (len < 17);
    IObuff[len] = NUL;
    যদি (code == রিক্ত)
	len += 4;
    অন্যথা
	len += vim_strsize(code);

    যদি (printit)
    {
	বার্তা_puts((বর্ণ *)IObuff);
	যদি (code == রিক্ত)
	    বার্তা_puts("NULL");
	অন্যথা
	    বার্তা_outtrans(code);
    }
    প্রত্যাগম len;
}

#যদি defined(FEAT_TERMRESPONSE)
/*
 * For Xterm >= 140 compiled with OPT_TCAP_QUERY: Obtain the actually used
 * termcap codes from the terminal itself.
 * We get them one by one to avoid a very long response string.
 */
স্থির পূর্ণাঙ্ক xt_index_in = 0;
স্থির পূর্ণাঙ্ক xt_index_out = 0;

    স্থির শূন্য
req_codes_from_term(শূন্য)
{
    xt_index_out = 0;
    xt_index_in = 0;
    req_more_codes_from_term();
}

    স্থির শূন্য
req_more_codes_from_term(শূন্য)
{
    বর্ণ	buf[32];  // extra size to shut up LGTM
    পূর্ণাঙ্ক		old_idx = xt_index_out;

    // Don't do anything when going to exit.
    যদি (exiting)
	প্রত্যাগম;

    // Send up to 10 more requests out than we received.  Avoid sending too
    // many, there can be a buffer overflow somewhere.
    যাবত্ (xt_index_out < xt_index_in + 10 && চাবি_names[xt_index_out] != রিক্ত)
    {
	বর্ণ *চাবি_name = চাবি_names[xt_index_out];

	MAY_WANT_TO_LOG_THIS;
	LOG_TRN("Requesting XT %d: %s", xt_index_out, চাবি_name);
	যদি (চাবি_name[2] != NUL)
	    sprintf(buf, "\033P+q%02x%02x%02x\033\\", চাবি_name[0], চাবি_name[1], চাবি_name[2]);
	অন্যথা
	    sprintf(buf, "\033P+q%02x%02x\033\\", চাবি_name[0], চাবি_name[1]);
	out_str_nf((char_u *)buf);
	++xt_index_out;
    }

    // Send the codes out right away.
    যদি (xt_index_out != old_idx)
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
    স্থির শূন্য
got_code_from_term(char_u *code, পূর্ণাঙ্ক len)
{
# define XT_LEN 100
    char_u	name[4];
    char_u	str[XT_LEN];
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		j = 0;
    পূর্ণাঙ্ক		c;

    // A '1' means the code is supported, a '0' means it isn't.
    // If it is supported, there must be a '=' following
    // When half the length is > XT_LEN we can't use it.
    যদি (code[0] == '1' && (code[7] == '=' || code[9] == '=') && len / 2 < XT_LEN)
    {
	// Get the name from the response and find it in the table.
	name[0] = hexhex2nr(code + 3);
	name[1] = hexhex2nr(code + 5);
	যদি (code[9] == '=')
	    name[2] = hexhex2nr(code + 7);
	অন্যথা
	    name[2] = NUL;
	name[3] = NUL;
	প্রতি (i = 0; চাবি_names[i] != রিক্ত; ++i)
	{
	    যদি (STRCMP(চাবি_names[i], name) == 0)
	    {
		xt_index_in = i;
		ভঙ্গ;
	    }
	}

	LOG_TRN("Received XT %d: %s", xt_index_in, (বর্ণ *)name);

	যদি (চাবি_names[i] != রিক্ত)
	{
	    i = (code[7] == '=') ? 8 : 10;
	    প্রতি (; (c = hexhex2nr(code + i)) >= 0; i += 2)
		str[j++] = c;
	    str[j] = NUL;
	    যদি (name[0] == 'C' && name[1] == 'o')
	    {
		// Color count is not a key code.
		পূর্ণাঙ্ক val = atoi((বর্ণ *)str);
# যদি defined(FEAT_EVAL)
		যদি (val == t_colors)
		    ch_log(রিক্ত, "got_code_from_term(Co): no change (%d)", val);
		অন্যথা
		    ch_log(রিক্ত,
			       "got_code_from_term(Co): changed from %d to %d",
								t_colors, val);
# endif
		may_adjust_color_গুনতি(val);
	    }
# যদি 0
#  ifdef FEAT_TERMGUICOLORS
	    // when RGB result comes back, it is supported when the result contains an '='
	    অন্যথা যদি (name[0] == 'R' && name[1] == 'G' && name[2] == 'B' && code[9] == '=')
	    {
		পূর্ণাঙ্ক val = atoi((বর্ণ *)str);
		// only enable it, if termguicolors hasn't been set yet and
		// there are 8 bits per color channel
		যদি (val == 8 && !p_tgc_set)
		{
#   ifdef FEAT_EVAL
		    ch_log(রিক্ত, "got_code_from_term(RGB): xterm-direct colors detected");
#   endif
		    // RGB capability set, enable termguicolors
		    set_option_মান((char_u *)"termguicolors", 1L, রিক্ত, 0);
		}
	    }
#  endif
# endif
	    অন্যথা
	    {
		i = find_term_bykeys(str, রিক্ত);
		যদি (i >= 0 && name[0] == termcodes[i].name[0]
					    && name[1] == termcodes[i].name[1])
		{
		    // Existing entry with the same name and code - skip.
# ifdef FEAT_EVAL
		    ch_log(রিক্ত, "got_code_from_term(): Entry %c%c did not change",
							     name[0], name[1]);
# endif
		}
		অন্যথা যদি (i >= 0 && name[0] == 'K' && VIM_ISDIGIT(name[1]))
		{
		    // Would replace existing entry with keypad key - skip.
# ifdef FEAT_EVAL
		    ch_log(রিক্ত, "got_code_from_term(): Skipping entry %c%c in favor of %c%c with matching keys %s",
			    name[0], name[1], termcodes[i].name[0], termcodes[i].name[1], str);
# endif
		}
		অন্যথা
		{
		    যদি (i >= 0)
		    {
			// Delete an existing entry using the same code.
# ifdef FEAT_EVAL
			ch_log(রিক্ত, "got_code_from_term(): Deleting entry %c%c with matching keys %s",
			      termcodes[i].name[0], termcodes[i].name[1], str);
# endif
			del_termcode_idx(i);
		    }
# ifdef FEAT_EVAL
		    অন্যথা
			ch_log(রিক্ত, "got_code_from_term(): Adding entry %c%c with keys %s",
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
    স্থির শূন্য
check_for_codes_from_term(শূন্য)
{
    পূর্ণাঙ্ক		c;

    // If no codes requested or all are answered, no need to wait.
    যদি (xt_index_out == 0 || xt_index_out == xt_index_in)
	প্রত্যাগম;

    // Vgetc() will check for and handle any response.
    // Keep calling vpeekc() until we don't get any responses.
    ++no_mapping;
    ++allow_চাবিগুলি;
    প্রতি (;;)
    {
	c = vpeekc();
	যদি (c == NUL)	    // nothing available
	    ভঙ্গ;

	// If a response is recognized it's replaced with K_IGNORE, must read
	// it from the input stream.  If there is no K_IGNORE we can't do
	// anything, break here (there might be some responses further on, but
	// we don't want to throw away any typed chars).
	যদি (c != K_SPECIAL && c != K_IGNORE)
	    ভঙ্গ;
	c = vgetc();
	যদি (c != K_IGNORE)
	{
	    vungetc(c);
	    ভঙ্গ;
	}
    }
    --no_mapping;
    --allow_চাবিগুলি;
}
#endif

#যদি defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
স্থির বর্ণ ksme_str[20];
স্থির বর্ণ ksmr_str[20];
স্থির বর্ণ ksmd_str[20];

/*
 * For Win32 console: update termcap codes for existing console attributes.
 */
    শূন্য
update_tcap(পূর্ণাঙ্ক attr)
{
    sprintf(ksme_str, "\033|%dm", attr);
    sprintf(ksmd_str, "\033|%dm", attr | 0x08);  // FOREGROUND_INTENSITY
    sprintf(ksmr_str, "\033|%dm", ((attr & 0x0F) << 4) | ((attr & 0xF0) >> 4));

    tcap_entry_T *p = find_builtin_term(DEFAULT_TERM);
    যদি (p == রিক্ত)  // did not find it
	প্রত্যাগম;
    যাবত্ (p->bt_string != রিক্ত)
    {
	যদি (p->bt_entry == (পূর্ণাঙ্ক)KS_ME)
	    p->bt_string = &ksme_str[0];
	অন্যথা যদি (p->bt_entry == (পূর্ণাঙ্ক)KS_MR)
	    p->bt_string = &ksmr_str[0];
	অন্যথা যদি (p->bt_entry == (পূর্ণাঙ্ক)KS_MD)
	    p->bt_string = &ksmd_str[0];
	++p;
    }
}

# ifdef FEAT_TERMGUICOLORS
#  define KSSIZE 20

প্রকারনির্দেশ গণনা
{
    CMODE_INDEXED = 0,	// Use cmd.exe 4bit palette.
    CMODE_RGB,		// Use 24bit RGB colors using VTP.
    CMODE_256COL,	// Emulate xterm's 256-color palette using VTP.
    CMODE_LAST,
} cmode_T;

সংরচনা ks_tbl_S
{
    পূর্ণাঙ্ক  code;				// value of KS_
    বর্ণ *vtp;				// code in RGB mode
    বর্ণ *vtp2;				// code in 256color mode
    বর্ণ buf[CMODE_LAST][KSSIZE];	// real buffer
};

স্থির সংরচনা ks_tbl_S ks_tbl[] =
{
    {(পূর্ণাঙ্ক)KS_ME,  "\033|0m",  "\033|0m", {""}},   // normal
    {(পূর্ণাঙ্ক)KS_MR,  "\033|7m",  "\033|7m", {""}},   // reverse
    {(পূর্ণাঙ্ক)KS_MD,  "\033|1m",  "\033|1m", {""}},   // bold
    {(পূর্ণাঙ্ক)KS_SO,  "\033|91m", "\033|91m", {""}},  // standout: bright red text
    {(পূর্ণাঙ্ক)KS_SE,  "\033|39m", "\033|39m", {""}},  // standout end: default color
    {(পূর্ণাঙ্ক)KS_CZH, "\033|3m",  "\033|3m", {""}},   // italic
    {(পূর্ণাঙ্ক)KS_CZR, "\033|0m",  "\033|0m", {""}},   // italic end
    {(পূর্ণাঙ্ক)KS_US,  "\033|4m",  "\033|4m", {""}},   // underscore
    {(পূর্ণাঙ্ক)KS_UE,  "\033|24m", "\033|24m", {""}},  // underscore end
#  ifdef TERMINFO
    {(পূর্ণাঙ্ক)KS_CAB, "\033|%p1%db", "\033|%p14%dm", {""}}, // set background color
    {(পূর্ণাঙ্ক)KS_CAF, "\033|%p1%df", "\033|%p13%dm", {""}}, // set foreground color
    {(পূর্ণাঙ্ক)KS_CS,  "\033|%p1%d;%p2%dR", "\033|%p1%d;%p2%dR", {""}},
    {(পূর্ণাঙ্ক)KS_CSV, "\033|%p1%d;%p2%dV", "\033|%p1%d;%p2%dV", {""}},
#  অন্যথা
    {(পূর্ণাঙ্ক)KS_CAB, "\033|%db", "\033|4%dm", {""}}, // set background color
    {(পূর্ণাঙ্ক)KS_CAF, "\033|%df", "\033|3%dm", {""}}, // set foreground color
    {(পূর্ণাঙ্ক)KS_CS,  "\033|%d;%dR", "\033|%d;%dR", {""}},
    {(পূর্ণাঙ্ক)KS_CSV, "\033|%d;%dV", "\033|%d;%dV", {""}},
#  endif
    {(পূর্ণাঙ্ক)KS_CCO, "256", "256", {""}},	    // colors
    {(পূর্ণাঙ্ক)KS_NAME, রিক্ত, রিক্ত, {""}}			    // terminator
};

/*
 * Find the first entry for "code" in the builtin termcap for "name".
 * Returns NULL when not found.
 */
    স্থির tcap_entry_T *
find_first_tcap(
    char_u *name,
    পূর্ণাঙ্ক	    code)
{
    tcap_entry_T *p = find_builtin_term(name);
    যদি (p == রিক্ত)
	প্রত্যাগম রিক্ত;
    যাবত্ (p->bt_string != রিক্ত)
    {
	যদি (p->bt_entry == code)
	    প্রত্যাগম p;
	++p;
    }
    প্রত্যাগম রিক্ত;
}
# endif

/*
 * For Win32 console: replace the sequence immediately after termguicolors.
 */
    শূন্য
swap_tcap(শূন্য)
{
# ifdef FEAT_TERMGUICOLORS
    স্থির পূর্ণাঙ্ক		init_done = FALSE;
    স্থির cmode_T	curr_mode;
    সংরচনা ks_tbl_S	*ks;
    cmode_T		mode;

    যদি (!init_done)
    {
	প্রতি (ks = ks_tbl; ks->code != (পূর্ণাঙ্ক)KS_NAME; ks++)
	{
	    tcap_entry_T *bt = find_first_tcap(DEFAULT_TERM, ks->code);
	    যদি (bt != রিক্ত)
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

    যদি (p_tgc)
	mode = CMODE_RGB;
    অন্যথা যদি (t_colors >= 256)
	mode = CMODE_256COL;
    অন্যথা
	mode = CMODE_INDEXED;

    যদি (mode == curr_mode)
	প্রত্যাগম;

    প্রতি (ks = ks_tbl; ks->code != (পূর্ণাঙ্ক)KS_NAME; ks++)
    {
	tcap_entry_T *bt = find_first_tcap(DEFAULT_TERM, ks->code);
	যদি (bt != রিক্ত)
	    bt->bt_string = ks->buf[mode];
    }

    curr_mode = mode;
# endif
}

#endif


#যদি (defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))) || defined(FEAT_TERMINAL)
স্থির ধ্রুব পূর্ণাঙ্ক cube_মান[] = {
    0x00, 0x5F, 0x87, 0xAF, 0xD7, 0xFF
};

স্থির ধ্রুব পূর্ণাঙ্ক grey_ramp[] = {
    0x08, 0x12, 0x1C, 0x26, 0x30, 0x3A, 0x44, 0x4E, 0x58, 0x62, 0x6C, 0x76,
    0x80, 0x8A, 0x94, 0x9E, 0xA8, 0xB2, 0xBC, 0xC6, 0xD0, 0xDA, 0xE4, 0xEE
};

স্থির ধ্রুব char_u ansi_table[16][3] = {
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

# যদি defined(MSWIN)
// Mapping between cterm indices < 16 and their counterpart in the ANSI palette.
ধ্রুব char_u cterm_ansi_idx[] = {
    0, 4, 2, 6, 1, 5, 3, 7, 8, 12, 10, 14, 9, 13, 11, 15
};
# endif

# define ANSI_INDEX_NONE 0

    শূন্য
ansi_color2rgb(পূর্ণাঙ্ক nr, char_u *r, char_u *g, char_u *b, char_u *ansi_idx)
{
    যদি (nr < 16)
    {
	*r = ansi_table[nr][0];
	*g = ansi_table[nr][1];
	*b = ansi_table[nr][2];
	*ansi_idx = nr + 1;
    }
    অন্যথা
    {
	*r = 0;
	*g = 0;
	*b = 0;
	*ansi_idx = ANSI_INDEX_NONE;
    }
}

    শূন্য
cterm_color2rgb(পূর্ণাঙ্ক nr, char_u *r, char_u *g, char_u *b, char_u *ansi_idx)
{
    পূর্ণাঙ্ক idx;

    যদি (nr < 16)
    {
# যদি defined(MSWIN)
	idx = cterm_ansi_idx[nr];
# অন্যথা
	idx = nr;
# endif
	*r = ansi_table[idx][0];
	*g = ansi_table[idx][1];
	*b = ansi_table[idx][2];
	*ansi_idx = idx + 1;
    }
    অন্যথা যদি (nr < 232)
    {
	// 216 color cube
	idx = nr - 16;
	*r = cube_মান[idx / 36 % 6];
	*g = cube_মান[idx / 6  % 6];
	*b = cube_মান[idx      % 6];
	*ansi_idx = ANSI_INDEX_NONE;
    }
    অন্যথা যদি (nr < 256)
    {
	// 24 grey scale ramp
	idx = nr - 232;
	*r = grey_ramp[idx];
	*g = grey_ramp[idx];
	*b = grey_ramp[idx];
	*ansi_idx = ANSI_INDEX_NONE;
    }
    অন্যথা
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
    পূর্ণাঙ্ক
term_replace_keycodes(char_u *ta_buf, পূর্ণাঙ্ক ta_len, পূর্ণাঙ্ক len_arg)
{
    পূর্ণাঙ্ক		len = len_arg;
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		c;

    প্রতি (i = ta_len; i < ta_len + len; ++i)
    {
	যদি (ta_buf[i] == CSI && len - i > 3 && ta_buf[i + 1] == KS_MODIFIER)
	{
	    পূর্ণাঙ্ক modifiers = ta_buf[i + 2];
	    পূর্ণাঙ্ক চাবি = ta_buf[i + 3];

	    // Try to use the modifier to modify the key.  In any case drop the
	    // modifier.
	    mch_memmove(ta_buf + i + 1, ta_buf + i + 4, (size_t)(len - i - 3));
	    len -= 3;
	    যদি (চাবি < 0x80)
		চাবি = merge_modify_Other_চাবিগুলি(চাবি, &modifiers);
	    ta_buf[i] = চাবি;
	}
	অন্যথা যদি (ta_buf[i] == CSI && len - i > 2)
	{
	    c = TERMCAP2_চাবি(ta_buf[i + 1], ta_buf[i + 2]);
	    যদি (c == K_DEL || c == K_KDEL || c == K_BS)
	    {
		mch_memmove(ta_buf + i + 1, ta_buf + i + 3,
							(size_t)(len - i - 2));
		যদি (c == K_DEL || c == K_KDEL)
		    ta_buf[i] = DEL;
		অন্যথা
		    ta_buf[i] = Ctrl_H;
		len -= 2;
	    }
	}
	অন্যথা যদি (ta_buf[i] == '\r')
	    ta_buf[i] = '\n';
	যদি (has_mbyte)
	    i += (*mb_ptr2len_len)(ta_buf + i, ta_len + len - i) - 1;
    }
    প্রত্যাগম len;
}

/*
 * Should be called when cleaning up terminal state.
 */
    শূন্য
term_disable_dec(শূন্য)
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
    শূন্য
term_set_win_resize(bool state)
{
# ifdef FEAT_GUI
    bool    in_gui = gui.in_use;

    যদি (state && in_gui)
	প্রত্যাগম;
# endif

    যদি (!state || win_resize_setting == 0 || win_resize_setting == 4)
    {
	// Make sure it update internal window size if DEC mode 2048 is
	// unavailable now.
	যদি (win_resize_enabled)
	{
	    set_shellsize(0, 0, false);
	    set_sigwinch_handler();
	    out_str((char_u *)"\033[?2048l");
	}
	win_resize_enabled = false;
    }
    অন্যথা যদি ((*p_trz == NUL || STRCMP(p_trz, "inband") == 0)
	    && !win_resize_enabled)
    {
	যদি (win_resize_setting == 2)
	    out_str((char_u *)"\033[?2048h");
# ifdef SIGWINCH
	mch_signal(SIGWINCH, SIG_DFL);
# endif
	win_resize_enabled = true;
    }
}
#endif

    পূর্ণাঙ্ক
sync_output_active(শূন্য)
{
#ifdef FEAT_GUI
    যদি (gui.in_use)
	প্রত্যাগম TRUE;
#endif
    প্রত্যাগম p_tsy && (sync_output_setting == 1 || sync_output_setting == 2)
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
    শূন্য
term_set_sync_output(পূর্ণাঙ্ক flags)
{
    bool    allowed;
#ifdef FEAT_GUI
    bool    in_gui = gui.in_use;
#অন্যথা
    bool    in_gui = false;
#endif

    allowed = p_tsy && (sync_output_setting == 1 || sync_output_setting == 2);

    যদি (flags & TERM_SYNC_OUTPUT_FLUSH)
    {
	// Tell terminal to display screen contents, then resume batching.
	যদি (allowed && !in_gui && sync_output_state > 0 && *T_ESU != NUL &&
		*T_BSU != NUL)
	{
	    out_str_nf(T_ESU);
	    out_str_nf(T_BSU);
	    out_flush();
	}
	প্রত্যাগম;
    }

    // Forcibly turn off synchronized output (e.g. 'notermsync')
    যদি (flags & TERM_SYNC_OUTPUT_OFF)
    {
	যদি (sync_output_state > 0 && *T_ESU != NUL)
	{
	    out_str_nf(T_ESU);
	    out_flush();
	    sync_output_state = 0;
	}
	প্রত্যাগম;
    }

    যদি (!allowed || in_gui || *T_BSU == NUL || *T_ESU == NUL)
	প্রত্যাগম;

    // Only enable if we aren't already, and only disable if we have reached
    // zero.
    যদি (flags & TERM_SYNC_OUTPUT_ENABLE)
    {
	যদি (sync_output_state++ > 0)
	    প্রত্যাগম;
	out_str_nf(T_BSU);
    }
    অন্যথা যদি (flags & TERM_SYNC_OUTPUT_DISABLE)
    {
	যদি (sync_output_state == 0 || --sync_output_state > 0)
	    প্রত্যাগম;
	// Flush the output buffer before ending the sync batch so that
	// all drawing output is sent to the terminal within the
	// BSU..ESU window.  Without this, the drawing data remaining in
	// out_buf would be sent after ESU, outside the sync batch.
	out_str_nf(T_ESU);
	out_flush();
    }
    অন্যথা
    {
	siemsg("Unknown sync output value %d", flags);
	প্রত্যাগম;
    }
}
