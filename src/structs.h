/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

/*
 * This file contains various definitions of structures that are used by Vim
 */

/*
 * There is something wrong in the SAS compiler that makes typedefs not
 * valid in include files.  Has been fixed in version 6.58.
 */
#ಆದರೆ defined(SASC) && SASC < 658
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ದೀರ್ಘ		linenr_T;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ		colnr_T;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ	short_u;
#ಆದರೆ_ಅಂತ್ಯ

// structure to store a string (including it's length)
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    char_u  *string;		// the string
    size_t  ಉದ್ದಳತೆ;		// length of the string (excluding the NUL)
} string_T;

/*
 * Position in file or buffer.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    linenr_T	lnum;	// line number
    colnr_T	ಲಂಬಸಾಲು;	// column number
    colnr_T	coladd; // extra virtual column
} pos_T;


/*
 * Same, but without coladd.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    linenr_T	lnum;	// line number
    colnr_T	ಲಂಬಸಾಲು;	// column number
} lpos_T;

/*
 * Structure used for growing arrays.
 * This is used to store information that only grows, is deleted all at
 * once, and needs to be accessed by index.  See ga_clear() and ga_grow().
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ growarray
{
    ಪೂರ್ಣಾಂಕ	    ga_len;		    // current number of items used
    ಪೂರ್ಣಾಂಕ	    ga_maxlen;		    // maximum number of items possible
    ಪೂರ್ಣಾಂಕ	    ga_itemsize;	    // sizeof(item)
    ಪೂರ್ಣಾಂಕ	    ga_growsize;	    // number of items to grow each time
    ಶೂನ್ಯ    *ga_ದತ್ತಾಂಶ;		    // pointer to the first item
} garray_T;

#ವ್ಯಾಖ್ಯಾನಿಸು GA_ಖಾಲಿಗಳು    {0, 0, 0, 0, ಖಾಲಿ}

// On rare systems "char" is unsigned, sometimes we really want a signed 8-bit
// value.
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಚಿಹ್ನೆಯುಳ್ಳ ಅಕ್ಷರ	int8_T;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ದ್ವಿಗುಣ		float_T;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ typval_S		typval_T;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ listvar_S	list_T;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ dictvar_S	dict_T;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ partial_S	partial_T;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ blobvar_S	blob_T;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ tuplevar_S	tuple_T;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ generictype_S	generic_T;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ gfargs_tab_S	gfargs_tab_T;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ ಕಿಟಕಿ_S		win_T;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ wininfo_S	wininfo_T;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ ಚೌಕಟ್ಟು_S		ಚೌಕಟ್ಟು_T;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ			scid_T;		// script ID
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ file_ಬಫರ್	buf_T;		// forward declaration
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ terminal_S	ಪದ_T;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ VimMenu vimmenu_T;
#ಆದರೆ_ಅಂತ್ಯ

// maximum value for sc_version
#ವ್ಯಾಖ್ಯಾನಿಸು SCRIPT_ಆವೃತ್ತಿ_MAX 4
// value for sc_version in a Vim9 script file
#ವ್ಯಾಖ್ಯಾನಿಸು SCRIPT_ಆವೃತ್ತಿ_VIM9 999999

/*
 * SCript ConteXt (SCTX): identifies a script line.
 * When sourcing a script "sc_lnum" is zero, "sourcing_lnum" is the current
 * line number. When executing a user function "sc_lnum" is the line where the
 * function was defined, "sourcing_lnum" is the line number inside the
 * function.  When stored with a function, mapping, option, etc. "sc_lnum" is
 * the line number in the script "sc_sid".
 *
 * sc_version is also here, for convenience.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    scid_T	sc_sid;		// script ID
    ಪೂರ್ಣಾಂಕ		sc_seq;		// sourcing sequence number
    linenr_T	sc_lnum;	// line number
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		sc_ಆವೃತ್ತಿ;	// :scriptversion
} sctx_T;

/*
 * Reference to a buffer that stores the value of buf_free_count.
 * bufref_valid() only needs to check "buf" when the count differs.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    buf_T   *br_buf;
    ಪೂರ್ಣಾಂಕ	    br_fnum;
    ಪೂರ್ಣಾಂಕ	    br_buf_free_ಎಣಿಕೆ;
} bufref_T;

/*
 * This is here because regexp.h needs pos_T and below regprog_T is used.
 */
#ಅನ್ತರ್ಭಾವ "regexp.h"

/*
 * This is here because gui.h needs the pos_T and win_T, and win_T needs gui.h
 * for scrollbar_T.
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
# include "gui.h"
#ಇಲ್ಲದಿದ್ದರೆ
# ifdef FEAT_XCLIPBOARD
#  include <X11/Intrinsic.h>
# endif
# define guicolor_T ದೀರ್ಘ
# define INVALCOLOR ((guicolor_T)0x1ffffff)
    // only used for cterm.bg_rgb and cterm.fg_rgb: use cterm color
# define CTERMCOLOR ((guicolor_T)0x1fffffe)
#ಆದರೆ_ಅಂತ್ಯ
// We have include guard in the file
#ಅನ್ತರ್ಭಾವ "image.h"
#ವ್ಯಾಖ್ಯಾನಿಸು COLOR_INVALID(x) ((x) == INVALCOLOR || (x) == CTERMCOLOR)

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
# include "libvterm/include/vterm.h"
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    VTermColor	fg;
    VTermColor	bg;
} termcellcolor_T;
#ಆದರೆ_ಅಂತ್ಯ

/*
 * marks: positions in a file
 * (a normal mark is a lnum/col pair, the same as a file position)
 */

#ವ್ಯಾಖ್ಯಾನಿಸು NMARKS		('z' - 'a' + 1)	// max. # of named marks
#ವ್ಯಾಖ್ಯಾನಿಸು EXTRA_MARKS	10		// marks 0-9
#ವ್ಯಾಖ್ಯಾನಿಸು JUMPLISTSIZE	100		// max. # of marks in jump list
#ವ್ಯಾಖ್ಯಾನಿಸು TAGSTACKSIZE	20		// max. # of tags in tag stack

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ filemark
{
    pos_T	mark;		// cursor position
    ಪೂರ್ಣಾಂಕ		fnum;		// file number
} fmark_T;

// Xtended file mark: also has a file name
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ xfilemark
{
    fmark_T	fmark;
    char_u	*fname;		// file name, used when fnum == 0
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VIMINFO
    ಸಮಯ_T	ಸಮಯ_ಗಣ;
#ಆದರೆ_ಅಂತ್ಯ
} xfmark_T;

/*
 * The taggy struct is used to store the information about a :tag command.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ taggy
{
    char_u	*tagname;	// tag name
    fmark_T	fmark;		// cursor position BEFORE ":tag"
    ಪೂರ್ಣಾಂಕ		cur_ಹೊಂದಿಕೆ;	// match number
    ಪೂರ್ಣಾಂಕ		cur_fnum;	// buffer number used for cur_match
    char_u	*user_ದತ್ತಾಂಶ;	// used with tagfunc
} taggy_T;

/*
 * Structure that contains all options that are local to a window.
 * Used twice in a window: for the current buffer and for all buffers.
 * Also used in wininfo_T.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ARABIC
    ಪೂರ್ಣಾಂಕ		wo_arab;
# define w_p_arab w_onebuf_opt.wo_arab	// 'arabic'
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
    ಪೂರ್ಣಾಂಕ		wo_bri;
# define w_p_bri w_onebuf_opt.wo_bri	// 'breakindent'
    char_u	*wo_briopt;
# define w_p_briopt w_onebuf_opt.wo_briopt // 'breakindentopt'
#ಆದರೆ_ಅಂತ್ಯ
    char_u	*wo_wcr;
#ವ್ಯಾಖ್ಯಾನಿಸು w_p_wcr w_onebuf_opt.wo_wcr	// 'wincolor'
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
    ಪೂರ್ಣಾಂಕ		wo_diff;
# define w_p_diff w_onebuf_opt.wo_diff	// 'diff'
#ಆದರೆ_ಅಂತ್ಯ
    char_u	*wo_eiw;
#ವ್ಯಾಖ್ಯಾನಿಸು w_p_eiw w_onebuf_opt.wo_eiw	// 'eventignorewin'
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
    ದೀರ್ಘ	wo_fdc;
# define w_p_fdc w_onebuf_opt.wo_fdc	// 'foldcolumn'
    ಪೂರ್ಣಾಂಕ		wo_fdc_save;
# define w_p_fdc_save w_onebuf_opt.wo_fdc_save	// 'foldenable' saved for diff mode
    ಪೂರ್ಣಾಂಕ		wo_fen;
# define w_p_fen w_onebuf_opt.wo_fen	// 'foldenable'
    ಪೂರ್ಣಾಂಕ		wo_fen_save;
# define w_p_fen_save w_onebuf_opt.wo_fen_save	// 'foldenable' saved for diff mode
    char_u	*wo_fdi;
# define w_p_fdi w_onebuf_opt.wo_fdi	// 'foldignore'
    ದೀರ್ಘ	wo_fdl;
# define w_p_fdl w_onebuf_opt.wo_fdl	// 'foldlevel'
    ಪೂರ್ಣಾಂಕ		wo_fdl_save;
# define w_p_fdl_save w_onebuf_opt.wo_fdl_save	// 'foldlevel' state saved for diff mode
    char_u	*wo_fdm;
# define w_p_fdm w_onebuf_opt.wo_fdm	// 'foldmethod'
    char_u	*wo_fdm_save;
# define w_p_fdm_save w_onebuf_opt.wo_fdm_save	// 'fdm' saved for diff mode
    ದೀರ್ಘ	wo_fml;
# define w_p_fml w_onebuf_opt.wo_fml	// 'foldminlines'
    ದೀರ್ಘ	wo_fdn;
# define w_p_fdn w_onebuf_opt.wo_fdn	// 'foldnestmax'
# ifdef FEAT_EVAL
    char_u	*wo_fde;
#  define w_p_fde w_onebuf_opt.wo_fde	// 'foldexpr'
    char_u	*wo_fdt;
#  define w_p_fdt w_onebuf_opt.wo_fdt	// 'foldtext'
# endif
    char_u	*wo_fmr;
# define w_p_fmr w_onebuf_opt.wo_fmr	// 'foldmarker'
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
    ಪೂರ್ಣಾಂಕ		wo_lbr;
# define w_p_lbr w_onebuf_opt.wo_lbr	// 'linebreak'
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		wo_list;
#ವ್ಯಾಖ್ಯಾನಿಸು w_p_list w_onebuf_opt.wo_list	// 'list'
    char_u	*wo_lcs;
#ವ್ಯಾಖ್ಯಾನಿಸು w_p_lcs w_onebuf_opt.wo_lcs	// 'listchars'
    char_u	*wo_fcs;
#ವ್ಯಾಖ್ಯಾನಿಸು w_p_fcs w_onebuf_opt.wo_fcs	// 'fillchars'
    ಪೂರ್ಣಾಂಕ		wo_nu;
#ವ್ಯಾಖ್ಯಾನಿಸು w_p_nu w_onebuf_opt.wo_nu	// 'number'
    ಪೂರ್ಣಾಂಕ		wo_rnu;
#ವ್ಯಾಖ್ಯಾನಿಸು w_p_rnu w_onebuf_opt.wo_rnu	// 'relativenumber'
    char_u	*wo_ve;
#ವ್ಯಾಖ್ಯಾನಿಸು w_p_ve w_onebuf_opt.wo_ve	// 'virtualedit'
    ಚಿಹ್ನೆರಹಿತ	wo_ve_flags;
#ವ್ಯಾಖ್ಯಾನಿಸು	w_ve_flags w_onebuf_opt.wo_ve_flags	// flags for 'virtualedit'
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
    ದೀರ್ಘ	wo_nuw;
# define w_p_nuw w_onebuf_opt.wo_nuw	// 'numberwidth'
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ wo_wfb;
#ವ್ಯಾಖ್ಯಾನಿಸು w_p_wfb w_onebuf_opt.wo_wfb	// 'winfixbuf'
    ಪೂರ್ಣಾಂಕ		wo_wfh;
#ವ್ಯಾಖ್ಯಾನಿಸು w_p_wfh w_onebuf_opt.wo_wfh	// 'winfixheight'
    ಪೂರ್ಣಾಂಕ		wo_wfw;
#ವ್ಯಾಖ್ಯಾನಿಸು w_p_wfw w_onebuf_opt.wo_wfw	// 'winfixwidth'
#ಆದರೆ defined(FEAT_QUICKFIX)
    ಪೂರ್ಣಾಂಕ		wo_pvw;
# define w_p_pvw w_onebuf_opt.wo_pvw	// 'previewwindow'
    ದೀರ್ಘ        wo_lhi;
# define w_p_lhi w_onebuf_opt.wo_lhi    // 'lhistory'
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
    ಪೂರ್ಣಾಂಕ		wo_rl;
# define w_p_rl w_onebuf_opt.wo_rl	// 'rightleft'
    char_u	*wo_rlc;
# define w_p_rlc w_onebuf_opt.wo_rlc	// 'rightleftcmd'
#ಆದರೆ_ಅಂತ್ಯ
    ದೀರ್ಘ	wo_scr;
#ವ್ಯಾಖ್ಯಾನಿಸು w_p_scr w_onebuf_opt.wo_scr	// 'scroll'
    ಪೂರ್ಣಾಂಕ		wo_sms;
#ವ್ಯಾಖ್ಯಾನಿಸು w_p_sms w_onebuf_opt.wo_sms	// 'smoothscroll'
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SPELL
    ಪೂರ್ಣಾಂಕ		wo_spell;
# define w_p_spell w_onebuf_opt.wo_spell // 'spell'
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_SYN_HL) || defined(FEAT_FOLDING) || defined(FEAT_DIFF)
    ಪೂರ್ಣಾಂಕ		wo_cuc;
# define w_p_cuc w_onebuf_opt.wo_cuc	// 'cursorcolumn'
    ಪೂರ್ಣಾಂಕ		wo_cul;
# define w_p_cul w_onebuf_opt.wo_cul	// 'cursorline'
    char_u	*wo_culopt;
# define w_p_culopt w_onebuf_opt.wo_culopt	// 'cursorlineopt'
    char_u	*wo_cc;
# define w_p_cc w_onebuf_opt.wo_cc	// 'colorcolumn'
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
    char_u	*wo_sbr;
# define w_p_sbr w_onebuf_opt.wo_sbr	// 'showbreak'
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_STL_OPT
    char_u	*wo_stl;
# define w_p_stl w_onebuf_opt.wo_stl	// 'statusline'
    char_u	*wo_stlo;
# define w_p_stlo w_onebuf_opt.wo_stlo	// 'statuslineopt'
    ಪೂರ್ಣಾಂಕ		wo_stlo_fh;
# define w_p_stlo_fh w_onebuf_opt.wo_stlo_fh // "fixedheight" of 'statuslineopt'
    ಪೂರ್ಣಾಂಕ		wo_stlo_mh;
# define w_p_stlo_mh w_onebuf_opt.wo_stlo_mh // "maxheight:" of 'statuslineopt'
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		wo_scb;
#ವ್ಯಾಖ್ಯಾನಿಸು w_p_scb w_onebuf_opt.wo_scb	// 'scrollbind'
    ಪೂರ್ಣಾಂಕ		wo_diff_saved; // options were saved for starting diff mode
#ವ್ಯಾಖ್ಯಾನಿಸು w_p_diff_saved w_onebuf_opt.wo_diff_saved
    ಪೂರ್ಣಾಂಕ		wo_scb_save;	// 'scrollbind' saved for diff mode
#ವ್ಯಾಖ್ಯಾನಿಸು w_p_scb_save w_onebuf_opt.wo_scb_save
    ಪೂರ್ಣಾಂಕ		wo_wrap;
#ವ್ಯಾಖ್ಯಾನಿಸು w_p_wrap w_onebuf_opt.wo_wrap	// 'wrap'
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
    ಪೂರ್ಣಾಂಕ		wo_wrap_save;	// 'wrap' state saved for diff mode
# define w_p_wrap_save w_onebuf_opt.wo_wrap_save
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CONCEAL
    char_u	*wo_cocu;		// 'concealcursor'
# define w_p_cocu w_onebuf_opt.wo_cocu
    ದೀರ್ಘ	wo_cole;		// 'conceallevel'
# define w_p_cole w_onebuf_opt.wo_cole
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		wo_crb;
#ವ್ಯಾಖ್ಯಾನಿಸು w_p_crb w_onebuf_opt.wo_crb	// 'cursorbind'
    ಪೂರ್ಣಾಂಕ		wo_crb_save;	// 'cursorbind' state saved for diff mode
#ವ್ಯಾಖ್ಯಾನಿಸು w_p_crb_save w_onebuf_opt.wo_crb_save
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SIGNS
    char_u	*wo_scl;
# define w_p_scl w_onebuf_opt.wo_scl	// 'signcolumn'
#ಆದರೆ_ಅಂತ್ಯ
    ದೀರ್ಘ	wo_siso;
#ವ್ಯಾಖ್ಯಾನಿಸು w_p_siso w_onebuf_opt.wo_siso	// 'sidescrolloff' local value
    ದೀರ್ಘ	wo_so;
#ವ್ಯಾಖ್ಯಾನಿಸು w_p_so w_onebuf_opt.wo_so	// 'scrolloff' local value
    ದೀರ್ಘ	wo_sop;
#ವ್ಯಾಖ್ಯಾನಿಸು w_p_sop w_onebuf_opt.wo_sop  // 'scrolloffpad' local value
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
    char_u	*wo_twk;
# define w_p_twk w_onebuf_opt.wo_twk	// 'termwinkey'
    char_u	*wo_tws;
# define w_p_tws w_onebuf_opt.wo_tws	// 'termwinsize'
#ಆದರೆ_ಅಂತ್ಯ

    // A few options have local flags for P_INSECURE.
    long_u	wo_wrap_flags;		// flags for 'wrap'
#ವ್ಯಾಖ್ಯಾನಿಸು w_p_wrap_flags w_onebuf_opt.wo_wrap_flags
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_STL_OPT
    long_u	wo_stl_flags;		// flags for 'statusline'
# define w_p_stl_flags w_onebuf_opt.wo_stl_flags
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    long_u	wo_fde_flags;		// flags for 'foldexpr'
# define w_p_fde_flags w_onebuf_opt.wo_fde_flags
    long_u	wo_fdt_flags;		// flags for 'foldtext'
# define w_p_fdt_flags w_onebuf_opt.wo_fdt_flags
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    sctx_T	wo_script_ctx[WV_ಎಣಿಕೆ];	// SCTXs for window-local options
# define w_p_script_ctx w_onebuf_opt.wo_script_ctx
#ಆದರೆ_ಅಂತ್ಯ
    char_u	*wo_whl;
#ವ್ಯಾಖ್ಯಾನಿಸು w_p_whl w_onebuf_opt.wo_whl	// 'winhighlight'
} winopt_T;

/*
 * Window info stored with a buffer.
 *
 * Two types of info are kept for a buffer which are associated with a
 * specific window:
 * 1. Each window can have a different line number associated with a buffer.
 * 2. The window-local options for a buffer work in a similar way.
 * The window-info is kept in a list at b_wininfo.  It is kept in
 * most-recently-used order.
 */
ರಚನೆ wininfo_S
{
    wininfo_T	*wi_ಮುಂದಿನ;	// next entry or NULL for last entry
    wininfo_T	*wi_prev;	// previous entry or NULL for first entry
    win_T	*wi_win;	// pointer to window that did set wi_fpos
    pos_T	wi_fpos;	// last cursor position in the file
    winopt_T	wi_opt;		// local window options
    ಪೂರ್ಣಾಂಕ		wi_optset;	// TRUE when wi_opt has useful values
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
    ಪೂರ್ಣಾಂಕ		wi_fold_manual;	// copy of w_fold_manual
    garray_T	wi_folds;	// clone of w_folds
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		wi_changelistidx; // copy of w_changelistidx
};

/*
 * Info used to pass info about a fold from the fold-detection code to the
 * code that displays the foldcolumn.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ foldinfo
{
    ಪೂರ್ಣಾಂಕ		fi_level;	// level of the fold; when this is zero the
				// other fields are invalid
    ಪೂರ್ಣಾಂಕ		fi_lnum;	// line number where fold starts
    ಪೂರ್ಣಾಂಕ		fi_ಕೆಳಗಿನ_level;	// lowest fold level that starts in the same
				//ಸಾಲು
} foldinfo_T;

/*
 * Structure to store info about the Visual area.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    pos_T	vi_ಆರಂಭ;	// start pos of last VIsual
    pos_T	vi_ಕೊನೆ;		// end position of last VIsual
    ಪೂರ್ಣಾಂಕ		vi_ಬಹುಲಕ;	// VIsual_mode of last VIsual
    colnr_T	vi_curswant;	// MAXCOL from w_curswant
} visualinfo_T;

/*
 * structures used for undo
 */

// One line saved for undo.  After the NUL terminated text there might be text
// properties, thus ul_len can be larger than STRLEN(ul_line) + 1.
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    char_u	*ul_ಸಾಲು;	// text of the line
    ದೀರ್ಘ	ul_len;		// length of the line including NUL, plus text
				// properties
    colnr_T	ul_textlen;	// length of the line excluding NUL and any text
				// properties
} undoline_T;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ u_ನಮೂದು u_ನಮೂದು_T;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ u_ಶೀರ್ಷಿಕೆ u_ಶೀರ್ಷಿಕೆ_T;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
// Which line around an undo block a cleared continuation flag was on.
# define UNDOPROP_NONE	0
# define UNDOPROP_ABOVE	1
# define UNDOPROP_ಕೆಳಗೆ	2

// A continuation flag cleared on a line around an undo block, given back when
// the block is undone or redone the other way.
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    ಪೂರ್ಣಾಂಕ		ಮೇಲೆ_which;	// UNDOPROP_ABOVE or UNDOPROP_BELOW
    ಪೂರ್ಣಾಂಕ		ಮೇಲೆ_type;	// tp_type of the property
    ಪೂರ್ಣಾಂಕ		ಮೇಲೆ_id;		// tp_id of the property
    colnr_T	ಮೇಲೆ_ಲಂಬಸಾಲು;		// tp_col of the property
    ಪೂರ್ಣಾಂಕ		ಮೇಲೆ_flag;	// TP_FLAG_CONT_NEXT or TP_FLAG_CONT_PREV
} undoprop_T;
#ಆದರೆ_ಅಂತ್ಯ

ರಚನೆ u_ನಮೂದು
{
    u_ನಮೂದು_T	*ue_ಮುಂದಿನ;	// pointer to next entry in list
    linenr_T	ue_ತುದಿ;		// number of line above undo block
    linenr_T	ue_bot;		// number of line below undo block
    linenr_T	ue_lcount;	// linecount when u_save called
    undoline_T	*ue_array;	// array of lines in undo block
    ದೀರ್ಘ	ue_ಗಾತ್ರ;	// number of lines in ue_array
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
    garray_T	ue_props;	// undoprop_T: flags cleared around the block
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ U_DEBUG
    ಪೂರ್ಣಾಂಕ		ue_magic;	// magic number to check allocation
#ಆದರೆ_ಅಂತ್ಯ
};

ರಚನೆ u_ಶೀರ್ಷಿಕೆ
{
    // The following have a pointer and a number. The number is used when
    // reading the undo file in u_read_undo()
    ಒಕ್ಕೂಟ {
	u_ಶೀರ್ಷಿಕೆ_T *ptr;	// pointer to next undo header in list
	ದೀರ್ಘ	   seq;
    } uh_ಮುಂದಿನ;
    ಒಕ್ಕೂಟ {
	u_ಶೀರ್ಷಿಕೆ_T *ptr;	// pointer to previous header in list
	ದೀರ್ಘ	   seq;
    } uh_prev;
    ಒಕ್ಕೂಟ {
	u_ಶೀರ್ಷಿಕೆ_T *ptr;	// pointer to next header for alt. redo
	ದೀರ್ಘ	   seq;
    } uh_alt_ಮುಂದಿನ;
    ಒಕ್ಕೂಟ {
	u_ಶೀರ್ಷಿಕೆ_T *ptr;	// pointer to previous header for alt. redo
	ದೀರ್ಘ	   seq;
    } uh_alt_prev;
    ದೀರ್ಘ	uh_seq;		// sequence number, higher == newer undo
    ಪೂರ್ಣಾಂಕ		uh_walk;	// used by undo_time()
    u_ನಮೂದು_T	*uh_ನಮೂದು;	// pointer to first entry
    u_ನಮೂದು_T	*uh_getbot_ನಮೂದು; // pointer to where ue_bot must be set
    pos_T	uh_cursor;	// cursor position before saving
    ದೀರ್ಘ	uh_cursor_vcol;
    ಪೂರ್ಣಾಂಕ		uh_flags;	// see below
    pos_T	uh_namedm[NMARKS];	// marks before undo/after redo
    visualinfo_T uh_visual;	// Visual areas before undo/after redo
    ಸಮಯ_T	uh_ಸಮಯ;	// timestamp when the change was made
    ದೀರ್ಘ	uh_save_nr;	// set when the file was saved after the
				// changes in this block
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ U_DEBUG
    ಪೂರ್ಣಾಂಕ		uh_magic;	// magic number to check allocation
#ಆದರೆ_ಅಂತ್ಯ
};

// values for uh_flags
#ವ್ಯಾಖ್ಯಾನಿಸು UH_ಬದಲಾಯಿತು  0x01	// b_changed flag before undo/after redo
#ವ್ಯಾಖ್ಯಾನಿಸು UH_EMPTYBUF 0x02	// buffer was empty

/*
 * structures used in undo.c
 */
#ವ್ಯಾಖ್ಯಾನಿಸು ALIGN_LONG	// longword alignment and use filler byte
#ವ್ಯಾಖ್ಯಾನಿಸು ALIGN_ಗಾತ್ರ (ಗಾತ್ರ(ದೀರ್ಘ))

#ವ್ಯಾಖ್ಯಾನಿಸು ALIGN_ಮುಖವಾಡ (ALIGN_ಗಾತ್ರ - 1)

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ m_info minfo_T;

/*
 * structure used to link chunks in one of the free chunk lists.
 */
ರಚನೆ m_info
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ALIGN_LONG
    long_u	m_ಗಾತ್ರ_2;		// size of the chunk (including m_info)
#ಇಲ್ಲದಿದ್ದರೆ
    short_u	m_ಗಾತ್ರ_2;		// size of the chunk (including m_info)
#ಆದರೆ_ಅಂತ್ಯ
    minfo_T	*m_ಮುಂದಿನ;	// pointer to next free chunk in the list
};

/*
 * things used in memfile.c
 */

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ ಖಂಡ_hdr    bhdr_T;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ memfile	    memfile_T;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ದೀರ್ಘ		    blocknr_T;

/*
 * mf_hashtab_T is a chained hashtable with blocknr_T key and arbitrary
 * structures as items.  This is an intrusive data structure: we require
 * that items begin with mf_hashitem_T which contains the key and linked
 * list pointers.  List of items in each bucket is doubly-linked.
 */

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ mf_hashitem_S mf_hashitem_T;

ರಚನೆ mf_hashitem_S
{
    mf_hashitem_T   *mhi_ಮುಂದಿನ;
    mf_hashitem_T   *mhi_prev;
    blocknr_T	    mhi_ಕೀಲಿ;
};

#ವ್ಯಾಖ್ಯಾನಿಸು MHT_INIT_ಗಾತ್ರ   64

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ mf_hashtab_S
{
    long_u	    mht_ಮುಖವಾಡ;	    // mask used for hash value (nr of items
				    // in array is "mht_mask" + 1)
    long_u	    mht_ಎಣಿಕೆ;	    // nr of items inserted into hashtable
    mf_hashitem_T   **mht_buckets;  // points to mht_small_buckets or
				    //dynamically allocated array
    mf_hashitem_T   *mht_small_buckets[MHT_INIT_ಗಾತ್ರ];   // initial buckets
    ಅಕ್ಷರ	    mht_fixed;	    // non-zero value forbids growth
} mf_hashtab_T;

/*
 * for each (previously) used block in the memfile there is one block header.
 *
 * The block may be linked in the used list OR in the free list.
 * The used blocks are also kept in hash lists.
 *
 * The used list is a doubly linked list, most recently used block first.
 *	The blocks in the used list have a block of memory allocated.
 *	mf_used_count is the number of pages in the used list.
 * The hash lists are used to quickly find a block in the used list.
 * The free list is a single linked list, not sorted.
 *	The blocks in the free list have no block of memory allocated and
 *	the contents of the block in the file (if any) is irrelevant.
 */

ರಚನೆ ಖಂಡ_hdr
{
    mf_hashitem_T bh_hashitem;      // header for hash table and key
#ವ್ಯಾಖ್ಯಾನಿಸು bh_bnum bh_hashitem.mhi_ಕೀಲಿ // block number, part of bh_hashitem

    bhdr_T	*bh_ಮುಂದಿನ;	    // next block_hdr in free or used list
    bhdr_T	*bh_prev;	    // previous block_hdr in used list
    char_u	*bh_ದತ್ತಾಂಶ;	    // pointer to memory (for used block)
    ಪೂರ್ಣಾಂಕ		bh_ಪುಟ_ಎಣಿಕೆ;	    // number of pages in this block

#ವ್ಯಾಖ್ಯಾನಿಸು BH_DIRTY    1
#ವ್ಯಾಖ್ಯಾನಿಸು BH_LOCKED   2
    ಅಕ್ಷರ	bh_flags;	    // BH_DIRTY or BH_LOCKED
};

/*
 * when a block with a negative number is flushed to the file, it gets
 * a positive number. Because the reference to the block is still the negative
 * number, we remember the translation to the new positive number in the
 * double linked trans lists. The structure is the same as the hash lists.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ nr_trans NR_TRANS;

ರಚನೆ nr_trans
{
    mf_hashitem_T nt_hashitem;		// header for hash table and key
#ವ್ಯಾಖ್ಯಾನಿಸು nt_old_bnum nt_hashitem.mhi_ಕೀಲಿ	// old, negative, number

    blocknr_T	nt_new_bnum;		// new, positive, number
};


ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ buffblock buffblock_T;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ buffheader buffheader_T;

/*
 * structure used to store one block of the stuff/redo/recording buffers
 */
ರಚನೆ buffblock
{
    buffblock_T	*b_ಮುಂದಿನ;	// pointer to next buffblock
    size_t	b_strlen;	// length of b_str, excluding the NUL
    char_u	b_str[1];	// contents (actually longer)
};

/*
 * header used for the stuff buffer and the redo buffer
 */
ರಚನೆ buffheader
{
    buffblock_T	bh_ಮೊದಲ;	// first (dummy) block of list
    buffblock_T	*bh_curr;	// buffblock for appending
    ಪೂರ್ಣಾಂಕ		bh_ಸೂಚ್ಯಂಕ;	// index for reading
    ಪೂರ್ಣಾಂಕ		bh_space;	// space in bh_curr for appending
    ಪೂರ್ಣಾಂಕ		bh_create_newblock;	// create a new block?
};

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    buffheader_T sr_redobuff;
    buffheader_T sr_old_redobuff;
} save_redo_T;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
    XP_ಪೂರ್ವಪ್ರತ್ಯಯ_NONE,	// prefix not used
    XP_ಪೂರ್ವಪ್ರತ್ಯಯ_NO,	// "no" prefix for bool option
    XP_ಪೂರ್ವಪ್ರತ್ಯಯ_INV,	// "inv" prefix for bool option
} xp_ಪೂರ್ವಪ್ರತ್ಯಯ_T;

/*
 * :set operator types
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
    OP_NONE = 0,
    OP_ADDING,		// "opt+=arg"
    OP_PREPENDING,	// "opt^=arg"
    OP_REMOVING,	// "opt-=arg"
} ಗಣ_op_T;

/*
 * used for completion on the command line
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ expand
{
    char_u	*xp_ನಮೂನೆ;		// start of item to expand, guaranteed
					// to be part of xp_line
    ಪೂರ್ಣಾಂಕ		xp_context;		// type of expansion
    ಪೂರ್ಣಾಂಕ		xp_ನಮೂನೆ_len;		// bytes in xp_pattern before cursor
    xp_ಪೂರ್ವಪ್ರತ್ಯಯ_T	xp_ಪೂರ್ವಪ್ರತ್ಯಯ;
#ಆದರೆ defined(FEAT_EVAL)
    char_u	*xp_arg;		// completion function
    ಪೂರ್ಣಾಂಕ		xp_complete_opt;	// UCC_ flags for user command
    sctx_T	xp_script_ctx;		// SCTX for completion function
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		xp_backslash;		// one of the XP_BS_ values
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ BACKSLASH_ಒಳ_FILENAME
    ಪೂರ್ಣಾಂಕ		xp_shell;		// TRUE for a shell command, more
					// characters need to be escaped
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		xp_numfiles;		// number of files found by
					// file name completion
    ಪೂರ್ಣಾಂಕ		xp_ಲಂಬಸಾಲು;			// cursor position in line
    ಪೂರ್ಣಾಂಕ		xp_selected;		// selected index in completion
    char_u	*xp_orig;		// originally expanded string
    char_u	**xp_files;		// list of files
    char_u	**xp_files_abbr;	// optional parallel array of display
					// strings (override xp_files for the
					// pum text); NULL if unused
    char_u	**xp_files_ಬಗೆ;	// optional parallel array of "kind"
					// strings; NULL if unused
    char_u	**xp_files_menu;	// optional parallel array of "menu"
					// strings (shown after the match);
					// NULL if unused
    char_u	**xp_files_info;	// optional parallel array of "info"
					// strings (shown in info popup);
					// NULL if unused
    char_u	*xp_ಸಾಲು;		// text being completed
#ವ್ಯಾಖ್ಯಾನಿಸು EXPAND_BUF_LEN 256
    char_u	xp_buf[EXPAND_BUF_LEN];	// buffer for returned match
    ಪೂರ್ಣಾಂಕ		xp_ಹುಡುಕು_dir;		// Direction of search
    pos_T	xp_pre_incsearch_pos;	// Cursor position before incsearch
} expand_T;

/*
 * values for xp_backslash
 */
#ವ್ಯಾಖ್ಯಾನಿಸು XP_BS_NONE	0	// nothing special for backslashes
#ವ್ಯಾಖ್ಯಾನಿಸು XP_BS_ONE	0x1	// uses one backslash before a space
#ವ್ಯಾಖ್ಯಾನಿಸು XP_BS_THREE	0x2	// uses three backslashes before a space
#ವ್ಯಾಖ್ಯಾನಿಸು XP_BS_COMMA	0x4	// commas need to be escaped with a backslash

/*
 * Variables shared between getcmdline(), redrawcmdline() and others.
 * These need to be saved when using CTRL-R |, that's why they are in a
 * structure.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    char_u	*cmdbuff;	// pointer to command line buffer
    ಪೂರ್ಣಾಂಕ		cmdbufflen;	// length of cmdbuff
    ಪೂರ್ಣಾಂಕ		cmdlen;		// number of chars in command line
    ಪೂರ್ಣಾಂಕ		cmdpos;		// current cursor position
    ಪೂರ್ಣಾಂಕ		cmdspos;	// cursor column on screen
    ಪೂರ್ಣಾಂಕ		cmdfirstc;	// ':', '/', '?', '=', '>' or NUL
    ಪೂರ್ಣಾಂಕ		cmdindent;	// number of spaces before cmdline
    char_u	*cmdprompt;	// message in front of cmdline
    ಪೂರ್ಣಾಂಕ		cmdattr;	// attributes for prompt
    ಪೂರ್ಣಾಂಕ		overstrike;	// Typing mode on the command line.  Shared by
				// getcmdline() and put_on_cmdline().
    expand_T	*xpc;		// struct being used for expansion, xp_pattern
				// may point into cmdbuff
    ಪೂರ್ಣಾಂಕ		xp_context;	// type of expansion
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    char_u	*xp_arg;	// user-defined expansion arg
    ಪೂರ್ಣಾಂಕ		ಒಳಹರಿವು_fn;	// when TRUE Invoked for input() function
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		cmdbuff_replaced; // when TRUE cmdline was replaced externally
				  // (e.g. by setcmdline())
} cmdline_info_T;

/*
 * Command modifiers ":vertical", ":browse", ":confirm" and ":hide" set a flag.
 * This needs to be saved for recursive commands, put them in a structure for
 * easy manipulation.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    ಪೂರ್ಣಾಂಕ		cmod_flags;		// CMOD_ flags
#ವ್ಯಾಖ್ಯಾನಿಸು CMOD_SANDBOX	    0x0001	// ":sandbox"
#ವ್ಯಾಖ್ಯಾನಿಸು CMOD_SILENT	    0x0002	// ":silent"
#ವ್ಯಾಖ್ಯಾನಿಸು CMOD_ERRSILENT	    0x0004	// ":silent!"
#ವ್ಯಾಖ್ಯಾನಿಸು CMOD_UNSILENT	    0x0008	// ":unsilent"
#ವ್ಯಾಖ್ಯಾನಿಸು CMOD_NOAUTOCMD	    0x0010	// ":noautocmd"
#ವ್ಯಾಖ್ಯಾನಿಸು CMOD_HIDE	    0x0020	// ":hide"
#ವ್ಯಾಖ್ಯಾನಿಸು CMOD_BROWSE	    0x0040	// ":browse" - invoke file dialog
#ವ್ಯಾಖ್ಯಾನಿಸು CMOD_CONFIRM	    0x0080	// ":confirm" - invoke yes/no dialog
#ವ್ಯಾಖ್ಯಾನಿಸು CMOD_KEEPALT	    0x0100	// ":keepalt"
#ವ್ಯಾಖ್ಯಾನಿಸು CMOD_KEEPMARKS	    0x0200	// ":keepmarks"
#ವ್ಯಾಖ್ಯಾನಿಸು CMOD_KEEPJUMPS	    0x0400	// ":keepjumps"
#ವ್ಯಾಖ್ಯಾನಿಸು CMOD_LOCKMARKS	    0x0800	// ":lockmarks"
#ವ್ಯಾಖ್ಯಾನಿಸು CMOD_KEEPPATTERNS   0x1000	// ":keeppatterns"
#ವ್ಯಾಖ್ಯಾನಿಸು CMOD_NOSWAPFILE	    0x2000	// ":noswapfile"
#ವ್ಯಾಖ್ಯಾನಿಸು CMOD_VIM9CMD	    0x4000	// ":vim9cmd"
#ವ್ಯಾಖ್ಯಾನಿಸು CMOD_LEGACY	    0x8000	// ":legacy"

    ಪೂರ್ಣಾಂಕ		cmod_ವಿಭಜಿಸು;		// flags for win_split()
    ಪೂರ್ಣಾಂಕ		cmod_tab;		// > 0 when ":tab" was used
    regmatch_T	cmod_filter_regmatch;	// set by :filter /pat/
    ಪೂರ್ಣಾಂಕ		cmod_filter_force;	// set for :filter!

    ಪೂರ್ಣಾಂಕ		cmod_verbose;		// 0 if not set, > 0 to set 'verbose'
					// to cmod_verbose - 1

    // values for undo_cmdmod()
    char_u	*cmod_save_ei;		// saved value of 'eventignore'
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SANDBOX
    ಪೂರ್ಣಾಂಕ		cmod_did_sandbox;	// set when "sandbox" was incremented
#ಆದರೆ_ಅಂತ್ಯ
    ದೀರ್ಘ	cmod_verbose_save;	// if 'verbose' was set: value of
					// p_verbose plus one
    ಪೂರ್ಣಾಂಕ		cmod_save_ಸಂದೇಶ_silent;	// if non-zero: saved value of
					// msg_silent + 1
    ಪೂರ್ಣಾಂಕ		cmod_save_ಸಂದೇಶ_scroll;	// for restoring msg_scroll
    ಪೂರ್ಣಾಂಕ		cmod_did_esilent;	// incremented when emsg_silent is
} cmdmod_T;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
    MF_DIRTY_NO = 0,		// no dirty blocks
    MF_DIRTY_YES,		// there are dirty blocks
    MF_DIRTY_YES_NOSYNC,	// there are dirty blocks, do not sync yet
} mfdirty_T;

#ವ್ಯಾಖ್ಯಾನಿಸು MF_ಬೀಜ_LEN	8

ರಚನೆ memfile
{
    char_u	*mf_fname;		// name of the file
    char_u	*mf_ffname;		// idem, full path
    ಪೂರ್ಣಾಂಕ		mf_fd;			// file descriptor
    ಪೂರ್ಣಾಂಕ		mf_flags;		// flags used when opening this memfile
    ಪೂರ್ಣಾಂಕ		mf_reopen;		// mf_fd was closed, retry opening
    bhdr_T	*mf_free_ಮೊದಲ;		// first block_hdr in free list
    bhdr_T	*mf_ಬಳಸಿದ_ಮೊದಲ;		// mru block_hdr in used list
    bhdr_T	*mf_ಬಳಸಿದ_ಕೊನೆಯ;		// lru block_hdr in used list
    ಚಿಹ್ನೆರಹಿತ	mf_ಬಳಸಿದ_ಎಣಿಕೆ;		// number of pages in used list
    ಚಿಹ್ನೆರಹಿತ	mf_ಬಳಸಿದ_ಎಣಿಕೆ_max;	// maximum number of pages in memory
    mf_hashtab_T mf_hash;		// hash lists
    mf_hashtab_T mf_trans;		// trans lists
    blocknr_T	mf_blocknr_max;		// highest positive block number + 1
    blocknr_T	mf_blocknr_min;		// lowest negative block number - 1
    blocknr_T	mf_neg_ಎಣಿಕೆ;		// number of negative blocks numbers
    blocknr_T	mf_infile_ಎಣಿಕೆ;	// number of pages in the file
    ಚಿಹ್ನೆರಹಿತ	mf_ಪುಟ_ಗಾತ್ರ;		// number of bytes in a page
    mfdirty_T	mf_dirty;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CRYPT
    buf_T	*mf_ಬಫರ್;		// buffer this memfile is for
    char_u	mf_ಬೀಜ[MF_ಬೀಜ_LEN];	// seed for encryption

    // Values for key, method and seed used for reading data blocks when
    // updating for a newly set key or method. Only when mf_old_key != NULL.
    char_u	*mf_old_ಕೀಲಿ;
    ಪೂರ್ಣಾಂಕ		mf_old_cm;
    char_u	mf_old_ಬೀಜ[MF_ಬೀಜ_LEN];
#ಆದರೆ_ಅಂತ್ಯ
};

/*
 * things used in memline.c
 */
/*
 * When searching for a specific line, we remember what blocks in the tree
 * are the branches leading to that block. This is stored in ml_stack.  Each
 * entry is a pointer to info in a block (may be data block or pointer block)
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ info_ಸೂಚಕ
{
    blocknr_T	ip_bnum;	// block number
    linenr_T	ip_ಕೆಳಗಿನ;		// lowest lnum in this block
    linenr_T	ip_ಮೇಲಿನ;	// highest lnum in this block
    ಪೂರ್ಣಾಂಕ		ip_ಸೂಚ್ಯಂಕ;	// index for block with current lnum
} infoptr_T;	// block/index pair

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_BYTEOFF
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ ml_chunksize
{
    ಪೂರ್ಣಾಂಕ		mlcs_numlines;
    ದೀರ್ಘ	mlcs_totalsize;
} chunksize_T;

/*
 * Flags when calling ml_updatechunk()
 */
# define ML_CHNK_ADDLINE 1
# define ML_CHNK_DELLINE 2
# define ML_CHNK_UPDLINE 3
#ಆದರೆ_ಅಂತ್ಯ

/*
 * the memline structure holds all the information about a memline
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ memline
{
    linenr_T	ml_ಸಾಲು_ಎಣಿಕೆ;	// number of lines in the buffer

    memfile_T	*ml_mfp;	// pointer to associated memfile

    infoptr_T	*ml_ರಾಶಿ;	// stack of pointer blocks (array of IPTRs)
    ಪೂರ್ಣಾಂಕ		ml_ರಾಶಿ_ತುದಿ;	// current top of ml_stack
    ಪೂರ್ಣಾಂಕ		ml_ರಾಶಿ_ಗಾತ್ರ;	// total number of entries in ml_stack

#ವ್ಯಾಖ್ಯಾನಿಸು ML_ಖಾಲಿಗಳು	0x01	// empty buffer
#ವ್ಯಾಖ್ಯಾನಿಸು ML_ಸಾಲು_DIRTY	0x02	// cached line was changed and allocated
#ವ್ಯಾಖ್ಯಾನಿಸು ML_LOCKED_DIRTY	0x04	// ml_locked was changed
#ವ್ಯಾಖ್ಯಾನಿಸು ML_LOCKED_POS	0x08	// ml_locked needs positive block number
#ವ್ಯಾಖ್ಯಾನಿಸು ML_ALLOCATED	0x10	// ml_line_ptr is an allocated copy
    ಪೂರ್ಣಾಂಕ		ml_flags;

    colnr_T	ml_ಸಾಲು_len;	// length of the cached line + NUL + text properties
    colnr_T	ml_ಸಾಲು_textlen;// length of the cached line + NUL, 0 if not known yet
    linenr_T	ml_ಸಾಲು_lnum;	// line number of cached line, 0 if not valid
    char_u	*ml_ಸಾಲು_ptr;	// pointer to cached line

    bhdr_T	*ml_locked;	// block used by last ml_get
    linenr_T	ml_locked_ಕೆಳಗಿನ;	// first line in ml_locked
    linenr_T	ml_locked_ಮೇಲಿನ;	// last line in ml_locked
    ಪೂರ್ಣಾಂಕ		ml_locked_lineadd;  // number of lines inserted in ml_locked
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_BYTEOFF
    chunksize_T *ml_chunksize;
    ಪೂರ್ಣಾಂಕ		ml_numchunks;
    ಪೂರ್ಣಾಂಕ		ml_usedchunks;
#ಆದರೆ_ಅಂತ್ಯ
} memline_T;

// Values for the flags argument of ml_delete_flags().
#ವ್ಯಾಖ್ಯಾನಿಸು ML_DEL_ಸಂದೇಶ	    1	// may give a "No lines in buffer" message
#ವ್ಯಾಖ್ಯಾನಿಸು ML_DEL_UNDO	    2	// called from undo, do not update textprops
#ವ್ಯಾಖ್ಯಾನಿಸು ML_DEL_NOPROP	    4	// splitting data block, do not update textprops

// Values for the flags argument of ml_append_int().
#ವ್ಯಾಖ್ಯಾನಿಸು ML_APPEND_NEW	    1	// starting to edit a new file
#ವ್ಯಾಖ್ಯಾನಿಸು ML_APPEND_MARK	    2	// mark the new line
#ವ್ಯಾಖ್ಯಾನಿಸು ML_APPEND_UNDO	    4	// called from undo
#ವ್ಯಾಖ್ಯಾನಿಸು ML_APPEND_NOPROP    8	// do not continue textprop from previous line


/*
 * Structure defining text properties.  These stick with the text.
 * When stored in memline they are after the text, ml_line_len is larger than
 * STRLEN(ml_line_ptr) + 1.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ textprop_S
{
    colnr_T	tp_ಲಂಬಸಾಲು;		// start column (one based, in bytes)
    colnr_T	tp_len;		// length in bytes; for virtual text props
				// this is STRLEN(vtext) (not including NUL)
    ಪೂರ್ಣಾಂಕ		tp_id;		// identifier
    ಪೂರ್ಣಾಂಕ		tp_type;	// property type
    ಪೂರ್ಣಾಂಕ		tp_flags;	// TP_FLAG_ values
    ಪೂರ್ಣಾಂಕ		tp_padleft;	// left padding between text line and virtual
				//ಪಠ್ಯ
    ಒಕ್ಕೂಟ			// For virtual text props (tp_id < 0):
    {				// check TP_FLAG_VTEXT_PTR in tp_flags to
				// determine which member is active.
	colnr_T	tp_ಪಠ್ಯ_ಸ್ಥಾನಾಂತರ; // offset to vtext string from the
				// prop_count position in the memline
				// (when TP_FLAG_VTEXT_PTR is NOT set)
	char_u	*tp_ಪಠ್ಯ;	// pointer to virtual text string
				// (when TP_FLAG_VTEXT_PTR IS set)
    } u;
} textprop_T;

#ವ್ಯಾಖ್ಯಾನಿಸು TP_FLAG_CONT_ಮುಂದಿನ	0x1	// property continues in next line
#ವ್ಯಾಖ್ಯಾನಿಸು TP_FLAG_CONT_PREV	0x2	// property was continued from prev line

// without these text is placed after the end of the line
#ವ್ಯಾಖ್ಯಾನಿಸು TP_FLAG_ALIGN_ಬಲ	0x010	// virtual text is right-aligned
#ವ್ಯಾಖ್ಯಾನಿಸು TP_FLAG_ALIGN_ABOVE	0x020	// virtual text above the line
#ವ್ಯಾಖ್ಯಾನಿಸು TP_FLAG_ALIGN_ಕೆಳಗೆ	0x040	// virtual text on next screen line

#ವ್ಯಾಖ್ಯಾನಿಸು TP_FLAG_WRAP		0x080	// virtual text wraps - when missing
					// text is truncated
#ವ್ಯಾಖ್ಯಾನಿಸು TP_FLAG_ಆರಂಭ_INCL	0x100	// "start_incl" copied from proptype
#ವ್ಯಾಖ್ಯಾನಿಸು TP_FLAG_DELETED		0x200	// marked for deletion in
					// unpacked_memline_T
#ವ್ಯಾಖ್ಯಾನಿಸು TP_FLAG_VTEXT_PTR	0x400	// u.tp_text access is valid

#ವ್ಯಾಖ್ಯಾನಿಸು PROP_ಎಣಿಕೆ_ಗಾತ್ರ	ಗಾತ್ರ(uint16_t)    // size of prop_count in memline

#ವ್ಯಾಖ್ಯಾನಿಸು PROP_ಪಠ್ಯ_MIN_CELLS	4	// minimum number of cells to use for
					// the text, even when truncating

/*
 * An unpacked form of a single memline with text properties.
 *
 * When packed in a memline, the format is:
 *   [line text] [NUL] [textprop_T...] [vtext strings...]
 * Virtual text props use u.tp_text_offset (relative to the start of props
 * area).  When unpacked, u.tp_text is a pointer: either into the memline
 * data (LOADED) or to separately allocated memory (DETACHED).
 *
 * States:
 * - LOADED: text and u.tp_text point into the memline. Read-only.
 * - DETACHED: text and u.tp_text are separately allocated. Writable.
 * - CLOSED: buf == NULL. Unusable (error recovery).
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ unpacked_memline_S
{
    buf_T	*buf;		// the line's buffer (NULL = closed)
    linenr_T	lnum;		// line number (0 = no line loaded)
    bool	detached;	// true when text/vtext are allocated
    colnr_T	ಪಠ್ಯ_ಗಾತ್ರ_2;	// size of text including NUL
    char_u	*ಪಠ್ಯ;		// NUL-terminated text
    ಪೂರ್ಣಾಂಕ		prop_ಗಾತ್ರ;	// number of allocated prop slots
    ಪೂರ್ಣಾಂಕ		prop_ಎಣಿಕೆ;	// number of properties
    textprop_T	*props;		// property array
    bool	ಪಠ್ಯ_ಬದಲಾಯಿತು;	// true if text was modified
} unpacked_memline_T;

/*
 * Structure defining a property type.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ proptype_S
{
    ಪೂರ್ಣಾಂಕ		pt_id;		// value used for tp_id
    ಪೂರ್ಣಾಂಕ		pt_type;	// number used for tp_type
    ಪೂರ್ಣಾಂಕ		pt_hl_id;	// highlighting
    ಪೂರ್ಣಾಂಕ		pt_ಆದ್ಯತೆ;	//ಆದ್ಯತೆ
    ಪೂರ್ಣಾಂಕ		pt_flags;	// PT_FLAG_ values
    char_u	pt_name[1];	// property type name, actually longer
} proptype_T;

#ವ್ಯಾಖ್ಯಾನಿಸು PT_FLAG_INS_ಆರಂಭ_INCL	1	// insert at start included in property
#ವ್ಯಾಖ್ಯಾನಿಸು PT_FLAG_INS_ಕೊನೆ_INCL	2	// insert at end included in property
#ವ್ಯಾಖ್ಯಾನಿಸು PT_FLAG_COMBINE		4	// combine with syntax highlight
#ವ್ಯಾಖ್ಯಾನಿಸು PT_FLAG_OVERRIDE	8	// override any highlight

// Sign group
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ signgroup_S
{
    ಪೂರ್ಣಾಂಕ		sg_ಮುಂದಿನ_sign_id;	// next sign id for this group
    short_u	sg_refcount;		// number of signs in this group
    char_u	sg_name[1];		// sign group name, actually longer
} signgroup_T;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ sign_ನಮೂದು sign_ನಮೂದು_T;
ರಚನೆ sign_ನಮೂದು
{
    ಪೂರ್ಣಾಂಕ		 se_id;		// unique identifier for each placed sign
    ಪೂರ್ಣಾಂಕ		 se_typenr;	// typenr of sign
    ಪೂರ್ಣಾಂಕ		 se_ಆದ್ಯತೆ;	// priority for highlighting
    linenr_T	 se_lnum;	// line number which has this sign
    signgroup_T	 *se_ಗುಂಪು;	// sign group
    sign_ನಮೂದು_T *se_ಮುಂದಿನ;	// next entry in a list of signs
    sign_ನಮೂದು_T *se_prev;	// previous entry -- for easy reordering
};

/*
 * Sign attributes. Used by the screen refresh routines.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ sign_attrs_S {
    ಪೂರ್ಣಾಂಕ		sat_typenr;
    ಶೂನ್ಯ	*sat_icon;
    char_u	*sat_ಪಠ್ಯ;
    ಪೂರ್ಣಾಂಕ		sat_texthl;
    ಪೂರ್ಣಾಂಕ		sat_linehl;
    ಪೂರ್ಣಾಂಕ		sat_culhl;
    ಪೂರ್ಣಾಂಕ		sat_numhl;
    ಪೂರ್ಣಾಂಕ		sat_ಆದ್ಯತೆ;
} sign_attrs_T;

#ಆದರೆ defined(FEAT_SIGNS)
// Macros to get the sign group structure from the group name
# define SGN_ಕೀಲಿ_OFF	offsetof(signgroup_T, sg_name)
# define HI2SG(hi)	((signgroup_T *)((hi)->hi_ಕೀಲಿ - SGN_ಕೀಲಿ_OFF))

// Default sign priority for highlighting
# define SIGN_DEF_PRIO	10

#ಆದರೆ_ಅಂತ್ಯ

/*
 * Argument list: Array of file names.
 * Used for the global argument list and the argument lists local to a window.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ arglist
{
    garray_T	al_ga;		// growarray with the array of file names
    ಪೂರ್ಣಾಂಕ		al_refcount;	// number of windows using this arglist
    ಪೂರ್ಣಾಂಕ		id;		// id of this arglist
} alist_T;

/*
 * For each argument remember the file name as it was given, and the buffer
 * number that contains the expanded file name (required for when ":cd" is
 * used).
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ argentry
{
    char_u	*ae_fname;	// file name as specified
    ಪೂರ್ಣಾಂಕ		ae_fnum;	// buffer number with expanded file name
} aentry_T;

#ವ್ಯಾಖ್ಯಾನಿಸು ALIST(win)	(win)->w_alist
#ವ್ಯಾಖ್ಯಾನಿಸು GARGLIST	((aentry_T *)global_alist.al_ga.ga_ದತ್ತಾಂಶ)
#ವ್ಯಾಖ್ಯಾನಿಸು ARGLIST		((aentry_T *)ALIST(curwin)->al_ga.ga_ದತ್ತಾಂಶ)
#ವ್ಯಾಖ್ಯಾನಿಸು WARGLIST(wp)	((aentry_T *)ALIST(wp)->al_ga.ga_ದತ್ತಾಂಶ)
#ವ್ಯಾಖ್ಯಾನಿಸು AARGLIST(al)	((aentry_T *)((al)->al_ga.ga_ದತ್ತಾಂಶ))
#ವ್ಯಾಖ್ಯಾನಿಸು GARGCOUNT	(global_alist.al_ga.ga_len)
#ವ್ಯಾಖ್ಯಾನಿಸು ARGCOUNT	(ALIST(curwin)->al_ga.ga_len)
#ವ್ಯಾಖ್ಯಾನಿಸು WARGCOUNT(wp)	(ALIST(wp)->al_ga.ga_len)

/*
 * A list used for saving values of "emsg_silent".  Used by ex_try() to save the
 * value of "emsg_silent" if it was non-zero.  When this is done, the CSF_SILENT
 * flag below is set.
 */

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ eslist_elem eslist_T;
ರಚನೆ eslist_elem
{
    ಪೂರ್ಣಾಂಕ		saved_emsg_silent;	// saved value of "emsg_silent"
    eslist_T	*ಮುಂದಿನ;			// next element on the list
};

/*
 * For conditional commands a stack is kept of nested conditionals.
 * When cs_idx < 0, there is no conditional command.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು CSTACK_LEN	50

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಗಿಡ್ಡ	cs_flags[CSTACK_LEN];	// CSF_ flags
    ಅಕ್ಷರ	cs_pending[CSTACK_LEN];	// CSTP_: what's pending in ":finally"
    ಒಕ್ಕೂಟ {
	ಶೂನ್ಯ	*csp_rv[CSTACK_LEN];	// return typeval for pending return
	ಶೂನ್ಯ	*csp_ex[CSTACK_LEN];	// exception for pending throw
    }		cs_pend;
    ಶೂನ್ಯ	*cs_forinfo[CSTACK_LEN]; // info used by ":for"
    ಪೂರ್ಣಾಂಕ		cs_ಸಾಲು[CSTACK_LEN];	// line nr of ":while"/":for" line
    ಪೂರ್ಣಾಂಕ		cs_ಖಂಡ_id[CSTACK_LEN];    // block ID stack
    ಪೂರ್ಣಾಂಕ		cs_script_var_len[CSTACK_LEN];	// value of sn_var_vals.ga_len
						// when entering the block
    ಪೂರ್ಣಾಂಕ		cs_idx;			// current entry, or -1 if none
    ಪೂರ್ಣಾಂಕ		cs_looplevel;		// nr of nested ":while"s and ":for"s
    ಪೂರ್ಣಾಂಕ		cs_trylevel;		// nr of nested ":try"s
    eslist_T	*cs_emsg_silent_list;	// saved values of "emsg_silent"
    ಅಕ್ಷರ	cs_lflags;		// loop flags: CSL_ flags
} cstack_T;
#ವ್ಯಾಖ್ಯಾನಿಸು cs_rettv	cs_pend.csp_rv
#ವ್ಯಾಖ್ಯಾನಿಸು cs_exception	cs_pend.csp_ex

// There is no CSF_IF, the lack of CSF_WHILE, CSF_FOR and CSF_TRY means ":if"
// was used.
#ವ್ಯಾಖ್ಯಾನಿಸು CSF_TRUE	0x0001	// condition was TRUE
#ವ್ಯಾಖ್ಯಾನಿಸು CSF_ಸಕ್ರಿಯ	0x0002	// current state is active
#ವ್ಯಾಖ್ಯಾನಿಸು CSF_ELSE	0x0004	// ":else" has been passed
#ವ್ಯಾಖ್ಯಾನಿಸು CSF_WHILE	0x0008	// is a ":while"
#ವ್ಯಾಖ್ಯಾನಿಸು CSF_FOR	0x0010	// is a ":for"
#ವ್ಯಾಖ್ಯಾನಿಸು CSF_ಖಂಡ	0x0020	// is a "{" block

#ವ್ಯಾಖ್ಯಾನಿಸು CSF_TRY	0x0100	// is a ":try"
#ವ್ಯಾಖ್ಯಾನಿಸು CSF_FINALLY	0x0200	// ":finally" has been passed
#ವ್ಯಾಖ್ಯಾನಿಸು CSF_CATCH	0x0400	// ":catch" has been seen
#ವ್ಯಾಖ್ಯಾನಿಸು CSF_THROWN	0x0800	// exception thrown to this try conditional
#ವ್ಯಾಖ್ಯಾನಿಸು CSF_CAUGHT	0x1000  // exception caught by this try conditional
#ವ್ಯಾಖ್ಯಾನಿಸು CSF_ಪೂರ್ಣಗೊಂಡ	0x2000  // CSF_CAUGHT was handled by finish_exception()
#ವ್ಯಾಖ್ಯಾನಿಸು CSF_SILENT	0x4000	// "emsg_silent" reset by ":try"
// Note that CSF_ELSE is only used when CSF_TRY and CSF_WHILE are unset
// (an ":if"), and CSF_SILENT is only used when CSF_TRY is set.

#ವ್ಯಾಖ್ಯಾನಿಸು CSF_FUNC_DEF	0x8000	// a function was defined in this block

/*
 * What's pending for being reactivated at the ":endtry" of this try
 * conditional:
 */
#ವ್ಯಾಖ್ಯಾನಿಸು CSTP_NONE	0	// nothing pending in ":finally" clause
#ವ್ಯಾಖ್ಯಾನಿಸು CSTP_ERROR	1	// an error is pending
#ವ್ಯಾಖ್ಯಾನಿಸು CSTP_ಅಡಚಣೆ	2	// an interrupt is pending
#ವ್ಯಾಖ್ಯಾನಿಸು CSTP_THROW	4	// a throw is pending
#ವ್ಯಾಖ್ಯಾನಿಸು CSTP_BREAK	8	// ":break" is pending
#ವ್ಯಾಖ್ಯಾನಿಸು CSTP_CONTINUE	16	// ":continue" is pending
#ವ್ಯಾಖ್ಯಾನಿಸು CSTP_RETURN	24	// ":return" is pending
#ವ್ಯಾಖ್ಯಾನಿಸು CSTP_ಮುಕ್ತಾಯ	32	// ":finish" is pending

/*
 * Flags for the cs_lflags item in cstack_T.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು CSL_HAD_LOOP	 1	// just found ":while" or ":for"
#ವ್ಯಾಖ್ಯಾನಿಸು CSL_HAD_ENDLOOP 2	// just found ":endwhile" or ":endfor"
#ವ್ಯಾಖ್ಯಾನಿಸು CSL_HAD_CONT	 4	// just found ":continue"
#ವ್ಯಾಖ್ಯಾನಿಸು CSL_HAD_FINA	 8	// just found ":finally"

/*
 * A list of error messages that can be converted to an exception.  "throw_msg"
 * is only set in the first element of the list.  Usually, it points to the
 * original message stored in that element, but sometimes it points to a later
 * message in the list.  See cause_errthrow().
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ msglist msglist_T;
ರಚನೆ msglist
{
    msglist_T	*ಮುಂದಿನ;		// next of several messages in a row
    ಅಕ್ಷರ	*ಸಂದೇಶ;		// original message, allocated
    ಅಕ್ಷರ	*throw_ಸಂದೇಶ;	// msg to throw: usually original one
    char_u	*sfile;		// value from estack_sfile(), allocated
    ದೀರ್ಘ	slnum;		// line number for "sfile"
    ಪೂರ್ಣಾಂಕ		ಸಂದೇಶ_compiling;	// saved value of estack_compiling
};

/*
 * The exception types.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ
{
    ET_USER,		// exception caused by ":throw" command
    ET_ERROR,		// error exception
    ET_ಅಡಚಣೆ,	// interrupt exception triggered by Ctrl-C
} except_type_T;

/*
 * Structure describing an exception.
 * (don't use "struct exception", it's used by the math library).
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ vim_exception except_T;
ರಚನೆ vim_exception
{
    except_type_T	type;		// exception type
    ಅಕ್ಷರ		*ಮೌಲ್ಯ;		// exception value
    ರಚನೆ msglist	*messages;	// message(s) causing error exception
    char_u		*throw_name;	// name of the throw point
    linenr_T		throw_lnum;	// line number of the throw point
    list_T		*stacktrace;	// stacktrace
    except_T		*caught;	// next exception on the caught stack
};

/*
 * Structure to save the error/interrupt/exception state between calls to
 * enter_cleanup() and leave_cleanup().  Must be allocated as an automatic
 * variable by the (common) caller of these functions.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ cleanup_stuff cleanup_T;
ರಚನೆ cleanup_stuff
{
    ಪೂರ್ಣಾಂಕ pending;		// error/interrupt/exception state
    except_T *exception;	// exception value
};

/*
 * Exception state that is saved and restored when calling timer callback
 * functions and deferred functions.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ exception_state_S exception_state_T;
ರಚನೆ exception_state_S
{
    except_T	*estate_current_exception;
    ಪೂರ್ಣಾಂಕ		estate_did_throw;
    ಪೂರ್ಣಾಂಕ		estate_need_rethrow;
    ಪೂರ್ಣಾಂಕ		estate_trylevel;
    ಪೂರ್ಣಾಂಕ		estate_did_emsg;
};

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
// struct passed to in_id_list()
ರಚನೆ sp_syn
{
    ಪೂರ್ಣಾಂಕ		inc_ಟ್ಯಾಗ್;	// ":syn include" unique tag
    ಗಿಡ್ಡ	id;		// highlight group ID of item
    ಗಿಡ್ಡ	*cont_ಒಳ_list;	// cont.in group IDs, if non-zero
};

/*
 * Each keyword has one keyentry, which is linked in a hash list.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ keyentry keyentry_T;

ರಚನೆ keyentry
{
    keyentry_T	*ke_ಮುಂದಿನ;	// next entry with identical "keyword[]"
    ರಚನೆ sp_syn k_syn;	// struct passed to in_id_list()
    ಗಿಡ್ಡ	*ಮುಂದಿನ_list;	// ID list for next match (if non-zero)
    ಪೂರ್ಣಾಂಕ		flags;
    ಪೂರ್ಣಾಂಕ		k_char;		// conceal substitute character
    char_u	keyword[1];	// actually longer
};

/*
 * Struct used to store one state of the state stack.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ buf_state
{
    ಪೂರ್ಣಾಂಕ		    bs_idx;	 // index of pattern
    ಪೂರ್ಣಾಂಕ		    bs_flags;	 // flags for pattern
# ifdef FEAT_CONCEAL
    ಪೂರ್ಣಾಂಕ		    bs_seqnr;	 // stores si_seqnr
    ಪೂರ್ಣಾಂಕ		    bs_cchar;	 // stores si_cchar
# endif
    reg_extmatch_T *bs_extmatch; // external matches from start pattern
} bufstate_T;

/*
 * syn_state contains the syntax state stack for the start of one line.
 * Used by b_sst_array[].
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ syn_state synstate_T;

ರಚನೆ syn_state
{
    synstate_T	*sst_ಮುಂದಿನ;	// next entry in used or free list
    linenr_T	sst_lnum;	// line number for this state
    ಒಕ್ಕೂಟ
    {
	bufstate_T	sst_ರಾಶಿ[SST_FIX_ಸ್ಥಿತಿಗಳು]; // short state stack
	garray_T	sst_ga;	// growarray for long state stack
    } sst_union;
    ಪೂರ್ಣಾಂಕ		sst_ಮುಂದಿನ_flags;	// flags for sst_next_list
    ಪೂರ್ಣಾಂಕ		sst_stacksize;	// number of states on the stack
    ಗಿಡ್ಡ	*sst_ಮುಂದಿನ_list;	// "nextgroup" list in this state
				// (this is a copy, don't free it!
    disptick_T	sst_tick;	// tick when last displayed
    linenr_T	sst_change_lnum;// when non-zero, change in this line
				// may have made the state invalid
};
#ಆದರೆ_ಅಂತ್ಯ // FEAT_SYN_HL

#ವ್ಯಾಖ್ಯಾನಿಸು MAX_HL_ID       20000	// maximum value for a highlight ID.

/*
 * Structure shared between syntax.c, screen.c and gui_x11.c.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ attr_ನಮೂದು
{
    ಗಿಡ್ಡ	    ae_attr;		// HL_BOLD, etc.
    ಒಕ್ಕೂಟ
    {
	ರಚನೆ
	{
	    char_u	    *ಆರಂಭ;	// start escape sequence
	    char_u	    *stop;	// stop escape sequence
	} ಪದ;
	ರಚನೆ
	{
	    // These colors need to be > 8 bits to hold 256.
	    short_u	    fg_color;	// foreground color number
	    short_u	    bg_color;	// background color number
	    short_u	    ul_color;	// underline color number
	    short_u	    font;	// font number
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMGUICOLORS
	    guicolor_T	    fg_rgb;	// foreground color RGB
	    guicolor_T	    bg_rgb;	// background color RGB
	    guicolor_T	    ul_rgb;	// underline color RGB
#ಆದರೆ_ಅಂತ್ಯ
	} cterm;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
	ರಚನೆ
	{
	    guicolor_T	    fg_color;	// foreground color handle
	    guicolor_T	    bg_color;	// background color handle
	    guicolor_T	    sp_color;	// special color handle
	    GuiFont	    font;	// font handle
# ifdef FEAT_XFONTSET
	    GuiFontset	    fontset;	// fontset handle
# endif
	} gui;
#ಆದರೆ_ಅಂತ್ಯ
    } ae_u;
} attrentry_T;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ICONV
# ifdef HAVE_ICONV_H
#  include <iconv.h>
# ಇಲ್ಲದಿದ್ದರೆ
#  ಆದರೆ defined(MACOS_X)
#   include <sys/errno.h>
#   ifndef EILSEQ
#    define EILSEQ ENOENT // Early MacOS X does not have EILSEQ
#   endif
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ _iconv_t *iconv_t;
#  ಇಲ್ಲದಿದ್ದರೆ
#   include <errno.h>
#  endif
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಶೂನ್ಯ *iconv_t;
# endif
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Used for the typeahead buffer: typebuf.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    char_u	*tb_buf;	// buffer for typed characters
    char_u	*tb_noremap;	// mapping flags for characters in tb_buf[]
    ಪೂರ್ಣಾಂಕ		tb_buflen;	// size of tb_buf[]
    ಪೂರ್ಣಾಂಕ		tb_off;		// current position in tb_buf[]
    ಪೂರ್ಣಾಂಕ		tb_len;		// number of valid bytes in tb_buf[]
    ಪೂರ್ಣಾಂಕ		tb_maplen;	// nr of mapped bytes in tb_buf[]
    ಪೂರ್ಣಾಂಕ		tb_silent;	// nr of silently mapped bytes in tb_buf[]
    ಪೂರ್ಣಾಂಕ		tb_no_abbr_cnt; // nr of bytes without abbrev. in tb_buf[]
    ಪೂರ್ಣಾಂಕ		tb_change_cnt;	// nr of time tb_buf was changed; never zero
} typebuf_T;

// Struct to hold the saved typeahead for save_typeahead().
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    typebuf_T		save_typebuf;
    ಪೂರ್ಣಾಂಕ			typebuf_valid;	    // TRUE when save_typebuf valid
    ಪೂರ್ಣಾಂಕ			old_char;
    ಪೂರ್ಣಾಂಕ			old_mod_ಮುಖವಾಡ;
    buffheader_T	save_readbuf1;
    buffheader_T	save_readbuf2;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ಒಳಹರಿವು_BUF
    char_u		*save_inputbuf;
#ಆದರೆ_ಅಂತ್ಯ
} tasave_T;

/*
 * Used for conversion of terminal I/O and script files.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    ಪೂರ್ಣಾಂಕ		vc_type;	// zero or one of the CONV_ values
    ಪೂರ್ಣಾಂಕ		vc_ಗುಣಕ;	// max. expansion factor
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
    ಪೂರ್ಣಾಂಕ		vc_cpfrom;	// codepage to convert from (CONV_CODEPAGE)
    ಪೂರ್ಣಾಂಕ		vc_cpto;	// codepage to convert to (CONV_CODEPAGE)
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ICONV
    iconv_t	vc_fd;		// for CONV_ICONV
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		vc_fail;	// fail for invalid char, don't use '?'
} vimconv_T;

/*
 * Structure used for the command line history.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ hist_ನಮೂದು
{
    ಪೂರ್ಣಾಂಕ		hisnum;		// identifying number
    ಪೂರ್ಣಾಂಕ		viminfo;	// when TRUE hisstr comes from viminfo
    char_u	*hisstr;	// actual entry, separator char after the NUL
    size_t	hisstrlen;	// length of hisstr (excluding the NUL)
    time_t	ಸಮಯ_ಗಣ;	// when it was typed, zero if unknown
} histentry_T;

#ವ್ಯಾಖ್ಯಾನಿಸು CONV_NONE		0
#ವ್ಯಾಖ್ಯಾನಿಸು CONV_ಗಮ್ಯ_UTF8		1
#ವ್ಯಾಖ್ಯಾನಿಸು CONV_9_ಗಮ್ಯ_UTF8		2
#ವ್ಯಾಖ್ಯಾನಿಸು CONV_ಗಮ್ಯ_LATIN1		3
#ವ್ಯಾಖ್ಯಾನಿಸು CONV_ಗಮ್ಯ_LATIN9		4
#ವ್ಯಾಖ್ಯಾನಿಸು CONV_ICONV		5
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
# define CONV_CODEPAGE		10	// codepage -> codepage
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MACOS_X
# define CONV_MAC_LATIN1	20
# define CONV_LATIN1_MAC	21
# define CONV_MAC_UTF8		22
# define CONV_UTF8_MAC		23
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Structure used for mappings and abbreviations.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ mapblock mapblock_T;
ರಚನೆ mapblock
{
    mapblock_T	*m_ಮುಂದಿನ;	// next mapblock in list
    mapblock_T	*m_alt;		// pointer to mapblock of the same mapping
				// with an alternative form of m_keys, or NULL
				// if there is no such mapblock
    char_u	*m_ಕೀಲಿಗಳು;	// mapped from, lhs
    char_u	*m_str;		// mapped to, rhs
    char_u	*m_orig_str;	// rhs as entered by the user
    ಪೂರ್ಣಾಂಕ		m_keylen;	// strlen(m_keys)
    ಪೂರ್ಣಾಂಕ		m_ಬಹುಲಕ;		// valid mode
    ಪೂರ್ಣಾಂಕ		m_simplified;	// m_keys was simplified, do not use this map
				// if key_protocol_enabled() returns TRUE
    ಪೂರ್ಣಾಂಕ		m_noremap;	// if non-zero no re-mapping for m_str
    ಅಕ್ಷರ	m_silent;	// <silent> used, don't echo commands
    ಅಕ್ಷರ	m_nowait;	// <nowait> used
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಅಕ್ಷರ	m_ಅಭಿವ್ಯಕ್ತಿ;		// <expr> used, m_str is an expression
    sctx_T	m_script_ctx;	// SCTX where map was defined
#ಆದರೆ_ಅಂತ್ಯ
};


/*
 * Used for highlighting in the status line.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    char_u	*ಆರಂಭ;
    ಪೂರ್ಣಾಂಕ		userhl;		// 0: no HL, 1-9: User HL, < 0 for syn ID
} stl_hlrec_T;

/*
 * Used for statusline click function regions.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    char_u	*ಆರಂಭ;		// position in output buffer where region starts
    char_u	*funcname;	// function name (NULL = end/close marker)
    ಪೂರ್ಣಾಂಕ		minwid;		// minwid value from %N@Func@
} stl_clickrec_T;

/*
 * Per-window resolved click regions (screen column based).
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಪೂರ್ಣಾಂಕ		ಅಡ್ಡಸಾಲು;		// screen row where region lives
    ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು_ಆರಂಭ;	// screen column where region starts
    ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು_ಕೊನೆ;	// screen column where region ends
    char_u	*funcname;	// function name (allocated copy)
    ಪೂರ್ಣಾಂಕ		minwid;		// minwid value
    ಪೂರ್ಣಾಂಕ		tabnr;		// tab page number (tabpanel only, 0 otherwise)
} stl_click_region_T;


/*
 * Syntax items - usually buffer-specific.
 */

/*
 * Item for a hashtable.  "hi_key" can be one of three values:
 * NULL:	   Never been used
 * HI_KEY_REMOVED: Entry was removed
 * Otherwise:	   Used item, pointer to the actual key; this usually is
 *		   inside the item, subtract an offset to locate the item.
 *		   This reduces the size of hashitem by 1/3.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ hashitem_S
{
    long_u	hi_hash;	// cached hash number of hi_key
    char_u	*hi_ಕೀಲಿ;
} hashitem_T;

// The address of "hash_removed" is used as a magic number for hi_key to
// indicate a removed item.
#ವ್ಯಾಖ್ಯಾನಿಸು HI_ಕೀಲಿ_REMOVED &hash_removed
#ವ್ಯಾಖ್ಯಾನಿಸು HASHITEM_ಖಾಲಿಗಳು(hi) ((hi)->hi_ಕೀಲಿ == ಖಾಲಿ || (hi)->hi_ಕೀಲಿ == &hash_removed)

// Initial size for a hashtable.  Our items are relatively small and growing
// is expensive, thus use 16 as a start.  Must be a power of 2.
// This allows for storing 10 items (2/3 of 16) before a resize is needed.
#ವ್ಯಾಖ್ಯಾನಿಸು HT_INIT_ಗಾತ್ರ 16

// flags used for ht_flags
#ವ್ಯಾಖ್ಯಾನಿಸು HTFLAGS_ERROR	0x01	// Set when growing failed, can't add more
				// items before growing works.
#ವ್ಯಾಖ್ಯಾನಿಸು HTFLAGS_FROZEN	0x02	// Trying to add or remove an item will result
				// in an error message.

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ hashtable_S
{
    long_u	ht_ಮುಖವಾಡ;	// mask used for hash value (nr of items in
				// array is "ht_mask" + 1)
    long_u	ht_ಬಳಸಿದ;	// number of items used
    long_u	ht_filled;	// number of items used + removed
    ಪೂರ್ಣಾಂಕ		ht_ಬದಲಾಯಿತು;	// incremented when adding or removing an item
    ಪೂರ್ಣಾಂಕ		ht_locked;	// counter for hash_lock()
    ಪೂರ್ಣಾಂಕ		ht_flags;	// HTFLAGS_ values
    hashitem_T	*ht_array;	// points to the array, allocated when it's
				// not "ht_smallarray"
    hashitem_T	ht_smallarray[HT_INIT_ಗಾತ್ರ];   // initial array
} hashtab_T;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ long_u hash_T;		// Type for hi_hash


// Use 64-bit Number.
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
   ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ __int64		varnumber_T;
   ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಚಿಹ್ನೆರಹಿತ __int64	uvarnumber_T;
# define VARNUM_MIN		_I64_MIN
# define VARNUM_MAX		_I64_MAX
# define UVARNUM_MAX		_UI64_MAX
#ಅಥವಾಆದರೆ defined(HAVE_NO_LONG_LONG)
# ಆದರೆ defined(HAVE_STDINT_H)
   ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ int64_t		varnumber_T;
   ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ uint64_t		uvarnumber_T;
#  define VARNUM_MIN		INT64_MIN
#  define VARNUM_MAX		INT64_MAX
#  define UVARNUM_MAX		UINT64_MAX
# ಇಲ್ಲದಿದ್ದರೆ
   // this may cause trouble for code that depends on 64 bit ints
   ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ದೀರ್ಘ			varnumber_T;
   ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ	uvarnumber_T;
#  define VARNUM_MIN		LONG_MIN
#  define VARNUM_MAX		LONG_MAX
#  define UVARNUM_MAX		ULONG_MAX
# endif
#ಇಲ್ಲದಿದ್ದರೆ
  ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ದೀರ್ಘ ದೀರ್ಘ		varnumber_T;
  ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ದೀರ್ಘ	uvarnumber_T;
# ifdef LLONG_MIN
#  define VARNUM_MIN		LLONG_MIN
#  define VARNUM_MAX		LLONG_MAX
#  define UVARNUM_MAX		ULLONG_MAX
# ಇಲ್ಲದಿದ್ದರೆ
#  define VARNUM_MIN		LONG_LONG_MIN
#  define VARNUM_MAX		LONG_LONG_MAX
#  define UVARNUM_MAX		ULONG_LONG_MAX
# endif
#ಆದರೆ_ಅಂತ್ಯ

// Struct that holds both a normal function name and a partial_T, as used for a
// callback argument.
// When used temporarily "cb_name" is not allocated.  The refcounts to either
// the function or the partial are incremented and need to be decremented
// later with free_callback().
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    char_u	*cb_name;
    partial_T	*cb_partial;
    ಪೂರ್ಣಾಂಕ		cb_free_name;	    // cb_name was allocated
} callback_T;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ isn_S isn_T;	    // instruction
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ dfunc_S dfunc_T;	    // :def function

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ type_S type_T;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ ufunc_S ufunc_T;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ jobvar_S ಕೆಲಸ_T;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ readq_S readq_T;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ writeq_S writeq_T;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ jsonq_S jsonq_T;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ cbq_S cbq_T;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ channel_S channel_T;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ cctx_S cctx_T;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ ectx_S ectx_T;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ instr_S instr_T;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ class_S class_T;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ object_S object_T;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ typealias_S typealias_T;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ
{
    VAR_UNKNOWN = 0,	// not set, any type or "void" allowed
    VAR_ANY,		// used for "any" type
    VAR_VOID,		// no value (function not returning anything)
    VAR_BOOL,		// "v_number" is used: VVAL_TRUE or VVAL_FALSE
    VAR_SPECIAL,	// "v_number" is used: VVAL_NULL or VVAL_NONE
    VAR_ಸಂಖ್ಯೆ,		// "v_number" is used
    VAR_FLOAT,		// "v_float" is used
    VAR_STRING,		// "v_string" is used
    VAR_BLOB,		// "v_blob" is used
    VAR_FUNC,		// "v_string" is function name
    VAR_PARTIAL,	// "v_partial" is used
    VAR_LIST,		// "v_list" is used
    VAR_DICT,		// "v_dict" is used
    VAR_ಕೆಲಸ,		// "v_job" is used
    VAR_CHANNEL,	// "v_channel" is used
    VAR_INSTR,		// "v_instr" is used
    VAR_CLASS,		// "v_class" is used (also used for interface)
    VAR_OBJECT,		// "v_object" is used
    VAR_TYPEALIAS,	// "v_typealias" is used
    VAR_TUPLE		// "v_tuple" is used
} vartype_T;

// A type specification.
ರಚನೆ type_S {
    vartype_T	    tt_type;
    ಪೂರ್ಣಾಂಕ	    tt_argcount;    // for func, incl. vararg, -1 for unknown
    ಪೂರ್ಣಾಂಕ	    tt_min_argcount; // number of non-optional arguments
    short_u	    tt_flags;	    // TTFLAG_ values
    type_T	    *tt_member;	    // for list, dict, func return type
    class_T	    *tt_class;	    // for class and object
    type_T	    **tt_ವಾದಗಳು;	    // func argument types, allocated
};

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    type_T	*type_curr;	    // current type, value type
    type_T	*type_decl;	    // declared type or equal to type_current
} type2_T;

#ವ್ಯಾಖ್ಯಾನಿಸು TTFLAG_VARARGS	    0x01    // func args ends with "..."
#ವ್ಯಾಖ್ಯಾನಿಸು TTFLAG_BOOL_OK	    0x02    // can be converted to bool
#ವ್ಯಾಖ್ಯಾನಿಸು TTFLAG_FLOAT_OK	    0x04    // number can be used/converted to float
#ವ್ಯಾಖ್ಯಾನಿಸು TTFLAG_ಸಂಖ್ಯೆ_OK    0x08    // number can be used for a float
#ವ್ಯಾಖ್ಯಾನಿಸು TTFLAG_STATIC	    0x10    // one of the static types, e.g. t_any
#ವ್ಯಾಖ್ಯಾನಿಸು TTFLAG_CONST	    0x20    // cannot be changed
#ವ್ಯಾಖ್ಯಾನಿಸು TTFLAG_SUPER	    0x40    // object from "super".
#ವ್ಯಾಖ್ಯಾನಿಸು TTFLAG_GENERIC	    0x80    // generic type
#ವ್ಯಾಖ್ಯಾನಿಸು TTFLAG_TUPLE_OK	    0x100   // tuple can be used for a list

#ವ್ಯಾಖ್ಯಾನಿಸು IS_GENERIC_TYPE(type)	\
    ((type->tt_flags & TTFLAG_GENERIC) == TTFLAG_GENERIC)

// Type check flags
#ವ್ಯಾಖ್ಯಾನಿಸು TYPECHK_ಸಂಖ್ಯೆ_OK	0x1	// number is accepted for a float
#ವ್ಯಾಖ್ಯಾನಿಸು TYPECHK_TUPLE_OK	0x2	// tuple is accepted for a list

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
    VIM_ACCESS_ಖಾಸಗಿ,	// read/write only inside the class
    VIM_ACCESS_READ,	// read everywhere, write only inside the class
    VIM_ACCESS_ALL	// read/write everywhere
} omacc_T;

#ವ್ಯಾಖ್ಯಾನಿಸು OCMFLAG_HAS_TYPE	0x01	// type specified explicitly
#ವ್ಯಾಖ್ಯಾನಿಸು OCMFLAG_FINAL		0x02	// "final" object/class member
#ವ್ಯಾಖ್ಯಾನಿಸು OCMFLAG_CONST		0x04	// "const" object/class member

/*
 * Object methods called by builtin functions (e.g. string(), empty(), etc.)
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
    CLASS_BUILTIN_INVALID,
    CLASS_BUILTIN_STRING,
    CLASS_BUILTIN_ಖಾಲಿಗಳು,
    CLASS_BUILTIN_LEN,
    CLASS_BUILTIN_MAX
} class_builtin_T;

/*
 * Entry for an object or class member variable.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    string_T	ocm_name;	// allocated
    omacc_T	ocm_access;
    type_T	*ocm_type;
    ಪೂರ್ಣಾಂಕ		ocm_flags;
    char_u	*ocm_init;	// allocated
    sctx_T	ocm_init_sctx;	// script context of the initializer expression
} ocmember_T;

// used for the lookup table of a class member index and object method index
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ itf2class_S itf2class_T;
ರಚನೆ itf2class_S {
    itf2class_T	*i2c_ಮುಂದಿನ;
    class_T	*i2c_class;
    ಪೂರ್ಣಾಂಕ		i2c_is_ವಿಧಾನ;	    // TRUE for method indexes
    // array with ints follows
};

#ವ್ಯಾಖ್ಯಾನಿಸು CLASS_INTERFACE	    0x1
#ವ್ಯಾಖ್ಯಾನಿಸು CLASS_EXTENDED	    0x2	    // another class extends this one
#ವ್ಯಾಖ್ಯಾನಿಸು CLASS_ABSTRACT	    0x4	    // abstract class
#ವ್ಯಾಖ್ಯಾನಿಸು CLASS_ENUM	    0x8	    // enum

// "class_T": used for v_class of typval of VAR_CLASS
// Also used for an interface (class_flags has CLASS_INTERFACE).
ರಚನೆ class_S
{
    string_T	class_name;		// allocated
    ಪೂರ್ಣಾಂಕ		class_flags;		// CLASS_ flags

    ಪೂರ್ಣಾಂಕ		class_refcount;
    ಪೂರ್ಣಾಂಕ		class_copyID;		// used by garbage collection
    class_T	*class_ಮುಂದಿನ_ಬಳಸಿದ;	// for list headed by "first_class"
    class_T	*class_prev_ಬಳಸಿದ;	// for list headed by "first_class"

    class_T	*class_extends;		// parent class or NULL

    // interfaces declared for the class
    ಪೂರ್ಣಾಂಕ		class_interface_ಎಣಿಕೆ;
    char_u	**class_interfaces;	// allocated array of names
    class_T	**class_interfaces_cl;	// interfaces (counts as reference)
    itf2class_T	*class_itf2class;	// member index lookup tables

    // class members: "static varname"
    ಪೂರ್ಣಾಂಕ		class_class_member_ಎಣಿಕೆ;
    ocmember_T	*class_class_members;	// allocated
    typval_T	*class_members_tv;	// allocated array of class member vals

    // class functions: "static def SomeMethod()"
    ಪೂರ್ಣಾಂಕ		class_class_function_ಎಣಿಕೆ;	    // total count
    ಪೂರ್ಣಾಂಕ		class_class_function_ಎಣಿಕೆ_ಮಗು;   // count without "extends"
    ufunc_T	**class_class_functions;	// allocated

    // object members: "this.varname"
    ಪೂರ್ಣಾಂಕ		class_obj_member_ಎಣಿಕೆ;
    ocmember_T	*class_obj_members;	// allocated

    // object methods: "def SomeMethod()"
    ಪೂರ್ಣಾಂಕ		class_obj_ವಿಧಾನ_ಎಣಿಕೆ;		    // total count
    ಪೂರ್ಣಾಂಕ		class_obj_ವಿಧಾನ_ಎಣಿಕೆ_ಮಗು;	    // count without "extends"
    ufunc_T	**class_obj_methods;	// allocated

					// index of builtin methods
    ಪೂರ್ಣಾಂಕ		class_builtin_methods[CLASS_BUILTIN_MAX];

    garray_T	class_type_list;	// used for type pointers
    type_T	class_type;		// type used for the class
    type_T	class_object_type;	// same as class_type but VAR_OBJECT
};

#ವ್ಯಾಖ್ಯಾನಿಸು IS_INTERFACE(cl)	((cl)->class_flags & CLASS_INTERFACE)
#ವ್ಯಾಖ್ಯಾನಿಸು IS_ENUM(cl)		((cl)->class_flags & CLASS_ENUM)

// Used for v_object of typval of VAR_OBJECT.
// The member variables follow in an array of typval_T.
ರಚನೆ object_S
{
    class_T	*obj_class;	    // class this object is created for;
				    // pointer adds to class_refcount
    ಪೂರ್ಣಾಂಕ		obj_refcount;

    object_T	*obj_ಮುಂದಿನ_ಬಳಸಿದ;	    // for list headed by "first_object"
    object_T	*obj_prev_ಬಳಸಿದ;	    // for list headed by "first_object"
    ಪೂರ್ಣಾಂಕ		obj_copyID;	    // used by garbage collection
};

ರಚನೆ typealias_S
{
    ಪೂರ್ಣಾಂಕ	    ta_refcount;
    type_T  *ta_type;
    char_u  *ta_name;
    ಪೂರ್ಣಾಂಕ	    ta_ಆಳ;	    // number of levels of "ta_type"
};

/*
 * Structure to hold an internal variable without a name.
 */
ರಚನೆ typval_S
{
    vartype_T	v_type;
    ಅಕ್ಷರ	v_ಬೀಗ;	    // see below: VAR_LOCKED, VAR_FIXED
    ಒಕ್ಕೂಟ
    {
	varnumber_T	v_ಸಂಖ್ಯೆ;	// number value
	float_T		v_float;	// floating point number value
	char_u		*v_string;	// string value (can be NULL)
	list_T		*v_list;	// list value (can be NULL)
	dict_T		*v_dict;	// dict value (can be NULL)
	partial_T	*v_partial;	// closure: function with args
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
	ಕೆಲಸ_T		*v_ಕೆಲಸ;		// job value (can be NULL)
	channel_T	*v_channel;	// channel value (can be NULL)
#ಆದರೆ_ಅಂತ್ಯ
	blob_T		*v_blob;	// blob value (can be NULL)
	instr_T		*v_instr;	// instructions to execute
	class_T		*v_class;	// class value (can be NULL)
	object_T	*v_object;	// object value (can be NULL)
	typealias_T	*v_typealias;	// user-defined type name
	tuple_T		*v_tuple;	// tuple
    }		vval;
};

// Values for "dv_scope".
#ವ್ಯಾಖ್ಯಾನಿಸು VAR_SCOPE     1	// a:, v:, s:, etc. scope dictionaries
#ವ್ಯಾಖ್ಯಾನಿಸು VAR_DEF_SCOPE 2	// l:, g: scope dictionaries: here funcrefs are not
			// allowed to mask existing functions

// Values for "v_lock".
#ವ್ಯಾಖ್ಯಾನಿಸು VAR_LOCKED	    1	// locked with lock(), can use unlock()
#ವ್ಯಾಖ್ಯಾನಿಸು VAR_FIXED	    2	// locked forever
#ವ್ಯಾಖ್ಯಾನಿಸು VAR_ವಸ್ತುಗಳು_LOCKED    4	// items of non-materialized list locked

/*
 * Structure to hold an item of a list: an internal variable without a name.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ listitem_S listitem_T;

ರಚನೆ listitem_S
{
    listitem_T	*li_ಮುಂದಿನ;	// next item in list
    listitem_T	*li_prev;	// previous item in list
    typval_T	li_tv;		// type and value of the variable
};

// Struct used by those that are using an item in a list.
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ listwatch_S listwatch_T;

ರಚನೆ listwatch_S
{
    listitem_T		*lw_ಅಂಶ;	// item being watched
    listwatch_T		*lw_ಮುಂದಿನ;	// next watcher
};

/*
 * Structure to hold info about a list.
 * Order of members is optimized to reduce padding.
 * When created by range() it will at first have special value:
 *  lv_first == &range_list_item;
 * and use lv_start, lv_end, lv_stride.
 */
ರಚನೆ listvar_S
{
    listitem_T	*lv_ಮೊದಲ;	// first item, NULL if none, &range_list_item
				// for a non-materialized list
    listwatch_T	*lv_watch;	// first watcher, NULL if none
    ಒಕ್ಕೂಟ {
	ರಚನೆ {	// used for non-materialized range list:
			// "lv_first" is &range_list_item
	    varnumber_T lv_ಆರಂಭ;
	    varnumber_T lv_ಕೊನೆ;
	    ಪೂರ್ಣಾಂಕ		lv_stride;
	} nonmat;
	ರಚನೆ {	// used for materialized list
	    listitem_T	*lv_ಕೊನೆಯ;	// last item, NULL if none
	    listitem_T	*lv_idx_ಅಂಶ;	// when not NULL item at index "lv_idx"
	    ಪೂರ್ಣಾಂಕ		lv_idx;		// cached index of an item
	} mat;
    } lv_u;
    type_T	*lv_type;	// current type, allocated by alloc_type()
    list_T	*lv_copylist;	// copied list used by deepcopy()
    list_T	*lv_ಬಳಸಿದ_ಮುಂದಿನ;	// next list in used lists list
    list_T	*lv_ಬಳಸಿದ_prev;	// previous list in used lists list
    ಪೂರ್ಣಾಂಕ		lv_refcount;	// reference count
    ಪೂರ್ಣಾಂಕ		lv_len;		// number of items
    ಪೂರ್ಣಾಂಕ		lv_with_ವಸ್ತುಗಳು;	// number of items following this struct that
				// should not be freed
    ಪೂರ್ಣಾಂಕ		lv_copyID;	// ID used by deepcopy()
    ಅಕ್ಷರ	lv_ಬೀಗ;	// zero, VAR_LOCKED, VAR_FIXED
};

/*
 * Static list with 10 items.  Use init_static_list() to initialize.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    list_T	sl_list;	// must be first
    listitem_T	sl_ವಸ್ತುಗಳು[10];
} staticList10_T;

/*
 * Structure to hold an item of a Dictionary.
 * Also used for a variable.
 * The key is copied into "di_key" to avoid an extra alloc/free for it.
 */
ರಚನೆ dictitem_S
{
    typval_T	di_tv;		// type and value of the variable
    char_u	di_flags;	// DI_FLAGS_ flags (only used for variable)
    char_u	di_ಕೀಲಿ[1];	// key (actually longer!)
};
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ dictitem_S dictitem_T;

/*
 * A dictitem with a 16 character key (plus NUL).  This is an efficient way to
 * have a fixed-size dictitem.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು DICTITEM16_ಕೀಲಿ_LEN 16
ರಚನೆ dictitem16_S
{
    typval_T	di_tv;		// type and value of the variable
    char_u	di_flags;	// DI_FLAGS_ flags (only used for variable)
    char_u	di_ಕೀಲಿ[DICTITEM16_ಕೀಲಿ_LEN + 1];	//ಕೀಲಿ
};
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ dictitem16_S dictitem16_T;

// Flags for "di_flags"
#ವ್ಯಾಖ್ಯಾನಿಸು DI_FLAGS_RO	   0x01	    // read-only variable
#ವ್ಯಾಖ್ಯಾನಿಸು DI_FLAGS_RO_SBX	   0x02	    // read-only in the sandbox
#ವ್ಯಾಖ್ಯಾನಿಸು DI_FLAGS_FIX	   0x04	    // fixed: no :unlet or remove()
#ವ್ಯಾಖ್ಯಾನಿಸು DI_FLAGS_ಬೀಗ	   0x08	    // locked variable
#ವ್ಯಾಖ್ಯಾನಿಸು DI_FLAGS_ALLOC	   0x10	    // separately allocated
#ವ್ಯಾಖ್ಯಾನಿಸು DI_FLAGS_RELOAD	   0x20	    // set when script sourced again

/*
 * Structure to hold info about a Dictionary.
 */
ರಚನೆ dictvar_S
{
    ಅಕ್ಷರ	dv_ಬೀಗ;	// zero, VAR_LOCKED, VAR_FIXED
    ಅಕ್ಷರ	dv_scope;	// zero, VAR_SCOPE, VAR_DEF_SCOPE
    ಪೂರ್ಣಾಂಕ		dv_refcount;	// reference count
    ಪೂರ್ಣಾಂಕ		dv_copyID;	// ID used by deepcopy()
    hashtab_T	dv_hashtab;	// hashtab that refers to the items
    type_T	*dv_type;	// current type, allocated by alloc_type()
    dict_T	*dv_copydict;	// copied dict used by deepcopy()
    dict_T	*dv_ಬಳಸಿದ_ಮುಂದಿನ;	// next dict in used dicts list
    dict_T	*dv_ಬಳಸಿದ_prev;	// previous dict in used dicts list
};

/*
 * Structure to hold info about a blob.
 */
ರಚನೆ blobvar_S
{
    garray_T	bv_ga;		// growarray with the data
    ಪೂರ್ಣಾಂಕ		bv_refcount;	// reference count
    ಅಕ್ಷರ	bv_ಬೀಗ;	// zero, VAR_LOCKED, VAR_FIXED
};

/*
 * Structure to hold info about a tuple.
 */
ರಚನೆ tuplevar_S
{
    garray_T	tv_ವಸ್ತುಗಳು;	// tuple items
    type_T	*tv_type;	// current type, allocated by alloc_type()
    tuple_T	*tv_copytuple;	// copied tuple used by deepcopy()
    tuple_T	*tv_ಬಳಸಿದ_ಮುಂದಿನ;	// next tuple in used tuples list
    tuple_T	*tv_ಬಳಸಿದ_prev;	// previous tuple in used tuples list
    ಪೂರ್ಣಾಂಕ		tv_refcount;	// reference count
    ಪೂರ್ಣಾಂಕ		tv_copyID;	// ID used by deepcopy()
    ಅಕ್ಷರ	tv_ಬೀಗ;	// zero, VAR_LOCKED, VAR_FIXED
};

/*
 * Structure to hold a generic type information
 */
ರಚನೆ generictype_S
{
    type_T	*gt_type;	// generic or concrete type
    char_u	*gt_name;	// type name
};

/*
 * Generic function args table
 */
ರಚನೆ gfargs_tab_S
{
    garray_T	gfat_ವಾದಗಳು;
    garray_T	gfat_param_types;
    garray_T	gfat_arg_types;
};

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ (*cfunc_T)(ಪೂರ್ಣಾಂಕ argcount, typval_T *argvars, typval_T *rettv, ಶೂನ್ಯ *state);
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಶೂನ್ಯ (*cfunc_free_T)(ಶೂನ್ಯ *state);

// type of getline() last argument
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
    GETLINE_NONE,	    // do not concatenate any lines
    GETLINE_CONCAT_CONT,    // concatenate continuation lines with backslash
    GETLINE_CONCAT_CONTBAR, // concatenate continuation lines with \ and |
    GETLINE_CONCAT_ALL	    // concatenate continuation and Vim9 # comment lines
} getline_opt_T;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ svar_S svar_T;

#ಆದರೆ defined(FEAT_EVAL)
/*
 * Info used by a ":for" loop.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    ಪೂರ್ಣಾಂಕ		fi_semicolon;	// TRUE if ending in '; var]'
    ಪೂರ್ಣಾಂಕ		fi_varcount;	// nr of variables in [] or zero
    ಪೂರ್ಣಾಂಕ		fi_break_ಎಣಿಕೆ;	// nr of line breaks encountered
    listwatch_T	fi_lw;		// keep an eye on the item used.
    list_T	*fi_list;	// list being used
    ಪೂರ್ಣಾಂಕ		fi_bi;		// index of blob
    blob_T	*fi_blob;	// blob being used
    char_u	*fi_string;	// copy of string being used
    ಪೂರ್ಣಾಂಕ		fi_byte_idx;	// byte index in fi_string
    tuple_T	*fi_tuple;	// tuple being used
    ಪೂರ್ಣಾಂಕ		fi_tuple_idx;	// tuple index in fi_tuple
    ಪೂರ್ಣಾಂಕ		fi_cs_flags;	// cs_flags or'ed together
} forinfo_T;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ funccall_S funccall_T;

// values used for "uf_def_status"
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
    UF_NOT_COMPILED,	    // executed with interpreter
    UF_ಗಮ್ಯ_BE_COMPILED,	    // to be compiled before execution
    UF_COMPILING,	    // in compile_def_function()
    UF_COMPILED,	    // successfully compiled
    UF_COMPILE_ERROR	    // compilation error, cannot execute
} def_ಸ್ಥಿತಿ_T;

/*
 * Structure to hold info for a user function.
 * When adding a field check copy_lambda_to_global_func().
 */
ರಚನೆ ufunc_S
{
    ಪೂರ್ಣಾಂಕ		uf_varargs;	// variable nr of arguments (old style)
    ಪೂರ್ಣಾಂಕ		uf_flags;	// FC_ flags
    ಪೂರ್ಣಾಂಕ		uf_calls;	// nr of active calls
    ಪೂರ್ಣಾಂಕ		uf_cleared;	// func_clear() was already called
    def_ಸ್ಥಿತಿ_T uf_def_ಸ್ಥಿತಿ; // UF_NOT_COMPILED, UF_TO_BE_COMPILED, etc.
    ಪೂರ್ಣಾಂಕ		uf_dfunc_idx;	// only valid if uf_def_status is UF_COMPILED

    class_T	*uf_class;	// for class/object method and constructor;
				// does not count for class_refcount.
				// class of the object which is invoking this
				// function.
    class_T	*uf_defclass;	// class where this function is defined.

    garray_T	uf_ವಾದಗಳು;	// arguments, including optional arguments
    garray_T	uf_def_ವಾದಗಳು;	// default argument expressions
    ಪೂರ್ಣಾಂಕ		uf_ವಾದಗಳು_visible; // normally uf_args.ga_len, less when
				 // compiling default argument expression.

    // for :def (for :function uf_ret_type is NULL)
    type_T	**uf_arg_types;	// argument types (count == uf_args.ga_len)
    type_T	*uf_ret_type;	// return type
    garray_T	uf_type_list;	// types used in arg and return types
    partial_T	*uf_partial;	// for closure created inside :def function:
				// information about the context

    char_u	*uf_va_name;	// name from "...name" or NULL
    type_T	*uf_va_type;	// type from "...name: type" or NULL
    type_T	*uf_func_type;	// type of the function, &t_func_any if unknown
    ಪೂರ್ಣಾಂಕ		uf_ಖಂಡ_ಆಳ;	// nr of entries in uf_block_ids
    ಪೂರ್ಣಾಂಕ		*uf_ಖಂಡ_ids;	// blocks a :def function is defined inside
# ಆದರೆ defined(FEAT_LUA)
    cfunc_T     uf_cb;		// callback function for cfunc
    cfunc_free_T uf_cb_free;    // callback function to free cfunc
    ಶೂನ್ಯ	*uf_cb_state;   // state of uf_cb
# endif

    // for generic functions
    ಪೂರ್ಣಾಂಕ		uf_generic_argcount;// type argument count
    generic_T	*uf_generic_ವಾದಗಳು;   // generic types
    type_T	*uf_generic_param_types; // list of allocated generic types
    garray_T	uf_generic_arg_types; // list of allocated type arguments
    hashtab_T	uf_generic_functab; // generic function table

    garray_T	uf_ಸಾಲುಗಳು;	// function lines

    ಪೂರ್ಣಾಂಕ		uf_debug_tick;	// when last checked for a breakpoint in this
				// function.
    ಪೂರ್ಣಾಂಕ		uf_has_breakpoint;  // TRUE when a breakpoint has been set in
				    // this function.
# ifdef FEAT_PROFILE
    ಪೂರ್ಣಾಂಕ		uf_profiling;	// TRUE when func is being profiled
    ಪೂರ್ಣಾಂಕ		uf_prof_initialized;
    hash_T	uf_hash;	// hash for uf_name when profiling
    // profiling the function as a whole
    ಪೂರ್ಣಾಂಕ		uf_tm_ಎಣಿಕೆ;	// nr of calls
    proftime_T	uf_tm_ಒಟ್ಟು;	// time spent in function + children
    proftime_T	uf_tm_self;	// time spent in function itself
    proftime_T	uf_tm_children;	// time spent in children this call
    // profiling the function per line
    ಪೂರ್ಣಾಂಕ		*uf_tml_ಎಣಿಕೆ;	// nr of times line was executed
    proftime_T	*uf_tml_ಒಟ್ಟು;	// time spent in a line + children
    proftime_T	*uf_tml_self;	// time spent in a line itself
    proftime_T	uf_tml_ಆರಂಭ;	// start time for current line
    proftime_T	uf_tml_children; // time spent in children for this line
    proftime_T	uf_tml_wait;	// start wait time for current line
    ಪೂರ್ಣಾಂಕ		uf_tml_idx;	// index of line being timed; -1 if none
    ಪೂರ್ಣಾಂಕ		uf_tml_execed;	// line being timed was executed
# endif
    sctx_T	uf_script_ctx;	// SCTX where function was defined,
				// used for s: variables; sc_version changed
				// for :function
    ಪೂರ್ಣಾಂಕ		uf_script_ctx_ಆವೃತ್ತಿ;  // original sc_version of SCTX
    ಪೂರ್ಣಾಂಕ		uf_refcount;	// reference count, see func_name_refcount()

    funccall_T	*uf_scoped;	// l: local variables for closure

    char_u	*uf_name_exp;	// if "uf_name[]" starts with SNR the name with
				// "<SNR>" as a string, otherwise NULL
    size_t	uf_namelen;	// length of uf_name (excluding the NUL)
    char_u	uf_name[4];	// name of function (actual size equals name);
				// can start with <SNR>123_ (<SNR> is K_SPECIAL
				// KS_EXTRA KE_SNR)
};

// flags used in uf_flags
# define FC_ABORT    0x01	// abort function on error
# define FC_RANGE    0x02	// function accepts range
# define FC_DICT	    0x04	// Dict function, uses "self"
# define FC_ಆವರಣ  0x08	// closure, uses outer scope variables
# define FC_DELETED  0x10	// :delfunction used while uf_refcount > 0
# define FC_REMOVED  0x20	// function redefined while uf_refcount > 0
# define FC_SANDBOX  0x40	// function defined in the sandbox
# define FC_DEAD	    0x80	// function kept only for reference to dfunc
# define FC_EXPORT   0x100	// "export def Func()"
# define FC_NOARGS   0x200	// no a: variables in lambda
# define FC_VIM9	    0x400	// defined in Vim9 script file
# define FC_CFUNC    0x800	// defined as Lua C func
# define FC_COPY	    0x1000	// copy of another function by
				// copy_lambda_to_global_func()
# define FC_LAMBDA   0x2000	// one line "return {expr}"

# define FC_OBJECT   0x4000	// object method
# define FC_NEW	    0x8000	// constructor
# define FC_ABSTRACT 0x10000	// abstract method
# define FC_GENERIC  0x20000	// generic function

// Is "ufunc" an object method?
# define IS_OBJECT_ವಿಧಾನ(ufunc) ((ufunc->uf_flags & FC_OBJECT) == FC_OBJECT)
// Is "ufunc" a class new() constructor method?
# define IS_CONSTRUCTOR_ವಿಧಾನ(ufunc) ((ufunc->uf_flags & FC_NEW) == FC_NEW)
// Is "ufunc" an abstract class method?
# define IS_ABSTRACT_ವಿಧಾನ(ufunc) ((ufunc->uf_flags & FC_ABSTRACT) == FC_ABSTRACT)
# define IS_GENERIC_FUNC(ufunc) (((ufunc)->uf_flags & FC_GENERIC) == FC_GENERIC)

# define MAX_FUNC_ವಾದಗಳು	20	// maximum number of function arguments
# define VAR_SHORT_LEN	20	// short variable name length
# define FIXVAR_CNT	12	// number of fixed variables

/*
 * Structure to hold info for a function that is currently being executed.
 */
ರಚನೆ funccall_S
{
    ufunc_T	*fc_func;	// function being called
    ಪೂರ್ಣಾಂಕ		fc_linenr;	// next line to be executed
    ಪೂರ್ಣಾಂಕ		fc_returned;	// ":return" used
    ರಚನೆ			// fixed variables for arguments
    {
	dictitem_T	var;		// variable (without room for name)
	char_u	ಸ್ಥಳ[VAR_SHORT_LEN];	// room for the name
    } fc_fixvar[FIXVAR_CNT];
    dict_T	fc_l_vars;	// l: local function variables
    dictitem_T	fc_l_vars_var;	// variable for l: scope
    dict_T	fc_l_avars;	// a: argument variables
    dictitem_T	fc_l_avars_var;	// variable for a: scope
    list_T	fc_l_varlist;	// list for a:000
    listitem_T	fc_l_listitems[MAX_FUNC_ವಾದಗಳು];	// listitems for a:000
    typval_T	*fc_rettv;	// return value
    linenr_T	fc_breakpoint;	// next line with breakpoint or zero
    ಪೂರ್ಣಾಂಕ		fc_dbg_tick;	// debug_tick when breakpoint was set
    ಪೂರ್ಣಾಂಕ		fc_level;	// top nesting level of executed function

    garray_T	fc_defer;	// functions to be called on return
    ectx_T	*fc_ectx;	// execution context for :def function, NULL
				// otherwise

# ifdef FEAT_PROFILE
    proftime_T	fc_prof_ಮಗು;	// time spent in a child
# endif
    funccall_T	*fc_caller;	// calling function or NULL; or next funccal in
				// list pointed to by previous_funccal.

    // for closure
    ಪೂರ್ಣಾಂಕ		fc_refcount;	// number of user functions that reference this
				// funccal
    ಪೂರ್ಣಾಂಕ		fc_copyID;	// for garbage collection
    garray_T	fc_ufuncs;	// list of ufunc_T* which keep a reference to
				// "fc_func"
};

// structure used as item in "fc_defer"
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    char_u	*dr_name;	// function name, allocated
    typval_T	dr_argvars[MAX_FUNC_ವಾದಗಳು + 1];
    ಪೂರ್ಣಾಂಕ		dr_argcount;
} defer_T;

/*
 * Struct used by trans_function_name()
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    dict_T	*fd_dict;	// Dictionary used
    char_u	*fd_newkey;	// new key in "dict" in allocated memory
    dictitem_T	*fd_di;		// Dictionary item used
} funcdict_T;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ funccal_ನಮೂದು funccal_ನಮೂದು_T;
ರಚನೆ funccal_ನಮೂದು {
    ಶೂನ್ಯ	    *ತುದಿ_funccal;
    funccal_ನಮೂದು_T *ಮುಂದಿನ;
};

// From user function to hashitem and back.
# define UF2HIKEY(fp) ((fp)->uf_name)
# define HIKEY2UF(p)  ((ufunc_T *)((p) - offsetof(ufunc_T, uf_name)))
# define HI2UF(hi)     HIKEY2UF((hi)->hi_ಕೀಲಿ)

/*
 * Holds the hashtab with variables local to each sourced script.
 * Each item holds a variable (nameless) that points to the dict_T.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    dictitem_T	sv_var;
    dict_T	sv_dict;
} scriptvar_T;

/*
 * Entry for "sn_all_vars".  Contains the s: variables from sn_vars plus the
 * block-local ones.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ sallvar_S sallvar_T;
ರಚನೆ sallvar_S {
    sallvar_T	*sav_ಮುಂದಿನ;	  // var with same name but different block
    ಪೂರ್ಣಾಂಕ		sav_ಖಂಡ_id;	  // block ID where declared
    ಪೂರ್ಣಾಂಕ		sav_var_vals_idx; // index in sn_var_vals

    // So long as the variable is valid (block it was defined in is still
    // active) "sav_di" is used.  It is set to NULL when leaving the block,
    // then sav_tv and sav_flags are used.
    dictitem_T *sav_di;		// dictitem with di_key and di_tv
    typval_T	sav_tv;		// type and value of the variable
    char_u	sav_flags;	// DI_FLAGS_ flags (only used for variable)
    char_u	sav_ಕೀಲಿ[1];	// key (actually longer!)
};

/*
 * In the sn_all_vars hashtab item "hi_key" points to "sav_key" in a sallvar_T.
 * This makes it possible to store and find the sallvar_T.
 * SAV2HIKEY() converts a sallvar_T pointer to a hashitem key pointer.
 * HIKEY2SAV() converts a hashitem key pointer to a sallvar_T pointer.
 * HI2SAV() converts a hashitem pointer to a sallvar_T pointer.
 */
# define SAV2HIKEY(sav) ((sav)->sav_ಕೀಲಿ)
# define HIKEY2SAV(p)  ((sallvar_T *)(p - offsetof(sallvar_T, sav_ಕೀಲಿ)))
# define HI2SAV(hi)     HIKEY2SAV((hi)->hi_ಕೀಲಿ)

# define SVFLAG_TYPE_ALLOCATED	1  // call free_type() for "sv_type"
# define SVFLAG_EXPORTED		2  // "export let var = val"
# define SVFLAG_ASSIGNED		4  // assigned a value

/*
 * Entry for "sn_var_vals".  Used for script-local variables.
 */
ರಚನೆ svar_S {
    char_u	*sv_name;	// points into "sn_all_vars" di_key
    typval_T	*sv_tv;		// points into "sn_vars" or "sn_all_vars" di_tv
    type_T	*sv_type;
    ಪೂರ್ಣಾಂಕ		sv_flags;	// SVFLAG_ values above
    ಪೂರ್ಣಾಂಕ		sv_const;	// 0, ASSIGN_CONST or ASSIGN_FINAL
};

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    char_u	*imp_name;	    // name imported as (allocated)
    scid_T	imp_sid;	    // script ID of "from"
    ಪೂರ್ಣಾಂಕ		imp_flags;	    // IMP_FLAGS_ values
} imported_T;

# define IMP_FLAGS_RELOAD	2   // script reloaded, OK to redefine
# define IMP_FLAGS_AUTOLOAD	4   // script still needs to be loaded

/*
 * Info about an encountered script.
 * When sn_state has SN_STATE_NOT_LOADED, it has not been sourced yet.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    char_u	*sn_name;	    // full path of script file
    ಪೂರ್ಣಾಂಕ		sn_script_seq;	    // latest sctx_T sc_seq value

    // When non-zero the script ID of the actually sourced script.  Used if a
    // script is used by a name which has a symlink, we list both names, but
    // only the linked-to script is actually sourced.
    ಪೂರ್ಣಾಂಕ		sn_sourced_sid;

    // "sn_vars" stores the s: variables currently valid.  When leaving a block
    // variables local to that block are removed.
    scriptvar_T	*sn_vars;

    // Specific for a Vim9 script.
    // "sn_all_vars" stores all script variables ever declared.  So long as the
    // variable is still valid the value is in "sn_vars->sv_dict...di_tv".
    // When the block of a declaration is left the value is moved to
    // "sn_all_vars..sav_tv".
    // Variables with duplicate names are possible, the sav_block_id must be
    // used to check that which variable is valid.
    dict_T	sn_all_vars;	// all script variables, dict of sallvar_T

    // Stores the same variables as in "sn_all_vars" as a list of svar_T, so
    // that they can be quickly found by index instead of a hash table lookup.
    // Also stores the type.
    garray_T	sn_var_vals;

    garray_T	sn_imports;	// imported items, imported_T
    garray_T	sn_type_list;	// keeps types used by variables
    ಪೂರ್ಣಾಂಕ		sn_current_ಖಂಡ_id; // ID for current block, 0 for outer
    ಪೂರ್ಣಾಂಕ		sn_ಕೊನೆಯ_ಖಂಡ_id;  // Unique ID for each script block

    ಪೂರ್ಣಾಂಕ		sn_ಆವೃತ್ತಿ;	// :scriptversion
    ಪೂರ್ಣಾಂಕ		sn_state;	// SN_STATE_ values
    char_u	*sn_save_cpo;	// 'cpo' value when :vim9script found
    ಅಕ್ಷರ	sn_is_vimrc;	// .vimrc file, do not restore 'cpo'
    ಅಕ್ಷರ	sn_syml_checked;// flag: this has been checked for sym link

    // for a Vim9 script under "rtp/autoload/" this is "dir#scriptname#"
    char_u	*sn_autoload_ಪೂರ್ವಪ್ರತ್ಯಯ;

    // TRUE for a script used with "import autoload './dirname/script.vim'"
    // For "../autoload/script.vim" sn_autoload_prefix is also set.
    ಪೂರ್ಣಾಂಕ		sn_import_autoload;

# ifdef FEAT_PROFILE
    ಪೂರ್ಣಾಂಕ		sn_prof_on;	// TRUE when script is/was profiled
    ಪೂರ್ಣಾಂಕ		sn_pr_force;	// forceit: profile functions in this script
    proftime_T	sn_pr_ಮಗು;	// time set when going into first child
    ಪೂರ್ಣಾಂಕ		sn_pr_nest;	// nesting for sn_pr_child
    // profiling the script as a whole
    ಪೂರ್ಣಾಂಕ		sn_pr_ಎಣಿಕೆ;	// nr of times sourced
    proftime_T	sn_pr_ಒಟ್ಟು;	// time spent in script + children
    proftime_T	sn_pr_self;	// time spent in script itself
    proftime_T	sn_pr_ಆರಂಭ;	// time at script start
    proftime_T	sn_pr_children; // time in children after script start
    // profiling the script per line
    garray_T	sn_prl_ga;	// things stored for every line
    proftime_T	sn_prl_ಆರಂಭ;	// start time for current line
    proftime_T	sn_prl_children; // time spent in children for this line
    proftime_T	sn_prl_wait;	// wait start time for current line
    ಪೂರ್ಣಾಂಕ		sn_prl_idx;	// index of line being timed; -1 if none
    ಪೂರ್ಣಾಂಕ		sn_prl_execed;	// line being timed was executed
# endif
} scriptitem_T;

# define SN_STATE_NEW		0   // newly loaded script, nothing done
# define SN_STATE_NOT_LOADED	1   // script located but not loaded
# define SN_STATE_RELOAD		2   // script loaded before, nothing done
# define SN_STATE_HAD_COMMAND	9   // a command was executed

// Struct passed through eval() functions.
// See EVALARG_EVALUATE for a fixed value with eval_flags set to EVAL_EVALUATE.
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಪೂರ್ಣಾಂಕ		eval_flags;	    // EVAL_ flag values below
    ಪೂರ್ಣಾಂಕ		eval_break_ಎಣಿಕೆ;   // nr of line breaks consumed

    // copied from exarg_T when "getline" is "getsourceline". Can be NULL.
    char_u	*(*eval_getline)(ಪೂರ್ಣಾಂಕ, ಶೂನ್ಯ *, ಪೂರ್ಣಾಂಕ, getline_opt_T);
    ಶೂನ್ಯ	*eval_cookie;	    // argument for eval_getline()

    // used when compiling a :def function, NULL otherwise
    cctx_T	*eval_cctx;

    // used when executing commands from a script, NULL otherwise
    cstack_T	*eval_cstack;

    // Used to collect lines while parsing them, so that they can be
    // concatenated later.  Used when "eval_ga.ga_itemsize" is not zero.
    // "eval_ga.ga_data" is a list of pointers to lines.
    // "eval_freega" list pointers that need to be freed after concatenating.
    garray_T	eval_ga;
    garray_T	eval_freega;

    // pointer to the last line obtained with getsourceline()
    char_u	*eval_tofree;

    // array with lines of an inline function
    garray_T	eval_tofree_ga;

    // set when "arg" points into the last entry of "eval_tofree_ga"
    ಪೂರ್ಣಾಂಕ		eval_using_cmdline;

    // pointer to the lines concatenated for a lambda.
    char_u	*eval_tofree_lambda;

    // pointer to name of class being constructed
    class_T	*eval_class;
} evalarg_T;

// Flag for expression evaluation.
# define EVAL_EVALUATE	    1	    // when missing don't actually evaluate

# ifdef FEAT_PROFILE
/*
 * Struct used in sn_prl_ga for every line of a script.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ sn_prl_S
{
    ಪೂರ್ಣಾಂಕ		snp_ಎಣಿಕೆ;	// nr of times line was executed
    proftime_T	sn_prl_ಒಟ್ಟು;	// time spent in a line + children
    proftime_T	sn_prl_self;	// time spent in a line itself
} sn_prl_T;

#  define PRL_ಅಂಶ(si, idx)	(((sn_prl_T *)(si)->sn_prl_ga.ga_ದತ್ತಾಂಶ)[(idx)])

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಪೂರ್ಣಾಂಕ		pi_started_profiling;
    proftime_T	pi_wait_ಆರಂಭ;
    proftime_T	pi_call_ಆರಂಭ;
} profinfo_T;

# ಇಲ್ಲದಿದ್ದರೆ
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    ಪೂರ್ಣಾಂಕ	    dummy;
} profinfo_T;
# endif
#ಇಲ್ಲದಿದ್ದರೆ
// dummy typedefs for use in function prototypes
ರಚನೆ ufunc_S
{
    ಪೂರ್ಣಾಂಕ	    dummy;
};
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    ಪೂರ್ಣಾಂಕ	    dummy;
} funccall_T;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    ಪೂರ್ಣಾಂಕ	    dummy;
} funcdict_T;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    ಪೂರ್ಣಾಂಕ	    dummy;
} funccal_ನಮೂದು_T;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    ಪೂರ್ಣಾಂಕ	    dummy;
} scriptitem_T;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    ಪೂರ್ಣಾಂಕ	    dummy;
} evalarg_T;
#ಆದರೆ_ಅಂತ್ಯ

// Struct passed between functions dealing with function call execution.
//
// "fe_argv_func", when not NULL, can be used to fill in arguments only when the
// invoked function uses them.  It is called like this:
//   new_argcount = fe_argv_func(current_argcount, argv, partial_argcount,
//							called_func)
//
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಪೂರ್ಣಾಂಕ		(* fe_argv_func)(ಪೂರ್ಣಾಂಕ, typval_T *, ಪೂರ್ಣಾಂಕ, ufunc_T *);
    linenr_T	fe_firstline;	// first line of range
    linenr_T	fe_lastline;	// last line of range
    ಪೂರ್ಣಾಂಕ		*fe_doesrange;	// if not NULL: return: function handled range
    ಪೂರ್ಣಾಂಕ		fe_evaluate;	// actually evaluate expressions
    ufunc_T	*fe_ufunc;	// function to be called, when NULL lookup by
				// name
    partial_T	*fe_partial;	// for "dict" and extra arguments
    dict_T	*fe_selfdict;	// Dictionary for "self"
    object_T	*fe_object;	// object, e.g. for "this.Func()"
    typval_T	*fe_basetv;	// base for base->method()
    type_T	*fe_check_type;	// type from funcref or NULL
    ಪೂರ್ಣಾಂಕ		fe_ಸಿಕ್ಕಿತು_var;	// if the function is not found then give an
				// error that a variable is not callable.
    cctx_T	*fe_cctx;	// when compiling a :def function
} funcexe_T;

/*
 * Structure to hold the context of a compiled function, used by closures
 * defined in that function.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ funcstack_S funcstack_T;
ರಚನೆ funcstack_S
{
    funcstack_T *fs_ಮುಂದಿನ;	// linked list at "first_funcstack"
    funcstack_T *fs_prev;

    garray_T	fs_ga;		// contains the stack, with:
				// - arguments
				// - frame
				// - local variables
    ಪೂರ್ಣಾಂಕ		fs_var_ಸ್ಥಾನಾಂತರ;	// count of arguments + frame size == offset to
				// local variables

    ಪೂರ್ಣಾಂಕ		fs_refcount;	// nr of closures referencing this funcstack
    ಪೂರ್ಣಾಂಕ		fs_min_refcount; // nr of closures on this funcstack
    ಪೂರ್ಣಾಂಕ		fs_copyID;	// for garbage collection
};

/*
 * Structure to hold the variables declared in a loop that are possibly used
 * in a closure.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ loopvars_S loopvars_T;
ರಚನೆ loopvars_S
{
    loopvars_T *lvs_ಮುಂದಿನ;	// linked list at "first_loopvars"
    loopvars_T *lvs_prev;

    garray_T	lvs_ga;		// contains the variables
    ಪೂರ್ಣಾಂಕ		lvs_refcount;	// nr of closures referencing this loopvars
    ಪೂರ್ಣಾಂಕ		lvs_min_refcount; // nr of closures on this loopvars
    ಪೂರ್ಣಾಂಕ		lvs_copyID;	// for garbage collection
};

// maximum nesting of :while and :for loops in a :def function
#ವ್ಯಾಖ್ಯಾನಿಸು MAX_LOOP_ಆಳ 10

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ outer_S outer_T;
ರಚನೆ outer_S {
    garray_T	*ಹೊರಹೋಗು_ರಾಶಿ;	    // stack from outer scope, or a copy
				    // containing only arguments and local vars
    ಪೂರ್ಣಾಂಕ		ಹೊರಹೋಗು_ಚೌಕಟ್ಟು_idx;	    // index of stack frame in out_stack
    outer_T	*ಹೊರಹೋಗು_ಮೇಲೆ;	    // outer scope of outer scope or NULL
    partial_T	*ಹೊರಹೋಗು_ಮೇಲೆ_partial;    // partial owning out_up or NULL

    ರಚನೆ {
	garray_T *ರಾಶಿ;	    // stack from outer scope, or a copy
				    // containing only vars inside the loop
	ಗಿಡ್ಡ	 var_idx;	    // first variable defined in a loop in
				    // out_loop_stack
	ಗಿಡ್ಡ	 var_ಎಣಿಕೆ;	    // number of variables defined in a loop
    } ಹೊರಹೋಗು_loop[MAX_LOOP_ಆಳ];
    ಪೂರ್ಣಾಂಕ		ಹೊರಹೋಗು_loop_ಗಾತ್ರ;	    // nr of used entries in out_loop[]
};

ರಚನೆ partial_S
{
    ಪೂರ್ಣಾಂಕ		pt_refcount;	// reference count
    ಪೂರ್ಣಾಂಕ		pt_auto;	// when TRUE the partial was created for using
				// dict.member in handle_subscript()
    char_u	*pt_name;	// function name; when NULL use
				// pt_func->uf_name
    ufunc_T	*pt_func;	// function pointer; when NULL lookup function
				// with pt_name

    // For a compiled closure: the arguments and local variables scope
    outer_T	pt_outer;

    // For a partial of a partial: use pt_outer values of this partial.
    partial_T	*pt_outer_partial;

    funcstack_T	*pt_funcstack;	// copy of stack, used after context
				// function returns
    loopvars_T	*(pt_loopvars[MAX_LOOP_ಆಳ]);
				// copy of loop variables, used after loop
				// block ends

    typval_T	*pt_argv;	// arguments in allocated array
    ಪೂರ್ಣಾಂಕ		pt_argc;	// number of arguments

    ಪೂರ್ಣಾಂಕ		pt_copyID;	// funcstack may contain pointer to partial
    dict_T	*pt_dict;	// dict for "self"
    object_T	*pt_obj;	// object method
};

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಗಿಡ್ಡ	lvi_ಆಳ;	    // current nested loop depth
    ರಚನೆ {
	ಗಿಡ್ಡ	var_idx;	    // index of first variable inside loop
	ಗಿಡ್ಡ	var_ಎಣಿಕೆ;	    // number of variables inside loop
    } lvi_loop[MAX_LOOP_ಆಳ];
} loopvarinfo_T;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ AutoPatCmd_S AutoPatCmd_T;

/*
 * Entry in the execution stack "exestack".
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
    ETYPE_ತುದಿ,		    // toplevel
    ETYPE_SCRIPT,	    // sourcing script, use es_info.sctx
    ETYPE_UFUNC,	    // user function, use es_info.ufunc
    ETYPE_AUCMD,	    // autocomand, use es_info.aucmd
    ETYPE_MODELINE,	    // modeline, use es_info.sctx
    ETYPE_EXCEPT,	    // exception, use es_info.exception
    ETYPE_ವಾದಗಳು,		    // command line argument
    ETYPE_ENV,		    // environment variable
    ETYPE_ಆಂತರಿಕ,	    // internal operation
    ETYPE_SPELL,	    // loading spell file
} etype_T;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ದೀರ್ಘ      es_lnum;      // replaces "sourcing_lnum"
    char_u    *es_name;     // replaces "sourcing_name"
    etype_T   es_type;
    ಒಕ್ಕೂಟ {
	sctx_T  *sctx;      // script and modeline info
#ಆದರೆ defined(FEAT_EVAL)
	ufunc_T *ufunc;     // function info
#ಆದರೆ_ಅಂತ್ಯ
	AutoPatCmd_T *aucmd;  // autocommand info
	except_T   *except; // exception info
    } es_info;
#ಆದರೆ defined(FEAT_EVAL)
    sctx_T	es_save_sctx;	    // saved current_sctx when calling function
#ಆದರೆ_ಅಂತ್ಯ
} estack_T;

// Information returned by get_tty_info().
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಪೂರ್ಣಾಂಕ backspace;	// what the Backspace key produces
    ಪೂರ್ಣಾಂಕ enter;		// what the Enter key produces
    ಪೂರ್ಣಾಂಕ ಅಡಚಣೆ;	// interrupt character
    ಪೂರ್ಣಾಂಕ nl_does_cr;	// TRUE when a NL is expanded to CR-NL on output
} ttyinfo_T;

// Status of a job.  Order matters!
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ
{
    ಕೆಲಸ_FAILED,
    ಕೆಲಸ_STARTED,
    ಕೆಲಸ_ENDED,	    // detected job done
    ಕೆಲಸ_ಪೂರ್ಣಗೊಂಡ,   // job done and cleanup done
} jobstatus_T;

/*
 * Structure to hold info about a Job.
 */
ರಚನೆ jobvar_S
{
    ಕೆಲಸ_T	*jv_ಮುಂದಿನ;
    ಕೆಲಸ_T	*jv_prev;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
    pid_t	jv_pid;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
    PROCESS_INFORMATION	jv_proc_info;
    HANDLE		jv_ಕೆಲಸ_object;
#ಆದರೆ_ಅಂತ್ಯ
    jobstatus_T	jv_ಸ್ಥಿತಿ;
    char_u	*jv_tty_ಒಳ;	// controlling tty input, allocated
    char_u	*jv_tty_ಹೊರಹೋಗು;	// controlling tty output, allocated
    char_u	*jv_stoponexit;	// allocated
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
    char_u	*jv_termsig;	// allocated
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
    char_u	*jv_tty_type;	// allocated
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		jv_exitval;
    callback_T	jv_exit_cb;

    buf_T	*jv_ಒಳ_buf;	// buffer from "in-name"

    ಪೂರ್ಣಾಂಕ		jv_refcount;	// reference count
    ಪೂರ್ಣಾಂಕ		jv_copyID;

    channel_T	*jv_channel;	// channel for I/O, reference counted
    ಅಕ್ಷರ	**jv_argv;	// command line used to start the job
};

/*
 * Structures to hold info about a Channel.
 */
ರಚನೆ readq_S
{
    char_u	*rq_ಬಫರ್;
    long_u	rq_buflen;
    readq_T	*rq_ಮುಂದಿನ;
    readq_T	*rq_prev;
};

ರಚನೆ writeq_S
{
    garray_T	wq_ga;
    writeq_T	*wq_ಮುಂದಿನ;
    writeq_T	*wq_prev;
};

ರಚನೆ jsonq_S
{
    typval_T	*jq_ಮೌಲ್ಯ;
    jsonq_T	*jq_ಮುಂದಿನ;
    jsonq_T	*jq_prev;
    ಪೂರ್ಣಾಂಕ		jq_no_callback; // TRUE when no callback was found
};

ರಚನೆ cbq_S
{
    callback_T	cq_callback;
    ಪೂರ್ಣಾಂಕ		cq_seq_nr;
    cbq_T	*cq_ಮುಂದಿನ;
    cbq_T	*cq_prev;
};

// mode for a channel
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ
{
    CH_ಬಹುಲಕ_NL = 0,
    CH_ಬಹುಲಕ_RAW,
    CH_ಬಹುಲಕ_BLOB,
    CH_ಬಹುಲಕ_JSON,
    CH_ಬಹುಲಕ_JS,
    CH_ಬಹುಲಕ_LSP,	// Language Server Protocol (http + json)
    CH_ಬಹುಲಕ_DAP		// Debug Adapter Protocol (like LSP, but does not
			// strictly follow JSON-RPC standard)
} ch_ಬಹುಲಕ_T;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
    JIO_PIPE,	    // default
    JIO_NULL,
    JIO_FILE,
    JIO_ಬಫರ್,
    JIO_ಹೊರಹೋಗು
} ಕೆಲಸ_io_T;

#ವ್ಯಾಖ್ಯಾನಿಸು CH_PART_FD(part)	ch_part[part].ch_fd

// Ordering matters, it is used in for loops: IN is last, only SOCK/OUT/ERR
// are polled.
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
    PART_SOCK = 0,
#ವ್ಯಾಖ್ಯಾನಿಸು CH_SOCK_FD	CH_PART_FD(PART_SOCK)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
    PART_ಹೊರಹೋಗು,
# define CH_ಹೊರಹೋಗು_FD	CH_PART_FD(PART_ಹೊರಹೋಗು)
    PART_ERR,
# define CH_ERR_FD	CH_PART_FD(PART_ERR)
    PART_ಒಳ,
# define CH_ಒಳ_FD	CH_PART_FD(PART_ಒಳ)
#ಆದರೆ_ಅಂತ್ಯ
    PART_ಎಣಿಕೆ,
} ch_part_T;

#ವ್ಯಾಖ್ಯಾನಿಸು INVALID_FD	(-1)

// The per-fd info for a channel.
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    sock_T	ch_fd;	    // socket/stdin/stdout/stderr, -1 if not used

#ಆದರೆ defined(UNIX) && !defined(HAVE_SELECT)
    ಪೂರ್ಣಾಂಕ		ch_poll_idx;	// used by channel_poll_setup()
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_X11
    Xt_ಒಳಹರಿವು_Id	ch_ಒಳಹರಿವು_Handler; // Cookie for input
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_GTK
    gint	ch_ಒಳಹರಿವು_Handler; // Cookie for input
#ಆದರೆ_ಅಂತ್ಯ

    ch_ಬಹುಲಕ_T	ch_ಬಹುಲಕ;
    ಕೆಲಸ_io_T	ch_io;
    ಪೂರ್ಣಾಂಕ		ch_timeout;	// request timeout in msec

    readq_T	ch_ತಲೆ;	// header for circular raw read queue
    jsonq_T	ch_json_ತಲೆ;	// header for circular json read queue
    garray_T	ch_ಖಂಡ_ids;	// list of IDs that channel_read_json_block()
				// is waiting for
    // When ch_wait_len is non-zero use ch_deadline to wait for incomplete
    // message to be complete. The value is the length of the incomplete
    // message when the deadline was set.  If it gets longer (something was
    // received) the deadline is reset.
    size_t	ch_wait_len;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
    DWORD	ch_deadline;
#ಇಲ್ಲದಿದ್ದರೆ
    ರಚನೆ timeval ch_deadline;
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		ch_ಖಂಡ_write;	// for testing: 0 when not used, -1 when write
				// does not block, 1 simulate blocking
    ಪೂರ್ಣಾಂಕ		ch_nonblocking;	// write() is non-blocking
    writeq_T	ch_writeque;	// header for write queue

    cbq_T	ch_cb_ತಲೆ;	// dummy node for per-request callbacks
    callback_T	ch_callback;	// call when a msg is not handled

    bufref_T	ch_bufref;	// buffer to read from or write to
    ಪೂರ್ಣಾಂಕ		ch_nomodifiable; // TRUE when buffer can be 'nomodifiable'
    ಪೂರ್ಣಾಂಕ		ch_nomod_error;	// TRUE when e_modifiable was given
    ಪೂರ್ಣಾಂಕ		ch_buf_append;	// write appended lines instead top-bot
    linenr_T	ch_buf_ತುದಿ;	// next line to send
    linenr_T	ch_buf_bot;	// last line to send
} chanpart_T;

ರಚನೆ channel_S {
    channel_T	*ch_ಮುಂದಿನ;
    channel_T	*ch_prev;

    ಪೂರ್ಣಾಂಕ		ch_id;		// ID of the channel
    ಪೂರ್ಣಾಂಕ		ch_ಕೊನೆಯ_ಸಂದೇಶ_id;	// ID of the last message

    chanpart_T	ch_part[PART_ಎಣಿಕೆ]; // info for socket, out, err and in
    ಪೂರ್ಣಾಂಕ		ch_write_ಪಠ್ಯ_ಬಹುಲಕ; // write buffer lines with CR, not NL

    ಅಕ್ಷರ	*ch_hostname;	// only for socket, allocated
    ಪೂರ್ಣಾಂಕ		ch_ಪೋರ್ಟ್;	// only for socket

    ಪೂರ್ಣಾಂಕ		ch_ಗಮ್ಯ_be_closed; // bitset of readable fds to be closed.
				 // When all readable fds have been closed,
				 // set to (1 << PART_COUNT).
    ಪೂರ್ಣಾಂಕ		ch_ಗಮ್ಯ_be_freed; // When TRUE channel must be freed when it's
				// safe to invoke callbacks.
    ಪೂರ್ಣಾಂಕ		ch_error;	// When TRUE an error was reported.  Avoids
    // giving pages full of error messages when
    // the other side has exited, only mention the
    // first error until the connection works
    // again.
    ಪೂರ್ಣಾಂಕ		ch_listen;	// When TRUE channel is listen socket.

    ಶೂನ್ಯ	(*ch_nb_close_cb)(ಶೂನ್ಯ);
				// callback for Netbeans when channel is
				// closed
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SOCKETSERVER
    bool	ch_socketserver; // If channel is used by socketserver
    ಶೂನ್ಯ	(*ch_ss_close_cb)(channel_T *);
    ಶೂನ್ಯ	(*ch_ss_accept_cb)(channel_T *);
    channel_T	*ch_ss_ಮುಂದಿನ;
    channel_T	*ch_ss_prev;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
    ಪೂರ್ಣಾಂಕ		ch_named_pipe;	// using named pipe instead of pty
#ಆದರೆ_ಅಂತ್ಯ
    callback_T	ch_callback;	// call when any msg is not handled
    callback_T	ch_close_cb;	// call when channel is closed
    ಪೂರ್ಣಾಂಕ		ch_drop_never;
    ಪೂರ್ಣಾಂಕ		ch_keep_open;	// do not close on read error
    ಪೂರ್ಣಾಂಕ		ch_nonblock;

    ಕೆಲಸ_T	*ch_ಕೆಲಸ;	// Job that uses this channel; this does not
				// count as a reference to avoid a circular
				// reference, the job refers to the channel.
    ಪೂರ್ಣಾಂಕ		ch_ಕೆಲಸ_killed;	// TRUE when there was a job and it was killed
				// or we know it died.
    ಪೂರ್ಣಾಂಕ		ch_anonymous_pipe;  // Indicates that anonymous pipes are being
				    // used for communication in the Windows
				    // ConPTY terminal.
    ಪೂರ್ಣಾಂಕ		ch_killing;	    // Indicates that the job associated with
				    // the channel is terminating.  It becomes
				    // TRUE when TerminateJobObject() was
				    // called or the process associated with
				    // the job had exited (only ConPTY).

    ಪೂರ್ಣಾಂಕ		ch_refcount;	// reference count
    ಪೂರ್ಣಾಂಕ		ch_copyID;
};

#ವ್ಯಾಖ್ಯಾನಿಸು JO_ಬಹುಲಕ		    0x0001	// channel mode
#ವ್ಯಾಖ್ಯಾನಿಸು JO_ಒಳ_ಬಹುಲಕ	    0x0002	// stdin mode
#ವ್ಯಾಖ್ಯಾನಿಸು JO_ಹೊರಹೋಗು_ಬಹುಲಕ	    0x0004	// stdout mode
#ವ್ಯಾಖ್ಯಾನಿಸು JO_ERR_ಬಹುಲಕ	    0x0008	// stderr mode
#ವ್ಯಾಖ್ಯಾನಿಸು JO_CALLBACK	    0x0010	// channel callback
#ವ್ಯಾಖ್ಯಾನಿಸು JO_ಹೊರಹೋಗು_CALLBACK	    0x0020	// stdout callback
#ವ್ಯಾಖ್ಯಾನಿಸು JO_ERR_CALLBACK	    0x0040	// stderr callback
#ವ್ಯಾಖ್ಯಾನಿಸು JO_CLOSE_CALLBACK   0x0080	// "close_cb"
#ವ್ಯಾಖ್ಯಾನಿಸು JO_WAITTIME	    0x0100	// only for ch_open()
#ವ್ಯಾಖ್ಯಾನಿಸು JO_TIMEOUT	    0x0200	// all timeouts
#ವ್ಯಾಖ್ಯಾನಿಸು JO_ಹೊರಹೋಗು_TIMEOUT	    0x0400	// stdout timeouts
#ವ್ಯಾಖ್ಯಾನಿಸು JO_ERR_TIMEOUT	    0x0800	// stderr timeouts
#ವ್ಯಾಖ್ಯಾನಿಸು JO_PART		    0x1000	// "part"
#ವ್ಯಾಖ್ಯಾನಿಸು JO_ID		    0x2000	// "id"
#ವ್ಯಾಖ್ಯಾನಿಸು JO_STOPONEXIT	    0x4000	// "stoponexit"
#ವ್ಯಾಖ್ಯಾನಿಸು JO_EXIT_CB	    0x8000	// "exit_cb"
#ವ್ಯಾಖ್ಯಾನಿಸು JO_ಹೊರಹೋಗು_IO	    0x10000	// "out_io"
#ವ್ಯಾಖ್ಯಾನಿಸು JO_ERR_IO	    0x20000	// "err_io" (JO_OUT_IO << 1)
#ವ್ಯಾಖ್ಯಾನಿಸು JO_ಒಳ_IO	    0x40000	// "in_io" (JO_OUT_IO << 2)
#ವ್ಯಾಖ್ಯಾನಿಸು JO_ಹೊರಹೋಗು_NAME	    0x80000	// "out_name"
#ವ್ಯಾಖ್ಯಾನಿಸು JO_ERR_NAME	    0x100000	// "err_name" (JO_OUT_NAME << 1)
#ವ್ಯಾಖ್ಯಾನಿಸು JO_ಒಳ_NAME	    0x200000	// "in_name" (JO_OUT_NAME << 2)
#ವ್ಯಾಖ್ಯಾನಿಸು JO_ಒಳ_ತುದಿ	    0x400000	// "in_top"
#ವ್ಯಾಖ್ಯಾನಿಸು JO_ಒಳ_BOT	    0x800000	// "in_bot"
#ವ್ಯಾಖ್ಯಾನಿಸು JO_ಹೊರಹೋಗು_BUF	    0x1000000	// "out_buf"
#ವ್ಯಾಖ್ಯಾನಿಸು JO_ERR_BUF	    0x2000000	// "err_buf" (JO_OUT_BUF << 1)
#ವ್ಯಾಖ್ಯಾನಿಸು JO_ಒಳ_BUF	    0x4000000	// "in_buf" (JO_OUT_BUF << 2)
#ವ್ಯಾಖ್ಯಾನಿಸು JO_CHANNEL	    0x8000000	// "channel"
#ವ್ಯಾಖ್ಯಾನಿಸು JO_ಖಂಡ_WRITE	    0x10000000	// "block_write"
#ವ್ಯಾಖ್ಯಾನಿಸು JO_ಹೊರಹೋಗು_MODIFIABLE   0x20000000	// "out_modifiable"
#ವ್ಯಾಖ್ಯಾನಿಸು JO_ERR_MODIFIABLE   0x40000000	// "err_modifiable" (JO_OUT_ << 1)
#ವ್ಯಾಖ್ಯಾನಿಸು JO_ALL		    0x7fffffff

#ವ್ಯಾಖ್ಯಾನಿಸು JO2_ಹೊರಹೋಗು_ಸಂದೇಶ	    0x0001	// "out_msg"
#ವ್ಯಾಖ್ಯಾನಿಸು JO2_ERR_ಸಂದೇಶ	    0x0002	// "err_msg" (JO_OUT_ << 1)
#ವ್ಯಾಖ್ಯಾನಿಸು JO2_ಪದ_NAME	    0x0004	// "term_name"
#ವ್ಯಾಖ್ಯಾನಿಸು JO2_ಪದ_ಮುಕ್ತಾಯ	    0x0008	// "term_finish"
#ವ್ಯಾಖ್ಯಾನಿಸು JO2_ENV		    0x0010	// "env"
#ವ್ಯಾಖ್ಯಾನಿಸು JO2_CWD		    0x0020	// "cwd"
#ವ್ಯಾಖ್ಯಾನಿಸು JO2_ಪದ_ಅಡ್ಡಸಾಲುಗಳು	    0x0040	// "term_rows"
#ವ್ಯಾಖ್ಯಾನಿಸು JO2_ಪದ_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ	    0x0080	// "term_cols"
#ವ್ಯಾಖ್ಯಾನಿಸು JO2_VERTICAL	    0x0100	// "vertical"
#ವ್ಯಾಖ್ಯಾನಿಸು JO2_CURWIN	    0x0200	// "curwin"
#ವ್ಯಾಖ್ಯಾನಿಸು JO2_HIDDEN	    0x0400	// "hidden"
#ವ್ಯಾಖ್ಯಾನಿಸು JO2_ಪದ_OPENCMD    0x0800	// "term_opencmd"
#ವ್ಯಾಖ್ಯಾನಿಸು JO2_EOF_CHARS	    0x1000	// "eof_chars"
#ವ್ಯಾಖ್ಯಾನಿಸು JO2_NORESTORE	    0x2000	// "norestore"
#ವ್ಯಾಖ್ಯಾನಿಸು JO2_ಪದ_KILL	    0x4000	// "term_kill"
#ವ್ಯಾಖ್ಯಾನಿಸು JO2_ANSI_COLORS	    0x8000	// "ansi_colors"
#ವ್ಯಾಖ್ಯಾನಿಸು JO2_TTY_TYPE	    0x10000	// "tty_type"
#ವ್ಯಾಖ್ಯಾನಿಸು JO2_BUFNR	    0x20000	// "bufnr"
#ವ್ಯಾಖ್ಯಾನಿಸು JO2_ಪದ_API	    0x40000	// "term_api"
#ವ್ಯಾಖ್ಯಾನಿಸು JO2_ಪದ_HIGHLIGHT  0x80000	// "highlight"

#ವ್ಯಾಖ್ಯಾನಿಸು JO_ಬಹುಲಕ_ALL	(JO_ಬಹುಲಕ + JO_ಒಳ_ಬಹುಲಕ + JO_ಹೊರಹೋಗು_ಬಹುಲಕ + JO_ERR_ಬಹುಲಕ)
#ವ್ಯಾಖ್ಯಾನಿಸು JO_CB_ALL \
    (JO_CALLBACK + JO_ಹೊರಹೋಗು_CALLBACK + JO_ERR_CALLBACK + JO_CLOSE_CALLBACK)
#ವ್ಯಾಖ್ಯಾನಿಸು JO_TIMEOUT_ALL	(JO_TIMEOUT + JO_ಹೊರಹೋಗು_TIMEOUT + JO_ERR_TIMEOUT)

/*
 * Options for job and channel commands.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    ಪೂರ್ಣಾಂಕ		jo_ಗಣ;		// JO_ bits for values that were set
    ಪೂರ್ಣಾಂಕ		jo_set2;	// JO2_ bits for values that were set

    ch_ಬಹುಲಕ_T	jo_ಬಹುಲಕ;
    ch_ಬಹುಲಕ_T	jo_ಒಳ_ಬಹುಲಕ;
    ch_ಬಹುಲಕ_T	jo_ಹೊರಹೋಗು_ಬಹುಲಕ;
    ch_ಬಹುಲಕ_T	jo_err_ಬಹುಲಕ;
    ಪೂರ್ಣಾಂಕ		jo_noblock;

    ಕೆಲಸ_io_T	jo_io[4];	// PART_OUT, PART_ERR, PART_IN
    char_u	jo_io_name_buf[4][NUMBUFLEN];
    char_u	*jo_io_name[4];	// not allocated!
    ಪೂರ್ಣಾಂಕ		jo_io_buf[4];
    ಪೂರ್ಣಾಂಕ		jo_pty;
    ಪೂರ್ಣಾಂಕ		jo_modifiable[4];
    ಪೂರ್ಣಾಂಕ		jo_ಸಂದೇಶ[4];
    channel_T	*jo_channel;

    linenr_T	jo_ಒಳ_ತುದಿ;
    linenr_T	jo_ಒಳ_bot;

    callback_T	jo_callback;
    callback_T	jo_ಹೊರಹೋಗು_cb;
    callback_T	jo_err_cb;
    callback_T	jo_close_cb;
    callback_T	jo_exit_cb;
    ಪೂರ್ಣಾಂಕ		jo_drop_never;
    ಪೂರ್ಣಾಂಕ		jo_waittime;
    ಪೂರ್ಣಾಂಕ		jo_timeout;
    ಪೂರ್ಣಾಂಕ		jo_ಹೊರಹೋಗು_timeout;
    ಪೂರ್ಣಾಂಕ		jo_err_timeout;
    ಪೂರ್ಣಾಂಕ		jo_ಖಂಡ_write;	// for testing only
    ಪೂರ್ಣಾಂಕ		jo_part;
    ಪೂರ್ಣಾಂಕ		jo_id;
    char_u	jo_stoponexit_buf[NUMBUFLEN];
    char_u	*jo_stoponexit;
    dict_T	*jo_env;	// environment variables
    char_u	jo_cwd_buf[NUMBUFLEN];
    char_u	*jo_cwd;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
    // when non-zero run the job in a terminal window of this size
    ಪೂರ್ಣಾಂಕ		jo_ಪದ_ಅಡ್ಡಸಾಲುಗಳು;
    ಪೂರ್ಣಾಂಕ		jo_ಪದ_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;
    ಪೂರ್ಣಾಂಕ		jo_vertical;
    ಪೂರ್ಣಾಂಕ		jo_curwin;
    buf_T	*jo_bufnr_buf;
    ಪೂರ್ಣಾಂಕ		jo_hidden;
    ಪೂರ್ಣಾಂಕ		jo_ಪದ_norestore;
    char_u	jo_ಪದ_name_buf[NUMBUFLEN];
    char_u	*jo_ಪದ_name;
    char_u	jo_ಪದ_opencmd_buf[NUMBUFLEN];
    char_u	*jo_ಪದ_opencmd;
    ಪೂರ್ಣಾಂಕ		jo_ಪದ_ಮುಕ್ತಾಯ;
    char_u	jo_eof_chars_buf[NUMBUFLEN];
    char_u	*jo_eof_chars;
    char_u	jo_ಪದ_kill_buf[NUMBUFLEN];
    char_u	*jo_ಪದ_kill;
# ಆದರೆ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    long_u	jo_ansi_colors[16];
# endif
    char_u	jo_ಪದ_highlight_buf[NUMBUFLEN];
    char_u	*jo_ಪದ_highlight;
    ಪೂರ್ಣಾಂಕ		jo_tty_type;	    // first character of "tty_type"
    char_u	jo_ಪದ_api_buf[NUMBUFLEN];
    char_u	*jo_ಪದ_api;
#ಆದರೆ_ಅಂತ್ಯ
} jobopt_T;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
/*
 * Structure used for listeners added with listener_add().
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ listener_S listener_T;
ರಚನೆ listener_S
{
    listener_T	*lr_ಮುಂದಿನ;
    ಪೂರ್ಣಾಂಕ		lr_id;
    bool	lr_ಪಠ್ಯ;	// include the resulting text in each change
    callback_T	lr_callback;
};

// Structure used for listeners added with redraw_listener_add().
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ redraw_listener_S redraw_listener_T;
ರಚನೆ redraw_listener_S
{
    redraw_listener_T	*rl_ಮುಂದಿನ;
    ಪೂರ್ಣಾಂಕ			rl_id;
    ರಚನೆ
    {
	callback_T	on_ಆರಂಭ;
	callback_T	on_ಕೊನೆ;
    }			rl_callbacks;
};
#ಆದರೆ_ಅಂತ್ಯ

/*
 * structure used for explicit stack while garbage collecting hash tables
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ ht_ರಾಶಿ_S
{
    hashtab_T		*ht;
    ರಚನೆ ht_ರಾಶಿ_S	*prev;
} ht_ರಾಶಿ_T;

/*
 * structure used for explicit stack while garbage collecting lists
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ list_ರಾಶಿ_S
{
    list_T		*list;
    ರಚನೆ list_ರಾಶಿ_S	*prev;
} list_ರಾಶಿ_T;

/*
 * structure used for explicit stack while garbage collecting tuples
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ tuple_ರಾಶಿ_S
{
    tuple_T			*tuple;
    ರಚನೆ tuple_ರಾಶಿ_S	*prev;
} tuple_ರಾಶಿ_T;

/*
 * Structure used for iterating over dictionary items.
 * Initialize with dict_iterate_start().
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    long_u	dit_todo;
    hashitem_T	*dit_hi;
} dict_iterator_T;

// values for b_syn_spell: what to do with toplevel text
#ವ್ಯಾಖ್ಯಾನಿಸು SYNSPL_DEFAULT	0	// spell check if @Spell not defined
#ವ್ಯಾಖ್ಯಾನಿಸು SYNSPL_ತುದಿ	1	// spell check toplevel text
#ವ್ಯಾಖ್ಯಾನಿಸು SYNSPL_NOTOP	2	// don't spell check toplevel text

// values for b_syn_foldlevel: how to compute foldlevel on a line
#ವ್ಯಾಖ್ಯಾನಿಸು SYNFLD_ಆರಂಭ	0	// use level of item at start of line
#ವ್ಯಾಖ್ಯಾನಿಸು SYNFLD_MINIMUM	1	// use lowest local minimum level on line

// avoid #ifdefs for when b_spell is not available
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SPELL
# define B_SPELL(buf)  ((buf)->b_spell)
#ಇಲ್ಲದಿದ್ದರೆ
# define B_SPELL(buf)  (0)
#ಆದರೆ_ಅಂತ್ಯ

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ qf_info_S qf_info_T;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROFILE
/*
 * Used for :syntime: timing of executing a syntax pattern.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    proftime_T	ಒಟ್ಟು;		// total time used
    proftime_T	slowest;	// time of slowest call
    ದೀರ್ಘ	ಎಣಿಕೆ_2;		// nr of times used
    ದೀರ್ಘ	ಹೊಂದಿಕೆ;		// nr of times matched
} syn_ಸಮಯ_T;
#ಆದರೆ_ಅಂತ್ಯ

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ timer_S timer_T;
ರಚನೆ timer_S
{
    ದೀರ್ಘ	tr_id;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TIMERS
    timer_T	*tr_ಮುಂದಿನ;
    timer_T	*tr_prev;
    proftime_T	tr_due;		    // when the callback is to be invoked
    ಅಕ್ಷರ	tr_firing;	    // when TRUE callback is being called
    ಅಕ್ಷರ	tr_paused;	    // when TRUE callback is not invoked
    ಅಕ್ಷರ	tr_keep;	    // when TRUE keep timer after it fired
    ಪೂರ್ಣಾಂಕ		tr_repeat;	    // number of times to repeat, -1 forever
    ದೀರ್ಘ	tr_interval;	    // msec
    callback_T	tr_callback;
    ಪೂರ್ಣಾಂಕ		tr_emsg_ಎಣಿಕೆ;
#ಆದರೆ_ಅಂತ್ಯ
};

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CRYPT
/*
 * Structure to hold the type of encryption and the state of encryption or
 * decryption.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಪೂರ್ಣಾಂಕ	    ವಿಧಾನ_nr;
    ಶೂನ್ಯ    *ವಿಧಾನ_state;  // method-specific state information
} cryptstate_T;

// values for method_nr
# define CRYPT_M_ZIP	0
# define CRYPT_M_BF	1
# define CRYPT_M_BF2	2
# define CRYPT_M_SOD    3
# define CRYPT_M_SOD2   4
# define CRYPT_M_ಎಣಿಕೆ	5 // number of crypt methods

// Currently all crypt methods work inplace.  If one is added that isn't then
// define this.
# define CRYPT_NOT_INPLACE 1

// Struct for passing arguments down to the crypt_init functions
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    char_u	*cat_salt;
    ಪೂರ್ಣಾಂಕ		cat_salt_len;
    char_u	*cat_ಬೀಜ;
    ಪೂರ್ಣಾಂಕ		cat_ಬೀಜ_len;
    char_u	*cat_ಕೂಡಿಸು;
    ಪೂರ್ಣಾಂಕ		cat_ಕೂಡಿಸು_len;
    ಪೂರ್ಣಾಂಕ		cat_init_from_file;
} crypt_arg_T;

#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
    POPPOS_BOTLEFT,
    POPPOS_TOPLEFT,
    POPPOS_BOTRIGHT,
    POPPOS_TOPRIGHT,
    POPPOS_CENTER,
    POPPOS_BOTTOM,	// bottom of popup just above the command line
    POPPOS_NONE
} poppos_T;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
    POPCLOSE_NONE,
    POPCLOSE_BUTTON,
    POPCLOSE_CLICK
} popclose_T;

# define POPUPWIN_DEFAULT_ZINDEX	 50
# define POPUPMENU_ZINDEX		100
# define POPUPWIN_DIALOG_ZINDEX		200
# define POPUPWIN_NOTIFICATION_ZINDEX   300
#ಆದರೆ_ಅಂತ್ಯ

/*
 * These are items normally related to a buffer.  But when using ":ownsyntax"
 * a window may have its own instance.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
    hashtab_T	b_keywtab;		// syntax keywords hash table
    hashtab_T	b_keywtab_ic;		// idem, ignore case
    ಪೂರ್ಣಾಂಕ		b_syn_error;		// TRUE when error occurred in HL
# ifdef FEAT_RELTIME
    ಪೂರ್ಣಾಂಕ		b_syn_slow;		// TRUE when 'redrawtime' reached
# endif
    ಪೂರ್ಣಾಂಕ		b_syn_ic;		// ignore case for :syn cmds
    ಪೂರ್ಣಾಂಕ		b_syn_foldlevel;	// how to compute foldlevel on a line
    ಪೂರ್ಣಾಂಕ		b_syn_spell;		// SYNSPL_ values
    garray_T	b_syn_patterns;		// table for syntax patterns
    garray_T	b_syn_clusters;		// table for syntax clusters
    ಪೂರ್ಣಾಂಕ		b_spell_cluster_id;	// @Spell cluster ID or 0
    ಪೂರ್ಣಾಂಕ		b_nospell_cluster_id;	// @NoSpell cluster ID or 0
    ಪೂರ್ಣಾಂಕ		b_syn_containedin;	// TRUE when there is an item with a
					// "containedin" argument
    ಪೂರ್ಣಾಂಕ		b_syn_sync_flags;	// flags about how to sync
    ಗಿಡ್ಡ	b_syn_sync_id;		// group to sync on
    ದೀರ್ಘ	b_syn_sync_minlines;	// minimal sync lines offset
    ದೀರ್ಘ	b_syn_sync_maxlines;	// maximal sync lines offset
    ದೀರ್ಘ	b_syn_sync_linebreaks;	// offset for multi-line pattern
    char_u	*b_syn_linecont_pat;	// line continuation pattern
    regprog_T	*b_syn_linecont_prog;	// line continuation program
# ifdef FEAT_PROFILE
    syn_ಸಮಯ_T  b_syn_linecont_ಸಮಯ;
# endif
    ಪೂರ್ಣಾಂಕ		b_syn_linecont_ic;	// ignore-case flag for above
    ಪೂರ್ಣಾಂಕ		b_syn_topgrp;		// for ":syntax include"
# ifdef FEAT_CONCEAL
    ಪೂರ್ಣಾಂಕ		b_syn_conceal;		// auto-conceal for :syn cmds
# endif
# ifdef FEAT_FOLDING
    ಪೂರ್ಣಾಂಕ		b_syn_folditems;	// number of patterns with the HL_FOLD
					// flag set
# endif
    /*
     * b_sst_array[] contains the state stack for a number of lines, for the
     * start of that line (col == 0).  This avoids having to recompute the
     * syntax state too often.
     * b_sst_array[] is allocated to hold the state for all displayed lines,
     * and states for 1 out of about 20 other lines.
     * b_sst_array	pointer to an array of synstate_T
     * b_sst_len	number of entries in b_sst_array[]
     * b_sst_first	pointer to first used entry in b_sst_array[] or NULL
     * b_sst_search	cached entry near the last accessed line, used as a
     *			start point for forward lookups, or NULL
     * b_sst_firstfree	pointer to first free entry in b_sst_array[] or NULL
     * b_sst_freecount	number of free entries in b_sst_array[]
     * b_sst_check_lnum	entries after this lnum need to be checked for
     *			validity (MAXLNUM means no check needed)
     */
    synstate_T	*b_sst_array;
    ಪೂರ್ಣಾಂಕ		b_sst_len;
    synstate_T	*b_sst_ಮೊದಲ;
    synstate_T	*b_sst_ಹುಡುಕು;
    synstate_T	*b_sst_firstfree;
    ಪೂರ್ಣಾಂಕ		b_sst_freecount;
    linenr_T	b_sst_check_lnum;
    short_u	b_sst_lasttick;	// last display tick

    // Cache for in_id_list(); see idl_cache_T in syntax.c.
    ಶೂನ್ಯ	*b_idlist_cache;
#ಆದರೆ_ಅಂತ್ಯ // FEAT_SYN_HL

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SPELL
    // for spell checking
    garray_T	b_langp;	    // list of pointers to slang_T, see spell.c
    char_u	b_spell_ismw[256];  // flags: is midword char
    char_u	*b_spell_ismw_mb;   // multi-byte midword chars
    char_u	*b_p_spc;	    // 'spellcapcheck'
    regprog_T	*b_cap_prog;	    // program for 'spellcapcheck'
    char_u	*b_p_spf;	    // 'spellfile'
    char_u	*b_p_spl;	    // 'spelllang'
    char_u	*b_p_spo;	    // 'spelloptions'
    ಪೂರ್ಣಾಂಕ		b_cjk;		    // all CJK letters as OK
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ !defined(FEAT_SYN_HL) && !defined(FEAT_SPELL)
    ಪೂರ್ಣಾಂಕ		dummy;
#ಆದರೆ_ಅಂತ್ಯ
    char_u	b_syn_chartab[32];  // syntax iskeyword option
    char_u	*b_syn_isk;	    // iskeyword option
} synblock_T;


/*
 * buffer: structure that holds information about one file
 *
 * Several windows can share a single Buffer
 * A buffer is unallocated if there is no memfile for it.
 * A buffer is new if the associated file has never been loaded yet.
 */

ರಚನೆ file_ಬಫರ್
{
    memline_T	b_ml;		// associated memline (also contains line
				// count)

    buf_T	*b_ಮುಂದಿನ;	// links in list of buffers
    buf_T	*b_prev;

    ಪೂರ್ಣಾಂಕ		b_nwindows;	// nr of windows open on this buffer

    ಪೂರ್ಣಾಂಕ		b_flags;	// various BF_ flags
    ಪೂರ್ಣಾಂಕ		b_locked;	// Buffer is being closed or referenced, don't
				// let autocommands wipe it out.
    ಪೂರ್ಣಾಂಕ		b_locked_ವಿಭಜಿಸು;	// Buffer is being closed, don't allow opening
				// it in more windows.

    /*
     * b_ffname has the full path of the file (NULL for no name).
     * b_sfname is the name as the user typed it (or NULL).
     * b_fname is the same as b_sfname, unless ":cd" has been done,
     *		then it is the same as b_ffname (NULL for no name).
     */
    char_u	*b_ffname;	// full path file name, allocated
    char_u	*b_sfname;	// short file name, allocated, may be equal to
				// b_ffname
    char_u	*b_fname;	// current file name, points to b_ffname or
				// b_sfname

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
    bool	b_dev_valid;	// true when b_dev has a valid number
    dev_t	b_dev;		// device number
    ino_t	b_ino;		// inode number
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ VMS
    ಅಕ್ಷರ	 b_fab_rfm;	// Record format
    ಅಕ್ಷರ	 b_fab_rat;	// Record attribute
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ b_fab_mrs;	// Max record size
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		b_fnum;		// buffer number for this file.
    char_u	b_ಕೀಲಿ[VIM_SIZEOF_INT * 2 + 1];
				// key used for buf_hashtab, holds b_fnum as
				// hex string

    ಪೂರ್ಣಾಂಕ		b_ಬದಲಾಯಿತು;	// 'modified': Set to TRUE if something in the
				// file has been changed and not written out.
    dictitem16_T b_ct_di;	// holds the b:changedtick value in
				// b_ct_di.di_tv.vval.v_number;
				// incremented for each change, also for undo
#ವ್ಯಾಖ್ಯಾನಿಸು CHANGEDTICK(buf) ((buf)->b_ct_di.di_tv.vval.v_ಸಂಖ್ಯೆ)

    varnumber_T	b_ಕೊನೆಯ_changedtick;	// b:changedtick when TextChanged was
					// last triggered.
    varnumber_T	b_ಕೊನೆಯ_changedtick_pum; // b:changedtick for TextChangedP
    varnumber_T	b_ಕೊನೆಯ_changedtick_i;   // b:changedtick for TextChangedI

    bool	b_saving;	// Set to true if we are in the middle of
				// saving the buffer.

    /*
     * Changes to a buffer require updating of the display.  To minimize the
     * work, remember changes made and update everything at once.
     */
    bool	b_mod_ಗಣ;	// true when there are changes since the last
				// time the display was updated
    linenr_T	b_mod_ತುದಿ;	// topmost lnum that was changed
    linenr_T	b_mod_bot;	// lnum below last changed line, AFTER the
				// change
    ದೀರ್ಘ	b_mod_xlines;	// number of extra buffer lines inserted;
				// negative when lines were deleted

    wininfo_T	*b_wininfo;	// list of last used info for each window

    ದೀರ್ಘ	b_mtime;	// last change time of original file
    ದೀರ್ಘ	b_mtime_ns;	// nanoseconds of last change time
    ದೀರ್ಘ	b_mtime_read;	// last change time when reading
    ದೀರ್ಘ	b_mtime_read_ns;  // nanoseconds of last read time
    off_T	b_orig_ಗಾತ್ರ;	// size of original file in bytes
    ಪೂರ್ಣಾಂಕ		b_orig_ಬಹುಲಕ;	// mode of original file
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VIMINFO
    ಸಮಯ_T	b_ಕೊನೆಯ_ಬಳಸಿದ;	// time when the buffer was last used; used
				// for viminfo
#ಆದರೆ_ಅಂತ್ಯ

    pos_T	b_namedm[NMARKS]; // current named marks (mark.c)

    // These variables are set when VIsual_active becomes FALSE
    visualinfo_T b_visual;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಪೂರ್ಣಾಂಕ		b_visual_ಬಹುಲಕ_eval;  // b_visual.vi_mode for visualmode()
#ಆದರೆ_ಅಂತ್ಯ

    pos_T	b_ಕೊನೆಯ_cursor;	// cursor position when last unloading this
				//ಬಫರ್
    pos_T	b_ಕೊನೆಯ_ಸೇರಿಸು;	// where Insert mode was left
    pos_T	b_ಕೊನೆಯ_change;	// position of last change: '. mark

    /*
     * the changelist contains old change positions
     */
    pos_T	b_changelist[JUMPLISTSIZE];
    ಪೂರ್ಣಾಂಕ		b_changelistlen;	// number of active entries
    bool	b_new_change;		// set in u_savecommon()

    /*
     * Character table, only used in charset.c for 'iskeyword'
     * 32 bytes of 8 bits: 1 bit per character 0-255.
     */
    char_u	b_chartab[32];

    // Table used for mappings local to a buffer.
    mapblock_T	*(b_maphash[256]);

    // First abbreviation local to a buffer.
    mapblock_T	*b_ಮೊದಲ_abbr;

    // User commands local to the buffer.
    garray_T	b_ucmds;
    // start and end of an operator, also used for '[ and ']
    pos_T	b_op_ಆರಂಭ;
    pos_T	b_op_ಆರಂಭ_orig;  // used for Insstart_orig
    pos_T	b_op_ಕೊನೆ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VIMINFO
    bool	b_marks_read;	// Have we read viminfo marks yet?
#ಆದರೆ_ಅಂತ್ಯ

    bool	b_modified_was_ಗಣ;	// did ":set modified"
    bool	b_did_filetype;		// FileType event found
    bool	b_keep_filetype;	// value for did_filetype when starting
					// to execute autocommands

    // Set by the apply_autocmds_group function if the given event is equal to
    // EVENT_FILETYPE. Used by the readfile function in order to determine if
    // EVENT_BUFREADPOST triggered the EVENT_FILETYPE.
    //
    // Relying on this value requires one to reset it prior calling
    // apply_autocmds_group().
    bool	b_au_did_filetype;

    /*
     * The following only used in undo.c.
     */
    u_ಶೀರ್ಷಿಕೆ_T	*b_u_oldhead;	// pointer to oldest header
    u_ಶೀರ್ಷಿಕೆ_T	*b_u_newhead;	// pointer to newest header; may not be valid
				// if b_u_curhead is not NULL
    u_ಶೀರ್ಷಿಕೆ_T	*b_u_curhead;	// pointer to current header
    ಪೂರ್ಣಾಂಕ		b_u_numhead;	// current number of headers
    bool	b_u_synced;	// entry lists are synced
    ದೀರ್ಘ	b_u_seq_ಕೊನೆಯ;	// last used undo sequence number
    ದೀರ್ಘ	b_u_save_nr_ಕೊನೆಯ; // counter for last file write
    ದೀರ್ಘ	b_u_seq_cur;	// uh_seq of header below which we are now
    ಸಮಯ_T	b_u_ಸಮಯ_cur;	// uh_time of header below which we are now
    ದೀರ್ಘ	b_u_save_nr_cur; // file write nr after which we are now

    /*
     * variables for "U" command in undo.c
     */
    undoline_T	b_u_ಸಾಲು_ptr;	// saved line for "U" command
    linenr_T	b_u_ಸಾಲು_lnum;	// line number of line in u_line
    colnr_T	b_u_ಸಾಲು_colnr;	// optional column number

    bool	b_scanned;	// ^N/^P have scanned this buffer

    // flags for use of ":lmap" and IM control
    ದೀರ್ಘ	b_p_iminsert;	// input mode for insert
    ದೀರ್ಘ	b_p_imsearch;	// input mode for search
#ವ್ಯಾಖ್ಯಾನಿಸು B_IMODE_USE_ಸೇರಿಸು (-1)	//	Use b_p_iminsert value for search
#ವ್ಯಾಖ್ಯಾನಿಸು B_IMODE_NONE 0		//	Input via none
#ವ್ಯಾಖ್ಯಾನಿಸು B_IMODE_LMAP 1		//	Input via langmap
#ವ್ಯಾಖ್ಯಾನಿಸು B_IMODE_IM 2		//	Input via input method
#ವ್ಯಾಖ್ಯಾನಿಸು B_IMODE_ಕೊನೆಯ 2

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_KEYMAP
    ಗಿಡ್ಡ	b_kmap_state;	// using "lmap" mappings
# define KEYMAP_INIT	1	// 'keymap' was set, call keymap_init()
# define KEYMAP_LOADED	2	// 'keymap' mappings have been loaded
    garray_T	b_kmap_ga;	// the keymap table
#ಆದರೆ_ಅಂತ್ಯ

    /*
     * Options local to a buffer.
     * They are here because their value depends on the type of file
     * or contents of the file being edited.
     */
    bool	b_p_initialized;	// set when options initialized

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    sctx_T	b_p_script_ctx[BV_ಎಣಿಕೆ]; // SCTXs for buffer-local options
#ಆದರೆ_ಅಂತ್ಯ

    ಪೂರ್ಣಾಂಕ		b_p_ac;		// 'autocomplete'
    ಪೂರ್ಣಾಂಕ		b_p_ai;		// 'autoindent'
    ಪೂರ್ಣಾಂಕ		b_p_ai_nopaste;	// b_p_ai saved for paste mode
    char_u	*b_p_bkc;	// 'backupcopy'
    ಚಿಹ್ನೆರಹಿತ	b_bkc_flags;    // flags for 'backupcopy'
    ಪೂರ್ಣಾಂಕ		b_p_ci;		// 'copyindent'
    ಪೂರ್ಣಾಂಕ		b_p_bin;	// 'binary'
    ಪೂರ್ಣಾಂಕ		b_p_bomb;	// 'bomb'
    char_u	*b_p_bh;	// 'bufhidden'
    char_u	*b_p_bt;	// 'buftype'
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
# define BUF_HAS_QF_ನಮೂದು 1
# define BUF_HAS_LL_ನಮೂದು 2
    ಪೂರ್ಣಾಂಕ		b_has_qf_ನಮೂದು;
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		b_p_bl;		// 'buflisted'
    ಪೂರ್ಣಾಂಕ		b_p_cin;	// 'cindent'
    char_u	*b_p_cino;	// 'cinoptions'
    char_u	*b_p_cink;	// 'cinkeys'
    char_u	*b_p_cinsd;	// 'cinscopedecls'
    char_u	*b_p_cinw;	// 'cinwords'
    char_u	*b_p_com;	// 'comments'
    char_u	*b_p_cms;	// 'commentstring'
    char_u	*b_p_cot;	// 'completeopt' local value
    ಚಿಹ್ನೆರಹಿತ	b_cot_flags;	// flags for 'completeopt'
    char_u	*b_p_cpt;	// 'complete'
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ BACKSLASH_ಒಳ_FILENAME
    char_u	*b_p_csl;	// 'completeslash'
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_COMPL_FUNC
    long_u	b_p_cpt_flags;	// flags for 'complete'
    callback_T	*b_p_cpt_cb;	// F{func} in 'complete' callback
    ಪೂರ್ಣಾಂಕ		b_p_cpt_ಎಣಿಕೆ;	// Count of values in 'complete'
    char_u	*b_p_cfu;	// 'completefunc'
    callback_T	b_cfu_cb;	// 'completefunc' callback
    char_u	*b_p_ofu;	// 'omnifunc'
    callback_T	b_ofu_cb;	// 'omnifunc' callback
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    char_u	*b_p_tfu;	// 'tagfunc' option value
    callback_T	b_tfu_cb;	// 'tagfunc' callback
    char_u	*b_p_ffu;	// 'findfunc' option value
    callback_T	b_ffu_cb;	// 'findfunc' callback
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		b_p_eof;	// 'endoffile'
    ಪೂರ್ಣಾಂಕ		b_p_eol;	// 'endofline'
    ಪೂರ್ಣಾಂಕ		b_p_fixeol;	// 'fixendofline'
    ಪೂರ್ಣಾಂಕ		b_p_et;		// 'expandtab'
    ಪೂರ್ಣಾಂಕ		b_p_et_nobin;	// b_p_et saved for binary mode
    ಪೂರ್ಣಾಂಕ		b_p_et_nopaste; // b_p_et saved for paste mode
    char_u	*b_p_fenc;	// 'fileencoding'
    char_u	*b_p_ff;	// 'fileformat'
    char_u	*b_p_ft;	// 'filetype'
    char_u	*b_p_fo;	// 'formatoptions'
    char_u	*b_p_flp;	// 'formatlistpat'
    ಪೂರ್ಣಾಂಕ		b_p_inf;	// 'infercase'
    char_u	*b_p_isk;	// 'iskeyword'
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕಂಡುಹಿಡಿ_ID
    char_u	*b_p_def;	// 'define' local value
    char_u	*b_p_inc;	// 'include'
# ifdef FEAT_EVAL
    char_u	*b_p_inex;	// 'includeexpr'
    long_u	b_p_inex_flags;	// flags for 'includeexpr'
# endif
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_EVAL)
    char_u	*b_p_inde;	// 'indentexpr'
    long_u	b_p_inde_flags;	// flags for 'indentexpr'
    char_u	*b_p_indk;	// 'indentkeys'
#ಆದರೆ_ಅಂತ್ಯ
    char_u	*b_p_fp;	// 'formatprg'
#ಆದರೆ defined(FEAT_EVAL)
    char_u	*b_p_fex;	// 'formatexpr'
    long_u	b_p_fex_flags;	// flags for 'formatexpr'
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_FSYNC
    ಪೂರ್ಣಾಂಕ		b_p_fs;		// 'fsync'
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CRYPT
    char_u	*b_p_ಕೀಲಿ;	// 'key'
#ಆದರೆ_ಅಂತ್ಯ
    char_u	*b_p_kp;	// 'keywordprg'
    ಪೂರ್ಣಾಂಕ		b_p_lisp;	// 'lisp'
    char_u	*b_p_lop;	// 'lispoptions'
    char_u	*b_p_menc;	// 'makeencoding'
    char_u	*b_p_mps;	// 'matchpairs'
    ಪೂರ್ಣಾಂಕ		b_p_ml;		// 'modeline'
    ಪೂರ್ಣಾಂಕ		b_p_ml_nobin;	// b_p_ml saved for binary mode
    ಪೂರ್ಣಾಂಕ		b_p_ma;		// 'modifiable'
    char_u	*b_p_nf;	// 'nrformats'
    ಪೂರ್ಣಾಂಕ		b_p_pi;		// 'preserveindent'
    char_u	*b_p_qe;	// 'quoteescape'
    ಪೂರ್ಣಾಂಕ		b_p_ro;		// 'readonly'
    ದೀರ್ಘ	b_p_sw;		// 'shiftwidth'
    ಪೂರ್ಣಾಂಕ		b_p_sn;		// 'shortname'
    ಪೂರ್ಣಾಂಕ		b_p_si;		// 'smartindent'
    ದೀರ್ಘ	b_p_sts;	// 'softtabstop'
    ದೀರ್ಘ	b_p_sts_nopaste; // b_p_sts saved for paste mode
    char_u	*b_p_sua;	// 'suffixesadd'
    ಪೂರ್ಣಾಂಕ		b_p_swf;	// 'swapfile'
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
    ದೀರ್ಘ	b_p_smc;	// 'synmaxcol'
    char_u	*b_p_syn;	// 'syntax'
#ಆದರೆ_ಅಂತ್ಯ
    ದೀರ್ಘ	b_p_ts;		// 'tabstop'
    ಪೂರ್ಣಾಂಕ		b_p_tx;		// 'textmode'
    ದೀರ್ಘ	b_p_tw;		// 'textwidth'
    ದೀರ್ಘ	b_p_tw_nobin;	// b_p_tw saved for binary mode
    ದೀರ್ಘ	b_p_tw_nopaste;	// b_p_tw saved for paste mode
    ದೀರ್ಘ	b_p_wm;		// 'wrapmargin'
    ದೀರ್ಘ	b_p_wm_nobin;	// b_p_wm saved for binary mode
    ದೀರ್ಘ	b_p_wm_nopaste;	// b_p_wm saved for paste mode
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VARTABS
    char_u	*b_p_vsts;	// 'varsofttabstop'
    ಪೂರ್ಣಾಂಕ		*b_p_vsts_array;   // 'varsofttabstop' in internal format
    char_u	*b_p_vsts_nopaste; // b_p_vsts saved for paste mode
    char_u	*b_p_vts;	// 'vartabstop'
    ಪೂರ್ಣಾಂಕ		*b_p_vts_array;	// 'vartabstop' in internal format
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_KEYMAP
    char_u	*b_p_keymap;	// 'keymap'
#ಆದರೆ_ಅಂತ್ಯ

    /*
     * local values for options which are normally global
     */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
    char_u	*b_p_gefm;	// 'grepformat' local value
    char_u	*b_p_gp;	// 'grepprg' local value
    char_u	*b_p_mp;	// 'makeprg' local value
    char_u	*b_p_efm;	// 'errorformat' local value
#ಆದರೆ_ಅಂತ್ಯ
    char_u	*b_p_ep;	// 'equalprg' local value
    char_u	*b_p_ಮಾರ್ಗ;	// 'path' local value
    ಪೂರ್ಣಾಂಕ		b_p_ar;		// 'autoread' local value
    char_u	*b_p_ಟ್ಯಾಗುಗಳು;	// 'tags' local value
    char_u	*b_p_tc;	// 'tagcase' local value
    ಚಿಹ್ನೆರಹಿತ	b_tc_flags;     // flags for 'tagcase'
    char_u	*b_p_dict;	// 'dictionary' local value
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
    char_u	*b_p_dia;	// 'diffanchors' local value
#ಆದರೆ_ಅಂತ್ಯ
    char_u	*b_p_tsr;	// 'thesaurus' local value
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_COMPL_FUNC
    char_u	*b_p_tsrfu;	// 'thesaurusfunc' local value
    callback_T	b_tsrfu_cb;	// 'thesaurusfunc' callback
#ಆದರೆ_ಅಂತ್ಯ
    ದೀರ್ಘ	b_p_ul;		// 'undolevels' local value
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PERSISTENT_UNDO
    ಪೂರ್ಣಾಂಕ		b_p_udf;	// 'undofile'
#ಆದರೆ_ಅಂತ್ಯ
    char_u	*b_p_lw;	// 'lispwords' local value
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
    ದೀರ್ಘ	b_p_twsl;	// 'termwinscroll'
#ಆದರೆ_ಅಂತ್ಯ

    /*
     * end of buffer options
     */

    // values set from b_p_cino
    ಪೂರ್ಣಾಂಕ		b_ind_level;
    ಪೂರ್ಣಾಂಕ		b_ind_open_imag;
    ಪೂರ್ಣಾಂಕ		b_ind_no_brace;
    ಪೂರ್ಣಾಂಕ		b_ind_ಮೊದಲ_open;
    ಪೂರ್ಣಾಂಕ		b_ind_open_extra;
    ಪೂರ್ಣಾಂಕ		b_ind_close_extra;
    ಪೂರ್ಣಾಂಕ		b_ind_open_ಎಡ_imag;
    ಪೂರ್ಣಾಂಕ		b_ind_jump_label;
    ಪೂರ್ಣಾಂಕ		b_ind_case;
    ಪೂರ್ಣಾಂಕ		b_ind_case_ಸಂಕೇತ;
    ಪೂರ್ಣಾಂಕ		b_ind_case_break;
    ಪೂರ್ಣಾಂಕ		b_ind_param;
    ಪೂರ್ಣಾಂಕ		b_ind_func_type;
    ಪೂರ್ಣಾಂಕ		b_ind_comment;
    ಪೂರ್ಣಾಂಕ		b_ind_ಒಳ_comment;
    ಪೂರ್ಣಾಂಕ		b_ind_ಒಳ_comment2;
    ಪೂರ್ಣಾಂಕ		b_ind_cpp_baseclass;
    ಪೂರ್ಣಾಂಕ		b_ind_continuation;
    ಪೂರ್ಣಾಂಕ		b_ind_unclosed;
    ಪೂರ್ಣಾಂಕ		b_ind_unclosed2;
    ಪೂರ್ಣಾಂಕ		b_ind_unclosed_noignore;
    ಪೂರ್ಣಾಂಕ		b_ind_unclosed_wrapped;
    ಪೂರ್ಣಾಂಕ		b_ind_unclosed_whiteok;
    ಪೂರ್ಣಾಂಕ		b_ind_matching_paren;
    ಪೂರ್ಣಾಂಕ		b_ind_paren_prev;
    ಪೂರ್ಣಾಂಕ		b_ind_maxparen;
    ಪೂರ್ಣಾಂಕ		b_ind_maxcomment;
    ಪೂರ್ಣಾಂಕ		b_ind_scopedecl;
    ಪೂರ್ಣಾಂಕ		b_ind_scopedecl_ಸಂಕೇತ;
    ಪೂರ್ಣಾಂಕ		b_ind_java;
    ಪೂರ್ಣಾಂಕ		b_ind_js;
    ಪೂರ್ಣಾಂಕ		b_ind_keep_case_label;
    ಪೂರ್ಣಾಂಕ		b_ind_hash_comment;
    ಪೂರ್ಣಾಂಕ		b_ind_cpp_namespace;
    ಪೂರ್ಣಾಂಕ		b_ind_if_for_while;
    ಪೂರ್ಣಾಂಕ		b_ind_cpp_extern_c;
    ಪೂರ್ಣಾಂಕ		b_ind_pragma;

    linenr_T	b_no_eol_lnum;	// non-zero lnum when last line of next binary
				// write should not have an end-of-line

    ಪೂರ್ಣಾಂಕ		b_ಆರಂಭ_eof;	// last line had eof (CTRL-Z) when it was read
    ಪೂರ್ಣಾಂಕ		b_ಆರಂಭ_eol;	// last line had eol when it was read
    ಪೂರ್ಣಾಂಕ		b_ಆರಂಭ_ffc;	// first char of 'ff' when edit started
    char_u	*b_ಆರಂಭ_fenc;	// 'fileencoding' when edit started or NULL
    ಪೂರ್ಣಾಂಕ		b_bad_char;	// "++bad=" argument when edit started or 0
    ಪೂರ್ಣಾಂಕ		b_ಆರಂಭ_bomb;	// 'bomb' when it was read

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    dictitem_T	b_bufvar;	// variable for "b:" Dictionary
    dict_T	*b_vars;	// internal variables, local to buffer

    listener_T	*b_listener;       // Listeners accepting buffered reports.
    listener_T	*b_sync_listener;  // Listeners requiring unbuffered reports.
    list_T	*b_recorded_changes;
    size_t	b_recorded_ಪಠ್ಯ_ಗಾತ್ರ;  // bytes of text held by the above
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
    bool	b_has_textprop;	// true when text props were added
    hashtab_T	*b_proptypes;	// text property types local to buffer
    proptype_T	**b_proparray;	// entries of b_proptypes sorted on tp_id
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_BEVAL) && defined(FEAT_EVAL)
    char_u	*b_p_bexpr;	// 'balloonexpr' local value
    long_u	b_p_bexpr_flags;// flags for 'balloonexpr'
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CRYPT
    char_u	*b_p_cm;	// 'cryptmethod'
#ಆದರೆ_ಅಂತ್ಯ

    // When a buffer is created, it starts without a swap file.  b_may_swap is
    // then set to indicate that a swap file may be opened later.  It is reset
    // if a swap file could not be opened.
    bool	b_may_ಅದಲುಬದಲು;
    bool	b_did_warn;	// Set to true if user has been warned on first
				// change of a read-only file

    // Two special kinds of buffers:
    // help buffer  - used for help files, won't use a swap file.
    // spell buffer - used for spell info, never displayed and doesn't have a
    //		      file name.
    bool	b_help;		// true for help file buffer (when set b_p_bt
				// is "help")
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SPELL
    bool	b_spell;	// true for a spell file buffer, most fields
				// are not used!  Use the B_SPELL macro to
				// access b_spell without #ifdef.
#ಆದರೆ_ಅಂತ್ಯ

    bool	b_shortname;	// this file has an 8.3 file name

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
    char_u	*b_prompt_ಪಠ್ಯ;		// set by prompt_setprompt()
    callback_T	b_prompt_callback;	// set by prompt_setcallback()
    callback_T	b_prompt_ಅಡಚಣೆ;	// set by prompt_setinterrupt()
    ಪೂರ್ಣಾಂಕ		b_prompt_ಸೇರಿಸು;	// value for restart_edit when entering
					// a prompt buffer window.
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MZSCHEME
    ಶೂನ್ಯ	*b_mzscheme_ref; // The MzScheme reference to this buffer
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PERL
    ಶೂನ್ಯ	*b_perl_ಖಾಸಗಿ;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PYTHON
    ಶೂನ್ಯ	*b_python_ref;	// The Python reference to this buffer
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PYTHON3
    ಶೂನ್ಯ	*b_python3_ref;	// The Python3 reference to this buffer
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TCL
    ಶೂನ್ಯ	*b_tcl_ref;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RUBY
    ಶೂನ್ಯ	*b_ruby_ref;
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_SYN_HL) || defined(FEAT_SPELL)
    synblock_T	b_s;		// Info related to syntax highlighting.  w_s
				// normally points to this, but some windows
				// may use a different synblock_T.
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SIGNS
    sign_ನಮೂದು_T *b_signlist;	   // list of placed signs
    sign_ನಮೂದು_T *b_sign_finger;   // last sign inserted, used to speed up
				   // inserting signs in ascending line order
# ifdef FEAT_NETBEANS_INTG
    bool	b_has_sign_ಸ್ತಂಭ; // Flag that is set when a first sign is
				   // added and remains set until the end of
				   // the netbeans session.
# endif
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_NETBEANS_INTG
    bool	b_netbeans_file;    // true when buffer is owned by NetBeans
    bool	b_was_netbeans_file;// true if b_netbeans_file was once set
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
    bool	b_write_ಗಮ್ಯ_channel; // true when appended lines are written to
				    // a channel.
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CRYPT
    cryptstate_T *b_cryptstate;	// Encryption state while reading or writing
				// the file. NULL when not using encryption.
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		b_mapped_ctrl_c; // modes where CTRL-C is mapped

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
    ಪದ_T	*b_ಪದ;	// When not NULL this buffer is for a terminal
				// window.
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
    bool	b_diff_failed;	// internal diff failed for this buffer
#ಆದರೆ_ಅಂತ್ಯ
}; // file_buffer


#ಆದರೆ defined(FEAT_DIFF)
/*
 * Stuff for diff mode.
 */
# define DB_ಎಣಿಕೆ 8	// up to eight buffers can be diff'ed

/*
 * Each diffblock defines where a block of lines starts in each of the buffers
 * and how many lines it occupies in that buffer.  When the lines are missing
 * in the buffer the df_count[] is zero.  This is all counted in
 * buffer lines.
 * Usually there is always at least one unchanged line in between the diffs as
 * otherwise it would have been included in the diff above or below it.  When
 * linematch or diff anchors are used, this is no longer guaranteed, and we may
 * have adjacent diff blocks.  In all cases they will not overlap, although it
 * is possible to have multiple 0-count diff blocks at the same line.
 * df_lnum[] + df_count[] is the lnum below the change.  When in one buffer
 * lines have been inserted, in the other buffer df_lnum[] is the line below
 * the insertion and df_count[] is zero.  When appending lines at the end of
 * the buffer, df_lnum[] is one beyond the end!
 * This is using a linked list, because the number of differences is expected
 * to be reasonable small.  The list is sorted on lnum.
 * Each diffblock also contains a cached list of inline diff of changes within
 * the block, used for highlighting.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ diffblock_S diff_T;
ರಚನೆ diffblock_S
{
    diff_T	*df_ಮುಂದಿನ;
    linenr_T	df_lnum[DB_ಎಣಿಕೆ];	// line number in buffer
    linenr_T	df_ಎಣಿಕೆ[DB_ಎಣಿಕೆ];	// nr of inserted/changed lines
    ಪೂರ್ಣಾಂಕ is_linematched;  // has the linematch algorithm ran on this diff hunk to divide it into
			  // smaller diff hunks?

    ಪೂರ್ಣಾಂಕ		has_changes;		// has cached list of inline changes
    garray_T	df_changes;		// list of inline changes (diffline_change_T)
};

/*
 * Each entry stores a single inline change within a diff block. Line numbers
 * are recorded as relative offsets, and columns are byte offsets, not
 * character counts.
 * Ranges are [start,end), with the end being exclusive.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ diffline_change_S diffline_change_T;
ರಚನೆ diffline_change_S
{
    colnr_T	dc_ಆರಂಭ[DB_ಎಣಿಕೆ];	// byte offset of start of range in the line
    colnr_T	dc_ಕೊನೆ[DB_ಎಣಿಕೆ];	// 1 past byte offset of end of range in line
    ಪೂರ್ಣಾಂಕ		dc_ಆರಂಭ_lnum_off[DB_ಎಣಿಕೆ];	// starting line offset
    ಪೂರ್ಣಾಂಕ		dc_ಕೊನೆ_lnum_off[DB_ಎಣಿಕೆ];	// end line offset
};

/*
 * Describes a single line's list of inline changes. Use diff_change_parse() to
 * parse this.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ diffline_S diffline_T;
ರಚನೆ diffline_S
{
    diffline_change_T *changes;
    ಪೂರ್ಣಾಂಕ ಅಂಕೆ_changes;
    ಪೂರ್ಣಾಂಕ bufidx;
    ಪೂರ್ಣಾಂಕ lineoff;
};
#ಇಲ್ಲದಿದ್ದರೆ  // FEAT_DIFF
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಶೂನ್ಯ diffline_T;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಶೂನ್ಯ diffline_change_T;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು SNAP_HELP_IDX	    0
#ವ್ಯಾಖ್ಯಾನಿಸು SNAP_AUCMD_IDX	    1
#ವ್ಯಾಖ್ಯಾನಿಸು SNAP_QUICKFIX_IDX   2
#ವ್ಯಾಖ್ಯಾನಿಸು SNAP_ಎಣಿಕೆ	    3

/*
 * Tab pages point to the top frame of each tab page.
 * Note: Most values are NOT valid for the current tab page!  Use "curwin",
 * "firstwin", etc. for that.  "tp_topframe" is always valid and can be
 * compared against "topframe" to find the current tab page.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ tabpage_S tabpage_T;
ರಚನೆ tabpage_S
{
    tabpage_T	    *tp_ಮುಂದಿನ;	    // next tabpage or NULL
    ಚೌಕಟ್ಟು_T	    *tp_topframe;   // topframe for the windows
    win_T	    *tp_curwin;	    // current window in this Tab page
    win_T	    *tp_prevwin;    // previous window in this Tab page
    win_T	    *tp_firstwin;   // first window in this Tab page
    win_T	    *tp_lastwin;    // last window in this Tab page
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
    win_T	    *tp_ಮೊದಲ_popupwin; // first popup window in this Tab page
#ಆದರೆ_ಅಂತ್ಯ
    ದೀರ್ಘ	    tp_old_ಅಡ್ಡಸಾಲುಗಳು;    // Rows when Tab page was left
    ದೀರ್ಘ	    tp_old_Columns; // Columns when Tab page was left, -1 when
				    // calling shell_new_columns() postponed
    ಪೂರ್ಣಾಂಕ		    tp_old_coloff;  // Column offset when Tab page was left
    ದೀರ್ಘ	    tp_ch_ಬಳಸಿದ;	    // value of 'cmdheight' when frame size
				    // was set
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    ಪೂರ್ಣಾಂಕ		    tp_prev_which_scrollbars[3];
				    // previous value of which_scrollbars
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		    tp_did_tabclosedpre;  // whether TabClosedPre was triggered

    char_u	    *tp_localdir;	// absolute path of local directory or
					// NULL
    char_u	    *tp_prevdir;	// previous directory

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
    diff_T	    *tp_ಮೊದಲ_diff;
    buf_T	    *(tp_diffbuf[DB_ಎಣಿಕೆ]);
    ಪೂರ್ಣಾಂಕ		    tp_diff_invalid;	// list of diffs is outdated
    ಪೂರ್ಣಾಂಕ		    tp_diff_update;	// update diffs before redrawing
#ಆದರೆ_ಅಂತ್ಯ
    ಚೌಕಟ್ಟು_T	    *(tp_snapshot[SNAP_ಎಣಿಕೆ]);  // window layout snapshots
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    dictitem_T	    tp_winvar;	    // variable for "t:" Dictionary
    dict_T	    *tp_vars;	    // internal variables, local to tab page
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PYTHON
    ಶೂನ್ಯ	    *tp_python_ref;	// The Python value for this tab page
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PYTHON3
    ಶೂನ್ಯ	    *tp_python3_ref;	// The Python value for this tab page
#ಆದರೆ_ಅಂತ್ಯ
};

/*
 * Structure to cache info for displayed lines in w_lines[].
 * Each logical line has one entry.
 * The entry tells how the logical line is currently displayed in the window.
 * This is updated when displaying the window.
 * When the display is changed (e.g., when clearing the screen) w_lines_valid
 * is changed to exclude invalid entries.
 * When making changes to the buffer, wl_valid is reset to indicate wl_size
 * may not reflect what is actually in the buffer.  When wl_valid is FALSE,
 * the entries can only be used to count the number of displayed lines used.
 * wl_lnum and wl_lastlnum are invalid too.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ w_ಸಾಲು
{
    linenr_T	wl_lnum;	// buffer line number for logical line
    short_u	wl_ಗಾತ್ರ;	// height in screen lines
    ಅಕ್ಷರ	wl_valid;	// TRUE values are valid for text in buffer
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
    ಅಕ್ಷರ	wl_folded;	// TRUE when this is a range of folded lines
    linenr_T	wl_lastlnum;	// last buffer line number for logical line
#ಆದರೆ_ಅಂತ್ಯ
} wline_T;

/*
 * Windows are kept in a tree of frames.  Each frame has a column (FR_COL)
 * or row (FR_ROW) layout or is a leaf, which has a window.
 */
ರಚನೆ ಚೌಕಟ್ಟು_S
{
    ಅಕ್ಷರ	fr_layout;	// FR_LEAF, FR_COL or FR_ROW
    ಪೂರ್ಣಾಂಕ		fr_ಅಗಲ;
    ಪೂರ್ಣಾಂಕ		fr_newwidth;	// new width used in win_equal_rec()
    ಪೂರ್ಣಾಂಕ		fr_ಎತ್ತರ;
    ಪೂರ್ಣಾಂಕ		fr_newheight;	// new height used in win_equal_rec()
    ಚೌಕಟ್ಟು_T	*fr_ಪೋಷಕ;	// containing frame or NULL
    ಚೌಕಟ್ಟು_T	*fr_ಮುಂದಿನ;	// frame right or below in same parent, NULL
				// for last
    ಚೌಕಟ್ಟು_T	*fr_prev;	// frame left or above in same parent, NULL
				// for first
    // fr_child and fr_win are mutually exclusive
    ಚೌಕಟ್ಟು_T	*fr_ಮಗು;	// first contained frame
    win_T	*fr_win;	// window that fills this frame; for a snapshot
				// set to the current window
};

#ವ್ಯಾಖ್ಯಾನಿಸು FR_ಎಲೆ	0	// frame is a leaf
#ವ್ಯಾಖ್ಯಾನಿಸು FR_ಅಡ್ಡಸಾಲು	1	// frame with a row of windows
#ವ್ಯಾಖ್ಯಾನಿಸು FR_ಲಂಬಸಾಲು	2	// frame with a column of windows

/*
 * Struct used for highlighting 'hlsearch' matches, matches defined by
 * ":match" and matches defined by match functions.
 * For 'hlsearch' there is one pattern for all windows.  For ":match" and the
 * match functions there is a different pattern for each window.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    regmmatch_T	rm;	    // points to the regexp program; contains last
			    // found match (may continue in next line)
    buf_T	*buf;	    // the buffer to search for a match
    linenr_T	lnum;	    // the line to search for a match
    ಪೂರ್ಣಾಂಕ		attr;	    // attributes to be used for a match
    ಪೂರ್ಣಾಂಕ		attr_cur;   // attributes currently active in win_line()
    linenr_T	ಮೊದಲ_lnum; // first lnum to search for multi-line pat
    colnr_T	startcol;   // in win_line() points to char where HL starts
    colnr_T	endcol;	    // in win_line() points to char where HL ends
    ಅಕ್ಷರ	is_addpos;  // position specified directly by
			    // matchaddpos(). TRUE/FALSE
    ಅಕ್ಷರ	has_cursor; // TRUE if the cursor is inside the match, used for
			    // CurSearch
} ಹೊಂದಿಕೆ_T;

/*
 * Same as lpos_T, but with additional field len.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    linenr_T	lnum;	// line number
    colnr_T	ಲಂಬಸಾಲು;	// column number
    ಪೂರ್ಣಾಂಕ		len;	// length: 0 - to the end of line
} llpos_T;

/*
 * matchitem_T provides a linked list for storing match items for ":match",
 * matchadd() and matchaddpos().
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ matchitem matchitem_T;
ರಚನೆ matchitem
{
    matchitem_T	*mit_ಮುಂದಿನ;
    ಪೂರ್ಣಾಂಕ		mit_id;		// match ID
    ಪೂರ್ಣಾಂಕ		mit_ಆದ್ಯತೆ;   // match priority

    // Either a pattern is defined (mit_pattern is not NUL) or a list of
    // positions is given (mit_pos is not NULL and mit_pos_count > 0).
    char_u	*mit_ನಮೂನೆ;   // pattern to highlight
    regmmatch_T	mit_ಹೊಂದಿಕೆ;	// regexp program for pattern

    llpos_T	*mit_pos_array;	// array of positions
    ಪೂರ್ಣಾಂಕ		mit_pos_ಎಣಿಕೆ;	// nr of entries in mit_pos
    ಪೂರ್ಣಾಂಕ		mit_pos_cur;	// internal position counter
    linenr_T	mit_toplnum;	// top buffer line
    linenr_T	mit_botlnum;	// bottom buffer line

    ಹೊಂದಿಕೆ_T	mit_hl;		// struct for doing the actual highlighting
    ಪೂರ್ಣಾಂಕ		mit_hlg_id;	// highlight group ID
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CONCEAL
    ಪೂರ್ಣಾಂಕ		mit_conceal_char; // cchar for Conceal highlighting
#ಆದರೆ_ಅಂತ್ಯ
};

// Structure to store last cursor position and topline.  Used by check_lnums()
// and reset_lnums().
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    ಪೂರ್ಣಾಂಕ		w_topline_save;	// original topline value
    ಪೂರ್ಣಾಂಕ		w_topline_corr;	// corrected topline value
    pos_T	w_cursor_save;	// original cursor position
    pos_T	w_cursor_corr;	// corrected cursor position
} pos_save_T;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಪೂರ್ಣಾಂಕ		wb_startcol;
    ಪೂರ್ಣಾಂಕ		wb_endcol;
    vimmenu_T	*wb_menu;
} winbar_ಅಂಶ_T;
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Characters from the 'listchars' option
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    ಪೂರ್ಣಾಂಕ		eol;
    ಪೂರ್ಣಾಂಕ		ext;
    ಪೂರ್ಣಾಂಕ		prec;
    ಪೂರ್ಣಾಂಕ		nbsp;
    ಪೂರ್ಣಾಂಕ		space;
    ಪೂರ್ಣಾಂಕ		tab1;
    ಪೂರ್ಣಾಂಕ		tab2;
    ಪೂರ್ಣಾಂಕ		tab3;
    ಪೂರ್ಣಾಂಕ		trail;
    ಪೂರ್ಣಾಂಕ		lead;
    ಪೂರ್ಣಾಂಕ		leadtab1;
    ಪೂರ್ಣಾಂಕ		leadtab2;
    ಪೂರ್ಣಾಂಕ		leadtab3;
    ಪೂರ್ಣಾಂಕ		*multispace;
    ಪೂರ್ಣಾಂಕ		*leadmultispace;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CONCEAL
    ಪೂರ್ಣಾಂಕ		conceal;
#ಆದರೆ_ಅಂತ್ಯ
} lcs_chars_T;

/*
 * Characters from the 'fillchars' option
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    ಪೂರ್ಣಾಂಕ	stl;
    ಪೂರ್ಣಾಂಕ	stlnc;
    ಪೂರ್ಣಾಂಕ	vert;
    ಪೂರ್ಣಾಂಕ	fold;
    ಪೂರ್ಣಾಂಕ	foldopen;
    ಪೂರ್ಣಾಂಕ	foldclosed;
    ಪೂರ್ಣಾಂಕ	foldsep;
    ಪೂರ್ಣಾಂಕ	foldinner;
    ಪೂರ್ಣಾಂಕ	diff;
    ಪೂರ್ಣಾಂಕ	eob;
    ಪೂರ್ಣಾಂಕ	lastline;
#ಆದರೆ defined(FEAT_TABPANEL)
    ಪೂರ್ಣಾಂಕ	tpl_vert;
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ trunc;
    ಪೂರ್ಣಾಂಕ truncrl;
} fill_chars_T;

/*
 * Represents current highlight overrides (used by 'winhighlight' option). The
 * highlight group with ID "from" will be overridden by the highlight group with
 * ID "to"
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    ಪೂರ್ಣಾಂಕ from; // If zero or negative then it is hlf_T enum
    ಪೂರ್ಣಾಂಕ ಗಮ್ಯ; // Same thing as "from"
} hl_override_T;

/*
 * Structure which contains all information that belongs to a window
 *
 * All row numbers are relative to the start of the window, except w_winrow.
 */
ರಚನೆ ಕಿಟಕಿ_S
{
    ಪೂರ್ಣಾಂಕ		w_id;		    // unique window ID

    buf_T	*w_ಬಫರ್;	    // buffer we are a window into

    win_T	*w_prev;	    // link to previous window
    win_T	*w_ಮುಂದಿನ;	    // link to next window

#ಆದರೆ defined(FEAT_SYN_HL) || defined(FEAT_SPELL)
    synblock_T	*w_s;		    // for :ownsyntax
#ಆದರೆ_ಅಂತ್ಯ

    ಪೂರ್ಣಾಂಕ		w_locked;	    // don't let autocommands close the window

    ಚೌಕಟ್ಟು_T	*w_ಚೌಕಟ್ಟು;	    // frame containing this window

    pos_T	w_cursor;	    // cursor position in buffer

    colnr_T	w_curswant;	    // The column we'd like to be at.  This is
				    // used to try to stay in the same column
				    // for up/down cursor motions.

    bool	w_ಗಣ_curswant;	    // If set, then update w_curswant the next
				    // time through cursupdate() to the
				    // current virtual column

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
    linenr_T	w_ಕೊನೆಯ_cursorline;  // where last time 'cursorline' was drawn
#ಆದರೆ_ಅಂತ್ಯ

    /*
     * the next seven are used to update the Visual highlighting
     */
    ಅಕ್ಷರ	w_old_visual_ಬಹುಲಕ;  // last known VIsual_mode
    linenr_T	w_old_cursor_lnum;  // last known end of visual part
    colnr_T	w_old_cursor_fcol;  // first column for block visual part
    colnr_T	w_old_cursor_lcol;  // last column for block visual part
    linenr_T	w_old_visual_lnum;  // last known start of visual part
    colnr_T	w_old_visual_ಲಂಬಸಾಲು;   // last known start of visual part
    colnr_T	w_old_curswant;	    // last known value of Curswant

    linenr_T    w_ಕೊನೆಯ_cursor_lnum_rnu;  // cursor lnum when 'rnu' was last
					 // redrawn

    lcs_chars_T	w_lcs_chars;	    // 'listchars' characters
    fill_chars_T w_fill_chars;	    // 'fillchars' characters

    /*
     * "w_topline", "w_leftcol" and "w_skipcol" specify the offsets for
     * displaying the buffer.
     */
    linenr_T	w_topline;	    // buffer line number of the line at the
				    // top of the window
    bool	w_topline_was_ಗಣ;  // flag set to true when topline is set,
				    // e.g. by winrestview()

    linenr_T	w_botline;	    // number of the line below the bottom of
				    // the window

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
    ಪೂರ್ಣಾಂಕ		w_topfill;	    // number of filler lines above w_topline
    ಪೂರ್ಣಾಂಕ		w_old_topfill;	    // w_topfill at last redraw
    bool	w_botfill;	    // true when filler lines are actually
				    // below w_topline (at end of file)
    bool	w_old_botfill;	    // w_botfill at last redraw
#ಆದರೆ_ಅಂತ್ಯ
    colnr_T	w_leftcol;	    // screen column number of the left most
				    // character in the window; used when
				    // 'wrap' is off
    colnr_T	w_skipcol;	    // starting screen column for the first
				    // line in the window; used when 'wrap' is
				    // on; does not include win_col_off()

    ಪೂರ್ಣಾಂಕ		w_ಖಾಲಿಗಳು_ಅಡ್ಡಸಾಲುಗಳು;	    // number of ~ rows in window
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
    ಪೂರ್ಣಾಂಕ		w_filler_ಅಡ್ಡಸಾಲುಗಳು;	    // number of filler rows at the end of the
				    //ಕಿಟಕಿ
#ಆದರೆ_ಅಂತ್ಯ

    // six fields that are only used when there is a WinScrolled autocommand
    linenr_T	w_ಕೊನೆಯ_topline;	    // last known value for w_topline
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
    ಪೂರ್ಣಾಂಕ		w_ಕೊನೆಯ_topfill;	    // last known value for w_topfill
#ಆದರೆ_ಅಂತ್ಯ
    colnr_T	w_ಕೊನೆಯ_leftcol;	    // last known value for w_leftcol
    colnr_T	w_ಕೊನೆಯ_skipcol;	    // last known value for w_skipcol
    ಪೂರ್ಣಾಂಕ		w_ಕೊನೆಯ_ಅಗಲ;	    // last known value for w_width
    ಪೂರ್ಣಾಂಕ		w_ಕೊನೆಯ_ಎತ್ತರ;	    // last known value for w_height

    /*
     * Layout of the window in the screen.
     * May need to add "msg_scrolled" to "w_winrow" in rare situations.
     */
    ಪೂರ್ಣಾಂಕ		w_winrow;	    // first row of window in screen
    ಪೂರ್ಣಾಂಕ		w_ಎತ್ತರ;	    // number of rows in window, excluding
				    // status/command/winbar line(s)
    ಪೂರ್ಣಾಂಕ		w_prev_winrow;	    // previous winrow used for 'splitkeep'
    ಪೂರ್ಣಾಂಕ		w_prev_ಎತ್ತರ;	    // previous height used for 'splitkeep'
    ಪೂರ್ಣಾಂಕ		w_stl_rendered_ಎತ್ತರ; // rendered height of window-local 'stl'
				    // (number of "%@" + 1)
    stl_click_region_T *w_stl_click;  // statusline click regions
    ಪೂರ್ಣಾಂಕ		w_stl_click_ಎಣಿಕೆ;  // number of click regions
    ಪೂರ್ಣಾಂಕ		w_ಸ್ಥಿತಿ_ಎತ್ತರ;    // number of status lines.
				    // If 'statuslineopt' was changed, this
				    // member holds the previous value until
				    // frame_change_statusline_height() calls.
    ಪೂರ್ಣಾಂಕ		w_wincol;	    // Leftmost column of window in screen.
    ಪೂರ್ಣಾಂಕ		w_ಅಗಲ;	    // Width of window, excluding separation.
    ಪೂರ್ಣಾಂಕ		w_vsep_ಅಗಲ;	    // Number of separator columns (0 or 1).

    pos_save_T	w_save_cursor;	    // backup of cursor pos and topline
    bool	w_do_win_fix_cursor;// if true cursor may be invalid

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
    ಪೂರ್ಣಾಂಕ		w_popup_flags;	    // POPF_ values
    ಪೂರ್ಣಾಂಕ		w_popup_blend;	    // 0-100: transparency level for popup with opacitys
    ಪೂರ್ಣಾಂಕ		w_popup_handled;    // POPUP_HANDLE[0-9] flags
    char_u	*w_popup_title;
    poppos_T	w_popup_pos;
    bool	w_popup_fixed;	    // do not shift popup to fit on screen
    ಪೂರ್ಣಾಂಕ		w_popup_prop_type;  // when not zero: textprop type ID
    win_T	*w_popup_prop_win;  // window to search for textprop
    ಪೂರ್ಣಾಂಕ		w_popup_prop_id;    // when not zero: textprop ID
    ಪೂರ್ಣಾಂಕ		w_zindex;
    ಪೂರ್ಣಾಂಕ		w_minheight;	    // "minheight" for popup window
    ಪೂರ್ಣಾಂಕ		w_minwidth;	    // "minwidth" for popup window
    ಪೂರ್ಣಾಂಕ		w_maxheight;	    // "maxheight" for popup window
    ಪೂರ್ಣಾಂಕ		w_maxwidth;	    // "maxwidth" for popup window
    ಪೂರ್ಣಾಂಕ		w_maxwidth_opt;	    // maxwidth from option
    ಪೂರ್ಣಾಂಕ		w_wantline;	    // "line" for popup window
    ಪೂರ್ಣಾಂಕ		w_wantcol;	    // "col" for popup window
    ಪೂರ್ಣಾಂಕ		w_firstline;	    // "firstline" for popup window
    ಪೂರ್ಣಾಂಕ		w_want_scrollbar;   // when zero don't use a scrollbar
    bool	w_has_scrollbar;    // true if scrollbar displayed
    char_u	*w_scrollbar_highlight; // "scrollbarhighlight"
    char_u	*w_thumb_highlight; // "thumbhighlight"
    ಪೂರ್ಣಾಂಕ		w_popup_padding[4]; // popup padding top/right/bot/left
    ಪೂರ್ಣಾಂಕ		w_popup_border[4];  // popup border top/right/bot/left
    char_u	*w_border_highlight[4];  // popup border highlight
    bool	w_border_highlight_isset; // borderhighlight was explicitly set
    ಪೂರ್ಣಾಂಕ		w_border_char[8];   // popup border characters
    ಪೂರ್ಣಾಂಕ		w_popup_shadow;     // popup shadow (right and bottom edges)

    ಪೂರ್ಣಾಂಕ		w_popup_leftoff;    // columns left of the screen
    ಪೂರ್ಣಾಂಕ		w_popup_rightoff;   // columns right of the screen
    ಪೂರ್ಣಾಂಕ		w_popup_topoff;	    // rows above the host window's top
				    // when "clipwindow" is set
    ಪೂರ್ಣಾಂಕ		w_popup_bottomoff;  // rows below the host window's bottom
				    // when "clipwindow" is set
    ಪೂರ್ಣಾಂಕ		w_popup_leftclip;   // columns left of the host window's left
				    // when "clipwindow" is set
    ಪೂರ್ಣಾಂಕ		w_popup_rightclip;  // columns right of the host window's right
				    // when "clipwindow" is set
    varnumber_T	w_popup_ಕೊನೆಯ_changedtick; // b:changedtick of popup buffer
					  // when position was computed
    varnumber_T	w_popup_prop_changedtick; // b:changedtick of buffer with
					  // w_popup_prop_type when position
					  // was computed
    ಪೂರ್ಣಾಂಕ		w_popup_prop_topline; // w_topline of window with
				      // w_popup_prop_type when position was
				      // computed
    ಪೂರ್ಣಾಂಕ		w_popup_prop_winrow;  // w_winrow of host window when
				      // position was computed
    ಪೂರ್ಣಾಂಕ		w_popup_prop_wincol;  // w_wincol of host window when
				      // position was computed
    ಪೂರ್ಣಾಂಕ		w_popup_prop_ಅಗಲ;   // w_width of host window when
				      // position was computed
    ಪೂರ್ಣಾಂಕ		w_popup_prop_winheight; // w_height of host window when
				      // position was computed
    linenr_T	w_popup_ಕೊನೆಯ_curline; // last known w_cursor.lnum of window
				      // with "cursorline" set
    callback_T	w_close_cb;	    // popup close callback
    callback_T	w_filter_cb;	    // popup filter callback
    ಪೂರ್ಣಾಂಕ		w_filter_errors;    // popup filter error count
    ಪೂರ್ಣಾಂಕ		w_filter_ಬಹುಲಕ;	    // mode when filter callback is used

    win_T	*w_popup_curwin;    // close popup if curwin differs
    linenr_T	w_popup_lnum;	    // close popup if cursor not on this line
    colnr_T	w_popup_mincol;	    // close popup if cursor before this col
    colnr_T	w_popup_maxcol;	    // close popup if cursor after this col
    ಪೂರ್ಣಾಂಕ		w_popup_mouse_ಅಡ್ಡಸಾಲು;  // close popup if mouse moves away
    ಪೂರ್ಣಾಂಕ		w_popup_mouse_mincol;  // close popup if mouse moves away
    ಪೂರ್ಣಾಂಕ		w_popup_mouse_maxcol;  // close popup if mouse moves away
    popclose_T	w_popup_close;	    // allow closing the popup with the mouse

    list_T	*w_popup_ಮುಖವಾಡ;	     // list of lists for "mask"
    char_u	*w_popup_ಮುಖವಾಡ_cells; // cached mask cells
    ಪೂರ್ಣಾಂಕ		w_popup_ಮುಖವಾಡ_ಎತ್ತರ; // height of w_popup_mask_cells
    ಪೂರ್ಣಾಂಕ		w_popup_ಮುಖವಾಡ_ಅಗಲ;  // width of w_popup_mask_cells

# ಆದರೆ defined(FEAT_TIMERS)
    timer_T	*w_popup_timer;	    // timer for closing popup window
# endif

    ಪೂರ್ಣಾಂಕ		w_flags;	    // WFLAG_ flags

# define WFLAG_WCOL_OFF_ADDED	1   // popup border and padding were added to
				    // w_wcol
# define WFLAG_WROW_OFF_ADDED	2   // popup border and padding were added to
				    // w_wrow

# ifdef FEAT_IMAGE
    image_placement_T *w_popup_imagep;
# endif
#ಆದರೆ_ಅಂತ್ಯ

    /*
     * === start of cached values ====
     */
    /*
     * Recomputing is minimized by storing the result of computations.
     * Use functions in screen.c to check if they are valid and to update.
     * w_valid is a bitfield of flags, which indicate if specific values are
     * valid or need to be recomputed.	See screen.c for values.
     */
    ಪೂರ್ಣಾಂಕ		w_valid;
    pos_T	w_valid_cursor;	    // last known position of w_cursor, used
				    // to adjust w_valid
    colnr_T	w_valid_leftcol;    // last known w_leftcol
    colnr_T	w_valid_skipcol;    // last known w_skipcol

    /*
     * w_cline_height is the number of physical lines taken by the buffer line
     * that the cursor is on.  We use this to avoid extra calls to plines().
     */
    ಪೂರ್ಣಾಂಕ		w_cline_ಎತ್ತರ;	    // current size of cursor line
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
    bool	w_cline_folded;	    // cursor line is folded
#ಆದರೆ_ಅಂತ್ಯ

    ಪೂರ್ಣಾಂಕ		w_cline_ಅಡ್ಡಸಾಲು;	    // starting row of the cursor line

    colnr_T	w_virtcol;	    // column number of the cursor in the
				    // buffer line, as opposed to the column
				    // number we're at on the screen.  This
				    // makes a difference on lines which span
				    // more than one screen line or when
				    // w_leftcol is non-zero

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
    colnr_T	w_virtcol_ಮೊದಲ_char;	// offset for w_virtcol when there are
					// virtual text properties above the
					//ಸಾಲು
#ಆದರೆ_ಅಂತ್ಯ
    /*
     * w_wrow and w_wcol specify the cursor position in the window.
     * This is related to positions in the window, not in the display or
     * buffer, thus w_wrow is relative to w_winrow.
     */
    ಪೂರ್ಣಾಂಕ		w_wrow, w_wcol;	    // cursor position in window
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CONCEAL
    ಪೂರ್ಣಾಂಕ		w_wcol_conceal_off; // screen cells concealed before w_wcol on
				    // the cursor's screen line, set by win_line()
#ಆದರೆ_ಅಂತ್ಯ

    /*
     * Info about the lines currently in the window is remembered to avoid
     * recomputing it every time.  The allocated size of w_lines[] is Rows.
     * Only the w_lines_valid entries are actually valid.
     * When the display is up-to-date w_lines[0].wl_lnum is equal to w_topline
     * and w_lines[w_lines_valid - 1].wl_lnum is equal to w_botline.
     * Between changing text and updating the display w_lines[] represents
     * what is currently displayed.  wl_valid is reset to indicated this.
     * This is used for efficient redrawing.
     */
    ಪೂರ್ಣಾಂಕ		w_ಸಾಲುಗಳು_valid;	    // number of valid entries
    wline_T	*w_ಸಾಲುಗಳು;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
    garray_T	w_folds;	    // array of nested folds
    bool	w_fold_manual;	    // when true: some folds are opened/closed
				    // manually
    bool	w_foldinvalid;	    // when true: folding needs to be
				    // recomputed
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
    ಪೂರ್ಣಾಂಕ		w_nrwidth;	    // width of 'number' and 'relativenumber'
				    // column being used
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
    termcellcolor_T w_ಪದ_hlfwin;  // cache for term color of HLF_WIN
#ಆದರೆ_ಅಂತ್ಯ

    /*
     * === end of cached values ===
     */

    ಪೂರ್ಣಾಂಕ		w_redr_type;	    // type of redraw to be performed on win
    ಪೂರ್ಣಾಂಕ		w_upd_ಅಡ್ಡಸಾಲುಗಳು;	    // number of window lines to update when
				    // w_redr_type is UPD_REDRAW_TOP
    linenr_T	w_redraw_ತುದಿ;	    // when != 0: first line needing redraw
    linenr_T	w_redraw_bot;	    // when != 0: last line needing redraw
    bool	w_redr_ಸ್ಥಿತಿ;	    // if true status line must be redrawn

    // remember what is shown in the ruler for this window (if 'ruler' set)
    pos_T	w_ru_cursor;	    // cursor position shown in ruler
    colnr_T	w_ru_virtcol;	    // virtcol shown in ruler
    linenr_T	w_ru_topline;	    // topline shown in ruler
    linenr_T	w_ru_ಸಾಲು_ಎಣಿಕೆ;    // line count used for ruler
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
    ಪೂರ್ಣಾಂಕ		w_ru_topfill;	    // topfill shown in ruler
#ಆದರೆ_ಅಂತ್ಯ
    bool	w_ru_ಖಾಲಿಗಳು;	    // true if ruler shows 0-1 (empty line)

    ಪೂರ್ಣಾಂಕ		w_alt_fnum;	    // alternate file (for # and CTRL-^)

    alist_T	*w_alist;	    // pointer to arglist for this window
    ಪೂರ್ಣಾಂಕ		w_arg_idx;	    // current index in argument list (can be
				    // out of range!)
    bool	w_arg_idx_invalid;  // editing another file than w_arg_idx

    char_u	*w_localdir;	    // absolute path of local directory or
				    // NULL
    char_u	*w_prevdir;	    // previous directory
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU
    vimmenu_T	*w_winbar;	    // The root of the WinBar menu hierarchy.
    winbar_ಅಂಶ_T *w_winbar_ವಸ್ತುಗಳು;  // list of items in the WinBar
    ಪೂರ್ಣಾಂಕ		w_winbar_ಎತ್ತರ;    // 1 if there is a window toolbar
#ಆದರೆ_ಅಂತ್ಯ

    /*
     * Options local to a window.
     * They are local because they influence the layout of the window or
     * depend on the window layout.
     * There are two values: w_onebuf_opt is local to the buffer currently in
     * this window, w_allbuf_opt is for all buffers in this window.
     */
    winopt_T	w_onebuf_opt;
    winopt_T	w_allbuf_opt;
    // transform a pointer to a "onebuf" option into a "allbuf" option
#ವ್ಯಾಖ್ಯಾನಿಸು GLOBAL_WO(p)	((ಅಕ್ಷರ *)(p) + ಗಾತ್ರ(winopt_T))

#ಆದರೆ defined(FEAT_SIGNS) || defined(FEAT_FOLDING) || defined(FEAT_DIFF)
    ಪೂರ್ಣಾಂಕ		*w_p_cc_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;	    // array of columns to highlight or NULL
    char_u	w_p_culopt_flags;   // flags for cursorline highlighting
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
    ಪೂರ್ಣಾಂಕ		w_briopt_min;	    // minimum width for breakindent
    ಪೂರ್ಣಾಂಕ		w_briopt_ಸರಿಸು;	    // additional shift for breakindent
    ಪೂರ್ಣಾಂಕ		w_briopt_sbr;	    // sbr in 'briopt'
    ಪೂರ್ಣಾಂಕ		w_briopt_list;      // additional indent for lists
    ಪೂರ್ಣಾಂಕ		w_briopt_vcol;	    // indent for specific column
#ಆದರೆ_ಅಂತ್ಯ

    ದೀರ್ಘ	w_scbind_pos;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    dictitem_T	w_winvar;	// variable for "w:" Dictionary
    dict_T	*w_vars;	// internal variables, local to window
#ಆದರೆ_ಅಂತ್ಯ

    /*
     * The w_prev_pcmark field is used to check whether we really did jump to
     * a new line after setting the w_pcmark.  If not, then we revert to
     * using the previous w_pcmark.
     */
    pos_T	w_pcmark;	// previous context mark
    pos_T	w_prev_pcmark;	// previous w_pcmark

    /*
     * the jumplist contains old cursor positions
     */
    xfmark_T	w_jumplist[JUMPLISTSIZE];
    ಪೂರ್ಣಾಂಕ		w_jumplistlen;		// number of active entries
    ಪೂರ್ಣಾಂಕ		w_jumplistidx;		// current position

    ಪೂರ್ಣಾಂಕ		w_changelistidx;	// current position in b_changelist

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಹುಡುಕು_EXTRA
    matchitem_T	*w_ಹೊಂದಿಕೆ_ತಲೆ;		// head of match list
    ಪೂರ್ಣಾಂಕ		w_ಮುಂದಿನ_ಹೊಂದಿಕೆ_id;	// next match ID
#ಆದರೆ_ಅಂತ್ಯ

    /*
     * the tagstack grows from 0 upwards:
     * entry 0: older
     * entry 1: newer
     * entry 2: newest
     */
    taggy_T	w_tagstack[TAGSTACKSIZE];   // the tag stack
    ಪೂರ್ಣಾಂಕ		w_tagstackidx;		    // idx just below active entry
    ಪೂರ್ಣಾಂಕ		w_tagstacklen;		    // number of tags on stack

    /*
     * w_fraction is the fractional row of the cursor within the window, from
     * 0 at the top row to FRACTION_MULT at the last row.
     * w_prev_fraction_row was the actual cursor row when w_fraction was last
     * calculated.
     */
    ಪೂರ್ಣಾಂಕ		w_fraction;
    ಪೂರ್ಣಾಂಕ		w_prev_fraction_ಅಡ್ಡಸಾಲು;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    scrollbar_T	w_scrollbars[2];	// vert. Scrollbars for this window
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
    linenr_T	w_nrwidth_ಸಾಲು_ಎಣಿಕೆ;	// line count when ml_nrwidth_width
					// was computed.
    ದೀರ್ಘ	w_nuw_cached;		// 'numberwidth' option cached
    ಪೂರ್ಣಾಂಕ		w_nrwidth_ಅಗಲ;	// nr of chars to print line count.
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
    qf_info_T	*w_llist;		// Location list for this window
    /*
     * Location list reference used in the location list window.
     * In a non-location list window, w_llist_ref is NULL.
     */
    qf_info_T	*w_llist_ref;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MZSCHEME
    ಶೂನ್ಯ	*w_mzscheme_ref;	// The MzScheme value for this window
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PERL
    ಶೂನ್ಯ	*w_perl_ಖಾಸಗಿ;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PYTHON
    ಶೂನ್ಯ	*w_python_ref;		// The Python value for this window
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PYTHON3
    ಶೂನ್ಯ	*w_python3_ref;		// The Python value for this window
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TCL
    ಶೂನ್ಯ	*w_tcl_ref;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RUBY
    ಶೂನ್ಯ	*w_ruby_ref;
#ಆದರೆ_ಅಂತ್ಯ

    hl_override_T *w_hl;
    ಪೂರ್ಣಾಂಕ		w_hl_len;
    ಪೂರ್ಣಾಂಕ		w_hlfwin_id; // Cached HLF_WIN highlight group id, zero if none,
			     // or -1 if it was set to itself.
};

/*
 * Arguments for operators.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ oparg_S
{
    ಪೂರ್ಣಾಂಕ		op_type;	// current pending operator type
    ಪೂರ್ಣಾಂಕ		regname;	// register to use for the operator
    ಪೂರ್ಣಾಂಕ		motion_type;	// type of the current cursor motion
    ಪೂರ್ಣಾಂಕ		motion_force;	// force motion type: 'v', 'V' or CTRL-V
    ಪೂರ್ಣಾಂಕ		use_reg_one;	// TRUE if delete uses reg 1 even when not
				// linewise
    ಪೂರ್ಣಾಂಕ		inclusive;	// TRUE if char motion is inclusive (only
				// valid when motion_type is MCHAR)
    ಪೂರ್ಣಾಂಕ		ಕೊನೆ_adjusted;	// backuped b_op_end one char (only used by
				// do_format())
    pos_T	ಆರಂಭ;		// start of the operator
    pos_T	ಕೊನೆ;		// end of the operator
    pos_T	cursor_ಆರಂಭ;	// cursor position before motion for "gw"

    ದೀರ್ಘ	ಸಾಲು_ಎಣಿಕೆ;	// number of lines from op_start to op_end
				// (inclusive)
    ಪೂರ್ಣಾಂಕ		ಖಾಲಿಗಳು_2;		// op_start and op_end the same (only used by
				// do_change())
    ಪೂರ್ಣಾಂಕ		is_VIsual;	// operator on Visual area
    ಪೂರ್ಣಾಂಕ		ಖಂಡ_ಬಹುಲಕ;	// current operator is Visual block mode
    colnr_T	ಆರಂಭ_vcol;	// start col for block mode operator
    colnr_T	ಕೊನೆ_vcol;	// end col for block mode operator
    ದೀರ್ಘ	prev_opcount;	// ca.opcount saved for K_CURSORHOLD
    ದೀರ್ಘ	prev_count0;	// ca.count0 saved for K_CURSORHOLD
    ಪೂರ್ಣಾಂಕ		excl_tr_ws;	// exclude trailing whitespace for yank of a
				//ಖಂಡ
} oparg_T;

/*
 * Arguments for Normal mode commands.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ cmdarg_S
{
    oparg_T	*oap;		// Operator arguments
    ಪೂರ್ಣಾಂಕ		prechar;	// prefix character (optional, always 'g')
    ಪೂರ್ಣಾಂಕ		cmdchar;	// command character
    ಪೂರ್ಣಾಂಕ		nchar;		// next command character (optional)
    ಪೂರ್ಣಾಂಕ		ncharC1;	// first composing character (optional)
    ಪೂರ್ಣಾಂಕ		ncharC2;	// second composing character (optional)
    ಪೂರ್ಣಾಂಕ		extra_char;	// yet another character (optional)
    ದೀರ್ಘ	opcount;	// count before an operator
    ದೀರ್ಘ	count0;		// count before command, default 0
    ದೀರ್ಘ	count1;		// count before command, default 1
    ಪೂರ್ಣಾಂಕ		arg;		// extra argument from nv_cmds[]
    ಪೂರ್ಣಾಂಕ		retval;		// return: CA_* values
    char_u	*searchbuf;	// return: pointer to search pattern or NULL
} cmdarg_T;

// values for retval:
#ವ್ಯಾಖ್ಯಾನಿಸು CA_COMMAND_BUSY	    1	// skip restarting edit() once
#ವ್ಯಾಖ್ಯಾನಿಸು CA_NO_ADJ_OP_ಕೊನೆ    2	// don't adjust operator end

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURSOR_SHAPE
/*
 * struct to store values from 'guicursor' and 'mouseshape'
 */
// Indexes in shape_table[]
# define SHAPE_IDX_N	0	// Normal mode
# define SHAPE_IDX_V	1	// Visual mode
# define SHAPE_IDX_I	2	// Insert mode
# define SHAPE_IDX_R	3	// Replace mode
# define SHAPE_IDX_C	4	// Command line Normal mode
# define SHAPE_IDX_CI	5	// Command line Insert mode
# define SHAPE_IDX_CR	6	// Command line Replace mode
# define SHAPE_IDX_O	7	// Operator-pending mode
# define SHAPE_IDX_VE	8	// Visual mode with 'selection' exclusive
# define SHAPE_IDX_CLINE	9	// On command line
# define SHAPE_IDX_ಸ್ಥಿತಿ 10	// A status line
# define SHAPE_IDX_SDRAG 11	// dragging a status line
# define SHAPE_IDX_VSEP	12	// A vertical separator line
# define SHAPE_IDX_VDRAG 13	// dragging a vertical separator line
# define SHAPE_IDX_ಇನ್ನಷ್ಟು	14	// Hit-return or More
# define SHAPE_IDX_MOREL	15	// Hit-return or More in last line
# define SHAPE_IDX_SM	16	// showing matching paren
# define SHAPE_IDX_ಎಣಿಕೆ	17

# define SHAPE_ಖಂಡ	0	// block cursor
# define SHAPE_HOR	1	// horizontal bar cursor
# define SHAPE_VER	2	// vertical bar cursor

# define MSHAPE_NUMBERED	1000	// offset for shapes identified by number
# define MSHAPE_HIDE	1	// hide mouse pointer

# define SHAPE_MOUSE	1	// used for mouse pointer shape
# define SHAPE_CURSOR	2	// used for text cursor shape

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ cursor_ನಮೂದು
{
    ಪೂರ್ಣಾಂಕ		shape;		// one of the SHAPE_ defines
    ಪೂರ್ಣಾಂಕ		mshape;		// one of the MSHAPE defines
    ಪೂರ್ಣಾಂಕ		percentage;	// percentage of cell for bar
    ದೀರ್ಘ	blinkwait;	// blinking, wait time before blinking starts
    ದೀರ್ಘ	blinkon;	// blinking, on time
    ದೀರ್ಘ	blinkoff;	// blinking, off time
    ಪೂರ್ಣಾಂಕ		id;		// highlight group ID
    ಪೂರ್ಣಾಂಕ		id_lm;		// highlight group ID for :lmap mode
    ಅಕ್ಷರ	*name;		// mode name (fixed)
    ಅಕ್ಷರ	ಬಳಸಿದ_for;	// SHAPE_MOUSE and/or SHAPE_CURSOR
} cursorentry_T;
#ಆದರೆ_ಅಂತ್ಯ // CURSOR_SHAPE

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU

// Indices into vimmenu_T->strings[] and vimmenu_T->noremap[] for each mode
# define MENU_ಸೂಚ್ಯಂಕ_INVALID	-1
# define MENU_ಸೂಚ್ಯಂಕ_ಸಾಮಾನ್ಯ	0
# define MENU_ಸೂಚ್ಯಂಕ_VISUAL	1
# define MENU_ಸೂಚ್ಯಂಕ_SELECT	2
# define MENU_ಸೂಚ್ಯಂಕ_OP_PENDING	3
# define MENU_ಸೂಚ್ಯಂಕ_ಸೇರಿಸು	4
# define MENU_ಸೂಚ್ಯಂಕ_CMDLINE	5
# define MENU_ಸೂಚ್ಯಂಕ_TERMINAL	6
# define MENU_ಸೂಚ್ಯಂಕ_TIP		7
# define MENU_MODES		8

// Menu modes
# define MENU_ಸಾಮಾನ್ಯ_ಬಹುಲಕ	(1 << MENU_ಸೂಚ್ಯಂಕ_ಸಾಮಾನ್ಯ)
# define MENU_VISUAL_ಬಹುಲಕ	(1 << MENU_ಸೂಚ್ಯಂಕ_VISUAL)
# define MENU_SELECT_ಬಹುಲಕ	(1 << MENU_ಸೂಚ್ಯಂಕ_SELECT)
# define MENU_OP_PENDING_ಬಹುಲಕ	(1 << MENU_ಸೂಚ್ಯಂಕ_OP_PENDING)
# define MENU_ಸೇರಿಸು_ಬಹುಲಕ	(1 << MENU_ಸೂಚ್ಯಂಕ_ಸೇರಿಸು)
# define MENU_CMDLINE_ಬಹುಲಕ	(1 << MENU_ಸೂಚ್ಯಂಕ_CMDLINE)
# define MENU_TERMINAL_ಬಹುಲಕ	(1 << MENU_ಸೂಚ್ಯಂಕ_TERMINAL)
# define MENU_TIP_ಬಹುಲಕ		(1 << MENU_ಸೂಚ್ಯಂಕ_TIP)
# define MENU_ALL_MODES		((1 << MENU_ಸೂಚ್ಯಂಕ_TIP) - 1)
// note MENU_INDEX_TIP is not a 'real' mode

// Start a menu name with this to not include it on the main menu bar
# define MNU_HIDDEN_CHAR		']'

ರಚನೆ VimMenu
{
    ಪೂರ್ಣಾಂಕ		modes;		    // Which modes is this menu visible for?
    ಪೂರ್ಣಾಂಕ		enabled;	    // for which modes the menu is enabled
    char_u	*name;		    // Name of menu, possibly translated
    char_u	*dname;		    // Displayed Name ("name" without '&')
# ifdef FEAT_MULTI_LANG
    char_u	*en_name;	    // "name" untranslated, NULL when "name"
				    // was not translated
    char_u	*en_dname;	    // "dname" untranslated, NULL when "dname"
				    // was not translated
# endif
    char_u	*actext;	    // accelerator text (after TAB)
    ಪೂರ್ಣಾಂಕ		mnemonic;	    // mnemonic key (after '&')
    ಪೂರ್ಣಾಂಕ		ಆದ್ಯತೆ;	    // Menu order priority
# ifdef FEAT_GUI
    ಶೂನ್ಯ	(*cb)(vimmenu_T *); // Call-back function
# endif
# ifdef FEAT_TOOLBAR
    char_u	*iconfile;	    // name of file for icon or NULL
    ಪೂರ್ಣಾಂಕ		iconidx;	    // icon index (-1 if not set)
    ಪೂರ್ಣಾಂಕ		icon_builtin;	    // icon names is BuiltIn{nr}
# endif
    char_u	*ಅಕ್ಷರಸರಣಿಗಳು[MENU_MODES]; // Mapped string for each mode
    ಪೂರ್ಣಾಂಕ		noremap[MENU_MODES]; // A REMAP_ flag for each mode
    ಅಕ್ಷರ	silent[MENU_MODES]; // A silent flag for each mode
    vimmenu_T	*children;	    // Children of sub-menu
    vimmenu_T	*ಪೋಷಕ;	    // Parent of menu
    vimmenu_T	*ಮುಂದಿನ;		    // Next item in menu
# ifdef FEAT_GUI_X11
    Widget	id;		    // Manage this to enable item
    Widget	submenu_id;	    // If this is submenu, add children here
# endif
# ifdef FEAT_GUI_GTK
    GtkWidget	*id;		    // Manage this to enable item
    GtkWidget	*submenu_id;	    // If this is submenu, add children here
#  ಆದರೆ defined(GTK_CHECK_ಆವೃತ್ತಿ) && !GTK_CHECK_ಆವೃತ್ತಿ(3,4,0)
    GtkWidget	*tearoff_handle;
#  endif
#  ifndef USE_GTK4
    GtkWidget   *label;		    // Used by "set wak=" code.
#  endif
# endif
# ifdef FEAT_GUI_MOTIF
    ಪೂರ್ಣಾಂಕ		sensitive;	    // turn button on/off
    ಅಕ್ಷರ	**xpm;		    // pixmap data
    ಅಕ್ಷರ	*xpm_fname;	    // file with pixmap data
# endif
# ifdef FEAT_BEVAL_TIP
    BalloonEval *tip;		    // tooltip for this menu item
# endif
# ifdef FEAT_GUI_MSWIN
    UINT	id;		    // Id of menu item
    HMENU	submenu_id;	    // If this is submenu, add children here
    HWND	tearoff_handle;	    // hWnd of tearoff if created
# endif
# ifdef FEAT_GUI_HAIKU
    BMenu_ಅಂಶ  *id;		    // Id of menu item
    BMenu  *submenu_id;		    // If this is submenu, add children here
#  ifdef FEAT_TOOLBAR
    BPictureButton *button;
#  endif
# endif
# ifdef FEAT_GUI_PHOTON
    PtWidget_t	*id;
    PtWidget_t	*submenu_id;
# endif
};
#ಇಲ್ಲದಿದ್ದರೆ
// For generating prototypes when FEAT_MENU isn't defined.
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ vimmenu_T;

#ಆದರೆ_ಅಂತ್ಯ // FEAT_MENU

/*
 * Struct to save values in before executing autocommands for a buffer that is
 * not the current buffer.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    ಪೂರ್ಣಾಂಕ		use_aucmd_win_idx;  // index in aucmd_win[] if >= 0
    ಪೂರ್ಣಾಂಕ		save_curwin_id;	    // ID of saved curwin
    ಪೂರ್ಣಾಂಕ		new_curwin_id;	    // ID of new curwin
    ಪೂರ್ಣಾಂಕ		save_prevwin_id;    // ID of saved prevwin
    bufref_T	new_curbuf;	    // new curbuf
    char_u	*tp_localdir;	    // saved value of tp_localdir
    char_u	*globaldir;	    // saved value of globaldir
    ಪೂರ್ಣಾಂಕ		save_VIsual_ಸಕ್ರಿಯ; // saved VIsual_active
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
    ಪೂರ್ಣಾಂಕ		save_prompt_ಸೇರಿಸು; // saved b_prompt_insert
#ಆದರೆ_ಅಂತ್ಯ
} aco_save_T;

/*
 * Generic option table item, only used for printer at the moment.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ	*name;
    ಪೂರ್ಣಾಂಕ		hasnum;
    ದೀರ್ಘ	ಸಂಖ್ಯೆ;
    char_u	*string;	// points into option string
    ಪೂರ್ಣಾಂಕ		ಅಕ್ಷರಸರಣಿ_ಉದ್ದ;
    ಪೂರ್ಣಾಂಕ		present;
} option_ಕೋಷ್ಟಕ_T;

/*
 * Structure to hold printing color and font attributes.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    long_u	fg_color;
    long_u	bg_color;
    ಪೂರ್ಣಾಂಕ		bold;
    ಪೂರ್ಣಾಂಕ		italic;
    ಪೂರ್ಣಾಂಕ		underline;
    ಪೂರ್ಣಾಂಕ		undercurl;
} prt_ಪಠ್ಯ_attr_T;

/*
 * Structure passed back to the generic printer code.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    ಪೂರ್ಣಾಂಕ		n_collated_copies;
    ಪೂರ್ಣಾಂಕ		n_uncollated_copies;
    ಪೂರ್ಣಾಂಕ		duplex;
    ಪೂರ್ಣಾಂಕ		chars_ಪ್ರತಿ_ಸಾಲು;
    ಪೂರ್ಣಾಂಕ		ಸಾಲುಗಳು_ಪ್ರತಿ_ಪುಟ;
    ಪೂರ್ಣಾಂಕ		has_color;
    prt_ಪಠ್ಯ_attr_T ಸಂಖ್ಯೆ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
    ಪೂರ್ಣಾಂಕ		modec;
    ಪೂರ್ಣಾಂಕ		do_syntax;
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		user_abort;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PRINT_PANGO
    ಪೂರ್ಣಾಂಕ		user_abort_ಸಂದೇಶ;
#ಆದರೆ_ಅಂತ್ಯ
    char_u	*jobname;
#ಆದರೆ defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
    char_u	*outfile;
    char_u	*arguments;
#ಆದರೆ_ಅಂತ್ಯ
} prt_settings_T;

#ವ್ಯಾಖ್ಯಾನಿಸು PRINT_ಸಂಖ್ಯೆ_ಅಗಲ 8

/*
 * Used for popup menu items.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    char_u	*pum_ಪಠ್ಯ;		// main menu text
    char_u	*pum_ಬಗೆ;		// extra kind text (may be truncated)
    char_u	*pum_extra;		// extra menu text (may be truncated)
    char_u	*pum_info;		// extra info
    ಪೂರ್ಣಾಂಕ		pum_cpt_ಮೂಲ_idx;	// index of completion source in 'cpt'
    ಪೂರ್ಣಾಂಕ		pum_user_abbr_hlattr;	// highlight attribute for abbr
    ಪೂರ್ಣಾಂಕ		pum_user_ಬಗೆ_hlattr;	// highlight attribute for kind
} pumitem_T;

/*
 * Structure used for get_tagfname().
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    char_u	*tn_ಟ್ಯಾಗುಗಳು;	// value of 'tags' when starting
    char_u	*tn_np;		// current position in tn_tags
    ಪೂರ್ಣಾಂಕ		tn_did_filefind_init;
    ಪೂರ್ಣಾಂಕ		tn_hf_idx;
    ಶೂನ್ಯ	*tn_ಹುಡುಕು_ctx;
} tagname_T;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    UINT32_T ಒಟ್ಟು[2];
    UINT32_T state[8];
    char_u   ಬಫರ್[64];
} context_sha256_T;

/*
 * types for expressions.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ
{
    ಅಭಿವ್ಯಕ್ತಿ_UNKNOWN = 0,
    ಅಭಿವ್ಯಕ್ತಿ_EQUAL,		// ==
    ಅಭಿವ್ಯಕ್ತಿ_NEQUAL,	// !=
    ಅಭಿವ್ಯಕ್ತಿ_GREATER,	// >
    ಅಭಿವ್ಯಕ್ತಿ_GEQUAL,	// >=
    ಅಭಿವ್ಯಕ್ತಿ_SMALLER,	// <
    ಅಭಿವ್ಯಕ್ತಿ_SEQUAL,	// <=
    ಅಭಿವ್ಯಕ್ತಿ_ಹೊಂದಿಕೆ,		// =~
    ಅಭಿವ್ಯಕ್ತಿ_NOMATCH,	// !~
    ಅಭಿವ್ಯಕ್ತಿ_IS,		// is
    ಅಭಿವ್ಯಕ್ತಿ_ISNOT,		// isnot
    // used with ISN_OPNR
    ಅಭಿವ್ಯಕ್ತಿ_ಕೂಡಿಸು,		// +
    ಅಭಿವ್ಯಕ್ತಿ_SUB,		// -
    ಅಭಿವ್ಯಕ್ತಿ_MULT,		// *
    ಅಭಿವ್ಯಕ್ತಿ_DIV,		// /
    ಅಭಿವ್ಯಕ್ತಿ_REM,		// %
    ಅಭಿವ್ಯಕ್ತಿ_LSHIFT,	// <<
    ಅಭಿವ್ಯಕ್ತಿ_RSHIFT,	// >>
    // used with ISN_ADDLIST
    ಅಭಿವ್ಯಕ್ತಿ_COPY,		// create new list
    ಅಭಿವ್ಯಕ್ತಿ_APPEND,	// append to first list
} exprtype_T;

/*
 * Structure used for reading in json_decode().
 */
ರಚನೆ js_reader
{
    char_u	*js_buf;	// text to be decoded
    char_u	*js_ಕೊನೆ;	// NUL in js_buf
    ಪೂರ್ಣಾಂಕ		js_ಬಳಸಿದ;	// bytes used from js_buf
    ಪೂರ್ಣಾಂಕ		(*js_fill)(ರಚನೆ js_reader *);
				// function to fill the buffer or NULL;
				// return TRUE when the buffer was filled
    ಶೂನ್ಯ	*js_cookie;	// can be used by js_fill
    ಪೂರ್ಣಾಂಕ		js_cookie_arg;	// can be used by js_fill
};
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ js_reader js_read_T;

// Maximum number of commands from + or -c arguments.
#ವ್ಯಾಖ್ಯಾನಿಸು MAX_ARG_CMDS 10

// values for "window_layout"
#ವ್ಯಾಖ್ಯಾನಿಸು WIN_HOR	    1	    // "-o" horizontally split windows
#ವ್ಯಾಖ್ಯಾನಿಸು	WIN_VER	    2	    // "-O" vertically split windows
#ವ್ಯಾಖ್ಯಾನಿಸು	WIN_TABS    3	    // "-p" windows on tab pages

// Struct for various parameters passed between main() and other functions.
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    ಪೂರ್ಣಾಂಕ		argc;
    ಅಕ್ಷರ	**argv;

    char_u	*fname;			// first file to edit

    ಪೂರ್ಣಾಂಕ		evim_ಬಹುಲಕ;		// started as "evim"
    char_u	*use_vimrc;		// vimrc from -u argument
    ಪೂರ್ಣಾಂಕ		clean;			// --clean argument

    ಪೂರ್ಣಾಂಕ		n_commands;		     // no. of commands from + or -c
    char_u	*commands[MAX_ARG_CMDS];     // commands from + or -c arg.
    char_u	cmds_tofree[MAX_ARG_CMDS];   // commands that need free()
    ಪೂರ್ಣಾಂಕ		n_pre_commands;		     // no. of commands from --cmd
    char_u	*pre_commands[MAX_ARG_CMDS]; // commands from --cmd argument

    ಪೂರ್ಣಾಂಕ		edit_type;		// type of editing to do
    char_u	*tagname;		// tag from -t argument
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
    char_u	*use_ef;		// 'errorfile' from -q argument
#ಆದರೆ_ಅಂತ್ಯ

    ಪೂರ್ಣಾಂಕ		want_ತುಂಬಿದ_screen;
    ಪೂರ್ಣಾಂಕ		not_a_ಪದ;		// no warning for missing term?
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    char_u	*gui_dialog_file;	// file to write dialog text in
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		tty_fail;		// exit if not a tty
    char_u	*ಪದ;			// specified terminal name
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CRYPT
    ಪೂರ್ಣಾಂಕ		ask_for_ಕೀಲಿ;		// -x argument
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		no_ಅದಲುಬದಲು_file;		// "-n" argument used
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಪೂರ್ಣಾಂಕ		use_debug_break_level;
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		ಕಿಟಕಿ_ಎಣಿಕೆ;		// number of windows to use
    ಪೂರ್ಣಾಂಕ		ಕಿಟಕಿ_layout;		// 0, WIN_HOR, WIN_VER or WIN_TABS

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIENTSERVER
    ಪೂರ್ಣಾಂಕ		serverArg;		// TRUE when argument for a server
    char_u	*serverName_arg;	// cmdline arg for server name
    char_u	*serverStr;		// remote server command
    char_u	*serverStrEnc;		// encoding of serverStr
    char_u	*servername;		// allocated name for our server
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ !defined(UNIX)
# define EXPAND_FILENAMES
    ಪೂರ್ಣಾಂಕ		literal;		// don't expand file names
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
    ಪೂರ್ಣಾಂಕ		ತುಂಬಿದ_ಮಾರ್ಗ;		// file name argument was full path
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
    ಪೂರ್ಣಾಂಕ		diff_ಬಹುಲಕ;		// start with 'diff' set
#ಆದರೆ_ಅಂತ್ಯ
} mparm_T;

/*
 * Structure returned by get_lval() and used by set_var_lval().
 * For a plain name:
 *	"name"	    points to the variable name.
 *	"exp_name"  is NULL.
 *	"tv"	    is NULL
 * For a magic braces name:
 *	"name"	    points to the expanded variable name.
 *	"exp_name"  is non-NULL, to be freed later.
 *	"tv"	    is NULL
 * For an index in a list:
 *	"name"	    points to the (expanded) variable name.
 *	"exp_name"  NULL or non-NULL, to be freed later.
 *	"tv"	    points to the (first) list item value
 *	"li"	    points to the (first) list item
 *	"range", "n1", "n2" and "empty2" indicate what items are used.
 * For a plain class or object:
 *	"name"	    points to the variable name.
 *	"exp_name"  is NULL.
 *	"tv"	    points to the variable
 *	"is_root"   TRUE
 * For a variable in a class/object: (class is not NULL)
 *	"name"	    points to the (expanded) variable name.
 *	"exp_name"  NULL or non-NULL, to be freed later.
 *	"tv"	    May point to class/object variable.
 *	"object"    object containing variable, NULL if class variable
 *	"class"	    class of object or class containing variable
 *	"oi"	    index into class/object of tv
 * For an existing Dict item:
 *	"name"	    points to the (expanded) variable name.
 *	"exp_name"  NULL or non-NULL, to be freed later.
 *	"tv"	    points to the dict item value
 *	"newkey"    is NULL
 * For a non-existing Dict item:
 *	"name"	    points to the (expanded) variable name.
 *	"exp_name"  NULL or non-NULL, to be freed later.
 *	"tv"	    points to the Dictionary typval_T
 *	"newkey"    is the key for the new item.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ lval_S
{
    char_u	*ll_name;	// start of variable name (can be NULL)
    char_u	*ll_name_ಕೊನೆ;	// end of variable name (can be NULL)
    type_T	*ll_type;	// type of variable (can be NULL)
    char_u	*ll_exp_name;	// NULL or expanded name in allocated memory.

    scid_T	ll_sid;		// for an imported item: the script ID it was
				// imported from; zero otherwise

    typval_T	*ll_tv;		// Typeval of item being used.  If "newkey"
				// isn't NULL it's the Dict to which to add
				// the item.
    listitem_T	*ll_li;		// The list item or NULL.
    list_T	*ll_list;	// The list or NULL.
    ಪೂರ್ಣಾಂಕ		ll_range;	// TRUE when a [i:j] range was used
    ಪೂರ್ಣಾಂಕ		ll_empty2;	// Second index is empty: [i:]
    ದೀರ್ಘ	ll_n1;		// First index for list
    ದೀರ್ಘ	ll_n2;		// Second index for list range
    dict_T	*ll_dict;	// The Dictionary or NULL
    dictitem_T	*ll_di;		// The dictitem or NULL
    char_u	*ll_newkey;	// New key for Dict in alloc. mem or NULL.
    type_T	*ll_valtype;	// type expected for the value or NULL
    blob_T	*ll_blob;	// The Blob or NULL
    tuple_T	*ll_tuple;	// tuple or NULL
    ufunc_T	*ll_ufunc;	// The function or NULL
    object_T	*ll_object;	// The object or NULL, class is not NULL
    class_T	*ll_class;	// The class or NULL, object may be NULL
    ಪೂರ್ಣಾಂಕ		ll_oi;		// The object/class member index
    ಪೂರ್ಣಾಂಕ		ll_is_ಬೇರು;	// TRUE if ll_tv is the lval_root, like a
				// plain object/class. ll_tv is variable.
    garray_T	ll_type_list;   // list of pointers to allocated types
} lval_T;

/**
 * This specifies optional parameters for get_lval(). Arguments may be NULL.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ lval_ಬೇರು_S {
    typval_T	*lr_tv;		// Base typval.
    class_T	*lr_cl_exec;	// Executing class for access checking.
    ಪೂರ್ಣಾಂಕ		lr_is_arg;	// name is an arg (not a member).
} lval_ಬೇರು_T;

// Structure used to save the current state.  Used when executing Normal mode
// commands while in any other mode.
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಪೂರ್ಣಾಂಕ		save_ಸಂದೇಶ_scroll;
    ಪೂರ್ಣಾಂಕ		save_restart_edit;
    ಪೂರ್ಣಾಂಕ		save_ಸಂದೇಶ_didout;
    ಪೂರ್ಣಾಂಕ		save_State;
    ಪೂರ್ಣಾಂಕ		save_insertmode;
    ಪೂರ್ಣಾಂಕ		save_ಮುಕ್ತಾಯ_op;
    ಪೂರ್ಣಾಂಕ		save_opcount;
    ಪೂರ್ಣಾಂಕ		save_reg_executing;
    ಪೂರ್ಣಾಂಕ		save_pending_ಕೊನೆ_reg_executing;
    ಪೂರ್ಣಾಂಕ		save_script_ಆವೃತ್ತಿ;
    tasave_T	tabuf;
} save_state_T;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    varnumber_T vv_prevcount;
    varnumber_T vv_ಎಣಿಕೆ;
    varnumber_T vv_count1;
} vimvars_save_T;

// Scope for changing directory
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
    CDSCOPE_GLOBAL,	// :cd
    CDSCOPE_TABPAGE,	// :tcd
    CDSCOPE_ಕಿಟಕಿ	// :lcd
} cdscope_T;

// Variable flavor
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ
{
    VAR_FLAVOUR_DEFAULT,	// doesn't start with uppercase
    VAR_FLAVOUR_SESSION,	// starts with uppercase, some lower
    VAR_FLAVOUR_VIMINFO		// all uppercase
} var_flavour_T;

// argument for mouse_find_win()
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
    IGNORE_POPUP,	// only check non-popup windows
    ಕಂಡುಹಿಡಿ_POPUP,		// also find popup windows
    FAIL_POPUP		// return NULL if mouse on popup window
} mouse_ಕಂಡುಹಿಡಿ_T;

// Symbolic names for some registers.
#ವ್ಯಾಖ್ಯಾನಿಸು DELETION_ರಿಜಿಸ್ಟರ್	36
#ಆದರೆ defined(FEAT_CLIPBOARD) || defined(HAVE_CLIPMETHOD)
# define ನಕ್ಷತ್ರ_ರಿಜಿಸ್ಟರ್		37
# ಆದರೆ defined(FEAT_X11) || defined(FEAT_WAYLAND)
#  define PLUS_ರಿಜಿಸ್ಟರ್	38
#  define REAL_PLUS_ರಿಜಿಸ್ಟರ್	PLUS_ರಿಜಿಸ್ಟರ್
# ಇಲ್ಲದಿದ್ದರೆ
#  define PLUS_ರಿಜಿಸ್ಟರ್	ನಕ್ಷತ್ರ_ರಿಜಿಸ್ಟರ್	    // there is only one
#  ifdef FEAT_EVAL
// Make it so that if clipmethod is "none", the plus register is not available,
// but if clipmethod is a provider, then expose the plus register for use.
#   define REAL_PLUS_ರಿಜಿಸ್ಟರ್	38
#  ಇಲ್ಲದಿದ್ದರೆ
#   define REAL_PLUS_ರಿಜಿಸ್ಟರ್	ನಕ್ಷತ್ರ_ರಿಜಿಸ್ಟರ್
#  endif
# endif
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DND
# define TILDE_ರಿಜಿಸ್ಟರ್		(PLUS_ರಿಜಿಸ್ಟರ್ + 1)
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIPBOARD
# ifdef FEAT_DND
#  define ಅಂಕೆ_REGISTERS		(TILDE_ರಿಜಿಸ್ಟರ್ + 1)
# ಇಲ್ಲದಿದ್ದರೆ
#  define ಅಂಕೆ_REGISTERS		(REAL_PLUS_ರಿಜಿಸ್ಟರ್ + 1)
# endif
#ಇಲ್ಲದಿದ್ದರೆ
# ifdef HAVE_CLIPMETHOD
#  define ಅಂಕೆ_REGISTERS		(REAL_PLUS_ರಿಜಿಸ್ಟರ್ + 1)
# ಇಲ್ಲದಿದ್ದರೆ
#  define ಅಂಕೆ_REGISTERS		37
# endif
#ಆದರೆ_ಅಂತ್ಯ

// structure used by block_prep, op_delete and op_yank for blockwise operators
// also op_change, op_shift, op_insert, op_replace - AKelly
ರಚನೆ ಖಂಡ_def
{
    ಪೂರ್ಣಾಂಕ		startspaces;	// 'extra' cols before first char
    ಪೂರ್ಣಾಂಕ		endspaces;	// 'extra' cols after last char
    ಪೂರ್ಣಾಂಕ		textlen;	// chars in block
    char_u	*textstart;	// pointer to 1st char (partially) in block
    colnr_T	textcol;	// index of chars (partially) in block
    colnr_T	ಆರಂಭ_vcol;	// start col of 1st char wholly inside block
    colnr_T	ಕೊನೆ_vcol;	// start col of 1st char wholly after block
    ಪೂರ್ಣಾಂಕ		is_short;	// TRUE if line is too short to fit in block
    ಪೂರ್ಣಾಂಕ		is_MAX;		// TRUE if curswant==MAXCOL when starting
    ಪೂರ್ಣಾಂಕ		is_oneChar;	// TRUE if block within one character
    ಪೂರ್ಣಾಂಕ		pre_whitesp;	// screen cols of ws before block
    ಪೂರ್ಣಾಂಕ		pre_whitesp_c;	// chars of ws before block
    colnr_T	ಕೊನೆ_char_vcols;	// number of vcols of post-block char
    colnr_T	ಆರಂಭ_char_vcols; // number of vcols of pre-block char
};

// Each yank register has an array of pointers to lines.
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    string_T	*y_array;	// pointer to array of string_T structs
    linenr_T	y_ಗಾತ್ರ;		// number of lines in y_array
    char_u	y_type;		// MLINE, MCHAR or MBLOCK
    colnr_T	y_ಅಗಲ;	// only set if y_type == MBLOCK
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VIMINFO
    time_t	y_ಸಮಯ_ಗಣ;
#ಆದರೆ_ಅಂತ್ಯ
} yankreg_T;

// The offset for a search command is store in a soff struct
// Note: only spats[0].off is really used
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ soffset
{
    ಪೂರ್ಣಾಂಕ		dir;		// search direction, '/' or '?'
    ಪೂರ್ಣಾಂಕ		ಸಾಲು;		// search has line offset
    ಪೂರ್ಣಾಂಕ		ಕೊನೆ;		// search set cursor at end
    ದೀರ್ಘ	off;		// line or char offset
} soffset_T;

// A search pattern and its attributes are stored in a spat struct
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ spat
{
    char_u	    *pat;	// the pattern (in allocated memory) or NULL
    size_t	    patlen;	// the length of the pattern (0 if pat is NULL)
    ಪೂರ್ಣಾಂಕ		    magic;	// magicness of the pattern
    ಪೂರ್ಣಾಂಕ		    no_scs;	// no smartcase for this pattern
    soffset_T	    off;
} spat_T;

/*
 * Optional extra arguments for searchit().
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    linenr_T	sa_stop_lnum;	// stop after this line number when != 0
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RELTIME
    ದೀರ್ಘ	sa_tm;		// timeout limit or zero
    ಪೂರ್ಣಾಂಕ		sa_timed_ಹೊರಹೋಗು;	// set when timed out
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		sa_wrapped;	// search wrapped around
} searchit_arg_T;

/*
 * Cookie used by getsourceline().
 */
/*
 * Cookie used to store info for each sourced file.
 * It is shared between do_source() and getsourceline().
 * This is passed to do_cmdline().
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    FILE	*fp;		// opened file for sourcing
    char_u	*nextline;	// if not NULL: line that was read ahead
    linenr_T	sourcing_lnum;	// line number of the source file
    ಪೂರ್ಣಾಂಕ		ಪೂರ್ಣಗೊಂಡ;	// ":finish" used
    ಪೂರ್ಣಾಂಕ		ಮೂಲ_from_buf;// TRUE if sourcing from current buffer
    ಪೂರ್ಣಾಂಕ		buf_lnum;	// line number in the current buffer
    garray_T	buflines;	// lines in the current buffer
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_CRNL
    ಪೂರ್ಣಾಂಕ		fileformat;	// EOL_UNKNOWN, EOL_UNIX or EOL_DOS
    ಪೂರ್ಣಾಂಕ		error;		// TRUE if LF found after CR-LF
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    linenr_T	breakpoint;	// next line with breakpoint or zero
    char_u	*fname;		// name of sourced file
    ಪೂರ್ಣಾಂಕ		dbg_tick;	// debug_tick when breakpoint was set
    ಪೂರ್ಣಾಂಕ		level;		// top nesting level of sourced file
#ಆದರೆ_ಅಂತ್ಯ
    vimconv_T	conv;		// type of conversion
} ಮೂಲ_cookie_T;


#ವ್ಯಾಖ್ಯಾನಿಸು WRITEBUFSIZE	8192	// size of normal write buffer

#ವ್ಯಾಖ್ಯಾನಿಸು FIO_LATIN1	0x01	// convert Latin1
#ವ್ಯಾಖ್ಯಾನಿಸು FIO_UTF8	0x02	// convert UTF-8
#ವ್ಯಾಖ್ಯಾನಿಸು FIO_UCS2	0x04	// convert UCS-2
#ವ್ಯಾಖ್ಯಾನಿಸು FIO_UCS4	0x08	// convert UCS-4
#ವ್ಯಾಖ್ಯಾನಿಸು FIO_UTF16	0x10	// convert UTF-16
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
# define FIO_CODEPAGE	0x20	// convert MS-Windows codepage
# define FIO_PUT_CP(x) (((x) & 0xffff) << 16)	// put codepage in top word
# define FIO_GET_CP(x)	(((x)>>16) & 0xffff)	// get codepage from top word
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MACOS_CONVERT
# define FIO_MACROMAN	0x20	// convert MacRoman
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿಸು FIO_ENDIAN_L	0x80	// little endian
#ವ್ಯಾಖ್ಯಾನಿಸು FIO_ENCRYPTED	0x1000	// encrypt written bytes
#ವ್ಯಾಖ್ಯಾನಿಸು FIO_NOCONVERT	0x2000	// skip encoding conversion
#ವ್ಯಾಖ್ಯಾನಿಸು FIO_UCSBOM	0x4000	// check for BOM at start of file
#ವ್ಯಾಖ್ಯಾನಿಸು FIO_ALL	(-1)	// allow all formats

// When converting, a read() or write() may leave some bytes to be converted
// for the next call.  The value is guessed...
#ವ್ಯಾಖ್ಯಾನಿಸು CONV_RESTLEN 30

// We have to guess how much a sequence of bytes may expand when converting
// with iconv() to be able to allocate a buffer.
#ವ್ಯಾಖ್ಯಾನಿಸು ICONV_MULT 8

// Used for "magic_overruled".
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
    OPTION_MAGIC_NOT_ಗಣ,	// p_magic not overruled
    OPTION_MAGIC_ON,		// magic on inside regexp
    OPTION_MAGIC_OFF		// magic off inside regexp
} optmagic_T;

// Magicness of a pattern, used by regexp code.
// The order and values matter:
//  magic <= MAGIC_OFF includes MAGIC_NONE
//  magic >= MAGIC_ON  includes MAGIC_ALL
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
    MAGIC_NONE = 1,		// "\V" very unmagic
    MAGIC_OFF = 2,		// "\M" or 'magic' off
    MAGIC_ON = 3,		// "\m" or 'magic'
    MAGIC_ALL = 4		// "\v" very magic
} magic_T;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
    WT_UNKNOWN = 0,	// Unknown or unspecified location
    WT_ವಾದ,
    WT_VARIABLE,
    WT_MEMBER,
    WT_ವಿಧಾನ,		// object method
    WT_ವಿಧಾನ_ARG,	// object method argument type
    WT_ವಿಧಾನ_RETURN,	// object method return type
    WT_CAST,		// type cast
} wherekind_T;

// Struct used to pass the location of a type check.  Used in error messages to
// indicate where the error happened.  Also used for doing covariance type
// check for object method return type and contra-variance type check for
// object method arguments.
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಅಕ್ಷರ	*wt_func_name;  // function name or NULL
    ಅಕ್ಷರ	wt_ಸೂಚ್ಯಂಕ;	// argument or variable index, 0 means unknown
    wherekind_T	wt_ಬಗೆ;	// type check location
} where_T;

#ವ್ಯಾಖ್ಯಾನಿಸು WHERE_INIT {ಖಾಲಿ, 0, WT_UNKNOWN}

// Struct passed to get_v_event() and restore_v_event().
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಪೂರ್ಣಾಂಕ		sve_did_save;
    hashtab_T	sve_hashtab;
} save_v_event_T;

// Enum used by filter(), map(), mapnew() and foreach()
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
    FILTERMAP_FILTER,
    FILTERMAP_MAP,
    FILTERMAP_MAPNEW,
    FILTERMAP_FOREACH
} filtermap_T;

// Structure used by switch_win() to pass values to restore_win()
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    win_T	*sw_curwin;
    tabpage_T	*sw_curtab;
    ಪೂರ್ಣಾಂಕ		sw_same_win;	    // VIsual_active was not reset
    ಪೂರ್ಣಾಂಕ		sw_visual_ಸಕ್ರಿಯ;
} switchwin_T;

// Fuzzy matched string list item. Used for fuzzy match completion. Items are
// usually sorted by 'score'. The 'idx' member is used for stable-sort.
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಪೂರ್ಣಾಂಕ		idx;
    char_u	*str;
    ಪೂರ್ಣಾಂಕ		score;
} fuzmatch_str_T;

// Argument for lbr_chartabsize().
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    win_T	*cts_win;
    char_u	*cts_ಸಾಲು;		// start of the line
    char_u	*cts_ptr;		// current position in line
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
    ಪೂರ್ಣಾಂಕ		cts_bri_ಗಾತ್ರ;		// cached size of 'breakindent', or -1
					// if not computed yet
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
    ಪೂರ್ಣಾಂಕ		cts_ಪಠ್ಯ_prop_ಎಣಿಕೆ;	// number of text props; when zero
					// cts_text_props is not used
    textprop_T	*cts_ಪಠ್ಯ_props;	// text props (allocated)
    ಅಕ್ಷರ	cts_has_prop_with_ಪಠ್ಯ;	// TRUE if a property inserts text
    ಪೂರ್ಣಾಂಕ		cts_cur_ಪಠ್ಯ_ಅಗಲ;	// width of current inserted text
    ಪೂರ್ಣಾಂಕ		cts_prop_ಸಾಲುಗಳು;		// nr of properties above or below
    bool	cts_has_ಕೆಳಗೆ;		// true if a text property below was
					// counted, its width fills up the line
    ಪೂರ್ಣಾಂಕ		cts_ಮೊದಲ_char;		// width text props above the line
    ಪೂರ್ಣಾಂಕ		cts_above_ಅಗಲ;	// width of text props above the line,
					// kept for the whole line
    bool	cts_no_above;		// do not count the width of text props
					// above the line
    ಪೂರ್ಣಾಂಕ		cts_with_trailing;	// include size of trailing props with
					// last character
    ಪೂರ್ಣಾಂಕ		cts_ಆರಂಭ_incl;		// prop has true "start_incl" arg
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		cts_vcol;		// virtual column at current position
    ಪೂರ್ಣಾಂಕ		cts_max_ತಲೆ_vcol;	// see win_lbr_chartabsize()
} chartabsize_T;

/*
 * Argument for the callback function (opt_did_set_cb_T) invoked after an
 * option value is modified.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    // Pointer to the option variable.  The variable can be a long (numeric
    // option), an int (boolean option) or a char pointer (string option).
    char_u	*os_varp;
    ಪೂರ್ಣಾಂಕ		os_idx;
    ಪೂರ್ಣಾಂಕ		os_flags;
    ಗಣ_op_T	os_op;

    // old value of the option (can be a string, number or a boolean)
    ಒಕ್ಕೂಟ
    {
	ದೀರ್ಘ	ಸಂಖ್ಯೆ;
	ಪೂರ್ಣಾಂಕ	boolean;
	char_u	*string;
    } os_oldval;

    // new value of the option (can be a string, number or a boolean)
    ಒಕ್ಕೂಟ
    {
	ದೀರ್ಘ	ಸಂಖ್ಯೆ;
	ಪೂರ್ಣಾಂಕ	boolean;
	char_u	*string;
    } os_newval;

    // Option value was checked to be safe, no need to set P_INSECURE
    // Used for the 'keymap', 'filetype' and 'syntax' options.
    ಪೂರ್ಣಾಂಕ		os_ಮೌಲ್ಯ_checked;
    // Option value changed.  Used for the 'filetype' and 'syntax' options.
    ಪೂರ್ಣಾಂಕ		os_ಮೌಲ್ಯ_ಬದಲಾಯಿತು;

    // Used by the 'isident', 'iskeyword', 'isprint' and 'isfname' options.
    // Set to TRUE if the character table is modified when processing the
    // option and need to be restored because of a failure.
    ಪೂರ್ಣಾಂಕ		os_restore_chartab;

#ಆದರೆ defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
    // Used by the 't_xxx' terminal options on MS-Windows.
    ಪೂರ್ಣಾಂಕ		os_did_swaptcap;
#ಆದರೆ_ಅಂತ್ಯ

    // If the value specified for an option is not valid and the error message
    // is parameterized, then the "os_errbuf" buffer is used to store the error
    // message (when it is not NULL).
    ಅಕ್ಷರ	*os_errbuf;
    // length of the error buffer
    size_t	os_errbuflen;
} optset_T;

/*
 * Argument for the callback function (opt_expand_cb_T) invoked after a string
 * option value is expanded for cmdline completion.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    // Pointer to the option variable. It's always a string.
    char_u	*oe_varp;
    // The original option value, escaped.
    char_u	*oe_opt_ಮೌಲ್ಯ;

    // TRUE if using set+= instead of set=
    ಪೂರ್ಣಾಂಕ		oe_append;
    // TRUE if we would like to add the original option value as the first
    // choice.
    ಪೂರ್ಣಾಂಕ		oe_include_orig_val;

    // Regex from the cmdline, for matching potential options against.
    regmatch_T	*oe_regmatch;
    // The expansion context.
    expand_T	*oe_xp;

    // The full argument passed to :set. For example, if the user inputs
    // ':set dip=icase,algorithm:my<Tab>', oe_xp->xp_pattern will only have
    // 'my', but oe_set_arg will contain the whole 'icase,algorithm:my'.
    char_u	*oe_ಗಣ_arg;
} optexpand_T;

/*
 * Spell checking variables passed from win_update() to win_line().
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಪೂರ್ಣಾಂಕ		spv_has_spell;	    // drawn window has spell checking
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SPELL
    ಪೂರ್ಣಾಂಕ		spv_unchanged;	    // not updating for changed text
    ಪೂರ್ಣಾಂಕ		spv_checked_ಲಂಬಸಾಲು;    // column in "checked_lnum" up to
				    // which there are no spell errors
    linenr_T	spv_checked_lnum;   // line number for "checked_col"
    ಪೂರ್ಣಾಂಕ		spv_cap_ಲಂಬಸಾಲು;	    // column to check for Cap word
    linenr_T	spv_capcol_lnum;    // line number for "cap_col"
#ಆದರೆ_ಅಂತ್ಯ
} spellvars_T;

// Return the length of a string literal.
// This macro only computes a string's length for a string-literal token; for
// anything else, including a char*, compilation will fail (note "" following
// s).
#ವ್ಯಾಖ್ಯಾನಿಸು STRLEN_LITERAL(s) (ಗಾತ್ರ(s "") - 1)

// Store a key/value (string) pair
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    ಪೂರ್ಣಾಂಕ	    ಕೀಲಿ;        // the key
    string_T ಮೌಲ್ಯ;	// the value
} keyvalue_T;

#ವ್ಯಾಖ್ಯಾನಿಸು KEYVALUE_ನಮೂದು(k, v) \
    {(k), {((char_u *)v), STRLEN_LITERAL(v)}}

#ಆದರೆ defined(UNIX) || defined(MSWIN) || defined(VMS) || defined(AMIGA)
// Defined as signed, to return -1 on error
ರಚನೆ cellsize {
    ಪೂರ್ಣಾಂಕ cs_xpixel;
    ಪೂರ್ಣಾಂಕ cs_ypixel;
};
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_WAYLAND

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ vwl_connection_S vwl_connection_T;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ vwl_seat_S vwl_seat_T;

# ifdef FEAT_WAYLAND_CLIPBOARD

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ vwl_ದತ್ತಾಂಶ_offer_S vwl_ದತ್ತಾಂಶ_offer_T;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ vwl_ದತ್ತಾಂಶ_ಮೂಲ_S vwl_ದತ್ತಾಂಶ_ಮೂಲ_T;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ vwl_ದತ್ತಾಂಶ_device_S vwl_ದತ್ತಾಂಶ_device_T;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ vwl_ದತ್ತಾಂಶ_device_manager_S vwl_ದತ್ತಾಂಶ_device_manager_T;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ vwl_ದತ್ತಾಂಶ_device_listener_S vwl_ದತ್ತಾಂಶ_device_listener_T;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ vwl_ದತ್ತಾಂಶ_ಮೂಲ_listener_S vwl_ದತ್ತಾಂಶ_ಮೂಲ_listener_T;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ vwl_ದತ್ತಾಂಶ_offer_listener_S vwl_ದತ್ತಾಂಶ_offer_listener_T;

// Wayland selections
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
    WAYLAND_SELECTION_NONE	= 0,
    WAYLAND_SELECTION_REGULAR	= 1 << 0,
    WAYLAND_SELECTION_PRIMARY	= 1 << 1,
} wayland_selection_T;

# endif
#ಆದರೆ_ಅಂತ್ಯ

// Used in term_set_sync_output()
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ
{
    ಪದ_SYNC_OUTPUT_ENABLE = 1 << 0,
    ಪದ_SYNC_OUTPUT_DISABLE = 1 << 1,
    ಪದ_SYNC_OUTPUT_OFF = 1 << 2,
    ಪದ_SYNC_OUTPUT_FLUSH = 1 << 3,
} ಪದ_sync_output_T;
