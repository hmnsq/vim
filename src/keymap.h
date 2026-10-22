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
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_NUL			(0xce)	// for MSDOS: special key follows

/*
 * K_SPECIAL is the first byte of a special key code and is always followed by
 * two bytes.
 * The second byte can have any value. ASCII is used for normal termcap
 * entries, 0x80 and higher for special keys, see below.
 * The third byte is guaranteed to be between 0x02 and 0x7f.
 */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_SPECIAL		(0x80)

/*
 * Positive characters are "normal" characters.
 * Negative characters are special key codes.  Only characters below -0x200
 * are used to so that the absolute value can't be mistaken for a single-byte
 * character.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IS_SPECIAL(c)		((c) < 0)

/*
 * Characters 0x0100 - 0x01ff have a special meaning for abbreviations.
 * Multi-byte characters also have ABBR_OFF added, thus are above 0x0200.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ABBR_OFF		0x100

/*
 * NUL cannot be in the input string, therefore it is replaced by
 *	K_SPECIAL   KS_ZERO	KE_FILLER
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ KS_ZERO			255

/*
 * K_SPECIAL cannot be in the input string, therefore it is replaced by
 *	K_SPECIAL   KS_SPECIAL	KE_FILLER
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ KS_SPECIAL		254

/*
 * KS_EXTRA is used for keys that have no termcap name
 *	K_SPECIAL   KS_EXTRA	KE_xxx
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ KS_EXTRA		253

/*
 * KS_MODIFIER is used when a modifier is given for a (special) key
 *	K_SPECIAL   KS_MODIFIER	bitmask
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ KS_MODIFIER		252

/*
 * These are used for the GUI
 *	K_SPECIAL   KS_xxx	KE_FILLER
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ KS_MOUSE		251
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ KS_MENU			250
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ KS_VER_SCROLLBAR	249
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ KS_HOR_SCROLLBAR	248

/*
 * These are used for DEC mouse
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ KS_NETTERM_MOUSE	247
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ KS_DEC_MOUSE		246

/*
 * Used for switching Select mode back on after a mapping or menu.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ KS_SELECT		245
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_SELECT_STRING		(char_u *)"\200\365X"

/*
 * Used for tearing off a menu.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ KS_TEAROFF		244

// Used for JSB term mouse.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ KS_JSBTERM_MOUSE	243

// Used a termcap entry that produces a normal character.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ KS_ਕੁੰਜੀ			242

// Used for the qnx pterm mouse.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ KS_PTERM_MOUSE		241

// Used for click in a tab pages label.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ KS_TABLINE		240

// Used for menu in a tab pages line.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ KS_TABMENU		239

// Used for the urxvt mouse.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ KS_URXVT_MOUSE		238

// Used for the sgr mouse.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ KS_SGR_MOUSE		237
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ KS_SGR_MOUSE_RELEASE	236

// Used for the GPM mouse.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ KS_GPM_MOUSE		235

/*
 * Filler used after KS_SPECIAL and others
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ KE_FILLER		('X')

/*
 * translation of three byte code "K_SPECIAL a b" into int "K_xxx" and back
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TERMCAP2_ਕੁੰਜੀ(a, b)	(-((a) + ((ਪੂਰਨ)(b) << 8)))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ KEY2TERMCAP0(x)		((-(x)) & 0xff)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ KEY2TERMCAP1(x)		(((ਬਿਨਾਨਿਸ਼ਾਨੀ)(-(x)) >> 8) & 0xff)

/*
 * get second or third byte when translating special key code into three bytes
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_ਦੂਜਾ(c)	((c) == K_SPECIAL ? KS_SPECIAL : (c) == NUL ? KS_ZERO : KEY2TERMCAP0(c))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_ਤੀਜਾ(c)	(((c) == K_SPECIAL || (c) == NUL) ? KE_FILLER : KEY2TERMCAP1(c))

/*
 * get single int code from second byte after K_SPECIAL
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਮੰਜ਼ਿਲ_SPECIAL(a, b)    ((a) == KS_SPECIAL ? K_SPECIAL : (a) == KS_ZERO ? K_ZERO : TERMCAP2_ਕੁੰਜੀ(a, b))

/*
 * Codes for keys that do not have a termcap name.
 * The numbers are fixed to make sure that recorded key sequences remain valid.
 * Add new entries at the end, not halfway.
 *
 * K_SPECIAL KS_EXTRA KE_xxx
 */
ਗਿਣਤੀ ਕੁੰਜੀ_extra
{
      KE_S_ਉੱਪਰ = 4		// shift-up
    , KE_S_ਥੱਲੇ = 5		// shift-down

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

    , KE_SNIFF_ਅਣਵਰਤਿਆ = 56	// obsolete
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

    , KE_C_ਖੱਬਾ = 85		// control-left
    , KE_C_ਸੱਜਾ = 86		// control-right
    , KE_C_HOME = 87		// control-home
    , KE_C_ਸਿਰਾ = 88		// control-end

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
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_ZERO		TERMCAP2_ਕੁੰਜੀ(KS_ZERO, KE_FILLER)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_ESC		TERMCAP2_ਕੁੰਜੀ(KS_EXTRA, KE_ESC)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_ਉੱਪਰ		TERMCAP2_ਕੁੰਜੀ('k', 'u')
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_ਥੱਲੇ		TERMCAP2_ਕੁੰਜੀ('k', 'd')
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_ਖੱਬਾ		TERMCAP2_ਕੁੰਜੀ('k', 'l')
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_ਸੱਜਾ		TERMCAP2_ਕੁੰਜੀ('k', 'r')
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_S_ਉੱਪਰ		TERMCAP2_ਕੁੰਜੀ(KS_EXTRA, KE_S_ਉੱਪਰ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_S_ਥੱਲੇ	TERMCAP2_ਕੁੰਜੀ(KS_EXTRA, KE_S_ਥੱਲੇ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_S_ਖੱਬਾ	TERMCAP2_ਕੁੰਜੀ('#', '4')
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_C_ਖੱਬਾ	TERMCAP2_ਕੁੰਜੀ(KS_EXTRA, KE_C_ਖੱਬਾ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_S_ਸੱਜਾ	TERMCAP2_ਕੁੰਜੀ('%', 'i')
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_C_ਸੱਜਾ	TERMCAP2_ਕੁੰਜੀ(KS_EXTRA, KE_C_ਸੱਜਾ)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_S_HOME	TERMCAP2_ਕੁੰਜੀ('#', '2')
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_C_HOME	TERMCAP2_ਕੁੰਜੀ(KS_EXTRA, KE_C_HOME)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_S_ਸਿਰਾ		TERMCAP2_ਕੁੰਜੀ('*', '7')
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_C_ਸਿਰਾ		TERMCAP2_ਕੁੰਜੀ(KS_EXTRA, KE_C_ਸਿਰਾ)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_TAB		TERMCAP2_ਕੁੰਜੀ(KS_EXTRA, KE_TAB)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_S_TAB		TERMCAP2_ਕੁੰਜੀ('k', 'B')
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_S_BS		TERMCAP2_ਕੁੰਜੀ(KS_EXTRA, KE_S_BS)

// extra set of function keys F1-F4, for vt100 compatible xterm
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_XF1		TERMCAP2_ਕੁੰਜੀ(KS_EXTRA, KE_XF1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_XF2		TERMCAP2_ਕੁੰਜੀ(KS_EXTRA, KE_XF2)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_XF3		TERMCAP2_ਕੁੰਜੀ(KS_EXTRA, KE_XF3)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_XF4		TERMCAP2_ਕੁੰਜੀ(KS_EXTRA, KE_XF4)

// extra set of cursor keys for vt100 compatible xterm
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_XUP		TERMCAP2_ਕੁੰਜੀ(KS_EXTRA, KE_XUP)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_XDOWN		TERMCAP2_ਕੁੰਜੀ(KS_EXTRA, KE_XDOWN)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_XLEFT		TERMCAP2_ਕੁੰਜੀ(KS_EXTRA, KE_XLEFT)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_XRIGHT	TERMCAP2_ਕੁੰਜੀ(KS_EXTRA, KE_XRIGHT)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_F1		TERMCAP2_ਕੁੰਜੀ('k', '1')	// function keys
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_F2		TERMCAP2_ਕੁੰਜੀ('k', '2')
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_F3		TERMCAP2_ਕੁੰਜੀ('k', '3')
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_F4		TERMCAP2_ਕੁੰਜੀ('k', '4')
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_F5		TERMCAP2_ਕੁੰਜੀ('k', '5')
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_F6		TERMCAP2_ਕੁੰਜੀ('k', '6')
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_F7		TERMCAP2_ਕੁੰਜੀ('k', '7')
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_F8		TERMCAP2_ਕੁੰਜੀ('k', '8')
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_F9		TERMCAP2_ਕੁੰਜੀ('k', '9')
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_F10		TERMCAP2_ਕੁੰਜੀ('k', ';')

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_F11		TERMCAP2_ਕੁੰਜੀ('F', '1')
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_F12		TERMCAP2_ਕੁੰਜੀ('F', '2')
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_F13		TERMCAP2_ਕੁੰਜੀ('F', '3')
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_F14		TERMCAP2_ਕੁੰਜੀ('F', '4')
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_F15		TERMCAP2_ਕੁੰਜੀ('F', '5')
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_F16		TERMCAP2_ਕੁੰਜੀ('F', '6')
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_F17		TERMCAP2_ਕੁੰਜੀ('F', '7')
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_F18		TERMCAP2_ਕੁੰਜੀ('F', '8')
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_F19		TERMCAP2_ਕੁੰਜੀ('F', '9')
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_F20		TERMCAP2_ਕੁੰਜੀ('F', 'A')

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_F21		TERMCAP2_ਕੁੰਜੀ('F', 'B')
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_F22		TERMCAP2_ਕੁੰਜੀ('F', 'C')
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_F23		TERMCAP2_ਕੁੰਜੀ('F', 'D')
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_F24		TERMCAP2_ਕੁੰਜੀ('F', 'E')
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_F25		TERMCAP2_ਕੁੰਜੀ('F', 'F')
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_F26		TERMCAP2_ਕੁੰਜੀ('F', 'G')
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_F27		TERMCAP2_ਕੁੰਜੀ('F', 'H')
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_F28		TERMCAP2_ਕੁੰਜੀ('F', 'I')
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_F29		TERMCAP2_ਕੁੰਜੀ('F', 'J')
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_F30		TERMCAP2_ਕੁੰਜੀ('F', 'K')

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_F31		TERMCAP2_ਕੁੰਜੀ('F', 'L')
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_F32		TERMCAP2_ਕੁੰਜੀ('F', 'M')
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_F33		TERMCAP2_ਕੁੰਜੀ('F', 'N')
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_F34		TERMCAP2_ਕੁੰਜੀ('F', 'O')
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_F35		TERMCAP2_ਕੁੰਜੀ('F', 'P')
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_F36		TERMCAP2_ਕੁੰਜੀ('F', 'Q')
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_F37		TERMCAP2_ਕੁੰਜੀ('F', 'R')

// extra set of shifted function keys F1-F4, for vt100 compatible xterm
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_S_XF1		TERMCAP2_ਕੁੰਜੀ(KS_EXTRA, KE_S_XF1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_S_XF2		TERMCAP2_ਕੁੰਜੀ(KS_EXTRA, KE_S_XF2)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_S_XF3		TERMCAP2_ਕੁੰਜੀ(KS_EXTRA, KE_S_XF3)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_S_XF4		TERMCAP2_ਕੁੰਜੀ(KS_EXTRA, KE_S_XF4)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_S_F1		TERMCAP2_ਕੁੰਜੀ(KS_EXTRA, KE_S_F1)	// shifted func. keys
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_S_F2		TERMCAP2_ਕੁੰਜੀ(KS_EXTRA, KE_S_F2)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_S_F3		TERMCAP2_ਕੁੰਜੀ(KS_EXTRA, KE_S_F3)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_S_F4		TERMCAP2_ਕੁੰਜੀ(KS_EXTRA, KE_S_F4)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_S_F5		TERMCAP2_ਕੁੰਜੀ(KS_EXTRA, KE_S_F5)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_S_F6		TERMCAP2_ਕੁੰਜੀ(KS_EXTRA, KE_S_F6)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_S_F7		TERMCAP2_ਕੁੰਜੀ(KS_EXTRA, KE_S_F7)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_S_F8		TERMCAP2_ਕੁੰਜੀ(KS_EXTRA, KE_S_F8)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_S_F9		TERMCAP2_ਕੁੰਜੀ(KS_EXTRA, KE_S_F9)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_S_F10		TERMCAP2_ਕੁੰਜੀ(KS_EXTRA, KE_S_F10)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_S_F11		TERMCAP2_ਕੁੰਜੀ(KS_EXTRA, KE_S_F11)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_S_F12		TERMCAP2_ਕੁੰਜੀ(KS_EXTRA, KE_S_F12)
// K_S_F13 to K_S_F37  are currently not used

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_HELP		TERMCAP2_ਕੁੰਜੀ('%', '1')
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_UNDO		TERMCAP2_ਕੁੰਜੀ('&', '8')

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_BS		TERMCAP2_ਕੁੰਜੀ('k', 'b')

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_INS		TERMCAP2_ਕੁੰਜੀ('k', 'I')
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_KINS		TERMCAP2_ਕੁੰਜੀ(KS_EXTRA, KE_KINS)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_DEL		TERMCAP2_ਕੁੰਜੀ('k', 'D')
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_KDEL		TERMCAP2_ਕੁੰਜੀ(KS_EXTRA, KE_KDEL)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_HOME		TERMCAP2_ਕੁੰਜੀ('k', 'h')
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_KHOME		TERMCAP2_ਕੁੰਜੀ('K', '1')	// keypad home (upper left)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_XHOME		TERMCAP2_ਕੁੰਜੀ(KS_EXTRA, KE_XHOME)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_ZHOME		TERMCAP2_ਕੁੰਜੀ(KS_EXTRA, KE_ZHOME)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_ਸਿਰਾ		TERMCAP2_ਕੁੰਜੀ('@', '7')
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_KEND		TERMCAP2_ਕੁੰਜੀ('K', '4')	// keypad end (lower left)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_XEND		TERMCAP2_ਕੁੰਜੀ(KS_EXTRA, KE_XEND)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_ZEND		TERMCAP2_ਕੁੰਜੀ(KS_EXTRA, KE_ZEND)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_PAGEUP	TERMCAP2_ਕੁੰਜੀ('k', 'P')
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_PAGEDOWN	TERMCAP2_ਕੁੰਜੀ('k', 'N')
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_KPAGEUP	TERMCAP2_ਕੁੰਜੀ('K', '3')	// keypad pageup (upper R.)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_KPAGEDOWN	TERMCAP2_ਕੁੰਜੀ('K', '5')	// keypad pagedown (lower R.)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_KPLUS		TERMCAP2_ਕੁੰਜੀ('K', '6')	// keypad plus
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_KMINUS	TERMCAP2_ਕੁੰਜੀ('K', '7')	// keypad minus
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_KDIVIDE	TERMCAP2_ਕੁੰਜੀ('K', '8')	// keypad /
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_KMULTIPLY	TERMCAP2_ਕੁੰਜੀ('K', '9')	// keypad *
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_KENTER	TERMCAP2_ਕੁੰਜੀ('K', 'A')	// keypad Enter
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_KPOINT	TERMCAP2_ਕੁੰਜੀ('K', 'B')	// keypad . or ,
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_PS		TERMCAP2_ਕੁੰਜੀ('P', 'S')	// paste start
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_PE		TERMCAP2_ਕੁੰਜੀ('P', 'E')	// paste end

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_K0		TERMCAP2_ਕੁੰਜੀ('K', 'C')	// keypad 0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_K1		TERMCAP2_ਕੁੰਜੀ('K', 'D')	// keypad 1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_K2		TERMCAP2_ਕੁੰਜੀ('K', 'E')	// keypad 2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_K3		TERMCAP2_ਕੁੰਜੀ('K', 'F')	// keypad 3
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_K4		TERMCAP2_ਕੁੰਜੀ('K', 'G')	// keypad 4
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_K5		TERMCAP2_ਕੁੰਜੀ('K', 'H')	// keypad 5
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_K6		TERMCAP2_ਕੁੰਜੀ('K', 'I')	// keypad 6
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_K7		TERMCAP2_ਕੁੰਜੀ('K', 'J')	// keypad 7
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_K8		TERMCAP2_ਕੁੰਜੀ('K', 'K')	// keypad 8
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_K9		TERMCAP2_ਕੁੰਜੀ('K', 'L')	// keypad 9

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_MOUSE		TERMCAP2_ਕੁੰਜੀ(KS_MOUSE, KE_FILLER)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_MENU		TERMCAP2_ਕੁੰਜੀ(KS_MENU, KE_FILLER)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_VER_SCROLLBAR	TERMCAP2_ਕੁੰਜੀ(KS_VER_SCROLLBAR, KE_FILLER)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_HOR_SCROLLBAR   TERMCAP2_ਕੁੰਜੀ(KS_HOR_SCROLLBAR, KE_FILLER)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_NETTERM_MOUSE	TERMCAP2_ਕੁੰਜੀ(KS_NETTERM_MOUSE, KE_FILLER)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_DEC_MOUSE	TERMCAP2_ਕੁੰਜੀ(KS_DEC_MOUSE, KE_FILLER)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_JSBTERM_MOUSE	TERMCAP2_ਕੁੰਜੀ(KS_JSBTERM_MOUSE, KE_FILLER)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_PTERM_MOUSE	TERMCAP2_ਕੁੰਜੀ(KS_PTERM_MOUSE, KE_FILLER)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_URXVT_MOUSE	TERMCAP2_ਕੁੰਜੀ(KS_URXVT_MOUSE, KE_FILLER)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_SGR_MOUSE	TERMCAP2_ਕੁੰਜੀ(KS_SGR_MOUSE, KE_FILLER)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_SGR_MOUSERELEASE TERMCAP2_ਕੁੰਜੀ(KS_SGR_MOUSE_RELEASE, KE_FILLER)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_SELECT	TERMCAP2_ਕੁੰਜੀ(KS_SELECT, KE_FILLER)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_TEAROFF	TERMCAP2_ਕੁੰਜੀ(KS_TEAROFF, KE_FILLER)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_TABLINE	TERMCAP2_ਕੁੰਜੀ(KS_TABLINE, KE_FILLER)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_TABMENU	TERMCAP2_ਕੁੰਜੀ(KS_TABMENU, KE_FILLER)

/*
 * Symbols for pseudo keys which are translated from the real key symbols
 * above.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_LEFTMOUSE	TERMCAP2_ਕੁੰਜੀ(KS_EXTRA, KE_LEFTMOUSE)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_LEFTMOUSE_NM	TERMCAP2_ਕੁੰਜੀ(KS_EXTRA, KE_LEFTMOUSE_NM)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_LEFTDRAG	TERMCAP2_ਕੁੰਜੀ(KS_EXTRA, KE_LEFTDRAG)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_LEFTRELEASE	TERMCAP2_ਕੁੰਜੀ(KS_EXTRA, KE_LEFTRELEASE)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_LEFTRELEASE_NM TERMCAP2_ਕੁੰਜੀ(KS_EXTRA, KE_LEFTRELEASE_NM)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_MOUSEMOVE	TERMCAP2_ਕੁੰਜੀ(KS_EXTRA, KE_MOUSEMOVE)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_MIDDLEMOUSE	TERMCAP2_ਕੁੰਜੀ(KS_EXTRA, KE_MIDDLEMOUSE)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_MIDDLEDRAG	TERMCAP2_ਕੁੰਜੀ(KS_EXTRA, KE_MIDDLEDRAG)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_MIDDLERELEASE	TERMCAP2_ਕੁੰਜੀ(KS_EXTRA, KE_MIDDLERELEASE)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_RIGHTMOUSE	TERMCAP2_ਕੁੰਜੀ(KS_EXTRA, KE_RIGHTMOUSE)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_RIGHTDRAG	TERMCAP2_ਕੁੰਜੀ(KS_EXTRA, KE_RIGHTDRAG)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_RIGHTRELEASE	TERMCAP2_ਕੁੰਜੀ(KS_EXTRA, KE_RIGHTRELEASE)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_X1MOUSE	TERMCAP2_ਕੁੰਜੀ(KS_EXTRA, KE_X1MOUSE)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_X1MOUSE	TERMCAP2_ਕੁੰਜੀ(KS_EXTRA, KE_X1MOUSE)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_X1DRAG	TERMCAP2_ਕੁੰਜੀ(KS_EXTRA, KE_X1DRAG)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_X1RELEASE     TERMCAP2_ਕੁੰਜੀ(KS_EXTRA, KE_X1RELEASE)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_X2MOUSE	TERMCAP2_ਕੁੰਜੀ(KS_EXTRA, KE_X2MOUSE)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_X2DRAG	TERMCAP2_ਕੁੰਜੀ(KS_EXTRA, KE_X2DRAG)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_X2RELEASE     TERMCAP2_ਕੁੰਜੀ(KS_EXTRA, KE_X2RELEASE)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_IGNORE	TERMCAP2_ਕੁੰਜੀ(KS_EXTRA, KE_IGNORE)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_NOP		TERMCAP2_ਕੁੰਜੀ(KS_EXTRA, KE_NOP)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_CANCEL	TERMCAP2_ਕੁੰਜੀ(KS_EXTRA, KE_CANCEL)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_MOUSEDOWN	TERMCAP2_ਕੁੰਜੀ(KS_EXTRA, KE_MOUSEDOWN)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_MOUSEUP	TERMCAP2_ਕੁੰਜੀ(KS_EXTRA, KE_MOUSEUP)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_MOUSELEFT	TERMCAP2_ਕੁੰਜੀ(KS_EXTRA, KE_MOUSELEFT)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_MOUSERIGHT	TERMCAP2_ਕੁੰਜੀ(KS_EXTRA, KE_MOUSERIGHT)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_CSI		TERMCAP2_ਕੁੰਜੀ(KS_EXTRA, KE_CSI)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_OSC		TERMCAP2_ਕੁੰਜੀ(KS_EXTRA, KE_OSC)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_SNR		TERMCAP2_ਕੁੰਜੀ(KS_EXTRA, KE_SNR)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_PLUG		TERMCAP2_ਕੁੰਜੀ(KS_EXTRA, KE_PLUG)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_CMDWIN	TERMCAP2_ਕੁੰਜੀ(KS_EXTRA, KE_CMDWIN)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_DROP		TERMCAP2_ਕੁੰਜੀ(KS_EXTRA, KE_DROP)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_FOCUSGAINED	TERMCAP2_ਕੁੰਜੀ(KS_EXTRA, KE_FOCUSGAINED)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_FOCUSLOST	TERMCAP2_ਕੁੰਜੀ(KS_EXTRA, KE_FOCUSLOST)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_CURSORHOLD	TERMCAP2_ਕੁੰਜੀ(KS_EXTRA, KE_CURSORHOLD)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_COMPLETE_DELAY TERMCAP2_ਕੁੰਜੀ(KS_EXTRA, KE_COMPLETE_DELAY)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_COMMAND	TERMCAP2_ਕੁੰਜੀ(KS_EXTRA, KE_COMMAND)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_SCRIPT_COMMAND TERMCAP2_ਕੁੰਜੀ(KS_EXTRA, KE_SCRIPT_COMMAND)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_SID		TERMCAP2_ਕੁੰਜੀ(KS_EXTRA, KE_SID)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_WILD		TERMCAP2_ਕੁੰਜੀ(KS_EXTRA, KE_WILD)

// Bits for modifier mask
// 0x01 cannot be used, because the modifier must be 0x02 or higher
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MOD_ਨਕਾਬ_ਖਿਸਕਾਓ	    0x02
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MOD_ਨਕਾਬ_CTRL	    0x04
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MOD_ਨਕਾਬ_ALT	    0x08	// aka META
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MOD_ਨਕਾਬ_META	    0x10	// META when it's different from ALT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MOD_ਨਕਾਬ_2_CLICK	    0x20	// use MOD_MASK_MULTI_CLICK
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MOD_ਨਕਾਬ_3_CLICK	    0x40	// use MOD_MASK_MULTI_CLICK
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MOD_ਨਕਾਬ_4_CLICK	    0x60	// use MOD_MASK_MULTI_CLICK
#ਜੇ defined(MACOS_X) || defined(FEAT_GUI_GTK)
# define MOD_ਨਕਾਬ_CMD	    0x80        // aka SUPER
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MOD_ਨਕਾਬ_MULTI_CLICK	(MOD_ਨਕਾਬ_2_CLICK|MOD_ਨਕਾਬ_3_CLICK|MOD_ਨਕਾਬ_4_CLICK)

/*
 * The length of the longest special key name, including modifiers.
 * Current longest is <M-C-S-T-D-A-4-ScrollWheelRight> (length includes '<' and
 * '>').
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_ਕੁੰਜੀ_NAME_LEN    32

/*
 * Maximum length of a special key event as tokens.  This includes modifiers.
 * The longest event is something like <M-C-S-T-4-LeftDrag> which would be the
 * following string of tokens:
 *
 * <K_SPECIAL> <KS_MODIFIER> bitmask <K_SPECIAL> <KS_EXTRA> <KT_LEFTDRAG>.
 *
 * This is a total of 6 tokens, and is currently the longest one possible.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_ਕੁੰਜੀ_ਕੋਡ_LEN    6
