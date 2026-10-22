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

#నిర్వచించు CharOrd(x)	((x) < 'a' ? (x) - 'A' : (x) - 'a')
#నిర్వచించు Char_Ord_దిగువ(x)	((x) - 'a')
#నిర్వచించు Char_Ord_పైకి(x)	((x) - 'A')
#నిర్వచించు ROT13(c, a)	(((((c) - (a)) + 13) % 26) + (a))

#నిర్వచించు NUL		'\000'
#నిర్వచించు BELL		'\007'
#నిర్వచించు BS		'\010'
#నిర్వచించు TAB		'\011'
#నిర్వచించు NL		'\012'
#నిర్వచించు NL_STR		(char_u *)"\012"
#నిర్వచించు FF		'\014'
#నిర్వచించు CAR		'\015'	// CR is used by Mac OS X
#నిర్వచించు ESC		'\033'
#నిర్వచించు ESC_STR		(char_u *)"\033"
#నిర్వచించు ESC_STR_nc	"\033"
#నిర్వచించు DEL		0x7f
#నిర్వచించు DEL_STR		(char_u *)"\177"

#నిర్వచించు POUND		0xA3

#నిర్వచించు Ctrl_chr(x)	(TOUPPER_ASC(x) ^ 0x40) // '?' -> DEL, '@' -> ^@, etc.
#నిర్వచించు Meta(x)		((x) | 0x80)

#నిర్వచించు CTRL_F_STR	"\006"
#నిర్వచించు CTRL_H_STR	"\010"
#నిర్వచించు CTRL_V_STR	"\026"

#నిర్వచించు Ctrl_AT		0   // @
#నిర్వచించు Ctrl_A		1
#నిర్వచించు Ctrl_B		2
#నిర్వచించు Ctrl_C		3
#నిర్వచించు Ctrl_D		4
#నిర్వచించు Ctrl_E		5
#నిర్వచించు Ctrl_F		6
#నిర్వచించు Ctrl_G		7
#నిర్వచించు Ctrl_H		8
#నిర్వచించు Ctrl_I		9
#నిర్వచించు Ctrl_J		10
#నిర్వచించు Ctrl_K		11
#నిర్వచించు Ctrl_L		12
#నిర్వచించు Ctrl_M		13
#నిర్వచించు Ctrl_N		14
#నిర్వచించు Ctrl_O		15
#నిర్వచించు Ctrl_P		16
#నిర్వచించు Ctrl_Q		17
#నిర్వచించు Ctrl_R		18
#నిర్వచించు Ctrl_S		19
#నిర్వచించు Ctrl_T		20
#నిర్వచించు Ctrl_U		21
#నిర్వచించు Ctrl_V		22
#నిర్వచించు Ctrl_W		23
#నిర్వచించు Ctrl_X		24
#నిర్వచించు Ctrl_Y		25
#నిర్వచించు Ctrl_Z		26
			    // CTRL- [ Left Square Bracket == ESC
#నిర్వచించు Ctrl_BSL	28  // \ BackSLash
#నిర్వచించు Ctrl_RSB	29  // ] Right Square Bracket
#నిర్వచించు Ctrl_HAT	30  // ^
#నిర్వచించు Ctrl__		31

#నిర్వచించు CSI		0x9b	// Control Sequence Introducer
#నిర్వచించు CSI_STR		"\233"
#నిర్వచించు DCS		0x90	// Device Control String
#నిర్వచించు OSC		0x9d	// Operating System Command
#నిర్వచించు STERM		0x9c	// String Terminator

/*
 * Character that separates dir names in a path.
 * For MS-DOS, WIN32 and OS/2 we use a backslash.  A slash mostly works
 * fine, but there are places where it doesn't (e.g. in a command name).
 * For Acorn we use a dot.
 */
#నిర్వచితమైతే BACKSLASH_లోపల_FILENAME
# define PATHSEP	psepc
# define PATHSEPSTR	pseps
#లేకపోతే
# define PATHSEP	((char_u)'/')
# define PATHSEPSTR	"/"
#అయితే_ముగింపు
