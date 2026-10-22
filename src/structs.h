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
#اگر defined(SASC) && SASC < 658
قسمتعریف طویل		linenr_T;
قسمتعریف صحیح		colnr_T;
قسمتعریف غیرعلامتی مختصر	short_u;
#اگر_ختم

// structure to store a string (including it's length)
قسمتعریف ساخت
{
    char_u  *string;		// the string
    size_t  لمبائی;		// length of the string (excluding the NUL)
} string_T;

/*
 * Position in file or buffer.
 */
قسمتعریف ساخت
{
    linenr_T	lnum;	// line number
    colnr_T	ستون;	// column number
    colnr_T	coladd; // extra virtual column
} pos_T;


/*
 * Same, but without coladd.
 */
قسمتعریف ساخت
{
    linenr_T	lnum;	// line number
    colnr_T	ستون;	// column number
} lpos_T;

/*
 * Structure used for growing arrays.
 * This is used to store information that only grows, is deleted all at
 * once, and needs to be accessed by index.  See ga_clear() and ga_grow().
 */
قسمتعریف ساخت growarray
{
    صحیح	    ga_len;		    // current number of items used
    صحیح	    ga_maxlen;		    // maximum number of items possible
    صحیح	    ga_itemsize;	    // sizeof(item)
    صحیح	    ga_growsize;	    // number of items to grow each time
    خالی    *ga_ڈیٹا;		    // pointer to the first item
} garray_T;

#تعریف_کرو GA_خالی_خانے    {0, 0, 0, 0, تہی}

// On rare systems "char" is unsigned, sometimes we really want a signed 8-bit
// value.
قسمتعریف علامتی حرف	int8_T;
قسمتعریف دوہرا		float_T;

قسمتعریف ساخت typval_S		typval_T;
قسمتعریف ساخت listvar_S	list_T;
قسمتعریف ساخت dictvar_S	dict_T;
قسمتعریف ساخت partial_S	partial_T;
قسمتعریف ساخت blobvar_S	blob_T;
قسمتعریف ساخت tuplevar_S	tuple_T;
قسمتعریف ساخت generictype_S	generic_T;
قسمتعریف ساخت gfargs_tab_S	gfargs_tab_T;

قسمتعریف ساخت کھڑکی_S		win_T;
قسمتعریف ساخت wininfo_S	wininfo_T;
قسمتعریف ساخت چوکھٹا_S		چوکھٹا_T;
قسمتعریف صحیح			scid_T;		// script ID
قسمتعریف ساخت file_بفر	buf_T;		// forward declaration
قسمتعریف ساخت terminal_S	رکن_T;

#اگر_تعریف_شدہ FEAT_MENU
قسمتعریف ساخت VimMenu vimmenu_T;
#اگر_ختم

// maximum value for sc_version
#تعریف_کرو SCRIPT_نسخہ_MAX 4
// value for sc_version in a Vim9 script file
#تعریف_کرو SCRIPT_نسخہ_VIM9 999999

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
قسمتعریف ساخت {
#اگر_تعریف_شدہ FEAT_EVAL
    scid_T	sc_sid;		// script ID
    صحیح		sc_seq;		// sourcing sequence number
    linenr_T	sc_lnum;	// line number
#اگر_ختم
    صحیح		sc_نسخہ;	// :scriptversion
} sctx_T;

/*
 * Reference to a buffer that stores the value of buf_free_count.
 * bufref_valid() only needs to check "buf" when the count differs.
 */
قسمتعریف ساخت {
    buf_T   *br_buf;
    صحیح	    br_fnum;
    صحیح	    br_buf_free_گنتی;
} bufref_T;

/*
 * This is here because regexp.h needs pos_T and below regprog_T is used.
 */
#شامل "regexp.h"

/*
 * This is here because gui.h needs the pos_T and win_T, and win_T needs gui.h
 * for scrollbar_T.
 */
#اگر_تعریف_شدہ FEAT_GUI
# include "gui.h"
#ورنہ
# ifdef FEAT_XCLIPBOARD
#  include <X11/Intrinsic.h>
# endif
# define guicolor_T طویل
# define INVALCOLOR ((guicolor_T)0x1ffffff)
    // only used for cterm.bg_rgb and cterm.fg_rgb: use cterm color
# define CTERMCOLOR ((guicolor_T)0x1fffffe)
#اگر_ختم
// We have include guard in the file
#شامل "image.h"
#تعریف_کرو COLOR_INVALID(x) ((x) == INVALCOLOR || (x) == CTERMCOLOR)

#اگر_تعریف_شدہ FEAT_TERMINAL
# include "libvterm/include/vterm.h"
قسمتعریف ساخت {
    VTermColor	fg;
    VTermColor	bg;
} termcellcolor_T;
#اگر_ختم

/*
 * marks: positions in a file
 * (a normal mark is a lnum/col pair, the same as a file position)
 */

#تعریف_کرو NMARKS		('z' - 'a' + 1)	// max. # of named marks
#تعریف_کرو EXTRA_MARKS	10		// marks 0-9
#تعریف_کرو JUMPLISTSIZE	100		// max. # of marks in jump list
#تعریف_کرو TAGSTACKSIZE	20		// max. # of tags in tag stack

قسمتعریف ساخت filemark
{
    pos_T	mark;		// cursor position
    صحیح		fnum;		// file number
} fmark_T;

// Xtended file mark: also has a file name
قسمتعریف ساخت xfilemark
{
    fmark_T	fmark;
    char_u	*fname;		// file name, used when fnum == 0
#اگر_تعریف_شدہ FEAT_VIMINFO
    وقت_T	وقت_سیٹ;
#اگر_ختم
} xfmark_T;

/*
 * The taggy struct is used to store the information about a :tag command.
 */
قسمتعریف ساخت taggy
{
    char_u	*tagname;	// tag name
    fmark_T	fmark;		// cursor position BEFORE ":tag"
    صحیح		cur_مطابقت;	// match number
    صحیح		cur_fnum;	// buffer number used for cur_match
    char_u	*user_ڈیٹا;	// used with tagfunc
} taggy_T;

/*
 * Structure that contains all options that are local to a window.
 * Used twice in a window: for the current buffer and for all buffers.
 * Also used in wininfo_T.
 */
قسمتعریف ساخت
{
#اگر_تعریف_شدہ FEAT_ARABIC
    صحیح		wo_arab;
# define w_p_arab w_onebuf_opt.wo_arab	// 'arabic'
#اگر_ختم
#اگر_تعریف_شدہ FEAT_LINEBREAK
    صحیح		wo_bri;
# define w_p_bri w_onebuf_opt.wo_bri	// 'breakindent'
    char_u	*wo_briopt;
# define w_p_briopt w_onebuf_opt.wo_briopt // 'breakindentopt'
#اگر_ختم
    char_u	*wo_wcr;
#تعریف_کرو w_p_wcr w_onebuf_opt.wo_wcr	// 'wincolor'
#اگر_تعریف_شدہ FEAT_DIFF
    صحیح		wo_diff;
# define w_p_diff w_onebuf_opt.wo_diff	// 'diff'
#اگر_ختم
    char_u	*wo_eiw;
#تعریف_کرو w_p_eiw w_onebuf_opt.wo_eiw	// 'eventignorewin'
#اگر_تعریف_شدہ FEAT_FOLDING
    طویل	wo_fdc;
# define w_p_fdc w_onebuf_opt.wo_fdc	// 'foldcolumn'
    صحیح		wo_fdc_save;
# define w_p_fdc_save w_onebuf_opt.wo_fdc_save	// 'foldenable' saved for diff mode
    صحیح		wo_fen;
# define w_p_fen w_onebuf_opt.wo_fen	// 'foldenable'
    صحیح		wo_fen_save;
# define w_p_fen_save w_onebuf_opt.wo_fen_save	// 'foldenable' saved for diff mode
    char_u	*wo_fdi;
# define w_p_fdi w_onebuf_opt.wo_fdi	// 'foldignore'
    طویل	wo_fdl;
# define w_p_fdl w_onebuf_opt.wo_fdl	// 'foldlevel'
    صحیح		wo_fdl_save;
# define w_p_fdl_save w_onebuf_opt.wo_fdl_save	// 'foldlevel' state saved for diff mode
    char_u	*wo_fdm;
# define w_p_fdm w_onebuf_opt.wo_fdm	// 'foldmethod'
    char_u	*wo_fdm_save;
# define w_p_fdm_save w_onebuf_opt.wo_fdm_save	// 'fdm' saved for diff mode
    طویل	wo_fml;
# define w_p_fml w_onebuf_opt.wo_fml	// 'foldminlines'
    طویل	wo_fdn;
# define w_p_fdn w_onebuf_opt.wo_fdn	// 'foldnestmax'
# ifdef FEAT_EVAL
    char_u	*wo_fde;
#  define w_p_fde w_onebuf_opt.wo_fde	// 'foldexpr'
    char_u	*wo_fdt;
#  define w_p_fdt w_onebuf_opt.wo_fdt	// 'foldtext'
# endif
    char_u	*wo_fmr;
# define w_p_fmr w_onebuf_opt.wo_fmr	// 'foldmarker'
#اگر_ختم
#اگر_تعریف_شدہ FEAT_LINEBREAK
    صحیح		wo_lbr;
# define w_p_lbr w_onebuf_opt.wo_lbr	// 'linebreak'
#اگر_ختم
    صحیح		wo_list;
#تعریف_کرو w_p_list w_onebuf_opt.wo_list	// 'list'
    char_u	*wo_lcs;
#تعریف_کرو w_p_lcs w_onebuf_opt.wo_lcs	// 'listchars'
    char_u	*wo_fcs;
#تعریف_کرو w_p_fcs w_onebuf_opt.wo_fcs	// 'fillchars'
    صحیح		wo_nu;
#تعریف_کرو w_p_nu w_onebuf_opt.wo_nu	// 'number'
    صحیح		wo_rnu;
#تعریف_کرو w_p_rnu w_onebuf_opt.wo_rnu	// 'relativenumber'
    char_u	*wo_ve;
#تعریف_کرو w_p_ve w_onebuf_opt.wo_ve	// 'virtualedit'
    غیرعلامتی	wo_ve_flags;
#تعریف_کرو	w_ve_flags w_onebuf_opt.wo_ve_flags	// flags for 'virtualedit'
#اگر_تعریف_شدہ FEAT_LINEBREAK
    طویل	wo_nuw;
# define w_p_nuw w_onebuf_opt.wo_nuw	// 'numberwidth'
#اگر_ختم
    صحیح wo_wfb;
#تعریف_کرو w_p_wfb w_onebuf_opt.wo_wfb	// 'winfixbuf'
    صحیح		wo_wfh;
#تعریف_کرو w_p_wfh w_onebuf_opt.wo_wfh	// 'winfixheight'
    صحیح		wo_wfw;
#تعریف_کرو w_p_wfw w_onebuf_opt.wo_wfw	// 'winfixwidth'
#اگر defined(FEAT_QUICKFIX)
    صحیح		wo_pvw;
# define w_p_pvw w_onebuf_opt.wo_pvw	// 'previewwindow'
    طویل        wo_lhi;
# define w_p_lhi w_onebuf_opt.wo_lhi    // 'lhistory'
#اگر_ختم
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
    صحیح		wo_rl;
# define w_p_rl w_onebuf_opt.wo_rl	// 'rightleft'
    char_u	*wo_rlc;
# define w_p_rlc w_onebuf_opt.wo_rlc	// 'rightleftcmd'
#اگر_ختم
    طویل	wo_scr;
#تعریف_کرو w_p_scr w_onebuf_opt.wo_scr	// 'scroll'
    صحیح		wo_sms;
#تعریف_کرو w_p_sms w_onebuf_opt.wo_sms	// 'smoothscroll'
#اگر_تعریف_شدہ FEAT_SPELL
    صحیح		wo_spell;
# define w_p_spell w_onebuf_opt.wo_spell // 'spell'
#اگر_ختم
#اگر defined(FEAT_SYN_HL) || defined(FEAT_FOLDING) || defined(FEAT_DIFF)
    صحیح		wo_cuc;
# define w_p_cuc w_onebuf_opt.wo_cuc	// 'cursorcolumn'
    صحیح		wo_cul;
# define w_p_cul w_onebuf_opt.wo_cul	// 'cursorline'
    char_u	*wo_culopt;
# define w_p_culopt w_onebuf_opt.wo_culopt	// 'cursorlineopt'
    char_u	*wo_cc;
# define w_p_cc w_onebuf_opt.wo_cc	// 'colorcolumn'
#اگر_ختم
#اگر_تعریف_شدہ FEAT_LINEBREAK
    char_u	*wo_sbr;
# define w_p_sbr w_onebuf_opt.wo_sbr	// 'showbreak'
#اگر_ختم
#اگر_تعریف_شدہ FEAT_STL_OPT
    char_u	*wo_stl;
# define w_p_stl w_onebuf_opt.wo_stl	// 'statusline'
    char_u	*wo_stlo;
# define w_p_stlo w_onebuf_opt.wo_stlo	// 'statuslineopt'
    صحیح		wo_stlo_fh;
# define w_p_stlo_fh w_onebuf_opt.wo_stlo_fh // "fixedheight" of 'statuslineopt'
    صحیح		wo_stlo_mh;
# define w_p_stlo_mh w_onebuf_opt.wo_stlo_mh // "maxheight:" of 'statuslineopt'
#اگر_ختم
    صحیح		wo_scb;
#تعریف_کرو w_p_scb w_onebuf_opt.wo_scb	// 'scrollbind'
    صحیح		wo_diff_saved; // options were saved for starting diff mode
#تعریف_کرو w_p_diff_saved w_onebuf_opt.wo_diff_saved
    صحیح		wo_scb_save;	// 'scrollbind' saved for diff mode
#تعریف_کرو w_p_scb_save w_onebuf_opt.wo_scb_save
    صحیح		wo_wrap;
#تعریف_کرو w_p_wrap w_onebuf_opt.wo_wrap	// 'wrap'
#اگر_تعریف_شدہ FEAT_DIFF
    صحیح		wo_wrap_save;	// 'wrap' state saved for diff mode
# define w_p_wrap_save w_onebuf_opt.wo_wrap_save
#اگر_ختم
#اگر_تعریف_شدہ FEAT_CONCEAL
    char_u	*wo_cocu;		// 'concealcursor'
# define w_p_cocu w_onebuf_opt.wo_cocu
    طویل	wo_cole;		// 'conceallevel'
# define w_p_cole w_onebuf_opt.wo_cole
#اگر_ختم
    صحیح		wo_crb;
#تعریف_کرو w_p_crb w_onebuf_opt.wo_crb	// 'cursorbind'
    صحیح		wo_crb_save;	// 'cursorbind' state saved for diff mode
#تعریف_کرو w_p_crb_save w_onebuf_opt.wo_crb_save
#اگر_تعریف_شدہ FEAT_SIGNS
    char_u	*wo_scl;
# define w_p_scl w_onebuf_opt.wo_scl	// 'signcolumn'
#اگر_ختم
    طویل	wo_siso;
#تعریف_کرو w_p_siso w_onebuf_opt.wo_siso	// 'sidescrolloff' local value
    طویل	wo_so;
#تعریف_کرو w_p_so w_onebuf_opt.wo_so	// 'scrolloff' local value
    طویل	wo_sop;
#تعریف_کرو w_p_sop w_onebuf_opt.wo_sop  // 'scrolloffpad' local value
#اگر_تعریف_شدہ FEAT_TERMINAL
    char_u	*wo_twk;
# define w_p_twk w_onebuf_opt.wo_twk	// 'termwinkey'
    char_u	*wo_tws;
# define w_p_tws w_onebuf_opt.wo_tws	// 'termwinsize'
#اگر_ختم

    // A few options have local flags for P_INSECURE.
    long_u	wo_wrap_flags;		// flags for 'wrap'
#تعریف_کرو w_p_wrap_flags w_onebuf_opt.wo_wrap_flags
#اگر_تعریف_شدہ FEAT_STL_OPT
    long_u	wo_stl_flags;		// flags for 'statusline'
# define w_p_stl_flags w_onebuf_opt.wo_stl_flags
#اگر_ختم
#اگر_تعریف_شدہ FEAT_EVAL
    long_u	wo_fde_flags;		// flags for 'foldexpr'
# define w_p_fde_flags w_onebuf_opt.wo_fde_flags
    long_u	wo_fdt_flags;		// flags for 'foldtext'
# define w_p_fdt_flags w_onebuf_opt.wo_fdt_flags
#اگر_ختم

#اگر_تعریف_شدہ FEAT_EVAL
    sctx_T	wo_script_ctx[WV_گنتی];	// SCTXs for window-local options
# define w_p_script_ctx w_onebuf_opt.wo_script_ctx
#اگر_ختم
    char_u	*wo_whl;
#تعریف_کرو w_p_whl w_onebuf_opt.wo_whl	// 'winhighlight'
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
ساخت wininfo_S
{
    wininfo_T	*wi_اگلا;	// next entry or NULL for last entry
    wininfo_T	*wi_پچھلا;	// previous entry or NULL for first entry
    win_T	*wi_win;	// pointer to window that did set wi_fpos
    pos_T	wi_fpos;	// last cursor position in the file
    winopt_T	wi_opt;		// local window options
    صحیح		wi_optset;	// TRUE when wi_opt has useful values
#اگر_تعریف_شدہ FEAT_FOLDING
    صحیح		wi_fold_manual;	// copy of w_fold_manual
    garray_T	wi_folds;	// clone of w_folds
#اگر_ختم
    صحیح		wi_changelistidx; // copy of w_changelistidx
};

/*
 * Info used to pass info about a fold from the fold-detection code to the
 * code that displays the foldcolumn.
 */
قسمتعریف ساخت foldinfo
{
    صحیح		fi_level;	// level of the fold; when this is zero the
				// other fields are invalid
    صحیح		fi_lnum;	// line number where fold starts
    صحیح		fi_کم_level;	// lowest fold level that starts in the same
				//سطر
} foldinfo_T;

/*
 * Structure to store info about the Visual area.
 */
قسمتعریف ساخت
{
    pos_T	vi_آغاز;	// start pos of last VIsual
    pos_T	vi_انتہا;		// end position of last VIsual
    صحیح		vi_منوال;	// VIsual_mode of last VIsual
    colnr_T	vi_curswant;	// MAXCOL from w_curswant
} visualinfo_T;

/*
 * structures used for undo
 */

// One line saved for undo.  After the NUL terminated text there might be text
// properties, thus ul_len can be larger than STRLEN(ul_line) + 1.
قسمتعریف ساخت {
    char_u	*ul_سطر;	// text of the line
    طویل	ul_len;		// length of the line including NUL, plus text
				// properties
    colnr_T	ul_textlen;	// length of the line excluding NUL and any text
				// properties
} undoline_T;

قسمتعریف ساخت u_اندراج u_اندراج_T;
قسمتعریف ساخت u_سرخی u_سرخی_T;

#اگر_تعریف_شدہ FEAT_PROP_POPUP
// Which line around an undo block a cleared continuation flag was on.
# define UNDOPROP_NONE	0
# define UNDOPROP_ABOVE	1
# define UNDOPROP_نیچے	2

// A continuation flag cleared on a line around an undo block, given back when
// the block is undone or redone the other way.
قسمتعریف ساخت
{
    صحیح		اوپر_which;	// UNDOPROP_ABOVE or UNDOPROP_BELOW
    صحیح		اوپر_type;	// tp_type of the property
    صحیح		اوپر_id;		// tp_id of the property
    colnr_T	اوپر_ستون;		// tp_col of the property
    صحیح		اوپر_flag;	// TP_FLAG_CONT_NEXT or TP_FLAG_CONT_PREV
} undoprop_T;
#اگر_ختم

ساخت u_اندراج
{
    u_اندراج_T	*ue_اگلا;	// pointer to next entry in list
    linenr_T	ue_چوٹی;		// number of line above undo block
    linenr_T	ue_bot;		// number of line below undo block
    linenr_T	ue_lcount;	// linecount when u_save called
    undoline_T	*ue_array;	// array of lines in undo block
    طویل	ue_سائز;	// number of lines in ue_array
#اگر_تعریف_شدہ FEAT_PROP_POPUP
    garray_T	ue_props;	// undoprop_T: flags cleared around the block
#اگر_ختم
#اگر_تعریف_شدہ U_DEBUG
    صحیح		ue_magic;	// magic number to check allocation
#اگر_ختم
};

ساخت u_سرخی
{
    // The following have a pointer and a number. The number is used when
    // reading the undo file in u_read_undo()
    اتحاد {
	u_سرخی_T *ptr;	// pointer to next undo header in list
	طویل	   seq;
    } uh_اگلا;
    اتحاد {
	u_سرخی_T *ptr;	// pointer to previous header in list
	طویل	   seq;
    } uh_پچھلا;
    اتحاد {
	u_سرخی_T *ptr;	// pointer to next header for alt. redo
	طویل	   seq;
    } uh_alt_اگلا;
    اتحاد {
	u_سرخی_T *ptr;	// pointer to previous header for alt. redo
	طویل	   seq;
    } uh_alt_پچھلا;
    طویل	uh_seq;		// sequence number, higher == newer undo
    صحیح		uh_walk;	// used by undo_time()
    u_اندراج_T	*uh_اندراج;	// pointer to first entry
    u_اندراج_T	*uh_getbot_اندراج; // pointer to where ue_bot must be set
    pos_T	uh_cursor;	// cursor position before saving
    طویل	uh_cursor_vcol;
    صحیح		uh_flags;	// see below
    pos_T	uh_namedm[NMARKS];	// marks before undo/after redo
    visualinfo_T uh_visual;	// Visual areas before undo/after redo
    وقت_T	uh_وقت;	// timestamp when the change was made
    طویل	uh_save_nr;	// set when the file was saved after the
				// changes in this block
#اگر_تعریف_شدہ U_DEBUG
    صحیح		uh_magic;	// magic number to check allocation
#اگر_ختم
};

// values for uh_flags
#تعریف_کرو UH_بدلا  0x01	// b_changed flag before undo/after redo
#تعریف_کرو UH_EMPTYBUF 0x02	// buffer was empty

/*
 * structures used in undo.c
 */
#تعریف_کرو ALIGN_LONG	// longword alignment and use filler byte
#تعریف_کرو ALIGN_سائز (حجم(طویل))

#تعریف_کرو ALIGN_نقاب (ALIGN_سائز - 1)

قسمتعریف ساخت m_info minfo_T;

/*
 * structure used to link chunks in one of the free chunk lists.
 */
ساخت m_info
{
#اگر_تعریف_شدہ ALIGN_LONG
    long_u	m_سائز_2;		// size of the chunk (including m_info)
#ورنہ
    short_u	m_سائز_2;		// size of the chunk (including m_info)
#اگر_ختم
    minfo_T	*m_اگلا;	// pointer to next free chunk in the list
};

/*
 * things used in memfile.c
 */

قسمتعریف ساخت بلاک_hdr    bhdr_T;
قسمتعریف ساخت memfile	    memfile_T;
قسمتعریف طویل		    blocknr_T;

/*
 * mf_hashtab_T is a chained hashtable with blocknr_T key and arbitrary
 * structures as items.  This is an intrusive data structure: we require
 * that items begin with mf_hashitem_T which contains the key and linked
 * list pointers.  List of items in each bucket is doubly-linked.
 */

قسمتعریف ساخت mf_hashitem_S mf_hashitem_T;

ساخت mf_hashitem_S
{
    mf_hashitem_T   *mhi_اگلا;
    mf_hashitem_T   *mhi_پچھلا;
    blocknr_T	    mhi_کلید;
};

#تعریف_کرو MHT_INIT_سائز   64

قسمتعریف ساخت mf_hashtab_S
{
    long_u	    mht_نقاب;	    // mask used for hash value (nr of items
				    // in array is "mht_mask" + 1)
    long_u	    mht_گنتی;	    // nr of items inserted into hashtable
    mf_hashitem_T   **mht_buckets;  // points to mht_small_buckets or
				    //dynamically allocated array
    mf_hashitem_T   *mht_small_buckets[MHT_INIT_سائز];   // initial buckets
    حرف	    mht_fixed;	    // non-zero value forbids growth
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

ساخت بلاک_hdr
{
    mf_hashitem_T bh_hashitem;      // header for hash table and key
#تعریف_کرو bh_bnum bh_hashitem.mhi_کلید // block number, part of bh_hashitem

    bhdr_T	*bh_اگلا;	    // next block_hdr in free or used list
    bhdr_T	*bh_پچھلا;	    // previous block_hdr in used list
    char_u	*bh_ڈیٹا;	    // pointer to memory (for used block)
    صحیح		bh_صفحہ_گنتی;	    // number of pages in this block

#تعریف_کرو BH_DIRTY    1
#تعریف_کرو BH_LOCKED   2
    حرف	bh_flags;	    // BH_DIRTY or BH_LOCKED
};

/*
 * when a block with a negative number is flushed to the file, it gets
 * a positive number. Because the reference to the block is still the negative
 * number, we remember the translation to the new positive number in the
 * double linked trans lists. The structure is the same as the hash lists.
 */
قسمتعریف ساخت nr_trans NR_TRANS;

ساخت nr_trans
{
    mf_hashitem_T nt_hashitem;		// header for hash table and key
#تعریف_کرو nt_old_bnum nt_hashitem.mhi_کلید	// old, negative, number

    blocknr_T	nt_new_bnum;		// new, positive, number
};


قسمتعریف ساخت buffblock buffblock_T;
قسمتعریف ساخت buffheader buffheader_T;

/*
 * structure used to store one block of the stuff/redo/recording buffers
 */
ساخت buffblock
{
    buffblock_T	*b_اگلا;	// pointer to next buffblock
    size_t	b_strlen;	// length of b_str, excluding the NUL
    char_u	b_str[1];	// contents (actually longer)
};

/*
 * header used for the stuff buffer and the redo buffer
 */
ساخت buffheader
{
    buffblock_T	bh_پہلا;	// first (dummy) block of list
    buffblock_T	*bh_curr;	// buffblock for appending
    صحیح		bh_اشاریہ;	// index for reading
    صحیح		bh_space;	// space in bh_curr for appending
    صحیح		bh_create_newblock;	// create a new block?
};

قسمتعریف ساخت
{
    buffheader_T sr_redobuff;
    buffheader_T sr_old_redobuff;
} save_redo_T;

قسمتعریف شمار {
    XP_سابقہ_NONE,	// prefix not used
    XP_سابقہ_NO,	// "no" prefix for bool option
    XP_سابقہ_INV,	// "inv" prefix for bool option
} xp_سابقہ_T;

/*
 * :set operator types
 */
قسمتعریف شمار {
    OP_NONE = 0,
    OP_ADDING,		// "opt+=arg"
    OP_PREPENDING,	// "opt^=arg"
    OP_REMOVING,	// "opt-=arg"
} سیٹ_op_T;

/*
 * used for completion on the command line
 */
قسمتعریف ساخت expand
{
    char_u	*xp_نمونہ;		// start of item to expand, guaranteed
					// to be part of xp_line
    صحیح		xp_context;		// type of expansion
    صحیح		xp_نمونہ_len;		// bytes in xp_pattern before cursor
    xp_سابقہ_T	xp_سابقہ;
#اگر defined(FEAT_EVAL)
    char_u	*xp_arg;		// completion function
    صحیح		xp_complete_opt;	// UCC_ flags for user command
    sctx_T	xp_script_ctx;		// SCTX for completion function
#اگر_ختم
    صحیح		xp_backslash;		// one of the XP_BS_ values
#اگر_غیر_تعریف_شدہ BACKSLASH_اندر_FILENAME
    صحیح		xp_shell;		// TRUE for a shell command, more
					// characters need to be escaped
#اگر_ختم
    صحیح		xp_numfiles;		// number of files found by
					// file name completion
    صحیح		xp_ستون;			// cursor position in line
    صحیح		xp_selected;		// selected index in completion
    char_u	*xp_orig;		// originally expanded string
    char_u	**xp_files;		// list of files
    char_u	**xp_files_abbr;	// optional parallel array of display
					// strings (override xp_files for the
					// pum text); NULL if unused
    char_u	**xp_files_قسم;	// optional parallel array of "kind"
					// strings; NULL if unused
    char_u	**xp_files_menu;	// optional parallel array of "menu"
					// strings (shown after the match);
					// NULL if unused
    char_u	**xp_files_info;	// optional parallel array of "info"
					// strings (shown in info popup);
					// NULL if unused
    char_u	*xp_سطر;		// text being completed
#تعریف_کرو EXPAND_BUF_LEN 256
    char_u	xp_buf[EXPAND_BUF_LEN];	// buffer for returned match
    صحیح		xp_تلاش_dir;		// Direction of search
    pos_T	xp_pre_incsearch_pos;	// Cursor position before incsearch
} expand_T;

/*
 * values for xp_backslash
 */
#تعریف_کرو XP_BS_NONE	0	// nothing special for backslashes
#تعریف_کرو XP_BS_ONE	0x1	// uses one backslash before a space
#تعریف_کرو XP_BS_THREE	0x2	// uses three backslashes before a space
#تعریف_کرو XP_BS_COMMA	0x4	// commas need to be escaped with a backslash

/*
 * Variables shared between getcmdline(), redrawcmdline() and others.
 * These need to be saved when using CTRL-R |, that's why they are in a
 * structure.
 */
قسمتعریف ساخت
{
    char_u	*cmdbuff;	// pointer to command line buffer
    صحیح		cmdbufflen;	// length of cmdbuff
    صحیح		cmdlen;		// number of chars in command line
    صحیح		cmdpos;		// current cursor position
    صحیح		cmdspos;	// cursor column on screen
    صحیح		cmdfirstc;	// ':', '/', '?', '=', '>' or NUL
    صحیح		cmdindent;	// number of spaces before cmdline
    char_u	*cmdprompt;	// message in front of cmdline
    صحیح		cmdattr;	// attributes for prompt
    صحیح		overstrike;	// Typing mode on the command line.  Shared by
				// getcmdline() and put_on_cmdline().
    expand_T	*xpc;		// struct being used for expansion, xp_pattern
				// may point into cmdbuff
    صحیح		xp_context;	// type of expansion
#اگر_تعریف_شدہ FEAT_EVAL
    char_u	*xp_arg;	// user-defined expansion arg
    صحیح		داخلہ_fn;	// when TRUE Invoked for input() function
#اگر_ختم
    صحیح		cmdbuff_replaced; // when TRUE cmdline was replaced externally
				  // (e.g. by setcmdline())
} cmdline_info_T;

/*
 * Command modifiers ":vertical", ":browse", ":confirm" and ":hide" set a flag.
 * This needs to be saved for recursive commands, put them in a structure for
 * easy manipulation.
 */
قسمتعریف ساخت
{
    صحیح		cmod_flags;		// CMOD_ flags
#تعریف_کرو CMOD_SANDBOX	    0x0001	// ":sandbox"
#تعریف_کرو CMOD_SILENT	    0x0002	// ":silent"
#تعریف_کرو CMOD_ERRSILENT	    0x0004	// ":silent!"
#تعریف_کرو CMOD_UNSILENT	    0x0008	// ":unsilent"
#تعریف_کرو CMOD_NOAUTOCMD	    0x0010	// ":noautocmd"
#تعریف_کرو CMOD_HIDE	    0x0020	// ":hide"
#تعریف_کرو CMOD_BROWSE	    0x0040	// ":browse" - invoke file dialog
#تعریف_کرو CMOD_CONFIRM	    0x0080	// ":confirm" - invoke yes/no dialog
#تعریف_کرو CMOD_KEEPALT	    0x0100	// ":keepalt"
#تعریف_کرو CMOD_KEEPMARKS	    0x0200	// ":keepmarks"
#تعریف_کرو CMOD_KEEPJUMPS	    0x0400	// ":keepjumps"
#تعریف_کرو CMOD_LOCKMARKS	    0x0800	// ":lockmarks"
#تعریف_کرو CMOD_KEEPPATTERNS   0x1000	// ":keeppatterns"
#تعریف_کرو CMOD_NOSWAPFILE	    0x2000	// ":noswapfile"
#تعریف_کرو CMOD_VIM9CMD	    0x4000	// ":vim9cmd"
#تعریف_کرو CMOD_LEGACY	    0x8000	// ":legacy"

    صحیح		cmod_بانٹو;		// flags for win_split()
    صحیح		cmod_tab;		// > 0 when ":tab" was used
    regmatch_T	cmod_filter_regmatch;	// set by :filter /pat/
    صحیح		cmod_filter_force;	// set for :filter!

    صحیح		cmod_verbose;		// 0 if not set, > 0 to set 'verbose'
					// to cmod_verbose - 1

    // values for undo_cmdmod()
    char_u	*cmod_save_ei;		// saved value of 'eventignore'
#اگر_تعریف_شدہ HAVE_SANDBOX
    صحیح		cmod_did_sandbox;	// set when "sandbox" was incremented
#اگر_ختم
    طویل	cmod_verbose_save;	// if 'verbose' was set: value of
					// p_verbose plus one
    صحیح		cmod_save_پیغام_silent;	// if non-zero: saved value of
					// msg_silent + 1
    صحیح		cmod_save_پیغام_scroll;	// for restoring msg_scroll
    صحیح		cmod_did_esilent;	// incremented when emsg_silent is
} cmdmod_T;

قسمتعریف شمار {
    MF_DIRTY_NO = 0,		// no dirty blocks
    MF_DIRTY_YES,		// there are dirty blocks
    MF_DIRTY_YES_NOSYNC,	// there are dirty blocks, do not sync yet
} mfdirty_T;

#تعریف_کرو MF_بیج_LEN	8

ساخت memfile
{
    char_u	*mf_fname;		// name of the file
    char_u	*mf_ffname;		// idem, full path
    صحیح		mf_fd;			// file descriptor
    صحیح		mf_flags;		// flags used when opening this memfile
    صحیح		mf_reopen;		// mf_fd was closed, retry opening
    bhdr_T	*mf_free_پہلا;		// first block_hdr in free list
    bhdr_T	*mf_مستعمل_پہلا;		// mru block_hdr in used list
    bhdr_T	*mf_مستعمل_آخری;		// lru block_hdr in used list
    غیرعلامتی	mf_مستعمل_گنتی;		// number of pages in used list
    غیرعلامتی	mf_مستعمل_گنتی_max;	// maximum number of pages in memory
    mf_hashtab_T mf_hash;		// hash lists
    mf_hashtab_T mf_trans;		// trans lists
    blocknr_T	mf_blocknr_max;		// highest positive block number + 1
    blocknr_T	mf_blocknr_min;		// lowest negative block number - 1
    blocknr_T	mf_neg_گنتی;		// number of negative blocks numbers
    blocknr_T	mf_infile_گنتی;	// number of pages in the file
    غیرعلامتی	mf_صفحہ_سائز;		// number of bytes in a page
    mfdirty_T	mf_dirty;
#اگر_تعریف_شدہ FEAT_CRYPT
    buf_T	*mf_بفر;		// buffer this memfile is for
    char_u	mf_بیج[MF_بیج_LEN];	// seed for encryption

    // Values for key, method and seed used for reading data blocks when
    // updating for a newly set key or method. Only when mf_old_key != NULL.
    char_u	*mf_old_کلید;
    صحیح		mf_old_cm;
    char_u	mf_old_بیج[MF_بیج_LEN];
#اگر_ختم
};

/*
 * things used in memline.c
 */
/*
 * When searching for a specific line, we remember what blocks in the tree
 * are the branches leading to that block. This is stored in ml_stack.  Each
 * entry is a pointer to info in a block (may be data block or pointer block)
 */
قسمتعریف ساخت info_اشارہ_گر
{
    blocknr_T	ip_bnum;	// block number
    linenr_T	ip_کم;		// lowest lnum in this block
    linenr_T	ip_بلند;	// highest lnum in this block
    صحیح		ip_اشاریہ;	// index for block with current lnum
} infoptr_T;	// block/index pair

#اگر_تعریف_شدہ FEAT_BYTEOFF
قسمتعریف ساخت ml_chunksize
{
    صحیح		mlcs_numlines;
    طویل	mlcs_totalsize;
} chunksize_T;

/*
 * Flags when calling ml_updatechunk()
 */
# define ML_CHNK_ADDLINE 1
# define ML_CHNK_DELLINE 2
# define ML_CHNK_UPDLINE 3
#اگر_ختم

/*
 * the memline structure holds all the information about a memline
 */
قسمتعریف ساخت memline
{
    linenr_T	ml_سطر_گنتی;	// number of lines in the buffer

    memfile_T	*ml_mfp;	// pointer to associated memfile

    infoptr_T	*ml_ڈھیر;	// stack of pointer blocks (array of IPTRs)
    صحیح		ml_ڈھیر_چوٹی;	// current top of ml_stack
    صحیح		ml_ڈھیر_سائز;	// total number of entries in ml_stack

#تعریف_کرو ML_خالی_خانے	0x01	// empty buffer
#تعریف_کرو ML_سطر_DIRTY	0x02	// cached line was changed and allocated
#تعریف_کرو ML_LOCKED_DIRTY	0x04	// ml_locked was changed
#تعریف_کرو ML_LOCKED_POS	0x08	// ml_locked needs positive block number
#تعریف_کرو ML_ALLOCATED	0x10	// ml_line_ptr is an allocated copy
    صحیح		ml_flags;

    colnr_T	ml_سطر_len;	// length of the cached line + NUL + text properties
    colnr_T	ml_سطر_textlen;// length of the cached line + NUL, 0 if not known yet
    linenr_T	ml_سطر_lnum;	// line number of cached line, 0 if not valid
    char_u	*ml_سطر_ptr;	// pointer to cached line

    bhdr_T	*ml_locked;	// block used by last ml_get
    linenr_T	ml_locked_کم;	// first line in ml_locked
    linenr_T	ml_locked_بلند;	// last line in ml_locked
    صحیح		ml_locked_lineadd;  // number of lines inserted in ml_locked
#اگر_تعریف_شدہ FEAT_BYTEOFF
    chunksize_T *ml_chunksize;
    صحیح		ml_numchunks;
    صحیح		ml_usedchunks;
#اگر_ختم
} memline_T;

// Values for the flags argument of ml_delete_flags().
#تعریف_کرو ML_DEL_پیغام	    1	// may give a "No lines in buffer" message
#تعریف_کرو ML_DEL_UNDO	    2	// called from undo, do not update textprops
#تعریف_کرو ML_DEL_NOPROP	    4	// splitting data block, do not update textprops

// Values for the flags argument of ml_append_int().
#تعریف_کرو ML_APPEND_NEW	    1	// starting to edit a new file
#تعریف_کرو ML_APPEND_MARK	    2	// mark the new line
#تعریف_کرو ML_APPEND_UNDO	    4	// called from undo
#تعریف_کرو ML_APPEND_NOPROP    8	// do not continue textprop from previous line


/*
 * Structure defining text properties.  These stick with the text.
 * When stored in memline they are after the text, ml_line_len is larger than
 * STRLEN(ml_line_ptr) + 1.
 */
قسمتعریف ساخت textprop_S
{
    colnr_T	tp_ستون;		// start column (one based, in bytes)
    colnr_T	tp_len;		// length in bytes; for virtual text props
				// this is STRLEN(vtext) (not including NUL)
    صحیح		tp_id;		// identifier
    صحیح		tp_type;	// property type
    صحیح		tp_flags;	// TP_FLAG_ values
    صحیح		tp_padleft;	// left padding between text line and virtual
				//متن
    اتحاد			// For virtual text props (tp_id < 0):
    {				// check TP_FLAG_VTEXT_PTR in tp_flags to
				// determine which member is active.
	colnr_T	tp_متن_انحراف; // offset to vtext string from the
				// prop_count position in the memline
				// (when TP_FLAG_VTEXT_PTR is NOT set)
	char_u	*tp_متن;	// pointer to virtual text string
				// (when TP_FLAG_VTEXT_PTR IS set)
    } u;
} textprop_T;

#تعریف_کرو TP_FLAG_CONT_اگلا	0x1	// property continues in next line
#تعریف_کرو TP_FLAG_CONT_پچھلا	0x2	// property was continued from prev line

// without these text is placed after the end of the line
#تعریف_کرو TP_FLAG_ALIGN_دایاں	0x010	// virtual text is right-aligned
#تعریف_کرو TP_FLAG_ALIGN_ABOVE	0x020	// virtual text above the line
#تعریف_کرو TP_FLAG_ALIGN_نیچے	0x040	// virtual text on next screen line

#تعریف_کرو TP_FLAG_WRAP		0x080	// virtual text wraps - when missing
					// text is truncated
#تعریف_کرو TP_FLAG_آغاز_INCL	0x100	// "start_incl" copied from proptype
#تعریف_کرو TP_FLAG_DELETED		0x200	// marked for deletion in
					// unpacked_memline_T
#تعریف_کرو TP_FLAG_VTEXT_PTR	0x400	// u.tp_text access is valid

#تعریف_کرو PROP_گنتی_سائز	حجم(uint16_t)    // size of prop_count in memline

#تعریف_کرو PROP_متن_MIN_CELLS	4	// minimum number of cells to use for
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
قسمتعریف ساخت unpacked_memline_S
{
    buf_T	*buf;		// the line's buffer (NULL = closed)
    linenr_T	lnum;		// line number (0 = no line loaded)
    bool	detached;	// true when text/vtext are allocated
    colnr_T	متن_سائز_2;	// size of text including NUL
    char_u	*متن;		// NUL-terminated text
    صحیح		prop_سائز;	// number of allocated prop slots
    صحیح		prop_گنتی;	// number of properties
    textprop_T	*props;		// property array
    bool	متن_بدلا;	// true if text was modified
} unpacked_memline_T;

/*
 * Structure defining a property type.
 */
قسمتعریف ساخت proptype_S
{
    صحیح		pt_id;		// value used for tp_id
    صحیح		pt_type;	// number used for tp_type
    صحیح		pt_hl_id;	// highlighting
    صحیح		pt_ترجیح;	//ترجیح
    صحیح		pt_flags;	// PT_FLAG_ values
    char_u	pt_name[1];	// property type name, actually longer
} proptype_T;

#تعریف_کرو PT_FLAG_INS_آغاز_INCL	1	// insert at start included in property
#تعریف_کرو PT_FLAG_INS_انتہا_INCL	2	// insert at end included in property
#تعریف_کرو PT_FLAG_COMBINE		4	// combine with syntax highlight
#تعریف_کرو PT_FLAG_OVERRIDE	8	// override any highlight

// Sign group
قسمتعریف ساخت signgroup_S
{
    صحیح		sg_اگلا_sign_id;	// next sign id for this group
    short_u	sg_refcount;		// number of signs in this group
    char_u	sg_name[1];		// sign group name, actually longer
} signgroup_T;

قسمتعریف ساخت sign_اندراج sign_اندراج_T;
ساخت sign_اندراج
{
    صحیح		 se_id;		// unique identifier for each placed sign
    صحیح		 se_typenr;	// typenr of sign
    صحیح		 se_ترجیح;	// priority for highlighting
    linenr_T	 se_lnum;	// line number which has this sign
    signgroup_T	 *se_گروہ;	// sign group
    sign_اندراج_T *se_اگلا;	// next entry in a list of signs
    sign_اندراج_T *se_پچھلا;	// previous entry -- for easy reordering
};

/*
 * Sign attributes. Used by the screen refresh routines.
 */
قسمتعریف ساخت sign_attrs_S {
    صحیح		sat_typenr;
    خالی	*sat_icon;
    char_u	*sat_متن;
    صحیح		sat_texthl;
    صحیح		sat_linehl;
    صحیح		sat_culhl;
    صحیح		sat_numhl;
    صحیح		sat_ترجیح;
} sign_attrs_T;

#اگر defined(FEAT_SIGNS)
// Macros to get the sign group structure from the group name
# define SGN_کلید_OFF	offsetof(signgroup_T, sg_name)
# define HI2SG(hi)	((signgroup_T *)((hi)->hi_کلید - SGN_کلید_OFF))

// Default sign priority for highlighting
# define SIGN_DEF_PRIO	10

#اگر_ختم

/*
 * Argument list: Array of file names.
 * Used for the global argument list and the argument lists local to a window.
 */
قسمتعریف ساخت arglist
{
    garray_T	al_ga;		// growarray with the array of file names
    صحیح		al_refcount;	// number of windows using this arglist
    صحیح		id;		// id of this arglist
} alist_T;

/*
 * For each argument remember the file name as it was given, and the buffer
 * number that contains the expanded file name (required for when ":cd" is
 * used).
 */
قسمتعریف ساخت argentry
{
    char_u	*ae_fname;	// file name as specified
    صحیح		ae_fnum;	// buffer number with expanded file name
} aentry_T;

#تعریف_کرو ALIST(win)	(win)->w_alist
#تعریف_کرو GARGLIST	((aentry_T *)global_alist.al_ga.ga_ڈیٹا)
#تعریف_کرو ARGLIST		((aentry_T *)ALIST(curwin)->al_ga.ga_ڈیٹا)
#تعریف_کرو WARGLIST(wp)	((aentry_T *)ALIST(wp)->al_ga.ga_ڈیٹا)
#تعریف_کرو AARGLIST(al)	((aentry_T *)((al)->al_ga.ga_ڈیٹا))
#تعریف_کرو GARGCOUNT	(global_alist.al_ga.ga_len)
#تعریف_کرو ARGCOUNT	(ALIST(curwin)->al_ga.ga_len)
#تعریف_کرو WARGCOUNT(wp)	(ALIST(wp)->al_ga.ga_len)

/*
 * A list used for saving values of "emsg_silent".  Used by ex_try() to save the
 * value of "emsg_silent" if it was non-zero.  When this is done, the CSF_SILENT
 * flag below is set.
 */

قسمتعریف ساخت eslist_elem eslist_T;
ساخت eslist_elem
{
    صحیح		saved_emsg_silent;	// saved value of "emsg_silent"
    eslist_T	*اگلا;			// next element on the list
};

/*
 * For conditional commands a stack is kept of nested conditionals.
 * When cs_idx < 0, there is no conditional command.
 */
#تعریف_کرو CSTACK_LEN	50

قسمتعریف ساخت {
    مختصر	cs_flags[CSTACK_LEN];	// CSF_ flags
    حرف	cs_pending[CSTACK_LEN];	// CSTP_: what's pending in ":finally"
    اتحاد {
	خالی	*csp_rv[CSTACK_LEN];	// return typeval for pending return
	خالی	*csp_ex[CSTACK_LEN];	// exception for pending throw
    }		cs_pend;
    خالی	*cs_forinfo[CSTACK_LEN]; // info used by ":for"
    صحیح		cs_سطر[CSTACK_LEN];	// line nr of ":while"/":for" line
    صحیح		cs_بلاک_id[CSTACK_LEN];    // block ID stack
    صحیح		cs_script_var_len[CSTACK_LEN];	// value of sn_var_vals.ga_len
						// when entering the block
    صحیح		cs_idx;			// current entry, or -1 if none
    صحیح		cs_looplevel;		// nr of nested ":while"s and ":for"s
    صحیح		cs_trylevel;		// nr of nested ":try"s
    eslist_T	*cs_emsg_silent_list;	// saved values of "emsg_silent"
    حرف	cs_lflags;		// loop flags: CSL_ flags
} cstack_T;
#تعریف_کرو cs_rettv	cs_pend.csp_rv
#تعریف_کرو cs_exception	cs_pend.csp_ex

// There is no CSF_IF, the lack of CSF_WHILE, CSF_FOR and CSF_TRY means ":if"
// was used.
#تعریف_کرو CSF_TRUE	0x0001	// condition was TRUE
#تعریف_کرو CSF_فعال	0x0002	// current state is active
#تعریف_کرو CSF_ELSE	0x0004	// ":else" has been passed
#تعریف_کرو CSF_WHILE	0x0008	// is a ":while"
#تعریف_کرو CSF_FOR	0x0010	// is a ":for"
#تعریف_کرو CSF_بلاک	0x0020	// is a "{" block

#تعریف_کرو CSF_TRY	0x0100	// is a ":try"
#تعریف_کرو CSF_FINALLY	0x0200	// ":finally" has been passed
#تعریف_کرو CSF_CATCH	0x0400	// ":catch" has been seen
#تعریف_کرو CSF_THROWN	0x0800	// exception thrown to this try conditional
#تعریف_کرو CSF_CAUGHT	0x1000  // exception caught by this try conditional
#تعریف_کرو CSF_ختم_شدہ	0x2000  // CSF_CAUGHT was handled by finish_exception()
#تعریف_کرو CSF_SILENT	0x4000	// "emsg_silent" reset by ":try"
// Note that CSF_ELSE is only used when CSF_TRY and CSF_WHILE are unset
// (an ":if"), and CSF_SILENT is only used when CSF_TRY is set.

#تعریف_کرو CSF_FUNC_DEF	0x8000	// a function was defined in this block

/*
 * What's pending for being reactivated at the ":endtry" of this try
 * conditional:
 */
#تعریف_کرو CSTP_NONE	0	// nothing pending in ":finally" clause
#تعریف_کرو CSTP_ERROR	1	// an error is pending
#تعریف_کرو CSTP_مداخلت	2	// an interrupt is pending
#تعریف_کرو CSTP_THROW	4	// a throw is pending
#تعریف_کرو CSTP_BREAK	8	// ":break" is pending
#تعریف_کرو CSTP_CONTINUE	16	// ":continue" is pending
#تعریف_کرو CSTP_RETURN	24	// ":return" is pending
#تعریف_کرو CSTP_اختتام	32	// ":finish" is pending

/*
 * Flags for the cs_lflags item in cstack_T.
 */
#تعریف_کرو CSL_HAD_LOOP	 1	// just found ":while" or ":for"
#تعریف_کرو CSL_HAD_ENDLOOP 2	// just found ":endwhile" or ":endfor"
#تعریف_کرو CSL_HAD_CONT	 4	// just found ":continue"
#تعریف_کرو CSL_HAD_FINA	 8	// just found ":finally"

/*
 * A list of error messages that can be converted to an exception.  "throw_msg"
 * is only set in the first element of the list.  Usually, it points to the
 * original message stored in that element, but sometimes it points to a later
 * message in the list.  See cause_errthrow().
 */
قسمتعریف ساخت msglist msglist_T;
ساخت msglist
{
    msglist_T	*اگلا;		// next of several messages in a row
    حرف	*پیغام;		// original message, allocated
    حرف	*throw_پیغام;	// msg to throw: usually original one
    char_u	*sfile;		// value from estack_sfile(), allocated
    طویل	slnum;		// line number for "sfile"
    صحیح		پیغام_compiling;	// saved value of estack_compiling
};

/*
 * The exception types.
 */
قسمتعریف شمار
{
    ET_USER,		// exception caused by ":throw" command
    ET_ERROR,		// error exception
    ET_مداخلت,	// interrupt exception triggered by Ctrl-C
} except_type_T;

/*
 * Structure describing an exception.
 * (don't use "struct exception", it's used by the math library).
 */
قسمتعریف ساخت vim_exception except_T;
ساخت vim_exception
{
    except_type_T	type;		// exception type
    حرف		*قدر;		// exception value
    ساخت msglist	*messages;	// message(s) causing error exception
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
قسمتعریف ساخت cleanup_stuff cleanup_T;
ساخت cleanup_stuff
{
    صحیح pending;		// error/interrupt/exception state
    except_T *exception;	// exception value
};

/*
 * Exception state that is saved and restored when calling timer callback
 * functions and deferred functions.
 */
قسمتعریف ساخت exception_state_S exception_state_T;
ساخت exception_state_S
{
    except_T	*estate_current_exception;
    صحیح		estate_did_throw;
    صحیح		estate_need_rethrow;
    صحیح		estate_trylevel;
    صحیح		estate_did_emsg;
};

#اگر_تعریف_شدہ FEAT_SYN_HL
// struct passed to in_id_list()
ساخت sp_syn
{
    صحیح		inc_لیبل;	// ":syn include" unique tag
    مختصر	id;		// highlight group ID of item
    مختصر	*cont_اندر_list;	// cont.in group IDs, if non-zero
};

/*
 * Each keyword has one keyentry, which is linked in a hash list.
 */
قسمتعریف ساخت keyentry keyentry_T;

ساخت keyentry
{
    keyentry_T	*ke_اگلا;	// next entry with identical "keyword[]"
    ساخت sp_syn k_syn;	// struct passed to in_id_list()
    مختصر	*اگلا_list;	// ID list for next match (if non-zero)
    صحیح		flags;
    صحیح		k_char;		// conceal substitute character
    char_u	keyword[1];	// actually longer
};

/*
 * Struct used to store one state of the state stack.
 */
قسمتعریف ساخت buf_state
{
    صحیح		    bs_idx;	 // index of pattern
    صحیح		    bs_flags;	 // flags for pattern
# ifdef FEAT_CONCEAL
    صحیح		    bs_seqnr;	 // stores si_seqnr
    صحیح		    bs_cchar;	 // stores si_cchar
# endif
    reg_extmatch_T *bs_extmatch; // external matches from start pattern
} bufstate_T;

/*
 * syn_state contains the syntax state stack for the start of one line.
 * Used by b_sst_array[].
 */
قسمتعریف ساخت syn_state synstate_T;

ساخت syn_state
{
    synstate_T	*sst_اگلا;	// next entry in used or free list
    linenr_T	sst_lnum;	// line number for this state
    اتحاد
    {
	bufstate_T	sst_ڈھیر[SST_FIX_حالتیں]; // short state stack
	garray_T	sst_ga;	// growarray for long state stack
    } sst_union;
    صحیح		sst_اگلا_flags;	// flags for sst_next_list
    صحیح		sst_stacksize;	// number of states on the stack
    مختصر	*sst_اگلا_list;	// "nextgroup" list in this state
				// (this is a copy, don't free it!
    disptick_T	sst_tick;	// tick when last displayed
    linenr_T	sst_change_lnum;// when non-zero, change in this line
				// may have made the state invalid
};
#اگر_ختم // FEAT_SYN_HL

#تعریف_کرو MAX_HL_ID       20000	// maximum value for a highlight ID.

/*
 * Structure shared between syntax.c, screen.c and gui_x11.c.
 */
قسمتعریف ساخت attr_اندراج
{
    مختصر	    ae_attr;		// HL_BOLD, etc.
    اتحاد
    {
	ساخت
	{
	    char_u	    *آغاز;	// start escape sequence
	    char_u	    *stop;	// stop escape sequence
	} رکن;
	ساخت
	{
	    // These colors need to be > 8 bits to hold 256.
	    short_u	    fg_color;	// foreground color number
	    short_u	    bg_color;	// background color number
	    short_u	    ul_color;	// underline color number
	    short_u	    font;	// font number
#اگر_تعریف_شدہ FEAT_TERMGUICOLORS
	    guicolor_T	    fg_rgb;	// foreground color RGB
	    guicolor_T	    bg_rgb;	// background color RGB
	    guicolor_T	    ul_rgb;	// underline color RGB
#اگر_ختم
	} cterm;
#اگر_تعریف_شدہ FEAT_GUI
	ساخت
	{
	    guicolor_T	    fg_color;	// foreground color handle
	    guicolor_T	    bg_color;	// background color handle
	    guicolor_T	    sp_color;	// special color handle
	    GuiFont	    font;	// font handle
# ifdef FEAT_XFONTSET
	    GuiFontset	    fontset;	// fontset handle
# endif
	} gui;
#اگر_ختم
    } ae_u;
} attrentry_T;

#اگر_تعریف_شدہ USE_ICONV
# ifdef HAVE_ICONV_H
#  include <iconv.h>
# ورنہ
#  اگر defined(MACOS_X)
#   include <sys/errno.h>
#   ifndef EILSEQ
#    define EILSEQ ENOENT // Early MacOS X does not have EILSEQ
#   endif
قسمتعریف ساخت _iconv_t *iconv_t;
#  ورنہ
#   include <errno.h>
#  endif
قسمتعریف خالی *iconv_t;
# endif
#اگر_ختم

/*
 * Used for the typeahead buffer: typebuf.
 */
قسمتعریف ساخت
{
    char_u	*tb_buf;	// buffer for typed characters
    char_u	*tb_noremap;	// mapping flags for characters in tb_buf[]
    صحیح		tb_buflen;	// size of tb_buf[]
    صحیح		tb_off;		// current position in tb_buf[]
    صحیح		tb_len;		// number of valid bytes in tb_buf[]
    صحیح		tb_maplen;	// nr of mapped bytes in tb_buf[]
    صحیح		tb_silent;	// nr of silently mapped bytes in tb_buf[]
    صحیح		tb_no_abbr_cnt; // nr of bytes without abbrev. in tb_buf[]
    صحیح		tb_change_cnt;	// nr of time tb_buf was changed; never zero
} typebuf_T;

// Struct to hold the saved typeahead for save_typeahead().
قسمتعریف ساخت
{
    typebuf_T		save_typebuf;
    صحیح			typebuf_valid;	    // TRUE when save_typebuf valid
    صحیح			old_char;
    صحیح			old_mod_نقاب;
    buffheader_T	save_readbuf1;
    buffheader_T	save_readbuf2;
#اگر_تعریف_شدہ USE_داخلہ_BUF
    char_u		*save_inputbuf;
#اگر_ختم
} tasave_T;

/*
 * Used for conversion of terminal I/O and script files.
 */
قسمتعریف ساخت
{
    صحیح		vc_type;	// zero or one of the CONV_ values
    صحیح		vc_ضربی_عدد;	// max. expansion factor
#اگر_تعریف_شدہ MSWIN
    صحیح		vc_cpfrom;	// codepage to convert from (CONV_CODEPAGE)
    صحیح		vc_cpto;	// codepage to convert to (CONV_CODEPAGE)
#اگر_ختم
#اگر_تعریف_شدہ USE_ICONV
    iconv_t	vc_fd;		// for CONV_ICONV
#اگر_ختم
    صحیح		vc_fail;	// fail for invalid char, don't use '?'
} vimconv_T;

/*
 * Structure used for the command line history.
 */
قسمتعریف ساخت hist_اندراج
{
    صحیح		hisnum;		// identifying number
    صحیح		viminfo;	// when TRUE hisstr comes from viminfo
    char_u	*hisstr;	// actual entry, separator char after the NUL
    size_t	hisstrlen;	// length of hisstr (excluding the NUL)
    time_t	وقت_سیٹ;	// when it was typed, zero if unknown
} histentry_T;

#تعریف_کرو CONV_NONE		0
#تعریف_کرو CONV_مقصد_UTF8		1
#تعریف_کرو CONV_9_مقصد_UTF8		2
#تعریف_کرو CONV_مقصد_LATIN1		3
#تعریف_کرو CONV_مقصد_LATIN9		4
#تعریف_کرو CONV_ICONV		5
#اگر_تعریف_شدہ MSWIN
# define CONV_CODEPAGE		10	// codepage -> codepage
#اگر_ختم
#اگر_تعریف_شدہ MACOS_X
# define CONV_MAC_LATIN1	20
# define CONV_LATIN1_MAC	21
# define CONV_MAC_UTF8		22
# define CONV_UTF8_MAC		23
#اگر_ختم

/*
 * Structure used for mappings and abbreviations.
 */
قسمتعریف ساخت mapblock mapblock_T;
ساخت mapblock
{
    mapblock_T	*m_اگلا;	// next mapblock in list
    mapblock_T	*m_alt;		// pointer to mapblock of the same mapping
				// with an alternative form of m_keys, or NULL
				// if there is no such mapblock
    char_u	*m_کلیدیں;	// mapped from, lhs
    char_u	*m_str;		// mapped to, rhs
    char_u	*m_orig_str;	// rhs as entered by the user
    صحیح		m_keylen;	// strlen(m_keys)
    صحیح		m_منوال;		// valid mode
    صحیح		m_simplified;	// m_keys was simplified, do not use this map
				// if key_protocol_enabled() returns TRUE
    صحیح		m_noremap;	// if non-zero no re-mapping for m_str
    حرف	m_silent;	// <silent> used, don't echo commands
    حرف	m_nowait;	// <nowait> used
#اگر_تعریف_شدہ FEAT_EVAL
    حرف	m_اظہاریہ;		// <expr> used, m_str is an expression
    sctx_T	m_script_ctx;	// SCTX where map was defined
#اگر_ختم
};


/*
 * Used for highlighting in the status line.
 */
قسمتعریف ساخت
{
    char_u	*آغاز;
    صحیح		userhl;		// 0: no HL, 1-9: User HL, < 0 for syn ID
} stl_hlrec_T;

/*
 * Used for statusline click function regions.
 */
قسمتعریف ساخت {
    char_u	*آغاز;		// position in output buffer where region starts
    char_u	*funcname;	// function name (NULL = end/close marker)
    صحیح		minwid;		// minwid value from %N@Func@
} stl_clickrec_T;

/*
 * Per-window resolved click regions (screen column based).
 */
قسمتعریف ساخت {
    صحیح		صف;		// screen row where region lives
    صحیح		ستون_آغاز;	// screen column where region starts
    صحیح		ستون_انتہا;	// screen column where region ends
    char_u	*funcname;	// function name (allocated copy)
    صحیح		minwid;		// minwid value
    صحیح		tabnr;		// tab page number (tabpanel only, 0 otherwise)
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
قسمتعریف ساخت hashitem_S
{
    long_u	hi_hash;	// cached hash number of hi_key
    char_u	*hi_کلید;
} hashitem_T;

// The address of "hash_removed" is used as a magic number for hi_key to
// indicate a removed item.
#تعریف_کرو HI_کلید_REMOVED &hash_removed
#تعریف_کرو HASHITEM_خالی_خانے(hi) ((hi)->hi_کلید == تہی || (hi)->hi_کلید == &hash_removed)

// Initial size for a hashtable.  Our items are relatively small and growing
// is expensive, thus use 16 as a start.  Must be a power of 2.
// This allows for storing 10 items (2/3 of 16) before a resize is needed.
#تعریف_کرو HT_INIT_سائز 16

// flags used for ht_flags
#تعریف_کرو HTFLAGS_ERROR	0x01	// Set when growing failed, can't add more
				// items before growing works.
#تعریف_کرو HTFLAGS_FROZEN	0x02	// Trying to add or remove an item will result
				// in an error message.

قسمتعریف ساخت hashtable_S
{
    long_u	ht_نقاب;	// mask used for hash value (nr of items in
				// array is "ht_mask" + 1)
    long_u	ht_مستعمل;	// number of items used
    long_u	ht_filled;	// number of items used + removed
    صحیح		ht_بدلا;	// incremented when adding or removing an item
    صحیح		ht_locked;	// counter for hash_lock()
    صحیح		ht_flags;	// HTFLAGS_ values
    hashitem_T	*ht_array;	// points to the array, allocated when it's
				// not "ht_smallarray"
    hashitem_T	ht_smallarray[HT_INIT_سائز];   // initial array
} hashtab_T;

قسمتعریف long_u hash_T;		// Type for hi_hash


// Use 64-bit Number.
#اگر_تعریف_شدہ MSWIN
   قسمتعریف __int64		varnumber_T;
   قسمتعریف غیرعلامتی __int64	uvarnumber_T;
# define VARNUM_MIN		_I64_MIN
# define VARNUM_MAX		_I64_MAX
# define UVARNUM_MAX		_UI64_MAX
#وگرنہ_اگر defined(HAVE_NO_LONG_LONG)
# اگر defined(HAVE_STDINT_H)
   قسمتعریف int64_t		varnumber_T;
   قسمتعریف uint64_t		uvarnumber_T;
#  define VARNUM_MIN		INT64_MIN
#  define VARNUM_MAX		INT64_MAX
#  define UVARNUM_MAX		UINT64_MAX
# ورنہ
   // this may cause trouble for code that depends on 64 bit ints
   قسمتعریف طویل			varnumber_T;
   قسمتعریف غیرعلامتی طویل	uvarnumber_T;
#  define VARNUM_MIN		LONG_MIN
#  define VARNUM_MAX		LONG_MAX
#  define UVARNUM_MAX		ULONG_MAX
# endif
#ورنہ
  قسمتعریف طویل طویل		varnumber_T;
  قسمتعریف غیرعلامتی طویل طویل	uvarnumber_T;
# ifdef LLONG_MIN
#  define VARNUM_MIN		LLONG_MIN
#  define VARNUM_MAX		LLONG_MAX
#  define UVARNUM_MAX		ULLONG_MAX
# ورنہ
#  define VARNUM_MIN		LONG_LONG_MIN
#  define VARNUM_MAX		LONG_LONG_MAX
#  define UVARNUM_MAX		ULONG_LONG_MAX
# endif
#اگر_ختم

// Struct that holds both a normal function name and a partial_T, as used for a
// callback argument.
// When used temporarily "cb_name" is not allocated.  The refcounts to either
// the function or the partial are incremented and need to be decremented
// later with free_callback().
قسمتعریف ساخت {
    char_u	*cb_name;
    partial_T	*cb_partial;
    صحیح		cb_free_name;	    // cb_name was allocated
} callback_T;

قسمتعریف ساخت isn_S isn_T;	    // instruction
قسمتعریف ساخت dfunc_S dfunc_T;	    // :def function

قسمتعریف ساخت type_S type_T;
قسمتعریف ساخت ufunc_S ufunc_T;

قسمتعریف ساخت jobvar_S کام_T;
قسمتعریف ساخت readq_S readq_T;
قسمتعریف ساخت writeq_S writeq_T;
قسمتعریف ساخت jsonq_S jsonq_T;
قسمتعریف ساخت cbq_S cbq_T;
قسمتعریف ساخت channel_S channel_T;
قسمتعریف ساخت cctx_S cctx_T;
قسمتعریف ساخت ectx_S ectx_T;
قسمتعریف ساخت instr_S instr_T;
قسمتعریف ساخت class_S class_T;
قسمتعریف ساخت object_S object_T;
قسمتعریف ساخت typealias_S typealias_T;

قسمتعریف شمار
{
    VAR_UNKNOWN = 0,	// not set, any type or "void" allowed
    VAR_ANY,		// used for "any" type
    VAR_VOID,		// no value (function not returning anything)
    VAR_BOOL,		// "v_number" is used: VVAL_TRUE or VVAL_FALSE
    VAR_SPECIAL,	// "v_number" is used: VVAL_NULL or VVAL_NONE
    VAR_نمبر,		// "v_number" is used
    VAR_FLOAT,		// "v_float" is used
    VAR_STRING,		// "v_string" is used
    VAR_BLOB,		// "v_blob" is used
    VAR_FUNC,		// "v_string" is function name
    VAR_PARTIAL,	// "v_partial" is used
    VAR_LIST,		// "v_list" is used
    VAR_DICT,		// "v_dict" is used
    VAR_کام,		// "v_job" is used
    VAR_CHANNEL,	// "v_channel" is used
    VAR_INSTR,		// "v_instr" is used
    VAR_CLASS,		// "v_class" is used (also used for interface)
    VAR_OBJECT,		// "v_object" is used
    VAR_TYPEALIAS,	// "v_typealias" is used
    VAR_TUPLE		// "v_tuple" is used
} vartype_T;

// A type specification.
ساخت type_S {
    vartype_T	    tt_type;
    صحیح	    tt_argcount;    // for func, incl. vararg, -1 for unknown
    صحیح	    tt_min_argcount; // number of non-optional arguments
    short_u	    tt_flags;	    // TTFLAG_ values
    type_T	    *tt_member;	    // for list, dict, func return type
    class_T	    *tt_class;	    // for class and object
    type_T	    **tt_دلائل;	    // func argument types, allocated
};

قسمتعریف ساخت {
    type_T	*type_curr;	    // current type, value type
    type_T	*type_decl;	    // declared type or equal to type_current
} type2_T;

#تعریف_کرو TTFLAG_VARARGS	    0x01    // func args ends with "..."
#تعریف_کرو TTFLAG_BOOL_OK	    0x02    // can be converted to bool
#تعریف_کرو TTFLAG_FLOAT_OK	    0x04    // number can be used/converted to float
#تعریف_کرو TTFLAG_نمبر_OK    0x08    // number can be used for a float
#تعریف_کرو TTFLAG_STATIC	    0x10    // one of the static types, e.g. t_any
#تعریف_کرو TTFLAG_CONST	    0x20    // cannot be changed
#تعریف_کرو TTFLAG_SUPER	    0x40    // object from "super".
#تعریف_کرو TTFLAG_GENERIC	    0x80    // generic type
#تعریف_کرو TTFLAG_TUPLE_OK	    0x100   // tuple can be used for a list

#تعریف_کرو IS_GENERIC_TYPE(type)	\
    ((type->tt_flags & TTFLAG_GENERIC) == TTFLAG_GENERIC)

// Type check flags
#تعریف_کرو TYPECHK_نمبر_OK	0x1	// number is accepted for a float
#تعریف_کرو TYPECHK_TUPLE_OK	0x2	// tuple is accepted for a list

قسمتعریف شمار {
    VIM_ACCESS_نجی,	// read/write only inside the class
    VIM_ACCESS_READ,	// read everywhere, write only inside the class
    VIM_ACCESS_ALL	// read/write everywhere
} omacc_T;

#تعریف_کرو OCMFLAG_HAS_TYPE	0x01	// type specified explicitly
#تعریف_کرو OCMFLAG_FINAL		0x02	// "final" object/class member
#تعریف_کرو OCMFLAG_CONST		0x04	// "const" object/class member

/*
 * Object methods called by builtin functions (e.g. string(), empty(), etc.)
 */
قسمتعریف شمار {
    CLASS_BUILTIN_INVALID,
    CLASS_BUILTIN_STRING,
    CLASS_BUILTIN_خالی_خانے,
    CLASS_BUILTIN_LEN,
    CLASS_BUILTIN_MAX
} class_builtin_T;

/*
 * Entry for an object or class member variable.
 */
قسمتعریف ساخت {
    string_T	ocm_name;	// allocated
    omacc_T	ocm_access;
    type_T	*ocm_type;
    صحیح		ocm_flags;
    char_u	*ocm_init;	// allocated
    sctx_T	ocm_init_sctx;	// script context of the initializer expression
} ocmember_T;

// used for the lookup table of a class member index and object method index
قسمتعریف ساخت itf2class_S itf2class_T;
ساخت itf2class_S {
    itf2class_T	*i2c_اگلا;
    class_T	*i2c_class;
    صحیح		i2c_is_طریقہ;	    // TRUE for method indexes
    // array with ints follows
};

#تعریف_کرو CLASS_INTERFACE	    0x1
#تعریف_کرو CLASS_EXTENDED	    0x2	    // another class extends this one
#تعریف_کرو CLASS_ABSTRACT	    0x4	    // abstract class
#تعریف_کرو CLASS_ENUM	    0x8	    // enum

// "class_T": used for v_class of typval of VAR_CLASS
// Also used for an interface (class_flags has CLASS_INTERFACE).
ساخت class_S
{
    string_T	class_name;		// allocated
    صحیح		class_flags;		// CLASS_ flags

    صحیح		class_refcount;
    صحیح		class_copyID;		// used by garbage collection
    class_T	*class_اگلا_مستعمل;	// for list headed by "first_class"
    class_T	*class_پچھلا_مستعمل;	// for list headed by "first_class"

    class_T	*class_extends;		// parent class or NULL

    // interfaces declared for the class
    صحیح		class_interface_گنتی;
    char_u	**class_interfaces;	// allocated array of names
    class_T	**class_interfaces_cl;	// interfaces (counts as reference)
    itf2class_T	*class_itf2class;	// member index lookup tables

    // class members: "static varname"
    صحیح		class_class_member_گنتی;
    ocmember_T	*class_class_members;	// allocated
    typval_T	*class_members_tv;	// allocated array of class member vals

    // class functions: "static def SomeMethod()"
    صحیح		class_class_function_گنتی;	    // total count
    صحیح		class_class_function_گنتی_بچہ;   // count without "extends"
    ufunc_T	**class_class_functions;	// allocated

    // object members: "this.varname"
    صحیح		class_obj_member_گنتی;
    ocmember_T	*class_obj_members;	// allocated

    // object methods: "def SomeMethod()"
    صحیح		class_obj_طریقہ_گنتی;		    // total count
    صحیح		class_obj_طریقہ_گنتی_بچہ;	    // count without "extends"
    ufunc_T	**class_obj_methods;	// allocated

					// index of builtin methods
    صحیح		class_builtin_methods[CLASS_BUILTIN_MAX];

    garray_T	class_type_list;	// used for type pointers
    type_T	class_type;		// type used for the class
    type_T	class_object_type;	// same as class_type but VAR_OBJECT
};

#تعریف_کرو IS_INTERFACE(cl)	((cl)->class_flags & CLASS_INTERFACE)
#تعریف_کرو IS_ENUM(cl)		((cl)->class_flags & CLASS_ENUM)

// Used for v_object of typval of VAR_OBJECT.
// The member variables follow in an array of typval_T.
ساخت object_S
{
    class_T	*obj_class;	    // class this object is created for;
				    // pointer adds to class_refcount
    صحیح		obj_refcount;

    object_T	*obj_اگلا_مستعمل;	    // for list headed by "first_object"
    object_T	*obj_پچھلا_مستعمل;	    // for list headed by "first_object"
    صحیح		obj_copyID;	    // used by garbage collection
};

ساخت typealias_S
{
    صحیح	    ta_refcount;
    type_T  *ta_type;
    char_u  *ta_name;
    صحیح	    ta_گہرائی;	    // number of levels of "ta_type"
};

/*
 * Structure to hold an internal variable without a name.
 */
ساخت typval_S
{
    vartype_T	v_type;
    حرف	v_تالا;	    // see below: VAR_LOCKED, VAR_FIXED
    اتحاد
    {
	varnumber_T	v_نمبر;	// number value
	float_T		v_float;	// floating point number value
	char_u		*v_string;	// string value (can be NULL)
	list_T		*v_list;	// list value (can be NULL)
	dict_T		*v_dict;	// dict value (can be NULL)
	partial_T	*v_partial;	// closure: function with args
#اگر_تعریف_شدہ FEAT_کام_CHANNEL
	کام_T		*v_کام;		// job value (can be NULL)
	channel_T	*v_channel;	// channel value (can be NULL)
#اگر_ختم
	blob_T		*v_blob;	// blob value (can be NULL)
	instr_T		*v_instr;	// instructions to execute
	class_T		*v_class;	// class value (can be NULL)
	object_T	*v_object;	// object value (can be NULL)
	typealias_T	*v_typealias;	// user-defined type name
	tuple_T		*v_tuple;	// tuple
    }		vval;
};

// Values for "dv_scope".
#تعریف_کرو VAR_SCOPE     1	// a:, v:, s:, etc. scope dictionaries
#تعریف_کرو VAR_DEF_SCOPE 2	// l:, g: scope dictionaries: here funcrefs are not
			// allowed to mask existing functions

// Values for "v_lock".
#تعریف_کرو VAR_LOCKED	    1	// locked with lock(), can use unlock()
#تعریف_کرو VAR_FIXED	    2	// locked forever
#تعریف_کرو VAR_اشیاء_LOCKED    4	// items of non-materialized list locked

/*
 * Structure to hold an item of a list: an internal variable without a name.
 */
قسمتعریف ساخت listitem_S listitem_T;

ساخت listitem_S
{
    listitem_T	*li_اگلا;	// next item in list
    listitem_T	*li_پچھلا;	// previous item in list
    typval_T	li_tv;		// type and value of the variable
};

// Struct used by those that are using an item in a list.
قسمتعریف ساخت listwatch_S listwatch_T;

ساخت listwatch_S
{
    listitem_T		*lw_شے;	// item being watched
    listwatch_T		*lw_اگلا;	// next watcher
};

/*
 * Structure to hold info about a list.
 * Order of members is optimized to reduce padding.
 * When created by range() it will at first have special value:
 *  lv_first == &range_list_item;
 * and use lv_start, lv_end, lv_stride.
 */
ساخت listvar_S
{
    listitem_T	*lv_پہلا;	// first item, NULL if none, &range_list_item
				// for a non-materialized list
    listwatch_T	*lv_watch;	// first watcher, NULL if none
    اتحاد {
	ساخت {	// used for non-materialized range list:
			// "lv_first" is &range_list_item
	    varnumber_T lv_آغاز;
	    varnumber_T lv_انتہا;
	    صحیح		lv_stride;
	} nonmat;
	ساخت {	// used for materialized list
	    listitem_T	*lv_آخری;	// last item, NULL if none
	    listitem_T	*lv_idx_شے;	// when not NULL item at index "lv_idx"
	    صحیح		lv_idx;		// cached index of an item
	} mat;
    } lv_u;
    type_T	*lv_type;	// current type, allocated by alloc_type()
    list_T	*lv_copylist;	// copied list used by deepcopy()
    list_T	*lv_مستعمل_اگلا;	// next list in used lists list
    list_T	*lv_مستعمل_پچھلا;	// previous list in used lists list
    صحیح		lv_refcount;	// reference count
    صحیح		lv_len;		// number of items
    صحیح		lv_with_اشیاء;	// number of items following this struct that
				// should not be freed
    صحیح		lv_copyID;	// ID used by deepcopy()
    حرف	lv_تالا;	// zero, VAR_LOCKED, VAR_FIXED
};

/*
 * Static list with 10 items.  Use init_static_list() to initialize.
 */
قسمتعریف ساخت {
    list_T	sl_list;	// must be first
    listitem_T	sl_اشیاء[10];
} staticList10_T;

/*
 * Structure to hold an item of a Dictionary.
 * Also used for a variable.
 * The key is copied into "di_key" to avoid an extra alloc/free for it.
 */
ساخت dictitem_S
{
    typval_T	di_tv;		// type and value of the variable
    char_u	di_flags;	// DI_FLAGS_ flags (only used for variable)
    char_u	di_کلید[1];	// key (actually longer!)
};
قسمتعریف ساخت dictitem_S dictitem_T;

/*
 * A dictitem with a 16 character key (plus NUL).  This is an efficient way to
 * have a fixed-size dictitem.
 */
#تعریف_کرو DICTITEM16_کلید_LEN 16
ساخت dictitem16_S
{
    typval_T	di_tv;		// type and value of the variable
    char_u	di_flags;	// DI_FLAGS_ flags (only used for variable)
    char_u	di_کلید[DICTITEM16_کلید_LEN + 1];	//کلید
};
قسمتعریف ساخت dictitem16_S dictitem16_T;

// Flags for "di_flags"
#تعریف_کرو DI_FLAGS_RO	   0x01	    // read-only variable
#تعریف_کرو DI_FLAGS_RO_SBX	   0x02	    // read-only in the sandbox
#تعریف_کرو DI_FLAGS_FIX	   0x04	    // fixed: no :unlet or remove()
#تعریف_کرو DI_FLAGS_تالا	   0x08	    // locked variable
#تعریف_کرو DI_FLAGS_ALLOC	   0x10	    // separately allocated
#تعریف_کرو DI_FLAGS_RELOAD	   0x20	    // set when script sourced again

/*
 * Structure to hold info about a Dictionary.
 */
ساخت dictvar_S
{
    حرف	dv_تالا;	// zero, VAR_LOCKED, VAR_FIXED
    حرف	dv_scope;	// zero, VAR_SCOPE, VAR_DEF_SCOPE
    صحیح		dv_refcount;	// reference count
    صحیح		dv_copyID;	// ID used by deepcopy()
    hashtab_T	dv_hashtab;	// hashtab that refers to the items
    type_T	*dv_type;	// current type, allocated by alloc_type()
    dict_T	*dv_copydict;	// copied dict used by deepcopy()
    dict_T	*dv_مستعمل_اگلا;	// next dict in used dicts list
    dict_T	*dv_مستعمل_پچھلا;	// previous dict in used dicts list
};

/*
 * Structure to hold info about a blob.
 */
ساخت blobvar_S
{
    garray_T	bv_ga;		// growarray with the data
    صحیح		bv_refcount;	// reference count
    حرف	bv_تالا;	// zero, VAR_LOCKED, VAR_FIXED
};

/*
 * Structure to hold info about a tuple.
 */
ساخت tuplevar_S
{
    garray_T	tv_اشیاء;	// tuple items
    type_T	*tv_type;	// current type, allocated by alloc_type()
    tuple_T	*tv_copytuple;	// copied tuple used by deepcopy()
    tuple_T	*tv_مستعمل_اگلا;	// next tuple in used tuples list
    tuple_T	*tv_مستعمل_پچھلا;	// previous tuple in used tuples list
    صحیح		tv_refcount;	// reference count
    صحیح		tv_copyID;	// ID used by deepcopy()
    حرف	tv_تالا;	// zero, VAR_LOCKED, VAR_FIXED
};

/*
 * Structure to hold a generic type information
 */
ساخت generictype_S
{
    type_T	*gt_type;	// generic or concrete type
    char_u	*gt_name;	// type name
};

/*
 * Generic function args table
 */
ساخت gfargs_tab_S
{
    garray_T	gfat_دلائل;
    garray_T	gfat_param_types;
    garray_T	gfat_arg_types;
};

قسمتعریف صحیح (*cfunc_T)(صحیح argcount, typval_T *argvars, typval_T *rettv, خالی *state);
قسمتعریف خالی (*cfunc_free_T)(خالی *state);

// type of getline() last argument
قسمتعریف شمار {
    GETLINE_NONE,	    // do not concatenate any lines
    GETLINE_CONCAT_CONT,    // concatenate continuation lines with backslash
    GETLINE_CONCAT_CONTBAR, // concatenate continuation lines with \ and |
    GETLINE_CONCAT_ALL	    // concatenate continuation and Vim9 # comment lines
} getline_opt_T;

قسمتعریف ساخت svar_S svar_T;

#اگر defined(FEAT_EVAL)
/*
 * Info used by a ":for" loop.
 */
قسمتعریف ساخت
{
    صحیح		fi_semicolon;	// TRUE if ending in '; var]'
    صحیح		fi_varcount;	// nr of variables in [] or zero
    صحیح		fi_break_گنتی;	// nr of line breaks encountered
    listwatch_T	fi_lw;		// keep an eye on the item used.
    list_T	*fi_list;	// list being used
    صحیح		fi_bi;		// index of blob
    blob_T	*fi_blob;	// blob being used
    char_u	*fi_string;	// copy of string being used
    صحیح		fi_byte_idx;	// byte index in fi_string
    tuple_T	*fi_tuple;	// tuple being used
    صحیح		fi_tuple_idx;	// tuple index in fi_tuple
    صحیح		fi_cs_flags;	// cs_flags or'ed together
} forinfo_T;

قسمتعریف ساخت funccall_S funccall_T;

// values used for "uf_def_status"
قسمتعریف شمار {
    UF_NOT_COMPILED,	    // executed with interpreter
    UF_مقصد_BE_COMPILED,	    // to be compiled before execution
    UF_COMPILING,	    // in compile_def_function()
    UF_COMPILED,	    // successfully compiled
    UF_COMPILE_ERROR	    // compilation error, cannot execute
} def_حالت_T;

/*
 * Structure to hold info for a user function.
 * When adding a field check copy_lambda_to_global_func().
 */
ساخت ufunc_S
{
    صحیح		uf_varargs;	// variable nr of arguments (old style)
    صحیح		uf_flags;	// FC_ flags
    صحیح		uf_calls;	// nr of active calls
    صحیح		uf_cleared;	// func_clear() was already called
    def_حالت_T uf_def_حالت; // UF_NOT_COMPILED, UF_TO_BE_COMPILED, etc.
    صحیح		uf_dfunc_idx;	// only valid if uf_def_status is UF_COMPILED

    class_T	*uf_class;	// for class/object method and constructor;
				// does not count for class_refcount.
				// class of the object which is invoking this
				// function.
    class_T	*uf_defclass;	// class where this function is defined.

    garray_T	uf_دلائل;	// arguments, including optional arguments
    garray_T	uf_def_دلائل;	// default argument expressions
    صحیح		uf_دلائل_visible; // normally uf_args.ga_len, less when
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
    صحیح		uf_بلاک_گہرائی;	// nr of entries in uf_block_ids
    صحیح		*uf_بلاک_ids;	// blocks a :def function is defined inside
# اگر defined(FEAT_LUA)
    cfunc_T     uf_cb;		// callback function for cfunc
    cfunc_free_T uf_cb_free;    // callback function to free cfunc
    خالی	*uf_cb_state;   // state of uf_cb
# endif

    // for generic functions
    صحیح		uf_generic_argcount;// type argument count
    generic_T	*uf_generic_دلائل;   // generic types
    type_T	*uf_generic_param_types; // list of allocated generic types
    garray_T	uf_generic_arg_types; // list of allocated type arguments
    hashtab_T	uf_generic_functab; // generic function table

    garray_T	uf_سطریں;	// function lines

    صحیح		uf_debug_tick;	// when last checked for a breakpoint in this
				// function.
    صحیح		uf_has_breakpoint;  // TRUE when a breakpoint has been set in
				    // this function.
# ifdef FEAT_PROFILE
    صحیح		uf_profiling;	// TRUE when func is being profiled
    صحیح		uf_prof_initialized;
    hash_T	uf_hash;	// hash for uf_name when profiling
    // profiling the function as a whole
    صحیح		uf_tm_گنتی;	// nr of calls
    proftime_T	uf_tm_کل;	// time spent in function + children
    proftime_T	uf_tm_self;	// time spent in function itself
    proftime_T	uf_tm_children;	// time spent in children this call
    // profiling the function per line
    صحیح		*uf_tml_گنتی;	// nr of times line was executed
    proftime_T	*uf_tml_کل;	// time spent in a line + children
    proftime_T	*uf_tml_self;	// time spent in a line itself
    proftime_T	uf_tml_آغاز;	// start time for current line
    proftime_T	uf_tml_children; // time spent in children for this line
    proftime_T	uf_tml_wait;	// start wait time for current line
    صحیح		uf_tml_idx;	// index of line being timed; -1 if none
    صحیح		uf_tml_execed;	// line being timed was executed
# endif
    sctx_T	uf_script_ctx;	// SCTX where function was defined,
				// used for s: variables; sc_version changed
				// for :function
    صحیح		uf_script_ctx_نسخہ;  // original sc_version of SCTX
    صحیح		uf_refcount;	// reference count, see func_name_refcount()

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
# define FC_احاطہ  0x08	// closure, uses outer scope variables
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
# define IS_OBJECT_طریقہ(ufunc) ((ufunc->uf_flags & FC_OBJECT) == FC_OBJECT)
// Is "ufunc" a class new() constructor method?
# define IS_CONSTRUCTOR_طریقہ(ufunc) ((ufunc->uf_flags & FC_NEW) == FC_NEW)
// Is "ufunc" an abstract class method?
# define IS_ABSTRACT_طریقہ(ufunc) ((ufunc->uf_flags & FC_ABSTRACT) == FC_ABSTRACT)
# define IS_GENERIC_FUNC(ufunc) (((ufunc)->uf_flags & FC_GENERIC) == FC_GENERIC)

# define MAX_FUNC_دلائل	20	// maximum number of function arguments
# define VAR_SHORT_LEN	20	// short variable name length
# define FIXVAR_CNT	12	// number of fixed variables

/*
 * Structure to hold info for a function that is currently being executed.
 */
ساخت funccall_S
{
    ufunc_T	*fc_func;	// function being called
    صحیح		fc_linenr;	// next line to be executed
    صحیح		fc_returned;	// ":return" used
    ساخت			// fixed variables for arguments
    {
	dictitem_T	var;		// variable (without room for name)
	char_u	گنجائش[VAR_SHORT_LEN];	// room for the name
    } fc_fixvar[FIXVAR_CNT];
    dict_T	fc_l_vars;	// l: local function variables
    dictitem_T	fc_l_vars_var;	// variable for l: scope
    dict_T	fc_l_avars;	// a: argument variables
    dictitem_T	fc_l_avars_var;	// variable for a: scope
    list_T	fc_l_varlist;	// list for a:000
    listitem_T	fc_l_listitems[MAX_FUNC_دلائل];	// listitems for a:000
    typval_T	*fc_rettv;	// return value
    linenr_T	fc_breakpoint;	// next line with breakpoint or zero
    صحیح		fc_dbg_tick;	// debug_tick when breakpoint was set
    صحیح		fc_level;	// top nesting level of executed function

    garray_T	fc_defer;	// functions to be called on return
    ectx_T	*fc_ectx;	// execution context for :def function, NULL
				// otherwise

# ifdef FEAT_PROFILE
    proftime_T	fc_prof_بچہ;	// time spent in a child
# endif
    funccall_T	*fc_caller;	// calling function or NULL; or next funccal in
				// list pointed to by previous_funccal.

    // for closure
    صحیح		fc_refcount;	// number of user functions that reference this
				// funccal
    صحیح		fc_copyID;	// for garbage collection
    garray_T	fc_ufuncs;	// list of ufunc_T* which keep a reference to
				// "fc_func"
};

// structure used as item in "fc_defer"
قسمتعریف ساخت
{
    char_u	*dr_name;	// function name, allocated
    typval_T	dr_argvars[MAX_FUNC_دلائل + 1];
    صحیح		dr_argcount;
} defer_T;

/*
 * Struct used by trans_function_name()
 */
قسمتعریف ساخت
{
    dict_T	*fd_dict;	// Dictionary used
    char_u	*fd_newkey;	// new key in "dict" in allocated memory
    dictitem_T	*fd_di;		// Dictionary item used
} funcdict_T;

قسمتعریف ساخت funccal_اندراج funccal_اندراج_T;
ساخت funccal_اندراج {
    خالی	    *چوٹی_funccal;
    funccal_اندراج_T *اگلا;
};

// From user function to hashitem and back.
# define UF2HIKEY(fp) ((fp)->uf_name)
# define HIKEY2UF(p)  ((ufunc_T *)((p) - offsetof(ufunc_T, uf_name)))
# define HI2UF(hi)     HIKEY2UF((hi)->hi_کلید)

/*
 * Holds the hashtab with variables local to each sourced script.
 * Each item holds a variable (nameless) that points to the dict_T.
 */
قسمتعریف ساخت {
    dictitem_T	sv_var;
    dict_T	sv_dict;
} scriptvar_T;

/*
 * Entry for "sn_all_vars".  Contains the s: variables from sn_vars plus the
 * block-local ones.
 */
قسمتعریف ساخت sallvar_S sallvar_T;
ساخت sallvar_S {
    sallvar_T	*sav_اگلا;	  // var with same name but different block
    صحیح		sav_بلاک_id;	  // block ID where declared
    صحیح		sav_var_vals_idx; // index in sn_var_vals

    // So long as the variable is valid (block it was defined in is still
    // active) "sav_di" is used.  It is set to NULL when leaving the block,
    // then sav_tv and sav_flags are used.
    dictitem_T *sav_di;		// dictitem with di_key and di_tv
    typval_T	sav_tv;		// type and value of the variable
    char_u	sav_flags;	// DI_FLAGS_ flags (only used for variable)
    char_u	sav_کلید[1];	// key (actually longer!)
};

/*
 * In the sn_all_vars hashtab item "hi_key" points to "sav_key" in a sallvar_T.
 * This makes it possible to store and find the sallvar_T.
 * SAV2HIKEY() converts a sallvar_T pointer to a hashitem key pointer.
 * HIKEY2SAV() converts a hashitem key pointer to a sallvar_T pointer.
 * HI2SAV() converts a hashitem pointer to a sallvar_T pointer.
 */
# define SAV2HIKEY(sav) ((sav)->sav_کلید)
# define HIKEY2SAV(p)  ((sallvar_T *)(p - offsetof(sallvar_T, sav_کلید)))
# define HI2SAV(hi)     HIKEY2SAV((hi)->hi_کلید)

# define SVFLAG_TYPE_ALLOCATED	1  // call free_type() for "sv_type"
# define SVFLAG_EXPORTED		2  // "export let var = val"
# define SVFLAG_ASSIGNED		4  // assigned a value

/*
 * Entry for "sn_var_vals".  Used for script-local variables.
 */
ساخت svar_S {
    char_u	*sv_name;	// points into "sn_all_vars" di_key
    typval_T	*sv_tv;		// points into "sn_vars" or "sn_all_vars" di_tv
    type_T	*sv_type;
    صحیح		sv_flags;	// SVFLAG_ values above
    صحیح		sv_const;	// 0, ASSIGN_CONST or ASSIGN_FINAL
};

قسمتعریف ساخت {
    char_u	*imp_name;	    // name imported as (allocated)
    scid_T	imp_sid;	    // script ID of "from"
    صحیح		imp_flags;	    // IMP_FLAGS_ values
} imported_T;

# define IMP_FLAGS_RELOAD	2   // script reloaded, OK to redefine
# define IMP_FLAGS_AUTOLOAD	4   // script still needs to be loaded

/*
 * Info about an encountered script.
 * When sn_state has SN_STATE_NOT_LOADED, it has not been sourced yet.
 */
قسمتعریف ساخت
{
    char_u	*sn_name;	    // full path of script file
    صحیح		sn_script_seq;	    // latest sctx_T sc_seq value

    // When non-zero the script ID of the actually sourced script.  Used if a
    // script is used by a name which has a symlink, we list both names, but
    // only the linked-to script is actually sourced.
    صحیح		sn_sourced_sid;

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
    صحیح		sn_current_بلاک_id; // ID for current block, 0 for outer
    صحیح		sn_آخری_بلاک_id;  // Unique ID for each script block

    صحیح		sn_نسخہ;	// :scriptversion
    صحیح		sn_state;	// SN_STATE_ values
    char_u	*sn_save_cpo;	// 'cpo' value when :vim9script found
    حرف	sn_is_vimrc;	// .vimrc file, do not restore 'cpo'
    حرف	sn_syml_checked;// flag: this has been checked for sym link

    // for a Vim9 script under "rtp/autoload/" this is "dir#scriptname#"
    char_u	*sn_autoload_سابقہ;

    // TRUE for a script used with "import autoload './dirname/script.vim'"
    // For "../autoload/script.vim" sn_autoload_prefix is also set.
    صحیح		sn_import_autoload;

# ifdef FEAT_PROFILE
    صحیح		sn_prof_on;	// TRUE when script is/was profiled
    صحیح		sn_pr_force;	// forceit: profile functions in this script
    proftime_T	sn_pr_بچہ;	// time set when going into first child
    صحیح		sn_pr_nest;	// nesting for sn_pr_child
    // profiling the script as a whole
    صحیح		sn_pr_گنتی;	// nr of times sourced
    proftime_T	sn_pr_کل;	// time spent in script + children
    proftime_T	sn_pr_self;	// time spent in script itself
    proftime_T	sn_pr_آغاز;	// time at script start
    proftime_T	sn_pr_children; // time in children after script start
    // profiling the script per line
    garray_T	sn_prl_ga;	// things stored for every line
    proftime_T	sn_prl_آغاز;	// start time for current line
    proftime_T	sn_prl_children; // time spent in children for this line
    proftime_T	sn_prl_wait;	// wait start time for current line
    صحیح		sn_prl_idx;	// index of line being timed; -1 if none
    صحیح		sn_prl_execed;	// line being timed was executed
# endif
} scriptitem_T;

# define SN_STATE_NEW		0   // newly loaded script, nothing done
# define SN_STATE_NOT_LOADED	1   // script located but not loaded
# define SN_STATE_RELOAD		2   // script loaded before, nothing done
# define SN_STATE_HAD_COMMAND	9   // a command was executed

// Struct passed through eval() functions.
// See EVALARG_EVALUATE for a fixed value with eval_flags set to EVAL_EVALUATE.
قسمتعریف ساخت {
    صحیح		eval_flags;	    // EVAL_ flag values below
    صحیح		eval_break_گنتی;   // nr of line breaks consumed

    // copied from exarg_T when "getline" is "getsourceline". Can be NULL.
    char_u	*(*eval_getline)(صحیح, خالی *, صحیح, getline_opt_T);
    خالی	*eval_cookie;	    // argument for eval_getline()

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
    صحیح		eval_using_cmdline;

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
قسمتعریف ساخت sn_prl_S
{
    صحیح		snp_گنتی;	// nr of times line was executed
    proftime_T	sn_prl_کل;	// time spent in a line + children
    proftime_T	sn_prl_self;	// time spent in a line itself
} sn_prl_T;

#  define PRL_شے(si, idx)	(((sn_prl_T *)(si)->sn_prl_ga.ga_ڈیٹا)[(idx)])

قسمتعریف ساخت {
    صحیح		pi_started_profiling;
    proftime_T	pi_wait_آغاز;
    proftime_T	pi_call_آغاز;
} profinfo_T;

# ورنہ
قسمتعریف ساخت
{
    صحیح	    dummy;
} profinfo_T;
# endif
#ورنہ
// dummy typedefs for use in function prototypes
ساخت ufunc_S
{
    صحیح	    dummy;
};
قسمتعریف ساخت
{
    صحیح	    dummy;
} funccall_T;
قسمتعریف ساخت
{
    صحیح	    dummy;
} funcdict_T;
قسمتعریف ساخت
{
    صحیح	    dummy;
} funccal_اندراج_T;
قسمتعریف ساخت
{
    صحیح	    dummy;
} scriptitem_T;
قسمتعریف ساخت
{
    صحیح	    dummy;
} evalarg_T;
#اگر_ختم

// Struct passed between functions dealing with function call execution.
//
// "fe_argv_func", when not NULL, can be used to fill in arguments only when the
// invoked function uses them.  It is called like this:
//   new_argcount = fe_argv_func(current_argcount, argv, partial_argcount,
//							called_func)
//
قسمتعریف ساخت {
    صحیح		(* fe_argv_func)(صحیح, typval_T *, صحیح, ufunc_T *);
    linenr_T	fe_firstline;	// first line of range
    linenr_T	fe_lastline;	// last line of range
    صحیح		*fe_doesrange;	// if not NULL: return: function handled range
    صحیح		fe_evaluate;	// actually evaluate expressions
    ufunc_T	*fe_ufunc;	// function to be called, when NULL lookup by
				// name
    partial_T	*fe_partial;	// for "dict" and extra arguments
    dict_T	*fe_selfdict;	// Dictionary for "self"
    object_T	*fe_object;	// object, e.g. for "this.Func()"
    typval_T	*fe_basetv;	// base for base->method()
    type_T	*fe_check_type;	// type from funcref or NULL
    صحیح		fe_ملا_var;	// if the function is not found then give an
				// error that a variable is not callable.
    cctx_T	*fe_cctx;	// when compiling a :def function
} funcexe_T;

/*
 * Structure to hold the context of a compiled function, used by closures
 * defined in that function.
 */
قسمتعریف ساخت funcstack_S funcstack_T;
ساخت funcstack_S
{
    funcstack_T *fs_اگلا;	// linked list at "first_funcstack"
    funcstack_T *fs_پچھلا;

    garray_T	fs_ga;		// contains the stack, with:
				// - arguments
				// - frame
				// - local variables
    صحیح		fs_var_انحراف;	// count of arguments + frame size == offset to
				// local variables

    صحیح		fs_refcount;	// nr of closures referencing this funcstack
    صحیح		fs_min_refcount; // nr of closures on this funcstack
    صحیح		fs_copyID;	// for garbage collection
};

/*
 * Structure to hold the variables declared in a loop that are possibly used
 * in a closure.
 */
قسمتعریف ساخت loopvars_S loopvars_T;
ساخت loopvars_S
{
    loopvars_T *lvs_اگلا;	// linked list at "first_loopvars"
    loopvars_T *lvs_پچھلا;

    garray_T	lvs_ga;		// contains the variables
    صحیح		lvs_refcount;	// nr of closures referencing this loopvars
    صحیح		lvs_min_refcount; // nr of closures on this loopvars
    صحیح		lvs_copyID;	// for garbage collection
};

// maximum nesting of :while and :for loops in a :def function
#تعریف_کرو MAX_LOOP_گہرائی 10

قسمتعریف ساخت outer_S outer_T;
ساخت outer_S {
    garray_T	*برآمد_ڈھیر;	    // stack from outer scope, or a copy
				    // containing only arguments and local vars
    صحیح		برآمد_چوکھٹا_idx;	    // index of stack frame in out_stack
    outer_T	*برآمد_اوپر;	    // outer scope of outer scope or NULL
    partial_T	*برآمد_اوپر_partial;    // partial owning out_up or NULL

    ساخت {
	garray_T *ڈھیر;	    // stack from outer scope, or a copy
				    // containing only vars inside the loop
	مختصر	 var_idx;	    // first variable defined in a loop in
				    // out_loop_stack
	مختصر	 var_گنتی;	    // number of variables defined in a loop
    } برآمد_loop[MAX_LOOP_گہرائی];
    صحیح		برآمد_loop_سائز;	    // nr of used entries in out_loop[]
};

ساخت partial_S
{
    صحیح		pt_refcount;	// reference count
    صحیح		pt_auto;	// when TRUE the partial was created for using
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
    loopvars_T	*(pt_loopvars[MAX_LOOP_گہرائی]);
				// copy of loop variables, used after loop
				// block ends

    typval_T	*pt_argv;	// arguments in allocated array
    صحیح		pt_argc;	// number of arguments

    صحیح		pt_copyID;	// funcstack may contain pointer to partial
    dict_T	*pt_dict;	// dict for "self"
    object_T	*pt_obj;	// object method
};

قسمتعریف ساخت {
    مختصر	lvi_گہرائی;	    // current nested loop depth
    ساخت {
	مختصر	var_idx;	    // index of first variable inside loop
	مختصر	var_گنتی;	    // number of variables inside loop
    } lvi_loop[MAX_LOOP_گہرائی];
} loopvarinfo_T;

قسمتعریف ساخت AutoPatCmd_S AutoPatCmd_T;

/*
 * Entry in the execution stack "exestack".
 */
قسمتعریف شمار {
    ETYPE_چوٹی,		    // toplevel
    ETYPE_SCRIPT,	    // sourcing script, use es_info.sctx
    ETYPE_UFUNC,	    // user function, use es_info.ufunc
    ETYPE_AUCMD,	    // autocomand, use es_info.aucmd
    ETYPE_MODELINE,	    // modeline, use es_info.sctx
    ETYPE_EXCEPT,	    // exception, use es_info.exception
    ETYPE_دلائل,		    // command line argument
    ETYPE_ENV,		    // environment variable
    ETYPE_اندرونی,	    // internal operation
    ETYPE_SPELL,	    // loading spell file
} etype_T;

قسمتعریف ساخت {
    طویل      es_lnum;      // replaces "sourcing_lnum"
    char_u    *es_name;     // replaces "sourcing_name"
    etype_T   es_type;
    اتحاد {
	sctx_T  *sctx;      // script and modeline info
#اگر defined(FEAT_EVAL)
	ufunc_T *ufunc;     // function info
#اگر_ختم
	AutoPatCmd_T *aucmd;  // autocommand info
	except_T   *except; // exception info
    } es_info;
#اگر defined(FEAT_EVAL)
    sctx_T	es_save_sctx;	    // saved current_sctx when calling function
#اگر_ختم
} estack_T;

// Information returned by get_tty_info().
قسمتعریف ساخت {
    صحیح backspace;	// what the Backspace key produces
    صحیح enter;		// what the Enter key produces
    صحیح مداخلت;	// interrupt character
    صحیح nl_does_cr;	// TRUE when a NL is expanded to CR-NL on output
} ttyinfo_T;

// Status of a job.  Order matters!
قسمتعریف شمار
{
    کام_FAILED,
    کام_STARTED,
    کام_ENDED,	    // detected job done
    کام_ختم_شدہ,   // job done and cleanup done
} jobstatus_T;

/*
 * Structure to hold info about a Job.
 */
ساخت jobvar_S
{
    کام_T	*jv_اگلا;
    کام_T	*jv_پچھلا;
#اگر_تعریف_شدہ UNIX
    pid_t	jv_pid;
#اگر_ختم
#اگر_تعریف_شدہ MSWIN
    PROCESS_INFORMATION	jv_proc_info;
    HANDLE		jv_کام_object;
#اگر_ختم
    jobstatus_T	jv_حالت;
    char_u	*jv_tty_اندر;	// controlling tty input, allocated
    char_u	*jv_tty_برآمد;	// controlling tty output, allocated
    char_u	*jv_stoponexit;	// allocated
#اگر_تعریف_شدہ UNIX
    char_u	*jv_termsig;	// allocated
#اگر_ختم
#اگر_تعریف_شدہ MSWIN
    char_u	*jv_tty_type;	// allocated
#اگر_ختم
    صحیح		jv_exitval;
    callback_T	jv_exit_cb;

    buf_T	*jv_اندر_buf;	// buffer from "in-name"

    صحیح		jv_refcount;	// reference count
    صحیح		jv_copyID;

    channel_T	*jv_channel;	// channel for I/O, reference counted
    حرف	**jv_argv;	// command line used to start the job
};

/*
 * Structures to hold info about a Channel.
 */
ساخت readq_S
{
    char_u	*rq_بفر;
    long_u	rq_buflen;
    readq_T	*rq_اگلا;
    readq_T	*rq_پچھلا;
};

ساخت writeq_S
{
    garray_T	wq_ga;
    writeq_T	*wq_اگلا;
    writeq_T	*wq_پچھلا;
};

ساخت jsonq_S
{
    typval_T	*jq_قدر;
    jsonq_T	*jq_اگلا;
    jsonq_T	*jq_پچھلا;
    صحیح		jq_no_callback; // TRUE when no callback was found
};

ساخت cbq_S
{
    callback_T	cq_callback;
    صحیح		cq_seq_nr;
    cbq_T	*cq_اگلا;
    cbq_T	*cq_پچھلا;
};

// mode for a channel
قسمتعریف شمار
{
    CH_منوال_NL = 0,
    CH_منوال_RAW,
    CH_منوال_BLOB,
    CH_منوال_JSON,
    CH_منوال_JS,
    CH_منوال_LSP,	// Language Server Protocol (http + json)
    CH_منوال_DAP		// Debug Adapter Protocol (like LSP, but does not
			// strictly follow JSON-RPC standard)
} ch_منوال_T;

قسمتعریف شمار {
    JIO_PIPE,	    // default
    JIO_NULL,
    JIO_FILE,
    JIO_بفر,
    JIO_برآمد
} کام_io_T;

#تعریف_کرو CH_PART_FD(part)	ch_part[part].ch_fd

// Ordering matters, it is used in for loops: IN is last, only SOCK/OUT/ERR
// are polled.
قسمتعریف شمار {
    PART_SOCK = 0,
#تعریف_کرو CH_SOCK_FD	CH_PART_FD(PART_SOCK)
#اگر_تعریف_شدہ FEAT_کام_CHANNEL
    PART_برآمد,
# define CH_برآمد_FD	CH_PART_FD(PART_برآمد)
    PART_ERR,
# define CH_ERR_FD	CH_PART_FD(PART_ERR)
    PART_اندر,
# define CH_اندر_FD	CH_PART_FD(PART_اندر)
#اگر_ختم
    PART_گنتی,
} ch_part_T;

#تعریف_کرو INVALID_FD	(-1)

// The per-fd info for a channel.
قسمتعریف ساخت {
    sock_T	ch_fd;	    // socket/stdin/stdout/stderr, -1 if not used

#اگر defined(UNIX) && !defined(HAVE_SELECT)
    صحیح		ch_poll_idx;	// used by channel_poll_setup()
#اگر_ختم

#اگر_تعریف_شدہ FEAT_GUI_X11
    Xt_داخلہ_Id	ch_داخلہ_Handler; // Cookie for input
#اگر_ختم
#اگر_تعریف_شدہ FEAT_GUI_GTK
    gint	ch_داخلہ_Handler; // Cookie for input
#اگر_ختم

    ch_منوال_T	ch_منوال;
    کام_io_T	ch_io;
    صحیح		ch_timeout;	// request timeout in msec

    readq_T	ch_سر;	// header for circular raw read queue
    jsonq_T	ch_json_سر;	// header for circular json read queue
    garray_T	ch_بلاک_ids;	// list of IDs that channel_read_json_block()
				// is waiting for
    // When ch_wait_len is non-zero use ch_deadline to wait for incomplete
    // message to be complete. The value is the length of the incomplete
    // message when the deadline was set.  If it gets longer (something was
    // received) the deadline is reset.
    size_t	ch_wait_len;
#اگر_تعریف_شدہ MSWIN
    DWORD	ch_deadline;
#ورنہ
    ساخت timeval ch_deadline;
#اگر_ختم
    صحیح		ch_بلاک_write;	// for testing: 0 when not used, -1 when write
				// does not block, 1 simulate blocking
    صحیح		ch_nonblocking;	// write() is non-blocking
    writeq_T	ch_writeque;	// header for write queue

    cbq_T	ch_cb_سر;	// dummy node for per-request callbacks
    callback_T	ch_callback;	// call when a msg is not handled

    bufref_T	ch_bufref;	// buffer to read from or write to
    صحیح		ch_nomodifiable; // TRUE when buffer can be 'nomodifiable'
    صحیح		ch_nomod_error;	// TRUE when e_modifiable was given
    صحیح		ch_buf_append;	// write appended lines instead top-bot
    linenr_T	ch_buf_چوٹی;	// next line to send
    linenr_T	ch_buf_bot;	// last line to send
} chanpart_T;

ساخت channel_S {
    channel_T	*ch_اگلا;
    channel_T	*ch_پچھلا;

    صحیح		ch_id;		// ID of the channel
    صحیح		ch_آخری_پیغام_id;	// ID of the last message

    chanpart_T	ch_part[PART_گنتی]; // info for socket, out, err and in
    صحیح		ch_write_متن_منوال; // write buffer lines with CR, not NL

    حرف	*ch_hostname;	// only for socket, allocated
    صحیح		ch_پورٹ;	// only for socket

    صحیح		ch_مقصد_be_closed; // bitset of readable fds to be closed.
				 // When all readable fds have been closed,
				 // set to (1 << PART_COUNT).
    صحیح		ch_مقصد_be_freed; // When TRUE channel must be freed when it's
				// safe to invoke callbacks.
    صحیح		ch_error;	// When TRUE an error was reported.  Avoids
    // giving pages full of error messages when
    // the other side has exited, only mention the
    // first error until the connection works
    // again.
    صحیح		ch_listen;	// When TRUE channel is listen socket.

    خالی	(*ch_nb_close_cb)(خالی);
				// callback for Netbeans when channel is
				// closed
#اگر_تعریف_شدہ FEAT_SOCKETSERVER
    bool	ch_socketserver; // If channel is used by socketserver
    خالی	(*ch_ss_close_cb)(channel_T *);
    خالی	(*ch_ss_accept_cb)(channel_T *);
    channel_T	*ch_ss_اگلا;
    channel_T	*ch_ss_پچھلا;
#اگر_ختم

#اگر_تعریف_شدہ MSWIN
    صحیح		ch_named_pipe;	// using named pipe instead of pty
#اگر_ختم
    callback_T	ch_callback;	// call when any msg is not handled
    callback_T	ch_close_cb;	// call when channel is closed
    صحیح		ch_drop_never;
    صحیح		ch_keep_open;	// do not close on read error
    صحیح		ch_nonblock;

    کام_T	*ch_کام;	// Job that uses this channel; this does not
				// count as a reference to avoid a circular
				// reference, the job refers to the channel.
    صحیح		ch_کام_killed;	// TRUE when there was a job and it was killed
				// or we know it died.
    صحیح		ch_anonymous_pipe;  // Indicates that anonymous pipes are being
				    // used for communication in the Windows
				    // ConPTY terminal.
    صحیح		ch_killing;	    // Indicates that the job associated with
				    // the channel is terminating.  It becomes
				    // TRUE when TerminateJobObject() was
				    // called or the process associated with
				    // the job had exited (only ConPTY).

    صحیح		ch_refcount;	// reference count
    صحیح		ch_copyID;
};

#تعریف_کرو JO_منوال		    0x0001	// channel mode
#تعریف_کرو JO_اندر_منوال	    0x0002	// stdin mode
#تعریف_کرو JO_برآمد_منوال	    0x0004	// stdout mode
#تعریف_کرو JO_ERR_منوال	    0x0008	// stderr mode
#تعریف_کرو JO_CALLBACK	    0x0010	// channel callback
#تعریف_کرو JO_برآمد_CALLBACK	    0x0020	// stdout callback
#تعریف_کرو JO_ERR_CALLBACK	    0x0040	// stderr callback
#تعریف_کرو JO_CLOSE_CALLBACK   0x0080	// "close_cb"
#تعریف_کرو JO_WAITTIME	    0x0100	// only for ch_open()
#تعریف_کرو JO_TIMEOUT	    0x0200	// all timeouts
#تعریف_کرو JO_برآمد_TIMEOUT	    0x0400	// stdout timeouts
#تعریف_کرو JO_ERR_TIMEOUT	    0x0800	// stderr timeouts
#تعریف_کرو JO_PART		    0x1000	// "part"
#تعریف_کرو JO_ID		    0x2000	// "id"
#تعریف_کرو JO_STOPONEXIT	    0x4000	// "stoponexit"
#تعریف_کرو JO_EXIT_CB	    0x8000	// "exit_cb"
#تعریف_کرو JO_برآمد_IO	    0x10000	// "out_io"
#تعریف_کرو JO_ERR_IO	    0x20000	// "err_io" (JO_OUT_IO << 1)
#تعریف_کرو JO_اندر_IO	    0x40000	// "in_io" (JO_OUT_IO << 2)
#تعریف_کرو JO_برآمد_NAME	    0x80000	// "out_name"
#تعریف_کرو JO_ERR_NAME	    0x100000	// "err_name" (JO_OUT_NAME << 1)
#تعریف_کرو JO_اندر_NAME	    0x200000	// "in_name" (JO_OUT_NAME << 2)
#تعریف_کرو JO_اندر_چوٹی	    0x400000	// "in_top"
#تعریف_کرو JO_اندر_BOT	    0x800000	// "in_bot"
#تعریف_کرو JO_برآمد_BUF	    0x1000000	// "out_buf"
#تعریف_کرو JO_ERR_BUF	    0x2000000	// "err_buf" (JO_OUT_BUF << 1)
#تعریف_کرو JO_اندر_BUF	    0x4000000	// "in_buf" (JO_OUT_BUF << 2)
#تعریف_کرو JO_CHANNEL	    0x8000000	// "channel"
#تعریف_کرو JO_بلاک_WRITE	    0x10000000	// "block_write"
#تعریف_کرو JO_برآمد_MODIFIABLE   0x20000000	// "out_modifiable"
#تعریف_کرو JO_ERR_MODIFIABLE   0x40000000	// "err_modifiable" (JO_OUT_ << 1)
#تعریف_کرو JO_ALL		    0x7fffffff

#تعریف_کرو JO2_برآمد_پیغام	    0x0001	// "out_msg"
#تعریف_کرو JO2_ERR_پیغام	    0x0002	// "err_msg" (JO_OUT_ << 1)
#تعریف_کرو JO2_رکن_NAME	    0x0004	// "term_name"
#تعریف_کرو JO2_رکن_اختتام	    0x0008	// "term_finish"
#تعریف_کرو JO2_ENV		    0x0010	// "env"
#تعریف_کرو JO2_CWD		    0x0020	// "cwd"
#تعریف_کرو JO2_رکن_صفیں	    0x0040	// "term_rows"
#تعریف_کرو JO2_رکن_ستون_تعداد	    0x0080	// "term_cols"
#تعریف_کرو JO2_VERTICAL	    0x0100	// "vertical"
#تعریف_کرو JO2_CURWIN	    0x0200	// "curwin"
#تعریف_کرو JO2_HIDDEN	    0x0400	// "hidden"
#تعریف_کرو JO2_رکن_OPENCMD    0x0800	// "term_opencmd"
#تعریف_کرو JO2_EOF_CHARS	    0x1000	// "eof_chars"
#تعریف_کرو JO2_NORESTORE	    0x2000	// "norestore"
#تعریف_کرو JO2_رکن_KILL	    0x4000	// "term_kill"
#تعریف_کرو JO2_ANSI_COLORS	    0x8000	// "ansi_colors"
#تعریف_کرو JO2_TTY_TYPE	    0x10000	// "tty_type"
#تعریف_کرو JO2_BUFNR	    0x20000	// "bufnr"
#تعریف_کرو JO2_رکن_API	    0x40000	// "term_api"
#تعریف_کرو JO2_رکن_HIGHLIGHT  0x80000	// "highlight"

#تعریف_کرو JO_منوال_ALL	(JO_منوال + JO_اندر_منوال + JO_برآمد_منوال + JO_ERR_منوال)
#تعریف_کرو JO_CB_ALL \
    (JO_CALLBACK + JO_برآمد_CALLBACK + JO_ERR_CALLBACK + JO_CLOSE_CALLBACK)
#تعریف_کرو JO_TIMEOUT_ALL	(JO_TIMEOUT + JO_برآمد_TIMEOUT + JO_ERR_TIMEOUT)

/*
 * Options for job and channel commands.
 */
قسمتعریف ساخت
{
    صحیح		jo_سیٹ;		// JO_ bits for values that were set
    صحیح		jo_set2;	// JO2_ bits for values that were set

    ch_منوال_T	jo_منوال;
    ch_منوال_T	jo_اندر_منوال;
    ch_منوال_T	jo_برآمد_منوال;
    ch_منوال_T	jo_err_منوال;
    صحیح		jo_noblock;

    کام_io_T	jo_io[4];	// PART_OUT, PART_ERR, PART_IN
    char_u	jo_io_name_buf[4][NUMBUFLEN];
    char_u	*jo_io_name[4];	// not allocated!
    صحیح		jo_io_buf[4];
    صحیح		jo_pty;
    صحیح		jo_modifiable[4];
    صحیح		jo_پیغام[4];
    channel_T	*jo_channel;

    linenr_T	jo_اندر_چوٹی;
    linenr_T	jo_اندر_bot;

    callback_T	jo_callback;
    callback_T	jo_برآمد_cb;
    callback_T	jo_err_cb;
    callback_T	jo_close_cb;
    callback_T	jo_exit_cb;
    صحیح		jo_drop_never;
    صحیح		jo_waittime;
    صحیح		jo_timeout;
    صحیح		jo_برآمد_timeout;
    صحیح		jo_err_timeout;
    صحیح		jo_بلاک_write;	// for testing only
    صحیح		jo_part;
    صحیح		jo_id;
    char_u	jo_stoponexit_buf[NUMBUFLEN];
    char_u	*jo_stoponexit;
    dict_T	*jo_env;	// environment variables
    char_u	jo_cwd_buf[NUMBUFLEN];
    char_u	*jo_cwd;

#اگر_تعریف_شدہ FEAT_TERMINAL
    // when non-zero run the job in a terminal window of this size
    صحیح		jo_رکن_صفیں;
    صحیح		jo_رکن_ستون_تعداد;
    صحیح		jo_vertical;
    صحیح		jo_curwin;
    buf_T	*jo_bufnr_buf;
    صحیح		jo_hidden;
    صحیح		jo_رکن_norestore;
    char_u	jo_رکن_name_buf[NUMBUFLEN];
    char_u	*jo_رکن_name;
    char_u	jo_رکن_opencmd_buf[NUMBUFLEN];
    char_u	*jo_رکن_opencmd;
    صحیح		jo_رکن_اختتام;
    char_u	jo_eof_chars_buf[NUMBUFLEN];
    char_u	*jo_eof_chars;
    char_u	jo_رکن_kill_buf[NUMBUFLEN];
    char_u	*jo_رکن_kill;
# اگر defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    long_u	jo_ansi_colors[16];
# endif
    char_u	jo_رکن_highlight_buf[NUMBUFLEN];
    char_u	*jo_رکن_highlight;
    صحیح		jo_tty_type;	    // first character of "tty_type"
    char_u	jo_رکن_api_buf[NUMBUFLEN];
    char_u	*jo_رکن_api;
#اگر_ختم
} jobopt_T;

#اگر_تعریف_شدہ FEAT_EVAL
/*
 * Structure used for listeners added with listener_add().
 */
قسمتعریف ساخت listener_S listener_T;
ساخت listener_S
{
    listener_T	*lr_اگلا;
    صحیح		lr_id;
    bool	lr_متن;	// include the resulting text in each change
    callback_T	lr_callback;
};

// Structure used for listeners added with redraw_listener_add().
قسمتعریف ساخت redraw_listener_S redraw_listener_T;
ساخت redraw_listener_S
{
    redraw_listener_T	*rl_اگلا;
    صحیح			rl_id;
    ساخت
    {
	callback_T	on_آغاز;
	callback_T	on_انتہا;
    }			rl_callbacks;
};
#اگر_ختم

/*
 * structure used for explicit stack while garbage collecting hash tables
 */
قسمتعریف ساخت ht_ڈھیر_S
{
    hashtab_T		*ht;
    ساخت ht_ڈھیر_S	*پچھلا;
} ht_ڈھیر_T;

/*
 * structure used for explicit stack while garbage collecting lists
 */
قسمتعریف ساخت list_ڈھیر_S
{
    list_T		*list;
    ساخت list_ڈھیر_S	*پچھلا;
} list_ڈھیر_T;

/*
 * structure used for explicit stack while garbage collecting tuples
 */
قسمتعریف ساخت tuple_ڈھیر_S
{
    tuple_T			*tuple;
    ساخت tuple_ڈھیر_S	*پچھلا;
} tuple_ڈھیر_T;

/*
 * Structure used for iterating over dictionary items.
 * Initialize with dict_iterate_start().
 */
قسمتعریف ساخت
{
    long_u	dit_todo;
    hashitem_T	*dit_hi;
} dict_iterator_T;

// values for b_syn_spell: what to do with toplevel text
#تعریف_کرو SYNSPL_DEFAULT	0	// spell check if @Spell not defined
#تعریف_کرو SYNSPL_چوٹی	1	// spell check toplevel text
#تعریف_کرو SYNSPL_NOTOP	2	// don't spell check toplevel text

// values for b_syn_foldlevel: how to compute foldlevel on a line
#تعریف_کرو SYNFLD_آغاز	0	// use level of item at start of line
#تعریف_کرو SYNFLD_MINIMUM	1	// use lowest local minimum level on line

// avoid #ifdefs for when b_spell is not available
#اگر_تعریف_شدہ FEAT_SPELL
# define B_SPELL(buf)  ((buf)->b_spell)
#ورنہ
# define B_SPELL(buf)  (0)
#اگر_ختم

قسمتعریف ساخت qf_info_S qf_info_T;

#اگر_تعریف_شدہ FEAT_PROFILE
/*
 * Used for :syntime: timing of executing a syntax pattern.
 */
قسمتعریف ساخت {
    proftime_T	کل;		// total time used
    proftime_T	slowest;	// time of slowest call
    طویل	گنتی;		// nr of times used
    طویل	مطابقت;		// nr of times matched
} syn_وقت_T;
#اگر_ختم

قسمتعریف ساخت timer_S timer_T;
ساخت timer_S
{
    طویل	tr_id;
#اگر_تعریف_شدہ FEAT_TIMERS
    timer_T	*tr_اگلا;
    timer_T	*tr_پچھلا;
    proftime_T	tr_due;		    // when the callback is to be invoked
    حرف	tr_firing;	    // when TRUE callback is being called
    حرف	tr_paused;	    // when TRUE callback is not invoked
    حرف	tr_keep;	    // when TRUE keep timer after it fired
    صحیح		tr_repeat;	    // number of times to repeat, -1 forever
    طویل	tr_interval;	    // msec
    callback_T	tr_callback;
    صحیح		tr_emsg_گنتی;
#اگر_ختم
};

#اگر_تعریف_شدہ FEAT_CRYPT
/*
 * Structure to hold the type of encryption and the state of encryption or
 * decryption.
 */
قسمتعریف ساخت {
    صحیح	    طریقہ_nr;
    خالی    *طریقہ_state;  // method-specific state information
} cryptstate_T;

// values for method_nr
# define CRYPT_M_ZIP	0
# define CRYPT_M_BF	1
# define CRYPT_M_BF2	2
# define CRYPT_M_SOD    3
# define CRYPT_M_SOD2   4
# define CRYPT_M_گنتی	5 // number of crypt methods

// Currently all crypt methods work inplace.  If one is added that isn't then
// define this.
# define CRYPT_NOT_INPLACE 1

// Struct for passing arguments down to the crypt_init functions
قسمتعریف ساخت {
    char_u	*cat_salt;
    صحیح		cat_salt_len;
    char_u	*cat_بیج;
    صحیح		cat_بیج_len;
    char_u	*cat_جوڑو;
    صحیح		cat_جوڑو_len;
    صحیح		cat_init_from_file;
} crypt_arg_T;

#اگر_ختم

#اگر_تعریف_شدہ FEAT_PROP_POPUP
قسمتعریف شمار {
    POPPOS_BOTLEFT,
    POPPOS_TOPLEFT,
    POPPOS_BOTRIGHT,
    POPPOS_TOPRIGHT,
    POPPOS_CENTER,
    POPPOS_BOTTOM,	// bottom of popup just above the command line
    POPPOS_NONE
} poppos_T;

قسمتعریف شمار {
    POPCLOSE_NONE,
    POPCLOSE_BUTTON,
    POPCLOSE_CLICK
} popclose_T;

# define POPUPWIN_DEFAULT_ZINDEX	 50
# define POPUPMENU_ZINDEX		100
# define POPUPWIN_DIALOG_ZINDEX		200
# define POPUPWIN_NOTIFICATION_ZINDEX   300
#اگر_ختم

/*
 * These are items normally related to a buffer.  But when using ":ownsyntax"
 * a window may have its own instance.
 */
قسمتعریف ساخت {
#اگر_تعریف_شدہ FEAT_SYN_HL
    hashtab_T	b_keywtab;		// syntax keywords hash table
    hashtab_T	b_keywtab_ic;		// idem, ignore case
    صحیح		b_syn_error;		// TRUE when error occurred in HL
# ifdef FEAT_RELTIME
    صحیح		b_syn_slow;		// TRUE when 'redrawtime' reached
# endif
    صحیح		b_syn_ic;		// ignore case for :syn cmds
    صحیح		b_syn_foldlevel;	// how to compute foldlevel on a line
    صحیح		b_syn_spell;		// SYNSPL_ values
    garray_T	b_syn_patterns;		// table for syntax patterns
    garray_T	b_syn_clusters;		// table for syntax clusters
    صحیح		b_spell_cluster_id;	// @Spell cluster ID or 0
    صحیح		b_nospell_cluster_id;	// @NoSpell cluster ID or 0
    صحیح		b_syn_containedin;	// TRUE when there is an item with a
					// "containedin" argument
    صحیح		b_syn_sync_flags;	// flags about how to sync
    مختصر	b_syn_sync_id;		// group to sync on
    طویل	b_syn_sync_minlines;	// minimal sync lines offset
    طویل	b_syn_sync_maxlines;	// maximal sync lines offset
    طویل	b_syn_sync_linebreaks;	// offset for multi-line pattern
    char_u	*b_syn_linecont_pat;	// line continuation pattern
    regprog_T	*b_syn_linecont_prog;	// line continuation program
# ifdef FEAT_PROFILE
    syn_وقت_T  b_syn_linecont_وقت;
# endif
    صحیح		b_syn_linecont_ic;	// ignore-case flag for above
    صحیح		b_syn_topgrp;		// for ":syntax include"
# ifdef FEAT_CONCEAL
    صحیح		b_syn_conceal;		// auto-conceal for :syn cmds
# endif
# ifdef FEAT_FOLDING
    صحیح		b_syn_folditems;	// number of patterns with the HL_FOLD
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
    صحیح		b_sst_len;
    synstate_T	*b_sst_پہلا;
    synstate_T	*b_sst_تلاش;
    synstate_T	*b_sst_firstfree;
    صحیح		b_sst_freecount;
    linenr_T	b_sst_check_lnum;
    short_u	b_sst_lasttick;	// last display tick

    // Cache for in_id_list(); see idl_cache_T in syntax.c.
    خالی	*b_idlist_cache;
#اگر_ختم // FEAT_SYN_HL

#اگر_تعریف_شدہ FEAT_SPELL
    // for spell checking
    garray_T	b_langp;	    // list of pointers to slang_T, see spell.c
    char_u	b_spell_ismw[256];  // flags: is midword char
    char_u	*b_spell_ismw_mb;   // multi-byte midword chars
    char_u	*b_p_spc;	    // 'spellcapcheck'
    regprog_T	*b_cap_prog;	    // program for 'spellcapcheck'
    char_u	*b_p_spf;	    // 'spellfile'
    char_u	*b_p_spl;	    // 'spelllang'
    char_u	*b_p_spo;	    // 'spelloptions'
    صحیح		b_cjk;		    // all CJK letters as OK
#اگر_ختم
#اگر !defined(FEAT_SYN_HL) && !defined(FEAT_SPELL)
    صحیح		dummy;
#اگر_ختم
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

ساخت file_بفر
{
    memline_T	b_ml;		// associated memline (also contains line
				// count)

    buf_T	*b_اگلا;	// links in list of buffers
    buf_T	*b_پچھلا;

    صحیح		b_nwindows;	// nr of windows open on this buffer

    صحیح		b_flags;	// various BF_ flags
    صحیح		b_locked;	// Buffer is being closed or referenced, don't
				// let autocommands wipe it out.
    صحیح		b_locked_بانٹو;	// Buffer is being closed, don't allow opening
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

#اگر_تعریف_شدہ UNIX
    bool	b_dev_valid;	// true when b_dev has a valid number
    dev_t	b_dev;		// device number
    ino_t	b_ino;		// inode number
#اگر_ختم
#اگر_تعریف_شدہ VMS
    حرف	 b_fab_rfm;	// Record format
    حرف	 b_fab_rat;	// Record attribute
    غیرعلامتی صحیح b_fab_mrs;	// Max record size
#اگر_ختم
    صحیح		b_fnum;		// buffer number for this file.
    char_u	b_کلید[VIM_SIZEOF_INT * 2 + 1];
				// key used for buf_hashtab, holds b_fnum as
				// hex string

    صحیح		b_بدلا;	// 'modified': Set to TRUE if something in the
				// file has been changed and not written out.
    dictitem16_T b_ct_di;	// holds the b:changedtick value in
				// b_ct_di.di_tv.vval.v_number;
				// incremented for each change, also for undo
#تعریف_کرو CHANGEDTICK(buf) ((buf)->b_ct_di.di_tv.vval.v_نمبر)

    varnumber_T	b_آخری_changedtick;	// b:changedtick when TextChanged was
					// last triggered.
    varnumber_T	b_آخری_changedtick_pum; // b:changedtick for TextChangedP
    varnumber_T	b_آخری_changedtick_i;   // b:changedtick for TextChangedI

    bool	b_saving;	// Set to true if we are in the middle of
				// saving the buffer.

    /*
     * Changes to a buffer require updating of the display.  To minimize the
     * work, remember changes made and update everything at once.
     */
    bool	b_mod_سیٹ;	// true when there are changes since the last
				// time the display was updated
    linenr_T	b_mod_چوٹی;	// topmost lnum that was changed
    linenr_T	b_mod_bot;	// lnum below last changed line, AFTER the
				// change
    طویل	b_mod_xlines;	// number of extra buffer lines inserted;
				// negative when lines were deleted

    wininfo_T	*b_wininfo;	// list of last used info for each window

    طویل	b_mtime;	// last change time of original file
    طویل	b_mtime_ns;	// nanoseconds of last change time
    طویل	b_mtime_read;	// last change time when reading
    طویل	b_mtime_read_ns;  // nanoseconds of last read time
    off_T	b_orig_سائز;	// size of original file in bytes
    صحیح		b_orig_منوال;	// mode of original file
#اگر_تعریف_شدہ FEAT_VIMINFO
    وقت_T	b_آخری_مستعمل;	// time when the buffer was last used; used
				// for viminfo
#اگر_ختم

    pos_T	b_namedm[NMARKS]; // current named marks (mark.c)

    // These variables are set when VIsual_active becomes FALSE
    visualinfo_T b_visual;
#اگر_تعریف_شدہ FEAT_EVAL
    صحیح		b_visual_منوال_eval;  // b_visual.vi_mode for visualmode()
#اگر_ختم

    pos_T	b_آخری_cursor;	// cursor position when last unloading this
				//بفر
    pos_T	b_آخری_داخل_کرو;	// where Insert mode was left
    pos_T	b_آخری_change;	// position of last change: '. mark

    /*
     * the changelist contains old change positions
     */
    pos_T	b_changelist[JUMPLISTSIZE];
    صحیح		b_changelistlen;	// number of active entries
    bool	b_new_change;		// set in u_savecommon()

    /*
     * Character table, only used in charset.c for 'iskeyword'
     * 32 bytes of 8 bits: 1 bit per character 0-255.
     */
    char_u	b_chartab[32];

    // Table used for mappings local to a buffer.
    mapblock_T	*(b_maphash[256]);

    // First abbreviation local to a buffer.
    mapblock_T	*b_پہلا_abbr;

    // User commands local to the buffer.
    garray_T	b_ucmds;
    // start and end of an operator, also used for '[ and ']
    pos_T	b_op_آغاز;
    pos_T	b_op_آغاز_orig;  // used for Insstart_orig
    pos_T	b_op_انتہا;

#اگر_تعریف_شدہ FEAT_VIMINFO
    bool	b_marks_read;	// Have we read viminfo marks yet?
#اگر_ختم

    bool	b_modified_was_سیٹ;	// did ":set modified"
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
    u_سرخی_T	*b_u_oldhead;	// pointer to oldest header
    u_سرخی_T	*b_u_newhead;	// pointer to newest header; may not be valid
				// if b_u_curhead is not NULL
    u_سرخی_T	*b_u_curhead;	// pointer to current header
    صحیح		b_u_numhead;	// current number of headers
    bool	b_u_synced;	// entry lists are synced
    طویل	b_u_seq_آخری;	// last used undo sequence number
    طویل	b_u_save_nr_آخری; // counter for last file write
    طویل	b_u_seq_cur;	// uh_seq of header below which we are now
    وقت_T	b_u_وقت_cur;	// uh_time of header below which we are now
    طویل	b_u_save_nr_cur; // file write nr after which we are now

    /*
     * variables for "U" command in undo.c
     */
    undoline_T	b_u_سطر_ptr;	// saved line for "U" command
    linenr_T	b_u_سطر_lnum;	// line number of line in u_line
    colnr_T	b_u_سطر_colnr;	// optional column number

    bool	b_scanned;	// ^N/^P have scanned this buffer

    // flags for use of ":lmap" and IM control
    طویل	b_p_iminsert;	// input mode for insert
    طویل	b_p_imsearch;	// input mode for search
#تعریف_کرو B_IMODE_USE_داخل_کرو (-1)	//	Use b_p_iminsert value for search
#تعریف_کرو B_IMODE_NONE 0		//	Input via none
#تعریف_کرو B_IMODE_LMAP 1		//	Input via langmap
#تعریف_کرو B_IMODE_IM 2		//	Input via input method
#تعریف_کرو B_IMODE_آخری 2

#اگر_تعریف_شدہ FEAT_KEYMAP
    مختصر	b_kmap_state;	// using "lmap" mappings
# define KEYMAP_INIT	1	// 'keymap' was set, call keymap_init()
# define KEYMAP_LOADED	2	// 'keymap' mappings have been loaded
    garray_T	b_kmap_ga;	// the keymap table
#اگر_ختم

    /*
     * Options local to a buffer.
     * They are here because their value depends on the type of file
     * or contents of the file being edited.
     */
    bool	b_p_initialized;	// set when options initialized

#اگر_تعریف_شدہ FEAT_EVAL
    sctx_T	b_p_script_ctx[BV_گنتی]; // SCTXs for buffer-local options
#اگر_ختم

    صحیح		b_p_ac;		// 'autocomplete'
    صحیح		b_p_ai;		// 'autoindent'
    صحیح		b_p_ai_nopaste;	// b_p_ai saved for paste mode
    char_u	*b_p_bkc;	// 'backupcopy'
    غیرعلامتی	b_bkc_flags;    // flags for 'backupcopy'
    صحیح		b_p_ci;		// 'copyindent'
    صحیح		b_p_bin;	// 'binary'
    صحیح		b_p_bomb;	// 'bomb'
    char_u	*b_p_bh;	// 'bufhidden'
    char_u	*b_p_bt;	// 'buftype'
#اگر_تعریف_شدہ FEAT_QUICKFIX
# define BUF_HAS_QF_اندراج 1
# define BUF_HAS_LL_اندراج 2
    صحیح		b_has_qf_اندراج;
#اگر_ختم
    صحیح		b_p_bl;		// 'buflisted'
    صحیح		b_p_cin;	// 'cindent'
    char_u	*b_p_cino;	// 'cinoptions'
    char_u	*b_p_cink;	// 'cinkeys'
    char_u	*b_p_cinsd;	// 'cinscopedecls'
    char_u	*b_p_cinw;	// 'cinwords'
    char_u	*b_p_com;	// 'comments'
    char_u	*b_p_cms;	// 'commentstring'
    char_u	*b_p_cot;	// 'completeopt' local value
    غیرعلامتی	b_cot_flags;	// flags for 'completeopt'
    char_u	*b_p_cpt;	// 'complete'
#اگر_تعریف_شدہ BACKSLASH_اندر_FILENAME
    char_u	*b_p_csl;	// 'completeslash'
#اگر_ختم
#اگر_تعریف_شدہ FEAT_COMPL_FUNC
    long_u	b_p_cpt_flags;	// flags for 'complete'
    callback_T	*b_p_cpt_cb;	// F{func} in 'complete' callback
    صحیح		b_p_cpt_گنتی;	// Count of values in 'complete'
    char_u	*b_p_cfu;	// 'completefunc'
    callback_T	b_cfu_cb;	// 'completefunc' callback
    char_u	*b_p_ofu;	// 'omnifunc'
    callback_T	b_ofu_cb;	// 'omnifunc' callback
#اگر_ختم
#اگر_تعریف_شدہ FEAT_EVAL
    char_u	*b_p_tfu;	// 'tagfunc' option value
    callback_T	b_tfu_cb;	// 'tagfunc' callback
    char_u	*b_p_ffu;	// 'findfunc' option value
    callback_T	b_ffu_cb;	// 'findfunc' callback
#اگر_ختم
    صحیح		b_p_eof;	// 'endoffile'
    صحیح		b_p_eol;	// 'endofline'
    صحیح		b_p_fixeol;	// 'fixendofline'
    صحیح		b_p_et;		// 'expandtab'
    صحیح		b_p_et_nobin;	// b_p_et saved for binary mode
    صحیح		b_p_et_nopaste; // b_p_et saved for paste mode
    char_u	*b_p_fenc;	// 'fileencoding'
    char_u	*b_p_ff;	// 'fileformat'
    char_u	*b_p_ft;	// 'filetype'
    char_u	*b_p_fo;	// 'formatoptions'
    char_u	*b_p_flp;	// 'formatlistpat'
    صحیح		b_p_inf;	// 'infercase'
    char_u	*b_p_isk;	// 'iskeyword'
#اگر_تعریف_شدہ FEAT_ڈھونڈو_ID
    char_u	*b_p_def;	// 'define' local value
    char_u	*b_p_inc;	// 'include'
# ifdef FEAT_EVAL
    char_u	*b_p_inex;	// 'includeexpr'
    long_u	b_p_inex_flags;	// flags for 'includeexpr'
# endif
#اگر_ختم
#اگر defined(FEAT_EVAL)
    char_u	*b_p_inde;	// 'indentexpr'
    long_u	b_p_inde_flags;	// flags for 'indentexpr'
    char_u	*b_p_indk;	// 'indentkeys'
#اگر_ختم
    char_u	*b_p_fp;	// 'formatprg'
#اگر defined(FEAT_EVAL)
    char_u	*b_p_fex;	// 'formatexpr'
    long_u	b_p_fex_flags;	// flags for 'formatexpr'
#اگر_ختم
#اگر_تعریف_شدہ HAVE_FSYNC
    صحیح		b_p_fs;		// 'fsync'
#اگر_ختم
#اگر_تعریف_شدہ FEAT_CRYPT
    char_u	*b_p_کلید;	// 'key'
#اگر_ختم
    char_u	*b_p_kp;	// 'keywordprg'
    صحیح		b_p_lisp;	// 'lisp'
    char_u	*b_p_lop;	// 'lispoptions'
    char_u	*b_p_menc;	// 'makeencoding'
    char_u	*b_p_mps;	// 'matchpairs'
    صحیح		b_p_ml;		// 'modeline'
    صحیح		b_p_ml_nobin;	// b_p_ml saved for binary mode
    صحیح		b_p_ma;		// 'modifiable'
    char_u	*b_p_nf;	// 'nrformats'
    صحیح		b_p_pi;		// 'preserveindent'
    char_u	*b_p_qe;	// 'quoteescape'
    صحیح		b_p_ro;		// 'readonly'
    طویل	b_p_sw;		// 'shiftwidth'
    صحیح		b_p_sn;		// 'shortname'
    صحیح		b_p_si;		// 'smartindent'
    طویل	b_p_sts;	// 'softtabstop'
    طویل	b_p_sts_nopaste; // b_p_sts saved for paste mode
    char_u	*b_p_sua;	// 'suffixesadd'
    صحیح		b_p_swf;	// 'swapfile'
#اگر_تعریف_شدہ FEAT_SYN_HL
    طویل	b_p_smc;	// 'synmaxcol'
    char_u	*b_p_syn;	// 'syntax'
#اگر_ختم
    طویل	b_p_ts;		// 'tabstop'
    صحیح		b_p_tx;		// 'textmode'
    طویل	b_p_tw;		// 'textwidth'
    طویل	b_p_tw_nobin;	// b_p_tw saved for binary mode
    طویل	b_p_tw_nopaste;	// b_p_tw saved for paste mode
    طویل	b_p_wm;		// 'wrapmargin'
    طویل	b_p_wm_nobin;	// b_p_wm saved for binary mode
    طویل	b_p_wm_nopaste;	// b_p_wm saved for paste mode
#اگر_تعریف_شدہ FEAT_VARTABS
    char_u	*b_p_vsts;	// 'varsofttabstop'
    صحیح		*b_p_vsts_array;   // 'varsofttabstop' in internal format
    char_u	*b_p_vsts_nopaste; // b_p_vsts saved for paste mode
    char_u	*b_p_vts;	// 'vartabstop'
    صحیح		*b_p_vts_array;	// 'vartabstop' in internal format
#اگر_ختم
#اگر_تعریف_شدہ FEAT_KEYMAP
    char_u	*b_p_keymap;	// 'keymap'
#اگر_ختم

    /*
     * local values for options which are normally global
     */
#اگر_تعریف_شدہ FEAT_QUICKFIX
    char_u	*b_p_gefm;	// 'grepformat' local value
    char_u	*b_p_gp;	// 'grepprg' local value
    char_u	*b_p_mp;	// 'makeprg' local value
    char_u	*b_p_efm;	// 'errorformat' local value
#اگر_ختم
    char_u	*b_p_ep;	// 'equalprg' local value
    char_u	*b_p_راستہ;	// 'path' local value
    صحیح		b_p_ar;		// 'autoread' local value
    char_u	*b_p_لیبل_فہرست;	// 'tags' local value
    char_u	*b_p_tc;	// 'tagcase' local value
    غیرعلامتی	b_tc_flags;     // flags for 'tagcase'
    char_u	*b_p_dict;	// 'dictionary' local value
#اگر_تعریف_شدہ FEAT_DIFF
    char_u	*b_p_dia;	// 'diffanchors' local value
#اگر_ختم
    char_u	*b_p_tsr;	// 'thesaurus' local value
#اگر_تعریف_شدہ FEAT_COMPL_FUNC
    char_u	*b_p_tsrfu;	// 'thesaurusfunc' local value
    callback_T	b_tsrfu_cb;	// 'thesaurusfunc' callback
#اگر_ختم
    طویل	b_p_ul;		// 'undolevels' local value
#اگر_تعریف_شدہ FEAT_PERSISTENT_UNDO
    صحیح		b_p_udf;	// 'undofile'
#اگر_ختم
    char_u	*b_p_lw;	// 'lispwords' local value
#اگر_تعریف_شدہ FEAT_TERMINAL
    طویل	b_p_twsl;	// 'termwinscroll'
#اگر_ختم

    /*
     * end of buffer options
     */

    // values set from b_p_cino
    صحیح		b_ind_level;
    صحیح		b_ind_open_imag;
    صحیح		b_ind_no_brace;
    صحیح		b_ind_پہلا_open;
    صحیح		b_ind_open_extra;
    صحیح		b_ind_close_extra;
    صحیح		b_ind_open_بایاں_imag;
    صحیح		b_ind_jump_label;
    صحیح		b_ind_case;
    صحیح		b_ind_case_رمز;
    صحیح		b_ind_case_break;
    صحیح		b_ind_param;
    صحیح		b_ind_func_type;
    صحیح		b_ind_comment;
    صحیح		b_ind_اندر_comment;
    صحیح		b_ind_اندر_comment2;
    صحیح		b_ind_cpp_baseclass;
    صحیح		b_ind_continuation;
    صحیح		b_ind_unclosed;
    صحیح		b_ind_unclosed2;
    صحیح		b_ind_unclosed_noignore;
    صحیح		b_ind_unclosed_wrapped;
    صحیح		b_ind_unclosed_whiteok;
    صحیح		b_ind_matching_paren;
    صحیح		b_ind_paren_پچھلا;
    صحیح		b_ind_maxparen;
    صحیح		b_ind_maxcomment;
    صحیح		b_ind_scopedecl;
    صحیح		b_ind_scopedecl_رمز;
    صحیح		b_ind_java;
    صحیح		b_ind_js;
    صحیح		b_ind_keep_case_label;
    صحیح		b_ind_hash_comment;
    صحیح		b_ind_cpp_namespace;
    صحیح		b_ind_if_for_while;
    صحیح		b_ind_cpp_extern_c;
    صحیح		b_ind_pragma;

    linenr_T	b_no_eol_lnum;	// non-zero lnum when last line of next binary
				// write should not have an end-of-line

    صحیح		b_آغاز_eof;	// last line had eof (CTRL-Z) when it was read
    صحیح		b_آغاز_eol;	// last line had eol when it was read
    صحیح		b_آغاز_ffc;	// first char of 'ff' when edit started
    char_u	*b_آغاز_fenc;	// 'fileencoding' when edit started or NULL
    صحیح		b_bad_char;	// "++bad=" argument when edit started or 0
    صحیح		b_آغاز_bomb;	// 'bomb' when it was read

#اگر_تعریف_شدہ FEAT_EVAL
    dictitem_T	b_bufvar;	// variable for "b:" Dictionary
    dict_T	*b_vars;	// internal variables, local to buffer

    listener_T	*b_listener;       // Listeners accepting buffered reports.
    listener_T	*b_sync_listener;  // Listeners requiring unbuffered reports.
    list_T	*b_recorded_changes;
    size_t	b_recorded_متن_سائز;  // bytes of text held by the above
#اگر_ختم
#اگر_تعریف_شدہ FEAT_PROP_POPUP
    bool	b_has_textprop;	// true when text props were added
    hashtab_T	*b_proptypes;	// text property types local to buffer
    proptype_T	**b_proparray;	// entries of b_proptypes sorted on tp_id
#اگر_ختم

#اگر defined(FEAT_BEVAL) && defined(FEAT_EVAL)
    char_u	*b_p_bexpr;	// 'balloonexpr' local value
    long_u	b_p_bexpr_flags;// flags for 'balloonexpr'
#اگر_ختم
#اگر_تعریف_شدہ FEAT_CRYPT
    char_u	*b_p_cm;	// 'cryptmethod'
#اگر_ختم

    // When a buffer is created, it starts without a swap file.  b_may_swap is
    // then set to indicate that a swap file may be opened later.  It is reset
    // if a swap file could not be opened.
    bool	b_may_تبادلہ;
    bool	b_did_warn;	// Set to true if user has been warned on first
				// change of a read-only file

    // Two special kinds of buffers:
    // help buffer  - used for help files, won't use a swap file.
    // spell buffer - used for spell info, never displayed and doesn't have a
    //		      file name.
    bool	b_help;		// true for help file buffer (when set b_p_bt
				// is "help")
#اگر_تعریف_شدہ FEAT_SPELL
    bool	b_spell;	// true for a spell file buffer, most fields
				// are not used!  Use the B_SPELL macro to
				// access b_spell without #ifdef.
#اگر_ختم

    bool	b_shortname;	// this file has an 8.3 file name

#اگر_تعریف_شدہ FEAT_کام_CHANNEL
    char_u	*b_prompt_متن;		// set by prompt_setprompt()
    callback_T	b_prompt_callback;	// set by prompt_setcallback()
    callback_T	b_prompt_مداخلت;	// set by prompt_setinterrupt()
    صحیح		b_prompt_داخل_کرو;	// value for restart_edit when entering
					// a prompt buffer window.
#اگر_ختم
#اگر_تعریف_شدہ FEAT_MZSCHEME
    خالی	*b_mzscheme_ref; // The MzScheme reference to this buffer
#اگر_ختم

#اگر_تعریف_شدہ FEAT_PERL
    خالی	*b_perl_نجی;
#اگر_ختم

#اگر_تعریف_شدہ FEAT_PYTHON
    خالی	*b_python_ref;	// The Python reference to this buffer
#اگر_ختم

#اگر_تعریف_شدہ FEAT_PYTHON3
    خالی	*b_python3_ref;	// The Python3 reference to this buffer
#اگر_ختم

#اگر_تعریف_شدہ FEAT_TCL
    خالی	*b_tcl_ref;
#اگر_ختم

#اگر_تعریف_شدہ FEAT_RUBY
    خالی	*b_ruby_ref;
#اگر_ختم

#اگر defined(FEAT_SYN_HL) || defined(FEAT_SPELL)
    synblock_T	b_s;		// Info related to syntax highlighting.  w_s
				// normally points to this, but some windows
				// may use a different synblock_T.
#اگر_ختم

#اگر_تعریف_شدہ FEAT_SIGNS
    sign_اندراج_T *b_signlist;	   // list of placed signs
    sign_اندراج_T *b_sign_finger;   // last sign inserted, used to speed up
				   // inserting signs in ascending line order
# ifdef FEAT_NETBEANS_INTG
    bool	b_has_sign_عمود; // Flag that is set when a first sign is
				   // added and remains set until the end of
				   // the netbeans session.
# endif
#اگر_ختم

#اگر_تعریف_شدہ FEAT_NETBEANS_INTG
    bool	b_netbeans_file;    // true when buffer is owned by NetBeans
    bool	b_was_netbeans_file;// true if b_netbeans_file was once set
#اگر_ختم
#اگر_تعریف_شدہ FEAT_کام_CHANNEL
    bool	b_write_مقصد_channel; // true when appended lines are written to
				    // a channel.
#اگر_ختم

#اگر_تعریف_شدہ FEAT_CRYPT
    cryptstate_T *b_cryptstate;	// Encryption state while reading or writing
				// the file. NULL when not using encryption.
#اگر_ختم
    صحیح		b_mapped_ctrl_c; // modes where CTRL-C is mapped

#اگر_تعریف_شدہ FEAT_TERMINAL
    رکن_T	*b_رکن;	// When not NULL this buffer is for a terminal
				// window.
#اگر_ختم
#اگر_تعریف_شدہ FEAT_DIFF
    bool	b_diff_failed;	// internal diff failed for this buffer
#اگر_ختم
}; // file_buffer


#اگر defined(FEAT_DIFF)
/*
 * Stuff for diff mode.
 */
# define DB_گنتی 8	// up to eight buffers can be diff'ed

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
قسمتعریف ساخت diffblock_S diff_T;
ساخت diffblock_S
{
    diff_T	*df_اگلا;
    linenr_T	df_lnum[DB_گنتی];	// line number in buffer
    linenr_T	df_گنتی[DB_گنتی];	// nr of inserted/changed lines
    صحیح is_linematched;  // has the linematch algorithm ran on this diff hunk to divide it into
			  // smaller diff hunks?

    صحیح		has_changes;		// has cached list of inline changes
    garray_T	df_changes;		// list of inline changes (diffline_change_T)
};

/*
 * Each entry stores a single inline change within a diff block. Line numbers
 * are recorded as relative offsets, and columns are byte offsets, not
 * character counts.
 * Ranges are [start,end), with the end being exclusive.
 */
قسمتعریف ساخت diffline_change_S diffline_change_T;
ساخت diffline_change_S
{
    colnr_T	dc_آغاز[DB_گنتی];	// byte offset of start of range in the line
    colnr_T	dc_انتہا[DB_گنتی];	// 1 past byte offset of end of range in line
    صحیح		dc_آغاز_lnum_off[DB_گنتی];	// starting line offset
    صحیح		dc_انتہا_lnum_off[DB_گنتی];	// end line offset
};

/*
 * Describes a single line's list of inline changes. Use diff_change_parse() to
 * parse this.
 */
قسمتعریف ساخت diffline_S diffline_T;
ساخت diffline_S
{
    diffline_change_T *changes;
    صحیح عدد_changes;
    صحیح bufidx;
    صحیح lineoff;
};
#ورنہ  // FEAT_DIFF
قسمتعریف خالی diffline_T;
قسمتعریف خالی diffline_change_T;
#اگر_ختم

#تعریف_کرو SNAP_HELP_IDX	    0
#تعریف_کرو SNAP_AUCMD_IDX	    1
#تعریف_کرو SNAP_QUICKFIX_IDX   2
#تعریف_کرو SNAP_گنتی	    3

/*
 * Tab pages point to the top frame of each tab page.
 * Note: Most values are NOT valid for the current tab page!  Use "curwin",
 * "firstwin", etc. for that.  "tp_topframe" is always valid and can be
 * compared against "topframe" to find the current tab page.
 */
قسمتعریف ساخت tabpage_S tabpage_T;
ساخت tabpage_S
{
    tabpage_T	    *tp_اگلا;	    // next tabpage or NULL
    چوکھٹا_T	    *tp_topframe;   // topframe for the windows
    win_T	    *tp_curwin;	    // current window in this Tab page
    win_T	    *tp_prevwin;    // previous window in this Tab page
    win_T	    *tp_firstwin;   // first window in this Tab page
    win_T	    *tp_lastwin;    // last window in this Tab page
#اگر_تعریف_شدہ FEAT_PROP_POPUP
    win_T	    *tp_پہلا_popupwin; // first popup window in this Tab page
#اگر_ختم
    طویل	    tp_old_صفیں;    // Rows when Tab page was left
    طویل	    tp_old_Columns; // Columns when Tab page was left, -1 when
				    // calling shell_new_columns() postponed
    صحیح		    tp_old_coloff;  // Column offset when Tab page was left
    طویل	    tp_ch_مستعمل;	    // value of 'cmdheight' when frame size
				    // was set
#اگر_تعریف_شدہ FEAT_GUI
    صحیح		    tp_پچھلا_which_scrollbars[3];
				    // previous value of which_scrollbars
#اگر_ختم
    صحیح		    tp_did_tabclosedpre;  // whether TabClosedPre was triggered

    char_u	    *tp_localdir;	// absolute path of local directory or
					// NULL
    char_u	    *tp_prevdir;	// previous directory

#اگر_تعریف_شدہ FEAT_DIFF
    diff_T	    *tp_پہلا_diff;
    buf_T	    *(tp_diffbuf[DB_گنتی]);
    صحیح		    tp_diff_invalid;	// list of diffs is outdated
    صحیح		    tp_diff_update;	// update diffs before redrawing
#اگر_ختم
    چوکھٹا_T	    *(tp_snapshot[SNAP_گنتی]);  // window layout snapshots
#اگر_تعریف_شدہ FEAT_EVAL
    dictitem_T	    tp_winvar;	    // variable for "t:" Dictionary
    dict_T	    *tp_vars;	    // internal variables, local to tab page
#اگر_ختم

#اگر_تعریف_شدہ FEAT_PYTHON
    خالی	    *tp_python_ref;	// The Python value for this tab page
#اگر_ختم

#اگر_تعریف_شدہ FEAT_PYTHON3
    خالی	    *tp_python3_ref;	// The Python value for this tab page
#اگر_ختم
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
قسمتعریف ساخت w_سطر
{
    linenr_T	wl_lnum;	// buffer line number for logical line
    short_u	wl_سائز;	// height in screen lines
    حرف	wl_valid;	// TRUE values are valid for text in buffer
#اگر_تعریف_شدہ FEAT_FOLDING
    حرف	wl_folded;	// TRUE when this is a range of folded lines
    linenr_T	wl_lastlnum;	// last buffer line number for logical line
#اگر_ختم
} wline_T;

/*
 * Windows are kept in a tree of frames.  Each frame has a column (FR_COL)
 * or row (FR_ROW) layout or is a leaf, which has a window.
 */
ساخت چوکھٹا_S
{
    حرف	fr_layout;	// FR_LEAF, FR_COL or FR_ROW
    صحیح		fr_چوڑائی;
    صحیح		fr_newwidth;	// new width used in win_equal_rec()
    صحیح		fr_اونچائی;
    صحیح		fr_newheight;	// new height used in win_equal_rec()
    چوکھٹا_T	*fr_والد;	// containing frame or NULL
    چوکھٹا_T	*fr_اگلا;	// frame right or below in same parent, NULL
				// for last
    چوکھٹا_T	*fr_پچھلا;	// frame left or above in same parent, NULL
				// for first
    // fr_child and fr_win are mutually exclusive
    چوکھٹا_T	*fr_بچہ;	// first contained frame
    win_T	*fr_win;	// window that fills this frame; for a snapshot
				// set to the current window
};

#تعریف_کرو FR_برگ	0	// frame is a leaf
#تعریف_کرو FR_صف	1	// frame with a row of windows
#تعریف_کرو FR_ستون	2	// frame with a column of windows

/*
 * Struct used for highlighting 'hlsearch' matches, matches defined by
 * ":match" and matches defined by match functions.
 * For 'hlsearch' there is one pattern for all windows.  For ":match" and the
 * match functions there is a different pattern for each window.
 */
قسمتعریف ساخت
{
    regmmatch_T	rm;	    // points to the regexp program; contains last
			    // found match (may continue in next line)
    buf_T	*buf;	    // the buffer to search for a match
    linenr_T	lnum;	    // the line to search for a match
    صحیح		attr;	    // attributes to be used for a match
    صحیح		attr_cur;   // attributes currently active in win_line()
    linenr_T	پہلا_lnum; // first lnum to search for multi-line pat
    colnr_T	startcol;   // in win_line() points to char where HL starts
    colnr_T	endcol;	    // in win_line() points to char where HL ends
    حرف	is_addpos;  // position specified directly by
			    // matchaddpos(). TRUE/FALSE
    حرف	has_cursor; // TRUE if the cursor is inside the match, used for
			    // CurSearch
} مطابقت_T;

/*
 * Same as lpos_T, but with additional field len.
 */
قسمتعریف ساخت
{
    linenr_T	lnum;	// line number
    colnr_T	ستون;	// column number
    صحیح		len;	// length: 0 - to the end of line
} llpos_T;

/*
 * matchitem_T provides a linked list for storing match items for ":match",
 * matchadd() and matchaddpos().
 */
قسمتعریف ساخت matchitem matchitem_T;
ساخت matchitem
{
    matchitem_T	*mit_اگلا;
    صحیح		mit_id;		// match ID
    صحیح		mit_ترجیح;   // match priority

    // Either a pattern is defined (mit_pattern is not NUL) or a list of
    // positions is given (mit_pos is not NULL and mit_pos_count > 0).
    char_u	*mit_نمونہ;   // pattern to highlight
    regmmatch_T	mit_مطابقت;	// regexp program for pattern

    llpos_T	*mit_pos_array;	// array of positions
    صحیح		mit_pos_گنتی;	// nr of entries in mit_pos
    صحیح		mit_pos_cur;	// internal position counter
    linenr_T	mit_toplnum;	// top buffer line
    linenr_T	mit_botlnum;	// bottom buffer line

    مطابقت_T	mit_hl;		// struct for doing the actual highlighting
    صحیح		mit_hlg_id;	// highlight group ID
#اگر_تعریف_شدہ FEAT_CONCEAL
    صحیح		mit_conceal_char; // cchar for Conceal highlighting
#اگر_ختم
};

// Structure to store last cursor position and topline.  Used by check_lnums()
// and reset_lnums().
قسمتعریف ساخت
{
    صحیح		w_topline_save;	// original topline value
    صحیح		w_topline_corr;	// corrected topline value
    pos_T	w_cursor_save;	// original cursor position
    pos_T	w_cursor_corr;	// corrected cursor position
} pos_save_T;

#اگر_تعریف_شدہ FEAT_MENU
قسمتعریف ساخت {
    صحیح		wb_startcol;
    صحیح		wb_endcol;
    vimmenu_T	*wb_menu;
} winbar_شے_T;
#اگر_ختم

/*
 * Characters from the 'listchars' option
 */
قسمتعریف ساخت
{
    صحیح		eol;
    صحیح		ext;
    صحیح		prec;
    صحیح		nbsp;
    صحیح		space;
    صحیح		tab1;
    صحیح		tab2;
    صحیح		tab3;
    صحیح		trail;
    صحیح		lead;
    صحیح		leadtab1;
    صحیح		leadtab2;
    صحیح		leadtab3;
    صحیح		*multispace;
    صحیح		*leadmultispace;
#اگر_تعریف_شدہ FEAT_CONCEAL
    صحیح		conceal;
#اگر_ختم
} lcs_chars_T;

/*
 * Characters from the 'fillchars' option
 */
قسمتعریف ساخت
{
    صحیح	stl;
    صحیح	stlnc;
    صحیح	vert;
    صحیح	fold;
    صحیح	foldopen;
    صحیح	foldclosed;
    صحیح	foldsep;
    صحیح	foldinner;
    صحیح	diff;
    صحیح	eob;
    صحیح	lastline;
#اگر defined(FEAT_TABPANEL)
    صحیح	tpl_vert;
#اگر_ختم
    صحیح trunc;
    صحیح truncrl;
} fill_chars_T;

/*
 * Represents current highlight overrides (used by 'winhighlight' option). The
 * highlight group with ID "from" will be overridden by the highlight group with
 * ID "to"
 */
قسمتعریف ساخت
{
    صحیح from; // If zero or negative then it is hlf_T enum
    صحیح مقصد; // Same thing as "from"
} hl_override_T;

/*
 * Structure which contains all information that belongs to a window
 *
 * All row numbers are relative to the start of the window, except w_winrow.
 */
ساخت کھڑکی_S
{
    صحیح		w_id;		    // unique window ID

    buf_T	*w_بفر;	    // buffer we are a window into

    win_T	*w_پچھلا;	    // link to previous window
    win_T	*w_اگلا;	    // link to next window

#اگر defined(FEAT_SYN_HL) || defined(FEAT_SPELL)
    synblock_T	*w_s;		    // for :ownsyntax
#اگر_ختم

    صحیح		w_locked;	    // don't let autocommands close the window

    چوکھٹا_T	*w_چوکھٹا;	    // frame containing this window

    pos_T	w_cursor;	    // cursor position in buffer

    colnr_T	w_curswant;	    // The column we'd like to be at.  This is
				    // used to try to stay in the same column
				    // for up/down cursor motions.

    bool	w_سیٹ_curswant;	    // If set, then update w_curswant the next
				    // time through cursupdate() to the
				    // current virtual column

#اگر_تعریف_شدہ FEAT_SYN_HL
    linenr_T	w_آخری_cursorline;  // where last time 'cursorline' was drawn
#اگر_ختم

    /*
     * the next seven are used to update the Visual highlighting
     */
    حرف	w_old_visual_منوال;  // last known VIsual_mode
    linenr_T	w_old_cursor_lnum;  // last known end of visual part
    colnr_T	w_old_cursor_fcol;  // first column for block visual part
    colnr_T	w_old_cursor_lcol;  // last column for block visual part
    linenr_T	w_old_visual_lnum;  // last known start of visual part
    colnr_T	w_old_visual_ستون;   // last known start of visual part
    colnr_T	w_old_curswant;	    // last known value of Curswant

    linenr_T    w_آخری_cursor_lnum_rnu;  // cursor lnum when 'rnu' was last
					 // redrawn

    lcs_chars_T	w_lcs_chars;	    // 'listchars' characters
    fill_chars_T w_fill_chars;	    // 'fillchars' characters

    /*
     * "w_topline", "w_leftcol" and "w_skipcol" specify the offsets for
     * displaying the buffer.
     */
    linenr_T	w_topline;	    // buffer line number of the line at the
				    // top of the window
    bool	w_topline_was_سیٹ;  // flag set to true when topline is set,
				    // e.g. by winrestview()

    linenr_T	w_botline;	    // number of the line below the bottom of
				    // the window

#اگر_تعریف_شدہ FEAT_DIFF
    صحیح		w_topfill;	    // number of filler lines above w_topline
    صحیح		w_old_topfill;	    // w_topfill at last redraw
    bool	w_botfill;	    // true when filler lines are actually
				    // below w_topline (at end of file)
    bool	w_old_botfill;	    // w_botfill at last redraw
#اگر_ختم
    colnr_T	w_leftcol;	    // screen column number of the left most
				    // character in the window; used when
				    // 'wrap' is off
    colnr_T	w_skipcol;	    // starting screen column for the first
				    // line in the window; used when 'wrap' is
				    // on; does not include win_col_off()

    صحیح		w_خالی_خانے_صفیں;	    // number of ~ rows in window
#اگر_تعریف_شدہ FEAT_DIFF
    صحیح		w_filler_صفیں;	    // number of filler rows at the end of the
				    //کھڑکی
#اگر_ختم

    // six fields that are only used when there is a WinScrolled autocommand
    linenr_T	w_آخری_topline;	    // last known value for w_topline
#اگر_تعریف_شدہ FEAT_DIFF
    صحیح		w_آخری_topfill;	    // last known value for w_topfill
#اگر_ختم
    colnr_T	w_آخری_leftcol;	    // last known value for w_leftcol
    colnr_T	w_آخری_skipcol;	    // last known value for w_skipcol
    صحیح		w_آخری_چوڑائی;	    // last known value for w_width
    صحیح		w_آخری_اونچائی;	    // last known value for w_height

    /*
     * Layout of the window in the screen.
     * May need to add "msg_scrolled" to "w_winrow" in rare situations.
     */
    صحیح		w_winrow;	    // first row of window in screen
    صحیح		w_اونچائی;	    // number of rows in window, excluding
				    // status/command/winbar line(s)
    صحیح		w_پچھلا_winrow;	    // previous winrow used for 'splitkeep'
    صحیح		w_پچھلا_اونچائی;	    // previous height used for 'splitkeep'
    صحیح		w_stl_rendered_اونچائی; // rendered height of window-local 'stl'
				    // (number of "%@" + 1)
    stl_click_region_T *w_stl_click;  // statusline click regions
    صحیح		w_stl_click_گنتی;  // number of click regions
    صحیح		w_حالت_اونچائی;    // number of status lines.
				    // If 'statuslineopt' was changed, this
				    // member holds the previous value until
				    // frame_change_statusline_height() calls.
    صحیح		w_wincol;	    // Leftmost column of window in screen.
    صحیح		w_چوڑائی;	    // Width of window, excluding separation.
    صحیح		w_vsep_چوڑائی;	    // Number of separator columns (0 or 1).

    pos_save_T	w_save_cursor;	    // backup of cursor pos and topline
    bool	w_do_win_fix_cursor;// if true cursor may be invalid

#اگر_تعریف_شدہ FEAT_PROP_POPUP
    صحیح		w_popup_flags;	    // POPF_ values
    صحیح		w_popup_blend;	    // 0-100: transparency level for popup with opacitys
    صحیح		w_popup_handled;    // POPUP_HANDLE[0-9] flags
    char_u	*w_popup_title;
    poppos_T	w_popup_pos;
    bool	w_popup_fixed;	    // do not shift popup to fit on screen
    صحیح		w_popup_prop_type;  // when not zero: textprop type ID
    win_T	*w_popup_prop_win;  // window to search for textprop
    صحیح		w_popup_prop_id;    // when not zero: textprop ID
    صحیح		w_zindex;
    صحیح		w_minheight;	    // "minheight" for popup window
    صحیح		w_minwidth;	    // "minwidth" for popup window
    صحیح		w_maxheight;	    // "maxheight" for popup window
    صحیح		w_maxwidth;	    // "maxwidth" for popup window
    صحیح		w_maxwidth_opt;	    // maxwidth from option
    صحیح		w_wantline;	    // "line" for popup window
    صحیح		w_wantcol;	    // "col" for popup window
    صحیح		w_firstline;	    // "firstline" for popup window
    صحیح		w_want_scrollbar;   // when zero don't use a scrollbar
    bool	w_has_scrollbar;    // true if scrollbar displayed
    char_u	*w_scrollbar_highlight; // "scrollbarhighlight"
    char_u	*w_thumb_highlight; // "thumbhighlight"
    صحیح		w_popup_padding[4]; // popup padding top/right/bot/left
    صحیح		w_popup_border[4];  // popup border top/right/bot/left
    char_u	*w_border_highlight[4];  // popup border highlight
    bool	w_border_highlight_isset; // borderhighlight was explicitly set
    صحیح		w_border_char[8];   // popup border characters
    صحیح		w_popup_shadow;     // popup shadow (right and bottom edges)

    صحیح		w_popup_leftoff;    // columns left of the screen
    صحیح		w_popup_rightoff;   // columns right of the screen
    صحیح		w_popup_topoff;	    // rows above the host window's top
				    // when "clipwindow" is set
    صحیح		w_popup_bottomoff;  // rows below the host window's bottom
				    // when "clipwindow" is set
    صحیح		w_popup_leftclip;   // columns left of the host window's left
				    // when "clipwindow" is set
    صحیح		w_popup_rightclip;  // columns right of the host window's right
				    // when "clipwindow" is set
    varnumber_T	w_popup_آخری_changedtick; // b:changedtick of popup buffer
					  // when position was computed
    varnumber_T	w_popup_prop_changedtick; // b:changedtick of buffer with
					  // w_popup_prop_type when position
					  // was computed
    صحیح		w_popup_prop_topline; // w_topline of window with
				      // w_popup_prop_type when position was
				      // computed
    صحیح		w_popup_prop_winrow;  // w_winrow of host window when
				      // position was computed
    صحیح		w_popup_prop_wincol;  // w_wincol of host window when
				      // position was computed
    صحیح		w_popup_prop_چوڑائی;   // w_width of host window when
				      // position was computed
    صحیح		w_popup_prop_winheight; // w_height of host window when
				      // position was computed
    linenr_T	w_popup_آخری_curline; // last known w_cursor.lnum of window
				      // with "cursorline" set
    callback_T	w_close_cb;	    // popup close callback
    callback_T	w_filter_cb;	    // popup filter callback
    صحیح		w_filter_errors;    // popup filter error count
    صحیح		w_filter_منوال;	    // mode when filter callback is used

    win_T	*w_popup_curwin;    // close popup if curwin differs
    linenr_T	w_popup_lnum;	    // close popup if cursor not on this line
    colnr_T	w_popup_mincol;	    // close popup if cursor before this col
    colnr_T	w_popup_maxcol;	    // close popup if cursor after this col
    صحیح		w_popup_mouse_صف;  // close popup if mouse moves away
    صحیح		w_popup_mouse_mincol;  // close popup if mouse moves away
    صحیح		w_popup_mouse_maxcol;  // close popup if mouse moves away
    popclose_T	w_popup_close;	    // allow closing the popup with the mouse

    list_T	*w_popup_نقاب;	     // list of lists for "mask"
    char_u	*w_popup_نقاب_cells; // cached mask cells
    صحیح		w_popup_نقاب_اونچائی; // height of w_popup_mask_cells
    صحیح		w_popup_نقاب_چوڑائی;  // width of w_popup_mask_cells

# اگر defined(FEAT_TIMERS)
    timer_T	*w_popup_timer;	    // timer for closing popup window
# endif

    صحیح		w_flags;	    // WFLAG_ flags

# define WFLAG_WCOL_OFF_ADDED	1   // popup border and padding were added to
				    // w_wcol
# define WFLAG_WROW_OFF_ADDED	2   // popup border and padding were added to
				    // w_wrow

# ifdef FEAT_IMAGE
    image_placement_T *w_popup_imagep;
# endif
#اگر_ختم

    /*
     * === start of cached values ====
     */
    /*
     * Recomputing is minimized by storing the result of computations.
     * Use functions in screen.c to check if they are valid and to update.
     * w_valid is a bitfield of flags, which indicate if specific values are
     * valid or need to be recomputed.	See screen.c for values.
     */
    صحیح		w_valid;
    pos_T	w_valid_cursor;	    // last known position of w_cursor, used
				    // to adjust w_valid
    colnr_T	w_valid_leftcol;    // last known w_leftcol
    colnr_T	w_valid_skipcol;    // last known w_skipcol

    /*
     * w_cline_height is the number of physical lines taken by the buffer line
     * that the cursor is on.  We use this to avoid extra calls to plines().
     */
    صحیح		w_cline_اونچائی;	    // current size of cursor line
#اگر_تعریف_شدہ FEAT_FOLDING
    bool	w_cline_folded;	    // cursor line is folded
#اگر_ختم

    صحیح		w_cline_صف;	    // starting row of the cursor line

    colnr_T	w_virtcol;	    // column number of the cursor in the
				    // buffer line, as opposed to the column
				    // number we're at on the screen.  This
				    // makes a difference on lines which span
				    // more than one screen line or when
				    // w_leftcol is non-zero

#اگر_تعریف_شدہ FEAT_PROP_POPUP
    colnr_T	w_virtcol_پہلا_char;	// offset for w_virtcol when there are
					// virtual text properties above the
					//سطر
#اگر_ختم
    /*
     * w_wrow and w_wcol specify the cursor position in the window.
     * This is related to positions in the window, not in the display or
     * buffer, thus w_wrow is relative to w_winrow.
     */
    صحیح		w_wrow, w_wcol;	    // cursor position in window
#اگر_تعریف_شدہ FEAT_CONCEAL
    صحیح		w_wcol_conceal_off; // screen cells concealed before w_wcol on
				    // the cursor's screen line, set by win_line()
#اگر_ختم

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
    صحیح		w_سطریں_valid;	    // number of valid entries
    wline_T	*w_سطریں;

#اگر_تعریف_شدہ FEAT_FOLDING
    garray_T	w_folds;	    // array of nested folds
    bool	w_fold_manual;	    // when true: some folds are opened/closed
				    // manually
    bool	w_foldinvalid;	    // when true: folding needs to be
				    // recomputed
#اگر_ختم
#اگر_تعریف_شدہ FEAT_LINEBREAK
    صحیح		w_nrwidth;	    // width of 'number' and 'relativenumber'
				    // column being used
#اگر_ختم
#اگر_تعریف_شدہ FEAT_TERMINAL
    termcellcolor_T w_رکن_hlfwin;  // cache for term color of HLF_WIN
#اگر_ختم

    /*
     * === end of cached values ===
     */

    صحیح		w_redr_type;	    // type of redraw to be performed on win
    صحیح		w_upd_صفیں;	    // number of window lines to update when
				    // w_redr_type is UPD_REDRAW_TOP
    linenr_T	w_redraw_چوٹی;	    // when != 0: first line needing redraw
    linenr_T	w_redraw_bot;	    // when != 0: last line needing redraw
    bool	w_redr_حالت;	    // if true status line must be redrawn

    // remember what is shown in the ruler for this window (if 'ruler' set)
    pos_T	w_ru_cursor;	    // cursor position shown in ruler
    colnr_T	w_ru_virtcol;	    // virtcol shown in ruler
    linenr_T	w_ru_topline;	    // topline shown in ruler
    linenr_T	w_ru_سطر_گنتی;    // line count used for ruler
#اگر_تعریف_شدہ FEAT_DIFF
    صحیح		w_ru_topfill;	    // topfill shown in ruler
#اگر_ختم
    bool	w_ru_خالی_خانے;	    // true if ruler shows 0-1 (empty line)

    صحیح		w_alt_fnum;	    // alternate file (for # and CTRL-^)

    alist_T	*w_alist;	    // pointer to arglist for this window
    صحیح		w_arg_idx;	    // current index in argument list (can be
				    // out of range!)
    bool	w_arg_idx_invalid;  // editing another file than w_arg_idx

    char_u	*w_localdir;	    // absolute path of local directory or
				    // NULL
    char_u	*w_prevdir;	    // previous directory
#اگر_تعریف_شدہ FEAT_MENU
    vimmenu_T	*w_winbar;	    // The root of the WinBar menu hierarchy.
    winbar_شے_T *w_winbar_اشیاء;  // list of items in the WinBar
    صحیح		w_winbar_اونچائی;    // 1 if there is a window toolbar
#اگر_ختم

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
#تعریف_کرو GLOBAL_WO(p)	((حرف *)(p) + حجم(winopt_T))

#اگر defined(FEAT_SIGNS) || defined(FEAT_FOLDING) || defined(FEAT_DIFF)
    صحیح		*w_p_cc_ستون_تعداد;	    // array of columns to highlight or NULL
    char_u	w_p_culopt_flags;   // flags for cursorline highlighting
#اگر_ختم

#اگر_تعریف_شدہ FEAT_LINEBREAK
    صحیح		w_briopt_min;	    // minimum width for breakindent
    صحیح		w_briopt_سرکاؤ;	    // additional shift for breakindent
    صحیح		w_briopt_sbr;	    // sbr in 'briopt'
    صحیح		w_briopt_list;      // additional indent for lists
    صحیح		w_briopt_vcol;	    // indent for specific column
#اگر_ختم

    طویل	w_scbind_pos;

#اگر_تعریف_شدہ FEAT_EVAL
    dictitem_T	w_winvar;	// variable for "w:" Dictionary
    dict_T	*w_vars;	// internal variables, local to window
#اگر_ختم

    /*
     * The w_prev_pcmark field is used to check whether we really did jump to
     * a new line after setting the w_pcmark.  If not, then we revert to
     * using the previous w_pcmark.
     */
    pos_T	w_pcmark;	// previous context mark
    pos_T	w_پچھلا_pcmark;	// previous w_pcmark

    /*
     * the jumplist contains old cursor positions
     */
    xfmark_T	w_jumplist[JUMPLISTSIZE];
    صحیح		w_jumplistlen;		// number of active entries
    صحیح		w_jumplistidx;		// current position

    صحیح		w_changelistidx;	// current position in b_changelist

#اگر_تعریف_شدہ FEAT_تلاش_EXTRA
    matchitem_T	*w_مطابقت_سر;		// head of match list
    صحیح		w_اگلا_مطابقت_id;	// next match ID
#اگر_ختم

    /*
     * the tagstack grows from 0 upwards:
     * entry 0: older
     * entry 1: newer
     * entry 2: newest
     */
    taggy_T	w_tagstack[TAGSTACKSIZE];   // the tag stack
    صحیح		w_tagstackidx;		    // idx just below active entry
    صحیح		w_tagstacklen;		    // number of tags on stack

    /*
     * w_fraction is the fractional row of the cursor within the window, from
     * 0 at the top row to FRACTION_MULT at the last row.
     * w_prev_fraction_row was the actual cursor row when w_fraction was last
     * calculated.
     */
    صحیح		w_fraction;
    صحیح		w_پچھلا_fraction_صف;

#اگر_تعریف_شدہ FEAT_GUI
    scrollbar_T	w_scrollbars[2];	// vert. Scrollbars for this window
#اگر_ختم
#اگر_تعریف_شدہ FEAT_LINEBREAK
    linenr_T	w_nrwidth_سطر_گنتی;	// line count when ml_nrwidth_width
					// was computed.
    طویل	w_nuw_cached;		// 'numberwidth' option cached
    صحیح		w_nrwidth_چوڑائی;	// nr of chars to print line count.
#اگر_ختم

#اگر_تعریف_شدہ FEAT_QUICKFIX
    qf_info_T	*w_llist;		// Location list for this window
    /*
     * Location list reference used in the location list window.
     * In a non-location list window, w_llist_ref is NULL.
     */
    qf_info_T	*w_llist_ref;
#اگر_ختم

#اگر_تعریف_شدہ FEAT_MZSCHEME
    خالی	*w_mzscheme_ref;	// The MzScheme value for this window
#اگر_ختم

#اگر_تعریف_شدہ FEAT_PERL
    خالی	*w_perl_نجی;
#اگر_ختم

#اگر_تعریف_شدہ FEAT_PYTHON
    خالی	*w_python_ref;		// The Python value for this window
#اگر_ختم

#اگر_تعریف_شدہ FEAT_PYTHON3
    خالی	*w_python3_ref;		// The Python value for this window
#اگر_ختم

#اگر_تعریف_شدہ FEAT_TCL
    خالی	*w_tcl_ref;
#اگر_ختم

#اگر_تعریف_شدہ FEAT_RUBY
    خالی	*w_ruby_ref;
#اگر_ختم

    hl_override_T *w_hl;
    صحیح		w_hl_len;
    صحیح		w_hlfwin_id; // Cached HLF_WIN highlight group id, zero if none,
			     // or -1 if it was set to itself.
};

/*
 * Arguments for operators.
 */
قسمتعریف ساخت oparg_S
{
    صحیح		op_type;	// current pending operator type
    صحیح		regname;	// register to use for the operator
    صحیح		motion_type;	// type of the current cursor motion
    صحیح		motion_force;	// force motion type: 'v', 'V' or CTRL-V
    صحیح		use_reg_one;	// TRUE if delete uses reg 1 even when not
				// linewise
    صحیح		inclusive;	// TRUE if char motion is inclusive (only
				// valid when motion_type is MCHAR)
    صحیح		انتہا_adjusted;	// backuped b_op_end one char (only used by
				// do_format())
    pos_T	آغاز;		// start of the operator
    pos_T	انتہا;		// end of the operator
    pos_T	cursor_آغاز;	// cursor position before motion for "gw"

    طویل	سطر_گنتی;	// number of lines from op_start to op_end
				// (inclusive)
    صحیح		خالی_خانے_2;		// op_start and op_end the same (only used by
				// do_change())
    صحیح		is_VIsual;	// operator on Visual area
    صحیح		بلاک_منوال;	// current operator is Visual block mode
    colnr_T	آغاز_vcol;	// start col for block mode operator
    colnr_T	انتہا_vcol;	// end col for block mode operator
    طویل	پچھلا_opcount;	// ca.opcount saved for K_CURSORHOLD
    طویل	پچھلا_count0;	// ca.count0 saved for K_CURSORHOLD
    صحیح		excl_tr_ws;	// exclude trailing whitespace for yank of a
				//بلاک
} oparg_T;

/*
 * Arguments for Normal mode commands.
 */
قسمتعریف ساخت cmdarg_S
{
    oparg_T	*oap;		// Operator arguments
    صحیح		prechar;	// prefix character (optional, always 'g')
    صحیح		cmdchar;	// command character
    صحیح		nchar;		// next command character (optional)
    صحیح		ncharC1;	// first composing character (optional)
    صحیح		ncharC2;	// second composing character (optional)
    صحیح		extra_char;	// yet another character (optional)
    طویل	opcount;	// count before an operator
    طویل	count0;		// count before command, default 0
    طویل	count1;		// count before command, default 1
    صحیح		arg;		// extra argument from nv_cmds[]
    صحیح		retval;		// return: CA_* values
    char_u	*searchbuf;	// return: pointer to search pattern or NULL
} cmdarg_T;

// values for retval:
#تعریف_کرو CA_COMMAND_BUSY	    1	// skip restarting edit() once
#تعریف_کرو CA_NO_ADJ_OP_انتہا    2	// don't adjust operator end

#اگر_تعریف_شدہ CURSOR_SHAPE
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
# define SHAPE_IDX_حالت 10	// A status line
# define SHAPE_IDX_SDRAG 11	// dragging a status line
# define SHAPE_IDX_VSEP	12	// A vertical separator line
# define SHAPE_IDX_VDRAG 13	// dragging a vertical separator line
# define SHAPE_IDX_مزید	14	// Hit-return or More
# define SHAPE_IDX_MOREL	15	// Hit-return or More in last line
# define SHAPE_IDX_SM	16	// showing matching paren
# define SHAPE_IDX_گنتی	17

# define SHAPE_بلاک	0	// block cursor
# define SHAPE_HOR	1	// horizontal bar cursor
# define SHAPE_VER	2	// vertical bar cursor

# define MSHAPE_NUMBERED	1000	// offset for shapes identified by number
# define MSHAPE_HIDE	1	// hide mouse pointer

# define SHAPE_MOUSE	1	// used for mouse pointer shape
# define SHAPE_CURSOR	2	// used for text cursor shape

قسمتعریف ساخت cursor_اندراج
{
    صحیح		shape;		// one of the SHAPE_ defines
    صحیح		mshape;		// one of the MSHAPE defines
    صحیح		percentage;	// percentage of cell for bar
    طویل	blinkwait;	// blinking, wait time before blinking starts
    طویل	blinkon;	// blinking, on time
    طویل	blinkoff;	// blinking, off time
    صحیح		id;		// highlight group ID
    صحیح		id_lm;		// highlight group ID for :lmap mode
    حرف	*name;		// mode name (fixed)
    حرف	مستعمل_for;	// SHAPE_MOUSE and/or SHAPE_CURSOR
} cursorentry_T;
#اگر_ختم // CURSOR_SHAPE

#اگر_تعریف_شدہ FEAT_MENU

// Indices into vimmenu_T->strings[] and vimmenu_T->noremap[] for each mode
# define MENU_اشاریہ_INVALID	-1
# define MENU_اشاریہ_معمول	0
# define MENU_اشاریہ_VISUAL	1
# define MENU_اشاریہ_SELECT	2
# define MENU_اشاریہ_OP_PENDING	3
# define MENU_اشاریہ_داخل_کرو	4
# define MENU_اشاریہ_CMDLINE	5
# define MENU_اشاریہ_TERMINAL	6
# define MENU_اشاریہ_TIP		7
# define MENU_MODES		8

// Menu modes
# define MENU_معمول_منوال	(1 << MENU_اشاریہ_معمول)
# define MENU_VISUAL_منوال	(1 << MENU_اشاریہ_VISUAL)
# define MENU_SELECT_منوال	(1 << MENU_اشاریہ_SELECT)
# define MENU_OP_PENDING_منوال	(1 << MENU_اشاریہ_OP_PENDING)
# define MENU_داخل_کرو_منوال	(1 << MENU_اشاریہ_داخل_کرو)
# define MENU_CMDLINE_منوال	(1 << MENU_اشاریہ_CMDLINE)
# define MENU_TERMINAL_منوال	(1 << MENU_اشاریہ_TERMINAL)
# define MENU_TIP_منوال		(1 << MENU_اشاریہ_TIP)
# define MENU_ALL_MODES		((1 << MENU_اشاریہ_TIP) - 1)
// note MENU_INDEX_TIP is not a 'real' mode

// Start a menu name with this to not include it on the main menu bar
# define MNU_HIDDEN_CHAR		']'

ساخت VimMenu
{
    صحیح		modes;		    // Which modes is this menu visible for?
    صحیح		enabled;	    // for which modes the menu is enabled
    char_u	*name;		    // Name of menu, possibly translated
    char_u	*dname;		    // Displayed Name ("name" without '&')
# ifdef FEAT_MULTI_LANG
    char_u	*en_name;	    // "name" untranslated, NULL when "name"
				    // was not translated
    char_u	*en_dname;	    // "dname" untranslated, NULL when "dname"
				    // was not translated
# endif
    char_u	*actext;	    // accelerator text (after TAB)
    صحیح		mnemonic;	    // mnemonic key (after '&')
    صحیح		ترجیح;	    // Menu order priority
# ifdef FEAT_GUI
    خالی	(*cb)(vimmenu_T *); // Call-back function
# endif
# ifdef FEAT_TOOLBAR
    char_u	*iconfile;	    // name of file for icon or NULL
    صحیح		iconidx;	    // icon index (-1 if not set)
    صحیح		icon_builtin;	    // icon names is BuiltIn{nr}
# endif
    char_u	*لڑیاں[MENU_MODES]; // Mapped string for each mode
    صحیح		noremap[MENU_MODES]; // A REMAP_ flag for each mode
    حرف	silent[MENU_MODES]; // A silent flag for each mode
    vimmenu_T	*children;	    // Children of sub-menu
    vimmenu_T	*والد;	    // Parent of menu
    vimmenu_T	*اگلا;		    // Next item in menu
# ifdef FEAT_GUI_X11
    Widget	id;		    // Manage this to enable item
    Widget	submenu_id;	    // If this is submenu, add children here
# endif
# ifdef FEAT_GUI_GTK
    GtkWidget	*id;		    // Manage this to enable item
    GtkWidget	*submenu_id;	    // If this is submenu, add children here
#  اگر defined(GTK_CHECK_نسخہ) && !GTK_CHECK_نسخہ(3,4,0)
    GtkWidget	*tearoff_handle;
#  endif
#  ifndef USE_GTK4
    GtkWidget   *label;		    // Used by "set wak=" code.
#  endif
# endif
# ifdef FEAT_GUI_MOTIF
    صحیح		sensitive;	    // turn button on/off
    حرف	**xpm;		    // pixmap data
    حرف	*xpm_fname;	    // file with pixmap data
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
    BMenu_شے  *id;		    // Id of menu item
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
#ورنہ
// For generating prototypes when FEAT_MENU isn't defined.
قسمتعریف صحیح vimmenu_T;

#اگر_ختم // FEAT_MENU

/*
 * Struct to save values in before executing autocommands for a buffer that is
 * not the current buffer.
 */
قسمتعریف ساخت
{
    صحیح		use_aucmd_win_idx;  // index in aucmd_win[] if >= 0
    صحیح		save_curwin_id;	    // ID of saved curwin
    صحیح		new_curwin_id;	    // ID of new curwin
    صحیح		save_prevwin_id;    // ID of saved prevwin
    bufref_T	new_curbuf;	    // new curbuf
    char_u	*tp_localdir;	    // saved value of tp_localdir
    char_u	*globaldir;	    // saved value of globaldir
    صحیح		save_VIsual_فعال; // saved VIsual_active
#اگر_تعریف_شدہ FEAT_کام_CHANNEL
    صحیح		save_prompt_داخل_کرو; // saved b_prompt_insert
#اگر_ختم
} aco_save_T;

/*
 * Generic option table item, only used for printer at the moment.
 */
قسمتعریف ساخت
{
    ثابت حرف	*name;
    صحیح		hasnum;
    طویل	نمبر;
    char_u	*string;	// points into option string
    صحیح		لڑی_لمبائی;
    صحیح		present;
} option_جدول_T;

/*
 * Structure to hold printing color and font attributes.
 */
قسمتعریف ساخت
{
    long_u	fg_color;
    long_u	bg_color;
    صحیح		bold;
    صحیح		italic;
    صحیح		underline;
    صحیح		undercurl;
} prt_متن_attr_T;

/*
 * Structure passed back to the generic printer code.
 */
قسمتعریف ساخت
{
    صحیح		n_collated_copies;
    صحیح		n_uncollated_copies;
    صحیح		duplex;
    صحیح		chars_فی_سطر;
    صحیح		سطریں_فی_صفحہ;
    صحیح		has_color;
    prt_متن_attr_T نمبر;
#اگر_تعریف_شدہ FEAT_SYN_HL
    صحیح		modec;
    صحیح		do_syntax;
#اگر_ختم
    صحیح		user_abort;
#اگر_تعریف_شدہ FEAT_PRINT_PANGO
    صحیح		user_abort_پیغام;
#اگر_ختم
    char_u	*jobname;
#اگر defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
    char_u	*outfile;
    char_u	*arguments;
#اگر_ختم
} prt_settings_T;

#تعریف_کرو PRINT_نمبر_چوڑائی 8

/*
 * Used for popup menu items.
 */
قسمتعریف ساخت
{
    char_u	*pum_متن;		// main menu text
    char_u	*pum_قسم;		// extra kind text (may be truncated)
    char_u	*pum_extra;		// extra menu text (may be truncated)
    char_u	*pum_info;		// extra info
    صحیح		pum_cpt_ماخذ_idx;	// index of completion source in 'cpt'
    صحیح		pum_user_abbr_hlattr;	// highlight attribute for abbr
    صحیح		pum_user_قسم_hlattr;	// highlight attribute for kind
} pumitem_T;

/*
 * Structure used for get_tagfname().
 */
قسمتعریف ساخت
{
    char_u	*tn_لیبل_فہرست;	// value of 'tags' when starting
    char_u	*tn_np;		// current position in tn_tags
    صحیح		tn_did_filefind_init;
    صحیح		tn_hf_idx;
    خالی	*tn_تلاش_ctx;
} tagname_T;

قسمتعریف ساخت {
    UINT32_T کل[2];
    UINT32_T state[8];
    char_u   بفر[64];
} context_sha256_T;

/*
 * types for expressions.
 */
قسمتعریف شمار
{
    اظہاریہ_UNKNOWN = 0,
    اظہاریہ_EQUAL,		// ==
    اظہاریہ_NEQUAL,	// !=
    اظہاریہ_GREATER,	// >
    اظہاریہ_GEQUAL,	// >=
    اظہاریہ_SMALLER,	// <
    اظہاریہ_SEQUAL,	// <=
    اظہاریہ_مطابقت,		// =~
    اظہاریہ_NOMATCH,	// !~
    اظہاریہ_IS,		// is
    اظہاریہ_ISNOT,		// isnot
    // used with ISN_OPNR
    اظہاریہ_جوڑو,		// +
    اظہاریہ_SUB,		// -
    اظہاریہ_MULT,		// *
    اظہاریہ_DIV,		// /
    اظہاریہ_REM,		// %
    اظہاریہ_LSHIFT,	// <<
    اظہاریہ_RSHIFT,	// >>
    // used with ISN_ADDLIST
    اظہاریہ_COPY,		// create new list
    اظہاریہ_APPEND,	// append to first list
} exprtype_T;

/*
 * Structure used for reading in json_decode().
 */
ساخت js_reader
{
    char_u	*js_buf;	// text to be decoded
    char_u	*js_انتہا;	// NUL in js_buf
    صحیح		js_مستعمل;	// bytes used from js_buf
    صحیح		(*js_fill)(ساخت js_reader *);
				// function to fill the buffer or NULL;
				// return TRUE when the buffer was filled
    خالی	*js_cookie;	// can be used by js_fill
    صحیح		js_cookie_arg;	// can be used by js_fill
};
قسمتعریف ساخت js_reader js_read_T;

// Maximum number of commands from + or -c arguments.
#تعریف_کرو MAX_ARG_CMDS 10

// values for "window_layout"
#تعریف_کرو WIN_HOR	    1	    // "-o" horizontally split windows
#تعریف_کرو	WIN_VER	    2	    // "-O" vertically split windows
#تعریف_کرو	WIN_TABS    3	    // "-p" windows on tab pages

// Struct for various parameters passed between main() and other functions.
قسمتعریف ساخت
{
    صحیح		argc;
    حرف	**argv;

    char_u	*fname;			// first file to edit

    صحیح		evim_منوال;		// started as "evim"
    char_u	*use_vimrc;		// vimrc from -u argument
    صحیح		clean;			// --clean argument

    صحیح		n_commands;		     // no. of commands from + or -c
    char_u	*commands[MAX_ARG_CMDS];     // commands from + or -c arg.
    char_u	cmds_tofree[MAX_ARG_CMDS];   // commands that need free()
    صحیح		n_pre_commands;		     // no. of commands from --cmd
    char_u	*pre_commands[MAX_ARG_CMDS]; // commands from --cmd argument

    صحیح		edit_type;		// type of editing to do
    char_u	*tagname;		// tag from -t argument
#اگر_تعریف_شدہ FEAT_QUICKFIX
    char_u	*use_ef;		// 'errorfile' from -q argument
#اگر_ختم

    صحیح		want_بھرا_screen;
    صحیح		not_a_رکن;		// no warning for missing term?
#اگر_تعریف_شدہ FEAT_GUI
    char_u	*gui_dialog_file;	// file to write dialog text in
#اگر_ختم
    صحیح		tty_fail;		// exit if not a tty
    char_u	*رکن;			// specified terminal name
#اگر_تعریف_شدہ FEAT_CRYPT
    صحیح		ask_for_کلید;		// -x argument
#اگر_ختم
    صحیح		no_تبادلہ_file;		// "-n" argument used
#اگر_تعریف_شدہ FEAT_EVAL
    صحیح		use_debug_break_level;
#اگر_ختم
    صحیح		کھڑکی_گنتی;		// number of windows to use
    صحیح		کھڑکی_layout;		// 0, WIN_HOR, WIN_VER or WIN_TABS

#اگر_تعریف_شدہ FEAT_CLIENTSERVER
    صحیح		serverArg;		// TRUE when argument for a server
    char_u	*serverName_arg;	// cmdline arg for server name
    char_u	*serverStr;		// remote server command
    char_u	*serverStrEnc;		// encoding of serverStr
    char_u	*servername;		// allocated name for our server
#اگر_ختم
#اگر !defined(UNIX)
# define EXPAND_FILENAMES
    صحیح		literal;		// don't expand file names
#اگر_ختم
#اگر_تعریف_شدہ MSWIN
    صحیح		بھرا_راستہ;		// file name argument was full path
#اگر_ختم
#اگر_تعریف_شدہ FEAT_DIFF
    صحیح		diff_منوال;		// start with 'diff' set
#اگر_ختم
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
قسمتعریف ساخت lval_S
{
    char_u	*ll_name;	// start of variable name (can be NULL)
    char_u	*ll_name_انتہا;	// end of variable name (can be NULL)
    type_T	*ll_type;	// type of variable (can be NULL)
    char_u	*ll_exp_name;	// NULL or expanded name in allocated memory.

    scid_T	ll_sid;		// for an imported item: the script ID it was
				// imported from; zero otherwise

    typval_T	*ll_tv;		// Typeval of item being used.  If "newkey"
				// isn't NULL it's the Dict to which to add
				// the item.
    listitem_T	*ll_li;		// The list item or NULL.
    list_T	*ll_list;	// The list or NULL.
    صحیح		ll_range;	// TRUE when a [i:j] range was used
    صحیح		ll_empty2;	// Second index is empty: [i:]
    طویل	ll_n1;		// First index for list
    طویل	ll_n2;		// Second index for list range
    dict_T	*ll_dict;	// The Dictionary or NULL
    dictitem_T	*ll_di;		// The dictitem or NULL
    char_u	*ll_newkey;	// New key for Dict in alloc. mem or NULL.
    type_T	*ll_valtype;	// type expected for the value or NULL
    blob_T	*ll_blob;	// The Blob or NULL
    tuple_T	*ll_tuple;	// tuple or NULL
    ufunc_T	*ll_ufunc;	// The function or NULL
    object_T	*ll_object;	// The object or NULL, class is not NULL
    class_T	*ll_class;	// The class or NULL, object may be NULL
    صحیح		ll_oi;		// The object/class member index
    صحیح		ll_is_جڑ;	// TRUE if ll_tv is the lval_root, like a
				// plain object/class. ll_tv is variable.
    garray_T	ll_type_list;   // list of pointers to allocated types
} lval_T;

/**
 * This specifies optional parameters for get_lval(). Arguments may be NULL.
 */
قسمتعریف ساخت lval_جڑ_S {
    typval_T	*lr_tv;		// Base typval.
    class_T	*lr_cl_exec;	// Executing class for access checking.
    صحیح		lr_is_arg;	// name is an arg (not a member).
} lval_جڑ_T;

// Structure used to save the current state.  Used when executing Normal mode
// commands while in any other mode.
قسمتعریف ساخت {
    صحیح		save_پیغام_scroll;
    صحیح		save_restart_edit;
    صحیح		save_پیغام_didout;
    صحیح		save_State;
    صحیح		save_insertmode;
    صحیح		save_اختتام_op;
    صحیح		save_opcount;
    صحیح		save_reg_executing;
    صحیح		save_pending_انتہا_reg_executing;
    صحیح		save_script_نسخہ;
    tasave_T	tabuf;
} save_state_T;

قسمتعریف ساخت {
    varnumber_T vv_prevcount;
    varnumber_T vv_گنتی;
    varnumber_T vv_count1;
} vimvars_save_T;

// Scope for changing directory
قسمتعریف شمار {
    CDSCOPE_GLOBAL,	// :cd
    CDSCOPE_TABPAGE,	// :tcd
    CDSCOPE_کھڑکی	// :lcd
} cdscope_T;

// Variable flavor
قسمتعریف شمار
{
    VAR_FLAVOUR_DEFAULT,	// doesn't start with uppercase
    VAR_FLAVOUR_SESSION,	// starts with uppercase, some lower
    VAR_FLAVOUR_VIMINFO		// all uppercase
} var_flavour_T;

// argument for mouse_find_win()
قسمتعریف شمار {
    IGNORE_POPUP,	// only check non-popup windows
    ڈھونڈو_POPUP,		// also find popup windows
    FAIL_POPUP		// return NULL if mouse on popup window
} mouse_ڈھونڈو_T;

// Symbolic names for some registers.
#تعریف_کرو DELETION_رجسٹر	36
#اگر defined(FEAT_CLIPBOARD) || defined(HAVE_CLIPMETHOD)
# define ستارہ_رجسٹر		37
# اگر defined(FEAT_X11) || defined(FEAT_WAYLAND)
#  define PLUS_رجسٹر	38
#  define REAL_PLUS_رجسٹر	PLUS_رجسٹر
# ورنہ
#  define PLUS_رجسٹر	ستارہ_رجسٹر	    // there is only one
#  ifdef FEAT_EVAL
// Make it so that if clipmethod is "none", the plus register is not available,
// but if clipmethod is a provider, then expose the plus register for use.
#   define REAL_PLUS_رجسٹر	38
#  ورنہ
#   define REAL_PLUS_رجسٹر	ستارہ_رجسٹر
#  endif
# endif
#اگر_ختم
#اگر_تعریف_شدہ FEAT_DND
# define TILDE_رجسٹر		(PLUS_رجسٹر + 1)
#اگر_ختم

#اگر_تعریف_شدہ FEAT_CLIPBOARD
# ifdef FEAT_DND
#  define عدد_REGISTERS		(TILDE_رجسٹر + 1)
# ورنہ
#  define عدد_REGISTERS		(REAL_PLUS_رجسٹر + 1)
# endif
#ورنہ
# ifdef HAVE_CLIPMETHOD
#  define عدد_REGISTERS		(REAL_PLUS_رجسٹر + 1)
# ورنہ
#  define عدد_REGISTERS		37
# endif
#اگر_ختم

// structure used by block_prep, op_delete and op_yank for blockwise operators
// also op_change, op_shift, op_insert, op_replace - AKelly
ساخت بلاک_def
{
    صحیح		startspaces;	// 'extra' cols before first char
    صحیح		endspaces;	// 'extra' cols after last char
    صحیح		textlen;	// chars in block
    char_u	*textstart;	// pointer to 1st char (partially) in block
    colnr_T	textcol;	// index of chars (partially) in block
    colnr_T	آغاز_vcol;	// start col of 1st char wholly inside block
    colnr_T	انتہا_vcol;	// start col of 1st char wholly after block
    صحیح		is_short;	// TRUE if line is too short to fit in block
    صحیح		is_MAX;		// TRUE if curswant==MAXCOL when starting
    صحیح		is_oneChar;	// TRUE if block within one character
    صحیح		pre_whitesp;	// screen cols of ws before block
    صحیح		pre_whitesp_c;	// chars of ws before block
    colnr_T	انتہا_char_vcols;	// number of vcols of post-block char
    colnr_T	آغاز_char_vcols; // number of vcols of pre-block char
};

// Each yank register has an array of pointers to lines.
قسمتعریف ساخت
{
    string_T	*y_array;	// pointer to array of string_T structs
    linenr_T	y_سائز;		// number of lines in y_array
    char_u	y_type;		// MLINE, MCHAR or MBLOCK
    colnr_T	y_چوڑائی;	// only set if y_type == MBLOCK
#اگر_تعریف_شدہ FEAT_VIMINFO
    time_t	y_وقت_سیٹ;
#اگر_ختم
} yankreg_T;

// The offset for a search command is store in a soff struct
// Note: only spats[0].off is really used
قسمتعریف ساخت soffset
{
    صحیح		dir;		// search direction, '/' or '?'
    صحیح		سطر;		// search has line offset
    صحیح		انتہا;		// search set cursor at end
    طویل	off;		// line or char offset
} soffset_T;

// A search pattern and its attributes are stored in a spat struct
قسمتعریف ساخت spat
{
    char_u	    *pat;	// the pattern (in allocated memory) or NULL
    size_t	    patlen;	// the length of the pattern (0 if pat is NULL)
    صحیح		    magic;	// magicness of the pattern
    صحیح		    no_scs;	// no smartcase for this pattern
    soffset_T	    off;
} spat_T;

/*
 * Optional extra arguments for searchit().
 */
قسمتعریف ساخت
{
    linenr_T	sa_stop_lnum;	// stop after this line number when != 0
#اگر_تعریف_شدہ FEAT_RELTIME
    طویل	sa_tm;		// timeout limit or zero
    صحیح		sa_timed_برآمد;	// set when timed out
#اگر_ختم
    صحیح		sa_wrapped;	// search wrapped around
} searchit_arg_T;

/*
 * Cookie used by getsourceline().
 */
/*
 * Cookie used to store info for each sourced file.
 * It is shared between do_source() and getsourceline().
 * This is passed to do_cmdline().
 */
قسمتعریف ساخت {
    FILE	*fp;		// opened file for sourcing
    char_u	*nextline;	// if not NULL: line that was read ahead
    linenr_T	sourcing_lnum;	// line number of the source file
    صحیح		ختم_شدہ;	// ":finish" used
    صحیح		ماخذ_from_buf;// TRUE if sourcing from current buffer
    صحیح		buf_lnum;	// line number in the current buffer
    garray_T	buflines;	// lines in the current buffer
#اگر_تعریف_شدہ USE_CRNL
    صحیح		fileformat;	// EOL_UNKNOWN, EOL_UNIX or EOL_DOS
    صحیح		error;		// TRUE if LF found after CR-LF
#اگر_ختم
#اگر_تعریف_شدہ FEAT_EVAL
    linenr_T	breakpoint;	// next line with breakpoint or zero
    char_u	*fname;		// name of sourced file
    صحیح		dbg_tick;	// debug_tick when breakpoint was set
    صحیح		level;		// top nesting level of sourced file
#اگر_ختم
    vimconv_T	conv;		// type of conversion
} ماخذ_cookie_T;


#تعریف_کرو WRITEBUFSIZE	8192	// size of normal write buffer

#تعریف_کرو FIO_LATIN1	0x01	// convert Latin1
#تعریف_کرو FIO_UTF8	0x02	// convert UTF-8
#تعریف_کرو FIO_UCS2	0x04	// convert UCS-2
#تعریف_کرو FIO_UCS4	0x08	// convert UCS-4
#تعریف_کرو FIO_UTF16	0x10	// convert UTF-16
#اگر_تعریف_شدہ MSWIN
# define FIO_CODEPAGE	0x20	// convert MS-Windows codepage
# define FIO_PUT_CP(x) (((x) & 0xffff) << 16)	// put codepage in top word
# define FIO_GET_CP(x)	(((x)>>16) & 0xffff)	// get codepage from top word
#اگر_ختم
#اگر_تعریف_شدہ MACOS_CONVERT
# define FIO_MACROMAN	0x20	// convert MacRoman
#اگر_ختم
#تعریف_کرو FIO_ENDIAN_L	0x80	// little endian
#تعریف_کرو FIO_ENCRYPTED	0x1000	// encrypt written bytes
#تعریف_کرو FIO_NOCONVERT	0x2000	// skip encoding conversion
#تعریف_کرو FIO_UCSBOM	0x4000	// check for BOM at start of file
#تعریف_کرو FIO_ALL	(-1)	// allow all formats

// When converting, a read() or write() may leave some bytes to be converted
// for the next call.  The value is guessed...
#تعریف_کرو CONV_RESTLEN 30

// We have to guess how much a sequence of bytes may expand when converting
// with iconv() to be able to allocate a buffer.
#تعریف_کرو ICONV_MULT 8

// Used for "magic_overruled".
قسمتعریف شمار {
    OPTION_MAGIC_NOT_سیٹ,	// p_magic not overruled
    OPTION_MAGIC_ON,		// magic on inside regexp
    OPTION_MAGIC_OFF		// magic off inside regexp
} optmagic_T;

// Magicness of a pattern, used by regexp code.
// The order and values matter:
//  magic <= MAGIC_OFF includes MAGIC_NONE
//  magic >= MAGIC_ON  includes MAGIC_ALL
قسمتعریف شمار {
    MAGIC_NONE = 1,		// "\V" very unmagic
    MAGIC_OFF = 2,		// "\M" or 'magic' off
    MAGIC_ON = 3,		// "\m" or 'magic'
    MAGIC_ALL = 4		// "\v" very magic
} magic_T;

قسمتعریف شمار {
    WT_UNKNOWN = 0,	// Unknown or unspecified location
    WT_دلیل,
    WT_VARIABLE,
    WT_MEMBER,
    WT_طریقہ,		// object method
    WT_طریقہ_ARG,	// object method argument type
    WT_طریقہ_RETURN,	// object method return type
    WT_CAST,		// type cast
} wherekind_T;

// Struct used to pass the location of a type check.  Used in error messages to
// indicate where the error happened.  Also used for doing covariance type
// check for object method return type and contra-variance type check for
// object method arguments.
قسمتعریف ساخت {
    حرف	*wt_func_name;  // function name or NULL
    حرف	wt_اشاریہ;	// argument or variable index, 0 means unknown
    wherekind_T	wt_قسم;	// type check location
} where_T;

#تعریف_کرو WHERE_INIT {تہی, 0, WT_UNKNOWN}

// Struct passed to get_v_event() and restore_v_event().
قسمتعریف ساخت {
    صحیح		sve_did_save;
    hashtab_T	sve_hashtab;
} save_v_event_T;

// Enum used by filter(), map(), mapnew() and foreach()
قسمتعریف شمار {
    FILTERMAP_FILTER,
    FILTERMAP_MAP,
    FILTERMAP_MAPNEW,
    FILTERMAP_FOREACH
} filtermap_T;

// Structure used by switch_win() to pass values to restore_win()
قسمتعریف ساخت {
    win_T	*sw_curwin;
    tabpage_T	*sw_curtab;
    صحیح		sw_same_win;	    // VIsual_active was not reset
    صحیح		sw_visual_فعال;
} switchwin_T;

// Fuzzy matched string list item. Used for fuzzy match completion. Items are
// usually sorted by 'score'. The 'idx' member is used for stable-sort.
قسمتعریف ساخت {
    صحیح		idx;
    char_u	*str;
    صحیح		score;
} fuzmatch_str_T;

// Argument for lbr_chartabsize().
قسمتعریف ساخت {
    win_T	*cts_win;
    char_u	*cts_سطر;		// start of the line
    char_u	*cts_ptr;		// current position in line
#اگر_تعریف_شدہ FEAT_LINEBREAK
    صحیح		cts_bri_سائز;		// cached size of 'breakindent', or -1
					// if not computed yet
#اگر_ختم
#اگر_تعریف_شدہ FEAT_PROP_POPUP
    صحیح		cts_متن_prop_گنتی;	// number of text props; when zero
					// cts_text_props is not used
    textprop_T	*cts_متن_props;	// text props (allocated)
    حرف	cts_has_prop_with_متن;	// TRUE if a property inserts text
    صحیح		cts_cur_متن_چوڑائی;	// width of current inserted text
    صحیح		cts_prop_سطریں;		// nr of properties above or below
    bool	cts_has_نیچے;		// true if a text property below was
					// counted, its width fills up the line
    صحیح		cts_پہلا_char;		// width text props above the line
    صحیح		cts_above_چوڑائی;	// width of text props above the line,
					// kept for the whole line
    bool	cts_no_above;		// do not count the width of text props
					// above the line
    صحیح		cts_with_trailing;	// include size of trailing props with
					// last character
    صحیح		cts_آغاز_incl;		// prop has true "start_incl" arg
#اگر_ختم
    صحیح		cts_vcol;		// virtual column at current position
    صحیح		cts_max_سر_vcol;	// see win_lbr_chartabsize()
} chartabsize_T;

/*
 * Argument for the callback function (opt_did_set_cb_T) invoked after an
 * option value is modified.
 */
قسمتعریف ساخت
{
    // Pointer to the option variable.  The variable can be a long (numeric
    // option), an int (boolean option) or a char pointer (string option).
    char_u	*os_varp;
    صحیح		os_idx;
    صحیح		os_flags;
    سیٹ_op_T	os_op;

    // old value of the option (can be a string, number or a boolean)
    اتحاد
    {
	طویل	نمبر;
	صحیح	boolean;
	char_u	*string;
    } os_oldval;

    // new value of the option (can be a string, number or a boolean)
    اتحاد
    {
	طویل	نمبر;
	صحیح	boolean;
	char_u	*string;
    } os_newval;

    // Option value was checked to be safe, no need to set P_INSECURE
    // Used for the 'keymap', 'filetype' and 'syntax' options.
    صحیح		os_قدر_checked;
    // Option value changed.  Used for the 'filetype' and 'syntax' options.
    صحیح		os_قدر_بدلا;

    // Used by the 'isident', 'iskeyword', 'isprint' and 'isfname' options.
    // Set to TRUE if the character table is modified when processing the
    // option and need to be restored because of a failure.
    صحیح		os_restore_chartab;

#اگر defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
    // Used by the 't_xxx' terminal options on MS-Windows.
    صحیح		os_did_swaptcap;
#اگر_ختم

    // If the value specified for an option is not valid and the error message
    // is parameterized, then the "os_errbuf" buffer is used to store the error
    // message (when it is not NULL).
    حرف	*os_errbuf;
    // length of the error buffer
    size_t	os_errbuflen;
} optset_T;

/*
 * Argument for the callback function (opt_expand_cb_T) invoked after a string
 * option value is expanded for cmdline completion.
 */
قسمتعریف ساخت
{
    // Pointer to the option variable. It's always a string.
    char_u	*oe_varp;
    // The original option value, escaped.
    char_u	*oe_opt_قدر;

    // TRUE if using set+= instead of set=
    صحیح		oe_append;
    // TRUE if we would like to add the original option value as the first
    // choice.
    صحیح		oe_include_orig_val;

    // Regex from the cmdline, for matching potential options against.
    regmatch_T	*oe_regmatch;
    // The expansion context.
    expand_T	*oe_xp;

    // The full argument passed to :set. For example, if the user inputs
    // ':set dip=icase,algorithm:my<Tab>', oe_xp->xp_pattern will only have
    // 'my', but oe_set_arg will contain the whole 'icase,algorithm:my'.
    char_u	*oe_سیٹ_arg;
} optexpand_T;

/*
 * Spell checking variables passed from win_update() to win_line().
 */
قسمتعریف ساخت {
    صحیح		spv_has_spell;	    // drawn window has spell checking
#اگر_تعریف_شدہ FEAT_SPELL
    صحیح		spv_unchanged;	    // not updating for changed text
    صحیح		spv_checked_ستون;    // column in "checked_lnum" up to
				    // which there are no spell errors
    linenr_T	spv_checked_lnum;   // line number for "checked_col"
    صحیح		spv_cap_ستون;	    // column to check for Cap word
    linenr_T	spv_capcol_lnum;    // line number for "cap_col"
#اگر_ختم
} spellvars_T;

// Return the length of a string literal.
// This macro only computes a string's length for a string-literal token; for
// anything else, including a char*, compilation will fail (note "" following
// s).
#تعریف_کرو STRLEN_LITERAL(s) (حجم(s "") - 1)

// Store a key/value (string) pair
قسمتعریف ساخت
{
    صحیح	    کلید;        // the key
    string_T قدر;	// the value
} keyvalue_T;

#تعریف_کرو KEYVALUE_اندراج(k, v) \
    {(k), {((char_u *)v), STRLEN_LITERAL(v)}}

#اگر defined(UNIX) || defined(MSWIN) || defined(VMS) || defined(AMIGA)
// Defined as signed, to return -1 on error
ساخت cellsize {
    صحیح cs_xpixel;
    صحیح cs_ypixel;
};
#اگر_ختم

#اگر_تعریف_شدہ FEAT_WAYLAND

قسمتعریف ساخت vwl_connection_S vwl_connection_T;
قسمتعریف ساخت vwl_seat_S vwl_seat_T;

# ifdef FEAT_WAYLAND_CLIPBOARD

قسمتعریف ساخت vwl_ڈیٹا_offer_S vwl_ڈیٹا_offer_T;
قسمتعریف ساخت vwl_ڈیٹا_ماخذ_S vwl_ڈیٹا_ماخذ_T;
قسمتعریف ساخت vwl_ڈیٹا_device_S vwl_ڈیٹا_device_T;
قسمتعریف ساخت vwl_ڈیٹا_device_manager_S vwl_ڈیٹا_device_manager_T;

قسمتعریف ساخت vwl_ڈیٹا_device_listener_S vwl_ڈیٹا_device_listener_T;
قسمتعریف ساخت vwl_ڈیٹا_ماخذ_listener_S vwl_ڈیٹا_ماخذ_listener_T;
قسمتعریف ساخت vwl_ڈیٹا_offer_listener_S vwl_ڈیٹا_offer_listener_T;

// Wayland selections
قسمتعریف شمار {
    WAYLAND_SELECTION_NONE	= 0,
    WAYLAND_SELECTION_REGULAR	= 1 << 0,
    WAYLAND_SELECTION_PRIMARY	= 1 << 1,
} wayland_selection_T;

# endif
#اگر_ختم

// Used in term_set_sync_output()
قسمتعریف شمار
{
    رکن_SYNC_OUTPUT_ENABLE = 1 << 0,
    رکن_SYNC_OUTPUT_DISABLE = 1 << 1,
    رکن_SYNC_OUTPUT_OFF = 1 << 2,
    رکن_SYNC_OUTPUT_FLUSH = 1 << 3,
} رکن_sync_output_T;
