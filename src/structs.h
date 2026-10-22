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
#إذا defined(SASC) && SASC < 658
تعريفنوع ڊگهو		linenr_T;
تعريفنوع صحيح		colnr_T;
تعريفنوع بنانشان ننڍو	short_u;
#جيڪڏهن_پورو

// structure to store a string (including it's length)
تعريفنوع بنية
{
    char_u  *string;		// the string
    size_t  ڊيگهائي;		// length of the string (excluding the NUL)
} string_T;

/*
 * Position in file or buffer.
 */
تعريفنوع بنية
{
    linenr_T	lnum;	// line number
    colnr_T	کانو;	// column number
    colnr_T	coladd; // extra virtual column
} pos_T;


/*
 * Same, but without coladd.
 */
تعريفنوع بنية
{
    linenr_T	lnum;	// line number
    colnr_T	کانو;	// column number
} lpos_T;

/*
 * Structure used for growing arrays.
 * This is used to store information that only grows, is deleted all at
 * once, and needs to be accessed by index.  See ga_clear() and ga_grow().
 */
تعريفنوع بنية growarray
{
    صحيح	    ga_len;		    // current number of items used
    صحيح	    ga_maxlen;		    // maximum number of items possible
    صحيح	    ga_itemsize;	    // sizeof(item)
    صحيح	    ga_growsize;	    // number of items to grow each time
    فراغ    *ga_معلومات;		    // pointer to the first item
} garray_T;

#وصف_ڪريو GA_خالي_جايون    {0, 0, 0, 0, عدم}

// On rare systems "char" is unsigned, sometimes we really want a signed 8-bit
// value.
تعريفنوع نشانوارو حرف	int8_T;
تعريفنوع مزدوج		float_T;

تعريفنوع بنية typval_S		typval_T;
تعريفنوع بنية listvar_S	list_T;
تعريفنوع بنية dictvar_S	dict_T;
تعريفنوع بنية partial_S	partial_T;
تعريفنوع بنية blobvar_S	blob_T;
تعريفنوع بنية tuplevar_S	tuple_T;
تعريفنوع بنية generictype_S	generic_T;
تعريفنوع بنية gfargs_tab_S	gfargs_tab_T;

تعريفنوع بنية دري_S		win_T;
تعريفنوع بنية wininfo_S	wininfo_T;
تعريفنوع بنية چوکٺو_S		چوکٺو_T;
تعريفنوع صحيح			scid_T;		// script ID
تعريفنوع بنية file_بفر	buf_T;		// forward declaration
تعريفنوع بنية terminal_S	رڪن_T;

#جيڪڏهن_وصف_ٿيل FEAT_MENU
تعريفنوع بنية VimMenu vimmenu_T;
#جيڪڏهن_پورو

// maximum value for sc_version
#وصف_ڪريو SCRIPT_نسخو_MAX 4
// value for sc_version in a Vim9 script file
#وصف_ڪريو SCRIPT_نسخو_VIM9 999999

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
تعريفنوع بنية {
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
    scid_T	sc_sid;		// script ID
    صحيح		sc_seq;		// sourcing sequence number
    linenr_T	sc_lnum;	// line number
#جيڪڏهن_پورو
    صحيح		sc_نسخو;	// :scriptversion
} sctx_T;

/*
 * Reference to a buffer that stores the value of buf_free_count.
 * bufref_valid() only needs to check "buf" when the count differs.
 */
تعريفنوع بنية {
    buf_T   *br_buf;
    صحيح	    br_fnum;
    صحيح	    br_buf_free_ڳڻپ;
} bufref_T;

/*
 * This is here because regexp.h needs pos_T and below regprog_T is used.
 */
#ضمن "regexp.h"

/*
 * This is here because gui.h needs the pos_T and win_T, and win_T needs gui.h
 * for scrollbar_T.
 */
#جيڪڏهن_وصف_ٿيل FEAT_GUI
# include "gui.h"
#وإلا
# ifdef FEAT_XCLIPBOARD
#  include <X11/Intrinsic.h>
# endif
# define guicolor_T ڊگهو
# define INVALCOLOR ((guicolor_T)0x1ffffff)
    // only used for cterm.bg_rgb and cterm.fg_rgb: use cterm color
# define CTERMCOLOR ((guicolor_T)0x1fffffe)
#جيڪڏهن_پورو
// We have include guard in the file
#ضمن "image.h"
#وصف_ڪريو COLOR_INVALID(x) ((x) == INVALCOLOR || (x) == CTERMCOLOR)

#جيڪڏهن_وصف_ٿيل FEAT_TERMINAL
# include "libvterm/include/vterm.h"
تعريفنوع بنية {
    VTermColor	fg;
    VTermColor	bg;
} termcellcolor_T;
#جيڪڏهن_پورو

/*
 * marks: positions in a file
 * (a normal mark is a lnum/col pair, the same as a file position)
 */

#وصف_ڪريو NMARKS		('z' - 'a' + 1)	// max. # of named marks
#وصف_ڪريو EXTRA_MARKS	10		// marks 0-9
#وصف_ڪريو JUMPLISTSIZE	100		// max. # of marks in jump list
#وصف_ڪريو TAGSTACKSIZE	20		// max. # of tags in tag stack

تعريفنوع بنية filemark
{
    pos_T	mark;		// cursor position
    صحيح		fnum;		// file number
} fmark_T;

// Xtended file mark: also has a file name
تعريفنوع بنية xfilemark
{
    fmark_T	fmark;
    char_u	*fname;		// file name, used when fnum == 0
#جيڪڏهن_وصف_ٿيل FEAT_VIMINFO
    وقت_T	وقت_سيٽ;
#جيڪڏهن_پورو
} xfmark_T;

/*
 * The taggy struct is used to store the information about a :tag command.
 */
تعريفنوع بنية taggy
{
    char_u	*tagname;	// tag name
    fmark_T	fmark;		// cursor position BEFORE ":tag"
    صحيح		cur_ميلاپ;	// match number
    صحيح		cur_fnum;	// buffer number used for cur_match
    char_u	*user_معلومات;	// used with tagfunc
} taggy_T;

/*
 * Structure that contains all options that are local to a window.
 * Used twice in a window: for the current buffer and for all buffers.
 * Also used in wininfo_T.
 */
تعريفنوع بنية
{
#جيڪڏهن_وصف_ٿيل FEAT_ARABIC
    صحيح		wo_arab;
# define w_p_arab w_onebuf_opt.wo_arab	// 'arabic'
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_LINEBREAK
    صحيح		wo_bri;
# define w_p_bri w_onebuf_opt.wo_bri	// 'breakindent'
    char_u	*wo_briopt;
# define w_p_briopt w_onebuf_opt.wo_briopt // 'breakindentopt'
#جيڪڏهن_پورو
    char_u	*wo_wcr;
#وصف_ڪريو w_p_wcr w_onebuf_opt.wo_wcr	// 'wincolor'
#جيڪڏهن_وصف_ٿيل FEAT_DIFF
    صحيح		wo_diff;
# define w_p_diff w_onebuf_opt.wo_diff	// 'diff'
#جيڪڏهن_پورو
    char_u	*wo_eiw;
#وصف_ڪريو w_p_eiw w_onebuf_opt.wo_eiw	// 'eventignorewin'
#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
    ڊگهو	wo_fdc;
# define w_p_fdc w_onebuf_opt.wo_fdc	// 'foldcolumn'
    صحيح		wo_fdc_save;
# define w_p_fdc_save w_onebuf_opt.wo_fdc_save	// 'foldenable' saved for diff mode
    صحيح		wo_fen;
# define w_p_fen w_onebuf_opt.wo_fen	// 'foldenable'
    صحيح		wo_fen_save;
# define w_p_fen_save w_onebuf_opt.wo_fen_save	// 'foldenable' saved for diff mode
    char_u	*wo_fdi;
# define w_p_fdi w_onebuf_opt.wo_fdi	// 'foldignore'
    ڊگهو	wo_fdl;
# define w_p_fdl w_onebuf_opt.wo_fdl	// 'foldlevel'
    صحيح		wo_fdl_save;
# define w_p_fdl_save w_onebuf_opt.wo_fdl_save	// 'foldlevel' state saved for diff mode
    char_u	*wo_fdm;
# define w_p_fdm w_onebuf_opt.wo_fdm	// 'foldmethod'
    char_u	*wo_fdm_save;
# define w_p_fdm_save w_onebuf_opt.wo_fdm_save	// 'fdm' saved for diff mode
    ڊگهو	wo_fml;
# define w_p_fml w_onebuf_opt.wo_fml	// 'foldminlines'
    ڊگهو	wo_fdn;
# define w_p_fdn w_onebuf_opt.wo_fdn	// 'foldnestmax'
# ifdef FEAT_EVAL
    char_u	*wo_fde;
#  define w_p_fde w_onebuf_opt.wo_fde	// 'foldexpr'
    char_u	*wo_fdt;
#  define w_p_fdt w_onebuf_opt.wo_fdt	// 'foldtext'
# endif
    char_u	*wo_fmr;
# define w_p_fmr w_onebuf_opt.wo_fmr	// 'foldmarker'
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_LINEBREAK
    صحيح		wo_lbr;
# define w_p_lbr w_onebuf_opt.wo_lbr	// 'linebreak'
#جيڪڏهن_پورو
    صحيح		wo_list;
#وصف_ڪريو w_p_list w_onebuf_opt.wo_list	// 'list'
    char_u	*wo_lcs;
#وصف_ڪريو w_p_lcs w_onebuf_opt.wo_lcs	// 'listchars'
    char_u	*wo_fcs;
#وصف_ڪريو w_p_fcs w_onebuf_opt.wo_fcs	// 'fillchars'
    صحيح		wo_nu;
#وصف_ڪريو w_p_nu w_onebuf_opt.wo_nu	// 'number'
    صحيح		wo_rnu;
#وصف_ڪريو w_p_rnu w_onebuf_opt.wo_rnu	// 'relativenumber'
    char_u	*wo_ve;
#وصف_ڪريو w_p_ve w_onebuf_opt.wo_ve	// 'virtualedit'
    بنانشان	wo_ve_flags;
#وصف_ڪريو	w_ve_flags w_onebuf_opt.wo_ve_flags	// flags for 'virtualedit'
#جيڪڏهن_وصف_ٿيل FEAT_LINEBREAK
    ڊگهو	wo_nuw;
# define w_p_nuw w_onebuf_opt.wo_nuw	// 'numberwidth'
#جيڪڏهن_پورو
    صحيح wo_wfb;
#وصف_ڪريو w_p_wfb w_onebuf_opt.wo_wfb	// 'winfixbuf'
    صحيح		wo_wfh;
#وصف_ڪريو w_p_wfh w_onebuf_opt.wo_wfh	// 'winfixheight'
    صحيح		wo_wfw;
#وصف_ڪريو w_p_wfw w_onebuf_opt.wo_wfw	// 'winfixwidth'
#إذا defined(FEAT_QUICKFIX)
    صحيح		wo_pvw;
# define w_p_pvw w_onebuf_opt.wo_pvw	// 'previewwindow'
    ڊگهو        wo_lhi;
# define w_p_lhi w_onebuf_opt.wo_lhi    // 'lhistory'
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_RIGHTLEFT
    صحيح		wo_rl;
# define w_p_rl w_onebuf_opt.wo_rl	// 'rightleft'
    char_u	*wo_rlc;
# define w_p_rlc w_onebuf_opt.wo_rlc	// 'rightleftcmd'
#جيڪڏهن_پورو
    ڊگهو	wo_scr;
#وصف_ڪريو w_p_scr w_onebuf_opt.wo_scr	// 'scroll'
    صحيح		wo_sms;
#وصف_ڪريو w_p_sms w_onebuf_opt.wo_sms	// 'smoothscroll'
#جيڪڏهن_وصف_ٿيل FEAT_SPELL
    صحيح		wo_spell;
# define w_p_spell w_onebuf_opt.wo_spell // 'spell'
#جيڪڏهن_پورو
#إذا defined(FEAT_SYN_HL) || defined(FEAT_FOLDING) || defined(FEAT_DIFF)
    صحيح		wo_cuc;
# define w_p_cuc w_onebuf_opt.wo_cuc	// 'cursorcolumn'
    صحيح		wo_cul;
# define w_p_cul w_onebuf_opt.wo_cul	// 'cursorline'
    char_u	*wo_culopt;
# define w_p_culopt w_onebuf_opt.wo_culopt	// 'cursorlineopt'
    char_u	*wo_cc;
# define w_p_cc w_onebuf_opt.wo_cc	// 'colorcolumn'
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_LINEBREAK
    char_u	*wo_sbr;
# define w_p_sbr w_onebuf_opt.wo_sbr	// 'showbreak'
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_STL_OPT
    char_u	*wo_stl;
# define w_p_stl w_onebuf_opt.wo_stl	// 'statusline'
    char_u	*wo_stlo;
# define w_p_stlo w_onebuf_opt.wo_stlo	// 'statuslineopt'
    صحيح		wo_stlo_fh;
# define w_p_stlo_fh w_onebuf_opt.wo_stlo_fh // "fixedheight" of 'statuslineopt'
    صحيح		wo_stlo_mh;
# define w_p_stlo_mh w_onebuf_opt.wo_stlo_mh // "maxheight:" of 'statuslineopt'
#جيڪڏهن_پورو
    صحيح		wo_scb;
#وصف_ڪريو w_p_scb w_onebuf_opt.wo_scb	// 'scrollbind'
    صحيح		wo_diff_saved; // options were saved for starting diff mode
#وصف_ڪريو w_p_diff_saved w_onebuf_opt.wo_diff_saved
    صحيح		wo_scb_save;	// 'scrollbind' saved for diff mode
#وصف_ڪريو w_p_scb_save w_onebuf_opt.wo_scb_save
    صحيح		wo_wrap;
#وصف_ڪريو w_p_wrap w_onebuf_opt.wo_wrap	// 'wrap'
#جيڪڏهن_وصف_ٿيل FEAT_DIFF
    صحيح		wo_wrap_save;	// 'wrap' state saved for diff mode
# define w_p_wrap_save w_onebuf_opt.wo_wrap_save
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_CONCEAL
    char_u	*wo_cocu;		// 'concealcursor'
# define w_p_cocu w_onebuf_opt.wo_cocu
    ڊگهو	wo_cole;		// 'conceallevel'
# define w_p_cole w_onebuf_opt.wo_cole
#جيڪڏهن_پورو
    صحيح		wo_crb;
#وصف_ڪريو w_p_crb w_onebuf_opt.wo_crb	// 'cursorbind'
    صحيح		wo_crb_save;	// 'cursorbind' state saved for diff mode
#وصف_ڪريو w_p_crb_save w_onebuf_opt.wo_crb_save
#جيڪڏهن_وصف_ٿيل FEAT_SIGNS
    char_u	*wo_scl;
# define w_p_scl w_onebuf_opt.wo_scl	// 'signcolumn'
#جيڪڏهن_پورو
    ڊگهو	wo_siso;
#وصف_ڪريو w_p_siso w_onebuf_opt.wo_siso	// 'sidescrolloff' local value
    ڊگهو	wo_so;
#وصف_ڪريو w_p_so w_onebuf_opt.wo_so	// 'scrolloff' local value
    ڊگهو	wo_sop;
#وصف_ڪريو w_p_sop w_onebuf_opt.wo_sop  // 'scrolloffpad' local value
#جيڪڏهن_وصف_ٿيل FEAT_TERMINAL
    char_u	*wo_twk;
# define w_p_twk w_onebuf_opt.wo_twk	// 'termwinkey'
    char_u	*wo_tws;
# define w_p_tws w_onebuf_opt.wo_tws	// 'termwinsize'
#جيڪڏهن_پورو

    // A few options have local flags for P_INSECURE.
    long_u	wo_wrap_flags;		// flags for 'wrap'
#وصف_ڪريو w_p_wrap_flags w_onebuf_opt.wo_wrap_flags
#جيڪڏهن_وصف_ٿيل FEAT_STL_OPT
    long_u	wo_stl_flags;		// flags for 'statusline'
# define w_p_stl_flags w_onebuf_opt.wo_stl_flags
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
    long_u	wo_fde_flags;		// flags for 'foldexpr'
# define w_p_fde_flags w_onebuf_opt.wo_fde_flags
    long_u	wo_fdt_flags;		// flags for 'foldtext'
# define w_p_fdt_flags w_onebuf_opt.wo_fdt_flags
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_EVAL
    sctx_T	wo_script_ctx[WV_ڳڻپ];	// SCTXs for window-local options
# define w_p_script_ctx w_onebuf_opt.wo_script_ctx
#جيڪڏهن_پورو
    char_u	*wo_whl;
#وصف_ڪريو w_p_whl w_onebuf_opt.wo_whl	// 'winhighlight'
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
بنية wininfo_S
{
    wininfo_T	*wi_ايندڙ;	// next entry or NULL for last entry
    wininfo_T	*wi_prev;	// previous entry or NULL for first entry
    win_T	*wi_win;	// pointer to window that did set wi_fpos
    pos_T	wi_fpos;	// last cursor position in the file
    winopt_T	wi_opt;		// local window options
    صحيح		wi_optset;	// TRUE when wi_opt has useful values
#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
    صحيح		wi_fold_manual;	// copy of w_fold_manual
    garray_T	wi_folds;	// clone of w_folds
#جيڪڏهن_پورو
    صحيح		wi_changelistidx; // copy of w_changelistidx
};

/*
 * Info used to pass info about a fold from the fold-detection code to the
 * code that displays the foldcolumn.
 */
تعريفنوع بنية foldinfo
{
    صحيح		fi_level;	// level of the fold; when this is zero the
				// other fields are invalid
    صحيح		fi_lnum;	// line number where fold starts
    صحيح		fi_هيٺيون_level;	// lowest fold level that starts in the same
				//سٽ
} foldinfo_T;

/*
 * Structure to store info about the Visual area.
 */
تعريفنوع بنية
{
    pos_T	vi_شروعات;	// start pos of last VIsual
    pos_T	vi_پڇاڙي;		// end position of last VIsual
    صحيح		vi_گهڻائي;	// VIsual_mode of last VIsual
    colnr_T	vi_curswant;	// MAXCOL from w_curswant
} visualinfo_T;

/*
 * structures used for undo
 */

// One line saved for undo.  After the NUL terminated text there might be text
// properties, thus ul_len can be larger than STRLEN(ul_line) + 1.
تعريفنوع بنية {
    char_u	*ul_سٽ;	// text of the line
    ڊگهو	ul_len;		// length of the line including NUL, plus text
				// properties
    colnr_T	ul_textlen;	// length of the line excluding NUL and any text
				// properties
} undoline_T;

تعريفنوع بنية u_داخلا u_داخلا_T;
تعريفنوع بنية u_سرو u_سرو_T;

#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
// Which line around an undo block a cleared continuation flag was on.
# define UNDOPROP_NONE	0
# define UNDOPROP_ABOVE	1
# define UNDOPROP_هيٺ	2

// A continuation flag cleared on a line around an undo block, given back when
// the block is undone or redone the other way.
تعريفنوع بنية
{
    صحيح		مٿي_which;	// UNDOPROP_ABOVE or UNDOPROP_BELOW
    صحيح		مٿي_type;	// tp_type of the property
    صحيح		مٿي_id;		// tp_id of the property
    colnr_T	مٿي_کانو;		// tp_col of the property
    صحيح		مٿي_flag;	// TP_FLAG_CONT_NEXT or TP_FLAG_CONT_PREV
} undoprop_T;
#جيڪڏهن_پورو

بنية u_داخلا
{
    u_داخلا_T	*ue_ايندڙ;	// pointer to next entry in list
    linenr_T	ue_چوٽي;		// number of line above undo block
    linenr_T	ue_bot;		// number of line below undo block
    linenr_T	ue_lcount;	// linecount when u_save called
    undoline_T	*ue_array;	// array of lines in undo block
    ڊگهو	ue_سائيز;	// number of lines in ue_array
#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
    garray_T	ue_props;	// undoprop_T: flags cleared around the block
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل U_DEBUG
    صحيح		ue_magic;	// magic number to check allocation
#جيڪڏهن_پورو
};

بنية u_سرو
{
    // The following have a pointer and a number. The number is used when
    // reading the undo file in u_read_undo()
    اتحاد {
	u_سرو_T *ptr;	// pointer to next undo header in list
	ڊگهو	   seq;
    } uh_ايندڙ;
    اتحاد {
	u_سرو_T *ptr;	// pointer to previous header in list
	ڊگهو	   seq;
    } uh_prev;
    اتحاد {
	u_سرو_T *ptr;	// pointer to next header for alt. redo
	ڊگهو	   seq;
    } uh_alt_ايندڙ;
    اتحاد {
	u_سرو_T *ptr;	// pointer to previous header for alt. redo
	ڊگهو	   seq;
    } uh_alt_prev;
    ڊگهو	uh_seq;		// sequence number, higher == newer undo
    صحيح		uh_walk;	// used by undo_time()
    u_داخلا_T	*uh_داخلا;	// pointer to first entry
    u_داخلا_T	*uh_getbot_داخلا; // pointer to where ue_bot must be set
    pos_T	uh_cursor;	// cursor position before saving
    ڊگهو	uh_cursor_vcol;
    صحيح		uh_flags;	// see below
    pos_T	uh_namedm[NMARKS];	// marks before undo/after redo
    visualinfo_T uh_visual;	// Visual areas before undo/after redo
    وقت_T	uh_وقت;	// timestamp when the change was made
    ڊگهو	uh_save_nr;	// set when the file was saved after the
				// changes in this block
#جيڪڏهن_وصف_ٿيل U_DEBUG
    صحيح		uh_magic;	// magic number to check allocation
#جيڪڏهن_پورو
};

// values for uh_flags
#وصف_ڪريو UH_بدليو  0x01	// b_changed flag before undo/after redo
#وصف_ڪريو UH_EMPTYBUF 0x02	// buffer was empty

/*
 * structures used in undo.c
 */
#وصف_ڪريو ALIGN_LONG	// longword alignment and use filler byte
#وصف_ڪريو ALIGN_سائيز (حجم(ڊگهو))

#وصف_ڪريو ALIGN_نقاب (ALIGN_سائيز - 1)

تعريفنوع بنية m_info minfo_T;

/*
 * structure used to link chunks in one of the free chunk lists.
 */
بنية m_info
{
#جيڪڏهن_وصف_ٿيل ALIGN_LONG
    long_u	m_سائيز_2;		// size of the chunk (including m_info)
#وإلا
    short_u	m_سائيز_2;		// size of the chunk (including m_info)
#جيڪڏهن_پورو
    minfo_T	*m_ايندڙ;	// pointer to next free chunk in the list
};

/*
 * things used in memfile.c
 */

تعريفنوع بنية بلاڪ_hdr    bhdr_T;
تعريفنوع بنية memfile	    memfile_T;
تعريفنوع ڊگهو		    blocknr_T;

/*
 * mf_hashtab_T is a chained hashtable with blocknr_T key and arbitrary
 * structures as items.  This is an intrusive data structure: we require
 * that items begin with mf_hashitem_T which contains the key and linked
 * list pointers.  List of items in each bucket is doubly-linked.
 */

تعريفنوع بنية mf_hashitem_S mf_hashitem_T;

بنية mf_hashitem_S
{
    mf_hashitem_T   *mhi_ايندڙ;
    mf_hashitem_T   *mhi_prev;
    blocknr_T	    mhi_ڪنجي;
};

#وصف_ڪريو MHT_INIT_سائيز   64

تعريفنوع بنية mf_hashtab_S
{
    long_u	    mht_نقاب;	    // mask used for hash value (nr of items
				    // in array is "mht_mask" + 1)
    long_u	    mht_ڳڻپ;	    // nr of items inserted into hashtable
    mf_hashitem_T   **mht_buckets;  // points to mht_small_buckets or
				    //dynamically allocated array
    mf_hashitem_T   *mht_small_buckets[MHT_INIT_سائيز];   // initial buckets
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

بنية بلاڪ_hdr
{
    mf_hashitem_T bh_hashitem;      // header for hash table and key
#وصف_ڪريو bh_bnum bh_hashitem.mhi_ڪنجي // block number, part of bh_hashitem

    bhdr_T	*bh_ايندڙ;	    // next block_hdr in free or used list
    bhdr_T	*bh_prev;	    // previous block_hdr in used list
    char_u	*bh_معلومات;	    // pointer to memory (for used block)
    صحيح		bh_صفحو_ڳڻپ;	    // number of pages in this block

#وصف_ڪريو BH_DIRTY    1
#وصف_ڪريو BH_LOCKED   2
    حرف	bh_flags;	    // BH_DIRTY or BH_LOCKED
};

/*
 * when a block with a negative number is flushed to the file, it gets
 * a positive number. Because the reference to the block is still the negative
 * number, we remember the translation to the new positive number in the
 * double linked trans lists. The structure is the same as the hash lists.
 */
تعريفنوع بنية nr_trans NR_TRANS;

بنية nr_trans
{
    mf_hashitem_T nt_hashitem;		// header for hash table and key
#وصف_ڪريو nt_old_bnum nt_hashitem.mhi_ڪنجي	// old, negative, number

    blocknr_T	nt_new_bnum;		// new, positive, number
};


تعريفنوع بنية buffblock buffblock_T;
تعريفنوع بنية buffheader buffheader_T;

/*
 * structure used to store one block of the stuff/redo/recording buffers
 */
بنية buffblock
{
    buffblock_T	*b_ايندڙ;	// pointer to next buffblock
    size_t	b_strlen;	// length of b_str, excluding the NUL
    char_u	b_str[1];	// contents (actually longer)
};

/*
 * header used for the stuff buffer and the redo buffer
 */
بنية buffheader
{
    buffblock_T	bh_پهريون;	// first (dummy) block of list
    buffblock_T	*bh_curr;	// buffblock for appending
    صحيح		bh_اشاريو;	// index for reading
    صحيح		bh_space;	// space in bh_curr for appending
    صحيح		bh_create_newblock;	// create a new block?
};

تعريفنوع بنية
{
    buffheader_T sr_redobuff;
    buffheader_T sr_old_redobuff;
} save_redo_T;

تعريفنوع تعداد {
    XP_اڳياڙي_NONE,	// prefix not used
    XP_اڳياڙي_NO,	// "no" prefix for bool option
    XP_اڳياڙي_INV,	// "inv" prefix for bool option
} xp_اڳياڙي_T;

/*
 * :set operator types
 */
تعريفنوع تعداد {
    OP_NONE = 0,
    OP_ADDING,		// "opt+=arg"
    OP_PREPENDING,	// "opt^=arg"
    OP_REMOVING,	// "opt-=arg"
} سيٽ_op_T;

/*
 * used for completion on the command line
 */
تعريفنوع بنية expand
{
    char_u	*xp_نمونو;		// start of item to expand, guaranteed
					// to be part of xp_line
    صحيح		xp_context;		// type of expansion
    صحيح		xp_نمونو_len;		// bytes in xp_pattern before cursor
    xp_اڳياڙي_T	xp_اڳياڙي;
#إذا defined(FEAT_EVAL)
    char_u	*xp_arg;		// completion function
    صحيح		xp_complete_opt;	// UCC_ flags for user command
    sctx_T	xp_script_ctx;		// SCTX for completion function
#جيڪڏهن_پورو
    صحيح		xp_backslash;		// one of the XP_BS_ values
#جيڪڏهن_وصف_نه_ٿيل BACKSLASH_اندريون_FILENAME
    صحيح		xp_shell;		// TRUE for a shell command, more
					// characters need to be escaped
#جيڪڏهن_پورو
    صحيح		xp_numfiles;		// number of files found by
					// file name completion
    صحيح		xp_کانو;			// cursor position in line
    صحيح		xp_selected;		// selected index in completion
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
    char_u	*xp_سٽ;		// text being completed
#وصف_ڪريو EXPAND_BUF_LEN 256
    char_u	xp_buf[EXPAND_BUF_LEN];	// buffer for returned match
    صحيح		xp_ڳولا_dir;		// Direction of search
    pos_T	xp_pre_incsearch_pos;	// Cursor position before incsearch
} expand_T;

/*
 * values for xp_backslash
 */
#وصف_ڪريو XP_BS_NONE	0	// nothing special for backslashes
#وصف_ڪريو XP_BS_ONE	0x1	// uses one backslash before a space
#وصف_ڪريو XP_BS_THREE	0x2	// uses three backslashes before a space
#وصف_ڪريو XP_BS_COMMA	0x4	// commas need to be escaped with a backslash

/*
 * Variables shared between getcmdline(), redrawcmdline() and others.
 * These need to be saved when using CTRL-R |, that's why they are in a
 * structure.
 */
تعريفنوع بنية
{
    char_u	*cmdbuff;	// pointer to command line buffer
    صحيح		cmdbufflen;	// length of cmdbuff
    صحيح		cmdlen;		// number of chars in command line
    صحيح		cmdpos;		// current cursor position
    صحيح		cmdspos;	// cursor column on screen
    صحيح		cmdfirstc;	// ':', '/', '?', '=', '>' or NUL
    صحيح		cmdindent;	// number of spaces before cmdline
    char_u	*cmdprompt;	// message in front of cmdline
    صحيح		cmdattr;	// attributes for prompt
    صحيح		overstrike;	// Typing mode on the command line.  Shared by
				// getcmdline() and put_on_cmdline().
    expand_T	*xpc;		// struct being used for expansion, xp_pattern
				// may point into cmdbuff
    صحيح		xp_context;	// type of expansion
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
    char_u	*xp_arg;	// user-defined expansion arg
    صحيح		داخلا_fn;	// when TRUE Invoked for input() function
#جيڪڏهن_پورو
    صحيح		cmdbuff_replaced; // when TRUE cmdline was replaced externally
				  // (e.g. by setcmdline())
} cmdline_info_T;

/*
 * Command modifiers ":vertical", ":browse", ":confirm" and ":hide" set a flag.
 * This needs to be saved for recursive commands, put them in a structure for
 * easy manipulation.
 */
تعريفنوع بنية
{
    صحيح		cmod_flags;		// CMOD_ flags
#وصف_ڪريو CMOD_SANDBOX	    0x0001	// ":sandbox"
#وصف_ڪريو CMOD_SILENT	    0x0002	// ":silent"
#وصف_ڪريو CMOD_ERRSILENT	    0x0004	// ":silent!"
#وصف_ڪريو CMOD_UNSILENT	    0x0008	// ":unsilent"
#وصف_ڪريو CMOD_NOAUTOCMD	    0x0010	// ":noautocmd"
#وصف_ڪريو CMOD_HIDE	    0x0020	// ":hide"
#وصف_ڪريو CMOD_BROWSE	    0x0040	// ":browse" - invoke file dialog
#وصف_ڪريو CMOD_CONFIRM	    0x0080	// ":confirm" - invoke yes/no dialog
#وصف_ڪريو CMOD_KEEPALT	    0x0100	// ":keepalt"
#وصف_ڪريو CMOD_KEEPMARKS	    0x0200	// ":keepmarks"
#وصف_ڪريو CMOD_KEEPJUMPS	    0x0400	// ":keepjumps"
#وصف_ڪريو CMOD_LOCKMARKS	    0x0800	// ":lockmarks"
#وصف_ڪريو CMOD_KEEPPATTERNS   0x1000	// ":keeppatterns"
#وصف_ڪريو CMOD_NOSWAPFILE	    0x2000	// ":noswapfile"
#وصف_ڪريو CMOD_VIM9CMD	    0x4000	// ":vim9cmd"
#وصف_ڪريو CMOD_LEGACY	    0x8000	// ":legacy"

    صحيح		cmod_ورهايو;		// flags for win_split()
    صحيح		cmod_tab;		// > 0 when ":tab" was used
    regmatch_T	cmod_filter_regmatch;	// set by :filter /pat/
    صحيح		cmod_filter_force;	// set for :filter!

    صحيح		cmod_verbose;		// 0 if not set, > 0 to set 'verbose'
					// to cmod_verbose - 1

    // values for undo_cmdmod()
    char_u	*cmod_save_ei;		// saved value of 'eventignore'
#جيڪڏهن_وصف_ٿيل HAVE_SANDBOX
    صحيح		cmod_did_sandbox;	// set when "sandbox" was incremented
#جيڪڏهن_پورو
    ڊگهو	cmod_verbose_save;	// if 'verbose' was set: value of
					// p_verbose plus one
    صحيح		cmod_save_نياپو_silent;	// if non-zero: saved value of
					// msg_silent + 1
    صحيح		cmod_save_نياپو_scroll;	// for restoring msg_scroll
    صحيح		cmod_did_esilent;	// incremented when emsg_silent is
} cmdmod_T;

تعريفنوع تعداد {
    MF_DIRTY_NO = 0,		// no dirty blocks
    MF_DIRTY_YES,		// there are dirty blocks
    MF_DIRTY_YES_NOSYNC,	// there are dirty blocks, do not sync yet
} mfdirty_T;

#وصف_ڪريو MF_ٻج_LEN	8

بنية memfile
{
    char_u	*mf_fname;		// name of the file
    char_u	*mf_ffname;		// idem, full path
    صحيح		mf_fd;			// file descriptor
    صحيح		mf_flags;		// flags used when opening this memfile
    صحيح		mf_reopen;		// mf_fd was closed, retry opening
    bhdr_T	*mf_free_پهريون;		// first block_hdr in free list
    bhdr_T	*mf_استعمال_ٿيل_پهريون;		// mru block_hdr in used list
    bhdr_T	*mf_استعمال_ٿيل_آخري;		// lru block_hdr in used list
    بنانشان	mf_استعمال_ٿيل_ڳڻپ;		// number of pages in used list
    بنانشان	mf_استعمال_ٿيل_ڳڻپ_max;	// maximum number of pages in memory
    mf_hashtab_T mf_hash;		// hash lists
    mf_hashtab_T mf_trans;		// trans lists
    blocknr_T	mf_blocknr_max;		// highest positive block number + 1
    blocknr_T	mf_blocknr_min;		// lowest negative block number - 1
    blocknr_T	mf_neg_ڳڻپ;		// number of negative blocks numbers
    blocknr_T	mf_infile_ڳڻپ;	// number of pages in the file
    بنانشان	mf_صفحو_سائيز;		// number of bytes in a page
    mfdirty_T	mf_dirty;
#جيڪڏهن_وصف_ٿيل FEAT_CRYPT
    buf_T	*mf_بفر;		// buffer this memfile is for
    char_u	mf_ٻج[MF_ٻج_LEN];	// seed for encryption

    // Values for key, method and seed used for reading data blocks when
    // updating for a newly set key or method. Only when mf_old_key != NULL.
    char_u	*mf_old_ڪنجي;
    صحيح		mf_old_cm;
    char_u	mf_old_ٻج[MF_ٻج_LEN];
#جيڪڏهن_پورو
};

/*
 * things used in memline.c
 */
/*
 * When searching for a specific line, we remember what blocks in the tree
 * are the branches leading to that block. This is stored in ml_stack.  Each
 * entry is a pointer to info in a block (may be data block or pointer block)
 */
تعريفنوع بنية info_اشارو
{
    blocknr_T	ip_bnum;	// block number
    linenr_T	ip_هيٺيون;		// lowest lnum in this block
    linenr_T	ip_مٿاهون;	// highest lnum in this block
    صحيح		ip_اشاريو;	// index for block with current lnum
} infoptr_T;	// block/index pair

#جيڪڏهن_وصف_ٿيل FEAT_BYTEOFF
تعريفنوع بنية ml_chunksize
{
    صحيح		mlcs_numlines;
    ڊگهو	mlcs_totalsize;
} chunksize_T;

/*
 * Flags when calling ml_updatechunk()
 */
# define ML_CHNK_ADDLINE 1
# define ML_CHNK_DELLINE 2
# define ML_CHNK_UPDLINE 3
#جيڪڏهن_پورو

/*
 * the memline structure holds all the information about a memline
 */
تعريفنوع بنية memline
{
    linenr_T	ml_سٽ_ڳڻپ;	// number of lines in the buffer

    memfile_T	*ml_mfp;	// pointer to associated memfile

    infoptr_T	*ml_ڍير;	// stack of pointer blocks (array of IPTRs)
    صحيح		ml_ڍير_چوٽي;	// current top of ml_stack
    صحيح		ml_ڍير_سائيز;	// total number of entries in ml_stack

#وصف_ڪريو ML_خالي_جايون	0x01	// empty buffer
#وصف_ڪريو ML_سٽ_DIRTY	0x02	// cached line was changed and allocated
#وصف_ڪريو ML_LOCKED_DIRTY	0x04	// ml_locked was changed
#وصف_ڪريو ML_LOCKED_POS	0x08	// ml_locked needs positive block number
#وصف_ڪريو ML_ALLOCATED	0x10	// ml_line_ptr is an allocated copy
    صحيح		ml_flags;

    colnr_T	ml_سٽ_len;	// length of the cached line + NUL + text properties
    colnr_T	ml_سٽ_textlen;// length of the cached line + NUL, 0 if not known yet
    linenr_T	ml_سٽ_lnum;	// line number of cached line, 0 if not valid
    char_u	*ml_سٽ_ptr;	// pointer to cached line

    bhdr_T	*ml_locked;	// block used by last ml_get
    linenr_T	ml_locked_هيٺيون;	// first line in ml_locked
    linenr_T	ml_locked_مٿاهون;	// last line in ml_locked
    صحيح		ml_locked_lineadd;  // number of lines inserted in ml_locked
#جيڪڏهن_وصف_ٿيل FEAT_BYTEOFF
    chunksize_T *ml_chunksize;
    صحيح		ml_numchunks;
    صحيح		ml_usedchunks;
#جيڪڏهن_پورو
} memline_T;

// Values for the flags argument of ml_delete_flags().
#وصف_ڪريو ML_DEL_پيغام	    1	// may give a "No lines in buffer" message
#وصف_ڪريو ML_DEL_UNDO	    2	// called from undo, do not update textprops
#وصف_ڪريو ML_DEL_NOPROP	    4	// splitting data block, do not update textprops

// Values for the flags argument of ml_append_int().
#وصف_ڪريو ML_APPEND_NEW	    1	// starting to edit a new file
#وصف_ڪريو ML_APPEND_MARK	    2	// mark the new line
#وصف_ڪريو ML_APPEND_UNDO	    4	// called from undo
#وصف_ڪريو ML_APPEND_NOPROP    8	// do not continue textprop from previous line


/*
 * Structure defining text properties.  These stick with the text.
 * When stored in memline they are after the text, ml_line_len is larger than
 * STRLEN(ml_line_ptr) + 1.
 */
تعريفنوع بنية textprop_S
{
    colnr_T	tp_کانو;		// start column (one based, in bytes)
    colnr_T	tp_len;		// length in bytes; for virtual text props
				// this is STRLEN(vtext) (not including NUL)
    صحيح		tp_id;		// identifier
    صحيح		tp_type;	// property type
    صحيح		tp_flags;	// TP_FLAG_ values
    صحيح		tp_padleft;	// left padding between text line and virtual
				//متن
    اتحاد			// For virtual text props (tp_id < 0):
    {				// check TP_FLAG_VTEXT_PTR in tp_flags to
				// determine which member is active.
	colnr_T	tp_متن_وٿي; // offset to vtext string from the
				// prop_count position in the memline
				// (when TP_FLAG_VTEXT_PTR is NOT set)
	char_u	*tp_متن;	// pointer to virtual text string
				// (when TP_FLAG_VTEXT_PTR IS set)
    } u;
} textprop_T;

#وصف_ڪريو TP_FLAG_CONT_ايندڙ	0x1	// property continues in next line
#وصف_ڪريو TP_FLAG_CONT_PREV	0x2	// property was continued from prev line

// without these text is placed after the end of the line
#وصف_ڪريو TP_FLAG_ALIGN_ساڄو	0x010	// virtual text is right-aligned
#وصف_ڪريو TP_FLAG_ALIGN_ABOVE	0x020	// virtual text above the line
#وصف_ڪريو TP_FLAG_ALIGN_هيٺ	0x040	// virtual text on next screen line

#وصف_ڪريو TP_FLAG_WRAP		0x080	// virtual text wraps - when missing
					// text is truncated
#وصف_ڪريو TP_FLAG_شروعات_INCL	0x100	// "start_incl" copied from proptype
#وصف_ڪريو TP_FLAG_DELETED		0x200	// marked for deletion in
					// unpacked_memline_T
#وصف_ڪريو TP_FLAG_VTEXT_PTR	0x400	// u.tp_text access is valid

#وصف_ڪريو PROP_ڳڻپ_سائيز	حجم(uint16_t)    // size of prop_count in memline

#وصف_ڪريو PROP_متن_MIN_CELLS	4	// minimum number of cells to use for
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
تعريفنوع بنية unpacked_memline_S
{
    buf_T	*buf;		// the line's buffer (NULL = closed)
    linenr_T	lnum;		// line number (0 = no line loaded)
    bool	detached;	// true when text/vtext are allocated
    colnr_T	متن_سائيز_2;	// size of text including NUL
    char_u	*متن;		// NUL-terminated text
    صحيح		prop_سائيز;	// number of allocated prop slots
    صحيح		prop_ڳڻپ;	// number of properties
    textprop_T	*props;		// property array
    bool	متن_بدليو;	// true if text was modified
} unpacked_memline_T;

/*
 * Structure defining a property type.
 */
تعريفنوع بنية proptype_S
{
    صحيح		pt_id;		// value used for tp_id
    صحيح		pt_type;	// number used for tp_type
    صحيح		pt_hl_id;	// highlighting
    صحيح		pt_ترجيح;	//ترجيح
    صحيح		pt_flags;	// PT_FLAG_ values
    char_u	pt_name[1];	// property type name, actually longer
} proptype_T;

#وصف_ڪريو PT_FLAG_INS_شروعات_INCL	1	// insert at start included in property
#وصف_ڪريو PT_FLAG_INS_پڇاڙي_INCL	2	// insert at end included in property
#وصف_ڪريو PT_FLAG_COMBINE		4	// combine with syntax highlight
#وصف_ڪريو PT_FLAG_OVERRIDE	8	// override any highlight

// Sign group
تعريفنوع بنية signgroup_S
{
    صحيح		sg_ايندڙ_sign_id;	// next sign id for this group
    short_u	sg_refcount;		// number of signs in this group
    char_u	sg_name[1];		// sign group name, actually longer
} signgroup_T;

تعريفنوع بنية sign_داخلا sign_داخلا_T;
بنية sign_داخلا
{
    صحيح		 se_id;		// unique identifier for each placed sign
    صحيح		 se_typenr;	// typenr of sign
    صحيح		 se_ترجيح;	// priority for highlighting
    linenr_T	 se_lnum;	// line number which has this sign
    signgroup_T	 *se_گروهه;	// sign group
    sign_داخلا_T *se_ايندڙ;	// next entry in a list of signs
    sign_داخلا_T *se_prev;	// previous entry -- for easy reordering
};

/*
 * Sign attributes. Used by the screen refresh routines.
 */
تعريفنوع بنية sign_attrs_S {
    صحيح		sat_typenr;
    فراغ	*sat_icon;
    char_u	*sat_متن;
    صحيح		sat_texthl;
    صحيح		sat_linehl;
    صحيح		sat_culhl;
    صحيح		sat_numhl;
    صحيح		sat_ترجيح;
} sign_attrs_T;

#إذا defined(FEAT_SIGNS)
// Macros to get the sign group structure from the group name
# define SGN_ڪنجي_OFF	offsetof(signgroup_T, sg_name)
# define HI2SG(hi)	((signgroup_T *)((hi)->hi_ڪنجي - SGN_ڪنجي_OFF))

// Default sign priority for highlighting
# define SIGN_DEF_PRIO	10

#جيڪڏهن_پورو

/*
 * Argument list: Array of file names.
 * Used for the global argument list and the argument lists local to a window.
 */
تعريفنوع بنية arglist
{
    garray_T	al_ga;		// growarray with the array of file names
    صحيح		al_refcount;	// number of windows using this arglist
    صحيح		id;		// id of this arglist
} alist_T;

/*
 * For each argument remember the file name as it was given, and the buffer
 * number that contains the expanded file name (required for when ":cd" is
 * used).
 */
تعريفنوع بنية argentry
{
    char_u	*ae_fname;	// file name as specified
    صحيح		ae_fnum;	// buffer number with expanded file name
} aentry_T;

#وصف_ڪريو ALIST(win)	(win)->w_alist
#وصف_ڪريو GARGLIST	((aentry_T *)global_alist.al_ga.ga_معلومات)
#وصف_ڪريو ARGLIST		((aentry_T *)ALIST(curwin)->al_ga.ga_معلومات)
#وصف_ڪريو WARGLIST(wp)	((aentry_T *)ALIST(wp)->al_ga.ga_معلومات)
#وصف_ڪريو AARGLIST(al)	((aentry_T *)((al)->al_ga.ga_معلومات))
#وصف_ڪريو GARGCOUNT	(global_alist.al_ga.ga_len)
#وصف_ڪريو ARGCOUNT	(ALIST(curwin)->al_ga.ga_len)
#وصف_ڪريو WARGCOUNT(wp)	(ALIST(wp)->al_ga.ga_len)

/*
 * A list used for saving values of "emsg_silent".  Used by ex_try() to save the
 * value of "emsg_silent" if it was non-zero.  When this is done, the CSF_SILENT
 * flag below is set.
 */

تعريفنوع بنية eslist_elem eslist_T;
بنية eslist_elem
{
    صحيح		saved_emsg_silent;	// saved value of "emsg_silent"
    eslist_T	*ايندڙ;			// next element on the list
};

/*
 * For conditional commands a stack is kept of nested conditionals.
 * When cs_idx < 0, there is no conditional command.
 */
#وصف_ڪريو CSTACK_LEN	50

تعريفنوع بنية {
    ننڍو	cs_flags[CSTACK_LEN];	// CSF_ flags
    حرف	cs_pending[CSTACK_LEN];	// CSTP_: what's pending in ":finally"
    اتحاد {
	فراغ	*csp_rv[CSTACK_LEN];	// return typeval for pending return
	فراغ	*csp_ex[CSTACK_LEN];	// exception for pending throw
    }		cs_pend;
    فراغ	*cs_forinfo[CSTACK_LEN]; // info used by ":for"
    صحيح		cs_سٽ[CSTACK_LEN];	// line nr of ":while"/":for" line
    صحيح		cs_بلاڪ_id[CSTACK_LEN];    // block ID stack
    صحيح		cs_script_var_len[CSTACK_LEN];	// value of sn_var_vals.ga_len
						// when entering the block
    صحيح		cs_idx;			// current entry, or -1 if none
    صحيح		cs_looplevel;		// nr of nested ":while"s and ":for"s
    صحيح		cs_trylevel;		// nr of nested ":try"s
    eslist_T	*cs_emsg_silent_list;	// saved values of "emsg_silent"
    حرف	cs_lflags;		// loop flags: CSL_ flags
} cstack_T;
#وصف_ڪريو cs_rettv	cs_pend.csp_rv
#وصف_ڪريو cs_exception	cs_pend.csp_ex

// There is no CSF_IF, the lack of CSF_WHILE, CSF_FOR and CSF_TRY means ":if"
// was used.
#وصف_ڪريو CSF_TRUE	0x0001	// condition was TRUE
#وصف_ڪريو CSF_سرگرم	0x0002	// current state is active
#وصف_ڪريو CSF_ELSE	0x0004	// ":else" has been passed
#وصف_ڪريو CSF_WHILE	0x0008	// is a ":while"
#وصف_ڪريو CSF_FOR	0x0010	// is a ":for"
#وصف_ڪريو CSF_بلاڪ	0x0020	// is a "{" block

#وصف_ڪريو CSF_TRY	0x0100	// is a ":try"
#وصف_ڪريو CSF_FINALLY	0x0200	// ":finally" has been passed
#وصف_ڪريو CSF_CATCH	0x0400	// ":catch" has been seen
#وصف_ڪريو CSF_THROWN	0x0800	// exception thrown to this try conditional
#وصف_ڪريو CSF_CAUGHT	0x1000  // exception caught by this try conditional
#وصف_ڪريو CSF_ختم	0x2000  // CSF_CAUGHT was handled by finish_exception()
#وصف_ڪريو CSF_SILENT	0x4000	// "emsg_silent" reset by ":try"
// Note that CSF_ELSE is only used when CSF_TRY and CSF_WHILE are unset
// (an ":if"), and CSF_SILENT is only used when CSF_TRY is set.

#وصف_ڪريو CSF_FUNC_DEF	0x8000	// a function was defined in this block

/*
 * What's pending for being reactivated at the ":endtry" of this try
 * conditional:
 */
#وصف_ڪريو CSTP_NONE	0	// nothing pending in ":finally" clause
#وصف_ڪريو CSTP_ERROR	1	// an error is pending
#وصف_ڪريو CSTP_مداخلت	2	// an interrupt is pending
#وصف_ڪريو CSTP_THROW	4	// a throw is pending
#وصف_ڪريو CSTP_BREAK	8	// ":break" is pending
#وصف_ڪريو CSTP_CONTINUE	16	// ":continue" is pending
#وصف_ڪريو CSTP_RETURN	24	// ":return" is pending
#وصف_ڪريو CSTP_پڄاڻي	32	// ":finish" is pending

/*
 * Flags for the cs_lflags item in cstack_T.
 */
#وصف_ڪريو CSL_HAD_LOOP	 1	// just found ":while" or ":for"
#وصف_ڪريو CSL_HAD_ENDLOOP 2	// just found ":endwhile" or ":endfor"
#وصف_ڪريو CSL_HAD_CONT	 4	// just found ":continue"
#وصف_ڪريو CSL_HAD_FINA	 8	// just found ":finally"

/*
 * A list of error messages that can be converted to an exception.  "throw_msg"
 * is only set in the first element of the list.  Usually, it points to the
 * original message stored in that element, but sometimes it points to a later
 * message in the list.  See cause_errthrow().
 */
تعريفنوع بنية msglist msglist_T;
بنية msglist
{
    msglist_T	*ايندڙ;		// next of several messages in a row
    حرف	*نياپو;		// original message, allocated
    حرف	*throw_نياپو;	// msg to throw: usually original one
    char_u	*sfile;		// value from estack_sfile(), allocated
    ڊگهو	slnum;		// line number for "sfile"
    صحيح		نياپو_compiling;	// saved value of estack_compiling
};

/*
 * The exception types.
 */
تعريفنوع تعداد
{
    ET_USER,		// exception caused by ":throw" command
    ET_ERROR,		// error exception
    ET_مداخلت,	// interrupt exception triggered by Ctrl-C
} except_type_T;

/*
 * Structure describing an exception.
 * (don't use "struct exception", it's used by the math library).
 */
تعريفنوع بنية vim_exception except_T;
بنية vim_exception
{
    except_type_T	type;		// exception type
    حرف		*قدر;		// exception value
    بنية msglist	*messages;	// message(s) causing error exception
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
تعريفنوع بنية cleanup_stuff cleanup_T;
بنية cleanup_stuff
{
    صحيح pending;		// error/interrupt/exception state
    except_T *exception;	// exception value
};

/*
 * Exception state that is saved and restored when calling timer callback
 * functions and deferred functions.
 */
تعريفنوع بنية exception_state_S exception_state_T;
بنية exception_state_S
{
    except_T	*estate_current_exception;
    صحيح		estate_did_throw;
    صحيح		estate_need_rethrow;
    صحيح		estate_trylevel;
    صحيح		estate_did_emsg;
};

#جيڪڏهن_وصف_ٿيل FEAT_SYN_HL
// struct passed to in_id_list()
بنية sp_syn
{
    صحيح		inc_ٽيگ;	// ":syn include" unique tag
    ننڍو	id;		// highlight group ID of item
    ننڍو	*cont_اندريون_list;	// cont.in group IDs, if non-zero
};

/*
 * Each keyword has one keyentry, which is linked in a hash list.
 */
تعريفنوع بنية keyentry keyentry_T;

بنية keyentry
{
    keyentry_T	*ke_ايندڙ;	// next entry with identical "keyword[]"
    بنية sp_syn k_syn;	// struct passed to in_id_list()
    ننڍو	*ايندڙ_list;	// ID list for next match (if non-zero)
    صحيح		flags;
    صحيح		k_char;		// conceal substitute character
    char_u	keyword[1];	// actually longer
};

/*
 * Struct used to store one state of the state stack.
 */
تعريفنوع بنية buf_state
{
    صحيح		    bs_idx;	 // index of pattern
    صحيح		    bs_flags;	 // flags for pattern
# ifdef FEAT_CONCEAL
    صحيح		    bs_seqnr;	 // stores si_seqnr
    صحيح		    bs_cchar;	 // stores si_cchar
# endif
    reg_extmatch_T *bs_extmatch; // external matches from start pattern
} bufstate_T;

/*
 * syn_state contains the syntax state stack for the start of one line.
 * Used by b_sst_array[].
 */
تعريفنوع بنية syn_state synstate_T;

بنية syn_state
{
    synstate_T	*sst_ايندڙ;	// next entry in used or free list
    linenr_T	sst_lnum;	// line number for this state
    اتحاد
    {
	bufstate_T	sst_ڍير[SST_FIX_حالتون]; // short state stack
	garray_T	sst_ga;	// growarray for long state stack
    } sst_union;
    صحيح		sst_ايندڙ_flags;	// flags for sst_next_list
    صحيح		sst_stacksize;	// number of states on the stack
    ننڍو	*sst_ايندڙ_list;	// "nextgroup" list in this state
				// (this is a copy, don't free it!
    disptick_T	sst_tick;	// tick when last displayed
    linenr_T	sst_change_lnum;// when non-zero, change in this line
				// may have made the state invalid
};
#جيڪڏهن_پورو // FEAT_SYN_HL

#وصف_ڪريو MAX_HL_ID       20000	// maximum value for a highlight ID.

/*
 * Structure shared between syntax.c, screen.c and gui_x11.c.
 */
تعريفنوع بنية attr_داخلا
{
    ننڍو	    ae_attr;		// HL_BOLD, etc.
    اتحاد
    {
	بنية
	{
	    char_u	    *شروعات;	// start escape sequence
	    char_u	    *stop;	// stop escape sequence
	} رڪن;
	بنية
	{
	    // These colors need to be > 8 bits to hold 256.
	    short_u	    fg_color;	// foreground color number
	    short_u	    bg_color;	// background color number
	    short_u	    ul_color;	// underline color number
	    short_u	    font;	// font number
#جيڪڏهن_وصف_ٿيل FEAT_TERMGUICOLORS
	    guicolor_T	    fg_rgb;	// foreground color RGB
	    guicolor_T	    bg_rgb;	// background color RGB
	    guicolor_T	    ul_rgb;	// underline color RGB
#جيڪڏهن_پورو
	} cterm;
#جيڪڏهن_وصف_ٿيل FEAT_GUI
	بنية
	{
	    guicolor_T	    fg_color;	// foreground color handle
	    guicolor_T	    bg_color;	// background color handle
	    guicolor_T	    sp_color;	// special color handle
	    GuiFont	    font;	// font handle
# ifdef FEAT_XFONTSET
	    GuiFontset	    fontset;	// fontset handle
# endif
	} gui;
#جيڪڏهن_پورو
    } ae_u;
} attrentry_T;

#جيڪڏهن_وصف_ٿيل USE_ICONV
# ifdef HAVE_ICONV_H
#  include <iconv.h>
# وإلا
#  إذا defined(MACOS_X)
#   include <sys/errno.h>
#   ifndef EILSEQ
#    define EILSEQ ENOENT // Early MacOS X does not have EILSEQ
#   endif
تعريفنوع بنية _iconv_t *iconv_t;
#  وإلا
#   include <errno.h>
#  endif
تعريفنوع فراغ *iconv_t;
# endif
#جيڪڏهن_پورو

/*
 * Used for the typeahead buffer: typebuf.
 */
تعريفنوع بنية
{
    char_u	*tb_buf;	// buffer for typed characters
    char_u	*tb_noremap;	// mapping flags for characters in tb_buf[]
    صحيح		tb_buflen;	// size of tb_buf[]
    صحيح		tb_off;		// current position in tb_buf[]
    صحيح		tb_len;		// number of valid bytes in tb_buf[]
    صحيح		tb_maplen;	// nr of mapped bytes in tb_buf[]
    صحيح		tb_silent;	// nr of silently mapped bytes in tb_buf[]
    صحيح		tb_no_abbr_cnt; // nr of bytes without abbrev. in tb_buf[]
    صحيح		tb_change_cnt;	// nr of time tb_buf was changed; never zero
} typebuf_T;

// Struct to hold the saved typeahead for save_typeahead().
تعريفنوع بنية
{
    typebuf_T		save_typebuf;
    صحيح			typebuf_valid;	    // TRUE when save_typebuf valid
    صحيح			old_char;
    صحيح			old_mod_نقاب;
    buffheader_T	save_readbuf1;
    buffheader_T	save_readbuf2;
#جيڪڏهن_وصف_ٿيل USE_داخلا_BUF
    char_u		*save_inputbuf;
#جيڪڏهن_پورو
} tasave_T;

/*
 * Used for conversion of terminal I/O and script files.
 */
تعريفنوع بنية
{
    صحيح		vc_type;	// zero or one of the CONV_ values
    صحيح		vc_ضرب_ڪندڙ;	// max. expansion factor
#جيڪڏهن_وصف_ٿيل MSWIN
    صحيح		vc_cpfrom;	// codepage to convert from (CONV_CODEPAGE)
    صحيح		vc_cpto;	// codepage to convert to (CONV_CODEPAGE)
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل USE_ICONV
    iconv_t	vc_fd;		// for CONV_ICONV
#جيڪڏهن_پورو
    صحيح		vc_fail;	// fail for invalid char, don't use '?'
} vimconv_T;

/*
 * Structure used for the command line history.
 */
تعريفنوع بنية hist_داخلا
{
    صحيح		hisnum;		// identifying number
    صحيح		viminfo;	// when TRUE hisstr comes from viminfo
    char_u	*hisstr;	// actual entry, separator char after the NUL
    size_t	hisstrlen;	// length of hisstr (excluding the NUL)
    time_t	وقت_سيٽ;	// when it was typed, zero if unknown
} histentry_T;

#وصف_ڪريو CONV_NONE		0
#وصف_ڪريو CONV_منزل_UTF8		1
#وصف_ڪريو CONV_9_منزل_UTF8		2
#وصف_ڪريو CONV_منزل_LATIN1		3
#وصف_ڪريو CONV_منزل_LATIN9		4
#وصف_ڪريو CONV_ICONV		5
#جيڪڏهن_وصف_ٿيل MSWIN
# define CONV_CODEPAGE		10	// codepage -> codepage
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل MACOS_X
# define CONV_MAC_LATIN1	20
# define CONV_LATIN1_MAC	21
# define CONV_MAC_UTF8		22
# define CONV_UTF8_MAC		23
#جيڪڏهن_پورو

/*
 * Structure used for mappings and abbreviations.
 */
تعريفنوع بنية mapblock mapblock_T;
بنية mapblock
{
    mapblock_T	*m_ايندڙ;	// next mapblock in list
    mapblock_T	*m_alt;		// pointer to mapblock of the same mapping
				// with an alternative form of m_keys, or NULL
				// if there is no such mapblock
    char_u	*m_ڪنجيون;	// mapped from, lhs
    char_u	*m_str;		// mapped to, rhs
    char_u	*m_orig_str;	// rhs as entered by the user
    صحيح		m_keylen;	// strlen(m_keys)
    صحيح		m_گهڻائي;		// valid mode
    صحيح		m_simplified;	// m_keys was simplified, do not use this map
				// if key_protocol_enabled() returns TRUE
    صحيح		m_noremap;	// if non-zero no re-mapping for m_str
    حرف	m_silent;	// <silent> used, don't echo commands
    حرف	m_nowait;	// <nowait> used
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
    حرف	m_اظهار;		// <expr> used, m_str is an expression
    sctx_T	m_script_ctx;	// SCTX where map was defined
#جيڪڏهن_پورو
};


/*
 * Used for highlighting in the status line.
 */
تعريفنوع بنية
{
    char_u	*شروعات;
    صحيح		userhl;		// 0: no HL, 1-9: User HL, < 0 for syn ID
} stl_hlrec_T;

/*
 * Used for statusline click function regions.
 */
تعريفنوع بنية {
    char_u	*شروعات;		// position in output buffer where region starts
    char_u	*funcname;	// function name (NULL = end/close marker)
    صحيح		minwid;		// minwid value from %N@Func@
} stl_clickrec_T;

/*
 * Per-window resolved click regions (screen column based).
 */
تعريفنوع بنية {
    صحيح		صف;		// screen row where region lives
    صحيح		کانو_شروعات;	// screen column where region starts
    صحيح		کانو_پڇاڙي;	// screen column where region ends
    char_u	*funcname;	// function name (allocated copy)
    صحيح		minwid;		// minwid value
    صحيح		tabnr;		// tab page number (tabpanel only, 0 otherwise)
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
تعريفنوع بنية hashitem_S
{
    long_u	hi_hash;	// cached hash number of hi_key
    char_u	*hi_ڪنجي;
} hashitem_T;

// The address of "hash_removed" is used as a magic number for hi_key to
// indicate a removed item.
#وصف_ڪريو HI_ڪنجي_REMOVED &hash_removed
#وصف_ڪريو HASHITEM_خالي_جايون(hi) ((hi)->hi_ڪنجي == عدم || (hi)->hi_ڪنجي == &hash_removed)

// Initial size for a hashtable.  Our items are relatively small and growing
// is expensive, thus use 16 as a start.  Must be a power of 2.
// This allows for storing 10 items (2/3 of 16) before a resize is needed.
#وصف_ڪريو HT_INIT_سائيز 16

// flags used for ht_flags
#وصف_ڪريو HTFLAGS_ERROR	0x01	// Set when growing failed, can't add more
				// items before growing works.
#وصف_ڪريو HTFLAGS_FROZEN	0x02	// Trying to add or remove an item will result
				// in an error message.

تعريفنوع بنية hashtable_S
{
    long_u	ht_نقاب;	// mask used for hash value (nr of items in
				// array is "ht_mask" + 1)
    long_u	ht_استعمال_ٿيل;	// number of items used
    long_u	ht_filled;	// number of items used + removed
    صحيح		ht_بدليو;	// incremented when adding or removing an item
    صحيح		ht_locked;	// counter for hash_lock()
    صحيح		ht_flags;	// HTFLAGS_ values
    hashitem_T	*ht_array;	// points to the array, allocated when it's
				// not "ht_smallarray"
    hashitem_T	ht_smallarray[HT_INIT_سائيز];   // initial array
} hashtab_T;

تعريفنوع long_u hash_T;		// Type for hi_hash


// Use 64-bit Number.
#جيڪڏهن_وصف_ٿيل MSWIN
   تعريفنوع __int64		varnumber_T;
   تعريفنوع بنانشان __int64	uvarnumber_T;
# define VARNUM_MIN		_I64_MIN
# define VARNUM_MAX		_I64_MAX
# define UVARNUM_MAX		_UI64_MAX
#وإلاإذا defined(HAVE_NO_LONG_LONG)
# إذا defined(HAVE_STDINT_H)
   تعريفنوع int64_t		varnumber_T;
   تعريفنوع uint64_t		uvarnumber_T;
#  define VARNUM_MIN		INT64_MIN
#  define VARNUM_MAX		INT64_MAX
#  define UVARNUM_MAX		UINT64_MAX
# وإلا
   // this may cause trouble for code that depends on 64 bit ints
   تعريفنوع ڊگهو			varnumber_T;
   تعريفنوع بنانشان ڊگهو	uvarnumber_T;
#  define VARNUM_MIN		LONG_MIN
#  define VARNUM_MAX		LONG_MAX
#  define UVARNUM_MAX		ULONG_MAX
# endif
#وإلا
  تعريفنوع ڊگهو ڊگهو		varnumber_T;
  تعريفنوع بنانشان ڊگهو ڊگهو	uvarnumber_T;
# ifdef LLONG_MIN
#  define VARNUM_MIN		LLONG_MIN
#  define VARNUM_MAX		LLONG_MAX
#  define UVARNUM_MAX		ULLONG_MAX
# وإلا
#  define VARNUM_MIN		LONG_LONG_MIN
#  define VARNUM_MAX		LONG_LONG_MAX
#  define UVARNUM_MAX		ULONG_LONG_MAX
# endif
#جيڪڏهن_پورو

// Struct that holds both a normal function name and a partial_T, as used for a
// callback argument.
// When used temporarily "cb_name" is not allocated.  The refcounts to either
// the function or the partial are incremented and need to be decremented
// later with free_callback().
تعريفنوع بنية {
    char_u	*cb_name;
    partial_T	*cb_partial;
    صحيح		cb_free_name;	    // cb_name was allocated
} callback_T;

تعريفنوع بنية isn_S isn_T;	    // instruction
تعريفنوع بنية dfunc_S dfunc_T;	    // :def function

تعريفنوع بنية type_S type_T;
تعريفنوع بنية ufunc_S ufunc_T;

تعريفنوع بنية jobvar_S ڪم_T;
تعريفنوع بنية readq_S readq_T;
تعريفنوع بنية writeq_S writeq_T;
تعريفنوع بنية jsonq_S jsonq_T;
تعريفنوع بنية cbq_S cbq_T;
تعريفنوع بنية channel_S channel_T;
تعريفنوع بنية cctx_S cctx_T;
تعريفنوع بنية ectx_S ectx_T;
تعريفنوع بنية instr_S instr_T;
تعريفنوع بنية class_S class_T;
تعريفنوع بنية object_S object_T;
تعريفنوع بنية typealias_S typealias_T;

تعريفنوع تعداد
{
    VAR_UNKNOWN = 0,	// not set, any type or "void" allowed
    VAR_ANY,		// used for "any" type
    VAR_VOID,		// no value (function not returning anything)
    VAR_BOOL,		// "v_number" is used: VVAL_TRUE or VVAL_FALSE
    VAR_SPECIAL,	// "v_number" is used: VVAL_NULL or VVAL_NONE
    VAR_عدد,		// "v_number" is used
    VAR_FLOAT,		// "v_float" is used
    VAR_STRING,		// "v_string" is used
    VAR_BLOB,		// "v_blob" is used
    VAR_FUNC,		// "v_string" is function name
    VAR_PARTIAL,	// "v_partial" is used
    VAR_LIST,		// "v_list" is used
    VAR_DICT,		// "v_dict" is used
    VAR_ڪم,		// "v_job" is used
    VAR_CHANNEL,	// "v_channel" is used
    VAR_INSTR,		// "v_instr" is used
    VAR_CLASS,		// "v_class" is used (also used for interface)
    VAR_OBJECT,		// "v_object" is used
    VAR_TYPEALIAS,	// "v_typealias" is used
    VAR_TUPLE		// "v_tuple" is used
} vartype_T;

// A type specification.
بنية type_S {
    vartype_T	    tt_type;
    صحيح	    tt_argcount;    // for func, incl. vararg, -1 for unknown
    صحيح	    tt_min_argcount; // number of non-optional arguments
    short_u	    tt_flags;	    // TTFLAG_ values
    type_T	    *tt_member;	    // for list, dict, func return type
    class_T	    *tt_class;	    // for class and object
    type_T	    **tt_دليلون;	    // func argument types, allocated
};

تعريفنوع بنية {
    type_T	*type_curr;	    // current type, value type
    type_T	*type_decl;	    // declared type or equal to type_current
} type2_T;

#وصف_ڪريو TTFLAG_VARARGS	    0x01    // func args ends with "..."
#وصف_ڪريو TTFLAG_BOOL_OK	    0x02    // can be converted to bool
#وصف_ڪريو TTFLAG_FLOAT_OK	    0x04    // number can be used/converted to float
#وصف_ڪريو TTFLAG_عدد_OK    0x08    // number can be used for a float
#وصف_ڪريو TTFLAG_STATIC	    0x10    // one of the static types, e.g. t_any
#وصف_ڪريو TTFLAG_CONST	    0x20    // cannot be changed
#وصف_ڪريو TTFLAG_SUPER	    0x40    // object from "super".
#وصف_ڪريو TTFLAG_GENERIC	    0x80    // generic type
#وصف_ڪريو TTFLAG_TUPLE_OK	    0x100   // tuple can be used for a list

#وصف_ڪريو IS_GENERIC_TYPE(type)	\
    ((type->tt_flags & TTFLAG_GENERIC) == TTFLAG_GENERIC)

// Type check flags
#وصف_ڪريو TYPECHK_عدد_OK	0x1	// number is accepted for a float
#وصف_ڪريو TYPECHK_TUPLE_OK	0x2	// tuple is accepted for a list

تعريفنوع تعداد {
    VIM_ACCESS_ذاتي,	// read/write only inside the class
    VIM_ACCESS_READ,	// read everywhere, write only inside the class
    VIM_ACCESS_ALL	// read/write everywhere
} omacc_T;

#وصف_ڪريو OCMFLAG_HAS_TYPE	0x01	// type specified explicitly
#وصف_ڪريو OCMFLAG_FINAL		0x02	// "final" object/class member
#وصف_ڪريو OCMFLAG_CONST		0x04	// "const" object/class member

/*
 * Object methods called by builtin functions (e.g. string(), empty(), etc.)
 */
تعريفنوع تعداد {
    CLASS_BUILTIN_INVALID,
    CLASS_BUILTIN_STRING,
    CLASS_BUILTIN_خالي_جايون,
    CLASS_BUILTIN_LEN,
    CLASS_BUILTIN_MAX
} class_builtin_T;

/*
 * Entry for an object or class member variable.
 */
تعريفنوع بنية {
    string_T	ocm_name;	// allocated
    omacc_T	ocm_access;
    type_T	*ocm_type;
    صحيح		ocm_flags;
    char_u	*ocm_init;	// allocated
    sctx_T	ocm_init_sctx;	// script context of the initializer expression
} ocmember_T;

// used for the lookup table of a class member index and object method index
تعريفنوع بنية itf2class_S itf2class_T;
بنية itf2class_S {
    itf2class_T	*i2c_ايندڙ;
    class_T	*i2c_class;
    صحيح		i2c_is_طريقو;	    // TRUE for method indexes
    // array with ints follows
};

#وصف_ڪريو CLASS_INTERFACE	    0x1
#وصف_ڪريو CLASS_EXTENDED	    0x2	    // another class extends this one
#وصف_ڪريو CLASS_ABSTRACT	    0x4	    // abstract class
#وصف_ڪريو CLASS_ENUM	    0x8	    // enum

// "class_T": used for v_class of typval of VAR_CLASS
// Also used for an interface (class_flags has CLASS_INTERFACE).
بنية class_S
{
    string_T	class_name;		// allocated
    صحيح		class_flags;		// CLASS_ flags

    صحيح		class_refcount;
    صحيح		class_copyID;		// used by garbage collection
    class_T	*class_ايندڙ_استعمال_ٿيل;	// for list headed by "first_class"
    class_T	*class_prev_استعمال_ٿيل;	// for list headed by "first_class"

    class_T	*class_extends;		// parent class or NULL

    // interfaces declared for the class
    صحيح		class_interface_ڳڻپ;
    char_u	**class_interfaces;	// allocated array of names
    class_T	**class_interfaces_cl;	// interfaces (counts as reference)
    itf2class_T	*class_itf2class;	// member index lookup tables

    // class members: "static varname"
    صحيح		class_class_member_ڳڻپ;
    ocmember_T	*class_class_members;	// allocated
    typval_T	*class_members_tv;	// allocated array of class member vals

    // class functions: "static def SomeMethod()"
    صحيح		class_class_function_ڳڻپ;	    // total count
    صحيح		class_class_function_ڳڻپ_ٻار;   // count without "extends"
    ufunc_T	**class_class_functions;	// allocated

    // object members: "this.varname"
    صحيح		class_obj_member_ڳڻپ;
    ocmember_T	*class_obj_members;	// allocated

    // object methods: "def SomeMethod()"
    صحيح		class_obj_طريقو_ڳڻپ;		    // total count
    صحيح		class_obj_طريقو_ڳڻپ_ٻار;	    // count without "extends"
    ufunc_T	**class_obj_methods;	// allocated

					// index of builtin methods
    صحيح		class_builtin_methods[CLASS_BUILTIN_MAX];

    garray_T	class_type_list;	// used for type pointers
    type_T	class_type;		// type used for the class
    type_T	class_object_type;	// same as class_type but VAR_OBJECT
};

#وصف_ڪريو IS_INTERFACE(cl)	((cl)->class_flags & CLASS_INTERFACE)
#وصف_ڪريو IS_ENUM(cl)		((cl)->class_flags & CLASS_ENUM)

// Used for v_object of typval of VAR_OBJECT.
// The member variables follow in an array of typval_T.
بنية object_S
{
    class_T	*obj_class;	    // class this object is created for;
				    // pointer adds to class_refcount
    صحيح		obj_refcount;

    object_T	*obj_ايندڙ_استعمال_ٿيل;	    // for list headed by "first_object"
    object_T	*obj_prev_استعمال_ٿيل;	    // for list headed by "first_object"
    صحيح		obj_copyID;	    // used by garbage collection
};

بنية typealias_S
{
    صحيح	    ta_refcount;
    type_T  *ta_type;
    char_u  *ta_name;
    صحيح	    ta_اونهائي;	    // number of levels of "ta_type"
};

/*
 * Structure to hold an internal variable without a name.
 */
بنية typval_S
{
    vartype_T	v_type;
    حرف	v_تالو;	    // see below: VAR_LOCKED, VAR_FIXED
    اتحاد
    {
	varnumber_T	v_عدد;	// number value
	float_T		v_float;	// floating point number value
	char_u		*v_string;	// string value (can be NULL)
	list_T		*v_list;	// list value (can be NULL)
	dict_T		*v_dict;	// dict value (can be NULL)
	partial_T	*v_partial;	// closure: function with args
#جيڪڏهن_وصف_ٿيل FEAT_ڪم_CHANNEL
	ڪم_T		*v_ڪم;		// job value (can be NULL)
	channel_T	*v_channel;	// channel value (can be NULL)
#جيڪڏهن_پورو
	blob_T		*v_blob;	// blob value (can be NULL)
	instr_T		*v_instr;	// instructions to execute
	class_T		*v_class;	// class value (can be NULL)
	object_T	*v_object;	// object value (can be NULL)
	typealias_T	*v_typealias;	// user-defined type name
	tuple_T		*v_tuple;	// tuple
    }		vval;
};

// Values for "dv_scope".
#وصف_ڪريو VAR_SCOPE     1	// a:, v:, s:, etc. scope dictionaries
#وصف_ڪريو VAR_DEF_SCOPE 2	// l:, g: scope dictionaries: here funcrefs are not
			// allowed to mask existing functions

// Values for "v_lock".
#وصف_ڪريو VAR_LOCKED	    1	// locked with lock(), can use unlock()
#وصف_ڪريو VAR_FIXED	    2	// locked forever
#وصف_ڪريو VAR_شيون_LOCKED    4	// items of non-materialized list locked

/*
 * Structure to hold an item of a list: an internal variable without a name.
 */
تعريفنوع بنية listitem_S listitem_T;

بنية listitem_S
{
    listitem_T	*li_ايندڙ;	// next item in list
    listitem_T	*li_prev;	// previous item in list
    typval_T	li_tv;		// type and value of the variable
};

// Struct used by those that are using an item in a list.
تعريفنوع بنية listwatch_S listwatch_T;

بنية listwatch_S
{
    listitem_T		*lw_شي;	// item being watched
    listwatch_T		*lw_ايندڙ;	// next watcher
};

/*
 * Structure to hold info about a list.
 * Order of members is optimized to reduce padding.
 * When created by range() it will at first have special value:
 *  lv_first == &range_list_item;
 * and use lv_start, lv_end, lv_stride.
 */
بنية listvar_S
{
    listitem_T	*lv_پهريون;	// first item, NULL if none, &range_list_item
				// for a non-materialized list
    listwatch_T	*lv_watch;	// first watcher, NULL if none
    اتحاد {
	بنية {	// used for non-materialized range list:
			// "lv_first" is &range_list_item
	    varnumber_T lv_شروعات;
	    varnumber_T lv_پڇاڙي;
	    صحيح		lv_stride;
	} nonmat;
	بنية {	// used for materialized list
	    listitem_T	*lv_آخري;	// last item, NULL if none
	    listitem_T	*lv_idx_شي;	// when not NULL item at index "lv_idx"
	    صحيح		lv_idx;		// cached index of an item
	} mat;
    } lv_u;
    type_T	*lv_type;	// current type, allocated by alloc_type()
    list_T	*lv_copylist;	// copied list used by deepcopy()
    list_T	*lv_استعمال_ٿيل_ايندڙ;	// next list in used lists list
    list_T	*lv_استعمال_ٿيل_prev;	// previous list in used lists list
    صحيح		lv_refcount;	// reference count
    صحيح		lv_len;		// number of items
    صحيح		lv_with_شيون;	// number of items following this struct that
				// should not be freed
    صحيح		lv_copyID;	// ID used by deepcopy()
    حرف	lv_تالو;	// zero, VAR_LOCKED, VAR_FIXED
};

/*
 * Static list with 10 items.  Use init_static_list() to initialize.
 */
تعريفنوع بنية {
    list_T	sl_list;	// must be first
    listitem_T	sl_شيون[10];
} staticList10_T;

/*
 * Structure to hold an item of a Dictionary.
 * Also used for a variable.
 * The key is copied into "di_key" to avoid an extra alloc/free for it.
 */
بنية dictitem_S
{
    typval_T	di_tv;		// type and value of the variable
    char_u	di_flags;	// DI_FLAGS_ flags (only used for variable)
    char_u	di_ڪنجي[1];	// key (actually longer!)
};
تعريفنوع بنية dictitem_S dictitem_T;

/*
 * A dictitem with a 16 character key (plus NUL).  This is an efficient way to
 * have a fixed-size dictitem.
 */
#وصف_ڪريو DICTITEM16_ڪنجي_LEN 16
بنية dictitem16_S
{
    typval_T	di_tv;		// type and value of the variable
    char_u	di_flags;	// DI_FLAGS_ flags (only used for variable)
    char_u	di_ڪنجي[DICTITEM16_ڪنجي_LEN + 1];	//ڪنجي
};
تعريفنوع بنية dictitem16_S dictitem16_T;

// Flags for "di_flags"
#وصف_ڪريو DI_FLAGS_RO	   0x01	    // read-only variable
#وصف_ڪريو DI_FLAGS_RO_SBX	   0x02	    // read-only in the sandbox
#وصف_ڪريو DI_FLAGS_FIX	   0x04	    // fixed: no :unlet or remove()
#وصف_ڪريو DI_FLAGS_تالو	   0x08	    // locked variable
#وصف_ڪريو DI_FLAGS_ALLOC	   0x10	    // separately allocated
#وصف_ڪريو DI_FLAGS_RELOAD	   0x20	    // set when script sourced again

/*
 * Structure to hold info about a Dictionary.
 */
بنية dictvar_S
{
    حرف	dv_تالو;	// zero, VAR_LOCKED, VAR_FIXED
    حرف	dv_scope;	// zero, VAR_SCOPE, VAR_DEF_SCOPE
    صحيح		dv_refcount;	// reference count
    صحيح		dv_copyID;	// ID used by deepcopy()
    hashtab_T	dv_hashtab;	// hashtab that refers to the items
    type_T	*dv_type;	// current type, allocated by alloc_type()
    dict_T	*dv_copydict;	// copied dict used by deepcopy()
    dict_T	*dv_استعمال_ٿيل_ايندڙ;	// next dict in used dicts list
    dict_T	*dv_استعمال_ٿيل_prev;	// previous dict in used dicts list
};

/*
 * Structure to hold info about a blob.
 */
بنية blobvar_S
{
    garray_T	bv_ga;		// growarray with the data
    صحيح		bv_refcount;	// reference count
    حرف	bv_تالو;	// zero, VAR_LOCKED, VAR_FIXED
};

/*
 * Structure to hold info about a tuple.
 */
بنية tuplevar_S
{
    garray_T	tv_شيون;	// tuple items
    type_T	*tv_type;	// current type, allocated by alloc_type()
    tuple_T	*tv_copytuple;	// copied tuple used by deepcopy()
    tuple_T	*tv_استعمال_ٿيل_ايندڙ;	// next tuple in used tuples list
    tuple_T	*tv_استعمال_ٿيل_prev;	// previous tuple in used tuples list
    صحيح		tv_refcount;	// reference count
    صحيح		tv_copyID;	// ID used by deepcopy()
    حرف	tv_تالو;	// zero, VAR_LOCKED, VAR_FIXED
};

/*
 * Structure to hold a generic type information
 */
بنية generictype_S
{
    type_T	*gt_type;	// generic or concrete type
    char_u	*gt_name;	// type name
};

/*
 * Generic function args table
 */
بنية gfargs_tab_S
{
    garray_T	gfat_دليلون;
    garray_T	gfat_param_types;
    garray_T	gfat_arg_types;
};

تعريفنوع صحيح (*cfunc_T)(صحيح argcount, typval_T *argvars, typval_T *rettv, فراغ *state);
تعريفنوع فراغ (*cfunc_free_T)(فراغ *state);

// type of getline() last argument
تعريفنوع تعداد {
    GETLINE_NONE,	    // do not concatenate any lines
    GETLINE_CONCAT_CONT,    // concatenate continuation lines with backslash
    GETLINE_CONCAT_CONTBAR, // concatenate continuation lines with \ and |
    GETLINE_CONCAT_ALL	    // concatenate continuation and Vim9 # comment lines
} getline_opt_T;

تعريفنوع بنية svar_S svar_T;

#إذا defined(FEAT_EVAL)
/*
 * Info used by a ":for" loop.
 */
تعريفنوع بنية
{
    صحيح		fi_semicolon;	// TRUE if ending in '; var]'
    صحيح		fi_varcount;	// nr of variables in [] or zero
    صحيح		fi_break_ڳڻپ;	// nr of line breaks encountered
    listwatch_T	fi_lw;		// keep an eye on the item used.
    list_T	*fi_list;	// list being used
    صحيح		fi_bi;		// index of blob
    blob_T	*fi_blob;	// blob being used
    char_u	*fi_string;	// copy of string being used
    صحيح		fi_byte_idx;	// byte index in fi_string
    tuple_T	*fi_tuple;	// tuple being used
    صحيح		fi_tuple_idx;	// tuple index in fi_tuple
    صحيح		fi_cs_flags;	// cs_flags or'ed together
} forinfo_T;

تعريفنوع بنية funccall_S funccall_T;

// values used for "uf_def_status"
تعريفنوع تعداد {
    UF_NOT_COMPILED,	    // executed with interpreter
    UF_منزل_BE_COMPILED,	    // to be compiled before execution
    UF_COMPILING,	    // in compile_def_function()
    UF_COMPILED,	    // successfully compiled
    UF_COMPILE_ERROR	    // compilation error, cannot execute
} def_حالت_T;

/*
 * Structure to hold info for a user function.
 * When adding a field check copy_lambda_to_global_func().
 */
بنية ufunc_S
{
    صحيح		uf_varargs;	// variable nr of arguments (old style)
    صحيح		uf_flags;	// FC_ flags
    صحيح		uf_calls;	// nr of active calls
    صحيح		uf_cleared;	// func_clear() was already called
    def_حالت_T uf_def_حالت; // UF_NOT_COMPILED, UF_TO_BE_COMPILED, etc.
    صحيح		uf_dfunc_idx;	// only valid if uf_def_status is UF_COMPILED

    class_T	*uf_class;	// for class/object method and constructor;
				// does not count for class_refcount.
				// class of the object which is invoking this
				// function.
    class_T	*uf_defclass;	// class where this function is defined.

    garray_T	uf_دليلون;	// arguments, including optional arguments
    garray_T	uf_def_دليلون;	// default argument expressions
    صحيح		uf_دليلون_visible; // normally uf_args.ga_len, less when
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
    صحيح		uf_بلاڪ_اونهائي;	// nr of entries in uf_block_ids
    صحيح		*uf_بلاڪ_ids;	// blocks a :def function is defined inside
# إذا defined(FEAT_LUA)
    cfunc_T     uf_cb;		// callback function for cfunc
    cfunc_free_T uf_cb_free;    // callback function to free cfunc
    فراغ	*uf_cb_state;   // state of uf_cb
# endif

    // for generic functions
    صحيح		uf_generic_argcount;// type argument count
    generic_T	*uf_generic_دليلون;   // generic types
    type_T	*uf_generic_param_types; // list of allocated generic types
    garray_T	uf_generic_arg_types; // list of allocated type arguments
    hashtab_T	uf_generic_functab; // generic function table

    garray_T	uf_سٽون;	// function lines

    صحيح		uf_debug_tick;	// when last checked for a breakpoint in this
				// function.
    صحيح		uf_has_breakpoint;  // TRUE when a breakpoint has been set in
				    // this function.
# ifdef FEAT_PROFILE
    صحيح		uf_profiling;	// TRUE when func is being profiled
    صحيح		uf_prof_initialized;
    hash_T	uf_hash;	// hash for uf_name when profiling
    // profiling the function as a whole
    صحيح		uf_tm_ڳڻپ;	// nr of calls
    proftime_T	uf_tm_ڪل;	// time spent in function + children
    proftime_T	uf_tm_self;	// time spent in function itself
    proftime_T	uf_tm_children;	// time spent in children this call
    // profiling the function per line
    صحيح		*uf_tml_ڳڻپ;	// nr of times line was executed
    proftime_T	*uf_tml_ڪل;	// time spent in a line + children
    proftime_T	*uf_tml_self;	// time spent in a line itself
    proftime_T	uf_tml_شروعات;	// start time for current line
    proftime_T	uf_tml_children; // time spent in children for this line
    proftime_T	uf_tml_wait;	// start wait time for current line
    صحيح		uf_tml_idx;	// index of line being timed; -1 if none
    صحيح		uf_tml_execed;	// line being timed was executed
# endif
    sctx_T	uf_script_ctx;	// SCTX where function was defined,
				// used for s: variables; sc_version changed
				// for :function
    صحيح		uf_script_ctx_نسخو;  // original sc_version of SCTX
    صحيح		uf_refcount;	// reference count, see func_name_refcount()

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
# define FC_گهيرو  0x08	// closure, uses outer scope variables
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
# define IS_OBJECT_طريقو(ufunc) ((ufunc->uf_flags & FC_OBJECT) == FC_OBJECT)
// Is "ufunc" a class new() constructor method?
# define IS_CONSTRUCTOR_طريقو(ufunc) ((ufunc->uf_flags & FC_NEW) == FC_NEW)
// Is "ufunc" an abstract class method?
# define IS_ABSTRACT_طريقو(ufunc) ((ufunc->uf_flags & FC_ABSTRACT) == FC_ABSTRACT)
# define IS_GENERIC_FUNC(ufunc) (((ufunc)->uf_flags & FC_GENERIC) == FC_GENERIC)

# define MAX_FUNC_دليلون	20	// maximum number of function arguments
# define VAR_SHORT_LEN	20	// short variable name length
# define FIXVAR_CNT	12	// number of fixed variables

/*
 * Structure to hold info for a function that is currently being executed.
 */
بنية funccall_S
{
    ufunc_T	*fc_func;	// function being called
    صحيح		fc_linenr;	// next line to be executed
    صحيح		fc_returned;	// ":return" used
    بنية			// fixed variables for arguments
    {
	dictitem_T	var;		// variable (without room for name)
	char_u	گنجائش[VAR_SHORT_LEN];	// room for the name
    } fc_fixvar[FIXVAR_CNT];
    dict_T	fc_l_vars;	// l: local function variables
    dictitem_T	fc_l_vars_var;	// variable for l: scope
    dict_T	fc_l_avars;	// a: argument variables
    dictitem_T	fc_l_avars_var;	// variable for a: scope
    list_T	fc_l_varlist;	// list for a:000
    listitem_T	fc_l_listitems[MAX_FUNC_دليلون];	// listitems for a:000
    typval_T	*fc_rettv;	// return value
    linenr_T	fc_breakpoint;	// next line with breakpoint or zero
    صحيح		fc_dbg_tick;	// debug_tick when breakpoint was set
    صحيح		fc_level;	// top nesting level of executed function

    garray_T	fc_defer;	// functions to be called on return
    ectx_T	*fc_ectx;	// execution context for :def function, NULL
				// otherwise

# ifdef FEAT_PROFILE
    proftime_T	fc_prof_ٻار;	// time spent in a child
# endif
    funccall_T	*fc_caller;	// calling function or NULL; or next funccal in
				// list pointed to by previous_funccal.

    // for closure
    صحيح		fc_refcount;	// number of user functions that reference this
				// funccal
    صحيح		fc_copyID;	// for garbage collection
    garray_T	fc_ufuncs;	// list of ufunc_T* which keep a reference to
				// "fc_func"
};

// structure used as item in "fc_defer"
تعريفنوع بنية
{
    char_u	*dr_name;	// function name, allocated
    typval_T	dr_argvars[MAX_FUNC_دليلون + 1];
    صحيح		dr_argcount;
} defer_T;

/*
 * Struct used by trans_function_name()
 */
تعريفنوع بنية
{
    dict_T	*fd_dict;	// Dictionary used
    char_u	*fd_newkey;	// new key in "dict" in allocated memory
    dictitem_T	*fd_di;		// Dictionary item used
} funcdict_T;

تعريفنوع بنية funccal_داخلا funccal_داخلا_T;
بنية funccal_داخلا {
    فراغ	    *چوٽي_funccal;
    funccal_داخلا_T *ايندڙ;
};

// From user function to hashitem and back.
# define UF2HIKEY(fp) ((fp)->uf_name)
# define HIKEY2UF(p)  ((ufunc_T *)((p) - offsetof(ufunc_T, uf_name)))
# define HI2UF(hi)     HIKEY2UF((hi)->hi_ڪنجي)

/*
 * Holds the hashtab with variables local to each sourced script.
 * Each item holds a variable (nameless) that points to the dict_T.
 */
تعريفنوع بنية {
    dictitem_T	sv_var;
    dict_T	sv_dict;
} scriptvar_T;

/*
 * Entry for "sn_all_vars".  Contains the s: variables from sn_vars plus the
 * block-local ones.
 */
تعريفنوع بنية sallvar_S sallvar_T;
بنية sallvar_S {
    sallvar_T	*sav_ايندڙ;	  // var with same name but different block
    صحيح		sav_بلاڪ_id;	  // block ID where declared
    صحيح		sav_var_vals_idx; // index in sn_var_vals

    // So long as the variable is valid (block it was defined in is still
    // active) "sav_di" is used.  It is set to NULL when leaving the block,
    // then sav_tv and sav_flags are used.
    dictitem_T *sav_di;		// dictitem with di_key and di_tv
    typval_T	sav_tv;		// type and value of the variable
    char_u	sav_flags;	// DI_FLAGS_ flags (only used for variable)
    char_u	sav_ڪنجي[1];	// key (actually longer!)
};

/*
 * In the sn_all_vars hashtab item "hi_key" points to "sav_key" in a sallvar_T.
 * This makes it possible to store and find the sallvar_T.
 * SAV2HIKEY() converts a sallvar_T pointer to a hashitem key pointer.
 * HIKEY2SAV() converts a hashitem key pointer to a sallvar_T pointer.
 * HI2SAV() converts a hashitem pointer to a sallvar_T pointer.
 */
# define SAV2HIKEY(sav) ((sav)->sav_ڪنجي)
# define HIKEY2SAV(p)  ((sallvar_T *)(p - offsetof(sallvar_T, sav_ڪنجي)))
# define HI2SAV(hi)     HIKEY2SAV((hi)->hi_ڪنجي)

# define SVFLAG_TYPE_ALLOCATED	1  // call free_type() for "sv_type"
# define SVFLAG_EXPORTED		2  // "export let var = val"
# define SVFLAG_ASSIGNED		4  // assigned a value

/*
 * Entry for "sn_var_vals".  Used for script-local variables.
 */
بنية svar_S {
    char_u	*sv_name;	// points into "sn_all_vars" di_key
    typval_T	*sv_tv;		// points into "sn_vars" or "sn_all_vars" di_tv
    type_T	*sv_type;
    صحيح		sv_flags;	// SVFLAG_ values above
    صحيح		sv_const;	// 0, ASSIGN_CONST or ASSIGN_FINAL
};

تعريفنوع بنية {
    char_u	*imp_name;	    // name imported as (allocated)
    scid_T	imp_sid;	    // script ID of "from"
    صحيح		imp_flags;	    // IMP_FLAGS_ values
} imported_T;

# define IMP_FLAGS_RELOAD	2   // script reloaded, OK to redefine
# define IMP_FLAGS_AUTOLOAD	4   // script still needs to be loaded

/*
 * Info about an encountered script.
 * When sn_state has SN_STATE_NOT_LOADED, it has not been sourced yet.
 */
تعريفنوع بنية
{
    char_u	*sn_name;	    // full path of script file
    صحيح		sn_script_seq;	    // latest sctx_T sc_seq value

    // When non-zero the script ID of the actually sourced script.  Used if a
    // script is used by a name which has a symlink, we list both names, but
    // only the linked-to script is actually sourced.
    صحيح		sn_sourced_sid;

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
    صحيح		sn_current_بلاڪ_id; // ID for current block, 0 for outer
    صحيح		sn_آخري_بلاڪ_id;  // Unique ID for each script block

    صحيح		sn_نسخو;	// :scriptversion
    صحيح		sn_state;	// SN_STATE_ values
    char_u	*sn_save_cpo;	// 'cpo' value when :vim9script found
    حرف	sn_is_vimrc;	// .vimrc file, do not restore 'cpo'
    حرف	sn_syml_checked;// flag: this has been checked for sym link

    // for a Vim9 script under "rtp/autoload/" this is "dir#scriptname#"
    char_u	*sn_autoload_اڳياڙي;

    // TRUE for a script used with "import autoload './dirname/script.vim'"
    // For "../autoload/script.vim" sn_autoload_prefix is also set.
    صحيح		sn_import_autoload;

# ifdef FEAT_PROFILE
    صحيح		sn_prof_on;	// TRUE when script is/was profiled
    صحيح		sn_pr_force;	// forceit: profile functions in this script
    proftime_T	sn_pr_ٻار;	// time set when going into first child
    صحيح		sn_pr_nest;	// nesting for sn_pr_child
    // profiling the script as a whole
    صحيح		sn_pr_ڳڻپ;	// nr of times sourced
    proftime_T	sn_pr_ڪل;	// time spent in script + children
    proftime_T	sn_pr_self;	// time spent in script itself
    proftime_T	sn_pr_شروعات;	// time at script start
    proftime_T	sn_pr_children; // time in children after script start
    // profiling the script per line
    garray_T	sn_prl_ga;	// things stored for every line
    proftime_T	sn_prl_شروعات;	// start time for current line
    proftime_T	sn_prl_children; // time spent in children for this line
    proftime_T	sn_prl_wait;	// wait start time for current line
    صحيح		sn_prl_idx;	// index of line being timed; -1 if none
    صحيح		sn_prl_execed;	// line being timed was executed
# endif
} scriptitem_T;

# define SN_STATE_NEW		0   // newly loaded script, nothing done
# define SN_STATE_NOT_LOADED	1   // script located but not loaded
# define SN_STATE_RELOAD		2   // script loaded before, nothing done
# define SN_STATE_HAD_COMMAND	9   // a command was executed

// Struct passed through eval() functions.
// See EVALARG_EVALUATE for a fixed value with eval_flags set to EVAL_EVALUATE.
تعريفنوع بنية {
    صحيح		eval_flags;	    // EVAL_ flag values below
    صحيح		eval_break_ڳڻپ;   // nr of line breaks consumed

    // copied from exarg_T when "getline" is "getsourceline". Can be NULL.
    char_u	*(*eval_getline)(صحيح, فراغ *, صحيح, getline_opt_T);
    فراغ	*eval_cookie;	    // argument for eval_getline()

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
    صحيح		eval_using_cmdline;

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
تعريفنوع بنية sn_prl_S
{
    صحيح		snp_ڳڻپ;	// nr of times line was executed
    proftime_T	sn_prl_ڪل;	// time spent in a line + children
    proftime_T	sn_prl_self;	// time spent in a line itself
} sn_prl_T;

#  define PRL_شي(si, idx)	(((sn_prl_T *)(si)->sn_prl_ga.ga_معلومات)[(idx)])

تعريفنوع بنية {
    صحيح		pi_started_profiling;
    proftime_T	pi_wait_شروعات;
    proftime_T	pi_call_شروعات;
} profinfo_T;

# وإلا
تعريفنوع بنية
{
    صحيح	    dummy;
} profinfo_T;
# endif
#وإلا
// dummy typedefs for use in function prototypes
بنية ufunc_S
{
    صحيح	    dummy;
};
تعريفنوع بنية
{
    صحيح	    dummy;
} funccall_T;
تعريفنوع بنية
{
    صحيح	    dummy;
} funcdict_T;
تعريفنوع بنية
{
    صحيح	    dummy;
} funccal_داخلا_T;
تعريفنوع بنية
{
    صحيح	    dummy;
} scriptitem_T;
تعريفنوع بنية
{
    صحيح	    dummy;
} evalarg_T;
#جيڪڏهن_پورو

// Struct passed between functions dealing with function call execution.
//
// "fe_argv_func", when not NULL, can be used to fill in arguments only when the
// invoked function uses them.  It is called like this:
//   new_argcount = fe_argv_func(current_argcount, argv, partial_argcount,
//							called_func)
//
تعريفنوع بنية {
    صحيح		(* fe_argv_func)(صحيح, typval_T *, صحيح, ufunc_T *);
    linenr_T	fe_firstline;	// first line of range
    linenr_T	fe_lastline;	// last line of range
    صحيح		*fe_doesrange;	// if not NULL: return: function handled range
    صحيح		fe_evaluate;	// actually evaluate expressions
    ufunc_T	*fe_ufunc;	// function to be called, when NULL lookup by
				// name
    partial_T	*fe_partial;	// for "dict" and extra arguments
    dict_T	*fe_selfdict;	// Dictionary for "self"
    object_T	*fe_object;	// object, e.g. for "this.Func()"
    typval_T	*fe_basetv;	// base for base->method()
    type_T	*fe_check_type;	// type from funcref or NULL
    صحيح		fe_مليو_var;	// if the function is not found then give an
				// error that a variable is not callable.
    cctx_T	*fe_cctx;	// when compiling a :def function
} funcexe_T;

/*
 * Structure to hold the context of a compiled function, used by closures
 * defined in that function.
 */
تعريفنوع بنية funcstack_S funcstack_T;
بنية funcstack_S
{
    funcstack_T *fs_ايندڙ;	// linked list at "first_funcstack"
    funcstack_T *fs_prev;

    garray_T	fs_ga;		// contains the stack, with:
				// - arguments
				// - frame
				// - local variables
    صحيح		fs_var_وٿي;	// count of arguments + frame size == offset to
				// local variables

    صحيح		fs_refcount;	// nr of closures referencing this funcstack
    صحيح		fs_min_refcount; // nr of closures on this funcstack
    صحيح		fs_copyID;	// for garbage collection
};

/*
 * Structure to hold the variables declared in a loop that are possibly used
 * in a closure.
 */
تعريفنوع بنية loopvars_S loopvars_T;
بنية loopvars_S
{
    loopvars_T *lvs_ايندڙ;	// linked list at "first_loopvars"
    loopvars_T *lvs_prev;

    garray_T	lvs_ga;		// contains the variables
    صحيح		lvs_refcount;	// nr of closures referencing this loopvars
    صحيح		lvs_min_refcount; // nr of closures on this loopvars
    صحيح		lvs_copyID;	// for garbage collection
};

// maximum nesting of :while and :for loops in a :def function
#وصف_ڪريو MAX_LOOP_اونهائي 10

تعريفنوع بنية outer_S outer_T;
بنية outer_S {
    garray_T	*ٻاهريون_ڍير;	    // stack from outer scope, or a copy
				    // containing only arguments and local vars
    صحيح		ٻاهريون_چوکٺو_idx;	    // index of stack frame in out_stack
    outer_T	*ٻاهريون_مٿي;	    // outer scope of outer scope or NULL
    partial_T	*ٻاهريون_مٿي_partial;    // partial owning out_up or NULL

    بنية {
	garray_T *ڍير;	    // stack from outer scope, or a copy
				    // containing only vars inside the loop
	ننڍو	 var_idx;	    // first variable defined in a loop in
				    // out_loop_stack
	ننڍو	 var_ڳڻپ;	    // number of variables defined in a loop
    } ٻاهريون_loop[MAX_LOOP_اونهائي];
    صحيح		ٻاهريون_loop_سائيز;	    // nr of used entries in out_loop[]
};

بنية partial_S
{
    صحيح		pt_refcount;	// reference count
    صحيح		pt_auto;	// when TRUE the partial was created for using
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
    loopvars_T	*(pt_loopvars[MAX_LOOP_اونهائي]);
				// copy of loop variables, used after loop
				// block ends

    typval_T	*pt_argv;	// arguments in allocated array
    صحيح		pt_argc;	// number of arguments

    صحيح		pt_copyID;	// funcstack may contain pointer to partial
    dict_T	*pt_dict;	// dict for "self"
    object_T	*pt_obj;	// object method
};

تعريفنوع بنية {
    ننڍو	lvi_اونهائي;	    // current nested loop depth
    بنية {
	ننڍو	var_idx;	    // index of first variable inside loop
	ننڍو	var_ڳڻپ;	    // number of variables inside loop
    } lvi_loop[MAX_LOOP_اونهائي];
} loopvarinfo_T;

تعريفنوع بنية AutoPatCmd_S AutoPatCmd_T;

/*
 * Entry in the execution stack "exestack".
 */
تعريفنوع تعداد {
    ETYPE_چوٽي,		    // toplevel
    ETYPE_SCRIPT,	    // sourcing script, use es_info.sctx
    ETYPE_UFUNC,	    // user function, use es_info.ufunc
    ETYPE_AUCMD,	    // autocomand, use es_info.aucmd
    ETYPE_MODELINE,	    // modeline, use es_info.sctx
    ETYPE_EXCEPT,	    // exception, use es_info.exception
    ETYPE_دليلون,		    // command line argument
    ETYPE_ENV,		    // environment variable
    ETYPE_اندروني,	    // internal operation
    ETYPE_SPELL,	    // loading spell file
} etype_T;

تعريفنوع بنية {
    ڊگهو      es_lnum;      // replaces "sourcing_lnum"
    char_u    *es_name;     // replaces "sourcing_name"
    etype_T   es_type;
    اتحاد {
	sctx_T  *sctx;      // script and modeline info
#إذا defined(FEAT_EVAL)
	ufunc_T *ufunc;     // function info
#جيڪڏهن_پورو
	AutoPatCmd_T *aucmd;  // autocommand info
	except_T   *except; // exception info
    } es_info;
#إذا defined(FEAT_EVAL)
    sctx_T	es_save_sctx;	    // saved current_sctx when calling function
#جيڪڏهن_پورو
} estack_T;

// Information returned by get_tty_info().
تعريفنوع بنية {
    صحيح backspace;	// what the Backspace key produces
    صحيح enter;		// what the Enter key produces
    صحيح مداخلت;	// interrupt character
    صحيح nl_does_cr;	// TRUE when a NL is expanded to CR-NL on output
} ttyinfo_T;

// Status of a job.  Order matters!
تعريفنوع تعداد
{
    ڪم_FAILED,
    ڪم_STARTED,
    ڪم_ENDED,	    // detected job done
    ڪم_ختم,   // job done and cleanup done
} jobstatus_T;

/*
 * Structure to hold info about a Job.
 */
بنية jobvar_S
{
    ڪم_T	*jv_ايندڙ;
    ڪم_T	*jv_prev;
#جيڪڏهن_وصف_ٿيل UNIX
    pid_t	jv_pid;
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل MSWIN
    PROCESS_INFORMATION	jv_proc_info;
    HANDLE		jv_ڪم_object;
#جيڪڏهن_پورو
    jobstatus_T	jv_حالت;
    char_u	*jv_tty_اندريون;	// controlling tty input, allocated
    char_u	*jv_tty_ٻاهريون;	// controlling tty output, allocated
    char_u	*jv_stoponexit;	// allocated
#جيڪڏهن_وصف_ٿيل UNIX
    char_u	*jv_termsig;	// allocated
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل MSWIN
    char_u	*jv_tty_type;	// allocated
#جيڪڏهن_پورو
    صحيح		jv_exitval;
    callback_T	jv_exit_cb;

    buf_T	*jv_اندريون_buf;	// buffer from "in-name"

    صحيح		jv_refcount;	// reference count
    صحيح		jv_copyID;

    channel_T	*jv_channel;	// channel for I/O, reference counted
    حرف	**jv_argv;	// command line used to start the job
};

/*
 * Structures to hold info about a Channel.
 */
بنية readq_S
{
    char_u	*rq_بفر;
    long_u	rq_buflen;
    readq_T	*rq_ايندڙ;
    readq_T	*rq_prev;
};

بنية writeq_S
{
    garray_T	wq_ga;
    writeq_T	*wq_ايندڙ;
    writeq_T	*wq_prev;
};

بنية jsonq_S
{
    typval_T	*jq_قدر;
    jsonq_T	*jq_ايندڙ;
    jsonq_T	*jq_prev;
    صحيح		jq_no_callback; // TRUE when no callback was found
};

بنية cbq_S
{
    callback_T	cq_callback;
    صحيح		cq_seq_nr;
    cbq_T	*cq_ايندڙ;
    cbq_T	*cq_prev;
};

// mode for a channel
تعريفنوع تعداد
{
    CH_گهڻائي_NL = 0,
    CH_گهڻائي_RAW,
    CH_گهڻائي_BLOB,
    CH_گهڻائي_JSON,
    CH_گهڻائي_JS,
    CH_گهڻائي_LSP,	// Language Server Protocol (http + json)
    CH_گهڻائي_DAP		// Debug Adapter Protocol (like LSP, but does not
			// strictly follow JSON-RPC standard)
} ch_گهڻائي_T;

تعريفنوع تعداد {
    JIO_PIPE,	    // default
    JIO_NULL,
    JIO_FILE,
    JIO_بفر,
    JIO_ٻاهريون
} ڪم_io_T;

#وصف_ڪريو CH_PART_FD(part)	ch_part[part].ch_fd

// Ordering matters, it is used in for loops: IN is last, only SOCK/OUT/ERR
// are polled.
تعريفنوع تعداد {
    PART_SOCK = 0,
#وصف_ڪريو CH_SOCK_FD	CH_PART_FD(PART_SOCK)
#جيڪڏهن_وصف_ٿيل FEAT_ڪم_CHANNEL
    PART_ٻاهريون,
# define CH_ٻاهريون_FD	CH_PART_FD(PART_ٻاهريون)
    PART_ERR,
# define CH_ERR_FD	CH_PART_FD(PART_ERR)
    PART_اندريون,
# define CH_اندريون_FD	CH_PART_FD(PART_اندريون)
#جيڪڏهن_پورو
    PART_ڳڻپ,
} ch_part_T;

#وصف_ڪريو INVALID_FD	(-1)

// The per-fd info for a channel.
تعريفنوع بنية {
    sock_T	ch_fd;	    // socket/stdin/stdout/stderr, -1 if not used

#إذا defined(UNIX) && !defined(HAVE_SELECT)
    صحيح		ch_poll_idx;	// used by channel_poll_setup()
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_GUI_X11
    Xt_داخلا_Id	ch_داخلا_Handler; // Cookie for input
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_GUI_GTK
    gint	ch_داخلا_Handler; // Cookie for input
#جيڪڏهن_پورو

    ch_گهڻائي_T	ch_گهڻائي;
    ڪم_io_T	ch_io;
    صحيح		ch_timeout;	// request timeout in msec

    readq_T	ch_سر;	// header for circular raw read queue
    jsonq_T	ch_json_سر;	// header for circular json read queue
    garray_T	ch_بلاڪ_ids;	// list of IDs that channel_read_json_block()
				// is waiting for
    // When ch_wait_len is non-zero use ch_deadline to wait for incomplete
    // message to be complete. The value is the length of the incomplete
    // message when the deadline was set.  If it gets longer (something was
    // received) the deadline is reset.
    size_t	ch_wait_len;
#جيڪڏهن_وصف_ٿيل MSWIN
    DWORD	ch_deadline;
#وإلا
    بنية timeval ch_deadline;
#جيڪڏهن_پورو
    صحيح		ch_بلاڪ_write;	// for testing: 0 when not used, -1 when write
				// does not block, 1 simulate blocking
    صحيح		ch_nonblocking;	// write() is non-blocking
    writeq_T	ch_writeque;	// header for write queue

    cbq_T	ch_cb_سر;	// dummy node for per-request callbacks
    callback_T	ch_callback;	// call when a msg is not handled

    bufref_T	ch_bufref;	// buffer to read from or write to
    صحيح		ch_nomodifiable; // TRUE when buffer can be 'nomodifiable'
    صحيح		ch_nomod_error;	// TRUE when e_modifiable was given
    صحيح		ch_buf_append;	// write appended lines instead top-bot
    linenr_T	ch_buf_چوٽي;	// next line to send
    linenr_T	ch_buf_bot;	// last line to send
} chanpart_T;

بنية channel_S {
    channel_T	*ch_ايندڙ;
    channel_T	*ch_prev;

    صحيح		ch_id;		// ID of the channel
    صحيح		ch_آخري_نياپو_id;	// ID of the last message

    chanpart_T	ch_part[PART_ڳڻپ]; // info for socket, out, err and in
    صحيح		ch_write_متن_گهڻائي; // write buffer lines with CR, not NL

    حرف	*ch_hostname;	// only for socket, allocated
    صحيح		ch_پورٽ;	// only for socket

    صحيح		ch_منزل_be_closed; // bitset of readable fds to be closed.
				 // When all readable fds have been closed,
				 // set to (1 << PART_COUNT).
    صحيح		ch_منزل_be_freed; // When TRUE channel must be freed when it's
				// safe to invoke callbacks.
    صحيح		ch_error;	// When TRUE an error was reported.  Avoids
    // giving pages full of error messages when
    // the other side has exited, only mention the
    // first error until the connection works
    // again.
    صحيح		ch_listen;	// When TRUE channel is listen socket.

    فراغ	(*ch_nb_close_cb)(فراغ);
				// callback for Netbeans when channel is
				// closed
#جيڪڏهن_وصف_ٿيل FEAT_SOCKETSERVER
    bool	ch_socketserver; // If channel is used by socketserver
    فراغ	(*ch_ss_close_cb)(channel_T *);
    فراغ	(*ch_ss_accept_cb)(channel_T *);
    channel_T	*ch_ss_ايندڙ;
    channel_T	*ch_ss_prev;
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل MSWIN
    صحيح		ch_named_pipe;	// using named pipe instead of pty
#جيڪڏهن_پورو
    callback_T	ch_callback;	// call when any msg is not handled
    callback_T	ch_close_cb;	// call when channel is closed
    صحيح		ch_drop_never;
    صحيح		ch_keep_open;	// do not close on read error
    صحيح		ch_nonblock;

    ڪم_T	*ch_ڪم;	// Job that uses this channel; this does not
				// count as a reference to avoid a circular
				// reference, the job refers to the channel.
    صحيح		ch_ڪم_killed;	// TRUE when there was a job and it was killed
				// or we know it died.
    صحيح		ch_anonymous_pipe;  // Indicates that anonymous pipes are being
				    // used for communication in the Windows
				    // ConPTY terminal.
    صحيح		ch_killing;	    // Indicates that the job associated with
				    // the channel is terminating.  It becomes
				    // TRUE when TerminateJobObject() was
				    // called or the process associated with
				    // the job had exited (only ConPTY).

    صحيح		ch_refcount;	// reference count
    صحيح		ch_copyID;
};

#وصف_ڪريو JO_گهڻائي		    0x0001	// channel mode
#وصف_ڪريو JO_اندريون_گهڻائي	    0x0002	// stdin mode
#وصف_ڪريو JO_ٻاهريون_گهڻائي	    0x0004	// stdout mode
#وصف_ڪريو JO_ERR_گهڻائي	    0x0008	// stderr mode
#وصف_ڪريو JO_CALLBACK	    0x0010	// channel callback
#وصف_ڪريو JO_ٻاهريون_CALLBACK	    0x0020	// stdout callback
#وصف_ڪريو JO_ERR_CALLBACK	    0x0040	// stderr callback
#وصف_ڪريو JO_CLOSE_CALLBACK   0x0080	// "close_cb"
#وصف_ڪريو JO_WAITTIME	    0x0100	// only for ch_open()
#وصف_ڪريو JO_TIMEOUT	    0x0200	// all timeouts
#وصف_ڪريو JO_ٻاهريون_TIMEOUT	    0x0400	// stdout timeouts
#وصف_ڪريو JO_ERR_TIMEOUT	    0x0800	// stderr timeouts
#وصف_ڪريو JO_PART		    0x1000	// "part"
#وصف_ڪريو JO_ID		    0x2000	// "id"
#وصف_ڪريو JO_STOPONEXIT	    0x4000	// "stoponexit"
#وصف_ڪريو JO_EXIT_CB	    0x8000	// "exit_cb"
#وصف_ڪريو JO_ٻاهريون_IO	    0x10000	// "out_io"
#وصف_ڪريو JO_ERR_IO	    0x20000	// "err_io" (JO_OUT_IO << 1)
#وصف_ڪريو JO_اندريون_IO	    0x40000	// "in_io" (JO_OUT_IO << 2)
#وصف_ڪريو JO_ٻاهريون_NAME	    0x80000	// "out_name"
#وصف_ڪريو JO_ERR_NAME	    0x100000	// "err_name" (JO_OUT_NAME << 1)
#وصف_ڪريو JO_اندريون_NAME	    0x200000	// "in_name" (JO_OUT_NAME << 2)
#وصف_ڪريو JO_اندريون_چوٽي	    0x400000	// "in_top"
#وصف_ڪريو JO_اندريون_BOT	    0x800000	// "in_bot"
#وصف_ڪريو JO_ٻاهريون_BUF	    0x1000000	// "out_buf"
#وصف_ڪريو JO_ERR_BUF	    0x2000000	// "err_buf" (JO_OUT_BUF << 1)
#وصف_ڪريو JO_اندريون_BUF	    0x4000000	// "in_buf" (JO_OUT_BUF << 2)
#وصف_ڪريو JO_CHANNEL	    0x8000000	// "channel"
#وصف_ڪريو JO_بلاڪ_WRITE	    0x10000000	// "block_write"
#وصف_ڪريو JO_ٻاهريون_MODIFIABLE   0x20000000	// "out_modifiable"
#وصف_ڪريو JO_ERR_MODIFIABLE   0x40000000	// "err_modifiable" (JO_OUT_ << 1)
#وصف_ڪريو JO_ALL		    0x7fffffff

#وصف_ڪريو JO2_ٻاهريون_نياپو	    0x0001	// "out_msg"
#وصف_ڪريو JO2_ERR_نياپو	    0x0002	// "err_msg" (JO_OUT_ << 1)
#وصف_ڪريو JO2_رڪن_NAME	    0x0004	// "term_name"
#وصف_ڪريو JO2_رڪن_پڄاڻي	    0x0008	// "term_finish"
#وصف_ڪريو JO2_ENV		    0x0010	// "env"
#وصف_ڪريو JO2_CWD		    0x0020	// "cwd"
#وصف_ڪريو JO2_رڪن_صفون	    0x0040	// "term_rows"
#وصف_ڪريو JO2_رڪن_کانن_ڳڻپ	    0x0080	// "term_cols"
#وصف_ڪريو JO2_VERTICAL	    0x0100	// "vertical"
#وصف_ڪريو JO2_CURWIN	    0x0200	// "curwin"
#وصف_ڪريو JO2_HIDDEN	    0x0400	// "hidden"
#وصف_ڪريو JO2_رڪن_OPENCMD    0x0800	// "term_opencmd"
#وصف_ڪريو JO2_EOF_CHARS	    0x1000	// "eof_chars"
#وصف_ڪريو JO2_NORESTORE	    0x2000	// "norestore"
#وصف_ڪريو JO2_رڪن_KILL	    0x4000	// "term_kill"
#وصف_ڪريو JO2_ANSI_COLORS	    0x8000	// "ansi_colors"
#وصف_ڪريو JO2_TTY_TYPE	    0x10000	// "tty_type"
#وصف_ڪريو JO2_BUFNR	    0x20000	// "bufnr"
#وصف_ڪريو JO2_رڪن_API	    0x40000	// "term_api"
#وصف_ڪريو JO2_رڪن_HIGHLIGHT  0x80000	// "highlight"

#وصف_ڪريو JO_گهڻائي_ALL	(JO_گهڻائي + JO_اندريون_گهڻائي + JO_ٻاهريون_گهڻائي + JO_ERR_گهڻائي)
#وصف_ڪريو JO_CB_ALL \
    (JO_CALLBACK + JO_ٻاهريون_CALLBACK + JO_ERR_CALLBACK + JO_CLOSE_CALLBACK)
#وصف_ڪريو JO_TIMEOUT_ALL	(JO_TIMEOUT + JO_ٻاهريون_TIMEOUT + JO_ERR_TIMEOUT)

/*
 * Options for job and channel commands.
 */
تعريفنوع بنية
{
    صحيح		jo_سيٽ;		// JO_ bits for values that were set
    صحيح		jo_set2;	// JO2_ bits for values that were set

    ch_گهڻائي_T	jo_گهڻائي;
    ch_گهڻائي_T	jo_اندريون_گهڻائي;
    ch_گهڻائي_T	jo_ٻاهريون_گهڻائي;
    ch_گهڻائي_T	jo_err_گهڻائي;
    صحيح		jo_noblock;

    ڪم_io_T	jo_io[4];	// PART_OUT, PART_ERR, PART_IN
    char_u	jo_io_name_buf[4][NUMBUFLEN];
    char_u	*jo_io_name[4];	// not allocated!
    صحيح		jo_io_buf[4];
    صحيح		jo_pty;
    صحيح		jo_modifiable[4];
    صحيح		jo_پيغام[4];
    channel_T	*jo_channel;

    linenr_T	jo_اندريون_چوٽي;
    linenr_T	jo_اندريون_bot;

    callback_T	jo_callback;
    callback_T	jo_ٻاهريون_cb;
    callback_T	jo_err_cb;
    callback_T	jo_close_cb;
    callback_T	jo_exit_cb;
    صحيح		jo_drop_never;
    صحيح		jo_waittime;
    صحيح		jo_timeout;
    صحيح		jo_ٻاهريون_timeout;
    صحيح		jo_err_timeout;
    صحيح		jo_بلاڪ_write;	// for testing only
    صحيح		jo_part;
    صحيح		jo_id;
    char_u	jo_stoponexit_buf[NUMBUFLEN];
    char_u	*jo_stoponexit;
    dict_T	*jo_env;	// environment variables
    char_u	jo_cwd_buf[NUMBUFLEN];
    char_u	*jo_cwd;

#جيڪڏهن_وصف_ٿيل FEAT_TERMINAL
    // when non-zero run the job in a terminal window of this size
    صحيح		jo_رڪن_صفون;
    صحيح		jo_رڪن_کانن_ڳڻپ;
    صحيح		jo_vertical;
    صحيح		jo_curwin;
    buf_T	*jo_bufnr_buf;
    صحيح		jo_hidden;
    صحيح		jo_رڪن_norestore;
    char_u	jo_رڪن_name_buf[NUMBUFLEN];
    char_u	*jo_رڪن_name;
    char_u	jo_رڪن_opencmd_buf[NUMBUFLEN];
    char_u	*jo_رڪن_opencmd;
    صحيح		jo_رڪن_پڄاڻي;
    char_u	jo_eof_chars_buf[NUMBUFLEN];
    char_u	*jo_eof_chars;
    char_u	jo_رڪن_kill_buf[NUMBUFLEN];
    char_u	*jo_رڪن_kill;
# إذا defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    long_u	jo_ansi_colors[16];
# endif
    char_u	jo_رڪن_highlight_buf[NUMBUFLEN];
    char_u	*jo_رڪن_highlight;
    صحيح		jo_tty_type;	    // first character of "tty_type"
    char_u	jo_رڪن_api_buf[NUMBUFLEN];
    char_u	*jo_رڪن_api;
#جيڪڏهن_پورو
} jobopt_T;

#جيڪڏهن_وصف_ٿيل FEAT_EVAL
/*
 * Structure used for listeners added with listener_add().
 */
تعريفنوع بنية listener_S listener_T;
بنية listener_S
{
    listener_T	*lr_ايندڙ;
    صحيح		lr_id;
    bool	lr_متن;	// include the resulting text in each change
    callback_T	lr_callback;
};

// Structure used for listeners added with redraw_listener_add().
تعريفنوع بنية redraw_listener_S redraw_listener_T;
بنية redraw_listener_S
{
    redraw_listener_T	*rl_ايندڙ;
    صحيح			rl_id;
    بنية
    {
	callback_T	on_شروعات;
	callback_T	on_پڇاڙي;
    }			rl_callbacks;
};
#جيڪڏهن_پورو

/*
 * structure used for explicit stack while garbage collecting hash tables
 */
تعريفنوع بنية ht_ڍير_S
{
    hashtab_T		*ht;
    بنية ht_ڍير_S	*prev;
} ht_ڍير_T;

/*
 * structure used for explicit stack while garbage collecting lists
 */
تعريفنوع بنية list_ڍير_S
{
    list_T		*list;
    بنية list_ڍير_S	*prev;
} list_ڍير_T;

/*
 * structure used for explicit stack while garbage collecting tuples
 */
تعريفنوع بنية tuple_ڍير_S
{
    tuple_T			*tuple;
    بنية tuple_ڍير_S	*prev;
} tuple_ڍير_T;

/*
 * Structure used for iterating over dictionary items.
 * Initialize with dict_iterate_start().
 */
تعريفنوع بنية
{
    long_u	dit_todo;
    hashitem_T	*dit_hi;
} dict_iterator_T;

// values for b_syn_spell: what to do with toplevel text
#وصف_ڪريو SYNSPL_DEFAULT	0	// spell check if @Spell not defined
#وصف_ڪريو SYNSPL_چوٽي	1	// spell check toplevel text
#وصف_ڪريو SYNSPL_NOTOP	2	// don't spell check toplevel text

// values for b_syn_foldlevel: how to compute foldlevel on a line
#وصف_ڪريو SYNFLD_شروعات	0	// use level of item at start of line
#وصف_ڪريو SYNFLD_MINIMUM	1	// use lowest local minimum level on line

// avoid #ifdefs for when b_spell is not available
#جيڪڏهن_وصف_ٿيل FEAT_SPELL
# define B_SPELL(buf)  ((buf)->b_spell)
#وإلا
# define B_SPELL(buf)  (0)
#جيڪڏهن_پورو

تعريفنوع بنية qf_info_S qf_info_T;

#جيڪڏهن_وصف_ٿيل FEAT_PROFILE
/*
 * Used for :syntime: timing of executing a syntax pattern.
 */
تعريفنوع بنية {
    proftime_T	ڪل;		// total time used
    proftime_T	slowest;	// time of slowest call
    ڊگهو	ڳڻپ;		// nr of times used
    ڊگهو	ميلاپ;		// nr of times matched
} syn_وقت_T;
#جيڪڏهن_پورو

تعريفنوع بنية timer_S timer_T;
بنية timer_S
{
    ڊگهو	tr_id;
#جيڪڏهن_وصف_ٿيل FEAT_TIMERS
    timer_T	*tr_ايندڙ;
    timer_T	*tr_prev;
    proftime_T	tr_due;		    // when the callback is to be invoked
    حرف	tr_firing;	    // when TRUE callback is being called
    حرف	tr_paused;	    // when TRUE callback is not invoked
    حرف	tr_keep;	    // when TRUE keep timer after it fired
    صحيح		tr_repeat;	    // number of times to repeat, -1 forever
    ڊگهو	tr_interval;	    // msec
    callback_T	tr_callback;
    صحيح		tr_emsg_ڳڻپ;
#جيڪڏهن_پورو
};

#جيڪڏهن_وصف_ٿيل FEAT_CRYPT
/*
 * Structure to hold the type of encryption and the state of encryption or
 * decryption.
 */
تعريفنوع بنية {
    صحيح	    طريقو_nr;
    فراغ    *طريقو_state;  // method-specific state information
} cryptstate_T;

// values for method_nr
# define CRYPT_M_ZIP	0
# define CRYPT_M_BF	1
# define CRYPT_M_BF2	2
# define CRYPT_M_SOD    3
# define CRYPT_M_SOD2   4
# define CRYPT_M_ڳڻپ	5 // number of crypt methods

// Currently all crypt methods work inplace.  If one is added that isn't then
// define this.
# define CRYPT_NOT_INPLACE 1

// Struct for passing arguments down to the crypt_init functions
تعريفنوع بنية {
    char_u	*cat_salt;
    صحيح		cat_salt_len;
    char_u	*cat_ٻج;
    صحيح		cat_ٻج_len;
    char_u	*cat_جوڙيو;
    صحيح		cat_جوڙيو_len;
    صحيح		cat_init_from_file;
} crypt_arg_T;

#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
تعريفنوع تعداد {
    POPPOS_BOTLEFT,
    POPPOS_TOPLEFT,
    POPPOS_BOTRIGHT,
    POPPOS_TOPRIGHT,
    POPPOS_CENTER,
    POPPOS_BOTTOM,	// bottom of popup just above the command line
    POPPOS_NONE
} poppos_T;

تعريفنوع تعداد {
    POPCLOSE_NONE,
    POPCLOSE_BUTTON,
    POPCLOSE_CLICK
} popclose_T;

# define POPUPWIN_DEFAULT_ZINDEX	 50
# define POPUPMENU_ZINDEX		100
# define POPUPWIN_DIALOG_ZINDEX		200
# define POPUPWIN_NOTIFICATION_ZINDEX   300
#جيڪڏهن_پورو

/*
 * These are items normally related to a buffer.  But when using ":ownsyntax"
 * a window may have its own instance.
 */
تعريفنوع بنية {
#جيڪڏهن_وصف_ٿيل FEAT_SYN_HL
    hashtab_T	b_keywtab;		// syntax keywords hash table
    hashtab_T	b_keywtab_ic;		// idem, ignore case
    صحيح		b_syn_error;		// TRUE when error occurred in HL
# ifdef FEAT_RELTIME
    صحيح		b_syn_slow;		// TRUE when 'redrawtime' reached
# endif
    صحيح		b_syn_ic;		// ignore case for :syn cmds
    صحيح		b_syn_foldlevel;	// how to compute foldlevel on a line
    صحيح		b_syn_spell;		// SYNSPL_ values
    garray_T	b_syn_patterns;		// table for syntax patterns
    garray_T	b_syn_clusters;		// table for syntax clusters
    صحيح		b_spell_cluster_id;	// @Spell cluster ID or 0
    صحيح		b_nospell_cluster_id;	// @NoSpell cluster ID or 0
    صحيح		b_syn_containedin;	// TRUE when there is an item with a
					// "containedin" argument
    صحيح		b_syn_sync_flags;	// flags about how to sync
    ننڍو	b_syn_sync_id;		// group to sync on
    ڊگهو	b_syn_sync_minlines;	// minimal sync lines offset
    ڊگهو	b_syn_sync_maxlines;	// maximal sync lines offset
    ڊگهو	b_syn_sync_linebreaks;	// offset for multi-line pattern
    char_u	*b_syn_linecont_pat;	// line continuation pattern
    regprog_T	*b_syn_linecont_prog;	// line continuation program
# ifdef FEAT_PROFILE
    syn_وقت_T  b_syn_linecont_وقت;
# endif
    صحيح		b_syn_linecont_ic;	// ignore-case flag for above
    صحيح		b_syn_topgrp;		// for ":syntax include"
# ifdef FEAT_CONCEAL
    صحيح		b_syn_conceal;		// auto-conceal for :syn cmds
# endif
# ifdef FEAT_FOLDING
    صحيح		b_syn_folditems;	// number of patterns with the HL_FOLD
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
    صحيح		b_sst_len;
    synstate_T	*b_sst_پهريون;
    synstate_T	*b_sst_ڳولا;
    synstate_T	*b_sst_firstfree;
    صحيح		b_sst_freecount;
    linenr_T	b_sst_check_lnum;
    short_u	b_sst_lasttick;	// last display tick

    // Cache for in_id_list(); see idl_cache_T in syntax.c.
    فراغ	*b_idlist_cache;
#جيڪڏهن_پورو // FEAT_SYN_HL

#جيڪڏهن_وصف_ٿيل FEAT_SPELL
    // for spell checking
    garray_T	b_langp;	    // list of pointers to slang_T, see spell.c
    char_u	b_spell_ismw[256];  // flags: is midword char
    char_u	*b_spell_ismw_mb;   // multi-byte midword chars
    char_u	*b_p_spc;	    // 'spellcapcheck'
    regprog_T	*b_cap_prog;	    // program for 'spellcapcheck'
    char_u	*b_p_spf;	    // 'spellfile'
    char_u	*b_p_spl;	    // 'spelllang'
    char_u	*b_p_spo;	    // 'spelloptions'
    صحيح		b_cjk;		    // all CJK letters as OK
#جيڪڏهن_پورو
#إذا !defined(FEAT_SYN_HL) && !defined(FEAT_SPELL)
    صحيح		dummy;
#جيڪڏهن_پورو
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

بنية file_بفر
{
    memline_T	b_ml;		// associated memline (also contains line
				// count)

    buf_T	*b_ايندڙ;	// links in list of buffers
    buf_T	*b_prev;

    صحيح		b_nwindows;	// nr of windows open on this buffer

    صحيح		b_flags;	// various BF_ flags
    صحيح		b_locked;	// Buffer is being closed or referenced, don't
				// let autocommands wipe it out.
    صحيح		b_locked_ورهايو;	// Buffer is being closed, don't allow opening
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

#جيڪڏهن_وصف_ٿيل UNIX
    bool	b_dev_valid;	// true when b_dev has a valid number
    dev_t	b_dev;		// device number
    ino_t	b_ino;		// inode number
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل VMS
    حرف	 b_fab_rfm;	// Record format
    حرف	 b_fab_rat;	// Record attribute
    بنانشان صحيح b_fab_mrs;	// Max record size
#جيڪڏهن_پورو
    صحيح		b_fnum;		// buffer number for this file.
    char_u	b_ڪنجي[VIM_SIZEOF_INT * 2 + 1];
				// key used for buf_hashtab, holds b_fnum as
				// hex string

    صحيح		b_بدليو;	// 'modified': Set to TRUE if something in the
				// file has been changed and not written out.
    dictitem16_T b_ct_di;	// holds the b:changedtick value in
				// b_ct_di.di_tv.vval.v_number;
				// incremented for each change, also for undo
#وصف_ڪريو CHANGEDTICK(buf) ((buf)->b_ct_di.di_tv.vval.v_عدد)

    varnumber_T	b_آخري_changedtick;	// b:changedtick when TextChanged was
					// last triggered.
    varnumber_T	b_آخري_changedtick_pum; // b:changedtick for TextChangedP
    varnumber_T	b_آخري_changedtick_i;   // b:changedtick for TextChangedI

    bool	b_saving;	// Set to true if we are in the middle of
				// saving the buffer.

    /*
     * Changes to a buffer require updating of the display.  To minimize the
     * work, remember changes made and update everything at once.
     */
    bool	b_mod_سيٽ;	// true when there are changes since the last
				// time the display was updated
    linenr_T	b_mod_چوٽي;	// topmost lnum that was changed
    linenr_T	b_mod_bot;	// lnum below last changed line, AFTER the
				// change
    ڊگهو	b_mod_xlines;	// number of extra buffer lines inserted;
				// negative when lines were deleted

    wininfo_T	*b_wininfo;	// list of last used info for each window

    ڊگهو	b_mtime;	// last change time of original file
    ڊگهو	b_mtime_ns;	// nanoseconds of last change time
    ڊگهو	b_mtime_read;	// last change time when reading
    ڊگهو	b_mtime_read_ns;  // nanoseconds of last read time
    off_T	b_orig_سائيز;	// size of original file in bytes
    صحيح		b_orig_گهڻائي;	// mode of original file
#جيڪڏهن_وصف_ٿيل FEAT_VIMINFO
    وقت_T	b_آخري_استعمال_ٿيل;	// time when the buffer was last used; used
				// for viminfo
#جيڪڏهن_پورو

    pos_T	b_namedm[NMARKS]; // current named marks (mark.c)

    // These variables are set when VIsual_active becomes FALSE
    visualinfo_T b_visual;
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
    صحيح		b_visual_گهڻائي_eval;  // b_visual.vi_mode for visualmode()
#جيڪڏهن_پورو

    pos_T	b_آخري_cursor;	// cursor position when last unloading this
				//بفر
    pos_T	b_آخري_وجهو;	// where Insert mode was left
    pos_T	b_آخري_change;	// position of last change: '. mark

    /*
     * the changelist contains old change positions
     */
    pos_T	b_changelist[JUMPLISTSIZE];
    صحيح		b_changelistlen;	// number of active entries
    bool	b_new_change;		// set in u_savecommon()

    /*
     * Character table, only used in charset.c for 'iskeyword'
     * 32 bytes of 8 bits: 1 bit per character 0-255.
     */
    char_u	b_chartab[32];

    // Table used for mappings local to a buffer.
    mapblock_T	*(b_maphash[256]);

    // First abbreviation local to a buffer.
    mapblock_T	*b_پهريون_abbr;

    // User commands local to the buffer.
    garray_T	b_ucmds;
    // start and end of an operator, also used for '[ and ']
    pos_T	b_op_شروعات;
    pos_T	b_op_شروعات_orig;  // used for Insstart_orig
    pos_T	b_op_پڇاڙي;

#جيڪڏهن_وصف_ٿيل FEAT_VIMINFO
    bool	b_marks_read;	// Have we read viminfo marks yet?
#جيڪڏهن_پورو

    bool	b_modified_was_سيٽ;	// did ":set modified"
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
    u_سرو_T	*b_u_oldhead;	// pointer to oldest header
    u_سرو_T	*b_u_newhead;	// pointer to newest header; may not be valid
				// if b_u_curhead is not NULL
    u_سرو_T	*b_u_curhead;	// pointer to current header
    صحيح		b_u_numhead;	// current number of headers
    bool	b_u_synced;	// entry lists are synced
    ڊگهو	b_u_seq_آخري;	// last used undo sequence number
    ڊگهو	b_u_save_nr_آخري; // counter for last file write
    ڊگهو	b_u_seq_cur;	// uh_seq of header below which we are now
    وقت_T	b_u_وقت_cur;	// uh_time of header below which we are now
    ڊگهو	b_u_save_nr_cur; // file write nr after which we are now

    /*
     * variables for "U" command in undo.c
     */
    undoline_T	b_u_سٽ_ptr;	// saved line for "U" command
    linenr_T	b_u_سٽ_lnum;	// line number of line in u_line
    colnr_T	b_u_سٽ_colnr;	// optional column number

    bool	b_scanned;	// ^N/^P have scanned this buffer

    // flags for use of ":lmap" and IM control
    ڊگهو	b_p_iminsert;	// input mode for insert
    ڊگهو	b_p_imsearch;	// input mode for search
#وصف_ڪريو B_IMODE_USE_وجهو (-1)	//	Use b_p_iminsert value for search
#وصف_ڪريو B_IMODE_NONE 0		//	Input via none
#وصف_ڪريو B_IMODE_LMAP 1		//	Input via langmap
#وصف_ڪريو B_IMODE_IM 2		//	Input via input method
#وصف_ڪريو B_IMODE_آخري 2

#جيڪڏهن_وصف_ٿيل FEAT_KEYMAP
    ننڍو	b_kmap_state;	// using "lmap" mappings
# define KEYMAP_INIT	1	// 'keymap' was set, call keymap_init()
# define KEYMAP_LOADED	2	// 'keymap' mappings have been loaded
    garray_T	b_kmap_ga;	// the keymap table
#جيڪڏهن_پورو

    /*
     * Options local to a buffer.
     * They are here because their value depends on the type of file
     * or contents of the file being edited.
     */
    bool	b_p_initialized;	// set when options initialized

#جيڪڏهن_وصف_ٿيل FEAT_EVAL
    sctx_T	b_p_script_ctx[BV_ڳڻپ]; // SCTXs for buffer-local options
#جيڪڏهن_پورو

    صحيح		b_p_ac;		// 'autocomplete'
    صحيح		b_p_ai;		// 'autoindent'
    صحيح		b_p_ai_nopaste;	// b_p_ai saved for paste mode
    char_u	*b_p_bkc;	// 'backupcopy'
    بنانشان	b_bkc_flags;    // flags for 'backupcopy'
    صحيح		b_p_ci;		// 'copyindent'
    صحيح		b_p_bin;	// 'binary'
    صحيح		b_p_bomb;	// 'bomb'
    char_u	*b_p_bh;	// 'bufhidden'
    char_u	*b_p_bt;	// 'buftype'
#جيڪڏهن_وصف_ٿيل FEAT_QUICKFIX
# define BUF_HAS_QF_داخلا 1
# define BUF_HAS_LL_داخلا 2
    صحيح		b_has_qf_داخلا;
#جيڪڏهن_پورو
    صحيح		b_p_bl;		// 'buflisted'
    صحيح		b_p_cin;	// 'cindent'
    char_u	*b_p_cino;	// 'cinoptions'
    char_u	*b_p_cink;	// 'cinkeys'
    char_u	*b_p_cinsd;	// 'cinscopedecls'
    char_u	*b_p_cinw;	// 'cinwords'
    char_u	*b_p_com;	// 'comments'
    char_u	*b_p_cms;	// 'commentstring'
    char_u	*b_p_cot;	// 'completeopt' local value
    بنانشان	b_cot_flags;	// flags for 'completeopt'
    char_u	*b_p_cpt;	// 'complete'
#جيڪڏهن_وصف_ٿيل BACKSLASH_اندريون_FILENAME
    char_u	*b_p_csl;	// 'completeslash'
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_COMPL_FUNC
    long_u	b_p_cpt_flags;	// flags for 'complete'
    callback_T	*b_p_cpt_cb;	// F{func} in 'complete' callback
    صحيح		b_p_cpt_ڳڻپ;	// Count of values in 'complete'
    char_u	*b_p_cfu;	// 'completefunc'
    callback_T	b_cfu_cb;	// 'completefunc' callback
    char_u	*b_p_ofu;	// 'omnifunc'
    callback_T	b_ofu_cb;	// 'omnifunc' callback
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
    char_u	*b_p_tfu;	// 'tagfunc' option value
    callback_T	b_tfu_cb;	// 'tagfunc' callback
    char_u	*b_p_ffu;	// 'findfunc' option value
    callback_T	b_ffu_cb;	// 'findfunc' callback
#جيڪڏهن_پورو
    صحيح		b_p_eof;	// 'endoffile'
    صحيح		b_p_eol;	// 'endofline'
    صحيح		b_p_fixeol;	// 'fixendofline'
    صحيح		b_p_et;		// 'expandtab'
    صحيح		b_p_et_nobin;	// b_p_et saved for binary mode
    صحيح		b_p_et_nopaste; // b_p_et saved for paste mode
    char_u	*b_p_fenc;	// 'fileencoding'
    char_u	*b_p_ff;	// 'fileformat'
    char_u	*b_p_ft;	// 'filetype'
    char_u	*b_p_fo;	// 'formatoptions'
    char_u	*b_p_flp;	// 'formatlistpat'
    صحيح		b_p_inf;	// 'infercase'
    char_u	*b_p_isk;	// 'iskeyword'
#جيڪڏهن_وصف_ٿيل FEAT_ڳوليو_ID
    char_u	*b_p_def;	// 'define' local value
    char_u	*b_p_inc;	// 'include'
# ifdef FEAT_EVAL
    char_u	*b_p_inex;	// 'includeexpr'
    long_u	b_p_inex_flags;	// flags for 'includeexpr'
# endif
#جيڪڏهن_پورو
#إذا defined(FEAT_EVAL)
    char_u	*b_p_inde;	// 'indentexpr'
    long_u	b_p_inde_flags;	// flags for 'indentexpr'
    char_u	*b_p_indk;	// 'indentkeys'
#جيڪڏهن_پورو
    char_u	*b_p_fp;	// 'formatprg'
#إذا defined(FEAT_EVAL)
    char_u	*b_p_fex;	// 'formatexpr'
    long_u	b_p_fex_flags;	// flags for 'formatexpr'
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_FSYNC
    صحيح		b_p_fs;		// 'fsync'
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_CRYPT
    char_u	*b_p_ڪنجي;	// 'key'
#جيڪڏهن_پورو
    char_u	*b_p_kp;	// 'keywordprg'
    صحيح		b_p_lisp;	// 'lisp'
    char_u	*b_p_lop;	// 'lispoptions'
    char_u	*b_p_menc;	// 'makeencoding'
    char_u	*b_p_mps;	// 'matchpairs'
    صحيح		b_p_ml;		// 'modeline'
    صحيح		b_p_ml_nobin;	// b_p_ml saved for binary mode
    صحيح		b_p_ma;		// 'modifiable'
    char_u	*b_p_nf;	// 'nrformats'
    صحيح		b_p_pi;		// 'preserveindent'
    char_u	*b_p_qe;	// 'quoteescape'
    صحيح		b_p_ro;		// 'readonly'
    ڊگهو	b_p_sw;		// 'shiftwidth'
    صحيح		b_p_sn;		// 'shortname'
    صحيح		b_p_si;		// 'smartindent'
    ڊگهو	b_p_sts;	// 'softtabstop'
    ڊگهو	b_p_sts_nopaste; // b_p_sts saved for paste mode
    char_u	*b_p_sua;	// 'suffixesadd'
    صحيح		b_p_swf;	// 'swapfile'
#جيڪڏهن_وصف_ٿيل FEAT_SYN_HL
    ڊگهو	b_p_smc;	// 'synmaxcol'
    char_u	*b_p_syn;	// 'syntax'
#جيڪڏهن_پورو
    ڊگهو	b_p_ts;		// 'tabstop'
    صحيح		b_p_tx;		// 'textmode'
    ڊگهو	b_p_tw;		// 'textwidth'
    ڊگهو	b_p_tw_nobin;	// b_p_tw saved for binary mode
    ڊگهو	b_p_tw_nopaste;	// b_p_tw saved for paste mode
    ڊگهو	b_p_wm;		// 'wrapmargin'
    ڊگهو	b_p_wm_nobin;	// b_p_wm saved for binary mode
    ڊگهو	b_p_wm_nopaste;	// b_p_wm saved for paste mode
#جيڪڏهن_وصف_ٿيل FEAT_VARTABS
    char_u	*b_p_vsts;	// 'varsofttabstop'
    صحيح		*b_p_vsts_array;   // 'varsofttabstop' in internal format
    char_u	*b_p_vsts_nopaste; // b_p_vsts saved for paste mode
    char_u	*b_p_vts;	// 'vartabstop'
    صحيح		*b_p_vts_array;	// 'vartabstop' in internal format
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_KEYMAP
    char_u	*b_p_keymap;	// 'keymap'
#جيڪڏهن_پورو

    /*
     * local values for options which are normally global
     */
#جيڪڏهن_وصف_ٿيل FEAT_QUICKFIX
    char_u	*b_p_gefm;	// 'grepformat' local value
    char_u	*b_p_gp;	// 'grepprg' local value
    char_u	*b_p_mp;	// 'makeprg' local value
    char_u	*b_p_efm;	// 'errorformat' local value
#جيڪڏهن_پورو
    char_u	*b_p_ep;	// 'equalprg' local value
    char_u	*b_p_رستو;	// 'path' local value
    صحيح		b_p_ar;		// 'autoread' local value
    char_u	*b_p_ٽيگ_فهرست;	// 'tags' local value
    char_u	*b_p_tc;	// 'tagcase' local value
    بنانشان	b_tc_flags;     // flags for 'tagcase'
    char_u	*b_p_dict;	// 'dictionary' local value
#جيڪڏهن_وصف_ٿيل FEAT_DIFF
    char_u	*b_p_dia;	// 'diffanchors' local value
#جيڪڏهن_پورو
    char_u	*b_p_tsr;	// 'thesaurus' local value
#جيڪڏهن_وصف_ٿيل FEAT_COMPL_FUNC
    char_u	*b_p_tsrfu;	// 'thesaurusfunc' local value
    callback_T	b_tsrfu_cb;	// 'thesaurusfunc' callback
#جيڪڏهن_پورو
    ڊگهو	b_p_ul;		// 'undolevels' local value
#جيڪڏهن_وصف_ٿيل FEAT_PERSISTENT_UNDO
    صحيح		b_p_udf;	// 'undofile'
#جيڪڏهن_پورو
    char_u	*b_p_lw;	// 'lispwords' local value
#جيڪڏهن_وصف_ٿيل FEAT_TERMINAL
    ڊگهو	b_p_twsl;	// 'termwinscroll'
#جيڪڏهن_پورو

    /*
     * end of buffer options
     */

    // values set from b_p_cino
    صحيح		b_ind_level;
    صحيح		b_ind_open_imag;
    صحيح		b_ind_no_brace;
    صحيح		b_ind_پهريون_open;
    صحيح		b_ind_open_extra;
    صحيح		b_ind_close_extra;
    صحيح		b_ind_open_کاٻو_imag;
    صحيح		b_ind_jump_label;
    صحيح		b_ind_case;
    صحيح		b_ind_case_ڪوڊ;
    صحيح		b_ind_case_break;
    صحيح		b_ind_param;
    صحيح		b_ind_func_type;
    صحيح		b_ind_comment;
    صحيح		b_ind_اندريون_comment;
    صحيح		b_ind_اندريون_comment2;
    صحيح		b_ind_cpp_baseclass;
    صحيح		b_ind_continuation;
    صحيح		b_ind_unclosed;
    صحيح		b_ind_unclosed2;
    صحيح		b_ind_unclosed_noignore;
    صحيح		b_ind_unclosed_wrapped;
    صحيح		b_ind_unclosed_whiteok;
    صحيح		b_ind_matching_paren;
    صحيح		b_ind_paren_prev;
    صحيح		b_ind_maxparen;
    صحيح		b_ind_maxcomment;
    صحيح		b_ind_scopedecl;
    صحيح		b_ind_scopedecl_ڪوڊ;
    صحيح		b_ind_java;
    صحيح		b_ind_js;
    صحيح		b_ind_keep_case_label;
    صحيح		b_ind_hash_comment;
    صحيح		b_ind_cpp_namespace;
    صحيح		b_ind_if_for_while;
    صحيح		b_ind_cpp_extern_c;
    صحيح		b_ind_pragma;

    linenr_T	b_no_eol_lnum;	// non-zero lnum when last line of next binary
				// write should not have an end-of-line

    صحيح		b_شروعات_eof;	// last line had eof (CTRL-Z) when it was read
    صحيح		b_شروعات_eol;	// last line had eol when it was read
    صحيح		b_شروعات_ffc;	// first char of 'ff' when edit started
    char_u	*b_شروعات_fenc;	// 'fileencoding' when edit started or NULL
    صحيح		b_bad_char;	// "++bad=" argument when edit started or 0
    صحيح		b_شروعات_bomb;	// 'bomb' when it was read

#جيڪڏهن_وصف_ٿيل FEAT_EVAL
    dictitem_T	b_bufvar;	// variable for "b:" Dictionary
    dict_T	*b_vars;	// internal variables, local to buffer

    listener_T	*b_listener;       // Listeners accepting buffered reports.
    listener_T	*b_sync_listener;  // Listeners requiring unbuffered reports.
    list_T	*b_recorded_changes;
    size_t	b_recorded_متن_سائيز;  // bytes of text held by the above
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
    bool	b_has_textprop;	// true when text props were added
    hashtab_T	*b_proptypes;	// text property types local to buffer
    proptype_T	**b_proparray;	// entries of b_proptypes sorted on tp_id
#جيڪڏهن_پورو

#إذا defined(FEAT_BEVAL) && defined(FEAT_EVAL)
    char_u	*b_p_bexpr;	// 'balloonexpr' local value
    long_u	b_p_bexpr_flags;// flags for 'balloonexpr'
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_CRYPT
    char_u	*b_p_cm;	// 'cryptmethod'
#جيڪڏهن_پورو

    // When a buffer is created, it starts without a swap file.  b_may_swap is
    // then set to indicate that a swap file may be opened later.  It is reset
    // if a swap file could not be opened.
    bool	b_may_مٽا_سٽا;
    bool	b_did_warn;	// Set to true if user has been warned on first
				// change of a read-only file

    // Two special kinds of buffers:
    // help buffer  - used for help files, won't use a swap file.
    // spell buffer - used for spell info, never displayed and doesn't have a
    //		      file name.
    bool	b_help;		// true for help file buffer (when set b_p_bt
				// is "help")
#جيڪڏهن_وصف_ٿيل FEAT_SPELL
    bool	b_spell;	// true for a spell file buffer, most fields
				// are not used!  Use the B_SPELL macro to
				// access b_spell without #ifdef.
#جيڪڏهن_پورو

    bool	b_shortname;	// this file has an 8.3 file name

#جيڪڏهن_وصف_ٿيل FEAT_ڪم_CHANNEL
    char_u	*b_prompt_متن;		// set by prompt_setprompt()
    callback_T	b_prompt_callback;	// set by prompt_setcallback()
    callback_T	b_prompt_مداخلت;	// set by prompt_setinterrupt()
    صحيح		b_prompt_وجهو;	// value for restart_edit when entering
					// a prompt buffer window.
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_MZSCHEME
    فراغ	*b_mzscheme_ref; // The MzScheme reference to this buffer
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_PERL
    فراغ	*b_perl_ذاتي;
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_PYTHON
    فراغ	*b_python_ref;	// The Python reference to this buffer
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_PYTHON3
    فراغ	*b_python3_ref;	// The Python3 reference to this buffer
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_TCL
    فراغ	*b_tcl_ref;
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_RUBY
    فراغ	*b_ruby_ref;
#جيڪڏهن_پورو

#إذا defined(FEAT_SYN_HL) || defined(FEAT_SPELL)
    synblock_T	b_s;		// Info related to syntax highlighting.  w_s
				// normally points to this, but some windows
				// may use a different synblock_T.
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_SIGNS
    sign_داخلا_T *b_signlist;	   // list of placed signs
    sign_داخلا_T *b_sign_finger;   // last sign inserted, used to speed up
				   // inserting signs in ascending line order
# ifdef FEAT_NETBEANS_INTG
    bool	b_has_sign_ڪالم; // Flag that is set when a first sign is
				   // added and remains set until the end of
				   // the netbeans session.
# endif
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_NETBEANS_INTG
    bool	b_netbeans_file;    // true when buffer is owned by NetBeans
    bool	b_was_netbeans_file;// true if b_netbeans_file was once set
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_ڪم_CHANNEL
    bool	b_write_منزل_channel; // true when appended lines are written to
				    // a channel.
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_CRYPT
    cryptstate_T *b_cryptstate;	// Encryption state while reading or writing
				// the file. NULL when not using encryption.
#جيڪڏهن_پورو
    صحيح		b_mapped_ctrl_c; // modes where CTRL-C is mapped

#جيڪڏهن_وصف_ٿيل FEAT_TERMINAL
    رڪن_T	*b_رڪن;	// When not NULL this buffer is for a terminal
				// window.
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_DIFF
    bool	b_diff_failed;	// internal diff failed for this buffer
#جيڪڏهن_پورو
}; // file_buffer


#إذا defined(FEAT_DIFF)
/*
 * Stuff for diff mode.
 */
# define DB_ڳڻپ 8	// up to eight buffers can be diff'ed

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
تعريفنوع بنية diffblock_S diff_T;
بنية diffblock_S
{
    diff_T	*df_ايندڙ;
    linenr_T	df_lnum[DB_ڳڻپ];	// line number in buffer
    linenr_T	df_ڳڻپ[DB_ڳڻپ];	// nr of inserted/changed lines
    صحيح is_linematched;  // has the linematch algorithm ran on this diff hunk to divide it into
			  // smaller diff hunks?

    صحيح		has_changes;		// has cached list of inline changes
    garray_T	df_changes;		// list of inline changes (diffline_change_T)
};

/*
 * Each entry stores a single inline change within a diff block. Line numbers
 * are recorded as relative offsets, and columns are byte offsets, not
 * character counts.
 * Ranges are [start,end), with the end being exclusive.
 */
تعريفنوع بنية diffline_change_S diffline_change_T;
بنية diffline_change_S
{
    colnr_T	dc_شروعات[DB_ڳڻپ];	// byte offset of start of range in the line
    colnr_T	dc_پڇاڙي[DB_ڳڻپ];	// 1 past byte offset of end of range in line
    صحيح		dc_شروعات_lnum_off[DB_ڳڻپ];	// starting line offset
    صحيح		dc_پڇاڙي_lnum_off[DB_ڳڻپ];	// end line offset
};

/*
 * Describes a single line's list of inline changes. Use diff_change_parse() to
 * parse this.
 */
تعريفنوع بنية diffline_S diffline_T;
بنية diffline_S
{
    diffline_change_T *changes;
    صحيح انگ_changes;
    صحيح bufidx;
    صحيح lineoff;
};
#وإلا  // FEAT_DIFF
تعريفنوع فراغ diffline_T;
تعريفنوع فراغ diffline_change_T;
#جيڪڏهن_پورو

#وصف_ڪريو SNAP_HELP_IDX	    0
#وصف_ڪريو SNAP_AUCMD_IDX	    1
#وصف_ڪريو SNAP_QUICKFIX_IDX   2
#وصف_ڪريو SNAP_ڳڻپ	    3

/*
 * Tab pages point to the top frame of each tab page.
 * Note: Most values are NOT valid for the current tab page!  Use "curwin",
 * "firstwin", etc. for that.  "tp_topframe" is always valid and can be
 * compared against "topframe" to find the current tab page.
 */
تعريفنوع بنية tabpage_S tabpage_T;
بنية tabpage_S
{
    tabpage_T	    *tp_ايندڙ;	    // next tabpage or NULL
    چوکٺو_T	    *tp_topframe;   // topframe for the windows
    win_T	    *tp_curwin;	    // current window in this Tab page
    win_T	    *tp_prevwin;    // previous window in this Tab page
    win_T	    *tp_firstwin;   // first window in this Tab page
    win_T	    *tp_lastwin;    // last window in this Tab page
#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
    win_T	    *tp_پهريون_popupwin; // first popup window in this Tab page
#جيڪڏهن_پورو
    ڊگهو	    tp_old_صفون;    // Rows when Tab page was left
    ڊگهو	    tp_old_Columns; // Columns when Tab page was left, -1 when
				    // calling shell_new_columns() postponed
    صحيح		    tp_old_coloff;  // Column offset when Tab page was left
    ڊگهو	    tp_ch_استعمال_ٿيل;	    // value of 'cmdheight' when frame size
				    // was set
#جيڪڏهن_وصف_ٿيل FEAT_GUI
    صحيح		    tp_prev_which_scrollbars[3];
				    // previous value of which_scrollbars
#جيڪڏهن_پورو
    صحيح		    tp_did_tabclosedpre;  // whether TabClosedPre was triggered

    char_u	    *tp_localdir;	// absolute path of local directory or
					// NULL
    char_u	    *tp_prevdir;	// previous directory

#جيڪڏهن_وصف_ٿيل FEAT_DIFF
    diff_T	    *tp_پهريون_diff;
    buf_T	    *(tp_diffbuf[DB_ڳڻپ]);
    صحيح		    tp_diff_invalid;	// list of diffs is outdated
    صحيح		    tp_diff_update;	// update diffs before redrawing
#جيڪڏهن_پورو
    چوکٺو_T	    *(tp_snapshot[SNAP_ڳڻپ]);  // window layout snapshots
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
    dictitem_T	    tp_winvar;	    // variable for "t:" Dictionary
    dict_T	    *tp_vars;	    // internal variables, local to tab page
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_PYTHON
    فراغ	    *tp_python_ref;	// The Python value for this tab page
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_PYTHON3
    فراغ	    *tp_python3_ref;	// The Python value for this tab page
#جيڪڏهن_پورو
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
تعريفنوع بنية w_سٽ
{
    linenr_T	wl_lnum;	// buffer line number for logical line
    short_u	wl_سائيز;	// height in screen lines
    حرف	wl_valid;	// TRUE values are valid for text in buffer
#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
    حرف	wl_folded;	// TRUE when this is a range of folded lines
    linenr_T	wl_lastlnum;	// last buffer line number for logical line
#جيڪڏهن_پورو
} wline_T;

/*
 * Windows are kept in a tree of frames.  Each frame has a column (FR_COL)
 * or row (FR_ROW) layout or is a leaf, which has a window.
 */
بنية چوکٺو_S
{
    حرف	fr_layout;	// FR_LEAF, FR_COL or FR_ROW
    صحيح		fr_ويڪر;
    صحيح		fr_newwidth;	// new width used in win_equal_rec()
    صحيح		fr_اوچائي;
    صحيح		fr_newheight;	// new height used in win_equal_rec()
    چوکٺو_T	*fr_والد;	// containing frame or NULL
    چوکٺو_T	*fr_ايندڙ;	// frame right or below in same parent, NULL
				// for last
    چوکٺو_T	*fr_prev;	// frame left or above in same parent, NULL
				// for first
    // fr_child and fr_win are mutually exclusive
    چوکٺو_T	*fr_ٻار;	// first contained frame
    win_T	*fr_win;	// window that fills this frame; for a snapshot
				// set to the current window
};

#وصف_ڪريو FR_پن	0	// frame is a leaf
#وصف_ڪريو FR_صف	1	// frame with a row of windows
#وصف_ڪريو FR_کانو	2	// frame with a column of windows

/*
 * Struct used for highlighting 'hlsearch' matches, matches defined by
 * ":match" and matches defined by match functions.
 * For 'hlsearch' there is one pattern for all windows.  For ":match" and the
 * match functions there is a different pattern for each window.
 */
تعريفنوع بنية
{
    regmmatch_T	rm;	    // points to the regexp program; contains last
			    // found match (may continue in next line)
    buf_T	*buf;	    // the buffer to search for a match
    linenr_T	lnum;	    // the line to search for a match
    صحيح		attr;	    // attributes to be used for a match
    صحيح		attr_cur;   // attributes currently active in win_line()
    linenr_T	پهريون_lnum; // first lnum to search for multi-line pat
    colnr_T	startcol;   // in win_line() points to char where HL starts
    colnr_T	endcol;	    // in win_line() points to char where HL ends
    حرف	is_addpos;  // position specified directly by
			    // matchaddpos(). TRUE/FALSE
    حرف	has_cursor; // TRUE if the cursor is inside the match, used for
			    // CurSearch
} ميلاپ_T;

/*
 * Same as lpos_T, but with additional field len.
 */
تعريفنوع بنية
{
    linenr_T	lnum;	// line number
    colnr_T	کانو;	// column number
    صحيح		len;	// length: 0 - to the end of line
} llpos_T;

/*
 * matchitem_T provides a linked list for storing match items for ":match",
 * matchadd() and matchaddpos().
 */
تعريفنوع بنية matchitem matchitem_T;
بنية matchitem
{
    matchitem_T	*mit_ايندڙ;
    صحيح		mit_id;		// match ID
    صحيح		mit_ترجيح;   // match priority

    // Either a pattern is defined (mit_pattern is not NUL) or a list of
    // positions is given (mit_pos is not NULL and mit_pos_count > 0).
    char_u	*mit_نمونو;   // pattern to highlight
    regmmatch_T	mit_ميلاپ;	// regexp program for pattern

    llpos_T	*mit_pos_array;	// array of positions
    صحيح		mit_pos_ڳڻپ;	// nr of entries in mit_pos
    صحيح		mit_pos_cur;	// internal position counter
    linenr_T	mit_toplnum;	// top buffer line
    linenr_T	mit_botlnum;	// bottom buffer line

    ميلاپ_T	mit_hl;		// struct for doing the actual highlighting
    صحيح		mit_hlg_id;	// highlight group ID
#جيڪڏهن_وصف_ٿيل FEAT_CONCEAL
    صحيح		mit_conceal_char; // cchar for Conceal highlighting
#جيڪڏهن_پورو
};

// Structure to store last cursor position and topline.  Used by check_lnums()
// and reset_lnums().
تعريفنوع بنية
{
    صحيح		w_topline_save;	// original topline value
    صحيح		w_topline_corr;	// corrected topline value
    pos_T	w_cursor_save;	// original cursor position
    pos_T	w_cursor_corr;	// corrected cursor position
} pos_save_T;

#جيڪڏهن_وصف_ٿيل FEAT_MENU
تعريفنوع بنية {
    صحيح		wb_startcol;
    صحيح		wb_endcol;
    vimmenu_T	*wb_menu;
} winbar_شي_T;
#جيڪڏهن_پورو

/*
 * Characters from the 'listchars' option
 */
تعريفنوع بنية
{
    صحيح		eol;
    صحيح		ext;
    صحيح		prec;
    صحيح		nbsp;
    صحيح		space;
    صحيح		tab1;
    صحيح		tab2;
    صحيح		tab3;
    صحيح		trail;
    صحيح		lead;
    صحيح		leadtab1;
    صحيح		leadtab2;
    صحيح		leadtab3;
    صحيح		*multispace;
    صحيح		*leadmultispace;
#جيڪڏهن_وصف_ٿيل FEAT_CONCEAL
    صحيح		conceal;
#جيڪڏهن_پورو
} lcs_chars_T;

/*
 * Characters from the 'fillchars' option
 */
تعريفنوع بنية
{
    صحيح	stl;
    صحيح	stlnc;
    صحيح	vert;
    صحيح	fold;
    صحيح	foldopen;
    صحيح	foldclosed;
    صحيح	foldsep;
    صحيح	foldinner;
    صحيح	diff;
    صحيح	eob;
    صحيح	lastline;
#إذا defined(FEAT_TABPANEL)
    صحيح	tpl_vert;
#جيڪڏهن_پورو
    صحيح trunc;
    صحيح truncrl;
} fill_chars_T;

/*
 * Represents current highlight overrides (used by 'winhighlight' option). The
 * highlight group with ID "from" will be overridden by the highlight group with
 * ID "to"
 */
تعريفنوع بنية
{
    صحيح from; // If zero or negative then it is hlf_T enum
    صحيح منزل; // Same thing as "from"
} hl_override_T;

/*
 * Structure which contains all information that belongs to a window
 *
 * All row numbers are relative to the start of the window, except w_winrow.
 */
بنية دري_S
{
    صحيح		w_id;		    // unique window ID

    buf_T	*w_بفر;	    // buffer we are a window into

    win_T	*w_prev;	    // link to previous window
    win_T	*w_ايندڙ;	    // link to next window

#إذا defined(FEAT_SYN_HL) || defined(FEAT_SPELL)
    synblock_T	*w_s;		    // for :ownsyntax
#جيڪڏهن_پورو

    صحيح		w_locked;	    // don't let autocommands close the window

    چوکٺو_T	*w_چوکٺو;	    // frame containing this window

    pos_T	w_cursor;	    // cursor position in buffer

    colnr_T	w_curswant;	    // The column we'd like to be at.  This is
				    // used to try to stay in the same column
				    // for up/down cursor motions.

    bool	w_سيٽ_curswant;	    // If set, then update w_curswant the next
				    // time through cursupdate() to the
				    // current virtual column

#جيڪڏهن_وصف_ٿيل FEAT_SYN_HL
    linenr_T	w_آخري_cursorline;  // where last time 'cursorline' was drawn
#جيڪڏهن_پورو

    /*
     * the next seven are used to update the Visual highlighting
     */
    حرف	w_old_visual_گهڻائي;  // last known VIsual_mode
    linenr_T	w_old_cursor_lnum;  // last known end of visual part
    colnr_T	w_old_cursor_fcol;  // first column for block visual part
    colnr_T	w_old_cursor_lcol;  // last column for block visual part
    linenr_T	w_old_visual_lnum;  // last known start of visual part
    colnr_T	w_old_visual_کانو;   // last known start of visual part
    colnr_T	w_old_curswant;	    // last known value of Curswant

    linenr_T    w_آخري_cursor_lnum_rnu;  // cursor lnum when 'rnu' was last
					 // redrawn

    lcs_chars_T	w_lcs_chars;	    // 'listchars' characters
    fill_chars_T w_fill_chars;	    // 'fillchars' characters

    /*
     * "w_topline", "w_leftcol" and "w_skipcol" specify the offsets for
     * displaying the buffer.
     */
    linenr_T	w_topline;	    // buffer line number of the line at the
				    // top of the window
    bool	w_topline_was_سيٽ;  // flag set to true when topline is set,
				    // e.g. by winrestview()

    linenr_T	w_botline;	    // number of the line below the bottom of
				    // the window

#جيڪڏهن_وصف_ٿيل FEAT_DIFF
    صحيح		w_topfill;	    // number of filler lines above w_topline
    صحيح		w_old_topfill;	    // w_topfill at last redraw
    bool	w_botfill;	    // true when filler lines are actually
				    // below w_topline (at end of file)
    bool	w_old_botfill;	    // w_botfill at last redraw
#جيڪڏهن_پورو
    colnr_T	w_leftcol;	    // screen column number of the left most
				    // character in the window; used when
				    // 'wrap' is off
    colnr_T	w_skipcol;	    // starting screen column for the first
				    // line in the window; used when 'wrap' is
				    // on; does not include win_col_off()

    صحيح		w_خالي_جايون_صفون;	    // number of ~ rows in window
#جيڪڏهن_وصف_ٿيل FEAT_DIFF
    صحيح		w_filler_صفون;	    // number of filler rows at the end of the
				    //دري
#جيڪڏهن_پورو

    // six fields that are only used when there is a WinScrolled autocommand
    linenr_T	w_آخري_topline;	    // last known value for w_topline
#جيڪڏهن_وصف_ٿيل FEAT_DIFF
    صحيح		w_آخري_topfill;	    // last known value for w_topfill
#جيڪڏهن_پورو
    colnr_T	w_آخري_leftcol;	    // last known value for w_leftcol
    colnr_T	w_آخري_skipcol;	    // last known value for w_skipcol
    صحيح		w_آخري_ويڪر;	    // last known value for w_width
    صحيح		w_آخري_اوچائي;	    // last known value for w_height

    /*
     * Layout of the window in the screen.
     * May need to add "msg_scrolled" to "w_winrow" in rare situations.
     */
    صحيح		w_winrow;	    // first row of window in screen
    صحيح		w_اوچائي;	    // number of rows in window, excluding
				    // status/command/winbar line(s)
    صحيح		w_prev_winrow;	    // previous winrow used for 'splitkeep'
    صحيح		w_prev_اوچائي;	    // previous height used for 'splitkeep'
    صحيح		w_stl_rendered_اوچائي; // rendered height of window-local 'stl'
				    // (number of "%@" + 1)
    stl_click_region_T *w_stl_click;  // statusline click regions
    صحيح		w_stl_click_ڳڻپ;  // number of click regions
    صحيح		w_حالت_اوچائي;    // number of status lines.
				    // If 'statuslineopt' was changed, this
				    // member holds the previous value until
				    // frame_change_statusline_height() calls.
    صحيح		w_wincol;	    // Leftmost column of window in screen.
    صحيح		w_ويڪر;	    // Width of window, excluding separation.
    صحيح		w_vsep_ويڪر;	    // Number of separator columns (0 or 1).

    pos_save_T	w_save_cursor;	    // backup of cursor pos and topline
    bool	w_do_win_fix_cursor;// if true cursor may be invalid

#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
    صحيح		w_popup_flags;	    // POPF_ values
    صحيح		w_popup_blend;	    // 0-100: transparency level for popup with opacitys
    صحيح		w_popup_handled;    // POPUP_HANDLE[0-9] flags
    char_u	*w_popup_title;
    poppos_T	w_popup_pos;
    bool	w_popup_fixed;	    // do not shift popup to fit on screen
    صحيح		w_popup_prop_type;  // when not zero: textprop type ID
    win_T	*w_popup_prop_win;  // window to search for textprop
    صحيح		w_popup_prop_id;    // when not zero: textprop ID
    صحيح		w_zindex;
    صحيح		w_minheight;	    // "minheight" for popup window
    صحيح		w_minwidth;	    // "minwidth" for popup window
    صحيح		w_maxheight;	    // "maxheight" for popup window
    صحيح		w_maxwidth;	    // "maxwidth" for popup window
    صحيح		w_maxwidth_opt;	    // maxwidth from option
    صحيح		w_wantline;	    // "line" for popup window
    صحيح		w_wantcol;	    // "col" for popup window
    صحيح		w_firstline;	    // "firstline" for popup window
    صحيح		w_want_scrollbar;   // when zero don't use a scrollbar
    bool	w_has_scrollbar;    // true if scrollbar displayed
    char_u	*w_scrollbar_highlight; // "scrollbarhighlight"
    char_u	*w_thumb_highlight; // "thumbhighlight"
    صحيح		w_popup_padding[4]; // popup padding top/right/bot/left
    صحيح		w_popup_border[4];  // popup border top/right/bot/left
    char_u	*w_border_highlight[4];  // popup border highlight
    bool	w_border_highlight_isset; // borderhighlight was explicitly set
    صحيح		w_border_char[8];   // popup border characters
    صحيح		w_popup_shadow;     // popup shadow (right and bottom edges)

    صحيح		w_popup_leftoff;    // columns left of the screen
    صحيح		w_popup_rightoff;   // columns right of the screen
    صحيح		w_popup_topoff;	    // rows above the host window's top
				    // when "clipwindow" is set
    صحيح		w_popup_bottomoff;  // rows below the host window's bottom
				    // when "clipwindow" is set
    صحيح		w_popup_leftclip;   // columns left of the host window's left
				    // when "clipwindow" is set
    صحيح		w_popup_rightclip;  // columns right of the host window's right
				    // when "clipwindow" is set
    varnumber_T	w_popup_آخري_changedtick; // b:changedtick of popup buffer
					  // when position was computed
    varnumber_T	w_popup_prop_changedtick; // b:changedtick of buffer with
					  // w_popup_prop_type when position
					  // was computed
    صحيح		w_popup_prop_topline; // w_topline of window with
				      // w_popup_prop_type when position was
				      // computed
    صحيح		w_popup_prop_winrow;  // w_winrow of host window when
				      // position was computed
    صحيح		w_popup_prop_wincol;  // w_wincol of host window when
				      // position was computed
    صحيح		w_popup_prop_ويڪر;   // w_width of host window when
				      // position was computed
    صحيح		w_popup_prop_winheight; // w_height of host window when
				      // position was computed
    linenr_T	w_popup_آخري_curline; // last known w_cursor.lnum of window
				      // with "cursorline" set
    callback_T	w_close_cb;	    // popup close callback
    callback_T	w_filter_cb;	    // popup filter callback
    صحيح		w_filter_errors;    // popup filter error count
    صحيح		w_filter_گهڻائي;	    // mode when filter callback is used

    win_T	*w_popup_curwin;    // close popup if curwin differs
    linenr_T	w_popup_lnum;	    // close popup if cursor not on this line
    colnr_T	w_popup_mincol;	    // close popup if cursor before this col
    colnr_T	w_popup_maxcol;	    // close popup if cursor after this col
    صحيح		w_popup_mouse_صف;  // close popup if mouse moves away
    صحيح		w_popup_mouse_mincol;  // close popup if mouse moves away
    صحيح		w_popup_mouse_maxcol;  // close popup if mouse moves away
    popclose_T	w_popup_close;	    // allow closing the popup with the mouse

    list_T	*w_popup_نقاب;	     // list of lists for "mask"
    char_u	*w_popup_نقاب_cells; // cached mask cells
    صحيح		w_popup_نقاب_اوچائي; // height of w_popup_mask_cells
    صحيح		w_popup_نقاب_ويڪر;  // width of w_popup_mask_cells

# إذا defined(FEAT_TIMERS)
    timer_T	*w_popup_timer;	    // timer for closing popup window
# endif

    صحيح		w_flags;	    // WFLAG_ flags

# define WFLAG_WCOL_OFF_ADDED	1   // popup border and padding were added to
				    // w_wcol
# define WFLAG_WROW_OFF_ADDED	2   // popup border and padding were added to
				    // w_wrow

# ifdef FEAT_IMAGE
    image_placement_T *w_popup_imagep;
# endif
#جيڪڏهن_پورو

    /*
     * === start of cached values ====
     */
    /*
     * Recomputing is minimized by storing the result of computations.
     * Use functions in screen.c to check if they are valid and to update.
     * w_valid is a bitfield of flags, which indicate if specific values are
     * valid or need to be recomputed.	See screen.c for values.
     */
    صحيح		w_valid;
    pos_T	w_valid_cursor;	    // last known position of w_cursor, used
				    // to adjust w_valid
    colnr_T	w_valid_leftcol;    // last known w_leftcol
    colnr_T	w_valid_skipcol;    // last known w_skipcol

    /*
     * w_cline_height is the number of physical lines taken by the buffer line
     * that the cursor is on.  We use this to avoid extra calls to plines().
     */
    صحيح		w_cline_اوچائي;	    // current size of cursor line
#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
    bool	w_cline_folded;	    // cursor line is folded
#جيڪڏهن_پورو

    صحيح		w_cline_صف;	    // starting row of the cursor line

    colnr_T	w_virtcol;	    // column number of the cursor in the
				    // buffer line, as opposed to the column
				    // number we're at on the screen.  This
				    // makes a difference on lines which span
				    // more than one screen line or when
				    // w_leftcol is non-zero

#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
    colnr_T	w_virtcol_پهريون_char;	// offset for w_virtcol when there are
					// virtual text properties above the
					//سٽ
#جيڪڏهن_پورو
    /*
     * w_wrow and w_wcol specify the cursor position in the window.
     * This is related to positions in the window, not in the display or
     * buffer, thus w_wrow is relative to w_winrow.
     */
    صحيح		w_wrow, w_wcol;	    // cursor position in window
#جيڪڏهن_وصف_ٿيل FEAT_CONCEAL
    صحيح		w_wcol_conceal_off; // screen cells concealed before w_wcol on
				    // the cursor's screen line, set by win_line()
#جيڪڏهن_پورو

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
    صحيح		w_سٽون_valid;	    // number of valid entries
    wline_T	*w_سٽون;

#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
    garray_T	w_folds;	    // array of nested folds
    bool	w_fold_manual;	    // when true: some folds are opened/closed
				    // manually
    bool	w_foldinvalid;	    // when true: folding needs to be
				    // recomputed
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_LINEBREAK
    صحيح		w_nrwidth;	    // width of 'number' and 'relativenumber'
				    // column being used
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_TERMINAL
    termcellcolor_T w_رڪن_hlfwin;  // cache for term color of HLF_WIN
#جيڪڏهن_پورو

    /*
     * === end of cached values ===
     */

    صحيح		w_redr_type;	    // type of redraw to be performed on win
    صحيح		w_upd_صفون;	    // number of window lines to update when
				    // w_redr_type is UPD_REDRAW_TOP
    linenr_T	w_redraw_چوٽي;	    // when != 0: first line needing redraw
    linenr_T	w_redraw_bot;	    // when != 0: last line needing redraw
    bool	w_redr_حالت;	    // if true status line must be redrawn

    // remember what is shown in the ruler for this window (if 'ruler' set)
    pos_T	w_ru_cursor;	    // cursor position shown in ruler
    colnr_T	w_ru_virtcol;	    // virtcol shown in ruler
    linenr_T	w_ru_topline;	    // topline shown in ruler
    linenr_T	w_ru_سٽ_ڳڻپ;    // line count used for ruler
#جيڪڏهن_وصف_ٿيل FEAT_DIFF
    صحيح		w_ru_topfill;	    // topfill shown in ruler
#جيڪڏهن_پورو
    bool	w_ru_خالي_جايون;	    // true if ruler shows 0-1 (empty line)

    صحيح		w_alt_fnum;	    // alternate file (for # and CTRL-^)

    alist_T	*w_alist;	    // pointer to arglist for this window
    صحيح		w_arg_idx;	    // current index in argument list (can be
				    // out of range!)
    bool	w_arg_idx_invalid;  // editing another file than w_arg_idx

    char_u	*w_localdir;	    // absolute path of local directory or
				    // NULL
    char_u	*w_prevdir;	    // previous directory
#جيڪڏهن_وصف_ٿيل FEAT_MENU
    vimmenu_T	*w_winbar;	    // The root of the WinBar menu hierarchy.
    winbar_شي_T *w_winbar_شيون;  // list of items in the WinBar
    صحيح		w_winbar_اوچائي;    // 1 if there is a window toolbar
#جيڪڏهن_پورو

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
#وصف_ڪريو GLOBAL_WO(p)	((حرف *)(p) + حجم(winopt_T))

#إذا defined(FEAT_SIGNS) || defined(FEAT_FOLDING) || defined(FEAT_DIFF)
    صحيح		*w_p_cc_کانن_ڳڻپ;	    // array of columns to highlight or NULL
    char_u	w_p_culopt_flags;   // flags for cursorline highlighting
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_LINEBREAK
    صحيح		w_briopt_min;	    // minimum width for breakindent
    صحيح		w_briopt_سرڪايو;	    // additional shift for breakindent
    صحيح		w_briopt_sbr;	    // sbr in 'briopt'
    صحيح		w_briopt_list;      // additional indent for lists
    صحيح		w_briopt_vcol;	    // indent for specific column
#جيڪڏهن_پورو

    ڊگهو	w_scbind_pos;

#جيڪڏهن_وصف_ٿيل FEAT_EVAL
    dictitem_T	w_winvar;	// variable for "w:" Dictionary
    dict_T	*w_vars;	// internal variables, local to window
#جيڪڏهن_پورو

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
    صحيح		w_jumplistlen;		// number of active entries
    صحيح		w_jumplistidx;		// current position

    صحيح		w_changelistidx;	// current position in b_changelist

#جيڪڏهن_وصف_ٿيل FEAT_ڳولا_EXTRA
    matchitem_T	*w_ميلاپ_سر;		// head of match list
    صحيح		w_ايندڙ_ميلاپ_id;	// next match ID
#جيڪڏهن_پورو

    /*
     * the tagstack grows from 0 upwards:
     * entry 0: older
     * entry 1: newer
     * entry 2: newest
     */
    taggy_T	w_tagstack[TAGSTACKSIZE];   // the tag stack
    صحيح		w_tagstackidx;		    // idx just below active entry
    صحيح		w_tagstacklen;		    // number of tags on stack

    /*
     * w_fraction is the fractional row of the cursor within the window, from
     * 0 at the top row to FRACTION_MULT at the last row.
     * w_prev_fraction_row was the actual cursor row when w_fraction was last
     * calculated.
     */
    صحيح		w_fraction;
    صحيح		w_prev_fraction_صف;

#جيڪڏهن_وصف_ٿيل FEAT_GUI
    scrollbar_T	w_scrollbars[2];	// vert. Scrollbars for this window
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_LINEBREAK
    linenr_T	w_nrwidth_سٽ_ڳڻپ;	// line count when ml_nrwidth_width
					// was computed.
    ڊگهو	w_nuw_cached;		// 'numberwidth' option cached
    صحيح		w_nrwidth_ويڪر;	// nr of chars to print line count.
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_QUICKFIX
    qf_info_T	*w_llist;		// Location list for this window
    /*
     * Location list reference used in the location list window.
     * In a non-location list window, w_llist_ref is NULL.
     */
    qf_info_T	*w_llist_ref;
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_MZSCHEME
    فراغ	*w_mzscheme_ref;	// The MzScheme value for this window
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_PERL
    فراغ	*w_perl_ذاتي;
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_PYTHON
    فراغ	*w_python_ref;		// The Python value for this window
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_PYTHON3
    فراغ	*w_python3_ref;		// The Python value for this window
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_TCL
    فراغ	*w_tcl_ref;
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_RUBY
    فراغ	*w_ruby_ref;
#جيڪڏهن_پورو

    hl_override_T *w_hl;
    صحيح		w_hl_len;
    صحيح		w_hlfwin_id; // Cached HLF_WIN highlight group id, zero if none,
			     // or -1 if it was set to itself.
};

/*
 * Arguments for operators.
 */
تعريفنوع بنية oparg_S
{
    صحيح		op_type;	// current pending operator type
    صحيح		regname;	// register to use for the operator
    صحيح		motion_type;	// type of the current cursor motion
    صحيح		motion_force;	// force motion type: 'v', 'V' or CTRL-V
    صحيح		use_reg_one;	// TRUE if delete uses reg 1 even when not
				// linewise
    صحيح		inclusive;	// TRUE if char motion is inclusive (only
				// valid when motion_type is MCHAR)
    صحيح		پڇاڙي_adjusted;	// backuped b_op_end one char (only used by
				// do_format())
    pos_T	شروعات;		// start of the operator
    pos_T	پڇاڙي;		// end of the operator
    pos_T	cursor_شروعات;	// cursor position before motion for "gw"

    ڊگهو	سٽ_ڳڻپ;	// number of lines from op_start to op_end
				// (inclusive)
    صحيح		خالي_جايون_2;		// op_start and op_end the same (only used by
				// do_change())
    صحيح		is_VIsual;	// operator on Visual area
    صحيح		بلاڪ_گهڻائي;	// current operator is Visual block mode
    colnr_T	شروعات_vcol;	// start col for block mode operator
    colnr_T	پڇاڙي_vcol;	// end col for block mode operator
    ڊگهو	prev_opcount;	// ca.opcount saved for K_CURSORHOLD
    ڊگهو	prev_count0;	// ca.count0 saved for K_CURSORHOLD
    صحيح		excl_tr_ws;	// exclude trailing whitespace for yank of a
				//بلاڪ
} oparg_T;

/*
 * Arguments for Normal mode commands.
 */
تعريفنوع بنية cmdarg_S
{
    oparg_T	*oap;		// Operator arguments
    صحيح		prechar;	// prefix character (optional, always 'g')
    صحيح		cmdchar;	// command character
    صحيح		nchar;		// next command character (optional)
    صحيح		ncharC1;	// first composing character (optional)
    صحيح		ncharC2;	// second composing character (optional)
    صحيح		extra_char;	// yet another character (optional)
    ڊگهو	opcount;	// count before an operator
    ڊگهو	count0;		// count before command, default 0
    ڊگهو	count1;		// count before command, default 1
    صحيح		arg;		// extra argument from nv_cmds[]
    صحيح		retval;		// return: CA_* values
    char_u	*searchbuf;	// return: pointer to search pattern or NULL
} cmdarg_T;

// values for retval:
#وصف_ڪريو CA_COMMAND_BUSY	    1	// skip restarting edit() once
#وصف_ڪريو CA_NO_ADJ_OP_پڇاڙي    2	// don't adjust operator end

#جيڪڏهن_وصف_ٿيل CURSOR_SHAPE
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
# define SHAPE_IDX_وڌيڪ	14	// Hit-return or More
# define SHAPE_IDX_MOREL	15	// Hit-return or More in last line
# define SHAPE_IDX_SM	16	// showing matching paren
# define SHAPE_IDX_ڳڻپ	17

# define SHAPE_بلاڪ	0	// block cursor
# define SHAPE_HOR	1	// horizontal bar cursor
# define SHAPE_VER	2	// vertical bar cursor

# define MSHAPE_NUMBERED	1000	// offset for shapes identified by number
# define MSHAPE_HIDE	1	// hide mouse pointer

# define SHAPE_MOUSE	1	// used for mouse pointer shape
# define SHAPE_CURSOR	2	// used for text cursor shape

تعريفنوع بنية cursor_داخلا
{
    صحيح		shape;		// one of the SHAPE_ defines
    صحيح		mshape;		// one of the MSHAPE defines
    صحيح		percentage;	// percentage of cell for bar
    ڊگهو	blinkwait;	// blinking, wait time before blinking starts
    ڊگهو	blinkon;	// blinking, on time
    ڊگهو	blinkoff;	// blinking, off time
    صحيح		id;		// highlight group ID
    صحيح		id_lm;		// highlight group ID for :lmap mode
    حرف	*name;		// mode name (fixed)
    حرف	استعمال_ٿيل_for;	// SHAPE_MOUSE and/or SHAPE_CURSOR
} cursorentry_T;
#جيڪڏهن_پورو // CURSOR_SHAPE

#جيڪڏهن_وصف_ٿيل FEAT_MENU

// Indices into vimmenu_T->strings[] and vimmenu_T->noremap[] for each mode
# define MENU_اشاريو_INVALID	-1
# define MENU_اشاريو_عام	0
# define MENU_اشاريو_VISUAL	1
# define MENU_اشاريو_SELECT	2
# define MENU_اشاريو_OP_PENDING	3
# define MENU_اشاريو_وجهو	4
# define MENU_اشاريو_CMDLINE	5
# define MENU_اشاريو_TERMINAL	6
# define MENU_اشاريو_TIP		7
# define MENU_MODES		8

// Menu modes
# define MENU_عام_گهڻائي	(1 << MENU_اشاريو_عام)
# define MENU_VISUAL_گهڻائي	(1 << MENU_اشاريو_VISUAL)
# define MENU_SELECT_گهڻائي	(1 << MENU_اشاريو_SELECT)
# define MENU_OP_PENDING_گهڻائي	(1 << MENU_اشاريو_OP_PENDING)
# define MENU_وجهو_گهڻائي	(1 << MENU_اشاريو_وجهو)
# define MENU_CMDLINE_گهڻائي	(1 << MENU_اشاريو_CMDLINE)
# define MENU_TERMINAL_گهڻائي	(1 << MENU_اشاريو_TERMINAL)
# define MENU_TIP_گهڻائي		(1 << MENU_اشاريو_TIP)
# define MENU_ALL_MODES		((1 << MENU_اشاريو_TIP) - 1)
// note MENU_INDEX_TIP is not a 'real' mode

// Start a menu name with this to not include it on the main menu bar
# define MNU_HIDDEN_CHAR		']'

بنية VimMenu
{
    صحيح		modes;		    // Which modes is this menu visible for?
    صحيح		enabled;	    // for which modes the menu is enabled
    char_u	*name;		    // Name of menu, possibly translated
    char_u	*dname;		    // Displayed Name ("name" without '&')
# ifdef FEAT_MULTI_LANG
    char_u	*en_name;	    // "name" untranslated, NULL when "name"
				    // was not translated
    char_u	*en_dname;	    // "dname" untranslated, NULL when "dname"
				    // was not translated
# endif
    char_u	*actext;	    // accelerator text (after TAB)
    صحيح		mnemonic;	    // mnemonic key (after '&')
    صحيح		ترجيح;	    // Menu order priority
# ifdef FEAT_GUI
    فراغ	(*cb)(vimmenu_T *); // Call-back function
# endif
# ifdef FEAT_TOOLBAR
    char_u	*iconfile;	    // name of file for icon or NULL
    صحيح		iconidx;	    // icon index (-1 if not set)
    صحيح		icon_builtin;	    // icon names is BuiltIn{nr}
# endif
    char_u	*لڙيون[MENU_MODES]; // Mapped string for each mode
    صحيح		noremap[MENU_MODES]; // A REMAP_ flag for each mode
    حرف	silent[MENU_MODES]; // A silent flag for each mode
    vimmenu_T	*children;	    // Children of sub-menu
    vimmenu_T	*والد;	    // Parent of menu
    vimmenu_T	*ايندڙ;		    // Next item in menu
# ifdef FEAT_GUI_X11
    Widget	id;		    // Manage this to enable item
    Widget	submenu_id;	    // If this is submenu, add children here
# endif
# ifdef FEAT_GUI_GTK
    GtkWidget	*id;		    // Manage this to enable item
    GtkWidget	*submenu_id;	    // If this is submenu, add children here
#  إذا defined(GTK_CHECK_نسخو) && !GTK_CHECK_نسخو(3,4,0)
    GtkWidget	*tearoff_handle;
#  endif
#  ifndef USE_GTK4
    GtkWidget   *label;		    // Used by "set wak=" code.
#  endif
# endif
# ifdef FEAT_GUI_MOTIF
    صحيح		sensitive;	    // turn button on/off
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
    BMenu_شي  *id;		    // Id of menu item
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
#وإلا
// For generating prototypes when FEAT_MENU isn't defined.
تعريفنوع صحيح vimmenu_T;

#جيڪڏهن_پورو // FEAT_MENU

/*
 * Struct to save values in before executing autocommands for a buffer that is
 * not the current buffer.
 */
تعريفنوع بنية
{
    صحيح		use_aucmd_win_idx;  // index in aucmd_win[] if >= 0
    صحيح		save_curwin_id;	    // ID of saved curwin
    صحيح		new_curwin_id;	    // ID of new curwin
    صحيح		save_prevwin_id;    // ID of saved prevwin
    bufref_T	new_curbuf;	    // new curbuf
    char_u	*tp_localdir;	    // saved value of tp_localdir
    char_u	*globaldir;	    // saved value of globaldir
    صحيح		save_VIsual_سرگرم; // saved VIsual_active
#جيڪڏهن_وصف_ٿيل FEAT_ڪم_CHANNEL
    صحيح		save_prompt_وجهو; // saved b_prompt_insert
#جيڪڏهن_پورو
} aco_save_T;

/*
 * Generic option table item, only used for printer at the moment.
 */
تعريفنوع بنية
{
    ثابت حرف	*name;
    صحيح		hasnum;
    ڊگهو	عدد;
    char_u	*string;	// points into option string
    صحيح		لڙي_ڊيگهه;
    صحيح		present;
} option_جدول_T;

/*
 * Structure to hold printing color and font attributes.
 */
تعريفنوع بنية
{
    long_u	fg_color;
    long_u	bg_color;
    صحيح		bold;
    صحيح		italic;
    صحيح		underline;
    صحيح		undercurl;
} prt_متن_attr_T;

/*
 * Structure passed back to the generic printer code.
 */
تعريفنوع بنية
{
    صحيح		n_collated_copies;
    صحيح		n_uncollated_copies;
    صحيح		duplex;
    صحيح		chars_في_سٽ;
    صحيح		سٽون_في_صفحو;
    صحيح		has_color;
    prt_متن_attr_T عدد;
#جيڪڏهن_وصف_ٿيل FEAT_SYN_HL
    صحيح		modec;
    صحيح		do_syntax;
#جيڪڏهن_پورو
    صحيح		user_abort;
#جيڪڏهن_وصف_ٿيل FEAT_PRINT_PANGO
    صحيح		user_abort_نياپو;
#جيڪڏهن_پورو
    char_u	*jobname;
#إذا defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
    char_u	*outfile;
    char_u	*arguments;
#جيڪڏهن_پورو
} prt_settings_T;

#وصف_ڪريو PRINT_عدد_ويڪر 8

/*
 * Used for popup menu items.
 */
تعريفنوع بنية
{
    char_u	*pum_متن;		// main menu text
    char_u	*pum_قسم;		// extra kind text (may be truncated)
    char_u	*pum_extra;		// extra menu text (may be truncated)
    char_u	*pum_info;		// extra info
    صحيح		pum_cpt_ذريعو_idx;	// index of completion source in 'cpt'
    صحيح		pum_user_abbr_hlattr;	// highlight attribute for abbr
    صحيح		pum_user_قسم_hlattr;	// highlight attribute for kind
} pumitem_T;

/*
 * Structure used for get_tagfname().
 */
تعريفنوع بنية
{
    char_u	*tn_ٽيگ_فهرست;	// value of 'tags' when starting
    char_u	*tn_np;		// current position in tn_tags
    صحيح		tn_did_filefind_init;
    صحيح		tn_hf_idx;
    فراغ	*tn_ڳولا_ctx;
} tagname_T;

تعريفنوع بنية {
    UINT32_T ڪل[2];
    UINT32_T state[8];
    char_u   بفر[64];
} context_sha256_T;

/*
 * types for expressions.
 */
تعريفنوع تعداد
{
    اظهار_UNKNOWN = 0,
    اظهار_EQUAL,		// ==
    اظهار_NEQUAL,	// !=
    اظهار_GREATER,	// >
    اظهار_GEQUAL,	// >=
    اظهار_SMALLER,	// <
    اظهار_SEQUAL,	// <=
    اظهار_ميلاپ,		// =~
    اظهار_NOMATCH,	// !~
    اظهار_IS,		// is
    اظهار_ISNOT,		// isnot
    // used with ISN_OPNR
    اظهار_جوڙيو,		// +
    اظهار_SUB,		// -
    اظهار_MULT,		// *
    اظهار_DIV,		// /
    اظهار_REM,		// %
    اظهار_LSHIFT,	// <<
    اظهار_RSHIFT,	// >>
    // used with ISN_ADDLIST
    اظهار_COPY,		// create new list
    اظهار_APPEND,	// append to first list
} exprtype_T;

/*
 * Structure used for reading in json_decode().
 */
بنية js_reader
{
    char_u	*js_buf;	// text to be decoded
    char_u	*js_پڇاڙي;	// NUL in js_buf
    صحيح		js_استعمال_ٿيل;	// bytes used from js_buf
    صحيح		(*js_fill)(بنية js_reader *);
				// function to fill the buffer or NULL;
				// return TRUE when the buffer was filled
    فراغ	*js_cookie;	// can be used by js_fill
    صحيح		js_cookie_arg;	// can be used by js_fill
};
تعريفنوع بنية js_reader js_read_T;

// Maximum number of commands from + or -c arguments.
#وصف_ڪريو MAX_ARG_CMDS 10

// values for "window_layout"
#وصف_ڪريو WIN_HOR	    1	    // "-o" horizontally split windows
#وصف_ڪريو	WIN_VER	    2	    // "-O" vertically split windows
#وصف_ڪريو	WIN_TABS    3	    // "-p" windows on tab pages

// Struct for various parameters passed between main() and other functions.
تعريفنوع بنية
{
    صحيح		argc;
    حرف	**argv;

    char_u	*fname;			// first file to edit

    صحيح		evim_گهڻائي;		// started as "evim"
    char_u	*use_vimrc;		// vimrc from -u argument
    صحيح		clean;			// --clean argument

    صحيح		n_commands;		     // no. of commands from + or -c
    char_u	*commands[MAX_ARG_CMDS];     // commands from + or -c arg.
    char_u	cmds_tofree[MAX_ARG_CMDS];   // commands that need free()
    صحيح		n_pre_commands;		     // no. of commands from --cmd
    char_u	*pre_commands[MAX_ARG_CMDS]; // commands from --cmd argument

    صحيح		edit_type;		// type of editing to do
    char_u	*tagname;		// tag from -t argument
#جيڪڏهن_وصف_ٿيل FEAT_QUICKFIX
    char_u	*use_ef;		// 'errorfile' from -q argument
#جيڪڏهن_پورو

    صحيح		want_ڀريل_screen;
    صحيح		not_a_رڪن;		// no warning for missing term?
#جيڪڏهن_وصف_ٿيل FEAT_GUI
    char_u	*gui_dialog_file;	// file to write dialog text in
#جيڪڏهن_پورو
    صحيح		tty_fail;		// exit if not a tty
    char_u	*رڪن;			// specified terminal name
#جيڪڏهن_وصف_ٿيل FEAT_CRYPT
    صحيح		ask_for_ڪنجي;		// -x argument
#جيڪڏهن_پورو
    صحيح		no_مٽا_سٽا_file;		// "-n" argument used
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
    صحيح		use_debug_break_level;
#جيڪڏهن_پورو
    صحيح		دري_ڳڻپ;		// number of windows to use
    صحيح		دري_layout;		// 0, WIN_HOR, WIN_VER or WIN_TABS

#جيڪڏهن_وصف_ٿيل FEAT_CLIENTSERVER
    صحيح		serverArg;		// TRUE when argument for a server
    char_u	*serverName_arg;	// cmdline arg for server name
    char_u	*serverStr;		// remote server command
    char_u	*serverStrEnc;		// encoding of serverStr
    char_u	*servername;		// allocated name for our server
#جيڪڏهن_پورو
#إذا !defined(UNIX)
# define EXPAND_FILENAMES
    صحيح		literal;		// don't expand file names
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل MSWIN
    صحيح		ڀريل_رستو;		// file name argument was full path
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_DIFF
    صحيح		diff_گهڻائي;		// start with 'diff' set
#جيڪڏهن_پورو
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
تعريفنوع بنية lval_S
{
    char_u	*ll_name;	// start of variable name (can be NULL)
    char_u	*ll_name_پڇاڙي;	// end of variable name (can be NULL)
    type_T	*ll_type;	// type of variable (can be NULL)
    char_u	*ll_exp_name;	// NULL or expanded name in allocated memory.

    scid_T	ll_sid;		// for an imported item: the script ID it was
				// imported from; zero otherwise

    typval_T	*ll_tv;		// Typeval of item being used.  If "newkey"
				// isn't NULL it's the Dict to which to add
				// the item.
    listitem_T	*ll_li;		// The list item or NULL.
    list_T	*ll_list;	// The list or NULL.
    صحيح		ll_range;	// TRUE when a [i:j] range was used
    صحيح		ll_empty2;	// Second index is empty: [i:]
    ڊگهو	ll_n1;		// First index for list
    ڊگهو	ll_n2;		// Second index for list range
    dict_T	*ll_dict;	// The Dictionary or NULL
    dictitem_T	*ll_di;		// The dictitem or NULL
    char_u	*ll_newkey;	// New key for Dict in alloc. mem or NULL.
    type_T	*ll_valtype;	// type expected for the value or NULL
    blob_T	*ll_blob;	// The Blob or NULL
    tuple_T	*ll_tuple;	// tuple or NULL
    ufunc_T	*ll_ufunc;	// The function or NULL
    object_T	*ll_object;	// The object or NULL, class is not NULL
    class_T	*ll_class;	// The class or NULL, object may be NULL
    صحيح		ll_oi;		// The object/class member index
    صحيح		ll_is_پاڙ;	// TRUE if ll_tv is the lval_root, like a
				// plain object/class. ll_tv is variable.
    garray_T	ll_type_list;   // list of pointers to allocated types
} lval_T;

/**
 * This specifies optional parameters for get_lval(). Arguments may be NULL.
 */
تعريفنوع بنية lval_پاڙ_S {
    typval_T	*lr_tv;		// Base typval.
    class_T	*lr_cl_exec;	// Executing class for access checking.
    صحيح		lr_is_arg;	// name is an arg (not a member).
} lval_پاڙ_T;

// Structure used to save the current state.  Used when executing Normal mode
// commands while in any other mode.
تعريفنوع بنية {
    صحيح		save_نياپو_scroll;
    صحيح		save_restart_edit;
    صحيح		save_نياپو_didout;
    صحيح		save_State;
    صحيح		save_insertmode;
    صحيح		save_پڄاڻي_op;
    صحيح		save_opcount;
    صحيح		save_reg_executing;
    صحيح		save_pending_پڇاڙي_reg_executing;
    صحيح		save_script_نسخو;
    tasave_T	tabuf;
} save_state_T;

تعريفنوع بنية {
    varnumber_T vv_prevcount;
    varnumber_T vv_ڳڻپ;
    varnumber_T vv_count1;
} vimvars_save_T;

// Scope for changing directory
تعريفنوع تعداد {
    CDSCOPE_GLOBAL,	// :cd
    CDSCOPE_TABPAGE,	// :tcd
    CDSCOPE_دري	// :lcd
} cdscope_T;

// Variable flavor
تعريفنوع تعداد
{
    VAR_FLAVOUR_DEFAULT,	// doesn't start with uppercase
    VAR_FLAVOUR_SESSION,	// starts with uppercase, some lower
    VAR_FLAVOUR_VIMINFO		// all uppercase
} var_flavour_T;

// argument for mouse_find_win()
تعريفنوع تعداد {
    IGNORE_POPUP,	// only check non-popup windows
    ڳوليو_POPUP,		// also find popup windows
    FAIL_POPUP		// return NULL if mouse on popup window
} mouse_ڳوليو_T;

// Symbolic names for some registers.
#وصف_ڪريو DELETION_رجسٽر	36
#إذا defined(FEAT_CLIPBOARD) || defined(HAVE_CLIPMETHOD)
# define تارو_رجسٽر		37
# إذا defined(FEAT_X11) || defined(FEAT_WAYLAND)
#  define PLUS_رجسٽر	38
#  define REAL_PLUS_رجسٽر	PLUS_رجسٽر
# وإلا
#  define PLUS_رجسٽر	تارو_رجسٽر	    // there is only one
#  ifdef FEAT_EVAL
// Make it so that if clipmethod is "none", the plus register is not available,
// but if clipmethod is a provider, then expose the plus register for use.
#   define REAL_PLUS_رجسٽر	38
#  وإلا
#   define REAL_PLUS_رجسٽر	تارو_رجسٽر
#  endif
# endif
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_DND
# define TILDE_رجسٽر		(PLUS_رجسٽر + 1)
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_CLIPBOARD
# ifdef FEAT_DND
#  define انگ_REGISTERS		(TILDE_رجسٽر + 1)
# وإلا
#  define انگ_REGISTERS		(REAL_PLUS_رجسٽر + 1)
# endif
#وإلا
# ifdef HAVE_CLIPMETHOD
#  define انگ_REGISTERS		(REAL_PLUS_رجسٽر + 1)
# وإلا
#  define انگ_REGISTERS		37
# endif
#جيڪڏهن_پورو

// structure used by block_prep, op_delete and op_yank for blockwise operators
// also op_change, op_shift, op_insert, op_replace - AKelly
بنية بلاڪ_def
{
    صحيح		startspaces;	// 'extra' cols before first char
    صحيح		endspaces;	// 'extra' cols after last char
    صحيح		textlen;	// chars in block
    char_u	*textstart;	// pointer to 1st char (partially) in block
    colnr_T	textcol;	// index of chars (partially) in block
    colnr_T	شروعات_vcol;	// start col of 1st char wholly inside block
    colnr_T	پڇاڙي_vcol;	// start col of 1st char wholly after block
    صحيح		is_short;	// TRUE if line is too short to fit in block
    صحيح		is_MAX;		// TRUE if curswant==MAXCOL when starting
    صحيح		is_oneChar;	// TRUE if block within one character
    صحيح		pre_whitesp;	// screen cols of ws before block
    صحيح		pre_whitesp_c;	// chars of ws before block
    colnr_T	پڇاڙي_char_vcols;	// number of vcols of post-block char
    colnr_T	شروعات_char_vcols; // number of vcols of pre-block char
};

// Each yank register has an array of pointers to lines.
تعريفنوع بنية
{
    string_T	*y_array;	// pointer to array of string_T structs
    linenr_T	y_سائيز;		// number of lines in y_array
    char_u	y_type;		// MLINE, MCHAR or MBLOCK
    colnr_T	y_ويڪر;	// only set if y_type == MBLOCK
#جيڪڏهن_وصف_ٿيل FEAT_VIMINFO
    time_t	y_وقت_سيٽ;
#جيڪڏهن_پورو
} yankreg_T;

// The offset for a search command is store in a soff struct
// Note: only spats[0].off is really used
تعريفنوع بنية soffset
{
    صحيح		dir;		// search direction, '/' or '?'
    صحيح		سٽ;		// search has line offset
    صحيح		پڇاڙي;		// search set cursor at end
    ڊگهو	off;		// line or char offset
} soffset_T;

// A search pattern and its attributes are stored in a spat struct
تعريفنوع بنية spat
{
    char_u	    *pat;	// the pattern (in allocated memory) or NULL
    size_t	    patlen;	// the length of the pattern (0 if pat is NULL)
    صحيح		    magic;	// magicness of the pattern
    صحيح		    no_scs;	// no smartcase for this pattern
    soffset_T	    off;
} spat_T;

/*
 * Optional extra arguments for searchit().
 */
تعريفنوع بنية
{
    linenr_T	sa_stop_lnum;	// stop after this line number when != 0
#جيڪڏهن_وصف_ٿيل FEAT_RELTIME
    ڊگهو	sa_tm;		// timeout limit or zero
    صحيح		sa_timed_ٻاهريون;	// set when timed out
#جيڪڏهن_پورو
    صحيح		sa_wrapped;	// search wrapped around
} searchit_arg_T;

/*
 * Cookie used by getsourceline().
 */
/*
 * Cookie used to store info for each sourced file.
 * It is shared between do_source() and getsourceline().
 * This is passed to do_cmdline().
 */
تعريفنوع بنية {
    FILE	*fp;		// opened file for sourcing
    char_u	*nextline;	// if not NULL: line that was read ahead
    linenr_T	sourcing_lnum;	// line number of the source file
    صحيح		ختم;	// ":finish" used
    صحيح		ذريعو_from_buf;// TRUE if sourcing from current buffer
    صحيح		buf_lnum;	// line number in the current buffer
    garray_T	buflines;	// lines in the current buffer
#جيڪڏهن_وصف_ٿيل USE_CRNL
    صحيح		fileformat;	// EOL_UNKNOWN, EOL_UNIX or EOL_DOS
    صحيح		error;		// TRUE if LF found after CR-LF
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
    linenr_T	breakpoint;	// next line with breakpoint or zero
    char_u	*fname;		// name of sourced file
    صحيح		dbg_tick;	// debug_tick when breakpoint was set
    صحيح		level;		// top nesting level of sourced file
#جيڪڏهن_پورو
    vimconv_T	conv;		// type of conversion
} ذريعو_cookie_T;


#وصف_ڪريو WRITEBUFSIZE	8192	// size of normal write buffer

#وصف_ڪريو FIO_LATIN1	0x01	// convert Latin1
#وصف_ڪريو FIO_UTF8	0x02	// convert UTF-8
#وصف_ڪريو FIO_UCS2	0x04	// convert UCS-2
#وصف_ڪريو FIO_UCS4	0x08	// convert UCS-4
#وصف_ڪريو FIO_UTF16	0x10	// convert UTF-16
#جيڪڏهن_وصف_ٿيل MSWIN
# define FIO_CODEPAGE	0x20	// convert MS-Windows codepage
# define FIO_PUT_CP(x) (((x) & 0xffff) << 16)	// put codepage in top word
# define FIO_GET_CP(x)	(((x)>>16) & 0xffff)	// get codepage from top word
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل MACOS_CONVERT
# define FIO_MACROMAN	0x20	// convert MacRoman
#جيڪڏهن_پورو
#وصف_ڪريو FIO_ENDIAN_L	0x80	// little endian
#وصف_ڪريو FIO_ENCRYPTED	0x1000	// encrypt written bytes
#وصف_ڪريو FIO_NOCONVERT	0x2000	// skip encoding conversion
#وصف_ڪريو FIO_UCSBOM	0x4000	// check for BOM at start of file
#وصف_ڪريو FIO_ALL	(-1)	// allow all formats

// When converting, a read() or write() may leave some bytes to be converted
// for the next call.  The value is guessed...
#وصف_ڪريو CONV_RESTLEN 30

// We have to guess how much a sequence of bytes may expand when converting
// with iconv() to be able to allocate a buffer.
#وصف_ڪريو ICONV_MULT 8

// Used for "magic_overruled".
تعريفنوع تعداد {
    OPTION_MAGIC_NOT_سيٽ,	// p_magic not overruled
    OPTION_MAGIC_ON,		// magic on inside regexp
    OPTION_MAGIC_OFF		// magic off inside regexp
} optmagic_T;

// Magicness of a pattern, used by regexp code.
// The order and values matter:
//  magic <= MAGIC_OFF includes MAGIC_NONE
//  magic >= MAGIC_ON  includes MAGIC_ALL
تعريفنوع تعداد {
    MAGIC_NONE = 1,		// "\V" very unmagic
    MAGIC_OFF = 2,		// "\M" or 'magic' off
    MAGIC_ON = 3,		// "\m" or 'magic'
    MAGIC_ALL = 4		// "\v" very magic
} magic_T;

تعريفنوع تعداد {
    WT_UNKNOWN = 0,	// Unknown or unspecified location
    WT_دليل,
    WT_VARIABLE,
    WT_MEMBER,
    WT_طريقو,		// object method
    WT_طريقو_ARG,	// object method argument type
    WT_طريقو_RETURN,	// object method return type
    WT_CAST,		// type cast
} wherekind_T;

// Struct used to pass the location of a type check.  Used in error messages to
// indicate where the error happened.  Also used for doing covariance type
// check for object method return type and contra-variance type check for
// object method arguments.
تعريفنوع بنية {
    حرف	*wt_func_name;  // function name or NULL
    حرف	wt_اشاريو;	// argument or variable index, 0 means unknown
    wherekind_T	wt_قسم;	// type check location
} where_T;

#وصف_ڪريو WHERE_INIT {عدم, 0, WT_UNKNOWN}

// Struct passed to get_v_event() and restore_v_event().
تعريفنوع بنية {
    صحيح		sve_did_save;
    hashtab_T	sve_hashtab;
} save_v_event_T;

// Enum used by filter(), map(), mapnew() and foreach()
تعريفنوع تعداد {
    FILTERMAP_FILTER,
    FILTERMAP_MAP,
    FILTERMAP_MAPNEW,
    FILTERMAP_FOREACH
} filtermap_T;

// Structure used by switch_win() to pass values to restore_win()
تعريفنوع بنية {
    win_T	*sw_curwin;
    tabpage_T	*sw_curtab;
    صحيح		sw_same_win;	    // VIsual_active was not reset
    صحيح		sw_visual_سرگرم;
} switchwin_T;

// Fuzzy matched string list item. Used for fuzzy match completion. Items are
// usually sorted by 'score'. The 'idx' member is used for stable-sort.
تعريفنوع بنية {
    صحيح		idx;
    char_u	*str;
    صحيح		score;
} fuzmatch_str_T;

// Argument for lbr_chartabsize().
تعريفنوع بنية {
    win_T	*cts_win;
    char_u	*cts_سٽ;		// start of the line
    char_u	*cts_ptr;		// current position in line
#جيڪڏهن_وصف_ٿيل FEAT_LINEBREAK
    صحيح		cts_bri_سائيز;		// cached size of 'breakindent', or -1
					// if not computed yet
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
    صحيح		cts_متن_prop_ڳڻپ;	// number of text props; when zero
					// cts_text_props is not used
    textprop_T	*cts_متن_props;	// text props (allocated)
    حرف	cts_has_prop_with_متن;	// TRUE if a property inserts text
    صحيح		cts_cur_متن_ويڪر;	// width of current inserted text
    صحيح		cts_prop_سٽون;		// nr of properties above or below
    bool	cts_has_هيٺ;		// true if a text property below was
					// counted, its width fills up the line
    صحيح		cts_پهريون_char;		// width text props above the line
    صحيح		cts_above_ويڪر;	// width of text props above the line,
					// kept for the whole line
    bool	cts_no_above;		// do not count the width of text props
					// above the line
    صحيح		cts_with_trailing;	// include size of trailing props with
					// last character
    صحيح		cts_شروعات_incl;		// prop has true "start_incl" arg
#جيڪڏهن_پورو
    صحيح		cts_vcol;		// virtual column at current position
    صحيح		cts_max_سر_vcol;	// see win_lbr_chartabsize()
} chartabsize_T;

/*
 * Argument for the callback function (opt_did_set_cb_T) invoked after an
 * option value is modified.
 */
تعريفنوع بنية
{
    // Pointer to the option variable.  The variable can be a long (numeric
    // option), an int (boolean option) or a char pointer (string option).
    char_u	*os_varp;
    صحيح		os_idx;
    صحيح		os_flags;
    سيٽ_op_T	os_op;

    // old value of the option (can be a string, number or a boolean)
    اتحاد
    {
	ڊگهو	عدد;
	صحيح	boolean;
	char_u	*string;
    } os_oldval;

    // new value of the option (can be a string, number or a boolean)
    اتحاد
    {
	ڊگهو	عدد;
	صحيح	boolean;
	char_u	*string;
    } os_newval;

    // Option value was checked to be safe, no need to set P_INSECURE
    // Used for the 'keymap', 'filetype' and 'syntax' options.
    صحيح		os_قدر_checked;
    // Option value changed.  Used for the 'filetype' and 'syntax' options.
    صحيح		os_قدر_بدليو;

    // Used by the 'isident', 'iskeyword', 'isprint' and 'isfname' options.
    // Set to TRUE if the character table is modified when processing the
    // option and need to be restored because of a failure.
    صحيح		os_restore_chartab;

#إذا defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
    // Used by the 't_xxx' terminal options on MS-Windows.
    صحيح		os_did_swaptcap;
#جيڪڏهن_پورو

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
تعريفنوع بنية
{
    // Pointer to the option variable. It's always a string.
    char_u	*oe_varp;
    // The original option value, escaped.
    char_u	*oe_opt_قدر;

    // TRUE if using set+= instead of set=
    صحيح		oe_append;
    // TRUE if we would like to add the original option value as the first
    // choice.
    صحيح		oe_include_orig_val;

    // Regex from the cmdline, for matching potential options against.
    regmatch_T	*oe_regmatch;
    // The expansion context.
    expand_T	*oe_xp;

    // The full argument passed to :set. For example, if the user inputs
    // ':set dip=icase,algorithm:my<Tab>', oe_xp->xp_pattern will only have
    // 'my', but oe_set_arg will contain the whole 'icase,algorithm:my'.
    char_u	*oe_سيٽ_arg;
} optexpand_T;

/*
 * Spell checking variables passed from win_update() to win_line().
 */
تعريفنوع بنية {
    صحيح		spv_has_spell;	    // drawn window has spell checking
#جيڪڏهن_وصف_ٿيل FEAT_SPELL
    صحيح		spv_unchanged;	    // not updating for changed text
    صحيح		spv_checked_کانو;    // column in "checked_lnum" up to
				    // which there are no spell errors
    linenr_T	spv_checked_lnum;   // line number for "checked_col"
    صحيح		spv_cap_کانو;	    // column to check for Cap word
    linenr_T	spv_capcol_lnum;    // line number for "cap_col"
#جيڪڏهن_پورو
} spellvars_T;

// Return the length of a string literal.
// This macro only computes a string's length for a string-literal token; for
// anything else, including a char*, compilation will fail (note "" following
// s).
#وصف_ڪريو STRLEN_LITERAL(s) (حجم(s "") - 1)

// Store a key/value (string) pair
تعريفنوع بنية
{
    صحيح	    ڪنجي;        // the key
    string_T قدر;	// the value
} keyvalue_T;

#وصف_ڪريو KEYVALUE_داخلا(k, v) \
    {(k), {((char_u *)v), STRLEN_LITERAL(v)}}

#إذا defined(UNIX) || defined(MSWIN) || defined(VMS) || defined(AMIGA)
// Defined as signed, to return -1 on error
بنية cellsize {
    صحيح cs_xpixel;
    صحيح cs_ypixel;
};
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_WAYLAND

تعريفنوع بنية vwl_connection_S vwl_connection_T;
تعريفنوع بنية vwl_seat_S vwl_seat_T;

# ifdef FEAT_WAYLAND_CLIPBOARD

تعريفنوع بنية vwl_معلومات_offer_S vwl_معلومات_offer_T;
تعريفنوع بنية vwl_معلومات_ذريعو_S vwl_معلومات_ذريعو_T;
تعريفنوع بنية vwl_معلومات_device_S vwl_معلومات_device_T;
تعريفنوع بنية vwl_معلومات_device_manager_S vwl_معلومات_device_manager_T;

تعريفنوع بنية vwl_معلومات_device_listener_S vwl_معلومات_device_listener_T;
تعريفنوع بنية vwl_معلومات_ذريعو_listener_S vwl_معلومات_ذريعو_listener_T;
تعريفنوع بنية vwl_معلومات_offer_listener_S vwl_معلومات_offer_listener_T;

// Wayland selections
تعريفنوع تعداد {
    WAYLAND_SELECTION_NONE	= 0,
    WAYLAND_SELECTION_REGULAR	= 1 << 0,
    WAYLAND_SELECTION_PRIMARY	= 1 << 1,
} wayland_selection_T;

# endif
#جيڪڏهن_پورو

// Used in term_set_sync_output()
تعريفنوع تعداد
{
    رڪن_SYNC_OUTPUT_ENABLE = 1 << 0,
    رڪن_SYNC_OUTPUT_DISABLE = 1 << 1,
    رڪن_SYNC_OUTPUT_OFF = 1 << 2,
    رڪن_SYNC_OUTPUT_FLUSH = 1 << 3,
} رڪن_sync_output_T;
