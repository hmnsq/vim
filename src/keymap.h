/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

/*
 * Keycode definitions for special keys.
 *
 * Any special key code sequences are replaced by these codes.
 */

/*
 * For MSDOS some keys produce codes larger than 0xff. They are split into two
 * chars, the first one is K_NUL (same value used in term.h).
 */
#ವ್ಯಾಖ್ಯಾನಿಸು K_NUL			(0xce)	// for MSDOS: special key follows

/*
 * K_SPECIAL is the first byte of a special key code and is always followed by
 * two bytes.
 * The second byte can have any value. ASCII is used for normal termcap
 * entries, 0x80 and higher for special keys, see below.
 * The third byte is guaranteed to be between 0x02 and 0x7f.
 */

#ವ್ಯಾಖ್ಯಾನಿಸು K_SPECIAL		(0x80)

/*
 * Positive characters are "normal" characters.
 * Negative characters are special key codes.  Only characters below -0x200
 * are used to so that the absolute value can't be mistaken for a single-byte
 * character.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು IS_SPECIAL(c)		((c) < 0)

/*
 * Characters 0x0100 - 0x01ff have a special meaning for abbreviations.
 * Multi-byte characters also have ABBR_OFF added, thus are above 0x0200.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು ABBR_OFF		0x100

/*
 * NUL cannot be in the input string, therefore it is replaced by
 *	K_SPECIAL   KS_ZERO	KE_FILLER
 */
#ವ್ಯಾಖ್ಯಾನಿಸು KS_ZERO			255

/*
 * K_SPECIAL cannot be in the input string, therefore it is replaced by
 *	K_SPECIAL   KS_SPECIAL	KE_FILLER
 */
#ವ್ಯಾಖ್ಯಾನಿಸು KS_SPECIAL		254

/*
 * KS_EXTRA is used for keys that have no termcap name
 *	K_SPECIAL   KS_EXTRA	KE_xxx
 */
#ವ್ಯಾಖ್ಯಾನಿಸು KS_EXTRA		253

/*
 * KS_MODIFIER is used when a modifier is given for a (special) key
 *	K_SPECIAL   KS_MODIFIER	bitmask
 */
#ವ್ಯಾಖ್ಯಾನಿಸು KS_MODIFIER		252

/*
 * These are used for the GUI
 *	K_SPECIAL   KS_xxx	KE_FILLER
 */
#ವ್ಯಾಖ್ಯಾನಿಸು KS_MOUSE		251
#ವ್ಯಾಖ್ಯಾನಿಸು KS_MENU			250
#ವ್ಯಾಖ್ಯಾನಿಸು KS_VER_SCROLLBAR	249
#ವ್ಯಾಖ್ಯಾನಿಸು KS_HOR_SCROLLBAR	248

/*
 * These are used for DEC mouse
 */
#ವ್ಯಾಖ್ಯಾನಿಸು KS_NETTERM_MOUSE	247
#ವ್ಯಾಖ್ಯಾನಿಸು KS_DEC_MOUSE		246

/*
 * Used for switching Select mode back on after a mapping or menu.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು KS_SELECT		245
#ವ್ಯಾಖ್ಯಾನಿಸು K_SELECT_STRING		(char_u *)"\200\365X"

/*
 * Used for tearing off a menu.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು KS_TEAROFF		244

// Used for JSB term mouse.
#ವ್ಯಾಖ್ಯಾನಿಸು KS_JSBTERM_MOUSE	243

// Used a termcap entry that produces a normal character.
#ವ್ಯಾಖ್ಯಾನಿಸು KS_ಕೀಲಿ			242

// Used for the qnx pterm mouse.
#ವ್ಯಾಖ್ಯಾನಿಸು KS_PTERM_MOUSE		241

// Used for click in a tab pages label.
#ವ್ಯಾಖ್ಯಾನಿಸು KS_TABLINE		240

// Used for menu in a tab pages line.
#ವ್ಯಾಖ್ಯಾನಿಸು KS_TABMENU		239

// Used for the urxvt mouse.
#ವ್ಯಾಖ್ಯಾನಿಸು KS_URXVT_MOUSE		238

// Used for the sgr mouse.
#ವ್ಯಾಖ್ಯಾನಿಸು KS_SGR_MOUSE		237
#ವ್ಯಾಖ್ಯಾನಿಸು KS_SGR_MOUSE_RELEASE	236

// Used for the GPM mouse.
#ವ್ಯಾಖ್ಯಾನಿಸು KS_GPM_MOUSE		235

/*
 * Filler used after KS_SPECIAL and others
 */
#ವ್ಯಾಖ್ಯಾನಿಸು KE_FILLER		('X')

/*
 * translation of three byte code "K_SPECIAL a b" into int "K_xxx" and back
 */
#ವ್ಯಾಖ್ಯಾನಿಸು TERMCAP2_ಕೀಲಿ(a, b)	(-((a) + ((ಪೂರ್ಣಾಂಕ)(b) << 8)))
#ವ್ಯಾಖ್ಯಾನಿಸು KEY2TERMCAP0(x)		((-(x)) & 0xff)
#ವ್ಯಾಖ್ಯಾನಿಸು KEY2TERMCAP1(x)		(((ಚಿಹ್ನೆರಹಿತ)(-(x)) >> 8) & 0xff)

/*
 * get second or third byte when translating special key code into three bytes
 */
#ವ್ಯಾಖ್ಯಾನಿಸು K_ಎರಡನೇ(c)	((c) == K_SPECIAL ? KS_SPECIAL : (c) == NUL ? KS_ZERO : KEY2TERMCAP0(c))

#ವ್ಯಾಖ್ಯಾನಿಸು K_ಮೂರನೇ(c)	(((c) == K_SPECIAL || (c) == NUL) ? KE_FILLER : KEY2TERMCAP1(c))

/*
 * get single int code from second byte after K_SPECIAL
 */
#ವ್ಯಾಖ್ಯಾನಿಸು ಗಮ್ಯ_SPECIAL(a, b)    ((a) == KS_SPECIAL ? K_SPECIAL : (a) == KS_ZERO ? K_ZERO : TERMCAP2_ಕೀಲಿ(a, b))

/*
 * Codes for keys that do not have a termcap name.
 * The numbers are fixed to make sure that recorded key sequences remain valid.
 * Add new entries at the end, not halfway.
 *
 * K_SPECIAL KS_EXTRA KE_xxx
 */
ಎಣಿಕೆ ಕೀಲಿ_extra
{
      KE_S_ಮೇಲೆ = 4		// shift-up
    , KE_S_ಕೆಳಕ್ಕೆ = 5		// shift-down

    , KE_S_F1 = 6		// shifted function keys
    , KE_S_F2 = 7
    , KE_S_F3 = 8
    , KE_S_F4 = 9
    , KE_S_F5 = 10
    , KE_S_F6 = 11
    , KE_S_F7 = 12
    , KE_S_F8 = 13
    , KE_S_F9 = 14
    , KE_S_F10 = 15

    , KE_S_F11 = 16
    , KE_S_F12 = 17
    , KE_S_F13 = 18
    , KE_S_F14 = 19
    , KE_S_F15 = 20
    , KE_S_F16 = 21
    , KE_S_F17 = 22
    , KE_S_F18 = 23
    , KE_S_F19 = 24
    , KE_S_F20 = 25

    , KE_S_F21 = 26
    , KE_S_F22 = 27
    , KE_S_F23 = 28
    , KE_S_F24 = 29
    , KE_S_F25 = 30
    , KE_S_F26 = 31
    , KE_S_F27 = 32
    , KE_S_F28 = 33
    , KE_S_F29 = 34
    , KE_S_F30 = 35

    , KE_S_F31 = 36
    , KE_S_F32 = 37
    , KE_S_F33 = 38
    , KE_S_F34 = 39
    , KE_S_F35 = 40
    , KE_S_F36 = 41
    , KE_S_F37 = 42

    , KE_MOUSE = 43		// mouse event start

/*
 * Symbols for pseudo keys which are translated from the real key symbols
 * above.
 */
    , KE_LEFTMOUSE = 44		// Left mouse button click
    , KE_LEFTDRAG = 45		// Drag with left mouse button down
    , KE_LEFTRELEASE = 46	// Left mouse button release
    , KE_MIDDLEMOUSE = 47	// Middle mouse button click
    , KE_MIDDLEDRAG = 48	// Drag with middle mouse button down
    , KE_MIDDLERELEASE = 49	// Middle mouse button release
    , KE_RIGHTMOUSE = 50	// Right mouse button click
    , KE_RIGHTDRAG = 51		// Drag with right mouse button down
    , KE_RIGHTRELEASE = 52	// Right mouse button release

    , KE_IGNORE = 53		// Ignored mouse drag/release

    , KE_TAB = 54		// unshifted TAB key
    , KE_S_TAB_OLD = 55		// shifted TAB key (no longer used)

    , KE_SNIFF_ಬಳಸದ = 56	// obsolete
    , KE_XF1 = 57		// extra vt100 function keys for xterm
    , KE_XF2 = 58
    , KE_XF3 = 59
    , KE_XF4 = 60
    , KE_XEND = 61		// extra (vt100) end key for xterm
    , KE_ZEND = 62		// extra (vt100) end key for xterm
    , KE_XHOME = 63		// extra (vt100) home key for xterm
    , KE_ZHOME = 64		// extra (vt100) home key for xterm
    , KE_XUP = 65		// extra vt100 cursor keys for xterm
    , KE_XDOWN = 66
    , KE_XLEFT = 67
    , KE_XRIGHT = 68

    , KE_LEFTMOUSE_NM = 69	// non-mappable Left mouse button click
    , KE_LEFTRELEASE_NM = 70	// non-mappable left mouse button release

    , KE_S_XF1 = 71		// vt100 shifted function keys for xterm
    , KE_S_XF2 = 72
    , KE_S_XF3 = 73
    , KE_S_XF4 = 74

    // NOTE: The scroll wheel events are inverted: i.e. UP is the same as
    // moving the actual scroll wheel down, LEFT is the same as moving the
    // scroll wheel right.
    , KE_MOUSEDOWN = 75		// scroll wheel pseudo-button Down
    , KE_MOUSEUP = 76		// scroll wheel pseudo-button Up
    , KE_MOUSELEFT = 77		// scroll wheel pseudo-button Left
    , KE_MOUSERIGHT = 78	// scroll wheel pseudo-button Right

    , KE_KINS = 79		// keypad Insert key
    , KE_KDEL = 80		// keypad Delete key

    , KE_CSI = 81		// CSI typed directly
    , KE_SNR = 82		// <SNR>
    , KE_PLUG = 83		// <Plug>
    , KE_CMDWIN = 84		// open command-line window from Command-line
				// Mode

    , KE_C_ಎಡ = 85		// control-left
    , KE_C_ಬಲ = 86		// control-right
    , KE_C_HOME = 87		// control-home
    , KE_C_ಕೊನೆ = 88		// control-end

    , KE_X1MOUSE = 89		// X1/X2 mouse-buttons
    , KE_X1DRAG = 90
    , KE_X1RELEASE = 91
    , KE_X2MOUSE = 92
    , KE_X2DRAG = 93
    , KE_X2RELEASE = 94

    , KE_DROP = 95		// DnD data is available
    , KE_CURSORHOLD = 96	// CursorHold event
    , KE_NOP = 97		// doesn't do something
    , KE_FOCUSGAINED = 98	// focus gained
    , KE_FOCUSLOST = 99		// focus lost
    , KE_MOUSEMOVE = 100	// mouse moved with no button down
    , KE_MOUSEMOVE_XY = 101	// KE_MOUSEMOVE with coordinates
    , KE_CANCEL = 102		// return from vgetc()
    , KE_COMMAND = 103		// <Cmd> special key
    , KE_SCRIPT_COMMAND = 104	// <ScriptCmd> special key
    , KE_S_BS = 105		// shift + <BS>
    , KE_SID = 106		// <SID> special key, followed by {nr};
    , KE_ESC = 107		// used for K_ESC
    , KE_WILD = 108		// triggers wildmode completion
    , KE_OSC = 109		// finished OSC sequence
    , KE_COMPLETE_DELAY = 110	// 'autocompletedelay' expired
};

/*
 * The three-byte codes are replaced with a negative number when using vgetc().
 */
#ವ್ಯಾಖ್ಯಾನಿಸು K_ZERO		TERMCAP2_ಕೀಲಿ(KS_ZERO, KE_FILLER)

#ವ್ಯಾಖ್ಯಾನಿಸು K_ESC		TERMCAP2_ಕೀಲಿ(KS_EXTRA, KE_ESC)

#ವ್ಯಾಖ್ಯಾನಿಸು K_ಮೇಲೆ		TERMCAP2_ಕೀಲಿ('k', 'u')
#ವ್ಯಾಖ್ಯಾನಿಸು K_ಕೆಳಕ್ಕೆ		TERMCAP2_ಕೀಲಿ('k', 'd')
#ವ್ಯಾಖ್ಯಾನಿಸು K_ಎಡ		TERMCAP2_ಕೀಲಿ('k', 'l')
#ವ್ಯಾಖ್ಯಾನಿಸು K_ಬಲ		TERMCAP2_ಕೀಲಿ('k', 'r')
#ವ್ಯಾಖ್ಯಾನಿಸು K_S_ಮೇಲೆ		TERMCAP2_ಕೀಲಿ(KS_EXTRA, KE_S_ಮೇಲೆ)
#ವ್ಯಾಖ್ಯಾನಿಸು K_S_ಕೆಳಕ್ಕೆ	TERMCAP2_ಕೀಲಿ(KS_EXTRA, KE_S_ಕೆಳಕ್ಕೆ)
#ವ್ಯಾಖ್ಯಾನಿಸು K_S_ಎಡ	TERMCAP2_ಕೀಲಿ('#', '4')
#ವ್ಯಾಖ್ಯಾನಿಸು K_C_ಎಡ	TERMCAP2_ಕೀಲಿ(KS_EXTRA, KE_C_ಎಡ)
#ವ್ಯಾಖ್ಯಾನಿಸು K_S_ಬಲ	TERMCAP2_ಕೀಲಿ('%', 'i')
#ವ್ಯಾಖ್ಯಾನಿಸು K_C_ಬಲ	TERMCAP2_ಕೀಲಿ(KS_EXTRA, KE_C_ಬಲ)

#ವ್ಯಾಖ್ಯಾನಿಸು K_S_HOME	TERMCAP2_ಕೀಲಿ('#', '2')
#ವ್ಯಾಖ್ಯಾನಿಸು K_C_HOME	TERMCAP2_ಕೀಲಿ(KS_EXTRA, KE_C_HOME)
#ವ್ಯಾಖ್ಯಾನಿಸು K_S_ಕೊನೆ		TERMCAP2_ಕೀಲಿ('*', '7')
#ವ್ಯಾಖ್ಯಾನಿಸು K_C_ಕೊನೆ		TERMCAP2_ಕೀಲಿ(KS_EXTRA, KE_C_ಕೊನೆ)

#ವ್ಯಾಖ್ಯಾನಿಸು K_TAB		TERMCAP2_ಕೀಲಿ(KS_EXTRA, KE_TAB)
#ವ್ಯಾಖ್ಯಾನಿಸು K_S_TAB		TERMCAP2_ಕೀಲಿ('k', 'B')
#ವ್ಯಾಖ್ಯಾನಿಸು K_S_BS		TERMCAP2_ಕೀಲಿ(KS_EXTRA, KE_S_BS)

// extra set of function keys F1-F4, for vt100 compatible xterm
#ವ್ಯಾಖ್ಯಾನಿಸು K_XF1		TERMCAP2_ಕೀಲಿ(KS_EXTRA, KE_XF1)
#ವ್ಯಾಖ್ಯಾನಿಸು K_XF2		TERMCAP2_ಕೀಲಿ(KS_EXTRA, KE_XF2)
#ವ್ಯಾಖ್ಯಾನಿಸು K_XF3		TERMCAP2_ಕೀಲಿ(KS_EXTRA, KE_XF3)
#ವ್ಯಾಖ್ಯಾನಿಸು K_XF4		TERMCAP2_ಕೀಲಿ(KS_EXTRA, KE_XF4)

// extra set of cursor keys for vt100 compatible xterm
#ವ್ಯಾಖ್ಯಾನಿಸು K_XUP		TERMCAP2_ಕೀಲಿ(KS_EXTRA, KE_XUP)
#ವ್ಯಾಖ್ಯಾನಿಸು K_XDOWN		TERMCAP2_ಕೀಲಿ(KS_EXTRA, KE_XDOWN)
#ವ್ಯಾಖ್ಯಾನಿಸು K_XLEFT		TERMCAP2_ಕೀಲಿ(KS_EXTRA, KE_XLEFT)
#ವ್ಯಾಖ್ಯಾನಿಸು K_XRIGHT	TERMCAP2_ಕೀಲಿ(KS_EXTRA, KE_XRIGHT)

#ವ್ಯಾಖ್ಯಾನಿಸು K_F1		TERMCAP2_ಕೀಲಿ('k', '1')	// function keys
#ವ್ಯಾಖ್ಯಾನಿಸು K_F2		TERMCAP2_ಕೀಲಿ('k', '2')
#ವ್ಯಾಖ್ಯಾನಿಸು K_F3		TERMCAP2_ಕೀಲಿ('k', '3')
#ವ್ಯಾಖ್ಯಾನಿಸು K_F4		TERMCAP2_ಕೀಲಿ('k', '4')
#ವ್ಯಾಖ್ಯಾನಿಸು K_F5		TERMCAP2_ಕೀಲಿ('k', '5')
#ವ್ಯಾಖ್ಯಾನಿಸು K_F6		TERMCAP2_ಕೀಲಿ('k', '6')
#ವ್ಯಾಖ್ಯಾನಿಸು K_F7		TERMCAP2_ಕೀಲಿ('k', '7')
#ವ್ಯಾಖ್ಯಾನಿಸು K_F8		TERMCAP2_ಕೀಲಿ('k', '8')
#ವ್ಯಾಖ್ಯಾನಿಸು K_F9		TERMCAP2_ಕೀಲಿ('k', '9')
#ವ್ಯಾಖ್ಯಾನಿಸು K_F10		TERMCAP2_ಕೀಲಿ('k', ';')

#ವ್ಯಾಖ್ಯಾನಿಸು K_F11		TERMCAP2_ಕೀಲಿ('F', '1')
#ವ್ಯಾಖ್ಯಾನಿಸು K_F12		TERMCAP2_ಕೀಲಿ('F', '2')
#ವ್ಯಾಖ್ಯಾನಿಸು K_F13		TERMCAP2_ಕೀಲಿ('F', '3')
#ವ್ಯಾಖ್ಯಾನಿಸು K_F14		TERMCAP2_ಕೀಲಿ('F', '4')
#ವ್ಯಾಖ್ಯಾನಿಸು K_F15		TERMCAP2_ಕೀಲಿ('F', '5')
#ವ್ಯಾಖ್ಯಾನಿಸು K_F16		TERMCAP2_ಕೀಲಿ('F', '6')
#ವ್ಯಾಖ್ಯಾನಿಸು K_F17		TERMCAP2_ಕೀಲಿ('F', '7')
#ವ್ಯಾಖ್ಯಾನಿಸು K_F18		TERMCAP2_ಕೀಲಿ('F', '8')
#ವ್ಯಾಖ್ಯಾನಿಸು K_F19		TERMCAP2_ಕೀಲಿ('F', '9')
#ವ್ಯಾಖ್ಯಾನಿಸು K_F20		TERMCAP2_ಕೀಲಿ('F', 'A')

#ವ್ಯಾಖ್ಯಾನಿಸು K_F21		TERMCAP2_ಕೀಲಿ('F', 'B')
#ವ್ಯಾಖ್ಯಾನಿಸು K_F22		TERMCAP2_ಕೀಲಿ('F', 'C')
#ವ್ಯಾಖ್ಯಾನಿಸು K_F23		TERMCAP2_ಕೀಲಿ('F', 'D')
#ವ್ಯಾಖ್ಯಾನಿಸು K_F24		TERMCAP2_ಕೀಲಿ('F', 'E')
#ವ್ಯಾಖ್ಯಾನಿಸು K_F25		TERMCAP2_ಕೀಲಿ('F', 'F')
#ವ್ಯಾಖ್ಯಾನಿಸು K_F26		TERMCAP2_ಕೀಲಿ('F', 'G')
#ವ್ಯಾಖ್ಯಾನಿಸು K_F27		TERMCAP2_ಕೀಲಿ('F', 'H')
#ವ್ಯಾಖ್ಯಾನಿಸು K_F28		TERMCAP2_ಕೀಲಿ('F', 'I')
#ವ್ಯಾಖ್ಯಾನಿಸು K_F29		TERMCAP2_ಕೀಲಿ('F', 'J')
#ವ್ಯಾಖ್ಯಾನಿಸು K_F30		TERMCAP2_ಕೀಲಿ('F', 'K')

#ವ್ಯಾಖ್ಯಾನಿಸು K_F31		TERMCAP2_ಕೀಲಿ('F', 'L')
#ವ್ಯಾಖ್ಯಾನಿಸು K_F32		TERMCAP2_ಕೀಲಿ('F', 'M')
#ವ್ಯಾಖ್ಯಾನಿಸು K_F33		TERMCAP2_ಕೀಲಿ('F', 'N')
#ವ್ಯಾಖ್ಯಾನಿಸು K_F34		TERMCAP2_ಕೀಲಿ('F', 'O')
#ವ್ಯಾಖ್ಯಾನಿಸು K_F35		TERMCAP2_ಕೀಲಿ('F', 'P')
#ವ್ಯಾಖ್ಯಾನಿಸು K_F36		TERMCAP2_ಕೀಲಿ('F', 'Q')
#ವ್ಯಾಖ್ಯಾನಿಸು K_F37		TERMCAP2_ಕೀಲಿ('F', 'R')

// extra set of shifted function keys F1-F4, for vt100 compatible xterm
#ವ್ಯಾಖ್ಯಾನಿಸು K_S_XF1		TERMCAP2_ಕೀಲಿ(KS_EXTRA, KE_S_XF1)
#ವ್ಯಾಖ್ಯಾನಿಸು K_S_XF2		TERMCAP2_ಕೀಲಿ(KS_EXTRA, KE_S_XF2)
#ವ್ಯಾಖ್ಯಾನಿಸು K_S_XF3		TERMCAP2_ಕೀಲಿ(KS_EXTRA, KE_S_XF3)
#ವ್ಯಾಖ್ಯಾನಿಸು K_S_XF4		TERMCAP2_ಕೀಲಿ(KS_EXTRA, KE_S_XF4)

#ವ್ಯಾಖ್ಯಾನಿಸು K_S_F1		TERMCAP2_ಕೀಲಿ(KS_EXTRA, KE_S_F1)	// shifted func. keys
#ವ್ಯಾಖ್ಯಾನಿಸು K_S_F2		TERMCAP2_ಕೀಲಿ(KS_EXTRA, KE_S_F2)
#ವ್ಯಾಖ್ಯಾನಿಸು K_S_F3		TERMCAP2_ಕೀಲಿ(KS_EXTRA, KE_S_F3)
#ವ್ಯಾಖ್ಯಾನಿಸು K_S_F4		TERMCAP2_ಕೀಲಿ(KS_EXTRA, KE_S_F4)
#ವ್ಯಾಖ್ಯಾನಿಸು K_S_F5		TERMCAP2_ಕೀಲಿ(KS_EXTRA, KE_S_F5)
#ವ್ಯಾಖ್ಯಾನಿಸು K_S_F6		TERMCAP2_ಕೀಲಿ(KS_EXTRA, KE_S_F6)
#ವ್ಯಾಖ್ಯಾನಿಸು K_S_F7		TERMCAP2_ಕೀಲಿ(KS_EXTRA, KE_S_F7)
#ವ್ಯಾಖ್ಯಾನಿಸು K_S_F8		TERMCAP2_ಕೀಲಿ(KS_EXTRA, KE_S_F8)
#ವ್ಯಾಖ್ಯಾನಿಸು K_S_F9		TERMCAP2_ಕೀಲಿ(KS_EXTRA, KE_S_F9)
#ವ್ಯಾಖ್ಯಾನಿಸು K_S_F10		TERMCAP2_ಕೀಲಿ(KS_EXTRA, KE_S_F10)

#ವ್ಯಾಖ್ಯಾನಿಸು K_S_F11		TERMCAP2_ಕೀಲಿ(KS_EXTRA, KE_S_F11)
#ವ್ಯಾಖ್ಯಾನಿಸು K_S_F12		TERMCAP2_ಕೀಲಿ(KS_EXTRA, KE_S_F12)
// K_S_F13 to K_S_F37  are currently not used

#ವ್ಯಾಖ್ಯಾನಿಸು K_HELP		TERMCAP2_ಕೀಲಿ('%', '1')
#ವ್ಯಾಖ್ಯಾನಿಸು K_UNDO		TERMCAP2_ಕೀಲಿ('&', '8')

#ವ್ಯಾಖ್ಯಾನಿಸು K_BS		TERMCAP2_ಕೀಲಿ('k', 'b')

#ವ್ಯಾಖ್ಯಾನಿಸು K_INS		TERMCAP2_ಕೀಲಿ('k', 'I')
#ವ್ಯಾಖ್ಯಾನಿಸು K_KINS		TERMCAP2_ಕೀಲಿ(KS_EXTRA, KE_KINS)
#ವ್ಯಾಖ್ಯಾನಿಸು K_DEL		TERMCAP2_ಕೀಲಿ('k', 'D')
#ವ್ಯಾಖ್ಯಾನಿಸು K_KDEL		TERMCAP2_ಕೀಲಿ(KS_EXTRA, KE_KDEL)
#ವ್ಯಾಖ್ಯಾನಿಸು K_HOME		TERMCAP2_ಕೀಲಿ('k', 'h')
#ವ್ಯಾಖ್ಯಾನಿಸು K_KHOME		TERMCAP2_ಕೀಲಿ('K', '1')	// keypad home (upper left)
#ವ್ಯಾಖ್ಯಾನಿಸು K_XHOME		TERMCAP2_ಕೀಲಿ(KS_EXTRA, KE_XHOME)
#ವ್ಯಾಖ್ಯಾನಿಸು K_ZHOME		TERMCAP2_ಕೀಲಿ(KS_EXTRA, KE_ZHOME)
#ವ್ಯಾಖ್ಯಾನಿಸು K_ಕೊನೆ		TERMCAP2_ಕೀಲಿ('@', '7')
#ವ್ಯಾಖ್ಯಾನಿಸು K_KEND		TERMCAP2_ಕೀಲಿ('K', '4')	// keypad end (lower left)
#ವ್ಯಾಖ್ಯಾನಿಸು K_XEND		TERMCAP2_ಕೀಲಿ(KS_EXTRA, KE_XEND)
#ವ್ಯಾಖ್ಯಾನಿಸು K_ZEND		TERMCAP2_ಕೀಲಿ(KS_EXTRA, KE_ZEND)
#ವ್ಯಾಖ್ಯಾನಿಸು K_PAGEUP	TERMCAP2_ಕೀಲಿ('k', 'P')
#ವ್ಯಾಖ್ಯಾನಿಸು K_PAGEDOWN	TERMCAP2_ಕೀಲಿ('k', 'N')
#ವ್ಯಾಖ್ಯಾನಿಸು K_KPAGEUP	TERMCAP2_ಕೀಲಿ('K', '3')	// keypad pageup (upper R.)
#ವ್ಯಾಖ್ಯಾನಿಸು K_KPAGEDOWN	TERMCAP2_ಕೀಲಿ('K', '5')	// keypad pagedown (lower R.)

#ವ್ಯಾಖ್ಯಾನಿಸು K_KPLUS		TERMCAP2_ಕೀಲಿ('K', '6')	// keypad plus
#ವ್ಯಾಖ್ಯಾನಿಸು K_KMINUS	TERMCAP2_ಕೀಲಿ('K', '7')	// keypad minus
#ವ್ಯಾಖ್ಯಾನಿಸು K_KDIVIDE	TERMCAP2_ಕೀಲಿ('K', '8')	// keypad /
#ವ್ಯಾಖ್ಯಾನಿಸು K_KMULTIPLY	TERMCAP2_ಕೀಲಿ('K', '9')	// keypad *
#ವ್ಯಾಖ್ಯಾನಿಸು K_KENTER	TERMCAP2_ಕೀಲಿ('K', 'A')	// keypad Enter
#ವ್ಯಾಖ್ಯಾನಿಸು K_KPOINT	TERMCAP2_ಕೀಲಿ('K', 'B')	// keypad . or ,
#ವ್ಯಾಖ್ಯಾನಿಸು K_PS		TERMCAP2_ಕೀಲಿ('P', 'S')	// paste start
#ವ್ಯಾಖ್ಯಾನಿಸು K_PE		TERMCAP2_ಕೀಲಿ('P', 'E')	// paste end

#ವ್ಯಾಖ್ಯಾನಿಸು K_K0		TERMCAP2_ಕೀಲಿ('K', 'C')	// keypad 0
#ವ್ಯಾಖ್ಯಾನಿಸು K_K1		TERMCAP2_ಕೀಲಿ('K', 'D')	// keypad 1
#ವ್ಯಾಖ್ಯಾನಿಸು K_K2		TERMCAP2_ಕೀಲಿ('K', 'E')	// keypad 2
#ವ್ಯಾಖ್ಯಾನಿಸು K_K3		TERMCAP2_ಕೀಲಿ('K', 'F')	// keypad 3
#ವ್ಯಾಖ್ಯಾನಿಸು K_K4		TERMCAP2_ಕೀಲಿ('K', 'G')	// keypad 4
#ವ್ಯಾಖ್ಯಾನಿಸು K_K5		TERMCAP2_ಕೀಲಿ('K', 'H')	// keypad 5
#ವ್ಯಾಖ್ಯಾನಿಸು K_K6		TERMCAP2_ಕೀಲಿ('K', 'I')	// keypad 6
#ವ್ಯಾಖ್ಯಾನಿಸು K_K7		TERMCAP2_ಕೀಲಿ('K', 'J')	// keypad 7
#ವ್ಯಾಖ್ಯಾನಿಸು K_K8		TERMCAP2_ಕೀಲಿ('K', 'K')	// keypad 8
#ವ್ಯಾಖ್ಯಾನಿಸು K_K9		TERMCAP2_ಕೀಲಿ('K', 'L')	// keypad 9

#ವ್ಯಾಖ್ಯಾನಿಸು K_MOUSE		TERMCAP2_ಕೀಲಿ(KS_MOUSE, KE_FILLER)
#ವ್ಯಾಖ್ಯಾನಿಸು K_MENU		TERMCAP2_ಕೀಲಿ(KS_MENU, KE_FILLER)
#ವ್ಯಾಖ್ಯಾನಿಸು K_VER_SCROLLBAR	TERMCAP2_ಕೀಲಿ(KS_VER_SCROLLBAR, KE_FILLER)
#ವ್ಯಾಖ್ಯಾನಿಸು K_HOR_SCROLLBAR   TERMCAP2_ಕೀಲಿ(KS_HOR_SCROLLBAR, KE_FILLER)

#ವ್ಯಾಖ್ಯಾನಿಸು K_NETTERM_MOUSE	TERMCAP2_ಕೀಲಿ(KS_NETTERM_MOUSE, KE_FILLER)
#ವ್ಯಾಖ್ಯಾನಿಸು K_DEC_MOUSE	TERMCAP2_ಕೀಲಿ(KS_DEC_MOUSE, KE_FILLER)
#ವ್ಯಾಖ್ಯಾನಿಸು K_JSBTERM_MOUSE	TERMCAP2_ಕೀಲಿ(KS_JSBTERM_MOUSE, KE_FILLER)
#ವ್ಯಾಖ್ಯಾನಿಸು K_PTERM_MOUSE	TERMCAP2_ಕೀಲಿ(KS_PTERM_MOUSE, KE_FILLER)
#ವ್ಯಾಖ್ಯಾನಿಸು K_URXVT_MOUSE	TERMCAP2_ಕೀಲಿ(KS_URXVT_MOUSE, KE_FILLER)
#ವ್ಯಾಖ್ಯಾನಿಸು K_SGR_MOUSE	TERMCAP2_ಕೀಲಿ(KS_SGR_MOUSE, KE_FILLER)
#ವ್ಯಾಖ್ಯಾನಿಸು K_SGR_MOUSERELEASE TERMCAP2_ಕೀಲಿ(KS_SGR_MOUSE_RELEASE, KE_FILLER)

#ವ್ಯಾಖ್ಯಾನಿಸು K_SELECT	TERMCAP2_ಕೀಲಿ(KS_SELECT, KE_FILLER)
#ವ್ಯಾಖ್ಯಾನಿಸು K_TEAROFF	TERMCAP2_ಕೀಲಿ(KS_TEAROFF, KE_FILLER)

#ವ್ಯಾಖ್ಯಾನಿಸು K_TABLINE	TERMCAP2_ಕೀಲಿ(KS_TABLINE, KE_FILLER)
#ವ್ಯಾಖ್ಯಾನಿಸು K_TABMENU	TERMCAP2_ಕೀಲಿ(KS_TABMENU, KE_FILLER)

/*
 * Symbols for pseudo keys which are translated from the real key symbols
 * above.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು K_LEFTMOUSE	TERMCAP2_ಕೀಲಿ(KS_EXTRA, KE_LEFTMOUSE)
#ವ್ಯಾಖ್ಯಾನಿಸು K_LEFTMOUSE_NM	TERMCAP2_ಕೀಲಿ(KS_EXTRA, KE_LEFTMOUSE_NM)
#ವ್ಯಾಖ್ಯಾನಿಸು K_LEFTDRAG	TERMCAP2_ಕೀಲಿ(KS_EXTRA, KE_LEFTDRAG)
#ವ್ಯಾಖ್ಯಾನಿಸು K_LEFTRELEASE	TERMCAP2_ಕೀಲಿ(KS_EXTRA, KE_LEFTRELEASE)
#ವ್ಯಾಖ್ಯಾನಿಸು K_LEFTRELEASE_NM TERMCAP2_ಕೀಲಿ(KS_EXTRA, KE_LEFTRELEASE_NM)
#ವ್ಯಾಖ್ಯಾನಿಸು K_MOUSEMOVE	TERMCAP2_ಕೀಲಿ(KS_EXTRA, KE_MOUSEMOVE)
#ವ್ಯಾಖ್ಯಾನಿಸು K_MIDDLEMOUSE	TERMCAP2_ಕೀಲಿ(KS_EXTRA, KE_MIDDLEMOUSE)
#ವ್ಯಾಖ್ಯಾನಿಸು K_MIDDLEDRAG	TERMCAP2_ಕೀಲಿ(KS_EXTRA, KE_MIDDLEDRAG)
#ವ್ಯಾಖ್ಯಾನಿಸು K_MIDDLERELEASE	TERMCAP2_ಕೀಲಿ(KS_EXTRA, KE_MIDDLERELEASE)
#ವ್ಯಾಖ್ಯಾನಿಸು K_RIGHTMOUSE	TERMCAP2_ಕೀಲಿ(KS_EXTRA, KE_RIGHTMOUSE)
#ವ್ಯಾಖ್ಯಾನಿಸು K_RIGHTDRAG	TERMCAP2_ಕೀಲಿ(KS_EXTRA, KE_RIGHTDRAG)
#ವ್ಯಾಖ್ಯಾನಿಸು K_RIGHTRELEASE	TERMCAP2_ಕೀಲಿ(KS_EXTRA, KE_RIGHTRELEASE)
#ವ್ಯಾಖ್ಯಾನಿಸು K_X1MOUSE	TERMCAP2_ಕೀಲಿ(KS_EXTRA, KE_X1MOUSE)
#ವ್ಯಾಖ್ಯಾನಿಸು K_X1MOUSE	TERMCAP2_ಕೀಲಿ(KS_EXTRA, KE_X1MOUSE)
#ವ್ಯಾಖ್ಯಾನಿಸು K_X1DRAG	TERMCAP2_ಕೀಲಿ(KS_EXTRA, KE_X1DRAG)
#ವ್ಯಾಖ್ಯಾನಿಸು K_X1RELEASE     TERMCAP2_ಕೀಲಿ(KS_EXTRA, KE_X1RELEASE)
#ವ್ಯಾಖ್ಯಾನಿಸು K_X2MOUSE	TERMCAP2_ಕೀಲಿ(KS_EXTRA, KE_X2MOUSE)
#ವ್ಯಾಖ್ಯಾನಿಸು K_X2DRAG	TERMCAP2_ಕೀಲಿ(KS_EXTRA, KE_X2DRAG)
#ವ್ಯಾಖ್ಯಾನಿಸು K_X2RELEASE     TERMCAP2_ಕೀಲಿ(KS_EXTRA, KE_X2RELEASE)

#ವ್ಯಾಖ್ಯಾನಿಸು K_IGNORE	TERMCAP2_ಕೀಲಿ(KS_EXTRA, KE_IGNORE)
#ವ್ಯಾಖ್ಯಾನಿಸು K_NOP		TERMCAP2_ಕೀಲಿ(KS_EXTRA, KE_NOP)
#ವ್ಯಾಖ್ಯಾನಿಸು K_CANCEL	TERMCAP2_ಕೀಲಿ(KS_EXTRA, KE_CANCEL)

#ವ್ಯಾಖ್ಯಾನಿಸು K_MOUSEDOWN	TERMCAP2_ಕೀಲಿ(KS_EXTRA, KE_MOUSEDOWN)
#ವ್ಯಾಖ್ಯಾನಿಸು K_MOUSEUP	TERMCAP2_ಕೀಲಿ(KS_EXTRA, KE_MOUSEUP)
#ವ್ಯಾಖ್ಯಾನಿಸು K_MOUSELEFT	TERMCAP2_ಕೀಲಿ(KS_EXTRA, KE_MOUSELEFT)
#ವ್ಯಾಖ್ಯಾನಿಸು K_MOUSERIGHT	TERMCAP2_ಕೀಲಿ(KS_EXTRA, KE_MOUSERIGHT)

#ವ್ಯಾಖ್ಯಾನಿಸು K_CSI		TERMCAP2_ಕೀಲಿ(KS_EXTRA, KE_CSI)
#ವ್ಯಾಖ್ಯಾನಿಸು K_OSC		TERMCAP2_ಕೀಲಿ(KS_EXTRA, KE_OSC)
#ವ್ಯಾಖ್ಯಾನಿಸು K_SNR		TERMCAP2_ಕೀಲಿ(KS_EXTRA, KE_SNR)
#ವ್ಯಾಖ್ಯಾನಿಸು K_PLUG		TERMCAP2_ಕೀಲಿ(KS_EXTRA, KE_PLUG)
#ವ್ಯಾಖ್ಯಾನಿಸು K_CMDWIN	TERMCAP2_ಕೀಲಿ(KS_EXTRA, KE_CMDWIN)

#ವ್ಯಾಖ್ಯಾನಿಸು K_DROP		TERMCAP2_ಕೀಲಿ(KS_EXTRA, KE_DROP)
#ವ್ಯಾಖ್ಯಾನಿಸು K_FOCUSGAINED	TERMCAP2_ಕೀಲಿ(KS_EXTRA, KE_FOCUSGAINED)
#ವ್ಯಾಖ್ಯಾನಿಸು K_FOCUSLOST	TERMCAP2_ಕೀಲಿ(KS_EXTRA, KE_FOCUSLOST)

#ವ್ಯಾಖ್ಯಾನಿಸು K_CURSORHOLD	TERMCAP2_ಕೀಲಿ(KS_EXTRA, KE_CURSORHOLD)
#ವ್ಯಾಖ್ಯಾನಿಸು K_COMPLETE_DELAY TERMCAP2_ಕೀಲಿ(KS_EXTRA, KE_COMPLETE_DELAY)

#ವ್ಯಾಖ್ಯಾನಿಸು K_COMMAND	TERMCAP2_ಕೀಲಿ(KS_EXTRA, KE_COMMAND)
#ವ್ಯಾಖ್ಯಾನಿಸು K_SCRIPT_COMMAND TERMCAP2_ಕೀಲಿ(KS_EXTRA, KE_SCRIPT_COMMAND)
#ವ್ಯಾಖ್ಯಾನಿಸು K_SID		TERMCAP2_ಕೀಲಿ(KS_EXTRA, KE_SID)

#ವ್ಯಾಖ್ಯಾನಿಸು K_WILD		TERMCAP2_ಕೀಲಿ(KS_EXTRA, KE_WILD)

// Bits for modifier mask
// 0x01 cannot be used, because the modifier must be 0x02 or higher
#ವ್ಯಾಖ್ಯಾನಿಸು MOD_ಮುಖವಾಡ_ಸರಿಸು	    0x02
#ವ್ಯಾಖ್ಯಾನಿಸು MOD_ಮುಖವಾಡ_CTRL	    0x04
#ವ್ಯಾಖ್ಯಾನಿಸು MOD_ಮುಖವಾಡ_ALT	    0x08	// aka META
#ವ್ಯಾಖ್ಯಾನಿಸು MOD_ಮುಖವಾಡ_META	    0x10	// META when it's different from ALT
#ವ್ಯಾಖ್ಯಾನಿಸು MOD_ಮುಖವಾಡ_2_CLICK	    0x20	// use MOD_MASK_MULTI_CLICK
#ವ್ಯಾಖ್ಯಾನಿಸು MOD_ಮುಖವಾಡ_3_CLICK	    0x40	// use MOD_MASK_MULTI_CLICK
#ವ್ಯಾಖ್ಯಾನಿಸು MOD_ಮುಖವಾಡ_4_CLICK	    0x60	// use MOD_MASK_MULTI_CLICK
#ಆದರೆ defined(MACOS_X) || defined(FEAT_GUI_GTK)
# define MOD_ಮುಖವಾಡ_CMD	    0x80        // aka SUPER
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು MOD_ಮುಖವಾಡ_MULTI_CLICK	(MOD_ಮುಖವಾಡ_2_CLICK|MOD_ಮುಖವಾಡ_3_CLICK|MOD_ಮುಖವಾಡ_4_CLICK)

/*
 * The length of the longest special key name, including modifiers.
 * Current longest is <M-C-S-T-D-A-4-ScrollWheelRight> (length includes '<' and
 * '>').
 */
#ವ್ಯಾಖ್ಯಾನಿಸು MAX_ಕೀಲಿ_NAME_LEN    32

/*
 * Maximum length of a special key event as tokens.  This includes modifiers.
 * The longest event is something like <M-C-S-T-4-LeftDrag> which would be the
 * following string of tokens:
 *
 * <K_SPECIAL> <KS_MODIFIER> bitmask <K_SPECIAL> <KS_EXTRA> <KT_LEFTDRAG>.
 *
 * This is a total of 6 tokens, and is currently the longest one possible.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು MAX_ಕೀಲಿ_ಸಂಕೇತ_LEN    6
