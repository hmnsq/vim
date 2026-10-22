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
#ఒకవేళ defined(SASC) && SASC < 658
రకనిర్వచనం దీర్ఘ		linenr_T;
రకనిర్వచనం పూర్ణాంకం		colnr_T;
రకనిర్వచనం గుర్తులేని పొట్టి	short_u;
#అయితే_ముగింపు

// structure to store a string (including it's length)
రకనిర్వచనం నిర్మాణం
{
    char_u  *string;		// the string
    size_t  నిడివి;		// length of the string (excluding the NUL)
} string_T;

/*
 * Position in file or buffer.
 */
రకనిర్వచనం నిర్మాణం
{
    linenr_T	lnum;	// line number
    colnr_T	నిలువు;	// column number
    colnr_T	coladd; // extra virtual column
} pos_T;


/*
 * Same, but without coladd.
 */
రకనిర్వచనం నిర్మాణం
{
    linenr_T	lnum;	// line number
    colnr_T	నిలువు;	// column number
} lpos_T;

/*
 * Structure used for growing arrays.
 * This is used to store information that only grows, is deleted all at
 * once, and needs to be accessed by index.  See ga_clear() and ga_grow().
 */
రకనిర్వచనం నిర్మాణం growarray
{
    పూర్ణాంకం	    ga_len;		    // current number of items used
    పూర్ణాంకం	    ga_maxlen;		    // maximum number of items possible
    పూర్ణాంకం	    ga_itemsize;	    // sizeof(item)
    పూర్ణాంకం	    ga_growsize;	    // number of items to grow each time
    శూన్య    *ga_దత్తాంశం;		    // pointer to the first item
} garray_T;

#నిర్వచించు GA_ఖాళీలు    {0, 0, 0, 0, ఖాళీ}

// On rare systems "char" is unsigned, sometimes we really want a signed 8-bit
// value.
రకనిర్వచనం గుర్తుగల అక్షరం	int8_T;
రకనిర్వచనం ద్విగుణ		float_T;

రకనిర్వచనం నిర్మాణం typval_S		typval_T;
రకనిర్వచనం నిర్మాణం listvar_S	list_T;
రకనిర్వచనం నిర్మాణం dictvar_S	dict_T;
రకనిర్వచనం నిర్మాణం partial_S	partial_T;
రకనిర్వచనం నిర్మాణం blobvar_S	blob_T;
రకనిర్వచనం నిర్మాణం tuplevar_S	tuple_T;
రకనిర్వచనం నిర్మాణం generictype_S	generic_T;
రకనిర్వచనం నిర్మాణం gfargs_tab_S	gfargs_tab_T;

రకనిర్వచనం నిర్మాణం కిటికీ_S		win_T;
రకనిర్వచనం నిర్మాణం wininfo_S	wininfo_T;
రకనిర్వచనం నిర్మాణం చట్రం_S		చట్రం_T;
రకనిర్వచనం పూర్ణాంకం			scid_T;		// script ID
రకనిర్వచనం నిర్మాణం file_బఫరు	buf_T;		// forward declaration
రకనిర్వచనం నిర్మాణం terminal_S	పదం_T;

#నిర్వచితమైతే FEAT_MENU
రకనిర్వచనం నిర్మాణం VimMenu vimmenu_T;
#అయితే_ముగింపు

// maximum value for sc_version
#నిర్వచించు SCRIPT_సంస్కరణ_MAX 4
// value for sc_version in a Vim9 script file
#నిర్వచించు SCRIPT_సంస్కరణ_VIM9 999999

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
రకనిర్వచనం నిర్మాణం {
#నిర్వచితమైతే FEAT_EVAL
    scid_T	sc_sid;		// script ID
    పూర్ణాంకం		sc_seq;		// sourcing sequence number
    linenr_T	sc_lnum;	// line number
#అయితే_ముగింపు
    పూర్ణాంకం		sc_సంస్కరణ;	// :scriptversion
} sctx_T;

/*
 * Reference to a buffer that stores the value of buf_free_count.
 * bufref_valid() only needs to check "buf" when the count differs.
 */
రకనిర్వచనం నిర్మాణం {
    buf_T   *br_buf;
    పూర్ణాంకం	    br_fnum;
    పూర్ణాంకం	    br_buf_free_లెక్క;
} bufref_T;

/*
 * This is here because regexp.h needs pos_T and below regprog_T is used.
 */
#అన్తర్భావ "regexp.h"

/*
 * This is here because gui.h needs the pos_T and win_T, and win_T needs gui.h
 * for scrollbar_T.
 */
#నిర్వచితమైతే FEAT_GUI
# include "gui.h"
#లేకపోతే
# ifdef FEAT_XCLIPBOARD
#  include <X11/Intrinsic.h>
# endif
# define guicolor_T దీర్ఘ
# define INVALCOLOR ((guicolor_T)0x1ffffff)
    // only used for cterm.bg_rgb and cterm.fg_rgb: use cterm color
# define CTERMCOLOR ((guicolor_T)0x1fffffe)
#అయితే_ముగింపు
// We have include guard in the file
#అన్తర్భావ "image.h"
#నిర్వచించు COLOR_INVALID(x) ((x) == INVALCOLOR || (x) == CTERMCOLOR)

#నిర్వచితమైతే FEAT_TERMINAL
# include "libvterm/include/vterm.h"
రకనిర్వచనం నిర్మాణం {
    VTermColor	fg;
    VTermColor	bg;
} termcellcolor_T;
#అయితే_ముగింపు

/*
 * marks: positions in a file
 * (a normal mark is a lnum/col pair, the same as a file position)
 */

#నిర్వచించు NMARKS		('z' - 'a' + 1)	// max. # of named marks
#నిర్వచించు EXTRA_MARKS	10		// marks 0-9
#నిర్వచించు JUMPLISTSIZE	100		// max. # of marks in jump list
#నిర్వచించు TAGSTACKSIZE	20		// max. # of tags in tag stack

రకనిర్వచనం నిర్మాణం filemark
{
    pos_T	mark;		// cursor position
    పూర్ణాంకం		fnum;		// file number
} fmark_T;

// Xtended file mark: also has a file name
రకనిర్వచనం నిర్మాణం xfilemark
{
    fmark_T	fmark;
    char_u	*fname;		// file name, used when fnum == 0
#నిర్వచితమైతే FEAT_VIMINFO
    సమయం_T	సమయం_సమితి;
#అయితే_ముగింపు
} xfmark_T;

/*
 * The taggy struct is used to store the information about a :tag command.
 */
రకనిర్వచనం నిర్మాణం taggy
{
    char_u	*tagname;	// tag name
    fmark_T	fmark;		// cursor position BEFORE ":tag"
    పూర్ణాంకం		cur_సరిపోలిక;	// match number
    పూర్ణాంకం		cur_fnum;	// buffer number used for cur_match
    char_u	*user_దత్తాంశం;	// used with tagfunc
} taggy_T;

/*
 * Structure that contains all options that are local to a window.
 * Used twice in a window: for the current buffer and for all buffers.
 * Also used in wininfo_T.
 */
రకనిర్వచనం నిర్మాణం
{
#నిర్వచితమైతే FEAT_ARABIC
    పూర్ణాంకం		wo_arab;
# define w_p_arab w_onebuf_opt.wo_arab	// 'arabic'
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_LINEBREAK
    పూర్ణాంకం		wo_bri;
# define w_p_bri w_onebuf_opt.wo_bri	// 'breakindent'
    char_u	*wo_briopt;
# define w_p_briopt w_onebuf_opt.wo_briopt // 'breakindentopt'
#అయితే_ముగింపు
    char_u	*wo_wcr;
#నిర్వచించు w_p_wcr w_onebuf_opt.wo_wcr	// 'wincolor'
#నిర్వచితమైతే FEAT_DIFF
    పూర్ణాంకం		wo_diff;
# define w_p_diff w_onebuf_opt.wo_diff	// 'diff'
#అయితే_ముగింపు
    char_u	*wo_eiw;
#నిర్వచించు w_p_eiw w_onebuf_opt.wo_eiw	// 'eventignorewin'
#నిర్వచితమైతే FEAT_FOLDING
    దీర్ఘ	wo_fdc;
# define w_p_fdc w_onebuf_opt.wo_fdc	// 'foldcolumn'
    పూర్ణాంకం		wo_fdc_save;
# define w_p_fdc_save w_onebuf_opt.wo_fdc_save	// 'foldenable' saved for diff mode
    పూర్ణాంకం		wo_fen;
# define w_p_fen w_onebuf_opt.wo_fen	// 'foldenable'
    పూర్ణాంకం		wo_fen_save;
# define w_p_fen_save w_onebuf_opt.wo_fen_save	// 'foldenable' saved for diff mode
    char_u	*wo_fdi;
# define w_p_fdi w_onebuf_opt.wo_fdi	// 'foldignore'
    దీర్ఘ	wo_fdl;
# define w_p_fdl w_onebuf_opt.wo_fdl	// 'foldlevel'
    పూర్ణాంకం		wo_fdl_save;
# define w_p_fdl_save w_onebuf_opt.wo_fdl_save	// 'foldlevel' state saved for diff mode
    char_u	*wo_fdm;
# define w_p_fdm w_onebuf_opt.wo_fdm	// 'foldmethod'
    char_u	*wo_fdm_save;
# define w_p_fdm_save w_onebuf_opt.wo_fdm_save	// 'fdm' saved for diff mode
    దీర్ఘ	wo_fml;
# define w_p_fml w_onebuf_opt.wo_fml	// 'foldminlines'
    దీర్ఘ	wo_fdn;
# define w_p_fdn w_onebuf_opt.wo_fdn	// 'foldnestmax'
# ifdef FEAT_EVAL
    char_u	*wo_fde;
#  define w_p_fde w_onebuf_opt.wo_fde	// 'foldexpr'
    char_u	*wo_fdt;
#  define w_p_fdt w_onebuf_opt.wo_fdt	// 'foldtext'
# endif
    char_u	*wo_fmr;
# define w_p_fmr w_onebuf_opt.wo_fmr	// 'foldmarker'
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_LINEBREAK
    పూర్ణాంకం		wo_lbr;
# define w_p_lbr w_onebuf_opt.wo_lbr	// 'linebreak'
#అయితే_ముగింపు
    పూర్ణాంకం		wo_list;
#నిర్వచించు w_p_list w_onebuf_opt.wo_list	// 'list'
    char_u	*wo_lcs;
#నిర్వచించు w_p_lcs w_onebuf_opt.wo_lcs	// 'listchars'
    char_u	*wo_fcs;
#నిర్వచించు w_p_fcs w_onebuf_opt.wo_fcs	// 'fillchars'
    పూర్ణాంకం		wo_nu;
#నిర్వచించు w_p_nu w_onebuf_opt.wo_nu	// 'number'
    పూర్ణాంకం		wo_rnu;
#నిర్వచించు w_p_rnu w_onebuf_opt.wo_rnu	// 'relativenumber'
    char_u	*wo_ve;
#నిర్వచించు w_p_ve w_onebuf_opt.wo_ve	// 'virtualedit'
    గుర్తులేని	wo_ve_flags;
#నిర్వచించు	w_ve_flags w_onebuf_opt.wo_ve_flags	// flags for 'virtualedit'
#నిర్వచితమైతే FEAT_LINEBREAK
    దీర్ఘ	wo_nuw;
# define w_p_nuw w_onebuf_opt.wo_nuw	// 'numberwidth'
#అయితే_ముగింపు
    పూర్ణాంకం wo_wfb;
#నిర్వచించు w_p_wfb w_onebuf_opt.wo_wfb	// 'winfixbuf'
    పూర్ణాంకం		wo_wfh;
#నిర్వచించు w_p_wfh w_onebuf_opt.wo_wfh	// 'winfixheight'
    పూర్ణాంకం		wo_wfw;
#నిర్వచించు w_p_wfw w_onebuf_opt.wo_wfw	// 'winfixwidth'
#ఒకవేళ defined(FEAT_QUICKFIX)
    పూర్ణాంకం		wo_pvw;
# define w_p_pvw w_onebuf_opt.wo_pvw	// 'previewwindow'
    దీర్ఘ        wo_lhi;
# define w_p_lhi w_onebuf_opt.wo_lhi    // 'lhistory'
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_RIGHTLEFT
    పూర్ణాంకం		wo_rl;
# define w_p_rl w_onebuf_opt.wo_rl	// 'rightleft'
    char_u	*wo_rlc;
# define w_p_rlc w_onebuf_opt.wo_rlc	// 'rightleftcmd'
#అయితే_ముగింపు
    దీర్ఘ	wo_scr;
#నిర్వచించు w_p_scr w_onebuf_opt.wo_scr	// 'scroll'
    పూర్ణాంకం		wo_sms;
#నిర్వచించు w_p_sms w_onebuf_opt.wo_sms	// 'smoothscroll'
#నిర్వచితమైతే FEAT_SPELL
    పూర్ణాంకం		wo_spell;
# define w_p_spell w_onebuf_opt.wo_spell // 'spell'
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_SYN_HL) || defined(FEAT_FOLDING) || defined(FEAT_DIFF)
    పూర్ణాంకం		wo_cuc;
# define w_p_cuc w_onebuf_opt.wo_cuc	// 'cursorcolumn'
    పూర్ణాంకం		wo_cul;
# define w_p_cul w_onebuf_opt.wo_cul	// 'cursorline'
    char_u	*wo_culopt;
# define w_p_culopt w_onebuf_opt.wo_culopt	// 'cursorlineopt'
    char_u	*wo_cc;
# define w_p_cc w_onebuf_opt.wo_cc	// 'colorcolumn'
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_LINEBREAK
    char_u	*wo_sbr;
# define w_p_sbr w_onebuf_opt.wo_sbr	// 'showbreak'
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_STL_OPT
    char_u	*wo_stl;
# define w_p_stl w_onebuf_opt.wo_stl	// 'statusline'
    char_u	*wo_stlo;
# define w_p_stlo w_onebuf_opt.wo_stlo	// 'statuslineopt'
    పూర్ణాంకం		wo_stlo_fh;
# define w_p_stlo_fh w_onebuf_opt.wo_stlo_fh // "fixedheight" of 'statuslineopt'
    పూర్ణాంకం		wo_stlo_mh;
# define w_p_stlo_mh w_onebuf_opt.wo_stlo_mh // "maxheight:" of 'statuslineopt'
#అయితే_ముగింపు
    పూర్ణాంకం		wo_scb;
#నిర్వచించు w_p_scb w_onebuf_opt.wo_scb	// 'scrollbind'
    పూర్ణాంకం		wo_diff_saved; // options were saved for starting diff mode
#నిర్వచించు w_p_diff_saved w_onebuf_opt.wo_diff_saved
    పూర్ణాంకం		wo_scb_save;	// 'scrollbind' saved for diff mode
#నిర్వచించు w_p_scb_save w_onebuf_opt.wo_scb_save
    పూర్ణాంకం		wo_wrap;
#నిర్వచించు w_p_wrap w_onebuf_opt.wo_wrap	// 'wrap'
#నిర్వచితమైతే FEAT_DIFF
    పూర్ణాంకం		wo_wrap_save;	// 'wrap' state saved for diff mode
# define w_p_wrap_save w_onebuf_opt.wo_wrap_save
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_CONCEAL
    char_u	*wo_cocu;		// 'concealcursor'
# define w_p_cocu w_onebuf_opt.wo_cocu
    దీర్ఘ	wo_cole;		// 'conceallevel'
# define w_p_cole w_onebuf_opt.wo_cole
#అయితే_ముగింపు
    పూర్ణాంకం		wo_crb;
#నిర్వచించు w_p_crb w_onebuf_opt.wo_crb	// 'cursorbind'
    పూర్ణాంకం		wo_crb_save;	// 'cursorbind' state saved for diff mode
#నిర్వచించు w_p_crb_save w_onebuf_opt.wo_crb_save
#నిర్వచితమైతే FEAT_SIGNS
    char_u	*wo_scl;
# define w_p_scl w_onebuf_opt.wo_scl	// 'signcolumn'
#అయితే_ముగింపు
    దీర్ఘ	wo_siso;
#నిర్వచించు w_p_siso w_onebuf_opt.wo_siso	// 'sidescrolloff' local value
    దీర్ఘ	wo_so;
#నిర్వచించు w_p_so w_onebuf_opt.wo_so	// 'scrolloff' local value
    దీర్ఘ	wo_sop;
#నిర్వచించు w_p_sop w_onebuf_opt.wo_sop  // 'scrolloffpad' local value
#నిర్వచితమైతే FEAT_TERMINAL
    char_u	*wo_twk;
# define w_p_twk w_onebuf_opt.wo_twk	// 'termwinkey'
    char_u	*wo_tws;
# define w_p_tws w_onebuf_opt.wo_tws	// 'termwinsize'
#అయితే_ముగింపు

    // A few options have local flags for P_INSECURE.
    long_u	wo_wrap_flags;		// flags for 'wrap'
#నిర్వచించు w_p_wrap_flags w_onebuf_opt.wo_wrap_flags
#నిర్వచితమైతే FEAT_STL_OPT
    long_u	wo_stl_flags;		// flags for 'statusline'
# define w_p_stl_flags w_onebuf_opt.wo_stl_flags
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_EVAL
    long_u	wo_fde_flags;		// flags for 'foldexpr'
# define w_p_fde_flags w_onebuf_opt.wo_fde_flags
    long_u	wo_fdt_flags;		// flags for 'foldtext'
# define w_p_fdt_flags w_onebuf_opt.wo_fdt_flags
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_EVAL
    sctx_T	wo_script_ctx[WV_లెక్క];	// SCTXs for window-local options
# define w_p_script_ctx w_onebuf_opt.wo_script_ctx
#అయితే_ముగింపు
    char_u	*wo_whl;
#నిర్వచించు w_p_whl w_onebuf_opt.wo_whl	// 'winhighlight'
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
నిర్మాణం wininfo_S
{
    wininfo_T	*wi_తదుపరి;	// next entry or NULL for last entry
    wininfo_T	*wi_prev;	// previous entry or NULL for first entry
    win_T	*wi_win;	// pointer to window that did set wi_fpos
    pos_T	wi_fpos;	// last cursor position in the file
    winopt_T	wi_opt;		// local window options
    పూర్ణాంకం		wi_optset;	// TRUE when wi_opt has useful values
#నిర్వచితమైతే FEAT_FOLDING
    పూర్ణాంకం		wi_fold_manual;	// copy of w_fold_manual
    garray_T	wi_folds;	// clone of w_folds
#అయితే_ముగింపు
    పూర్ణాంకం		wi_changelistidx; // copy of w_changelistidx
};

/*
 * Info used to pass info about a fold from the fold-detection code to the
 * code that displays the foldcolumn.
 */
రకనిర్వచనం నిర్మాణం foldinfo
{
    పూర్ణాంకం		fi_level;	// level of the fold; when this is zero the
				// other fields are invalid
    పూర్ణాంకం		fi_lnum;	// line number where fold starts
    పూర్ణాంకం		fi_దిగువ_level;	// lowest fold level that starts in the same
				//పంక్తి
} foldinfo_T;

/*
 * Structure to store info about the Visual area.
 */
రకనిర్వచనం నిర్మాణం
{
    pos_T	vi_ప్రారంభం;	// start pos of last VIsual
    pos_T	vi_చివర;		// end position of last VIsual
    పూర్ణాంకం		vi_బహుళకం;	// VIsual_mode of last VIsual
    colnr_T	vi_curswant;	// MAXCOL from w_curswant
} visualinfo_T;

/*
 * structures used for undo
 */

// One line saved for undo.  After the NUL terminated text there might be text
// properties, thus ul_len can be larger than STRLEN(ul_line) + 1.
రకనిర్వచనం నిర్మాణం {
    char_u	*ul_పంక్తి;	// text of the line
    దీర్ఘ	ul_len;		// length of the line including NUL, plus text
				// properties
    colnr_T	ul_textlen;	// length of the line excluding NUL and any text
				// properties
} undoline_T;

రకనిర్వచనం నిర్మాణం u_నమోదు u_నమోదు_T;
రకనిర్వచనం నిర్మాణం u_శీర్షిక u_శీర్షిక_T;

#నిర్వచితమైతే FEAT_PROP_POPUP
// Which line around an undo block a cleared continuation flag was on.
# define UNDOPROP_NONE	0
# define UNDOPROP_ABOVE	1
# define UNDOPROP_క్రింద	2

// A continuation flag cleared on a line around an undo block, given back when
// the block is undone or redone the other way.
రకనిర్వచనం నిర్మాణం
{
    పూర్ణాంకం		పైకి_which;	// UNDOPROP_ABOVE or UNDOPROP_BELOW
    పూర్ణాంకం		పైకి_type;	// tp_type of the property
    పూర్ణాంకం		పైకి_id;		// tp_id of the property
    colnr_T	పైకి_నిలువు;		// tp_col of the property
    పూర్ణాంకం		పైకి_flag;	// TP_FLAG_CONT_NEXT or TP_FLAG_CONT_PREV
} undoprop_T;
#అయితే_ముగింపు

నిర్మాణం u_నమోదు
{
    u_నమోదు_T	*ue_తదుపరి;	// pointer to next entry in list
    linenr_T	ue_శిఖరం;		// number of line above undo block
    linenr_T	ue_bot;		// number of line below undo block
    linenr_T	ue_lcount;	// linecount when u_save called
    undoline_T	*ue_array;	// array of lines in undo block
    దీర్ఘ	ue_కొలత;	// number of lines in ue_array
#నిర్వచితమైతే FEAT_PROP_POPUP
    garray_T	ue_props;	// undoprop_T: flags cleared around the block
#అయితే_ముగింపు
#నిర్వచితమైతే U_DEBUG
    పూర్ణాంకం		ue_magic;	// magic number to check allocation
#అయితే_ముగింపు
};

నిర్మాణం u_శీర్షిక
{
    // The following have a pointer and a number. The number is used when
    // reading the undo file in u_read_undo()
    సంఘం {
	u_శీర్షిక_T *ptr;	// pointer to next undo header in list
	దీర్ఘ	   seq;
    } uh_తదుపరి;
    సంఘం {
	u_శీర్షిక_T *ptr;	// pointer to previous header in list
	దీర్ఘ	   seq;
    } uh_prev;
    సంఘం {
	u_శీర్షిక_T *ptr;	// pointer to next header for alt. redo
	దీర్ఘ	   seq;
    } uh_alt_తదుపరి;
    సంఘం {
	u_శీర్షిక_T *ptr;	// pointer to previous header for alt. redo
	దీర్ఘ	   seq;
    } uh_alt_prev;
    దీర్ఘ	uh_seq;		// sequence number, higher == newer undo
    పూర్ణాంకం		uh_walk;	// used by undo_time()
    u_నమోదు_T	*uh_నమోదు;	// pointer to first entry
    u_నమోదు_T	*uh_getbot_నమోదు; // pointer to where ue_bot must be set
    pos_T	uh_cursor;	// cursor position before saving
    దీర్ఘ	uh_cursor_vcol;
    పూర్ణాంకం		uh_flags;	// see below
    pos_T	uh_namedm[NMARKS];	// marks before undo/after redo
    visualinfo_T uh_visual;	// Visual areas before undo/after redo
    సమయం_T	uh_సమయం;	// timestamp when the change was made
    దీర్ఘ	uh_save_nr;	// set when the file was saved after the
				// changes in this block
#నిర్వచితమైతే U_DEBUG
    పూర్ణాంకం		uh_magic;	// magic number to check allocation
#అయితే_ముగింపు
};

// values for uh_flags
#నిర్వచించు UH_మారింది  0x01	// b_changed flag before undo/after redo
#నిర్వచించు UH_EMPTYBUF 0x02	// buffer was empty

/*
 * structures used in undo.c
 */
#నిర్వచించు ALIGN_LONG	// longword alignment and use filler byte
#నిర్వచించు ALIGN_కొలత (పరిమాణం(దీర్ఘ))

#నిర్వచించు ALIGN_ముసుగు (ALIGN_కొలత - 1)

రకనిర్వచనం నిర్మాణం m_info minfo_T;

/*
 * structure used to link chunks in one of the free chunk lists.
 */
నిర్మాణం m_info
{
#నిర్వచితమైతే ALIGN_LONG
    long_u	m_కొలత_2;		// size of the chunk (including m_info)
#లేకపోతే
    short_u	m_కొలత_2;		// size of the chunk (including m_info)
#అయితే_ముగింపు
    minfo_T	*m_తదుపరి;	// pointer to next free chunk in the list
};

/*
 * things used in memfile.c
 */

రకనిర్వచనం నిర్మాణం ఖండం_hdr    bhdr_T;
రకనిర్వచనం నిర్మాణం memfile	    memfile_T;
రకనిర్వచనం దీర్ఘ		    blocknr_T;

/*
 * mf_hashtab_T is a chained hashtable with blocknr_T key and arbitrary
 * structures as items.  This is an intrusive data structure: we require
 * that items begin with mf_hashitem_T which contains the key and linked
 * list pointers.  List of items in each bucket is doubly-linked.
 */

రకనిర్వచనం నిర్మాణం mf_hashitem_S mf_hashitem_T;

నిర్మాణం mf_hashitem_S
{
    mf_hashitem_T   *mhi_తదుపరి;
    mf_hashitem_T   *mhi_prev;
    blocknr_T	    mhi_కీలకం;
};

#నిర్వచించు MHT_INIT_కొలత   64

రకనిర్వచనం నిర్మాణం mf_hashtab_S
{
    long_u	    mht_ముసుగు;	    // mask used for hash value (nr of items
				    // in array is "mht_mask" + 1)
    long_u	    mht_లెక్క;	    // nr of items inserted into hashtable
    mf_hashitem_T   **mht_buckets;  // points to mht_small_buckets or
				    //dynamically allocated array
    mf_hashitem_T   *mht_small_buckets[MHT_INIT_కొలత];   // initial buckets
    అక్షరం	    mht_fixed;	    // non-zero value forbids growth
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

నిర్మాణం ఖండం_hdr
{
    mf_hashitem_T bh_hashitem;      // header for hash table and key
#నిర్వచించు bh_bnum bh_hashitem.mhi_కీలకం // block number, part of bh_hashitem

    bhdr_T	*bh_తదుపరి;	    // next block_hdr in free or used list
    bhdr_T	*bh_prev;	    // previous block_hdr in used list
    char_u	*bh_దత్తాంశం;	    // pointer to memory (for used block)
    పూర్ణాంకం		bh_పుట_లెక్క;	    // number of pages in this block

#నిర్వచించు BH_DIRTY    1
#నిర్వచించు BH_LOCKED   2
    అక్షరం	bh_flags;	    // BH_DIRTY or BH_LOCKED
};

/*
 * when a block with a negative number is flushed to the file, it gets
 * a positive number. Because the reference to the block is still the negative
 * number, we remember the translation to the new positive number in the
 * double linked trans lists. The structure is the same as the hash lists.
 */
రకనిర్వచనం నిర్మాణం nr_trans NR_TRANS;

నిర్మాణం nr_trans
{
    mf_hashitem_T nt_hashitem;		// header for hash table and key
#నిర్వచించు nt_old_bnum nt_hashitem.mhi_కీలకం	// old, negative, number

    blocknr_T	nt_new_bnum;		// new, positive, number
};


రకనిర్వచనం నిర్మాణం buffblock buffblock_T;
రకనిర్వచనం నిర్మాణం buffheader buffheader_T;

/*
 * structure used to store one block of the stuff/redo/recording buffers
 */
నిర్మాణం buffblock
{
    buffblock_T	*b_తదుపరి;	// pointer to next buffblock
    size_t	b_strlen;	// length of b_str, excluding the NUL
    char_u	b_str[1];	// contents (actually longer)
};

/*
 * header used for the stuff buffer and the redo buffer
 */
నిర్మాణం buffheader
{
    buffblock_T	bh_మొదటి;	// first (dummy) block of list
    buffblock_T	*bh_curr;	// buffblock for appending
    పూర్ణాంకం		bh_సూచీ;	// index for reading
    పూర్ణాంకం		bh_space;	// space in bh_curr for appending
    పూర్ణాంకం		bh_create_newblock;	// create a new block?
};

రకనిర్వచనం నిర్మాణం
{
    buffheader_T sr_redobuff;
    buffheader_T sr_old_redobuff;
} save_redo_T;

రకనిర్వచనం గణన {
    XP_ఉపసర్గ_NONE,	// prefix not used
    XP_ఉపసర్గ_NO,	// "no" prefix for bool option
    XP_ఉపసర్గ_INV,	// "inv" prefix for bool option
} xp_ఉపసర్గ_T;

/*
 * :set operator types
 */
రకనిర్వచనం గణన {
    OP_NONE = 0,
    OP_ADDING,		// "opt+=arg"
    OP_PREPENDING,	// "opt^=arg"
    OP_REMOVING,	// "opt-=arg"
} సమితి_op_T;

/*
 * used for completion on the command line
 */
రకనిర్వచనం నిర్మాణం expand
{
    char_u	*xp_నమూనా;		// start of item to expand, guaranteed
					// to be part of xp_line
    పూర్ణాంకం		xp_context;		// type of expansion
    పూర్ణాంకం		xp_నమూనా_len;		// bytes in xp_pattern before cursor
    xp_ఉపసర్గ_T	xp_ఉపసర్గ;
#ఒకవేళ defined(FEAT_EVAL)
    char_u	*xp_arg;		// completion function
    పూర్ణాంకం		xp_complete_opt;	// UCC_ flags for user command
    sctx_T	xp_script_ctx;		// SCTX for completion function
#అయితే_ముగింపు
    పూర్ణాంకం		xp_backslash;		// one of the XP_BS_ values
#నిర్వచితం_కాకపోతే BACKSLASH_లోపల_FILENAME
    పూర్ణాంకం		xp_shell;		// TRUE for a shell command, more
					// characters need to be escaped
#అయితే_ముగింపు
    పూర్ణాంకం		xp_numfiles;		// number of files found by
					// file name completion
    పూర్ణాంకం		xp_నిలువు;			// cursor position in line
    పూర్ణాంకం		xp_selected;		// selected index in completion
    char_u	*xp_orig;		// originally expanded string
    char_u	**xp_files;		// list of files
    char_u	**xp_files_abbr;	// optional parallel array of display
					// strings (override xp_files for the
					// pum text); NULL if unused
    char_u	**xp_files_రకం;	// optional parallel array of "kind"
					// strings; NULL if unused
    char_u	**xp_files_menu;	// optional parallel array of "menu"
					// strings (shown after the match);
					// NULL if unused
    char_u	**xp_files_info;	// optional parallel array of "info"
					// strings (shown in info popup);
					// NULL if unused
    char_u	*xp_పంక్తి;		// text being completed
#నిర్వచించు EXPAND_BUF_LEN 256
    char_u	xp_buf[EXPAND_BUF_LEN];	// buffer for returned match
    పూర్ణాంకం		xp_శోధన_dir;		// Direction of search
    pos_T	xp_pre_incsearch_pos;	// Cursor position before incsearch
} expand_T;

/*
 * values for xp_backslash
 */
#నిర్వచించు XP_BS_NONE	0	// nothing special for backslashes
#నిర్వచించు XP_BS_ONE	0x1	// uses one backslash before a space
#నిర్వచించు XP_BS_THREE	0x2	// uses three backslashes before a space
#నిర్వచించు XP_BS_COMMA	0x4	// commas need to be escaped with a backslash

/*
 * Variables shared between getcmdline(), redrawcmdline() and others.
 * These need to be saved when using CTRL-R |, that's why they are in a
 * structure.
 */
రకనిర్వచనం నిర్మాణం
{
    char_u	*cmdbuff;	// pointer to command line buffer
    పూర్ణాంకం		cmdbufflen;	// length of cmdbuff
    పూర్ణాంకం		cmdlen;		// number of chars in command line
    పూర్ణాంకం		cmdpos;		// current cursor position
    పూర్ణాంకం		cmdspos;	// cursor column on screen
    పూర్ణాంకం		cmdfirstc;	// ':', '/', '?', '=', '>' or NUL
    పూర్ణాంకం		cmdindent;	// number of spaces before cmdline
    char_u	*cmdprompt;	// message in front of cmdline
    పూర్ణాంకం		cmdattr;	// attributes for prompt
    పూర్ణాంకం		overstrike;	// Typing mode on the command line.  Shared by
				// getcmdline() and put_on_cmdline().
    expand_T	*xpc;		// struct being used for expansion, xp_pattern
				// may point into cmdbuff
    పూర్ణాంకం		xp_context;	// type of expansion
#నిర్వచితమైతే FEAT_EVAL
    char_u	*xp_arg;	// user-defined expansion arg
    పూర్ణాంకం		ఆదానం_fn;	// when TRUE Invoked for input() function
#అయితే_ముగింపు
    పూర్ణాంకం		cmdbuff_replaced; // when TRUE cmdline was replaced externally
				  // (e.g. by setcmdline())
} cmdline_info_T;

/*
 * Command modifiers ":vertical", ":browse", ":confirm" and ":hide" set a flag.
 * This needs to be saved for recursive commands, put them in a structure for
 * easy manipulation.
 */
రకనిర్వచనం నిర్మాణం
{
    పూర్ణాంకం		cmod_flags;		// CMOD_ flags
#నిర్వచించు CMOD_SANDBOX	    0x0001	// ":sandbox"
#నిర్వచించు CMOD_SILENT	    0x0002	// ":silent"
#నిర్వచించు CMOD_ERRSILENT	    0x0004	// ":silent!"
#నిర్వచించు CMOD_UNSILENT	    0x0008	// ":unsilent"
#నిర్వచించు CMOD_NOAUTOCMD	    0x0010	// ":noautocmd"
#నిర్వచించు CMOD_HIDE	    0x0020	// ":hide"
#నిర్వచించు CMOD_BROWSE	    0x0040	// ":browse" - invoke file dialog
#నిర్వచించు CMOD_CONFIRM	    0x0080	// ":confirm" - invoke yes/no dialog
#నిర్వచించు CMOD_KEEPALT	    0x0100	// ":keepalt"
#నిర్వచించు CMOD_KEEPMARKS	    0x0200	// ":keepmarks"
#నిర్వచించు CMOD_KEEPJUMPS	    0x0400	// ":keepjumps"
#నిర్వచించు CMOD_LOCKMARKS	    0x0800	// ":lockmarks"
#నిర్వచించు CMOD_KEEPPATTERNS   0x1000	// ":keeppatterns"
#నిర్వచించు CMOD_NOSWAPFILE	    0x2000	// ":noswapfile"
#నిర్వచించు CMOD_VIM9CMD	    0x4000	// ":vim9cmd"
#నిర్వచించు CMOD_LEGACY	    0x8000	// ":legacy"

    పూర్ణాంకం		cmod_విభజించు;		// flags for win_split()
    పూర్ణాంకం		cmod_tab;		// > 0 when ":tab" was used
    regmatch_T	cmod_filter_regmatch;	// set by :filter /pat/
    పూర్ణాంకం		cmod_filter_force;	// set for :filter!

    పూర్ణాంకం		cmod_verbose;		// 0 if not set, > 0 to set 'verbose'
					// to cmod_verbose - 1

    // values for undo_cmdmod()
    char_u	*cmod_save_ei;		// saved value of 'eventignore'
#నిర్వచితమైతే HAVE_SANDBOX
    పూర్ణాంకం		cmod_did_sandbox;	// set when "sandbox" was incremented
#అయితే_ముగింపు
    దీర్ఘ	cmod_verbose_save;	// if 'verbose' was set: value of
					// p_verbose plus one
    పూర్ణాంకం		cmod_save_సందేశం_silent;	// if non-zero: saved value of
					// msg_silent + 1
    పూర్ణాంకం		cmod_save_సందేశం_scroll;	// for restoring msg_scroll
    పూర్ణాంకం		cmod_did_esilent;	// incremented when emsg_silent is
} cmdmod_T;

రకనిర్వచనం గణన {
    MF_DIRTY_NO = 0,		// no dirty blocks
    MF_DIRTY_YES,		// there are dirty blocks
    MF_DIRTY_YES_NOSYNC,	// there are dirty blocks, do not sync yet
} mfdirty_T;

#నిర్వచించు MF_విత్తనం_LEN	8

నిర్మాణం memfile
{
    char_u	*mf_fname;		// name of the file
    char_u	*mf_ffname;		// idem, full path
    పూర్ణాంకం		mf_fd;			// file descriptor
    పూర్ణాంకం		mf_flags;		// flags used when opening this memfile
    పూర్ణాంకం		mf_reopen;		// mf_fd was closed, retry opening
    bhdr_T	*mf_free_మొదటి;		// first block_hdr in free list
    bhdr_T	*mf_వాడిన_మొదటి;		// mru block_hdr in used list
    bhdr_T	*mf_వాడిన_చివరి;		// lru block_hdr in used list
    గుర్తులేని	mf_వాడిన_లెక్క;		// number of pages in used list
    గుర్తులేని	mf_వాడిన_లెక్క_max;	// maximum number of pages in memory
    mf_hashtab_T mf_hash;		// hash lists
    mf_hashtab_T mf_trans;		// trans lists
    blocknr_T	mf_blocknr_max;		// highest positive block number + 1
    blocknr_T	mf_blocknr_min;		// lowest negative block number - 1
    blocknr_T	mf_neg_లెక్క;		// number of negative blocks numbers
    blocknr_T	mf_infile_లెక్క;	// number of pages in the file
    గుర్తులేని	mf_పుట_కొలత;		// number of bytes in a page
    mfdirty_T	mf_dirty;
#నిర్వచితమైతే FEAT_CRYPT
    buf_T	*mf_బఫరు;		// buffer this memfile is for
    char_u	mf_విత్తనం[MF_విత్తనం_LEN];	// seed for encryption

    // Values for key, method and seed used for reading data blocks when
    // updating for a newly set key or method. Only when mf_old_key != NULL.
    char_u	*mf_old_కీలకం;
    పూర్ణాంకం		mf_old_cm;
    char_u	mf_old_విత్తనం[MF_విత్తనం_LEN];
#అయితే_ముగింపు
};

/*
 * things used in memline.c
 */
/*
 * When searching for a specific line, we remember what blocks in the tree
 * are the branches leading to that block. This is stored in ml_stack.  Each
 * entry is a pointer to info in a block (may be data block or pointer block)
 */
రకనిర్వచనం నిర్మాణం info_సూచిక
{
    blocknr_T	ip_bnum;	// block number
    linenr_T	ip_దిగువ;		// lowest lnum in this block
    linenr_T	ip_ఎగువ;	// highest lnum in this block
    పూర్ణాంకం		ip_సూచీ;	// index for block with current lnum
} infoptr_T;	// block/index pair

#నిర్వచితమైతే FEAT_BYTEOFF
రకనిర్వచనం నిర్మాణం ml_chunksize
{
    పూర్ణాంకం		mlcs_numlines;
    దీర్ఘ	mlcs_totalsize;
} chunksize_T;

/*
 * Flags when calling ml_updatechunk()
 */
# define ML_CHNK_ADDLINE 1
# define ML_CHNK_DELLINE 2
# define ML_CHNK_UPDLINE 3
#అయితే_ముగింపు

/*
 * the memline structure holds all the information about a memline
 */
రకనిర్వచనం నిర్మాణం memline
{
    linenr_T	ml_పంక్తి_లెక్క;	// number of lines in the buffer

    memfile_T	*ml_mfp;	// pointer to associated memfile

    infoptr_T	*ml_దొంతర;	// stack of pointer blocks (array of IPTRs)
    పూర్ణాంకం		ml_దొంతర_శిఖరం;	// current top of ml_stack
    పూర్ణాంకం		ml_దొంతర_కొలత;	// total number of entries in ml_stack

#నిర్వచించు ML_ఖాళీలు	0x01	// empty buffer
#నిర్వచించు ML_పంక్తి_DIRTY	0x02	// cached line was changed and allocated
#నిర్వచించు ML_LOCKED_DIRTY	0x04	// ml_locked was changed
#నిర్వచించు ML_LOCKED_POS	0x08	// ml_locked needs positive block number
#నిర్వచించు ML_ALLOCATED	0x10	// ml_line_ptr is an allocated copy
    పూర్ణాంకం		ml_flags;

    colnr_T	ml_పంక్తి_len;	// length of the cached line + NUL + text properties
    colnr_T	ml_పంక్తి_textlen;// length of the cached line + NUL, 0 if not known yet
    linenr_T	ml_పంక్తి_lnum;	// line number of cached line, 0 if not valid
    char_u	*ml_పంక్తి_ptr;	// pointer to cached line

    bhdr_T	*ml_locked;	// block used by last ml_get
    linenr_T	ml_locked_దిగువ;	// first line in ml_locked
    linenr_T	ml_locked_ఎగువ;	// last line in ml_locked
    పూర్ణాంకం		ml_locked_lineadd;  // number of lines inserted in ml_locked
#నిర్వచితమైతే FEAT_BYTEOFF
    chunksize_T *ml_chunksize;
    పూర్ణాంకం		ml_numchunks;
    పూర్ణాంకం		ml_usedchunks;
#అయితే_ముగింపు
} memline_T;

// Values for the flags argument of ml_delete_flags().
#నిర్వచించు ML_DEL_సందేశం	    1	// may give a "No lines in buffer" message
#నిర్వచించు ML_DEL_UNDO	    2	// called from undo, do not update textprops
#నిర్వచించు ML_DEL_NOPROP	    4	// splitting data block, do not update textprops

// Values for the flags argument of ml_append_int().
#నిర్వచించు ML_APPEND_NEW	    1	// starting to edit a new file
#నిర్వచించు ML_APPEND_MARK	    2	// mark the new line
#నిర్వచించు ML_APPEND_UNDO	    4	// called from undo
#నిర్వచించు ML_APPEND_NOPROP    8	// do not continue textprop from previous line


/*
 * Structure defining text properties.  These stick with the text.
 * When stored in memline they are after the text, ml_line_len is larger than
 * STRLEN(ml_line_ptr) + 1.
 */
రకనిర్వచనం నిర్మాణం textprop_S
{
    colnr_T	tp_నిలువు;		// start column (one based, in bytes)
    colnr_T	tp_len;		// length in bytes; for virtual text props
				// this is STRLEN(vtext) (not including NUL)
    పూర్ణాంకం		tp_id;		// identifier
    పూర్ణాంకం		tp_type;	// property type
    పూర్ణాంకం		tp_flags;	// TP_FLAG_ values
    పూర్ణాంకం		tp_padleft;	// left padding between text line and virtual
				//పాఠ్యం
    సంఘం			// For virtual text props (tp_id < 0):
    {				// check TP_FLAG_VTEXT_PTR in tp_flags to
				// determine which member is active.
	colnr_T	tp_పాఠ్యం_స్థానభ్రంశం; // offset to vtext string from the
				// prop_count position in the memline
				// (when TP_FLAG_VTEXT_PTR is NOT set)
	char_u	*tp_పాఠ్యం;	// pointer to virtual text string
				// (when TP_FLAG_VTEXT_PTR IS set)
    } u;
} textprop_T;

#నిర్వచించు TP_FLAG_CONT_తదుపరి	0x1	// property continues in next line
#నిర్వచించు TP_FLAG_CONT_PREV	0x2	// property was continued from prev line

// without these text is placed after the end of the line
#నిర్వచించు TP_FLAG_ALIGN_కుడి	0x010	// virtual text is right-aligned
#నిర్వచించు TP_FLAG_ALIGN_ABOVE	0x020	// virtual text above the line
#నిర్వచించు TP_FLAG_ALIGN_క్రింద	0x040	// virtual text on next screen line

#నిర్వచించు TP_FLAG_WRAP		0x080	// virtual text wraps - when missing
					// text is truncated
#నిర్వచించు TP_FLAG_ప్రారంభం_INCL	0x100	// "start_incl" copied from proptype
#నిర్వచించు TP_FLAG_DELETED		0x200	// marked for deletion in
					// unpacked_memline_T
#నిర్వచించు TP_FLAG_VTEXT_PTR	0x400	// u.tp_text access is valid

#నిర్వచించు PROP_లెక్క_కొలత	పరిమాణం(uint16_t)    // size of prop_count in memline

#నిర్వచించు PROP_పాఠ్యం_MIN_CELLS	4	// minimum number of cells to use for
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
రకనిర్వచనం నిర్మాణం unpacked_memline_S
{
    buf_T	*buf;		// the line's buffer (NULL = closed)
    linenr_T	lnum;		// line number (0 = no line loaded)
    bool	detached;	// true when text/vtext are allocated
    colnr_T	పాఠ్యం_కొలత_2;	// size of text including NUL
    char_u	*పాఠ్యం;		// NUL-terminated text
    పూర్ణాంకం		prop_కొలత;	// number of allocated prop slots
    పూర్ణాంకం		prop_లెక్క;	// number of properties
    textprop_T	*props;		// property array
    bool	పాఠ్యం_మారింది;	// true if text was modified
} unpacked_memline_T;

/*
 * Structure defining a property type.
 */
రకనిర్వచనం నిర్మాణం proptype_S
{
    పూర్ణాంకం		pt_id;		// value used for tp_id
    పూర్ణాంకం		pt_type;	// number used for tp_type
    పూర్ణాంకం		pt_hl_id;	// highlighting
    పూర్ణాంకం		pt_ప్రాధాన్యత;	//ప్రాధాన్యత
    పూర్ణాంకం		pt_flags;	// PT_FLAG_ values
    char_u	pt_name[1];	// property type name, actually longer
} proptype_T;

#నిర్వచించు PT_FLAG_INS_ప్రారంభం_INCL	1	// insert at start included in property
#నిర్వచించు PT_FLAG_INS_చివర_INCL	2	// insert at end included in property
#నిర్వచించు PT_FLAG_COMBINE		4	// combine with syntax highlight
#నిర్వచించు PT_FLAG_OVERRIDE	8	// override any highlight

// Sign group
రకనిర్వచనం నిర్మాణం signgroup_S
{
    పూర్ణాంకం		sg_తదుపరి_sign_id;	// next sign id for this group
    short_u	sg_refcount;		// number of signs in this group
    char_u	sg_name[1];		// sign group name, actually longer
} signgroup_T;

రకనిర్వచనం నిర్మాణం sign_నమోదు sign_నమోదు_T;
నిర్మాణం sign_నమోదు
{
    పూర్ణాంకం		 se_id;		// unique identifier for each placed sign
    పూర్ణాంకం		 se_typenr;	// typenr of sign
    పూర్ణాంకం		 se_ప్రాధాన్యత;	// priority for highlighting
    linenr_T	 se_lnum;	// line number which has this sign
    signgroup_T	 *se_సమూహం;	// sign group
    sign_నమోదు_T *se_తదుపరి;	// next entry in a list of signs
    sign_నమోదు_T *se_prev;	// previous entry -- for easy reordering
};

/*
 * Sign attributes. Used by the screen refresh routines.
 */
రకనిర్వచనం నిర్మాణం sign_attrs_S {
    పూర్ణాంకం		sat_typenr;
    శూన్య	*sat_icon;
    char_u	*sat_పాఠ్యం;
    పూర్ణాంకం		sat_texthl;
    పూర్ణాంకం		sat_linehl;
    పూర్ణాంకం		sat_culhl;
    పూర్ణాంకం		sat_numhl;
    పూర్ణాంకం		sat_ప్రాధాన్యత;
} sign_attrs_T;

#ఒకవేళ defined(FEAT_SIGNS)
// Macros to get the sign group structure from the group name
# define SGN_కీలకం_OFF	offsetof(signgroup_T, sg_name)
# define HI2SG(hi)	((signgroup_T *)((hi)->hi_కీలకం - SGN_కీలకం_OFF))

// Default sign priority for highlighting
# define SIGN_DEF_PRIO	10

#అయితే_ముగింపు

/*
 * Argument list: Array of file names.
 * Used for the global argument list and the argument lists local to a window.
 */
రకనిర్వచనం నిర్మాణం arglist
{
    garray_T	al_ga;		// growarray with the array of file names
    పూర్ణాంకం		al_refcount;	// number of windows using this arglist
    పూర్ణాంకం		id;		// id of this arglist
} alist_T;

/*
 * For each argument remember the file name as it was given, and the buffer
 * number that contains the expanded file name (required for when ":cd" is
 * used).
 */
రకనిర్వచనం నిర్మాణం argentry
{
    char_u	*ae_fname;	// file name as specified
    పూర్ణాంకం		ae_fnum;	// buffer number with expanded file name
} aentry_T;

#నిర్వచించు ALIST(win)	(win)->w_alist
#నిర్వచించు GARGLIST	((aentry_T *)global_alist.al_ga.ga_దత్తాంశం)
#నిర్వచించు ARGLIST		((aentry_T *)ALIST(curwin)->al_ga.ga_దత్తాంశం)
#నిర్వచించు WARGLIST(wp)	((aentry_T *)ALIST(wp)->al_ga.ga_దత్తాంశం)
#నిర్వచించు AARGLIST(al)	((aentry_T *)((al)->al_ga.ga_దత్తాంశం))
#నిర్వచించు GARGCOUNT	(global_alist.al_ga.ga_len)
#నిర్వచించు ARGCOUNT	(ALIST(curwin)->al_ga.ga_len)
#నిర్వచించు WARGCOUNT(wp)	(ALIST(wp)->al_ga.ga_len)

/*
 * A list used for saving values of "emsg_silent".  Used by ex_try() to save the
 * value of "emsg_silent" if it was non-zero.  When this is done, the CSF_SILENT
 * flag below is set.
 */

రకనిర్వచనం నిర్మాణం eslist_elem eslist_T;
నిర్మాణం eslist_elem
{
    పూర్ణాంకం		saved_emsg_silent;	// saved value of "emsg_silent"
    eslist_T	*తదుపరి;			// next element on the list
};

/*
 * For conditional commands a stack is kept of nested conditionals.
 * When cs_idx < 0, there is no conditional command.
 */
#నిర్వచించు CSTACK_LEN	50

రకనిర్వచనం నిర్మాణం {
    పొట్టి	cs_flags[CSTACK_LEN];	// CSF_ flags
    అక్షరం	cs_pending[CSTACK_LEN];	// CSTP_: what's pending in ":finally"
    సంఘం {
	శూన్య	*csp_rv[CSTACK_LEN];	// return typeval for pending return
	శూన్య	*csp_ex[CSTACK_LEN];	// exception for pending throw
    }		cs_pend;
    శూన్య	*cs_forinfo[CSTACK_LEN]; // info used by ":for"
    పూర్ణాంకం		cs_పంక్తి[CSTACK_LEN];	// line nr of ":while"/":for" line
    పూర్ణాంకం		cs_ఖండం_id[CSTACK_LEN];    // block ID stack
    పూర్ణాంకం		cs_script_var_len[CSTACK_LEN];	// value of sn_var_vals.ga_len
						// when entering the block
    పూర్ణాంకం		cs_idx;			// current entry, or -1 if none
    పూర్ణాంకం		cs_looplevel;		// nr of nested ":while"s and ":for"s
    పూర్ణాంకం		cs_trylevel;		// nr of nested ":try"s
    eslist_T	*cs_emsg_silent_list;	// saved values of "emsg_silent"
    అక్షరం	cs_lflags;		// loop flags: CSL_ flags
} cstack_T;
#నిర్వచించు cs_rettv	cs_pend.csp_rv
#నిర్వచించు cs_exception	cs_pend.csp_ex

// There is no CSF_IF, the lack of CSF_WHILE, CSF_FOR and CSF_TRY means ":if"
// was used.
#నిర్వచించు CSF_TRUE	0x0001	// condition was TRUE
#నిర్వచించు CSF_చురుకైన	0x0002	// current state is active
#నిర్వచించు CSF_ELSE	0x0004	// ":else" has been passed
#నిర్వచించు CSF_WHILE	0x0008	// is a ":while"
#నిర్వచించు CSF_FOR	0x0010	// is a ":for"
#నిర్వచించు CSF_ఖండం	0x0020	// is a "{" block

#నిర్వచించు CSF_TRY	0x0100	// is a ":try"
#నిర్వచించు CSF_FINALLY	0x0200	// ":finally" has been passed
#నిర్వచించు CSF_CATCH	0x0400	// ":catch" has been seen
#నిర్వచించు CSF_THROWN	0x0800	// exception thrown to this try conditional
#నిర్వచించు CSF_CAUGHT	0x1000  // exception caught by this try conditional
#నిర్వచించు CSF_ముగిసిన	0x2000  // CSF_CAUGHT was handled by finish_exception()
#నిర్వచించు CSF_SILENT	0x4000	// "emsg_silent" reset by ":try"
// Note that CSF_ELSE is only used when CSF_TRY and CSF_WHILE are unset
// (an ":if"), and CSF_SILENT is only used when CSF_TRY is set.

#నిర్వచించు CSF_FUNC_DEF	0x8000	// a function was defined in this block

/*
 * What's pending for being reactivated at the ":endtry" of this try
 * conditional:
 */
#నిర్వచించు CSTP_NONE	0	// nothing pending in ":finally" clause
#నిర్వచించు CSTP_ERROR	1	// an error is pending
#నిర్వచించు CSTP_అంతరాయం	2	// an interrupt is pending
#నిర్వచించు CSTP_THROW	4	// a throw is pending
#నిర్వచించు CSTP_BREAK	8	// ":break" is pending
#నిర్వచించు CSTP_CONTINUE	16	// ":continue" is pending
#నిర్వచించు CSTP_RETURN	24	// ":return" is pending
#నిర్వచించు CSTP_ముగింపు	32	// ":finish" is pending

/*
 * Flags for the cs_lflags item in cstack_T.
 */
#నిర్వచించు CSL_HAD_LOOP	 1	// just found ":while" or ":for"
#నిర్వచించు CSL_HAD_ENDLOOP 2	// just found ":endwhile" or ":endfor"
#నిర్వచించు CSL_HAD_CONT	 4	// just found ":continue"
#నిర్వచించు CSL_HAD_FINA	 8	// just found ":finally"

/*
 * A list of error messages that can be converted to an exception.  "throw_msg"
 * is only set in the first element of the list.  Usually, it points to the
 * original message stored in that element, but sometimes it points to a later
 * message in the list.  See cause_errthrow().
 */
రకనిర్వచనం నిర్మాణం msglist msglist_T;
నిర్మాణం msglist
{
    msglist_T	*తదుపరి;		// next of several messages in a row
    అక్షరం	*సందేశం;		// original message, allocated
    అక్షరం	*throw_సందేశం;	// msg to throw: usually original one
    char_u	*sfile;		// value from estack_sfile(), allocated
    దీర్ఘ	slnum;		// line number for "sfile"
    పూర్ణాంకం		సందేశం_compiling;	// saved value of estack_compiling
};

/*
 * The exception types.
 */
రకనిర్వచనం గణన
{
    ET_USER,		// exception caused by ":throw" command
    ET_ERROR,		// error exception
    ET_అంతరాయం,	// interrupt exception triggered by Ctrl-C
} except_type_T;

/*
 * Structure describing an exception.
 * (don't use "struct exception", it's used by the math library).
 */
రకనిర్వచనం నిర్మాణం vim_exception except_T;
నిర్మాణం vim_exception
{
    except_type_T	type;		// exception type
    అక్షరం		*విలువ;		// exception value
    నిర్మాణం msglist	*messages;	// message(s) causing error exception
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
రకనిర్వచనం నిర్మాణం cleanup_stuff cleanup_T;
నిర్మాణం cleanup_stuff
{
    పూర్ణాంకం pending;		// error/interrupt/exception state
    except_T *exception;	// exception value
};

/*
 * Exception state that is saved and restored when calling timer callback
 * functions and deferred functions.
 */
రకనిర్వచనం నిర్మాణం exception_state_S exception_state_T;
నిర్మాణం exception_state_S
{
    except_T	*estate_current_exception;
    పూర్ణాంకం		estate_did_throw;
    పూర్ణాంకం		estate_need_rethrow;
    పూర్ణాంకం		estate_trylevel;
    పూర్ణాంకం		estate_did_emsg;
};

#నిర్వచితమైతే FEAT_SYN_HL
// struct passed to in_id_list()
నిర్మాణం sp_syn
{
    పూర్ణాంకం		inc_ట్యాగ్;	// ":syn include" unique tag
    పొట్టి	id;		// highlight group ID of item
    పొట్టి	*cont_లోపల_list;	// cont.in group IDs, if non-zero
};

/*
 * Each keyword has one keyentry, which is linked in a hash list.
 */
రకనిర్వచనం నిర్మాణం keyentry keyentry_T;

నిర్మాణం keyentry
{
    keyentry_T	*ke_తదుపరి;	// next entry with identical "keyword[]"
    నిర్మాణం sp_syn k_syn;	// struct passed to in_id_list()
    పొట్టి	*తదుపరి_list;	// ID list for next match (if non-zero)
    పూర్ణాంకం		flags;
    పూర్ణాంకం		k_char;		// conceal substitute character
    char_u	keyword[1];	// actually longer
};

/*
 * Struct used to store one state of the state stack.
 */
రకనిర్వచనం నిర్మాణం buf_state
{
    పూర్ణాంకం		    bs_idx;	 // index of pattern
    పూర్ణాంకం		    bs_flags;	 // flags for pattern
# ifdef FEAT_CONCEAL
    పూర్ణాంకం		    bs_seqnr;	 // stores si_seqnr
    పూర్ణాంకం		    bs_cchar;	 // stores si_cchar
# endif
    reg_extmatch_T *bs_extmatch; // external matches from start pattern
} bufstate_T;

/*
 * syn_state contains the syntax state stack for the start of one line.
 * Used by b_sst_array[].
 */
రకనిర్వచనం నిర్మాణం syn_state synstate_T;

నిర్మాణం syn_state
{
    synstate_T	*sst_తదుపరి;	// next entry in used or free list
    linenr_T	sst_lnum;	// line number for this state
    సంఘం
    {
	bufstate_T	sst_దొంతర[SST_FIX_స్థితులు]; // short state stack
	garray_T	sst_ga;	// growarray for long state stack
    } sst_union;
    పూర్ణాంకం		sst_తదుపరి_flags;	// flags for sst_next_list
    పూర్ణాంకం		sst_stacksize;	// number of states on the stack
    పొట్టి	*sst_తదుపరి_list;	// "nextgroup" list in this state
				// (this is a copy, don't free it!
    disptick_T	sst_tick;	// tick when last displayed
    linenr_T	sst_change_lnum;// when non-zero, change in this line
				// may have made the state invalid
};
#అయితే_ముగింపు // FEAT_SYN_HL

#నిర్వచించు MAX_HL_ID       20000	// maximum value for a highlight ID.

/*
 * Structure shared between syntax.c, screen.c and gui_x11.c.
 */
రకనిర్వచనం నిర్మాణం attr_నమోదు
{
    పొట్టి	    ae_attr;		// HL_BOLD, etc.
    సంఘం
    {
	నిర్మాణం
	{
	    char_u	    *ప్రారంభం;	// start escape sequence
	    char_u	    *stop;	// stop escape sequence
	} పదం;
	నిర్మాణం
	{
	    // These colors need to be > 8 bits to hold 256.
	    short_u	    fg_color;	// foreground color number
	    short_u	    bg_color;	// background color number
	    short_u	    ul_color;	// underline color number
	    short_u	    font;	// font number
#నిర్వచితమైతే FEAT_TERMGUICOLORS
	    guicolor_T	    fg_rgb;	// foreground color RGB
	    guicolor_T	    bg_rgb;	// background color RGB
	    guicolor_T	    ul_rgb;	// underline color RGB
#అయితే_ముగింపు
	} cterm;
#నిర్వచితమైతే FEAT_GUI
	నిర్మాణం
	{
	    guicolor_T	    fg_color;	// foreground color handle
	    guicolor_T	    bg_color;	// background color handle
	    guicolor_T	    sp_color;	// special color handle
	    GuiFont	    font;	// font handle
# ifdef FEAT_XFONTSET
	    GuiFontset	    fontset;	// fontset handle
# endif
	} gui;
#అయితే_ముగింపు
    } ae_u;
} attrentry_T;

#నిర్వచితమైతే USE_ICONV
# ifdef HAVE_ICONV_H
#  include <iconv.h>
# లేకపోతే
#  ఒకవేళ defined(MACOS_X)
#   include <sys/errno.h>
#   ifndef EILSEQ
#    define EILSEQ ENOENT // Early MacOS X does not have EILSEQ
#   endif
రకనిర్వచనం నిర్మాణం _iconv_t *iconv_t;
#  లేకపోతే
#   include <errno.h>
#  endif
రకనిర్వచనం శూన్య *iconv_t;
# endif
#అయితే_ముగింపు

/*
 * Used for the typeahead buffer: typebuf.
 */
రకనిర్వచనం నిర్మాణం
{
    char_u	*tb_buf;	// buffer for typed characters
    char_u	*tb_noremap;	// mapping flags for characters in tb_buf[]
    పూర్ణాంకం		tb_buflen;	// size of tb_buf[]
    పూర్ణాంకం		tb_off;		// current position in tb_buf[]
    పూర్ణాంకం		tb_len;		// number of valid bytes in tb_buf[]
    పూర్ణాంకం		tb_maplen;	// nr of mapped bytes in tb_buf[]
    పూర్ణాంకం		tb_silent;	// nr of silently mapped bytes in tb_buf[]
    పూర్ణాంకం		tb_no_abbr_cnt; // nr of bytes without abbrev. in tb_buf[]
    పూర్ణాంకం		tb_change_cnt;	// nr of time tb_buf was changed; never zero
} typebuf_T;

// Struct to hold the saved typeahead for save_typeahead().
రకనిర్వచనం నిర్మాణం
{
    typebuf_T		save_typebuf;
    పూర్ణాంకం			typebuf_valid;	    // TRUE when save_typebuf valid
    పూర్ణాంకం			old_char;
    పూర్ణాంకం			old_mod_ముసుగు;
    buffheader_T	save_readbuf1;
    buffheader_T	save_readbuf2;
#నిర్వచితమైతే USE_ఆదానం_BUF
    char_u		*save_inputbuf;
#అయితే_ముగింపు
} tasave_T;

/*
 * Used for conversion of terminal I/O and script files.
 */
రకనిర్వచనం నిర్మాణం
{
    పూర్ణాంకం		vc_type;	// zero or one of the CONV_ values
    పూర్ణాంకం		vc_గుణకం;	// max. expansion factor
#నిర్వచితమైతే MSWIN
    పూర్ణాంకం		vc_cpfrom;	// codepage to convert from (CONV_CODEPAGE)
    పూర్ణాంకం		vc_cpto;	// codepage to convert to (CONV_CODEPAGE)
#అయితే_ముగింపు
#నిర్వచితమైతే USE_ICONV
    iconv_t	vc_fd;		// for CONV_ICONV
#అయితే_ముగింపు
    పూర్ణాంకం		vc_fail;	// fail for invalid char, don't use '?'
} vimconv_T;

/*
 * Structure used for the command line history.
 */
రకనిర్వచనం నిర్మాణం hist_నమోదు
{
    పూర్ణాంకం		hisnum;		// identifying number
    పూర్ణాంకం		viminfo;	// when TRUE hisstr comes from viminfo
    char_u	*hisstr;	// actual entry, separator char after the NUL
    size_t	hisstrlen;	// length of hisstr (excluding the NUL)
    time_t	సమయం_సమితి;	// when it was typed, zero if unknown
} histentry_T;

#నిర్వచించు CONV_NONE		0
#నిర్వచించు CONV_గమ్యం_UTF8		1
#నిర్వచించు CONV_9_గమ్యం_UTF8		2
#నిర్వచించు CONV_గమ్యం_LATIN1		3
#నిర్వచించు CONV_గమ్యం_LATIN9		4
#నిర్వచించు CONV_ICONV		5
#నిర్వచితమైతే MSWIN
# define CONV_CODEPAGE		10	// codepage -> codepage
#అయితే_ముగింపు
#నిర్వచితమైతే MACOS_X
# define CONV_MAC_LATIN1	20
# define CONV_LATIN1_MAC	21
# define CONV_MAC_UTF8		22
# define CONV_UTF8_MAC		23
#అయితే_ముగింపు

/*
 * Structure used for mappings and abbreviations.
 */
రకనిర్వచనం నిర్మాణం mapblock mapblock_T;
నిర్మాణం mapblock
{
    mapblock_T	*m_తదుపరి;	// next mapblock in list
    mapblock_T	*m_alt;		// pointer to mapblock of the same mapping
				// with an alternative form of m_keys, or NULL
				// if there is no such mapblock
    char_u	*m_కీలకాలు;	// mapped from, lhs
    char_u	*m_str;		// mapped to, rhs
    char_u	*m_orig_str;	// rhs as entered by the user
    పూర్ణాంకం		m_keylen;	// strlen(m_keys)
    పూర్ణాంకం		m_బహుళకం;		// valid mode
    పూర్ణాంకం		m_simplified;	// m_keys was simplified, do not use this map
				// if key_protocol_enabled() returns TRUE
    పూర్ణాంకం		m_noremap;	// if non-zero no re-mapping for m_str
    అక్షరం	m_silent;	// <silent> used, don't echo commands
    అక్షరం	m_nowait;	// <nowait> used
#నిర్వచితమైతే FEAT_EVAL
    అక్షరం	m_సమాసం;		// <expr> used, m_str is an expression
    sctx_T	m_script_ctx;	// SCTX where map was defined
#అయితే_ముగింపు
};


/*
 * Used for highlighting in the status line.
 */
రకనిర్వచనం నిర్మాణం
{
    char_u	*ప్రారంభం;
    పూర్ణాంకం		userhl;		// 0: no HL, 1-9: User HL, < 0 for syn ID
} stl_hlrec_T;

/*
 * Used for statusline click function regions.
 */
రకనిర్వచనం నిర్మాణం {
    char_u	*ప్రారంభం;		// position in output buffer where region starts
    char_u	*funcname;	// function name (NULL = end/close marker)
    పూర్ణాంకం		minwid;		// minwid value from %N@Func@
} stl_clickrec_T;

/*
 * Per-window resolved click regions (screen column based).
 */
రకనిర్వచనం నిర్మాణం {
    పూర్ణాంకం		అడ్డువరుస;		// screen row where region lives
    పూర్ణాంకం		నిలువు_ప్రారంభం;	// screen column where region starts
    పూర్ణాంకం		నిలువు_చివర;	// screen column where region ends
    char_u	*funcname;	// function name (allocated copy)
    పూర్ణాంకం		minwid;		// minwid value
    పూర్ణాంకం		tabnr;		// tab page number (tabpanel only, 0 otherwise)
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
రకనిర్వచనం నిర్మాణం hashitem_S
{
    long_u	hi_hash;	// cached hash number of hi_key
    char_u	*hi_కీలకం;
} hashitem_T;

// The address of "hash_removed" is used as a magic number for hi_key to
// indicate a removed item.
#నిర్వచించు HI_కీలకం_REMOVED &hash_removed
#నిర్వచించు HASHITEM_ఖాళీలు(hi) ((hi)->hi_కీలకం == ఖాళీ || (hi)->hi_కీలకం == &hash_removed)

// Initial size for a hashtable.  Our items are relatively small and growing
// is expensive, thus use 16 as a start.  Must be a power of 2.
// This allows for storing 10 items (2/3 of 16) before a resize is needed.
#నిర్వచించు HT_INIT_కొలత 16

// flags used for ht_flags
#నిర్వచించు HTFLAGS_ERROR	0x01	// Set when growing failed, can't add more
				// items before growing works.
#నిర్వచించు HTFLAGS_FROZEN	0x02	// Trying to add or remove an item will result
				// in an error message.

రకనిర్వచనం నిర్మాణం hashtable_S
{
    long_u	ht_ముసుగు;	// mask used for hash value (nr of items in
				// array is "ht_mask" + 1)
    long_u	ht_వాడిన;	// number of items used
    long_u	ht_filled;	// number of items used + removed
    పూర్ణాంకం		ht_మారింది;	// incremented when adding or removing an item
    పూర్ణాంకం		ht_locked;	// counter for hash_lock()
    పూర్ణాంకం		ht_flags;	// HTFLAGS_ values
    hashitem_T	*ht_array;	// points to the array, allocated when it's
				// not "ht_smallarray"
    hashitem_T	ht_smallarray[HT_INIT_కొలత];   // initial array
} hashtab_T;

రకనిర్వచనం long_u hash_T;		// Type for hi_hash


// Use 64-bit Number.
#నిర్వచితమైతే MSWIN
   రకనిర్వచనం __int64		varnumber_T;
   రకనిర్వచనం గుర్తులేని __int64	uvarnumber_T;
# define VARNUM_MIN		_I64_MIN
# define VARNUM_MAX		_I64_MAX
# define UVARNUM_MAX		_UI64_MAX
#లేదావేళ defined(HAVE_NO_LONG_LONG)
# ఒకవేళ defined(HAVE_STDINT_H)
   రకనిర్వచనం int64_t		varnumber_T;
   రకనిర్వచనం uint64_t		uvarnumber_T;
#  define VARNUM_MIN		INT64_MIN
#  define VARNUM_MAX		INT64_MAX
#  define UVARNUM_MAX		UINT64_MAX
# లేకపోతే
   // this may cause trouble for code that depends on 64 bit ints
   రకనిర్వచనం దీర్ఘ			varnumber_T;
   రకనిర్వచనం గుర్తులేని దీర్ఘ	uvarnumber_T;
#  define VARNUM_MIN		LONG_MIN
#  define VARNUM_MAX		LONG_MAX
#  define UVARNUM_MAX		ULONG_MAX
# endif
#లేకపోతే
  రకనిర్వచనం దీర్ఘ దీర్ఘ		varnumber_T;
  రకనిర్వచనం గుర్తులేని దీర్ఘ దీర్ఘ	uvarnumber_T;
# ifdef LLONG_MIN
#  define VARNUM_MIN		LLONG_MIN
#  define VARNUM_MAX		LLONG_MAX
#  define UVARNUM_MAX		ULLONG_MAX
# లేకపోతే
#  define VARNUM_MIN		LONG_LONG_MIN
#  define VARNUM_MAX		LONG_LONG_MAX
#  define UVARNUM_MAX		ULONG_LONG_MAX
# endif
#అయితే_ముగింపు

// Struct that holds both a normal function name and a partial_T, as used for a
// callback argument.
// When used temporarily "cb_name" is not allocated.  The refcounts to either
// the function or the partial are incremented and need to be decremented
// later with free_callback().
రకనిర్వచనం నిర్మాణం {
    char_u	*cb_name;
    partial_T	*cb_partial;
    పూర్ణాంకం		cb_free_name;	    // cb_name was allocated
} callback_T;

రకనిర్వచనం నిర్మాణం isn_S isn_T;	    // instruction
రకనిర్వచనం నిర్మాణం dfunc_S dfunc_T;	    // :def function

రకనిర్వచనం నిర్మాణం type_S type_T;
రకనిర్వచనం నిర్మాణం ufunc_S ufunc_T;

రకనిర్వచనం నిర్మాణం jobvar_S పని_T;
రకనిర్వచనం నిర్మాణం readq_S readq_T;
రకనిర్వచనం నిర్మాణం writeq_S writeq_T;
రకనిర్వచనం నిర్మాణం jsonq_S jsonq_T;
రకనిర్వచనం నిర్మాణం cbq_S cbq_T;
రకనిర్వచనం నిర్మాణం channel_S channel_T;
రకనిర్వచనం నిర్మాణం cctx_S cctx_T;
రకనిర్వచనం నిర్మాణం ectx_S ectx_T;
రకనిర్వచనం నిర్మాణం instr_S instr_T;
రకనిర్వచనం నిర్మాణం class_S class_T;
రకనిర్వచనం నిర్మాణం object_S object_T;
రకనిర్వచనం నిర్మాణం typealias_S typealias_T;

రకనిర్వచనం గణన
{
    VAR_UNKNOWN = 0,	// not set, any type or "void" allowed
    VAR_ANY,		// used for "any" type
    VAR_VOID,		// no value (function not returning anything)
    VAR_BOOL,		// "v_number" is used: VVAL_TRUE or VVAL_FALSE
    VAR_SPECIAL,	// "v_number" is used: VVAL_NULL or VVAL_NONE
    VAR_సంఖ్య,		// "v_number" is used
    VAR_FLOAT,		// "v_float" is used
    VAR_STRING,		// "v_string" is used
    VAR_BLOB,		// "v_blob" is used
    VAR_FUNC,		// "v_string" is function name
    VAR_PARTIAL,	// "v_partial" is used
    VAR_LIST,		// "v_list" is used
    VAR_DICT,		// "v_dict" is used
    VAR_పని,		// "v_job" is used
    VAR_CHANNEL,	// "v_channel" is used
    VAR_INSTR,		// "v_instr" is used
    VAR_CLASS,		// "v_class" is used (also used for interface)
    VAR_OBJECT,		// "v_object" is used
    VAR_TYPEALIAS,	// "v_typealias" is used
    VAR_TUPLE		// "v_tuple" is used
} vartype_T;

// A type specification.
నిర్మాణం type_S {
    vartype_T	    tt_type;
    పూర్ణాంకం	    tt_argcount;    // for func, incl. vararg, -1 for unknown
    పూర్ణాంకం	    tt_min_argcount; // number of non-optional arguments
    short_u	    tt_flags;	    // TTFLAG_ values
    type_T	    *tt_member;	    // for list, dict, func return type
    class_T	    *tt_class;	    // for class and object
    type_T	    **tt_వాదనలు;	    // func argument types, allocated
};

రకనిర్వచనం నిర్మాణం {
    type_T	*type_curr;	    // current type, value type
    type_T	*type_decl;	    // declared type or equal to type_current
} type2_T;

#నిర్వచించు TTFLAG_VARARGS	    0x01    // func args ends with "..."
#నిర్వచించు TTFLAG_BOOL_OK	    0x02    // can be converted to bool
#నిర్వచించు TTFLAG_FLOAT_OK	    0x04    // number can be used/converted to float
#నిర్వచించు TTFLAG_సంఖ్య_OK    0x08    // number can be used for a float
#నిర్వచించు TTFLAG_STATIC	    0x10    // one of the static types, e.g. t_any
#నిర్వచించు TTFLAG_CONST	    0x20    // cannot be changed
#నిర్వచించు TTFLAG_SUPER	    0x40    // object from "super".
#నిర్వచించు TTFLAG_GENERIC	    0x80    // generic type
#నిర్వచించు TTFLAG_TUPLE_OK	    0x100   // tuple can be used for a list

#నిర్వచించు IS_GENERIC_TYPE(type)	\
    ((type->tt_flags & TTFLAG_GENERIC) == TTFLAG_GENERIC)

// Type check flags
#నిర్వచించు TYPECHK_సంఖ్య_OK	0x1	// number is accepted for a float
#నిర్వచించు TYPECHK_TUPLE_OK	0x2	// tuple is accepted for a list

రకనిర్వచనం గణన {
    VIM_ACCESS_వ్యక్తిగత,	// read/write only inside the class
    VIM_ACCESS_READ,	// read everywhere, write only inside the class
    VIM_ACCESS_ALL	// read/write everywhere
} omacc_T;

#నిర్వచించు OCMFLAG_HAS_TYPE	0x01	// type specified explicitly
#నిర్వచించు OCMFLAG_FINAL		0x02	// "final" object/class member
#నిర్వచించు OCMFLAG_CONST		0x04	// "const" object/class member

/*
 * Object methods called by builtin functions (e.g. string(), empty(), etc.)
 */
రకనిర్వచనం గణన {
    CLASS_BUILTIN_INVALID,
    CLASS_BUILTIN_STRING,
    CLASS_BUILTIN_ఖాళీలు,
    CLASS_BUILTIN_LEN,
    CLASS_BUILTIN_MAX
} class_builtin_T;

/*
 * Entry for an object or class member variable.
 */
రకనిర్వచనం నిర్మాణం {
    string_T	ocm_name;	// allocated
    omacc_T	ocm_access;
    type_T	*ocm_type;
    పూర్ణాంకం		ocm_flags;
    char_u	*ocm_init;	// allocated
    sctx_T	ocm_init_sctx;	// script context of the initializer expression
} ocmember_T;

// used for the lookup table of a class member index and object method index
రకనిర్వచనం నిర్మాణం itf2class_S itf2class_T;
నిర్మాణం itf2class_S {
    itf2class_T	*i2c_తదుపరి;
    class_T	*i2c_class;
    పూర్ణాంకం		i2c_is_పద్ధతి;	    // TRUE for method indexes
    // array with ints follows
};

#నిర్వచించు CLASS_INTERFACE	    0x1
#నిర్వచించు CLASS_EXTENDED	    0x2	    // another class extends this one
#నిర్వచించు CLASS_ABSTRACT	    0x4	    // abstract class
#నిర్వచించు CLASS_ENUM	    0x8	    // enum

// "class_T": used for v_class of typval of VAR_CLASS
// Also used for an interface (class_flags has CLASS_INTERFACE).
నిర్మాణం class_S
{
    string_T	class_name;		// allocated
    పూర్ణాంకం		class_flags;		// CLASS_ flags

    పూర్ణాంకం		class_refcount;
    పూర్ణాంకం		class_copyID;		// used by garbage collection
    class_T	*class_తదుపరి_వాడిన;	// for list headed by "first_class"
    class_T	*class_prev_వాడిన;	// for list headed by "first_class"

    class_T	*class_extends;		// parent class or NULL

    // interfaces declared for the class
    పూర్ణాంకం		class_interface_లెక్క;
    char_u	**class_interfaces;	// allocated array of names
    class_T	**class_interfaces_cl;	// interfaces (counts as reference)
    itf2class_T	*class_itf2class;	// member index lookup tables

    // class members: "static varname"
    పూర్ణాంకం		class_class_member_లెక్క;
    ocmember_T	*class_class_members;	// allocated
    typval_T	*class_members_tv;	// allocated array of class member vals

    // class functions: "static def SomeMethod()"
    పూర్ణాంకం		class_class_function_లెక్క;	    // total count
    పూర్ణాంకం		class_class_function_లెక్క_బిడ్డ;   // count without "extends"
    ufunc_T	**class_class_functions;	// allocated

    // object members: "this.varname"
    పూర్ణాంకం		class_obj_member_లెక్క;
    ocmember_T	*class_obj_members;	// allocated

    // object methods: "def SomeMethod()"
    పూర్ణాంకం		class_obj_పద్ధతి_లెక్క;		    // total count
    పూర్ణాంకం		class_obj_పద్ధతి_లెక్క_బిడ్డ;	    // count without "extends"
    ufunc_T	**class_obj_methods;	// allocated

					// index of builtin methods
    పూర్ణాంకం		class_builtin_methods[CLASS_BUILTIN_MAX];

    garray_T	class_type_list;	// used for type pointers
    type_T	class_type;		// type used for the class
    type_T	class_object_type;	// same as class_type but VAR_OBJECT
};

#నిర్వచించు IS_INTERFACE(cl)	((cl)->class_flags & CLASS_INTERFACE)
#నిర్వచించు IS_ENUM(cl)		((cl)->class_flags & CLASS_ENUM)

// Used for v_object of typval of VAR_OBJECT.
// The member variables follow in an array of typval_T.
నిర్మాణం object_S
{
    class_T	*obj_class;	    // class this object is created for;
				    // pointer adds to class_refcount
    పూర్ణాంకం		obj_refcount;

    object_T	*obj_తదుపరి_వాడిన;	    // for list headed by "first_object"
    object_T	*obj_prev_వాడిన;	    // for list headed by "first_object"
    పూర్ణాంకం		obj_copyID;	    // used by garbage collection
};

నిర్మాణం typealias_S
{
    పూర్ణాంకం	    ta_refcount;
    type_T  *ta_type;
    char_u  *ta_name;
    పూర్ణాంకం	    ta_లోతు;	    // number of levels of "ta_type"
};

/*
 * Structure to hold an internal variable without a name.
 */
నిర్మాణం typval_S
{
    vartype_T	v_type;
    అక్షరం	v_తాళం;	    // see below: VAR_LOCKED, VAR_FIXED
    సంఘం
    {
	varnumber_T	v_సంఖ్య;	// number value
	float_T		v_float;	// floating point number value
	char_u		*v_string;	// string value (can be NULL)
	list_T		*v_list;	// list value (can be NULL)
	dict_T		*v_dict;	// dict value (can be NULL)
	partial_T	*v_partial;	// closure: function with args
#నిర్వచితమైతే FEAT_పని_CHANNEL
	పని_T		*v_పని;		// job value (can be NULL)
	channel_T	*v_channel;	// channel value (can be NULL)
#అయితే_ముగింపు
	blob_T		*v_blob;	// blob value (can be NULL)
	instr_T		*v_instr;	// instructions to execute
	class_T		*v_class;	// class value (can be NULL)
	object_T	*v_object;	// object value (can be NULL)
	typealias_T	*v_typealias;	// user-defined type name
	tuple_T		*v_tuple;	// tuple
    }		vval;
};

// Values for "dv_scope".
#నిర్వచించు VAR_SCOPE     1	// a:, v:, s:, etc. scope dictionaries
#నిర్వచించు VAR_DEF_SCOPE 2	// l:, g: scope dictionaries: here funcrefs are not
			// allowed to mask existing functions

// Values for "v_lock".
#నిర్వచించు VAR_LOCKED	    1	// locked with lock(), can use unlock()
#నిర్వచించు VAR_FIXED	    2	// locked forever
#నిర్వచించు VAR_వస్తువులు_LOCKED    4	// items of non-materialized list locked

/*
 * Structure to hold an item of a list: an internal variable without a name.
 */
రకనిర్వచనం నిర్మాణం listitem_S listitem_T;

నిర్మాణం listitem_S
{
    listitem_T	*li_తదుపరి;	// next item in list
    listitem_T	*li_prev;	// previous item in list
    typval_T	li_tv;		// type and value of the variable
};

// Struct used by those that are using an item in a list.
రకనిర్వచనం నిర్మాణం listwatch_S listwatch_T;

నిర్మాణం listwatch_S
{
    listitem_T		*lw_అంశం;	// item being watched
    listwatch_T		*lw_తదుపరి;	// next watcher
};

/*
 * Structure to hold info about a list.
 * Order of members is optimized to reduce padding.
 * When created by range() it will at first have special value:
 *  lv_first == &range_list_item;
 * and use lv_start, lv_end, lv_stride.
 */
నిర్మాణం listvar_S
{
    listitem_T	*lv_మొదటి;	// first item, NULL if none, &range_list_item
				// for a non-materialized list
    listwatch_T	*lv_watch;	// first watcher, NULL if none
    సంఘం {
	నిర్మాణం {	// used for non-materialized range list:
			// "lv_first" is &range_list_item
	    varnumber_T lv_ప్రారంభం;
	    varnumber_T lv_చివర;
	    పూర్ణాంకం		lv_stride;
	} nonmat;
	నిర్మాణం {	// used for materialized list
	    listitem_T	*lv_చివరి;	// last item, NULL if none
	    listitem_T	*lv_idx_అంశం;	// when not NULL item at index "lv_idx"
	    పూర్ణాంకం		lv_idx;		// cached index of an item
	} mat;
    } lv_u;
    type_T	*lv_type;	// current type, allocated by alloc_type()
    list_T	*lv_copylist;	// copied list used by deepcopy()
    list_T	*lv_వాడిన_తదుపరి;	// next list in used lists list
    list_T	*lv_వాడిన_prev;	// previous list in used lists list
    పూర్ణాంకం		lv_refcount;	// reference count
    పూర్ణాంకం		lv_len;		// number of items
    పూర్ణాంకం		lv_with_వస్తువులు;	// number of items following this struct that
				// should not be freed
    పూర్ణాంకం		lv_copyID;	// ID used by deepcopy()
    అక్షరం	lv_తాళం;	// zero, VAR_LOCKED, VAR_FIXED
};

/*
 * Static list with 10 items.  Use init_static_list() to initialize.
 */
రకనిర్వచనం నిర్మాణం {
    list_T	sl_list;	// must be first
    listitem_T	sl_వస్తువులు[10];
} staticList10_T;

/*
 * Structure to hold an item of a Dictionary.
 * Also used for a variable.
 * The key is copied into "di_key" to avoid an extra alloc/free for it.
 */
నిర్మాణం dictitem_S
{
    typval_T	di_tv;		// type and value of the variable
    char_u	di_flags;	// DI_FLAGS_ flags (only used for variable)
    char_u	di_కీలకం[1];	// key (actually longer!)
};
రకనిర్వచనం నిర్మాణం dictitem_S dictitem_T;

/*
 * A dictitem with a 16 character key (plus NUL).  This is an efficient way to
 * have a fixed-size dictitem.
 */
#నిర్వచించు DICTITEM16_కీలకం_LEN 16
నిర్మాణం dictitem16_S
{
    typval_T	di_tv;		// type and value of the variable
    char_u	di_flags;	// DI_FLAGS_ flags (only used for variable)
    char_u	di_కీలకం[DICTITEM16_కీలకం_LEN + 1];	//కీలకం
};
రకనిర్వచనం నిర్మాణం dictitem16_S dictitem16_T;

// Flags for "di_flags"
#నిర్వచించు DI_FLAGS_RO	   0x01	    // read-only variable
#నిర్వచించు DI_FLAGS_RO_SBX	   0x02	    // read-only in the sandbox
#నిర్వచించు DI_FLAGS_FIX	   0x04	    // fixed: no :unlet or remove()
#నిర్వచించు DI_FLAGS_తాళం	   0x08	    // locked variable
#నిర్వచించు DI_FLAGS_ALLOC	   0x10	    // separately allocated
#నిర్వచించు DI_FLAGS_RELOAD	   0x20	    // set when script sourced again

/*
 * Structure to hold info about a Dictionary.
 */
నిర్మాణం dictvar_S
{
    అక్షరం	dv_తాళం;	// zero, VAR_LOCKED, VAR_FIXED
    అక్షరం	dv_scope;	// zero, VAR_SCOPE, VAR_DEF_SCOPE
    పూర్ణాంకం		dv_refcount;	// reference count
    పూర్ణాంకం		dv_copyID;	// ID used by deepcopy()
    hashtab_T	dv_hashtab;	// hashtab that refers to the items
    type_T	*dv_type;	// current type, allocated by alloc_type()
    dict_T	*dv_copydict;	// copied dict used by deepcopy()
    dict_T	*dv_వాడిన_తదుపరి;	// next dict in used dicts list
    dict_T	*dv_వాడిన_prev;	// previous dict in used dicts list
};

/*
 * Structure to hold info about a blob.
 */
నిర్మాణం blobvar_S
{
    garray_T	bv_ga;		// growarray with the data
    పూర్ణాంకం		bv_refcount;	// reference count
    అక్షరం	bv_తాళం;	// zero, VAR_LOCKED, VAR_FIXED
};

/*
 * Structure to hold info about a tuple.
 */
నిర్మాణం tuplevar_S
{
    garray_T	tv_వస్తువులు;	// tuple items
    type_T	*tv_type;	// current type, allocated by alloc_type()
    tuple_T	*tv_copytuple;	// copied tuple used by deepcopy()
    tuple_T	*tv_వాడిన_తదుపరి;	// next tuple in used tuples list
    tuple_T	*tv_వాడిన_prev;	// previous tuple in used tuples list
    పూర్ణాంకం		tv_refcount;	// reference count
    పూర్ణాంకం		tv_copyID;	// ID used by deepcopy()
    అక్షరం	tv_తాళం;	// zero, VAR_LOCKED, VAR_FIXED
};

/*
 * Structure to hold a generic type information
 */
నిర్మాణం generictype_S
{
    type_T	*gt_type;	// generic or concrete type
    char_u	*gt_name;	// type name
};

/*
 * Generic function args table
 */
నిర్మాణం gfargs_tab_S
{
    garray_T	gfat_వాదనలు;
    garray_T	gfat_param_types;
    garray_T	gfat_arg_types;
};

రకనిర్వచనం పూర్ణాంకం (*cfunc_T)(పూర్ణాంకం argcount, typval_T *argvars, typval_T *rettv, శూన్య *state);
రకనిర్వచనం శూన్య (*cfunc_free_T)(శూన్య *state);

// type of getline() last argument
రకనిర్వచనం గణన {
    GETLINE_NONE,	    // do not concatenate any lines
    GETLINE_CONCAT_CONT,    // concatenate continuation lines with backslash
    GETLINE_CONCAT_CONTBAR, // concatenate continuation lines with \ and |
    GETLINE_CONCAT_ALL	    // concatenate continuation and Vim9 # comment lines
} getline_opt_T;

రకనిర్వచనం నిర్మాణం svar_S svar_T;

#ఒకవేళ defined(FEAT_EVAL)
/*
 * Info used by a ":for" loop.
 */
రకనిర్వచనం నిర్మాణం
{
    పూర్ణాంకం		fi_semicolon;	// TRUE if ending in '; var]'
    పూర్ణాంకం		fi_varcount;	// nr of variables in [] or zero
    పూర్ణాంకం		fi_break_లెక్క;	// nr of line breaks encountered
    listwatch_T	fi_lw;		// keep an eye on the item used.
    list_T	*fi_list;	// list being used
    పూర్ణాంకం		fi_bi;		// index of blob
    blob_T	*fi_blob;	// blob being used
    char_u	*fi_string;	// copy of string being used
    పూర్ణాంకం		fi_byte_idx;	// byte index in fi_string
    tuple_T	*fi_tuple;	// tuple being used
    పూర్ణాంకం		fi_tuple_idx;	// tuple index in fi_tuple
    పూర్ణాంకం		fi_cs_flags;	// cs_flags or'ed together
} forinfo_T;

రకనిర్వచనం నిర్మాణం funccall_S funccall_T;

// values used for "uf_def_status"
రకనిర్వచనం గణన {
    UF_NOT_COMPILED,	    // executed with interpreter
    UF_గమ్యం_BE_COMPILED,	    // to be compiled before execution
    UF_COMPILING,	    // in compile_def_function()
    UF_COMPILED,	    // successfully compiled
    UF_COMPILE_ERROR	    // compilation error, cannot execute
} def_స్థితి_T;

/*
 * Structure to hold info for a user function.
 * When adding a field check copy_lambda_to_global_func().
 */
నిర్మాణం ufunc_S
{
    పూర్ణాంకం		uf_varargs;	// variable nr of arguments (old style)
    పూర్ణాంకం		uf_flags;	// FC_ flags
    పూర్ణాంకం		uf_calls;	// nr of active calls
    పూర్ణాంకం		uf_cleared;	// func_clear() was already called
    def_స్థితి_T uf_def_స్థితి; // UF_NOT_COMPILED, UF_TO_BE_COMPILED, etc.
    పూర్ణాంకం		uf_dfunc_idx;	// only valid if uf_def_status is UF_COMPILED

    class_T	*uf_class;	// for class/object method and constructor;
				// does not count for class_refcount.
				// class of the object which is invoking this
				// function.
    class_T	*uf_defclass;	// class where this function is defined.

    garray_T	uf_వాదనలు;	// arguments, including optional arguments
    garray_T	uf_def_వాదనలు;	// default argument expressions
    పూర్ణాంకం		uf_వాదనలు_visible; // normally uf_args.ga_len, less when
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
    పూర్ణాంకం		uf_ఖండం_లోతు;	// nr of entries in uf_block_ids
    పూర్ణాంకం		*uf_ఖండం_ids;	// blocks a :def function is defined inside
# ఒకవేళ defined(FEAT_LUA)
    cfunc_T     uf_cb;		// callback function for cfunc
    cfunc_free_T uf_cb_free;    // callback function to free cfunc
    శూన్య	*uf_cb_state;   // state of uf_cb
# endif

    // for generic functions
    పూర్ణాంకం		uf_generic_argcount;// type argument count
    generic_T	*uf_generic_వాదనలు;   // generic types
    type_T	*uf_generic_param_types; // list of allocated generic types
    garray_T	uf_generic_arg_types; // list of allocated type arguments
    hashtab_T	uf_generic_functab; // generic function table

    garray_T	uf_పంక్తులు;	// function lines

    పూర్ణాంకం		uf_debug_tick;	// when last checked for a breakpoint in this
				// function.
    పూర్ణాంకం		uf_has_breakpoint;  // TRUE when a breakpoint has been set in
				    // this function.
# ifdef FEAT_PROFILE
    పూర్ణాంకం		uf_profiling;	// TRUE when func is being profiled
    పూర్ణాంకం		uf_prof_initialized;
    hash_T	uf_hash;	// hash for uf_name when profiling
    // profiling the function as a whole
    పూర్ణాంకం		uf_tm_లెక్క;	// nr of calls
    proftime_T	uf_tm_మొత్తం;	// time spent in function + children
    proftime_T	uf_tm_self;	// time spent in function itself
    proftime_T	uf_tm_children;	// time spent in children this call
    // profiling the function per line
    పూర్ణాంకం		*uf_tml_లెక్క;	// nr of times line was executed
    proftime_T	*uf_tml_మొత్తం;	// time spent in a line + children
    proftime_T	*uf_tml_self;	// time spent in a line itself
    proftime_T	uf_tml_ప్రారంభం;	// start time for current line
    proftime_T	uf_tml_children; // time spent in children for this line
    proftime_T	uf_tml_wait;	// start wait time for current line
    పూర్ణాంకం		uf_tml_idx;	// index of line being timed; -1 if none
    పూర్ణాంకం		uf_tml_execed;	// line being timed was executed
# endif
    sctx_T	uf_script_ctx;	// SCTX where function was defined,
				// used for s: variables; sc_version changed
				// for :function
    పూర్ణాంకం		uf_script_ctx_సంస్కరణ;  // original sc_version of SCTX
    పూర్ణాంకం		uf_refcount;	// reference count, see func_name_refcount()

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
# define FC_సంవృతం  0x08	// closure, uses outer scope variables
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
# define IS_OBJECT_పద్ధతి(ufunc) ((ufunc->uf_flags & FC_OBJECT) == FC_OBJECT)
// Is "ufunc" a class new() constructor method?
# define IS_CONSTRUCTOR_పద్ధతి(ufunc) ((ufunc->uf_flags & FC_NEW) == FC_NEW)
// Is "ufunc" an abstract class method?
# define IS_ABSTRACT_పద్ధతి(ufunc) ((ufunc->uf_flags & FC_ABSTRACT) == FC_ABSTRACT)
# define IS_GENERIC_FUNC(ufunc) (((ufunc)->uf_flags & FC_GENERIC) == FC_GENERIC)

# define MAX_FUNC_వాదనలు	20	// maximum number of function arguments
# define VAR_SHORT_LEN	20	// short variable name length
# define FIXVAR_CNT	12	// number of fixed variables

/*
 * Structure to hold info for a function that is currently being executed.
 */
నిర్మాణం funccall_S
{
    ufunc_T	*fc_func;	// function being called
    పూర్ణాంకం		fc_linenr;	// next line to be executed
    పూర్ణాంకం		fc_returned;	// ":return" used
    నిర్మాణం			// fixed variables for arguments
    {
	dictitem_T	var;		// variable (without room for name)
	char_u	స్థలం[VAR_SHORT_LEN];	// room for the name
    } fc_fixvar[FIXVAR_CNT];
    dict_T	fc_l_vars;	// l: local function variables
    dictitem_T	fc_l_vars_var;	// variable for l: scope
    dict_T	fc_l_avars;	// a: argument variables
    dictitem_T	fc_l_avars_var;	// variable for a: scope
    list_T	fc_l_varlist;	// list for a:000
    listitem_T	fc_l_listitems[MAX_FUNC_వాదనలు];	// listitems for a:000
    typval_T	*fc_rettv;	// return value
    linenr_T	fc_breakpoint;	// next line with breakpoint or zero
    పూర్ణాంకం		fc_dbg_tick;	// debug_tick when breakpoint was set
    పూర్ణాంకం		fc_level;	// top nesting level of executed function

    garray_T	fc_defer;	// functions to be called on return
    ectx_T	*fc_ectx;	// execution context for :def function, NULL
				// otherwise

# ifdef FEAT_PROFILE
    proftime_T	fc_prof_బిడ్డ;	// time spent in a child
# endif
    funccall_T	*fc_caller;	// calling function or NULL; or next funccal in
				// list pointed to by previous_funccal.

    // for closure
    పూర్ణాంకం		fc_refcount;	// number of user functions that reference this
				// funccal
    పూర్ణాంకం		fc_copyID;	// for garbage collection
    garray_T	fc_ufuncs;	// list of ufunc_T* which keep a reference to
				// "fc_func"
};

// structure used as item in "fc_defer"
రకనిర్వచనం నిర్మాణం
{
    char_u	*dr_name;	// function name, allocated
    typval_T	dr_argvars[MAX_FUNC_వాదనలు + 1];
    పూర్ణాంకం		dr_argcount;
} defer_T;

/*
 * Struct used by trans_function_name()
 */
రకనిర్వచనం నిర్మాణం
{
    dict_T	*fd_dict;	// Dictionary used
    char_u	*fd_newkey;	// new key in "dict" in allocated memory
    dictitem_T	*fd_di;		// Dictionary item used
} funcdict_T;

రకనిర్వచనం నిర్మాణం funccal_నమోదు funccal_నమోదు_T;
నిర్మాణం funccal_నమోదు {
    శూన్య	    *శిఖరం_funccal;
    funccal_నమోదు_T *తదుపరి;
};

// From user function to hashitem and back.
# define UF2HIKEY(fp) ((fp)->uf_name)
# define HIKEY2UF(p)  ((ufunc_T *)((p) - offsetof(ufunc_T, uf_name)))
# define HI2UF(hi)     HIKEY2UF((hi)->hi_కీలకం)

/*
 * Holds the hashtab with variables local to each sourced script.
 * Each item holds a variable (nameless) that points to the dict_T.
 */
రకనిర్వచనం నిర్మాణం {
    dictitem_T	sv_var;
    dict_T	sv_dict;
} scriptvar_T;

/*
 * Entry for "sn_all_vars".  Contains the s: variables from sn_vars plus the
 * block-local ones.
 */
రకనిర్వచనం నిర్మాణం sallvar_S sallvar_T;
నిర్మాణం sallvar_S {
    sallvar_T	*sav_తదుపరి;	  // var with same name but different block
    పూర్ణాంకం		sav_ఖండం_id;	  // block ID where declared
    పూర్ణాంకం		sav_var_vals_idx; // index in sn_var_vals

    // So long as the variable is valid (block it was defined in is still
    // active) "sav_di" is used.  It is set to NULL when leaving the block,
    // then sav_tv and sav_flags are used.
    dictitem_T *sav_di;		// dictitem with di_key and di_tv
    typval_T	sav_tv;		// type and value of the variable
    char_u	sav_flags;	// DI_FLAGS_ flags (only used for variable)
    char_u	sav_కీలకం[1];	// key (actually longer!)
};

/*
 * In the sn_all_vars hashtab item "hi_key" points to "sav_key" in a sallvar_T.
 * This makes it possible to store and find the sallvar_T.
 * SAV2HIKEY() converts a sallvar_T pointer to a hashitem key pointer.
 * HIKEY2SAV() converts a hashitem key pointer to a sallvar_T pointer.
 * HI2SAV() converts a hashitem pointer to a sallvar_T pointer.
 */
# define SAV2HIKEY(sav) ((sav)->sav_కీలకం)
# define HIKEY2SAV(p)  ((sallvar_T *)(p - offsetof(sallvar_T, sav_కీలకం)))
# define HI2SAV(hi)     HIKEY2SAV((hi)->hi_కీలకం)

# define SVFLAG_TYPE_ALLOCATED	1  // call free_type() for "sv_type"
# define SVFLAG_EXPORTED		2  // "export let var = val"
# define SVFLAG_ASSIGNED		4  // assigned a value

/*
 * Entry for "sn_var_vals".  Used for script-local variables.
 */
నిర్మాణం svar_S {
    char_u	*sv_name;	// points into "sn_all_vars" di_key
    typval_T	*sv_tv;		// points into "sn_vars" or "sn_all_vars" di_tv
    type_T	*sv_type;
    పూర్ణాంకం		sv_flags;	// SVFLAG_ values above
    పూర్ణాంకం		sv_const;	// 0, ASSIGN_CONST or ASSIGN_FINAL
};

రకనిర్వచనం నిర్మాణం {
    char_u	*imp_name;	    // name imported as (allocated)
    scid_T	imp_sid;	    // script ID of "from"
    పూర్ణాంకం		imp_flags;	    // IMP_FLAGS_ values
} imported_T;

# define IMP_FLAGS_RELOAD	2   // script reloaded, OK to redefine
# define IMP_FLAGS_AUTOLOAD	4   // script still needs to be loaded

/*
 * Info about an encountered script.
 * When sn_state has SN_STATE_NOT_LOADED, it has not been sourced yet.
 */
రకనిర్వచనం నిర్మాణం
{
    char_u	*sn_name;	    // full path of script file
    పూర్ణాంకం		sn_script_seq;	    // latest sctx_T sc_seq value

    // When non-zero the script ID of the actually sourced script.  Used if a
    // script is used by a name which has a symlink, we list both names, but
    // only the linked-to script is actually sourced.
    పూర్ణాంకం		sn_sourced_sid;

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
    పూర్ణాంకం		sn_current_ఖండం_id; // ID for current block, 0 for outer
    పూర్ణాంకం		sn_చివరి_ఖండం_id;  // Unique ID for each script block

    పూర్ణాంకం		sn_సంస్కరణ;	// :scriptversion
    పూర్ణాంకం		sn_state;	// SN_STATE_ values
    char_u	*sn_save_cpo;	// 'cpo' value when :vim9script found
    అక్షరం	sn_is_vimrc;	// .vimrc file, do not restore 'cpo'
    అక్షరం	sn_syml_checked;// flag: this has been checked for sym link

    // for a Vim9 script under "rtp/autoload/" this is "dir#scriptname#"
    char_u	*sn_autoload_ఉపసర్గ;

    // TRUE for a script used with "import autoload './dirname/script.vim'"
    // For "../autoload/script.vim" sn_autoload_prefix is also set.
    పూర్ణాంకం		sn_import_autoload;

# ifdef FEAT_PROFILE
    పూర్ణాంకం		sn_prof_on;	// TRUE when script is/was profiled
    పూర్ణాంకం		sn_pr_force;	// forceit: profile functions in this script
    proftime_T	sn_pr_బిడ్డ;	// time set when going into first child
    పూర్ణాంకం		sn_pr_nest;	// nesting for sn_pr_child
    // profiling the script as a whole
    పూర్ణాంకం		sn_pr_లెక్క;	// nr of times sourced
    proftime_T	sn_pr_మొత్తం;	// time spent in script + children
    proftime_T	sn_pr_self;	// time spent in script itself
    proftime_T	sn_pr_ప్రారంభం;	// time at script start
    proftime_T	sn_pr_children; // time in children after script start
    // profiling the script per line
    garray_T	sn_prl_ga;	// things stored for every line
    proftime_T	sn_prl_ప్రారంభం;	// start time for current line
    proftime_T	sn_prl_children; // time spent in children for this line
    proftime_T	sn_prl_wait;	// wait start time for current line
    పూర్ణాంకం		sn_prl_idx;	// index of line being timed; -1 if none
    పూర్ణాంకం		sn_prl_execed;	// line being timed was executed
# endif
} scriptitem_T;

# define SN_STATE_NEW		0   // newly loaded script, nothing done
# define SN_STATE_NOT_LOADED	1   // script located but not loaded
# define SN_STATE_RELOAD		2   // script loaded before, nothing done
# define SN_STATE_HAD_COMMAND	9   // a command was executed

// Struct passed through eval() functions.
// See EVALARG_EVALUATE for a fixed value with eval_flags set to EVAL_EVALUATE.
రకనిర్వచనం నిర్మాణం {
    పూర్ణాంకం		eval_flags;	    // EVAL_ flag values below
    పూర్ణాంకం		eval_break_లెక్క;   // nr of line breaks consumed

    // copied from exarg_T when "getline" is "getsourceline". Can be NULL.
    char_u	*(*eval_getline)(పూర్ణాంకం, శూన్య *, పూర్ణాంకం, getline_opt_T);
    శూన్య	*eval_cookie;	    // argument for eval_getline()

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
    పూర్ణాంకం		eval_using_cmdline;

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
రకనిర్వచనం నిర్మాణం sn_prl_S
{
    పూర్ణాంకం		snp_లెక్క;	// nr of times line was executed
    proftime_T	sn_prl_మొత్తం;	// time spent in a line + children
    proftime_T	sn_prl_self;	// time spent in a line itself
} sn_prl_T;

#  define PRL_అంశం(si, idx)	(((sn_prl_T *)(si)->sn_prl_ga.ga_దత్తాంశం)[(idx)])

రకనిర్వచనం నిర్మాణం {
    పూర్ణాంకం		pi_started_profiling;
    proftime_T	pi_wait_ప్రారంభం;
    proftime_T	pi_call_ప్రారంభం;
} profinfo_T;

# లేకపోతే
రకనిర్వచనం నిర్మాణం
{
    పూర్ణాంకం	    dummy;
} profinfo_T;
# endif
#లేకపోతే
// dummy typedefs for use in function prototypes
నిర్మాణం ufunc_S
{
    పూర్ణాంకం	    dummy;
};
రకనిర్వచనం నిర్మాణం
{
    పూర్ణాంకం	    dummy;
} funccall_T;
రకనిర్వచనం నిర్మాణం
{
    పూర్ణాంకం	    dummy;
} funcdict_T;
రకనిర్వచనం నిర్మాణం
{
    పూర్ణాంకం	    dummy;
} funccal_నమోదు_T;
రకనిర్వచనం నిర్మాణం
{
    పూర్ణాంకం	    dummy;
} scriptitem_T;
రకనిర్వచనం నిర్మాణం
{
    పూర్ణాంకం	    dummy;
} evalarg_T;
#అయితే_ముగింపు

// Struct passed between functions dealing with function call execution.
//
// "fe_argv_func", when not NULL, can be used to fill in arguments only when the
// invoked function uses them.  It is called like this:
//   new_argcount = fe_argv_func(current_argcount, argv, partial_argcount,
//							called_func)
//
రకనిర్వచనం నిర్మాణం {
    పూర్ణాంకం		(* fe_argv_func)(పూర్ణాంకం, typval_T *, పూర్ణాంకం, ufunc_T *);
    linenr_T	fe_firstline;	// first line of range
    linenr_T	fe_lastline;	// last line of range
    పూర్ణాంకం		*fe_doesrange;	// if not NULL: return: function handled range
    పూర్ణాంకం		fe_evaluate;	// actually evaluate expressions
    ufunc_T	*fe_ufunc;	// function to be called, when NULL lookup by
				// name
    partial_T	*fe_partial;	// for "dict" and extra arguments
    dict_T	*fe_selfdict;	// Dictionary for "self"
    object_T	*fe_object;	// object, e.g. for "this.Func()"
    typval_T	*fe_basetv;	// base for base->method()
    type_T	*fe_check_type;	// type from funcref or NULL
    పూర్ణాంకం		fe_దొరికింది_var;	// if the function is not found then give an
				// error that a variable is not callable.
    cctx_T	*fe_cctx;	// when compiling a :def function
} funcexe_T;

/*
 * Structure to hold the context of a compiled function, used by closures
 * defined in that function.
 */
రకనిర్వచనం నిర్మాణం funcstack_S funcstack_T;
నిర్మాణం funcstack_S
{
    funcstack_T *fs_తదుపరి;	// linked list at "first_funcstack"
    funcstack_T *fs_prev;

    garray_T	fs_ga;		// contains the stack, with:
				// - arguments
				// - frame
				// - local variables
    పూర్ణాంకం		fs_var_స్థానభ్రంశం;	// count of arguments + frame size == offset to
				// local variables

    పూర్ణాంకం		fs_refcount;	// nr of closures referencing this funcstack
    పూర్ణాంకం		fs_min_refcount; // nr of closures on this funcstack
    పూర్ణాంకం		fs_copyID;	// for garbage collection
};

/*
 * Structure to hold the variables declared in a loop that are possibly used
 * in a closure.
 */
రకనిర్వచనం నిర్మాణం loopvars_S loopvars_T;
నిర్మాణం loopvars_S
{
    loopvars_T *lvs_తదుపరి;	// linked list at "first_loopvars"
    loopvars_T *lvs_prev;

    garray_T	lvs_ga;		// contains the variables
    పూర్ణాంకం		lvs_refcount;	// nr of closures referencing this loopvars
    పూర్ణాంకం		lvs_min_refcount; // nr of closures on this loopvars
    పూర్ణాంకం		lvs_copyID;	// for garbage collection
};

// maximum nesting of :while and :for loops in a :def function
#నిర్వచించు MAX_LOOP_లోతు 10

రకనిర్వచనం నిర్మాణం outer_S outer_T;
నిర్మాణం outer_S {
    garray_T	*బహిర్గతం_దొంతర;	    // stack from outer scope, or a copy
				    // containing only arguments and local vars
    పూర్ణాంకం		బహిర్గతం_చట్రం_idx;	    // index of stack frame in out_stack
    outer_T	*బహిర్గతం_పైకి;	    // outer scope of outer scope or NULL
    partial_T	*బహిర్గతం_పైకి_partial;    // partial owning out_up or NULL

    నిర్మాణం {
	garray_T *దొంతర;	    // stack from outer scope, or a copy
				    // containing only vars inside the loop
	పొట్టి	 var_idx;	    // first variable defined in a loop in
				    // out_loop_stack
	పొట్టి	 var_లెక్క;	    // number of variables defined in a loop
    } బహిర్గతం_loop[MAX_LOOP_లోతు];
    పూర్ణాంకం		బహిర్గతం_loop_కొలత;	    // nr of used entries in out_loop[]
};

నిర్మాణం partial_S
{
    పూర్ణాంకం		pt_refcount;	// reference count
    పూర్ణాంకం		pt_auto;	// when TRUE the partial was created for using
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
    loopvars_T	*(pt_loopvars[MAX_LOOP_లోతు]);
				// copy of loop variables, used after loop
				// block ends

    typval_T	*pt_argv;	// arguments in allocated array
    పూర్ణాంకం		pt_argc;	// number of arguments

    పూర్ణాంకం		pt_copyID;	// funcstack may contain pointer to partial
    dict_T	*pt_dict;	// dict for "self"
    object_T	*pt_obj;	// object method
};

రకనిర్వచనం నిర్మాణం {
    పొట్టి	lvi_లోతు;	    // current nested loop depth
    నిర్మాణం {
	పొట్టి	var_idx;	    // index of first variable inside loop
	పొట్టి	var_లెక్క;	    // number of variables inside loop
    } lvi_loop[MAX_LOOP_లోతు];
} loopvarinfo_T;

రకనిర్వచనం నిర్మాణం AutoPatCmd_S AutoPatCmd_T;

/*
 * Entry in the execution stack "exestack".
 */
రకనిర్వచనం గణన {
    ETYPE_శిఖరం,		    // toplevel
    ETYPE_SCRIPT,	    // sourcing script, use es_info.sctx
    ETYPE_UFUNC,	    // user function, use es_info.ufunc
    ETYPE_AUCMD,	    // autocomand, use es_info.aucmd
    ETYPE_MODELINE,	    // modeline, use es_info.sctx
    ETYPE_EXCEPT,	    // exception, use es_info.exception
    ETYPE_వాదనలు,		    // command line argument
    ETYPE_ENV,		    // environment variable
    ETYPE_అంతర్గత,	    // internal operation
    ETYPE_SPELL,	    // loading spell file
} etype_T;

రకనిర్వచనం నిర్మాణం {
    దీర్ఘ      es_lnum;      // replaces "sourcing_lnum"
    char_u    *es_name;     // replaces "sourcing_name"
    etype_T   es_type;
    సంఘం {
	sctx_T  *sctx;      // script and modeline info
#ఒకవేళ defined(FEAT_EVAL)
	ufunc_T *ufunc;     // function info
#అయితే_ముగింపు
	AutoPatCmd_T *aucmd;  // autocommand info
	except_T   *except; // exception info
    } es_info;
#ఒకవేళ defined(FEAT_EVAL)
    sctx_T	es_save_sctx;	    // saved current_sctx when calling function
#అయితే_ముగింపు
} estack_T;

// Information returned by get_tty_info().
రకనిర్వచనం నిర్మాణం {
    పూర్ణాంకం backspace;	// what the Backspace key produces
    పూర్ణాంకం enter;		// what the Enter key produces
    పూర్ణాంకం అంతరాయం;	// interrupt character
    పూర్ణాంకం nl_does_cr;	// TRUE when a NL is expanded to CR-NL on output
} ttyinfo_T;

// Status of a job.  Order matters!
రకనిర్వచనం గణన
{
    పని_FAILED,
    పని_STARTED,
    పని_ENDED,	    // detected job done
    పని_ముగిసిన,   // job done and cleanup done
} jobstatus_T;

/*
 * Structure to hold info about a Job.
 */
నిర్మాణం jobvar_S
{
    పని_T	*jv_తదుపరి;
    పని_T	*jv_prev;
#నిర్వచితమైతే UNIX
    pid_t	jv_pid;
#అయితే_ముగింపు
#నిర్వచితమైతే MSWIN
    PROCESS_INFORMATION	jv_proc_info;
    HANDLE		jv_పని_object;
#అయితే_ముగింపు
    jobstatus_T	jv_స్థితి;
    char_u	*jv_tty_లోపల;	// controlling tty input, allocated
    char_u	*jv_tty_బహిర్గతం;	// controlling tty output, allocated
    char_u	*jv_stoponexit;	// allocated
#నిర్వచితమైతే UNIX
    char_u	*jv_termsig;	// allocated
#అయితే_ముగింపు
#నిర్వచితమైతే MSWIN
    char_u	*jv_tty_type;	// allocated
#అయితే_ముగింపు
    పూర్ణాంకం		jv_exitval;
    callback_T	jv_exit_cb;

    buf_T	*jv_లోపల_buf;	// buffer from "in-name"

    పూర్ణాంకం		jv_refcount;	// reference count
    పూర్ణాంకం		jv_copyID;

    channel_T	*jv_channel;	// channel for I/O, reference counted
    అక్షరం	**jv_argv;	// command line used to start the job
};

/*
 * Structures to hold info about a Channel.
 */
నిర్మాణం readq_S
{
    char_u	*rq_బఫరు;
    long_u	rq_buflen;
    readq_T	*rq_తదుపరి;
    readq_T	*rq_prev;
};

నిర్మాణం writeq_S
{
    garray_T	wq_ga;
    writeq_T	*wq_తదుపరి;
    writeq_T	*wq_prev;
};

నిర్మాణం jsonq_S
{
    typval_T	*jq_విలువ;
    jsonq_T	*jq_తదుపరి;
    jsonq_T	*jq_prev;
    పూర్ణాంకం		jq_no_callback; // TRUE when no callback was found
};

నిర్మాణం cbq_S
{
    callback_T	cq_callback;
    పూర్ణాంకం		cq_seq_nr;
    cbq_T	*cq_తదుపరి;
    cbq_T	*cq_prev;
};

// mode for a channel
రకనిర్వచనం గణన
{
    CH_బహుళకం_NL = 0,
    CH_బహుళకం_RAW,
    CH_బహుళకం_BLOB,
    CH_బహుళకం_JSON,
    CH_బహుళకం_JS,
    CH_బహుళకం_LSP,	// Language Server Protocol (http + json)
    CH_బహుళకం_DAP		// Debug Adapter Protocol (like LSP, but does not
			// strictly follow JSON-RPC standard)
} ch_బహుళకం_T;

రకనిర్వచనం గణన {
    JIO_PIPE,	    // default
    JIO_NULL,
    JIO_FILE,
    JIO_బఫరు,
    JIO_బహిర్గతం
} పని_io_T;

#నిర్వచించు CH_PART_FD(part)	ch_part[part].ch_fd

// Ordering matters, it is used in for loops: IN is last, only SOCK/OUT/ERR
// are polled.
రకనిర్వచనం గణన {
    PART_SOCK = 0,
#నిర్వచించు CH_SOCK_FD	CH_PART_FD(PART_SOCK)
#నిర్వచితమైతే FEAT_పని_CHANNEL
    PART_బహిర్గతం,
# define CH_బహిర్గతం_FD	CH_PART_FD(PART_బహిర్గతం)
    PART_ERR,
# define CH_ERR_FD	CH_PART_FD(PART_ERR)
    PART_లోపల,
# define CH_లోపల_FD	CH_PART_FD(PART_లోపల)
#అయితే_ముగింపు
    PART_లెక్క,
} ch_part_T;

#నిర్వచించు INVALID_FD	(-1)

// The per-fd info for a channel.
రకనిర్వచనం నిర్మాణం {
    sock_T	ch_fd;	    // socket/stdin/stdout/stderr, -1 if not used

#ఒకవేళ defined(UNIX) && !defined(HAVE_SELECT)
    పూర్ణాంకం		ch_poll_idx;	// used by channel_poll_setup()
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_GUI_X11
    Xt_ఆదానం_Id	ch_ఆదానం_Handler; // Cookie for input
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_GUI_GTK
    gint	ch_ఆదానం_Handler; // Cookie for input
#అయితే_ముగింపు

    ch_బహుళకం_T	ch_బహుళకం;
    పని_io_T	ch_io;
    పూర్ణాంకం		ch_timeout;	// request timeout in msec

    readq_T	ch_తల;	// header for circular raw read queue
    jsonq_T	ch_json_తల;	// header for circular json read queue
    garray_T	ch_ఖండం_ids;	// list of IDs that channel_read_json_block()
				// is waiting for
    // When ch_wait_len is non-zero use ch_deadline to wait for incomplete
    // message to be complete. The value is the length of the incomplete
    // message when the deadline was set.  If it gets longer (something was
    // received) the deadline is reset.
    size_t	ch_wait_len;
#నిర్వచితమైతే MSWIN
    DWORD	ch_deadline;
#లేకపోతే
    నిర్మాణం timeval ch_deadline;
#అయితే_ముగింపు
    పూర్ణాంకం		ch_ఖండం_write;	// for testing: 0 when not used, -1 when write
				// does not block, 1 simulate blocking
    పూర్ణాంకం		ch_nonblocking;	// write() is non-blocking
    writeq_T	ch_writeque;	// header for write queue

    cbq_T	ch_cb_తల;	// dummy node for per-request callbacks
    callback_T	ch_callback;	// call when a msg is not handled

    bufref_T	ch_bufref;	// buffer to read from or write to
    పూర్ణాంకం		ch_nomodifiable; // TRUE when buffer can be 'nomodifiable'
    పూర్ణాంకం		ch_nomod_error;	// TRUE when e_modifiable was given
    పూర్ణాంకం		ch_buf_append;	// write appended lines instead top-bot
    linenr_T	ch_buf_శిఖరం;	// next line to send
    linenr_T	ch_buf_bot;	// last line to send
} chanpart_T;

నిర్మాణం channel_S {
    channel_T	*ch_తదుపరి;
    channel_T	*ch_prev;

    పూర్ణాంకం		ch_id;		// ID of the channel
    పూర్ణాంకం		ch_చివరి_సందేశం_id;	// ID of the last message

    chanpart_T	ch_part[PART_లెక్క]; // info for socket, out, err and in
    పూర్ణాంకం		ch_write_పాఠ్యం_బహుళకం; // write buffer lines with CR, not NL

    అక్షరం	*ch_hostname;	// only for socket, allocated
    పూర్ణాంకం		ch_పోర్టు;	// only for socket

    పూర్ణాంకం		ch_గమ్యం_be_closed; // bitset of readable fds to be closed.
				 // When all readable fds have been closed,
				 // set to (1 << PART_COUNT).
    పూర్ణాంకం		ch_గమ్యం_be_freed; // When TRUE channel must be freed when it's
				// safe to invoke callbacks.
    పూర్ణాంకం		ch_error;	// When TRUE an error was reported.  Avoids
    // giving pages full of error messages when
    // the other side has exited, only mention the
    // first error until the connection works
    // again.
    పూర్ణాంకం		ch_listen;	// When TRUE channel is listen socket.

    శూన్య	(*ch_nb_close_cb)(శూన్య);
				// callback for Netbeans when channel is
				// closed
#నిర్వచితమైతే FEAT_SOCKETSERVER
    bool	ch_socketserver; // If channel is used by socketserver
    శూన్య	(*ch_ss_close_cb)(channel_T *);
    శూన్య	(*ch_ss_accept_cb)(channel_T *);
    channel_T	*ch_ss_తదుపరి;
    channel_T	*ch_ss_prev;
#అయితే_ముగింపు

#నిర్వచితమైతే MSWIN
    పూర్ణాంకం		ch_named_pipe;	// using named pipe instead of pty
#అయితే_ముగింపు
    callback_T	ch_callback;	// call when any msg is not handled
    callback_T	ch_close_cb;	// call when channel is closed
    పూర్ణాంకం		ch_drop_never;
    పూర్ణాంకం		ch_keep_open;	// do not close on read error
    పూర్ణాంకం		ch_nonblock;

    పని_T	*ch_పని;	// Job that uses this channel; this does not
				// count as a reference to avoid a circular
				// reference, the job refers to the channel.
    పూర్ణాంకం		ch_పని_killed;	// TRUE when there was a job and it was killed
				// or we know it died.
    పూర్ణాంకం		ch_anonymous_pipe;  // Indicates that anonymous pipes are being
				    // used for communication in the Windows
				    // ConPTY terminal.
    పూర్ణాంకం		ch_killing;	    // Indicates that the job associated with
				    // the channel is terminating.  It becomes
				    // TRUE when TerminateJobObject() was
				    // called or the process associated with
				    // the job had exited (only ConPTY).

    పూర్ణాంకం		ch_refcount;	// reference count
    పూర్ణాంకం		ch_copyID;
};

#నిర్వచించు JO_బహుళకం		    0x0001	// channel mode
#నిర్వచించు JO_లోపల_బహుళకం	    0x0002	// stdin mode
#నిర్వచించు JO_బహిర్గతం_బహుళకం	    0x0004	// stdout mode
#నిర్వచించు JO_ERR_బహుళకం	    0x0008	// stderr mode
#నిర్వచించు JO_CALLBACK	    0x0010	// channel callback
#నిర్వచించు JO_బహిర్గతం_CALLBACK	    0x0020	// stdout callback
#నిర్వచించు JO_ERR_CALLBACK	    0x0040	// stderr callback
#నిర్వచించు JO_CLOSE_CALLBACK   0x0080	// "close_cb"
#నిర్వచించు JO_WAITTIME	    0x0100	// only for ch_open()
#నిర్వచించు JO_TIMEOUT	    0x0200	// all timeouts
#నిర్వచించు JO_బహిర్గతం_TIMEOUT	    0x0400	// stdout timeouts
#నిర్వచించు JO_ERR_TIMEOUT	    0x0800	// stderr timeouts
#నిర్వచించు JO_PART		    0x1000	// "part"
#నిర్వచించు JO_ID		    0x2000	// "id"
#నిర్వచించు JO_STOPONEXIT	    0x4000	// "stoponexit"
#నిర్వచించు JO_EXIT_CB	    0x8000	// "exit_cb"
#నిర్వచించు JO_బహిర్గతం_IO	    0x10000	// "out_io"
#నిర్వచించు JO_ERR_IO	    0x20000	// "err_io" (JO_OUT_IO << 1)
#నిర్వచించు JO_లోపల_IO	    0x40000	// "in_io" (JO_OUT_IO << 2)
#నిర్వచించు JO_బహిర్గతం_NAME	    0x80000	// "out_name"
#నిర్వచించు JO_ERR_NAME	    0x100000	// "err_name" (JO_OUT_NAME << 1)
#నిర్వచించు JO_లోపల_NAME	    0x200000	// "in_name" (JO_OUT_NAME << 2)
#నిర్వచించు JO_లోపల_శిఖరం	    0x400000	// "in_top"
#నిర్వచించు JO_లోపల_BOT	    0x800000	// "in_bot"
#నిర్వచించు JO_బహిర్గతం_BUF	    0x1000000	// "out_buf"
#నిర్వచించు JO_ERR_BUF	    0x2000000	// "err_buf" (JO_OUT_BUF << 1)
#నిర్వచించు JO_లోపల_BUF	    0x4000000	// "in_buf" (JO_OUT_BUF << 2)
#నిర్వచించు JO_CHANNEL	    0x8000000	// "channel"
#నిర్వచించు JO_ఖండం_WRITE	    0x10000000	// "block_write"
#నిర్వచించు JO_బహిర్గతం_MODIFIABLE   0x20000000	// "out_modifiable"
#నిర్వచించు JO_ERR_MODIFIABLE   0x40000000	// "err_modifiable" (JO_OUT_ << 1)
#నిర్వచించు JO_ALL		    0x7fffffff

#నిర్వచించు JO2_బహిర్గతం_సందేశం	    0x0001	// "out_msg"
#నిర్వచించు JO2_ERR_సందేశం	    0x0002	// "err_msg" (JO_OUT_ << 1)
#నిర్వచించు JO2_పదం_NAME	    0x0004	// "term_name"
#నిర్వచించు JO2_పదం_ముగింపు	    0x0008	// "term_finish"
#నిర్వచించు JO2_ENV		    0x0010	// "env"
#నిర్వచించు JO2_CWD		    0x0020	// "cwd"
#నిర్వచించు JO2_పదం_అడ్డువరుసలు	    0x0040	// "term_rows"
#నిర్వచించు JO2_పదం_నిలువుల_సంఖ్య	    0x0080	// "term_cols"
#నిర్వచించు JO2_VERTICAL	    0x0100	// "vertical"
#నిర్వచించు JO2_CURWIN	    0x0200	// "curwin"
#నిర్వచించు JO2_HIDDEN	    0x0400	// "hidden"
#నిర్వచించు JO2_పదం_OPENCMD    0x0800	// "term_opencmd"
#నిర్వచించు JO2_EOF_CHARS	    0x1000	// "eof_chars"
#నిర్వచించు JO2_NORESTORE	    0x2000	// "norestore"
#నిర్వచించు JO2_పదం_KILL	    0x4000	// "term_kill"
#నిర్వచించు JO2_ANSI_COLORS	    0x8000	// "ansi_colors"
#నిర్వచించు JO2_TTY_TYPE	    0x10000	// "tty_type"
#నిర్వచించు JO2_BUFNR	    0x20000	// "bufnr"
#నిర్వచించు JO2_పదం_API	    0x40000	// "term_api"
#నిర్వచించు JO2_పదం_HIGHLIGHT  0x80000	// "highlight"

#నిర్వచించు JO_బహుళకం_ALL	(JO_బహుళకం + JO_లోపల_బహుళకం + JO_బహిర్గతం_బహుళకం + JO_ERR_బహుళకం)
#నిర్వచించు JO_CB_ALL \
    (JO_CALLBACK + JO_బహిర్గతం_CALLBACK + JO_ERR_CALLBACK + JO_CLOSE_CALLBACK)
#నిర్వచించు JO_TIMEOUT_ALL	(JO_TIMEOUT + JO_బహిర్గతం_TIMEOUT + JO_ERR_TIMEOUT)

/*
 * Options for job and channel commands.
 */
రకనిర్వచనం నిర్మాణం
{
    పూర్ణాంకం		jo_సమితి;		// JO_ bits for values that were set
    పూర్ణాంకం		jo_set2;	// JO2_ bits for values that were set

    ch_బహుళకం_T	jo_బహుళకం;
    ch_బహుళకం_T	jo_లోపల_బహుళకం;
    ch_బహుళకం_T	jo_బహిర్గతం_బహుళకం;
    ch_బహుళకం_T	jo_err_బహుళకం;
    పూర్ణాంకం		jo_noblock;

    పని_io_T	jo_io[4];	// PART_OUT, PART_ERR, PART_IN
    char_u	jo_io_name_buf[4][NUMBUFLEN];
    char_u	*jo_io_name[4];	// not allocated!
    పూర్ణాంకం		jo_io_buf[4];
    పూర్ణాంకం		jo_pty;
    పూర్ణాంకం		jo_modifiable[4];
    పూర్ణాంకం		jo_సందేశం[4];
    channel_T	*jo_channel;

    linenr_T	jo_లోపల_శిఖరం;
    linenr_T	jo_లోపల_bot;

    callback_T	jo_callback;
    callback_T	jo_బహిర్గతం_cb;
    callback_T	jo_err_cb;
    callback_T	jo_close_cb;
    callback_T	jo_exit_cb;
    పూర్ణాంకం		jo_drop_never;
    పూర్ణాంకం		jo_waittime;
    పూర్ణాంకం		jo_timeout;
    పూర్ణాంకం		jo_బహిర్గతం_timeout;
    పూర్ణాంకం		jo_err_timeout;
    పూర్ణాంకం		jo_ఖండం_write;	// for testing only
    పూర్ణాంకం		jo_part;
    పూర్ణాంకం		jo_id;
    char_u	jo_stoponexit_buf[NUMBUFLEN];
    char_u	*jo_stoponexit;
    dict_T	*jo_env;	// environment variables
    char_u	jo_cwd_buf[NUMBUFLEN];
    char_u	*jo_cwd;

#నిర్వచితమైతే FEAT_TERMINAL
    // when non-zero run the job in a terminal window of this size
    పూర్ణాంకం		jo_పదం_అడ్డువరుసలు;
    పూర్ణాంకం		jo_పదం_నిలువుల_సంఖ్య;
    పూర్ణాంకం		jo_vertical;
    పూర్ణాంకం		jo_curwin;
    buf_T	*jo_bufnr_buf;
    పూర్ణాంకం		jo_hidden;
    పూర్ణాంకం		jo_పదం_norestore;
    char_u	jo_పదం_name_buf[NUMBUFLEN];
    char_u	*jo_పదం_name;
    char_u	jo_పదం_opencmd_buf[NUMBUFLEN];
    char_u	*jo_పదం_opencmd;
    పూర్ణాంకం		jo_పదం_ముగింపు;
    char_u	jo_eof_chars_buf[NUMBUFLEN];
    char_u	*jo_eof_chars;
    char_u	jo_పదం_kill_buf[NUMBUFLEN];
    char_u	*jo_పదం_kill;
# ఒకవేళ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    long_u	jo_ansi_colors[16];
# endif
    char_u	jo_పదం_highlight_buf[NUMBUFLEN];
    char_u	*jo_పదం_highlight;
    పూర్ణాంకం		jo_tty_type;	    // first character of "tty_type"
    char_u	jo_పదం_api_buf[NUMBUFLEN];
    char_u	*jo_పదం_api;
#అయితే_ముగింపు
} jobopt_T;

#నిర్వచితమైతే FEAT_EVAL
/*
 * Structure used for listeners added with listener_add().
 */
రకనిర్వచనం నిర్మాణం listener_S listener_T;
నిర్మాణం listener_S
{
    listener_T	*lr_తదుపరి;
    పూర్ణాంకం		lr_id;
    bool	lr_పాఠ్యం;	// include the resulting text in each change
    callback_T	lr_callback;
};

// Structure used for listeners added with redraw_listener_add().
రకనిర్వచనం నిర్మాణం redraw_listener_S redraw_listener_T;
నిర్మాణం redraw_listener_S
{
    redraw_listener_T	*rl_తదుపరి;
    పూర్ణాంకం			rl_id;
    నిర్మాణం
    {
	callback_T	on_ప్రారంభం;
	callback_T	on_చివర;
    }			rl_callbacks;
};
#అయితే_ముగింపు

/*
 * structure used for explicit stack while garbage collecting hash tables
 */
రకనిర్వచనం నిర్మాణం ht_దొంతర_S
{
    hashtab_T		*ht;
    నిర్మాణం ht_దొంతర_S	*prev;
} ht_దొంతర_T;

/*
 * structure used for explicit stack while garbage collecting lists
 */
రకనిర్వచనం నిర్మాణం list_దొంతర_S
{
    list_T		*list;
    నిర్మాణం list_దొంతర_S	*prev;
} list_దొంతర_T;

/*
 * structure used for explicit stack while garbage collecting tuples
 */
రకనిర్వచనం నిర్మాణం tuple_దొంతర_S
{
    tuple_T			*tuple;
    నిర్మాణం tuple_దొంతర_S	*prev;
} tuple_దొంతర_T;

/*
 * Structure used for iterating over dictionary items.
 * Initialize with dict_iterate_start().
 */
రకనిర్వచనం నిర్మాణం
{
    long_u	dit_todo;
    hashitem_T	*dit_hi;
} dict_iterator_T;

// values for b_syn_spell: what to do with toplevel text
#నిర్వచించు SYNSPL_DEFAULT	0	// spell check if @Spell not defined
#నిర్వచించు SYNSPL_శిఖరం	1	// spell check toplevel text
#నిర్వచించు SYNSPL_NOTOP	2	// don't spell check toplevel text

// values for b_syn_foldlevel: how to compute foldlevel on a line
#నిర్వచించు SYNFLD_ప్రారంభం	0	// use level of item at start of line
#నిర్వచించు SYNFLD_MINIMUM	1	// use lowest local minimum level on line

// avoid #ifdefs for when b_spell is not available
#నిర్వచితమైతే FEAT_SPELL
# define B_SPELL(buf)  ((buf)->b_spell)
#లేకపోతే
# define B_SPELL(buf)  (0)
#అయితే_ముగింపు

రకనిర్వచనం నిర్మాణం qf_info_S qf_info_T;

#నిర్వచితమైతే FEAT_PROFILE
/*
 * Used for :syntime: timing of executing a syntax pattern.
 */
రకనిర్వచనం నిర్మాణం {
    proftime_T	మొత్తం;		// total time used
    proftime_T	slowest;	// time of slowest call
    దీర్ఘ	లెక్క;		// nr of times used
    దీర్ఘ	సరిపోలిక;		// nr of times matched
} syn_సమయం_T;
#అయితే_ముగింపు

రకనిర్వచనం నిర్మాణం timer_S timer_T;
నిర్మాణం timer_S
{
    దీర్ఘ	tr_id;
#నిర్వచితమైతే FEAT_TIMERS
    timer_T	*tr_తదుపరి;
    timer_T	*tr_prev;
    proftime_T	tr_due;		    // when the callback is to be invoked
    అక్షరం	tr_firing;	    // when TRUE callback is being called
    అక్షరం	tr_paused;	    // when TRUE callback is not invoked
    అక్షరం	tr_keep;	    // when TRUE keep timer after it fired
    పూర్ణాంకం		tr_repeat;	    // number of times to repeat, -1 forever
    దీర్ఘ	tr_interval;	    // msec
    callback_T	tr_callback;
    పూర్ణాంకం		tr_emsg_లెక్క;
#అయితే_ముగింపు
};

#నిర్వచితమైతే FEAT_CRYPT
/*
 * Structure to hold the type of encryption and the state of encryption or
 * decryption.
 */
రకనిర్వచనం నిర్మాణం {
    పూర్ణాంకం	    పద్ధతి_nr;
    శూన్య    *పద్ధతి_state;  // method-specific state information
} cryptstate_T;

// values for method_nr
# define CRYPT_M_ZIP	0
# define CRYPT_M_BF	1
# define CRYPT_M_BF2	2
# define CRYPT_M_SOD    3
# define CRYPT_M_SOD2   4
# define CRYPT_M_లెక్క	5 // number of crypt methods

// Currently all crypt methods work inplace.  If one is added that isn't then
// define this.
# define CRYPT_NOT_INPLACE 1

// Struct for passing arguments down to the crypt_init functions
రకనిర్వచనం నిర్మాణం {
    char_u	*cat_salt;
    పూర్ణాంకం		cat_salt_len;
    char_u	*cat_విత్తనం;
    పూర్ణాంకం		cat_విత్తనం_len;
    char_u	*cat_కూడు;
    పూర్ణాంకం		cat_కూడు_len;
    పూర్ణాంకం		cat_init_from_file;
} crypt_arg_T;

#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_PROP_POPUP
రకనిర్వచనం గణన {
    POPPOS_BOTLEFT,
    POPPOS_TOPLEFT,
    POPPOS_BOTRIGHT,
    POPPOS_TOPRIGHT,
    POPPOS_CENTER,
    POPPOS_BOTTOM,	// bottom of popup just above the command line
    POPPOS_NONE
} poppos_T;

రకనిర్వచనం గణన {
    POPCLOSE_NONE,
    POPCLOSE_BUTTON,
    POPCLOSE_CLICK
} popclose_T;

# define POPUPWIN_DEFAULT_ZINDEX	 50
# define POPUPMENU_ZINDEX		100
# define POPUPWIN_DIALOG_ZINDEX		200
# define POPUPWIN_NOTIFICATION_ZINDEX   300
#అయితే_ముగింపు

/*
 * These are items normally related to a buffer.  But when using ":ownsyntax"
 * a window may have its own instance.
 */
రకనిర్వచనం నిర్మాణం {
#నిర్వచితమైతే FEAT_SYN_HL
    hashtab_T	b_keywtab;		// syntax keywords hash table
    hashtab_T	b_keywtab_ic;		// idem, ignore case
    పూర్ణాంకం		b_syn_error;		// TRUE when error occurred in HL
# ifdef FEAT_RELTIME
    పూర్ణాంకం		b_syn_slow;		// TRUE when 'redrawtime' reached
# endif
    పూర్ణాంకం		b_syn_ic;		// ignore case for :syn cmds
    పూర్ణాంకం		b_syn_foldlevel;	// how to compute foldlevel on a line
    పూర్ణాంకం		b_syn_spell;		// SYNSPL_ values
    garray_T	b_syn_patterns;		// table for syntax patterns
    garray_T	b_syn_clusters;		// table for syntax clusters
    పూర్ణాంకం		b_spell_cluster_id;	// @Spell cluster ID or 0
    పూర్ణాంకం		b_nospell_cluster_id;	// @NoSpell cluster ID or 0
    పూర్ణాంకం		b_syn_containedin;	// TRUE when there is an item with a
					// "containedin" argument
    పూర్ణాంకం		b_syn_sync_flags;	// flags about how to sync
    పొట్టి	b_syn_sync_id;		// group to sync on
    దీర్ఘ	b_syn_sync_minlines;	// minimal sync lines offset
    దీర్ఘ	b_syn_sync_maxlines;	// maximal sync lines offset
    దీర్ఘ	b_syn_sync_linebreaks;	// offset for multi-line pattern
    char_u	*b_syn_linecont_pat;	// line continuation pattern
    regprog_T	*b_syn_linecont_prog;	// line continuation program
# ifdef FEAT_PROFILE
    syn_సమయం_T  b_syn_linecont_సమయం;
# endif
    పూర్ణాంకం		b_syn_linecont_ic;	// ignore-case flag for above
    పూర్ణాంకం		b_syn_topgrp;		// for ":syntax include"
# ifdef FEAT_CONCEAL
    పూర్ణాంకం		b_syn_conceal;		// auto-conceal for :syn cmds
# endif
# ifdef FEAT_FOLDING
    పూర్ణాంకం		b_syn_folditems;	// number of patterns with the HL_FOLD
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
    పూర్ణాంకం		b_sst_len;
    synstate_T	*b_sst_మొదటి;
    synstate_T	*b_sst_శోధన;
    synstate_T	*b_sst_firstfree;
    పూర్ణాంకం		b_sst_freecount;
    linenr_T	b_sst_check_lnum;
    short_u	b_sst_lasttick;	// last display tick

    // Cache for in_id_list(); see idl_cache_T in syntax.c.
    శూన్య	*b_idlist_cache;
#అయితే_ముగింపు // FEAT_SYN_HL

#నిర్వచితమైతే FEAT_SPELL
    // for spell checking
    garray_T	b_langp;	    // list of pointers to slang_T, see spell.c
    char_u	b_spell_ismw[256];  // flags: is midword char
    char_u	*b_spell_ismw_mb;   // multi-byte midword chars
    char_u	*b_p_spc;	    // 'spellcapcheck'
    regprog_T	*b_cap_prog;	    // program for 'spellcapcheck'
    char_u	*b_p_spf;	    // 'spellfile'
    char_u	*b_p_spl;	    // 'spelllang'
    char_u	*b_p_spo;	    // 'spelloptions'
    పూర్ణాంకం		b_cjk;		    // all CJK letters as OK
#అయితే_ముగింపు
#ఒకవేళ !defined(FEAT_SYN_HL) && !defined(FEAT_SPELL)
    పూర్ణాంకం		dummy;
#అయితే_ముగింపు
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

నిర్మాణం file_బఫరు
{
    memline_T	b_ml;		// associated memline (also contains line
				// count)

    buf_T	*b_తదుపరి;	// links in list of buffers
    buf_T	*b_prev;

    పూర్ణాంకం		b_nwindows;	// nr of windows open on this buffer

    పూర్ణాంకం		b_flags;	// various BF_ flags
    పూర్ణాంకం		b_locked;	// Buffer is being closed or referenced, don't
				// let autocommands wipe it out.
    పూర్ణాంకం		b_locked_విభజించు;	// Buffer is being closed, don't allow opening
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

#నిర్వచితమైతే UNIX
    bool	b_dev_valid;	// true when b_dev has a valid number
    dev_t	b_dev;		// device number
    ino_t	b_ino;		// inode number
#అయితే_ముగింపు
#నిర్వచితమైతే VMS
    అక్షరం	 b_fab_rfm;	// Record format
    అక్షరం	 b_fab_rat;	// Record attribute
    గుర్తులేని పూర్ణాంకం b_fab_mrs;	// Max record size
#అయితే_ముగింపు
    పూర్ణాంకం		b_fnum;		// buffer number for this file.
    char_u	b_కీలకం[VIM_SIZEOF_INT * 2 + 1];
				// key used for buf_hashtab, holds b_fnum as
				// hex string

    పూర్ణాంకం		b_మారింది;	// 'modified': Set to TRUE if something in the
				// file has been changed and not written out.
    dictitem16_T b_ct_di;	// holds the b:changedtick value in
				// b_ct_di.di_tv.vval.v_number;
				// incremented for each change, also for undo
#నిర్వచించు CHANGEDTICK(buf) ((buf)->b_ct_di.di_tv.vval.v_సంఖ్య)

    varnumber_T	b_చివరి_changedtick;	// b:changedtick when TextChanged was
					// last triggered.
    varnumber_T	b_చివరి_changedtick_pum; // b:changedtick for TextChangedP
    varnumber_T	b_చివరి_changedtick_i;   // b:changedtick for TextChangedI

    bool	b_saving;	// Set to true if we are in the middle of
				// saving the buffer.

    /*
     * Changes to a buffer require updating of the display.  To minimize the
     * work, remember changes made and update everything at once.
     */
    bool	b_mod_సమితి;	// true when there are changes since the last
				// time the display was updated
    linenr_T	b_mod_శిఖరం;	// topmost lnum that was changed
    linenr_T	b_mod_bot;	// lnum below last changed line, AFTER the
				// change
    దీర్ఘ	b_mod_xlines;	// number of extra buffer lines inserted;
				// negative when lines were deleted

    wininfo_T	*b_wininfo;	// list of last used info for each window

    దీర్ఘ	b_mtime;	// last change time of original file
    దీర్ఘ	b_mtime_ns;	// nanoseconds of last change time
    దీర్ఘ	b_mtime_read;	// last change time when reading
    దీర్ఘ	b_mtime_read_ns;  // nanoseconds of last read time
    off_T	b_orig_కొలత;	// size of original file in bytes
    పూర్ణాంకం		b_orig_బహుళకం;	// mode of original file
#నిర్వచితమైతే FEAT_VIMINFO
    సమయం_T	b_చివరి_వాడిన;	// time when the buffer was last used; used
				// for viminfo
#అయితే_ముగింపు

    pos_T	b_namedm[NMARKS]; // current named marks (mark.c)

    // These variables are set when VIsual_active becomes FALSE
    visualinfo_T b_visual;
#నిర్వచితమైతే FEAT_EVAL
    పూర్ణాంకం		b_visual_బహుళకం_eval;  // b_visual.vi_mode for visualmode()
#అయితే_ముగింపు

    pos_T	b_చివరి_cursor;	// cursor position when last unloading this
				//బఫరు
    pos_T	b_చివరి_చొప్పించు;	// where Insert mode was left
    pos_T	b_చివరి_change;	// position of last change: '. mark

    /*
     * the changelist contains old change positions
     */
    pos_T	b_changelist[JUMPLISTSIZE];
    పూర్ణాంకం		b_changelistlen;	// number of active entries
    bool	b_new_change;		// set in u_savecommon()

    /*
     * Character table, only used in charset.c for 'iskeyword'
     * 32 bytes of 8 bits: 1 bit per character 0-255.
     */
    char_u	b_chartab[32];

    // Table used for mappings local to a buffer.
    mapblock_T	*(b_maphash[256]);

    // First abbreviation local to a buffer.
    mapblock_T	*b_మొదటి_abbr;

    // User commands local to the buffer.
    garray_T	b_ucmds;
    // start and end of an operator, also used for '[ and ']
    pos_T	b_op_ప్రారంభం;
    pos_T	b_op_ప్రారంభం_orig;  // used for Insstart_orig
    pos_T	b_op_చివర;

#నిర్వచితమైతే FEAT_VIMINFO
    bool	b_marks_read;	// Have we read viminfo marks yet?
#అయితే_ముగింపు

    bool	b_modified_was_సమితి;	// did ":set modified"
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
    u_శీర్షిక_T	*b_u_oldhead;	// pointer to oldest header
    u_శీర్షిక_T	*b_u_newhead;	// pointer to newest header; may not be valid
				// if b_u_curhead is not NULL
    u_శీర్షిక_T	*b_u_curhead;	// pointer to current header
    పూర్ణాంకం		b_u_numhead;	// current number of headers
    bool	b_u_synced;	// entry lists are synced
    దీర్ఘ	b_u_seq_చివరి;	// last used undo sequence number
    దీర్ఘ	b_u_save_nr_చివరి; // counter for last file write
    దీర్ఘ	b_u_seq_cur;	// uh_seq of header below which we are now
    సమయం_T	b_u_సమయం_cur;	// uh_time of header below which we are now
    దీర్ఘ	b_u_save_nr_cur; // file write nr after which we are now

    /*
     * variables for "U" command in undo.c
     */
    undoline_T	b_u_పంక్తి_ptr;	// saved line for "U" command
    linenr_T	b_u_పంక్తి_lnum;	// line number of line in u_line
    colnr_T	b_u_పంక్తి_colnr;	// optional column number

    bool	b_scanned;	// ^N/^P have scanned this buffer

    // flags for use of ":lmap" and IM control
    దీర్ఘ	b_p_iminsert;	// input mode for insert
    దీర్ఘ	b_p_imsearch;	// input mode for search
#నిర్వచించు B_IMODE_USE_చొప్పించు (-1)	//	Use b_p_iminsert value for search
#నిర్వచించు B_IMODE_NONE 0		//	Input via none
#నిర్వచించు B_IMODE_LMAP 1		//	Input via langmap
#నిర్వచించు B_IMODE_IM 2		//	Input via input method
#నిర్వచించు B_IMODE_చివరి 2

#నిర్వచితమైతే FEAT_KEYMAP
    పొట్టి	b_kmap_state;	// using "lmap" mappings
# define KEYMAP_INIT	1	// 'keymap' was set, call keymap_init()
# define KEYMAP_LOADED	2	// 'keymap' mappings have been loaded
    garray_T	b_kmap_ga;	// the keymap table
#అయితే_ముగింపు

    /*
     * Options local to a buffer.
     * They are here because their value depends on the type of file
     * or contents of the file being edited.
     */
    bool	b_p_initialized;	// set when options initialized

#నిర్వచితమైతే FEAT_EVAL
    sctx_T	b_p_script_ctx[BV_లెక్క]; // SCTXs for buffer-local options
#అయితే_ముగింపు

    పూర్ణాంకం		b_p_ac;		// 'autocomplete'
    పూర్ణాంకం		b_p_ai;		// 'autoindent'
    పూర్ణాంకం		b_p_ai_nopaste;	// b_p_ai saved for paste mode
    char_u	*b_p_bkc;	// 'backupcopy'
    గుర్తులేని	b_bkc_flags;    // flags for 'backupcopy'
    పూర్ణాంకం		b_p_ci;		// 'copyindent'
    పూర్ణాంకం		b_p_bin;	// 'binary'
    పూర్ణాంకం		b_p_bomb;	// 'bomb'
    char_u	*b_p_bh;	// 'bufhidden'
    char_u	*b_p_bt;	// 'buftype'
#నిర్వచితమైతే FEAT_QUICKFIX
# define BUF_HAS_QF_నమోదు 1
# define BUF_HAS_LL_నమోదు 2
    పూర్ణాంకం		b_has_qf_నమోదు;
#అయితే_ముగింపు
    పూర్ణాంకం		b_p_bl;		// 'buflisted'
    పూర్ణాంకం		b_p_cin;	// 'cindent'
    char_u	*b_p_cino;	// 'cinoptions'
    char_u	*b_p_cink;	// 'cinkeys'
    char_u	*b_p_cinsd;	// 'cinscopedecls'
    char_u	*b_p_cinw;	// 'cinwords'
    char_u	*b_p_com;	// 'comments'
    char_u	*b_p_cms;	// 'commentstring'
    char_u	*b_p_cot;	// 'completeopt' local value
    గుర్తులేని	b_cot_flags;	// flags for 'completeopt'
    char_u	*b_p_cpt;	// 'complete'
#నిర్వచితమైతే BACKSLASH_లోపల_FILENAME
    char_u	*b_p_csl;	// 'completeslash'
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_COMPL_FUNC
    long_u	b_p_cpt_flags;	// flags for 'complete'
    callback_T	*b_p_cpt_cb;	// F{func} in 'complete' callback
    పూర్ణాంకం		b_p_cpt_లెక్క;	// Count of values in 'complete'
    char_u	*b_p_cfu;	// 'completefunc'
    callback_T	b_cfu_cb;	// 'completefunc' callback
    char_u	*b_p_ofu;	// 'omnifunc'
    callback_T	b_ofu_cb;	// 'omnifunc' callback
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_EVAL
    char_u	*b_p_tfu;	// 'tagfunc' option value
    callback_T	b_tfu_cb;	// 'tagfunc' callback
    char_u	*b_p_ffu;	// 'findfunc' option value
    callback_T	b_ffu_cb;	// 'findfunc' callback
#అయితే_ముగింపు
    పూర్ణాంకం		b_p_eof;	// 'endoffile'
    పూర్ణాంకం		b_p_eol;	// 'endofline'
    పూర్ణాంకం		b_p_fixeol;	// 'fixendofline'
    పూర్ణాంకం		b_p_et;		// 'expandtab'
    పూర్ణాంకం		b_p_et_nobin;	// b_p_et saved for binary mode
    పూర్ణాంకం		b_p_et_nopaste; // b_p_et saved for paste mode
    char_u	*b_p_fenc;	// 'fileencoding'
    char_u	*b_p_ff;	// 'fileformat'
    char_u	*b_p_ft;	// 'filetype'
    char_u	*b_p_fo;	// 'formatoptions'
    char_u	*b_p_flp;	// 'formatlistpat'
    పూర్ణాంకం		b_p_inf;	// 'infercase'
    char_u	*b_p_isk;	// 'iskeyword'
#నిర్వచితమైతే FEAT_కనుగొను_ID
    char_u	*b_p_def;	// 'define' local value
    char_u	*b_p_inc;	// 'include'
# ifdef FEAT_EVAL
    char_u	*b_p_inex;	// 'includeexpr'
    long_u	b_p_inex_flags;	// flags for 'includeexpr'
# endif
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_EVAL)
    char_u	*b_p_inde;	// 'indentexpr'
    long_u	b_p_inde_flags;	// flags for 'indentexpr'
    char_u	*b_p_indk;	// 'indentkeys'
#అయితే_ముగింపు
    char_u	*b_p_fp;	// 'formatprg'
#ఒకవేళ defined(FEAT_EVAL)
    char_u	*b_p_fex;	// 'formatexpr'
    long_u	b_p_fex_flags;	// flags for 'formatexpr'
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_FSYNC
    పూర్ణాంకం		b_p_fs;		// 'fsync'
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_CRYPT
    char_u	*b_p_కీలకం;	// 'key'
#అయితే_ముగింపు
    char_u	*b_p_kp;	// 'keywordprg'
    పూర్ణాంకం		b_p_lisp;	// 'lisp'
    char_u	*b_p_lop;	// 'lispoptions'
    char_u	*b_p_menc;	// 'makeencoding'
    char_u	*b_p_mps;	// 'matchpairs'
    పూర్ణాంకం		b_p_ml;		// 'modeline'
    పూర్ణాంకం		b_p_ml_nobin;	// b_p_ml saved for binary mode
    పూర్ణాంకం		b_p_ma;		// 'modifiable'
    char_u	*b_p_nf;	// 'nrformats'
    పూర్ణాంకం		b_p_pi;		// 'preserveindent'
    char_u	*b_p_qe;	// 'quoteescape'
    పూర్ణాంకం		b_p_ro;		// 'readonly'
    దీర్ఘ	b_p_sw;		// 'shiftwidth'
    పూర్ణాంకం		b_p_sn;		// 'shortname'
    పూర్ణాంకం		b_p_si;		// 'smartindent'
    దీర్ఘ	b_p_sts;	// 'softtabstop'
    దీర్ఘ	b_p_sts_nopaste; // b_p_sts saved for paste mode
    char_u	*b_p_sua;	// 'suffixesadd'
    పూర్ణాంకం		b_p_swf;	// 'swapfile'
#నిర్వచితమైతే FEAT_SYN_HL
    దీర్ఘ	b_p_smc;	// 'synmaxcol'
    char_u	*b_p_syn;	// 'syntax'
#అయితే_ముగింపు
    దీర్ఘ	b_p_ts;		// 'tabstop'
    పూర్ణాంకం		b_p_tx;		// 'textmode'
    దీర్ఘ	b_p_tw;		// 'textwidth'
    దీర్ఘ	b_p_tw_nobin;	// b_p_tw saved for binary mode
    దీర్ఘ	b_p_tw_nopaste;	// b_p_tw saved for paste mode
    దీర్ఘ	b_p_wm;		// 'wrapmargin'
    దీర్ఘ	b_p_wm_nobin;	// b_p_wm saved for binary mode
    దీర్ఘ	b_p_wm_nopaste;	// b_p_wm saved for paste mode
#నిర్వచితమైతే FEAT_VARTABS
    char_u	*b_p_vsts;	// 'varsofttabstop'
    పూర్ణాంకం		*b_p_vsts_array;   // 'varsofttabstop' in internal format
    char_u	*b_p_vsts_nopaste; // b_p_vsts saved for paste mode
    char_u	*b_p_vts;	// 'vartabstop'
    పూర్ణాంకం		*b_p_vts_array;	// 'vartabstop' in internal format
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_KEYMAP
    char_u	*b_p_keymap;	// 'keymap'
#అయితే_ముగింపు

    /*
     * local values for options which are normally global
     */
#నిర్వచితమైతే FEAT_QUICKFIX
    char_u	*b_p_gefm;	// 'grepformat' local value
    char_u	*b_p_gp;	// 'grepprg' local value
    char_u	*b_p_mp;	// 'makeprg' local value
    char_u	*b_p_efm;	// 'errorformat' local value
#అయితే_ముగింపు
    char_u	*b_p_ep;	// 'equalprg' local value
    char_u	*b_p_మార్గం;	// 'path' local value
    పూర్ణాంకం		b_p_ar;		// 'autoread' local value
    char_u	*b_p_ట్యాగులు;	// 'tags' local value
    char_u	*b_p_tc;	// 'tagcase' local value
    గుర్తులేని	b_tc_flags;     // flags for 'tagcase'
    char_u	*b_p_dict;	// 'dictionary' local value
#నిర్వచితమైతే FEAT_DIFF
    char_u	*b_p_dia;	// 'diffanchors' local value
#అయితే_ముగింపు
    char_u	*b_p_tsr;	// 'thesaurus' local value
#నిర్వచితమైతే FEAT_COMPL_FUNC
    char_u	*b_p_tsrfu;	// 'thesaurusfunc' local value
    callback_T	b_tsrfu_cb;	// 'thesaurusfunc' callback
#అయితే_ముగింపు
    దీర్ఘ	b_p_ul;		// 'undolevels' local value
#నిర్వచితమైతే FEAT_PERSISTENT_UNDO
    పూర్ణాంకం		b_p_udf;	// 'undofile'
#అయితే_ముగింపు
    char_u	*b_p_lw;	// 'lispwords' local value
#నిర్వచితమైతే FEAT_TERMINAL
    దీర్ఘ	b_p_twsl;	// 'termwinscroll'
#అయితే_ముగింపు

    /*
     * end of buffer options
     */

    // values set from b_p_cino
    పూర్ణాంకం		b_ind_level;
    పూర్ణాంకం		b_ind_open_imag;
    పూర్ణాంకం		b_ind_no_brace;
    పూర్ణాంకం		b_ind_మొదటి_open;
    పూర్ణాంకం		b_ind_open_extra;
    పూర్ణాంకం		b_ind_close_extra;
    పూర్ణాంకం		b_ind_open_ఎడమ_imag;
    పూర్ణాంకం		b_ind_jump_label;
    పూర్ణాంకం		b_ind_case;
    పూర్ణాంకం		b_ind_case_సంకేతం;
    పూర్ణాంకం		b_ind_case_break;
    పూర్ణాంకం		b_ind_param;
    పూర్ణాంకం		b_ind_func_type;
    పూర్ణాంకం		b_ind_comment;
    పూర్ణాంకం		b_ind_లోపల_comment;
    పూర్ణాంకం		b_ind_లోపల_comment2;
    పూర్ణాంకం		b_ind_cpp_baseclass;
    పూర్ణాంకం		b_ind_continuation;
    పూర్ణాంకం		b_ind_unclosed;
    పూర్ణాంకం		b_ind_unclosed2;
    పూర్ణాంకం		b_ind_unclosed_noignore;
    పూర్ణాంకం		b_ind_unclosed_wrapped;
    పూర్ణాంకం		b_ind_unclosed_whiteok;
    పూర్ణాంకం		b_ind_matching_paren;
    పూర్ణాంకం		b_ind_paren_prev;
    పూర్ణాంకం		b_ind_maxparen;
    పూర్ణాంకం		b_ind_maxcomment;
    పూర్ణాంకం		b_ind_scopedecl;
    పూర్ణాంకం		b_ind_scopedecl_సంకేతం;
    పూర్ణాంకం		b_ind_java;
    పూర్ణాంకం		b_ind_js;
    పూర్ణాంకం		b_ind_keep_case_label;
    పూర్ణాంకం		b_ind_hash_comment;
    పూర్ణాంకం		b_ind_cpp_namespace;
    పూర్ణాంకం		b_ind_if_for_while;
    పూర్ణాంకం		b_ind_cpp_extern_c;
    పూర్ణాంకం		b_ind_pragma;

    linenr_T	b_no_eol_lnum;	// non-zero lnum when last line of next binary
				// write should not have an end-of-line

    పూర్ణాంకం		b_ప్రారంభం_eof;	// last line had eof (CTRL-Z) when it was read
    పూర్ణాంకం		b_ప్రారంభం_eol;	// last line had eol when it was read
    పూర్ణాంకం		b_ప్రారంభం_ffc;	// first char of 'ff' when edit started
    char_u	*b_ప్రారంభం_fenc;	// 'fileencoding' when edit started or NULL
    పూర్ణాంకం		b_bad_char;	// "++bad=" argument when edit started or 0
    పూర్ణాంకం		b_ప్రారంభం_bomb;	// 'bomb' when it was read

#నిర్వచితమైతే FEAT_EVAL
    dictitem_T	b_bufvar;	// variable for "b:" Dictionary
    dict_T	*b_vars;	// internal variables, local to buffer

    listener_T	*b_listener;       // Listeners accepting buffered reports.
    listener_T	*b_sync_listener;  // Listeners requiring unbuffered reports.
    list_T	*b_recorded_changes;
    size_t	b_recorded_పాఠ్యం_కొలత;  // bytes of text held by the above
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_PROP_POPUP
    bool	b_has_textprop;	// true when text props were added
    hashtab_T	*b_proptypes;	// text property types local to buffer
    proptype_T	**b_proparray;	// entries of b_proptypes sorted on tp_id
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_BEVAL) && defined(FEAT_EVAL)
    char_u	*b_p_bexpr;	// 'balloonexpr' local value
    long_u	b_p_bexpr_flags;// flags for 'balloonexpr'
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_CRYPT
    char_u	*b_p_cm;	// 'cryptmethod'
#అయితే_ముగింపు

    // When a buffer is created, it starts without a swap file.  b_may_swap is
    // then set to indicate that a swap file may be opened later.  It is reset
    // if a swap file could not be opened.
    bool	b_may_మార్పిడి;
    bool	b_did_warn;	// Set to true if user has been warned on first
				// change of a read-only file

    // Two special kinds of buffers:
    // help buffer  - used for help files, won't use a swap file.
    // spell buffer - used for spell info, never displayed and doesn't have a
    //		      file name.
    bool	b_help;		// true for help file buffer (when set b_p_bt
				// is "help")
#నిర్వచితమైతే FEAT_SPELL
    bool	b_spell;	// true for a spell file buffer, most fields
				// are not used!  Use the B_SPELL macro to
				// access b_spell without #ifdef.
#అయితే_ముగింపు

    bool	b_shortname;	// this file has an 8.3 file name

#నిర్వచితమైతే FEAT_పని_CHANNEL
    char_u	*b_prompt_పాఠ్యం;		// set by prompt_setprompt()
    callback_T	b_prompt_callback;	// set by prompt_setcallback()
    callback_T	b_prompt_అంతరాయం;	// set by prompt_setinterrupt()
    పూర్ణాంకం		b_prompt_చొప్పించు;	// value for restart_edit when entering
					// a prompt buffer window.
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_MZSCHEME
    శూన్య	*b_mzscheme_ref; // The MzScheme reference to this buffer
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_PERL
    శూన్య	*b_perl_వ్యక్తిగత;
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_PYTHON
    శూన్య	*b_python_ref;	// The Python reference to this buffer
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_PYTHON3
    శూన్య	*b_python3_ref;	// The Python3 reference to this buffer
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_TCL
    శూన్య	*b_tcl_ref;
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_RUBY
    శూన్య	*b_ruby_ref;
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_SYN_HL) || defined(FEAT_SPELL)
    synblock_T	b_s;		// Info related to syntax highlighting.  w_s
				// normally points to this, but some windows
				// may use a different synblock_T.
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_SIGNS
    sign_నమోదు_T *b_signlist;	   // list of placed signs
    sign_నమోదు_T *b_sign_finger;   // last sign inserted, used to speed up
				   // inserting signs in ascending line order
# ifdef FEAT_NETBEANS_INTG
    bool	b_has_sign_స్తంభం; // Flag that is set when a first sign is
				   // added and remains set until the end of
				   // the netbeans session.
# endif
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_NETBEANS_INTG
    bool	b_netbeans_file;    // true when buffer is owned by NetBeans
    bool	b_was_netbeans_file;// true if b_netbeans_file was once set
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_పని_CHANNEL
    bool	b_write_గమ్యం_channel; // true when appended lines are written to
				    // a channel.
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_CRYPT
    cryptstate_T *b_cryptstate;	// Encryption state while reading or writing
				// the file. NULL when not using encryption.
#అయితే_ముగింపు
    పూర్ణాంకం		b_mapped_ctrl_c; // modes where CTRL-C is mapped

#నిర్వచితమైతే FEAT_TERMINAL
    పదం_T	*b_పదం;	// When not NULL this buffer is for a terminal
				// window.
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_DIFF
    bool	b_diff_failed;	// internal diff failed for this buffer
#అయితే_ముగింపు
}; // file_buffer


#ఒకవేళ defined(FEAT_DIFF)
/*
 * Stuff for diff mode.
 */
# define DB_లెక్క 8	// up to eight buffers can be diff'ed

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
రకనిర్వచనం నిర్మాణం diffblock_S diff_T;
నిర్మాణం diffblock_S
{
    diff_T	*df_తదుపరి;
    linenr_T	df_lnum[DB_లెక్క];	// line number in buffer
    linenr_T	df_లెక్క[DB_లెక్క];	// nr of inserted/changed lines
    పూర్ణాంకం is_linematched;  // has the linematch algorithm ran on this diff hunk to divide it into
			  // smaller diff hunks?

    పూర్ణాంకం		has_changes;		// has cached list of inline changes
    garray_T	df_changes;		// list of inline changes (diffline_change_T)
};

/*
 * Each entry stores a single inline change within a diff block. Line numbers
 * are recorded as relative offsets, and columns are byte offsets, not
 * character counts.
 * Ranges are [start,end), with the end being exclusive.
 */
రకనిర్వచనం నిర్మాణం diffline_change_S diffline_change_T;
నిర్మాణం diffline_change_S
{
    colnr_T	dc_ప్రారంభం[DB_లెక్క];	// byte offset of start of range in the line
    colnr_T	dc_చివర[DB_లెక్క];	// 1 past byte offset of end of range in line
    పూర్ణాంకం		dc_ప్రారంభం_lnum_off[DB_లెక్క];	// starting line offset
    పూర్ణాంకం		dc_చివర_lnum_off[DB_లెక్క];	// end line offset
};

/*
 * Describes a single line's list of inline changes. Use diff_change_parse() to
 * parse this.
 */
రకనిర్వచనం నిర్మాణం diffline_S diffline_T;
నిర్మాణం diffline_S
{
    diffline_change_T *changes;
    పూర్ణాంకం అంకె_changes;
    పూర్ణాంకం bufidx;
    పూర్ణాంకం lineoff;
};
#లేకపోతే  // FEAT_DIFF
రకనిర్వచనం శూన్య diffline_T;
రకనిర్వచనం శూన్య diffline_change_T;
#అయితే_ముగింపు

#నిర్వచించు SNAP_HELP_IDX	    0
#నిర్వచించు SNAP_AUCMD_IDX	    1
#నిర్వచించు SNAP_QUICKFIX_IDX   2
#నిర్వచించు SNAP_లెక్క	    3

/*
 * Tab pages point to the top frame of each tab page.
 * Note: Most values are NOT valid for the current tab page!  Use "curwin",
 * "firstwin", etc. for that.  "tp_topframe" is always valid and can be
 * compared against "topframe" to find the current tab page.
 */
రకనిర్వచనం నిర్మాణం tabpage_S tabpage_T;
నిర్మాణం tabpage_S
{
    tabpage_T	    *tp_తదుపరి;	    // next tabpage or NULL
    చట్రం_T	    *tp_topframe;   // topframe for the windows
    win_T	    *tp_curwin;	    // current window in this Tab page
    win_T	    *tp_prevwin;    // previous window in this Tab page
    win_T	    *tp_firstwin;   // first window in this Tab page
    win_T	    *tp_lastwin;    // last window in this Tab page
#నిర్వచితమైతే FEAT_PROP_POPUP
    win_T	    *tp_మొదటి_popupwin; // first popup window in this Tab page
#అయితే_ముగింపు
    దీర్ఘ	    tp_old_అడ్డువరుసలు;    // Rows when Tab page was left
    దీర్ఘ	    tp_old_Columns; // Columns when Tab page was left, -1 when
				    // calling shell_new_columns() postponed
    పూర్ణాంకం		    tp_old_coloff;  // Column offset when Tab page was left
    దీర్ఘ	    tp_ch_వాడిన;	    // value of 'cmdheight' when frame size
				    // was set
#నిర్వచితమైతే FEAT_GUI
    పూర్ణాంకం		    tp_prev_which_scrollbars[3];
				    // previous value of which_scrollbars
#అయితే_ముగింపు
    పూర్ణాంకం		    tp_did_tabclosedpre;  // whether TabClosedPre was triggered

    char_u	    *tp_localdir;	// absolute path of local directory or
					// NULL
    char_u	    *tp_prevdir;	// previous directory

#నిర్వచితమైతే FEAT_DIFF
    diff_T	    *tp_మొదటి_diff;
    buf_T	    *(tp_diffbuf[DB_లెక్క]);
    పూర్ణాంకం		    tp_diff_invalid;	// list of diffs is outdated
    పూర్ణాంకం		    tp_diff_update;	// update diffs before redrawing
#అయితే_ముగింపు
    చట్రం_T	    *(tp_snapshot[SNAP_లెక్క]);  // window layout snapshots
#నిర్వచితమైతే FEAT_EVAL
    dictitem_T	    tp_winvar;	    // variable for "t:" Dictionary
    dict_T	    *tp_vars;	    // internal variables, local to tab page
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_PYTHON
    శూన్య	    *tp_python_ref;	// The Python value for this tab page
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_PYTHON3
    శూన్య	    *tp_python3_ref;	// The Python value for this tab page
#అయితే_ముగింపు
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
రకనిర్వచనం నిర్మాణం w_పంక్తి
{
    linenr_T	wl_lnum;	// buffer line number for logical line
    short_u	wl_కొలత;	// height in screen lines
    అక్షరం	wl_valid;	// TRUE values are valid for text in buffer
#నిర్వచితమైతే FEAT_FOLDING
    అక్షరం	wl_folded;	// TRUE when this is a range of folded lines
    linenr_T	wl_lastlnum;	// last buffer line number for logical line
#అయితే_ముగింపు
} wline_T;

/*
 * Windows are kept in a tree of frames.  Each frame has a column (FR_COL)
 * or row (FR_ROW) layout or is a leaf, which has a window.
 */
నిర్మాణం చట్రం_S
{
    అక్షరం	fr_layout;	// FR_LEAF, FR_COL or FR_ROW
    పూర్ణాంకం		fr_వెడల్పు;
    పూర్ణాంకం		fr_newwidth;	// new width used in win_equal_rec()
    పూర్ణాంకం		fr_ఎత్తు;
    పూర్ణాంకం		fr_newheight;	// new height used in win_equal_rec()
    చట్రం_T	*fr_మాతృ;	// containing frame or NULL
    చట్రం_T	*fr_తదుపరి;	// frame right or below in same parent, NULL
				// for last
    చట్రం_T	*fr_prev;	// frame left or above in same parent, NULL
				// for first
    // fr_child and fr_win are mutually exclusive
    చట్రం_T	*fr_బిడ్డ;	// first contained frame
    win_T	*fr_win;	// window that fills this frame; for a snapshot
				// set to the current window
};

#నిర్వచించు FR_ఆకు	0	// frame is a leaf
#నిర్వచించు FR_అడ్డువరుస	1	// frame with a row of windows
#నిర్వచించు FR_నిలువు	2	// frame with a column of windows

/*
 * Struct used for highlighting 'hlsearch' matches, matches defined by
 * ":match" and matches defined by match functions.
 * For 'hlsearch' there is one pattern for all windows.  For ":match" and the
 * match functions there is a different pattern for each window.
 */
రకనిర్వచనం నిర్మాణం
{
    regmmatch_T	rm;	    // points to the regexp program; contains last
			    // found match (may continue in next line)
    buf_T	*buf;	    // the buffer to search for a match
    linenr_T	lnum;	    // the line to search for a match
    పూర్ణాంకం		attr;	    // attributes to be used for a match
    పూర్ణాంకం		attr_cur;   // attributes currently active in win_line()
    linenr_T	మొదటి_lnum; // first lnum to search for multi-line pat
    colnr_T	startcol;   // in win_line() points to char where HL starts
    colnr_T	endcol;	    // in win_line() points to char where HL ends
    అక్షరం	is_addpos;  // position specified directly by
			    // matchaddpos(). TRUE/FALSE
    అక్షరం	has_cursor; // TRUE if the cursor is inside the match, used for
			    // CurSearch
} సరిపోలిక_T;

/*
 * Same as lpos_T, but with additional field len.
 */
రకనిర్వచనం నిర్మాణం
{
    linenr_T	lnum;	// line number
    colnr_T	నిలువు;	// column number
    పూర్ణాంకం		len;	// length: 0 - to the end of line
} llpos_T;

/*
 * matchitem_T provides a linked list for storing match items for ":match",
 * matchadd() and matchaddpos().
 */
రకనిర్వచనం నిర్మాణం matchitem matchitem_T;
నిర్మాణం matchitem
{
    matchitem_T	*mit_తదుపరి;
    పూర్ణాంకం		mit_id;		// match ID
    పూర్ణాంకం		mit_ప్రాధాన్యత;   // match priority

    // Either a pattern is defined (mit_pattern is not NUL) or a list of
    // positions is given (mit_pos is not NULL and mit_pos_count > 0).
    char_u	*mit_నమూనా;   // pattern to highlight
    regmmatch_T	mit_సరిపోలిక;	// regexp program for pattern

    llpos_T	*mit_pos_array;	// array of positions
    పూర్ణాంకం		mit_pos_లెక్క;	// nr of entries in mit_pos
    పూర్ణాంకం		mit_pos_cur;	// internal position counter
    linenr_T	mit_toplnum;	// top buffer line
    linenr_T	mit_botlnum;	// bottom buffer line

    సరిపోలిక_T	mit_hl;		// struct for doing the actual highlighting
    పూర్ణాంకం		mit_hlg_id;	// highlight group ID
#నిర్వచితమైతే FEAT_CONCEAL
    పూర్ణాంకం		mit_conceal_char; // cchar for Conceal highlighting
#అయితే_ముగింపు
};

// Structure to store last cursor position and topline.  Used by check_lnums()
// and reset_lnums().
రకనిర్వచనం నిర్మాణం
{
    పూర్ణాంకం		w_topline_save;	// original topline value
    పూర్ణాంకం		w_topline_corr;	// corrected topline value
    pos_T	w_cursor_save;	// original cursor position
    pos_T	w_cursor_corr;	// corrected cursor position
} pos_save_T;

#నిర్వచితమైతే FEAT_MENU
రకనిర్వచనం నిర్మాణం {
    పూర్ణాంకం		wb_startcol;
    పూర్ణాంకం		wb_endcol;
    vimmenu_T	*wb_menu;
} winbar_అంశం_T;
#అయితే_ముగింపు

/*
 * Characters from the 'listchars' option
 */
రకనిర్వచనం నిర్మాణం
{
    పూర్ణాంకం		eol;
    పూర్ణాంకం		ext;
    పూర్ణాంకం		prec;
    పూర్ణాంకం		nbsp;
    పూర్ణాంకం		space;
    పూర్ణాంకం		tab1;
    పూర్ణాంకం		tab2;
    పూర్ణాంకం		tab3;
    పూర్ణాంకం		trail;
    పూర్ణాంకం		lead;
    పూర్ణాంకం		leadtab1;
    పూర్ణాంకం		leadtab2;
    పూర్ణాంకం		leadtab3;
    పూర్ణాంకం		*multispace;
    పూర్ణాంకం		*leadmultispace;
#నిర్వచితమైతే FEAT_CONCEAL
    పూర్ణాంకం		conceal;
#అయితే_ముగింపు
} lcs_chars_T;

/*
 * Characters from the 'fillchars' option
 */
రకనిర్వచనం నిర్మాణం
{
    పూర్ణాంకం	stl;
    పూర్ణాంకం	stlnc;
    పూర్ణాంకం	vert;
    పూర్ణాంకం	fold;
    పూర్ణాంకం	foldopen;
    పూర్ణాంకం	foldclosed;
    పూర్ణాంకం	foldsep;
    పూర్ణాంకం	foldinner;
    పూర్ణాంకం	diff;
    పూర్ణాంకం	eob;
    పూర్ణాంకం	lastline;
#ఒకవేళ defined(FEAT_TABPANEL)
    పూర్ణాంకం	tpl_vert;
#అయితే_ముగింపు
    పూర్ణాంకం trunc;
    పూర్ణాంకం truncrl;
} fill_chars_T;

/*
 * Represents current highlight overrides (used by 'winhighlight' option). The
 * highlight group with ID "from" will be overridden by the highlight group with
 * ID "to"
 */
రకనిర్వచనం నిర్మాణం
{
    పూర్ణాంకం from; // If zero or negative then it is hlf_T enum
    పూర్ణాంకం గమ్యం; // Same thing as "from"
} hl_override_T;

/*
 * Structure which contains all information that belongs to a window
 *
 * All row numbers are relative to the start of the window, except w_winrow.
 */
నిర్మాణం కిటికీ_S
{
    పూర్ణాంకం		w_id;		    // unique window ID

    buf_T	*w_బఫరు;	    // buffer we are a window into

    win_T	*w_prev;	    // link to previous window
    win_T	*w_తదుపరి;	    // link to next window

#ఒకవేళ defined(FEAT_SYN_HL) || defined(FEAT_SPELL)
    synblock_T	*w_s;		    // for :ownsyntax
#అయితే_ముగింపు

    పూర్ణాంకం		w_locked;	    // don't let autocommands close the window

    చట్రం_T	*w_చట్రం;	    // frame containing this window

    pos_T	w_cursor;	    // cursor position in buffer

    colnr_T	w_curswant;	    // The column we'd like to be at.  This is
				    // used to try to stay in the same column
				    // for up/down cursor motions.

    bool	w_సమితి_curswant;	    // If set, then update w_curswant the next
				    // time through cursupdate() to the
				    // current virtual column

#నిర్వచితమైతే FEAT_SYN_HL
    linenr_T	w_చివరి_cursorline;  // where last time 'cursorline' was drawn
#అయితే_ముగింపు

    /*
     * the next seven are used to update the Visual highlighting
     */
    అక్షరం	w_old_visual_బహుళకం;  // last known VIsual_mode
    linenr_T	w_old_cursor_lnum;  // last known end of visual part
    colnr_T	w_old_cursor_fcol;  // first column for block visual part
    colnr_T	w_old_cursor_lcol;  // last column for block visual part
    linenr_T	w_old_visual_lnum;  // last known start of visual part
    colnr_T	w_old_visual_నిలువు;   // last known start of visual part
    colnr_T	w_old_curswant;	    // last known value of Curswant

    linenr_T    w_చివరి_cursor_lnum_rnu;  // cursor lnum when 'rnu' was last
					 // redrawn

    lcs_chars_T	w_lcs_chars;	    // 'listchars' characters
    fill_chars_T w_fill_chars;	    // 'fillchars' characters

    /*
     * "w_topline", "w_leftcol" and "w_skipcol" specify the offsets for
     * displaying the buffer.
     */
    linenr_T	w_topline;	    // buffer line number of the line at the
				    // top of the window
    bool	w_topline_was_సమితి;  // flag set to true when topline is set,
				    // e.g. by winrestview()

    linenr_T	w_botline;	    // number of the line below the bottom of
				    // the window

#నిర్వచితమైతే FEAT_DIFF
    పూర్ణాంకం		w_topfill;	    // number of filler lines above w_topline
    పూర్ణాంకం		w_old_topfill;	    // w_topfill at last redraw
    bool	w_botfill;	    // true when filler lines are actually
				    // below w_topline (at end of file)
    bool	w_old_botfill;	    // w_botfill at last redraw
#అయితే_ముగింపు
    colnr_T	w_leftcol;	    // screen column number of the left most
				    // character in the window; used when
				    // 'wrap' is off
    colnr_T	w_skipcol;	    // starting screen column for the first
				    // line in the window; used when 'wrap' is
				    // on; does not include win_col_off()

    పూర్ణాంకం		w_ఖాళీలు_అడ్డువరుసలు;	    // number of ~ rows in window
#నిర్వచితమైతే FEAT_DIFF
    పూర్ణాంకం		w_filler_అడ్డువరుసలు;	    // number of filler rows at the end of the
				    //కిటికీ
#అయితే_ముగింపు

    // six fields that are only used when there is a WinScrolled autocommand
    linenr_T	w_చివరి_topline;	    // last known value for w_topline
#నిర్వచితమైతే FEAT_DIFF
    పూర్ణాంకం		w_చివరి_topfill;	    // last known value for w_topfill
#అయితే_ముగింపు
    colnr_T	w_చివరి_leftcol;	    // last known value for w_leftcol
    colnr_T	w_చివరి_skipcol;	    // last known value for w_skipcol
    పూర్ణాంకం		w_చివరి_వెడల్పు;	    // last known value for w_width
    పూర్ణాంకం		w_చివరి_ఎత్తు;	    // last known value for w_height

    /*
     * Layout of the window in the screen.
     * May need to add "msg_scrolled" to "w_winrow" in rare situations.
     */
    పూర్ణాంకం		w_winrow;	    // first row of window in screen
    పూర్ణాంకం		w_ఎత్తు;	    // number of rows in window, excluding
				    // status/command/winbar line(s)
    పూర్ణాంకం		w_prev_winrow;	    // previous winrow used for 'splitkeep'
    పూర్ణాంకం		w_prev_ఎత్తు;	    // previous height used for 'splitkeep'
    పూర్ణాంకం		w_stl_rendered_ఎత్తు; // rendered height of window-local 'stl'
				    // (number of "%@" + 1)
    stl_click_region_T *w_stl_click;  // statusline click regions
    పూర్ణాంకం		w_stl_click_లెక్క;  // number of click regions
    పూర్ణాంకం		w_స్థితి_ఎత్తు;    // number of status lines.
				    // If 'statuslineopt' was changed, this
				    // member holds the previous value until
				    // frame_change_statusline_height() calls.
    పూర్ణాంకం		w_wincol;	    // Leftmost column of window in screen.
    పూర్ణాంకం		w_వెడల్పు;	    // Width of window, excluding separation.
    పూర్ణాంకం		w_vsep_వెడల్పు;	    // Number of separator columns (0 or 1).

    pos_save_T	w_save_cursor;	    // backup of cursor pos and topline
    bool	w_do_win_fix_cursor;// if true cursor may be invalid

#నిర్వచితమైతే FEAT_PROP_POPUP
    పూర్ణాంకం		w_popup_flags;	    // POPF_ values
    పూర్ణాంకం		w_popup_blend;	    // 0-100: transparency level for popup with opacitys
    పూర్ణాంకం		w_popup_handled;    // POPUP_HANDLE[0-9] flags
    char_u	*w_popup_title;
    poppos_T	w_popup_pos;
    bool	w_popup_fixed;	    // do not shift popup to fit on screen
    పూర్ణాంకం		w_popup_prop_type;  // when not zero: textprop type ID
    win_T	*w_popup_prop_win;  // window to search for textprop
    పూర్ణాంకం		w_popup_prop_id;    // when not zero: textprop ID
    పూర్ణాంకం		w_zindex;
    పూర్ణాంకం		w_minheight;	    // "minheight" for popup window
    పూర్ణాంకం		w_minwidth;	    // "minwidth" for popup window
    పూర్ణాంకం		w_maxheight;	    // "maxheight" for popup window
    పూర్ణాంకం		w_maxwidth;	    // "maxwidth" for popup window
    పూర్ణాంకం		w_maxwidth_opt;	    // maxwidth from option
    పూర్ణాంకం		w_wantline;	    // "line" for popup window
    పూర్ణాంకం		w_wantcol;	    // "col" for popup window
    పూర్ణాంకం		w_firstline;	    // "firstline" for popup window
    పూర్ణాంకం		w_want_scrollbar;   // when zero don't use a scrollbar
    bool	w_has_scrollbar;    // true if scrollbar displayed
    char_u	*w_scrollbar_highlight; // "scrollbarhighlight"
    char_u	*w_thumb_highlight; // "thumbhighlight"
    పూర్ణాంకం		w_popup_padding[4]; // popup padding top/right/bot/left
    పూర్ణాంకం		w_popup_border[4];  // popup border top/right/bot/left
    char_u	*w_border_highlight[4];  // popup border highlight
    bool	w_border_highlight_isset; // borderhighlight was explicitly set
    పూర్ణాంకం		w_border_char[8];   // popup border characters
    పూర్ణాంకం		w_popup_shadow;     // popup shadow (right and bottom edges)

    పూర్ణాంకం		w_popup_leftoff;    // columns left of the screen
    పూర్ణాంకం		w_popup_rightoff;   // columns right of the screen
    పూర్ణాంకం		w_popup_topoff;	    // rows above the host window's top
				    // when "clipwindow" is set
    పూర్ణాంకం		w_popup_bottomoff;  // rows below the host window's bottom
				    // when "clipwindow" is set
    పూర్ణాంకం		w_popup_leftclip;   // columns left of the host window's left
				    // when "clipwindow" is set
    పూర్ణాంకం		w_popup_rightclip;  // columns right of the host window's right
				    // when "clipwindow" is set
    varnumber_T	w_popup_చివరి_changedtick; // b:changedtick of popup buffer
					  // when position was computed
    varnumber_T	w_popup_prop_changedtick; // b:changedtick of buffer with
					  // w_popup_prop_type when position
					  // was computed
    పూర్ణాంకం		w_popup_prop_topline; // w_topline of window with
				      // w_popup_prop_type when position was
				      // computed
    పూర్ణాంకం		w_popup_prop_winrow;  // w_winrow of host window when
				      // position was computed
    పూర్ణాంకం		w_popup_prop_wincol;  // w_wincol of host window when
				      // position was computed
    పూర్ణాంకం		w_popup_prop_వెడల్పు;   // w_width of host window when
				      // position was computed
    పూర్ణాంకం		w_popup_prop_winheight; // w_height of host window when
				      // position was computed
    linenr_T	w_popup_చివరి_curline; // last known w_cursor.lnum of window
				      // with "cursorline" set
    callback_T	w_close_cb;	    // popup close callback
    callback_T	w_filter_cb;	    // popup filter callback
    పూర్ణాంకం		w_filter_errors;    // popup filter error count
    పూర్ణాంకం		w_filter_బహుళకం;	    // mode when filter callback is used

    win_T	*w_popup_curwin;    // close popup if curwin differs
    linenr_T	w_popup_lnum;	    // close popup if cursor not on this line
    colnr_T	w_popup_mincol;	    // close popup if cursor before this col
    colnr_T	w_popup_maxcol;	    // close popup if cursor after this col
    పూర్ణాంకం		w_popup_mouse_అడ్డువరుస;  // close popup if mouse moves away
    పూర్ణాంకం		w_popup_mouse_mincol;  // close popup if mouse moves away
    పూర్ణాంకం		w_popup_mouse_maxcol;  // close popup if mouse moves away
    popclose_T	w_popup_close;	    // allow closing the popup with the mouse

    list_T	*w_popup_ముసుగు;	     // list of lists for "mask"
    char_u	*w_popup_ముసుగు_cells; // cached mask cells
    పూర్ణాంకం		w_popup_ముసుగు_ఎత్తు; // height of w_popup_mask_cells
    పూర్ణాంకం		w_popup_ముసుగు_వెడల్పు;  // width of w_popup_mask_cells

# ఒకవేళ defined(FEAT_TIMERS)
    timer_T	*w_popup_timer;	    // timer for closing popup window
# endif

    పూర్ణాంకం		w_flags;	    // WFLAG_ flags

# define WFLAG_WCOL_OFF_ADDED	1   // popup border and padding were added to
				    // w_wcol
# define WFLAG_WROW_OFF_ADDED	2   // popup border and padding were added to
				    // w_wrow

# ifdef FEAT_IMAGE
    image_placement_T *w_popup_imagep;
# endif
#అయితే_ముగింపు

    /*
     * === start of cached values ====
     */
    /*
     * Recomputing is minimized by storing the result of computations.
     * Use functions in screen.c to check if they are valid and to update.
     * w_valid is a bitfield of flags, which indicate if specific values are
     * valid or need to be recomputed.	See screen.c for values.
     */
    పూర్ణాంకం		w_valid;
    pos_T	w_valid_cursor;	    // last known position of w_cursor, used
				    // to adjust w_valid
    colnr_T	w_valid_leftcol;    // last known w_leftcol
    colnr_T	w_valid_skipcol;    // last known w_skipcol

    /*
     * w_cline_height is the number of physical lines taken by the buffer line
     * that the cursor is on.  We use this to avoid extra calls to plines().
     */
    పూర్ణాంకం		w_cline_ఎత్తు;	    // current size of cursor line
#నిర్వచితమైతే FEAT_FOLDING
    bool	w_cline_folded;	    // cursor line is folded
#అయితే_ముగింపు

    పూర్ణాంకం		w_cline_అడ్డువరుస;	    // starting row of the cursor line

    colnr_T	w_virtcol;	    // column number of the cursor in the
				    // buffer line, as opposed to the column
				    // number we're at on the screen.  This
				    // makes a difference on lines which span
				    // more than one screen line or when
				    // w_leftcol is non-zero

#నిర్వచితమైతే FEAT_PROP_POPUP
    colnr_T	w_virtcol_మొదటి_char;	// offset for w_virtcol when there are
					// virtual text properties above the
					//పంక్తి
#అయితే_ముగింపు
    /*
     * w_wrow and w_wcol specify the cursor position in the window.
     * This is related to positions in the window, not in the display or
     * buffer, thus w_wrow is relative to w_winrow.
     */
    పూర్ణాంకం		w_wrow, w_wcol;	    // cursor position in window
#నిర్వచితమైతే FEAT_CONCEAL
    పూర్ణాంకం		w_wcol_conceal_off; // screen cells concealed before w_wcol on
				    // the cursor's screen line, set by win_line()
#అయితే_ముగింపు

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
    పూర్ణాంకం		w_పంక్తులు_valid;	    // number of valid entries
    wline_T	*w_పంక్తులు;

#నిర్వచితమైతే FEAT_FOLDING
    garray_T	w_folds;	    // array of nested folds
    bool	w_fold_manual;	    // when true: some folds are opened/closed
				    // manually
    bool	w_foldinvalid;	    // when true: folding needs to be
				    // recomputed
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_LINEBREAK
    పూర్ణాంకం		w_nrwidth;	    // width of 'number' and 'relativenumber'
				    // column being used
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_TERMINAL
    termcellcolor_T w_పదం_hlfwin;  // cache for term color of HLF_WIN
#అయితే_ముగింపు

    /*
     * === end of cached values ===
     */

    పూర్ణాంకం		w_redr_type;	    // type of redraw to be performed on win
    పూర్ణాంకం		w_upd_అడ్డువరుసలు;	    // number of window lines to update when
				    // w_redr_type is UPD_REDRAW_TOP
    linenr_T	w_redraw_శిఖరం;	    // when != 0: first line needing redraw
    linenr_T	w_redraw_bot;	    // when != 0: last line needing redraw
    bool	w_redr_స్థితి;	    // if true status line must be redrawn

    // remember what is shown in the ruler for this window (if 'ruler' set)
    pos_T	w_ru_cursor;	    // cursor position shown in ruler
    colnr_T	w_ru_virtcol;	    // virtcol shown in ruler
    linenr_T	w_ru_topline;	    // topline shown in ruler
    linenr_T	w_ru_పంక్తి_లెక్క;    // line count used for ruler
#నిర్వచితమైతే FEAT_DIFF
    పూర్ణాంకం		w_ru_topfill;	    // topfill shown in ruler
#అయితే_ముగింపు
    bool	w_ru_ఖాళీలు;	    // true if ruler shows 0-1 (empty line)

    పూర్ణాంకం		w_alt_fnum;	    // alternate file (for # and CTRL-^)

    alist_T	*w_alist;	    // pointer to arglist for this window
    పూర్ణాంకం		w_arg_idx;	    // current index in argument list (can be
				    // out of range!)
    bool	w_arg_idx_invalid;  // editing another file than w_arg_idx

    char_u	*w_localdir;	    // absolute path of local directory or
				    // NULL
    char_u	*w_prevdir;	    // previous directory
#నిర్వచితమైతే FEAT_MENU
    vimmenu_T	*w_winbar;	    // The root of the WinBar menu hierarchy.
    winbar_అంశం_T *w_winbar_వస్తువులు;  // list of items in the WinBar
    పూర్ణాంకం		w_winbar_ఎత్తు;    // 1 if there is a window toolbar
#అయితే_ముగింపు

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
#నిర్వచించు GLOBAL_WO(p)	((అక్షరం *)(p) + పరిమాణం(winopt_T))

#ఒకవేళ defined(FEAT_SIGNS) || defined(FEAT_FOLDING) || defined(FEAT_DIFF)
    పూర్ణాంకం		*w_p_cc_నిలువుల_సంఖ్య;	    // array of columns to highlight or NULL
    char_u	w_p_culopt_flags;   // flags for cursorline highlighting
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_LINEBREAK
    పూర్ణాంకం		w_briopt_min;	    // minimum width for breakindent
    పూర్ణాంకం		w_briopt_జరుపు;	    // additional shift for breakindent
    పూర్ణాంకం		w_briopt_sbr;	    // sbr in 'briopt'
    పూర్ణాంకం		w_briopt_list;      // additional indent for lists
    పూర్ణాంకం		w_briopt_vcol;	    // indent for specific column
#అయితే_ముగింపు

    దీర్ఘ	w_scbind_pos;

#నిర్వచితమైతే FEAT_EVAL
    dictitem_T	w_winvar;	// variable for "w:" Dictionary
    dict_T	*w_vars;	// internal variables, local to window
#అయితే_ముగింపు

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
    పూర్ణాంకం		w_jumplistlen;		// number of active entries
    పూర్ణాంకం		w_jumplistidx;		// current position

    పూర్ణాంకం		w_changelistidx;	// current position in b_changelist

#నిర్వచితమైతే FEAT_శోధన_EXTRA
    matchitem_T	*w_సరిపోలిక_తల;		// head of match list
    పూర్ణాంకం		w_తదుపరి_సరిపోలిక_id;	// next match ID
#అయితే_ముగింపు

    /*
     * the tagstack grows from 0 upwards:
     * entry 0: older
     * entry 1: newer
     * entry 2: newest
     */
    taggy_T	w_tagstack[TAGSTACKSIZE];   // the tag stack
    పూర్ణాంకం		w_tagstackidx;		    // idx just below active entry
    పూర్ణాంకం		w_tagstacklen;		    // number of tags on stack

    /*
     * w_fraction is the fractional row of the cursor within the window, from
     * 0 at the top row to FRACTION_MULT at the last row.
     * w_prev_fraction_row was the actual cursor row when w_fraction was last
     * calculated.
     */
    పూర్ణాంకం		w_fraction;
    పూర్ణాంకం		w_prev_fraction_అడ్డువరుస;

#నిర్వచితమైతే FEAT_GUI
    scrollbar_T	w_scrollbars[2];	// vert. Scrollbars for this window
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_LINEBREAK
    linenr_T	w_nrwidth_పంక్తి_లెక్క;	// line count when ml_nrwidth_width
					// was computed.
    దీర్ఘ	w_nuw_cached;		// 'numberwidth' option cached
    పూర్ణాంకం		w_nrwidth_వెడల్పు;	// nr of chars to print line count.
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_QUICKFIX
    qf_info_T	*w_llist;		// Location list for this window
    /*
     * Location list reference used in the location list window.
     * In a non-location list window, w_llist_ref is NULL.
     */
    qf_info_T	*w_llist_ref;
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_MZSCHEME
    శూన్య	*w_mzscheme_ref;	// The MzScheme value for this window
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_PERL
    శూన్య	*w_perl_వ్యక్తిగత;
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_PYTHON
    శూన్య	*w_python_ref;		// The Python value for this window
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_PYTHON3
    శూన్య	*w_python3_ref;		// The Python value for this window
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_TCL
    శూన్య	*w_tcl_ref;
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_RUBY
    శూన్య	*w_ruby_ref;
#అయితే_ముగింపు

    hl_override_T *w_hl;
    పూర్ణాంకం		w_hl_len;
    పూర్ణాంకం		w_hlfwin_id; // Cached HLF_WIN highlight group id, zero if none,
			     // or -1 if it was set to itself.
};

/*
 * Arguments for operators.
 */
రకనిర్వచనం నిర్మాణం oparg_S
{
    పూర్ణాంకం		op_type;	// current pending operator type
    పూర్ణాంకం		regname;	// register to use for the operator
    పూర్ణాంకం		motion_type;	// type of the current cursor motion
    పూర్ణాంకం		motion_force;	// force motion type: 'v', 'V' or CTRL-V
    పూర్ణాంకం		use_reg_one;	// TRUE if delete uses reg 1 even when not
				// linewise
    పూర్ణాంకం		inclusive;	// TRUE if char motion is inclusive (only
				// valid when motion_type is MCHAR)
    పూర్ణాంకం		చివర_adjusted;	// backuped b_op_end one char (only used by
				// do_format())
    pos_T	ప్రారంభం;		// start of the operator
    pos_T	చివర;		// end of the operator
    pos_T	cursor_ప్రారంభం;	// cursor position before motion for "gw"

    దీర్ఘ	పంక్తి_లెక్క;	// number of lines from op_start to op_end
				// (inclusive)
    పూర్ణాంకం		ఖాళీలు_2;		// op_start and op_end the same (only used by
				// do_change())
    పూర్ణాంకం		is_VIsual;	// operator on Visual area
    పూర్ణాంకం		ఖండం_బహుళకం;	// current operator is Visual block mode
    colnr_T	ప్రారంభం_vcol;	// start col for block mode operator
    colnr_T	చివర_vcol;	// end col for block mode operator
    దీర్ఘ	prev_opcount;	// ca.opcount saved for K_CURSORHOLD
    దీర్ఘ	prev_count0;	// ca.count0 saved for K_CURSORHOLD
    పూర్ణాంకం		excl_tr_ws;	// exclude trailing whitespace for yank of a
				//ఖండం
} oparg_T;

/*
 * Arguments for Normal mode commands.
 */
రకనిర్వచనం నిర్మాణం cmdarg_S
{
    oparg_T	*oap;		// Operator arguments
    పూర్ణాంకం		prechar;	// prefix character (optional, always 'g')
    పూర్ణాంకం		cmdchar;	// command character
    పూర్ణాంకం		nchar;		// next command character (optional)
    పూర్ణాంకం		ncharC1;	// first composing character (optional)
    పూర్ణాంకం		ncharC2;	// second composing character (optional)
    పూర్ణాంకం		extra_char;	// yet another character (optional)
    దీర్ఘ	opcount;	// count before an operator
    దీర్ఘ	count0;		// count before command, default 0
    దీర్ఘ	count1;		// count before command, default 1
    పూర్ణాంకం		arg;		// extra argument from nv_cmds[]
    పూర్ణాంకం		retval;		// return: CA_* values
    char_u	*searchbuf;	// return: pointer to search pattern or NULL
} cmdarg_T;

// values for retval:
#నిర్వచించు CA_COMMAND_BUSY	    1	// skip restarting edit() once
#నిర్వచించు CA_NO_ADJ_OP_చివర    2	// don't adjust operator end

#నిర్వచితమైతే CURSOR_SHAPE
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
# define SHAPE_IDX_స్థితి 10	// A status line
# define SHAPE_IDX_SDRAG 11	// dragging a status line
# define SHAPE_IDX_VSEP	12	// A vertical separator line
# define SHAPE_IDX_VDRAG 13	// dragging a vertical separator line
# define SHAPE_IDX_మరిన్ని	14	// Hit-return or More
# define SHAPE_IDX_MOREL	15	// Hit-return or More in last line
# define SHAPE_IDX_SM	16	// showing matching paren
# define SHAPE_IDX_లెక్క	17

# define SHAPE_ఖండం	0	// block cursor
# define SHAPE_HOR	1	// horizontal bar cursor
# define SHAPE_VER	2	// vertical bar cursor

# define MSHAPE_NUMBERED	1000	// offset for shapes identified by number
# define MSHAPE_HIDE	1	// hide mouse pointer

# define SHAPE_MOUSE	1	// used for mouse pointer shape
# define SHAPE_CURSOR	2	// used for text cursor shape

రకనిర్వచనం నిర్మాణం cursor_నమోదు
{
    పూర్ణాంకం		shape;		// one of the SHAPE_ defines
    పూర్ణాంకం		mshape;		// one of the MSHAPE defines
    పూర్ణాంకం		percentage;	// percentage of cell for bar
    దీర్ఘ	blinkwait;	// blinking, wait time before blinking starts
    దీర్ఘ	blinkon;	// blinking, on time
    దీర్ఘ	blinkoff;	// blinking, off time
    పూర్ణాంకం		id;		// highlight group ID
    పూర్ణాంకం		id_lm;		// highlight group ID for :lmap mode
    అక్షరం	*name;		// mode name (fixed)
    అక్షరం	వాడిన_for;	// SHAPE_MOUSE and/or SHAPE_CURSOR
} cursorentry_T;
#అయితే_ముగింపు // CURSOR_SHAPE

#నిర్వచితమైతే FEAT_MENU

// Indices into vimmenu_T->strings[] and vimmenu_T->noremap[] for each mode
# define MENU_సూచీ_INVALID	-1
# define MENU_సూచీ_సాధారణ	0
# define MENU_సూచీ_VISUAL	1
# define MENU_సూచీ_SELECT	2
# define MENU_సూచీ_OP_PENDING	3
# define MENU_సూచీ_చొప్పించు	4
# define MENU_సూచీ_CMDLINE	5
# define MENU_సూచీ_TERMINAL	6
# define MENU_సూచీ_TIP		7
# define MENU_MODES		8

// Menu modes
# define MENU_సాధారణ_బహుళకం	(1 << MENU_సూచీ_సాధారణ)
# define MENU_VISUAL_బహుళకం	(1 << MENU_సూచీ_VISUAL)
# define MENU_SELECT_బహుళకం	(1 << MENU_సూచీ_SELECT)
# define MENU_OP_PENDING_బహుళకం	(1 << MENU_సూచీ_OP_PENDING)
# define MENU_చొప్పించు_బహుళకం	(1 << MENU_సూచీ_చొప్పించు)
# define MENU_CMDLINE_బహుళకం	(1 << MENU_సూచీ_CMDLINE)
# define MENU_TERMINAL_బహుళకం	(1 << MENU_సూచీ_TERMINAL)
# define MENU_TIP_బహుళకం		(1 << MENU_సూచీ_TIP)
# define MENU_ALL_MODES		((1 << MENU_సూచీ_TIP) - 1)
// note MENU_INDEX_TIP is not a 'real' mode

// Start a menu name with this to not include it on the main menu bar
# define MNU_HIDDEN_CHAR		']'

నిర్మాణం VimMenu
{
    పూర్ణాంకం		modes;		    // Which modes is this menu visible for?
    పూర్ణాంకం		enabled;	    // for which modes the menu is enabled
    char_u	*name;		    // Name of menu, possibly translated
    char_u	*dname;		    // Displayed Name ("name" without '&')
# ifdef FEAT_MULTI_LANG
    char_u	*en_name;	    // "name" untranslated, NULL when "name"
				    // was not translated
    char_u	*en_dname;	    // "dname" untranslated, NULL when "dname"
				    // was not translated
# endif
    char_u	*actext;	    // accelerator text (after TAB)
    పూర్ణాంకం		mnemonic;	    // mnemonic key (after '&')
    పూర్ణాంకం		ప్రాధాన్యత;	    // Menu order priority
# ifdef FEAT_GUI
    శూన్య	(*cb)(vimmenu_T *); // Call-back function
# endif
# ifdef FEAT_TOOLBAR
    char_u	*iconfile;	    // name of file for icon or NULL
    పూర్ణాంకం		iconidx;	    // icon index (-1 if not set)
    పూర్ణాంకం		icon_builtin;	    // icon names is BuiltIn{nr}
# endif
    char_u	*అక్షరశ్రేణులు[MENU_MODES]; // Mapped string for each mode
    పూర్ణాంకం		noremap[MENU_MODES]; // A REMAP_ flag for each mode
    అక్షరం	silent[MENU_MODES]; // A silent flag for each mode
    vimmenu_T	*children;	    // Children of sub-menu
    vimmenu_T	*మాతృ;	    // Parent of menu
    vimmenu_T	*తదుపరి;		    // Next item in menu
# ifdef FEAT_GUI_X11
    Widget	id;		    // Manage this to enable item
    Widget	submenu_id;	    // If this is submenu, add children here
# endif
# ifdef FEAT_GUI_GTK
    GtkWidget	*id;		    // Manage this to enable item
    GtkWidget	*submenu_id;	    // If this is submenu, add children here
#  ఒకవేళ defined(GTK_CHECK_సంస్కరణ) && !GTK_CHECK_సంస్కరణ(3,4,0)
    GtkWidget	*tearoff_handle;
#  endif
#  ifndef USE_GTK4
    GtkWidget   *label;		    // Used by "set wak=" code.
#  endif
# endif
# ifdef FEAT_GUI_MOTIF
    పూర్ణాంకం		sensitive;	    // turn button on/off
    అక్షరం	**xpm;		    // pixmap data
    అక్షరం	*xpm_fname;	    // file with pixmap data
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
    BMenu_అంశం  *id;		    // Id of menu item
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
#లేకపోతే
// For generating prototypes when FEAT_MENU isn't defined.
రకనిర్వచనం పూర్ణాంకం vimmenu_T;

#అయితే_ముగింపు // FEAT_MENU

/*
 * Struct to save values in before executing autocommands for a buffer that is
 * not the current buffer.
 */
రకనిర్వచనం నిర్మాణం
{
    పూర్ణాంకం		use_aucmd_win_idx;  // index in aucmd_win[] if >= 0
    పూర్ణాంకం		save_curwin_id;	    // ID of saved curwin
    పూర్ణాంకం		new_curwin_id;	    // ID of new curwin
    పూర్ణాంకం		save_prevwin_id;    // ID of saved prevwin
    bufref_T	new_curbuf;	    // new curbuf
    char_u	*tp_localdir;	    // saved value of tp_localdir
    char_u	*globaldir;	    // saved value of globaldir
    పూర్ణాంకం		save_VIsual_చురుకైన; // saved VIsual_active
#నిర్వచితమైతే FEAT_పని_CHANNEL
    పూర్ణాంకం		save_prompt_చొప్పించు; // saved b_prompt_insert
#అయితే_ముగింపు
} aco_save_T;

/*
 * Generic option table item, only used for printer at the moment.
 */
రకనిర్వచనం నిర్మాణం
{
    స్థిరరాశి అక్షరం	*name;
    పూర్ణాంకం		hasnum;
    దీర్ఘ	సంఖ్య;
    char_u	*string;	// points into option string
    పూర్ణాంకం		అక్షరశ్రేణి_పొడవు;
    పూర్ణాంకం		present;
} option_పట్టిక_T;

/*
 * Structure to hold printing color and font attributes.
 */
రకనిర్వచనం నిర్మాణం
{
    long_u	fg_color;
    long_u	bg_color;
    పూర్ణాంకం		bold;
    పూర్ణాంకం		italic;
    పూర్ణాంకం		underline;
    పూర్ణాంకం		undercurl;
} prt_పాఠ్యం_attr_T;

/*
 * Structure passed back to the generic printer code.
 */
రకనిర్వచనం నిర్మాణం
{
    పూర్ణాంకం		n_collated_copies;
    పూర్ణాంకం		n_uncollated_copies;
    పూర్ణాంకం		duplex;
    పూర్ణాంకం		chars_చొప్పున_పంక్తి;
    పూర్ణాంకం		పంక్తులు_చొప్పున_పుట;
    పూర్ణాంకం		has_color;
    prt_పాఠ్యం_attr_T సంఖ్య;
#నిర్వచితమైతే FEAT_SYN_HL
    పూర్ణాంకం		modec;
    పూర్ణాంకం		do_syntax;
#అయితే_ముగింపు
    పూర్ణాంకం		user_abort;
#నిర్వచితమైతే FEAT_PRINT_PANGO
    పూర్ణాంకం		user_abort_సందేశం;
#అయితే_ముగింపు
    char_u	*jobname;
#ఒకవేళ defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
    char_u	*outfile;
    char_u	*arguments;
#అయితే_ముగింపు
} prt_settings_T;

#నిర్వచించు PRINT_సంఖ్య_వెడల్పు 8

/*
 * Used for popup menu items.
 */
రకనిర్వచనం నిర్మాణం
{
    char_u	*pum_పాఠ్యం;		// main menu text
    char_u	*pum_రకం;		// extra kind text (may be truncated)
    char_u	*pum_extra;		// extra menu text (may be truncated)
    char_u	*pum_info;		// extra info
    పూర్ణాంకం		pum_cpt_ఆధారం_idx;	// index of completion source in 'cpt'
    పూర్ణాంకం		pum_user_abbr_hlattr;	// highlight attribute for abbr
    పూర్ణాంకం		pum_user_రకం_hlattr;	// highlight attribute for kind
} pumitem_T;

/*
 * Structure used for get_tagfname().
 */
రకనిర్వచనం నిర్మాణం
{
    char_u	*tn_ట్యాగులు;	// value of 'tags' when starting
    char_u	*tn_np;		// current position in tn_tags
    పూర్ణాంకం		tn_did_filefind_init;
    పూర్ణాంకం		tn_hf_idx;
    శూన్య	*tn_శోధన_ctx;
} tagname_T;

రకనిర్వచనం నిర్మాణం {
    UINT32_T మొత్తం[2];
    UINT32_T state[8];
    char_u   బఫరు[64];
} context_sha256_T;

/*
 * types for expressions.
 */
రకనిర్వచనం గణన
{
    సమాసం_UNKNOWN = 0,
    సమాసం_EQUAL,		// ==
    సమాసం_NEQUAL,	// !=
    సమాసం_GREATER,	// >
    సమాసం_GEQUAL,	// >=
    సమాసం_SMALLER,	// <
    సమాసం_SEQUAL,	// <=
    సమాసం_సరిపోలిక,		// =~
    సమాసం_NOMATCH,	// !~
    సమాసం_IS,		// is
    సమాసం_ISNOT,		// isnot
    // used with ISN_OPNR
    సమాసం_కూడు,		// +
    సమాసం_SUB,		// -
    సమాసం_MULT,		// *
    సమాసం_DIV,		// /
    సమాసం_REM,		// %
    సమాసం_LSHIFT,	// <<
    సమాసం_RSHIFT,	// >>
    // used with ISN_ADDLIST
    సమాసం_COPY,		// create new list
    సమాసం_APPEND,	// append to first list
} exprtype_T;

/*
 * Structure used for reading in json_decode().
 */
నిర్మాణం js_reader
{
    char_u	*js_buf;	// text to be decoded
    char_u	*js_చివర;	// NUL in js_buf
    పూర్ణాంకం		js_వాడిన;	// bytes used from js_buf
    పూర్ణాంకం		(*js_fill)(నిర్మాణం js_reader *);
				// function to fill the buffer or NULL;
				// return TRUE when the buffer was filled
    శూన్య	*js_cookie;	// can be used by js_fill
    పూర్ణాంకం		js_cookie_arg;	// can be used by js_fill
};
రకనిర్వచనం నిర్మాణం js_reader js_read_T;

// Maximum number of commands from + or -c arguments.
#నిర్వచించు MAX_ARG_CMDS 10

// values for "window_layout"
#నిర్వచించు WIN_HOR	    1	    // "-o" horizontally split windows
#నిర్వచించు	WIN_VER	    2	    // "-O" vertically split windows
#నిర్వచించు	WIN_TABS    3	    // "-p" windows on tab pages

// Struct for various parameters passed between main() and other functions.
రకనిర్వచనం నిర్మాణం
{
    పూర్ణాంకం		argc;
    అక్షరం	**argv;

    char_u	*fname;			// first file to edit

    పూర్ణాంకం		evim_బహుళకం;		// started as "evim"
    char_u	*use_vimrc;		// vimrc from -u argument
    పూర్ణాంకం		clean;			// --clean argument

    పూర్ణాంకం		n_commands;		     // no. of commands from + or -c
    char_u	*commands[MAX_ARG_CMDS];     // commands from + or -c arg.
    char_u	cmds_tofree[MAX_ARG_CMDS];   // commands that need free()
    పూర్ణాంకం		n_pre_commands;		     // no. of commands from --cmd
    char_u	*pre_commands[MAX_ARG_CMDS]; // commands from --cmd argument

    పూర్ణాంకం		edit_type;		// type of editing to do
    char_u	*tagname;		// tag from -t argument
#నిర్వచితమైతే FEAT_QUICKFIX
    char_u	*use_ef;		// 'errorfile' from -q argument
#అయితే_ముగింపు

    పూర్ణాంకం		want_నిండిన_screen;
    పూర్ణాంకం		not_a_పదం;		// no warning for missing term?
#నిర్వచితమైతే FEAT_GUI
    char_u	*gui_dialog_file;	// file to write dialog text in
#అయితే_ముగింపు
    పూర్ణాంకం		tty_fail;		// exit if not a tty
    char_u	*పదం;			// specified terminal name
#నిర్వచితమైతే FEAT_CRYPT
    పూర్ణాంకం		ask_for_కీలకం;		// -x argument
#అయితే_ముగింపు
    పూర్ణాంకం		no_మార్పిడి_file;		// "-n" argument used
#నిర్వచితమైతే FEAT_EVAL
    పూర్ణాంకం		use_debug_break_level;
#అయితే_ముగింపు
    పూర్ణాంకం		కిటికీ_లెక్క;		// number of windows to use
    పూర్ణాంకం		కిటికీ_layout;		// 0, WIN_HOR, WIN_VER or WIN_TABS

#నిర్వచితమైతే FEAT_CLIENTSERVER
    పూర్ణాంకం		serverArg;		// TRUE when argument for a server
    char_u	*serverName_arg;	// cmdline arg for server name
    char_u	*serverStr;		// remote server command
    char_u	*serverStrEnc;		// encoding of serverStr
    char_u	*servername;		// allocated name for our server
#అయితే_ముగింపు
#ఒకవేళ !defined(UNIX)
# define EXPAND_FILENAMES
    పూర్ణాంకం		literal;		// don't expand file names
#అయితే_ముగింపు
#నిర్వచితమైతే MSWIN
    పూర్ణాంకం		నిండిన_మార్గం;		// file name argument was full path
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_DIFF
    పూర్ణాంకం		diff_బహుళకం;		// start with 'diff' set
#అయితే_ముగింపు
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
రకనిర్వచనం నిర్మాణం lval_S
{
    char_u	*ll_name;	// start of variable name (can be NULL)
    char_u	*ll_name_చివర;	// end of variable name (can be NULL)
    type_T	*ll_type;	// type of variable (can be NULL)
    char_u	*ll_exp_name;	// NULL or expanded name in allocated memory.

    scid_T	ll_sid;		// for an imported item: the script ID it was
				// imported from; zero otherwise

    typval_T	*ll_tv;		// Typeval of item being used.  If "newkey"
				// isn't NULL it's the Dict to which to add
				// the item.
    listitem_T	*ll_li;		// The list item or NULL.
    list_T	*ll_list;	// The list or NULL.
    పూర్ణాంకం		ll_range;	// TRUE when a [i:j] range was used
    పూర్ణాంకం		ll_empty2;	// Second index is empty: [i:]
    దీర్ఘ	ll_n1;		// First index for list
    దీర్ఘ	ll_n2;		// Second index for list range
    dict_T	*ll_dict;	// The Dictionary or NULL
    dictitem_T	*ll_di;		// The dictitem or NULL
    char_u	*ll_newkey;	// New key for Dict in alloc. mem or NULL.
    type_T	*ll_valtype;	// type expected for the value or NULL
    blob_T	*ll_blob;	// The Blob or NULL
    tuple_T	*ll_tuple;	// tuple or NULL
    ufunc_T	*ll_ufunc;	// The function or NULL
    object_T	*ll_object;	// The object or NULL, class is not NULL
    class_T	*ll_class;	// The class or NULL, object may be NULL
    పూర్ణాంకం		ll_oi;		// The object/class member index
    పూర్ణాంకం		ll_is_మూలం;	// TRUE if ll_tv is the lval_root, like a
				// plain object/class. ll_tv is variable.
    garray_T	ll_type_list;   // list of pointers to allocated types
} lval_T;

/**
 * This specifies optional parameters for get_lval(). Arguments may be NULL.
 */
రకనిర్వచనం నిర్మాణం lval_మూలం_S {
    typval_T	*lr_tv;		// Base typval.
    class_T	*lr_cl_exec;	// Executing class for access checking.
    పూర్ణాంకం		lr_is_arg;	// name is an arg (not a member).
} lval_మూలం_T;

// Structure used to save the current state.  Used when executing Normal mode
// commands while in any other mode.
రకనిర్వచనం నిర్మాణం {
    పూర్ణాంకం		save_సందేశం_scroll;
    పూర్ణాంకం		save_restart_edit;
    పూర్ణాంకం		save_సందేశం_didout;
    పూర్ణాంకం		save_State;
    పూర్ణాంకం		save_insertmode;
    పూర్ణాంకం		save_ముగింపు_op;
    పూర్ణాంకం		save_opcount;
    పూర్ణాంకం		save_reg_executing;
    పూర్ణాంకం		save_pending_చివర_reg_executing;
    పూర్ణాంకం		save_script_సంస్కరణ;
    tasave_T	tabuf;
} save_state_T;

రకనిర్వచనం నిర్మాణం {
    varnumber_T vv_prevcount;
    varnumber_T vv_లెక్క;
    varnumber_T vv_count1;
} vimvars_save_T;

// Scope for changing directory
రకనిర్వచనం గణన {
    CDSCOPE_GLOBAL,	// :cd
    CDSCOPE_TABPAGE,	// :tcd
    CDSCOPE_కిటికీ	// :lcd
} cdscope_T;

// Variable flavor
రకనిర్వచనం గణన
{
    VAR_FLAVOUR_DEFAULT,	// doesn't start with uppercase
    VAR_FLAVOUR_SESSION,	// starts with uppercase, some lower
    VAR_FLAVOUR_VIMINFO		// all uppercase
} var_flavour_T;

// argument for mouse_find_win()
రకనిర్వచనం గణన {
    IGNORE_POPUP,	// only check non-popup windows
    కనుగొను_POPUP,		// also find popup windows
    FAIL_POPUP		// return NULL if mouse on popup window
} mouse_కనుగొను_T;

// Symbolic names for some registers.
#నిర్వచించు DELETION_రిజిస్టరు	36
#ఒకవేళ defined(FEAT_CLIPBOARD) || defined(HAVE_CLIPMETHOD)
# define నక్షత్రం_రిజిస్టరు		37
# ఒకవేళ defined(FEAT_X11) || defined(FEAT_WAYLAND)
#  define PLUS_రిజిస్టరు	38
#  define REAL_PLUS_రిజిస్టరు	PLUS_రిజిస్టరు
# లేకపోతే
#  define PLUS_రిజిస్టరు	నక్షత్రం_రిజిస్టరు	    // there is only one
#  ifdef FEAT_EVAL
// Make it so that if clipmethod is "none", the plus register is not available,
// but if clipmethod is a provider, then expose the plus register for use.
#   define REAL_PLUS_రిజిస్టరు	38
#  లేకపోతే
#   define REAL_PLUS_రిజిస్టరు	నక్షత్రం_రిజిస్టరు
#  endif
# endif
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_DND
# define TILDE_రిజిస్టరు		(PLUS_రిజిస్టరు + 1)
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_CLIPBOARD
# ifdef FEAT_DND
#  define అంకె_REGISTERS		(TILDE_రిజిస్టరు + 1)
# లేకపోతే
#  define అంకె_REGISTERS		(REAL_PLUS_రిజిస్టరు + 1)
# endif
#లేకపోతే
# ifdef HAVE_CLIPMETHOD
#  define అంకె_REGISTERS		(REAL_PLUS_రిజిస్టరు + 1)
# లేకపోతే
#  define అంకె_REGISTERS		37
# endif
#అయితే_ముగింపు

// structure used by block_prep, op_delete and op_yank for blockwise operators
// also op_change, op_shift, op_insert, op_replace - AKelly
నిర్మాణం ఖండం_def
{
    పూర్ణాంకం		startspaces;	// 'extra' cols before first char
    పూర్ణాంకం		endspaces;	// 'extra' cols after last char
    పూర్ణాంకం		textlen;	// chars in block
    char_u	*textstart;	// pointer to 1st char (partially) in block
    colnr_T	textcol;	// index of chars (partially) in block
    colnr_T	ప్రారంభం_vcol;	// start col of 1st char wholly inside block
    colnr_T	చివర_vcol;	// start col of 1st char wholly after block
    పూర్ణాంకం		is_short;	// TRUE if line is too short to fit in block
    పూర్ణాంకం		is_MAX;		// TRUE if curswant==MAXCOL when starting
    పూర్ణాంకం		is_oneChar;	// TRUE if block within one character
    పూర్ణాంకం		pre_whitesp;	// screen cols of ws before block
    పూర్ణాంకం		pre_whitesp_c;	// chars of ws before block
    colnr_T	చివర_char_vcols;	// number of vcols of post-block char
    colnr_T	ప్రారంభం_char_vcols; // number of vcols of pre-block char
};

// Each yank register has an array of pointers to lines.
రకనిర్వచనం నిర్మాణం
{
    string_T	*y_array;	// pointer to array of string_T structs
    linenr_T	y_కొలత;		// number of lines in y_array
    char_u	y_type;		// MLINE, MCHAR or MBLOCK
    colnr_T	y_వెడల్పు;	// only set if y_type == MBLOCK
#నిర్వచితమైతే FEAT_VIMINFO
    time_t	y_సమయం_సమితి;
#అయితే_ముగింపు
} yankreg_T;

// The offset for a search command is store in a soff struct
// Note: only spats[0].off is really used
రకనిర్వచనం నిర్మాణం soffset
{
    పూర్ణాంకం		dir;		// search direction, '/' or '?'
    పూర్ణాంకం		పంక్తి;		// search has line offset
    పూర్ణాంకం		చివర;		// search set cursor at end
    దీర్ఘ	off;		// line or char offset
} soffset_T;

// A search pattern and its attributes are stored in a spat struct
రకనిర్వచనం నిర్మాణం spat
{
    char_u	    *pat;	// the pattern (in allocated memory) or NULL
    size_t	    patlen;	// the length of the pattern (0 if pat is NULL)
    పూర్ణాంకం		    magic;	// magicness of the pattern
    పూర్ణాంకం		    no_scs;	// no smartcase for this pattern
    soffset_T	    off;
} spat_T;

/*
 * Optional extra arguments for searchit().
 */
రకనిర్వచనం నిర్మాణం
{
    linenr_T	sa_stop_lnum;	// stop after this line number when != 0
#నిర్వచితమైతే FEAT_RELTIME
    దీర్ఘ	sa_tm;		// timeout limit or zero
    పూర్ణాంకం		sa_timed_బహిర్గతం;	// set when timed out
#అయితే_ముగింపు
    పూర్ణాంకం		sa_wrapped;	// search wrapped around
} searchit_arg_T;

/*
 * Cookie used by getsourceline().
 */
/*
 * Cookie used to store info for each sourced file.
 * It is shared between do_source() and getsourceline().
 * This is passed to do_cmdline().
 */
రకనిర్వచనం నిర్మాణం {
    FILE	*fp;		// opened file for sourcing
    char_u	*nextline;	// if not NULL: line that was read ahead
    linenr_T	sourcing_lnum;	// line number of the source file
    పూర్ణాంకం		ముగిసిన;	// ":finish" used
    పూర్ణాంకం		ఆధారం_from_buf;// TRUE if sourcing from current buffer
    పూర్ణాంకం		buf_lnum;	// line number in the current buffer
    garray_T	buflines;	// lines in the current buffer
#నిర్వచితమైతే USE_CRNL
    పూర్ణాంకం		fileformat;	// EOL_UNKNOWN, EOL_UNIX or EOL_DOS
    పూర్ణాంకం		error;		// TRUE if LF found after CR-LF
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_EVAL
    linenr_T	breakpoint;	// next line with breakpoint or zero
    char_u	*fname;		// name of sourced file
    పూర్ణాంకం		dbg_tick;	// debug_tick when breakpoint was set
    పూర్ణాంకం		level;		// top nesting level of sourced file
#అయితే_ముగింపు
    vimconv_T	conv;		// type of conversion
} ఆధారం_cookie_T;


#నిర్వచించు WRITEBUFSIZE	8192	// size of normal write buffer

#నిర్వచించు FIO_LATIN1	0x01	// convert Latin1
#నిర్వచించు FIO_UTF8	0x02	// convert UTF-8
#నిర్వచించు FIO_UCS2	0x04	// convert UCS-2
#నిర్వచించు FIO_UCS4	0x08	// convert UCS-4
#నిర్వచించు FIO_UTF16	0x10	// convert UTF-16
#నిర్వచితమైతే MSWIN
# define FIO_CODEPAGE	0x20	// convert MS-Windows codepage
# define FIO_PUT_CP(x) (((x) & 0xffff) << 16)	// put codepage in top word
# define FIO_GET_CP(x)	(((x)>>16) & 0xffff)	// get codepage from top word
#అయితే_ముగింపు
#నిర్వచితమైతే MACOS_CONVERT
# define FIO_MACROMAN	0x20	// convert MacRoman
#అయితే_ముగింపు
#నిర్వచించు FIO_ENDIAN_L	0x80	// little endian
#నిర్వచించు FIO_ENCRYPTED	0x1000	// encrypt written bytes
#నిర్వచించు FIO_NOCONVERT	0x2000	// skip encoding conversion
#నిర్వచించు FIO_UCSBOM	0x4000	// check for BOM at start of file
#నిర్వచించు FIO_ALL	(-1)	// allow all formats

// When converting, a read() or write() may leave some bytes to be converted
// for the next call.  The value is guessed...
#నిర్వచించు CONV_RESTLEN 30

// We have to guess how much a sequence of bytes may expand when converting
// with iconv() to be able to allocate a buffer.
#నిర్వచించు ICONV_MULT 8

// Used for "magic_overruled".
రకనిర్వచనం గణన {
    OPTION_MAGIC_NOT_సమితి,	// p_magic not overruled
    OPTION_MAGIC_ON,		// magic on inside regexp
    OPTION_MAGIC_OFF		// magic off inside regexp
} optmagic_T;

// Magicness of a pattern, used by regexp code.
// The order and values matter:
//  magic <= MAGIC_OFF includes MAGIC_NONE
//  magic >= MAGIC_ON  includes MAGIC_ALL
రకనిర్వచనం గణన {
    MAGIC_NONE = 1,		// "\V" very unmagic
    MAGIC_OFF = 2,		// "\M" or 'magic' off
    MAGIC_ON = 3,		// "\m" or 'magic'
    MAGIC_ALL = 4		// "\v" very magic
} magic_T;

రకనిర్వచనం గణన {
    WT_UNKNOWN = 0,	// Unknown or unspecified location
    WT_వాదం,
    WT_VARIABLE,
    WT_MEMBER,
    WT_పద్ధతి,		// object method
    WT_పద్ధతి_ARG,	// object method argument type
    WT_పద్ధతి_RETURN,	// object method return type
    WT_CAST,		// type cast
} wherekind_T;

// Struct used to pass the location of a type check.  Used in error messages to
// indicate where the error happened.  Also used for doing covariance type
// check for object method return type and contra-variance type check for
// object method arguments.
రకనిర్వచనం నిర్మాణం {
    అక్షరం	*wt_func_name;  // function name or NULL
    అక్షరం	wt_సూచీ;	// argument or variable index, 0 means unknown
    wherekind_T	wt_రకం;	// type check location
} where_T;

#నిర్వచించు WHERE_INIT {ఖాళీ, 0, WT_UNKNOWN}

// Struct passed to get_v_event() and restore_v_event().
రకనిర్వచనం నిర్మాణం {
    పూర్ణాంకం		sve_did_save;
    hashtab_T	sve_hashtab;
} save_v_event_T;

// Enum used by filter(), map(), mapnew() and foreach()
రకనిర్వచనం గణన {
    FILTERMAP_FILTER,
    FILTERMAP_MAP,
    FILTERMAP_MAPNEW,
    FILTERMAP_FOREACH
} filtermap_T;

// Structure used by switch_win() to pass values to restore_win()
రకనిర్వచనం నిర్మాణం {
    win_T	*sw_curwin;
    tabpage_T	*sw_curtab;
    పూర్ణాంకం		sw_same_win;	    // VIsual_active was not reset
    పూర్ణాంకం		sw_visual_చురుకైన;
} switchwin_T;

// Fuzzy matched string list item. Used for fuzzy match completion. Items are
// usually sorted by 'score'. The 'idx' member is used for stable-sort.
రకనిర్వచనం నిర్మాణం {
    పూర్ణాంకం		idx;
    char_u	*str;
    పూర్ణాంకం		score;
} fuzmatch_str_T;

// Argument for lbr_chartabsize().
రకనిర్వచనం నిర్మాణం {
    win_T	*cts_win;
    char_u	*cts_పంక్తి;		// start of the line
    char_u	*cts_ptr;		// current position in line
#నిర్వచితమైతే FEAT_LINEBREAK
    పూర్ణాంకం		cts_bri_కొలత;		// cached size of 'breakindent', or -1
					// if not computed yet
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_PROP_POPUP
    పూర్ణాంకం		cts_పాఠ్యం_prop_లెక్క;	// number of text props; when zero
					// cts_text_props is not used
    textprop_T	*cts_పాఠ్యం_props;	// text props (allocated)
    అక్షరం	cts_has_prop_with_పాఠ్యం;	// TRUE if a property inserts text
    పూర్ణాంకం		cts_cur_పాఠ్యం_వెడల్పు;	// width of current inserted text
    పూర్ణాంకం		cts_prop_పంక్తులు;		// nr of properties above or below
    bool	cts_has_క్రింద;		// true if a text property below was
					// counted, its width fills up the line
    పూర్ణాంకం		cts_మొదటి_char;		// width text props above the line
    పూర్ణాంకం		cts_above_వెడల్పు;	// width of text props above the line,
					// kept for the whole line
    bool	cts_no_above;		// do not count the width of text props
					// above the line
    పూర్ణాంకం		cts_with_trailing;	// include size of trailing props with
					// last character
    పూర్ణాంకం		cts_ప్రారంభం_incl;		// prop has true "start_incl" arg
#అయితే_ముగింపు
    పూర్ణాంకం		cts_vcol;		// virtual column at current position
    పూర్ణాంకం		cts_max_తల_vcol;	// see win_lbr_chartabsize()
} chartabsize_T;

/*
 * Argument for the callback function (opt_did_set_cb_T) invoked after an
 * option value is modified.
 */
రకనిర్వచనం నిర్మాణం
{
    // Pointer to the option variable.  The variable can be a long (numeric
    // option), an int (boolean option) or a char pointer (string option).
    char_u	*os_varp;
    పూర్ణాంకం		os_idx;
    పూర్ణాంకం		os_flags;
    సమితి_op_T	os_op;

    // old value of the option (can be a string, number or a boolean)
    సంఘం
    {
	దీర్ఘ	సంఖ్య;
	పూర్ణాంకం	boolean;
	char_u	*string;
    } os_oldval;

    // new value of the option (can be a string, number or a boolean)
    సంఘం
    {
	దీర్ఘ	సంఖ్య;
	పూర్ణాంకం	boolean;
	char_u	*string;
    } os_newval;

    // Option value was checked to be safe, no need to set P_INSECURE
    // Used for the 'keymap', 'filetype' and 'syntax' options.
    పూర్ణాంకం		os_విలువ_checked;
    // Option value changed.  Used for the 'filetype' and 'syntax' options.
    పూర్ణాంకం		os_విలువ_మారింది;

    // Used by the 'isident', 'iskeyword', 'isprint' and 'isfname' options.
    // Set to TRUE if the character table is modified when processing the
    // option and need to be restored because of a failure.
    పూర్ణాంకం		os_restore_chartab;

#ఒకవేళ defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
    // Used by the 't_xxx' terminal options on MS-Windows.
    పూర్ణాంకం		os_did_swaptcap;
#అయితే_ముగింపు

    // If the value specified for an option is not valid and the error message
    // is parameterized, then the "os_errbuf" buffer is used to store the error
    // message (when it is not NULL).
    అక్షరం	*os_errbuf;
    // length of the error buffer
    size_t	os_errbuflen;
} optset_T;

/*
 * Argument for the callback function (opt_expand_cb_T) invoked after a string
 * option value is expanded for cmdline completion.
 */
రకనిర్వచనం నిర్మాణం
{
    // Pointer to the option variable. It's always a string.
    char_u	*oe_varp;
    // The original option value, escaped.
    char_u	*oe_opt_విలువ;

    // TRUE if using set+= instead of set=
    పూర్ణాంకం		oe_append;
    // TRUE if we would like to add the original option value as the first
    // choice.
    పూర్ణాంకం		oe_include_orig_val;

    // Regex from the cmdline, for matching potential options against.
    regmatch_T	*oe_regmatch;
    // The expansion context.
    expand_T	*oe_xp;

    // The full argument passed to :set. For example, if the user inputs
    // ':set dip=icase,algorithm:my<Tab>', oe_xp->xp_pattern will only have
    // 'my', but oe_set_arg will contain the whole 'icase,algorithm:my'.
    char_u	*oe_సమితి_arg;
} optexpand_T;

/*
 * Spell checking variables passed from win_update() to win_line().
 */
రకనిర్వచనం నిర్మాణం {
    పూర్ణాంకం		spv_has_spell;	    // drawn window has spell checking
#నిర్వచితమైతే FEAT_SPELL
    పూర్ణాంకం		spv_unchanged;	    // not updating for changed text
    పూర్ణాంకం		spv_checked_నిలువు;    // column in "checked_lnum" up to
				    // which there are no spell errors
    linenr_T	spv_checked_lnum;   // line number for "checked_col"
    పూర్ణాంకం		spv_cap_నిలువు;	    // column to check for Cap word
    linenr_T	spv_capcol_lnum;    // line number for "cap_col"
#అయితే_ముగింపు
} spellvars_T;

// Return the length of a string literal.
// This macro only computes a string's length for a string-literal token; for
// anything else, including a char*, compilation will fail (note "" following
// s).
#నిర్వచించు STRLEN_LITERAL(s) (పరిమాణం(s "") - 1)

// Store a key/value (string) pair
రకనిర్వచనం నిర్మాణం
{
    పూర్ణాంకం	    కీలకం;        // the key
    string_T విలువ;	// the value
} keyvalue_T;

#నిర్వచించు KEYVALUE_నమోదు(k, v) \
    {(k), {((char_u *)v), STRLEN_LITERAL(v)}}

#ఒకవేళ defined(UNIX) || defined(MSWIN) || defined(VMS) || defined(AMIGA)
// Defined as signed, to return -1 on error
నిర్మాణం cellsize {
    పూర్ణాంకం cs_xpixel;
    పూర్ణాంకం cs_ypixel;
};
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_WAYLAND

రకనిర్వచనం నిర్మాణం vwl_connection_S vwl_connection_T;
రకనిర్వచనం నిర్మాణం vwl_seat_S vwl_seat_T;

# ifdef FEAT_WAYLAND_CLIPBOARD

రకనిర్వచనం నిర్మాణం vwl_దత్తాంశం_offer_S vwl_దత్తాంశం_offer_T;
రకనిర్వచనం నిర్మాణం vwl_దత్తాంశం_ఆధారం_S vwl_దత్తాంశం_ఆధారం_T;
రకనిర్వచనం నిర్మాణం vwl_దత్తాంశం_device_S vwl_దత్తాంశం_device_T;
రకనిర్వచనం నిర్మాణం vwl_దత్తాంశం_device_manager_S vwl_దత్తాంశం_device_manager_T;

రకనిర్వచనం నిర్మాణం vwl_దత్తాంశం_device_listener_S vwl_దత్తాంశం_device_listener_T;
రకనిర్వచనం నిర్మాణం vwl_దత్తాంశం_ఆధారం_listener_S vwl_దత్తాంశం_ఆధారం_listener_T;
రకనిర్వచనం నిర్మాణం vwl_దత్తాంశం_offer_listener_S vwl_దత్తాంశం_offer_listener_T;

// Wayland selections
రకనిర్వచనం గణన {
    WAYLAND_SELECTION_NONE	= 0,
    WAYLAND_SELECTION_REGULAR	= 1 << 0,
    WAYLAND_SELECTION_PRIMARY	= 1 << 1,
} wayland_selection_T;

# endif
#అయితే_ముగింపు

// Used in term_set_sync_output()
రకనిర్వచనం గణన
{
    పదం_SYNC_OUTPUT_ENABLE = 1 << 0,
    పదం_SYNC_OUTPUT_DISABLE = 1 << 1,
    పదం_SYNC_OUTPUT_OFF = 1 << 2,
    పదం_SYNC_OUTPUT_FLUSH = 1 << 3,
} పదం_sync_output_T;
