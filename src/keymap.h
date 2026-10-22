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
#تعریف_کرو K_NUL			(0xce)	// for MSDOS: special key follows

/*
 * K_SPECIAL is the first byte of a special key code and is always followed by
 * two bytes.
 * The second byte can have any value. ASCII is used for normal termcap
 * entries, 0x80 and higher for special keys, see below.
 * The third byte is guaranteed to be between 0x02 and 0x7f.
 */

#تعریف_کرو K_SPECIAL		(0x80)

/*
 * Positive characters are "normal" characters.
 * Negative characters are special key codes.  Only characters below -0x200
 * are used to so that the absolute value can't be mistaken for a single-byte
 * character.
 */
#تعریف_کرو IS_SPECIAL(c)		((c) < 0)

/*
 * Characters 0x0100 - 0x01ff have a special meaning for abbreviations.
 * Multi-byte characters also have ABBR_OFF added, thus are above 0x0200.
 */
#تعریف_کرو ABBR_OFF		0x100

/*
 * NUL cannot be in the input string, therefore it is replaced by
 *	K_SPECIAL   KS_ZERO	KE_FILLER
 */
#تعریف_کرو KS_ZERO			255

/*
 * K_SPECIAL cannot be in the input string, therefore it is replaced by
 *	K_SPECIAL   KS_SPECIAL	KE_FILLER
 */
#تعریف_کرو KS_SPECIAL		254

/*
 * KS_EXTRA is used for keys that have no termcap name
 *	K_SPECIAL   KS_EXTRA	KE_xxx
 */
#تعریف_کرو KS_EXTRA		253

/*
 * KS_MODIFIER is used when a modifier is given for a (special) key
 *	K_SPECIAL   KS_MODIFIER	bitmask
 */
#تعریف_کرو KS_MODIFIER		252

/*
 * These are used for the GUI
 *	K_SPECIAL   KS_xxx	KE_FILLER
 */
#تعریف_کرو KS_MOUSE		251
#تعریف_کرو KS_MENU			250
#تعریف_کرو KS_VER_SCROLLBAR	249
#تعریف_کرو KS_HOR_SCROLLBAR	248

/*
 * These are used for DEC mouse
 */
#تعریف_کرو KS_NETTERM_MOUSE	247
#تعریف_کرو KS_DEC_MOUSE		246

/*
 * Used for switching Select mode back on after a mapping or menu.
 */
#تعریف_کرو KS_SELECT		245
#تعریف_کرو K_SELECT_STRING		(char_u *)"\200\365X"

/*
 * Used for tearing off a menu.
 */
#تعریف_کرو KS_TEAROFF		244

// Used for JSB term mouse.
#تعریف_کرو KS_JSBTERM_MOUSE	243

// Used a termcap entry that produces a normal character.
#تعریف_کرو KS_کلید			242

// Used for the qnx pterm mouse.
#تعریف_کرو KS_PTERM_MOUSE		241

// Used for click in a tab pages label.
#تعریف_کرو KS_TABLINE		240

// Used for menu in a tab pages line.
#تعریف_کرو KS_TABMENU		239

// Used for the urxvt mouse.
#تعریف_کرو KS_URXVT_MOUSE		238

// Used for the sgr mouse.
#تعریف_کرو KS_SGR_MOUSE		237
#تعریف_کرو KS_SGR_MOUSE_RELEASE	236

// Used for the GPM mouse.
#تعریف_کرو KS_GPM_MOUSE		235

/*
 * Filler used after KS_SPECIAL and others
 */
#تعریف_کرو KE_FILLER		('X')

/*
 * translation of three byte code "K_SPECIAL a b" into int "K_xxx" and back
 */
#تعریف_کرو TERMCAP2_کلید(a, b)	(-((a) + ((صحیح)(b) << 8)))
#تعریف_کرو KEY2TERMCAP0(x)		((-(x)) & 0xff)
#تعریف_کرو KEY2TERMCAP1(x)		(((غیرعلامتی)(-(x)) >> 8) & 0xff)

/*
 * get second or third byte when translating special key code into three bytes
 */
#تعریف_کرو K_دوسرا(c)	((c) == K_SPECIAL ? KS_SPECIAL : (c) == NUL ? KS_ZERO : KEY2TERMCAP0(c))

#تعریف_کرو K_تیسرا(c)	(((c) == K_SPECIAL || (c) == NUL) ? KE_FILLER : KEY2TERMCAP1(c))

/*
 * get single int code from second byte after K_SPECIAL
 */
#تعریف_کرو مقصد_SPECIAL(a, b)    ((a) == KS_SPECIAL ? K_SPECIAL : (a) == KS_ZERO ? K_ZERO : TERMCAP2_کلید(a, b))

/*
 * Codes for keys that do not have a termcap name.
 * The numbers are fixed to make sure that recorded key sequences remain valid.
 * Add new entries at the end, not halfway.
 *
 * K_SPECIAL KS_EXTRA KE_xxx
 */
شمار کلید_extra
{
      KE_S_اوپر = 4		// shift-up
    , KE_S_نیچے_کو = 5		// shift-down

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

    , KE_SNIFF_غیر_مستعمل = 56	// obsolete
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

    , KE_C_بایاں = 85		// control-left
    , KE_C_دایاں = 86		// control-right
    , KE_C_HOME = 87		// control-home
    , KE_C_انتہا = 88		// control-end

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
#تعریف_کرو K_ZERO		TERMCAP2_کلید(KS_ZERO, KE_FILLER)

#تعریف_کرو K_ESC		TERMCAP2_کلید(KS_EXTRA, KE_ESC)

#تعریف_کرو K_اوپر		TERMCAP2_کلید('k', 'u')
#تعریف_کرو K_نیچے_کو		TERMCAP2_کلید('k', 'd')
#تعریف_کرو K_بایاں		TERMCAP2_کلید('k', 'l')
#تعریف_کرو K_دایاں		TERMCAP2_کلید('k', 'r')
#تعریف_کرو K_S_اوپر		TERMCAP2_کلید(KS_EXTRA, KE_S_اوپر)
#تعریف_کرو K_S_نیچے_کو	TERMCAP2_کلید(KS_EXTRA, KE_S_نیچے_کو)
#تعریف_کرو K_S_بایاں	TERMCAP2_کلید('#', '4')
#تعریف_کرو K_C_بایاں	TERMCAP2_کلید(KS_EXTRA, KE_C_بایاں)
#تعریف_کرو K_S_دایاں	TERMCAP2_کلید('%', 'i')
#تعریف_کرو K_C_دایاں	TERMCAP2_کلید(KS_EXTRA, KE_C_دایاں)

#تعریف_کرو K_S_HOME	TERMCAP2_کلید('#', '2')
#تعریف_کرو K_C_HOME	TERMCAP2_کلید(KS_EXTRA, KE_C_HOME)
#تعریف_کرو K_S_انتہا		TERMCAP2_کلید('*', '7')
#تعریف_کرو K_C_انتہا		TERMCAP2_کلید(KS_EXTRA, KE_C_انتہا)

#تعریف_کرو K_TAB		TERMCAP2_کلید(KS_EXTRA, KE_TAB)
#تعریف_کرو K_S_TAB		TERMCAP2_کلید('k', 'B')
#تعریف_کرو K_S_BS		TERMCAP2_کلید(KS_EXTRA, KE_S_BS)

// extra set of function keys F1-F4, for vt100 compatible xterm
#تعریف_کرو K_XF1		TERMCAP2_کلید(KS_EXTRA, KE_XF1)
#تعریف_کرو K_XF2		TERMCAP2_کلید(KS_EXTRA, KE_XF2)
#تعریف_کرو K_XF3		TERMCAP2_کلید(KS_EXTRA, KE_XF3)
#تعریف_کرو K_XF4		TERMCAP2_کلید(KS_EXTRA, KE_XF4)

// extra set of cursor keys for vt100 compatible xterm
#تعریف_کرو K_XUP		TERMCAP2_کلید(KS_EXTRA, KE_XUP)
#تعریف_کرو K_XDOWN		TERMCAP2_کلید(KS_EXTRA, KE_XDOWN)
#تعریف_کرو K_XLEFT		TERMCAP2_کلید(KS_EXTRA, KE_XLEFT)
#تعریف_کرو K_XRIGHT	TERMCAP2_کلید(KS_EXTRA, KE_XRIGHT)

#تعریف_کرو K_F1		TERMCAP2_کلید('k', '1')	// function keys
#تعریف_کرو K_F2		TERMCAP2_کلید('k', '2')
#تعریف_کرو K_F3		TERMCAP2_کلید('k', '3')
#تعریف_کرو K_F4		TERMCAP2_کلید('k', '4')
#تعریف_کرو K_F5		TERMCAP2_کلید('k', '5')
#تعریف_کرو K_F6		TERMCAP2_کلید('k', '6')
#تعریف_کرو K_F7		TERMCAP2_کلید('k', '7')
#تعریف_کرو K_F8		TERMCAP2_کلید('k', '8')
#تعریف_کرو K_F9		TERMCAP2_کلید('k', '9')
#تعریف_کرو K_F10		TERMCAP2_کلید('k', ';')

#تعریف_کرو K_F11		TERMCAP2_کلید('F', '1')
#تعریف_کرو K_F12		TERMCAP2_کلید('F', '2')
#تعریف_کرو K_F13		TERMCAP2_کلید('F', '3')
#تعریف_کرو K_F14		TERMCAP2_کلید('F', '4')
#تعریف_کرو K_F15		TERMCAP2_کلید('F', '5')
#تعریف_کرو K_F16		TERMCAP2_کلید('F', '6')
#تعریف_کرو K_F17		TERMCAP2_کلید('F', '7')
#تعریف_کرو K_F18		TERMCAP2_کلید('F', '8')
#تعریف_کرو K_F19		TERMCAP2_کلید('F', '9')
#تعریف_کرو K_F20		TERMCAP2_کلید('F', 'A')

#تعریف_کرو K_F21		TERMCAP2_کلید('F', 'B')
#تعریف_کرو K_F22		TERMCAP2_کلید('F', 'C')
#تعریف_کرو K_F23		TERMCAP2_کلید('F', 'D')
#تعریف_کرو K_F24		TERMCAP2_کلید('F', 'E')
#تعریف_کرو K_F25		TERMCAP2_کلید('F', 'F')
#تعریف_کرو K_F26		TERMCAP2_کلید('F', 'G')
#تعریف_کرو K_F27		TERMCAP2_کلید('F', 'H')
#تعریف_کرو K_F28		TERMCAP2_کلید('F', 'I')
#تعریف_کرو K_F29		TERMCAP2_کلید('F', 'J')
#تعریف_کرو K_F30		TERMCAP2_کلید('F', 'K')

#تعریف_کرو K_F31		TERMCAP2_کلید('F', 'L')
#تعریف_کرو K_F32		TERMCAP2_کلید('F', 'M')
#تعریف_کرو K_F33		TERMCAP2_کلید('F', 'N')
#تعریف_کرو K_F34		TERMCAP2_کلید('F', 'O')
#تعریف_کرو K_F35		TERMCAP2_کلید('F', 'P')
#تعریف_کرو K_F36		TERMCAP2_کلید('F', 'Q')
#تعریف_کرو K_F37		TERMCAP2_کلید('F', 'R')

// extra set of shifted function keys F1-F4, for vt100 compatible xterm
#تعریف_کرو K_S_XF1		TERMCAP2_کلید(KS_EXTRA, KE_S_XF1)
#تعریف_کرو K_S_XF2		TERMCAP2_کلید(KS_EXTRA, KE_S_XF2)
#تعریف_کرو K_S_XF3		TERMCAP2_کلید(KS_EXTRA, KE_S_XF3)
#تعریف_کرو K_S_XF4		TERMCAP2_کلید(KS_EXTRA, KE_S_XF4)

#تعریف_کرو K_S_F1		TERMCAP2_کلید(KS_EXTRA, KE_S_F1)	// shifted func. keys
#تعریف_کرو K_S_F2		TERMCAP2_کلید(KS_EXTRA, KE_S_F2)
#تعریف_کرو K_S_F3		TERMCAP2_کلید(KS_EXTRA, KE_S_F3)
#تعریف_کرو K_S_F4		TERMCAP2_کلید(KS_EXTRA, KE_S_F4)
#تعریف_کرو K_S_F5		TERMCAP2_کلید(KS_EXTRA, KE_S_F5)
#تعریف_کرو K_S_F6		TERMCAP2_کلید(KS_EXTRA, KE_S_F6)
#تعریف_کرو K_S_F7		TERMCAP2_کلید(KS_EXTRA, KE_S_F7)
#تعریف_کرو K_S_F8		TERMCAP2_کلید(KS_EXTRA, KE_S_F8)
#تعریف_کرو K_S_F9		TERMCAP2_کلید(KS_EXTRA, KE_S_F9)
#تعریف_کرو K_S_F10		TERMCAP2_کلید(KS_EXTRA, KE_S_F10)

#تعریف_کرو K_S_F11		TERMCAP2_کلید(KS_EXTRA, KE_S_F11)
#تعریف_کرو K_S_F12		TERMCAP2_کلید(KS_EXTRA, KE_S_F12)
// K_S_F13 to K_S_F37  are currently not used

#تعریف_کرو K_HELP		TERMCAP2_کلید('%', '1')
#تعریف_کرو K_UNDO		TERMCAP2_کلید('&', '8')

#تعریف_کرو K_BS		TERMCAP2_کلید('k', 'b')

#تعریف_کرو K_INS		TERMCAP2_کلید('k', 'I')
#تعریف_کرو K_KINS		TERMCAP2_کلید(KS_EXTRA, KE_KINS)
#تعریف_کرو K_DEL		TERMCAP2_کلید('k', 'D')
#تعریف_کرو K_KDEL		TERMCAP2_کلید(KS_EXTRA, KE_KDEL)
#تعریف_کرو K_HOME		TERMCAP2_کلید('k', 'h')
#تعریف_کرو K_KHOME		TERMCAP2_کلید('K', '1')	// keypad home (upper left)
#تعریف_کرو K_XHOME		TERMCAP2_کلید(KS_EXTRA, KE_XHOME)
#تعریف_کرو K_ZHOME		TERMCAP2_کلید(KS_EXTRA, KE_ZHOME)
#تعریف_کرو K_انتہا		TERMCAP2_کلید('@', '7')
#تعریف_کرو K_KEND		TERMCAP2_کلید('K', '4')	// keypad end (lower left)
#تعریف_کرو K_XEND		TERMCAP2_کلید(KS_EXTRA, KE_XEND)
#تعریف_کرو K_ZEND		TERMCAP2_کلید(KS_EXTRA, KE_ZEND)
#تعریف_کرو K_PAGEUP	TERMCAP2_کلید('k', 'P')
#تعریف_کرو K_PAGEDOWN	TERMCAP2_کلید('k', 'N')
#تعریف_کرو K_KPAGEUP	TERMCAP2_کلید('K', '3')	// keypad pageup (upper R.)
#تعریف_کرو K_KPAGEDOWN	TERMCAP2_کلید('K', '5')	// keypad pagedown (lower R.)

#تعریف_کرو K_KPLUS		TERMCAP2_کلید('K', '6')	// keypad plus
#تعریف_کرو K_KMINUS	TERMCAP2_کلید('K', '7')	// keypad minus
#تعریف_کرو K_KDIVIDE	TERMCAP2_کلید('K', '8')	// keypad /
#تعریف_کرو K_KMULTIPLY	TERMCAP2_کلید('K', '9')	// keypad *
#تعریف_کرو K_KENTER	TERMCAP2_کلید('K', 'A')	// keypad Enter
#تعریف_کرو K_KPOINT	TERMCAP2_کلید('K', 'B')	// keypad . or ,
#تعریف_کرو K_PS		TERMCAP2_کلید('P', 'S')	// paste start
#تعریف_کرو K_PE		TERMCAP2_کلید('P', 'E')	// paste end

#تعریف_کرو K_K0		TERMCAP2_کلید('K', 'C')	// keypad 0
#تعریف_کرو K_K1		TERMCAP2_کلید('K', 'D')	// keypad 1
#تعریف_کرو K_K2		TERMCAP2_کلید('K', 'E')	// keypad 2
#تعریف_کرو K_K3		TERMCAP2_کلید('K', 'F')	// keypad 3
#تعریف_کرو K_K4		TERMCAP2_کلید('K', 'G')	// keypad 4
#تعریف_کرو K_K5		TERMCAP2_کلید('K', 'H')	// keypad 5
#تعریف_کرو K_K6		TERMCAP2_کلید('K', 'I')	// keypad 6
#تعریف_کرو K_K7		TERMCAP2_کلید('K', 'J')	// keypad 7
#تعریف_کرو K_K8		TERMCAP2_کلید('K', 'K')	// keypad 8
#تعریف_کرو K_K9		TERMCAP2_کلید('K', 'L')	// keypad 9

#تعریف_کرو K_MOUSE		TERMCAP2_کلید(KS_MOUSE, KE_FILLER)
#تعریف_کرو K_MENU		TERMCAP2_کلید(KS_MENU, KE_FILLER)
#تعریف_کرو K_VER_SCROLLBAR	TERMCAP2_کلید(KS_VER_SCROLLBAR, KE_FILLER)
#تعریف_کرو K_HOR_SCROLLBAR   TERMCAP2_کلید(KS_HOR_SCROLLBAR, KE_FILLER)

#تعریف_کرو K_NETTERM_MOUSE	TERMCAP2_کلید(KS_NETTERM_MOUSE, KE_FILLER)
#تعریف_کرو K_DEC_MOUSE	TERMCAP2_کلید(KS_DEC_MOUSE, KE_FILLER)
#تعریف_کرو K_JSBTERM_MOUSE	TERMCAP2_کلید(KS_JSBTERM_MOUSE, KE_FILLER)
#تعریف_کرو K_PTERM_MOUSE	TERMCAP2_کلید(KS_PTERM_MOUSE, KE_FILLER)
#تعریف_کرو K_URXVT_MOUSE	TERMCAP2_کلید(KS_URXVT_MOUSE, KE_FILLER)
#تعریف_کرو K_SGR_MOUSE	TERMCAP2_کلید(KS_SGR_MOUSE, KE_FILLER)
#تعریف_کرو K_SGR_MOUSERELEASE TERMCAP2_کلید(KS_SGR_MOUSE_RELEASE, KE_FILLER)

#تعریف_کرو K_SELECT	TERMCAP2_کلید(KS_SELECT, KE_FILLER)
#تعریف_کرو K_TEAROFF	TERMCAP2_کلید(KS_TEAROFF, KE_FILLER)

#تعریف_کرو K_TABLINE	TERMCAP2_کلید(KS_TABLINE, KE_FILLER)
#تعریف_کرو K_TABMENU	TERMCAP2_کلید(KS_TABMENU, KE_FILLER)

/*
 * Symbols for pseudo keys which are translated from the real key symbols
 * above.
 */
#تعریف_کرو K_LEFTMOUSE	TERMCAP2_کلید(KS_EXTRA, KE_LEFTMOUSE)
#تعریف_کرو K_LEFTMOUSE_NM	TERMCAP2_کلید(KS_EXTRA, KE_LEFTMOUSE_NM)
#تعریف_کرو K_LEFTDRAG	TERMCAP2_کلید(KS_EXTRA, KE_LEFTDRAG)
#تعریف_کرو K_LEFTRELEASE	TERMCAP2_کلید(KS_EXTRA, KE_LEFTRELEASE)
#تعریف_کرو K_LEFTRELEASE_NM TERMCAP2_کلید(KS_EXTRA, KE_LEFTRELEASE_NM)
#تعریف_کرو K_MOUSEMOVE	TERMCAP2_کلید(KS_EXTRA, KE_MOUSEMOVE)
#تعریف_کرو K_MIDDLEMOUSE	TERMCAP2_کلید(KS_EXTRA, KE_MIDDLEMOUSE)
#تعریف_کرو K_MIDDLEDRAG	TERMCAP2_کلید(KS_EXTRA, KE_MIDDLEDRAG)
#تعریف_کرو K_MIDDLERELEASE	TERMCAP2_کلید(KS_EXTRA, KE_MIDDLERELEASE)
#تعریف_کرو K_RIGHTMOUSE	TERMCAP2_کلید(KS_EXTRA, KE_RIGHTMOUSE)
#تعریف_کرو K_RIGHTDRAG	TERMCAP2_کلید(KS_EXTRA, KE_RIGHTDRAG)
#تعریف_کرو K_RIGHTRELEASE	TERMCAP2_کلید(KS_EXTRA, KE_RIGHTRELEASE)
#تعریف_کرو K_X1MOUSE	TERMCAP2_کلید(KS_EXTRA, KE_X1MOUSE)
#تعریف_کرو K_X1MOUSE	TERMCAP2_کلید(KS_EXTRA, KE_X1MOUSE)
#تعریف_کرو K_X1DRAG	TERMCAP2_کلید(KS_EXTRA, KE_X1DRAG)
#تعریف_کرو K_X1RELEASE     TERMCAP2_کلید(KS_EXTRA, KE_X1RELEASE)
#تعریف_کرو K_X2MOUSE	TERMCAP2_کلید(KS_EXTRA, KE_X2MOUSE)
#تعریف_کرو K_X2DRAG	TERMCAP2_کلید(KS_EXTRA, KE_X2DRAG)
#تعریف_کرو K_X2RELEASE     TERMCAP2_کلید(KS_EXTRA, KE_X2RELEASE)

#تعریف_کرو K_IGNORE	TERMCAP2_کلید(KS_EXTRA, KE_IGNORE)
#تعریف_کرو K_NOP		TERMCAP2_کلید(KS_EXTRA, KE_NOP)
#تعریف_کرو K_CANCEL	TERMCAP2_کلید(KS_EXTRA, KE_CANCEL)

#تعریف_کرو K_MOUSEDOWN	TERMCAP2_کلید(KS_EXTRA, KE_MOUSEDOWN)
#تعریف_کرو K_MOUSEUP	TERMCAP2_کلید(KS_EXTRA, KE_MOUSEUP)
#تعریف_کرو K_MOUSELEFT	TERMCAP2_کلید(KS_EXTRA, KE_MOUSELEFT)
#تعریف_کرو K_MOUSERIGHT	TERMCAP2_کلید(KS_EXTRA, KE_MOUSERIGHT)

#تعریف_کرو K_CSI		TERMCAP2_کلید(KS_EXTRA, KE_CSI)
#تعریف_کرو K_OSC		TERMCAP2_کلید(KS_EXTRA, KE_OSC)
#تعریف_کرو K_SNR		TERMCAP2_کلید(KS_EXTRA, KE_SNR)
#تعریف_کرو K_PLUG		TERMCAP2_کلید(KS_EXTRA, KE_PLUG)
#تعریف_کرو K_CMDWIN	TERMCAP2_کلید(KS_EXTRA, KE_CMDWIN)

#تعریف_کرو K_DROP		TERMCAP2_کلید(KS_EXTRA, KE_DROP)
#تعریف_کرو K_FOCUSGAINED	TERMCAP2_کلید(KS_EXTRA, KE_FOCUSGAINED)
#تعریف_کرو K_FOCUSLOST	TERMCAP2_کلید(KS_EXTRA, KE_FOCUSLOST)

#تعریف_کرو K_CURSORHOLD	TERMCAP2_کلید(KS_EXTRA, KE_CURSORHOLD)
#تعریف_کرو K_COMPLETE_DELAY TERMCAP2_کلید(KS_EXTRA, KE_COMPLETE_DELAY)

#تعریف_کرو K_COMMAND	TERMCAP2_کلید(KS_EXTRA, KE_COMMAND)
#تعریف_کرو K_SCRIPT_COMMAND TERMCAP2_کلید(KS_EXTRA, KE_SCRIPT_COMMAND)
#تعریف_کرو K_SID		TERMCAP2_کلید(KS_EXTRA, KE_SID)

#تعریف_کرو K_WILD		TERMCAP2_کلید(KS_EXTRA, KE_WILD)

// Bits for modifier mask
// 0x01 cannot be used, because the modifier must be 0x02 or higher
#تعریف_کرو MOD_نقاب_سرکاؤ	    0x02
#تعریف_کرو MOD_نقاب_CTRL	    0x04
#تعریف_کرو MOD_نقاب_ALT	    0x08	// aka META
#تعریف_کرو MOD_نقاب_META	    0x10	// META when it's different from ALT
#تعریف_کرو MOD_نقاب_2_CLICK	    0x20	// use MOD_MASK_MULTI_CLICK
#تعریف_کرو MOD_نقاب_3_CLICK	    0x40	// use MOD_MASK_MULTI_CLICK
#تعریف_کرو MOD_نقاب_4_CLICK	    0x60	// use MOD_MASK_MULTI_CLICK
#اگر defined(MACOS_X) || defined(FEAT_GUI_GTK)
# define MOD_نقاب_CMD	    0x80        // aka SUPER
#اگر_ختم

#تعریف_کرو MOD_نقاب_MULTI_CLICK	(MOD_نقاب_2_CLICK|MOD_نقاب_3_CLICK|MOD_نقاب_4_CLICK)

/*
 * The length of the longest special key name, including modifiers.
 * Current longest is <M-C-S-T-D-A-4-ScrollWheelRight> (length includes '<' and
 * '>').
 */
#تعریف_کرو MAX_کلید_NAME_LEN    32

/*
 * Maximum length of a special key event as tokens.  This includes modifiers.
 * The longest event is something like <M-C-S-T-4-LeftDrag> which would be the
 * following string of tokens:
 *
 * <K_SPECIAL> <KS_MODIFIER> bitmask <K_SPECIAL> <KS_EXTRA> <KT_LEFTDRAG>.
 *
 * This is a total of 6 tokens, and is currently the longest one possible.
 */
#تعریف_کرو MAX_کلید_رمز_LEN    6
