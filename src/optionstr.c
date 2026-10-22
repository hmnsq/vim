/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * optionstr.c: Functions related to string options
 */

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

ਸਥਿਰ char_u shm_buf[SHM_LEN];
ਸਥਿਰ ਪੂਰਨ ਸੈੱਟ_shm_recursive = 0;

ਸਥਿਰ ਅੱਖਰ *(p_ambw_ਮੁੱਲਾਂ[]) = {"single", "double", ਨਿਰਾ};
ਸਥਿਰ ਅੱਖਰ *(p_bg_ਮੁੱਲਾਂ[]) = {"light", "dark", ਨਿਰਾ};
ਸਥਿਰ ਅੱਖਰ *(p_bkc_ਮੁੱਲਾਂ[]) = {"yes", "auto", "no", "breaksymlink", "breakhardlink", ਨਿਰਾ};
ਸਥਿਰ ਅੱਖਰ *(p_bo_ਮੁੱਲਾਂ[]) = {"all", "backspace", "cursor", "complete",
				 "copy", "ctrlg", "error", "esc", "ex",
				 "hangul", "insertmode", "lang", "mess",
				 "showmatch", "operator", "register", "shell",
				 "spell", "term", "wildmode", ਨਿਰਾ};
#ਜੇ defined(FEAT_LINEBREAK)
// Note: Keep this in sync with briopt_check()
ਸਥਿਰ ਅੱਖਰ *(p_briopt_ਮੁੱਲਾਂ[]) = {"shift:", "min:", "sbr", "list:", "column:", ਨਿਰਾ};
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_TABPANEL)
// Note: Keep this in sync with tabpanelopt_changed()
ਸਥਿਰ ਅੱਖਰ *(p_tplo_ਮੁੱਲਾਂ[]) = {"align:", "columns:", "scrollbar", "vert", ਨਿਰਾ};
ਸਥਿਰ ਅੱਖਰ *(p_tplo_align_ਮੁੱਲਾਂ[]) = {"left", "right", ਨਿਰਾ};
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_DIFF)
// Note: Keep this in sync with diffopt_changed()
ਸਥਿਰ ਅੱਖਰ *(p_dip_ਮੁੱਲਾਂ[]) = {"filler", "anchor", "context:", "iblank", "icase", "iwhite", "iwhiteall", "iwhiteeol", "horizontal", "vertical", "closeoff", "hiddenoff", "foldcolumn:", "followwrap", "internal", "indent-heuristic", "algorithm:", "inline:", "linematch:", ਨਿਰਾ};
ਸਥਿਰ ਅੱਖਰ *(p_dip_algorithm_ਮੁੱਲਾਂ[]) = {"myers", "minimal", "patience", "histogram", ਨਿਰਾ};
ਸਥਿਰ ਅੱਖਰ *(p_dip_inline_ਮੁੱਲਾਂ[]) = {"none", "simple", "char", "word", ਨਿਰਾ};
#ਜੇ_ਅੰਤ
ਸਥਿਰ ਅੱਖਰ *(p_nf_ਮੁੱਲਾਂ[]) = {"bin", "octal", "hex", "alpha", "unsigned", "blank", ਨਿਰਾ};
ਸਥਿਰ ਅੱਖਰ *(p_ff_ਮੁੱਲਾਂ[]) = {FF_UNIX, FF_DOS, FF_MAC, ਨਿਰਾ};
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_CLIPMETHOD
// Note: Keep this in sync with did_set_clipboard()
ਸਥਿਰ ਅੱਖਰ *(p_cb_ਮੁੱਲਾਂ[]) = {"unnamed", "unnamedplus",
# ifdef FEAT_CLIPBOARD
    "autoselect", "autoselectplus", "autoselectml", "html", "exclude:",
# endif
    ਨਿਰਾ};
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CRYPT
ਸਥਿਰ ਅੱਖਰ *(p_cm_ਮੁੱਲਾਂ[]) = {"zip", "blowfish", "blowfish2",
# ifdef FEAT_SODIUM
    "xchacha20", "xchacha20v2",
# endif
    ਨਿਰਾ};
#ਜੇ_ਅੰਤ
ਸਥਿਰ ਅੱਖਰ *(p_cmp_ਮੁੱਲਾਂ[]) = {"internal", "keepascii", ਨਿਰਾ};
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
// Note: Keep this in sync with fill_culopt_flags()
ਸਥਿਰ ਅੱਖਰ *(p_culopt_ਮੁੱਲਾਂ[]) = {"line", "screenline", "number", "both", ਨਿਰਾ};
#ਜੇ_ਅੰਤ
ਸਥਿਰ ਅੱਖਰ *(p_dy_ਮੁੱਲਾਂ[]) = {"lastline", "truncate", "uhex", ਨਿਰਾ};
ਸਥਿਰ ਅੱਖਰ *(p_jop_ਮੁੱਲਾਂ[]) = {"stack", ਨਿਰਾ};
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
ਸਥਿਰ ਅੱਖਰ *(p_fdo_ਮੁੱਲਾਂ[]) = {"all", "block", "hor", "mark", "percent",
				 "quickfix", "search", "tag", "insert",
				 "undo", "jump", ਨਿਰਾ};
#ਜੇ_ਅੰਤ
// Note: Keep this in sync with match_keyprotocol()
ਸਥਿਰ ਅੱਖਰ *(p_kpc_protocol_ਮੁੱਲਾਂ[]) = {"none", "mok2", "kitty", ਨਿਰਾ};
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_IMAGE
// Note: Keep this in sync with match_imageprotocol()
ਸਥਿਰ ਅੱਖਰ *(p_ipc_protocol_ਮੁੱਲਾਂ[]) = {"none", "kitty", "sixel", ਨਿਰਾ};
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
// Note: Keep this in sync with parse_popup_option()
ਸਥਿਰ ਅੱਖਰ *(p_popup_cpp_option_ਮੁੱਲਾਂ[]) = {"align:", "border:",
    "borderhighlight:", "close:", "height:", "highlight:", "opacity:",
    "resize:", "shadow:", "width:", ਨਿਰਾ};
ਸਥਿਰ ਅੱਖਰ *(p_popup_pvp_option_ਮੁੱਲਾਂ[]) = {"border:",
    "borderhighlight:", "close:", "height:", "highlight:", "opacity:",
    "resize:", "shadow:", "width:", ਨਿਰਾ};
ਸਥਿਰ ਅੱਖਰ *(p_popup_option_on_off_ਮੁੱਲਾਂ[]) = {"on", "off", ਨਿਰਾ};
ਸਥਿਰ ਅੱਖਰ *(p_popup_cpp_border_ਮੁੱਲਾਂ[]) = {"single", "double", "round",
    "ascii", "on", "off", "custom:", ਨਿਰਾ};
ਸਥਿਰ ਅੱਖਰ *(p_popup_pvp_border_ਮੁੱਲਾਂ[]) = {"single", "double", "round",
    "ascii", "on", "off", "custom:", ਨਿਰਾ};
ਸਥਿਰ ਅੱਖਰ *(p_popup_option_align_ਮੁੱਲਾਂ[]) = {"item", "menu", ਨਿਰਾ};
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_SPELL)
// Note: Keep this in sync with spell_check_sps()
ਸਥਿਰ ਅੱਖਰ *(p_sps_ਮੁੱਲਾਂ[]) = {"best", "fast", "double", "expr:", "file:", "timeout:", ਨਿਰਾ};
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SESSION
// Also used for 'viewoptions'!  Keep in sync with SSOP_ flags.
ਸਥਿਰ ਅੱਖਰ *(p_ssop_ਮੁੱਲਾਂ[]) = {"buffers", "winpos", "resize", "winsize",
    "localoptions", "options", "help", "blank", "globals", "slash", "unix",
    "sesdir", "curdir", "folds", "cursor", "tabpages", "terminal", "skiprtp",
    ਨਿਰਾ};
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_STL_OPT)
ਸਥਿਰ ਅੱਖਰ *(p_stlo_ਮੁੱਲਾਂ[]) = {"fixedheight", "maxheight:", ਨਿਰਾ};
#ਜੇ_ਅੰਤ
// Keep in sync with SWB_ flags in option.h
ਸਥਿਰ ਅੱਖਰ *(p_swb_ਮੁੱਲਾਂ[]) = {"useopen", "usetab", "split", "newtab", "vsplit", "uselast", ਨਿਰਾ};
ਸਥਿਰ ਅੱਖਰ *(p_spk_ਮੁੱਲਾਂ[]) = {"cursor", "screen", "topline", ਨਿਰਾ};
ਸਥਿਰ ਅੱਖਰ *(p_tc_ਮੁੱਲਾਂ[]) = {"followic", "ignore", "match", "followscs", "smart", ਨਿਰਾ};
// Keep in sync with TCL_ flags in option.h
ਸਥਿਰ ਅੱਖਰ *(p_tcl_ਮੁੱਲਾਂ[]) = {"left", "uselast", ਨਿਰਾ};
#ਜੇ defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
ਸਥਿਰ ਅੱਖਰ *(p_toolbar_ਮੁੱਲਾਂ[]) = {"text", "icons", "tooltips", "horiz", ਨਿਰਾ};
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_TOOLBAR) && defined(FEAT_GUI_GTK)
ਸਥਿਰ ਅੱਖਰ *(p_tbis_ਮੁੱਲਾਂ[]) = {"tiny", "small", "medium", "large", "huge", "giant", ਨਿਰਾ};
#ਜੇ_ਅੰਤ
#ਜੇ defined(UNIX) || defined(VMS)
ਸਥਿਰ ਅੱਖਰ *(p_ttym_ਮੁੱਲਾਂ[]) = {"xterm", "xterm2", "dec", "netterm", "jsbterm", "pterm", "urxvt", "sgr", ਨਿਰਾ};
#ਜੇ_ਅੰਤ
ਸਥਿਰ ਅੱਖਰ *(p_ve_ਮੁੱਲਾਂ[]) = {"block", "insert", "all", "onemore", "none", "NONE", ਨਿਰਾ};
// Note: Keep this in sync with check_opt_wim()
ਸਥਿਰ ਅੱਖਰ *(p_wim_ਮੁੱਲਾਂ[]) = {"full", "longest", "list", "lastused", "noselect", "noinsert", ਨਿਰਾ};
ਸਥਿਰ ਅੱਖਰ *(p_wop_ਮੁੱਲਾਂ[]) = {"fuzzy", "tagfile", "pum", "exacttext", ਨਿਰਾ};
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_WAK
ਸਥਿਰ ਅੱਖਰ *(p_wak_ਮੁੱਲਾਂ[]) = {"yes", "menu", "no", ਨਿਰਾ};
#ਜੇ_ਅੰਤ
ਸਥਿਰ ਅੱਖਰ *(p_mousem_ਮੁੱਲਾਂ[]) = {"extend", "popup", "popup_setpos", "mac", ਨਿਰਾ};
ਸਥਿਰ ਅੱਖਰ *(p_sel_ਮੁੱਲਾਂ[]) = {"inclusive", "exclusive", "old", ਨਿਰਾ};
ਸਥਿਰ ਅੱਖਰ *(p_slm_ਮੁੱਲਾਂ[]) = {"mouse", "key", "cmd", ਨਿਰਾ};
ਸਥਿਰ ਅੱਖਰ *(p_km_ਮੁੱਲਾਂ[]) = {"startsel", "stopsel", ਨਿਰਾ};
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_BROWSE
ਸਥਿਰ ਅੱਖਰ *(p_bsdir_ਮੁੱਲਾਂ[]) = {"current", "last", "buffer", ਨਿਰਾ};
#ਜੇ_ਅੰਤ
ਸਥਿਰ ਅੱਖਰ *(p_scbopt_ਮੁੱਲਾਂ[]) = {"ver", "hor", "jump", ਨਿਰਾ};
ਸਥਿਰ ਅੱਖਰ *(p_debug_ਮੁੱਲਾਂ[]) = {"msg", "throw", "beep", ਨਿਰਾ};
ਸਥਿਰ ਅੱਖਰ *(p_ead_ਮੁੱਲਾਂ[]) = {"both", "ver", "hor", ਨਿਰਾ};
ਸਥਿਰ ਅੱਖਰ *(p_buftype_ਮੁੱਲਾਂ[]) = {"nofile", "nowrite", "quickfix", "help", "terminal", "acwrite", "prompt", "popup", ਨਿਰਾ};
ਸਥਿਰ ਅੱਖਰ *(p_bufhidden_ਮੁੱਲਾਂ[]) = {"hide", "unload", "delete", "wipe", ਨਿਰਾ};
ਸਥਿਰ ਅੱਖਰ *(p_bs_ਮੁੱਲਾਂ[]) = {"indent", "eol", "start", "nostop", ਨਿਰਾ};
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
ਸਥਿਰ ਅੱਖਰ *(p_fdm_ਮੁੱਲਾਂ[]) = {"manual", "expr", "marker", "indent", "syntax",
# ifdef FEAT_DIFF
				"diff",
# endif
				ਨਿਰਾ};
ਸਥਿਰ ਅੱਖਰ *(p_fcl_ਮੁੱਲਾਂ[]) = {"all", ਨਿਰਾ};
#ਜੇ_ਅੰਤ
ਸਥਿਰ ਅੱਖਰ *(p_cfc_ਮੁੱਲਾਂ[]) = {"keyword", "files", "whole_line", ਨਿਰਾ};
ਸਥਿਰ ਅੱਖਰ *(p_cot_ਮੁੱਲਾਂ[]) = {"menu", "menuone", "longest", "preview", "popup", "popuphidden", "noinsert", "noselect", "fuzzy", "nosort", "preinsert", "nearest", ਨਿਰਾ};
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ BACKSLASH_ਵਿੱਚ_FILENAME
ਸਥਿਰ ਅੱਖਰ *(p_csl_ਮੁੱਲਾਂ[]) = {"slash", "backslash", ਨਿਰਾ};
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SIGNS
ਸਥਿਰ ਅੱਖਰ *(p_scl_ਮੁੱਲਾਂ[]) = {"yes", "no", "auto", "number", ਨਿਰਾ};
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
ਸਥਿਰ ਅੱਖਰ *(p_trz_ਮੁੱਲਾਂ[]) = {"inband", "sigwinch", "", ਨਿਰਾ};
#ਜੇ_ਅੰਤ
#ਜੇ defined(MSWIN) && defined(FEAT_TERMINAL)
ਸਥਿਰ ਅੱਖਰ *(p_twt_ਮੁੱਲਾਂ[]) = {"winpty", "conpty", "", ਨਿਰਾ};
#ਜੇ_ਅੰਤ
ਸਥਿਰ ਅੱਖਰ *(p_sloc_ਮੁੱਲਾਂ[]) = {"last", "statusline", "tabline", ਨਿਰਾ};
ਸਥਿਰ ਅੱਖਰ *(p_sws_ਮੁੱਲਾਂ[]) = {"fsync", "sync", ਨਿਰਾ};

ਸਥਿਰ ਪੂਰਨ check_opt_ਲੜੀਆਂ(char_u *val, ਅੱਖਰ **ਮੁੱਲਾਂ, ਪੂਰਨ list);
ਸਥਿਰ ਪੂਰਨ opt_ਲੜੀਆਂ_flags(char_u *val, ਅੱਖਰ **ਮੁੱਲਾਂ, ਬਿਨਾਨਿਸ਼ਾਨੀ *flagp, ਪੂਰਨ list);

/*
 * After setting various option values: recompute variables that depend on
 * option values.
 */
    ਖਾਲੀ
didset_string_options(ਖਾਲੀ)
{
    (ਖਾਲੀ)opt_ਲੜੀਆਂ_flags(p_cmp, p_cmp_ਮੁੱਲਾਂ, &cmp_flags, TRUE);
    (ਖਾਲੀ)opt_ਲੜੀਆਂ_flags(p_bkc, p_bkc_ਮੁੱਲਾਂ, &bkc_flags, TRUE);
    (ਖਾਲੀ)opt_ਲੜੀਆਂ_flags(p_bo, p_bo_ਮੁੱਲਾਂ, &bo_flags, TRUE);
    (ਖਾਲੀ)opt_ਲੜੀਆਂ_flags(p_cot, p_cot_ਮੁੱਲਾਂ, &cot_flags, TRUE);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SESSION
    (ਖਾਲੀ)opt_ਲੜੀਆਂ_flags(p_ssop, p_ssop_ਮੁੱਲਾਂ, &ssop_flags, TRUE);
    (ਖਾਲੀ)opt_ਲੜੀਆਂ_flags(p_vop, p_ssop_ਮੁੱਲਾਂ, &vop_flags, TRUE);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
    (ਖਾਲੀ)opt_ਲੜੀਆਂ_flags(p_fdo, p_fdo_ਮੁੱਲਾਂ, &fdo_flags, TRUE);
#ਜੇ_ਅੰਤ
    (ਖਾਲੀ)opt_ਲੜੀਆਂ_flags(p_dy, p_dy_ਮੁੱਲਾਂ, &dy_flags, TRUE);
    (ਖਾਲੀ)opt_ਲੜੀਆਂ_flags(p_jop, p_jop_ਮੁੱਲਾਂ, &jop_flags, TRUE);
    (ਖਾਲੀ)opt_ਲੜੀਆਂ_flags(p_tc, p_tc_ਮੁੱਲਾਂ, &tc_flags, FALSE);
    (ਖਾਲੀ)opt_ਲੜੀਆਂ_flags(p_ve, p_ve_ਮੁੱਲਾਂ, &ve_flags, TRUE);
#ਜੇ defined(UNIX) || defined(VMS)
    (ਖਾਲੀ)opt_ਲੜੀਆਂ_flags(p_ttym, p_ttym_ਮੁੱਲਾਂ, &ttym_flags, FALSE);
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
    (ਖਾਲੀ)opt_ਲੜੀਆਂ_flags(p_toolbar, p_toolbar_ਮੁੱਲਾਂ, &toolbar_flags, TRUE);
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_TOOLBAR) && defined(FEAT_GUI_GTK)
    (ਖਾਲੀ)opt_ਲੜੀਆਂ_flags(p_tbis, p_tbis_ਮੁੱਲਾਂ, &tbis_flags, FALSE);
#ਜੇ_ਅੰਤ
    (ਖਾਲੀ)opt_ਲੜੀਆਂ_flags(p_swb, p_swb_ਮੁੱਲਾਂ, &swb_flags, TRUE);
    (ਖਾਲੀ)opt_ਲੜੀਆਂ_flags(p_tcl, p_tcl_ਮੁੱਲਾਂ, &tcl_flags, TRUE);
}

#ਜੇ defined(FEAT_EVAL)
/*
 * Trigger the OptionSet autocommand.
 * "opt_idx"	is the index of the option being set.
 * "opt_flags"	can be OPT_LOCAL etc.
 * "oldval"	the old value
 *  "oldval_l"  the old local value (only non-NULL if global and local value
 *		are set)
 * "oldval_g"   the old global value (only non-NULL if global and local value
 *		are set)
 * "newval"	the new value
 */
    ਖਾਲੀ
trigger_optionset_string(
	ਪੂਰਨ	opt_idx,
	ਪੂਰਨ	opt_flags,
	char_u  *oldval,
	char_u  *oldval_l,
	char_u  *oldval_g,
	char_u  *newval)
{
    // Don't do this recursively.
    ਜੇ (oldval == ਨਿਰਾ || newval == ਨਿਰਾ
				    || *get_vim_var_str(VV_OPTION_TYPE) != NUL)
	ਵਾਪਸ;

    char_u buf_type[7];
    size_t buf_typelen;
    size_t oldvallen;

    oldvallen = STRLEN(oldval);
    ਸੈੱਟ_vim_var_string(VV_OPTION_OLD, oldval, (ਪੂਰਨ)oldvallen);
    ਸੈੱਟ_vim_var_string(VV_OPTION_NEW, newval, -1);
    buf_typelen = vim_snprintf_safelen((ਅੱਖਰ *)buf_type, ਆਕਾਰ(buf_type),
	"%s", (opt_flags & OPT_LOCAL) ? "local" : "global");
    ਸੈੱਟ_vim_var_string(VV_OPTION_TYPE, buf_type, (ਪੂਰਨ)buf_typelen);
    ਜੇ (opt_flags & OPT_LOCAL)
    {
	ਸੈੱਟ_vim_var_string(VV_OPTION_COMMAND, (char_u *)"setlocal", STRLEN_LITERAL("setlocal"));
	ਸੈੱਟ_vim_var_string(VV_OPTION_OLDLOCAL, oldval, (ਪੂਰਨ)oldvallen);
    }
    ਜੇ (opt_flags & OPT_GLOBAL)
    {
	ਸੈੱਟ_vim_var_string(VV_OPTION_COMMAND, (char_u *)"setglobal", STRLEN_LITERAL("setglobal"));
	ਸੈੱਟ_vim_var_string(VV_OPTION_OLDGLOBAL, oldval, (ਪੂਰਨ)oldvallen);
    }
    ਜੇ ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	ਸੈੱਟ_vim_var_string(VV_OPTION_COMMAND, (char_u *)"set", STRLEN_LITERAL("set"));
	ਸੈੱਟ_vim_var_string(VV_OPTION_OLDLOCAL, oldval_l, -1);
	ਸੈੱਟ_vim_var_string(VV_OPTION_OLDGLOBAL, oldval_g, -1);
    }
    ਜੇ (opt_flags & OPT_MODELINE)
    {
	ਸੈੱਟ_vim_var_string(VV_OPTION_COMMAND, (char_u *)"modeline", STRLEN_LITERAL("modeline"));
	ਸੈੱਟ_vim_var_string(VV_OPTION_OLDLOCAL, oldval, (ਪੂਰਨ)oldvallen);
    }
    apply_autocmds(EVENT_OPTIONSET,
	    get_option_fullname(opt_idx), ਨਿਰਾ, FALSE,
	    ਨਿਰਾ);
    reset_v_option_vars();
}
#ਜੇ_ਅੰਤ

    ਸਥਿਰ ਅੱਖਰ *
illegal_char(ਅੱਖਰ *errbuf, size_t errbuflen, ਪੂਰਨ c)
{
    ਜੇ (errbuf == ਨਿਰਾ)
	ਵਾਪਸ "";
    vim_snprintf(errbuf, errbuflen, _(e_illegal_character_str),
		    (ਅੱਖਰ *)transchar(c));
    ਵਾਪਸ errbuf;
}

    ਸਥਿਰ ਅੱਖਰ *
illegal_char_after_chr(ਅੱਖਰ *errbuf, size_t errbuflen, ਪੂਰਨ c)
{
    ਜੇ (errbuf == ਨਿਰਾ)
	ਵਾਪਸ "";
    vim_snprintf(errbuf, errbuflen, _(e_illegal_character_after_chr), c);
    ਵਾਪਸ errbuf;
}

/*
 * Check string options in a buffer for NULL value.
 */
    ਖਾਲੀ
check_buf_options(buf_T *buf)
{
    check_string_option(&buf->b_p_bh);
    check_string_option(&buf->b_p_bt);
    check_string_option(&buf->b_p_fenc);
    check_string_option(&buf->b_p_ff);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਲੱਭੋ_ID
    check_string_option(&buf->b_p_def);
    check_string_option(&buf->b_p_inc);
# ifdef FEAT_EVAL
    check_string_option(&buf->b_p_inex);
# endif
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_EVAL)
    check_string_option(&buf->b_p_inde);
    check_string_option(&buf->b_p_indk);
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_BEVAL) && defined(FEAT_EVAL)
    check_string_option(&buf->b_p_bexpr);
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_CRYPT)
    check_string_option(&buf->b_p_cm);
#ਜੇ_ਅੰਤ
    check_string_option(&buf->b_p_fp);
#ਜੇ defined(FEAT_EVAL)
    check_string_option(&buf->b_p_fex);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CRYPT
    check_string_option(&buf->b_p_ਕੁੰਜੀ);
#ਜੇ_ਅੰਤ
    check_string_option(&buf->b_p_kp);
    check_string_option(&buf->b_p_mps);
    check_string_option(&buf->b_p_fo);
    check_string_option(&buf->b_p_flp);
    check_string_option(&buf->b_p_isk);
    check_string_option(&buf->b_p_com);
    check_string_option(&buf->b_p_cms);
    check_string_option(&buf->b_p_nf);
    check_string_option(&buf->b_p_qe);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
    check_string_option(&buf->b_p_syn);
    check_string_option(&buf->b_s.b_syn_isk);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SPELL
    check_string_option(&buf->b_s.b_p_spc);
    check_string_option(&buf->b_s.b_p_spf);
    check_string_option(&buf->b_s.b_p_spl);
    check_string_option(&buf->b_s.b_p_spo);
#ਜੇ_ਅੰਤ
    check_string_option(&buf->b_p_sua);
    check_string_option(&buf->b_p_cink);
    check_string_option(&buf->b_p_cino);
    check_string_option(&buf->b_p_cinsd);
    parse_cino(buf);
    check_string_option(&buf->b_p_lop);
    check_string_option(&buf->b_p_ft);
    check_string_option(&buf->b_p_cinw);
    check_string_option(&buf->b_p_cot);
    check_string_option(&buf->b_p_cpt);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_COMPL_FUNC
    check_string_option(&buf->b_p_cfu);
    check_string_option(&buf->b_p_ofu);
    check_string_option(&buf->b_p_tsrfu);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    check_string_option(&buf->b_p_ffu);
    check_string_option(&buf->b_p_tfu);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_KEYMAP
    check_string_option(&buf->b_p_keymap);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_QUICKFIX
    check_string_option(&buf->b_p_gefm);
    check_string_option(&buf->b_p_gp);
    check_string_option(&buf->b_p_mp);
    check_string_option(&buf->b_p_efm);
#ਜੇ_ਅੰਤ
    check_string_option(&buf->b_p_ep);
    check_string_option(&buf->b_p_ਰਸਤਾ);
    check_string_option(&buf->b_p_ਟੈਗ_ਸੂਚੀ);
    check_string_option(&buf->b_p_tc);
    check_string_option(&buf->b_p_dict);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
    check_string_option(&buf->b_p_dia);
#ਜੇ_ਅੰਤ
    check_string_option(&buf->b_p_tsr);
    check_string_option(&buf->b_p_lw);
    check_string_option(&buf->b_p_bkc);
    check_string_option(&buf->b_p_menc);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VARTABS
    check_string_option(&buf->b_p_vsts);
    check_string_option(&buf->b_p_vts);
#ਜੇ_ਅੰਤ
}

/*
 * Free the string allocated for an option.
 * Checks for the string being empty_option. This may happen if we're out of
 * memory, vim_strsave() returned NULL, which was replaced by empty_option by
 * check_options().
 * Does NOT check for P_ALLOCED flag!
 */
    ਖਾਲੀ
free_string_option(char_u *p)
{
    ਜੇ (p != ਖਾਲੀ_ਥਾਵਾਂ_option)
	vim_free(p);
}

    ਖਾਲੀ
clear_string_option(char_u **pp)
{
    ਜੇ (*pp != ਖਾਲੀ_ਥਾਵਾਂ_option)
	vim_free(*pp);
    *pp = ਖਾਲੀ_ਥਾਵਾਂ_option;
}

    ਖਾਲੀ
check_string_option(char_u **pp)
{
    ਜੇ (*pp == ਨਿਰਾ)
	*pp = ਖਾਲੀ_ਥਾਵਾਂ_option;
}

/*
 * Set global value for string option when it's a local option.
 */
    ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_string_option_global(
    ਪੂਰਨ		opt_idx,	// option index
    char_u	**varp)		// pointer to option variable
{
    char_u	**p, *s;

    // the global value is always allocated
    ਜੇ (is_ਖਿੜਕੀ_local_option(opt_idx))
	p = (char_u **)GLOBAL_WO(varp);
    ਨਹੀਂਤਾਂ
	p = (char_u **)get_option_var(opt_idx);
    ਜੇ (!is_global_option(opt_idx)
	    && p != varp
	    && (s = vim_strsave(*varp)) != ਨਿਰਾ)
    {
	free_string_option(*p);
	*p = s;
    }
}

/*
 * Set a string option to a new value (without checking the effect).
 * The string is copied into allocated memory.
 * if ("opt_idx" == -1) "name" is used, otherwise "opt_idx" is used.
 * When "set_sid" is zero set the scriptID to current_sctx.sc_sid.  When
 * "set_sid" is SID_NONE don't set the scriptID.  Otherwise set the scriptID to
 * "set_sid".
 */
    ਖਾਲੀ
ਸੈੱਟ_string_option_ਸਿੱਧਾ(
    char_u	*name,
    ਪੂਰਨ		opt_idx,
    char_u	*val,
    ਪੂਰਨ		opt_flags,	// OPT_FREE, OPT_LOCAL and/or OPT_GLOBAL
    ਪੂਰਨ		ਸੈੱਟ_sid ਅਣਵਰਤਿਆ)
{
    char_u	*s;
    char_u	**varp;
    ਪੂਰਨ		both = (opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0;
    ਪੂਰਨ		idx = opt_idx;

    ਜੇ (idx == -1)		// use name
    {
	idx = findoption(name);
	ਜੇ (idx < 0)	// not found (should not happen)
	{
	    semsg(_(e_ਅੰਦਰੂਨੀ_error_str), "set_string_option_direct()");
	    siemsg("For option %s", name);
	    ਵਾਪਸ;
	}
    }

    ਜੇ (is_hidden_option(idx))		// can't set hidden option
	ਵਾਪਸ;

    s = vim_strsave(val);
    ਜੇ (s == ਨਿਰਾ)
	ਵਾਪਸ;

    varp = (char_u **)get_option_varp_scope(idx,
	    both ? OPT_LOCAL : opt_flags);
    ਜੇ ((opt_flags & OPT_FREE) && (get_option_flags(idx) & P_ALLOCED))
	free_string_option(*varp);
    *varp = s;

    // For buffer/window local option may also set the global value.
    ਜੇ (both)
	ਸੈੱਟ_string_option_global(idx, varp);

    ਸੈੱਟ_option_flag(idx, P_ALLOCED);

    // When setting both values of a global option with a local value,
    // make the local value empty, so that the global value is used.
    ਜੇ (is_global_local_option(idx) && both)
    {
	free_string_option(*varp);
	*varp = ਖਾਲੀ_ਥਾਵਾਂ_option;
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਜੇ (ਸੈੱਟ_sid != SID_NONE)
    {
	sctx_T script_ctx;

	ਜੇ (ਸੈੱਟ_sid == 0)
	    script_ctx = current_sctx;
	ਨਹੀਂਤਾਂ
	{
	    script_ctx.sc_sid = ਸੈੱਟ_sid;
	    script_ctx.sc_seq = 0;
	    script_ctx.sc_lnum = 0;
	    script_ctx.sc_ਸੰਸਕਰਣ = 1;
	}
	ਸੈੱਟ_option_sctx_idx(idx, opt_flags, script_ctx);
    }
#ਜੇ_ਅੰਤ
}

/*
 * Like set_string_option_direct(), but for a window-local option in "wp".
 * Blocks autocommands to avoid the old curwin becoming invalid.
 */
    ਖਾਲੀ
ਸੈੱਟ_string_option_ਸਿੱਧਾ_ਵਿੱਚ_win(
	win_T		*wp,
	char_u		*name,
	ਪੂਰਨ		opt_idx,
	char_u		*val,
	ਪੂਰਨ		opt_flags,
	ਪੂਰਨ		ਸੈੱਟ_sid)
{
    win_T	*save_curwin = curwin;

    ਖੰਡ_autocmds();
    curwin = wp;
    curbuf = curwin->w_ਬਫਰ;
    ਸੈੱਟ_string_option_ਸਿੱਧਾ(name, opt_idx, val, opt_flags, ਸੈੱਟ_sid);
    curwin = save_curwin;
    curbuf = curwin->w_ਬਫਰ;
    unblock_autocmds();
}

#ਜੇ defined(FEAT_PROP_POPUP)
/*
 * Like set_string_option_direct(), but for a buffer-local option in "buf".
 * Blocks autocommands to avoid the old curbuf becoming invalid.
 */
    ਖਾਲੀ
ਸੈੱਟ_string_option_ਸਿੱਧਾ_ਵਿੱਚ_buf(
	buf_T		*buf,
	char_u		*name,
	ਪੂਰਨ		opt_idx,
	char_u		*val,
	ਪੂਰਨ		opt_flags,
	ਪੂਰਨ		ਸੈੱਟ_sid)
{
    buf_T	*save_curbuf = curbuf;

    ਖੰਡ_autocmds();
    curbuf = buf;
    curwin->w_ਬਫਰ = curbuf;
    ਸੈੱਟ_string_option_ਸਿੱਧਾ(name, opt_idx, val, opt_flags, ਸੈੱਟ_sid);
    curbuf = save_curbuf;
    curwin->w_ਬਫਰ = curbuf;
    unblock_autocmds();
}
#ਜੇ_ਅੰਤ

/*
 * Set a string option to a new value, and handle the effects.
 *
 * Returns NULL on success or an untranslated error message on error.
 */
    ਅੱਖਰ *
ਸੈੱਟ_string_option(
    ਪੂਰਨ		opt_idx,
    char_u	*ਮੁੱਲ,
    ਪੂਰਨ		opt_flags,	// OPT_LOCAL and/or OPT_GLOBAL
    ਅੱਖਰ	*errbuf,
    size_t	errbuflen)
{
    char_u	*s;
    char_u	**varp;
    char_u	*oldval;
#ਜੇ defined(FEAT_EVAL)
    char_u	*oldval_l = ਨਿਰਾ;
    char_u	*oldval_g = ਨਿਰਾ;
    char_u	*saved_oldval = ਨਿਰਾ;
    char_u	*saved_oldval_l = ਨਿਰਾ;
    char_u	*saved_oldval_g = ਨਿਰਾ;
    char_u	*saved_newval = ਨਿਰਾ;
#ਜੇ_ਅੰਤ
    ਅੱਖਰ	*errmsg = ਨਿਰਾ;
    ਪੂਰਨ		ਮੁੱਲ_checked = FALSE;

    ਜੇ (is_hidden_option(opt_idx))	// don't set hidden option
	ਵਾਪਸ ਨਿਰਾ;

    s = vim_strsave(ਮੁੱਲ == ਨਿਰਾ ? (char_u *)"" : ਮੁੱਲ);
    ਜੇ (s == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    varp = (char_u **)get_option_varp_scope(opt_idx,
	    (opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0
	    ? (is_global_local_option(opt_idx)
		? OPT_GLOBAL : OPT_LOCAL)
	    : opt_flags);
    oldval = *varp;
#ਜੇ defined(FEAT_EVAL)
    ਜੇ ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	oldval_l = *(char_u **)get_option_varp_scope(opt_idx, OPT_LOCAL);
	oldval_g = *(char_u **)get_option_varp_scope(opt_idx, OPT_GLOBAL);
    }
#ਜੇ_ਅੰਤ
    *varp = s;

#ਜੇ defined(FEAT_EVAL)
    ਜੇ (!starting
# ifdef FEAT_CRYPT
	    && !is_crypt_ਕੁੰਜੀ_option(opt_idx)
# endif
       )
    {
	ਜੇ (oldval_l != ਨਿਰਾ)
	    saved_oldval_l = vim_strsave(oldval_l);
	ਜੇ (oldval_g != ਨਿਰਾ)
	    saved_oldval_g = vim_strsave(oldval_g);
	saved_oldval = vim_strsave(oldval);
	saved_newval = vim_strsave(s);
    }
#ਜੇ_ਅੰਤ
    ਜੇ ((errmsg = did_ਸੈੱਟ_string_option(opt_idx, varp, oldval, ਮੁੱਲ, errbuf,
		    errbuflen, opt_flags, OP_NONE, &ਮੁੱਲ_checked)) == ਨਿਰਾ)
	did_ਸੈੱਟ_option(opt_idx, opt_flags, TRUE, ਮੁੱਲ_checked);

#ਜੇ defined(FEAT_EVAL)
    // call autocommand after handling side effects
    ਜੇ (errmsg == ਨਿਰਾ)
	trigger_optionset_string(opt_idx, opt_flags,
		saved_oldval, saved_oldval_l,
		saved_oldval_g, saved_newval);
    vim_free(saved_oldval);
    vim_free(saved_oldval_l);
    vim_free(saved_oldval_g);
    vim_free(saved_newval);
#ਜੇ_ਅੰਤ
    ਵਾਪਸ errmsg;
}

/*
 * Return TRUE if "val" is a valid 'filetype' name.
 * Also used for 'syntax' and 'keymap'.
 */
    ਸਥਿਰ ਪੂਰਨ
valid_filetype(char_u *val)
{
    ਵਾਪਸ valid_name(val, ".-_");
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_STL_OPT
/*
 * Check validity of options with the 'statusline' format.
 * Return an untranslated error message or NULL.
 */
    ਸਥਿਰ ਅੱਖਰ *
check_stl_option(char_u *s)
{
    ਪੂਰਨ		groupdepth = 0;
    ਸਥਿਰ ਅੱਖਰ errbuf[ERR_BUFLEN];
    ਪੂਰਨ		errbuflen = ERR_BUFLEN;

    ਜਦੋਂ (*s)
    {
	// Check for valid keys after % sequences
	ਜਦੋਂ (*s && *s != '%')
	    s++;
	ਜੇ (!*s)
	    ਰੋਕੋ;
	s++;
	ਜੇ (*s == STL_CLICKFUNC)
	{
	    ਜੇ (s[1] == ']')
	    {
		// %[] - end click region
		s += 2;
		ਜਾਰੀ;
	    }
	    ਜੇ (ASCII_ISALPHA(s[1]) || s[1] == '_')
	    {
		// %[FuncName] - start click region
		char_u *rb = vim_strchr(s + 2, ']');
		ਜੇ (rb != ਨਿਰਾ)
		{
		    s = rb + 1;
		    ਜਾਰੀ;
		}
	    }
	    // Bare %[ is invalid
	    ਵਾਪਸ illegal_char(errbuf, errbuflen, *s);
	}
	ਜੇ (*s == STL_LINEBREAK)
	{
	    // Plain %@ - line break
	    s++;
	    ਜਾਰੀ;
	}
	ਜੇ (*s == '%' || *s == STL_TRUNCMARK || *s == STL_SEPARATE)
	{
	    s++;
	    ਜਾਰੀ;
	}
	ਜੇ (*s == ')')
	{
	    s++;
	    ਜੇ (--groupdepth < 0)
		ਰੋਕੋ;
	    ਜਾਰੀ;
	}
	ਜੇ (*s == '-')
	    s++;
	ਜਦੋਂ (VIM_ISDIGIT(*s))
	    s++;
	ਜੇ (*s == STL_USER_HL)
	    ਜਾਰੀ;
	ਜੇ (*s == STL_CLICKFUNC)
	{
	    // %N[FuncName] or %N[]
	    ਜੇ (s[1] == ']')
	    {
		s += 2;
		ਜਾਰੀ;
	    }
	    ਜੇ (ASCII_ISALPHA(s[1]) || s[1] == '_')
	    {
		char_u *rb = vim_strchr(s + 2, ']');
		ਜੇ (rb != ਨਿਰਾ)
		{
		    s = rb + 1;
		    ਜਾਰੀ;
		}
	    }
	    // Bare %N[ is invalid
	    ਵਾਪਸ illegal_char(errbuf, errbuflen, *s);
	}
	ਜੇ (*s == STL_LINEBREAK)
	{
	    // %N@ - line break
	    s++;
	    ਜਾਰੀ;
	}
	ਜੇ (*s == '.')
	{
	    s++;
	    ਜਦੋਂ (*s && VIM_ISDIGIT(*s))
		s++;
	}
	ਜੇ (*s == '(')
	{
	    groupdepth++;
	    ਜਾਰੀ;
	}
	ਜੇ (vim_strchr(STL_ALL, *s) == ਨਿਰਾ)
	{
	    ਵਾਪਸ illegal_char(errbuf, errbuflen, *s);
	}
	ਜੇ (*s == '{')
	{
	    ਪੂਰਨ reevaluate = (*++s == '%');

	    ਜੇ (reevaluate && *++s == '}')
		// "}" is not allowed immediately after "%{%"
		ਵਾਪਸ illegal_char(errbuf, errbuflen, '}');
	    ਜਦੋਂ ((*s != '}' || (reevaluate && s[-1] != '%')) && *s)
		s++;
	    ਜੇ (*s != '}')
		ਵਾਪਸ e_unclosed_ਵਿਅੰਜਕ_sequence;
	}
    }
    ਜੇ (groupdepth != 0)
	ਵਾਪਸ e_unbalanced_groups;
    ਵਾਪਸ ਨਿਰਾ;
}
#ਜੇ_ਅੰਤ

/*
 * Check for a "normal" directory or file name in some options.  Disallow a
 * path separator (slash and/or backslash), wildcards and characters that are
 * often illegal in a file name. Be more permissive if "secure" is off.
 */
    ਸਥਿਰ ਪੂਰਨ
check_illegal_ਰਸਤਾ_ਨਾਂ(ਪੂਰਨ opt_idx, char_u **varp)
{
    ਵਾਪਸ (((get_option_flags(opt_idx) & P_NFNAME)
		&& vim_strpbrk(*varp, (char_u *)(secure
			? "/\\*?[|;&<>\r\n" : "/\\*?[<>\r\n")) != ਨਿਰਾ)
	    || ((get_option_flags(opt_idx) & P_NDNAME)
		&& vim_strpbrk(*varp, (char_u *)"*?[|;&<>\r\n") != ਨਿਰਾ));
}

/*
 * An option that accepts a list of flags is changed.
 * e.g. 'viewoptions', 'switchbuf', 'casemap', etc.
 */
    ਸਥਿਰ ਅੱਖਰ *
did_ਸੈੱਟ_opt_flags(char_u *val, ਅੱਖਰ **ਮੁੱਲਾਂ, ਬਿਨਾਨਿਸ਼ਾਨੀ *flagp, ਪੂਰਨ list)
{
    ਜੇ (opt_ਲੜੀਆਂ_flags(val, ਮੁੱਲਾਂ, flagp, list) == FAIL)
	ਵਾਪਸ e_invalid_ਦਲੀਲ;

    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * An option that accepts a list of string values is changed.
 * e.g. 'nrformats', 'scrollopt', 'wildoptions', etc.
 */
    ਸਥਿਰ ਅੱਖਰ *
did_ਸੈੱਟ_opt_ਲੜੀਆਂ(char_u *val, ਅੱਖਰ **ਮੁੱਲਾਂ, ਪੂਰਨ list)
{
    ਵਾਪਸ did_ਸੈੱਟ_opt_flags(val, ਮੁੱਲਾਂ, ਨਿਰਾ, list);
}

/*
 * An option which is a list of flags is set.  Valid values are in 'flags'.
 */
    ਸਥਿਰ ਅੱਖਰ *
did_ਸੈੱਟ_option_listflag(
	char_u *val,
	char_u *flags,
	ਅੱਖਰ *errbuf,
	size_t errbuflen)
{
    char_u	*s;

    ਲਈ (s = val; *s; ++s)
	ਜੇ (vim_strchr(flags, *s) == ਨਿਰਾ)
	    ਵਾਪਸ illegal_char(errbuf, errbuflen, *s);

    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Expand an option that accepts a list of fixed string values with known
 * number of items.
 */
    ਸਥਿਰ ਪੂਰਨ
expand_ਸੈੱਟ_opt_string(
	optexpand_T *ਦਲੀਲਾਂ,
	ਅੱਖਰ **ਮੁੱਲਾਂ,
	size_t ਅੰਕ_ਮੁੱਲਾਂ,
	ਪੂਰਨ *ਅੰਕ_Matches,
	char_u ***matches)
{
    char_u	*p;
    regmatch_T	*regmatch = ਦਲੀਲਾਂ->oe_regmatch;
    ਪੂਰਨ		include_orig_val = ਦਲੀਲਾਂ->oe_include_orig_val;
    char_u	*option_val = ਦਲੀਲਾਂ->oe_opt_ਮੁੱਲ;

    // Assume numValues is small since they are fixed enums, so just allocate
    // upfront instead of needing two passes to calculate output size.
    *matches = ALLOC_MULT(char_u *, ਅੰਕ_ਮੁੱਲਾਂ + 1);
    ਜੇ (*matches == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    ਪੂਰਨ ਗਿਣਤੀ_2 = 0;

    ਜੇ (include_orig_val && *option_val != NUL)
    {
	p = vim_strsave(option_val);
	ਜੇ (p == ਨਿਰਾ)
	{
	    VIM_CLEAR(*matches);
	    ਵਾਪਸ FAIL;
	}
	(*matches)[ਗਿਣਤੀ_2++] = p;
    }

    ਲਈ (ਅੱਖਰ **val = ਮੁੱਲਾਂ; *val != ਨਿਰਾ; val++)
    {
	ਜੇ (include_orig_val && *option_val != NUL)
	{
	    ਜੇ (STRCMP((char_u*)*val, option_val) == 0)
		ਜਾਰੀ;
	}
	ਜੇ (vim_regexec(regmatch, (char_u*)(*val), (colnr_T)0))
	{
	    p = vim_strsave((char_u*)*val);
	    ਜੇ (p == ਨਿਰਾ)
	    {
		ਜੇ (ਗਿਣਤੀ_2 == 0)
		{
		    VIM_CLEAR(*matches);
		    ਵਾਪਸ FAIL;
		}
		ਨਹੀਂਤਾਂ
		    ਰੋਕੋ;
	    }
	    (*matches)[ਗਿਣਤੀ_2++] = p;
	}
    }
    ਜੇ (ਗਿਣਤੀ_2 == 0)
    {
	VIM_CLEAR(*matches);
	ਵਾਪਸ FAIL;
    }
    *ਅੰਕ_Matches = ਗਿਣਤੀ_2;
    ਵਾਪਸ OK;
}

ਸਥਿਰ char_u *ਸੈੱਟ_opt_callback_orig_option = ਨਿਰਾ;
ਸਥਿਰ char_u *((*ਸੈੱਟ_opt_callback_func)(expand_T *, ਪੂਰਨ));

/*
 * Callback used by expand_set_opt_generic to also include the original value
 * as the first item.
 */
    ਸਥਿਰ char_u *
expand_ਸੈੱਟ_opt_generic_cb(expand_T *xp, ਪੂਰਨ idx)
{
    ਜੇ (idx == 0)
    {
	ਜੇ (ਸੈੱਟ_opt_callback_orig_option != ਨਿਰਾ)
	    ਵਾਪਸ ਸੈੱਟ_opt_callback_orig_option;
	ਨਹੀਂਤਾਂ
	    ਵਾਪਸ (char_u *)""; // empty strings are ignored
    }
    ਵਾਪਸ ਸੈੱਟ_opt_callback_func(xp, idx - 1);
}

/*
 * Expand an option with a callback that iterates through a list of possible
 * names using an index.
 */
    ਸਥਿਰ ਪੂਰਨ
expand_ਸੈੱਟ_opt_generic(
	optexpand_T *ਦਲੀਲਾਂ,
	char_u *((*func)(expand_T *, ਪੂਰਨ)),
	ਪੂਰਨ *ਅੰਕ_Matches,
	char_u ***matches)
{
    ਪੂਰਨ ret;

    ਸੈੱਟ_opt_callback_orig_option = ਦਲੀਲਾਂ->oe_include_orig_val ?
	ਦਲੀਲਾਂ->oe_opt_ਮੁੱਲ : ਨਿਰਾ;
    ਸੈੱਟ_opt_callback_func = func;

    ret = ExpandGeneric(
	    (char_u*)"", // not using fuzzy as currently EXPAND_STRING_SETTING doesn't use it
	    ਦਲੀਲਾਂ->oe_xp,
	    ਦਲੀਲਾਂ->oe_regmatch,
	    matches,
	    ਅੰਕ_Matches,
	    expand_ਸੈੱਟ_opt_generic_cb,
	    FALSE);

    ਸੈੱਟ_opt_callback_orig_option = ਨਿਰਾ;
    ਸੈੱਟ_opt_callback_func = ਨਿਰਾ;
    ਵਾਪਸ ret;
}

#ਜੇ defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
ਸਥਿਰ garray_T *expand_cb_ga;
ਸਥਿਰ optexpand_T *expand_cb_ਦਲੀਲਾਂ;

/*
 * Callback provided to a function in expand_set_opt_callback. Will perform
 * regex matching against the value and add to the list.
 *
 * Returns OK usually. Returns FAIL if it failed to allocate memory, and the
 * caller should terminate the enumeration.
 */
    ਸਥਿਰ ਪੂਰਨ
expand_ਸੈੱਟ_opt_callback_cb(char_u *val)
{
    regmatch_T	*regmatch = expand_cb_ਦਲੀਲਾਂ->oe_regmatch;
    expand_T	*xp = expand_cb_ਦਲੀਲਾਂ->oe_xp;
    garray_T	*ga = expand_cb_ga;
    char_u	*str;

    ਜੇ (val == ਨਿਰਾ || *val == NUL)
	ਵਾਪਸ OK;

    ਜੇ (xp->xp_ਨਮੂਨਾ[0] != NUL &&
	    !vim_regexec(regmatch, val, (colnr_T)0))
	ਵਾਪਸ OK;

    str = vim_strsave_escaped(val, (char_u *)" \t\\");

    ਜੇ (str == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    ਜੇ (ga_grow(ga, 1) == FAIL)
    {
	vim_free(str);
	ਵਾਪਸ FAIL;
    }

    ((char_u **)ga->ga_ਅੰਕੜੇ)[ga->ga_len] = str;
    ++ga->ga_len;
    ਵਾਪਸ OK;
}

/*
 * Expand an option with a provided function that takes a callback. The
 * function will enumerate through all options and call the callback to add it
 * to the list.
 *
 * "func" is the enumerator function that will generate the list of options.
 * "func_params" is a single parameter that will be passed to func.
 */
    ਸਥਿਰ ਪੂਰਨ
expand_ਸੈੱਟ_opt_callback(
	optexpand_T *ਦਲੀਲਾਂ,
	ਖਾਲੀ (*func)(optexpand_T *, ਖਾਲੀ* params, ਪੂਰਨ (*cb)(char_u *val)),
	ਖਾਲੀ *func_params,
	ਪੂਰਨ *ਅੰਕ_Matches,
	char_u ***matches)
{
    garray_T	ga;
    ਪੂਰਨ		include_orig_val = ਦਲੀਲਾਂ->oe_include_orig_val;
    char_u	*option_val = ਦਲੀਲਾਂ->oe_opt_ਮੁੱਲ;

    ga_init2(&ga, ਆਕਾਰ(ਅੱਖਰ *), 30);

    ਜੇ (include_orig_val && *option_val != NUL)
    {
	char_u *p = vim_strsave(option_val);
	ਜੇ (p == ਨਿਰਾ)
	    ਵਾਪਸ FAIL;
	ਜੇ (ga_grow(&ga, 1) == FAIL)
	{
	    vim_free(p);
	    ਵਾਪਸ FAIL;
	}
	((char_u **)ga.ga_ਅੰਕੜੇ)[ga.ga_len] = p;
	++ga.ga_len;
    }

    expand_cb_ga = &ga;
    expand_cb_ਦਲੀਲਾਂ = ਦਲੀਲਾਂ;

    func(ਦਲੀਲਾਂ, func_params, expand_ਸੈੱਟ_opt_callback_cb);

    expand_cb_ga = ਨਿਰਾ;
    expand_cb_ਦਲੀਲਾਂ = ਨਿਰਾ;

    *matches = ga.ga_ਅੰਕੜੇ;
    *ਅੰਕ_Matches = ga.ga_len;
    ਵਾਪਸ OK;
}
#ਜੇ_ਅੰਤ

/*
 * Expand an option which is a list of flags.
 */
    ਸਥਿਰ ਪੂਰਨ
expand_ਸੈੱਟ_opt_listflag(
	optexpand_T *ਦਲੀਲਾਂ,
	char_u *flags,
	ਪੂਰਨ *ਅੰਕ_Matches,
	char_u ***matches)
{
    char_u  *p;
    char_u  *option_val = ਦਲੀਲਾਂ->oe_opt_ਮੁੱਲ;
    char_u  *cmdline_val = ਦਲੀਲਾਂ->oe_ਸੈੱਟ_arg;
    ਪੂਰਨ	    append = ਦਲੀਲਾਂ->oe_append;
    ਪੂਰਨ	    include_orig_val = ਦਲੀਲਾਂ->oe_include_orig_val && (*option_val != NUL);

    size_t ਅੰਕ_flags = STRLEN(flags);

    // Assume we only have small number of flags, so just allocate max size.
    *matches = ALLOC_MULT(char_u *, ਅੰਕ_flags + 1);
    ਜੇ (*matches == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    ਪੂਰਨ ਗਿਣਤੀ_2 = 0;

    ਜੇ (include_orig_val)
    {
	p = vim_strsave(option_val);
	ਜੇ (p == ਨਿਰਾ)
	{
	    VIM_CLEAR(*matches);
	    ਵਾਪਸ FAIL;
	}
	(*matches)[ਗਿਣਤੀ_2++] = p;
    }

    ਲਈ (char_u *flag = flags; *flag != NUL; flag++)
    {
	ਜੇ (append && vim_strchr(option_val, *flag) != ਨਿਰਾ)
	    ਜਾਰੀ;

	ਜੇ (vim_strchr(cmdline_val, *flag) == ਨਿਰਾ)
	{
	    ਜੇ (include_orig_val
		    && option_val[1] == NUL
		    && *flag == option_val[0])
	    {
		// This value is already used as the first choice as it's the
		// existing flag. Just skip it to avoid duplicate.
		ਜਾਰੀ;
	    }
	    p = vim_strnsave(flag, 1);
	    ਜੇ (p == ਨਿਰਾ)
	    {
		ਜੇ (ਗਿਣਤੀ_2 == 0)
		{
		    VIM_CLEAR(*matches);
		    ਵਾਪਸ FAIL;
		}
		ਨਹੀਂਤਾਂ
		    ਰੋਕੋ;
	    }
	    (*matches)[ਗਿਣਤੀ_2++] = p;
	}
    }

    ਜੇ (ਗਿਣਤੀ_2 == 0)
    {
	VIM_CLEAR(*matches);
	ਵਾਪਸ FAIL;
    }
    *ਅੰਕ_Matches = ਗਿਣਤੀ_2;
    ਵਾਪਸ OK;
}

/*
 * The 'ambiwidth' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_ambiwidth(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਜੇ (check_opt_ਲੜੀਆਂ(p_ambw, p_ambw_ਮੁੱਲਾਂ, FALSE) != OK)
	ਵਾਪਸ e_invalid_ਦਲੀਲ;

    ਵਾਪਸ check_chars_options();
}

// "name" must be a string literal, the length is computed at compile time.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ completing_ਮੁੱਲ_for_subopt(ਦਲੀਲਾਂ, name) \
	  completing_ਮੁੱਲ_for_subopt_len(ਦਲੀਲਾਂ, name, (ਪੂਰਨ)STRLEN_LITERAL(name))

/*
 * Return true when completing the value of the sub-option "name" with length
 * "len", e.g. the value after "close:" in 'completepopup'.
 */
    ਸਥਿਰ bool
completing_ਮੁੱਲ_for_subopt_len(optexpand_T *ਦਲੀਲਾਂ, ਅੱਖਰ *name, ਪੂਰਨ len)
{
    char_u  *colon = ਦਲੀਲਾਂ->oe_xp->xp_ਨਮੂਨਾ - 1;
    ਪੂਰਨ	    off = (ਪੂਰਨ)(colon - ਦਲੀਲਾਂ->oe_ਸੈੱਟ_arg);

    ਜੇ (off < len)
	ਵਾਪਸ false;
    // The name must follow a comma when it does not start the option value.
    ਜੇ (off > len && *(colon - len - 1) != ',')
	ਵਾਪਸ false;

    ਵਾਪਸ STRNCMP(colon - len, name, len) == 0;
}

    ਪੂਰਨ
expand_ਸੈੱਟ_ambiwidth(optexpand_T *ਦਲੀਲਾਂ, ਪੂਰਨ *ਅੰਕ_Matches, char_u ***matches)
{
    ਵਾਪਸ expand_ਸੈੱਟ_opt_string(
	    ਦਲੀਲਾਂ,
	    p_ambw_ਮੁੱਲਾਂ,
	    ARRAY_ਲੰਮਾਈ(p_ambw_ਮੁੱਲਾਂ) - 1,
	    ਅੰਕ_Matches,
	    matches);
}

/*
 * The 'background' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_background(optset_T *ਦਲੀਲਾਂ)
{
    ਜੇ (check_opt_ਲੜੀਆਂ(p_bg, p_bg_ਮੁੱਲਾਂ, FALSE) == FAIL)
	ਵਾਪਸ e_invalid_ਦਲੀਲ;

    ਜੇ (ਦਲੀਲਾਂ->os_oldval.string != ਨਿਰਾ && ਦਲੀਲਾਂ->os_oldval.string[0] == *p_bg)
	// Value was not changed
	ਵਾਪਸ ਨਿਰਾ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਪੂਰਨ dark = (*p_bg == 'd');
#ਜੇ_ਅੰਤ

    init_highlight(FALSE, FALSE);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਜੇ (dark != (*p_bg == 'd')
	    && get_var_ਮੁੱਲ((char_u *)"g:colors_name") != ਨਿਰਾ)
    {
	// The color scheme must have set 'background' back to another
	// value, that's not what we want here.  Disable the color
	// scheme and set the colors again.
	do_unlet((char_u *)"g:colors_name", TRUE);
	free_string_option(p_bg);
	p_bg = vim_strsave((char_u *)(dark ? "dark" : "light"));
	check_string_option(&p_bg);
	init_highlight(FALSE, FALSE);
    }
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
    ਪਦ_update_colors_all();
#ਜੇ_ਅੰਤ

    ਵਾਪਸ ਨਿਰਾ;
}

    ਪੂਰਨ
expand_ਸੈੱਟ_background(optexpand_T *ਦਲੀਲਾਂ, ਪੂਰਨ *ਅੰਕ_Matches, char_u ***matches)
{
    ਵਾਪਸ expand_ਸੈੱਟ_opt_string(
	    ਦਲੀਲਾਂ,
	    p_bg_ਮੁੱਲਾਂ,
	    ARRAY_ਲੰਮਾਈ(p_bg_ਮੁੱਲਾਂ) - 1,
	    ਅੰਕ_Matches,
	    matches);
}

/*
 * The 'backspace' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_backspace(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਜੇ (VIM_ISDIGIT(*p_bs))
    {
	ਜੇ (*p_bs > '3' || p_bs[1] != NUL)
	    ਵਾਪਸ e_invalid_ਦਲੀਲ;
    }
    ਨਹੀਂਤਾਂ ਜੇ (check_opt_ਲੜੀਆਂ(p_bs, p_bs_ਮੁੱਲਾਂ, TRUE) != OK)
	ਵਾਪਸ e_invalid_ਦਲੀਲ;

    ਵਾਪਸ ਨਿਰਾ;
}

    ਪੂਰਨ
expand_ਸੈੱਟ_backspace(optexpand_T *ਦਲੀਲਾਂ, ਪੂਰਨ *ਅੰਕ_Matches, char_u ***matches)
{
    ਵਾਪਸ expand_ਸੈੱਟ_opt_string(
	    ਦਲੀਲਾਂ,
	    p_bs_ਮੁੱਲਾਂ,
	    ARRAY_ਲੰਮਾਈ(p_bs_ਮੁੱਲਾਂ) - 1,
	    ਅੰਕ_Matches,
	    matches);
}

/*
 * The 'backupcopy' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_backupcopy(optset_T *ਦਲੀਲਾਂ)
{
    char_u		*bkc = p_bkc;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ	*flags = &bkc_flags;
    ਅੱਖਰ		*errmsg = ਨਿਰਾ;

    ਜੇ (ਦਲੀਲਾਂ->os_flags & OPT_LOCAL)
    {
	bkc = curbuf->b_p_bkc;
	flags = &curbuf->b_bkc_flags;
    }
    ਨਹੀਂਤਾਂ ਜੇ (!(ਦਲੀਲਾਂ->os_flags & OPT_GLOBAL))
	// When using :set, clear the local flags.
	curbuf->b_bkc_flags = 0;

    ਜੇ ((ਦਲੀਲਾਂ->os_flags & OPT_LOCAL) && *bkc == NUL)
	// make the local value empty: use the global value
	*flags = 0;
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (opt_ਲੜੀਆਂ_flags(bkc, p_bkc_ਮੁੱਲਾਂ, flags, TRUE) != OK)
	    errmsg = e_invalid_ਦਲੀਲ;
	ਜੇ ((((ਪੂਰਨ)*flags & BKC_AUTO) != 0)
		+ (((ਪੂਰਨ)*flags & BKC_YES) != 0)
		+ (((ਪੂਰਨ)*flags & BKC_NO) != 0) != 1)
	{
	    // Must have exactly one of "auto", "yes"  and "no".
	    (ਖਾਲੀ)opt_ਲੜੀਆਂ_flags(ਦਲੀਲਾਂ->os_oldval.string, p_bkc_ਮੁੱਲਾਂ,
								  flags, TRUE);
	    errmsg = e_invalid_ਦਲੀਲ;
	}
    }

    ਵਾਪਸ errmsg;
}

    ਪੂਰਨ
expand_ਸੈੱਟ_backupcopy(optexpand_T *ਦਲੀਲਾਂ, ਪੂਰਨ *ਅੰਕ_Matches, char_u ***matches)
{
    ਵਾਪਸ expand_ਸੈੱਟ_opt_string(
	    ਦਲੀਲਾਂ,
	    p_bkc_ਮੁੱਲਾਂ,
	    ARRAY_ਲੰਮਾਈ(p_bkc_ਮੁੱਲਾਂ) - 1,
	    ਅੰਕ_Matches,
	    matches);
}

/*
 * The 'backupext' or the 'patchmode' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_backupext_or_patchmode(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਜੇ (STRCMP(*p_bex == '.' ? p_bex + 1 : p_bex,
		*p_pm == '.' ? p_pm + 1 : p_pm) == 0)
	ਵਾਪਸ e_backupext_and_patchmode_are_equal;

    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * The 'belloff' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_belloff(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ did_ਸੈੱਟ_opt_flags(p_bo, p_bo_ਮੁੱਲਾਂ, &bo_flags, TRUE);
}

    ਪੂਰਨ
expand_ਸੈੱਟ_belloff(optexpand_T *ਦਲੀਲਾਂ, ਪੂਰਨ *ਅੰਕ_Matches, char_u ***matches)
{
    ਵਾਪਸ expand_ਸੈੱਟ_opt_string(
	    ਦਲੀਲਾਂ,
	    p_bo_ਮੁੱਲਾਂ,
	    ARRAY_ਲੰਮਾਈ(p_bo_ਮੁੱਲਾਂ) - 1,
	    ਅੰਕ_Matches,
	    matches);
}

#ਜੇ defined(FEAT_LINEBREAK)
/*
 * The 'breakat' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_breakat(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    char_u	*p;
    ਪੂਰਨ		i;

    ਲਈ (i = 0; i < 256; i++)
	breakat_flags[i] = FALSE;

    ਜੇ (p_breakat != ਨਿਰਾ)
	ਲਈ (p = p_breakat; *p; p++)
	    breakat_flags[*p] = TRUE;

    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * The 'breakindentopt' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_breakindentopt(optset_T *ਦਲੀਲਾਂ)
{
    char_u	**varp = (char_u **)ਦਲੀਲਾਂ->os_varp;

    ਜੇ (briopt_check(*varp, varp == &curwin->w_p_briopt ? curwin : ਨਿਰਾ)
								      == FAIL)
	ਵਾਪਸ e_invalid_ਦਲੀਲ;

    // list setting requires a redraw
    ਜੇ (varp == &curwin->w_p_briopt && curwin->w_briopt_list)
	redraw_all_later(UPD_NOT_VALID);

    ਵਾਪਸ ਨਿਰਾ;
}

    ਪੂਰਨ
expand_ਸੈੱਟ_breakindentopt(
    optexpand_T		*ਦਲੀਲਾਂ,
    ਪੂਰਨ			*ਅੰਕ_Matches,
    char_u		***matches)
{
    ਵਾਪਸ expand_ਸੈੱਟ_opt_string(
	    ਦਲੀਲਾਂ,
	    p_briopt_ਮੁੱਲਾਂ,
	    ARRAY_ਲੰਮਾਈ(p_briopt_ਮੁੱਲਾਂ) - 1,
	    ਅੰਕ_Matches,
	    matches);
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_BROWSE)
/*
 * The 'browsedir' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_browsedir(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਜੇ (check_opt_ਲੜੀਆਂ(p_bsdir, p_bsdir_ਮੁੱਲਾਂ, FALSE) != OK
	    && !mch_isdir(p_bsdir))
	ਵਾਪਸ e_invalid_ਦਲੀਲ;

    ਵਾਪਸ ਨਿਰਾ;
}

    ਪੂਰਨ
expand_ਸੈੱਟ_browsedir(optexpand_T *ਦਲੀਲਾਂ, ਪੂਰਨ *ਅੰਕ_Matches, char_u ***matches)
{
    ਵਾਪਸ expand_ਸੈੱਟ_opt_string(
	    ਦਲੀਲਾਂ,
	    p_bsdir_ਮੁੱਲਾਂ,
	    ARRAY_ਲੰਮਾਈ(p_bsdir_ਮੁੱਲਾਂ) - 1,
	    ਅੰਕ_Matches,
	    matches);
}
#ਜੇ_ਅੰਤ

/*
 * The 'bufhidden' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_bufhidden(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ did_ਸੈੱਟ_opt_ਲੜੀਆਂ(curbuf->b_p_bh, p_bufhidden_ਮੁੱਲਾਂ, FALSE);
}

    ਪੂਰਨ
expand_ਸੈੱਟ_bufhidden(optexpand_T *ਦਲੀਲਾਂ, ਪੂਰਨ *ਅੰਕ_Matches, char_u ***matches)
{
    ਵਾਪਸ expand_ਸੈੱਟ_opt_string(
	    ਦਲੀਲਾਂ,
	    p_bufhidden_ਮੁੱਲਾਂ,
	    ARRAY_ਲੰਮਾਈ(p_bufhidden_ਮੁੱਲਾਂ) - 1,
	    ਅੰਕ_Matches,
	    matches);
}

/*
 * The 'buftype' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_buftype(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਜੇ (check_opt_ਲੜੀਆਂ(curbuf->b_p_bt, p_buftype_ਮੁੱਲਾਂ, FALSE) != OK)
	ਵਾਪਸ e_invalid_ਦਲੀਲ;

    ਜੇ (curwin->w_ਸਥਿਤੀ_ਉਚਾਈ)
    {
	curwin->w_redr_ਸਥਿਤੀ = true;
	redraw_later(UPD_VALID);
    }
    curbuf->b_help = (curbuf->b_p_bt[0] == 'h');
    redraw_titles();

    ਵਾਪਸ ਨਿਰਾ;
}

    ਪੂਰਨ
expand_ਸੈੱਟ_buftype(optexpand_T *ਦਲੀਲਾਂ, ਪੂਰਨ *ਅੰਕ_Matches, char_u ***matches)
{
    ਵਾਪਸ expand_ਸੈੱਟ_opt_string(
	    ਦਲੀਲਾਂ,
	    p_buftype_ਮੁੱਲਾਂ,
	    ARRAY_ਲੰਮਾਈ(p_buftype_ਮੁੱਲਾਂ) - 1,
	    ਅੰਕ_Matches,
	    matches);
}

/*
 * The 'casemap' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_casemap(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ did_ਸੈੱਟ_opt_flags(p_cmp, p_cmp_ਮੁੱਲਾਂ, &cmp_flags, TRUE);
}

    ਪੂਰਨ
expand_ਸੈੱਟ_casemap(optexpand_T *ਦਲੀਲਾਂ, ਪੂਰਨ *ਅੰਕ_Matches, char_u ***matches)
{
    ਵਾਪਸ expand_ਸੈੱਟ_opt_string(
	    ਦਲੀਲਾਂ,
	    p_cmp_ਮੁੱਲਾਂ,
	    ARRAY_ਲੰਮਾਈ(p_cmp_ਮੁੱਲਾਂ) - 1,
	    ਅੰਕ_Matches,
	    matches);
}

#ਜੇ defined(HAVE_CLIPMETHOD)
    ਪੂਰਨ
expand_ਸੈੱਟ_clipboard(optexpand_T *ਦਲੀਲਾਂ, ਪੂਰਨ *ਅੰਕ_Matches, char_u ***matches)
{
    ਵਾਪਸ expand_ਸੈੱਟ_opt_string(
	    ਦਲੀਲਾਂ,
	    p_cb_ਮੁੱਲਾਂ,
	    ARRAY_ਲੰਮਾਈ(p_cb_ਮੁੱਲਾਂ) - 1,
	    ਅੰਕ_Matches,
	    matches);
}
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_CLIPMETHOD
    ਅੱਖਰ *
did_ਸੈੱਟ_clipmethod(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ ਚੁਣੋ_clipmethod();
}

    ਪੂਰਨ
expand_ਸੈੱਟ_clipmethod(optexpand_T *ਦਲੀਲਾਂ, ਪੂਰਨ *ਅੰਕ_Matches, char_u ***matches)
{
    // We want to expand using the predefined clipmethod values + clipboard
    // provider names.
    ਪੂਰਨ		ਨਤੀਜਾ;
    ਅੱਖਰ	**ਮੁੱਲਾਂ;
    ਪੂਰਨ		ਗਿਣਤੀ_2, pos = 0, ਸ਼ੁਰੂ = 0;
# ifdef FEAT_EVAL
    dict_T	*providers = get_vim_var_dict(VV_CLIPPROVIDERS);
# ਨਹੀਂਤਾਂ
    dict_T	*providers = ਨਿਰਾ;
# endif
    hashtab_T	*ht = providers == ਨਿਰਾ ? ਨਿਰਾ : &providers->dv_hashtab;

    ਗਿਣਤੀ_2 = (ht == ਨਿਰਾ ? 0 : ht->ht_ਵਰਤਿਆ);
# ifdef FEAT_WAYLAND_CLIPBOARD
    ਗਿਣਤੀ_2++;
    ਸ਼ੁਰੂ++;
# endif
# ifdef FEAT_XCLIPBOARD
    ਗਿਣਤੀ_2++;
    ਸ਼ੁਰੂ++;
# endif
    ਮੁੱਲਾਂ = ALLOC_MULT(ਅੱਖਰ *, ਗਿਣਤੀ_2 + 1); // Add NULL terminator too

    ਜੇ (ਮੁੱਲਾਂ == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

# ifdef FEAT_WAYLAND_CLIPBOARD
    ਮੁੱਲਾਂ[pos++] = "wayland";
# endif
# ifdef FEAT_XCLIPBOARD
    ਮੁੱਲਾਂ[pos++] = "x11";
# endif

    ਜੇ (ht != ਨਿਰਾ)
	ਲਈ (long_u i = 0; i < ht->ht_ਨਕਾਬ + 1; i++)
	{
	    hashitem_T	*hi = ht->ht_array + i;

	    ਜੇ (!HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
		ਮੁੱਲਾਂ[pos++] = (ਅੱਖਰ *)vim_strsave(hi->hi_ਕੁੰਜੀ);
	}
    ਮੁੱਲਾਂ[pos++] = ਨਿਰਾ;

    ਨਤੀਜਾ = expand_ਸੈੱਟ_opt_string(
	    ਦਲੀਲਾਂ,
	    ਮੁੱਲਾਂ,
	    ਗਿਣਤੀ_2,
	    ਅੰਕ_Matches,
	    matches);

    ਲਈ (ਪੂਰਨ i = ਸ਼ੁਰੂ; i < ਗਿਣਤੀ_2; i++)
	vim_free(ਮੁੱਲਾਂ[i]);
    vim_free(ਮੁੱਲਾਂ);

    ਵਾਪਸ ਨਤੀਜਾ;
}
#ਜੇ_ਅੰਤ

/*
 * The global 'listchars' or 'fillchars' option is changed.
 */
    ਸਥਿਰ ਅੱਖਰ *
did_ਸੈੱਟ_global_listfillchars(char_u *val, ਪੂਰਨ opt_lcs, ਪੂਰਨ opt_flags,
						ਅੱਖਰ *errbuf, size_t errbuflen)
{
    ਅੱਖਰ	*errmsg = ਨਿਰਾ;
    char_u	**local_ptr = opt_lcs ? &curwin->w_p_lcs : &curwin->w_p_fcs;

    // only apply the global value to "curwin" when it does not have a
    // local value
    ਜੇ (opt_lcs)
	errmsg = ਸੈੱਟ_listchars_option(curwin, val,
		**local_ptr == NUL || !(opt_flags & OPT_GLOBAL),
							    errbuf, errbuflen);
    ਨਹੀਂਤਾਂ
	errmsg = ਸੈੱਟ_fillchars_option(curwin, val,
		**local_ptr == NUL || !(opt_flags & OPT_GLOBAL),
							    errbuf, errbuflen);
    ਜੇ (errmsg != ਨਿਰਾ)
	ਵਾਪਸ errmsg;

    tabpage_T	*tp;
    win_T	*wp;

    // If the current window is set to use the global
    // 'listchars'/'fillchars' value, clear the window-local value.
    ਜੇ (!(opt_flags & OPT_GLOBAL))
	clear_string_option(local_ptr);
    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	// If the current window has a local value need to apply it
	// again, it was changed when setting the global value.
	// If no error was returned above, we don't expect an error
	// here, so ignore the return value.
	ਜੇ (opt_lcs)
	{
	    ਜੇ (*wp->w_p_lcs == NUL)
		(ਖਾਲੀ)ਸੈੱਟ_listchars_option(wp, wp->w_p_lcs, TRUE, ਨਿਰਾ, 0);
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (*wp->w_p_fcs == NUL)
		(ਖਾਲੀ)ਸੈੱਟ_fillchars_option(wp, wp->w_p_fcs, TRUE, ਨਿਰਾ, 0);
	}
    }

    redraw_all_later(UPD_NOT_VALID);

    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * The 'fillchars' option or the 'listchars' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_chars_option(optset_T *ਦਲੀਲਾਂ)
{
    char_u **varp = (char_u **)ਦਲੀਲਾਂ->os_varp;
    ਅੱਖਰ *errmsg = ਨਿਰਾ;

    ਜੇ (   varp == &p_lcs		// global 'listchars'
	|| varp == &p_fcs)		// global 'fillchars'
	errmsg = did_ਸੈੱਟ_global_listfillchars(*varp, varp == &p_lcs,
			  ਦਲੀਲਾਂ->os_flags, ਦਲੀਲਾਂ->os_errbuf, ਦਲੀਲਾਂ->os_errbuflen);
    ਨਹੀਂਤਾਂ ਜੇ (varp == &curwin->w_p_lcs)	// local 'listchars'
	errmsg = ਸੈੱਟ_listchars_option(curwin, *varp, TRUE,
					  ਦਲੀਲਾਂ->os_errbuf, ਦਲੀਲਾਂ->os_errbuflen);
    ਨਹੀਂਤਾਂ ਜੇ (varp == &curwin->w_p_fcs)	// local 'fillchars'
	errmsg = ਸੈੱਟ_fillchars_option(curwin, *varp, TRUE,
					  ਦਲੀਲਾਂ->os_errbuf, ਦਲੀਲਾਂ->os_errbuflen);

    ਵਾਪਸ errmsg;
}

/*
 * Expand 'fillchars' or 'listchars' option value.
 */
    ਪੂਰਨ
expand_ਸੈੱਟ_chars_option(optexpand_T *ਦਲੀਲਾਂ, ਪੂਰਨ *ਅੰਕ_Matches, char_u ***matches)
{
    char_u **varp = (char_u **)ਦਲੀਲਾਂ->oe_varp;
    ਪੂਰਨ is_lcs = (varp == &p_lcs || varp == &curwin->w_p_lcs);
    ਵਾਪਸ expand_ਸੈੱਟ_opt_generic(
	    ਦਲੀਲਾਂ,
	    is_lcs ? get_listchars_name : get_fillchars_name,
	    ਅੰਕ_Matches,
	    matches);
}

/*
 * The 'cinoptions' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_cinoptions(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    // TODO: recognize errors
    parse_cino(curbuf);

    ਵਾਪਸ ਨਿਰਾ;
}

#ਜੇ defined(FEAT_SYN_HL)
/*
 * The 'colorcolumn' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_colorcolumn(optset_T *ਦਲੀਲਾਂ)
{
    char_u	**varp = (char_u **)ਦਲੀਲਾਂ->os_varp;

    ਵਾਪਸ check_colorcolumn(*varp, varp == &curwin->w_p_cc ? curwin : ਨਿਰਾ);
}
#ਜੇ_ਅੰਤ

/*
 * The 'comments' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_comments(optset_T *ਦਲੀਲਾਂ)
{
    char_u	**varp = (char_u **)ਦਲੀਲਾਂ->os_varp;
    char_u	*s;
    ਅੱਖਰ	*errmsg = ਨਿਰਾ;

    ਲਈ (s = *varp; *s; )
    {
	ਜਦੋਂ (*s && *s != ':')
	{
	    ਜੇ (vim_strchr((char_u *)COM_ALL, *s) == ਨਿਰਾ
		    && !VIM_ISDIGIT(*s) && *s != '-')
	    {
		errmsg = illegal_char(ਦਲੀਲਾਂ->os_errbuf, ਦਲੀਲਾਂ->os_errbuflen, *s);
		ਰੋਕੋ;
	    }
	    ++s;
	}
	ਜੇ (*s++ == NUL)
	    errmsg = e_missing_colon;
	ਨਹੀਂਤਾਂ ਜੇ (*s == ',' || *s == NUL)
	    errmsg = e_zero_ਲੰਮਾਈ_string;
	ਜੇ (errmsg != ਨਿਰਾ)
	    ਰੋਕੋ;
	ਜਦੋਂ (*s && *s != ',')
	{
	    ਜੇ (*s == '\\' && s[1] != NUL)
		++s;
	    ++s;
	}
	s = skip_ਮੰਜ਼ਿਲ_option_part(s);
    }

    ਵਾਪਸ errmsg;
}

/*
 * The 'commentstring' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_commentstring(optset_T *ਦਲੀਲਾਂ)
{
    char_u	**varp = (char_u **)ਦਲੀਲਾਂ->os_varp;

    ਜੇ (**varp != NUL && ਲੜੀ_ਖੋਜ((ਅੱਖਰ *)*varp, "%s") == ਨਿਰਾ)
	ਵਾਪਸ e_commentstring_must_be_ਖਾਲੀ_ਥਾਵਾਂ_or_contain_str;

    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Check if value for 'complete' is valid when 'complete' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_complete(optset_T *ਦਲੀਲਾਂ)
{
    char_u	**varp = (char_u **)ਦਲੀਲਾਂ->os_varp;
    char_u	*p, *t;
    char_u	ਬਫਰ[LSIZE];
    char_u	*buf_ptr;
    char_u	char_before = NUL;
    ਪੂਰਨ		escape;

    ਲਈ (p = *varp; *p; )
    {
	vim_memset(ਬਫਰ, 0, LSIZE);
	buf_ptr = ਬਫਰ;
	escape = 0;

	// Extract substring while handling escaped commas
	ਜਦੋਂ (*p && (*p != ',' || escape) && buf_ptr < (ਬਫਰ + LSIZE - 1))
	{
	    ਜੇ (*p == '\\' && *(p + 1) == ',')
	    {
		escape = 1;  // Mark escape mode
		p++;         // Skip '\'
	    }
	    ਨਹੀਂਤਾਂ
	    {
		escape = 0;
		*buf_ptr++ = *p;
	    }
	    p++;
	}
	*buf_ptr = NUL;

	ਜੇ (vim_strchr((char_u *)".wbuksid]tUFo", *ਬਫਰ) == ਨਿਰਾ)
	    ਵਾਪਸ illegal_char(ਦਲੀਲਾਂ->os_errbuf, ਦਲੀਲਾਂ->os_errbuflen, *ਬਫਰ);

	ਜੇ (vim_strchr((char_u *)"ksF", *ਬਫਰ) == ਨਿਰਾ && *(ਬਫਰ + 1) != NUL
		&& *(ਬਫਰ + 1) != '^')
	    char_before = *ਬਫਰ;
	ਨਹੀਂਤਾਂ
	{
	    // Test for a number after '^'
	    ਜੇ ((t = vim_strchr(ਬਫਰ, '^')) != ਨਿਰਾ)
	    {
		*t++ = NUL;
		ਜੇ (!*t)
		    char_before = '^';
		ਨਹੀਂਤਾਂ
		{
		    ਲਈ (; *t; t++)
		    {
			ਜੇ (!vim_isdigit(*t))
			{
			    char_before = '^';
			    ਰੋਕੋ;
			}
		    }
		}
	    }
	}
	ਜੇ (char_before != NUL)
	    ਵਾਪਸ illegal_char_after_chr(ਦਲੀਲਾਂ->os_errbuf, ਦਲੀਲਾਂ->os_errbuflen,
		    char_before);
	// Skip comma and spaces
	ਜਦੋਂ (*p == ',' || *p == ' ')
	    p++;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_COMPL_FUNC
    ਜੇ (ਸੈੱਟ_cpt_callbacks(ਦਲੀਲਾਂ) != OK)
	ਵਾਪਸ illegal_char_after_chr(ਦਲੀਲਾਂ->os_errbuf, ਦਲੀਲਾਂ->os_errbuflen,
		'F');
#ਜੇ_ਅੰਤ
    ਵਾਪਸ ਨਿਰਾ;
}

    ਪੂਰਨ
expand_ਸੈੱਟ_complete(optexpand_T *ਦਲੀਲਾਂ, ਪੂਰਨ *ਅੰਕ_Matches, char_u ***matches)
{
    ਸਥਿਰ ਅੱਖਰ *(p_cpt_ਮੁੱਲਾਂ[]) = {
	".", "w", "b", "u", "k", "kspell", "s", "i", "d", "]", "t", "U", "F", "o",
	ਨਿਰਾ};
    ਵਾਪਸ expand_ਸੈੱਟ_opt_string(
	    ਦਲੀਲਾਂ,
	    p_cpt_ਮੁੱਲਾਂ,
	    ARRAY_ਲੰਮਾਈ(p_cpt_ਮੁੱਲਾਂ) - 1,
	    ਅੰਕ_Matches,
	    matches);
}

/*
 * The 'completeopt' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_completeopt(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    char_u	*cot = p_cot;
    ਬਿਨਾਨਿਸ਼ਾਨੀ	*flags = &cot_flags;

    ਜੇ (ਦਲੀਲਾਂ->os_flags & OPT_LOCAL)
    {
	cot = curbuf->b_p_cot;
	flags = &curbuf->b_cot_flags;
    }
    ਨਹੀਂਤਾਂ ਜੇ (!(ਦਲੀਲਾਂ->os_flags & OPT_GLOBAL))
	// When using :set, clear the local flags.
	curbuf->b_cot_flags = 0;

    ਜੇ (opt_ਲੜੀਆਂ_flags(cot, p_cot_ਮੁੱਲਾਂ, flags, TRUE) != OK)
	ਵਾਪਸ e_invalid_ਦਲੀਲ;

    ਵਾਪਸ ਨਿਰਾ;
}

    ਪੂਰਨ
expand_ਸੈੱਟ_completeopt(optexpand_T *ਦਲੀਲਾਂ, ਪੂਰਨ *ਅੰਕ_Matches, char_u ***matches)
{
    ਵਾਪਸ expand_ਸੈੱਟ_opt_string(
	    ਦਲੀਲਾਂ,
	    p_cot_ਮੁੱਲਾਂ,
	    ARRAY_ਲੰਮਾਈ(p_cot_ਮੁੱਲਾਂ) - 1,
	    ਅੰਕ_Matches,
	    matches);
}

/*
 * The 'completefuzzycollect' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_completefuzzycollect(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਜੇ (opt_ਲੜੀਆਂ_flags(p_cfc, p_cfc_ਮੁੱਲਾਂ, &cfc_flags, TRUE) != OK)
	ਵਾਪਸ e_invalid_ਦਲੀਲ;
    ਵਾਪਸ ਨਿਰਾ;
}

    ਪੂਰਨ
expand_ਸੈੱਟ_completefuzzycollect(
	optexpand_T *ਦਲੀਲਾਂ,
	ਪੂਰਨ *ਅੰਕ_Matches,
	char_u ***matches)
{
    ਵਾਪਸ expand_ਸੈੱਟ_opt_string(
	    ਦਲੀਲਾਂ,
	    p_cfc_ਮੁੱਲਾਂ,
	    ARRAY_ਲੰਮਾਈ(p_cfc_ਮੁੱਲਾਂ) - 1,
	    ਅੰਕ_Matches,
	    matches);
}

/*
 * The 'completeitemalign' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_completeitemalign(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    char_u	*p = p_cia;
    ਬਿਨਾਨਿਸ਼ਾਨੀ	new_cia_flags = 0;
    ਪੂਰਨ		ਵੇਖਿਆ[3] = { FALSE, FALSE, FALSE };
    ਪੂਰਨ		ਗਿਣਤੀ_2 = 0;
    char_u	buf[10];

    ਜਦੋਂ (*p)
    {
	copy_option_part(&p, buf, ਆਕਾਰ(buf), ",");
	ਜੇ (ਗਿਣਤੀ_2 >= 3)
	    ਵਾਪਸ e_invalid_ਦਲੀਲ;

	ਜੇ (STRCMP(buf, "abbr") == 0)
	{
	    ਜੇ (ਵੇਖਿਆ[CPT_ABBR])
		ਵਾਪਸ e_invalid_ਦਲੀਲ;
	    new_cia_flags = new_cia_flags * 10 + CPT_ABBR;
	    ਵੇਖਿਆ[CPT_ABBR] = TRUE;
	    ਗਿਣਤੀ_2++;
	}
	ਨਹੀਂਤਾਂ ਜੇ (STRCMP(buf, "kind") == 0)
	{
	    ਜੇ (ਵੇਖਿਆ[CPT_ਕਿਸਮ])
		ਵਾਪਸ e_invalid_ਦਲੀਲ;
	    new_cia_flags = new_cia_flags * 10 + CPT_ਕਿਸਮ;
	    ਵੇਖਿਆ[CPT_ਕਿਸਮ] = TRUE;
	    ਗਿਣਤੀ_2++;
	}
	ਨਹੀਂਤਾਂ ਜੇ (STRCMP(buf, "menu") == 0)
	{
	    ਜੇ (ਵੇਖਿਆ[CPT_MENU])
		ਵਾਪਸ e_invalid_ਦਲੀਲ;
	    new_cia_flags = new_cia_flags * 10 + CPT_MENU;
	    ਵੇਖਿਆ[CPT_MENU] = TRUE;
	    ਗਿਣਤੀ_2++;
	}
	ਨਹੀਂਤਾਂ
	    ਵਾਪਸ e_invalid_ਦਲੀਲ;
    }
    ਜੇ (new_cia_flags == 0 || ਗਿਣਤੀ_2 != 3)
	ਵਾਪਸ e_invalid_ਦਲੀਲ;

    cia_flags = new_cia_flags;
    ਵਾਪਸ ਨਿਰਾ;
}

#ਜੇ defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
/*
 * The 'completepopup' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_completepopup(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਜੇ (parse_completepopup(ਨਿਰਾ) == FAIL)
	ਵਾਪਸ e_invalid_ਦਲੀਲ;

    popup_close_info();
    ਵਾਪਸ ਨਿਰਾ;
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(BACKSLASH_ਵਿੱਚ_FILENAME)
/*
 * The 'completeslash' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_completeslash(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਜੇ (check_opt_ਲੜੀਆਂ(p_csl, p_csl_ਮੁੱਲਾਂ, FALSE) != OK
	    || check_opt_ਲੜੀਆਂ(curbuf->b_p_csl, p_csl_ਮੁੱਲਾਂ, FALSE) != OK)
	ਵਾਪਸ e_invalid_ਦਲੀਲ;

    ਵਾਪਸ ਨਿਰਾ;
}

    ਪੂਰਨ
expand_ਸੈੱਟ_completeslash(optexpand_T *ਦਲੀਲਾਂ, ਪੂਰਨ *ਅੰਕ_Matches, char_u ***matches)
{
    ਵਾਪਸ expand_ਸੈੱਟ_opt_string(
	    ਦਲੀਲਾਂ,
	    p_csl_ਮੁੱਲਾਂ,
	    ARRAY_ਲੰਮਾਈ(p_csl_ਮੁੱਲਾਂ) - 1,
	    ਅੰਕ_Matches,
	    matches);
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_CONCEAL)
/*
 * The 'concealcursor' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_concealcursor(optset_T *ਦਲੀਲਾਂ)
{
    char_u	**varp = (char_u **)ਦਲੀਲਾਂ->os_varp;

    ਵਾਪਸ did_ਸੈੱਟ_option_listflag(*varp, (char_u *)COCU_ALL, ਦਲੀਲਾਂ->os_errbuf,
		    ਦਲੀਲਾਂ->os_errbuflen);
}

    ਪੂਰਨ
expand_ਸੈੱਟ_concealcursor(optexpand_T *ਦਲੀਲਾਂ, ਪੂਰਨ *ਅੰਕ_Matches, char_u ***matches)
{
    ਵਾਪਸ expand_ਸੈੱਟ_opt_listflag(ਦਲੀਲਾਂ, (char_u*)COCU_ALL, ਅੰਕ_Matches, matches);
}
#ਜੇ_ਅੰਤ

/*
 * The 'cpoptions' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_cpoptions(optset_T *ਦਲੀਲਾਂ)
{
    char_u	**varp = (char_u **)ਦਲੀਲਾਂ->os_varp;

    ਵਾਪਸ did_ਸੈੱਟ_option_listflag(*varp, (char_u *)CPO_ALL, ਦਲੀਲਾਂ->os_errbuf,
		    ਦਲੀਲਾਂ->os_errbuflen);
}

    ਪੂਰਨ
expand_ਸੈੱਟ_cpoptions(optexpand_T *ਦਲੀਲਾਂ, ਪੂਰਨ *ਅੰਕ_Matches, char_u ***matches)
{
    ਵਾਪਸ expand_ਸੈੱਟ_opt_listflag(ਦਲੀਲਾਂ, (char_u*)CPO_ALL, ਅੰਕ_Matches, matches);
}

#ਜੇ defined(FEAT_CRYPT)
/*
 * The 'cryptkey' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_cryptkey(optset_T *ਦਲੀਲਾਂ)
{
    // Make sure the ":set" command doesn't show the new value in the
    // history.
    remove_ਕੁੰਜੀ_from_history();

    ਜੇ (ਦਲੀਲਾਂ->os_op != OP_NONE)
	// Don't allow set+=/-=/^= as they can allow for substring guessing
	ਵਾਪਸ e_invalid_ਦਲੀਲ;

    ਜੇ (STRCMP(curbuf->b_p_ਕੁੰਜੀ, ਦਲੀਲਾਂ->os_oldval.string) != 0)
    {
	// Need to update the swapfile.
	ml_ਸੈੱਟ_crypt_ਕੁੰਜੀ(curbuf, ਦਲੀਲਾਂ->os_oldval.string,
		*curbuf->b_p_cm == NUL ? p_cm : curbuf->b_p_cm);
	ਬਦਲਿਆ_ਅੰਦਰੂਨੀ();
    }
# ifdef FEAT_SODIUM
    ਜੇ (crypt_ਵਿਧੀ_is_sodium(crypt_get_ਵਿਧੀ_nr(curbuf)))
	crypt_sodium_ਤਾਲਾ_ਕੁੰਜੀ(ਦਲੀਲਾਂ->os_newval.string);
# endif

    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * The 'cryptmethod' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_cryptmethod(optset_T *ਦਲੀਲਾਂ)
{
    char_u  *p;
    char_u  *s;

    ਜੇ (ਦਲੀਲਾਂ->os_flags & OPT_LOCAL)
	p = curbuf->b_p_cm;
    ਨਹੀਂਤਾਂ
	p = p_cm;
    ਜੇ (check_opt_ਲੜੀਆਂ(p, p_cm_ਮੁੱਲਾਂ, TRUE) != OK)
	ਵਾਪਸ e_invalid_ਦਲੀਲ;
    ਨਹੀਂਤਾਂ ਜੇ (crypt_self_test() == FAIL)
	ਵਾਪਸ e_invalid_ਦਲੀਲ;

    // When setting the global value to empty, make it "zip".
    ਜੇ (*p_cm == NUL)
    {
	free_string_option(p_cm);
	p_cm = vim_strsave((char_u *)"zip");
    }
    // When using ":set cm=name" the local value is going to be empty.
    // Do that here, otherwise the crypt functions will still use the
    // local value.
    ਜੇ ((ਦਲੀਲਾਂ->os_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	free_string_option(curbuf->b_p_cm);
	curbuf->b_p_cm = ਖਾਲੀ_ਥਾਵਾਂ_option;
    }

    // Need to update the swapfile when the effective method changed.
    // Set "s" to the effective old value, "p" to the effective new
    // method and compare.
    ਜੇ ((ਦਲੀਲਾਂ->os_flags & OPT_LOCAL) && *ਦਲੀਲਾਂ->os_oldval.string == NUL)
	s = p_cm;  // was previously using the global value
    ਨਹੀਂਤਾਂ
	s = ਦਲੀਲਾਂ->os_oldval.string;
    ਜੇ (*curbuf->b_p_cm == NUL)
	p = p_cm;  // is now using the global value
    ਨਹੀਂਤਾਂ
	p = curbuf->b_p_cm;
    ਜੇ (STRCMP(s, p) != 0)
	ml_ਸੈੱਟ_crypt_ਕੁੰਜੀ(curbuf, curbuf->b_p_ਕੁੰਜੀ, s);

    // If the global value changes need to update the swapfile for all
    // buffers using that value.
    ਜੇ ((ਦਲੀਲਾਂ->os_flags & OPT_GLOBAL)
	    && STRCMP(p_cm, ਦਲੀਲਾਂ->os_oldval.string) != 0)
    {
	buf_T	*buf;

	FOR_ALL_BUFFERS(buf)
	    ਜੇ (buf != curbuf && *buf->b_p_cm == NUL)
		ml_ਸੈੱਟ_crypt_ਕੁੰਜੀ(buf, buf->b_p_ਕੁੰਜੀ, ਦਲੀਲਾਂ->os_oldval.string);
    }
    ਵਾਪਸ ਨਿਰਾ;
}

    ਪੂਰਨ
expand_ਸੈੱਟ_cryptmethod(optexpand_T *ਦਲੀਲਾਂ, ਪੂਰਨ *ਅੰਕ_Matches, char_u ***matches)
{
    ਵਾਪਸ expand_ਸੈੱਟ_opt_string(
	    ਦਲੀਲਾਂ,
	    p_cm_ਮੁੱਲਾਂ,
	    ARRAY_ਲੰਮਾਈ(p_cm_ਮੁੱਲਾਂ) - 1,
	    ਅੰਕ_Matches,
	    matches);
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_CSCOPE) && defined(FEAT_QUICKFIX)
/*
 * The 'cscopequickfix' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_cscopequickfix(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    char_u	*p;

    ਜੇ (p_csqf == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    p = p_csqf;
    ਜਦੋਂ (*p != NUL)
    {
	ਜੇ (vim_strchr((char_u *)CSQF_CMDS, *p) == ਨਿਰਾ
		|| p[1] == NUL
		|| vim_strchr((char_u *)CSQF_FLAGS, p[1]) == ਨਿਰਾ
		|| (p[2] != NUL && p[2] != ','))
	    ਵਾਪਸ e_invalid_ਦਲੀਲ;
	ਨਹੀਂਤਾਂ ਜੇ (p[2] == NUL)
	    ਰੋਕੋ;
	ਨਹੀਂਤਾਂ
	    p += 3;
    }

    ਵਾਪਸ ਨਿਰਾ;
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_SYN_HL)
/*
 * The 'cursorlineopt' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_cursorlineopt(optset_T *ਦਲੀਲਾਂ)
{
    char_u	**varp = (char_u **)ਦਲੀਲਾਂ->os_varp;

    // This could be changed to use opt_strings_flags() instead.
    ਜੇ (**varp == NUL || fill_culopt_flags(*varp, curwin) != OK)
	ਵਾਪਸ e_invalid_ਦਲੀਲ;

    ਵਾਪਸ ਨਿਰਾ;
}

    ਪੂਰਨ
expand_ਸੈੱਟ_cursorlineopt(optexpand_T *ਦਲੀਲਾਂ, ਪੂਰਨ *ਅੰਕ_Matches, char_u ***matches)
{
    ਵਾਪਸ expand_ਸੈੱਟ_opt_string(
	    ਦਲੀਲਾਂ,
	    p_culopt_ਮੁੱਲਾਂ,
	    ARRAY_ਲੰਮਾਈ(p_culopt_ਮੁੱਲਾਂ) - 1,
	    ਅੰਕ_Matches,
	    matches);
}
#ਜੇ_ਅੰਤ

/*
 * The 'debug' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_debug(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ did_ਸੈੱਟ_opt_ਲੜੀਆਂ(p_debug, p_debug_ਮੁੱਲਾਂ, TRUE);
}

    ਪੂਰਨ
expand_ਸੈੱਟ_debug(optexpand_T *ਦਲੀਲਾਂ, ਪੂਰਨ *ਅੰਕ_Matches, char_u ***matches)
{
    ਵਾਪਸ expand_ਸੈੱਟ_opt_string(
	    ਦਲੀਲਾਂ,
	    p_debug_ਮੁੱਲਾਂ,
	    ARRAY_ਲੰਮਾਈ(p_debug_ਮੁੱਲਾਂ) - 1,
	    ਅੰਕ_Matches,
	    matches);
}

#ਜੇ defined(FEAT_DIFF)
/*
 * The 'diffanchors' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_diffanchors(optset_T *ਦਲੀਲਾਂ)
{
    ਜੇ (diffanchors_ਬਦਲਿਆ(ਦਲੀਲਾਂ->os_flags & OPT_LOCAL) == FAIL)
	ਵਾਪਸ e_invalid_ਦਲੀਲ;

    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * The 'diffopt' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_diffopt(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਜੇ (diffopt_ਬਦਲਿਆ() == FAIL)
	ਵਾਪਸ e_invalid_ਦਲੀਲ;

    ਵਾਪਸ ਨਿਰਾ;
}

    ਪੂਰਨ
expand_ਸੈੱਟ_diffopt(optexpand_T *ਦਲੀਲਾਂ, ਪੂਰਨ *ਅੰਕ_Matches, char_u ***matches)
{
    expand_T *xp = ਦਲੀਲਾਂ->oe_xp;

    ਜੇ (xp->xp_ਨਮੂਨਾ > ਦਲੀਲਾਂ->oe_ਸੈੱਟ_arg && *(xp->xp_ਨਮੂਨਾ-1) == ':')
    {
	ਜੇ (completing_ਮੁੱਲ_for_subopt(ਦਲੀਲਾਂ, "algorithm"))
	{
	    ਵਾਪਸ expand_ਸੈੱਟ_opt_string(
		    ਦਲੀਲਾਂ,
		    p_dip_algorithm_ਮੁੱਲਾਂ,
		    ARRAY_ਲੰਮਾਈ(p_dip_algorithm_ਮੁੱਲਾਂ) - 1,
		    ਅੰਕ_Matches,
		    matches);
	}
	ਜੇ (completing_ਮੁੱਲ_for_subopt(ਦਲੀਲਾਂ, "inline"))
	{
	    ਵਾਪਸ expand_ਸੈੱਟ_opt_string(
		    ਦਲੀਲਾਂ,
		    p_dip_inline_ਮੁੱਲਾਂ,
		    ARRAY_ਲੰਮਾਈ(p_dip_inline_ਮੁੱਲਾਂ) - 1,
		    ਅੰਕ_Matches,
		    matches);
	}
	ਵਾਪਸ FAIL;
    }

    ਵਾਪਸ expand_ਸੈੱਟ_opt_string(
	    ਦਲੀਲਾਂ,
	    p_dip_ਮੁੱਲਾਂ,
	    ARRAY_ਲੰਮਾਈ(p_dip_ਮੁੱਲਾਂ) - 1,
	    ਅੰਕ_Matches,
	    matches);
}
#ਜੇ_ਅੰਤ

/*
 * The 'display' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_display(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਜੇ (opt_ਲੜੀਆਂ_flags(p_dy, p_dy_ਮੁੱਲਾਂ, &dy_flags, TRUE) != OK)
	ਵਾਪਸ e_invalid_ਦਲੀਲ;

    (ਖਾਲੀ)init_chartab();
    ਵਾਪਸ ਨਿਰਾ;
}

    ਪੂਰਨ
expand_ਸੈੱਟ_display(optexpand_T *ਦਲੀਲਾਂ, ਪੂਰਨ *ਅੰਕ_Matches, char_u ***matches)
{
    ਵਾਪਸ expand_ਸੈੱਟ_opt_string(
	    ਦਲੀਲਾਂ,
	    p_dy_ਮੁੱਲਾਂ,
	    ARRAY_ਲੰਮਾਈ(p_dy_ਮੁੱਲਾਂ) - 1,
	    ਅੰਕ_Matches,
	    matches);
}

/*
 * The 'eadirection' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_eadirection(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ did_ਸੈੱਟ_opt_ਲੜੀਆਂ(p_ead, p_ead_ਮੁੱਲਾਂ, FALSE);
}

    ਪੂਰਨ
expand_ਸੈੱਟ_eadirection(optexpand_T *ਦਲੀਲਾਂ, ਪੂਰਨ *ਅੰਕ_Matches, char_u ***matches)
{
    ਵਾਪਸ expand_ਸੈੱਟ_opt_string(
	    ਦਲੀਲਾਂ,
	    p_ead_ਮੁੱਲਾਂ,
	    ARRAY_ਲੰਮਾਈ(p_ead_ਮੁੱਲਾਂ) - 1,
	    ਅੰਕ_Matches,
	    matches);
}

/*
 * One of the 'encoding', 'fileencoding', 'termencoding' or 'makeencoding'
 * options is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_encoding(optset_T *ਦਲੀਲਾਂ)
{
    char_u	**varp = (char_u **)ਦਲੀਲਾਂ->os_varp;
    char_u	**gvarp;
    ਅੱਖਰ	*errmsg = ਨਿਰਾ;
    char_u	*p;

    // Get the global option to compare with, otherwise we would have to check
    // two values for all local options.
    gvarp = (char_u **)get_option_varp_scope(ਦਲੀਲਾਂ->os_idx, OPT_GLOBAL);

    ਜੇ (gvarp == &p_fenc)
    {
	ਜੇ (!curbuf->b_p_ma && ਦਲੀਲਾਂ->os_flags != OPT_GLOBAL)
	    errmsg = e_cannot_make_changes_modifiable_is_off;
	ਨਹੀਂਤਾਂ ਜੇ (vim_strchr(*varp, ',') != ਨਿਰਾ)
	    // No comma allowed in 'fileencoding'; catches confusing it
	    // with 'fileencodings'.
	    errmsg = e_invalid_ਦਲੀਲ;
	ਨਹੀਂਤਾਂ
	{
	    // May show a "+" in the title now.
	    redraw_titles();
	    // Add 'fileencoding' to the swap file.
	    ml_setflags(curbuf);
	}
    }
    ਜੇ (errmsg == ਨਿਰਾ)
    {
	// canonize the value, so that STRCMP() can be used on it
	p = enc_canonize(*varp);
	ਜੇ (p != ਨਿਰਾ)
	{
	    vim_free(*varp);
	    *varp = p;
	}
	ਜੇ (varp == &p_enc)
	{
	    errmsg = mb_init();
	    redraw_titles();
	}
    }

#ਜੇ defined(FEAT_GUI_GTK)
    ਜੇ (errmsg == ਨਿਰਾ && varp == &p_tenc && gui.ਵਿੱਚ_use)
    {
	// GTK uses only a single encoding, and that is UTF-8.
	ਜੇ (STRCMP(p_tenc, "utf-8") != 0)
	    errmsg = e_cannot_be_ਬਦਲਿਆ_ਵਿੱਚ_gtk_GUI;
    }
#ਜੇ_ਅੰਤ

    ਜੇ (errmsg == ਨਿਰਾ)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_KEYMAP
	// When 'keymap' is used and 'encoding' changes, reload the keymap
	// (with another encoding).
	ਜੇ (varp == &p_enc && *curbuf->b_p_keymap != NUL)
	    (ਖਾਲੀ)keymap_init();
#ਜੇ_ਅੰਤ

	// When 'termencoding' is not empty and 'encoding' changes or when
	// 'termencoding' changes, need to setup for keyboard input and
	// display output conversion.
	ਜੇ (((varp == &p_enc && *p_tenc != NUL) || varp == &p_tenc))
	{
	    ਜੇ (convert_ਤਿਆਰੀ(&ਨਿਵੇਸ਼_conv, p_tenc, p_enc) == FAIL
		    || convert_ਤਿਆਰੀ(&output_conv, p_enc, p_tenc) == FAIL)
	    {
		semsg(_(e_cannot_convert_between_str_and_str),
			p_tenc, p_enc);
		errmsg = e_invalid_ਦਲੀਲ;
	    }
	}

#ਜੇ defined(MSWIN)
	// $HOME, $VIM and $VIMRUNTIME may have characters in active code page.
	ਜੇ (varp == &p_enc)
	{
	    init_homedir();
	    init_vimdir();
	}
#ਜੇ_ਅੰਤ
    }

    ਵਾਪਸ errmsg;
}

    ਪੂਰਨ
expand_ਸੈੱਟ_encoding(optexpand_T *ਦਲੀਲਾਂ, ਪੂਰਨ *ਅੰਕ_Matches, char_u ***matches)
{
    ਵਾਪਸ expand_ਸੈੱਟ_opt_generic(
	    ਦਲੀਲਾਂ,
	    get_encoding_name,
	    ਅੰਕ_Matches,
	    matches);
}

/*
 * The 'eventignore(win)' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_eventignore(optset_T *ਦਲੀਲਾਂ)
{
    char_u	**varp = (char_u **)ਦਲੀਲਾਂ->os_varp;
    char_u	*oldval = ਦਲੀਲਾਂ->os_oldval.string;

    ਜੇ (check_ei(*varp) == FAIL)
	ਵਾਪਸ e_invalid_ਦਲੀਲ;

    ਜੇ (oldval == ਨਿਰਾ || STRCMP(oldval, *varp) == 0)
	ਵਾਪਸ ਨਿਰਾ;

    // Deal with the events that are triggered by comparing against a stored
    // state, with the old value in effect: what happened while an event was
    // ignored must not be reported once it is not ignored anymore, and what
    // happened before must still be reported.
    // Use a copy, the caller owns "oldval" and autocommands may free it.
    char_u	*save_ei = vim_strsave(oldval);
    ਜੇ (save_ei != ਨਿਰਾ)
    {
	char_u	*newval = *varp;
	win_T	*wp = is_ਖਿੜਕੀ_local_option(ਦਲੀਲਾਂ->os_idx) ? curwin : ਨਿਰਾ;

	*varp = save_ei;
	// "varp" points into "wp" for 'eventignorewin'.
	ਜੇ (wp != ਨਿਰਾ)
	    ++wp->w_locked;
	may_trigger_deferred_events();
	ਜੇ (wp != ਨਿਰਾ)
	    --wp->w_locked;

	free_string_option(*varp);
	*varp = newval;
    }

    ਵਾਪਸ ਨਿਰਾ;
}

ਸਥਿਰ ਪੂਰਨ expand_eiw = FALSE;

    ਸਥਿਰ char_u *
get_eventignore_name(expand_T *xp, ਪੂਰਨ idx)
{
    ਪੂਰਨ ਘਟਾਓ = *xp->xp_ਨਮੂਨਾ == '-';
    // 'eventignore(win)' allows special keyword "all" in addition to
    // all event names.
    ਜੇ (!ਘਟਾਓ && idx == 0)
	ਵਾਪਸ (char_u *)"all";

    char_u *name = get_event_name_no_ਸਮੂਹ(xp, idx - 1 + ਘਟਾਓ, expand_eiw);
    ਜੇ (name == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    sprintf((ਅੱਖਰ *)IObuff, "%s%s", ਘਟਾਓ ? "-" : "", name);
    ਵਾਪਸ IObuff;
}

    ਪੂਰਨ
expand_ਸੈੱਟ_eventignore(optexpand_T *ਦਲੀਲਾਂ, ਪੂਰਨ *ਅੰਕ_Matches, char_u ***matches)
{
    expand_eiw = ਦਲੀਲਾਂ->oe_varp != (char_u *)&p_ei;
    ਵਾਪਸ expand_ਸੈੱਟ_opt_generic(
	    ਦਲੀਲਾਂ,
	    get_eventignore_name,
	    ਅੰਕ_Matches,
	    matches);
}

/*
 * The 'fileformat' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_fileformat(optset_T *ਦਲੀਲਾਂ)
{
    char_u	**varp = (char_u **)ਦਲੀਲਾਂ->os_varp;

    ਜੇ (!curbuf->b_p_ma && !(ਦਲੀਲਾਂ->os_flags & OPT_GLOBAL))
	ਵਾਪਸ e_cannot_make_changes_modifiable_is_off;
    ਨਹੀਂਤਾਂ ਜੇ (check_opt_ਲੜੀਆਂ(*varp, p_ff_ਮੁੱਲਾਂ, FALSE) != OK)
	ਵਾਪਸ e_invalid_ਦਲੀਲ;

    // may also change 'textmode'
    ਜੇ (get_fileformat(curbuf) == EOL_DOS)
	curbuf->b_p_tx = TRUE;
    ਨਹੀਂਤਾਂ
	curbuf->b_p_tx = FALSE;
    redraw_titles();
    // update flag in swap file
    ml_setflags(curbuf);
    // Redraw needed when switching to/from "mac": a CR in the text
    // will be displayed differently.
    ਜੇ (get_fileformat(curbuf) == EOL_MAC || *ਦਲੀਲਾਂ->os_oldval.string == 'm')
	redraw_curbuf_later(UPD_NOT_VALID);

    ਵਾਪਸ ਨਿਰਾ;
}

    ਪੂਰਨ
expand_ਸੈੱਟ_fileformat(optexpand_T *ਦਲੀਲਾਂ, ਪੂਰਨ *ਅੰਕ_Matches, char_u ***matches)
{
    ਵਾਪਸ expand_ਸੈੱਟ_opt_string(
	    ਦਲੀਲਾਂ,
	    p_ff_ਮੁੱਲਾਂ,
	    ARRAY_ਲੰਮਾਈ(p_ff_ਮੁੱਲਾਂ) - 1,
	    ਅੰਕ_Matches,
	    matches);
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * fileformat options.
 */
    char_u *
get_fileformat_name(expand_T *xp ਅਣਵਰਤਿਆ, ਪੂਰਨ idx)
{
    ਜੇ (idx >= (ਪੂਰਨ)ARRAY_ਲੰਮਾਈ(p_ff_ਮੁੱਲਾਂ))
	ਵਾਪਸ ਨਿਰਾ;

    ਵਾਪਸ (char_u*)p_ff_ਮੁੱਲਾਂ[idx];
}

/*
 * The 'fileformats' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_fileformats(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਜੇ (check_opt_ਲੜੀਆਂ(p_ffs, p_ff_ਮੁੱਲਾਂ, TRUE) != OK)
	ਵਾਪਸ e_invalid_ਦਲੀਲ;

    // also change 'textauto'
    ਜੇ (*p_ffs == NUL)
	p_ta = FALSE;
    ਨਹੀਂਤਾਂ
	p_ta = TRUE;

    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * The 'filetype' or the 'syntax' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_filetype_or_syntax(optset_T *ਦਲੀਲਾਂ)
{
    char_u	**varp = (char_u **)ਦਲੀਲਾਂ->os_varp;

    ਜੇ (!valid_filetype(*varp))
	ਵਾਪਸ e_invalid_ਦਲੀਲ;

    ਦਲੀਲਾਂ->os_ਮੁੱਲ_ਬਦਲਿਆ = STRCMP(ਦਲੀਲਾਂ->os_oldval.string, *varp) != 0;

    // Since we check the value, there is no need to set P_INSECURE,
    // even when the value comes from a modeline.
    ਦਲੀਲਾਂ->os_ਮੁੱਲ_checked = TRUE;

    ਵਾਪਸ ਨਿਰਾ;
}

#ਜੇ defined(FEAT_FOLDING)
/*
 * The 'foldclose' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_foldclose(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ did_ਸੈੱਟ_opt_ਲੜੀਆਂ(p_fcl, p_fcl_ਮੁੱਲਾਂ, TRUE);
}

    ਪੂਰਨ
expand_ਸੈੱਟ_foldclose(optexpand_T *ਦਲੀਲਾਂ, ਪੂਰਨ *ਅੰਕ_Matches, char_u ***matches)
{
    ਵਾਪਸ expand_ਸੈੱਟ_opt_string(
	    ਦਲੀਲਾਂ,
	    p_fcl_ਮੁੱਲਾਂ,
	    ARRAY_ਲੰਮਾਈ(p_fcl_ਮੁੱਲਾਂ) - 1,
	    ਅੰਕ_Matches,
	    matches);
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_EVAL) && defined(FEAT_FOLDING)
/*
 * The 'foldexpr' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_foldexpr(optset_T *ਦਲੀਲਾਂ)
{
    (ਖਾਲੀ)did_ਸੈੱਟ_optexpr(ਦਲੀਲਾਂ);
    ਜੇ (foldmethod_Is_ਵਿਅੰਜਕ(curwin))
	foldUpdateAll(curwin);
    ਵਾਪਸ ਨਿਰਾ;
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_FOLDING)
/*
 * The 'foldignore' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_foldignore(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਜੇ (foldmethodIsIndent(curwin))
	foldUpdateAll(curwin);
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * The 'foldmarker' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_foldmarker(optset_T *ਦਲੀਲਾਂ)
{
    char_u	**varp = (char_u **)ਦਲੀਲਾਂ->os_varp;
    char_u	*p;

    p = vim_strchr(*varp, ',');
    ਜੇ (p == ਨਿਰਾ)
	ਵਾਪਸ e_comma_required;
    ਨਹੀਂਤਾਂ ਜੇ (p == *varp || p[1] == NUL)
	ਵਾਪਸ e_invalid_ਦਲੀਲ;
    ਨਹੀਂਤਾਂ ਜੇ (foldmethodIsMarker(curwin))
	foldUpdateAll(curwin);

    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * The 'foldmethod' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_foldmethod(optset_T *ਦਲੀਲਾਂ)
{
    char_u	**varp = (char_u **)ਦਲੀਲਾਂ->os_varp;

    ਜੇ (check_opt_ਲੜੀਆਂ(*varp, p_fdm_ਮੁੱਲਾਂ, FALSE) != OK || **varp == NUL)
	ਵਾਪਸ e_invalid_ਦਲੀਲ;

    foldUpdateAll(curwin);
    ਜੇ (foldmethodIsDiff(curwin))
	newFoldLevel();
    ਵਾਪਸ ਨਿਰਾ;
}

    ਪੂਰਨ
expand_ਸੈੱਟ_foldmethod(optexpand_T *ਦਲੀਲਾਂ, ਪੂਰਨ *ਅੰਕ_Matches, char_u ***matches)
{
    ਵਾਪਸ expand_ਸੈੱਟ_opt_string(
	    ਦਲੀਲਾਂ,
	    p_fdm_ਮੁੱਲਾਂ,
	    ARRAY_ਲੰਮਾਈ(p_fdm_ਮੁੱਲਾਂ) - 1,
	    ਅੰਕ_Matches,
	    matches);
}

/*
 * The 'foldopen' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_foldopen(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ did_ਸੈੱਟ_opt_flags(p_fdo, p_fdo_ਮੁੱਲਾਂ, &fdo_flags, TRUE);
}

    ਪੂਰਨ
expand_ਸੈੱਟ_foldopen(optexpand_T *ਦਲੀਲਾਂ, ਪੂਰਨ *ਅੰਕ_Matches, char_u ***matches)
{
    ਵਾਪਸ expand_ਸੈੱਟ_opt_string(
	    ਦਲੀਲਾਂ,
	    p_fdo_ਮੁੱਲਾਂ,
	    ARRAY_ਲੰਮਾਈ(p_fdo_ਮੁੱਲਾਂ) - 1,
	    ਅੰਕ_Matches,
	    matches);
}
#ਜੇ_ਅੰਤ

/*
 * The 'formatoptions' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_formatoptions(optset_T *ਦਲੀਲਾਂ)
{
    char_u	**varp = (char_u **)ਦਲੀਲਾਂ->os_varp;

    ਵਾਪਸ did_ਸੈੱਟ_option_listflag(*varp, (char_u *)FO_ALL, ਦਲੀਲਾਂ->os_errbuf,
		    ਦਲੀਲਾਂ->os_errbuflen);
}

    ਪੂਰਨ
expand_ਸੈੱਟ_formatoptions(optexpand_T *ਦਲੀਲਾਂ, ਪੂਰਨ *ਅੰਕ_Matches, char_u ***matches)
{
    ਵਾਪਸ expand_ਸੈੱਟ_opt_listflag(ਦਲੀਲਾਂ, (char_u*)FO_ALL, ਅੰਕ_Matches, matches);
}

#ਜੇ defined(CURSOR_SHAPE)
/*
 * The 'guicursor' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_guicursor(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ parse_shape_opt(SHAPE_CURSOR);
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_GUI)
/*
 * The 'guifont' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_guifont(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    char_u	*p;
    ਅੱਖਰ	*errmsg = ਨਿਰਾ;

    ਜੇ (gui.ਵਿੱਚ_use)
    {
	p = p_guifont;
# ਜੇ defined(FEAT_GUI_GTK)
	// Put up a font dialog and let the user select a new value.
	// If this is cancelled go back to the old value but don't
	// give an error message.
	ਜੇ (STRCMP(p, "*") == 0)
	{
	    p = gui_mch_font_dialog(ਦਲੀਲਾਂ->os_oldval.string);
	    free_string_option(p_guifont);
	    p_guifont = (p != ਨਿਰਾ) ? p : vim_strsave(ਦਲੀਲਾਂ->os_oldval.string);
	}
# endif
	ਜੇ (p != ਨਿਰਾ && gui_init_font(p_guifont, FALSE) != OK)
	{
# ਜੇ defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_PHOTON)
	    ਜੇ (STRCMP(p_guifont, "*") == 0)
	    {
		// Dialog was cancelled: Keep the old value without giving
		// an error message.
		free_string_option(p_guifont);
		p_guifont = vim_strsave(ਦਲੀਲਾਂ->os_oldval.string);
	    }
	    ਨਹੀਂਤਾਂ
# endif
		errmsg = e_invalid_fonts;
	}
    }

    ਵਾਪਸ errmsg;
}

/*
 * Expand the 'guifont' option. Only when GUI is being used. Each platform has
 * specific behaviors.
 */
    ਪੂਰਨ
expand_ਸੈੱਟ_guifont(optexpand_T *ਦਲੀਲਾਂ, ਪੂਰਨ *ਅੰਕ_Matches, char_u ***matches)
{
    ਜੇ (!gui.ਵਿੱਚ_use)
	ਵਾਪਸ FAIL;

# ਜੇ defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
    char_u **varp = (char_u **)ਦਲੀਲਾਂ->oe_varp;
    ਪੂਰਨ wide = (varp == &p_guifontwide);

    ਵਾਪਸ expand_ਸੈੱਟ_opt_callback(
	    ਦਲੀਲਾਂ, gui_mch_expand_font, &wide, ਅੰਕ_Matches, matches);
# ਨਹੀਂਤਾਂ
    ਵਾਪਸ FAIL;
# endif
}

# ਜੇ defined(FEAT_XFONTSET)
/*
 * The 'guifontset' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_guifontset(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਅੱਖਰ *errmsg = ਨਿਰਾ;

    ਜੇ (STRCMP(p_guifontset, "*") == 0)
	errmsg = e_cant_select_fontset;
    ਨਹੀਂਤਾਂ ਜੇ (gui.ਵਿੱਚ_use && gui_init_font(p_guifontset, TRUE) != OK)
	errmsg = e_invalid_fontset;

    ਵਾਪਸ errmsg;
}
# endif

/*
 * The 'guifontwide' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_guifontwide(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਅੱਖਰ *errmsg = ਨਿਰਾ;

    ਜੇ (STRCMP(p_guifontwide, "*") == 0)
	errmsg = e_cant_select_wide_font;
    ਨਹੀਂਤਾਂ ਜੇ (gui_get_wide_font() == FAIL)
	errmsg = e_invalid_wide_font;

    ਵਾਪਸ errmsg;
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
/*
 * The 'guiligatures' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_guiligatures(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    gui_ਸੈੱਟ_ligatures();
    ਵਾਪਸ ਨਿਰਾ;
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_GUI)
/*
 * The 'guioptions' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_guioptions(optset_T *ਦਲੀਲਾਂ)
{
    char_u	**varp = (char_u **)ਦਲੀਲਾਂ->os_varp;
    ਅੱਖਰ *errmsg;

    errmsg = did_ਸੈੱਟ_option_listflag(*varp, (char_u *)GO_ALL, ਦਲੀਲਾਂ->os_errbuf,
		    ਦਲੀਲਾਂ->os_errbuflen);
    ਜੇ (errmsg != ਨਿਰਾ)
	ਵਾਪਸ errmsg;

    gui_init_which_components(ਦਲੀਲਾਂ->os_oldval.string);
    ਵਾਪਸ ਨਿਰਾ;
}

    ਪੂਰਨ
expand_ਸੈੱਟ_guioptions(optexpand_T *ਦਲੀਲਾਂ, ਪੂਰਨ *ਅੰਕ_Matches, char_u ***matches)
{
    ਵਾਪਸ expand_ਸੈੱਟ_opt_listflag(ਦਲੀਲਾਂ, (char_u*)GO_ALL, ਅੰਕ_Matches, matches);
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_GUI_TABLINE)
/*
 * The 'guitablabel' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_guitablabel(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    redraw_tabline = TRUE;
    ਵਾਪਸ ਨਿਰਾ;
}
#ਜੇ_ਅੰਤ

/*
 * The 'helpfile' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_helpfile(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    // May compute new values for $VIM and $VIMRUNTIME
    ਜੇ (didset_vim)
	vim_unsetenv_ext((char_u *)"VIM");
    ਜੇ (didset_vimruntime)
	vim_unsetenv_ext((char_u *)"VIMRUNTIME");
    ਵਾਪਸ ਨਿਰਾ;
}

#ਜੇ defined(FEAT_MULTI_LANG)
/*
 * The 'helplang' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_helplang(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਅੱਖਰ *errmsg = ਨਿਰਾ;

    // Check for "", "ab", "ab,cd", etc.
    ਲਈ (char_u *s = p_hlg; *s != NUL; s += 3)
    {
	ਜੇ (s[1] == NUL || ((s[2] != ',' || s[3] == NUL) && s[2] != NUL))
	{
	    errmsg = e_invalid_ਦਲੀਲ;
	    ਰੋਕੋ;
	}
	ਜੇ (s[2] == NUL)
	    ਰੋਕੋ;
    }

    ਵਾਪਸ errmsg;
}
#ਜੇ_ਅੰਤ

/*
 * The 'highlight' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_highlight(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਜੇ (highlight_ਬਦਲਿਆ() == FAIL)
	ਵਾਪਸ e_invalid_ਦਲੀਲ;	// invalid flags

    ਵਾਪਸ ਨਿਰਾ;
}

    ਸਥਿਰ ਪੂਰਨ
expand_hl_occasions(
	optexpand_T *ਦਲੀਲਾਂ,
	ਪੂਰਨ *ਅੰਕ_Matches,
	char_u ***matches,
	char_u ਅਗੇਤਰ)
{
    char_u	    *p;
    ਸਥਿਰ char_u   hl_flags[HLF_ਗਿਣਤੀ] = HL_FLAGS;
    size_t	    i;
    ਪੂਰਨ		    ਗਿਣਤੀ_2 = 0;

    *matches = ALLOC_MULT(char_u *, HLF_ਗਿਣਤੀ + 1);
    ਜੇ (*matches == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    // We still want to return the full option if it's requested.
    ਜੇ (ਦਲੀਲਾਂ->oe_include_orig_val)
    {
	p = vim_strsave(ਦਲੀਲਾਂ->oe_opt_ਮੁੱਲ);
	ਜੇ (p == ਨਿਰਾ)
	{
	    VIM_CLEAR(*matches);
	    ਵਾਪਸ FAIL;
	}
	(*matches)[ਗਿਣਤੀ_2++] = p;
    }

    ਲਈ (i = 0; i < HLF_ਗਿਣਤੀ; i++)
    {
	p = alloc((ਅਗੇਤਰ == NUL ? 1 : 2) + 1);
	ਜੇ (p == ਨਿਰਾ)
	{
	    ਜੇ (ਗਿਣਤੀ_2 == 0)
	    {
		VIM_CLEAR(*matches);
		ਵਾਪਸ FAIL;
	    }
	    ਨਹੀਂਤਾਂ
		ਰੋਕੋ;
	}
	ਜੇ (ਅਗੇਤਰ == NUL)
	    sprintf((ਅੱਖਰ *)p, "%c", hl_flags[i]);
	ਨਹੀਂਤਾਂ
	    sprintf((ਅੱਖਰ *)p, "%c%c", ਅਗੇਤਰ, hl_flags[i]);
	(*matches)[ਗਿਣਤੀ_2++] = p;
    }

    ਜੇ (ਗਿਣਤੀ_2 == 0)
    {
	VIM_CLEAR(*matches);
	ਵਾਪਸ FAIL;
    }
    *ਅੰਕ_Matches = ਗਿਣਤੀ_2;
    ਵਾਪਸ OK;
}

/*
 * Expand 'highlight' option.
 */
    ਪੂਰਨ
expand_ਸੈੱਟ_highlight(optexpand_T *ਦਲੀਲਾਂ, ਪੂਰਨ *ਅੰਕ_Matches, char_u ***matches)
{
    char_u	    *p;
    expand_T	    *xp = ਦਲੀਲਾਂ->oe_xp;
    size_t	    i;
    ਪੂਰਨ		    ਗਿਣਤੀ_2 = 0;

    ਜੇ (xp->xp_ਨਮੂਨਾ > ਦਲੀਲਾਂ->oe_ਸੈੱਟ_arg && *(xp->xp_ਨਮੂਨਾ-1) == ':')
    {
	// Right after a ':', meaning we just return all highlight names.
	ਵਾਪਸ expand_ਸੈੱਟ_opt_generic(
		ਦਲੀਲਾਂ,
		get_highlight_name,
		ਅੰਕ_Matches,
		matches);
    }

    ਜੇ (*xp->xp_ਨਮੂਨਾ == NUL)
	// At beginning of a comma-separated list. Return the specific list of
	// supported occasions.
	ਵਾਪਸ expand_hl_occasions(ਦਲੀਲਾਂ, ਅੰਕ_Matches, matches, NUL);

    // We are after the initial character (which indicates the occasion). We
    // already made sure we are not matching after a ':' above, so now we want
    // to match against display mode modifiers.
    // Since the xp_pattern starts from the beginning, we need to include it in
    // the returned match.

    // Note: Keep this in sync with highlight_changed()
    ਸਥਿਰ char_u p_hl_ਬਹੁਲਕ_ਮੁੱਲਾਂ[] =
	{':', 'b', 'i', '-', 'n', 'r', 's', 'u', 'c', '2', 'd', '=', 't'};
    size_t ਅੰਕ_hl_modes = ARRAY_ਲੰਮਾਈ(p_hl_ਬਹੁਲਕ_ਮੁੱਲਾਂ);

    *matches = ALLOC_MULT(char_u *, ਅੰਕ_hl_modes);
    ਜੇ (*matches == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    ਪੂਰਨ ਨਮੂਨਾ_len = xp->xp_ਨਮੂਨਾ_len;

    ਲਈ (i = 0; i < ਅੰਕ_hl_modes; i++)
    {
	// Don't allow duplicates as these are unique flags
	char_u *dup = vim_strchr(xp->xp_ਨਮੂਨਾ + 1, p_hl_ਬਹੁਲਕ_ਮੁੱਲਾਂ[i]);
	ਜੇ (dup != ਨਿਰਾ && (ਪੂਰਨ)(dup - xp->xp_ਨਮੂਨਾ) < ਨਮੂਨਾ_len)
	    ਜਾਰੀ;

	// ':' only works by itself, not with other flags.
	ਜੇ (ਨਮੂਨਾ_len > 1 && p_hl_ਬਹੁਲਕ_ਮੁੱਲਾਂ[i] == ':')
	    ਜਾਰੀ;

	p = vim_strnsave(xp->xp_ਨਮੂਨਾ, ਨਮੂਨਾ_len + 1);
	ਜੇ (p == ਨਿਰਾ)
	{
	    ਜੇ (i == 0)
	    {
		VIM_CLEAR(*matches);
		ਵਾਪਸ FAIL;
	    }
	    ਨਹੀਂਤਾਂ
		ਰੋਕੋ;
	}
	p[ਨਮੂਨਾ_len] = p_hl_ਬਹੁਲਕ_ਮੁੱਲਾਂ[i];
	p[ਨਮੂਨਾ_len + 1] = NUL;
	(*matches)[ਗਿਣਤੀ_2++] = p;
    }
    ਜੇ (ਗਿਣਤੀ_2 == 0)
    {
	VIM_CLEAR(*matches);
	ਵਾਪਸ FAIL;
    }
    *ਅੰਕ_Matches = ਗਿਣਤੀ_2;

    ਵਾਪਸ OK;
}

/*
 * The 'titlestring' or the 'iconstring' option is changed.
 */
    ਸਥਿਰ ਅੱਖਰ *
parse_titleiconstring(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ, ਪੂਰਨ flagval ਅਣਵਰਤਿਆ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_STL_OPT
    char_u	**varp = (char_u **)ਦਲੀਲਾਂ->os_varp;

    // NULL => statusline syntax
    ਜੇ (vim_strchr(*varp, '%') && check_stl_option(*varp) == ਨਿਰਾ)
	stl_syntax |= flagval;
    ਨਹੀਂਤਾਂ
	stl_syntax &= ~flagval;
#ਜੇ_ਅੰਤ
    did_ਸੈੱਟ_title();

    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * The 'iconstring' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_iconstring(optset_T *ਦਲੀਲਾਂ)
{
    ਪੂਰਨ flagval = 0;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_STL_OPT
    flagval = STL_ਵਿੱਚ_ICON;
#ਜੇ_ਅੰਤ

    ਵਾਪਸ parse_titleiconstring(ਦਲੀਲਾਂ, flagval);
}

#ਜੇ defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
/*
 * The 'imactivatekey' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_imactivatekey(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਜੇ (!im_xim_isvalid_imactivate())
	ਵਾਪਸ e_invalid_ਦਲੀਲ;
    ਵਾਪਸ ਨਿਰਾ;
}
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_IMAGE
/*
 * The 'imageprotocol' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_imageprotocol(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਜੇ (update_image_backend() == FAIL)
	ਵਾਪਸ e_invalid_ਦਲੀਲ;
    ਵਾਪਸ ਨਿਰਾ;
}

    ਪੂਰਨ
expand_ਸੈੱਟ_imageprotocol(optexpand_T *ਦਲੀਲਾਂ, ਪੂਰਨ *ਅੰਕ_Matches, char_u ***matches)
{
    expand_T *xp = ਦਲੀਲਾਂ->oe_xp;

    ਜੇ (xp->xp_ਨਮੂਨਾ > ਦਲੀਲਾਂ->oe_ਸੈੱਟ_arg && *(xp->xp_ਨਮੂਨਾ-1) == ':')
    {
	// 'imageprotocol' only has well-defined terms for completion for the
	// protocol part after the colon.
	ਵਾਪਸ expand_ਸੈੱਟ_opt_string(
		ਦਲੀਲਾਂ,
		p_ipc_protocol_ਮੁੱਲਾਂ,
		ARRAY_ਲੰਮਾਈ(p_ipc_protocol_ਮੁੱਲਾਂ) - 1,
		ਅੰਕ_Matches,
		matches);
    }
    // Use expand_set_opt_string instead of returning FAIL so that we can
    // include the original value if args->oe_include_orig_val is set.
    ਸਥਿਰ ਅੱਖਰ *(ਖਾਲੀ_ਥਾਵਾਂ_2[]) = {ਨਿਰਾ};
    ਵਾਪਸ expand_ਸੈੱਟ_opt_string(ਦਲੀਲਾਂ, ਖਾਲੀ_ਥਾਵਾਂ_2, 0, ਅੰਕ_Matches, matches);
}
#ਜੇ_ਅੰਤ

/*
 * The 'iskeyword' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_iskeyword(optset_T *ਦਲੀਲਾਂ)
{
    char_u	**varp = (char_u **)ਦਲੀਲਾਂ->os_varp;

    ਜੇ (varp == &p_isk)		// only check for global-value
    {
	ਜੇ (check_isopt(*varp) == FAIL)
	    ਵਾਪਸ e_invalid_ਦਲੀਲ;
    }
    ਨਹੀਂਤਾਂ			// fallthrough for local-value
	ਵਾਪਸ did_ਸੈੱਟ_isopt(ਦਲੀਲਾਂ);

    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * The 'isident' or the 'iskeyword' or the 'isprint' or the 'isfname' option is
 * changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_isopt(optset_T *ਦਲੀਲਾਂ)
{
    // 'isident', 'iskeyword', 'isprint' or 'isfname' option: refill g_chartab[]
    // If the new option is invalid, use old value.
    // 'lisp' option: refill g_chartab[] for '-' char.
    ਜੇ (init_chartab() == FAIL)
    {
	ਦਲੀਲਾਂ->os_restore_chartab = TRUE;// need to restore the chartab.
	ਵਾਪਸ e_invalid_ਦਲੀਲ;	// error in value
    }

    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * The 'jumpoptions' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_jumpoptions(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਜੇ (opt_ਲੜੀਆਂ_flags(p_jop, p_jop_ਮੁੱਲਾਂ, &jop_flags, TRUE) != OK)
	ਵਾਪਸ e_invalid_ਦਲੀਲ;

    ਵਾਪਸ ਨਿਰਾ;
}

    ਪੂਰਨ
expand_ਸੈੱਟ_jumpoptions(optexpand_T *ਦਲੀਲਾਂ, ਪੂਰਨ *ਅੰਕ_Matches, char_u ***matches)
{
    ਵਾਪਸ expand_ਸੈੱਟ_opt_string(
	    ਦਲੀਲਾਂ,
	    p_jop_ਮੁੱਲਾਂ,
	    ARRAY_ਲੰਮਾਈ(p_jop_ਮੁੱਲਾਂ) - 1,
	    ਅੰਕ_Matches,
	    matches);
}

#ਜੇ defined(FEAT_KEYMAP)
/*
 * The 'keymap' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_keymap(optset_T *ਦਲੀਲਾਂ)
{
    char_u	**varp = (char_u **)ਦਲੀਲਾਂ->os_varp;
    ਅੱਖਰ *errmsg = ਨਿਰਾ;

    ਜੇ (!valid_filetype(*varp))
	errmsg = e_invalid_ਦਲੀਲ;
    ਨਹੀਂਤਾਂ
    {
	ਪੂਰਨ	    secure_save = secure;

	// Reset the secure flag, since the value of 'keymap' has
	// been checked to be safe.
	secure = 0;

	// load or unload key mapping tables
	errmsg = keymap_init();

	secure = secure_save;

	// Since we check the value, there is no need to set P_INSECURE,
	// even when the value comes from a modeline.
	ਦਲੀਲਾਂ->os_ਮੁੱਲ_checked = TRUE;
    }

    ਜੇ (errmsg == ਨਿਰਾ)
    {
	ਜੇ (*curbuf->b_p_keymap != NUL)
	{
	    // Installed a new keymap, switch on using it.
	    curbuf->b_p_iminsert = B_IMODE_LMAP;
	    ਜੇ (curbuf->b_p_imsearch != B_IMODE_USE_ਪਾਓ)
		curbuf->b_p_imsearch = B_IMODE_LMAP;
	}
	ਨਹੀਂਤਾਂ
	{
	    // Cleared the keymap, may reset 'iminsert' and 'imsearch'.
	    ਜੇ (curbuf->b_p_iminsert == B_IMODE_LMAP)
		curbuf->b_p_iminsert = B_IMODE_NONE;
	    ਜੇ (curbuf->b_p_imsearch == B_IMODE_LMAP)
		curbuf->b_p_imsearch = B_IMODE_USE_ਪਾਓ;
	}
	ਜੇ ((ਦਲੀਲਾਂ->os_flags & OPT_LOCAL) == 0)
	{
	    ਸੈੱਟ_iminsert_global();
	    ਸੈੱਟ_imsearch_global();
	}
	ਸਥਿਤੀ_redraw_curbuf();
    }

    ਵਾਪਸ errmsg;
}
#ਜੇ_ਅੰਤ

/*
 * The 'keymodel' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_keymodel(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਜੇ (check_opt_ਲੜੀਆਂ(p_km, p_km_ਮੁੱਲਾਂ, TRUE) != OK)
	ਵਾਪਸ e_invalid_ਦਲੀਲ;

    km_stopsel = (vim_strchr(p_km, 'o') != ਨਿਰਾ);
    km_startsel = (vim_strchr(p_km, 'a') != ਨਿਰਾ);
    ਵਾਪਸ ਨਿਰਾ;
}

    ਪੂਰਨ
expand_ਸੈੱਟ_keymodel(optexpand_T *ਦਲੀਲਾਂ, ਪੂਰਨ *ਅੰਕ_Matches, char_u ***matches)
{
    ਵਾਪਸ expand_ਸੈੱਟ_opt_string(
	    ਦਲੀਲਾਂ,
	    p_km_ਮੁੱਲਾਂ,
	    ARRAY_ਲੰਮਾਈ(p_km_ਮੁੱਲਾਂ) - 1,
	    ਅੰਕ_Matches,
	    matches);
}

/*
 * The 'keyprotocol' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_keyprotocol(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    char_u *ਪਦ = T_NAME;
    keyprot_T kpc = ਮੇਲ_keyprotocol(ਪਦ);
    ਜੇ (kpc == KEYPROTOCOL_FAIL)
	ਵਾਪਸ e_invalid_ਦਲੀਲ;

    apply_keyprotocol(ਪਦ, kpc);

    ਵਾਪਸ ਨਿਰਾ;
}

    ਪੂਰਨ
expand_ਸੈੱਟ_keyprotocol(optexpand_T *ਦਲੀਲਾਂ, ਪੂਰਨ *ਅੰਕ_Matches, char_u ***matches)
{
    expand_T *xp = ਦਲੀਲਾਂ->oe_xp;
    ਜੇ (xp->xp_ਨਮੂਨਾ > ਦਲੀਲਾਂ->oe_ਸੈੱਟ_arg && *(xp->xp_ਨਮੂਨਾ-1) == ':')
    {
	// 'keyprotocol' only has well-defined terms for completion for the
	// protocol part after the colon.
	ਵਾਪਸ expand_ਸੈੱਟ_opt_string(
		ਦਲੀਲਾਂ,
		p_kpc_protocol_ਮੁੱਲਾਂ,
		ARRAY_ਲੰਮਾਈ(p_kpc_protocol_ਮੁੱਲਾਂ) - 1,
		ਅੰਕ_Matches,
		matches);
    }
    // Use expand_set_opt_string instead of returning FAIL so that we can
    // include the original value if args->oe_include_orig_val is set.
    ਸਥਿਰ ਅੱਖਰ *(ਖਾਲੀ_ਥਾਵਾਂ_2[]) = {ਨਿਰਾ};
    ਵਾਪਸ expand_ਸੈੱਟ_opt_string(ਦਲੀਲਾਂ, ਖਾਲੀ_ਥਾਵਾਂ_2, 0, ਅੰਕ_Matches, matches);
}

/*
 * The 'lispoptions' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_lispoptions(optset_T *ਦਲੀਲਾਂ)
{
    char_u	**varp = (char_u **)ਦਲੀਲਾਂ->os_varp;

    ਜੇ (**varp != NUL
	    && STRCMP(*varp, "expr:0") != 0
	    && STRCMP(*varp, "expr:1") != 0)
	ਵਾਪਸ e_invalid_ਦਲੀਲ;

    ਵਾਪਸ ਨਿਰਾ;
}

    ਪੂਰਨ
expand_ਸੈੱਟ_lispoptions(optexpand_T *ਦਲੀਲਾਂ, ਪੂਰਨ *ਅੰਕ_Matches, char_u ***matches)
{
    ਸਥਿਰ ਅੱਖਰ *(p_lop_ਮੁੱਲਾਂ[]) = {"expr:0", "expr:1", ਨਿਰਾ};
    ਵਾਪਸ expand_ਸੈੱਟ_opt_string(
	    ਦਲੀਲਾਂ,
	    p_lop_ਮੁੱਲਾਂ,
	    ARRAY_ਲੰਮਾਈ(p_lop_ਮੁੱਲਾਂ) - 1,
	    ਅੰਕ_Matches,
	    matches);
}

/*
 * The 'matchpairs' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_matchpairs(optset_T *ਦਲੀਲਾਂ)
{
    char_u	**varp = (char_u **)ਦਲੀਲਾਂ->os_varp;
    char_u	*p;

    ਜੇ (has_mbyte)
    {
	ਲਈ (p = *varp; *p != NUL; ++p)
	{
	    ਪੂਰਨ x2 = -1;
	    ਪੂਰਨ x3 = -1;

	    p += mb_ptr2len(p);
	    ਜੇ (*p != NUL)
		x2 = *p++;
	    ਜੇ (*p != NUL)
	    {
		x3 = mb_ptr2char(p);
		p += mb_ptr2len(p);
	    }
	    ਜੇ (x2 != ':' || x3 == -1 || (*p != NUL && *p != ','))
		ਵਾਪਸ e_invalid_ਦਲੀਲ;
	    ਜੇ (*p == NUL)
		ਰੋਕੋ;
	}
    }
    ਨਹੀਂਤਾਂ
    {
	// Check for "x:y,x:y"
	ਲਈ (p = *varp; *p != NUL; p += 4)
	{
	    ਜੇ (p[1] != ':' || p[2] == NUL || (p[3] != NUL && p[3] != ','))
		ਵਾਪਸ e_invalid_ਦਲੀਲ;
	    ਜੇ (p[3] == NUL)
		ਰੋਕੋ;
	}
    }

    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Process the updated 'messagesopt' option value.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_messagesopt(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਜੇ (messagesopt_ਬਦਲਿਆ() == FAIL)
	ਵਾਪਸ e_invalid_ਦਲੀਲ;

    ਵਾਪਸ ਨਿਰਾ;
}

    ਪੂਰਨ
expand_ਸੈੱਟ_messagesopt(optexpand_T *ਦਲੀਲਾਂ, ਪੂਰਨ *ਅੰਕ_Matches, char_u ***matches)
{
    ਸਥਿਰ ਅੱਖਰ *(p_mopt_ਮੁੱਲਾਂ[]) = {"hit-enter", "wait:", "history:", ਨਿਰਾ};
    ਵਾਪਸ expand_ਸੈੱਟ_opt_string(
	    ਦਲੀਲਾਂ,
	    p_mopt_ਮੁੱਲਾਂ,
	    ARRAY_ਲੰਮਾਈ(p_mopt_ਮੁੱਲਾਂ) - 1,
	    ਅੰਕ_Matches,
	    matches);
}

#ਜੇ defined(FEAT_SPELL)
/*
 * The 'mkspellmem' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_mkspellmem(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਜੇ (spell_check_msm() != OK)
	ਵਾਪਸ e_invalid_ਦਲੀਲ;

    ਵਾਪਸ ਨਿਰਾ;
}
#ਜੇ_ਅੰਤ

/*
 * The 'mouse' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_mouse(optset_T *ਦਲੀਲਾਂ)
{
    char_u	**varp = (char_u **)ਦਲੀਲਾਂ->os_varp;
    ਅੱਖਰ	*retval;

    retval = did_ਸੈੱਟ_option_listflag(*varp, (char_u *)MOUSE_ALL, ਦਲੀਲਾਂ->os_errbuf,
		    ਦਲੀਲਾਂ->os_errbuflen);
    ਜੇ (retval == ਨਿਰਾ)
    {
	redraw_tabline = TRUE;
	ਜੇ (tabline_ਉਚਾਈ() > 0)
	    update_screen(UPD_VALID);
#ਜੇ defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
	popup_close_info(); // Close info popup to apply new properties
#ਜੇ_ਅੰਤ
    }
    ਵਾਪਸ retval;
}

    ਪੂਰਨ
expand_ਸੈੱਟ_mouse(optexpand_T *ਦਲੀਲਾਂ, ਪੂਰਨ *ਅੰਕ_Matches, char_u ***matches)
{
    ਵਾਪਸ expand_ਸੈੱਟ_opt_listflag(ਦਲੀਲਾਂ, (char_u*)MOUSE_ALL, ਅੰਕ_Matches, matches);
}

/*
 * The 'mousemodel' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_mousemodel(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਜੇ (check_opt_ਲੜੀਆਂ(p_mousem, p_mousem_ਮੁੱਲਾਂ, FALSE) != OK)
	ਵਾਪਸ e_invalid_ਦਲੀਲ;
#ਜੇ defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU) && (Xm_ਸੰਸਕਰਣ <= 1002)
    ਨਹੀਂਤਾਂ ਜੇ (*p_mousem != *ਦਲੀਲਾਂ->os_oldval.string)
	// Changed from "extend" to "popup" or "popup_setpos" or vv: need
	// to create or delete the popup menus.
	gui_motif_update_mousemodel(ਜੜ੍ਹ_menu);
#ਜੇ_ਅੰਤ

    ਵਾਪਸ ਨਿਰਾ;
}

    ਪੂਰਨ
expand_ਸੈੱਟ_mousemodel(optexpand_T *ਦਲੀਲਾਂ, ਪੂਰਨ *ਅੰਕ_Matches, char_u ***matches)
{
    ਵਾਪਸ expand_ਸੈੱਟ_opt_string(
	    ਦਲੀਲਾਂ,
	    p_mousem_ਮੁੱਲਾਂ,
	    ARRAY_ਲੰਮਾਈ(p_mousem_ਮੁੱਲਾਂ) - 1,
	    ਅੰਕ_Matches,
	    matches);
}

#ਜੇ defined(FEAT_MOUSESHAPE)
    ਅੱਖਰ *
did_ਸੈੱਟ_mouseshape(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਅੱਖਰ *errmsg = ਨਿਰਾ;

    errmsg = parse_shape_opt(SHAPE_MOUSE);
    update_mouseshape(-1);

    ਵਾਪਸ errmsg;
}
#ਜੇ_ਅੰਤ

/*
 * The 'nrformats' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_nrformats(optset_T *ਦਲੀਲਾਂ)
{
    char_u	**varp = (char_u **)ਦਲੀਲਾਂ->os_varp;

    ਵਾਪਸ did_ਸੈੱਟ_opt_ਲੜੀਆਂ(*varp, p_nf_ਮੁੱਲਾਂ, TRUE);
}

    ਪੂਰਨ
expand_ਸੈੱਟ_nrformats(optexpand_T *ਦਲੀਲਾਂ, ਪੂਰਨ *ਅੰਕ_Matches, char_u ***matches)
{
    ਵਾਪਸ expand_ਸੈੱਟ_opt_string(
	    ਦਲੀਲਾਂ,
	    p_nf_ਮੁੱਲਾਂ,
	    ARRAY_ਲੰਮਾਈ(p_nf_ਮੁੱਲਾਂ) - 1,
	    ਅੰਕ_Matches,
	    matches);
}

#ਜੇ defined(FEAT_EVAL)
/*
 * One of the '*expr' options is changed: 'balloonexpr', 'diffexpr',
 * 'foldexpr', 'foldtext', 'formatexpr', 'includeexpr', 'indentexpr',
 * 'patchexpr', 'printexpr' and 'charconvert'.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_optexpr(optset_T *ਦਲੀਲਾਂ)
{
    char_u	**varp = (char_u **)ਦਲੀਲਾਂ->os_varp;

    // If the option value starts with <SID> or s:, then replace that with
    // the script identifier.
    char_u *name = get_scriptlocal_funcname(*varp);
    ਜੇ (name != ਨਿਰਾ)
    {
	free_string_option(*varp);
	*varp = name;
    }

    ਵਾਪਸ ਨਿਰਾ;
}
#ਜੇ_ਅੰਤ

/*
 * The 'pastetoggle' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_pastetoggle(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    char_u	*p;

    // translate key codes like in a mapping
    ਜੇ (*p_pt)
    {
	(ਖਾਲੀ)replace_termcodes(p_pt, &p, 0,
		REPTERM_FROM_PART | REPTERM_DO_LT, ਨਿਰਾ);
	ਜੇ (p != ਨਿਰਾ)
	{
	    free_string_option(p_pt);
	    p_pt = p;
	}
    }

    ਵਾਪਸ ਨਿਰਾ;
}

#ਜੇ defined(FEAT_PROP_POPUP)
/*
 * The 'previewpopup' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_previewpopup(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਜੇ (parse_previewpopup(ਨਿਰਾ) == FAIL)
	ਵਾਪਸ e_invalid_ਦਲੀਲ;

# ਜੇ defined(FEAT_QUICKFIX)
    popup_close_info();
# endif
    ਵਾਪਸ ਨਿਰਾ;
}

    ਸਥਿਰ ਪੂਰਨ
expand_ਸੈੱਟ_popupoption(optexpand_T *ਦਲੀਲਾਂ, ਪੂਰਨ *ਅੰਕ_Matches, char_u ***matches,
	ਪੂਰਨ previewpopup)
{
    expand_T *xp = ਦਲੀਲਾਂ->oe_xp;

    ਜੇ (xp->xp_ਨਮੂਨਾ > ਦਲੀਲਾਂ->oe_ਸੈੱਟ_arg && *(xp->xp_ਨਮੂਨਾ-1) == ':')
    {
	ਜੇ (completing_ਮੁੱਲ_for_subopt(ਦਲੀਲਾਂ, "close")
		|| completing_ਮੁੱਲ_for_subopt(ਦਲੀਲਾਂ, "resize")
		|| completing_ਮੁੱਲ_for_subopt(ਦਲੀਲਾਂ, "shadow"))
	{
	    ਵਾਪਸ expand_ਸੈੱਟ_opt_string(
		    ਦਲੀਲਾਂ,
		    p_popup_option_on_off_ਮੁੱਲਾਂ,
		    ARRAY_ਲੰਮਾਈ(p_popup_option_on_off_ਮੁੱਲਾਂ) - 1,
		    ਅੰਕ_Matches,
		    matches);
	}
	ਜੇ (completing_ਮੁੱਲ_for_subopt(ਦਲੀਲਾਂ, "border"))
	{
	    ਵਾਪਸ expand_ਸੈੱਟ_opt_string(
		    ਦਲੀਲਾਂ,
		    previewpopup ? p_popup_pvp_border_ਮੁੱਲਾਂ
			: p_popup_cpp_border_ਮੁੱਲਾਂ,
		    (previewpopup ? ARRAY_ਲੰਮਾਈ(p_popup_pvp_border_ਮੁੱਲਾਂ) - 1
			: ARRAY_ਲੰਮਾਈ(p_popup_cpp_border_ਮੁੱਲਾਂ)) - 1,
		    ਅੰਕ_Matches,
		    matches);
	}
	ਜੇ (completing_ਮੁੱਲ_for_subopt(ਦਲੀਲਾਂ, "align"))
	{
	    ਵਾਪਸ expand_ਸੈੱਟ_opt_string(
		    ਦਲੀਲਾਂ,
		    p_popup_option_align_ਮੁੱਲਾਂ,
		    ARRAY_ਲੰਮਾਈ(p_popup_option_align_ਮੁੱਲਾਂ) - 1,
		    ਅੰਕ_Matches,
		    matches);
	}
	ਜੇ (completing_ਮੁੱਲ_for_subopt(ਦਲੀਲਾਂ, "highlight")
		|| completing_ਮੁੱਲ_for_subopt(ਦਲੀਲਾਂ, "borderhighlight"))
	{
	    // Return the list of all highlight names
	    ਵਾਪਸ expand_ਸੈੱਟ_opt_generic(
		    ਦਲੀਲਾਂ,
		    get_highlight_name,
		    ਅੰਕ_Matches,
		    matches);
	}
	ਵਾਪਸ FAIL;
    }

    ਵਾਪਸ expand_ਸੈੱਟ_opt_string(
	    ਦਲੀਲਾਂ,
	    previewpopup ? p_popup_pvp_option_ਮੁੱਲਾਂ
		: p_popup_cpp_option_ਮੁੱਲਾਂ,
	    previewpopup ? ARRAY_ਲੰਮਾਈ(p_popup_pvp_option_ਮੁੱਲਾਂ) - 1
		: ARRAY_ਲੰਮਾਈ(p_popup_cpp_option_ਮੁੱਲਾਂ) - 1,
	    ਅੰਕ_Matches,
	    matches);
}

    ਪੂਰਨ
expand_ਸੈੱਟ_previewpopup(optexpand_T *ਦਲੀਲਾਂ, ਪੂਰਨ *ਅੰਕ_Matches, char_u ***matches)
{
    ਵਾਪਸ expand_ਸੈੱਟ_popupoption(ਦਲੀਲਾਂ, ਅੰਕ_Matches, matches, TRUE);
}

    ਪੂਰਨ
expand_ਸੈੱਟ_completepopup(optexpand_T *ਦਲੀਲਾਂ, ਪੂਰਨ *ਅੰਕ_Matches, char_u ***matches)
{
    ਵਾਪਸ expand_ਸੈੱਟ_popupoption(ਦਲੀਲਾਂ, ਅੰਕ_Matches, matches, FALSE);
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_POSTSCRIPT)
/*
 * The 'printencoding' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_printencoding(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    char_u	*s, *p;

    // Canonize 'printencoding' if VIM standard one
    p = enc_canonize(p_penc);
    ਜੇ (p != ਨਿਰਾ)
    {
	vim_free(p_penc);
	p_penc = p;
    }
    ਨਹੀਂਤਾਂ
    {
	// Ensure lower case and '-' for '_'
	ਲਈ (s = p_penc; *s != NUL; s++)
	{
	    ਜੇ (*s == '_')
		*s = '-';
	    ਨਹੀਂਤਾਂ
		*s = TOLOWER_ASC(*s);
	}
    }

    ਵਾਪਸ ਨਿਰਾ;
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_PRINTER)

    ਸਥਿਰ char_u *
get_printoptions_ਨਾਂ(expand_T *xp ਅਣਵਰਤਿਆ, ਪੂਰਨ idx)
{
    ਜੇ (idx >= (ਪੂਰਨ)ARRAY_ਲੰਮਾਈ(printer_opts))
	ਵਾਪਸ ਨਿਰਾ;
    ਵਾਪਸ (char_u*)printer_opts[idx].name;
}

/*
 * Expand 'printoptions' option
 */
    ਪੂਰਨ
expand_ਸੈੱਟ_printoptions(optexpand_T *ਦਲੀਲਾਂ, ਪੂਰਨ *ਅੰਕ_Matches, char_u ***matches)
{
    ਵਾਪਸ expand_ਸੈੱਟ_opt_generic(
	    ਦਲੀਲਾਂ,
	    get_printoptions_ਨਾਂ,
	    ਅੰਕ_Matches,
	    matches);
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_STL_OPT)
/*
 * The 'statusline' or the 'tabline' or the 'rulerformat' option is changed.
 * "rulerformat" is TRUE if the 'rulerformat' option is changed.
 */
    ਸਥਿਰ ਅੱਖਰ *
parse_statustabline_rulerformat(optset_T *ਦਲੀਲਾਂ, ਪੂਰਨ rulerformat)
{
    char_u	**varp = (char_u **)ਦਲੀਲਾਂ->os_varp;
    char_u	*s;
    ਅੱਖਰ	*errmsg = ਨਿਰਾ;
    ਪੂਰਨ		wid;

    ਜੇ (rulerformat)	// reset ru_wid first
	ru_wid = 0;
    s = *varp;
    ਜੇ (rulerformat && *s == '%')
    {
	// set ru_wid if 'ruf' starts with "%99("
	ਜੇ (*++s == '-')	// ignore a '-'
	    s++;
	wid = getdigits(&s);
	ਜੇ (wid && *s == '(' && (errmsg = check_stl_option(p_ruf)) == ਨਿਰਾ)
	    ru_wid = wid;
	ਨਹੀਂਤਾਂ
	{
	    // Validate the flags in 'rulerformat' only if it doesn't point to
	    // a custom function ("%!" flag).
	    ਜੇ ((*varp)[1] != '!')
		errmsg = check_stl_option(p_ruf);
	}
    }
    // check 'statusline' or 'tabline' only if it doesn't start with "%!"
    ਨਹੀਂਤਾਂ ਜੇ (rulerformat || s[0] != '%' || s[1] != '!')
	errmsg = check_stl_option(s);
    ਜੇ (rulerformat && errmsg == ਨਿਰਾ)
	comp_ਖਾਨਾ();

    ਵਾਪਸ errmsg;
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_RENDER_OPTIONS)
/*
 * The 'renderoptions' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_renderoptions(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਜੇ (!gui_mch_ਸੈੱਟ_rendering_options(p_rop))
	ਵਾਪਸ e_invalid_ਦਲੀਲ;

    ਵਾਪਸ ਨਿਰਾ;
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_RIGHTLEFT)
/*
 * The 'rightleftcmd' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_rightleftcmd(optset_T *ਦਲੀਲਾਂ)
{
    char_u	**varp = (char_u **)ਦਲੀਲਾਂ->os_varp;

    // Currently only "search" is a supported value.
    ਜੇ (**varp != NUL && STRCMP(*varp, "search") != 0)
	ਵਾਪਸ e_invalid_ਦਲੀਲ;

    ਵਾਪਸ ਨਿਰਾ;
}

    ਪੂਰਨ
expand_ਸੈੱਟ_rightleftcmd(optexpand_T *ਦਲੀਲਾਂ, ਪੂਰਨ *ਅੰਕ_Matches, char_u ***matches)
{
    ਸਥਿਰ ਅੱਖਰ *(p_rlc_ਮੁੱਲਾਂ[]) = {"search", ਨਿਰਾ};
    ਵਾਪਸ expand_ਸੈੱਟ_opt_string(
	    ਦਲੀਲਾਂ,
	    p_rlc_ਮੁੱਲਾਂ,
	    ARRAY_ਲੰਮਾਈ(p_rlc_ਮੁੱਲਾਂ) - 1,
	    ਅੰਕ_Matches,
	    matches);
}
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PUM_BORDER_CLEAR()   \
    ਕਰੋ {                     \
	pum_ਸੈੱਟ_border(FALSE); \
	pum_ਸੈੱਟ_shadow(FALSE); \
	pum_ਸੈੱਟ_margin(FALSE); \
    } ਜਦੋਂ (0)

/*
 * Parse a border value from a pumopt "border:" token.
 * Returns OK on success, FAIL on error.
 */
    ਸਥਿਰ ਪੂਰਨ
parse_pumopt_border(char_u *val, ਪੂਰਨ len)
{
    // Use box-drawing characters only when 'encoding' is "utf-8" and
    // 'ambiwidth' is "single".
    ਪੂਰਨ	    can_use_box_chars = (enc_utf8 && *p_ambw == 's');
    char_u  *token;

    token = vim_strnsave(val, len);
    ਜੇ (token == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    ਜੇ (can_use_box_chars && STRCMP(token, "single") == 0)
	pum_ਸੈੱਟ_border_chars(0x2500, 0x2502, 0x2500, 0x2502, // ─ │ ─ │
		0x250c, 0x2510, 0x2518, 0x2514); // ┌ ┐ ┘ └
    ਨਹੀਂਤਾਂ ਜੇ (can_use_box_chars && STRCMP(token, "double") == 0)
	pum_ਸੈੱਟ_border_chars(0x2550, 0x2551, 0x2550, 0x2551, // ═ ║ ═ ║
		0x2554, 0x2557, 0x255D, 0x255A); // ╔ ╗ ╝  ╚
    ਨਹੀਂਤਾਂ ਜੇ (can_use_box_chars && STRCMP(token, "round") == 0)
	pum_ਸੈੱਟ_border_chars(0x2500, 0x2502, 0x2500, 0x2502, // ─ │ ─ │
		0x256d, 0x256e, 0x256f, 0x2570); // ╭ ╮ ╯ ╰
    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(token, "ascii") == 0)
	pum_ਸੈੱਟ_border_chars('-', '|', '-', '|', '+', '+', '+', '+');
    ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(token, "custom:", 7) == 0)
    {
	char_u	*q = token + 7;
	ਪੂਰਨ	ਬਾਹਰਲਾ[8];

	ਲਈ (ਪੂਰਨ i = 0; i < 8; i++)
	{
	    ਜੇ (*q == NUL || *q == ',')
	    {
		vim_free(token);
		ਵਾਪਸ FAIL;
	    }
	    ਬਾਹਰਲਾ[i] = mb_ptr2char(q);
	    mb_ptr2char_adv(&q);
	    ਜੇ (i < 7)
	    {
		ਜੇ (*q != ';')
		{
		    vim_free(token);
		    ਵਾਪਸ FAIL;
		}
		q++;
	    }
	}
	ਜੇ (*q != NUL && *q != ',')
	{
	    vim_free(token);
	    ਵਾਪਸ FAIL;
	}
	pum_ਸੈੱਟ_border_chars(ਬਾਹਰਲਾ[0], ਬਾਹਰਲਾ[1], ਬਾਹਰਲਾ[2], ਬਾਹਰਲਾ[3], ਬਾਹਰਲਾ[4], ਬਾਹਰਲਾ[5],
		ਬਾਹਰਲਾ[6], ਬਾਹਰਲਾ[7]);
    }
    ਨਹੀਂਤਾਂ
    {
	vim_free(token);
	ਵਾਪਸ FAIL;
    }

    vim_free(token);
    pum_ਸੈੱਟ_border(TRUE);
    ਵਾਪਸ OK;
}

/*
 * The 'pumopt' option is changed.
 * Format: comma-separated key:value pairs.
 *   border:{single|double|round|ascii|custom:X;X;X;X;X;X;X;X}
 *   height:{n}
 *   width:{n}
 *   maxwidth:{n}
 *   opacity:{n}
 *   shadow
 *   margin (requires border)
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_pumopt(optset_T *ਦਲੀਲਾਂ)
{
    char_u  **varp = (char_u **)ਦਲੀਲਾਂ->os_varp;
    char_u  *p;
    ਪੂਰਨ	    have_border = FALSE;
    ਪੂਰਨ	    have_margin = FALSE;

    // Reset to defaults.
    PUM_BORDER_CLEAR();
    p_ph = 0;
    p_pw = 15;
    p_pmw = 0;
    p_po = 100;

    ਜੇ (*varp == ਨਿਰਾ || **varp == NUL)
	ਵਾਪਸ ਨਿਰਾ;

    ਲਈ (p = *varp; p != ਨਿਰਾ && *p != NUL; )
    {
	char_u *comma = vim_strchr(p, ',');
	ਪੂਰਨ len;

	ਜੇ (comma != ਨਿਰਾ)
	    len = (ਪੂਰਨ)(comma - p);
	ਨਹੀਂਤਾਂ
	    len = (ਪੂਰਨ)STRLEN(p);

	ਜੇ (STRNCMP(p, "border:", 7) == 0)
	{
	    ਜੇ (have_border)
		ਜਾਓ error;
	    have_border = TRUE;
	    ਜੇ (parse_pumopt_border(p + 7, len - 7) == FAIL)
		ਜਾਓ error;
	}
	ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(p, "height:", 7) == 0)
	{
	    ਲੰਮਾ n = atol((ਅੱਖਰ *)p + 7);
	    ਜੇ (n < 0)
		ਜਾਓ error;
	    p_ph = n;
	}
	ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(p, "width:", 6) == 0)
	{
	    ਲੰਮਾ n = atol((ਅੱਖਰ *)p + 6);
	    ਜੇ (n < 0)
		ਜਾਓ error;
	    p_pw = n;
	}
	ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(p, "maxwidth:", 9) == 0)
	{
	    ਲੰਮਾ n = atol((ਅੱਖਰ *)p + 9);
	    ਜੇ (n < 0)
		ਜਾਓ error;
	    p_pmw = n;
	}
	ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(p, "opacity:", 8) == 0)
	{
	    ਲੰਮਾ n = atol((ਅੱਖਰ *)p + 8);
	    ਜੇ (n < 0 || n > 100)
		ਜਾਓ error;
	    p_po = n;
	}
	ਨਹੀਂਤਾਂ ਜੇ (len == 6 && STRNCMP(p, "shadow", 6) == 0)
	    pum_ਸੈੱਟ_shadow(TRUE);
	ਨਹੀਂਤਾਂ ਜੇ (len == 6 && STRNCMP(p, "margin", 6) == 0)
	{
	    have_margin = TRUE;
	    pum_ਸੈੱਟ_margin(TRUE);
	}
	ਨਹੀਂਤਾਂ
	    ਜਾਓ error;

	ਜੇ (comma != ਨਿਰਾ)
	    p = comma + 1;
	ਨਹੀਂਤਾਂ
	    ਰੋਕੋ;
    }

    ਜੇ (have_margin && !have_border)
	ਜਾਓ error;

    // Invalidate cached background for opacity changes.
    pum_opacity_ਬਦਲਿਆ();

    ਵਾਪਸ ਨਿਰਾ;

error:
    PUM_BORDER_CLEAR();
    p_ph = 0;
    p_pw = 15;
    p_pmw = 0;
    p_po = 100;
    ਵਾਪਸ e_invalid_ਦਲੀਲ;
}

    ਸਥਿਰ char_u *
get_pum_border_style(expand_T *xp ਅਣਵਰਤਿਆ, ਪੂਰਨ idx)
{
    ਸਥਿਰ ਅੱਖਰ *styles[] = {"ascii", "custom:", "single", "double", "round"};
    ਵਾਪਸ idx < ((enc_utf8 && *p_ambw == 's') ? (ਪੂਰਨ)ARRAY_ਲੰਮਾਈ(styles) : 2)
	    ? (char_u *)styles[idx] : ਨਿਰਾ;
}

    ਪੂਰਨ
expand_ਸੈੱਟ_pumopt(optexpand_T *ਦਲੀਲਾਂ, ਪੂਰਨ *ਅੰਕ_Matches, char_u ***matches)
{
    expand_T *xp = ਦਲੀਲਾਂ->oe_xp;

    ਜੇ (xp->xp_ਨਮੂਨਾ > ਦਲੀਲਾਂ->oe_ਸੈੱਟ_arg && *(xp->xp_ਨਮੂਨਾ-1) == ':')
    {
	ਜੇ (completing_ਮੁੱਲ_for_subopt(ਦਲੀਲਾਂ, "border"))
	{
	    ਵਾਪਸ expand_ਸੈੱਟ_opt_generic(
		    ਦਲੀਲਾਂ, get_pum_border_style, ਅੰਕ_Matches, matches);
	}
	ਵਾਪਸ FAIL;
    }

    ਸਥਿਰ ਅੱਖਰ *(p_pumopt_ਮੁੱਲਾਂ[]) = {"border:", "height:", "width:",
	"maxwidth:", "opacity:", "shadow", "margin", ਨਿਰਾ};
    ਵਾਪਸ expand_ਸੈੱਟ_opt_string(
	    ਦਲੀਲਾਂ,
	    p_pumopt_ਮੁੱਲਾਂ,
	    ARRAY_ਲੰਮਾਈ(p_pumopt_ਮੁੱਲਾਂ) - 1,
	    ਅੰਕ_Matches,
	    matches);
}

/*
 * The 'pumborder' option is changed.
 * Rules:
 *   - One of { single, double, round, ascii, custom:XXXXXXXX } may appear.
 *   - "margin" may appear, but only together with exactly one border style.
 *   - "shadow" is independent and can be combined freely.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_pumborder(optset_T *ਦਲੀਲਾਂ)
{
    char_u  **varp = (char_u **)ਦਲੀਲਾਂ->os_varp;
    char_u  *p;
    ਪੂਰਨ	    len;
    ਪੂਰਨ	    have_border = FALSE;
    ਪੂਰਨ	    have_margin = FALSE;

    PUM_BORDER_CLEAR();

    ਜੇ (*varp == ਨਿਰਾ || **varp == NUL)
	ਵਾਪਸ ਨਿਰਾ;

    ਲਈ (p = *varp; p != ਨਿਰਾ && *p != NUL; )
    {
	char_u *comma = vim_strchr(p, ',');
	ਜੇ (comma != ਨਿਰਾ)
	    len = (ਪੂਰਨ)(comma - p);
	ਨਹੀਂਤਾਂ
	    len = (ਪੂਰਨ)STRLEN(p);

	ਜੇ (STRNCMP(p, "shadow", len) == 0 && len == 6)
	    pum_ਸੈੱਟ_shadow(TRUE);
	ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(p, "margin", len) == 0 && len == 6)
	{
	    have_margin = TRUE;
	    pum_ਸੈੱਟ_margin(TRUE);
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (have_border)
		ਜਾਓ error;
	    have_border = TRUE;
	    ਜੇ (parse_pumopt_border(p, len) == FAIL)
		ਜਾਓ error;
	}

	ਜੇ (comma != ਨਿਰਾ)
	    p = comma + 1;
	ਨਹੀਂਤਾਂ
	    ਰੋਕੋ;
    }

    ਜੇ (have_margin && !have_border)
	ਜਾਓ error;

    ਵਾਪਸ ਨਿਰਾ;

error:
    PUM_BORDER_CLEAR();
    pum_ਸੈੱਟ_border_chars(0, 0, 0, 0, 0, 0, 0, 0);
    ਵਾਪਸ e_invalid_ਦਲੀਲ;
}

    ਸਥਿਰ char_u *
get_pumborder_token(expand_T *xp, ਪੂਰਨ idx)
{
    ਵਾਪਸ idx == 0 ? (char_u *)"margin"
	 : idx == 1 ? (char_u *)"shadow"
	 : get_pum_border_style(xp, idx - 2);
}

    ਪੂਰਨ
expand_ਸੈੱਟ_pumborder(optexpand_T *ਦਲੀਲਾਂ, ਪੂਰਨ *ਅੰਕ_Matches, char_u ***matches)
{
    ਵਾਪਸ expand_ਸੈੱਟ_opt_generic(
	    ਦਲੀਲਾਂ, get_pumborder_token, ਅੰਕ_Matches, matches);
}

#ਜੇ defined(FEAT_STL_OPT)

/*
 * The 'rulerformat' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_rulerformat(optset_T *ਦਲੀਲਾਂ)
{
    ਵਾਪਸ parse_statustabline_rulerformat(ਦਲੀਲਾਂ, TRUE);
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_TABPANEL)
/*
 * Process the new 'tabpanelopt' option value.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_tabpanelopt(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਜੇ (tabpanelopt_ਬਦਲਿਆ() == FAIL)
	ਵਾਪਸ e_invalid_ਦਲੀਲ;

    ਵਾਪਸ ਨਿਰਾ;
}

    ਪੂਰਨ
expand_ਸੈੱਟ_tabpanelopt(optexpand_T *ਦਲੀਲਾਂ, ਪੂਰਨ *ਅੰਕ_Matches, char_u ***matches)
{
    expand_T *xp = ਦਲੀਲਾਂ->oe_xp;

    ਜੇ (xp->xp_ਨਮੂਨਾ > ਦਲੀਲਾਂ->oe_ਸੈੱਟ_arg && *(xp->xp_ਨਮੂਨਾ-1) == ':')
    {
	ਜੇ (completing_ਮੁੱਲ_for_subopt(ਦਲੀਲਾਂ, "align"))
	{
	    ਵਾਪਸ expand_ਸੈੱਟ_opt_string(
		    ਦਲੀਲਾਂ,
		    p_tplo_align_ਮੁੱਲਾਂ,
		    ARRAY_ਲੰਮਾਈ(p_tplo_align_ਮੁੱਲਾਂ) - 1,
		    ਅੰਕ_Matches,
		    matches);
	}
	ਵਾਪਸ FAIL;
    }

    ਵਾਪਸ expand_ਸੈੱਟ_opt_string(
	    ਦਲੀਲਾਂ,
	    p_tplo_ਮੁੱਲਾਂ,
	    ARRAY_ਲੰਮਾਈ(p_tplo_ਮੁੱਲਾਂ) - 1,
	    ਅੰਕ_Matches,
	    matches);
}
#ਜੇ_ਅੰਤ

/*
 * The 'scrollopt' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_scrollopt(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ did_ਸੈੱਟ_opt_ਲੜੀਆਂ(p_sbo, p_scbopt_ਮੁੱਲਾਂ, TRUE);
}

    ਪੂਰਨ
expand_ਸੈੱਟ_scrollopt(optexpand_T *ਦਲੀਲਾਂ, ਪੂਰਨ *ਅੰਕ_Matches, char_u ***matches)
{
    ਵਾਪਸ expand_ਸੈੱਟ_opt_string(
	    ਦਲੀਲਾਂ,
	    p_scbopt_ਮੁੱਲਾਂ,
	    ARRAY_ਲੰਮਾਈ(p_scbopt_ਮੁੱਲਾਂ) - 1,
	    ਅੰਕ_Matches,
	    matches);
}

/*
 * The 'wlseat' option is changed
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_wlseat(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_WAYLAND_CLIPBOARD
    // If there isn't any seat named 'wlseat', then let the Wayland clipboard be
    // unavailable. Ignore errors returned.
    clip_reset_wayland();
#ਜੇ_ਅੰਤ

    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * The 'selection' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_selection(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਜੇ (*p_sel == NUL || check_opt_ਲੜੀਆਂ(p_sel, p_sel_ਮੁੱਲਾਂ, FALSE) != OK)
	ਵਾਪਸ e_invalid_ਦਲੀਲ;

    ਵਾਪਸ ਨਿਰਾ;
}

    ਪੂਰਨ
expand_ਸੈੱਟ_selection(optexpand_T *ਦਲੀਲਾਂ, ਪੂਰਨ *ਅੰਕ_Matches, char_u ***matches)
{
    ਵਾਪਸ expand_ਸੈੱਟ_opt_string(
	    ਦਲੀਲਾਂ,
	    p_sel_ਮੁੱਲਾਂ,
	    ARRAY_ਲੰਮਾਈ(p_sel_ਮੁੱਲਾਂ) - 1,
	    ਅੰਕ_Matches,
	    matches);
}

/*
 * The 'selectmode' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_selectmode(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ did_ਸੈੱਟ_opt_ਲੜੀਆਂ(p_slm, p_slm_ਮੁੱਲਾਂ, TRUE);
}

    ਪੂਰਨ
expand_ਸੈੱਟ_selectmode(optexpand_T *ਦਲੀਲਾਂ, ਪੂਰਨ *ਅੰਕ_Matches, char_u ***matches)
{
    ਵਾਪਸ expand_ਸੈੱਟ_opt_string(
	    ਦਲੀਲਾਂ,
	    p_slm_ਮੁੱਲਾਂ,
	    ARRAY_ਲੰਮਾਈ(p_slm_ਮੁੱਲਾਂ) - 1,
	    ਅੰਕ_Matches,
	    matches);
}

#ਜੇ defined(FEAT_SESSION)
/*
 * The 'sessionoptions' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_sessionoptions(optset_T *ਦਲੀਲਾਂ)
{
    ਜੇ (opt_ਲੜੀਆਂ_flags(p_ssop, p_ssop_ਮੁੱਲਾਂ, &ssop_flags, TRUE) != OK)
	ਵਾਪਸ e_invalid_ਦਲੀਲ;
    ਜੇ ((ssop_flags & SSOP_CURDIR) && (ssop_flags & SSOP_SESDIR))
    {
	// Don't allow both "sesdir" and "curdir".
	(ਖਾਲੀ)opt_ਲੜੀਆਂ_flags(ਦਲੀਲਾਂ->os_oldval.string, p_ssop_ਮੁੱਲਾਂ,
							&ssop_flags, TRUE);
	ਵਾਪਸ e_invalid_ਦਲੀਲ;
    }

    ਵਾਪਸ ਨਿਰਾ;
}

    ਪੂਰਨ
expand_ਸੈੱਟ_sessionoptions(
    optexpand_T		*ਦਲੀਲਾਂ,
    ਪੂਰਨ			*ਅੰਕ_Matches,
    char_u		***matches)
{
    ਵਾਪਸ expand_ਸੈੱਟ_opt_string(
	    ਦਲੀਲਾਂ,
	    p_ssop_ਮੁੱਲਾਂ,
	    ARRAY_ਲੰਮਾਈ(p_ssop_ਮੁੱਲਾਂ) - 1,
	    ਅੰਕ_Matches,
	    matches);
}
#ਜੇ_ਅੰਤ

/*
 * Validate 'shellpipe'/'shellredir' option.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_shellpipe_redir(optset_T *ਦਲੀਲਾਂ)
{
    char_u	*p;
    bool	ਵੇਖਿਆ = false;

    ਲਈ (p = ਦਲੀਲਾਂ->os_newval.string; *p != NUL; ++p)
    {
	ਜੇ (*p != '%')
	    ਜਾਰੀ;

	ਜੇ (p[1] == NUL)
	    ਵਾਪਸ e_invalid_format_string_single_percent_s;

	ਜੇ (p[1] == '%')
	{
	    ++p;    // skip second %
	    ਜਾਰੀ;
	}

	ਜੇ (p[1] == 's')
	{
	    ਜੇ (ਵੇਖਿਆ)
		ਵਾਪਸ e_invalid_format_string_single_percent_s;

	    ਵੇਖਿਆ = true;
	    ++p;    // consume 's'
	    ਜਾਰੀ;
	}
	ਵਾਪਸ e_invalid_format_string_single_percent_s;
    }
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * The 'shortmess' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_shortmess(optset_T *ਦਲੀਲਾਂ)
{
    char_u	**varp = (char_u **)ਦਲੀਲਾਂ->os_varp;

    ਵਾਪਸ did_ਸੈੱਟ_option_listflag(*varp, (char_u *)SHM_ALL, ਦਲੀਲਾਂ->os_errbuf,
		    ਦਲੀਲਾਂ->os_errbuflen);
}

    ਪੂਰਨ
expand_ਸੈੱਟ_shortmess(optexpand_T *ਦਲੀਲਾਂ, ਪੂਰਨ *ਅੰਕ_Matches, char_u ***matches)
{
    ਵਾਪਸ expand_ਸੈੱਟ_opt_listflag(ਦਲੀਲਾਂ, (char_u*)SHM_ALL, ਅੰਕ_Matches, matches);
}

#ਜੇ defined(FEAT_LINEBREAK)
/*
 * The 'showbreak' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_showbreak(optset_T *ਦਲੀਲਾਂ)
{
    char_u	**varp = (char_u **)ਦਲੀਲਾਂ->os_varp;
    char_u	*s;

    ਲਈ (s = *varp; *s; )
    {
	ਜੇ (ptr2cells(s) != 1)
	    ਵਾਪਸ e_showbreak_ਸ਼ਾਮਲ_ਹੈ_unprintable_or_wide_character;
	MB_PTR_ADV(s);
    }

    ਵਾਪਸ ਨਿਰਾ;
}
#ਜੇ_ਅੰਤ

/*
 * The 'showcmdloc' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_showcmdloc(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਅੱਖਰ	*errmsg = did_ਸੈੱਟ_opt_ਲੜੀਆਂ(p_sloc, p_sloc_ਮੁੱਲਾਂ, FALSE);

    ਜੇ (errmsg == ਨਿਰਾ)
	comp_ਖਾਨਾ();

    ਵਾਪਸ errmsg;
}

    ਪੂਰਨ
expand_ਸੈੱਟ_showcmdloc(optexpand_T *ਦਲੀਲਾਂ, ਪੂਰਨ *ਅੰਕ_Matches, char_u ***matches)
{
    ਵਾਪਸ expand_ਸੈੱਟ_opt_string(
	    ਦਲੀਲਾਂ,
	    p_sloc_ਮੁੱਲਾਂ,
	    ARRAY_ਲੰਮਾਈ(p_sloc_ਮੁੱਲਾਂ) - 1,
	    ਅੰਕ_Matches,
	    matches);
}

#ਜੇ defined(FEAT_SIGNS)
/*
 * The 'signcolumn' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_signcolumn(optset_T *ਦਲੀਲਾਂ)
{
    char_u	**varp = (char_u **)ਦਲੀਲਾਂ->os_varp;

# ਜੇ defined(FEAT_LINEBREAK)
    ਜੇ (check_opt_ਲੜੀਆਂ(*varp, p_scl_ਮੁੱਲਾਂ, FALSE) != OK)
	ਵਾਪਸ e_invalid_ਦਲੀਲ;
    // When changing the 'signcolumn' to or from 'number', recompute the
    // width of the number column if 'number' or 'relativenumber' is set.
    ਜੇ (((*ਦਲੀਲਾਂ->os_oldval.string == 'n' && ਦਲੀਲਾਂ->os_oldval.string[1] == 'u')
		|| (*curwin->w_p_scl == 'n' && *(curwin->w_p_scl + 1) =='u'))
	    && (curwin->w_p_nu || curwin->w_p_rnu))
	curwin->w_nrwidth_ਸਤਰ_ਗਿਣਤੀ = 0;
# endif

# ਜੇ defined(FEAT_GUI)
    // In the GUI, when sign icons are used, a full screen clear is needed
    // to properly redraw the sign icons.
    ਜੇ (gui.ਵਿੱਚ_use && curbuf->b_signlist != ਨਿਰਾ)
	redraw_all_later(UPD_CLEAR);
# endif

    ਵਾਪਸ ਨਿਰਾ;
}

    ਪੂਰਨ
expand_ਸੈੱਟ_signcolumn(optexpand_T *ਦਲੀਲਾਂ, ਪੂਰਨ *ਅੰਕ_Matches, char_u ***matches)
{
    ਵਾਪਸ expand_ਸੈੱਟ_opt_string(
	    ਦਲੀਲਾਂ,
	    p_scl_ਮੁੱਲਾਂ,
	    ARRAY_ਲੰਮਾਈ(p_scl_ਮੁੱਲਾਂ) - 1,
	    ਅੰਕ_Matches,
	    matches);
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_SPELL)
/*
 * The 'spellcapcheck' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_spellcapcheck(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    // compile the regexp program.
    ਵਾਪਸ compile_cap_prog(curwin->w_s);
}

/*
 * The 'spellfile' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_spellfile(optset_T *ਦਲੀਲਾਂ)
{
    char_u	**varp = (char_u **)ਦਲੀਲਾਂ->os_varp;

    ਜੇ (!valid_spellfile(*varp))
	ਵਾਪਸ e_invalid_ਦਲੀਲ;

    // If there is a window for this buffer in which 'spell' is set load the
    // wordlists.
    ਵਾਪਸ did_ਸੈੱਟ_spell_option();
}

/*
 * The 'spell' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_spelllang(optset_T *ਦਲੀਲਾਂ)
{
    char_u	**varp = (char_u **)ਦਲੀਲਾਂ->os_varp;

    ਜੇ (!valid_spelllang(*varp))
	ਵਾਪਸ e_invalid_ਦਲੀਲ;

    // If there is a window for this buffer in which 'spell' is set load the
    // wordlists.
    ਵਾਪਸ did_ਸੈੱਟ_spell_option();
}

/*
 * The 'spelloptions' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_spelloptions(optset_T *ਦਲੀਲਾਂ)
{
    char_u	**varp = (char_u **)ਦਲੀਲਾਂ->os_varp;

    ਜੇ (**varp != NUL && STRCMP(*varp, "camel") != 0)
	ਵਾਪਸ e_invalid_ਦਲੀਲ;

    ਵਾਪਸ ਨਿਰਾ;
}

    ਪੂਰਨ
expand_ਸੈੱਟ_spelloptions(optexpand_T *ਦਲੀਲਾਂ, ਪੂਰਨ *ਅੰਕ_Matches, char_u ***matches)
{
    ਸਥਿਰ ਅੱਖਰ *(p_spo_ਮੁੱਲਾਂ[]) = {"camel", ਨਿਰਾ};
    ਵਾਪਸ expand_ਸੈੱਟ_opt_string(
	    ਦਲੀਲਾਂ,
	    p_spo_ਮੁੱਲਾਂ,
	    ARRAY_ਲੰਮਾਈ(p_spo_ਮੁੱਲਾਂ) - 1,
	    ਅੰਕ_Matches,
	    matches);
}

/*
 * The 'spellsuggest' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_spellsuggest(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਜੇ (spell_check_sps() != OK)
	ਵਾਪਸ e_invalid_ਦਲੀਲ;

    ਵਾਪਸ ਨਿਰਾ;
}

    ਪੂਰਨ
expand_ਸੈੱਟ_spellsuggest(optexpand_T *ਦਲੀਲਾਂ, ਪੂਰਨ *ਅੰਕ_Matches, char_u ***matches)
{
    ਵਾਪਸ expand_ਸੈੱਟ_opt_string(
	    ਦਲੀਲਾਂ,
	    p_sps_ਮੁੱਲਾਂ,
	    ARRAY_ਲੰਮਾਈ(p_sps_ਮੁੱਲਾਂ) - 1,
	    ਅੰਕ_Matches,
	    matches);
}
#ਜੇ_ਅੰਤ

/*
 * The 'splitkeep' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_splitkeep(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    win_T	*wp;
    tabpage_T	*tp;
    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	wp->w_prev_ਉਚਾਈ = wp->w_ਉਚਾਈ;
	wp->w_prev_winrow = wp->w_winrow;
    }
    ਵਾਪਸ did_ਸੈੱਟ_opt_ਲੜੀਆਂ(p_spk, p_spk_ਮੁੱਲਾਂ, FALSE);
}

    ਪੂਰਨ
expand_ਸੈੱਟ_splitkeep(optexpand_T *ਦਲੀਲਾਂ, ਪੂਰਨ *ਅੰਕ_Matches, char_u ***matches)
{
    ਵਾਪਸ expand_ਸੈੱਟ_opt_string(
	    ਦਲੀਲਾਂ,
	    p_spk_ਮੁੱਲਾਂ,
	    ARRAY_ਲੰਮਾਈ(p_spk_ਮੁੱਲਾਂ) - 1,
	    ਅੰਕ_Matches,
	    matches);
}

#ਜੇ defined(FEAT_STL_OPT)
/*
 * The 'statusline' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_statusline(optset_T *ਦਲੀਲਾਂ)
{
    char_u  **varp = (char_u **)ਦਲੀਲਾਂ->os_varp;
    ਅੱਖਰ    *ret = parse_statustabline_rulerformat(ਦਲੀਲਾਂ, FALSE);

    ਜੇ (ret != ਨਿਰਾ)
	ਵਾਪਸ ret;
    update_stl_rendered_ਉਚਾਈ(varp == &curwin->w_p_stl ? curwin : ਨਿਰਾ);
    ਚੌਖਟਾ_change_statusline_ਉਚਾਈ();

    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Rewrite the 'statuslineopt' string: replace the last "maxheight:N" with
 * actual_stlh and remove earlier duplicates.  Keyword order is preserved.
 */
    ਸਥਿਰ ਖਾਲੀ
update_stlo_maxheight(char_u **varp, ਪੂਰਨ actual_stlh)
{
    ਅਚਲ char_u    *src = *varp;
    ਅਚਲ char_u    *ਆਖਰੀ_mh = ਨਿਰਾ;
    ਅਚਲ char_u    *p;

    // Find the last "maxheight:" token.
    p = src;
    ਜਦੋਂ (*p != NUL)
    {
	ਜੇ (STRNCMP(p, "maxheight:", 10) == 0)
	    ਆਖਰੀ_mh = p;
	ਜਦੋਂ (*p != ',' && *p != NUL)
	    ++p;
	ਜੇ (*p == ',')
	    ++p;
    }

    ਜੇ (ਆਖਰੀ_mh == ਨਿਰਾ)
	ਵਾਪਸ;

    size_t	bufsize = STRLEN(src) + NUMBUFLEN + 1;
    char_u	*buf = alloc(bufsize);
    ਜੇ (buf == ਨਿਰਾ)
	ਵਾਪਸ;

    ਪੂਰਨ		len = 0;
    bool	need_comma = false;

    p = src;
    ਜਦੋਂ (*p != NUL)
    {
	ਅਚਲ	char_u	*tok = p;
	ਪੂਰਨ	tok_len;

	ਜਦੋਂ (*p != ',' && *p != NUL)
	    ++p;
	tok_len = (ਪੂਰਨ)(p - tok);
	ਜੇ (*p == ',')
	    ++p;

	ਜੇ (STRNCMP(tok, "maxheight:", 10) == 0)
	{
	    ਜੇ (tok == ਆਖਰੀ_mh)
	    {
		// Replace the last occurrence with the actual value.
		ਜੇ (need_comma)
		    buf[len++] = ',';
		len += vim_snprintf((ਅੱਖਰ *)buf + len,
			bufsize - len, "maxheight:%d", actual_stlh);
		need_comma = true;
	    }
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (need_comma)
		buf[len++] = ',';
	    mch_memmove(buf + len, tok, tok_len);
	    len += tok_len;
	    need_comma = true;
	}
    }
    buf[len] = NUL;
    free_string_option(*varp);
    *varp = buf;
}

/*
 * The 'statuslineopt' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_statuslineopt(optset_T *ਦਲੀਲਾਂ)
{
    char_u	**varp = (char_u **)ਦਲੀਲਾਂ->os_varp;
    win_T       *wp = varp == &curwin->w_p_stlo ? curwin : ਨਿਰਾ;

    ਜੇ (statuslineopt_ਬਦਲਿਆ(*varp, wp) == FAIL)
	ਵਾਪਸ e_invalid_ਦਲੀਲ;

    // Sync stl_rendered_height with the current statusline.
    update_stl_rendered_ਉਚਾਈ(wp);

    // Update the maxheight value to the actual value set.
    // Note: Must be changed if p_stlo_values are changed.
    ਜੇ (*varp != ਖਾਲੀ_ਥਾਵਾਂ_option)
    {
	ਪੂਰਨ actual_stlh;

	ਜੇ (wp != ਨਿਰਾ)
	{
	    ਚੌਖਟਾ_change_statusline_ਉਚਾਈ();
	    actual_stlh = MIN(wp->w_p_stlo_mh,
		    wp->w_ਉਚਾਈ + wp->w_ਸਥਿਤੀ_ਉਚਾਈ - p_wmh);
	}
	ਨਹੀਂਤਾਂ
	    actual_stlh = ਚੌਖਟਾ_change_statusline_ਉਚਾਈ();

	// Only re-serialize when the window actually has a status line shown.
	ਜੇ (actual_stlh > 0)
	{
	    update_stlo_maxheight(varp, actual_stlh);
	    // Update the parsed maxheight member directly.
	    ਜੇ (wp != ਨਿਰਾ)
		wp->w_p_stlo_mh = actual_stlh;
	    ਨਹੀਂਤਾਂ
		ਸੈੱਟ_stlo_mh(actual_stlh);
	}
    }
    ਨਹੀਂਤਾਂ
	ਚੌਖਟਾ_change_statusline_ਉਚਾਈ();

    ਜੇ ((ਦਲੀਲਾਂ->os_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	// :set clears the local values.
	clear_string_option(&curwin->w_p_stlo);
	curwin->w_p_stlo_fh = -1;
	curwin->w_p_stlo_mh = -1;
    }

    ਵਾਪਸ ਨਿਰਾ;
}

    ਪੂਰਨ
expand_ਸੈੱਟ_statuslineopt(optexpand_T *ਦਲੀਲਾਂ, ਪੂਰਨ *ਅੰਕ_Matches, char_u ***matches)
{
    ਵਾਪਸ expand_ਸੈੱਟ_opt_string(
	    ਦਲੀਲਾਂ,
	    p_stlo_ਮੁੱਲਾਂ,
	    ARRAY_ਲੰਮਾਈ(p_stlo_ਮੁੱਲਾਂ) - 1,
	    ਅੰਕ_Matches,
	    matches);
}
#ਜੇ_ਅੰਤ

/*
 * The 'swapsync' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_swapsync(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ did_ਸੈੱਟ_opt_ਲੜੀਆਂ(p_sws, p_sws_ਮੁੱਲਾਂ, FALSE);
}

    ਪੂਰਨ
expand_ਸੈੱਟ_swapsync(optexpand_T *ਦਲੀਲਾਂ, ਪੂਰਨ *ਅੰਕ_Matches, char_u ***matches)
{
    ਵਾਪਸ expand_ਸੈੱਟ_opt_string(
	    ਦਲੀਲਾਂ,
	    p_sws_ਮੁੱਲਾਂ,
	    ARRAY_ਲੰਮਾਈ(p_sws_ਮੁੱਲਾਂ) - 1,
	    ਅੰਕ_Matches,
	    matches);
}

/*
 * The 'switchbuf' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_switchbuf(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ did_ਸੈੱਟ_opt_flags(p_swb, p_swb_ਮੁੱਲਾਂ, &swb_flags, TRUE);
}

    ਪੂਰਨ
expand_ਸੈੱਟ_switchbuf(optexpand_T *ਦਲੀਲਾਂ, ਪੂਰਨ *ਅੰਕ_Matches, char_u ***matches)
{
    ਵਾਪਸ expand_ਸੈੱਟ_opt_string(
	    ਦਲੀਲਾਂ,
	    p_swb_ਮੁੱਲਾਂ,
	    ARRAY_ਲੰਮਾਈ(p_swb_ਮੁੱਲਾਂ) - 1,
	    ਅੰਕ_Matches,
	    matches);
}

/*
 * The 'tabclose' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_tabclose(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ did_ਸੈੱਟ_opt_flags(p_tcl, p_tcl_ਮੁੱਲਾਂ, &tcl_flags, TRUE);
}

    ਪੂਰਨ
expand_ਸੈੱਟ_tabclose(optexpand_T *ਦਲੀਲਾਂ, ਪੂਰਨ *ਅੰਕ_Matches, char_u ***matches)
{
    ਵਾਪਸ expand_ਸੈੱਟ_opt_string(
	    ਦਲੀਲਾਂ,
	    p_tcl_ਮੁੱਲਾਂ,
	    ARRAY_ਲੰਮਾਈ(p_tcl_ਮੁੱਲਾਂ) - 1,
	    ਅੰਕ_Matches,
	    matches);
}

#ਜੇ defined(FEAT_STL_OPT)
/*
 * The 'tabline' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_tabline(optset_T *ਦਲੀਲਾਂ)
{
    ਵਾਪਸ parse_statustabline_rulerformat(ਦਲੀਲਾਂ, FALSE);
}
#ਜੇ_ਅੰਤ

/*
 * The 'tagcase' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_tagcase(optset_T *ਦਲੀਲਾਂ)
{
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ	*flags;
    char_u		*p;

    ਜੇ (ਦਲੀਲਾਂ->os_flags & OPT_LOCAL)
    {
	p = curbuf->b_p_tc;
	flags = &curbuf->b_tc_flags;
    }
    ਨਹੀਂਤਾਂ
    {
	p = p_tc;
	flags = &tc_flags;
    }

    ਜੇ ((ਦਲੀਲਾਂ->os_flags & OPT_LOCAL) && *p == NUL)
	// make the local value empty: use the global value
	*flags = 0;
    ਨਹੀਂਤਾਂ ਜੇ (*p == NUL
	    || opt_ਲੜੀਆਂ_flags(p, p_tc_ਮੁੱਲਾਂ, flags, FALSE) != OK)
	ਵਾਪਸ e_invalid_ਦਲੀਲ;

    ਵਾਪਸ ਨਿਰਾ;
}

    ਪੂਰਨ
expand_ਸੈੱਟ_tagcase(optexpand_T *ਦਲੀਲਾਂ, ਪੂਰਨ *ਅੰਕ_Matches, char_u ***matches)
{
    ਵਾਪਸ expand_ਸੈੱਟ_opt_string(
	    ਦਲੀਲਾਂ,
	    p_tc_ਮੁੱਲਾਂ,
	    ARRAY_ਲੰਮਾਈ(p_tc_ਮੁੱਲਾਂ) - 1,
	    ਅੰਕ_Matches,
	    matches);
}

/*
 * The 'term' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_ਪਦ(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਜੇ (T_NAME[0] == NUL)
	ਵਾਪਸ e_cannot_ਸੈੱਟ_ਪਦ_ਮੰਜ਼ਿਲ_ਖਾਲੀ_ਥਾਵਾਂ_string;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    ਜੇ (gui.ਵਿੱਚ_use)
	ਵਾਪਸ e_cannot_change_ਪਦ_ਵਿੱਚ_GUI;
    ਜੇ (ਪਦ_is_gui(T_NAME))
	ਵਾਪਸ e_use_gui_ਮੰਜ਼ਿਲ_ਸ਼ੁਰੂ_GUI;
#ਜੇ_ਅੰਤ
    ਜੇ (ਸੈੱਟ_termname(T_NAME) == FAIL)
	ਵਾਪਸ e_not_ਮਿਲਿਆ_ਵਿੱਚ_termcap;

    // Screen colors may have changed.
    redraw_later_clear();

    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Some terminal option (t_xxx) is changed
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_ਪਦ_option(optset_T *ਦਲੀਲਾਂ)
{
    char_u	**varp = (char_u **)ਦਲੀਲਾਂ->os_varp;

    ਜੇ (!ਭਰਿਆ_screen)
	ਵਾਪਸ ਨਿਰਾ;

    // ":set t_Co=0" and ":set t_Co=1" do ":set t_Co="
    ਜੇ (varp == &T_CCO)
    {
	ਪੂਰਨ colors = atoi((ਅੱਖਰ *)T_CCO);

	// Only reinitialize colors if t_Co value has really changed to
	// avoid expensive reload of colorscheme if t_Co is set to the
	// same value multiple times.
	ਜੇ (colors != t_colors)
	{
	    t_colors = colors;
	    ਜੇ (t_colors <= 1)
	    {
		vim_free(T_CCO);
		T_CCO = ਖਾਲੀ_ਥਾਵਾਂ_option;
	    }
#ਜੇ defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
	    ਜੇ (is_ਪਦ_win32())
	    {
		ਅਦਲਾ_ਬਦਲੀ_tcap();
		ਦਲੀਲਾਂ->os_did_swaptcap = TRUE;
	    }
#ਜੇ_ਅੰਤ
	    // We now have a different color setup, initialize it again.
	    init_highlight(TRUE, FALSE);
	}
    }
    ttest(FALSE);
    ਜੇ (varp == &T_ME)
    {
	ਬਾਹਰਲਾ_str(T_ME);
	redraw_later(UPD_CLEAR);
#ਜੇ defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
	// Since t_me has been set, this probably means that the user
	// wants to use this as default colors.  Need to reset default
	// background/foreground colors.
# ifdef VIMDLL
	ਜੇ (!gui.ਵਿੱਚ_use && !gui.starting)
# endif
	    mch_ਸੈੱਟ_ਸਾਧਾਰਨ_colors();
#ਜੇ_ਅੰਤ
    }
    ਜੇ (varp == &T_BE && termcap_ਸਰਗਰਮ)
    {
	MAY_WANT_ਮੰਜ਼ਿਲ_LOG_THIS;

	ਜੇ (*T_BE == NUL)
	    // When clearing t_BE we assume the user no longer wants
	    // bracketed paste, thus disable it by writing t_BD.
	    ਬਾਹਰਲਾ_str(T_BD);
	ਨਹੀਂਤਾਂ
	    ਬਾਹਰਲਾ_str(T_BE);
    }

    ਜੇ (varp == &T_BSU || varp == &T_ESU)
	ਪਦ_ਸੈੱਟ_sync_output(ਪਦ_SYNC_OUTPUT_OFF);

    ਵਾਪਸ ਨਿਰਾ;
}


#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
/*
 * The 'termresize' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_termresize(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    // If empty or "inband", then attempt to enable in-band resize events.
    ਜੇ (*p_trz == NUL || STRCMP(p_trz, "inband") == 0)
	ਪਦ_ਸੈੱਟ_win_resize(true);
    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(p_trz, "sigwinch") == 0)
	ਪਦ_ਸੈੱਟ_win_resize(false);
    ਨਹੀਂਤਾਂ
	ਵਾਪਸ e_invalid_ਦਲੀਲ;

    ਵਾਪਸ ਨਿਰਾ;
}

    ਪੂਰਨ
expand_ਸੈੱਟ_termresize(optexpand_T *ਦਲੀਲਾਂ, ਪੂਰਨ *ਅੰਕ_Matches, char_u ***matches)
{
    ਵਾਪਸ expand_ਸੈੱਟ_opt_string(
	    ਦਲੀਲਾਂ,
	    p_trz_ਮੁੱਲਾਂ,
	    ARRAY_ਲੰਮਾਈ(p_trz_ਮੁੱਲਾਂ) - 1,
	    ਅੰਕ_Matches,
	    matches);
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_TERMINAL)
/*
 * The 'termwinkey' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_termwinkey(optset_T *ਦਲੀਲਾਂ)
{
    char_u	**varp = (char_u **)ਦਲੀਲਾਂ->os_varp;

    ਜੇ ((*varp)[0] != NUL && string_ਮੰਜ਼ਿਲ_ਕੁੰਜੀ(*varp, TRUE) == 0)
	ਵਾਪਸ e_invalid_ਦਲੀਲ;

    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * The 'termwinsize' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_termwinsize(optset_T *ਦਲੀਲਾਂ)
{
    char_u	**varp = (char_u **)ਦਲੀਲਾਂ->os_varp;
    char_u	*p;

    ਜੇ ((*varp)[0] == NUL)
	ਵਾਪਸ ਨਿਰਾ;

    p = skipdigits(*varp);
    ਜੇ (p == *varp || (*p != 'x' && *p != '*') || *skipdigits(p + 1) != NUL)
	ਵਾਪਸ e_invalid_ਦਲੀਲ;

    ਵਾਪਸ ਨਿਰਾ;
}

# ਜੇ defined(MSWIN)
/*
 * The 'termwintype' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_termwintype(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ did_ਸੈੱਟ_opt_ਲੜੀਆਂ(p_twt, p_twt_ਮੁੱਲਾਂ, FALSE);
}

    ਪੂਰਨ
expand_ਸੈੱਟ_termwintype(optexpand_T *ਦਲੀਲਾਂ, ਪੂਰਨ *ਅੰਕ_Matches, char_u ***matches)
{
    ਵਾਪਸ expand_ਸੈੱਟ_opt_string(
	    ਦਲੀਲਾਂ,
	    p_twt_ਮੁੱਲਾਂ,
	    ARRAY_ਲੰਮਾਈ(p_twt_ਮੁੱਲਾਂ) - 1,
	    ਅੰਕ_Matches,
	    matches);
}
# endif
#ਜੇ_ਅੰਤ

/*
 * The 'titlestring' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_titlestring(optset_T *ਦਲੀਲਾਂ)
{
    ਪੂਰਨ flagval = 0;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_STL_OPT
    flagval = STL_ਵਿੱਚ_TITLE;
#ਜੇ_ਅੰਤ
    ਵਾਪਸ parse_titleiconstring(ਦਲੀਲਾਂ, flagval);
}

#ਜੇ defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
/*
 * The 'toolbar' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_toolbar(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਜੇ (opt_ਲੜੀਆਂ_flags(p_toolbar, p_toolbar_ਮੁੱਲਾਂ, &toolbar_flags,
								TRUE) != OK)
	ਵਾਪਸ e_invalid_ਦਲੀਲ;

    ਬਾਹਰਲਾ_flush();
    gui_mch_show_toolbar((toolbar_flags &
		(TOOLBAR_ਪਾਠ | TOOLBAR_ICONS)) != 0);
    ਵਾਪਸ ਨਿਰਾ;
}

    ਪੂਰਨ
expand_ਸੈੱਟ_toolbar(optexpand_T *ਦਲੀਲਾਂ, ਪੂਰਨ *ਅੰਕ_Matches, char_u ***matches)
{
    ਵਾਪਸ expand_ਸੈੱਟ_opt_string(
	    ਦਲੀਲਾਂ,
	    p_toolbar_ਮੁੱਲਾਂ,
	    ARRAY_ਲੰਮਾਈ(p_toolbar_ਮੁੱਲਾਂ) - 1,
	    ਅੰਕ_Matches,
	    matches);
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_TOOLBAR) && defined(FEAT_GUI_GTK)
/*
 * The 'toolbariconsize' option is changed.  GTK+ 2 only.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_toolbariconsize(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਜੇ (opt_ਲੜੀਆਂ_flags(p_tbis, p_tbis_ਮੁੱਲਾਂ, &tbis_flags, FALSE) != OK)
	ਵਾਪਸ e_invalid_ਦਲੀਲ;

    ਬਾਹਰਲਾ_flush();
    gui_mch_show_toolbar((toolbar_flags &
		(TOOLBAR_ਪਾਠ | TOOLBAR_ICONS)) != 0);
    ਵਾਪਸ ਨਿਰਾ;
}

    ਪੂਰਨ
expand_ਸੈੱਟ_toolbariconsize(
    optexpand_T	*ਦਲੀਲਾਂ,
    ਪੂਰਨ		*ਅੰਕ_Matches,
    char_u	***matches)
{
    ਵਾਪਸ expand_ਸੈੱਟ_opt_string(
	    ਦਲੀਲਾਂ,
	    p_tbis_ਮੁੱਲਾਂ,
	    ARRAY_ਲੰਮਾਈ(p_tbis_ਮੁੱਲਾਂ) - 1,
	    ਅੰਕ_Matches,
	    matches);
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(UNIX) || defined(VMS)
/*
 * The 'ttymouse' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_ttymouse(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਅੱਖਰ *errmsg = ਨਿਰਾ;

    // Switch the mouse off before changing the escape sequences used for
    // that.
    mch_setmouse(FALSE);
    ਜੇ (opt_ਲੜੀਆਂ_flags(p_ttym, p_ttym_ਮੁੱਲਾਂ, &ttym_flags, FALSE) != OK)
	errmsg = e_invalid_ਦਲੀਲ;
    ਨਹੀਂਤਾਂ
	check_mouse_termcode();
    ਜੇ (termcap_ਸਰਗਰਮ)
	setmouse();		// may switch it on again

    ਵਾਪਸ errmsg;
}

    ਪੂਰਨ
expand_ਸੈੱਟ_ttymouse(optexpand_T *ਦਲੀਲਾਂ, ਪੂਰਨ *ਅੰਕ_Matches, char_u ***matches)
{
    ਵਾਪਸ expand_ਸੈੱਟ_opt_string(
	    ਦਲੀਲਾਂ,
	    p_ttym_ਮੁੱਲਾਂ,
	    ARRAY_ਲੰਮਾਈ(p_ttym_ਮੁੱਲਾਂ) - 1,
	    ਅੰਕ_Matches,
	    matches);
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_VARTABS)
/*
 * The 'varsofttabstop' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_varsofttabstop(optset_T *ਦਲੀਲਾਂ)
{
    char_u	**varp = (char_u **)ਦਲੀਲਾਂ->os_varp;
    char_u *cp;

    ਜੇ (!((*varp)[0]) || ((*varp)[0] == '0' && !((*varp)[1])))
	VIM_CLEAR(curbuf->b_p_vsts_array);
    ਨਹੀਂਤਾਂ
    {
	ਲਈ (cp = *varp; *cp; ++cp)
	{
	    ਜੇ (vim_isdigit(*cp))
		ਜਾਰੀ;
	    ਜੇ (*cp == ',' && cp > *varp && *(cp-1) != ',')
		ਜਾਰੀ;
	    ਵਾਪਸ e_invalid_ਦਲੀਲ;
	}

	ਪੂਰਨ *oldarray = curbuf->b_p_vsts_array;
	ਜੇ (tabstop_ਸੈੱਟ(*varp, &(curbuf->b_p_vsts_array)) == OK)
	{
	    ਜੇ (oldarray)
		vim_free(oldarray);
	}
	ਨਹੀਂਤਾਂ
	    ਵਾਪਸ e_invalid_ਦਲੀਲ;
    }

    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * The 'vartabstop' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_vartabstop(optset_T *ਦਲੀਲਾਂ)
{
    char_u	**varp = (char_u **)ਦਲੀਲਾਂ->os_varp;
    char_u *cp;

    ਜੇ (!((*varp)[0]) || ((*varp)[0] == '0' && !((*varp)[1])))
	VIM_CLEAR(curbuf->b_p_vts_array);
    ਨਹੀਂਤਾਂ
    {
	ਲਈ (cp = *varp; *cp; ++cp)
	{
	    ਜੇ (vim_isdigit(*cp))
		ਜਾਰੀ;
	    ਜੇ (*cp == ',' && cp > *varp && *(cp-1) != ',')
		ਜਾਰੀ;
	    ਵਾਪਸ e_invalid_ਦਲੀਲ;
	}

	ਪੂਰਨ *oldarray = curbuf->b_p_vts_array;

	ਜੇ (tabstop_ਸੈੱਟ(*varp, &(curbuf->b_p_vts_array)) == OK)
	{
	    vim_free(oldarray);
# ifdef FEAT_FOLDING
	    ਜੇ (foldmethodIsIndent(curwin))
		foldUpdateAll(curwin);
# endif
	}
	ਨਹੀਂਤਾਂ
	    ਵਾਪਸ e_invalid_ਦਲੀਲ;
    }

    ਵਾਪਸ ਨਿਰਾ;
}
#ਜੇ_ਅੰਤ

/*
 * The 'verbosefile' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_verbosefile(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    verbose_stop();
    ਜੇ (*p_vfile != NUL && verbose_open() == FAIL)
	ਵਾਪਸ e_invalid_ਦਲੀਲ;

    ਵਾਪਸ ਨਿਰਾ;
}

#ਜੇ defined(FEAT_SESSION)
/*
 * The 'viewoptions' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_viewoptions(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ did_ਸੈੱਟ_opt_flags(p_vop, p_ssop_ਮੁੱਲਾਂ, &vop_flags, TRUE);
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_VIMINFO)
/*
 * The 'viminfo' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_viminfo(optset_T *ਦਲੀਲਾਂ)
{
    char_u	*s;
    ਅੱਖਰ	*errmsg = ਨਿਰਾ;

    ਲਈ (s = p_viminfo; *s;)
    {
	// Check it's a valid character
	ਜੇ (vim_strchr((char_u *)"!\"%'/:<@cfhnrs", *s) == ਨਿਰਾ)
	{
	    errmsg = illegal_char(ਦਲੀਲਾਂ->os_errbuf, ਦਲੀਲਾਂ->os_errbuflen, *s);
	    ਰੋਕੋ;
	}
	ਜੇ (*s == 'n')	// name is always last one
	    ਰੋਕੋ;
	ਨਹੀਂਤਾਂ ਜੇ (*s == 'r') // skip until next ','
	{
	    ਜਦੋਂ (*++s && *s != ',')
		;
	}
	ਨਹੀਂਤਾਂ ਜੇ (*s == '%')
	{
	    // optional number
	    ਜਦੋਂ (vim_isdigit(*++s))
		;
	}
	ਨਹੀਂਤਾਂ ਜੇ (*s == '!' || *s == 'h' || *s == 'c')
	    ++s;		// no extra chars
	ਨਹੀਂਤਾਂ		// must have a number
	{
	    ਜਦੋਂ (vim_isdigit(*++s))
		;

	    ਜੇ (!VIM_ISDIGIT(*(s - 1)))
	    {
		ਜੇ (ਦਲੀਲਾਂ->os_errbuf != ਨਿਰਾ)
		{
		    vim_snprintf(ਦਲੀਲਾਂ->os_errbuf, ਦਲੀਲਾਂ->os_errbuflen,
			    _(e_missing_ਸੰਖਿਆ_after_angle_str_angle),
			    transchar_byte(*(s - 1)));
		    errmsg = ਦਲੀਲਾਂ->os_errbuf;
		}
		ਨਹੀਂਤਾਂ
		    errmsg = "";
		ਰੋਕੋ;
	    }
	}
	ਜੇ (*s == ',')
	    ++s;
	ਨਹੀਂਤਾਂ ਜੇ (*s)
	{
	    ਜੇ (ਦਲੀਲਾਂ->os_errbuf != ਨਿਰਾ)
		errmsg = e_missing_comma;
	    ਨਹੀਂਤਾਂ
		errmsg = "";
	    ਰੋਕੋ;
	}
    }
    ਜੇ (*p_viminfo && errmsg == ਨਿਰਾ && get_viminfo_parameter('\'') < 0)
	errmsg = e_must_specify_a_ਮੁੱਲ;

    ਵਾਪਸ errmsg;
}
#ਜੇ_ਅੰਤ

/*
 * The 'virtualedit' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_virtualedit(optset_T *ਦਲੀਲਾਂ)
{
    char_u		*ve = p_ve;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ	*flags = &ve_flags;

    ਜੇ (ਦਲੀਲਾਂ->os_flags & OPT_LOCAL)
    {
	ve = curwin->w_p_ve;
	flags = &curwin->w_ve_flags;
    }

    ਜੇ ((ਦਲੀਲਾਂ->os_flags & OPT_LOCAL) && *ve == NUL)
	// make the local value empty: use the global value
	*flags = 0;
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (opt_ਲੜੀਆਂ_flags(ve, p_ve_ਮੁੱਲਾਂ, flags, TRUE) != OK)
	    ਵਾਪਸ e_invalid_ਦਲੀਲ;
	ਨਹੀਂਤਾਂ ਜੇ (STRCMP(ve, ਦਲੀਲਾਂ->os_oldval.string) != 0)
	{
	    // Recompute cursor position in case the new 've' setting
	    // changes something.
	    validate_virtcol();
	    coladvance(curwin->w_virtcol);
	}
    }

    ਵਾਪਸ ਨਿਰਾ;
}

    ਪੂਰਨ
expand_ਸੈੱਟ_virtualedit(optexpand_T *ਦਲੀਲਾਂ, ਪੂਰਨ *ਅੰਕ_Matches, char_u ***matches)
{
    ਵਾਪਸ expand_ਸੈੱਟ_opt_string(
	    ਦਲੀਲਾਂ,
	    p_ve_ਮੁੱਲਾਂ,
	    ARRAY_ਲੰਮਾਈ(p_ve_ਮੁੱਲਾਂ) - 1,
	    ਅੰਕ_Matches,
	    matches);
}

/*
 * The 'whichwrap' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_whichwrap(optset_T *ਦਲੀਲਾਂ)
{
    char_u	**varp = (char_u **)ਦਲੀਲਾਂ->os_varp;

    // Add ',' to the list flags because 'whichwrap' is a flag
    // list that is comma-separated.
    ਵਾਪਸ did_ਸੈੱਟ_option_listflag(*varp, (char_u *)(WW_ALL ","),
		    ਦਲੀਲਾਂ->os_errbuf, ਦਲੀਲਾਂ->os_errbuflen);
}

    ਪੂਰਨ
expand_ਸੈੱਟ_whichwrap(optexpand_T *ਦਲੀਲਾਂ, ਪੂਰਨ *ਅੰਕ_Matches, char_u ***matches)
{
    ਵਾਪਸ expand_ਸੈੱਟ_opt_listflag(ਦਲੀਲਾਂ, (char_u*)WW_ALL, ਅੰਕ_Matches, matches);
}

/*
 * The 'wildmode' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_wildmode(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਜੇ (check_opt_wim() == FAIL)
	ਵਾਪਸ e_invalid_ਦਲੀਲ;
    ਵਾਪਸ ਨਿਰਾ;
}

    ਪੂਰਨ
expand_ਸੈੱਟ_wildmode(optexpand_T *ਦਲੀਲਾਂ, ਪੂਰਨ *ਅੰਕ_Matches, char_u ***matches)
{
    ਵਾਪਸ expand_ਸੈੱਟ_opt_string(
	    ਦਲੀਲਾਂ,
	    p_wim_ਮੁੱਲਾਂ,
	    ARRAY_ਲੰਮਾਈ(p_wim_ਮੁੱਲਾਂ) - 1,
	    ਅੰਕ_Matches,
	    matches);
}

/*
 * The 'wildoptions' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_wildoptions(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ did_ਸੈੱਟ_opt_ਲੜੀਆਂ(p_wop, p_wop_ਮੁੱਲਾਂ, TRUE);
}

    ਪੂਰਨ
expand_ਸੈੱਟ_wildoptions(optexpand_T *ਦਲੀਲਾਂ, ਪੂਰਨ *ਅੰਕ_Matches, char_u ***matches)
{
    ਵਾਪਸ expand_ਸੈੱਟ_opt_string(
	    ਦਲੀਲਾਂ,
	    p_wop_ਮੁੱਲਾਂ,
	    ARRAY_ਲੰਮਾਈ(p_wop_ਮੁੱਲਾਂ) - 1,
	    ਅੰਕ_Matches,
	    matches);
}

#ਜੇ defined(FEAT_WAK)
/*
 * The 'winaltkeys' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_winaltkeys(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਅੱਖਰ *errmsg = ਨਿਰਾ;

    ਜੇ (*p_wak == NUL || check_opt_ਲੜੀਆਂ(p_wak, p_wak_ਮੁੱਲਾਂ, FALSE) != OK)
	errmsg = e_invalid_ਦਲੀਲ;
# ifdef FEAT_MENU
#  ਜੇ defined(FEAT_GUI_MOTIF)
    ਨਹੀਂਤਾਂ ਜੇ (gui.ਵਿੱਚ_use)
	gui_motif_ਸੈੱਟ_mnemonics(p_wak[0] == 'y' || p_wak[0] == 'm');
#  elif defined(FEAT_GUI_GTK)
    ਨਹੀਂਤਾਂ ਜੇ (gui.ਵਿੱਚ_use)
	gui_gtk_ਸੈੱਟ_mnemonics(p_wak[0] == 'y' || p_wak[0] == 'm');
#  endif
# endif
    ਵਾਪਸ errmsg;
}

    ਪੂਰਨ
expand_ਸੈੱਟ_winaltkeys(optexpand_T *ਦਲੀਲਾਂ, ਪੂਰਨ *ਅੰਕ_Matches, char_u ***matches)
{
    ਵਾਪਸ expand_ਸੈੱਟ_opt_string(
	    ਦਲੀਲਾਂ,
	    p_wak_ਮੁੱਲਾਂ,
	    ARRAY_ਲੰਮਾਈ(p_wak_ਮੁੱਲਾਂ) - 1,
	    ਅੰਕ_Matches,
	    matches);
}
#ਜੇ_ਅੰਤ

/*
 * The 'wincolor' option is changed.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_wincolor(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    update_wincolor(curwin, ਦਲੀਲਾਂ->os_newval.string);
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * The 'winhighlight' option is changed
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_winhighlight(optset_T *ਦਲੀਲਾਂ)
{
    ਵਾਪਸ update_winhighlight(curwin, ਦਲੀਲਾਂ->os_newval.string);
}

/*
 * Expand 'winhighlight' option.
 */
    ਪੂਰਨ
expand_ਸੈੱਟ_winhighlight(optexpand_T *ਦਲੀਲਾਂ, ਪੂਰਨ *ਅੰਕ_Matches, char_u ***matches)
{
    expand_T	    *xp = ਦਲੀਲਾਂ->oe_xp;

    ਜੇ ((xp->xp_ਨਮੂਨਾ > ਦਲੀਲਾਂ->oe_ਸੈੱਟ_arg && *(xp->xp_ਨਮੂਨਾ-1) == ':')
	    || xp->xp_ਨਮੂਨਾ == ਦਲੀਲਾਂ->oe_ਸੈੱਟ_arg || *(xp->xp_ਨਮੂਨਾ-1) == ',')
    {
	// After a ':' or after a ',', or at the start, so expand highlight
	// group name.

	// If starts with !, then expand 'highlight' occasions.
	ਜੇ (*xp->xp_ਨਮੂਨਾ == '!')
	    ਵਾਪਸ expand_hl_occasions(ਦਲੀਲਾਂ, ਅੰਕ_Matches, matches, '!');
	ਨਹੀਂਤਾਂ
	    ਵਾਪਸ expand_ਸੈੱਟ_opt_generic(
		    ਦਲੀਲਾਂ,
		    get_highlight_name,
		    ਅੰਕ_Matches,
		    matches);
    }

    VIM_CLEAR(*matches);
    ਵਾਪਸ FAIL;
}

    ਪੂਰਨ
expand_ਸੈੱਟ_wincolor(optexpand_T *ਦਲੀਲਾਂ, ਪੂਰਨ *ਅੰਕ_Matches, char_u ***matches)
{
    ਵਾਪਸ expand_ਸੈੱਟ_opt_generic(
	    ਦਲੀਲਾਂ,
	    get_highlight_name,
	    ਅੰਕ_Matches,
	    matches);
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
/*
 * When the 'syntax' option is set, load the syntax of that name.
 */
    ਸਥਿਰ ਖਾਲੀ
do_syntax_autocmd(ਪੂਰਨ ਮੁੱਲ_ਬਦਲਿਆ)
{
    ਸਥਿਰ ਪੂਰਨ syn_recursive = 0;

    ++syn_recursive;
    // Only pass TRUE for "force" when the value changed or not used
    // recursively, to avoid endless recurrence.
    apply_autocmds(EVENT_SYNTAX, curbuf->b_p_syn, curbuf->b_fname,
	    ਮੁੱਲ_ਬਦਲਿਆ || syn_recursive == 1, curbuf);
    curbuf->b_flags |= BF_SYN_ਸੈੱਟ;
    --syn_recursive;
}
#ਜੇ_ਅੰਤ

/*
 * When the 'filetype' option is set, trigger the FileType autocommand.
 */
    ਸਥਿਰ ਖਾਲੀ
do_filetype_autocmd(char_u **varp, ਪੂਰਨ opt_flags, ਪੂਰਨ ਮੁੱਲ_ਬਦਲਿਆ)
{
    // Skip this when called from a modeline and the filetype was already set
    // to this value.
    ਜੇ ((opt_flags & OPT_MODELINE) && !ਮੁੱਲ_ਬਦਲਿਆ)
	ਵਾਪਸ;

    ਸਥਿਰ ਪੂਰਨ  ft_recursive = 0;
    ਪੂਰਨ	    secure_save = secure;

    // Reset the secure flag, since the value of 'filetype' has
    // been checked to be safe.
    secure = 0;

    ++ft_recursive;
    curbuf->b_did_filetype = true;
    // Only pass TRUE for "force" when the value changed or not
    // used recursively, to avoid endless recurrence.
    apply_autocmds(EVENT_FILETYPE, curbuf->b_p_ft, curbuf->b_fname,
	    ਮੁੱਲ_ਬਦਲਿਆ || ft_recursive == 1, curbuf);
    --ft_recursive;
    // Just in case the old "curbuf" is now invalid.
    ਜੇ (varp != &(curbuf->b_p_ft))
	varp = ਨਿਰਾ;

    secure = secure_save;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SPELL
/*
 * When the 'spelllang' option is set, source the spell/LANG.vim file in
 * 'runtimepath'.
 */
    ਸਥਿਰ ਖਾਲੀ
do_spelllang_ਸਰੋਤ(ਖਾਲੀ)
{
    char_u	fname[200];
    char_u	*p;
    char_u	*q = curwin->w_s->b_p_spl;

    // Skip the first name if it is "cjk".
    ਜੇ (STRNCMP(q, "cjk,", 4) == 0)
	q += 4;

    // They could set 'spellcapcheck' depending on the language.  Use the first
    // name in 'spelllang' up to '_region' or '.encoding'.
    ਲਈ (p = q; *p != NUL; ++p)
	ਜੇ (!ASCII_ISALNUM(*p) && *p != '-')
	    ਰੋਕੋ;
    ਜੇ (p > q)
    {
	vim_snprintf((ਅੱਖਰ *)fname, 200, "spell/%.*s.vim",
		(ਪੂਰਨ)(p - q), q);
	ਸਰੋਤ_runtime(fname, DIP_ALL);
    }
}
#ਜੇ_ਅੰਤ

/*
 * Handle string options that need some action to perform when changed.
 * The new value must be allocated.
 * Returns NULL for success, or an untranslated error message for an error.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_string_option(
    ਪੂਰਨ		opt_idx,		// index in options[] table
    char_u	**varp,			// pointer to the option variable
    char_u	*oldval,		// previous value of the option
    char_u	*ਮੁੱਲ,			// new value of the option
    ਅੱਖਰ	*errbuf,		// buffer for errors, or NULL
    size_t	errbuflen,		// length of error buffer
    ਪੂਰਨ		opt_flags,		// OPT_LOCAL and/or OPT_GLOBAL
    ਸੈੱਟ_op_T    op,			// OP_ADDING/OP_PREPENDING/OP_REMOVING
    ਪੂਰਨ		*ਮੁੱਲ_checked)		// value was checked to be safe, no
					// need to set P_INSECURE
{
    ਅੱਖਰ	*errmsg = ਨਿਰਾ;
    long_u	free_oldval = (get_option_flags(opt_idx) & P_ALLOCED);
    opt_did_ਸੈੱਟ_cb_T did_ਸੈੱਟ_cb = get_option_did_ਸੈੱਟ_cb(opt_idx);
    optset_T	ਦਲੀਲਾਂ;

    // 'ttytype' is an alias for 'term'.  Both 'term' and 'ttytype' point to
    // T_NAME.  If 'term' or 'ttytype' is modified, then use the index for the
    // 'term' option.  Only set the P_ALLOCED flag on 'term'.
    ਜੇ (varp == &T_NAME)
    {
	opt_idx = findoption((char_u *)"term");
	ਜੇ (opt_idx >= 0)
	{
	    free_oldval = (get_option_flags(opt_idx) & P_ALLOCED);
	    did_ਸੈੱਟ_cb = get_option_did_ਸੈੱਟ_cb(opt_idx);
	}
    }

    CLEAR_ਖੇਤਰ(ਦਲੀਲਾਂ);

    // Disallow changing some options from secure mode
    ਜੇ ((secure
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SANDBOX
		|| sandbox != 0
#ਜੇ_ਅੰਤ
		) && (get_option_flags(opt_idx) & P_SECURE))
	errmsg = e_not_allowed_ਇੱਥੇ;
    // Check for a "normal" directory or file name in some options.
    ਨਹੀਂਤਾਂ ਜੇ (check_illegal_ਰਸਤਾ_ਨਾਂ(opt_idx, varp))
	errmsg = e_invalid_ਦਲੀਲ;
    ਨਹੀਂਤਾਂ ਜੇ (did_ਸੈੱਟ_cb != ਨਿਰਾ)
    {
	ਦਲੀਲਾਂ.os_varp = (char_u *)varp;
	ਦਲੀਲਾਂ.os_idx = opt_idx;
	ਦਲੀਲਾਂ.os_flags = opt_flags;
	ਦਲੀਲਾਂ.os_op = op;
	ਦਲੀਲਾਂ.os_oldval.string = oldval;
	ਦਲੀਲਾਂ.os_newval.string = ਮੁੱਲ;
	ਦਲੀਲਾਂ.os_errbuf = errbuf;
	ਦਲੀਲਾਂ.os_errbuflen = errbuflen;
	// Invoke the option specific callback function to validate and apply
	// the new option value.
	errmsg = did_ਸੈੱਟ_cb(&ਦਲੀਲਾਂ);

	// The 'keymap', 'filetype' and 'syntax' option callback functions
	// may change the os_value_checked field.
	*ਮੁੱਲ_checked = ਦਲੀਲਾਂ.os_ਮੁੱਲ_checked;
    }

    // If an error is detected, restore the previous value.
    ਜੇ (errmsg != ਨਿਰਾ)
    {
	free_string_option(*varp);
	*varp = oldval;
	// When resetting some values, need to act on it.
	ਜੇ (ਦਲੀਲਾਂ.os_restore_chartab)
	    (ਖਾਲੀ)init_chartab();
	ਜੇ (varp == &p_hl)
	    (ਖਾਲੀ)highlight_ਬਦਲਿਆ();
    }
    ਨਹੀਂਤਾਂ
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	// Remember where the option was set.
	ਸੈੱਟ_option_sctx_idx(opt_idx, opt_flags, current_sctx);
#ਜੇ_ਅੰਤ
	// Free string options that are in allocated memory.
	// Use "free_oldval", because recursiveness may change the flags under
	// our fingers (esp. init_highlight()).
	ਜੇ (free_oldval)
	    free_string_option(oldval);
	ਸੈੱਟ_option_flag(opt_idx, P_ALLOCED);

	ਜੇ ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0
		&& is_global_local_option(opt_idx))
	{
	    // global option with local value set to use global value; free
	    // the local value and make it empty
	    char_u *p = get_option_varp_scope(opt_idx, OPT_LOCAL);
	    free_string_option(*(char_u **)p);
	    *(char_u **)p = ਖਾਲੀ_ਥਾਵਾਂ_option;
	}

	// May set global value for local option.
	ਨਹੀਂਤਾਂ ਜੇ (!(opt_flags & OPT_LOCAL) && opt_flags != OPT_GLOBAL)
	    ਸੈੱਟ_string_option_global(opt_idx, varp);

	// Trigger the autocommand only after setting the flags.
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
	ਜੇ (varp == &(curbuf->b_p_syn))
	    do_syntax_autocmd(ਦਲੀਲਾਂ.os_ਮੁੱਲ_ਬਦਲਿਆ);
#ਜੇ_ਅੰਤ
	ਨਹੀਂਤਾਂ ਜੇ (varp == &(curbuf->b_p_ft))
	    do_filetype_autocmd(varp, opt_flags, ਦਲੀਲਾਂ.os_ਮੁੱਲ_ਬਦਲਿਆ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SPELL
	ਜੇ (varp == &(curwin->w_s->b_p_spl))
	    do_spelllang_ਸਰੋਤ();
#ਜੇ_ਅੰਤ
    }

    ਜੇ (varp == &p_mouse)
    {
	ਜੇ (*p_mouse == NUL)
	    mch_setmouse(FALSE);    // switch mouse off
	ਨਹੀਂਤਾਂ
	    setmouse();		    // in case 'mouse' changed
    }

    ਜੇ (varp == &p_rtp)
    {
	export_myvimdir();
#ਜੇ defined(FEAT_LUA)
	update_package_paths_ਵਿੱਚ_lua();
#ਜੇ_ਅੰਤ
    }

#ਜੇ defined(FEAT_LINEBREAK)
    // Changing Formatlistpattern when briopt includes the list setting:
    // redraw
    ਜੇ ((varp == &p_flp || varp == &(curbuf->b_p_flp))
	    && curwin->w_briopt_list)
	redraw_all_later(UPD_NOT_VALID);
#ਜੇ_ਅੰਤ

    ਜੇ (curwin->w_curswant != MAXCOL
		   && (get_option_flags(opt_idx) & (P_CURSWANT | P_RALL)) != 0
				&& (get_option_flags(opt_idx) & P_HLONLY) == 0)
	curwin->w_ਸੈੱਟ_curswant = true;

    ਜੇ ((opt_flags & OPT_NO_REDRAW) == 0)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
	// set when changing an option that only requires a redraw in the GUI
	ਪੂਰਨ	redraw_gui_only = FALSE;

	ਜੇ (varp == &p_go			// 'guioptions'
		|| varp == &p_guifont		// 'guifont'
# ifdef FEAT_GUI_TABLINE
		|| varp == &p_gtl		// 'guitablabel'
		|| varp == &p_gtt		// 'guitabtooltip'
# endif
# ifdef FEAT_XFONTSET
		|| varp == &p_guifontset	// 'guifontset'
# endif
		|| varp == &p_guifontwide	// 'guifontwide'
# ਜੇ defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
		|| varp == &p_guiligatures	// 'guiligatures'
# endif
	    )
	    redraw_gui_only = TRUE;

	// check redraw when it's not a GUI option or the GUI is active.
	ਜੇ (!redraw_gui_only || gui.ਵਿੱਚ_use)
#ਜੇ_ਅੰਤ
	    check_redraw(get_option_flags(opt_idx));
    }

#ਜੇ defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
    ਜੇ (ਦਲੀਲਾਂ.os_did_swaptcap)
    {
	ਸੈੱਟ_termname((char_u *)"win32");
	init_highlight(TRUE, FALSE);
    }
#ਜੇ_ਅੰਤ

    ਵਾਪਸ errmsg;
}

/*
 * Check an option that can be a range of string values.
 *
 * Return OK for correct value, FAIL otherwise.
 * Empty is always OK.
 */
    ਸਥਿਰ ਪੂਰਨ
check_opt_ਲੜੀਆਂ(
    char_u	*val,
    ਅੱਖਰ	**ਮੁੱਲਾਂ,
    ਪੂਰਨ		list)	    // when TRUE: accept a list of values
{
    ਵਾਪਸ opt_ਲੜੀਆਂ_flags(val, ਮੁੱਲਾਂ, ਨਿਰਾ, list);
}

/*
 * Handle an option that can be a range of string values.
 * Set a flag in "*flagp" for each string present.
 *
 * Return OK for correct value, FAIL otherwise.
 * Empty is always OK.
 */
    ਸਥਿਰ ਪੂਰਨ
opt_ਲੜੀਆਂ_flags(
    char_u	*val,		// new value
    ਅੱਖਰ	**ਮੁੱਲਾਂ,	// array of valid string values
    ਬਿਨਾਨਿਸ਼ਾਨੀ	*flagp,
    ਪੂਰਨ		list)		// when TRUE: accept a list of values
{
    ਪੂਰਨ		i;
    ਪੂਰਨ		len;
    ਬਿਨਾਨਿਸ਼ਾਨੀ	new_flags = 0;

    ਜਦੋਂ (*val)
    {
	ਲਈ (i = 0; ; ++i)
	{
	    ਜੇ (ਮੁੱਲਾਂ[i] == ਨਿਰਾ)	// val not found in values[]
		ਵਾਪਸ FAIL;

	    len = (ਪੂਰਨ)STRLEN(ਮੁੱਲਾਂ[i]);
	    ਜੇ (STRNCMP(ਮੁੱਲਾਂ[i], val, len) == 0
		    && ((list && val[len] == ',') || val[len] == NUL))
	    {
		val += len + (val[len] == ',');
		new_flags |= (1 << i);
		ਰੋਕੋ;		// check next item in val list
	    }
	}
    }
    ਜੇ (flagp != ਨਿਰਾ)
	*flagp = new_flags;

    ਵਾਪਸ OK;
}

/*
 * return OK if "p" is a valid fileformat name, FAIL otherwise.
 */
    ਪੂਰਨ
check_ff_ਮੁੱਲ(char_u *p)
{
    ਵਾਪਸ check_opt_ਲੜੀਆਂ(p, p_ff_ਮੁੱਲਾਂ, FALSE);
}

/*
 * Save the actual shortmess Flags and clear them temporarily to avoid that
 * file messages overwrites any output from the following commands.
 *
 * Caller must make sure to first call save_clear_shm_value() and then
 * restore_shm_value() exactly the same number of times.
 */
    ਖਾਲੀ
save_clear_shm_ਮੁੱਲ(ਖਾਲੀ)
{
    ਜੇ (STRLEN(p_shm) >= SHM_LEN)
    {
	iemsg(e_ਅੰਦਰੂਨੀ_error_shortmess_too_long);
	ਵਾਪਸ;
    }

    ਜੇ (++ਸੈੱਟ_shm_recursive == 1)
    {
	STRCPY(shm_buf, p_shm);
	ਸੈੱਟ_option_ਮੁੱਲ_give_err((char_u *)"shm", 0L, (char_u *)"", 0);
    }
}

/*
 * Restore the shortmess Flags set from the save_clear_shm_value() function.
 */
    ਖਾਲੀ
restore_shm_ਮੁੱਲ(ਖਾਲੀ)
{
    ਜੇ (--ਸੈੱਟ_shm_recursive == 0)
    {
	ਸੈੱਟ_option_ਮੁੱਲ_give_err((char_u *)"shm", 0L, shm_buf, 0);
	vim_memset(shm_buf, 0, SHM_LEN);
    }
}

/*
 * Export the environment variable $MYVIMDIR to the first item in runtimepath
 */
    ਖਾਲੀ
export_myvimdir(ਖਾਲੀ)
{
    ਪੂਰਨ		dofree = FALSE;
    char_u	*p;
    char_u	*q = p_rtp;
    string_T	buf;

    buf.string = alloc(MAXPATHL);
    ਜੇ (buf.string == ਨਿਰਾ)
	ਵਾਪਸ;

    buf.ਲੰਮਾਈ = (size_t)copy_option_part(&q, buf.string, MAXPATHL, ",");

    p = vim_getenv((char_u *)"MYVIMDIR", &dofree);

    ਜੇ (p == ਨਿਰਾ || STRCMP(p, buf.string) != 0)
    {
	ਜੇ (*buf.string != NUL && !after_pathsep(buf.string, buf.string + buf.ਲੰਮਾਈ))
	    STRCPY(buf.string + buf.ਲੰਮਾਈ, PATHSEPSTR);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
	// normalize path separators
	ਲਈ (q = buf.string; *q != NUL; q++)
	    ਜੇ (*q == '/')
		*q = '\\';
#ਜੇ_ਅੰਤ
	vim_setenv((char_u *)"MYVIMDIR", buf.string);
    }
    ਜੇ (dofree)
	vim_free(p);
    vim_free(buf.string);
}
