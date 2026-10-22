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
#ਜੇ defined(SASC) && SASC < 658
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਲੰਮਾ		linenr_T;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ		colnr_T;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ	short_u;
#ਜੇ_ਅੰਤ

// structure to store a string (including it's length)
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    char_u  *string;		// the string
    size_t  ਲੰਮਾਈ;		// length of the string (excluding the NUL)
} string_T;

/*
 * Position in file or buffer.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    linenr_T	lnum;	// line number
    colnr_T	ਖਾਨਾ;	// column number
    colnr_T	coladd; // extra virtual column
} pos_T;


/*
 * Same, but without coladd.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    linenr_T	lnum;	// line number
    colnr_T	ਖਾਨਾ;	// column number
} lpos_T;

/*
 * Structure used for growing arrays.
 * This is used to store information that only grows, is deleted all at
 * once, and needs to be accessed by index.  See ga_clear() and ga_grow().
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ growarray
{
    ਪੂਰਨ	    ga_len;		    // current number of items used
    ਪੂਰਨ	    ga_maxlen;		    // maximum number of items possible
    ਪੂਰਨ	    ga_itemsize;	    // sizeof(item)
    ਪੂਰਨ	    ga_growsize;	    // number of items to grow each time
    ਖਾਲੀ    *ga_ਅੰਕੜੇ;		    // pointer to the first item
} garray_T;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GA_ਖਾਲੀ_ਥਾਵਾਂ    {0, 0, 0, 0, ਨਿਰਾ}

// On rare systems "char" is unsigned, sometimes we really want a signed 8-bit
// value.
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਨਿਸ਼ਾਨੀਵਾਲਾ ਅੱਖਰ	int8_T;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਦੁੱਗਣਾ		float_T;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ typval_S		typval_T;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ listvar_S	list_T;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ dictvar_S	dict_T;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ partial_S	partial_T;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ blobvar_S	blob_T;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ tuplevar_S	tuple_T;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ generictype_S	generic_T;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ gfargs_tab_S	gfargs_tab_T;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ ਖਿੜਕੀ_S		win_T;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ wininfo_S	wininfo_T;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ ਚੌਖਟਾ_S		ਚੌਖਟਾ_T;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ			scid_T;		// script ID
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ file_ਬਫਰ	buf_T;		// forward declaration
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ terminal_S	ਪਦ_T;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MENU
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ VimMenu vimmenu_T;
#ਜੇ_ਅੰਤ

// maximum value for sc_version
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SCRIPT_ਸੰਸਕਰਣ_MAX 4
// value for sc_version in a Vim9 script file
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SCRIPT_ਸੰਸਕਰਣ_VIM9 999999

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
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    scid_T	sc_sid;		// script ID
    ਪੂਰਨ		sc_seq;		// sourcing sequence number
    linenr_T	sc_lnum;	// line number
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		sc_ਸੰਸਕਰਣ;	// :scriptversion
} sctx_T;

/*
 * Reference to a buffer that stores the value of buf_free_count.
 * bufref_valid() only needs to check "buf" when the count differs.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    buf_T   *br_buf;
    ਪੂਰਨ	    br_fnum;
    ਪੂਰਨ	    br_buf_free_ਗਿਣਤੀ;
} bufref_T;

/*
 * This is here because regexp.h needs pos_T and below regprog_T is used.
 */
#ਅਨ੍ਤਰ੍ਭਾਵ "regexp.h"

/*
 * This is here because gui.h needs the pos_T and win_T, and win_T needs gui.h
 * for scrollbar_T.
 */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
# include "gui.h"
#ਨਹੀਂਤਾਂ
# ifdef FEAT_XCLIPBOARD
#  include <X11/Intrinsic.h>
# endif
# define guicolor_T ਲੰਮਾ
# define INVALCOLOR ((guicolor_T)0x1ffffff)
    // only used for cterm.bg_rgb and cterm.fg_rgb: use cterm color
# define CTERMCOLOR ((guicolor_T)0x1fffffe)
#ਜੇ_ਅੰਤ
// We have include guard in the file
#ਅਨ੍ਤਰ੍ਭਾਵ "image.h"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ COLOR_INVALID(x) ((x) == INVALCOLOR || (x) == CTERMCOLOR)

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
# include "libvterm/include/vterm.h"
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    VTermColor	fg;
    VTermColor	bg;
} termcellcolor_T;
#ਜੇ_ਅੰਤ

/*
 * marks: positions in a file
 * (a normal mark is a lnum/col pair, the same as a file position)
 */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NMARKS		('z' - 'a' + 1)	// max. # of named marks
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXTRA_MARKS	10		// marks 0-9
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ JUMPLISTSIZE	100		// max. # of marks in jump list
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TAGSTACKSIZE	20		// max. # of tags in tag stack

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ filemark
{
    pos_T	mark;		// cursor position
    ਪੂਰਨ		fnum;		// file number
} fmark_T;

// Xtended file mark: also has a file name
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ xfilemark
{
    fmark_T	fmark;
    char_u	*fname;		// file name, used when fnum == 0
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VIMINFO
    ਸਮਾਂ_T	ਸਮਾਂ_ਸੈੱਟ;
#ਜੇ_ਅੰਤ
} xfmark_T;

/*
 * The taggy struct is used to store the information about a :tag command.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ taggy
{
    char_u	*tagname;	// tag name
    fmark_T	fmark;		// cursor position BEFORE ":tag"
    ਪੂਰਨ		cur_ਮੇਲ;	// match number
    ਪੂਰਨ		cur_fnum;	// buffer number used for cur_match
    char_u	*user_ਅੰਕੜੇ;	// used with tagfunc
} taggy_T;

/*
 * Structure that contains all options that are local to a window.
 * Used twice in a window: for the current buffer and for all buffers.
 * Also used in wininfo_T.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ARABIC
    ਪੂਰਨ		wo_arab;
# define w_p_arab w_onebuf_opt.wo_arab	// 'arabic'
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
    ਪੂਰਨ		wo_bri;
# define w_p_bri w_onebuf_opt.wo_bri	// 'breakindent'
    char_u	*wo_briopt;
# define w_p_briopt w_onebuf_opt.wo_briopt // 'breakindentopt'
#ਜੇ_ਅੰਤ
    char_u	*wo_wcr;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ w_p_wcr w_onebuf_opt.wo_wcr	// 'wincolor'
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
    ਪੂਰਨ		wo_diff;
# define w_p_diff w_onebuf_opt.wo_diff	// 'diff'
#ਜੇ_ਅੰਤ
    char_u	*wo_eiw;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ w_p_eiw w_onebuf_opt.wo_eiw	// 'eventignorewin'
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
    ਲੰਮਾ	wo_fdc;
# define w_p_fdc w_onebuf_opt.wo_fdc	// 'foldcolumn'
    ਪੂਰਨ		wo_fdc_save;
# define w_p_fdc_save w_onebuf_opt.wo_fdc_save	// 'foldenable' saved for diff mode
    ਪੂਰਨ		wo_fen;
# define w_p_fen w_onebuf_opt.wo_fen	// 'foldenable'
    ਪੂਰਨ		wo_fen_save;
# define w_p_fen_save w_onebuf_opt.wo_fen_save	// 'foldenable' saved for diff mode
    char_u	*wo_fdi;
# define w_p_fdi w_onebuf_opt.wo_fdi	// 'foldignore'
    ਲੰਮਾ	wo_fdl;
# define w_p_fdl w_onebuf_opt.wo_fdl	// 'foldlevel'
    ਪੂਰਨ		wo_fdl_save;
# define w_p_fdl_save w_onebuf_opt.wo_fdl_save	// 'foldlevel' state saved for diff mode
    char_u	*wo_fdm;
# define w_p_fdm w_onebuf_opt.wo_fdm	// 'foldmethod'
    char_u	*wo_fdm_save;
# define w_p_fdm_save w_onebuf_opt.wo_fdm_save	// 'fdm' saved for diff mode
    ਲੰਮਾ	wo_fml;
# define w_p_fml w_onebuf_opt.wo_fml	// 'foldminlines'
    ਲੰਮਾ	wo_fdn;
# define w_p_fdn w_onebuf_opt.wo_fdn	// 'foldnestmax'
# ifdef FEAT_EVAL
    char_u	*wo_fde;
#  define w_p_fde w_onebuf_opt.wo_fde	// 'foldexpr'
    char_u	*wo_fdt;
#  define w_p_fdt w_onebuf_opt.wo_fdt	// 'foldtext'
# endif
    char_u	*wo_fmr;
# define w_p_fmr w_onebuf_opt.wo_fmr	// 'foldmarker'
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
    ਪੂਰਨ		wo_lbr;
# define w_p_lbr w_onebuf_opt.wo_lbr	// 'linebreak'
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		wo_list;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ w_p_list w_onebuf_opt.wo_list	// 'list'
    char_u	*wo_lcs;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ w_p_lcs w_onebuf_opt.wo_lcs	// 'listchars'
    char_u	*wo_fcs;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ w_p_fcs w_onebuf_opt.wo_fcs	// 'fillchars'
    ਪੂਰਨ		wo_nu;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ w_p_nu w_onebuf_opt.wo_nu	// 'number'
    ਪੂਰਨ		wo_rnu;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ w_p_rnu w_onebuf_opt.wo_rnu	// 'relativenumber'
    char_u	*wo_ve;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ w_p_ve w_onebuf_opt.wo_ve	// 'virtualedit'
    ਬਿਨਾਨਿਸ਼ਾਨੀ	wo_ve_flags;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ	w_ve_flags w_onebuf_opt.wo_ve_flags	// flags for 'virtualedit'
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
    ਲੰਮਾ	wo_nuw;
# define w_p_nuw w_onebuf_opt.wo_nuw	// 'numberwidth'
#ਜੇ_ਅੰਤ
    ਪੂਰਨ wo_wfb;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ w_p_wfb w_onebuf_opt.wo_wfb	// 'winfixbuf'
    ਪੂਰਨ		wo_wfh;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ w_p_wfh w_onebuf_opt.wo_wfh	// 'winfixheight'
    ਪੂਰਨ		wo_wfw;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ w_p_wfw w_onebuf_opt.wo_wfw	// 'winfixwidth'
#ਜੇ defined(FEAT_QUICKFIX)
    ਪੂਰਨ		wo_pvw;
# define w_p_pvw w_onebuf_opt.wo_pvw	// 'previewwindow'
    ਲੰਮਾ        wo_lhi;
# define w_p_lhi w_onebuf_opt.wo_lhi    // 'lhistory'
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
    ਪੂਰਨ		wo_rl;
# define w_p_rl w_onebuf_opt.wo_rl	// 'rightleft'
    char_u	*wo_rlc;
# define w_p_rlc w_onebuf_opt.wo_rlc	// 'rightleftcmd'
#ਜੇ_ਅੰਤ
    ਲੰਮਾ	wo_scr;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ w_p_scr w_onebuf_opt.wo_scr	// 'scroll'
    ਪੂਰਨ		wo_sms;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ w_p_sms w_onebuf_opt.wo_sms	// 'smoothscroll'
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SPELL
    ਪੂਰਨ		wo_spell;
# define w_p_spell w_onebuf_opt.wo_spell // 'spell'
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_SYN_HL) || defined(FEAT_FOLDING) || defined(FEAT_DIFF)
    ਪੂਰਨ		wo_cuc;
# define w_p_cuc w_onebuf_opt.wo_cuc	// 'cursorcolumn'
    ਪੂਰਨ		wo_cul;
# define w_p_cul w_onebuf_opt.wo_cul	// 'cursorline'
    char_u	*wo_culopt;
# define w_p_culopt w_onebuf_opt.wo_culopt	// 'cursorlineopt'
    char_u	*wo_cc;
# define w_p_cc w_onebuf_opt.wo_cc	// 'colorcolumn'
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
    char_u	*wo_sbr;
# define w_p_sbr w_onebuf_opt.wo_sbr	// 'showbreak'
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_STL_OPT
    char_u	*wo_stl;
# define w_p_stl w_onebuf_opt.wo_stl	// 'statusline'
    char_u	*wo_stlo;
# define w_p_stlo w_onebuf_opt.wo_stlo	// 'statuslineopt'
    ਪੂਰਨ		wo_stlo_fh;
# define w_p_stlo_fh w_onebuf_opt.wo_stlo_fh // "fixedheight" of 'statuslineopt'
    ਪੂਰਨ		wo_stlo_mh;
# define w_p_stlo_mh w_onebuf_opt.wo_stlo_mh // "maxheight:" of 'statuslineopt'
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		wo_scb;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ w_p_scb w_onebuf_opt.wo_scb	// 'scrollbind'
    ਪੂਰਨ		wo_diff_saved; // options were saved for starting diff mode
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ w_p_diff_saved w_onebuf_opt.wo_diff_saved
    ਪੂਰਨ		wo_scb_save;	// 'scrollbind' saved for diff mode
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ w_p_scb_save w_onebuf_opt.wo_scb_save
    ਪੂਰਨ		wo_wrap;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ w_p_wrap w_onebuf_opt.wo_wrap	// 'wrap'
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
    ਪੂਰਨ		wo_wrap_save;	// 'wrap' state saved for diff mode
# define w_p_wrap_save w_onebuf_opt.wo_wrap_save
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CONCEAL
    char_u	*wo_cocu;		// 'concealcursor'
# define w_p_cocu w_onebuf_opt.wo_cocu
    ਲੰਮਾ	wo_cole;		// 'conceallevel'
# define w_p_cole w_onebuf_opt.wo_cole
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		wo_crb;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ w_p_crb w_onebuf_opt.wo_crb	// 'cursorbind'
    ਪੂਰਨ		wo_crb_save;	// 'cursorbind' state saved for diff mode
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ w_p_crb_save w_onebuf_opt.wo_crb_save
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SIGNS
    char_u	*wo_scl;
# define w_p_scl w_onebuf_opt.wo_scl	// 'signcolumn'
#ਜੇ_ਅੰਤ
    ਲੰਮਾ	wo_siso;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ w_p_siso w_onebuf_opt.wo_siso	// 'sidescrolloff' local value
    ਲੰਮਾ	wo_so;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ w_p_so w_onebuf_opt.wo_so	// 'scrolloff' local value
    ਲੰਮਾ	wo_sop;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ w_p_sop w_onebuf_opt.wo_sop  // 'scrolloffpad' local value
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
    char_u	*wo_twk;
# define w_p_twk w_onebuf_opt.wo_twk	// 'termwinkey'
    char_u	*wo_tws;
# define w_p_tws w_onebuf_opt.wo_tws	// 'termwinsize'
#ਜੇ_ਅੰਤ

    // A few options have local flags for P_INSECURE.
    long_u	wo_wrap_flags;		// flags for 'wrap'
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ w_p_wrap_flags w_onebuf_opt.wo_wrap_flags
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_STL_OPT
    long_u	wo_stl_flags;		// flags for 'statusline'
# define w_p_stl_flags w_onebuf_opt.wo_stl_flags
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    long_u	wo_fde_flags;		// flags for 'foldexpr'
# define w_p_fde_flags w_onebuf_opt.wo_fde_flags
    long_u	wo_fdt_flags;		// flags for 'foldtext'
# define w_p_fdt_flags w_onebuf_opt.wo_fdt_flags
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    sctx_T	wo_script_ctx[WV_ਗਿਣਤੀ];	// SCTXs for window-local options
# define w_p_script_ctx w_onebuf_opt.wo_script_ctx
#ਜੇ_ਅੰਤ
    char_u	*wo_whl;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ w_p_whl w_onebuf_opt.wo_whl	// 'winhighlight'
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
ਬਣਤਰ wininfo_S
{
    wininfo_T	*wi_ਅਗਲਾ;	// next entry or NULL for last entry
    wininfo_T	*wi_prev;	// previous entry or NULL for first entry
    win_T	*wi_win;	// pointer to window that did set wi_fpos
    pos_T	wi_fpos;	// last cursor position in the file
    winopt_T	wi_opt;		// local window options
    ਪੂਰਨ		wi_optset;	// TRUE when wi_opt has useful values
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
    ਪੂਰਨ		wi_fold_manual;	// copy of w_fold_manual
    garray_T	wi_folds;	// clone of w_folds
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		wi_changelistidx; // copy of w_changelistidx
};

/*
 * Info used to pass info about a fold from the fold-detection code to the
 * code that displays the foldcolumn.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ foldinfo
{
    ਪੂਰਨ		fi_level;	// level of the fold; when this is zero the
				// other fields are invalid
    ਪੂਰਨ		fi_lnum;	// line number where fold starts
    ਪੂਰਨ		fi_ਨੀਵਾਂ_level;	// lowest fold level that starts in the same
				//ਸਤਰ
} foldinfo_T;

/*
 * Structure to store info about the Visual area.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    pos_T	vi_ਸ਼ੁਰੂ;	// start pos of last VIsual
    pos_T	vi_ਸਿਰਾ;		// end position of last VIsual
    ਪੂਰਨ		vi_ਬਹੁਲਕ;	// VIsual_mode of last VIsual
    colnr_T	vi_curswant;	// MAXCOL from w_curswant
} visualinfo_T;

/*
 * structures used for undo
 */

// One line saved for undo.  After the NUL terminated text there might be text
// properties, thus ul_len can be larger than STRLEN(ul_line) + 1.
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    char_u	*ul_ਸਤਰ;	// text of the line
    ਲੰਮਾ	ul_len;		// length of the line including NUL, plus text
				// properties
    colnr_T	ul_textlen;	// length of the line excluding NUL and any text
				// properties
} undoline_T;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ u_ਇੰਦਰਾਜ u_ਇੰਦਰਾਜ_T;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ u_ਸਿਰਲੇਖ u_ਸਿਰਲੇਖ_T;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
// Which line around an undo block a cleared continuation flag was on.
# define UNDOPROP_NONE	0
# define UNDOPROP_ABOVE	1
# define UNDOPROP_ਹੇਠਾਂ	2

// A continuation flag cleared on a line around an undo block, given back when
// the block is undone or redone the other way.
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    ਪੂਰਨ		ਉੱਪਰ_which;	// UNDOPROP_ABOVE or UNDOPROP_BELOW
    ਪੂਰਨ		ਉੱਪਰ_type;	// tp_type of the property
    ਪੂਰਨ		ਉੱਪਰ_id;		// tp_id of the property
    colnr_T	ਉੱਪਰ_ਖਾਨਾ;		// tp_col of the property
    ਪੂਰਨ		ਉੱਪਰ_flag;	// TP_FLAG_CONT_NEXT or TP_FLAG_CONT_PREV
} undoprop_T;
#ਜੇ_ਅੰਤ

ਬਣਤਰ u_ਇੰਦਰਾਜ
{
    u_ਇੰਦਰਾਜ_T	*ue_ਅਗਲਾ;	// pointer to next entry in list
    linenr_T	ue_ਸਿਖਰ;		// number of line above undo block
    linenr_T	ue_bot;		// number of line below undo block
    linenr_T	ue_lcount;	// linecount when u_save called
    undoline_T	*ue_array;	// array of lines in undo block
    ਲੰਮਾ	ue_ਮਾਪ;	// number of lines in ue_array
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    garray_T	ue_props;	// undoprop_T: flags cleared around the block
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ U_DEBUG
    ਪੂਰਨ		ue_magic;	// magic number to check allocation
#ਜੇ_ਅੰਤ
};

ਬਣਤਰ u_ਸਿਰਲੇਖ
{
    // The following have a pointer and a number. The number is used when
    // reading the undo file in u_read_undo()
    ਸੰਘ {
	u_ਸਿਰਲੇਖ_T *ptr;	// pointer to next undo header in list
	ਲੰਮਾ	   seq;
    } uh_ਅਗਲਾ;
    ਸੰਘ {
	u_ਸਿਰਲੇਖ_T *ptr;	// pointer to previous header in list
	ਲੰਮਾ	   seq;
    } uh_prev;
    ਸੰਘ {
	u_ਸਿਰਲੇਖ_T *ptr;	// pointer to next header for alt. redo
	ਲੰਮਾ	   seq;
    } uh_alt_ਅਗਲਾ;
    ਸੰਘ {
	u_ਸਿਰਲੇਖ_T *ptr;	// pointer to previous header for alt. redo
	ਲੰਮਾ	   seq;
    } uh_alt_prev;
    ਲੰਮਾ	uh_seq;		// sequence number, higher == newer undo
    ਪੂਰਨ		uh_walk;	// used by undo_time()
    u_ਇੰਦਰਾਜ_T	*uh_ਇੰਦਰਾਜ;	// pointer to first entry
    u_ਇੰਦਰਾਜ_T	*uh_getbot_ਇੰਦਰਾਜ; // pointer to where ue_bot must be set
    pos_T	uh_cursor;	// cursor position before saving
    ਲੰਮਾ	uh_cursor_vcol;
    ਪੂਰਨ		uh_flags;	// see below
    pos_T	uh_namedm[NMARKS];	// marks before undo/after redo
    visualinfo_T uh_visual;	// Visual areas before undo/after redo
    ਸਮਾਂ_T	uh_ਸਮਾਂ;	// timestamp when the change was made
    ਲੰਮਾ	uh_save_nr;	// set when the file was saved after the
				// changes in this block
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ U_DEBUG
    ਪੂਰਨ		uh_magic;	// magic number to check allocation
#ਜੇ_ਅੰਤ
};

// values for uh_flags
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UH_ਬਦਲਿਆ  0x01	// b_changed flag before undo/after redo
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UH_EMPTYBUF 0x02	// buffer was empty

/*
 * structures used in undo.c
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ALIGN_LONG	// longword alignment and use filler byte
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ALIGN_ਮਾਪ (ਆਕਾਰ(ਲੰਮਾ))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ALIGN_ਨਕਾਬ (ALIGN_ਮਾਪ - 1)

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ m_info minfo_T;

/*
 * structure used to link chunks in one of the free chunk lists.
 */
ਬਣਤਰ m_info
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ALIGN_LONG
    long_u	m_ਮਾਪ_2;		// size of the chunk (including m_info)
#ਨਹੀਂਤਾਂ
    short_u	m_ਮਾਪ_2;		// size of the chunk (including m_info)
#ਜੇ_ਅੰਤ
    minfo_T	*m_ਅਗਲਾ;	// pointer to next free chunk in the list
};

/*
 * things used in memfile.c
 */

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ ਖੰਡ_hdr    bhdr_T;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ memfile	    memfile_T;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਲੰਮਾ		    blocknr_T;

/*
 * mf_hashtab_T is a chained hashtable with blocknr_T key and arbitrary
 * structures as items.  This is an intrusive data structure: we require
 * that items begin with mf_hashitem_T which contains the key and linked
 * list pointers.  List of items in each bucket is doubly-linked.
 */

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ mf_hashitem_S mf_hashitem_T;

ਬਣਤਰ mf_hashitem_S
{
    mf_hashitem_T   *mhi_ਅਗਲਾ;
    mf_hashitem_T   *mhi_prev;
    blocknr_T	    mhi_ਕੁੰਜੀ;
};

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MHT_INIT_ਮਾਪ   64

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ mf_hashtab_S
{
    long_u	    mht_ਨਕਾਬ;	    // mask used for hash value (nr of items
				    // in array is "mht_mask" + 1)
    long_u	    mht_ਗਿਣਤੀ;	    // nr of items inserted into hashtable
    mf_hashitem_T   **mht_buckets;  // points to mht_small_buckets or
				    //dynamically allocated array
    mf_hashitem_T   *mht_small_buckets[MHT_INIT_ਮਾਪ];   // initial buckets
    ਅੱਖਰ	    mht_fixed;	    // non-zero value forbids growth
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

ਬਣਤਰ ਖੰਡ_hdr
{
    mf_hashitem_T bh_hashitem;      // header for hash table and key
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ bh_bnum bh_hashitem.mhi_ਕੁੰਜੀ // block number, part of bh_hashitem

    bhdr_T	*bh_ਅਗਲਾ;	    // next block_hdr in free or used list
    bhdr_T	*bh_prev;	    // previous block_hdr in used list
    char_u	*bh_ਅੰਕੜੇ;	    // pointer to memory (for used block)
    ਪੂਰਨ		bh_ਪੰਨਾ_ਗਿਣਤੀ;	    // number of pages in this block

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BH_DIRTY    1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BH_LOCKED   2
    ਅੱਖਰ	bh_flags;	    // BH_DIRTY or BH_LOCKED
};

/*
 * when a block with a negative number is flushed to the file, it gets
 * a positive number. Because the reference to the block is still the negative
 * number, we remember the translation to the new positive number in the
 * double linked trans lists. The structure is the same as the hash lists.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ nr_trans NR_TRANS;

ਬਣਤਰ nr_trans
{
    mf_hashitem_T nt_hashitem;		// header for hash table and key
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ nt_old_bnum nt_hashitem.mhi_ਕੁੰਜੀ	// old, negative, number

    blocknr_T	nt_new_bnum;		// new, positive, number
};


ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ buffblock buffblock_T;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ buffheader buffheader_T;

/*
 * structure used to store one block of the stuff/redo/recording buffers
 */
ਬਣਤਰ buffblock
{
    buffblock_T	*b_ਅਗਲਾ;	// pointer to next buffblock
    size_t	b_strlen;	// length of b_str, excluding the NUL
    char_u	b_str[1];	// contents (actually longer)
};

/*
 * header used for the stuff buffer and the redo buffer
 */
ਬਣਤਰ buffheader
{
    buffblock_T	bh_ਪਹਿਲਾ;	// first (dummy) block of list
    buffblock_T	*bh_curr;	// buffblock for appending
    ਪੂਰਨ		bh_ਸੂਚਕਾਂਕ;	// index for reading
    ਪੂਰਨ		bh_space;	// space in bh_curr for appending
    ਪੂਰਨ		bh_create_newblock;	// create a new block?
};

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    buffheader_T sr_redobuff;
    buffheader_T sr_old_redobuff;
} save_redo_T;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
    XP_ਅਗੇਤਰ_NONE,	// prefix not used
    XP_ਅਗੇਤਰ_NO,	// "no" prefix for bool option
    XP_ਅਗੇਤਰ_INV,	// "inv" prefix for bool option
} xp_ਅਗੇਤਰ_T;

/*
 * :set operator types
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
    OP_NONE = 0,
    OP_ADDING,		// "opt+=arg"
    OP_PREPENDING,	// "opt^=arg"
    OP_REMOVING,	// "opt-=arg"
} ਸੈੱਟ_op_T;

/*
 * used for completion on the command line
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ expand
{
    char_u	*xp_ਨਮੂਨਾ;		// start of item to expand, guaranteed
					// to be part of xp_line
    ਪੂਰਨ		xp_context;		// type of expansion
    ਪੂਰਨ		xp_ਨਮੂਨਾ_len;		// bytes in xp_pattern before cursor
    xp_ਅਗੇਤਰ_T	xp_ਅਗੇਤਰ;
#ਜੇ defined(FEAT_EVAL)
    char_u	*xp_arg;		// completion function
    ਪੂਰਨ		xp_complete_opt;	// UCC_ flags for user command
    sctx_T	xp_script_ctx;		// SCTX for completion function
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		xp_backslash;		// one of the XP_BS_ values
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ BACKSLASH_ਵਿੱਚ_FILENAME
    ਪੂਰਨ		xp_shell;		// TRUE for a shell command, more
					// characters need to be escaped
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		xp_numfiles;		// number of files found by
					// file name completion
    ਪੂਰਨ		xp_ਖਾਨਾ;			// cursor position in line
    ਪੂਰਨ		xp_selected;		// selected index in completion
    char_u	*xp_orig;		// originally expanded string
    char_u	**xp_files;		// list of files
    char_u	**xp_files_abbr;	// optional parallel array of display
					// strings (override xp_files for the
					// pum text); NULL if unused
    char_u	**xp_files_ਕਿਸਮ;	// optional parallel array of "kind"
					// strings; NULL if unused
    char_u	**xp_files_menu;	// optional parallel array of "menu"
					// strings (shown after the match);
					// NULL if unused
    char_u	**xp_files_info;	// optional parallel array of "info"
					// strings (shown in info popup);
					// NULL if unused
    char_u	*xp_ਸਤਰ;		// text being completed
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXPAND_BUF_LEN 256
    char_u	xp_buf[EXPAND_BUF_LEN];	// buffer for returned match
    ਪੂਰਨ		xp_ਖੋਜ_dir;		// Direction of search
    pos_T	xp_pre_incsearch_pos;	// Cursor position before incsearch
} expand_T;

/*
 * values for xp_backslash
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XP_BS_NONE	0	// nothing special for backslashes
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XP_BS_ONE	0x1	// uses one backslash before a space
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XP_BS_THREE	0x2	// uses three backslashes before a space
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XP_BS_COMMA	0x4	// commas need to be escaped with a backslash

/*
 * Variables shared between getcmdline(), redrawcmdline() and others.
 * These need to be saved when using CTRL-R |, that's why they are in a
 * structure.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    char_u	*cmdbuff;	// pointer to command line buffer
    ਪੂਰਨ		cmdbufflen;	// length of cmdbuff
    ਪੂਰਨ		cmdlen;		// number of chars in command line
    ਪੂਰਨ		cmdpos;		// current cursor position
    ਪੂਰਨ		cmdspos;	// cursor column on screen
    ਪੂਰਨ		cmdfirstc;	// ':', '/', '?', '=', '>' or NUL
    ਪੂਰਨ		cmdindent;	// number of spaces before cmdline
    char_u	*cmdprompt;	// message in front of cmdline
    ਪੂਰਨ		cmdattr;	// attributes for prompt
    ਪੂਰਨ		overstrike;	// Typing mode on the command line.  Shared by
				// getcmdline() and put_on_cmdline().
    expand_T	*xpc;		// struct being used for expansion, xp_pattern
				// may point into cmdbuff
    ਪੂਰਨ		xp_context;	// type of expansion
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    char_u	*xp_arg;	// user-defined expansion arg
    ਪੂਰਨ		ਨਿਵੇਸ਼_fn;	// when TRUE Invoked for input() function
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		cmdbuff_replaced; // when TRUE cmdline was replaced externally
				  // (e.g. by setcmdline())
} cmdline_info_T;

/*
 * Command modifiers ":vertical", ":browse", ":confirm" and ":hide" set a flag.
 * This needs to be saved for recursive commands, put them in a structure for
 * easy manipulation.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    ਪੂਰਨ		cmod_flags;		// CMOD_ flags
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CMOD_SANDBOX	    0x0001	// ":sandbox"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CMOD_SILENT	    0x0002	// ":silent"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CMOD_ERRSILENT	    0x0004	// ":silent!"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CMOD_UNSILENT	    0x0008	// ":unsilent"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CMOD_NOAUTOCMD	    0x0010	// ":noautocmd"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CMOD_HIDE	    0x0020	// ":hide"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CMOD_BROWSE	    0x0040	// ":browse" - invoke file dialog
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CMOD_CONFIRM	    0x0080	// ":confirm" - invoke yes/no dialog
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CMOD_KEEPALT	    0x0100	// ":keepalt"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CMOD_KEEPMARKS	    0x0200	// ":keepmarks"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CMOD_KEEPJUMPS	    0x0400	// ":keepjumps"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CMOD_LOCKMARKS	    0x0800	// ":lockmarks"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CMOD_KEEPPATTERNS   0x1000	// ":keeppatterns"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CMOD_NOSWAPFILE	    0x2000	// ":noswapfile"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CMOD_VIM9CMD	    0x4000	// ":vim9cmd"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CMOD_LEGACY	    0x8000	// ":legacy"

    ਪੂਰਨ		cmod_ਵੰਡੋ;		// flags for win_split()
    ਪੂਰਨ		cmod_tab;		// > 0 when ":tab" was used
    regmatch_T	cmod_filter_regmatch;	// set by :filter /pat/
    ਪੂਰਨ		cmod_filter_force;	// set for :filter!

    ਪੂਰਨ		cmod_verbose;		// 0 if not set, > 0 to set 'verbose'
					// to cmod_verbose - 1

    // values for undo_cmdmod()
    char_u	*cmod_save_ei;		// saved value of 'eventignore'
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SANDBOX
    ਪੂਰਨ		cmod_did_sandbox;	// set when "sandbox" was incremented
#ਜੇ_ਅੰਤ
    ਲੰਮਾ	cmod_verbose_save;	// if 'verbose' was set: value of
					// p_verbose plus one
    ਪੂਰਨ		cmod_save_ਸੁਨੇਹਾ_silent;	// if non-zero: saved value of
					// msg_silent + 1
    ਪੂਰਨ		cmod_save_ਸੁਨੇਹਾ_scroll;	// for restoring msg_scroll
    ਪੂਰਨ		cmod_did_esilent;	// incremented when emsg_silent is
} cmdmod_T;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
    MF_DIRTY_NO = 0,		// no dirty blocks
    MF_DIRTY_YES,		// there are dirty blocks
    MF_DIRTY_YES_NOSYNC,	// there are dirty blocks, do not sync yet
} mfdirty_T;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MF_ਬੀਜ_LEN	8

ਬਣਤਰ memfile
{
    char_u	*mf_fname;		// name of the file
    char_u	*mf_ffname;		// idem, full path
    ਪੂਰਨ		mf_fd;			// file descriptor
    ਪੂਰਨ		mf_flags;		// flags used when opening this memfile
    ਪੂਰਨ		mf_reopen;		// mf_fd was closed, retry opening
    bhdr_T	*mf_free_ਪਹਿਲਾ;		// first block_hdr in free list
    bhdr_T	*mf_ਵਰਤਿਆ_ਪਹਿਲਾ;		// mru block_hdr in used list
    bhdr_T	*mf_ਵਰਤਿਆ_ਆਖਰੀ;		// lru block_hdr in used list
    ਬਿਨਾਨਿਸ਼ਾਨੀ	mf_ਵਰਤਿਆ_ਗਿਣਤੀ;		// number of pages in used list
    ਬਿਨਾਨਿਸ਼ਾਨੀ	mf_ਵਰਤਿਆ_ਗਿਣਤੀ_max;	// maximum number of pages in memory
    mf_hashtab_T mf_hash;		// hash lists
    mf_hashtab_T mf_trans;		// trans lists
    blocknr_T	mf_blocknr_max;		// highest positive block number + 1
    blocknr_T	mf_blocknr_min;		// lowest negative block number - 1
    blocknr_T	mf_neg_ਗਿਣਤੀ;		// number of negative blocks numbers
    blocknr_T	mf_infile_ਗਿਣਤੀ;	// number of pages in the file
    ਬਿਨਾਨਿਸ਼ਾਨੀ	mf_ਪੰਨਾ_ਮਾਪ;		// number of bytes in a page
    mfdirty_T	mf_dirty;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CRYPT
    buf_T	*mf_ਬਫਰ;		// buffer this memfile is for
    char_u	mf_ਬੀਜ[MF_ਬੀਜ_LEN];	// seed for encryption

    // Values for key, method and seed used for reading data blocks when
    // updating for a newly set key or method. Only when mf_old_key != NULL.
    char_u	*mf_old_ਕੁੰਜੀ;
    ਪੂਰਨ		mf_old_cm;
    char_u	mf_old_ਬੀਜ[MF_ਬੀਜ_LEN];
#ਜੇ_ਅੰਤ
};

/*
 * things used in memline.c
 */
/*
 * When searching for a specific line, we remember what blocks in the tree
 * are the branches leading to that block. This is stored in ml_stack.  Each
 * entry is a pointer to info in a block (may be data block or pointer block)
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ info_ਸੂਚਕ
{
    blocknr_T	ip_bnum;	// block number
    linenr_T	ip_ਨੀਵਾਂ;		// lowest lnum in this block
    linenr_T	ip_ਉੱਚਾ;	// highest lnum in this block
    ਪੂਰਨ		ip_ਸੂਚਕਾਂਕ;	// index for block with current lnum
} infoptr_T;	// block/index pair

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_BYTEOFF
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ ml_chunksize
{
    ਪੂਰਨ		mlcs_numlines;
    ਲੰਮਾ	mlcs_totalsize;
} chunksize_T;

/*
 * Flags when calling ml_updatechunk()
 */
# define ML_CHNK_ADDLINE 1
# define ML_CHNK_DELLINE 2
# define ML_CHNK_UPDLINE 3
#ਜੇ_ਅੰਤ

/*
 * the memline structure holds all the information about a memline
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ memline
{
    linenr_T	ml_ਸਤਰ_ਗਿਣਤੀ;	// number of lines in the buffer

    memfile_T	*ml_mfp;	// pointer to associated memfile

    infoptr_T	*ml_ਢੇਰ;	// stack of pointer blocks (array of IPTRs)
    ਪੂਰਨ		ml_ਢੇਰ_ਸਿਖਰ;	// current top of ml_stack
    ਪੂਰਨ		ml_ਢੇਰ_ਮਾਪ;	// total number of entries in ml_stack

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ML_ਖਾਲੀ_ਥਾਵਾਂ	0x01	// empty buffer
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ML_ਸਤਰ_DIRTY	0x02	// cached line was changed and allocated
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ML_LOCKED_DIRTY	0x04	// ml_locked was changed
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ML_LOCKED_POS	0x08	// ml_locked needs positive block number
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ML_ALLOCATED	0x10	// ml_line_ptr is an allocated copy
    ਪੂਰਨ		ml_flags;

    colnr_T	ml_ਸਤਰ_len;	// length of the cached line + NUL + text properties
    colnr_T	ml_ਸਤਰ_textlen;// length of the cached line + NUL, 0 if not known yet
    linenr_T	ml_ਸਤਰ_lnum;	// line number of cached line, 0 if not valid
    char_u	*ml_ਸਤਰ_ptr;	// pointer to cached line

    bhdr_T	*ml_locked;	// block used by last ml_get
    linenr_T	ml_locked_ਨੀਵਾਂ;	// first line in ml_locked
    linenr_T	ml_locked_ਉੱਚਾ;	// last line in ml_locked
    ਪੂਰਨ		ml_locked_lineadd;  // number of lines inserted in ml_locked
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_BYTEOFF
    chunksize_T *ml_chunksize;
    ਪੂਰਨ		ml_numchunks;
    ਪੂਰਨ		ml_usedchunks;
#ਜੇ_ਅੰਤ
} memline_T;

// Values for the flags argument of ml_delete_flags().
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ML_DEL_ਸੁਨੇਹਾ	    1	// may give a "No lines in buffer" message
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ML_DEL_UNDO	    2	// called from undo, do not update textprops
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ML_DEL_NOPROP	    4	// splitting data block, do not update textprops

// Values for the flags argument of ml_append_int().
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ML_APPEND_NEW	    1	// starting to edit a new file
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ML_APPEND_MARK	    2	// mark the new line
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ML_APPEND_UNDO	    4	// called from undo
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ML_APPEND_NOPROP    8	// do not continue textprop from previous line


/*
 * Structure defining text properties.  These stick with the text.
 * When stored in memline they are after the text, ml_line_len is larger than
 * STRLEN(ml_line_ptr) + 1.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ textprop_S
{
    colnr_T	tp_ਖਾਨਾ;		// start column (one based, in bytes)
    colnr_T	tp_len;		// length in bytes; for virtual text props
				// this is STRLEN(vtext) (not including NUL)
    ਪੂਰਨ		tp_id;		// identifier
    ਪੂਰਨ		tp_type;	// property type
    ਪੂਰਨ		tp_flags;	// TP_FLAG_ values
    ਪੂਰਨ		tp_padleft;	// left padding between text line and virtual
				//ਪਾਠ
    ਸੰਘ			// For virtual text props (tp_id < 0):
    {				// check TP_FLAG_VTEXT_PTR in tp_flags to
				// determine which member is active.
	colnr_T	tp_ਪਾਠ_ਵਿੱਥ; // offset to vtext string from the
				// prop_count position in the memline
				// (when TP_FLAG_VTEXT_PTR is NOT set)
	char_u	*tp_ਪਾਠ;	// pointer to virtual text string
				// (when TP_FLAG_VTEXT_PTR IS set)
    } u;
} textprop_T;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TP_FLAG_CONT_ਅਗਲਾ	0x1	// property continues in next line
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TP_FLAG_CONT_PREV	0x2	// property was continued from prev line

// without these text is placed after the end of the line
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TP_FLAG_ALIGN_ਸੱਜਾ	0x010	// virtual text is right-aligned
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TP_FLAG_ALIGN_ABOVE	0x020	// virtual text above the line
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TP_FLAG_ALIGN_ਹੇਠਾਂ	0x040	// virtual text on next screen line

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TP_FLAG_WRAP		0x080	// virtual text wraps - when missing
					// text is truncated
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TP_FLAG_ਸ਼ੁਰੂ_INCL	0x100	// "start_incl" copied from proptype
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TP_FLAG_DELETED		0x200	// marked for deletion in
					// unpacked_memline_T
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TP_FLAG_VTEXT_PTR	0x400	// u.tp_text access is valid

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PROP_ਗਿਣਤੀ_ਮਾਪ	ਆਕਾਰ(uint16_t)    // size of prop_count in memline

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PROP_ਪਾਠ_MIN_CELLS	4	// minimum number of cells to use for
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
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ unpacked_memline_S
{
    buf_T	*buf;		// the line's buffer (NULL = closed)
    linenr_T	lnum;		// line number (0 = no line loaded)
    bool	detached;	// true when text/vtext are allocated
    colnr_T	ਪਾਠ_ਮਾਪ_2;	// size of text including NUL
    char_u	*ਪਾਠ;		// NUL-terminated text
    ਪੂਰਨ		prop_ਮਾਪ;	// number of allocated prop slots
    ਪੂਰਨ		prop_ਗਿਣਤੀ;	// number of properties
    textprop_T	*props;		// property array
    bool	ਪਾਠ_ਬਦਲਿਆ;	// true if text was modified
} unpacked_memline_T;

/*
 * Structure defining a property type.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ proptype_S
{
    ਪੂਰਨ		pt_id;		// value used for tp_id
    ਪੂਰਨ		pt_type;	// number used for tp_type
    ਪੂਰਨ		pt_hl_id;	// highlighting
    ਪੂਰਨ		pt_ਤਰਜੀਹ;	//ਤਰਜੀਹ
    ਪੂਰਨ		pt_flags;	// PT_FLAG_ values
    char_u	pt_name[1];	// property type name, actually longer
} proptype_T;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PT_FLAG_INS_ਸ਼ੁਰੂ_INCL	1	// insert at start included in property
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PT_FLAG_INS_ਸਿਰਾ_INCL	2	// insert at end included in property
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PT_FLAG_COMBINE		4	// combine with syntax highlight
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PT_FLAG_OVERRIDE	8	// override any highlight

// Sign group
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ signgroup_S
{
    ਪੂਰਨ		sg_ਅਗਲਾ_sign_id;	// next sign id for this group
    short_u	sg_refcount;		// number of signs in this group
    char_u	sg_name[1];		// sign group name, actually longer
} signgroup_T;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ sign_ਇੰਦਰਾਜ sign_ਇੰਦਰਾਜ_T;
ਬਣਤਰ sign_ਇੰਦਰਾਜ
{
    ਪੂਰਨ		 se_id;		// unique identifier for each placed sign
    ਪੂਰਨ		 se_typenr;	// typenr of sign
    ਪੂਰਨ		 se_ਤਰਜੀਹ;	// priority for highlighting
    linenr_T	 se_lnum;	// line number which has this sign
    signgroup_T	 *se_ਸਮੂਹ;	// sign group
    sign_ਇੰਦਰਾਜ_T *se_ਅਗਲਾ;	// next entry in a list of signs
    sign_ਇੰਦਰਾਜ_T *se_prev;	// previous entry -- for easy reordering
};

/*
 * Sign attributes. Used by the screen refresh routines.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ sign_attrs_S {
    ਪੂਰਨ		sat_typenr;
    ਖਾਲੀ	*sat_icon;
    char_u	*sat_ਪਾਠ;
    ਪੂਰਨ		sat_texthl;
    ਪੂਰਨ		sat_linehl;
    ਪੂਰਨ		sat_culhl;
    ਪੂਰਨ		sat_numhl;
    ਪੂਰਨ		sat_ਤਰਜੀਹ;
} sign_attrs_T;

#ਜੇ defined(FEAT_SIGNS)
// Macros to get the sign group structure from the group name
# define SGN_ਕੁੰਜੀ_OFF	offsetof(signgroup_T, sg_name)
# define HI2SG(hi)	((signgroup_T *)((hi)->hi_ਕੁੰਜੀ - SGN_ਕੁੰਜੀ_OFF))

// Default sign priority for highlighting
# define SIGN_DEF_PRIO	10

#ਜੇ_ਅੰਤ

/*
 * Argument list: Array of file names.
 * Used for the global argument list and the argument lists local to a window.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ arglist
{
    garray_T	al_ga;		// growarray with the array of file names
    ਪੂਰਨ		al_refcount;	// number of windows using this arglist
    ਪੂਰਨ		id;		// id of this arglist
} alist_T;

/*
 * For each argument remember the file name as it was given, and the buffer
 * number that contains the expanded file name (required for when ":cd" is
 * used).
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ argentry
{
    char_u	*ae_fname;	// file name as specified
    ਪੂਰਨ		ae_fnum;	// buffer number with expanded file name
} aentry_T;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ALIST(win)	(win)->w_alist
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GARGLIST	((aentry_T *)global_alist.al_ga.ga_ਅੰਕੜੇ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ARGLIST		((aentry_T *)ALIST(curwin)->al_ga.ga_ਅੰਕੜੇ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WARGLIST(wp)	((aentry_T *)ALIST(wp)->al_ga.ga_ਅੰਕੜੇ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AARGLIST(al)	((aentry_T *)((al)->al_ga.ga_ਅੰਕੜੇ))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GARGCOUNT	(global_alist.al_ga.ga_len)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ARGCOUNT	(ALIST(curwin)->al_ga.ga_len)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WARGCOUNT(wp)	(ALIST(wp)->al_ga.ga_len)

/*
 * A list used for saving values of "emsg_silent".  Used by ex_try() to save the
 * value of "emsg_silent" if it was non-zero.  When this is done, the CSF_SILENT
 * flag below is set.
 */

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ eslist_elem eslist_T;
ਬਣਤਰ eslist_elem
{
    ਪੂਰਨ		saved_emsg_silent;	// saved value of "emsg_silent"
    eslist_T	*ਅਗਲਾ;			// next element on the list
};

/*
 * For conditional commands a stack is kept of nested conditionals.
 * When cs_idx < 0, there is no conditional command.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CSTACK_LEN	50

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    ਛੋਟਾ	cs_flags[CSTACK_LEN];	// CSF_ flags
    ਅੱਖਰ	cs_pending[CSTACK_LEN];	// CSTP_: what's pending in ":finally"
    ਸੰਘ {
	ਖਾਲੀ	*csp_rv[CSTACK_LEN];	// return typeval for pending return
	ਖਾਲੀ	*csp_ex[CSTACK_LEN];	// exception for pending throw
    }		cs_pend;
    ਖਾਲੀ	*cs_forinfo[CSTACK_LEN]; // info used by ":for"
    ਪੂਰਨ		cs_ਸਤਰ[CSTACK_LEN];	// line nr of ":while"/":for" line
    ਪੂਰਨ		cs_ਖੰਡ_id[CSTACK_LEN];    // block ID stack
    ਪੂਰਨ		cs_script_var_len[CSTACK_LEN];	// value of sn_var_vals.ga_len
						// when entering the block
    ਪੂਰਨ		cs_idx;			// current entry, or -1 if none
    ਪੂਰਨ		cs_looplevel;		// nr of nested ":while"s and ":for"s
    ਪੂਰਨ		cs_trylevel;		// nr of nested ":try"s
    eslist_T	*cs_emsg_silent_list;	// saved values of "emsg_silent"
    ਅੱਖਰ	cs_lflags;		// loop flags: CSL_ flags
} cstack_T;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ cs_rettv	cs_pend.csp_rv
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ cs_exception	cs_pend.csp_ex

// There is no CSF_IF, the lack of CSF_WHILE, CSF_FOR and CSF_TRY means ":if"
// was used.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CSF_TRUE	0x0001	// condition was TRUE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CSF_ਸਰਗਰਮ	0x0002	// current state is active
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CSF_ELSE	0x0004	// ":else" has been passed
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CSF_WHILE	0x0008	// is a ":while"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CSF_FOR	0x0010	// is a ":for"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CSF_ਖੰਡ	0x0020	// is a "{" block

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CSF_TRY	0x0100	// is a ":try"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CSF_FINALLY	0x0200	// ":finally" has been passed
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CSF_CATCH	0x0400	// ":catch" has been seen
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CSF_THROWN	0x0800	// exception thrown to this try conditional
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CSF_CAUGHT	0x1000  // exception caught by this try conditional
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CSF_ਮੁਕੰਮਲ	0x2000  // CSF_CAUGHT was handled by finish_exception()
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CSF_SILENT	0x4000	// "emsg_silent" reset by ":try"
// Note that CSF_ELSE is only used when CSF_TRY and CSF_WHILE are unset
// (an ":if"), and CSF_SILENT is only used when CSF_TRY is set.

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CSF_FUNC_DEF	0x8000	// a function was defined in this block

/*
 * What's pending for being reactivated at the ":endtry" of this try
 * conditional:
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CSTP_NONE	0	// nothing pending in ":finally" clause
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CSTP_ERROR	1	// an error is pending
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CSTP_ਰੁਕਾਵਟ	2	// an interrupt is pending
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CSTP_THROW	4	// a throw is pending
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CSTP_BREAK	8	// ":break" is pending
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CSTP_CONTINUE	16	// ":continue" is pending
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CSTP_RETURN	24	// ":return" is pending
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CSTP_ਅੰਤ	32	// ":finish" is pending

/*
 * Flags for the cs_lflags item in cstack_T.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CSL_HAD_LOOP	 1	// just found ":while" or ":for"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CSL_HAD_ENDLOOP 2	// just found ":endwhile" or ":endfor"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CSL_HAD_CONT	 4	// just found ":continue"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CSL_HAD_FINA	 8	// just found ":finally"

/*
 * A list of error messages that can be converted to an exception.  "throw_msg"
 * is only set in the first element of the list.  Usually, it points to the
 * original message stored in that element, but sometimes it points to a later
 * message in the list.  See cause_errthrow().
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ msglist msglist_T;
ਬਣਤਰ msglist
{
    msglist_T	*ਅਗਲਾ;		// next of several messages in a row
    ਅੱਖਰ	*ਸੁਨੇਹਾ;		// original message, allocated
    ਅੱਖਰ	*throw_ਸੁਨੇਹਾ;	// msg to throw: usually original one
    char_u	*sfile;		// value from estack_sfile(), allocated
    ਲੰਮਾ	slnum;		// line number for "sfile"
    ਪੂਰਨ		ਸੁਨੇਹਾ_compiling;	// saved value of estack_compiling
};

/*
 * The exception types.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ
{
    ET_USER,		// exception caused by ":throw" command
    ET_ERROR,		// error exception
    ET_ਰੁਕਾਵਟ,	// interrupt exception triggered by Ctrl-C
} except_type_T;

/*
 * Structure describing an exception.
 * (don't use "struct exception", it's used by the math library).
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ vim_exception except_T;
ਬਣਤਰ vim_exception
{
    except_type_T	type;		// exception type
    ਅੱਖਰ		*ਮੁੱਲ;		// exception value
    ਬਣਤਰ msglist	*messages;	// message(s) causing error exception
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
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ cleanup_stuff cleanup_T;
ਬਣਤਰ cleanup_stuff
{
    ਪੂਰਨ pending;		// error/interrupt/exception state
    except_T *exception;	// exception value
};

/*
 * Exception state that is saved and restored when calling timer callback
 * functions and deferred functions.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ exception_state_S exception_state_T;
ਬਣਤਰ exception_state_S
{
    except_T	*estate_current_exception;
    ਪੂਰਨ		estate_did_throw;
    ਪੂਰਨ		estate_need_rethrow;
    ਪੂਰਨ		estate_trylevel;
    ਪੂਰਨ		estate_did_emsg;
};

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
// struct passed to in_id_list()
ਬਣਤਰ sp_syn
{
    ਪੂਰਨ		inc_ਟੈਗ;	// ":syn include" unique tag
    ਛੋਟਾ	id;		// highlight group ID of item
    ਛੋਟਾ	*cont_ਵਿੱਚ_list;	// cont.in group IDs, if non-zero
};

/*
 * Each keyword has one keyentry, which is linked in a hash list.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ keyentry keyentry_T;

ਬਣਤਰ keyentry
{
    keyentry_T	*ke_ਅਗਲਾ;	// next entry with identical "keyword[]"
    ਬਣਤਰ sp_syn k_syn;	// struct passed to in_id_list()
    ਛੋਟਾ	*ਅਗਲਾ_list;	// ID list for next match (if non-zero)
    ਪੂਰਨ		flags;
    ਪੂਰਨ		k_char;		// conceal substitute character
    char_u	keyword[1];	// actually longer
};

/*
 * Struct used to store one state of the state stack.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ buf_state
{
    ਪੂਰਨ		    bs_idx;	 // index of pattern
    ਪੂਰਨ		    bs_flags;	 // flags for pattern
# ifdef FEAT_CONCEAL
    ਪੂਰਨ		    bs_seqnr;	 // stores si_seqnr
    ਪੂਰਨ		    bs_cchar;	 // stores si_cchar
# endif
    reg_extmatch_T *bs_extmatch; // external matches from start pattern
} bufstate_T;

/*
 * syn_state contains the syntax state stack for the start of one line.
 * Used by b_sst_array[].
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ syn_state synstate_T;

ਬਣਤਰ syn_state
{
    synstate_T	*sst_ਅਗਲਾ;	// next entry in used or free list
    linenr_T	sst_lnum;	// line number for this state
    ਸੰਘ
    {
	bufstate_T	sst_ਢੇਰ[SST_FIX_ਹਾਲਤਾਂ]; // short state stack
	garray_T	sst_ga;	// growarray for long state stack
    } sst_union;
    ਪੂਰਨ		sst_ਅਗਲਾ_flags;	// flags for sst_next_list
    ਪੂਰਨ		sst_stacksize;	// number of states on the stack
    ਛੋਟਾ	*sst_ਅਗਲਾ_list;	// "nextgroup" list in this state
				// (this is a copy, don't free it!
    disptick_T	sst_tick;	// tick when last displayed
    linenr_T	sst_change_lnum;// when non-zero, change in this line
				// may have made the state invalid
};
#ਜੇ_ਅੰਤ // FEAT_SYN_HL

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_HL_ID       20000	// maximum value for a highlight ID.

/*
 * Structure shared between syntax.c, screen.c and gui_x11.c.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ attr_ਇੰਦਰਾਜ
{
    ਛੋਟਾ	    ae_attr;		// HL_BOLD, etc.
    ਸੰਘ
    {
	ਬਣਤਰ
	{
	    char_u	    *ਸ਼ੁਰੂ;	// start escape sequence
	    char_u	    *stop;	// stop escape sequence
	} ਪਦ;
	ਬਣਤਰ
	{
	    // These colors need to be > 8 bits to hold 256.
	    short_u	    fg_color;	// foreground color number
	    short_u	    bg_color;	// background color number
	    short_u	    ul_color;	// underline color number
	    short_u	    font;	// font number
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMGUICOLORS
	    guicolor_T	    fg_rgb;	// foreground color RGB
	    guicolor_T	    bg_rgb;	// background color RGB
	    guicolor_T	    ul_rgb;	// underline color RGB
#ਜੇ_ਅੰਤ
	} cterm;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
	ਬਣਤਰ
	{
	    guicolor_T	    fg_color;	// foreground color handle
	    guicolor_T	    bg_color;	// background color handle
	    guicolor_T	    sp_color;	// special color handle
	    GuiFont	    font;	// font handle
# ifdef FEAT_XFONTSET
	    GuiFontset	    fontset;	// fontset handle
# endif
	} gui;
#ਜੇ_ਅੰਤ
    } ae_u;
} attrentry_T;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ICONV
# ifdef HAVE_ICONV_H
#  include <iconv.h>
# ਨਹੀਂਤਾਂ
#  ਜੇ defined(MACOS_X)
#   include <sys/errno.h>
#   ifndef EILSEQ
#    define EILSEQ ENOENT // Early MacOS X does not have EILSEQ
#   endif
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ _iconv_t *iconv_t;
#  ਨਹੀਂਤਾਂ
#   include <errno.h>
#  endif
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਖਾਲੀ *iconv_t;
# endif
#ਜੇ_ਅੰਤ

/*
 * Used for the typeahead buffer: typebuf.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    char_u	*tb_buf;	// buffer for typed characters
    char_u	*tb_noremap;	// mapping flags for characters in tb_buf[]
    ਪੂਰਨ		tb_buflen;	// size of tb_buf[]
    ਪੂਰਨ		tb_off;		// current position in tb_buf[]
    ਪੂਰਨ		tb_len;		// number of valid bytes in tb_buf[]
    ਪੂਰਨ		tb_maplen;	// nr of mapped bytes in tb_buf[]
    ਪੂਰਨ		tb_silent;	// nr of silently mapped bytes in tb_buf[]
    ਪੂਰਨ		tb_no_abbr_cnt; // nr of bytes without abbrev. in tb_buf[]
    ਪੂਰਨ		tb_change_cnt;	// nr of time tb_buf was changed; never zero
} typebuf_T;

// Struct to hold the saved typeahead for save_typeahead().
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    typebuf_T		save_typebuf;
    ਪੂਰਨ			typebuf_valid;	    // TRUE when save_typebuf valid
    ਪੂਰਨ			old_char;
    ਪੂਰਨ			old_mod_ਨਕਾਬ;
    buffheader_T	save_readbuf1;
    buffheader_T	save_readbuf2;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ਨਿਵੇਸ਼_BUF
    char_u		*save_inputbuf;
#ਜੇ_ਅੰਤ
} tasave_T;

/*
 * Used for conversion of terminal I/O and script files.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    ਪੂਰਨ		vc_type;	// zero or one of the CONV_ values
    ਪੂਰਨ		vc_ਗੁਣਕ;	// max. expansion factor
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
    ਪੂਰਨ		vc_cpfrom;	// codepage to convert from (CONV_CODEPAGE)
    ਪੂਰਨ		vc_cpto;	// codepage to convert to (CONV_CODEPAGE)
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ICONV
    iconv_t	vc_fd;		// for CONV_ICONV
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		vc_fail;	// fail for invalid char, don't use '?'
} vimconv_T;

/*
 * Structure used for the command line history.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ hist_ਇੰਦਰਾਜ
{
    ਪੂਰਨ		hisnum;		// identifying number
    ਪੂਰਨ		viminfo;	// when TRUE hisstr comes from viminfo
    char_u	*hisstr;	// actual entry, separator char after the NUL
    size_t	hisstrlen;	// length of hisstr (excluding the NUL)
    time_t	ਸਮਾਂ_ਸੈੱਟ;	// when it was typed, zero if unknown
} histentry_T;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CONV_NONE		0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CONV_ਮੰਜ਼ਿਲ_UTF8		1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CONV_9_ਮੰਜ਼ਿਲ_UTF8		2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CONV_ਮੰਜ਼ਿਲ_LATIN1		3
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CONV_ਮੰਜ਼ਿਲ_LATIN9		4
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CONV_ICONV		5
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
# define CONV_CODEPAGE		10	// codepage -> codepage
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MACOS_X
# define CONV_MAC_LATIN1	20
# define CONV_LATIN1_MAC	21
# define CONV_MAC_UTF8		22
# define CONV_UTF8_MAC		23
#ਜੇ_ਅੰਤ

/*
 * Structure used for mappings and abbreviations.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ mapblock mapblock_T;
ਬਣਤਰ mapblock
{
    mapblock_T	*m_ਅਗਲਾ;	// next mapblock in list
    mapblock_T	*m_alt;		// pointer to mapblock of the same mapping
				// with an alternative form of m_keys, or NULL
				// if there is no such mapblock
    char_u	*m_ਕੁੰਜੀਆਂ;	// mapped from, lhs
    char_u	*m_str;		// mapped to, rhs
    char_u	*m_orig_str;	// rhs as entered by the user
    ਪੂਰਨ		m_keylen;	// strlen(m_keys)
    ਪੂਰਨ		m_ਬਹੁਲਕ;		// valid mode
    ਪੂਰਨ		m_simplified;	// m_keys was simplified, do not use this map
				// if key_protocol_enabled() returns TRUE
    ਪੂਰਨ		m_noremap;	// if non-zero no re-mapping for m_str
    ਅੱਖਰ	m_silent;	// <silent> used, don't echo commands
    ਅੱਖਰ	m_nowait;	// <nowait> used
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਅੱਖਰ	m_ਵਿਅੰਜਕ;		// <expr> used, m_str is an expression
    sctx_T	m_script_ctx;	// SCTX where map was defined
#ਜੇ_ਅੰਤ
};


/*
 * Used for highlighting in the status line.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    char_u	*ਸ਼ੁਰੂ;
    ਪੂਰਨ		userhl;		// 0: no HL, 1-9: User HL, < 0 for syn ID
} stl_hlrec_T;

/*
 * Used for statusline click function regions.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    char_u	*ਸ਼ੁਰੂ;		// position in output buffer where region starts
    char_u	*funcname;	// function name (NULL = end/close marker)
    ਪੂਰਨ		minwid;		// minwid value from %N@Func@
} stl_clickrec_T;

/*
 * Per-window resolved click regions (screen column based).
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    ਪੂਰਨ		ਪੰਕਤੀ;		// screen row where region lives
    ਪੂਰਨ		ਖਾਨਾ_ਸ਼ੁਰੂ;	// screen column where region starts
    ਪੂਰਨ		ਖਾਨਾ_ਸਿਰਾ;	// screen column where region ends
    char_u	*funcname;	// function name (allocated copy)
    ਪੂਰਨ		minwid;		// minwid value
    ਪੂਰਨ		tabnr;		// tab page number (tabpanel only, 0 otherwise)
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
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ hashitem_S
{
    long_u	hi_hash;	// cached hash number of hi_key
    char_u	*hi_ਕੁੰਜੀ;
} hashitem_T;

// The address of "hash_removed" is used as a magic number for hi_key to
// indicate a removed item.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HI_ਕੁੰਜੀ_REMOVED &hash_removed
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi) ((hi)->hi_ਕੁੰਜੀ == ਨਿਰਾ || (hi)->hi_ਕੁੰਜੀ == &hash_removed)

// Initial size for a hashtable.  Our items are relatively small and growing
// is expensive, thus use 16 as a start.  Must be a power of 2.
// This allows for storing 10 items (2/3 of 16) before a resize is needed.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HT_INIT_ਮਾਪ 16

// flags used for ht_flags
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HTFLAGS_ERROR	0x01	// Set when growing failed, can't add more
				// items before growing works.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HTFLAGS_FROZEN	0x02	// Trying to add or remove an item will result
				// in an error message.

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ hashtable_S
{
    long_u	ht_ਨਕਾਬ;	// mask used for hash value (nr of items in
				// array is "ht_mask" + 1)
    long_u	ht_ਵਰਤਿਆ;	// number of items used
    long_u	ht_filled;	// number of items used + removed
    ਪੂਰਨ		ht_ਬਦਲਿਆ;	// incremented when adding or removing an item
    ਪੂਰਨ		ht_locked;	// counter for hash_lock()
    ਪੂਰਨ		ht_flags;	// HTFLAGS_ values
    hashitem_T	*ht_array;	// points to the array, allocated when it's
				// not "ht_smallarray"
    hashitem_T	ht_smallarray[HT_INIT_ਮਾਪ];   // initial array
} hashtab_T;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ long_u hash_T;		// Type for hi_hash


// Use 64-bit Number.
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
   ਕਿਸਮਪਰਿਭਾਸ਼ਾ __int64		varnumber_T;
   ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਿਨਾਨਿਸ਼ਾਨੀ __int64	uvarnumber_T;
# define VARNUM_MIN		_I64_MIN
# define VARNUM_MAX		_I64_MAX
# define UVARNUM_MAX		_UI64_MAX
#ਜਾਂਜੇ defined(HAVE_NO_LONG_LONG)
# ਜੇ defined(HAVE_STDINT_H)
   ਕਿਸਮਪਰਿਭਾਸ਼ਾ int64_t		varnumber_T;
   ਕਿਸਮਪਰਿਭਾਸ਼ਾ uint64_t		uvarnumber_T;
#  define VARNUM_MIN		INT64_MIN
#  define VARNUM_MAX		INT64_MAX
#  define UVARNUM_MAX		UINT64_MAX
# ਨਹੀਂਤਾਂ
   // this may cause trouble for code that depends on 64 bit ints
   ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਲੰਮਾ			varnumber_T;
   ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ	uvarnumber_T;
#  define VARNUM_MIN		LONG_MIN
#  define VARNUM_MAX		LONG_MAX
#  define UVARNUM_MAX		ULONG_MAX
# endif
#ਨਹੀਂਤਾਂ
  ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਲੰਮਾ ਲੰਮਾ		varnumber_T;
  ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਲੰਮਾ	uvarnumber_T;
# ifdef LLONG_MIN
#  define VARNUM_MIN		LLONG_MIN
#  define VARNUM_MAX		LLONG_MAX
#  define UVARNUM_MAX		ULLONG_MAX
# ਨਹੀਂਤਾਂ
#  define VARNUM_MIN		LONG_LONG_MIN
#  define VARNUM_MAX		LONG_LONG_MAX
#  define UVARNUM_MAX		ULONG_LONG_MAX
# endif
#ਜੇ_ਅੰਤ

// Struct that holds both a normal function name and a partial_T, as used for a
// callback argument.
// When used temporarily "cb_name" is not allocated.  The refcounts to either
// the function or the partial are incremented and need to be decremented
// later with free_callback().
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    char_u	*cb_name;
    partial_T	*cb_partial;
    ਪੂਰਨ		cb_free_name;	    // cb_name was allocated
} callback_T;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ isn_S isn_T;	    // instruction
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ dfunc_S dfunc_T;	    // :def function

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ type_S type_T;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ ufunc_S ufunc_T;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ jobvar_S ਕੰਮ_T;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ readq_S readq_T;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ writeq_S writeq_T;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ jsonq_S jsonq_T;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ cbq_S cbq_T;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ channel_S channel_T;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ cctx_S cctx_T;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ ectx_S ectx_T;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ instr_S instr_T;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ class_S class_T;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ object_S object_T;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ typealias_S typealias_T;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ
{
    VAR_UNKNOWN = 0,	// not set, any type or "void" allowed
    VAR_ANY,		// used for "any" type
    VAR_VOID,		// no value (function not returning anything)
    VAR_BOOL,		// "v_number" is used: VVAL_TRUE or VVAL_FALSE
    VAR_SPECIAL,	// "v_number" is used: VVAL_NULL or VVAL_NONE
    VAR_ਸੰਖਿਆ,		// "v_number" is used
    VAR_FLOAT,		// "v_float" is used
    VAR_STRING,		// "v_string" is used
    VAR_BLOB,		// "v_blob" is used
    VAR_FUNC,		// "v_string" is function name
    VAR_PARTIAL,	// "v_partial" is used
    VAR_LIST,		// "v_list" is used
    VAR_DICT,		// "v_dict" is used
    VAR_ਕੰਮ,		// "v_job" is used
    VAR_CHANNEL,	// "v_channel" is used
    VAR_INSTR,		// "v_instr" is used
    VAR_CLASS,		// "v_class" is used (also used for interface)
    VAR_OBJECT,		// "v_object" is used
    VAR_TYPEALIAS,	// "v_typealias" is used
    VAR_TUPLE		// "v_tuple" is used
} vartype_T;

// A type specification.
ਬਣਤਰ type_S {
    vartype_T	    tt_type;
    ਪੂਰਨ	    tt_argcount;    // for func, incl. vararg, -1 for unknown
    ਪੂਰਨ	    tt_min_argcount; // number of non-optional arguments
    short_u	    tt_flags;	    // TTFLAG_ values
    type_T	    *tt_member;	    // for list, dict, func return type
    class_T	    *tt_class;	    // for class and object
    type_T	    **tt_ਦਲੀਲਾਂ;	    // func argument types, allocated
};

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    type_T	*type_curr;	    // current type, value type
    type_T	*type_decl;	    // declared type or equal to type_current
} type2_T;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TTFLAG_VARARGS	    0x01    // func args ends with "..."
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TTFLAG_BOOL_OK	    0x02    // can be converted to bool
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TTFLAG_FLOAT_OK	    0x04    // number can be used/converted to float
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TTFLAG_ਸੰਖਿਆ_OK    0x08    // number can be used for a float
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TTFLAG_STATIC	    0x10    // one of the static types, e.g. t_any
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TTFLAG_CONST	    0x20    // cannot be changed
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TTFLAG_SUPER	    0x40    // object from "super".
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TTFLAG_GENERIC	    0x80    // generic type
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TTFLAG_TUPLE_OK	    0x100   // tuple can be used for a list

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IS_GENERIC_TYPE(type)	\
    ((type->tt_flags & TTFLAG_GENERIC) == TTFLAG_GENERIC)

// Type check flags
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TYPECHK_ਸੰਖਿਆ_OK	0x1	// number is accepted for a float
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TYPECHK_TUPLE_OK	0x2	// tuple is accepted for a list

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
    VIM_ACCESS_ਨਿੱਜੀ,	// read/write only inside the class
    VIM_ACCESS_READ,	// read everywhere, write only inside the class
    VIM_ACCESS_ALL	// read/write everywhere
} omacc_T;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OCMFLAG_HAS_TYPE	0x01	// type specified explicitly
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OCMFLAG_FINAL		0x02	// "final" object/class member
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OCMFLAG_CONST		0x04	// "const" object/class member

/*
 * Object methods called by builtin functions (e.g. string(), empty(), etc.)
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
    CLASS_BUILTIN_INVALID,
    CLASS_BUILTIN_STRING,
    CLASS_BUILTIN_ਖਾਲੀ_ਥਾਵਾਂ,
    CLASS_BUILTIN_LEN,
    CLASS_BUILTIN_MAX
} class_builtin_T;

/*
 * Entry for an object or class member variable.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    string_T	ocm_name;	// allocated
    omacc_T	ocm_access;
    type_T	*ocm_type;
    ਪੂਰਨ		ocm_flags;
    char_u	*ocm_init;	// allocated
    sctx_T	ocm_init_sctx;	// script context of the initializer expression
} ocmember_T;

// used for the lookup table of a class member index and object method index
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ itf2class_S itf2class_T;
ਬਣਤਰ itf2class_S {
    itf2class_T	*i2c_ਅਗਲਾ;
    class_T	*i2c_class;
    ਪੂਰਨ		i2c_is_ਵਿਧੀ;	    // TRUE for method indexes
    // array with ints follows
};

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CLASS_INTERFACE	    0x1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CLASS_EXTENDED	    0x2	    // another class extends this one
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CLASS_ABSTRACT	    0x4	    // abstract class
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CLASS_ENUM	    0x8	    // enum

// "class_T": used for v_class of typval of VAR_CLASS
// Also used for an interface (class_flags has CLASS_INTERFACE).
ਬਣਤਰ class_S
{
    string_T	class_name;		// allocated
    ਪੂਰਨ		class_flags;		// CLASS_ flags

    ਪੂਰਨ		class_refcount;
    ਪੂਰਨ		class_copyID;		// used by garbage collection
    class_T	*class_ਅਗਲਾ_ਵਰਤਿਆ;	// for list headed by "first_class"
    class_T	*class_prev_ਵਰਤਿਆ;	// for list headed by "first_class"

    class_T	*class_extends;		// parent class or NULL

    // interfaces declared for the class
    ਪੂਰਨ		class_interface_ਗਿਣਤੀ;
    char_u	**class_interfaces;	// allocated array of names
    class_T	**class_interfaces_cl;	// interfaces (counts as reference)
    itf2class_T	*class_itf2class;	// member index lookup tables

    // class members: "static varname"
    ਪੂਰਨ		class_class_member_ਗਿਣਤੀ;
    ocmember_T	*class_class_members;	// allocated
    typval_T	*class_members_tv;	// allocated array of class member vals

    // class functions: "static def SomeMethod()"
    ਪੂਰਨ		class_class_function_ਗਿਣਤੀ;	    // total count
    ਪੂਰਨ		class_class_function_ਗਿਣਤੀ_ਬੱਚਾ;   // count without "extends"
    ufunc_T	**class_class_functions;	// allocated

    // object members: "this.varname"
    ਪੂਰਨ		class_obj_member_ਗਿਣਤੀ;
    ocmember_T	*class_obj_members;	// allocated

    // object methods: "def SomeMethod()"
    ਪੂਰਨ		class_obj_ਵਿਧੀ_ਗਿਣਤੀ;		    // total count
    ਪੂਰਨ		class_obj_ਵਿਧੀ_ਗਿਣਤੀ_ਬੱਚਾ;	    // count without "extends"
    ufunc_T	**class_obj_methods;	// allocated

					// index of builtin methods
    ਪੂਰਨ		class_builtin_methods[CLASS_BUILTIN_MAX];

    garray_T	class_type_list;	// used for type pointers
    type_T	class_type;		// type used for the class
    type_T	class_object_type;	// same as class_type but VAR_OBJECT
};

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IS_INTERFACE(cl)	((cl)->class_flags & CLASS_INTERFACE)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IS_ENUM(cl)		((cl)->class_flags & CLASS_ENUM)

// Used for v_object of typval of VAR_OBJECT.
// The member variables follow in an array of typval_T.
ਬਣਤਰ object_S
{
    class_T	*obj_class;	    // class this object is created for;
				    // pointer adds to class_refcount
    ਪੂਰਨ		obj_refcount;

    object_T	*obj_ਅਗਲਾ_ਵਰਤਿਆ;	    // for list headed by "first_object"
    object_T	*obj_prev_ਵਰਤਿਆ;	    // for list headed by "first_object"
    ਪੂਰਨ		obj_copyID;	    // used by garbage collection
};

ਬਣਤਰ typealias_S
{
    ਪੂਰਨ	    ta_refcount;
    type_T  *ta_type;
    char_u  *ta_name;
    ਪੂਰਨ	    ta_ਡੂੰਘਾਈ;	    // number of levels of "ta_type"
};

/*
 * Structure to hold an internal variable without a name.
 */
ਬਣਤਰ typval_S
{
    vartype_T	v_type;
    ਅੱਖਰ	v_ਤਾਲਾ;	    // see below: VAR_LOCKED, VAR_FIXED
    ਸੰਘ
    {
	varnumber_T	v_ਸੰਖਿਆ;	// number value
	float_T		v_float;	// floating point number value
	char_u		*v_string;	// string value (can be NULL)
	list_T		*v_list;	// list value (can be NULL)
	dict_T		*v_dict;	// dict value (can be NULL)
	partial_T	*v_partial;	// closure: function with args
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
	ਕੰਮ_T		*v_ਕੰਮ;		// job value (can be NULL)
	channel_T	*v_channel;	// channel value (can be NULL)
#ਜੇ_ਅੰਤ
	blob_T		*v_blob;	// blob value (can be NULL)
	instr_T		*v_instr;	// instructions to execute
	class_T		*v_class;	// class value (can be NULL)
	object_T	*v_object;	// object value (can be NULL)
	typealias_T	*v_typealias;	// user-defined type name
	tuple_T		*v_tuple;	// tuple
    }		vval;
};

// Values for "dv_scope".
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VAR_SCOPE     1	// a:, v:, s:, etc. scope dictionaries
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VAR_DEF_SCOPE 2	// l:, g: scope dictionaries: here funcrefs are not
			// allowed to mask existing functions

// Values for "v_lock".
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VAR_LOCKED	    1	// locked with lock(), can use unlock()
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VAR_FIXED	    2	// locked forever
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VAR_ਚੀਜ਼ਾਂ_LOCKED    4	// items of non-materialized list locked

/*
 * Structure to hold an item of a list: an internal variable without a name.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ listitem_S listitem_T;

ਬਣਤਰ listitem_S
{
    listitem_T	*li_ਅਗਲਾ;	// next item in list
    listitem_T	*li_prev;	// previous item in list
    typval_T	li_tv;		// type and value of the variable
};

// Struct used by those that are using an item in a list.
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ listwatch_S listwatch_T;

ਬਣਤਰ listwatch_S
{
    listitem_T		*lw_ਵਸਤੂ;	// item being watched
    listwatch_T		*lw_ਅਗਲਾ;	// next watcher
};

/*
 * Structure to hold info about a list.
 * Order of members is optimized to reduce padding.
 * When created by range() it will at first have special value:
 *  lv_first == &range_list_item;
 * and use lv_start, lv_end, lv_stride.
 */
ਬਣਤਰ listvar_S
{
    listitem_T	*lv_ਪਹਿਲਾ;	// first item, NULL if none, &range_list_item
				// for a non-materialized list
    listwatch_T	*lv_watch;	// first watcher, NULL if none
    ਸੰਘ {
	ਬਣਤਰ {	// used for non-materialized range list:
			// "lv_first" is &range_list_item
	    varnumber_T lv_ਸ਼ੁਰੂ;
	    varnumber_T lv_ਸਿਰਾ;
	    ਪੂਰਨ		lv_stride;
	} nonmat;
	ਬਣਤਰ {	// used for materialized list
	    listitem_T	*lv_ਆਖਰੀ;	// last item, NULL if none
	    listitem_T	*lv_idx_ਵਸਤੂ;	// when not NULL item at index "lv_idx"
	    ਪੂਰਨ		lv_idx;		// cached index of an item
	} mat;
    } lv_u;
    type_T	*lv_type;	// current type, allocated by alloc_type()
    list_T	*lv_copylist;	// copied list used by deepcopy()
    list_T	*lv_ਵਰਤਿਆ_ਅਗਲਾ;	// next list in used lists list
    list_T	*lv_ਵਰਤਿਆ_prev;	// previous list in used lists list
    ਪੂਰਨ		lv_refcount;	// reference count
    ਪੂਰਨ		lv_len;		// number of items
    ਪੂਰਨ		lv_with_ਚੀਜ਼ਾਂ;	// number of items following this struct that
				// should not be freed
    ਪੂਰਨ		lv_copyID;	// ID used by deepcopy()
    ਅੱਖਰ	lv_ਤਾਲਾ;	// zero, VAR_LOCKED, VAR_FIXED
};

/*
 * Static list with 10 items.  Use init_static_list() to initialize.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    list_T	sl_list;	// must be first
    listitem_T	sl_ਚੀਜ਼ਾਂ[10];
} staticList10_T;

/*
 * Structure to hold an item of a Dictionary.
 * Also used for a variable.
 * The key is copied into "di_key" to avoid an extra alloc/free for it.
 */
ਬਣਤਰ dictitem_S
{
    typval_T	di_tv;		// type and value of the variable
    char_u	di_flags;	// DI_FLAGS_ flags (only used for variable)
    char_u	di_ਕੁੰਜੀ[1];	// key (actually longer!)
};
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ dictitem_S dictitem_T;

/*
 * A dictitem with a 16 character key (plus NUL).  This is an efficient way to
 * have a fixed-size dictitem.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DICTITEM16_ਕੁੰਜੀ_LEN 16
ਬਣਤਰ dictitem16_S
{
    typval_T	di_tv;		// type and value of the variable
    char_u	di_flags;	// DI_FLAGS_ flags (only used for variable)
    char_u	di_ਕੁੰਜੀ[DICTITEM16_ਕੁੰਜੀ_LEN + 1];	//ਕੁੰਜੀ
};
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ dictitem16_S dictitem16_T;

// Flags for "di_flags"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DI_FLAGS_RO	   0x01	    // read-only variable
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DI_FLAGS_RO_SBX	   0x02	    // read-only in the sandbox
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DI_FLAGS_FIX	   0x04	    // fixed: no :unlet or remove()
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DI_FLAGS_ਤਾਲਾ	   0x08	    // locked variable
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DI_FLAGS_ALLOC	   0x10	    // separately allocated
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DI_FLAGS_RELOAD	   0x20	    // set when script sourced again

/*
 * Structure to hold info about a Dictionary.
 */
ਬਣਤਰ dictvar_S
{
    ਅੱਖਰ	dv_ਤਾਲਾ;	// zero, VAR_LOCKED, VAR_FIXED
    ਅੱਖਰ	dv_scope;	// zero, VAR_SCOPE, VAR_DEF_SCOPE
    ਪੂਰਨ		dv_refcount;	// reference count
    ਪੂਰਨ		dv_copyID;	// ID used by deepcopy()
    hashtab_T	dv_hashtab;	// hashtab that refers to the items
    type_T	*dv_type;	// current type, allocated by alloc_type()
    dict_T	*dv_copydict;	// copied dict used by deepcopy()
    dict_T	*dv_ਵਰਤਿਆ_ਅਗਲਾ;	// next dict in used dicts list
    dict_T	*dv_ਵਰਤਿਆ_prev;	// previous dict in used dicts list
};

/*
 * Structure to hold info about a blob.
 */
ਬਣਤਰ blobvar_S
{
    garray_T	bv_ga;		// growarray with the data
    ਪੂਰਨ		bv_refcount;	// reference count
    ਅੱਖਰ	bv_ਤਾਲਾ;	// zero, VAR_LOCKED, VAR_FIXED
};

/*
 * Structure to hold info about a tuple.
 */
ਬਣਤਰ tuplevar_S
{
    garray_T	tv_ਚੀਜ਼ਾਂ;	// tuple items
    type_T	*tv_type;	// current type, allocated by alloc_type()
    tuple_T	*tv_copytuple;	// copied tuple used by deepcopy()
    tuple_T	*tv_ਵਰਤਿਆ_ਅਗਲਾ;	// next tuple in used tuples list
    tuple_T	*tv_ਵਰਤਿਆ_prev;	// previous tuple in used tuples list
    ਪੂਰਨ		tv_refcount;	// reference count
    ਪੂਰਨ		tv_copyID;	// ID used by deepcopy()
    ਅੱਖਰ	tv_ਤਾਲਾ;	// zero, VAR_LOCKED, VAR_FIXED
};

/*
 * Structure to hold a generic type information
 */
ਬਣਤਰ generictype_S
{
    type_T	*gt_type;	// generic or concrete type
    char_u	*gt_name;	// type name
};

/*
 * Generic function args table
 */
ਬਣਤਰ gfargs_tab_S
{
    garray_T	gfat_ਦਲੀਲਾਂ;
    garray_T	gfat_param_types;
    garray_T	gfat_arg_types;
};

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ (*cfunc_T)(ਪੂਰਨ argcount, typval_T *argvars, typval_T *rettv, ਖਾਲੀ *state);
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਖਾਲੀ (*cfunc_free_T)(ਖਾਲੀ *state);

// type of getline() last argument
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
    GETLINE_NONE,	    // do not concatenate any lines
    GETLINE_CONCAT_CONT,    // concatenate continuation lines with backslash
    GETLINE_CONCAT_CONTBAR, // concatenate continuation lines with \ and |
    GETLINE_CONCAT_ALL	    // concatenate continuation and Vim9 # comment lines
} getline_opt_T;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ svar_S svar_T;

#ਜੇ defined(FEAT_EVAL)
/*
 * Info used by a ":for" loop.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    ਪੂਰਨ		fi_semicolon;	// TRUE if ending in '; var]'
    ਪੂਰਨ		fi_varcount;	// nr of variables in [] or zero
    ਪੂਰਨ		fi_break_ਗਿਣਤੀ;	// nr of line breaks encountered
    listwatch_T	fi_lw;		// keep an eye on the item used.
    list_T	*fi_list;	// list being used
    ਪੂਰਨ		fi_bi;		// index of blob
    blob_T	*fi_blob;	// blob being used
    char_u	*fi_string;	// copy of string being used
    ਪੂਰਨ		fi_byte_idx;	// byte index in fi_string
    tuple_T	*fi_tuple;	// tuple being used
    ਪੂਰਨ		fi_tuple_idx;	// tuple index in fi_tuple
    ਪੂਰਨ		fi_cs_flags;	// cs_flags or'ed together
} forinfo_T;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ funccall_S funccall_T;

// values used for "uf_def_status"
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
    UF_NOT_COMPILED,	    // executed with interpreter
    UF_ਮੰਜ਼ਿਲ_BE_COMPILED,	    // to be compiled before execution
    UF_COMPILING,	    // in compile_def_function()
    UF_COMPILED,	    // successfully compiled
    UF_COMPILE_ERROR	    // compilation error, cannot execute
} def_ਸਥਿਤੀ_T;

/*
 * Structure to hold info for a user function.
 * When adding a field check copy_lambda_to_global_func().
 */
ਬਣਤਰ ufunc_S
{
    ਪੂਰਨ		uf_varargs;	// variable nr of arguments (old style)
    ਪੂਰਨ		uf_flags;	// FC_ flags
    ਪੂਰਨ		uf_calls;	// nr of active calls
    ਪੂਰਨ		uf_cleared;	// func_clear() was already called
    def_ਸਥਿਤੀ_T uf_def_ਸਥਿਤੀ; // UF_NOT_COMPILED, UF_TO_BE_COMPILED, etc.
    ਪੂਰਨ		uf_dfunc_idx;	// only valid if uf_def_status is UF_COMPILED

    class_T	*uf_class;	// for class/object method and constructor;
				// does not count for class_refcount.
				// class of the object which is invoking this
				// function.
    class_T	*uf_defclass;	// class where this function is defined.

    garray_T	uf_ਦਲੀਲਾਂ;	// arguments, including optional arguments
    garray_T	uf_def_ਦਲੀਲਾਂ;	// default argument expressions
    ਪੂਰਨ		uf_ਦਲੀਲਾਂ_visible; // normally uf_args.ga_len, less when
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
    ਪੂਰਨ		uf_ਖੰਡ_ਡੂੰਘਾਈ;	// nr of entries in uf_block_ids
    ਪੂਰਨ		*uf_ਖੰਡ_ids;	// blocks a :def function is defined inside
# ਜੇ defined(FEAT_LUA)
    cfunc_T     uf_cb;		// callback function for cfunc
    cfunc_free_T uf_cb_free;    // callback function to free cfunc
    ਖਾਲੀ	*uf_cb_state;   // state of uf_cb
# endif

    // for generic functions
    ਪੂਰਨ		uf_generic_argcount;// type argument count
    generic_T	*uf_generic_ਦਲੀਲਾਂ;   // generic types
    type_T	*uf_generic_param_types; // list of allocated generic types
    garray_T	uf_generic_arg_types; // list of allocated type arguments
    hashtab_T	uf_generic_functab; // generic function table

    garray_T	uf_ਸਤਰਾਂ;	// function lines

    ਪੂਰਨ		uf_debug_tick;	// when last checked for a breakpoint in this
				// function.
    ਪੂਰਨ		uf_has_breakpoint;  // TRUE when a breakpoint has been set in
				    // this function.
# ifdef FEAT_PROFILE
    ਪੂਰਨ		uf_profiling;	// TRUE when func is being profiled
    ਪੂਰਨ		uf_prof_initialized;
    hash_T	uf_hash;	// hash for uf_name when profiling
    // profiling the function as a whole
    ਪੂਰਨ		uf_tm_ਗਿਣਤੀ;	// nr of calls
    proftime_T	uf_tm_ਕੁੱਲ;	// time spent in function + children
    proftime_T	uf_tm_self;	// time spent in function itself
    proftime_T	uf_tm_children;	// time spent in children this call
    // profiling the function per line
    ਪੂਰਨ		*uf_tml_ਗਿਣਤੀ;	// nr of times line was executed
    proftime_T	*uf_tml_ਕੁੱਲ;	// time spent in a line + children
    proftime_T	*uf_tml_self;	// time spent in a line itself
    proftime_T	uf_tml_ਸ਼ੁਰੂ;	// start time for current line
    proftime_T	uf_tml_children; // time spent in children for this line
    proftime_T	uf_tml_wait;	// start wait time for current line
    ਪੂਰਨ		uf_tml_idx;	// index of line being timed; -1 if none
    ਪੂਰਨ		uf_tml_execed;	// line being timed was executed
# endif
    sctx_T	uf_script_ctx;	// SCTX where function was defined,
				// used for s: variables; sc_version changed
				// for :function
    ਪੂਰਨ		uf_script_ctx_ਸੰਸਕਰਣ;  // original sc_version of SCTX
    ਪੂਰਨ		uf_refcount;	// reference count, see func_name_refcount()

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
# define FC_ਘੇਰਾ  0x08	// closure, uses outer scope variables
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
# define IS_OBJECT_ਵਿਧੀ(ufunc) ((ufunc->uf_flags & FC_OBJECT) == FC_OBJECT)
// Is "ufunc" a class new() constructor method?
# define IS_CONSTRUCTOR_ਵਿਧੀ(ufunc) ((ufunc->uf_flags & FC_NEW) == FC_NEW)
// Is "ufunc" an abstract class method?
# define IS_ABSTRACT_ਵਿਧੀ(ufunc) ((ufunc->uf_flags & FC_ABSTRACT) == FC_ABSTRACT)
# define IS_GENERIC_FUNC(ufunc) (((ufunc)->uf_flags & FC_GENERIC) == FC_GENERIC)

# define MAX_FUNC_ਦਲੀਲਾਂ	20	// maximum number of function arguments
# define VAR_SHORT_LEN	20	// short variable name length
# define FIXVAR_CNT	12	// number of fixed variables

/*
 * Structure to hold info for a function that is currently being executed.
 */
ਬਣਤਰ funccall_S
{
    ufunc_T	*fc_func;	// function being called
    ਪੂਰਨ		fc_linenr;	// next line to be executed
    ਪੂਰਨ		fc_returned;	// ":return" used
    ਬਣਤਰ			// fixed variables for arguments
    {
	dictitem_T	var;		// variable (without room for name)
	char_u	ਗੁੰਜਾਇਸ਼[VAR_SHORT_LEN];	// room for the name
    } fc_fixvar[FIXVAR_CNT];
    dict_T	fc_l_vars;	// l: local function variables
    dictitem_T	fc_l_vars_var;	// variable for l: scope
    dict_T	fc_l_avars;	// a: argument variables
    dictitem_T	fc_l_avars_var;	// variable for a: scope
    list_T	fc_l_varlist;	// list for a:000
    listitem_T	fc_l_listitems[MAX_FUNC_ਦਲੀਲਾਂ];	// listitems for a:000
    typval_T	*fc_rettv;	// return value
    linenr_T	fc_breakpoint;	// next line with breakpoint or zero
    ਪੂਰਨ		fc_dbg_tick;	// debug_tick when breakpoint was set
    ਪੂਰਨ		fc_level;	// top nesting level of executed function

    garray_T	fc_defer;	// functions to be called on return
    ectx_T	*fc_ectx;	// execution context for :def function, NULL
				// otherwise

# ifdef FEAT_PROFILE
    proftime_T	fc_prof_ਬੱਚਾ;	// time spent in a child
# endif
    funccall_T	*fc_caller;	// calling function or NULL; or next funccal in
				// list pointed to by previous_funccal.

    // for closure
    ਪੂਰਨ		fc_refcount;	// number of user functions that reference this
				// funccal
    ਪੂਰਨ		fc_copyID;	// for garbage collection
    garray_T	fc_ufuncs;	// list of ufunc_T* which keep a reference to
				// "fc_func"
};

// structure used as item in "fc_defer"
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    char_u	*dr_name;	// function name, allocated
    typval_T	dr_argvars[MAX_FUNC_ਦਲੀਲਾਂ + 1];
    ਪੂਰਨ		dr_argcount;
} defer_T;

/*
 * Struct used by trans_function_name()
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    dict_T	*fd_dict;	// Dictionary used
    char_u	*fd_newkey;	// new key in "dict" in allocated memory
    dictitem_T	*fd_di;		// Dictionary item used
} funcdict_T;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ funccal_ਇੰਦਰਾਜ funccal_ਇੰਦਰਾਜ_T;
ਬਣਤਰ funccal_ਇੰਦਰਾਜ {
    ਖਾਲੀ	    *ਸਿਖਰ_funccal;
    funccal_ਇੰਦਰਾਜ_T *ਅਗਲਾ;
};

// From user function to hashitem and back.
# define UF2HIKEY(fp) ((fp)->uf_name)
# define HIKEY2UF(p)  ((ufunc_T *)((p) - offsetof(ufunc_T, uf_name)))
# define HI2UF(hi)     HIKEY2UF((hi)->hi_ਕੁੰਜੀ)

/*
 * Holds the hashtab with variables local to each sourced script.
 * Each item holds a variable (nameless) that points to the dict_T.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    dictitem_T	sv_var;
    dict_T	sv_dict;
} scriptvar_T;

/*
 * Entry for "sn_all_vars".  Contains the s: variables from sn_vars plus the
 * block-local ones.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ sallvar_S sallvar_T;
ਬਣਤਰ sallvar_S {
    sallvar_T	*sav_ਅਗਲਾ;	  // var with same name but different block
    ਪੂਰਨ		sav_ਖੰਡ_id;	  // block ID where declared
    ਪੂਰਨ		sav_var_vals_idx; // index in sn_var_vals

    // So long as the variable is valid (block it was defined in is still
    // active) "sav_di" is used.  It is set to NULL when leaving the block,
    // then sav_tv and sav_flags are used.
    dictitem_T *sav_di;		// dictitem with di_key and di_tv
    typval_T	sav_tv;		// type and value of the variable
    char_u	sav_flags;	// DI_FLAGS_ flags (only used for variable)
    char_u	sav_ਕੁੰਜੀ[1];	// key (actually longer!)
};

/*
 * In the sn_all_vars hashtab item "hi_key" points to "sav_key" in a sallvar_T.
 * This makes it possible to store and find the sallvar_T.
 * SAV2HIKEY() converts a sallvar_T pointer to a hashitem key pointer.
 * HIKEY2SAV() converts a hashitem key pointer to a sallvar_T pointer.
 * HI2SAV() converts a hashitem pointer to a sallvar_T pointer.
 */
# define SAV2HIKEY(sav) ((sav)->sav_ਕੁੰਜੀ)
# define HIKEY2SAV(p)  ((sallvar_T *)(p - offsetof(sallvar_T, sav_ਕੁੰਜੀ)))
# define HI2SAV(hi)     HIKEY2SAV((hi)->hi_ਕੁੰਜੀ)

# define SVFLAG_TYPE_ALLOCATED	1  // call free_type() for "sv_type"
# define SVFLAG_EXPORTED		2  // "export let var = val"
# define SVFLAG_ASSIGNED		4  // assigned a value

/*
 * Entry for "sn_var_vals".  Used for script-local variables.
 */
ਬਣਤਰ svar_S {
    char_u	*sv_name;	// points into "sn_all_vars" di_key
    typval_T	*sv_tv;		// points into "sn_vars" or "sn_all_vars" di_tv
    type_T	*sv_type;
    ਪੂਰਨ		sv_flags;	// SVFLAG_ values above
    ਪੂਰਨ		sv_const;	// 0, ASSIGN_CONST or ASSIGN_FINAL
};

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    char_u	*imp_name;	    // name imported as (allocated)
    scid_T	imp_sid;	    // script ID of "from"
    ਪੂਰਨ		imp_flags;	    // IMP_FLAGS_ values
} imported_T;

# define IMP_FLAGS_RELOAD	2   // script reloaded, OK to redefine
# define IMP_FLAGS_AUTOLOAD	4   // script still needs to be loaded

/*
 * Info about an encountered script.
 * When sn_state has SN_STATE_NOT_LOADED, it has not been sourced yet.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    char_u	*sn_name;	    // full path of script file
    ਪੂਰਨ		sn_script_seq;	    // latest sctx_T sc_seq value

    // When non-zero the script ID of the actually sourced script.  Used if a
    // script is used by a name which has a symlink, we list both names, but
    // only the linked-to script is actually sourced.
    ਪੂਰਨ		sn_sourced_sid;

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
    ਪੂਰਨ		sn_current_ਖੰਡ_id; // ID for current block, 0 for outer
    ਪੂਰਨ		sn_ਆਖਰੀ_ਖੰਡ_id;  // Unique ID for each script block

    ਪੂਰਨ		sn_ਸੰਸਕਰਣ;	// :scriptversion
    ਪੂਰਨ		sn_state;	// SN_STATE_ values
    char_u	*sn_save_cpo;	// 'cpo' value when :vim9script found
    ਅੱਖਰ	sn_is_vimrc;	// .vimrc file, do not restore 'cpo'
    ਅੱਖਰ	sn_syml_checked;// flag: this has been checked for sym link

    // for a Vim9 script under "rtp/autoload/" this is "dir#scriptname#"
    char_u	*sn_autoload_ਅਗੇਤਰ;

    // TRUE for a script used with "import autoload './dirname/script.vim'"
    // For "../autoload/script.vim" sn_autoload_prefix is also set.
    ਪੂਰਨ		sn_import_autoload;

# ifdef FEAT_PROFILE
    ਪੂਰਨ		sn_prof_on;	// TRUE when script is/was profiled
    ਪੂਰਨ		sn_pr_force;	// forceit: profile functions in this script
    proftime_T	sn_pr_ਬੱਚਾ;	// time set when going into first child
    ਪੂਰਨ		sn_pr_nest;	// nesting for sn_pr_child
    // profiling the script as a whole
    ਪੂਰਨ		sn_pr_ਗਿਣਤੀ;	// nr of times sourced
    proftime_T	sn_pr_ਕੁੱਲ;	// time spent in script + children
    proftime_T	sn_pr_self;	// time spent in script itself
    proftime_T	sn_pr_ਸ਼ੁਰੂ;	// time at script start
    proftime_T	sn_pr_children; // time in children after script start
    // profiling the script per line
    garray_T	sn_prl_ga;	// things stored for every line
    proftime_T	sn_prl_ਸ਼ੁਰੂ;	// start time for current line
    proftime_T	sn_prl_children; // time spent in children for this line
    proftime_T	sn_prl_wait;	// wait start time for current line
    ਪੂਰਨ		sn_prl_idx;	// index of line being timed; -1 if none
    ਪੂਰਨ		sn_prl_execed;	// line being timed was executed
# endif
} scriptitem_T;

# define SN_STATE_NEW		0   // newly loaded script, nothing done
# define SN_STATE_NOT_LOADED	1   // script located but not loaded
# define SN_STATE_RELOAD		2   // script loaded before, nothing done
# define SN_STATE_HAD_COMMAND	9   // a command was executed

// Struct passed through eval() functions.
// See EVALARG_EVALUATE for a fixed value with eval_flags set to EVAL_EVALUATE.
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    ਪੂਰਨ		eval_flags;	    // EVAL_ flag values below
    ਪੂਰਨ		eval_break_ਗਿਣਤੀ;   // nr of line breaks consumed

    // copied from exarg_T when "getline" is "getsourceline". Can be NULL.
    char_u	*(*eval_getline)(ਪੂਰਨ, ਖਾਲੀ *, ਪੂਰਨ, getline_opt_T);
    ਖਾਲੀ	*eval_cookie;	    // argument for eval_getline()

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
    ਪੂਰਨ		eval_using_cmdline;

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
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ sn_prl_S
{
    ਪੂਰਨ		snp_ਗਿਣਤੀ;	// nr of times line was executed
    proftime_T	sn_prl_ਕੁੱਲ;	// time spent in a line + children
    proftime_T	sn_prl_self;	// time spent in a line itself
} sn_prl_T;

#  define PRL_ਵਸਤੂ(si, idx)	(((sn_prl_T *)(si)->sn_prl_ga.ga_ਅੰਕੜੇ)[(idx)])

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    ਪੂਰਨ		pi_started_profiling;
    proftime_T	pi_wait_ਸ਼ੁਰੂ;
    proftime_T	pi_call_ਸ਼ੁਰੂ;
} profinfo_T;

# ਨਹੀਂਤਾਂ
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    ਪੂਰਨ	    dummy;
} profinfo_T;
# endif
#ਨਹੀਂਤਾਂ
// dummy typedefs for use in function prototypes
ਬਣਤਰ ufunc_S
{
    ਪੂਰਨ	    dummy;
};
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    ਪੂਰਨ	    dummy;
} funccall_T;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    ਪੂਰਨ	    dummy;
} funcdict_T;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    ਪੂਰਨ	    dummy;
} funccal_ਇੰਦਰਾਜ_T;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    ਪੂਰਨ	    dummy;
} scriptitem_T;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    ਪੂਰਨ	    dummy;
} evalarg_T;
#ਜੇ_ਅੰਤ

// Struct passed between functions dealing with function call execution.
//
// "fe_argv_func", when not NULL, can be used to fill in arguments only when the
// invoked function uses them.  It is called like this:
//   new_argcount = fe_argv_func(current_argcount, argv, partial_argcount,
//							called_func)
//
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    ਪੂਰਨ		(* fe_argv_func)(ਪੂਰਨ, typval_T *, ਪੂਰਨ, ufunc_T *);
    linenr_T	fe_firstline;	// first line of range
    linenr_T	fe_lastline;	// last line of range
    ਪੂਰਨ		*fe_doesrange;	// if not NULL: return: function handled range
    ਪੂਰਨ		fe_evaluate;	// actually evaluate expressions
    ufunc_T	*fe_ufunc;	// function to be called, when NULL lookup by
				// name
    partial_T	*fe_partial;	// for "dict" and extra arguments
    dict_T	*fe_selfdict;	// Dictionary for "self"
    object_T	*fe_object;	// object, e.g. for "this.Func()"
    typval_T	*fe_basetv;	// base for base->method()
    type_T	*fe_check_type;	// type from funcref or NULL
    ਪੂਰਨ		fe_ਮਿਲਿਆ_var;	// if the function is not found then give an
				// error that a variable is not callable.
    cctx_T	*fe_cctx;	// when compiling a :def function
} funcexe_T;

/*
 * Structure to hold the context of a compiled function, used by closures
 * defined in that function.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ funcstack_S funcstack_T;
ਬਣਤਰ funcstack_S
{
    funcstack_T *fs_ਅਗਲਾ;	// linked list at "first_funcstack"
    funcstack_T *fs_prev;

    garray_T	fs_ga;		// contains the stack, with:
				// - arguments
				// - frame
				// - local variables
    ਪੂਰਨ		fs_var_ਵਿੱਥ;	// count of arguments + frame size == offset to
				// local variables

    ਪੂਰਨ		fs_refcount;	// nr of closures referencing this funcstack
    ਪੂਰਨ		fs_min_refcount; // nr of closures on this funcstack
    ਪੂਰਨ		fs_copyID;	// for garbage collection
};

/*
 * Structure to hold the variables declared in a loop that are possibly used
 * in a closure.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ loopvars_S loopvars_T;
ਬਣਤਰ loopvars_S
{
    loopvars_T *lvs_ਅਗਲਾ;	// linked list at "first_loopvars"
    loopvars_T *lvs_prev;

    garray_T	lvs_ga;		// contains the variables
    ਪੂਰਨ		lvs_refcount;	// nr of closures referencing this loopvars
    ਪੂਰਨ		lvs_min_refcount; // nr of closures on this loopvars
    ਪੂਰਨ		lvs_copyID;	// for garbage collection
};

// maximum nesting of :while and :for loops in a :def function
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_LOOP_ਡੂੰਘਾਈ 10

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ outer_S outer_T;
ਬਣਤਰ outer_S {
    garray_T	*ਬਾਹਰਲਾ_ਢੇਰ;	    // stack from outer scope, or a copy
				    // containing only arguments and local vars
    ਪੂਰਨ		ਬਾਹਰਲਾ_ਚੌਖਟਾ_idx;	    // index of stack frame in out_stack
    outer_T	*ਬਾਹਰਲਾ_ਉੱਪਰ;	    // outer scope of outer scope or NULL
    partial_T	*ਬਾਹਰਲਾ_ਉੱਪਰ_partial;    // partial owning out_up or NULL

    ਬਣਤਰ {
	garray_T *ਢੇਰ;	    // stack from outer scope, or a copy
				    // containing only vars inside the loop
	ਛੋਟਾ	 var_idx;	    // first variable defined in a loop in
				    // out_loop_stack
	ਛੋਟਾ	 var_ਗਿਣਤੀ;	    // number of variables defined in a loop
    } ਬਾਹਰਲਾ_loop[MAX_LOOP_ਡੂੰਘਾਈ];
    ਪੂਰਨ		ਬਾਹਰਲਾ_loop_ਮਾਪ;	    // nr of used entries in out_loop[]
};

ਬਣਤਰ partial_S
{
    ਪੂਰਨ		pt_refcount;	// reference count
    ਪੂਰਨ		pt_auto;	// when TRUE the partial was created for using
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
    loopvars_T	*(pt_loopvars[MAX_LOOP_ਡੂੰਘਾਈ]);
				// copy of loop variables, used after loop
				// block ends

    typval_T	*pt_argv;	// arguments in allocated array
    ਪੂਰਨ		pt_argc;	// number of arguments

    ਪੂਰਨ		pt_copyID;	// funcstack may contain pointer to partial
    dict_T	*pt_dict;	// dict for "self"
    object_T	*pt_obj;	// object method
};

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    ਛੋਟਾ	lvi_ਡੂੰਘਾਈ;	    // current nested loop depth
    ਬਣਤਰ {
	ਛੋਟਾ	var_idx;	    // index of first variable inside loop
	ਛੋਟਾ	var_ਗਿਣਤੀ;	    // number of variables inside loop
    } lvi_loop[MAX_LOOP_ਡੂੰਘਾਈ];
} loopvarinfo_T;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ AutoPatCmd_S AutoPatCmd_T;

/*
 * Entry in the execution stack "exestack".
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
    ETYPE_ਸਿਖਰ,		    // toplevel
    ETYPE_SCRIPT,	    // sourcing script, use es_info.sctx
    ETYPE_UFUNC,	    // user function, use es_info.ufunc
    ETYPE_AUCMD,	    // autocomand, use es_info.aucmd
    ETYPE_MODELINE,	    // modeline, use es_info.sctx
    ETYPE_EXCEPT,	    // exception, use es_info.exception
    ETYPE_ਦਲੀਲਾਂ,		    // command line argument
    ETYPE_ENV,		    // environment variable
    ETYPE_ਅੰਦਰੂਨੀ,	    // internal operation
    ETYPE_SPELL,	    // loading spell file
} etype_T;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    ਲੰਮਾ      es_lnum;      // replaces "sourcing_lnum"
    char_u    *es_name;     // replaces "sourcing_name"
    etype_T   es_type;
    ਸੰਘ {
	sctx_T  *sctx;      // script and modeline info
#ਜੇ defined(FEAT_EVAL)
	ufunc_T *ufunc;     // function info
#ਜੇ_ਅੰਤ
	AutoPatCmd_T *aucmd;  // autocommand info
	except_T   *except; // exception info
    } es_info;
#ਜੇ defined(FEAT_EVAL)
    sctx_T	es_save_sctx;	    // saved current_sctx when calling function
#ਜੇ_ਅੰਤ
} estack_T;

// Information returned by get_tty_info().
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    ਪੂਰਨ backspace;	// what the Backspace key produces
    ਪੂਰਨ enter;		// what the Enter key produces
    ਪੂਰਨ ਰੁਕਾਵਟ;	// interrupt character
    ਪੂਰਨ nl_does_cr;	// TRUE when a NL is expanded to CR-NL on output
} ttyinfo_T;

// Status of a job.  Order matters!
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ
{
    ਕੰਮ_FAILED,
    ਕੰਮ_STARTED,
    ਕੰਮ_ENDED,	    // detected job done
    ਕੰਮ_ਮੁਕੰਮਲ,   // job done and cleanup done
} jobstatus_T;

/*
 * Structure to hold info about a Job.
 */
ਬਣਤਰ jobvar_S
{
    ਕੰਮ_T	*jv_ਅਗਲਾ;
    ਕੰਮ_T	*jv_prev;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
    pid_t	jv_pid;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
    PROCESS_INFORMATION	jv_proc_info;
    HANDLE		jv_ਕੰਮ_object;
#ਜੇ_ਅੰਤ
    jobstatus_T	jv_ਸਥਿਤੀ;
    char_u	*jv_tty_ਵਿੱਚ;	// controlling tty input, allocated
    char_u	*jv_tty_ਬਾਹਰਲਾ;	// controlling tty output, allocated
    char_u	*jv_stoponexit;	// allocated
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
    char_u	*jv_termsig;	// allocated
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
    char_u	*jv_tty_type;	// allocated
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		jv_exitval;
    callback_T	jv_exit_cb;

    buf_T	*jv_ਵਿੱਚ_buf;	// buffer from "in-name"

    ਪੂਰਨ		jv_refcount;	// reference count
    ਪੂਰਨ		jv_copyID;

    channel_T	*jv_channel;	// channel for I/O, reference counted
    ਅੱਖਰ	**jv_argv;	// command line used to start the job
};

/*
 * Structures to hold info about a Channel.
 */
ਬਣਤਰ readq_S
{
    char_u	*rq_ਬਫਰ;
    long_u	rq_buflen;
    readq_T	*rq_ਅਗਲਾ;
    readq_T	*rq_prev;
};

ਬਣਤਰ writeq_S
{
    garray_T	wq_ga;
    writeq_T	*wq_ਅਗਲਾ;
    writeq_T	*wq_prev;
};

ਬਣਤਰ jsonq_S
{
    typval_T	*jq_ਮੁੱਲ;
    jsonq_T	*jq_ਅਗਲਾ;
    jsonq_T	*jq_prev;
    ਪੂਰਨ		jq_no_callback; // TRUE when no callback was found
};

ਬਣਤਰ cbq_S
{
    callback_T	cq_callback;
    ਪੂਰਨ		cq_seq_nr;
    cbq_T	*cq_ਅਗਲਾ;
    cbq_T	*cq_prev;
};

// mode for a channel
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ
{
    CH_ਬਹੁਲਕ_NL = 0,
    CH_ਬਹੁਲਕ_RAW,
    CH_ਬਹੁਲਕ_BLOB,
    CH_ਬਹੁਲਕ_JSON,
    CH_ਬਹੁਲਕ_JS,
    CH_ਬਹੁਲਕ_LSP,	// Language Server Protocol (http + json)
    CH_ਬਹੁਲਕ_DAP		// Debug Adapter Protocol (like LSP, but does not
			// strictly follow JSON-RPC standard)
} ch_ਬਹੁਲਕ_T;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
    JIO_PIPE,	    // default
    JIO_NULL,
    JIO_FILE,
    JIO_ਬਫਰ,
    JIO_ਬਾਹਰਲਾ
} ਕੰਮ_io_T;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CH_PART_FD(part)	ch_part[part].ch_fd

// Ordering matters, it is used in for loops: IN is last, only SOCK/OUT/ERR
// are polled.
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
    PART_SOCK = 0,
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CH_SOCK_FD	CH_PART_FD(PART_SOCK)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
    PART_ਬਾਹਰਲਾ,
# define CH_ਬਾਹਰਲਾ_FD	CH_PART_FD(PART_ਬਾਹਰਲਾ)
    PART_ERR,
# define CH_ERR_FD	CH_PART_FD(PART_ERR)
    PART_ਵਿੱਚ,
# define CH_ਵਿੱਚ_FD	CH_PART_FD(PART_ਵਿੱਚ)
#ਜੇ_ਅੰਤ
    PART_ਗਿਣਤੀ,
} ch_part_T;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ INVALID_FD	(-1)

// The per-fd info for a channel.
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    sock_T	ch_fd;	    // socket/stdin/stdout/stderr, -1 if not used

#ਜੇ defined(UNIX) && !defined(HAVE_SELECT)
    ਪੂਰਨ		ch_poll_idx;	// used by channel_poll_setup()
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_X11
    Xt_ਨਿਵੇਸ਼_Id	ch_ਨਿਵੇਸ਼_Handler; // Cookie for input
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_GTK
    gint	ch_ਨਿਵੇਸ਼_Handler; // Cookie for input
#ਜੇ_ਅੰਤ

    ch_ਬਹੁਲਕ_T	ch_ਬਹੁਲਕ;
    ਕੰਮ_io_T	ch_io;
    ਪੂਰਨ		ch_timeout;	// request timeout in msec

    readq_T	ch_ਸਿਰ;	// header for circular raw read queue
    jsonq_T	ch_json_ਸਿਰ;	// header for circular json read queue
    garray_T	ch_ਖੰਡ_ids;	// list of IDs that channel_read_json_block()
				// is waiting for
    // When ch_wait_len is non-zero use ch_deadline to wait for incomplete
    // message to be complete. The value is the length of the incomplete
    // message when the deadline was set.  If it gets longer (something was
    // received) the deadline is reset.
    size_t	ch_wait_len;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
    DWORD	ch_deadline;
#ਨਹੀਂਤਾਂ
    ਬਣਤਰ timeval ch_deadline;
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		ch_ਖੰਡ_write;	// for testing: 0 when not used, -1 when write
				// does not block, 1 simulate blocking
    ਪੂਰਨ		ch_nonblocking;	// write() is non-blocking
    writeq_T	ch_writeque;	// header for write queue

    cbq_T	ch_cb_ਸਿਰ;	// dummy node for per-request callbacks
    callback_T	ch_callback;	// call when a msg is not handled

    bufref_T	ch_bufref;	// buffer to read from or write to
    ਪੂਰਨ		ch_nomodifiable; // TRUE when buffer can be 'nomodifiable'
    ਪੂਰਨ		ch_nomod_error;	// TRUE when e_modifiable was given
    ਪੂਰਨ		ch_buf_append;	// write appended lines instead top-bot
    linenr_T	ch_buf_ਸਿਖਰ;	// next line to send
    linenr_T	ch_buf_bot;	// last line to send
} chanpart_T;

ਬਣਤਰ channel_S {
    channel_T	*ch_ਅਗਲਾ;
    channel_T	*ch_prev;

    ਪੂਰਨ		ch_id;		// ID of the channel
    ਪੂਰਨ		ch_ਆਖਰੀ_ਸੁਨੇਹਾ_id;	// ID of the last message

    chanpart_T	ch_part[PART_ਗਿਣਤੀ]; // info for socket, out, err and in
    ਪੂਰਨ		ch_write_ਪਾਠ_ਬਹੁਲਕ; // write buffer lines with CR, not NL

    ਅੱਖਰ	*ch_hostname;	// only for socket, allocated
    ਪੂਰਨ		ch_ਪੋਰਟ;	// only for socket

    ਪੂਰਨ		ch_ਮੰਜ਼ਿਲ_be_closed; // bitset of readable fds to be closed.
				 // When all readable fds have been closed,
				 // set to (1 << PART_COUNT).
    ਪੂਰਨ		ch_ਮੰਜ਼ਿਲ_be_freed; // When TRUE channel must be freed when it's
				// safe to invoke callbacks.
    ਪੂਰਨ		ch_error;	// When TRUE an error was reported.  Avoids
    // giving pages full of error messages when
    // the other side has exited, only mention the
    // first error until the connection works
    // again.
    ਪੂਰਨ		ch_listen;	// When TRUE channel is listen socket.

    ਖਾਲੀ	(*ch_nb_close_cb)(ਖਾਲੀ);
				// callback for Netbeans when channel is
				// closed
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SOCKETSERVER
    bool	ch_socketserver; // If channel is used by socketserver
    ਖਾਲੀ	(*ch_ss_close_cb)(channel_T *);
    ਖਾਲੀ	(*ch_ss_accept_cb)(channel_T *);
    channel_T	*ch_ss_ਅਗਲਾ;
    channel_T	*ch_ss_prev;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
    ਪੂਰਨ		ch_named_pipe;	// using named pipe instead of pty
#ਜੇ_ਅੰਤ
    callback_T	ch_callback;	// call when any msg is not handled
    callback_T	ch_close_cb;	// call when channel is closed
    ਪੂਰਨ		ch_drop_never;
    ਪੂਰਨ		ch_keep_open;	// do not close on read error
    ਪੂਰਨ		ch_nonblock;

    ਕੰਮ_T	*ch_ਕੰਮ;	// Job that uses this channel; this does not
				// count as a reference to avoid a circular
				// reference, the job refers to the channel.
    ਪੂਰਨ		ch_ਕੰਮ_killed;	// TRUE when there was a job and it was killed
				// or we know it died.
    ਪੂਰਨ		ch_anonymous_pipe;  // Indicates that anonymous pipes are being
				    // used for communication in the Windows
				    // ConPTY terminal.
    ਪੂਰਨ		ch_killing;	    // Indicates that the job associated with
				    // the channel is terminating.  It becomes
				    // TRUE when TerminateJobObject() was
				    // called or the process associated with
				    // the job had exited (only ConPTY).

    ਪੂਰਨ		ch_refcount;	// reference count
    ਪੂਰਨ		ch_copyID;
};

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ JO_ਬਹੁਲਕ		    0x0001	// channel mode
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ JO_ਵਿੱਚ_ਬਹੁਲਕ	    0x0002	// stdin mode
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ JO_ਬਾਹਰਲਾ_ਬਹੁਲਕ	    0x0004	// stdout mode
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ JO_ERR_ਬਹੁਲਕ	    0x0008	// stderr mode
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ JO_CALLBACK	    0x0010	// channel callback
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ JO_ਬਾਹਰਲਾ_CALLBACK	    0x0020	// stdout callback
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ JO_ERR_CALLBACK	    0x0040	// stderr callback
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ JO_CLOSE_CALLBACK   0x0080	// "close_cb"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ JO_WAITTIME	    0x0100	// only for ch_open()
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ JO_TIMEOUT	    0x0200	// all timeouts
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ JO_ਬਾਹਰਲਾ_TIMEOUT	    0x0400	// stdout timeouts
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ JO_ERR_TIMEOUT	    0x0800	// stderr timeouts
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ JO_PART		    0x1000	// "part"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ JO_ID		    0x2000	// "id"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ JO_STOPONEXIT	    0x4000	// "stoponexit"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ JO_EXIT_CB	    0x8000	// "exit_cb"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ JO_ਬਾਹਰਲਾ_IO	    0x10000	// "out_io"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ JO_ERR_IO	    0x20000	// "err_io" (JO_OUT_IO << 1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ JO_ਵਿੱਚ_IO	    0x40000	// "in_io" (JO_OUT_IO << 2)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ JO_ਬਾਹਰਲਾ_NAME	    0x80000	// "out_name"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ JO_ERR_NAME	    0x100000	// "err_name" (JO_OUT_NAME << 1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ JO_ਵਿੱਚ_NAME	    0x200000	// "in_name" (JO_OUT_NAME << 2)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ JO_ਵਿੱਚ_ਸਿਖਰ	    0x400000	// "in_top"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ JO_ਵਿੱਚ_BOT	    0x800000	// "in_bot"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ JO_ਬਾਹਰਲਾ_BUF	    0x1000000	// "out_buf"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ JO_ERR_BUF	    0x2000000	// "err_buf" (JO_OUT_BUF << 1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ JO_ਵਿੱਚ_BUF	    0x4000000	// "in_buf" (JO_OUT_BUF << 2)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ JO_CHANNEL	    0x8000000	// "channel"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ JO_ਖੰਡ_WRITE	    0x10000000	// "block_write"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ JO_ਬਾਹਰਲਾ_MODIFIABLE   0x20000000	// "out_modifiable"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ JO_ERR_MODIFIABLE   0x40000000	// "err_modifiable" (JO_OUT_ << 1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ JO_ALL		    0x7fffffff

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ JO2_ਬਾਹਰਲਾ_ਸੁਨੇਹਾ	    0x0001	// "out_msg"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ JO2_ERR_ਸੁਨੇਹਾ	    0x0002	// "err_msg" (JO_OUT_ << 1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ JO2_ਪਦ_NAME	    0x0004	// "term_name"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ JO2_ਪਦ_ਅੰਤ	    0x0008	// "term_finish"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ JO2_ENV		    0x0010	// "env"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ JO2_CWD		    0x0020	// "cwd"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ JO2_ਪਦ_ਪੰਕਤੀਆਂ	    0x0040	// "term_rows"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ JO2_ਪਦ_ਖਾਨੇ_ਗਿਣਤੀ	    0x0080	// "term_cols"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ JO2_VERTICAL	    0x0100	// "vertical"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ JO2_CURWIN	    0x0200	// "curwin"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ JO2_HIDDEN	    0x0400	// "hidden"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ JO2_ਪਦ_OPENCMD    0x0800	// "term_opencmd"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ JO2_EOF_CHARS	    0x1000	// "eof_chars"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ JO2_NORESTORE	    0x2000	// "norestore"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ JO2_ਪਦ_KILL	    0x4000	// "term_kill"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ JO2_ANSI_COLORS	    0x8000	// "ansi_colors"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ JO2_TTY_TYPE	    0x10000	// "tty_type"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ JO2_BUFNR	    0x20000	// "bufnr"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ JO2_ਪਦ_API	    0x40000	// "term_api"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ JO2_ਪਦ_HIGHLIGHT  0x80000	// "highlight"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ JO_ਬਹੁਲਕ_ALL	(JO_ਬਹੁਲਕ + JO_ਵਿੱਚ_ਬਹੁਲਕ + JO_ਬਾਹਰਲਾ_ਬਹੁਲਕ + JO_ERR_ਬਹੁਲਕ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ JO_CB_ALL \
    (JO_CALLBACK + JO_ਬਾਹਰਲਾ_CALLBACK + JO_ERR_CALLBACK + JO_CLOSE_CALLBACK)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ JO_TIMEOUT_ALL	(JO_TIMEOUT + JO_ਬਾਹਰਲਾ_TIMEOUT + JO_ERR_TIMEOUT)

/*
 * Options for job and channel commands.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    ਪੂਰਨ		jo_ਸੈੱਟ;		// JO_ bits for values that were set
    ਪੂਰਨ		jo_set2;	// JO2_ bits for values that were set

    ch_ਬਹੁਲਕ_T	jo_ਬਹੁਲਕ;
    ch_ਬਹੁਲਕ_T	jo_ਵਿੱਚ_ਬਹੁਲਕ;
    ch_ਬਹੁਲਕ_T	jo_ਬਾਹਰਲਾ_ਬਹੁਲਕ;
    ch_ਬਹੁਲਕ_T	jo_err_ਬਹੁਲਕ;
    ਪੂਰਨ		jo_noblock;

    ਕੰਮ_io_T	jo_io[4];	// PART_OUT, PART_ERR, PART_IN
    char_u	jo_io_name_buf[4][NUMBUFLEN];
    char_u	*jo_io_name[4];	// not allocated!
    ਪੂਰਨ		jo_io_buf[4];
    ਪੂਰਨ		jo_pty;
    ਪੂਰਨ		jo_modifiable[4];
    ਪੂਰਨ		jo_ਸੁਨੇਹਾ[4];
    channel_T	*jo_channel;

    linenr_T	jo_ਵਿੱਚ_ਸਿਖਰ;
    linenr_T	jo_ਵਿੱਚ_bot;

    callback_T	jo_callback;
    callback_T	jo_ਬਾਹਰਲਾ_cb;
    callback_T	jo_err_cb;
    callback_T	jo_close_cb;
    callback_T	jo_exit_cb;
    ਪੂਰਨ		jo_drop_never;
    ਪੂਰਨ		jo_waittime;
    ਪੂਰਨ		jo_timeout;
    ਪੂਰਨ		jo_ਬਾਹਰਲਾ_timeout;
    ਪੂਰਨ		jo_err_timeout;
    ਪੂਰਨ		jo_ਖੰਡ_write;	// for testing only
    ਪੂਰਨ		jo_part;
    ਪੂਰਨ		jo_id;
    char_u	jo_stoponexit_buf[NUMBUFLEN];
    char_u	*jo_stoponexit;
    dict_T	*jo_env;	// environment variables
    char_u	jo_cwd_buf[NUMBUFLEN];
    char_u	*jo_cwd;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
    // when non-zero run the job in a terminal window of this size
    ਪੂਰਨ		jo_ਪਦ_ਪੰਕਤੀਆਂ;
    ਪੂਰਨ		jo_ਪਦ_ਖਾਨੇ_ਗਿਣਤੀ;
    ਪੂਰਨ		jo_vertical;
    ਪੂਰਨ		jo_curwin;
    buf_T	*jo_bufnr_buf;
    ਪੂਰਨ		jo_hidden;
    ਪੂਰਨ		jo_ਪਦ_norestore;
    char_u	jo_ਪਦ_name_buf[NUMBUFLEN];
    char_u	*jo_ਪਦ_name;
    char_u	jo_ਪਦ_opencmd_buf[NUMBUFLEN];
    char_u	*jo_ਪਦ_opencmd;
    ਪੂਰਨ		jo_ਪਦ_ਅੰਤ;
    char_u	jo_eof_chars_buf[NUMBUFLEN];
    char_u	*jo_eof_chars;
    char_u	jo_ਪਦ_kill_buf[NUMBUFLEN];
    char_u	*jo_ਪਦ_kill;
# ਜੇ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    long_u	jo_ansi_colors[16];
# endif
    char_u	jo_ਪਦ_highlight_buf[NUMBUFLEN];
    char_u	*jo_ਪਦ_highlight;
    ਪੂਰਨ		jo_tty_type;	    // first character of "tty_type"
    char_u	jo_ਪਦ_api_buf[NUMBUFLEN];
    char_u	*jo_ਪਦ_api;
#ਜੇ_ਅੰਤ
} jobopt_T;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
/*
 * Structure used for listeners added with listener_add().
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ listener_S listener_T;
ਬਣਤਰ listener_S
{
    listener_T	*lr_ਅਗਲਾ;
    ਪੂਰਨ		lr_id;
    bool	lr_ਪਾਠ;	// include the resulting text in each change
    callback_T	lr_callback;
};

// Structure used for listeners added with redraw_listener_add().
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ redraw_listener_S redraw_listener_T;
ਬਣਤਰ redraw_listener_S
{
    redraw_listener_T	*rl_ਅਗਲਾ;
    ਪੂਰਨ			rl_id;
    ਬਣਤਰ
    {
	callback_T	on_ਸ਼ੁਰੂ;
	callback_T	on_ਸਿਰਾ;
    }			rl_callbacks;
};
#ਜੇ_ਅੰਤ

/*
 * structure used for explicit stack while garbage collecting hash tables
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ ht_ਢੇਰ_S
{
    hashtab_T		*ht;
    ਬਣਤਰ ht_ਢੇਰ_S	*prev;
} ht_ਢੇਰ_T;

/*
 * structure used for explicit stack while garbage collecting lists
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ list_ਢੇਰ_S
{
    list_T		*list;
    ਬਣਤਰ list_ਢੇਰ_S	*prev;
} list_ਢੇਰ_T;

/*
 * structure used for explicit stack while garbage collecting tuples
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ tuple_ਢੇਰ_S
{
    tuple_T			*tuple;
    ਬਣਤਰ tuple_ਢੇਰ_S	*prev;
} tuple_ਢੇਰ_T;

/*
 * Structure used for iterating over dictionary items.
 * Initialize with dict_iterate_start().
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    long_u	dit_todo;
    hashitem_T	*dit_hi;
} dict_iterator_T;

// values for b_syn_spell: what to do with toplevel text
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SYNSPL_DEFAULT	0	// spell check if @Spell not defined
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SYNSPL_ਸਿਖਰ	1	// spell check toplevel text
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SYNSPL_NOTOP	2	// don't spell check toplevel text

// values for b_syn_foldlevel: how to compute foldlevel on a line
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SYNFLD_ਸ਼ੁਰੂ	0	// use level of item at start of line
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SYNFLD_MINIMUM	1	// use lowest local minimum level on line

// avoid #ifdefs for when b_spell is not available
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SPELL
# define B_SPELL(buf)  ((buf)->b_spell)
#ਨਹੀਂਤਾਂ
# define B_SPELL(buf)  (0)
#ਜੇ_ਅੰਤ

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ qf_info_S qf_info_T;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROFILE
/*
 * Used for :syntime: timing of executing a syntax pattern.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    proftime_T	ਕੁੱਲ;		// total time used
    proftime_T	slowest;	// time of slowest call
    ਲੰਮਾ	ਗਿਣਤੀ_2;		// nr of times used
    ਲੰਮਾ	ਮੇਲ;		// nr of times matched
} syn_ਸਮਾਂ_T;
#ਜੇ_ਅੰਤ

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ timer_S timer_T;
ਬਣਤਰ timer_S
{
    ਲੰਮਾ	tr_id;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TIMERS
    timer_T	*tr_ਅਗਲਾ;
    timer_T	*tr_prev;
    proftime_T	tr_due;		    // when the callback is to be invoked
    ਅੱਖਰ	tr_firing;	    // when TRUE callback is being called
    ਅੱਖਰ	tr_paused;	    // when TRUE callback is not invoked
    ਅੱਖਰ	tr_keep;	    // when TRUE keep timer after it fired
    ਪੂਰਨ		tr_repeat;	    // number of times to repeat, -1 forever
    ਲੰਮਾ	tr_interval;	    // msec
    callback_T	tr_callback;
    ਪੂਰਨ		tr_emsg_ਗਿਣਤੀ;
#ਜੇ_ਅੰਤ
};

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CRYPT
/*
 * Structure to hold the type of encryption and the state of encryption or
 * decryption.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    ਪੂਰਨ	    ਵਿਧੀ_nr;
    ਖਾਲੀ    *ਵਿਧੀ_state;  // method-specific state information
} cryptstate_T;

// values for method_nr
# define CRYPT_M_ZIP	0
# define CRYPT_M_BF	1
# define CRYPT_M_BF2	2
# define CRYPT_M_SOD    3
# define CRYPT_M_SOD2   4
# define CRYPT_M_ਗਿਣਤੀ	5 // number of crypt methods

// Currently all crypt methods work inplace.  If one is added that isn't then
// define this.
# define CRYPT_NOT_INPLACE 1

// Struct for passing arguments down to the crypt_init functions
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    char_u	*cat_salt;
    ਪੂਰਨ		cat_salt_len;
    char_u	*cat_ਬੀਜ;
    ਪੂਰਨ		cat_ਬੀਜ_len;
    char_u	*cat_ਜੋੜੋ;
    ਪੂਰਨ		cat_ਜੋੜੋ_len;
    ਪੂਰਨ		cat_init_from_file;
} crypt_arg_T;

#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
    POPPOS_BOTLEFT,
    POPPOS_TOPLEFT,
    POPPOS_BOTRIGHT,
    POPPOS_TOPRIGHT,
    POPPOS_CENTER,
    POPPOS_BOTTOM,	// bottom of popup just above the command line
    POPPOS_NONE
} poppos_T;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
    POPCLOSE_NONE,
    POPCLOSE_BUTTON,
    POPCLOSE_CLICK
} popclose_T;

# define POPUPWIN_DEFAULT_ZINDEX	 50
# define POPUPMENU_ZINDEX		100
# define POPUPWIN_DIALOG_ZINDEX		200
# define POPUPWIN_NOTIFICATION_ZINDEX   300
#ਜੇ_ਅੰਤ

/*
 * These are items normally related to a buffer.  But when using ":ownsyntax"
 * a window may have its own instance.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
    hashtab_T	b_keywtab;		// syntax keywords hash table
    hashtab_T	b_keywtab_ic;		// idem, ignore case
    ਪੂਰਨ		b_syn_error;		// TRUE when error occurred in HL
# ifdef FEAT_RELTIME
    ਪੂਰਨ		b_syn_slow;		// TRUE when 'redrawtime' reached
# endif
    ਪੂਰਨ		b_syn_ic;		// ignore case for :syn cmds
    ਪੂਰਨ		b_syn_foldlevel;	// how to compute foldlevel on a line
    ਪੂਰਨ		b_syn_spell;		// SYNSPL_ values
    garray_T	b_syn_patterns;		// table for syntax patterns
    garray_T	b_syn_clusters;		// table for syntax clusters
    ਪੂਰਨ		b_spell_cluster_id;	// @Spell cluster ID or 0
    ਪੂਰਨ		b_nospell_cluster_id;	// @NoSpell cluster ID or 0
    ਪੂਰਨ		b_syn_containedin;	// TRUE when there is an item with a
					// "containedin" argument
    ਪੂਰਨ		b_syn_sync_flags;	// flags about how to sync
    ਛੋਟਾ	b_syn_sync_id;		// group to sync on
    ਲੰਮਾ	b_syn_sync_minlines;	// minimal sync lines offset
    ਲੰਮਾ	b_syn_sync_maxlines;	// maximal sync lines offset
    ਲੰਮਾ	b_syn_sync_linebreaks;	// offset for multi-line pattern
    char_u	*b_syn_linecont_pat;	// line continuation pattern
    regprog_T	*b_syn_linecont_prog;	// line continuation program
# ifdef FEAT_PROFILE
    syn_ਸਮਾਂ_T  b_syn_linecont_ਸਮਾਂ;
# endif
    ਪੂਰਨ		b_syn_linecont_ic;	// ignore-case flag for above
    ਪੂਰਨ		b_syn_topgrp;		// for ":syntax include"
# ifdef FEAT_CONCEAL
    ਪੂਰਨ		b_syn_conceal;		// auto-conceal for :syn cmds
# endif
# ifdef FEAT_FOLDING
    ਪੂਰਨ		b_syn_folditems;	// number of patterns with the HL_FOLD
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
    ਪੂਰਨ		b_sst_len;
    synstate_T	*b_sst_ਪਹਿਲਾ;
    synstate_T	*b_sst_ਖੋਜ;
    synstate_T	*b_sst_firstfree;
    ਪੂਰਨ		b_sst_freecount;
    linenr_T	b_sst_check_lnum;
    short_u	b_sst_lasttick;	// last display tick

    // Cache for in_id_list(); see idl_cache_T in syntax.c.
    ਖਾਲੀ	*b_idlist_cache;
#ਜੇ_ਅੰਤ // FEAT_SYN_HL

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SPELL
    // for spell checking
    garray_T	b_langp;	    // list of pointers to slang_T, see spell.c
    char_u	b_spell_ismw[256];  // flags: is midword char
    char_u	*b_spell_ismw_mb;   // multi-byte midword chars
    char_u	*b_p_spc;	    // 'spellcapcheck'
    regprog_T	*b_cap_prog;	    // program for 'spellcapcheck'
    char_u	*b_p_spf;	    // 'spellfile'
    char_u	*b_p_spl;	    // 'spelllang'
    char_u	*b_p_spo;	    // 'spelloptions'
    ਪੂਰਨ		b_cjk;		    // all CJK letters as OK
#ਜੇ_ਅੰਤ
#ਜੇ !defined(FEAT_SYN_HL) && !defined(FEAT_SPELL)
    ਪੂਰਨ		dummy;
#ਜੇ_ਅੰਤ
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

ਬਣਤਰ file_ਬਫਰ
{
    memline_T	b_ml;		// associated memline (also contains line
				// count)

    buf_T	*b_ਅਗਲਾ;	// links in list of buffers
    buf_T	*b_prev;

    ਪੂਰਨ		b_nwindows;	// nr of windows open on this buffer

    ਪੂਰਨ		b_flags;	// various BF_ flags
    ਪੂਰਨ		b_locked;	// Buffer is being closed or referenced, don't
				// let autocommands wipe it out.
    ਪੂਰਨ		b_locked_ਵੰਡੋ;	// Buffer is being closed, don't allow opening
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

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
    bool	b_dev_valid;	// true when b_dev has a valid number
    dev_t	b_dev;		// device number
    ino_t	b_ino;		// inode number
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ VMS
    ਅੱਖਰ	 b_fab_rfm;	// Record format
    ਅੱਖਰ	 b_fab_rat;	// Record attribute
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ b_fab_mrs;	// Max record size
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		b_fnum;		// buffer number for this file.
    char_u	b_ਕੁੰਜੀ[VIM_SIZEOF_INT * 2 + 1];
				// key used for buf_hashtab, holds b_fnum as
				// hex string

    ਪੂਰਨ		b_ਬਦਲਿਆ;	// 'modified': Set to TRUE if something in the
				// file has been changed and not written out.
    dictitem16_T b_ct_di;	// holds the b:changedtick value in
				// b_ct_di.di_tv.vval.v_number;
				// incremented for each change, also for undo
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CHANGEDTICK(buf) ((buf)->b_ct_di.di_tv.vval.v_ਸੰਖਿਆ)

    varnumber_T	b_ਆਖਰੀ_changedtick;	// b:changedtick when TextChanged was
					// last triggered.
    varnumber_T	b_ਆਖਰੀ_changedtick_pum; // b:changedtick for TextChangedP
    varnumber_T	b_ਆਖਰੀ_changedtick_i;   // b:changedtick for TextChangedI

    bool	b_saving;	// Set to true if we are in the middle of
				// saving the buffer.

    /*
     * Changes to a buffer require updating of the display.  To minimize the
     * work, remember changes made and update everything at once.
     */
    bool	b_mod_ਸੈੱਟ;	// true when there are changes since the last
				// time the display was updated
    linenr_T	b_mod_ਸਿਖਰ;	// topmost lnum that was changed
    linenr_T	b_mod_bot;	// lnum below last changed line, AFTER the
				// change
    ਲੰਮਾ	b_mod_xlines;	// number of extra buffer lines inserted;
				// negative when lines were deleted

    wininfo_T	*b_wininfo;	// list of last used info for each window

    ਲੰਮਾ	b_mtime;	// last change time of original file
    ਲੰਮਾ	b_mtime_ns;	// nanoseconds of last change time
    ਲੰਮਾ	b_mtime_read;	// last change time when reading
    ਲੰਮਾ	b_mtime_read_ns;  // nanoseconds of last read time
    off_T	b_orig_ਮਾਪ;	// size of original file in bytes
    ਪੂਰਨ		b_orig_ਬਹੁਲਕ;	// mode of original file
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VIMINFO
    ਸਮਾਂ_T	b_ਆਖਰੀ_ਵਰਤਿਆ;	// time when the buffer was last used; used
				// for viminfo
#ਜੇ_ਅੰਤ

    pos_T	b_namedm[NMARKS]; // current named marks (mark.c)

    // These variables are set when VIsual_active becomes FALSE
    visualinfo_T b_visual;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਪੂਰਨ		b_visual_ਬਹੁਲਕ_eval;  // b_visual.vi_mode for visualmode()
#ਜੇ_ਅੰਤ

    pos_T	b_ਆਖਰੀ_cursor;	// cursor position when last unloading this
				//ਬਫਰ
    pos_T	b_ਆਖਰੀ_ਪਾਓ;	// where Insert mode was left
    pos_T	b_ਆਖਰੀ_change;	// position of last change: '. mark

    /*
     * the changelist contains old change positions
     */
    pos_T	b_changelist[JUMPLISTSIZE];
    ਪੂਰਨ		b_changelistlen;	// number of active entries
    bool	b_new_change;		// set in u_savecommon()

    /*
     * Character table, only used in charset.c for 'iskeyword'
     * 32 bytes of 8 bits: 1 bit per character 0-255.
     */
    char_u	b_chartab[32];

    // Table used for mappings local to a buffer.
    mapblock_T	*(b_maphash[256]);

    // First abbreviation local to a buffer.
    mapblock_T	*b_ਪਹਿਲਾ_abbr;

    // User commands local to the buffer.
    garray_T	b_ucmds;
    // start and end of an operator, also used for '[ and ']
    pos_T	b_op_ਸ਼ੁਰੂ;
    pos_T	b_op_ਸ਼ੁਰੂ_orig;  // used for Insstart_orig
    pos_T	b_op_ਸਿਰਾ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VIMINFO
    bool	b_marks_read;	// Have we read viminfo marks yet?
#ਜੇ_ਅੰਤ

    bool	b_modified_was_ਸੈੱਟ;	// did ":set modified"
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
    u_ਸਿਰਲੇਖ_T	*b_u_oldhead;	// pointer to oldest header
    u_ਸਿਰਲੇਖ_T	*b_u_newhead;	// pointer to newest header; may not be valid
				// if b_u_curhead is not NULL
    u_ਸਿਰਲੇਖ_T	*b_u_curhead;	// pointer to current header
    ਪੂਰਨ		b_u_numhead;	// current number of headers
    bool	b_u_synced;	// entry lists are synced
    ਲੰਮਾ	b_u_seq_ਆਖਰੀ;	// last used undo sequence number
    ਲੰਮਾ	b_u_save_nr_ਆਖਰੀ; // counter for last file write
    ਲੰਮਾ	b_u_seq_cur;	// uh_seq of header below which we are now
    ਸਮਾਂ_T	b_u_ਸਮਾਂ_cur;	// uh_time of header below which we are now
    ਲੰਮਾ	b_u_save_nr_cur; // file write nr after which we are now

    /*
     * variables for "U" command in undo.c
     */
    undoline_T	b_u_ਸਤਰ_ptr;	// saved line for "U" command
    linenr_T	b_u_ਸਤਰ_lnum;	// line number of line in u_line
    colnr_T	b_u_ਸਤਰ_colnr;	// optional column number

    bool	b_scanned;	// ^N/^P have scanned this buffer

    // flags for use of ":lmap" and IM control
    ਲੰਮਾ	b_p_iminsert;	// input mode for insert
    ਲੰਮਾ	b_p_imsearch;	// input mode for search
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ B_IMODE_USE_ਪਾਓ (-1)	//	Use b_p_iminsert value for search
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ B_IMODE_NONE 0		//	Input via none
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ B_IMODE_LMAP 1		//	Input via langmap
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ B_IMODE_IM 2		//	Input via input method
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ B_IMODE_ਆਖਰੀ 2

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_KEYMAP
    ਛੋਟਾ	b_kmap_state;	// using "lmap" mappings
# define KEYMAP_INIT	1	// 'keymap' was set, call keymap_init()
# define KEYMAP_LOADED	2	// 'keymap' mappings have been loaded
    garray_T	b_kmap_ga;	// the keymap table
#ਜੇ_ਅੰਤ

    /*
     * Options local to a buffer.
     * They are here because their value depends on the type of file
     * or contents of the file being edited.
     */
    bool	b_p_initialized;	// set when options initialized

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    sctx_T	b_p_script_ctx[BV_ਗਿਣਤੀ]; // SCTXs for buffer-local options
#ਜੇ_ਅੰਤ

    ਪੂਰਨ		b_p_ac;		// 'autocomplete'
    ਪੂਰਨ		b_p_ai;		// 'autoindent'
    ਪੂਰਨ		b_p_ai_nopaste;	// b_p_ai saved for paste mode
    char_u	*b_p_bkc;	// 'backupcopy'
    ਬਿਨਾਨਿਸ਼ਾਨੀ	b_bkc_flags;    // flags for 'backupcopy'
    ਪੂਰਨ		b_p_ci;		// 'copyindent'
    ਪੂਰਨ		b_p_bin;	// 'binary'
    ਪੂਰਨ		b_p_bomb;	// 'bomb'
    char_u	*b_p_bh;	// 'bufhidden'
    char_u	*b_p_bt;	// 'buftype'
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_QUICKFIX
# define BUF_HAS_QF_ਇੰਦਰਾਜ 1
# define BUF_HAS_LL_ਇੰਦਰਾਜ 2
    ਪੂਰਨ		b_has_qf_ਇੰਦਰਾਜ;
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		b_p_bl;		// 'buflisted'
    ਪੂਰਨ		b_p_cin;	// 'cindent'
    char_u	*b_p_cino;	// 'cinoptions'
    char_u	*b_p_cink;	// 'cinkeys'
    char_u	*b_p_cinsd;	// 'cinscopedecls'
    char_u	*b_p_cinw;	// 'cinwords'
    char_u	*b_p_com;	// 'comments'
    char_u	*b_p_cms;	// 'commentstring'
    char_u	*b_p_cot;	// 'completeopt' local value
    ਬਿਨਾਨਿਸ਼ਾਨੀ	b_cot_flags;	// flags for 'completeopt'
    char_u	*b_p_cpt;	// 'complete'
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ BACKSLASH_ਵਿੱਚ_FILENAME
    char_u	*b_p_csl;	// 'completeslash'
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_COMPL_FUNC
    long_u	b_p_cpt_flags;	// flags for 'complete'
    callback_T	*b_p_cpt_cb;	// F{func} in 'complete' callback
    ਪੂਰਨ		b_p_cpt_ਗਿਣਤੀ;	// Count of values in 'complete'
    char_u	*b_p_cfu;	// 'completefunc'
    callback_T	b_cfu_cb;	// 'completefunc' callback
    char_u	*b_p_ofu;	// 'omnifunc'
    callback_T	b_ofu_cb;	// 'omnifunc' callback
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    char_u	*b_p_tfu;	// 'tagfunc' option value
    callback_T	b_tfu_cb;	// 'tagfunc' callback
    char_u	*b_p_ffu;	// 'findfunc' option value
    callback_T	b_ffu_cb;	// 'findfunc' callback
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		b_p_eof;	// 'endoffile'
    ਪੂਰਨ		b_p_eol;	// 'endofline'
    ਪੂਰਨ		b_p_fixeol;	// 'fixendofline'
    ਪੂਰਨ		b_p_et;		// 'expandtab'
    ਪੂਰਨ		b_p_et_nobin;	// b_p_et saved for binary mode
    ਪੂਰਨ		b_p_et_nopaste; // b_p_et saved for paste mode
    char_u	*b_p_fenc;	// 'fileencoding'
    char_u	*b_p_ff;	// 'fileformat'
    char_u	*b_p_ft;	// 'filetype'
    char_u	*b_p_fo;	// 'formatoptions'
    char_u	*b_p_flp;	// 'formatlistpat'
    ਪੂਰਨ		b_p_inf;	// 'infercase'
    char_u	*b_p_isk;	// 'iskeyword'
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਲੱਭੋ_ID
    char_u	*b_p_def;	// 'define' local value
    char_u	*b_p_inc;	// 'include'
# ifdef FEAT_EVAL
    char_u	*b_p_inex;	// 'includeexpr'
    long_u	b_p_inex_flags;	// flags for 'includeexpr'
# endif
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_EVAL)
    char_u	*b_p_inde;	// 'indentexpr'
    long_u	b_p_inde_flags;	// flags for 'indentexpr'
    char_u	*b_p_indk;	// 'indentkeys'
#ਜੇ_ਅੰਤ
    char_u	*b_p_fp;	// 'formatprg'
#ਜੇ defined(FEAT_EVAL)
    char_u	*b_p_fex;	// 'formatexpr'
    long_u	b_p_fex_flags;	// flags for 'formatexpr'
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_FSYNC
    ਪੂਰਨ		b_p_fs;		// 'fsync'
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CRYPT
    char_u	*b_p_ਕੁੰਜੀ;	// 'key'
#ਜੇ_ਅੰਤ
    char_u	*b_p_kp;	// 'keywordprg'
    ਪੂਰਨ		b_p_lisp;	// 'lisp'
    char_u	*b_p_lop;	// 'lispoptions'
    char_u	*b_p_menc;	// 'makeencoding'
    char_u	*b_p_mps;	// 'matchpairs'
    ਪੂਰਨ		b_p_ml;		// 'modeline'
    ਪੂਰਨ		b_p_ml_nobin;	// b_p_ml saved for binary mode
    ਪੂਰਨ		b_p_ma;		// 'modifiable'
    char_u	*b_p_nf;	// 'nrformats'
    ਪੂਰਨ		b_p_pi;		// 'preserveindent'
    char_u	*b_p_qe;	// 'quoteescape'
    ਪੂਰਨ		b_p_ro;		// 'readonly'
    ਲੰਮਾ	b_p_sw;		// 'shiftwidth'
    ਪੂਰਨ		b_p_sn;		// 'shortname'
    ਪੂਰਨ		b_p_si;		// 'smartindent'
    ਲੰਮਾ	b_p_sts;	// 'softtabstop'
    ਲੰਮਾ	b_p_sts_nopaste; // b_p_sts saved for paste mode
    char_u	*b_p_sua;	// 'suffixesadd'
    ਪੂਰਨ		b_p_swf;	// 'swapfile'
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
    ਲੰਮਾ	b_p_smc;	// 'synmaxcol'
    char_u	*b_p_syn;	// 'syntax'
#ਜੇ_ਅੰਤ
    ਲੰਮਾ	b_p_ts;		// 'tabstop'
    ਪੂਰਨ		b_p_tx;		// 'textmode'
    ਲੰਮਾ	b_p_tw;		// 'textwidth'
    ਲੰਮਾ	b_p_tw_nobin;	// b_p_tw saved for binary mode
    ਲੰਮਾ	b_p_tw_nopaste;	// b_p_tw saved for paste mode
    ਲੰਮਾ	b_p_wm;		// 'wrapmargin'
    ਲੰਮਾ	b_p_wm_nobin;	// b_p_wm saved for binary mode
    ਲੰਮਾ	b_p_wm_nopaste;	// b_p_wm saved for paste mode
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VARTABS
    char_u	*b_p_vsts;	// 'varsofttabstop'
    ਪੂਰਨ		*b_p_vsts_array;   // 'varsofttabstop' in internal format
    char_u	*b_p_vsts_nopaste; // b_p_vsts saved for paste mode
    char_u	*b_p_vts;	// 'vartabstop'
    ਪੂਰਨ		*b_p_vts_array;	// 'vartabstop' in internal format
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_KEYMAP
    char_u	*b_p_keymap;	// 'keymap'
#ਜੇ_ਅੰਤ

    /*
     * local values for options which are normally global
     */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_QUICKFIX
    char_u	*b_p_gefm;	// 'grepformat' local value
    char_u	*b_p_gp;	// 'grepprg' local value
    char_u	*b_p_mp;	// 'makeprg' local value
    char_u	*b_p_efm;	// 'errorformat' local value
#ਜੇ_ਅੰਤ
    char_u	*b_p_ep;	// 'equalprg' local value
    char_u	*b_p_ਰਸਤਾ;	// 'path' local value
    ਪੂਰਨ		b_p_ar;		// 'autoread' local value
    char_u	*b_p_ਟੈਗ_ਸੂਚੀ;	// 'tags' local value
    char_u	*b_p_tc;	// 'tagcase' local value
    ਬਿਨਾਨਿਸ਼ਾਨੀ	b_tc_flags;     // flags for 'tagcase'
    char_u	*b_p_dict;	// 'dictionary' local value
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
    char_u	*b_p_dia;	// 'diffanchors' local value
#ਜੇ_ਅੰਤ
    char_u	*b_p_tsr;	// 'thesaurus' local value
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_COMPL_FUNC
    char_u	*b_p_tsrfu;	// 'thesaurusfunc' local value
    callback_T	b_tsrfu_cb;	// 'thesaurusfunc' callback
#ਜੇ_ਅੰਤ
    ਲੰਮਾ	b_p_ul;		// 'undolevels' local value
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PERSISTENT_UNDO
    ਪੂਰਨ		b_p_udf;	// 'undofile'
#ਜੇ_ਅੰਤ
    char_u	*b_p_lw;	// 'lispwords' local value
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
    ਲੰਮਾ	b_p_twsl;	// 'termwinscroll'
#ਜੇ_ਅੰਤ

    /*
     * end of buffer options
     */

    // values set from b_p_cino
    ਪੂਰਨ		b_ind_level;
    ਪੂਰਨ		b_ind_open_imag;
    ਪੂਰਨ		b_ind_no_brace;
    ਪੂਰਨ		b_ind_ਪਹਿਲਾ_open;
    ਪੂਰਨ		b_ind_open_extra;
    ਪੂਰਨ		b_ind_close_extra;
    ਪੂਰਨ		b_ind_open_ਖੱਬਾ_imag;
    ਪੂਰਨ		b_ind_jump_label;
    ਪੂਰਨ		b_ind_case;
    ਪੂਰਨ		b_ind_case_ਕੋਡ;
    ਪੂਰਨ		b_ind_case_break;
    ਪੂਰਨ		b_ind_param;
    ਪੂਰਨ		b_ind_func_type;
    ਪੂਰਨ		b_ind_comment;
    ਪੂਰਨ		b_ind_ਵਿੱਚ_comment;
    ਪੂਰਨ		b_ind_ਵਿੱਚ_comment2;
    ਪੂਰਨ		b_ind_cpp_baseclass;
    ਪੂਰਨ		b_ind_continuation;
    ਪੂਰਨ		b_ind_unclosed;
    ਪੂਰਨ		b_ind_unclosed2;
    ਪੂਰਨ		b_ind_unclosed_noignore;
    ਪੂਰਨ		b_ind_unclosed_wrapped;
    ਪੂਰਨ		b_ind_unclosed_whiteok;
    ਪੂਰਨ		b_ind_matching_paren;
    ਪੂਰਨ		b_ind_paren_prev;
    ਪੂਰਨ		b_ind_maxparen;
    ਪੂਰਨ		b_ind_maxcomment;
    ਪੂਰਨ		b_ind_scopedecl;
    ਪੂਰਨ		b_ind_scopedecl_ਕੋਡ;
    ਪੂਰਨ		b_ind_java;
    ਪੂਰਨ		b_ind_js;
    ਪੂਰਨ		b_ind_keep_case_label;
    ਪੂਰਨ		b_ind_hash_comment;
    ਪੂਰਨ		b_ind_cpp_namespace;
    ਪੂਰਨ		b_ind_if_for_while;
    ਪੂਰਨ		b_ind_cpp_extern_c;
    ਪੂਰਨ		b_ind_pragma;

    linenr_T	b_no_eol_lnum;	// non-zero lnum when last line of next binary
				// write should not have an end-of-line

    ਪੂਰਨ		b_ਸ਼ੁਰੂ_eof;	// last line had eof (CTRL-Z) when it was read
    ਪੂਰਨ		b_ਸ਼ੁਰੂ_eol;	// last line had eol when it was read
    ਪੂਰਨ		b_ਸ਼ੁਰੂ_ffc;	// first char of 'ff' when edit started
    char_u	*b_ਸ਼ੁਰੂ_fenc;	// 'fileencoding' when edit started or NULL
    ਪੂਰਨ		b_bad_char;	// "++bad=" argument when edit started or 0
    ਪੂਰਨ		b_ਸ਼ੁਰੂ_bomb;	// 'bomb' when it was read

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    dictitem_T	b_bufvar;	// variable for "b:" Dictionary
    dict_T	*b_vars;	// internal variables, local to buffer

    listener_T	*b_listener;       // Listeners accepting buffered reports.
    listener_T	*b_sync_listener;  // Listeners requiring unbuffered reports.
    list_T	*b_recorded_changes;
    size_t	b_recorded_ਪਾਠ_ਮਾਪ;  // bytes of text held by the above
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    bool	b_has_textprop;	// true when text props were added
    hashtab_T	*b_proptypes;	// text property types local to buffer
    proptype_T	**b_proparray;	// entries of b_proptypes sorted on tp_id
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_BEVAL) && defined(FEAT_EVAL)
    char_u	*b_p_bexpr;	// 'balloonexpr' local value
    long_u	b_p_bexpr_flags;// flags for 'balloonexpr'
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CRYPT
    char_u	*b_p_cm;	// 'cryptmethod'
#ਜੇ_ਅੰਤ

    // When a buffer is created, it starts without a swap file.  b_may_swap is
    // then set to indicate that a swap file may be opened later.  It is reset
    // if a swap file could not be opened.
    bool	b_may_ਅਦਲਾ_ਬਦਲੀ;
    bool	b_did_warn;	// Set to true if user has been warned on first
				// change of a read-only file

    // Two special kinds of buffers:
    // help buffer  - used for help files, won't use a swap file.
    // spell buffer - used for spell info, never displayed and doesn't have a
    //		      file name.
    bool	b_help;		// true for help file buffer (when set b_p_bt
				// is "help")
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SPELL
    bool	b_spell;	// true for a spell file buffer, most fields
				// are not used!  Use the B_SPELL macro to
				// access b_spell without #ifdef.
#ਜੇ_ਅੰਤ

    bool	b_shortname;	// this file has an 8.3 file name

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
    char_u	*b_prompt_ਪਾਠ;		// set by prompt_setprompt()
    callback_T	b_prompt_callback;	// set by prompt_setcallback()
    callback_T	b_prompt_ਰੁਕਾਵਟ;	// set by prompt_setinterrupt()
    ਪੂਰਨ		b_prompt_ਪਾਓ;	// value for restart_edit when entering
					// a prompt buffer window.
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MZSCHEME
    ਖਾਲੀ	*b_mzscheme_ref; // The MzScheme reference to this buffer
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PERL
    ਖਾਲੀ	*b_perl_ਨਿੱਜੀ;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PYTHON
    ਖਾਲੀ	*b_python_ref;	// The Python reference to this buffer
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PYTHON3
    ਖਾਲੀ	*b_python3_ref;	// The Python3 reference to this buffer
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TCL
    ਖਾਲੀ	*b_tcl_ref;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RUBY
    ਖਾਲੀ	*b_ruby_ref;
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_SYN_HL) || defined(FEAT_SPELL)
    synblock_T	b_s;		// Info related to syntax highlighting.  w_s
				// normally points to this, but some windows
				// may use a different synblock_T.
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SIGNS
    sign_ਇੰਦਰਾਜ_T *b_signlist;	   // list of placed signs
    sign_ਇੰਦਰਾਜ_T *b_sign_finger;   // last sign inserted, used to speed up
				   // inserting signs in ascending line order
# ifdef FEAT_NETBEANS_INTG
    bool	b_has_sign_ਕਾਲਮ; // Flag that is set when a first sign is
				   // added and remains set until the end of
				   // the netbeans session.
# endif
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_NETBEANS_INTG
    bool	b_netbeans_file;    // true when buffer is owned by NetBeans
    bool	b_was_netbeans_file;// true if b_netbeans_file was once set
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
    bool	b_write_ਮੰਜ਼ਿਲ_channel; // true when appended lines are written to
				    // a channel.
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CRYPT
    cryptstate_T *b_cryptstate;	// Encryption state while reading or writing
				// the file. NULL when not using encryption.
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		b_mapped_ctrl_c; // modes where CTRL-C is mapped

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
    ਪਦ_T	*b_ਪਦ;	// When not NULL this buffer is for a terminal
				// window.
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
    bool	b_diff_failed;	// internal diff failed for this buffer
#ਜੇ_ਅੰਤ
}; // file_buffer


#ਜੇ defined(FEAT_DIFF)
/*
 * Stuff for diff mode.
 */
# define DB_ਗਿਣਤੀ 8	// up to eight buffers can be diff'ed

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
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ diffblock_S diff_T;
ਬਣਤਰ diffblock_S
{
    diff_T	*df_ਅਗਲਾ;
    linenr_T	df_lnum[DB_ਗਿਣਤੀ];	// line number in buffer
    linenr_T	df_ਗਿਣਤੀ[DB_ਗਿਣਤੀ];	// nr of inserted/changed lines
    ਪੂਰਨ is_linematched;  // has the linematch algorithm ran on this diff hunk to divide it into
			  // smaller diff hunks?

    ਪੂਰਨ		has_changes;		// has cached list of inline changes
    garray_T	df_changes;		// list of inline changes (diffline_change_T)
};

/*
 * Each entry stores a single inline change within a diff block. Line numbers
 * are recorded as relative offsets, and columns are byte offsets, not
 * character counts.
 * Ranges are [start,end), with the end being exclusive.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ diffline_change_S diffline_change_T;
ਬਣਤਰ diffline_change_S
{
    colnr_T	dc_ਸ਼ੁਰੂ[DB_ਗਿਣਤੀ];	// byte offset of start of range in the line
    colnr_T	dc_ਸਿਰਾ[DB_ਗਿਣਤੀ];	// 1 past byte offset of end of range in line
    ਪੂਰਨ		dc_ਸ਼ੁਰੂ_lnum_off[DB_ਗਿਣਤੀ];	// starting line offset
    ਪੂਰਨ		dc_ਸਿਰਾ_lnum_off[DB_ਗਿਣਤੀ];	// end line offset
};

/*
 * Describes a single line's list of inline changes. Use diff_change_parse() to
 * parse this.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ diffline_S diffline_T;
ਬਣਤਰ diffline_S
{
    diffline_change_T *changes;
    ਪੂਰਨ ਅੰਕ_changes;
    ਪੂਰਨ bufidx;
    ਪੂਰਨ lineoff;
};
#ਨਹੀਂਤਾਂ  // FEAT_DIFF
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਖਾਲੀ diffline_T;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਖਾਲੀ diffline_change_T;
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SNAP_HELP_IDX	    0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SNAP_AUCMD_IDX	    1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SNAP_QUICKFIX_IDX   2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SNAP_ਗਿਣਤੀ	    3

/*
 * Tab pages point to the top frame of each tab page.
 * Note: Most values are NOT valid for the current tab page!  Use "curwin",
 * "firstwin", etc. for that.  "tp_topframe" is always valid and can be
 * compared against "topframe" to find the current tab page.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ tabpage_S tabpage_T;
ਬਣਤਰ tabpage_S
{
    tabpage_T	    *tp_ਅਗਲਾ;	    // next tabpage or NULL
    ਚੌਖਟਾ_T	    *tp_topframe;   // topframe for the windows
    win_T	    *tp_curwin;	    // current window in this Tab page
    win_T	    *tp_prevwin;    // previous window in this Tab page
    win_T	    *tp_firstwin;   // first window in this Tab page
    win_T	    *tp_lastwin;    // last window in this Tab page
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    win_T	    *tp_ਪਹਿਲਾ_popupwin; // first popup window in this Tab page
#ਜੇ_ਅੰਤ
    ਲੰਮਾ	    tp_old_ਪੰਕਤੀਆਂ;    // Rows when Tab page was left
    ਲੰਮਾ	    tp_old_Columns; // Columns when Tab page was left, -1 when
				    // calling shell_new_columns() postponed
    ਪੂਰਨ		    tp_old_coloff;  // Column offset when Tab page was left
    ਲੰਮਾ	    tp_ch_ਵਰਤਿਆ;	    // value of 'cmdheight' when frame size
				    // was set
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    ਪੂਰਨ		    tp_prev_which_scrollbars[3];
				    // previous value of which_scrollbars
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		    tp_did_tabclosedpre;  // whether TabClosedPre was triggered

    char_u	    *tp_localdir;	// absolute path of local directory or
					// NULL
    char_u	    *tp_prevdir;	// previous directory

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
    diff_T	    *tp_ਪਹਿਲਾ_diff;
    buf_T	    *(tp_diffbuf[DB_ਗਿਣਤੀ]);
    ਪੂਰਨ		    tp_diff_invalid;	// list of diffs is outdated
    ਪੂਰਨ		    tp_diff_update;	// update diffs before redrawing
#ਜੇ_ਅੰਤ
    ਚੌਖਟਾ_T	    *(tp_snapshot[SNAP_ਗਿਣਤੀ]);  // window layout snapshots
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    dictitem_T	    tp_winvar;	    // variable for "t:" Dictionary
    dict_T	    *tp_vars;	    // internal variables, local to tab page
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PYTHON
    ਖਾਲੀ	    *tp_python_ref;	// The Python value for this tab page
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PYTHON3
    ਖਾਲੀ	    *tp_python3_ref;	// The Python value for this tab page
#ਜੇ_ਅੰਤ
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
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ w_ਸਤਰ
{
    linenr_T	wl_lnum;	// buffer line number for logical line
    short_u	wl_ਮਾਪ;	// height in screen lines
    ਅੱਖਰ	wl_valid;	// TRUE values are valid for text in buffer
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
    ਅੱਖਰ	wl_folded;	// TRUE when this is a range of folded lines
    linenr_T	wl_lastlnum;	// last buffer line number for logical line
#ਜੇ_ਅੰਤ
} wline_T;

/*
 * Windows are kept in a tree of frames.  Each frame has a column (FR_COL)
 * or row (FR_ROW) layout or is a leaf, which has a window.
 */
ਬਣਤਰ ਚੌਖਟਾ_S
{
    ਅੱਖਰ	fr_layout;	// FR_LEAF, FR_COL or FR_ROW
    ਪੂਰਨ		fr_ਚੌੜਾਈ;
    ਪੂਰਨ		fr_newwidth;	// new width used in win_equal_rec()
    ਪੂਰਨ		fr_ਉਚਾਈ;
    ਪੂਰਨ		fr_newheight;	// new height used in win_equal_rec()
    ਚੌਖਟਾ_T	*fr_ਮਾਪਾ;	// containing frame or NULL
    ਚੌਖਟਾ_T	*fr_ਅਗਲਾ;	// frame right or below in same parent, NULL
				// for last
    ਚੌਖਟਾ_T	*fr_prev;	// frame left or above in same parent, NULL
				// for first
    // fr_child and fr_win are mutually exclusive
    ਚੌਖਟਾ_T	*fr_ਬੱਚਾ;	// first contained frame
    win_T	*fr_win;	// window that fills this frame; for a snapshot
				// set to the current window
};

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FR_ਪੱਤੀ	0	// frame is a leaf
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FR_ਪੰਕਤੀ	1	// frame with a row of windows
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FR_ਖਾਨਾ	2	// frame with a column of windows

/*
 * Struct used for highlighting 'hlsearch' matches, matches defined by
 * ":match" and matches defined by match functions.
 * For 'hlsearch' there is one pattern for all windows.  For ":match" and the
 * match functions there is a different pattern for each window.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    regmmatch_T	rm;	    // points to the regexp program; contains last
			    // found match (may continue in next line)
    buf_T	*buf;	    // the buffer to search for a match
    linenr_T	lnum;	    // the line to search for a match
    ਪੂਰਨ		attr;	    // attributes to be used for a match
    ਪੂਰਨ		attr_cur;   // attributes currently active in win_line()
    linenr_T	ਪਹਿਲਾ_lnum; // first lnum to search for multi-line pat
    colnr_T	startcol;   // in win_line() points to char where HL starts
    colnr_T	endcol;	    // in win_line() points to char where HL ends
    ਅੱਖਰ	is_addpos;  // position specified directly by
			    // matchaddpos(). TRUE/FALSE
    ਅੱਖਰ	has_cursor; // TRUE if the cursor is inside the match, used for
			    // CurSearch
} ਮੇਲ_T;

/*
 * Same as lpos_T, but with additional field len.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    linenr_T	lnum;	// line number
    colnr_T	ਖਾਨਾ;	// column number
    ਪੂਰਨ		len;	// length: 0 - to the end of line
} llpos_T;

/*
 * matchitem_T provides a linked list for storing match items for ":match",
 * matchadd() and matchaddpos().
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ matchitem matchitem_T;
ਬਣਤਰ matchitem
{
    matchitem_T	*mit_ਅਗਲਾ;
    ਪੂਰਨ		mit_id;		// match ID
    ਪੂਰਨ		mit_ਤਰਜੀਹ;   // match priority

    // Either a pattern is defined (mit_pattern is not NUL) or a list of
    // positions is given (mit_pos is not NULL and mit_pos_count > 0).
    char_u	*mit_ਨਮੂਨਾ;   // pattern to highlight
    regmmatch_T	mit_ਮੇਲ;	// regexp program for pattern

    llpos_T	*mit_pos_array;	// array of positions
    ਪੂਰਨ		mit_pos_ਗਿਣਤੀ;	// nr of entries in mit_pos
    ਪੂਰਨ		mit_pos_cur;	// internal position counter
    linenr_T	mit_toplnum;	// top buffer line
    linenr_T	mit_botlnum;	// bottom buffer line

    ਮੇਲ_T	mit_hl;		// struct for doing the actual highlighting
    ਪੂਰਨ		mit_hlg_id;	// highlight group ID
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CONCEAL
    ਪੂਰਨ		mit_conceal_char; // cchar for Conceal highlighting
#ਜੇ_ਅੰਤ
};

// Structure to store last cursor position and topline.  Used by check_lnums()
// and reset_lnums().
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    ਪੂਰਨ		w_topline_save;	// original topline value
    ਪੂਰਨ		w_topline_corr;	// corrected topline value
    pos_T	w_cursor_save;	// original cursor position
    pos_T	w_cursor_corr;	// corrected cursor position
} pos_save_T;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MENU
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    ਪੂਰਨ		wb_startcol;
    ਪੂਰਨ		wb_endcol;
    vimmenu_T	*wb_menu;
} winbar_ਵਸਤੂ_T;
#ਜੇ_ਅੰਤ

/*
 * Characters from the 'listchars' option
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    ਪੂਰਨ		eol;
    ਪੂਰਨ		ext;
    ਪੂਰਨ		prec;
    ਪੂਰਨ		nbsp;
    ਪੂਰਨ		space;
    ਪੂਰਨ		tab1;
    ਪੂਰਨ		tab2;
    ਪੂਰਨ		tab3;
    ਪੂਰਨ		trail;
    ਪੂਰਨ		lead;
    ਪੂਰਨ		leadtab1;
    ਪੂਰਨ		leadtab2;
    ਪੂਰਨ		leadtab3;
    ਪੂਰਨ		*multispace;
    ਪੂਰਨ		*leadmultispace;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CONCEAL
    ਪੂਰਨ		conceal;
#ਜੇ_ਅੰਤ
} lcs_chars_T;

/*
 * Characters from the 'fillchars' option
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    ਪੂਰਨ	stl;
    ਪੂਰਨ	stlnc;
    ਪੂਰਨ	vert;
    ਪੂਰਨ	fold;
    ਪੂਰਨ	foldopen;
    ਪੂਰਨ	foldclosed;
    ਪੂਰਨ	foldsep;
    ਪੂਰਨ	foldinner;
    ਪੂਰਨ	diff;
    ਪੂਰਨ	eob;
    ਪੂਰਨ	lastline;
#ਜੇ defined(FEAT_TABPANEL)
    ਪੂਰਨ	tpl_vert;
#ਜੇ_ਅੰਤ
    ਪੂਰਨ trunc;
    ਪੂਰਨ truncrl;
} fill_chars_T;

/*
 * Represents current highlight overrides (used by 'winhighlight' option). The
 * highlight group with ID "from" will be overridden by the highlight group with
 * ID "to"
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    ਪੂਰਨ from; // If zero or negative then it is hlf_T enum
    ਪੂਰਨ ਮੰਜ਼ਿਲ; // Same thing as "from"
} hl_override_T;

/*
 * Structure which contains all information that belongs to a window
 *
 * All row numbers are relative to the start of the window, except w_winrow.
 */
ਬਣਤਰ ਖਿੜਕੀ_S
{
    ਪੂਰਨ		w_id;		    // unique window ID

    buf_T	*w_ਬਫਰ;	    // buffer we are a window into

    win_T	*w_prev;	    // link to previous window
    win_T	*w_ਅਗਲਾ;	    // link to next window

#ਜੇ defined(FEAT_SYN_HL) || defined(FEAT_SPELL)
    synblock_T	*w_s;		    // for :ownsyntax
#ਜੇ_ਅੰਤ

    ਪੂਰਨ		w_locked;	    // don't let autocommands close the window

    ਚੌਖਟਾ_T	*w_ਚੌਖਟਾ;	    // frame containing this window

    pos_T	w_cursor;	    // cursor position in buffer

    colnr_T	w_curswant;	    // The column we'd like to be at.  This is
				    // used to try to stay in the same column
				    // for up/down cursor motions.

    bool	w_ਸੈੱਟ_curswant;	    // If set, then update w_curswant the next
				    // time through cursupdate() to the
				    // current virtual column

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
    linenr_T	w_ਆਖਰੀ_cursorline;  // where last time 'cursorline' was drawn
#ਜੇ_ਅੰਤ

    /*
     * the next seven are used to update the Visual highlighting
     */
    ਅੱਖਰ	w_old_visual_ਬਹੁਲਕ;  // last known VIsual_mode
    linenr_T	w_old_cursor_lnum;  // last known end of visual part
    colnr_T	w_old_cursor_fcol;  // first column for block visual part
    colnr_T	w_old_cursor_lcol;  // last column for block visual part
    linenr_T	w_old_visual_lnum;  // last known start of visual part
    colnr_T	w_old_visual_ਖਾਨਾ;   // last known start of visual part
    colnr_T	w_old_curswant;	    // last known value of Curswant

    linenr_T    w_ਆਖਰੀ_cursor_lnum_rnu;  // cursor lnum when 'rnu' was last
					 // redrawn

    lcs_chars_T	w_lcs_chars;	    // 'listchars' characters
    fill_chars_T w_fill_chars;	    // 'fillchars' characters

    /*
     * "w_topline", "w_leftcol" and "w_skipcol" specify the offsets for
     * displaying the buffer.
     */
    linenr_T	w_topline;	    // buffer line number of the line at the
				    // top of the window
    bool	w_topline_was_ਸੈੱਟ;  // flag set to true when topline is set,
				    // e.g. by winrestview()

    linenr_T	w_botline;	    // number of the line below the bottom of
				    // the window

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
    ਪੂਰਨ		w_topfill;	    // number of filler lines above w_topline
    ਪੂਰਨ		w_old_topfill;	    // w_topfill at last redraw
    bool	w_botfill;	    // true when filler lines are actually
				    // below w_topline (at end of file)
    bool	w_old_botfill;	    // w_botfill at last redraw
#ਜੇ_ਅੰਤ
    colnr_T	w_leftcol;	    // screen column number of the left most
				    // character in the window; used when
				    // 'wrap' is off
    colnr_T	w_skipcol;	    // starting screen column for the first
				    // line in the window; used when 'wrap' is
				    // on; does not include win_col_off()

    ਪੂਰਨ		w_ਖਾਲੀ_ਥਾਵਾਂ_ਪੰਕਤੀਆਂ;	    // number of ~ rows in window
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
    ਪੂਰਨ		w_filler_ਪੰਕਤੀਆਂ;	    // number of filler rows at the end of the
				    //ਖਿੜਕੀ
#ਜੇ_ਅੰਤ

    // six fields that are only used when there is a WinScrolled autocommand
    linenr_T	w_ਆਖਰੀ_topline;	    // last known value for w_topline
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
    ਪੂਰਨ		w_ਆਖਰੀ_topfill;	    // last known value for w_topfill
#ਜੇ_ਅੰਤ
    colnr_T	w_ਆਖਰੀ_leftcol;	    // last known value for w_leftcol
    colnr_T	w_ਆਖਰੀ_skipcol;	    // last known value for w_skipcol
    ਪੂਰਨ		w_ਆਖਰੀ_ਚੌੜਾਈ;	    // last known value for w_width
    ਪੂਰਨ		w_ਆਖਰੀ_ਉਚਾਈ;	    // last known value for w_height

    /*
     * Layout of the window in the screen.
     * May need to add "msg_scrolled" to "w_winrow" in rare situations.
     */
    ਪੂਰਨ		w_winrow;	    // first row of window in screen
    ਪੂਰਨ		w_ਉਚਾਈ;	    // number of rows in window, excluding
				    // status/command/winbar line(s)
    ਪੂਰਨ		w_prev_winrow;	    // previous winrow used for 'splitkeep'
    ਪੂਰਨ		w_prev_ਉਚਾਈ;	    // previous height used for 'splitkeep'
    ਪੂਰਨ		w_stl_rendered_ਉਚਾਈ; // rendered height of window-local 'stl'
				    // (number of "%@" + 1)
    stl_click_region_T *w_stl_click;  // statusline click regions
    ਪੂਰਨ		w_stl_click_ਗਿਣਤੀ;  // number of click regions
    ਪੂਰਨ		w_ਸਥਿਤੀ_ਉਚਾਈ;    // number of status lines.
				    // If 'statuslineopt' was changed, this
				    // member holds the previous value until
				    // frame_change_statusline_height() calls.
    ਪੂਰਨ		w_wincol;	    // Leftmost column of window in screen.
    ਪੂਰਨ		w_ਚੌੜਾਈ;	    // Width of window, excluding separation.
    ਪੂਰਨ		w_vsep_ਚੌੜਾਈ;	    // Number of separator columns (0 or 1).

    pos_save_T	w_save_cursor;	    // backup of cursor pos and topline
    bool	w_do_win_fix_cursor;// if true cursor may be invalid

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    ਪੂਰਨ		w_popup_flags;	    // POPF_ values
    ਪੂਰਨ		w_popup_blend;	    // 0-100: transparency level for popup with opacitys
    ਪੂਰਨ		w_popup_handled;    // POPUP_HANDLE[0-9] flags
    char_u	*w_popup_title;
    poppos_T	w_popup_pos;
    bool	w_popup_fixed;	    // do not shift popup to fit on screen
    ਪੂਰਨ		w_popup_prop_type;  // when not zero: textprop type ID
    win_T	*w_popup_prop_win;  // window to search for textprop
    ਪੂਰਨ		w_popup_prop_id;    // when not zero: textprop ID
    ਪੂਰਨ		w_zindex;
    ਪੂਰਨ		w_minheight;	    // "minheight" for popup window
    ਪੂਰਨ		w_minwidth;	    // "minwidth" for popup window
    ਪੂਰਨ		w_maxheight;	    // "maxheight" for popup window
    ਪੂਰਨ		w_maxwidth;	    // "maxwidth" for popup window
    ਪੂਰਨ		w_maxwidth_opt;	    // maxwidth from option
    ਪੂਰਨ		w_wantline;	    // "line" for popup window
    ਪੂਰਨ		w_wantcol;	    // "col" for popup window
    ਪੂਰਨ		w_firstline;	    // "firstline" for popup window
    ਪੂਰਨ		w_want_scrollbar;   // when zero don't use a scrollbar
    bool	w_has_scrollbar;    // true if scrollbar displayed
    char_u	*w_scrollbar_highlight; // "scrollbarhighlight"
    char_u	*w_thumb_highlight; // "thumbhighlight"
    ਪੂਰਨ		w_popup_padding[4]; // popup padding top/right/bot/left
    ਪੂਰਨ		w_popup_border[4];  // popup border top/right/bot/left
    char_u	*w_border_highlight[4];  // popup border highlight
    bool	w_border_highlight_isset; // borderhighlight was explicitly set
    ਪੂਰਨ		w_border_char[8];   // popup border characters
    ਪੂਰਨ		w_popup_shadow;     // popup shadow (right and bottom edges)

    ਪੂਰਨ		w_popup_leftoff;    // columns left of the screen
    ਪੂਰਨ		w_popup_rightoff;   // columns right of the screen
    ਪੂਰਨ		w_popup_topoff;	    // rows above the host window's top
				    // when "clipwindow" is set
    ਪੂਰਨ		w_popup_bottomoff;  // rows below the host window's bottom
				    // when "clipwindow" is set
    ਪੂਰਨ		w_popup_leftclip;   // columns left of the host window's left
				    // when "clipwindow" is set
    ਪੂਰਨ		w_popup_rightclip;  // columns right of the host window's right
				    // when "clipwindow" is set
    varnumber_T	w_popup_ਆਖਰੀ_changedtick; // b:changedtick of popup buffer
					  // when position was computed
    varnumber_T	w_popup_prop_changedtick; // b:changedtick of buffer with
					  // w_popup_prop_type when position
					  // was computed
    ਪੂਰਨ		w_popup_prop_topline; // w_topline of window with
				      // w_popup_prop_type when position was
				      // computed
    ਪੂਰਨ		w_popup_prop_winrow;  // w_winrow of host window when
				      // position was computed
    ਪੂਰਨ		w_popup_prop_wincol;  // w_wincol of host window when
				      // position was computed
    ਪੂਰਨ		w_popup_prop_ਚੌੜਾਈ;   // w_width of host window when
				      // position was computed
    ਪੂਰਨ		w_popup_prop_winheight; // w_height of host window when
				      // position was computed
    linenr_T	w_popup_ਆਖਰੀ_curline; // last known w_cursor.lnum of window
				      // with "cursorline" set
    callback_T	w_close_cb;	    // popup close callback
    callback_T	w_filter_cb;	    // popup filter callback
    ਪੂਰਨ		w_filter_errors;    // popup filter error count
    ਪੂਰਨ		w_filter_ਬਹੁਲਕ;	    // mode when filter callback is used

    win_T	*w_popup_curwin;    // close popup if curwin differs
    linenr_T	w_popup_lnum;	    // close popup if cursor not on this line
    colnr_T	w_popup_mincol;	    // close popup if cursor before this col
    colnr_T	w_popup_maxcol;	    // close popup if cursor after this col
    ਪੂਰਨ		w_popup_mouse_ਪੰਕਤੀ;  // close popup if mouse moves away
    ਪੂਰਨ		w_popup_mouse_mincol;  // close popup if mouse moves away
    ਪੂਰਨ		w_popup_mouse_maxcol;  // close popup if mouse moves away
    popclose_T	w_popup_close;	    // allow closing the popup with the mouse

    list_T	*w_popup_ਨਕਾਬ;	     // list of lists for "mask"
    char_u	*w_popup_ਨਕਾਬ_cells; // cached mask cells
    ਪੂਰਨ		w_popup_ਨਕਾਬ_ਉਚਾਈ; // height of w_popup_mask_cells
    ਪੂਰਨ		w_popup_ਨਕਾਬ_ਚੌੜਾਈ;  // width of w_popup_mask_cells

# ਜੇ defined(FEAT_TIMERS)
    timer_T	*w_popup_timer;	    // timer for closing popup window
# endif

    ਪੂਰਨ		w_flags;	    // WFLAG_ flags

# define WFLAG_WCOL_OFF_ADDED	1   // popup border and padding were added to
				    // w_wcol
# define WFLAG_WROW_OFF_ADDED	2   // popup border and padding were added to
				    // w_wrow

# ifdef FEAT_IMAGE
    image_placement_T *w_popup_imagep;
# endif
#ਜੇ_ਅੰਤ

    /*
     * === start of cached values ====
     */
    /*
     * Recomputing is minimized by storing the result of computations.
     * Use functions in screen.c to check if they are valid and to update.
     * w_valid is a bitfield of flags, which indicate if specific values are
     * valid or need to be recomputed.	See screen.c for values.
     */
    ਪੂਰਨ		w_valid;
    pos_T	w_valid_cursor;	    // last known position of w_cursor, used
				    // to adjust w_valid
    colnr_T	w_valid_leftcol;    // last known w_leftcol
    colnr_T	w_valid_skipcol;    // last known w_skipcol

    /*
     * w_cline_height is the number of physical lines taken by the buffer line
     * that the cursor is on.  We use this to avoid extra calls to plines().
     */
    ਪੂਰਨ		w_cline_ਉਚਾਈ;	    // current size of cursor line
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
    bool	w_cline_folded;	    // cursor line is folded
#ਜੇ_ਅੰਤ

    ਪੂਰਨ		w_cline_ਪੰਕਤੀ;	    // starting row of the cursor line

    colnr_T	w_virtcol;	    // column number of the cursor in the
				    // buffer line, as opposed to the column
				    // number we're at on the screen.  This
				    // makes a difference on lines which span
				    // more than one screen line or when
				    // w_leftcol is non-zero

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    colnr_T	w_virtcol_ਪਹਿਲਾ_char;	// offset for w_virtcol when there are
					// virtual text properties above the
					//ਸਤਰ
#ਜੇ_ਅੰਤ
    /*
     * w_wrow and w_wcol specify the cursor position in the window.
     * This is related to positions in the window, not in the display or
     * buffer, thus w_wrow is relative to w_winrow.
     */
    ਪੂਰਨ		w_wrow, w_wcol;	    // cursor position in window
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CONCEAL
    ਪੂਰਨ		w_wcol_conceal_off; // screen cells concealed before w_wcol on
				    // the cursor's screen line, set by win_line()
#ਜੇ_ਅੰਤ

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
    ਪੂਰਨ		w_ਸਤਰਾਂ_valid;	    // number of valid entries
    wline_T	*w_ਸਤਰਾਂ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
    garray_T	w_folds;	    // array of nested folds
    bool	w_fold_manual;	    // when true: some folds are opened/closed
				    // manually
    bool	w_foldinvalid;	    // when true: folding needs to be
				    // recomputed
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
    ਪੂਰਨ		w_nrwidth;	    // width of 'number' and 'relativenumber'
				    // column being used
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
    termcellcolor_T w_ਪਦ_hlfwin;  // cache for term color of HLF_WIN
#ਜੇ_ਅੰਤ

    /*
     * === end of cached values ===
     */

    ਪੂਰਨ		w_redr_type;	    // type of redraw to be performed on win
    ਪੂਰਨ		w_upd_ਪੰਕਤੀਆਂ;	    // number of window lines to update when
				    // w_redr_type is UPD_REDRAW_TOP
    linenr_T	w_redraw_ਸਿਖਰ;	    // when != 0: first line needing redraw
    linenr_T	w_redraw_bot;	    // when != 0: last line needing redraw
    bool	w_redr_ਸਥਿਤੀ;	    // if true status line must be redrawn

    // remember what is shown in the ruler for this window (if 'ruler' set)
    pos_T	w_ru_cursor;	    // cursor position shown in ruler
    colnr_T	w_ru_virtcol;	    // virtcol shown in ruler
    linenr_T	w_ru_topline;	    // topline shown in ruler
    linenr_T	w_ru_ਸਤਰ_ਗਿਣਤੀ;    // line count used for ruler
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
    ਪੂਰਨ		w_ru_topfill;	    // topfill shown in ruler
#ਜੇ_ਅੰਤ
    bool	w_ru_ਖਾਲੀ_ਥਾਵਾਂ;	    // true if ruler shows 0-1 (empty line)

    ਪੂਰਨ		w_alt_fnum;	    // alternate file (for # and CTRL-^)

    alist_T	*w_alist;	    // pointer to arglist for this window
    ਪੂਰਨ		w_arg_idx;	    // current index in argument list (can be
				    // out of range!)
    bool	w_arg_idx_invalid;  // editing another file than w_arg_idx

    char_u	*w_localdir;	    // absolute path of local directory or
				    // NULL
    char_u	*w_prevdir;	    // previous directory
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MENU
    vimmenu_T	*w_winbar;	    // The root of the WinBar menu hierarchy.
    winbar_ਵਸਤੂ_T *w_winbar_ਚੀਜ਼ਾਂ;  // list of items in the WinBar
    ਪੂਰਨ		w_winbar_ਉਚਾਈ;    // 1 if there is a window toolbar
#ਜੇ_ਅੰਤ

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
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GLOBAL_WO(p)	((ਅੱਖਰ *)(p) + ਆਕਾਰ(winopt_T))

#ਜੇ defined(FEAT_SIGNS) || defined(FEAT_FOLDING) || defined(FEAT_DIFF)
    ਪੂਰਨ		*w_p_cc_ਖਾਨੇ_ਗਿਣਤੀ;	    // array of columns to highlight or NULL
    char_u	w_p_culopt_flags;   // flags for cursorline highlighting
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
    ਪੂਰਨ		w_briopt_min;	    // minimum width for breakindent
    ਪੂਰਨ		w_briopt_ਖਿਸਕਾਓ;	    // additional shift for breakindent
    ਪੂਰਨ		w_briopt_sbr;	    // sbr in 'briopt'
    ਪੂਰਨ		w_briopt_list;      // additional indent for lists
    ਪੂਰਨ		w_briopt_vcol;	    // indent for specific column
#ਜੇ_ਅੰਤ

    ਲੰਮਾ	w_scbind_pos;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    dictitem_T	w_winvar;	// variable for "w:" Dictionary
    dict_T	*w_vars;	// internal variables, local to window
#ਜੇ_ਅੰਤ

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
    ਪੂਰਨ		w_jumplistlen;		// number of active entries
    ਪੂਰਨ		w_jumplistidx;		// current position

    ਪੂਰਨ		w_changelistidx;	// current position in b_changelist

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਖੋਜ_EXTRA
    matchitem_T	*w_ਮੇਲ_ਸਿਰ;		// head of match list
    ਪੂਰਨ		w_ਅਗਲਾ_ਮੇਲ_id;	// next match ID
#ਜੇ_ਅੰਤ

    /*
     * the tagstack grows from 0 upwards:
     * entry 0: older
     * entry 1: newer
     * entry 2: newest
     */
    taggy_T	w_tagstack[TAGSTACKSIZE];   // the tag stack
    ਪੂਰਨ		w_tagstackidx;		    // idx just below active entry
    ਪੂਰਨ		w_tagstacklen;		    // number of tags on stack

    /*
     * w_fraction is the fractional row of the cursor within the window, from
     * 0 at the top row to FRACTION_MULT at the last row.
     * w_prev_fraction_row was the actual cursor row when w_fraction was last
     * calculated.
     */
    ਪੂਰਨ		w_fraction;
    ਪੂਰਨ		w_prev_fraction_ਪੰਕਤੀ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    scrollbar_T	w_scrollbars[2];	// vert. Scrollbars for this window
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
    linenr_T	w_nrwidth_ਸਤਰ_ਗਿਣਤੀ;	// line count when ml_nrwidth_width
					// was computed.
    ਲੰਮਾ	w_nuw_cached;		// 'numberwidth' option cached
    ਪੂਰਨ		w_nrwidth_ਚੌੜਾਈ;	// nr of chars to print line count.
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_QUICKFIX
    qf_info_T	*w_llist;		// Location list for this window
    /*
     * Location list reference used in the location list window.
     * In a non-location list window, w_llist_ref is NULL.
     */
    qf_info_T	*w_llist_ref;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MZSCHEME
    ਖਾਲੀ	*w_mzscheme_ref;	// The MzScheme value for this window
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PERL
    ਖਾਲੀ	*w_perl_ਨਿੱਜੀ;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PYTHON
    ਖਾਲੀ	*w_python_ref;		// The Python value for this window
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PYTHON3
    ਖਾਲੀ	*w_python3_ref;		// The Python value for this window
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TCL
    ਖਾਲੀ	*w_tcl_ref;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RUBY
    ਖਾਲੀ	*w_ruby_ref;
#ਜੇ_ਅੰਤ

    hl_override_T *w_hl;
    ਪੂਰਨ		w_hl_len;
    ਪੂਰਨ		w_hlfwin_id; // Cached HLF_WIN highlight group id, zero if none,
			     // or -1 if it was set to itself.
};

/*
 * Arguments for operators.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ oparg_S
{
    ਪੂਰਨ		op_type;	// current pending operator type
    ਪੂਰਨ		regname;	// register to use for the operator
    ਪੂਰਨ		motion_type;	// type of the current cursor motion
    ਪੂਰਨ		motion_force;	// force motion type: 'v', 'V' or CTRL-V
    ਪੂਰਨ		use_reg_one;	// TRUE if delete uses reg 1 even when not
				// linewise
    ਪੂਰਨ		inclusive;	// TRUE if char motion is inclusive (only
				// valid when motion_type is MCHAR)
    ਪੂਰਨ		ਸਿਰਾ_adjusted;	// backuped b_op_end one char (only used by
				// do_format())
    pos_T	ਸ਼ੁਰੂ;		// start of the operator
    pos_T	ਸਿਰਾ;		// end of the operator
    pos_T	cursor_ਸ਼ੁਰੂ;	// cursor position before motion for "gw"

    ਲੰਮਾ	ਸਤਰ_ਗਿਣਤੀ;	// number of lines from op_start to op_end
				// (inclusive)
    ਪੂਰਨ		ਖਾਲੀ_ਥਾਵਾਂ_2;		// op_start and op_end the same (only used by
				// do_change())
    ਪੂਰਨ		is_VIsual;	// operator on Visual area
    ਪੂਰਨ		ਖੰਡ_ਬਹੁਲਕ;	// current operator is Visual block mode
    colnr_T	ਸ਼ੁਰੂ_vcol;	// start col for block mode operator
    colnr_T	ਸਿਰਾ_vcol;	// end col for block mode operator
    ਲੰਮਾ	prev_opcount;	// ca.opcount saved for K_CURSORHOLD
    ਲੰਮਾ	prev_count0;	// ca.count0 saved for K_CURSORHOLD
    ਪੂਰਨ		excl_tr_ws;	// exclude trailing whitespace for yank of a
				//ਖੰਡ
} oparg_T;

/*
 * Arguments for Normal mode commands.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ cmdarg_S
{
    oparg_T	*oap;		// Operator arguments
    ਪੂਰਨ		prechar;	// prefix character (optional, always 'g')
    ਪੂਰਨ		cmdchar;	// command character
    ਪੂਰਨ		nchar;		// next command character (optional)
    ਪੂਰਨ		ncharC1;	// first composing character (optional)
    ਪੂਰਨ		ncharC2;	// second composing character (optional)
    ਪੂਰਨ		extra_char;	// yet another character (optional)
    ਲੰਮਾ	opcount;	// count before an operator
    ਲੰਮਾ	count0;		// count before command, default 0
    ਲੰਮਾ	count1;		// count before command, default 1
    ਪੂਰਨ		arg;		// extra argument from nv_cmds[]
    ਪੂਰਨ		retval;		// return: CA_* values
    char_u	*searchbuf;	// return: pointer to search pattern or NULL
} cmdarg_T;

// values for retval:
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CA_COMMAND_BUSY	    1	// skip restarting edit() once
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CA_NO_ADJ_OP_ਸਿਰਾ    2	// don't adjust operator end

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURSOR_SHAPE
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
# define SHAPE_IDX_ਸਥਿਤੀ 10	// A status line
# define SHAPE_IDX_SDRAG 11	// dragging a status line
# define SHAPE_IDX_VSEP	12	// A vertical separator line
# define SHAPE_IDX_VDRAG 13	// dragging a vertical separator line
# define SHAPE_IDX_ਹੋਰ	14	// Hit-return or More
# define SHAPE_IDX_MOREL	15	// Hit-return or More in last line
# define SHAPE_IDX_SM	16	// showing matching paren
# define SHAPE_IDX_ਗਿਣਤੀ	17

# define SHAPE_ਖੰਡ	0	// block cursor
# define SHAPE_HOR	1	// horizontal bar cursor
# define SHAPE_VER	2	// vertical bar cursor

# define MSHAPE_NUMBERED	1000	// offset for shapes identified by number
# define MSHAPE_HIDE	1	// hide mouse pointer

# define SHAPE_MOUSE	1	// used for mouse pointer shape
# define SHAPE_CURSOR	2	// used for text cursor shape

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ cursor_ਇੰਦਰਾਜ
{
    ਪੂਰਨ		shape;		// one of the SHAPE_ defines
    ਪੂਰਨ		mshape;		// one of the MSHAPE defines
    ਪੂਰਨ		percentage;	// percentage of cell for bar
    ਲੰਮਾ	blinkwait;	// blinking, wait time before blinking starts
    ਲੰਮਾ	blinkon;	// blinking, on time
    ਲੰਮਾ	blinkoff;	// blinking, off time
    ਪੂਰਨ		id;		// highlight group ID
    ਪੂਰਨ		id_lm;		// highlight group ID for :lmap mode
    ਅੱਖਰ	*name;		// mode name (fixed)
    ਅੱਖਰ	ਵਰਤਿਆ_for;	// SHAPE_MOUSE and/or SHAPE_CURSOR
} cursorentry_T;
#ਜੇ_ਅੰਤ // CURSOR_SHAPE

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MENU

// Indices into vimmenu_T->strings[] and vimmenu_T->noremap[] for each mode
# define MENU_ਸੂਚਕਾਂਕ_INVALID	-1
# define MENU_ਸੂਚਕਾਂਕ_ਸਾਧਾਰਨ	0
# define MENU_ਸੂਚਕਾਂਕ_VISUAL	1
# define MENU_ਸੂਚਕਾਂਕ_SELECT	2
# define MENU_ਸੂਚਕਾਂਕ_OP_PENDING	3
# define MENU_ਸੂਚਕਾਂਕ_ਪਾਓ	4
# define MENU_ਸੂਚਕਾਂਕ_CMDLINE	5
# define MENU_ਸੂਚਕਾਂਕ_TERMINAL	6
# define MENU_ਸੂਚਕਾਂਕ_TIP		7
# define MENU_MODES		8

// Menu modes
# define MENU_ਸਾਧਾਰਨ_ਬਹੁਲਕ	(1 << MENU_ਸੂਚਕਾਂਕ_ਸਾਧਾਰਨ)
# define MENU_VISUAL_ਬਹੁਲਕ	(1 << MENU_ਸੂਚਕਾਂਕ_VISUAL)
# define MENU_SELECT_ਬਹੁਲਕ	(1 << MENU_ਸੂਚਕਾਂਕ_SELECT)
# define MENU_OP_PENDING_ਬਹੁਲਕ	(1 << MENU_ਸੂਚਕਾਂਕ_OP_PENDING)
# define MENU_ਪਾਓ_ਬਹੁਲਕ	(1 << MENU_ਸੂਚਕਾਂਕ_ਪਾਓ)
# define MENU_CMDLINE_ਬਹੁਲਕ	(1 << MENU_ਸੂਚਕਾਂਕ_CMDLINE)
# define MENU_TERMINAL_ਬਹੁਲਕ	(1 << MENU_ਸੂਚਕਾਂਕ_TERMINAL)
# define MENU_TIP_ਬਹੁਲਕ		(1 << MENU_ਸੂਚਕਾਂਕ_TIP)
# define MENU_ALL_MODES		((1 << MENU_ਸੂਚਕਾਂਕ_TIP) - 1)
// note MENU_INDEX_TIP is not a 'real' mode

// Start a menu name with this to not include it on the main menu bar
# define MNU_HIDDEN_CHAR		']'

ਬਣਤਰ VimMenu
{
    ਪੂਰਨ		modes;		    // Which modes is this menu visible for?
    ਪੂਰਨ		enabled;	    // for which modes the menu is enabled
    char_u	*name;		    // Name of menu, possibly translated
    char_u	*dname;		    // Displayed Name ("name" without '&')
# ifdef FEAT_MULTI_LANG
    char_u	*en_name;	    // "name" untranslated, NULL when "name"
				    // was not translated
    char_u	*en_dname;	    // "dname" untranslated, NULL when "dname"
				    // was not translated
# endif
    char_u	*actext;	    // accelerator text (after TAB)
    ਪੂਰਨ		mnemonic;	    // mnemonic key (after '&')
    ਪੂਰਨ		ਤਰਜੀਹ;	    // Menu order priority
# ifdef FEAT_GUI
    ਖਾਲੀ	(*cb)(vimmenu_T *); // Call-back function
# endif
# ifdef FEAT_TOOLBAR
    char_u	*iconfile;	    // name of file for icon or NULL
    ਪੂਰਨ		iconidx;	    // icon index (-1 if not set)
    ਪੂਰਨ		icon_builtin;	    // icon names is BuiltIn{nr}
# endif
    char_u	*ਲੜੀਆਂ[MENU_MODES]; // Mapped string for each mode
    ਪੂਰਨ		noremap[MENU_MODES]; // A REMAP_ flag for each mode
    ਅੱਖਰ	silent[MENU_MODES]; // A silent flag for each mode
    vimmenu_T	*children;	    // Children of sub-menu
    vimmenu_T	*ਮਾਪਾ;	    // Parent of menu
    vimmenu_T	*ਅਗਲਾ;		    // Next item in menu
# ifdef FEAT_GUI_X11
    Widget	id;		    // Manage this to enable item
    Widget	submenu_id;	    // If this is submenu, add children here
# endif
# ifdef FEAT_GUI_GTK
    GtkWidget	*id;		    // Manage this to enable item
    GtkWidget	*submenu_id;	    // If this is submenu, add children here
#  ਜੇ defined(GTK_CHECK_ਸੰਸਕਰਣ) && !GTK_CHECK_ਸੰਸਕਰਣ(3,4,0)
    GtkWidget	*tearoff_handle;
#  endif
#  ifndef USE_GTK4
    GtkWidget   *label;		    // Used by "set wak=" code.
#  endif
# endif
# ifdef FEAT_GUI_MOTIF
    ਪੂਰਨ		sensitive;	    // turn button on/off
    ਅੱਖਰ	**xpm;		    // pixmap data
    ਅੱਖਰ	*xpm_fname;	    // file with pixmap data
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
    BMenu_ਵਸਤੂ  *id;		    // Id of menu item
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
#ਨਹੀਂਤਾਂ
// For generating prototypes when FEAT_MENU isn't defined.
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ vimmenu_T;

#ਜੇ_ਅੰਤ // FEAT_MENU

/*
 * Struct to save values in before executing autocommands for a buffer that is
 * not the current buffer.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    ਪੂਰਨ		use_aucmd_win_idx;  // index in aucmd_win[] if >= 0
    ਪੂਰਨ		save_curwin_id;	    // ID of saved curwin
    ਪੂਰਨ		new_curwin_id;	    // ID of new curwin
    ਪੂਰਨ		save_prevwin_id;    // ID of saved prevwin
    bufref_T	new_curbuf;	    // new curbuf
    char_u	*tp_localdir;	    // saved value of tp_localdir
    char_u	*globaldir;	    // saved value of globaldir
    ਪੂਰਨ		save_VIsual_ਸਰਗਰਮ; // saved VIsual_active
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
    ਪੂਰਨ		save_prompt_ਪਾਓ; // saved b_prompt_insert
#ਜੇ_ਅੰਤ
} aco_save_T;

/*
 * Generic option table item, only used for printer at the moment.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    ਅਚਲ ਅੱਖਰ	*name;
    ਪੂਰਨ		hasnum;
    ਲੰਮਾ	ਸੰਖਿਆ;
    char_u	*string;	// points into option string
    ਪੂਰਨ		ਲੜੀ_ਲੰਬਾਈ;
    ਪੂਰਨ		present;
} option_ਸਾਰਣੀ_T;

/*
 * Structure to hold printing color and font attributes.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    long_u	fg_color;
    long_u	bg_color;
    ਪੂਰਨ		bold;
    ਪੂਰਨ		italic;
    ਪੂਰਨ		underline;
    ਪੂਰਨ		undercurl;
} prt_ਪਾਠ_attr_T;

/*
 * Structure passed back to the generic printer code.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    ਪੂਰਨ		n_collated_copies;
    ਪੂਰਨ		n_uncollated_copies;
    ਪੂਰਨ		duplex;
    ਪੂਰਨ		chars_ਪ੍ਰਤੀ_ਸਤਰ;
    ਪੂਰਨ		ਸਤਰਾਂ_ਪ੍ਰਤੀ_ਪੰਨਾ;
    ਪੂਰਨ		has_color;
    prt_ਪਾਠ_attr_T ਸੰਖਿਆ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
    ਪੂਰਨ		modec;
    ਪੂਰਨ		do_syntax;
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		user_abort;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PRINT_PANGO
    ਪੂਰਨ		user_abort_ਸੁਨੇਹਾ;
#ਜੇ_ਅੰਤ
    char_u	*jobname;
#ਜੇ defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
    char_u	*outfile;
    char_u	*arguments;
#ਜੇ_ਅੰਤ
} prt_settings_T;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PRINT_ਸੰਖਿਆ_ਚੌੜਾਈ 8

/*
 * Used for popup menu items.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    char_u	*pum_ਪਾਠ;		// main menu text
    char_u	*pum_ਕਿਸਮ;		// extra kind text (may be truncated)
    char_u	*pum_extra;		// extra menu text (may be truncated)
    char_u	*pum_info;		// extra info
    ਪੂਰਨ		pum_cpt_ਸਰੋਤ_idx;	// index of completion source in 'cpt'
    ਪੂਰਨ		pum_user_abbr_hlattr;	// highlight attribute for abbr
    ਪੂਰਨ		pum_user_ਕਿਸਮ_hlattr;	// highlight attribute for kind
} pumitem_T;

/*
 * Structure used for get_tagfname().
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    char_u	*tn_ਟੈਗ_ਸੂਚੀ;	// value of 'tags' when starting
    char_u	*tn_np;		// current position in tn_tags
    ਪੂਰਨ		tn_did_filefind_init;
    ਪੂਰਨ		tn_hf_idx;
    ਖਾਲੀ	*tn_ਖੋਜ_ctx;
} tagname_T;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    UINT32_T ਕੁੱਲ[2];
    UINT32_T state[8];
    char_u   ਬਫਰ[64];
} context_sha256_T;

/*
 * types for expressions.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ
{
    ਵਿਅੰਜਕ_UNKNOWN = 0,
    ਵਿਅੰਜਕ_EQUAL,		// ==
    ਵਿਅੰਜਕ_NEQUAL,	// !=
    ਵਿਅੰਜਕ_GREATER,	// >
    ਵਿਅੰਜਕ_GEQUAL,	// >=
    ਵਿਅੰਜਕ_SMALLER,	// <
    ਵਿਅੰਜਕ_SEQUAL,	// <=
    ਵਿਅੰਜਕ_ਮੇਲ,		// =~
    ਵਿਅੰਜਕ_NOMATCH,	// !~
    ਵਿਅੰਜਕ_IS,		// is
    ਵਿਅੰਜਕ_ISNOT,		// isnot
    // used with ISN_OPNR
    ਵਿਅੰਜਕ_ਜੋੜੋ,		// +
    ਵਿਅੰਜਕ_SUB,		// -
    ਵਿਅੰਜਕ_MULT,		// *
    ਵਿਅੰਜਕ_DIV,		// /
    ਵਿਅੰਜਕ_REM,		// %
    ਵਿਅੰਜਕ_LSHIFT,	// <<
    ਵਿਅੰਜਕ_RSHIFT,	// >>
    // used with ISN_ADDLIST
    ਵਿਅੰਜਕ_COPY,		// create new list
    ਵਿਅੰਜਕ_APPEND,	// append to first list
} exprtype_T;

/*
 * Structure used for reading in json_decode().
 */
ਬਣਤਰ js_reader
{
    char_u	*js_buf;	// text to be decoded
    char_u	*js_ਸਿਰਾ;	// NUL in js_buf
    ਪੂਰਨ		js_ਵਰਤਿਆ;	// bytes used from js_buf
    ਪੂਰਨ		(*js_fill)(ਬਣਤਰ js_reader *);
				// function to fill the buffer or NULL;
				// return TRUE when the buffer was filled
    ਖਾਲੀ	*js_cookie;	// can be used by js_fill
    ਪੂਰਨ		js_cookie_arg;	// can be used by js_fill
};
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ js_reader js_read_T;

// Maximum number of commands from + or -c arguments.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_ARG_CMDS 10

// values for "window_layout"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WIN_HOR	    1	    // "-o" horizontally split windows
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ	WIN_VER	    2	    // "-O" vertically split windows
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ	WIN_TABS    3	    // "-p" windows on tab pages

// Struct for various parameters passed between main() and other functions.
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    ਪੂਰਨ		argc;
    ਅੱਖਰ	**argv;

    char_u	*fname;			// first file to edit

    ਪੂਰਨ		evim_ਬਹੁਲਕ;		// started as "evim"
    char_u	*use_vimrc;		// vimrc from -u argument
    ਪੂਰਨ		clean;			// --clean argument

    ਪੂਰਨ		n_commands;		     // no. of commands from + or -c
    char_u	*commands[MAX_ARG_CMDS];     // commands from + or -c arg.
    char_u	cmds_tofree[MAX_ARG_CMDS];   // commands that need free()
    ਪੂਰਨ		n_pre_commands;		     // no. of commands from --cmd
    char_u	*pre_commands[MAX_ARG_CMDS]; // commands from --cmd argument

    ਪੂਰਨ		edit_type;		// type of editing to do
    char_u	*tagname;		// tag from -t argument
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_QUICKFIX
    char_u	*use_ef;		// 'errorfile' from -q argument
#ਜੇ_ਅੰਤ

    ਪੂਰਨ		want_ਭਰਿਆ_screen;
    ਪੂਰਨ		not_a_ਪਦ;		// no warning for missing term?
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    char_u	*gui_dialog_file;	// file to write dialog text in
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		tty_fail;		// exit if not a tty
    char_u	*ਪਦ;			// specified terminal name
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CRYPT
    ਪੂਰਨ		ask_for_ਕੁੰਜੀ;		// -x argument
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		no_ਅਦਲਾ_ਬਦਲੀ_file;		// "-n" argument used
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਪੂਰਨ		use_debug_break_level;
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		ਖਿੜਕੀ_ਗਿਣਤੀ;		// number of windows to use
    ਪੂਰਨ		ਖਿੜਕੀ_layout;		// 0, WIN_HOR, WIN_VER or WIN_TABS

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIENTSERVER
    ਪੂਰਨ		serverArg;		// TRUE when argument for a server
    char_u	*serverName_arg;	// cmdline arg for server name
    char_u	*serverStr;		// remote server command
    char_u	*serverStrEnc;		// encoding of serverStr
    char_u	*servername;		// allocated name for our server
#ਜੇ_ਅੰਤ
#ਜੇ !defined(UNIX)
# define EXPAND_FILENAMES
    ਪੂਰਨ		literal;		// don't expand file names
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
    ਪੂਰਨ		ਭਰਿਆ_ਰਸਤਾ;		// file name argument was full path
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
    ਪੂਰਨ		diff_ਬਹੁਲਕ;		// start with 'diff' set
#ਜੇ_ਅੰਤ
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
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ lval_S
{
    char_u	*ll_name;	// start of variable name (can be NULL)
    char_u	*ll_name_ਸਿਰਾ;	// end of variable name (can be NULL)
    type_T	*ll_type;	// type of variable (can be NULL)
    char_u	*ll_exp_name;	// NULL or expanded name in allocated memory.

    scid_T	ll_sid;		// for an imported item: the script ID it was
				// imported from; zero otherwise

    typval_T	*ll_tv;		// Typeval of item being used.  If "newkey"
				// isn't NULL it's the Dict to which to add
				// the item.
    listitem_T	*ll_li;		// The list item or NULL.
    list_T	*ll_list;	// The list or NULL.
    ਪੂਰਨ		ll_range;	// TRUE when a [i:j] range was used
    ਪੂਰਨ		ll_empty2;	// Second index is empty: [i:]
    ਲੰਮਾ	ll_n1;		// First index for list
    ਲੰਮਾ	ll_n2;		// Second index for list range
    dict_T	*ll_dict;	// The Dictionary or NULL
    dictitem_T	*ll_di;		// The dictitem or NULL
    char_u	*ll_newkey;	// New key for Dict in alloc. mem or NULL.
    type_T	*ll_valtype;	// type expected for the value or NULL
    blob_T	*ll_blob;	// The Blob or NULL
    tuple_T	*ll_tuple;	// tuple or NULL
    ufunc_T	*ll_ufunc;	// The function or NULL
    object_T	*ll_object;	// The object or NULL, class is not NULL
    class_T	*ll_class;	// The class or NULL, object may be NULL
    ਪੂਰਨ		ll_oi;		// The object/class member index
    ਪੂਰਨ		ll_is_ਜੜ੍ਹ;	// TRUE if ll_tv is the lval_root, like a
				// plain object/class. ll_tv is variable.
    garray_T	ll_type_list;   // list of pointers to allocated types
} lval_T;

/**
 * This specifies optional parameters for get_lval(). Arguments may be NULL.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ lval_ਜੜ੍ਹ_S {
    typval_T	*lr_tv;		// Base typval.
    class_T	*lr_cl_exec;	// Executing class for access checking.
    ਪੂਰਨ		lr_is_arg;	// name is an arg (not a member).
} lval_ਜੜ੍ਹ_T;

// Structure used to save the current state.  Used when executing Normal mode
// commands while in any other mode.
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    ਪੂਰਨ		save_ਸੁਨੇਹਾ_scroll;
    ਪੂਰਨ		save_restart_edit;
    ਪੂਰਨ		save_ਸੁਨੇਹਾ_didout;
    ਪੂਰਨ		save_State;
    ਪੂਰਨ		save_insertmode;
    ਪੂਰਨ		save_ਅੰਤ_op;
    ਪੂਰਨ		save_opcount;
    ਪੂਰਨ		save_reg_executing;
    ਪੂਰਨ		save_pending_ਸਿਰਾ_reg_executing;
    ਪੂਰਨ		save_script_ਸੰਸਕਰਣ;
    tasave_T	tabuf;
} save_state_T;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    varnumber_T vv_prevcount;
    varnumber_T vv_ਗਿਣਤੀ;
    varnumber_T vv_count1;
} vimvars_save_T;

// Scope for changing directory
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
    CDSCOPE_GLOBAL,	// :cd
    CDSCOPE_TABPAGE,	// :tcd
    CDSCOPE_ਖਿੜਕੀ	// :lcd
} cdscope_T;

// Variable flavor
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ
{
    VAR_FLAVOUR_DEFAULT,	// doesn't start with uppercase
    VAR_FLAVOUR_SESSION,	// starts with uppercase, some lower
    VAR_FLAVOUR_VIMINFO		// all uppercase
} var_flavour_T;

// argument for mouse_find_win()
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
    IGNORE_POPUP,	// only check non-popup windows
    ਲੱਭੋ_POPUP,		// also find popup windows
    FAIL_POPUP		// return NULL if mouse on popup window
} mouse_ਲੱਭੋ_T;

// Symbolic names for some registers.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DELETION_ਰਜਿਸਟਰ	36
#ਜੇ defined(FEAT_CLIPBOARD) || defined(HAVE_CLIPMETHOD)
# define ਤਾਰਾ_ਰਜਿਸਟਰ		37
# ਜੇ defined(FEAT_X11) || defined(FEAT_WAYLAND)
#  define PLUS_ਰਜਿਸਟਰ	38
#  define REAL_PLUS_ਰਜਿਸਟਰ	PLUS_ਰਜਿਸਟਰ
# ਨਹੀਂਤਾਂ
#  define PLUS_ਰਜਿਸਟਰ	ਤਾਰਾ_ਰਜਿਸਟਰ	    // there is only one
#  ifdef FEAT_EVAL
// Make it so that if clipmethod is "none", the plus register is not available,
// but if clipmethod is a provider, then expose the plus register for use.
#   define REAL_PLUS_ਰਜਿਸਟਰ	38
#  ਨਹੀਂਤਾਂ
#   define REAL_PLUS_ਰਜਿਸਟਰ	ਤਾਰਾ_ਰਜਿਸਟਰ
#  endif
# endif
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DND
# define TILDE_ਰਜਿਸਟਰ		(PLUS_ਰਜਿਸਟਰ + 1)
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIPBOARD
# ifdef FEAT_DND
#  define ਅੰਕ_REGISTERS		(TILDE_ਰਜਿਸਟਰ + 1)
# ਨਹੀਂਤਾਂ
#  define ਅੰਕ_REGISTERS		(REAL_PLUS_ਰਜਿਸਟਰ + 1)
# endif
#ਨਹੀਂਤਾਂ
# ifdef HAVE_CLIPMETHOD
#  define ਅੰਕ_REGISTERS		(REAL_PLUS_ਰਜਿਸਟਰ + 1)
# ਨਹੀਂਤਾਂ
#  define ਅੰਕ_REGISTERS		37
# endif
#ਜੇ_ਅੰਤ

// structure used by block_prep, op_delete and op_yank for blockwise operators
// also op_change, op_shift, op_insert, op_replace - AKelly
ਬਣਤਰ ਖੰਡ_def
{
    ਪੂਰਨ		startspaces;	// 'extra' cols before first char
    ਪੂਰਨ		endspaces;	// 'extra' cols after last char
    ਪੂਰਨ		textlen;	// chars in block
    char_u	*textstart;	// pointer to 1st char (partially) in block
    colnr_T	textcol;	// index of chars (partially) in block
    colnr_T	ਸ਼ੁਰੂ_vcol;	// start col of 1st char wholly inside block
    colnr_T	ਸਿਰਾ_vcol;	// start col of 1st char wholly after block
    ਪੂਰਨ		is_short;	// TRUE if line is too short to fit in block
    ਪੂਰਨ		is_MAX;		// TRUE if curswant==MAXCOL when starting
    ਪੂਰਨ		is_oneChar;	// TRUE if block within one character
    ਪੂਰਨ		pre_whitesp;	// screen cols of ws before block
    ਪੂਰਨ		pre_whitesp_c;	// chars of ws before block
    colnr_T	ਸਿਰਾ_char_vcols;	// number of vcols of post-block char
    colnr_T	ਸ਼ੁਰੂ_char_vcols; // number of vcols of pre-block char
};

// Each yank register has an array of pointers to lines.
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    string_T	*y_array;	// pointer to array of string_T structs
    linenr_T	y_ਮਾਪ;		// number of lines in y_array
    char_u	y_type;		// MLINE, MCHAR or MBLOCK
    colnr_T	y_ਚੌੜਾਈ;	// only set if y_type == MBLOCK
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VIMINFO
    time_t	y_ਸਮਾਂ_ਸੈੱਟ;
#ਜੇ_ਅੰਤ
} yankreg_T;

// The offset for a search command is store in a soff struct
// Note: only spats[0].off is really used
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ soffset
{
    ਪੂਰਨ		dir;		// search direction, '/' or '?'
    ਪੂਰਨ		ਸਤਰ;		// search has line offset
    ਪੂਰਨ		ਸਿਰਾ;		// search set cursor at end
    ਲੰਮਾ	off;		// line or char offset
} soffset_T;

// A search pattern and its attributes are stored in a spat struct
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ spat
{
    char_u	    *pat;	// the pattern (in allocated memory) or NULL
    size_t	    patlen;	// the length of the pattern (0 if pat is NULL)
    ਪੂਰਨ		    magic;	// magicness of the pattern
    ਪੂਰਨ		    no_scs;	// no smartcase for this pattern
    soffset_T	    off;
} spat_T;

/*
 * Optional extra arguments for searchit().
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    linenr_T	sa_stop_lnum;	// stop after this line number when != 0
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RELTIME
    ਲੰਮਾ	sa_tm;		// timeout limit or zero
    ਪੂਰਨ		sa_timed_ਬਾਹਰਲਾ;	// set when timed out
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		sa_wrapped;	// search wrapped around
} searchit_arg_T;

/*
 * Cookie used by getsourceline().
 */
/*
 * Cookie used to store info for each sourced file.
 * It is shared between do_source() and getsourceline().
 * This is passed to do_cmdline().
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    FILE	*fp;		// opened file for sourcing
    char_u	*nextline;	// if not NULL: line that was read ahead
    linenr_T	sourcing_lnum;	// line number of the source file
    ਪੂਰਨ		ਮੁਕੰਮਲ;	// ":finish" used
    ਪੂਰਨ		ਸਰੋਤ_from_buf;// TRUE if sourcing from current buffer
    ਪੂਰਨ		buf_lnum;	// line number in the current buffer
    garray_T	buflines;	// lines in the current buffer
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_CRNL
    ਪੂਰਨ		fileformat;	// EOL_UNKNOWN, EOL_UNIX or EOL_DOS
    ਪੂਰਨ		error;		// TRUE if LF found after CR-LF
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    linenr_T	breakpoint;	// next line with breakpoint or zero
    char_u	*fname;		// name of sourced file
    ਪੂਰਨ		dbg_tick;	// debug_tick when breakpoint was set
    ਪੂਰਨ		level;		// top nesting level of sourced file
#ਜੇ_ਅੰਤ
    vimconv_T	conv;		// type of conversion
} ਸਰੋਤ_cookie_T;


#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WRITEBUFSIZE	8192	// size of normal write buffer

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FIO_LATIN1	0x01	// convert Latin1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FIO_UTF8	0x02	// convert UTF-8
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FIO_UCS2	0x04	// convert UCS-2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FIO_UCS4	0x08	// convert UCS-4
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FIO_UTF16	0x10	// convert UTF-16
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
# define FIO_CODEPAGE	0x20	// convert MS-Windows codepage
# define FIO_PUT_CP(x) (((x) & 0xffff) << 16)	// put codepage in top word
# define FIO_GET_CP(x)	(((x)>>16) & 0xffff)	// get codepage from top word
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MACOS_CONVERT
# define FIO_MACROMAN	0x20	// convert MacRoman
#ਜੇ_ਅੰਤ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FIO_ENDIAN_L	0x80	// little endian
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FIO_ENCRYPTED	0x1000	// encrypt written bytes
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FIO_NOCONVERT	0x2000	// skip encoding conversion
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FIO_UCSBOM	0x4000	// check for BOM at start of file
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FIO_ALL	(-1)	// allow all formats

// When converting, a read() or write() may leave some bytes to be converted
// for the next call.  The value is guessed...
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CONV_RESTLEN 30

// We have to guess how much a sequence of bytes may expand when converting
// with iconv() to be able to allocate a buffer.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ICONV_MULT 8

// Used for "magic_overruled".
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
    OPTION_MAGIC_NOT_ਸੈੱਟ,	// p_magic not overruled
    OPTION_MAGIC_ON,		// magic on inside regexp
    OPTION_MAGIC_OFF		// magic off inside regexp
} optmagic_T;

// Magicness of a pattern, used by regexp code.
// The order and values matter:
//  magic <= MAGIC_OFF includes MAGIC_NONE
//  magic >= MAGIC_ON  includes MAGIC_ALL
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
    MAGIC_NONE = 1,		// "\V" very unmagic
    MAGIC_OFF = 2,		// "\M" or 'magic' off
    MAGIC_ON = 3,		// "\m" or 'magic'
    MAGIC_ALL = 4		// "\v" very magic
} magic_T;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
    WT_UNKNOWN = 0,	// Unknown or unspecified location
    WT_ਦਲੀਲ,
    WT_VARIABLE,
    WT_MEMBER,
    WT_ਵਿਧੀ,		// object method
    WT_ਵਿਧੀ_ARG,	// object method argument type
    WT_ਵਿਧੀ_RETURN,	// object method return type
    WT_CAST,		// type cast
} wherekind_T;

// Struct used to pass the location of a type check.  Used in error messages to
// indicate where the error happened.  Also used for doing covariance type
// check for object method return type and contra-variance type check for
// object method arguments.
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    ਅੱਖਰ	*wt_func_name;  // function name or NULL
    ਅੱਖਰ	wt_ਸੂਚਕਾਂਕ;	// argument or variable index, 0 means unknown
    wherekind_T	wt_ਕਿਸਮ;	// type check location
} where_T;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WHERE_INIT {ਨਿਰਾ, 0, WT_UNKNOWN}

// Struct passed to get_v_event() and restore_v_event().
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    ਪੂਰਨ		sve_did_save;
    hashtab_T	sve_hashtab;
} save_v_event_T;

// Enum used by filter(), map(), mapnew() and foreach()
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
    FILTERMAP_FILTER,
    FILTERMAP_MAP,
    FILTERMAP_MAPNEW,
    FILTERMAP_FOREACH
} filtermap_T;

// Structure used by switch_win() to pass values to restore_win()
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    win_T	*sw_curwin;
    tabpage_T	*sw_curtab;
    ਪੂਰਨ		sw_same_win;	    // VIsual_active was not reset
    ਪੂਰਨ		sw_visual_ਸਰਗਰਮ;
} switchwin_T;

// Fuzzy matched string list item. Used for fuzzy match completion. Items are
// usually sorted by 'score'. The 'idx' member is used for stable-sort.
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    ਪੂਰਨ		idx;
    char_u	*str;
    ਪੂਰਨ		score;
} fuzmatch_str_T;

// Argument for lbr_chartabsize().
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    win_T	*cts_win;
    char_u	*cts_ਸਤਰ;		// start of the line
    char_u	*cts_ptr;		// current position in line
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
    ਪੂਰਨ		cts_bri_ਮਾਪ;		// cached size of 'breakindent', or -1
					// if not computed yet
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    ਪੂਰਨ		cts_ਪਾਠ_prop_ਗਿਣਤੀ;	// number of text props; when zero
					// cts_text_props is not used
    textprop_T	*cts_ਪਾਠ_props;	// text props (allocated)
    ਅੱਖਰ	cts_has_prop_with_ਪਾਠ;	// TRUE if a property inserts text
    ਪੂਰਨ		cts_cur_ਪਾਠ_ਚੌੜਾਈ;	// width of current inserted text
    ਪੂਰਨ		cts_prop_ਸਤਰਾਂ;		// nr of properties above or below
    bool	cts_has_ਹੇਠਾਂ;		// true if a text property below was
					// counted, its width fills up the line
    ਪੂਰਨ		cts_ਪਹਿਲਾ_char;		// width text props above the line
    ਪੂਰਨ		cts_above_ਚੌੜਾਈ;	// width of text props above the line,
					// kept for the whole line
    bool	cts_no_above;		// do not count the width of text props
					// above the line
    ਪੂਰਨ		cts_with_trailing;	// include size of trailing props with
					// last character
    ਪੂਰਨ		cts_ਸ਼ੁਰੂ_incl;		// prop has true "start_incl" arg
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		cts_vcol;		// virtual column at current position
    ਪੂਰਨ		cts_max_ਸਿਰ_vcol;	// see win_lbr_chartabsize()
} chartabsize_T;

/*
 * Argument for the callback function (opt_did_set_cb_T) invoked after an
 * option value is modified.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    // Pointer to the option variable.  The variable can be a long (numeric
    // option), an int (boolean option) or a char pointer (string option).
    char_u	*os_varp;
    ਪੂਰਨ		os_idx;
    ਪੂਰਨ		os_flags;
    ਸੈੱਟ_op_T	os_op;

    // old value of the option (can be a string, number or a boolean)
    ਸੰਘ
    {
	ਲੰਮਾ	ਸੰਖਿਆ;
	ਪੂਰਨ	boolean;
	char_u	*string;
    } os_oldval;

    // new value of the option (can be a string, number or a boolean)
    ਸੰਘ
    {
	ਲੰਮਾ	ਸੰਖਿਆ;
	ਪੂਰਨ	boolean;
	char_u	*string;
    } os_newval;

    // Option value was checked to be safe, no need to set P_INSECURE
    // Used for the 'keymap', 'filetype' and 'syntax' options.
    ਪੂਰਨ		os_ਮੁੱਲ_checked;
    // Option value changed.  Used for the 'filetype' and 'syntax' options.
    ਪੂਰਨ		os_ਮੁੱਲ_ਬਦਲਿਆ;

    // Used by the 'isident', 'iskeyword', 'isprint' and 'isfname' options.
    // Set to TRUE if the character table is modified when processing the
    // option and need to be restored because of a failure.
    ਪੂਰਨ		os_restore_chartab;

#ਜੇ defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
    // Used by the 't_xxx' terminal options on MS-Windows.
    ਪੂਰਨ		os_did_swaptcap;
#ਜੇ_ਅੰਤ

    // If the value specified for an option is not valid and the error message
    // is parameterized, then the "os_errbuf" buffer is used to store the error
    // message (when it is not NULL).
    ਅੱਖਰ	*os_errbuf;
    // length of the error buffer
    size_t	os_errbuflen;
} optset_T;

/*
 * Argument for the callback function (opt_expand_cb_T) invoked after a string
 * option value is expanded for cmdline completion.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    // Pointer to the option variable. It's always a string.
    char_u	*oe_varp;
    // The original option value, escaped.
    char_u	*oe_opt_ਮੁੱਲ;

    // TRUE if using set+= instead of set=
    ਪੂਰਨ		oe_append;
    // TRUE if we would like to add the original option value as the first
    // choice.
    ਪੂਰਨ		oe_include_orig_val;

    // Regex from the cmdline, for matching potential options against.
    regmatch_T	*oe_regmatch;
    // The expansion context.
    expand_T	*oe_xp;

    // The full argument passed to :set. For example, if the user inputs
    // ':set dip=icase,algorithm:my<Tab>', oe_xp->xp_pattern will only have
    // 'my', but oe_set_arg will contain the whole 'icase,algorithm:my'.
    char_u	*oe_ਸੈੱਟ_arg;
} optexpand_T;

/*
 * Spell checking variables passed from win_update() to win_line().
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    ਪੂਰਨ		spv_has_spell;	    // drawn window has spell checking
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SPELL
    ਪੂਰਨ		spv_unchanged;	    // not updating for changed text
    ਪੂਰਨ		spv_checked_ਖਾਨਾ;    // column in "checked_lnum" up to
				    // which there are no spell errors
    linenr_T	spv_checked_lnum;   // line number for "checked_col"
    ਪੂਰਨ		spv_cap_ਖਾਨਾ;	    // column to check for Cap word
    linenr_T	spv_capcol_lnum;    // line number for "cap_col"
#ਜੇ_ਅੰਤ
} spellvars_T;

// Return the length of a string literal.
// This macro only computes a string's length for a string-literal token; for
// anything else, including a char*, compilation will fail (note "" following
// s).
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STRLEN_LITERAL(s) (ਆਕਾਰ(s "") - 1)

// Store a key/value (string) pair
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    ਪੂਰਨ	    ਕੁੰਜੀ;        // the key
    string_T ਮੁੱਲ;	// the value
} keyvalue_T;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ KEYVALUE_ਇੰਦਰਾਜ(k, v) \
    {(k), {((char_u *)v), STRLEN_LITERAL(v)}}

#ਜੇ defined(UNIX) || defined(MSWIN) || defined(VMS) || defined(AMIGA)
// Defined as signed, to return -1 on error
ਬਣਤਰ cellsize {
    ਪੂਰਨ cs_xpixel;
    ਪੂਰਨ cs_ypixel;
};
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_WAYLAND

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ vwl_connection_S vwl_connection_T;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ vwl_seat_S vwl_seat_T;

# ifdef FEAT_WAYLAND_CLIPBOARD

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ vwl_ਅੰਕੜੇ_offer_S vwl_ਅੰਕੜੇ_offer_T;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ vwl_ਅੰਕੜੇ_ਸਰੋਤ_S vwl_ਅੰਕੜੇ_ਸਰੋਤ_T;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ vwl_ਅੰਕੜੇ_device_S vwl_ਅੰਕੜੇ_device_T;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ vwl_ਅੰਕੜੇ_device_manager_S vwl_ਅੰਕੜੇ_device_manager_T;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ vwl_ਅੰਕੜੇ_device_listener_S vwl_ਅੰਕੜੇ_device_listener_T;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ vwl_ਅੰਕੜੇ_ਸਰੋਤ_listener_S vwl_ਅੰਕੜੇ_ਸਰੋਤ_listener_T;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ vwl_ਅੰਕੜੇ_offer_listener_S vwl_ਅੰਕੜੇ_offer_listener_T;

// Wayland selections
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
    WAYLAND_SELECTION_NONE	= 0,
    WAYLAND_SELECTION_REGULAR	= 1 << 0,
    WAYLAND_SELECTION_PRIMARY	= 1 << 1,
} wayland_selection_T;

# endif
#ਜੇ_ਅੰਤ

// Used in term_set_sync_output()
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ
{
    ਪਦ_SYNC_OUTPUT_ENABLE = 1 << 0,
    ਪਦ_SYNC_OUTPUT_DISABLE = 1 << 1,
    ਪਦ_SYNC_OUTPUT_OFF = 1 << 2,
    ਪਦ_SYNC_OUTPUT_FLUSH = 1 << 3,
} ਪਦ_sync_output_T;
