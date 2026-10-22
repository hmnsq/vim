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

#ਜੇ defined(SASC) && SASC < 658
/*
 * The SAS C compiler has a bug that makes typedefs being forgotten
 * in include files.  Has been fixed in version 6.58.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ char_u;
#ਜੇ_ਅੰਤ

/*
 * Index of the termcap codes in the term_strings array.
 */
ਗਿਣਤੀ Special_ਕੁੰਜੀ
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
    KS_ਚੱਕਰੀ_ਜਾਂਚ,	// request cursor blinking
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

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ KS_ਆਖਰੀ	    KS_ESU

/*
 * the terminal capabilities are stored in this array
 * IMPORTANT: When making changes, note the following:
 * - there should be an entry for each code in the builtin termcaps
 * - there should be an option for each code in option.c
 * - there should be code in term.c to obtain the value from the termcap
 */

ਬਾਹਰੀ char_u *(ਪਦ_ਲੜੀਆਂ[]);    // current terminal strings

/*
 * strings used for terminal
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_NAME	(ਪਦ_STR(KS_NAME))	// terminal name
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_CE	(ਪਦ_STR(KS_CE))	// clear to end of line
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_AL	(ਪਦ_STR(KS_AL))	// add new blank line
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_CAL	(ਪਦ_STR(KS_CAL))	// add number of blank lines
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_DL	(ਪਦ_STR(KS_DL))	// delete line
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_CDL	(ਪਦ_STR(KS_CDL))	// delete number of lines
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_CS	(ਪਦ_STR(KS_CS))	// scroll region
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_CSV	(ਪਦ_STR(KS_CSV))	// scroll region vertical
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_CL	(ਪਦ_STR(KS_CL))	// clear screen
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_CD	(ਪਦ_STR(KS_CD))	// clear to end of display
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_UT	(ਪਦ_STR(KS_UT))	// clearing uses background color
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_DA	(ਪਦ_STR(KS_DA))	// text may be scrolled down from up
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_DB	(ਪਦ_STR(KS_DB))	// text may be scrolled up from down
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_VI	(ਪਦ_STR(KS_VI))	// cursor invisible
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_VE	(ਪਦ_STR(KS_VE))	// cursor visible
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_VS	(ਪਦ_STR(KS_VS))	// cursor very visible (blink)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_CVS	(ਪਦ_STR(KS_CVS))	// cursor normally visible (no blink)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_CSH	(ਪਦ_STR(KS_CSH))	// cursor shape
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_ਚੱਕਰੀ_ਜਾਂਚ	(ਪਦ_STR(KS_ਚੱਕਰੀ_ਜਾਂਚ))	// request cursor blinking
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_CRS	(ਪਦ_STR(KS_CRS))	// request cursor style
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_ME	(ਪਦ_STR(KS_ME))	// normal mode
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_MR	(ਪਦ_STR(KS_MR))	// reverse mode
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_MD	(ਪਦ_STR(KS_MD))	// bold mode
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_SE	(ਪਦ_STR(KS_SE))	// normal mode
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_SO	(ਪਦ_STR(KS_SO))	// standout mode
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_CZH	(ਪਦ_STR(KS_CZH))	// italic mode start
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_CZR	(ਪਦ_STR(KS_CZR))	// italic mode end
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_UE	(ਪਦ_STR(KS_UE))	// exit underscore (underline) mode
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_US	(ਪਦ_STR(KS_US))	// underscore (underline) mode
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_UCE	(ਪਦ_STR(KS_UCE))	// exit undercurl mode
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_UCS	(ਪਦ_STR(KS_UCS))	// undercurl mode
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_USS	(ਪਦ_STR(KS_USS))	// double underline mode
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_DS	(ਪਦ_STR(KS_DS))	// dotted underline mode
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_CDS	(ਪਦ_STR(KS_CDS))	// dashed underline mode
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_STE	(ਪਦ_STR(KS_STE))	// exit strikethrough mode
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_STS	(ਪਦ_STR(KS_STS))	// strikethrough mode
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_MS	(ਪਦ_STR(KS_MS))	// save to move cur in reverse mode
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_CM	(ਪਦ_STR(KS_CM))	// cursor motion
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_SR	(ਪਦ_STR(KS_SR))	// scroll reverse (backward)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_CRI	(ਪਦ_STR(KS_CRI))	// cursor number of chars right
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_VB	(ਪਦ_STR(KS_VB))	// visual bell
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_KS	(ਪਦ_STR(KS_KS))	// put term in "keypad transmit" mode
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_KE	(ਪਦ_STR(KS_KE))	// out of "keypad transmit" mode
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_TI	(ਪਦ_STR(KS_TI))	// put terminal in termcap mode
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_CTI	(ਪਦ_STR(KS_CTI))	// put terminal in "raw" mode
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_CRK	(ਪਦ_STR(KS_CRK))	// request keyboard protocol status
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_TE	(ਪਦ_STR(KS_TE))	// end of termcap mode
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_CTE	(ਪਦ_STR(KS_CTE))	// end of "raw" mode
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_BC	(ਪਦ_STR(KS_BC))	// backspace character
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_CCS	(ਪਦ_STR(KS_CCS))	// cur is relative to scroll region
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_CCO	(ਪਦ_STR(KS_CCO))	// number of colors
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_CSF	(ਪਦ_STR(KS_CSF))	// set foreground color
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_CSB	(ਪਦ_STR(KS_CSB))	// set background color
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_XS	(ਪਦ_STR(KS_XS))	// standout not erased by overwriting
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_XN	(ਪਦ_STR(KS_XN))	// newline glitch
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_MB	(ਪਦ_STR(KS_MB))	// blink mode
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_CAF	(ਪਦ_STR(KS_CAF))	// set foreground color (ANSI)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_CAB	(ਪਦ_STR(KS_CAB))	// set background color (ANSI)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_CAU	(ਪਦ_STR(KS_CAU))	// set underline color (ANSI)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_CFO	(ਪਦ_STR(KS_CF))	// set alternate font
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_LE	(ਪਦ_STR(KS_LE))	// cursor left
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_ND	(ਪਦ_STR(KS_ND))	// cursor right
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_CIS	(ਪਦ_STR(KS_CIS))	// set icon text start
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_CIE	(ਪਦ_STR(KS_CIE))	// set icon text end
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_TS	(ਪਦ_STR(KS_TS))	// set window title start
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_FS	(ਪਦ_STR(KS_FS))	// set window title end
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_CSC	(ਪਦ_STR(KS_CSC))	// set cursor color start
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_CEC	(ਪਦ_STR(KS_CEC))	// set cursor color end
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_CWP	(ਪਦ_STR(KS_CWP))	// set window position
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_CGP	(ਪਦ_STR(KS_CGP))	// get window position
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_CWS	(ਪਦ_STR(KS_CWS))	// window size
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_CSI	(ਪਦ_STR(KS_CSI))	// start insert mode
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_CEI	(ਪਦ_STR(KS_CEI))	// end insert mode
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_CSR	(ਪਦ_STR(KS_CSR))	// start replace mode
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_CRV	(ਪਦ_STR(KS_CRV))	// request version string
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_CXM	(ਪਦ_STR(KS_CXM))	// enable/disable mouse reporting
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_RFG	(ਪਦ_STR(KS_RFG))	// request foreground RGB
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_RBG	(ਪਦ_STR(KS_RBG))	// request background RGB
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_OP	(ਪਦ_STR(KS_OP))	// original color pair
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_U7	(ਪਦ_STR(KS_U7))	// request cursor position
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_8F	(ਪਦ_STR(KS_8F))	// set foreground color (RGB)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_8B	(ਪਦ_STR(KS_8B))	// set background color (RGB)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_8U	(ਪਦ_STR(KS_8U))	// set underline color (RGB)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_BE	(ਪਦ_STR(KS_CBE))	// enable bracketed paste mode
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_BD	(ਪਦ_STR(KS_CBD))	// disable bracketed paste mode
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_CST	(ਪਦ_STR(KS_CST))	// save window title
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_CRT	(ਪਦ_STR(KS_CRT))	// restore window title
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_SSI	(ਪਦ_STR(KS_SSI))	// save icon text
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_SRI	(ਪਦ_STR(KS_SRI))	// restore icon text
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_FD	(ਪਦ_STR(KS_FD))	// disable focus event tracking
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_FE	(ਪਦ_STR(KS_FE))	// enable focus event tracking
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_XON	(ਪਦ_STR(KS_XON))	// terminal uses xon/xoff handshaking
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_BSU	(ਪਦ_STR(KS_BSU))	// begin synchronized update
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_ESU	(ਪਦ_STR(KS_ESU))	// end synchronized update

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
    TMODE_COOK,	    // terminal mode for external cmds and Ex mode
    TMODE_SLEEP,    // terminal mode for sleeping (cooked but no echo)
    TMODE_RAW,	    // terminal mode for Normal and Insert mode
    TMODE_UNKNOWN   // after executing a shell
} tmode_T;

#ਜੇ defined(MSWIN)
// Mapping between cterm indices < 16 and their counterpart in the ANSI palette.
ਬਾਹਰੀ ਅਚਲ char_u cterm_ansi_idx[];
#ਜੇ_ਅੰਤ
