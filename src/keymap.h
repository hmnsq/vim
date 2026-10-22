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
#నిర్వచించు K_NUL			(0xce)	// for MSDOS: special key follows

/*
 * K_SPECIAL is the first byte of a special key code and is always followed by
 * two bytes.
 * The second byte can have any value. ASCII is used for normal termcap
 * entries, 0x80 and higher for special keys, see below.
 * The third byte is guaranteed to be between 0x02 and 0x7f.
 */

#నిర్వచించు K_SPECIAL		(0x80)

/*
 * Positive characters are "normal" characters.
 * Negative characters are special key codes.  Only characters below -0x200
 * are used to so that the absolute value can't be mistaken for a single-byte
 * character.
 */
#నిర్వచించు IS_SPECIAL(c)		((c) < 0)

/*
 * Characters 0x0100 - 0x01ff have a special meaning for abbreviations.
 * Multi-byte characters also have ABBR_OFF added, thus are above 0x0200.
 */
#నిర్వచించు ABBR_OFF		0x100

/*
 * NUL cannot be in the input string, therefore it is replaced by
 *	K_SPECIAL   KS_ZERO	KE_FILLER
 */
#నిర్వచించు KS_ZERO			255

/*
 * K_SPECIAL cannot be in the input string, therefore it is replaced by
 *	K_SPECIAL   KS_SPECIAL	KE_FILLER
 */
#నిర్వచించు KS_SPECIAL		254

/*
 * KS_EXTRA is used for keys that have no termcap name
 *	K_SPECIAL   KS_EXTRA	KE_xxx
 */
#నిర్వచించు KS_EXTRA		253

/*
 * KS_MODIFIER is used when a modifier is given for a (special) key
 *	K_SPECIAL   KS_MODIFIER	bitmask
 */
#నిర్వచించు KS_MODIFIER		252

/*
 * These are used for the GUI
 *	K_SPECIAL   KS_xxx	KE_FILLER
 */
#నిర్వచించు KS_MOUSE		251
#నిర్వచించు KS_MENU			250
#నిర్వచించు KS_VER_SCROLLBAR	249
#నిర్వచించు KS_HOR_SCROLLBAR	248

/*
 * These are used for DEC mouse
 */
#నిర్వచించు KS_NETTERM_MOUSE	247
#నిర్వచించు KS_DEC_MOUSE		246

/*
 * Used for switching Select mode back on after a mapping or menu.
 */
#నిర్వచించు KS_SELECT		245
#నిర్వచించు K_SELECT_STRING		(char_u *)"\200\365X"

/*
 * Used for tearing off a menu.
 */
#నిర్వచించు KS_TEAROFF		244

// Used for JSB term mouse.
#నిర్వచించు KS_JSBTERM_MOUSE	243

// Used a termcap entry that produces a normal character.
#నిర్వచించు KS_కీలకం			242

// Used for the qnx pterm mouse.
#నిర్వచించు KS_PTERM_MOUSE		241

// Used for click in a tab pages label.
#నిర్వచించు KS_TABLINE		240

// Used for menu in a tab pages line.
#నిర్వచించు KS_TABMENU		239

// Used for the urxvt mouse.
#నిర్వచించు KS_URXVT_MOUSE		238

// Used for the sgr mouse.
#నిర్వచించు KS_SGR_MOUSE		237
#నిర్వచించు KS_SGR_MOUSE_RELEASE	236

// Used for the GPM mouse.
#నిర్వచించు KS_GPM_MOUSE		235

/*
 * Filler used after KS_SPECIAL and others
 */
#నిర్వచించు KE_FILLER		('X')

/*
 * translation of three byte code "K_SPECIAL a b" into int "K_xxx" and back
 */
#నిర్వచించు TERMCAP2_కీలకం(a, b)	(-((a) + ((పూర్ణాంకం)(b) << 8)))
#నిర్వచించు KEY2TERMCAP0(x)		((-(x)) & 0xff)
#నిర్వచించు KEY2TERMCAP1(x)		(((గుర్తులేని)(-(x)) >> 8) & 0xff)

/*
 * get second or third byte when translating special key code into three bytes
 */
#నిర్వచించు K_రెండవ(c)	((c) == K_SPECIAL ? KS_SPECIAL : (c) == NUL ? KS_ZERO : KEY2TERMCAP0(c))

#నిర్వచించు K_మూడవ(c)	(((c) == K_SPECIAL || (c) == NUL) ? KE_FILLER : KEY2TERMCAP1(c))

/*
 * get single int code from second byte after K_SPECIAL
 */
#నిర్వచించు గమ్యం_SPECIAL(a, b)    ((a) == KS_SPECIAL ? K_SPECIAL : (a) == KS_ZERO ? K_ZERO : TERMCAP2_కీలకం(a, b))

/*
 * Codes for keys that do not have a termcap name.
 * The numbers are fixed to make sure that recorded key sequences remain valid.
 * Add new entries at the end, not halfway.
 *
 * K_SPECIAL KS_EXTRA KE_xxx
 */
గణన కీలకం_extra
{
      KE_S_పైకి = 4		// shift-up
    , KE_S_కిందికి = 5		// shift-down

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

    , KE_SNIFF_వాడని = 56	// obsolete
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

    , KE_C_ఎడమ = 85		// control-left
    , KE_C_కుడి = 86		// control-right
    , KE_C_HOME = 87		// control-home
    , KE_C_చివర = 88		// control-end

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
#నిర్వచించు K_ZERO		TERMCAP2_కీలకం(KS_ZERO, KE_FILLER)

#నిర్వచించు K_ESC		TERMCAP2_కీలకం(KS_EXTRA, KE_ESC)

#నిర్వచించు K_పైకి		TERMCAP2_కీలకం('k', 'u')
#నిర్వచించు K_కిందికి		TERMCAP2_కీలకం('k', 'd')
#నిర్వచించు K_ఎడమ		TERMCAP2_కీలకం('k', 'l')
#నిర్వచించు K_కుడి		TERMCAP2_కీలకం('k', 'r')
#నిర్వచించు K_S_పైకి		TERMCAP2_కీలకం(KS_EXTRA, KE_S_పైకి)
#నిర్వచించు K_S_కిందికి	TERMCAP2_కీలకం(KS_EXTRA, KE_S_కిందికి)
#నిర్వచించు K_S_ఎడమ	TERMCAP2_కీలకం('#', '4')
#నిర్వచించు K_C_ఎడమ	TERMCAP2_కీలకం(KS_EXTRA, KE_C_ఎడమ)
#నిర్వచించు K_S_కుడి	TERMCAP2_కీలకం('%', 'i')
#నిర్వచించు K_C_కుడి	TERMCAP2_కీలకం(KS_EXTRA, KE_C_కుడి)

#నిర్వచించు K_S_HOME	TERMCAP2_కీలకం('#', '2')
#నిర్వచించు K_C_HOME	TERMCAP2_కీలకం(KS_EXTRA, KE_C_HOME)
#నిర్వచించు K_S_చివర		TERMCAP2_కీలకం('*', '7')
#నిర్వచించు K_C_చివర		TERMCAP2_కీలకం(KS_EXTRA, KE_C_చివర)

#నిర్వచించు K_TAB		TERMCAP2_కీలకం(KS_EXTRA, KE_TAB)
#నిర్వచించు K_S_TAB		TERMCAP2_కీలకం('k', 'B')
#నిర్వచించు K_S_BS		TERMCAP2_కీలకం(KS_EXTRA, KE_S_BS)

// extra set of function keys F1-F4, for vt100 compatible xterm
#నిర్వచించు K_XF1		TERMCAP2_కీలకం(KS_EXTRA, KE_XF1)
#నిర్వచించు K_XF2		TERMCAP2_కీలకం(KS_EXTRA, KE_XF2)
#నిర్వచించు K_XF3		TERMCAP2_కీలకం(KS_EXTRA, KE_XF3)
#నిర్వచించు K_XF4		TERMCAP2_కీలకం(KS_EXTRA, KE_XF4)

// extra set of cursor keys for vt100 compatible xterm
#నిర్వచించు K_XUP		TERMCAP2_కీలకం(KS_EXTRA, KE_XUP)
#నిర్వచించు K_XDOWN		TERMCAP2_కీలకం(KS_EXTRA, KE_XDOWN)
#నిర్వచించు K_XLEFT		TERMCAP2_కీలకం(KS_EXTRA, KE_XLEFT)
#నిర్వచించు K_XRIGHT	TERMCAP2_కీలకం(KS_EXTRA, KE_XRIGHT)

#నిర్వచించు K_F1		TERMCAP2_కీలకం('k', '1')	// function keys
#నిర్వచించు K_F2		TERMCAP2_కీలకం('k', '2')
#నిర్వచించు K_F3		TERMCAP2_కీలకం('k', '3')
#నిర్వచించు K_F4		TERMCAP2_కీలకం('k', '4')
#నిర్వచించు K_F5		TERMCAP2_కీలకం('k', '5')
#నిర్వచించు K_F6		TERMCAP2_కీలకం('k', '6')
#నిర్వచించు K_F7		TERMCAP2_కీలకం('k', '7')
#నిర్వచించు K_F8		TERMCAP2_కీలకం('k', '8')
#నిర్వచించు K_F9		TERMCAP2_కీలకం('k', '9')
#నిర్వచించు K_F10		TERMCAP2_కీలకం('k', ';')

#నిర్వచించు K_F11		TERMCAP2_కీలకం('F', '1')
#నిర్వచించు K_F12		TERMCAP2_కీలకం('F', '2')
#నిర్వచించు K_F13		TERMCAP2_కీలకం('F', '3')
#నిర్వచించు K_F14		TERMCAP2_కీలకం('F', '4')
#నిర్వచించు K_F15		TERMCAP2_కీలకం('F', '5')
#నిర్వచించు K_F16		TERMCAP2_కీలకం('F', '6')
#నిర్వచించు K_F17		TERMCAP2_కీలకం('F', '7')
#నిర్వచించు K_F18		TERMCAP2_కీలకం('F', '8')
#నిర్వచించు K_F19		TERMCAP2_కీలకం('F', '9')
#నిర్వచించు K_F20		TERMCAP2_కీలకం('F', 'A')

#నిర్వచించు K_F21		TERMCAP2_కీలకం('F', 'B')
#నిర్వచించు K_F22		TERMCAP2_కీలకం('F', 'C')
#నిర్వచించు K_F23		TERMCAP2_కీలకం('F', 'D')
#నిర్వచించు K_F24		TERMCAP2_కీలకం('F', 'E')
#నిర్వచించు K_F25		TERMCAP2_కీలకం('F', 'F')
#నిర్వచించు K_F26		TERMCAP2_కీలకం('F', 'G')
#నిర్వచించు K_F27		TERMCAP2_కీలకం('F', 'H')
#నిర్వచించు K_F28		TERMCAP2_కీలకం('F', 'I')
#నిర్వచించు K_F29		TERMCAP2_కీలకం('F', 'J')
#నిర్వచించు K_F30		TERMCAP2_కీలకం('F', 'K')

#నిర్వచించు K_F31		TERMCAP2_కీలకం('F', 'L')
#నిర్వచించు K_F32		TERMCAP2_కీలకం('F', 'M')
#నిర్వచించు K_F33		TERMCAP2_కీలకం('F', 'N')
#నిర్వచించు K_F34		TERMCAP2_కీలకం('F', 'O')
#నిర్వచించు K_F35		TERMCAP2_కీలకం('F', 'P')
#నిర్వచించు K_F36		TERMCAP2_కీలకం('F', 'Q')
#నిర్వచించు K_F37		TERMCAP2_కీలకం('F', 'R')

// extra set of shifted function keys F1-F4, for vt100 compatible xterm
#నిర్వచించు K_S_XF1		TERMCAP2_కీలకం(KS_EXTRA, KE_S_XF1)
#నిర్వచించు K_S_XF2		TERMCAP2_కీలకం(KS_EXTRA, KE_S_XF2)
#నిర్వచించు K_S_XF3		TERMCAP2_కీలకం(KS_EXTRA, KE_S_XF3)
#నిర్వచించు K_S_XF4		TERMCAP2_కీలకం(KS_EXTRA, KE_S_XF4)

#నిర్వచించు K_S_F1		TERMCAP2_కీలకం(KS_EXTRA, KE_S_F1)	// shifted func. keys
#నిర్వచించు K_S_F2		TERMCAP2_కీలకం(KS_EXTRA, KE_S_F2)
#నిర్వచించు K_S_F3		TERMCAP2_కీలకం(KS_EXTRA, KE_S_F3)
#నిర్వచించు K_S_F4		TERMCAP2_కీలకం(KS_EXTRA, KE_S_F4)
#నిర్వచించు K_S_F5		TERMCAP2_కీలకం(KS_EXTRA, KE_S_F5)
#నిర్వచించు K_S_F6		TERMCAP2_కీలకం(KS_EXTRA, KE_S_F6)
#నిర్వచించు K_S_F7		TERMCAP2_కీలకం(KS_EXTRA, KE_S_F7)
#నిర్వచించు K_S_F8		TERMCAP2_కీలకం(KS_EXTRA, KE_S_F8)
#నిర్వచించు K_S_F9		TERMCAP2_కీలకం(KS_EXTRA, KE_S_F9)
#నిర్వచించు K_S_F10		TERMCAP2_కీలకం(KS_EXTRA, KE_S_F10)

#నిర్వచించు K_S_F11		TERMCAP2_కీలకం(KS_EXTRA, KE_S_F11)
#నిర్వచించు K_S_F12		TERMCAP2_కీలకం(KS_EXTRA, KE_S_F12)
// K_S_F13 to K_S_F37  are currently not used

#నిర్వచించు K_HELP		TERMCAP2_కీలకం('%', '1')
#నిర్వచించు K_UNDO		TERMCAP2_కీలకం('&', '8')

#నిర్వచించు K_BS		TERMCAP2_కీలకం('k', 'b')

#నిర్వచించు K_INS		TERMCAP2_కీలకం('k', 'I')
#నిర్వచించు K_KINS		TERMCAP2_కీలకం(KS_EXTRA, KE_KINS)
#నిర్వచించు K_DEL		TERMCAP2_కీలకం('k', 'D')
#నిర్వచించు K_KDEL		TERMCAP2_కీలకం(KS_EXTRA, KE_KDEL)
#నిర్వచించు K_HOME		TERMCAP2_కీలకం('k', 'h')
#నిర్వచించు K_KHOME		TERMCAP2_కీలకం('K', '1')	// keypad home (upper left)
#నిర్వచించు K_XHOME		TERMCAP2_కీలకం(KS_EXTRA, KE_XHOME)
#నిర్వచించు K_ZHOME		TERMCAP2_కీలకం(KS_EXTRA, KE_ZHOME)
#నిర్వచించు K_చివర		TERMCAP2_కీలకం('@', '7')
#నిర్వచించు K_KEND		TERMCAP2_కీలకం('K', '4')	// keypad end (lower left)
#నిర్వచించు K_XEND		TERMCAP2_కీలకం(KS_EXTRA, KE_XEND)
#నిర్వచించు K_ZEND		TERMCAP2_కీలకం(KS_EXTRA, KE_ZEND)
#నిర్వచించు K_PAGEUP	TERMCAP2_కీలకం('k', 'P')
#నిర్వచించు K_PAGEDOWN	TERMCAP2_కీలకం('k', 'N')
#నిర్వచించు K_KPAGEUP	TERMCAP2_కీలకం('K', '3')	// keypad pageup (upper R.)
#నిర్వచించు K_KPAGEDOWN	TERMCAP2_కీలకం('K', '5')	// keypad pagedown (lower R.)

#నిర్వచించు K_KPLUS		TERMCAP2_కీలకం('K', '6')	// keypad plus
#నిర్వచించు K_KMINUS	TERMCAP2_కీలకం('K', '7')	// keypad minus
#నిర్వచించు K_KDIVIDE	TERMCAP2_కీలకం('K', '8')	// keypad /
#నిర్వచించు K_KMULTIPLY	TERMCAP2_కీలకం('K', '9')	// keypad *
#నిర్వచించు K_KENTER	TERMCAP2_కీలకం('K', 'A')	// keypad Enter
#నిర్వచించు K_KPOINT	TERMCAP2_కీలకం('K', 'B')	// keypad . or ,
#నిర్వచించు K_PS		TERMCAP2_కీలకం('P', 'S')	// paste start
#నిర్వచించు K_PE		TERMCAP2_కీలకం('P', 'E')	// paste end

#నిర్వచించు K_K0		TERMCAP2_కీలకం('K', 'C')	// keypad 0
#నిర్వచించు K_K1		TERMCAP2_కీలకం('K', 'D')	// keypad 1
#నిర్వచించు K_K2		TERMCAP2_కీలకం('K', 'E')	// keypad 2
#నిర్వచించు K_K3		TERMCAP2_కీలకం('K', 'F')	// keypad 3
#నిర్వచించు K_K4		TERMCAP2_కీలకం('K', 'G')	// keypad 4
#నిర్వచించు K_K5		TERMCAP2_కీలకం('K', 'H')	// keypad 5
#నిర్వచించు K_K6		TERMCAP2_కీలకం('K', 'I')	// keypad 6
#నిర్వచించు K_K7		TERMCAP2_కీలకం('K', 'J')	// keypad 7
#నిర్వచించు K_K8		TERMCAP2_కీలకం('K', 'K')	// keypad 8
#నిర్వచించు K_K9		TERMCAP2_కీలకం('K', 'L')	// keypad 9

#నిర్వచించు K_MOUSE		TERMCAP2_కీలకం(KS_MOUSE, KE_FILLER)
#నిర్వచించు K_MENU		TERMCAP2_కీలకం(KS_MENU, KE_FILLER)
#నిర్వచించు K_VER_SCROLLBAR	TERMCAP2_కీలకం(KS_VER_SCROLLBAR, KE_FILLER)
#నిర్వచించు K_HOR_SCROLLBAR   TERMCAP2_కీలకం(KS_HOR_SCROLLBAR, KE_FILLER)

#నిర్వచించు K_NETTERM_MOUSE	TERMCAP2_కీలకం(KS_NETTERM_MOUSE, KE_FILLER)
#నిర్వచించు K_DEC_MOUSE	TERMCAP2_కీలకం(KS_DEC_MOUSE, KE_FILLER)
#నిర్వచించు K_JSBTERM_MOUSE	TERMCAP2_కీలకం(KS_JSBTERM_MOUSE, KE_FILLER)
#నిర్వచించు K_PTERM_MOUSE	TERMCAP2_కీలకం(KS_PTERM_MOUSE, KE_FILLER)
#నిర్వచించు K_URXVT_MOUSE	TERMCAP2_కీలకం(KS_URXVT_MOUSE, KE_FILLER)
#నిర్వచించు K_SGR_MOUSE	TERMCAP2_కీలకం(KS_SGR_MOUSE, KE_FILLER)
#నిర్వచించు K_SGR_MOUSERELEASE TERMCAP2_కీలకం(KS_SGR_MOUSE_RELEASE, KE_FILLER)

#నిర్వచించు K_SELECT	TERMCAP2_కీలకం(KS_SELECT, KE_FILLER)
#నిర్వచించు K_TEAROFF	TERMCAP2_కీలకం(KS_TEAROFF, KE_FILLER)

#నిర్వచించు K_TABLINE	TERMCAP2_కీలకం(KS_TABLINE, KE_FILLER)
#నిర్వచించు K_TABMENU	TERMCAP2_కీలకం(KS_TABMENU, KE_FILLER)

/*
 * Symbols for pseudo keys which are translated from the real key symbols
 * above.
 */
#నిర్వచించు K_LEFTMOUSE	TERMCAP2_కీలకం(KS_EXTRA, KE_LEFTMOUSE)
#నిర్వచించు K_LEFTMOUSE_NM	TERMCAP2_కీలకం(KS_EXTRA, KE_LEFTMOUSE_NM)
#నిర్వచించు K_LEFTDRAG	TERMCAP2_కీలకం(KS_EXTRA, KE_LEFTDRAG)
#నిర్వచించు K_LEFTRELEASE	TERMCAP2_కీలకం(KS_EXTRA, KE_LEFTRELEASE)
#నిర్వచించు K_LEFTRELEASE_NM TERMCAP2_కీలకం(KS_EXTRA, KE_LEFTRELEASE_NM)
#నిర్వచించు K_MOUSEMOVE	TERMCAP2_కీలకం(KS_EXTRA, KE_MOUSEMOVE)
#నిర్వచించు K_MIDDLEMOUSE	TERMCAP2_కీలకం(KS_EXTRA, KE_MIDDLEMOUSE)
#నిర్వచించు K_MIDDLEDRAG	TERMCAP2_కీలకం(KS_EXTRA, KE_MIDDLEDRAG)
#నిర్వచించు K_MIDDLERELEASE	TERMCAP2_కీలకం(KS_EXTRA, KE_MIDDLERELEASE)
#నిర్వచించు K_RIGHTMOUSE	TERMCAP2_కీలకం(KS_EXTRA, KE_RIGHTMOUSE)
#నిర్వచించు K_RIGHTDRAG	TERMCAP2_కీలకం(KS_EXTRA, KE_RIGHTDRAG)
#నిర్వచించు K_RIGHTRELEASE	TERMCAP2_కీలకం(KS_EXTRA, KE_RIGHTRELEASE)
#నిర్వచించు K_X1MOUSE	TERMCAP2_కీలకం(KS_EXTRA, KE_X1MOUSE)
#నిర్వచించు K_X1MOUSE	TERMCAP2_కీలకం(KS_EXTRA, KE_X1MOUSE)
#నిర్వచించు K_X1DRAG	TERMCAP2_కీలకం(KS_EXTRA, KE_X1DRAG)
#నిర్వచించు K_X1RELEASE     TERMCAP2_కీలకం(KS_EXTRA, KE_X1RELEASE)
#నిర్వచించు K_X2MOUSE	TERMCAP2_కీలకం(KS_EXTRA, KE_X2MOUSE)
#నిర్వచించు K_X2DRAG	TERMCAP2_కీలకం(KS_EXTRA, KE_X2DRAG)
#నిర్వచించు K_X2RELEASE     TERMCAP2_కీలకం(KS_EXTRA, KE_X2RELEASE)

#నిర్వచించు K_IGNORE	TERMCAP2_కీలకం(KS_EXTRA, KE_IGNORE)
#నిర్వచించు K_NOP		TERMCAP2_కీలకం(KS_EXTRA, KE_NOP)
#నిర్వచించు K_CANCEL	TERMCAP2_కీలకం(KS_EXTRA, KE_CANCEL)

#నిర్వచించు K_MOUSEDOWN	TERMCAP2_కీలకం(KS_EXTRA, KE_MOUSEDOWN)
#నిర్వచించు K_MOUSEUP	TERMCAP2_కీలకం(KS_EXTRA, KE_MOUSEUP)
#నిర్వచించు K_MOUSELEFT	TERMCAP2_కీలకం(KS_EXTRA, KE_MOUSELEFT)
#నిర్వచించు K_MOUSERIGHT	TERMCAP2_కీలకం(KS_EXTRA, KE_MOUSERIGHT)

#నిర్వచించు K_CSI		TERMCAP2_కీలకం(KS_EXTRA, KE_CSI)
#నిర్వచించు K_OSC		TERMCAP2_కీలకం(KS_EXTRA, KE_OSC)
#నిర్వచించు K_SNR		TERMCAP2_కీలకం(KS_EXTRA, KE_SNR)
#నిర్వచించు K_PLUG		TERMCAP2_కీలకం(KS_EXTRA, KE_PLUG)
#నిర్వచించు K_CMDWIN	TERMCAP2_కీలకం(KS_EXTRA, KE_CMDWIN)

#నిర్వచించు K_DROP		TERMCAP2_కీలకం(KS_EXTRA, KE_DROP)
#నిర్వచించు K_FOCUSGAINED	TERMCAP2_కీలకం(KS_EXTRA, KE_FOCUSGAINED)
#నిర్వచించు K_FOCUSLOST	TERMCAP2_కీలకం(KS_EXTRA, KE_FOCUSLOST)

#నిర్వచించు K_CURSORHOLD	TERMCAP2_కీలకం(KS_EXTRA, KE_CURSORHOLD)
#నిర్వచించు K_COMPLETE_DELAY TERMCAP2_కీలకం(KS_EXTRA, KE_COMPLETE_DELAY)

#నిర్వచించు K_COMMAND	TERMCAP2_కీలకం(KS_EXTRA, KE_COMMAND)
#నిర్వచించు K_SCRIPT_COMMAND TERMCAP2_కీలకం(KS_EXTRA, KE_SCRIPT_COMMAND)
#నిర్వచించు K_SID		TERMCAP2_కీలకం(KS_EXTRA, KE_SID)

#నిర్వచించు K_WILD		TERMCAP2_కీలకం(KS_EXTRA, KE_WILD)

// Bits for modifier mask
// 0x01 cannot be used, because the modifier must be 0x02 or higher
#నిర్వచించు MOD_ముసుగు_జరుపు	    0x02
#నిర్వచించు MOD_ముసుగు_CTRL	    0x04
#నిర్వచించు MOD_ముసుగు_ALT	    0x08	// aka META
#నిర్వచించు MOD_ముసుగు_META	    0x10	// META when it's different from ALT
#నిర్వచించు MOD_ముసుగు_2_CLICK	    0x20	// use MOD_MASK_MULTI_CLICK
#నిర్వచించు MOD_ముసుగు_3_CLICK	    0x40	// use MOD_MASK_MULTI_CLICK
#నిర్వచించు MOD_ముసుగు_4_CLICK	    0x60	// use MOD_MASK_MULTI_CLICK
#ఒకవేళ defined(MACOS_X) || defined(FEAT_GUI_GTK)
# define MOD_ముసుగు_CMD	    0x80        // aka SUPER
#అయితే_ముగింపు

#నిర్వచించు MOD_ముసుగు_MULTI_CLICK	(MOD_ముసుగు_2_CLICK|MOD_ముసుగు_3_CLICK|MOD_ముసుగు_4_CLICK)

/*
 * The length of the longest special key name, including modifiers.
 * Current longest is <M-C-S-T-D-A-4-ScrollWheelRight> (length includes '<' and
 * '>').
 */
#నిర్వచించు MAX_కీలకం_NAME_LEN    32

/*
 * Maximum length of a special key event as tokens.  This includes modifiers.
 * The longest event is something like <M-C-S-T-4-LeftDrag> which would be the
 * following string of tokens:
 *
 * <K_SPECIAL> <KS_MODIFIER> bitmask <K_SPECIAL> <KS_EXTRA> <KT_LEFTDRAG>.
 *
 * This is a total of 6 tokens, and is currently the longest one possible.
 */
#నిర్వచించు MAX_కీలకం_సంకేతం_LEN    6
