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
تعريفنوع بنانشان حرف char_u;
#جيڪڏهن_پورو

/*
 * Index of the termcap codes in the term_strings array.
 */
تعداد Special_ڪنجي
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
    KS_چڪري_جاچ,	// request cursor blinking
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

#وصف_ڪريو KS_آخري	    KS_ESU

/*
 * the terminal capabilities are stored in this array
 * IMPORTANT: When making changes, note the following:
 * - there should be an entry for each code in the builtin termcaps
 * - there should be an option for each code in option.c
 * - there should be code in term.c to obtain the value from the termcap
 */

خارجي char_u *(رڪن_لڙيون[]);    // current terminal strings

/*
 * strings used for terminal
 */
#وصف_ڪريو T_NAME	(رڪن_STR(KS_NAME))	// terminal name
#وصف_ڪريو T_CE	(رڪن_STR(KS_CE))	// clear to end of line
#وصف_ڪريو T_AL	(رڪن_STR(KS_AL))	// add new blank line
#وصف_ڪريو T_CAL	(رڪن_STR(KS_CAL))	// add number of blank lines
#وصف_ڪريو T_DL	(رڪن_STR(KS_DL))	// delete line
#وصف_ڪريو T_CDL	(رڪن_STR(KS_CDL))	// delete number of lines
#وصف_ڪريو T_CS	(رڪن_STR(KS_CS))	// scroll region
#وصف_ڪريو T_CSV	(رڪن_STR(KS_CSV))	// scroll region vertical
#وصف_ڪريو T_CL	(رڪن_STR(KS_CL))	// clear screen
#وصف_ڪريو T_CD	(رڪن_STR(KS_CD))	// clear to end of display
#وصف_ڪريو T_UT	(رڪن_STR(KS_UT))	// clearing uses background color
#وصف_ڪريو T_DA	(رڪن_STR(KS_DA))	// text may be scrolled down from up
#وصف_ڪريو T_DB	(رڪن_STR(KS_DB))	// text may be scrolled up from down
#وصف_ڪريو T_VI	(رڪن_STR(KS_VI))	// cursor invisible
#وصف_ڪريو T_VE	(رڪن_STR(KS_VE))	// cursor visible
#وصف_ڪريو T_VS	(رڪن_STR(KS_VS))	// cursor very visible (blink)
#وصف_ڪريو T_CVS	(رڪن_STR(KS_CVS))	// cursor normally visible (no blink)
#وصف_ڪريو T_CSH	(رڪن_STR(KS_CSH))	// cursor shape
#وصف_ڪريو T_چڪري_جاچ	(رڪن_STR(KS_چڪري_جاچ))	// request cursor blinking
#وصف_ڪريو T_CRS	(رڪن_STR(KS_CRS))	// request cursor style
#وصف_ڪريو T_ME	(رڪن_STR(KS_ME))	// normal mode
#وصف_ڪريو T_MR	(رڪن_STR(KS_MR))	// reverse mode
#وصف_ڪريو T_MD	(رڪن_STR(KS_MD))	// bold mode
#وصف_ڪريو T_SE	(رڪن_STR(KS_SE))	// normal mode
#وصف_ڪريو T_SO	(رڪن_STR(KS_SO))	// standout mode
#وصف_ڪريو T_CZH	(رڪن_STR(KS_CZH))	// italic mode start
#وصف_ڪريو T_CZR	(رڪن_STR(KS_CZR))	// italic mode end
#وصف_ڪريو T_UE	(رڪن_STR(KS_UE))	// exit underscore (underline) mode
#وصف_ڪريو T_US	(رڪن_STR(KS_US))	// underscore (underline) mode
#وصف_ڪريو T_UCE	(رڪن_STR(KS_UCE))	// exit undercurl mode
#وصف_ڪريو T_UCS	(رڪن_STR(KS_UCS))	// undercurl mode
#وصف_ڪريو T_USS	(رڪن_STR(KS_USS))	// double underline mode
#وصف_ڪريو T_DS	(رڪن_STR(KS_DS))	// dotted underline mode
#وصف_ڪريو T_CDS	(رڪن_STR(KS_CDS))	// dashed underline mode
#وصف_ڪريو T_STE	(رڪن_STR(KS_STE))	// exit strikethrough mode
#وصف_ڪريو T_STS	(رڪن_STR(KS_STS))	// strikethrough mode
#وصف_ڪريو T_MS	(رڪن_STR(KS_MS))	// save to move cur in reverse mode
#وصف_ڪريو T_CM	(رڪن_STR(KS_CM))	// cursor motion
#وصف_ڪريو T_SR	(رڪن_STR(KS_SR))	// scroll reverse (backward)
#وصف_ڪريو T_CRI	(رڪن_STR(KS_CRI))	// cursor number of chars right
#وصف_ڪريو T_VB	(رڪن_STR(KS_VB))	// visual bell
#وصف_ڪريو T_KS	(رڪن_STR(KS_KS))	// put term in "keypad transmit" mode
#وصف_ڪريو T_KE	(رڪن_STR(KS_KE))	// out of "keypad transmit" mode
#وصف_ڪريو T_TI	(رڪن_STR(KS_TI))	// put terminal in termcap mode
#وصف_ڪريو T_CTI	(رڪن_STR(KS_CTI))	// put terminal in "raw" mode
#وصف_ڪريو T_CRK	(رڪن_STR(KS_CRK))	// request keyboard protocol status
#وصف_ڪريو T_TE	(رڪن_STR(KS_TE))	// end of termcap mode
#وصف_ڪريو T_CTE	(رڪن_STR(KS_CTE))	// end of "raw" mode
#وصف_ڪريو T_BC	(رڪن_STR(KS_BC))	// backspace character
#وصف_ڪريو T_CCS	(رڪن_STR(KS_CCS))	// cur is relative to scroll region
#وصف_ڪريو T_CCO	(رڪن_STR(KS_CCO))	// number of colors
#وصف_ڪريو T_CSF	(رڪن_STR(KS_CSF))	// set foreground color
#وصف_ڪريو T_CSB	(رڪن_STR(KS_CSB))	// set background color
#وصف_ڪريو T_XS	(رڪن_STR(KS_XS))	// standout not erased by overwriting
#وصف_ڪريو T_XN	(رڪن_STR(KS_XN))	// newline glitch
#وصف_ڪريو T_MB	(رڪن_STR(KS_MB))	// blink mode
#وصف_ڪريو T_CAF	(رڪن_STR(KS_CAF))	// set foreground color (ANSI)
#وصف_ڪريو T_CAB	(رڪن_STR(KS_CAB))	// set background color (ANSI)
#وصف_ڪريو T_CAU	(رڪن_STR(KS_CAU))	// set underline color (ANSI)
#وصف_ڪريو T_CFO	(رڪن_STR(KS_CF))	// set alternate font
#وصف_ڪريو T_LE	(رڪن_STR(KS_LE))	// cursor left
#وصف_ڪريو T_ND	(رڪن_STR(KS_ND))	// cursor right
#وصف_ڪريو T_CIS	(رڪن_STR(KS_CIS))	// set icon text start
#وصف_ڪريو T_CIE	(رڪن_STR(KS_CIE))	// set icon text end
#وصف_ڪريو T_TS	(رڪن_STR(KS_TS))	// set window title start
#وصف_ڪريو T_FS	(رڪن_STR(KS_FS))	// set window title end
#وصف_ڪريو T_CSC	(رڪن_STR(KS_CSC))	// set cursor color start
#وصف_ڪريو T_CEC	(رڪن_STR(KS_CEC))	// set cursor color end
#وصف_ڪريو T_CWP	(رڪن_STR(KS_CWP))	// set window position
#وصف_ڪريو T_CGP	(رڪن_STR(KS_CGP))	// get window position
#وصف_ڪريو T_CWS	(رڪن_STR(KS_CWS))	// window size
#وصف_ڪريو T_CSI	(رڪن_STR(KS_CSI))	// start insert mode
#وصف_ڪريو T_CEI	(رڪن_STR(KS_CEI))	// end insert mode
#وصف_ڪريو T_CSR	(رڪن_STR(KS_CSR))	// start replace mode
#وصف_ڪريو T_CRV	(رڪن_STR(KS_CRV))	// request version string
#وصف_ڪريو T_CXM	(رڪن_STR(KS_CXM))	// enable/disable mouse reporting
#وصف_ڪريو T_RFG	(رڪن_STR(KS_RFG))	// request foreground RGB
#وصف_ڪريو T_RBG	(رڪن_STR(KS_RBG))	// request background RGB
#وصف_ڪريو T_OP	(رڪن_STR(KS_OP))	// original color pair
#وصف_ڪريو T_U7	(رڪن_STR(KS_U7))	// request cursor position
#وصف_ڪريو T_8F	(رڪن_STR(KS_8F))	// set foreground color (RGB)
#وصف_ڪريو T_8B	(رڪن_STR(KS_8B))	// set background color (RGB)
#وصف_ڪريو T_8U	(رڪن_STR(KS_8U))	// set underline color (RGB)
#وصف_ڪريو T_BE	(رڪن_STR(KS_CBE))	// enable bracketed paste mode
#وصف_ڪريو T_BD	(رڪن_STR(KS_CBD))	// disable bracketed paste mode
#وصف_ڪريو T_CST	(رڪن_STR(KS_CST))	// save window title
#وصف_ڪريو T_CRT	(رڪن_STR(KS_CRT))	// restore window title
#وصف_ڪريو T_SSI	(رڪن_STR(KS_SSI))	// save icon text
#وصف_ڪريو T_SRI	(رڪن_STR(KS_SRI))	// restore icon text
#وصف_ڪريو T_FD	(رڪن_STR(KS_FD))	// disable focus event tracking
#وصف_ڪريو T_FE	(رڪن_STR(KS_FE))	// enable focus event tracking
#وصف_ڪريو T_XON	(رڪن_STR(KS_XON))	// terminal uses xon/xoff handshaking
#وصف_ڪريو T_BSU	(رڪن_STR(KS_BSU))	// begin synchronized update
#وصف_ڪريو T_ESU	(رڪن_STR(KS_ESU))	// end synchronized update

تعريفنوع تعداد {
    TMODE_COOK,	    // terminal mode for external cmds and Ex mode
    TMODE_SLEEP,    // terminal mode for sleeping (cooked but no echo)
    TMODE_RAW,	    // terminal mode for Normal and Insert mode
    TMODE_UNKNOWN   // after executing a shell
} tmode_T;

#إذا defined(MSWIN)
// Mapping between cterm indices < 16 and their counterpart in the ANSI palette.
خارجي ثابت char_u cterm_ansi_idx[];
#جيڪڏهن_پورو
