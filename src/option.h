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

#جيڪڏهن_وصف_نه_ٿيل _OPTION_H_
#وصف_ڪريو _OPTION_H_

//
// Option Flags
//
#وصف_ڪريو P_BOOL		0x01	// the option is boolean
#وصف_ڪريو P_انگ		0x02	// the option is numeric
#وصف_ڪريو P_STRING	0x04	// the option is a string
#وصف_ڪريو P_ALLOCED	0x08	// the string option is in allocated memory,
				// must use free_string_option() when
				// assigning new value. Not set if default is
				// the same.
#وصف_ڪريو P_EXPAND	0x10	// environment expansion.  NOTE: P_EXPAND can
				// never be used for local or hidden options!
#وصف_ڪريو P_NO_CMD_EXPAND	0x20	// don't perform cmdline completions
#وصف_ڪريو P_NODEFAULT	0x40	// don't set to default value
#وصف_ڪريو P_DEF_ALLOCED	0x80	// default value is in allocated memory, must
				//  use vim_free() when assigning new value
#وصف_ڪريو P_WAS_سيٽ	0x100	// option has been set/reset
#وصف_ڪريو P_NO_MKRC	0x200	// don't include in :mkvimrc output
#وصف_ڪريو P_VI_DEF	0x400	// Use Vi default for Vim
#وصف_ڪريو P_VIM		0x800	// Vim option, reset when 'cp' set

				// when option changed, what to display:
#وصف_ڪريو P_RSTAT		0x1000	// redraw status lines
#وصف_ڪريو P_RWIN		0x2000	// redraw current window and recompute text
#وصف_ڪريو P_RBUF		0x4000	// redraw current buffer and recompute text
#وصف_ڪريو P_RALL		0x6000	// redraw all windows and recompute text
#وصف_ڪريو P_RCLR		0x7000	// clear and redraw all and recompute text

#وصف_ڪريو P_COMMA		 0x8000	 // comma separated list
#وصف_ڪريو P_ONECOMMA	0x18000L // P_COMMA and cannot have two consecutive
				 // commas
#وصف_ڪريو P_NODUP		0x20000L // don't allow duplicate strings
#وصف_ڪريو P_FLAGLIST	0x40000L // list of single-char flags

#وصف_ڪريو P_SECURE	0x80000L // cannot change in modeline or secure mode
#وصف_ڪريو P_GETTEXT      0x100000L // expand default value with _()
#وصف_ڪريو P_NOGLOB       0x200000L // do not use local value for global vimrc
#وصف_ڪريو P_NFNAME       0x400000L // only normal file name chars allowed
#وصف_ڪريو P_INSECURE     0x800000L // option was set from a modeline
#وصف_ڪريو P_PRI_MKRC    0x1000000L // priority for :mkvimrc (setting option has
				 // side effects)
#وصف_ڪريو P_NO_ML       0x2000000L // not allowed in modeline
#وصف_ڪريو P_CURSWANT    0x4000000L // update curswant required; not needed when
				 // there is a redraw flag
#وصف_ڪريو P_NDNAME      0x8000000L // only normal dir name chars allowed
#وصف_ڪريو P_HLONLY     0x10000000L // option only changes highlight, not text
#وصف_ڪريو P_MLE	     0x20000000L // under control of 'modelineexpr'
#وصف_ڪريو P_FUNC	     0x40000000L // accept a function reference or a lambda
#وصف_ڪريو P_COLON	     0x80000000L // values use colons to create sublists
// Warning: Currently we have used all 32 bits for option flags. On some 32-bit
//          systems, the flags are stored as a 32-bit integer, and adding more
//          flags will overflow it. Adding another flag will need to change how
//          it's stored first.

// Returned by get_option_value().
تعريفنوع تعداد {
    gov_unknown,
    gov_bool,
    gov_عدد,
    gov_string,
    gov_hidden_bool,
    gov_hidden_عدد,
    gov_hidden_string
} getoption_T;

/*
 * Default values for 'errorformat'.
 * The "%f|%l| %m" one is used for when the contents of the quickfix window is
 * written to a file.
 */
#جيڪڏهن_وصف_ٿيل AMIGA
# define DFLT_EFM	"%f>%l:%c:%t:%n:%m,%f:%l: %t%*\\D%n: %m,%f %l %t%*\\D%n: %m,%*[^\"]\"%f\"%*\\D%l: %m,%f:%l:%m,%f|%l| %m"
#وإلا
# إذا defined(MSWIN)
#  define DFLT_EFM	"%f(%l): %t%*\\D%n: %m,%f(%l\\,%c): %t%*\\D%n: %m,%f(%l) \\=: %t%*\\D%n: %m,%*[^\"]\"%f\"%*\\D%l: %m,%f(%l) \\=: %m,%*[^ ] %f %l: %m,%f:%l:%c:%m,%f(%l):%m,%f:%l:%m,%f|%l| %m"
# وإلا
#  إذا defined(__QNX__)
#   define DFLT_EFM	"%f(%l):%*[^WE]%t%*\\D%n:%m,%f|%l| %m"
#  وإلا
#   ifdef VMS
#    define DFLT_EFM	"%A%p^,%C%%CC-%t-%m,%Cat line number %l in file %f,%f|%l| %m"
#   وإلا // Unix, probably
#    define DFLT_EFM	"%*[^\"]\"%f\"%*\\D%l: %m,\"%f\"%*\\D%l: %m,%-Gg%\\?make[%*\\d]: *** [%f:%l:%m,%-Gg%\\?make: *** [%f:%l:%m,%-G%f:%l: (Each undeclared identifier is reported only once,%-G%f:%l: for each function it appears in.),%-GIn file included from %f:%l:%c:,%-GIn file included from %f:%l:%c\\,,%-GIn file included from %f:%l:%c,%-GIn file included from %f:%l,%-G%*[ ]from %f:%l:%c,%-G%*[ ]from %f:%l:,%-G%*[ ]from %f:%l\\,,%-G%*[ ]from %f:%l,%f:%l:%c:%m,%f(%l):%m,%f:%l:%m,\"%f\"\\, line %l%*\\D%c%*[^ ] %m,%D%*\\a[%*\\d]: Entering directory %*[`']%f',%X%*\\a[%*\\d]: Leaving directory %*[`']%f',%D%*\\a: Entering directory %*[`']%f',%X%*\\a: Leaving directory %*[`']%f',%DMaking %*\\a in %f,%f|%l| %m"
#   endif
#  endif
# endif
#جيڪڏهن_پورو

#وصف_ڪريو DFLT_GREPFORMAT	"%f:%l:%m,%f:%l%m,%f  %l%m"

// default values for b_p_ff 'fileformat' and p_ffs 'fileformats'
#وصف_ڪريو FF_DOS		"dos"
#وصف_ڪريو FF_MAC		"mac"
#وصف_ڪريو FF_UNIX		"unix"

#جيڪڏهن_وصف_ٿيل USE_CRNL
# define DFLT_FF	"dos"
# define DFLT_FFS_VIM	"dos,unix"
# define DFLT_FFS_VI	"dos,unix"	// also autodetect in compatible mode
# define DFLT_TEXTAUTO	TRUE
#وإلا
# define DFLT_FF	"unix"
# define DFLT_FFS_VIM	"unix,dos"
# ifdef __CYGWIN__
#  define DFLT_FFS_VI	"unix,dos"	// Cygwin always needs file detection
#  define DFLT_TEXTAUTO TRUE
# وإلا
#  define DFLT_FFS_VI	""
#  define DFLT_TEXTAUTO FALSE
# endif
#جيڪڏهن_پورو


// Possible values for 'encoding'
#وصف_ڪريو ENC_UCSBOM	"ucs-bom"	// check for BOM at start of file

// default value for 'encoding'
#إذا defined(MSWIN) || defined(__MVS__)
# define ENC_DFLT	"utf-8"
#وإلا
# define ENC_DFLT	"latin1"
#جيڪڏهن_پورو

// end-of-line style
#وصف_ڪريو EOL_UNKNOWN	(-1)	// not defined yet
#وصف_ڪريو EOL_UNIX	0	// NL
#وصف_ڪريو EOL_DOS		1	// CR NL
#وصف_ڪريو EOL_MAC		2	// CR

// Formatting options for p_fo 'formatoptions'
#وصف_ڪريو FO_WRAP		't'
#وصف_ڪريو FO_WRAP_COMS	'c'
#وصف_ڪريو FO_RET_COMS	'r'
#وصف_ڪريو FO_OPEN_COMS	'o'
#وصف_ڪريو FO_NO_OPEN_COMS	'/'
#وصف_ڪريو FO_Q_COMS	'q'
#وصف_ڪريو FO_Q_عدد	'n'
#وصف_ڪريو FO_Q_ٻيو	'2'
#وصف_ڪريو FO_INS_VI	'v'
#وصف_ڪريو FO_INS_LONG	'l'
#وصف_ڪريو FO_INS_BLANK	'b'
#وصف_ڪريو FO_MBYTE_BREAK	'm'	// break before/after multi-byte char
#وصف_ڪريو FO_MBYTE_JOIN	'M'	// no space before/after multi-byte char
#وصف_ڪريو FO_MBYTE_JOIN2	'B'	// no space between multi-byte chars
#وصف_ڪريو FO_ONE_LETTER	'1'
#وصف_ڪريو FO_WHITE_PAR	'w'	// trailing white space continues paragr.
#وصف_ڪريو FO_AUTO		'a'	// automatic formatting
#وصف_ڪريو FO_RIGOROUS_TW	']'     // respect textwidth rigorously
#وصف_ڪريو FO_REMOVE_COMS	'j'	// remove comment leaders when joining lines
#وصف_ڪريو FO_PERIOD_ABBR	'p'	// don't break a single space after a period

#وصف_ڪريو DFLT_FO_VI	"vt"
#وصف_ڪريو DFLT_FO_VIM	"tcq"
#وصف_ڪريو FO_ALL		"tcro/q2vlb1mMBn,aw]jp"	// for do_set()

// characters for the p_cpo option:
#وصف_ڪريو CPO_ALTREAD	'a'	// ":read" sets alternate file name
#وصف_ڪريو CPO_ALTWRITE	'A'	// ":write" sets alternate file name
#وصف_ڪريو CPO_BAR		'b'	// "\|" ends a mapping
#وصف_ڪريو CPO_BSLASH	'B'	// backslash in mapping is not special
#وصف_ڪريو CPO_ڳولا	'c'
#وصف_ڪريو CPO_CONCAT	'C'	// Don't concatenate sourced lines
#وصف_ڪريو CPO_DOTTAG	'd'	// "./tags" in 'tags' is in current dir
#وصف_ڪريو CPO_DIGRAPH	'D'	// No digraph after "r", "f", etc.
#وصف_ڪريو CPO_EXECBUF	'e'
#وصف_ڪريو CPO_EMPTYREGION	'E'	// operating on empty region is an error
#وصف_ڪريو CPO_FNAMER	'f'	// set file name for ":r file"
#وصف_ڪريو CPO_FNAMEW	'F'	// set file name for ":w file"
#وصف_ڪريو CPO_GOTO1	'g'	// goto line 1 for ":edit"
#وصف_ڪريو CPO_INSEND	'H'	// "I" inserts before last blank in line
#وصف_ڪريو CPO_INTMOD	'i'	// interrupt a read makes buffer modified
#وصف_ڪريو CPO_INDENT	'I'	// remove auto-indent more often
#وصف_ڪريو CPO_JOINSP	'j'	// only use two spaces for join after '.'
#وصف_ڪريو CPO_ENDOFSENT	'J'	// need two spaces to detect end of sentence
#وصف_ڪريو CPO_KEYCODE	'k'	// don't recognize raw key code in mappings
#وصف_ڪريو CPO_KOFFSET	'K'	// don't wait for key code in mappings
#وصف_ڪريو CPO_LITERAL	'l'	// take char after backslash in [] literal
#وصف_ڪريو CPO_LISTWM	'L'	// 'list' changes wrapmargin
#وصف_ڪريو CPO_SHOWMATCH	'm'
#وصف_ڪريو CPO_MATCHBSL	'M'	// "%" ignores use of backslashes
#وصف_ڪريو CPO_NUMCOL	'n'	// 'number' column also used for text
#وصف_ڪريو CPO_LINEOFF	'o'
#وصف_ڪريو CPO_OVERNEW	'O'	// silently overwrite new file
#وصف_ڪريو CPO_LISP	'p'	// 'lisp' indenting
#وصف_ڪريو CPO_FNAMEAPP	'P'	// set file name for ":w >>file"
#وصف_ڪريو CPO_JOINCOL	'q'	// with "3J" use column after first join
#وصف_ڪريو CPO_REDO	'r'
#وصف_ڪريو CPO_REMMARK	'R'	// remove marks when filtering
#وصف_ڪريو CPO_BUFOPT	's'
#وصف_ڪريو CPO_BUFOPTGLOB	'S'
#وصف_ڪريو CPO_TAGPAT	't'	// tag pattern is used for "n"
#وصف_ڪريو CPO_UNDO	'u'	// "u" undoes itself
#وصف_ڪريو CPO_BACKSPACE	'v'	// "v" keep deleted text
#وصف_ڪريو CPO_CW		'w'	// "cw" only changes one blank
#وصف_ڪريو CPO_FWRITE	'W'	// "w!" doesn't overwrite readonly files
#وصف_ڪريو CPO_ESC		'x'
#وصف_ڪريو CPO_REPLCNT	'X'	// "R" with a count only deletes chars once
#وصف_ڪريو CPO_YANK	'y'
#وصف_ڪريو CPO_KEEPRO	'Z'	// don't reset 'readonly' on ":w!"
#وصف_ڪريو CPO_لفظ	'z'	// do not special-case word motions cw and dw
#وصف_ڪريو CPO_DOLLAR	'$'
#وصف_ڪريو CPO_FILTER	'!'
#وصف_ڪريو CPO_ميلاپ	'%'
#وصف_ڪريو CPO_تارو	'*'	// ":*" means ":@"
#وصف_ڪريو CPO_PLUS	'+'	// ":write file" resets 'modified'
#وصف_ڪريو CPO_MINUS	'-'	// "9-" fails at and before line 9
#وصف_ڪريو CPO_SPECI	'<'	// don't recognize <> in mappings
#وصف_ڪريو CPO_REGAPPEND	'>'	// insert NL when appending to a register
// POSIX flags
#وصف_ڪريو CPO_HASH	'#'	// "D", "o" and "O" do not use a count
#وصف_ڪريو CPO_PARA	'{'	// "{" is also a paragraph boundary
#وصف_ڪريو CPO_TSIZE	'|'	// $LINES and $COLUMNS overrule term size
#وصف_ڪريو CPO_PRESERVE	'&'	// keep swap file after :preserve
#وصف_ڪريو CPO_SUBPERCENT	'/'	// % in :s string uses previous one
#وصف_ڪريو CPO_BACKSL	'\\'	// \ is not special in []
#وصف_ڪريو CPO_CHDIR	'.'	// don't chdir if buffer is modified
#وصف_ڪريو CPO_SCOLON	';'	// using "," and ";" will skip over char if
				// cursor would not move
#وصف_ڪريو CPO_NOSYMLINKS	'~'	// don't resolve symlinks when changing directory
// default values for Vim, Vi and POSIX
#وصف_ڪريو CPO_VIM		"aABceFsz"
#وصف_ڪريو CPO_VI		"aAbBcCdDeEfFgHiIjJkKlLmMnoOpPqrRsStuvwWxXyZz$!%*-+<>;"
#وصف_ڪريو CPO_ALL		"aAbBcCdDeEfFgHiIjJkKlLmMnoOpPqrRsStuvwWxXyZz$!%*-+<>#{|&/\\.;~"

// characters for p_ww option:
#وصف_ڪريو WW_ALL		"bshl<>[]~"

// characters for p_mouse option:
#وصف_ڪريو MOUSE_عام	'n'		// use mouse in Normal mode
#وصف_ڪريو MOUSE_VISUAL	'v'		// use mouse in Visual/Select mode
#وصف_ڪريو MOUSE_وجهو	'i'		// use mouse in Insert mode
#وصف_ڪريو MOUSE_COMMAND	'c'		// use mouse in Command-line mode
#وصف_ڪريو MOUSE_HELP	'h'		// use mouse in help buffers
#وصف_ڪريو MOUSE_RETURN	'r'		// use mouse for hit-return message
#وصف_ڪريو MOUSE_A		"nvich"		// used for 'a' flag
#وصف_ڪريو MOUSE_ALL	"anvichr"	// all possible characters
#وصف_ڪريو MOUSE_NONE	' '		// don't use Visual selection
#وصف_ڪريو MOUSE_NONEF	'x'		// forced modeless selection

#وصف_ڪريو COCU_ALL	"nvic"		// flags for 'concealcursor'

// characters for p_shm option:
#وصف_ڪريو SHM_RO		'r'		// readonly
#وصف_ڪريو SHM_MOD		'm'		// modified
#وصف_ڪريو SHM_FILE	'f'		// (file 1 of 2)
#وصف_ڪريو SHM_آخري	'i'		// last line incomplete
#وصف_ڪريو SHM_متن	'x'		// tx instead of textmode
#وصف_ڪريو SHM_سٽون	'l'		// "L" instead of "lines"
#وصف_ڪريو SHM_NEW		'n'		// "[New]" instead of "[New file]"
#وصف_ڪريو SHM_WRI		'w'		// "[w]" instead of "written"
#وصف_ڪريو SHM_A		"rmfixlnw"	// represented by 'a' flag
#وصف_ڪريو SHM_WRITE	'W'		// don't use "written" at all
#وصف_ڪريو SHM_TRUNC	't'		// truncate file messages
#وصف_ڪريو SHM_TRUNCALL	'T'		// truncate all messages
#وصف_ڪريو SHM_OVER	'o'		// overwrite file messages
#وصف_ڪريو SHM_OVERALL	'O'		// overwrite more messages
#وصف_ڪريو SHM_ڳولا	's'		// no search hit bottom messages
#وصف_ڪريو SHM_ATTENTION	'A'		// no ATTENTION messages
#وصف_ڪريو SHM_INTRO	'I'		// intro messages
#وصف_ڪريو SHM_COMPLETIONMENU  'c'		// completion menu messages
#وصف_ڪريو SHM_COMPLETIONSCAN  'C'		// completion scanning messages
#وصف_ڪريو SHM_RECORDING	'q'		// short recording message
#وصف_ڪريو SHM_FILEINFO	'F'		// no file info messages
#وصف_ڪريو SHM_SEARCHCOUNT  'S'		// no search stats: '[1/10]'
#وصف_ڪريو SHM_UNDO	'u'		// undo messages
#وصف_ڪريو SHM_POSIX       "AS"		// POSIX value
#وصف_ڪريو SHM_ALL		"rmfixlnwaWtToOsAIcCqFSu" // all possible flags for 'shm'
#وصف_ڪريو SHM_LEN		30		// max length of all flags together
					// plus a NUL character

// characters for p_go:
#وصف_ڪريو GO_TERMINAL	'!'		// use terminal for system commands
#وصف_ڪريو GO_ASEL		'a'		// autoselect
#وصف_ڪريو GO_ASELML	'A'		// autoselect modeless selection
#وصف_ڪريو GO_BOT		'b'		// use bottom scrollbar
#وصف_ڪريو GO_CONDIALOG	'c'		// use console dialog
#وصف_ڪريو GO_TITLEBAR	'C'		// use 'hl-TitleBar'
#وصف_ڪريو GO_DARKTHEME	'd'		// use dark theme variant
#وصف_ڪريو GO_TABLINE	'e'		// may show tabline
#وصف_ڪريو GO_FORG		'f'		// start GUI in foreground
#وصف_ڪريو GO_GREY		'g'		// use grey menu items
#وصف_ڪريو GO_HORSCROLL	'h'		// flexible horizontal scrolling
#وصف_ڪريو GO_ICON		'i'		// use Vim icon
#وصف_ڪريو GO_کاٻو		'l'		// use left scrollbar
#وصف_ڪريو GO_VLEFT	'L'		// left scrollbar with vert split
#وصف_ڪريو GO_MENUS	'm'		// use menu bar
#وصف_ڪريو GO_NOSYSMENU	'M'		// don't source system menu
#وصف_ڪريو GO_اشارو	'p'		// pointer enter/leave callbacks
#وصف_ڪريو GO_ASELPLUS	'P'		// autoselectPlus
#وصف_ڪريو GO_ساڄو	'r'		// use right scrollbar
#وصف_ڪريو GO_VRIGHT	'R'		// right scrollbar with vert split
#وصف_ڪريو GO_FULLSCREEN	's'		// enter fullscreen
#وصف_ڪريو GO_TEAROFF	't'		// add tear-off menu items
#وصف_ڪريو GO_TOOLBAR	'T'		// add toolbar
#وصف_ڪريو GO_FOOTER	'F'		// add footer
#وصف_ڪريو GO_VERTICAL	'v'		// arrange dialog buttons vertically
#وصف_ڪريو GO_KEEPWINSIZE	'k'		// keep GUI window size
// all possible flags for 'go'
#وصف_ڪريو GO_ALL		"!aAbcCdefFghilLmMpPrRstTvk"

// flags for 'comments' option
#وصف_ڪريو COM_NEST	'n'		// comments strings nest
#وصف_ڪريو COM_BLANK	'b'		// needs blank after string
#وصف_ڪريو COM_شروعات	's'		// start of comment
#وصف_ڪريو COM_وچ	'm'		// middle of comment
#وصف_ڪريو COM_پڇاڙي		'e'		// end of comment
#وصف_ڪريو COM_AUTO_پڇاڙي	'x'		// last char of end closes comment
#وصف_ڪريو COM_پهريون	'f'		// first line comment only
#وصف_ڪريو COM_کاٻو	'l'		// left adjusted
#وصف_ڪريو COM_ساڄو	'r'		// right adjusted
#وصف_ڪريو COM_NOBACK	'O'		// don't use for "O" command
#وصف_ڪريو COM_ALL		"nbsmexflrO"	// all flags for 'comments' option
#وصف_ڪريو COM_MAX_LEN	50		// maximum length of a part

// flags for 'statusline' option
#وصف_ڪريو STL_FILEPATH	'f'		// path of file in buffer
#وصف_ڪريو STL_FULLPATH	'F'		// full path of file in buffer
#وصف_ڪريو STL_FILENAME	't'		// last part (tail) of file path
#وصف_ڪريو STL_ڪالم	'c'		// column og cursor
#وصف_ڪريو STL_VIRTCOL	'v'		// virtual column
#وصف_ڪريو STL_VIRTCOL_ALT	'V'		// - with 'if different' display
#وصف_ڪريو STL_سٽ	'l'		// line number of cursor
#وصف_ڪريو STL_NUMLINES	'L'		// number of lines in buffer
#وصف_ڪريو STL_BUFNO	'n'		// current buffer number
#وصف_ڪريو STL_KEYMAP	'k'		// 'keymap' when active
#وصف_ڪريو STL_وٿي	'o'		// offset of character under cursor
#وصف_ڪريو STL_وٿي_X	'O'		// - in hexadecimal
#وصف_ڪريو STL_BYTEVAL	'b'		// byte value of character
#وصف_ڪريو STL_BYTEVAL_X	'B'		// - in hexadecimal
#وصف_ڪريو STL_ROFLAG	'r'		// readonly flag
#وصف_ڪريو STL_ROFLAG_ALT	'R'		// - other display
#وصف_ڪريو STL_HELPFLAG	'h'		// window is showing a help file
#وصف_ڪريو STL_HELPFLAG_ALT 'H'		// - other display
#وصف_ڪريو STL_FILETYPE	'y'		// 'filetype'
#وصف_ڪريو STL_FILETYPE_ALT 'Y'		// - other display
#وصف_ڪريو STL_PREVIEWFLAG	'w'		// window is showing the preview buf
#وصف_ڪريو STL_PREVIEWFLAG_ALT 'W'		// - other display
#وصف_ڪريو STL_MODIFIED	'm'		// modified flag
#وصف_ڪريو STL_MODIFIED_ALT 'M'		// - other display
#وصف_ڪريو STL_QUICKFIX	'q'		// quickfix window description
#وصف_ڪريو STL_PERCENTAGE	'p'		// percentage through file
#وصف_ڪريو STL_ALTPERCENT	'P'		// percentage as TOP BOT ALL or NN%
#وصف_ڪريو STL_ARGLISTSTAT	'a'		// argument list status as (x of y)
#وصف_ڪريو STL_PAGENUM	'N'		// page number (when printing)
#وصف_ڪريو STL_SHOWCMD	'S'		// 'showcmd' buffer
#وصف_ڪريو STL_VIM_اظهار	'{'		// start of expression to substitute
#وصف_ڪريو STL_SEPARATE	'='		// separation between alignment
					// sections
#وصف_ڪريو STL_TRUNCMARK	'<'		// truncation mark if line is too long
#وصف_ڪريو STL_USER_HL	'*'		// highlight from (User)1..9 or 0
#وصف_ڪريو STL_HIGHLIGHT	'#'		// highlight name
#وصف_ڪريو STL_LINEBREAK	'@'		// insert a line break
#وصف_ڪريو STL_CLICKFUNC	'['		// click handler region
#وصف_ڪريو STL_TABPAGENR	'T'		// tab page label nr
#وصف_ڪريو STL_TABCLOSENR	'X'		// tab page close nr
#وصف_ڪريو STL_ALL		((char_u *) "fFtcvVlLknoObBrRhHmYyWwMqpPaNS{#@[")

// flags used for parsed 'wildmode'
#وصف_ڪريو WIM_ڀريل	0x01
#وصف_ڪريو WIM_LONGEST	0x02
#وصف_ڪريو WIM_LIST	0x04
#وصف_ڪريو WIM_BUFLASTUSED	0x08
#وصف_ڪريو WIM_NOSELECT	0x10
#وصف_ڪريو WIM_NOINSERT	0x20

// flags for the 'wildoptions' option
// each defined char should be unique over all values.
#وصف_ڪريو WOP_FUZZY	'z'
#وصف_ڪريو WOP_TAGFILE	'g'
#وصف_ڪريو WOP_PUM		'p'
#وصف_ڪريو WOP_EXACTTEXT	'x'

// arguments for can_bs()
// each defined char should be unique over all values
// except for BS_START, that intentionally also matches BS_NOSTOP
// because BS_NOSTOP behaves exactly the same except it
// does not stop at the start of the insert point
#وصف_ڪريو BS_INDENT	'i'	// "Indent"
#وصف_ڪريو BS_EOL		'l'	// "eoL"
#وصف_ڪريو BS_شروعات	's'	// "Start"
#وصف_ڪريو BS_NOSTOP	'p'	// "nostoP

// flags for the 'culopt' option
#وصف_ڪريو CULOPT_سٽ	0x01	// Highlight complete line
#وصف_ڪريو CULOPT_SCRLINE	0x02	// Highlight screen line
#وصف_ڪريو CULOPT_NBR	0x04	// Highlight Number column

#وصف_ڪريو LISPWORD_قدر	"defun,define,defmacro,set!,lambda,if,case,let,flet,let*,letrec,do,do*,define-syntax,let-syntax,letrec-syntax,destructuring-bind,defpackage,defparameter,defstruct,deftype,defvar,do-all-symbols,do-external-symbols,do-symbols,dolist,dotimes,ecase,etypecase,eval-when,labels,macrolet,multiple-value-bind,multiple-value-call,multiple-value-prog1,multiple-value-setq,prog1,progv,typecase,unless,unwind-protect,when,with-input-from-string,with-open-file,with-open-stream,with-output-to-string,with-package-iterator,define-condition,handler-bind,handler-case,restart-bind,restart-case,with-simple-restart,store-value,use-value,muffle-warning,abort,continue,with-slots,with-slots*,with-accessors,with-accessors*,defclass,defmethod,print-unreadable-object"

/*
 * The following are actual variables for the options
 */

#جيڪڏهن_وصف_ٿيل FEAT_RIGHTLEFT
EXTERN ڊگهو	p_aleph;	// 'aleph'
#جيڪڏهن_پورو
EXTERN char_u	*p_ambw;	// 'ambiwidth'
#جيڪڏهن_وصف_ٿيل FEAT_AUTOCHDIR
EXTERN صحيح	p_acd;		// 'autochdir'
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_AUTOSHELLDIR
EXTERN صحيح	p_asd;		// 'autoshelldir'
#جيڪڏهن_پورو
EXTERN صحيح	p_ai;		// 'autoindent'
EXTERN صحيح	p_bin;		// 'binary'
EXTERN صحيح	p_bomb;		// 'bomb'
EXTERN صحيح	p_bl;		// 'buflisted'
EXTERN صحيح	p_cin;		// 'cindent'
EXTERN char_u	*p_cink;	// 'cinkeys'
EXTERN char_u	*p_cinsd;	// 'cinscopedecls'
EXTERN char_u	*p_cinw;	// 'cinwords'
#جيڪڏهن_وصف_ٿيل FEAT_COMPL_FUNC
EXTERN char_u	*p_cfu;		// 'completefunc'
EXTERN char_u	*p_ofu;		// 'omnifunc'
EXTERN char_u	*p_tsrfu;	// 'thesaurusfunc'
#جيڪڏهن_پورو
EXTERN صحيح	p_ci;		// 'copyindent'
#إذا defined(FEAT_GUI) && defined(MACOS_X)
EXTERN صحيح	*p_antialias;	// 'antialias'
#جيڪڏهن_پورو
EXTERN صحيح	p_ar;		// 'autoread'
EXTERN صحيح	p_aw;		// 'autowrite'
EXTERN صحيح	p_awa;		// 'autowriteall'
EXTERN char_u	*p_bs;		// 'backspace'
EXTERN char_u	*p_bg;		// 'background'
EXTERN صحيح	p_bk;		// 'backup'
EXTERN char_u	*p_bkc;		// 'backupcopy'
EXTERN بنانشان	bkc_flags;	// flags from 'backupcopy'
#وصف_ڪريو BKC_YES		0x001
#وصف_ڪريو BKC_AUTO		0x002
#وصف_ڪريو BKC_NO			0x004
#وصف_ڪريو BKC_BREAKSYMLINK	0x008
#وصف_ڪريو BKC_BREAKHARDLINK	0x010
EXTERN char_u	*p_bdir;	// 'backupdir'
EXTERN char_u	*p_bex;		// 'backupext'
EXTERN char_u	*p_bo;		// 'belloff'
EXTERN بنانشان	bo_flags;

// values for the 'belloff' option
#وصف_ڪريو BO_ALL		0x0001
#وصف_ڪريو BO_BS		0x0002
#وصف_ڪريو BO_CRSR		0x0004
#وصف_ڪريو BO_COMPL	0x0008
#وصف_ڪريو BO_COPY		0x0010
#وصف_ڪريو BO_CTRLG	0x0020
#وصف_ڪريو BO_ERROR	0x0040
#وصف_ڪريو BO_ESC		0x0080
#وصف_ڪريو BO_EX		0x0100
#وصف_ڪريو BO_HANGUL	0x0200
#وصف_ڪريو BO_IM		0x0400
#وصف_ڪريو BO_LANG		0x0800
#وصف_ڪريو BO_MESS		0x1000
#وصف_ڪريو BO_ميلاپ	0x2000
#وصف_ڪريو BO_OPER		0x4000
#وصف_ڪريو BO_REG		0x8000
#وصف_ڪريو BO_SH		0x10000
#وصف_ڪريو BO_SPELL	0x20000
#وصف_ڪريو BO_رڪن		0x40000
#وصف_ڪريو BO_WILD		0x80000

EXTERN char_u	*p_bsk;		// 'backupskip'
#جيڪڏهن_وصف_ٿيل FEAT_CRYPT
EXTERN char_u	*p_cm;		// 'cryptmethod'
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_BEVAL
# ifdef FEAT_BEVAL_GUI
EXTERN صحيح	p_beval;	// 'ballooneval'
# endif
EXTERN ڊگهو	p_bdlay;	// 'balloondelay'
# ifdef FEAT_EVAL
EXTERN char_u	*p_bexpr;
# endif
# ifdef FEAT_BEVAL_رڪن
EXTERN صحيح	p_bevalterm;	// 'balloonevalterm'
# endif
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_BROWSE
EXTERN char_u	*p_bsdir;	// 'browsedir'
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_LINEBREAK
EXTERN char_u	*p_breakat;	// 'breakat'
#جيڪڏهن_پورو
EXTERN char_u	*p_bh;		// 'bufhidden'
EXTERN char_u	*p_bt;		// 'buftype'
EXTERN char_u	*p_cmp;		// 'casemap'
EXTERN بنانشان	cmp_flags;
#وصف_ڪريو CMP_اندروني		0x001
#وصف_ڪريو CMP_KEEPASCII		0x002
EXTERN char_u	*p_enc;		// 'encoding'
EXTERN صحيح	p_deco;		// 'delcombine'
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
EXTERN char_u	*p_ccv;		// 'charconvert'
#جيڪڏهن_پورو
EXTERN صحيح	p_cdh;		// 'cdhome'
EXTERN char_u	*p_cino;	// 'cinoptions'
EXTERN char_u	*p_cedit;	// 'cedit'
EXTERN ڊگهو	p_cwh;		// 'cmdwinheight'
#جيڪڏهن_وصف_ٿيل HAVE_CLIPMETHOD
EXTERN char_u	*p_cb;		// 'clipboard'
EXTERN char_u	*p_cpm;		// 'clipmethod'
#جيڪڏهن_پورو
EXTERN ڊگهو	p_ch;		// 'cmdheight'
EXTERN char_u	*p_cms;		// 'commentstring'
EXTERN char_u	*p_cpt;		// 'complete'
EXTERN ڊگهو	p_cto;		// 'completetimeout'
#إذا defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
EXTERN صحيح	p_confirm;	// 'confirm'
#جيڪڏهن_پورو
EXTERN صحيح	p_cp;		// 'compatible'
EXTERN char_u	*p_cfc;		// 'completefuzzycollect'
EXTERN بنانشان cfc_flags;	// flags from "completefuzzycollect"
EXTERN char_u	*p_cia;		// 'completeitemalign'
EXTERN بنانشان cia_flags;	// order flags of 'completeitemalign'
EXTERN char_u	*p_cot;		// 'completeopt'
EXTERN بنانشان	cot_flags;	// flags from 'completeopt'
EXTERN صحيح	p_ac;		// 'autocomplete'
EXTERN ڊگهو	p_act;		// 'autocompletetimeout'
EXTERN ڊگهو	p_acl;		// 'autocompletedelay'
// Keep in sync with p_cot_values in optionstr.c
#وصف_ڪريو COT_MENU	0x001
#وصف_ڪريو COT_MENUONE	0x002
#وصف_ڪريو COT_ANY_MENU	0x003	// combination of menu flags
#وصف_ڪريو COT_LONGEST	0x004	// FALSE: insert full match,
				// TRUE: insert longest prefix
#وصف_ڪريو COT_PREVIEW	    0x008
#وصف_ڪريو COT_POPUP	    0x010
#وصف_ڪريو COT_POPUPHIDDEN	    0x020
#وصف_ڪريو COT_ANY_PREVIEW	    0x038   // combination of preview flags
#وصف_ڪريو COT_NOINSERT	    0x040   // FALSE: select & insert, TRUE: noinsert
#وصف_ڪريو COT_NOSELECT	    0x080   // FALSE: select & insert, TRUE: noselect
#وصف_ڪريو COT_FUZZY	    0x100   // TRUE: fuzzy match enabled
#وصف_ڪريو COT_NOSORT	    0x200   // TRUE: fuzzy match without qsort score
#وصف_ڪريو COT_PREINSERT	    0x400   // TRUE: preinsert
#وصف_ڪريو COT_سڀ_کان_ويجهو	    0x800   // TRUE: prioritize matches close to cursor

#وصف_ڪريو CFC_KEYWORD         0x001
#وصف_ڪريو CFC_FILES           0x002
#وصف_ڪريو CFC_WHOLELINE       0x004

#جيڪڏهن_وصف_ٿيل BACKSLASH_اندريون_FILENAME
EXTERN char_u	*p_csl;		// 'completeslash'
#جيڪڏهن_پورو
EXTERN ڊگهو	p_ph;		// 'pumheight'
EXTERN ڊگهو	p_pw;		// 'pumwidth'
EXTERN ڊگهو	p_pmw;		// 'pummaxwidth'
EXTERN char_u	*p_pb;		// 'pumborder'
EXTERN char_u	*p_pumopt;	// 'pumopt'
EXTERN char_u	*p_com;		// 'comments'
EXTERN char_u	*p_cpo;		// 'cpoptions'
#جيڪڏهن_وصف_ٿيل FEAT_CSCOPE
EXTERN char_u	*p_csprg;	// 'cscopeprg'
EXTERN صحيح	p_csre;		// 'cscoperelative'
# ifdef FEAT_QUICKFIX
EXTERN char_u	*p_csqf;	// 'cscopequickfix'
#  define	CSQF_CMDS   "sgdctefia"
#  define	CSQF_FLAGS  "+-0"
# endif
EXTERN صحيح	p_cst;		// 'cscopetag'
EXTERN ڊگهو	p_csto;		// 'cscopetagorder'
EXTERN ڊگهو	p_cspc;		// 'cscopepathcomp'
EXTERN صحيح	p_csverbose;	// 'cscopeverbose'
#جيڪڏهن_پورو
EXTERN char_u	*p_debug;	// 'debug'
#جيڪڏهن_وصف_ٿيل FEAT_ڳوليو_ID
EXTERN char_u	*p_def;		// 'define'
EXTERN char_u	*p_inc;
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_DIFF
EXTERN char_u	*p_dia;		// 'diffanchors'
EXTERN char_u	*p_dip;		// 'diffopt'
# ifdef FEAT_EVAL
EXTERN char_u	*p_dex;		// 'diffexpr'
# endif
#جيڪڏهن_پورو
EXTERN char_u	*p_dict;	// 'dictionary'
#جيڪڏهن_وصف_ٿيل FEAT_DIGRAPHS
EXTERN صحيح	p_dg;		// 'digraph'
#جيڪڏهن_پورو
EXTERN char_u	*p_dir;		// 'directory'
EXTERN char_u	*p_dy;		// 'display'
EXTERN بنانشان	dy_flags;
#وصف_ڪريو DY_LASTLINE		0x001
#وصف_ڪريو DY_TRUNCATE		0x002
#وصف_ڪريو DY_UHEX			0x004
EXTERN صحيح	p_ed;		// 'edcompatible'
EXTERN char_u	*p_ead;		// 'eadirection'
EXTERN char_u	*p_emoji;	// 'emoji'
EXTERN صحيح	p_ea;		// 'equalalways'
EXTERN char_u	*p_ep;		// 'equalprg'
EXTERN صحيح	p_eb;		// 'errorbells'
#جيڪڏهن_وصف_ٿيل FEAT_QUICKFIX
EXTERN char_u	*p_ef;		// 'errorfile'
EXTERN char_u	*p_efm;		// 'errorformat'
EXTERN char_u	*p_gefm;	// 'grepformat'
EXTERN char_u	*p_gp;		// 'grepprg'
#جيڪڏهن_پورو
EXTERN صحيح	p_eof;		// 'endoffile'
EXTERN صحيح	p_eol;		// 'endofline'
EXTERN صحيح	p_ek;		// 'esckeys'
EXTERN char_u	*p_ei;		// 'eventignore'
EXTERN صحيح	p_et;		// 'expandtab'
EXTERN صحيح	p_exrc;		// 'exrc'
EXTERN char_u	*p_fenc;	// 'fileencoding'
EXTERN char_u	*p_fencs;	// 'fileencodings'
EXTERN char_u	*p_ff;		// 'fileformat'
EXTERN char_u	*p_ffs;		// 'fileformats'
EXTERN صحيح	p_fic;		// 'fileignorecase'
EXTERN char_u	*p_ft;		// 'filetype'
EXTERN char_u	*p_fcs;		// 'fillchar'
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
EXTERN char_u	*p_ffu;		// 'findfunc'
#جيڪڏهن_پورو
EXTERN صحيح	p_fixeol;	// 'fixendofline'
#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
EXTERN char_u	*p_fcl;		// 'foldclose'
EXTERN ڊگهو	p_fdls;		// 'foldlevelstart'
EXTERN char_u	*p_fdo;		// 'foldopen'
EXTERN بنانشان	fdo_flags;
# define FDO_ALL		0x001
# define FDO_بلاڪ		0x002
# define FDO_HOR		0x004
# define FDO_MARK		0x008
# define FDO_PERCENT		0x010
# define FDO_QUICKFIX		0x020
# define FDO_ڳولا		0x040
# define FDO_ٽيگ		0x080
# define FDO_وجهو		0x100
# define FDO_UNDO		0x200
# define FDO_JUMP		0x400
#جيڪڏهن_پورو
#إذا defined(FEAT_EVAL)
EXTERN char_u	*p_fex;		// 'formatexpr'
#جيڪڏهن_پورو
EXTERN char_u	*p_flp;		// 'formatlistpat'
EXTERN char_u	*p_fo;		// 'formatoptions'
EXTERN char_u	*p_fp;		// 'formatprg'
#جيڪڏهن_وصف_ٿيل HAVE_FSYNC
EXTERN صحيح	p_fs;		// 'fsync'
#جيڪڏهن_پورو
EXTERN صحيح	p_gd;		// 'gdefault'
EXTERN char_u	*p_jop;		// 'jumpoptions'
EXTERN بنانشان	jop_flags;	//
#وصف_ڪريو JOP_ڍير		0x001
#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
# ifdef FEAT_QUICKFIX
EXTERN char_u	*p_cpp;		// 'completepopup'
# endif
EXTERN char_u	*p_pvp;		// 'previewpopup'
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_PRINTER
EXTERN char_u	*p_pdev;	// 'printdevice'
# ifdef FEAT_POSTSCRIPT
EXTERN char_u	*p_penc;	// 'printencoding'
EXTERN char_u	*p_pmfn;	// 'printmbfont'
EXTERN char_u	*p_pmcs;	// 'printmbcharset'
# endif
# إذا defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
EXTERN char_u	*p_pexpr;	// 'printexpr'
# endif
EXTERN char_u	*p_pfn;		// 'printfont'
EXTERN char_u	*p_popt;	// 'printoptions'
EXTERN char_u	*p_سرو;	// 'printheader'
#جيڪڏهن_پورو
EXTERN صحيح	p_prompt;	// 'prompt'
#جيڪڏهن_وصف_ٿيل FEAT_GUI
EXTERN char_u	*p_guifont;	// 'guifont'
# ifdef FEAT_XFONTSET
EXTERN char_u	*p_guifontset;	// 'guifontset'
# endif
EXTERN char_u	*p_guifontwide;	// 'guifontwide'
EXTERN صحيح	p_guipty;	// 'guipty'
#جيڪڏهن_پورو
#إذا defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
EXTERN char_u	*p_guiligatures;  // 'guiligatures'
#جيڪڏهن_پورو
#إذا (defined(FEAT_GUI_GTK) && !defined(USE_GTK4)) || defined(FEAT_GUI_X11)
EXTERN ڊگهو	p_ghr;		// 'guiheadroom'
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل CURSOR_SHAPE
EXTERN char_u	*p_guicursor;	// 'guicursor'
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_MOUSESHAPE
EXTERN char_u	*p_mouseshape;	// 'mouseshape'
#جيڪڏهن_پورو
#إذا defined(FEAT_GUI)
EXTERN char_u	*p_go;		// 'guioptions'
#جيڪڏهن_پورو
#إذا defined(FEAT_GUI_TABLINE)
EXTERN char_u	*p_gtl;		// 'guitablabel'
EXTERN char_u	*p_gtt;		// 'guitabtooltip'
#جيڪڏهن_پورو
EXTERN char_u	*p_hf;		// 'helpfile'
EXTERN ڊگهو	p_hh;		// 'helpheight'
#جيڪڏهن_وصف_ٿيل FEAT_MULTI_LANG
EXTERN char_u	*p_hlg;		// 'helplang'
#جيڪڏهن_پورو
EXTERN صحيح	p_hid;		// 'hidden'
EXTERN char_u	*p_hl;		// 'highlight'
EXTERN صحيح	p_hls;		// 'hlsearch'
EXTERN ڊگهو	p_hi;		// 'history'
#جيڪڏهن_وصف_ٿيل FEAT_RIGHTLEFT
EXTERN صحيح	p_hkmap;	// 'hkmap'
EXTERN صحيح	p_hkmapp;	// 'hkmapp'
# ifdef FEAT_ARABIC
EXTERN صحيح	p_arshape;	// 'arabicshape'
# endif
#جيڪڏهن_پورو
EXTERN صحيح	p_icon;		// 'icon'
EXTERN char_u	*p_iconstring;	// 'iconstring'
EXTERN صحيح	p_ic;		// 'ignorecase'
#إذا defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
EXTERN char_u	*p_imak;	// 'imactivatekey'
# define IM_ON_THE_SPOT		0L
# define IM_OVER_THE_SPOT	1L
EXTERN ڊگهو	p_imst;		// 'imstyle'
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_IMAGE
EXTERN char_u	*p_ipc;		// 'imageprotocol'
#جيڪڏهن_پورو
#إذا defined(FEAT_EVAL)
EXTERN char_u	*p_imaf;	// 'imactivatefunc'
EXTERN char_u	*p_imsf;	// 'imstatusfunc'
#جيڪڏهن_پورو
EXTERN صحيح	p_imcmdline;	// 'imcmdline'
EXTERN صحيح	p_imdisable;	// 'imdisable'
EXTERN ڊگهو	p_iminsert;	// 'iminsert'
EXTERN ڊگهو	p_imsearch;	// 'imsearch'
EXTERN صحيح	p_inf;		// 'infercase'
#إذا defined(FEAT_ڳوليو_ID) && defined(FEAT_EVAL)
EXTERN char_u	*p_inex;	// 'includeexpr'
#جيڪڏهن_پورو
EXTERN صحيح	p_is;		// 'incsearch'
#إذا defined(FEAT_EVAL)
EXTERN char_u	*p_inde;	// 'indentexpr'
EXTERN char_u	*p_indk;	// 'indentkeys'
#جيڪڏهن_پورو
EXTERN صحيح	p_im;		// 'insertmode'
EXTERN char_u	*p_isf;		// 'isfname'
EXTERN char_u	*p_isi;		// 'isident'
EXTERN char_u	*p_isk;		// 'iskeyword'
EXTERN char_u	*p_isp;		// 'isprint'
EXTERN صحيح	p_js;		// 'joinspaces'
#جيڪڏهن_وصف_ٿيل FEAT_CRYPT
EXTERN char_u	*p_ڪنجي;		// 'key'
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_KEYMAP
EXTERN char_u	*p_keymap;	// 'keymap'
#جيڪڏهن_پورو
EXTERN char_u	*p_kp;		// 'keywordprg'
EXTERN char_u	*p_km;		// 'keymodel'
EXTERN char_u	*p_kpc;		// 'keyprotocol'
#جيڪڏهن_وصف_ٿيل FEAT_LANGMAP
EXTERN char_u	*p_langmap;	// 'langmap'
EXTERN صحيح	p_lnr;		// 'langnoremap'
EXTERN صحيح	p_lrm;		// 'langremap'
#جيڪڏهن_پورو
#إذا defined(FEAT_MENU) && defined(FEAT_MULTI_LANG)
EXTERN char_u	*p_lm;		// 'langmenu'
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_GUI
EXTERN ڊگهو	p_linespace;	// 'linespace'
#جيڪڏهن_پورو
EXTERN صحيح	p_lisp;		// 'lisp'
EXTERN char_u	*p_lop;		// 'lispoptions'
EXTERN char_u	*p_lispwords;	// 'lispwords'
EXTERN ڊگهو	p_ls;		// 'laststatus'
EXTERN ڊگهو	p_stal;		// 'showtabline'
EXTERN char_u	*p_lcs;		// 'listchars'

EXTERN صحيح	p_lz;		// 'lazyredraw'
EXTERN صحيح	p_lpl;		// 'loadplugins'
#إذا defined(DYNAMIC_LUA)
EXTERN char_u	*p_luadll;	// 'luadll'
#جيڪڏهن_پورو
EXTERN صحيح	p_magic;	// 'magic'
EXTERN char_u	*p_menc;	// 'makeencoding'
#جيڪڏهن_وصف_ٿيل FEAT_QUICKFIX
EXTERN char_u	*p_mef;		// 'makeef'
EXTERN char_u	*p_mp;		// 'makeprg'
#جيڪڏهن_پورو
EXTERN char_u	*p_mps;		// 'matchpairs'
EXTERN ڊگهو	p_mat;		// 'matchtime'
EXTERN ڊگهو	p_mco;		// 'maxcombine'
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
EXTERN ڊگهو	p_mfd;		// 'maxfuncdepth'
#جيڪڏهن_پورو
EXTERN ڊگهو	p_mmd;		// 'maxmapdepth'
EXTERN ڊگهو	p_mm;		// 'maxmem'
EXTERN ڊگهو	p_mmp;		// 'maxmempattern'
EXTERN ڊگهو	p_mmt;		// 'maxmemtot'
#جيڪڏهن_وصف_ٿيل FEAT_MENU
EXTERN ڊگهو	p_mis;		// 'menuitems'
#جيڪڏهن_پورو
EXTERN char_u	*p_mopt;	// 'messagesopt'
EXTERN ڊگهو	p_msc;		// 'maxsearchcount'
#جيڪڏهن_وصف_ٿيل FEAT_SPELL
EXTERN char_u	*p_msm;		// 'mkspellmem'
#جيڪڏهن_پورو
EXTERN صحيح	p_ml;		// 'modeline'
EXTERN صحيح	p_mle;		// 'modelineexpr'
EXTERN ڊگهو	p_mls;		// 'modelines'
EXTERN صحيح	p_mlstr;	// 'modelinestrict'
EXTERN صحيح	p_ma;		// 'modifiable'
EXTERN صحيح	p_mod;		// 'modified'
EXTERN char_u	*p_mouse;	// 'mouse'
#جيڪڏهن_وصف_ٿيل FEAT_GUI
EXTERN صحيح	p_mousef;	// 'mousefocus'
EXTERN صحيح	p_mh;		// 'mousehide'
#جيڪڏهن_پورو
EXTERN char_u	*p_mousem;	// 'mousemodel'
#جيڪڏهن_وصف_ٿيل FEAT_GUI
EXTERN صحيح	p_mousemev;	// 'mousemoveevent'
#جيڪڏهن_پورو
EXTERN ڊگهو	p_mouset;	// 'mousetime'
EXTERN صحيح	p_وڌيڪ;		// 'more'
#جيڪڏهن_وصف_ٿيل FEAT_MZSCHEME
EXTERN ڊگهو	p_mzq;		// 'mzquantum
# إذا defined(DYNAMIC_MZSCHEME)
EXTERN char_u	*p_mzschemedll;	// 'mzschemedll'
EXTERN char_u	*p_mzschemegcdll; // 'mzschemegcdll'
# endif
#جيڪڏهن_پورو
EXTERN char_u	*p_nf;		// 'nrformats'
#إذا defined(MSWIN)
EXTERN صحيح	p_odev;		// 'opendevice'
#جيڪڏهن_پورو
EXTERN ڊگهو	p_ost;		// 'osctimeoutlen'
EXTERN char_u	*p_opfunc;	// 'operatorfunc'
EXTERN char_u	*p_para;	// 'paragraphs'
EXTERN صحيح	p_paste;	// 'paste'
EXTERN char_u	*p_pt;		// 'pastetoggle'
#إذا defined(FEAT_EVAL) && defined(FEAT_DIFF)
EXTERN char_u	*p_pex;		// 'patchexpr'
#جيڪڏهن_پورو
EXTERN char_u	*p_pm;		// 'patchmode'
EXTERN char_u	*p_رستو;	// 'path'
EXTERN char_u	*p_cdpath;	// 'cdpath'
#إذا defined(DYNAMIC_PERL)
EXTERN char_u	*p_perldll;	// 'perldll'
#جيڪڏهن_پورو
EXTERN صحيح	p_pi;		// 'preserveindent'
#إذا defined(DYNAMIC_PYTHON3)
EXTERN char_u	*p_py3dll;	// 'pythonthreedll'
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_PYTHON3
EXTERN char_u	*p_py3home;	// 'pythonthreehome'
#جيڪڏهن_پورو
#إذا defined(DYNAMIC_PYTHON)
EXTERN char_u	*p_pydll;	// 'pythondll'
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_PYTHON
EXTERN char_u	*p_pyhome;	// 'pythonhome'
#جيڪڏهن_پورو
#إذا defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
EXTERN ڊگهو	p_pyx;		// 'pyxversion'
#جيڪڏهن_پورو
EXTERN char_u	*p_qe;		// 'quoteescape'
EXTERN صحيح	p_ro;		// 'readonly'
#جيڪڏهن_وصف_ٿيل FEAT_RELTIME
EXTERN ڊگهو	p_rdt;		// 'redrawtime'
#جيڪڏهن_پورو
EXTERN صحيح	p_remap;	// 'remap'
EXTERN ڊگهو	p_re;		// 'regexpengine'
#جيڪڏهن_وصف_ٿيل FEAT_RENDER_OPTIONS
EXTERN char_u	*p_rop;		// 'renderoptions'
#جيڪڏهن_پورو
EXTERN ڊگهو	p_report;	// 'report'
#إذا defined(FEAT_QUICKFIX)
EXTERN ڊگهو	p_pvh;		// 'previewheight'
EXTERN ڊگهو     p_chi;          // 'chistory'
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل MSWIN
EXTERN صحيح	p_rs;		// 'restorescreen'
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_RIGHTLEFT
EXTERN صحيح	p_ari;		// 'allowrevins'
EXTERN صحيح	p_ri;		// 'revins'
#جيڪڏهن_پورو
#إذا defined(DYNAMIC_RUBY)
EXTERN char_u	*p_rubydll;	// 'rubydll'
#جيڪڏهن_پورو
EXTERN صحيح	p_ru;		// 'ruler'
#جيڪڏهن_وصف_ٿيل FEAT_STL_OPT
EXTERN char_u	*p_ruf;		// 'rulerformat'
#جيڪڏهن_پورو
EXTERN char_u	*p_pp;		// 'packpath'
#جيڪڏهن_وصف_ٿيل FEAT_QUICKFIX
EXTERN char_u	*p_qftf;	// 'quickfixtextfunc'
#جيڪڏهن_پورو
EXTERN char_u	*p_rtp;		// 'runtimepath'
EXTERN ڊگهو	p_sj;		// 'scrolljump'
#إذا defined(MSWIN) && defined(FEAT_GUI)
EXTERN صحيح	p_scf;		// 'scrollfocus'
#جيڪڏهن_پورو
EXTERN ڊگهو	p_so;		// 'scrolloff'
EXTERN ڊگهو	p_sop;		// 'scrolloffpad'
EXTERN char_u	*p_sbo;		// 'scrollopt'
EXTERN char_u	*p_sections;	// 'sections'
EXTERN صحيح	p_secure;	// 'secure'
EXTERN char_u	*p_sel;		// 'selection'
EXTERN char_u	*p_slm;		// 'selectmode'
#جيڪڏهن_وصف_ٿيل FEAT_SESSION
EXTERN char_u	*p_ssop;	// 'sessionoptions'
EXTERN بنانشان	ssop_flags;
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
#جيڪڏهن_پورو
EXTERN char_u	*p_sh;		// 'shell'
EXTERN char_u	*p_shcf;	// 'shellcmdflag'
#جيڪڏهن_وصف_ٿيل FEAT_QUICKFIX
EXTERN char_u	*p_sp;		// 'shellpipe'
#جيڪڏهن_پورو
EXTERN char_u	*p_shq;		// 'shellquote'
EXTERN char_u	*p_sxq;		// 'shellxquote'
EXTERN char_u	*p_sxe;		// 'shellxescape'
EXTERN char_u	*p_srr;		// 'shellredir'
#جيڪڏهن_وصف_ٿيل AMIGA
EXTERN ڊگهو	p_st;		// 'shelltype'
#جيڪڏهن_پورو
EXTERN صحيح	p_stmp;		// 'shelltemp'
#جيڪڏهن_وصف_ٿيل BACKSLASH_اندريون_FILENAME
EXTERN صحيح	p_ssl;		// 'shellslash'
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_STL_OPT
EXTERN char_u	*p_stl;		// 'statusline'
EXTERN char_u	*p_stlo;	// 'statuslineopt'
#جيڪڏهن_پورو
EXTERN صحيح	p_sr;		// 'shiftround'
EXTERN ڊگهو	p_sw;		// 'shiftwidth'
EXTERN char_u	*p_shm;		// 'shortmess'
EXTERN صحيح	p_sn;		// 'shortname'
#جيڪڏهن_وصف_ٿيل FEAT_LINEBREAK
EXTERN char_u	*p_sbr;		// 'showbreak'
#جيڪڏهن_پورو
EXTERN صحيح	p_sc;		// 'showcmd'
EXTERN char_u	*p_sloc;	// 'showcmdloc'
EXTERN صحيح	p_sft;		// 'showfulltag'
EXTERN صحيح	p_sm;		// 'showmatch'
EXTERN صحيح	p_smd;		// 'showmode'
EXTERN ڊگهو	p_ss;		// 'sidescroll'
EXTERN ڊگهو	p_siso;		// 'sidescrolloff'
EXTERN صحيح	p_scs;		// 'smartcase'
EXTERN صحيح	p_si;		// 'smartindent'
EXTERN صحيح	p_sta;		// 'smarttab'
EXTERN ڊگهو	p_sts;		// 'softtabstop'
EXTERN صحيح	p_sb;		// 'splitbelow'
EXTERN char_u	*p_sua;		// 'suffixesadd'
EXTERN صحيح	p_swf;		// 'swapfile'
#جيڪڏهن_وصف_ٿيل FEAT_SYN_HL
EXTERN ڊگهو	p_smc;		// 'synmaxcol'
#جيڪڏهن_پورو
EXTERN ڊگهو	p_tpm;		// 'tabpagemax'
#جيڪڏهن_وصف_ٿيل FEAT_STL_OPT
EXTERN char_u	*p_tal;		// 'tabline'
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
EXTERN char_u	*p_tfu;		// 'tagfunc'
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_SPELL
EXTERN char_u	*p_spc;		// 'spellcapcheck'
EXTERN char_u	*p_spf;		// 'spellfile'
EXTERN char_u	*p_spl;		// 'spelllang'
EXTERN char_u	*p_spo;		// 'spelloptions'
EXTERN char_u	*p_sps;		// 'spellsuggest'
#جيڪڏهن_پورو
EXTERN صحيح	p_spr;		// 'splitright'
EXTERN صحيح	p_sol;		// 'startofline'
EXTERN char_u	*p_su;		// 'suffixes'
EXTERN char_u	*p_sws;		// 'swapsync'
EXTERN char_u	*p_swb;		// 'switchbuf'
EXTERN بنانشان	swb_flags;
// Keep in sync with p_swb_values in optionstr.c
#وصف_ڪريو SWB_USEOPEN		0x001
#وصف_ڪريو SWB_USETAB		0x002
#وصف_ڪريو SWB_ورهايو		0x004
#وصف_ڪريو SWB_NEWTAB		0x008
#وصف_ڪريو SWB_VSPLIT		0x010
#وصف_ڪريو SWB_USELAST		0x020

EXTERN char_u	*p_spk;		// 'splitkeep'

#إذا defined(FEAT_TABPANEL)
EXTERN char_u	*p_tpl;		// 'tabpanel'
EXTERN ڊگهو	p_stpl;		// 'showtabpanel'
EXTERN char_u	*p_tplo;	// 'tabpanelopt'
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_SYN_HL
EXTERN char_u	*p_syn;		// 'syntax'
#جيڪڏهن_پورو
EXTERN char_u	*p_tcl;		// 'tabclose'
EXTERN بنانشان	tcl_flags;	// flags from 'tabclose'
#وصف_ڪريو TCL_کاٻو		0x001
#وصف_ڪريو TCL_USELAST		0x002
EXTERN ڊگهو	p_ts;		// 'tabstop'
EXTERN صحيح	p_tbs;		// 'tagbsearch'
EXTERN char_u	*p_tc;		// 'tagcase'
EXTERN بنانشان tc_flags;       // flags from 'tagcase'
#وصف_ڪريو TC_FOLLOWIC		0x01
#وصف_ڪريو TC_IGNORE		0x02
#وصف_ڪريو TC_ميلاپ		0x04
#وصف_ڪريو TC_FOLLOWSCS		0x08
#وصف_ڪريو TC_SMART		0x10
EXTERN ڊگهو	p_tl;		// 'taglength'
EXTERN صحيح	p_tr;		// 'tagrelative'
EXTERN char_u	*p_ٽيگ_فهرست;	// 'tags'
EXTERN صحيح	p_tagsecure;    // 'tagsecure'
EXTERN صحيح	p_tgst;		// 'tagstack'
#إذا defined(DYNAMIC_TCL)
EXTERN char_u	*p_tcldll;	// 'tcldll'
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_ARABIC
EXTERN صحيح	p_tbidi;	// 'termbidi'
#جيڪڏهن_پورو
EXTERN char_u	*p_tenc;	// 'termencoding'
#جيڪڏهن_وصف_ٿيل FEAT_TERMGUICOLORS
EXTERN صحيح	p_tgc;		// 'termguicolors'
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل UNIX
EXTERN char_u	*p_trz;		// 'termresize'
#جيڪڏهن_پورو
EXTERN صحيح	p_tsy;		// 'termsync'
#جيڪڏهن_وصف_ٿيل FEAT_TERMINAL
EXTERN ڊگهو	p_twsl;		// 'termwinscroll'
#جيڪڏهن_پورو
#إذا defined(MSWIN) && defined(FEAT_TERMINAL)
EXTERN char_u	*p_twt;		// 'termwintype'
#جيڪڏهن_پورو
EXTERN صحيح	p_terse;	// 'terse'
EXTERN صحيح	p_ta;		// 'textauto'
EXTERN صحيح	p_tx;		// 'textmode'
EXTERN ڊگهو	p_tw;		// 'textwidth'
EXTERN صحيح	p_منزل;		// 'tildeop'
EXTERN صحيح	p_timeout;	// 'timeout'
EXTERN ڊگهو	p_tm;		// 'timeoutlen'
EXTERN صحيح	p_title;	// 'title'
EXTERN ڊگهو	p_titlelen;	// 'titlelen'
EXTERN char_u	*p_titleold;	// 'titleold'
EXTERN char_u	*p_titlestring;	// 'titlestring'
EXTERN char_u	*p_tsr;		// 'thesaurus'
EXTERN صحيح	p_ttimeout;	// 'ttimeout'
EXTERN ڊگهو	p_ttm;		// 'ttimeoutlen'
EXTERN صحيح	p_tbi;		// 'ttybuiltin'
EXTERN صحيح	p_tf;		// 'ttyfast'
#إذا defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
EXTERN char_u	*p_toolbar;	// 'toolbar'
EXTERN بنانشان toolbar_flags;
# define TOOLBAR_متن		0x01
# define TOOLBAR_ICONS		0x02
# define TOOLBAR_TOOLTIPS	0x04
# define TOOLBAR_HORIZ		0x08
#جيڪڏهن_پورو
#إذا defined(FEAT_TOOLBAR) && defined(FEAT_GUI_GTK)
EXTERN char_u	*p_tbis;	// 'toolbariconsize'
EXTERN بنانشان tbis_flags;
# define TBIS_TINY		0x01
# define TBIS_SMALL		0x02
# define TBIS_MEDIUM		0x04
# define TBIS_LARGE		0x08
# define TBIS_HUGE		0x10
# define TBIS_GIANT		0x20
#جيڪڏهن_پورو
EXTERN ڊگهو	p_ttyscroll;	// 'ttyscroll'
#إذا defined(UNIX) || defined(VMS)
EXTERN char_u	*p_ttym;	// 'ttymouse'
EXTERN بنانشان ttym_flags;
# define TTYM_XTERM		0x01
# define TTYM_XTERM2		0x02
# define TTYM_DEC		0x04
# define TTYM_NETTERM		0x08
# define TTYM_JSBTERM		0x10
# define TTYM_PTERM		0x20
# define TTYM_URXVT		0x40
# define TTYM_SGR		0x80
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_PERSISTENT_UNDO
EXTERN char_u	*p_udir;	// 'undodir'
EXTERN صحيح	p_udf;		// 'undofile'
#جيڪڏهن_پورو
EXTERN ڊگهو	p_ul;		// 'undolevels'
EXTERN ڊگهو	p_ur;		// 'undoreload'
EXTERN ڊگهو	p_uc;		// 'updatecount'
EXTERN ڊگهو	p_ut;		// 'updatetime'
#جيڪڏهن_وصف_ٿيل FEAT_VARTABS
EXTERN char_u	*p_vsts;	// 'varsofttabstop'
EXTERN char_u	*p_vts;		// 'vartabstop'
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_VIMINFO
EXTERN char_u	*p_viminfo;	// 'viminfo'
EXTERN char_u	*p_viminfofile;	// 'viminfofile'
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_SESSION
EXTERN char_u	*p_vdir;	// 'viewdir'
EXTERN char_u	*p_vop;		// 'viewoptions'
EXTERN بنانشان	vop_flags;	// uses SSOP_ flags
#جيڪڏهن_پورو
EXTERN صحيح	p_vb;		// 'visualbell'
EXTERN char_u	*p_ve;		// 'virtualedit'
EXTERN بنانشان ve_flags;
#وصف_ڪريو VE_بلاڪ	5	// includes "all"
#وصف_ڪريو VE_وجهو	6	// includes "all"
#وصف_ڪريو VE_ALL		4
#وصف_ڪريو VE_ONEMORE	8
#وصف_ڪريو VE_NONE		16	// "none"
#وصف_ڪريو VE_NONEU	32      // "NONE"
EXTERN ڊگهو	p_verbose;	// 'verbose'
#جيڪڏهن_وصف_ٿيل اندريون_OPTION_C
char_u	*p_vfile = (char_u *)""; // used before options are initialized
#وإلا
خارجي char_u	*p_vfile;	// 'verbosefile'
#جيڪڏهن_پورو
EXTERN صحيح	p_warn;		// 'warn'
EXTERN char_u	*p_wop;		// 'wildoptions'
EXTERN ڊگهو	p_دري;	// 'window'
#إذا defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_MOTIF) || defined(LINT) \
	|| defined (FEAT_GUI_GTK) || defined(FEAT_GUI_PHOTON)
# define FEAT_WAK
EXTERN char_u	*p_wak;		// 'winaltkeys'
#جيڪڏهن_پورو
EXTERN char_u	*p_wig;		// 'wildignore'
EXTERN صحيح	p_wiv;		// 'weirdinvert'
EXTERN char_u	*p_ww;		// 'whichwrap'
EXTERN ڊگهو	p_wc;		// 'wildchar'
EXTERN ڊگهو	p_wcm;		// 'wildcharm'
EXTERN صحيح	p_wic;		// 'wildignorecase'
EXTERN char_u	*p_wim;		// 'wildmode'
EXTERN صحيح	p_wmnu;		// 'wildmenu'
EXTERN ڊگهو	p_wh;		// 'winheight'
EXTERN ڊگهو	p_wmh;		// 'winminheight'
EXTERN ڊگهو	p_wmw;		// 'winminwidth'
EXTERN ڊگهو	p_wiw;		// 'winwidth'
#جيڪڏهن_وصف_ٿيل FEAT_WAYLAND
EXTERN char_u	*p_wse;		// 'wlseat'
EXTERN ڊگهو     p_wtm;		// 'wltimeoutlen'
#جيڪڏهن_پورو
#إذا defined(MSWIN) && defined(FEAT_TERMINAL)
EXTERN char_u	*p_winptydll;	// 'winptydll'
#جيڪڏهن_پورو
EXTERN ڊگهو	p_wm;		// 'wrapmargin'
EXTERN صحيح	p_ws;		// 'wrapscan'
EXTERN صحيح	p_write;	// 'write'
EXTERN صحيح	p_wa;		// 'writeany'
EXTERN صحيح	p_wb;		// 'writebackup'
EXTERN ڊگهو	p_wd;		// 'writedelay'
EXTERN صحيح	p_xtermcodes;	// 'xtermcodes'

/*
 * "indir" values for buffer-local options.
 * These need to be defined globally, so that the BV_COUNT can be used with
 * b_p_scriptID[].
 */
تعداد
{
    BV_AI = 0
    , BV_AC
    , BV_AR
    , BV_BH
    , BV_BKC
    , BV_BT
#جيڪڏهن_وصف_ٿيل FEAT_QUICKFIX
    , BV_EFM
    , BV_GEFM
    , BV_GP
    , BV_MP
#جيڪڏهن_پورو
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
#جيڪڏهن_وصف_ٿيل FEAT_DIFF
    , BV_DIA
#جيڪڏهن_پورو
    , BV_TSR
#جيڪڏهن_وصف_ٿيل BACKSLASH_اندريون_FILENAME
    , BV_CSL
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_COMPL_FUNC
    , BV_CFU
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_ڳوليو_ID
    , BV_DEF
    , BV_INC
#جيڪڏهن_پورو
    , BV_EOF
    , BV_EOL
    , BV_FIXEOL
    , BV_EP
    , BV_ET
    , BV_FENC
    , BV_FP
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
    , BV_BEXPR
    , BV_FEX
    , BV_FFU
#جيڪڏهن_پورو
    , BV_FF
    , BV_FLP
    , BV_FO
#جيڪڏهن_وصف_ٿيل HAVE_FSYNC
    , BV_FS
#جيڪڏهن_پورو
    , BV_FT
    , BV_IMI
    , BV_IMS
#إذا defined(FEAT_EVAL)
    , BV_INDE
    , BV_INDK
#جيڪڏهن_پورو
#إذا defined(FEAT_ڳوليو_ID) && defined(FEAT_EVAL)
    , BV_INEX
#جيڪڏهن_پورو
    , BV_INF
    , BV_ISE
    , BV_ISK
#جيڪڏهن_وصف_ٿيل FEAT_CRYPT
    , BV_ڪنجي
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_KEYMAP
    , BV_KMAP
#جيڪڏهن_پورو
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
#جيڪڏهن_وصف_ٿيل FEAT_COMPL_FUNC
    , BV_OFU
#جيڪڏهن_پورو
    , BV_رستو
    , BV_PI
    , BV_QE
    , BV_RO
    , BV_SI
    , BV_SN
#جيڪڏهن_وصف_ٿيل FEAT_SYN_HL
    , BV_SMC
    , BV_SYN
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_SPELL
    , BV_SPC
    , BV_SPF
    , BV_SPL
    , BV_SPO
#جيڪڏهن_پورو
    , BV_STS
    , BV_SUA
    , BV_SW
    , BV_SWF
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
    , BV_TFU
#جيڪڏهن_پورو
    , BV_ٽيگ_فهرست
    , BV_TC
#جيڪڏهن_وصف_ٿيل FEAT_COMPL_FUNC
    , BV_TSRFU
#جيڪڏهن_پورو
    , BV_TS
    , BV_TW
    , BV_TX
    , BV_UDF
    , BV_UL
    , BV_WM
#جيڪڏهن_وصف_ٿيل FEAT_TERMINAL
    , BV_TWSL
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_VARTABS
    , BV_VSTS
    , BV_VTS
#جيڪڏهن_پورو
    , BV_ڳڻپ	    // must be the last one
};

/*
 * "indir" values for window-local options.
 * These need to be defined globally, so that the WV_COUNT can be used in the
 * window structure.
 */
تعداد
{
    WV_LIST = 0
    , WV_LCS
    , WV_FCS
#جيڪڏهن_وصف_ٿيل FEAT_ARABIC
    , WV_ARAB
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_CONCEAL
    , WV_COCU
    , WV_COLE
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_TERMINAL
    , WV_TWK
    , WV_TWS
#جيڪڏهن_پورو
    , WV_CRBIND
#جيڪڏهن_وصف_ٿيل FEAT_LINEBREAK
    , WV_BRI
    , WV_BRIOPT
#جيڪڏهن_پورو
    , WV_WCR
#جيڪڏهن_وصف_ٿيل FEAT_DIFF
    , WV_DIFF
#جيڪڏهن_پورو
    , WV_EIW
#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
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
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_LINEBREAK
    , WV_LBR
#جيڪڏهن_پورو
    , WV_NU
    , WV_RNU
    , WV_VE
#جيڪڏهن_وصف_ٿيل FEAT_LINEBREAK
    , WV_NUW
#جيڪڏهن_پورو
#إذا defined(FEAT_QUICKFIX)
    , WV_PVW
    , WV_LHI
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_RIGHTLEFT
    , WV_RL
    , WV_RLC
#جيڪڏهن_پورو
    , WV_SCBIND
    , WV_SCROLL
    , WV_SMS
    , WV_SISO
    , WV_SO
    , WV_SOP
#جيڪڏهن_وصف_ٿيل FEAT_SPELL
    , WV_SPELL
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_SYN_HL
    , WV_CUC
    , WV_CUL
    , WV_CULOPT
    , WV_CC
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_LINEBREAK
    , WV_SBR
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_STL_OPT
    , WV_STL
    , WV_STLO
#جيڪڏهن_پورو
    , WV_WFB
    , WV_WFH
    , WV_WFW
    , WV_WHL
    , WV_WRAP
#جيڪڏهن_وصف_ٿيل FEAT_SIGNS
    , WV_SCL
#جيڪڏهن_پورو
    , WV_ڳڻپ	    // must be the last one
};

// Value for b_p_ul indicating the global value must be used.
#وصف_ڪريو NO_LOCAL_UNDOLEVEL (-123456)

#وصف_ڪريو ERR_BUFLEN 80

#جيڪڏهن_پورو // _OPTION_H_
