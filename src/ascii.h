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

#ವ್ಯಾಖ್ಯಾನಿಸು CharOrd(x)	((x) < 'a' ? (x) - 'A' : (x) - 'a')
#ವ್ಯಾಖ್ಯಾನಿಸು Char_Ord_ಕೆಳಗಿನ(x)	((x) - 'a')
#ವ್ಯಾಖ್ಯಾನಿಸು Char_Ord_ಮೇಲೆ(x)	((x) - 'A')
#ವ್ಯಾಖ್ಯಾನಿಸು ROT13(c, a)	(((((c) - (a)) + 13) % 26) + (a))

#ವ್ಯಾಖ್ಯಾನಿಸು NUL		'\000'
#ವ್ಯಾಖ್ಯಾನಿಸು BELL		'\007'
#ವ್ಯಾಖ್ಯಾನಿಸು BS		'\010'
#ವ್ಯಾಖ್ಯಾನಿಸು TAB		'\011'
#ವ್ಯಾಖ್ಯಾನಿಸು NL		'\012'
#ವ್ಯಾಖ್ಯಾನಿಸು NL_STR		(char_u *)"\012"
#ವ್ಯಾಖ್ಯಾನಿಸು FF		'\014'
#ವ್ಯಾಖ್ಯಾನಿಸು CAR		'\015'	// CR is used by Mac OS X
#ವ್ಯಾಖ್ಯಾನಿಸು ESC		'\033'
#ವ್ಯಾಖ್ಯಾನಿಸು ESC_STR		(char_u *)"\033"
#ವ್ಯಾಖ್ಯಾನಿಸು ESC_STR_nc	"\033"
#ವ್ಯಾಖ್ಯಾನಿಸು DEL		0x7f
#ವ್ಯಾಖ್ಯಾನಿಸು DEL_STR		(char_u *)"\177"

#ವ್ಯಾಖ್ಯಾನಿಸು POUND		0xA3

#ವ್ಯಾಖ್ಯಾನಿಸು Ctrl_chr(x)	(TOUPPER_ASC(x) ^ 0x40) // '?' -> DEL, '@' -> ^@, etc.
#ವ್ಯಾಖ್ಯಾನಿಸು Meta(x)		((x) | 0x80)

#ವ್ಯಾಖ್ಯಾನಿಸು CTRL_F_STR	"\006"
#ವ್ಯಾಖ್ಯಾನಿಸು CTRL_H_STR	"\010"
#ವ್ಯಾಖ್ಯಾನಿಸು CTRL_V_STR	"\026"

#ವ್ಯಾಖ್ಯಾನಿಸು Ctrl_AT		0   // @
#ವ್ಯಾಖ್ಯಾನಿಸು Ctrl_A		1
#ವ್ಯಾಖ್ಯಾನಿಸು Ctrl_B		2
#ವ್ಯಾಖ್ಯಾನಿಸು Ctrl_C		3
#ವ್ಯಾಖ್ಯಾನಿಸು Ctrl_D		4
#ವ್ಯಾಖ್ಯಾನಿಸು Ctrl_E		5
#ವ್ಯಾಖ್ಯಾನಿಸು Ctrl_F		6
#ವ್ಯಾಖ್ಯಾನಿಸು Ctrl_G		7
#ವ್ಯಾಖ್ಯಾನಿಸು Ctrl_H		8
#ವ್ಯಾಖ್ಯಾನಿಸು Ctrl_I		9
#ವ್ಯಾಖ್ಯಾನಿಸು Ctrl_J		10
#ವ್ಯಾಖ್ಯಾನಿಸು Ctrl_K		11
#ವ್ಯಾಖ್ಯಾನಿಸು Ctrl_L		12
#ವ್ಯಾಖ್ಯಾನಿಸು Ctrl_M		13
#ವ್ಯಾಖ್ಯಾನಿಸು Ctrl_N		14
#ವ್ಯಾಖ್ಯಾನಿಸು Ctrl_O		15
#ವ್ಯಾಖ್ಯಾನಿಸು Ctrl_P		16
#ವ್ಯಾಖ್ಯಾನಿಸು Ctrl_Q		17
#ವ್ಯಾಖ್ಯಾನಿಸು Ctrl_R		18
#ವ್ಯಾಖ್ಯಾನಿಸು Ctrl_S		19
#ವ್ಯಾಖ್ಯಾನಿಸು Ctrl_T		20
#ವ್ಯಾಖ್ಯಾನಿಸು Ctrl_U		21
#ವ್ಯಾಖ್ಯಾನಿಸು Ctrl_V		22
#ವ್ಯಾಖ್ಯಾನಿಸು Ctrl_W		23
#ವ್ಯಾಖ್ಯಾನಿಸು Ctrl_X		24
#ವ್ಯಾಖ್ಯಾನಿಸು Ctrl_Y		25
#ವ್ಯಾಖ್ಯಾನಿಸು Ctrl_Z		26
			    // CTRL- [ Left Square Bracket == ESC
#ವ್ಯಾಖ್ಯಾನಿಸು Ctrl_BSL	28  // \ BackSLash
#ವ್ಯಾಖ್ಯಾನಿಸು Ctrl_RSB	29  // ] Right Square Bracket
#ವ್ಯಾಖ್ಯಾನಿಸು Ctrl_HAT	30  // ^
#ವ್ಯಾಖ್ಯಾನಿಸು Ctrl__		31

#ವ್ಯಾಖ್ಯಾನಿಸು CSI		0x9b	// Control Sequence Introducer
#ವ್ಯಾಖ್ಯಾನಿಸು CSI_STR		"\233"
#ವ್ಯಾಖ್ಯಾನಿಸು DCS		0x90	// Device Control String
#ವ್ಯಾಖ್ಯಾನಿಸು OSC		0x9d	// Operating System Command
#ವ್ಯಾಖ್ಯಾನಿಸು STERM		0x9c	// String Terminator

/*
 * Character that separates dir names in a path.
 * For MS-DOS, WIN32 and OS/2 we use a backslash.  A slash mostly works
 * fine, but there are places where it doesn't (e.g. in a command name).
 * For Acorn we use a dot.
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ BACKSLASH_ಒಳ_FILENAME
# define PATHSEP	psepc
# define PATHSEPSTR	pseps
#ಇಲ್ಲದಿದ್ದರೆ
# define PATHSEP	((char_u)'/')
# define PATHSEPSTR	"/"
#ಆದರೆ_ಅಂತ್ಯ
