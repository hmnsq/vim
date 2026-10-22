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

#إذا defined(SASC) && SASC < 658
/*
 * The SAS C compiler has a bug that makes typedefs being forgotten
 * in include files.  Has been fixed in version 6.58.
 */
تعريفنوع بینښه حرف char_u;
#د_که_پای

/*
 * Index of the termcap codes in the term_strings array.
 */
تعداد Special_کلی
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
    KS_دوراني_ازموینه,	// request cursor blinking
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

#تعریف_کړه KS_وروستی	    KS_ESU

/*
 * the terminal capabilities are stored in this array
 * IMPORTANT: When making changes, note the following:
 * - there should be an entry for each code in the builtin termcaps
 * - there should be an option for each code in option.c
 * - there should be code in term.c to obtain the value from the termcap
 */

خارجي char_u *(غړی_مزي_ګانې[]);    // current terminal strings

/*
 * strings used for terminal
 */
#تعریف_کړه T_NAME	(غړی_STR(KS_NAME))	// terminal name
#تعریف_کړه T_CE	(غړی_STR(KS_CE))	// clear to end of line
#تعریف_کړه T_AL	(غړی_STR(KS_AL))	// add new blank line
#تعریف_کړه T_CAL	(غړی_STR(KS_CAL))	// add number of blank lines
#تعریف_کړه T_DL	(غړی_STR(KS_DL))	// delete line
#تعریف_کړه T_CDL	(غړی_STR(KS_CDL))	// delete number of lines
#تعریف_کړه T_CS	(غړی_STR(KS_CS))	// scroll region
#تعریف_کړه T_CSV	(غړی_STR(KS_CSV))	// scroll region vertical
#تعریف_کړه T_CL	(غړی_STR(KS_CL))	// clear screen
#تعریف_کړه T_CD	(غړی_STR(KS_CD))	// clear to end of display
#تعریف_کړه T_UT	(غړی_STR(KS_UT))	// clearing uses background color
#تعریف_کړه T_DA	(غړی_STR(KS_DA))	// text may be scrolled down from up
#تعریف_کړه T_DB	(غړی_STR(KS_DB))	// text may be scrolled up from down
#تعریف_کړه T_VI	(غړی_STR(KS_VI))	// cursor invisible
#تعریف_کړه T_VE	(غړی_STR(KS_VE))	// cursor visible
#تعریف_کړه T_VS	(غړی_STR(KS_VS))	// cursor very visible (blink)
#تعریف_کړه T_CVS	(غړی_STR(KS_CVS))	// cursor normally visible (no blink)
#تعریف_کړه T_CSH	(غړی_STR(KS_CSH))	// cursor shape
#تعریف_کړه T_دوراني_ازموینه	(غړی_STR(KS_دوراني_ازموینه))	// request cursor blinking
#تعریف_کړه T_CRS	(غړی_STR(KS_CRS))	// request cursor style
#تعریف_کړه T_ME	(غړی_STR(KS_ME))	// normal mode
#تعریف_کړه T_MR	(غړی_STR(KS_MR))	// reverse mode
#تعریف_کړه T_MD	(غړی_STR(KS_MD))	// bold mode
#تعریف_کړه T_SE	(غړی_STR(KS_SE))	// normal mode
#تعریف_کړه T_SO	(غړی_STR(KS_SO))	// standout mode
#تعریف_کړه T_CZH	(غړی_STR(KS_CZH))	// italic mode start
#تعریف_کړه T_CZR	(غړی_STR(KS_CZR))	// italic mode end
#تعریف_کړه T_UE	(غړی_STR(KS_UE))	// exit underscore (underline) mode
#تعریف_کړه T_US	(غړی_STR(KS_US))	// underscore (underline) mode
#تعریف_کړه T_UCE	(غړی_STR(KS_UCE))	// exit undercurl mode
#تعریف_کړه T_UCS	(غړی_STR(KS_UCS))	// undercurl mode
#تعریف_کړه T_USS	(غړی_STR(KS_USS))	// double underline mode
#تعریف_کړه T_DS	(غړی_STR(KS_DS))	// dotted underline mode
#تعریف_کړه T_CDS	(غړی_STR(KS_CDS))	// dashed underline mode
#تعریف_کړه T_STE	(غړی_STR(KS_STE))	// exit strikethrough mode
#تعریف_کړه T_STS	(غړی_STR(KS_STS))	// strikethrough mode
#تعریف_کړه T_MS	(غړی_STR(KS_MS))	// save to move cur in reverse mode
#تعریف_کړه T_CM	(غړی_STR(KS_CM))	// cursor motion
#تعریف_کړه T_SR	(غړی_STR(KS_SR))	// scroll reverse (backward)
#تعریف_کړه T_CRI	(غړی_STR(KS_CRI))	// cursor number of chars right
#تعریف_کړه T_VB	(غړی_STR(KS_VB))	// visual bell
#تعریف_کړه T_KS	(غړی_STR(KS_KS))	// put term in "keypad transmit" mode
#تعریف_کړه T_KE	(غړی_STR(KS_KE))	// out of "keypad transmit" mode
#تعریف_کړه T_TI	(غړی_STR(KS_TI))	// put terminal in termcap mode
#تعریف_کړه T_CTI	(غړی_STR(KS_CTI))	// put terminal in "raw" mode
#تعریف_کړه T_CRK	(غړی_STR(KS_CRK))	// request keyboard protocol status
#تعریف_کړه T_TE	(غړی_STR(KS_TE))	// end of termcap mode
#تعریف_کړه T_CTE	(غړی_STR(KS_CTE))	// end of "raw" mode
#تعریف_کړه T_BC	(غړی_STR(KS_BC))	// backspace character
#تعریف_کړه T_CCS	(غړی_STR(KS_CCS))	// cur is relative to scroll region
#تعریف_کړه T_CCO	(غړی_STR(KS_CCO))	// number of colors
#تعریف_کړه T_CSF	(غړی_STR(KS_CSF))	// set foreground color
#تعریف_کړه T_CSB	(غړی_STR(KS_CSB))	// set background color
#تعریف_کړه T_XS	(غړی_STR(KS_XS))	// standout not erased by overwriting
#تعریف_کړه T_XN	(غړی_STR(KS_XN))	// newline glitch
#تعریف_کړه T_MB	(غړی_STR(KS_MB))	// blink mode
#تعریف_کړه T_CAF	(غړی_STR(KS_CAF))	// set foreground color (ANSI)
#تعریف_کړه T_CAB	(غړی_STR(KS_CAB))	// set background color (ANSI)
#تعریف_کړه T_CAU	(غړی_STR(KS_CAU))	// set underline color (ANSI)
#تعریف_کړه T_CFO	(غړی_STR(KS_CF))	// set alternate font
#تعریف_کړه T_LE	(غړی_STR(KS_LE))	// cursor left
#تعریف_کړه T_ND	(غړی_STR(KS_ND))	// cursor right
#تعریف_کړه T_CIS	(غړی_STR(KS_CIS))	// set icon text start
#تعریف_کړه T_CIE	(غړی_STR(KS_CIE))	// set icon text end
#تعریف_کړه T_TS	(غړی_STR(KS_TS))	// set window title start
#تعریف_کړه T_FS	(غړی_STR(KS_FS))	// set window title end
#تعریف_کړه T_CSC	(غړی_STR(KS_CSC))	// set cursor color start
#تعریف_کړه T_CEC	(غړی_STR(KS_CEC))	// set cursor color end
#تعریف_کړه T_CWP	(غړی_STR(KS_CWP))	// set window position
#تعریف_کړه T_CGP	(غړی_STR(KS_CGP))	// get window position
#تعریف_کړه T_CWS	(غړی_STR(KS_CWS))	// window size
#تعریف_کړه T_CSI	(غړی_STR(KS_CSI))	// start insert mode
#تعریف_کړه T_CEI	(غړی_STR(KS_CEI))	// end insert mode
#تعریف_کړه T_CSR	(غړی_STR(KS_CSR))	// start replace mode
#تعریف_کړه T_CRV	(غړی_STR(KS_CRV))	// request version string
#تعریف_کړه T_CXM	(غړی_STR(KS_CXM))	// enable/disable mouse reporting
#تعریف_کړه T_RFG	(غړی_STR(KS_RFG))	// request foreground RGB
#تعریف_کړه T_RBG	(غړی_STR(KS_RBG))	// request background RGB
#تعریف_کړه T_OP	(غړی_STR(KS_OP))	// original color pair
#تعریف_کړه T_U7	(غړی_STR(KS_U7))	// request cursor position
#تعریف_کړه T_8F	(غړی_STR(KS_8F))	// set foreground color (RGB)
#تعریف_کړه T_8B	(غړی_STR(KS_8B))	// set background color (RGB)
#تعریف_کړه T_8U	(غړی_STR(KS_8U))	// set underline color (RGB)
#تعریف_کړه T_BE	(غړی_STR(KS_CBE))	// enable bracketed paste mode
#تعریف_کړه T_BD	(غړی_STR(KS_CBD))	// disable bracketed paste mode
#تعریف_کړه T_CST	(غړی_STR(KS_CST))	// save window title
#تعریف_کړه T_CRT	(غړی_STR(KS_CRT))	// restore window title
#تعریف_کړه T_SSI	(غړی_STR(KS_SSI))	// save icon text
#تعریف_کړه T_SRI	(غړی_STR(KS_SRI))	// restore icon text
#تعریف_کړه T_FD	(غړی_STR(KS_FD))	// disable focus event tracking
#تعریف_کړه T_FE	(غړی_STR(KS_FE))	// enable focus event tracking
#تعریف_کړه T_XON	(غړی_STR(KS_XON))	// terminal uses xon/xoff handshaking
#تعریف_کړه T_BSU	(غړی_STR(KS_BSU))	// begin synchronized update
#تعریف_کړه T_ESU	(غړی_STR(KS_ESU))	// end synchronized update

تعريفنوع تعداد {
    TMODE_COOK,	    // terminal mode for external cmds and Ex mode
    TMODE_SLEEP,    // terminal mode for sleeping (cooked but no echo)
    TMODE_RAW,	    // terminal mode for Normal and Insert mode
    TMODE_UNKNOWN   // after executing a shell
} tmode_T;

#إذا defined(MSWIN)
// Mapping between cterm indices < 16 and their counterpart in the ANSI palette.
خارجي ثابت char_u cterm_ansi_idx[];
#د_که_پای
