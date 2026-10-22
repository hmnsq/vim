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

#اگر_تعریف_نشده _OPTION_H_
#تعریف_کن _OPTION_H_

//
// Option Flags
//
#تعریف_کن P_BOOL		0x01	// the option is boolean
#تعریف_کن P_رقم		0x02	// the option is numeric
#تعریف_کن P_STRING	0x04	// the option is a string
#تعریف_کن P_ALLOCED	0x08	// the string option is in allocated memory,
				// must use free_string_option() when
				// assigning new value. Not set if default is
				// the same.
#تعریف_کن P_EXPAND	0x10	// environment expansion.  NOTE: P_EXPAND can
				// never be used for local or hidden options!
#تعریف_کن P_NO_CMD_EXPAND	0x20	// don't perform cmdline completions
#تعریف_کن P_NODEFAULT	0x40	// don't set to default value
#تعریف_کن P_DEF_ALLOCED	0x80	// default value is in allocated memory, must
				//  use vim_free() when assigning new value
#تعریف_کن P_WAS_مجموعه	0x100	// option has been set/reset
#تعریف_کن P_NO_MKRC	0x200	// don't include in :mkvimrc output
#تعریف_کن P_VI_DEF	0x400	// Use Vi default for Vim
#تعریف_کن P_VIM		0x800	// Vim option, reset when 'cp' set

				// when option changed, what to display:
#تعریف_کن P_RSTAT		0x1000	// redraw status lines
#تعریف_کن P_RWIN		0x2000	// redraw current window and recompute text
#تعریف_کن P_RBUF		0x4000	// redraw current buffer and recompute text
#تعریف_کن P_RALL		0x6000	// redraw all windows and recompute text
#تعریف_کن P_RCLR		0x7000	// clear and redraw all and recompute text

#تعریف_کن P_COMMA		 0x8000	 // comma separated list
#تعریف_کن P_ONECOMMA	0x18000L // P_COMMA and cannot have two consecutive
				 // commas
#تعریف_کن P_NODUP		0x20000L // don't allow duplicate strings
#تعریف_کن P_FLAGLIST	0x40000L // list of single-char flags

#تعریف_کن P_SECURE	0x80000L // cannot change in modeline or secure mode
#تعریف_کن P_GETTEXT      0x100000L // expand default value with _()
#تعریف_کن P_NOGLOB       0x200000L // do not use local value for global vimrc
#تعریف_کن P_NFNAME       0x400000L // only normal file name chars allowed
#تعریف_کن P_INSECURE     0x800000L // option was set from a modeline
#تعریف_کن P_PRI_MKRC    0x1000000L // priority for :mkvimrc (setting option has
				 // side effects)
#تعریف_کن P_NO_ML       0x2000000L // not allowed in modeline
#تعریف_کن P_CURSWANT    0x4000000L // update curswant required; not needed when
				 // there is a redraw flag
#تعریف_کن P_NDNAME      0x8000000L // only normal dir name chars allowed
#تعریف_کن P_HLONLY     0x10000000L // option only changes highlight, not text
#تعریف_کن P_MLE	     0x20000000L // under control of 'modelineexpr'
#تعریف_کن P_FUNC	     0x40000000L // accept a function reference or a lambda
#تعریف_کن P_COLON	     0x80000000L // values use colons to create sublists
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
#اگر_تعریف_شده AMIGA
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
#پایان_اگر

#تعریف_کن DFLT_GREPFORMAT	"%f:%l:%m,%f:%l%m,%f  %l%m"

// default values for b_p_ff 'fileformat' and p_ffs 'fileformats'
#تعریف_کن FF_DOS		"dos"
#تعریف_کن FF_MAC		"mac"
#تعریف_کن FF_UNIX		"unix"

#اگر_تعریف_شده USE_CRNL
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
#پایان_اگر


// Possible values for 'encoding'
#تعریف_کن ENC_UCSBOM	"ucs-bom"	// check for BOM at start of file

// default value for 'encoding'
#إذا defined(MSWIN) || defined(__MVS__)
# define ENC_DFLT	"utf-8"
#وإلا
# define ENC_DFLT	"latin1"
#پایان_اگر

// end-of-line style
#تعریف_کن EOL_UNKNOWN	(-1)	// not defined yet
#تعریف_کن EOL_UNIX	0	// NL
#تعریف_کن EOL_DOS		1	// CR NL
#تعریف_کن EOL_MAC		2	// CR

// Formatting options for p_fo 'formatoptions'
#تعریف_کن FO_WRAP		't'
#تعریف_کن FO_WRAP_COMS	'c'
#تعریف_کن FO_RET_COMS	'r'
#تعریف_کن FO_OPEN_COMS	'o'
#تعریف_کن FO_NO_OPEN_COMS	'/'
#تعریف_کن FO_Q_COMS	'q'
#تعریف_کن FO_Q_عدد	'n'
#تعریف_کن FO_Q_دومی	'2'
#تعریف_کن FO_INS_VI	'v'
#تعریف_کن FO_INS_LONG	'l'
#تعریف_کن FO_INS_BLANK	'b'
#تعریف_کن FO_MBYTE_BREAK	'm'	// break before/after multi-byte char
#تعریف_کن FO_MBYTE_JOIN	'M'	// no space before/after multi-byte char
#تعریف_کن FO_MBYTE_JOIN2	'B'	// no space between multi-byte chars
#تعریف_کن FO_ONE_LETTER	'1'
#تعریف_کن FO_WHITE_PAR	'w'	// trailing white space continues paragr.
#تعریف_کن FO_AUTO		'a'	// automatic formatting
#تعریف_کن FO_RIGOROUS_TW	']'     // respect textwidth rigorously
#تعریف_کن FO_REMOVE_COMS	'j'	// remove comment leaders when joining lines
#تعریف_کن FO_PERIOD_ABBR	'p'	// don't break a single space after a period

#تعریف_کن DFLT_FO_VI	"vt"
#تعریف_کن DFLT_FO_VIM	"tcq"
#تعریف_کن FO_ALL		"tcro/q2vlb1mMBn,aw]jp"	// for do_set()

// characters for the p_cpo option:
#تعریف_کن CPO_ALTREAD	'a'	// ":read" sets alternate file name
#تعریف_کن CPO_ALTWRITE	'A'	// ":write" sets alternate file name
#تعریف_کن CPO_BAR		'b'	// "\|" ends a mapping
#تعریف_کن CPO_BSLASH	'B'	// backslash in mapping is not special
#تعریف_کن CPO_جستجو	'c'
#تعریف_کن CPO_CONCAT	'C'	// Don't concatenate sourced lines
#تعریف_کن CPO_DOTTAG	'd'	// "./tags" in 'tags' is in current dir
#تعریف_کن CPO_DIGRAPH	'D'	// No digraph after "r", "f", etc.
#تعریف_کن CPO_EXECBUF	'e'
#تعریف_کن CPO_EMPTYREGION	'E'	// operating on empty region is an error
#تعریف_کن CPO_FNAMER	'f'	// set file name for ":r file"
#تعریف_کن CPO_FNAMEW	'F'	// set file name for ":w file"
#تعریف_کن CPO_GOTO1	'g'	// goto line 1 for ":edit"
#تعریف_کن CPO_INSEND	'H'	// "I" inserts before last blank in line
#تعریف_کن CPO_INTMOD	'i'	// interrupt a read makes buffer modified
#تعریف_کن CPO_INDENT	'I'	// remove auto-indent more often
#تعریف_کن CPO_JOINSP	'j'	// only use two spaces for join after '.'
#تعریف_کن CPO_ENDOFSENT	'J'	// need two spaces to detect end of sentence
#تعریف_کن CPO_KEYCODE	'k'	// don't recognize raw key code in mappings
#تعریف_کن CPO_KOFFSET	'K'	// don't wait for key code in mappings
#تعریف_کن CPO_LITERAL	'l'	// take char after backslash in [] literal
#تعریف_کن CPO_LISTWM	'L'	// 'list' changes wrapmargin
#تعریف_کن CPO_SHOWMATCH	'm'
#تعریف_کن CPO_MATCHBSL	'M'	// "%" ignores use of backslashes
#تعریف_کن CPO_NUMCOL	'n'	// 'number' column also used for text
#تعریف_کن CPO_LINEOFF	'o'
#تعریف_کن CPO_OVERNEW	'O'	// silently overwrite new file
#تعریف_کن CPO_LISP	'p'	// 'lisp' indenting
#تعریف_کن CPO_FNAMEAPP	'P'	// set file name for ":w >>file"
#تعریف_کن CPO_JOINCOL	'q'	// with "3J" use column after first join
#تعریف_کن CPO_REDO	'r'
#تعریف_کن CPO_REMMARK	'R'	// remove marks when filtering
#تعریف_کن CPO_BUFOPT	's'
#تعریف_کن CPO_BUFOPTGLOB	'S'
#تعریف_کن CPO_TAGPAT	't'	// tag pattern is used for "n"
#تعریف_کن CPO_UNDO	'u'	// "u" undoes itself
#تعریف_کن CPO_BACKSPACE	'v'	// "v" keep deleted text
#تعریف_کن CPO_CW		'w'	// "cw" only changes one blank
#تعریف_کن CPO_FWRITE	'W'	// "w!" doesn't overwrite readonly files
#تعریف_کن CPO_ESC		'x'
#تعریف_کن CPO_REPLCNT	'X'	// "R" with a count only deletes chars once
#تعریف_کن CPO_YANK	'y'
#تعریف_کن CPO_KEEPRO	'Z'	// don't reset 'readonly' on ":w!"
#تعریف_کن CPO_کلمه	'z'	// do not special-case word motions cw and dw
#تعریف_کن CPO_DOLLAR	'$'
#تعریف_کن CPO_FILTER	'!'
#تعریف_کن CPO_تطابق	'%'
#تعریف_کن CPO_ستاره	'*'	// ":*" means ":@"
#تعریف_کن CPO_PLUS	'+'	// ":write file" resets 'modified'
#تعریف_کن CPO_MINUS	'-'	// "9-" fails at and before line 9
#تعریف_کن CPO_SPECI	'<'	// don't recognize <> in mappings
#تعریف_کن CPO_REGAPPEND	'>'	// insert NL when appending to a register
// POSIX flags
#تعریف_کن CPO_HASH	'#'	// "D", "o" and "O" do not use a count
#تعریف_کن CPO_PARA	'{'	// "{" is also a paragraph boundary
#تعریف_کن CPO_TSIZE	'|'	// $LINES and $COLUMNS overrule term size
#تعریف_کن CPO_PRESERVE	'&'	// keep swap file after :preserve
#تعریف_کن CPO_SUBPERCENT	'/'	// % in :s string uses previous one
#تعریف_کن CPO_BACKSL	'\\'	// \ is not special in []
#تعریف_کن CPO_CHDIR	'.'	// don't chdir if buffer is modified
#تعریف_کن CPO_SCOLON	';'	// using "," and ";" will skip over char if
				// cursor would not move
#تعریف_کن CPO_NOSYMLINKS	'~'	// don't resolve symlinks when changing directory
// default values for Vim, Vi and POSIX
#تعریف_کن CPO_VIM		"aABceFsz"
#تعریف_کن CPO_VI		"aAbBcCdDeEfFgHiIjJkKlLmMnoOpPqrRsStuvwWxXyZz$!%*-+<>;"
#تعریف_کن CPO_ALL		"aAbBcCdDeEfFgHiIjJkKlLmMnoOpPqrRsStuvwWxXyZz$!%*-+<>#{|&/\\.;~"

// characters for p_ww option:
#تعریف_کن WW_ALL		"bshl<>[]~"

// characters for p_mouse option:
#تعریف_کن MOUSE_نرمال	'n'		// use mouse in Normal mode
#تعریف_کن MOUSE_VISUAL	'v'		// use mouse in Visual/Select mode
#تعریف_کن MOUSE_درج	'i'		// use mouse in Insert mode
#تعریف_کن MOUSE_COMMAND	'c'		// use mouse in Command-line mode
#تعریف_کن MOUSE_HELP	'h'		// use mouse in help buffers
#تعریف_کن MOUSE_RETURN	'r'		// use mouse for hit-return message
#تعریف_کن MOUSE_A		"nvich"		// used for 'a' flag
#تعریف_کن MOUSE_ALL	"anvichr"	// all possible characters
#تعریف_کن MOUSE_NONE	' '		// don't use Visual selection
#تعریف_کن MOUSE_NONEF	'x'		// forced modeless selection

#تعریف_کن COCU_ALL	"nvic"		// flags for 'concealcursor'

// characters for p_shm option:
#تعریف_کن SHM_RO		'r'		// readonly
#تعریف_کن SHM_MOD		'm'		// modified
#تعریف_کن SHM_FILE	'f'		// (file 1 of 2)
#تعریف_کن SHM_آخرین	'i'		// last line incomplete
#تعریف_کن SHM_متن	'x'		// tx instead of textmode
#تعریف_کن SHM_سطرها	'l'		// "L" instead of "lines"
#تعریف_کن SHM_NEW		'n'		// "[New]" instead of "[New file]"
#تعریف_کن SHM_WRI		'w'		// "[w]" instead of "written"
#تعریف_کن SHM_A		"rmfixlnw"	// represented by 'a' flag
#تعریف_کن SHM_WRITE	'W'		// don't use "written" at all
#تعریف_کن SHM_TRUNC	't'		// truncate file messages
#تعریف_کن SHM_TRUNCALL	'T'		// truncate all messages
#تعریف_کن SHM_OVER	'o'		// overwrite file messages
#تعریف_کن SHM_OVERALL	'O'		// overwrite more messages
#تعریف_کن SHM_جستجو	's'		// no search hit bottom messages
#تعریف_کن SHM_ATTENTION	'A'		// no ATTENTION messages
#تعریف_کن SHM_INTRO	'I'		// intro messages
#تعریف_کن SHM_COMPLETIONMENU  'c'		// completion menu messages
#تعریف_کن SHM_COMPLETIONSCAN  'C'		// completion scanning messages
#تعریف_کن SHM_RECORDING	'q'		// short recording message
#تعریف_کن SHM_FILEINFO	'F'		// no file info messages
#تعریف_کن SHM_SEARCHCOUNT  'S'		// no search stats: '[1/10]'
#تعریف_کن SHM_UNDO	'u'		// undo messages
#تعریف_کن SHM_POSIX       "AS"		// POSIX value
#تعریف_کن SHM_ALL		"rmfixlnwaWtToOsAIcCqFSu" // all possible flags for 'shm'
#تعریف_کن SHM_LEN		30		// max length of all flags together
					// plus a NUL character

// characters for p_go:
#تعریف_کن GO_TERMINAL	'!'		// use terminal for system commands
#تعریف_کن GO_ASEL		'a'		// autoselect
#تعریف_کن GO_ASELML	'A'		// autoselect modeless selection
#تعریف_کن GO_BOT		'b'		// use bottom scrollbar
#تعریف_کن GO_CONDIALOG	'c'		// use console dialog
#تعریف_کن GO_TITLEBAR	'C'		// use 'hl-TitleBar'
#تعریف_کن GO_DARKTHEME	'd'		// use dark theme variant
#تعریف_کن GO_TABLINE	'e'		// may show tabline
#تعریف_کن GO_FORG		'f'		// start GUI in foreground
#تعریف_کن GO_GREY		'g'		// use grey menu items
#تعریف_کن GO_HORSCROLL	'h'		// flexible horizontal scrolling
#تعریف_کن GO_ICON		'i'		// use Vim icon
#تعریف_کن GO_چپ		'l'		// use left scrollbar
#تعریف_کن GO_VLEFT	'L'		// left scrollbar with vert split
#تعریف_کن GO_MENUS	'm'		// use menu bar
#تعریف_کن GO_NOSYSMENU	'M'		// don't source system menu
#تعریف_کن GO_اشاره_گر	'p'		// pointer enter/leave callbacks
#تعریف_کن GO_ASELPLUS	'P'		// autoselectPlus
#تعریف_کن GO_راست	'r'		// use right scrollbar
#تعریف_کن GO_VRIGHT	'R'		// right scrollbar with vert split
#تعریف_کن GO_FULLSCREEN	's'		// enter fullscreen
#تعریف_کن GO_TEAROFF	't'		// add tear-off menu items
#تعریف_کن GO_TOOLBAR	'T'		// add toolbar
#تعریف_کن GO_FOOTER	'F'		// add footer
#تعریف_کن GO_VERTICAL	'v'		// arrange dialog buttons vertically
#تعریف_کن GO_KEEPWINSIZE	'k'		// keep GUI window size
// all possible flags for 'go'
#تعریف_کن GO_ALL		"!aAbcCdefFghilLmMpPrRstTvk"

// flags for 'comments' option
#تعریف_کن COM_NEST	'n'		// comments strings nest
#تعریف_کن COM_BLANK	'b'		// needs blank after string
#تعریف_کن COM_شروع	's'		// start of comment
#تعریف_کن COM_میانه	'm'		// middle of comment
#تعریف_کن COM_انتها		'e'		// end of comment
#تعریف_کن COM_AUTO_انتها	'x'		// last char of end closes comment
#تعریف_کن COM_اولی	'f'		// first line comment only
#تعریف_کن COM_چپ	'l'		// left adjusted
#تعریف_کن COM_راست	'r'		// right adjusted
#تعریف_کن COM_NOBACK	'O'		// don't use for "O" command
#تعریف_کن COM_ALL		"nbsmexflrO"	// all flags for 'comments' option
#تعریف_کن COM_MAX_LEN	50		// maximum length of a part

// flags for 'statusline' option
#تعریف_کن STL_FILEPATH	'f'		// path of file in buffer
#تعریف_کن STL_FULLPATH	'F'		// full path of file in buffer
#تعریف_کن STL_FILENAME	't'		// last part (tail) of file path
#تعریف_کن STL_ستونی	'c'		// column og cursor
#تعریف_کن STL_VIRTCOL	'v'		// virtual column
#تعریف_کن STL_VIRTCOL_ALT	'V'		// - with 'if different' display
#تعریف_کن STL_سطر	'l'		// line number of cursor
#تعریف_کن STL_NUMLINES	'L'		// number of lines in buffer
#تعریف_کن STL_BUFNO	'n'		// current buffer number
#تعریف_کن STL_KEYMAP	'k'		// 'keymap' when active
#تعریف_کن STL_جابه_جایی	'o'		// offset of character under cursor
#تعریف_کن STL_جابه_جایی_X	'O'		// - in hexadecimal
#تعریف_کن STL_BYTEVAL	'b'		// byte value of character
#تعریف_کن STL_BYTEVAL_X	'B'		// - in hexadecimal
#تعریف_کن STL_ROFLAG	'r'		// readonly flag
#تعریف_کن STL_ROFLAG_ALT	'R'		// - other display
#تعریف_کن STL_HELPFLAG	'h'		// window is showing a help file
#تعریف_کن STL_HELPFLAG_ALT 'H'		// - other display
#تعریف_کن STL_FILETYPE	'y'		// 'filetype'
#تعریف_کن STL_FILETYPE_ALT 'Y'		// - other display
#تعریف_کن STL_PREVIEWFLAG	'w'		// window is showing the preview buf
#تعریف_کن STL_PREVIEWFLAG_ALT 'W'		// - other display
#تعریف_کن STL_MODIFIED	'm'		// modified flag
#تعریف_کن STL_MODIFIED_ALT 'M'		// - other display
#تعریف_کن STL_QUICKFIX	'q'		// quickfix window description
#تعریف_کن STL_PERCENTAGE	'p'		// percentage through file
#تعریف_کن STL_ALTPERCENT	'P'		// percentage as TOP BOT ALL or NN%
#تعریف_کن STL_ARGLISTSTAT	'a'		// argument list status as (x of y)
#تعریف_کن STL_PAGENUM	'N'		// page number (when printing)
#تعریف_کن STL_SHOWCMD	'S'		// 'showcmd' buffer
#تعریف_کن STL_VIM_عبارت	'{'		// start of expression to substitute
#تعریف_کن STL_SEPARATE	'='		// separation between alignment
					// sections
#تعریف_کن STL_TRUNCMARK	'<'		// truncation mark if line is too long
#تعریف_کن STL_USER_HL	'*'		// highlight from (User)1..9 or 0
#تعریف_کن STL_HIGHLIGHT	'#'		// highlight name
#تعریف_کن STL_LINEBREAK	'@'		// insert a line break
#تعریف_کن STL_CLICKFUNC	'['		// click handler region
#تعریف_کن STL_TABPAGENR	'T'		// tab page label nr
#تعریف_کن STL_TABCLOSENR	'X'		// tab page close nr
#تعریف_کن STL_ALL		((char_u *) "fFtcvVlLknoObBrRhHmYyWwMqpPaNS{#@[")

// flags used for parsed 'wildmode'
#تعریف_کن WIM_پر	0x01
#تعریف_کن WIM_LONGEST	0x02
#تعریف_کن WIM_LIST	0x04
#تعریف_کن WIM_BUFLASTUSED	0x08
#تعریف_کن WIM_NOSELECT	0x10
#تعریف_کن WIM_NOINSERT	0x20

// flags for the 'wildoptions' option
// each defined char should be unique over all values.
#تعریف_کن WOP_FUZZY	'z'
#تعریف_کن WOP_TAGFILE	'g'
#تعریف_کن WOP_PUM		'p'
#تعریف_کن WOP_EXACTTEXT	'x'

// arguments for can_bs()
// each defined char should be unique over all values
// except for BS_START, that intentionally also matches BS_NOSTOP
// because BS_NOSTOP behaves exactly the same except it
// does not stop at the start of the insert point
#تعریف_کن BS_INDENT	'i'	// "Indent"
#تعریف_کن BS_EOL		'l'	// "eoL"
#تعریف_کن BS_شروع	's'	// "Start"
#تعریف_کن BS_NOSTOP	'p'	// "nostoP

// flags for the 'culopt' option
#تعریف_کن CULOPT_سطر	0x01	// Highlight complete line
#تعریف_کن CULOPT_SCRLINE	0x02	// Highlight screen line
#تعریف_کن CULOPT_NBR	0x04	// Highlight Number column

#تعریف_کن LISPWORD_مقدار	"defun,define,defmacro,set!,lambda,if,case,let,flet,let*,letrec,do,do*,define-syntax,let-syntax,letrec-syntax,destructuring-bind,defpackage,defparameter,defstruct,deftype,defvar,do-all-symbols,do-external-symbols,do-symbols,dolist,dotimes,ecase,etypecase,eval-when,labels,macrolet,multiple-value-bind,multiple-value-call,multiple-value-prog1,multiple-value-setq,prog1,progv,typecase,unless,unwind-protect,when,with-input-from-string,with-open-file,with-open-stream,with-output-to-string,with-package-iterator,define-condition,handler-bind,handler-case,restart-bind,restart-case,with-simple-restart,store-value,use-value,muffle-warning,abort,continue,with-slots,with-slots*,with-accessors,with-accessors*,defclass,defmethod,print-unreadable-object"

/*
 * The following are actual variables for the options
 */

#اگر_تعریف_شده FEAT_RIGHTLEFT
EXTERN دراز	p_aleph;	// 'aleph'
#پایان_اگر
EXTERN char_u	*p_ambw;	// 'ambiwidth'
#اگر_تعریف_شده FEAT_AUTOCHDIR
EXTERN صحيح	p_acd;		// 'autochdir'
#پایان_اگر
#اگر_تعریف_شده FEAT_AUTOSHELLDIR
EXTERN صحيح	p_asd;		// 'autoshelldir'
#پایان_اگر
EXTERN صحيح	p_ai;		// 'autoindent'
EXTERN صحيح	p_bin;		// 'binary'
EXTERN صحيح	p_bomb;		// 'bomb'
EXTERN صحيح	p_bl;		// 'buflisted'
EXTERN صحيح	p_cin;		// 'cindent'
EXTERN char_u	*p_cink;	// 'cinkeys'
EXTERN char_u	*p_cinsd;	// 'cinscopedecls'
EXTERN char_u	*p_cinw;	// 'cinwords'
#اگر_تعریف_شده FEAT_COMPL_FUNC
EXTERN char_u	*p_cfu;		// 'completefunc'
EXTERN char_u	*p_ofu;		// 'omnifunc'
EXTERN char_u	*p_tsrfu;	// 'thesaurusfunc'
#پایان_اگر
EXTERN صحيح	p_ci;		// 'copyindent'
#إذا defined(FEAT_GUI) && defined(MACOS_X)
EXTERN صحيح	*p_antialias;	// 'antialias'
#پایان_اگر
EXTERN صحيح	p_ar;		// 'autoread'
EXTERN صحيح	p_aw;		// 'autowrite'
EXTERN صحيح	p_awa;		// 'autowriteall'
EXTERN char_u	*p_bs;		// 'backspace'
EXTERN char_u	*p_bg;		// 'background'
EXTERN صحيح	p_bk;		// 'backup'
EXTERN char_u	*p_bkc;		// 'backupcopy'
EXTERN بیعلامت	bkc_flags;	// flags from 'backupcopy'
#تعریف_کن BKC_YES		0x001
#تعریف_کن BKC_AUTO		0x002
#تعریف_کن BKC_NO			0x004
#تعریف_کن BKC_BREAKSYMLINK	0x008
#تعریف_کن BKC_BREAKHARDLINK	0x010
EXTERN char_u	*p_bdir;	// 'backupdir'
EXTERN char_u	*p_bex;		// 'backupext'
EXTERN char_u	*p_bo;		// 'belloff'
EXTERN بیعلامت	bo_flags;

// values for the 'belloff' option
#تعریف_کن BO_ALL		0x0001
#تعریف_کن BO_BS		0x0002
#تعریف_کن BO_CRSR		0x0004
#تعریف_کن BO_COMPL	0x0008
#تعریف_کن BO_COPY		0x0010
#تعریف_کن BO_CTRLG	0x0020
#تعریف_کن BO_ERROR	0x0040
#تعریف_کن BO_ESC		0x0080
#تعریف_کن BO_EX		0x0100
#تعریف_کن BO_HANGUL	0x0200
#تعریف_کن BO_IM		0x0400
#تعریف_کن BO_LANG		0x0800
#تعریف_کن BO_MESS		0x1000
#تعریف_کن BO_تطابق	0x2000
#تعریف_کن BO_OPER		0x4000
#تعریف_کن BO_REG		0x8000
#تعریف_کن BO_SH		0x10000
#تعریف_کن BO_SPELL	0x20000
#تعریف_کن BO_جمله		0x40000
#تعریف_کن BO_WILD		0x80000

EXTERN char_u	*p_bsk;		// 'backupskip'
#اگر_تعریف_شده FEAT_CRYPT
EXTERN char_u	*p_cm;		// 'cryptmethod'
#پایان_اگر
#اگر_تعریف_شده FEAT_BEVAL
# ifdef FEAT_BEVAL_GUI
EXTERN صحيح	p_beval;	// 'ballooneval'
# endif
EXTERN دراز	p_bdlay;	// 'balloondelay'
# ifdef FEAT_EVAL
EXTERN char_u	*p_bexpr;
# endif
# ifdef FEAT_BEVAL_جمله
EXTERN صحيح	p_bevalterm;	// 'balloonevalterm'
# endif
#پایان_اگر
#اگر_تعریف_شده FEAT_BROWSE
EXTERN char_u	*p_bsdir;	// 'browsedir'
#پایان_اگر
#اگر_تعریف_شده FEAT_LINEBREAK
EXTERN char_u	*p_breakat;	// 'breakat'
#پایان_اگر
EXTERN char_u	*p_bh;		// 'bufhidden'
EXTERN char_u	*p_bt;		// 'buftype'
EXTERN char_u	*p_cmp;		// 'casemap'
EXTERN بیعلامت	cmp_flags;
#تعریف_کن CMP_داخلی		0x001
#تعریف_کن CMP_KEEPASCII		0x002
EXTERN char_u	*p_enc;		// 'encoding'
EXTERN صحيح	p_deco;		// 'delcombine'
#اگر_تعریف_شده FEAT_EVAL
EXTERN char_u	*p_ccv;		// 'charconvert'
#پایان_اگر
EXTERN صحيح	p_cdh;		// 'cdhome'
EXTERN char_u	*p_cino;	// 'cinoptions'
EXTERN char_u	*p_cedit;	// 'cedit'
EXTERN دراز	p_cwh;		// 'cmdwinheight'
#اگر_تعریف_شده HAVE_CLIPMETHOD
EXTERN char_u	*p_cb;		// 'clipboard'
EXTERN char_u	*p_cpm;		// 'clipmethod'
#پایان_اگر
EXTERN دراز	p_ch;		// 'cmdheight'
EXTERN char_u	*p_cms;		// 'commentstring'
EXTERN char_u	*p_cpt;		// 'complete'
EXTERN دراز	p_cto;		// 'completetimeout'
#إذا defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
EXTERN صحيح	p_confirm;	// 'confirm'
#پایان_اگر
EXTERN صحيح	p_cp;		// 'compatible'
EXTERN char_u	*p_cfc;		// 'completefuzzycollect'
EXTERN بیعلامت cfc_flags;	// flags from "completefuzzycollect"
EXTERN char_u	*p_cia;		// 'completeitemalign'
EXTERN بیعلامت cia_flags;	// order flags of 'completeitemalign'
EXTERN char_u	*p_cot;		// 'completeopt'
EXTERN بیعلامت	cot_flags;	// flags from 'completeopt'
EXTERN صحيح	p_ac;		// 'autocomplete'
EXTERN دراز	p_act;		// 'autocompletetimeout'
EXTERN دراز	p_acl;		// 'autocompletedelay'
// Keep in sync with p_cot_values in optionstr.c
#تعریف_کن COT_MENU	0x001
#تعریف_کن COT_MENUONE	0x002
#تعریف_کن COT_ANY_MENU	0x003	// combination of menu flags
#تعریف_کن COT_LONGEST	0x004	// FALSE: insert full match,
				// TRUE: insert longest prefix
#تعریف_کن COT_PREVIEW	    0x008
#تعریف_کن COT_POPUP	    0x010
#تعریف_کن COT_POPUPHIDDEN	    0x020
#تعریف_کن COT_ANY_PREVIEW	    0x038   // combination of preview flags
#تعریف_کن COT_NOINSERT	    0x040   // FALSE: select & insert, TRUE: noinsert
#تعریف_کن COT_NOSELECT	    0x080   // FALSE: select & insert, TRUE: noselect
#تعریف_کن COT_FUZZY	    0x100   // TRUE: fuzzy match enabled
#تعریف_کن COT_NOSORT	    0x200   // TRUE: fuzzy match without qsort score
#تعریف_کن COT_PREINSERT	    0x400   // TRUE: preinsert
#تعریف_کن COT_نزدیکترین	    0x800   // TRUE: prioritize matches close to cursor

#تعریف_کن CFC_KEYWORD         0x001
#تعریف_کن CFC_FILES           0x002
#تعریف_کن CFC_WHOLELINE       0x004

#اگر_تعریف_شده BACKSLASH_درون_FILENAME
EXTERN char_u	*p_csl;		// 'completeslash'
#پایان_اگر
EXTERN دراز	p_ph;		// 'pumheight'
EXTERN دراز	p_pw;		// 'pumwidth'
EXTERN دراز	p_pmw;		// 'pummaxwidth'
EXTERN char_u	*p_pb;		// 'pumborder'
EXTERN char_u	*p_pumopt;	// 'pumopt'
EXTERN char_u	*p_com;		// 'comments'
EXTERN char_u	*p_cpo;		// 'cpoptions'
#اگر_تعریف_شده FEAT_CSCOPE
EXTERN char_u	*p_csprg;	// 'cscopeprg'
EXTERN صحيح	p_csre;		// 'cscoperelative'
# ifdef FEAT_QUICKFIX
EXTERN char_u	*p_csqf;	// 'cscopequickfix'
#  define	CSQF_CMDS   "sgdctefia"
#  define	CSQF_FLAGS  "+-0"
# endif
EXTERN صحيح	p_cst;		// 'cscopetag'
EXTERN دراز	p_csto;		// 'cscopetagorder'
EXTERN دراز	p_cspc;		// 'cscopepathcomp'
EXTERN صحيح	p_csverbose;	// 'cscopeverbose'
#پایان_اگر
EXTERN char_u	*p_debug;	// 'debug'
#اگر_تعریف_شده FEAT_پیدا_کن_ID
EXTERN char_u	*p_def;		// 'define'
EXTERN char_u	*p_inc;
#پایان_اگر
#اگر_تعریف_شده FEAT_DIFF
EXTERN char_u	*p_dia;		// 'diffanchors'
EXTERN char_u	*p_dip;		// 'diffopt'
# ifdef FEAT_EVAL
EXTERN char_u	*p_dex;		// 'diffexpr'
# endif
#پایان_اگر
EXTERN char_u	*p_dict;	// 'dictionary'
#اگر_تعریف_شده FEAT_DIGRAPHS
EXTERN صحيح	p_dg;		// 'digraph'
#پایان_اگر
EXTERN char_u	*p_dir;		// 'directory'
EXTERN char_u	*p_dy;		// 'display'
EXTERN بیعلامت	dy_flags;
#تعریف_کن DY_LASTLINE		0x001
#تعریف_کن DY_TRUNCATE		0x002
#تعریف_کن DY_UHEX			0x004
EXTERN صحيح	p_ed;		// 'edcompatible'
EXTERN char_u	*p_ead;		// 'eadirection'
EXTERN char_u	*p_emoji;	// 'emoji'
EXTERN صحيح	p_ea;		// 'equalalways'
EXTERN char_u	*p_ep;		// 'equalprg'
EXTERN صحيح	p_eb;		// 'errorbells'
#اگر_تعریف_شده FEAT_QUICKFIX
EXTERN char_u	*p_ef;		// 'errorfile'
EXTERN char_u	*p_efm;		// 'errorformat'
EXTERN char_u	*p_gefm;	// 'grepformat'
EXTERN char_u	*p_gp;		// 'grepprg'
#پایان_اگر
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
#اگر_تعریف_شده FEAT_EVAL
EXTERN char_u	*p_ffu;		// 'findfunc'
#پایان_اگر
EXTERN صحيح	p_fixeol;	// 'fixendofline'
#اگر_تعریف_شده FEAT_FOLDING
EXTERN char_u	*p_fcl;		// 'foldclose'
EXTERN دراز	p_fdls;		// 'foldlevelstart'
EXTERN char_u	*p_fdo;		// 'foldopen'
EXTERN بیعلامت	fdo_flags;
# define FDO_ALL		0x001
# define FDO_بلوک		0x002
# define FDO_HOR		0x004
# define FDO_MARK		0x008
# define FDO_PERCENT		0x010
# define FDO_QUICKFIX		0x020
# define FDO_جستجو		0x040
# define FDO_برچسب		0x080
# define FDO_درج		0x100
# define FDO_UNDO		0x200
# define FDO_JUMP		0x400
#پایان_اگر
#إذا defined(FEAT_EVAL)
EXTERN char_u	*p_fex;		// 'formatexpr'
#پایان_اگر
EXTERN char_u	*p_flp;		// 'formatlistpat'
EXTERN char_u	*p_fo;		// 'formatoptions'
EXTERN char_u	*p_fp;		// 'formatprg'
#اگر_تعریف_شده HAVE_FSYNC
EXTERN صحيح	p_fs;		// 'fsync'
#پایان_اگر
EXTERN صحيح	p_gd;		// 'gdefault'
EXTERN char_u	*p_jop;		// 'jumpoptions'
EXTERN بیعلامت	jop_flags;	//
#تعریف_کن JOP_پشته		0x001
#اگر_تعریف_شده FEAT_PROP_POPUP
# ifdef FEAT_QUICKFIX
EXTERN char_u	*p_cpp;		// 'completepopup'
# endif
EXTERN char_u	*p_pvp;		// 'previewpopup'
#پایان_اگر
#اگر_تعریف_شده FEAT_PRINTER
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
EXTERN char_u	*p_سرآیند;	// 'printheader'
#پایان_اگر
EXTERN صحيح	p_prompt;	// 'prompt'
#اگر_تعریف_شده FEAT_GUI
EXTERN char_u	*p_guifont;	// 'guifont'
# ifdef FEAT_XFONTSET
EXTERN char_u	*p_guifontset;	// 'guifontset'
# endif
EXTERN char_u	*p_guifontwide;	// 'guifontwide'
EXTERN صحيح	p_guipty;	// 'guipty'
#پایان_اگر
#إذا defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
EXTERN char_u	*p_guiligatures;  // 'guiligatures'
#پایان_اگر
#إذا (defined(FEAT_GUI_GTK) && !defined(USE_GTK4)) || defined(FEAT_GUI_X11)
EXTERN دراز	p_ghr;		// 'guiheadroom'
#پایان_اگر
#اگر_تعریف_شده CURSOR_SHAPE
EXTERN char_u	*p_guicursor;	// 'guicursor'
#پایان_اگر
#اگر_تعریف_شده FEAT_MOUSESHAPE
EXTERN char_u	*p_mouseshape;	// 'mouseshape'
#پایان_اگر
#إذا defined(FEAT_GUI)
EXTERN char_u	*p_go;		// 'guioptions'
#پایان_اگر
#إذا defined(FEAT_GUI_TABLINE)
EXTERN char_u	*p_gtl;		// 'guitablabel'
EXTERN char_u	*p_gtt;		// 'guitabtooltip'
#پایان_اگر
EXTERN char_u	*p_hf;		// 'helpfile'
EXTERN دراز	p_hh;		// 'helpheight'
#اگر_تعریف_شده FEAT_MULTI_LANG
EXTERN char_u	*p_hlg;		// 'helplang'
#پایان_اگر
EXTERN صحيح	p_hid;		// 'hidden'
EXTERN char_u	*p_hl;		// 'highlight'
EXTERN صحيح	p_hls;		// 'hlsearch'
EXTERN دراز	p_hi;		// 'history'
#اگر_تعریف_شده FEAT_RIGHTLEFT
EXTERN صحيح	p_hkmap;	// 'hkmap'
EXTERN صحيح	p_hkmapp;	// 'hkmapp'
# ifdef FEAT_ARABIC
EXTERN صحيح	p_arshape;	// 'arabicshape'
# endif
#پایان_اگر
EXTERN صحيح	p_icon;		// 'icon'
EXTERN char_u	*p_iconstring;	// 'iconstring'
EXTERN صحيح	p_ic;		// 'ignorecase'
#إذا defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
EXTERN char_u	*p_imak;	// 'imactivatekey'
# define IM_ON_THE_SPOT		0L
# define IM_OVER_THE_SPOT	1L
EXTERN دراز	p_imst;		// 'imstyle'
#پایان_اگر
#اگر_تعریف_شده FEAT_IMAGE
EXTERN char_u	*p_ipc;		// 'imageprotocol'
#پایان_اگر
#إذا defined(FEAT_EVAL)
EXTERN char_u	*p_imaf;	// 'imactivatefunc'
EXTERN char_u	*p_imsf;	// 'imstatusfunc'
#پایان_اگر
EXTERN صحيح	p_imcmdline;	// 'imcmdline'
EXTERN صحيح	p_imdisable;	// 'imdisable'
EXTERN دراز	p_iminsert;	// 'iminsert'
EXTERN دراز	p_imsearch;	// 'imsearch'
EXTERN صحيح	p_inf;		// 'infercase'
#إذا defined(FEAT_پیدا_کن_ID) && defined(FEAT_EVAL)
EXTERN char_u	*p_inex;	// 'includeexpr'
#پایان_اگر
EXTERN صحيح	p_is;		// 'incsearch'
#إذا defined(FEAT_EVAL)
EXTERN char_u	*p_inde;	// 'indentexpr'
EXTERN char_u	*p_indk;	// 'indentkeys'
#پایان_اگر
EXTERN صحيح	p_im;		// 'insertmode'
EXTERN char_u	*p_isf;		// 'isfname'
EXTERN char_u	*p_isi;		// 'isident'
EXTERN char_u	*p_isk;		// 'iskeyword'
EXTERN char_u	*p_isp;		// 'isprint'
EXTERN صحيح	p_js;		// 'joinspaces'
#اگر_تعریف_شده FEAT_CRYPT
EXTERN char_u	*p_کلید;		// 'key'
#پایان_اگر
#اگر_تعریف_شده FEAT_KEYMAP
EXTERN char_u	*p_keymap;	// 'keymap'
#پایان_اگر
EXTERN char_u	*p_kp;		// 'keywordprg'
EXTERN char_u	*p_km;		// 'keymodel'
EXTERN char_u	*p_kpc;		// 'keyprotocol'
#اگر_تعریف_شده FEAT_LANGMAP
EXTERN char_u	*p_langmap;	// 'langmap'
EXTERN صحيح	p_lnr;		// 'langnoremap'
EXTERN صحيح	p_lrm;		// 'langremap'
#پایان_اگر
#إذا defined(FEAT_MENU) && defined(FEAT_MULTI_LANG)
EXTERN char_u	*p_lm;		// 'langmenu'
#پایان_اگر
#اگر_تعریف_شده FEAT_GUI
EXTERN دراز	p_linespace;	// 'linespace'
#پایان_اگر
EXTERN صحيح	p_lisp;		// 'lisp'
EXTERN char_u	*p_lop;		// 'lispoptions'
EXTERN char_u	*p_lispwords;	// 'lispwords'
EXTERN دراز	p_ls;		// 'laststatus'
EXTERN دراز	p_stal;		// 'showtabline'
EXTERN char_u	*p_lcs;		// 'listchars'

EXTERN صحيح	p_lz;		// 'lazyredraw'
EXTERN صحيح	p_lpl;		// 'loadplugins'
#إذا defined(DYNAMIC_LUA)
EXTERN char_u	*p_luadll;	// 'luadll'
#پایان_اگر
EXTERN صحيح	p_magic;	// 'magic'
EXTERN char_u	*p_menc;	// 'makeencoding'
#اگر_تعریف_شده FEAT_QUICKFIX
EXTERN char_u	*p_mef;		// 'makeef'
EXTERN char_u	*p_mp;		// 'makeprg'
#پایان_اگر
EXTERN char_u	*p_mps;		// 'matchpairs'
EXTERN دراز	p_mat;		// 'matchtime'
EXTERN دراز	p_mco;		// 'maxcombine'
#اگر_تعریف_شده FEAT_EVAL
EXTERN دراز	p_mfd;		// 'maxfuncdepth'
#پایان_اگر
EXTERN دراز	p_mmd;		// 'maxmapdepth'
EXTERN دراز	p_mm;		// 'maxmem'
EXTERN دراز	p_mmp;		// 'maxmempattern'
EXTERN دراز	p_mmt;		// 'maxmemtot'
#اگر_تعریف_شده FEAT_MENU
EXTERN دراز	p_mis;		// 'menuitems'
#پایان_اگر
EXTERN char_u	*p_mopt;	// 'messagesopt'
EXTERN دراز	p_msc;		// 'maxsearchcount'
#اگر_تعریف_شده FEAT_SPELL
EXTERN char_u	*p_msm;		// 'mkspellmem'
#پایان_اگر
EXTERN صحيح	p_ml;		// 'modeline'
EXTERN صحيح	p_mle;		// 'modelineexpr'
EXTERN دراز	p_mls;		// 'modelines'
EXTERN صحيح	p_mlstr;	// 'modelinestrict'
EXTERN صحيح	p_ma;		// 'modifiable'
EXTERN صحيح	p_mod;		// 'modified'
EXTERN char_u	*p_mouse;	// 'mouse'
#اگر_تعریف_شده FEAT_GUI
EXTERN صحيح	p_mousef;	// 'mousefocus'
EXTERN صحيح	p_mh;		// 'mousehide'
#پایان_اگر
EXTERN char_u	*p_mousem;	// 'mousemodel'
#اگر_تعریف_شده FEAT_GUI
EXTERN صحيح	p_mousemev;	// 'mousemoveevent'
#پایان_اگر
EXTERN دراز	p_mouset;	// 'mousetime'
EXTERN صحيح	p_بیشتر;		// 'more'
#اگر_تعریف_شده FEAT_MZSCHEME
EXTERN دراز	p_mzq;		// 'mzquantum
# إذا defined(DYNAMIC_MZSCHEME)
EXTERN char_u	*p_mzschemedll;	// 'mzschemedll'
EXTERN char_u	*p_mzschemegcdll; // 'mzschemegcdll'
# endif
#پایان_اگر
EXTERN char_u	*p_nf;		// 'nrformats'
#إذا defined(MSWIN)
EXTERN صحيح	p_odev;		// 'opendevice'
#پایان_اگر
EXTERN دراز	p_ost;		// 'osctimeoutlen'
EXTERN char_u	*p_opfunc;	// 'operatorfunc'
EXTERN char_u	*p_para;	// 'paragraphs'
EXTERN صحيح	p_paste;	// 'paste'
EXTERN char_u	*p_pt;		// 'pastetoggle'
#إذا defined(FEAT_EVAL) && defined(FEAT_DIFF)
EXTERN char_u	*p_pex;		// 'patchexpr'
#پایان_اگر
EXTERN char_u	*p_pm;		// 'patchmode'
EXTERN char_u	*p_مسیر;	// 'path'
EXTERN char_u	*p_cdpath;	// 'cdpath'
#إذا defined(DYNAMIC_PERL)
EXTERN char_u	*p_perldll;	// 'perldll'
#پایان_اگر
EXTERN صحيح	p_pi;		// 'preserveindent'
#إذا defined(DYNAMIC_PYTHON3)
EXTERN char_u	*p_py3dll;	// 'pythonthreedll'
#پایان_اگر
#اگر_تعریف_شده FEAT_PYTHON3
EXTERN char_u	*p_py3home;	// 'pythonthreehome'
#پایان_اگر
#إذا defined(DYNAMIC_PYTHON)
EXTERN char_u	*p_pydll;	// 'pythondll'
#پایان_اگر
#اگر_تعریف_شده FEAT_PYTHON
EXTERN char_u	*p_pyhome;	// 'pythonhome'
#پایان_اگر
#إذا defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
EXTERN دراز	p_pyx;		// 'pyxversion'
#پایان_اگر
EXTERN char_u	*p_qe;		// 'quoteescape'
EXTERN صحيح	p_ro;		// 'readonly'
#اگر_تعریف_شده FEAT_RELTIME
EXTERN دراز	p_rdt;		// 'redrawtime'
#پایان_اگر
EXTERN صحيح	p_remap;	// 'remap'
EXTERN دراز	p_re;		// 'regexpengine'
#اگر_تعریف_شده FEAT_RENDER_OPTIONS
EXTERN char_u	*p_rop;		// 'renderoptions'
#پایان_اگر
EXTERN دراز	p_report;	// 'report'
#إذا defined(FEAT_QUICKFIX)
EXTERN دراز	p_pvh;		// 'previewheight'
EXTERN دراز     p_chi;          // 'chistory'
#پایان_اگر
#اگر_تعریف_شده MSWIN
EXTERN صحيح	p_rs;		// 'restorescreen'
#پایان_اگر
#اگر_تعریف_شده FEAT_RIGHTLEFT
EXTERN صحيح	p_ari;		// 'allowrevins'
EXTERN صحيح	p_ri;		// 'revins'
#پایان_اگر
#إذا defined(DYNAMIC_RUBY)
EXTERN char_u	*p_rubydll;	// 'rubydll'
#پایان_اگر
EXTERN صحيح	p_ru;		// 'ruler'
#اگر_تعریف_شده FEAT_STL_OPT
EXTERN char_u	*p_ruf;		// 'rulerformat'
#پایان_اگر
EXTERN char_u	*p_pp;		// 'packpath'
#اگر_تعریف_شده FEAT_QUICKFIX
EXTERN char_u	*p_qftf;	// 'quickfixtextfunc'
#پایان_اگر
EXTERN char_u	*p_rtp;		// 'runtimepath'
EXTERN دراز	p_sj;		// 'scrolljump'
#إذا defined(MSWIN) && defined(FEAT_GUI)
EXTERN صحيح	p_scf;		// 'scrollfocus'
#پایان_اگر
EXTERN دراز	p_so;		// 'scrolloff'
EXTERN دراز	p_sop;		// 'scrolloffpad'
EXTERN char_u	*p_sbo;		// 'scrollopt'
EXTERN char_u	*p_sections;	// 'sections'
EXTERN صحيح	p_secure;	// 'secure'
EXTERN char_u	*p_sel;		// 'selection'
EXTERN char_u	*p_slm;		// 'selectmode'
#اگر_تعریف_شده FEAT_SESSION
EXTERN char_u	*p_ssop;	// 'sessionoptions'
EXTERN بیعلامت	ssop_flags;
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
#پایان_اگر
EXTERN char_u	*p_sh;		// 'shell'
EXTERN char_u	*p_shcf;	// 'shellcmdflag'
#اگر_تعریف_شده FEAT_QUICKFIX
EXTERN char_u	*p_sp;		// 'shellpipe'
#پایان_اگر
EXTERN char_u	*p_shq;		// 'shellquote'
EXTERN char_u	*p_sxq;		// 'shellxquote'
EXTERN char_u	*p_sxe;		// 'shellxescape'
EXTERN char_u	*p_srr;		// 'shellredir'
#اگر_تعریف_شده AMIGA
EXTERN دراز	p_st;		// 'shelltype'
#پایان_اگر
EXTERN صحيح	p_stmp;		// 'shelltemp'
#اگر_تعریف_شده BACKSLASH_درون_FILENAME
EXTERN صحيح	p_ssl;		// 'shellslash'
#پایان_اگر
#اگر_تعریف_شده FEAT_STL_OPT
EXTERN char_u	*p_stl;		// 'statusline'
EXTERN char_u	*p_stlo;	// 'statuslineopt'
#پایان_اگر
EXTERN صحيح	p_sr;		// 'shiftround'
EXTERN دراز	p_sw;		// 'shiftwidth'
EXTERN char_u	*p_shm;		// 'shortmess'
EXTERN صحيح	p_sn;		// 'shortname'
#اگر_تعریف_شده FEAT_LINEBREAK
EXTERN char_u	*p_sbr;		// 'showbreak'
#پایان_اگر
EXTERN صحيح	p_sc;		// 'showcmd'
EXTERN char_u	*p_sloc;	// 'showcmdloc'
EXTERN صحيح	p_sft;		// 'showfulltag'
EXTERN صحيح	p_sm;		// 'showmatch'
EXTERN صحيح	p_smd;		// 'showmode'
EXTERN دراز	p_ss;		// 'sidescroll'
EXTERN دراز	p_siso;		// 'sidescrolloff'
EXTERN صحيح	p_scs;		// 'smartcase'
EXTERN صحيح	p_si;		// 'smartindent'
EXTERN صحيح	p_sta;		// 'smarttab'
EXTERN دراز	p_sts;		// 'softtabstop'
EXTERN صحيح	p_sb;		// 'splitbelow'
EXTERN char_u	*p_sua;		// 'suffixesadd'
EXTERN صحيح	p_swf;		// 'swapfile'
#اگر_تعریف_شده FEAT_SYN_HL
EXTERN دراز	p_smc;		// 'synmaxcol'
#پایان_اگر
EXTERN دراز	p_tpm;		// 'tabpagemax'
#اگر_تعریف_شده FEAT_STL_OPT
EXTERN char_u	*p_tal;		// 'tabline'
#پایان_اگر
#اگر_تعریف_شده FEAT_EVAL
EXTERN char_u	*p_tfu;		// 'tagfunc'
#پایان_اگر
#اگر_تعریف_شده FEAT_SPELL
EXTERN char_u	*p_spc;		// 'spellcapcheck'
EXTERN char_u	*p_spf;		// 'spellfile'
EXTERN char_u	*p_spl;		// 'spelllang'
EXTERN char_u	*p_spo;		// 'spelloptions'
EXTERN char_u	*p_sps;		// 'spellsuggest'
#پایان_اگر
EXTERN صحيح	p_spr;		// 'splitright'
EXTERN صحيح	p_sol;		// 'startofline'
EXTERN char_u	*p_su;		// 'suffixes'
EXTERN char_u	*p_sws;		// 'swapsync'
EXTERN char_u	*p_swb;		// 'switchbuf'
EXTERN بیعلامت	swb_flags;
// Keep in sync with p_swb_values in optionstr.c
#تعریف_کن SWB_USEOPEN		0x001
#تعریف_کن SWB_USETAB		0x002
#تعریف_کن SWB_تقسیم		0x004
#تعریف_کن SWB_NEWTAB		0x008
#تعریف_کن SWB_VSPLIT		0x010
#تعریف_کن SWB_USELAST		0x020

EXTERN char_u	*p_spk;		// 'splitkeep'

#إذا defined(FEAT_TABPANEL)
EXTERN char_u	*p_tpl;		// 'tabpanel'
EXTERN دراز	p_stpl;		// 'showtabpanel'
EXTERN char_u	*p_tplo;	// 'tabpanelopt'
#پایان_اگر

#اگر_تعریف_شده FEAT_SYN_HL
EXTERN char_u	*p_syn;		// 'syntax'
#پایان_اگر
EXTERN char_u	*p_tcl;		// 'tabclose'
EXTERN بیعلامت	tcl_flags;	// flags from 'tabclose'
#تعریف_کن TCL_چپ		0x001
#تعریف_کن TCL_USELAST		0x002
EXTERN دراز	p_ts;		// 'tabstop'
EXTERN صحيح	p_tbs;		// 'tagbsearch'
EXTERN char_u	*p_tc;		// 'tagcase'
EXTERN بیعلامت tc_flags;       // flags from 'tagcase'
#تعریف_کن TC_FOLLOWIC		0x01
#تعریف_کن TC_IGNORE		0x02
#تعریف_کن TC_تطابق		0x04
#تعریف_کن TC_FOLLOWSCS		0x08
#تعریف_کن TC_SMART		0x10
EXTERN دراز	p_tl;		// 'taglength'
EXTERN صحيح	p_tr;		// 'tagrelative'
EXTERN char_u	*p_برچسبها;	// 'tags'
EXTERN صحيح	p_tagsecure;    // 'tagsecure'
EXTERN صحيح	p_tgst;		// 'tagstack'
#إذا defined(DYNAMIC_TCL)
EXTERN char_u	*p_tcldll;	// 'tcldll'
#پایان_اگر
#اگر_تعریف_شده FEAT_ARABIC
EXTERN صحيح	p_tbidi;	// 'termbidi'
#پایان_اگر
EXTERN char_u	*p_tenc;	// 'termencoding'
#اگر_تعریف_شده FEAT_TERMGUICOLORS
EXTERN صحيح	p_tgc;		// 'termguicolors'
#پایان_اگر
#اگر_تعریف_شده UNIX
EXTERN char_u	*p_trz;		// 'termresize'
#پایان_اگر
EXTERN صحيح	p_tsy;		// 'termsync'
#اگر_تعریف_شده FEAT_TERMINAL
EXTERN دراز	p_twsl;		// 'termwinscroll'
#پایان_اگر
#إذا defined(MSWIN) && defined(FEAT_TERMINAL)
EXTERN char_u	*p_twt;		// 'termwintype'
#پایان_اگر
EXTERN صحيح	p_terse;	// 'terse'
EXTERN صحيح	p_ta;		// 'textauto'
EXTERN صحيح	p_tx;		// 'textmode'
EXTERN دراز	p_tw;		// 'textwidth'
EXTERN صحيح	p_مقصد;		// 'tildeop'
EXTERN صحيح	p_timeout;	// 'timeout'
EXTERN دراز	p_tm;		// 'timeoutlen'
EXTERN صحيح	p_title;	// 'title'
EXTERN دراز	p_titlelen;	// 'titlelen'
EXTERN char_u	*p_titleold;	// 'titleold'
EXTERN char_u	*p_titlestring;	// 'titlestring'
EXTERN char_u	*p_tsr;		// 'thesaurus'
EXTERN صحيح	p_ttimeout;	// 'ttimeout'
EXTERN دراز	p_ttm;		// 'ttimeoutlen'
EXTERN صحيح	p_tbi;		// 'ttybuiltin'
EXTERN صحيح	p_tf;		// 'ttyfast'
#إذا defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
EXTERN char_u	*p_toolbar;	// 'toolbar'
EXTERN بیعلامت toolbar_flags;
# define TOOLBAR_متن		0x01
# define TOOLBAR_ICONS		0x02
# define TOOLBAR_TOOLTIPS	0x04
# define TOOLBAR_HORIZ		0x08
#پایان_اگر
#إذا defined(FEAT_TOOLBAR) && defined(FEAT_GUI_GTK)
EXTERN char_u	*p_tbis;	// 'toolbariconsize'
EXTERN بیعلامت tbis_flags;
# define TBIS_TINY		0x01
# define TBIS_SMALL		0x02
# define TBIS_MEDIUM		0x04
# define TBIS_LARGE		0x08
# define TBIS_HUGE		0x10
# define TBIS_GIANT		0x20
#پایان_اگر
EXTERN دراز	p_ttyscroll;	// 'ttyscroll'
#إذا defined(UNIX) || defined(VMS)
EXTERN char_u	*p_ttym;	// 'ttymouse'
EXTERN بیعلامت ttym_flags;
# define TTYM_XTERM		0x01
# define TTYM_XTERM2		0x02
# define TTYM_DEC		0x04
# define TTYM_NETTERM		0x08
# define TTYM_JSBTERM		0x10
# define TTYM_PTERM		0x20
# define TTYM_URXVT		0x40
# define TTYM_SGR		0x80
#پایان_اگر
#اگر_تعریف_شده FEAT_PERSISTENT_UNDO
EXTERN char_u	*p_udir;	// 'undodir'
EXTERN صحيح	p_udf;		// 'undofile'
#پایان_اگر
EXTERN دراز	p_ul;		// 'undolevels'
EXTERN دراز	p_ur;		// 'undoreload'
EXTERN دراز	p_uc;		// 'updatecount'
EXTERN دراز	p_ut;		// 'updatetime'
#اگر_تعریف_شده FEAT_VARTABS
EXTERN char_u	*p_vsts;	// 'varsofttabstop'
EXTERN char_u	*p_vts;		// 'vartabstop'
#پایان_اگر
#اگر_تعریف_شده FEAT_VIMINFO
EXTERN char_u	*p_viminfo;	// 'viminfo'
EXTERN char_u	*p_viminfofile;	// 'viminfofile'
#پایان_اگر
#اگر_تعریف_شده FEAT_SESSION
EXTERN char_u	*p_vdir;	// 'viewdir'
EXTERN char_u	*p_vop;		// 'viewoptions'
EXTERN بیعلامت	vop_flags;	// uses SSOP_ flags
#پایان_اگر
EXTERN صحيح	p_vb;		// 'visualbell'
EXTERN char_u	*p_ve;		// 'virtualedit'
EXTERN بیعلامت ve_flags;
#تعریف_کن VE_بلوک	5	// includes "all"
#تعریف_کن VE_درج	6	// includes "all"
#تعریف_کن VE_ALL		4
#تعریف_کن VE_ONEMORE	8
#تعریف_کن VE_NONE		16	// "none"
#تعریف_کن VE_NONEU	32      // "NONE"
EXTERN دراز	p_verbose;	// 'verbose'
#اگر_تعریف_شده درون_OPTION_C
char_u	*p_vfile = (char_u *)""; // used before options are initialized
#وإلا
خارجي char_u	*p_vfile;	// 'verbosefile'
#پایان_اگر
EXTERN صحيح	p_warn;		// 'warn'
EXTERN char_u	*p_wop;		// 'wildoptions'
EXTERN دراز	p_پنجره;	// 'window'
#إذا defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_MOTIF) || defined(LINT) \
	|| defined (FEAT_GUI_GTK) || defined(FEAT_GUI_PHOTON)
# define FEAT_WAK
EXTERN char_u	*p_wak;		// 'winaltkeys'
#پایان_اگر
EXTERN char_u	*p_wig;		// 'wildignore'
EXTERN صحيح	p_wiv;		// 'weirdinvert'
EXTERN char_u	*p_ww;		// 'whichwrap'
EXTERN دراز	p_wc;		// 'wildchar'
EXTERN دراز	p_wcm;		// 'wildcharm'
EXTERN صحيح	p_wic;		// 'wildignorecase'
EXTERN char_u	*p_wim;		// 'wildmode'
EXTERN صحيح	p_wmnu;		// 'wildmenu'
EXTERN دراز	p_wh;		// 'winheight'
EXTERN دراز	p_wmh;		// 'winminheight'
EXTERN دراز	p_wmw;		// 'winminwidth'
EXTERN دراز	p_wiw;		// 'winwidth'
#اگر_تعریف_شده FEAT_WAYLAND
EXTERN char_u	*p_wse;		// 'wlseat'
EXTERN دراز     p_wtm;		// 'wltimeoutlen'
#پایان_اگر
#إذا defined(MSWIN) && defined(FEAT_TERMINAL)
EXTERN char_u	*p_winptydll;	// 'winptydll'
#پایان_اگر
EXTERN دراز	p_wm;		// 'wrapmargin'
EXTERN صحيح	p_ws;		// 'wrapscan'
EXTERN صحيح	p_write;	// 'write'
EXTERN صحيح	p_wa;		// 'writeany'
EXTERN صحيح	p_wb;		// 'writebackup'
EXTERN دراز	p_wd;		// 'writedelay'
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
#اگر_تعریف_شده FEAT_QUICKFIX
    , BV_EFM
    , BV_GEFM
    , BV_GP
    , BV_MP
#پایان_اگر
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
#اگر_تعریف_شده FEAT_DIFF
    , BV_DIA
#پایان_اگر
    , BV_TSR
#اگر_تعریف_شده BACKSLASH_درون_FILENAME
    , BV_CSL
#پایان_اگر
#اگر_تعریف_شده FEAT_COMPL_FUNC
    , BV_CFU
#پایان_اگر
#اگر_تعریف_شده FEAT_پیدا_کن_ID
    , BV_DEF
    , BV_INC
#پایان_اگر
    , BV_EOF
    , BV_EOL
    , BV_FIXEOL
    , BV_EP
    , BV_ET
    , BV_FENC
    , BV_FP
#اگر_تعریف_شده FEAT_EVAL
    , BV_BEXPR
    , BV_FEX
    , BV_FFU
#پایان_اگر
    , BV_FF
    , BV_FLP
    , BV_FO
#اگر_تعریف_شده HAVE_FSYNC
    , BV_FS
#پایان_اگر
    , BV_FT
    , BV_IMI
    , BV_IMS
#إذا defined(FEAT_EVAL)
    , BV_INDE
    , BV_INDK
#پایان_اگر
#إذا defined(FEAT_پیدا_کن_ID) && defined(FEAT_EVAL)
    , BV_INEX
#پایان_اگر
    , BV_INF
    , BV_ISE
    , BV_ISK
#اگر_تعریف_شده FEAT_CRYPT
    , BV_کلید
#پایان_اگر
#اگر_تعریف_شده FEAT_KEYMAP
    , BV_KMAP
#پایان_اگر
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
#اگر_تعریف_شده FEAT_COMPL_FUNC
    , BV_OFU
#پایان_اگر
    , BV_مسیر
    , BV_PI
    , BV_QE
    , BV_RO
    , BV_SI
    , BV_SN
#اگر_تعریف_شده FEAT_SYN_HL
    , BV_SMC
    , BV_SYN
#پایان_اگر
#اگر_تعریف_شده FEAT_SPELL
    , BV_SPC
    , BV_SPF
    , BV_SPL
    , BV_SPO
#پایان_اگر
    , BV_STS
    , BV_SUA
    , BV_SW
    , BV_SWF
#اگر_تعریف_شده FEAT_EVAL
    , BV_TFU
#پایان_اگر
    , BV_برچسبها
    , BV_TC
#اگر_تعریف_شده FEAT_COMPL_FUNC
    , BV_TSRFU
#پایان_اگر
    , BV_TS
    , BV_TW
    , BV_TX
    , BV_UDF
    , BV_UL
    , BV_WM
#اگر_تعریف_شده FEAT_TERMINAL
    , BV_TWSL
#پایان_اگر
#اگر_تعریف_شده FEAT_VARTABS
    , BV_VSTS
    , BV_VTS
#پایان_اگر
    , BV_شمار	    // must be the last one
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
#اگر_تعریف_شده FEAT_ARABIC
    , WV_ARAB
#پایان_اگر
#اگر_تعریف_شده FEAT_CONCEAL
    , WV_COCU
    , WV_COLE
#پایان_اگر
#اگر_تعریف_شده FEAT_TERMINAL
    , WV_TWK
    , WV_TWS
#پایان_اگر
    , WV_CRBIND
#اگر_تعریف_شده FEAT_LINEBREAK
    , WV_BRI
    , WV_BRIOPT
#پایان_اگر
    , WV_WCR
#اگر_تعریف_شده FEAT_DIFF
    , WV_DIFF
#پایان_اگر
    , WV_EIW
#اگر_تعریف_شده FEAT_FOLDING
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
#پایان_اگر
#اگر_تعریف_شده FEAT_LINEBREAK
    , WV_LBR
#پایان_اگر
    , WV_NU
    , WV_RNU
    , WV_VE
#اگر_تعریف_شده FEAT_LINEBREAK
    , WV_NUW
#پایان_اگر
#إذا defined(FEAT_QUICKFIX)
    , WV_PVW
    , WV_LHI
#پایان_اگر
#اگر_تعریف_شده FEAT_RIGHTLEFT
    , WV_RL
    , WV_RLC
#پایان_اگر
    , WV_SCBIND
    , WV_SCROLL
    , WV_SMS
    , WV_SISO
    , WV_SO
    , WV_SOP
#اگر_تعریف_شده FEAT_SPELL
    , WV_SPELL
#پایان_اگر
#اگر_تعریف_شده FEAT_SYN_HL
    , WV_CUC
    , WV_CUL
    , WV_CULOPT
    , WV_CC
#پایان_اگر
#اگر_تعریف_شده FEAT_LINEBREAK
    , WV_SBR
#پایان_اگر
#اگر_تعریف_شده FEAT_STL_OPT
    , WV_STL
    , WV_STLO
#پایان_اگر
    , WV_WFB
    , WV_WFH
    , WV_WFW
    , WV_WHL
    , WV_WRAP
#اگر_تعریف_شده FEAT_SIGNS
    , WV_SCL
#پایان_اگر
    , WV_شمار	    // must be the last one
};

// Value for b_p_ul indicating the global value must be used.
#تعریف_کن NO_LOCAL_UNDOLEVEL (-123456)

#تعریف_کن ERR_BUFLEN 80

#پایان_اگر // _OPTION_H_
