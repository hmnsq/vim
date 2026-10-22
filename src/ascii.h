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

#تعریف_کرو CharOrd(x)	((x) < 'a' ? (x) - 'A' : (x) - 'a')
#تعریف_کرو Char_Ord_کم(x)	((x) - 'a')
#تعریف_کرو Char_Ord_اتے(x)	((x) - 'A')
#تعریف_کرو ROT13(c, a)	(((((c) - (a)) + 13) % 26) + (a))

#تعریف_کرو NUL		'\000'
#تعریف_کرو BELL		'\007'
#تعریف_کرو BS		'\010'
#تعریف_کرو TAB		'\011'
#تعریف_کرو NL		'\012'
#تعریف_کرو NL_STR		(char_u *)"\012"
#تعریف_کرو FF		'\014'
#تعریف_کرو CAR		'\015'	// CR is used by Mac OS X
#تعریف_کرو ESC		'\033'
#تعریف_کرو ESC_STR		(char_u *)"\033"
#تعریف_کرو ESC_STR_nc	"\033"
#تعریف_کرو DEL		0x7f
#تعریف_کرو DEL_STR		(char_u *)"\177"

#تعریف_کرو POUND		0xA3

#تعریف_کرو Ctrl_chr(x)	(TOUPPER_ASC(x) ^ 0x40) // '?' -> DEL, '@' -> ^@, etc.
#تعریف_کرو Meta(x)		((x) | 0x80)

#تعریف_کرو CTRL_F_STR	"\006"
#تعریف_کرو CTRL_H_STR	"\010"
#تعریف_کرو CTRL_V_STR	"\026"

#تعریف_کرو Ctrl_AT		0   // @
#تعریف_کرو Ctrl_A		1
#تعریف_کرو Ctrl_B		2
#تعریف_کرو Ctrl_C		3
#تعریف_کرو Ctrl_D		4
#تعریف_کرو Ctrl_E		5
#تعریف_کرو Ctrl_F		6
#تعریف_کرو Ctrl_G		7
#تعریف_کرو Ctrl_H		8
#تعریف_کرو Ctrl_I		9
#تعریف_کرو Ctrl_J		10
#تعریف_کرو Ctrl_K		11
#تعریف_کرو Ctrl_L		12
#تعریف_کرو Ctrl_M		13
#تعریف_کرو Ctrl_N		14
#تعریف_کرو Ctrl_O		15
#تعریف_کرو Ctrl_P		16
#تعریف_کرو Ctrl_Q		17
#تعریف_کرو Ctrl_R		18
#تعریف_کرو Ctrl_S		19
#تعریف_کرو Ctrl_T		20
#تعریف_کرو Ctrl_U		21
#تعریف_کرو Ctrl_V		22
#تعریف_کرو Ctrl_W		23
#تعریف_کرو Ctrl_X		24
#تعریف_کرو Ctrl_Y		25
#تعریف_کرو Ctrl_Z		26
			    // CTRL- [ Left Square Bracket == ESC
#تعریف_کرو Ctrl_BSL	28  // \ BackSLash
#تعریف_کرو Ctrl_RSB	29  // ] Right Square Bracket
#تعریف_کرو Ctrl_HAT	30  // ^
#تعریف_کرو Ctrl__		31

#تعریف_کرو CSI		0x9b	// Control Sequence Introducer
#تعریف_کرو CSI_STR		"\233"
#تعریف_کرو DCS		0x90	// Device Control String
#تعریف_کرو OSC		0x9d	// Operating System Command
#تعریف_کرو STERM		0x9c	// String Terminator

/*
 * Character that separates dir names in a path.
 * For MS-DOS, WIN32 and OS/2 we use a backslash.  A slash mostly works
 * fine, but there are places where it doesn't (e.g. in a command name).
 * For Acorn we use a dot.
 */
#جے_تعریف_شدہ BACKSLASH_وچ_FILENAME
# define PATHSEP	psepc
# define PATHSEPSTR	pseps
#وإلا
# define PATHSEP	((char_u)'/')
# define PATHSEPSTR	"/"
#جے_انت
