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

#تعریف_کړه CharOrd(x)	((x) < 'a' ? (x) - 'A' : (x) - 'a')
#تعریف_کړه Char_Ord_ټیټ(x)	((x) - 'a')
#تعریف_کړه Char_Ord_پورته(x)	((x) - 'A')
#تعریف_کړه ROT13(c, a)	(((((c) - (a)) + 13) % 26) + (a))

#تعریف_کړه NUL		'\000'
#تعریف_کړه BELL		'\007'
#تعریف_کړه BS		'\010'
#تعریف_کړه TAB		'\011'
#تعریف_کړه NL		'\012'
#تعریف_کړه NL_STR		(char_u *)"\012"
#تعریف_کړه FF		'\014'
#تعریف_کړه CAR		'\015'	// CR is used by Mac OS X
#تعریف_کړه ESC		'\033'
#تعریف_کړه ESC_STR		(char_u *)"\033"
#تعریف_کړه ESC_STR_nc	"\033"
#تعریف_کړه DEL		0x7f
#تعریف_کړه DEL_STR		(char_u *)"\177"

#تعریف_کړه POUND		0xA3

#تعریف_کړه Ctrl_chr(x)	(TOUPPER_ASC(x) ^ 0x40) // '?' -> DEL, '@' -> ^@, etc.
#تعریف_کړه Meta(x)		((x) | 0x80)

#تعریف_کړه CTRL_F_STR	"\006"
#تعریف_کړه CTRL_H_STR	"\010"
#تعریف_کړه CTRL_V_STR	"\026"

#تعریف_کړه Ctrl_AT		0   // @
#تعریف_کړه Ctrl_A		1
#تعریف_کړه Ctrl_B		2
#تعریف_کړه Ctrl_C		3
#تعریف_کړه Ctrl_D		4
#تعریف_کړه Ctrl_E		5
#تعریف_کړه Ctrl_F		6
#تعریف_کړه Ctrl_G		7
#تعریف_کړه Ctrl_H		8
#تعریف_کړه Ctrl_I		9
#تعریف_کړه Ctrl_J		10
#تعریف_کړه Ctrl_K		11
#تعریف_کړه Ctrl_L		12
#تعریف_کړه Ctrl_M		13
#تعریف_کړه Ctrl_N		14
#تعریف_کړه Ctrl_O		15
#تعریف_کړه Ctrl_P		16
#تعریف_کړه Ctrl_Q		17
#تعریف_کړه Ctrl_R		18
#تعریف_کړه Ctrl_S		19
#تعریف_کړه Ctrl_T		20
#تعریف_کړه Ctrl_U		21
#تعریف_کړه Ctrl_V		22
#تعریف_کړه Ctrl_W		23
#تعریف_کړه Ctrl_X		24
#تعریف_کړه Ctrl_Y		25
#تعریف_کړه Ctrl_Z		26
			    // CTRL- [ Left Square Bracket == ESC
#تعریف_کړه Ctrl_BSL	28  // \ BackSLash
#تعریف_کړه Ctrl_RSB	29  // ] Right Square Bracket
#تعریف_کړه Ctrl_HAT	30  // ^
#تعریف_کړه Ctrl__		31

#تعریف_کړه CSI		0x9b	// Control Sequence Introducer
#تعریف_کړه CSI_STR		"\233"
#تعریف_کړه DCS		0x90	// Device Control String
#تعریف_کړه OSC		0x9d	// Operating System Command
#تعریف_کړه STERM		0x9c	// String Terminator

/*
 * Character that separates dir names in a path.
 * For MS-DOS, WIN32 and OS/2 we use a backslash.  A slash mostly works
 * fine, but there are places where it doesn't (e.g. in a command name).
 * For Acorn we use a dot.
 */
#که_تعریف_شوی BACKSLASH_دننه_FILENAME
# define PATHSEP	psepc
# define PATHSEPSTR	pseps
#وإلا
# define PATHSEP	((char_u)'/')
# define PATHSEPSTR	"/"
#د_که_پای
