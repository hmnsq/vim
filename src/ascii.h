/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

/*
 * Definitions of various common control characters.
 */

#تعریف_کن CharOrd(x)	((x) < 'a' ? (x) - 'A' : (x) - 'a')
#تعریف_کن Char_Ord_کم(x)	((x) - 'a')
#تعریف_کن Char_Ord_به_بالا(x)	((x) - 'A')
#تعریف_کن ROT13(c, a)	(((((c) - (a)) + 13) % 26) + (a))

#تعریف_کن NUL		'\000'
#تعریف_کن BELL		'\007'
#تعریف_کن BS		'\010'
#تعریف_کن TAB		'\011'
#تعریف_کن NL		'\012'
#تعریف_کن NL_STR		(char_u *)"\012"
#تعریف_کن FF		'\014'
#تعریف_کن CAR		'\015'	// CR is used by Mac OS X
#تعریف_کن ESC		'\033'
#تعریف_کن ESC_STR		(char_u *)"\033"
#تعریف_کن ESC_STR_nc	"\033"
#تعریف_کن DEL		0x7f
#تعریف_کن DEL_STR		(char_u *)"\177"

#تعریف_کن POUND		0xA3

#تعریف_کن Ctrl_chr(x)	(TOUPPER_ASC(x) ^ 0x40) // '?' -> DEL, '@' -> ^@, etc.
#تعریف_کن Meta(x)		((x) | 0x80)

#تعریف_کن CTRL_F_STR	"\006"
#تعریف_کن CTRL_H_STR	"\010"
#تعریف_کن CTRL_V_STR	"\026"

#تعریف_کن Ctrl_AT		0   // @
#تعریف_کن Ctrl_A		1
#تعریف_کن Ctrl_B		2
#تعریف_کن Ctrl_C		3
#تعریف_کن Ctrl_D		4
#تعریف_کن Ctrl_E		5
#تعریف_کن Ctrl_F		6
#تعریف_کن Ctrl_G		7
#تعریف_کن Ctrl_H		8
#تعریف_کن Ctrl_I		9
#تعریف_کن Ctrl_J		10
#تعریف_کن Ctrl_K		11
#تعریف_کن Ctrl_L		12
#تعریف_کن Ctrl_M		13
#تعریف_کن Ctrl_N		14
#تعریف_کن Ctrl_O		15
#تعریف_کن Ctrl_P		16
#تعریف_کن Ctrl_Q		17
#تعریف_کن Ctrl_R		18
#تعریف_کن Ctrl_S		19
#تعریف_کن Ctrl_T		20
#تعریف_کن Ctrl_U		21
#تعریف_کن Ctrl_V		22
#تعریف_کن Ctrl_W		23
#تعریف_کن Ctrl_X		24
#تعریف_کن Ctrl_Y		25
#تعریف_کن Ctrl_Z		26
			    // CTRL- [ Left Square Bracket == ESC
#تعریف_کن Ctrl_BSL	28  // \ BackSLash
#تعریف_کن Ctrl_RSB	29  // ] Right Square Bracket
#تعریف_کن Ctrl_HAT	30  // ^
#تعریف_کن Ctrl__		31

#تعریف_کن CSI		0x9b	// Control Sequence Introducer
#تعریف_کن CSI_STR		"\233"
#تعریف_کن DCS		0x90	// Device Control String
#تعریف_کن OSC		0x9d	// Operating System Command
#تعریف_کن STERM		0x9c	// String Terminator

/*
 * Character that separates dir names in a path.
 * For MS-DOS, WIN32 and OS/2 we use a backslash.  A slash mostly works
 * fine, but there are places where it doesn't (e.g. in a command name).
 * For Acorn we use a dot.
 */
#اگر_تعریف_شده BACKSLASH_درون_FILENAME
# define PATHSEP	psepc
# define PATHSEPSTR	pseps
#وإلا
# define PATHSEP	((char_u)'/')
# define PATHSEPSTR	"/"
#پایان_اگر
