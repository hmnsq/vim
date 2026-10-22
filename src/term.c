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
#నిర్వచించు tgetstr tgetstr_defined_wrong

#అన్తర్భావ "vim.h"

#నిర్వచితమైతే HAVE_TGETENT
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
# ఒకవేళ defined(VMS) || defined(AMIGA)
#  define TPUTSFUNCAST (శూన్య (*)(గుర్తులేని పూర్ణాంకం))
# లేకపోతే
#  ifdef HAVE_OUTFUNTYPE
#   define TPUTSFUNCAST (outfuntype)
#  లేకపోతే
#   define TPUTSFUNCAST (పూర్ణాంకం (*)(పూర్ణాంకం))
#  endif
# endif
#అయితే_ముగింపు

#నిర్వచనం_రద్దు tgetstr

// start of keys that are not directly used by Vim but can be mapped
#నిర్వచించు BT_EXTRA_కీలకాలు	0x101

స్థిర శూన్య parse_builtin_tcap(char_u *s);
స్థిర శూన్య gather_termleader(శూన్య);
#నిర్వచితమైతే FEAT_TERMRESPONSE
స్థిర శూన్య req_codes_from_పదం(శూన్య);
స్థిర శూన్య req_మరిన్ని_codes_from_పదం(శూన్య);
స్థిర శూన్య got_సంకేతం_from_పదం(char_u *సంకేతం_2, పూర్ణాంకం len);
స్థిర శూన్య check_for_codes_from_పదం(శూన్య);
#అయితే_ముగింపు
స్థిర శూన్య del_termcode_idx(పూర్ణాంకం idx);
స్థిర పూర్ణాంకం కనుగొను_పదం_bykeys(char_u *src, పూర్ణాంకం *len);
స్థిర పూర్ణాంకం పదం_is_builtin(char_u *name);
స్థిర పూర్ణాంకం పదం_7to8bit(char_u *p);
స్థిర శూన్య accept_modifiers_for_function_కీలకాలు(శూన్య);

#ఒకవేళ 0  // Change to 1 to enable ch_log() calls for termresponse debugging.
# define DEBUG_TERMRESPONSE
# define LOG_TR1(str) \
		ch_log(ఖాళీ, "TermResp: %s " str, \
			must_redraw == UPD_NOT_VALID ? "NV" \
			: must_redraw == UPD_CLEAR ? "CL" : "  ")
# define LOG_TRN(fmt,...) \
		ch_log(ఖాళీ, "TermResp: %s " fmt, \
			must_redraw == UPD_NOT_VALID ? "NV" \
			: must_redraw == UPD_CLEAR ? "CL" : "  ", __VA_ARGS__)
#లేకపోతే
# define LOG_TR1(str) చేయి { /**/ } అయితే (0)
# define LOG_TRN(fmt,...) చేయి { /**/ } అయితే (0)
#అయితే_ముగింపు

#నిర్వచితమైతే HAVE_TGETENT
స్థిర అక్షరం *invoke_tgetent(char_u *, char_u *);

/*
 * Here is our own prototype for tgetstr(), any prototypes from the include
 * files have been disabled by the define at the start of this file.
 */
అక్షరం		*tgetstr(అక్షరం *, అక్షరం **);
#అయితే_ముగింపు

రకనిర్వచనం గణన {
    స్థితి_GET,		// send request when switching to RAW mode
    స్థితి_పంపిన,	// did send request, checking for response
    స్థితి_GOT,		// received response
    స్థితి_FAIL		// timed out
} request_పురోగతి_T;

రకనిర్వచనం నిర్మాణం {
    request_పురోగతి_T	    tr_పురోగతి;
    time_t		    tr_ప్రారంభం;	// when request was sent, -1 for never
} termrequest_T;

// Holds state for current OSC response.
రకనిర్వచనం నిర్మాణం
{
    పూర్ణాంకం		    processing;	// If we are in the middle of an OSC response
    char_u	    ప్రారంభం_char;	// First char in the OSC response
    garray_T	    buf;	// Buffer holding the OSC response, to be
				// placed in the "v:termosc" vim var.

#నిర్వచితమైతే ELAPSED_FUNC
    elapsed_T	    ప్రారంభం_tv;	// Set at the beginning of an OSC response.
				// Used to timeout after a set amount of
				// time.
#అయితే_ముగింపు
} oscstate_T;

#నిర్వచించు TERMREQUEST_INIT {స్థితి_GET, -1}

// Request Terminal Version status:
స్థిర termrequest_T crv_స్థితి = TERMREQUEST_INIT;

// Request Cursor position report:
స్థిర termrequest_T u7_స్థితి = TERMREQUEST_INIT;

// Request xterm compatibility check:
స్థిర termrequest_T xcc_స్థితి = TERMREQUEST_INIT;

#నిర్వచితమైతే FEAT_TERMRESPONSE
# ifdef FEAT_TERMINAL
// Request foreground color report:
స్థిర termrequest_T rfg_స్థితి = TERMREQUEST_INIT;
స్థిర పూర్ణాంకం fg_r = 0;
స్థిర పూర్ణాంకం fg_g = 0;
స్థిర పూర్ణాంకం fg_b = 0;
# endif
// Background color values from the OSC 11 response, also used by the
// image backend to flatten RGBA alpha onto the actual terminal background.
స్థిర పూర్ణాంకం bg_r = 255;
స్థిర పూర్ణాంకం bg_g = 255;
స్థిర పూర్ణాంకం bg_b = 255;

// Request background color report:
స్థిర termrequest_T rbg_స్థితి = TERMREQUEST_INIT;

// Request cursor blinking mode report:
స్థిర termrequest_T rbm_స్థితి = TERMREQUEST_INIT;

// Request cursor style report:
స్థిర termrequest_T rcs_స్థితి = TERMREQUEST_INIT;

// Request window's position report:
స్థిర termrequest_T winpos_స్థితి = TERMREQUEST_INIT;

// Request DECRQM (DEC mode) report:
స్థిర termrequest_T decrqm_స్థితి = TERMREQUEST_INIT;

స్థిర termrequest_T *all_termrequests[] = {
    &crv_స్థితి,
    &u7_స్థితి,
    &xcc_స్థితి,
# ifdef FEAT_TERMINAL
    &rfg_స్థితి,
# endif
    &rbg_స్థితి,
    &rbm_స్థితి,
    &rcs_స్థితి,
    &winpos_స్థితి,
    &decrqm_స్థితి,
    ఖాళీ
};

// The t_8u code may default to a value but get reset when the term response is
// received.  To avoid redrawing too often, only redraw when t_8u is not reset
// and it was supposed to be written.  Unless t_8u was set explicitly.
// FALSE -> don't output t_8u yet
// MAYBE -> tried outputting t_8u while FALSE
// OK    -> can write t_8u
పూర్ణాంకం write_t_8u_state = FALSE;
#అయితే_ముగింపు

#నిర్వచితమైతే HAVE_TGETENT
/*
 * Don't declare these variables if termcap.h contains them.
 * Autoconf checks if these variables should be declared extern (not all
 * systems have them).
 * Some versions define ospeed to be speed_t, but that is incompatible with
 * BSD, where ospeed is short and speed_t is long.
 */
# ifndef HAVE_OSPEED
#  ifdef OSPEED_EXTERN
బాహ్య పొట్టి ospeed;
#  లేకపోతే
పొట్టి ospeed;
#  endif
# endif
# ifndef HAVE_పైకి_BC_PC
#  ifdef పైకి_BC_PC_EXTERN
బాహ్య అక్షరం *పైకి_2, *BC, PC;
#  లేకపోతే
అక్షరం *పైకి_2, *BC, PC;
#  endif
# endif

# define TGETSTR(s, p)	vim_tgetstr((s), (p))
# define TGETENT(b, t)	tgetent((అక్షరం *)(b), (అక్షరం *)(t))
స్థిర char_u *vim_tgetstr(అక్షరం *s, char_u **pp);
#అయితే_ముగింపు // HAVE_TGETENT

స్థిర పూర్ణాంకం  detected_8bit = FALSE;	// detected 8-bit terminal

#ఒకవేళ (defined(UNIX) || defined(VMS))
స్థిర పూర్ణాంకం focus_state = MAYBE; // TRUE if the Vim window has focus
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_TERMRESPONSE
// When the cursor shape was detected these values are used:
// 1: block, 2: underline, 3: vertical bar
స్థిర పూర్ణాంకం initial_cursor_shape = 0;

// The blink flag from the style response may be inverted from the actual
// blinking state, xterm XORs the flags.
స్థిర పూర్ణాంకం initial_cursor_shape_blink = FALSE;

// The blink flag from the blinking-cursor mode response
స్థిర పూర్ణాంకం initial_cursor_blink = FALSE;
#అయితే_ముగింపు

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
స్థిర పూర్ణాంకం sync_output_setting = 0;

// > 0: Currently batching output
// == 0: No synchronized output
స్థిర పూర్ణాంకం sync_output_state = 0;

#నిర్వచితమైతే UNIX
// DEC mode 2048 (in-band window resize events)
// https://gist.github.com/rockorager/e695fb2924d36b2bcf1fff4a3704bd83
స్థిర పూర్ణాంకం win_resize_setting = 0;
స్థిర bool win_resize_enabled = false;
#అయితే_ముగింపు

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
రకనిర్వచనం నిర్మాణం
{
    పూర్ణాంకం		bt_నమోదు;   // either a KS_xxx code (>= 0), or a K_xxx code.
    అక్షరం	*bt_string; //విలువ
} tcap_నమోదు_T;

/*
 * Standard ANSI terminal, default for Unix.
 */
స్థిర tcap_నమోదు_T builtin_ansi[] = {
    {(పూర్ణాంకం)KS_CE,	"\033[K"},
    {(పూర్ణాంకం)KS_AL,	"\033[L"},
#నిర్వచితమైతే TERMINFO
    {(పూర్ణాంకం)KS_CAL,	"\033[%p1%dL"},
#లేకపోతే
    {(పూర్ణాంకం)KS_CAL,	"\033[%dL"},
#అయితే_ముగింపు
    {(పూర్ణాంకం)KS_DL,	"\033[M"},
#నిర్వచితమైతే TERMINFO
    {(పూర్ణాంకం)KS_CDL,	"\033[%p1%dM"},
#లేకపోతే
    {(పూర్ణాంకం)KS_CDL,	"\033[%dM"},
#అయితే_ముగింపు
    {(పూర్ణాంకం)KS_CL,	"\033[H\033[2J"},
    {(పూర్ణాంకం)KS_ME,	"\033[0m"},
    {(పూర్ణాంకం)KS_MR,	"\033[7m"},
    {(పూర్ణాంకం)KS_MS,	"y"},
    {(పూర్ణాంకం)KS_UT,	"y"},		// guessed
    {(పూర్ణాంకం)KS_LE,	"\b"},
#నిర్వచితమైతే TERMINFO
    {(పూర్ణాంకం)KS_CM,	"\033[%i%p1%d;%p2%dH"},
#లేకపోతే
    {(పూర్ణాంకం)KS_CM,	"\033[%i%d;%dH"},
#అయితే_ముగింపు
#నిర్వచితమైతే TERMINFO
    {(పూర్ణాంకం)KS_CRI,	"\033[%p1%dC"},
#లేకపోతే
    {(పూర్ణాంకం)KS_CRI,	"\033[%dC"},
#అయితే_ముగింపు

    {(పూర్ణాంకం)KS_NAME,	ఖాళీ}  // end marker
};

/*
 * VT320 is working as an ANSI terminal compatible DEC terminal.
 * (it covers VT1x0, VT2x0 and VT3x0 up to VT320 on VMS as well)
 * TODO:- rewrite ESC[ codes to CSI
 *      - keyboard languages (CSI ? 26 n)
 */
స్థిర tcap_నమోదు_T builtin_vt320[] = {
    {(పూర్ణాంకం)KS_CE,	"\033[K"},
    {(పూర్ణాంకం)KS_AL,	"\033[L"},
#నిర్వచితమైతే TERMINFO
    {(పూర్ణాంకం)KS_CAL,	"\033[%p1%dL"},
#లేకపోతే
    {(పూర్ణాంకం)KS_CAL,	"\033[%dL"},
#అయితే_ముగింపు
    {(పూర్ణాంకం)KS_DL,	"\033[M"},
#నిర్వచితమైతే TERMINFO
    {(పూర్ణాంకం)KS_CDL,	"\033[%p1%dM"},
#లేకపోతే
    {(పూర్ణాంకం)KS_CDL,	"\033[%dM"},
#అయితే_ముగింపు
    {(పూర్ణాంకం)KS_CL,	"\033[H\033[2J"},
    {(పూర్ణాంకం)KS_CD,	"\033[J"},
    {(పూర్ణాంకం)KS_CCO,	"8"},			// allow 8 colors
    {(పూర్ణాంకం)KS_ME,	"\033[0m"},
    {(పూర్ణాంకం)KS_MR,	"\033[7m"},
    {(పూర్ణాంకం)KS_MD,	"\033[1m"},  // bold mode
    {(పూర్ణాంకం)KS_SE,	"\033[22m"},// normal mode
    {(పూర్ణాంకం)KS_UE,	"\033[24m"},// exit underscore mode
    {(పూర్ణాంకం)KS_US,	"\033[4m"},  // underscore mode
    {(పూర్ణాంకం)KS_CZH,	"\033[34;43m"},  // italic mode: blue text on yellow
    {(పూర్ణాంకం)KS_CZR,	"\033[0m"},	    // italic mode end
    {(పూర్ణాంకం)KS_CAB,	"\033[4%dm"},	    // set background color (ANSI)
    {(పూర్ణాంకం)KS_CAF,	"\033[3%dm"},	    // set foreground color (ANSI)
    {(పూర్ణాంకం)KS_CSB,	"\033[102;%dm"},	// set screen background color
    {(పూర్ణాంకం)KS_CSF,	"\033[101;%dm"},	// set screen foreground color
    {(పూర్ణాంకం)KS_MS,	"y"},
    {(పూర్ణాంకం)KS_UT,	"y"},
    {(పూర్ణాంకం)KS_XN,	"y"},
    {(పూర్ణాంకం)KS_LE,	"\b"},
#నిర్వచితమైతే TERMINFO
    {(పూర్ణాంకం)KS_CM,	"\033[%i%p1%d;%p2%dH"},
#లేకపోతే
    {(పూర్ణాంకం)KS_CM,	"\033[%i%d;%dH"},
#అయితే_ముగింపు
#నిర్వచితమైతే TERMINFO
    {(పూర్ణాంకం)KS_CRI,	"\033[%p1%dC"},
#లేకపోతే
    {(పూర్ణాంకం)KS_CRI,	"\033[%dC"},
#అయితే_ముగింపు
    {K_పైకి,		"\033[A"},
    {K_కిందికి,		"\033[B"},
    {K_కుడి,		"\033[C"},
    {K_ఎడమ,		"\033[D"},
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
    {K_చివర,		"\033[4~"},
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

    {(పూర్ణాంకం)KS_NAME,	ఖాళీ}  // end marker
};

/*
 * Ordinary vt52
 */
స్థిర tcap_నమోదు_T builtin_vt52[] = {
    {(పూర్ణాంకం)KS_CE,	"\033K"},
    {(పూర్ణాంకం)KS_CD,	"\033J"},
#నిర్వచితమైతే TERMINFO
    {(పూర్ణాంకం)KS_CM,	"\033Y%p1%' '%+%c%p2%' '%+%c"},
#లేకపోతే
    {(పూర్ణాంకం)KS_CM,	"\033Y%+ %+ "},
#అయితే_ముగింపు
    {(పూర్ణాంకం)KS_LE,	"\b"},
    {(పూర్ణాంకం)KS_SR,	"\033I"},
    {(పూర్ణాంకం)KS_AL,	"\033L"},
    {(పూర్ణాంకం)KS_DL,	"\033M"},
    {K_పైకి,		"\033A"},
    {K_కిందికి,		"\033B"},
    {K_ఎడమ,		"\033D"},
    {K_కుడి,		"\033C"},
    {K_F1,		"\033P"},
    {K_F2,		"\033Q"},
    {K_F3,		"\033R"},
    {(పూర్ణాంకం)KS_CL,	"\033H\033J"},
    {(పూర్ణాంకం)KS_MS,	"y"},

    {(పూర్ణాంకం)KS_NAME,	ఖాళీ}  // end marker
};

/*
 * Builtin xterm with Vim-specific entries.
 */
స్థిర tcap_నమోదు_T builtin_xterm[] = {
    {(పూర్ణాంకం)KS_CE,	"\033[K"},
    {(పూర్ణాంకం)KS_AL,	"\033[L"},
#నిర్వచితమైతే TERMINFO
    {(పూర్ణాంకం)KS_CAL,	"\033[%p1%dL"},
#లేకపోతే
    {(పూర్ణాంకం)KS_CAL,	"\033[%dL"},
#అయితే_ముగింపు
    {(పూర్ణాంకం)KS_DL,	"\033[M"},
#నిర్వచితమైతే TERMINFO
    {(పూర్ణాంకం)KS_CDL,	"\033[%p1%dM"},
#లేకపోతే
    {(పూర్ణాంకం)KS_CDL,	"\033[%dM"},
#అయితే_ముగింపు
#నిర్వచితమైతే TERMINFO
    {(పూర్ణాంకం)KS_CS,	"\033[%i%p1%d;%p2%dr"},
#లేకపోతే
    {(పూర్ణాంకం)KS_CS,	"\033[%i%d;%dr"},
#అయితే_ముగింపు
    {(పూర్ణాంకం)KS_CL,	"\033[H\033[2J"},
    {(పూర్ణాంకం)KS_CD,	"\033[J"},
    {(పూర్ణాంకం)KS_ME,	"\033[m"},
    {(పూర్ణాంకం)KS_MR,	"\033[7m"},
    {(పూర్ణాంకం)KS_MD,	"\033[1m"},
    {(పూర్ణాంకం)KS_UE,	"\033[m"},
    {(పూర్ణాంకం)KS_US,	"\033[4m"},
    {(పూర్ణాంకం)KS_STE,	"\033[29m"},
    {(పూర్ణాంకం)KS_STS,	"\033[9m"},
    {(పూర్ణాంకం)KS_MS,	"y"},
    {(పూర్ణాంకం)KS_UT,	"y"},
    {(పూర్ణాంకం)KS_LE,	"\b"},
    {(పూర్ణాంకం)KS_VI,	"\033[?25l"},
    {(పూర్ణాంకం)KS_VE,	"\033[?25h"},
    {(పూర్ణాంకం)KS_VS,	"\033[?12h"},
    {(పూర్ణాంకం)KS_CVS,	"\033[?12l"},
#నిర్వచితమైతే TERMINFO
    {(పూర్ణాంకం)KS_CSH,	"\033[%p1%d q"},
#లేకపోతే
    {(పూర్ణాంకం)KS_CSH,	"\033[%d q"},
#అయితే_ముగింపు
    {(పూర్ణాంకం)KS_చక్రీయ_తనిఖీ,	"\033[?12$p"},
    {(పూర్ణాంకం)KS_CRS,	"\033P$q q\033\\"},
#నిర్వచితమైతే TERMINFO
    {(పూర్ణాంకం)KS_CM,	"\033[%i%p1%d;%p2%dH"},
#లేకపోతే
    {(పూర్ణాంకం)KS_CM,	"\033[%i%d;%dH"},
#అయితే_ముగింపు
    {(పూర్ణాంకం)KS_SR,	"\033M"},
#నిర్వచితమైతే TERMINFO
    {(పూర్ణాంకం)KS_CRI,	"\033[%p1%dC"},
#లేకపోతే
    {(పూర్ణాంకం)KS_CRI,	"\033[%dC"},
#అయితే_ముగింపు
    {(పూర్ణాంకం)KS_KS,	"\033[?1h\033="},
    {(పూర్ణాంకం)KS_KE,	"\033[?1l\033>"},
#నిర్వచితమైతే FEAT_XTERM_SAVE
    {(పూర్ణాంకం)KS_TI,	"\0337\033[?47h"},
    {(పూర్ణాంకం)KS_TE,	"\033[?47l\0338"},
#అయితే_ముగింపు
    // These are now under control of the 'keyprotocol' option, see
    // "builtin_mok2".
    // {(int)KS_CTI,	"\033[>4;2m"},
    // {(int)KS_CRK,	"\033[?4m"},
    // {(int)KS_CTE,	"\033[>4;m"},
    {(పూర్ణాంకం)KS_CIS,	"\033]1;"},
    {(పూర్ణాంకం)KS_CIE,	"\007"},
    {(పూర్ణాంకం)KS_TS,	"\033]2;"},
    {(పూర్ణాంకం)KS_FS,	"\007"},
    {(పూర్ణాంకం)KS_CSC,	"\033]12;"},
    {(పూర్ణాంకం)KS_CEC,	"\007"},
#నిర్వచితమైతే TERMINFO
    {(పూర్ణాంకం)KS_CWS,	"\033[8;%p1%d;%p2%dt"},
    {(పూర్ణాంకం)KS_CWP,	"\033[3;%p1%d;%p2%dt"},
    {(పూర్ణాంకం)KS_CGP,	"\033[13t"},
#లేకపోతే
    {(పూర్ణాంకం)KS_CWS,	"\033[8;%d;%dt"},
    {(పూర్ణాంకం)KS_CWP,	"\033[3;%d;%dt"},
    {(పూర్ణాంకం)KS_CGP,	"\033[13t"},
#అయితే_ముగింపు
    {(పూర్ణాంకం)KS_CRV,	"\033[>c"},
    {(పూర్ణాంకం)KS_CXM,	"\033[?1006;1000%?%p1%{1}%=%th%el%;"},
    {(పూర్ణాంకం)KS_RFG,	"\033]10;?\007"},
    {(పూర్ణాంకం)KS_RBG,	"\033]11;?\007"},
    {(పూర్ణాంకం)KS_U7,	"\033[6n"},
    {(పూర్ణాంకం)KS_CAU,	"\033[58;5;%dm"},
    {(పూర్ణాంకం)KS_CBE,	"\033[?2004h"},
    {(పూర్ణాంకం)KS_CBD,	"\033[?2004l"},
    {(పూర్ణాంకం)KS_CST,	"\033[22;2t"},
    {(పూర్ణాంకం)KS_CRT,	"\033[23;2t"},
    {(పూర్ణాంకం)KS_SSI,	"\033[22;1t"},
    {(పూర్ణాంకం)KS_SRI,	"\033[23;1t"},
#ఒకవేళ (defined(UNIX) || defined(VMS))
    {(పూర్ణాంకం)KS_FD,	"\033[?1004l"},
    {(పూర్ణాంకం)KS_FE,	"\033[?1004h"},
#అయితే_ముగింపు

    {K_పైకి,		"\033O*A"},
    {K_కిందికి,		"\033O*B"},
    {K_కుడి,		"\033O*C"},
    {K_ఎడమ,		"\033O*D"},
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
    {K_చివర,		"\033[@;*F"},	// Esc [ F or Esc 1 ; F
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

    {BT_EXTRA_కీలకాలు,   ""},
    {TERMCAP2_కీలకం('k', '0'), "\033[10;*~"}, // F0
    {TERMCAP2_కీలకం('F', '3'), "\033[25;*~"}, // F13
    // F14 and F15 are missing, because they send the same codes as the undo
    // and help key, although they don't work on all keyboards.
    {TERMCAP2_కీలకం('F', '6'), "\033[29;*~"}, // F16
    {TERMCAP2_కీలకం('F', '7'), "\033[31;*~"}, // F17
    {TERMCAP2_కీలకం('F', '8'), "\033[32;*~"}, // F18
    {TERMCAP2_కీలకం('F', '9'), "\033[33;*~"}, // F19
    {TERMCAP2_కీలకం('F', 'A'), "\033[34;*~"}, // F20

    {TERMCAP2_కీలకం('F', 'B'), "\033[42;*~"}, // F21
    {TERMCAP2_కీలకం('F', 'C'), "\033[43;*~"}, // F22
    {TERMCAP2_కీలకం('F', 'D'), "\033[44;*~"}, // F23
    {TERMCAP2_కీలకం('F', 'E'), "\033[45;*~"}, // F24
    {TERMCAP2_కీలకం('F', 'F'), "\033[46;*~"}, // F25
    {TERMCAP2_కీలకం('F', 'G'), "\033[47;*~"}, // F26
    {TERMCAP2_కీలకం('F', 'H'), "\033[48;*~"}, // F27
    {TERMCAP2_కీలకం('F', 'I'), "\033[49;*~"}, // F28
    {TERMCAP2_కీలకం('F', 'J'), "\033[50;*~"}, // F29
    {TERMCAP2_కీలకం('F', 'K'), "\033[51;*~"}, // F30

    {TERMCAP2_కీలకం('F', 'L'), "\033[52;*~"}, // F31
    {TERMCAP2_కీలకం('F', 'M'), "\033[53;*~"}, // F32
    {TERMCAP2_కీలకం('F', 'N'), "\033[54;*~"}, // F33
    {TERMCAP2_కీలకం('F', 'O'), "\033[55;*~"}, // F34
    {TERMCAP2_కీలకం('F', 'P'), "\033[56;*~"}, // F35
    {TERMCAP2_కీలకం('F', 'Q'), "\033[57;*~"}, // F36
    {TERMCAP2_కీలకం('F', 'R'), "\033[58;*~"}, // F37

    {(పూర్ణాంకం)KS_NAME,	ఖాళీ}  // end marker
};

/*
 * Additions for using modifyOtherKeys level 2.  Same as what is used for
 * xterm.
 */
స్థిర tcap_నమోదు_T builtin_mok2[] = {
    // t_TI enables modifyOtherKeys level 2
    {(పూర్ణాంకం)KS_CTI,	"\033[>4;2m"},

    // XTQMODKEYS was added in xterm version 377: "CSI ? 4 m" which should
    // return "{lead} > 4 ; Pv m".  Before version 377 we expect it to have no
    // effect.
    {(పూర్ణాంకం)KS_CRK,	"\033[?4m"},

    // t_TE disables modifyOtherKeys
    {(పూర్ణాంకం)KS_CTE,	"\033[>4;m"},

    {(పూర్ణాంకం)KS_NAME,	ఖాళీ}  // end marker
};

/*
 * Additions for using the Kitty keyboard protocol.
 */
స్థిర tcap_నమోదు_T builtin_kitty[] = {
    // t_TI enables the kitty keyboard protocol.
    {(పూర్ణాంకం)KS_CTI,	"\033[=1;1u"},

    // t_RK requests the kitty keyboard protocol state
    {(పూర్ణాంకం)KS_CRK,	"\033[?u"},

    // t_TE also disables modifyOtherKeys, because t_TI from xterm may already
    // have been used.
    {(పూర్ణాంకం)KS_CTE,	"\033[>4;m\033[=0;1u"},

    // t_RF request terminal foreground color
    {(పూర్ణాంకం)KS_RFG,	"\033]10;?\033\\"},

    // t_RB request terminal background color
    {(పూర్ణాంకం)KS_RBG,	"\033]11;?\033\\"},

    {(పూర్ణాంకం)KS_NAME,	ఖాళీ}  // end marker
};

#నిర్వచితమైతే HAVE_TGETENT
/*
 * Additions for enabling/disabling synchronized output mode for terminal.
 */
స్థిర tcap_నమోదు_T builtin_sync_output[] = {
    {(పూర్ణాంకం)KS_BSU,	"\033[?2026h"},
    {(పూర్ణాంకం)KS_ESU,	"\033[?2026l"},
    {(పూర్ణాంకం)KS_NAME,	ఖాళీ}  // end marker
};
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_TERMRESPONSE
/*
 * List of DECRQM modes that Vim supports
 */
స్థిర స్థిరరాశి పూర్ణాంకం dec_modes[] = {
    2026,   // Synchronized output
# ifdef UNIX
    2048    // In-band terminal resize events
# endif
};
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_TERMGUICOLORS
/*
 * Additions for using the RGB colors and terminal font
 */
స్థిర tcap_నమోదు_T builtin_rgb[] = {
    // These are printf strings, not terminal codes.
    {(పూర్ణాంకం)KS_8F,	"\033[38;2;%lu;%lu;%lum"},
    {(పూర్ణాంకం)KS_8B,	"\033[48;2;%lu;%lu;%lum"},
    {(పూర్ణాంకం)KS_8U,	"\033[58;2;%lu;%lu;%lum"},

    {(పూర్ణాంకం)KS_NAME,	ఖాళీ}  // end marker
};
#అయితే_ముగింపు

#నిర్వచితమైతే HAVE_TGETENT
స్థిర tcap_నమోదు_T special_పదం[] = {
    // These are printf strings, not terminal codes.
    {(పూర్ణాంకం)KS_CF,	"\033[%dm"},
    {(పూర్ణాంకం)KS_NAME,	ఖాళీ}  // end marker
};
#అయితే_ముగింపు

/*
 * iris-ansi for Silicon Graphics machines.
 */
స్థిర tcap_నమోదు_T builtin_iris_ansi[] = {
    {(పూర్ణాంకం)KS_CE,	"\033[K"},
    {(పూర్ణాంకం)KS_CD,	"\033[J"},
    {(పూర్ణాంకం)KS_AL,	"\033[L"},
#నిర్వచితమైతే TERMINFO
    {(పూర్ణాంకం)KS_CAL,	"\033[%p1%dL"},
#లేకపోతే
    {(పూర్ణాంకం)KS_CAL,	"\033[%dL"},
#అయితే_ముగింపు
    {(పూర్ణాంకం)KS_DL,	"\033[M"},
#నిర్వచితమైతే TERMINFO
    {(పూర్ణాంకం)KS_CDL,	"\033[%p1%dM"},
#లేకపోతే
    {(పూర్ణాంకం)KS_CDL,	"\033[%dM"},
#అయితే_ముగింపు
#ఒకవేళ 0	// The scroll region is not working as Vim expects.
# ifdef TERMINFO
    {(పూర్ణాంకం)KS_CS,	"\033[%i%p1%d;%p2%dr"},
# లేకపోతే
    {(పూర్ణాంకం)KS_CS,	"\033[%i%d;%dr"},
# endif
#అయితే_ముగింపు
    {(పూర్ణాంకం)KS_CL,	"\033[H\033[2J"},
    {(పూర్ణాంకం)KS_VE,	"\033[9/y\033[12/y"},	// These aren't documented
    {(పూర్ణాంకం)KS_VS,	"\033[10/y\033[=1h\033[=2l"}, // These aren't documented
    {(పూర్ణాంకం)KS_TI,	"\033[=6h"},
    {(పూర్ణాంకం)KS_TE,	"\033[=6l"},
    {(పూర్ణాంకం)KS_SE,	"\033[21;27m"},
    {(పూర్ణాంకం)KS_SO,	"\033[1;7m"},
    {(పూర్ణాంకం)KS_ME,	"\033[m"},
    {(పూర్ణాంకం)KS_MR,	"\033[7m"},
    {(పూర్ణాంకం)KS_MD,	"\033[1m"},
    {(పూర్ణాంకం)KS_CCO,	"8"},			// allow 8 colors
    {(పూర్ణాంకం)KS_CZH,	"\033[3m"},		// italic mode on
    {(పూర్ణాంకం)KS_CZR,	"\033[23m"},		// italic mode off
    {(పూర్ణాంకం)KS_US,	"\033[4m"},		// underline on
    {(పూర్ణాంకం)KS_UE,	"\033[24m"},		// underline off
#నిర్వచితమైతే TERMINFO
    {(పూర్ణాంకం)KS_CAB,	"\033[4%p1%dm"},    // set background color (ANSI)
    {(పూర్ణాంకం)KS_CAF,	"\033[3%p1%dm"},    // set foreground color (ANSI)
    {(పూర్ణాంకం)KS_CSB,	"\033[102;%p1%dm"}, // set screen background color
    {(పూర్ణాంకం)KS_CSF,	"\033[101;%p1%dm"}, // set screen foreground color
#లేకపోతే
    {(పూర్ణాంకం)KS_CAB,	"\033[4%dm"},	    // set background color (ANSI)
    {(పూర్ణాంకం)KS_CAF,	"\033[3%dm"},	    // set foreground color (ANSI)
    {(పూర్ణాంకం)KS_CSB,	"\033[102;%dm"},    // set screen background color
    {(పూర్ణాంకం)KS_CSF,	"\033[101;%dm"},    // set screen foreground color
#అయితే_ముగింపు
    {(పూర్ణాంకం)KS_MS,	"y"},		// guessed
    {(పూర్ణాంకం)KS_UT,	"y"},		// guessed
    {(పూర్ణాంకం)KS_LE,	"\b"},
#నిర్వచితమైతే TERMINFO
    {(పూర్ణాంకం)KS_CM,	"\033[%i%p1%d;%p2%dH"},
#లేకపోతే
    {(పూర్ణాంకం)KS_CM,	"\033[%i%d;%dH"},
#అయితే_ముగింపు
    {(పూర్ణాంకం)KS_SR,	"\033M"},
#నిర్వచితమైతే TERMINFO
    {(పూర్ణాంకం)KS_CRI,	"\033[%p1%dC"},
#లేకపోతే
    {(పూర్ణాంకం)KS_CRI,	"\033[%dC"},
#అయితే_ముగింపు
    {(పూర్ణాంకం)KS_CIS,	"\033P3.y"},
    {(పూర్ణాంకం)KS_CIE,	"\234"},    // ST "String Terminator"
    {(పూర్ణాంకం)KS_TS,	"\033P1.y"},
    {(పూర్ణాంకం)KS_FS,	"\234"},    // ST "String Terminator"
#నిర్వచితమైతే TERMINFO
    {(పూర్ణాంకం)KS_CWS,	"\033[203;%p1%d;%p2%d/y"},
    {(పూర్ణాంకం)KS_CWP,	"\033[205;%p1%d;%p2%d/y"},
#లేకపోతే
    {(పూర్ణాంకం)KS_CWS,	"\033[203;%d;%d/y"},
    {(పూర్ణాంకం)KS_CWP,	"\033[205;%d;%d/y"},
#అయితే_ముగింపు
    {K_పైకి,		"\033[A"},
    {K_కిందికి,		"\033[B"},
    {K_ఎడమ,		"\033[D"},
    {K_కుడి,		"\033[C"},
    {K_S_పైకి,		"\033[161q"},
    {K_S_కిందికి,		"\033[164q"},
    {K_S_ఎడమ,		"\033[158q"},
    {K_S_కుడి,		"\033[167q"},
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
    {K_చివర,		"\033[146q"},
    {K_PAGEUP,		"\033[150q"},
    {K_PAGEDOWN,	"\033[154q"},

    {(పూర్ణాంకం)KS_NAME,	ఖాళీ}  // end marker
};

/*
 * These codes are valid when nansi.sys or equivalent has been installed.
 * Function keys on a PC are preceded with a NUL. These are converted into
 * K_NUL '\316' in mch_inchar(), because we cannot handle NULs in key codes.
 * CTRL-arrow is used instead of SHIFT-arrow.
 */
స్థిర tcap_నమోదు_T builtin_pcansi[] = {
    {(పూర్ణాంకం)KS_DL,	"\033[M"},
    {(పూర్ణాంకం)KS_AL,	"\033[L"},
    {(పూర్ణాంకం)KS_CE,	"\033[K"},
    {(పూర్ణాంకం)KS_CL,	"\033[2J"},
    {(పూర్ణాంకం)KS_ME,	"\033[0m"},
    {(పూర్ణాంకం)KS_MR,	"\033[5m"},	// reverse: black on lightgrey
    {(పూర్ణాంకం)KS_MD,	"\033[1m"},	// bold: white text
    {(పూర్ణాంకం)KS_SE,	"\033[0m"},	// standout end
    {(పూర్ణాంకం)KS_SO,	"\033[31m"},	// standout: white on blue
    {(పూర్ణాంకం)KS_CZH,	"\033[34;43m"},	// italic mode: blue text on yellow
    {(పూర్ణాంకం)KS_CZR,	"\033[0m"},	// italic mode end
    {(పూర్ణాంకం)KS_US,	"\033[36;41m"},	// underscore mode: cyan text on red
    {(పూర్ణాంకం)KS_UE,	"\033[0m"},	// underscore mode end
    {(పూర్ణాంకం)KS_CCO,	"8"},		// allow 8 colors
#నిర్వచితమైతే TERMINFO
    {(పూర్ణాంకం)KS_CAB,	"\033[4%p1%dm"},// set background color
    {(పూర్ణాంకం)KS_CAF,	"\033[3%p1%dm"},// set foreground color
#లేకపోతే
    {(పూర్ణాంకం)KS_CAB,	"\033[4%dm"},	// set background color
    {(పూర్ణాంకం)KS_CAF,	"\033[3%dm"},	// set foreground color
#అయితే_ముగింపు
    {(పూర్ణాంకం)KS_OP,	"\033[0m"},	// reset colors
    {(పూర్ణాంకం)KS_MS,	"y"},
    {(పూర్ణాంకం)KS_UT,	"y"},		// guessed
    {(పూర్ణాంకం)KS_LE,	"\b"},
#నిర్వచితమైతే TERMINFO
    {(పూర్ణాంకం)KS_CM,	"\033[%i%p1%d;%p2%dH"},
#లేకపోతే
    {(పూర్ణాంకం)KS_CM,	"\033[%i%d;%dH"},
#అయితే_ముగింపు
#నిర్వచితమైతే TERMINFO
    {(పూర్ణాంకం)KS_CRI,	"\033[%p1%dC"},
#లేకపోతే
    {(పూర్ణాంకం)KS_CRI,	"\033[%dC"},
#అయితే_ముగింపు
    {K_పైకి,		"\316H"},
    {K_కిందికి,		"\316P"},
    {K_ఎడమ,		"\316K"},
    {K_కుడి,		"\316M"},
    {K_S_ఎడమ,		"\316s"},
    {K_S_కుడి,		"\316t"},
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
    {K_చివర,		"\316O"},
    {K_PAGEDOWN,	"\316Q"},
    {K_PAGEUP,		"\316I"},

    {(పూర్ణాంకం)KS_NAME,	ఖాళీ}  // end marker
};

/*
 * These codes are valid for the Win32 Console.  The entries that start with
 * ESC | are translated into console calls in os_win32.c.  The function keys
 * are also translated in os_win32.c.
 */
స్థిర tcap_నమోదు_T builtin_win32[] = {
    {(పూర్ణాంకం)KS_CE,	"\033|K"},	// clear to end of line
    {(పూర్ణాంకం)KS_AL,	"\033|L"},	// add new blank line
#నిర్వచితమైతే TERMINFO
    {(పూర్ణాంకం)KS_CAL,	"\033|%p1%dL"},	// add number of new blank lines
#లేకపోతే
    {(పూర్ణాంకం)KS_CAL,	"\033|%dL"},	// add number of new blank lines
#అయితే_ముగింపు
    {(పూర్ణాంకం)KS_DL,	"\033|M"},	// delete line
#నిర్వచితమైతే TERMINFO
    {(పూర్ణాంకం)KS_CDL,	"\033|%p1%dM"},	// delete number of lines
    {(పూర్ణాంకం)KS_CSV,	"\033|%p1%d;%p2%dV"},
#లేకపోతే
    {(పూర్ణాంకం)KS_CDL,	"\033|%dM"},	// delete number of lines
    {(పూర్ణాంకం)KS_CSV,	"\033|%d;%dV"},
#అయితే_ముగింపు
    {(పూర్ణాంకం)KS_CL,	"\033|J"},	// clear screen
    {(పూర్ణాంకం)KS_CD,	"\033|j"},	// clear to end of display
    {(పూర్ణాంకం)KS_VI,	"\033|v"},	// cursor invisible
    {(పూర్ణాంకం)KS_VE,	"\033|V"},	// cursor visible

    {(పూర్ణాంకం)KS_ME,	"\033|0m"},	//సాధారణ
    {(పూర్ణాంకం)KS_MR,	"\033|112m"},	// reverse: black on lightgray
    {(పూర్ణాంకం)KS_MD,	"\033|15m"},	// bold: white on black
#ఒకవేళ 1
    {(పూర్ణాంకం)KS_SO,	"\033|31m"},	// standout: white on blue
    {(పూర్ణాంకం)KS_SE,	"\033|0m"},	// standout end
#లేకపోతే
    {(పూర్ణాంకం)KS_SO,	"\033|F"},	// standout: high intensity
    {(పూర్ణాంకం)KS_SE,	"\033|f"},	// standout end
#అయితే_ముగింపు
    {(పూర్ణాంకం)KS_CZH,	"\033|225m"},	// italic: blue text on yellow
    {(పూర్ణాంకం)KS_CZR,	"\033|0m"},	// italic end
    {(పూర్ణాంకం)KS_US,	"\033|67m"},	// underscore: cyan text on red
    {(పూర్ణాంకం)KS_UE,	"\033|0m"},	// underscore end
    {(పూర్ణాంకం)KS_CCO,	"16"},		// allow 16 colors
#నిర్వచితమైతే TERMINFO
    {(పూర్ణాంకం)KS_CAB,	"\033|%p1%db"},	// set background color
    {(పూర్ణాంకం)KS_CAF,	"\033|%p1%df"},	// set foreground color
#లేకపోతే
    {(పూర్ణాంకం)KS_CAB,	"\033|%db"},	// set background color
    {(పూర్ణాంకం)KS_CAF,	"\033|%df"},	// set foreground color
#అయితే_ముగింపు

    {(పూర్ణాంకం)KS_MS,	"y"},		// save to move cur in reverse mode
    {(పూర్ణాంకం)KS_UT,	"y"},
    {(పూర్ణాంకం)KS_XN,	"y"},
    {(పూర్ణాంకం)KS_LE,	"\b"},
#నిర్వచితమైతే TERMINFO
    {(పూర్ణాంకం)KS_CM,	"\033|%i%p1%d;%p2%dH"}, // cursor motion
#లేకపోతే
    {(పూర్ణాంకం)KS_CM,	"\033|%i%d;%dH"}, // cursor motion
#అయితే_ముగింపు
    {(పూర్ణాంకం)KS_VB,	"\033|B"},	// visual bell
    {(పూర్ణాంకం)KS_TI,	"\033|S"},	// put terminal in termcap mode
    {(పూర్ణాంకం)KS_TE,	"\033|E"},	// out of termcap mode
#నిర్వచితమైతే TERMINFO
    {(పూర్ణాంకం)KS_CS,	"\033|%i%p1%d;%p2%dr"}, // scroll region
#లేకపోతే
    {(పూర్ణాంకం)KS_CS,	"\033|%i%d;%dr"}, // scroll region
#అయితే_ముగింపు

    {K_పైకి,		"\316H"},
    {K_కిందికి,		"\316P"},
    {K_ఎడమ,		"\316K"},
    {K_కుడి,		"\316M"},
    {K_S_పైకి,		"\316\304"},
    {K_S_కిందికి,		"\316\317"},
    {K_S_ఎడమ,		"\316\311"},
    {K_C_ఎడమ,		"\316s"},
    {K_S_కుడి,		"\316\313"},
    {K_C_కుడి,		"\316t"},
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
    {K_చివర,		"\316O"},
    {K_S_చివర,		"\316\315"},
    {K_C_చివర,		"\316u"},
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

    {(పూర్ణాంకం)KS_NAME,	ఖాళీ}  // end marker
};

#ఒకవేళ defined(FEAT_GUI)
/*
 * GUI uses made-up codes, only used inside Vim.
 */
స్థిర tcap_నమోదు_T builtin_gui[] = {
    {(పూర్ణాంకం)KS_CE,	"\033|$"},
    {(పూర్ణాంకం)KS_AL,	"\033|i"},
# ifdef TERMINFO
    {(పూర్ణాంకం)KS_CAL,	"\033|%p1%dI"},
# లేకపోతే
    {(పూర్ణాంకం)KS_CAL,	"\033|%dI"},
# endif
    {(పూర్ణాంకం)KS_DL,	"\033|d"},
# ifdef TERMINFO
    {(పూర్ణాంకం)KS_CDL,	"\033|%p1%dD"},
    {(పూర్ణాంకం)KS_CS,	"\033|%p1%d;%p2%dR"},
    {(పూర్ణాంకం)KS_CSV,	"\033|%p1%d;%p2%dV"},
# లేకపోతే
    {(పూర్ణాంకం)KS_CDL,	"\033|%dD"},
    {(పూర్ణాంకం)KS_CS,	"\033|%d;%dR"},
    {(పూర్ణాంకం)KS_CSV,	"\033|%d;%dV"},
# endif
    {(పూర్ణాంకం)KS_CL,	"\033|C"},
			// attributes switched on with 'h', off with * 'H'
    {(పూర్ణాంకం)KS_ME,	"\033|31H"}, // HL_ALL
    {(పూర్ణాంకం)KS_MR,	"\033|1h"},   // HL_INVERSE
    {(పూర్ణాంకం)KS_MD,	"\033|2h"},   // HL_BOLD
    {(పూర్ణాంకం)KS_SE,	"\033|16H"}, // HL_STANDOUT
    {(పూర్ణాంకం)KS_SO,	"\033|16h"}, // HL_STANDOUT
    {(పూర్ణాంకం)KS_UE,	"\033|8H"},   // HL_UNDERLINE
    {(పూర్ణాంకం)KS_US,	"\033|8h"},   // HL_UNDERLINE
    {(పూర్ణాంకం)KS_UCE,	"\033|8C"},   // HL_UNDERCURL
    {(పూర్ణాంకం)KS_UCS,	"\033|8c"},   // HL_UNDERCURL
    {(పూర్ణాంకం)KS_STE,	"\033|4C"},   // HL_STRIKETHROUGH
    {(పూర్ణాంకం)KS_STS,	"\033|4c"},   // HL_STRIKETHROUGH
    {(పూర్ణాంకం)KS_CZR,	"\033|4H"},   // HL_ITALIC
    {(పూర్ణాంకం)KS_CZH,	"\033|4h"},   // HL_ITALIC
    {(పూర్ణాంకం)KS_VB,	"\033|f"},
    {(పూర్ణాంకం)KS_MS,	"y"},
    {(పూర్ణాంకం)KS_UT,	"y"},
    {(పూర్ణాంకం)KS_XN,	"y"},
    {(పూర్ణాంకం)KS_LE,	"\b"},		// cursor-left = BS
    {(పూర్ణాంకం)KS_ND,	"\014"},	// cursor-right = CTRL-L
# ifdef TERMINFO
    {(పూర్ణాంకం)KS_CM,	"\033|%p1%d;%p2%dM"},
# లేకపోతే
    {(పూర్ణాంకం)KS_CM,	"\033|%d;%dM"},
# endif
	// there are no key sequences here, the GUI sequences are recognized
	// in check_termcode()

    {(పూర్ణాంకం)KS_NAME,	ఖాళీ}  // end marker
};
#అయితే_ముగింపు

/*
 * Amiga console window, default for Amiga.
 */
స్థిర tcap_నమోదు_T builtin_amiga[] = {
    {(పూర్ణాంకం)KS_CE,	"\033[K"},
    {(పూర్ణాంకం)KS_CD,	"\033[J"},
    {(పూర్ణాంకం)KS_AL,	"\033[L"},
#నిర్వచితమైతే TERMINFO
    {(పూర్ణాంకం)KS_CAL,	"\033[%p1%dL"},
#లేకపోతే
    {(పూర్ణాంకం)KS_CAL,	"\033[%dL"},
#అయితే_ముగింపు
    {(పూర్ణాంకం)KS_DL,	"\033[M"},
#నిర్వచితమైతే TERMINFO
    {(పూర్ణాంకం)KS_CDL,	"\033[%p1%dM"},
#లేకపోతే
    {(పూర్ణాంకం)KS_CDL,	"\033[%dM"},
#అయితే_ముగింపు
    {(పూర్ణాంకం)KS_CL,	"\014"},
    {(పూర్ణాంకం)KS_VI,	"\033[0 p"},
    {(పూర్ణాంకం)KS_VE,	"\033[1 p"},
    {(పూర్ణాంకం)KS_ME,	"\033[0m"},
    {(పూర్ణాంకం)KS_MR,	"\033[7m"},
    {(పూర్ణాంకం)KS_MD,	"\033[1m"},
    {(పూర్ణాంకం)KS_SE,	"\033[0m"},
    {(పూర్ణాంకం)KS_SO,	"\033[33m"},
    {(పూర్ణాంకం)KS_US,	"\033[4m"},
    {(పూర్ణాంకం)KS_UE,	"\033[0m"},
    {(పూర్ణాంకం)KS_CZH,	"\033[3m"},
    {(పూర్ణాంకం)KS_CZR,	"\033[0m"},
#ఒకవేళ defined(__amigaos4__) || defined(__MORPHOS__) || defined(__AROS__)
    {(పూర్ణాంకం)KS_CCO,	"8"},		// allow 8 colors
# ifdef TERMINFO
    {(పూర్ణాంకం)KS_CAB,	"\033[4%p1%dm"},// set background color
    {(పూర్ణాంకం)KS_CAF,	"\033[3%p1%dm"},// set foreground color
# లేకపోతే
    {(పూర్ణాంకం)KS_CAB,	"\033[4%dm"},	// set background color
    {(పూర్ణాంకం)KS_CAF,	"\033[3%dm"},	// set foreground color
# endif
    {(పూర్ణాంకం)KS_OP,	"\033[m"},	// reset colors
#అయితే_ముగింపు
    {(పూర్ణాంకం)KS_MS,	"y"},
    {(పూర్ణాంకం)KS_UT,	"y"},		// guessed
    {(పూర్ణాంకం)KS_LE,	"\b"},
#నిర్వచితమైతే TERMINFO
    {(పూర్ణాంకం)KS_CM,	"\033[%i%p1%d;%p2%dH"},
#లేకపోతే
    {(పూర్ణాంకం)KS_CM,	"\033[%i%d;%dH"},
#అయితే_ముగింపు
#ఒకవేళ defined(__MORPHOS__)
    {(పూర్ణాంకం)KS_SR,	"\033M"},
#అయితే_ముగింపు
#నిర్వచితమైతే TERMINFO
    {(పూర్ణాంకం)KS_CRI,	"\033[%p1%dC"},
#లేకపోతే
    {(పూర్ణాంకం)KS_CRI,	"\033[%dC"},
#అయితే_ముగింపు
    {K_పైకి,		"\233A"},
    {K_కిందికి,		"\233B"},
    {K_ఎడమ,		"\233D"},
    {K_కుడి,		"\233C"},
    {K_S_పైకి,		"\233T"},
    {K_S_కిందికి,		"\233S"},
    {K_S_ఎడమ,		"\233 A"},
    {K_S_కుడి,		"\233 @"},
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
    {K_చివర,		"\233\064\065~"},	// 101 key keyboard

    {BT_EXTRA_కీలకాలు,	""},
    {TERMCAP2_కీలకం('#', '2'), "\233\065\064~"},	// shifted home key
    {TERMCAP2_కీలకం('#', '3'), "\233\065\060~"},	// shifted insert key
    {TERMCAP2_కీలకం('*', '7'), "\233\065\065~"},	// shifted end key

    {(పూర్ణాంకం)KS_NAME,	ఖాళీ}  // end marker
};

/*
 * The most minimal terminal: only clear screen and cursor positioning.
 */
స్థిర tcap_నమోదు_T builtin_dumb[] = {
    {(పూర్ణాంకం)KS_CL,	"\014"},
#నిర్వచితమైతే TERMINFO
    {(పూర్ణాంకం)KS_CM,	"\033[%i%p1%d;%p2%dH"},
#లేకపోతే
    {(పూర్ణాంకం)KS_CM,	"\033[%i%d;%dH"},
#అయితే_ముగింపు

    {(పూర్ణాంకం)KS_NAME,	ఖాళీ}  // end marker
};

/*
 * Terminal used for debugging.
 */
స్థిర tcap_నమోదు_T builtin_debug[] = {
    {(పూర్ణాంకం)KS_CE,	"[CE]"},
    {(పూర్ణాంకం)KS_CD,	"[CD]"},
    {(పూర్ణాంకం)KS_AL,	"[AL]"},
#నిర్వచితమైతే TERMINFO
    {(పూర్ణాంకం)KS_CAL,	"[CAL%p1%d]"},
#లేకపోతే
    {(పూర్ణాంకం)KS_CAL,	"[CAL%d]"},
#అయితే_ముగింపు
    {(పూర్ణాంకం)KS_DL,	"[DL]"},
#నిర్వచితమైతే TERMINFO
    {(పూర్ణాంకం)KS_CDL,	"[CDL%p1%d]"},
#లేకపోతే
    {(పూర్ణాంకం)KS_CDL,	"[CDL%d]"},
#అయితే_ముగింపు
#నిర్వచితమైతే TERMINFO
    {(పూర్ణాంకం)KS_CS,	"[%p1%dCS%p2%d]"},
#లేకపోతే
    {(పూర్ణాంకం)KS_CS,	"[%dCS%d]"},
#అయితే_ముగింపు
#నిర్వచితమైతే TERMINFO
    {(పూర్ణాంకం)KS_CSV,	"[%p1%dCSV%p2%d]"},
#లేకపోతే
    {(పూర్ణాంకం)KS_CSV,	"[%dCSV%d]"},
#అయితే_ముగింపు
#నిర్వచితమైతే TERMINFO
    {(పూర్ణాంకం)KS_CAB,	"[CAB%p1%d]"},
    {(పూర్ణాంకం)KS_CAF,	"[CAF%p1%d]"},
    {(పూర్ణాంకం)KS_CSB,	"[CSB%p1%d]"},
    {(పూర్ణాంకం)KS_CSF,	"[CSF%p1%d]"},
#లేకపోతే
    {(పూర్ణాంకం)KS_CAB,	"[CAB%d]"},
    {(పూర్ణాంకం)KS_CAF,	"[CAF%d]"},
    {(పూర్ణాంకం)KS_CSB,	"[CSB%d]"},
    {(పూర్ణాంకం)KS_CSF,	"[CSF%d]"},
#అయితే_ముగింపు
    {(పూర్ణాంకం)KS_CAU,	"[CAU%d]"},
    {(పూర్ణాంకం)KS_OP,	"[OP]"},
    {(పూర్ణాంకం)KS_LE,	"[LE]"},
    {(పూర్ణాంకం)KS_CL,	"[CL]"},
    {(పూర్ణాంకం)KS_VI,	"[VI]"},
    {(పూర్ణాంకం)KS_VE,	"[VE]"},
    {(పూర్ణాంకం)KS_VS,	"[VS]"},
    {(పూర్ణాంకం)KS_ME,	"[ME]"},
    {(పూర్ణాంకం)KS_MR,	"[MR]"},
    {(పూర్ణాంకం)KS_MB,	"[MB]"},
    {(పూర్ణాంకం)KS_MD,	"[MD]"},
    {(పూర్ణాంకం)KS_SE,	"[SE]"},
    {(పూర్ణాంకం)KS_SO,	"[SO]"},
    {(పూర్ణాంకం)KS_UE,	"[UE]"},
    {(పూర్ణాంకం)KS_US,	"[US]"},
    {(పూర్ణాంకం)KS_UCE,	"[UCE]"},
    {(పూర్ణాంకం)KS_UCS,	"[UCS]"},
    {(పూర్ణాంకం)KS_USS,	"[USS]"},
    {(పూర్ణాంకం)KS_DS,	"[DS]"},
    {(పూర్ణాంకం)KS_CDS,	"[CDS]"},
    {(పూర్ణాంకం)KS_STE,	"[STE]"},
    {(పూర్ణాంకం)KS_STS,	"[STS]"},
    {(పూర్ణాంకం)KS_MS,	"[MS]"},
    {(పూర్ణాంకం)KS_UT,	"[UT]"},
    {(పూర్ణాంకం)KS_XN,	"[XN]"},
#నిర్వచితమైతే TERMINFO
    {(పూర్ణాంకం)KS_CM,	"[%p1%dCM%p2%d]"},
#లేకపోతే
    {(పూర్ణాంకం)KS_CM,	"[%dCM%d]"},
#అయితే_ముగింపు
    {(పూర్ణాంకం)KS_SR,	"[SR]"},
#నిర్వచితమైతే TERMINFO
    {(పూర్ణాంకం)KS_CRI,	"[CRI%p1%d]"},
#లేకపోతే
    {(పూర్ణాంకం)KS_CRI,	"[CRI%d]"},
#అయితే_ముగింపు
    {(పూర్ణాంకం)KS_VB,	"[VB]"},
    {(పూర్ణాంకం)KS_KS,	"[KS]"},
    {(పూర్ణాంకం)KS_KE,	"[KE]"},
    {(పూర్ణాంకం)KS_TI,	"[TI]"},
    {(పూర్ణాంకం)KS_TE,	"[TE]"},
    {(పూర్ణాంకం)KS_CIS,	"[CIS]"},
    {(పూర్ణాంకం)KS_CIE,	"[CIE]"},
    {(పూర్ణాంకం)KS_CSC,	"[CSC]"},
    {(పూర్ణాంకం)KS_CEC,	"[CEC]"},
    {(పూర్ణాంకం)KS_TS,	"[TS]"},
    {(పూర్ణాంకం)KS_FS,	"[FS]"},
#నిర్వచితమైతే TERMINFO
    {(పూర్ణాంకం)KS_CWS,	"[%p1%dCWS%p2%d]"},
    {(పూర్ణాంకం)KS_CWP,	"[%p1%dCWP%p2%d]"},
#లేకపోతే
    {(పూర్ణాంకం)KS_CWS,	"[%dCWS%d]"},
    {(పూర్ణాంకం)KS_CWP,	"[%dCWP%d]"},
#అయితే_ముగింపు
    {(పూర్ణాంకం)KS_CRV,	"[CRV]"},
    {(పూర్ణాంకం)KS_CXM,	"[CXM]"},
    {(పూర్ణాంకం)KS_U7,	"[U7]"},
    {(పూర్ణాంకం)KS_RFG,	"[RFG]"},
    {(పూర్ణాంకం)KS_RBG,	"[RBG]"},
    {(పూర్ణాంకం)KS_CF,	"[CF%d]"},
    {K_పైకి,		"[KU]"},
    {K_కిందికి,		"[KD]"},
    {K_ఎడమ,		"[KL]"},
    {K_కుడి,		"[KR]"},
    {K_XUP,		"[xKU]"},
    {K_XDOWN,		"[xKD]"},
    {K_XLEFT,		"[xKL]"},
    {K_XRIGHT,		"[xKR]"},
    {K_S_పైకి,		"[S-KU]"},
    {K_S_కిందికి,		"[S-KD]"},
    {K_S_ఎడమ,		"[S-KL]"},
    {K_C_ఎడమ,		"[C-KL]"},
    {K_S_కుడి,		"[S-KR]"},
    {K_C_కుడి,		"[C-KR]"},
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
    {K_చివర,		"[END]"},
    {K_S_చివర,		"[C-END]"},
    {K_C_చివర,		"[C-END]"},
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

    {(పూర్ణాంకం)KS_NAME,	ఖాళీ}  // end marker
};

/*
 * List of builtin terminals.
 */
రకనిర్వచనం నిర్మాణం {
    అక్షరం	    *bitc_name;	    // name, such as "xterm"
    tcap_నమోదు_T    *bitc_పట్టిక;    // table with entries for bitc_name
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
#ఒకవేళ defined(FEAT_GUI)
    {"gui",	    builtin_gui},
#అయితే_ముగింపు
    {"amiga",	    builtin_amiga},
    {"dumb",	    builtin_dumb},
    {"debug",	    builtin_debug},

    {ఖాళీ,	    ఖాళీ},  // end marker
};

#ఒకవేళ defined(FEAT_TERMGUICOLORS)
    స్థిర guicolor_T
termgui_mch_get_color(char_u *name)
{
    తిరిగి gui_get_color_cmn(name);
}

    guicolor_T
termgui_get_color(char_u *name)
{
    guicolor_T	t;

    ఒకవేళ (*name == NUL)
	తిరిగి INVALCOLOR;
    t = termgui_mch_get_color(name);

    ఒకవేళ (t == INVALCOLOR)
	semsg(_(e_cannot_allocate_color_str), name);
    తిరిగి t;
}

    guicolor_T
termgui_mch_get_rgb(guicolor_T color)
{
    తిరిగి color;
}
#అయితే_ముగింపు

/*
 * DEFAULT_TERM is used, when no terminal is specified with -T option or $TERM.
 */
#నిర్వచితమైతే AMIGA
# define DEFAULT_పదం	(char_u *)"amiga"
#అయితే_ముగింపు

#నిర్వచితమైతే MSWIN
# define DEFAULT_పదం	(char_u *)"win32"
#అయితే_ముగింపు

#ఒకవేళ defined(UNIX)
# define DEFAULT_పదం	(char_u *)"ansi"
#అయితే_ముగింపు

#నిర్వచితమైతే VMS
# define DEFAULT_పదం	(char_u *)"vt320"
#అయితే_ముగింపు

#నిర్వచితమైతే __HAIKU__
# undef DEFAULT_పదం
# define DEFAULT_పదం	(char_u *)"xterm"
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే DEFAULT_పదం
# define DEFAULT_పదం	(char_u *)"dumb"
#అయితే_ముగింపు

/*
 * Term_strings contains currently used terminal output strings.
 * It is initialized with the default values by parse_builtin_tcap().
 * The values can be changed by setting the option with the same name.
 */
char_u *(పదం_అక్షరశ్రేణులు[(పూర్ణాంకం)KS_చివరి + 1]);

స్థిర పూర్ణాంకం	need_gather = FALSE;	    // need to fill termleader[]
స్థిర char_u	termleader[256 + 1];	    // for check_termcode()
#నిర్వచితమైతే FEAT_TERMRESPONSE
స్థిర పూర్ణాంకం	check_for_codes = FALSE;    // check for key code response
#అయితే_ముగింపు

/*
 * Structure and table to store terminal features that can be detected by
 * querying the terminal.  Either by inspecting the termresponse or a more
 * specific request.  Besides this there are:
 * t_colors - number of colors supported
 */
రకనిర్వచనం నిర్మాణం {
    అక్షరం    *tpr_name;
    పూర్ణాంకం	    tpr_సమితి_by_termresponse;
    పూర్ణాంకం	    tpr_స్థితి;
} termprop_T;

// Values for tpr_status.
#నిర్వచించు TPR_UNKNOWN	    'u'
#నిర్వచించు TPR_YES		    'y'
#నిర్వచించు TPR_NO		    'n'
#నిర్వచించు TPR_MOUSE_XTERM     'x'	// use "xterm" for 'ttymouse'
#నిర్వచించు TPR_MOUSE_XTERM2    '2'	// use "xterm2" for 'ttymouse'
#నిర్వచించు TPR_MOUSE_SGR	    's'	// use "sgr" for 'ttymouse'

// can request the cursor style without messing up the display
#నిర్వచించు TPR_CURSOR_STYLE	    0
// can request the cursor blink mode without messing up the display
#నిర్వచించు TPR_CURSOR_BLINK	    1
// can set the underline color with t_8u without resetting other colors
#నిర్వచించు TPR_UNDERLINE_RGB	    2
// mouse support - TPR_MOUSE_XTERM, TPR_MOUSE_XTERM2 or TPR_MOUSE_SGR
#నిర్వచించు TPR_MOUSE		    3
// term response indicates kitty
#నిర్వచించు TPR_KITTY		    4
// can send DECRQM requests to terminal
#నిర్వచించు TPR_DECRQM		    5
// true color, from the color count of the terminal entry
#నిర్వచించు TPR_RGB			    6
// table size
#నిర్వచించు TPR_లెక్క		    7

స్థిర termprop_T పదం_props[TPR_లెక్క];

/*
 * Set the "rgb" terminal property from the color count.  The entry of a
 * direct color terminal, such as xterm-direct, has 16777216 colors.
 */
    స్థిర శూన్య
సమితి_rgb_పదం_prop(శూన్య)
{
    పదం_props[TPR_RGB].tpr_స్థితి = t_colors == 0x1000000
						       ? TPR_YES : TPR_UNKNOWN;
}

/*
 * Initialize the term_props table.
 * When "all" is FALSE only set those that are detected from the version
 * response.
 */
    శూన్య
init_పదం_props(పూర్ణాంకం all)
{
    పూర్ణాంకం i;

    పదం_props[TPR_CURSOR_STYLE].tpr_name = "cursor_style";
    పదం_props[TPR_CURSOR_STYLE].tpr_సమితి_by_termresponse = FALSE;
    పదం_props[TPR_CURSOR_BLINK].tpr_name = "cursor_blink_mode";
    పదం_props[TPR_CURSOR_BLINK].tpr_సమితి_by_termresponse = FALSE;
    పదం_props[TPR_UNDERLINE_RGB].tpr_name = "underline_rgb";
    పదం_props[TPR_UNDERLINE_RGB].tpr_సమితి_by_termresponse = TRUE;
    పదం_props[TPR_MOUSE].tpr_name = "mouse";
    పదం_props[TPR_MOUSE].tpr_సమితి_by_termresponse = TRUE;
    పదం_props[TPR_KITTY].tpr_name = "kitty";
    పదం_props[TPR_KITTY].tpr_సమితి_by_termresponse = FALSE;
    పదం_props[TPR_DECRQM].tpr_name = "decrqm";
    పదం_props[TPR_DECRQM].tpr_సమితి_by_termresponse = TRUE;
    పదం_props[TPR_RGB].tpr_name = "rgb";
    పదం_props[TPR_RGB].tpr_సమితి_by_termresponse = FALSE;

    కొరకు (i = 0; i < TPR_లెక్క; ++i)
	ఒకవేళ (all || పదం_props[i].tpr_సమితి_by_termresponse)
	    పదం_props[i].tpr_స్థితి = TPR_UNKNOWN;

    // Derived from the color count, not reset.
    సమితి_rgb_పదం_prop();
}

#ఒకవేళ defined(FEAT_EVAL)
    శూన్య
f_terminalprops(typval_T *argvars వాడని, typval_T *rettv)
{
# ifdef FEAT_TERMRESPONSE
    పూర్ణాంకం i;
# endif

    ఒకవేళ (rettv_dict_alloc(rettv) == FAIL)
	తిరిగి;
# ifdef FEAT_TERMRESPONSE
    కొరకు (i = 0; i < TPR_లెక్క; ++i)
    {
	char_u	విలువ[2];

	విలువ[0] = పదం_props[i].tpr_స్థితి;
	విలువ[1] = NUL;
	dict_కూడు_string_len(rettv->vval.v_dict, పదం_props[i].tpr_name,
	    విలువ, (విలువ[0] == NUL) ? 0 : 1);
    }
# endif
}
#అయితే_ముగింపు

/*
 * Find the builtin termcap entries for "term".
 * This also recognizes similar names.  E.g. "xterm-256color" finds the "xterm"
 * entry.
 * Returns NULL when "term" is not found.
 */
    స్థిర tcap_నమోదు_T *
కనుగొను_builtin_పదం(char_u *పదం)
{
    కొరకు (పూర్ణాంకం i = 0; ; ++i)
    {
	char_u *name = (char_u *)builtin_terminals[i].bitc_name;
	ఒకవేళ (name == ఖాళీ)  // end marker
	    ఆపు;
#నిర్వచితమైతే UNIX
	ఒకవేళ (STRCMP(name, "iris-ansi") == 0 && vim_is_iris(పదం))
	    తిరిగి builtin_terminals[i].bitc_పట్టిక;
	ఒకవేళ (STRCMP(name, "xterm") == 0 && vim_is_xterm(పదం))
	    తిరిగి builtin_terminals[i].bitc_పట్టిక;
#అయితే_ముగింపు
#నిర్వచితమైతే VMS
	ఒకవేళ (STRCMP(name, "vt320") == 0 && vim_is_vt300(పదం))
	    తిరిగి builtin_terminals[i].bitc_పట్టిక;
#అయితే_ముగింపు
	ఒకవేళ (STRCMP(పదం, name) == 0)
	    తిరిగి builtin_terminals[i].bitc_పట్టిక;
    }
    తిరిగి ఖాళీ;
}

/*
 * Apply entries from a builtin termcap.
 */
    స్థిర శూన్య
apply_builtin_tcap(char_u *పదం, tcap_నమోదు_T *entries, పూర్ణాంకం overwrite)
{
    పూర్ణాంకం పదం_8bit = పదం_is_8bit(పదం);

    కొరకు (tcap_నమోదు_T *p = entries;
	      p->bt_నమోదు != (పూర్ణాంకం)KS_NAME && p->bt_నమోదు != BT_EXTRA_కీలకాలు; ++p)
    {
	ఒకవేళ ((పూర్ణాంకం)p->bt_నమోదు >= 0)	// KS_xx entry
	{
	    // Only set the value if it wasn't set yet or "overwrite" is TRUE.
	    ఒకవేళ (పదం_అక్షరశ్రేణులు[p->bt_నమోదు] == ఖాళీ
		    || పదం_అక్షరశ్రేణులు[p->bt_నమోదు] == ఖాళీలు_option
		    || overwrite)
	    {
#నిర్వచితమైతే FEAT_EVAL
		పూర్ణాంకం opt_idx = -1;
#అయితే_ముగింపు
		// 8bit terminal: use CSI instead of <Esc>[
		ఒకవేళ (పదం_8bit && పదం_7to8bit((char_u *)p->bt_string) != 0)
		{
		    char_u  *s, *t;

		    s = vim_strsave((char_u *)p->bt_string);
		    ఒకవేళ (s != ఖాళీ)
		    {
			కొరకు (t = s; *t; ++t)
			    ఒకవేళ (పదం_7to8bit(t))
			    {
				*t = పదం_7to8bit(t);
				STRMOVE(t + 1, t + 2);
			    }
			పదం_అక్షరశ్రేణులు[p->bt_నమోదు] = s;
#నిర్వచితమైతే FEAT_EVAL
			opt_idx =
#అయితే_ముగింపు
				  సమితి_పదం_option_alloced(
						   &పదం_అక్షరశ్రేణులు[p->bt_నమోదు]);
		    }
		}
		లేకపోతే
		{
		    పదం_అక్షరశ్రేణులు[p->bt_నమోదు] = (char_u *)p->bt_string;
#నిర్వచితమైతే FEAT_EVAL
		    opt_idx = get_పదం_opt_idx(&పదం_అక్షరశ్రేణులు[p->bt_నమోదు]);
#అయితే_ముగింపు
		}
#నిర్వచితమైతే FEAT_EVAL
		సమితి_పదం_option_sctx_idx(ఖాళీ, opt_idx);
#అయితే_ముగింపు
	    }
	}
	లేకపోతే
	{
	    char_u  name[2];
	    name[0] = KEY2TERMCAP0((పూర్ణాంకం)p->bt_నమోదు);
	    name[1] = KEY2TERMCAP1((పూర్ణాంకం)p->bt_నమోదు);
	    ఒకవేళ (కనుగొను_termcode(name) == ఖాళీ || overwrite)
		కూడు_termcode(name, (char_u *)p->bt_string, పదం_8bit);
	}
    }
}

/*
 * Apply builtin termcap entries for a given keyprotocol.
 */
    శూన్య
apply_keyprotocol(char_u *పదం, keyprot_T prot)
{
    ఒకవేళ (prot == KEYPROTOCOL_KITTY)
	apply_builtin_tcap(పదం, builtin_kitty, TRUE);
    ఒకవేళ (prot == KEYPROTOCOL_MOK2)
	apply_builtin_tcap(పదం, builtin_mok2, TRUE);

    ఒకవేళ (prot != KEYPROTOCOL_NONE)
	// Some function keys may accept modifiers even though the
	// terminfo/termcap entry does not indicate this.
	accept_modifiers_for_function_కీలకాలు();
}

/*
 * Parsing of the builtin termcap entries.
 * Caller should check if "term" is a valid builtin terminal name.
 * The terminal's name is not set, as this is already done in termcapinit().
 */
    స్థిర శూన్య
parse_builtin_tcap(char_u *పదం)
{
    tcap_నమోదు_T *entries = కనుగొను_builtin_పదం(పదం);
    ఒకవేళ (entries != ఖాళీ)
	apply_builtin_tcap(పదం, entries, FALSE);
}

/*
 * Set number of colors.
 * Store it as a number in t_colors.
 * Store it as a string in T_CCO (using nr_colors[]).
 */
    శూన్య
సమితి_color_లెక్క(పూర్ణాంకం nr)
{
    char_u	nr_colors[20];		// string for number of colors

    t_colors = nr;
    ఒకవేళ (t_colors > 1)
	sprintf((అక్షరం *)nr_colors, "%d", t_colors);
    లేకపోతే
	*nr_colors = NUL;
#ఒకవేళ 0
# ifdef FEAT_TERMGUICOLORS
    // xterm-direct, enable termguicolors, when it wasn't set yet
    ఒకవేళ (t_colors == 0x1000000 && !p_tgc_సమితి)
	సమితి_option_విలువ((char_u *)"termguicolors", 1L, ఖాళీ, 0);
# endif
#అయితే_ముగింపు
    సమితి_string_option_ప్రత్యక్ష((char_u *)"t_Co", -1, nr_colors, OPT_FREE, 0);
}

/*
 * Set the color count to "val" and redraw if it changed.
 */
    స్థిర శూన్య
may_adjust_color_లెక్క(పూర్ణాంకం val)
{
    ఒకవేళ (val == t_colors)
	తిరిగి;

    // Nr of colors changed, initialize highlighting and redraw everything.
    // This causes a redraw, which usually clears the message.  Try keeping
    // the message if it might work.
    సమితి_keep_సందేశం_from_hist();
    సమితి_color_లెక్క(val);
    init_highlight(TRUE, FALSE);
#నిర్వచితమైతే DEBUG_TERMRESPONSE
    {
	పూర్ణాంకం r = redraw_asap(UPD_CLEAR);

	LOG_TRN("Received t_Co, redraw_asap(): %d", r);
    }
#లేకపోతే
    redraw_asap(UPD_CLEAR);
#అయితే_ముగింపు
}

#నిర్వచితమైతే HAVE_TGETENT
స్థిర అక్షరం *(కీలకం_పేర్లు[]) =
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
    ఖాళీ
};
#అయితే_ముగింపు

#ఒకవేళ defined(HAVE_TGETENT) || defined(FEAT_TERMGUICOLORS)
/*
 * Return TRUE if "term_strings[idx]" was not set.
 */
    స్థిర పూర్ణాంకం
పదం_అక్షరశ్రేణులు_not_సమితి(గణన Special_కీలకం idx)
{
    తిరిగి పదం_STR(idx) == ఖాళీ || పదం_STR(idx) == ఖాళీలు_option;
}
#అయితే_ముగింపు

#నిర్వచితమైతే HAVE_TGETENT
/*
 * Get the termcap entries we need with tgetstr(), tgetflag() and tgetnum().
 * "invoke_tgetent()" must have been called before.
 * If "*height" or "*width" are not zero then use the "li" and "col" entries to
 * get their value.
 */
    స్థిర శూన్య
get_పదం_entries(పూర్ణాంకం *ఎత్తు, పూర్ణాంకం *వెడల్పు)
{
    స్థిర నిర్మాణం {
		    గణన Special_కీలకం dest; // index in term_strings[]
		    అక్షరం *name;		  // termcap name for string
		  } string_పేర్లు[] =
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
			{(గణన Special_కీలకం)0, ఖాళీ}
		    };
    పూర్ణాంకం		    i;
    స్థిర char_u   tstrbuf[TBUFSZ];
    char_u	    *tp = tstrbuf;

    /*
     * get output strings
     */
    కొరకు (i = 0; string_పేర్లు[i].name != ఖాళీ; ++i)
    {
	ఒకవేళ (పదం_అక్షరశ్రేణులు_not_సమితి(string_పేర్లు[i].dest))
	{
	    పదం_STR(string_పేర్లు[i].dest) = TGETSTR(string_పేర్లు[i].name, &tp);
# ifdef FEAT_EVAL
	    సమితి_పదం_option_sctx_idx(string_పేర్లు[i].name, -1);
# endif
	}
    }

    // tgetflag() returns 1 if the flag is present, 0 if not and
    // possibly -1 if the flag doesn't exist.
    ఒకవేళ ((T_MS == ఖాళీ || T_MS == ఖాళీలు_option) && tgetflag("ms") > 0)
	T_MS = (char_u *)"y";
    ఒకవేళ ((T_XS == ఖాళీ || T_XS == ఖాళీలు_option) && tgetflag("xs") > 0)
	T_XS = (char_u *)"y";
    ఒకవేళ ((T_XN == ఖాళీ || T_XN == ఖాళీలు_option) && tgetflag("xn") > 0)
	T_XN = (char_u *)"y";
    ఒకవేళ ((T_DB == ఖాళీ || T_DB == ఖాళీలు_option) && tgetflag("db") > 0)
	T_DB = (char_u *)"y";
    ఒకవేళ ((T_DA == ఖాళీ || T_DA == ఖాళీలు_option) && tgetflag("da") > 0)
	T_DA = (char_u *)"y";
    ఒకవేళ ((T_UT == ఖాళీ || T_UT == ఖాళీలు_option) && tgetflag("ut") > 0)
	T_UT = (char_u *)"y";
    ఒకవేళ ((T_XON == ఖాళీ || T_XON == ఖాళీలు_option) && tgetflag("xo") > 0)
	T_XON = (char_u *)"y";

    /*
     * get key codes
     */
    కొరకు (i = 0; కీలకం_పేర్లు[i] != ఖాళీ; ++i)
	ఒకవేళ (కనుగొను_termcode((char_u *)కీలకం_పేర్లు[i]) == ఖాళీ)
	{
	    char_u *p = TGETSTR(కీలకం_పేర్లు[i], &tp);

	    // if cursor-left == backspace, ignore it (televideo 925)
	    ఒకవేళ (p != ఖాళీ
		    && (*p != Ctrl_H
			|| కీలకం_పేర్లు[i][0] != 'k'
			|| కీలకం_పేర్లు[i][1] != 'l'))
		కూడు_termcode((char_u *)కీలకం_పేర్లు[i], p, FALSE);
	}

    ఒకవేళ (*ఎత్తు == 0)
	*ఎత్తు = tgetnum("li");
    ఒకవేళ (*వెడల్పు == 0)
	*వెడల్పు = tgetnum("co");

    /*
     * Get number of colors (if not done already).
     */
    ఒకవేళ (పదం_అక్షరశ్రేణులు_not_సమితి(KS_CCO))
    {
	సమితి_color_లెక్క(tgetnum("Co"));
# ifdef FEAT_EVAL
	సమితి_పదం_option_sctx_idx("Co", -1);
# endif
    }

# ifndef hpux
    BC = (అక్షరం *)TGETSTR("bc", &tp);
    పైకి_2 = (అక్షరం *)TGETSTR("up", &tp);
    char_u *p = TGETSTR("pc", &tp);
    ఒకవేళ (p != ఖాళీ)
	PC = *p;
# endif
}
#అయితే_ముగింపు

/*
 * Report "term" is not found and list the ones we do know about.
 */
    స్థిర శూన్య
report_పదం_error(అక్షరం *error_సందేశం, char_u *పదం)
{
    mch_errmsg("\r\n");
    ఒకవేళ (error_సందేశం != ఖాళీ)
    {
	mch_errmsg(error_సందేశం);
	mch_errmsg("\r\n");
    }
    mch_errmsg("'");
    mch_errmsg((అక్షరం *)పదం);
    mch_errmsg(_("' not known. Available builtin terminals are:"));
    mch_errmsg("\r\n");

    కొరకు (పూర్ణాంకం i = 0; ; ++i)
    {
	అక్షరం *name = builtin_terminals[i].bitc_name;
	ఒకవేళ (name == ఖాళీ)  // end marker
	    ఆపు;
	// Do not mention the "gui" entry, the user won't need to type it.
	ఒకవేళ (STRCMP(name, "gui") != 0)
	{
#నిర్వచితమైతే HAVE_TGETENT
	    mch_errmsg("    builtin_");
#లేకపోతే
	    mch_errmsg("    ");
#అయితే_ముగింపు
	    mch_errmsg(name);
	    mch_errmsg("\r\n");
	}
    }
    // Output extra 'cmdheight' line breaks to avoid that the following error
    // message overwrites the last terminal name.
    కొరకు (పూర్ణాంకం i = 1; i < p_ch; ++i)
	mch_errmsg("\r\n");
}

    స్థిర శూన్య
report_default_పదం(char_u *పదం)
{
    mch_errmsg(_("defaulting to '"));
    mch_errmsg((అక్షరం *)పదం);
    mch_errmsg("'\r\n");
    ఒకవేళ (emsg_silent == 0 && !లోపల_assert_fails)
    {
	screen_ప్రారంభం();	// don't know where cursor is now
	బహిర్గతం_flush();
	ఒకవేళ (!is_not_a_పదం())
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
సరిపోలిక_keyprotocol(char_u *పదం)
{
    పూర్ణాంకం		len = (పూర్ణాంకం)STRLEN(p_kpc) + 1;
    char_u	*buf = alloc(len);
    ఒకవేళ (buf == ఖాళీ)
	తిరిగి KEYPROTOCOL_FAIL;

    keyprot_T	ret = KEYPROTOCOL_FAIL;
    char_u	*p = p_kpc;
    అయితే (*p != NUL)
    {
	// Isolate one comma separated item.
	(శూన్య)copy_option_part(&p, buf, len, ",");
	char_u *colon = vim_strchr(buf, ':');
	ఒకవేళ (colon == ఖాళీ || colon == buf || colon[1] == NUL)
	    వెళ్ళు theend;
	*colon = NUL;

	keyprot_T prot;
	// Note: Keep this in sync with p_kpc_protocol_values.
	ఒకవేళ (STRCMP(colon + 1, "none") == 0)
	    prot = KEYPROTOCOL_NONE;
	లేకపోతే ఒకవేళ (STRCMP(colon + 1, "mok2") == 0)
	    prot = KEYPROTOCOL_MOK2;
	లేకపోతే ఒకవేళ (STRCMP(colon + 1, "kitty") == 0)
	    prot = KEYPROTOCOL_KITTY;
	లేకపోతే
	    వెళ్ళు theend;

	regmatch_T regmatch;
	CLEAR_క్షేత్రం(regmatch);
	regmatch.rm_ic = TRUE;
	regmatch.regprog = vim_regcomp(buf, RE_MAGIC);
	ఒకవేళ (regmatch.regprog == ఖాళీ)
	    వెళ్ళు theend;

	పూర్ణాంకం సరిపోలిక = పదం != ఖాళీ && vim_regexec(&regmatch, పదం, (colnr_T)0);
	vim_regfree(regmatch.regprog);
	ఒకవేళ (సరిపోలిక)
	{
	    ret = prot;
	    వెళ్ళు theend;
	}

    }

    // No match found, use "none".
    ret = KEYPROTOCOL_NONE;

theend:
    vim_free(buf);
    తిరిగి ret;
}

/*
 * Set terminal options for terminal "term".
 * Return OK if terminal 'term' was found in a termcap, FAIL otherwise.
 *
 * While doing this, until ttest(), some options may be NULL, be careful.
 */
    పూర్ణాంకం
సమితి_termname(char_u *పదం)
{
#నిర్వచితమైతే HAVE_TGETENT
    పూర్ణాంకం		builtin_మొదటి = p_tbi;
    పూర్ణాంకం		try;
    పూర్ణాంకం		termcap_cleared = FALSE;
#అయితే_ముగింపు
    పూర్ణాంకం		వెడల్పు = 0, ఎత్తు = 0;
    అక్షరం	*error_సందేశం = ఖాళీ;
    char_u	*bs_p, *del_p;

    // In silect mode (ex -s) we don't use the 'term' option.
    ఒకవేళ (silent_బహుళకం)
	తిరిగి OK;

    detected_8bit = FALSE;		// reset 8-bit detection

    ఒకవేళ (పదం_is_builtin(పదం))
    {
	పదం += 8;
#నిర్వచితమైతే HAVE_TGETENT
	builtin_మొదటి = 1;
#అయితే_ముగింపు
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
#నిర్వచితమైతే HAVE_TGETENT
    కొరకు (try = builtin_మొదటి ? 0 : 1; try < 3; ++try)
    {
	/*
	 * Use external termcap
	 */
	ఒకవేళ (try == 1)
	{
	    char_u	    tbuf[TBUFSZ];

	    /*
	     * If the external termcap does not have a matching entry, try the
	     * builtin ones.
	     */
	    ఒకవేళ ((error_సందేశం = invoke_tgetent(tbuf, పదం)) == ఖాళీ)
	    {
		ఒకవేళ (!termcap_cleared)
		{
		    clear_termoptions();	// clear old options
		    termcap_cleared = TRUE;
		}

		get_పదం_entries(&ఎత్తు, &వెడల్పు);
	    }
	}
	లేకపోతే	    // try == 0 || try == 2
#అయితే_ముగింపు // HAVE_TGETENT
	/*
	 * Use builtin termcap
	 */
	{
#నిర్వచితమైతే HAVE_TGETENT
	    /*
	     * If builtin termcap was already used, there is no need to search
	     * for the builtin termcap again, quit now.
	     */
	    ఒకవేళ (try == 2 && builtin_మొదటి && termcap_cleared)
		ఆపు;
#అయితే_ముగింపు
	    /*
	     * Search for 'term' in builtin_terminals[].
	     */
	    tcap_నమోదు_T *termp = కనుగొను_builtin_పదం(పదం);
	    ఒకవేళ (termp == ఖాళీ)	// did not find it
	    {
#నిర్వచితమైతే HAVE_TGETENT
		/*
		 * If try == 0, first try the external termcap. If that is not
		 * found we'll get back here with try == 2.
		 * If termcap_cleared is set we used the external termcap,
		 * don't complain about not finding the term in the builtin
		 * termcap.
		 */
		ఒకవేళ (try == 0)			// try external one
		    కొనసాగు;
		ఒకవేళ (termcap_cleared)		// found in external termcap
		    ఆపు;
#అయితే_ముగింపు
		report_పదం_error(error_సందేశం, పదం);

		// when user typed :set term=xxx, quit here
		ఒకవేళ (starting != NO_SCREEN)
		{
		    screen_ప్రారంభం();	// don't know where cursor is now
		    wait_return(TRUE);
		    తిరిగి FAIL;
		}
		పదం = DEFAULT_పదం;
		report_default_పదం(పదం);
		సమితి_string_option_ప్రత్యక్ష((char_u *)"term", -1, పదం,
								 OPT_FREE, 0);
		display_errors();
	    }
	    బహిర్గతం_flush();
#నిర్వచితమైతే HAVE_TGETENT
	    ఒకవేళ (!termcap_cleared)
	    {
#అయితే_ముగింపు
		clear_termoptions();	    // clear old options
#నిర్వచితమైతే HAVE_TGETENT
		termcap_cleared = TRUE;
	    }
#అయితే_ముగింపు
	    parse_builtin_tcap(పదం);

#నిర్వచితమైతే FEAT_GUI
	    ఒకవేళ (పదం_is_gui(పదం))
	    {
		బహిర్గతం_flush();
		gui_init();
		// If starting the GUI failed, don't do any of the other
		// things for this terminal
		ఒకవేళ (!gui.లోపల_use)
		    తిరిగి FAIL;
# ifdef HAVE_TGETENT
		ఆపు;		// don't try using external termcap
# endif
	    }
#అయితే_ముగింపు // FEAT_GUI
	}
#నిర్వచితమైతే HAVE_TGETENT
    }
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_GUI
    ఒకవేళ (!gui.లోపల_use)
#అయితే_ముగింపు
    {
	// Use the 'keyprotocol' option to adjust the t_TE and t_TI
	// termcap entries if there is an entry matching "term".
	keyprot_T kpc = సరిపోలిక_keyprotocol(పదం);
	apply_keyprotocol(పదం, kpc);

#నిర్వచితమైతే FEAT_TERMGUICOLORS
	// There is no good way to detect that the terminal supports RGB
	// colors.  Since these termcap entries are non-standard anyway and
	// only used when the user sets 'termguicolors' we might as well add
	// them.  But not when one of them was already set.
	ఒకవేళ (పదం_అక్షరశ్రేణులు_not_సమితి(KS_8F)
		&& పదం_అక్షరశ్రేణులు_not_సమితి(KS_8B)
		&& పదం_అక్షరశ్రేణులు_not_సమితి(KS_8U))
	    apply_builtin_tcap(పదం, builtin_rgb, TRUE);
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_TGETENT
	ఒకవేళ (పదం_అక్షరశ్రేణులు_not_సమితి(KS_CF))
	    apply_builtin_tcap(పదం, special_పదం, TRUE);
	ఒకవేళ (పదం_అక్షరశ్రేణులు_not_సమితి(KS_BSU) && పదం_అక్షరశ్రేణులు_not_సమితి(KS_ESU))
	    apply_builtin_tcap(పదం, builtin_sync_output, TRUE);
#అయితే_ముగింపు
    }

/*
 * special: There is no info in the termcap about whether the cursor
 * positioning is relative to the start of the screen or to the start of the
 * scrolling region.  We just guess here. Only msdos pcterm is known to do it
 * relative.
 */
    ఒకవేళ (STRCMP(పదం, "pcterm") == 0)
	T_CCS = (char_u *)"yes";
    లేకపోతే
	T_CCS = ఖాళీలు_option;

    // Special case: "kitty" may not have a "RV" entry in terminfo, but we need
    // to request the version for several other things to work.
    ఒకవేళ (అక్షరశ్రేణి_శోధన((అక్షరం *)పదం, "kitty") != ఖాళీ
					   && (T_CRV == ఖాళీ || *T_CRV == NUL))
	T_CRV = (char_u *)"\033[>c";

#నిర్వచితమైతే UNIX
/*
 * Any "stty" settings override the default for t_kb from the termcap.
 * This is in os_unix.c, because it depends a lot on the version of unix that
 * is being used.
 * Don't do this when the GUI is active, it uses "t_kb" and "t_kD" directly.
 */
# ifdef FEAT_GUI
    ఒకవేళ (!gui.లోపల_use)
# endif
	get_stty();
#అయితే_ముగింపు

/*
 * If the termcap has no entry for 'bs' and/or 'del' and the ioctl() also
 * didn't work, use the default CTRL-H
 * The default for t_kD is DEL, unless t_kb is DEL.
 * The vim_strsave'd strings are probably lost forever, well it's only two
 * bytes.  Don't do this when the GUI is active, it uses "t_kb" and "t_kD"
 * directly.
 */
#నిర్వచితమైతే FEAT_GUI
    ఒకవేళ (!gui.లోపల_use)
#అయితే_ముగింపు
    {
	bs_p = కనుగొను_termcode((char_u *)"kb");
	del_p = కనుగొను_termcode((char_u *)"kD");
	ఒకవేళ (bs_p == ఖాళీ || *bs_p == NUL)
	    కూడు_termcode((char_u *)"kb", (bs_p = (char_u *)CTRL_H_STR), FALSE);
	ఒకవేళ ((del_p == ఖాళీ || *del_p == NUL) &&
					    (bs_p == ఖాళీ || *bs_p != DEL))
	    కూడు_termcode((char_u *)"kD", (char_u *)DEL_STR, FALSE);
    }

#ఒకవేళ defined(UNIX) || defined(VMS)
    పదం_is_xterm = vim_is_xterm(పదం);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_TERMRESPONSE
    // Reset terminal properties that are set based on the termresponse, which
    // will be sent out soon.
    init_పదం_props(FALSE);
#అయితే_ముగింపు

#ఒకవేళ defined(UNIX) || defined(VMS)
    // If the first number in t_XM is 1006 then the terminal will support SGR
    // mouse reporting.
    పూర్ణాంకం did_సమితి_ttym = FALSE;
    ఒకవేళ (T_CXM != ఖాళీ && *T_CXM != NUL && !option_was_సమితి((char_u *)"ttym"))
    {
	char_u *p = T_CXM;

	అయితే (*p != NUL && !VIM_ISDIGIT(*p))
	    ++p;
	ఒకవేళ (getdigits(&p) == 1006)
	{
	    did_సమితి_ttym = TRUE;
	    సమితి_option_విలువ_give_err((char_u *)"ttym", 0L, (char_u *)"sgr", 0);
	}
    }

    /*
     * For Unix, set the 'ttymouse' option to the type of mouse to be used.
     * The termcode for the mouse is added as a side effect in option.c.
     */
    {
	char_u	*p = (char_u *)"";

# ifdef FEAT_MOUSE_XTERM
	ఒకవేళ (use_xterm_like_mouse(పదం))
	{
	    ఒకవేళ (use_xterm_mouse())
		p = ఖాళీ;	// keep existing value, might be "xterm2"
	    లేకపోతే
		p = (char_u *)"xterm";
	}
# endif
	ఒకవేళ (p != ఖాళీ && !did_సమితి_ttym)
	{
	    సమితి_option_విలువ_give_err((char_u *)"ttym", 0L, p, 0);
	    // Reset the WAS_SET flag, 'ttymouse' can be set to "sgr" or
	    // "xterm2" in check_termcode().
	    reset_option_was_సమితి((char_u *)"ttym");
	}
	ఒకవేళ (p == ఖాళీ
# ifdef FEAT_GUI
		|| gui.లోపల_use
# endif
		)
	    check_mouse_termcode();	// set mouse termcode anyway
    }
#లేకపోతే
    సమితి_mouse_termcode(KS_MOUSE, (char_u *)"\233M");
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_MOUSE_XTERM
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
	కూడు_termcode(name, (char_u *)"\033[I", FALSE);

	// handle focus out event
	name[1] = KE_FOCUSLOST;
	కూడు_termcode(name, (char_u *)"\033[O", FALSE);

	need_gather = TRUE;
    }
#అయితే_ముగింపు
#ఒకవేళ (defined(UNIX) || defined(VMS))
    // First time after setting 'term' a focus event is always reported.
    focus_state = MAYBE;
#అయితే_ముగింపు

#నిర్వచితమైతే USE_పదం_CONSOLE
    // DEFAULT_TERM indicates that it is the machine console.
    ఒకవేళ (STRCMP(పదం, DEFAULT_పదం) != 0)
	పదం_console = FALSE;
    లేకపోతే
    {
	పదం_console = TRUE;
# ifdef AMIGA
	win_resize_on();	// enable window resizing reports
# endif
    }
#అయితే_ముగింపు

    ttest(TRUE);	// make sure we have a valid set of terminal codes

    నిండిన_screen = TRUE;		// we can use termcap codes from now on
    సమితి_పదం_defaults();	// use current values as defaults
#నిర్వచితమైతే FEAT_TERMRESPONSE
    LOG_TR1("setting crv_status to STATUS_GET");
    crv_స్థితి.tr_పురోగతి = స్థితి_GET;	// Get terminal version later
    write_t_8u_state = FALSE;
#అయితే_ముగింపు

    /*
     * Initialize the terminal with the appropriate termcap codes.
     * Set the mouse and window title if possible.
     * Don't do this when starting, need to parse the .vimrc first, because it
     * may redefine t_TI etc.
     */
    ఒకవేళ (starting != NO_SCREEN)
    {
	starttermcap();		// may change terminal mode
	setmouse();		// may start using the mouse
	maketitle();		// may display window title
    }

	// display initial screen after ttest() checking. jw.
    ఒకవేళ (వెడల్పు <= 0 || ఎత్తు <= 0)
    {
	// termcap failed to report size
	// set defaults, in case ui_get_shellsize() also fails
	వెడల్పు = 80;
#ఒకవేళ defined(MSWIN)
	ఎత్తు = 25;	    // console is often 25 lines
#లేకపోతే
	ఎత్తు = 24;	    // most terminals are 24 lines
#అయితే_ముగింపు
    }
    సమితి_shellsize(వెడల్పు, ఎత్తు, FALSE);	// may change Rows
    ఒకవేళ (starting != NO_SCREEN)
    {
	ఒకవేళ (scroll_region)
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
		ఒకవేళ (curbuf->b_ml.ml_mfp != ఖాళీ)
		{
		    aucmd_prepbuf(&aco, buf);
		    ఒకవేళ (curbuf == buf)
		    {
			apply_autocmds(EVENT_TERMCHANGED, ఖాళీ, ఖాళీ, FALSE,
								      curbuf);
			// restore curwin/curbuf and a few other things
			aucmd_restbuf(&aco);
		    }
		}
	    }
	}
    }

#నిర్వచితమైతే FEAT_TERMRESPONSE
    may_req_termresponse();
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_IMAGE
    (శూన్య)update_image_backend();
#అయితే_ముగింపు

    తిరిగి OK;
}

#ఒకవేళ defined(EXITFREE)

# ifdef HAVE_DEL_CURTERM
#  include <term.h>	    // declares cur_term
# endif

/*
 * If supported, delete "cur_term", which caches terminal related entries.
 * Avoids that valgrind reports possibly lost memory.
 */
    శూన్య
free_cur_పదం(శూన్య)
{
# ifdef HAVE_DEL_CURTERM
    ఒకవేళ (cur_పదం)
	del_curterm(cur_పదం);
# endif
}

#అయితే_ముగింపు

#నిర్వచితమైతే HAVE_TGETENT
/*
 * Call tgetent()
 * Return error message if it fails, NULL if it's OK.
 */
    స్థిర అక్షరం *
invoke_tgetent(char_u *tbuf, char_u *పదం)
{
    పూర్ణాంకం	    i;

    // Note: Valgrind may report a leak here, because the library keeps one
    // buffer around that we can't ever free.
    i = TGETENT(tbuf, పదం);
    ఒకవేళ (i < 0		    // -1 is always an error
# ifdef TGETENT_ZERO_ERR
	    || i == 0	    // sometimes zero is also an error
# endif
       )
    {
	// On FreeBSD tputs() gets a SEGV after a tgetent() which fails.  Call
	// tgetent() with the always existing "dumb" entry to avoid a crash or
	// hang.
	(శూన్య)TGETENT(tbuf, "dumb");

	ఒకవేళ (i < 0)
# ifdef TGETENT_ZERO_ERR
	    తిరిగి _(e_cannot_open_termcap_file);
	ఒకవేళ (i == 0)
# endif
# ifdef TERMINFO
	    తిరిగి _(e_terminal_నమోదు_not_దొరికింది_లోపల_terminfo);
# లేకపోతే
	    తిరిగి _(e_terminal_నమోదు_not_దొరికింది_లోపల_termcap);
# endif
    }
    తిరిగి ఖాళీ;
}

/*
 * Some versions of tgetstr() have been reported to return -1 instead of NULL.
 * Fix that here.
 */
    స్థిర char_u *
vim_tgetstr(అక్షరం *s, char_u **pp)
{
    అక్షరం	*p;

    p = tgetstr(s, (అక్షరం **)pp);
    ఒకవేళ (p == (అక్షరం *)-1)
	p = ఖాళీ;
    తిరిగి (char_u *)p;
}
#అయితే_ముగింపు // HAVE_TGETENT

#ఒకవేళ defined(HAVE_TGETENT) && (defined(UNIX) || defined(VMS) || defined(MACOS_X))
/*
 * Get Columns and Rows from the termcap. Used after a window signal if the
 * ioctl() fails. It doesn't make sense to call tgetent each time if the "co"
 * and "li" entries never change. But on some systems this works.
 * Errors while getting the entries are ignored.
 */
    శూన్య
getlinecol(
    దీర్ఘ	*cp,	// pointer to columns
    దీర్ఘ	*rp)	// pointer to rows
{
    char_u	tbuf[TBUFSZ];

    ఒకవేళ (T_NAME == ఖాళీ || *T_NAME == NUL
	    || invoke_tgetent(tbuf, T_NAME) != ఖాళీ)
	తిరిగి;

    ఒకవేళ (*cp == 0)
	*cp = tgetnum("co");
    ఒకవేళ (*rp == 0)
	*rp = tgetnum("li");
}
#అయితే_ముగింపు // defined(HAVE_TGETENT) && defined(UNIX)

/*
 * Get a string entry from the termcap and add it to the list of termcodes.
 * Used for <t_xx> special keys.
 * Give an error message for failure when not sourcing.
 * If force given, replace an existing entry.
 * Return FAIL if the entry was not found, OK if the entry was added.
 */
    పూర్ణాంకం
కూడు_termcap_నమోదు(char_u *name, పూర్ణాంకం force)
{
    char_u  *పదం;
    పూర్ణాంకం	    కీలకం;
#నిర్వచితమైతే HAVE_TGETENT
    char_u  *string;
    పూర్ణాంకం	    i;
    పూర్ణాంకం	    builtin_మొదటి;
    char_u  tbuf[TBUFSZ];
    char_u  tstrbuf[TBUFSZ];
    char_u  *tp = tstrbuf;
    అక్షరం    *error_సందేశం = ఖాళీ;
#అయితే_ముగింపు

/*
 * If the GUI is running or will start in a moment, we only support the keys
 * that the GUI can produce.
 */
#నిర్వచితమైతే FEAT_GUI
    ఒకవేళ (gui.లోపల_use || gui.starting)
	తిరిగి gui_mch_haskey(name);
#అయితే_ముగింపు

    ఒకవేళ (!force && కనుగొను_termcode(name) != ఖాళీ)	    // it's already there
	తిరిగి OK;

    పదం = T_NAME;
    ఒకవేళ (పదం == ఖాళీ || *పదం == NUL)	    // 'term' not defined yet
	తిరిగి FAIL;

    ఒకవేళ (పదం_is_builtin(పదం))		    // name starts with "builtin_"
    {
	పదం += 8;
#నిర్వచితమైతే HAVE_TGETENT
	builtin_మొదటి = TRUE;
#అయితే_ముగింపు
    }
#నిర్వచితమైతే HAVE_TGETENT
    లేకపోతే
	builtin_మొదటి = p_tbi;
#అయితే_ముగింపు

#నిర్వచితమైతే HAVE_TGETENT
/*
 * We can get the entry from the builtin termcap and from the external one.
 * If 'ttybuiltin' is on or the terminal name starts with "builtin_", try
 * builtin termcap first.
 * If 'ttybuiltin' is off, try external termcap first.
 */
    కొరకు (i = 0; i < 2; ++i)
    {
	ఒకవేళ ((!builtin_మొదటి) == i)
#అయితే_ముగింపు
	/*
	 * Search in builtin termcaps
	 */
	{
	    tcap_నమోదు_T *termp = కనుగొను_builtin_పదం(పదం);
	    ఒకవేళ (termp != ఖాళీ)	// found it
	    {
		కీలకం = TERMCAP2_కీలకం(name[0], name[1]);
		++termp;
		అయితే (termp->bt_నమోదు != (పూర్ణాంకం)KS_NAME)
		{
		    ఒకవేళ ((పూర్ణాంకం)termp->bt_నమోదు == కీలకం)
		    {
			కూడు_termcode(name, (char_u *)termp->bt_string,
							  పదం_is_8bit(పదం));
			తిరిగి OK;
		    }
		    ++termp;
		}
	    }
	}
#నిర్వచితమైతే HAVE_TGETENT
	లేకపోతే
	/*
	 * Search in external termcap
	 */
	{
	    error_సందేశం = invoke_tgetent(tbuf, పదం);
	    ఒకవేళ (error_సందేశం == ఖాళీ)
	    {
		string = TGETSTR((అక్షరం *)name, &tp);
		ఒకవేళ (string != ఖాళీ && *string != NUL)
		{
		    కూడు_termcode(name, string, FALSE);
		    తిరిగి OK;
		}
	    }
	}
    }
#అయితే_ముగింపు

    ఒకవేళ (SOURCING_NAME == ఖాళీ)
    {
#నిర్వచితమైతే HAVE_TGETENT
	ఒకవేళ (error_సందేశం != ఖాళీ)
	    emsg(error_సందేశం);
	లేకపోతే
#అయితే_ముగింపు
	    semsg(_(e_no_str_నమోదు_లోపల_termcap), name);
    }
    తిరిగి FAIL;
}

    స్థిర పూర్ణాంకం
పదం_is_builtin(char_u *name)
{
    తిరిగి (STRNCMP(name, "builtin_", (size_t)8) == 0);
}

/*
 * Return TRUE if terminal "name" uses CSI instead of <Esc>[.
 * Assume that the terminal is using 8-bit controls when the name contains
 * "8bit", like in "xterm-8bit".
 */
    పూర్ణాంకం
పదం_is_8bit(char_u *name)
{
    తిరిగి (detected_8bit || అక్షరశ్రేణి_శోధన((అక్షరం *)name, "8bit") != ఖాళీ);
}

/*
 * Translate terminal control chars from 7-bit to 8-bit:
 * <Esc>[ -> CSI  <M_C_[>
 * <Esc>] -> OSC  <M-C-]>
 * <Esc>O -> <M-C-O>
 */
    స్థిర పూర్ణాంకం
పదం_7to8bit(char_u *p)
{
    ఒకవేళ (*p != ESC)
	తిరిగి 0;

    ఒకవేళ (p[1] == '[')
	తిరిగి CSI;
    లేకపోతే ఒకవేళ (p[1] == ']')
	తిరిగి OSC;
    లేకపోతే ఒకవేళ (p[1] == 'O')
	తిరిగి 0x8f;
    తిరిగి 0;
}

#ఒకవేళ defined(FEAT_GUI)
    పూర్ణాంకం
పదం_is_gui(char_u *name)
{
    తిరిగి (STRCMP(name, "builtin_gui") == 0 || STRCMP(name, "gui") == 0);
}
#అయితే_ముగింపు

#ఒకవేళ !defined(HAVE_TGETENT) || defined(AMIGA)

    char_u *
tltoa(గుర్తులేని దీర్ఘ i)
{
    స్థిర char_u buf[16];
    char_u	*p;

    p = buf + 15;
    *p = '\0';
    చేయి
    {
	--p;
	*p = (char_u) (i % 10 + '0');
	i /= 10;
    }
    అయితే (i > 0 && p > buf);
    తిరిగి p;
}
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే HAVE_TGETENT

/*
 * minimal tgoto() implementation.
 * no padding and we only parse for %i %d and %+char
 */
    స్థిర అక్షరం *
tgoto(అక్షరం *cm, పూర్ణాంకం x, పూర్ణాంకం y)
{
    స్థిర అక్షరం buf[30];
    అక్షరం *p, *s, *e;

    ఒకవేళ (!cm)
	తిరిగి "OOPS";
    e = buf + 29;
    కొరకు (s = buf; s < e && *cm; cm++)
    {
	ఒకవేళ (*cm != '%')
	{
	    *s++ = *cm;
	    కొనసాగు;
	}
	ఎంపిక (*++cm)
	{
	స్థితి 'd':
	    p = (అక్షరం *)tltoa((గుర్తులేని దీర్ఘ)y);
	    y = x;
	    అయితే (*p)
		*s++ = *p++;
	    ఆపు;
	స్థితి 'i':
	    x++;
	    y++;
	    ఆపు;
	స్థితి '+':
	    *s++ = (అక్షరం)(*++cm + y);
	    y = x;
	    ఆపు;
	స్థితి '%':
	    *s++ = *cm;
	    ఆపు;
	అప్రమేయ:
	    తిరిగి "OOPS";
	}
    }
    *s = '\0';
    తిరిగి buf;
}

#అయితే_ముగింపు // HAVE_TGETENT

/*
 * Set the terminal name and initialize the terminal options.
 * If "name" is NULL or empty, get the terminal name from the environment.
 * If that fails, use the default terminal name.
 */
    శూన్య
termcapinit(char_u *name)
{
    char_u	*పదం = name;

    ఒకవేళ (పదం != ఖాళీ && *పదం == NUL)
	పదం = ఖాళీ;	    // empty name is equal to no name

#నిర్వచితం_కాకపోతే MSWIN
    ఒకవేళ (పదం == ఖాళీ)
	పదం = mch_getenv((char_u *)"TERM");
#అయితే_ముగింపు
    ఒకవేళ (పదం == ఖాళీ || *పదం == NUL)
	పదం = DEFAULT_పదం;
    సమితి_string_option_ప్రత్యక్ష((char_u *)"term", -1, పదం, OPT_FREE, 0);

    // Set the default terminal name.
    సమితి_string_default("term", పదం);
    సమితి_string_default("ttytype", పదం);

    // Avoid using "term" here, because the next mch_getenv() may overwrite it.
    సమితి_termname(T_NAME != ఖాళీ ? T_NAME : పదం);
}

/*
 * The number of calls to ui_write is reduced by using "out_buf".
 */
#నిర్వచించు బహిర్గతం_కొలత	8191

// add one to allow mch_write() in os_win32.c to append a NUL
స్థిర char_u		బహిర్గతం_buf[బహిర్గతం_కొలత + 1];

స్థిర పూర్ణాంకం		బహిర్గతం_pos = 0;	// number of chars in out_buf

// Since the maximum number of SGR parameters shown as a normal value range is
// 16, the escape sequence length can be 4 * 16 + lead + tail.
#నిర్వచించు MAX_ESC_SEQ_LEN	80

/*
 * out_flush(): flush the output buffer
 */
    శూన్య
బహిర్గతం_flush(శూన్య)
{
    పూర్ణాంకం	    len;

    ఒకవేళ (బహిర్గతం_pos == 0)
	తిరిగి;

    // set out_pos to 0 before ui_write, to avoid recursiveness
    len = బహిర్గతం_pos;
    బహిర్గతం_pos = 0;
    ui_write(బహిర్గతం_buf, len, FALSE);
#నిర్వచితమైతే FEAT_EVAL
    ఒకవేళ (ch_log_output != FALSE)
    {
	బహిర్గతం_buf[len] = NUL;
	ch_log(ఖాళీ, "raw %s output: \"%s\"",
# ifdef FEAT_GUI
		(gui.లోపల_use && !gui.dying && !gui.starting) ? "GUI" :
# endif
		"terminal",
		బహిర్గతం_buf);
	ఒకవేళ (ch_log_output == TRUE)
	    ch_log_output = FALSE;  // only log once
    }
#అయితే_ముగింపు
}

/*
 * out_flush_cursor(): flush the output buffer and redraw the cursor.
 * Does not flush recursively in the GUI to avoid slow drawing.
 */
    శూన్య
బహిర్గతం_flush_cursor(
    పూర్ణాంకం	    force వాడని,   // when TRUE, update cursor even when not moved
    పూర్ణాంకం	    clear_selection వాడని) // clear selection under cursor
{
    mch_disable_flush();
    బహిర్గతం_flush();
    mch_enable_flush();
#నిర్వచితమైతే FEAT_GUI
    ఒకవేళ (gui.లోపల_use)
    {
	gui_update_cursor(force, clear_selection);
	gui_may_flush();
    }
#అయితే_ముగింపు
}


/*
 * Sometimes a byte out of a multi-byte character is written with out_char().
 * To avoid flushing half of the character, call this function first.
 */
    శూన్య
బహిర్గతం_flush_check(శూన్య)
{
    ఒకవేళ (enc_dbcs != 0 && బహిర్గతం_pos >= బహిర్గతం_కొలత - MB_MAXBYTES)
	బహిర్గతం_flush();
}

#నిర్వచితమైతే FEAT_GUI
/*
 * out_trash(): Throw away the contents of the output buffer
 */
    శూన్య
బహిర్గతం_trash(శూన్య)
{
    బహిర్గతం_pos = 0;
}
#అయితే_ముగింపు

/*
 * out_char(c): put a byte into the output buffer.
 *		Flush it if it becomes full.
 * This should not be used for outputting text on the screen (use functions
 * like msg_puts() and screen_putchar() for that).
 */
    శూన్య
బహిర్గతం_char(గుర్తులేని c)
{
#ఒకవేళ defined(UNIX) || defined(VMS) || defined(AMIGA) || defined(MACOS_X)
    ఒకవేళ (c == '\n')	// turn LF into CR-LF (CRMOD doesn't seem to do this)
	బహిర్గతం_char('\r');
#అయితే_ముగింపు

    బహిర్గతం_buf[బహిర్గతం_pos++] = c;

    // For testing we flush each time.
    ఒకవేళ (బహిర్గతం_pos >= బహిర్గతం_కొలత || p_wd)
	బహిర్గతం_flush();
}

/*
 * Output "c" like out_char(), but don't flush when p_wd is set.
 */
    స్థిర పూర్ణాంకం
బహిర్గతం_char_nf(పూర్ణాంకం c)
{
    బహిర్గతం_buf[బహిర్గతం_pos++] = (గుర్తులేని)c;

    ఒకవేళ (బహిర్గతం_pos >= బహిర్గతం_కొలత)
	బహిర్గతం_flush();
    తిరిగి (గుర్తులేని)c;
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
    శూన్య
బహిర్గతం_str_nf(char_u *s)
{
    // avoid terminal strings being split up
    ఒకవేళ (బహిర్గతం_pos > బహిర్గతం_కొలత - MAX_ESC_SEQ_LEN)
	బహిర్గతం_flush();

    కొరకు (char_u *p = s; *p != NUL; ++p)
	బహిర్గతం_char_nf(*p);

    // For testing we write one string at a time.
    ఒకవేళ (p_wd)
	బహిర్గతం_flush();
}

/*
 * A conditional-flushing out_str, mainly for visualbell.
 * Handles a delay internally, because termlib may not respect the delay or do
 * it at the wrong time.
 * Note: Only for terminal strings.
 */
    శూన్య
బహిర్గతం_str_cf(char_u *s)
{
    ఒకవేళ (s == ఖాళీ || *s == NUL)
	తిరిగి;

#నిర్వచితమైతే HAVE_TGETENT
    char_u *p;
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_GUI
    // Don't use tputs() when GUI is used, ncurses crashes.
    ఒకవేళ (gui.లోపల_use)
    {
	బహిర్గతం_str_nf(s);
	తిరిగి;
    }
#అయితే_ముగింపు
    ఒకవేళ (బహిర్గతం_pos > బహిర్గతం_కొలత - MAX_ESC_SEQ_LEN)
	బహిర్గతం_flush();
#నిర్వచితమైతే HAVE_TGETENT
    కొరకు (p = s; *s; ++s)
    {
	// flush just before delay command
	ఒకవేళ (*s == '$' && *(s + 1) == '<')
	{
	    char_u save_c = *s;
	    పూర్ణాంకం duration = atoi((అక్షరం *)s + 2);

	    *s = NUL;
	    tputs((అక్షరం *)p, 1, TPUTSFUNCAST బహిర్గతం_char_nf);
	    *s = save_c;
	    బహిర్గతం_flush();
# ifdef ELAPSED_FUNC
	    // Only sleep here if we can limit this happening in
	    // vim_beep().
	    p = vim_strchr(s, '>');
	    ఒకవేళ (p == ఖాళీ || duration <= 0)
	    {
		// can't parse the time, don't sleep here
		p = s;
	    }
	    లేకపోతే
	    {
		++p;
		do_sleep(duration, FALSE);
	    }
# లేకపోతే
	    // Rely on the terminal library to sleep.
	    p = s;
# endif
	    ఆపు;
	}
    }
    tputs((అక్షరం *)p, 1, TPUTSFUNCAST బహిర్గతం_char_nf);
#లేకపోతే
    అయితే (*s)
	బహిర్గతం_char_nf(*s++);
#అయితే_ముగింపు

    // For testing we write one string at a time.
    ఒకవేళ (p_wd)
	బహిర్గతం_flush();
}

/*
 * out_str(s): Put a character string a byte at a time into the output buffer.
 * If HAVE_TGETENT is defined use tputs(), the termcap parser. (jw)
 * This should only be used for writing terminal codes, not for outputting
 * normal text (use functions like msg_puts() and screen_putchar() for that).
 */
    శూన్య
బహిర్గతం_str(char_u *s)
{
    ఒకవేళ (s == ఖాళీ || *s == NUL)
	తిరిగి;

#నిర్వచితమైతే FEAT_GUI
    // Don't use tputs() when GUI is used, ncurses crashes.
    ఒకవేళ (gui.లోపల_use)
    {
	బహిర్గతం_str_nf(s);
	తిరిగి;
    }
#అయితే_ముగింపు
    // avoid terminal strings being split up
    ఒకవేళ (బహిర్గతం_pos > బహిర్గతం_కొలత - MAX_ESC_SEQ_LEN)
	బహిర్గతం_flush();
#నిర్వచితమైతే HAVE_TGETENT
    tputs((అక్షరం *)s, 1, TPUTSFUNCAST బహిర్గతం_char_nf);
#లేకపోతే
    అయితే (*s)
	బహిర్గతం_char_nf(*s++);
#అయితే_ముగింపు

    // For testing we write one string at a time.
    ఒకవేళ (p_wd)
	బహిర్గతం_flush();
}

/*
 * cursor positioning using termcap parser. (jw)
 */
    శూన్య
పదం_windgoto(పూర్ణాంకం అడ్డువరుస, పూర్ణాంకం నిలువు)
{
    బహిర్గతం_STR(tgoto((అక్షరం *)T_CM, నిలువు, అడ్డువరుస));
}

    శూన్య
పదం_cursor_కుడి(పూర్ణాంకం i)
{
    బహిర్గతం_STR(tgoto((అక్షరం *)T_CRI, 0, i));
}

    శూన్య
పదం_append_పంక్తులు(పూర్ణాంకం పంక్తి_లెక్క)
{
    బహిర్గతం_STR(tgoto((అక్షరం *)T_CAL, 0, పంక్తి_లెక్క));
}

    శూన్య
పదం_delete_పంక్తులు(పూర్ణాంకం పంక్తి_లెక్క)
{
    బహిర్గతం_STR(tgoto((అక్షరం *)T_CDL, 0, పంక్తి_లెక్క));
}

#ఒకవేళ defined(UNIX) || defined(VMS)
    శూన్య
పదం_enable_mouse(పూర్ణాంకం enable)
{
    పూర్ణాంకం on = enable ? 1 : 0;
    బహిర్గతం_STR(tgoto((అక్షరం *)T_CXM, 0, on));
}
#అయితే_ముగింపు

#ఒకవేళ defined(HAVE_TGETENT)
    శూన్య
పదం_సమితి_winpos(పూర్ణాంకం x, పూర్ణాంకం y)
{
    // Can't handle a negative value here
    ఒకవేళ (x < 0)
	x = 0;
    ఒకవేళ (y < 0)
	y = 0;
    బహిర్గతం_STR(tgoto((అక్షరం *)T_CWP, y, x));
}

# ఒకవేళ defined(FEAT_TERMRESPONSE)
/*
 * Return TRUE if we can request the terminal for a response.
 */
    స్థిర పూర్ణాంకం
can_get_termresponse(శూన్య)
{
    తిరిగి cur_tmode == TMODE_RAW
	    && termcap_చురుకైన
#  ifdef UNIX
	    && (is_not_a_పదం() || (isatty(1) && isatty(read_cmd_fd)))
#  endif
	    && p_ek;
}

/*
 * Set "status" to STATUS_SENT.
 */
    స్థిర శూన్య
termrequest_పంపిన(termrequest_T *స్థితి_2)
{
    స్థితి_2->tr_పురోగతి = స్థితి_పంపిన;
    స్థితి_2->tr_ప్రారంభం = సమయం(ఖాళీ);
}

/*
 * Return TRUE if any of the requests are in STATUS_SENT.
 */
    స్థిర పూర్ణాంకం
termrequest_any_pending(శూన్య)
{
    పూర్ణాంకం	    i;
    time_t  now = సమయం(ఖాళీ);

    కొరకు (i = 0; all_termrequests[i] != ఖాళీ; ++i)
    {
	ఒకవేళ (all_termrequests[i]->tr_పురోగతి == స్థితి_పంపిన)
	{
	    ఒకవేళ (all_termrequests[i]->tr_ప్రారంభం > 0 && now > 0
				    && all_termrequests[i]->tr_ప్రారంభం + 2 < now)
		// Sent the request more than 2 seconds ago and didn't get a
		// response, assume it failed.
		all_termrequests[i]->tr_పురోగతి = స్థితి_FAIL;
	    లేకపోతే
		తిరిగి TRUE;
	}
    }
    తిరిగి FALSE;
}

స్థిర పూర్ణాంకం winpos_x = -1;
స్థిర పూర్ణాంకం winpos_y = -1;
స్థిర పూర్ణాంకం did_request_winpos = 0;

#  ఒకవేళ defined(FEAT_EVAL) || defined(FEAT_TERMINAL)
/*
 * Try getting the Vim window position from the terminal.
 * Returns OK or FAIL.
 */
    పూర్ణాంకం
పదం_get_winpos(పూర్ణాంకం *x, పూర్ణాంకం *y, varnumber_T timeout)
{
    పూర్ణాంకం లెక్క = 0;
    పూర్ణాంకం prev_winpos_x = winpos_x;
    పూర్ణాంకం prev_winpos_y = winpos_y;

    ఒకవేళ (*T_CGP == NUL || !can_get_termresponse())
	తిరిగి FAIL;
    winpos_x = -1;
    winpos_y = -1;
    ++did_request_winpos;
    termrequest_పంపిన(&winpos_స్థితి);
    బహిర్గతం_STR(T_CGP);
    బహిర్గతం_flush();

    // Try reading the result for "timeout" msec.
    అయితే (లెక్క++ <= timeout / 10 && !got_int)
    {
	(శూన్య)vpeekc_nomap();
	ఒకవేళ (winpos_x >= 0 && winpos_y >= 0)
	{
	    *x = winpos_x;
	    *y = winpos_y;
	    తిరిగి OK;
	}
	ui_delay(10L, FALSE);
    }
    // Do not reset "did_request_winpos", if we timed out the response might
    // still come later and we must consume it.

    winpos_x = prev_winpos_x;
    winpos_y = prev_winpos_y;
    ఒకవేళ (timeout < 10 && prev_winpos_y >= 0 && prev_winpos_x >= 0)
    {
	// Polling: return previous values if we have them.
	*x = winpos_x;
	*y = winpos_y;
	తిరిగి OK;
    }

    తిరిగి FALSE;
}
#  endif
# endif

    శూన్య
పదం_సమితి_winsize(పూర్ణాంకం ఎత్తు, పూర్ణాంకం వెడల్పు)
{
    బహిర్గతం_STR(tgoto((అక్షరం *)T_CWS, వెడల్పు, ఎత్తు));
}
#అయితే_ముగింపు

    శూన్య
పదం_font(పూర్ణాంకం n)
{
    ఒకవేళ (*T_CFO)
    {
	అక్షరం buf[20];
	sprintf(buf, (అక్షరం *)T_CFO, 9 + n);
	బహిర్గతం_STR(buf);
    }
}

    స్థిర శూన్య
పదం_color(char_u *s, పూర్ణాంకం n)
{
    అక్షరం	buf[20];
    పూర్ణాంకం		i = *s == CSI ? 1 : 2;
		// index in s[] just after <Esc>[ or CSI

    // Special handling of 16 colors, because termcap can't handle it
    // Also accept "\e[3%dm" for TERMINFO, it is sometimes used
    // Also accept CSI instead of <Esc>[
    ఒకవేళ (n >= 8 && t_colors >= 16
	      && ((s[0] == ESC && s[1] == '[')
#ఒకవేళ defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
		  || (s[0] == ESC && s[1] == '|')
#అయితే_ముగింపు
		  || (s[0] == CSI && (i = 1) == 1))
	      && s[i] != NUL
	      && (STRCMP(s + i + 1, "%p1%dm") == 0
		  || STRCMP(s + i + 1, "%dm") == 0)
	      && (s[i] == '3' || s[i] == '4'))
    {
#నిర్వచితమైతే TERMINFO
	అక్షరం *format = "%s%s%%p1%%dm";
#లేకపోతే
	అక్షరం *format = "%s%s%%dm";
#అయితే_ముగింపు
	అక్షరం *lead = i == 2 ? (
#ఒకవేళ defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
		    s[1] == '|' ? "\033|" :
#అయితే_ముగింపు
		    "\033[") : "\233";
	అక్షరం *తోక = s[i] == '3' ? (n >= 16 ? "38;5;" : "9")
				 : (n >= 16 ? "48;5;" : "10");

	sprintf(buf, format, lead, తోక);
	బహిర్గతం_STR(tgoto(buf, 0, n >= 16 ? n : n - 8));
    }
    లేకపోతే
	బహిర్గతం_STR(tgoto((అక్షరం *)s, 0, n));
}

    శూన్య
పదం_fg_color(పూర్ణాంకం n)
{
    // Use "AF" termcap entry if present, "Sf" entry otherwise
    ఒకవేళ (*T_CAF)
	పదం_color(T_CAF, n);
    లేకపోతే ఒకవేళ (*T_CSF)
	పదం_color(T_CSF, n);
}

    శూన్య
పదం_bg_color(పూర్ణాంకం n)
{
    // Use "AB" termcap entry if present, "Sb" entry otherwise
    ఒకవేళ (*T_CAB)
	పదం_color(T_CAB, n);
    లేకపోతే ఒకవేళ (*T_CSB)
	పదం_color(T_CSB, n);
}

    శూన్య
పదం_ul_color(పూర్ణాంకం n)
{
    ఒకవేళ (*T_CAU)
	పదం_color(T_CAU, n);
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
పదం_bg_default(శూన్య)
{
#ఒకవేళ defined(MSWIN)
    // DOS console is nearly always black
    తిరిగి (char_u *)"dark";
#లేకపోతే
    char_u	*p;

    ఒకవేళ (STRCMP(T_NAME, "linux") == 0
	    || STRCMP(T_NAME, "screen.linux") == 0
	    || STRNCMP(T_NAME, "cygwin", 6) == 0
	    || STRNCMP(T_NAME, "putty", 5) == 0
	    || ((p = mch_getenv((char_u *)"COLORFGBG")) != ఖాళీ
		&& (p = vim_strrchr(p, ';')) != ఖాళీ
		&& ((p[1] >= '0' && p[1] <= '6') || p[1] == '8')
		&& p[2] == NUL))
	తిరిగి (char_u *)"dark";
    తిరిగి (char_u *)"light";
#అయితే_ముగింపు
}

#ఒకవేళ defined(FEAT_TERMGUICOLORS)

# define RED(rgb)   (((long_u)(rgb) >> 16) & 0xFF)
# define GREEN(rgb) (((long_u)(rgb) >>  8) & 0xFF)
# define BLUE(rgb)  (((long_u)(rgb)      ) & 0xFF)

    స్థిర శూన్య
పదం_rgb_color(char_u *s, guicolor_T rgb)
{
# define MAX_COLOR_STR_LEN 100
    అక్షరం	buf[MAX_COLOR_STR_LEN];

    ఒకవేళ (*s == NUL)
	తిరిగి;
    vim_snprintf(buf, MAX_COLOR_STR_LEN,
				  (అక్షరం *)s, RED(rgb), GREEN(rgb), BLUE(rgb));
# ifdef FEAT_VTP
    ఒకవేళ (use_vtp() && (p_tgc || t_colors >= 256))
    {
	బహిర్గతం_flush();
	buf[1] = '[';
	vtp_printf(buf);
    }
    లేకపోతే
# endif
	బహిర్గతం_STR(buf);
}

    శూన్య
పదం_fg_rgb_color(guicolor_T rgb)
{
    ఒకవేళ (rgb != INVALCOLOR)
	పదం_rgb_color(T_8F, rgb);
}

    శూన్య
పదం_bg_rgb_color(guicolor_T rgb)
{
    ఒకవేళ (rgb != INVALCOLOR)
	పదం_rgb_color(T_8B, rgb);
}

    శూన్య
పదం_ul_rgb_color(guicolor_T rgb)
{
# ifdef FEAT_TERMRESPONSE
    // If the user explicitly sets t_8u then use it.  Otherwise wait for
    // termresponse to be received, which is when t_8u would be set and a
    // redraw is needed if it was used.
    ఒకవేళ (!option_was_సమితి((char_u *)"t_8u") && write_t_8u_state != OK)
	write_t_8u_state = MAYBE;
    లేకపోతే
# endif
	పదం_rgb_color(T_8U, rgb);
}
#అయితే_ముగింపు

#ఒకవేళ defined(UNIX) || defined(VMS) || defined(MACOS_X)
/*
 * Generic function to set window title, using t_ts and t_fs.
 */
    శూన్య
పదం_settitle(char_u *title)
{
    MAY_WANT_గమ్యం_LOG_THIS;

    // t_ts takes one argument: column in status line
    బహిర్గతం_STR(tgoto((అక్షరం *)T_TS, 0, 0));	// set title start
    బహిర్గతం_str_nf(title);
    బహిర్గతం_str(T_FS);			// set title end
    బహిర్గతం_flush();
}

/*
 * Tell the terminal to push (save) the title and/or icon, so that it can be
 * popped (restored) later.
 */
    శూన్య
పదం_నెట్టు_title(పూర్ణాంకం which)
{
    ఒకవేళ ((which & SAVE_RESTORE_TITLE) && T_CST != ఖాళీ && *T_CST != NUL)
    {
	బహిర్గతం_STR(T_CST);
	బహిర్గతం_flush();
    }

    ఒకవేళ ((which & SAVE_RESTORE_ICON) && T_SSI != ఖాళీ && *T_SSI != NUL)
    {
	బహిర్గతం_STR(T_SSI);
	బహిర్గతం_flush();
    }
}

/*
 * Tell the terminal to pop the title and/or icon.
 */
    శూన్య
పదం_తీయి_title(పూర్ణాంకం which)
{
    ఒకవేళ ((which & SAVE_RESTORE_TITLE) && T_CRT != ఖాళీ && *T_CRT != NUL)
    {
	బహిర్గతం_STR(T_CRT);
	బహిర్గతం_flush();
    }

    ఒకవేళ ((which & SAVE_RESTORE_ICON) && T_SRI != ఖాళీ && *T_SRI != NUL)
    {
	బహిర్గతం_STR(T_SRI);
	బహిర్గతం_flush();
    }
}
#అయితే_ముగింపు

/*
 * Make sure we have a valid set or terminal options.
 * Replace all entries that are NULL by empty_option
 */
    శూన్య
ttest(పూర్ణాంకం జతలు)
{
    char_u *env_colors;

    check_options();		    // make sure no options are NULL

    /*
     * MUST have "cm": cursor motion.
     */
    ఒకవేళ (*T_CM == NUL)
	emsg(_(e_terminal_capability_cm_required));

    /*
     * if "cs" defined, use a scroll region, it's faster.
     */
    ఒకవేళ (*T_CS != NUL)
	scroll_region = TRUE;
    లేకపోతే
	scroll_region = FALSE;

    ఒకవేళ (జతలు)
    {
	/*
	 * optional pairs
	 */
	// TP goes to normal mode for TI (invert) and TB (bold)
	ఒకవేళ (*T_ME == NUL)
	    T_ME = T_MR = T_MD = T_MB = ఖాళీలు_option;
	ఒకవేళ (*T_SO == NUL || *T_SE == NUL)
	    T_SO = T_SE = ఖాళీలు_option;
	ఒకవేళ (*T_US == NUL || *T_UE == NUL)
	    T_US = T_UE = ఖాళీలు_option;
	ఒకవేళ (*T_CZH == NUL || *T_CZR == NUL)
	    T_CZH = T_CZR = ఖాళీలు_option;

	// T_VE is needed even though T_VI is not defined
	ఒకవేళ (*T_VE == NUL)
	    T_VI = ఖాళీలు_option;

	// if 'mr' or 'me' is not defined use 'so' and 'se'
	ఒకవేళ (*T_ME == NUL)
	{
	    T_ME = T_SE;
	    T_MR = T_SO;
	    T_MD = T_SO;
	}

	// if 'so' or 'se' is not defined use 'mr' and 'me'
	ఒకవేళ (*T_SO == NUL)
	{
	    T_SE = T_ME;
	    ఒకవేళ (*T_MR == NUL)
		T_SO = T_MD;
	    లేకపోతే
		T_SO = T_MR;
	}

	// if 'ZH' or 'ZR' is not defined use 'mr' and 'me'
	ఒకవేళ (*T_CZH == NUL)
	{
	    T_CZR = T_ME;
	    ఒకవేళ (*T_MR == NUL)
		T_CZH = T_MD;
	    లేకపోతే
		T_CZH = T_MR;
	}

	// "Sb" and "Sf" come in pairs
	ఒకవేళ (*T_CSB == NUL || *T_CSF == NUL)
	{
	    T_CSB = ఖాళీలు_option;
	    T_CSF = ఖాళీలు_option;
	}

	// "AB" and "AF" come in pairs
	ఒకవేళ (*T_CAB == NUL || *T_CAF == NUL)
	{
	    T_CAB = ఖాళీలు_option;
	    T_CAF = ఖాళీలు_option;
	}

	// if 'Sb' and 'AB' are not defined, reset "Co"
	ఒకవేళ (*T_CSB == NUL && *T_CAB == NUL)
	    free_one_termoption(T_CCO);

	// Set 'weirdinvert' according to value of 't_xs'
	p_wiv = (*T_XS != NUL);
    }
    need_gather = TRUE;

    // Set t_colors to the value of $COLORS or t_Co.  Ignore $COLORS in the
    // GUI.
    t_colors = atoi((అక్షరం *)T_CCO);
#నిర్వచితమైతే FEAT_GUI
    ఒకవేళ (!gui.లోపల_use)
#అయితే_ముగింపు
    {
	env_colors = mch_getenv((char_u *)"COLORS");
	ఒకవేళ (env_colors != ఖాళీ && సురక్షిత_isdigit(*env_colors))
	{
	    పూర్ణాంకం colors = atoi((అక్షరం *)env_colors);

	    ఒకవేళ (colors != t_colors)
		సమితి_color_లెక్క(colors);
	}
    }
    సమితి_rgb_పదం_prop();
}

#ఒకవేళ defined(FEAT_GUI) && (defined(FEAT_MENU) || !defined(USE_ON_FLY_SCROLL))
/*
 * Represent the given long_u as individual bytes, with the most significant
 * byte first, and store them in dst.
 */
    శూన్య
కూడు_long_గమ్యం_buf(long_u val, char_u *dst)
{
    పూర్ణాంకం	    i;
    పూర్ణాంకం	    జరుపు;

    కొరకు (i = 1; i <= (పూర్ణాంకం)పరిమాణం(long_u); i++)
    {
	జరుపు = 8 * (పరిమాణం(long_u) - i);
	dst[i - 1] = (char_u) ((val >> జరుపు) & 0xff);
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
    స్థిర పూర్ణాంకం
get_long_from_buf(char_u *buf, long_u *val)
{
    పూర్ణాంకం	    len;
    char_u  బైట్లు[పరిమాణం(long_u)];
    పూర్ణాంకం	    i;
    పూర్ణాంకం	    జరుపు;

    *val = 0;
    len = get_బైట్లు_from_buf(buf, బైట్లు, (పూర్ణాంకం)పరిమాణం(long_u));
    ఒకవేళ (len == -1)
	తిరిగి -1;
    కొరకు (i = 0; i < (పూర్ణాంకం)పరిమాణం(long_u); i++)
    {
	జరుపు = 8 * (పరిమాణం(long_u) - 1 - i);
	*val += (long_u)బైట్లు[i] << జరుపు;
    }
    తిరిగి len;
}
#అయితే_ముగింపు

/*
 * Read the next num_bytes bytes from buf, and store them in bytes.  Assume
 * that buf has been through inchar().	Returns the actual number of bytes used
 * from buf (between num_bytes and num_bytes*2), or -1 if not enough bytes were
 * available.
 */
    పూర్ణాంకం
get_బైట్లు_from_buf(char_u *buf, char_u *బైట్లు, పూర్ణాంకం అంకె_బైట్లు)
{
    పూర్ణాంకం	    len = 0;
    పూర్ణాంకం	    i;
    char_u  c;

    కొరకు (i = 0; i < అంకె_బైట్లు; i++)
    {
	ఒకవేళ ((c = buf[len++]) == NUL)
	    తిరిగి -1;
	ఒకవేళ (c == K_SPECIAL)
	{
	    ఒకవేళ (buf[len] == NUL || buf[len + 1] == NUL)	    // cannot happen?
		తిరిగి -1;
	    ఒకవేళ (buf[len++] == (పూర్ణాంకం)KS_ZERO)
		c = NUL;
	    // else it should be KS_SPECIAL; when followed by KE_FILLER c is
	    // K_SPECIAL, or followed by KE_CSI and c must be CSI.
	    ఒకవేళ (buf[len++] == (పూర్ణాంకం)KE_CSI)
		c = CSI;
	}
	లేకపోతే ఒకవేళ (c == CSI && buf[len] == KS_EXTRA
					       && buf[len + 1] == (పూర్ణాంకం)KE_CSI)
	    // CSI is stored as CSI KS_SPECIAL KE_CSI to avoid confusion with
	    // the start of a special key, see add_to_input_buf_csi().
	    len += 2;
	బైట్లు[i] = c;
    }
    తిరిగి len;
}

/*
 * Check if the new shell size is valid, correct it if it's too small or way
 * too big.
 */
    శూన్య
check_shellsize(శూన్య)
{
    // need room for one window and command line
    ఒకవేళ (అడ్డువరుసలు < min_అడ్డువరుసలు_for_all_tabpages())
	అడ్డువరుసలు = min_అడ్డువరుసలు_for_all_tabpages();
    హద్దు_screen_కొలత();

    // make sure these values are not invalid
    ఒకవేళ (cmdline_అడ్డువరుస >= అడ్డువరుసలు)
	cmdline_అడ్డువరుస = అడ్డువరుసలు - 1;
    ఒకవేళ (సందేశం_అడ్డువరుస >= అడ్డువరుసలు)
	సందేశం_అడ్డువరుస = అడ్డువరుసలు - 1;
}

/*
 * Limit Rows and Columns to avoid an overflow in Rows * Columns.
 */
    శూన్య
హద్దు_screen_కొలత(శూన్య)
{
    ఒకవేళ (Columns < MIN_COLUMNS)
	Columns = MIN_COLUMNS;
    లేకపోతే ఒకవేళ (Columns > 10000)
	Columns = 10000;
    ఒకవేళ (అడ్డువరుసలు > 1000)
	అడ్డువరుసలు = 1000;
}

/*
 * Invoked just before the screen structures are going to be (re)allocated.
 */
    శూన్య
win_new_shellsize(శూన్య)
{
    స్థిర పూర్ణాంకం	old_అడ్డువరుసలు_2 = 0;
    స్థిర పూర్ణాంకం	old_Columns = 0;
    స్థిర పూర్ణాంకం	old_coloff = 0;

    ఒకవేళ (old_అడ్డువరుసలు_2 != అడ్డువరుసలు || old_Columns != COLUMNS_WITHOUT_TPL()
	    || old_coloff != TPL_LCOL())
	ui_new_shellsize();
    ఒకవేళ (old_Columns != COLUMNS_WITHOUT_TPL() || old_coloff != TPL_LCOL())
    {
	old_Columns = COLUMNS_WITHOUT_TPL();
	old_coloff = TPL_LCOL();

	shell_new_columns();
    }
    ఒకవేళ (old_అడ్డువరుసలు_2 != అడ్డువరుసలు)
    {
	// If 'window' uses the whole screen, keep it using that.
	// Don't change it when set with "-w size" on the command line.
	ఒకవేళ (p_కిటికీ == old_అడ్డువరుసలు_2 - 1
		    || (old_అడ్డువరుసలు_2 == 0 && !option_was_సమితి((char_u *)"window")))
	    p_కిటికీ = అడ్డువరుసలు - 1;
	old_అడ్డువరుసలు_2 = అడ్డువరుసలు;
	shell_new_అడ్డువరుసలు();	// update window sizes
    }
}

#ఒకవేళ defined(FEAT_IMAGE) || defined(FEAT_EVAL)
    శూన్య
update_cell_కొలత(శూన్య)
{
# ifdef FEAT_GUI
    ఒకవేళ (gui.లోపల_use)
    {
	cell_వెడల్పు = gui.char_వెడల్పు;
	cell_ఎత్తు = gui.char_ఎత్తు;
    }
    లేకపోతే
# endif
    {
	నిర్మాణం cellsize cell_sz;

	mch_calc_cell_కొలత(&cell_sz);
	ఒకవేళ (cell_sz.cs_xpixel <= 0 || cell_sz.cs_ypixel <= 0)
	{
	    cell_వెడల్పు = 8;
	    cell_ఎత్తు = 16;
	}
	లేకపోతే
	{
	    cell_వెడల్పు = cell_sz.cs_xpixel;
	    cell_ఎత్తు = cell_sz.cs_ypixel;
	}
    }
}
#అయితే_ముగింపు

/*
 * Call this function when the Vim shell has been resized in any way.
 * Will obtain the current size and redraw (also when size didn't change).
 */
    శూన్య
shell_resized(శూన్య)
{
#ఒకవేళ defined(FEAT_IMAGE) || defined(FEAT_EVAL)
    update_cell_కొలత();
#అయితే_ముగింపు
    సమితి_shellsize(0, 0, FALSE);
}

/*
 * Check if the shell size changed.  Handle a resize.
 * When the size didn't change, nothing happens.
 */
    శూన్య
shell_resized_check(శూన్య)
{
    పూర్ణాంకం		old_అడ్డువరుసలు_2 = అడ్డువరుసలు;
    పూర్ణాంకం		old_Columns = Columns;

    ఒకవేళ (exiting
#నిర్వచితమైతే FEAT_GUI
	    // Do not get the size when executing a shell command during
	    // startup.
	    || gui.starting
#అయితే_ముగింపు
	    )
	తిరిగి;

    (శూన్య)ui_get_shellsize();
    check_shellsize();
    ఒకవేళ (old_అడ్డువరుసలు_2 != అడ్డువరుసలు || old_Columns != Columns)
	shell_resized();
}

/*
 * Set size of the Vim shell.
 * If 'mustset' is TRUE, we must set Rows and Columns, do not get the real
 * window size (this is used for the :win command).
 * If 'mustset' is FALSE, we may try to get the real window size and if
 * it fails use 'width' and 'height'.
 */
    స్థిర శూన్య
సమితి_shellsize_inner(పూర్ణాంకం వెడల్పు, పూర్ణాంకం ఎత్తు, పూర్ణాంకం mustset)
{
    ఒకవేళ (updating_screen)
	// resizing while in update_screen() may cause a crash
	తిరిగి;

    // curwin->w_buffer can be NULL when we are closing a window and the
    // buffer (or window) has already been closed and removing a scrollbar
    // causes a resize event. Don't resize then, it will happen after entering
    // another buffer.
    ఒకవేళ (curwin->w_బఫరు == ఖాళీ || curwin->w_పంక్తులు == ఖాళీ)
	తిరిగి;

#నిర్వచితమైతే AMIGA
    బహిర్గతం_flush();	    // must do this before mch_get_shellsize() for
			    // some obscure reason
#అయితే_ముగింపు

    ఒకవేళ (mustset || (ui_get_shellsize() == FAIL && ఎత్తు != 0))
    {
	అడ్డువరుసలు = ఎత్తు;
	Columns = వెడల్పు;
	check_shellsize();
	ui_సమితి_shellsize(mustset);
    }
    లేకపోతే
	check_shellsize();

    // The window layout used to be adjusted here, but it now happens in
    // screenalloc() (also invoked from screenclear()).  That is because the
    // "busy" check above may skip this, but not screenalloc().

    ఒకవేళ (State != బహుళకం_ASKMORE && State != బహుళకం_EXTERNCMD
						      && State != బహుళకం_CONFIRM)
	screenclear();
    లేకపోతే
	screen_ప్రారంభం();	    // don't know where cursor is now

    ఒకవేళ (starting != NO_SCREEN)
    {
	maketitle();

	మారింది_పంక్తి_abv_curs();
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
	ఒకవేళ (State == బహుళకం_ASKMORE || State == బహుళకం_EXTERNCMD
				     || State == బహుళకం_CONFIRM || exmode_చురుకైన)
	{
	    screenalloc(FALSE);
	    repeat_సందేశం();
	}
	లేకపోతే
	{
	    ఒకవేళ (curwin->w_p_scb)
		do_check_scrollbind(TRUE);
	    ఒకవేళ (State & బహుళకం_CMDLINE)
	    {
		update_screen(UPD_NOT_VALID);
		redrawcmdline();
	    }
	    లేకపోతే
	    {
		update_topline();
		ఒకవేళ (pum_visible())
		{
		    redraw_later(UPD_NOT_VALID);
		    ins_compl_show_pum();
		}
		update_screen(UPD_NOT_VALID);
		ఒకవేళ (redrawing())
		    setcursor();
	    }
	}
	cursor_on();	    // redrawing may have switched it off
    }
    బహిర్గతం_flush();
}

    శూన్య
సమితి_shellsize(పూర్ణాంకం వెడల్పు, పూర్ణాంకం ఎత్తు, పూర్ణాంకం mustset)
{
    స్థిర పూర్ణాంకం	busy = FALSE;
    స్థిర పూర్ణాంకం	do_నిరంతరం = FALSE;

    ఒకవేళ (వెడల్పు < 0 || ఎత్తు < 0)    // just checking...
	తిరిగి;

    ఒకవేళ (State == బహుళకం_HITRETURN || State == బహుళకం_SETWSIZE)
    {
	// postpone the resizing
	State = బహుళకం_SETWSIZE;
	తిరిగి;
    }

    // Avoid recursiveness.  This can happen when setting the window size
    // causes another window-changed signal or when two SIGWINCH signals come
    // very close together.  There needs to be another run then after the
    // current one is done to pick up the latest size.
    do_నిరంతరం = TRUE;
    ఒకవేళ (busy)
	తిరిగి;

    అయితే (do_నిరంతరం)
    {
	do_నిరంతరం = FALSE;
	busy = TRUE;
	సమితి_shellsize_inner(వెడల్పు, ఎత్తు, mustset);
	busy = FALSE;
    }
}

/*
 * Output T_CTE, the t_TE termcap entry, and handle expected effects.
 * The code possibly disables modifyOtherKeys and the Kitty keyboard protocol.
 */
    శూన్య
బహిర్గతం_str_t_TE(శూన్య)
{
    బహిర్గతం_str(T_CTE);

    // The seenModifyOtherKeys flag is not reset here.  We do expect t_TE to
    // disable modifyOtherKeys, but until Xterm version 377 there is no way to
    // detect it's enabled again after the following t_TI.  We assume that when
    // seenModifyOtherKeys was set before it will still be valid.

    // When the modifyOtherKeys level is detected to be 2 we expect t_TE to
    // disable it.  Remembering that it was detected to be enabled is useful in
    // some situations.
    // The following t_TI is expected to request the state and then
    // modify_otherkeys_state will be set again.
    ఒకవేళ (modify_otherkeys_state == MOKS_ENABLED
	    || modify_otherkeys_state == MOKS_DISABLED)
	modify_otherkeys_state = MOKS_DISABLED;
    లేకపోతే ఒకవేళ (modify_otherkeys_state != MOKS_INITIAL)
	modify_otherkeys_state = MOKS_AFTER_T_TE;

    // When the kitty keyboard protocol is enabled we expect t_TE to disable
    // it.  Remembering that it was detected to be enabled is useful in some
    // situations.
    // The following t_TI is expected to request the state and then
    // kitty_protocol_state will be set again.
    ఒకవేళ (kitty_protocol_state == KKPS_ENABLED
	    || kitty_protocol_state == KKPS_DISABLED)
	kitty_protocol_state = KKPS_DISABLED;
    లేకపోతే
	kitty_protocol_state = KKPS_AFTER_T_TE;
}

స్థిర పూర్ణాంకం send_t_RK = FALSE;

/*
 * Output T_TI and setup for what follows.
 */
    శూన్య
బహిర్గతం_str_t_TI(శూన్య)
{
    బహిర్గతం_str(T_CTI);

    // Send t_RK when there is no more work to do.
    send_t_RK = TRUE;
}

/*
 * Output T_BE, but only when t_PS and t_PE are set.
 */
    శూన్య
బహిర్గతం_str_t_BE(శూన్య)
{
    char_u *p;

    ఒకవేళ (T_BE == ఖాళీ || *T_BE == NUL
	    || (p = కనుగొను_termcode((char_u *)"PS")) == ఖాళీ || *p == NUL
	    || (p = కనుగొను_termcode((char_u *)"PE")) == ఖాళీ || *p == NUL)
	తిరిగి;
    బహిర్గతం_str(T_BE);
}

/*
 * If t_TI was recently sent and there is no typeahead or work to do, now send
 * t_RK.  This is postponed to avoid the response arriving in a shell command
 * or after Vim exits.
 */
    శూన్య
may_send_t_RK(శూన్య)
{
    ఒకవేళ (send_t_RK
	    && !work_pending()
	    && !ex_సాధారణ_busy
#నిర్వచితమైతే FEAT_EVAL
	    && !లోపల_feedkeys
#అయితే_ముగింపు
	    && !exiting)
    {
	send_t_RK = FALSE;
	బహిర్గతం_str(T_CRK);
	బహిర్గతం_flush();
    }
}

/*
 * Set the terminal to TMODE_RAW (for Normal mode) or TMODE_COOK (for external
 * commands and Ex mode).
 */
    శూన్య
settmode(tmode_T tmode)
{
#నిర్వచితమైతే FEAT_GUI
    // don't set the term where gvim was started to any mode
    ఒకవేళ (gui.లోపల_use)
	తిరిగి;
#అయితే_ముగింపు

    ఒకవేళ (!నిండిన_screen)
	తిరిగి;

    /*
     * When returning after calling a shell cur_tmode is TMODE_UNKNOWN,
     * set the terminal to raw mode, even though we think it already is,
     * because the shell program may have reset the terminal mode.
     * When we think the terminal is normal, don't try to set it to
     * normal again, because that causes problems (logout!) on some
     * machines.
     */
    ఒకవేళ (tmode != cur_tmode)
    {
#నిర్వచితమైతే FEAT_TERMRESPONSE
# ifdef FEAT_GUI
	ఒకవేళ (!gui.లోపల_use && !gui.starting)
# endif
	{
	    // May need to check for T_CRV response and termcodes, it
	    // doesn't work in Cooked mode, an external program may get
	    // them.
	    ఒకవేళ (tmode != TMODE_RAW && termrequest_any_pending())
		(శూన్య)vpeekc_nomap();
	    check_for_codes_from_పదం();
	}
#అయితే_ముగింపు
	ఒకవేళ (tmode != TMODE_RAW)
	    mch_setmouse(FALSE);	// switch mouse off

	// Disable bracketed paste and modifyOtherKeys in cooked mode.
	// Avoid doing this too often, on some terminals the codes are not
	// handled properly.
	ఒకవేళ (termcap_చురుకైన && tmode != TMODE_SLEEP
		&& cur_tmode != TMODE_SLEEP)
	{
	    MAY_WANT_గమ్యం_LOG_THIS;

	    ఒకవేళ (tmode != TMODE_RAW)
	    {
		బహిర్గతం_str(T_BD);	// disable bracketed paste mode
		బహిర్గతం_str_t_TE();	// possibly disables modifyOtherKeys
	    }
	    లేకపోతే
	    {
		బహిర్గతం_str_t_BE();	// enable bracketed paste mode (should
				// be before mch_settmode().
		బహిర్గతం_str_t_TI();	// possibly enables modifyOtherKeys
	    }
	}
	బహిర్గతం_flush();
	mch_settmode(tmode);	// machine specific function
	cur_tmode = tmode;
	ఒకవేళ (tmode == TMODE_RAW)
	    setmouse();		// may switch mouse on
	బహిర్గతం_flush();
    }
#నిర్వచితమైతే FEAT_TERMRESPONSE
    may_req_termresponse();
#అయితే_ముగింపు
}

    శూన్య
starttermcap(శూన్య)
{
    ఒకవేళ (!నిండిన_screen || termcap_చురుకైన)
	తిరిగి;

    MAY_WANT_గమ్యం_LOG_THIS;

    బహిర్గతం_str(T_TI);			// start termcap mode
    బహిర్గతం_str_t_TI();			// start "raw" mode
    బహిర్గతం_str(T_KS);			// start "keypad transmit" mode
    బహిర్గతం_str_t_BE();			// enable bracketed paste mode

#ఒకవేళ defined(UNIX) || defined(VMS)
    // Enable xterm's focus reporting mode when 'esckeys' is set.
    ఒకవేళ (p_ek && *T_FE != NUL)
	బహిర్గతం_str(T_FE);
#అయితే_ముగింపు

    బహిర్గతం_flush();
    termcap_చురుకైన = TRUE;
    screen_ప్రారంభం();			// don't know where cursor is now
#నిర్వచితమైతే FEAT_TERMRESPONSE
# ifdef FEAT_GUI
    ఒకవేళ (!gui.లోపల_use && !gui.starting)
# endif
    {
	may_req_termresponse();
	// Immediately check for a response.  If t_Co changes, we don't
	// want to redraw with wrong colors first.
	ఒకవేళ (crv_స్థితి.tr_పురోగతి == స్థితి_పంపిన)
	    check_for_codes_from_పదం();
    }
#అయితే_ముగింపు
}

    శూన్య
stoptermcap(శూన్య)
{
    screen_stop_highlight();
    reset_cterm_colors();

    ఒకవేళ (!termcap_చురుకైన)
	తిరిగి;

#నిర్వచితమైతే FEAT_TERMRESPONSE
# ifdef FEAT_GUI
    ఒకవేళ (!gui.లోపల_use && !gui.starting)
# endif
    {
	// May need to discard T_CRV, T_U7 or T_RBG response.
	ఒకవేళ (termrequest_any_pending())
	{
# ifdef UNIX
	    // Give the terminal a chance to respond.
	    mch_delay(100L, 0);
# endif
# ifdef TCIFLUSH
	    // Discard data received but not read.
	    ఒకవేళ (exiting)
		tcflush(fileno(stdin), TCIFLUSH);
# endif
	}
	// Check for termcodes first, otherwise an external program may
	// get them.
	check_for_codes_from_పదం();
    }
#అయితే_ముగింపు
    MAY_WANT_గమ్యం_LOG_THIS;

#ఒకవేళ defined(UNIX) || defined(VMS)
    // Disable xterm's focus reporting mode if 'esckeys' is set.
    ఒకవేళ (p_ek && *T_FD != NUL)
	బహిర్గతం_str(T_FD);
#అయితే_ముగింపు

    బహిర్గతం_str(T_BD);			// disable bracketed paste mode
    బహిర్గతం_str(T_KE);			// stop "keypad transmit" mode
    బహిర్గతం_flush();
    termcap_చురుకైన = FALSE;

    // Output t_te before t_TE, t_te may switch between main and alternate
    // screen and following codes may work on the active screen only.
    //
    // When using the Kitty keyboard protocol the main and alternate screen
    // use a separate state.  If we are (or were) using the Kitty keyboard
    // protocol and t_te is not empty (possibly switching screens) then
    // output t_TE both before and after outputting t_te.
    ఒకవేళ (*T_TE != NUL && (kitty_protocol_state == KKPS_ENABLED
		|| kitty_protocol_state == KKPS_DISABLED))
	బహిర్గతం_str_t_TE();		// probably disables the kitty keyboard
				// protocol

    బహిర్గతం_str(T_TE);			// stop termcap mode
    cursor_on();			// just in case it is still off
    బహిర్గతం_str_t_TE();			// stop "raw" mode, modifyOtherKeys and
					// Kitty keyboard protocol
    screen_ప్రారంభం();			// don't know where cursor is now
    బహిర్గతం_flush();
}

#ఒకవేళ defined(FEAT_TERMRESPONSE)
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
    శూన్య
may_req_termresponse(శూన్య)
{
    ఒకవేళ (crv_స్థితి.tr_పురోగతి == స్థితి_GET
	    && can_get_termresponse()
	    && starting == 0
	    && *T_CRV != NUL)
    {
	MAY_WANT_గమ్యం_LOG_THIS;
	LOG_TR1("Sending CRV request");
	బహిర్గతం_str(T_CRV);
	termrequest_పంపిన(&crv_స్థితి);
	// check for the characters now, otherwise they might be eaten by
	// get_keystroke()
	బహిర్గతం_flush();
	(శూన్య)vpeekc_nomap();
    }
}

/*
 * Send sequences to the terminal and check with t_u7 how the cursor moves, to
 * find out properties of the terminal.
 * Note that this goes out before T_CRV, so that the result can be used when
 * the termresponse arrives.
 */
    శూన్య
check_terminal_behavior(శూన్య)
{
    పూర్ణాంకం	    did_send = FALSE;

    ఒకవేళ (!can_get_termresponse() || starting != 0 || *T_U7 == NUL)
	తిరిగి;

    ఒకవేళ (u7_స్థితి.tr_పురోగతి == స్థితి_GET
	    && !option_was_సమితి((char_u *)"ambiwidth"))
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
	MAY_WANT_గమ్యం_LOG_THIS;
	LOG_TR1("Sending request for ambiwidth check");
	// Do this in the second row.  In the first row the returned sequence
	// may be CSI 1;2R, which is the same as <S-F3>.
	పదం_windgoto(1, 0);
	buf[mb_char2bytes(0x25bd, buf)] = NUL;
	బహిర్గతం_str(buf);
	బహిర్గతం_str(T_U7);
	termrequest_పంపిన(&u7_స్థితి);
	బహిర్గతం_flush();
	did_send = TRUE;

	// This overwrites a few characters on the screen, a redraw is needed
	// after this. Clear them out for now.
	screen_stop_highlight();
	పదం_windgoto(1, 0);
	బహిర్గతం_str((char_u *)"  ");
	పంక్తి_was_clobbered(1);
    }

    ఒకవేళ (xcc_స్థితి.tr_పురోగతి == స్థితి_GET && అడ్డువరుసలు > 2)
    {
	// 2. Check compatibility with xterm.
	// We move the cursor to (2, 0), print a test sequence and then query
	// the current cursor position.  If the terminal properly handles
	// unknown DCS string and CSI sequence with intermediate byte, the test
	// sequence is ignored and the cursor does not move.  If the terminal
	// handles test sequence incorrectly, a garbage string is displayed and
	// the cursor does move.
	MAY_WANT_గమ్యం_LOG_THIS;
	LOG_TR1("Sending xterm compatibility test sequence.");
	// Do this in the third row.  Second row is used by ambiguous
	// character width check.
	పదం_windgoto(2, 0);
	// send the test DCS string.
	బహిర్గతం_str((char_u *)"\033Pzz\033\\");
	// send the test CSI sequence with intermediate byte.
	బహిర్గతం_str((char_u *)"\033[0%m");
	బహిర్గతం_str(T_U7);
	termrequest_పంపిన(&xcc_స్థితి);
	బహిర్గతం_flush();
	did_send = TRUE;

	// If the terminal handles test sequence incorrectly, garbage text is
	// displayed. Clear them out for now.
	screen_stop_highlight();
	పదం_windgoto(2, 0);
	బహిర్గతం_str((char_u *)"           ");
	పంక్తి_was_clobbered(2);
    }

    ఒకవేళ (did_send)
    {
	పదం_windgoto(0, 0);

	// Need to reset the known cursor position.
	screen_ప్రారంభం();

	// check for the characters now, otherwise they might be eaten by
	// get_keystroke()
	బహిర్గతం_flush();
	(శూన్య)vpeekc_nomap();
    }
}

/*
 * Similar to requesting the version string: Request the terminal background
 * color when it is the right moment.
 */
    శూన్య
may_req_bg_color(శూన్య)
{
    ఒకవేళ (can_get_termresponse() && starting == 0)
    {
	పూర్ణాంకం didit = FALSE;

# ifdef FEAT_TERMINAL
	// Only request foreground if t_RF is set.
	ఒకవేళ (rfg_స్థితి.tr_పురోగతి == స్థితి_GET && *T_RFG != NUL)
	{
	    MAY_WANT_గమ్యం_LOG_THIS;
	    LOG_TR1("Sending FG request");
	    బహిర్గతం_str(T_RFG);
	    termrequest_పంపిన(&rfg_స్థితి);
	    didit = TRUE;
	}
# endif

	// Only request background if t_RB is set.
	ఒకవేళ (rbg_స్థితి.tr_పురోగతి == స్థితి_GET && *T_RBG != NUL)
	{
	    MAY_WANT_గమ్యం_LOG_THIS;
	    LOG_TR1("Sending BG request");
	    బహిర్గతం_str(T_RBG);
	    termrequest_పంపిన(&rbg_స్థితి);
	    didit = TRUE;
	}

	ఒకవేళ (didit)
	{
	    // check for the characters now, otherwise they might be eaten by
	    // get_keystroke()
	    బహిర్గతం_flush();
	    (శూన్య)vpeekc_nomap();
	}
    }
}

#అయితే_ముగింపు

/*
 * Return TRUE when saving and restoring the screen.
 */
    పూర్ణాంకం
swapping_screen(శూన్య)
{
    తిరిగి (నిండిన_screen && *T_TI != NUL);
}

/*
 * By outputting the 'cursor very visible' termcap code, for some windowed
 * terminals this makes the screen scrolled to the correct position.
 * Used when starting Vim or returning from a shell.
 */
    శూన్య
scroll_ప్రారంభం(శూన్య)
{
    ఒకవేళ (*T_VS == NUL || *T_CVS == NUL)
	తిరిగి;

    MAY_WANT_గమ్యం_LOG_THIS;
    బహిర్గతం_str(T_VS);
    బహిర్గతం_str(T_CVS);
    screen_ప్రారంభం();		// don't know where cursor is now
}

// True if cursor is not visible
స్థిర పూర్ణాంకం cursor_is_off = FALSE;

// True if cursor is not visible due to an ongoing cursor-less sleep
స్థిర పూర్ణాంకం cursor_is_asleep = FALSE;

/*
 * Enable the cursor without checking if it's already enabled.
 */
    శూన్య
cursor_on_force(శూన్య)
{
    బహిర్గతం_str(T_VE);
    cursor_is_off = FALSE;
    cursor_is_asleep = FALSE;
}

/*
 * Enable the cursor if it's currently off.
 */
    శూన్య
cursor_on(శూన్య)
{
    ఒకవేళ (cursor_is_off && !cursor_is_asleep)
	cursor_on_force();
}

/*
 * Disable the cursor.
 */
    శూన్య
cursor_off(శూన్య)
{
    ఒకవేళ (నిండిన_screen && !cursor_is_off)
    {
	బహిర్గతం_str(T_VI);	    // disable cursor
	cursor_is_off = TRUE;
    }
}

#నిర్వచితమైతే FEAT_GUI
/*
 * Check whether the cursor is invisible due to an ongoing cursor-less sleep
 */
    పూర్ణాంకం
cursor_is_sleeping(శూన్య)
{
    తిరిగి cursor_is_asleep;
}
#అయితే_ముగింపు

/*
 * Disable the cursor and mark it disabled by cursor-less sleep
 */
    శూన్య
cursor_sleep(శూన్య)
{
    cursor_is_asleep = TRUE;
    cursor_off();
}

/*
 * Enable the cursor and mark it not disabled by cursor-less sleep
 */
    శూన్య
cursor_unsleep(శూన్య)
{
    cursor_is_asleep = FALSE;
    cursor_on();
}

#ఒకవేళ defined(CURSOR_SHAPE)
/*
 * Set cursor shape to match Insert or Replace mode.
 */
    శూన్య
పదం_cursor_బహుళకం(పూర్ణాంకం forced)
{
    స్థిర పూర్ణాంకం showing_బహుళకం = -1;
    char_u *p;

    // Only do something when redrawing the screen and we can restore the
    // mode.
    ఒకవేళ (!నిండిన_screen || *T_CEI == NUL)
    {
# ifdef FEAT_TERMRESPONSE
	ఒకవేళ (forced && initial_cursor_shape > 0)
	    // Restore to initial values.
	    పదం_cursor_shape(initial_cursor_shape, initial_cursor_blink);
# endif
	తిరిగి;
    }

    ఒకవేళ ((State & బహుళకం_REPLACE) == బహుళకం_REPLACE)
    {
	ఒకవేళ (forced || showing_బహుళకం != బహుళకం_REPLACE)
	{
	    ఒకవేళ (*T_CSR != NUL)
		p = T_CSR;	// Replace mode cursor
	    లేకపోతే
		p = T_CSI;	// fall back to Insert mode cursor
	    ఒకవేళ (*p != NUL)
	    {
		బహిర్గతం_str(p);
		showing_బహుళకం = బహుళకం_REPLACE;
	    }
	}
    }
    లేకపోతే ఒకవేళ (State & బహుళకం_చొప్పించు)
    {
	ఒకవేళ ((forced || showing_బహుళకం != బహుళకం_చొప్పించు) && *T_CSI != NUL)
	{
	    బహిర్గతం_str(T_CSI);	    // Insert mode cursor
	    showing_బహుళకం = బహుళకం_చొప్పించు;
	}
    }
    లేకపోతే ఒకవేళ (forced || showing_బహుళకం != బహుళకం_సాధారణ)
    {
	బహిర్గతం_str(T_CEI);		    // non-Insert mode cursor
	showing_బహుళకం = బహుళకం_సాధారణ;
    }
}

# ఒకవేళ defined(FEAT_TERMINAL)
    శూన్య
పదం_cursor_color(char_u *color)
{
    ఒకవేళ (*T_CSC == NUL)
	తిరిగి;

    బహిర్గతం_str(T_CSC);		// set cursor color start
    బహిర్గతం_str_nf(color);
    బహిర్గతం_str(T_CEC);		// set cursor color end
    బహిర్గతం_flush();
}
# endif

    పూర్ణాంకం
blink_state_is_inverted(శూన్య)
{
# ifdef FEAT_TERMRESPONSE
    తిరిగి rbm_స్థితి.tr_పురోగతి == స్థితి_GOT
	&& rcs_స్థితి.tr_పురోగతి == స్థితి_GOT
		&& initial_cursor_blink != initial_cursor_shape_blink;
# లేకపోతే
    తిరిగి FALSE;
# endif
}

/*
 * "shape": 1 = block, 2 = underline, 3 = vertical bar
 */
    శూన్య
పదం_cursor_shape(పూర్ణాంకం shape, పూర్ణాంకం blink)
{
    ఒకవేళ (*T_CSH != NUL)
    {
	బహిర్గతం_STR(tgoto((అక్షరం *)T_CSH, 0, shape * 2 - blink));
	బహిర్గతం_flush();
    }
    లేకపోతే
    {
	పూర్ణాంకం do_blink = blink;

	// t_SH is empty: try setting just the blink state.
	// The blink flags are XORed together, if the initial blinking from
	// style and shape differs, we need to invert the flag here.
	ఒకవేళ (blink_state_is_inverted())
	    do_blink = !blink;

	ఒకవేళ (do_blink && *T_VS != NUL)
	{
	    బహిర్గతం_str(T_VS);
	    బహిర్గతం_flush();
	}
	లేకపోతే ఒకవేళ (!do_blink && *T_CVS != NUL)
	{
	    బహిర్గతం_str(T_CVS);
	    బహిర్గతం_flush();
	}
    }
}
#అయితే_ముగింపు

/*
 * Set scrolling region for window 'wp'.
 * The region starts 'off' lines from the start of the window.
 * Also set the vertical scroll region for a vertically split window.  Always
 * the full width of the window, excluding the vertical separator.
 */
    శూన్య
scroll_region_సమితి(win_T *wp, పూర్ణాంకం off)
{
    బహిర్గతం_STR(tgoto((అక్షరం *)T_CS, W_WINROW(wp) + wp->w_ఎత్తు - 1,
							 W_WINROW(wp) + off));
    ఒకవేళ (*T_CSV != NUL && wp->w_వెడల్పు != Columns)
	బహిర్గతం_STR(tgoto((అక్షరం *)T_CSV, wp->w_wincol + wp->w_వెడల్పు - 1,
							       wp->w_wincol));
    screen_ప్రారంభం();		    // don't know where cursor is now
}

/*
 * Reset scrolling region to the whole screen.
 */
    శూన్య
scroll_region_reset(శూన్య)
{
    బహిర్గతం_STR(tgoto((అక్షరం *)T_CS, (పూర్ణాంకం)అడ్డువరుసలు - 1, 0));
    ఒకవేళ (*T_CSV != NUL)
	బహిర్గతం_STR(tgoto((అక్షరం *)T_CSV, (పూర్ణాంకం)Columns - 1, 0));
    screen_ప్రారంభం();		    // don't know where cursor is now
}


/*
 * List of terminal codes that are currently recognized.
 */

స్థిర నిర్మాణం termcode
{
    char_u  name[2];	    // termcap name of entry
    char_u  *సంకేతం_2;	    // terminal code (in allocated memory)
    పూర్ణాంకం	    len;	    // STRLEN(code)
    పూర్ణాంకం	    modlen;	    // length of part before ";*~".
} *termcodes = ఖాళీ;

స్థిర పూర్ణాంకం  tc_max_len = 0; // number of entries that termcodes[] can hold
స్థిర పూర్ణాంకం  tc_len = 0;	    // current number of entries in termcodes[]

స్థిర పూర్ణాంకం termcode_నక్షత్రం(char_u *సంకేతం_2, పూర్ణాంకం len);

    శూన్య
clear_termcodes(శూన్య)
{
    అయితే (tc_len > 0)
	vim_free(termcodes[--tc_len].సంకేతం_2);
    VIM_CLEAR(termcodes);
    tc_max_len = 0;

#నిర్వచితమైతే HAVE_TGETENT
    BC = (అక్షరం *)ఖాళీలు_option;
    పైకి_2 = (అక్షరం *)ఖాళీలు_option;
    PC = NUL;			// set pad character to NUL
    ospeed = 0;
#అయితే_ముగింపు

    need_gather = TRUE;		// need to fill termleader[]
}

#నిర్వచించు ATC_FROM_పదం 55

/*
 * For xterm we recognize special codes like "ESC[42;*X" and "ESC O*X" that
 * accept modifiers.
 * Set "termcodes[idx].modlen".
 */
    స్థిర శూన్య
adjust_modlen(పూర్ణాంకం idx)
{
    termcodes[idx].modlen = 0;
    పూర్ణాంకం j = termcode_నక్షత్రం(termcodes[idx].సంకేతం_2, termcodes[idx].len);
    ఒకవేళ (j <= 0)
	తిరిగి;

    termcodes[idx].modlen = termcodes[idx].len - 1 - j;
    // For "CSI[@;X" the "@" is not included in "modlen".
    ఒకవేళ (termcodes[idx].సంకేతం_2[termcodes[idx].modlen - 1] == '@')
	--termcodes[idx].modlen;
}

/*
 * Add a new entry for "name[2]" to the list of terminal codes.
 * Note that "name" may not have a terminating NUL.
 * The list is kept alphabetical for ":set termcap"
 * "flags" is TRUE when replacing 7-bit by 8-bit controls is desired.
 * "flags" can also be ATC_FROM_TERM for got_code_from_term().
 */
    శూన్య
కూడు_termcode(char_u *name, char_u *string, పూర్ణాంకం flags)
{
    నిర్మాణం termcode *new_tc;
    పూర్ణాంకం		    i, j;
    char_u	    *s;
    పూర్ణాంకం		    len;
#నిర్వచితమైతే FEAT_EVAL
    అక్షరం	    *action = "Setting";
#అయితే_ముగింపు

    ఒకవేళ (string == ఖాళీ || *string == NUL)
    {
	del_termcode(name);
	తిరిగి;
    }

#ఒకవేళ defined(MSWIN) && !defined(FEAT_GUI)
    s = vim_strnsave(string, STRLEN(string) + 1);
#లేకపోతే
# ifdef VIMDLL
    ఒకవేళ (!gui.లోపల_use)
	s = vim_strnsave(string, STRLEN(string) + 1);
    లేకపోతే
# endif
	s = vim_strsave(string);
#అయితే_ముగింపు
    ఒకవేళ (s == ఖాళీ)
	తిరిగి;

    // Change leading <Esc>[ to CSI, change <Esc>O to <M-O>.
    ఒకవేళ (flags != 0 && flags != ATC_FROM_పదం && పదం_7to8bit(string) != 0)
    {
	STRMOVE(s, s + 1);
	s[0] = పదం_7to8bit(string);
    }

#ఒకవేళ defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
# ifdef VIMDLL
    ఒకవేళ (!gui.లోపల_use)
# endif
    {
	ఒకవేళ (s[0] == K_NUL)
	{
	    STRMOVE(s + 1, s);
	    s[1] = 3;
	}
    }
#అయితే_ముగింపు

    len = (పూర్ణాంకం)STRLEN(s);

    need_gather = TRUE;		// need to fill termleader[]

    /*
     * need to make space for more entries
     */
    ఒకవేళ (tc_len == tc_max_len)
    {
	tc_max_len += 20;
	new_tc = ALLOC_MULT(నిర్మాణం termcode, tc_max_len);
	ఒకవేళ (new_tc == ఖాళీ)
	{
	    tc_max_len -= 20;
	    vim_free(s);
	    తిరిగి;
	}
	కొరకు (i = 0; i < tc_len; ++i)
	    new_tc[i] = termcodes[i];
	vim_free(termcodes);
	termcodes = new_tc;
    }

    /*
     * Look for existing entry with the same name, it is replaced.
     * Look for an existing entry that is alphabetical higher, the new entry
     * is inserted in front of it.
     */
    కొరకు (i = 0; i < tc_len; ++i)
    {
	ఒకవేళ (termcodes[i].name[0] < name[0])
	    కొనసాగు;
	ఒకవేళ (termcodes[i].name[0] == name[0])
	{
	    ఒకవేళ (termcodes[i].name[1] < name[1])
		కొనసాగు;
	    /*
	     * Exact match: May replace old code.
	     */
	    ఒకవేళ (termcodes[i].name[1] == name[1])
	    {
		ఒకవేళ (flags == ATC_FROM_పదం && (j = termcode_నక్షత్రం(
				    termcodes[i].సంకేతం_2, termcodes[i].len)) > 0)
		{
		    // Don't replace ESC[123;*X or ESC O*X with another when
		    // invoked from got_code_from_term().
		    ఒకవేళ (len == termcodes[i].len - j
			    && STRNCMP(s, termcodes[i].సంకేతం_2, len - 1) == 0
			    && s[len - 1]
				   == termcodes[i].సంకేతం_2[termcodes[i].len - 1])
		    {
			// They are equal but for the ";*": don't add it.
#నిర్వచితమైతే FEAT_EVAL
			ch_log(ఖాళీ, "Termcap entry %c%c did not change",
							     name[0], name[1]);
#అయితే_ముగింపు
			vim_free(s);
			తిరిగి;
		    }
		}
		లేకపోతే
		{
		    // Replace old code.
#నిర్వచితమైతే FEAT_EVAL
		    ch_log(ఖాళీ, "Termcap entry %c%c was: %s",
					  name[0], name[1], termcodes[i].సంకేతం_2);
#అయితే_ముగింపు
		    vim_free(termcodes[i].సంకేతం_2);
		    --tc_len;
		    ఆపు;
		}
	    }
	}
	/*
	 * Found alphabetical larger entry, move rest to insert new entry
	 */
#నిర్వచితమైతే FEAT_EVAL
	action = "Adding";
#అయితే_ముగింపు
	కొరకు (j = tc_len; j > i; --j)
	    termcodes[j] = termcodes[j - 1];
	ఆపు;
    }

#నిర్వచితమైతే FEAT_EVAL
    ch_log(ఖాళీ, "%s termcap entry %c%c to %s", action, name[0], name[1], s);
#అయితే_ముగింపు
    termcodes[i].name[0] = name[0];
    termcodes[i].name[1] = name[1];
    termcodes[i].సంకేతం_2 = s;
    termcodes[i].len = len;
    adjust_modlen(i);

    ++tc_len;
}

/*
 * Some function keys may include modifiers, but the terminfo/termcap entries
 * do not indicate that.  Insert ";*" where we expect modifiers might appear.
 */
    స్థిర శూన్య
accept_modifiers_for_function_కీలకాలు(శూన్య)
{
    regmatch_T regmatch;
    CLEAR_క్షేత్రం(regmatch);
    regmatch.rm_ic = TRUE;
    regmatch.regprog = vim_regcomp((char_u *)"^\033[\\d\\+\\~$", RE_MAGIC);

    కొరకు (పూర్ణాంకం i = 0; i < tc_len; ++i)
    {
	ఒకవేళ (regmatch.regprog == ఖాళీ)
	    తిరిగి;

	// skip PasteStart and PasteEnd
	ఒకవేళ (termcodes[i].name[0] == 'P'
		&& (termcodes[i].name[1] == 'S' || termcodes[i].name[1] == 'E'))
	    కొనసాగు;

	char_u *s = termcodes[i].సంకేతం_2;
	ఒకవేళ (s != ఖాళీ && vim_regexec(&regmatch, s, (colnr_T)0))
	{
	    size_t len = STRLEN(s);
	    char_u *ns = alloc(len + 3);
	    ఒకవేళ (ns != ఖాళీ)
	    {
		mch_memmove(ns, s, len - 1);
		mch_memmove(ns + len - 1, ";*~", 4);
		vim_free(s);
		termcodes[i].సంకేతం_2 = ns;
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
    స్థిర పూర్ణాంకం
termcode_నక్షత్రం(char_u *సంకేతం_2, పూర్ణాంకం len)
{
    // Shortest is <M-O>*X.  With ; shortest is <CSI>@;*X
    ఒకవేళ (len >= 3 && సంకేతం_2[len - 2] == '*')
    {
	ఒకవేళ (len >= 5 && సంకేతం_2[len - 3] == ';')
	    తిరిగి 2;
	లేకపోతే
	    తిరిగి 1;
    }
    తిరిగి 0;
}

    char_u  *
కనుగొను_termcode(char_u *name)
{
    పూర్ణాంకం	    i;

    కొరకు (i = 0; i < tc_len; ++i)
	ఒకవేళ (termcodes[i].name[0] == name[0] && termcodes[i].name[1] == name[1])
	    తిరిగి termcodes[i].సంకేతం_2;
    తిరిగి ఖాళీ;
}

    char_u *
get_termcode(పూర్ణాంకం i)
{
    ఒకవేళ (i >= tc_len)
	తిరిగి ఖాళీ;
    తిరిగి &termcodes[i].name[0];
}

/*
 * Returns the length of the terminal code at index 'idx'.
 */
    పూర్ణాంకం
get_termcode_len(పూర్ణాంకం idx)
{
    తిరిగి termcodes[idx].len;
}

    శూన్య
del_termcode(char_u *name)
{
    పూర్ణాంకం	    i;

    ఒకవేళ (termcodes == ఖాళీ)	// nothing there yet
	తిరిగి;

    need_gather = TRUE;		// need to fill termleader[]

    కొరకు (i = 0; i < tc_len; ++i)
	ఒకవేళ (termcodes[i].name[0] == name[0] && termcodes[i].name[1] == name[1])
	{
	    del_termcode_idx(i);
	    తిరిగి;
	}
    // not found. Give error message?
}

    స్థిర శూన్య
del_termcode_idx(పూర్ణాంకం idx)
{
    పూర్ణాంకం		i;

    vim_free(termcodes[idx].సంకేతం_2);
    --tc_len;
    కొరకు (i = idx; i < tc_len; ++i)
	termcodes[i] = termcodes[i + 1];
}

/*
 * Called when detected that the terminal sends 8-bit codes.
 * Convert all 7-bit codes to their 8-bit equivalent.
 */
    స్థిర శూన్య
switch_గమ్యం_8bit(శూన్య)
{
    పూర్ణాంకం		i;
    పూర్ణాంకం		c;

    // Only need to do something when not already using 8-bit codes.
    ఒకవేళ (!పదం_is_8bit(T_NAME))
    {
	కొరకు (i = 0; i < tc_len; ++i)
	{
	    c = పదం_7to8bit(termcodes[i].సంకేతం_2);
	    ఒకవేళ (c != 0)
	    {
		STRMOVE(termcodes[i].సంకేతం_2 + 1, termcodes[i].సంకేతం_2 + 2);
		termcodes[i].సంకేతం_2[0] = c;
	    }
	}
	need_gather = TRUE;		// need to fill termleader[]
    }
    detected_8bit = TRUE;
    LOG_TR1("Switching to 8 bit");
}

#నిర్వచితమైతే CHECK_DOUBLE_CLICK
స్థిర linenr_T orig_topline = 0;
# ifdef FEAT_DIFF
స్థిర పూర్ణాంకం orig_topfill = 0;
# endif
#అయితే_ముగింపు
#ఒకవేళ defined(CHECK_DOUBLE_CLICK)
/*
 * Checking for double-clicks ourselves.
 * "orig_topline" is used to avoid detecting a double-click when the window
 * contents scrolled (e.g., when 'scrolloff' is non-zero).
 */
/*
 * Set orig_topline.  Used when jumping to another window, so that a double
 * click still works.
 */
    శూన్య
సమితి_mouse_topline(win_T *wp)
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
    పూర్ణాంకం
is_mouse_topline(win_T *wp)
{
    తిరిగి orig_topline == wp->w_topline
# ifdef FEAT_DIFF
	&& orig_topfill == wp->w_topfill
# endif
	;
}
#అయితే_ముగింపు

/*
 * If "buf" is NULL put "string[new_slen]" in typebuf; "buflen" is not used.
 * If "buf" is not NULL put "string[new_slen]" in "buf[bufsize]" and adjust
 * "buflen".
 * Remove "slen" bytes.
 * Returns FAIL for error.
 */
    పూర్ణాంకం
put_string_లోపల_typebuf(
	పూర్ణాంకం	స్థానభ్రంశం,
	పూర్ణాంకం	slen,
	char_u	*string,
	పూర్ణాంకం	new_slen,
	char_u	*buf,
	పూర్ణాంకం	bufsize,
	పూర్ణాంకం	*buflen)
{
    పూర్ణాంకం		extra = new_slen - slen;

    string[new_slen] = NUL;
    ఒకవేళ (buf == ఖాళీ)
    {
	ఒకవేళ (extra < 0)
	    // remove matched chars, taking care of noremap
	    del_typebuf(-extra, స్థానభ్రంశం);
	లేకపోతే ఒకవేళ (extra > 0)
	    // insert the extra space we need
	    ఒకవేళ (ins_typebuf(string + slen, REMAP_YES, స్థానభ్రంశం, FALSE, FALSE)
								       == FAIL)
		తిరిగి FAIL;

	// Careful: del_typebuf() and ins_typebuf() may have reallocated
	// typebuf.tb_buf[]!
	mch_memmove(typebuf.tb_buf + typebuf.tb_off + స్థానభ్రంశం, string,
							     (size_t)new_slen);
    }
    లేకపోతే
    {
	ఒకవేళ (extra < 0)
	    // remove matched characters
	    mch_memmove(buf + స్థానభ్రంశం, buf + స్థానభ్రంశం - extra,
					   (size_t)(*buflen + స్థానభ్రంశం + extra));
	లేకపోతే ఒకవేళ (extra > 0)
	{
	    // Insert the extra space we need.  If there is insufficient
	    // space return -1.
	    ఒకవేళ (*buflen + extra + new_slen >= bufsize)
		తిరిగి FAIL;
	    mch_memmove(buf + స్థానభ్రంశం + extra, buf + స్థానభ్రంశం,
						   (size_t)(*buflen - స్థానభ్రంశం));
	}
	mch_memmove(buf + స్థానభ్రంశం, string, (size_t)new_slen);
	*buflen = *buflen + extra + new_slen;
    }
    తిరిగి OK;
}

/*
 * Decode a modifier number as xterm provides it into MOD_MASK bits.
 */
    పూర్ణాంకం
decode_modifiers(పూర్ణాంకం n)
{
    పూర్ణాంకం	    సంకేతం_2 = n - 1;
    పూర్ణాంకం	    modifiers = 0;

    ఒకవేళ (సంకేతం_2 & 1)
	modifiers |= MOD_ముసుగు_జరుపు;
    ఒకవేళ (సంకేతం_2 & 2)
	modifiers |= MOD_ముసుగు_ALT;
    ఒకవేళ (సంకేతం_2 & 4)
	modifiers |= MOD_ముసుగు_CTRL;
    ఒకవేళ (సంకేతం_2 & 8)
	modifiers |= MOD_ముసుగు_META;
    // Any further modifiers are silently dropped.

    తిరిగి modifiers;
}

    స్థిర పూర్ణాంకం
modifiers2keycode(పూర్ణాంకం modifiers, పూర్ణాంకం *కీలకం, char_u *string)
{
    పూర్ణాంకం new_slen = 0;

    ఒకవేళ (modifiers == 0)
	తిరిగి 0;

    // Some keys have the modifier included.  Need to handle that here to
    // make mappings work.  This may result in a special key, such as
    // K_S_TAB.
    *కీలకం = simplify_కీలకం(*కీలకం, &modifiers);
    ఒకవేళ (modifiers != 0)
    {
	string[new_slen++] = K_SPECIAL;
	string[new_slen++] = (పూర్ణాంకం)KS_MODIFIER;
	string[new_slen++] = modifiers;
    }
    తిరిగి new_slen;
}

/*
 * Handle a cursor position report.
 */
    స్థిర శూన్య
handle_u7_response(పూర్ణాంకం *arg, char_u *tp వాడని, పూర్ణాంకం csi_len వాడని)
{
    ఒకవేళ (arg[0] == 2 && arg[1] >= 2)
    {
	అక్షరం *aw = ఖాళీ;

	LOG_TRN("Received U7 status: %s", tp);
	u7_స్థితి.tr_పురోగతి = స్థితి_GOT;
	did_cursorhold = TRUE;
	ఒకవేళ (arg[1] == 2)
	    aw = "single";
	లేకపోతే ఒకవేళ (arg[1] == 3)
	    aw = "double";
	ఒకవేళ (aw != ఖాళీ && STRCMP(aw, p_ambw) != 0)
	{
	    // Setting the option causes a screen redraw. Do
	    // that right away if possible, keeping any
	    // messages.
	    సమితి_option_విలువ_give_err((char_u *)"ambw", 0L, (char_u *)aw, 0);
#నిర్వచితమైతే DEBUG_TERMRESPONSE
	    {
		పూర్ణాంకం r = redraw_asap(UPD_CLEAR);

		LOG_TRN("set 'ambiwidth', redraw_asap(): %d", r);
	    }
#లేకపోతే
	    redraw_asap(UPD_CLEAR);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_EVAL
	    సమితి_vim_var_string(VV_TERMU7RESP, tp, csi_len);
#అయితే_ముగింపు
	    apply_autocmds(EVENT_TERMRESPONSEALL,
					(char_u *)"ambiguouswidth", ఖాళీ, FALSE, curbuf);
	}
    }
    లేకపోతే ఒకవేళ (arg[0] == 3)
    {
	పూర్ణాంకం విలువ;

	LOG_TRN("Received compatibility test result: %s", tp);
	xcc_స్థితి.tr_పురోగతి = స్థితి_GOT;

	// Third row: xterm compatibility test.
	// If the cursor is on the first column then the terminal can handle
	// the request for cursor style and blinking.
	విలువ = arg[1] == 1 ? TPR_YES : TPR_NO;
	పదం_props[TPR_CURSOR_STYLE].tpr_స్థితి = విలువ;
	పదం_props[TPR_CURSOR_BLINK].tpr_స్థితి = విలువ;
    }
}



/*
 * Handle a response to T_CRV: {lead}{first}{x};{vers};{y}c
 * Xterm and alike use '>' for {first}.
 * Rxvt sends "{lead}?1;2c".
 */
    స్థిర శూన్య
handle_సంస్కరణ_response(పూర్ణాంకం మొదటి, పూర్ణాంకం *arg, పూర్ణాంకం argc, char_u *tp)
{
    // The xterm version.  It is set to zero when it can't be an actual xterm
    // version.
    పూర్ణాంకం సంస్కరణ_2 = arg[1];

    LOG_TRN("Received CRV response: %s", tp);
    crv_స్థితి.tr_పురోగతి = స్థితి_GOT;
    did_cursorhold = TRUE;

    // Reset terminal properties that are set based on the termresponse.
    // Mainly useful for tests that send the termresponse multiple times.
    // For testing all props can be reset.
    init_పదం_props(
#నిర్వచితమైతే FEAT_EVAL
	    reset_పదం_props_on_termresponse
#లేకపోతే
	    FALSE
#అయితే_ముగింపు
	    );

    // If this code starts with CSI, you can bet that the
    // terminal uses 8-bit codes.
    ఒకవేళ (tp[0] == CSI)
	switch_గమ్యం_8bit();

    // Screen sends 40500.
    // rxvt sends its version number: "20703" is 2.7.3.
    // Ignore it for when the user has set 'term' to xterm,
    // even though it's an rxvt.
    ఒకవేళ (సంస్కరణ_2 > 20000)
	సంస్కరణ_2 = 0;

    // Figure out more if the response is CSI > 99 ; 99 ; 99 c
    ఒకవేళ (మొదటి == '>' && argc == 3)
    {
	// mintty 2.9.5 sends 77;20905;0c.
	// (77 is ASCII 'M' for mintty.)
	ఒకవేళ (arg[0] == 77)
	{
	    // mintty can do SGR mouse reporting
	    పదం_props[TPR_MOUSE].tpr_స్థితి = TPR_MOUSE_SGR;
	}

#నిర్వచితమైతే FEAT_TERMRESPONSE
	// If xterm version >= 141 try to get termcap codes.  For other
	// terminals the request should be ignored.
	ఒకవేళ (సంస్కరణ_2 >= 141 && p_xtermcodes)
	{
	    LOG_TR1("Enable checking for XT codes");
	    check_for_codes = TRUE;
	    need_gather = TRUE;
	    req_codes_from_పదం();
	}
#అయితే_ముగింపు

	// libvterm sends 0;100;0
	// Konsole sends 0;115;0 and works the same way
	ఒకవేళ ((సంస్కరణ_2 == 100 || సంస్కరణ_2 == 115) && arg[0] == 0 && arg[2] == 0)
	{
	    // If run from Vim $COLORS is set to the number of
	    // colors the terminal supports.  Otherwise assume
	    // 256, libvterm supports even more.
	    ఒకవేళ (mch_getenv((char_u *)"COLORS") == ఖాళీ)
		may_adjust_color_లెక్క(256);
	    // Libvterm can handle SGR mouse reporting.
	    పదం_props[TPR_MOUSE].tpr_స్థితి = TPR_MOUSE_SGR;
	}

	ఒకవేళ (సంస్కరణ_2 == 95)
	{
	    // Mac Terminal.app sends 1;95;0
	    //
	    // Terminal.app doesn't seem to handle DECRQM sequences
	    // properly, see issue #19852.
	    ఒకవేళ (arg[0] == 1 && arg[2] == 0)
	    {
		పదం_props[TPR_UNDERLINE_RGB].tpr_స్థితి = TPR_YES;
		పదం_props[TPR_MOUSE].tpr_స్థితి = TPR_MOUSE_SGR;
		పదం_props[TPR_DECRQM].tpr_స్థితి = TPR_NO;
	    }
	    // iTerm2 sends 0;95;0
	    లేకపోతే ఒకవేళ (arg[0] == 0 && arg[2] == 0)
	    {
		// iTerm2 can do SGR mouse reporting
		పదం_props[TPR_MOUSE].tpr_స్థితి = TPR_MOUSE_SGR;
		పదం_props[TPR_DECRQM].tpr_స్థితి = TPR_YES;
	    }
	    // old iTerm2 sends 0;95;
	    లేకపోతే ఒకవేళ (arg[0] == 0 && arg[2] == -1)
		పదం_props[TPR_UNDERLINE_RGB].tpr_స్థితి = TPR_YES;
	}

	// screen sends 83;40500;0 83 is 'S' in ASCII.
	ఒకవేళ (arg[0] == 83)
	{
	    // screen supports SGR mouse codes since 4.7.0
	    ఒకవేళ (arg[1] >= 40700)
		పదం_props[TPR_MOUSE].tpr_స్థితి = TPR_MOUSE_SGR;
	    లేకపోతే
		పదం_props[TPR_MOUSE].tpr_స్థితి = TPR_MOUSE_XTERM;
	}

	// If no recognized terminal has set mouse behavior, assume xterm.
	ఒకవేళ (పదం_props[TPR_MOUSE].tpr_స్థితి == TPR_UNKNOWN)
	{
	    // Xterm version 277 supports SGR.
	    // Xterm version >= 95 supports mouse dragging.
	    ఒకవేళ (సంస్కరణ_2 >= 277)
		పదం_props[TPR_MOUSE].tpr_స్థితి = TPR_MOUSE_SGR;
	    లేకపోతే ఒకవేళ (సంస్కరణ_2 >= 95)
		పదం_props[TPR_MOUSE].tpr_స్థితి = TPR_MOUSE_XTERM2;
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
	ఒకవేళ (arg[1] >= 2500)
	{
	    పదం_props[TPR_UNDERLINE_RGB].tpr_స్థితి = TPR_YES;
	}

	లేకపోతే ఒకవేళ (సంస్కరణ_2 == 136 && arg[2] == 0)
	{
	    పదం_props[TPR_UNDERLINE_RGB].tpr_స్థితి = TPR_YES;

	    // PuTTY sends 0;136;0
	    ఒకవేళ (arg[0] == 0)
	    {
		// supports sgr-like mouse reporting.
		పదం_props[TPR_MOUSE].tpr_స్థితి = TPR_MOUSE_SGR;
	    }
	    // vandyke SecureCRT sends 1;136;0
	}

	// Konsole sends 0;115;0 - but t_u8 does not actually work, therefore
	// commented out.
	// else if (version == 115 && arg[0] == 0 && arg[2] == 0)
	//     term_props[TPR_UNDERLINE_RGB].tpr_status = TPR_YES;

	// Kitty up to 9.x sends 1;400{version};{secondary-version}
	ఒకవేళ (arg[0] == 1 && arg[1] >= 4000 && arg[1] <= 4009)
	{
	    పదం_props[TPR_KITTY].tpr_స్థితి = TPR_YES;
	    పదం_props[TPR_KITTY].tpr_సమితి_by_termresponse = TRUE;

	    // Kitty can handle SGR mouse reporting.
	    పదం_props[TPR_MOUSE].tpr_స్థితి = TPR_MOUSE_SGR;
	    పదం_props[TPR_DECRQM].tpr_స్థితి = TPR_YES;
	}

	// foot terminal sends 1;12700;0
	ఒకవేళ (arg[0] == 1 && సంస్కరణ_2 == 12700 && arg[2] == 0)
	{
	    పదం_props[TPR_MOUSE].tpr_స్థితి = TPR_MOUSE_SGR;
	    పదం_props[TPR_DECRQM].tpr_స్థితి = TPR_YES;
	}

	// GNU screen sends 83;30600;0, 83;40500;0, etc.
	// 30600/40500 is a version number of GNU screen. DA2 support is added
	// on 3.6.  DCS string has a special meaning to GNU screen, but xterm
	// compatibility checking does not detect GNU screen.
	ఒకవేళ (arg[0] == 83 && arg[1] >= 30600)
	{
	    పదం_props[TPR_CURSOR_STYLE].tpr_స్థితి = TPR_NO;
	    పదం_props[TPR_CURSOR_BLINK].tpr_స్థితి = TPR_NO;
	    పదం_props[TPR_DECRQM].tpr_స్థితి = TPR_NO; // screen doesn't seem
							// to handle DECRQM
							// sequences
	}

	// Xterm first responded to this request at patch level
	// 95, so assume anything below 95 is not xterm and hopefully supports
	// the underline RGB color sequence.
	ఒకవేళ (సంస్కరణ_2 < 95)
	    పదం_props[TPR_UNDERLINE_RGB].tpr_స్థితి = TPR_YES;

	// Getting the cursor style is only supported properly by xterm since
	// version 279 (otherwise it returns 0x18).
	ఒకవేళ (సంస్కరణ_2 < 279)
	    పదం_props[TPR_CURSOR_STYLE].tpr_స్థితి = TPR_NO;

	/*
	 * Take action on the detected properties.
	 */

	// Unless the underline RGB color is expected to work, disable "t_8u".
	// It does not work for the real Xterm, it resets the background color.
	// This may cause some flicker.  Alternative would be to set "t_8u"
	// here if the terminal is expected to support it, but that might
	// conflict with what was set in the .vimrc.
	ఒకవేళ (పదం_props[TPR_UNDERLINE_RGB].tpr_స్థితి != TPR_YES
			&& *T_8U != NUL
			&& !option_was_సమితి((char_u *)"t_8u"))
	{
	    సమితి_string_option_ప్రత్యక్ష((char_u *)"t_8u", -1, (char_u *)"",
								  OPT_FREE, 0);
	}
#నిర్వచితమైతే FEAT_TERMRESPONSE
	ఒకవేళ (*T_8U != NUL && write_t_8u_state == MAYBE)
	    // Did skip writing t_8u, a complete redraw is needed.
	    redraw_later_clear();
	write_t_8u_state = OK;  // can output t_8u now
#అయితే_ముగింపు

	// Only set 'ttymouse' automatically if it was not set
	// by the user already.
	ఒకవేళ (!option_was_సమితి((char_u *)"ttym")
		&& (పదం_props[TPR_MOUSE].tpr_స్థితి == TPR_MOUSE_XTERM2
		    || పదం_props[TPR_MOUSE].tpr_స్థితి == TPR_MOUSE_SGR))
	{
	    సమితి_option_విలువ_give_err((char_u *)"ttym", 0L,
		    పదం_props[TPR_MOUSE].tpr_స్థితి == TPR_MOUSE_SGR
				    ? (char_u *)"sgr" : (char_u *)"xterm2", 0);
	}

#నిర్వచితమైతే FEAT_TERMRESPONSE
	పూర్ణాంకం need_flush = FALSE;

	// Only request the cursor style if t_SH and t_RS are
	// set. Only supported properly by xterm since version
	// 279 (otherwise it returns 0x18).
	// Only when getting the cursor style was detected to work.
	// Not for Terminal.app, it can't handle t_RS, it
	// echoes the characters to the screen.
	ఒకవేళ (rcs_స్థితి.tr_పురోగతి == స్థితి_GET
		&& పదం_props[TPR_CURSOR_STYLE].tpr_స్థితి == TPR_YES
		&& *T_CSH != NUL
		&& *T_CRS != NUL)
	{
	    MAY_WANT_గమ్యం_LOG_THIS;
	    LOG_TR1("Sending cursor style request");
	    బహిర్గతం_str(T_CRS);
	    termrequest_పంపిన(&rcs_స్థితి);
	    need_flush = TRUE;
	}

	// Only request the cursor blink mode if t_RC set. Not
	// for Gnome terminal, it can't handle t_RC, it
	// echoes the characters to the screen.
	// Only when getting the cursor style was detected to work.
	ఒకవేళ (rbm_స్థితి.tr_పురోగతి == స్థితి_GET
		&& పదం_props[TPR_CURSOR_BLINK].tpr_స్థితి == TPR_YES
		&& *T_చక్రీయ_తనిఖీ != NUL)
	{
	    MAY_WANT_గమ్యం_LOG_THIS;
	    LOG_TR1("Sending cursor blink mode request");
	    బహిర్గతం_str(T_చక్రీయ_తనిఖీ);
	    termrequest_పంపిన(&rbm_స్థితి);
	    need_flush = TRUE;
	}

	// Only request DEC modes via DECRQM when the terminal is known to
	// handle it.  Not for Apple Terminal.app or GNU screen, they echo
	// the trailing "p" to the screen.  See issue #19852.
	ఒకవేళ (decrqm_స్థితి.tr_పురోగతి == స్థితి_GET
		&& పదం_props[TPR_DECRQM].tpr_స్థితి == TPR_YES)
	{
	    MAY_WANT_గమ్యం_LOG_THIS;
	    LOG_TR1("Sending DECRQM requests");
	    కొరకు (పూర్ణాంకం i = 0; i < (పూర్ణాంకం)ARRAY_నిడివి(dec_modes); i++)
	    {
		vim_snprintf((అక్షరం *)IObuff, IOSIZE, "\033[?%d$p", dec_modes[i]);
		బహిర్గతం_str(IObuff);
	    }
	    termrequest_పంపిన(&decrqm_స్థితి);
	    need_flush = TRUE;

	}

	ఒకవేళ (need_flush)
	    బహిర్గతం_flush();
#అయితే_ముగింపు
    }
}

/*
 * Add "key" to "buf" and return the number of bytes used.
 * Handles special keys and multi-byte characters.
 */
    స్థిర పూర్ణాంకం
కూడు_కీలకం_గమ్యం_buf(పూర్ణాంకం కీలకం, char_u *buf)
{
    పూర్ణాంకం idx = 0;

    ఒకవేళ (IS_SPECIAL(కీలకం))
    {
	buf[idx++] = K_SPECIAL;
	buf[idx++] = KEY2TERMCAP0(కీలకం);
	buf[idx++] = KEY2TERMCAP1(కీలకం);
    }
    లేకపోతే ఒకవేళ (has_mbyte)
	idx += (*mb_char2bytes)(కీలకం, buf + idx);
    లేకపోతే
	buf[idx++] = కీలకం;
    తిరిగి idx;
}

/*
 * Shared between handle_key_with_modifier() and handle_csi_function_key().
 */
    స్థిర పూర్ణాంకం
put_కీలకం_modifiers_లోపల_typebuf(
	పూర్ణాంకం	కీలకం_arg,
	పూర్ణాంకం	modifiers_arg,
	పూర్ణాంకం	csi_len,
	పూర్ణాంకం	స్థానభ్రంశం,
	char_u	*buf,
	పూర్ణాంకం	bufsize,
	పూర్ణాంకం	*buflen)
{
    పూర్ణాంకం కీలకం = కీలకం_arg;
    పూర్ణాంకం modifiers = modifiers_arg;

    // Some keys need adjustment when the Ctrl modifier is used.
    కీలకం = may_adjust_కీలకం_for_ctrl(modifiers, కీలకం);

    // May remove the shift modifier if it's already included in the key.
    modifiers = may_remove_జరుపు_modifier(modifiers, కీలకం);

    // Produce modifiers with K_SPECIAL KS_MODIFIER {mod}
    // worst-case: 3-byte modifier + 4 byte multi-char key + NUL
    char_u string[MAX_కీలకం_సంకేతం_LEN + 2];
    పూర్ణాంకం new_slen = modifiers2keycode(modifiers, &కీలకం, string);

    // reject overlong key that would overflow string
    ఒకవేళ (కీలకం > 0x10FFFF)
	తిరిగి -1;

    // Add the bytes for the key.
    new_slen += కూడు_కీలకం_గమ్యం_buf(కీలకం, string + new_slen);

    string[new_slen] = NUL;
    ఒకవేళ (put_string_లోపల_typebuf(స్థానభ్రంశం, csi_len, string, new_slen,
						 buf, bufsize, buflen) == FAIL)
	తిరిగి -1;
    తిరిగి new_slen - csi_len + స్థానభ్రంశం;
}

/*
 * Parse the number from a CSI numbered sequence for an F1-F12 key:
 *	ESC [ {number} ~
 * Returns the key
 */
    స్థిర పూర్ణాంకం
parse_csi_f_కీలకాలు(పూర్ణాంకం arg)
{
    char_u కీలకం_name[2] = { 0, 0 };

    ఎంపిక (arg)
    {
	స్థితి 11: కీలకం_name[0] = 'k'; కీలకం_name[1] = '1'; ఆపు;  // K_F1
	స్థితి 12: కీలకం_name[0] = 'k'; కీలకం_name[1] = '2'; ఆపు;  // K_F2
	స్థితి 13: కీలకం_name[0] = 'k'; కీలకం_name[1] = '3'; ఆపు;  // K_F3
	స్థితి 14: కీలకం_name[0] = 'k'; కీలకం_name[1] = '4'; ఆపు;  // K_F4
	స్థితి 15: కీలకం_name[0] = 'k'; కీలకం_name[1] = '5'; ఆపు;  // K_F5
	స్థితి 17: కీలకం_name[0] = 'k'; కీలకం_name[1] = '6'; ఆపు;  // K_F6
	స్థితి 18: కీలకం_name[0] = 'k'; కీలకం_name[1] = '7'; ఆపు;  // K_F7
	స్థితి 19: కీలకం_name[0] = 'k'; కీలకం_name[1] = '8'; ఆపు;  // K_F8
	స్థితి 20: కీలకం_name[0] = 'k'; కీలకం_name[1] = '9'; ఆపు;  // K_F9
	స్థితి 21: కీలకం_name[0] = 'F'; కీలకం_name[1] = ';'; ఆపు;  // K_F10
	స్థితి 23: కీలకం_name[0] = 'F'; కీలకం_name[1] = '1'; ఆపు;  // K_F11
	స్థితి 24: కీలకం_name[0] = 'F'; కీలకం_name[1] = '2'; ఆపు;  // K_F12
    }
    ఒకవేళ (కీలకం_name[0])
	తిరిగి TERMCAP2_కీలకం(కీలకం_name[0], కీలకం_name[1]);
    // shouldn't happen
    తిరిగి arg;
}

/*
 * Handle a sequence with key and modifier, one of:
 *	{lead}27;{modifier};{key}~
 *	{lead}{key};{modifier}u
 * Returns the difference in length.
 */
    స్థిర పూర్ణాంకం
handle_కీలకం_with_modifier(
	పూర్ణాంకం	*arg,
	పూర్ణాంకం	csi_len,
	పూర్ణాంకం	స్థానభ్రంశం,
	char_u	*buf,
	పూర్ణాంకం	bufsize,
	పూర్ణాంకం	*buflen,
	పూర్ణాంకం	iskitty,
	పూర్ణాంకం	trail)
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
    ఒకవేళ (!iskitty
	    && (kitty_protocol_state == KKPS_INITIAL
		|| kitty_protocol_state == KKPS_OFF
		|| kitty_protocol_state == KKPS_AFTER_T_TE)
	    && పదం_props[TPR_KITTY].tpr_స్థితి != TPR_YES)
    {
#నిర్వచితమైతే FEAT_EVAL
	ch_log(ఖాళీ, "setting seenModifyOtherKeys to TRUE");
#అయితే_ముగింపు
	చూసిన_Modify_Other_కీలకాలు = TRUE;
    }

    పూర్ణాంకం కీలకం = iskitty ? arg[0] : arg[2];
    పూర్ణాంకం modifiers = decode_modifiers(arg[1]);

    // Some terminals do not apply the Shift modifier to the key.  To make
    // mappings consistent we do it here.  TODO: support more keys.
    ఒకవేళ ((modifiers & MOD_ముసుగు_జరుపు) && కీలకం >= 'a' && కీలకం <= 'z')
	కీలకం += 'A' - 'a';

    // Putting Esc in the buffer creates ambiguity, it can be the start of an
    // escape sequence.  Use K_ESC to avoid that.
    ఒకవేళ (కీలకం == ESC)
	కీలకం = K_ESC;

    లేకపోతే ఒకవేళ (arg[0] >= 11 && arg[0] <= 24 && trail == '~')
	కీలకం = parse_csi_f_కీలకాలు(arg[0]);

    తిరిగి put_కీలకం_modifiers_లోపల_typebuf(కీలకం, modifiers, csi_len, స్థానభ్రంశం, buf,
	    bufsize, buflen);
}

/*
 * Handle a sequence with key without a modifier:
 *	{lead}{key}u
 *	{lead}{key}~
 * Returns the difference in length.
 */
    స్థిర పూర్ణాంకం
handle_కీలకం_without_modifier(
	పూర్ణాంకం	*arg,
	పూర్ణాంకం	csi_len,
	పూర్ణాంకం	స్థానభ్రంశం,
	char_u	*buf,
	పూర్ణాంకం	bufsize,
	పూర్ణాంకం	*buflen,
	పూర్ణాంకం	trail)
{
    char_u  string[MAX_కీలకం_సంకేతం_LEN + 1];
    పూర్ణాంకం	    new_slen;

    ఒకవేళ (arg[0] == ESC)
    {
	// Putting Esc in the buffer creates ambiguity, it can be the start of
	// an escape sequence.  Use K_ESC to avoid that.
	string[0] = K_SPECIAL;
	string[1] = KS_EXTRA;
	string[2] = KE_ESC;
	new_slen = 3;
    }
    లేకపోతే ఒకవేళ (arg[0] >= 11 && arg[0] <= 24 && trail == '~')
    {
	పూర్ణాంకం కీలకం = parse_csi_f_కీలకాలు(arg[0]);
	string[0] = K_SPECIAL;
	string[1] = KEY2TERMCAP0(కీలకం);
	string[2] = KEY2TERMCAP1(కీలకం);
	new_slen = 3;
    }
    లేకపోతే
	new_slen = కూడు_కీలకం_గమ్యం_buf(arg[0], string);

    ఒకవేళ (put_string_లోపల_typebuf(స్థానభ్రంశం, csi_len, string, new_slen,
						 buf, bufsize, buflen) == FAIL)
	తిరిగి -1;
    తిరిగి new_slen - csi_len + స్థానభ్రంశం;
}

/*
 * CSI function key without or with modifiers:
 *	{lead}[ABCDEFHPQRS]
 *	{lead}1;{modifier}[ABCDEFHPQRS]
 * Returns zero when nog recognized, a positive number when recognized.
 */
    స్థిర పూర్ణాంకం
handle_csi_function_కీలకం(
	పూర్ణాంకం	argc,
	పూర్ణాంకం	*arg,
	పూర్ణాంకం	trail,
	పూర్ణాంకం	csi_len,
	char_u	*కీలకం_name,
	పూర్ణాంకం	స్థానభ్రంశం,
	char_u  *buf,
	పూర్ణాంకం	bufsize,
	పూర్ణాంకం	*buflen)
{
    కీలకం_name[0] = 'k';
    ఎంపిక (trail)
    {
	స్థితి 'A': కీలకం_name[1] = 'u'; ఆపు;  // K_UP
	స్థితి 'B': కీలకం_name[1] = 'd'; ఆపు;  // K_DOWN
	స్థితి 'C': కీలకం_name[1] = 'r'; ఆపు;  // K_RIGHT
	స్థితి 'D': కీలకం_name[1] = 'l'; ఆపు;  // K_LEFT

	// case 'E': keypad BEGIN - not supported
	స్థితి 'F': కీలకం_name[0] = '@'; కీలకం_name[1] = '7'; ఆపు;  // K_END
	స్థితి 'H': కీలకం_name[1] = 'h'; ఆపు;  // K_HOME

	స్థితి 'P': కీలకం_name[1] = '1'; ఆపు;  // K_F1
	స్థితి 'Q': కీలకం_name[1] = '2'; ఆపు;  // K_F2
	స్థితి 'R': కీలకం_name[1] = '3'; ఆపు;  // K_F3
	స్థితి 'S': కీలకం_name[1] = '4'; ఆపు;  // K_F4

	అప్రమేయ: తిరిగి 0;  // not recognized
    }

    పూర్ణాంకం కీలకం = TERMCAP2_కీలకం(కీలకం_name[0], కీలకం_name[1]);
    పూర్ణాంకం modifiers = argc == 2 ? decode_modifiers(arg[1]) : 0;
    put_కీలకం_modifiers_లోపల_typebuf(కీలకం, modifiers,
					csi_len, స్థానభ్రంశం, buf, bufsize, buflen);
    తిరిగి csi_len;
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
    స్థిర పూర్ణాంకం
handle_csi(
	char_u	*tp,
	పూర్ణాంకం	len,
	char_u	*argp,
	పూర్ణాంకం	స్థానభ్రంశం,
	char_u  *buf,
	పూర్ణాంకం	bufsize,
	పూర్ణాంకం	*buflen,
	char_u	*కీలకం_name,
	పూర్ణాంకం	*slen)
{
    పూర్ణాంకం		మొదటి = -1;  // optional char right after {lead}
    పూర్ణాంకం		trail;	     // char that ends CSI sequence
    పూర్ణాంకం		arg[3] = {-1, -1, -1};	// argument numbers
    పూర్ణాంకం		argc = 0;		// number of arguments
    char_u	*ap = argp;
    పూర్ణాంకం		csi_len;

    // Check for non-digit after CSI.
    ఒకవేళ (!VIM_ISDIGIT(*ap))
	మొదటి = *ap++;

    ఒకవేళ (మొదటి >= 'A' && మొదటి <= 'Z')
    {
	// If "first" is in [ABCDEFHPQRS] then it is actually the "trail" and
	// no argument follows.
	trail = మొదటి;
	మొదటి = -1;
	--ap;
    }
    లేకపోతే
    {
	// Find up to three argument numbers.
	కొరకు (argc = 0; argc < 3; )
	{
	    ఒకవేళ (ap >= tp + len)
		తిరిగి -1;
	    ఒకవేళ (*ap == ';')
		arg[argc++] = -1;  // omitted number
	    లేకపోతే ఒకవేళ (VIM_ISDIGIT(*ap))
	    {
		arg[argc] = 0;
		కొరకు (;;)
		{
		    ఒకవేళ (ap >= tp + len)
			తిరిగి -1;
		    ఒకవేళ (!VIM_ISDIGIT(*ap))
			ఆపు;
		    // avoid overflow
		    ఒకవేళ (arg[argc] <= (గరిష్ఠ_పూర్ణాంకం - 9) / 10)
			arg[argc] = arg[argc] * 10 + (*ap - '0');
		    ++ap;
		}
		++argc;
	    }
	    ఒకవేళ (*ap == ';')
		++ap;
	    లేకపోతే
		ఆపు;
	}

	// mrxvt has been reported to have "+" in the version. Assume
	// the escape sequence ends with a letter or one of "{|}~".
	అయితే (ap < tp + len
		&& !(*ap >= '{' && *ap <= '~')
		&& !ASCII_ISALPHA(*ap))
	    ++ap;
	ఒకవేళ (ap >= tp + len)
	    తిరిగి -1;
	trail = *ap;
    }

    csi_len = (పూర్ణాంకం)(ap - tp) + 1;

    // Response to XTQMODKEYS: "CSI > 4 ; Pv m" where Pv indicates the
    // modifyOtherKeys level.  Drop similar responses.
    ఒకవేళ (మొదటి == '>' && (argc == 1 || argc == 2) && trail == 'm')
    {
	ఒకవేళ (arg[0] == 4 && argc == 2)
	    modify_otherkeys_state = arg[1] == 2 ? MOKS_ENABLED : MOKS_OFF;

	కీలకం_name[0] = (పూర్ణాంకం)KS_EXTRA;
	కీలకం_name[1] = (పూర్ణాంకం)KE_IGNORE;
	*slen = csi_len;
    }

    // Function key starting with CSI:
    //	{lead}[ABCDEFHPQRS]
    //	{lead}1;{modifier}[ABCDEFHPQRS]
    లేకపోతే ఒకవేళ (మొదటి == -1 && ASCII_ISUPPER(trail)
	    && (argc == 0 || (argc == 2 && arg[0] == 1)))
    {
	పూర్ణాంకం res = handle_csi_function_కీలకం(argc, arg, trail,
			      csi_len, కీలకం_name, స్థానభ్రంశం, buf, bufsize, buflen);
	తిరిగి res <= 0 ? res : len + res;
    }

    // Cursor position report: {lead}{row};{col}R
    // Eat it when there are 2 arguments and it ends in 'R'.
    // Also when u7_status is not "sent", it may be from a previous Vim that
    // just exited.  But not for <S-F3>, it sends something similar, check for
    // row and column to make sense.
    లేకపోతే ఒకవేళ (మొదటి == -1 && argc == 2 && trail == 'R')
    {
	handle_u7_response(arg, tp, csi_len);

	కీలకం_name[0] = (పూర్ణాంకం)KS_EXTRA;
	కీలకం_name[1] = (పూర్ణాంకం)KE_IGNORE;
	*slen = csi_len;
    }

#ఒకవేళ (defined(UNIX) || defined(MSWIN)) \
    && (defined(FEAT_IMAGE) || defined(FEAT_EVAL))
    // Response to CSI 14 t or CSI 16 t
    లేకపోతే ఒకవేళ (మొదటి == -1 && argc >= 3
	    && (arg[0] == 4 || arg[0] == 6) && trail == 't')
    {
	ఒకవేళ (arg[0] == 4)
	{
	    LOG_TRN("Received CSI 14 t response: %s", tp);
	    cell_వెడల్పు = arg[2] / Columns;
	    cell_ఎత్తు = arg[1] / అడ్డువరుసలు;
	}
	లేకపోతే
	{
	    LOG_TRN("Received CSI 16 t response: %s", tp);
	    cell_వెడల్పు = arg[2];
	    cell_ఎత్తు = arg[1];
	}

	కీలకం_name[0] = (పూర్ణాంకం)KS_EXTRA;
	కీలకం_name[1] = (పూర్ణాంకం)KE_IGNORE;
	*slen = csi_len;
    }
#అయితే_ముగింపు

    // Primary device attributes (DA1) response
    లేకపోతే ఒకవేళ (మొదటి == '?' && trail == 'c')
    {
	LOG_TRN("Received DA1 response: %s", tp);

	*slen = csi_len;
#నిర్వచితమైతే FEAT_EVAL
	సమితి_vim_var_string(VV_TERMDA1, tp, *slen);
#అయితే_ముగింపు
	apply_autocmds(EVENT_TERMRESPONSEALL,
					(char_u *)"da1", ఖాళీ, FALSE, curbuf);

	కీలకం_name[0] = (పూర్ణాంకం)KS_EXTRA;
	కీలకం_name[1] = (పూర్ణాంకం)KE_IGNORE;
	*slen = csi_len;
    }

    // DECRPM mode 2026 or 2048.
    లేకపోతే ఒకవేళ (మొదటి == '?' && trail == 'y' && argc == 2
	    && (arg[0] == 2026 || arg[0] == 2048))
    {
	పూర్ణాంకం setting = arg[1];

	*slen = csi_len;
	కీలకం_name[0] = (పూర్ణాంకం)KS_EXTRA;
	కీలకం_name[1] = (పూర్ణాంకం)KE_IGNORE;

#నిర్వచితమైతే FEAT_TERMRESPONSE
	// Mark the DECRQM request as answered so it is not sent again and
	// stoptermcap() does not wait for it.
	ఒకవేళ (decrqm_స్థితి.tr_పురోగతి == స్థితి_పంపిన)
	    decrqm_స్థితి.tr_పురోగతి = స్థితి_GOT;
#అయితే_ముగింపు

	ఒకవేళ (setting >= 0 && setting <= 4)
	{
	    LOG_TRN("Received DECRPM mode %d: %s", arg[0], tp);

	    ఎంపిక (arg[0])
	    {
		స్థితి 2026:
		    sync_output_setting = setting;
		    సమితి_option_విలువ_give_err((char_u *)"termsync",
			    setting == 1 || setting == 2, ఖాళీ, 0);
		    ఆపు;
#నిర్వచితమైతే UNIX
		స్థితి 2048:
		    win_resize_setting = setting;

		    పదం_సమితి_win_resize(true);
		    ఆపు;
#అయితే_ముగింపు
	    }
	}
	లేకపోతే
	    LOG_TRN("Unknown DECRPM mode %d setting %d", arg[0], setting);
    }

#నిర్వచితమైతే UNIX
    // In-band window resize event
    లేకపోతే ఒకవేళ (win_resize_enabled && argc >= 3 && arg[0] == 48)
    {
	పూర్ణాంకం ఎత్తు = arg[1], వెడల్పు = arg[2];

	*slen = csi_len;
	కీలకం_name[0] = (పూర్ణాంకం)KS_EXTRA;
	కీలకం_name[1] = (పూర్ణాంకం)KE_IGNORE;

	// Only update if needed. Avoids intro message from disappearing on
	// startup (due to initial event).
	ఒకవేళ (ఎత్తు != అడ్డువరుసలు || వెడల్పు != Columns)
	    సమితి_shellsize(వెడల్పు, ఎత్తు, true);
    }
#అయితే_ముగింపు

    // Version string: Eat it when there is at least one digit and
    // it ends in 'c'
    లేకపోతే ఒకవేళ (*T_CRV != NUL && ap > argp + 1 && trail == 'c')
    {
	handle_సంస్కరణ_response(మొదటి, arg, argc, tp);

	*slen = csi_len;
#నిర్వచితమైతే FEAT_EVAL
	సమితి_vim_var_string(VV_TERMRESPONSE, tp, *slen);
#అయితే_ముగింపు
	apply_autocmds(EVENT_TERMRESPONSE,
					ఖాళీ, ఖాళీ, FALSE, curbuf);
	apply_autocmds(EVENT_TERMRESPONSEALL,
					(char_u *)"version", ఖాళీ, FALSE, curbuf);
	కీలకం_name[0] = (పూర్ణాంకం)KS_EXTRA;
	కీలకం_name[1] = (పూర్ణాంకం)KE_IGNORE;
    }

#నిర్వచితమైతే FEAT_TERMRESPONSE
    // Check blinking cursor from xterm:
    // {lead}?12;1$y       set
    // {lead}?12;2$y       not set
    //
    // {lead} can be <Esc>[ or CSI
    లేకపోతే ఒకవేళ (rbm_స్థితి.tr_పురోగతి == స్థితి_పంపిన
	    && మొదటి == '?'
	    && ap == argp + 6
	    && arg[0] == 12
	    && ap[-1] == '$'
	    && trail == 'y')
    {
	initial_cursor_blink = (arg[1] == '1');
	rbm_స్థితి.tr_పురోగతి = స్థితి_GOT;
	LOG_TRN("Received cursor blinking mode response: %s", tp);
	కీలకం_name[0] = (పూర్ణాంకం)KS_EXTRA;
	కీలకం_name[1] = (పూర్ణాంకం)KE_IGNORE;
	*slen = csi_len;
# ifdef FEAT_EVAL
	సమితి_vim_var_string(VV_TERMBLINKRESP, tp, *slen);
# endif
	apply_autocmds(EVENT_TERMRESPONSEALL,
					(char_u *)"cursorblink", ఖాళీ, FALSE, curbuf);
    }
#అయితే_ముగింపు

    // Kitty keyboard protocol status response: CSI ? flags u
    లేకపోతే ఒకవేళ (మొదటి == '?' && argc == 1 && trail == 'u')
    {
	// The protocol has various "progressive enhancement flags" values, but
	// we only check for zero and non-zero here.
	ఒకవేళ (arg[0] == '0')
	{
	    kitty_protocol_state = KKPS_OFF;
	}
	లేకపోతే
	{
	    kitty_protocol_state = KKPS_ENABLED;

	    // Reset seenModifyOtherKeys just in case some key combination has
	    // been seen that set it before we get the status response.
#నిర్వచితమైతే FEAT_EVAL
	    ch_log(ఖాళీ, "setting seenModifyOtherKeys to FALSE");
#అయితే_ముగింపు
	    చూసిన_Modify_Other_కీలకాలు = FALSE;
	}

	కీలకం_name[0] = (పూర్ణాంకం)KS_EXTRA;
	కీలకం_name[1] = (పూర్ణాంకం)KE_IGNORE;
	*slen = csi_len;
    }

#నిర్వచితమైతే FEAT_TERMRESPONSE
    // Check for a window position response from the terminal:
    //       {lead}3;{x};{y}t
    లేకపోతే ఒకవేళ (did_request_winpos && argc == 3 && arg[0] == 3
						   && trail == 't')
    {
	winpos_x = arg[1];
	winpos_y = arg[2];
	// got finished code: consume it
	కీలకం_name[0] = (పూర్ణాంకం)KS_EXTRA;
	కీలకం_name[1] = (పూర్ణాంకం)KE_IGNORE;
	*slen = csi_len;

	ఒకవేళ (--did_request_winpos <= 0)
	    winpos_స్థితి.tr_పురోగతి = స్థితి_GOT;
    }
#అయితే_ముగింపు

    // Key with modifier:
    //	{lead}27;{modifier};{key}~
    //	{lead}{key};{modifier}u
    //	{lead}{key};{modifier}~
    // Even though we only handle four modifiers and the {modifier} value
    // should be 16 or lower, we accept all modifier values to avoid the raw
    // sequence to be passed through.
    లేకపోతే ఒకవేళ ((arg[0] == 27 && argc == 3 && trail == '~')
		|| (argc == 2 && (trail == 'u' || trail == '~')))
    {
	పూర్ణాంకం iskitty = argc == 2 && (trail == 'u' || trail == '~');
	తిరిగి len + handle_కీలకం_with_modifier(arg, csi_len, స్థానభ్రంశం, buf,
		bufsize, buflen, iskitty, trail);
    }

    // Key without modifier (Kitty sends this for Esc or F3):
    //	{lead}{key}u
    //	{lead}{key}~
    లేకపోతే ఒకవేళ (argc == 1 && (trail == 'u' || trail == '~'))
    {
	తిరిగి len + handle_కీలకం_without_modifier(arg, csi_len, స్థానభ్రంశం, buf,
		bufsize, buflen, trail);
    }

    // else: Unknown CSI sequence.  We could drop it, but then the
    // user can't create a map for it.
    తిరిగి 0;
}

స్థిర శూన్య
check_for_color_response(char_u *resp, పూర్ణాంకం len)
{
    పూర్ణాంకం		i, j;
    char_u	*argp;

    j = 1 + (resp[0] == ESC);
    argp = resp + j;

    ఒకవేళ (len >= j + 3 && (argp[0] != '1'
			     || (argp[1] != '1' && argp[1] != '0')
			     || argp[2] != ';'))
	i = 0; // no match
    లేకపోతే
    {
	కొరకు (i = j; i < len; ++i)
	    ఒకవేళ (resp[i] == '\007' || (resp[0] == OSC ? resp[i] == STERM
			: (resp[i] == ESC && i + 1 < len && resp[i + 1] == '\\')))
	    {
		పూర్ణాంకం is_bg = argp[1] == '1';
		పూర్ణాంకం is_4digit = i - j >= 21 && resp[j + 11] == '/'
						  && resp[j + 16] == '/';

		ఒకవేళ (i - j >= 15 && STRNCMP(resp + j + 3, "rgb:", 4) == 0
			    && (is_4digit
				   || (resp[j + 9] == '/' && resp[j + 12] == '/')))
		{
		    char_u *tp_r = resp + j + 7;
		    char_u *tp_g = resp + j + (is_4digit ? 12 : 10);
		    char_u *tp_b = resp + j + (is_4digit ? 17 : 13);
#నిర్వచితమైతే FEAT_TERMRESPONSE
		    పూర్ణాంకం rval, gval, bval;

		    rval = hexhex2nr(tp_r);
		    gval = hexhex2nr(tp_g);
		    bval = hexhex2nr(tp_b);
#అయితే_ముగింపు
		    ఒకవేళ (is_bg)
		    {
			అక్షరం *new_bg_val = (3 * '6' < *tp_r + *tp_g +
					     *tp_b) ? "light" : "dark";

			LOG_TRN("Received RBG response: r=%d g=%d b=%d", rval, gval, bval);
#నిర్వచితమైతే FEAT_TERMRESPONSE
			rbg_స్థితి.tr_పురోగతి = స్థితి_GOT;
			bg_r = rval;
			bg_g = gval;
			bg_b = bval;
#అయితే_ముగింపు
			ఒకవేళ (!option_was_సమితి((char_u *)"bg")
				      && STRCMP(p_bg, new_bg_val) != 0)
			{
			    // value differs, apply it
			    సమితి_option_విలువ_give_err((char_u *)"bg",
						  0L, (char_u *)new_bg_val, 0);
			    reset_option_was_సమితి((char_u *)"bg");
			    redraw_asap(UPD_CLEAR);
			}
		    }
#ఒకవేళ defined(FEAT_TERMRESPONSE) && defined(FEAT_TERMINAL)
		    లేకపోతే
		    {
			LOG_TRN("Received RFG response: r=%d g=%d b=%d", rval, gval, bval);
			rfg_స్థితి.tr_పురోగతి = స్థితి_GOT;
			fg_r = rval;
			fg_g = gval;
			fg_b = bval;
		    }
#అయితే_ముగింపు
		}

#నిర్వచితమైతే FEAT_EVAL
		సమితి_vim_var_string(is_bg ? VV_TERMRBGRESP
						  : VV_TERMRFGRESP, resp, len);
#అయితే_ముగింపు
		apply_autocmds(EVENT_TERMRESPONSEALL,
			is_bg ? (char_u *)"background" : (char_u *)"foreground",
			ఖాళీ, FALSE, curbuf);
		ఆపు;
	    }
    }
    ఒకవేళ (i == len)
	LOG_TR1("not enough characters for RB");
}

స్థిర oscstate_T osc_state;

/*
 * Return true if currently receiving an OSC response.
 */
    bool
లోపల_osc_sequence(శూన్య)
{
    తిరిగి osc_state.processing;
}

/*
 * Handles any OSC sequence and places the result in "v:termosc". Returns OK on
 * success and FAIL on failure.
 */
    స్థిర పూర్ణాంకం
handle_osc(char_u *tp, పూర్ణాంకం len, char_u *కీలకం_name, పూర్ణాంకం *slen)
{
    char_u చివరి_char;

    ఒకవేళ (!osc_state.processing)
    {
	పూర్ణాంకం cur;

	LOG_TRN("Received OSC response: %s", (అక్షరం*)tp);
	// Check if it is a valid OSC sequence, and consume it. OSC format
	// consists of:
	// <identifier><data><terminator>
	// <identifier> is either <Esc>] or an OSC character
	// <terminator> can be '\007', <Esc>\ or STERM.
	cur = 1 + (tp[0] == ESC);

	ఒకవేళ (len < cur + 1 + (tp[0] != OSC)) // Include terminator as well
	    తిరిగి FAIL;

	// The whole OSC response may be larger than the typeahead buffer.
	// To handle this, keep reading data in and out of the typeahead
	// buffer until we read an OSC terminator or timeout.

	// We can't use the previous buffer since we transferred ownership of it
	// to the vim var.
	ga_init2(&osc_state.buf, 1, 1024);
#నిర్వచితమైతే ELAPSED_FUNC
	ELAPSED_INIT(osc_state.ప్రారంభం_tv);
#అయితే_ముగింపు
	osc_state.processing = TRUE;
	osc_state.ప్రారంభం_char = tp[0];
	చివరి_char = 0;
    }
    లేకపోతే
	చివరి_char = ((char_u *)osc_state.buf.ga_దత్తాంశం)[osc_state.buf.ga_len - 1];

    కీలకం_name[0] = (పూర్ణాంకం)KS_EXTRA;

    // Read data and append to buffer. If we reach a terminator, then
    // finally set the vim var.
    కొరకు (పూర్ణాంకం i = 0; i < len; i++)
	ఒకవేళ (tp[i] == '\007' || (osc_state.ప్రారంభం_char == OSC ? tp[i] == STERM
		    : ((tp[i] == ESC && i + 1 < len && tp[i + 1] == '\\')
			|| (i == 0 && tp[i] == '\\' && చివరి_char == ESC)
			)))
	{
	    osc_state.processing = FALSE;

	    కీలకం_name[1] = (పూర్ణాంకం)KE_OSC;

	    ga_concat_len(&osc_state.buf, tp, i + 1 + (tp[i] == ESC));
	    ga_append(&osc_state.buf, NUL);
	    *slen = i + 1 + (tp[i] == ESC);
#నిర్వచితమైతే FEAT_EVAL
	    సమితి_vim_var_string_ప్రత్యక్ష(VV_TERMOSC, osc_state.buf.ga_దత్తాంశం);
#అయితే_ముగింపు
	    // Check for background/foreground colour response
	    check_for_color_response(osc_state.buf.ga_దత్తాంశం, osc_state.buf.ga_len - 1);

	    char_u savebg = *p_bg;
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"osc",
		    ఖాళీ, FALSE, curbuf);
	    ఒకవేళ (*p_bg != savebg)
		redraw_asap(UPD_CLEAR);
	    తిరిగి OK;
	}

    కీలకం_name[1] = (పూర్ణాంకం)KE_IGNORE;

#నిర్వచితమైతే ELAPSED_FUNC
    ఒకవేళ (ELAPSED_FUNC(osc_state.ప్రారంభం_tv) >= p_ost)
    {
	semsg(_(e_osc_response_timed_బహిర్గతం), osc_state.buf.ga_len,
		osc_state.buf.ga_దత్తాంశం);

	ga_clear(&osc_state.buf);
	osc_state.processing = FALSE;
	తిరిగి FAIL;
    }
#అయితే_ముగింపు

    ga_concat(&osc_state.buf, tp);
    *slen = len; // Consume everything

    తిరిగి OK;
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
    స్థిర పూర్ణాంకం
handle_dcs(char_u *tp, char_u *argp, పూర్ణాంకం len, char_u *కీలకం_name, పూర్ణాంకం *slen)
{
    పూర్ణాంకం i, j;

    LOG_TRN("Received DCS response: %s", (అక్షరం*)tp);
    j = 1 + (tp[0] == ESC);
    ఒకవేళ (len < j + 3)
	i = len; // need more chars
    లేకపోతే ఒకవేళ ((argp[1] != '+' && argp[1] != '$')
	    || (argp[2] != 'r' && argp[2] != 'R'))
	i = 0; // no match
    లేకపోతే ఒకవేళ (argp[1] == '+')
	// key code response
	కొరకు (i = j; i < len; ++i)
	{
	    ఒకవేళ ((tp[i] == ESC && i + 1 < len && tp[i + 1] == '\\')
		    || tp[i] == STERM)
	    {
#నిర్వచితమైతే FEAT_TERMRESPONSE
		// handle a key code response, drop a resource response
		ఒకవేళ (i - j >= 3 && argp[2] == 'r')
		    got_సంకేతం_from_పదం(tp + j, i);
#అయితే_ముగింపు
		కీలకం_name[0] = (పూర్ణాంకం)KS_EXTRA;
		కీలకం_name[1] = (పూర్ణాంకం)KE_IGNORE;
		*slen = i + 1 + (tp[i] == ESC);
		ఆపు;
	    }
	}
    లేకపోతే
    {
	// Probably the cursor shape response.  Make sure that "i"
	// is equal to "len" when there are not sufficient
	// characters.
	కొరకు (i = j + 3; i < len; ++i)
	{
	    ఒకవేళ (i - j == 3 && !సురక్షిత_isdigit(tp[i]))
		ఆపు;
	    ఒకవేళ (i - j == 4 && tp[i] != ' ')
		ఆపు;
	    ఒకవేళ (i - j == 5 && tp[i] != 'q')
		ఆపు;
	    ఒకవేళ (i - j == 6 && tp[i] != ESC && tp[i] != STERM)
		ఆపు;
	    ఒకవేళ ((i - j == 6 && tp[i] == STERM)
		    || (i - j == 7 && tp[i] == '\\'))
	    {
#నిర్వచితమైతే FEAT_TERMRESPONSE
		పూర్ణాంకం సంఖ్య = argp[3] - '0';

		// 0, 1 = block blink, 2 = block
		// 3 = underline blink, 4 = underline
		// 5 = vertical bar blink, 6 = vertical bar
		సంఖ్య = సంఖ్య == 0 ? 1 : సంఖ్య;
		initial_cursor_shape = (సంఖ్య + 1) / 2;
		// The blink flag is actually inverted, compared to
		// the value set with T_SH.
		initial_cursor_shape_blink =
				       (సంఖ్య & 1) ? FALSE : TRUE;
		rcs_స్థితి.tr_పురోగతి = స్థితి_GOT;
#అయితే_ముగింపు
		LOG_TRN("Received cursor shape response: %s", tp);

		కీలకం_name[0] = (పూర్ణాంకం)KS_EXTRA;
		కీలకం_name[1] = (పూర్ణాంకం)KE_IGNORE;
		*slen = i + 1;
#నిర్వచితమైతే FEAT_EVAL
		సమితి_vim_var_string(VV_TERMSTYLERESP, tp, *slen);
#అయితే_ముగింపు
		apply_autocmds(EVENT_TERMRESPONSEALL,
					(char_u *)"cursorshape", ఖాళీ, FALSE, curbuf);
		ఆపు;
	    }
	}
    }

    ఒకవేళ (i == len)
    {
	// These codes arrive many together, each code can be
	// truncated at any point.
	LOG_TR1("not enough characters for XT");
	తిరిగి FAIL;
    }
    తిరిగి OK;
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
    పూర్ణాంకం
check_termcode(
    పూర్ణాంకం		max_స్థానభ్రంశం,
    char_u	*buf,
    పూర్ణాంకం		bufsize,
    పూర్ణాంకం		*buflen)
{
    char_u	*tp;
    char_u	*p;
    పూర్ణాంకం		slen = 0;	// init for GCC
    పూర్ణాంకం		modslen;
    పూర్ణాంకం		len;
    పూర్ణాంకం		retval = 0;
    పూర్ణాంకం		స్థానభ్రంశం;
    char_u	కీలకం_name[2];
    పూర్ణాంకం		modifiers;
    char_u	*modifiers_ప్రారంభం = ఖాళీ;
    పూర్ణాంకం		కీలకం;
    పూర్ణాంకం		new_slen;   // Length of what will replace the termcode
    char_u	string[MAX_కీలకం_సంకేతం_LEN + 1];
    పూర్ణాంకం		i, j;
    పూర్ణాంకం		idx = 0;
    పూర్ణాంకం		cpo_koffset;

    cpo_koffset = (vim_strchr(p_cpo, CPO_KOFFSET) != ఖాళీ);

    /*
     * Speed up the checks for terminal codes by gathering all first bytes
     * used in termleader[].  Often this is just a single <Esc>.
     */
    ఒకవేళ (need_gather)
	gather_termleader();

    /*
     * Check at several positions in typebuf.tb_buf[], to catch something like
     * "x<Up>" that can be mapped. Stop at max_offset, because characters
     * after that cannot be used for mapping, and with @r commands
     * typebuf.tb_buf[] can become very long.
     * This is used often, KEEP IT FAST!
     */
    కొరకు (స్థానభ్రంశం = 0; స్థానభ్రంశం < max_స్థానభ్రంశం; ++స్థానభ్రంశం)
    {
	ఒకవేళ (buf == ఖాళీ)
	{
	    ఒకవేళ (స్థానభ్రంశం >= typebuf.tb_len)
		ఆపు;
	    tp = typebuf.tb_buf + typebuf.tb_off + స్థానభ్రంశం;
	    len = typebuf.tb_len - స్థానభ్రంశం;	// length of the input
	}
	లేకపోతే
	{
	    ఒకవేళ (స్థానభ్రంశం >= *buflen)
		ఆపు;
	    tp = buf + స్థానభ్రంశం;
	    len = *buflen - స్థానభ్రంశం;
	}

	/*
	 * Don't check characters after K_SPECIAL, those are already
	 * translated terminal chars (avoid translating ~@^Hx).
	 */
	ఒకవేళ (*tp == K_SPECIAL)
	{
	    స్థానభ్రంశం += 2;	// there are always 2 extra characters
	    కొనసాగు;
	}

	ఒకవేళ (osc_state.processing)
	{
	    // Still processing OSC response data, go straight to handler
	    // function.
	    tp[len] = NUL;
	    కీలకం_name[0] = NUL;
	    కీలకం_name[1] = NUL;
	    modifiers = 0;
	    వెళ్ళు handle_osc;
	}

	/*
	 * Skip this position if the character does not appear as the first
	 * character in term_strings. This speeds up a lot, since most
	 * termcodes start with the same character (ESC or CSI).
	 */
	i = *tp;
	కొరకు (p = termleader; *p && *p != i; ++p)
	    ;
	ఒకవేళ (*p == NUL)
	    కొనసాగు;

	/*
	 * Skip this position if p_ek is not set and tp[0] is an ESC and we
	 * are in Insert mode.
	 */
	ఒకవేళ (*tp == ESC && !p_ek && (State & బహుళకం_చొప్పించు))
	    కొనసాగు;

	tp[len] = NUL;
	కీలకం_name[0] = NUL;	// no key name found yet
	కీలకం_name[1] = NUL;	// no key name found yet
	modifiers = 0;		// no modifiers yet

#నిర్వచితమైతే FEAT_GUI
	ఒకవేళ (gui.లోపల_use)
	{
	    /*
	     * GUI special key codes are all of the form [CSI xx].
	     */
	    ఒకవేళ (*tp == CSI)	    // Special key from GUI
	    {
		ఒకవేళ (len < 3)
		    తిరిగి -1;	    // Shouldn't happen
		slen = 3;
		కీలకం_name[0] = tp[1];
		కీలకం_name[1] = tp[2];
	    }
	}
	లేకపోతే
#అయితే_ముగింపు // FEAT_GUI
#నిర్వచితమైతే MSWIN
	    ఒకవేళ (len >= 3 && tp[0] == CSI && tp[1] == KS_EXTRA
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
	    కీలకం_name[0] = tp[1];
	    కీలకం_name[1] = tp[2];
	}
	లేకపోతే
#అయితే_ముగింపు
	{
	    పూర్ణాంకం  mouse_సూచీ_దొరికింది = -1;
	    పూర్ణాంకం keypad_సూచీ_దొరికింది = -1;
	    పూర్ణాంకం keypad_slen_దొరికింది = 0;

	    కొరకు (idx = 0; idx < tc_len; ++idx)
	    {
		పూర్ణాంకం     is_keypad = FALSE;
		/*
		 * Ignore the entry if we are not at the start of
		 * typebuf.tb_buf[]
		 * and there are not enough characters to make a match.
		 * But only when the 'K' flag is in 'cpoptions'.
		 */
		slen = termcodes[idx].len;
		modifiers_ప్రారంభం = ఖాళీ;
		ఒకవేళ (cpo_koffset && స్థానభ్రంశం && len < slen)
		    కొనసాగు;
		ఒకవేళ (STRNCMP(termcodes[idx].సంకేతం_2, tp,
				     (size_t)(slen > len ? len : slen)) == 0)
		{
		    పూర్ణాంకం	    looks_like_mouse_ప్రారంభం = FALSE;

		    ఒకవేళ (len < slen)		// got a partial sequence
			తిరిగి -1;		// need to get more chars

		    /*
		     * When found a keypad key, check if there is another key
		     * that matches and use that one.  This makes <Home> to be
		     * found instead of <kHome> when they produce the same
		     * key code.
		     */
		    ఒకవేళ (termcodes[idx].name[0] == 'K'
				       && (VIM_ISDIGIT(termcodes[idx].name[1])
						|| ASCII_ISUPPER(termcodes[idx].name[1])))
		    {
			is_keypad = TRUE;
			// Only use it when there is no other match.
			ఒకవేళ (keypad_సూచీ_దొరికింది < 0)
			{
			    keypad_సూచీ_దొరికింది = idx;
			    keypad_slen_దొరికింది = slen;
			}
		    }

		    ఒకవేళ (slen == 2 && len > 2
			    && termcodes[idx].సంకేతం_2[0] == ESC
			    && termcodes[idx].సంకేతం_2[1] == '[')
		    {
			// The mouse termcode "ESC [" is also the prefix of
			// "ESC [ I" (focus gained) and other keys.  Check some
			// more bytes to find out.
			ఒకవేళ (!సురక్షిత_isdigit(tp[2]))
			{
			    // ESC [ without number following: Only use it when
			    // there is no other match.
			    looks_like_mouse_ప్రారంభం = TRUE;
			}
			లేకపోతే ఒకవేళ (termcodes[idx].name[0] == KS_DEC_MOUSE)
			{
			    char_u  *nr = tp + 2;
			    పూర్ణాంకం	    లెక్క = 0;

			    // If a digit is following it could be a key with
			    // modifier, e.g., ESC [ 1;2P.  Can be confused
			    // with DEC_MOUSE, which requires four numbers
			    // following.  If not then it can't be a DEC_MOUSE
			    // code.
			    కొరకు (;;)
			    {
				++లెక్క;
				(శూన్య)getdigits(&nr);
				ఒకవేళ (nr >= tp + len)
				    తిరిగి -1;	// partial sequence
				ఒకవేళ (*nr != ';')
				    ఆపు;
				++nr;
				ఒకవేళ (nr >= tp + len)
				    తిరిగి -1;	// partial sequence
			    }
			    ఒకవేళ (లెక్క < 4)
				కొనసాగు;	// no match
			}
		    }
		    ఒకవేళ (looks_like_mouse_ప్రారంభం)
		    {
			// Only use it when there is no other match.
			ఒకవేళ (mouse_సూచీ_దొరికింది < 0)
			    mouse_సూచీ_దొరికింది = idx;
		    }
		    లేకపోతే ఒకవేళ (!is_keypad)
		    {
			కీలకం_name[0] = termcodes[idx].name[0];
			కీలకం_name[1] = termcodes[idx].name[1];
			ఆపు;
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
		ఒకవేళ (termcodes[idx].modlen > 0 && mouse_సూచీ_దొరికింది < 0)
		{
		    modslen = termcodes[idx].modlen;
		    ఒకవేళ (cpo_koffset && స్థానభ్రంశం && len < modslen)
			కొనసాగు;
		    ఒకవేళ (STRNCMP(termcodes[idx].సంకేతం_2, tp,
				(size_t)(modslen > len ? len : modslen)) == 0)
		    {
			పూర్ణాంకం	    n;

			ఒకవేళ (len <= modslen)	// got a partial sequence
			    తిరిగి -1;		// need to get more chars

			ఒకవేళ (tp[modslen] == termcodes[idx].సంకేతం_2[slen - 1])
			    // no modifiers
			    slen = modslen + 1;
			లేకపోతే ఒకవేళ (tp[modslen] != ';' && modslen == slen - 3)
			    // no match for "code;*X" with "code;"
			    కొనసాగు;
			లేకపోతే ఒకవేళ (termcodes[idx].సంకేతం_2[modslen] == '@'
					 && (tp[modslen] != '1'
						    || tp[modslen + 1] != ';'))
			    // no match for "<Esc>[@" with "<Esc>[1;"
			    కొనసాగు;
			లేకపోతే
			{
			    // Skip over the digits, the final char must
			    // follow. URXVT can use a negative value, thus
			    // also accept '-'.
			    కొరకు (j = slen - 2; j < len && (సురక్షిత_isdigit(tp[j])
				       || tp[j] == '-' || tp[j] == ';'); ++j)
				;
			    ++j;
			    ఒకవేళ (len < j)	// got a partial sequence
				తిరిగి -1;	// need to get more chars
			    ఒకవేళ (tp[j - 1] != termcodes[idx].సంకేతం_2[slen - 1])
				కొనసాగు;	// no match

			    modifiers_ప్రారంభం = tp + slen - 2;

			    // Match!  Convert modifier bits.
			    n = atoi((అక్షరం *)modifiers_ప్రారంభం);
			    modifiers |= decode_modifiers(n);

			    slen = j;
			}
			ఒకవేళ (termcodes[idx].name[0] == 'K'
					   && (VIM_ISDIGIT(termcodes[idx].name[1])
						    || ASCII_ISUPPER(termcodes[idx].name[1])))
			{
			    is_keypad = TRUE;
			    ఒకవేళ (keypad_సూచీ_దొరికింది < 0)
			    {
				keypad_సూచీ_దొరికింది = idx;
				keypad_slen_దొరికింది = slen;
			    }
			}
			ఒకవేళ (!is_keypad)
			{
			    కీలకం_name[0] = termcodes[idx].name[0];
			    కీలకం_name[1] = termcodes[idx].name[1];
			    ఆపు;
			}
		    }
		}
	    }
	    ఒకవేళ (idx == tc_len && keypad_సూచీ_దొరికింది >= 0)
	    {
		కీలకం_name[0] = termcodes[keypad_సూచీ_దొరికింది].name[0];
		కీలకం_name[1] = termcodes[keypad_సూచీ_దొరికింది].name[1];
		slen = keypad_slen_దొరికింది;
	    }
	    లేకపోతే ఒకవేళ (idx == tc_len && mouse_సూచీ_దొరికింది >= 0)
	    {
		కీలకం_name[0] = termcodes[mouse_సూచీ_దొరికింది].name[0];
		కీలకం_name[1] = termcodes[mouse_సూచీ_దొరికింది].name[1];
	    }
	}

	ఒకవేళ (కీలకం_name[0] == NUL
	    // Mouse codes of DEC and pterm start with <ESC>[.  When
	    // detecting the start of these mouse codes they might as well be
	    // another key code or terminal response.
#నిర్వచితమైతే FEAT_MOUSE_DEC
	    || కీలకం_name[0] == KS_DEC_MOUSE
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_MOUSE_PTERM
	    || కీలకం_name[0] == KS_PTERM_MOUSE
#అయితే_ముగింపు
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
	    ఒకవేళ (((tp[0] == ESC && len >= 3 && tp[1] == '[')
			|| (tp[0] == CSI && len >= 2))
		    && vim_strchr((char_u *)"0123456789>?ABCDEFHPQRS",
								*argp) != ఖాళీ)
	    {
		పూర్ణాంకం resp = handle_csi(tp, len, argp, స్థానభ్రంశం, buf,
					     bufsize, buflen, కీలకం_name, &slen);
		ఒకవేళ (resp != 0)
		{
#నిర్వచితమైతే DEBUG_TERMRESPONSE
		    ఒకవేళ (resp == -1)
			LOG_TR1("Not enough characters for CSI sequence");
#అయితే_ముగింపు
		    తిరిగి resp;
		}
	    }

	    // Check for OSC responses from terminal
	    లేకపోతే ఒకవేళ ((tp[0] == ESC && len >= 2 && tp[1] == ']') || tp[0] == OSC)
	    {
handle_osc:
		ఒకవేళ (handle_osc(tp, len, కీలకం_name, &slen) == FAIL)
		    తిరిగి -1;
	    }

	    // Check for key code response from xterm,
	    // starting with <Esc>P or DCS
	    // It would only be needed with this condition:
	    //	    (check_for_codes || rcs_status.tr_progress == STATUS_SENT)
	    // Now this is always done so that DCS codes don't mess up things.
	    లేకపోతే ఒకవేళ ((tp[0] == ESC && len >= 2 && tp[1] == 'P') || tp[0] == DCS)
	    {
		ఒకవేళ (handle_dcs(tp, argp, len, కీలకం_name, &slen) == FAIL)
		    తిరిగి -1;
	    }
	}

	ఒకవేళ (కీలకం_name[0] == NUL)
	    కొనసాగు;	    // No match at this position, try next one

	// We only get here when we have a complete termcode match

#ఒకవేళ defined(FEAT_GUI) || defined(MSWIN)
	/*
	 * For scroll events from the GUI or MS-Windows console, fetch the
	 * pointer coordinates so that we know which window to scroll later.
	 */
	ఒకవేళ (TRUE
# ఒకవేళ defined(FEAT_GUI) && !defined(MSWIN)
		&& gui.లోపల_use
# endif
		&& కీలకం_name[0] == (పూర్ణాంకం)KS_EXTRA
		&& (కీలకం_name[1] == (పూర్ణాంకం)KE_X1MOUSE
		    || కీలకం_name[1] == (పూర్ణాంకం)KE_X2MOUSE
		    || కీలకం_name[1] == (పూర్ణాంకం)KE_MOUSEMOVE_XY
		    || కీలకం_name[1] == (పూర్ణాంకం)KE_MOUSELEFT
		    || కీలకం_name[1] == (పూర్ణాంకం)KE_MOUSERIGHT
		    || కీలకం_name[1] == (పూర్ణాంకం)KE_MOUSEDOWN
		    || కీలకం_name[1] == (పూర్ణాంకం)KE_MOUSEUP))
	{
	    char_u	బైట్లు[6];
	    పూర్ణాంకం		అంకె_బైట్లు = get_బైట్లు_from_buf(tp + slen, బైట్లు, 4);

	    ఒకవేళ (అంకె_బైట్లు == -1)	// not enough coordinates
		తిరిగి -1;
	    mouse_నిలువు = 128 * (బైట్లు[0] - ' ' - 1) + బైట్లు[1] - ' ' - 1;
	    mouse_అడ్డువరుస = 128 * (బైట్లు[2] - ' ' - 1) + బైట్లు[3] - ' ' - 1;
	    slen += అంకె_బైట్లు;
	    // equal to K_MOUSEMOVE
	    ఒకవేళ (కీలకం_name[1] == (పూర్ణాంకం)KE_MOUSEMOVE_XY)
		కీలకం_name[1] = (పూర్ణాంకం)KE_MOUSEMOVE;
	}
	లేకపోతే
#అయితే_ముగింపు
	/*
	 * If it is a mouse click, get the coordinates.
	 */
	ఒకవేళ (కీలకం_name[0] == KS_MOUSE
#నిర్వచితమైతే FEAT_MOUSE_GPM
		|| కీలకం_name[0] == KS_GPM_MOUSE
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_MOUSE_JSB
		|| కీలకం_name[0] == KS_JSBTERM_MOUSE
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_MOUSE_NET
		|| కీలకం_name[0] == KS_NETTERM_MOUSE
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_MOUSE_DEC
		|| కీలకం_name[0] == KS_DEC_MOUSE
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_MOUSE_PTERM
		|| కీలకం_name[0] == KS_PTERM_MOUSE
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_MOUSE_URXVT
		|| కీలకం_name[0] == KS_URXVT_MOUSE
#అయితే_ముగింపు
		|| కీలకం_name[0] == KS_SGR_MOUSE
		|| కీలకం_name[0] == KS_SGR_MOUSE_RELEASE)
	{
	    ఒకవేళ (check_termcode_mouse(tp, &slen, కీలకం_name, modifiers_ప్రారంభం, idx,
							     &modifiers) == -1)
		తిరిగి -1;
	}

#నిర్వచితమైతే FEAT_GUI
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
	లేకపోతే ఒకవేళ (కీలకం_name[0] == (పూర్ణాంకం)KS_MENU)
	{
	    long_u	val;
	    పూర్ణాంకం		అంకె_బైట్లు = get_long_from_buf(tp + slen, &val);

	    ఒకవేళ (అంకె_బైట్లు == -1)
		తిరిగి -1;
	    current_menu = (vimmenu_T *)val;
	    slen += అంకె_బైట్లు;

	    // The menu may have been deleted right after it was used, check
	    // for that.
	    ఒకవేళ (check_menu_సూచిక(మూలం_menu, current_menu) == FAIL)
	    {
		కీలకం_name[0] = KS_EXTRA;
		కీలకం_name[1] = (పూర్ణాంకం)KE_IGNORE;
	    }
	}
# endif
# ifdef FEAT_GUI_TABLINE
	లేకపోతే ఒకవేళ (కీలకం_name[0] == (పూర్ణాంకం)KS_TABLINE)
	{
	    // Selecting tabline tab or using its menu.
	    char_u	బైట్లు[6];
	    పూర్ణాంకం		అంకె_బైట్లు = get_బైట్లు_from_buf(tp + slen, బైట్లు, 1);

	    ఒకవేళ (అంకె_బైట్లు == -1)
		తిరిగి -1;
	    current_tab = (పూర్ణాంకం)బైట్లు[0];
	    ఒకవేళ (current_tab == 255)	// -1 in a byte gives 255
		current_tab = -1;
	    slen += అంకె_బైట్లు;
	}
	లేకపోతే ఒకవేళ (కీలకం_name[0] == (పూర్ణాంకం)KS_TABMENU)
	{
	    // Selecting tabline tab or using its menu.
	    char_u	బైట్లు[6];
	    పూర్ణాంకం		అంకె_బైట్లు = get_బైట్లు_from_buf(tp + slen, బైట్లు, 2);

	    ఒకవేళ (అంకె_బైట్లు == -1)
		తిరిగి -1;
	    current_tab = (పూర్ణాంకం)బైట్లు[0];
	    current_tabmenu = (పూర్ణాంకం)బైట్లు[1];
	    slen += అంకె_బైట్లు;
	}
# endif
# ifndef USE_ON_FLY_SCROLL
	లేకపోతే ఒకవేళ (కీలకం_name[0] == (పూర్ణాంకం)KS_VER_SCROLLBAR)
	{
	    long_u	val;
	    char_u	బైట్లు[6];
	    పూర్ణాంకం		అంకె_బైట్లు;

	    // Get the last scrollbar event in the queue of the same type
	    j = 0;
	    కొరకు (i = 0; tp[j] == CSI && tp[j + 1] == KS_VER_SCROLLBAR
						     && tp[j + 2] != NUL; ++i)
	    {
		j += 3;
		అంకె_బైట్లు = get_బైట్లు_from_buf(tp + j, బైట్లు, 1);
		ఒకవేళ (అంకె_బైట్లు == -1)
		    ఆపు;
		ఒకవేళ (i == 0)
		    current_scrollbar = (పూర్ణాంకం)బైట్లు[0];
		లేకపోతే ఒకవేళ (current_scrollbar != (పూర్ణాంకం)బైట్లు[0])
		    ఆపు;
		j += అంకె_బైట్లు;
		అంకె_బైట్లు = get_long_from_buf(tp + j, &val);
		ఒకవేళ (అంకె_బైట్లు == -1)
		    ఆపు;
		scrollbar_విలువ = val;
		j += అంకె_బైట్లు;
		slen = j;
	    }
	    ఒకవేళ (i == 0)		// not enough characters to make one
		తిరిగి -1;
	}
	లేకపోతే ఒకవేళ (కీలకం_name[0] == (పూర్ణాంకం)KS_HOR_SCROLLBAR)
	{
	    long_u	val;
	    పూర్ణాంకం		అంకె_బైట్లు;

	    // Get the last horiz. scrollbar event in the queue
	    j = 0;
	    కొరకు (i = 0; tp[j] == CSI && tp[j + 1] == KS_HOR_SCROLLBAR
						     && tp[j + 2] != NUL; ++i)
	    {
		j += 3;
		అంకె_బైట్లు = get_long_from_buf(tp + j, &val);
		ఒకవేళ (అంకె_బైట్లు == -1)
		    ఆపు;
		scrollbar_విలువ = val;
		j += అంకె_బైట్లు;
		slen = j;
	    }
	    ఒకవేళ (i == 0)		// not enough characters to make one
		తిరిగి -1;
	}
# endif // !USE_ON_FLY_SCROLL
#అయితే_ముగింపు // FEAT_GUI

#ఒకవేళ defined(UNIX) || defined(VMS)
	/*
	 * Handle FocusIn/FocusOut event sequences reported by XTerm.
	 * (CSI I/CSI O)
	 */
	ఒకవేళ (కీలకం_name[0] == KS_EXTRA
# ifdef FEAT_GUI
		&& !gui.లోపల_use
# endif
	    )
	{
	    ఒకవేళ (కీలకం_name[1] == KE_FOCUSGAINED)
	    {
		ఒకవేళ (!focus_state)
		{
		    ui_focus_change(TRUE);
		    did_cursorhold = TRUE;
		    focus_state = TRUE;
		}
		కీలకం_name[1] = (పూర్ణాంకం)KE_IGNORE;
	    }
	    లేకపోతే ఒకవేళ (కీలకం_name[1] == KE_FOCUSLOST)
	    {
		ఒకవేళ (focus_state)
		{
		    ui_focus_change(FALSE);
		    did_cursorhold = TRUE;
		    focus_state = FALSE;
		}
		కీలకం_name[1] = (పూర్ణాంకం)KE_IGNORE;
	    }
	}
#అయితే_ముగింపు

	/*
	 * Change <xHome> to <Home>, <xUp> to <Up>, etc.
	 */
	కీలకం = handle_x_కీలకాలు(TERMCAP2_కీలకం(కీలకం_name[0], కీలకం_name[1]));

	// Add any modifier codes to our string.
	new_slen = modifiers2keycode(modifiers, &కీలకం, string);

	// Finally, add the special key code to our string
	కీలకం_name[0] = KEY2TERMCAP0(కీలకం);
	కీలకం_name[1] = KEY2TERMCAP1(కీలకం);
	ఒకవేళ (కీలకం_name[0] == KS_కీలకం)
	{
	    // from ":set <M-b>=xx"
	    ఒకవేళ (has_mbyte)
		new_slen += (*mb_char2bytes)(కీలకం_name[1], string + new_slen);
	    లేకపోతే
		string[new_slen++] = కీలకం_name[1];
	}
	లేకపోతే ఒకవేళ (osc_state.processing ||
		(new_slen == 0
		 && కీలకం_name[0] == KS_EXTRA
		 && కీలకం_name[1] == KE_IGNORE))
	{
	    // Do not put K_IGNORE into the buffer, do return KEYLEN_REMOVED
	    // to indicate what happened.
	    retval = KEYLEN_REMOVED;
	}
	లేకపోతే
	{
	    string[new_slen++] = K_SPECIAL;
	    string[new_slen++] = కీలకం_name[0];
	    string[new_slen++] = కీలకం_name[1];
	}
	ఒకవేళ (put_string_లోపల_typebuf(స్థానభ్రంశం, slen, string, new_slen,
						 buf, bufsize, buflen) == FAIL)
	    తిరిగి -1;
	తిరిగి retval == 0 ? (len + new_slen - slen + స్థానభ్రంశం) : retval;
    }

#నిర్వచితమైతే FEAT_TERMRESPONSE
    LOG_TR1("normal character");
#అయితే_ముగింపు

    తిరిగి 0;			    // no match found
}

#ఒకవేళ defined(FEAT_TERMINAL) && defined(FEAT_TERMRESPONSE)
/*
 * Get the text foreground color, if known.
 */
    శూన్య
పదం_get_fg_color(char_u *r, char_u *g, char_u *b)
{
    ఒకవేళ (rfg_స్థితి.tr_పురోగతి != స్థితి_GOT)
	తిరిగి;

    *r = fg_r;
    *g = fg_g;
    *b = fg_b;
}
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_TERMRESPONSE
/*
 * Get the text background color, if known.  Returns OK when the OSC 11
 * response has been parsed and *r, *g, *b are populated; FAIL otherwise.
 */
    పూర్ణాంకం
పదం_get_bg_color(char_u *r, char_u *g, char_u *b)
{
    ఒకవేళ (rbg_స్థితి.tr_పురోగతి != స్థితి_GOT)
	తిరిగి FAIL;

    *r = bg_r;
    *g = bg_g;
    *b = bg_b;
    తిరిగి OK;
}
#అయితే_ముగింపు

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
    scid_T	sid_arg వాడని,	// script ID to use for <SID>,
				// or 0 to use current_sctx
    పూర్ణాంకం		flags,
    పూర్ణాంకం		*did_simplify)
{
    పూర్ణాంకం		i;
    పూర్ణాంకం		slen;
    పూర్ణాంకం		కీలకం;
    size_t	dlen = 0;
    char_u	*src;
    పూర్ణాంకం		do_backslash;	// backslash is a special character
    పూర్ణాంకం		do_special;	// recognize <> key codes
    పూర్ణాంకం		do_కీలకం_సంకేతం;	// recognize raw key codes
    char_u	*ఫలితం;	// buffer for resulting string
    garray_T	ga;

    do_backslash = (vim_strchr(p_cpo, CPO_BSLASH) == ఖాళీ);
    do_special = (vim_strchr(p_cpo, CPO_SPECI) == ఖాళీ)
						  || (flags & REPTERM_SPECIAL);
    do_కీలకం_సంకేతం = (vim_strchr(p_cpo, CPO_KEYCODE) == ఖాళీ);
    src = from;

    /*
     * Allocate space for the translation.  Worst case a single character is
     * replaced by 6 bytes (shifted special key), plus a NUL at the end.
     * In the rare case more might be needed ga_grow() must be called again.
     */
    ga_init2(&ga, 1L, 100);
    ఒకవేళ (ga_grow(&ga, (పూర్ణాంకం)(STRLEN(src) * 6 + 1)) == FAIL) // out of memory
    {
	*bufp = ఖాళీ;
	తిరిగి from;
    }
    ఫలితం = ga.ga_దత్తాంశం;

    /*
     * Check for #n at start only: function key n
     */
    ఒకవేళ ((flags & REPTERM_FROM_PART) && src[0] == '#' && VIM_ISDIGIT(src[1]))
    {
	ఫలితం[dlen++] = K_SPECIAL;
	ఫలితం[dlen++] = 'k';
	ఒకవేళ (src[1] == '0')
	    ఫలితం[dlen++] = ';';	// #0 is F10 is "k;"
	లేకపోతే
	    ఫలితం[dlen++] = src[1];	// #3 is F3 is "k3"
	src += 2;
    }

    /*
     * Copy each byte from *from to result[dlen]
     */
    అయితే (*src != NUL)
    {
	/*
	 * If 'cpoptions' does not contain '<', check for special key codes,
	 * like "<C-S-LeftMouse>"
	 */
	ఒకవేళ (do_special && ((flags & REPTERM_DO_LT)
					      || STRNCMP(src, "<lt>", 4) != 0))
	{
#నిర్వచితమైతే FEAT_EVAL
	    /*
	     * Change <SID>Func to K_SNR <script-nr> _Func.  This name is used
	     * for script-local user functions.
	     * (room: 5 * 6 = 30 bytes; needed: 3 + <nr> + 1 <= 14)
	     * Also change <SID>name.Func to K_SNR <import-script-nr> _Func.
	     * Only if "name" is recognized as an import.
	     */
	    ఒకవేళ (STRNICMP(src, "<SID>", 5) == 0)
	    {
		ఒకవేళ (sid_arg < 0 || (sid_arg == 0 && current_sctx.sc_sid <= 0))
		    emsg(_(e_using_sid_not_లోపల_script_context));
		లేకపోతే
		{
		    char_u  *dot;
		    దీర్ఘ    sid = sid_arg != 0 ? sid_arg : current_sctx.sc_sid;

		    src += 5;
		    ఒకవేళ (లోపల_vim9script()
				       && (dot = vim_strchr(src, '.')) != ఖాళీ)
		    {
			imported_T *imp = కనుగొను_imported(src, dot - src, FALSE);

			ఒకవేళ (imp != ఖాళీ)
			{
			    scriptitem_T    *si = SCRIPT_అంశం(imp->imp_sid);
			    size_t	    len;

			    src = dot + 1;
			    ఒకవేళ (si->sn_autoload_ఉపసర్గ != ఖాళీ)
			    {
				// Turn "<SID>name.Func"
				// into "scriptname#Func".
				len = STRLEN(si->sn_autoload_ఉపసర్గ);
				ఒకవేళ (ga_grow(&ga,
				     (పూర్ణాంకం)(STRLEN(src) * 6 + len + 1)) == FAIL)
				{
				    ga_clear(&ga);
				    *bufp = ఖాళీ;
				    తిరిగి from;
				}
				ఫలితం = ga.ga_దత్తాంశం;
				STRCPY(ఫలితం + dlen, si->sn_autoload_ఉపసర్గ);
				dlen += len;
				కొనసాగు;
			    }
			    sid = imp->imp_sid;
			}
		    }

		    ఫలితం[dlen++] = K_SPECIAL;
		    ఫలితం[dlen++] = (పూర్ణాంకం)KS_EXTRA;
		    ఫలితం[dlen++] = (పూర్ణాంకం)KE_SNR;
		    sprintf((అక్షరం *)ఫలితం + dlen, "%ld", sid);
		    dlen += STRLEN(ఫలితం + dlen);
		    ఫలితం[dlen++] = '_';
		    కొనసాగు;
		}
	    }
#అయితే_ముగింపు
	    పూర్ణాంకం fsk_flags = FSK_KEYCODE
			| ((flags & REPTERM_NO_SIMPLIFY) ? 0 : FSK_SIMPLIFY)
			| ((flags & REPTERM_FROM_PART) ? FSK_FROM_PART : 0);
	    slen = trans_special(&src, ఫలితం + dlen, fsk_flags,
							   TRUE, did_simplify);
	    ఒకవేళ (slen > 0)
	    {
		dlen += slen;
		కొనసాగు;
	    }
	}

	/*
	 * If 'cpoptions' does not contain 'k', see if it's an actual key-code.
	 * Note that this is also checked after replacing the <> form.
	 * Single character codes are NOT replaced (e.g. ^H or DEL), because
	 * it could be a character in the file.
	 */
	ఒకవేళ (do_కీలకం_సంకేతం)
	{
	    పూర్ణాంకం len;
	    i = కనుగొను_పదం_bykeys(src, &len);
	    ఒకవేళ (i >= 0)
	    {
		ఫలితం[dlen++] = K_SPECIAL;
		ఫలితం[dlen++] = termcodes[i].name[0];
		ఫలితం[dlen++] = termcodes[i].name[1];
		src += len;
		// If terminal code matched, continue after it.
		కొనసాగు;
	    }
	}

#నిర్వచితమైతే FEAT_EVAL
	ఒకవేళ (do_special)
	{
	    char_u	*p, *s, len;

	    /*
	     * Replace <Leader> by the value of "mapleader".
	     * Replace <LocalLeader> by the value of "maplocalleader".
	     * If "mapleader" or "maplocalleader" isn't set use a backslash.
	     */
	    ఒకవేళ (STRNICMP(src, "<Leader>", 8) == 0)
	    {
		len = 8;
		p = get_var_విలువ((char_u *)"g:mapleader");
	    }
	    లేకపోతే ఒకవేళ (STRNICMP(src, "<LocalLeader>", 13) == 0)
	    {
		len = 13;
		p = get_var_విలువ((char_u *)"g:maplocalleader");
	    }
	    లేకపోతే
	    {
		len = 0;
		p = ఖాళీ;
	    }
	    ఒకవేళ (len != 0)
	    {
		// Allow up to 8 * 6 characters for "mapleader".
		ఒకవేళ (p == ఖాళీ || *p == NUL || STRLEN(p) > 8 * 6)
		    s = (char_u *)"\\";
		లేకపోతే
		    s = p;
		అయితే (*s != NUL)
		    ఫలితం[dlen++] = *s++;
		src += len;
		కొనసాగు;
	    }
	}
#అయితే_ముగింపు

	/*
	 * Remove CTRL-V and ignore the next character.
	 * For "from" side the CTRL-V at the end is included, for the "to"
	 * part it is removed.
	 * If 'cpoptions' does not contain 'B', also accept a backslash.
	 */
	కీలకం = *src;
	ఒకవేళ (కీలకం == Ctrl_V || (do_backslash && కీలకం == '\\'))
	{
	    ++src;				// skip CTRL-V or backslash
	    ఒకవేళ (*src == NUL)
	    {
		ఒకవేళ (flags & REPTERM_FROM_PART)
		    ఫలితం[dlen++] = కీలకం;
		ఆపు;
	    }
	}

	// skip multibyte char correctly
	కొరకు (i = (*mb_ptr2len)(src); i > 0; --i)
	{
	    /*
	     * If the character is K_SPECIAL, replace it with K_SPECIAL
	     * KS_SPECIAL KE_FILLER.
	     * If compiled with the GUI replace CSI with K_CSI.
	     */
	    ఒకవేళ (*src == K_SPECIAL)
	    {
		ఫలితం[dlen++] = K_SPECIAL;
		ఫలితం[dlen++] = KS_SPECIAL;
		ఫలితం[dlen++] = KE_FILLER;
	    }
#నిర్వచితమైతే FEAT_GUI
	    లేకపోతే ఒకవేళ (*src == CSI)
	    {
		ఫలితం[dlen++] = K_SPECIAL;
		ఫలితం[dlen++] = KS_EXTRA;
		ఫలితం[dlen++] = (పూర్ణాంకం)KE_CSI;
	    }
#అయితే_ముగింపు
	    లేకపోతే
		ఫలితం[dlen++] = *src;
	    ++src;
	}
    }
    ఫలితం[dlen] = NUL;

    /*
     * Copy the new string to allocated memory.
     * If this fails, just return from.
     */
    ఒకవేళ ((*bufp = vim_strsave(ఫలితం)) != ఖాళీ)
	from = *bufp;
    vim_free(ఫలితం);
    తిరిగి from;
}

/*
 * Find a termcode with keys 'src' (must be NUL terminated).
 * Return the index in termcodes[], or -1 if not found.
 */
    స్థిర పూర్ణాంకం
కనుగొను_పదం_bykeys(char_u *src, పూర్ణాంకం *matchlen)
{
    పూర్ణాంకం		i, j;
    పూర్ణాంకం		len = (పూర్ణాంకం)STRLEN(src);
    పూర్ణాంకం         దొరికింది = -1;
    // Don't return a match for a single character
    పూర్ణాంకం         foundlen = 1;
    పూర్ణాంకం         slen, modslen;
    పూర్ణాంకం         thislen;

    // Most input bytes cannot start a terminal code.  Reuse the same leader
    // table as check_termcode() to avoid scanning termcodes[] unnecessarily.
    ఒకవేళ (need_gather)
	gather_termleader();
    ఒకవేళ (*src == NUL || vim_strchr(termleader, *src) == ఖాళీ)
	తిరిగి -1;

    // find longest match
    // borrows part of check_termcode
    కొరకు (i = 0; i < tc_len; ++i)
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
	ఒకవేళ (modslen > 0)
	{
	    ఒకవేళ (len > modslen
			&& STRNCMP(termcodes[i].సంకేతం_2, src, (size_t)modslen) == 0)
	    {
		thislen = 0;

		ఒకవేళ (src[modslen] == termcodes[i].సంకేతం_2[slen - 1])
		    // no modifiers
		    thislen = modslen + 1;
		లేకపోతే ఒకవేళ (src[modslen] != ';' && modslen == slen - 3)
		    // no match for "code;*X" with "code;"
		    కొనసాగు;
		లేకపోతే ఒకవేళ (termcodes[i].సంకేతం_2[modslen] == '@'
				&& (src[modslen] != '1'
					    || src[modslen + 1] != ';'))
		    // no match for "<Esc>[@" with "<Esc>[1;"
		    కొనసాగు;
		లేకపోతే
		{
		    // Skip over the digits, the final char must
		    // follow. URXVT can use a negative value, thus
		    // also accept '-'.
		    కొరకు (j = slen - 2; j < len && (సురక్షిత_isdigit(src[j])
			       || src[j] == '-' || src[j] == ';'); ++j)
			;
		    ++j;
		    ఒకవేళ (len < j)	// got a partial sequence
			కొనసాగు;
		    ఒకవేళ (src[j - 1] != termcodes[i].సంకేతం_2[slen - 1])
			కొనసాగు;	// no match

		    thislen = j;
		}

		ఒకవేళ (thislen > foundlen)
		{
		    దొరికింది = i;
		    foundlen = thislen;
		}
	    }
	}
	లేకపోతే
	{
	    ఒకవేళ (slen > foundlen && len >= slen
			&& STRNCMP(termcodes[i].సంకేతం_2, src, (size_t)slen) == 0)
	    {
		దొరికింది = i;
		foundlen = slen;
	    }
	}
    }

    ఒకవేళ (matchlen != ఖాళీ && దొరికింది >= 0)
	*matchlen = foundlen;

    తిరిగి దొరికింది;
}

/*
 * Gather the first characters in the terminal key codes into a string.
 * Used to speed up check_termcode().
 */
    స్థిర శూన్య
gather_termleader(శూన్య)
{
    పూర్ణాంకం	    i;
    పూర్ణాంకం	    len = 0;

#నిర్వచితమైతే FEAT_GUI
    ఒకవేళ (gui.లోపల_use)
	termleader[len++] = CSI;    // the GUI codes are not in termcodes[]
#అయితే_ముగింపు

    // Querying cell size uses CSI 14 t
#ఒకవేళ defined(MSWIN) && (defined(FEAT_IMAGE) || defined(FEAT_EVAL))
# ifdef FEAT_GUI
    ఒకవేళ (!gui.లోపల_use)
# endif
    {
	termleader[len++] = ESC;
# ifdef FEAT_GUI
	ఒకవేళ (!gui.లోపల_use)
# endif
	    termleader[len++] = CSI;
    }
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_TERMRESPONSE
    ఒకవేళ (check_for_codes || *T_CRS != NUL)
	termleader[len++] = DCS;    // the termcode response starts with DCS
				    // in 8-bit mode
#అయితే_ముగింపు
    termleader[len] = NUL;

    కొరకు (i = 0; i < tc_len; ++i)
	ఒకవేళ (vim_strchr(termleader, termcodes[i].సంకేతం_2[0]) == ఖాళీ)
	{
	    termleader[len++] = termcodes[i].సంకేతం_2[0];
	    termleader[len] = NUL;
	}

    need_gather = FALSE;
}

/*
 * Show all termcodes (for ":set termcap")
 * This code looks a lot like showoptions(), but is different.
 * "flags" can have OPT_ONECOLUMN.
 */
    శూన్య
show_termcodes(పూర్ణాంకం flags)
{
    పూర్ణాంకం		నిలువు;
    పూర్ణాంకం		*వస్తువులు;
    పూర్ణాంకం		అంశం_లెక్క;
    పూర్ణాంకం		నిరంతరం;
    పూర్ణాంకం		అడ్డువరుస, అడ్డువరుసలు_2;
    పూర్ణాంకం		నిలువుల_సంఖ్య;
    పూర్ణాంకం		i;
    పూర్ణాంకం		len;

#నిర్వచించు INC3 27	    // try to make three columns
#నిర్వచించు INC2 40	    // try to make two columns
#నిర్వచించు GAP 2	    // spaces between columns

    ఒకవేళ (tc_len == 0)	    // no terminal codes (must be GUI)
	తిరిగి;
    వస్తువులు = ALLOC_MULT(పూర్ణాంకం, tc_len);
    ఒకవేళ (వస్తువులు == ఖాళీ)
	తిరిగి;

    // Highlight title
    సందేశం_puts_title(_("\n--- Terminal keys ---"));

    /*
     * Do the loop three times:
     * 1. display the short items (non-strings and short strings)
     * 2. display the medium items (medium length strings)
     * 3. display the long items (remaining strings)
     * When "flags" has OPT_ONECOLUMN do everything in 3.
     */
    కొరకు (నిరంతరం = (flags & OPT_ONECOLUMN) ? 3 : 1; నిరంతరం <= 3 && !got_int; ++నిరంతరం)
    {
	/*
	 * collect the items in items[]
	 */
	అంశం_లెక్క = 0;
	కొరకు (i = 0; i < tc_len; i++)
	{
	    len = show_one_termcode(termcodes[i].name,
						    termcodes[i].సంకేతం_2, FALSE);
	    ఒకవేళ ((flags & OPT_ONECOLUMN) ||
		    (len <= INC3 - GAP ? నిరంతరం == 1
			: len <= INC2 - GAP ? నిరంతరం == 2
			: నిరంతరం == 3))
		వస్తువులు[అంశం_లెక్క++] = i;
	}

	/*
	 * display the items
	 */
	ఒకవేళ (నిరంతరం <= 2)
	{
	    నిలువుల_సంఖ్య = (Columns + GAP) / (నిరంతరం == 1 ? INC3 : INC2);
	    ఒకవేళ (నిలువుల_సంఖ్య == 0)
		నిలువుల_సంఖ్య = 1;
	    అడ్డువరుసలు_2 = (అంశం_లెక్క + నిలువుల_సంఖ్య - 1) / నిలువుల_సంఖ్య;
	}
	లేకపోతే	// run == 3
	    అడ్డువరుసలు_2 = అంశం_లెక్క;
	కొరకు (అడ్డువరుస = 0; అడ్డువరుస < అడ్డువరుసలు_2 && !got_int; ++అడ్డువరుస)
	{
	    సందేశం_putchar('\n');			// go to next line
	    ఒకవేళ (got_int)			// 'q' typed in more
		ఆపు;
	    నిలువు = 0;
	    కొరకు (i = అడ్డువరుస; i < అంశం_లెక్క; i += అడ్డువరుసలు_2)
	    {
		సందేశం_నిలువు = నిలువు;			// make columns
		show_one_termcode(termcodes[వస్తువులు[i]].name,
					      termcodes[వస్తువులు[i]].సంకేతం_2, TRUE);
		ఒకవేళ (నిరంతరం == 2)
		    నిలువు += INC2;
		లేకపోతే
		    నిలువు += INC3;
	    }
	    బహిర్గతం_flush();
	    ui_breakcheck();
	}
    }
    vim_free(వస్తువులు);
}

/*
 * Show one termcode entry.
 * Output goes into IObuff[]
 */
    పూర్ణాంకం
show_one_termcode(char_u *name, char_u *సంకేతం_2, పూర్ణాంకం printit)
{
    char_u	*p;
    పూర్ణాంకం		len;

    ఒకవేళ (name[0] > '~')
    {
	IObuff[0] = ' ';
	IObuff[1] = ' ';
	IObuff[2] = ' ';
	IObuff[3] = ' ';
    }
    లేకపోతే
    {
	IObuff[0] = 't';
	IObuff[1] = '_';
	IObuff[2] = name[0];
	IObuff[3] = name[1];
    }
    IObuff[4] = ' ';

    p = get_special_కీలకం_name(TERMCAP2_కీలకం(name[0], name[1]), 0);
    ఒకవేళ (p[1] != 't')
	STRCPY(IObuff + 5, p);
    లేకపోతే
	IObuff[5] = NUL;
    len = (పూర్ణాంకం)STRLEN(IObuff);
    చేయి
	IObuff[len++] = ' ';
    అయితే (len < 17);
    IObuff[len] = NUL;
    ఒకవేళ (సంకేతం_2 == ఖాళీ)
	len += 4;
    లేకపోతే
	len += vim_strsize(సంకేతం_2);

    ఒకవేళ (printit)
    {
	సందేశం_puts((అక్షరం *)IObuff);
	ఒకవేళ (సంకేతం_2 == ఖాళీ)
	    సందేశం_puts("NULL");
	లేకపోతే
	    సందేశం_outtrans(సంకేతం_2);
    }
    తిరిగి len;
}

#ఒకవేళ defined(FEAT_TERMRESPONSE)
/*
 * For Xterm >= 140 compiled with OPT_TCAP_QUERY: Obtain the actually used
 * termcap codes from the terminal itself.
 * We get them one by one to avoid a very long response string.
 */
స్థిర పూర్ణాంకం xt_సూచీ_లోపల = 0;
స్థిర పూర్ణాంకం xt_సూచీ_బహిర్గతం = 0;

    స్థిర శూన్య
req_codes_from_పదం(శూన్య)
{
    xt_సూచీ_బహిర్గతం = 0;
    xt_సూచీ_లోపల = 0;
    req_మరిన్ని_codes_from_పదం();
}

    స్థిర శూన్య
req_మరిన్ని_codes_from_పదం(శూన్య)
{
    అక్షరం	buf[32];  // extra size to shut up LGTM
    పూర్ణాంకం		old_idx = xt_సూచీ_బహిర్గతం;

    // Don't do anything when going to exit.
    ఒకవేళ (exiting)
	తిరిగి;

    // Send up to 10 more requests out than we received.  Avoid sending too
    // many, there can be a buffer overflow somewhere.
    అయితే (xt_సూచీ_బహిర్గతం < xt_సూచీ_లోపల + 10 && కీలకం_పేర్లు[xt_సూచీ_బహిర్గతం] != ఖాళీ)
    {
	అక్షరం *కీలకం_name = కీలకం_పేర్లు[xt_సూచీ_బహిర్గతం];

	MAY_WANT_గమ్యం_LOG_THIS;
	LOG_TRN("Requesting XT %d: %s", xt_సూచీ_బహిర్గతం, కీలకం_name);
	ఒకవేళ (కీలకం_name[2] != NUL)
	    sprintf(buf, "\033P+q%02x%02x%02x\033\\", కీలకం_name[0], కీలకం_name[1], కీలకం_name[2]);
	లేకపోతే
	    sprintf(buf, "\033P+q%02x%02x\033\\", కీలకం_name[0], కీలకం_name[1]);
	బహిర్గతం_str_nf((char_u *)buf);
	++xt_సూచీ_బహిర్గతం;
    }

    // Send the codes out right away.
    ఒకవేళ (xt_సూచీ_బహిర్గతం != old_idx)
	బహిర్గతం_flush();
}

/*
 * Decode key code response from xterm:
 * '<Esc>P1+r<name>=<string><Esc>\' if it is enabled/supported
 * '<Esc>P0+r<Esc>\'                if it not enabled
 * A "0" instead of the "1" indicates a code that isn't supported.
 * Both <name> and <string> are encoded in hex.
 * "code" points to the "0" or "1".
 */
    స్థిర శూన్య
got_సంకేతం_from_పదం(char_u *సంకేతం_2, పూర్ణాంకం len)
{
# define XT_LEN 100
    char_u	name[4];
    char_u	str[XT_LEN];
    పూర్ణాంకం		i;
    పూర్ణాంకం		j = 0;
    పూర్ణాంకం		c;

    // A '1' means the code is supported, a '0' means it isn't.
    // If it is supported, there must be a '=' following
    // When half the length is > XT_LEN we can't use it.
    ఒకవేళ (సంకేతం_2[0] == '1' && (సంకేతం_2[7] == '=' || సంకేతం_2[9] == '=') && len / 2 < XT_LEN)
    {
	// Get the name from the response and find it in the table.
	name[0] = hexhex2nr(సంకేతం_2 + 3);
	name[1] = hexhex2nr(సంకేతం_2 + 5);
	ఒకవేళ (సంకేతం_2[9] == '=')
	    name[2] = hexhex2nr(సంకేతం_2 + 7);
	లేకపోతే
	    name[2] = NUL;
	name[3] = NUL;
	కొరకు (i = 0; కీలకం_పేర్లు[i] != ఖాళీ; ++i)
	{
	    ఒకవేళ (STRCMP(కీలకం_పేర్లు[i], name) == 0)
	    {
		xt_సూచీ_లోపల = i;
		ఆపు;
	    }
	}

	LOG_TRN("Received XT %d: %s", xt_సూచీ_లోపల, (అక్షరం *)name);

	ఒకవేళ (కీలకం_పేర్లు[i] != ఖాళీ)
	{
	    i = (సంకేతం_2[7] == '=') ? 8 : 10;
	    కొరకు (; (c = hexhex2nr(సంకేతం_2 + i)) >= 0; i += 2)
		str[j++] = c;
	    str[j] = NUL;
	    ఒకవేళ (name[0] == 'C' && name[1] == 'o')
	    {
		// Color count is not a key code.
		పూర్ణాంకం val = atoi((అక్షరం *)str);
# ఒకవేళ defined(FEAT_EVAL)
		ఒకవేళ (val == t_colors)
		    ch_log(ఖాళీ, "got_code_from_term(Co): no change (%d)", val);
		లేకపోతే
		    ch_log(ఖాళీ,
			       "got_code_from_term(Co): changed from %d to %d",
								t_colors, val);
# endif
		may_adjust_color_లెక్క(val);
	    }
# ఒకవేళ 0
#  ifdef FEAT_TERMGUICOLORS
	    // when RGB result comes back, it is supported when the result contains an '='
	    లేకపోతే ఒకవేళ (name[0] == 'R' && name[1] == 'G' && name[2] == 'B' && సంకేతం_2[9] == '=')
	    {
		పూర్ణాంకం val = atoi((అక్షరం *)str);
		// only enable it, if termguicolors hasn't been set yet and
		// there are 8 bits per color channel
		ఒకవేళ (val == 8 && !p_tgc_సమితి)
		{
#   ifdef FEAT_EVAL
		    ch_log(ఖాళీ, "got_code_from_term(RGB): xterm-direct colors detected");
#   endif
		    // RGB capability set, enable termguicolors
		    సమితి_option_విలువ((char_u *)"termguicolors", 1L, ఖాళీ, 0);
		}
	    }
#  endif
# endif
	    లేకపోతే
	    {
		i = కనుగొను_పదం_bykeys(str, ఖాళీ);
		ఒకవేళ (i >= 0 && name[0] == termcodes[i].name[0]
					    && name[1] == termcodes[i].name[1])
		{
		    // Existing entry with the same name and code - skip.
# ifdef FEAT_EVAL
		    ch_log(ఖాళీ, "got_code_from_term(): Entry %c%c did not change",
							     name[0], name[1]);
# endif
		}
		లేకపోతే ఒకవేళ (i >= 0 && name[0] == 'K' && VIM_ISDIGIT(name[1]))
		{
		    // Would replace existing entry with keypad key - skip.
# ifdef FEAT_EVAL
		    ch_log(ఖాళీ, "got_code_from_term(): Skipping entry %c%c in favor of %c%c with matching keys %s",
			    name[0], name[1], termcodes[i].name[0], termcodes[i].name[1], str);
# endif
		}
		లేకపోతే
		{
		    ఒకవేళ (i >= 0)
		    {
			// Delete an existing entry using the same code.
# ifdef FEAT_EVAL
			ch_log(ఖాళీ, "got_code_from_term(): Deleting entry %c%c with matching keys %s",
			      termcodes[i].name[0], termcodes[i].name[1], str);
# endif
			del_termcode_idx(i);
		    }
# ifdef FEAT_EVAL
		    లేకపోతే
			ch_log(ఖాళీ, "got_code_from_term(): Adding entry %c%c with keys %s",
							name[0], name[1], str);
# endif
		    కూడు_termcode(name, str, ATC_FROM_పదం);
		}
	    }
	}
    }

    // May request more codes now that we received one.
    ++xt_సూచీ_లోపల;
    req_మరిన్ని_codes_from_పదం();
}

/*
 * Check if there are any unanswered requests and deal with them.
 * This is called before starting an external program or getting direct
 * keyboard input.  We don't want responses to be send to that program or
 * handled as typed text.
 */
    స్థిర శూన్య
check_for_codes_from_పదం(శూన్య)
{
    పూర్ణాంకం		c;

    // If no codes requested or all are answered, no need to wait.
    ఒకవేళ (xt_సూచీ_బహిర్గతం == 0 || xt_సూచీ_బహిర్గతం == xt_సూచీ_లోపల)
	తిరిగి;

    // Vgetc() will check for and handle any response.
    // Keep calling vpeekc() until we don't get any responses.
    ++no_mapping;
    ++allow_కీలకాలు;
    కొరకు (;;)
    {
	c = vpeekc();
	ఒకవేళ (c == NUL)	    // nothing available
	    ఆపు;

	// If a response is recognized it's replaced with K_IGNORE, must read
	// it from the input stream.  If there is no K_IGNORE we can't do
	// anything, break here (there might be some responses further on, but
	// we don't want to throw away any typed chars).
	ఒకవేళ (c != K_SPECIAL && c != K_IGNORE)
	    ఆపు;
	c = vgetc();
	ఒకవేళ (c != K_IGNORE)
	{
	    vungetc(c);
	    ఆపు;
	}
    }
    --no_mapping;
    --allow_కీలకాలు;
}
#అయితే_ముగింపు

#ఒకవేళ defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
స్థిర అక్షరం ksme_str[20];
స్థిర అక్షరం ksmr_str[20];
స్థిర అక్షరం ksmd_str[20];

/*
 * For Win32 console: update termcap codes for existing console attributes.
 */
    శూన్య
update_tcap(పూర్ణాంకం attr)
{
    sprintf(ksme_str, "\033|%dm", attr);
    sprintf(ksmd_str, "\033|%dm", attr | 0x08);  // FOREGROUND_INTENSITY
    sprintf(ksmr_str, "\033|%dm", ((attr & 0x0F) << 4) | ((attr & 0xF0) >> 4));

    tcap_నమోదు_T *p = కనుగొను_builtin_పదం(DEFAULT_పదం);
    ఒకవేళ (p == ఖాళీ)  // did not find it
	తిరిగి;
    అయితే (p->bt_string != ఖాళీ)
    {
	ఒకవేళ (p->bt_నమోదు == (పూర్ణాంకం)KS_ME)
	    p->bt_string = &ksme_str[0];
	లేకపోతే ఒకవేళ (p->bt_నమోదు == (పూర్ణాంకం)KS_MR)
	    p->bt_string = &ksmr_str[0];
	లేకపోతే ఒకవేళ (p->bt_నమోదు == (పూర్ణాంకం)KS_MD)
	    p->bt_string = &ksmd_str[0];
	++p;
    }
}

# ifdef FEAT_TERMGUICOLORS
#  define KSSIZE 20

రకనిర్వచనం గణన
{
    CMODE_సూచీకృత = 0,	// Use cmd.exe 4bit palette.
    CMODE_RGB,		// Use 24bit RGB colors using VTP.
    CMODE_256_నిలువు,	// Emulate xterm's 256-color palette using VTP.
    CMODE_చివరి,
} cmode_T;

నిర్మాణం ks_tbl_S
{
    పూర్ణాంకం  సంకేతం_2;				// value of KS_
    అక్షరం *vtp;				// code in RGB mode
    అక్షరం *vtp2;				// code in 256color mode
    అక్షరం buf[CMODE_చివరి][KSSIZE];	// real buffer
};

స్థిర నిర్మాణం ks_tbl_S ks_tbl[] =
{
    {(పూర్ణాంకం)KS_ME,  "\033|0m",  "\033|0m", {""}},   //సాధారణ
    {(పూర్ణాంకం)KS_MR,  "\033|7m",  "\033|7m", {""}},   //తిరగేయి
    {(పూర్ణాంకం)KS_MD,  "\033|1m",  "\033|1m", {""}},   // bold
    {(పూర్ణాంకం)KS_SO,  "\033|91m", "\033|91m", {""}},  // standout: bright red text
    {(పూర్ణాంకం)KS_SE,  "\033|39m", "\033|39m", {""}},  // standout end: default color
    {(పూర్ణాంకం)KS_CZH, "\033|3m",  "\033|3m", {""}},   // italic
    {(పూర్ణాంకం)KS_CZR, "\033|0m",  "\033|0m", {""}},   // italic end
    {(పూర్ణాంకం)KS_US,  "\033|4m",  "\033|4m", {""}},   // underscore
    {(పూర్ణాంకం)KS_UE,  "\033|24m", "\033|24m", {""}},  // underscore end
#  ifdef TERMINFO
    {(పూర్ణాంకం)KS_CAB, "\033|%p1%db", "\033|%p14%dm", {""}}, // set background color
    {(పూర్ణాంకం)KS_CAF, "\033|%p1%df", "\033|%p13%dm", {""}}, // set foreground color
    {(పూర్ణాంకం)KS_CS,  "\033|%p1%d;%p2%dR", "\033|%p1%d;%p2%dR", {""}},
    {(పూర్ణాంకం)KS_CSV, "\033|%p1%d;%p2%dV", "\033|%p1%d;%p2%dV", {""}},
#  లేకపోతే
    {(పూర్ణాంకం)KS_CAB, "\033|%db", "\033|4%dm", {""}}, // set background color
    {(పూర్ణాంకం)KS_CAF, "\033|%df", "\033|3%dm", {""}}, // set foreground color
    {(పూర్ణాంకం)KS_CS,  "\033|%d;%dR", "\033|%d;%dR", {""}},
    {(పూర్ణాంకం)KS_CSV, "\033|%d;%dV", "\033|%d;%dV", {""}},
#  endif
    {(పూర్ణాంకం)KS_CCO, "256", "256", {""}},	    // colors
    {(పూర్ణాంకం)KS_NAME, ఖాళీ, ఖాళీ, {""}}			    // terminator
};

/*
 * Find the first entry for "code" in the builtin termcap for "name".
 * Returns NULL when not found.
 */
    స్థిర tcap_నమోదు_T *
కనుగొను_మొదటి_tcap(
    char_u *name,
    పూర్ణాంకం	    సంకేతం_2)
{
    tcap_నమోదు_T *p = కనుగొను_builtin_పదం(name);
    ఒకవేళ (p == ఖాళీ)
	తిరిగి ఖాళీ;
    అయితే (p->bt_string != ఖాళీ)
    {
	ఒకవేళ (p->bt_నమోదు == సంకేతం_2)
	    తిరిగి p;
	++p;
    }
    తిరిగి ఖాళీ;
}
# endif

/*
 * For Win32 console: replace the sequence immediately after termguicolors.
 */
    శూన్య
మార్పిడి_tcap(శూన్య)
{
# ifdef FEAT_TERMGUICOLORS
    స్థిర పూర్ణాంకం		init_పూర్తి_2 = FALSE;
    స్థిర cmode_T	curr_బహుళకం;
    నిర్మాణం ks_tbl_S	*ks;
    cmode_T		బహుళకం;

    ఒకవేళ (!init_పూర్తి_2)
    {
	కొరకు (ks = ks_tbl; ks->సంకేతం_2 != (పూర్ణాంకం)KS_NAME; ks++)
	{
	    tcap_నమోదు_T *bt = కనుగొను_మొదటి_tcap(DEFAULT_పదం, ks->సంకేతం_2);
	    ఒకవేళ (bt != ఖాళీ)
	    {
		// Preserve the original value.
		STRNCPY(ks->buf[CMODE_సూచీకృత], bt->bt_string, KSSIZE);
		STRNCPY(ks->buf[CMODE_RGB], ks->vtp, KSSIZE);
		STRNCPY(ks->buf[CMODE_256_నిలువు], ks->vtp2, KSSIZE);

		bt->bt_string = ks->buf[CMODE_సూచీకృత];
	    }
	}
	init_పూర్తి_2 = TRUE;
	curr_బహుళకం = CMODE_సూచీకృత;
    }

    ఒకవేళ (p_tgc)
	బహుళకం = CMODE_RGB;
    లేకపోతే ఒకవేళ (t_colors >= 256)
	బహుళకం = CMODE_256_నిలువు;
    లేకపోతే
	బహుళకం = CMODE_సూచీకృత;

    ఒకవేళ (బహుళకం == curr_బహుళకం)
	తిరిగి;

    కొరకు (ks = ks_tbl; ks->సంకేతం_2 != (పూర్ణాంకం)KS_NAME; ks++)
    {
	tcap_నమోదు_T *bt = కనుగొను_మొదటి_tcap(DEFAULT_పదం, ks->సంకేతం_2);
	ఒకవేళ (bt != ఖాళీ)
	    bt->bt_string = ks->buf[బహుళకం];
    }

    curr_బహుళకం = బహుళకం;
# endif
}

#అయితే_ముగింపు


#ఒకవేళ (defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))) || defined(FEAT_TERMINAL)
స్థిర స్థిరరాశి పూర్ణాంకం cube_విలువ[] = {
    0x00, 0x5F, 0x87, 0xAF, 0xD7, 0xFF
};

స్థిర స్థిరరాశి పూర్ణాంకం grey_ramp[] = {
    0x08, 0x12, 0x1C, 0x26, 0x30, 0x3A, 0x44, 0x4E, 0x58, 0x62, 0x6C, 0x76,
    0x80, 0x8A, 0x94, 0x9E, 0xA8, 0xB2, 0xBC, 0xC6, 0xD0, 0xDA, 0xE4, 0xEE
};

స్థిర స్థిరరాశి char_u ansi_పట్టిక[16][3] = {
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

# ఒకవేళ defined(MSWIN)
// Mapping between cterm indices < 16 and their counterpart in the ANSI palette.
స్థిరరాశి char_u cterm_ansi_idx[] = {
    0, 4, 2, 6, 1, 5, 3, 7, 8, 12, 10, 14, 9, 13, 11, 15
};
# endif

# define ANSI_సూచీ_NONE 0

    శూన్య
ansi_color2rgb(పూర్ణాంకం nr, char_u *r, char_u *g, char_u *b, char_u *ansi_idx)
{
    ఒకవేళ (nr < 16)
    {
	*r = ansi_పట్టిక[nr][0];
	*g = ansi_పట్టిక[nr][1];
	*b = ansi_పట్టిక[nr][2];
	*ansi_idx = nr + 1;
    }
    లేకపోతే
    {
	*r = 0;
	*g = 0;
	*b = 0;
	*ansi_idx = ANSI_సూచీ_NONE;
    }
}

    శూన్య
cterm_color2rgb(పూర్ణాంకం nr, char_u *r, char_u *g, char_u *b, char_u *ansi_idx)
{
    పూర్ణాంకం idx;

    ఒకవేళ (nr < 16)
    {
# ఒకవేళ defined(MSWIN)
	idx = cterm_ansi_idx[nr];
# లేకపోతే
	idx = nr;
# endif
	*r = ansi_పట్టిక[idx][0];
	*g = ansi_పట్టిక[idx][1];
	*b = ansi_పట్టిక[idx][2];
	*ansi_idx = idx + 1;
    }
    లేకపోతే ఒకవేళ (nr < 232)
    {
	// 216 color cube
	idx = nr - 16;
	*r = cube_విలువ[idx / 36 % 6];
	*g = cube_విలువ[idx / 6  % 6];
	*b = cube_విలువ[idx      % 6];
	*ansi_idx = ANSI_సూచీ_NONE;
    }
    లేకపోతే ఒకవేళ (nr < 256)
    {
	// 24 grey scale ramp
	idx = nr - 232;
	*r = grey_ramp[idx];
	*g = grey_ramp[idx];
	*b = grey_ramp[idx];
	*ansi_idx = ANSI_సూచీ_NONE;
    }
    లేకపోతే
    {
	*r = 0;
	*g = 0;
	*b = 0;
	*ansi_idx = ANSI_సూచీ_NONE;
    }
}
#అయితే_ముగింపు

/*
 * Replace K_BS by <BS> and K_DEL by <DEL>.
 * Include any modifiers into the key and drop them.
 * Returns "len" adjusted for replaced codes.
 */
    పూర్ణాంకం
పదం_replace_keycodes(char_u *ta_buf, పూర్ణాంకం ta_len, పూర్ణాంకం len_arg)
{
    పూర్ణాంకం		len = len_arg;
    పూర్ణాంకం		i;
    పూర్ణాంకం		c;

    కొరకు (i = ta_len; i < ta_len + len; ++i)
    {
	ఒకవేళ (ta_buf[i] == CSI && len - i > 3 && ta_buf[i + 1] == KS_MODIFIER)
	{
	    పూర్ణాంకం modifiers = ta_buf[i + 2];
	    పూర్ణాంకం కీలకం = ta_buf[i + 3];

	    // Try to use the modifier to modify the key.  In any case drop the
	    // modifier.
	    mch_memmove(ta_buf + i + 1, ta_buf + i + 4, (size_t)(len - i - 3));
	    len -= 3;
	    ఒకవేళ (కీలకం < 0x80)
		కీలకం = విలీనం_modify_Other_కీలకాలు(కీలకం, &modifiers);
	    ta_buf[i] = కీలకం;
	}
	లేకపోతే ఒకవేళ (ta_buf[i] == CSI && len - i > 2)
	{
	    c = TERMCAP2_కీలకం(ta_buf[i + 1], ta_buf[i + 2]);
	    ఒకవేళ (c == K_DEL || c == K_KDEL || c == K_BS)
	    {
		mch_memmove(ta_buf + i + 1, ta_buf + i + 3,
							(size_t)(len - i - 2));
		ఒకవేళ (c == K_DEL || c == K_KDEL)
		    ta_buf[i] = DEL;
		లేకపోతే
		    ta_buf[i] = Ctrl_H;
		len -= 2;
	    }
	}
	లేకపోతే ఒకవేళ (ta_buf[i] == '\r')
	    ta_buf[i] = '\n';
	ఒకవేళ (has_mbyte)
	    i += (*mb_ptr2len_len)(ta_buf + i, ta_len + len - i) - 1;
    }
    తిరిగి len;
}

/*
 * Should be called when cleaning up terminal state.
 */
    శూన్య
పదం_disable_dec(శూన్య)
{
    పదం_సమితి_sync_output(పదం_SYNC_OUTPUT_OFF);
#నిర్వచితమైతే UNIX
    పదం_సమితి_win_resize(false);
#అయితే_ముగింపు
    // Make sure to always flush the output buffer, because this may be called
    // before starting the GUI
    బహిర్గతం_flush();
}

#నిర్వచితమైతే UNIX
/*
 * Enable or disable receiving in-band window resize events from the terminal.
 * If "state" is true, then if the terminal supports DEC mode 2048 and
 * 'termresize' is "" or "inband", then enable it and disable the SIGWINCH
 * signal handling. Otherwise disable the mode if it is enabled and reinstall
 * the SIGWINCH handler.
 */
    శూన్య
పదం_సమితి_win_resize(bool state)
{
# ifdef FEAT_GUI
    bool    లోపల_gui = gui.లోపల_use;

    ఒకవేళ (state && లోపల_gui)
	తిరిగి;
# endif

    ఒకవేళ (!state || win_resize_setting == 0 || win_resize_setting == 4)
    {
	// Make sure it update internal window size if DEC mode 2048 is
	// unavailable now.
	ఒకవేళ (win_resize_enabled)
	{
	    సమితి_shellsize(0, 0, false);
	    సమితి_sigwinch_handler();
	    బహిర్గతం_str((char_u *)"\033[?2048l");
	}
	win_resize_enabled = false;
    }
    లేకపోతే ఒకవేళ ((*p_trz == NUL || STRCMP(p_trz, "inband") == 0)
	    && !win_resize_enabled)
    {
	ఒకవేళ (win_resize_setting == 2)
	    బహిర్గతం_str((char_u *)"\033[?2048h");
# ifdef SIGWINCH
	mch_signal(SIGWINCH, SIG_DFL);
# endif
	win_resize_enabled = true;
    }
}
#అయితే_ముగింపు

    పూర్ణాంకం
sync_output_చురుకైన(శూన్య)
{
#నిర్వచితమైతే FEAT_GUI
    ఒకవేళ (gui.లోపల_use)
	తిరిగి TRUE;
#అయితే_ముగింపు
    తిరిగి p_tsy && (sync_output_setting == 1 || sync_output_setting == 2)
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
    శూన్య
పదం_సమితి_sync_output(పూర్ణాంకం flags)
{
    bool    allowed;
#నిర్వచితమైతే FEAT_GUI
    bool    లోపల_gui = gui.లోపల_use;
#లేకపోతే
    bool    లోపల_gui = false;
#అయితే_ముగింపు

    allowed = p_tsy && (sync_output_setting == 1 || sync_output_setting == 2);

    ఒకవేళ (flags & పదం_SYNC_OUTPUT_FLUSH)
    {
	// Tell terminal to display screen contents, then resume batching.
	ఒకవేళ (allowed && !లోపల_gui && sync_output_state > 0 && *T_ESU != NUL &&
		*T_BSU != NUL)
	{
	    బహిర్గతం_str_nf(T_ESU);
	    బహిర్గతం_str_nf(T_BSU);
	    బహిర్గతం_flush();
	}
	తిరిగి;
    }

    // Forcibly turn off synchronized output (e.g. 'notermsync')
    ఒకవేళ (flags & పదం_SYNC_OUTPUT_OFF)
    {
	ఒకవేళ (sync_output_state > 0 && *T_ESU != NUL)
	{
	    బహిర్గతం_str_nf(T_ESU);
	    బహిర్గతం_flush();
	    sync_output_state = 0;
	}
	తిరిగి;
    }

    ఒకవేళ (!allowed || లోపల_gui || *T_BSU == NUL || *T_ESU == NUL)
	తిరిగి;

    // Only enable if we aren't already, and only disable if we have reached
    // zero.
    ఒకవేళ (flags & పదం_SYNC_OUTPUT_ENABLE)
    {
	ఒకవేళ (sync_output_state++ > 0)
	    తిరిగి;
	బహిర్గతం_str_nf(T_BSU);
    }
    లేకపోతే ఒకవేళ (flags & పదం_SYNC_OUTPUT_DISABLE)
    {
	ఒకవేళ (sync_output_state == 0 || --sync_output_state > 0)
	    తిరిగి;
	// Flush the output buffer before ending the sync batch so that
	// all drawing output is sent to the terminal within the
	// BSU..ESU window.  Without this, the drawing data remaining in
	// out_buf would be sent after ESU, outside the sync batch.
	బహిర్గతం_str_nf(T_ESU);
	బహిర్గతం_flush();
    }
    లేకపోతే
    {
	siemsg("Unknown sync output value %d", flags);
	తిరిగి;
    }
}
