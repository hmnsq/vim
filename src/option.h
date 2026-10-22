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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ _OPTION_H_
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _OPTION_H_

//
// Option Flags
//
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ P_BOOL		0x01	// the option is boolean
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ P_ਅੰਕ		0x02	// the option is numeric
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ P_STRING	0x04	// the option is a string
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ P_ALLOCED	0x08	// the string option is in allocated memory,
				// must use free_string_option() when
				// assigning new value. Not set if default is
				// the same.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ P_EXPAND	0x10	// environment expansion.  NOTE: P_EXPAND can
				// never be used for local or hidden options!
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ P_NO_CMD_EXPAND	0x20	// don't perform cmdline completions
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ P_NODEFAULT	0x40	// don't set to default value
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ P_DEF_ALLOCED	0x80	// default value is in allocated memory, must
				//  use vim_free() when assigning new value
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ P_WAS_ਸੈੱਟ	0x100	// option has been set/reset
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ P_NO_MKRC	0x200	// don't include in :mkvimrc output
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ P_VI_DEF	0x400	// Use Vi default for Vim
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ P_VIM		0x800	// Vim option, reset when 'cp' set

				// when option changed, what to display:
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ P_RSTAT		0x1000	// redraw status lines
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ P_RWIN		0x2000	// redraw current window and recompute text
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ P_RBUF		0x4000	// redraw current buffer and recompute text
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ P_RALL		0x6000	// redraw all windows and recompute text
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ P_RCLR		0x7000	// clear and redraw all and recompute text

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ P_COMMA		 0x8000	 // comma separated list
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ P_ONECOMMA	0x18000L // P_COMMA and cannot have two consecutive
				 // commas
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ P_NODUP		0x20000L // don't allow duplicate strings
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ P_FLAGLIST	0x40000L // list of single-char flags

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ P_SECURE	0x80000L // cannot change in modeline or secure mode
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ P_GETTEXT      0x100000L // expand default value with _()
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ P_NOGLOB       0x200000L // do not use local value for global vimrc
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ P_NFNAME       0x400000L // only normal file name chars allowed
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ P_INSECURE     0x800000L // option was set from a modeline
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ P_PRI_MKRC    0x1000000L // priority for :mkvimrc (setting option has
				 // side effects)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ P_NO_ML       0x2000000L // not allowed in modeline
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ P_CURSWANT    0x4000000L // update curswant required; not needed when
				 // there is a redraw flag
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ P_NDNAME      0x8000000L // only normal dir name chars allowed
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ P_HLONLY     0x10000000L // option only changes highlight, not text
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ P_MLE	     0x20000000L // under control of 'modelineexpr'
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ P_FUNC	     0x40000000L // accept a function reference or a lambda
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ P_COLON	     0x80000000L // values use colons to create sublists
// Warning: Currently we have used all 32 bits for option flags. On some 32-bit
//          systems, the flags are stored as a 32-bit integer, and adding more
//          flags will overflow it. Adding another flag will need to change how
//          it's stored first.

// Returned by get_option_value().
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
    gov_unknown,
    gov_bool,
    gov_ਸੰਖਿਆ,
    gov_string,
    gov_hidden_bool,
    gov_hidden_ਸੰਖਿਆ,
    gov_hidden_string
} getoption_T;

/*
 * Default values for 'errorformat'.
 * The "%f|%l| %m" one is used for when the contents of the quickfix window is
 * written to a file.
 */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ AMIGA
# define DFLT_EFM	"%f>%l:%c:%t:%n:%m,%f:%l: %t%*\\D%n: %m,%f %l %t%*\\D%n: %m,%*[^\"]\"%f\"%*\\D%l: %m,%f:%l:%m,%f|%l| %m"
#ਨਹੀਂਤਾਂ
# ਜੇ defined(MSWIN)
#  define DFLT_EFM	"%f(%l): %t%*\\D%n: %m,%f(%l\\,%c): %t%*\\D%n: %m,%f(%l) \\=: %t%*\\D%n: %m,%*[^\"]\"%f\"%*\\D%l: %m,%f(%l) \\=: %m,%*[^ ] %f %l: %m,%f:%l:%c:%m,%f(%l):%m,%f:%l:%m,%f|%l| %m"
# ਨਹੀਂਤਾਂ
#  ਜੇ defined(__QNX__)
#   define DFLT_EFM	"%f(%l):%*[^WE]%t%*\\D%n:%m,%f|%l| %m"
#  ਨਹੀਂਤਾਂ
#   ifdef VMS
#    define DFLT_EFM	"%A%p^,%C%%CC-%t-%m,%Cat line number %l in file %f,%f|%l| %m"
#   ਨਹੀਂਤਾਂ // Unix, probably
#    define DFLT_EFM	"%*[^\"]\"%f\"%*\\D%l: %m,\"%f\"%*\\D%l: %m,%-Gg%\\?make[%*\\d]: *** [%f:%l:%m,%-Gg%\\?make: *** [%f:%l:%m,%-G%f:%l: (Each undeclared identifier is reported only once,%-G%f:%l: for each function it appears in.),%-GIn file included from %f:%l:%c:,%-GIn file included from %f:%l:%c\\,,%-GIn file included from %f:%l:%c,%-GIn file included from %f:%l,%-G%*[ ]from %f:%l:%c,%-G%*[ ]from %f:%l:,%-G%*[ ]from %f:%l\\,,%-G%*[ ]from %f:%l,%f:%l:%c:%m,%f(%l):%m,%f:%l:%m,\"%f\"\\, line %l%*\\D%c%*[^ ] %m,%D%*\\a[%*\\d]: Entering directory %*[`']%f',%X%*\\a[%*\\d]: Leaving directory %*[`']%f',%D%*\\a: Entering directory %*[`']%f',%X%*\\a: Leaving directory %*[`']%f',%DMaking %*\\a in %f,%f|%l| %m"
#   endif
#  endif
# endif
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DFLT_GREPFORMAT	"%f:%l:%m,%f:%l%m,%f  %l%m"

// default values for b_p_ff 'fileformat' and p_ffs 'fileformats'
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FF_DOS		"dos"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FF_MAC		"mac"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FF_UNIX		"unix"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_CRNL
# define DFLT_FF	"dos"
# define DFLT_FFS_VIM	"dos,unix"
# define DFLT_FFS_VI	"dos,unix"	// also autodetect in compatible mode
# define DFLT_TEXTAUTO	TRUE
#ਨਹੀਂਤਾਂ
# define DFLT_FF	"unix"
# define DFLT_FFS_VIM	"unix,dos"
# ifdef __CYGWIN__
#  define DFLT_FFS_VI	"unix,dos"	// Cygwin always needs file detection
#  define DFLT_TEXTAUTO TRUE
# ਨਹੀਂਤਾਂ
#  define DFLT_FFS_VI	""
#  define DFLT_TEXTAUTO FALSE
# endif
#ਜੇ_ਅੰਤ


// Possible values for 'encoding'
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ENC_UCSBOM	"ucs-bom"	// check for BOM at start of file

// default value for 'encoding'
#ਜੇ defined(MSWIN) || defined(__MVS__)
# define ENC_DFLT	"utf-8"
#ਨਹੀਂਤਾਂ
# define ENC_DFLT	"latin1"
#ਜੇ_ਅੰਤ

// end-of-line style
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EOL_UNKNOWN	(-1)	// not defined yet
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EOL_UNIX	0	// NL
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EOL_DOS		1	// CR NL
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EOL_MAC		2	// CR

// Formatting options for p_fo 'formatoptions'
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FO_WRAP		't'
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FO_WRAP_COMS	'c'
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FO_RET_COMS	'r'
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FO_OPEN_COMS	'o'
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FO_NO_OPEN_COMS	'/'
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FO_Q_COMS	'q'
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FO_Q_ਸੰਖਿਆ	'n'
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FO_Q_ਦੂਜਾ	'2'
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FO_INS_VI	'v'
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FO_INS_LONG	'l'
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FO_INS_BLANK	'b'
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FO_MBYTE_BREAK	'm'	// break before/after multi-byte char
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FO_MBYTE_JOIN	'M'	// no space before/after multi-byte char
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FO_MBYTE_JOIN2	'B'	// no space between multi-byte chars
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FO_ONE_LETTER	'1'
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FO_WHITE_PAR	'w'	// trailing white space continues paragr.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FO_AUTO		'a'	// automatic formatting
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FO_RIGOROUS_TW	']'     // respect textwidth rigorously
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FO_REMOVE_COMS	'j'	// remove comment leaders when joining lines
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FO_PERIOD_ABBR	'p'	// don't break a single space after a period

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DFLT_FO_VI	"vt"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DFLT_FO_VIM	"tcq"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FO_ALL		"tcro/q2vlb1mMBn,aw]jp"	// for do_set()

// characters for the p_cpo option:
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CPO_ALTREAD	'a'	// ":read" sets alternate file name
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CPO_ALTWRITE	'A'	// ":write" sets alternate file name
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CPO_BAR		'b'	// "\|" ends a mapping
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CPO_BSLASH	'B'	// backslash in mapping is not special
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CPO_ਖੋਜ	'c'
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CPO_CONCAT	'C'	// Don't concatenate sourced lines
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CPO_DOTTAG	'd'	// "./tags" in 'tags' is in current dir
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CPO_DIGRAPH	'D'	// No digraph after "r", "f", etc.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CPO_EXECBUF	'e'
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CPO_EMPTYREGION	'E'	// operating on empty region is an error
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CPO_FNAMER	'f'	// set file name for ":r file"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CPO_FNAMEW	'F'	// set file name for ":w file"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CPO_GOTO1	'g'	// goto line 1 for ":edit"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CPO_INSEND	'H'	// "I" inserts before last blank in line
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CPO_INTMOD	'i'	// interrupt a read makes buffer modified
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CPO_INDENT	'I'	// remove auto-indent more often
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CPO_JOINSP	'j'	// only use two spaces for join after '.'
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CPO_ENDOFSENT	'J'	// need two spaces to detect end of sentence
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CPO_KEYCODE	'k'	// don't recognize raw key code in mappings
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CPO_KOFFSET	'K'	// don't wait for key code in mappings
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CPO_LITERAL	'l'	// take char after backslash in [] literal
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CPO_LISTWM	'L'	// 'list' changes wrapmargin
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CPO_SHOWMATCH	'm'
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CPO_MATCHBSL	'M'	// "%" ignores use of backslashes
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CPO_NUMCOL	'n'	// 'number' column also used for text
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CPO_LINEOFF	'o'
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CPO_OVERNEW	'O'	// silently overwrite new file
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CPO_LISP	'p'	// 'lisp' indenting
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CPO_FNAMEAPP	'P'	// set file name for ":w >>file"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CPO_JOINCOL	'q'	// with "3J" use column after first join
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CPO_REDO	'r'
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CPO_REMMARK	'R'	// remove marks when filtering
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CPO_BUFOPT	's'
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CPO_BUFOPTGLOB	'S'
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CPO_TAGPAT	't'	// tag pattern is used for "n"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CPO_UNDO	'u'	// "u" undoes itself
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CPO_BACKSPACE	'v'	// "v" keep deleted text
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CPO_CW		'w'	// "cw" only changes one blank
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CPO_FWRITE	'W'	// "w!" doesn't overwrite readonly files
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CPO_ESC		'x'
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CPO_REPLCNT	'X'	// "R" with a count only deletes chars once
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CPO_YANK	'y'
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CPO_KEEPRO	'Z'	// don't reset 'readonly' on ":w!"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CPO_ਸ਼ਬਦ	'z'	// do not special-case word motions cw and dw
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CPO_DOLLAR	'$'
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CPO_FILTER	'!'
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CPO_ਮੇਲ	'%'
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CPO_ਤਾਰਾ	'*'	// ":*" means ":@"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CPO_PLUS	'+'	// ":write file" resets 'modified'
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CPO_MINUS	'-'	// "9-" fails at and before line 9
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CPO_SPECI	'<'	// don't recognize <> in mappings
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CPO_REGAPPEND	'>'	// insert NL when appending to a register
// POSIX flags
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CPO_HASH	'#'	// "D", "o" and "O" do not use a count
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CPO_PARA	'{'	// "{" is also a paragraph boundary
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CPO_TSIZE	'|'	// $LINES and $COLUMNS overrule term size
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CPO_PRESERVE	'&'	// keep swap file after :preserve
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CPO_SUBPERCENT	'/'	// % in :s string uses previous one
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CPO_BACKSL	'\\'	// \ is not special in []
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CPO_CHDIR	'.'	// don't chdir if buffer is modified
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CPO_SCOLON	';'	// using "," and ";" will skip over char if
				// cursor would not move
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CPO_NOSYMLINKS	'~'	// don't resolve symlinks when changing directory
// default values for Vim, Vi and POSIX
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CPO_VIM		"aABceFsz"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CPO_VI		"aAbBcCdDeEfFgHiIjJkKlLmMnoOpPqrRsStuvwWxXyZz$!%*-+<>;"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CPO_ALL		"aAbBcCdDeEfFgHiIjJkKlLmMnoOpPqrRsStuvwWxXyZz$!%*-+<>#{|&/\\.;~"

// characters for p_ww option:
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WW_ALL		"bshl<>[]~"

// characters for p_mouse option:
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MOUSE_ਸਾਧਾਰਨ	'n'		// use mouse in Normal mode
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MOUSE_VISUAL	'v'		// use mouse in Visual/Select mode
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MOUSE_ਪਾਓ	'i'		// use mouse in Insert mode
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MOUSE_COMMAND	'c'		// use mouse in Command-line mode
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MOUSE_HELP	'h'		// use mouse in help buffers
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MOUSE_RETURN	'r'		// use mouse for hit-return message
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MOUSE_A		"nvich"		// used for 'a' flag
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MOUSE_ALL	"anvichr"	// all possible characters
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MOUSE_NONE	' '		// don't use Visual selection
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MOUSE_NONEF	'x'		// forced modeless selection

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ COCU_ALL	"nvic"		// flags for 'concealcursor'

// characters for p_shm option:
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SHM_RO		'r'		// readonly
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SHM_MOD		'm'		// modified
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SHM_FILE	'f'		// (file 1 of 2)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SHM_ਆਖਰੀ	'i'		// last line incomplete
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SHM_ਪਾਠ	'x'		// tx instead of textmode
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SHM_ਸਤਰਾਂ	'l'		// "L" instead of "lines"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SHM_NEW		'n'		// "[New]" instead of "[New file]"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SHM_WRI		'w'		// "[w]" instead of "written"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SHM_A		"rmfixlnw"	// represented by 'a' flag
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SHM_WRITE	'W'		// don't use "written" at all
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SHM_TRUNC	't'		// truncate file messages
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SHM_TRUNCALL	'T'		// truncate all messages
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SHM_OVER	'o'		// overwrite file messages
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SHM_OVERALL	'O'		// overwrite more messages
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SHM_ਖੋਜ	's'		// no search hit bottom messages
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SHM_ATTENTION	'A'		// no ATTENTION messages
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SHM_INTRO	'I'		// intro messages
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SHM_COMPLETIONMENU  'c'		// completion menu messages
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SHM_COMPLETIONSCAN  'C'		// completion scanning messages
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SHM_RECORDING	'q'		// short recording message
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SHM_FILEINFO	'F'		// no file info messages
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SHM_SEARCHCOUNT  'S'		// no search stats: '[1/10]'
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SHM_UNDO	'u'		// undo messages
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SHM_POSIX       "AS"		// POSIX value
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SHM_ALL		"rmfixlnwaWtToOsAIcCqFSu" // all possible flags for 'shm'
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SHM_LEN		30		// max length of all flags together
					// plus a NUL character

// characters for p_go:
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GO_TERMINAL	'!'		// use terminal for system commands
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GO_ASEL		'a'		// autoselect
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GO_ASELML	'A'		// autoselect modeless selection
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GO_BOT		'b'		// use bottom scrollbar
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GO_CONDIALOG	'c'		// use console dialog
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GO_TITLEBAR	'C'		// use 'hl-TitleBar'
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GO_DARKTHEME	'd'		// use dark theme variant
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GO_TABLINE	'e'		// may show tabline
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GO_FORG		'f'		// start GUI in foreground
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GO_GREY		'g'		// use grey menu items
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GO_HORSCROLL	'h'		// flexible horizontal scrolling
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GO_ICON		'i'		// use Vim icon
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GO_ਖੱਬਾ		'l'		// use left scrollbar
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GO_VLEFT	'L'		// left scrollbar with vert split
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GO_MENUS	'm'		// use menu bar
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GO_NOSYSMENU	'M'		// don't source system menu
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GO_ਸੂਚਕ	'p'		// pointer enter/leave callbacks
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GO_ASELPLUS	'P'		// autoselectPlus
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GO_ਸੱਜਾ	'r'		// use right scrollbar
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GO_VRIGHT	'R'		// right scrollbar with vert split
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GO_FULLSCREEN	's'		// enter fullscreen
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GO_TEAROFF	't'		// add tear-off menu items
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GO_TOOLBAR	'T'		// add toolbar
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GO_FOOTER	'F'		// add footer
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GO_VERTICAL	'v'		// arrange dialog buttons vertically
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GO_KEEPWINSIZE	'k'		// keep GUI window size
// all possible flags for 'go'
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GO_ALL		"!aAbcCdefFghilLmMpPrRstTvk"

// flags for 'comments' option
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ COM_NEST	'n'		// comments strings nest
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ COM_BLANK	'b'		// needs blank after string
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ COM_ਸ਼ੁਰੂ	's'		// start of comment
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ COM_ਵਿਚਕਾਰ	'm'		// middle of comment
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ COM_ਸਿਰਾ		'e'		// end of comment
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ COM_AUTO_ਸਿਰਾ	'x'		// last char of end closes comment
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ COM_ਪਹਿਲਾ	'f'		// first line comment only
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ COM_ਖੱਬਾ	'l'		// left adjusted
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ COM_ਸੱਜਾ	'r'		// right adjusted
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ COM_NOBACK	'O'		// don't use for "O" command
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ COM_ALL		"nbsmexflrO"	// all flags for 'comments' option
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ COM_MAX_LEN	50		// maximum length of a part

// flags for 'statusline' option
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STL_FILEPATH	'f'		// path of file in buffer
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STL_FULLPATH	'F'		// full path of file in buffer
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STL_FILENAME	't'		// last part (tail) of file path
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STL_ਕਾਲਮ	'c'		// column og cursor
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STL_VIRTCOL	'v'		// virtual column
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STL_VIRTCOL_ALT	'V'		// - with 'if different' display
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STL_ਸਤਰ	'l'		// line number of cursor
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STL_NUMLINES	'L'		// number of lines in buffer
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STL_BUFNO	'n'		// current buffer number
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STL_KEYMAP	'k'		// 'keymap' when active
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STL_ਵਿੱਥ	'o'		// offset of character under cursor
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STL_ਵਿੱਥ_X	'O'		// - in hexadecimal
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STL_BYTEVAL	'b'		// byte value of character
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STL_BYTEVAL_X	'B'		// - in hexadecimal
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STL_ROFLAG	'r'		// readonly flag
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STL_ROFLAG_ALT	'R'		// - other display
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STL_HELPFLAG	'h'		// window is showing a help file
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STL_HELPFLAG_ALT 'H'		// - other display
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STL_FILETYPE	'y'		// 'filetype'
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STL_FILETYPE_ALT 'Y'		// - other display
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STL_PREVIEWFLAG	'w'		// window is showing the preview buf
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STL_PREVIEWFLAG_ALT 'W'		// - other display
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STL_MODIFIED	'm'		// modified flag
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STL_MODIFIED_ALT 'M'		// - other display
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STL_QUICKFIX	'q'		// quickfix window description
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STL_PERCENTAGE	'p'		// percentage through file
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STL_ALTPERCENT	'P'		// percentage as TOP BOT ALL or NN%
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STL_ARGLISTSTAT	'a'		// argument list status as (x of y)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STL_PAGENUM	'N'		// page number (when printing)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STL_SHOWCMD	'S'		// 'showcmd' buffer
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STL_VIM_ਵਿਅੰਜਕ	'{'		// start of expression to substitute
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STL_SEPARATE	'='		// separation between alignment
					// sections
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STL_TRUNCMARK	'<'		// truncation mark if line is too long
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STL_USER_HL	'*'		// highlight from (User)1..9 or 0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STL_HIGHLIGHT	'#'		// highlight name
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STL_LINEBREAK	'@'		// insert a line break
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STL_CLICKFUNC	'['		// click handler region
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STL_TABPAGENR	'T'		// tab page label nr
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STL_TABCLOSENR	'X'		// tab page close nr
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STL_ALL		((char_u *) "fFtcvVlLknoObBrRhHmYyWwMqpPaNS{#@[")

// flags used for parsed 'wildmode'
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WIM_ਭਰਿਆ	0x01
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WIM_LONGEST	0x02
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WIM_LIST	0x04
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WIM_BUFLASTUSED	0x08
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WIM_NOSELECT	0x10
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WIM_NOINSERT	0x20

// flags for the 'wildoptions' option
// each defined char should be unique over all values.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WOP_FUZZY	'z'
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WOP_TAGFILE	'g'
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WOP_PUM		'p'
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WOP_EXACTTEXT	'x'

// arguments for can_bs()
// each defined char should be unique over all values
// except for BS_START, that intentionally also matches BS_NOSTOP
// because BS_NOSTOP behaves exactly the same except it
// does not stop at the start of the insert point
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BS_INDENT	'i'	// "Indent"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BS_EOL		'l'	// "eoL"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BS_ਸ਼ੁਰੂ	's'	// "Start"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BS_NOSTOP	'p'	// "nostoP

// flags for the 'culopt' option
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CULOPT_ਸਤਰ	0x01	// Highlight complete line
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CULOPT_SCRLINE	0x02	// Highlight screen line
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CULOPT_NBR	0x04	// Highlight Number column

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LISPWORD_ਮੁੱਲ	"defun,define,defmacro,set!,lambda,if,case,let,flet,let*,letrec,do,do*,define-syntax,let-syntax,letrec-syntax,destructuring-bind,defpackage,defparameter,defstruct,deftype,defvar,do-all-symbols,do-external-symbols,do-symbols,dolist,dotimes,ecase,etypecase,eval-when,labels,macrolet,multiple-value-bind,multiple-value-call,multiple-value-prog1,multiple-value-setq,prog1,progv,typecase,unless,unwind-protect,when,with-input-from-string,with-open-file,with-open-stream,with-output-to-string,with-package-iterator,define-condition,handler-bind,handler-case,restart-bind,restart-case,with-simple-restart,store-value,use-value,muffle-warning,abort,continue,with-slots,with-slots*,with-accessors,with-accessors*,defclass,defmethod,print-unreadable-object"

/*
 * The following are actual variables for the options
 */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
EXTERN ਲੰਮਾ	p_aleph;	// 'aleph'
#ਜੇ_ਅੰਤ
EXTERN char_u	*p_ambw;	// 'ambiwidth'
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_AUTOCHDIR
EXTERN ਪੂਰਨ	p_acd;		// 'autochdir'
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_AUTOSHELLDIR
EXTERN ਪੂਰਨ	p_asd;		// 'autoshelldir'
#ਜੇ_ਅੰਤ
EXTERN ਪੂਰਨ	p_ai;		// 'autoindent'
EXTERN ਪੂਰਨ	p_bin;		// 'binary'
EXTERN ਪੂਰਨ	p_bomb;		// 'bomb'
EXTERN ਪੂਰਨ	p_bl;		// 'buflisted'
EXTERN ਪੂਰਨ	p_cin;		// 'cindent'
EXTERN char_u	*p_cink;	// 'cinkeys'
EXTERN char_u	*p_cinsd;	// 'cinscopedecls'
EXTERN char_u	*p_cinw;	// 'cinwords'
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_COMPL_FUNC
EXTERN char_u	*p_cfu;		// 'completefunc'
EXTERN char_u	*p_ofu;		// 'omnifunc'
EXTERN char_u	*p_tsrfu;	// 'thesaurusfunc'
#ਜੇ_ਅੰਤ
EXTERN ਪੂਰਨ	p_ci;		// 'copyindent'
#ਜੇ defined(FEAT_GUI) && defined(MACOS_X)
EXTERN ਪੂਰਨ	*p_antialias;	// 'antialias'
#ਜੇ_ਅੰਤ
EXTERN ਪੂਰਨ	p_ar;		// 'autoread'
EXTERN ਪੂਰਨ	p_aw;		// 'autowrite'
EXTERN ਪੂਰਨ	p_awa;		// 'autowriteall'
EXTERN char_u	*p_bs;		// 'backspace'
EXTERN char_u	*p_bg;		// 'background'
EXTERN ਪੂਰਨ	p_bk;		// 'backup'
EXTERN char_u	*p_bkc;		// 'backupcopy'
EXTERN ਬਿਨਾਨਿਸ਼ਾਨੀ	bkc_flags;	// flags from 'backupcopy'
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BKC_YES		0x001
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BKC_AUTO		0x002
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BKC_NO			0x004
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BKC_BREAKSYMLINK	0x008
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BKC_BREAKHARDLINK	0x010
EXTERN char_u	*p_bdir;	// 'backupdir'
EXTERN char_u	*p_bex;		// 'backupext'
EXTERN char_u	*p_bo;		// 'belloff'
EXTERN ਬਿਨਾਨਿਸ਼ਾਨੀ	bo_flags;

// values for the 'belloff' option
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BO_ALL		0x0001
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BO_BS		0x0002
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BO_CRSR		0x0004
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BO_COMPL	0x0008
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BO_COPY		0x0010
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BO_CTRLG	0x0020
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BO_ERROR	0x0040
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BO_ESC		0x0080
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BO_EX		0x0100
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BO_HANGUL	0x0200
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BO_IM		0x0400
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BO_LANG		0x0800
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BO_MESS		0x1000
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BO_ਮੇਲ	0x2000
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BO_OPER		0x4000
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BO_REG		0x8000
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BO_SH		0x10000
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BO_SPELL	0x20000
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BO_ਪਦ		0x40000
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BO_WILD		0x80000

EXTERN char_u	*p_bsk;		// 'backupskip'
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CRYPT
EXTERN char_u	*p_cm;		// 'cryptmethod'
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_BEVAL
# ifdef FEAT_BEVAL_GUI
EXTERN ਪੂਰਨ	p_beval;	// 'ballooneval'
# endif
EXTERN ਲੰਮਾ	p_bdlay;	// 'balloondelay'
# ifdef FEAT_EVAL
EXTERN char_u	*p_bexpr;
# endif
# ifdef FEAT_BEVAL_ਪਦ
EXTERN ਪੂਰਨ	p_bevalterm;	// 'balloonevalterm'
# endif
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_BROWSE
EXTERN char_u	*p_bsdir;	// 'browsedir'
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
EXTERN char_u	*p_breakat;	// 'breakat'
#ਜੇ_ਅੰਤ
EXTERN char_u	*p_bh;		// 'bufhidden'
EXTERN char_u	*p_bt;		// 'buftype'
EXTERN char_u	*p_cmp;		// 'casemap'
EXTERN ਬਿਨਾਨਿਸ਼ਾਨੀ	cmp_flags;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CMP_ਅੰਦਰੂਨੀ		0x001
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CMP_KEEPASCII		0x002
EXTERN char_u	*p_enc;		// 'encoding'
EXTERN ਪੂਰਨ	p_deco;		// 'delcombine'
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
EXTERN char_u	*p_ccv;		// 'charconvert'
#ਜੇ_ਅੰਤ
EXTERN ਪੂਰਨ	p_cdh;		// 'cdhome'
EXTERN char_u	*p_cino;	// 'cinoptions'
EXTERN char_u	*p_cedit;	// 'cedit'
EXTERN ਲੰਮਾ	p_cwh;		// 'cmdwinheight'
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_CLIPMETHOD
EXTERN char_u	*p_cb;		// 'clipboard'
EXTERN char_u	*p_cpm;		// 'clipmethod'
#ਜੇ_ਅੰਤ
EXTERN ਲੰਮਾ	p_ch;		// 'cmdheight'
EXTERN char_u	*p_cms;		// 'commentstring'
EXTERN char_u	*p_cpt;		// 'complete'
EXTERN ਲੰਮਾ	p_cto;		// 'completetimeout'
#ਜੇ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
EXTERN ਪੂਰਨ	p_confirm;	// 'confirm'
#ਜੇ_ਅੰਤ
EXTERN ਪੂਰਨ	p_cp;		// 'compatible'
EXTERN char_u	*p_cfc;		// 'completefuzzycollect'
EXTERN ਬਿਨਾਨਿਸ਼ਾਨੀ cfc_flags;	// flags from "completefuzzycollect"
EXTERN char_u	*p_cia;		// 'completeitemalign'
EXTERN ਬਿਨਾਨਿਸ਼ਾਨੀ cia_flags;	// order flags of 'completeitemalign'
EXTERN char_u	*p_cot;		// 'completeopt'
EXTERN ਬਿਨਾਨਿਸ਼ਾਨੀ	cot_flags;	// flags from 'completeopt'
EXTERN ਪੂਰਨ	p_ac;		// 'autocomplete'
EXTERN ਲੰਮਾ	p_act;		// 'autocompletetimeout'
EXTERN ਲੰਮਾ	p_acl;		// 'autocompletedelay'
// Keep in sync with p_cot_values in optionstr.c
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ COT_MENU	0x001
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ COT_MENUONE	0x002
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ COT_ANY_MENU	0x003	// combination of menu flags
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ COT_LONGEST	0x004	// FALSE: insert full match,
				// TRUE: insert longest prefix
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ COT_PREVIEW	    0x008
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ COT_POPUP	    0x010
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ COT_POPUPHIDDEN	    0x020
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ COT_ANY_PREVIEW	    0x038   // combination of preview flags
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ COT_NOINSERT	    0x040   // FALSE: select & insert, TRUE: noinsert
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ COT_NOSELECT	    0x080   // FALSE: select & insert, TRUE: noselect
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ COT_FUZZY	    0x100   // TRUE: fuzzy match enabled
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ COT_NOSORT	    0x200   // TRUE: fuzzy match without qsort score
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ COT_PREINSERT	    0x400   // TRUE: preinsert
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ COT_ਸਭ_ਤੋਂ_ਨੇੜੇ	    0x800   // TRUE: prioritize matches close to cursor

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CFC_KEYWORD         0x001
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CFC_FILES           0x002
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CFC_WHOLELINE       0x004

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ BACKSLASH_ਵਿੱਚ_FILENAME
EXTERN char_u	*p_csl;		// 'completeslash'
#ਜੇ_ਅੰਤ
EXTERN ਲੰਮਾ	p_ph;		// 'pumheight'
EXTERN ਲੰਮਾ	p_pw;		// 'pumwidth'
EXTERN ਲੰਮਾ	p_pmw;		// 'pummaxwidth'
EXTERN char_u	*p_pb;		// 'pumborder'
EXTERN char_u	*p_pumopt;	// 'pumopt'
EXTERN char_u	*p_com;		// 'comments'
EXTERN char_u	*p_cpo;		// 'cpoptions'
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CSCOPE
EXTERN char_u	*p_csprg;	// 'cscopeprg'
EXTERN ਪੂਰਨ	p_csre;		// 'cscoperelative'
# ifdef FEAT_QUICKFIX
EXTERN char_u	*p_csqf;	// 'cscopequickfix'
#  define	CSQF_CMDS   "sgdctefia"
#  define	CSQF_FLAGS  "+-0"
# endif
EXTERN ਪੂਰਨ	p_cst;		// 'cscopetag'
EXTERN ਲੰਮਾ	p_csto;		// 'cscopetagorder'
EXTERN ਲੰਮਾ	p_cspc;		// 'cscopepathcomp'
EXTERN ਪੂਰਨ	p_csverbose;	// 'cscopeverbose'
#ਜੇ_ਅੰਤ
EXTERN char_u	*p_debug;	// 'debug'
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਲੱਭੋ_ID
EXTERN char_u	*p_def;		// 'define'
EXTERN char_u	*p_inc;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
EXTERN char_u	*p_dia;		// 'diffanchors'
EXTERN char_u	*p_dip;		// 'diffopt'
# ifdef FEAT_EVAL
EXTERN char_u	*p_dex;		// 'diffexpr'
# endif
#ਜੇ_ਅੰਤ
EXTERN char_u	*p_dict;	// 'dictionary'
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIGRAPHS
EXTERN ਪੂਰਨ	p_dg;		// 'digraph'
#ਜੇ_ਅੰਤ
EXTERN char_u	*p_dir;		// 'directory'
EXTERN char_u	*p_dy;		// 'display'
EXTERN ਬਿਨਾਨਿਸ਼ਾਨੀ	dy_flags;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DY_LASTLINE		0x001
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DY_TRUNCATE		0x002
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DY_UHEX			0x004
EXTERN ਪੂਰਨ	p_ed;		// 'edcompatible'
EXTERN char_u	*p_ead;		// 'eadirection'
EXTERN char_u	*p_emoji;	// 'emoji'
EXTERN ਪੂਰਨ	p_ea;		// 'equalalways'
EXTERN char_u	*p_ep;		// 'equalprg'
EXTERN ਪੂਰਨ	p_eb;		// 'errorbells'
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_QUICKFIX
EXTERN char_u	*p_ef;		// 'errorfile'
EXTERN char_u	*p_efm;		// 'errorformat'
EXTERN char_u	*p_gefm;	// 'grepformat'
EXTERN char_u	*p_gp;		// 'grepprg'
#ਜੇ_ਅੰਤ
EXTERN ਪੂਰਨ	p_eof;		// 'endoffile'
EXTERN ਪੂਰਨ	p_eol;		// 'endofline'
EXTERN ਪੂਰਨ	p_ek;		// 'esckeys'
EXTERN char_u	*p_ei;		// 'eventignore'
EXTERN ਪੂਰਨ	p_et;		// 'expandtab'
EXTERN ਪੂਰਨ	p_exrc;		// 'exrc'
EXTERN char_u	*p_fenc;	// 'fileencoding'
EXTERN char_u	*p_fencs;	// 'fileencodings'
EXTERN char_u	*p_ff;		// 'fileformat'
EXTERN char_u	*p_ffs;		// 'fileformats'
EXTERN ਪੂਰਨ	p_fic;		// 'fileignorecase'
EXTERN char_u	*p_ft;		// 'filetype'
EXTERN char_u	*p_fcs;		// 'fillchar'
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
EXTERN char_u	*p_ffu;		// 'findfunc'
#ਜੇ_ਅੰਤ
EXTERN ਪੂਰਨ	p_fixeol;	// 'fixendofline'
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
EXTERN char_u	*p_fcl;		// 'foldclose'
EXTERN ਲੰਮਾ	p_fdls;		// 'foldlevelstart'
EXTERN char_u	*p_fdo;		// 'foldopen'
EXTERN ਬਿਨਾਨਿਸ਼ਾਨੀ	fdo_flags;
# define FDO_ALL		0x001
# define FDO_ਖੰਡ		0x002
# define FDO_HOR		0x004
# define FDO_MARK		0x008
# define FDO_PERCENT		0x010
# define FDO_QUICKFIX		0x020
# define FDO_ਖੋਜ		0x040
# define FDO_ਟੈਗ		0x080
# define FDO_ਪਾਓ		0x100
# define FDO_UNDO		0x200
# define FDO_JUMP		0x400
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_EVAL)
EXTERN char_u	*p_fex;		// 'formatexpr'
#ਜੇ_ਅੰਤ
EXTERN char_u	*p_flp;		// 'formatlistpat'
EXTERN char_u	*p_fo;		// 'formatoptions'
EXTERN char_u	*p_fp;		// 'formatprg'
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_FSYNC
EXTERN ਪੂਰਨ	p_fs;		// 'fsync'
#ਜੇ_ਅੰਤ
EXTERN ਪੂਰਨ	p_gd;		// 'gdefault'
EXTERN char_u	*p_jop;		// 'jumpoptions'
EXTERN ਬਿਨਾਨਿਸ਼ਾਨੀ	jop_flags;	//
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ JOP_ਢੇਰ		0x001
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
# ifdef FEAT_QUICKFIX
EXTERN char_u	*p_cpp;		// 'completepopup'
# endif
EXTERN char_u	*p_pvp;		// 'previewpopup'
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PRINTER
EXTERN char_u	*p_pdev;	// 'printdevice'
# ifdef FEAT_POSTSCRIPT
EXTERN char_u	*p_penc;	// 'printencoding'
EXTERN char_u	*p_pmfn;	// 'printmbfont'
EXTERN char_u	*p_pmcs;	// 'printmbcharset'
# endif
# ਜੇ defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
EXTERN char_u	*p_pexpr;	// 'printexpr'
# endif
EXTERN char_u	*p_pfn;		// 'printfont'
EXTERN char_u	*p_popt;	// 'printoptions'
EXTERN char_u	*p_ਸਿਰਲੇਖ;	// 'printheader'
#ਜੇ_ਅੰਤ
EXTERN ਪੂਰਨ	p_prompt;	// 'prompt'
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
EXTERN char_u	*p_guifont;	// 'guifont'
# ifdef FEAT_XFONTSET
EXTERN char_u	*p_guifontset;	// 'guifontset'
# endif
EXTERN char_u	*p_guifontwide;	// 'guifontwide'
EXTERN ਪੂਰਨ	p_guipty;	// 'guipty'
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
EXTERN char_u	*p_guiligatures;  // 'guiligatures'
#ਜੇ_ਅੰਤ
#ਜੇ (defined(FEAT_GUI_GTK) && !defined(USE_GTK4)) || defined(FEAT_GUI_X11)
EXTERN ਲੰਮਾ	p_ghr;		// 'guiheadroom'
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURSOR_SHAPE
EXTERN char_u	*p_guicursor;	// 'guicursor'
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MOUSESHAPE
EXTERN char_u	*p_mouseshape;	// 'mouseshape'
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_GUI)
EXTERN char_u	*p_go;		// 'guioptions'
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_GUI_TABLINE)
EXTERN char_u	*p_gtl;		// 'guitablabel'
EXTERN char_u	*p_gtt;		// 'guitabtooltip'
#ਜੇ_ਅੰਤ
EXTERN char_u	*p_hf;		// 'helpfile'
EXTERN ਲੰਮਾ	p_hh;		// 'helpheight'
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MULTI_LANG
EXTERN char_u	*p_hlg;		// 'helplang'
#ਜੇ_ਅੰਤ
EXTERN ਪੂਰਨ	p_hid;		// 'hidden'
EXTERN char_u	*p_hl;		// 'highlight'
EXTERN ਪੂਰਨ	p_hls;		// 'hlsearch'
EXTERN ਲੰਮਾ	p_hi;		// 'history'
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
EXTERN ਪੂਰਨ	p_hkmap;	// 'hkmap'
EXTERN ਪੂਰਨ	p_hkmapp;	// 'hkmapp'
# ifdef FEAT_ARABIC
EXTERN ਪੂਰਨ	p_arshape;	// 'arabicshape'
# endif
#ਜੇ_ਅੰਤ
EXTERN ਪੂਰਨ	p_icon;		// 'icon'
EXTERN char_u	*p_iconstring;	// 'iconstring'
EXTERN ਪੂਰਨ	p_ic;		// 'ignorecase'
#ਜੇ defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
EXTERN char_u	*p_imak;	// 'imactivatekey'
# define IM_ON_THE_SPOT		0L
# define IM_OVER_THE_SPOT	1L
EXTERN ਲੰਮਾ	p_imst;		// 'imstyle'
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_IMAGE
EXTERN char_u	*p_ipc;		// 'imageprotocol'
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_EVAL)
EXTERN char_u	*p_imaf;	// 'imactivatefunc'
EXTERN char_u	*p_imsf;	// 'imstatusfunc'
#ਜੇ_ਅੰਤ
EXTERN ਪੂਰਨ	p_imcmdline;	// 'imcmdline'
EXTERN ਪੂਰਨ	p_imdisable;	// 'imdisable'
EXTERN ਲੰਮਾ	p_iminsert;	// 'iminsert'
EXTERN ਲੰਮਾ	p_imsearch;	// 'imsearch'
EXTERN ਪੂਰਨ	p_inf;		// 'infercase'
#ਜੇ defined(FEAT_ਲੱਭੋ_ID) && defined(FEAT_EVAL)
EXTERN char_u	*p_inex;	// 'includeexpr'
#ਜੇ_ਅੰਤ
EXTERN ਪੂਰਨ	p_is;		// 'incsearch'
#ਜੇ defined(FEAT_EVAL)
EXTERN char_u	*p_inde;	// 'indentexpr'
EXTERN char_u	*p_indk;	// 'indentkeys'
#ਜੇ_ਅੰਤ
EXTERN ਪੂਰਨ	p_im;		// 'insertmode'
EXTERN char_u	*p_isf;		// 'isfname'
EXTERN char_u	*p_isi;		// 'isident'
EXTERN char_u	*p_isk;		// 'iskeyword'
EXTERN char_u	*p_isp;		// 'isprint'
EXTERN ਪੂਰਨ	p_js;		// 'joinspaces'
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CRYPT
EXTERN char_u	*p_ਕੁੰਜੀ;		// 'key'
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_KEYMAP
EXTERN char_u	*p_keymap;	// 'keymap'
#ਜੇ_ਅੰਤ
EXTERN char_u	*p_kp;		// 'keywordprg'
EXTERN char_u	*p_km;		// 'keymodel'
EXTERN char_u	*p_kpc;		// 'keyprotocol'
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LANGMAP
EXTERN char_u	*p_langmap;	// 'langmap'
EXTERN ਪੂਰਨ	p_lnr;		// 'langnoremap'
EXTERN ਪੂਰਨ	p_lrm;		// 'langremap'
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_MENU) && defined(FEAT_MULTI_LANG)
EXTERN char_u	*p_lm;		// 'langmenu'
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
EXTERN ਲੰਮਾ	p_linespace;	// 'linespace'
#ਜੇ_ਅੰਤ
EXTERN ਪੂਰਨ	p_lisp;		// 'lisp'
EXTERN char_u	*p_lop;		// 'lispoptions'
EXTERN char_u	*p_lispwords;	// 'lispwords'
EXTERN ਲੰਮਾ	p_ls;		// 'laststatus'
EXTERN ਲੰਮਾ	p_stal;		// 'showtabline'
EXTERN char_u	*p_lcs;		// 'listchars'

EXTERN ਪੂਰਨ	p_lz;		// 'lazyredraw'
EXTERN ਪੂਰਨ	p_lpl;		// 'loadplugins'
#ਜੇ defined(DYNAMIC_LUA)
EXTERN char_u	*p_luadll;	// 'luadll'
#ਜੇ_ਅੰਤ
EXTERN ਪੂਰਨ	p_magic;	// 'magic'
EXTERN char_u	*p_menc;	// 'makeencoding'
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_QUICKFIX
EXTERN char_u	*p_mef;		// 'makeef'
EXTERN char_u	*p_mp;		// 'makeprg'
#ਜੇ_ਅੰਤ
EXTERN char_u	*p_mps;		// 'matchpairs'
EXTERN ਲੰਮਾ	p_mat;		// 'matchtime'
EXTERN ਲੰਮਾ	p_mco;		// 'maxcombine'
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
EXTERN ਲੰਮਾ	p_mfd;		// 'maxfuncdepth'
#ਜੇ_ਅੰਤ
EXTERN ਲੰਮਾ	p_mmd;		// 'maxmapdepth'
EXTERN ਲੰਮਾ	p_mm;		// 'maxmem'
EXTERN ਲੰਮਾ	p_mmp;		// 'maxmempattern'
EXTERN ਲੰਮਾ	p_mmt;		// 'maxmemtot'
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MENU
EXTERN ਲੰਮਾ	p_mis;		// 'menuitems'
#ਜੇ_ਅੰਤ
EXTERN char_u	*p_mopt;	// 'messagesopt'
EXTERN ਲੰਮਾ	p_msc;		// 'maxsearchcount'
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SPELL
EXTERN char_u	*p_msm;		// 'mkspellmem'
#ਜੇ_ਅੰਤ
EXTERN ਪੂਰਨ	p_ml;		// 'modeline'
EXTERN ਪੂਰਨ	p_mle;		// 'modelineexpr'
EXTERN ਲੰਮਾ	p_mls;		// 'modelines'
EXTERN ਪੂਰਨ	p_mlstr;	// 'modelinestrict'
EXTERN ਪੂਰਨ	p_ma;		// 'modifiable'
EXTERN ਪੂਰਨ	p_mod;		// 'modified'
EXTERN char_u	*p_mouse;	// 'mouse'
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
EXTERN ਪੂਰਨ	p_mousef;	// 'mousefocus'
EXTERN ਪੂਰਨ	p_mh;		// 'mousehide'
#ਜੇ_ਅੰਤ
EXTERN char_u	*p_mousem;	// 'mousemodel'
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
EXTERN ਪੂਰਨ	p_mousemev;	// 'mousemoveevent'
#ਜੇ_ਅੰਤ
EXTERN ਲੰਮਾ	p_mouset;	// 'mousetime'
EXTERN ਪੂਰਨ	p_ਹੋਰ;		// 'more'
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MZSCHEME
EXTERN ਲੰਮਾ	p_mzq;		// 'mzquantum
# ਜੇ defined(DYNAMIC_MZSCHEME)
EXTERN char_u	*p_mzschemedll;	// 'mzschemedll'
EXTERN char_u	*p_mzschemegcdll; // 'mzschemegcdll'
# endif
#ਜੇ_ਅੰਤ
EXTERN char_u	*p_nf;		// 'nrformats'
#ਜੇ defined(MSWIN)
EXTERN ਪੂਰਨ	p_odev;		// 'opendevice'
#ਜੇ_ਅੰਤ
EXTERN ਲੰਮਾ	p_ost;		// 'osctimeoutlen'
EXTERN char_u	*p_opfunc;	// 'operatorfunc'
EXTERN char_u	*p_para;	// 'paragraphs'
EXTERN ਪੂਰਨ	p_paste;	// 'paste'
EXTERN char_u	*p_pt;		// 'pastetoggle'
#ਜੇ defined(FEAT_EVAL) && defined(FEAT_DIFF)
EXTERN char_u	*p_pex;		// 'patchexpr'
#ਜੇ_ਅੰਤ
EXTERN char_u	*p_pm;		// 'patchmode'
EXTERN char_u	*p_ਰਸਤਾ;	// 'path'
EXTERN char_u	*p_cdpath;	// 'cdpath'
#ਜੇ defined(DYNAMIC_PERL)
EXTERN char_u	*p_perldll;	// 'perldll'
#ਜੇ_ਅੰਤ
EXTERN ਪੂਰਨ	p_pi;		// 'preserveindent'
#ਜੇ defined(DYNAMIC_PYTHON3)
EXTERN char_u	*p_py3dll;	// 'pythonthreedll'
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PYTHON3
EXTERN char_u	*p_py3home;	// 'pythonthreehome'
#ਜੇ_ਅੰਤ
#ਜੇ defined(DYNAMIC_PYTHON)
EXTERN char_u	*p_pydll;	// 'pythondll'
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PYTHON
EXTERN char_u	*p_pyhome;	// 'pythonhome'
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
EXTERN ਲੰਮਾ	p_pyx;		// 'pyxversion'
#ਜੇ_ਅੰਤ
EXTERN char_u	*p_qe;		// 'quoteescape'
EXTERN ਪੂਰਨ	p_ro;		// 'readonly'
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RELTIME
EXTERN ਲੰਮਾ	p_rdt;		// 'redrawtime'
#ਜੇ_ਅੰਤ
EXTERN ਪੂਰਨ	p_remap;	// 'remap'
EXTERN ਲੰਮਾ	p_re;		// 'regexpengine'
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RENDER_OPTIONS
EXTERN char_u	*p_rop;		// 'renderoptions'
#ਜੇ_ਅੰਤ
EXTERN ਲੰਮਾ	p_report;	// 'report'
#ਜੇ defined(FEAT_QUICKFIX)
EXTERN ਲੰਮਾ	p_pvh;		// 'previewheight'
EXTERN ਲੰਮਾ     p_chi;          // 'chistory'
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
EXTERN ਪੂਰਨ	p_rs;		// 'restorescreen'
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
EXTERN ਪੂਰਨ	p_ari;		// 'allowrevins'
EXTERN ਪੂਰਨ	p_ri;		// 'revins'
#ਜੇ_ਅੰਤ
#ਜੇ defined(DYNAMIC_RUBY)
EXTERN char_u	*p_rubydll;	// 'rubydll'
#ਜੇ_ਅੰਤ
EXTERN ਪੂਰਨ	p_ru;		// 'ruler'
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_STL_OPT
EXTERN char_u	*p_ruf;		// 'rulerformat'
#ਜੇ_ਅੰਤ
EXTERN char_u	*p_pp;		// 'packpath'
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_QUICKFIX
EXTERN char_u	*p_qftf;	// 'quickfixtextfunc'
#ਜੇ_ਅੰਤ
EXTERN char_u	*p_rtp;		// 'runtimepath'
EXTERN ਲੰਮਾ	p_sj;		// 'scrolljump'
#ਜੇ defined(MSWIN) && defined(FEAT_GUI)
EXTERN ਪੂਰਨ	p_scf;		// 'scrollfocus'
#ਜੇ_ਅੰਤ
EXTERN ਲੰਮਾ	p_so;		// 'scrolloff'
EXTERN ਲੰਮਾ	p_sop;		// 'scrolloffpad'
EXTERN char_u	*p_sbo;		// 'scrollopt'
EXTERN char_u	*p_sections;	// 'sections'
EXTERN ਪੂਰਨ	p_secure;	// 'secure'
EXTERN char_u	*p_sel;		// 'selection'
EXTERN char_u	*p_slm;		// 'selectmode'
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SESSION
EXTERN char_u	*p_ssop;	// 'sessionoptions'
EXTERN ਬਿਨਾਨਿਸ਼ਾਨੀ	ssop_flags;
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
#ਜੇ_ਅੰਤ
EXTERN char_u	*p_sh;		// 'shell'
EXTERN char_u	*p_shcf;	// 'shellcmdflag'
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_QUICKFIX
EXTERN char_u	*p_sp;		// 'shellpipe'
#ਜੇ_ਅੰਤ
EXTERN char_u	*p_shq;		// 'shellquote'
EXTERN char_u	*p_sxq;		// 'shellxquote'
EXTERN char_u	*p_sxe;		// 'shellxescape'
EXTERN char_u	*p_srr;		// 'shellredir'
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ AMIGA
EXTERN ਲੰਮਾ	p_st;		// 'shelltype'
#ਜੇ_ਅੰਤ
EXTERN ਪੂਰਨ	p_stmp;		// 'shelltemp'
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ BACKSLASH_ਵਿੱਚ_FILENAME
EXTERN ਪੂਰਨ	p_ssl;		// 'shellslash'
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_STL_OPT
EXTERN char_u	*p_stl;		// 'statusline'
EXTERN char_u	*p_stlo;	// 'statuslineopt'
#ਜੇ_ਅੰਤ
EXTERN ਪੂਰਨ	p_sr;		// 'shiftround'
EXTERN ਲੰਮਾ	p_sw;		// 'shiftwidth'
EXTERN char_u	*p_shm;		// 'shortmess'
EXTERN ਪੂਰਨ	p_sn;		// 'shortname'
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
EXTERN char_u	*p_sbr;		// 'showbreak'
#ਜੇ_ਅੰਤ
EXTERN ਪੂਰਨ	p_sc;		// 'showcmd'
EXTERN char_u	*p_sloc;	// 'showcmdloc'
EXTERN ਪੂਰਨ	p_sft;		// 'showfulltag'
EXTERN ਪੂਰਨ	p_sm;		// 'showmatch'
EXTERN ਪੂਰਨ	p_smd;		// 'showmode'
EXTERN ਲੰਮਾ	p_ss;		// 'sidescroll'
EXTERN ਲੰਮਾ	p_siso;		// 'sidescrolloff'
EXTERN ਪੂਰਨ	p_scs;		// 'smartcase'
EXTERN ਪੂਰਨ	p_si;		// 'smartindent'
EXTERN ਪੂਰਨ	p_sta;		// 'smarttab'
EXTERN ਲੰਮਾ	p_sts;		// 'softtabstop'
EXTERN ਪੂਰਨ	p_sb;		// 'splitbelow'
EXTERN char_u	*p_sua;		// 'suffixesadd'
EXTERN ਪੂਰਨ	p_swf;		// 'swapfile'
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
EXTERN ਲੰਮਾ	p_smc;		// 'synmaxcol'
#ਜੇ_ਅੰਤ
EXTERN ਲੰਮਾ	p_tpm;		// 'tabpagemax'
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_STL_OPT
EXTERN char_u	*p_tal;		// 'tabline'
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
EXTERN char_u	*p_tfu;		// 'tagfunc'
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SPELL
EXTERN char_u	*p_spc;		// 'spellcapcheck'
EXTERN char_u	*p_spf;		// 'spellfile'
EXTERN char_u	*p_spl;		// 'spelllang'
EXTERN char_u	*p_spo;		// 'spelloptions'
EXTERN char_u	*p_sps;		// 'spellsuggest'
#ਜੇ_ਅੰਤ
EXTERN ਪੂਰਨ	p_spr;		// 'splitright'
EXTERN ਪੂਰਨ	p_sol;		// 'startofline'
EXTERN char_u	*p_su;		// 'suffixes'
EXTERN char_u	*p_sws;		// 'swapsync'
EXTERN char_u	*p_swb;		// 'switchbuf'
EXTERN ਬਿਨਾਨਿਸ਼ਾਨੀ	swb_flags;
// Keep in sync with p_swb_values in optionstr.c
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SWB_USEOPEN		0x001
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SWB_USETAB		0x002
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SWB_ਵੰਡੋ		0x004
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SWB_NEWTAB		0x008
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SWB_VSPLIT		0x010
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SWB_USELAST		0x020

EXTERN char_u	*p_spk;		// 'splitkeep'

#ਜੇ defined(FEAT_TABPANEL)
EXTERN char_u	*p_tpl;		// 'tabpanel'
EXTERN ਲੰਮਾ	p_stpl;		// 'showtabpanel'
EXTERN char_u	*p_tplo;	// 'tabpanelopt'
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
EXTERN char_u	*p_syn;		// 'syntax'
#ਜੇ_ਅੰਤ
EXTERN char_u	*p_tcl;		// 'tabclose'
EXTERN ਬਿਨਾਨਿਸ਼ਾਨੀ	tcl_flags;	// flags from 'tabclose'
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TCL_ਖੱਬਾ		0x001
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TCL_USELAST		0x002
EXTERN ਲੰਮਾ	p_ts;		// 'tabstop'
EXTERN ਪੂਰਨ	p_tbs;		// 'tagbsearch'
EXTERN char_u	*p_tc;		// 'tagcase'
EXTERN ਬਿਨਾਨਿਸ਼ਾਨੀ tc_flags;       // flags from 'tagcase'
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TC_FOLLOWIC		0x01
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TC_IGNORE		0x02
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TC_ਮੇਲ		0x04
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TC_FOLLOWSCS		0x08
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TC_SMART		0x10
EXTERN ਲੰਮਾ	p_tl;		// 'taglength'
EXTERN ਪੂਰਨ	p_tr;		// 'tagrelative'
EXTERN char_u	*p_ਟੈਗ_ਸੂਚੀ;	// 'tags'
EXTERN ਪੂਰਨ	p_tagsecure;    // 'tagsecure'
EXTERN ਪੂਰਨ	p_tgst;		// 'tagstack'
#ਜੇ defined(DYNAMIC_TCL)
EXTERN char_u	*p_tcldll;	// 'tcldll'
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ARABIC
EXTERN ਪੂਰਨ	p_tbidi;	// 'termbidi'
#ਜੇ_ਅੰਤ
EXTERN char_u	*p_tenc;	// 'termencoding'
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMGUICOLORS
EXTERN ਪੂਰਨ	p_tgc;		// 'termguicolors'
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
EXTERN char_u	*p_trz;		// 'termresize'
#ਜੇ_ਅੰਤ
EXTERN ਪੂਰਨ	p_tsy;		// 'termsync'
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
EXTERN ਲੰਮਾ	p_twsl;		// 'termwinscroll'
#ਜੇ_ਅੰਤ
#ਜੇ defined(MSWIN) && defined(FEAT_TERMINAL)
EXTERN char_u	*p_twt;		// 'termwintype'
#ਜੇ_ਅੰਤ
EXTERN ਪੂਰਨ	p_terse;	// 'terse'
EXTERN ਪੂਰਨ	p_ta;		// 'textauto'
EXTERN ਪੂਰਨ	p_tx;		// 'textmode'
EXTERN ਲੰਮਾ	p_tw;		// 'textwidth'
EXTERN ਪੂਰਨ	p_ਮੰਜ਼ਿਲ;		// 'tildeop'
EXTERN ਪੂਰਨ	p_timeout;	// 'timeout'
EXTERN ਲੰਮਾ	p_tm;		// 'timeoutlen'
EXTERN ਪੂਰਨ	p_title;	// 'title'
EXTERN ਲੰਮਾ	p_titlelen;	// 'titlelen'
EXTERN char_u	*p_titleold;	// 'titleold'
EXTERN char_u	*p_titlestring;	// 'titlestring'
EXTERN char_u	*p_tsr;		// 'thesaurus'
EXTERN ਪੂਰਨ	p_ttimeout;	// 'ttimeout'
EXTERN ਲੰਮਾ	p_ttm;		// 'ttimeoutlen'
EXTERN ਪੂਰਨ	p_tbi;		// 'ttybuiltin'
EXTERN ਪੂਰਨ	p_tf;		// 'ttyfast'
#ਜੇ defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
EXTERN char_u	*p_toolbar;	// 'toolbar'
EXTERN ਬਿਨਾਨਿਸ਼ਾਨੀ toolbar_flags;
# define TOOLBAR_ਪਾਠ		0x01
# define TOOLBAR_ICONS		0x02
# define TOOLBAR_TOOLTIPS	0x04
# define TOOLBAR_HORIZ		0x08
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_TOOLBAR) && defined(FEAT_GUI_GTK)
EXTERN char_u	*p_tbis;	// 'toolbariconsize'
EXTERN ਬਿਨਾਨਿਸ਼ਾਨੀ tbis_flags;
# define TBIS_TINY		0x01
# define TBIS_SMALL		0x02
# define TBIS_MEDIUM		0x04
# define TBIS_LARGE		0x08
# define TBIS_HUGE		0x10
# define TBIS_GIANT		0x20
#ਜੇ_ਅੰਤ
EXTERN ਲੰਮਾ	p_ttyscroll;	// 'ttyscroll'
#ਜੇ defined(UNIX) || defined(VMS)
EXTERN char_u	*p_ttym;	// 'ttymouse'
EXTERN ਬਿਨਾਨਿਸ਼ਾਨੀ ttym_flags;
# define TTYM_XTERM		0x01
# define TTYM_XTERM2		0x02
# define TTYM_DEC		0x04
# define TTYM_NETTERM		0x08
# define TTYM_JSBTERM		0x10
# define TTYM_PTERM		0x20
# define TTYM_URXVT		0x40
# define TTYM_SGR		0x80
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PERSISTENT_UNDO
EXTERN char_u	*p_udir;	// 'undodir'
EXTERN ਪੂਰਨ	p_udf;		// 'undofile'
#ਜੇ_ਅੰਤ
EXTERN ਲੰਮਾ	p_ul;		// 'undolevels'
EXTERN ਲੰਮਾ	p_ur;		// 'undoreload'
EXTERN ਲੰਮਾ	p_uc;		// 'updatecount'
EXTERN ਲੰਮਾ	p_ut;		// 'updatetime'
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VARTABS
EXTERN char_u	*p_vsts;	// 'varsofttabstop'
EXTERN char_u	*p_vts;		// 'vartabstop'
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VIMINFO
EXTERN char_u	*p_viminfo;	// 'viminfo'
EXTERN char_u	*p_viminfofile;	// 'viminfofile'
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SESSION
EXTERN char_u	*p_vdir;	// 'viewdir'
EXTERN char_u	*p_vop;		// 'viewoptions'
EXTERN ਬਿਨਾਨਿਸ਼ਾਨੀ	vop_flags;	// uses SSOP_ flags
#ਜੇ_ਅੰਤ
EXTERN ਪੂਰਨ	p_vb;		// 'visualbell'
EXTERN char_u	*p_ve;		// 'virtualedit'
EXTERN ਬਿਨਾਨਿਸ਼ਾਨੀ ve_flags;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VE_ਖੰਡ	5	// includes "all"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VE_ਪਾਓ	6	// includes "all"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VE_ALL		4
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VE_ONEMORE	8
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VE_NONE		16	// "none"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VE_NONEU	32      // "NONE"
EXTERN ਲੰਮਾ	p_verbose;	// 'verbose'
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ਵਿੱਚ_OPTION_C
char_u	*p_vfile = (char_u *)""; // used before options are initialized
#ਨਹੀਂਤਾਂ
ਬਾਹਰੀ char_u	*p_vfile;	// 'verbosefile'
#ਜੇ_ਅੰਤ
EXTERN ਪੂਰਨ	p_warn;		// 'warn'
EXTERN char_u	*p_wop;		// 'wildoptions'
EXTERN ਲੰਮਾ	p_ਖਿੜਕੀ;	// 'window'
#ਜੇ defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_MOTIF) || defined(LINT) \
	|| defined (FEAT_GUI_GTK) || defined(FEAT_GUI_PHOTON)
# define FEAT_WAK
EXTERN char_u	*p_wak;		// 'winaltkeys'
#ਜੇ_ਅੰਤ
EXTERN char_u	*p_wig;		// 'wildignore'
EXTERN ਪੂਰਨ	p_wiv;		// 'weirdinvert'
EXTERN char_u	*p_ww;		// 'whichwrap'
EXTERN ਲੰਮਾ	p_wc;		// 'wildchar'
EXTERN ਲੰਮਾ	p_wcm;		// 'wildcharm'
EXTERN ਪੂਰਨ	p_wic;		// 'wildignorecase'
EXTERN char_u	*p_wim;		// 'wildmode'
EXTERN ਪੂਰਨ	p_wmnu;		// 'wildmenu'
EXTERN ਲੰਮਾ	p_wh;		// 'winheight'
EXTERN ਲੰਮਾ	p_wmh;		// 'winminheight'
EXTERN ਲੰਮਾ	p_wmw;		// 'winminwidth'
EXTERN ਲੰਮਾ	p_wiw;		// 'winwidth'
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_WAYLAND
EXTERN char_u	*p_wse;		// 'wlseat'
EXTERN ਲੰਮਾ     p_wtm;		// 'wltimeoutlen'
#ਜੇ_ਅੰਤ
#ਜੇ defined(MSWIN) && defined(FEAT_TERMINAL)
EXTERN char_u	*p_winptydll;	// 'winptydll'
#ਜੇ_ਅੰਤ
EXTERN ਲੰਮਾ	p_wm;		// 'wrapmargin'
EXTERN ਪੂਰਨ	p_ws;		// 'wrapscan'
EXTERN ਪੂਰਨ	p_write;	// 'write'
EXTERN ਪੂਰਨ	p_wa;		// 'writeany'
EXTERN ਪੂਰਨ	p_wb;		// 'writebackup'
EXTERN ਲੰਮਾ	p_wd;		// 'writedelay'
EXTERN ਪੂਰਨ	p_xtermcodes;	// 'xtermcodes'

/*
 * "indir" values for buffer-local options.
 * These need to be defined globally, so that the BV_COUNT can be used with
 * b_p_scriptID[].
 */
ਗਿਣਤੀ
{
    BV_AI = 0
    , BV_AC
    , BV_AR
    , BV_BH
    , BV_BKC
    , BV_BT
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_QUICKFIX
    , BV_EFM
    , BV_GEFM
    , BV_GP
    , BV_MP
#ਜੇ_ਅੰਤ
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
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
    , BV_DIA
#ਜੇ_ਅੰਤ
    , BV_TSR
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ BACKSLASH_ਵਿੱਚ_FILENAME
    , BV_CSL
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_COMPL_FUNC
    , BV_CFU
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਲੱਭੋ_ID
    , BV_DEF
    , BV_INC
#ਜੇ_ਅੰਤ
    , BV_EOF
    , BV_EOL
    , BV_FIXEOL
    , BV_EP
    , BV_ET
    , BV_FENC
    , BV_FP
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    , BV_BEXPR
    , BV_FEX
    , BV_FFU
#ਜੇ_ਅੰਤ
    , BV_FF
    , BV_FLP
    , BV_FO
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_FSYNC
    , BV_FS
#ਜੇ_ਅੰਤ
    , BV_FT
    , BV_IMI
    , BV_IMS
#ਜੇ defined(FEAT_EVAL)
    , BV_INDE
    , BV_INDK
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_ਲੱਭੋ_ID) && defined(FEAT_EVAL)
    , BV_INEX
#ਜੇ_ਅੰਤ
    , BV_INF
    , BV_ISE
    , BV_ISK
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CRYPT
    , BV_ਕੁੰਜੀ
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_KEYMAP
    , BV_KMAP
#ਜੇ_ਅੰਤ
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
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_COMPL_FUNC
    , BV_OFU
#ਜੇ_ਅੰਤ
    , BV_ਰਸਤਾ
    , BV_PI
    , BV_QE
    , BV_RO
    , BV_SI
    , BV_SN
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
    , BV_SMC
    , BV_SYN
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SPELL
    , BV_SPC
    , BV_SPF
    , BV_SPL
    , BV_SPO
#ਜੇ_ਅੰਤ
    , BV_STS
    , BV_SUA
    , BV_SW
    , BV_SWF
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    , BV_TFU
#ਜੇ_ਅੰਤ
    , BV_ਟੈਗ_ਸੂਚੀ
    , BV_TC
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_COMPL_FUNC
    , BV_TSRFU
#ਜੇ_ਅੰਤ
    , BV_TS
    , BV_TW
    , BV_TX
    , BV_UDF
    , BV_UL
    , BV_WM
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
    , BV_TWSL
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VARTABS
    , BV_VSTS
    , BV_VTS
#ਜੇ_ਅੰਤ
    , BV_ਗਿਣਤੀ	    // must be the last one
};

/*
 * "indir" values for window-local options.
 * These need to be defined globally, so that the WV_COUNT can be used in the
 * window structure.
 */
ਗਿਣਤੀ
{
    WV_LIST = 0
    , WV_LCS
    , WV_FCS
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ARABIC
    , WV_ARAB
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CONCEAL
    , WV_COCU
    , WV_COLE
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
    , WV_TWK
    , WV_TWS
#ਜੇ_ਅੰਤ
    , WV_CRBIND
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
    , WV_BRI
    , WV_BRIOPT
#ਜੇ_ਅੰਤ
    , WV_WCR
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
    , WV_DIFF
#ਜੇ_ਅੰਤ
    , WV_EIW
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
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
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
    , WV_LBR
#ਜੇ_ਅੰਤ
    , WV_NU
    , WV_RNU
    , WV_VE
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
    , WV_NUW
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_QUICKFIX)
    , WV_PVW
    , WV_LHI
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
    , WV_RL
    , WV_RLC
#ਜੇ_ਅੰਤ
    , WV_SCBIND
    , WV_SCROLL
    , WV_SMS
    , WV_SISO
    , WV_SO
    , WV_SOP
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SPELL
    , WV_SPELL
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
    , WV_CUC
    , WV_CUL
    , WV_CULOPT
    , WV_CC
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
    , WV_SBR
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_STL_OPT
    , WV_STL
    , WV_STLO
#ਜੇ_ਅੰਤ
    , WV_WFB
    , WV_WFH
    , WV_WFW
    , WV_WHL
    , WV_WRAP
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SIGNS
    , WV_SCL
#ਜੇ_ਅੰਤ
    , WV_ਗਿਣਤੀ	    // must be the last one
};

// Value for b_p_ul indicating the global value must be used.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NO_LOCAL_UNDOLEVEL (-123456)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ERR_BUFLEN 80

#ਜੇ_ਅੰਤ // _OPTION_H_
