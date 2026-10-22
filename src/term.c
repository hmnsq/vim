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

#અન્તર્ભાવ "vim.h"

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
# જો defined(VMS) || defined(AMIGA)
#  define TPUTSFUNCAST (શૂન્ય (*)(unsigned પૂર્ણાંક))
# નહીંતર
#  ifdef HAVE_OUTFUNTYPE
#   define TPUTSFUNCAST (outfuntype)
#  નહીંતર
#   define TPUTSFUNCAST (પૂર્ણાંક (*)(પૂર્ણાંક))
#  endif
# endif
#endif

#undef tgetstr

// start of keys that are not directly used by Vim but can be mapped
#define BT_EXTRA_ચાવીઓ	0x101

સ્થિર શૂન્ય parse_builtin_tcap(char_u *s);
સ્થિર શૂન્ય gather_termleader(શૂન્ય);
#ifdef FEAT_TERMRESPONSE
સ્થિર શૂન્ય req_codes_from_term(શૂન્ય);
સ્થિર શૂન્ય req_more_codes_from_term(શૂન્ય);
સ્થિર શૂન્ય got_code_from_term(char_u *code, પૂર્ણાંક len);
સ્થિર શૂન્ય check_for_codes_from_term(શૂન્ય);
#endif
સ્થિર શૂન્ય del_termcode_idx(પૂર્ણાંક idx);
સ્થિર પૂર્ણાંક find_term_bykeys(char_u *src, પૂર્ણાંક *len);
સ્થિર પૂર્ણાંક term_is_builtin(char_u *name);
સ્થિર પૂર્ણાંક term_7to8bit(char_u *p);
સ્થિર શૂન્ય accept_modifiers_for_function_ચાવીઓ(શૂન્ય);

#જો 0  // Change to 1 to enable ch_log() calls for termresponse debugging.
# define DEBUG_TERMRESPONSE
# define LOG_TR1(str) \
		ch_log(ખાલી, "TermResp: %s " str, \
			must_redraw == UPD_NOT_VALID ? "NV" \
			: must_redraw == UPD_CLEAR ? "CL" : "  ")
# define LOG_TRN(fmt,...) \
		ch_log(ખાલી, "TermResp: %s " fmt, \
			must_redraw == UPD_NOT_VALID ? "NV" \
			: must_redraw == UPD_CLEAR ? "CL" : "  ", __VA_ARGS__)
#નહીંતર
# define LOG_TR1(str) કરો { /**/ } જ્યારે (0)
# define LOG_TRN(fmt,...) કરો { /**/ } જ્યારે (0)
#endif

#ifdef HAVE_TGETENT
સ્થિર અક્ષર *invoke_tgetent(char_u *, char_u *);

/*
 * Here is our own prototype for tgetstr(), any prototypes from the include
 * files have been disabled by the define at the start of this file.
 */
અક્ષર		*tgetstr(અક્ષર *, અક્ષર **);
#endif

પ્રકારવ્યાખ્યા ગણના {
    STATUS_GET,		// send request when switching to RAW mode
    STATUS_SENT,	// did send request, checking for response
    STATUS_GOT,		// received response
    STATUS_FAIL		// timed out
} request_progress_T;

પ્રકારવ્યાખ્યા સંરચના {
    request_progress_T	    tr_progress;
    time_t		    tr_start;	// when request was sent, -1 for never
} termrequest_T;

// Holds state for current OSC response.
પ્રકારવ્યાખ્યા સંરચના
{
    પૂર્ણાંક		    processing;	// If we are in the middle of an OSC response
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
સ્થિર termrequest_T crv_status = TERMREQUEST_INIT;

// Request Cursor position report:
સ્થિર termrequest_T u7_status = TERMREQUEST_INIT;

// Request xterm compatibility check:
સ્થિર termrequest_T xcc_status = TERMREQUEST_INIT;

#ifdef FEAT_TERMRESPONSE
# ifdef FEAT_TERMINAL
// Request foreground color report:
સ્થિર termrequest_T rfg_status = TERMREQUEST_INIT;
સ્થિર પૂર્ણાંક fg_r = 0;
સ્થિર પૂર્ણાંક fg_g = 0;
સ્થિર પૂર્ણાંક fg_b = 0;
# endif
// Background color values from the OSC 11 response, also used by the
// image backend to flatten RGBA alpha onto the actual terminal background.
સ્થિર પૂર્ણાંક bg_r = 255;
સ્થિર પૂર્ણાંક bg_g = 255;
સ્થિર પૂર્ણાંક bg_b = 255;

// Request background color report:
સ્થિર termrequest_T rbg_status = TERMREQUEST_INIT;

// Request cursor blinking mode report:
સ્થિર termrequest_T rbm_status = TERMREQUEST_INIT;

// Request cursor style report:
સ્થિર termrequest_T rcs_status = TERMREQUEST_INIT;

// Request window's position report:
સ્થિર termrequest_T winpos_status = TERMREQUEST_INIT;

// Request DECRQM (DEC mode) report:
સ્થિર termrequest_T decrqm_status = TERMREQUEST_INIT;

સ્થિર termrequest_T *all_termrequests[] = {
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
    ખાલી
};

// The t_8u code may default to a value but get reset when the term response is
// received.  To avoid redrawing too often, only redraw when t_8u is not reset
// and it was supposed to be written.  Unless t_8u was set explicitly.
// FALSE -> don't output t_8u yet
// MAYBE -> tried outputting t_8u while FALSE
// OK    -> can write t_8u
પૂર્ણાંક write_t_8u_state = FALSE;
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
બાહ્ય short ospeed;
#  નહીંતર
short ospeed;
#  endif
# endif
# ifndef HAVE_UP_BC_PC
#  ifdef UP_BC_PC_EXTERN
બાહ્ય અક્ષર *UP, *BC, PC;
#  નહીંતર
અક્ષર *UP, *BC, PC;
#  endif
# endif

# define TGETSTR(s, p)	vim_tgetstr((s), (p))
# define TGETENT(b, t)	tgetent((અક્ષર *)(b), (અક્ષર *)(t))
સ્થિર char_u *vim_tgetstr(અક્ષર *s, char_u **pp);
#endif // HAVE_TGETENT

સ્થિર પૂર્ણાંક  detected_8bit = FALSE;	// detected 8-bit terminal

#જો (defined(UNIX) || defined(VMS))
સ્થિર પૂર્ણાંક focus_state = MAYBE; // TRUE if the Vim window has focus
#endif

#ifdef FEAT_TERMRESPONSE
// When the cursor shape was detected these values are used:
// 1: block, 2: underline, 3: vertical bar
સ્થિર પૂર્ણાંક initial_cursor_shape = 0;

// The blink flag from the style response may be inverted from the actual
// blinking state, xterm XORs the flags.
સ્થિર પૂર્ણાંક initial_cursor_shape_blink = FALSE;

// The blink flag from the blinking-cursor mode response
સ્થિર પૂર્ણાંક initial_cursor_blink = FALSE;
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
સ્થિર પૂર્ણાંક sync_output_setting = 0;

// > 0: Currently batching output
// == 0: No synchronized output
સ્થિર પૂર્ણાંક sync_output_state = 0;

#ifdef UNIX
// DEC mode 2048 (in-band window resize events)
// https://gist.github.com/rockorager/e695fb2924d36b2bcf1fff4a3704bd83
સ્થિર પૂર્ણાંક win_resize_setting = 0;
સ્થિર bool win_resize_enabled = false;
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
પ્રકારવ્યાખ્યા સંરચના
{
    પૂર્ણાંક		bt_entry;   // either a KS_xxx code (>= 0), or a K_xxx code.
    અક્ષર	*bt_string; //કિંમત
} tcap_entry_T;

/*
 * Standard ANSI terminal, default for Unix.
 */
સ્થિર tcap_entry_T builtin_ansi[] = {
    {(પૂર્ણાંક)KS_CE,	"\033[K"},
    {(પૂર્ણાંક)KS_AL,	"\033[L"},
#ifdef TERMINFO
    {(પૂર્ણાંક)KS_CAL,	"\033[%p1%dL"},
#નહીંતર
    {(પૂર્ણાંક)KS_CAL,	"\033[%dL"},
#endif
    {(પૂર્ણાંક)KS_DL,	"\033[M"},
#ifdef TERMINFO
    {(પૂર્ણાંક)KS_CDL,	"\033[%p1%dM"},
#નહીંતર
    {(પૂર્ણાંક)KS_CDL,	"\033[%dM"},
#endif
    {(પૂર્ણાંક)KS_CL,	"\033[H\033[2J"},
    {(પૂર્ણાંક)KS_ME,	"\033[0m"},
    {(પૂર્ણાંક)KS_MR,	"\033[7m"},
    {(પૂર્ણાંક)KS_MS,	"y"},
    {(પૂર્ણાંક)KS_UT,	"y"},		// guessed
    {(પૂર્ણાંક)KS_LE,	"\b"},
#ifdef TERMINFO
    {(પૂર્ણાંક)KS_CM,	"\033[%i%p1%d;%p2%dH"},
#નહીંતર
    {(પૂર્ણાંક)KS_CM,	"\033[%i%d;%dH"},
#endif
#ifdef TERMINFO
    {(પૂર્ણાંક)KS_CRI,	"\033[%p1%dC"},
#નહીંતર
    {(પૂર્ણાંક)KS_CRI,	"\033[%dC"},
#endif

    {(પૂર્ણાંક)KS_NAME,	ખાલી}  // end marker
};

/*
 * VT320 is working as an ANSI terminal compatible DEC terminal.
 * (it covers VT1x0, VT2x0 and VT3x0 up to VT320 on VMS as well)
 * TODO:- rewrite ESC[ codes to CSI
 *      - keyboard languages (CSI ? 26 n)
 */
સ્થિર tcap_entry_T builtin_vt320[] = {
    {(પૂર્ણાંક)KS_CE,	"\033[K"},
    {(પૂર્ણાંક)KS_AL,	"\033[L"},
#ifdef TERMINFO
    {(પૂર્ણાંક)KS_CAL,	"\033[%p1%dL"},
#નહીંતર
    {(પૂર્ણાંક)KS_CAL,	"\033[%dL"},
#endif
    {(પૂર્ણાંક)KS_DL,	"\033[M"},
#ifdef TERMINFO
    {(પૂર્ણાંક)KS_CDL,	"\033[%p1%dM"},
#નહીંતર
    {(પૂર્ણાંક)KS_CDL,	"\033[%dM"},
#endif
    {(પૂર્ણાંક)KS_CL,	"\033[H\033[2J"},
    {(પૂર્ણાંક)KS_CD,	"\033[J"},
    {(પૂર્ણાંક)KS_CCO,	"8"},			// allow 8 colors
    {(પૂર્ણાંક)KS_ME,	"\033[0m"},
    {(પૂર્ણાંક)KS_MR,	"\033[7m"},
    {(પૂર્ણાંક)KS_MD,	"\033[1m"},  // bold mode
    {(પૂર્ણાંક)KS_SE,	"\033[22m"},// normal mode
    {(પૂર્ણાંક)KS_UE,	"\033[24m"},// exit underscore mode
    {(પૂર્ણાંક)KS_US,	"\033[4m"},  // underscore mode
    {(પૂર્ણાંક)KS_CZH,	"\033[34;43m"},  // italic mode: blue text on yellow
    {(પૂર્ણાંક)KS_CZR,	"\033[0m"},	    // italic mode end
    {(પૂર્ણાંક)KS_CAB,	"\033[4%dm"},	    // set background color (ANSI)
    {(પૂર્ણાંક)KS_CAF,	"\033[3%dm"},	    // set foreground color (ANSI)
    {(પૂર્ણાંક)KS_CSB,	"\033[102;%dm"},	// set screen background color
    {(પૂર્ણાંક)KS_CSF,	"\033[101;%dm"},	// set screen foreground color
    {(પૂર્ણાંક)KS_MS,	"y"},
    {(પૂર્ણાંક)KS_UT,	"y"},
    {(પૂર્ણાંક)KS_XN,	"y"},
    {(પૂર્ણાંક)KS_LE,	"\b"},
#ifdef TERMINFO
    {(પૂર્ણાંક)KS_CM,	"\033[%i%p1%d;%p2%dH"},
#નહીંતર
    {(પૂર્ણાંક)KS_CM,	"\033[%i%d;%dH"},
#endif
#ifdef TERMINFO
    {(પૂર્ણાંક)KS_CRI,	"\033[%p1%dC"},
#નહીંતર
    {(પૂર્ણાંક)KS_CRI,	"\033[%dC"},
#endif
    {K_UP,		"\033[A"},
    {K_DOWN,		"\033[B"},
    {K_જમણું,		"\033[C"},
    {K_ડાબું,		"\033[D"},
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

    {(પૂર્ણાંક)KS_NAME,	ખાલી}  // end marker
};

/*
 * Ordinary vt52
 */
સ્થિર tcap_entry_T builtin_vt52[] = {
    {(પૂર્ણાંક)KS_CE,	"\033K"},
    {(પૂર્ણાંક)KS_CD,	"\033J"},
#ifdef TERMINFO
    {(પૂર્ણાંક)KS_CM,	"\033Y%p1%' '%+%c%p2%' '%+%c"},
#નહીંતર
    {(પૂર્ણાંક)KS_CM,	"\033Y%+ %+ "},
#endif
    {(પૂર્ણાંક)KS_LE,	"\b"},
    {(પૂર્ણાંક)KS_SR,	"\033I"},
    {(પૂર્ણાંક)KS_AL,	"\033L"},
    {(પૂર્ણાંક)KS_DL,	"\033M"},
    {K_UP,		"\033A"},
    {K_DOWN,		"\033B"},
    {K_ડાબું,		"\033D"},
    {K_જમણું,		"\033C"},
    {K_F1,		"\033P"},
    {K_F2,		"\033Q"},
    {K_F3,		"\033R"},
    {(પૂર્ણાંક)KS_CL,	"\033H\033J"},
    {(પૂર્ણાંક)KS_MS,	"y"},

    {(પૂર્ણાંક)KS_NAME,	ખાલી}  // end marker
};

/*
 * Builtin xterm with Vim-specific entries.
 */
સ્થિર tcap_entry_T builtin_xterm[] = {
    {(પૂર્ણાંક)KS_CE,	"\033[K"},
    {(પૂર્ણાંક)KS_AL,	"\033[L"},
#ifdef TERMINFO
    {(પૂર્ણાંક)KS_CAL,	"\033[%p1%dL"},
#નહીંતર
    {(પૂર્ણાંક)KS_CAL,	"\033[%dL"},
#endif
    {(પૂર્ણાંક)KS_DL,	"\033[M"},
#ifdef TERMINFO
    {(પૂર્ણાંક)KS_CDL,	"\033[%p1%dM"},
#નહીંતર
    {(પૂર્ણાંક)KS_CDL,	"\033[%dM"},
#endif
#ifdef TERMINFO
    {(પૂર્ણાંક)KS_CS,	"\033[%i%p1%d;%p2%dr"},
#નહીંતર
    {(પૂર્ણાંક)KS_CS,	"\033[%i%d;%dr"},
#endif
    {(પૂર્ણાંક)KS_CL,	"\033[H\033[2J"},
    {(પૂર્ણાંક)KS_CD,	"\033[J"},
    {(પૂર્ણાંક)KS_ME,	"\033[m"},
    {(પૂર્ણાંક)KS_MR,	"\033[7m"},
    {(પૂર્ણાંક)KS_MD,	"\033[1m"},
    {(પૂર્ણાંક)KS_UE,	"\033[m"},
    {(પૂર્ણાંક)KS_US,	"\033[4m"},
    {(પૂર્ણાંક)KS_STE,	"\033[29m"},
    {(પૂર્ણાંક)KS_STS,	"\033[9m"},
    {(પૂર્ણાંક)KS_MS,	"y"},
    {(પૂર્ણાંક)KS_UT,	"y"},
    {(પૂર્ણાંક)KS_LE,	"\b"},
    {(પૂર્ણાંક)KS_VI,	"\033[?25l"},
    {(પૂર્ણાંક)KS_VE,	"\033[?25h"},
    {(પૂર્ણાંક)KS_VS,	"\033[?12h"},
    {(પૂર્ણાંક)KS_CVS,	"\033[?12l"},
#ifdef TERMINFO
    {(પૂર્ણાંક)KS_CSH,	"\033[%p1%d q"},
#નહીંતર
    {(પૂર્ણાંક)KS_CSH,	"\033[%d q"},
#endif
    {(પૂર્ણાંક)KS_CRC,	"\033[?12$p"},
    {(પૂર્ણાંક)KS_CRS,	"\033P$q q\033\\"},
#ifdef TERMINFO
    {(પૂર્ણાંક)KS_CM,	"\033[%i%p1%d;%p2%dH"},
#નહીંતર
    {(પૂર્ણાંક)KS_CM,	"\033[%i%d;%dH"},
#endif
    {(પૂર્ણાંક)KS_SR,	"\033M"},
#ifdef TERMINFO
    {(પૂર્ણાંક)KS_CRI,	"\033[%p1%dC"},
#નહીંતર
    {(પૂર્ણાંક)KS_CRI,	"\033[%dC"},
#endif
    {(પૂર્ણાંક)KS_KS,	"\033[?1h\033="},
    {(પૂર્ણાંક)KS_KE,	"\033[?1l\033>"},
#ifdef FEAT_XTERM_SAVE
    {(પૂર્ણાંક)KS_TI,	"\0337\033[?47h"},
    {(પૂર્ણાંક)KS_TE,	"\033[?47l\0338"},
#endif
    // These are now under control of the 'keyprotocol' option, see
    // "builtin_mok2".
    // {(int)KS_CTI,	"\033[>4;2m"},
    // {(int)KS_CRK,	"\033[?4m"},
    // {(int)KS_CTE,	"\033[>4;m"},
    {(પૂર્ણાંક)KS_CIS,	"\033]1;"},
    {(પૂર્ણાંક)KS_CIE,	"\007"},
    {(પૂર્ણાંક)KS_TS,	"\033]2;"},
    {(પૂર્ણાંક)KS_FS,	"\007"},
    {(પૂર્ણાંક)KS_CSC,	"\033]12;"},
    {(પૂર્ણાંક)KS_CEC,	"\007"},
#ifdef TERMINFO
    {(પૂર્ણાંક)KS_CWS,	"\033[8;%p1%d;%p2%dt"},
    {(પૂર્ણાંક)KS_CWP,	"\033[3;%p1%d;%p2%dt"},
    {(પૂર્ણાંક)KS_CGP,	"\033[13t"},
#નહીંતર
    {(પૂર્ણાંક)KS_CWS,	"\033[8;%d;%dt"},
    {(પૂર્ણાંક)KS_CWP,	"\033[3;%d;%dt"},
    {(પૂર્ણાંક)KS_CGP,	"\033[13t"},
#endif
    {(પૂર્ણાંક)KS_CRV,	"\033[>c"},
    {(પૂર્ણાંક)KS_CXM,	"\033[?1006;1000%?%p1%{1}%=%th%el%;"},
    {(પૂર્ણાંક)KS_RFG,	"\033]10;?\007"},
    {(પૂર્ણાંક)KS_RBG,	"\033]11;?\007"},
    {(પૂર્ણાંક)KS_U7,	"\033[6n"},
    {(પૂર્ણાંક)KS_CAU,	"\033[58;5;%dm"},
    {(પૂર્ણાંક)KS_CBE,	"\033[?2004h"},
    {(પૂર્ણાંક)KS_CBD,	"\033[?2004l"},
    {(પૂર્ણાંક)KS_CST,	"\033[22;2t"},
    {(પૂર્ણાંક)KS_CRT,	"\033[23;2t"},
    {(પૂર્ણાંક)KS_SSI,	"\033[22;1t"},
    {(પૂર્ણાંક)KS_SRI,	"\033[23;1t"},
#જો (defined(UNIX) || defined(VMS))
    {(પૂર્ણાંક)KS_FD,	"\033[?1004l"},
    {(પૂર્ણાંક)KS_FE,	"\033[?1004h"},
#endif

    {K_UP,		"\033O*A"},
    {K_DOWN,		"\033O*B"},
    {K_જમણું,		"\033O*C"},
    {K_ડાબું,		"\033O*D"},
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

    {BT_EXTRA_ચાવીઓ,   ""},
    {TERMCAP2_ચાવી('k', '0'), "\033[10;*~"}, // F0
    {TERMCAP2_ચાવી('F', '3'), "\033[25;*~"}, // F13
    // F14 and F15 are missing, because they send the same codes as the undo
    // and help key, although they don't work on all keyboards.
    {TERMCAP2_ચાવી('F', '6'), "\033[29;*~"}, // F16
    {TERMCAP2_ચાવી('F', '7'), "\033[31;*~"}, // F17
    {TERMCAP2_ચાવી('F', '8'), "\033[32;*~"}, // F18
    {TERMCAP2_ચાવી('F', '9'), "\033[33;*~"}, // F19
    {TERMCAP2_ચાવી('F', 'A'), "\033[34;*~"}, // F20

    {TERMCAP2_ચાવી('F', 'B'), "\033[42;*~"}, // F21
    {TERMCAP2_ચાવી('F', 'C'), "\033[43;*~"}, // F22
    {TERMCAP2_ચાવી('F', 'D'), "\033[44;*~"}, // F23
    {TERMCAP2_ચાવી('F', 'E'), "\033[45;*~"}, // F24
    {TERMCAP2_ચાવી('F', 'F'), "\033[46;*~"}, // F25
    {TERMCAP2_ચાવી('F', 'G'), "\033[47;*~"}, // F26
    {TERMCAP2_ચાવી('F', 'H'), "\033[48;*~"}, // F27
    {TERMCAP2_ચાવી('F', 'I'), "\033[49;*~"}, // F28
    {TERMCAP2_ચાવી('F', 'J'), "\033[50;*~"}, // F29
    {TERMCAP2_ચાવી('F', 'K'), "\033[51;*~"}, // F30

    {TERMCAP2_ચાવી('F', 'L'), "\033[52;*~"}, // F31
    {TERMCAP2_ચાવી('F', 'M'), "\033[53;*~"}, // F32
    {TERMCAP2_ચાવી('F', 'N'), "\033[54;*~"}, // F33
    {TERMCAP2_ચાવી('F', 'O'), "\033[55;*~"}, // F34
    {TERMCAP2_ચાવી('F', 'P'), "\033[56;*~"}, // F35
    {TERMCAP2_ચાવી('F', 'Q'), "\033[57;*~"}, // F36
    {TERMCAP2_ચાવી('F', 'R'), "\033[58;*~"}, // F37

    {(પૂર્ણાંક)KS_NAME,	ખાલી}  // end marker
};

/*
 * Additions for using modifyOtherKeys level 2.  Same as what is used for
 * xterm.
 */
સ્થિર tcap_entry_T builtin_mok2[] = {
    // t_TI enables modifyOtherKeys level 2
    {(પૂર્ણાંક)KS_CTI,	"\033[>4;2m"},

    // XTQMODKEYS was added in xterm version 377: "CSI ? 4 m" which should
    // return "{lead} > 4 ; Pv m".  Before version 377 we expect it to have no
    // effect.
    {(પૂર્ણાંક)KS_CRK,	"\033[?4m"},

    // t_TE disables modifyOtherKeys
    {(પૂર્ણાંક)KS_CTE,	"\033[>4;m"},

    {(પૂર્ણાંક)KS_NAME,	ખાલી}  // end marker
};

/*
 * Additions for using the Kitty keyboard protocol.
 */
સ્થિર tcap_entry_T builtin_kitty[] = {
    // t_TI enables the kitty keyboard protocol.
    {(પૂર્ણાંક)KS_CTI,	"\033[=1;1u"},

    // t_RK requests the kitty keyboard protocol state
    {(પૂર્ણાંક)KS_CRK,	"\033[?u"},

    // t_TE also disables modifyOtherKeys, because t_TI from xterm may already
    // have been used.
    {(પૂર્ણાંક)KS_CTE,	"\033[>4;m\033[=0;1u"},

    // t_RF request terminal foreground color
    {(પૂર્ણાંક)KS_RFG,	"\033]10;?\033\\"},

    // t_RB request terminal background color
    {(પૂર્ણાંક)KS_RBG,	"\033]11;?\033\\"},

    {(પૂર્ણાંક)KS_NAME,	ખાલી}  // end marker
};

#ifdef HAVE_TGETENT
/*
 * Additions for enabling/disabling synchronized output mode for terminal.
 */
સ્થિર tcap_entry_T builtin_sync_output[] = {
    {(પૂર્ણાંક)KS_BSU,	"\033[?2026h"},
    {(પૂર્ણાંક)KS_ESU,	"\033[?2026l"},
    {(પૂર્ણાંક)KS_NAME,	ખાલી}  // end marker
};
#endif

#ifdef FEAT_TERMRESPONSE
/*
 * List of DECRQM modes that Vim supports
 */
સ્થિર અચલ પૂર્ણાંક dec_modes[] = {
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
સ્થિર tcap_entry_T builtin_rgb[] = {
    // These are printf strings, not terminal codes.
    {(પૂર્ણાંક)KS_8F,	"\033[38;2;%lu;%lu;%lum"},
    {(પૂર્ણાંક)KS_8B,	"\033[48;2;%lu;%lu;%lum"},
    {(પૂર્ણાંક)KS_8U,	"\033[58;2;%lu;%lu;%lum"},

    {(પૂર્ણાંક)KS_NAME,	ખાલી}  // end marker
};
#endif

#ifdef HAVE_TGETENT
સ્થિર tcap_entry_T special_term[] = {
    // These are printf strings, not terminal codes.
    {(પૂર્ણાંક)KS_CF,	"\033[%dm"},
    {(પૂર્ણાંક)KS_NAME,	ખાલી}  // end marker
};
#endif

/*
 * iris-ansi for Silicon Graphics machines.
 */
સ્થિર tcap_entry_T builtin_iris_ansi[] = {
    {(પૂર્ણાંક)KS_CE,	"\033[K"},
    {(પૂર્ણાંક)KS_CD,	"\033[J"},
    {(પૂર્ણાંક)KS_AL,	"\033[L"},
#ifdef TERMINFO
    {(પૂર્ણાંક)KS_CAL,	"\033[%p1%dL"},
#નહીંતર
    {(પૂર્ણાંક)KS_CAL,	"\033[%dL"},
#endif
    {(પૂર્ણાંક)KS_DL,	"\033[M"},
#ifdef TERMINFO
    {(પૂર્ણાંક)KS_CDL,	"\033[%p1%dM"},
#નહીંતર
    {(પૂર્ણાંક)KS_CDL,	"\033[%dM"},
#endif
#જો 0	// The scroll region is not working as Vim expects.
# ifdef TERMINFO
    {(પૂર્ણાંક)KS_CS,	"\033[%i%p1%d;%p2%dr"},
# નહીંતર
    {(પૂર્ણાંક)KS_CS,	"\033[%i%d;%dr"},
# endif
#endif
    {(પૂર્ણાંક)KS_CL,	"\033[H\033[2J"},
    {(પૂર્ણાંક)KS_VE,	"\033[9/y\033[12/y"},	// These aren't documented
    {(પૂર્ણાંક)KS_VS,	"\033[10/y\033[=1h\033[=2l"}, // These aren't documented
    {(પૂર્ણાંક)KS_TI,	"\033[=6h"},
    {(પૂર્ણાંક)KS_TE,	"\033[=6l"},
    {(પૂર્ણાંક)KS_SE,	"\033[21;27m"},
    {(પૂર્ણાંક)KS_SO,	"\033[1;7m"},
    {(પૂર્ણાંક)KS_ME,	"\033[m"},
    {(પૂર્ણાંક)KS_MR,	"\033[7m"},
    {(પૂર્ણાંક)KS_MD,	"\033[1m"},
    {(પૂર્ણાંક)KS_CCO,	"8"},			// allow 8 colors
    {(પૂર્ણાંક)KS_CZH,	"\033[3m"},		// italic mode on
    {(પૂર્ણાંક)KS_CZR,	"\033[23m"},		// italic mode off
    {(પૂર્ણાંક)KS_US,	"\033[4m"},		// underline on
    {(પૂર્ણાંક)KS_UE,	"\033[24m"},		// underline off
#ifdef TERMINFO
    {(પૂર્ણાંક)KS_CAB,	"\033[4%p1%dm"},    // set background color (ANSI)
    {(પૂર્ણાંક)KS_CAF,	"\033[3%p1%dm"},    // set foreground color (ANSI)
    {(પૂર્ણાંક)KS_CSB,	"\033[102;%p1%dm"}, // set screen background color
    {(પૂર્ણાંક)KS_CSF,	"\033[101;%p1%dm"}, // set screen foreground color
#નહીંતર
    {(પૂર્ણાંક)KS_CAB,	"\033[4%dm"},	    // set background color (ANSI)
    {(પૂર્ણાંક)KS_CAF,	"\033[3%dm"},	    // set foreground color (ANSI)
    {(પૂર્ણાંક)KS_CSB,	"\033[102;%dm"},    // set screen background color
    {(પૂર્ણાંક)KS_CSF,	"\033[101;%dm"},    // set screen foreground color
#endif
    {(પૂર્ણાંક)KS_MS,	"y"},		// guessed
    {(પૂર્ણાંક)KS_UT,	"y"},		// guessed
    {(પૂર્ણાંક)KS_LE,	"\b"},
#ifdef TERMINFO
    {(પૂર્ણાંક)KS_CM,	"\033[%i%p1%d;%p2%dH"},
#નહીંતર
    {(પૂર્ણાંક)KS_CM,	"\033[%i%d;%dH"},
#endif
    {(પૂર્ણાંક)KS_SR,	"\033M"},
#ifdef TERMINFO
    {(પૂર્ણાંક)KS_CRI,	"\033[%p1%dC"},
#નહીંતર
    {(પૂર્ણાંક)KS_CRI,	"\033[%dC"},
#endif
    {(પૂર્ણાંક)KS_CIS,	"\033P3.y"},
    {(પૂર્ણાંક)KS_CIE,	"\234"},    // ST "String Terminator"
    {(પૂર્ણાંક)KS_TS,	"\033P1.y"},
    {(પૂર્ણાંક)KS_FS,	"\234"},    // ST "String Terminator"
#ifdef TERMINFO
    {(પૂર્ણાંક)KS_CWS,	"\033[203;%p1%d;%p2%d/y"},
    {(પૂર્ણાંક)KS_CWP,	"\033[205;%p1%d;%p2%d/y"},
#નહીંતર
    {(પૂર્ણાંક)KS_CWS,	"\033[203;%d;%d/y"},
    {(પૂર્ણાંક)KS_CWP,	"\033[205;%d;%d/y"},
#endif
    {K_UP,		"\033[A"},
    {K_DOWN,		"\033[B"},
    {K_ડાબું,		"\033[D"},
    {K_જમણું,		"\033[C"},
    {K_S_UP,		"\033[161q"},
    {K_S_DOWN,		"\033[164q"},
    {K_S_ડાબું,		"\033[158q"},
    {K_S_જમણું,		"\033[167q"},
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

    {(પૂર્ણાંક)KS_NAME,	ખાલી}  // end marker
};

/*
 * These codes are valid when nansi.sys or equivalent has been installed.
 * Function keys on a PC are preceded with a NUL. These are converted into
 * K_NUL '\316' in mch_inchar(), because we cannot handle NULs in key codes.
 * CTRL-arrow is used instead of SHIFT-arrow.
 */
સ્થિર tcap_entry_T builtin_pcansi[] = {
    {(પૂર્ણાંક)KS_DL,	"\033[M"},
    {(પૂર્ણાંક)KS_AL,	"\033[L"},
    {(પૂર્ણાંક)KS_CE,	"\033[K"},
    {(પૂર્ણાંક)KS_CL,	"\033[2J"},
    {(પૂર્ણાંક)KS_ME,	"\033[0m"},
    {(પૂર્ણાંક)KS_MR,	"\033[5m"},	// reverse: black on lightgrey
    {(પૂર્ણાંક)KS_MD,	"\033[1m"},	// bold: white text
    {(પૂર્ણાંક)KS_SE,	"\033[0m"},	// standout end
    {(પૂર્ણાંક)KS_SO,	"\033[31m"},	// standout: white on blue
    {(પૂર્ણાંક)KS_CZH,	"\033[34;43m"},	// italic mode: blue text on yellow
    {(પૂર્ણાંક)KS_CZR,	"\033[0m"},	// italic mode end
    {(પૂર્ણાંક)KS_US,	"\033[36;41m"},	// underscore mode: cyan text on red
    {(પૂર્ણાંક)KS_UE,	"\033[0m"},	// underscore mode end
    {(પૂર્ણાંક)KS_CCO,	"8"},		// allow 8 colors
#ifdef TERMINFO
    {(પૂર્ણાંક)KS_CAB,	"\033[4%p1%dm"},// set background color
    {(પૂર્ણાંક)KS_CAF,	"\033[3%p1%dm"},// set foreground color
#નહીંતર
    {(પૂર્ણાંક)KS_CAB,	"\033[4%dm"},	// set background color
    {(પૂર્ણાંક)KS_CAF,	"\033[3%dm"},	// set foreground color
#endif
    {(પૂર્ણાંક)KS_OP,	"\033[0m"},	// reset colors
    {(પૂર્ણાંક)KS_MS,	"y"},
    {(પૂર્ણાંક)KS_UT,	"y"},		// guessed
    {(પૂર્ણાંક)KS_LE,	"\b"},
#ifdef TERMINFO
    {(પૂર્ણાંક)KS_CM,	"\033[%i%p1%d;%p2%dH"},
#નહીંતર
    {(પૂર્ણાંક)KS_CM,	"\033[%i%d;%dH"},
#endif
#ifdef TERMINFO
    {(પૂર્ણાંક)KS_CRI,	"\033[%p1%dC"},
#નહીંતર
    {(પૂર્ણાંક)KS_CRI,	"\033[%dC"},
#endif
    {K_UP,		"\316H"},
    {K_DOWN,		"\316P"},
    {K_ડાબું,		"\316K"},
    {K_જમણું,		"\316M"},
    {K_S_ડાબું,		"\316s"},
    {K_S_જમણું,		"\316t"},
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

    {(પૂર્ણાંક)KS_NAME,	ખાલી}  // end marker
};

/*
 * These codes are valid for the Win32 Console.  The entries that start with
 * ESC | are translated into console calls in os_win32.c.  The function keys
 * are also translated in os_win32.c.
 */
સ્થિર tcap_entry_T builtin_win32[] = {
    {(પૂર્ણાંક)KS_CE,	"\033|K"},	// clear to end of line
    {(પૂર્ણાંક)KS_AL,	"\033|L"},	// add new blank line
#ifdef TERMINFO
    {(પૂર્ણાંક)KS_CAL,	"\033|%p1%dL"},	// add number of new blank lines
#નહીંતર
    {(પૂર્ણાંક)KS_CAL,	"\033|%dL"},	// add number of new blank lines
#endif
    {(પૂર્ણાંક)KS_DL,	"\033|M"},	// delete line
#ifdef TERMINFO
    {(પૂર્ણાંક)KS_CDL,	"\033|%p1%dM"},	// delete number of lines
    {(પૂર્ણાંક)KS_CSV,	"\033|%p1%d;%p2%dV"},
#નહીંતર
    {(પૂર્ણાંક)KS_CDL,	"\033|%dM"},	// delete number of lines
    {(પૂર્ણાંક)KS_CSV,	"\033|%d;%dV"},
#endif
    {(પૂર્ણાંક)KS_CL,	"\033|J"},	// clear screen
    {(પૂર્ણાંક)KS_CD,	"\033|j"},	// clear to end of display
    {(પૂર્ણાંક)KS_VI,	"\033|v"},	// cursor invisible
    {(પૂર્ણાંક)KS_VE,	"\033|V"},	// cursor visible

    {(પૂર્ણાંક)KS_ME,	"\033|0m"},	// normal
    {(પૂર્ણાંક)KS_MR,	"\033|112m"},	// reverse: black on lightgray
    {(પૂર્ણાંક)KS_MD,	"\033|15m"},	// bold: white on black
#જો 1
    {(પૂર્ણાંક)KS_SO,	"\033|31m"},	// standout: white on blue
    {(પૂર્ણાંક)KS_SE,	"\033|0m"},	// standout end
#નહીંતર
    {(પૂર્ણાંક)KS_SO,	"\033|F"},	// standout: high intensity
    {(પૂર્ણાંક)KS_SE,	"\033|f"},	// standout end
#endif
    {(પૂર્ણાંક)KS_CZH,	"\033|225m"},	// italic: blue text on yellow
    {(પૂર્ણાંક)KS_CZR,	"\033|0m"},	// italic end
    {(પૂર્ણાંક)KS_US,	"\033|67m"},	// underscore: cyan text on red
    {(પૂર્ણાંક)KS_UE,	"\033|0m"},	// underscore end
    {(પૂર્ણાંક)KS_CCO,	"16"},		// allow 16 colors
#ifdef TERMINFO
    {(પૂર્ણાંક)KS_CAB,	"\033|%p1%db"},	// set background color
    {(પૂર્ણાંક)KS_CAF,	"\033|%p1%df"},	// set foreground color
#નહીંતર
    {(પૂર્ણાંક)KS_CAB,	"\033|%db"},	// set background color
    {(પૂર્ણાંક)KS_CAF,	"\033|%df"},	// set foreground color
#endif

    {(પૂર્ણાંક)KS_MS,	"y"},		// save to move cur in reverse mode
    {(પૂર્ણાંક)KS_UT,	"y"},
    {(પૂર્ણાંક)KS_XN,	"y"},
    {(પૂર્ણાંક)KS_LE,	"\b"},
#ifdef TERMINFO
    {(પૂર્ણાંક)KS_CM,	"\033|%i%p1%d;%p2%dH"}, // cursor motion
#નહીંતર
    {(પૂર્ણાંક)KS_CM,	"\033|%i%d;%dH"}, // cursor motion
#endif
    {(પૂર્ણાંક)KS_VB,	"\033|B"},	// visual bell
    {(પૂર્ણાંક)KS_TI,	"\033|S"},	// put terminal in termcap mode
    {(પૂર્ણાંક)KS_TE,	"\033|E"},	// out of termcap mode
#ifdef TERMINFO
    {(પૂર્ણાંક)KS_CS,	"\033|%i%p1%d;%p2%dr"}, // scroll region
#નહીંતર
    {(પૂર્ણાંક)KS_CS,	"\033|%i%d;%dr"}, // scroll region
#endif

    {K_UP,		"\316H"},
    {K_DOWN,		"\316P"},
    {K_ડાબું,		"\316K"},
    {K_જમણું,		"\316M"},
    {K_S_UP,		"\316\304"},
    {K_S_DOWN,		"\316\317"},
    {K_S_ડાબું,		"\316\311"},
    {K_C_ડાબું,		"\316s"},
    {K_S_જમણું,		"\316\313"},
    {K_C_જમણું,		"\316t"},
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

    {(પૂર્ણાંક)KS_NAME,	ખાલી}  // end marker
};

#જો defined(FEAT_GUI)
/*
 * GUI uses made-up codes, only used inside Vim.
 */
સ્થિર tcap_entry_T builtin_gui[] = {
    {(પૂર્ણાંક)KS_CE,	"\033|$"},
    {(પૂર્ણાંક)KS_AL,	"\033|i"},
# ifdef TERMINFO
    {(પૂર્ણાંક)KS_CAL,	"\033|%p1%dI"},
# નહીંતર
    {(પૂર્ણાંક)KS_CAL,	"\033|%dI"},
# endif
    {(પૂર્ણાંક)KS_DL,	"\033|d"},
# ifdef TERMINFO
    {(પૂર્ણાંક)KS_CDL,	"\033|%p1%dD"},
    {(પૂર્ણાંક)KS_CS,	"\033|%p1%d;%p2%dR"},
    {(પૂર્ણાંક)KS_CSV,	"\033|%p1%d;%p2%dV"},
# નહીંતર
    {(પૂર્ણાંક)KS_CDL,	"\033|%dD"},
    {(પૂર્ણાંક)KS_CS,	"\033|%d;%dR"},
    {(પૂર્ણાંક)KS_CSV,	"\033|%d;%dV"},
# endif
    {(પૂર્ણાંક)KS_CL,	"\033|C"},
			// attributes switched on with 'h', off with * 'H'
    {(પૂર્ણાંક)KS_ME,	"\033|31H"}, // HL_ALL
    {(પૂર્ણાંક)KS_MR,	"\033|1h"},   // HL_INVERSE
    {(પૂર્ણાંક)KS_MD,	"\033|2h"},   // HL_BOLD
    {(પૂર્ણાંક)KS_SE,	"\033|16H"}, // HL_STANDOUT
    {(પૂર્ણાંક)KS_SO,	"\033|16h"}, // HL_STANDOUT
    {(પૂર્ણાંક)KS_UE,	"\033|8H"},   // HL_UNDERLINE
    {(પૂર્ણાંક)KS_US,	"\033|8h"},   // HL_UNDERLINE
    {(પૂર્ણાંક)KS_UCE,	"\033|8C"},   // HL_UNDERCURL
    {(પૂર્ણાંક)KS_UCS,	"\033|8c"},   // HL_UNDERCURL
    {(પૂર્ણાંક)KS_STE,	"\033|4C"},   // HL_STRIKETHROUGH
    {(પૂર્ણાંક)KS_STS,	"\033|4c"},   // HL_STRIKETHROUGH
    {(પૂર્ણાંક)KS_CZR,	"\033|4H"},   // HL_ITALIC
    {(પૂર્ણાંક)KS_CZH,	"\033|4h"},   // HL_ITALIC
    {(પૂર્ણાંક)KS_VB,	"\033|f"},
    {(પૂર્ણાંક)KS_MS,	"y"},
    {(પૂર્ણાંક)KS_UT,	"y"},
    {(પૂર્ણાંક)KS_XN,	"y"},
    {(પૂર્ણાંક)KS_LE,	"\b"},		// cursor-left = BS
    {(પૂર્ણાંક)KS_ND,	"\014"},	// cursor-right = CTRL-L
# ifdef TERMINFO
    {(પૂર્ણાંક)KS_CM,	"\033|%p1%d;%p2%dM"},
# નહીંતર
    {(પૂર્ણાંક)KS_CM,	"\033|%d;%dM"},
# endif
	// there are no key sequences here, the GUI sequences are recognized
	// in check_termcode()

    {(પૂર્ણાંક)KS_NAME,	ખાલી}  // end marker
};
#endif

/*
 * Amiga console window, default for Amiga.
 */
સ્થિર tcap_entry_T builtin_amiga[] = {
    {(પૂર્ણાંક)KS_CE,	"\033[K"},
    {(પૂર્ણાંક)KS_CD,	"\033[J"},
    {(પૂર્ણાંક)KS_AL,	"\033[L"},
#ifdef TERMINFO
    {(પૂર્ણાંક)KS_CAL,	"\033[%p1%dL"},
#નહીંતર
    {(પૂર્ણાંક)KS_CAL,	"\033[%dL"},
#endif
    {(પૂર્ણાંક)KS_DL,	"\033[M"},
#ifdef TERMINFO
    {(પૂર્ણાંક)KS_CDL,	"\033[%p1%dM"},
#નહીંતર
    {(પૂર્ણાંક)KS_CDL,	"\033[%dM"},
#endif
    {(પૂર્ણાંક)KS_CL,	"\014"},
    {(પૂર્ણાંક)KS_VI,	"\033[0 p"},
    {(પૂર્ણાંક)KS_VE,	"\033[1 p"},
    {(પૂર્ણાંક)KS_ME,	"\033[0m"},
    {(પૂર્ણાંક)KS_MR,	"\033[7m"},
    {(પૂર્ણાંક)KS_MD,	"\033[1m"},
    {(પૂર્ણાંક)KS_SE,	"\033[0m"},
    {(પૂર્ણાંક)KS_SO,	"\033[33m"},
    {(પૂર્ણાંક)KS_US,	"\033[4m"},
    {(પૂર્ણાંક)KS_UE,	"\033[0m"},
    {(પૂર્ણાંક)KS_CZH,	"\033[3m"},
    {(પૂર્ણાંક)KS_CZR,	"\033[0m"},
#જો defined(__amigaos4__) || defined(__MORPHOS__) || defined(__AROS__)
    {(પૂર્ણાંક)KS_CCO,	"8"},		// allow 8 colors
# ifdef TERMINFO
    {(પૂર્ણાંક)KS_CAB,	"\033[4%p1%dm"},// set background color
    {(પૂર્ણાંક)KS_CAF,	"\033[3%p1%dm"},// set foreground color
# નહીંતર
    {(પૂર્ણાંક)KS_CAB,	"\033[4%dm"},	// set background color
    {(પૂર્ણાંક)KS_CAF,	"\033[3%dm"},	// set foreground color
# endif
    {(પૂર્ણાંક)KS_OP,	"\033[m"},	// reset colors
#endif
    {(પૂર્ણાંક)KS_MS,	"y"},
    {(પૂર્ણાંક)KS_UT,	"y"},		// guessed
    {(પૂર્ણાંક)KS_LE,	"\b"},
#ifdef TERMINFO
    {(પૂર્ણાંક)KS_CM,	"\033[%i%p1%d;%p2%dH"},
#નહીંતર
    {(પૂર્ણાંક)KS_CM,	"\033[%i%d;%dH"},
#endif
#જો defined(__MORPHOS__)
    {(પૂર્ણાંક)KS_SR,	"\033M"},
#endif
#ifdef TERMINFO
    {(પૂર્ણાંક)KS_CRI,	"\033[%p1%dC"},
#નહીંતર
    {(પૂર્ણાંક)KS_CRI,	"\033[%dC"},
#endif
    {K_UP,		"\233A"},
    {K_DOWN,		"\233B"},
    {K_ડાબું,		"\233D"},
    {K_જમણું,		"\233C"},
    {K_S_UP,		"\233T"},
    {K_S_DOWN,		"\233S"},
    {K_S_ડાબું,		"\233 A"},
    {K_S_જમણું,		"\233 @"},
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

    {BT_EXTRA_ચાવીઓ,	""},
    {TERMCAP2_ચાવી('#', '2'), "\233\065\064~"},	// shifted home key
    {TERMCAP2_ચાવી('#', '3'), "\233\065\060~"},	// shifted insert key
    {TERMCAP2_ચાવી('*', '7'), "\233\065\065~"},	// shifted end key

    {(પૂર્ણાંક)KS_NAME,	ખાલી}  // end marker
};

/*
 * The most minimal terminal: only clear screen and cursor positioning.
 */
સ્થિર tcap_entry_T builtin_dumb[] = {
    {(પૂર્ણાંક)KS_CL,	"\014"},
#ifdef TERMINFO
    {(પૂર્ણાંક)KS_CM,	"\033[%i%p1%d;%p2%dH"},
#નહીંતર
    {(પૂર્ણાંક)KS_CM,	"\033[%i%d;%dH"},
#endif

    {(પૂર્ણાંક)KS_NAME,	ખાલી}  // end marker
};

/*
 * Terminal used for debugging.
 */
સ્થિર tcap_entry_T builtin_debug[] = {
    {(પૂર્ણાંક)KS_CE,	"[CE]"},
    {(પૂર્ણાંક)KS_CD,	"[CD]"},
    {(પૂર્ણાંક)KS_AL,	"[AL]"},
#ifdef TERMINFO
    {(પૂર્ણાંક)KS_CAL,	"[CAL%p1%d]"},
#નહીંતર
    {(પૂર્ણાંક)KS_CAL,	"[CAL%d]"},
#endif
    {(પૂર્ણાંક)KS_DL,	"[DL]"},
#ifdef TERMINFO
    {(પૂર્ણાંક)KS_CDL,	"[CDL%p1%d]"},
#નહીંતર
    {(પૂર્ણાંક)KS_CDL,	"[CDL%d]"},
#endif
#ifdef TERMINFO
    {(પૂર્ણાંક)KS_CS,	"[%p1%dCS%p2%d]"},
#નહીંતર
    {(પૂર્ણાંક)KS_CS,	"[%dCS%d]"},
#endif
#ifdef TERMINFO
    {(પૂર્ણાંક)KS_CSV,	"[%p1%dCSV%p2%d]"},
#નહીંતર
    {(પૂર્ણાંક)KS_CSV,	"[%dCSV%d]"},
#endif
#ifdef TERMINFO
    {(પૂર્ણાંક)KS_CAB,	"[CAB%p1%d]"},
    {(પૂર્ણાંક)KS_CAF,	"[CAF%p1%d]"},
    {(પૂર્ણાંક)KS_CSB,	"[CSB%p1%d]"},
    {(પૂર્ણાંક)KS_CSF,	"[CSF%p1%d]"},
#નહીંતર
    {(પૂર્ણાંક)KS_CAB,	"[CAB%d]"},
    {(પૂર્ણાંક)KS_CAF,	"[CAF%d]"},
    {(પૂર્ણાંક)KS_CSB,	"[CSB%d]"},
    {(પૂર્ણાંક)KS_CSF,	"[CSF%d]"},
#endif
    {(પૂર્ણાંક)KS_CAU,	"[CAU%d]"},
    {(પૂર્ણાંક)KS_OP,	"[OP]"},
    {(પૂર્ણાંક)KS_LE,	"[LE]"},
    {(પૂર્ણાંક)KS_CL,	"[CL]"},
    {(પૂર્ણાંક)KS_VI,	"[VI]"},
    {(પૂર્ણાંક)KS_VE,	"[VE]"},
    {(પૂર્ણાંક)KS_VS,	"[VS]"},
    {(પૂર્ણાંક)KS_ME,	"[ME]"},
    {(પૂર્ણાંક)KS_MR,	"[MR]"},
    {(પૂર્ણાંક)KS_MB,	"[MB]"},
    {(પૂર્ણાંક)KS_MD,	"[MD]"},
    {(પૂર્ણાંક)KS_SE,	"[SE]"},
    {(પૂર્ણાંક)KS_SO,	"[SO]"},
    {(પૂર્ણાંક)KS_UE,	"[UE]"},
    {(પૂર્ણાંક)KS_US,	"[US]"},
    {(પૂર્ણાંક)KS_UCE,	"[UCE]"},
    {(પૂર્ણાંક)KS_UCS,	"[UCS]"},
    {(પૂર્ણાંક)KS_USS,	"[USS]"},
    {(પૂર્ણાંક)KS_DS,	"[DS]"},
    {(પૂર્ણાંક)KS_CDS,	"[CDS]"},
    {(પૂર્ણાંક)KS_STE,	"[STE]"},
    {(પૂર્ણાંક)KS_STS,	"[STS]"},
    {(પૂર્ણાંક)KS_MS,	"[MS]"},
    {(પૂર્ણાંક)KS_UT,	"[UT]"},
    {(પૂર્ણાંક)KS_XN,	"[XN]"},
#ifdef TERMINFO
    {(પૂર્ણાંક)KS_CM,	"[%p1%dCM%p2%d]"},
#નહીંતર
    {(પૂર્ણાંક)KS_CM,	"[%dCM%d]"},
#endif
    {(પૂર્ણાંક)KS_SR,	"[SR]"},
#ifdef TERMINFO
    {(પૂર્ણાંક)KS_CRI,	"[CRI%p1%d]"},
#નહીંતર
    {(પૂર્ણાંક)KS_CRI,	"[CRI%d]"},
#endif
    {(પૂર્ણાંક)KS_VB,	"[VB]"},
    {(પૂર્ણાંક)KS_KS,	"[KS]"},
    {(પૂર્ણાંક)KS_KE,	"[KE]"},
    {(પૂર્ણાંક)KS_TI,	"[TI]"},
    {(પૂર્ણાંક)KS_TE,	"[TE]"},
    {(પૂર્ણાંક)KS_CIS,	"[CIS]"},
    {(પૂર્ણાંક)KS_CIE,	"[CIE]"},
    {(પૂર્ણાંક)KS_CSC,	"[CSC]"},
    {(પૂર્ણાંક)KS_CEC,	"[CEC]"},
    {(પૂર્ણાંક)KS_TS,	"[TS]"},
    {(પૂર્ણાંક)KS_FS,	"[FS]"},
#ifdef TERMINFO
    {(પૂર્ણાંક)KS_CWS,	"[%p1%dCWS%p2%d]"},
    {(પૂર્ણાંક)KS_CWP,	"[%p1%dCWP%p2%d]"},
#નહીંતર
    {(પૂર્ણાંક)KS_CWS,	"[%dCWS%d]"},
    {(પૂર્ણાંક)KS_CWP,	"[%dCWP%d]"},
#endif
    {(પૂર્ણાંક)KS_CRV,	"[CRV]"},
    {(પૂર્ણાંક)KS_CXM,	"[CXM]"},
    {(પૂર્ણાંક)KS_U7,	"[U7]"},
    {(પૂર્ણાંક)KS_RFG,	"[RFG]"},
    {(પૂર્ણાંક)KS_RBG,	"[RBG]"},
    {(પૂર્ણાંક)KS_CF,	"[CF%d]"},
    {K_UP,		"[KU]"},
    {K_DOWN,		"[KD]"},
    {K_ડાબું,		"[KL]"},
    {K_જમણું,		"[KR]"},
    {K_XUP,		"[xKU]"},
    {K_XDOWN,		"[xKD]"},
    {K_XLEFT,		"[xKL]"},
    {K_XRIGHT,		"[xKR]"},
    {K_S_UP,		"[S-KU]"},
    {K_S_DOWN,		"[S-KD]"},
    {K_S_ડાબું,		"[S-KL]"},
    {K_C_ડાબું,		"[C-KL]"},
    {K_S_જમણું,		"[S-KR]"},
    {K_C_જમણું,		"[C-KR]"},
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

    {(પૂર્ણાંક)KS_NAME,	ખાલી}  // end marker
};

/*
 * List of builtin terminals.
 */
પ્રકારવ્યાખ્યા સંરચના {
    અક્ષર	    *bitc_name;	    // name, such as "xterm"
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
#જો defined(FEAT_GUI)
    {"gui",	    builtin_gui},
#endif
    {"amiga",	    builtin_amiga},
    {"dumb",	    builtin_dumb},
    {"debug",	    builtin_debug},

    {ખાલી,	    ખાલી},  // end marker
};

#જો defined(FEAT_TERMGUICOLORS)
    સ્થિર guicolor_T
termgui_mch_get_color(char_u *name)
{
    પાછા gui_get_color_cmn(name);
}

    guicolor_T
termgui_get_color(char_u *name)
{
    guicolor_T	t;

    જો (*name == NUL)
	પાછા INVALCOLOR;
    t = termgui_mch_get_color(name);

    જો (t == INVALCOLOR)
	semsg(_(e_cannot_allocate_color_str), name);
    પાછા t;
}

    guicolor_T
termgui_mch_get_rgb(guicolor_T color)
{
    પાછા color;
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

#જો defined(UNIX)
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
char_u *(term_strings[(પૂર્ણાંક)KS_LAST + 1]);

સ્થિર પૂર્ણાંક	need_gather = FALSE;	    // need to fill termleader[]
સ્થિર char_u	termleader[256 + 1];	    // for check_termcode()
#ifdef FEAT_TERMRESPONSE
સ્થિર પૂર્ણાંક	check_for_codes = FALSE;    // check for key code response
#endif

/*
 * Structure and table to store terminal features that can be detected by
 * querying the terminal.  Either by inspecting the termresponse or a more
 * specific request.  Besides this there are:
 * t_colors - number of colors supported
 */
પ્રકારવ્યાખ્યા સંરચના {
    અક્ષર    *tpr_name;
    પૂર્ણાંક	    tpr_set_by_termresponse;
    પૂર્ણાંક	    tpr_status;
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
#define TPR_ગણતરી		    7

સ્થિર termprop_T term_props[TPR_ગણતરી];

/*
 * Set the "rgb" terminal property from the color count.  The entry of a
 * direct color terminal, such as xterm-direct, has 16777216 colors.
 */
    સ્થિર શૂન્ય
set_rgb_term_prop(શૂન્ય)
{
    term_props[TPR_RGB].tpr_status = t_colors == 0x1000000
						       ? TPR_YES : TPR_UNKNOWN;
}

/*
 * Initialize the term_props table.
 * When "all" is FALSE only set those that are detected from the version
 * response.
 */
    શૂન્ય
init_term_props(પૂર્ણાંક all)
{
    પૂર્ણાંક i;

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

    માટે (i = 0; i < TPR_ગણતરી; ++i)
	જો (all || term_props[i].tpr_set_by_termresponse)
	    term_props[i].tpr_status = TPR_UNKNOWN;

    // Derived from the color count, not reset.
    set_rgb_term_prop();
}

#જો defined(FEAT_EVAL)
    શૂન્ય
f_terminalprops(typval_T *argvars UNUSED, typval_T *rettv)
{
# ifdef FEAT_TERMRESPONSE
    પૂર્ણાંક i;
# endif

    જો (rettv_dict_alloc(rettv) == FAIL)
	પાછા;
# ifdef FEAT_TERMRESPONSE
    માટે (i = 0; i < TPR_ગણતરી; ++i)
    {
	char_u	કિંમત[2];

	કિંમત[0] = term_props[i].tpr_status;
	કિંમત[1] = NUL;
	dict_add_string_len(rettv->vval.v_dict, term_props[i].tpr_name,
	    કિંમત, (કિંમત[0] == NUL) ? 0 : 1);
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
    સ્થિર tcap_entry_T *
find_builtin_term(char_u *term)
{
    માટે (પૂર્ણાંક i = 0; ; ++i)
    {
	char_u *name = (char_u *)builtin_terminals[i].bitc_name;
	જો (name == ખાલી)  // end marker
	    તોડો;
#ifdef UNIX
	જો (STRCMP(name, "iris-ansi") == 0 && vim_is_iris(term))
	    પાછા builtin_terminals[i].bitc_table;
	જો (STRCMP(name, "xterm") == 0 && vim_is_xterm(term))
	    પાછા builtin_terminals[i].bitc_table;
#endif
#ifdef VMS
	જો (STRCMP(name, "vt320") == 0 && vim_is_vt300(term))
	    પાછા builtin_terminals[i].bitc_table;
#endif
	જો (STRCMP(term, name) == 0)
	    પાછા builtin_terminals[i].bitc_table;
    }
    પાછા ખાલી;
}

/*
 * Apply entries from a builtin termcap.
 */
    સ્થિર શૂન્ય
apply_builtin_tcap(char_u *term, tcap_entry_T *entries, પૂર્ણાંક overwrite)
{
    પૂર્ણાંક term_8bit = term_is_8bit(term);

    માટે (tcap_entry_T *p = entries;
	      p->bt_entry != (પૂર્ણાંક)KS_NAME && p->bt_entry != BT_EXTRA_ચાવીઓ; ++p)
    {
	જો ((પૂર્ણાંક)p->bt_entry >= 0)	// KS_xx entry
	{
	    // Only set the value if it wasn't set yet or "overwrite" is TRUE.
	    જો (term_strings[p->bt_entry] == ખાલી
		    || term_strings[p->bt_entry] == empty_option
		    || overwrite)
	    {
#ifdef FEAT_EVAL
		પૂર્ણાંક opt_idx = -1;
#endif
		// 8bit terminal: use CSI instead of <Esc>[
		જો (term_8bit && term_7to8bit((char_u *)p->bt_string) != 0)
		{
		    char_u  *s, *t;

		    s = vim_strsave((char_u *)p->bt_string);
		    જો (s != ખાલી)
		    {
			માટે (t = s; *t; ++t)
			    જો (term_7to8bit(t))
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
		નહીંતર
		{
		    term_strings[p->bt_entry] = (char_u *)p->bt_string;
#ifdef FEAT_EVAL
		    opt_idx = get_term_opt_idx(&term_strings[p->bt_entry]);
#endif
		}
#ifdef FEAT_EVAL
		set_term_option_sctx_idx(ખાલી, opt_idx);
#endif
	    }
	}
	નહીંતર
	{
	    char_u  name[2];
	    name[0] = KEY2TERMCAP0((પૂર્ણાંક)p->bt_entry);
	    name[1] = KEY2TERMCAP1((પૂર્ણાંક)p->bt_entry);
	    જો (find_termcode(name) == ખાલી || overwrite)
		add_termcode(name, (char_u *)p->bt_string, term_8bit);
	}
    }
}

/*
 * Apply builtin termcap entries for a given keyprotocol.
 */
    શૂન્ય
apply_keyprotocol(char_u *term, keyprot_T prot)
{
    જો (prot == KEYPROTOCOL_KITTY)
	apply_builtin_tcap(term, builtin_kitty, TRUE);
    જો (prot == KEYPROTOCOL_MOK2)
	apply_builtin_tcap(term, builtin_mok2, TRUE);

    જો (prot != KEYPROTOCOL_NONE)
	// Some function keys may accept modifiers even though the
	// terminfo/termcap entry does not indicate this.
	accept_modifiers_for_function_ચાવીઓ();
}

/*
 * Parsing of the builtin termcap entries.
 * Caller should check if "term" is a valid builtin terminal name.
 * The terminal's name is not set, as this is already done in termcapinit().
 */
    સ્થિર શૂન્ય
parse_builtin_tcap(char_u *term)
{
    tcap_entry_T *entries = find_builtin_term(term);
    જો (entries != ખાલી)
	apply_builtin_tcap(term, entries, FALSE);
}

/*
 * Set number of colors.
 * Store it as a number in t_colors.
 * Store it as a string in T_CCO (using nr_colors[]).
 */
    શૂન્ય
set_color_ગણતરી(પૂર્ણાંક nr)
{
    char_u	nr_colors[20];		// string for number of colors

    t_colors = nr;
    જો (t_colors > 1)
	sprintf((અક્ષર *)nr_colors, "%d", t_colors);
    નહીંતર
	*nr_colors = NUL;
#જો 0
# ifdef FEAT_TERMGUICOLORS
    // xterm-direct, enable termguicolors, when it wasn't set yet
    જો (t_colors == 0x1000000 && !p_tgc_set)
	set_option_કિંમત((char_u *)"termguicolors", 1L, ખાલી, 0);
# endif
#endif
    set_string_option_direct((char_u *)"t_Co", -1, nr_colors, OPT_FREE, 0);
}

/*
 * Set the color count to "val" and redraw if it changed.
 */
    સ્થિર શૂન્ય
may_adjust_color_ગણતરી(પૂર્ણાંક val)
{
    જો (val == t_colors)
	પાછા;

    // Nr of colors changed, initialize highlighting and redraw everything.
    // This causes a redraw, which usually clears the message.  Try keeping
    // the message if it might work.
    set_keep_સંદેશ_from_hist();
    set_color_ગણતરી(val);
    init_highlight(TRUE, FALSE);
#ifdef DEBUG_TERMRESPONSE
    {
	પૂર્ણાંક r = redraw_asap(UPD_CLEAR);

	LOG_TRN("Received t_Co, redraw_asap(): %d", r);
    }
#નહીંતર
    redraw_asap(UPD_CLEAR);
#endif
}

#ifdef HAVE_TGETENT
સ્થિર અક્ષર *(ચાવી_names[]) =
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
    ખાલી
};
#endif

#જો defined(HAVE_TGETENT) || defined(FEAT_TERMGUICOLORS)
/*
 * Return TRUE if "term_strings[idx]" was not set.
 */
    સ્થિર પૂર્ણાંક
term_strings_not_set(ગણના Special_ચાવી idx)
{
    પાછા TERM_STR(idx) == ખાલી || TERM_STR(idx) == empty_option;
}
#endif

#ifdef HAVE_TGETENT
/*
 * Get the termcap entries we need with tgetstr(), tgetflag() and tgetnum().
 * "invoke_tgetent()" must have been called before.
 * If "*height" or "*width" are not zero then use the "li" and "col" entries to
 * get their value.
 */
    સ્થિર શૂન્ય
get_term_entries(પૂર્ણાંક *height, પૂર્ણાંક *width)
{
    સ્થિર સંરચના {
		    ગણના Special_ચાવી dest; // index in term_strings[]
		    અક્ષર *name;		  // termcap name for string
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
			{(ગણના Special_ચાવી)0, ખાલી}
		    };
    પૂર્ણાંક		    i;
    સ્થિર char_u   tstrbuf[TBUFSZ];
    char_u	    *tp = tstrbuf;

    /*
     * get output strings
     */
    માટે (i = 0; string_names[i].name != ખાલી; ++i)
    {
	જો (term_strings_not_set(string_names[i].dest))
	{
	    TERM_STR(string_names[i].dest) = TGETSTR(string_names[i].name, &tp);
# ifdef FEAT_EVAL
	    set_term_option_sctx_idx(string_names[i].name, -1);
# endif
	}
    }

    // tgetflag() returns 1 if the flag is present, 0 if not and
    // possibly -1 if the flag doesn't exist.
    જો ((T_MS == ખાલી || T_MS == empty_option) && tgetflag("ms") > 0)
	T_MS = (char_u *)"y";
    જો ((T_XS == ખાલી || T_XS == empty_option) && tgetflag("xs") > 0)
	T_XS = (char_u *)"y";
    જો ((T_XN == ખાલી || T_XN == empty_option) && tgetflag("xn") > 0)
	T_XN = (char_u *)"y";
    જો ((T_DB == ખાલી || T_DB == empty_option) && tgetflag("db") > 0)
	T_DB = (char_u *)"y";
    જો ((T_DA == ખાલી || T_DA == empty_option) && tgetflag("da") > 0)
	T_DA = (char_u *)"y";
    જો ((T_UT == ખાલી || T_UT == empty_option) && tgetflag("ut") > 0)
	T_UT = (char_u *)"y";
    જો ((T_XON == ખાલી || T_XON == empty_option) && tgetflag("xo") > 0)
	T_XON = (char_u *)"y";

    /*
     * get key codes
     */
    માટે (i = 0; ચાવી_names[i] != ખાલી; ++i)
	જો (find_termcode((char_u *)ચાવી_names[i]) == ખાલી)
	{
	    char_u *p = TGETSTR(ચાવી_names[i], &tp);

	    // if cursor-left == backspace, ignore it (televideo 925)
	    જો (p != ખાલી
		    && (*p != Ctrl_H
			|| ચાવી_names[i][0] != 'k'
			|| ચાવી_names[i][1] != 'l'))
		add_termcode((char_u *)ચાવી_names[i], p, FALSE);
	}

    જો (*height == 0)
	*height = tgetnum("li");
    જો (*width == 0)
	*width = tgetnum("co");

    /*
     * Get number of colors (if not done already).
     */
    જો (term_strings_not_set(KS_CCO))
    {
	set_color_ગણતરી(tgetnum("Co"));
# ifdef FEAT_EVAL
	set_term_option_sctx_idx("Co", -1);
# endif
    }

# ifndef hpux
    BC = (અક્ષર *)TGETSTR("bc", &tp);
    UP = (અક્ષર *)TGETSTR("up", &tp);
    char_u *p = TGETSTR("pc", &tp);
    જો (p != ખાલી)
	PC = *p;
# endif
}
#endif

/*
 * Report "term" is not found and list the ones we do know about.
 */
    સ્થિર શૂન્ય
report_term_error(અક્ષર *error_સંદેશ, char_u *term)
{
    mch_errmsg("\r\n");
    જો (error_સંદેશ != ખાલી)
    {
	mch_errmsg(error_સંદેશ);
	mch_errmsg("\r\n");
    }
    mch_errmsg("'");
    mch_errmsg((અક્ષર *)term);
    mch_errmsg(_("' not known. Available builtin terminals are:"));
    mch_errmsg("\r\n");

    માટે (પૂર્ણાંક i = 0; ; ++i)
    {
	અક્ષર *name = builtin_terminals[i].bitc_name;
	જો (name == ખાલી)  // end marker
	    તોડો;
	// Do not mention the "gui" entry, the user won't need to type it.
	જો (STRCMP(name, "gui") != 0)
	{
#ifdef HAVE_TGETENT
	    mch_errmsg("    builtin_");
#નહીંતર
	    mch_errmsg("    ");
#endif
	    mch_errmsg(name);
	    mch_errmsg("\r\n");
	}
    }
    // Output extra 'cmdheight' line breaks to avoid that the following error
    // message overwrites the last terminal name.
    માટે (પૂર્ણાંક i = 1; i < p_ch; ++i)
	mch_errmsg("\r\n");
}

    સ્થિર શૂન્ય
report_default_term(char_u *term)
{
    mch_errmsg(_("defaulting to '"));
    mch_errmsg((અક્ષર *)term);
    mch_errmsg("'\r\n");
    જો (emsg_silent == 0 && !in_assert_fails)
    {
	screen_start();	// don't know where cursor is now
	out_flush();
	જો (!is_not_a_term())
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
    પૂર્ણાંક		len = (પૂર્ણાંક)STRLEN(p_kpc) + 1;
    char_u	*buf = alloc(len);
    જો (buf == ખાલી)
	પાછા KEYPROTOCOL_FAIL;

    keyprot_T	ret = KEYPROTOCOL_FAIL;
    char_u	*p = p_kpc;
    જ્યારે (*p != NUL)
    {
	// Isolate one comma separated item.
	(શૂન્ય)copy_option_part(&p, buf, len, ",");
	char_u *colon = vim_strchr(buf, ':');
	જો (colon == ખાલી || colon == buf || colon[1] == NUL)
	    જાઓ theend;
	*colon = NUL;

	keyprot_T prot;
	// Note: Keep this in sync with p_kpc_protocol_values.
	જો (STRCMP(colon + 1, "none") == 0)
	    prot = KEYPROTOCOL_NONE;
	નહીંતર જો (STRCMP(colon + 1, "mok2") == 0)
	    prot = KEYPROTOCOL_MOK2;
	નહીંતર જો (STRCMP(colon + 1, "kitty") == 0)
	    prot = KEYPROTOCOL_KITTY;
	નહીંતર
	    જાઓ theend;

	regmatch_T regmatch;
	CLEAR_FIELD(regmatch);
	regmatch.rm_ic = TRUE;
	regmatch.regprog = vim_regcomp(buf, RE_MAGIC);
	જો (regmatch.regprog == ખાલી)
	    જાઓ theend;

	પૂર્ણાંક match = term != ખાલી && vim_regexec(&regmatch, term, (colnr_T)0);
	vim_regfree(regmatch.regprog);
	જો (match)
	{
	    ret = prot;
	    જાઓ theend;
	}

    }

    // No match found, use "none".
    ret = KEYPROTOCOL_NONE;

theend:
    vim_free(buf);
    પાછા ret;
}

/*
 * Set terminal options for terminal "term".
 * Return OK if terminal 'term' was found in a termcap, FAIL otherwise.
 *
 * While doing this, until ttest(), some options may be NULL, be careful.
 */
    પૂર્ણાંક
set_termname(char_u *term)
{
#ifdef HAVE_TGETENT
    પૂર્ણાંક		builtin_first = p_tbi;
    પૂર્ણાંક		try;
    પૂર્ણાંક		termcap_cleared = FALSE;
#endif
    પૂર્ણાંક		width = 0, height = 0;
    અક્ષર	*error_સંદેશ = ખાલી;
    char_u	*bs_p, *del_p;

    // In silect mode (ex -s) we don't use the 'term' option.
    જો (silent_mode)
	પાછા OK;

    detected_8bit = FALSE;		// reset 8-bit detection

    જો (term_is_builtin(term))
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
    માટે (try = builtin_first ? 0 : 1; try < 3; ++try)
    {
	/*
	 * Use external termcap
	 */
	જો (try == 1)
	{
	    char_u	    tbuf[TBUFSZ];

	    /*
	     * If the external termcap does not have a matching entry, try the
	     * builtin ones.
	     */
	    જો ((error_સંદેશ = invoke_tgetent(tbuf, term)) == ખાલી)
	    {
		જો (!termcap_cleared)
		{
		    clear_termoptions();	// clear old options
		    termcap_cleared = TRUE;
		}

		get_term_entries(&height, &width);
	    }
	}
	નહીંતર	    // try == 0 || try == 2
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
	    જો (try == 2 && builtin_first && termcap_cleared)
		તોડો;
#endif
	    /*
	     * Search for 'term' in builtin_terminals[].
	     */
	    tcap_entry_T *termp = find_builtin_term(term);
	    જો (termp == ખાલી)	// did not find it
	    {
#ifdef HAVE_TGETENT
		/*
		 * If try == 0, first try the external termcap. If that is not
		 * found we'll get back here with try == 2.
		 * If termcap_cleared is set we used the external termcap,
		 * don't complain about not finding the term in the builtin
		 * termcap.
		 */
		જો (try == 0)			// try external one
		    ચાલુ;
		જો (termcap_cleared)		// found in external termcap
		    તોડો;
#endif
		report_term_error(error_સંદેશ, term);

		// when user typed :set term=xxx, quit here
		જો (starting != NO_SCREEN)
		{
		    screen_start();	// don't know where cursor is now
		    wait_return(TRUE);
		    પાછા FAIL;
		}
		term = DEFAULT_TERM;
		report_default_term(term);
		set_string_option_direct((char_u *)"term", -1, term,
								 OPT_FREE, 0);
		display_errors();
	    }
	    out_flush();
#ifdef HAVE_TGETENT
	    જો (!termcap_cleared)
	    {
#endif
		clear_termoptions();	    // clear old options
#ifdef HAVE_TGETENT
		termcap_cleared = TRUE;
	    }
#endif
	    parse_builtin_tcap(term);

#ifdef FEAT_GUI
	    જો (term_is_gui(term))
	    {
		out_flush();
		gui_init();
		// If starting the GUI failed, don't do any of the other
		// things for this terminal
		જો (!gui.in_use)
		    પાછા FAIL;
# ifdef HAVE_TGETENT
		તોડો;		// don't try using external termcap
# endif
	    }
#endif // FEAT_GUI
	}
#ifdef HAVE_TGETENT
    }
#endif

#ifdef FEAT_GUI
    જો (!gui.in_use)
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
	જો (term_strings_not_set(KS_8F)
		&& term_strings_not_set(KS_8B)
		&& term_strings_not_set(KS_8U))
	    apply_builtin_tcap(term, builtin_rgb, TRUE);
#endif
#ifdef HAVE_TGETENT
	જો (term_strings_not_set(KS_CF))
	    apply_builtin_tcap(term, special_term, TRUE);
	જો (term_strings_not_set(KS_BSU) && term_strings_not_set(KS_ESU))
	    apply_builtin_tcap(term, builtin_sync_output, TRUE);
#endif
    }

/*
 * special: There is no info in the termcap about whether the cursor
 * positioning is relative to the start of the screen or to the start of the
 * scrolling region.  We just guess here. Only msdos pcterm is known to do it
 * relative.
 */
    જો (STRCMP(term, "pcterm") == 0)
	T_CCS = (char_u *)"yes";
    નહીંતર
	T_CCS = empty_option;

    // Special case: "kitty" may not have a "RV" entry in terminfo, but we need
    // to request the version for several other things to work.
    જો (strstr((અક્ષર *)term, "kitty") != ખાલી
					   && (T_CRV == ખાલી || *T_CRV == NUL))
	T_CRV = (char_u *)"\033[>c";

#ifdef UNIX
/*
 * Any "stty" settings override the default for t_kb from the termcap.
 * This is in os_unix.c, because it depends a lot on the version of unix that
 * is being used.
 * Don't do this when the GUI is active, it uses "t_kb" and "t_kD" directly.
 */
# ifdef FEAT_GUI
    જો (!gui.in_use)
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
    જો (!gui.in_use)
#endif
    {
	bs_p = find_termcode((char_u *)"kb");
	del_p = find_termcode((char_u *)"kD");
	જો (bs_p == ખાલી || *bs_p == NUL)
	    add_termcode((char_u *)"kb", (bs_p = (char_u *)CTRL_H_STR), FALSE);
	જો ((del_p == ખાલી || *del_p == NUL) &&
					    (bs_p == ખાલી || *bs_p != DEL))
	    add_termcode((char_u *)"kD", (char_u *)DEL_STR, FALSE);
    }

#જો defined(UNIX) || defined(VMS)
    term_is_xterm = vim_is_xterm(term);
#endif
#ifdef FEAT_TERMRESPONSE
    // Reset terminal properties that are set based on the termresponse, which
    // will be sent out soon.
    init_term_props(FALSE);
#endif

#જો defined(UNIX) || defined(VMS)
    // If the first number in t_XM is 1006 then the terminal will support SGR
    // mouse reporting.
    પૂર્ણાંક did_set_ttym = FALSE;
    જો (T_CXM != ખાલી && *T_CXM != NUL && !option_was_set((char_u *)"ttym"))
    {
	char_u *p = T_CXM;

	જ્યારે (*p != NUL && !VIM_ISDIGIT(*p))
	    ++p;
	જો (getdigits(&p) == 1006)
	{
	    did_set_ttym = TRUE;
	    set_option_કિંમત_give_err((char_u *)"ttym", 0L, (char_u *)"sgr", 0);
	}
    }

    /*
     * For Unix, set the 'ttymouse' option to the type of mouse to be used.
     * The termcode for the mouse is added as a side effect in option.c.
     */
    {
	char_u	*p = (char_u *)"";

# ifdef FEAT_MOUSE_XTERM
	જો (use_xterm_like_mouse(term))
	{
	    જો (use_xterm_mouse())
		p = ખાલી;	// keep existing value, might be "xterm2"
	    નહીંતર
		p = (char_u *)"xterm";
	}
# endif
	જો (p != ખાલી && !did_set_ttym)
	{
	    set_option_કિંમત_give_err((char_u *)"ttym", 0L, p, 0);
	    // Reset the WAS_SET flag, 'ttymouse' can be set to "sgr" or
	    // "xterm2" in check_termcode().
	    reset_option_was_set((char_u *)"ttym");
	}
	જો (p == ખાલી
# ifdef FEAT_GUI
		|| gui.in_use
# endif
		)
	    check_mouse_termcode();	// set mouse termcode anyway
    }
#નહીંતર
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
#જો (defined(UNIX) || defined(VMS))
    // First time after setting 'term' a focus event is always reported.
    focus_state = MAYBE;
#endif

#ifdef USE_TERM_CONSOLE
    // DEFAULT_TERM indicates that it is the machine console.
    જો (STRCMP(term, DEFAULT_TERM) != 0)
	term_console = FALSE;
    નહીંતર
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
    જો (starting != NO_SCREEN)
    {
	starttermcap();		// may change terminal mode
	setmouse();		// may start using the mouse
	maketitle();		// may display window title
    }

	// display initial screen after ttest() checking. jw.
    જો (width <= 0 || height <= 0)
    {
	// termcap failed to report size
	// set defaults, in case ui_get_shellsize() also fails
	width = 80;
#જો defined(MSWIN)
	height = 25;	    // console is often 25 lines
#નહીંતર
	height = 24;	    // most terminals are 24 lines
#endif
    }
    set_shellsize(width, height, FALSE);	// may change Rows
    જો (starting != NO_SCREEN)
    {
	જો (scroll_region)
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
		જો (curbuf->b_ml.ml_mfp != ખાલી)
		{
		    aucmd_prepbuf(&aco, buf);
		    જો (curbuf == buf)
		    {
			apply_autocmds(EVENT_TERMCHANGED, ખાલી, ખાલી, FALSE,
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
    (શૂન્ય)update_image_backend();
#endif

    પાછા OK;
}

#જો defined(EXITFREE)

# ifdef HAVE_DEL_CURTERM
#  include <term.h>	    // declares cur_term
# endif

/*
 * If supported, delete "cur_term", which caches terminal related entries.
 * Avoids that valgrind reports possibly lost memory.
 */
    શૂન્ય
free_cur_term(શૂન્ય)
{
# ifdef HAVE_DEL_CURTERM
    જો (cur_term)
	del_curterm(cur_term);
# endif
}

#endif

#ifdef HAVE_TGETENT
/*
 * Call tgetent()
 * Return error message if it fails, NULL if it's OK.
 */
    સ્થિર અક્ષર *
invoke_tgetent(char_u *tbuf, char_u *term)
{
    પૂર્ણાંક	    i;

    // Note: Valgrind may report a leak here, because the library keeps one
    // buffer around that we can't ever free.
    i = TGETENT(tbuf, term);
    જો (i < 0		    // -1 is always an error
# ifdef TGETENT_ZERO_ERR
	    || i == 0	    // sometimes zero is also an error
# endif
       )
    {
	// On FreeBSD tputs() gets a SEGV after a tgetent() which fails.  Call
	// tgetent() with the always existing "dumb" entry to avoid a crash or
	// hang.
	(શૂન્ય)TGETENT(tbuf, "dumb");

	જો (i < 0)
# ifdef TGETENT_ZERO_ERR
	    પાછા _(e_cannot_open_termcap_file);
	જો (i == 0)
# endif
# ifdef TERMINFO
	    પાછા _(e_terminal_entry_not_found_in_terminfo);
# નહીંતર
	    પાછા _(e_terminal_entry_not_found_in_termcap);
# endif
    }
    પાછા ખાલી;
}

/*
 * Some versions of tgetstr() have been reported to return -1 instead of NULL.
 * Fix that here.
 */
    સ્થિર char_u *
vim_tgetstr(અક્ષર *s, char_u **pp)
{
    અક્ષર	*p;

    p = tgetstr(s, (અક્ષર **)pp);
    જો (p == (અક્ષર *)-1)
	p = ખાલી;
    પાછા (char_u *)p;
}
#endif // HAVE_TGETENT

#જો defined(HAVE_TGETENT) && (defined(UNIX) || defined(VMS) || defined(MACOS_X))
/*
 * Get Columns and Rows from the termcap. Used after a window signal if the
 * ioctl() fails. It doesn't make sense to call tgetent each time if the "co"
 * and "li" entries never change. But on some systems this works.
 * Errors while getting the entries are ignored.
 */
    શૂન્ય
getlinecol(
    long	*cp,	// pointer to columns
    long	*rp)	// pointer to rows
{
    char_u	tbuf[TBUFSZ];

    જો (T_NAME == ખાલી || *T_NAME == NUL
	    || invoke_tgetent(tbuf, T_NAME) != ખાલી)
	પાછા;

    જો (*cp == 0)
	*cp = tgetnum("co");
    જો (*rp == 0)
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
    પૂર્ણાંક
add_termcap_entry(char_u *name, પૂર્ણાંક force)
{
    char_u  *term;
    પૂર્ણાંક	    ચાવી;
#ifdef HAVE_TGETENT
    char_u  *string;
    પૂર્ણાંક	    i;
    પૂર્ણાંક	    builtin_first;
    char_u  tbuf[TBUFSZ];
    char_u  tstrbuf[TBUFSZ];
    char_u  *tp = tstrbuf;
    અક્ષર    *error_સંદેશ = ખાલી;
#endif

/*
 * If the GUI is running or will start in a moment, we only support the keys
 * that the GUI can produce.
 */
#ifdef FEAT_GUI
    જો (gui.in_use || gui.starting)
	પાછા gui_mch_haskey(name);
#endif

    જો (!force && find_termcode(name) != ખાલી)	    // it's already there
	પાછા OK;

    term = T_NAME;
    જો (term == ખાલી || *term == NUL)	    // 'term' not defined yet
	પાછા FAIL;

    જો (term_is_builtin(term))		    // name starts with "builtin_"
    {
	term += 8;
#ifdef HAVE_TGETENT
	builtin_first = TRUE;
#endif
    }
#ifdef HAVE_TGETENT
    નહીંતર
	builtin_first = p_tbi;
#endif

#ifdef HAVE_TGETENT
/*
 * We can get the entry from the builtin termcap and from the external one.
 * If 'ttybuiltin' is on or the terminal name starts with "builtin_", try
 * builtin termcap first.
 * If 'ttybuiltin' is off, try external termcap first.
 */
    માટે (i = 0; i < 2; ++i)
    {
	જો ((!builtin_first) == i)
#endif
	/*
	 * Search in builtin termcaps
	 */
	{
	    tcap_entry_T *termp = find_builtin_term(term);
	    જો (termp != ખાલી)	// found it
	    {
		ચાવી = TERMCAP2_ચાવી(name[0], name[1]);
		++termp;
		જ્યારે (termp->bt_entry != (પૂર્ણાંક)KS_NAME)
		{
		    જો ((પૂર્ણાંક)termp->bt_entry == ચાવી)
		    {
			add_termcode(name, (char_u *)termp->bt_string,
							  term_is_8bit(term));
			પાછા OK;
		    }
		    ++termp;
		}
	    }
	}
#ifdef HAVE_TGETENT
	નહીંતર
	/*
	 * Search in external termcap
	 */
	{
	    error_સંદેશ = invoke_tgetent(tbuf, term);
	    જો (error_સંદેશ == ખાલી)
	    {
		string = TGETSTR((અક્ષર *)name, &tp);
		જો (string != ખાલી && *string != NUL)
		{
		    add_termcode(name, string, FALSE);
		    પાછા OK;
		}
	    }
	}
    }
#endif

    જો (SOURCING_NAME == ખાલી)
    {
#ifdef HAVE_TGETENT
	જો (error_સંદેશ != ખાલી)
	    emsg(error_સંદેશ);
	નહીંતર
#endif
	    semsg(_(e_no_str_entry_in_termcap), name);
    }
    પાછા FAIL;
}

    સ્થિર પૂર્ણાંક
term_is_builtin(char_u *name)
{
    પાછા (STRNCMP(name, "builtin_", (size_t)8) == 0);
}

/*
 * Return TRUE if terminal "name" uses CSI instead of <Esc>[.
 * Assume that the terminal is using 8-bit controls when the name contains
 * "8bit", like in "xterm-8bit".
 */
    પૂર્ણાંક
term_is_8bit(char_u *name)
{
    પાછા (detected_8bit || strstr((અક્ષર *)name, "8bit") != ખાલી);
}

/*
 * Translate terminal control chars from 7-bit to 8-bit:
 * <Esc>[ -> CSI  <M_C_[>
 * <Esc>] -> OSC  <M-C-]>
 * <Esc>O -> <M-C-O>
 */
    સ્થિર પૂર્ણાંક
term_7to8bit(char_u *p)
{
    જો (*p != ESC)
	પાછા 0;

    જો (p[1] == '[')
	પાછા CSI;
    નહીંતર જો (p[1] == ']')
	પાછા OSC;
    નહીંતર જો (p[1] == 'O')
	પાછા 0x8f;
    પાછા 0;
}

#જો defined(FEAT_GUI)
    પૂર્ણાંક
term_is_gui(char_u *name)
{
    પાછા (STRCMP(name, "builtin_gui") == 0 || STRCMP(name, "gui") == 0);
}
#endif

#જો !defined(HAVE_TGETENT) || defined(AMIGA)

    char_u *
tltoa(unsigned long i)
{
    સ્થિર char_u buf[16];
    char_u	*p;

    p = buf + 15;
    *p = '\0';
    કરો
    {
	--p;
	*p = (char_u) (i % 10 + '0');
	i /= 10;
    }
    જ્યારે (i > 0 && p > buf);
    પાછા p;
}
#endif

#ifndef HAVE_TGETENT

/*
 * minimal tgoto() implementation.
 * no padding and we only parse for %i %d and %+char
 */
    સ્થિર અક્ષર *
tgoto(અક્ષર *cm, પૂર્ણાંક x, પૂર્ણાંક y)
{
    સ્થિર અક્ષર buf[30];
    અક્ષર *p, *s, *e;

    જો (!cm)
	પાછા "OOPS";
    e = buf + 29;
    માટે (s = buf; s < e && *cm; cm++)
    {
	જો (*cm != '%')
	{
	    *s++ = *cm;
	    ચાલુ;
	}
	પસંદગી (*++cm)
	{
	સ્થિતિ 'd':
	    p = (અક્ષર *)tltoa((unsigned long)y);
	    y = x;
	    જ્યારે (*p)
		*s++ = *p++;
	    તોડો;
	સ્થિતિ 'i':
	    x++;
	    y++;
	    તોડો;
	સ્થિતિ '+':
	    *s++ = (અક્ષર)(*++cm + y);
	    y = x;
	    તોડો;
	સ્થિતિ '%':
	    *s++ = *cm;
	    તોડો;
	મૂળભૂત:
	    પાછા "OOPS";
	}
    }
    *s = '\0';
    પાછા buf;
}

#endif // HAVE_TGETENT

/*
 * Set the terminal name and initialize the terminal options.
 * If "name" is NULL or empty, get the terminal name from the environment.
 * If that fails, use the default terminal name.
 */
    શૂન્ય
termcapinit(char_u *name)
{
    char_u	*term = name;

    જો (term != ખાલી && *term == NUL)
	term = ખાલી;	    // empty name is equal to no name

#ifndef MSWIN
    જો (term == ખાલી)
	term = mch_getenv((char_u *)"TERM");
#endif
    જો (term == ખાલી || *term == NUL)
	term = DEFAULT_TERM;
    set_string_option_direct((char_u *)"term", -1, term, OPT_FREE, 0);

    // Set the default terminal name.
    set_string_default("term", term);
    set_string_default("ttytype", term);

    // Avoid using "term" here, because the next mch_getenv() may overwrite it.
    set_termname(T_NAME != ખાલી ? T_NAME : term);
}

/*
 * The number of calls to ui_write is reduced by using "out_buf".
 */
#define OUT_SIZE	8191

// add one to allow mch_write() in os_win32.c to append a NUL
સ્થિર char_u		out_buf[OUT_SIZE + 1];

સ્થિર પૂર્ણાંક		out_pos = 0;	// number of chars in out_buf

// Since the maximum number of SGR parameters shown as a normal value range is
// 16, the escape sequence length can be 4 * 16 + lead + tail.
#define MAX_ESC_SEQ_LEN	80

/*
 * out_flush(): flush the output buffer
 */
    શૂન્ય
out_flush(શૂન્ય)
{
    પૂર્ણાંક	    len;

    જો (out_pos == 0)
	પાછા;

    // set out_pos to 0 before ui_write, to avoid recursiveness
    len = out_pos;
    out_pos = 0;
    ui_write(out_buf, len, FALSE);
#ifdef FEAT_EVAL
    જો (ch_log_output != FALSE)
    {
	out_buf[len] = NUL;
	ch_log(ખાલી, "raw %s output: \"%s\"",
# ifdef FEAT_GUI
		(gui.in_use && !gui.dying && !gui.starting) ? "GUI" :
# endif
		"terminal",
		out_buf);
	જો (ch_log_output == TRUE)
	    ch_log_output = FALSE;  // only log once
    }
#endif
}

/*
 * out_flush_cursor(): flush the output buffer and redraw the cursor.
 * Does not flush recursively in the GUI to avoid slow drawing.
 */
    શૂન્ય
out_flush_cursor(
    પૂર્ણાંક	    force UNUSED,   // when TRUE, update cursor even when not moved
    પૂર્ણાંક	    clear_selection UNUSED) // clear selection under cursor
{
    mch_disable_flush();
    out_flush();
    mch_enable_flush();
#ifdef FEAT_GUI
    જો (gui.in_use)
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
    શૂન્ય
out_flush_check(શૂન્ય)
{
    જો (enc_dbcs != 0 && out_pos >= OUT_SIZE - MB_MAXBYTES)
	out_flush();
}

#ifdef FEAT_GUI
/*
 * out_trash(): Throw away the contents of the output buffer
 */
    શૂન્ય
out_trash(શૂન્ય)
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
    શૂન્ય
out_char(unsigned c)
{
#જો defined(UNIX) || defined(VMS) || defined(AMIGA) || defined(MACOS_X)
    જો (c == '\n')	// turn LF into CR-LF (CRMOD doesn't seem to do this)
	out_char('\r');
#endif

    out_buf[out_pos++] = c;

    // For testing we flush each time.
    જો (out_pos >= OUT_SIZE || p_wd)
	out_flush();
}

/*
 * Output "c" like out_char(), but don't flush when p_wd is set.
 */
    સ્થિર પૂર્ણાંક
out_char_nf(પૂર્ણાંક c)
{
    out_buf[out_pos++] = (unsigned)c;

    જો (out_pos >= OUT_SIZE)
	out_flush();
    પાછા (unsigned)c;
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
    શૂન્ય
out_str_nf(char_u *s)
{
    // avoid terminal strings being split up
    જો (out_pos > OUT_SIZE - MAX_ESC_SEQ_LEN)
	out_flush();

    માટે (char_u *p = s; *p != NUL; ++p)
	out_char_nf(*p);

    // For testing we write one string at a time.
    જો (p_wd)
	out_flush();
}

/*
 * A conditional-flushing out_str, mainly for visualbell.
 * Handles a delay internally, because termlib may not respect the delay or do
 * it at the wrong time.
 * Note: Only for terminal strings.
 */
    શૂન્ય
out_str_cf(char_u *s)
{
    જો (s == ખાલી || *s == NUL)
	પાછા;

#ifdef HAVE_TGETENT
    char_u *p;
#endif

#ifdef FEAT_GUI
    // Don't use tputs() when GUI is used, ncurses crashes.
    જો (gui.in_use)
    {
	out_str_nf(s);
	પાછા;
    }
#endif
    જો (out_pos > OUT_SIZE - MAX_ESC_SEQ_LEN)
	out_flush();
#ifdef HAVE_TGETENT
    માટે (p = s; *s; ++s)
    {
	// flush just before delay command
	જો (*s == '$' && *(s + 1) == '<')
	{
	    char_u save_c = *s;
	    પૂર્ણાંક duration = atoi((અક્ષર *)s + 2);

	    *s = NUL;
	    tputs((અક્ષર *)p, 1, TPUTSFUNCAST out_char_nf);
	    *s = save_c;
	    out_flush();
# ifdef ELAPSED_FUNC
	    // Only sleep here if we can limit this happening in
	    // vim_beep().
	    p = vim_strchr(s, '>');
	    જો (p == ખાલી || duration <= 0)
	    {
		// can't parse the time, don't sleep here
		p = s;
	    }
	    નહીંતર
	    {
		++p;
		do_sleep(duration, FALSE);
	    }
# નહીંતર
	    // Rely on the terminal library to sleep.
	    p = s;
# endif
	    તોડો;
	}
    }
    tputs((અક્ષર *)p, 1, TPUTSFUNCAST out_char_nf);
#નહીંતર
    જ્યારે (*s)
	out_char_nf(*s++);
#endif

    // For testing we write one string at a time.
    જો (p_wd)
	out_flush();
}

/*
 * out_str(s): Put a character string a byte at a time into the output buffer.
 * If HAVE_TGETENT is defined use tputs(), the termcap parser. (jw)
 * This should only be used for writing terminal codes, not for outputting
 * normal text (use functions like msg_puts() and screen_putchar() for that).
 */
    શૂન્ય
out_str(char_u *s)
{
    જો (s == ખાલી || *s == NUL)
	પાછા;

#ifdef FEAT_GUI
    // Don't use tputs() when GUI is used, ncurses crashes.
    જો (gui.in_use)
    {
	out_str_nf(s);
	પાછા;
    }
#endif
    // avoid terminal strings being split up
    જો (out_pos > OUT_SIZE - MAX_ESC_SEQ_LEN)
	out_flush();
#ifdef HAVE_TGETENT
    tputs((અક્ષર *)s, 1, TPUTSFUNCAST out_char_nf);
#નહીંતર
    જ્યારે (*s)
	out_char_nf(*s++);
#endif

    // For testing we write one string at a time.
    જો (p_wd)
	out_flush();
}

/*
 * cursor positioning using termcap parser. (jw)
 */
    શૂન્ય
term_windgoto(પૂર્ણાંક row, પૂર્ણાંક col)
{
    OUT_STR(tgoto((અક્ષર *)T_CM, col, row));
}

    શૂન્ય
term_cursor_જમણું(પૂર્ણાંક i)
{
    OUT_STR(tgoto((અક્ષર *)T_CRI, 0, i));
}

    શૂન્ય
term_append_લીટીઓ(પૂર્ણાંક લીટી_ગણતરી)
{
    OUT_STR(tgoto((અક્ષર *)T_CAL, 0, લીટી_ગણતરી));
}

    શૂન્ય
term_delete_લીટીઓ(પૂર્ણાંક લીટી_ગણતરી)
{
    OUT_STR(tgoto((અક્ષર *)T_CDL, 0, લીટી_ગણતરી));
}

#જો defined(UNIX) || defined(VMS)
    શૂન્ય
term_enable_mouse(પૂર્ણાંક enable)
{
    પૂર્ણાંક on = enable ? 1 : 0;
    OUT_STR(tgoto((અક્ષર *)T_CXM, 0, on));
}
#endif

#જો defined(HAVE_TGETENT)
    શૂન્ય
term_set_winpos(પૂર્ણાંક x, પૂર્ણાંક y)
{
    // Can't handle a negative value here
    જો (x < 0)
	x = 0;
    જો (y < 0)
	y = 0;
    OUT_STR(tgoto((અક્ષર *)T_CWP, y, x));
}

# જો defined(FEAT_TERMRESPONSE)
/*
 * Return TRUE if we can request the terminal for a response.
 */
    સ્થિર પૂર્ણાંક
can_get_termresponse(શૂન્ય)
{
    પાછા cur_tmode == TMODE_RAW
	    && termcap_active
#  ifdef UNIX
	    && (is_not_a_term() || (isatty(1) && isatty(read_cmd_fd)))
#  endif
	    && p_ek;
}

/*
 * Set "status" to STATUS_SENT.
 */
    સ્થિર શૂન્ય
termrequest_sent(termrequest_T *status)
{
    status->tr_progress = STATUS_SENT;
    status->tr_start = time(ખાલી);
}

/*
 * Return TRUE if any of the requests are in STATUS_SENT.
 */
    સ્થિર પૂર્ણાંક
termrequest_any_pending(શૂન્ય)
{
    પૂર્ણાંક	    i;
    time_t  now = time(ખાલી);

    માટે (i = 0; all_termrequests[i] != ખાલી; ++i)
    {
	જો (all_termrequests[i]->tr_progress == STATUS_SENT)
	{
	    જો (all_termrequests[i]->tr_start > 0 && now > 0
				    && all_termrequests[i]->tr_start + 2 < now)
		// Sent the request more than 2 seconds ago and didn't get a
		// response, assume it failed.
		all_termrequests[i]->tr_progress = STATUS_FAIL;
	    નહીંતર
		પાછા TRUE;
	}
    }
    પાછા FALSE;
}

સ્થિર પૂર્ણાંક winpos_x = -1;
સ્થિર પૂર્ણાંક winpos_y = -1;
સ્થિર પૂર્ણાંક did_request_winpos = 0;

#  જો defined(FEAT_EVAL) || defined(FEAT_TERMINAL)
/*
 * Try getting the Vim window position from the terminal.
 * Returns OK or FAIL.
 */
    પૂર્ણાંક
term_get_winpos(પૂર્ણાંક *x, પૂર્ણાંક *y, varnumber_T timeout)
{
    પૂર્ણાંક ગણતરી = 0;
    પૂર્ણાંક prev_winpos_x = winpos_x;
    પૂર્ણાંક prev_winpos_y = winpos_y;

    જો (*T_CGP == NUL || !can_get_termresponse())
	પાછા FAIL;
    winpos_x = -1;
    winpos_y = -1;
    ++did_request_winpos;
    termrequest_sent(&winpos_status);
    OUT_STR(T_CGP);
    out_flush();

    // Try reading the result for "timeout" msec.
    જ્યારે (ગણતરી++ <= timeout / 10 && !got_int)
    {
	(શૂન્ય)vpeekc_nomap();
	જો (winpos_x >= 0 && winpos_y >= 0)
	{
	    *x = winpos_x;
	    *y = winpos_y;
	    પાછા OK;
	}
	ui_delay(10L, FALSE);
    }
    // Do not reset "did_request_winpos", if we timed out the response might
    // still come later and we must consume it.

    winpos_x = prev_winpos_x;
    winpos_y = prev_winpos_y;
    જો (timeout < 10 && prev_winpos_y >= 0 && prev_winpos_x >= 0)
    {
	// Polling: return previous values if we have them.
	*x = winpos_x;
	*y = winpos_y;
	પાછા OK;
    }

    પાછા FALSE;
}
#  endif
# endif

    શૂન્ય
term_set_winsize(પૂર્ણાંક height, પૂર્ણાંક width)
{
    OUT_STR(tgoto((અક્ષર *)T_CWS, width, height));
}
#endif

    શૂન્ય
term_font(પૂર્ણાંક n)
{
    જો (*T_CFO)
    {
	અક્ષર buf[20];
	sprintf(buf, (અક્ષર *)T_CFO, 9 + n);
	OUT_STR(buf);
    }
}

    સ્થિર શૂન્ય
term_color(char_u *s, પૂર્ણાંક n)
{
    અક્ષર	buf[20];
    પૂર્ણાંક		i = *s == CSI ? 1 : 2;
		// index in s[] just after <Esc>[ or CSI

    // Special handling of 16 colors, because termcap can't handle it
    // Also accept "\e[3%dm" for TERMINFO, it is sometimes used
    // Also accept CSI instead of <Esc>[
    જો (n >= 8 && t_colors >= 16
	      && ((s[0] == ESC && s[1] == '[')
#જો defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
		  || (s[0] == ESC && s[1] == '|')
#endif
		  || (s[0] == CSI && (i = 1) == 1))
	      && s[i] != NUL
	      && (STRCMP(s + i + 1, "%p1%dm") == 0
		  || STRCMP(s + i + 1, "%dm") == 0)
	      && (s[i] == '3' || s[i] == '4'))
    {
#ifdef TERMINFO
	અક્ષર *format = "%s%s%%p1%%dm";
#નહીંતર
	અક્ષર *format = "%s%s%%dm";
#endif
	અક્ષર *lead = i == 2 ? (
#જો defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
		    s[1] == '|' ? "\033|" :
#endif
		    "\033[") : "\233";
	અક્ષર *tail = s[i] == '3' ? (n >= 16 ? "38;5;" : "9")
				 : (n >= 16 ? "48;5;" : "10");

	sprintf(buf, format, lead, tail);
	OUT_STR(tgoto(buf, 0, n >= 16 ? n : n - 8));
    }
    નહીંતર
	OUT_STR(tgoto((અક્ષર *)s, 0, n));
}

    શૂન્ય
term_fg_color(પૂર્ણાંક n)
{
    // Use "AF" termcap entry if present, "Sf" entry otherwise
    જો (*T_CAF)
	term_color(T_CAF, n);
    નહીંતર જો (*T_CSF)
	term_color(T_CSF, n);
}

    શૂન્ય
term_bg_color(પૂર્ણાંક n)
{
    // Use "AB" termcap entry if present, "Sb" entry otherwise
    જો (*T_CAB)
	term_color(T_CAB, n);
    નહીંતર જો (*T_CSB)
	term_color(T_CSB, n);
}

    શૂન્ય
term_ul_color(પૂર્ણાંક n)
{
    જો (*T_CAU)
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
term_bg_default(શૂન્ય)
{
#જો defined(MSWIN)
    // DOS console is nearly always black
    પાછા (char_u *)"dark";
#નહીંતર
    char_u	*p;

    જો (STRCMP(T_NAME, "linux") == 0
	    || STRCMP(T_NAME, "screen.linux") == 0
	    || STRNCMP(T_NAME, "cygwin", 6) == 0
	    || STRNCMP(T_NAME, "putty", 5) == 0
	    || ((p = mch_getenv((char_u *)"COLORFGBG")) != ખાલી
		&& (p = vim_strrchr(p, ';')) != ખાલી
		&& ((p[1] >= '0' && p[1] <= '6') || p[1] == '8')
		&& p[2] == NUL))
	પાછા (char_u *)"dark";
    પાછા (char_u *)"light";
#endif
}

#જો defined(FEAT_TERMGUICOLORS)

# define RED(rgb)   (((long_u)(rgb) >> 16) & 0xFF)
# define GREEN(rgb) (((long_u)(rgb) >>  8) & 0xFF)
# define BLUE(rgb)  (((long_u)(rgb)      ) & 0xFF)

    સ્થિર શૂન્ય
term_rgb_color(char_u *s, guicolor_T rgb)
{
# define MAX_COLOR_STR_LEN 100
    અક્ષર	buf[MAX_COLOR_STR_LEN];

    જો (*s == NUL)
	પાછા;
    vim_snprintf(buf, MAX_COLOR_STR_LEN,
				  (અક્ષર *)s, RED(rgb), GREEN(rgb), BLUE(rgb));
# ifdef FEAT_VTP
    જો (use_vtp() && (p_tgc || t_colors >= 256))
    {
	out_flush();
	buf[1] = '[';
	vtp_printf(buf);
    }
    નહીંતર
# endif
	OUT_STR(buf);
}

    શૂન્ય
term_fg_rgb_color(guicolor_T rgb)
{
    જો (rgb != INVALCOLOR)
	term_rgb_color(T_8F, rgb);
}

    શૂન્ય
term_bg_rgb_color(guicolor_T rgb)
{
    જો (rgb != INVALCOLOR)
	term_rgb_color(T_8B, rgb);
}

    શૂન્ય
term_ul_rgb_color(guicolor_T rgb)
{
# ifdef FEAT_TERMRESPONSE
    // If the user explicitly sets t_8u then use it.  Otherwise wait for
    // termresponse to be received, which is when t_8u would be set and a
    // redraw is needed if it was used.
    જો (!option_was_set((char_u *)"t_8u") && write_t_8u_state != OK)
	write_t_8u_state = MAYBE;
    નહીંતર
# endif
	term_rgb_color(T_8U, rgb);
}
#endif

#જો defined(UNIX) || defined(VMS) || defined(MACOS_X)
/*
 * Generic function to set window title, using t_ts and t_fs.
 */
    શૂન્ય
term_settitle(char_u *title)
{
    MAY_WANT_TO_LOG_THIS;

    // t_ts takes one argument: column in status line
    OUT_STR(tgoto((અક્ષર *)T_TS, 0, 0));	// set title start
    out_str_nf(title);
    out_str(T_FS);			// set title end
    out_flush();
}

/*
 * Tell the terminal to push (save) the title and/or icon, so that it can be
 * popped (restored) later.
 */
    શૂન્ય
term_ધકેલો_title(પૂર્ણાંક which)
{
    જો ((which & SAVE_RESTORE_TITLE) && T_CST != ખાલી && *T_CST != NUL)
    {
	OUT_STR(T_CST);
	out_flush();
    }

    જો ((which & SAVE_RESTORE_ICON) && T_SSI != ખાલી && *T_SSI != NUL)
    {
	OUT_STR(T_SSI);
	out_flush();
    }
}

/*
 * Tell the terminal to pop the title and/or icon.
 */
    શૂન્ય
term_કાઢો_title(પૂર્ણાંક which)
{
    જો ((which & SAVE_RESTORE_TITLE) && T_CRT != ખાલી && *T_CRT != NUL)
    {
	OUT_STR(T_CRT);
	out_flush();
    }

    જો ((which & SAVE_RESTORE_ICON) && T_SRI != ખાલી && *T_SRI != NUL)
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
    શૂન્ય
ttest(પૂર્ણાંક pairs)
{
    char_u *env_colors;

    check_options();		    // make sure no options are NULL

    /*
     * MUST have "cm": cursor motion.
     */
    જો (*T_CM == NUL)
	emsg(_(e_terminal_capability_cm_required));

    /*
     * if "cs" defined, use a scroll region, it's faster.
     */
    જો (*T_CS != NUL)
	scroll_region = TRUE;
    નહીંતર
	scroll_region = FALSE;

    જો (pairs)
    {
	/*
	 * optional pairs
	 */
	// TP goes to normal mode for TI (invert) and TB (bold)
	જો (*T_ME == NUL)
	    T_ME = T_MR = T_MD = T_MB = empty_option;
	જો (*T_SO == NUL || *T_SE == NUL)
	    T_SO = T_SE = empty_option;
	જો (*T_US == NUL || *T_UE == NUL)
	    T_US = T_UE = empty_option;
	જો (*T_CZH == NUL || *T_CZR == NUL)
	    T_CZH = T_CZR = empty_option;

	// T_VE is needed even though T_VI is not defined
	જો (*T_VE == NUL)
	    T_VI = empty_option;

	// if 'mr' or 'me' is not defined use 'so' and 'se'
	જો (*T_ME == NUL)
	{
	    T_ME = T_SE;
	    T_MR = T_SO;
	    T_MD = T_SO;
	}

	// if 'so' or 'se' is not defined use 'mr' and 'me'
	જો (*T_SO == NUL)
	{
	    T_SE = T_ME;
	    જો (*T_MR == NUL)
		T_SO = T_MD;
	    નહીંતર
		T_SO = T_MR;
	}

	// if 'ZH' or 'ZR' is not defined use 'mr' and 'me'
	જો (*T_CZH == NUL)
	{
	    T_CZR = T_ME;
	    જો (*T_MR == NUL)
		T_CZH = T_MD;
	    નહીંતર
		T_CZH = T_MR;
	}

	// "Sb" and "Sf" come in pairs
	જો (*T_CSB == NUL || *T_CSF == NUL)
	{
	    T_CSB = empty_option;
	    T_CSF = empty_option;
	}

	// "AB" and "AF" come in pairs
	જો (*T_CAB == NUL || *T_CAF == NUL)
	{
	    T_CAB = empty_option;
	    T_CAF = empty_option;
	}

	// if 'Sb' and 'AB' are not defined, reset "Co"
	જો (*T_CSB == NUL && *T_CAB == NUL)
	    free_one_termoption(T_CCO);

	// Set 'weirdinvert' according to value of 't_xs'
	p_wiv = (*T_XS != NUL);
    }
    need_gather = TRUE;

    // Set t_colors to the value of $COLORS or t_Co.  Ignore $COLORS in the
    // GUI.
    t_colors = atoi((અક્ષર *)T_CCO);
#ifdef FEAT_GUI
    જો (!gui.in_use)
#endif
    {
	env_colors = mch_getenv((char_u *)"COLORS");
	જો (env_colors != ખાલી && SAFE_isdigit(*env_colors))
	{
	    પૂર્ણાંક colors = atoi((અક્ષર *)env_colors);

	    જો (colors != t_colors)
		set_color_ગણતરી(colors);
	}
    }
    set_rgb_term_prop();
}

#જો defined(FEAT_GUI) && (defined(FEAT_MENU) || !defined(USE_ON_FLY_SCROLL))
/*
 * Represent the given long_u as individual bytes, with the most significant
 * byte first, and store them in dst.
 */
    શૂન્ય
add_long_to_buf(long_u val, char_u *dst)
{
    પૂર્ણાંક	    i;
    પૂર્ણાંક	    shift;

    માટે (i = 1; i <= (પૂર્ણાંક)કદ(long_u); i++)
    {
	shift = 8 * (કદ(long_u) - i);
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
    સ્થિર પૂર્ણાંક
get_long_from_buf(char_u *buf, long_u *val)
{
    પૂર્ણાંક	    len;
    char_u  bytes[કદ(long_u)];
    પૂર્ણાંક	    i;
    પૂર્ણાંક	    shift;

    *val = 0;
    len = get_bytes_from_buf(buf, bytes, (પૂર્ણાંક)કદ(long_u));
    જો (len == -1)
	પાછા -1;
    માટે (i = 0; i < (પૂર્ણાંક)કદ(long_u); i++)
    {
	shift = 8 * (કદ(long_u) - 1 - i);
	*val += (long_u)bytes[i] << shift;
    }
    પાછા len;
}
#endif

/*
 * Read the next num_bytes bytes from buf, and store them in bytes.  Assume
 * that buf has been through inchar().	Returns the actual number of bytes used
 * from buf (between num_bytes and num_bytes*2), or -1 if not enough bytes were
 * available.
 */
    પૂર્ણાંક
get_bytes_from_buf(char_u *buf, char_u *bytes, પૂર્ણાંક અંક_bytes)
{
    પૂર્ણાંક	    len = 0;
    પૂર્ણાંક	    i;
    char_u  c;

    માટે (i = 0; i < અંક_bytes; i++)
    {
	જો ((c = buf[len++]) == NUL)
	    પાછા -1;
	જો (c == K_SPECIAL)
	{
	    જો (buf[len] == NUL || buf[len + 1] == NUL)	    // cannot happen?
		પાછા -1;
	    જો (buf[len++] == (પૂર્ણાંક)KS_ZERO)
		c = NUL;
	    // else it should be KS_SPECIAL; when followed by KE_FILLER c is
	    // K_SPECIAL, or followed by KE_CSI and c must be CSI.
	    જો (buf[len++] == (પૂર્ણાંક)KE_CSI)
		c = CSI;
	}
	નહીંતર જો (c == CSI && buf[len] == KS_EXTRA
					       && buf[len + 1] == (પૂર્ણાંક)KE_CSI)
	    // CSI is stored as CSI KS_SPECIAL KE_CSI to avoid confusion with
	    // the start of a special key, see add_to_input_buf_csi().
	    len += 2;
	bytes[i] = c;
    }
    પાછા len;
}

/*
 * Check if the new shell size is valid, correct it if it's too small or way
 * too big.
 */
    શૂન્ય
check_shellsize(શૂન્ય)
{
    // need room for one window and command line
    જો (Rows < min_rows_for_all_tabpages())
	Rows = min_rows_for_all_tabpages();
    limit_screen_size();

    // make sure these values are not invalid
    જો (cmdline_row >= Rows)
	cmdline_row = Rows - 1;
    જો (સંદેશ_row >= Rows)
	સંદેશ_row = Rows - 1;
}

/*
 * Limit Rows and Columns to avoid an overflow in Rows * Columns.
 */
    શૂન્ય
limit_screen_size(શૂન્ય)
{
    જો (Columns < MIN_COLUMNS)
	Columns = MIN_COLUMNS;
    નહીંતર જો (Columns > 10000)
	Columns = 10000;
    જો (Rows > 1000)
	Rows = 1000;
}

/*
 * Invoked just before the screen structures are going to be (re)allocated.
 */
    શૂન્ય
win_new_shellsize(શૂન્ય)
{
    સ્થિર પૂર્ણાંક	old_Rows = 0;
    સ્થિર પૂર્ણાંક	old_Columns = 0;
    સ્થિર પૂર્ણાંક	old_coloff = 0;

    જો (old_Rows != Rows || old_Columns != COLUMNS_WITHOUT_TPL()
	    || old_coloff != TPL_LCOL())
	ui_new_shellsize();
    જો (old_Columns != COLUMNS_WITHOUT_TPL() || old_coloff != TPL_LCOL())
    {
	old_Columns = COLUMNS_WITHOUT_TPL();
	old_coloff = TPL_LCOL();

	shell_new_columns();
    }
    જો (old_Rows != Rows)
    {
	// If 'window' uses the whole screen, keep it using that.
	// Don't change it when set with "-w size" on the command line.
	જો (p_window == old_Rows - 1
		    || (old_Rows == 0 && !option_was_set((char_u *)"window")))
	    p_window = Rows - 1;
	old_Rows = Rows;
	shell_new_rows();	// update window sizes
    }
}

#જો defined(FEAT_IMAGE) || defined(FEAT_EVAL)
    શૂન્ય
update_cell_size(શૂન્ય)
{
# ifdef FEAT_GUI
    જો (gui.in_use)
    {
	cell_width = gui.char_width;
	cell_height = gui.char_height;
    }
    નહીંતર
# endif
    {
	સંરચના cellsize cell_sz;

	mch_calc_cell_size(&cell_sz);
	જો (cell_sz.cs_xpixel <= 0 || cell_sz.cs_ypixel <= 0)
	{
	    cell_width = 8;
	    cell_height = 16;
	}
	નહીંતર
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
    શૂન્ય
shell_resized(શૂન્ય)
{
#જો defined(FEAT_IMAGE) || defined(FEAT_EVAL)
    update_cell_size();
#endif
    set_shellsize(0, 0, FALSE);
}

/*
 * Check if the shell size changed.  Handle a resize.
 * When the size didn't change, nothing happens.
 */
    શૂન્ય
shell_resized_check(શૂન્ય)
{
    પૂર્ણાંક		old_Rows = Rows;
    પૂર્ણાંક		old_Columns = Columns;

    જો (exiting
#ifdef FEAT_GUI
	    // Do not get the size when executing a shell command during
	    // startup.
	    || gui.starting
#endif
	    )
	પાછા;

    (શૂન્ય)ui_get_shellsize();
    check_shellsize();
    જો (old_Rows != Rows || old_Columns != Columns)
	shell_resized();
}

/*
 * Set size of the Vim shell.
 * If 'mustset' is TRUE, we must set Rows and Columns, do not get the real
 * window size (this is used for the :win command).
 * If 'mustset' is FALSE, we may try to get the real window size and if
 * it fails use 'width' and 'height'.
 */
    સ્થિર શૂન્ય
set_shellsize_inner(પૂર્ણાંક width, પૂર્ણાંક height, પૂર્ણાંક mustset)
{
    જો (updating_screen)
	// resizing while in update_screen() may cause a crash
	પાછા;

    // curwin->w_buffer can be NULL when we are closing a window and the
    // buffer (or window) has already been closed and removing a scrollbar
    // causes a resize event. Don't resize then, it will happen after entering
    // another buffer.
    જો (curwin->w_buffer == ખાલી || curwin->w_લીટીઓ == ખાલી)
	પાછા;

#ifdef AMIGA
    out_flush();	    // must do this before mch_get_shellsize() for
			    // some obscure reason
#endif

    જો (mustset || (ui_get_shellsize() == FAIL && height != 0))
    {
	Rows = height;
	Columns = width;
	check_shellsize();
	ui_set_shellsize(mustset);
    }
    નહીંતર
	check_shellsize();

    // The window layout used to be adjusted here, but it now happens in
    // screenalloc() (also invoked from screenclear()).  That is because the
    // "busy" check above may skip this, but not screenalloc().

    જો (State != MODE_ASKMORE && State != MODE_EXTERNCMD
						      && State != MODE_CONFIRM)
	screenclear();
    નહીંતર
	screen_start();	    // don't know where cursor is now

    જો (starting != NO_SCREEN)
    {
	maketitle();

	changed_લીટી_abv_curs();
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
	જો (State == MODE_ASKMORE || State == MODE_EXTERNCMD
				     || State == MODE_CONFIRM || exmode_active)
	{
	    screenalloc(FALSE);
	    repeat_message();
	}
	નહીંતર
	{
	    જો (curwin->w_p_scb)
		do_check_scrollbind(TRUE);
	    જો (State & MODE_CMDLINE)
	    {
		update_screen(UPD_NOT_VALID);
		redrawcmdline();
	    }
	    નહીંતર
	    {
		update_topline();
		જો (pum_visible())
		{
		    redraw_later(UPD_NOT_VALID);
		    ins_compl_show_pum();
		}
		update_screen(UPD_NOT_VALID);
		જો (redrawing())
		    setcursor();
	    }
	}
	cursor_on();	    // redrawing may have switched it off
    }
    out_flush();
}

    શૂન્ય
set_shellsize(પૂર્ણાંક width, પૂર્ણાંક height, પૂર્ણાંક mustset)
{
    સ્થિર પૂર્ણાંક	busy = FALSE;
    સ્થિર પૂર્ણાંક	do_run = FALSE;

    જો (width < 0 || height < 0)    // just checking...
	પાછા;

    જો (State == MODE_HITRETURN || State == MODE_SETWSIZE)
    {
	// postpone the resizing
	State = MODE_SETWSIZE;
	પાછા;
    }

    // Avoid recursiveness.  This can happen when setting the window size
    // causes another window-changed signal or when two SIGWINCH signals come
    // very close together.  There needs to be another run then after the
    // current one is done to pick up the latest size.
    do_run = TRUE;
    જો (busy)
	પાછા;

    જ્યારે (do_run)
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
    શૂન્ય
out_str_t_TE(શૂન્ય)
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
    જો (modify_otherkeys_state == MOKS_ENABLED
	    || modify_otherkeys_state == MOKS_DISABLED)
	modify_otherkeys_state = MOKS_DISABLED;
    નહીંતર જો (modify_otherkeys_state != MOKS_INITIAL)
	modify_otherkeys_state = MOKS_AFTER_T_TE;

    // When the kitty keyboard protocol is enabled we expect t_TE to disable
    // it.  Remembering that it was detected to be enabled is useful in some
    // situations.
    // The following t_TI is expected to request the state and then
    // kitty_protocol_state will be set again.
    જો (kitty_protocol_state == KKPS_ENABLED
	    || kitty_protocol_state == KKPS_DISABLED)
	kitty_protocol_state = KKPS_DISABLED;
    નહીંતર
	kitty_protocol_state = KKPS_AFTER_T_TE;
}

સ્થિર પૂર્ણાંક send_t_RK = FALSE;

/*
 * Output T_TI and setup for what follows.
 */
    શૂન્ય
out_str_t_TI(શૂન્ય)
{
    out_str(T_CTI);

    // Send t_RK when there is no more work to do.
    send_t_RK = TRUE;
}

/*
 * Output T_BE, but only when t_PS and t_PE are set.
 */
    શૂન્ય
out_str_t_BE(શૂન્ય)
{
    char_u *p;

    જો (T_BE == ખાલી || *T_BE == NUL
	    || (p = find_termcode((char_u *)"PS")) == ખાલી || *p == NUL
	    || (p = find_termcode((char_u *)"PE")) == ખાલી || *p == NUL)
	પાછા;
    out_str(T_BE);
}

/*
 * If t_TI was recently sent and there is no typeahead or work to do, now send
 * t_RK.  This is postponed to avoid the response arriving in a shell command
 * or after Vim exits.
 */
    શૂન્ય
may_send_t_RK(શૂન્ય)
{
    જો (send_t_RK
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
    શૂન્ય
settmode(tmode_T tmode)
{
#ifdef FEAT_GUI
    // don't set the term where gvim was started to any mode
    જો (gui.in_use)
	પાછા;
#endif

    જો (!full_screen)
	પાછા;

    /*
     * When returning after calling a shell cur_tmode is TMODE_UNKNOWN,
     * set the terminal to raw mode, even though we think it already is,
     * because the shell program may have reset the terminal mode.
     * When we think the terminal is normal, don't try to set it to
     * normal again, because that causes problems (logout!) on some
     * machines.
     */
    જો (tmode != cur_tmode)
    {
#ifdef FEAT_TERMRESPONSE
# ifdef FEAT_GUI
	જો (!gui.in_use && !gui.starting)
# endif
	{
	    // May need to check for T_CRV response and termcodes, it
	    // doesn't work in Cooked mode, an external program may get
	    // them.
	    જો (tmode != TMODE_RAW && termrequest_any_pending())
		(શૂન્ય)vpeekc_nomap();
	    check_for_codes_from_term();
	}
#endif
	જો (tmode != TMODE_RAW)
	    mch_setmouse(FALSE);	// switch mouse off

	// Disable bracketed paste and modifyOtherKeys in cooked mode.
	// Avoid doing this too often, on some terminals the codes are not
	// handled properly.
	જો (termcap_active && tmode != TMODE_SLEEP
		&& cur_tmode != TMODE_SLEEP)
	{
	    MAY_WANT_TO_LOG_THIS;

	    જો (tmode != TMODE_RAW)
	    {
		out_str(T_BD);	// disable bracketed paste mode
		out_str_t_TE();	// possibly disables modifyOtherKeys
	    }
	    નહીંતર
	    {
		out_str_t_BE();	// enable bracketed paste mode (should
				// be before mch_settmode().
		out_str_t_TI();	// possibly enables modifyOtherKeys
	    }
	}
	out_flush();
	mch_settmode(tmode);	// machine specific function
	cur_tmode = tmode;
	જો (tmode == TMODE_RAW)
	    setmouse();		// may switch mouse on
	out_flush();
    }
#ifdef FEAT_TERMRESPONSE
    may_req_termresponse();
#endif
}

    શૂન્ય
starttermcap(શૂન્ય)
{
    જો (!full_screen || termcap_active)
	પાછા;

    MAY_WANT_TO_LOG_THIS;

    out_str(T_TI);			// start termcap mode
    out_str_t_TI();			// start "raw" mode
    out_str(T_KS);			// start "keypad transmit" mode
    out_str_t_BE();			// enable bracketed paste mode

#જો defined(UNIX) || defined(VMS)
    // Enable xterm's focus reporting mode when 'esckeys' is set.
    જો (p_ek && *T_FE != NUL)
	out_str(T_FE);
#endif

    out_flush();
    termcap_active = TRUE;
    screen_start();			// don't know where cursor is now
#ifdef FEAT_TERMRESPONSE
# ifdef FEAT_GUI
    જો (!gui.in_use && !gui.starting)
# endif
    {
	may_req_termresponse();
	// Immediately check for a response.  If t_Co changes, we don't
	// want to redraw with wrong colors first.
	જો (crv_status.tr_progress == STATUS_SENT)
	    check_for_codes_from_term();
    }
#endif
}

    શૂન્ય
stoptermcap(શૂન્ય)
{
    screen_stop_highlight();
    reset_cterm_colors();

    જો (!termcap_active)
	પાછા;

#ifdef FEAT_TERMRESPONSE
# ifdef FEAT_GUI
    જો (!gui.in_use && !gui.starting)
# endif
    {
	// May need to discard T_CRV, T_U7 or T_RBG response.
	જો (termrequest_any_pending())
	{
# ifdef UNIX
	    // Give the terminal a chance to respond.
	    mch_delay(100L, 0);
# endif
# ifdef TCIFLUSH
	    // Discard data received but not read.
	    જો (exiting)
		tcflush(fileno(stdin), TCIFLUSH);
# endif
	}
	// Check for termcodes first, otherwise an external program may
	// get them.
	check_for_codes_from_term();
    }
#endif
    MAY_WANT_TO_LOG_THIS;

#જો defined(UNIX) || defined(VMS)
    // Disable xterm's focus reporting mode if 'esckeys' is set.
    જો (p_ek && *T_FD != NUL)
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
    જો (*T_TE != NUL && (kitty_protocol_state == KKPS_ENABLED
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

#જો defined(FEAT_TERMRESPONSE)
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
    શૂન્ય
may_req_termresponse(શૂન્ય)
{
    જો (crv_status.tr_progress == STATUS_GET
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
	(શૂન્ય)vpeekc_nomap();
    }
}

/*
 * Send sequences to the terminal and check with t_u7 how the cursor moves, to
 * find out properties of the terminal.
 * Note that this goes out before T_CRV, so that the result can be used when
 * the termresponse arrives.
 */
    શૂન્ય
check_terminal_behavior(શૂન્ય)
{
    પૂર્ણાંક	    did_send = FALSE;

    જો (!can_get_termresponse() || starting != 0 || *T_U7 == NUL)
	પાછા;

    જો (u7_status.tr_progress == STATUS_GET
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
	લીટી_was_clobbered(1);
    }

    જો (xcc_status.tr_progress == STATUS_GET && Rows > 2)
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
	લીટી_was_clobbered(2);
    }

    જો (did_send)
    {
	term_windgoto(0, 0);

	// Need to reset the known cursor position.
	screen_start();

	// check for the characters now, otherwise they might be eaten by
	// get_keystroke()
	out_flush();
	(શૂન્ય)vpeekc_nomap();
    }
}

/*
 * Similar to requesting the version string: Request the terminal background
 * color when it is the right moment.
 */
    શૂન્ય
may_req_bg_color(શૂન્ય)
{
    જો (can_get_termresponse() && starting == 0)
    {
	પૂર્ણાંક didit = FALSE;

# ifdef FEAT_TERMINAL
	// Only request foreground if t_RF is set.
	જો (rfg_status.tr_progress == STATUS_GET && *T_RFG != NUL)
	{
	    MAY_WANT_TO_LOG_THIS;
	    LOG_TR1("Sending FG request");
	    out_str(T_RFG);
	    termrequest_sent(&rfg_status);
	    didit = TRUE;
	}
# endif

	// Only request background if t_RB is set.
	જો (rbg_status.tr_progress == STATUS_GET && *T_RBG != NUL)
	{
	    MAY_WANT_TO_LOG_THIS;
	    LOG_TR1("Sending BG request");
	    out_str(T_RBG);
	    termrequest_sent(&rbg_status);
	    didit = TRUE;
	}

	જો (didit)
	{
	    // check for the characters now, otherwise they might be eaten by
	    // get_keystroke()
	    out_flush();
	    (શૂન્ય)vpeekc_nomap();
	}
    }
}

#endif

/*
 * Return TRUE when saving and restoring the screen.
 */
    પૂર્ણાંક
swapping_screen(શૂન્ય)
{
    પાછા (full_screen && *T_TI != NUL);
}

/*
 * By outputting the 'cursor very visible' termcap code, for some windowed
 * terminals this makes the screen scrolled to the correct position.
 * Used when starting Vim or returning from a shell.
 */
    શૂન્ય
scroll_start(શૂન્ય)
{
    જો (*T_VS == NUL || *T_CVS == NUL)
	પાછા;

    MAY_WANT_TO_LOG_THIS;
    out_str(T_VS);
    out_str(T_CVS);
    screen_start();		// don't know where cursor is now
}

// True if cursor is not visible
સ્થિર પૂર્ણાંક cursor_is_off = FALSE;

// True if cursor is not visible due to an ongoing cursor-less sleep
સ્થિર પૂર્ણાંક cursor_is_asleep = FALSE;

/*
 * Enable the cursor without checking if it's already enabled.
 */
    શૂન્ય
cursor_on_force(શૂન્ય)
{
    out_str(T_VE);
    cursor_is_off = FALSE;
    cursor_is_asleep = FALSE;
}

/*
 * Enable the cursor if it's currently off.
 */
    શૂન્ય
cursor_on(શૂન્ય)
{
    જો (cursor_is_off && !cursor_is_asleep)
	cursor_on_force();
}

/*
 * Disable the cursor.
 */
    શૂન્ય
cursor_off(શૂન્ય)
{
    જો (full_screen && !cursor_is_off)
    {
	out_str(T_VI);	    // disable cursor
	cursor_is_off = TRUE;
    }
}

#ifdef FEAT_GUI
/*
 * Check whether the cursor is invisible due to an ongoing cursor-less sleep
 */
    પૂર્ણાંક
cursor_is_sleeping(શૂન્ય)
{
    પાછા cursor_is_asleep;
}
#endif

/*
 * Disable the cursor and mark it disabled by cursor-less sleep
 */
    શૂન્ય
cursor_sleep(શૂન્ય)
{
    cursor_is_asleep = TRUE;
    cursor_off();
}

/*
 * Enable the cursor and mark it not disabled by cursor-less sleep
 */
    શૂન્ય
cursor_unsleep(શૂન્ય)
{
    cursor_is_asleep = FALSE;
    cursor_on();
}

#જો defined(CURSOR_SHAPE)
/*
 * Set cursor shape to match Insert or Replace mode.
 */
    શૂન્ય
term_cursor_mode(પૂર્ણાંક forced)
{
    સ્થિર પૂર્ણાંક showing_mode = -1;
    char_u *p;

    // Only do something when redrawing the screen and we can restore the
    // mode.
    જો (!full_screen || *T_CEI == NUL)
    {
# ifdef FEAT_TERMRESPONSE
	જો (forced && initial_cursor_shape > 0)
	    // Restore to initial values.
	    term_cursor_shape(initial_cursor_shape, initial_cursor_blink);
# endif
	પાછા;
    }

    જો ((State & MODE_REPLACE) == MODE_REPLACE)
    {
	જો (forced || showing_mode != MODE_REPLACE)
	{
	    જો (*T_CSR != NUL)
		p = T_CSR;	// Replace mode cursor
	    નહીંતર
		p = T_CSI;	// fall back to Insert mode cursor
	    જો (*p != NUL)
	    {
		out_str(p);
		showing_mode = MODE_REPLACE;
	    }
	}
    }
    નહીંતર જો (State & MODE_ઉમેરો)
    {
	જો ((forced || showing_mode != MODE_ઉમેરો) && *T_CSI != NUL)
	{
	    out_str(T_CSI);	    // Insert mode cursor
	    showing_mode = MODE_ઉમેરો;
	}
    }
    નહીંતર જો (forced || showing_mode != MODE_NORMAL)
    {
	out_str(T_CEI);		    // non-Insert mode cursor
	showing_mode = MODE_NORMAL;
    }
}

# જો defined(FEAT_TERMINAL)
    શૂન્ય
term_cursor_color(char_u *color)
{
    જો (*T_CSC == NUL)
	પાછા;

    out_str(T_CSC);		// set cursor color start
    out_str_nf(color);
    out_str(T_CEC);		// set cursor color end
    out_flush();
}
# endif

    પૂર્ણાંક
blink_state_is_inverted(શૂન્ય)
{
# ifdef FEAT_TERMRESPONSE
    પાછા rbm_status.tr_progress == STATUS_GOT
	&& rcs_status.tr_progress == STATUS_GOT
		&& initial_cursor_blink != initial_cursor_shape_blink;
# નહીંતર
    પાછા FALSE;
# endif
}

/*
 * "shape": 1 = block, 2 = underline, 3 = vertical bar
 */
    શૂન્ય
term_cursor_shape(પૂર્ણાંક shape, પૂર્ણાંક blink)
{
    જો (*T_CSH != NUL)
    {
	OUT_STR(tgoto((અક્ષર *)T_CSH, 0, shape * 2 - blink));
	out_flush();
    }
    નહીંતર
    {
	પૂર્ણાંક do_blink = blink;

	// t_SH is empty: try setting just the blink state.
	// The blink flags are XORed together, if the initial blinking from
	// style and shape differs, we need to invert the flag here.
	જો (blink_state_is_inverted())
	    do_blink = !blink;

	જો (do_blink && *T_VS != NUL)
	{
	    out_str(T_VS);
	    out_flush();
	}
	નહીંતર જો (!do_blink && *T_CVS != NUL)
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
    શૂન્ય
scroll_region_set(win_T *wp, પૂર્ણાંક off)
{
    OUT_STR(tgoto((અક્ષર *)T_CS, W_WINROW(wp) + wp->w_height - 1,
							 W_WINROW(wp) + off));
    જો (*T_CSV != NUL && wp->w_width != Columns)
	OUT_STR(tgoto((અક્ષર *)T_CSV, wp->w_wincol + wp->w_width - 1,
							       wp->w_wincol));
    screen_start();		    // don't know where cursor is now
}

/*
 * Reset scrolling region to the whole screen.
 */
    શૂન્ય
scroll_region_reset(શૂન્ય)
{
    OUT_STR(tgoto((અક્ષર *)T_CS, (પૂર્ણાંક)Rows - 1, 0));
    જો (*T_CSV != NUL)
	OUT_STR(tgoto((અક્ષર *)T_CSV, (પૂર્ણાંક)Columns - 1, 0));
    screen_start();		    // don't know where cursor is now
}


/*
 * List of terminal codes that are currently recognized.
 */

સ્થિર સંરચના termcode
{
    char_u  name[2];	    // termcap name of entry
    char_u  *code;	    // terminal code (in allocated memory)
    પૂર્ણાંક	    len;	    // STRLEN(code)
    પૂર્ણાંક	    modlen;	    // length of part before ";*~".
} *termcodes = ખાલી;

સ્થિર પૂર્ણાંક  tc_max_len = 0; // number of entries that termcodes[] can hold
સ્થિર પૂર્ણાંક  tc_len = 0;	    // current number of entries in termcodes[]

સ્થિર પૂર્ણાંક termcode_star(char_u *code, પૂર્ણાંક len);

    શૂન્ય
clear_termcodes(શૂન્ય)
{
    જ્યારે (tc_len > 0)
	vim_free(termcodes[--tc_len].code);
    VIM_CLEAR(termcodes);
    tc_max_len = 0;

#ifdef HAVE_TGETENT
    BC = (અક્ષર *)empty_option;
    UP = (અક્ષર *)empty_option;
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
    સ્થિર શૂન્ય
adjust_modlen(પૂર્ણાંક idx)
{
    termcodes[idx].modlen = 0;
    પૂર્ણાંક j = termcode_star(termcodes[idx].code, termcodes[idx].len);
    જો (j <= 0)
	પાછા;

    termcodes[idx].modlen = termcodes[idx].len - 1 - j;
    // For "CSI[@;X" the "@" is not included in "modlen".
    જો (termcodes[idx].code[termcodes[idx].modlen - 1] == '@')
	--termcodes[idx].modlen;
}

/*
 * Add a new entry for "name[2]" to the list of terminal codes.
 * Note that "name" may not have a terminating NUL.
 * The list is kept alphabetical for ":set termcap"
 * "flags" is TRUE when replacing 7-bit by 8-bit controls is desired.
 * "flags" can also be ATC_FROM_TERM for got_code_from_term().
 */
    શૂન્ય
add_termcode(char_u *name, char_u *string, પૂર્ણાંક flags)
{
    સંરચના termcode *new_tc;
    પૂર્ણાંક		    i, j;
    char_u	    *s;
    પૂર્ણાંક		    len;
#ifdef FEAT_EVAL
    અક્ષર	    *action = "Setting";
#endif

    જો (string == ખાલી || *string == NUL)
    {
	del_termcode(name);
	પાછા;
    }

#જો defined(MSWIN) && !defined(FEAT_GUI)
    s = vim_strnsave(string, STRLEN(string) + 1);
#નહીંતર
# ifdef VIMDLL
    જો (!gui.in_use)
	s = vim_strnsave(string, STRLEN(string) + 1);
    નહીંતર
# endif
	s = vim_strsave(string);
#endif
    જો (s == ખાલી)
	પાછા;

    // Change leading <Esc>[ to CSI, change <Esc>O to <M-O>.
    જો (flags != 0 && flags != ATC_FROM_TERM && term_7to8bit(string) != 0)
    {
	STRMOVE(s, s + 1);
	s[0] = term_7to8bit(string);
    }

#જો defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
# ifdef VIMDLL
    જો (!gui.in_use)
# endif
    {
	જો (s[0] == K_NUL)
	{
	    STRMOVE(s + 1, s);
	    s[1] = 3;
	}
    }
#endif

    len = (પૂર્ણાંક)STRLEN(s);

    need_gather = TRUE;		// need to fill termleader[]

    /*
     * need to make space for more entries
     */
    જો (tc_len == tc_max_len)
    {
	tc_max_len += 20;
	new_tc = ALLOC_MULT(સંરચના termcode, tc_max_len);
	જો (new_tc == ખાલી)
	{
	    tc_max_len -= 20;
	    vim_free(s);
	    પાછા;
	}
	માટે (i = 0; i < tc_len; ++i)
	    new_tc[i] = termcodes[i];
	vim_free(termcodes);
	termcodes = new_tc;
    }

    /*
     * Look for existing entry with the same name, it is replaced.
     * Look for an existing entry that is alphabetical higher, the new entry
     * is inserted in front of it.
     */
    માટે (i = 0; i < tc_len; ++i)
    {
	જો (termcodes[i].name[0] < name[0])
	    ચાલુ;
	જો (termcodes[i].name[0] == name[0])
	{
	    જો (termcodes[i].name[1] < name[1])
		ચાલુ;
	    /*
	     * Exact match: May replace old code.
	     */
	    જો (termcodes[i].name[1] == name[1])
	    {
		જો (flags == ATC_FROM_TERM && (j = termcode_star(
				    termcodes[i].code, termcodes[i].len)) > 0)
		{
		    // Don't replace ESC[123;*X or ESC O*X with another when
		    // invoked from got_code_from_term().
		    જો (len == termcodes[i].len - j
			    && STRNCMP(s, termcodes[i].code, len - 1) == 0
			    && s[len - 1]
				   == termcodes[i].code[termcodes[i].len - 1])
		    {
			// They are equal but for the ";*": don't add it.
#ifdef FEAT_EVAL
			ch_log(ખાલી, "Termcap entry %c%c did not change",
							     name[0], name[1]);
#endif
			vim_free(s);
			પાછા;
		    }
		}
		નહીંતર
		{
		    // Replace old code.
#ifdef FEAT_EVAL
		    ch_log(ખાલી, "Termcap entry %c%c was: %s",
					  name[0], name[1], termcodes[i].code);
#endif
		    vim_free(termcodes[i].code);
		    --tc_len;
		    તોડો;
		}
	    }
	}
	/*
	 * Found alphabetical larger entry, move rest to insert new entry
	 */
#ifdef FEAT_EVAL
	action = "Adding";
#endif
	માટે (j = tc_len; j > i; --j)
	    termcodes[j] = termcodes[j - 1];
	તોડો;
    }

#ifdef FEAT_EVAL
    ch_log(ખાલી, "%s termcap entry %c%c to %s", action, name[0], name[1], s);
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
    સ્થિર શૂન્ય
accept_modifiers_for_function_ચાવીઓ(શૂન્ય)
{
    regmatch_T regmatch;
    CLEAR_FIELD(regmatch);
    regmatch.rm_ic = TRUE;
    regmatch.regprog = vim_regcomp((char_u *)"^\033[\\d\\+\\~$", RE_MAGIC);

    માટે (પૂર્ણાંક i = 0; i < tc_len; ++i)
    {
	જો (regmatch.regprog == ખાલી)
	    પાછા;

	// skip PasteStart and PasteEnd
	જો (termcodes[i].name[0] == 'P'
		&& (termcodes[i].name[1] == 'S' || termcodes[i].name[1] == 'E'))
	    ચાલુ;

	char_u *s = termcodes[i].code;
	જો (s != ખાલી && vim_regexec(&regmatch, s, (colnr_T)0))
	{
	    size_t len = STRLEN(s);
	    char_u *ns = alloc(len + 3);
	    જો (ns != ખાલી)
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
    સ્થિર પૂર્ણાંક
termcode_star(char_u *code, પૂર્ણાંક len)
{
    // Shortest is <M-O>*X.  With ; shortest is <CSI>@;*X
    જો (len >= 3 && code[len - 2] == '*')
    {
	જો (len >= 5 && code[len - 3] == ';')
	    પાછા 2;
	નહીંતર
	    પાછા 1;
    }
    પાછા 0;
}

    char_u  *
find_termcode(char_u *name)
{
    પૂર્ણાંક	    i;

    માટે (i = 0; i < tc_len; ++i)
	જો (termcodes[i].name[0] == name[0] && termcodes[i].name[1] == name[1])
	    પાછા termcodes[i].code;
    પાછા ખાલી;
}

    char_u *
get_termcode(પૂર્ણાંક i)
{
    જો (i >= tc_len)
	પાછા ખાલી;
    પાછા &termcodes[i].name[0];
}

/*
 * Returns the length of the terminal code at index 'idx'.
 */
    પૂર્ણાંક
get_termcode_len(પૂર્ણાંક idx)
{
    પાછા termcodes[idx].len;
}

    શૂન્ય
del_termcode(char_u *name)
{
    પૂર્ણાંક	    i;

    જો (termcodes == ખાલી)	// nothing there yet
	પાછા;

    need_gather = TRUE;		// need to fill termleader[]

    માટે (i = 0; i < tc_len; ++i)
	જો (termcodes[i].name[0] == name[0] && termcodes[i].name[1] == name[1])
	{
	    del_termcode_idx(i);
	    પાછા;
	}
    // not found. Give error message?
}

    સ્થિર શૂન્ય
del_termcode_idx(પૂર્ણાંક idx)
{
    પૂર્ણાંક		i;

    vim_free(termcodes[idx].code);
    --tc_len;
    માટે (i = idx; i < tc_len; ++i)
	termcodes[i] = termcodes[i + 1];
}

/*
 * Called when detected that the terminal sends 8-bit codes.
 * Convert all 7-bit codes to their 8-bit equivalent.
 */
    સ્થિર શૂન્ય
switch_to_8bit(શૂન્ય)
{
    પૂર્ણાંક		i;
    પૂર્ણાંક		c;

    // Only need to do something when not already using 8-bit codes.
    જો (!term_is_8bit(T_NAME))
    {
	માટે (i = 0; i < tc_len; ++i)
	{
	    c = term_7to8bit(termcodes[i].code);
	    જો (c != 0)
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
સ્થિર linenr_T orig_topline = 0;
# ifdef FEAT_DIFF
સ્થિર પૂર્ણાંક orig_topfill = 0;
# endif
#endif
#જો defined(CHECK_DOUBLE_CLICK)
/*
 * Checking for double-clicks ourselves.
 * "orig_topline" is used to avoid detecting a double-click when the window
 * contents scrolled (e.g., when 'scrolloff' is non-zero).
 */
/*
 * Set orig_topline.  Used when jumping to another window, so that a double
 * click still works.
 */
    શૂન્ય
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
    પૂર્ણાંક
is_mouse_topline(win_T *wp)
{
    પાછા orig_topline == wp->w_topline
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
    પૂર્ણાંક
put_string_in_typebuf(
	પૂર્ણાંક	offset,
	પૂર્ણાંક	slen,
	char_u	*string,
	પૂર્ણાંક	new_slen,
	char_u	*buf,
	પૂર્ણાંક	bufsize,
	પૂર્ણાંક	*buflen)
{
    પૂર્ણાંક		extra = new_slen - slen;

    string[new_slen] = NUL;
    જો (buf == ખાલી)
    {
	જો (extra < 0)
	    // remove matched chars, taking care of noremap
	    del_typebuf(-extra, offset);
	નહીંતર જો (extra > 0)
	    // insert the extra space we need
	    જો (ins_typebuf(string + slen, REMAP_YES, offset, FALSE, FALSE)
								       == FAIL)
		પાછા FAIL;

	// Careful: del_typebuf() and ins_typebuf() may have reallocated
	// typebuf.tb_buf[]!
	mch_memmove(typebuf.tb_buf + typebuf.tb_off + offset, string,
							     (size_t)new_slen);
    }
    નહીંતર
    {
	જો (extra < 0)
	    // remove matched characters
	    mch_memmove(buf + offset, buf + offset - extra,
					   (size_t)(*buflen + offset + extra));
	નહીંતર જો (extra > 0)
	{
	    // Insert the extra space we need.  If there is insufficient
	    // space return -1.
	    જો (*buflen + extra + new_slen >= bufsize)
		પાછા FAIL;
	    mch_memmove(buf + offset + extra, buf + offset,
						   (size_t)(*buflen - offset));
	}
	mch_memmove(buf + offset, string, (size_t)new_slen);
	*buflen = *buflen + extra + new_slen;
    }
    પાછા OK;
}

/*
 * Decode a modifier number as xterm provides it into MOD_MASK bits.
 */
    પૂર્ણાંક
decode_modifiers(પૂર્ણાંક n)
{
    પૂર્ણાંક	    code = n - 1;
    પૂર્ણાંક	    modifiers = 0;

    જો (code & 1)
	modifiers |= MOD_MASK_SHIFT;
    જો (code & 2)
	modifiers |= MOD_MASK_ALT;
    જો (code & 4)
	modifiers |= MOD_MASK_CTRL;
    જો (code & 8)
	modifiers |= MOD_MASK_META;
    // Any further modifiers are silently dropped.

    પાછા modifiers;
}

    સ્થિર પૂર્ણાંક
modifiers2keycode(પૂર્ણાંક modifiers, પૂર્ણાંક *ચાવી, char_u *string)
{
    પૂર્ણાંક new_slen = 0;

    જો (modifiers == 0)
	પાછા 0;

    // Some keys have the modifier included.  Need to handle that here to
    // make mappings work.  This may result in a special key, such as
    // K_S_TAB.
    *ચાવી = simplify_ચાવી(*ચાવી, &modifiers);
    જો (modifiers != 0)
    {
	string[new_slen++] = K_SPECIAL;
	string[new_slen++] = (પૂર્ણાંક)KS_MODIFIER;
	string[new_slen++] = modifiers;
    }
    પાછા new_slen;
}

/*
 * Handle a cursor position report.
 */
    સ્થિર શૂન્ય
handle_u7_response(પૂર્ણાંક *arg, char_u *tp UNUSED, પૂર્ણાંક csi_len UNUSED)
{
    જો (arg[0] == 2 && arg[1] >= 2)
    {
	અક્ષર *aw = ખાલી;

	LOG_TRN("Received U7 status: %s", tp);
	u7_status.tr_progress = STATUS_GOT;
	did_cursorhold = TRUE;
	જો (arg[1] == 2)
	    aw = "single";
	નહીંતર જો (arg[1] == 3)
	    aw = "double";
	જો (aw != ખાલી && STRCMP(aw, p_ambw) != 0)
	{
	    // Setting the option causes a screen redraw. Do
	    // that right away if possible, keeping any
	    // messages.
	    set_option_કિંમત_give_err((char_u *)"ambw", 0L, (char_u *)aw, 0);
#ifdef DEBUG_TERMRESPONSE
	    {
		પૂર્ણાંક r = redraw_asap(UPD_CLEAR);

		LOG_TRN("set 'ambiwidth', redraw_asap(): %d", r);
	    }
#નહીંતર
	    redraw_asap(UPD_CLEAR);
#endif
#ifdef FEAT_EVAL
	    set_vim_var_string(VV_TERMU7RESP, tp, csi_len);
#endif
	    apply_autocmds(EVENT_TERMRESPONSEALL,
					(char_u *)"ambiguouswidth", ખાલી, FALSE, curbuf);
	}
    }
    નહીંતર જો (arg[0] == 3)
    {
	પૂર્ણાંક કિંમત;

	LOG_TRN("Received compatibility test result: %s", tp);
	xcc_status.tr_progress = STATUS_GOT;

	// Third row: xterm compatibility test.
	// If the cursor is on the first column then the terminal can handle
	// the request for cursor style and blinking.
	કિંમત = arg[1] == 1 ? TPR_YES : TPR_NO;
	term_props[TPR_CURSOR_STYLE].tpr_status = કિંમત;
	term_props[TPR_CURSOR_BLINK].tpr_status = કિંમત;
    }
}



/*
 * Handle a response to T_CRV: {lead}{first}{x};{vers};{y}c
 * Xterm and alike use '>' for {first}.
 * Rxvt sends "{lead}?1;2c".
 */
    સ્થિર શૂન્ય
handle_version_response(પૂર્ણાંક first, પૂર્ણાંક *arg, પૂર્ણાંક argc, char_u *tp)
{
    // The xterm version.  It is set to zero when it can't be an actual xterm
    // version.
    પૂર્ણાંક version = arg[1];

    LOG_TRN("Received CRV response: %s", tp);
    crv_status.tr_progress = STATUS_GOT;
    did_cursorhold = TRUE;

    // Reset terminal properties that are set based on the termresponse.
    // Mainly useful for tests that send the termresponse multiple times.
    // For testing all props can be reset.
    init_term_props(
#ifdef FEAT_EVAL
	    reset_term_props_on_termresponse
#નહીંતર
	    FALSE
#endif
	    );

    // If this code starts with CSI, you can bet that the
    // terminal uses 8-bit codes.
    જો (tp[0] == CSI)
	switch_to_8bit();

    // Screen sends 40500.
    // rxvt sends its version number: "20703" is 2.7.3.
    // Ignore it for when the user has set 'term' to xterm,
    // even though it's an rxvt.
    જો (version > 20000)
	version = 0;

    // Figure out more if the response is CSI > 99 ; 99 ; 99 c
    જો (first == '>' && argc == 3)
    {
	// mintty 2.9.5 sends 77;20905;0c.
	// (77 is ASCII 'M' for mintty.)
	જો (arg[0] == 77)
	{
	    // mintty can do SGR mouse reporting
	    term_props[TPR_MOUSE].tpr_status = TPR_MOUSE_SGR;
	}

#ifdef FEAT_TERMRESPONSE
	// If xterm version >= 141 try to get termcap codes.  For other
	// terminals the request should be ignored.
	જો (version >= 141 && p_xtermcodes)
	{
	    LOG_TR1("Enable checking for XT codes");
	    check_for_codes = TRUE;
	    need_gather = TRUE;
	    req_codes_from_term();
	}
#endif

	// libvterm sends 0;100;0
	// Konsole sends 0;115;0 and works the same way
	જો ((version == 100 || version == 115) && arg[0] == 0 && arg[2] == 0)
	{
	    // If run from Vim $COLORS is set to the number of
	    // colors the terminal supports.  Otherwise assume
	    // 256, libvterm supports even more.
	    જો (mch_getenv((char_u *)"COLORS") == ખાલી)
		may_adjust_color_ગણતરી(256);
	    // Libvterm can handle SGR mouse reporting.
	    term_props[TPR_MOUSE].tpr_status = TPR_MOUSE_SGR;
	}

	જો (version == 95)
	{
	    // Mac Terminal.app sends 1;95;0
	    //
	    // Terminal.app doesn't seem to handle DECRQM sequences
	    // properly, see issue #19852.
	    જો (arg[0] == 1 && arg[2] == 0)
	    {
		term_props[TPR_UNDERLINE_RGB].tpr_status = TPR_YES;
		term_props[TPR_MOUSE].tpr_status = TPR_MOUSE_SGR;
		term_props[TPR_DECRQM].tpr_status = TPR_NO;
	    }
	    // iTerm2 sends 0;95;0
	    નહીંતર જો (arg[0] == 0 && arg[2] == 0)
	    {
		// iTerm2 can do SGR mouse reporting
		term_props[TPR_MOUSE].tpr_status = TPR_MOUSE_SGR;
		term_props[TPR_DECRQM].tpr_status = TPR_YES;
	    }
	    // old iTerm2 sends 0;95;
	    નહીંતર જો (arg[0] == 0 && arg[2] == -1)
		term_props[TPR_UNDERLINE_RGB].tpr_status = TPR_YES;
	}

	// screen sends 83;40500;0 83 is 'S' in ASCII.
	જો (arg[0] == 83)
	{
	    // screen supports SGR mouse codes since 4.7.0
	    જો (arg[1] >= 40700)
		term_props[TPR_MOUSE].tpr_status = TPR_MOUSE_SGR;
	    નહીંતર
		term_props[TPR_MOUSE].tpr_status = TPR_MOUSE_XTERM;
	}

	// If no recognized terminal has set mouse behavior, assume xterm.
	જો (term_props[TPR_MOUSE].tpr_status == TPR_UNKNOWN)
	{
	    // Xterm version 277 supports SGR.
	    // Xterm version >= 95 supports mouse dragging.
	    જો (version >= 277)
		term_props[TPR_MOUSE].tpr_status = TPR_MOUSE_SGR;
	    નહીંતર જો (version >= 95)
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
	જો (arg[1] >= 2500)
	{
	    term_props[TPR_UNDERLINE_RGB].tpr_status = TPR_YES;
	}

	નહીંતર જો (version == 136 && arg[2] == 0)
	{
	    term_props[TPR_UNDERLINE_RGB].tpr_status = TPR_YES;

	    // PuTTY sends 0;136;0
	    જો (arg[0] == 0)
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
	જો (arg[0] == 1 && arg[1] >= 4000 && arg[1] <= 4009)
	{
	    term_props[TPR_KITTY].tpr_status = TPR_YES;
	    term_props[TPR_KITTY].tpr_set_by_termresponse = TRUE;

	    // Kitty can handle SGR mouse reporting.
	    term_props[TPR_MOUSE].tpr_status = TPR_MOUSE_SGR;
	    term_props[TPR_DECRQM].tpr_status = TPR_YES;
	}

	// foot terminal sends 1;12700;0
	જો (arg[0] == 1 && version == 12700 && arg[2] == 0)
	{
	    term_props[TPR_MOUSE].tpr_status = TPR_MOUSE_SGR;
	    term_props[TPR_DECRQM].tpr_status = TPR_YES;
	}

	// GNU screen sends 83;30600;0, 83;40500;0, etc.
	// 30600/40500 is a version number of GNU screen. DA2 support is added
	// on 3.6.  DCS string has a special meaning to GNU screen, but xterm
	// compatibility checking does not detect GNU screen.
	જો (arg[0] == 83 && arg[1] >= 30600)
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
	જો (version < 95)
	    term_props[TPR_UNDERLINE_RGB].tpr_status = TPR_YES;

	// Getting the cursor style is only supported properly by xterm since
	// version 279 (otherwise it returns 0x18).
	જો (version < 279)
	    term_props[TPR_CURSOR_STYLE].tpr_status = TPR_NO;

	/*
	 * Take action on the detected properties.
	 */

	// Unless the underline RGB color is expected to work, disable "t_8u".
	// It does not work for the real Xterm, it resets the background color.
	// This may cause some flicker.  Alternative would be to set "t_8u"
	// here if the terminal is expected to support it, but that might
	// conflict with what was set in the .vimrc.
	જો (term_props[TPR_UNDERLINE_RGB].tpr_status != TPR_YES
			&& *T_8U != NUL
			&& !option_was_set((char_u *)"t_8u"))
	{
	    set_string_option_direct((char_u *)"t_8u", -1, (char_u *)"",
								  OPT_FREE, 0);
	}
#ifdef FEAT_TERMRESPONSE
	જો (*T_8U != NUL && write_t_8u_state == MAYBE)
	    // Did skip writing t_8u, a complete redraw is needed.
	    redraw_later_clear();
	write_t_8u_state = OK;  // can output t_8u now
#endif

	// Only set 'ttymouse' automatically if it was not set
	// by the user already.
	જો (!option_was_set((char_u *)"ttym")
		&& (term_props[TPR_MOUSE].tpr_status == TPR_MOUSE_XTERM2
		    || term_props[TPR_MOUSE].tpr_status == TPR_MOUSE_SGR))
	{
	    set_option_કિંમત_give_err((char_u *)"ttym", 0L,
		    term_props[TPR_MOUSE].tpr_status == TPR_MOUSE_SGR
				    ? (char_u *)"sgr" : (char_u *)"xterm2", 0);
	}

#ifdef FEAT_TERMRESPONSE
	પૂર્ણાંક need_flush = FALSE;

	// Only request the cursor style if t_SH and t_RS are
	// set. Only supported properly by xterm since version
	// 279 (otherwise it returns 0x18).
	// Only when getting the cursor style was detected to work.
	// Not for Terminal.app, it can't handle t_RS, it
	// echoes the characters to the screen.
	જો (rcs_status.tr_progress == STATUS_GET
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
	જો (rbm_status.tr_progress == STATUS_GET
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
	જો (decrqm_status.tr_progress == STATUS_GET
		&& term_props[TPR_DECRQM].tpr_status == TPR_YES)
	{
	    MAY_WANT_TO_LOG_THIS;
	    LOG_TR1("Sending DECRQM requests");
	    માટે (પૂર્ણાંક i = 0; i < (પૂર્ણાંક)ARRAY_LENGTH(dec_modes); i++)
	    {
		vim_snprintf((અક્ષર *)IObuff, IOSIZE, "\033[?%d$p", dec_modes[i]);
		out_str(IObuff);
	    }
	    termrequest_sent(&decrqm_status);
	    need_flush = TRUE;

	}

	જો (need_flush)
	    out_flush();
#endif
    }
}

/*
 * Add "key" to "buf" and return the number of bytes used.
 * Handles special keys and multi-byte characters.
 */
    સ્થિર પૂર્ણાંક
add_ચાવી_to_buf(પૂર્ણાંક ચાવી, char_u *buf)
{
    પૂર્ણાંક idx = 0;

    જો (IS_SPECIAL(ચાવી))
    {
	buf[idx++] = K_SPECIAL;
	buf[idx++] = KEY2TERMCAP0(ચાવી);
	buf[idx++] = KEY2TERMCAP1(ચાવી);
    }
    નહીંતર જો (has_mbyte)
	idx += (*mb_char2bytes)(ચાવી, buf + idx);
    નહીંતર
	buf[idx++] = ચાવી;
    પાછા idx;
}

/*
 * Shared between handle_key_with_modifier() and handle_csi_function_key().
 */
    સ્થિર પૂર્ણાંક
put_ચાવી_modifiers_in_typebuf(
	પૂર્ણાંક	ચાવી_arg,
	પૂર્ણાંક	modifiers_arg,
	પૂર્ણાંક	csi_len,
	પૂર્ણાંક	offset,
	char_u	*buf,
	પૂર્ણાંક	bufsize,
	પૂર્ણાંક	*buflen)
{
    પૂર્ણાંક ચાવી = ચાવી_arg;
    પૂર્ણાંક modifiers = modifiers_arg;

    // Some keys need adjustment when the Ctrl modifier is used.
    ચાવી = may_adjust_ચાવી_for_ctrl(modifiers, ચાવી);

    // May remove the shift modifier if it's already included in the key.
    modifiers = may_remove_shift_modifier(modifiers, ચાવી);

    // Produce modifiers with K_SPECIAL KS_MODIFIER {mod}
    // worst-case: 3-byte modifier + 4 byte multi-char key + NUL
    char_u string[MAX_ચાવી_CODE_LEN + 2];
    પૂર્ણાંક new_slen = modifiers2keycode(modifiers, &ચાવી, string);

    // reject overlong key that would overflow string
    જો (ચાવી > 0x10FFFF)
	પાછા -1;

    // Add the bytes for the key.
    new_slen += add_ચાવી_to_buf(ચાવી, string + new_slen);

    string[new_slen] = NUL;
    જો (put_string_in_typebuf(offset, csi_len, string, new_slen,
						 buf, bufsize, buflen) == FAIL)
	પાછા -1;
    પાછા new_slen - csi_len + offset;
}

/*
 * Parse the number from a CSI numbered sequence for an F1-F12 key:
 *	ESC [ {number} ~
 * Returns the key
 */
    સ્થિર પૂર્ણાંક
parse_csi_f_ચાવીઓ(પૂર્ણાંક arg)
{
    char_u ચાવી_name[2] = { 0, 0 };

    પસંદગી (arg)
    {
	સ્થિતિ 11: ચાવી_name[0] = 'k'; ચાવી_name[1] = '1'; તોડો;  // K_F1
	સ્થિતિ 12: ચાવી_name[0] = 'k'; ચાવી_name[1] = '2'; તોડો;  // K_F2
	સ્થિતિ 13: ચાવી_name[0] = 'k'; ચાવી_name[1] = '3'; તોડો;  // K_F3
	સ્થિતિ 14: ચાવી_name[0] = 'k'; ચાવી_name[1] = '4'; તોડો;  // K_F4
	સ્થિતિ 15: ચાવી_name[0] = 'k'; ચાવી_name[1] = '5'; તોડો;  // K_F5
	સ્થિતિ 17: ચાવી_name[0] = 'k'; ચાવી_name[1] = '6'; તોડો;  // K_F6
	સ્થિતિ 18: ચાવી_name[0] = 'k'; ચાવી_name[1] = '7'; તોડો;  // K_F7
	સ્થિતિ 19: ચાવી_name[0] = 'k'; ચાવી_name[1] = '8'; તોડો;  // K_F8
	સ્થિતિ 20: ચાવી_name[0] = 'k'; ચાવી_name[1] = '9'; તોડો;  // K_F9
	સ્થિતિ 21: ચાવી_name[0] = 'F'; ચાવી_name[1] = ';'; તોડો;  // K_F10
	સ્થિતિ 23: ચાવી_name[0] = 'F'; ચાવી_name[1] = '1'; તોડો;  // K_F11
	સ્થિતિ 24: ચાવી_name[0] = 'F'; ચાવી_name[1] = '2'; તોડો;  // K_F12
    }
    જો (ચાવી_name[0])
	પાછા TERMCAP2_ચાવી(ચાવી_name[0], ચાવી_name[1]);
    // shouldn't happen
    પાછા arg;
}

/*
 * Handle a sequence with key and modifier, one of:
 *	{lead}27;{modifier};{key}~
 *	{lead}{key};{modifier}u
 * Returns the difference in length.
 */
    સ્થિર પૂર્ણાંક
handle_ચાવી_with_modifier(
	પૂર્ણાંક	*arg,
	પૂર્ણાંક	csi_len,
	પૂર્ણાંક	offset,
	char_u	*buf,
	પૂર્ણાંક	bufsize,
	પૂર્ણાંક	*buflen,
	પૂર્ણાંક	iskitty,
	પૂર્ણાંક	trail)
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
    જો (!iskitty
	    && (kitty_protocol_state == KKPS_INITIAL
		|| kitty_protocol_state == KKPS_OFF
		|| kitty_protocol_state == KKPS_AFTER_T_TE)
	    && term_props[TPR_KITTY].tpr_status != TPR_YES)
    {
#ifdef FEAT_EVAL
	ch_log(ખાલી, "setting seenModifyOtherKeys to TRUE");
#endif
	seen_Modify_Other_ચાવીઓ = TRUE;
    }

    પૂર્ણાંક ચાવી = iskitty ? arg[0] : arg[2];
    પૂર્ણાંક modifiers = decode_modifiers(arg[1]);

    // Some terminals do not apply the Shift modifier to the key.  To make
    // mappings consistent we do it here.  TODO: support more keys.
    જો ((modifiers & MOD_MASK_SHIFT) && ચાવી >= 'a' && ચાવી <= 'z')
	ચાવી += 'A' - 'a';

    // Putting Esc in the buffer creates ambiguity, it can be the start of an
    // escape sequence.  Use K_ESC to avoid that.
    જો (ચાવી == ESC)
	ચાવી = K_ESC;

    નહીંતર જો (arg[0] >= 11 && arg[0] <= 24 && trail == '~')
	ચાવી = parse_csi_f_ચાવીઓ(arg[0]);

    પાછા put_ચાવી_modifiers_in_typebuf(ચાવી, modifiers, csi_len, offset, buf,
	    bufsize, buflen);
}

/*
 * Handle a sequence with key without a modifier:
 *	{lead}{key}u
 *	{lead}{key}~
 * Returns the difference in length.
 */
    સ્થિર પૂર્ણાંક
handle_ચાવી_without_modifier(
	પૂર્ણાંક	*arg,
	પૂર્ણાંક	csi_len,
	પૂર્ણાંક	offset,
	char_u	*buf,
	પૂર્ણાંક	bufsize,
	પૂર્ણાંક	*buflen,
	પૂર્ણાંક	trail)
{
    char_u  string[MAX_ચાવી_CODE_LEN + 1];
    પૂર્ણાંક	    new_slen;

    જો (arg[0] == ESC)
    {
	// Putting Esc in the buffer creates ambiguity, it can be the start of
	// an escape sequence.  Use K_ESC to avoid that.
	string[0] = K_SPECIAL;
	string[1] = KS_EXTRA;
	string[2] = KE_ESC;
	new_slen = 3;
    }
    નહીંતર જો (arg[0] >= 11 && arg[0] <= 24 && trail == '~')
    {
	પૂર્ણાંક ચાવી = parse_csi_f_ચાવીઓ(arg[0]);
	string[0] = K_SPECIAL;
	string[1] = KEY2TERMCAP0(ચાવી);
	string[2] = KEY2TERMCAP1(ચાવી);
	new_slen = 3;
    }
    નહીંતર
	new_slen = add_ચાવી_to_buf(arg[0], string);

    જો (put_string_in_typebuf(offset, csi_len, string, new_slen,
						 buf, bufsize, buflen) == FAIL)
	પાછા -1;
    પાછા new_slen - csi_len + offset;
}

/*
 * CSI function key without or with modifiers:
 *	{lead}[ABCDEFHPQRS]
 *	{lead}1;{modifier}[ABCDEFHPQRS]
 * Returns zero when nog recognized, a positive number when recognized.
 */
    સ્થિર પૂર્ણાંક
handle_csi_function_ચાવી(
	પૂર્ણાંક	argc,
	પૂર્ણાંક	*arg,
	પૂર્ણાંક	trail,
	પૂર્ણાંક	csi_len,
	char_u	*ચાવી_name,
	પૂર્ણાંક	offset,
	char_u  *buf,
	પૂર્ણાંક	bufsize,
	પૂર્ણાંક	*buflen)
{
    ચાવી_name[0] = 'k';
    પસંદગી (trail)
    {
	સ્થિતિ 'A': ચાવી_name[1] = 'u'; તોડો;  // K_UP
	સ્થિતિ 'B': ચાવી_name[1] = 'd'; તોડો;  // K_DOWN
	સ્થિતિ 'C': ચાવી_name[1] = 'r'; તોડો;  // K_RIGHT
	સ્થિતિ 'D': ચાવી_name[1] = 'l'; તોડો;  // K_LEFT

	// case 'E': keypad BEGIN - not supported
	સ્થિતિ 'F': ચાવી_name[0] = '@'; ચાવી_name[1] = '7'; તોડો;  // K_END
	સ્થિતિ 'H': ચાવી_name[1] = 'h'; તોડો;  // K_HOME

	સ્થિતિ 'P': ચાવી_name[1] = '1'; તોડો;  // K_F1
	સ્થિતિ 'Q': ચાવી_name[1] = '2'; તોડો;  // K_F2
	સ્થિતિ 'R': ચાવી_name[1] = '3'; તોડો;  // K_F3
	સ્થિતિ 'S': ચાવી_name[1] = '4'; તોડો;  // K_F4

	મૂળભૂત: પાછા 0;  // not recognized
    }

    પૂર્ણાંક ચાવી = TERMCAP2_ચાવી(ચાવી_name[0], ચાવી_name[1]);
    પૂર્ણાંક modifiers = argc == 2 ? decode_modifiers(arg[1]) : 0;
    put_ચાવી_modifiers_in_typebuf(ચાવી, modifiers,
					csi_len, offset, buf, bufsize, buflen);
    પાછા csi_len;
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
    સ્થિર પૂર્ણાંક
handle_csi(
	char_u	*tp,
	પૂર્ણાંક	len,
	char_u	*argp,
	પૂર્ણાંક	offset,
	char_u  *buf,
	પૂર્ણાંક	bufsize,
	પૂર્ણાંક	*buflen,
	char_u	*ચાવી_name,
	પૂર્ણાંક	*slen)
{
    પૂર્ણાંક		first = -1;  // optional char right after {lead}
    પૂર્ણાંક		trail;	     // char that ends CSI sequence
    પૂર્ણાંક		arg[3] = {-1, -1, -1};	// argument numbers
    પૂર્ણાંક		argc = 0;		// number of arguments
    char_u	*ap = argp;
    પૂર્ણાંક		csi_len;

    // Check for non-digit after CSI.
    જો (!VIM_ISDIGIT(*ap))
	first = *ap++;

    જો (first >= 'A' && first <= 'Z')
    {
	// If "first" is in [ABCDEFHPQRS] then it is actually the "trail" and
	// no argument follows.
	trail = first;
	first = -1;
	--ap;
    }
    નહીંતર
    {
	// Find up to three argument numbers.
	માટે (argc = 0; argc < 3; )
	{
	    જો (ap >= tp + len)
		પાછા -1;
	    જો (*ap == ';')
		arg[argc++] = -1;  // omitted number
	    નહીંતર જો (VIM_ISDIGIT(*ap))
	    {
		arg[argc] = 0;
		માટે (;;)
		{
		    જો (ap >= tp + len)
			પાછા -1;
		    જો (!VIM_ISDIGIT(*ap))
			તોડો;
		    // avoid overflow
		    જો (arg[argc] <= (INT_MAX - 9) / 10)
			arg[argc] = arg[argc] * 10 + (*ap - '0');
		    ++ap;
		}
		++argc;
	    }
	    જો (*ap == ';')
		++ap;
	    નહીંતર
		તોડો;
	}

	// mrxvt has been reported to have "+" in the version. Assume
	// the escape sequence ends with a letter or one of "{|}~".
	જ્યારે (ap < tp + len
		&& !(*ap >= '{' && *ap <= '~')
		&& !ASCII_ISALPHA(*ap))
	    ++ap;
	જો (ap >= tp + len)
	    પાછા -1;
	trail = *ap;
    }

    csi_len = (પૂર્ણાંક)(ap - tp) + 1;

    // Response to XTQMODKEYS: "CSI > 4 ; Pv m" where Pv indicates the
    // modifyOtherKeys level.  Drop similar responses.
    જો (first == '>' && (argc == 1 || argc == 2) && trail == 'm')
    {
	જો (arg[0] == 4 && argc == 2)
	    modify_otherkeys_state = arg[1] == 2 ? MOKS_ENABLED : MOKS_OFF;

	ચાવી_name[0] = (પૂર્ણાંક)KS_EXTRA;
	ચાવી_name[1] = (પૂર્ણાંક)KE_IGNORE;
	*slen = csi_len;
    }

    // Function key starting with CSI:
    //	{lead}[ABCDEFHPQRS]
    //	{lead}1;{modifier}[ABCDEFHPQRS]
    નહીંતર જો (first == -1 && ASCII_ISUPPER(trail)
	    && (argc == 0 || (argc == 2 && arg[0] == 1)))
    {
	પૂર્ણાંક res = handle_csi_function_ચાવી(argc, arg, trail,
			      csi_len, ચાવી_name, offset, buf, bufsize, buflen);
	પાછા res <= 0 ? res : len + res;
    }

    // Cursor position report: {lead}{row};{col}R
    // Eat it when there are 2 arguments and it ends in 'R'.
    // Also when u7_status is not "sent", it may be from a previous Vim that
    // just exited.  But not for <S-F3>, it sends something similar, check for
    // row and column to make sense.
    નહીંતર જો (first == -1 && argc == 2 && trail == 'R')
    {
	handle_u7_response(arg, tp, csi_len);

	ચાવી_name[0] = (પૂર્ણાંક)KS_EXTRA;
	ચાવી_name[1] = (પૂર્ણાંક)KE_IGNORE;
	*slen = csi_len;
    }

#જો (defined(UNIX) || defined(MSWIN)) \
    && (defined(FEAT_IMAGE) || defined(FEAT_EVAL))
    // Response to CSI 14 t or CSI 16 t
    નહીંતર જો (first == -1 && argc >= 3
	    && (arg[0] == 4 || arg[0] == 6) && trail == 't')
    {
	જો (arg[0] == 4)
	{
	    LOG_TRN("Received CSI 14 t response: %s", tp);
	    cell_width = arg[2] / Columns;
	    cell_height = arg[1] / Rows;
	}
	નહીંતર
	{
	    LOG_TRN("Received CSI 16 t response: %s", tp);
	    cell_width = arg[2];
	    cell_height = arg[1];
	}

	ચાવી_name[0] = (પૂર્ણાંક)KS_EXTRA;
	ચાવી_name[1] = (પૂર્ણાંક)KE_IGNORE;
	*slen = csi_len;
    }
#endif

    // Primary device attributes (DA1) response
    નહીંતર જો (first == '?' && trail == 'c')
    {
	LOG_TRN("Received DA1 response: %s", tp);

	*slen = csi_len;
#ifdef FEAT_EVAL
	set_vim_var_string(VV_TERMDA1, tp, *slen);
#endif
	apply_autocmds(EVENT_TERMRESPONSEALL,
					(char_u *)"da1", ખાલી, FALSE, curbuf);

	ચાવી_name[0] = (પૂર્ણાંક)KS_EXTRA;
	ચાવી_name[1] = (પૂર્ણાંક)KE_IGNORE;
	*slen = csi_len;
    }

    // DECRPM mode 2026 or 2048.
    નહીંતર જો (first == '?' && trail == 'y' && argc == 2
	    && (arg[0] == 2026 || arg[0] == 2048))
    {
	પૂર્ણાંક setting = arg[1];

	*slen = csi_len;
	ચાવી_name[0] = (પૂર્ણાંક)KS_EXTRA;
	ચાવી_name[1] = (પૂર્ણાંક)KE_IGNORE;

#ifdef FEAT_TERMRESPONSE
	// Mark the DECRQM request as answered so it is not sent again and
	// stoptermcap() does not wait for it.
	જો (decrqm_status.tr_progress == STATUS_SENT)
	    decrqm_status.tr_progress = STATUS_GOT;
#endif

	જો (setting >= 0 && setting <= 4)
	{
	    LOG_TRN("Received DECRPM mode %d: %s", arg[0], tp);

	    પસંદગી (arg[0])
	    {
		સ્થિતિ 2026:
		    sync_output_setting = setting;
		    set_option_કિંમત_give_err((char_u *)"termsync",
			    setting == 1 || setting == 2, ખાલી, 0);
		    તોડો;
#ifdef UNIX
		સ્થિતિ 2048:
		    win_resize_setting = setting;

		    term_set_win_resize(true);
		    તોડો;
#endif
	    }
	}
	નહીંતર
	    LOG_TRN("Unknown DECRPM mode %d setting %d", arg[0], setting);
    }

#ifdef UNIX
    // In-band window resize event
    નહીંતર જો (win_resize_enabled && argc >= 3 && arg[0] == 48)
    {
	પૂર્ણાંક height = arg[1], width = arg[2];

	*slen = csi_len;
	ચાવી_name[0] = (પૂર્ણાંક)KS_EXTRA;
	ચાવી_name[1] = (પૂર્ણાંક)KE_IGNORE;

	// Only update if needed. Avoids intro message from disappearing on
	// startup (due to initial event).
	જો (height != Rows || width != Columns)
	    set_shellsize(width, height, true);
    }
#endif

    // Version string: Eat it when there is at least one digit and
    // it ends in 'c'
    નહીંતર જો (*T_CRV != NUL && ap > argp + 1 && trail == 'c')
    {
	handle_version_response(first, arg, argc, tp);

	*slen = csi_len;
#ifdef FEAT_EVAL
	set_vim_var_string(VV_TERMRESPONSE, tp, *slen);
#endif
	apply_autocmds(EVENT_TERMRESPONSE,
					ખાલી, ખાલી, FALSE, curbuf);
	apply_autocmds(EVENT_TERMRESPONSEALL,
					(char_u *)"version", ખાલી, FALSE, curbuf);
	ચાવી_name[0] = (પૂર્ણાંક)KS_EXTRA;
	ચાવી_name[1] = (પૂર્ણાંક)KE_IGNORE;
    }

#ifdef FEAT_TERMRESPONSE
    // Check blinking cursor from xterm:
    // {lead}?12;1$y       set
    // {lead}?12;2$y       not set
    //
    // {lead} can be <Esc>[ or CSI
    નહીંતર જો (rbm_status.tr_progress == STATUS_SENT
	    && first == '?'
	    && ap == argp + 6
	    && arg[0] == 12
	    && ap[-1] == '$'
	    && trail == 'y')
    {
	initial_cursor_blink = (arg[1] == '1');
	rbm_status.tr_progress = STATUS_GOT;
	LOG_TRN("Received cursor blinking mode response: %s", tp);
	ચાવી_name[0] = (પૂર્ણાંક)KS_EXTRA;
	ચાવી_name[1] = (પૂર્ણાંક)KE_IGNORE;
	*slen = csi_len;
# ifdef FEAT_EVAL
	set_vim_var_string(VV_TERMBLINKRESP, tp, *slen);
# endif
	apply_autocmds(EVENT_TERMRESPONSEALL,
					(char_u *)"cursorblink", ખાલી, FALSE, curbuf);
    }
#endif

    // Kitty keyboard protocol status response: CSI ? flags u
    નહીંતર જો (first == '?' && argc == 1 && trail == 'u')
    {
	// The protocol has various "progressive enhancement flags" values, but
	// we only check for zero and non-zero here.
	જો (arg[0] == '0')
	{
	    kitty_protocol_state = KKPS_OFF;
	}
	નહીંતર
	{
	    kitty_protocol_state = KKPS_ENABLED;

	    // Reset seenModifyOtherKeys just in case some key combination has
	    // been seen that set it before we get the status response.
#ifdef FEAT_EVAL
	    ch_log(ખાલી, "setting seenModifyOtherKeys to FALSE");
#endif
	    seen_Modify_Other_ચાવીઓ = FALSE;
	}

	ચાવી_name[0] = (પૂર્ણાંક)KS_EXTRA;
	ચાવી_name[1] = (પૂર્ણાંક)KE_IGNORE;
	*slen = csi_len;
    }

#ifdef FEAT_TERMRESPONSE
    // Check for a window position response from the terminal:
    //       {lead}3;{x};{y}t
    નહીંતર જો (did_request_winpos && argc == 3 && arg[0] == 3
						   && trail == 't')
    {
	winpos_x = arg[1];
	winpos_y = arg[2];
	// got finished code: consume it
	ચાવી_name[0] = (પૂર્ણાંક)KS_EXTRA;
	ચાવી_name[1] = (પૂર્ણાંક)KE_IGNORE;
	*slen = csi_len;

	જો (--did_request_winpos <= 0)
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
    નહીંતર જો ((arg[0] == 27 && argc == 3 && trail == '~')
		|| (argc == 2 && (trail == 'u' || trail == '~')))
    {
	પૂર્ણાંક iskitty = argc == 2 && (trail == 'u' || trail == '~');
	પાછા len + handle_ચાવી_with_modifier(arg, csi_len, offset, buf,
		bufsize, buflen, iskitty, trail);
    }

    // Key without modifier (Kitty sends this for Esc or F3):
    //	{lead}{key}u
    //	{lead}{key}~
    નહીંતર જો (argc == 1 && (trail == 'u' || trail == '~'))
    {
	પાછા len + handle_ચાવી_without_modifier(arg, csi_len, offset, buf,
		bufsize, buflen, trail);
    }

    // else: Unknown CSI sequence.  We could drop it, but then the
    // user can't create a map for it.
    પાછા 0;
}

સ્થિર શૂન્ય
check_for_color_response(char_u *resp, પૂર્ણાંક len)
{
    પૂર્ણાંક		i, j;
    char_u	*argp;

    j = 1 + (resp[0] == ESC);
    argp = resp + j;

    જો (len >= j + 3 && (argp[0] != '1'
			     || (argp[1] != '1' && argp[1] != '0')
			     || argp[2] != ';'))
	i = 0; // no match
    નહીંતર
    {
	માટે (i = j; i < len; ++i)
	    જો (resp[i] == '\007' || (resp[0] == OSC ? resp[i] == STERM
			: (resp[i] == ESC && i + 1 < len && resp[i + 1] == '\\')))
	    {
		પૂર્ણાંક is_bg = argp[1] == '1';
		પૂર્ણાંક is_4digit = i - j >= 21 && resp[j + 11] == '/'
						  && resp[j + 16] == '/';

		જો (i - j >= 15 && STRNCMP(resp + j + 3, "rgb:", 4) == 0
			    && (is_4digit
				   || (resp[j + 9] == '/' && resp[j + 12] == '/')))
		{
		    char_u *tp_r = resp + j + 7;
		    char_u *tp_g = resp + j + (is_4digit ? 12 : 10);
		    char_u *tp_b = resp + j + (is_4digit ? 17 : 13);
#ifdef FEAT_TERMRESPONSE
		    પૂર્ણાંક rval, gval, bval;

		    rval = hexhex2nr(tp_r);
		    gval = hexhex2nr(tp_g);
		    bval = hexhex2nr(tp_b);
#endif
		    જો (is_bg)
		    {
			અક્ષર *new_bg_val = (3 * '6' < *tp_r + *tp_g +
					     *tp_b) ? "light" : "dark";

			LOG_TRN("Received RBG response: r=%d g=%d b=%d", rval, gval, bval);
#ifdef FEAT_TERMRESPONSE
			rbg_status.tr_progress = STATUS_GOT;
			bg_r = rval;
			bg_g = gval;
			bg_b = bval;
#endif
			જો (!option_was_set((char_u *)"bg")
				      && STRCMP(p_bg, new_bg_val) != 0)
			{
			    // value differs, apply it
			    set_option_કિંમત_give_err((char_u *)"bg",
						  0L, (char_u *)new_bg_val, 0);
			    reset_option_was_set((char_u *)"bg");
			    redraw_asap(UPD_CLEAR);
			}
		    }
#જો defined(FEAT_TERMRESPONSE) && defined(FEAT_TERMINAL)
		    નહીંતર
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
			ખાલી, FALSE, curbuf);
		તોડો;
	    }
    }
    જો (i == len)
	LOG_TR1("not enough characters for RB");
}

સ્થિર oscstate_T osc_state;

/*
 * Return true if currently receiving an OSC response.
 */
    bool
in_osc_sequence(શૂન્ય)
{
    પાછા osc_state.processing;
}

/*
 * Handles any OSC sequence and places the result in "v:termosc". Returns OK on
 * success and FAIL on failure.
 */
    સ્થિર પૂર્ણાંક
handle_osc(char_u *tp, પૂર્ણાંક len, char_u *ચાવી_name, પૂર્ણાંક *slen)
{
    char_u last_char;

    જો (!osc_state.processing)
    {
	પૂર્ણાંક cur;

	LOG_TRN("Received OSC response: %s", (અક્ષર*)tp);
	// Check if it is a valid OSC sequence, and consume it. OSC format
	// consists of:
	// <identifier><data><terminator>
	// <identifier> is either <Esc>] or an OSC character
	// <terminator> can be '\007', <Esc>\ or STERM.
	cur = 1 + (tp[0] == ESC);

	જો (len < cur + 1 + (tp[0] != OSC)) // Include terminator as well
	    પાછા FAIL;

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
    નહીંતર
	last_char = ((char_u *)osc_state.buf.ga_data)[osc_state.buf.ga_len - 1];

    ચાવી_name[0] = (પૂર્ણાંક)KS_EXTRA;

    // Read data and append to buffer. If we reach a terminator, then
    // finally set the vim var.
    માટે (પૂર્ણાંક i = 0; i < len; i++)
	જો (tp[i] == '\007' || (osc_state.start_char == OSC ? tp[i] == STERM
		    : ((tp[i] == ESC && i + 1 < len && tp[i + 1] == '\\')
			|| (i == 0 && tp[i] == '\\' && last_char == ESC)
			)))
	{
	    osc_state.processing = FALSE;

	    ચાવી_name[1] = (પૂર્ણાંક)KE_OSC;

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
		    ખાલી, FALSE, curbuf);
	    જો (*p_bg != savebg)
		redraw_asap(UPD_CLEAR);
	    પાછા OK;
	}

    ચાવી_name[1] = (પૂર્ણાંક)KE_IGNORE;

#ifdef ELAPSED_FUNC
    જો (ELAPSED_FUNC(osc_state.start_tv) >= p_ost)
    {
	semsg(_(e_osc_response_timed_out), osc_state.buf.ga_len,
		osc_state.buf.ga_data);

	ga_clear(&osc_state.buf);
	osc_state.processing = FALSE;
	પાછા FAIL;
    }
#endif

    ga_concat(&osc_state.buf, tp);
    *slen = len; // Consume everything

    પાછા OK;
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
    સ્થિર પૂર્ણાંક
handle_dcs(char_u *tp, char_u *argp, પૂર્ણાંક len, char_u *ચાવી_name, પૂર્ણાંક *slen)
{
    પૂર્ણાંક i, j;

    LOG_TRN("Received DCS response: %s", (અક્ષર*)tp);
    j = 1 + (tp[0] == ESC);
    જો (len < j + 3)
	i = len; // need more chars
    નહીંતર જો ((argp[1] != '+' && argp[1] != '$')
	    || (argp[2] != 'r' && argp[2] != 'R'))
	i = 0; // no match
    નહીંતર જો (argp[1] == '+')
	// key code response
	માટે (i = j; i < len; ++i)
	{
	    જો ((tp[i] == ESC && i + 1 < len && tp[i + 1] == '\\')
		    || tp[i] == STERM)
	    {
#ifdef FEAT_TERMRESPONSE
		// handle a key code response, drop a resource response
		જો (i - j >= 3 && argp[2] == 'r')
		    got_code_from_term(tp + j, i);
#endif
		ચાવી_name[0] = (પૂર્ણાંક)KS_EXTRA;
		ચાવી_name[1] = (પૂર્ણાંક)KE_IGNORE;
		*slen = i + 1 + (tp[i] == ESC);
		તોડો;
	    }
	}
    નહીંતર
    {
	// Probably the cursor shape response.  Make sure that "i"
	// is equal to "len" when there are not sufficient
	// characters.
	માટે (i = j + 3; i < len; ++i)
	{
	    જો (i - j == 3 && !SAFE_isdigit(tp[i]))
		તોડો;
	    જો (i - j == 4 && tp[i] != ' ')
		તોડો;
	    જો (i - j == 5 && tp[i] != 'q')
		તોડો;
	    જો (i - j == 6 && tp[i] != ESC && tp[i] != STERM)
		તોડો;
	    જો ((i - j == 6 && tp[i] == STERM)
		    || (i - j == 7 && tp[i] == '\\'))
	    {
#ifdef FEAT_TERMRESPONSE
		પૂર્ણાંક સંખ્યા = argp[3] - '0';

		// 0, 1 = block blink, 2 = block
		// 3 = underline blink, 4 = underline
		// 5 = vertical bar blink, 6 = vertical bar
		સંખ્યા = સંખ્યા == 0 ? 1 : સંખ્યા;
		initial_cursor_shape = (સંખ્યા + 1) / 2;
		// The blink flag is actually inverted, compared to
		// the value set with T_SH.
		initial_cursor_shape_blink =
				       (સંખ્યા & 1) ? FALSE : TRUE;
		rcs_status.tr_progress = STATUS_GOT;
#endif
		LOG_TRN("Received cursor shape response: %s", tp);

		ચાવી_name[0] = (પૂર્ણાંક)KS_EXTRA;
		ચાવી_name[1] = (પૂર્ણાંક)KE_IGNORE;
		*slen = i + 1;
#ifdef FEAT_EVAL
		set_vim_var_string(VV_TERMSTYLERESP, tp, *slen);
#endif
		apply_autocmds(EVENT_TERMRESPONSEALL,
					(char_u *)"cursorshape", ખાલી, FALSE, curbuf);
		તોડો;
	    }
	}
    }

    જો (i == len)
    {
	// These codes arrive many together, each code can be
	// truncated at any point.
	LOG_TR1("not enough characters for XT");
	પાછા FAIL;
    }
    પાછા OK;
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
    પૂર્ણાંક
check_termcode(
    પૂર્ણાંક		max_offset,
    char_u	*buf,
    પૂર્ણાંક		bufsize,
    પૂર્ણાંક		*buflen)
{
    char_u	*tp;
    char_u	*p;
    પૂર્ણાંક		slen = 0;	// init for GCC
    પૂર્ણાંક		modslen;
    પૂર્ણાંક		len;
    પૂર્ણાંક		retval = 0;
    પૂર્ણાંક		offset;
    char_u	ચાવી_name[2];
    પૂર્ણાંક		modifiers;
    char_u	*modifiers_start = ખાલી;
    પૂર્ણાંક		ચાવી;
    પૂર્ણાંક		new_slen;   // Length of what will replace the termcode
    char_u	string[MAX_ચાવી_CODE_LEN + 1];
    પૂર્ણાંક		i, j;
    પૂર્ણાંક		idx = 0;
    પૂર્ણાંક		cpo_koffset;

    cpo_koffset = (vim_strchr(p_cpo, CPO_KOFFSET) != ખાલી);

    /*
     * Speed up the checks for terminal codes by gathering all first bytes
     * used in termleader[].  Often this is just a single <Esc>.
     */
    જો (need_gather)
	gather_termleader();

    /*
     * Check at several positions in typebuf.tb_buf[], to catch something like
     * "x<Up>" that can be mapped. Stop at max_offset, because characters
     * after that cannot be used for mapping, and with @r commands
     * typebuf.tb_buf[] can become very long.
     * This is used often, KEEP IT FAST!
     */
    માટે (offset = 0; offset < max_offset; ++offset)
    {
	જો (buf == ખાલી)
	{
	    જો (offset >= typebuf.tb_len)
		તોડો;
	    tp = typebuf.tb_buf + typebuf.tb_off + offset;
	    len = typebuf.tb_len - offset;	// length of the input
	}
	નહીંતર
	{
	    જો (offset >= *buflen)
		તોડો;
	    tp = buf + offset;
	    len = *buflen - offset;
	}

	/*
	 * Don't check characters after K_SPECIAL, those are already
	 * translated terminal chars (avoid translating ~@^Hx).
	 */
	જો (*tp == K_SPECIAL)
	{
	    offset += 2;	// there are always 2 extra characters
	    ચાલુ;
	}

	જો (osc_state.processing)
	{
	    // Still processing OSC response data, go straight to handler
	    // function.
	    tp[len] = NUL;
	    ચાવી_name[0] = NUL;
	    ચાવી_name[1] = NUL;
	    modifiers = 0;
	    જાઓ handle_osc;
	}

	/*
	 * Skip this position if the character does not appear as the first
	 * character in term_strings. This speeds up a lot, since most
	 * termcodes start with the same character (ESC or CSI).
	 */
	i = *tp;
	માટે (p = termleader; *p && *p != i; ++p)
	    ;
	જો (*p == NUL)
	    ચાલુ;

	/*
	 * Skip this position if p_ek is not set and tp[0] is an ESC and we
	 * are in Insert mode.
	 */
	જો (*tp == ESC && !p_ek && (State & MODE_ઉમેરો))
	    ચાલુ;

	tp[len] = NUL;
	ચાવી_name[0] = NUL;	// no key name found yet
	ચાવી_name[1] = NUL;	// no key name found yet
	modifiers = 0;		// no modifiers yet

#ifdef FEAT_GUI
	જો (gui.in_use)
	{
	    /*
	     * GUI special key codes are all of the form [CSI xx].
	     */
	    જો (*tp == CSI)	    // Special key from GUI
	    {
		જો (len < 3)
		    પાછા -1;	    // Shouldn't happen
		slen = 3;
		ચાવી_name[0] = tp[1];
		ચાવી_name[1] = tp[2];
	    }
	}
	નહીંતર
#endif // FEAT_GUI
#ifdef MSWIN
	    જો (len >= 3 && tp[0] == CSI && tp[1] == KS_EXTRA
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
	    ચાવી_name[0] = tp[1];
	    ચાવી_name[1] = tp[2];
	}
	નહીંતર
#endif
	{
	    પૂર્ણાંક  mouse_index_found = -1;
	    પૂર્ણાંક keypad_index_found = -1;
	    પૂર્ણાંક keypad_slen_found = 0;

	    માટે (idx = 0; idx < tc_len; ++idx)
	    {
		પૂર્ણાંક     is_keypad = FALSE;
		/*
		 * Ignore the entry if we are not at the start of
		 * typebuf.tb_buf[]
		 * and there are not enough characters to make a match.
		 * But only when the 'K' flag is in 'cpoptions'.
		 */
		slen = termcodes[idx].len;
		modifiers_start = ખાલી;
		જો (cpo_koffset && offset && len < slen)
		    ચાલુ;
		જો (STRNCMP(termcodes[idx].code, tp,
				     (size_t)(slen > len ? len : slen)) == 0)
		{
		    પૂર્ણાંક	    looks_like_mouse_start = FALSE;

		    જો (len < slen)		// got a partial sequence
			પાછા -1;		// need to get more chars

		    /*
		     * When found a keypad key, check if there is another key
		     * that matches and use that one.  This makes <Home> to be
		     * found instead of <kHome> when they produce the same
		     * key code.
		     */
		    જો (termcodes[idx].name[0] == 'K'
				       && (VIM_ISDIGIT(termcodes[idx].name[1])
						|| ASCII_ISUPPER(termcodes[idx].name[1])))
		    {
			is_keypad = TRUE;
			// Only use it when there is no other match.
			જો (keypad_index_found < 0)
			{
			    keypad_index_found = idx;
			    keypad_slen_found = slen;
			}
		    }

		    જો (slen == 2 && len > 2
			    && termcodes[idx].code[0] == ESC
			    && termcodes[idx].code[1] == '[')
		    {
			// The mouse termcode "ESC [" is also the prefix of
			// "ESC [ I" (focus gained) and other keys.  Check some
			// more bytes to find out.
			જો (!SAFE_isdigit(tp[2]))
			{
			    // ESC [ without number following: Only use it when
			    // there is no other match.
			    looks_like_mouse_start = TRUE;
			}
			નહીંતર જો (termcodes[idx].name[0] == KS_DEC_MOUSE)
			{
			    char_u  *nr = tp + 2;
			    પૂર્ણાંક	    ગણતરી = 0;

			    // If a digit is following it could be a key with
			    // modifier, e.g., ESC [ 1;2P.  Can be confused
			    // with DEC_MOUSE, which requires four numbers
			    // following.  If not then it can't be a DEC_MOUSE
			    // code.
			    માટે (;;)
			    {
				++ગણતરી;
				(શૂન્ય)getdigits(&nr);
				જો (nr >= tp + len)
				    પાછા -1;	// partial sequence
				જો (*nr != ';')
				    તોડો;
				++nr;
				જો (nr >= tp + len)
				    પાછા -1;	// partial sequence
			    }
			    જો (ગણતરી < 4)
				ચાલુ;	// no match
			}
		    }
		    જો (looks_like_mouse_start)
		    {
			// Only use it when there is no other match.
			જો (mouse_index_found < 0)
			    mouse_index_found = idx;
		    }
		    નહીંતર જો (!is_keypad)
		    {
			ચાવી_name[0] = termcodes[idx].name[0];
			ચાવી_name[1] = termcodes[idx].name[1];
			તોડો;
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
		જો (termcodes[idx].modlen > 0 && mouse_index_found < 0)
		{
		    modslen = termcodes[idx].modlen;
		    જો (cpo_koffset && offset && len < modslen)
			ચાલુ;
		    જો (STRNCMP(termcodes[idx].code, tp,
				(size_t)(modslen > len ? len : modslen)) == 0)
		    {
			પૂર્ણાંક	    n;

			જો (len <= modslen)	// got a partial sequence
			    પાછા -1;		// need to get more chars

			જો (tp[modslen] == termcodes[idx].code[slen - 1])
			    // no modifiers
			    slen = modslen + 1;
			નહીંતર જો (tp[modslen] != ';' && modslen == slen - 3)
			    // no match for "code;*X" with "code;"
			    ચાલુ;
			નહીંતર જો (termcodes[idx].code[modslen] == '@'
					 && (tp[modslen] != '1'
						    || tp[modslen + 1] != ';'))
			    // no match for "<Esc>[@" with "<Esc>[1;"
			    ચાલુ;
			નહીંતર
			{
			    // Skip over the digits, the final char must
			    // follow. URXVT can use a negative value, thus
			    // also accept '-'.
			    માટે (j = slen - 2; j < len && (SAFE_isdigit(tp[j])
				       || tp[j] == '-' || tp[j] == ';'); ++j)
				;
			    ++j;
			    જો (len < j)	// got a partial sequence
				પાછા -1;	// need to get more chars
			    જો (tp[j - 1] != termcodes[idx].code[slen - 1])
				ચાલુ;	// no match

			    modifiers_start = tp + slen - 2;

			    // Match!  Convert modifier bits.
			    n = atoi((અક્ષર *)modifiers_start);
			    modifiers |= decode_modifiers(n);

			    slen = j;
			}
			જો (termcodes[idx].name[0] == 'K'
					   && (VIM_ISDIGIT(termcodes[idx].name[1])
						    || ASCII_ISUPPER(termcodes[idx].name[1])))
			{
			    is_keypad = TRUE;
			    જો (keypad_index_found < 0)
			    {
				keypad_index_found = idx;
				keypad_slen_found = slen;
			    }
			}
			જો (!is_keypad)
			{
			    ચાવી_name[0] = termcodes[idx].name[0];
			    ચાવી_name[1] = termcodes[idx].name[1];
			    તોડો;
			}
		    }
		}
	    }
	    જો (idx == tc_len && keypad_index_found >= 0)
	    {
		ચાવી_name[0] = termcodes[keypad_index_found].name[0];
		ચાવી_name[1] = termcodes[keypad_index_found].name[1];
		slen = keypad_slen_found;
	    }
	    નહીંતર જો (idx == tc_len && mouse_index_found >= 0)
	    {
		ચાવી_name[0] = termcodes[mouse_index_found].name[0];
		ચાવી_name[1] = termcodes[mouse_index_found].name[1];
	    }
	}

	જો (ચાવી_name[0] == NUL
	    // Mouse codes of DEC and pterm start with <ESC>[.  When
	    // detecting the start of these mouse codes they might as well be
	    // another key code or terminal response.
#ifdef FEAT_MOUSE_DEC
	    || ચાવી_name[0] == KS_DEC_MOUSE
#endif
#ifdef FEAT_MOUSE_PTERM
	    || ચાવી_name[0] == KS_PTERM_MOUSE
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
	    જો (((tp[0] == ESC && len >= 3 && tp[1] == '[')
			|| (tp[0] == CSI && len >= 2))
		    && vim_strchr((char_u *)"0123456789>?ABCDEFHPQRS",
								*argp) != ખાલી)
	    {
		પૂર્ણાંક resp = handle_csi(tp, len, argp, offset, buf,
					     bufsize, buflen, ચાવી_name, &slen);
		જો (resp != 0)
		{
#ifdef DEBUG_TERMRESPONSE
		    જો (resp == -1)
			LOG_TR1("Not enough characters for CSI sequence");
#endif
		    પાછા resp;
		}
	    }

	    // Check for OSC responses from terminal
	    નહીંતર જો ((tp[0] == ESC && len >= 2 && tp[1] == ']') || tp[0] == OSC)
	    {
handle_osc:
		જો (handle_osc(tp, len, ચાવી_name, &slen) == FAIL)
		    પાછા -1;
	    }

	    // Check for key code response from xterm,
	    // starting with <Esc>P or DCS
	    // It would only be needed with this condition:
	    //	    (check_for_codes || rcs_status.tr_progress == STATUS_SENT)
	    // Now this is always done so that DCS codes don't mess up things.
	    નહીંતર જો ((tp[0] == ESC && len >= 2 && tp[1] == 'P') || tp[0] == DCS)
	    {
		જો (handle_dcs(tp, argp, len, ચાવી_name, &slen) == FAIL)
		    પાછા -1;
	    }
	}

	જો (ચાવી_name[0] == NUL)
	    ચાલુ;	    // No match at this position, try next one

	// We only get here when we have a complete termcode match

#જો defined(FEAT_GUI) || defined(MSWIN)
	/*
	 * For scroll events from the GUI or MS-Windows console, fetch the
	 * pointer coordinates so that we know which window to scroll later.
	 */
	જો (TRUE
# જો defined(FEAT_GUI) && !defined(MSWIN)
		&& gui.in_use
# endif
		&& ચાવી_name[0] == (પૂર્ણાંક)KS_EXTRA
		&& (ચાવી_name[1] == (પૂર્ણાંક)KE_X1MOUSE
		    || ચાવી_name[1] == (પૂર્ણાંક)KE_X2MOUSE
		    || ચાવી_name[1] == (પૂર્ણાંક)KE_MOUSEMOVE_XY
		    || ચાવી_name[1] == (પૂર્ણાંક)KE_MOUSELEFT
		    || ચાવી_name[1] == (પૂર્ણાંક)KE_MOUSERIGHT
		    || ચાવી_name[1] == (પૂર્ણાંક)KE_MOUSEDOWN
		    || ચાવી_name[1] == (પૂર્ણાંક)KE_MOUSEUP))
	{
	    char_u	bytes[6];
	    પૂર્ણાંક		અંક_bytes = get_bytes_from_buf(tp + slen, bytes, 4);

	    જો (અંક_bytes == -1)	// not enough coordinates
		પાછા -1;
	    mouse_col = 128 * (bytes[0] - ' ' - 1) + bytes[1] - ' ' - 1;
	    mouse_row = 128 * (bytes[2] - ' ' - 1) + bytes[3] - ' ' - 1;
	    slen += અંક_bytes;
	    // equal to K_MOUSEMOVE
	    જો (ચાવી_name[1] == (પૂર્ણાંક)KE_MOUSEMOVE_XY)
		ચાવી_name[1] = (પૂર્ણાંક)KE_MOUSEMOVE;
	}
	નહીંતર
#endif
	/*
	 * If it is a mouse click, get the coordinates.
	 */
	જો (ચાવી_name[0] == KS_MOUSE
#ifdef FEAT_MOUSE_GPM
		|| ચાવી_name[0] == KS_GPM_MOUSE
#endif
#ifdef FEAT_MOUSE_JSB
		|| ચાવી_name[0] == KS_JSBTERM_MOUSE
#endif
#ifdef FEAT_MOUSE_NET
		|| ચાવી_name[0] == KS_NETTERM_MOUSE
#endif
#ifdef FEAT_MOUSE_DEC
		|| ચાવી_name[0] == KS_DEC_MOUSE
#endif
#ifdef FEAT_MOUSE_PTERM
		|| ચાવી_name[0] == KS_PTERM_MOUSE
#endif
#ifdef FEAT_MOUSE_URXVT
		|| ચાવી_name[0] == KS_URXVT_MOUSE
#endif
		|| ચાવી_name[0] == KS_SGR_MOUSE
		|| ચાવી_name[0] == KS_SGR_MOUSE_RELEASE)
	{
	    જો (check_termcode_mouse(tp, &slen, ચાવી_name, modifiers_start, idx,
							     &modifiers) == -1)
		પાછા -1;
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
	નહીંતર જો (ચાવી_name[0] == (પૂર્ણાંક)KS_MENU)
	{
	    long_u	val;
	    પૂર્ણાંક		અંક_bytes = get_long_from_buf(tp + slen, &val);

	    જો (અંક_bytes == -1)
		પાછા -1;
	    current_menu = (vimmenu_T *)val;
	    slen += અંક_bytes;

	    // The menu may have been deleted right after it was used, check
	    // for that.
	    જો (check_menu_pointer(મૂળ_menu, current_menu) == FAIL)
	    {
		ચાવી_name[0] = KS_EXTRA;
		ચાવી_name[1] = (પૂર્ણાંક)KE_IGNORE;
	    }
	}
# endif
# ifdef FEAT_GUI_TABLINE
	નહીંતર જો (ચાવી_name[0] == (પૂર્ણાંક)KS_TABLINE)
	{
	    // Selecting tabline tab or using its menu.
	    char_u	bytes[6];
	    પૂર્ણાંક		અંક_bytes = get_bytes_from_buf(tp + slen, bytes, 1);

	    જો (અંક_bytes == -1)
		પાછા -1;
	    current_tab = (પૂર્ણાંક)bytes[0];
	    જો (current_tab == 255)	// -1 in a byte gives 255
		current_tab = -1;
	    slen += અંક_bytes;
	}
	નહીંતર જો (ચાવી_name[0] == (પૂર્ણાંક)KS_TABMENU)
	{
	    // Selecting tabline tab or using its menu.
	    char_u	bytes[6];
	    પૂર્ણાંક		અંક_bytes = get_bytes_from_buf(tp + slen, bytes, 2);

	    જો (અંક_bytes == -1)
		પાછા -1;
	    current_tab = (પૂર્ણાંક)bytes[0];
	    current_tabmenu = (પૂર્ણાંક)bytes[1];
	    slen += અંક_bytes;
	}
# endif
# ifndef USE_ON_FLY_SCROLL
	નહીંતર જો (ચાવી_name[0] == (પૂર્ણાંક)KS_VER_SCROLLBAR)
	{
	    long_u	val;
	    char_u	bytes[6];
	    પૂર્ણાંક		અંક_bytes;

	    // Get the last scrollbar event in the queue of the same type
	    j = 0;
	    માટે (i = 0; tp[j] == CSI && tp[j + 1] == KS_VER_SCROLLBAR
						     && tp[j + 2] != NUL; ++i)
	    {
		j += 3;
		અંક_bytes = get_bytes_from_buf(tp + j, bytes, 1);
		જો (અંક_bytes == -1)
		    તોડો;
		જો (i == 0)
		    current_scrollbar = (પૂર્ણાંક)bytes[0];
		નહીંતર જો (current_scrollbar != (પૂર્ણાંક)bytes[0])
		    તોડો;
		j += અંક_bytes;
		અંક_bytes = get_long_from_buf(tp + j, &val);
		જો (અંક_bytes == -1)
		    તોડો;
		scrollbar_કિંમત = val;
		j += અંક_bytes;
		slen = j;
	    }
	    જો (i == 0)		// not enough characters to make one
		પાછા -1;
	}
	નહીંતર જો (ચાવી_name[0] == (પૂર્ણાંક)KS_HOR_SCROLLBAR)
	{
	    long_u	val;
	    પૂર્ણાંક		અંક_bytes;

	    // Get the last horiz. scrollbar event in the queue
	    j = 0;
	    માટે (i = 0; tp[j] == CSI && tp[j + 1] == KS_HOR_SCROLLBAR
						     && tp[j + 2] != NUL; ++i)
	    {
		j += 3;
		અંક_bytes = get_long_from_buf(tp + j, &val);
		જો (અંક_bytes == -1)
		    તોડો;
		scrollbar_કિંમત = val;
		j += અંક_bytes;
		slen = j;
	    }
	    જો (i == 0)		// not enough characters to make one
		પાછા -1;
	}
# endif // !USE_ON_FLY_SCROLL
#endif // FEAT_GUI

#જો defined(UNIX) || defined(VMS)
	/*
	 * Handle FocusIn/FocusOut event sequences reported by XTerm.
	 * (CSI I/CSI O)
	 */
	જો (ચાવી_name[0] == KS_EXTRA
# ifdef FEAT_GUI
		&& !gui.in_use
# endif
	    )
	{
	    જો (ચાવી_name[1] == KE_FOCUSGAINED)
	    {
		જો (!focus_state)
		{
		    ui_focus_change(TRUE);
		    did_cursorhold = TRUE;
		    focus_state = TRUE;
		}
		ચાવી_name[1] = (પૂર્ણાંક)KE_IGNORE;
	    }
	    નહીંતર જો (ચાવી_name[1] == KE_FOCUSLOST)
	    {
		જો (focus_state)
		{
		    ui_focus_change(FALSE);
		    did_cursorhold = TRUE;
		    focus_state = FALSE;
		}
		ચાવી_name[1] = (પૂર્ણાંક)KE_IGNORE;
	    }
	}
#endif

	/*
	 * Change <xHome> to <Home>, <xUp> to <Up>, etc.
	 */
	ચાવી = handle_x_ચાવીઓ(TERMCAP2_ચાવી(ચાવી_name[0], ચાવી_name[1]));

	// Add any modifier codes to our string.
	new_slen = modifiers2keycode(modifiers, &ચાવી, string);

	// Finally, add the special key code to our string
	ચાવી_name[0] = KEY2TERMCAP0(ચાવી);
	ચાવી_name[1] = KEY2TERMCAP1(ચાવી);
	જો (ચાવી_name[0] == KS_ચાવી)
	{
	    // from ":set <M-b>=xx"
	    જો (has_mbyte)
		new_slen += (*mb_char2bytes)(ચાવી_name[1], string + new_slen);
	    નહીંતર
		string[new_slen++] = ચાવી_name[1];
	}
	નહીંતર જો (osc_state.processing ||
		(new_slen == 0
		 && ચાવી_name[0] == KS_EXTRA
		 && ચાવી_name[1] == KE_IGNORE))
	{
	    // Do not put K_IGNORE into the buffer, do return KEYLEN_REMOVED
	    // to indicate what happened.
	    retval = KEYLEN_REMOVED;
	}
	નહીંતર
	{
	    string[new_slen++] = K_SPECIAL;
	    string[new_slen++] = ચાવી_name[0];
	    string[new_slen++] = ચાવી_name[1];
	}
	જો (put_string_in_typebuf(offset, slen, string, new_slen,
						 buf, bufsize, buflen) == FAIL)
	    પાછા -1;
	પાછા retval == 0 ? (len + new_slen - slen + offset) : retval;
    }

#ifdef FEAT_TERMRESPONSE
    LOG_TR1("normal character");
#endif

    પાછા 0;			    // no match found
}

#જો defined(FEAT_TERMINAL) && defined(FEAT_TERMRESPONSE)
/*
 * Get the text foreground color, if known.
 */
    શૂન્ય
term_get_fg_color(char_u *r, char_u *g, char_u *b)
{
    જો (rfg_status.tr_progress != STATUS_GOT)
	પાછા;

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
    પૂર્ણાંક
term_get_bg_color(char_u *r, char_u *g, char_u *b)
{
    જો (rbg_status.tr_progress != STATUS_GOT)
	પાછા FAIL;

    *r = bg_r;
    *g = bg_g;
    *b = bg_b;
    પાછા OK;
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
    પૂર્ણાંક		flags,
    પૂર્ણાંક		*did_simplify)
{
    પૂર્ણાંક		i;
    પૂર્ણાંક		slen;
    પૂર્ણાંક		ચાવી;
    size_t	dlen = 0;
    char_u	*src;
    પૂર્ણાંક		do_backslash;	// backslash is a special character
    પૂર્ણાંક		do_special;	// recognize <> key codes
    પૂર્ણાંક		do_ચાવી_code;	// recognize raw key codes
    char_u	*result;	// buffer for resulting string
    garray_T	ga;

    do_backslash = (vim_strchr(p_cpo, CPO_BSLASH) == ખાલી);
    do_special = (vim_strchr(p_cpo, CPO_SPECI) == ખાલી)
						  || (flags & REPTERM_SPECIAL);
    do_ચાવી_code = (vim_strchr(p_cpo, CPO_KEYCODE) == ખાલી);
    src = from;

    /*
     * Allocate space for the translation.  Worst case a single character is
     * replaced by 6 bytes (shifted special key), plus a NUL at the end.
     * In the rare case more might be needed ga_grow() must be called again.
     */
    ga_init2(&ga, 1L, 100);
    જો (ga_grow(&ga, (પૂર્ણાંક)(STRLEN(src) * 6 + 1)) == FAIL) // out of memory
    {
	*bufp = ખાલી;
	પાછા from;
    }
    result = ga.ga_data;

    /*
     * Check for #n at start only: function key n
     */
    જો ((flags & REPTERM_FROM_PART) && src[0] == '#' && VIM_ISDIGIT(src[1]))
    {
	result[dlen++] = K_SPECIAL;
	result[dlen++] = 'k';
	જો (src[1] == '0')
	    result[dlen++] = ';';	// #0 is F10 is "k;"
	નહીંતર
	    result[dlen++] = src[1];	// #3 is F3 is "k3"
	src += 2;
    }

    /*
     * Copy each byte from *from to result[dlen]
     */
    જ્યારે (*src != NUL)
    {
	/*
	 * If 'cpoptions' does not contain '<', check for special key codes,
	 * like "<C-S-LeftMouse>"
	 */
	જો (do_special && ((flags & REPTERM_DO_LT)
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
	    જો (STRNICMP(src, "<SID>", 5) == 0)
	    {
		જો (sid_arg < 0 || (sid_arg == 0 && current_sctx.sc_sid <= 0))
		    emsg(_(e_using_sid_not_in_script_context));
		નહીંતર
		{
		    char_u  *dot;
		    long    sid = sid_arg != 0 ? sid_arg : current_sctx.sc_sid;

		    src += 5;
		    જો (in_vim9script()
				       && (dot = vim_strchr(src, '.')) != ખાલી)
		    {
			imported_T *imp = find_imported(src, dot - src, FALSE);

			જો (imp != ખાલી)
			{
			    scriptitem_T    *si = SCRIPT_ITEM(imp->imp_sid);
			    size_t	    len;

			    src = dot + 1;
			    જો (si->sn_autoload_prefix != ખાલી)
			    {
				// Turn "<SID>name.Func"
				// into "scriptname#Func".
				len = STRLEN(si->sn_autoload_prefix);
				જો (ga_grow(&ga,
				     (પૂર્ણાંક)(STRLEN(src) * 6 + len + 1)) == FAIL)
				{
				    ga_clear(&ga);
				    *bufp = ખાલી;
				    પાછા from;
				}
				result = ga.ga_data;
				STRCPY(result + dlen, si->sn_autoload_prefix);
				dlen += len;
				ચાલુ;
			    }
			    sid = imp->imp_sid;
			}
		    }

		    result[dlen++] = K_SPECIAL;
		    result[dlen++] = (પૂર્ણાંક)KS_EXTRA;
		    result[dlen++] = (પૂર્ણાંક)KE_SNR;
		    sprintf((અક્ષર *)result + dlen, "%ld", sid);
		    dlen += STRLEN(result + dlen);
		    result[dlen++] = '_';
		    ચાલુ;
		}
	    }
#endif
	    પૂર્ણાંક fsk_flags = FSK_KEYCODE
			| ((flags & REPTERM_NO_SIMPLIFY) ? 0 : FSK_SIMPLIFY)
			| ((flags & REPTERM_FROM_PART) ? FSK_FROM_PART : 0);
	    slen = trans_special(&src, result + dlen, fsk_flags,
							   TRUE, did_simplify);
	    જો (slen > 0)
	    {
		dlen += slen;
		ચાલુ;
	    }
	}

	/*
	 * If 'cpoptions' does not contain 'k', see if it's an actual key-code.
	 * Note that this is also checked after replacing the <> form.
	 * Single character codes are NOT replaced (e.g. ^H or DEL), because
	 * it could be a character in the file.
	 */
	જો (do_ચાવી_code)
	{
	    પૂર્ણાંક len;
	    i = find_term_bykeys(src, &len);
	    જો (i >= 0)
	    {
		result[dlen++] = K_SPECIAL;
		result[dlen++] = termcodes[i].name[0];
		result[dlen++] = termcodes[i].name[1];
		src += len;
		// If terminal code matched, continue after it.
		ચાલુ;
	    }
	}

#ifdef FEAT_EVAL
	જો (do_special)
	{
	    char_u	*p, *s, len;

	    /*
	     * Replace <Leader> by the value of "mapleader".
	     * Replace <LocalLeader> by the value of "maplocalleader".
	     * If "mapleader" or "maplocalleader" isn't set use a backslash.
	     */
	    જો (STRNICMP(src, "<Leader>", 8) == 0)
	    {
		len = 8;
		p = get_var_કિંમત((char_u *)"g:mapleader");
	    }
	    નહીંતર જો (STRNICMP(src, "<LocalLeader>", 13) == 0)
	    {
		len = 13;
		p = get_var_કિંમત((char_u *)"g:maplocalleader");
	    }
	    નહીંતર
	    {
		len = 0;
		p = ખાલી;
	    }
	    જો (len != 0)
	    {
		// Allow up to 8 * 6 characters for "mapleader".
		જો (p == ખાલી || *p == NUL || STRLEN(p) > 8 * 6)
		    s = (char_u *)"\\";
		નહીંતર
		    s = p;
		જ્યારે (*s != NUL)
		    result[dlen++] = *s++;
		src += len;
		ચાલુ;
	    }
	}
#endif

	/*
	 * Remove CTRL-V and ignore the next character.
	 * For "from" side the CTRL-V at the end is included, for the "to"
	 * part it is removed.
	 * If 'cpoptions' does not contain 'B', also accept a backslash.
	 */
	ચાવી = *src;
	જો (ચાવી == Ctrl_V || (do_backslash && ચાવી == '\\'))
	{
	    ++src;				// skip CTRL-V or backslash
	    જો (*src == NUL)
	    {
		જો (flags & REPTERM_FROM_PART)
		    result[dlen++] = ચાવી;
		તોડો;
	    }
	}

	// skip multibyte char correctly
	માટે (i = (*mb_ptr2len)(src); i > 0; --i)
	{
	    /*
	     * If the character is K_SPECIAL, replace it with K_SPECIAL
	     * KS_SPECIAL KE_FILLER.
	     * If compiled with the GUI replace CSI with K_CSI.
	     */
	    જો (*src == K_SPECIAL)
	    {
		result[dlen++] = K_SPECIAL;
		result[dlen++] = KS_SPECIAL;
		result[dlen++] = KE_FILLER;
	    }
#ifdef FEAT_GUI
	    નહીંતર જો (*src == CSI)
	    {
		result[dlen++] = K_SPECIAL;
		result[dlen++] = KS_EXTRA;
		result[dlen++] = (પૂર્ણાંક)KE_CSI;
	    }
#endif
	    નહીંતર
		result[dlen++] = *src;
	    ++src;
	}
    }
    result[dlen] = NUL;

    /*
     * Copy the new string to allocated memory.
     * If this fails, just return from.
     */
    જો ((*bufp = vim_strsave(result)) != ખાલી)
	from = *bufp;
    vim_free(result);
    પાછા from;
}

/*
 * Find a termcode with keys 'src' (must be NUL terminated).
 * Return the index in termcodes[], or -1 if not found.
 */
    સ્થિર પૂર્ણાંક
find_term_bykeys(char_u *src, પૂર્ણાંક *matchlen)
{
    પૂર્ણાંક		i, j;
    પૂર્ણાંક		len = (પૂર્ણાંક)STRLEN(src);
    પૂર્ણાંક         found = -1;
    // Don't return a match for a single character
    પૂર્ણાંક         foundlen = 1;
    પૂર્ણાંક         slen, modslen;
    પૂર્ણાંક         thislen;

    // Most input bytes cannot start a terminal code.  Reuse the same leader
    // table as check_termcode() to avoid scanning termcodes[] unnecessarily.
    જો (need_gather)
	gather_termleader();
    જો (*src == NUL || vim_strchr(termleader, *src) == ખાલી)
	પાછા -1;

    // find longest match
    // borrows part of check_termcode
    માટે (i = 0; i < tc_len; ++i)
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
	જો (modslen > 0)
	{
	    જો (len > modslen
			&& STRNCMP(termcodes[i].code, src, (size_t)modslen) == 0)
	    {
		thislen = 0;

		જો (src[modslen] == termcodes[i].code[slen - 1])
		    // no modifiers
		    thislen = modslen + 1;
		નહીંતર જો (src[modslen] != ';' && modslen == slen - 3)
		    // no match for "code;*X" with "code;"
		    ચાલુ;
		નહીંતર જો (termcodes[i].code[modslen] == '@'
				&& (src[modslen] != '1'
					    || src[modslen + 1] != ';'))
		    // no match for "<Esc>[@" with "<Esc>[1;"
		    ચાલુ;
		નહીંતર
		{
		    // Skip over the digits, the final char must
		    // follow. URXVT can use a negative value, thus
		    // also accept '-'.
		    માટે (j = slen - 2; j < len && (SAFE_isdigit(src[j])
			       || src[j] == '-' || src[j] == ';'); ++j)
			;
		    ++j;
		    જો (len < j)	// got a partial sequence
			ચાલુ;
		    જો (src[j - 1] != termcodes[i].code[slen - 1])
			ચાલુ;	// no match

		    thislen = j;
		}

		જો (thislen > foundlen)
		{
		    found = i;
		    foundlen = thislen;
		}
	    }
	}
	નહીંતર
	{
	    જો (slen > foundlen && len >= slen
			&& STRNCMP(termcodes[i].code, src, (size_t)slen) == 0)
	    {
		found = i;
		foundlen = slen;
	    }
	}
    }

    જો (matchlen != ખાલી && found >= 0)
	*matchlen = foundlen;

    પાછા found;
}

/*
 * Gather the first characters in the terminal key codes into a string.
 * Used to speed up check_termcode().
 */
    સ્થિર શૂન્ય
gather_termleader(શૂન્ય)
{
    પૂર્ણાંક	    i;
    પૂર્ણાંક	    len = 0;

#ifdef FEAT_GUI
    જો (gui.in_use)
	termleader[len++] = CSI;    // the GUI codes are not in termcodes[]
#endif

    // Querying cell size uses CSI 14 t
#જો defined(MSWIN) && (defined(FEAT_IMAGE) || defined(FEAT_EVAL))
# ifdef FEAT_GUI
    જો (!gui.in_use)
# endif
    {
	termleader[len++] = ESC;
# ifdef FEAT_GUI
	જો (!gui.in_use)
# endif
	    termleader[len++] = CSI;
    }
#endif

#ifdef FEAT_TERMRESPONSE
    જો (check_for_codes || *T_CRS != NUL)
	termleader[len++] = DCS;    // the termcode response starts with DCS
				    // in 8-bit mode
#endif
    termleader[len] = NUL;

    માટે (i = 0; i < tc_len; ++i)
	જો (vim_strchr(termleader, termcodes[i].code[0]) == ખાલી)
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
    શૂન્ય
show_termcodes(પૂર્ણાંક flags)
{
    પૂર્ણાંક		col;
    પૂર્ણાંક		*વસ્તુઓ;
    પૂર્ણાંક		item_ગણતરી;
    પૂર્ણાંક		run;
    પૂર્ણાંક		row, rows;
    પૂર્ણાંક		cols;
    પૂર્ણાંક		i;
    પૂર્ણાંક		len;

#define INC3 27	    // try to make three columns
#define INC2 40	    // try to make two columns
#define GAP 2	    // spaces between columns

    જો (tc_len == 0)	    // no terminal codes (must be GUI)
	પાછા;
    વસ્તુઓ = ALLOC_MULT(પૂર્ણાંક, tc_len);
    જો (વસ્તુઓ == ખાલી)
	પાછા;

    // Highlight title
    સંદેશ_puts_title(_("\n--- Terminal keys ---"));

    /*
     * Do the loop three times:
     * 1. display the short items (non-strings and short strings)
     * 2. display the medium items (medium length strings)
     * 3. display the long items (remaining strings)
     * When "flags" has OPT_ONECOLUMN do everything in 3.
     */
    માટે (run = (flags & OPT_ONECOLUMN) ? 3 : 1; run <= 3 && !got_int; ++run)
    {
	/*
	 * collect the items in items[]
	 */
	item_ગણતરી = 0;
	માટે (i = 0; i < tc_len; i++)
	{
	    len = show_one_termcode(termcodes[i].name,
						    termcodes[i].code, FALSE);
	    જો ((flags & OPT_ONECOLUMN) ||
		    (len <= INC3 - GAP ? run == 1
			: len <= INC2 - GAP ? run == 2
			: run == 3))
		વસ્તુઓ[item_ગણતરી++] = i;
	}

	/*
	 * display the items
	 */
	જો (run <= 2)
	{
	    cols = (Columns + GAP) / (run == 1 ? INC3 : INC2);
	    જો (cols == 0)
		cols = 1;
	    rows = (item_ગણતરી + cols - 1) / cols;
	}
	નહીંતર	// run == 3
	    rows = item_ગણતરી;
	માટે (row = 0; row < rows && !got_int; ++row)
	{
	    સંદેશ_putchar('\n');			// go to next line
	    જો (got_int)			// 'q' typed in more
		તોડો;
	    col = 0;
	    માટે (i = row; i < item_ગણતરી; i += rows)
	    {
		સંદેશ_col = col;			// make columns
		show_one_termcode(termcodes[વસ્તુઓ[i]].name,
					      termcodes[વસ્તુઓ[i]].code, TRUE);
		જો (run == 2)
		    col += INC2;
		નહીંતર
		    col += INC3;
	    }
	    out_flush();
	    ui_breakcheck();
	}
    }
    vim_free(વસ્તુઓ);
}

/*
 * Show one termcode entry.
 * Output goes into IObuff[]
 */
    પૂર્ણાંક
show_one_termcode(char_u *name, char_u *code, પૂર્ણાંક printit)
{
    char_u	*p;
    પૂર્ણાંક		len;

    જો (name[0] > '~')
    {
	IObuff[0] = ' ';
	IObuff[1] = ' ';
	IObuff[2] = ' ';
	IObuff[3] = ' ';
    }
    નહીંતર
    {
	IObuff[0] = 't';
	IObuff[1] = '_';
	IObuff[2] = name[0];
	IObuff[3] = name[1];
    }
    IObuff[4] = ' ';

    p = get_special_ચાવી_name(TERMCAP2_ચાવી(name[0], name[1]), 0);
    જો (p[1] != 't')
	STRCPY(IObuff + 5, p);
    નહીંતર
	IObuff[5] = NUL;
    len = (પૂર્ણાંક)STRLEN(IObuff);
    કરો
	IObuff[len++] = ' ';
    જ્યારે (len < 17);
    IObuff[len] = NUL;
    જો (code == ખાલી)
	len += 4;
    નહીંતર
	len += vim_strsize(code);

    જો (printit)
    {
	સંદેશ_puts((અક્ષર *)IObuff);
	જો (code == ખાલી)
	    સંદેશ_puts("NULL");
	નહીંતર
	    સંદેશ_outtrans(code);
    }
    પાછા len;
}

#જો defined(FEAT_TERMRESPONSE)
/*
 * For Xterm >= 140 compiled with OPT_TCAP_QUERY: Obtain the actually used
 * termcap codes from the terminal itself.
 * We get them one by one to avoid a very long response string.
 */
સ્થિર પૂર્ણાંક xt_index_in = 0;
સ્થિર પૂર્ણાંક xt_index_out = 0;

    સ્થિર શૂન્ય
req_codes_from_term(શૂન્ય)
{
    xt_index_out = 0;
    xt_index_in = 0;
    req_more_codes_from_term();
}

    સ્થિર શૂન્ય
req_more_codes_from_term(શૂન્ય)
{
    અક્ષર	buf[32];  // extra size to shut up LGTM
    પૂર્ણાંક		old_idx = xt_index_out;

    // Don't do anything when going to exit.
    જો (exiting)
	પાછા;

    // Send up to 10 more requests out than we received.  Avoid sending too
    // many, there can be a buffer overflow somewhere.
    જ્યારે (xt_index_out < xt_index_in + 10 && ચાવી_names[xt_index_out] != ખાલી)
    {
	અક્ષર *ચાવી_name = ચાવી_names[xt_index_out];

	MAY_WANT_TO_LOG_THIS;
	LOG_TRN("Requesting XT %d: %s", xt_index_out, ચાવી_name);
	જો (ચાવી_name[2] != NUL)
	    sprintf(buf, "\033P+q%02x%02x%02x\033\\", ચાવી_name[0], ચાવી_name[1], ચાવી_name[2]);
	નહીંતર
	    sprintf(buf, "\033P+q%02x%02x\033\\", ચાવી_name[0], ચાવી_name[1]);
	out_str_nf((char_u *)buf);
	++xt_index_out;
    }

    // Send the codes out right away.
    જો (xt_index_out != old_idx)
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
    સ્થિર શૂન્ય
got_code_from_term(char_u *code, પૂર્ણાંક len)
{
# define XT_LEN 100
    char_u	name[4];
    char_u	str[XT_LEN];
    પૂર્ણાંક		i;
    પૂર્ણાંક		j = 0;
    પૂર્ણાંક		c;

    // A '1' means the code is supported, a '0' means it isn't.
    // If it is supported, there must be a '=' following
    // When half the length is > XT_LEN we can't use it.
    જો (code[0] == '1' && (code[7] == '=' || code[9] == '=') && len / 2 < XT_LEN)
    {
	// Get the name from the response and find it in the table.
	name[0] = hexhex2nr(code + 3);
	name[1] = hexhex2nr(code + 5);
	જો (code[9] == '=')
	    name[2] = hexhex2nr(code + 7);
	નહીંતર
	    name[2] = NUL;
	name[3] = NUL;
	માટે (i = 0; ચાવી_names[i] != ખાલી; ++i)
	{
	    જો (STRCMP(ચાવી_names[i], name) == 0)
	    {
		xt_index_in = i;
		તોડો;
	    }
	}

	LOG_TRN("Received XT %d: %s", xt_index_in, (અક્ષર *)name);

	જો (ચાવી_names[i] != ખાલી)
	{
	    i = (code[7] == '=') ? 8 : 10;
	    માટે (; (c = hexhex2nr(code + i)) >= 0; i += 2)
		str[j++] = c;
	    str[j] = NUL;
	    જો (name[0] == 'C' && name[1] == 'o')
	    {
		// Color count is not a key code.
		પૂર્ણાંક val = atoi((અક્ષર *)str);
# જો defined(FEAT_EVAL)
		જો (val == t_colors)
		    ch_log(ખાલી, "got_code_from_term(Co): no change (%d)", val);
		નહીંતર
		    ch_log(ખાલી,
			       "got_code_from_term(Co): changed from %d to %d",
								t_colors, val);
# endif
		may_adjust_color_ગણતરી(val);
	    }
# જો 0
#  ifdef FEAT_TERMGUICOLORS
	    // when RGB result comes back, it is supported when the result contains an '='
	    નહીંતર જો (name[0] == 'R' && name[1] == 'G' && name[2] == 'B' && code[9] == '=')
	    {
		પૂર્ણાંક val = atoi((અક્ષર *)str);
		// only enable it, if termguicolors hasn't been set yet and
		// there are 8 bits per color channel
		જો (val == 8 && !p_tgc_set)
		{
#   ifdef FEAT_EVAL
		    ch_log(ખાલી, "got_code_from_term(RGB): xterm-direct colors detected");
#   endif
		    // RGB capability set, enable termguicolors
		    set_option_કિંમત((char_u *)"termguicolors", 1L, ખાલી, 0);
		}
	    }
#  endif
# endif
	    નહીંતર
	    {
		i = find_term_bykeys(str, ખાલી);
		જો (i >= 0 && name[0] == termcodes[i].name[0]
					    && name[1] == termcodes[i].name[1])
		{
		    // Existing entry with the same name and code - skip.
# ifdef FEAT_EVAL
		    ch_log(ખાલી, "got_code_from_term(): Entry %c%c did not change",
							     name[0], name[1]);
# endif
		}
		નહીંતર જો (i >= 0 && name[0] == 'K' && VIM_ISDIGIT(name[1]))
		{
		    // Would replace existing entry with keypad key - skip.
# ifdef FEAT_EVAL
		    ch_log(ખાલી, "got_code_from_term(): Skipping entry %c%c in favor of %c%c with matching keys %s",
			    name[0], name[1], termcodes[i].name[0], termcodes[i].name[1], str);
# endif
		}
		નહીંતર
		{
		    જો (i >= 0)
		    {
			// Delete an existing entry using the same code.
# ifdef FEAT_EVAL
			ch_log(ખાલી, "got_code_from_term(): Deleting entry %c%c with matching keys %s",
			      termcodes[i].name[0], termcodes[i].name[1], str);
# endif
			del_termcode_idx(i);
		    }
# ifdef FEAT_EVAL
		    નહીંતર
			ch_log(ખાલી, "got_code_from_term(): Adding entry %c%c with keys %s",
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
    સ્થિર શૂન્ય
check_for_codes_from_term(શૂન્ય)
{
    પૂર્ણાંક		c;

    // If no codes requested or all are answered, no need to wait.
    જો (xt_index_out == 0 || xt_index_out == xt_index_in)
	પાછા;

    // Vgetc() will check for and handle any response.
    // Keep calling vpeekc() until we don't get any responses.
    ++no_mapping;
    ++allow_ચાવીઓ;
    માટે (;;)
    {
	c = vpeekc();
	જો (c == NUL)	    // nothing available
	    તોડો;

	// If a response is recognized it's replaced with K_IGNORE, must read
	// it from the input stream.  If there is no K_IGNORE we can't do
	// anything, break here (there might be some responses further on, but
	// we don't want to throw away any typed chars).
	જો (c != K_SPECIAL && c != K_IGNORE)
	    તોડો;
	c = vgetc();
	જો (c != K_IGNORE)
	{
	    vungetc(c);
	    તોડો;
	}
    }
    --no_mapping;
    --allow_ચાવીઓ;
}
#endif

#જો defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
સ્થિર અક્ષર ksme_str[20];
સ્થિર અક્ષર ksmr_str[20];
સ્થિર અક્ષર ksmd_str[20];

/*
 * For Win32 console: update termcap codes for existing console attributes.
 */
    શૂન્ય
update_tcap(પૂર્ણાંક attr)
{
    sprintf(ksme_str, "\033|%dm", attr);
    sprintf(ksmd_str, "\033|%dm", attr | 0x08);  // FOREGROUND_INTENSITY
    sprintf(ksmr_str, "\033|%dm", ((attr & 0x0F) << 4) | ((attr & 0xF0) >> 4));

    tcap_entry_T *p = find_builtin_term(DEFAULT_TERM);
    જો (p == ખાલી)  // did not find it
	પાછા;
    જ્યારે (p->bt_string != ખાલી)
    {
	જો (p->bt_entry == (પૂર્ણાંક)KS_ME)
	    p->bt_string = &ksme_str[0];
	નહીંતર જો (p->bt_entry == (પૂર્ણાંક)KS_MR)
	    p->bt_string = &ksmr_str[0];
	નહીંતર જો (p->bt_entry == (પૂર્ણાંક)KS_MD)
	    p->bt_string = &ksmd_str[0];
	++p;
    }
}

# ifdef FEAT_TERMGUICOLORS
#  define KSSIZE 20

પ્રકારવ્યાખ્યા ગણના
{
    CMODE_INDEXED = 0,	// Use cmd.exe 4bit palette.
    CMODE_RGB,		// Use 24bit RGB colors using VTP.
    CMODE_256COL,	// Emulate xterm's 256-color palette using VTP.
    CMODE_LAST,
} cmode_T;

સંરચના ks_tbl_S
{
    પૂર્ણાંક  code;				// value of KS_
    અક્ષર *vtp;				// code in RGB mode
    અક્ષર *vtp2;				// code in 256color mode
    અક્ષર buf[CMODE_LAST][KSSIZE];	// real buffer
};

સ્થિર સંરચના ks_tbl_S ks_tbl[] =
{
    {(પૂર્ણાંક)KS_ME,  "\033|0m",  "\033|0m", {""}},   // normal
    {(પૂર્ણાંક)KS_MR,  "\033|7m",  "\033|7m", {""}},   // reverse
    {(પૂર્ણાંક)KS_MD,  "\033|1m",  "\033|1m", {""}},   // bold
    {(પૂર્ણાંક)KS_SO,  "\033|91m", "\033|91m", {""}},  // standout: bright red text
    {(પૂર્ણાંક)KS_SE,  "\033|39m", "\033|39m", {""}},  // standout end: default color
    {(પૂર્ણાંક)KS_CZH, "\033|3m",  "\033|3m", {""}},   // italic
    {(પૂર્ણાંક)KS_CZR, "\033|0m",  "\033|0m", {""}},   // italic end
    {(પૂર્ણાંક)KS_US,  "\033|4m",  "\033|4m", {""}},   // underscore
    {(પૂર્ણાંક)KS_UE,  "\033|24m", "\033|24m", {""}},  // underscore end
#  ifdef TERMINFO
    {(પૂર્ણાંક)KS_CAB, "\033|%p1%db", "\033|%p14%dm", {""}}, // set background color
    {(પૂર્ણાંક)KS_CAF, "\033|%p1%df", "\033|%p13%dm", {""}}, // set foreground color
    {(પૂર્ણાંક)KS_CS,  "\033|%p1%d;%p2%dR", "\033|%p1%d;%p2%dR", {""}},
    {(પૂર્ણાંક)KS_CSV, "\033|%p1%d;%p2%dV", "\033|%p1%d;%p2%dV", {""}},
#  નહીંતર
    {(પૂર્ણાંક)KS_CAB, "\033|%db", "\033|4%dm", {""}}, // set background color
    {(પૂર્ણાંક)KS_CAF, "\033|%df", "\033|3%dm", {""}}, // set foreground color
    {(પૂર્ણાંક)KS_CS,  "\033|%d;%dR", "\033|%d;%dR", {""}},
    {(પૂર્ણાંક)KS_CSV, "\033|%d;%dV", "\033|%d;%dV", {""}},
#  endif
    {(પૂર્ણાંક)KS_CCO, "256", "256", {""}},	    // colors
    {(પૂર્ણાંક)KS_NAME, ખાલી, ખાલી, {""}}			    // terminator
};

/*
 * Find the first entry for "code" in the builtin termcap for "name".
 * Returns NULL when not found.
 */
    સ્થિર tcap_entry_T *
find_first_tcap(
    char_u *name,
    પૂર્ણાંક	    code)
{
    tcap_entry_T *p = find_builtin_term(name);
    જો (p == ખાલી)
	પાછા ખાલી;
    જ્યારે (p->bt_string != ખાલી)
    {
	જો (p->bt_entry == code)
	    પાછા p;
	++p;
    }
    પાછા ખાલી;
}
# endif

/*
 * For Win32 console: replace the sequence immediately after termguicolors.
 */
    શૂન્ય
swap_tcap(શૂન્ય)
{
# ifdef FEAT_TERMGUICOLORS
    સ્થિર પૂર્ણાંક		init_done = FALSE;
    સ્થિર cmode_T	curr_mode;
    સંરચના ks_tbl_S	*ks;
    cmode_T		mode;

    જો (!init_done)
    {
	માટે (ks = ks_tbl; ks->code != (પૂર્ણાંક)KS_NAME; ks++)
	{
	    tcap_entry_T *bt = find_first_tcap(DEFAULT_TERM, ks->code);
	    જો (bt != ખાલી)
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

    જો (p_tgc)
	mode = CMODE_RGB;
    નહીંતર જો (t_colors >= 256)
	mode = CMODE_256COL;
    નહીંતર
	mode = CMODE_INDEXED;

    જો (mode == curr_mode)
	પાછા;

    માટે (ks = ks_tbl; ks->code != (પૂર્ણાંક)KS_NAME; ks++)
    {
	tcap_entry_T *bt = find_first_tcap(DEFAULT_TERM, ks->code);
	જો (bt != ખાલી)
	    bt->bt_string = ks->buf[mode];
    }

    curr_mode = mode;
# endif
}

#endif


#જો (defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))) || defined(FEAT_TERMINAL)
સ્થિર અચલ પૂર્ણાંક cube_કિંમત[] = {
    0x00, 0x5F, 0x87, 0xAF, 0xD7, 0xFF
};

સ્થિર અચલ પૂર્ણાંક grey_ramp[] = {
    0x08, 0x12, 0x1C, 0x26, 0x30, 0x3A, 0x44, 0x4E, 0x58, 0x62, 0x6C, 0x76,
    0x80, 0x8A, 0x94, 0x9E, 0xA8, 0xB2, 0xBC, 0xC6, 0xD0, 0xDA, 0xE4, 0xEE
};

સ્થિર અચલ char_u ansi_table[16][3] = {
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

# જો defined(MSWIN)
// Mapping between cterm indices < 16 and their counterpart in the ANSI palette.
અચલ char_u cterm_ansi_idx[] = {
    0, 4, 2, 6, 1, 5, 3, 7, 8, 12, 10, 14, 9, 13, 11, 15
};
# endif

# define ANSI_INDEX_NONE 0

    શૂન્ય
ansi_color2rgb(પૂર્ણાંક nr, char_u *r, char_u *g, char_u *b, char_u *ansi_idx)
{
    જો (nr < 16)
    {
	*r = ansi_table[nr][0];
	*g = ansi_table[nr][1];
	*b = ansi_table[nr][2];
	*ansi_idx = nr + 1;
    }
    નહીંતર
    {
	*r = 0;
	*g = 0;
	*b = 0;
	*ansi_idx = ANSI_INDEX_NONE;
    }
}

    શૂન્ય
cterm_color2rgb(પૂર્ણાંક nr, char_u *r, char_u *g, char_u *b, char_u *ansi_idx)
{
    પૂર્ણાંક idx;

    જો (nr < 16)
    {
# જો defined(MSWIN)
	idx = cterm_ansi_idx[nr];
# નહીંતર
	idx = nr;
# endif
	*r = ansi_table[idx][0];
	*g = ansi_table[idx][1];
	*b = ansi_table[idx][2];
	*ansi_idx = idx + 1;
    }
    નહીંતર જો (nr < 232)
    {
	// 216 color cube
	idx = nr - 16;
	*r = cube_કિંમત[idx / 36 % 6];
	*g = cube_કિંમત[idx / 6  % 6];
	*b = cube_કિંમત[idx      % 6];
	*ansi_idx = ANSI_INDEX_NONE;
    }
    નહીંતર જો (nr < 256)
    {
	// 24 grey scale ramp
	idx = nr - 232;
	*r = grey_ramp[idx];
	*g = grey_ramp[idx];
	*b = grey_ramp[idx];
	*ansi_idx = ANSI_INDEX_NONE;
    }
    નહીંતર
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
    પૂર્ણાંક
term_replace_keycodes(char_u *ta_buf, પૂર્ણાંક ta_len, પૂર્ણાંક len_arg)
{
    પૂર્ણાંક		len = len_arg;
    પૂર્ણાંક		i;
    પૂર્ણાંક		c;

    માટે (i = ta_len; i < ta_len + len; ++i)
    {
	જો (ta_buf[i] == CSI && len - i > 3 && ta_buf[i + 1] == KS_MODIFIER)
	{
	    પૂર્ણાંક modifiers = ta_buf[i + 2];
	    પૂર્ણાંક ચાવી = ta_buf[i + 3];

	    // Try to use the modifier to modify the key.  In any case drop the
	    // modifier.
	    mch_memmove(ta_buf + i + 1, ta_buf + i + 4, (size_t)(len - i - 3));
	    len -= 3;
	    જો (ચાવી < 0x80)
		ચાવી = merge_modify_Other_ચાવીઓ(ચાવી, &modifiers);
	    ta_buf[i] = ચાવી;
	}
	નહીંતર જો (ta_buf[i] == CSI && len - i > 2)
	{
	    c = TERMCAP2_ચાવી(ta_buf[i + 1], ta_buf[i + 2]);
	    જો (c == K_DEL || c == K_KDEL || c == K_BS)
	    {
		mch_memmove(ta_buf + i + 1, ta_buf + i + 3,
							(size_t)(len - i - 2));
		જો (c == K_DEL || c == K_KDEL)
		    ta_buf[i] = DEL;
		નહીંતર
		    ta_buf[i] = Ctrl_H;
		len -= 2;
	    }
	}
	નહીંતર જો (ta_buf[i] == '\r')
	    ta_buf[i] = '\n';
	જો (has_mbyte)
	    i += (*mb_ptr2len_len)(ta_buf + i, ta_len + len - i) - 1;
    }
    પાછા len;
}

/*
 * Should be called when cleaning up terminal state.
 */
    શૂન્ય
term_disable_dec(શૂન્ય)
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
    શૂન્ય
term_set_win_resize(bool state)
{
# ifdef FEAT_GUI
    bool    in_gui = gui.in_use;

    જો (state && in_gui)
	પાછા;
# endif

    જો (!state || win_resize_setting == 0 || win_resize_setting == 4)
    {
	// Make sure it update internal window size if DEC mode 2048 is
	// unavailable now.
	જો (win_resize_enabled)
	{
	    set_shellsize(0, 0, false);
	    set_sigwinch_handler();
	    out_str((char_u *)"\033[?2048l");
	}
	win_resize_enabled = false;
    }
    નહીંતર જો ((*p_trz == NUL || STRCMP(p_trz, "inband") == 0)
	    && !win_resize_enabled)
    {
	જો (win_resize_setting == 2)
	    out_str((char_u *)"\033[?2048h");
# ifdef SIGWINCH
	mch_signal(SIGWINCH, SIG_DFL);
# endif
	win_resize_enabled = true;
    }
}
#endif

    પૂર્ણાંક
sync_output_active(શૂન્ય)
{
#ifdef FEAT_GUI
    જો (gui.in_use)
	પાછા TRUE;
#endif
    પાછા p_tsy && (sync_output_setting == 1 || sync_output_setting == 2)
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
    શૂન્ય
term_set_sync_output(પૂર્ણાંક flags)
{
    bool    allowed;
#ifdef FEAT_GUI
    bool    in_gui = gui.in_use;
#નહીંતર
    bool    in_gui = false;
#endif

    allowed = p_tsy && (sync_output_setting == 1 || sync_output_setting == 2);

    જો (flags & TERM_SYNC_OUTPUT_FLUSH)
    {
	// Tell terminal to display screen contents, then resume batching.
	જો (allowed && !in_gui && sync_output_state > 0 && *T_ESU != NUL &&
		*T_BSU != NUL)
	{
	    out_str_nf(T_ESU);
	    out_str_nf(T_BSU);
	    out_flush();
	}
	પાછા;
    }

    // Forcibly turn off synchronized output (e.g. 'notermsync')
    જો (flags & TERM_SYNC_OUTPUT_OFF)
    {
	જો (sync_output_state > 0 && *T_ESU != NUL)
	{
	    out_str_nf(T_ESU);
	    out_flush();
	    sync_output_state = 0;
	}
	પાછા;
    }

    જો (!allowed || in_gui || *T_BSU == NUL || *T_ESU == NUL)
	પાછા;

    // Only enable if we aren't already, and only disable if we have reached
    // zero.
    જો (flags & TERM_SYNC_OUTPUT_ENABLE)
    {
	જો (sync_output_state++ > 0)
	    પાછા;
	out_str_nf(T_BSU);
    }
    નહીંતર જો (flags & TERM_SYNC_OUTPUT_DISABLE)
    {
	જો (sync_output_state == 0 || --sync_output_state > 0)
	    પાછા;
	// Flush the output buffer before ending the sync batch so that
	// all drawing output is sent to the terminal within the
	// BSU..ESU window.  Without this, the drawing data remaining in
	// out_buf would be sent after ESU, outside the sync batch.
	out_str_nf(T_ESU);
	out_flush();
    }
    નહીંતર
    {
	siemsg("Unknown sync output value %d", flags);
	પાછા;
    }
}
