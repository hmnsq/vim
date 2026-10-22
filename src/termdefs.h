/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

/*
 * This file contains the defines for the machine dependent escape sequences
 * that the editor needs to perform various operations. All of the sequences
 * here are optional, except "cm" (cursor motion).
 */

#اگر defined(SASC) && SASC < 658
/*
 * The SAS C compiler has a bug that makes typedefs being forgotten
 * in include files.  Has been fixed in version 6.58.
 */
قسمتعریف غیرعلامتی حرف char_u;
#اگر_ختم

/*
 * Index of the termcap codes in the term_strings array.
 */
شمار Special_کلید
{
    KS_NAME = 0,// name of this terminal entry
    KS_CE,	// clear to end of line
    KS_AL,	// add new blank line
    KS_CAL,	// add number of blank lines
    KS_DL,	// delete line
    KS_CDL,	// delete number of lines
    KS_CS,	// scroll region
    KS_CL,	// clear screen
    KS_CD,	// clear to end of display
    KS_UT,	// clearing uses current background color
    KS_DA,	// text may be scrolled down from up
    KS_DB,	// text may be scrolled up from down
    KS_VI,	// cursor invisible
    KS_VE,	// cursor visible
    KS_VS,	// cursor very visible (blink)
    KS_CVS,	// cursor normally visible (no blink)
    KS_CSH,	// cursor shape
    KS_چکری_جانچ,	// request cursor blinking
    KS_CRS,	// request cursor style
    KS_ME,	// normal mode
    KS_MR,	// reverse mode
    KS_MD,	// bold mode
    KS_SE,	// normal mode
    KS_SO,	// standout mode
    KS_CZH,	// italic mode start
    KS_CZR,	// italic mode end
    KS_UE,	// exit underscore (underline) mode
    KS_US,	// underscore (underline) mode
    KS_UCE,	// exit undercurl mode
    KS_UCS,	// undercurl mode
    KS_USS,	// double underline mode
    KS_DS,	// dotted underline mode
    KS_CDS,	// dashed underline mode
    KS_STE,	// exit strikethrough mode
    KS_STS,	// strikethrough mode
    KS_MS,	// save to move cur in reverse mode
    KS_CM,	// cursor motion
    KS_SR,	// scroll reverse (backward)
    KS_CRI,	// cursor number of chars right
    KS_VB,	// visual bell
    KS_KS,	// put term in "keypad transmit" mode
    KS_KE,	// out of "keypad transmit" mode
    KS_TI,	// put terminal in termcap mode
    KS_CTI,	// put terminal in "raw" mode
    KS_CRK,	// request keyboard protocol state
    KS_TE,	// end of termcap mode
    KS_CTE,	// end of "raw" mode
    KS_BC,	// backspace character (cursor left)
    KS_CCS,	// cur is relative to scroll region
    KS_CCO,	// number of colors
    KS_CSF,	// set foreground color
    KS_CSB,	// set background color
    KS_XS,	// standout not erased by overwriting (hpterm)
    KS_XN,	// newline glitch
    KS_MB,	// blink mode
    KS_CAF,	// set foreground color (ANSI)
    KS_CAB,	// set background color (ANSI)
    KS_CAU,	// set underline color (ANSI)
    KS_LE,	// cursor left (mostly backspace)
    KS_ND,	// cursor right
    KS_CIS,	// set icon text start
    KS_CIE,	// set icon text end
    KS_CSC,	// set cursor color start
    KS_CEC,	// set cursor color end
    KS_TS,	// set window title start (to status line)
    KS_FS,	// set window title end (from status line)
    KS_CWP,	// set window position in pixels
    KS_CGP,	// get window position
    KS_CWS,	// set window size in characters
    KS_CRV,	// request version string
    KS_CXM,	// enable/disable mouse reporting
    KS_RFG,	// request foreground color
    KS_RBG,	// request background color
    KS_CSI,	// start insert mode (bar cursor)
    KS_CEI,	// end insert mode (block cursor)
    KS_CSR,	// start replace mode (underline cursor)
    KS_CSV,	// scroll region vertical
    KS_OP,	// original color pair
    KS_U7,	// request cursor position
    KS_8F,	// set foreground color (RGB)
    KS_8B,	// set background color (RGB)
    KS_8U,	// set underline color (RGB)
    KS_CBE,	// enable bracketed paste mode
    KS_CBD,	// disable bracketed paste mode
    KS_CST,	// save window title
    KS_CRT,	// restore window title
    KS_SSI,	// save icon text
    KS_SRI,	// restore icon text
    KS_FD,	// disable focus event tracking
    KS_FE,	// enable focus event tracking
    KS_CF,	// set terminal alternate font
    KS_XON,	// terminal uses xon/xoff handshaking
    KS_BSU,	// begin synchronized update
    KS_ESU	// end synchronized update
};

#تعریف_کرو KS_آخری	    KS_ESU

/*
 * the terminal capabilities are stored in this array
 * IMPORTANT: When making changes, note the following:
 * - there should be an entry for each code in the builtin termcaps
 * - there should be an option for each code in option.c
 * - there should be code in term.c to obtain the value from the termcap
 */

خارجی char_u *(رکن_لڑیاں[]);    // current terminal strings

/*
 * strings used for terminal
 */
#تعریف_کرو T_NAME	(رکن_STR(KS_NAME))	// terminal name
#تعریف_کرو T_CE	(رکن_STR(KS_CE))	// clear to end of line
#تعریف_کرو T_AL	(رکن_STR(KS_AL))	// add new blank line
#تعریف_کرو T_CAL	(رکن_STR(KS_CAL))	// add number of blank lines
#تعریف_کرو T_DL	(رکن_STR(KS_DL))	// delete line
#تعریف_کرو T_CDL	(رکن_STR(KS_CDL))	// delete number of lines
#تعریف_کرو T_CS	(رکن_STR(KS_CS))	// scroll region
#تعریف_کرو T_CSV	(رکن_STR(KS_CSV))	// scroll region vertical
#تعریف_کرو T_CL	(رکن_STR(KS_CL))	// clear screen
#تعریف_کرو T_CD	(رکن_STR(KS_CD))	// clear to end of display
#تعریف_کرو T_UT	(رکن_STR(KS_UT))	// clearing uses background color
#تعریف_کرو T_DA	(رکن_STR(KS_DA))	// text may be scrolled down from up
#تعریف_کرو T_DB	(رکن_STR(KS_DB))	// text may be scrolled up from down
#تعریف_کرو T_VI	(رکن_STR(KS_VI))	// cursor invisible
#تعریف_کرو T_VE	(رکن_STR(KS_VE))	// cursor visible
#تعریف_کرو T_VS	(رکن_STR(KS_VS))	// cursor very visible (blink)
#تعریف_کرو T_CVS	(رکن_STR(KS_CVS))	// cursor normally visible (no blink)
#تعریف_کرو T_CSH	(رکن_STR(KS_CSH))	// cursor shape
#تعریف_کرو T_چکری_جانچ	(رکن_STR(KS_چکری_جانچ))	// request cursor blinking
#تعریف_کرو T_CRS	(رکن_STR(KS_CRS))	// request cursor style
#تعریف_کرو T_ME	(رکن_STR(KS_ME))	// normal mode
#تعریف_کرو T_MR	(رکن_STR(KS_MR))	// reverse mode
#تعریف_کرو T_MD	(رکن_STR(KS_MD))	// bold mode
#تعریف_کرو T_SE	(رکن_STR(KS_SE))	// normal mode
#تعریف_کرو T_SO	(رکن_STR(KS_SO))	// standout mode
#تعریف_کرو T_CZH	(رکن_STR(KS_CZH))	// italic mode start
#تعریف_کرو T_CZR	(رکن_STR(KS_CZR))	// italic mode end
#تعریف_کرو T_UE	(رکن_STR(KS_UE))	// exit underscore (underline) mode
#تعریف_کرو T_US	(رکن_STR(KS_US))	// underscore (underline) mode
#تعریف_کرو T_UCE	(رکن_STR(KS_UCE))	// exit undercurl mode
#تعریف_کرو T_UCS	(رکن_STR(KS_UCS))	// undercurl mode
#تعریف_کرو T_USS	(رکن_STR(KS_USS))	// double underline mode
#تعریف_کرو T_DS	(رکن_STR(KS_DS))	// dotted underline mode
#تعریف_کرو T_CDS	(رکن_STR(KS_CDS))	// dashed underline mode
#تعریف_کرو T_STE	(رکن_STR(KS_STE))	// exit strikethrough mode
#تعریف_کرو T_STS	(رکن_STR(KS_STS))	// strikethrough mode
#تعریف_کرو T_MS	(رکن_STR(KS_MS))	// save to move cur in reverse mode
#تعریف_کرو T_CM	(رکن_STR(KS_CM))	// cursor motion
#تعریف_کرو T_SR	(رکن_STR(KS_SR))	// scroll reverse (backward)
#تعریف_کرو T_CRI	(رکن_STR(KS_CRI))	// cursor number of chars right
#تعریف_کرو T_VB	(رکن_STR(KS_VB))	// visual bell
#تعریف_کرو T_KS	(رکن_STR(KS_KS))	// put term in "keypad transmit" mode
#تعریف_کرو T_KE	(رکن_STR(KS_KE))	// out of "keypad transmit" mode
#تعریف_کرو T_TI	(رکن_STR(KS_TI))	// put terminal in termcap mode
#تعریف_کرو T_CTI	(رکن_STR(KS_CTI))	// put terminal in "raw" mode
#تعریف_کرو T_CRK	(رکن_STR(KS_CRK))	// request keyboard protocol status
#تعریف_کرو T_TE	(رکن_STR(KS_TE))	// end of termcap mode
#تعریف_کرو T_CTE	(رکن_STR(KS_CTE))	// end of "raw" mode
#تعریف_کرو T_BC	(رکن_STR(KS_BC))	// backspace character
#تعریف_کرو T_CCS	(رکن_STR(KS_CCS))	// cur is relative to scroll region
#تعریف_کرو T_CCO	(رکن_STR(KS_CCO))	// number of colors
#تعریف_کرو T_CSF	(رکن_STR(KS_CSF))	// set foreground color
#تعریف_کرو T_CSB	(رکن_STR(KS_CSB))	// set background color
#تعریف_کرو T_XS	(رکن_STR(KS_XS))	// standout not erased by overwriting
#تعریف_کرو T_XN	(رکن_STR(KS_XN))	// newline glitch
#تعریف_کرو T_MB	(رکن_STR(KS_MB))	// blink mode
#تعریف_کرو T_CAF	(رکن_STR(KS_CAF))	// set foreground color (ANSI)
#تعریف_کرو T_CAB	(رکن_STR(KS_CAB))	// set background color (ANSI)
#تعریف_کرو T_CAU	(رکن_STR(KS_CAU))	// set underline color (ANSI)
#تعریف_کرو T_CFO	(رکن_STR(KS_CF))	// set alternate font
#تعریف_کرو T_LE	(رکن_STR(KS_LE))	// cursor left
#تعریف_کرو T_ND	(رکن_STR(KS_ND))	// cursor right
#تعریف_کرو T_CIS	(رکن_STR(KS_CIS))	// set icon text start
#تعریف_کرو T_CIE	(رکن_STR(KS_CIE))	// set icon text end
#تعریف_کرو T_TS	(رکن_STR(KS_TS))	// set window title start
#تعریف_کرو T_FS	(رکن_STR(KS_FS))	// set window title end
#تعریف_کرو T_CSC	(رکن_STR(KS_CSC))	// set cursor color start
#تعریف_کرو T_CEC	(رکن_STR(KS_CEC))	// set cursor color end
#تعریف_کرو T_CWP	(رکن_STR(KS_CWP))	// set window position
#تعریف_کرو T_CGP	(رکن_STR(KS_CGP))	// get window position
#تعریف_کرو T_CWS	(رکن_STR(KS_CWS))	// window size
#تعریف_کرو T_CSI	(رکن_STR(KS_CSI))	// start insert mode
#تعریف_کرو T_CEI	(رکن_STR(KS_CEI))	// end insert mode
#تعریف_کرو T_CSR	(رکن_STR(KS_CSR))	// start replace mode
#تعریف_کرو T_CRV	(رکن_STR(KS_CRV))	// request version string
#تعریف_کرو T_CXM	(رکن_STR(KS_CXM))	// enable/disable mouse reporting
#تعریف_کرو T_RFG	(رکن_STR(KS_RFG))	// request foreground RGB
#تعریف_کرو T_RBG	(رکن_STR(KS_RBG))	// request background RGB
#تعریف_کرو T_OP	(رکن_STR(KS_OP))	// original color pair
#تعریف_کرو T_U7	(رکن_STR(KS_U7))	// request cursor position
#تعریف_کرو T_8F	(رکن_STR(KS_8F))	// set foreground color (RGB)
#تعریف_کرو T_8B	(رکن_STR(KS_8B))	// set background color (RGB)
#تعریف_کرو T_8U	(رکن_STR(KS_8U))	// set underline color (RGB)
#تعریف_کرو T_BE	(رکن_STR(KS_CBE))	// enable bracketed paste mode
#تعریف_کرو T_BD	(رکن_STR(KS_CBD))	// disable bracketed paste mode
#تعریف_کرو T_CST	(رکن_STR(KS_CST))	// save window title
#تعریف_کرو T_CRT	(رکن_STR(KS_CRT))	// restore window title
#تعریف_کرو T_SSI	(رکن_STR(KS_SSI))	// save icon text
#تعریف_کرو T_SRI	(رکن_STR(KS_SRI))	// restore icon text
#تعریف_کرو T_FD	(رکن_STR(KS_FD))	// disable focus event tracking
#تعریف_کرو T_FE	(رکن_STR(KS_FE))	// enable focus event tracking
#تعریف_کرو T_XON	(رکن_STR(KS_XON))	// terminal uses xon/xoff handshaking
#تعریف_کرو T_BSU	(رکن_STR(KS_BSU))	// begin synchronized update
#تعریف_کرو T_ESU	(رکن_STR(KS_ESU))	// end synchronized update

قسمتعریف شمار {
    TMODE_COOK,	    // terminal mode for external cmds and Ex mode
    TMODE_SLEEP,    // terminal mode for sleeping (cooked but no echo)
    TMODE_RAW,	    // terminal mode for Normal and Insert mode
    TMODE_UNKNOWN   // after executing a shell
} tmode_T;

#اگر defined(MSWIN)
// Mapping between cterm indices < 16 and their counterpart in the ANSI palette.
خارجی ثابت char_u cterm_ansi_idx[];
#اگر_ختم
