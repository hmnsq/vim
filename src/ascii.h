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

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CharOrd(x)	((x) < 'a' ? (x) - 'A' : (x) - 'a')
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Char_Ord_ਨੀਵਾਂ(x)	((x) - 'a')
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Char_Ord_ਉੱਪਰ(x)	((x) - 'A')
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ROT13(c, a)	(((((c) - (a)) + 13) % 26) + (a))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NUL		'\000'
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BELL		'\007'
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BS		'\010'
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TAB		'\011'
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NL		'\012'
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NL_STR		(char_u *)"\012"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FF		'\014'
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CAR		'\015'	// CR is used by Mac OS X
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ESC		'\033'
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ESC_STR		(char_u *)"\033"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ESC_STR_nc	"\033"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEL		0x7f
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEL_STR		(char_u *)"\177"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ POUND		0xA3

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Ctrl_chr(x)	(TOUPPER_ASC(x) ^ 0x40) // '?' -> DEL, '@' -> ^@, etc.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Meta(x)		((x) | 0x80)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CTRL_F_STR	"\006"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CTRL_H_STR	"\010"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CTRL_V_STR	"\026"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Ctrl_AT		0   // @
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Ctrl_A		1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Ctrl_B		2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Ctrl_C		3
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Ctrl_D		4
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Ctrl_E		5
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Ctrl_F		6
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Ctrl_G		7
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Ctrl_H		8
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Ctrl_I		9
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Ctrl_J		10
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Ctrl_K		11
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Ctrl_L		12
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Ctrl_M		13
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Ctrl_N		14
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Ctrl_O		15
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Ctrl_P		16
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Ctrl_Q		17
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Ctrl_R		18
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Ctrl_S		19
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Ctrl_T		20
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Ctrl_U		21
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Ctrl_V		22
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Ctrl_W		23
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Ctrl_X		24
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Ctrl_Y		25
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Ctrl_Z		26
			    // CTRL- [ Left Square Bracket == ESC
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Ctrl_BSL	28  // \ BackSLash
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Ctrl_RSB	29  // ] Right Square Bracket
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Ctrl_HAT	30  // ^
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Ctrl__		31

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CSI		0x9b	// Control Sequence Introducer
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CSI_STR		"\233"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DCS		0x90	// Device Control String
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OSC		0x9d	// Operating System Command
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STERM		0x9c	// String Terminator

/*
 * Character that separates dir names in a path.
 * For MS-DOS, WIN32 and OS/2 we use a backslash.  A slash mostly works
 * fine, but there are places where it doesn't (e.g. in a command name).
 * For Acorn we use a dot.
 */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ BACKSLASH_ਵਿੱਚ_FILENAME
# define PATHSEP	psepc
# define PATHSEPSTR	pseps
#ਨਹੀਂਤਾਂ
# define PATHSEP	((char_u)'/')
# define PATHSEPSTR	"/"
#ਜੇ_ਅੰਤ
