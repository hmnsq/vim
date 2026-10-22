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

#ಆದರೆ defined(SASC) && SASC < 658
/*
 * The SAS C compiler has a bug that makes typedefs being forgotten
 * in include files.  Has been fixed in version 6.58.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ char_u;
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Index of the termcap codes in the term_strings array.
 */
ಎಣಿಕೆ Special_ಕೀಲಿ
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
    KS_ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ,	// request cursor blinking
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

#ವ್ಯಾಖ್ಯಾನಿಸು KS_ಕೊನೆಯ	    KS_ESU

/*
 * the terminal capabilities are stored in this array
 * IMPORTANT: When making changes, note the following:
 * - there should be an entry for each code in the builtin termcaps
 * - there should be an option for each code in option.c
 * - there should be code in term.c to obtain the value from the termcap
 */

ಬಾಹ್ಯ char_u *(ಪದ_ಅಕ್ಷರಸರಣಿಗಳು[]);    // current terminal strings

/*
 * strings used for terminal
 */
#ವ್ಯಾಖ್ಯಾನಿಸು T_NAME	(ಪದ_STR(KS_NAME))	// terminal name
#ವ್ಯಾಖ್ಯಾನಿಸು T_CE	(ಪದ_STR(KS_CE))	// clear to end of line
#ವ್ಯಾಖ್ಯಾನಿಸು T_AL	(ಪದ_STR(KS_AL))	// add new blank line
#ವ್ಯಾಖ್ಯಾನಿಸು T_CAL	(ಪದ_STR(KS_CAL))	// add number of blank lines
#ವ್ಯಾಖ್ಯಾನಿಸು T_DL	(ಪದ_STR(KS_DL))	// delete line
#ವ್ಯಾಖ್ಯಾನಿಸು T_CDL	(ಪದ_STR(KS_CDL))	// delete number of lines
#ವ್ಯಾಖ್ಯಾನಿಸು T_CS	(ಪದ_STR(KS_CS))	// scroll region
#ವ್ಯಾಖ್ಯಾನಿಸು T_CSV	(ಪದ_STR(KS_CSV))	// scroll region vertical
#ವ್ಯಾಖ್ಯಾನಿಸು T_CL	(ಪದ_STR(KS_CL))	// clear screen
#ವ್ಯಾಖ್ಯಾನಿಸು T_CD	(ಪದ_STR(KS_CD))	// clear to end of display
#ವ್ಯಾಖ್ಯಾನಿಸು T_UT	(ಪದ_STR(KS_UT))	// clearing uses background color
#ವ್ಯಾಖ್ಯಾನಿಸು T_DA	(ಪದ_STR(KS_DA))	// text may be scrolled down from up
#ವ್ಯಾಖ್ಯಾನಿಸು T_DB	(ಪದ_STR(KS_DB))	// text may be scrolled up from down
#ವ್ಯಾಖ್ಯಾನಿಸು T_VI	(ಪದ_STR(KS_VI))	// cursor invisible
#ವ್ಯಾಖ್ಯಾನಿಸು T_VE	(ಪದ_STR(KS_VE))	// cursor visible
#ವ್ಯಾಖ್ಯಾನಿಸು T_VS	(ಪದ_STR(KS_VS))	// cursor very visible (blink)
#ವ್ಯಾಖ್ಯಾನಿಸು T_CVS	(ಪದ_STR(KS_CVS))	// cursor normally visible (no blink)
#ವ್ಯಾಖ್ಯಾನಿಸು T_CSH	(ಪದ_STR(KS_CSH))	// cursor shape
#ವ್ಯಾಖ್ಯಾನಿಸು T_ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ	(ಪದ_STR(KS_ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ))	// request cursor blinking
#ವ್ಯಾಖ್ಯಾನಿಸು T_CRS	(ಪದ_STR(KS_CRS))	// request cursor style
#ವ್ಯಾಖ್ಯಾನಿಸು T_ME	(ಪದ_STR(KS_ME))	// normal mode
#ವ್ಯಾಖ್ಯಾನಿಸು T_MR	(ಪದ_STR(KS_MR))	// reverse mode
#ವ್ಯಾಖ್ಯಾನಿಸು T_MD	(ಪದ_STR(KS_MD))	// bold mode
#ವ್ಯಾಖ್ಯಾನಿಸು T_SE	(ಪದ_STR(KS_SE))	// normal mode
#ವ್ಯಾಖ್ಯಾನಿಸು T_SO	(ಪದ_STR(KS_SO))	// standout mode
#ವ್ಯಾಖ್ಯಾನಿಸು T_CZH	(ಪದ_STR(KS_CZH))	// italic mode start
#ವ್ಯಾಖ್ಯಾನಿಸು T_CZR	(ಪದ_STR(KS_CZR))	// italic mode end
#ವ್ಯಾಖ್ಯಾನಿಸು T_UE	(ಪದ_STR(KS_UE))	// exit underscore (underline) mode
#ವ್ಯಾಖ್ಯಾನಿಸು T_US	(ಪದ_STR(KS_US))	// underscore (underline) mode
#ವ್ಯಾಖ್ಯಾನಿಸು T_UCE	(ಪದ_STR(KS_UCE))	// exit undercurl mode
#ವ್ಯಾಖ್ಯಾನಿಸು T_UCS	(ಪದ_STR(KS_UCS))	// undercurl mode
#ವ್ಯಾಖ್ಯಾನಿಸು T_USS	(ಪದ_STR(KS_USS))	// double underline mode
#ವ್ಯಾಖ್ಯಾನಿಸು T_DS	(ಪದ_STR(KS_DS))	// dotted underline mode
#ವ್ಯಾಖ್ಯಾನಿಸು T_CDS	(ಪದ_STR(KS_CDS))	// dashed underline mode
#ವ್ಯಾಖ್ಯಾನಿಸು T_STE	(ಪದ_STR(KS_STE))	// exit strikethrough mode
#ವ್ಯಾಖ್ಯಾನಿಸು T_STS	(ಪದ_STR(KS_STS))	// strikethrough mode
#ವ್ಯಾಖ್ಯಾನಿಸು T_MS	(ಪದ_STR(KS_MS))	// save to move cur in reverse mode
#ವ್ಯಾಖ್ಯಾನಿಸು T_CM	(ಪದ_STR(KS_CM))	// cursor motion
#ವ್ಯಾಖ್ಯಾನಿಸು T_SR	(ಪದ_STR(KS_SR))	// scroll reverse (backward)
#ವ್ಯಾಖ್ಯಾನಿಸು T_CRI	(ಪದ_STR(KS_CRI))	// cursor number of chars right
#ವ್ಯಾಖ್ಯಾನಿಸು T_VB	(ಪದ_STR(KS_VB))	// visual bell
#ವ್ಯಾಖ್ಯಾನಿಸು T_KS	(ಪದ_STR(KS_KS))	// put term in "keypad transmit" mode
#ವ್ಯಾಖ್ಯಾನಿಸು T_KE	(ಪದ_STR(KS_KE))	// out of "keypad transmit" mode
#ವ್ಯಾಖ್ಯಾನಿಸು T_TI	(ಪದ_STR(KS_TI))	// put terminal in termcap mode
#ವ್ಯಾಖ್ಯಾನಿಸು T_CTI	(ಪದ_STR(KS_CTI))	// put terminal in "raw" mode
#ವ್ಯಾಖ್ಯಾನಿಸು T_CRK	(ಪದ_STR(KS_CRK))	// request keyboard protocol status
#ವ್ಯಾಖ್ಯಾನಿಸು T_TE	(ಪದ_STR(KS_TE))	// end of termcap mode
#ವ್ಯಾಖ್ಯಾನಿಸು T_CTE	(ಪದ_STR(KS_CTE))	// end of "raw" mode
#ವ್ಯಾಖ್ಯಾನಿಸು T_BC	(ಪದ_STR(KS_BC))	// backspace character
#ವ್ಯಾಖ್ಯಾನಿಸು T_CCS	(ಪದ_STR(KS_CCS))	// cur is relative to scroll region
#ವ್ಯಾಖ್ಯಾನಿಸು T_CCO	(ಪದ_STR(KS_CCO))	// number of colors
#ವ್ಯಾಖ್ಯಾನಿಸು T_CSF	(ಪದ_STR(KS_CSF))	// set foreground color
#ವ್ಯಾಖ್ಯಾನಿಸು T_CSB	(ಪದ_STR(KS_CSB))	// set background color
#ವ್ಯಾಖ್ಯಾನಿಸು T_XS	(ಪದ_STR(KS_XS))	// standout not erased by overwriting
#ವ್ಯಾಖ್ಯಾನಿಸು T_XN	(ಪದ_STR(KS_XN))	// newline glitch
#ವ್ಯಾಖ್ಯಾನಿಸು T_MB	(ಪದ_STR(KS_MB))	// blink mode
#ವ್ಯಾಖ್ಯಾನಿಸು T_CAF	(ಪದ_STR(KS_CAF))	// set foreground color (ANSI)
#ವ್ಯಾಖ್ಯಾನಿಸು T_CAB	(ಪದ_STR(KS_CAB))	// set background color (ANSI)
#ವ್ಯಾಖ್ಯಾನಿಸು T_CAU	(ಪದ_STR(KS_CAU))	// set underline color (ANSI)
#ವ್ಯಾಖ್ಯಾನಿಸು T_CFO	(ಪದ_STR(KS_CF))	// set alternate font
#ವ್ಯಾಖ್ಯಾನಿಸು T_LE	(ಪದ_STR(KS_LE))	// cursor left
#ವ್ಯಾಖ್ಯಾನಿಸು T_ND	(ಪದ_STR(KS_ND))	// cursor right
#ವ್ಯಾಖ್ಯಾನಿಸು T_CIS	(ಪದ_STR(KS_CIS))	// set icon text start
#ವ್ಯಾಖ್ಯಾನಿಸು T_CIE	(ಪದ_STR(KS_CIE))	// set icon text end
#ವ್ಯಾಖ್ಯಾನಿಸು T_TS	(ಪದ_STR(KS_TS))	// set window title start
#ವ್ಯಾಖ್ಯಾನಿಸು T_FS	(ಪದ_STR(KS_FS))	// set window title end
#ವ್ಯಾಖ್ಯಾನಿಸು T_CSC	(ಪದ_STR(KS_CSC))	// set cursor color start
#ವ್ಯಾಖ್ಯಾನಿಸು T_CEC	(ಪದ_STR(KS_CEC))	// set cursor color end
#ವ್ಯಾಖ್ಯಾನಿಸು T_CWP	(ಪದ_STR(KS_CWP))	// set window position
#ವ್ಯಾಖ್ಯಾನಿಸು T_CGP	(ಪದ_STR(KS_CGP))	// get window position
#ವ್ಯಾಖ್ಯಾನಿಸು T_CWS	(ಪದ_STR(KS_CWS))	// window size
#ವ್ಯಾಖ್ಯಾನಿಸು T_CSI	(ಪದ_STR(KS_CSI))	// start insert mode
#ವ್ಯಾಖ್ಯಾನಿಸು T_CEI	(ಪದ_STR(KS_CEI))	// end insert mode
#ವ್ಯಾಖ್ಯಾನಿಸು T_CSR	(ಪದ_STR(KS_CSR))	// start replace mode
#ವ್ಯಾಖ್ಯಾನಿಸು T_CRV	(ಪದ_STR(KS_CRV))	// request version string
#ವ್ಯಾಖ್ಯಾನಿಸು T_CXM	(ಪದ_STR(KS_CXM))	// enable/disable mouse reporting
#ವ್ಯಾಖ್ಯಾನಿಸು T_RFG	(ಪದ_STR(KS_RFG))	// request foreground RGB
#ವ್ಯಾಖ್ಯಾನಿಸು T_RBG	(ಪದ_STR(KS_RBG))	// request background RGB
#ವ್ಯಾಖ್ಯಾನಿಸು T_OP	(ಪದ_STR(KS_OP))	// original color pair
#ವ್ಯಾಖ್ಯಾನಿಸು T_U7	(ಪದ_STR(KS_U7))	// request cursor position
#ವ್ಯಾಖ್ಯಾನಿಸು T_8F	(ಪದ_STR(KS_8F))	// set foreground color (RGB)
#ವ್ಯಾಖ್ಯಾನಿಸು T_8B	(ಪದ_STR(KS_8B))	// set background color (RGB)
#ವ್ಯಾಖ್ಯಾನಿಸು T_8U	(ಪದ_STR(KS_8U))	// set underline color (RGB)
#ವ್ಯಾಖ್ಯಾನಿಸು T_BE	(ಪದ_STR(KS_CBE))	// enable bracketed paste mode
#ವ್ಯಾಖ್ಯಾನಿಸು T_BD	(ಪದ_STR(KS_CBD))	// disable bracketed paste mode
#ವ್ಯಾಖ್ಯಾನಿಸು T_CST	(ಪದ_STR(KS_CST))	// save window title
#ವ್ಯಾಖ್ಯಾನಿಸು T_CRT	(ಪದ_STR(KS_CRT))	// restore window title
#ವ್ಯಾಖ್ಯಾನಿಸು T_SSI	(ಪದ_STR(KS_SSI))	// save icon text
#ವ್ಯಾಖ್ಯಾನಿಸು T_SRI	(ಪದ_STR(KS_SRI))	// restore icon text
#ವ್ಯಾಖ್ಯಾನಿಸು T_FD	(ಪದ_STR(KS_FD))	// disable focus event tracking
#ವ್ಯಾಖ್ಯಾನಿಸು T_FE	(ಪದ_STR(KS_FE))	// enable focus event tracking
#ವ್ಯಾಖ್ಯಾನಿಸು T_XON	(ಪದ_STR(KS_XON))	// terminal uses xon/xoff handshaking
#ವ್ಯಾಖ್ಯಾನಿಸು T_BSU	(ಪದ_STR(KS_BSU))	// begin synchronized update
#ವ್ಯಾಖ್ಯಾನಿಸು T_ESU	(ಪದ_STR(KS_ESU))	// end synchronized update

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
    TMODE_COOK,	    // terminal mode for external cmds and Ex mode
    TMODE_SLEEP,    // terminal mode for sleeping (cooked but no echo)
    TMODE_RAW,	    // terminal mode for Normal and Insert mode
    TMODE_UNKNOWN   // after executing a shell
} tmode_T;

#ಆದರೆ defined(MSWIN)
// Mapping between cterm indices < 16 and their counterpart in the ANSI palette.
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ char_u cterm_ansi_idx[];
#ಆದರೆ_ಅಂತ್ಯ
