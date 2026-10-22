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

#وصف_ڪريو CharOrd(x)	((x) < 'a' ? (x) - 'A' : (x) - 'a')
#وصف_ڪريو Char_Ord_هيٺيون(x)	((x) - 'a')
#وصف_ڪريو Char_Ord_مٿي(x)	((x) - 'A')
#وصف_ڪريو ROT13(c, a)	(((((c) - (a)) + 13) % 26) + (a))

#وصف_ڪريو NUL		'\000'
#وصف_ڪريو BELL		'\007'
#وصف_ڪريو BS		'\010'
#وصف_ڪريو TAB		'\011'
#وصف_ڪريو NL		'\012'
#وصف_ڪريو NL_STR		(char_u *)"\012"
#وصف_ڪريو FF		'\014'
#وصف_ڪريو CAR		'\015'	// CR is used by Mac OS X
#وصف_ڪريو ESC		'\033'
#وصف_ڪريو ESC_STR		(char_u *)"\033"
#وصف_ڪريو ESC_STR_nc	"\033"
#وصف_ڪريو DEL		0x7f
#وصف_ڪريو DEL_STR		(char_u *)"\177"

#وصف_ڪريو POUND		0xA3

#وصف_ڪريو Ctrl_chr(x)	(TOUPPER_ASC(x) ^ 0x40) // '?' -> DEL, '@' -> ^@, etc.
#وصف_ڪريو Meta(x)		((x) | 0x80)

#وصف_ڪريو CTRL_F_STR	"\006"
#وصف_ڪريو CTRL_H_STR	"\010"
#وصف_ڪريو CTRL_V_STR	"\026"

#وصف_ڪريو Ctrl_AT		0   // @
#وصف_ڪريو Ctrl_A		1
#وصف_ڪريو Ctrl_B		2
#وصف_ڪريو Ctrl_C		3
#وصف_ڪريو Ctrl_D		4
#وصف_ڪريو Ctrl_E		5
#وصف_ڪريو Ctrl_F		6
#وصف_ڪريو Ctrl_G		7
#وصف_ڪريو Ctrl_H		8
#وصف_ڪريو Ctrl_I		9
#وصف_ڪريو Ctrl_J		10
#وصف_ڪريو Ctrl_K		11
#وصف_ڪريو Ctrl_L		12
#وصف_ڪريو Ctrl_M		13
#وصف_ڪريو Ctrl_N		14
#وصف_ڪريو Ctrl_O		15
#وصف_ڪريو Ctrl_P		16
#وصف_ڪريو Ctrl_Q		17
#وصف_ڪريو Ctrl_R		18
#وصف_ڪريو Ctrl_S		19
#وصف_ڪريو Ctrl_T		20
#وصف_ڪريو Ctrl_U		21
#وصف_ڪريو Ctrl_V		22
#وصف_ڪريو Ctrl_W		23
#وصف_ڪريو Ctrl_X		24
#وصف_ڪريو Ctrl_Y		25
#وصف_ڪريو Ctrl_Z		26
			    // CTRL- [ Left Square Bracket == ESC
#وصف_ڪريو Ctrl_BSL	28  // \ BackSLash
#وصف_ڪريو Ctrl_RSB	29  // ] Right Square Bracket
#وصف_ڪريو Ctrl_HAT	30  // ^
#وصف_ڪريو Ctrl__		31

#وصف_ڪريو CSI		0x9b	// Control Sequence Introducer
#وصف_ڪريو CSI_STR		"\233"
#وصف_ڪريو DCS		0x90	// Device Control String
#وصف_ڪريو OSC		0x9d	// Operating System Command
#وصف_ڪريو STERM		0x9c	// String Terminator

/*
 * Character that separates dir names in a path.
 * For MS-DOS, WIN32 and OS/2 we use a backslash.  A slash mostly works
 * fine, but there are places where it doesn't (e.g. in a command name).
 * For Acorn we use a dot.
 */
#جيڪڏهن_وصف_ٿيل BACKSLASH_اندريون_FILENAME
# define PATHSEP	psepc
# define PATHSEPSTR	pseps
#وإلا
# define PATHSEP	((char_u)'/')
# define PATHSEPSTR	"/"
#جيڪڏهن_پورو
