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

#ఒకవేళ defined(SASC) && SASC < 658
/*
 * The SAS C compiler has a bug that makes typedefs being forgotten
 * in include files.  Has been fixed in version 6.58.
 */
రకనిర్వచనం గుర్తులేని అక్షరం char_u;
#అయితే_ముగింపు

/*
 * Index of the termcap codes in the term_strings array.
 */
గణన Special_కీలకం
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
    KS_చక్రీయ_తనిఖీ,	// request cursor blinking
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

#నిర్వచించు KS_చివరి	    KS_ESU

/*
 * the terminal capabilities are stored in this array
 * IMPORTANT: When making changes, note the following:
 * - there should be an entry for each code in the builtin termcaps
 * - there should be an option for each code in option.c
 * - there should be code in term.c to obtain the value from the termcap
 */

బాహ్య char_u *(పదం_అక్షరశ్రేణులు[]);    // current terminal strings

/*
 * strings used for terminal
 */
#నిర్వచించు T_NAME	(పదం_STR(KS_NAME))	// terminal name
#నిర్వచించు T_CE	(పదం_STR(KS_CE))	// clear to end of line
#నిర్వచించు T_AL	(పదం_STR(KS_AL))	// add new blank line
#నిర్వచించు T_CAL	(పదం_STR(KS_CAL))	// add number of blank lines
#నిర్వచించు T_DL	(పదం_STR(KS_DL))	// delete line
#నిర్వచించు T_CDL	(పదం_STR(KS_CDL))	// delete number of lines
#నిర్వచించు T_CS	(పదం_STR(KS_CS))	// scroll region
#నిర్వచించు T_CSV	(పదం_STR(KS_CSV))	// scroll region vertical
#నిర్వచించు T_CL	(పదం_STR(KS_CL))	// clear screen
#నిర్వచించు T_CD	(పదం_STR(KS_CD))	// clear to end of display
#నిర్వచించు T_UT	(పదం_STR(KS_UT))	// clearing uses background color
#నిర్వచించు T_DA	(పదం_STR(KS_DA))	// text may be scrolled down from up
#నిర్వచించు T_DB	(పదం_STR(KS_DB))	// text may be scrolled up from down
#నిర్వచించు T_VI	(పదం_STR(KS_VI))	// cursor invisible
#నిర్వచించు T_VE	(పదం_STR(KS_VE))	// cursor visible
#నిర్వచించు T_VS	(పదం_STR(KS_VS))	// cursor very visible (blink)
#నిర్వచించు T_CVS	(పదం_STR(KS_CVS))	// cursor normally visible (no blink)
#నిర్వచించు T_CSH	(పదం_STR(KS_CSH))	// cursor shape
#నిర్వచించు T_చక్రీయ_తనిఖీ	(పదం_STR(KS_చక్రీయ_తనిఖీ))	// request cursor blinking
#నిర్వచించు T_CRS	(పదం_STR(KS_CRS))	// request cursor style
#నిర్వచించు T_ME	(పదం_STR(KS_ME))	// normal mode
#నిర్వచించు T_MR	(పదం_STR(KS_MR))	// reverse mode
#నిర్వచించు T_MD	(పదం_STR(KS_MD))	// bold mode
#నిర్వచించు T_SE	(పదం_STR(KS_SE))	// normal mode
#నిర్వచించు T_SO	(పదం_STR(KS_SO))	// standout mode
#నిర్వచించు T_CZH	(పదం_STR(KS_CZH))	// italic mode start
#నిర్వచించు T_CZR	(పదం_STR(KS_CZR))	// italic mode end
#నిర్వచించు T_UE	(పదం_STR(KS_UE))	// exit underscore (underline) mode
#నిర్వచించు T_US	(పదం_STR(KS_US))	// underscore (underline) mode
#నిర్వచించు T_UCE	(పదం_STR(KS_UCE))	// exit undercurl mode
#నిర్వచించు T_UCS	(పదం_STR(KS_UCS))	// undercurl mode
#నిర్వచించు T_USS	(పదం_STR(KS_USS))	// double underline mode
#నిర్వచించు T_DS	(పదం_STR(KS_DS))	// dotted underline mode
#నిర్వచించు T_CDS	(పదం_STR(KS_CDS))	// dashed underline mode
#నిర్వచించు T_STE	(పదం_STR(KS_STE))	// exit strikethrough mode
#నిర్వచించు T_STS	(పదం_STR(KS_STS))	// strikethrough mode
#నిర్వచించు T_MS	(పదం_STR(KS_MS))	// save to move cur in reverse mode
#నిర్వచించు T_CM	(పదం_STR(KS_CM))	// cursor motion
#నిర్వచించు T_SR	(పదం_STR(KS_SR))	// scroll reverse (backward)
#నిర్వచించు T_CRI	(పదం_STR(KS_CRI))	// cursor number of chars right
#నిర్వచించు T_VB	(పదం_STR(KS_VB))	// visual bell
#నిర్వచించు T_KS	(పదం_STR(KS_KS))	// put term in "keypad transmit" mode
#నిర్వచించు T_KE	(పదం_STR(KS_KE))	// out of "keypad transmit" mode
#నిర్వచించు T_TI	(పదం_STR(KS_TI))	// put terminal in termcap mode
#నిర్వచించు T_CTI	(పదం_STR(KS_CTI))	// put terminal in "raw" mode
#నిర్వచించు T_CRK	(పదం_STR(KS_CRK))	// request keyboard protocol status
#నిర్వచించు T_TE	(పదం_STR(KS_TE))	// end of termcap mode
#నిర్వచించు T_CTE	(పదం_STR(KS_CTE))	// end of "raw" mode
#నిర్వచించు T_BC	(పదం_STR(KS_BC))	// backspace character
#నిర్వచించు T_CCS	(పదం_STR(KS_CCS))	// cur is relative to scroll region
#నిర్వచించు T_CCO	(పదం_STR(KS_CCO))	// number of colors
#నిర్వచించు T_CSF	(పదం_STR(KS_CSF))	// set foreground color
#నిర్వచించు T_CSB	(పదం_STR(KS_CSB))	// set background color
#నిర్వచించు T_XS	(పదం_STR(KS_XS))	// standout not erased by overwriting
#నిర్వచించు T_XN	(పదం_STR(KS_XN))	// newline glitch
#నిర్వచించు T_MB	(పదం_STR(KS_MB))	// blink mode
#నిర్వచించు T_CAF	(పదం_STR(KS_CAF))	// set foreground color (ANSI)
#నిర్వచించు T_CAB	(పదం_STR(KS_CAB))	// set background color (ANSI)
#నిర్వచించు T_CAU	(పదం_STR(KS_CAU))	// set underline color (ANSI)
#నిర్వచించు T_CFO	(పదం_STR(KS_CF))	// set alternate font
#నిర్వచించు T_LE	(పదం_STR(KS_LE))	// cursor left
#నిర్వచించు T_ND	(పదం_STR(KS_ND))	// cursor right
#నిర్వచించు T_CIS	(పదం_STR(KS_CIS))	// set icon text start
#నిర్వచించు T_CIE	(పదం_STR(KS_CIE))	// set icon text end
#నిర్వచించు T_TS	(పదం_STR(KS_TS))	// set window title start
#నిర్వచించు T_FS	(పదం_STR(KS_FS))	// set window title end
#నిర్వచించు T_CSC	(పదం_STR(KS_CSC))	// set cursor color start
#నిర్వచించు T_CEC	(పదం_STR(KS_CEC))	// set cursor color end
#నిర్వచించు T_CWP	(పదం_STR(KS_CWP))	// set window position
#నిర్వచించు T_CGP	(పదం_STR(KS_CGP))	// get window position
#నిర్వచించు T_CWS	(పదం_STR(KS_CWS))	// window size
#నిర్వచించు T_CSI	(పదం_STR(KS_CSI))	// start insert mode
#నిర్వచించు T_CEI	(పదం_STR(KS_CEI))	// end insert mode
#నిర్వచించు T_CSR	(పదం_STR(KS_CSR))	// start replace mode
#నిర్వచించు T_CRV	(పదం_STR(KS_CRV))	// request version string
#నిర్వచించు T_CXM	(పదం_STR(KS_CXM))	// enable/disable mouse reporting
#నిర్వచించు T_RFG	(పదం_STR(KS_RFG))	// request foreground RGB
#నిర్వచించు T_RBG	(పదం_STR(KS_RBG))	// request background RGB
#నిర్వచించు T_OP	(పదం_STR(KS_OP))	// original color pair
#నిర్వచించు T_U7	(పదం_STR(KS_U7))	// request cursor position
#నిర్వచించు T_8F	(పదం_STR(KS_8F))	// set foreground color (RGB)
#నిర్వచించు T_8B	(పదం_STR(KS_8B))	// set background color (RGB)
#నిర్వచించు T_8U	(పదం_STR(KS_8U))	// set underline color (RGB)
#నిర్వచించు T_BE	(పదం_STR(KS_CBE))	// enable bracketed paste mode
#నిర్వచించు T_BD	(పదం_STR(KS_CBD))	// disable bracketed paste mode
#నిర్వచించు T_CST	(పదం_STR(KS_CST))	// save window title
#నిర్వచించు T_CRT	(పదం_STR(KS_CRT))	// restore window title
#నిర్వచించు T_SSI	(పదం_STR(KS_SSI))	// save icon text
#నిర్వచించు T_SRI	(పదం_STR(KS_SRI))	// restore icon text
#నిర్వచించు T_FD	(పదం_STR(KS_FD))	// disable focus event tracking
#నిర్వచించు T_FE	(పదం_STR(KS_FE))	// enable focus event tracking
#నిర్వచించు T_XON	(పదం_STR(KS_XON))	// terminal uses xon/xoff handshaking
#నిర్వచించు T_BSU	(పదం_STR(KS_BSU))	// begin synchronized update
#నిర్వచించు T_ESU	(పదం_STR(KS_ESU))	// end synchronized update

రకనిర్వచనం గణన {
    TMODE_COOK,	    // terminal mode for external cmds and Ex mode
    TMODE_SLEEP,    // terminal mode for sleeping (cooked but no echo)
    TMODE_RAW,	    // terminal mode for Normal and Insert mode
    TMODE_UNKNOWN   // after executing a shell
} tmode_T;

#ఒకవేళ defined(MSWIN)
// Mapping between cterm indices < 16 and their counterpart in the ANSI palette.
బాహ్య స్థిరరాశి char_u cterm_ansi_idx[];
#అయితే_ముగింపు
