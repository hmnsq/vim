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
#تعریف_کړه K_NUL			(0xce)	// for MSDOS: special key follows

/*
 * K_SPECIAL is the first byte of a special key code and is always followed by
 * two bytes.
 * The second byte can have any value. ASCII is used for normal termcap
 * entries, 0x80 and higher for special keys, see below.
 * The third byte is guaranteed to be between 0x02 and 0x7f.
 */

#تعریف_کړه K_SPECIAL		(0x80)

/*
 * Positive characters are "normal" characters.
 * Negative characters are special key codes.  Only characters below -0x200
 * are used to so that the absolute value can't be mistaken for a single-byte
 * character.
 */
#تعریف_کړه IS_SPECIAL(c)		((c) < 0)

/*
 * Characters 0x0100 - 0x01ff have a special meaning for abbreviations.
 * Multi-byte characters also have ABBR_OFF added, thus are above 0x0200.
 */
#تعریف_کړه ABBR_OFF		0x100

/*
 * NUL cannot be in the input string, therefore it is replaced by
 *	K_SPECIAL   KS_ZERO	KE_FILLER
 */
#تعریف_کړه KS_ZERO			255

/*
 * K_SPECIAL cannot be in the input string, therefore it is replaced by
 *	K_SPECIAL   KS_SPECIAL	KE_FILLER
 */
#تعریف_کړه KS_SPECIAL		254

/*
 * KS_EXTRA is used for keys that have no termcap name
 *	K_SPECIAL   KS_EXTRA	KE_xxx
 */
#تعریف_کړه KS_EXTRA		253

/*
 * KS_MODIFIER is used when a modifier is given for a (special) key
 *	K_SPECIAL   KS_MODIFIER	bitmask
 */
#تعریف_کړه KS_MODIFIER		252

/*
 * These are used for the GUI
 *	K_SPECIAL   KS_xxx	KE_FILLER
 */
#تعریف_کړه KS_MOUSE		251
#تعریف_کړه KS_MENU			250
#تعریف_کړه KS_VER_SCROLLBAR	249
#تعریف_کړه KS_HOR_SCROLLBAR	248

/*
 * These are used for DEC mouse
 */
#تعریف_کړه KS_NETTERM_MOUSE	247
#تعریف_کړه KS_DEC_MOUSE		246

/*
 * Used for switching Select mode back on after a mapping or menu.
 */
#تعریف_کړه KS_SELECT		245
#تعریف_کړه K_SELECT_STRING		(char_u *)"\200\365X"

/*
 * Used for tearing off a menu.
 */
#تعریف_کړه KS_TEAROFF		244

// Used for JSB term mouse.
#تعریف_کړه KS_JSBTERM_MOUSE	243

// Used a termcap entry that produces a normal character.
#تعریف_کړه KS_کلی			242

// Used for the qnx pterm mouse.
#تعریف_کړه KS_PTERM_MOUSE		241

// Used for click in a tab pages label.
#تعریف_کړه KS_TABLINE		240

// Used for menu in a tab pages line.
#تعریف_کړه KS_TABMENU		239

// Used for the urxvt mouse.
#تعریف_کړه KS_URXVT_MOUSE		238

// Used for the sgr mouse.
#تعریف_کړه KS_SGR_MOUSE		237
#تعریف_کړه KS_SGR_MOUSE_RELEASE	236

// Used for the GPM mouse.
#تعریف_کړه KS_GPM_MOUSE		235

/*
 * Filler used after KS_SPECIAL and others
 */
#تعریف_کړه KE_FILLER		('X')

/*
 * translation of three byte code "K_SPECIAL a b" into int "K_xxx" and back
 */
#تعریف_کړه TERMCAP2_کلی(a, b)	(-((a) + ((صحيح)(b) << 8)))
#تعریف_کړه KEY2TERMCAP0(x)		((-(x)) & 0xff)
#تعریف_کړه KEY2TERMCAP1(x)		(((بینښه)(-(x)) >> 8) & 0xff)

/*
 * get second or third byte when translating special key code into three bytes
 */
#تعریف_کړه K_دوهم(c)	((c) == K_SPECIAL ? KS_SPECIAL : (c) == NUL ? KS_ZERO : KEY2TERMCAP0(c))

#تعریف_کړه K_دریم(c)	(((c) == K_SPECIAL || (c) == NUL) ? KE_FILLER : KEY2TERMCAP1(c))

/*
 * get single int code from second byte after K_SPECIAL
 */
#تعریف_کړه منزل_SPECIAL(a, b)    ((a) == KS_SPECIAL ? K_SPECIAL : (a) == KS_ZERO ? K_ZERO : TERMCAP2_کلی(a, b))

/*
 * Codes for keys that do not have a termcap name.
 * The numbers are fixed to make sure that recorded key sequences remain valid.
 * Add new entries at the end, not halfway.
 *
 * K_SPECIAL KS_EXTRA KE_xxx
 */
تعداد کلی_extra
{
      KE_S_پورته = 4		// shift-up
    , KE_S_ښکته = 5		// shift-down

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

    , KE_SNIFF_ناکارول_شوی = 56	// obsolete
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

    , KE_C_کیڼ = 85		// control-left
    , KE_C_ښی = 86		// control-right
    , KE_C_HOME = 87		// control-home
    , KE_C_پای_ټکی = 88		// control-end

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
#تعریف_کړه K_ZERO		TERMCAP2_کلی(KS_ZERO, KE_FILLER)

#تعریف_کړه K_ESC		TERMCAP2_کلی(KS_EXTRA, KE_ESC)

#تعریف_کړه K_پورته		TERMCAP2_کلی('k', 'u')
#تعریف_کړه K_ښکته		TERMCAP2_کلی('k', 'd')
#تعریف_کړه K_کیڼ		TERMCAP2_کلی('k', 'l')
#تعریف_کړه K_ښی		TERMCAP2_کلی('k', 'r')
#تعریف_کړه K_S_پورته		TERMCAP2_کلی(KS_EXTRA, KE_S_پورته)
#تعریف_کړه K_S_ښکته	TERMCAP2_کلی(KS_EXTRA, KE_S_ښکته)
#تعریف_کړه K_S_کیڼ	TERMCAP2_کلی('#', '4')
#تعریف_کړه K_C_کیڼ	TERMCAP2_کلی(KS_EXTRA, KE_C_کیڼ)
#تعریف_کړه K_S_ښی	TERMCAP2_کلی('%', 'i')
#تعریف_کړه K_C_ښی	TERMCAP2_کلی(KS_EXTRA, KE_C_ښی)

#تعریف_کړه K_S_HOME	TERMCAP2_کلی('#', '2')
#تعریف_کړه K_C_HOME	TERMCAP2_کلی(KS_EXTRA, KE_C_HOME)
#تعریف_کړه K_S_پای_ټکی		TERMCAP2_کلی('*', '7')
#تعریف_کړه K_C_پای_ټکی		TERMCAP2_کلی(KS_EXTRA, KE_C_پای_ټکی)

#تعریف_کړه K_TAB		TERMCAP2_کلی(KS_EXTRA, KE_TAB)
#تعریف_کړه K_S_TAB		TERMCAP2_کلی('k', 'B')
#تعریف_کړه K_S_BS		TERMCAP2_کلی(KS_EXTRA, KE_S_BS)

// extra set of function keys F1-F4, for vt100 compatible xterm
#تعریف_کړه K_XF1		TERMCAP2_کلی(KS_EXTRA, KE_XF1)
#تعریف_کړه K_XF2		TERMCAP2_کلی(KS_EXTRA, KE_XF2)
#تعریف_کړه K_XF3		TERMCAP2_کلی(KS_EXTRA, KE_XF3)
#تعریف_کړه K_XF4		TERMCAP2_کلی(KS_EXTRA, KE_XF4)

// extra set of cursor keys for vt100 compatible xterm
#تعریف_کړه K_XUP		TERMCAP2_کلی(KS_EXTRA, KE_XUP)
#تعریف_کړه K_XDOWN		TERMCAP2_کلی(KS_EXTRA, KE_XDOWN)
#تعریف_کړه K_XLEFT		TERMCAP2_کلی(KS_EXTRA, KE_XLEFT)
#تعریف_کړه K_XRIGHT	TERMCAP2_کلی(KS_EXTRA, KE_XRIGHT)

#تعریف_کړه K_F1		TERMCAP2_کلی('k', '1')	// function keys
#تعریف_کړه K_F2		TERMCAP2_کلی('k', '2')
#تعریف_کړه K_F3		TERMCAP2_کلی('k', '3')
#تعریف_کړه K_F4		TERMCAP2_کلی('k', '4')
#تعریف_کړه K_F5		TERMCAP2_کلی('k', '5')
#تعریف_کړه K_F6		TERMCAP2_کلی('k', '6')
#تعریف_کړه K_F7		TERMCAP2_کلی('k', '7')
#تعریف_کړه K_F8		TERMCAP2_کلی('k', '8')
#تعریف_کړه K_F9		TERMCAP2_کلی('k', '9')
#تعریف_کړه K_F10		TERMCAP2_کلی('k', ';')

#تعریف_کړه K_F11		TERMCAP2_کلی('F', '1')
#تعریف_کړه K_F12		TERMCAP2_کلی('F', '2')
#تعریف_کړه K_F13		TERMCAP2_کلی('F', '3')
#تعریف_کړه K_F14		TERMCAP2_کلی('F', '4')
#تعریف_کړه K_F15		TERMCAP2_کلی('F', '5')
#تعریف_کړه K_F16		TERMCAP2_کلی('F', '6')
#تعریف_کړه K_F17		TERMCAP2_کلی('F', '7')
#تعریف_کړه K_F18		TERMCAP2_کلی('F', '8')
#تعریف_کړه K_F19		TERMCAP2_کلی('F', '9')
#تعریف_کړه K_F20		TERMCAP2_کلی('F', 'A')

#تعریف_کړه K_F21		TERMCAP2_کلی('F', 'B')
#تعریف_کړه K_F22		TERMCAP2_کلی('F', 'C')
#تعریف_کړه K_F23		TERMCAP2_کلی('F', 'D')
#تعریف_کړه K_F24		TERMCAP2_کلی('F', 'E')
#تعریف_کړه K_F25		TERMCAP2_کلی('F', 'F')
#تعریف_کړه K_F26		TERMCAP2_کلی('F', 'G')
#تعریف_کړه K_F27		TERMCAP2_کلی('F', 'H')
#تعریف_کړه K_F28		TERMCAP2_کلی('F', 'I')
#تعریف_کړه K_F29		TERMCAP2_کلی('F', 'J')
#تعریف_کړه K_F30		TERMCAP2_کلی('F', 'K')

#تعریف_کړه K_F31		TERMCAP2_کلی('F', 'L')
#تعریف_کړه K_F32		TERMCAP2_کلی('F', 'M')
#تعریف_کړه K_F33		TERMCAP2_کلی('F', 'N')
#تعریف_کړه K_F34		TERMCAP2_کلی('F', 'O')
#تعریف_کړه K_F35		TERMCAP2_کلی('F', 'P')
#تعریف_کړه K_F36		TERMCAP2_کلی('F', 'Q')
#تعریف_کړه K_F37		TERMCAP2_کلی('F', 'R')

// extra set of shifted function keys F1-F4, for vt100 compatible xterm
#تعریف_کړه K_S_XF1		TERMCAP2_کلی(KS_EXTRA, KE_S_XF1)
#تعریف_کړه K_S_XF2		TERMCAP2_کلی(KS_EXTRA, KE_S_XF2)
#تعریف_کړه K_S_XF3		TERMCAP2_کلی(KS_EXTRA, KE_S_XF3)
#تعریف_کړه K_S_XF4		TERMCAP2_کلی(KS_EXTRA, KE_S_XF4)

#تعریف_کړه K_S_F1		TERMCAP2_کلی(KS_EXTRA, KE_S_F1)	// shifted func. keys
#تعریف_کړه K_S_F2		TERMCAP2_کلی(KS_EXTRA, KE_S_F2)
#تعریف_کړه K_S_F3		TERMCAP2_کلی(KS_EXTRA, KE_S_F3)
#تعریف_کړه K_S_F4		TERMCAP2_کلی(KS_EXTRA, KE_S_F4)
#تعریف_کړه K_S_F5		TERMCAP2_کلی(KS_EXTRA, KE_S_F5)
#تعریف_کړه K_S_F6		TERMCAP2_کلی(KS_EXTRA, KE_S_F6)
#تعریف_کړه K_S_F7		TERMCAP2_کلی(KS_EXTRA, KE_S_F7)
#تعریف_کړه K_S_F8		TERMCAP2_کلی(KS_EXTRA, KE_S_F8)
#تعریف_کړه K_S_F9		TERMCAP2_کلی(KS_EXTRA, KE_S_F9)
#تعریف_کړه K_S_F10		TERMCAP2_کلی(KS_EXTRA, KE_S_F10)

#تعریف_کړه K_S_F11		TERMCAP2_کلی(KS_EXTRA, KE_S_F11)
#تعریف_کړه K_S_F12		TERMCAP2_کلی(KS_EXTRA, KE_S_F12)
// K_S_F13 to K_S_F37  are currently not used

#تعریف_کړه K_HELP		TERMCAP2_کلی('%', '1')
#تعریف_کړه K_UNDO		TERMCAP2_کلی('&', '8')

#تعریف_کړه K_BS		TERMCAP2_کلی('k', 'b')

#تعریف_کړه K_INS		TERMCAP2_کلی('k', 'I')
#تعریف_کړه K_KINS		TERMCAP2_کلی(KS_EXTRA, KE_KINS)
#تعریف_کړه K_DEL		TERMCAP2_کلی('k', 'D')
#تعریف_کړه K_KDEL		TERMCAP2_کلی(KS_EXTRA, KE_KDEL)
#تعریف_کړه K_HOME		TERMCAP2_کلی('k', 'h')
#تعریف_کړه K_KHOME		TERMCAP2_کلی('K', '1')	// keypad home (upper left)
#تعریف_کړه K_XHOME		TERMCAP2_کلی(KS_EXTRA, KE_XHOME)
#تعریف_کړه K_ZHOME		TERMCAP2_کلی(KS_EXTRA, KE_ZHOME)
#تعریف_کړه K_پای_ټکی		TERMCAP2_کلی('@', '7')
#تعریف_کړه K_KEND		TERMCAP2_کلی('K', '4')	// keypad end (lower left)
#تعریف_کړه K_XEND		TERMCAP2_کلی(KS_EXTRA, KE_XEND)
#تعریف_کړه K_ZEND		TERMCAP2_کلی(KS_EXTRA, KE_ZEND)
#تعریف_کړه K_PAGEUP	TERMCAP2_کلی('k', 'P')
#تعریف_کړه K_PAGEDOWN	TERMCAP2_کلی('k', 'N')
#تعریف_کړه K_KPAGEUP	TERMCAP2_کلی('K', '3')	// keypad pageup (upper R.)
#تعریف_کړه K_KPAGEDOWN	TERMCAP2_کلی('K', '5')	// keypad pagedown (lower R.)

#تعریف_کړه K_KPLUS		TERMCAP2_کلی('K', '6')	// keypad plus
#تعریف_کړه K_KMINUS	TERMCAP2_کلی('K', '7')	// keypad minus
#تعریف_کړه K_KDIVIDE	TERMCAP2_کلی('K', '8')	// keypad /
#تعریف_کړه K_KMULTIPLY	TERMCAP2_کلی('K', '9')	// keypad *
#تعریف_کړه K_KENTER	TERMCAP2_کلی('K', 'A')	// keypad Enter
#تعریف_کړه K_KPOINT	TERMCAP2_کلی('K', 'B')	// keypad . or ,
#تعریف_کړه K_PS		TERMCAP2_کلی('P', 'S')	// paste start
#تعریف_کړه K_PE		TERMCAP2_کلی('P', 'E')	// paste end

#تعریف_کړه K_K0		TERMCAP2_کلی('K', 'C')	// keypad 0
#تعریف_کړه K_K1		TERMCAP2_کلی('K', 'D')	// keypad 1
#تعریف_کړه K_K2		TERMCAP2_کلی('K', 'E')	// keypad 2
#تعریف_کړه K_K3		TERMCAP2_کلی('K', 'F')	// keypad 3
#تعریف_کړه K_K4		TERMCAP2_کلی('K', 'G')	// keypad 4
#تعریف_کړه K_K5		TERMCAP2_کلی('K', 'H')	// keypad 5
#تعریف_کړه K_K6		TERMCAP2_کلی('K', 'I')	// keypad 6
#تعریف_کړه K_K7		TERMCAP2_کلی('K', 'J')	// keypad 7
#تعریف_کړه K_K8		TERMCAP2_کلی('K', 'K')	// keypad 8
#تعریف_کړه K_K9		TERMCAP2_کلی('K', 'L')	// keypad 9

#تعریف_کړه K_MOUSE		TERMCAP2_کلی(KS_MOUSE, KE_FILLER)
#تعریف_کړه K_MENU		TERMCAP2_کلی(KS_MENU, KE_FILLER)
#تعریف_کړه K_VER_SCROLLBAR	TERMCAP2_کلی(KS_VER_SCROLLBAR, KE_FILLER)
#تعریف_کړه K_HOR_SCROLLBAR   TERMCAP2_کلی(KS_HOR_SCROLLBAR, KE_FILLER)

#تعریف_کړه K_NETTERM_MOUSE	TERMCAP2_کلی(KS_NETTERM_MOUSE, KE_FILLER)
#تعریف_کړه K_DEC_MOUSE	TERMCAP2_کلی(KS_DEC_MOUSE, KE_FILLER)
#تعریف_کړه K_JSBTERM_MOUSE	TERMCAP2_کلی(KS_JSBTERM_MOUSE, KE_FILLER)
#تعریف_کړه K_PTERM_MOUSE	TERMCAP2_کلی(KS_PTERM_MOUSE, KE_FILLER)
#تعریف_کړه K_URXVT_MOUSE	TERMCAP2_کلی(KS_URXVT_MOUSE, KE_FILLER)
#تعریف_کړه K_SGR_MOUSE	TERMCAP2_کلی(KS_SGR_MOUSE, KE_FILLER)
#تعریف_کړه K_SGR_MOUSERELEASE TERMCAP2_کلی(KS_SGR_MOUSE_RELEASE, KE_FILLER)

#تعریف_کړه K_SELECT	TERMCAP2_کلی(KS_SELECT, KE_FILLER)
#تعریف_کړه K_TEAROFF	TERMCAP2_کلی(KS_TEAROFF, KE_FILLER)

#تعریف_کړه K_TABLINE	TERMCAP2_کلی(KS_TABLINE, KE_FILLER)
#تعریف_کړه K_TABMENU	TERMCAP2_کلی(KS_TABMENU, KE_FILLER)

/*
 * Symbols for pseudo keys which are translated from the real key symbols
 * above.
 */
#تعریف_کړه K_LEFTMOUSE	TERMCAP2_کلی(KS_EXTRA, KE_LEFTMOUSE)
#تعریف_کړه K_LEFTMOUSE_NM	TERMCAP2_کلی(KS_EXTRA, KE_LEFTMOUSE_NM)
#تعریف_کړه K_LEFTDRAG	TERMCAP2_کلی(KS_EXTRA, KE_LEFTDRAG)
#تعریف_کړه K_LEFTRELEASE	TERMCAP2_کلی(KS_EXTRA, KE_LEFTRELEASE)
#تعریف_کړه K_LEFTRELEASE_NM TERMCAP2_کلی(KS_EXTRA, KE_LEFTRELEASE_NM)
#تعریف_کړه K_MOUSEMOVE	TERMCAP2_کلی(KS_EXTRA, KE_MOUSEMOVE)
#تعریف_کړه K_MIDDLEMOUSE	TERMCAP2_کلی(KS_EXTRA, KE_MIDDLEMOUSE)
#تعریف_کړه K_MIDDLEDRAG	TERMCAP2_کلی(KS_EXTRA, KE_MIDDLEDRAG)
#تعریف_کړه K_MIDDLERELEASE	TERMCAP2_کلی(KS_EXTRA, KE_MIDDLERELEASE)
#تعریف_کړه K_RIGHTMOUSE	TERMCAP2_کلی(KS_EXTRA, KE_RIGHTMOUSE)
#تعریف_کړه K_RIGHTDRAG	TERMCAP2_کلی(KS_EXTRA, KE_RIGHTDRAG)
#تعریف_کړه K_RIGHTRELEASE	TERMCAP2_کلی(KS_EXTRA, KE_RIGHTRELEASE)
#تعریف_کړه K_X1MOUSE	TERMCAP2_کلی(KS_EXTRA, KE_X1MOUSE)
#تعریف_کړه K_X1MOUSE	TERMCAP2_کلی(KS_EXTRA, KE_X1MOUSE)
#تعریف_کړه K_X1DRAG	TERMCAP2_کلی(KS_EXTRA, KE_X1DRAG)
#تعریف_کړه K_X1RELEASE     TERMCAP2_کلی(KS_EXTRA, KE_X1RELEASE)
#تعریف_کړه K_X2MOUSE	TERMCAP2_کلی(KS_EXTRA, KE_X2MOUSE)
#تعریف_کړه K_X2DRAG	TERMCAP2_کلی(KS_EXTRA, KE_X2DRAG)
#تعریف_کړه K_X2RELEASE     TERMCAP2_کلی(KS_EXTRA, KE_X2RELEASE)

#تعریف_کړه K_IGNORE	TERMCAP2_کلی(KS_EXTRA, KE_IGNORE)
#تعریف_کړه K_NOP		TERMCAP2_کلی(KS_EXTRA, KE_NOP)
#تعریف_کړه K_CANCEL	TERMCAP2_کلی(KS_EXTRA, KE_CANCEL)

#تعریف_کړه K_MOUSEDOWN	TERMCAP2_کلی(KS_EXTRA, KE_MOUSEDOWN)
#تعریف_کړه K_MOUSEUP	TERMCAP2_کلی(KS_EXTRA, KE_MOUSEUP)
#تعریف_کړه K_MOUSELEFT	TERMCAP2_کلی(KS_EXTRA, KE_MOUSELEFT)
#تعریف_کړه K_MOUSERIGHT	TERMCAP2_کلی(KS_EXTRA, KE_MOUSERIGHT)

#تعریف_کړه K_CSI		TERMCAP2_کلی(KS_EXTRA, KE_CSI)
#تعریف_کړه K_OSC		TERMCAP2_کلی(KS_EXTRA, KE_OSC)
#تعریف_کړه K_SNR		TERMCAP2_کلی(KS_EXTRA, KE_SNR)
#تعریف_کړه K_PLUG		TERMCAP2_کلی(KS_EXTRA, KE_PLUG)
#تعریف_کړه K_CMDWIN	TERMCAP2_کلی(KS_EXTRA, KE_CMDWIN)

#تعریف_کړه K_DROP		TERMCAP2_کلی(KS_EXTRA, KE_DROP)
#تعریف_کړه K_FOCUSGAINED	TERMCAP2_کلی(KS_EXTRA, KE_FOCUSGAINED)
#تعریف_کړه K_FOCUSLOST	TERMCAP2_کلی(KS_EXTRA, KE_FOCUSLOST)

#تعریف_کړه K_CURSORHOLD	TERMCAP2_کلی(KS_EXTRA, KE_CURSORHOLD)
#تعریف_کړه K_COMPLETE_DELAY TERMCAP2_کلی(KS_EXTRA, KE_COMPLETE_DELAY)

#تعریف_کړه K_COMMAND	TERMCAP2_کلی(KS_EXTRA, KE_COMMAND)
#تعریف_کړه K_SCRIPT_COMMAND TERMCAP2_کلی(KS_EXTRA, KE_SCRIPT_COMMAND)
#تعریف_کړه K_SID		TERMCAP2_کلی(KS_EXTRA, KE_SID)

#تعریف_کړه K_WILD		TERMCAP2_کلی(KS_EXTRA, KE_WILD)

// Bits for modifier mask
// 0x01 cannot be used, because the modifier must be 0x02 or higher
#تعریف_کړه MOD_ماسک_ښويدنه	    0x02
#تعریف_کړه MOD_ماسک_CTRL	    0x04
#تعریف_کړه MOD_ماسک_ALT	    0x08	// aka META
#تعریف_کړه MOD_ماسک_META	    0x10	// META when it's different from ALT
#تعریف_کړه MOD_ماسک_2_CLICK	    0x20	// use MOD_MASK_MULTI_CLICK
#تعریف_کړه MOD_ماسک_3_CLICK	    0x40	// use MOD_MASK_MULTI_CLICK
#تعریف_کړه MOD_ماسک_4_CLICK	    0x60	// use MOD_MASK_MULTI_CLICK
#إذا defined(MACOS_X) || defined(FEAT_GUI_GTK)
# define MOD_ماسک_CMD	    0x80        // aka SUPER
#د_که_پای

#تعریف_کړه MOD_ماسک_MULTI_CLICK	(MOD_ماسک_2_CLICK|MOD_ماسک_3_CLICK|MOD_ماسک_4_CLICK)

/*
 * The length of the longest special key name, including modifiers.
 * Current longest is <M-C-S-T-D-A-4-ScrollWheelRight> (length includes '<' and
 * '>').
 */
#تعریف_کړه MAX_کلی_NAME_LEN    32

/*
 * Maximum length of a special key event as tokens.  This includes modifiers.
 * The longest event is something like <M-C-S-T-4-LeftDrag> which would be the
 * following string of tokens:
 *
 * <K_SPECIAL> <KS_MODIFIER> bitmask <K_SPECIAL> <KS_EXTRA> <KT_LEFTDRAG>.
 *
 * This is a total of 6 tokens, and is currently the longest one possible.
 */
#تعریف_کړه MAX_کلی_کوډ_LEN    6
