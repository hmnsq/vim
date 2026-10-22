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

#నిర్వచితం_కాకపోతే _OPTION_H_
#నిర్వచించు _OPTION_H_

//
// Option Flags
//
#నిర్వచించు P_BOOL		0x01	// the option is boolean
#నిర్వచించు P_అంకె		0x02	// the option is numeric
#నిర్వచించు P_STRING	0x04	// the option is a string
#నిర్వచించు P_ALLOCED	0x08	// the string option is in allocated memory,
				// must use free_string_option() when
				// assigning new value. Not set if default is
				// the same.
#నిర్వచించు P_EXPAND	0x10	// environment expansion.  NOTE: P_EXPAND can
				// never be used for local or hidden options!
#నిర్వచించు P_NO_CMD_EXPAND	0x20	// don't perform cmdline completions
#నిర్వచించు P_NODEFAULT	0x40	// don't set to default value
#నిర్వచించు P_DEF_ALLOCED	0x80	// default value is in allocated memory, must
				//  use vim_free() when assigning new value
#నిర్వచించు P_WAS_సమితి	0x100	// option has been set/reset
#నిర్వచించు P_NO_MKRC	0x200	// don't include in :mkvimrc output
#నిర్వచించు P_VI_DEF	0x400	// Use Vi default for Vim
#నిర్వచించు P_VIM		0x800	// Vim option, reset when 'cp' set

				// when option changed, what to display:
#నిర్వచించు P_RSTAT		0x1000	// redraw status lines
#నిర్వచించు P_RWIN		0x2000	// redraw current window and recompute text
#నిర్వచించు P_RBUF		0x4000	// redraw current buffer and recompute text
#నిర్వచించు P_RALL		0x6000	// redraw all windows and recompute text
#నిర్వచించు P_RCLR		0x7000	// clear and redraw all and recompute text

#నిర్వచించు P_COMMA		 0x8000	 // comma separated list
#నిర్వచించు P_ONECOMMA	0x18000L // P_COMMA and cannot have two consecutive
				 // commas
#నిర్వచించు P_NODUP		0x20000L // don't allow duplicate strings
#నిర్వచించు P_FLAGLIST	0x40000L // list of single-char flags

#నిర్వచించు P_SECURE	0x80000L // cannot change in modeline or secure mode
#నిర్వచించు P_GETTEXT      0x100000L // expand default value with _()
#నిర్వచించు P_NOGLOB       0x200000L // do not use local value for global vimrc
#నిర్వచించు P_NFNAME       0x400000L // only normal file name chars allowed
#నిర్వచించు P_INSECURE     0x800000L // option was set from a modeline
#నిర్వచించు P_PRI_MKRC    0x1000000L // priority for :mkvimrc (setting option has
				 // side effects)
#నిర్వచించు P_NO_ML       0x2000000L // not allowed in modeline
#నిర్వచించు P_CURSWANT    0x4000000L // update curswant required; not needed when
				 // there is a redraw flag
#నిర్వచించు P_NDNAME      0x8000000L // only normal dir name chars allowed
#నిర్వచించు P_HLONLY     0x10000000L // option only changes highlight, not text
#నిర్వచించు P_MLE	     0x20000000L // under control of 'modelineexpr'
#నిర్వచించు P_FUNC	     0x40000000L // accept a function reference or a lambda
#నిర్వచించు P_COLON	     0x80000000L // values use colons to create sublists
// Warning: Currently we have used all 32 bits for option flags. On some 32-bit
//          systems, the flags are stored as a 32-bit integer, and adding more
//          flags will overflow it. Adding another flag will need to change how
//          it's stored first.

// Returned by get_option_value().
రకనిర్వచనం గణన {
    gov_unknown,
    gov_bool,
    gov_సంఖ్య,
    gov_string,
    gov_hidden_bool,
    gov_hidden_సంఖ్య,
    gov_hidden_string
} getoption_T;

/*
 * Default values for 'errorformat'.
 * The "%f|%l| %m" one is used for when the contents of the quickfix window is
 * written to a file.
 */
#నిర్వచితమైతే AMIGA
# define DFLT_EFM	"%f>%l:%c:%t:%n:%m,%f:%l: %t%*\\D%n: %m,%f %l %t%*\\D%n: %m,%*[^\"]\"%f\"%*\\D%l: %m,%f:%l:%m,%f|%l| %m"
#లేకపోతే
# ఒకవేళ defined(MSWIN)
#  define DFLT_EFM	"%f(%l): %t%*\\D%n: %m,%f(%l\\,%c): %t%*\\D%n: %m,%f(%l) \\=: %t%*\\D%n: %m,%*[^\"]\"%f\"%*\\D%l: %m,%f(%l) \\=: %m,%*[^ ] %f %l: %m,%f:%l:%c:%m,%f(%l):%m,%f:%l:%m,%f|%l| %m"
# లేకపోతే
#  ఒకవేళ defined(__QNX__)
#   define DFLT_EFM	"%f(%l):%*[^WE]%t%*\\D%n:%m,%f|%l| %m"
#  లేకపోతే
#   ifdef VMS
#    define DFLT_EFM	"%A%p^,%C%%CC-%t-%m,%Cat line number %l in file %f,%f|%l| %m"
#   లేకపోతే // Unix, probably
#    define DFLT_EFM	"%*[^\"]\"%f\"%*\\D%l: %m,\"%f\"%*\\D%l: %m,%-Gg%\\?make[%*\\d]: *** [%f:%l:%m,%-Gg%\\?make: *** [%f:%l:%m,%-G%f:%l: (Each undeclared identifier is reported only once,%-G%f:%l: for each function it appears in.),%-GIn file included from %f:%l:%c:,%-GIn file included from %f:%l:%c\\,,%-GIn file included from %f:%l:%c,%-GIn file included from %f:%l,%-G%*[ ]from %f:%l:%c,%-G%*[ ]from %f:%l:,%-G%*[ ]from %f:%l\\,,%-G%*[ ]from %f:%l,%f:%l:%c:%m,%f(%l):%m,%f:%l:%m,\"%f\"\\, line %l%*\\D%c%*[^ ] %m,%D%*\\a[%*\\d]: Entering directory %*[`']%f',%X%*\\a[%*\\d]: Leaving directory %*[`']%f',%D%*\\a: Entering directory %*[`']%f',%X%*\\a: Leaving directory %*[`']%f',%DMaking %*\\a in %f,%f|%l| %m"
#   endif
#  endif
# endif
#అయితే_ముగింపు

#నిర్వచించు DFLT_GREPFORMAT	"%f:%l:%m,%f:%l%m,%f  %l%m"

// default values for b_p_ff 'fileformat' and p_ffs 'fileformats'
#నిర్వచించు FF_DOS		"dos"
#నిర్వచించు FF_MAC		"mac"
#నిర్వచించు FF_UNIX		"unix"

#నిర్వచితమైతే USE_CRNL
# define DFLT_FF	"dos"
# define DFLT_FFS_VIM	"dos,unix"
# define DFLT_FFS_VI	"dos,unix"	// also autodetect in compatible mode
# define DFLT_TEXTAUTO	TRUE
#లేకపోతే
# define DFLT_FF	"unix"
# define DFLT_FFS_VIM	"unix,dos"
# ifdef __CYGWIN__
#  define DFLT_FFS_VI	"unix,dos"	// Cygwin always needs file detection
#  define DFLT_TEXTAUTO TRUE
# లేకపోతే
#  define DFLT_FFS_VI	""
#  define DFLT_TEXTAUTO FALSE
# endif
#అయితే_ముగింపు


// Possible values for 'encoding'
#నిర్వచించు ENC_UCSBOM	"ucs-bom"	// check for BOM at start of file

// default value for 'encoding'
#ఒకవేళ defined(MSWIN) || defined(__MVS__)
# define ENC_DFLT	"utf-8"
#లేకపోతే
# define ENC_DFLT	"latin1"
#అయితే_ముగింపు

// end-of-line style
#నిర్వచించు EOL_UNKNOWN	(-1)	// not defined yet
#నిర్వచించు EOL_UNIX	0	// NL
#నిర్వచించు EOL_DOS		1	// CR NL
#నిర్వచించు EOL_MAC		2	// CR

// Formatting options for p_fo 'formatoptions'
#నిర్వచించు FO_WRAP		't'
#నిర్వచించు FO_WRAP_COMS	'c'
#నిర్వచించు FO_RET_COMS	'r'
#నిర్వచించు FO_OPEN_COMS	'o'
#నిర్వచించు FO_NO_OPEN_COMS	'/'
#నిర్వచించు FO_Q_COMS	'q'
#నిర్వచించు FO_Q_సంఖ్య	'n'
#నిర్వచించు FO_Q_రెండవ	'2'
#నిర్వచించు FO_INS_VI	'v'
#నిర్వచించు FO_INS_LONG	'l'
#నిర్వచించు FO_INS_BLANK	'b'
#నిర్వచించు FO_MBYTE_BREAK	'm'	// break before/after multi-byte char
#నిర్వచించు FO_MBYTE_JOIN	'M'	// no space before/after multi-byte char
#నిర్వచించు FO_MBYTE_JOIN2	'B'	// no space between multi-byte chars
#నిర్వచించు FO_ONE_LETTER	'1'
#నిర్వచించు FO_WHITE_PAR	'w'	// trailing white space continues paragr.
#నిర్వచించు FO_AUTO		'a'	// automatic formatting
#నిర్వచించు FO_RIGOROUS_TW	']'     // respect textwidth rigorously
#నిర్వచించు FO_REMOVE_COMS	'j'	// remove comment leaders when joining lines
#నిర్వచించు FO_PERIOD_ABBR	'p'	// don't break a single space after a period

#నిర్వచించు DFLT_FO_VI	"vt"
#నిర్వచించు DFLT_FO_VIM	"tcq"
#నిర్వచించు FO_ALL		"tcro/q2vlb1mMBn,aw]jp"	// for do_set()

// characters for the p_cpo option:
#నిర్వచించు CPO_ALTREAD	'a'	// ":read" sets alternate file name
#నిర్వచించు CPO_ALTWRITE	'A'	// ":write" sets alternate file name
#నిర్వచించు CPO_BAR		'b'	// "\|" ends a mapping
#నిర్వచించు CPO_BSLASH	'B'	// backslash in mapping is not special
#నిర్వచించు CPO_శోధన	'c'
#నిర్వచించు CPO_CONCAT	'C'	// Don't concatenate sourced lines
#నిర్వచించు CPO_DOTTAG	'd'	// "./tags" in 'tags' is in current dir
#నిర్వచించు CPO_DIGRAPH	'D'	// No digraph after "r", "f", etc.
#నిర్వచించు CPO_EXECBUF	'e'
#నిర్వచించు CPO_EMPTYREGION	'E'	// operating on empty region is an error
#నిర్వచించు CPO_FNAMER	'f'	// set file name for ":r file"
#నిర్వచించు CPO_FNAMEW	'F'	// set file name for ":w file"
#నిర్వచించు CPO_GOTO1	'g'	// goto line 1 for ":edit"
#నిర్వచించు CPO_INSEND	'H'	// "I" inserts before last blank in line
#నిర్వచించు CPO_INTMOD	'i'	// interrupt a read makes buffer modified
#నిర్వచించు CPO_INDENT	'I'	// remove auto-indent more often
#నిర్వచించు CPO_JOINSP	'j'	// only use two spaces for join after '.'
#నిర్వచించు CPO_ENDOFSENT	'J'	// need two spaces to detect end of sentence
#నిర్వచించు CPO_KEYCODE	'k'	// don't recognize raw key code in mappings
#నిర్వచించు CPO_KOFFSET	'K'	// don't wait for key code in mappings
#నిర్వచించు CPO_LITERAL	'l'	// take char after backslash in [] literal
#నిర్వచించు CPO_LISTWM	'L'	// 'list' changes wrapmargin
#నిర్వచించు CPO_SHOWMATCH	'm'
#నిర్వచించు CPO_MATCHBSL	'M'	// "%" ignores use of backslashes
#నిర్వచించు CPO_NUMCOL	'n'	// 'number' column also used for text
#నిర్వచించు CPO_LINEOFF	'o'
#నిర్వచించు CPO_OVERNEW	'O'	// silently overwrite new file
#నిర్వచించు CPO_LISP	'p'	// 'lisp' indenting
#నిర్వచించు CPO_FNAMEAPP	'P'	// set file name for ":w >>file"
#నిర్వచించు CPO_JOINCOL	'q'	// with "3J" use column after first join
#నిర్వచించు CPO_REDO	'r'
#నిర్వచించు CPO_REMMARK	'R'	// remove marks when filtering
#నిర్వచించు CPO_BUFOPT	's'
#నిర్వచించు CPO_BUFOPTGLOB	'S'
#నిర్వచించు CPO_TAGPAT	't'	// tag pattern is used for "n"
#నిర్వచించు CPO_UNDO	'u'	// "u" undoes itself
#నిర్వచించు CPO_BACKSPACE	'v'	// "v" keep deleted text
#నిర్వచించు CPO_CW		'w'	// "cw" only changes one blank
#నిర్వచించు CPO_FWRITE	'W'	// "w!" doesn't overwrite readonly files
#నిర్వచించు CPO_ESC		'x'
#నిర్వచించు CPO_REPLCNT	'X'	// "R" with a count only deletes chars once
#నిర్వచించు CPO_YANK	'y'
#నిర్వచించు CPO_KEEPRO	'Z'	// don't reset 'readonly' on ":w!"
#నిర్వచించు CPO_మాట	'z'	// do not special-case word motions cw and dw
#నిర్వచించు CPO_DOLLAR	'$'
#నిర్వచించు CPO_FILTER	'!'
#నిర్వచించు CPO_సరిపోలిక	'%'
#నిర్వచించు CPO_నక్షత్రం	'*'	// ":*" means ":@"
#నిర్వచించు CPO_PLUS	'+'	// ":write file" resets 'modified'
#నిర్వచించు CPO_MINUS	'-'	// "9-" fails at and before line 9
#నిర్వచించు CPO_SPECI	'<'	// don't recognize <> in mappings
#నిర్వచించు CPO_REGAPPEND	'>'	// insert NL when appending to a register
// POSIX flags
#నిర్వచించు CPO_HASH	'#'	// "D", "o" and "O" do not use a count
#నిర్వచించు CPO_PARA	'{'	// "{" is also a paragraph boundary
#నిర్వచించు CPO_TSIZE	'|'	// $LINES and $COLUMNS overrule term size
#నిర్వచించు CPO_PRESERVE	'&'	// keep swap file after :preserve
#నిర్వచించు CPO_SUBPERCENT	'/'	// % in :s string uses previous one
#నిర్వచించు CPO_BACKSL	'\\'	// \ is not special in []
#నిర్వచించు CPO_CHDIR	'.'	// don't chdir if buffer is modified
#నిర్వచించు CPO_SCOLON	';'	// using "," and ";" will skip over char if
				// cursor would not move
#నిర్వచించు CPO_NOSYMLINKS	'~'	// don't resolve symlinks when changing directory
// default values for Vim, Vi and POSIX
#నిర్వచించు CPO_VIM		"aABceFsz"
#నిర్వచించు CPO_VI		"aAbBcCdDeEfFgHiIjJkKlLmMnoOpPqrRsStuvwWxXyZz$!%*-+<>;"
#నిర్వచించు CPO_ALL		"aAbBcCdDeEfFgHiIjJkKlLmMnoOpPqrRsStuvwWxXyZz$!%*-+<>#{|&/\\.;~"

// characters for p_ww option:
#నిర్వచించు WW_ALL		"bshl<>[]~"

// characters for p_mouse option:
#నిర్వచించు MOUSE_సాధారణ	'n'		// use mouse in Normal mode
#నిర్వచించు MOUSE_VISUAL	'v'		// use mouse in Visual/Select mode
#నిర్వచించు MOUSE_చొప్పించు	'i'		// use mouse in Insert mode
#నిర్వచించు MOUSE_COMMAND	'c'		// use mouse in Command-line mode
#నిర్వచించు MOUSE_HELP	'h'		// use mouse in help buffers
#నిర్వచించు MOUSE_RETURN	'r'		// use mouse for hit-return message
#నిర్వచించు MOUSE_A		"nvich"		// used for 'a' flag
#నిర్వచించు MOUSE_ALL	"anvichr"	// all possible characters
#నిర్వచించు MOUSE_NONE	' '		// don't use Visual selection
#నిర్వచించు MOUSE_NONEF	'x'		// forced modeless selection

#నిర్వచించు COCU_ALL	"nvic"		// flags for 'concealcursor'

// characters for p_shm option:
#నిర్వచించు SHM_RO		'r'		// readonly
#నిర్వచించు SHM_MOD		'm'		// modified
#నిర్వచించు SHM_FILE	'f'		// (file 1 of 2)
#నిర్వచించు SHM_చివరి	'i'		// last line incomplete
#నిర్వచించు SHM_పాఠ్యం	'x'		// tx instead of textmode
#నిర్వచించు SHM_పంక్తులు	'l'		// "L" instead of "lines"
#నిర్వచించు SHM_NEW		'n'		// "[New]" instead of "[New file]"
#నిర్వచించు SHM_WRI		'w'		// "[w]" instead of "written"
#నిర్వచించు SHM_A		"rmfixlnw"	// represented by 'a' flag
#నిర్వచించు SHM_WRITE	'W'		// don't use "written" at all
#నిర్వచించు SHM_TRUNC	't'		// truncate file messages
#నిర్వచించు SHM_TRUNCALL	'T'		// truncate all messages
#నిర్వచించు SHM_OVER	'o'		// overwrite file messages
#నిర్వచించు SHM_OVERALL	'O'		// overwrite more messages
#నిర్వచించు SHM_శోధన	's'		// no search hit bottom messages
#నిర్వచించు SHM_ATTENTION	'A'		// no ATTENTION messages
#నిర్వచించు SHM_INTRO	'I'		// intro messages
#నిర్వచించు SHM_COMPLETIONMENU  'c'		// completion menu messages
#నిర్వచించు SHM_COMPLETIONSCAN  'C'		// completion scanning messages
#నిర్వచించు SHM_RECORDING	'q'		// short recording message
#నిర్వచించు SHM_FILEINFO	'F'		// no file info messages
#నిర్వచించు SHM_SEARCHCOUNT  'S'		// no search stats: '[1/10]'
#నిర్వచించు SHM_UNDO	'u'		// undo messages
#నిర్వచించు SHM_POSIX       "AS"		// POSIX value
#నిర్వచించు SHM_ALL		"rmfixlnwaWtToOsAIcCqFSu" // all possible flags for 'shm'
#నిర్వచించు SHM_LEN		30		// max length of all flags together
					// plus a NUL character

// characters for p_go:
#నిర్వచించు GO_TERMINAL	'!'		// use terminal for system commands
#నిర్వచించు GO_ASEL		'a'		// autoselect
#నిర్వచించు GO_ASELML	'A'		// autoselect modeless selection
#నిర్వచించు GO_BOT		'b'		// use bottom scrollbar
#నిర్వచించు GO_CONDIALOG	'c'		// use console dialog
#నిర్వచించు GO_TITLEBAR	'C'		// use 'hl-TitleBar'
#నిర్వచించు GO_DARKTHEME	'd'		// use dark theme variant
#నిర్వచించు GO_TABLINE	'e'		// may show tabline
#నిర్వచించు GO_FORG		'f'		// start GUI in foreground
#నిర్వచించు GO_GREY		'g'		// use grey menu items
#నిర్వచించు GO_HORSCROLL	'h'		// flexible horizontal scrolling
#నిర్వచించు GO_ICON		'i'		// use Vim icon
#నిర్వచించు GO_ఎడమ		'l'		// use left scrollbar
#నిర్వచించు GO_VLEFT	'L'		// left scrollbar with vert split
#నిర్వచించు GO_MENUS	'm'		// use menu bar
#నిర్వచించు GO_NOSYSMENU	'M'		// don't source system menu
#నిర్వచించు GO_సూచిక	'p'		// pointer enter/leave callbacks
#నిర్వచించు GO_ASELPLUS	'P'		// autoselectPlus
#నిర్వచించు GO_కుడి	'r'		// use right scrollbar
#నిర్వచించు GO_VRIGHT	'R'		// right scrollbar with vert split
#నిర్వచించు GO_FULLSCREEN	's'		// enter fullscreen
#నిర్వచించు GO_TEAROFF	't'		// add tear-off menu items
#నిర్వచించు GO_TOOLBAR	'T'		// add toolbar
#నిర్వచించు GO_FOOTER	'F'		// add footer
#నిర్వచించు GO_VERTICAL	'v'		// arrange dialog buttons vertically
#నిర్వచించు GO_KEEPWINSIZE	'k'		// keep GUI window size
// all possible flags for 'go'
#నిర్వచించు GO_ALL		"!aAbcCdefFghilLmMpPrRstTvk"

// flags for 'comments' option
#నిర్వచించు COM_NEST	'n'		// comments strings nest
#నిర్వచించు COM_BLANK	'b'		// needs blank after string
#నిర్వచించు COM_ప్రారంభం	's'		// start of comment
#నిర్వచించు COM_మధ్య	'm'		// middle of comment
#నిర్వచించు COM_చివర		'e'		// end of comment
#నిర్వచించు COM_AUTO_చివర	'x'		// last char of end closes comment
#నిర్వచించు COM_మొదటి	'f'		// first line comment only
#నిర్వచించు COM_ఎడమ	'l'		// left adjusted
#నిర్వచించు COM_కుడి	'r'		// right adjusted
#నిర్వచించు COM_NOBACK	'O'		// don't use for "O" command
#నిర్వచించు COM_ALL		"nbsmexflrO"	// all flags for 'comments' option
#నిర్వచించు COM_MAX_LEN	50		// maximum length of a part

// flags for 'statusline' option
#నిర్వచించు STL_FILEPATH	'f'		// path of file in buffer
#నిర్వచించు STL_FULLPATH	'F'		// full path of file in buffer
#నిర్వచించు STL_FILENAME	't'		// last part (tail) of file path
#నిర్వచించు STL_స్తంభం	'c'		// column og cursor
#నిర్వచించు STL_VIRTCOL	'v'		// virtual column
#నిర్వచించు STL_VIRTCOL_ALT	'V'		// - with 'if different' display
#నిర్వచించు STL_పంక్తి	'l'		// line number of cursor
#నిర్వచించు STL_NUMLINES	'L'		// number of lines in buffer
#నిర్వచించు STL_BUFNO	'n'		// current buffer number
#నిర్వచించు STL_KEYMAP	'k'		// 'keymap' when active
#నిర్వచించు STL_స్థానభ్రంశం	'o'		// offset of character under cursor
#నిర్వచించు STL_స్థానభ్రంశం_X	'O'		// - in hexadecimal
#నిర్వచించు STL_BYTEVAL	'b'		// byte value of character
#నిర్వచించు STL_BYTEVAL_X	'B'		// - in hexadecimal
#నిర్వచించు STL_ROFLAG	'r'		// readonly flag
#నిర్వచించు STL_ROFLAG_ALT	'R'		// - other display
#నిర్వచించు STL_HELPFLAG	'h'		// window is showing a help file
#నిర్వచించు STL_HELPFLAG_ALT 'H'		// - other display
#నిర్వచించు STL_FILETYPE	'y'		// 'filetype'
#నిర్వచించు STL_FILETYPE_ALT 'Y'		// - other display
#నిర్వచించు STL_PREVIEWFLAG	'w'		// window is showing the preview buf
#నిర్వచించు STL_PREVIEWFLAG_ALT 'W'		// - other display
#నిర్వచించు STL_MODIFIED	'm'		// modified flag
#నిర్వచించు STL_MODIFIED_ALT 'M'		// - other display
#నిర్వచించు STL_QUICKFIX	'q'		// quickfix window description
#నిర్వచించు STL_PERCENTAGE	'p'		// percentage through file
#నిర్వచించు STL_ALTPERCENT	'P'		// percentage as TOP BOT ALL or NN%
#నిర్వచించు STL_ARGLISTSTAT	'a'		// argument list status as (x of y)
#నిర్వచించు STL_PAGENUM	'N'		// page number (when printing)
#నిర్వచించు STL_SHOWCMD	'S'		// 'showcmd' buffer
#నిర్వచించు STL_VIM_సమాసం	'{'		// start of expression to substitute
#నిర్వచించు STL_SEPARATE	'='		// separation between alignment
					// sections
#నిర్వచించు STL_TRUNCMARK	'<'		// truncation mark if line is too long
#నిర్వచించు STL_USER_HL	'*'		// highlight from (User)1..9 or 0
#నిర్వచించు STL_HIGHLIGHT	'#'		// highlight name
#నిర్వచించు STL_LINEBREAK	'@'		// insert a line break
#నిర్వచించు STL_CLICKFUNC	'['		// click handler region
#నిర్వచించు STL_TABPAGENR	'T'		// tab page label nr
#నిర్వచించు STL_TABCLOSENR	'X'		// tab page close nr
#నిర్వచించు STL_ALL		((char_u *) "fFtcvVlLknoObBrRhHmYyWwMqpPaNS{#@[")

// flags used for parsed 'wildmode'
#నిర్వచించు WIM_నిండిన	0x01
#నిర్వచించు WIM_LONGEST	0x02
#నిర్వచించు WIM_LIST	0x04
#నిర్వచించు WIM_BUFLASTUSED	0x08
#నిర్వచించు WIM_NOSELECT	0x10
#నిర్వచించు WIM_NOINSERT	0x20

// flags for the 'wildoptions' option
// each defined char should be unique over all values.
#నిర్వచించు WOP_FUZZY	'z'
#నిర్వచించు WOP_TAGFILE	'g'
#నిర్వచించు WOP_PUM		'p'
#నిర్వచించు WOP_EXACTTEXT	'x'

// arguments for can_bs()
// each defined char should be unique over all values
// except for BS_START, that intentionally also matches BS_NOSTOP
// because BS_NOSTOP behaves exactly the same except it
// does not stop at the start of the insert point
#నిర్వచించు BS_INDENT	'i'	// "Indent"
#నిర్వచించు BS_EOL		'l'	// "eoL"
#నిర్వచించు BS_ప్రారంభం	's'	// "Start"
#నిర్వచించు BS_NOSTOP	'p'	// "nostoP

// flags for the 'culopt' option
#నిర్వచించు CULOPT_పంక్తి	0x01	// Highlight complete line
#నిర్వచించు CULOPT_SCRLINE	0x02	// Highlight screen line
#నిర్వచించు CULOPT_NBR	0x04	// Highlight Number column

#నిర్వచించు LISPWORD_విలువ	"defun,define,defmacro,set!,lambda,if,case,let,flet,let*,letrec,do,do*,define-syntax,let-syntax,letrec-syntax,destructuring-bind,defpackage,defparameter,defstruct,deftype,defvar,do-all-symbols,do-external-symbols,do-symbols,dolist,dotimes,ecase,etypecase,eval-when,labels,macrolet,multiple-value-bind,multiple-value-call,multiple-value-prog1,multiple-value-setq,prog1,progv,typecase,unless,unwind-protect,when,with-input-from-string,with-open-file,with-open-stream,with-output-to-string,with-package-iterator,define-condition,handler-bind,handler-case,restart-bind,restart-case,with-simple-restart,store-value,use-value,muffle-warning,abort,continue,with-slots,with-slots*,with-accessors,with-accessors*,defclass,defmethod,print-unreadable-object"

/*
 * The following are actual variables for the options
 */

#నిర్వచితమైతే FEAT_RIGHTLEFT
EXTERN దీర్ఘ	p_aleph;	// 'aleph'
#అయితే_ముగింపు
EXTERN char_u	*p_ambw;	// 'ambiwidth'
#నిర్వచితమైతే FEAT_AUTOCHDIR
EXTERN పూర్ణాంకం	p_acd;		// 'autochdir'
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_AUTOSHELLDIR
EXTERN పూర్ణాంకం	p_asd;		// 'autoshelldir'
#అయితే_ముగింపు
EXTERN పూర్ణాంకం	p_ai;		// 'autoindent'
EXTERN పూర్ణాంకం	p_bin;		// 'binary'
EXTERN పూర్ణాంకం	p_bomb;		// 'bomb'
EXTERN పూర్ణాంకం	p_bl;		// 'buflisted'
EXTERN పూర్ణాంకం	p_cin;		// 'cindent'
EXTERN char_u	*p_cink;	// 'cinkeys'
EXTERN char_u	*p_cinsd;	// 'cinscopedecls'
EXTERN char_u	*p_cinw;	// 'cinwords'
#నిర్వచితమైతే FEAT_COMPL_FUNC
EXTERN char_u	*p_cfu;		// 'completefunc'
EXTERN char_u	*p_ofu;		// 'omnifunc'
EXTERN char_u	*p_tsrfu;	// 'thesaurusfunc'
#అయితే_ముగింపు
EXTERN పూర్ణాంకం	p_ci;		// 'copyindent'
#ఒకవేళ defined(FEAT_GUI) && defined(MACOS_X)
EXTERN పూర్ణాంకం	*p_antialias;	// 'antialias'
#అయితే_ముగింపు
EXTERN పూర్ణాంకం	p_ar;		// 'autoread'
EXTERN పూర్ణాంకం	p_aw;		// 'autowrite'
EXTERN పూర్ణాంకం	p_awa;		// 'autowriteall'
EXTERN char_u	*p_bs;		// 'backspace'
EXTERN char_u	*p_bg;		// 'background'
EXTERN పూర్ణాంకం	p_bk;		// 'backup'
EXTERN char_u	*p_bkc;		// 'backupcopy'
EXTERN గుర్తులేని	bkc_flags;	// flags from 'backupcopy'
#నిర్వచించు BKC_YES		0x001
#నిర్వచించు BKC_AUTO		0x002
#నిర్వచించు BKC_NO			0x004
#నిర్వచించు BKC_BREAKSYMLINK	0x008
#నిర్వచించు BKC_BREAKHARDLINK	0x010
EXTERN char_u	*p_bdir;	// 'backupdir'
EXTERN char_u	*p_bex;		// 'backupext'
EXTERN char_u	*p_bo;		// 'belloff'
EXTERN గుర్తులేని	bo_flags;

// values for the 'belloff' option
#నిర్వచించు BO_ALL		0x0001
#నిర్వచించు BO_BS		0x0002
#నిర్వచించు BO_CRSR		0x0004
#నిర్వచించు BO_COMPL	0x0008
#నిర్వచించు BO_COPY		0x0010
#నిర్వచించు BO_CTRLG	0x0020
#నిర్వచించు BO_ERROR	0x0040
#నిర్వచించు BO_ESC		0x0080
#నిర్వచించు BO_EX		0x0100
#నిర్వచించు BO_HANGUL	0x0200
#నిర్వచించు BO_IM		0x0400
#నిర్వచించు BO_LANG		0x0800
#నిర్వచించు BO_MESS		0x1000
#నిర్వచించు BO_సరిపోలిక	0x2000
#నిర్వచించు BO_OPER		0x4000
#నిర్వచించు BO_REG		0x8000
#నిర్వచించు BO_SH		0x10000
#నిర్వచించు BO_SPELL	0x20000
#నిర్వచించు BO_పదం		0x40000
#నిర్వచించు BO_WILD		0x80000

EXTERN char_u	*p_bsk;		// 'backupskip'
#నిర్వచితమైతే FEAT_CRYPT
EXTERN char_u	*p_cm;		// 'cryptmethod'
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_BEVAL
# ifdef FEAT_BEVAL_GUI
EXTERN పూర్ణాంకం	p_beval;	// 'ballooneval'
# endif
EXTERN దీర్ఘ	p_bdlay;	// 'balloondelay'
# ifdef FEAT_EVAL
EXTERN char_u	*p_bexpr;
# endif
# ifdef FEAT_BEVAL_పదం
EXTERN పూర్ణాంకం	p_bevalterm;	// 'balloonevalterm'
# endif
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_BROWSE
EXTERN char_u	*p_bsdir;	// 'browsedir'
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_LINEBREAK
EXTERN char_u	*p_breakat;	// 'breakat'
#అయితే_ముగింపు
EXTERN char_u	*p_bh;		// 'bufhidden'
EXTERN char_u	*p_bt;		// 'buftype'
EXTERN char_u	*p_cmp;		// 'casemap'
EXTERN గుర్తులేని	cmp_flags;
#నిర్వచించు CMP_అంతర్గత		0x001
#నిర్వచించు CMP_KEEPASCII		0x002
EXTERN char_u	*p_enc;		// 'encoding'
EXTERN పూర్ణాంకం	p_deco;		// 'delcombine'
#నిర్వచితమైతే FEAT_EVAL
EXTERN char_u	*p_ccv;		// 'charconvert'
#అయితే_ముగింపు
EXTERN పూర్ణాంకం	p_cdh;		// 'cdhome'
EXTERN char_u	*p_cino;	// 'cinoptions'
EXTERN char_u	*p_cedit;	// 'cedit'
EXTERN దీర్ఘ	p_cwh;		// 'cmdwinheight'
#నిర్వచితమైతే HAVE_CLIPMETHOD
EXTERN char_u	*p_cb;		// 'clipboard'
EXTERN char_u	*p_cpm;		// 'clipmethod'
#అయితే_ముగింపు
EXTERN దీర్ఘ	p_ch;		// 'cmdheight'
EXTERN char_u	*p_cms;		// 'commentstring'
EXTERN char_u	*p_cpt;		// 'complete'
EXTERN దీర్ఘ	p_cto;		// 'completetimeout'
#ఒకవేళ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
EXTERN పూర్ణాంకం	p_confirm;	// 'confirm'
#అయితే_ముగింపు
EXTERN పూర్ణాంకం	p_cp;		// 'compatible'
EXTERN char_u	*p_cfc;		// 'completefuzzycollect'
EXTERN గుర్తులేని cfc_flags;	// flags from "completefuzzycollect"
EXTERN char_u	*p_cia;		// 'completeitemalign'
EXTERN గుర్తులేని cia_flags;	// order flags of 'completeitemalign'
EXTERN char_u	*p_cot;		// 'completeopt'
EXTERN గుర్తులేని	cot_flags;	// flags from 'completeopt'
EXTERN పూర్ణాంకం	p_ac;		// 'autocomplete'
EXTERN దీర్ఘ	p_act;		// 'autocompletetimeout'
EXTERN దీర్ఘ	p_acl;		// 'autocompletedelay'
// Keep in sync with p_cot_values in optionstr.c
#నిర్వచించు COT_MENU	0x001
#నిర్వచించు COT_MENUONE	0x002
#నిర్వచించు COT_ANY_MENU	0x003	// combination of menu flags
#నిర్వచించు COT_LONGEST	0x004	// FALSE: insert full match,
				// TRUE: insert longest prefix
#నిర్వచించు COT_PREVIEW	    0x008
#నిర్వచించు COT_POPUP	    0x010
#నిర్వచించు COT_POPUPHIDDEN	    0x020
#నిర్వచించు COT_ANY_PREVIEW	    0x038   // combination of preview flags
#నిర్వచించు COT_NOINSERT	    0x040   // FALSE: select & insert, TRUE: noinsert
#నిర్వచించు COT_NOSELECT	    0x080   // FALSE: select & insert, TRUE: noselect
#నిర్వచించు COT_FUZZY	    0x100   // TRUE: fuzzy match enabled
#నిర్వచించు COT_NOSORT	    0x200   // TRUE: fuzzy match without qsort score
#నిర్వచించు COT_PREINSERT	    0x400   // TRUE: preinsert
#నిర్వచించు COT_సమీప	    0x800   // TRUE: prioritize matches close to cursor

#నిర్వచించు CFC_KEYWORD         0x001
#నిర్వచించు CFC_FILES           0x002
#నిర్వచించు CFC_WHOLELINE       0x004

#నిర్వచితమైతే BACKSLASH_లోపల_FILENAME
EXTERN char_u	*p_csl;		// 'completeslash'
#అయితే_ముగింపు
EXTERN దీర్ఘ	p_ph;		// 'pumheight'
EXTERN దీర్ఘ	p_pw;		// 'pumwidth'
EXTERN దీర్ఘ	p_pmw;		// 'pummaxwidth'
EXTERN char_u	*p_pb;		// 'pumborder'
EXTERN char_u	*p_pumopt;	// 'pumopt'
EXTERN char_u	*p_com;		// 'comments'
EXTERN char_u	*p_cpo;		// 'cpoptions'
#నిర్వచితమైతే FEAT_CSCOPE
EXTERN char_u	*p_csprg;	// 'cscopeprg'
EXTERN పూర్ణాంకం	p_csre;		// 'cscoperelative'
# ifdef FEAT_QUICKFIX
EXTERN char_u	*p_csqf;	// 'cscopequickfix'
#  define	CSQF_CMDS   "sgdctefia"
#  define	CSQF_FLAGS  "+-0"
# endif
EXTERN పూర్ణాంకం	p_cst;		// 'cscopetag'
EXTERN దీర్ఘ	p_csto;		// 'cscopetagorder'
EXTERN దీర్ఘ	p_cspc;		// 'cscopepathcomp'
EXTERN పూర్ణాంకం	p_csverbose;	// 'cscopeverbose'
#అయితే_ముగింపు
EXTERN char_u	*p_debug;	// 'debug'
#నిర్వచితమైతే FEAT_కనుగొను_ID
EXTERN char_u	*p_def;		// 'define'
EXTERN char_u	*p_inc;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_DIFF
EXTERN char_u	*p_dia;		// 'diffanchors'
EXTERN char_u	*p_dip;		// 'diffopt'
# ifdef FEAT_EVAL
EXTERN char_u	*p_dex;		// 'diffexpr'
# endif
#అయితే_ముగింపు
EXTERN char_u	*p_dict;	// 'dictionary'
#నిర్వచితమైతే FEAT_DIGRAPHS
EXTERN పూర్ణాంకం	p_dg;		// 'digraph'
#అయితే_ముగింపు
EXTERN char_u	*p_dir;		// 'directory'
EXTERN char_u	*p_dy;		// 'display'
EXTERN గుర్తులేని	dy_flags;
#నిర్వచించు DY_LASTLINE		0x001
#నిర్వచించు DY_TRUNCATE		0x002
#నిర్వచించు DY_UHEX			0x004
EXTERN పూర్ణాంకం	p_ed;		// 'edcompatible'
EXTERN char_u	*p_ead;		// 'eadirection'
EXTERN char_u	*p_emoji;	// 'emoji'
EXTERN పూర్ణాంకం	p_ea;		// 'equalalways'
EXTERN char_u	*p_ep;		// 'equalprg'
EXTERN పూర్ణాంకం	p_eb;		// 'errorbells'
#నిర్వచితమైతే FEAT_QUICKFIX
EXTERN char_u	*p_ef;		// 'errorfile'
EXTERN char_u	*p_efm;		// 'errorformat'
EXTERN char_u	*p_gefm;	// 'grepformat'
EXTERN char_u	*p_gp;		// 'grepprg'
#అయితే_ముగింపు
EXTERN పూర్ణాంకం	p_eof;		// 'endoffile'
EXTERN పూర్ణాంకం	p_eol;		// 'endofline'
EXTERN పూర్ణాంకం	p_ek;		// 'esckeys'
EXTERN char_u	*p_ei;		// 'eventignore'
EXTERN పూర్ణాంకం	p_et;		// 'expandtab'
EXTERN పూర్ణాంకం	p_exrc;		// 'exrc'
EXTERN char_u	*p_fenc;	// 'fileencoding'
EXTERN char_u	*p_fencs;	// 'fileencodings'
EXTERN char_u	*p_ff;		// 'fileformat'
EXTERN char_u	*p_ffs;		// 'fileformats'
EXTERN పూర్ణాంకం	p_fic;		// 'fileignorecase'
EXTERN char_u	*p_ft;		// 'filetype'
EXTERN char_u	*p_fcs;		// 'fillchar'
#నిర్వచితమైతే FEAT_EVAL
EXTERN char_u	*p_ffu;		// 'findfunc'
#అయితే_ముగింపు
EXTERN పూర్ణాంకం	p_fixeol;	// 'fixendofline'
#నిర్వచితమైతే FEAT_FOLDING
EXTERN char_u	*p_fcl;		// 'foldclose'
EXTERN దీర్ఘ	p_fdls;		// 'foldlevelstart'
EXTERN char_u	*p_fdo;		// 'foldopen'
EXTERN గుర్తులేని	fdo_flags;
# define FDO_ALL		0x001
# define FDO_ఖండం		0x002
# define FDO_HOR		0x004
# define FDO_MARK		0x008
# define FDO_PERCENT		0x010
# define FDO_QUICKFIX		0x020
# define FDO_శోధన		0x040
# define FDO_ట్యాగ్		0x080
# define FDO_చొప్పించు		0x100
# define FDO_UNDO		0x200
# define FDO_JUMP		0x400
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_EVAL)
EXTERN char_u	*p_fex;		// 'formatexpr'
#అయితే_ముగింపు
EXTERN char_u	*p_flp;		// 'formatlistpat'
EXTERN char_u	*p_fo;		// 'formatoptions'
EXTERN char_u	*p_fp;		// 'formatprg'
#నిర్వచితమైతే HAVE_FSYNC
EXTERN పూర్ణాంకం	p_fs;		// 'fsync'
#అయితే_ముగింపు
EXTERN పూర్ణాంకం	p_gd;		// 'gdefault'
EXTERN char_u	*p_jop;		// 'jumpoptions'
EXTERN గుర్తులేని	jop_flags;	//
#నిర్వచించు JOP_దొంతర		0x001
#నిర్వచితమైతే FEAT_PROP_POPUP
# ifdef FEAT_QUICKFIX
EXTERN char_u	*p_cpp;		// 'completepopup'
# endif
EXTERN char_u	*p_pvp;		// 'previewpopup'
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_PRINTER
EXTERN char_u	*p_pdev;	// 'printdevice'
# ifdef FEAT_POSTSCRIPT
EXTERN char_u	*p_penc;	// 'printencoding'
EXTERN char_u	*p_pmfn;	// 'printmbfont'
EXTERN char_u	*p_pmcs;	// 'printmbcharset'
# endif
# ఒకవేళ defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
EXTERN char_u	*p_pexpr;	// 'printexpr'
# endif
EXTERN char_u	*p_pfn;		// 'printfont'
EXTERN char_u	*p_popt;	// 'printoptions'
EXTERN char_u	*p_శీర్షిక;	// 'printheader'
#అయితే_ముగింపు
EXTERN పూర్ణాంకం	p_prompt;	// 'prompt'
#నిర్వచితమైతే FEAT_GUI
EXTERN char_u	*p_guifont;	// 'guifont'
# ifdef FEAT_XFONTSET
EXTERN char_u	*p_guifontset;	// 'guifontset'
# endif
EXTERN char_u	*p_guifontwide;	// 'guifontwide'
EXTERN పూర్ణాంకం	p_guipty;	// 'guipty'
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
EXTERN char_u	*p_guiligatures;  // 'guiligatures'
#అయితే_ముగింపు
#ఒకవేళ (defined(FEAT_GUI_GTK) && !defined(USE_GTK4)) || defined(FEAT_GUI_X11)
EXTERN దీర్ఘ	p_ghr;		// 'guiheadroom'
#అయితే_ముగింపు
#నిర్వచితమైతే CURSOR_SHAPE
EXTERN char_u	*p_guicursor;	// 'guicursor'
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_MOUSESHAPE
EXTERN char_u	*p_mouseshape;	// 'mouseshape'
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_GUI)
EXTERN char_u	*p_go;		// 'guioptions'
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_GUI_TABLINE)
EXTERN char_u	*p_gtl;		// 'guitablabel'
EXTERN char_u	*p_gtt;		// 'guitabtooltip'
#అయితే_ముగింపు
EXTERN char_u	*p_hf;		// 'helpfile'
EXTERN దీర్ఘ	p_hh;		// 'helpheight'
#నిర్వచితమైతే FEAT_MULTI_LANG
EXTERN char_u	*p_hlg;		// 'helplang'
#అయితే_ముగింపు
EXTERN పూర్ణాంకం	p_hid;		// 'hidden'
EXTERN char_u	*p_hl;		// 'highlight'
EXTERN పూర్ణాంకం	p_hls;		// 'hlsearch'
EXTERN దీర్ఘ	p_hi;		// 'history'
#నిర్వచితమైతే FEAT_RIGHTLEFT
EXTERN పూర్ణాంకం	p_hkmap;	// 'hkmap'
EXTERN పూర్ణాంకం	p_hkmapp;	// 'hkmapp'
# ifdef FEAT_ARABIC
EXTERN పూర్ణాంకం	p_arshape;	// 'arabicshape'
# endif
#అయితే_ముగింపు
EXTERN పూర్ణాంకం	p_icon;		// 'icon'
EXTERN char_u	*p_iconstring;	// 'iconstring'
EXTERN పూర్ణాంకం	p_ic;		// 'ignorecase'
#ఒకవేళ defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
EXTERN char_u	*p_imak;	// 'imactivatekey'
# define IM_ON_THE_SPOT		0L
# define IM_OVER_THE_SPOT	1L
EXTERN దీర్ఘ	p_imst;		// 'imstyle'
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_IMAGE
EXTERN char_u	*p_ipc;		// 'imageprotocol'
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_EVAL)
EXTERN char_u	*p_imaf;	// 'imactivatefunc'
EXTERN char_u	*p_imsf;	// 'imstatusfunc'
#అయితే_ముగింపు
EXTERN పూర్ణాంకం	p_imcmdline;	// 'imcmdline'
EXTERN పూర్ణాంకం	p_imdisable;	// 'imdisable'
EXTERN దీర్ఘ	p_iminsert;	// 'iminsert'
EXTERN దీర్ఘ	p_imsearch;	// 'imsearch'
EXTERN పూర్ణాంకం	p_inf;		// 'infercase'
#ఒకవేళ defined(FEAT_కనుగొను_ID) && defined(FEAT_EVAL)
EXTERN char_u	*p_inex;	// 'includeexpr'
#అయితే_ముగింపు
EXTERN పూర్ణాంకం	p_is;		// 'incsearch'
#ఒకవేళ defined(FEAT_EVAL)
EXTERN char_u	*p_inde;	// 'indentexpr'
EXTERN char_u	*p_indk;	// 'indentkeys'
#అయితే_ముగింపు
EXTERN పూర్ణాంకం	p_im;		// 'insertmode'
EXTERN char_u	*p_isf;		// 'isfname'
EXTERN char_u	*p_isi;		// 'isident'
EXTERN char_u	*p_isk;		// 'iskeyword'
EXTERN char_u	*p_isp;		// 'isprint'
EXTERN పూర్ణాంకం	p_js;		// 'joinspaces'
#నిర్వచితమైతే FEAT_CRYPT
EXTERN char_u	*p_కీలకం;		// 'key'
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_KEYMAP
EXTERN char_u	*p_keymap;	// 'keymap'
#అయితే_ముగింపు
EXTERN char_u	*p_kp;		// 'keywordprg'
EXTERN char_u	*p_km;		// 'keymodel'
EXTERN char_u	*p_kpc;		// 'keyprotocol'
#నిర్వచితమైతే FEAT_LANGMAP
EXTERN char_u	*p_langmap;	// 'langmap'
EXTERN పూర్ణాంకం	p_lnr;		// 'langnoremap'
EXTERN పూర్ణాంకం	p_lrm;		// 'langremap'
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_MENU) && defined(FEAT_MULTI_LANG)
EXTERN char_u	*p_lm;		// 'langmenu'
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_GUI
EXTERN దీర్ఘ	p_linespace;	// 'linespace'
#అయితే_ముగింపు
EXTERN పూర్ణాంకం	p_lisp;		// 'lisp'
EXTERN char_u	*p_lop;		// 'lispoptions'
EXTERN char_u	*p_lispwords;	// 'lispwords'
EXTERN దీర్ఘ	p_ls;		// 'laststatus'
EXTERN దీర్ఘ	p_stal;		// 'showtabline'
EXTERN char_u	*p_lcs;		// 'listchars'

EXTERN పూర్ణాంకం	p_lz;		// 'lazyredraw'
EXTERN పూర్ణాంకం	p_lpl;		// 'loadplugins'
#ఒకవేళ defined(DYNAMIC_LUA)
EXTERN char_u	*p_luadll;	// 'luadll'
#అయితే_ముగింపు
EXTERN పూర్ణాంకం	p_magic;	// 'magic'
EXTERN char_u	*p_menc;	// 'makeencoding'
#నిర్వచితమైతే FEAT_QUICKFIX
EXTERN char_u	*p_mef;		// 'makeef'
EXTERN char_u	*p_mp;		// 'makeprg'
#అయితే_ముగింపు
EXTERN char_u	*p_mps;		// 'matchpairs'
EXTERN దీర్ఘ	p_mat;		// 'matchtime'
EXTERN దీర్ఘ	p_mco;		// 'maxcombine'
#నిర్వచితమైతే FEAT_EVAL
EXTERN దీర్ఘ	p_mfd;		// 'maxfuncdepth'
#అయితే_ముగింపు
EXTERN దీర్ఘ	p_mmd;		// 'maxmapdepth'
EXTERN దీర్ఘ	p_mm;		// 'maxmem'
EXTERN దీర్ఘ	p_mmp;		// 'maxmempattern'
EXTERN దీర్ఘ	p_mmt;		// 'maxmemtot'
#నిర్వచితమైతే FEAT_MENU
EXTERN దీర్ఘ	p_mis;		// 'menuitems'
#అయితే_ముగింపు
EXTERN char_u	*p_mopt;	// 'messagesopt'
EXTERN దీర్ఘ	p_msc;		// 'maxsearchcount'
#నిర్వచితమైతే FEAT_SPELL
EXTERN char_u	*p_msm;		// 'mkspellmem'
#అయితే_ముగింపు
EXTERN పూర్ణాంకం	p_ml;		// 'modeline'
EXTERN పూర్ణాంకం	p_mle;		// 'modelineexpr'
EXTERN దీర్ఘ	p_mls;		// 'modelines'
EXTERN పూర్ణాంకం	p_mlstr;	// 'modelinestrict'
EXTERN పూర్ణాంకం	p_ma;		// 'modifiable'
EXTERN పూర్ణాంకం	p_mod;		// 'modified'
EXTERN char_u	*p_mouse;	// 'mouse'
#నిర్వచితమైతే FEAT_GUI
EXTERN పూర్ణాంకం	p_mousef;	// 'mousefocus'
EXTERN పూర్ణాంకం	p_mh;		// 'mousehide'
#అయితే_ముగింపు
EXTERN char_u	*p_mousem;	// 'mousemodel'
#నిర్వచితమైతే FEAT_GUI
EXTERN పూర్ణాంకం	p_mousemev;	// 'mousemoveevent'
#అయితే_ముగింపు
EXTERN దీర్ఘ	p_mouset;	// 'mousetime'
EXTERN పూర్ణాంకం	p_మరిన్ని;		// 'more'
#నిర్వచితమైతే FEAT_MZSCHEME
EXTERN దీర్ఘ	p_mzq;		// 'mzquantum
# ఒకవేళ defined(DYNAMIC_MZSCHEME)
EXTERN char_u	*p_mzschemedll;	// 'mzschemedll'
EXTERN char_u	*p_mzschemegcdll; // 'mzschemegcdll'
# endif
#అయితే_ముగింపు
EXTERN char_u	*p_nf;		// 'nrformats'
#ఒకవేళ defined(MSWIN)
EXTERN పూర్ణాంకం	p_odev;		// 'opendevice'
#అయితే_ముగింపు
EXTERN దీర్ఘ	p_ost;		// 'osctimeoutlen'
EXTERN char_u	*p_opfunc;	// 'operatorfunc'
EXTERN char_u	*p_para;	// 'paragraphs'
EXTERN పూర్ణాంకం	p_paste;	// 'paste'
EXTERN char_u	*p_pt;		// 'pastetoggle'
#ఒకవేళ defined(FEAT_EVAL) && defined(FEAT_DIFF)
EXTERN char_u	*p_pex;		// 'patchexpr'
#అయితే_ముగింపు
EXTERN char_u	*p_pm;		// 'patchmode'
EXTERN char_u	*p_మార్గం;	// 'path'
EXTERN char_u	*p_cdpath;	// 'cdpath'
#ఒకవేళ defined(DYNAMIC_PERL)
EXTERN char_u	*p_perldll;	// 'perldll'
#అయితే_ముగింపు
EXTERN పూర్ణాంకం	p_pi;		// 'preserveindent'
#ఒకవేళ defined(DYNAMIC_PYTHON3)
EXTERN char_u	*p_py3dll;	// 'pythonthreedll'
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_PYTHON3
EXTERN char_u	*p_py3home;	// 'pythonthreehome'
#అయితే_ముగింపు
#ఒకవేళ defined(DYNAMIC_PYTHON)
EXTERN char_u	*p_pydll;	// 'pythondll'
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_PYTHON
EXTERN char_u	*p_pyhome;	// 'pythonhome'
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
EXTERN దీర్ఘ	p_pyx;		// 'pyxversion'
#అయితే_ముగింపు
EXTERN char_u	*p_qe;		// 'quoteescape'
EXTERN పూర్ణాంకం	p_ro;		// 'readonly'
#నిర్వచితమైతే FEAT_RELTIME
EXTERN దీర్ఘ	p_rdt;		// 'redrawtime'
#అయితే_ముగింపు
EXTERN పూర్ణాంకం	p_remap;	// 'remap'
EXTERN దీర్ఘ	p_re;		// 'regexpengine'
#నిర్వచితమైతే FEAT_RENDER_OPTIONS
EXTERN char_u	*p_rop;		// 'renderoptions'
#అయితే_ముగింపు
EXTERN దీర్ఘ	p_report;	// 'report'
#ఒకవేళ defined(FEAT_QUICKFIX)
EXTERN దీర్ఘ	p_pvh;		// 'previewheight'
EXTERN దీర్ఘ     p_chi;          // 'chistory'
#అయితే_ముగింపు
#నిర్వచితమైతే MSWIN
EXTERN పూర్ణాంకం	p_rs;		// 'restorescreen'
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_RIGHTLEFT
EXTERN పూర్ణాంకం	p_ari;		// 'allowrevins'
EXTERN పూర్ణాంకం	p_ri;		// 'revins'
#అయితే_ముగింపు
#ఒకవేళ defined(DYNAMIC_RUBY)
EXTERN char_u	*p_rubydll;	// 'rubydll'
#అయితే_ముగింపు
EXTERN పూర్ణాంకం	p_ru;		// 'ruler'
#నిర్వచితమైతే FEAT_STL_OPT
EXTERN char_u	*p_ruf;		// 'rulerformat'
#అయితే_ముగింపు
EXTERN char_u	*p_pp;		// 'packpath'
#నిర్వచితమైతే FEAT_QUICKFIX
EXTERN char_u	*p_qftf;	// 'quickfixtextfunc'
#అయితే_ముగింపు
EXTERN char_u	*p_rtp;		// 'runtimepath'
EXTERN దీర్ఘ	p_sj;		// 'scrolljump'
#ఒకవేళ defined(MSWIN) && defined(FEAT_GUI)
EXTERN పూర్ణాంకం	p_scf;		// 'scrollfocus'
#అయితే_ముగింపు
EXTERN దీర్ఘ	p_so;		// 'scrolloff'
EXTERN దీర్ఘ	p_sop;		// 'scrolloffpad'
EXTERN char_u	*p_sbo;		// 'scrollopt'
EXTERN char_u	*p_sections;	// 'sections'
EXTERN పూర్ణాంకం	p_secure;	// 'secure'
EXTERN char_u	*p_sel;		// 'selection'
EXTERN char_u	*p_slm;		// 'selectmode'
#నిర్వచితమైతే FEAT_SESSION
EXTERN char_u	*p_ssop;	// 'sessionoptions'
EXTERN గుర్తులేని	ssop_flags;
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
#అయితే_ముగింపు
EXTERN char_u	*p_sh;		// 'shell'
EXTERN char_u	*p_shcf;	// 'shellcmdflag'
#నిర్వచితమైతే FEAT_QUICKFIX
EXTERN char_u	*p_sp;		// 'shellpipe'
#అయితే_ముగింపు
EXTERN char_u	*p_shq;		// 'shellquote'
EXTERN char_u	*p_sxq;		// 'shellxquote'
EXTERN char_u	*p_sxe;		// 'shellxescape'
EXTERN char_u	*p_srr;		// 'shellredir'
#నిర్వచితమైతే AMIGA
EXTERN దీర్ఘ	p_st;		// 'shelltype'
#అయితే_ముగింపు
EXTERN పూర్ణాంకం	p_stmp;		// 'shelltemp'
#నిర్వచితమైతే BACKSLASH_లోపల_FILENAME
EXTERN పూర్ణాంకం	p_ssl;		// 'shellslash'
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_STL_OPT
EXTERN char_u	*p_stl;		// 'statusline'
EXTERN char_u	*p_stlo;	// 'statuslineopt'
#అయితే_ముగింపు
EXTERN పూర్ణాంకం	p_sr;		// 'shiftround'
EXTERN దీర్ఘ	p_sw;		// 'shiftwidth'
EXTERN char_u	*p_shm;		// 'shortmess'
EXTERN పూర్ణాంకం	p_sn;		// 'shortname'
#నిర్వచితమైతే FEAT_LINEBREAK
EXTERN char_u	*p_sbr;		// 'showbreak'
#అయితే_ముగింపు
EXTERN పూర్ణాంకం	p_sc;		// 'showcmd'
EXTERN char_u	*p_sloc;	// 'showcmdloc'
EXTERN పూర్ణాంకం	p_sft;		// 'showfulltag'
EXTERN పూర్ణాంకం	p_sm;		// 'showmatch'
EXTERN పూర్ణాంకం	p_smd;		// 'showmode'
EXTERN దీర్ఘ	p_ss;		// 'sidescroll'
EXTERN దీర్ఘ	p_siso;		// 'sidescrolloff'
EXTERN పూర్ణాంకం	p_scs;		// 'smartcase'
EXTERN పూర్ణాంకం	p_si;		// 'smartindent'
EXTERN పూర్ణాంకం	p_sta;		// 'smarttab'
EXTERN దీర్ఘ	p_sts;		// 'softtabstop'
EXTERN పూర్ణాంకం	p_sb;		// 'splitbelow'
EXTERN char_u	*p_sua;		// 'suffixesadd'
EXTERN పూర్ణాంకం	p_swf;		// 'swapfile'
#నిర్వచితమైతే FEAT_SYN_HL
EXTERN దీర్ఘ	p_smc;		// 'synmaxcol'
#అయితే_ముగింపు
EXTERN దీర్ఘ	p_tpm;		// 'tabpagemax'
#నిర్వచితమైతే FEAT_STL_OPT
EXTERN char_u	*p_tal;		// 'tabline'
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_EVAL
EXTERN char_u	*p_tfu;		// 'tagfunc'
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_SPELL
EXTERN char_u	*p_spc;		// 'spellcapcheck'
EXTERN char_u	*p_spf;		// 'spellfile'
EXTERN char_u	*p_spl;		// 'spelllang'
EXTERN char_u	*p_spo;		// 'spelloptions'
EXTERN char_u	*p_sps;		// 'spellsuggest'
#అయితే_ముగింపు
EXTERN పూర్ణాంకం	p_spr;		// 'splitright'
EXTERN పూర్ణాంకం	p_sol;		// 'startofline'
EXTERN char_u	*p_su;		// 'suffixes'
EXTERN char_u	*p_sws;		// 'swapsync'
EXTERN char_u	*p_swb;		// 'switchbuf'
EXTERN గుర్తులేని	swb_flags;
// Keep in sync with p_swb_values in optionstr.c
#నిర్వచించు SWB_USEOPEN		0x001
#నిర్వచించు SWB_USETAB		0x002
#నిర్వచించు SWB_విభజించు		0x004
#నిర్వచించు SWB_NEWTAB		0x008
#నిర్వచించు SWB_VSPLIT		0x010
#నిర్వచించు SWB_USELAST		0x020

EXTERN char_u	*p_spk;		// 'splitkeep'

#ఒకవేళ defined(FEAT_TABPANEL)
EXTERN char_u	*p_tpl;		// 'tabpanel'
EXTERN దీర్ఘ	p_stpl;		// 'showtabpanel'
EXTERN char_u	*p_tplo;	// 'tabpanelopt'
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_SYN_HL
EXTERN char_u	*p_syn;		// 'syntax'
#అయితే_ముగింపు
EXTERN char_u	*p_tcl;		// 'tabclose'
EXTERN గుర్తులేని	tcl_flags;	// flags from 'tabclose'
#నిర్వచించు TCL_ఎడమ		0x001
#నిర్వచించు TCL_USELAST		0x002
EXTERN దీర్ఘ	p_ts;		// 'tabstop'
EXTERN పూర్ణాంకం	p_tbs;		// 'tagbsearch'
EXTERN char_u	*p_tc;		// 'tagcase'
EXTERN గుర్తులేని tc_flags;       // flags from 'tagcase'
#నిర్వచించు TC_FOLLOWIC		0x01
#నిర్వచించు TC_IGNORE		0x02
#నిర్వచించు TC_సరిపోలిక		0x04
#నిర్వచించు TC_FOLLOWSCS		0x08
#నిర్వచించు TC_SMART		0x10
EXTERN దీర్ఘ	p_tl;		// 'taglength'
EXTERN పూర్ణాంకం	p_tr;		// 'tagrelative'
EXTERN char_u	*p_ట్యాగులు;	// 'tags'
EXTERN పూర్ణాంకం	p_tagsecure;    // 'tagsecure'
EXTERN పూర్ణాంకం	p_tgst;		// 'tagstack'
#ఒకవేళ defined(DYNAMIC_TCL)
EXTERN char_u	*p_tcldll;	// 'tcldll'
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_ARABIC
EXTERN పూర్ణాంకం	p_tbidi;	// 'termbidi'
#అయితే_ముగింపు
EXTERN char_u	*p_tenc;	// 'termencoding'
#నిర్వచితమైతే FEAT_TERMGUICOLORS
EXTERN పూర్ణాంకం	p_tgc;		// 'termguicolors'
#అయితే_ముగింపు
#నిర్వచితమైతే UNIX
EXTERN char_u	*p_trz;		// 'termresize'
#అయితే_ముగింపు
EXTERN పూర్ణాంకం	p_tsy;		// 'termsync'
#నిర్వచితమైతే FEAT_TERMINAL
EXTERN దీర్ఘ	p_twsl;		// 'termwinscroll'
#అయితే_ముగింపు
#ఒకవేళ defined(MSWIN) && defined(FEAT_TERMINAL)
EXTERN char_u	*p_twt;		// 'termwintype'
#అయితే_ముగింపు
EXTERN పూర్ణాంకం	p_terse;	// 'terse'
EXTERN పూర్ణాంకం	p_ta;		// 'textauto'
EXTERN పూర్ణాంకం	p_tx;		// 'textmode'
EXTERN దీర్ఘ	p_tw;		// 'textwidth'
EXTERN పూర్ణాంకం	p_గమ్యం;		// 'tildeop'
EXTERN పూర్ణాంకం	p_timeout;	// 'timeout'
EXTERN దీర్ఘ	p_tm;		// 'timeoutlen'
EXTERN పూర్ణాంకం	p_title;	// 'title'
EXTERN దీర్ఘ	p_titlelen;	// 'titlelen'
EXTERN char_u	*p_titleold;	// 'titleold'
EXTERN char_u	*p_titlestring;	// 'titlestring'
EXTERN char_u	*p_tsr;		// 'thesaurus'
EXTERN పూర్ణాంకం	p_ttimeout;	// 'ttimeout'
EXTERN దీర్ఘ	p_ttm;		// 'ttimeoutlen'
EXTERN పూర్ణాంకం	p_tbi;		// 'ttybuiltin'
EXTERN పూర్ణాంకం	p_tf;		// 'ttyfast'
#ఒకవేళ defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
EXTERN char_u	*p_toolbar;	// 'toolbar'
EXTERN గుర్తులేని toolbar_flags;
# define TOOLBAR_పాఠ్యం		0x01
# define TOOLBAR_ICONS		0x02
# define TOOLBAR_TOOLTIPS	0x04
# define TOOLBAR_HORIZ		0x08
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_TOOLBAR) && defined(FEAT_GUI_GTK)
EXTERN char_u	*p_tbis;	// 'toolbariconsize'
EXTERN గుర్తులేని tbis_flags;
# define TBIS_TINY		0x01
# define TBIS_SMALL		0x02
# define TBIS_MEDIUM		0x04
# define TBIS_LARGE		0x08
# define TBIS_HUGE		0x10
# define TBIS_GIANT		0x20
#అయితే_ముగింపు
EXTERN దీర్ఘ	p_ttyscroll;	// 'ttyscroll'
#ఒకవేళ defined(UNIX) || defined(VMS)
EXTERN char_u	*p_ttym;	// 'ttymouse'
EXTERN గుర్తులేని ttym_flags;
# define TTYM_XTERM		0x01
# define TTYM_XTERM2		0x02
# define TTYM_DEC		0x04
# define TTYM_NETTERM		0x08
# define TTYM_JSBTERM		0x10
# define TTYM_PTERM		0x20
# define TTYM_URXVT		0x40
# define TTYM_SGR		0x80
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_PERSISTENT_UNDO
EXTERN char_u	*p_udir;	// 'undodir'
EXTERN పూర్ణాంకం	p_udf;		// 'undofile'
#అయితే_ముగింపు
EXTERN దీర్ఘ	p_ul;		// 'undolevels'
EXTERN దీర్ఘ	p_ur;		// 'undoreload'
EXTERN దీర్ఘ	p_uc;		// 'updatecount'
EXTERN దీర్ఘ	p_ut;		// 'updatetime'
#నిర్వచితమైతే FEAT_VARTABS
EXTERN char_u	*p_vsts;	// 'varsofttabstop'
EXTERN char_u	*p_vts;		// 'vartabstop'
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_VIMINFO
EXTERN char_u	*p_viminfo;	// 'viminfo'
EXTERN char_u	*p_viminfofile;	// 'viminfofile'
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_SESSION
EXTERN char_u	*p_vdir;	// 'viewdir'
EXTERN char_u	*p_vop;		// 'viewoptions'
EXTERN గుర్తులేని	vop_flags;	// uses SSOP_ flags
#అయితే_ముగింపు
EXTERN పూర్ణాంకం	p_vb;		// 'visualbell'
EXTERN char_u	*p_ve;		// 'virtualedit'
EXTERN గుర్తులేని ve_flags;
#నిర్వచించు VE_ఖండం	5	// includes "all"
#నిర్వచించు VE_చొప్పించు	6	// includes "all"
#నిర్వచించు VE_ALL		4
#నిర్వచించు VE_ONEMORE	8
#నిర్వచించు VE_NONE		16	// "none"
#నిర్వచించు VE_NONEU	32      // "NONE"
EXTERN దీర్ఘ	p_verbose;	// 'verbose'
#నిర్వచితమైతే లోపల_OPTION_C
char_u	*p_vfile = (char_u *)""; // used before options are initialized
#లేకపోతే
బాహ్య char_u	*p_vfile;	// 'verbosefile'
#అయితే_ముగింపు
EXTERN పూర్ణాంకం	p_warn;		// 'warn'
EXTERN char_u	*p_wop;		// 'wildoptions'
EXTERN దీర్ఘ	p_కిటికీ;	// 'window'
#ఒకవేళ defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_MOTIF) || defined(LINT) \
	|| defined (FEAT_GUI_GTK) || defined(FEAT_GUI_PHOTON)
# define FEAT_WAK
EXTERN char_u	*p_wak;		// 'winaltkeys'
#అయితే_ముగింపు
EXTERN char_u	*p_wig;		// 'wildignore'
EXTERN పూర్ణాంకం	p_wiv;		// 'weirdinvert'
EXTERN char_u	*p_ww;		// 'whichwrap'
EXTERN దీర్ఘ	p_wc;		// 'wildchar'
EXTERN దీర్ఘ	p_wcm;		// 'wildcharm'
EXTERN పూర్ణాంకం	p_wic;		// 'wildignorecase'
EXTERN char_u	*p_wim;		// 'wildmode'
EXTERN పూర్ణాంకం	p_wmnu;		// 'wildmenu'
EXTERN దీర్ఘ	p_wh;		// 'winheight'
EXTERN దీర్ఘ	p_wmh;		// 'winminheight'
EXTERN దీర్ఘ	p_wmw;		// 'winminwidth'
EXTERN దీర్ఘ	p_wiw;		// 'winwidth'
#నిర్వచితమైతే FEAT_WAYLAND
EXTERN char_u	*p_wse;		// 'wlseat'
EXTERN దీర్ఘ     p_wtm;		// 'wltimeoutlen'
#అయితే_ముగింపు
#ఒకవేళ defined(MSWIN) && defined(FEAT_TERMINAL)
EXTERN char_u	*p_winptydll;	// 'winptydll'
#అయితే_ముగింపు
EXTERN దీర్ఘ	p_wm;		// 'wrapmargin'
EXTERN పూర్ణాంకం	p_ws;		// 'wrapscan'
EXTERN పూర్ణాంకం	p_write;	// 'write'
EXTERN పూర్ణాంకం	p_wa;		// 'writeany'
EXTERN పూర్ణాంకం	p_wb;		// 'writebackup'
EXTERN దీర్ఘ	p_wd;		// 'writedelay'
EXTERN పూర్ణాంకం	p_xtermcodes;	// 'xtermcodes'

/*
 * "indir" values for buffer-local options.
 * These need to be defined globally, so that the BV_COUNT can be used with
 * b_p_scriptID[].
 */
గణన
{
    BV_AI = 0
    , BV_AC
    , BV_AR
    , BV_BH
    , BV_BKC
    , BV_BT
#నిర్వచితమైతే FEAT_QUICKFIX
    , BV_EFM
    , BV_GEFM
    , BV_GP
    , BV_MP
#అయితే_ముగింపు
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
#నిర్వచితమైతే FEAT_DIFF
    , BV_DIA
#అయితే_ముగింపు
    , BV_TSR
#నిర్వచితమైతే BACKSLASH_లోపల_FILENAME
    , BV_CSL
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_COMPL_FUNC
    , BV_CFU
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_కనుగొను_ID
    , BV_DEF
    , BV_INC
#అయితే_ముగింపు
    , BV_EOF
    , BV_EOL
    , BV_FIXEOL
    , BV_EP
    , BV_ET
    , BV_FENC
    , BV_FP
#నిర్వచితమైతే FEAT_EVAL
    , BV_BEXPR
    , BV_FEX
    , BV_FFU
#అయితే_ముగింపు
    , BV_FF
    , BV_FLP
    , BV_FO
#నిర్వచితమైతే HAVE_FSYNC
    , BV_FS
#అయితే_ముగింపు
    , BV_FT
    , BV_IMI
    , BV_IMS
#ఒకవేళ defined(FEAT_EVAL)
    , BV_INDE
    , BV_INDK
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_కనుగొను_ID) && defined(FEAT_EVAL)
    , BV_INEX
#అయితే_ముగింపు
    , BV_INF
    , BV_ISE
    , BV_ISK
#నిర్వచితమైతే FEAT_CRYPT
    , BV_కీలకం
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_KEYMAP
    , BV_KMAP
#అయితే_ముగింపు
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
#నిర్వచితమైతే FEAT_COMPL_FUNC
    , BV_OFU
#అయితే_ముగింపు
    , BV_మార్గం
    , BV_PI
    , BV_QE
    , BV_RO
    , BV_SI
    , BV_SN
#నిర్వచితమైతే FEAT_SYN_HL
    , BV_SMC
    , BV_SYN
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_SPELL
    , BV_SPC
    , BV_SPF
    , BV_SPL
    , BV_SPO
#అయితే_ముగింపు
    , BV_STS
    , BV_SUA
    , BV_SW
    , BV_SWF
#నిర్వచితమైతే FEAT_EVAL
    , BV_TFU
#అయితే_ముగింపు
    , BV_ట్యాగులు
    , BV_TC
#నిర్వచితమైతే FEAT_COMPL_FUNC
    , BV_TSRFU
#అయితే_ముగింపు
    , BV_TS
    , BV_TW
    , BV_TX
    , BV_UDF
    , BV_UL
    , BV_WM
#నిర్వచితమైతే FEAT_TERMINAL
    , BV_TWSL
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_VARTABS
    , BV_VSTS
    , BV_VTS
#అయితే_ముగింపు
    , BV_లెక్క	    // must be the last one
};

/*
 * "indir" values for window-local options.
 * These need to be defined globally, so that the WV_COUNT can be used in the
 * window structure.
 */
గణన
{
    WV_LIST = 0
    , WV_LCS
    , WV_FCS
#నిర్వచితమైతే FEAT_ARABIC
    , WV_ARAB
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_CONCEAL
    , WV_COCU
    , WV_COLE
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_TERMINAL
    , WV_TWK
    , WV_TWS
#అయితే_ముగింపు
    , WV_CRBIND
#నిర్వచితమైతే FEAT_LINEBREAK
    , WV_BRI
    , WV_BRIOPT
#అయితే_ముగింపు
    , WV_WCR
#నిర్వచితమైతే FEAT_DIFF
    , WV_DIFF
#అయితే_ముగింపు
    , WV_EIW
#నిర్వచితమైతే FEAT_FOLDING
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
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_LINEBREAK
    , WV_LBR
#అయితే_ముగింపు
    , WV_NU
    , WV_RNU
    , WV_VE
#నిర్వచితమైతే FEAT_LINEBREAK
    , WV_NUW
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_QUICKFIX)
    , WV_PVW
    , WV_LHI
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_RIGHTLEFT
    , WV_RL
    , WV_RLC
#అయితే_ముగింపు
    , WV_SCBIND
    , WV_SCROLL
    , WV_SMS
    , WV_SISO
    , WV_SO
    , WV_SOP
#నిర్వచితమైతే FEAT_SPELL
    , WV_SPELL
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_SYN_HL
    , WV_CUC
    , WV_CUL
    , WV_CULOPT
    , WV_CC
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_LINEBREAK
    , WV_SBR
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_STL_OPT
    , WV_STL
    , WV_STLO
#అయితే_ముగింపు
    , WV_WFB
    , WV_WFH
    , WV_WFW
    , WV_WHL
    , WV_WRAP
#నిర్వచితమైతే FEAT_SIGNS
    , WV_SCL
#అయితే_ముగింపు
    , WV_లెక్క	    // must be the last one
};

// Value for b_p_ul indicating the global value must be used.
#నిర్వచించు NO_LOCAL_UNDOLEVEL (-123456)

#నిర్వచించు ERR_BUFLEN 80

#అయితే_ముగింపు // _OPTION_H_
