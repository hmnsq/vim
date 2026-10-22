/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

/*
 * option.h: definition of global variables for settable options
 */

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ _OPTION_H_
#ವ್ಯಾಖ್ಯಾನಿಸು _OPTION_H_

//
// Option Flags
//
#ವ್ಯಾಖ್ಯಾನಿಸು P_BOOL		0x01	// the option is boolean
#ವ್ಯಾಖ್ಯಾನಿಸು P_ಅಂಕೆ		0x02	// the option is numeric
#ವ್ಯಾಖ್ಯಾನಿಸು P_STRING	0x04	// the option is a string
#ವ್ಯಾಖ್ಯಾನಿಸು P_ALLOCED	0x08	// the string option is in allocated memory,
				// must use free_string_option() when
				// assigning new value. Not set if default is
				// the same.
#ವ್ಯಾಖ್ಯಾನಿಸು P_EXPAND	0x10	// environment expansion.  NOTE: P_EXPAND can
				// never be used for local or hidden options!
#ವ್ಯಾಖ್ಯಾನಿಸು P_NO_CMD_EXPAND	0x20	// don't perform cmdline completions
#ವ್ಯಾಖ್ಯಾನಿಸು P_NODEFAULT	0x40	// don't set to default value
#ವ್ಯಾಖ್ಯಾನಿಸು P_DEF_ALLOCED	0x80	// default value is in allocated memory, must
				//  use vim_free() when assigning new value
#ವ್ಯಾಖ್ಯಾನಿಸು P_WAS_ಗಣ	0x100	// option has been set/reset
#ವ್ಯಾಖ್ಯಾನಿಸು P_NO_MKRC	0x200	// don't include in :mkvimrc output
#ವ್ಯಾಖ್ಯಾನಿಸು P_VI_DEF	0x400	// Use Vi default for Vim
#ವ್ಯಾಖ್ಯಾನಿಸು P_VIM		0x800	// Vim option, reset when 'cp' set

				// when option changed, what to display:
#ವ್ಯಾಖ್ಯಾನಿಸು P_RSTAT		0x1000	// redraw status lines
#ವ್ಯಾಖ್ಯಾನಿಸು P_RWIN		0x2000	// redraw current window and recompute text
#ವ್ಯಾಖ್ಯಾನಿಸು P_RBUF		0x4000	// redraw current buffer and recompute text
#ವ್ಯಾಖ್ಯಾನಿಸು P_RALL		0x6000	// redraw all windows and recompute text
#ವ್ಯಾಖ್ಯಾನಿಸು P_RCLR		0x7000	// clear and redraw all and recompute text

#ವ್ಯಾಖ್ಯಾನಿಸು P_COMMA		 0x8000	 // comma separated list
#ವ್ಯಾಖ್ಯಾನಿಸು P_ONECOMMA	0x18000L // P_COMMA and cannot have two consecutive
				 // commas
#ವ್ಯಾಖ್ಯಾನಿಸು P_NODUP		0x20000L // don't allow duplicate strings
#ವ್ಯಾಖ್ಯಾನಿಸು P_FLAGLIST	0x40000L // list of single-char flags

#ವ್ಯಾಖ್ಯಾನಿಸು P_SECURE	0x80000L // cannot change in modeline or secure mode
#ವ್ಯಾಖ್ಯಾನಿಸು P_GETTEXT      0x100000L // expand default value with _()
#ವ್ಯಾಖ್ಯಾನಿಸು P_NOGLOB       0x200000L // do not use local value for global vimrc
#ವ್ಯಾಖ್ಯಾನಿಸು P_NFNAME       0x400000L // only normal file name chars allowed
#ವ್ಯಾಖ್ಯಾನಿಸು P_INSECURE     0x800000L // option was set from a modeline
#ವ್ಯಾಖ್ಯಾನಿಸು P_PRI_MKRC    0x1000000L // priority for :mkvimrc (setting option has
				 // side effects)
#ವ್ಯಾಖ್ಯಾನಿಸು P_NO_ML       0x2000000L // not allowed in modeline
#ವ್ಯಾಖ್ಯಾನಿಸು P_CURSWANT    0x4000000L // update curswant required; not needed when
				 // there is a redraw flag
#ವ್ಯಾಖ್ಯಾನಿಸು P_NDNAME      0x8000000L // only normal dir name chars allowed
#ವ್ಯಾಖ್ಯಾನಿಸು P_HLONLY     0x10000000L // option only changes highlight, not text
#ವ್ಯಾಖ್ಯಾನಿಸು P_MLE	     0x20000000L // under control of 'modelineexpr'
#ವ್ಯಾಖ್ಯಾನಿಸು P_FUNC	     0x40000000L // accept a function reference or a lambda
#ವ್ಯಾಖ್ಯಾನಿಸು P_COLON	     0x80000000L // values use colons to create sublists
// Warning: Currently we have used all 32 bits for option flags. On some 32-bit
//          systems, the flags are stored as a 32-bit integer, and adding more
//          flags will overflow it. Adding another flag will need to change how
//          it's stored first.

// Returned by get_option_value().
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
    gov_unknown,
    gov_bool,
    gov_ಸಂಖ್ಯೆ,
    gov_string,
    gov_hidden_bool,
    gov_hidden_ಸಂಖ್ಯೆ,
    gov_hidden_string
} getoption_T;

/*
 * Default values for 'errorformat'.
 * The "%f|%l| %m" one is used for when the contents of the quickfix window is
 * written to a file.
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ AMIGA
# define DFLT_EFM	"%f>%l:%c:%t:%n:%m,%f:%l: %t%*\\D%n: %m,%f %l %t%*\\D%n: %m,%*[^\"]\"%f\"%*\\D%l: %m,%f:%l:%m,%f|%l| %m"
#ಇಲ್ಲದಿದ್ದರೆ
# ಆದರೆ defined(MSWIN)
#  define DFLT_EFM	"%f(%l): %t%*\\D%n: %m,%f(%l\\,%c): %t%*\\D%n: %m,%f(%l) \\=: %t%*\\D%n: %m,%*[^\"]\"%f\"%*\\D%l: %m,%f(%l) \\=: %m,%*[^ ] %f %l: %m,%f:%l:%c:%m,%f(%l):%m,%f:%l:%m,%f|%l| %m"
# ಇಲ್ಲದಿದ್ದರೆ
#  ಆದರೆ defined(__QNX__)
#   define DFLT_EFM	"%f(%l):%*[^WE]%t%*\\D%n:%m,%f|%l| %m"
#  ಇಲ್ಲದಿದ್ದರೆ
#   ifdef VMS
#    define DFLT_EFM	"%A%p^,%C%%CC-%t-%m,%Cat line number %l in file %f,%f|%l| %m"
#   ಇಲ್ಲದಿದ್ದರೆ // Unix, probably
#    define DFLT_EFM	"%*[^\"]\"%f\"%*\\D%l: %m,\"%f\"%*\\D%l: %m,%-Gg%\\?make[%*\\d]: *** [%f:%l:%m,%-Gg%\\?make: *** [%f:%l:%m,%-G%f:%l: (Each undeclared identifier is reported only once,%-G%f:%l: for each function it appears in.),%-GIn file included from %f:%l:%c:,%-GIn file included from %f:%l:%c\\,,%-GIn file included from %f:%l:%c,%-GIn file included from %f:%l,%-G%*[ ]from %f:%l:%c,%-G%*[ ]from %f:%l:,%-G%*[ ]from %f:%l\\,,%-G%*[ ]from %f:%l,%f:%l:%c:%m,%f(%l):%m,%f:%l:%m,\"%f\"\\, line %l%*\\D%c%*[^ ] %m,%D%*\\a[%*\\d]: Entering directory %*[`']%f',%X%*\\a[%*\\d]: Leaving directory %*[`']%f',%D%*\\a: Entering directory %*[`']%f',%X%*\\a: Leaving directory %*[`']%f',%DMaking %*\\a in %f,%f|%l| %m"
#   endif
#  endif
# endif
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು DFLT_GREPFORMAT	"%f:%l:%m,%f:%l%m,%f  %l%m"

// default values for b_p_ff 'fileformat' and p_ffs 'fileformats'
#ವ್ಯಾಖ್ಯಾನಿಸು FF_DOS		"dos"
#ವ್ಯಾಖ್ಯಾನಿಸು FF_MAC		"mac"
#ವ್ಯಾಖ್ಯಾನಿಸು FF_UNIX		"unix"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_CRNL
# define DFLT_FF	"dos"
# define DFLT_FFS_VIM	"dos,unix"
# define DFLT_FFS_VI	"dos,unix"	// also autodetect in compatible mode
# define DFLT_TEXTAUTO	TRUE
#ಇಲ್ಲದಿದ್ದರೆ
# define DFLT_FF	"unix"
# define DFLT_FFS_VIM	"unix,dos"
# ifdef __CYGWIN__
#  define DFLT_FFS_VI	"unix,dos"	// Cygwin always needs file detection
#  define DFLT_TEXTAUTO TRUE
# ಇಲ್ಲದಿದ್ದರೆ
#  define DFLT_FFS_VI	""
#  define DFLT_TEXTAUTO FALSE
# endif
#ಆದರೆ_ಅಂತ್ಯ


// Possible values for 'encoding'
#ವ್ಯಾಖ್ಯಾನಿಸು ENC_UCSBOM	"ucs-bom"	// check for BOM at start of file

// default value for 'encoding'
#ಆದರೆ defined(MSWIN) || defined(__MVS__)
# define ENC_DFLT	"utf-8"
#ಇಲ್ಲದಿದ್ದರೆ
# define ENC_DFLT	"latin1"
#ಆದರೆ_ಅಂತ್ಯ

// end-of-line style
#ವ್ಯಾಖ್ಯಾನಿಸು EOL_UNKNOWN	(-1)	// not defined yet
#ವ್ಯಾಖ್ಯಾನಿಸು EOL_UNIX	0	// NL
#ವ್ಯಾಖ್ಯಾನಿಸು EOL_DOS		1	// CR NL
#ವ್ಯಾಖ್ಯಾನಿಸು EOL_MAC		2	// CR

// Formatting options for p_fo 'formatoptions'
#ವ್ಯಾಖ್ಯಾನಿಸು FO_WRAP		't'
#ವ್ಯಾಖ್ಯಾನಿಸು FO_WRAP_COMS	'c'
#ವ್ಯಾಖ್ಯಾನಿಸು FO_RET_COMS	'r'
#ವ್ಯಾಖ್ಯಾನಿಸು FO_OPEN_COMS	'o'
#ವ್ಯಾಖ್ಯಾನಿಸು FO_NO_OPEN_COMS	'/'
#ವ್ಯಾಖ್ಯಾನಿಸು FO_Q_COMS	'q'
#ವ್ಯಾಖ್ಯಾನಿಸು FO_Q_ಸಂಖ್ಯೆ	'n'
#ವ್ಯಾಖ್ಯಾನಿಸು FO_Q_ಎರಡನೇ	'2'
#ವ್ಯಾಖ್ಯಾನಿಸು FO_INS_VI	'v'
#ವ್ಯಾಖ್ಯಾನಿಸು FO_INS_LONG	'l'
#ವ್ಯಾಖ್ಯಾನಿಸು FO_INS_BLANK	'b'
#ವ್ಯಾಖ್ಯಾನಿಸು FO_MBYTE_BREAK	'm'	// break before/after multi-byte char
#ವ್ಯಾಖ್ಯಾನಿಸು FO_MBYTE_JOIN	'M'	// no space before/after multi-byte char
#ವ್ಯಾಖ್ಯಾನಿಸು FO_MBYTE_JOIN2	'B'	// no space between multi-byte chars
#ವ್ಯಾಖ್ಯಾನಿಸು FO_ONE_LETTER	'1'
#ವ್ಯಾಖ್ಯಾನಿಸು FO_WHITE_PAR	'w'	// trailing white space continues paragr.
#ವ್ಯಾಖ್ಯಾನಿಸು FO_AUTO		'a'	// automatic formatting
#ವ್ಯಾಖ್ಯಾನಿಸು FO_RIGOROUS_TW	']'     // respect textwidth rigorously
#ವ್ಯಾಖ್ಯಾನಿಸು FO_REMOVE_COMS	'j'	// remove comment leaders when joining lines
#ವ್ಯಾಖ್ಯಾನಿಸು FO_PERIOD_ABBR	'p'	// don't break a single space after a period

#ವ್ಯಾಖ್ಯಾನಿಸು DFLT_FO_VI	"vt"
#ವ್ಯಾಖ್ಯಾನಿಸು DFLT_FO_VIM	"tcq"
#ವ್ಯಾಖ್ಯಾನಿಸು FO_ALL		"tcro/q2vlb1mMBn,aw]jp"	// for do_set()

// characters for the p_cpo option:
#ವ್ಯಾಖ್ಯಾನಿಸು CPO_ALTREAD	'a'	// ":read" sets alternate file name
#ವ್ಯಾಖ್ಯಾನಿಸು CPO_ALTWRITE	'A'	// ":write" sets alternate file name
#ವ್ಯಾಖ್ಯಾನಿಸು CPO_BAR		'b'	// "\|" ends a mapping
#ವ್ಯಾಖ್ಯಾನಿಸು CPO_BSLASH	'B'	// backslash in mapping is not special
#ವ್ಯಾಖ್ಯಾನಿಸು CPO_ಹುಡುಕು	'c'
#ವ್ಯಾಖ್ಯಾನಿಸು CPO_CONCAT	'C'	// Don't concatenate sourced lines
#ವ್ಯಾಖ್ಯಾನಿಸು CPO_DOTTAG	'd'	// "./tags" in 'tags' is in current dir
#ವ್ಯಾಖ್ಯಾನಿಸು CPO_DIGRAPH	'D'	// No digraph after "r", "f", etc.
#ವ್ಯಾಖ್ಯಾನಿಸು CPO_EXECBUF	'e'
#ವ್ಯಾಖ್ಯಾನಿಸು CPO_EMPTYREGION	'E'	// operating on empty region is an error
#ವ್ಯಾಖ್ಯಾನಿಸು CPO_FNAMER	'f'	// set file name for ":r file"
#ವ್ಯಾಖ್ಯಾನಿಸು CPO_FNAMEW	'F'	// set file name for ":w file"
#ವ್ಯಾಖ್ಯಾನಿಸು CPO_GOTO1	'g'	// goto line 1 for ":edit"
#ವ್ಯಾಖ್ಯಾನಿಸು CPO_INSEND	'H'	// "I" inserts before last blank in line
#ವ್ಯಾಖ್ಯಾನಿಸು CPO_INTMOD	'i'	// interrupt a read makes buffer modified
#ವ್ಯಾಖ್ಯಾನಿಸು CPO_INDENT	'I'	// remove auto-indent more often
#ವ್ಯಾಖ್ಯಾನಿಸು CPO_JOINSP	'j'	// only use two spaces for join after '.'
#ವ್ಯಾಖ್ಯಾನಿಸು CPO_ENDOFSENT	'J'	// need two spaces to detect end of sentence
#ವ್ಯಾಖ್ಯಾನಿಸು CPO_KEYCODE	'k'	// don't recognize raw key code in mappings
#ವ್ಯಾಖ್ಯಾನಿಸು CPO_KOFFSET	'K'	// don't wait for key code in mappings
#ವ್ಯಾಖ್ಯಾನಿಸು CPO_LITERAL	'l'	// take char after backslash in [] literal
#ವ್ಯಾಖ್ಯಾನಿಸು CPO_LISTWM	'L'	// 'list' changes wrapmargin
#ವ್ಯಾಖ್ಯಾನಿಸು CPO_SHOWMATCH	'm'
#ವ್ಯಾಖ್ಯಾನಿಸು CPO_MATCHBSL	'M'	// "%" ignores use of backslashes
#ವ್ಯಾಖ್ಯಾನಿಸು CPO_NUMCOL	'n'	// 'number' column also used for text
#ವ್ಯಾಖ್ಯಾನಿಸು CPO_LINEOFF	'o'
#ವ್ಯಾಖ್ಯಾನಿಸು CPO_OVERNEW	'O'	// silently overwrite new file
#ವ್ಯಾಖ್ಯಾನಿಸು CPO_LISP	'p'	// 'lisp' indenting
#ವ್ಯಾಖ್ಯಾನಿಸು CPO_FNAMEAPP	'P'	// set file name for ":w >>file"
#ವ್ಯಾಖ್ಯಾನಿಸು CPO_JOINCOL	'q'	// with "3J" use column after first join
#ವ್ಯಾಖ್ಯಾನಿಸು CPO_REDO	'r'
#ವ್ಯಾಖ್ಯಾನಿಸು CPO_REMMARK	'R'	// remove marks when filtering
#ವ್ಯಾಖ್ಯಾನಿಸು CPO_BUFOPT	's'
#ವ್ಯಾಖ್ಯಾನಿಸು CPO_BUFOPTGLOB	'S'
#ವ್ಯಾಖ್ಯಾನಿಸು CPO_TAGPAT	't'	// tag pattern is used for "n"
#ವ್ಯಾಖ್ಯಾನಿಸು CPO_UNDO	'u'	// "u" undoes itself
#ವ್ಯಾಖ್ಯಾನಿಸು CPO_BACKSPACE	'v'	// "v" keep deleted text
#ವ್ಯಾಖ್ಯಾನಿಸು CPO_CW		'w'	// "cw" only changes one blank
#ವ್ಯಾಖ್ಯಾನಿಸು CPO_FWRITE	'W'	// "w!" doesn't overwrite readonly files
#ವ್ಯಾಖ್ಯಾನಿಸು CPO_ESC		'x'
#ವ್ಯಾಖ್ಯಾನಿಸು CPO_REPLCNT	'X'	// "R" with a count only deletes chars once
#ವ್ಯಾಖ್ಯಾನಿಸು CPO_YANK	'y'
#ವ್ಯಾಖ್ಯಾನಿಸು CPO_KEEPRO	'Z'	// don't reset 'readonly' on ":w!"
#ವ್ಯಾಖ್ಯಾನಿಸು CPO_ಶಬ್ದ	'z'	// do not special-case word motions cw and dw
#ವ್ಯಾಖ್ಯಾನಿಸು CPO_DOLLAR	'$'
#ವ್ಯಾಖ್ಯಾನಿಸು CPO_FILTER	'!'
#ವ್ಯಾಖ್ಯಾನಿಸು CPO_ಹೊಂದಿಕೆ	'%'
#ವ್ಯಾಖ್ಯಾನಿಸು CPO_ನಕ್ಷತ್ರ	'*'	// ":*" means ":@"
#ವ್ಯಾಖ್ಯಾನಿಸು CPO_PLUS	'+'	// ":write file" resets 'modified'
#ವ್ಯಾಖ್ಯಾನಿಸು CPO_MINUS	'-'	// "9-" fails at and before line 9
#ವ್ಯಾಖ್ಯಾನಿಸು CPO_SPECI	'<'	// don't recognize <> in mappings
#ವ್ಯಾಖ್ಯಾನಿಸು CPO_REGAPPEND	'>'	// insert NL when appending to a register
// POSIX flags
#ವ್ಯಾಖ್ಯಾನಿಸು CPO_HASH	'#'	// "D", "o" and "O" do not use a count
#ವ್ಯಾಖ್ಯಾನಿಸು CPO_PARA	'{'	// "{" is also a paragraph boundary
#ವ್ಯಾಖ್ಯಾನಿಸು CPO_TSIZE	'|'	// $LINES and $COLUMNS overrule term size
#ವ್ಯಾಖ್ಯಾನಿಸು CPO_PRESERVE	'&'	// keep swap file after :preserve
#ವ್ಯಾಖ್ಯಾನಿಸು CPO_SUBPERCENT	'/'	// % in :s string uses previous one
#ವ್ಯಾಖ್ಯಾನಿಸು CPO_BACKSL	'\\'	// \ is not special in []
#ವ್ಯಾಖ್ಯಾನಿಸು CPO_CHDIR	'.'	// don't chdir if buffer is modified
#ವ್ಯಾಖ್ಯಾನಿಸು CPO_SCOLON	';'	// using "," and ";" will skip over char if
				// cursor would not move
#ವ್ಯಾಖ್ಯಾನಿಸು CPO_NOSYMLINKS	'~'	// don't resolve symlinks when changing directory
// default values for Vim, Vi and POSIX
#ವ್ಯಾಖ್ಯಾನಿಸು CPO_VIM		"aABceFsz"
#ವ್ಯಾಖ್ಯಾನಿಸು CPO_VI		"aAbBcCdDeEfFgHiIjJkKlLmMnoOpPqrRsStuvwWxXyZz$!%*-+<>;"
#ವ್ಯಾಖ್ಯಾನಿಸು CPO_ALL		"aAbBcCdDeEfFgHiIjJkKlLmMnoOpPqrRsStuvwWxXyZz$!%*-+<>#{|&/\\.;~"

// characters for p_ww option:
#ವ್ಯಾಖ್ಯಾನಿಸು WW_ALL		"bshl<>[]~"

// characters for p_mouse option:
#ವ್ಯಾಖ್ಯಾನಿಸು MOUSE_ಸಾಮಾನ್ಯ	'n'		// use mouse in Normal mode
#ವ್ಯಾಖ್ಯಾನಿಸು MOUSE_VISUAL	'v'		// use mouse in Visual/Select mode
#ವ್ಯಾಖ್ಯಾನಿಸು MOUSE_ಸೇರಿಸು	'i'		// use mouse in Insert mode
#ವ್ಯಾಖ್ಯಾನಿಸು MOUSE_COMMAND	'c'		// use mouse in Command-line mode
#ವ್ಯಾಖ್ಯಾನಿಸು MOUSE_HELP	'h'		// use mouse in help buffers
#ವ್ಯಾಖ್ಯಾನಿಸು MOUSE_RETURN	'r'		// use mouse for hit-return message
#ವ್ಯಾಖ್ಯಾನಿಸು MOUSE_A		"nvich"		// used for 'a' flag
#ವ್ಯಾಖ್ಯಾನಿಸು MOUSE_ALL	"anvichr"	// all possible characters
#ವ್ಯಾಖ್ಯಾನಿಸು MOUSE_NONE	' '		// don't use Visual selection
#ವ್ಯಾಖ್ಯಾನಿಸು MOUSE_NONEF	'x'		// forced modeless selection

#ವ್ಯಾಖ್ಯಾನಿಸು COCU_ALL	"nvic"		// flags for 'concealcursor'

// characters for p_shm option:
#ವ್ಯಾಖ್ಯಾನಿಸು SHM_RO		'r'		// readonly
#ವ್ಯಾಖ್ಯಾನಿಸು SHM_MOD		'm'		// modified
#ವ್ಯಾಖ್ಯಾನಿಸು SHM_FILE	'f'		// (file 1 of 2)
#ವ್ಯಾಖ್ಯಾನಿಸು SHM_ಕೊನೆಯ	'i'		// last line incomplete
#ವ್ಯಾಖ್ಯಾನಿಸು SHM_ಪಠ್ಯ	'x'		// tx instead of textmode
#ವ್ಯಾಖ್ಯಾನಿಸು SHM_ಸಾಲುಗಳು	'l'		// "L" instead of "lines"
#ವ್ಯಾಖ್ಯಾನಿಸು SHM_NEW		'n'		// "[New]" instead of "[New file]"
#ವ್ಯಾಖ್ಯಾನಿಸು SHM_WRI		'w'		// "[w]" instead of "written"
#ವ್ಯಾಖ್ಯಾನಿಸು SHM_A		"rmfixlnw"	// represented by 'a' flag
#ವ್ಯಾಖ್ಯಾನಿಸು SHM_WRITE	'W'		// don't use "written" at all
#ವ್ಯಾಖ್ಯಾನಿಸು SHM_TRUNC	't'		// truncate file messages
#ವ್ಯಾಖ್ಯಾನಿಸು SHM_TRUNCALL	'T'		// truncate all messages
#ವ್ಯಾಖ್ಯಾನಿಸು SHM_OVER	'o'		// overwrite file messages
#ವ್ಯಾಖ್ಯಾನಿಸು SHM_OVERALL	'O'		// overwrite more messages
#ವ್ಯಾಖ್ಯಾನಿಸು SHM_ಹುಡುಕು	's'		// no search hit bottom messages
#ವ್ಯಾಖ್ಯಾನಿಸು SHM_ATTENTION	'A'		// no ATTENTION messages
#ವ್ಯಾಖ್ಯಾನಿಸು SHM_INTRO	'I'		// intro messages
#ವ್ಯಾಖ್ಯಾನಿಸು SHM_COMPLETIONMENU  'c'		// completion menu messages
#ವ್ಯಾಖ್ಯಾನಿಸು SHM_COMPLETIONSCAN  'C'		// completion scanning messages
#ವ್ಯಾಖ್ಯಾನಿಸು SHM_RECORDING	'q'		// short recording message
#ವ್ಯಾಖ್ಯಾನಿಸು SHM_FILEINFO	'F'		// no file info messages
#ವ್ಯಾಖ್ಯಾನಿಸು SHM_SEARCHCOUNT  'S'		// no search stats: '[1/10]'
#ವ್ಯಾಖ್ಯಾನಿಸು SHM_UNDO	'u'		// undo messages
#ವ್ಯಾಖ್ಯಾನಿಸು SHM_POSIX       "AS"		// POSIX value
#ವ್ಯಾಖ್ಯಾನಿಸು SHM_ALL		"rmfixlnwaWtToOsAIcCqFSu" // all possible flags for 'shm'
#ವ್ಯಾಖ್ಯಾನಿಸು SHM_LEN		30		// max length of all flags together
					// plus a NUL character

// characters for p_go:
#ವ್ಯಾಖ್ಯಾನಿಸು GO_TERMINAL	'!'		// use terminal for system commands
#ವ್ಯಾಖ್ಯಾನಿಸು GO_ASEL		'a'		// autoselect
#ವ್ಯಾಖ್ಯಾನಿಸು GO_ASELML	'A'		// autoselect modeless selection
#ವ್ಯಾಖ್ಯಾನಿಸು GO_BOT		'b'		// use bottom scrollbar
#ವ್ಯಾಖ್ಯಾನಿಸು GO_CONDIALOG	'c'		// use console dialog
#ವ್ಯಾಖ್ಯಾನಿಸು GO_TITLEBAR	'C'		// use 'hl-TitleBar'
#ವ್ಯಾಖ್ಯಾನಿಸು GO_DARKTHEME	'd'		// use dark theme variant
#ವ್ಯಾಖ್ಯಾನಿಸು GO_TABLINE	'e'		// may show tabline
#ವ್ಯಾಖ್ಯಾನಿಸು GO_FORG		'f'		// start GUI in foreground
#ವ್ಯಾಖ್ಯಾನಿಸು GO_GREY		'g'		// use grey menu items
#ವ್ಯಾಖ್ಯಾನಿಸು GO_HORSCROLL	'h'		// flexible horizontal scrolling
#ವ್ಯಾಖ್ಯಾನಿಸು GO_ICON		'i'		// use Vim icon
#ವ್ಯಾಖ್ಯಾನಿಸು GO_ಎಡ		'l'		// use left scrollbar
#ವ್ಯಾಖ್ಯಾನಿಸು GO_VLEFT	'L'		// left scrollbar with vert split
#ವ್ಯಾಖ್ಯಾನಿಸು GO_MENUS	'm'		// use menu bar
#ವ್ಯಾಖ್ಯಾನಿಸು GO_NOSYSMENU	'M'		// don't source system menu
#ವ್ಯಾಖ್ಯಾನಿಸು GO_ಸೂಚಕ	'p'		// pointer enter/leave callbacks
#ವ್ಯಾಖ್ಯಾನಿಸು GO_ASELPLUS	'P'		// autoselectPlus
#ವ್ಯಾಖ್ಯಾನಿಸು GO_ಬಲ	'r'		// use right scrollbar
#ವ್ಯಾಖ್ಯಾನಿಸು GO_VRIGHT	'R'		// right scrollbar with vert split
#ವ್ಯಾಖ್ಯಾನಿಸು GO_FULLSCREEN	's'		// enter fullscreen
#ವ್ಯಾಖ್ಯಾನಿಸು GO_TEAROFF	't'		// add tear-off menu items
#ವ್ಯಾಖ್ಯಾನಿಸು GO_TOOLBAR	'T'		// add toolbar
#ವ್ಯಾಖ್ಯಾನಿಸು GO_FOOTER	'F'		// add footer
#ವ್ಯಾಖ್ಯಾನಿಸು GO_VERTICAL	'v'		// arrange dialog buttons vertically
#ವ್ಯಾಖ್ಯಾನಿಸು GO_KEEPWINSIZE	'k'		// keep GUI window size
// all possible flags for 'go'
#ವ್ಯಾಖ್ಯಾನಿಸು GO_ALL		"!aAbcCdefFghilLmMpPrRstTvk"

// flags for 'comments' option
#ವ್ಯಾಖ್ಯಾನಿಸು COM_NEST	'n'		// comments strings nest
#ವ್ಯಾಖ್ಯಾನಿಸು COM_BLANK	'b'		// needs blank after string
#ವ್ಯಾಖ್ಯಾನಿಸು COM_ಆರಂಭ	's'		// start of comment
#ವ್ಯಾಖ್ಯಾನಿಸು COM_ಮಧ್ಯ	'm'		// middle of comment
#ವ್ಯಾಖ್ಯಾನಿಸು COM_ಕೊನೆ		'e'		// end of comment
#ವ್ಯಾಖ್ಯಾನಿಸು COM_AUTO_ಕೊನೆ	'x'		// last char of end closes comment
#ವ್ಯಾಖ್ಯಾನಿಸು COM_ಮೊದಲ	'f'		// first line comment only
#ವ್ಯಾಖ್ಯಾನಿಸು COM_ಎಡ	'l'		// left adjusted
#ವ್ಯಾಖ್ಯಾನಿಸು COM_ಬಲ	'r'		// right adjusted
#ವ್ಯಾಖ್ಯಾನಿಸು COM_NOBACK	'O'		// don't use for "O" command
#ವ್ಯಾಖ್ಯಾನಿಸು COM_ALL		"nbsmexflrO"	// all flags for 'comments' option
#ವ್ಯಾಖ್ಯಾನಿಸು COM_MAX_LEN	50		// maximum length of a part

// flags for 'statusline' option
#ವ್ಯಾಖ್ಯಾನಿಸು STL_FILEPATH	'f'		// path of file in buffer
#ವ್ಯಾಖ್ಯಾನಿಸು STL_FULLPATH	'F'		// full path of file in buffer
#ವ್ಯಾಖ್ಯಾನಿಸು STL_FILENAME	't'		// last part (tail) of file path
#ವ್ಯಾಖ್ಯಾನಿಸು STL_ಸ್ತಂಭ	'c'		// column og cursor
#ವ್ಯಾಖ್ಯಾನಿಸು STL_VIRTCOL	'v'		// virtual column
#ವ್ಯಾಖ್ಯಾನಿಸು STL_VIRTCOL_ALT	'V'		// - with 'if different' display
#ವ್ಯಾಖ್ಯಾನಿಸು STL_ಸಾಲು	'l'		// line number of cursor
#ವ್ಯಾಖ್ಯಾನಿಸು STL_NUMLINES	'L'		// number of lines in buffer
#ವ್ಯಾಖ್ಯಾನಿಸು STL_BUFNO	'n'		// current buffer number
#ವ್ಯಾಖ್ಯಾನಿಸು STL_KEYMAP	'k'		// 'keymap' when active
#ವ್ಯಾಖ್ಯಾನಿಸು STL_ಸ್ಥಾನಾಂತರ	'o'		// offset of character under cursor
#ವ್ಯಾಖ್ಯಾನಿಸು STL_ಸ್ಥಾನಾಂತರ_X	'O'		// - in hexadecimal
#ವ್ಯಾಖ್ಯಾನಿಸು STL_BYTEVAL	'b'		// byte value of character
#ವ್ಯಾಖ್ಯಾನಿಸು STL_BYTEVAL_X	'B'		// - in hexadecimal
#ವ್ಯಾಖ್ಯಾನಿಸು STL_ROFLAG	'r'		// readonly flag
#ವ್ಯಾಖ್ಯಾನಿಸು STL_ROFLAG_ALT	'R'		// - other display
#ವ್ಯಾಖ್ಯಾನಿಸು STL_HELPFLAG	'h'		// window is showing a help file
#ವ್ಯಾಖ್ಯಾನಿಸು STL_HELPFLAG_ALT 'H'		// - other display
#ವ್ಯಾಖ್ಯಾನಿಸು STL_FILETYPE	'y'		// 'filetype'
#ವ್ಯಾಖ್ಯಾನಿಸು STL_FILETYPE_ALT 'Y'		// - other display
#ವ್ಯಾಖ್ಯಾನಿಸು STL_PREVIEWFLAG	'w'		// window is showing the preview buf
#ವ್ಯಾಖ್ಯಾನಿಸು STL_PREVIEWFLAG_ALT 'W'		// - other display
#ವ್ಯಾಖ್ಯಾನಿಸು STL_MODIFIED	'm'		// modified flag
#ವ್ಯಾಖ್ಯಾನಿಸು STL_MODIFIED_ALT 'M'		// - other display
#ವ್ಯಾಖ್ಯಾನಿಸು STL_QUICKFIX	'q'		// quickfix window description
#ವ್ಯಾಖ್ಯಾನಿಸು STL_PERCENTAGE	'p'		// percentage through file
#ವ್ಯಾಖ್ಯಾನಿಸು STL_ALTPERCENT	'P'		// percentage as TOP BOT ALL or NN%
#ವ್ಯಾಖ್ಯಾನಿಸು STL_ARGLISTSTAT	'a'		// argument list status as (x of y)
#ವ್ಯಾಖ್ಯಾನಿಸು STL_PAGENUM	'N'		// page number (when printing)
#ವ್ಯಾಖ್ಯಾನಿಸು STL_SHOWCMD	'S'		// 'showcmd' buffer
#ವ್ಯಾಖ್ಯಾನಿಸು STL_VIM_ಅಭಿವ್ಯಕ್ತಿ	'{'		// start of expression to substitute
#ವ್ಯಾಖ್ಯಾನಿಸು STL_SEPARATE	'='		// separation between alignment
					// sections
#ವ್ಯಾಖ್ಯಾನಿಸು STL_TRUNCMARK	'<'		// truncation mark if line is too long
#ವ್ಯಾಖ್ಯಾನಿಸು STL_USER_HL	'*'		// highlight from (User)1..9 or 0
#ವ್ಯಾಖ್ಯಾನಿಸು STL_HIGHLIGHT	'#'		// highlight name
#ವ್ಯಾಖ್ಯಾನಿಸು STL_LINEBREAK	'@'		// insert a line break
#ವ್ಯಾಖ್ಯಾನಿಸು STL_CLICKFUNC	'['		// click handler region
#ವ್ಯಾಖ್ಯಾನಿಸು STL_TABPAGENR	'T'		// tab page label nr
#ವ್ಯಾಖ್ಯಾನಿಸು STL_TABCLOSENR	'X'		// tab page close nr
#ವ್ಯಾಖ್ಯಾನಿಸು STL_ALL		((char_u *) "fFtcvVlLknoObBrRhHmYyWwMqpPaNS{#@[")

// flags used for parsed 'wildmode'
#ವ್ಯಾಖ್ಯಾನಿಸು WIM_ತುಂಬಿದ	0x01
#ವ್ಯಾಖ್ಯಾನಿಸು WIM_LONGEST	0x02
#ವ್ಯಾಖ್ಯಾನಿಸು WIM_LIST	0x04
#ವ್ಯಾಖ್ಯಾನಿಸು WIM_BUFLASTUSED	0x08
#ವ್ಯಾಖ್ಯಾನಿಸು WIM_NOSELECT	0x10
#ವ್ಯಾಖ್ಯಾನಿಸು WIM_NOINSERT	0x20

// flags for the 'wildoptions' option
// each defined char should be unique over all values.
#ವ್ಯಾಖ್ಯಾನಿಸು WOP_FUZZY	'z'
#ವ್ಯಾಖ್ಯಾನಿಸು WOP_TAGFILE	'g'
#ವ್ಯಾಖ್ಯಾನಿಸು WOP_PUM		'p'
#ವ್ಯಾಖ್ಯಾನಿಸು WOP_EXACTTEXT	'x'

// arguments for can_bs()
// each defined char should be unique over all values
// except for BS_START, that intentionally also matches BS_NOSTOP
// because BS_NOSTOP behaves exactly the same except it
// does not stop at the start of the insert point
#ವ್ಯಾಖ್ಯಾನಿಸು BS_INDENT	'i'	// "Indent"
#ವ್ಯಾಖ್ಯಾನಿಸು BS_EOL		'l'	// "eoL"
#ವ್ಯಾಖ್ಯಾನಿಸು BS_ಆರಂಭ	's'	// "Start"
#ವ್ಯಾಖ್ಯಾನಿಸು BS_NOSTOP	'p'	// "nostoP

// flags for the 'culopt' option
#ವ್ಯಾಖ್ಯಾನಿಸು CULOPT_ಸಾಲು	0x01	// Highlight complete line
#ವ್ಯಾಖ್ಯಾನಿಸು CULOPT_SCRLINE	0x02	// Highlight screen line
#ವ್ಯಾಖ್ಯಾನಿಸು CULOPT_NBR	0x04	// Highlight Number column

#ವ್ಯಾಖ್ಯಾನಿಸು LISPWORD_ಮೌಲ್ಯ	"defun,define,defmacro,set!,lambda,if,case,let,flet,let*,letrec,do,do*,define-syntax,let-syntax,letrec-syntax,destructuring-bind,defpackage,defparameter,defstruct,deftype,defvar,do-all-symbols,do-external-symbols,do-symbols,dolist,dotimes,ecase,etypecase,eval-when,labels,macrolet,multiple-value-bind,multiple-value-call,multiple-value-prog1,multiple-value-setq,prog1,progv,typecase,unless,unwind-protect,when,with-input-from-string,with-open-file,with-open-stream,with-output-to-string,with-package-iterator,define-condition,handler-bind,handler-case,restart-bind,restart-case,with-simple-restart,store-value,use-value,muffle-warning,abort,continue,with-slots,with-slots*,with-accessors,with-accessors*,defclass,defmethod,print-unreadable-object"

/*
 * The following are actual variables for the options
 */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
EXTERN ದೀರ್ಘ	p_aleph;	// 'aleph'
#ಆದರೆ_ಅಂತ್ಯ
EXTERN char_u	*p_ambw;	// 'ambiwidth'
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_AUTOCHDIR
EXTERN ಪೂರ್ಣಾಂಕ	p_acd;		// 'autochdir'
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_AUTOSHELLDIR
EXTERN ಪೂರ್ಣಾಂಕ	p_asd;		// 'autoshelldir'
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಪೂರ್ಣಾಂಕ	p_ai;		// 'autoindent'
EXTERN ಪೂರ್ಣಾಂಕ	p_bin;		// 'binary'
EXTERN ಪೂರ್ಣಾಂಕ	p_bomb;		// 'bomb'
EXTERN ಪೂರ್ಣಾಂಕ	p_bl;		// 'buflisted'
EXTERN ಪೂರ್ಣಾಂಕ	p_cin;		// 'cindent'
EXTERN char_u	*p_cink;	// 'cinkeys'
EXTERN char_u	*p_cinsd;	// 'cinscopedecls'
EXTERN char_u	*p_cinw;	// 'cinwords'
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_COMPL_FUNC
EXTERN char_u	*p_cfu;		// 'completefunc'
EXTERN char_u	*p_ofu;		// 'omnifunc'
EXTERN char_u	*p_tsrfu;	// 'thesaurusfunc'
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಪೂರ್ಣಾಂಕ	p_ci;		// 'copyindent'
#ಆದರೆ defined(FEAT_GUI) && defined(MACOS_X)
EXTERN ಪೂರ್ಣಾಂಕ	*p_antialias;	// 'antialias'
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಪೂರ್ಣಾಂಕ	p_ar;		// 'autoread'
EXTERN ಪೂರ್ಣಾಂಕ	p_aw;		// 'autowrite'
EXTERN ಪೂರ್ಣಾಂಕ	p_awa;		// 'autowriteall'
EXTERN char_u	*p_bs;		// 'backspace'
EXTERN char_u	*p_bg;		// 'background'
EXTERN ಪೂರ್ಣಾಂಕ	p_bk;		// 'backup'
EXTERN char_u	*p_bkc;		// 'backupcopy'
EXTERN ಚಿಹ್ನೆರಹಿತ	bkc_flags;	// flags from 'backupcopy'
#ವ್ಯಾಖ್ಯಾನಿಸು BKC_YES		0x001
#ವ್ಯಾಖ್ಯಾನಿಸು BKC_AUTO		0x002
#ವ್ಯಾಖ್ಯಾನಿಸು BKC_NO			0x004
#ವ್ಯಾಖ್ಯಾನಿಸು BKC_BREAKSYMLINK	0x008
#ವ್ಯಾಖ್ಯಾನಿಸು BKC_BREAKHARDLINK	0x010
EXTERN char_u	*p_bdir;	// 'backupdir'
EXTERN char_u	*p_bex;		// 'backupext'
EXTERN char_u	*p_bo;		// 'belloff'
EXTERN ಚಿಹ್ನೆರಹಿತ	bo_flags;

// values for the 'belloff' option
#ವ್ಯಾಖ್ಯಾನಿಸು BO_ALL		0x0001
#ವ್ಯಾಖ್ಯಾನಿಸು BO_BS		0x0002
#ವ್ಯಾಖ್ಯಾನಿಸು BO_CRSR		0x0004
#ವ್ಯಾಖ್ಯಾನಿಸು BO_COMPL	0x0008
#ವ್ಯಾಖ್ಯಾನಿಸು BO_COPY		0x0010
#ವ್ಯಾಖ್ಯಾನಿಸು BO_CTRLG	0x0020
#ವ್ಯಾಖ್ಯಾನಿಸು BO_ERROR	0x0040
#ವ್ಯಾಖ್ಯಾನಿಸು BO_ESC		0x0080
#ವ್ಯಾಖ್ಯಾನಿಸು BO_EX		0x0100
#ವ್ಯಾಖ್ಯಾನಿಸು BO_HANGUL	0x0200
#ವ್ಯಾಖ್ಯಾನಿಸು BO_IM		0x0400
#ವ್ಯಾಖ್ಯಾನಿಸು BO_LANG		0x0800
#ವ್ಯಾಖ್ಯಾನಿಸು BO_MESS		0x1000
#ವ್ಯಾಖ್ಯಾನಿಸು BO_ಹೊಂದಿಕೆ	0x2000
#ವ್ಯಾಖ್ಯಾನಿಸು BO_OPER		0x4000
#ವ್ಯಾಖ್ಯಾನಿಸು BO_REG		0x8000
#ವ್ಯಾಖ್ಯಾನಿಸು BO_SH		0x10000
#ವ್ಯಾಖ್ಯಾನಿಸು BO_SPELL	0x20000
#ವ್ಯಾಖ್ಯಾನಿಸು BO_ಪದ		0x40000
#ವ್ಯಾಖ್ಯಾನಿಸು BO_WILD		0x80000

EXTERN char_u	*p_bsk;		// 'backupskip'
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CRYPT
EXTERN char_u	*p_cm;		// 'cryptmethod'
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_BEVAL
# ifdef FEAT_BEVAL_GUI
EXTERN ಪೂರ್ಣಾಂಕ	p_beval;	// 'ballooneval'
# endif
EXTERN ದೀರ್ಘ	p_bdlay;	// 'balloondelay'
# ifdef FEAT_EVAL
EXTERN char_u	*p_bexpr;
# endif
# ifdef FEAT_BEVAL_ಪದ
EXTERN ಪೂರ್ಣಾಂಕ	p_bevalterm;	// 'balloonevalterm'
# endif
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_BROWSE
EXTERN char_u	*p_bsdir;	// 'browsedir'
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
EXTERN char_u	*p_breakat;	// 'breakat'
#ಆದರೆ_ಅಂತ್ಯ
EXTERN char_u	*p_bh;		// 'bufhidden'
EXTERN char_u	*p_bt;		// 'buftype'
EXTERN char_u	*p_cmp;		// 'casemap'
EXTERN ಚಿಹ್ನೆರಹಿತ	cmp_flags;
#ವ್ಯಾಖ್ಯಾನಿಸು CMP_ಆಂತರಿಕ		0x001
#ವ್ಯಾಖ್ಯಾನಿಸು CMP_KEEPASCII		0x002
EXTERN char_u	*p_enc;		// 'encoding'
EXTERN ಪೂರ್ಣಾಂಕ	p_deco;		// 'delcombine'
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN char_u	*p_ccv;		// 'charconvert'
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಪೂರ್ಣಾಂಕ	p_cdh;		// 'cdhome'
EXTERN char_u	*p_cino;	// 'cinoptions'
EXTERN char_u	*p_cedit;	// 'cedit'
EXTERN ದೀರ್ಘ	p_cwh;		// 'cmdwinheight'
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_CLIPMETHOD
EXTERN char_u	*p_cb;		// 'clipboard'
EXTERN char_u	*p_cpm;		// 'clipmethod'
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ದೀರ್ಘ	p_ch;		// 'cmdheight'
EXTERN char_u	*p_cms;		// 'commentstring'
EXTERN char_u	*p_cpt;		// 'complete'
EXTERN ದೀರ್ಘ	p_cto;		// 'completetimeout'
#ಆದರೆ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
EXTERN ಪೂರ್ಣಾಂಕ	p_confirm;	// 'confirm'
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಪೂರ್ಣಾಂಕ	p_cp;		// 'compatible'
EXTERN char_u	*p_cfc;		// 'completefuzzycollect'
EXTERN ಚಿಹ್ನೆರಹಿತ cfc_flags;	// flags from "completefuzzycollect"
EXTERN char_u	*p_cia;		// 'completeitemalign'
EXTERN ಚಿಹ್ನೆರಹಿತ cia_flags;	// order flags of 'completeitemalign'
EXTERN char_u	*p_cot;		// 'completeopt'
EXTERN ಚಿಹ್ನೆರಹಿತ	cot_flags;	// flags from 'completeopt'
EXTERN ಪೂರ್ಣಾಂಕ	p_ac;		// 'autocomplete'
EXTERN ದೀರ್ಘ	p_act;		// 'autocompletetimeout'
EXTERN ದೀರ್ಘ	p_acl;		// 'autocompletedelay'
// Keep in sync with p_cot_values in optionstr.c
#ವ್ಯಾಖ್ಯಾನಿಸು COT_MENU	0x001
#ವ್ಯಾಖ್ಯಾನಿಸು COT_MENUONE	0x002
#ವ್ಯಾಖ್ಯಾನಿಸು COT_ANY_MENU	0x003	// combination of menu flags
#ವ್ಯಾಖ್ಯಾನಿಸು COT_LONGEST	0x004	// FALSE: insert full match,
				// TRUE: insert longest prefix
#ವ್ಯಾಖ್ಯಾನಿಸು COT_PREVIEW	    0x008
#ವ್ಯಾಖ್ಯಾನಿಸು COT_POPUP	    0x010
#ವ್ಯಾಖ್ಯಾನಿಸು COT_POPUPHIDDEN	    0x020
#ವ್ಯಾಖ್ಯಾನಿಸು COT_ANY_PREVIEW	    0x038   // combination of preview flags
#ವ್ಯಾಖ್ಯಾನಿಸು COT_NOINSERT	    0x040   // FALSE: select & insert, TRUE: noinsert
#ವ್ಯಾಖ್ಯಾನಿಸು COT_NOSELECT	    0x080   // FALSE: select & insert, TRUE: noselect
#ವ್ಯಾಖ್ಯಾನಿಸು COT_FUZZY	    0x100   // TRUE: fuzzy match enabled
#ವ್ಯಾಖ್ಯಾನಿಸು COT_NOSORT	    0x200   // TRUE: fuzzy match without qsort score
#ವ್ಯಾಖ್ಯಾನಿಸು COT_PREINSERT	    0x400   // TRUE: preinsert
#ವ್ಯಾಖ್ಯಾನಿಸು COT_ಹತ್ತಿರದ	    0x800   // TRUE: prioritize matches close to cursor

#ವ್ಯಾಖ್ಯಾನಿಸು CFC_KEYWORD         0x001
#ವ್ಯಾಖ್ಯಾನಿಸು CFC_FILES           0x002
#ವ್ಯಾಖ್ಯಾನಿಸು CFC_WHOLELINE       0x004

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ BACKSLASH_ಒಳ_FILENAME
EXTERN char_u	*p_csl;		// 'completeslash'
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ದೀರ್ಘ	p_ph;		// 'pumheight'
EXTERN ದೀರ್ಘ	p_pw;		// 'pumwidth'
EXTERN ದೀರ್ಘ	p_pmw;		// 'pummaxwidth'
EXTERN char_u	*p_pb;		// 'pumborder'
EXTERN char_u	*p_pumopt;	// 'pumopt'
EXTERN char_u	*p_com;		// 'comments'
EXTERN char_u	*p_cpo;		// 'cpoptions'
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CSCOPE
EXTERN char_u	*p_csprg;	// 'cscopeprg'
EXTERN ಪೂರ್ಣಾಂಕ	p_csre;		// 'cscoperelative'
# ifdef FEAT_QUICKFIX
EXTERN char_u	*p_csqf;	// 'cscopequickfix'
#  define	CSQF_CMDS   "sgdctefia"
#  define	CSQF_FLAGS  "+-0"
# endif
EXTERN ಪೂರ್ಣಾಂಕ	p_cst;		// 'cscopetag'
EXTERN ದೀರ್ಘ	p_csto;		// 'cscopetagorder'
EXTERN ದೀರ್ಘ	p_cspc;		// 'cscopepathcomp'
EXTERN ಪೂರ್ಣಾಂಕ	p_csverbose;	// 'cscopeverbose'
#ಆದರೆ_ಅಂತ್ಯ
EXTERN char_u	*p_debug;	// 'debug'
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕಂಡುಹಿಡಿ_ID
EXTERN char_u	*p_def;		// 'define'
EXTERN char_u	*p_inc;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
EXTERN char_u	*p_dia;		// 'diffanchors'
EXTERN char_u	*p_dip;		// 'diffopt'
# ifdef FEAT_EVAL
EXTERN char_u	*p_dex;		// 'diffexpr'
# endif
#ಆದರೆ_ಅಂತ್ಯ
EXTERN char_u	*p_dict;	// 'dictionary'
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIGRAPHS
EXTERN ಪೂರ್ಣಾಂಕ	p_dg;		// 'digraph'
#ಆದರೆ_ಅಂತ್ಯ
EXTERN char_u	*p_dir;		// 'directory'
EXTERN char_u	*p_dy;		// 'display'
EXTERN ಚಿಹ್ನೆರಹಿತ	dy_flags;
#ವ್ಯಾಖ್ಯಾನಿಸು DY_LASTLINE		0x001
#ವ್ಯಾಖ್ಯಾನಿಸು DY_TRUNCATE		0x002
#ವ್ಯಾಖ್ಯಾನಿಸು DY_UHEX			0x004
EXTERN ಪೂರ್ಣಾಂಕ	p_ed;		// 'edcompatible'
EXTERN char_u	*p_ead;		// 'eadirection'
EXTERN char_u	*p_emoji;	// 'emoji'
EXTERN ಪೂರ್ಣಾಂಕ	p_ea;		// 'equalalways'
EXTERN char_u	*p_ep;		// 'equalprg'
EXTERN ಪೂರ್ಣಾಂಕ	p_eb;		// 'errorbells'
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
EXTERN char_u	*p_ef;		// 'errorfile'
EXTERN char_u	*p_efm;		// 'errorformat'
EXTERN char_u	*p_gefm;	// 'grepformat'
EXTERN char_u	*p_gp;		// 'grepprg'
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಪೂರ್ಣಾಂಕ	p_eof;		// 'endoffile'
EXTERN ಪೂರ್ಣಾಂಕ	p_eol;		// 'endofline'
EXTERN ಪೂರ್ಣಾಂಕ	p_ek;		// 'esckeys'
EXTERN char_u	*p_ei;		// 'eventignore'
EXTERN ಪೂರ್ಣಾಂಕ	p_et;		// 'expandtab'
EXTERN ಪೂರ್ಣಾಂಕ	p_exrc;		// 'exrc'
EXTERN char_u	*p_fenc;	// 'fileencoding'
EXTERN char_u	*p_fencs;	// 'fileencodings'
EXTERN char_u	*p_ff;		// 'fileformat'
EXTERN char_u	*p_ffs;		// 'fileformats'
EXTERN ಪೂರ್ಣಾಂಕ	p_fic;		// 'fileignorecase'
EXTERN char_u	*p_ft;		// 'filetype'
EXTERN char_u	*p_fcs;		// 'fillchar'
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN char_u	*p_ffu;		// 'findfunc'
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಪೂರ್ಣಾಂಕ	p_fixeol;	// 'fixendofline'
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
EXTERN char_u	*p_fcl;		// 'foldclose'
EXTERN ದೀರ್ಘ	p_fdls;		// 'foldlevelstart'
EXTERN char_u	*p_fdo;		// 'foldopen'
EXTERN ಚಿಹ್ನೆರಹಿತ	fdo_flags;
# define FDO_ALL		0x001
# define FDO_ಖಂಡ		0x002
# define FDO_HOR		0x004
# define FDO_MARK		0x008
# define FDO_PERCENT		0x010
# define FDO_QUICKFIX		0x020
# define FDO_ಹುಡುಕು		0x040
# define FDO_ಟ್ಯಾಗ್		0x080
# define FDO_ಸೇರಿಸು		0x100
# define FDO_UNDO		0x200
# define FDO_JUMP		0x400
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_EVAL)
EXTERN char_u	*p_fex;		// 'formatexpr'
#ಆದರೆ_ಅಂತ್ಯ
EXTERN char_u	*p_flp;		// 'formatlistpat'
EXTERN char_u	*p_fo;		// 'formatoptions'
EXTERN char_u	*p_fp;		// 'formatprg'
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_FSYNC
EXTERN ಪೂರ್ಣಾಂಕ	p_fs;		// 'fsync'
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಪೂರ್ಣಾಂಕ	p_gd;		// 'gdefault'
EXTERN char_u	*p_jop;		// 'jumpoptions'
EXTERN ಚಿಹ್ನೆರಹಿತ	jop_flags;	//
#ವ್ಯಾಖ್ಯಾನಿಸು JOP_ರಾಶಿ		0x001
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
# ifdef FEAT_QUICKFIX
EXTERN char_u	*p_cpp;		// 'completepopup'
# endif
EXTERN char_u	*p_pvp;		// 'previewpopup'
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PRINTER
EXTERN char_u	*p_pdev;	// 'printdevice'
# ifdef FEAT_POSTSCRIPT
EXTERN char_u	*p_penc;	// 'printencoding'
EXTERN char_u	*p_pmfn;	// 'printmbfont'
EXTERN char_u	*p_pmcs;	// 'printmbcharset'
# endif
# ಆದರೆ defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
EXTERN char_u	*p_pexpr;	// 'printexpr'
# endif
EXTERN char_u	*p_pfn;		// 'printfont'
EXTERN char_u	*p_popt;	// 'printoptions'
EXTERN char_u	*p_ಶೀರ್ಷಿಕೆ;	// 'printheader'
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಪೂರ್ಣಾಂಕ	p_prompt;	// 'prompt'
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
EXTERN char_u	*p_guifont;	// 'guifont'
# ifdef FEAT_XFONTSET
EXTERN char_u	*p_guifontset;	// 'guifontset'
# endif
EXTERN char_u	*p_guifontwide;	// 'guifontwide'
EXTERN ಪೂರ್ಣಾಂಕ	p_guipty;	// 'guipty'
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
EXTERN char_u	*p_guiligatures;  // 'guiligatures'
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ (defined(FEAT_GUI_GTK) && !defined(USE_GTK4)) || defined(FEAT_GUI_X11)
EXTERN ದೀರ್ಘ	p_ghr;		// 'guiheadroom'
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURSOR_SHAPE
EXTERN char_u	*p_guicursor;	// 'guicursor'
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MOUSESHAPE
EXTERN char_u	*p_mouseshape;	// 'mouseshape'
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_GUI)
EXTERN char_u	*p_go;		// 'guioptions'
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_GUI_TABLINE)
EXTERN char_u	*p_gtl;		// 'guitablabel'
EXTERN char_u	*p_gtt;		// 'guitabtooltip'
#ಆದರೆ_ಅಂತ್ಯ
EXTERN char_u	*p_hf;		// 'helpfile'
EXTERN ದೀರ್ಘ	p_hh;		// 'helpheight'
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MULTI_LANG
EXTERN char_u	*p_hlg;		// 'helplang'
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಪೂರ್ಣಾಂಕ	p_hid;		// 'hidden'
EXTERN char_u	*p_hl;		// 'highlight'
EXTERN ಪೂರ್ಣಾಂಕ	p_hls;		// 'hlsearch'
EXTERN ದೀರ್ಘ	p_hi;		// 'history'
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
EXTERN ಪೂರ್ಣಾಂಕ	p_hkmap;	// 'hkmap'
EXTERN ಪೂರ್ಣಾಂಕ	p_hkmapp;	// 'hkmapp'
# ifdef FEAT_ARABIC
EXTERN ಪೂರ್ಣಾಂಕ	p_arshape;	// 'arabicshape'
# endif
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಪೂರ್ಣಾಂಕ	p_icon;		// 'icon'
EXTERN char_u	*p_iconstring;	// 'iconstring'
EXTERN ಪೂರ್ಣಾಂಕ	p_ic;		// 'ignorecase'
#ಆದರೆ defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
EXTERN char_u	*p_imak;	// 'imactivatekey'
# define IM_ON_THE_SPOT		0L
# define IM_OVER_THE_SPOT	1L
EXTERN ದೀರ್ಘ	p_imst;		// 'imstyle'
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_IMAGE
EXTERN char_u	*p_ipc;		// 'imageprotocol'
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_EVAL)
EXTERN char_u	*p_imaf;	// 'imactivatefunc'
EXTERN char_u	*p_imsf;	// 'imstatusfunc'
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಪೂರ್ಣಾಂಕ	p_imcmdline;	// 'imcmdline'
EXTERN ಪೂರ್ಣಾಂಕ	p_imdisable;	// 'imdisable'
EXTERN ದೀರ್ಘ	p_iminsert;	// 'iminsert'
EXTERN ದೀರ್ಘ	p_imsearch;	// 'imsearch'
EXTERN ಪೂರ್ಣಾಂಕ	p_inf;		// 'infercase'
#ಆದರೆ defined(FEAT_ಕಂಡುಹಿಡಿ_ID) && defined(FEAT_EVAL)
EXTERN char_u	*p_inex;	// 'includeexpr'
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಪೂರ್ಣಾಂಕ	p_is;		// 'incsearch'
#ಆದರೆ defined(FEAT_EVAL)
EXTERN char_u	*p_inde;	// 'indentexpr'
EXTERN char_u	*p_indk;	// 'indentkeys'
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಪೂರ್ಣಾಂಕ	p_im;		// 'insertmode'
EXTERN char_u	*p_isf;		// 'isfname'
EXTERN char_u	*p_isi;		// 'isident'
EXTERN char_u	*p_isk;		// 'iskeyword'
EXTERN char_u	*p_isp;		// 'isprint'
EXTERN ಪೂರ್ಣಾಂಕ	p_js;		// 'joinspaces'
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CRYPT
EXTERN char_u	*p_ಕೀಲಿ;		// 'key'
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_KEYMAP
EXTERN char_u	*p_keymap;	// 'keymap'
#ಆದರೆ_ಅಂತ್ಯ
EXTERN char_u	*p_kp;		// 'keywordprg'
EXTERN char_u	*p_km;		// 'keymodel'
EXTERN char_u	*p_kpc;		// 'keyprotocol'
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LANGMAP
EXTERN char_u	*p_langmap;	// 'langmap'
EXTERN ಪೂರ್ಣಾಂಕ	p_lnr;		// 'langnoremap'
EXTERN ಪೂರ್ಣಾಂಕ	p_lrm;		// 'langremap'
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_MENU) && defined(FEAT_MULTI_LANG)
EXTERN char_u	*p_lm;		// 'langmenu'
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
EXTERN ದೀರ್ಘ	p_linespace;	// 'linespace'
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಪೂರ್ಣಾಂಕ	p_lisp;		// 'lisp'
EXTERN char_u	*p_lop;		// 'lispoptions'
EXTERN char_u	*p_lispwords;	// 'lispwords'
EXTERN ದೀರ್ಘ	p_ls;		// 'laststatus'
EXTERN ದೀರ್ಘ	p_stal;		// 'showtabline'
EXTERN char_u	*p_lcs;		// 'listchars'

EXTERN ಪೂರ್ಣಾಂಕ	p_lz;		// 'lazyredraw'
EXTERN ಪೂರ್ಣಾಂಕ	p_lpl;		// 'loadplugins'
#ಆದರೆ defined(DYNAMIC_LUA)
EXTERN char_u	*p_luadll;	// 'luadll'
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಪೂರ್ಣಾಂಕ	p_magic;	// 'magic'
EXTERN char_u	*p_menc;	// 'makeencoding'
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
EXTERN char_u	*p_mef;		// 'makeef'
EXTERN char_u	*p_mp;		// 'makeprg'
#ಆದರೆ_ಅಂತ್ಯ
EXTERN char_u	*p_mps;		// 'matchpairs'
EXTERN ದೀರ್ಘ	p_mat;		// 'matchtime'
EXTERN ದೀರ್ಘ	p_mco;		// 'maxcombine'
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ದೀರ್ಘ	p_mfd;		// 'maxfuncdepth'
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ದೀರ್ಘ	p_mmd;		// 'maxmapdepth'
EXTERN ದೀರ್ಘ	p_mm;		// 'maxmem'
EXTERN ದೀರ್ಘ	p_mmp;		// 'maxmempattern'
EXTERN ದೀರ್ಘ	p_mmt;		// 'maxmemtot'
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU
EXTERN ದೀರ್ಘ	p_mis;		// 'menuitems'
#ಆದರೆ_ಅಂತ್ಯ
EXTERN char_u	*p_mopt;	// 'messagesopt'
EXTERN ದೀರ್ಘ	p_msc;		// 'maxsearchcount'
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SPELL
EXTERN char_u	*p_msm;		// 'mkspellmem'
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಪೂರ್ಣಾಂಕ	p_ml;		// 'modeline'
EXTERN ಪೂರ್ಣಾಂಕ	p_mle;		// 'modelineexpr'
EXTERN ದೀರ್ಘ	p_mls;		// 'modelines'
EXTERN ಪೂರ್ಣಾಂಕ	p_mlstr;	// 'modelinestrict'
EXTERN ಪೂರ್ಣಾಂಕ	p_ma;		// 'modifiable'
EXTERN ಪೂರ್ಣಾಂಕ	p_mod;		// 'modified'
EXTERN char_u	*p_mouse;	// 'mouse'
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
EXTERN ಪೂರ್ಣಾಂಕ	p_mousef;	// 'mousefocus'
EXTERN ಪೂರ್ಣಾಂಕ	p_mh;		// 'mousehide'
#ಆದರೆ_ಅಂತ್ಯ
EXTERN char_u	*p_mousem;	// 'mousemodel'
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
EXTERN ಪೂರ್ಣಾಂಕ	p_mousemev;	// 'mousemoveevent'
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ದೀರ್ಘ	p_mouset;	// 'mousetime'
EXTERN ಪೂರ್ಣಾಂಕ	p_ಇನ್ನಷ್ಟು;		// 'more'
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MZSCHEME
EXTERN ದೀರ್ಘ	p_mzq;		// 'mzquantum
# ಆದರೆ defined(DYNAMIC_MZSCHEME)
EXTERN char_u	*p_mzschemedll;	// 'mzschemedll'
EXTERN char_u	*p_mzschemegcdll; // 'mzschemegcdll'
# endif
#ಆದರೆ_ಅಂತ್ಯ
EXTERN char_u	*p_nf;		// 'nrformats'
#ಆದರೆ defined(MSWIN)
EXTERN ಪೂರ್ಣಾಂಕ	p_odev;		// 'opendevice'
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ದೀರ್ಘ	p_ost;		// 'osctimeoutlen'
EXTERN char_u	*p_opfunc;	// 'operatorfunc'
EXTERN char_u	*p_para;	// 'paragraphs'
EXTERN ಪೂರ್ಣಾಂಕ	p_paste;	// 'paste'
EXTERN char_u	*p_pt;		// 'pastetoggle'
#ಆದರೆ defined(FEAT_EVAL) && defined(FEAT_DIFF)
EXTERN char_u	*p_pex;		// 'patchexpr'
#ಆದರೆ_ಅಂತ್ಯ
EXTERN char_u	*p_pm;		// 'patchmode'
EXTERN char_u	*p_ಮಾರ್ಗ;	// 'path'
EXTERN char_u	*p_cdpath;	// 'cdpath'
#ಆದರೆ defined(DYNAMIC_PERL)
EXTERN char_u	*p_perldll;	// 'perldll'
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಪೂರ್ಣಾಂಕ	p_pi;		// 'preserveindent'
#ಆದರೆ defined(DYNAMIC_PYTHON3)
EXTERN char_u	*p_py3dll;	// 'pythonthreedll'
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PYTHON3
EXTERN char_u	*p_py3home;	// 'pythonthreehome'
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(DYNAMIC_PYTHON)
EXTERN char_u	*p_pydll;	// 'pythondll'
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PYTHON
EXTERN char_u	*p_pyhome;	// 'pythonhome'
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
EXTERN ದೀರ್ಘ	p_pyx;		// 'pyxversion'
#ಆದರೆ_ಅಂತ್ಯ
EXTERN char_u	*p_qe;		// 'quoteescape'
EXTERN ಪೂರ್ಣಾಂಕ	p_ro;		// 'readonly'
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RELTIME
EXTERN ದೀರ್ಘ	p_rdt;		// 'redrawtime'
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಪೂರ್ಣಾಂಕ	p_remap;	// 'remap'
EXTERN ದೀರ್ಘ	p_re;		// 'regexpengine'
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RENDER_OPTIONS
EXTERN char_u	*p_rop;		// 'renderoptions'
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ದೀರ್ಘ	p_report;	// 'report'
#ಆದರೆ defined(FEAT_QUICKFIX)
EXTERN ದೀರ್ಘ	p_pvh;		// 'previewheight'
EXTERN ದೀರ್ಘ     p_chi;          // 'chistory'
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
EXTERN ಪೂರ್ಣಾಂಕ	p_rs;		// 'restorescreen'
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
EXTERN ಪೂರ್ಣಾಂಕ	p_ari;		// 'allowrevins'
EXTERN ಪೂರ್ಣಾಂಕ	p_ri;		// 'revins'
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(DYNAMIC_RUBY)
EXTERN char_u	*p_rubydll;	// 'rubydll'
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಪೂರ್ಣಾಂಕ	p_ru;		// 'ruler'
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_STL_OPT
EXTERN char_u	*p_ruf;		// 'rulerformat'
#ಆದರೆ_ಅಂತ್ಯ
EXTERN char_u	*p_pp;		// 'packpath'
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
EXTERN char_u	*p_qftf;	// 'quickfixtextfunc'
#ಆದರೆ_ಅಂತ್ಯ
EXTERN char_u	*p_rtp;		// 'runtimepath'
EXTERN ದೀರ್ಘ	p_sj;		// 'scrolljump'
#ಆದರೆ defined(MSWIN) && defined(FEAT_GUI)
EXTERN ಪೂರ್ಣಾಂಕ	p_scf;		// 'scrollfocus'
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ದೀರ್ಘ	p_so;		// 'scrolloff'
EXTERN ದೀರ್ಘ	p_sop;		// 'scrolloffpad'
EXTERN char_u	*p_sbo;		// 'scrollopt'
EXTERN char_u	*p_sections;	// 'sections'
EXTERN ಪೂರ್ಣಾಂಕ	p_secure;	// 'secure'
EXTERN char_u	*p_sel;		// 'selection'
EXTERN char_u	*p_slm;		// 'selectmode'
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SESSION
EXTERN char_u	*p_ssop;	// 'sessionoptions'
EXTERN ಚಿಹ್ನೆರಹಿತ	ssop_flags;
# define SSOP_BUFFERS		0x001
# define SSOP_WINPOS		0x002
# define SSOP_RESIZE		0x004
# define SSOP_WINSIZE		0x008
# define SSOP_LOCALOPTIONS	0x010
# define SSOP_OPTIONS		0x020
# define SSOP_HELP		0x040
# define SSOP_BLANK		0x080
# define SSOP_GLOBALS		0x100
# define SSOP_SLASH		0x200
# define SSOP_UNIX		0x400
# define SSOP_SESDIR		0x800
# define SSOP_CURDIR		0x1000
# define SSOP_FOLDS		0x2000
# define SSOP_CURSOR		0x4000
# define SSOP_TABPAGES		0x8000
# define SSOP_TERMINAL		0x10000
# define SSOP_SKIP_RTP		0x20000
#ಆದರೆ_ಅಂತ್ಯ
EXTERN char_u	*p_sh;		// 'shell'
EXTERN char_u	*p_shcf;	// 'shellcmdflag'
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
EXTERN char_u	*p_sp;		// 'shellpipe'
#ಆದರೆ_ಅಂತ್ಯ
EXTERN char_u	*p_shq;		// 'shellquote'
EXTERN char_u	*p_sxq;		// 'shellxquote'
EXTERN char_u	*p_sxe;		// 'shellxescape'
EXTERN char_u	*p_srr;		// 'shellredir'
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ AMIGA
EXTERN ದೀರ್ಘ	p_st;		// 'shelltype'
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಪೂರ್ಣಾಂಕ	p_stmp;		// 'shelltemp'
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ BACKSLASH_ಒಳ_FILENAME
EXTERN ಪೂರ್ಣಾಂಕ	p_ssl;		// 'shellslash'
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_STL_OPT
EXTERN char_u	*p_stl;		// 'statusline'
EXTERN char_u	*p_stlo;	// 'statuslineopt'
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಪೂರ್ಣಾಂಕ	p_sr;		// 'shiftround'
EXTERN ದೀರ್ಘ	p_sw;		// 'shiftwidth'
EXTERN char_u	*p_shm;		// 'shortmess'
EXTERN ಪೂರ್ಣಾಂಕ	p_sn;		// 'shortname'
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
EXTERN char_u	*p_sbr;		// 'showbreak'
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಪೂರ್ಣಾಂಕ	p_sc;		// 'showcmd'
EXTERN char_u	*p_sloc;	// 'showcmdloc'
EXTERN ಪೂರ್ಣಾಂಕ	p_sft;		// 'showfulltag'
EXTERN ಪೂರ್ಣಾಂಕ	p_sm;		// 'showmatch'
EXTERN ಪೂರ್ಣಾಂಕ	p_smd;		// 'showmode'
EXTERN ದೀರ್ಘ	p_ss;		// 'sidescroll'
EXTERN ದೀರ್ಘ	p_siso;		// 'sidescrolloff'
EXTERN ಪೂರ್ಣಾಂಕ	p_scs;		// 'smartcase'
EXTERN ಪೂರ್ಣಾಂಕ	p_si;		// 'smartindent'
EXTERN ಪೂರ್ಣಾಂಕ	p_sta;		// 'smarttab'
EXTERN ದೀರ್ಘ	p_sts;		// 'softtabstop'
EXTERN ಪೂರ್ಣಾಂಕ	p_sb;		// 'splitbelow'
EXTERN char_u	*p_sua;		// 'suffixesadd'
EXTERN ಪೂರ್ಣಾಂಕ	p_swf;		// 'swapfile'
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
EXTERN ದೀರ್ಘ	p_smc;		// 'synmaxcol'
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ದೀರ್ಘ	p_tpm;		// 'tabpagemax'
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_STL_OPT
EXTERN char_u	*p_tal;		// 'tabline'
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN char_u	*p_tfu;		// 'tagfunc'
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SPELL
EXTERN char_u	*p_spc;		// 'spellcapcheck'
EXTERN char_u	*p_spf;		// 'spellfile'
EXTERN char_u	*p_spl;		// 'spelllang'
EXTERN char_u	*p_spo;		// 'spelloptions'
EXTERN char_u	*p_sps;		// 'spellsuggest'
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಪೂರ್ಣಾಂಕ	p_spr;		// 'splitright'
EXTERN ಪೂರ್ಣಾಂಕ	p_sol;		// 'startofline'
EXTERN char_u	*p_su;		// 'suffixes'
EXTERN char_u	*p_sws;		// 'swapsync'
EXTERN char_u	*p_swb;		// 'switchbuf'
EXTERN ಚಿಹ್ನೆರಹಿತ	swb_flags;
// Keep in sync with p_swb_values in optionstr.c
#ವ್ಯಾಖ್ಯಾನಿಸು SWB_USEOPEN		0x001
#ವ್ಯಾಖ್ಯಾನಿಸು SWB_USETAB		0x002
#ವ್ಯಾಖ್ಯಾನಿಸು SWB_ವಿಭಜಿಸು		0x004
#ವ್ಯಾಖ್ಯಾನಿಸು SWB_NEWTAB		0x008
#ವ್ಯಾಖ್ಯಾನಿಸು SWB_VSPLIT		0x010
#ವ್ಯಾಖ್ಯಾನಿಸು SWB_USELAST		0x020

EXTERN char_u	*p_spk;		// 'splitkeep'

#ಆದರೆ defined(FEAT_TABPANEL)
EXTERN char_u	*p_tpl;		// 'tabpanel'
EXTERN ದೀರ್ಘ	p_stpl;		// 'showtabpanel'
EXTERN char_u	*p_tplo;	// 'tabpanelopt'
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
EXTERN char_u	*p_syn;		// 'syntax'
#ಆದರೆ_ಅಂತ್ಯ
EXTERN char_u	*p_tcl;		// 'tabclose'
EXTERN ಚಿಹ್ನೆರಹಿತ	tcl_flags;	// flags from 'tabclose'
#ವ್ಯಾಖ್ಯಾನಿಸು TCL_ಎಡ		0x001
#ವ್ಯಾಖ್ಯಾನಿಸು TCL_USELAST		0x002
EXTERN ದೀರ್ಘ	p_ts;		// 'tabstop'
EXTERN ಪೂರ್ಣಾಂಕ	p_tbs;		// 'tagbsearch'
EXTERN char_u	*p_tc;		// 'tagcase'
EXTERN ಚಿಹ್ನೆರಹಿತ tc_flags;       // flags from 'tagcase'
#ವ್ಯಾಖ್ಯಾನಿಸು TC_FOLLOWIC		0x01
#ವ್ಯಾಖ್ಯಾನಿಸು TC_IGNORE		0x02
#ವ್ಯಾಖ್ಯಾನಿಸು TC_ಹೊಂದಿಕೆ		0x04
#ವ್ಯಾಖ್ಯಾನಿಸು TC_FOLLOWSCS		0x08
#ವ್ಯಾಖ್ಯಾನಿಸು TC_SMART		0x10
EXTERN ದೀರ್ಘ	p_tl;		// 'taglength'
EXTERN ಪೂರ್ಣಾಂಕ	p_tr;		// 'tagrelative'
EXTERN char_u	*p_ಟ್ಯಾಗುಗಳು;	// 'tags'
EXTERN ಪೂರ್ಣಾಂಕ	p_tagsecure;    // 'tagsecure'
EXTERN ಪೂರ್ಣಾಂಕ	p_tgst;		// 'tagstack'
#ಆದರೆ defined(DYNAMIC_TCL)
EXTERN char_u	*p_tcldll;	// 'tcldll'
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ARABIC
EXTERN ಪೂರ್ಣಾಂಕ	p_tbidi;	// 'termbidi'
#ಆದರೆ_ಅಂತ್ಯ
EXTERN char_u	*p_tenc;	// 'termencoding'
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMGUICOLORS
EXTERN ಪೂರ್ಣಾಂಕ	p_tgc;		// 'termguicolors'
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
EXTERN char_u	*p_trz;		// 'termresize'
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಪೂರ್ಣಾಂಕ	p_tsy;		// 'termsync'
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
EXTERN ದೀರ್ಘ	p_twsl;		// 'termwinscroll'
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(MSWIN) && defined(FEAT_TERMINAL)
EXTERN char_u	*p_twt;		// 'termwintype'
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಪೂರ್ಣಾಂಕ	p_terse;	// 'terse'
EXTERN ಪೂರ್ಣಾಂಕ	p_ta;		// 'textauto'
EXTERN ಪೂರ್ಣಾಂಕ	p_tx;		// 'textmode'
EXTERN ದೀರ್ಘ	p_tw;		// 'textwidth'
EXTERN ಪೂರ್ಣಾಂಕ	p_ಗಮ್ಯ;		// 'tildeop'
EXTERN ಪೂರ್ಣಾಂಕ	p_timeout;	// 'timeout'
EXTERN ದೀರ್ಘ	p_tm;		// 'timeoutlen'
EXTERN ಪೂರ್ಣಾಂಕ	p_title;	// 'title'
EXTERN ದೀರ್ಘ	p_titlelen;	// 'titlelen'
EXTERN char_u	*p_titleold;	// 'titleold'
EXTERN char_u	*p_titlestring;	// 'titlestring'
EXTERN char_u	*p_tsr;		// 'thesaurus'
EXTERN ಪೂರ್ಣಾಂಕ	p_ttimeout;	// 'ttimeout'
EXTERN ದೀರ್ಘ	p_ttm;		// 'ttimeoutlen'
EXTERN ಪೂರ್ಣಾಂಕ	p_tbi;		// 'ttybuiltin'
EXTERN ಪೂರ್ಣಾಂಕ	p_tf;		// 'ttyfast'
#ಆದರೆ defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
EXTERN char_u	*p_toolbar;	// 'toolbar'
EXTERN ಚಿಹ್ನೆರಹಿತ toolbar_flags;
# define TOOLBAR_ಪಠ್ಯ		0x01
# define TOOLBAR_ICONS		0x02
# define TOOLBAR_TOOLTIPS	0x04
# define TOOLBAR_HORIZ		0x08
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_TOOLBAR) && defined(FEAT_GUI_GTK)
EXTERN char_u	*p_tbis;	// 'toolbariconsize'
EXTERN ಚಿಹ್ನೆರಹಿತ tbis_flags;
# define TBIS_TINY		0x01
# define TBIS_SMALL		0x02
# define TBIS_MEDIUM		0x04
# define TBIS_LARGE		0x08
# define TBIS_HUGE		0x10
# define TBIS_GIANT		0x20
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ದೀರ್ಘ	p_ttyscroll;	// 'ttyscroll'
#ಆದರೆ defined(UNIX) || defined(VMS)
EXTERN char_u	*p_ttym;	// 'ttymouse'
EXTERN ಚಿಹ್ನೆರಹಿತ ttym_flags;
# define TTYM_XTERM		0x01
# define TTYM_XTERM2		0x02
# define TTYM_DEC		0x04
# define TTYM_NETTERM		0x08
# define TTYM_JSBTERM		0x10
# define TTYM_PTERM		0x20
# define TTYM_URXVT		0x40
# define TTYM_SGR		0x80
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PERSISTENT_UNDO
EXTERN char_u	*p_udir;	// 'undodir'
EXTERN ಪೂರ್ಣಾಂಕ	p_udf;		// 'undofile'
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ದೀರ್ಘ	p_ul;		// 'undolevels'
EXTERN ದೀರ್ಘ	p_ur;		// 'undoreload'
EXTERN ದೀರ್ಘ	p_uc;		// 'updatecount'
EXTERN ದೀರ್ಘ	p_ut;		// 'updatetime'
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VARTABS
EXTERN char_u	*p_vsts;	// 'varsofttabstop'
EXTERN char_u	*p_vts;		// 'vartabstop'
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VIMINFO
EXTERN char_u	*p_viminfo;	// 'viminfo'
EXTERN char_u	*p_viminfofile;	// 'viminfofile'
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SESSION
EXTERN char_u	*p_vdir;	// 'viewdir'
EXTERN char_u	*p_vop;		// 'viewoptions'
EXTERN ಚಿಹ್ನೆರಹಿತ	vop_flags;	// uses SSOP_ flags
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಪೂರ್ಣಾಂಕ	p_vb;		// 'visualbell'
EXTERN char_u	*p_ve;		// 'virtualedit'
EXTERN ಚಿಹ್ನೆರಹಿತ ve_flags;
#ವ್ಯಾಖ್ಯಾನಿಸು VE_ಖಂಡ	5	// includes "all"
#ವ್ಯಾಖ್ಯಾನಿಸು VE_ಸೇರಿಸು	6	// includes "all"
#ವ್ಯಾಖ್ಯಾನಿಸು VE_ALL		4
#ವ್ಯಾಖ್ಯಾನಿಸು VE_ONEMORE	8
#ವ್ಯಾಖ್ಯಾನಿಸು VE_NONE		16	// "none"
#ವ್ಯಾಖ್ಯಾನಿಸು VE_NONEU	32      // "NONE"
EXTERN ದೀರ್ಘ	p_verbose;	// 'verbose'
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ಒಳ_OPTION_C
char_u	*p_vfile = (char_u *)""; // used before options are initialized
#ಇಲ್ಲದಿದ್ದರೆ
ಬಾಹ್ಯ char_u	*p_vfile;	// 'verbosefile'
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಪೂರ್ಣಾಂಕ	p_warn;		// 'warn'
EXTERN char_u	*p_wop;		// 'wildoptions'
EXTERN ದೀರ್ಘ	p_ಕಿಟಕಿ;	// 'window'
#ಆದರೆ defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_MOTIF) || defined(LINT) \
	|| defined (FEAT_GUI_GTK) || defined(FEAT_GUI_PHOTON)
# define FEAT_WAK
EXTERN char_u	*p_wak;		// 'winaltkeys'
#ಆದರೆ_ಅಂತ್ಯ
EXTERN char_u	*p_wig;		// 'wildignore'
EXTERN ಪೂರ್ಣಾಂಕ	p_wiv;		// 'weirdinvert'
EXTERN char_u	*p_ww;		// 'whichwrap'
EXTERN ದೀರ್ಘ	p_wc;		// 'wildchar'
EXTERN ದೀರ್ಘ	p_wcm;		// 'wildcharm'
EXTERN ಪೂರ್ಣಾಂಕ	p_wic;		// 'wildignorecase'
EXTERN char_u	*p_wim;		// 'wildmode'
EXTERN ಪೂರ್ಣಾಂಕ	p_wmnu;		// 'wildmenu'
EXTERN ದೀರ್ಘ	p_wh;		// 'winheight'
EXTERN ದೀರ್ಘ	p_wmh;		// 'winminheight'
EXTERN ದೀರ್ಘ	p_wmw;		// 'winminwidth'
EXTERN ದೀರ್ಘ	p_wiw;		// 'winwidth'
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_WAYLAND
EXTERN char_u	*p_wse;		// 'wlseat'
EXTERN ದೀರ್ಘ     p_wtm;		// 'wltimeoutlen'
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(MSWIN) && defined(FEAT_TERMINAL)
EXTERN char_u	*p_winptydll;	// 'winptydll'
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ದೀರ್ಘ	p_wm;		// 'wrapmargin'
EXTERN ಪೂರ್ಣಾಂಕ	p_ws;		// 'wrapscan'
EXTERN ಪೂರ್ಣಾಂಕ	p_write;	// 'write'
EXTERN ಪೂರ್ಣಾಂಕ	p_wa;		// 'writeany'
EXTERN ಪೂರ್ಣಾಂಕ	p_wb;		// 'writebackup'
EXTERN ದೀರ್ಘ	p_wd;		// 'writedelay'
EXTERN ಪೂರ್ಣಾಂಕ	p_xtermcodes;	// 'xtermcodes'

/*
 * "indir" values for buffer-local options.
 * These need to be defined globally, so that the BV_COUNT can be used with
 * b_p_scriptID[].
 */
ಎಣಿಕೆ
{
    BV_AI = 0
    , BV_AC
    , BV_AR
    , BV_BH
    , BV_BKC
    , BV_BT
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
    , BV_EFM
    , BV_GEFM
    , BV_GP
    , BV_MP
#ಆದರೆ_ಅಂತ್ಯ
    , BV_BIN
    , BV_BL
    , BV_BOMB
    , BV_CI
    , BV_CIN
    , BV_CINK
    , BV_CINO
    , BV_CINSD
    , BV_CINW
    , BV_CM
    , BV_CMS
    , BV_COM
    , BV_COT
    , BV_CPT
    , BV_DICT
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
    , BV_DIA
#ಆದರೆ_ಅಂತ್ಯ
    , BV_TSR
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ BACKSLASH_ಒಳ_FILENAME
    , BV_CSL
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_COMPL_FUNC
    , BV_CFU
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕಂಡುಹಿಡಿ_ID
    , BV_DEF
    , BV_INC
#ಆದರೆ_ಅಂತ್ಯ
    , BV_EOF
    , BV_EOL
    , BV_FIXEOL
    , BV_EP
    , BV_ET
    , BV_FENC
    , BV_FP
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    , BV_BEXPR
    , BV_FEX
    , BV_FFU
#ಆದರೆ_ಅಂತ್ಯ
    , BV_FF
    , BV_FLP
    , BV_FO
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_FSYNC
    , BV_FS
#ಆದರೆ_ಅಂತ್ಯ
    , BV_FT
    , BV_IMI
    , BV_IMS
#ಆದರೆ defined(FEAT_EVAL)
    , BV_INDE
    , BV_INDK
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_ಕಂಡುಹಿಡಿ_ID) && defined(FEAT_EVAL)
    , BV_INEX
#ಆದರೆ_ಅಂತ್ಯ
    , BV_INF
    , BV_ISE
    , BV_ISK
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CRYPT
    , BV_ಕೀಲಿ
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_KEYMAP
    , BV_KMAP
#ಆದರೆ_ಅಂತ್ಯ
    , BV_KP
    , BV_LISP
    , BV_LOP
    , BV_LW
    , BV_MENC
    , BV_MA
    , BV_ML
    , BV_MOD
    , BV_MPS
    , BV_NF
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_COMPL_FUNC
    , BV_OFU
#ಆದರೆ_ಅಂತ್ಯ
    , BV_ಮಾರ್ಗ
    , BV_PI
    , BV_QE
    , BV_RO
    , BV_SI
    , BV_SN
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
    , BV_SMC
    , BV_SYN
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SPELL
    , BV_SPC
    , BV_SPF
    , BV_SPL
    , BV_SPO
#ಆದರೆ_ಅಂತ್ಯ
    , BV_STS
    , BV_SUA
    , BV_SW
    , BV_SWF
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    , BV_TFU
#ಆದರೆ_ಅಂತ್ಯ
    , BV_ಟ್ಯಾಗುಗಳು
    , BV_TC
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_COMPL_FUNC
    , BV_TSRFU
#ಆದರೆ_ಅಂತ್ಯ
    , BV_TS
    , BV_TW
    , BV_TX
    , BV_UDF
    , BV_UL
    , BV_WM
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
    , BV_TWSL
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VARTABS
    , BV_VSTS
    , BV_VTS
#ಆದರೆ_ಅಂತ್ಯ
    , BV_ಎಣಿಕೆ	    // must be the last one
};

/*
 * "indir" values for window-local options.
 * These need to be defined globally, so that the WV_COUNT can be used in the
 * window structure.
 */
ಎಣಿಕೆ
{
    WV_LIST = 0
    , WV_LCS
    , WV_FCS
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ARABIC
    , WV_ARAB
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CONCEAL
    , WV_COCU
    , WV_COLE
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
    , WV_TWK
    , WV_TWS
#ಆದರೆ_ಅಂತ್ಯ
    , WV_CRBIND
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
    , WV_BRI
    , WV_BRIOPT
#ಆದರೆ_ಅಂತ್ಯ
    , WV_WCR
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
    , WV_DIFF
#ಆದರೆ_ಅಂತ್ಯ
    , WV_EIW
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
    , WV_FDC
    , WV_FEN
    , WV_FDI
    , WV_FDL
    , WV_FDM
    , WV_FML
    , WV_FDN
# ifdef FEAT_EVAL
    , WV_FDE
    , WV_FDT
# endif
    , WV_FMR
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
    , WV_LBR
#ಆದರೆ_ಅಂತ್ಯ
    , WV_NU
    , WV_RNU
    , WV_VE
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
    , WV_NUW
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_QUICKFIX)
    , WV_PVW
    , WV_LHI
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
    , WV_RL
    , WV_RLC
#ಆದರೆ_ಅಂತ್ಯ
    , WV_SCBIND
    , WV_SCROLL
    , WV_SMS
    , WV_SISO
    , WV_SO
    , WV_SOP
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SPELL
    , WV_SPELL
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
    , WV_CUC
    , WV_CUL
    , WV_CULOPT
    , WV_CC
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
    , WV_SBR
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_STL_OPT
    , WV_STL
    , WV_STLO
#ಆದರೆ_ಅಂತ್ಯ
    , WV_WFB
    , WV_WFH
    , WV_WFW
    , WV_WHL
    , WV_WRAP
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SIGNS
    , WV_SCL
#ಆದರೆ_ಅಂತ್ಯ
    , WV_ಎಣಿಕೆ	    // must be the last one
};

// Value for b_p_ul indicating the global value must be used.
#ವ್ಯಾಖ್ಯಾನಿಸು NO_LOCAL_UNDOLEVEL (-123456)

#ವ್ಯಾಖ್ಯಾನಿಸು ERR_BUFLEN 80

#ಆದರೆ_ಅಂತ್ಯ // _OPTION_H_
