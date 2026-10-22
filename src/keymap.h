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
#وصف_ڪريو K_NUL			(0xce)	// for MSDOS: special key follows

/*
 * K_SPECIAL is the first byte of a special key code and is always followed by
 * two bytes.
 * The second byte can have any value. ASCII is used for normal termcap
 * entries, 0x80 and higher for special keys, see below.
 * The third byte is guaranteed to be between 0x02 and 0x7f.
 */

#وصف_ڪريو K_SPECIAL		(0x80)

/*
 * Positive characters are "normal" characters.
 * Negative characters are special key codes.  Only characters below -0x200
 * are used to so that the absolute value can't be mistaken for a single-byte
 * character.
 */
#وصف_ڪريو IS_SPECIAL(c)		((c) < 0)

/*
 * Characters 0x0100 - 0x01ff have a special meaning for abbreviations.
 * Multi-byte characters also have ABBR_OFF added, thus are above 0x0200.
 */
#وصف_ڪريو ABBR_OFF		0x100

/*
 * NUL cannot be in the input string, therefore it is replaced by
 *	K_SPECIAL   KS_ZERO	KE_FILLER
 */
#وصف_ڪريو KS_ZERO			255

/*
 * K_SPECIAL cannot be in the input string, therefore it is replaced by
 *	K_SPECIAL   KS_SPECIAL	KE_FILLER
 */
#وصف_ڪريو KS_SPECIAL		254

/*
 * KS_EXTRA is used for keys that have no termcap name
 *	K_SPECIAL   KS_EXTRA	KE_xxx
 */
#وصف_ڪريو KS_EXTRA		253

/*
 * KS_MODIFIER is used when a modifier is given for a (special) key
 *	K_SPECIAL   KS_MODIFIER	bitmask
 */
#وصف_ڪريو KS_MODIFIER		252

/*
 * These are used for the GUI
 *	K_SPECIAL   KS_xxx	KE_FILLER
 */
#وصف_ڪريو KS_MOUSE		251
#وصف_ڪريو KS_MENU			250
#وصف_ڪريو KS_VER_SCROLLBAR	249
#وصف_ڪريو KS_HOR_SCROLLBAR	248

/*
 * These are used for DEC mouse
 */
#وصف_ڪريو KS_NETTERM_MOUSE	247
#وصف_ڪريو KS_DEC_MOUSE		246

/*
 * Used for switching Select mode back on after a mapping or menu.
 */
#وصف_ڪريو KS_SELECT		245
#وصف_ڪريو K_SELECT_STRING		(char_u *)"\200\365X"

/*
 * Used for tearing off a menu.
 */
#وصف_ڪريو KS_TEAROFF		244

// Used for JSB term mouse.
#وصف_ڪريو KS_JSBTERM_MOUSE	243

// Used a termcap entry that produces a normal character.
#وصف_ڪريو KS_ڪنجي			242

// Used for the qnx pterm mouse.
#وصف_ڪريو KS_PTERM_MOUSE		241

// Used for click in a tab pages label.
#وصف_ڪريو KS_TABLINE		240

// Used for menu in a tab pages line.
#وصف_ڪريو KS_TABMENU		239

// Used for the urxvt mouse.
#وصف_ڪريو KS_URXVT_MOUSE		238

// Used for the sgr mouse.
#وصف_ڪريو KS_SGR_MOUSE		237
#وصف_ڪريو KS_SGR_MOUSE_RELEASE	236

// Used for the GPM mouse.
#وصف_ڪريو KS_GPM_MOUSE		235

/*
 * Filler used after KS_SPECIAL and others
 */
#وصف_ڪريو KE_FILLER		('X')

/*
 * translation of three byte code "K_SPECIAL a b" into int "K_xxx" and back
 */
#وصف_ڪريو TERMCAP2_ڪنجي(a, b)	(-((a) + ((صحيح)(b) << 8)))
#وصف_ڪريو KEY2TERMCAP0(x)		((-(x)) & 0xff)
#وصف_ڪريو KEY2TERMCAP1(x)		(((بنانشان)(-(x)) >> 8) & 0xff)

/*
 * get second or third byte when translating special key code into three bytes
 */
#وصف_ڪريو K_ٻيو(c)	((c) == K_SPECIAL ? KS_SPECIAL : (c) == NUL ? KS_ZERO : KEY2TERMCAP0(c))

#وصف_ڪريو K_ٽيون(c)	(((c) == K_SPECIAL || (c) == NUL) ? KE_FILLER : KEY2TERMCAP1(c))

/*
 * get single int code from second byte after K_SPECIAL
 */
#وصف_ڪريو منزل_SPECIAL(a, b)    ((a) == KS_SPECIAL ? K_SPECIAL : (a) == KS_ZERO ? K_ZERO : TERMCAP2_ڪنجي(a, b))

/*
 * Codes for keys that do not have a termcap name.
 * The numbers are fixed to make sure that recorded key sequences remain valid.
 * Add new entries at the end, not halfway.
 *
 * K_SPECIAL KS_EXTRA KE_xxx
 */
تعداد ڪنجي_extra
{
      KE_S_مٿي = 4		// shift-up
    , KE_S_هيٺ_ڏانهن = 5		// shift-down

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

    , KE_SNIFF_اڻ_استعمال_ٿيل = 56	// obsolete
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

    , KE_C_کاٻو = 85		// control-left
    , KE_C_ساڄو = 86		// control-right
    , KE_C_HOME = 87		// control-home
    , KE_C_پڇاڙي = 88		// control-end

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
#وصف_ڪريو K_ZERO		TERMCAP2_ڪنجي(KS_ZERO, KE_FILLER)

#وصف_ڪريو K_ESC		TERMCAP2_ڪنجي(KS_EXTRA, KE_ESC)

#وصف_ڪريو K_مٿي		TERMCAP2_ڪنجي('k', 'u')
#وصف_ڪريو K_هيٺ_ڏانهن		TERMCAP2_ڪنجي('k', 'd')
#وصف_ڪريو K_کاٻو		TERMCAP2_ڪنجي('k', 'l')
#وصف_ڪريو K_ساڄو		TERMCAP2_ڪنجي('k', 'r')
#وصف_ڪريو K_S_مٿي		TERMCAP2_ڪنجي(KS_EXTRA, KE_S_مٿي)
#وصف_ڪريو K_S_هيٺ_ڏانهن	TERMCAP2_ڪنجي(KS_EXTRA, KE_S_هيٺ_ڏانهن)
#وصف_ڪريو K_S_کاٻو	TERMCAP2_ڪنجي('#', '4')
#وصف_ڪريو K_C_کاٻو	TERMCAP2_ڪنجي(KS_EXTRA, KE_C_کاٻو)
#وصف_ڪريو K_S_ساڄو	TERMCAP2_ڪنجي('%', 'i')
#وصف_ڪريو K_C_ساڄو	TERMCAP2_ڪنجي(KS_EXTRA, KE_C_ساڄو)

#وصف_ڪريو K_S_HOME	TERMCAP2_ڪنجي('#', '2')
#وصف_ڪريو K_C_HOME	TERMCAP2_ڪنجي(KS_EXTRA, KE_C_HOME)
#وصف_ڪريو K_S_پڇاڙي		TERMCAP2_ڪنجي('*', '7')
#وصف_ڪريو K_C_پڇاڙي		TERMCAP2_ڪنجي(KS_EXTRA, KE_C_پڇاڙي)

#وصف_ڪريو K_TAB		TERMCAP2_ڪنجي(KS_EXTRA, KE_TAB)
#وصف_ڪريو K_S_TAB		TERMCAP2_ڪنجي('k', 'B')
#وصف_ڪريو K_S_BS		TERMCAP2_ڪنجي(KS_EXTRA, KE_S_BS)

// extra set of function keys F1-F4, for vt100 compatible xterm
#وصف_ڪريو K_XF1		TERMCAP2_ڪنجي(KS_EXTRA, KE_XF1)
#وصف_ڪريو K_XF2		TERMCAP2_ڪنجي(KS_EXTRA, KE_XF2)
#وصف_ڪريو K_XF3		TERMCAP2_ڪنجي(KS_EXTRA, KE_XF3)
#وصف_ڪريو K_XF4		TERMCAP2_ڪنجي(KS_EXTRA, KE_XF4)

// extra set of cursor keys for vt100 compatible xterm
#وصف_ڪريو K_XUP		TERMCAP2_ڪنجي(KS_EXTRA, KE_XUP)
#وصف_ڪريو K_XDOWN		TERMCAP2_ڪنجي(KS_EXTRA, KE_XDOWN)
#وصف_ڪريو K_XLEFT		TERMCAP2_ڪنجي(KS_EXTRA, KE_XLEFT)
#وصف_ڪريو K_XRIGHT	TERMCAP2_ڪنجي(KS_EXTRA, KE_XRIGHT)

#وصف_ڪريو K_F1		TERMCAP2_ڪنجي('k', '1')	// function keys
#وصف_ڪريو K_F2		TERMCAP2_ڪنجي('k', '2')
#وصف_ڪريو K_F3		TERMCAP2_ڪنجي('k', '3')
#وصف_ڪريو K_F4		TERMCAP2_ڪنجي('k', '4')
#وصف_ڪريو K_F5		TERMCAP2_ڪنجي('k', '5')
#وصف_ڪريو K_F6		TERMCAP2_ڪنجي('k', '6')
#وصف_ڪريو K_F7		TERMCAP2_ڪنجي('k', '7')
#وصف_ڪريو K_F8		TERMCAP2_ڪنجي('k', '8')
#وصف_ڪريو K_F9		TERMCAP2_ڪنجي('k', '9')
#وصف_ڪريو K_F10		TERMCAP2_ڪنجي('k', ';')

#وصف_ڪريو K_F11		TERMCAP2_ڪنجي('F', '1')
#وصف_ڪريو K_F12		TERMCAP2_ڪنجي('F', '2')
#وصف_ڪريو K_F13		TERMCAP2_ڪنجي('F', '3')
#وصف_ڪريو K_F14		TERMCAP2_ڪنجي('F', '4')
#وصف_ڪريو K_F15		TERMCAP2_ڪنجي('F', '5')
#وصف_ڪريو K_F16		TERMCAP2_ڪنجي('F', '6')
#وصف_ڪريو K_F17		TERMCAP2_ڪنجي('F', '7')
#وصف_ڪريو K_F18		TERMCAP2_ڪنجي('F', '8')
#وصف_ڪريو K_F19		TERMCAP2_ڪنجي('F', '9')
#وصف_ڪريو K_F20		TERMCAP2_ڪنجي('F', 'A')

#وصف_ڪريو K_F21		TERMCAP2_ڪنجي('F', 'B')
#وصف_ڪريو K_F22		TERMCAP2_ڪنجي('F', 'C')
#وصف_ڪريو K_F23		TERMCAP2_ڪنجي('F', 'D')
#وصف_ڪريو K_F24		TERMCAP2_ڪنجي('F', 'E')
#وصف_ڪريو K_F25		TERMCAP2_ڪنجي('F', 'F')
#وصف_ڪريو K_F26		TERMCAP2_ڪنجي('F', 'G')
#وصف_ڪريو K_F27		TERMCAP2_ڪنجي('F', 'H')
#وصف_ڪريو K_F28		TERMCAP2_ڪنجي('F', 'I')
#وصف_ڪريو K_F29		TERMCAP2_ڪنجي('F', 'J')
#وصف_ڪريو K_F30		TERMCAP2_ڪنجي('F', 'K')

#وصف_ڪريو K_F31		TERMCAP2_ڪنجي('F', 'L')
#وصف_ڪريو K_F32		TERMCAP2_ڪنجي('F', 'M')
#وصف_ڪريو K_F33		TERMCAP2_ڪنجي('F', 'N')
#وصف_ڪريو K_F34		TERMCAP2_ڪنجي('F', 'O')
#وصف_ڪريو K_F35		TERMCAP2_ڪنجي('F', 'P')
#وصف_ڪريو K_F36		TERMCAP2_ڪنجي('F', 'Q')
#وصف_ڪريو K_F37		TERMCAP2_ڪنجي('F', 'R')

// extra set of shifted function keys F1-F4, for vt100 compatible xterm
#وصف_ڪريو K_S_XF1		TERMCAP2_ڪنجي(KS_EXTRA, KE_S_XF1)
#وصف_ڪريو K_S_XF2		TERMCAP2_ڪنجي(KS_EXTRA, KE_S_XF2)
#وصف_ڪريو K_S_XF3		TERMCAP2_ڪنجي(KS_EXTRA, KE_S_XF3)
#وصف_ڪريو K_S_XF4		TERMCAP2_ڪنجي(KS_EXTRA, KE_S_XF4)

#وصف_ڪريو K_S_F1		TERMCAP2_ڪنجي(KS_EXTRA, KE_S_F1)	// shifted func. keys
#وصف_ڪريو K_S_F2		TERMCAP2_ڪنجي(KS_EXTRA, KE_S_F2)
#وصف_ڪريو K_S_F3		TERMCAP2_ڪنجي(KS_EXTRA, KE_S_F3)
#وصف_ڪريو K_S_F4		TERMCAP2_ڪنجي(KS_EXTRA, KE_S_F4)
#وصف_ڪريو K_S_F5		TERMCAP2_ڪنجي(KS_EXTRA, KE_S_F5)
#وصف_ڪريو K_S_F6		TERMCAP2_ڪنجي(KS_EXTRA, KE_S_F6)
#وصف_ڪريو K_S_F7		TERMCAP2_ڪنجي(KS_EXTRA, KE_S_F7)
#وصف_ڪريو K_S_F8		TERMCAP2_ڪنجي(KS_EXTRA, KE_S_F8)
#وصف_ڪريو K_S_F9		TERMCAP2_ڪنجي(KS_EXTRA, KE_S_F9)
#وصف_ڪريو K_S_F10		TERMCAP2_ڪنجي(KS_EXTRA, KE_S_F10)

#وصف_ڪريو K_S_F11		TERMCAP2_ڪنجي(KS_EXTRA, KE_S_F11)
#وصف_ڪريو K_S_F12		TERMCAP2_ڪنجي(KS_EXTRA, KE_S_F12)
// K_S_F13 to K_S_F37  are currently not used

#وصف_ڪريو K_HELP		TERMCAP2_ڪنجي('%', '1')
#وصف_ڪريو K_UNDO		TERMCAP2_ڪنجي('&', '8')

#وصف_ڪريو K_BS		TERMCAP2_ڪنجي('k', 'b')

#وصف_ڪريو K_INS		TERMCAP2_ڪنجي('k', 'I')
#وصف_ڪريو K_KINS		TERMCAP2_ڪنجي(KS_EXTRA, KE_KINS)
#وصف_ڪريو K_DEL		TERMCAP2_ڪنجي('k', 'D')
#وصف_ڪريو K_KDEL		TERMCAP2_ڪنجي(KS_EXTRA, KE_KDEL)
#وصف_ڪريو K_HOME		TERMCAP2_ڪنجي('k', 'h')
#وصف_ڪريو K_KHOME		TERMCAP2_ڪنجي('K', '1')	// keypad home (upper left)
#وصف_ڪريو K_XHOME		TERMCAP2_ڪنجي(KS_EXTRA, KE_XHOME)
#وصف_ڪريو K_ZHOME		TERMCAP2_ڪنجي(KS_EXTRA, KE_ZHOME)
#وصف_ڪريو K_پڇاڙي		TERMCAP2_ڪنجي('@', '7')
#وصف_ڪريو K_KEND		TERMCAP2_ڪنجي('K', '4')	// keypad end (lower left)
#وصف_ڪريو K_XEND		TERMCAP2_ڪنجي(KS_EXTRA, KE_XEND)
#وصف_ڪريو K_ZEND		TERMCAP2_ڪنجي(KS_EXTRA, KE_ZEND)
#وصف_ڪريو K_PAGEUP	TERMCAP2_ڪنجي('k', 'P')
#وصف_ڪريو K_PAGEDOWN	TERMCAP2_ڪنجي('k', 'N')
#وصف_ڪريو K_KPAGEUP	TERMCAP2_ڪنجي('K', '3')	// keypad pageup (upper R.)
#وصف_ڪريو K_KPAGEDOWN	TERMCAP2_ڪنجي('K', '5')	// keypad pagedown (lower R.)

#وصف_ڪريو K_KPLUS		TERMCAP2_ڪنجي('K', '6')	// keypad plus
#وصف_ڪريو K_KMINUS	TERMCAP2_ڪنجي('K', '7')	// keypad minus
#وصف_ڪريو K_KDIVIDE	TERMCAP2_ڪنجي('K', '8')	// keypad /
#وصف_ڪريو K_KMULTIPLY	TERMCAP2_ڪنجي('K', '9')	// keypad *
#وصف_ڪريو K_KENTER	TERMCAP2_ڪنجي('K', 'A')	// keypad Enter
#وصف_ڪريو K_KPOINT	TERMCAP2_ڪنجي('K', 'B')	// keypad . or ,
#وصف_ڪريو K_PS		TERMCAP2_ڪنجي('P', 'S')	// paste start
#وصف_ڪريو K_PE		TERMCAP2_ڪنجي('P', 'E')	// paste end

#وصف_ڪريو K_K0		TERMCAP2_ڪنجي('K', 'C')	// keypad 0
#وصف_ڪريو K_K1		TERMCAP2_ڪنجي('K', 'D')	// keypad 1
#وصف_ڪريو K_K2		TERMCAP2_ڪنجي('K', 'E')	// keypad 2
#وصف_ڪريو K_K3		TERMCAP2_ڪنجي('K', 'F')	// keypad 3
#وصف_ڪريو K_K4		TERMCAP2_ڪنجي('K', 'G')	// keypad 4
#وصف_ڪريو K_K5		TERMCAP2_ڪنجي('K', 'H')	// keypad 5
#وصف_ڪريو K_K6		TERMCAP2_ڪنجي('K', 'I')	// keypad 6
#وصف_ڪريو K_K7		TERMCAP2_ڪنجي('K', 'J')	// keypad 7
#وصف_ڪريو K_K8		TERMCAP2_ڪنجي('K', 'K')	// keypad 8
#وصف_ڪريو K_K9		TERMCAP2_ڪنجي('K', 'L')	// keypad 9

#وصف_ڪريو K_MOUSE		TERMCAP2_ڪنجي(KS_MOUSE, KE_FILLER)
#وصف_ڪريو K_MENU		TERMCAP2_ڪنجي(KS_MENU, KE_FILLER)
#وصف_ڪريو K_VER_SCROLLBAR	TERMCAP2_ڪنجي(KS_VER_SCROLLBAR, KE_FILLER)
#وصف_ڪريو K_HOR_SCROLLBAR   TERMCAP2_ڪنجي(KS_HOR_SCROLLBAR, KE_FILLER)

#وصف_ڪريو K_NETTERM_MOUSE	TERMCAP2_ڪنجي(KS_NETTERM_MOUSE, KE_FILLER)
#وصف_ڪريو K_DEC_MOUSE	TERMCAP2_ڪنجي(KS_DEC_MOUSE, KE_FILLER)
#وصف_ڪريو K_JSBTERM_MOUSE	TERMCAP2_ڪنجي(KS_JSBTERM_MOUSE, KE_FILLER)
#وصف_ڪريو K_PTERM_MOUSE	TERMCAP2_ڪنجي(KS_PTERM_MOUSE, KE_FILLER)
#وصف_ڪريو K_URXVT_MOUSE	TERMCAP2_ڪنجي(KS_URXVT_MOUSE, KE_FILLER)
#وصف_ڪريو K_SGR_MOUSE	TERMCAP2_ڪنجي(KS_SGR_MOUSE, KE_FILLER)
#وصف_ڪريو K_SGR_MOUSERELEASE TERMCAP2_ڪنجي(KS_SGR_MOUSE_RELEASE, KE_FILLER)

#وصف_ڪريو K_SELECT	TERMCAP2_ڪنجي(KS_SELECT, KE_FILLER)
#وصف_ڪريو K_TEAROFF	TERMCAP2_ڪنجي(KS_TEAROFF, KE_FILLER)

#وصف_ڪريو K_TABLINE	TERMCAP2_ڪنجي(KS_TABLINE, KE_FILLER)
#وصف_ڪريو K_TABMENU	TERMCAP2_ڪنجي(KS_TABMENU, KE_FILLER)

/*
 * Symbols for pseudo keys which are translated from the real key symbols
 * above.
 */
#وصف_ڪريو K_LEFTMOUSE	TERMCAP2_ڪنجي(KS_EXTRA, KE_LEFTMOUSE)
#وصف_ڪريو K_LEFTMOUSE_NM	TERMCAP2_ڪنجي(KS_EXTRA, KE_LEFTMOUSE_NM)
#وصف_ڪريو K_LEFTDRAG	TERMCAP2_ڪنجي(KS_EXTRA, KE_LEFTDRAG)
#وصف_ڪريو K_LEFTRELEASE	TERMCAP2_ڪنجي(KS_EXTRA, KE_LEFTRELEASE)
#وصف_ڪريو K_LEFTRELEASE_NM TERMCAP2_ڪنجي(KS_EXTRA, KE_LEFTRELEASE_NM)
#وصف_ڪريو K_MOUSEMOVE	TERMCAP2_ڪنجي(KS_EXTRA, KE_MOUSEMOVE)
#وصف_ڪريو K_MIDDLEMOUSE	TERMCAP2_ڪنجي(KS_EXTRA, KE_MIDDLEMOUSE)
#وصف_ڪريو K_MIDDLEDRAG	TERMCAP2_ڪنجي(KS_EXTRA, KE_MIDDLEDRAG)
#وصف_ڪريو K_MIDDLERELEASE	TERMCAP2_ڪنجي(KS_EXTRA, KE_MIDDLERELEASE)
#وصف_ڪريو K_RIGHTMOUSE	TERMCAP2_ڪنجي(KS_EXTRA, KE_RIGHTMOUSE)
#وصف_ڪريو K_RIGHTDRAG	TERMCAP2_ڪنجي(KS_EXTRA, KE_RIGHTDRAG)
#وصف_ڪريو K_RIGHTRELEASE	TERMCAP2_ڪنجي(KS_EXTRA, KE_RIGHTRELEASE)
#وصف_ڪريو K_X1MOUSE	TERMCAP2_ڪنجي(KS_EXTRA, KE_X1MOUSE)
#وصف_ڪريو K_X1MOUSE	TERMCAP2_ڪنجي(KS_EXTRA, KE_X1MOUSE)
#وصف_ڪريو K_X1DRAG	TERMCAP2_ڪنجي(KS_EXTRA, KE_X1DRAG)
#وصف_ڪريو K_X1RELEASE     TERMCAP2_ڪنجي(KS_EXTRA, KE_X1RELEASE)
#وصف_ڪريو K_X2MOUSE	TERMCAP2_ڪنجي(KS_EXTRA, KE_X2MOUSE)
#وصف_ڪريو K_X2DRAG	TERMCAP2_ڪنجي(KS_EXTRA, KE_X2DRAG)
#وصف_ڪريو K_X2RELEASE     TERMCAP2_ڪنجي(KS_EXTRA, KE_X2RELEASE)

#وصف_ڪريو K_IGNORE	TERMCAP2_ڪنجي(KS_EXTRA, KE_IGNORE)
#وصف_ڪريو K_NOP		TERMCAP2_ڪنجي(KS_EXTRA, KE_NOP)
#وصف_ڪريو K_CANCEL	TERMCAP2_ڪنجي(KS_EXTRA, KE_CANCEL)

#وصف_ڪريو K_MOUSEDOWN	TERMCAP2_ڪنجي(KS_EXTRA, KE_MOUSEDOWN)
#وصف_ڪريو K_MOUSEUP	TERMCAP2_ڪنجي(KS_EXTRA, KE_MOUSEUP)
#وصف_ڪريو K_MOUSELEFT	TERMCAP2_ڪنجي(KS_EXTRA, KE_MOUSELEFT)
#وصف_ڪريو K_MOUSERIGHT	TERMCAP2_ڪنجي(KS_EXTRA, KE_MOUSERIGHT)

#وصف_ڪريو K_CSI		TERMCAP2_ڪنجي(KS_EXTRA, KE_CSI)
#وصف_ڪريو K_OSC		TERMCAP2_ڪنجي(KS_EXTRA, KE_OSC)
#وصف_ڪريو K_SNR		TERMCAP2_ڪنجي(KS_EXTRA, KE_SNR)
#وصف_ڪريو K_PLUG		TERMCAP2_ڪنجي(KS_EXTRA, KE_PLUG)
#وصف_ڪريو K_CMDWIN	TERMCAP2_ڪنجي(KS_EXTRA, KE_CMDWIN)

#وصف_ڪريو K_DROP		TERMCAP2_ڪنجي(KS_EXTRA, KE_DROP)
#وصف_ڪريو K_FOCUSGAINED	TERMCAP2_ڪنجي(KS_EXTRA, KE_FOCUSGAINED)
#وصف_ڪريو K_FOCUSLOST	TERMCAP2_ڪنجي(KS_EXTRA, KE_FOCUSLOST)

#وصف_ڪريو K_CURSORHOLD	TERMCAP2_ڪنجي(KS_EXTRA, KE_CURSORHOLD)
#وصف_ڪريو K_COMPLETE_DELAY TERMCAP2_ڪنجي(KS_EXTRA, KE_COMPLETE_DELAY)

#وصف_ڪريو K_COMMAND	TERMCAP2_ڪنجي(KS_EXTRA, KE_COMMAND)
#وصف_ڪريو K_SCRIPT_COMMAND TERMCAP2_ڪنجي(KS_EXTRA, KE_SCRIPT_COMMAND)
#وصف_ڪريو K_SID		TERMCAP2_ڪنجي(KS_EXTRA, KE_SID)

#وصف_ڪريو K_WILD		TERMCAP2_ڪنجي(KS_EXTRA, KE_WILD)

// Bits for modifier mask
// 0x01 cannot be used, because the modifier must be 0x02 or higher
#وصف_ڪريو MOD_نقاب_سرڪايو	    0x02
#وصف_ڪريو MOD_نقاب_CTRL	    0x04
#وصف_ڪريو MOD_نقاب_ALT	    0x08	// aka META
#وصف_ڪريو MOD_نقاب_META	    0x10	// META when it's different from ALT
#وصف_ڪريو MOD_نقاب_2_CLICK	    0x20	// use MOD_MASK_MULTI_CLICK
#وصف_ڪريو MOD_نقاب_3_CLICK	    0x40	// use MOD_MASK_MULTI_CLICK
#وصف_ڪريو MOD_نقاب_4_CLICK	    0x60	// use MOD_MASK_MULTI_CLICK
#إذا defined(MACOS_X) || defined(FEAT_GUI_GTK)
# define MOD_نقاب_CMD	    0x80        // aka SUPER
#جيڪڏهن_پورو

#وصف_ڪريو MOD_نقاب_MULTI_CLICK	(MOD_نقاب_2_CLICK|MOD_نقاب_3_CLICK|MOD_نقاب_4_CLICK)

/*
 * The length of the longest special key name, including modifiers.
 * Current longest is <M-C-S-T-D-A-4-ScrollWheelRight> (length includes '<' and
 * '>').
 */
#وصف_ڪريو MAX_ڪنجي_NAME_LEN    32

/*
 * Maximum length of a special key event as tokens.  This includes modifiers.
 * The longest event is something like <M-C-S-T-4-LeftDrag> which would be the
 * following string of tokens:
 *
 * <K_SPECIAL> <KS_MODIFIER> bitmask <K_SPECIAL> <KS_EXTRA> <KT_LEFTDRAG>.
 *
 * This is a total of 6 tokens, and is currently the longest one possible.
 */
#وصف_ڪريو MAX_ڪنجي_ڪوڊ_LEN    6
