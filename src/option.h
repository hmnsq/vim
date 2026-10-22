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

#که_نه_تعریف_شوی _OPTION_H_
#تعریف_کړه _OPTION_H_

//
// Option Flags
//
#تعریف_کړه P_BOOL		0x01	// the option is boolean
#تعریف_کړه P_عدد		0x02	// the option is numeric
#تعریف_کړه P_STRING	0x04	// the option is a string
#تعریف_کړه P_ALLOCED	0x08	// the string option is in allocated memory,
				// must use free_string_option() when
				// assigning new value. Not set if default is
				// the same.
#تعریف_کړه P_EXPAND	0x10	// environment expansion.  NOTE: P_EXPAND can
				// never be used for local or hidden options!
#تعریف_کړه P_NO_CMD_EXPAND	0x20	// don't perform cmdline completions
#تعریف_کړه P_NODEFAULT	0x40	// don't set to default value
#تعریف_کړه P_DEF_ALLOCED	0x80	// default value is in allocated memory, must
				//  use vim_free() when assigning new value
#تعریف_کړه P_WAS_سېټ	0x100	// option has been set/reset
#تعریف_کړه P_NO_MKRC	0x200	// don't include in :mkvimrc output
#تعریف_کړه P_VI_DEF	0x400	// Use Vi default for Vim
#تعریف_کړه P_VIM		0x800	// Vim option, reset when 'cp' set

				// when option changed, what to display:
#تعریف_کړه P_RSTAT		0x1000	// redraw status lines
#تعریف_کړه P_RWIN		0x2000	// redraw current window and recompute text
#تعریف_کړه P_RBUF		0x4000	// redraw current buffer and recompute text
#تعریف_کړه P_RALL		0x6000	// redraw all windows and recompute text
#تعریف_کړه P_RCLR		0x7000	// clear and redraw all and recompute text

#تعریف_کړه P_COMMA		 0x8000	 // comma separated list
#تعریف_کړه P_ONECOMMA	0x18000L // P_COMMA and cannot have two consecutive
				 // commas
#تعریف_کړه P_NODUP		0x20000L // don't allow duplicate strings
#تعریف_کړه P_FLAGLIST	0x40000L // list of single-char flags

#تعریف_کړه P_SECURE	0x80000L // cannot change in modeline or secure mode
#تعریف_کړه P_GETTEXT      0x100000L // expand default value with _()
#تعریف_کړه P_NOGLOB       0x200000L // do not use local value for global vimrc
#تعریف_کړه P_NFNAME       0x400000L // only normal file name chars allowed
#تعریف_کړه P_INSECURE     0x800000L // option was set from a modeline
#تعریف_کړه P_PRI_MKRC    0x1000000L // priority for :mkvimrc (setting option has
				 // side effects)
#تعریف_کړه P_NO_ML       0x2000000L // not allowed in modeline
#تعریف_کړه P_CURSWANT    0x4000000L // update curswant required; not needed when
				 // there is a redraw flag
#تعریف_کړه P_NDNAME      0x8000000L // only normal dir name chars allowed
#تعریف_کړه P_HLONLY     0x10000000L // option only changes highlight, not text
#تعریف_کړه P_MLE	     0x20000000L // under control of 'modelineexpr'
#تعریف_کړه P_FUNC	     0x40000000L // accept a function reference or a lambda
#تعریف_کړه P_COLON	     0x80000000L // values use colons to create sublists
// Warning: Currently we have used all 32 bits for option flags. On some 32-bit
//          systems, the flags are stored as a 32-bit integer, and adding more
//          flags will overflow it. Adding another flag will need to change how
//          it's stored first.

// Returned by get_option_value().
تعريفنوع تعداد {
    gov_unknown,
    gov_bool,
    gov_شمېره,
    gov_string,
    gov_hidden_bool,
    gov_hidden_شمېره,
    gov_hidden_string
} getoption_T;

/*
 * Default values for 'errorformat'.
 * The "%f|%l| %m" one is used for when the contents of the quickfix window is
 * written to a file.
 */
#که_تعریف_شوی AMIGA
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
#د_که_پای

#تعریف_کړه DFLT_GREPFORMAT	"%f:%l:%m,%f:%l%m,%f  %l%m"

// default values for b_p_ff 'fileformat' and p_ffs 'fileformats'
#تعریف_کړه FF_DOS		"dos"
#تعریف_کړه FF_MAC		"mac"
#تعریف_کړه FF_UNIX		"unix"

#که_تعریف_شوی USE_CRNL
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
#د_که_پای


// Possible values for 'encoding'
#تعریف_کړه ENC_UCSBOM	"ucs-bom"	// check for BOM at start of file

// default value for 'encoding'
#إذا defined(MSWIN) || defined(__MVS__)
# define ENC_DFLT	"utf-8"
#وإلا
# define ENC_DFLT	"latin1"
#د_که_پای

// end-of-line style
#تعریف_کړه EOL_UNKNOWN	(-1)	// not defined yet
#تعریف_کړه EOL_UNIX	0	// NL
#تعریف_کړه EOL_DOS		1	// CR NL
#تعریف_کړه EOL_MAC		2	// CR

// Formatting options for p_fo 'formatoptions'
#تعریف_کړه FO_WRAP		't'
#تعریف_کړه FO_WRAP_COMS	'c'
#تعریف_کړه FO_RET_COMS	'r'
#تعریف_کړه FO_OPEN_COMS	'o'
#تعریف_کړه FO_NO_OPEN_COMS	'/'
#تعریف_کړه FO_Q_COMS	'q'
#تعریف_کړه FO_Q_شمېره	'n'
#تعریف_کړه FO_Q_دوهم	'2'
#تعریف_کړه FO_INS_VI	'v'
#تعریف_کړه FO_INS_LONG	'l'
#تعریف_کړه FO_INS_BLANK	'b'
#تعریف_کړه FO_MBYTE_BREAK	'm'	// break before/after multi-byte char
#تعریف_کړه FO_MBYTE_JOIN	'M'	// no space before/after multi-byte char
#تعریف_کړه FO_MBYTE_JOIN2	'B'	// no space between multi-byte chars
#تعریف_کړه FO_ONE_LETTER	'1'
#تعریف_کړه FO_WHITE_PAR	'w'	// trailing white space continues paragr.
#تعریف_کړه FO_AUTO		'a'	// automatic formatting
#تعریف_کړه FO_RIGOROUS_TW	']'     // respect textwidth rigorously
#تعریف_کړه FO_REMOVE_COMS	'j'	// remove comment leaders when joining lines
#تعریف_کړه FO_PERIOD_ABBR	'p'	// don't break a single space after a period

#تعریف_کړه DFLT_FO_VI	"vt"
#تعریف_کړه DFLT_FO_VIM	"tcq"
#تعریف_کړه FO_ALL		"tcro/q2vlb1mMBn,aw]jp"	// for do_set()

// characters for the p_cpo option:
#تعریف_کړه CPO_ALTREAD	'a'	// ":read" sets alternate file name
#تعریف_کړه CPO_ALTWRITE	'A'	// ":write" sets alternate file name
#تعریف_کړه CPO_BAR		'b'	// "\|" ends a mapping
#تعریف_کړه CPO_BSLASH	'B'	// backslash in mapping is not special
#تعریف_کړه CPO_پلټنه	'c'
#تعریف_کړه CPO_CONCAT	'C'	// Don't concatenate sourced lines
#تعریف_کړه CPO_DOTTAG	'd'	// "./tags" in 'tags' is in current dir
#تعریف_کړه CPO_DIGRAPH	'D'	// No digraph after "r", "f", etc.
#تعریف_کړه CPO_EXECBUF	'e'
#تعریف_کړه CPO_EMPTYREGION	'E'	// operating on empty region is an error
#تعریف_کړه CPO_FNAMER	'f'	// set file name for ":r file"
#تعریف_کړه CPO_FNAMEW	'F'	// set file name for ":w file"
#تعریف_کړه CPO_GOTO1	'g'	// goto line 1 for ":edit"
#تعریف_کړه CPO_INSEND	'H'	// "I" inserts before last blank in line
#تعریف_کړه CPO_INTMOD	'i'	// interrupt a read makes buffer modified
#تعریف_کړه CPO_INDENT	'I'	// remove auto-indent more often
#تعریف_کړه CPO_JOINSP	'j'	// only use two spaces for join after '.'
#تعریف_کړه CPO_ENDOFSENT	'J'	// need two spaces to detect end of sentence
#تعریف_کړه CPO_KEYCODE	'k'	// don't recognize raw key code in mappings
#تعریف_کړه CPO_KOFFSET	'K'	// don't wait for key code in mappings
#تعریف_کړه CPO_LITERAL	'l'	// take char after backslash in [] literal
#تعریف_کړه CPO_LISTWM	'L'	// 'list' changes wrapmargin
#تعریف_کړه CPO_SHOWMATCH	'm'
#تعریف_کړه CPO_MATCHBSL	'M'	// "%" ignores use of backslashes
#تعریف_کړه CPO_NUMCOL	'n'	// 'number' column also used for text
#تعریف_کړه CPO_LINEOFF	'o'
#تعریف_کړه CPO_OVERNEW	'O'	// silently overwrite new file
#تعریف_کړه CPO_LISP	'p'	// 'lisp' indenting
#تعریف_کړه CPO_FNAMEAPP	'P'	// set file name for ":w >>file"
#تعریف_کړه CPO_JOINCOL	'q'	// with "3J" use column after first join
#تعریف_کړه CPO_REDO	'r'
#تعریف_کړه CPO_REMMARK	'R'	// remove marks when filtering
#تعریف_کړه CPO_BUFOPT	's'
#تعریف_کړه CPO_BUFOPTGLOB	'S'
#تعریف_کړه CPO_TAGPAT	't'	// tag pattern is used for "n"
#تعریف_کړه CPO_UNDO	'u'	// "u" undoes itself
#تعریف_کړه CPO_BACKSPACE	'v'	// "v" keep deleted text
#تعریف_کړه CPO_CW		'w'	// "cw" only changes one blank
#تعریف_کړه CPO_FWRITE	'W'	// "w!" doesn't overwrite readonly files
#تعریف_کړه CPO_ESC		'x'
#تعریف_کړه CPO_REPLCNT	'X'	// "R" with a count only deletes chars once
#تعریف_کړه CPO_YANK	'y'
#تعریف_کړه CPO_KEEPRO	'Z'	// don't reset 'readonly' on ":w!"
#تعریف_کړه CPO_کلمه	'z'	// do not special-case word motions cw and dw
#تعریف_کړه CPO_DOLLAR	'$'
#تعریف_کړه CPO_FILTER	'!'
#تعریف_کړه CPO_سمون	'%'
#تعریف_کړه CPO_ستوری	'*'	// ":*" means ":@"
#تعریف_کړه CPO_PLUS	'+'	// ":write file" resets 'modified'
#تعریف_کړه CPO_MINUS	'-'	// "9-" fails at and before line 9
#تعریف_کړه CPO_SPECI	'<'	// don't recognize <> in mappings
#تعریف_کړه CPO_REGAPPEND	'>'	// insert NL when appending to a register
// POSIX flags
#تعریف_کړه CPO_HASH	'#'	// "D", "o" and "O" do not use a count
#تعریف_کړه CPO_PARA	'{'	// "{" is also a paragraph boundary
#تعریف_کړه CPO_TSIZE	'|'	// $LINES and $COLUMNS overrule term size
#تعریف_کړه CPO_PRESERVE	'&'	// keep swap file after :preserve
#تعریف_کړه CPO_SUBPERCENT	'/'	// % in :s string uses previous one
#تعریف_کړه CPO_BACKSL	'\\'	// \ is not special in []
#تعریف_کړه CPO_CHDIR	'.'	// don't chdir if buffer is modified
#تعریف_کړه CPO_SCOLON	';'	// using "," and ";" will skip over char if
				// cursor would not move
#تعریف_کړه CPO_NOSYMLINKS	'~'	// don't resolve symlinks when changing directory
// default values for Vim, Vi and POSIX
#تعریف_کړه CPO_VIM		"aABceFsz"
#تعریف_کړه CPO_VI		"aAbBcCdDeEfFgHiIjJkKlLmMnoOpPqrRsStuvwWxXyZz$!%*-+<>;"
#تعریف_کړه CPO_ALL		"aAbBcCdDeEfFgHiIjJkKlLmMnoOpPqrRsStuvwWxXyZz$!%*-+<>#{|&/\\.;~"

// characters for p_ww option:
#تعریف_کړه WW_ALL		"bshl<>[]~"

// characters for p_mouse option:
#تعریف_کړه MOUSE_نورمال	'n'		// use mouse in Normal mode
#تعریف_کړه MOUSE_VISUAL	'v'		// use mouse in Visual/Select mode
#تعریف_کړه MOUSE_ورننباسه	'i'		// use mouse in Insert mode
#تعریف_کړه MOUSE_COMMAND	'c'		// use mouse in Command-line mode
#تعریف_کړه MOUSE_HELP	'h'		// use mouse in help buffers
#تعریف_کړه MOUSE_RETURN	'r'		// use mouse for hit-return message
#تعریف_کړه MOUSE_A		"nvich"		// used for 'a' flag
#تعریف_کړه MOUSE_ALL	"anvichr"	// all possible characters
#تعریف_کړه MOUSE_NONE	' '		// don't use Visual selection
#تعریف_کړه MOUSE_NONEF	'x'		// forced modeless selection

#تعریف_کړه COCU_ALL	"nvic"		// flags for 'concealcursor'

// characters for p_shm option:
#تعریف_کړه SHM_RO		'r'		// readonly
#تعریف_کړه SHM_MOD		'm'		// modified
#تعریف_کړه SHM_FILE	'f'		// (file 1 of 2)
#تعریف_کړه SHM_وروستی	'i'		// last line incomplete
#تعریف_کړه SHM_متن	'x'		// tx instead of textmode
#تعریف_کړه SHM_کرښې	'l'		// "L" instead of "lines"
#تعریف_کړه SHM_NEW		'n'		// "[New]" instead of "[New file]"
#تعریف_کړه SHM_WRI		'w'		// "[w]" instead of "written"
#تعریف_کړه SHM_A		"rmfixlnw"	// represented by 'a' flag
#تعریف_کړه SHM_WRITE	'W'		// don't use "written" at all
#تعریف_کړه SHM_TRUNC	't'		// truncate file messages
#تعریف_کړه SHM_TRUNCALL	'T'		// truncate all messages
#تعریف_کړه SHM_OVER	'o'		// overwrite file messages
#تعریف_کړه SHM_OVERALL	'O'		// overwrite more messages
#تعریف_کړه SHM_پلټنه	's'		// no search hit bottom messages
#تعریف_کړه SHM_ATTENTION	'A'		// no ATTENTION messages
#تعریف_کړه SHM_INTRO	'I'		// intro messages
#تعریف_کړه SHM_COMPLETIONMENU  'c'		// completion menu messages
#تعریف_کړه SHM_COMPLETIONSCAN  'C'		// completion scanning messages
#تعریف_کړه SHM_RECORDING	'q'		// short recording message
#تعریف_کړه SHM_FILEINFO	'F'		// no file info messages
#تعریف_کړه SHM_SEARCHCOUNT  'S'		// no search stats: '[1/10]'
#تعریف_کړه SHM_UNDO	'u'		// undo messages
#تعریف_کړه SHM_POSIX       "AS"		// POSIX value
#تعریف_کړه SHM_ALL		"rmfixlnwaWtToOsAIcCqFSu" // all possible flags for 'shm'
#تعریف_کړه SHM_LEN		30		// max length of all flags together
					// plus a NUL character

// characters for p_go:
#تعریف_کړه GO_TERMINAL	'!'		// use terminal for system commands
#تعریف_کړه GO_ASEL		'a'		// autoselect
#تعریف_کړه GO_ASELML	'A'		// autoselect modeless selection
#تعریف_کړه GO_BOT		'b'		// use bottom scrollbar
#تعریف_کړه GO_CONDIALOG	'c'		// use console dialog
#تعریف_کړه GO_TITLEBAR	'C'		// use 'hl-TitleBar'
#تعریف_کړه GO_DARKTHEME	'd'		// use dark theme variant
#تعریف_کړه GO_TABLINE	'e'		// may show tabline
#تعریف_کړه GO_FORG		'f'		// start GUI in foreground
#تعریف_کړه GO_GREY		'g'		// use grey menu items
#تعریف_کړه GO_HORSCROLL	'h'		// flexible horizontal scrolling
#تعریف_کړه GO_ICON		'i'		// use Vim icon
#تعریف_کړه GO_کیڼ		'l'		// use left scrollbar
#تعریف_کړه GO_VLEFT	'L'		// left scrollbar with vert split
#تعریف_کړه GO_MENUS	'm'		// use menu bar
#تعریف_کړه GO_NOSYSMENU	'M'		// don't source system menu
#تعریف_کړه GO_اشاره_ګر	'p'		// pointer enter/leave callbacks
#تعریف_کړه GO_ASELPLUS	'P'		// autoselectPlus
#تعریف_کړه GO_ښی	'r'		// use right scrollbar
#تعریف_کړه GO_VRIGHT	'R'		// right scrollbar with vert split
#تعریف_کړه GO_FULLSCREEN	's'		// enter fullscreen
#تعریف_کړه GO_TEAROFF	't'		// add tear-off menu items
#تعریف_کړه GO_TOOLBAR	'T'		// add toolbar
#تعریف_کړه GO_FOOTER	'F'		// add footer
#تعریف_کړه GO_VERTICAL	'v'		// arrange dialog buttons vertically
#تعریف_کړه GO_KEEPWINSIZE	'k'		// keep GUI window size
// all possible flags for 'go'
#تعریف_کړه GO_ALL		"!aAbcCdefFghilLmMpPrRstTvk"

// flags for 'comments' option
#تعریف_کړه COM_NEST	'n'		// comments strings nest
#تعریف_کړه COM_BLANK	'b'		// needs blank after string
#تعریف_کړه COM_پیل	's'		// start of comment
#تعریف_کړه COM_منځ	'm'		// middle of comment
#تعریف_کړه COM_پای_ټکی		'e'		// end of comment
#تعریف_کړه COM_AUTO_پای_ټکی	'x'		// last char of end closes comment
#تعریف_کړه COM_لومړی	'f'		// first line comment only
#تعریف_کړه COM_کیڼ	'l'		// left adjusted
#تعریف_کړه COM_ښی	'r'		// right adjusted
#تعریف_کړه COM_NOBACK	'O'		// don't use for "O" command
#تعریف_کړه COM_ALL		"nbsmexflrO"	// all flags for 'comments' option
#تعریف_کړه COM_MAX_LEN	50		// maximum length of a part

// flags for 'statusline' option
#تعریف_کړه STL_FILEPATH	'f'		// path of file in buffer
#تعریف_کړه STL_FULLPATH	'F'		// full path of file in buffer
#تعریف_کړه STL_FILENAME	't'		// last part (tail) of file path
#تعریف_کړه STL_ستنه	'c'		// column og cursor
#تعریف_کړه STL_VIRTCOL	'v'		// virtual column
#تعریف_کړه STL_VIRTCOL_ALT	'V'		// - with 'if different' display
#تعریف_کړه STL_کرښه	'l'		// line number of cursor
#تعریف_کړه STL_NUMLINES	'L'		// number of lines in buffer
#تعریف_کړه STL_BUFNO	'n'		// current buffer number
#تعریف_کړه STL_KEYMAP	'k'		// 'keymap' when active
#تعریف_کړه STL_دننه_واټن	'o'		// offset of character under cursor
#تعریف_کړه STL_دننه_واټن_X	'O'		// - in hexadecimal
#تعریف_کړه STL_BYTEVAL	'b'		// byte value of character
#تعریف_کړه STL_BYTEVAL_X	'B'		// - in hexadecimal
#تعریف_کړه STL_ROFLAG	'r'		// readonly flag
#تعریف_کړه STL_ROFLAG_ALT	'R'		// - other display
#تعریف_کړه STL_HELPFLAG	'h'		// window is showing a help file
#تعریف_کړه STL_HELPFLAG_ALT 'H'		// - other display
#تعریف_کړه STL_FILETYPE	'y'		// 'filetype'
#تعریف_کړه STL_FILETYPE_ALT 'Y'		// - other display
#تعریف_کړه STL_PREVIEWFLAG	'w'		// window is showing the preview buf
#تعریف_کړه STL_PREVIEWFLAG_ALT 'W'		// - other display
#تعریف_کړه STL_MODIFIED	'm'		// modified flag
#تعریف_کړه STL_MODIFIED_ALT 'M'		// - other display
#تعریف_کړه STL_QUICKFIX	'q'		// quickfix window description
#تعریف_کړه STL_PERCENTAGE	'p'		// percentage through file
#تعریف_کړه STL_ALTPERCENT	'P'		// percentage as TOP BOT ALL or NN%
#تعریف_کړه STL_ARGLISTSTAT	'a'		// argument list status as (x of y)
#تعریف_کړه STL_PAGENUM	'N'		// page number (when printing)
#تعریف_کړه STL_SHOWCMD	'S'		// 'showcmd' buffer
#تعریف_کړه STL_VIM_بیان	'{'		// start of expression to substitute
#تعریف_کړه STL_SEPARATE	'='		// separation between alignment
					// sections
#تعریف_کړه STL_TRUNCMARK	'<'		// truncation mark if line is too long
#تعریف_کړه STL_USER_HL	'*'		// highlight from (User)1..9 or 0
#تعریف_کړه STL_HIGHLIGHT	'#'		// highlight name
#تعریف_کړه STL_LINEBREAK	'@'		// insert a line break
#تعریف_کړه STL_CLICKFUNC	'['		// click handler region
#تعریف_کړه STL_TABPAGENR	'T'		// tab page label nr
#تعریف_کړه STL_TABCLOSENR	'X'		// tab page close nr
#تعریف_کړه STL_ALL		((char_u *) "fFtcvVlLknoObBrRhHmYyWwMqpPaNS{#@[")

// flags used for parsed 'wildmode'
#تعریف_کړه WIM_ډک	0x01
#تعریف_کړه WIM_LONGEST	0x02
#تعریف_کړه WIM_LIST	0x04
#تعریف_کړه WIM_BUFLASTUSED	0x08
#تعریف_کړه WIM_NOSELECT	0x10
#تعریف_کړه WIM_NOINSERT	0x20

// flags for the 'wildoptions' option
// each defined char should be unique over all values.
#تعریف_کړه WOP_FUZZY	'z'
#تعریف_کړه WOP_TAGFILE	'g'
#تعریف_کړه WOP_PUM		'p'
#تعریف_کړه WOP_EXACTTEXT	'x'

// arguments for can_bs()
// each defined char should be unique over all values
// except for BS_START, that intentionally also matches BS_NOSTOP
// because BS_NOSTOP behaves exactly the same except it
// does not stop at the start of the insert point
#تعریف_کړه BS_INDENT	'i'	// "Indent"
#تعریف_کړه BS_EOL		'l'	// "eoL"
#تعریف_کړه BS_پیل	's'	// "Start"
#تعریف_کړه BS_NOSTOP	'p'	// "nostoP

// flags for the 'culopt' option
#تعریف_کړه CULOPT_کرښه	0x01	// Highlight complete line
#تعریف_کړه CULOPT_SCRLINE	0x02	// Highlight screen line
#تعریف_کړه CULOPT_NBR	0x04	// Highlight Number column

#تعریف_کړه LISPWORD_ارزښت	"defun,define,defmacro,set!,lambda,if,case,let,flet,let*,letrec,do,do*,define-syntax,let-syntax,letrec-syntax,destructuring-bind,defpackage,defparameter,defstruct,deftype,defvar,do-all-symbols,do-external-symbols,do-symbols,dolist,dotimes,ecase,etypecase,eval-when,labels,macrolet,multiple-value-bind,multiple-value-call,multiple-value-prog1,multiple-value-setq,prog1,progv,typecase,unless,unwind-protect,when,with-input-from-string,with-open-file,with-open-stream,with-output-to-string,with-package-iterator,define-condition,handler-bind,handler-case,restart-bind,restart-case,with-simple-restart,store-value,use-value,muffle-warning,abort,continue,with-slots,with-slots*,with-accessors,with-accessors*,defclass,defmethod,print-unreadable-object"

/*
 * The following are actual variables for the options
 */

#که_تعریف_شوی FEAT_RIGHTLEFT
EXTERN اوږد	p_aleph;	// 'aleph'
#د_که_پای
EXTERN char_u	*p_ambw;	// 'ambiwidth'
#که_تعریف_شوی FEAT_AUTOCHDIR
EXTERN صحيح	p_acd;		// 'autochdir'
#د_که_پای
#که_تعریف_شوی FEAT_AUTOSHELLDIR
EXTERN صحيح	p_asd;		// 'autoshelldir'
#د_که_پای
EXTERN صحيح	p_ai;		// 'autoindent'
EXTERN صحيح	p_bin;		// 'binary'
EXTERN صحيح	p_bomb;		// 'bomb'
EXTERN صحيح	p_bl;		// 'buflisted'
EXTERN صحيح	p_cin;		// 'cindent'
EXTERN char_u	*p_cink;	// 'cinkeys'
EXTERN char_u	*p_cinsd;	// 'cinscopedecls'
EXTERN char_u	*p_cinw;	// 'cinwords'
#که_تعریف_شوی FEAT_COMPL_FUNC
EXTERN char_u	*p_cfu;		// 'completefunc'
EXTERN char_u	*p_ofu;		// 'omnifunc'
EXTERN char_u	*p_tsrfu;	// 'thesaurusfunc'
#د_که_پای
EXTERN صحيح	p_ci;		// 'copyindent'
#إذا defined(FEAT_GUI) && defined(MACOS_X)
EXTERN صحيح	*p_antialias;	// 'antialias'
#د_که_پای
EXTERN صحيح	p_ar;		// 'autoread'
EXTERN صحيح	p_aw;		// 'autowrite'
EXTERN صحيح	p_awa;		// 'autowriteall'
EXTERN char_u	*p_bs;		// 'backspace'
EXTERN char_u	*p_bg;		// 'background'
EXTERN صحيح	p_bk;		// 'backup'
EXTERN char_u	*p_bkc;		// 'backupcopy'
EXTERN بینښه	bkc_flags;	// flags from 'backupcopy'
#تعریف_کړه BKC_YES		0x001
#تعریف_کړه BKC_AUTO		0x002
#تعریف_کړه BKC_NO			0x004
#تعریف_کړه BKC_BREAKSYMLINK	0x008
#تعریف_کړه BKC_BREAKHARDLINK	0x010
EXTERN char_u	*p_bdir;	// 'backupdir'
EXTERN char_u	*p_bex;		// 'backupext'
EXTERN char_u	*p_bo;		// 'belloff'
EXTERN بینښه	bo_flags;

// values for the 'belloff' option
#تعریف_کړه BO_ALL		0x0001
#تعریف_کړه BO_BS		0x0002
#تعریف_کړه BO_CRSR		0x0004
#تعریف_کړه BO_COMPL	0x0008
#تعریف_کړه BO_COPY		0x0010
#تعریف_کړه BO_CTRLG	0x0020
#تعریف_کړه BO_ERROR	0x0040
#تعریف_کړه BO_ESC		0x0080
#تعریف_کړه BO_EX		0x0100
#تعریف_کړه BO_HANGUL	0x0200
#تعریف_کړه BO_IM		0x0400
#تعریف_کړه BO_LANG		0x0800
#تعریف_کړه BO_MESS		0x1000
#تعریف_کړه BO_سمون	0x2000
#تعریف_کړه BO_OPER		0x4000
#تعریف_کړه BO_REG		0x8000
#تعریف_کړه BO_SH		0x10000
#تعریف_کړه BO_SPELL	0x20000
#تعریف_کړه BO_غړی		0x40000
#تعریف_کړه BO_WILD		0x80000

EXTERN char_u	*p_bsk;		// 'backupskip'
#که_تعریف_شوی FEAT_CRYPT
EXTERN char_u	*p_cm;		// 'cryptmethod'
#د_که_پای
#که_تعریف_شوی FEAT_BEVAL
# ifdef FEAT_BEVAL_GUI
EXTERN صحيح	p_beval;	// 'ballooneval'
# endif
EXTERN اوږد	p_bdlay;	// 'balloondelay'
# ifdef FEAT_EVAL
EXTERN char_u	*p_bexpr;
# endif
# ifdef FEAT_BEVAL_غړی
EXTERN صحيح	p_bevalterm;	// 'balloonevalterm'
# endif
#د_که_پای
#که_تعریف_شوی FEAT_BROWSE
EXTERN char_u	*p_bsdir;	// 'browsedir'
#د_که_پای
#که_تعریف_شوی FEAT_LINEBREAK
EXTERN char_u	*p_breakat;	// 'breakat'
#د_که_پای
EXTERN char_u	*p_bh;		// 'bufhidden'
EXTERN char_u	*p_bt;		// 'buftype'
EXTERN char_u	*p_cmp;		// 'casemap'
EXTERN بینښه	cmp_flags;
#تعریف_کړه CMP_دننی		0x001
#تعریف_کړه CMP_KEEPASCII		0x002
EXTERN char_u	*p_enc;		// 'encoding'
EXTERN صحيح	p_deco;		// 'delcombine'
#که_تعریف_شوی FEAT_EVAL
EXTERN char_u	*p_ccv;		// 'charconvert'
#د_که_پای
EXTERN صحيح	p_cdh;		// 'cdhome'
EXTERN char_u	*p_cino;	// 'cinoptions'
EXTERN char_u	*p_cedit;	// 'cedit'
EXTERN اوږد	p_cwh;		// 'cmdwinheight'
#که_تعریف_شوی HAVE_CLIPMETHOD
EXTERN char_u	*p_cb;		// 'clipboard'
EXTERN char_u	*p_cpm;		// 'clipmethod'
#د_که_پای
EXTERN اوږد	p_ch;		// 'cmdheight'
EXTERN char_u	*p_cms;		// 'commentstring'
EXTERN char_u	*p_cpt;		// 'complete'
EXTERN اوږد	p_cto;		// 'completetimeout'
#إذا defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
EXTERN صحيح	p_confirm;	// 'confirm'
#د_که_پای
EXTERN صحيح	p_cp;		// 'compatible'
EXTERN char_u	*p_cfc;		// 'completefuzzycollect'
EXTERN بینښه cfc_flags;	// flags from "completefuzzycollect"
EXTERN char_u	*p_cia;		// 'completeitemalign'
EXTERN بینښه cia_flags;	// order flags of 'completeitemalign'
EXTERN char_u	*p_cot;		// 'completeopt'
EXTERN بینښه	cot_flags;	// flags from 'completeopt'
EXTERN صحيح	p_ac;		// 'autocomplete'
EXTERN اوږد	p_act;		// 'autocompletetimeout'
EXTERN اوږد	p_acl;		// 'autocompletedelay'
// Keep in sync with p_cot_values in optionstr.c
#تعریف_کړه COT_MENU	0x001
#تعریف_کړه COT_MENUONE	0x002
#تعریف_کړه COT_ANY_MENU	0x003	// combination of menu flags
#تعریف_کړه COT_LONGEST	0x004	// FALSE: insert full match,
				// TRUE: insert longest prefix
#تعریف_کړه COT_PREVIEW	    0x008
#تعریف_کړه COT_POPUP	    0x010
#تعریف_کړه COT_POPUPHIDDEN	    0x020
#تعریف_کړه COT_ANY_PREVIEW	    0x038   // combination of preview flags
#تعریف_کړه COT_NOINSERT	    0x040   // FALSE: select & insert, TRUE: noinsert
#تعریف_کړه COT_NOSELECT	    0x080   // FALSE: select & insert, TRUE: noselect
#تعریف_کړه COT_FUZZY	    0x100   // TRUE: fuzzy match enabled
#تعریف_کړه COT_NOSORT	    0x200   // TRUE: fuzzy match without qsort score
#تعریف_کړه COT_PREINSERT	    0x400   // TRUE: preinsert
#تعریف_کړه COT_تر_ټولو_نږدې	    0x800   // TRUE: prioritize matches close to cursor

#تعریف_کړه CFC_KEYWORD         0x001
#تعریف_کړه CFC_FILES           0x002
#تعریف_کړه CFC_WHOLELINE       0x004

#که_تعریف_شوی BACKSLASH_دننه_FILENAME
EXTERN char_u	*p_csl;		// 'completeslash'
#د_که_پای
EXTERN اوږد	p_ph;		// 'pumheight'
EXTERN اوږد	p_pw;		// 'pumwidth'
EXTERN اوږد	p_pmw;		// 'pummaxwidth'
EXTERN char_u	*p_pb;		// 'pumborder'
EXTERN char_u	*p_pumopt;	// 'pumopt'
EXTERN char_u	*p_com;		// 'comments'
EXTERN char_u	*p_cpo;		// 'cpoptions'
#که_تعریف_شوی FEAT_CSCOPE
EXTERN char_u	*p_csprg;	// 'cscopeprg'
EXTERN صحيح	p_csre;		// 'cscoperelative'
# ifdef FEAT_QUICKFIX
EXTERN char_u	*p_csqf;	// 'cscopequickfix'
#  define	CSQF_CMDS   "sgdctefia"
#  define	CSQF_FLAGS  "+-0"
# endif
EXTERN صحيح	p_cst;		// 'cscopetag'
EXTERN اوږد	p_csto;		// 'cscopetagorder'
EXTERN اوږد	p_cspc;		// 'cscopepathcomp'
EXTERN صحيح	p_csverbose;	// 'cscopeverbose'
#د_که_پای
EXTERN char_u	*p_debug;	// 'debug'
#که_تعریف_شوی FEAT_پیدا_کړه_ID
EXTERN char_u	*p_def;		// 'define'
EXTERN char_u	*p_inc;
#د_که_پای
#که_تعریف_شوی FEAT_DIFF
EXTERN char_u	*p_dia;		// 'diffanchors'
EXTERN char_u	*p_dip;		// 'diffopt'
# ifdef FEAT_EVAL
EXTERN char_u	*p_dex;		// 'diffexpr'
# endif
#د_که_پای
EXTERN char_u	*p_dict;	// 'dictionary'
#که_تعریف_شوی FEAT_DIGRAPHS
EXTERN صحيح	p_dg;		// 'digraph'
#د_که_پای
EXTERN char_u	*p_dir;		// 'directory'
EXTERN char_u	*p_dy;		// 'display'
EXTERN بینښه	dy_flags;
#تعریف_کړه DY_LASTLINE		0x001
#تعریف_کړه DY_TRUNCATE		0x002
#تعریف_کړه DY_UHEX			0x004
EXTERN صحيح	p_ed;		// 'edcompatible'
EXTERN char_u	*p_ead;		// 'eadirection'
EXTERN char_u	*p_emoji;	// 'emoji'
EXTERN صحيح	p_ea;		// 'equalalways'
EXTERN char_u	*p_ep;		// 'equalprg'
EXTERN صحيح	p_eb;		// 'errorbells'
#که_تعریف_شوی FEAT_QUICKFIX
EXTERN char_u	*p_ef;		// 'errorfile'
EXTERN char_u	*p_efm;		// 'errorformat'
EXTERN char_u	*p_gefm;	// 'grepformat'
EXTERN char_u	*p_gp;		// 'grepprg'
#د_که_پای
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
#که_تعریف_شوی FEAT_EVAL
EXTERN char_u	*p_ffu;		// 'findfunc'
#د_که_پای
EXTERN صحيح	p_fixeol;	// 'fixendofline'
#که_تعریف_شوی FEAT_FOLDING
EXTERN char_u	*p_fcl;		// 'foldclose'
EXTERN اوږد	p_fdls;		// 'foldlevelstart'
EXTERN char_u	*p_fdo;		// 'foldopen'
EXTERN بینښه	fdo_flags;
# define FDO_ALL		0x001
# define FDO_بلاک		0x002
# define FDO_HOR		0x004
# define FDO_MARK		0x008
# define FDO_PERCENT		0x010
# define FDO_QUICKFIX		0x020
# define FDO_پلټنه		0x040
# define FDO_نښان		0x080
# define FDO_ورننباسه		0x100
# define FDO_UNDO		0x200
# define FDO_JUMP		0x400
#د_که_پای
#إذا defined(FEAT_EVAL)
EXTERN char_u	*p_fex;		// 'formatexpr'
#د_که_پای
EXTERN char_u	*p_flp;		// 'formatlistpat'
EXTERN char_u	*p_fo;		// 'formatoptions'
EXTERN char_u	*p_fp;		// 'formatprg'
#که_تعریف_شوی HAVE_FSYNC
EXTERN صحيح	p_fs;		// 'fsync'
#د_که_پای
EXTERN صحيح	p_gd;		// 'gdefault'
EXTERN char_u	*p_jop;		// 'jumpoptions'
EXTERN بینښه	jop_flags;	//
#تعریف_کړه JOP_ډېری		0x001
#که_تعریف_شوی FEAT_PROP_POPUP
# ifdef FEAT_QUICKFIX
EXTERN char_u	*p_cpp;		// 'completepopup'
# endif
EXTERN char_u	*p_pvp;		// 'previewpopup'
#د_که_پای
#که_تعریف_شوی FEAT_PRINTER
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
EXTERN char_u	*p_سرلیک;	// 'printheader'
#د_که_پای
EXTERN صحيح	p_prompt;	// 'prompt'
#که_تعریف_شوی FEAT_GUI
EXTERN char_u	*p_guifont;	// 'guifont'
# ifdef FEAT_XFONTSET
EXTERN char_u	*p_guifontset;	// 'guifontset'
# endif
EXTERN char_u	*p_guifontwide;	// 'guifontwide'
EXTERN صحيح	p_guipty;	// 'guipty'
#د_که_پای
#إذا defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
EXTERN char_u	*p_guiligatures;  // 'guiligatures'
#د_که_پای
#إذا (defined(FEAT_GUI_GTK) && !defined(USE_GTK4)) || defined(FEAT_GUI_X11)
EXTERN اوږد	p_ghr;		// 'guiheadroom'
#د_که_پای
#که_تعریف_شوی CURSOR_SHAPE
EXTERN char_u	*p_guicursor;	// 'guicursor'
#د_که_پای
#که_تعریف_شوی FEAT_MOUSESHAPE
EXTERN char_u	*p_mouseshape;	// 'mouseshape'
#د_که_پای
#إذا defined(FEAT_GUI)
EXTERN char_u	*p_go;		// 'guioptions'
#د_که_پای
#إذا defined(FEAT_GUI_TABLINE)
EXTERN char_u	*p_gtl;		// 'guitablabel'
EXTERN char_u	*p_gtt;		// 'guitabtooltip'
#د_که_پای
EXTERN char_u	*p_hf;		// 'helpfile'
EXTERN اوږد	p_hh;		// 'helpheight'
#که_تعریف_شوی FEAT_MULTI_LANG
EXTERN char_u	*p_hlg;		// 'helplang'
#د_که_پای
EXTERN صحيح	p_hid;		// 'hidden'
EXTERN char_u	*p_hl;		// 'highlight'
EXTERN صحيح	p_hls;		// 'hlsearch'
EXTERN اوږد	p_hi;		// 'history'
#که_تعریف_شوی FEAT_RIGHTLEFT
EXTERN صحيح	p_hkmap;	// 'hkmap'
EXTERN صحيح	p_hkmapp;	// 'hkmapp'
# ifdef FEAT_ARABIC
EXTERN صحيح	p_arshape;	// 'arabicshape'
# endif
#د_که_پای
EXTERN صحيح	p_icon;		// 'icon'
EXTERN char_u	*p_iconstring;	// 'iconstring'
EXTERN صحيح	p_ic;		// 'ignorecase'
#إذا defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
EXTERN char_u	*p_imak;	// 'imactivatekey'
# define IM_ON_THE_SPOT		0L
# define IM_OVER_THE_SPOT	1L
EXTERN اوږد	p_imst;		// 'imstyle'
#د_که_پای
#که_تعریف_شوی FEAT_IMAGE
EXTERN char_u	*p_ipc;		// 'imageprotocol'
#د_که_پای
#إذا defined(FEAT_EVAL)
EXTERN char_u	*p_imaf;	// 'imactivatefunc'
EXTERN char_u	*p_imsf;	// 'imstatusfunc'
#د_که_پای
EXTERN صحيح	p_imcmdline;	// 'imcmdline'
EXTERN صحيح	p_imdisable;	// 'imdisable'
EXTERN اوږد	p_iminsert;	// 'iminsert'
EXTERN اوږد	p_imsearch;	// 'imsearch'
EXTERN صحيح	p_inf;		// 'infercase'
#إذا defined(FEAT_پیدا_کړه_ID) && defined(FEAT_EVAL)
EXTERN char_u	*p_inex;	// 'includeexpr'
#د_که_پای
EXTERN صحيح	p_is;		// 'incsearch'
#إذا defined(FEAT_EVAL)
EXTERN char_u	*p_inde;	// 'indentexpr'
EXTERN char_u	*p_indk;	// 'indentkeys'
#د_که_پای
EXTERN صحيح	p_im;		// 'insertmode'
EXTERN char_u	*p_isf;		// 'isfname'
EXTERN char_u	*p_isi;		// 'isident'
EXTERN char_u	*p_isk;		// 'iskeyword'
EXTERN char_u	*p_isp;		// 'isprint'
EXTERN صحيح	p_js;		// 'joinspaces'
#که_تعریف_شوی FEAT_CRYPT
EXTERN char_u	*p_کلی;		// 'key'
#د_که_پای
#که_تعریف_شوی FEAT_KEYMAP
EXTERN char_u	*p_keymap;	// 'keymap'
#د_که_پای
EXTERN char_u	*p_kp;		// 'keywordprg'
EXTERN char_u	*p_km;		// 'keymodel'
EXTERN char_u	*p_kpc;		// 'keyprotocol'
#که_تعریف_شوی FEAT_LANGMAP
EXTERN char_u	*p_langmap;	// 'langmap'
EXTERN صحيح	p_lnr;		// 'langnoremap'
EXTERN صحيح	p_lrm;		// 'langremap'
#د_که_پای
#إذا defined(FEAT_MENU) && defined(FEAT_MULTI_LANG)
EXTERN char_u	*p_lm;		// 'langmenu'
#د_که_پای
#که_تعریف_شوی FEAT_GUI
EXTERN اوږد	p_linespace;	// 'linespace'
#د_که_پای
EXTERN صحيح	p_lisp;		// 'lisp'
EXTERN char_u	*p_lop;		// 'lispoptions'
EXTERN char_u	*p_lispwords;	// 'lispwords'
EXTERN اوږد	p_ls;		// 'laststatus'
EXTERN اوږد	p_stal;		// 'showtabline'
EXTERN char_u	*p_lcs;		// 'listchars'

EXTERN صحيح	p_lz;		// 'lazyredraw'
EXTERN صحيح	p_lpl;		// 'loadplugins'
#إذا defined(DYNAMIC_LUA)
EXTERN char_u	*p_luadll;	// 'luadll'
#د_که_پای
EXTERN صحيح	p_magic;	// 'magic'
EXTERN char_u	*p_menc;	// 'makeencoding'
#که_تعریف_شوی FEAT_QUICKFIX
EXTERN char_u	*p_mef;		// 'makeef'
EXTERN char_u	*p_mp;		// 'makeprg'
#د_که_پای
EXTERN char_u	*p_mps;		// 'matchpairs'
EXTERN اوږد	p_mat;		// 'matchtime'
EXTERN اوږد	p_mco;		// 'maxcombine'
#که_تعریف_شوی FEAT_EVAL
EXTERN اوږد	p_mfd;		// 'maxfuncdepth'
#د_که_پای
EXTERN اوږد	p_mmd;		// 'maxmapdepth'
EXTERN اوږد	p_mm;		// 'maxmem'
EXTERN اوږد	p_mmp;		// 'maxmempattern'
EXTERN اوږد	p_mmt;		// 'maxmemtot'
#که_تعریف_شوی FEAT_MENU
EXTERN اوږد	p_mis;		// 'menuitems'
#د_که_پای
EXTERN char_u	*p_mopt;	// 'messagesopt'
EXTERN اوږد	p_msc;		// 'maxsearchcount'
#که_تعریف_شوی FEAT_SPELL
EXTERN char_u	*p_msm;		// 'mkspellmem'
#د_که_پای
EXTERN صحيح	p_ml;		// 'modeline'
EXTERN صحيح	p_mle;		// 'modelineexpr'
EXTERN اوږد	p_mls;		// 'modelines'
EXTERN صحيح	p_mlstr;	// 'modelinestrict'
EXTERN صحيح	p_ma;		// 'modifiable'
EXTERN صحيح	p_mod;		// 'modified'
EXTERN char_u	*p_mouse;	// 'mouse'
#که_تعریف_شوی FEAT_GUI
EXTERN صحيح	p_mousef;	// 'mousefocus'
EXTERN صحيح	p_mh;		// 'mousehide'
#د_که_پای
EXTERN char_u	*p_mousem;	// 'mousemodel'
#که_تعریف_شوی FEAT_GUI
EXTERN صحيح	p_mousemev;	// 'mousemoveevent'
#د_که_پای
EXTERN اوږد	p_mouset;	// 'mousetime'
EXTERN صحيح	p_نور;		// 'more'
#که_تعریف_شوی FEAT_MZSCHEME
EXTERN اوږد	p_mzq;		// 'mzquantum
# إذا defined(DYNAMIC_MZSCHEME)
EXTERN char_u	*p_mzschemedll;	// 'mzschemedll'
EXTERN char_u	*p_mzschemegcdll; // 'mzschemegcdll'
# endif
#د_که_پای
EXTERN char_u	*p_nf;		// 'nrformats'
#إذا defined(MSWIN)
EXTERN صحيح	p_odev;		// 'opendevice'
#د_که_پای
EXTERN اوږد	p_ost;		// 'osctimeoutlen'
EXTERN char_u	*p_opfunc;	// 'operatorfunc'
EXTERN char_u	*p_para;	// 'paragraphs'
EXTERN صحيح	p_paste;	// 'paste'
EXTERN char_u	*p_pt;		// 'pastetoggle'
#إذا defined(FEAT_EVAL) && defined(FEAT_DIFF)
EXTERN char_u	*p_pex;		// 'patchexpr'
#د_که_پای
EXTERN char_u	*p_pm;		// 'patchmode'
EXTERN char_u	*p_لار;	// 'path'
EXTERN char_u	*p_cdpath;	// 'cdpath'
#إذا defined(DYNAMIC_PERL)
EXTERN char_u	*p_perldll;	// 'perldll'
#د_که_پای
EXTERN صحيح	p_pi;		// 'preserveindent'
#إذا defined(DYNAMIC_PYTHON3)
EXTERN char_u	*p_py3dll;	// 'pythonthreedll'
#د_که_پای
#که_تعریف_شوی FEAT_PYTHON3
EXTERN char_u	*p_py3home;	// 'pythonthreehome'
#د_که_پای
#إذا defined(DYNAMIC_PYTHON)
EXTERN char_u	*p_pydll;	// 'pythondll'
#د_که_پای
#که_تعریف_شوی FEAT_PYTHON
EXTERN char_u	*p_pyhome;	// 'pythonhome'
#د_که_پای
#إذا defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
EXTERN اوږد	p_pyx;		// 'pyxversion'
#د_که_پای
EXTERN char_u	*p_qe;		// 'quoteescape'
EXTERN صحيح	p_ro;		// 'readonly'
#که_تعریف_شوی FEAT_RELTIME
EXTERN اوږد	p_rdt;		// 'redrawtime'
#د_که_پای
EXTERN صحيح	p_remap;	// 'remap'
EXTERN اوږد	p_re;		// 'regexpengine'
#که_تعریف_شوی FEAT_RENDER_OPTIONS
EXTERN char_u	*p_rop;		// 'renderoptions'
#د_که_پای
EXTERN اوږد	p_report;	// 'report'
#إذا defined(FEAT_QUICKFIX)
EXTERN اوږد	p_pvh;		// 'previewheight'
EXTERN اوږد     p_chi;          // 'chistory'
#د_که_پای
#که_تعریف_شوی MSWIN
EXTERN صحيح	p_rs;		// 'restorescreen'
#د_که_پای
#که_تعریف_شوی FEAT_RIGHTLEFT
EXTERN صحيح	p_ari;		// 'allowrevins'
EXTERN صحيح	p_ri;		// 'revins'
#د_که_پای
#إذا defined(DYNAMIC_RUBY)
EXTERN char_u	*p_rubydll;	// 'rubydll'
#د_که_پای
EXTERN صحيح	p_ru;		// 'ruler'
#که_تعریف_شوی FEAT_STL_OPT
EXTERN char_u	*p_ruf;		// 'rulerformat'
#د_که_پای
EXTERN char_u	*p_pp;		// 'packpath'
#که_تعریف_شوی FEAT_QUICKFIX
EXTERN char_u	*p_qftf;	// 'quickfixtextfunc'
#د_که_پای
EXTERN char_u	*p_rtp;		// 'runtimepath'
EXTERN اوږد	p_sj;		// 'scrolljump'
#إذا defined(MSWIN) && defined(FEAT_GUI)
EXTERN صحيح	p_scf;		// 'scrollfocus'
#د_که_پای
EXTERN اوږد	p_so;		// 'scrolloff'
EXTERN اوږد	p_sop;		// 'scrolloffpad'
EXTERN char_u	*p_sbo;		// 'scrollopt'
EXTERN char_u	*p_sections;	// 'sections'
EXTERN صحيح	p_secure;	// 'secure'
EXTERN char_u	*p_sel;		// 'selection'
EXTERN char_u	*p_slm;		// 'selectmode'
#که_تعریف_شوی FEAT_SESSION
EXTERN char_u	*p_ssop;	// 'sessionoptions'
EXTERN بینښه	ssop_flags;
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
#د_که_پای
EXTERN char_u	*p_sh;		// 'shell'
EXTERN char_u	*p_shcf;	// 'shellcmdflag'
#که_تعریف_شوی FEAT_QUICKFIX
EXTERN char_u	*p_sp;		// 'shellpipe'
#د_که_پای
EXTERN char_u	*p_shq;		// 'shellquote'
EXTERN char_u	*p_sxq;		// 'shellxquote'
EXTERN char_u	*p_sxe;		// 'shellxescape'
EXTERN char_u	*p_srr;		// 'shellredir'
#که_تعریف_شوی AMIGA
EXTERN اوږد	p_st;		// 'shelltype'
#د_که_پای
EXTERN صحيح	p_stmp;		// 'shelltemp'
#که_تعریف_شوی BACKSLASH_دننه_FILENAME
EXTERN صحيح	p_ssl;		// 'shellslash'
#د_که_پای
#که_تعریف_شوی FEAT_STL_OPT
EXTERN char_u	*p_stl;		// 'statusline'
EXTERN char_u	*p_stlo;	// 'statuslineopt'
#د_که_پای
EXTERN صحيح	p_sr;		// 'shiftround'
EXTERN اوږد	p_sw;		// 'shiftwidth'
EXTERN char_u	*p_shm;		// 'shortmess'
EXTERN صحيح	p_sn;		// 'shortname'
#که_تعریف_شوی FEAT_LINEBREAK
EXTERN char_u	*p_sbr;		// 'showbreak'
#د_که_پای
EXTERN صحيح	p_sc;		// 'showcmd'
EXTERN char_u	*p_sloc;	// 'showcmdloc'
EXTERN صحيح	p_sft;		// 'showfulltag'
EXTERN صحيح	p_sm;		// 'showmatch'
EXTERN صحيح	p_smd;		// 'showmode'
EXTERN اوږد	p_ss;		// 'sidescroll'
EXTERN اوږد	p_siso;		// 'sidescrolloff'
EXTERN صحيح	p_scs;		// 'smartcase'
EXTERN صحيح	p_si;		// 'smartindent'
EXTERN صحيح	p_sta;		// 'smarttab'
EXTERN اوږد	p_sts;		// 'softtabstop'
EXTERN صحيح	p_sb;		// 'splitbelow'
EXTERN char_u	*p_sua;		// 'suffixesadd'
EXTERN صحيح	p_swf;		// 'swapfile'
#که_تعریف_شوی FEAT_SYN_HL
EXTERN اوږد	p_smc;		// 'synmaxcol'
#د_که_پای
EXTERN اوږد	p_tpm;		// 'tabpagemax'
#که_تعریف_شوی FEAT_STL_OPT
EXTERN char_u	*p_tal;		// 'tabline'
#د_که_پای
#که_تعریف_شوی FEAT_EVAL
EXTERN char_u	*p_tfu;		// 'tagfunc'
#د_که_پای
#که_تعریف_شوی FEAT_SPELL
EXTERN char_u	*p_spc;		// 'spellcapcheck'
EXTERN char_u	*p_spf;		// 'spellfile'
EXTERN char_u	*p_spl;		// 'spelllang'
EXTERN char_u	*p_spo;		// 'spelloptions'
EXTERN char_u	*p_sps;		// 'spellsuggest'
#د_که_پای
EXTERN صحيح	p_spr;		// 'splitright'
EXTERN صحيح	p_sol;		// 'startofline'
EXTERN char_u	*p_su;		// 'suffixes'
EXTERN char_u	*p_sws;		// 'swapsync'
EXTERN char_u	*p_swb;		// 'switchbuf'
EXTERN بینښه	swb_flags;
// Keep in sync with p_swb_values in optionstr.c
#تعریف_کړه SWB_USEOPEN		0x001
#تعریف_کړه SWB_USETAB		0x002
#تعریف_کړه SWB_وېشل		0x004
#تعریف_کړه SWB_NEWTAB		0x008
#تعریف_کړه SWB_VSPLIT		0x010
#تعریف_کړه SWB_USELAST		0x020

EXTERN char_u	*p_spk;		// 'splitkeep'

#إذا defined(FEAT_TABPANEL)
EXTERN char_u	*p_tpl;		// 'tabpanel'
EXTERN اوږد	p_stpl;		// 'showtabpanel'
EXTERN char_u	*p_tplo;	// 'tabpanelopt'
#د_که_پای

#که_تعریف_شوی FEAT_SYN_HL
EXTERN char_u	*p_syn;		// 'syntax'
#د_که_پای
EXTERN char_u	*p_tcl;		// 'tabclose'
EXTERN بینښه	tcl_flags;	// flags from 'tabclose'
#تعریف_کړه TCL_کیڼ		0x001
#تعریف_کړه TCL_USELAST		0x002
EXTERN اوږد	p_ts;		// 'tabstop'
EXTERN صحيح	p_tbs;		// 'tagbsearch'
EXTERN char_u	*p_tc;		// 'tagcase'
EXTERN بینښه tc_flags;       // flags from 'tagcase'
#تعریف_کړه TC_FOLLOWIC		0x01
#تعریف_کړه TC_IGNORE		0x02
#تعریف_کړه TC_سمون		0x04
#تعریف_کړه TC_FOLLOWSCS		0x08
#تعریف_کړه TC_SMART		0x10
EXTERN اوږد	p_tl;		// 'taglength'
EXTERN صحيح	p_tr;		// 'tagrelative'
EXTERN char_u	*p_نښانونه;	// 'tags'
EXTERN صحيح	p_tagsecure;    // 'tagsecure'
EXTERN صحيح	p_tgst;		// 'tagstack'
#إذا defined(DYNAMIC_TCL)
EXTERN char_u	*p_tcldll;	// 'tcldll'
#د_که_پای
#که_تعریف_شوی FEAT_ARABIC
EXTERN صحيح	p_tbidi;	// 'termbidi'
#د_که_پای
EXTERN char_u	*p_tenc;	// 'termencoding'
#که_تعریف_شوی FEAT_TERMGUICOLORS
EXTERN صحيح	p_tgc;		// 'termguicolors'
#د_که_پای
#که_تعریف_شوی UNIX
EXTERN char_u	*p_trz;		// 'termresize'
#د_که_پای
EXTERN صحيح	p_tsy;		// 'termsync'
#که_تعریف_شوی FEAT_TERMINAL
EXTERN اوږد	p_twsl;		// 'termwinscroll'
#د_که_پای
#إذا defined(MSWIN) && defined(FEAT_TERMINAL)
EXTERN char_u	*p_twt;		// 'termwintype'
#د_که_پای
EXTERN صحيح	p_terse;	// 'terse'
EXTERN صحيح	p_ta;		// 'textauto'
EXTERN صحيح	p_tx;		// 'textmode'
EXTERN اوږد	p_tw;		// 'textwidth'
EXTERN صحيح	p_منزل;		// 'tildeop'
EXTERN صحيح	p_timeout;	// 'timeout'
EXTERN اوږد	p_tm;		// 'timeoutlen'
EXTERN صحيح	p_title;	// 'title'
EXTERN اوږد	p_titlelen;	// 'titlelen'
EXTERN char_u	*p_titleold;	// 'titleold'
EXTERN char_u	*p_titlestring;	// 'titlestring'
EXTERN char_u	*p_tsr;		// 'thesaurus'
EXTERN صحيح	p_ttimeout;	// 'ttimeout'
EXTERN اوږد	p_ttm;		// 'ttimeoutlen'
EXTERN صحيح	p_tbi;		// 'ttybuiltin'
EXTERN صحيح	p_tf;		// 'ttyfast'
#إذا defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
EXTERN char_u	*p_toolbar;	// 'toolbar'
EXTERN بینښه toolbar_flags;
# define TOOLBAR_متن		0x01
# define TOOLBAR_ICONS		0x02
# define TOOLBAR_TOOLTIPS	0x04
# define TOOLBAR_HORIZ		0x08
#د_که_پای
#إذا defined(FEAT_TOOLBAR) && defined(FEAT_GUI_GTK)
EXTERN char_u	*p_tbis;	// 'toolbariconsize'
EXTERN بینښه tbis_flags;
# define TBIS_TINY		0x01
# define TBIS_SMALL		0x02
# define TBIS_MEDIUM		0x04
# define TBIS_LARGE		0x08
# define TBIS_HUGE		0x10
# define TBIS_GIANT		0x20
#د_که_پای
EXTERN اوږد	p_ttyscroll;	// 'ttyscroll'
#إذا defined(UNIX) || defined(VMS)
EXTERN char_u	*p_ttym;	// 'ttymouse'
EXTERN بینښه ttym_flags;
# define TTYM_XTERM		0x01
# define TTYM_XTERM2		0x02
# define TTYM_DEC		0x04
# define TTYM_NETTERM		0x08
# define TTYM_JSBTERM		0x10
# define TTYM_PTERM		0x20
# define TTYM_URXVT		0x40
# define TTYM_SGR		0x80
#د_که_پای
#که_تعریف_شوی FEAT_PERSISTENT_UNDO
EXTERN char_u	*p_udir;	// 'undodir'
EXTERN صحيح	p_udf;		// 'undofile'
#د_که_پای
EXTERN اوږد	p_ul;		// 'undolevels'
EXTERN اوږد	p_ur;		// 'undoreload'
EXTERN اوږد	p_uc;		// 'updatecount'
EXTERN اوږد	p_ut;		// 'updatetime'
#که_تعریف_شوی FEAT_VARTABS
EXTERN char_u	*p_vsts;	// 'varsofttabstop'
EXTERN char_u	*p_vts;		// 'vartabstop'
#د_که_پای
#که_تعریف_شوی FEAT_VIMINFO
EXTERN char_u	*p_viminfo;	// 'viminfo'
EXTERN char_u	*p_viminfofile;	// 'viminfofile'
#د_که_پای
#که_تعریف_شوی FEAT_SESSION
EXTERN char_u	*p_vdir;	// 'viewdir'
EXTERN char_u	*p_vop;		// 'viewoptions'
EXTERN بینښه	vop_flags;	// uses SSOP_ flags
#د_که_پای
EXTERN صحيح	p_vb;		// 'visualbell'
EXTERN char_u	*p_ve;		// 'virtualedit'
EXTERN بینښه ve_flags;
#تعریف_کړه VE_بلاک	5	// includes "all"
#تعریف_کړه VE_ورننباسه	6	// includes "all"
#تعریف_کړه VE_ALL		4
#تعریف_کړه VE_ONEMORE	8
#تعریف_کړه VE_NONE		16	// "none"
#تعریف_کړه VE_NONEU	32      // "NONE"
EXTERN اوږد	p_verbose;	// 'verbose'
#که_تعریف_شوی دننه_OPTION_C
char_u	*p_vfile = (char_u *)""; // used before options are initialized
#وإلا
خارجي char_u	*p_vfile;	// 'verbosefile'
#د_که_پای
EXTERN صحيح	p_warn;		// 'warn'
EXTERN char_u	*p_wop;		// 'wildoptions'
EXTERN اوږد	p_کړکۍ;	// 'window'
#إذا defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_MOTIF) || defined(LINT) \
	|| defined (FEAT_GUI_GTK) || defined(FEAT_GUI_PHOTON)
# define FEAT_WAK
EXTERN char_u	*p_wak;		// 'winaltkeys'
#د_که_پای
EXTERN char_u	*p_wig;		// 'wildignore'
EXTERN صحيح	p_wiv;		// 'weirdinvert'
EXTERN char_u	*p_ww;		// 'whichwrap'
EXTERN اوږد	p_wc;		// 'wildchar'
EXTERN اوږد	p_wcm;		// 'wildcharm'
EXTERN صحيح	p_wic;		// 'wildignorecase'
EXTERN char_u	*p_wim;		// 'wildmode'
EXTERN صحيح	p_wmnu;		// 'wildmenu'
EXTERN اوږد	p_wh;		// 'winheight'
EXTERN اوږد	p_wmh;		// 'winminheight'
EXTERN اوږد	p_wmw;		// 'winminwidth'
EXTERN اوږد	p_wiw;		// 'winwidth'
#که_تعریف_شوی FEAT_WAYLAND
EXTERN char_u	*p_wse;		// 'wlseat'
EXTERN اوږد     p_wtm;		// 'wltimeoutlen'
#د_که_پای
#إذا defined(MSWIN) && defined(FEAT_TERMINAL)
EXTERN char_u	*p_winptydll;	// 'winptydll'
#د_که_پای
EXTERN اوږد	p_wm;		// 'wrapmargin'
EXTERN صحيح	p_ws;		// 'wrapscan'
EXTERN صحيح	p_write;	// 'write'
EXTERN صحيح	p_wa;		// 'writeany'
EXTERN صحيح	p_wb;		// 'writebackup'
EXTERN اوږد	p_wd;		// 'writedelay'
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
#که_تعریف_شوی FEAT_QUICKFIX
    , BV_EFM
    , BV_GEFM
    , BV_GP
    , BV_MP
#د_که_پای
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
#که_تعریف_شوی FEAT_DIFF
    , BV_DIA
#د_که_پای
    , BV_TSR
#که_تعریف_شوی BACKSLASH_دننه_FILENAME
    , BV_CSL
#د_که_پای
#که_تعریف_شوی FEAT_COMPL_FUNC
    , BV_CFU
#د_که_پای
#که_تعریف_شوی FEAT_پیدا_کړه_ID
    , BV_DEF
    , BV_INC
#د_که_پای
    , BV_EOF
    , BV_EOL
    , BV_FIXEOL
    , BV_EP
    , BV_ET
    , BV_FENC
    , BV_FP
#که_تعریف_شوی FEAT_EVAL
    , BV_BEXPR
    , BV_FEX
    , BV_FFU
#د_که_پای
    , BV_FF
    , BV_FLP
    , BV_FO
#که_تعریف_شوی HAVE_FSYNC
    , BV_FS
#د_که_پای
    , BV_FT
    , BV_IMI
    , BV_IMS
#إذا defined(FEAT_EVAL)
    , BV_INDE
    , BV_INDK
#د_که_پای
#إذا defined(FEAT_پیدا_کړه_ID) && defined(FEAT_EVAL)
    , BV_INEX
#د_که_پای
    , BV_INF
    , BV_ISE
    , BV_ISK
#که_تعریف_شوی FEAT_CRYPT
    , BV_کلی
#د_که_پای
#که_تعریف_شوی FEAT_KEYMAP
    , BV_KMAP
#د_که_پای
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
#که_تعریف_شوی FEAT_COMPL_FUNC
    , BV_OFU
#د_که_پای
    , BV_لار
    , BV_PI
    , BV_QE
    , BV_RO
    , BV_SI
    , BV_SN
#که_تعریف_شوی FEAT_SYN_HL
    , BV_SMC
    , BV_SYN
#د_که_پای
#که_تعریف_شوی FEAT_SPELL
    , BV_SPC
    , BV_SPF
    , BV_SPL
    , BV_SPO
#د_که_پای
    , BV_STS
    , BV_SUA
    , BV_SW
    , BV_SWF
#که_تعریف_شوی FEAT_EVAL
    , BV_TFU
#د_که_پای
    , BV_نښانونه
    , BV_TC
#که_تعریف_شوی FEAT_COMPL_FUNC
    , BV_TSRFU
#د_که_پای
    , BV_TS
    , BV_TW
    , BV_TX
    , BV_UDF
    , BV_UL
    , BV_WM
#که_تعریف_شوی FEAT_TERMINAL
    , BV_TWSL
#د_که_پای
#که_تعریف_شوی FEAT_VARTABS
    , BV_VSTS
    , BV_VTS
#د_که_پای
    , BV_شمېر	    // must be the last one
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
#که_تعریف_شوی FEAT_ARABIC
    , WV_ARAB
#د_که_پای
#که_تعریف_شوی FEAT_CONCEAL
    , WV_COCU
    , WV_COLE
#د_که_پای
#که_تعریف_شوی FEAT_TERMINAL
    , WV_TWK
    , WV_TWS
#د_که_پای
    , WV_CRBIND
#که_تعریف_شوی FEAT_LINEBREAK
    , WV_BRI
    , WV_BRIOPT
#د_که_پای
    , WV_WCR
#که_تعریف_شوی FEAT_DIFF
    , WV_DIFF
#د_که_پای
    , WV_EIW
#که_تعریف_شوی FEAT_FOLDING
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
#د_که_پای
#که_تعریف_شوی FEAT_LINEBREAK
    , WV_LBR
#د_که_پای
    , WV_NU
    , WV_RNU
    , WV_VE
#که_تعریف_شوی FEAT_LINEBREAK
    , WV_NUW
#د_که_پای
#إذا defined(FEAT_QUICKFIX)
    , WV_PVW
    , WV_LHI
#د_که_پای
#که_تعریف_شوی FEAT_RIGHTLEFT
    , WV_RL
    , WV_RLC
#د_که_پای
    , WV_SCBIND
    , WV_SCROLL
    , WV_SMS
    , WV_SISO
    , WV_SO
    , WV_SOP
#که_تعریف_شوی FEAT_SPELL
    , WV_SPELL
#د_که_پای
#که_تعریف_شوی FEAT_SYN_HL
    , WV_CUC
    , WV_CUL
    , WV_CULOPT
    , WV_CC
#د_که_پای
#که_تعریف_شوی FEAT_LINEBREAK
    , WV_SBR
#د_که_پای
#که_تعریف_شوی FEAT_STL_OPT
    , WV_STL
    , WV_STLO
#د_که_پای
    , WV_WFB
    , WV_WFH
    , WV_WFW
    , WV_WHL
    , WV_WRAP
#که_تعریف_شوی FEAT_SIGNS
    , WV_SCL
#د_که_پای
    , WV_شمېر	    // must be the last one
};

// Value for b_p_ul indicating the global value must be used.
#تعریف_کړه NO_LOCAL_UNDOLEVEL (-123456)

#تعریف_کړه ERR_BUFLEN 80

#د_که_پای // _OPTION_H_
