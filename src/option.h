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

#جے_بنا_تعریف _OPTION_H_
#تعریف_کرو _OPTION_H_

//
// Option Flags
//
#تعریف_کرو P_BOOL		0x01	// the option is boolean
#تعریف_کرو P_عدد		0x02	// the option is numeric
#تعریف_کرو P_STRING	0x04	// the option is a string
#تعریف_کرو P_ALLOCED	0x08	// the string option is in allocated memory,
				// must use free_string_option() when
				// assigning new value. Not set if default is
				// the same.
#تعریف_کرو P_EXPAND	0x10	// environment expansion.  NOTE: P_EXPAND can
				// never be used for local or hidden options!
#تعریف_کرو P_NO_CMD_EXPAND	0x20	// don't perform cmdline completions
#تعریف_کرو P_NODEFAULT	0x40	// don't set to default value
#تعریف_کرو P_DEF_ALLOCED	0x80	// default value is in allocated memory, must
				//  use vim_free() when assigning new value
#تعریف_کرو P_WAS_سیٹ	0x100	// option has been set/reset
#تعریف_کرو P_NO_MKRC	0x200	// don't include in :mkvimrc output
#تعریف_کرو P_VI_DEF	0x400	// Use Vi default for Vim
#تعریف_کرو P_VIM		0x800	// Vim option, reset when 'cp' set

				// when option changed, what to display:
#تعریف_کرو P_RSTAT		0x1000	// redraw status lines
#تعریف_کرو P_RWIN		0x2000	// redraw current window and recompute text
#تعریف_کرو P_RBUF		0x4000	// redraw current buffer and recompute text
#تعریف_کرو P_RALL		0x6000	// redraw all windows and recompute text
#تعریف_کرو P_RCLR		0x7000	// clear and redraw all and recompute text

#تعریف_کرو P_COMMA		 0x8000	 // comma separated list
#تعریف_کرو P_ONECOMMA	0x18000L // P_COMMA and cannot have two consecutive
				 // commas
#تعریف_کرو P_NODUP		0x20000L // don't allow duplicate strings
#تعریف_کرو P_FLAGLIST	0x40000L // list of single-char flags

#تعریف_کرو P_SECURE	0x80000L // cannot change in modeline or secure mode
#تعریف_کرو P_GETTEXT      0x100000L // expand default value with _()
#تعریف_کرو P_NOGLOB       0x200000L // do not use local value for global vimrc
#تعریف_کرو P_NFNAME       0x400000L // only normal file name chars allowed
#تعریف_کرو P_INSECURE     0x800000L // option was set from a modeline
#تعریف_کرو P_PRI_MKRC    0x1000000L // priority for :mkvimrc (setting option has
				 // side effects)
#تعریف_کرو P_NO_ML       0x2000000L // not allowed in modeline
#تعریف_کرو P_CURSWANT    0x4000000L // update curswant required; not needed when
				 // there is a redraw flag
#تعریف_کرو P_NDNAME      0x8000000L // only normal dir name chars allowed
#تعریف_کرو P_HLONLY     0x10000000L // option only changes highlight, not text
#تعریف_کرو P_MLE	     0x20000000L // under control of 'modelineexpr'
#تعریف_کرو P_FUNC	     0x40000000L // accept a function reference or a lambda
#تعریف_کرو P_COLON	     0x80000000L // values use colons to create sublists
// Warning: Currently we have used all 32 bits for option flags. On some 32-bit
//          systems, the flags are stored as a 32-bit integer, and adding more
//          flags will overflow it. Adding another flag will need to change how
//          it's stored first.

// Returned by get_option_value().
تعريفنوع تعداد {
    gov_unknown,
    gov_bool,
    gov_نمبر,
    gov_string,
    gov_hidden_bool,
    gov_hidden_نمبر,
    gov_hidden_string
} getoption_T;

/*
 * Default values for 'errorformat'.
 * The "%f|%l| %m" one is used for when the contents of the quickfix window is
 * written to a file.
 */
#جے_تعریف_شدہ AMIGA
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
#جے_انت

#تعریف_کرو DFLT_GREPFORMAT	"%f:%l:%m,%f:%l%m,%f  %l%m"

// default values for b_p_ff 'fileformat' and p_ffs 'fileformats'
#تعریف_کرو FF_DOS		"dos"
#تعریف_کرو FF_MAC		"mac"
#تعریف_کرو FF_UNIX		"unix"

#جے_تعریف_شدہ USE_CRNL
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
#جے_انت


// Possible values for 'encoding'
#تعریف_کرو ENC_UCSBOM	"ucs-bom"	// check for BOM at start of file

// default value for 'encoding'
#إذا defined(MSWIN) || defined(__MVS__)
# define ENC_DFLT	"utf-8"
#وإلا
# define ENC_DFLT	"latin1"
#جے_انت

// end-of-line style
#تعریف_کرو EOL_UNKNOWN	(-1)	// not defined yet
#تعریف_کرو EOL_UNIX	0	// NL
#تعریف_کرو EOL_DOS		1	// CR NL
#تعریف_کرو EOL_MAC		2	// CR

// Formatting options for p_fo 'formatoptions'
#تعریف_کرو FO_WRAP		't'
#تعریف_کرو FO_WRAP_COMS	'c'
#تعریف_کرو FO_RET_COMS	'r'
#تعریف_کرو FO_OPEN_COMS	'o'
#تعریف_کرو FO_NO_OPEN_COMS	'/'
#تعریف_کرو FO_Q_COMS	'q'
#تعریف_کرو FO_Q_نمبر	'n'
#تعریف_کرو FO_Q_دوجا	'2'
#تعریف_کرو FO_INS_VI	'v'
#تعریف_کرو FO_INS_LONG	'l'
#تعریف_کرو FO_INS_BLANK	'b'
#تعریف_کرو FO_MBYTE_BREAK	'm'	// break before/after multi-byte char
#تعریف_کرو FO_MBYTE_JOIN	'M'	// no space before/after multi-byte char
#تعریف_کرو FO_MBYTE_JOIN2	'B'	// no space between multi-byte chars
#تعریف_کرو FO_ONE_LETTER	'1'
#تعریف_کرو FO_WHITE_PAR	'w'	// trailing white space continues paragr.
#تعریف_کرو FO_AUTO		'a'	// automatic formatting
#تعریف_کرو FO_RIGOROUS_TW	']'     // respect textwidth rigorously
#تعریف_کرو FO_REMOVE_COMS	'j'	// remove comment leaders when joining lines
#تعریف_کرو FO_PERIOD_ABBR	'p'	// don't break a single space after a period

#تعریف_کرو DFLT_FO_VI	"vt"
#تعریف_کرو DFLT_FO_VIM	"tcq"
#تعریف_کرو FO_ALL		"tcro/q2vlb1mMBn,aw]jp"	// for do_set()

// characters for the p_cpo option:
#تعریف_کرو CPO_ALTREAD	'a'	// ":read" sets alternate file name
#تعریف_کرو CPO_ALTWRITE	'A'	// ":write" sets alternate file name
#تعریف_کرو CPO_BAR		'b'	// "\|" ends a mapping
#تعریف_کرو CPO_BSLASH	'B'	// backslash in mapping is not special
#تعریف_کرو CPO_تلاش	'c'
#تعریف_کرو CPO_CONCAT	'C'	// Don't concatenate sourced lines
#تعریف_کرو CPO_DOTTAG	'd'	// "./tags" in 'tags' is in current dir
#تعریف_کرو CPO_DIGRAPH	'D'	// No digraph after "r", "f", etc.
#تعریف_کرو CPO_EXECBUF	'e'
#تعریف_کرو CPO_EMPTYREGION	'E'	// operating on empty region is an error
#تعریف_کرو CPO_FNAMER	'f'	// set file name for ":r file"
#تعریف_کرو CPO_FNAMEW	'F'	// set file name for ":w file"
#تعریف_کرو CPO_GOTO1	'g'	// goto line 1 for ":edit"
#تعریف_کرو CPO_INSEND	'H'	// "I" inserts before last blank in line
#تعریف_کرو CPO_INTMOD	'i'	// interrupt a read makes buffer modified
#تعریف_کرو CPO_INDENT	'I'	// remove auto-indent more often
#تعریف_کرو CPO_JOINSP	'j'	// only use two spaces for join after '.'
#تعریف_کرو CPO_ENDOFSENT	'J'	// need two spaces to detect end of sentence
#تعریف_کرو CPO_KEYCODE	'k'	// don't recognize raw key code in mappings
#تعریف_کرو CPO_KOFFSET	'K'	// don't wait for key code in mappings
#تعریف_کرو CPO_LITERAL	'l'	// take char after backslash in [] literal
#تعریف_کرو CPO_LISTWM	'L'	// 'list' changes wrapmargin
#تعریف_کرو CPO_SHOWMATCH	'm'
#تعریف_کرو CPO_MATCHBSL	'M'	// "%" ignores use of backslashes
#تعریف_کرو CPO_NUMCOL	'n'	// 'number' column also used for text
#تعریف_کرو CPO_LINEOFF	'o'
#تعریف_کرو CPO_OVERNEW	'O'	// silently overwrite new file
#تعریف_کرو CPO_LISP	'p'	// 'lisp' indenting
#تعریف_کرو CPO_FNAMEAPP	'P'	// set file name for ":w >>file"
#تعریف_کرو CPO_JOINCOL	'q'	// with "3J" use column after first join
#تعریف_کرو CPO_REDO	'r'
#تعریف_کرو CPO_REMMARK	'R'	// remove marks when filtering
#تعریف_کرو CPO_BUFOPT	's'
#تعریف_کرو CPO_BUFOPTGLOB	'S'
#تعریف_کرو CPO_TAGPAT	't'	// tag pattern is used for "n"
#تعریف_کرو CPO_UNDO	'u'	// "u" undoes itself
#تعریف_کرو CPO_BACKSPACE	'v'	// "v" keep deleted text
#تعریف_کرو CPO_CW		'w'	// "cw" only changes one blank
#تعریف_کرو CPO_FWRITE	'W'	// "w!" doesn't overwrite readonly files
#تعریف_کرو CPO_ESC		'x'
#تعریف_کرو CPO_REPLCNT	'X'	// "R" with a count only deletes chars once
#تعریف_کرو CPO_YANK	'y'
#تعریف_کرو CPO_KEEPRO	'Z'	// don't reset 'readonly' on ":w!"
#تعریف_کرو CPO_لفظ	'z'	// do not special-case word motions cw and dw
#تعریف_کرو CPO_DOLLAR	'$'
#تعریف_کرو CPO_FILTER	'!'
#تعریف_کرو CPO_میل	'%'
#تعریف_کرو CPO_تارا	'*'	// ":*" means ":@"
#تعریف_کرو CPO_PLUS	'+'	// ":write file" resets 'modified'
#تعریف_کرو CPO_MINUS	'-'	// "9-" fails at and before line 9
#تعریف_کرو CPO_SPECI	'<'	// don't recognize <> in mappings
#تعریف_کرو CPO_REGAPPEND	'>'	// insert NL when appending to a register
// POSIX flags
#تعریف_کرو CPO_HASH	'#'	// "D", "o" and "O" do not use a count
#تعریف_کرو CPO_PARA	'{'	// "{" is also a paragraph boundary
#تعریف_کرو CPO_TSIZE	'|'	// $LINES and $COLUMNS overrule term size
#تعریف_کرو CPO_PRESERVE	'&'	// keep swap file after :preserve
#تعریف_کرو CPO_SUBPERCENT	'/'	// % in :s string uses previous one
#تعریف_کرو CPO_BACKSL	'\\'	// \ is not special in []
#تعریف_کرو CPO_CHDIR	'.'	// don't chdir if buffer is modified
#تعریف_کرو CPO_SCOLON	';'	// using "," and ";" will skip over char if
				// cursor would not move
#تعریف_کرو CPO_NOSYMLINKS	'~'	// don't resolve symlinks when changing directory
// default values for Vim, Vi and POSIX
#تعریف_کرو CPO_VIM		"aABceFsz"
#تعریف_کرو CPO_VI		"aAbBcCdDeEfFgHiIjJkKlLmMnoOpPqrRsStuvwWxXyZz$!%*-+<>;"
#تعریف_کرو CPO_ALL		"aAbBcCdDeEfFgHiIjJkKlLmMnoOpPqrRsStuvwWxXyZz$!%*-+<>#{|&/\\.;~"

// characters for p_ww option:
#تعریف_کرو WW_ALL		"bshl<>[]~"

// characters for p_mouse option:
#تعریف_کرو MOUSE_عام	'n'		// use mouse in Normal mode
#تعریف_کرو MOUSE_VISUAL	'v'		// use mouse in Visual/Select mode
#تعریف_کرو MOUSE_داخل_کرو	'i'		// use mouse in Insert mode
#تعریف_کرو MOUSE_COMMAND	'c'		// use mouse in Command-line mode
#تعریف_کرو MOUSE_HELP	'h'		// use mouse in help buffers
#تعریف_کرو MOUSE_RETURN	'r'		// use mouse for hit-return message
#تعریف_کرو MOUSE_A		"nvich"		// used for 'a' flag
#تعریف_کرو MOUSE_ALL	"anvichr"	// all possible characters
#تعریف_کرو MOUSE_NONE	' '		// don't use Visual selection
#تعریف_کرو MOUSE_NONEF	'x'		// forced modeless selection

#تعریف_کرو COCU_ALL	"nvic"		// flags for 'concealcursor'

// characters for p_shm option:
#تعریف_کرو SHM_RO		'r'		// readonly
#تعریف_کرو SHM_MOD		'm'		// modified
#تعریف_کرو SHM_FILE	'f'		// (file 1 of 2)
#تعریف_کرو SHM_آخری	'i'		// last line incomplete
#تعریف_کرو SHM_متن	'x'		// tx instead of textmode
#تعریف_کرو SHM_سطریں	'l'		// "L" instead of "lines"
#تعریف_کرو SHM_NEW		'n'		// "[New]" instead of "[New file]"
#تعریف_کرو SHM_WRI		'w'		// "[w]" instead of "written"
#تعریف_کرو SHM_A		"rmfixlnw"	// represented by 'a' flag
#تعریف_کرو SHM_WRITE	'W'		// don't use "written" at all
#تعریف_کرو SHM_TRUNC	't'		// truncate file messages
#تعریف_کرو SHM_TRUNCALL	'T'		// truncate all messages
#تعریف_کرو SHM_OVER	'o'		// overwrite file messages
#تعریف_کرو SHM_OVERALL	'O'		// overwrite more messages
#تعریف_کرو SHM_تلاش	's'		// no search hit bottom messages
#تعریف_کرو SHM_ATTENTION	'A'		// no ATTENTION messages
#تعریف_کرو SHM_INTRO	'I'		// intro messages
#تعریف_کرو SHM_COMPLETIONMENU  'c'		// completion menu messages
#تعریف_کرو SHM_COMPLETIONSCAN  'C'		// completion scanning messages
#تعریف_کرو SHM_RECORDING	'q'		// short recording message
#تعریف_کرو SHM_FILEINFO	'F'		// no file info messages
#تعریف_کرو SHM_SEARCHCOUNT  'S'		// no search stats: '[1/10]'
#تعریف_کرو SHM_UNDO	'u'		// undo messages
#تعریف_کرو SHM_POSIX       "AS"		// POSIX value
#تعریف_کرو SHM_ALL		"rmfixlnwaWtToOsAIcCqFSu" // all possible flags for 'shm'
#تعریف_کرو SHM_LEN		30		// max length of all flags together
					// plus a NUL character

// characters for p_go:
#تعریف_کرو GO_TERMINAL	'!'		// use terminal for system commands
#تعریف_کرو GO_ASEL		'a'		// autoselect
#تعریف_کرو GO_ASELML	'A'		// autoselect modeless selection
#تعریف_کرو GO_BOT		'b'		// use bottom scrollbar
#تعریف_کرو GO_CONDIALOG	'c'		// use console dialog
#تعریف_کرو GO_TITLEBAR	'C'		// use 'hl-TitleBar'
#تعریف_کرو GO_DARKTHEME	'd'		// use dark theme variant
#تعریف_کرو GO_TABLINE	'e'		// may show tabline
#تعریف_کرو GO_FORG		'f'		// start GUI in foreground
#تعریف_کرو GO_GREY		'g'		// use grey menu items
#تعریف_کرو GO_HORSCROLL	'h'		// flexible horizontal scrolling
#تعریف_کرو GO_ICON		'i'		// use Vim icon
#تعریف_کرو GO_بایاں		'l'		// use left scrollbar
#تعریف_کرو GO_VLEFT	'L'		// left scrollbar with vert split
#تعریف_کرو GO_MENUS	'm'		// use menu bar
#تعریف_کرو GO_NOSYSMENU	'M'		// don't source system menu
#تعریف_کرو GO_اشارہ_گر	'p'		// pointer enter/leave callbacks
#تعریف_کرو GO_ASELPLUS	'P'		// autoselectPlus
#تعریف_کرو GO_دایاں	'r'		// use right scrollbar
#تعریف_کرو GO_VRIGHT	'R'		// right scrollbar with vert split
#تعریف_کرو GO_FULLSCREEN	's'		// enter fullscreen
#تعریف_کرو GO_TEAROFF	't'		// add tear-off menu items
#تعریف_کرو GO_TOOLBAR	'T'		// add toolbar
#تعریف_کرو GO_FOOTER	'F'		// add footer
#تعریف_کرو GO_VERTICAL	'v'		// arrange dialog buttons vertically
#تعریف_کرو GO_KEEPWINSIZE	'k'		// keep GUI window size
// all possible flags for 'go'
#تعریف_کرو GO_ALL		"!aAbcCdefFghilLmMpPrRstTvk"

// flags for 'comments' option
#تعریف_کرو COM_NEST	'n'		// comments strings nest
#تعریف_کرو COM_BLANK	'b'		// needs blank after string
#تعریف_کرو COM_شروع	's'		// start of comment
#تعریف_کرو COM_درمیان	'm'		// middle of comment
#تعریف_کرو COM_سرا		'e'		// end of comment
#تعریف_کرو COM_AUTO_سرا	'x'		// last char of end closes comment
#تعریف_کرو COM_پہلا	'f'		// first line comment only
#تعریف_کرو COM_بایاں	'l'		// left adjusted
#تعریف_کرو COM_دایاں	'r'		// right adjusted
#تعریف_کرو COM_NOBACK	'O'		// don't use for "O" command
#تعریف_کرو COM_ALL		"nbsmexflrO"	// all flags for 'comments' option
#تعریف_کرو COM_MAX_LEN	50		// maximum length of a part

// flags for 'statusline' option
#تعریف_کرو STL_FILEPATH	'f'		// path of file in buffer
#تعریف_کرو STL_FULLPATH	'F'		// full path of file in buffer
#تعریف_کرو STL_FILENAME	't'		// last part (tail) of file path
#تعریف_کرو STL_کالم	'c'		// column og cursor
#تعریف_کرو STL_VIRTCOL	'v'		// virtual column
#تعریف_کرو STL_VIRTCOL_ALT	'V'		// - with 'if different' display
#تعریف_کرو STL_سطر	'l'		// line number of cursor
#تعریف_کرو STL_NUMLINES	'L'		// number of lines in buffer
#تعریف_کرو STL_BUFNO	'n'		// current buffer number
#تعریف_کرو STL_KEYMAP	'k'		// 'keymap' when active
#تعریف_کرو STL_وتھ	'o'		// offset of character under cursor
#تعریف_کرو STL_وتھ_X	'O'		// - in hexadecimal
#تعریف_کرو STL_BYTEVAL	'b'		// byte value of character
#تعریف_کرو STL_BYTEVAL_X	'B'		// - in hexadecimal
#تعریف_کرو STL_ROFLAG	'r'		// readonly flag
#تعریف_کرو STL_ROFLAG_ALT	'R'		// - other display
#تعریف_کرو STL_HELPFLAG	'h'		// window is showing a help file
#تعریف_کرو STL_HELPFLAG_ALT 'H'		// - other display
#تعریف_کرو STL_FILETYPE	'y'		// 'filetype'
#تعریف_کرو STL_FILETYPE_ALT 'Y'		// - other display
#تعریف_کرو STL_PREVIEWFLAG	'w'		// window is showing the preview buf
#تعریف_کرو STL_PREVIEWFLAG_ALT 'W'		// - other display
#تعریف_کرو STL_MODIFIED	'm'		// modified flag
#تعریف_کرو STL_MODIFIED_ALT 'M'		// - other display
#تعریف_کرو STL_QUICKFIX	'q'		// quickfix window description
#تعریف_کرو STL_PERCENTAGE	'p'		// percentage through file
#تعریف_کرو STL_ALTPERCENT	'P'		// percentage as TOP BOT ALL or NN%
#تعریف_کرو STL_ARGLISTSTAT	'a'		// argument list status as (x of y)
#تعریف_کرو STL_PAGENUM	'N'		// page number (when printing)
#تعریف_کرو STL_SHOWCMD	'S'		// 'showcmd' buffer
#تعریف_کرو STL_VIM_اظہاریہ	'{'		// start of expression to substitute
#تعریف_کرو STL_SEPARATE	'='		// separation between alignment
					// sections
#تعریف_کرو STL_TRUNCMARK	'<'		// truncation mark if line is too long
#تعریف_کرو STL_USER_HL	'*'		// highlight from (User)1..9 or 0
#تعریف_کرو STL_HIGHLIGHT	'#'		// highlight name
#تعریف_کرو STL_LINEBREAK	'@'		// insert a line break
#تعریف_کرو STL_CLICKFUNC	'['		// click handler region
#تعریف_کرو STL_TABPAGENR	'T'		// tab page label nr
#تعریف_کرو STL_TABCLOSENR	'X'		// tab page close nr
#تعریف_کرو STL_ALL		((char_u *) "fFtcvVlLknoObBrRhHmYyWwMqpPaNS{#@[")

// flags used for parsed 'wildmode'
#تعریف_کرو WIM_بھریا	0x01
#تعریف_کرو WIM_LONGEST	0x02
#تعریف_کرو WIM_LIST	0x04
#تعریف_کرو WIM_BUFLASTUSED	0x08
#تعریف_کرو WIM_NOSELECT	0x10
#تعریف_کرو WIM_NOINSERT	0x20

// flags for the 'wildoptions' option
// each defined char should be unique over all values.
#تعریف_کرو WOP_FUZZY	'z'
#تعریف_کرو WOP_TAGFILE	'g'
#تعریف_کرو WOP_PUM		'p'
#تعریف_کرو WOP_EXACTTEXT	'x'

// arguments for can_bs()
// each defined char should be unique over all values
// except for BS_START, that intentionally also matches BS_NOSTOP
// because BS_NOSTOP behaves exactly the same except it
// does not stop at the start of the insert point
#تعریف_کرو BS_INDENT	'i'	// "Indent"
#تعریف_کرو BS_EOL		'l'	// "eoL"
#تعریف_کرو BS_شروع	's'	// "Start"
#تعریف_کرو BS_NOSTOP	'p'	// "nostoP

// flags for the 'culopt' option
#تعریف_کرو CULOPT_سطر	0x01	// Highlight complete line
#تعریف_کرو CULOPT_SCRLINE	0x02	// Highlight screen line
#تعریف_کرو CULOPT_NBR	0x04	// Highlight Number column

#تعریف_کرو LISPWORD_قدر	"defun,define,defmacro,set!,lambda,if,case,let,flet,let*,letrec,do,do*,define-syntax,let-syntax,letrec-syntax,destructuring-bind,defpackage,defparameter,defstruct,deftype,defvar,do-all-symbols,do-external-symbols,do-symbols,dolist,dotimes,ecase,etypecase,eval-when,labels,macrolet,multiple-value-bind,multiple-value-call,multiple-value-prog1,multiple-value-setq,prog1,progv,typecase,unless,unwind-protect,when,with-input-from-string,with-open-file,with-open-stream,with-output-to-string,with-package-iterator,define-condition,handler-bind,handler-case,restart-bind,restart-case,with-simple-restart,store-value,use-value,muffle-warning,abort,continue,with-slots,with-slots*,with-accessors,with-accessors*,defclass,defmethod,print-unreadable-object"

/*
 * The following are actual variables for the options
 */

#جے_تعریف_شدہ FEAT_RIGHTLEFT
EXTERN لما	p_aleph;	// 'aleph'
#جے_انت
EXTERN char_u	*p_ambw;	// 'ambiwidth'
#جے_تعریف_شدہ FEAT_AUTOCHDIR
EXTERN صحيح	p_acd;		// 'autochdir'
#جے_انت
#جے_تعریف_شدہ FEAT_AUTOSHELLDIR
EXTERN صحيح	p_asd;		// 'autoshelldir'
#جے_انت
EXTERN صحيح	p_ai;		// 'autoindent'
EXTERN صحيح	p_bin;		// 'binary'
EXTERN صحيح	p_bomb;		// 'bomb'
EXTERN صحيح	p_bl;		// 'buflisted'
EXTERN صحيح	p_cin;		// 'cindent'
EXTERN char_u	*p_cink;	// 'cinkeys'
EXTERN char_u	*p_cinsd;	// 'cinscopedecls'
EXTERN char_u	*p_cinw;	// 'cinwords'
#جے_تعریف_شدہ FEAT_COMPL_FUNC
EXTERN char_u	*p_cfu;		// 'completefunc'
EXTERN char_u	*p_ofu;		// 'omnifunc'
EXTERN char_u	*p_tsrfu;	// 'thesaurusfunc'
#جے_انت
EXTERN صحيح	p_ci;		// 'copyindent'
#إذا defined(FEAT_GUI) && defined(MACOS_X)
EXTERN صحيح	*p_antialias;	// 'antialias'
#جے_انت
EXTERN صحيح	p_ar;		// 'autoread'
EXTERN صحيح	p_aw;		// 'autowrite'
EXTERN صحيح	p_awa;		// 'autowriteall'
EXTERN char_u	*p_bs;		// 'backspace'
EXTERN char_u	*p_bg;		// 'background'
EXTERN صحيح	p_bk;		// 'backup'
EXTERN char_u	*p_bkc;		// 'backupcopy'
EXTERN بنانشانی	bkc_flags;	// flags from 'backupcopy'
#تعریف_کرو BKC_YES		0x001
#تعریف_کرو BKC_AUTO		0x002
#تعریف_کرو BKC_NO			0x004
#تعریف_کرو BKC_BREAKSYMLINK	0x008
#تعریف_کرو BKC_BREAKHARDLINK	0x010
EXTERN char_u	*p_bdir;	// 'backupdir'
EXTERN char_u	*p_bex;		// 'backupext'
EXTERN char_u	*p_bo;		// 'belloff'
EXTERN بنانشانی	bo_flags;

// values for the 'belloff' option
#تعریف_کرو BO_ALL		0x0001
#تعریف_کرو BO_BS		0x0002
#تعریف_کرو BO_CRSR		0x0004
#تعریف_کرو BO_COMPL	0x0008
#تعریف_کرو BO_COPY		0x0010
#تعریف_کرو BO_CTRLG	0x0020
#تعریف_کرو BO_ERROR	0x0040
#تعریف_کرو BO_ESC		0x0080
#تعریف_کرو BO_EX		0x0100
#تعریف_کرو BO_HANGUL	0x0200
#تعریف_کرو BO_IM		0x0400
#تعریف_کرو BO_LANG		0x0800
#تعریف_کرو BO_MESS		0x1000
#تعریف_کرو BO_میل	0x2000
#تعریف_کرو BO_OPER		0x4000
#تعریف_کرو BO_REG		0x8000
#تعریف_کرو BO_SH		0x10000
#تعریف_کرو BO_SPELL	0x20000
#تعریف_کرو BO_رکن		0x40000
#تعریف_کرو BO_WILD		0x80000

EXTERN char_u	*p_bsk;		// 'backupskip'
#جے_تعریف_شدہ FEAT_CRYPT
EXTERN char_u	*p_cm;		// 'cryptmethod'
#جے_انت
#جے_تعریف_شدہ FEAT_BEVAL
# ifdef FEAT_BEVAL_GUI
EXTERN صحيح	p_beval;	// 'ballooneval'
# endif
EXTERN لما	p_bdlay;	// 'balloondelay'
# ifdef FEAT_EVAL
EXTERN char_u	*p_bexpr;
# endif
# ifdef FEAT_BEVAL_رکن
EXTERN صحيح	p_bevalterm;	// 'balloonevalterm'
# endif
#جے_انت
#جے_تعریف_شدہ FEAT_BROWSE
EXTERN char_u	*p_bsdir;	// 'browsedir'
#جے_انت
#جے_تعریف_شدہ FEAT_LINEBREAK
EXTERN char_u	*p_breakat;	// 'breakat'
#جے_انت
EXTERN char_u	*p_bh;		// 'bufhidden'
EXTERN char_u	*p_bt;		// 'buftype'
EXTERN char_u	*p_cmp;		// 'casemap'
EXTERN بنانشانی	cmp_flags;
#تعریف_کرو CMP_اندرونی		0x001
#تعریف_کرو CMP_KEEPASCII		0x002
EXTERN char_u	*p_enc;		// 'encoding'
EXTERN صحيح	p_deco;		// 'delcombine'
#جے_تعریف_شدہ FEAT_EVAL
EXTERN char_u	*p_ccv;		// 'charconvert'
#جے_انت
EXTERN صحيح	p_cdh;		// 'cdhome'
EXTERN char_u	*p_cino;	// 'cinoptions'
EXTERN char_u	*p_cedit;	// 'cedit'
EXTERN لما	p_cwh;		// 'cmdwinheight'
#جے_تعریف_شدہ HAVE_CLIPMETHOD
EXTERN char_u	*p_cb;		// 'clipboard'
EXTERN char_u	*p_cpm;		// 'clipmethod'
#جے_انت
EXTERN لما	p_ch;		// 'cmdheight'
EXTERN char_u	*p_cms;		// 'commentstring'
EXTERN char_u	*p_cpt;		// 'complete'
EXTERN لما	p_cto;		// 'completetimeout'
#إذا defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
EXTERN صحيح	p_confirm;	// 'confirm'
#جے_انت
EXTERN صحيح	p_cp;		// 'compatible'
EXTERN char_u	*p_cfc;		// 'completefuzzycollect'
EXTERN بنانشانی cfc_flags;	// flags from "completefuzzycollect"
EXTERN char_u	*p_cia;		// 'completeitemalign'
EXTERN بنانشانی cia_flags;	// order flags of 'completeitemalign'
EXTERN char_u	*p_cot;		// 'completeopt'
EXTERN بنانشانی	cot_flags;	// flags from 'completeopt'
EXTERN صحيح	p_ac;		// 'autocomplete'
EXTERN لما	p_act;		// 'autocompletetimeout'
EXTERN لما	p_acl;		// 'autocompletedelay'
// Keep in sync with p_cot_values in optionstr.c
#تعریف_کرو COT_MENU	0x001
#تعریف_کرو COT_MENUONE	0x002
#تعریف_کرو COT_ANY_MENU	0x003	// combination of menu flags
#تعریف_کرو COT_LONGEST	0x004	// FALSE: insert full match,
				// TRUE: insert longest prefix
#تعریف_کرو COT_PREVIEW	    0x008
#تعریف_کرو COT_POPUP	    0x010
#تعریف_کرو COT_POPUPHIDDEN	    0x020
#تعریف_کرو COT_ANY_PREVIEW	    0x038   // combination of preview flags
#تعریف_کرو COT_NOINSERT	    0x040   // FALSE: select & insert, TRUE: noinsert
#تعریف_کرو COT_NOSELECT	    0x080   // FALSE: select & insert, TRUE: noselect
#تعریف_کرو COT_FUZZY	    0x100   // TRUE: fuzzy match enabled
#تعریف_کرو COT_NOSORT	    0x200   // TRUE: fuzzy match without qsort score
#تعریف_کرو COT_PREINSERT	    0x400   // TRUE: preinsert
#تعریف_کرو COT_سبھ_توں_نیڑے	    0x800   // TRUE: prioritize matches close to cursor

#تعریف_کرو CFC_KEYWORD         0x001
#تعریف_کرو CFC_FILES           0x002
#تعریف_کرو CFC_WHOLELINE       0x004

#جے_تعریف_شدہ BACKSLASH_وچ_FILENAME
EXTERN char_u	*p_csl;		// 'completeslash'
#جے_انت
EXTERN لما	p_ph;		// 'pumheight'
EXTERN لما	p_pw;		// 'pumwidth'
EXTERN لما	p_pmw;		// 'pummaxwidth'
EXTERN char_u	*p_pb;		// 'pumborder'
EXTERN char_u	*p_pumopt;	// 'pumopt'
EXTERN char_u	*p_com;		// 'comments'
EXTERN char_u	*p_cpo;		// 'cpoptions'
#جے_تعریف_شدہ FEAT_CSCOPE
EXTERN char_u	*p_csprg;	// 'cscopeprg'
EXTERN صحيح	p_csre;		// 'cscoperelative'
# ifdef FEAT_QUICKFIX
EXTERN char_u	*p_csqf;	// 'cscopequickfix'
#  define	CSQF_CMDS   "sgdctefia"
#  define	CSQF_FLAGS  "+-0"
# endif
EXTERN صحيح	p_cst;		// 'cscopetag'
EXTERN لما	p_csto;		// 'cscopetagorder'
EXTERN لما	p_cspc;		// 'cscopepathcomp'
EXTERN صحيح	p_csverbose;	// 'cscopeverbose'
#جے_انت
EXTERN char_u	*p_debug;	// 'debug'
#جے_تعریف_شدہ FEAT_لبھو_ID
EXTERN char_u	*p_def;		// 'define'
EXTERN char_u	*p_inc;
#جے_انت
#جے_تعریف_شدہ FEAT_DIFF
EXTERN char_u	*p_dia;		// 'diffanchors'
EXTERN char_u	*p_dip;		// 'diffopt'
# ifdef FEAT_EVAL
EXTERN char_u	*p_dex;		// 'diffexpr'
# endif
#جے_انت
EXTERN char_u	*p_dict;	// 'dictionary'
#جے_تعریف_شدہ FEAT_DIGRAPHS
EXTERN صحيح	p_dg;		// 'digraph'
#جے_انت
EXTERN char_u	*p_dir;		// 'directory'
EXTERN char_u	*p_dy;		// 'display'
EXTERN بنانشانی	dy_flags;
#تعریف_کرو DY_LASTLINE		0x001
#تعریف_کرو DY_TRUNCATE		0x002
#تعریف_کرو DY_UHEX			0x004
EXTERN صحيح	p_ed;		// 'edcompatible'
EXTERN char_u	*p_ead;		// 'eadirection'
EXTERN char_u	*p_emoji;	// 'emoji'
EXTERN صحيح	p_ea;		// 'equalalways'
EXTERN char_u	*p_ep;		// 'equalprg'
EXTERN صحيح	p_eb;		// 'errorbells'
#جے_تعریف_شدہ FEAT_QUICKFIX
EXTERN char_u	*p_ef;		// 'errorfile'
EXTERN char_u	*p_efm;		// 'errorformat'
EXTERN char_u	*p_gefm;	// 'grepformat'
EXTERN char_u	*p_gp;		// 'grepprg'
#جے_انت
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
#جے_تعریف_شدہ FEAT_EVAL
EXTERN char_u	*p_ffu;		// 'findfunc'
#جے_انت
EXTERN صحيح	p_fixeol;	// 'fixendofline'
#جے_تعریف_شدہ FEAT_FOLDING
EXTERN char_u	*p_fcl;		// 'foldclose'
EXTERN لما	p_fdls;		// 'foldlevelstart'
EXTERN char_u	*p_fdo;		// 'foldopen'
EXTERN بنانشانی	fdo_flags;
# define FDO_ALL		0x001
# define FDO_بلاک		0x002
# define FDO_HOR		0x004
# define FDO_MARK		0x008
# define FDO_PERCENT		0x010
# define FDO_QUICKFIX		0x020
# define FDO_تلاش		0x040
# define FDO_ٹیگ		0x080
# define FDO_داخل_کرو		0x100
# define FDO_UNDO		0x200
# define FDO_JUMP		0x400
#جے_انت
#إذا defined(FEAT_EVAL)
EXTERN char_u	*p_fex;		// 'formatexpr'
#جے_انت
EXTERN char_u	*p_flp;		// 'formatlistpat'
EXTERN char_u	*p_fo;		// 'formatoptions'
EXTERN char_u	*p_fp;		// 'formatprg'
#جے_تعریف_شدہ HAVE_FSYNC
EXTERN صحيح	p_fs;		// 'fsync'
#جے_انت
EXTERN صحيح	p_gd;		// 'gdefault'
EXTERN char_u	*p_jop;		// 'jumpoptions'
EXTERN بنانشانی	jop_flags;	//
#تعریف_کرو JOP_ڈھیر		0x001
#جے_تعریف_شدہ FEAT_PROP_POPUP
# ifdef FEAT_QUICKFIX
EXTERN char_u	*p_cpp;		// 'completepopup'
# endif
EXTERN char_u	*p_pvp;		// 'previewpopup'
#جے_انت
#جے_تعریف_شدہ FEAT_PRINTER
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
EXTERN char_u	*p_سرخی;	// 'printheader'
#جے_انت
EXTERN صحيح	p_prompt;	// 'prompt'
#جے_تعریف_شدہ FEAT_GUI
EXTERN char_u	*p_guifont;	// 'guifont'
# ifdef FEAT_XFONTSET
EXTERN char_u	*p_guifontset;	// 'guifontset'
# endif
EXTERN char_u	*p_guifontwide;	// 'guifontwide'
EXTERN صحيح	p_guipty;	// 'guipty'
#جے_انت
#إذا defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
EXTERN char_u	*p_guiligatures;  // 'guiligatures'
#جے_انت
#إذا (defined(FEAT_GUI_GTK) && !defined(USE_GTK4)) || defined(FEAT_GUI_X11)
EXTERN لما	p_ghr;		// 'guiheadroom'
#جے_انت
#جے_تعریف_شدہ CURSOR_SHAPE
EXTERN char_u	*p_guicursor;	// 'guicursor'
#جے_انت
#جے_تعریف_شدہ FEAT_MOUSESHAPE
EXTERN char_u	*p_mouseshape;	// 'mouseshape'
#جے_انت
#إذا defined(FEAT_GUI)
EXTERN char_u	*p_go;		// 'guioptions'
#جے_انت
#إذا defined(FEAT_GUI_TABLINE)
EXTERN char_u	*p_gtl;		// 'guitablabel'
EXTERN char_u	*p_gtt;		// 'guitabtooltip'
#جے_انت
EXTERN char_u	*p_hf;		// 'helpfile'
EXTERN لما	p_hh;		// 'helpheight'
#جے_تعریف_شدہ FEAT_MULTI_LANG
EXTERN char_u	*p_hlg;		// 'helplang'
#جے_انت
EXTERN صحيح	p_hid;		// 'hidden'
EXTERN char_u	*p_hl;		// 'highlight'
EXTERN صحيح	p_hls;		// 'hlsearch'
EXTERN لما	p_hi;		// 'history'
#جے_تعریف_شدہ FEAT_RIGHTLEFT
EXTERN صحيح	p_hkmap;	// 'hkmap'
EXTERN صحيح	p_hkmapp;	// 'hkmapp'
# ifdef FEAT_ARABIC
EXTERN صحيح	p_arshape;	// 'arabicshape'
# endif
#جے_انت
EXTERN صحيح	p_icon;		// 'icon'
EXTERN char_u	*p_iconstring;	// 'iconstring'
EXTERN صحيح	p_ic;		// 'ignorecase'
#إذا defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
EXTERN char_u	*p_imak;	// 'imactivatekey'
# define IM_ON_THE_SPOT		0L
# define IM_OVER_THE_SPOT	1L
EXTERN لما	p_imst;		// 'imstyle'
#جے_انت
#جے_تعریف_شدہ FEAT_IMAGE
EXTERN char_u	*p_ipc;		// 'imageprotocol'
#جے_انت
#إذا defined(FEAT_EVAL)
EXTERN char_u	*p_imaf;	// 'imactivatefunc'
EXTERN char_u	*p_imsf;	// 'imstatusfunc'
#جے_انت
EXTERN صحيح	p_imcmdline;	// 'imcmdline'
EXTERN صحيح	p_imdisable;	// 'imdisable'
EXTERN لما	p_iminsert;	// 'iminsert'
EXTERN لما	p_imsearch;	// 'imsearch'
EXTERN صحيح	p_inf;		// 'infercase'
#إذا defined(FEAT_لبھو_ID) && defined(FEAT_EVAL)
EXTERN char_u	*p_inex;	// 'includeexpr'
#جے_انت
EXTERN صحيح	p_is;		// 'incsearch'
#إذا defined(FEAT_EVAL)
EXTERN char_u	*p_inde;	// 'indentexpr'
EXTERN char_u	*p_indk;	// 'indentkeys'
#جے_انت
EXTERN صحيح	p_im;		// 'insertmode'
EXTERN char_u	*p_isf;		// 'isfname'
EXTERN char_u	*p_isi;		// 'isident'
EXTERN char_u	*p_isk;		// 'iskeyword'
EXTERN char_u	*p_isp;		// 'isprint'
EXTERN صحيح	p_js;		// 'joinspaces'
#جے_تعریف_شدہ FEAT_CRYPT
EXTERN char_u	*p_کلید;		// 'key'
#جے_انت
#جے_تعریف_شدہ FEAT_KEYMAP
EXTERN char_u	*p_keymap;	// 'keymap'
#جے_انت
EXTERN char_u	*p_kp;		// 'keywordprg'
EXTERN char_u	*p_km;		// 'keymodel'
EXTERN char_u	*p_kpc;		// 'keyprotocol'
#جے_تعریف_شدہ FEAT_LANGMAP
EXTERN char_u	*p_langmap;	// 'langmap'
EXTERN صحيح	p_lnr;		// 'langnoremap'
EXTERN صحيح	p_lrm;		// 'langremap'
#جے_انت
#إذا defined(FEAT_MENU) && defined(FEAT_MULTI_LANG)
EXTERN char_u	*p_lm;		// 'langmenu'
#جے_انت
#جے_تعریف_شدہ FEAT_GUI
EXTERN لما	p_linespace;	// 'linespace'
#جے_انت
EXTERN صحيح	p_lisp;		// 'lisp'
EXTERN char_u	*p_lop;		// 'lispoptions'
EXTERN char_u	*p_lispwords;	// 'lispwords'
EXTERN لما	p_ls;		// 'laststatus'
EXTERN لما	p_stal;		// 'showtabline'
EXTERN char_u	*p_lcs;		// 'listchars'

EXTERN صحيح	p_lz;		// 'lazyredraw'
EXTERN صحيح	p_lpl;		// 'loadplugins'
#إذا defined(DYNAMIC_LUA)
EXTERN char_u	*p_luadll;	// 'luadll'
#جے_انت
EXTERN صحيح	p_magic;	// 'magic'
EXTERN char_u	*p_menc;	// 'makeencoding'
#جے_تعریف_شدہ FEAT_QUICKFIX
EXTERN char_u	*p_mef;		// 'makeef'
EXTERN char_u	*p_mp;		// 'makeprg'
#جے_انت
EXTERN char_u	*p_mps;		// 'matchpairs'
EXTERN لما	p_mat;		// 'matchtime'
EXTERN لما	p_mco;		// 'maxcombine'
#جے_تعریف_شدہ FEAT_EVAL
EXTERN لما	p_mfd;		// 'maxfuncdepth'
#جے_انت
EXTERN لما	p_mmd;		// 'maxmapdepth'
EXTERN لما	p_mm;		// 'maxmem'
EXTERN لما	p_mmp;		// 'maxmempattern'
EXTERN لما	p_mmt;		// 'maxmemtot'
#جے_تعریف_شدہ FEAT_MENU
EXTERN لما	p_mis;		// 'menuitems'
#جے_انت
EXTERN char_u	*p_mopt;	// 'messagesopt'
EXTERN لما	p_msc;		// 'maxsearchcount'
#جے_تعریف_شدہ FEAT_SPELL
EXTERN char_u	*p_msm;		// 'mkspellmem'
#جے_انت
EXTERN صحيح	p_ml;		// 'modeline'
EXTERN صحيح	p_mle;		// 'modelineexpr'
EXTERN لما	p_mls;		// 'modelines'
EXTERN صحيح	p_mlstr;	// 'modelinestrict'
EXTERN صحيح	p_ma;		// 'modifiable'
EXTERN صحيح	p_mod;		// 'modified'
EXTERN char_u	*p_mouse;	// 'mouse'
#جے_تعریف_شدہ FEAT_GUI
EXTERN صحيح	p_mousef;	// 'mousefocus'
EXTERN صحيح	p_mh;		// 'mousehide'
#جے_انت
EXTERN char_u	*p_mousem;	// 'mousemodel'
#جے_تعریف_شدہ FEAT_GUI
EXTERN صحيح	p_mousemev;	// 'mousemoveevent'
#جے_انت
EXTERN لما	p_mouset;	// 'mousetime'
EXTERN صحيح	p_ہور;		// 'more'
#جے_تعریف_شدہ FEAT_MZSCHEME
EXTERN لما	p_mzq;		// 'mzquantum
# إذا defined(DYNAMIC_MZSCHEME)
EXTERN char_u	*p_mzschemedll;	// 'mzschemedll'
EXTERN char_u	*p_mzschemegcdll; // 'mzschemegcdll'
# endif
#جے_انت
EXTERN char_u	*p_nf;		// 'nrformats'
#إذا defined(MSWIN)
EXTERN صحيح	p_odev;		// 'opendevice'
#جے_انت
EXTERN لما	p_ost;		// 'osctimeoutlen'
EXTERN char_u	*p_opfunc;	// 'operatorfunc'
EXTERN char_u	*p_para;	// 'paragraphs'
EXTERN صحيح	p_paste;	// 'paste'
EXTERN char_u	*p_pt;		// 'pastetoggle'
#إذا defined(FEAT_EVAL) && defined(FEAT_DIFF)
EXTERN char_u	*p_pex;		// 'patchexpr'
#جے_انت
EXTERN char_u	*p_pm;		// 'patchmode'
EXTERN char_u	*p_رستہ;	// 'path'
EXTERN char_u	*p_cdpath;	// 'cdpath'
#إذا defined(DYNAMIC_PERL)
EXTERN char_u	*p_perldll;	// 'perldll'
#جے_انت
EXTERN صحيح	p_pi;		// 'preserveindent'
#إذا defined(DYNAMIC_PYTHON3)
EXTERN char_u	*p_py3dll;	// 'pythonthreedll'
#جے_انت
#جے_تعریف_شدہ FEAT_PYTHON3
EXTERN char_u	*p_py3home;	// 'pythonthreehome'
#جے_انت
#إذا defined(DYNAMIC_PYTHON)
EXTERN char_u	*p_pydll;	// 'pythondll'
#جے_انت
#جے_تعریف_شدہ FEAT_PYTHON
EXTERN char_u	*p_pyhome;	// 'pythonhome'
#جے_انت
#إذا defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
EXTERN لما	p_pyx;		// 'pyxversion'
#جے_انت
EXTERN char_u	*p_qe;		// 'quoteescape'
EXTERN صحيح	p_ro;		// 'readonly'
#جے_تعریف_شدہ FEAT_RELTIME
EXTERN لما	p_rdt;		// 'redrawtime'
#جے_انت
EXTERN صحيح	p_remap;	// 'remap'
EXTERN لما	p_re;		// 'regexpengine'
#جے_تعریف_شدہ FEAT_RENDER_OPTIONS
EXTERN char_u	*p_rop;		// 'renderoptions'
#جے_انت
EXTERN لما	p_report;	// 'report'
#إذا defined(FEAT_QUICKFIX)
EXTERN لما	p_pvh;		// 'previewheight'
EXTERN لما     p_chi;          // 'chistory'
#جے_انت
#جے_تعریف_شدہ MSWIN
EXTERN صحيح	p_rs;		// 'restorescreen'
#جے_انت
#جے_تعریف_شدہ FEAT_RIGHTLEFT
EXTERN صحيح	p_ari;		// 'allowrevins'
EXTERN صحيح	p_ri;		// 'revins'
#جے_انت
#إذا defined(DYNAMIC_RUBY)
EXTERN char_u	*p_rubydll;	// 'rubydll'
#جے_انت
EXTERN صحيح	p_ru;		// 'ruler'
#جے_تعریف_شدہ FEAT_STL_OPT
EXTERN char_u	*p_ruf;		// 'rulerformat'
#جے_انت
EXTERN char_u	*p_pp;		// 'packpath'
#جے_تعریف_شدہ FEAT_QUICKFIX
EXTERN char_u	*p_qftf;	// 'quickfixtextfunc'
#جے_انت
EXTERN char_u	*p_rtp;		// 'runtimepath'
EXTERN لما	p_sj;		// 'scrolljump'
#إذا defined(MSWIN) && defined(FEAT_GUI)
EXTERN صحيح	p_scf;		// 'scrollfocus'
#جے_انت
EXTERN لما	p_so;		// 'scrolloff'
EXTERN لما	p_sop;		// 'scrolloffpad'
EXTERN char_u	*p_sbo;		// 'scrollopt'
EXTERN char_u	*p_sections;	// 'sections'
EXTERN صحيح	p_secure;	// 'secure'
EXTERN char_u	*p_sel;		// 'selection'
EXTERN char_u	*p_slm;		// 'selectmode'
#جے_تعریف_شدہ FEAT_SESSION
EXTERN char_u	*p_ssop;	// 'sessionoptions'
EXTERN بنانشانی	ssop_flags;
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
#جے_انت
EXTERN char_u	*p_sh;		// 'shell'
EXTERN char_u	*p_shcf;	// 'shellcmdflag'
#جے_تعریف_شدہ FEAT_QUICKFIX
EXTERN char_u	*p_sp;		// 'shellpipe'
#جے_انت
EXTERN char_u	*p_shq;		// 'shellquote'
EXTERN char_u	*p_sxq;		// 'shellxquote'
EXTERN char_u	*p_sxe;		// 'shellxescape'
EXTERN char_u	*p_srr;		// 'shellredir'
#جے_تعریف_شدہ AMIGA
EXTERN لما	p_st;		// 'shelltype'
#جے_انت
EXTERN صحيح	p_stmp;		// 'shelltemp'
#جے_تعریف_شدہ BACKSLASH_وچ_FILENAME
EXTERN صحيح	p_ssl;		// 'shellslash'
#جے_انت
#جے_تعریف_شدہ FEAT_STL_OPT
EXTERN char_u	*p_stl;		// 'statusline'
EXTERN char_u	*p_stlo;	// 'statuslineopt'
#جے_انت
EXTERN صحيح	p_sr;		// 'shiftround'
EXTERN لما	p_sw;		// 'shiftwidth'
EXTERN char_u	*p_shm;		// 'shortmess'
EXTERN صحيح	p_sn;		// 'shortname'
#جے_تعریف_شدہ FEAT_LINEBREAK
EXTERN char_u	*p_sbr;		// 'showbreak'
#جے_انت
EXTERN صحيح	p_sc;		// 'showcmd'
EXTERN char_u	*p_sloc;	// 'showcmdloc'
EXTERN صحيح	p_sft;		// 'showfulltag'
EXTERN صحيح	p_sm;		// 'showmatch'
EXTERN صحيح	p_smd;		// 'showmode'
EXTERN لما	p_ss;		// 'sidescroll'
EXTERN لما	p_siso;		// 'sidescrolloff'
EXTERN صحيح	p_scs;		// 'smartcase'
EXTERN صحيح	p_si;		// 'smartindent'
EXTERN صحيح	p_sta;		// 'smarttab'
EXTERN لما	p_sts;		// 'softtabstop'
EXTERN صحيح	p_sb;		// 'splitbelow'
EXTERN char_u	*p_sua;		// 'suffixesadd'
EXTERN صحيح	p_swf;		// 'swapfile'
#جے_تعریف_شدہ FEAT_SYN_HL
EXTERN لما	p_smc;		// 'synmaxcol'
#جے_انت
EXTERN لما	p_tpm;		// 'tabpagemax'
#جے_تعریف_شدہ FEAT_STL_OPT
EXTERN char_u	*p_tal;		// 'tabline'
#جے_انت
#جے_تعریف_شدہ FEAT_EVAL
EXTERN char_u	*p_tfu;		// 'tagfunc'
#جے_انت
#جے_تعریف_شدہ FEAT_SPELL
EXTERN char_u	*p_spc;		// 'spellcapcheck'
EXTERN char_u	*p_spf;		// 'spellfile'
EXTERN char_u	*p_spl;		// 'spelllang'
EXTERN char_u	*p_spo;		// 'spelloptions'
EXTERN char_u	*p_sps;		// 'spellsuggest'
#جے_انت
EXTERN صحيح	p_spr;		// 'splitright'
EXTERN صحيح	p_sol;		// 'startofline'
EXTERN char_u	*p_su;		// 'suffixes'
EXTERN char_u	*p_sws;		// 'swapsync'
EXTERN char_u	*p_swb;		// 'switchbuf'
EXTERN بنانشانی	swb_flags;
// Keep in sync with p_swb_values in optionstr.c
#تعریف_کرو SWB_USEOPEN		0x001
#تعریف_کرو SWB_USETAB		0x002
#تعریف_کرو SWB_ونڈو		0x004
#تعریف_کرو SWB_NEWTAB		0x008
#تعریف_کرو SWB_VSPLIT		0x010
#تعریف_کرو SWB_USELAST		0x020

EXTERN char_u	*p_spk;		// 'splitkeep'

#إذا defined(FEAT_TABPANEL)
EXTERN char_u	*p_tpl;		// 'tabpanel'
EXTERN لما	p_stpl;		// 'showtabpanel'
EXTERN char_u	*p_tplo;	// 'tabpanelopt'
#جے_انت

#جے_تعریف_شدہ FEAT_SYN_HL
EXTERN char_u	*p_syn;		// 'syntax'
#جے_انت
EXTERN char_u	*p_tcl;		// 'tabclose'
EXTERN بنانشانی	tcl_flags;	// flags from 'tabclose'
#تعریف_کرو TCL_بایاں		0x001
#تعریف_کرو TCL_USELAST		0x002
EXTERN لما	p_ts;		// 'tabstop'
EXTERN صحيح	p_tbs;		// 'tagbsearch'
EXTERN char_u	*p_tc;		// 'tagcase'
EXTERN بنانشانی tc_flags;       // flags from 'tagcase'
#تعریف_کرو TC_FOLLOWIC		0x01
#تعریف_کرو TC_IGNORE		0x02
#تعریف_کرو TC_میل		0x04
#تعریف_کرو TC_FOLLOWSCS		0x08
#تعریف_کرو TC_SMART		0x10
EXTERN لما	p_tl;		// 'taglength'
EXTERN صحيح	p_tr;		// 'tagrelative'
EXTERN char_u	*p_ٹیگ_سوچی;	// 'tags'
EXTERN صحيح	p_tagsecure;    // 'tagsecure'
EXTERN صحيح	p_tgst;		// 'tagstack'
#إذا defined(DYNAMIC_TCL)
EXTERN char_u	*p_tcldll;	// 'tcldll'
#جے_انت
#جے_تعریف_شدہ FEAT_ARABIC
EXTERN صحيح	p_tbidi;	// 'termbidi'
#جے_انت
EXTERN char_u	*p_tenc;	// 'termencoding'
#جے_تعریف_شدہ FEAT_TERMGUICOLORS
EXTERN صحيح	p_tgc;		// 'termguicolors'
#جے_انت
#جے_تعریف_شدہ UNIX
EXTERN char_u	*p_trz;		// 'termresize'
#جے_انت
EXTERN صحيح	p_tsy;		// 'termsync'
#جے_تعریف_شدہ FEAT_TERMINAL
EXTERN لما	p_twsl;		// 'termwinscroll'
#جے_انت
#إذا defined(MSWIN) && defined(FEAT_TERMINAL)
EXTERN char_u	*p_twt;		// 'termwintype'
#جے_انت
EXTERN صحيح	p_terse;	// 'terse'
EXTERN صحيح	p_ta;		// 'textauto'
EXTERN صحيح	p_tx;		// 'textmode'
EXTERN لما	p_tw;		// 'textwidth'
EXTERN صحيح	p_منزل;		// 'tildeop'
EXTERN صحيح	p_timeout;	// 'timeout'
EXTERN لما	p_tm;		// 'timeoutlen'
EXTERN صحيح	p_title;	// 'title'
EXTERN لما	p_titlelen;	// 'titlelen'
EXTERN char_u	*p_titleold;	// 'titleold'
EXTERN char_u	*p_titlestring;	// 'titlestring'
EXTERN char_u	*p_tsr;		// 'thesaurus'
EXTERN صحيح	p_ttimeout;	// 'ttimeout'
EXTERN لما	p_ttm;		// 'ttimeoutlen'
EXTERN صحيح	p_tbi;		// 'ttybuiltin'
EXTERN صحيح	p_tf;		// 'ttyfast'
#إذا defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
EXTERN char_u	*p_toolbar;	// 'toolbar'
EXTERN بنانشانی toolbar_flags;
# define TOOLBAR_متن		0x01
# define TOOLBAR_ICONS		0x02
# define TOOLBAR_TOOLTIPS	0x04
# define TOOLBAR_HORIZ		0x08
#جے_انت
#إذا defined(FEAT_TOOLBAR) && defined(FEAT_GUI_GTK)
EXTERN char_u	*p_tbis;	// 'toolbariconsize'
EXTERN بنانشانی tbis_flags;
# define TBIS_TINY		0x01
# define TBIS_SMALL		0x02
# define TBIS_MEDIUM		0x04
# define TBIS_LARGE		0x08
# define TBIS_HUGE		0x10
# define TBIS_GIANT		0x20
#جے_انت
EXTERN لما	p_ttyscroll;	// 'ttyscroll'
#إذا defined(UNIX) || defined(VMS)
EXTERN char_u	*p_ttym;	// 'ttymouse'
EXTERN بنانشانی ttym_flags;
# define TTYM_XTERM		0x01
# define TTYM_XTERM2		0x02
# define TTYM_DEC		0x04
# define TTYM_NETTERM		0x08
# define TTYM_JSBTERM		0x10
# define TTYM_PTERM		0x20
# define TTYM_URXVT		0x40
# define TTYM_SGR		0x80
#جے_انت
#جے_تعریف_شدہ FEAT_PERSISTENT_UNDO
EXTERN char_u	*p_udir;	// 'undodir'
EXTERN صحيح	p_udf;		// 'undofile'
#جے_انت
EXTERN لما	p_ul;		// 'undolevels'
EXTERN لما	p_ur;		// 'undoreload'
EXTERN لما	p_uc;		// 'updatecount'
EXTERN لما	p_ut;		// 'updatetime'
#جے_تعریف_شدہ FEAT_VARTABS
EXTERN char_u	*p_vsts;	// 'varsofttabstop'
EXTERN char_u	*p_vts;		// 'vartabstop'
#جے_انت
#جے_تعریف_شدہ FEAT_VIMINFO
EXTERN char_u	*p_viminfo;	// 'viminfo'
EXTERN char_u	*p_viminfofile;	// 'viminfofile'
#جے_انت
#جے_تعریف_شدہ FEAT_SESSION
EXTERN char_u	*p_vdir;	// 'viewdir'
EXTERN char_u	*p_vop;		// 'viewoptions'
EXTERN بنانشانی	vop_flags;	// uses SSOP_ flags
#جے_انت
EXTERN صحيح	p_vb;		// 'visualbell'
EXTERN char_u	*p_ve;		// 'virtualedit'
EXTERN بنانشانی ve_flags;
#تعریف_کرو VE_بلاک	5	// includes "all"
#تعریف_کرو VE_داخل_کرو	6	// includes "all"
#تعریف_کرو VE_ALL		4
#تعریف_کرو VE_ONEMORE	8
#تعریف_کرو VE_NONE		16	// "none"
#تعریف_کرو VE_NONEU	32      // "NONE"
EXTERN لما	p_verbose;	// 'verbose'
#جے_تعریف_شدہ وچ_OPTION_C
char_u	*p_vfile = (char_u *)""; // used before options are initialized
#وإلا
خارجي char_u	*p_vfile;	// 'verbosefile'
#جے_انت
EXTERN صحيح	p_warn;		// 'warn'
EXTERN char_u	*p_wop;		// 'wildoptions'
EXTERN لما	p_کھڑکی;	// 'window'
#إذا defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_MOTIF) || defined(LINT) \
	|| defined (FEAT_GUI_GTK) || defined(FEAT_GUI_PHOTON)
# define FEAT_WAK
EXTERN char_u	*p_wak;		// 'winaltkeys'
#جے_انت
EXTERN char_u	*p_wig;		// 'wildignore'
EXTERN صحيح	p_wiv;		// 'weirdinvert'
EXTERN char_u	*p_ww;		// 'whichwrap'
EXTERN لما	p_wc;		// 'wildchar'
EXTERN لما	p_wcm;		// 'wildcharm'
EXTERN صحيح	p_wic;		// 'wildignorecase'
EXTERN char_u	*p_wim;		// 'wildmode'
EXTERN صحيح	p_wmnu;		// 'wildmenu'
EXTERN لما	p_wh;		// 'winheight'
EXTERN لما	p_wmh;		// 'winminheight'
EXTERN لما	p_wmw;		// 'winminwidth'
EXTERN لما	p_wiw;		// 'winwidth'
#جے_تعریف_شدہ FEAT_WAYLAND
EXTERN char_u	*p_wse;		// 'wlseat'
EXTERN لما     p_wtm;		// 'wltimeoutlen'
#جے_انت
#إذا defined(MSWIN) && defined(FEAT_TERMINAL)
EXTERN char_u	*p_winptydll;	// 'winptydll'
#جے_انت
EXTERN لما	p_wm;		// 'wrapmargin'
EXTERN صحيح	p_ws;		// 'wrapscan'
EXTERN صحيح	p_write;	// 'write'
EXTERN صحيح	p_wa;		// 'writeany'
EXTERN صحيح	p_wb;		// 'writebackup'
EXTERN لما	p_wd;		// 'writedelay'
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
#جے_تعریف_شدہ FEAT_QUICKFIX
    , BV_EFM
    , BV_GEFM
    , BV_GP
    , BV_MP
#جے_انت
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
#جے_تعریف_شدہ FEAT_DIFF
    , BV_DIA
#جے_انت
    , BV_TSR
#جے_تعریف_شدہ BACKSLASH_وچ_FILENAME
    , BV_CSL
#جے_انت
#جے_تعریف_شدہ FEAT_COMPL_FUNC
    , BV_CFU
#جے_انت
#جے_تعریف_شدہ FEAT_لبھو_ID
    , BV_DEF
    , BV_INC
#جے_انت
    , BV_EOF
    , BV_EOL
    , BV_FIXEOL
    , BV_EP
    , BV_ET
    , BV_FENC
    , BV_FP
#جے_تعریف_شدہ FEAT_EVAL
    , BV_BEXPR
    , BV_FEX
    , BV_FFU
#جے_انت
    , BV_FF
    , BV_FLP
    , BV_FO
#جے_تعریف_شدہ HAVE_FSYNC
    , BV_FS
#جے_انت
    , BV_FT
    , BV_IMI
    , BV_IMS
#إذا defined(FEAT_EVAL)
    , BV_INDE
    , BV_INDK
#جے_انت
#إذا defined(FEAT_لبھو_ID) && defined(FEAT_EVAL)
    , BV_INEX
#جے_انت
    , BV_INF
    , BV_ISE
    , BV_ISK
#جے_تعریف_شدہ FEAT_CRYPT
    , BV_کلید
#جے_انت
#جے_تعریف_شدہ FEAT_KEYMAP
    , BV_KMAP
#جے_انت
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
#جے_تعریف_شدہ FEAT_COMPL_FUNC
    , BV_OFU
#جے_انت
    , BV_رستہ
    , BV_PI
    , BV_QE
    , BV_RO
    , BV_SI
    , BV_SN
#جے_تعریف_شدہ FEAT_SYN_HL
    , BV_SMC
    , BV_SYN
#جے_انت
#جے_تعریف_شدہ FEAT_SPELL
    , BV_SPC
    , BV_SPF
    , BV_SPL
    , BV_SPO
#جے_انت
    , BV_STS
    , BV_SUA
    , BV_SW
    , BV_SWF
#جے_تعریف_شدہ FEAT_EVAL
    , BV_TFU
#جے_انت
    , BV_ٹیگ_سوچی
    , BV_TC
#جے_تعریف_شدہ FEAT_COMPL_FUNC
    , BV_TSRFU
#جے_انت
    , BV_TS
    , BV_TW
    , BV_TX
    , BV_UDF
    , BV_UL
    , BV_WM
#جے_تعریف_شدہ FEAT_TERMINAL
    , BV_TWSL
#جے_انت
#جے_تعریف_شدہ FEAT_VARTABS
    , BV_VSTS
    , BV_VTS
#جے_انت
    , BV_گنتی	    // must be the last one
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
#جے_تعریف_شدہ FEAT_ARABIC
    , WV_ARAB
#جے_انت
#جے_تعریف_شدہ FEAT_CONCEAL
    , WV_COCU
    , WV_COLE
#جے_انت
#جے_تعریف_شدہ FEAT_TERMINAL
    , WV_TWK
    , WV_TWS
#جے_انت
    , WV_CRBIND
#جے_تعریف_شدہ FEAT_LINEBREAK
    , WV_BRI
    , WV_BRIOPT
#جے_انت
    , WV_WCR
#جے_تعریف_شدہ FEAT_DIFF
    , WV_DIFF
#جے_انت
    , WV_EIW
#جے_تعریف_شدہ FEAT_FOLDING
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
#جے_انت
#جے_تعریف_شدہ FEAT_LINEBREAK
    , WV_LBR
#جے_انت
    , WV_NU
    , WV_RNU
    , WV_VE
#جے_تعریف_شدہ FEAT_LINEBREAK
    , WV_NUW
#جے_انت
#إذا defined(FEAT_QUICKFIX)
    , WV_PVW
    , WV_LHI
#جے_انت
#جے_تعریف_شدہ FEAT_RIGHTLEFT
    , WV_RL
    , WV_RLC
#جے_انت
    , WV_SCBIND
    , WV_SCROLL
    , WV_SMS
    , WV_SISO
    , WV_SO
    , WV_SOP
#جے_تعریف_شدہ FEAT_SPELL
    , WV_SPELL
#جے_انت
#جے_تعریف_شدہ FEAT_SYN_HL
    , WV_CUC
    , WV_CUL
    , WV_CULOPT
    , WV_CC
#جے_انت
#جے_تعریف_شدہ FEAT_LINEBREAK
    , WV_SBR
#جے_انت
#جے_تعریف_شدہ FEAT_STL_OPT
    , WV_STL
    , WV_STLO
#جے_انت
    , WV_WFB
    , WV_WFH
    , WV_WFW
    , WV_WHL
    , WV_WRAP
#جے_تعریف_شدہ FEAT_SIGNS
    , WV_SCL
#جے_انت
    , WV_گنتی	    // must be the last one
};

// Value for b_p_ul indicating the global value must be used.
#تعریف_کرو NO_LOCAL_UNDOLEVEL (-123456)

#تعریف_کرو ERR_BUFLEN 80

#جے_انت // _OPTION_H_
