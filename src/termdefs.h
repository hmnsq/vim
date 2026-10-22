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
تعريفنوع بیعلامت حرف char_u;
#پایان_اگر

/*
 * Index of the termcap codes in the term_strings array.
 */
تعداد Special_کلید
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
    KS_بررسی_چرخهای,	// request cursor blinking
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

#تعریف_کن KS_آخرین	    KS_ESU

/*
 * the terminal capabilities are stored in this array
 * IMPORTANT: When making changes, note the following:
 * - there should be an entry for each code in the builtin termcaps
 * - there should be an option for each code in option.c
 * - there should be code in term.c to obtain the value from the termcap
 */

خارجي char_u *(جمله_رشتهها[]);    // current terminal strings

/*
 * strings used for terminal
 */
#تعریف_کن T_NAME	(جمله_STR(KS_NAME))	// terminal name
#تعریف_کن T_CE	(جمله_STR(KS_CE))	// clear to end of line
#تعریف_کن T_AL	(جمله_STR(KS_AL))	// add new blank line
#تعریف_کن T_CAL	(جمله_STR(KS_CAL))	// add number of blank lines
#تعریف_کن T_DL	(جمله_STR(KS_DL))	// delete line
#تعریف_کن T_CDL	(جمله_STR(KS_CDL))	// delete number of lines
#تعریف_کن T_CS	(جمله_STR(KS_CS))	// scroll region
#تعریف_کن T_CSV	(جمله_STR(KS_CSV))	// scroll region vertical
#تعریف_کن T_CL	(جمله_STR(KS_CL))	// clear screen
#تعریف_کن T_CD	(جمله_STR(KS_CD))	// clear to end of display
#تعریف_کن T_UT	(جمله_STR(KS_UT))	// clearing uses background color
#تعریف_کن T_DA	(جمله_STR(KS_DA))	// text may be scrolled down from up
#تعریف_کن T_DB	(جمله_STR(KS_DB))	// text may be scrolled up from down
#تعریف_کن T_VI	(جمله_STR(KS_VI))	// cursor invisible
#تعریف_کن T_VE	(جمله_STR(KS_VE))	// cursor visible
#تعریف_کن T_VS	(جمله_STR(KS_VS))	// cursor very visible (blink)
#تعریف_کن T_CVS	(جمله_STR(KS_CVS))	// cursor normally visible (no blink)
#تعریف_کن T_CSH	(جمله_STR(KS_CSH))	// cursor shape
#تعریف_کن T_بررسی_چرخهای	(جمله_STR(KS_بررسی_چرخهای))	// request cursor blinking
#تعریف_کن T_CRS	(جمله_STR(KS_CRS))	// request cursor style
#تعریف_کن T_ME	(جمله_STR(KS_ME))	// normal mode
#تعریف_کن T_MR	(جمله_STR(KS_MR))	// reverse mode
#تعریف_کن T_MD	(جمله_STR(KS_MD))	// bold mode
#تعریف_کن T_SE	(جمله_STR(KS_SE))	// normal mode
#تعریف_کن T_SO	(جمله_STR(KS_SO))	// standout mode
#تعریف_کن T_CZH	(جمله_STR(KS_CZH))	// italic mode start
#تعریف_کن T_CZR	(جمله_STR(KS_CZR))	// italic mode end
#تعریف_کن T_UE	(جمله_STR(KS_UE))	// exit underscore (underline) mode
#تعریف_کن T_US	(جمله_STR(KS_US))	// underscore (underline) mode
#تعریف_کن T_UCE	(جمله_STR(KS_UCE))	// exit undercurl mode
#تعریف_کن T_UCS	(جمله_STR(KS_UCS))	// undercurl mode
#تعریف_کن T_USS	(جمله_STR(KS_USS))	// double underline mode
#تعریف_کن T_DS	(جمله_STR(KS_DS))	// dotted underline mode
#تعریف_کن T_CDS	(جمله_STR(KS_CDS))	// dashed underline mode
#تعریف_کن T_STE	(جمله_STR(KS_STE))	// exit strikethrough mode
#تعریف_کن T_STS	(جمله_STR(KS_STS))	// strikethrough mode
#تعریف_کن T_MS	(جمله_STR(KS_MS))	// save to move cur in reverse mode
#تعریف_کن T_CM	(جمله_STR(KS_CM))	// cursor motion
#تعریف_کن T_SR	(جمله_STR(KS_SR))	// scroll reverse (backward)
#تعریف_کن T_CRI	(جمله_STR(KS_CRI))	// cursor number of chars right
#تعریف_کن T_VB	(جمله_STR(KS_VB))	// visual bell
#تعریف_کن T_KS	(جمله_STR(KS_KS))	// put term in "keypad transmit" mode
#تعریف_کن T_KE	(جمله_STR(KS_KE))	// out of "keypad transmit" mode
#تعریف_کن T_TI	(جمله_STR(KS_TI))	// put terminal in termcap mode
#تعریف_کن T_CTI	(جمله_STR(KS_CTI))	// put terminal in "raw" mode
#تعریف_کن T_CRK	(جمله_STR(KS_CRK))	// request keyboard protocol status
#تعریف_کن T_TE	(جمله_STR(KS_TE))	// end of termcap mode
#تعریف_کن T_CTE	(جمله_STR(KS_CTE))	// end of "raw" mode
#تعریف_کن T_BC	(جمله_STR(KS_BC))	// backspace character
#تعریف_کن T_CCS	(جمله_STR(KS_CCS))	// cur is relative to scroll region
#تعریف_کن T_CCO	(جمله_STR(KS_CCO))	// number of colors
#تعریف_کن T_CSF	(جمله_STR(KS_CSF))	// set foreground color
#تعریف_کن T_CSB	(جمله_STR(KS_CSB))	// set background color
#تعریف_کن T_XS	(جمله_STR(KS_XS))	// standout not erased by overwriting
#تعریف_کن T_XN	(جمله_STR(KS_XN))	// newline glitch
#تعریف_کن T_MB	(جمله_STR(KS_MB))	// blink mode
#تعریف_کن T_CAF	(جمله_STR(KS_CAF))	// set foreground color (ANSI)
#تعریف_کن T_CAB	(جمله_STR(KS_CAB))	// set background color (ANSI)
#تعریف_کن T_CAU	(جمله_STR(KS_CAU))	// set underline color (ANSI)
#تعریف_کن T_CFO	(جمله_STR(KS_CF))	// set alternate font
#تعریف_کن T_LE	(جمله_STR(KS_LE))	// cursor left
#تعریف_کن T_ND	(جمله_STR(KS_ND))	// cursor right
#تعریف_کن T_CIS	(جمله_STR(KS_CIS))	// set icon text start
#تعریف_کن T_CIE	(جمله_STR(KS_CIE))	// set icon text end
#تعریف_کن T_TS	(جمله_STR(KS_TS))	// set window title start
#تعریف_کن T_FS	(جمله_STR(KS_FS))	// set window title end
#تعریف_کن T_CSC	(جمله_STR(KS_CSC))	// set cursor color start
#تعریف_کن T_CEC	(جمله_STR(KS_CEC))	// set cursor color end
#تعریف_کن T_CWP	(جمله_STR(KS_CWP))	// set window position
#تعریف_کن T_CGP	(جمله_STR(KS_CGP))	// get window position
#تعریف_کن T_CWS	(جمله_STR(KS_CWS))	// window size
#تعریف_کن T_CSI	(جمله_STR(KS_CSI))	// start insert mode
#تعریف_کن T_CEI	(جمله_STR(KS_CEI))	// end insert mode
#تعریف_کن T_CSR	(جمله_STR(KS_CSR))	// start replace mode
#تعریف_کن T_CRV	(جمله_STR(KS_CRV))	// request version string
#تعریف_کن T_CXM	(جمله_STR(KS_CXM))	// enable/disable mouse reporting
#تعریف_کن T_RFG	(جمله_STR(KS_RFG))	// request foreground RGB
#تعریف_کن T_RBG	(جمله_STR(KS_RBG))	// request background RGB
#تعریف_کن T_OP	(جمله_STR(KS_OP))	// original color pair
#تعریف_کن T_U7	(جمله_STR(KS_U7))	// request cursor position
#تعریف_کن T_8F	(جمله_STR(KS_8F))	// set foreground color (RGB)
#تعریف_کن T_8B	(جمله_STR(KS_8B))	// set background color (RGB)
#تعریف_کن T_8U	(جمله_STR(KS_8U))	// set underline color (RGB)
#تعریف_کن T_BE	(جمله_STR(KS_CBE))	// enable bracketed paste mode
#تعریف_کن T_BD	(جمله_STR(KS_CBD))	// disable bracketed paste mode
#تعریف_کن T_CST	(جمله_STR(KS_CST))	// save window title
#تعریف_کن T_CRT	(جمله_STR(KS_CRT))	// restore window title
#تعریف_کن T_SSI	(جمله_STR(KS_SSI))	// save icon text
#تعریف_کن T_SRI	(جمله_STR(KS_SRI))	// restore icon text
#تعریف_کن T_FD	(جمله_STR(KS_FD))	// disable focus event tracking
#تعریف_کن T_FE	(جمله_STR(KS_FE))	// enable focus event tracking
#تعریف_کن T_XON	(جمله_STR(KS_XON))	// terminal uses xon/xoff handshaking
#تعریف_کن T_BSU	(جمله_STR(KS_BSU))	// begin synchronized update
#تعریف_کن T_ESU	(جمله_STR(KS_ESU))	// end synchronized update

تعريفنوع تعداد {
    TMODE_COOK,	    // terminal mode for external cmds and Ex mode
    TMODE_SLEEP,    // terminal mode for sleeping (cooked but no echo)
    TMODE_RAW,	    // terminal mode for Normal and Insert mode
    TMODE_UNKNOWN   // after executing a shell
} tmode_T;

#إذا defined(MSWIN)
// Mapping between cterm indices < 16 and their counterpart in the ANSI palette.
خارجي ثابت char_u cterm_ansi_idx[];
#پایان_اگر
