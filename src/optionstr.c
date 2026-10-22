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

#ଅନ୍ତର୍ଭାଵ "vim.h"

ସ୍ଥିର char_u shm_buf[SHM_LEN];
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା set_shm_recursive = 0;

ସ୍ଥିର ଅକ୍ଷର *(p_ambw_ମୂଲ୍ୟଗୁଡ଼ିକ[]) = {"single", "double", ଖାଲି};
ସ୍ଥିର ଅକ୍ଷର *(p_bg_ମୂଲ୍ୟଗୁଡ଼ିକ[]) = {"light", "dark", ଖାଲି};
ସ୍ଥିର ଅକ୍ଷର *(p_bkc_ମୂଲ୍ୟଗୁଡ଼ିକ[]) = {"yes", "auto", "no", "breaksymlink", "breakhardlink", ଖାଲି};
ସ୍ଥିର ଅକ୍ଷର *(p_bo_ମୂଲ୍ୟଗୁଡ଼ିକ[]) = {"all", "backspace", "cursor", "complete",
				 "copy", "ctrlg", "error", "esc", "ex",
				 "hangul", "insertmode", "lang", "mess",
				 "showmatch", "operator", "register", "shell",
				 "spell", "term", "wildmode", ଖାଲି};
#ଯଦି defined(FEAT_LINEBREAK)
// Note: Keep this in sync with briopt_check()
ସ୍ଥିର ଅକ୍ଷର *(p_briopt_ମୂଲ୍ୟଗୁଡ଼ିକ[]) = {"shift:", "min:", "sbr", "list:", "column:", ଖାଲି};
#endif
#ଯଦି defined(FEAT_TABPANEL)
// Note: Keep this in sync with tabpanelopt_changed()
ସ୍ଥିର ଅକ୍ଷର *(p_tplo_ମୂଲ୍ୟଗୁଡ଼ିକ[]) = {"align:", "columns:", "scrollbar", "vert", ଖାଲି};
ସ୍ଥିର ଅକ୍ଷର *(p_tplo_align_ମୂଲ୍ୟଗୁଡ଼ିକ[]) = {"left", "right", ଖାଲି};
#endif
#ଯଦି defined(FEAT_DIFF)
// Note: Keep this in sync with diffopt_changed()
ସ୍ଥିର ଅକ୍ଷର *(p_dip_ମୂଲ୍ୟଗୁଡ଼ିକ[]) = {"filler", "anchor", "context:", "iblank", "icase", "iwhite", "iwhiteall", "iwhiteeol", "horizontal", "vertical", "closeoff", "hiddenoff", "foldcolumn:", "followwrap", "internal", "indent-heuristic", "algorithm:", "inline:", "linematch:", ଖାଲି};
ସ୍ଥିର ଅକ୍ଷର *(p_dip_algorithm_ମୂଲ୍ୟଗୁଡ଼ିକ[]) = {"myers", "minimal", "patience", "histogram", ଖାଲି};
ସ୍ଥିର ଅକ୍ଷର *(p_dip_inline_ମୂଲ୍ୟଗୁଡ଼ିକ[]) = {"none", "simple", "char", "word", ଖାଲି};
#endif
ସ୍ଥିର ଅକ୍ଷର *(p_nf_ମୂଲ୍ୟଗୁଡ଼ିକ[]) = {"bin", "octal", "hex", "alpha", "unsigned", "blank", ଖାଲି};
ସ୍ଥିର ଅକ୍ଷର *(p_ff_ମୂଲ୍ୟଗୁଡ଼ିକ[]) = {FF_UNIX, FF_DOS, FF_MAC, ଖାଲି};
#ifdef HAVE_CLIPMETHOD
// Note: Keep this in sync with did_set_clipboard()
ସ୍ଥିର ଅକ୍ଷର *(p_cb_ମୂଲ୍ୟଗୁଡ଼ିକ[]) = {"unnamed", "unnamedplus",
# ifdef FEAT_CLIPBOARD
    "autoselect", "autoselectplus", "autoselectml", "html", "exclude:",
# endif
    ଖାଲି};
#endif
#ifdef FEAT_CRYPT
ସ୍ଥିର ଅକ୍ଷର *(p_cm_ମୂଲ୍ୟଗୁଡ଼ିକ[]) = {"zip", "blowfish", "blowfish2",
# ifdef FEAT_SODIUM
    "xchacha20", "xchacha20v2",
# endif
    ଖାଲି};
#endif
ସ୍ଥିର ଅକ୍ଷର *(p_cmp_ମୂଲ୍ୟଗୁଡ଼ିକ[]) = {"internal", "keepascii", ଖାଲି};
#ifdef FEAT_SYN_HL
// Note: Keep this in sync with fill_culopt_flags()
ସ୍ଥିର ଅକ୍ଷର *(p_culopt_ମୂଲ୍ୟଗୁଡ଼ିକ[]) = {"line", "screenline", "number", "both", ଖାଲି};
#endif
ସ୍ଥିର ଅକ୍ଷର *(p_dy_ମୂଲ୍ୟଗୁଡ଼ିକ[]) = {"lastline", "truncate", "uhex", ଖାଲି};
ସ୍ଥିର ଅକ୍ଷର *(p_jop_ମୂଲ୍ୟଗୁଡ଼ିକ[]) = {"stack", ଖାଲି};
#ifdef FEAT_FOLDING
ସ୍ଥିର ଅକ୍ଷର *(p_fdo_ମୂଲ୍ୟଗୁଡ଼ିକ[]) = {"all", "block", "hor", "mark", "percent",
				 "quickfix", "search", "tag", "insert",
				 "undo", "jump", ଖାଲି};
#endif
// Note: Keep this in sync with match_keyprotocol()
ସ୍ଥିର ଅକ୍ଷର *(p_kpc_protocol_ମୂଲ୍ୟଗୁଡ଼ିକ[]) = {"none", "mok2", "kitty", ଖାଲି};
#ifdef FEAT_IMAGE
// Note: Keep this in sync with match_imageprotocol()
ସ୍ଥିର ଅକ୍ଷର *(p_ipc_protocol_ମୂଲ୍ୟଗୁଡ଼ିକ[]) = {"none", "kitty", "sixel", ଖାଲି};
#endif
#ifdef FEAT_PROP_POPUP
// Note: Keep this in sync with parse_popup_option()
ସ୍ଥିର ଅକ୍ଷର *(p_popup_cpp_option_ମୂଲ୍ୟଗୁଡ଼ିକ[]) = {"align:", "border:",
    "borderhighlight:", "close:", "height:", "highlight:", "opacity:",
    "resize:", "shadow:", "width:", ଖାଲି};
ସ୍ଥିର ଅକ୍ଷର *(p_popup_pvp_option_ମୂଲ୍ୟଗୁଡ଼ିକ[]) = {"border:",
    "borderhighlight:", "close:", "height:", "highlight:", "opacity:",
    "resize:", "shadow:", "width:", ଖାଲି};
ସ୍ଥିର ଅକ୍ଷର *(p_popup_option_on_off_ମୂଲ୍ୟଗୁଡ଼ିକ[]) = {"on", "off", ଖାଲି};
ସ୍ଥିର ଅକ୍ଷର *(p_popup_cpp_border_ମୂଲ୍ୟଗୁଡ଼ିକ[]) = {"single", "double", "round",
    "ascii", "on", "off", "custom:", ଖାଲି};
ସ୍ଥିର ଅକ୍ଷର *(p_popup_pvp_border_ମୂଲ୍ୟଗୁଡ଼ିକ[]) = {"single", "double", "round",
    "ascii", "on", "off", "custom:", ଖାଲି};
ସ୍ଥିର ଅକ୍ଷର *(p_popup_option_align_ମୂଲ୍ୟଗୁଡ଼ିକ[]) = {"item", "menu", ଖାଲି};
#endif
#ଯଦି defined(FEAT_SPELL)
// Note: Keep this in sync with spell_check_sps()
ସ୍ଥିର ଅକ୍ଷର *(p_sps_ମୂଲ୍ୟଗୁଡ଼ିକ[]) = {"best", "fast", "double", "expr:", "file:", "timeout:", ଖାଲି};
#endif
#ifdef FEAT_SESSION
// Also used for 'viewoptions'!  Keep in sync with SSOP_ flags.
ସ୍ଥିର ଅକ୍ଷର *(p_ssop_ମୂଲ୍ୟଗୁଡ଼ିକ[]) = {"buffers", "winpos", "resize", "winsize",
    "localoptions", "options", "help", "blank", "globals", "slash", "unix",
    "sesdir", "curdir", "folds", "cursor", "tabpages", "terminal", "skiprtp",
    ଖାଲି};
#endif
#ଯଦି defined(FEAT_STL_OPT)
ସ୍ଥିର ଅକ୍ଷର *(p_stlo_ମୂଲ୍ୟଗୁଡ଼ିକ[]) = {"fixedheight", "maxheight:", ଖାଲି};
#endif
// Keep in sync with SWB_ flags in option.h
ସ୍ଥିର ଅକ୍ଷର *(p_swb_ମୂଲ୍ୟଗୁଡ଼ିକ[]) = {"useopen", "usetab", "split", "newtab", "vsplit", "uselast", ଖାଲି};
ସ୍ଥିର ଅକ୍ଷର *(p_spk_ମୂଲ୍ୟଗୁଡ଼ିକ[]) = {"cursor", "screen", "topline", ଖାଲି};
ସ୍ଥିର ଅକ୍ଷର *(p_tc_ମୂଲ୍ୟଗୁଡ଼ିକ[]) = {"followic", "ignore", "match", "followscs", "smart", ଖାଲି};
// Keep in sync with TCL_ flags in option.h
ସ୍ଥିର ଅକ୍ଷର *(p_tcl_ମୂଲ୍ୟଗୁଡ଼ିକ[]) = {"left", "uselast", ଖାଲି};
#ଯଦି defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
ସ୍ଥିର ଅକ୍ଷର *(p_toolbar_ମୂଲ୍ୟଗୁଡ଼ିକ[]) = {"text", "icons", "tooltips", "horiz", ଖାଲି};
#endif
#ଯଦି defined(FEAT_TOOLBAR) && defined(FEAT_GUI_GTK)
ସ୍ଥିର ଅକ୍ଷର *(p_tbis_ମୂଲ୍ୟଗୁଡ଼ିକ[]) = {"tiny", "small", "medium", "large", "huge", "giant", ଖାଲି};
#endif
#ଯଦି defined(UNIX) || defined(VMS)
ସ୍ଥିର ଅକ୍ଷର *(p_ttym_ମୂଲ୍ୟଗୁଡ଼ିକ[]) = {"xterm", "xterm2", "dec", "netterm", "jsbterm", "pterm", "urxvt", "sgr", ଖାଲି};
#endif
ସ୍ଥିର ଅକ୍ଷର *(p_ve_ମୂଲ୍ୟଗୁଡ଼ିକ[]) = {"block", "insert", "all", "onemore", "none", "NONE", ଖାଲି};
// Note: Keep this in sync with check_opt_wim()
ସ୍ଥିର ଅକ୍ଷର *(p_wim_ମୂଲ୍ୟଗୁଡ଼ିକ[]) = {"full", "longest", "list", "lastused", "noselect", "noinsert", ଖାଲି};
ସ୍ଥିର ଅକ୍ଷର *(p_wop_ମୂଲ୍ୟଗୁଡ଼ିକ[]) = {"fuzzy", "tagfile", "pum", "exacttext", ଖାଲି};
#ifdef FEAT_WAK
ସ୍ଥିର ଅକ୍ଷର *(p_wak_ମୂଲ୍ୟଗୁଡ଼ିକ[]) = {"yes", "menu", "no", ଖାଲି};
#endif
ସ୍ଥିର ଅକ୍ଷର *(p_mousem_ମୂଲ୍ୟଗୁଡ଼ିକ[]) = {"extend", "popup", "popup_setpos", "mac", ଖାଲି};
ସ୍ଥିର ଅକ୍ଷର *(p_sel_ମୂଲ୍ୟଗୁଡ଼ିକ[]) = {"inclusive", "exclusive", "old", ଖାଲି};
ସ୍ଥିର ଅକ୍ଷର *(p_slm_ମୂଲ୍ୟଗୁଡ଼ିକ[]) = {"mouse", "key", "cmd", ଖାଲି};
ସ୍ଥିର ଅକ୍ଷର *(p_km_ମୂଲ୍ୟଗୁଡ଼ିକ[]) = {"startsel", "stopsel", ଖାଲି};
#ifdef FEAT_BROWSE
ସ୍ଥିର ଅକ୍ଷର *(p_bsdir_ମୂଲ୍ୟଗୁଡ଼ିକ[]) = {"current", "last", "buffer", ଖାଲି};
#endif
ସ୍ଥିର ଅକ୍ଷର *(p_scbopt_ମୂଲ୍ୟଗୁଡ଼ିକ[]) = {"ver", "hor", "jump", ଖାଲି};
ସ୍ଥିର ଅକ୍ଷର *(p_debug_ମୂଲ୍ୟଗୁଡ଼ିକ[]) = {"msg", "throw", "beep", ଖାଲି};
ସ୍ଥିର ଅକ୍ଷର *(p_ead_ମୂଲ୍ୟଗୁଡ଼ିକ[]) = {"both", "ver", "hor", ଖାଲି};
ସ୍ଥିର ଅକ୍ଷର *(p_buftype_ମୂଲ୍ୟଗୁଡ଼ିକ[]) = {"nofile", "nowrite", "quickfix", "help", "terminal", "acwrite", "prompt", "popup", ଖାଲି};
ସ୍ଥିର ଅକ୍ଷର *(p_bufhidden_ମୂଲ୍ୟଗୁଡ଼ିକ[]) = {"hide", "unload", "delete", "wipe", ଖାଲି};
ସ୍ଥିର ଅକ୍ଷର *(p_bs_ମୂଲ୍ୟଗୁଡ଼ିକ[]) = {"indent", "eol", "start", "nostop", ଖାଲି};
#ifdef FEAT_FOLDING
ସ୍ଥିର ଅକ୍ଷର *(p_fdm_ମୂଲ୍ୟଗୁଡ଼ିକ[]) = {"manual", "expr", "marker", "indent", "syntax",
# ifdef FEAT_DIFF
				"diff",
# endif
				ଖାଲି};
ସ୍ଥିର ଅକ୍ଷର *(p_fcl_ମୂଲ୍ୟଗୁଡ଼ିକ[]) = {"all", ଖାଲି};
#endif
ସ୍ଥିର ଅକ୍ଷର *(p_cfc_ମୂଲ୍ୟଗୁଡ଼ିକ[]) = {"keyword", "files", "whole_line", ଖାଲି};
ସ୍ଥିର ଅକ୍ଷର *(p_cot_ମୂଲ୍ୟଗୁଡ଼ିକ[]) = {"menu", "menuone", "longest", "preview", "popup", "popuphidden", "noinsert", "noselect", "fuzzy", "nosort", "preinsert", "nearest", ଖାଲି};
#ifdef BACKSLASH_IN_FILENAME
ସ୍ଥିର ଅକ୍ଷର *(p_csl_ମୂଲ୍ୟଗୁଡ଼ିକ[]) = {"slash", "backslash", ଖାଲି};
#endif
#ifdef FEAT_SIGNS
ସ୍ଥିର ଅକ୍ଷର *(p_scl_ମୂଲ୍ୟଗୁଡ଼ିକ[]) = {"yes", "no", "auto", "number", ଖାଲି};
#endif
#ifdef UNIX
ସ୍ଥିର ଅକ୍ଷର *(p_trz_ମୂଲ୍ୟଗୁଡ଼ିକ[]) = {"inband", "sigwinch", "", ଖାଲି};
#endif
#ଯଦି defined(MSWIN) && defined(FEAT_TERMINAL)
ସ୍ଥିର ଅକ୍ଷର *(p_twt_ମୂଲ୍ୟଗୁଡ଼ିକ[]) = {"winpty", "conpty", "", ଖାଲି};
#endif
ସ୍ଥିର ଅକ୍ଷର *(p_sloc_ମୂଲ୍ୟଗୁଡ଼ିକ[]) = {"last", "statusline", "tabline", ଖାଲି};
ସ୍ଥିର ଅକ୍ଷର *(p_sws_ମୂଲ୍ୟଗୁଡ଼ିକ[]) = {"fsync", "sync", ଖାଲି};

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା check_opt_strings(char_u *val, ଅକ୍ଷର **ମୂଲ୍ୟଗୁଡ଼ିକ, ପୂର୍ଣ୍ଣସଂଖ୍ୟା list);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା opt_strings_flags(char_u *val, ଅକ୍ଷର **ମୂଲ୍ୟଗୁଡ଼ିକ, unsigned *flagp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା list);

/*
 * After setting various option values: recompute variables that depend on
 * option values.
 */
    ଶୂନ୍ୟ
didset_string_options(ଶୂନ୍ୟ)
{
    (ଶୂନ୍ୟ)opt_strings_flags(p_cmp, p_cmp_ମୂଲ୍ୟଗୁଡ଼ିକ, &cmp_flags, TRUE);
    (ଶୂନ୍ୟ)opt_strings_flags(p_bkc, p_bkc_ମୂଲ୍ୟଗୁଡ଼ିକ, &bkc_flags, TRUE);
    (ଶୂନ୍ୟ)opt_strings_flags(p_bo, p_bo_ମୂଲ୍ୟଗୁଡ଼ିକ, &bo_flags, TRUE);
    (ଶୂନ୍ୟ)opt_strings_flags(p_cot, p_cot_ମୂଲ୍ୟଗୁଡ଼ିକ, &cot_flags, TRUE);
#ifdef FEAT_SESSION
    (ଶୂନ୍ୟ)opt_strings_flags(p_ssop, p_ssop_ମୂଲ୍ୟଗୁଡ଼ିକ, &ssop_flags, TRUE);
    (ଶୂନ୍ୟ)opt_strings_flags(p_vop, p_ssop_ମୂଲ୍ୟଗୁଡ଼ିକ, &vop_flags, TRUE);
#endif
#ifdef FEAT_FOLDING
    (ଶୂନ୍ୟ)opt_strings_flags(p_fdo, p_fdo_ମୂଲ୍ୟଗୁଡ଼ିକ, &fdo_flags, TRUE);
#endif
    (ଶୂନ୍ୟ)opt_strings_flags(p_dy, p_dy_ମୂଲ୍ୟଗୁଡ଼ିକ, &dy_flags, TRUE);
    (ଶୂନ୍ୟ)opt_strings_flags(p_jop, p_jop_ମୂଲ୍ୟଗୁଡ଼ିକ, &jop_flags, TRUE);
    (ଶୂନ୍ୟ)opt_strings_flags(p_tc, p_tc_ମୂଲ୍ୟଗୁଡ଼ିକ, &tc_flags, FALSE);
    (ଶୂନ୍ୟ)opt_strings_flags(p_ve, p_ve_ମୂଲ୍ୟଗୁଡ଼ିକ, &ve_flags, TRUE);
#ଯଦି defined(UNIX) || defined(VMS)
    (ଶୂନ୍ୟ)opt_strings_flags(p_ttym, p_ttym_ମୂଲ୍ୟଗୁଡ଼ିକ, &ttym_flags, FALSE);
#endif
#ଯଦି defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
    (ଶୂନ୍ୟ)opt_strings_flags(p_toolbar, p_toolbar_ମୂଲ୍ୟଗୁଡ଼ିକ, &toolbar_flags, TRUE);
#endif
#ଯଦି defined(FEAT_TOOLBAR) && defined(FEAT_GUI_GTK)
    (ଶୂନ୍ୟ)opt_strings_flags(p_tbis, p_tbis_ମୂଲ୍ୟଗୁଡ଼ିକ, &tbis_flags, FALSE);
#endif
    (ଶୂନ୍ୟ)opt_strings_flags(p_swb, p_swb_ମୂଲ୍ୟଗୁଡ଼ିକ, &swb_flags, TRUE);
    (ଶୂନ୍ୟ)opt_strings_flags(p_tcl, p_tcl_ମୂଲ୍ୟଗୁଡ଼ିକ, &tcl_flags, TRUE);
}

#ଯଦି defined(FEAT_EVAL)
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
    ଶୂନ୍ୟ
trigger_optionset_string(
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	opt_idx,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	opt_flags,
	char_u  *oldval,
	char_u  *oldval_l,
	char_u  *oldval_g,
	char_u  *newval)
{
    // Don't do this recursively.
    ଯଦି (oldval == ଖାଲି || newval == ଖାଲି
				    || *get_vim_var_str(VV_OPTION_TYPE) != NUL)
	ଫେରନ୍ତୁ;

    char_u buf_type[7];
    size_t buf_typelen;
    size_t oldvallen;

    oldvallen = STRLEN(oldval);
    set_vim_var_string(VV_OPTION_OLD, oldval, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)oldvallen);
    set_vim_var_string(VV_OPTION_NEW, newval, -1);
    buf_typelen = vim_snprintf_safelen((ଅକ୍ଷର *)buf_type, ଆକାର(buf_type),
	"%s", (opt_flags & OPT_LOCAL) ? "local" : "global");
    set_vim_var_string(VV_OPTION_TYPE, buf_type, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)buf_typelen);
    ଯଦି (opt_flags & OPT_LOCAL)
    {
	set_vim_var_string(VV_OPTION_COMMAND, (char_u *)"setlocal", STRLEN_LITERAL("setlocal"));
	set_vim_var_string(VV_OPTION_OLDLOCAL, oldval, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)oldvallen);
    }
    ଯଦି (opt_flags & OPT_GLOBAL)
    {
	set_vim_var_string(VV_OPTION_COMMAND, (char_u *)"setglobal", STRLEN_LITERAL("setglobal"));
	set_vim_var_string(VV_OPTION_OLDGLOBAL, oldval, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)oldvallen);
    }
    ଯଦି ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	set_vim_var_string(VV_OPTION_COMMAND, (char_u *)"set", STRLEN_LITERAL("set"));
	set_vim_var_string(VV_OPTION_OLDLOCAL, oldval_l, -1);
	set_vim_var_string(VV_OPTION_OLDGLOBAL, oldval_g, -1);
    }
    ଯଦି (opt_flags & OPT_MODELINE)
    {
	set_vim_var_string(VV_OPTION_COMMAND, (char_u *)"modeline", STRLEN_LITERAL("modeline"));
	set_vim_var_string(VV_OPTION_OLDLOCAL, oldval, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)oldvallen);
    }
    apply_autocmds(EVENT_OPTIONSET,
	    get_option_fullname(opt_idx), ଖାଲି, FALSE,
	    ଖାଲି);
    reset_v_option_vars();
}
#endif

    ସ୍ଥିର ଅକ୍ଷର *
illegal_char(ଅକ୍ଷର *errbuf, size_t errbuflen, ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    ଯଦି (errbuf == ଖାଲି)
	ଫେରନ୍ତୁ "";
    vim_snprintf(errbuf, errbuflen, _(e_illegal_character_str),
		    (ଅକ୍ଷର *)transchar(c));
    ଫେରନ୍ତୁ errbuf;
}

    ସ୍ଥିର ଅକ୍ଷର *
illegal_char_after_chr(ଅକ୍ଷର *errbuf, size_t errbuflen, ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    ଯଦି (errbuf == ଖାଲି)
	ଫେରନ୍ତୁ "";
    vim_snprintf(errbuf, errbuflen, _(e_illegal_character_after_chr), c);
    ଫେରନ୍ତୁ errbuf;
}

/*
 * Check string options in a buffer for NULL value.
 */
    ଶୂନ୍ୟ
check_buf_options(buf_T *buf)
{
    check_string_option(&buf->b_p_bh);
    check_string_option(&buf->b_p_bt);
    check_string_option(&buf->b_p_fenc);
    check_string_option(&buf->b_p_ff);
#ifdef FEAT_FIND_ID
    check_string_option(&buf->b_p_def);
    check_string_option(&buf->b_p_inc);
# ifdef FEAT_EVAL
    check_string_option(&buf->b_p_inex);
# endif
#endif
#ଯଦି defined(FEAT_EVAL)
    check_string_option(&buf->b_p_inde);
    check_string_option(&buf->b_p_indk);
#endif
#ଯଦି defined(FEAT_BEVAL) && defined(FEAT_EVAL)
    check_string_option(&buf->b_p_bexpr);
#endif
#ଯଦି defined(FEAT_CRYPT)
    check_string_option(&buf->b_p_cm);
#endif
    check_string_option(&buf->b_p_fp);
#ଯଦି defined(FEAT_EVAL)
    check_string_option(&buf->b_p_fex);
#endif
#ifdef FEAT_CRYPT
    check_string_option(&buf->b_p_ଚାବି);
#endif
    check_string_option(&buf->b_p_kp);
    check_string_option(&buf->b_p_mps);
    check_string_option(&buf->b_p_fo);
    check_string_option(&buf->b_p_flp);
    check_string_option(&buf->b_p_isk);
    check_string_option(&buf->b_p_com);
    check_string_option(&buf->b_p_cms);
    check_string_option(&buf->b_p_nf);
    check_string_option(&buf->b_p_qe);
#ifdef FEAT_SYN_HL
    check_string_option(&buf->b_p_syn);
    check_string_option(&buf->b_s.b_syn_isk);
#endif
#ifdef FEAT_SPELL
    check_string_option(&buf->b_s.b_p_spc);
    check_string_option(&buf->b_s.b_p_spf);
    check_string_option(&buf->b_s.b_p_spl);
    check_string_option(&buf->b_s.b_p_spo);
#endif
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
#ifdef FEAT_COMPL_FUNC
    check_string_option(&buf->b_p_cfu);
    check_string_option(&buf->b_p_ofu);
    check_string_option(&buf->b_p_tsrfu);
#endif
#ifdef FEAT_EVAL
    check_string_option(&buf->b_p_ffu);
    check_string_option(&buf->b_p_tfu);
#endif
#ifdef FEAT_KEYMAP
    check_string_option(&buf->b_p_keymap);
#endif
#ifdef FEAT_QUICKFIX
    check_string_option(&buf->b_p_gefm);
    check_string_option(&buf->b_p_gp);
    check_string_option(&buf->b_p_mp);
    check_string_option(&buf->b_p_efm);
#endif
    check_string_option(&buf->b_p_ep);
    check_string_option(&buf->b_p_path);
    check_string_option(&buf->b_p_tags);
    check_string_option(&buf->b_p_tc);
    check_string_option(&buf->b_p_dict);
#ifdef FEAT_DIFF
    check_string_option(&buf->b_p_dia);
#endif
    check_string_option(&buf->b_p_tsr);
    check_string_option(&buf->b_p_lw);
    check_string_option(&buf->b_p_bkc);
    check_string_option(&buf->b_p_menc);
#ifdef FEAT_VARTABS
    check_string_option(&buf->b_p_vsts);
    check_string_option(&buf->b_p_vts);
#endif
}

/*
 * Free the string allocated for an option.
 * Checks for the string being empty_option. This may happen if we're out of
 * memory, vim_strsave() returned NULL, which was replaced by empty_option by
 * check_options().
 * Does NOT check for P_ALLOCED flag!
 */
    ଶୂନ୍ୟ
free_string_option(char_u *p)
{
    ଯଦି (p != empty_option)
	vim_free(p);
}

    ଶୂନ୍ୟ
clear_string_option(char_u **pp)
{
    ଯଦି (*pp != empty_option)
	vim_free(*pp);
    *pp = empty_option;
}

    ଶୂନ୍ୟ
check_string_option(char_u **pp)
{
    ଯଦି (*pp == ଖାଲି)
	*pp = empty_option;
}

/*
 * Set global value for string option when it's a local option.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
set_string_option_global(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		opt_idx,	// option index
    char_u	**varp)		// pointer to option variable
{
    char_u	**p, *s;

    // the global value is always allocated
    ଯଦି (is_window_local_option(opt_idx))
	p = (char_u **)GLOBAL_WO(varp);
    ଅନ୍ୟଥା
	p = (char_u **)get_option_var(opt_idx);
    ଯଦି (!is_global_option(opt_idx)
	    && p != varp
	    && (s = vim_strsave(*varp)) != ଖାଲି)
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
    ଶୂନ୍ୟ
set_string_option_direct(
    char_u	*name,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		opt_idx,
    char_u	*val,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		opt_flags,	// OPT_FREE, OPT_LOCAL and/or OPT_GLOBAL
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		set_sid UNUSED)
{
    char_u	*s;
    char_u	**varp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		both = (opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx = opt_idx;

    ଯଦି (idx == -1)		// use name
    {
	idx = findoption(name);
	ଯଦି (idx < 0)	// not found (should not happen)
	{
	    semsg(_(e_internal_error_str), "set_string_option_direct()");
	    siemsg("For option %s", name);
	    ଫେରନ୍ତୁ;
	}
    }

    ଯଦି (is_hidden_option(idx))		// can't set hidden option
	ଫେରନ୍ତୁ;

    s = vim_strsave(val);
    ଯଦି (s == ଖାଲି)
	ଫେରନ୍ତୁ;

    varp = (char_u **)get_option_varp_scope(idx,
	    both ? OPT_LOCAL : opt_flags);
    ଯଦି ((opt_flags & OPT_FREE) && (get_option_flags(idx) & P_ALLOCED))
	free_string_option(*varp);
    *varp = s;

    // For buffer/window local option may also set the global value.
    ଯଦି (both)
	set_string_option_global(idx, varp);

    set_option_flag(idx, P_ALLOCED);

    // When setting both values of a global option with a local value,
    // make the local value empty, so that the global value is used.
    ଯଦି (is_global_local_option(idx) && both)
    {
	free_string_option(*varp);
	*varp = empty_option;
    }
#ifdef FEAT_EVAL
    ଯଦି (set_sid != SID_NONE)
    {
	sctx_T script_ctx;

	ଯଦି (set_sid == 0)
	    script_ctx = current_sctx;
	ଅନ୍ୟଥା
	{
	    script_ctx.sc_sid = set_sid;
	    script_ctx.sc_seq = 0;
	    script_ctx.sc_lnum = 0;
	    script_ctx.sc_version = 1;
	}
	set_option_sctx_idx(idx, opt_flags, script_ctx);
    }
#endif
}

/*
 * Like set_string_option_direct(), but for a window-local option in "wp".
 * Blocks autocommands to avoid the old curwin becoming invalid.
 */
    ଶୂନ୍ୟ
set_string_option_direct_in_win(
	win_T		*wp,
	char_u		*name,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		opt_idx,
	char_u		*val,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		opt_flags,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		set_sid)
{
    win_T	*save_curwin = curwin;

    block_autocmds();
    curwin = wp;
    curbuf = curwin->w_buffer;
    set_string_option_direct(name, opt_idx, val, opt_flags, set_sid);
    curwin = save_curwin;
    curbuf = curwin->w_buffer;
    unblock_autocmds();
}

#ଯଦି defined(FEAT_PROP_POPUP)
/*
 * Like set_string_option_direct(), but for a buffer-local option in "buf".
 * Blocks autocommands to avoid the old curbuf becoming invalid.
 */
    ଶୂନ୍ୟ
set_string_option_direct_in_buf(
	buf_T		*buf,
	char_u		*name,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		opt_idx,
	char_u		*val,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		opt_flags,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		set_sid)
{
    buf_T	*save_curbuf = curbuf;

    block_autocmds();
    curbuf = buf;
    curwin->w_buffer = curbuf;
    set_string_option_direct(name, opt_idx, val, opt_flags, set_sid);
    curbuf = save_curbuf;
    curwin->w_buffer = curbuf;
    unblock_autocmds();
}
#endif

/*
 * Set a string option to a new value, and handle the effects.
 *
 * Returns NULL on success or an untranslated error message on error.
 */
    ଅକ୍ଷର *
set_string_option(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		opt_idx,
    char_u	*ମୂଲ୍ୟ,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		opt_flags,	// OPT_LOCAL and/or OPT_GLOBAL
    ଅକ୍ଷର	*errbuf,
    size_t	errbuflen)
{
    char_u	*s;
    char_u	**varp;
    char_u	*oldval;
#ଯଦି defined(FEAT_EVAL)
    char_u	*oldval_l = ଖାଲି;
    char_u	*oldval_g = ଖାଲି;
    char_u	*saved_oldval = ଖାଲି;
    char_u	*saved_oldval_l = ଖାଲି;
    char_u	*saved_oldval_g = ଖାଲି;
    char_u	*saved_newval = ଖାଲି;
#endif
    ଅକ୍ଷର	*errmsg = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ମୂଲ୍ୟ_checked = FALSE;

    ଯଦି (is_hidden_option(opt_idx))	// don't set hidden option
	ଫେରନ୍ତୁ ଖାଲି;

    s = vim_strsave(ମୂଲ୍ୟ == ଖାଲି ? (char_u *)"" : ମୂଲ୍ୟ);
    ଯଦି (s == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    varp = (char_u **)get_option_varp_scope(opt_idx,
	    (opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0
	    ? (is_global_local_option(opt_idx)
		? OPT_GLOBAL : OPT_LOCAL)
	    : opt_flags);
    oldval = *varp;
#ଯଦି defined(FEAT_EVAL)
    ଯଦି ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	oldval_l = *(char_u **)get_option_varp_scope(opt_idx, OPT_LOCAL);
	oldval_g = *(char_u **)get_option_varp_scope(opt_idx, OPT_GLOBAL);
    }
#endif
    *varp = s;

#ଯଦି defined(FEAT_EVAL)
    ଯଦି (!starting
# ifdef FEAT_CRYPT
	    && !is_crypt_ଚାବି_option(opt_idx)
# endif
       )
    {
	ଯଦି (oldval_l != ଖାଲି)
	    saved_oldval_l = vim_strsave(oldval_l);
	ଯଦି (oldval_g != ଖାଲି)
	    saved_oldval_g = vim_strsave(oldval_g);
	saved_oldval = vim_strsave(oldval);
	saved_newval = vim_strsave(s);
    }
#endif
    ଯଦି ((errmsg = did_set_string_option(opt_idx, varp, oldval, ମୂଲ୍ୟ, errbuf,
		    errbuflen, opt_flags, OP_NONE, &ମୂଲ୍ୟ_checked)) == ଖାଲି)
	did_set_option(opt_idx, opt_flags, TRUE, ମୂଲ୍ୟ_checked);

#ଯଦି defined(FEAT_EVAL)
    // call autocommand after handling side effects
    ଯଦି (errmsg == ଖାଲି)
	trigger_optionset_string(opt_idx, opt_flags,
		saved_oldval, saved_oldval_l,
		saved_oldval_g, saved_newval);
    vim_free(saved_oldval);
    vim_free(saved_oldval_l);
    vim_free(saved_oldval_g);
    vim_free(saved_newval);
#endif
    ଫେରନ୍ତୁ errmsg;
}

/*
 * Return TRUE if "val" is a valid 'filetype' name.
 * Also used for 'syntax' and 'keymap'.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
valid_filetype(char_u *val)
{
    ଫେରନ୍ତୁ valid_name(val, ".-_");
}

#ifdef FEAT_STL_OPT
/*
 * Check validity of options with the 'statusline' format.
 * Return an untranslated error message or NULL.
 */
    ସ୍ଥିର ଅକ୍ଷର *
check_stl_option(char_u *s)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		groupdepth = 0;
    ସ୍ଥିର ଅକ୍ଷର errbuf[ERR_BUFLEN];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		errbuflen = ERR_BUFLEN;

    ଯେତେବେଳେ (*s)
    {
	// Check for valid keys after % sequences
	ଯେତେବେଳେ (*s && *s != '%')
	    s++;
	ଯଦି (!*s)
	    ଭାଙ୍ଗ;
	s++;
	ଯଦି (*s == STL_CLICKFUNC)
	{
	    ଯଦି (s[1] == ']')
	    {
		// %[] - end click region
		s += 2;
		ଜାରି;
	    }
	    ଯଦି (ASCII_ISALPHA(s[1]) || s[1] == '_')
	    {
		// %[FuncName] - start click region
		char_u *rb = vim_strchr(s + 2, ']');
		ଯଦି (rb != ଖାଲି)
		{
		    s = rb + 1;
		    ଜାରି;
		}
	    }
	    // Bare %[ is invalid
	    ଫେରନ୍ତୁ illegal_char(errbuf, errbuflen, *s);
	}
	ଯଦି (*s == STL_LINEBREAK)
	{
	    // Plain %@ - line break
	    s++;
	    ଜାରି;
	}
	ଯଦି (*s == '%' || *s == STL_TRUNCMARK || *s == STL_SEPARATE)
	{
	    s++;
	    ଜାରି;
	}
	ଯଦି (*s == ')')
	{
	    s++;
	    ଯଦି (--groupdepth < 0)
		ଭାଙ୍ଗ;
	    ଜାରି;
	}
	ଯଦି (*s == '-')
	    s++;
	ଯେତେବେଳେ (VIM_ISDIGIT(*s))
	    s++;
	ଯଦି (*s == STL_USER_HL)
	    ଜାରି;
	ଯଦି (*s == STL_CLICKFUNC)
	{
	    // %N[FuncName] or %N[]
	    ଯଦି (s[1] == ']')
	    {
		s += 2;
		ଜାରି;
	    }
	    ଯଦି (ASCII_ISALPHA(s[1]) || s[1] == '_')
	    {
		char_u *rb = vim_strchr(s + 2, ']');
		ଯଦି (rb != ଖାଲି)
		{
		    s = rb + 1;
		    ଜାରି;
		}
	    }
	    // Bare %N[ is invalid
	    ଫେରନ୍ତୁ illegal_char(errbuf, errbuflen, *s);
	}
	ଯଦି (*s == STL_LINEBREAK)
	{
	    // %N@ - line break
	    s++;
	    ଜାରି;
	}
	ଯଦି (*s == '.')
	{
	    s++;
	    ଯେତେବେଳେ (*s && VIM_ISDIGIT(*s))
		s++;
	}
	ଯଦି (*s == '(')
	{
	    groupdepth++;
	    ଜାରି;
	}
	ଯଦି (vim_strchr(STL_ALL, *s) == ଖାଲି)
	{
	    ଫେରନ୍ତୁ illegal_char(errbuf, errbuflen, *s);
	}
	ଯଦି (*s == '{')
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା reevaluate = (*++s == '%');

	    ଯଦି (reevaluate && *++s == '}')
		// "}" is not allowed immediately after "%{%"
		ଫେରନ୍ତୁ illegal_char(errbuf, errbuflen, '}');
	    ଯେତେବେଳେ ((*s != '}' || (reevaluate && s[-1] != '%')) && *s)
		s++;
	    ଯଦି (*s != '}')
		ଫେରନ୍ତୁ e_unclosed_expression_sequence;
	}
    }
    ଯଦି (groupdepth != 0)
	ଫେରନ୍ତୁ e_unbalanced_groups;
    ଫେରନ୍ତୁ ଖାଲି;
}
#endif

/*
 * Check for a "normal" directory or file name in some options.  Disallow a
 * path separator (slash and/or backslash), wildcards and characters that are
 * often illegal in a file name. Be more permissive if "secure" is off.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_illegal_path_names(ପୂର୍ଣ୍ଣସଂଖ୍ୟା opt_idx, char_u **varp)
{
    ଫେରନ୍ତୁ (((get_option_flags(opt_idx) & P_NFNAME)
		&& vim_strpbrk(*varp, (char_u *)(secure
			? "/\\*?[|;&<>\r\n" : "/\\*?[<>\r\n")) != ଖାଲି)
	    || ((get_option_flags(opt_idx) & P_NDNAME)
		&& vim_strpbrk(*varp, (char_u *)"*?[|;&<>\r\n") != ଖାଲି));
}

/*
 * An option that accepts a list of flags is changed.
 * e.g. 'viewoptions', 'switchbuf', 'casemap', etc.
 */
    ସ୍ଥିର ଅକ୍ଷର *
did_set_opt_flags(char_u *val, ଅକ୍ଷର **ମୂଲ୍ୟଗୁଡ଼ିକ, unsigned *flagp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା list)
{
    ଯଦି (opt_strings_flags(val, ମୂଲ୍ୟଗୁଡ଼ିକ, flagp, list) == FAIL)
	ଫେରନ୍ତୁ e_invalid_argument;

    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * An option that accepts a list of string values is changed.
 * e.g. 'nrformats', 'scrollopt', 'wildoptions', etc.
 */
    ସ୍ଥିର ଅକ୍ଷର *
did_set_opt_strings(char_u *val, ଅକ୍ଷର **ମୂଲ୍ୟଗୁଡ଼ିକ, ପୂର୍ଣ୍ଣସଂଖ୍ୟା list)
{
    ଫେରନ୍ତୁ did_set_opt_flags(val, ମୂଲ୍ୟଗୁଡ଼ିକ, ଖାଲି, list);
}

/*
 * An option which is a list of flags is set.  Valid values are in 'flags'.
 */
    ସ୍ଥିର ଅକ୍ଷର *
did_set_option_listflag(
	char_u *val,
	char_u *flags,
	ଅକ୍ଷର *errbuf,
	size_t errbuflen)
{
    char_u	*s;

    ପାଇଁ (s = val; *s; ++s)
	ଯଦି (vim_strchr(flags, *s) == ଖାଲି)
	    ଫେରନ୍ତୁ illegal_char(errbuf, errbuflen, *s);

    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Expand an option that accepts a list of fixed string values with known
 * number of items.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_set_opt_string(
	optexpand_T *args,
	ଅକ୍ଷର **ମୂଲ୍ୟଗୁଡ଼ିକ,
	size_t ଅଙ୍କ_ମୂଲ୍ୟଗୁଡ଼ିକ,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_Matches,
	char_u ***matches)
{
    char_u	*p;
    regmatch_T	*regmatch = args->oe_regmatch;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		include_orig_val = args->oe_include_orig_val;
    char_u	*option_val = args->oe_opt_ମୂଲ୍ୟ;

    // Assume numValues is small since they are fixed enums, so just allocate
    // upfront instead of needing two passes to calculate output size.
    *matches = ALLOC_MULT(char_u *, ଅଙ୍କ_ମୂଲ୍ୟଗୁଡ଼ିକ + 1);
    ଯଦି (*matches == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣନା_2 = 0;

    ଯଦି (include_orig_val && *option_val != NUL)
    {
	p = vim_strsave(option_val);
	ଯଦି (p == ଖାଲି)
	{
	    VIM_CLEAR(*matches);
	    ଫେରନ୍ତୁ FAIL;
	}
	(*matches)[ଗଣନା_2++] = p;
    }

    ପାଇଁ (ଅକ୍ଷର **val = ମୂଲ୍ୟଗୁଡ଼ିକ; *val != ଖାଲି; val++)
    {
	ଯଦି (include_orig_val && *option_val != NUL)
	{
	    ଯଦି (STRCMP((char_u*)*val, option_val) == 0)
		ଜାରି;
	}
	ଯଦି (vim_regexec(regmatch, (char_u*)(*val), (colnr_T)0))
	{
	    p = vim_strsave((char_u*)*val);
	    ଯଦି (p == ଖାଲି)
	    {
		ଯଦି (ଗଣନା_2 == 0)
		{
		    VIM_CLEAR(*matches);
		    ଫେରନ୍ତୁ FAIL;
		}
		ଅନ୍ୟଥା
		    ଭାଙ୍ଗ;
	    }
	    (*matches)[ଗଣନା_2++] = p;
	}
    }
    ଯଦି (ଗଣନା_2 == 0)
    {
	VIM_CLEAR(*matches);
	ଫେରନ୍ତୁ FAIL;
    }
    *ଅଙ୍କ_Matches = ଗଣନା_2;
    ଫେରନ୍ତୁ OK;
}

ସ୍ଥିର char_u *set_opt_callback_orig_option = ଖାଲି;
ସ୍ଥିର char_u *((*set_opt_callback_func)(expand_T *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା));

/*
 * Callback used by expand_set_opt_generic to also include the original value
 * as the first item.
 */
    ସ୍ଥିର char_u *
expand_set_opt_generic_cb(expand_T *xp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଯଦି (idx == 0)
    {
	ଯଦି (set_opt_callback_orig_option != ଖାଲି)
	    ଫେରନ୍ତୁ set_opt_callback_orig_option;
	ଅନ୍ୟଥା
	    ଫେରନ୍ତୁ (char_u *)""; // empty strings are ignored
    }
    ଫେରନ୍ତୁ set_opt_callback_func(xp, idx - 1);
}

/*
 * Expand an option with a callback that iterates through a list of possible
 * names using an index.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_set_opt_generic(
	optexpand_T *args,
	char_u *((*func)(expand_T *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା)),
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_Matches,
	char_u ***matches)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret;

    set_opt_callback_orig_option = args->oe_include_orig_val ?
	args->oe_opt_ମୂଲ୍ୟ : ଖାଲି;
    set_opt_callback_func = func;

    ret = ExpandGeneric(
	    (char_u*)"", // not using fuzzy as currently EXPAND_STRING_SETTING doesn't use it
	    args->oe_xp,
	    args->oe_regmatch,
	    matches,
	    ଅଙ୍କ_Matches,
	    expand_set_opt_generic_cb,
	    FALSE);

    set_opt_callback_orig_option = ଖାଲି;
    set_opt_callback_func = ଖାଲି;
    ଫେରନ୍ତୁ ret;
}

#ଯଦି defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
ସ୍ଥିର garray_T *expand_cb_ga;
ସ୍ଥିର optexpand_T *expand_cb_args;

/*
 * Callback provided to a function in expand_set_opt_callback. Will perform
 * regex matching against the value and add to the list.
 *
 * Returns OK usually. Returns FAIL if it failed to allocate memory, and the
 * caller should terminate the enumeration.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_set_opt_callback_cb(char_u *val)
{
    regmatch_T	*regmatch = expand_cb_args->oe_regmatch;
    expand_T	*xp = expand_cb_args->oe_xp;
    garray_T	*ga = expand_cb_ga;
    char_u	*str;

    ଯଦି (val == ଖାଲି || *val == NUL)
	ଫେରନ୍ତୁ OK;

    ଯଦି (xp->xp_pattern[0] != NUL &&
	    !vim_regexec(regmatch, val, (colnr_T)0))
	ଫେରନ୍ତୁ OK;

    str = vim_strsave_escaped(val, (char_u *)" \t\\");

    ଯଦି (str == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    ଯଦି (ga_grow(ga, 1) == FAIL)
    {
	vim_free(str);
	ଫେରନ୍ତୁ FAIL;
    }

    ((char_u **)ga->ga_data)[ga->ga_len] = str;
    ++ga->ga_len;
    ଫେରନ୍ତୁ OK;
}

/*
 * Expand an option with a provided function that takes a callback. The
 * function will enumerate through all options and call the callback to add it
 * to the list.
 *
 * "func" is the enumerator function that will generate the list of options.
 * "func_params" is a single parameter that will be passed to func.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_set_opt_callback(
	optexpand_T *args,
	ଶୂନ୍ୟ (*func)(optexpand_T *, ଶୂନ୍ୟ* params, ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*cb)(char_u *val)),
	ଶୂନ୍ୟ *func_params,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_Matches,
	char_u ***matches)
{
    garray_T	ga;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		include_orig_val = args->oe_include_orig_val;
    char_u	*option_val = args->oe_opt_ମୂଲ୍ୟ;

    ga_init2(&ga, ଆକାର(ଅକ୍ଷର *), 30);

    ଯଦି (include_orig_val && *option_val != NUL)
    {
	char_u *p = vim_strsave(option_val);
	ଯଦି (p == ଖାଲି)
	    ଫେରନ୍ତୁ FAIL;
	ଯଦି (ga_grow(&ga, 1) == FAIL)
	{
	    vim_free(p);
	    ଫେରନ୍ତୁ FAIL;
	}
	((char_u **)ga.ga_data)[ga.ga_len] = p;
	++ga.ga_len;
    }

    expand_cb_ga = &ga;
    expand_cb_args = args;

    func(args, func_params, expand_set_opt_callback_cb);

    expand_cb_ga = ଖାଲି;
    expand_cb_args = ଖାଲି;

    *matches = ga.ga_data;
    *ଅଙ୍କ_Matches = ga.ga_len;
    ଫେରନ୍ତୁ OK;
}
#endif

/*
 * Expand an option which is a list of flags.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_set_opt_listflag(
	optexpand_T *args,
	char_u *flags,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_Matches,
	char_u ***matches)
{
    char_u  *p;
    char_u  *option_val = args->oe_opt_ମୂଲ୍ୟ;
    char_u  *cmdline_val = args->oe_set_arg;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    append = args->oe_append;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    include_orig_val = args->oe_include_orig_val && (*option_val != NUL);

    size_t ଅଙ୍କ_flags = STRLEN(flags);

    // Assume we only have small number of flags, so just allocate max size.
    *matches = ALLOC_MULT(char_u *, ଅଙ୍କ_flags + 1);
    ଯଦି (*matches == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣନା_2 = 0;

    ଯଦି (include_orig_val)
    {
	p = vim_strsave(option_val);
	ଯଦି (p == ଖାଲି)
	{
	    VIM_CLEAR(*matches);
	    ଫେରନ୍ତୁ FAIL;
	}
	(*matches)[ଗଣନା_2++] = p;
    }

    ପାଇଁ (char_u *flag = flags; *flag != NUL; flag++)
    {
	ଯଦି (append && vim_strchr(option_val, *flag) != ଖାଲି)
	    ଜାରି;

	ଯଦି (vim_strchr(cmdline_val, *flag) == ଖାଲି)
	{
	    ଯଦି (include_orig_val
		    && option_val[1] == NUL
		    && *flag == option_val[0])
	    {
		// This value is already used as the first choice as it's the
		// existing flag. Just skip it to avoid duplicate.
		ଜାରି;
	    }
	    p = vim_strnsave(flag, 1);
	    ଯଦି (p == ଖାଲି)
	    {
		ଯଦି (ଗଣନା_2 == 0)
		{
		    VIM_CLEAR(*matches);
		    ଫେରନ୍ତୁ FAIL;
		}
		ଅନ୍ୟଥା
		    ଭାଙ୍ଗ;
	    }
	    (*matches)[ଗଣନା_2++] = p;
	}
    }

    ଯଦି (ଗଣନା_2 == 0)
    {
	VIM_CLEAR(*matches);
	ଫେରନ୍ତୁ FAIL;
    }
    *ଅଙ୍କ_Matches = ଗଣନା_2;
    ଫେରନ୍ତୁ OK;
}

/*
 * The 'ambiwidth' option is changed.
 */
    ଅକ୍ଷର *
did_set_ambiwidth(optset_T *args UNUSED)
{
    ଯଦି (check_opt_strings(p_ambw, p_ambw_ମୂଲ୍ୟଗୁଡ଼ିକ, FALSE) != OK)
	ଫେରନ୍ତୁ e_invalid_argument;

    ଫେରନ୍ତୁ check_chars_options();
}

// "name" must be a string literal, the length is computed at compile time.
#define completing_ମୂଲ୍ୟ_for_subopt(args, name) \
	  completing_ମୂଲ୍ୟ_for_subopt_len(args, name, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN_LITERAL(name))

/*
 * Return true when completing the value of the sub-option "name" with length
 * "len", e.g. the value after "close:" in 'completepopup'.
 */
    ସ୍ଥିର bool
completing_ମୂଲ୍ୟ_for_subopt_len(optexpand_T *args, ଅକ୍ଷର *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len)
{
    char_u  *colon = args->oe_xp->xp_pattern - 1;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    off = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(colon - args->oe_set_arg);

    ଯଦି (off < len)
	ଫେରନ୍ତୁ false;
    // The name must follow a comma when it does not start the option value.
    ଯଦି (off > len && *(colon - len - 1) != ',')
	ଫେରନ୍ତୁ false;

    ଫେରନ୍ତୁ STRNCMP(colon - len, name, len) == 0;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_set_ambiwidth(optexpand_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_Matches, char_u ***matches)
{
    ଫେରନ୍ତୁ expand_set_opt_string(
	    args,
	    p_ambw_ମୂଲ୍ୟଗୁଡ଼ିକ,
	    ARRAY_LENGTH(p_ambw_ମୂଲ୍ୟଗୁଡ଼ିକ) - 1,
	    ଅଙ୍କ_Matches,
	    matches);
}

/*
 * The 'background' option is changed.
 */
    ଅକ୍ଷର *
did_set_background(optset_T *args)
{
    ଯଦି (check_opt_strings(p_bg, p_bg_ମୂଲ୍ୟଗୁଡ଼ିକ, FALSE) == FAIL)
	ଫେରନ୍ତୁ e_invalid_argument;

    ଯଦି (args->os_oldval.string != ଖାଲି && args->os_oldval.string[0] == *p_bg)
	// Value was not changed
	ଫେରନ୍ତୁ ଖାଲି;

#ifdef FEAT_EVAL
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା dark = (*p_bg == 'd');
#endif

    init_highlight(FALSE, FALSE);

#ifdef FEAT_EVAL
    ଯଦି (dark != (*p_bg == 'd')
	    && get_var_ମୂଲ୍ୟ((char_u *)"g:colors_name") != ଖାଲି)
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
#endif
#ifdef FEAT_TERMINAL
    term_update_colors_all();
#endif

    ଫେରନ୍ତୁ ଖାଲି;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_set_background(optexpand_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_Matches, char_u ***matches)
{
    ଫେରନ୍ତୁ expand_set_opt_string(
	    args,
	    p_bg_ମୂଲ୍ୟଗୁଡ଼ିକ,
	    ARRAY_LENGTH(p_bg_ମୂଲ୍ୟଗୁଡ଼ିକ) - 1,
	    ଅଙ୍କ_Matches,
	    matches);
}

/*
 * The 'backspace' option is changed.
 */
    ଅକ୍ଷର *
did_set_backspace(optset_T *args UNUSED)
{
    ଯଦି (VIM_ISDIGIT(*p_bs))
    {
	ଯଦି (*p_bs > '3' || p_bs[1] != NUL)
	    ଫେରନ୍ତୁ e_invalid_argument;
    }
    ଅନ୍ୟଥା ଯଦି (check_opt_strings(p_bs, p_bs_ମୂଲ୍ୟଗୁଡ଼ିକ, TRUE) != OK)
	ଫେରନ୍ତୁ e_invalid_argument;

    ଫେରନ୍ତୁ ଖାଲି;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_set_backspace(optexpand_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_Matches, char_u ***matches)
{
    ଫେରନ୍ତୁ expand_set_opt_string(
	    args,
	    p_bs_ମୂଲ୍ୟଗୁଡ଼ିକ,
	    ARRAY_LENGTH(p_bs_ମୂଲ୍ୟଗୁଡ଼ିକ) - 1,
	    ଅଙ୍କ_Matches,
	    matches);
}

/*
 * The 'backupcopy' option is changed.
 */
    ଅକ୍ଷର *
did_set_backupcopy(optset_T *args)
{
    char_u		*bkc = p_bkc;
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା	*flags = &bkc_flags;
    ଅକ୍ଷର		*errmsg = ଖାଲି;

    ଯଦି (args->os_flags & OPT_LOCAL)
    {
	bkc = curbuf->b_p_bkc;
	flags = &curbuf->b_bkc_flags;
    }
    ଅନ୍ୟଥା ଯଦି (!(args->os_flags & OPT_GLOBAL))
	// When using :set, clear the local flags.
	curbuf->b_bkc_flags = 0;

    ଯଦି ((args->os_flags & OPT_LOCAL) && *bkc == NUL)
	// make the local value empty: use the global value
	*flags = 0;
    ଅନ୍ୟଥା
    {
	ଯଦି (opt_strings_flags(bkc, p_bkc_ମୂଲ୍ୟଗୁଡ଼ିକ, flags, TRUE) != OK)
	    errmsg = e_invalid_argument;
	ଯଦି ((((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)*flags & BKC_AUTO) != 0)
		+ (((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)*flags & BKC_YES) != 0)
		+ (((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)*flags & BKC_NO) != 0) != 1)
	{
	    // Must have exactly one of "auto", "yes"  and "no".
	    (ଶୂନ୍ୟ)opt_strings_flags(args->os_oldval.string, p_bkc_ମୂଲ୍ୟଗୁଡ଼ିକ,
								  flags, TRUE);
	    errmsg = e_invalid_argument;
	}
    }

    ଫେରନ୍ତୁ errmsg;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_set_backupcopy(optexpand_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_Matches, char_u ***matches)
{
    ଫେରନ୍ତୁ expand_set_opt_string(
	    args,
	    p_bkc_ମୂଲ୍ୟଗୁଡ଼ିକ,
	    ARRAY_LENGTH(p_bkc_ମୂଲ୍ୟଗୁଡ଼ିକ) - 1,
	    ଅଙ୍କ_Matches,
	    matches);
}

/*
 * The 'backupext' or the 'patchmode' option is changed.
 */
    ଅକ୍ଷର *
did_set_backupext_or_patchmode(optset_T *args UNUSED)
{
    ଯଦି (STRCMP(*p_bex == '.' ? p_bex + 1 : p_bex,
		*p_pm == '.' ? p_pm + 1 : p_pm) == 0)
	ଫେରନ୍ତୁ e_backupext_and_patchmode_are_equal;

    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * The 'belloff' option is changed.
 */
    ଅକ୍ଷର *
did_set_belloff(optset_T *args UNUSED)
{
    ଫେରନ୍ତୁ did_set_opt_flags(p_bo, p_bo_ମୂଲ୍ୟଗୁଡ଼ିକ, &bo_flags, TRUE);
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_set_belloff(optexpand_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_Matches, char_u ***matches)
{
    ଫେରନ୍ତୁ expand_set_opt_string(
	    args,
	    p_bo_ମୂଲ୍ୟଗୁଡ଼ିକ,
	    ARRAY_LENGTH(p_bo_ମୂଲ୍ୟଗୁଡ଼ିକ) - 1,
	    ଅଙ୍କ_Matches,
	    matches);
}

#ଯଦି defined(FEAT_LINEBREAK)
/*
 * The 'breakat' option is changed.
 */
    ଅକ୍ଷର *
did_set_breakat(optset_T *args UNUSED)
{
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    ପାଇଁ (i = 0; i < 256; i++)
	breakat_flags[i] = FALSE;

    ଯଦି (p_breakat != ଖାଲି)
	ପାଇଁ (p = p_breakat; *p; p++)
	    breakat_flags[*p] = TRUE;

    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * The 'breakindentopt' option is changed.
 */
    ଅକ୍ଷର *
did_set_breakindentopt(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    ଯଦି (briopt_check(*varp, varp == &curwin->w_p_briopt ? curwin : ଖାଲି)
								      == FAIL)
	ଫେରନ୍ତୁ e_invalid_argument;

    // list setting requires a redraw
    ଯଦି (varp == &curwin->w_p_briopt && curwin->w_briopt_list)
	redraw_all_later(UPD_NOT_VALID);

    ଫେରନ୍ତୁ ଖାଲି;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_set_breakindentopt(
    optexpand_T		*args,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			*ଅଙ୍କ_Matches,
    char_u		***matches)
{
    ଫେରନ୍ତୁ expand_set_opt_string(
	    args,
	    p_briopt_ମୂଲ୍ୟଗୁଡ଼ିକ,
	    ARRAY_LENGTH(p_briopt_ମୂଲ୍ୟଗୁଡ଼ିକ) - 1,
	    ଅଙ୍କ_Matches,
	    matches);
}
#endif

#ଯଦି defined(FEAT_BROWSE)
/*
 * The 'browsedir' option is changed.
 */
    ଅକ୍ଷର *
did_set_browsedir(optset_T *args UNUSED)
{
    ଯଦି (check_opt_strings(p_bsdir, p_bsdir_ମୂଲ୍ୟଗୁଡ଼ିକ, FALSE) != OK
	    && !mch_isdir(p_bsdir))
	ଫେରନ୍ତୁ e_invalid_argument;

    ଫେରନ୍ତୁ ଖାଲି;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_set_browsedir(optexpand_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_Matches, char_u ***matches)
{
    ଫେରନ୍ତୁ expand_set_opt_string(
	    args,
	    p_bsdir_ମୂଲ୍ୟଗୁଡ଼ିକ,
	    ARRAY_LENGTH(p_bsdir_ମୂଲ୍ୟଗୁଡ଼ିକ) - 1,
	    ଅଙ୍କ_Matches,
	    matches);
}
#endif

/*
 * The 'bufhidden' option is changed.
 */
    ଅକ୍ଷର *
did_set_bufhidden(optset_T *args UNUSED)
{
    ଫେରନ୍ତୁ did_set_opt_strings(curbuf->b_p_bh, p_bufhidden_ମୂଲ୍ୟଗୁଡ଼ିକ, FALSE);
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_set_bufhidden(optexpand_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_Matches, char_u ***matches)
{
    ଫେରନ୍ତୁ expand_set_opt_string(
	    args,
	    p_bufhidden_ମୂଲ୍ୟଗୁଡ଼ିକ,
	    ARRAY_LENGTH(p_bufhidden_ମୂଲ୍ୟଗୁଡ଼ିକ) - 1,
	    ଅଙ୍କ_Matches,
	    matches);
}

/*
 * The 'buftype' option is changed.
 */
    ଅକ୍ଷର *
did_set_buftype(optset_T *args UNUSED)
{
    ଯଦି (check_opt_strings(curbuf->b_p_bt, p_buftype_ମୂଲ୍ୟଗୁଡ଼ିକ, FALSE) != OK)
	ଫେରନ୍ତୁ e_invalid_argument;

    ଯଦି (curwin->w_status_height)
    {
	curwin->w_redr_status = true;
	redraw_later(UPD_VALID);
    }
    curbuf->b_help = (curbuf->b_p_bt[0] == 'h');
    redraw_titles();

    ଫେରନ୍ତୁ ଖାଲି;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_set_buftype(optexpand_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_Matches, char_u ***matches)
{
    ଫେରନ୍ତୁ expand_set_opt_string(
	    args,
	    p_buftype_ମୂଲ୍ୟଗୁଡ଼ିକ,
	    ARRAY_LENGTH(p_buftype_ମୂଲ୍ୟଗୁଡ଼ିକ) - 1,
	    ଅଙ୍କ_Matches,
	    matches);
}

/*
 * The 'casemap' option is changed.
 */
    ଅକ୍ଷର *
did_set_casemap(optset_T *args UNUSED)
{
    ଫେରନ୍ତୁ did_set_opt_flags(p_cmp, p_cmp_ମୂଲ୍ୟଗୁଡ଼ିକ, &cmp_flags, TRUE);
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_set_casemap(optexpand_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_Matches, char_u ***matches)
{
    ଫେରନ୍ତୁ expand_set_opt_string(
	    args,
	    p_cmp_ମୂଲ୍ୟଗୁଡ଼ିକ,
	    ARRAY_LENGTH(p_cmp_ମୂଲ୍ୟଗୁଡ଼ିକ) - 1,
	    ଅଙ୍କ_Matches,
	    matches);
}

#ଯଦି defined(HAVE_CLIPMETHOD)
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_set_clipboard(optexpand_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_Matches, char_u ***matches)
{
    ଫେରନ୍ତୁ expand_set_opt_string(
	    args,
	    p_cb_ମୂଲ୍ୟଗୁଡ଼ିକ,
	    ARRAY_LENGTH(p_cb_ମୂଲ୍ୟଗୁଡ଼ିକ) - 1,
	    ଅଙ୍କ_Matches,
	    matches);
}
#endif

#ifdef HAVE_CLIPMETHOD
    ଅକ୍ଷର *
did_set_clipmethod(optset_T *args UNUSED)
{
    ଫେରନ୍ତୁ choose_clipmethod();
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_set_clipmethod(optexpand_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_Matches, char_u ***matches)
{
    // We want to expand using the predefined clipmethod values + clipboard
    // provider names.
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		result;
    ଅକ୍ଷର	**ମୂଲ୍ୟଗୁଡ଼ିକ;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଗଣନା_2, pos = 0, start = 0;
# ifdef FEAT_EVAL
    dict_T	*providers = get_vim_var_dict(VV_CLIPPROVIDERS);
# ଅନ୍ୟଥା
    dict_T	*providers = ଖାଲି;
# endif
    hashtab_T	*ht = providers == ଖାଲି ? ଖାଲି : &providers->dv_hashtab;

    ଗଣନା_2 = (ht == ଖାଲି ? 0 : ht->ht_used);
# ifdef FEAT_WAYLAND_CLIPBOARD
    ଗଣନା_2++;
    start++;
# endif
# ifdef FEAT_XCLIPBOARD
    ଗଣନା_2++;
    start++;
# endif
    ମୂଲ୍ୟଗୁଡ଼ିକ = ALLOC_MULT(ଅକ୍ଷର *, ଗଣନା_2 + 1); // Add NULL terminator too

    ଯଦି (ମୂଲ୍ୟଗୁଡ଼ିକ == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

# ifdef FEAT_WAYLAND_CLIPBOARD
    ମୂଲ୍ୟଗୁଡ଼ିକ[pos++] = "wayland";
# endif
# ifdef FEAT_XCLIPBOARD
    ମୂଲ୍ୟଗୁଡ଼ିକ[pos++] = "x11";
# endif

    ଯଦି (ht != ଖାଲି)
	ପାଇଁ (long_u i = 0; i < ht->ht_mask + 1; i++)
	{
	    hashitem_T	*hi = ht->ht_array + i;

	    ଯଦି (!HASHITEM_EMPTY(hi))
		ମୂଲ୍ୟଗୁଡ଼ିକ[pos++] = (ଅକ୍ଷର *)vim_strsave(hi->hi_ଚାବି);
	}
    ମୂଲ୍ୟଗୁଡ଼ିକ[pos++] = ଖାଲି;

    result = expand_set_opt_string(
	    args,
	    ମୂଲ୍ୟଗୁଡ଼ିକ,
	    ଗଣନା_2,
	    ଅଙ୍କ_Matches,
	    matches);

    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = start; i < ଗଣନା_2; i++)
	vim_free(ମୂଲ୍ୟଗୁଡ଼ିକ[i]);
    vim_free(ମୂଲ୍ୟଗୁଡ଼ିକ);

    ଫେରନ୍ତୁ result;
}
#endif

/*
 * The global 'listchars' or 'fillchars' option is changed.
 */
    ସ୍ଥିର ଅକ୍ଷର *
did_set_global_listfillchars(char_u *val, ପୂର୍ଣ୍ଣସଂଖ୍ୟା opt_lcs, ପୂର୍ଣ୍ଣସଂଖ୍ୟା opt_flags,
						ଅକ୍ଷର *errbuf, size_t errbuflen)
{
    ଅକ୍ଷର	*errmsg = ଖାଲି;
    char_u	**local_ptr = opt_lcs ? &curwin->w_p_lcs : &curwin->w_p_fcs;

    // only apply the global value to "curwin" when it does not have a
    // local value
    ଯଦି (opt_lcs)
	errmsg = set_listchars_option(curwin, val,
		**local_ptr == NUL || !(opt_flags & OPT_GLOBAL),
							    errbuf, errbuflen);
    ଅନ୍ୟଥା
	errmsg = set_fillchars_option(curwin, val,
		**local_ptr == NUL || !(opt_flags & OPT_GLOBAL),
							    errbuf, errbuflen);
    ଯଦି (errmsg != ଖାଲି)
	ଫେରନ୍ତୁ errmsg;

    tabpage_T	*tp;
    win_T	*wp;

    // If the current window is set to use the global
    // 'listchars'/'fillchars' value, clear the window-local value.
    ଯଦି (!(opt_flags & OPT_GLOBAL))
	clear_string_option(local_ptr);
    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	// If the current window has a local value need to apply it
	// again, it was changed when setting the global value.
	// If no error was returned above, we don't expect an error
	// here, so ignore the return value.
	ଯଦି (opt_lcs)
	{
	    ଯଦି (*wp->w_p_lcs == NUL)
		(ଶୂନ୍ୟ)set_listchars_option(wp, wp->w_p_lcs, TRUE, ଖାଲି, 0);
	}
	ଅନ୍ୟଥା
	{
	    ଯଦି (*wp->w_p_fcs == NUL)
		(ଶୂନ୍ୟ)set_fillchars_option(wp, wp->w_p_fcs, TRUE, ଖାଲି, 0);
	}
    }

    redraw_all_later(UPD_NOT_VALID);

    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * The 'fillchars' option or the 'listchars' option is changed.
 */
    ଅକ୍ଷର *
did_set_chars_option(optset_T *args)
{
    char_u **varp = (char_u **)args->os_varp;
    ଅକ୍ଷର *errmsg = ଖାଲି;

    ଯଦି (   varp == &p_lcs		// global 'listchars'
	|| varp == &p_fcs)		// global 'fillchars'
	errmsg = did_set_global_listfillchars(*varp, varp == &p_lcs,
			  args->os_flags, args->os_errbuf, args->os_errbuflen);
    ଅନ୍ୟଥା ଯଦି (varp == &curwin->w_p_lcs)	// local 'listchars'
	errmsg = set_listchars_option(curwin, *varp, TRUE,
					  args->os_errbuf, args->os_errbuflen);
    ଅନ୍ୟଥା ଯଦି (varp == &curwin->w_p_fcs)	// local 'fillchars'
	errmsg = set_fillchars_option(curwin, *varp, TRUE,
					  args->os_errbuf, args->os_errbuflen);

    ଫେରନ୍ତୁ errmsg;
}

/*
 * Expand 'fillchars' or 'listchars' option value.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_set_chars_option(optexpand_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_Matches, char_u ***matches)
{
    char_u **varp = (char_u **)args->oe_varp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_lcs = (varp == &p_lcs || varp == &curwin->w_p_lcs);
    ଫେରନ୍ତୁ expand_set_opt_generic(
	    args,
	    is_lcs ? get_listchars_name : get_fillchars_name,
	    ଅଙ୍କ_Matches,
	    matches);
}

/*
 * The 'cinoptions' option is changed.
 */
    ଅକ୍ଷର *
did_set_cinoptions(optset_T *args UNUSED)
{
    // TODO: recognize errors
    parse_cino(curbuf);

    ଫେରନ୍ତୁ ଖାଲି;
}

#ଯଦି defined(FEAT_SYN_HL)
/*
 * The 'colorcolumn' option is changed.
 */
    ଅକ୍ଷର *
did_set_colorcolumn(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    ଫେରନ୍ତୁ check_colorcolumn(*varp, varp == &curwin->w_p_cc ? curwin : ଖାଲି);
}
#endif

/*
 * The 'comments' option is changed.
 */
    ଅକ୍ଷର *
did_set_comments(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u	*s;
    ଅକ୍ଷର	*errmsg = ଖାଲି;

    ପାଇଁ (s = *varp; *s; )
    {
	ଯେତେବେଳେ (*s && *s != ':')
	{
	    ଯଦି (vim_strchr((char_u *)COM_ALL, *s) == ଖାଲି
		    && !VIM_ISDIGIT(*s) && *s != '-')
	    {
		errmsg = illegal_char(args->os_errbuf, args->os_errbuflen, *s);
		ଭାଙ୍ଗ;
	    }
	    ++s;
	}
	ଯଦି (*s++ == NUL)
	    errmsg = e_missing_colon;
	ଅନ୍ୟଥା ଯଦି (*s == ',' || *s == NUL)
	    errmsg = e_zero_length_string;
	ଯଦି (errmsg != ଖାଲି)
	    ଭାଙ୍ଗ;
	ଯେତେବେଳେ (*s && *s != ',')
	{
	    ଯଦି (*s == '\\' && s[1] != NUL)
		++s;
	    ++s;
	}
	s = skip_to_option_part(s);
    }

    ଫେରନ୍ତୁ errmsg;
}

/*
 * The 'commentstring' option is changed.
 */
    ଅକ୍ଷର *
did_set_commentstring(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    ଯଦି (**varp != NUL && strstr((ଅକ୍ଷର *)*varp, "%s") == ଖାଲି)
	ଫେରନ୍ତୁ e_commentstring_must_be_empty_or_contain_str;

    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Check if value for 'complete' is valid when 'complete' option is changed.
 */
    ଅକ୍ଷର *
did_set_complete(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u	*p, *t;
    char_u	buffer[LSIZE];
    char_u	*buf_ptr;
    char_u	char_before = NUL;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		escape;

    ପାଇଁ (p = *varp; *p; )
    {
	vim_memset(buffer, 0, LSIZE);
	buf_ptr = buffer;
	escape = 0;

	// Extract substring while handling escaped commas
	ଯେତେବେଳେ (*p && (*p != ',' || escape) && buf_ptr < (buffer + LSIZE - 1))
	{
	    ଯଦି (*p == '\\' && *(p + 1) == ',')
	    {
		escape = 1;  // Mark escape mode
		p++;         // Skip '\'
	    }
	    ଅନ୍ୟଥା
	    {
		escape = 0;
		*buf_ptr++ = *p;
	    }
	    p++;
	}
	*buf_ptr = NUL;

	ଯଦି (vim_strchr((char_u *)".wbuksid]tUFo", *buffer) == ଖାଲି)
	    ଫେରନ୍ତୁ illegal_char(args->os_errbuf, args->os_errbuflen, *buffer);

	ଯଦି (vim_strchr((char_u *)"ksF", *buffer) == ଖାଲି && *(buffer + 1) != NUL
		&& *(buffer + 1) != '^')
	    char_before = *buffer;
	ଅନ୍ୟଥା
	{
	    // Test for a number after '^'
	    ଯଦି ((t = vim_strchr(buffer, '^')) != ଖାଲି)
	    {
		*t++ = NUL;
		ଯଦି (!*t)
		    char_before = '^';
		ଅନ୍ୟଥା
		{
		    ପାଇଁ (; *t; t++)
		    {
			ଯଦି (!vim_isdigit(*t))
			{
			    char_before = '^';
			    ଭାଙ୍ଗ;
			}
		    }
		}
	    }
	}
	ଯଦି (char_before != NUL)
	    ଫେରନ୍ତୁ illegal_char_after_chr(args->os_errbuf, args->os_errbuflen,
		    char_before);
	// Skip comma and spaces
	ଯେତେବେଳେ (*p == ',' || *p == ' ')
	    p++;
    }

#ifdef FEAT_COMPL_FUNC
    ଯଦି (set_cpt_callbacks(args) != OK)
	ଫେରନ୍ତୁ illegal_char_after_chr(args->os_errbuf, args->os_errbuflen,
		'F');
#endif
    ଫେରନ୍ତୁ ଖାଲି;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_set_complete(optexpand_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_Matches, char_u ***matches)
{
    ସ୍ଥିର ଅକ୍ଷର *(p_cpt_ମୂଲ୍ୟଗୁଡ଼ିକ[]) = {
	".", "w", "b", "u", "k", "kspell", "s", "i", "d", "]", "t", "U", "F", "o",
	ଖାଲି};
    ଫେରନ୍ତୁ expand_set_opt_string(
	    args,
	    p_cpt_ମୂଲ୍ୟଗୁଡ଼ିକ,
	    ARRAY_LENGTH(p_cpt_ମୂଲ୍ୟଗୁଡ଼ିକ) - 1,
	    ଅଙ୍କ_Matches,
	    matches);
}

/*
 * The 'completeopt' option is changed.
 */
    ଅକ୍ଷର *
did_set_completeopt(optset_T *args UNUSED)
{
    char_u	*cot = p_cot;
    unsigned	*flags = &cot_flags;

    ଯଦି (args->os_flags & OPT_LOCAL)
    {
	cot = curbuf->b_p_cot;
	flags = &curbuf->b_cot_flags;
    }
    ଅନ୍ୟଥା ଯଦି (!(args->os_flags & OPT_GLOBAL))
	// When using :set, clear the local flags.
	curbuf->b_cot_flags = 0;

    ଯଦି (opt_strings_flags(cot, p_cot_ମୂଲ୍ୟଗୁଡ଼ିକ, flags, TRUE) != OK)
	ଫେରନ୍ତୁ e_invalid_argument;

    ଫେରନ୍ତୁ ଖାଲି;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_set_completeopt(optexpand_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_Matches, char_u ***matches)
{
    ଫେରନ୍ତୁ expand_set_opt_string(
	    args,
	    p_cot_ମୂଲ୍ୟଗୁଡ଼ିକ,
	    ARRAY_LENGTH(p_cot_ମୂଲ୍ୟଗୁଡ଼ିକ) - 1,
	    ଅଙ୍କ_Matches,
	    matches);
}

/*
 * The 'completefuzzycollect' option is changed.
 */
    ଅକ୍ଷର *
did_set_completefuzzycollect(optset_T *args UNUSED)
{
    ଯଦି (opt_strings_flags(p_cfc, p_cfc_ମୂଲ୍ୟଗୁଡ଼ିକ, &cfc_flags, TRUE) != OK)
	ଫେରନ୍ତୁ e_invalid_argument;
    ଫେରନ୍ତୁ ଖାଲି;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_set_completefuzzycollect(
	optexpand_T *args,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_Matches,
	char_u ***matches)
{
    ଫେରନ୍ତୁ expand_set_opt_string(
	    args,
	    p_cfc_ମୂଲ୍ୟଗୁଡ଼ିକ,
	    ARRAY_LENGTH(p_cfc_ମୂଲ୍ୟଗୁଡ଼ିକ) - 1,
	    ଅଙ୍କ_Matches,
	    matches);
}

/*
 * The 'completeitemalign' option is changed.
 */
    ଅକ୍ଷର *
did_set_completeitemalign(optset_T *args UNUSED)
{
    char_u	*p = p_cia;
    unsigned	new_cia_flags = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		seen[3] = { FALSE, FALSE, FALSE };
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଗଣନା_2 = 0;
    char_u	buf[10];

    ଯେତେବେଳେ (*p)
    {
	copy_option_part(&p, buf, ଆକାର(buf), ",");
	ଯଦି (ଗଣନା_2 >= 3)
	    ଫେରନ୍ତୁ e_invalid_argument;

	ଯଦି (STRCMP(buf, "abbr") == 0)
	{
	    ଯଦି (seen[CPT_ABBR])
		ଫେରନ୍ତୁ e_invalid_argument;
	    new_cia_flags = new_cia_flags * 10 + CPT_ABBR;
	    seen[CPT_ABBR] = TRUE;
	    ଗଣନା_2++;
	}
	ଅନ୍ୟଥା ଯଦି (STRCMP(buf, "kind") == 0)
	{
	    ଯଦି (seen[CPT_KIND])
		ଫେରନ୍ତୁ e_invalid_argument;
	    new_cia_flags = new_cia_flags * 10 + CPT_KIND;
	    seen[CPT_KIND] = TRUE;
	    ଗଣନା_2++;
	}
	ଅନ୍ୟଥା ଯଦି (STRCMP(buf, "menu") == 0)
	{
	    ଯଦି (seen[CPT_MENU])
		ଫେରନ୍ତୁ e_invalid_argument;
	    new_cia_flags = new_cia_flags * 10 + CPT_MENU;
	    seen[CPT_MENU] = TRUE;
	    ଗଣନା_2++;
	}
	ଅନ୍ୟଥା
	    ଫେରନ୍ତୁ e_invalid_argument;
    }
    ଯଦି (new_cia_flags == 0 || ଗଣନା_2 != 3)
	ଫେରନ୍ତୁ e_invalid_argument;

    cia_flags = new_cia_flags;
    ଫେରନ୍ତୁ ଖାଲି;
}

#ଯଦି defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
/*
 * The 'completepopup' option is changed.
 */
    ଅକ୍ଷର *
did_set_completepopup(optset_T *args UNUSED)
{
    ଯଦି (parse_completepopup(ଖାଲି) == FAIL)
	ଫେରନ୍ତୁ e_invalid_argument;

    popup_close_info();
    ଫେରନ୍ତୁ ଖାଲି;
}
#endif

#ଯଦି defined(BACKSLASH_IN_FILENAME)
/*
 * The 'completeslash' option is changed.
 */
    ଅକ୍ଷର *
did_set_completeslash(optset_T *args UNUSED)
{
    ଯଦି (check_opt_strings(p_csl, p_csl_ମୂଲ୍ୟଗୁଡ଼ିକ, FALSE) != OK
	    || check_opt_strings(curbuf->b_p_csl, p_csl_ମୂଲ୍ୟଗୁଡ଼ିକ, FALSE) != OK)
	ଫେରନ୍ତୁ e_invalid_argument;

    ଫେରନ୍ତୁ ଖାଲି;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_set_completeslash(optexpand_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_Matches, char_u ***matches)
{
    ଫେରନ୍ତୁ expand_set_opt_string(
	    args,
	    p_csl_ମୂଲ୍ୟଗୁଡ଼ିକ,
	    ARRAY_LENGTH(p_csl_ମୂଲ୍ୟଗୁଡ଼ିକ) - 1,
	    ଅଙ୍କ_Matches,
	    matches);
}
#endif

#ଯଦି defined(FEAT_CONCEAL)
/*
 * The 'concealcursor' option is changed.
 */
    ଅକ୍ଷର *
did_set_concealcursor(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    ଫେରନ୍ତୁ did_set_option_listflag(*varp, (char_u *)COCU_ALL, args->os_errbuf,
		    args->os_errbuflen);
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_set_concealcursor(optexpand_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_Matches, char_u ***matches)
{
    ଫେରନ୍ତୁ expand_set_opt_listflag(args, (char_u*)COCU_ALL, ଅଙ୍କ_Matches, matches);
}
#endif

/*
 * The 'cpoptions' option is changed.
 */
    ଅକ୍ଷର *
did_set_cpoptions(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    ଫେରନ୍ତୁ did_set_option_listflag(*varp, (char_u *)CPO_ALL, args->os_errbuf,
		    args->os_errbuflen);
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_set_cpoptions(optexpand_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_Matches, char_u ***matches)
{
    ଫେରନ୍ତୁ expand_set_opt_listflag(args, (char_u*)CPO_ALL, ଅଙ୍କ_Matches, matches);
}

#ଯଦି defined(FEAT_CRYPT)
/*
 * The 'cryptkey' option is changed.
 */
    ଅକ୍ଷର *
did_set_cryptkey(optset_T *args)
{
    // Make sure the ":set" command doesn't show the new value in the
    // history.
    remove_ଚାବି_from_history();

    ଯଦି (args->os_op != OP_NONE)
	// Don't allow set+=/-=/^= as they can allow for substring guessing
	ଫେରନ୍ତୁ e_invalid_argument;

    ଯଦି (STRCMP(curbuf->b_p_ଚାବି, args->os_oldval.string) != 0)
    {
	// Need to update the swapfile.
	ml_set_crypt_ଚାବି(curbuf, args->os_oldval.string,
		*curbuf->b_p_cm == NUL ? p_cm : curbuf->b_p_cm);
	changed_internal();
    }
# ifdef FEAT_SODIUM
    ଯଦି (crypt_method_is_sodium(crypt_get_method_nr(curbuf)))
	crypt_sodium_lock_ଚାବି(args->os_newval.string);
# endif

    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * The 'cryptmethod' option is changed.
 */
    ଅକ୍ଷର *
did_set_cryptmethod(optset_T *args)
{
    char_u  *p;
    char_u  *s;

    ଯଦି (args->os_flags & OPT_LOCAL)
	p = curbuf->b_p_cm;
    ଅନ୍ୟଥା
	p = p_cm;
    ଯଦି (check_opt_strings(p, p_cm_ମୂଲ୍ୟଗୁଡ଼ିକ, TRUE) != OK)
	ଫେରନ୍ତୁ e_invalid_argument;
    ଅନ୍ୟଥା ଯଦି (crypt_self_test() == FAIL)
	ଫେରନ୍ତୁ e_invalid_argument;

    // When setting the global value to empty, make it "zip".
    ଯଦି (*p_cm == NUL)
    {
	free_string_option(p_cm);
	p_cm = vim_strsave((char_u *)"zip");
    }
    // When using ":set cm=name" the local value is going to be empty.
    // Do that here, otherwise the crypt functions will still use the
    // local value.
    ଯଦି ((args->os_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	free_string_option(curbuf->b_p_cm);
	curbuf->b_p_cm = empty_option;
    }

    // Need to update the swapfile when the effective method changed.
    // Set "s" to the effective old value, "p" to the effective new
    // method and compare.
    ଯଦି ((args->os_flags & OPT_LOCAL) && *args->os_oldval.string == NUL)
	s = p_cm;  // was previously using the global value
    ଅନ୍ୟଥା
	s = args->os_oldval.string;
    ଯଦି (*curbuf->b_p_cm == NUL)
	p = p_cm;  // is now using the global value
    ଅନ୍ୟଥା
	p = curbuf->b_p_cm;
    ଯଦି (STRCMP(s, p) != 0)
	ml_set_crypt_ଚାବି(curbuf, curbuf->b_p_ଚାବି, s);

    // If the global value changes need to update the swapfile for all
    // buffers using that value.
    ଯଦି ((args->os_flags & OPT_GLOBAL)
	    && STRCMP(p_cm, args->os_oldval.string) != 0)
    {
	buf_T	*buf;

	FOR_ALL_BUFFERS(buf)
	    ଯଦି (buf != curbuf && *buf->b_p_cm == NUL)
		ml_set_crypt_ଚାବି(buf, buf->b_p_ଚାବି, args->os_oldval.string);
    }
    ଫେରନ୍ତୁ ଖାଲି;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_set_cryptmethod(optexpand_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_Matches, char_u ***matches)
{
    ଫେରନ୍ତୁ expand_set_opt_string(
	    args,
	    p_cm_ମୂଲ୍ୟଗୁଡ଼ିକ,
	    ARRAY_LENGTH(p_cm_ମୂଲ୍ୟଗୁଡ଼ିକ) - 1,
	    ଅଙ୍କ_Matches,
	    matches);
}
#endif

#ଯଦି defined(FEAT_CSCOPE) && defined(FEAT_QUICKFIX)
/*
 * The 'cscopequickfix' option is changed.
 */
    ଅକ୍ଷର *
did_set_cscopequickfix(optset_T *args UNUSED)
{
    char_u	*p;

    ଯଦି (p_csqf == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    p = p_csqf;
    ଯେତେବେଳେ (*p != NUL)
    {
	ଯଦି (vim_strchr((char_u *)CSQF_CMDS, *p) == ଖାଲି
		|| p[1] == NUL
		|| vim_strchr((char_u *)CSQF_FLAGS, p[1]) == ଖାଲି
		|| (p[2] != NUL && p[2] != ','))
	    ଫେରନ୍ତୁ e_invalid_argument;
	ଅନ୍ୟଥା ଯଦି (p[2] == NUL)
	    ଭାଙ୍ଗ;
	ଅନ୍ୟଥା
	    p += 3;
    }

    ଫେରନ୍ତୁ ଖାଲି;
}
#endif

#ଯଦି defined(FEAT_SYN_HL)
/*
 * The 'cursorlineopt' option is changed.
 */
    ଅକ୍ଷର *
did_set_cursorlineopt(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    // This could be changed to use opt_strings_flags() instead.
    ଯଦି (**varp == NUL || fill_culopt_flags(*varp, curwin) != OK)
	ଫେରନ୍ତୁ e_invalid_argument;

    ଫେରନ୍ତୁ ଖାଲି;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_set_cursorlineopt(optexpand_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_Matches, char_u ***matches)
{
    ଫେରନ୍ତୁ expand_set_opt_string(
	    args,
	    p_culopt_ମୂଲ୍ୟଗୁଡ଼ିକ,
	    ARRAY_LENGTH(p_culopt_ମୂଲ୍ୟଗୁଡ଼ିକ) - 1,
	    ଅଙ୍କ_Matches,
	    matches);
}
#endif

/*
 * The 'debug' option is changed.
 */
    ଅକ୍ଷର *
did_set_debug(optset_T *args UNUSED)
{
    ଫେରନ୍ତୁ did_set_opt_strings(p_debug, p_debug_ମୂଲ୍ୟଗୁଡ଼ିକ, TRUE);
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_set_debug(optexpand_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_Matches, char_u ***matches)
{
    ଫେରନ୍ତୁ expand_set_opt_string(
	    args,
	    p_debug_ମୂଲ୍ୟଗୁଡ଼ିକ,
	    ARRAY_LENGTH(p_debug_ମୂଲ୍ୟଗୁଡ଼ିକ) - 1,
	    ଅଙ୍କ_Matches,
	    matches);
}

#ଯଦି defined(FEAT_DIFF)
/*
 * The 'diffanchors' option is changed.
 */
    ଅକ୍ଷର *
did_set_diffanchors(optset_T *args)
{
    ଯଦି (diffanchors_changed(args->os_flags & OPT_LOCAL) == FAIL)
	ଫେରନ୍ତୁ e_invalid_argument;

    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * The 'diffopt' option is changed.
 */
    ଅକ୍ଷର *
did_set_diffopt(optset_T *args UNUSED)
{
    ଯଦି (diffopt_changed() == FAIL)
	ଫେରନ୍ତୁ e_invalid_argument;

    ଫେରନ୍ତୁ ଖାଲି;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_set_diffopt(optexpand_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_Matches, char_u ***matches)
{
    expand_T *xp = args->oe_xp;

    ଯଦି (xp->xp_pattern > args->oe_set_arg && *(xp->xp_pattern-1) == ':')
    {
	ଯଦି (completing_ମୂଲ୍ୟ_for_subopt(args, "algorithm"))
	{
	    ଫେରନ୍ତୁ expand_set_opt_string(
		    args,
		    p_dip_algorithm_ମୂଲ୍ୟଗୁଡ଼ିକ,
		    ARRAY_LENGTH(p_dip_algorithm_ମୂଲ୍ୟଗୁଡ଼ିକ) - 1,
		    ଅଙ୍କ_Matches,
		    matches);
	}
	ଯଦି (completing_ମୂଲ୍ୟ_for_subopt(args, "inline"))
	{
	    ଫେରନ୍ତୁ expand_set_opt_string(
		    args,
		    p_dip_inline_ମୂଲ୍ୟଗୁଡ଼ିକ,
		    ARRAY_LENGTH(p_dip_inline_ମୂଲ୍ୟଗୁଡ଼ିକ) - 1,
		    ଅଙ୍କ_Matches,
		    matches);
	}
	ଫେରନ୍ତୁ FAIL;
    }

    ଫେରନ୍ତୁ expand_set_opt_string(
	    args,
	    p_dip_ମୂଲ୍ୟଗୁଡ଼ିକ,
	    ARRAY_LENGTH(p_dip_ମୂଲ୍ୟଗୁଡ଼ିକ) - 1,
	    ଅଙ୍କ_Matches,
	    matches);
}
#endif

/*
 * The 'display' option is changed.
 */
    ଅକ୍ଷର *
did_set_display(optset_T *args UNUSED)
{
    ଯଦି (opt_strings_flags(p_dy, p_dy_ମୂଲ୍ୟଗୁଡ଼ିକ, &dy_flags, TRUE) != OK)
	ଫେରନ୍ତୁ e_invalid_argument;

    (ଶୂନ୍ୟ)init_chartab();
    ଫେରନ୍ତୁ ଖାଲି;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_set_display(optexpand_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_Matches, char_u ***matches)
{
    ଫେରନ୍ତୁ expand_set_opt_string(
	    args,
	    p_dy_ମୂଲ୍ୟଗୁଡ଼ିକ,
	    ARRAY_LENGTH(p_dy_ମୂଲ୍ୟଗୁଡ଼ିକ) - 1,
	    ଅଙ୍କ_Matches,
	    matches);
}

/*
 * The 'eadirection' option is changed.
 */
    ଅକ୍ଷର *
did_set_eadirection(optset_T *args UNUSED)
{
    ଫେରନ୍ତୁ did_set_opt_strings(p_ead, p_ead_ମୂଲ୍ୟଗୁଡ଼ିକ, FALSE);
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_set_eadirection(optexpand_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_Matches, char_u ***matches)
{
    ଫେରନ୍ତୁ expand_set_opt_string(
	    args,
	    p_ead_ମୂଲ୍ୟଗୁଡ଼ିକ,
	    ARRAY_LENGTH(p_ead_ମୂଲ୍ୟଗୁଡ଼ିକ) - 1,
	    ଅଙ୍କ_Matches,
	    matches);
}

/*
 * One of the 'encoding', 'fileencoding', 'termencoding' or 'makeencoding'
 * options is changed.
 */
    ଅକ୍ଷର *
did_set_encoding(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u	**gvarp;
    ଅକ୍ଷର	*errmsg = ଖାଲି;
    char_u	*p;

    // Get the global option to compare with, otherwise we would have to check
    // two values for all local options.
    gvarp = (char_u **)get_option_varp_scope(args->os_idx, OPT_GLOBAL);

    ଯଦି (gvarp == &p_fenc)
    {
	ଯଦି (!curbuf->b_p_ma && args->os_flags != OPT_GLOBAL)
	    errmsg = e_cannot_make_changes_modifiable_is_off;
	ଅନ୍ୟଥା ଯଦି (vim_strchr(*varp, ',') != ଖାଲି)
	    // No comma allowed in 'fileencoding'; catches confusing it
	    // with 'fileencodings'.
	    errmsg = e_invalid_argument;
	ଅନ୍ୟଥା
	{
	    // May show a "+" in the title now.
	    redraw_titles();
	    // Add 'fileencoding' to the swap file.
	    ml_setflags(curbuf);
	}
    }
    ଯଦି (errmsg == ଖାଲି)
    {
	// canonize the value, so that STRCMP() can be used on it
	p = enc_canonize(*varp);
	ଯଦି (p != ଖାଲି)
	{
	    vim_free(*varp);
	    *varp = p;
	}
	ଯଦି (varp == &p_enc)
	{
	    errmsg = mb_init();
	    redraw_titles();
	}
    }

#ଯଦି defined(FEAT_GUI_GTK)
    ଯଦି (errmsg == ଖାଲି && varp == &p_tenc && gui.in_use)
    {
	// GTK uses only a single encoding, and that is UTF-8.
	ଯଦି (STRCMP(p_tenc, "utf-8") != 0)
	    errmsg = e_cannot_be_changed_in_gtk_GUI;
    }
#endif

    ଯଦି (errmsg == ଖାଲି)
    {
#ifdef FEAT_KEYMAP
	// When 'keymap' is used and 'encoding' changes, reload the keymap
	// (with another encoding).
	ଯଦି (varp == &p_enc && *curbuf->b_p_keymap != NUL)
	    (ଶୂନ୍ୟ)keymap_init();
#endif

	// When 'termencoding' is not empty and 'encoding' changes or when
	// 'termencoding' changes, need to setup for keyboard input and
	// display output conversion.
	ଯଦି (((varp == &p_enc && *p_tenc != NUL) || varp == &p_tenc))
	{
	    ଯଦି (convert_setup(&ନିବେଶ_conv, p_tenc, p_enc) == FAIL
		    || convert_setup(&output_conv, p_enc, p_tenc) == FAIL)
	    {
		semsg(_(e_cannot_convert_between_str_and_str),
			p_tenc, p_enc);
		errmsg = e_invalid_argument;
	    }
	}

#ଯଦି defined(MSWIN)
	// $HOME, $VIM and $VIMRUNTIME may have characters in active code page.
	ଯଦି (varp == &p_enc)
	{
	    init_homedir();
	    init_vimdir();
	}
#endif
    }

    ଫେରନ୍ତୁ errmsg;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_set_encoding(optexpand_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_Matches, char_u ***matches)
{
    ଫେରନ୍ତୁ expand_set_opt_generic(
	    args,
	    get_encoding_name,
	    ଅଙ୍କ_Matches,
	    matches);
}

/*
 * The 'eventignore(win)' option is changed.
 */
    ଅକ୍ଷର *
did_set_eventignore(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u	*oldval = args->os_oldval.string;

    ଯଦି (check_ei(*varp) == FAIL)
	ଫେରନ୍ତୁ e_invalid_argument;

    ଯଦି (oldval == ଖାଲି || STRCMP(oldval, *varp) == 0)
	ଫେରନ୍ତୁ ଖାଲି;

    // Deal with the events that are triggered by comparing against a stored
    // state, with the old value in effect: what happened while an event was
    // ignored must not be reported once it is not ignored anymore, and what
    // happened before must still be reported.
    // Use a copy, the caller owns "oldval" and autocommands may free it.
    char_u	*save_ei = vim_strsave(oldval);
    ଯଦି (save_ei != ଖାଲି)
    {
	char_u	*newval = *varp;
	win_T	*wp = is_window_local_option(args->os_idx) ? curwin : ଖାଲି;

	*varp = save_ei;
	// "varp" points into "wp" for 'eventignorewin'.
	ଯଦି (wp != ଖାଲି)
	    ++wp->w_locked;
	may_trigger_deferred_events();
	ଯଦି (wp != ଖାଲି)
	    --wp->w_locked;

	free_string_option(*varp);
	*varp = newval;
    }

    ଫେରନ୍ତୁ ଖାଲି;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା expand_eiw = FALSE;

    ସ୍ଥିର char_u *
get_eventignore_name(expand_T *xp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା subtract = *xp->xp_pattern == '-';
    // 'eventignore(win)' allows special keyword "all" in addition to
    // all event names.
    ଯଦି (!subtract && idx == 0)
	ଫେରନ୍ତୁ (char_u *)"all";

    char_u *name = get_event_name_no_group(xp, idx - 1 + subtract, expand_eiw);
    ଯଦି (name == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    sprintf((ଅକ୍ଷର *)IObuff, "%s%s", subtract ? "-" : "", name);
    ଫେରନ୍ତୁ IObuff;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_set_eventignore(optexpand_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_Matches, char_u ***matches)
{
    expand_eiw = args->oe_varp != (char_u *)&p_ei;
    ଫେରନ୍ତୁ expand_set_opt_generic(
	    args,
	    get_eventignore_name,
	    ଅଙ୍କ_Matches,
	    matches);
}

/*
 * The 'fileformat' option is changed.
 */
    ଅକ୍ଷର *
did_set_fileformat(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    ଯଦି (!curbuf->b_p_ma && !(args->os_flags & OPT_GLOBAL))
	ଫେରନ୍ତୁ e_cannot_make_changes_modifiable_is_off;
    ଅନ୍ୟଥା ଯଦି (check_opt_strings(*varp, p_ff_ମୂଲ୍ୟଗୁଡ଼ିକ, FALSE) != OK)
	ଫେରନ୍ତୁ e_invalid_argument;

    // may also change 'textmode'
    ଯଦି (get_fileformat(curbuf) == EOL_DOS)
	curbuf->b_p_tx = TRUE;
    ଅନ୍ୟଥା
	curbuf->b_p_tx = FALSE;
    redraw_titles();
    // update flag in swap file
    ml_setflags(curbuf);
    // Redraw needed when switching to/from "mac": a CR in the text
    // will be displayed differently.
    ଯଦି (get_fileformat(curbuf) == EOL_MAC || *args->os_oldval.string == 'm')
	redraw_curbuf_later(UPD_NOT_VALID);

    ଫେରନ୍ତୁ ଖାଲି;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_set_fileformat(optexpand_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_Matches, char_u ***matches)
{
    ଫେରନ୍ତୁ expand_set_opt_string(
	    args,
	    p_ff_ମୂଲ୍ୟଗୁଡ଼ିକ,
	    ARRAY_LENGTH(p_ff_ମୂଲ୍ୟଗୁଡ଼ିକ) - 1,
	    ଅଙ୍କ_Matches,
	    matches);
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * fileformat options.
 */
    char_u *
get_fileformat_name(expand_T *xp UNUSED, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଯଦି (idx >= (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ARRAY_LENGTH(p_ff_ମୂଲ୍ୟଗୁଡ଼ିକ))
	ଫେରନ୍ତୁ ଖାଲି;

    ଫେରନ୍ତୁ (char_u*)p_ff_ମୂଲ୍ୟଗୁଡ଼ିକ[idx];
}

/*
 * The 'fileformats' option is changed.
 */
    ଅକ୍ଷର *
did_set_fileformats(optset_T *args UNUSED)
{
    ଯଦି (check_opt_strings(p_ffs, p_ff_ମୂଲ୍ୟଗୁଡ଼ିକ, TRUE) != OK)
	ଫେରନ୍ତୁ e_invalid_argument;

    // also change 'textauto'
    ଯଦି (*p_ffs == NUL)
	p_ta = FALSE;
    ଅନ୍ୟଥା
	p_ta = TRUE;

    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * The 'filetype' or the 'syntax' option is changed.
 */
    ଅକ୍ଷର *
did_set_filetype_or_syntax(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    ଯଦି (!valid_filetype(*varp))
	ଫେରନ୍ତୁ e_invalid_argument;

    args->os_ମୂଲ୍ୟ_changed = STRCMP(args->os_oldval.string, *varp) != 0;

    // Since we check the value, there is no need to set P_INSECURE,
    // even when the value comes from a modeline.
    args->os_ମୂଲ୍ୟ_checked = TRUE;

    ଫେରନ୍ତୁ ଖାଲି;
}

#ଯଦି defined(FEAT_FOLDING)
/*
 * The 'foldclose' option is changed.
 */
    ଅକ୍ଷର *
did_set_foldclose(optset_T *args UNUSED)
{
    ଫେରନ୍ତୁ did_set_opt_strings(p_fcl, p_fcl_ମୂଲ୍ୟଗୁଡ଼ିକ, TRUE);
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_set_foldclose(optexpand_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_Matches, char_u ***matches)
{
    ଫେରନ୍ତୁ expand_set_opt_string(
	    args,
	    p_fcl_ମୂଲ୍ୟଗୁଡ଼ିକ,
	    ARRAY_LENGTH(p_fcl_ମୂଲ୍ୟଗୁଡ଼ିକ) - 1,
	    ଅଙ୍କ_Matches,
	    matches);
}
#endif

#ଯଦି defined(FEAT_EVAL) && defined(FEAT_FOLDING)
/*
 * The 'foldexpr' option is changed.
 */
    ଅକ୍ଷର *
did_set_foldexpr(optset_T *args)
{
    (ଶୂନ୍ୟ)did_set_optexpr(args);
    ଯଦି (foldmethod_Is_ପଦସମଷ୍ଟି(curwin))
	foldUpdateAll(curwin);
    ଫେରନ୍ତୁ ଖାଲି;
}
#endif

#ଯଦି defined(FEAT_FOLDING)
/*
 * The 'foldignore' option is changed.
 */
    ଅକ୍ଷର *
did_set_foldignore(optset_T *args UNUSED)
{
    ଯଦି (foldmethodIsIndent(curwin))
	foldUpdateAll(curwin);
    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * The 'foldmarker' option is changed.
 */
    ଅକ୍ଷର *
did_set_foldmarker(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u	*p;

    p = vim_strchr(*varp, ',');
    ଯଦି (p == ଖାଲି)
	ଫେରନ୍ତୁ e_comma_required;
    ଅନ୍ୟଥା ଯଦି (p == *varp || p[1] == NUL)
	ଫେରନ୍ତୁ e_invalid_argument;
    ଅନ୍ୟଥା ଯଦି (foldmethodIsMarker(curwin))
	foldUpdateAll(curwin);

    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * The 'foldmethod' option is changed.
 */
    ଅକ୍ଷର *
did_set_foldmethod(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    ଯଦି (check_opt_strings(*varp, p_fdm_ମୂଲ୍ୟଗୁଡ଼ିକ, FALSE) != OK || **varp == NUL)
	ଫେରନ୍ତୁ e_invalid_argument;

    foldUpdateAll(curwin);
    ଯଦି (foldmethodIsDiff(curwin))
	newFoldLevel();
    ଫେରନ୍ତୁ ଖାଲି;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_set_foldmethod(optexpand_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_Matches, char_u ***matches)
{
    ଫେରନ୍ତୁ expand_set_opt_string(
	    args,
	    p_fdm_ମୂଲ୍ୟଗୁଡ଼ିକ,
	    ARRAY_LENGTH(p_fdm_ମୂଲ୍ୟଗୁଡ଼ିକ) - 1,
	    ଅଙ୍କ_Matches,
	    matches);
}

/*
 * The 'foldopen' option is changed.
 */
    ଅକ୍ଷର *
did_set_foldopen(optset_T *args UNUSED)
{
    ଫେରନ୍ତୁ did_set_opt_flags(p_fdo, p_fdo_ମୂଲ୍ୟଗୁଡ଼ିକ, &fdo_flags, TRUE);
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_set_foldopen(optexpand_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_Matches, char_u ***matches)
{
    ଫେରନ୍ତୁ expand_set_opt_string(
	    args,
	    p_fdo_ମୂଲ୍ୟଗୁଡ଼ିକ,
	    ARRAY_LENGTH(p_fdo_ମୂଲ୍ୟଗୁଡ଼ିକ) - 1,
	    ଅଙ୍କ_Matches,
	    matches);
}
#endif

/*
 * The 'formatoptions' option is changed.
 */
    ଅକ୍ଷର *
did_set_formatoptions(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    ଫେରନ୍ତୁ did_set_option_listflag(*varp, (char_u *)FO_ALL, args->os_errbuf,
		    args->os_errbuflen);
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_set_formatoptions(optexpand_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_Matches, char_u ***matches)
{
    ଫେରନ୍ତୁ expand_set_opt_listflag(args, (char_u*)FO_ALL, ଅଙ୍କ_Matches, matches);
}

#ଯଦି defined(CURSOR_SHAPE)
/*
 * The 'guicursor' option is changed.
 */
    ଅକ୍ଷର *
did_set_guicursor(optset_T *args UNUSED)
{
    ଫେରନ୍ତୁ parse_shape_opt(SHAPE_CURSOR);
}
#endif

#ଯଦି defined(FEAT_GUI)
/*
 * The 'guifont' option is changed.
 */
    ଅକ୍ଷର *
did_set_guifont(optset_T *args UNUSED)
{
    char_u	*p;
    ଅକ୍ଷର	*errmsg = ଖାଲି;

    ଯଦି (gui.in_use)
    {
	p = p_guifont;
# ଯଦି defined(FEAT_GUI_GTK)
	// Put up a font dialog and let the user select a new value.
	// If this is cancelled go back to the old value but don't
	// give an error message.
	ଯଦି (STRCMP(p, "*") == 0)
	{
	    p = gui_mch_font_dialog(args->os_oldval.string);
	    free_string_option(p_guifont);
	    p_guifont = (p != ଖାଲି) ? p : vim_strsave(args->os_oldval.string);
	}
# endif
	ଯଦି (p != ଖାଲି && gui_init_font(p_guifont, FALSE) != OK)
	{
# ଯଦି defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_PHOTON)
	    ଯଦି (STRCMP(p_guifont, "*") == 0)
	    {
		// Dialog was cancelled: Keep the old value without giving
		// an error message.
		free_string_option(p_guifont);
		p_guifont = vim_strsave(args->os_oldval.string);
	    }
	    ଅନ୍ୟଥା
# endif
		errmsg = e_invalid_fonts;
	}
    }

    ଫେରନ୍ତୁ errmsg;
}

/*
 * Expand the 'guifont' option. Only when GUI is being used. Each platform has
 * specific behaviors.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_set_guifont(optexpand_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_Matches, char_u ***matches)
{
    ଯଦି (!gui.in_use)
	ଫେରନ୍ତୁ FAIL;

# ଯଦି defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
    char_u **varp = (char_u **)args->oe_varp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା wide = (varp == &p_guifontwide);

    ଫେରନ୍ତୁ expand_set_opt_callback(
	    args, gui_mch_expand_font, &wide, ଅଙ୍କ_Matches, matches);
# ଅନ୍ୟଥା
    ଫେରନ୍ତୁ FAIL;
# endif
}

# ଯଦି defined(FEAT_XFONTSET)
/*
 * The 'guifontset' option is changed.
 */
    ଅକ୍ଷର *
did_set_guifontset(optset_T *args UNUSED)
{
    ଅକ୍ଷର *errmsg = ଖାଲି;

    ଯଦି (STRCMP(p_guifontset, "*") == 0)
	errmsg = e_cant_select_fontset;
    ଅନ୍ୟଥା ଯଦି (gui.in_use && gui_init_font(p_guifontset, TRUE) != OK)
	errmsg = e_invalid_fontset;

    ଫେରନ୍ତୁ errmsg;
}
# endif

/*
 * The 'guifontwide' option is changed.
 */
    ଅକ୍ଷର *
did_set_guifontwide(optset_T *args UNUSED)
{
    ଅକ୍ଷର *errmsg = ଖାଲି;

    ଯଦି (STRCMP(p_guifontwide, "*") == 0)
	errmsg = e_cant_select_wide_font;
    ଅନ୍ୟଥା ଯଦି (gui_get_wide_font() == FAIL)
	errmsg = e_invalid_wide_font;

    ଫେରନ୍ତୁ errmsg;
}
#endif

#ଯଦି defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
/*
 * The 'guiligatures' option is changed.
 */
    ଅକ୍ଷର *
did_set_guiligatures(optset_T *args UNUSED)
{
    gui_set_ligatures();
    ଫେରନ୍ତୁ ଖାଲି;
}
#endif

#ଯଦି defined(FEAT_GUI)
/*
 * The 'guioptions' option is changed.
 */
    ଅକ୍ଷର *
did_set_guioptions(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    ଅକ୍ଷର *errmsg;

    errmsg = did_set_option_listflag(*varp, (char_u *)GO_ALL, args->os_errbuf,
		    args->os_errbuflen);
    ଯଦି (errmsg != ଖାଲି)
	ଫେରନ୍ତୁ errmsg;

    gui_init_which_components(args->os_oldval.string);
    ଫେରନ୍ତୁ ଖାଲି;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_set_guioptions(optexpand_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_Matches, char_u ***matches)
{
    ଫେରନ୍ତୁ expand_set_opt_listflag(args, (char_u*)GO_ALL, ଅଙ୍କ_Matches, matches);
}
#endif

#ଯଦି defined(FEAT_GUI_TABLINE)
/*
 * The 'guitablabel' option is changed.
 */
    ଅକ୍ଷର *
did_set_guitablabel(optset_T *args UNUSED)
{
    redraw_tabline = TRUE;
    ଫେରନ୍ତୁ ଖାଲି;
}
#endif

/*
 * The 'helpfile' option is changed.
 */
    ଅକ୍ଷର *
did_set_helpfile(optset_T *args UNUSED)
{
    // May compute new values for $VIM and $VIMRUNTIME
    ଯଦି (didset_vim)
	vim_unsetenv_ext((char_u *)"VIM");
    ଯଦି (didset_vimruntime)
	vim_unsetenv_ext((char_u *)"VIMRUNTIME");
    ଫେରନ୍ତୁ ଖାଲି;
}

#ଯଦି defined(FEAT_MULTI_LANG)
/*
 * The 'helplang' option is changed.
 */
    ଅକ୍ଷର *
did_set_helplang(optset_T *args UNUSED)
{
    ଅକ୍ଷର *errmsg = ଖାଲି;

    // Check for "", "ab", "ab,cd", etc.
    ପାଇଁ (char_u *s = p_hlg; *s != NUL; s += 3)
    {
	ଯଦି (s[1] == NUL || ((s[2] != ',' || s[3] == NUL) && s[2] != NUL))
	{
	    errmsg = e_invalid_argument;
	    ଭାଙ୍ଗ;
	}
	ଯଦି (s[2] == NUL)
	    ଭାଙ୍ଗ;
    }

    ଫେରନ୍ତୁ errmsg;
}
#endif

/*
 * The 'highlight' option is changed.
 */
    ଅକ୍ଷର *
did_set_highlight(optset_T *args UNUSED)
{
    ଯଦି (highlight_changed() == FAIL)
	ଫେରନ୍ତୁ e_invalid_argument;	// invalid flags

    ଫେରନ୍ତୁ ଖାଲି;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_hl_occasions(
	optexpand_T *args,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_Matches,
	char_u ***matches,
	char_u prefix)
{
    char_u	    *p;
    ସ୍ଥିର char_u   hl_flags[HLF_ଗଣନା] = HL_FLAGS;
    size_t	    i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    ଗଣନା_2 = 0;

    *matches = ALLOC_MULT(char_u *, HLF_ଗଣନା + 1);
    ଯଦି (*matches == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    // We still want to return the full option if it's requested.
    ଯଦି (args->oe_include_orig_val)
    {
	p = vim_strsave(args->oe_opt_ମୂଲ୍ୟ);
	ଯଦି (p == ଖାଲି)
	{
	    VIM_CLEAR(*matches);
	    ଫେରନ୍ତୁ FAIL;
	}
	(*matches)[ଗଣନା_2++] = p;
    }

    ପାଇଁ (i = 0; i < HLF_ଗଣନା; i++)
    {
	p = alloc((prefix == NUL ? 1 : 2) + 1);
	ଯଦି (p == ଖାଲି)
	{
	    ଯଦି (ଗଣନା_2 == 0)
	    {
		VIM_CLEAR(*matches);
		ଫେରନ୍ତୁ FAIL;
	    }
	    ଅନ୍ୟଥା
		ଭାଙ୍ଗ;
	}
	ଯଦି (prefix == NUL)
	    sprintf((ଅକ୍ଷର *)p, "%c", hl_flags[i]);
	ଅନ୍ୟଥା
	    sprintf((ଅକ୍ଷର *)p, "%c%c", prefix, hl_flags[i]);
	(*matches)[ଗଣନା_2++] = p;
    }

    ଯଦି (ଗଣନା_2 == 0)
    {
	VIM_CLEAR(*matches);
	ଫେରନ୍ତୁ FAIL;
    }
    *ଅଙ୍କ_Matches = ଗଣନା_2;
    ଫେରନ୍ତୁ OK;
}

/*
 * Expand 'highlight' option.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_set_highlight(optexpand_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_Matches, char_u ***matches)
{
    char_u	    *p;
    expand_T	    *xp = args->oe_xp;
    size_t	    i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    ଗଣନା_2 = 0;

    ଯଦି (xp->xp_pattern > args->oe_set_arg && *(xp->xp_pattern-1) == ':')
    {
	// Right after a ':', meaning we just return all highlight names.
	ଫେରନ୍ତୁ expand_set_opt_generic(
		args,
		get_highlight_name,
		ଅଙ୍କ_Matches,
		matches);
    }

    ଯଦି (*xp->xp_pattern == NUL)
	// At beginning of a comma-separated list. Return the specific list of
	// supported occasions.
	ଫେରନ୍ତୁ expand_hl_occasions(args, ଅଙ୍କ_Matches, matches, NUL);

    // We are after the initial character (which indicates the occasion). We
    // already made sure we are not matching after a ':' above, so now we want
    // to match against display mode modifiers.
    // Since the xp_pattern starts from the beginning, we need to include it in
    // the returned match.

    // Note: Keep this in sync with highlight_changed()
    ସ୍ଥିର char_u p_hl_mode_ମୂଲ୍ୟଗୁଡ଼ିକ[] =
	{':', 'b', 'i', '-', 'n', 'r', 's', 'u', 'c', '2', 'd', '=', 't'};
    size_t ଅଙ୍କ_hl_modes = ARRAY_LENGTH(p_hl_mode_ମୂଲ୍ୟଗୁଡ଼ିକ);

    *matches = ALLOC_MULT(char_u *, ଅଙ୍କ_hl_modes);
    ଯଦି (*matches == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା pattern_len = xp->xp_pattern_len;

    ପାଇଁ (i = 0; i < ଅଙ୍କ_hl_modes; i++)
    {
	// Don't allow duplicates as these are unique flags
	char_u *dup = vim_strchr(xp->xp_pattern + 1, p_hl_mode_ମୂଲ୍ୟଗୁଡ଼ିକ[i]);
	ଯଦି (dup != ଖାଲି && (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(dup - xp->xp_pattern) < pattern_len)
	    ଜାରି;

	// ':' only works by itself, not with other flags.
	ଯଦି (pattern_len > 1 && p_hl_mode_ମୂଲ୍ୟଗୁଡ଼ିକ[i] == ':')
	    ଜାରି;

	p = vim_strnsave(xp->xp_pattern, pattern_len + 1);
	ଯଦି (p == ଖାଲି)
	{
	    ଯଦି (i == 0)
	    {
		VIM_CLEAR(*matches);
		ଫେରନ୍ତୁ FAIL;
	    }
	    ଅନ୍ୟଥା
		ଭାଙ୍ଗ;
	}
	p[pattern_len] = p_hl_mode_ମୂଲ୍ୟଗୁଡ଼ିକ[i];
	p[pattern_len + 1] = NUL;
	(*matches)[ଗଣନା_2++] = p;
    }
    ଯଦି (ଗଣନା_2 == 0)
    {
	VIM_CLEAR(*matches);
	ଫେରନ୍ତୁ FAIL;
    }
    *ଅଙ୍କ_Matches = ଗଣନା_2;

    ଫେରନ୍ତୁ OK;
}

/*
 * The 'titlestring' or the 'iconstring' option is changed.
 */
    ସ୍ଥିର ଅକ୍ଷର *
parse_titleiconstring(optset_T *args UNUSED, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flagval UNUSED)
{
#ifdef FEAT_STL_OPT
    char_u	**varp = (char_u **)args->os_varp;

    // NULL => statusline syntax
    ଯଦି (vim_strchr(*varp, '%') && check_stl_option(*varp) == ଖାଲି)
	stl_syntax |= flagval;
    ଅନ୍ୟଥା
	stl_syntax &= ~flagval;
#endif
    did_set_title();

    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * The 'iconstring' option is changed.
 */
    ଅକ୍ଷର *
did_set_iconstring(optset_T *args)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା flagval = 0;

#ifdef FEAT_STL_OPT
    flagval = STL_IN_ICON;
#endif

    ଫେରନ୍ତୁ parse_titleiconstring(args, flagval);
}

#ଯଦି defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
/*
 * The 'imactivatekey' option is changed.
 */
    ଅକ୍ଷର *
did_set_imactivatekey(optset_T *args UNUSED)
{
    ଯଦି (!im_xim_isvalid_imactivate())
	ଫେରନ୍ତୁ e_invalid_argument;
    ଫେରନ୍ତୁ ଖାଲି;
}
#endif

#ifdef FEAT_IMAGE
/*
 * The 'imageprotocol' option is changed.
 */
    ଅକ୍ଷର *
did_set_imageprotocol(optset_T *args UNUSED)
{
    ଯଦି (update_image_backend() == FAIL)
	ଫେରନ୍ତୁ e_invalid_argument;
    ଫେରନ୍ତୁ ଖାଲି;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_set_imageprotocol(optexpand_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_Matches, char_u ***matches)
{
    expand_T *xp = args->oe_xp;

    ଯଦି (xp->xp_pattern > args->oe_set_arg && *(xp->xp_pattern-1) == ':')
    {
	// 'imageprotocol' only has well-defined terms for completion for the
	// protocol part after the colon.
	ଫେରନ୍ତୁ expand_set_opt_string(
		args,
		p_ipc_protocol_ମୂଲ୍ୟଗୁଡ଼ିକ,
		ARRAY_LENGTH(p_ipc_protocol_ମୂଲ୍ୟଗୁଡ଼ିକ) - 1,
		ଅଙ୍କ_Matches,
		matches);
    }
    // Use expand_set_opt_string instead of returning FAIL so that we can
    // include the original value if args->oe_include_orig_val is set.
    ସ୍ଥିର ଅକ୍ଷର *(empty[]) = {ଖାଲି};
    ଫେରନ୍ତୁ expand_set_opt_string(args, empty, 0, ଅଙ୍କ_Matches, matches);
}
#endif

/*
 * The 'iskeyword' option is changed.
 */
    ଅକ୍ଷର *
did_set_iskeyword(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    ଯଦି (varp == &p_isk)		// only check for global-value
    {
	ଯଦି (check_isopt(*varp) == FAIL)
	    ଫେରନ୍ତୁ e_invalid_argument;
    }
    ଅନ୍ୟଥା			// fallthrough for local-value
	ଫେରନ୍ତୁ did_set_isopt(args);

    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * The 'isident' or the 'iskeyword' or the 'isprint' or the 'isfname' option is
 * changed.
 */
    ଅକ୍ଷର *
did_set_isopt(optset_T *args)
{
    // 'isident', 'iskeyword', 'isprint' or 'isfname' option: refill g_chartab[]
    // If the new option is invalid, use old value.
    // 'lisp' option: refill g_chartab[] for '-' char.
    ଯଦି (init_chartab() == FAIL)
    {
	args->os_restore_chartab = TRUE;// need to restore the chartab.
	ଫେରନ୍ତୁ e_invalid_argument;	// error in value
    }

    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * The 'jumpoptions' option is changed.
 */
    ଅକ୍ଷର *
did_set_jumpoptions(optset_T *args UNUSED)
{
    ଯଦି (opt_strings_flags(p_jop, p_jop_ମୂଲ୍ୟଗୁଡ଼ିକ, &jop_flags, TRUE) != OK)
	ଫେରନ୍ତୁ e_invalid_argument;

    ଫେରନ୍ତୁ ଖାଲି;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_set_jumpoptions(optexpand_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_Matches, char_u ***matches)
{
    ଫେରନ୍ତୁ expand_set_opt_string(
	    args,
	    p_jop_ମୂଲ୍ୟଗୁଡ଼ିକ,
	    ARRAY_LENGTH(p_jop_ମୂଲ୍ୟଗୁଡ଼ିକ) - 1,
	    ଅଙ୍କ_Matches,
	    matches);
}

#ଯଦି defined(FEAT_KEYMAP)
/*
 * The 'keymap' option is changed.
 */
    ଅକ୍ଷର *
did_set_keymap(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    ଅକ୍ଷର *errmsg = ଖାଲି;

    ଯଦି (!valid_filetype(*varp))
	errmsg = e_invalid_argument;
    ଅନ୍ୟଥା
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    secure_save = secure;

	// Reset the secure flag, since the value of 'keymap' has
	// been checked to be safe.
	secure = 0;

	// load or unload key mapping tables
	errmsg = keymap_init();

	secure = secure_save;

	// Since we check the value, there is no need to set P_INSECURE,
	// even when the value comes from a modeline.
	args->os_ମୂଲ୍ୟ_checked = TRUE;
    }

    ଯଦି (errmsg == ଖାଲି)
    {
	ଯଦି (*curbuf->b_p_keymap != NUL)
	{
	    // Installed a new keymap, switch on using it.
	    curbuf->b_p_iminsert = B_IMODE_LMAP;
	    ଯଦି (curbuf->b_p_imsearch != B_IMODE_USE_ଭର୍ତ୍ତି)
		curbuf->b_p_imsearch = B_IMODE_LMAP;
	}
	ଅନ୍ୟଥା
	{
	    // Cleared the keymap, may reset 'iminsert' and 'imsearch'.
	    ଯଦି (curbuf->b_p_iminsert == B_IMODE_LMAP)
		curbuf->b_p_iminsert = B_IMODE_NONE;
	    ଯଦି (curbuf->b_p_imsearch == B_IMODE_LMAP)
		curbuf->b_p_imsearch = B_IMODE_USE_ଭର୍ତ୍ତି;
	}
	ଯଦି ((args->os_flags & OPT_LOCAL) == 0)
	{
	    set_iminsert_global();
	    set_imsearch_global();
	}
	status_redraw_curbuf();
    }

    ଫେରନ୍ତୁ errmsg;
}
#endif

/*
 * The 'keymodel' option is changed.
 */
    ଅକ୍ଷର *
did_set_keymodel(optset_T *args UNUSED)
{
    ଯଦି (check_opt_strings(p_km, p_km_ମୂଲ୍ୟଗୁଡ଼ିକ, TRUE) != OK)
	ଫେରନ୍ତୁ e_invalid_argument;

    km_stopsel = (vim_strchr(p_km, 'o') != ଖାଲି);
    km_startsel = (vim_strchr(p_km, 'a') != ଖାଲି);
    ଫେରନ୍ତୁ ଖାଲି;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_set_keymodel(optexpand_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_Matches, char_u ***matches)
{
    ଫେରନ୍ତୁ expand_set_opt_string(
	    args,
	    p_km_ମୂଲ୍ୟଗୁଡ଼ିକ,
	    ARRAY_LENGTH(p_km_ମୂଲ୍ୟଗୁଡ଼ିକ) - 1,
	    ଅଙ୍କ_Matches,
	    matches);
}

/*
 * The 'keyprotocol' option is changed.
 */
    ଅକ୍ଷର *
did_set_keyprotocol(optset_T *args UNUSED)
{
    char_u *term = T_NAME;
    keyprot_T kpc = match_keyprotocol(term);
    ଯଦି (kpc == KEYPROTOCOL_FAIL)
	ଫେରନ୍ତୁ e_invalid_argument;

    apply_keyprotocol(term, kpc);

    ଫେରନ୍ତୁ ଖାଲି;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_set_keyprotocol(optexpand_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_Matches, char_u ***matches)
{
    expand_T *xp = args->oe_xp;
    ଯଦି (xp->xp_pattern > args->oe_set_arg && *(xp->xp_pattern-1) == ':')
    {
	// 'keyprotocol' only has well-defined terms for completion for the
	// protocol part after the colon.
	ଫେରନ୍ତୁ expand_set_opt_string(
		args,
		p_kpc_protocol_ମୂଲ୍ୟଗୁଡ଼ିକ,
		ARRAY_LENGTH(p_kpc_protocol_ମୂଲ୍ୟଗୁଡ଼ିକ) - 1,
		ଅଙ୍କ_Matches,
		matches);
    }
    // Use expand_set_opt_string instead of returning FAIL so that we can
    // include the original value if args->oe_include_orig_val is set.
    ସ୍ଥିର ଅକ୍ଷର *(empty[]) = {ଖାଲି};
    ଫେରନ୍ତୁ expand_set_opt_string(args, empty, 0, ଅଙ୍କ_Matches, matches);
}

/*
 * The 'lispoptions' option is changed.
 */
    ଅକ୍ଷର *
did_set_lispoptions(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    ଯଦି (**varp != NUL
	    && STRCMP(*varp, "expr:0") != 0
	    && STRCMP(*varp, "expr:1") != 0)
	ଫେରନ୍ତୁ e_invalid_argument;

    ଫେରନ୍ତୁ ଖାଲି;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_set_lispoptions(optexpand_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_Matches, char_u ***matches)
{
    ସ୍ଥିର ଅକ୍ଷର *(p_lop_ମୂଲ୍ୟଗୁଡ଼ିକ[]) = {"expr:0", "expr:1", ଖାଲି};
    ଫେରନ୍ତୁ expand_set_opt_string(
	    args,
	    p_lop_ମୂଲ୍ୟଗୁଡ଼ିକ,
	    ARRAY_LENGTH(p_lop_ମୂଲ୍ୟଗୁଡ଼ିକ) - 1,
	    ଅଙ୍କ_Matches,
	    matches);
}

/*
 * The 'matchpairs' option is changed.
 */
    ଅକ୍ଷର *
did_set_matchpairs(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u	*p;

    ଯଦି (has_mbyte)
    {
	ପାଇଁ (p = *varp; *p != NUL; ++p)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା x2 = -1;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା x3 = -1;

	    p += mb_ptr2len(p);
	    ଯଦି (*p != NUL)
		x2 = *p++;
	    ଯଦି (*p != NUL)
	    {
		x3 = mb_ptr2char(p);
		p += mb_ptr2len(p);
	    }
	    ଯଦି (x2 != ':' || x3 == -1 || (*p != NUL && *p != ','))
		ଫେରନ୍ତୁ e_invalid_argument;
	    ଯଦି (*p == NUL)
		ଭାଙ୍ଗ;
	}
    }
    ଅନ୍ୟଥା
    {
	// Check for "x:y,x:y"
	ପାଇଁ (p = *varp; *p != NUL; p += 4)
	{
	    ଯଦି (p[1] != ':' || p[2] == NUL || (p[3] != NUL && p[3] != ','))
		ଫେରନ୍ତୁ e_invalid_argument;
	    ଯଦି (p[3] == NUL)
		ଭାଙ୍ଗ;
	}
    }

    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Process the updated 'messagesopt' option value.
 */
    ଅକ୍ଷର *
did_set_messagesopt(optset_T *args UNUSED)
{
    ଯଦି (messagesopt_changed() == FAIL)
	ଫେରନ୍ତୁ e_invalid_argument;

    ଫେରନ୍ତୁ ଖାଲି;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_set_messagesopt(optexpand_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_Matches, char_u ***matches)
{
    ସ୍ଥିର ଅକ୍ଷର *(p_mopt_ମୂଲ୍ୟଗୁଡ଼ିକ[]) = {"hit-enter", "wait:", "history:", ଖାଲି};
    ଫେରନ୍ତୁ expand_set_opt_string(
	    args,
	    p_mopt_ମୂଲ୍ୟଗୁଡ଼ିକ,
	    ARRAY_LENGTH(p_mopt_ମୂଲ୍ୟଗୁଡ଼ିକ) - 1,
	    ଅଙ୍କ_Matches,
	    matches);
}

#ଯଦି defined(FEAT_SPELL)
/*
 * The 'mkspellmem' option is changed.
 */
    ଅକ୍ଷର *
did_set_mkspellmem(optset_T *args UNUSED)
{
    ଯଦି (spell_check_msm() != OK)
	ଫେରନ୍ତୁ e_invalid_argument;

    ଫେରନ୍ତୁ ଖାଲି;
}
#endif

/*
 * The 'mouse' option is changed.
 */
    ଅକ୍ଷର *
did_set_mouse(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    ଅକ୍ଷର	*retval;

    retval = did_set_option_listflag(*varp, (char_u *)MOUSE_ALL, args->os_errbuf,
		    args->os_errbuflen);
    ଯଦି (retval == ଖାଲି)
    {
	redraw_tabline = TRUE;
	ଯଦି (tabline_height() > 0)
	    update_screen(UPD_VALID);
#ଯଦି defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
	popup_close_info(); // Close info popup to apply new properties
#endif
    }
    ଫେରନ୍ତୁ retval;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_set_mouse(optexpand_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_Matches, char_u ***matches)
{
    ଫେରନ୍ତୁ expand_set_opt_listflag(args, (char_u*)MOUSE_ALL, ଅଙ୍କ_Matches, matches);
}

/*
 * The 'mousemodel' option is changed.
 */
    ଅକ୍ଷର *
did_set_mousemodel(optset_T *args UNUSED)
{
    ଯଦି (check_opt_strings(p_mousem, p_mousem_ମୂଲ୍ୟଗୁଡ଼ିକ, FALSE) != OK)
	ଫେରନ୍ତୁ e_invalid_argument;
#ଯଦି defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU) && (XmVersion <= 1002)
    ଅନ୍ୟଥା ଯଦି (*p_mousem != *args->os_oldval.string)
	// Changed from "extend" to "popup" or "popup_setpos" or vv: need
	// to create or delete the popup menus.
	gui_motif_update_mousemodel(ମୂଳ_menu);
#endif

    ଫେରନ୍ତୁ ଖାଲି;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_set_mousemodel(optexpand_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_Matches, char_u ***matches)
{
    ଫେରନ୍ତୁ expand_set_opt_string(
	    args,
	    p_mousem_ମୂଲ୍ୟଗୁଡ଼ିକ,
	    ARRAY_LENGTH(p_mousem_ମୂଲ୍ୟଗୁଡ଼ିକ) - 1,
	    ଅଙ୍କ_Matches,
	    matches);
}

#ଯଦି defined(FEAT_MOUSESHAPE)
    ଅକ୍ଷର *
did_set_mouseshape(optset_T *args UNUSED)
{
    ଅକ୍ଷର *errmsg = ଖାଲି;

    errmsg = parse_shape_opt(SHAPE_MOUSE);
    update_mouseshape(-1);

    ଫେରନ୍ତୁ errmsg;
}
#endif

/*
 * The 'nrformats' option is changed.
 */
    ଅକ୍ଷର *
did_set_nrformats(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    ଫେରନ୍ତୁ did_set_opt_strings(*varp, p_nf_ମୂଲ୍ୟଗୁଡ଼ିକ, TRUE);
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_set_nrformats(optexpand_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_Matches, char_u ***matches)
{
    ଫେରନ୍ତୁ expand_set_opt_string(
	    args,
	    p_nf_ମୂଲ୍ୟଗୁଡ଼ିକ,
	    ARRAY_LENGTH(p_nf_ମୂଲ୍ୟଗୁଡ଼ିକ) - 1,
	    ଅଙ୍କ_Matches,
	    matches);
}

#ଯଦି defined(FEAT_EVAL)
/*
 * One of the '*expr' options is changed: 'balloonexpr', 'diffexpr',
 * 'foldexpr', 'foldtext', 'formatexpr', 'includeexpr', 'indentexpr',
 * 'patchexpr', 'printexpr' and 'charconvert'.
 */
    ଅକ୍ଷର *
did_set_optexpr(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    // If the option value starts with <SID> or s:, then replace that with
    // the script identifier.
    char_u *name = get_scriptlocal_funcname(*varp);
    ଯଦି (name != ଖାଲି)
    {
	free_string_option(*varp);
	*varp = name;
    }

    ଫେରନ୍ତୁ ଖାଲି;
}
#endif

/*
 * The 'pastetoggle' option is changed.
 */
    ଅକ୍ଷର *
did_set_pastetoggle(optset_T *args UNUSED)
{
    char_u	*p;

    // translate key codes like in a mapping
    ଯଦି (*p_pt)
    {
	(ଶୂନ୍ୟ)replace_termcodes(p_pt, &p, 0,
		REPTERM_FROM_PART | REPTERM_DO_LT, ଖାଲି);
	ଯଦି (p != ଖାଲି)
	{
	    free_string_option(p_pt);
	    p_pt = p;
	}
    }

    ଫେରନ୍ତୁ ଖାଲି;
}

#ଯଦି defined(FEAT_PROP_POPUP)
/*
 * The 'previewpopup' option is changed.
 */
    ଅକ୍ଷର *
did_set_previewpopup(optset_T *args UNUSED)
{
    ଯଦି (parse_previewpopup(ଖାଲି) == FAIL)
	ଫେରନ୍ତୁ e_invalid_argument;

# ଯଦି defined(FEAT_QUICKFIX)
    popup_close_info();
# endif
    ଫେରନ୍ତୁ ଖାଲି;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_set_popupoption(optexpand_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_Matches, char_u ***matches,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା previewpopup)
{
    expand_T *xp = args->oe_xp;

    ଯଦି (xp->xp_pattern > args->oe_set_arg && *(xp->xp_pattern-1) == ':')
    {
	ଯଦି (completing_ମୂଲ୍ୟ_for_subopt(args, "close")
		|| completing_ମୂଲ୍ୟ_for_subopt(args, "resize")
		|| completing_ମୂଲ୍ୟ_for_subopt(args, "shadow"))
	{
	    ଫେରନ୍ତୁ expand_set_opt_string(
		    args,
		    p_popup_option_on_off_ମୂଲ୍ୟଗୁଡ଼ିକ,
		    ARRAY_LENGTH(p_popup_option_on_off_ମୂଲ୍ୟଗୁଡ଼ିକ) - 1,
		    ଅଙ୍କ_Matches,
		    matches);
	}
	ଯଦି (completing_ମୂଲ୍ୟ_for_subopt(args, "border"))
	{
	    ଫେରନ୍ତୁ expand_set_opt_string(
		    args,
		    previewpopup ? p_popup_pvp_border_ମୂଲ୍ୟଗୁଡ଼ିକ
			: p_popup_cpp_border_ମୂଲ୍ୟଗୁଡ଼ିକ,
		    (previewpopup ? ARRAY_LENGTH(p_popup_pvp_border_ମୂଲ୍ୟଗୁଡ଼ିକ) - 1
			: ARRAY_LENGTH(p_popup_cpp_border_ମୂଲ୍ୟଗୁଡ଼ିକ)) - 1,
		    ଅଙ୍କ_Matches,
		    matches);
	}
	ଯଦି (completing_ମୂଲ୍ୟ_for_subopt(args, "align"))
	{
	    ଫେରନ୍ତୁ expand_set_opt_string(
		    args,
		    p_popup_option_align_ମୂଲ୍ୟଗୁଡ଼ିକ,
		    ARRAY_LENGTH(p_popup_option_align_ମୂଲ୍ୟଗୁଡ଼ିକ) - 1,
		    ଅଙ୍କ_Matches,
		    matches);
	}
	ଯଦି (completing_ମୂଲ୍ୟ_for_subopt(args, "highlight")
		|| completing_ମୂଲ୍ୟ_for_subopt(args, "borderhighlight"))
	{
	    // Return the list of all highlight names
	    ଫେରନ୍ତୁ expand_set_opt_generic(
		    args,
		    get_highlight_name,
		    ଅଙ୍କ_Matches,
		    matches);
	}
	ଫେରନ୍ତୁ FAIL;
    }

    ଫେରନ୍ତୁ expand_set_opt_string(
	    args,
	    previewpopup ? p_popup_pvp_option_ମୂଲ୍ୟଗୁଡ଼ିକ
		: p_popup_cpp_option_ମୂଲ୍ୟଗୁଡ଼ିକ,
	    previewpopup ? ARRAY_LENGTH(p_popup_pvp_option_ମୂଲ୍ୟଗୁଡ଼ିକ) - 1
		: ARRAY_LENGTH(p_popup_cpp_option_ମୂଲ୍ୟଗୁଡ଼ିକ) - 1,
	    ଅଙ୍କ_Matches,
	    matches);
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_set_previewpopup(optexpand_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_Matches, char_u ***matches)
{
    ଫେରନ୍ତୁ expand_set_popupoption(args, ଅଙ୍କ_Matches, matches, TRUE);
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_set_completepopup(optexpand_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_Matches, char_u ***matches)
{
    ଫେରନ୍ତୁ expand_set_popupoption(args, ଅଙ୍କ_Matches, matches, FALSE);
}
#endif

#ଯଦି defined(FEAT_POSTSCRIPT)
/*
 * The 'printencoding' option is changed.
 */
    ଅକ୍ଷର *
did_set_printencoding(optset_T *args UNUSED)
{
    char_u	*s, *p;

    // Canonize 'printencoding' if VIM standard one
    p = enc_canonize(p_penc);
    ଯଦି (p != ଖାଲି)
    {
	vim_free(p_penc);
	p_penc = p;
    }
    ଅନ୍ୟଥା
    {
	// Ensure lower case and '-' for '_'
	ପାଇଁ (s = p_penc; *s != NUL; s++)
	{
	    ଯଦି (*s == '_')
		*s = '-';
	    ଅନ୍ୟଥା
		*s = TOLOWER_ASC(*s);
	}
    }

    ଫେରନ୍ତୁ ଖାଲି;
}
#endif

#ଯଦି defined(FEAT_PRINTER)

    ସ୍ଥିର char_u *
get_printoptions_names(expand_T *xp UNUSED, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଯଦି (idx >= (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ARRAY_LENGTH(printer_opts))
	ଫେରନ୍ତୁ ଖାଲି;
    ଫେରନ୍ତୁ (char_u*)printer_opts[idx].name;
}

/*
 * Expand 'printoptions' option
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_set_printoptions(optexpand_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_Matches, char_u ***matches)
{
    ଫେରନ୍ତୁ expand_set_opt_generic(
	    args,
	    get_printoptions_names,
	    ଅଙ୍କ_Matches,
	    matches);
}
#endif

#ଯଦି defined(FEAT_STL_OPT)
/*
 * The 'statusline' or the 'tabline' or the 'rulerformat' option is changed.
 * "rulerformat" is TRUE if the 'rulerformat' option is changed.
 */
    ସ୍ଥିର ଅକ୍ଷର *
parse_statustabline_rulerformat(optset_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା rulerformat)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u	*s;
    ଅକ୍ଷର	*errmsg = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		wid;

    ଯଦି (rulerformat)	// reset ru_wid first
	ru_wid = 0;
    s = *varp;
    ଯଦି (rulerformat && *s == '%')
    {
	// set ru_wid if 'ruf' starts with "%99("
	ଯଦି (*++s == '-')	// ignore a '-'
	    s++;
	wid = getdigits(&s);
	ଯଦି (wid && *s == '(' && (errmsg = check_stl_option(p_ruf)) == ଖାଲି)
	    ru_wid = wid;
	ଅନ୍ୟଥା
	{
	    // Validate the flags in 'rulerformat' only if it doesn't point to
	    // a custom function ("%!" flag).
	    ଯଦି ((*varp)[1] != '!')
		errmsg = check_stl_option(p_ruf);
	}
    }
    // check 'statusline' or 'tabline' only if it doesn't start with "%!"
    ଅନ୍ୟଥା ଯଦି (rulerformat || s[0] != '%' || s[1] != '!')
	errmsg = check_stl_option(s);
    ଯଦି (rulerformat && errmsg == ଖାଲି)
	comp_col();

    ଫେରନ୍ତୁ errmsg;
}
#endif

#ଯଦି defined(FEAT_RENDER_OPTIONS)
/*
 * The 'renderoptions' option is changed.
 */
    ଅକ୍ଷର *
did_set_renderoptions(optset_T *args UNUSED)
{
    ଯଦି (!gui_mch_set_rendering_options(p_rop))
	ଫେରନ୍ତୁ e_invalid_argument;

    ଫେରନ୍ତୁ ଖାଲି;
}
#endif

#ଯଦି defined(FEAT_RIGHTLEFT)
/*
 * The 'rightleftcmd' option is changed.
 */
    ଅକ୍ଷର *
did_set_rightleftcmd(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    // Currently only "search" is a supported value.
    ଯଦି (**varp != NUL && STRCMP(*varp, "search") != 0)
	ଫେରନ୍ତୁ e_invalid_argument;

    ଫେରନ୍ତୁ ଖାଲି;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_set_rightleftcmd(optexpand_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_Matches, char_u ***matches)
{
    ସ୍ଥିର ଅକ୍ଷର *(p_rlc_ମୂଲ୍ୟଗୁଡ଼ିକ[]) = {"search", ଖାଲି};
    ଫେରନ୍ତୁ expand_set_opt_string(
	    args,
	    p_rlc_ମୂଲ୍ୟଗୁଡ଼ିକ,
	    ARRAY_LENGTH(p_rlc_ମୂଲ୍ୟଗୁଡ଼ିକ) - 1,
	    ଅଙ୍କ_Matches,
	    matches);
}
#endif

#define PUM_BORDER_CLEAR()   \
    କର {                     \
	pum_set_border(FALSE); \
	pum_set_shadow(FALSE); \
	pum_set_margin(FALSE); \
    } ଯେତେବେଳେ (0)

/*
 * Parse a border value from a pumopt "border:" token.
 * Returns OK on success, FAIL on error.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
parse_pumopt_border(char_u *val, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len)
{
    // Use box-drawing characters only when 'encoding' is "utf-8" and
    // 'ambiwidth' is "single".
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    can_use_box_chars = (enc_utf8 && *p_ambw == 's');
    char_u  *token;

    token = vim_strnsave(val, len);
    ଯଦି (token == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    ଯଦି (can_use_box_chars && STRCMP(token, "single") == 0)
	pum_set_border_chars(0x2500, 0x2502, 0x2500, 0x2502, // ─ │ ─ │
		0x250c, 0x2510, 0x2518, 0x2514); // ┌ ┐ ┘ └
    ଅନ୍ୟଥା ଯଦି (can_use_box_chars && STRCMP(token, "double") == 0)
	pum_set_border_chars(0x2550, 0x2551, 0x2550, 0x2551, // ═ ║ ═ ║
		0x2554, 0x2557, 0x255D, 0x255A); // ╔ ╗ ╝  ╚
    ଅନ୍ୟଥା ଯଦି (can_use_box_chars && STRCMP(token, "round") == 0)
	pum_set_border_chars(0x2500, 0x2502, 0x2500, 0x2502, // ─ │ ─ │
		0x256d, 0x256e, 0x256f, 0x2570); // ╭ ╮ ╯ ╰
    ଅନ୍ୟଥା ଯଦି (STRCMP(token, "ascii") == 0)
	pum_set_border_chars('-', '|', '-', '|', '+', '+', '+', '+');
    ଅନ୍ୟଥା ଯଦି (STRNCMP(token, "custom:", 7) == 0)
    {
	char_u	*q = token + 7;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	out[8];

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < 8; i++)
	{
	    ଯଦି (*q == NUL || *q == ',')
	    {
		vim_free(token);
		ଫେରନ୍ତୁ FAIL;
	    }
	    out[i] = mb_ptr2char(q);
	    mb_ptr2char_adv(&q);
	    ଯଦି (i < 7)
	    {
		ଯଦି (*q != ';')
		{
		    vim_free(token);
		    ଫେରନ୍ତୁ FAIL;
		}
		q++;
	    }
	}
	ଯଦି (*q != NUL && *q != ',')
	{
	    vim_free(token);
	    ଫେରନ୍ତୁ FAIL;
	}
	pum_set_border_chars(out[0], out[1], out[2], out[3], out[4], out[5],
		out[6], out[7]);
    }
    ଅନ୍ୟଥା
    {
	vim_free(token);
	ଫେରନ୍ତୁ FAIL;
    }

    vim_free(token);
    pum_set_border(TRUE);
    ଫେରନ୍ତୁ OK;
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
    ଅକ୍ଷର *
did_set_pumopt(optset_T *args)
{
    char_u  **varp = (char_u **)args->os_varp;
    char_u  *p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    have_border = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    have_margin = FALSE;

    // Reset to defaults.
    PUM_BORDER_CLEAR();
    p_ph = 0;
    p_pw = 15;
    p_pmw = 0;
    p_po = 100;

    ଯଦି (*varp == ଖାଲି || **varp == NUL)
	ଫେରନ୍ତୁ ଖାଲି;

    ପାଇଁ (p = *varp; p != ଖାଲି && *p != NUL; )
    {
	char_u *comma = vim_strchr(p, ',');
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା len;

	ଯଦି (comma != ଖାଲି)
	    len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(comma - p);
	ଅନ୍ୟଥା
	    len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(p);

	ଯଦି (STRNCMP(p, "border:", 7) == 0)
	{
	    ଯଦି (have_border)
		ଯାଅ error;
	    have_border = TRUE;
	    ଯଦି (parse_pumopt_border(p + 7, len - 7) == FAIL)
		ଯାଅ error;
	}
	ଅନ୍ୟଥା ଯଦି (STRNCMP(p, "height:", 7) == 0)
	{
	    long n = atol((ଅକ୍ଷର *)p + 7);
	    ଯଦି (n < 0)
		ଯାଅ error;
	    p_ph = n;
	}
	ଅନ୍ୟଥା ଯଦି (STRNCMP(p, "width:", 6) == 0)
	{
	    long n = atol((ଅକ୍ଷର *)p + 6);
	    ଯଦି (n < 0)
		ଯାଅ error;
	    p_pw = n;
	}
	ଅନ୍ୟଥା ଯଦି (STRNCMP(p, "maxwidth:", 9) == 0)
	{
	    long n = atol((ଅକ୍ଷର *)p + 9);
	    ଯଦି (n < 0)
		ଯାଅ error;
	    p_pmw = n;
	}
	ଅନ୍ୟଥା ଯଦି (STRNCMP(p, "opacity:", 8) == 0)
	{
	    long n = atol((ଅକ୍ଷର *)p + 8);
	    ଯଦି (n < 0 || n > 100)
		ଯାଅ error;
	    p_po = n;
	}
	ଅନ୍ୟଥା ଯଦି (len == 6 && STRNCMP(p, "shadow", 6) == 0)
	    pum_set_shadow(TRUE);
	ଅନ୍ୟଥା ଯଦି (len == 6 && STRNCMP(p, "margin", 6) == 0)
	{
	    have_margin = TRUE;
	    pum_set_margin(TRUE);
	}
	ଅନ୍ୟଥା
	    ଯାଅ error;

	ଯଦି (comma != ଖାଲି)
	    p = comma + 1;
	ଅନ୍ୟଥା
	    ଭାଙ୍ଗ;
    }

    ଯଦି (have_margin && !have_border)
	ଯାଅ error;

    // Invalidate cached background for opacity changes.
    pum_opacity_changed();

    ଫେରନ୍ତୁ ଖାଲି;

error:
    PUM_BORDER_CLEAR();
    p_ph = 0;
    p_pw = 15;
    p_pmw = 0;
    p_po = 100;
    ଫେରନ୍ତୁ e_invalid_argument;
}

    ସ୍ଥିର char_u *
get_pum_border_style(expand_T *xp UNUSED, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ସ୍ଥିର ଅକ୍ଷର *styles[] = {"ascii", "custom:", "single", "double", "round"};
    ଫେରନ୍ତୁ idx < ((enc_utf8 && *p_ambw == 's') ? (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ARRAY_LENGTH(styles) : 2)
	    ? (char_u *)styles[idx] : ଖାଲି;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_set_pumopt(optexpand_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_Matches, char_u ***matches)
{
    expand_T *xp = args->oe_xp;

    ଯଦି (xp->xp_pattern > args->oe_set_arg && *(xp->xp_pattern-1) == ':')
    {
	ଯଦି (completing_ମୂଲ୍ୟ_for_subopt(args, "border"))
	{
	    ଫେରନ୍ତୁ expand_set_opt_generic(
		    args, get_pum_border_style, ଅଙ୍କ_Matches, matches);
	}
	ଫେରନ୍ତୁ FAIL;
    }

    ସ୍ଥିର ଅକ୍ଷର *(p_pumopt_ମୂଲ୍ୟଗୁଡ଼ିକ[]) = {"border:", "height:", "width:",
	"maxwidth:", "opacity:", "shadow", "margin", ଖାଲି};
    ଫେରନ୍ତୁ expand_set_opt_string(
	    args,
	    p_pumopt_ମୂଲ୍ୟଗୁଡ଼ିକ,
	    ARRAY_LENGTH(p_pumopt_ମୂଲ୍ୟଗୁଡ଼ିକ) - 1,
	    ଅଙ୍କ_Matches,
	    matches);
}

/*
 * The 'pumborder' option is changed.
 * Rules:
 *   - One of { single, double, round, ascii, custom:XXXXXXXX } may appear.
 *   - "margin" may appear, but only together with exactly one border style.
 *   - "shadow" is independent and can be combined freely.
 */
    ଅକ୍ଷର *
did_set_pumborder(optset_T *args)
{
    char_u  **varp = (char_u **)args->os_varp;
    char_u  *p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    have_border = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    have_margin = FALSE;

    PUM_BORDER_CLEAR();

    ଯଦି (*varp == ଖାଲି || **varp == NUL)
	ଫେରନ୍ତୁ ଖାଲି;

    ପାଇଁ (p = *varp; p != ଖାଲି && *p != NUL; )
    {
	char_u *comma = vim_strchr(p, ',');
	ଯଦି (comma != ଖାଲି)
	    len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(comma - p);
	ଅନ୍ୟଥା
	    len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(p);

	ଯଦି (STRNCMP(p, "shadow", len) == 0 && len == 6)
	    pum_set_shadow(TRUE);
	ଅନ୍ୟଥା ଯଦି (STRNCMP(p, "margin", len) == 0 && len == 6)
	{
	    have_margin = TRUE;
	    pum_set_margin(TRUE);
	}
	ଅନ୍ୟଥା
	{
	    ଯଦି (have_border)
		ଯାଅ error;
	    have_border = TRUE;
	    ଯଦି (parse_pumopt_border(p, len) == FAIL)
		ଯାଅ error;
	}

	ଯଦି (comma != ଖାଲି)
	    p = comma + 1;
	ଅନ୍ୟଥା
	    ଭାଙ୍ଗ;
    }

    ଯଦି (have_margin && !have_border)
	ଯାଅ error;

    ଫେରନ୍ତୁ ଖାଲି;

error:
    PUM_BORDER_CLEAR();
    pum_set_border_chars(0, 0, 0, 0, 0, 0, 0, 0);
    ଫେରନ୍ତୁ e_invalid_argument;
}

    ସ୍ଥିର char_u *
get_pumborder_token(expand_T *xp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଫେରନ୍ତୁ idx == 0 ? (char_u *)"margin"
	 : idx == 1 ? (char_u *)"shadow"
	 : get_pum_border_style(xp, idx - 2);
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_set_pumborder(optexpand_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_Matches, char_u ***matches)
{
    ଫେରନ୍ତୁ expand_set_opt_generic(
	    args, get_pumborder_token, ଅଙ୍କ_Matches, matches);
}

#ଯଦି defined(FEAT_STL_OPT)

/*
 * The 'rulerformat' option is changed.
 */
    ଅକ୍ଷର *
did_set_rulerformat(optset_T *args)
{
    ଫେରନ୍ତୁ parse_statustabline_rulerformat(args, TRUE);
}
#endif

#ଯଦି defined(FEAT_TABPANEL)
/*
 * Process the new 'tabpanelopt' option value.
 */
    ଅକ୍ଷର *
did_set_tabpanelopt(optset_T *args UNUSED)
{
    ଯଦି (tabpanelopt_changed() == FAIL)
	ଫେରନ୍ତୁ e_invalid_argument;

    ଫେରନ୍ତୁ ଖାଲି;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_set_tabpanelopt(optexpand_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_Matches, char_u ***matches)
{
    expand_T *xp = args->oe_xp;

    ଯଦି (xp->xp_pattern > args->oe_set_arg && *(xp->xp_pattern-1) == ':')
    {
	ଯଦି (completing_ମୂଲ୍ୟ_for_subopt(args, "align"))
	{
	    ଫେରନ୍ତୁ expand_set_opt_string(
		    args,
		    p_tplo_align_ମୂଲ୍ୟଗୁଡ଼ିକ,
		    ARRAY_LENGTH(p_tplo_align_ମୂଲ୍ୟଗୁଡ଼ିକ) - 1,
		    ଅଙ୍କ_Matches,
		    matches);
	}
	ଫେରନ୍ତୁ FAIL;
    }

    ଫେରନ୍ତୁ expand_set_opt_string(
	    args,
	    p_tplo_ମୂଲ୍ୟଗୁଡ଼ିକ,
	    ARRAY_LENGTH(p_tplo_ମୂଲ୍ୟଗୁଡ଼ିକ) - 1,
	    ଅଙ୍କ_Matches,
	    matches);
}
#endif

/*
 * The 'scrollopt' option is changed.
 */
    ଅକ୍ଷର *
did_set_scrollopt(optset_T *args UNUSED)
{
    ଫେରନ୍ତୁ did_set_opt_strings(p_sbo, p_scbopt_ମୂଲ୍ୟଗୁଡ଼ିକ, TRUE);
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_set_scrollopt(optexpand_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_Matches, char_u ***matches)
{
    ଫେରନ୍ତୁ expand_set_opt_string(
	    args,
	    p_scbopt_ମୂଲ୍ୟଗୁଡ଼ିକ,
	    ARRAY_LENGTH(p_scbopt_ମୂଲ୍ୟଗୁଡ଼ିକ) - 1,
	    ଅଙ୍କ_Matches,
	    matches);
}

/*
 * The 'wlseat' option is changed
 */
    ଅକ୍ଷର *
did_set_wlseat(optset_T *args UNUSED)
{
#ifdef FEAT_WAYLAND_CLIPBOARD
    // If there isn't any seat named 'wlseat', then let the Wayland clipboard be
    // unavailable. Ignore errors returned.
    clip_reset_wayland();
#endif

    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * The 'selection' option is changed.
 */
    ଅକ୍ଷର *
did_set_selection(optset_T *args UNUSED)
{
    ଯଦି (*p_sel == NUL || check_opt_strings(p_sel, p_sel_ମୂଲ୍ୟଗୁଡ଼ିକ, FALSE) != OK)
	ଫେରନ୍ତୁ e_invalid_argument;

    ଫେରନ୍ତୁ ଖାଲି;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_set_selection(optexpand_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_Matches, char_u ***matches)
{
    ଫେରନ୍ତୁ expand_set_opt_string(
	    args,
	    p_sel_ମୂଲ୍ୟଗୁଡ଼ିକ,
	    ARRAY_LENGTH(p_sel_ମୂଲ୍ୟଗୁଡ଼ିକ) - 1,
	    ଅଙ୍କ_Matches,
	    matches);
}

/*
 * The 'selectmode' option is changed.
 */
    ଅକ୍ଷର *
did_set_selectmode(optset_T *args UNUSED)
{
    ଫେରନ୍ତୁ did_set_opt_strings(p_slm, p_slm_ମୂଲ୍ୟଗୁଡ଼ିକ, TRUE);
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_set_selectmode(optexpand_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_Matches, char_u ***matches)
{
    ଫେରନ୍ତୁ expand_set_opt_string(
	    args,
	    p_slm_ମୂଲ୍ୟଗୁଡ଼ିକ,
	    ARRAY_LENGTH(p_slm_ମୂଲ୍ୟଗୁଡ଼ିକ) - 1,
	    ଅଙ୍କ_Matches,
	    matches);
}

#ଯଦି defined(FEAT_SESSION)
/*
 * The 'sessionoptions' option is changed.
 */
    ଅକ୍ଷର *
did_set_sessionoptions(optset_T *args)
{
    ଯଦି (opt_strings_flags(p_ssop, p_ssop_ମୂଲ୍ୟଗୁଡ଼ିକ, &ssop_flags, TRUE) != OK)
	ଫେରନ୍ତୁ e_invalid_argument;
    ଯଦି ((ssop_flags & SSOP_CURDIR) && (ssop_flags & SSOP_SESDIR))
    {
	// Don't allow both "sesdir" and "curdir".
	(ଶୂନ୍ୟ)opt_strings_flags(args->os_oldval.string, p_ssop_ମୂଲ୍ୟଗୁଡ଼ିକ,
							&ssop_flags, TRUE);
	ଫେରନ୍ତୁ e_invalid_argument;
    }

    ଫେରନ୍ତୁ ଖାଲି;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_set_sessionoptions(
    optexpand_T		*args,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			*ଅଙ୍କ_Matches,
    char_u		***matches)
{
    ଫେରନ୍ତୁ expand_set_opt_string(
	    args,
	    p_ssop_ମୂଲ୍ୟଗୁଡ଼ିକ,
	    ARRAY_LENGTH(p_ssop_ମୂଲ୍ୟଗୁଡ଼ିକ) - 1,
	    ଅଙ୍କ_Matches,
	    matches);
}
#endif

/*
 * Validate 'shellpipe'/'shellredir' option.
 */
    ଅକ୍ଷର *
did_set_shellpipe_redir(optset_T *args)
{
    char_u	*p;
    bool	seen = false;

    ପାଇଁ (p = args->os_newval.string; *p != NUL; ++p)
    {
	ଯଦି (*p != '%')
	    ଜାରି;

	ଯଦି (p[1] == NUL)
	    ଫେରନ୍ତୁ e_invalid_format_string_single_percent_s;

	ଯଦି (p[1] == '%')
	{
	    ++p;    // skip second %
	    ଜାରି;
	}

	ଯଦି (p[1] == 's')
	{
	    ଯଦି (seen)
		ଫେରନ୍ତୁ e_invalid_format_string_single_percent_s;

	    seen = true;
	    ++p;    // consume 's'
	    ଜାରି;
	}
	ଫେରନ୍ତୁ e_invalid_format_string_single_percent_s;
    }
    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * The 'shortmess' option is changed.
 */
    ଅକ୍ଷର *
did_set_shortmess(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    ଫେରନ୍ତୁ did_set_option_listflag(*varp, (char_u *)SHM_ALL, args->os_errbuf,
		    args->os_errbuflen);
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_set_shortmess(optexpand_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_Matches, char_u ***matches)
{
    ଫେରନ୍ତୁ expand_set_opt_listflag(args, (char_u*)SHM_ALL, ଅଙ୍କ_Matches, matches);
}

#ଯଦି defined(FEAT_LINEBREAK)
/*
 * The 'showbreak' option is changed.
 */
    ଅକ୍ଷର *
did_set_showbreak(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u	*s;

    ପାଇଁ (s = *varp; *s; )
    {
	ଯଦି (ptr2cells(s) != 1)
	    ଫେରନ୍ତୁ e_showbreak_contains_unprintable_or_wide_character;
	MB_PTR_ADV(s);
    }

    ଫେରନ୍ତୁ ଖାଲି;
}
#endif

/*
 * The 'showcmdloc' option is changed.
 */
    ଅକ୍ଷର *
did_set_showcmdloc(optset_T *args UNUSED)
{
    ଅକ୍ଷର	*errmsg = did_set_opt_strings(p_sloc, p_sloc_ମୂଲ୍ୟଗୁଡ଼ିକ, FALSE);

    ଯଦି (errmsg == ଖାଲି)
	comp_col();

    ଫେରନ୍ତୁ errmsg;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_set_showcmdloc(optexpand_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_Matches, char_u ***matches)
{
    ଫେରନ୍ତୁ expand_set_opt_string(
	    args,
	    p_sloc_ମୂଲ୍ୟଗୁଡ଼ିକ,
	    ARRAY_LENGTH(p_sloc_ମୂଲ୍ୟଗୁଡ଼ିକ) - 1,
	    ଅଙ୍କ_Matches,
	    matches);
}

#ଯଦି defined(FEAT_SIGNS)
/*
 * The 'signcolumn' option is changed.
 */
    ଅକ୍ଷର *
did_set_signcolumn(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

# ଯଦି defined(FEAT_LINEBREAK)
    ଯଦି (check_opt_strings(*varp, p_scl_ମୂଲ୍ୟଗୁଡ଼ିକ, FALSE) != OK)
	ଫେରନ୍ତୁ e_invalid_argument;
    // When changing the 'signcolumn' to or from 'number', recompute the
    // width of the number column if 'number' or 'relativenumber' is set.
    ଯଦି (((*args->os_oldval.string == 'n' && args->os_oldval.string[1] == 'u')
		|| (*curwin->w_p_scl == 'n' && *(curwin->w_p_scl + 1) =='u'))
	    && (curwin->w_p_nu || curwin->w_p_rnu))
	curwin->w_nrwidth_ଧାଡ଼ି_ଗଣନା = 0;
# endif

# ଯଦି defined(FEAT_GUI)
    // In the GUI, when sign icons are used, a full screen clear is needed
    // to properly redraw the sign icons.
    ଯଦି (gui.in_use && curbuf->b_signlist != ଖାଲି)
	redraw_all_later(UPD_CLEAR);
# endif

    ଫେରନ୍ତୁ ଖାଲି;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_set_signcolumn(optexpand_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_Matches, char_u ***matches)
{
    ଫେରନ୍ତୁ expand_set_opt_string(
	    args,
	    p_scl_ମୂଲ୍ୟଗୁଡ଼ିକ,
	    ARRAY_LENGTH(p_scl_ମୂଲ୍ୟଗୁଡ଼ିକ) - 1,
	    ଅଙ୍କ_Matches,
	    matches);
}
#endif

#ଯଦି defined(FEAT_SPELL)
/*
 * The 'spellcapcheck' option is changed.
 */
    ଅକ୍ଷର *
did_set_spellcapcheck(optset_T *args UNUSED)
{
    // compile the regexp program.
    ଫେରନ୍ତୁ compile_cap_prog(curwin->w_s);
}

/*
 * The 'spellfile' option is changed.
 */
    ଅକ୍ଷର *
did_set_spellfile(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    ଯଦି (!valid_spellfile(*varp))
	ଫେରନ୍ତୁ e_invalid_argument;

    // If there is a window for this buffer in which 'spell' is set load the
    // wordlists.
    ଫେରନ୍ତୁ did_set_spell_option();
}

/*
 * The 'spell' option is changed.
 */
    ଅକ୍ଷର *
did_set_spelllang(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    ଯଦି (!valid_spelllang(*varp))
	ଫେରନ୍ତୁ e_invalid_argument;

    // If there is a window for this buffer in which 'spell' is set load the
    // wordlists.
    ଫେରନ୍ତୁ did_set_spell_option();
}

/*
 * The 'spelloptions' option is changed.
 */
    ଅକ୍ଷର *
did_set_spelloptions(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    ଯଦି (**varp != NUL && STRCMP(*varp, "camel") != 0)
	ଫେରନ୍ତୁ e_invalid_argument;

    ଫେରନ୍ତୁ ଖାଲି;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_set_spelloptions(optexpand_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_Matches, char_u ***matches)
{
    ସ୍ଥିର ଅକ୍ଷର *(p_spo_ମୂଲ୍ୟଗୁଡ଼ିକ[]) = {"camel", ଖାଲି};
    ଫେରନ୍ତୁ expand_set_opt_string(
	    args,
	    p_spo_ମୂଲ୍ୟଗୁଡ଼ିକ,
	    ARRAY_LENGTH(p_spo_ମୂଲ୍ୟଗୁଡ଼ିକ) - 1,
	    ଅଙ୍କ_Matches,
	    matches);
}

/*
 * The 'spellsuggest' option is changed.
 */
    ଅକ୍ଷର *
did_set_spellsuggest(optset_T *args UNUSED)
{
    ଯଦି (spell_check_sps() != OK)
	ଫେରନ୍ତୁ e_invalid_argument;

    ଫେରନ୍ତୁ ଖାଲି;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_set_spellsuggest(optexpand_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_Matches, char_u ***matches)
{
    ଫେରନ୍ତୁ expand_set_opt_string(
	    args,
	    p_sps_ମୂଲ୍ୟଗୁଡ଼ିକ,
	    ARRAY_LENGTH(p_sps_ମୂଲ୍ୟଗୁଡ଼ିକ) - 1,
	    ଅଙ୍କ_Matches,
	    matches);
}
#endif

/*
 * The 'splitkeep' option is changed.
 */
    ଅକ୍ଷର *
did_set_splitkeep(optset_T *args UNUSED)
{
    win_T	*wp;
    tabpage_T	*tp;
    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	wp->w_prev_height = wp->w_height;
	wp->w_prev_winrow = wp->w_winrow;
    }
    ଫେରନ୍ତୁ did_set_opt_strings(p_spk, p_spk_ମୂଲ୍ୟଗୁଡ଼ିକ, FALSE);
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_set_splitkeep(optexpand_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_Matches, char_u ***matches)
{
    ଫେରନ୍ତୁ expand_set_opt_string(
	    args,
	    p_spk_ମୂଲ୍ୟଗୁଡ଼ିକ,
	    ARRAY_LENGTH(p_spk_ମୂଲ୍ୟଗୁଡ଼ିକ) - 1,
	    ଅଙ୍କ_Matches,
	    matches);
}

#ଯଦି defined(FEAT_STL_OPT)
/*
 * The 'statusline' option is changed.
 */
    ଅକ୍ଷର *
did_set_statusline(optset_T *args)
{
    char_u  **varp = (char_u **)args->os_varp;
    ଅକ୍ଷର    *ret = parse_statustabline_rulerformat(args, FALSE);

    ଯଦି (ret != ଖାଲି)
	ଫେରନ୍ତୁ ret;
    update_stl_rendered_height(varp == &curwin->w_p_stl ? curwin : ଖାଲି);
    frame_change_statusline_height();

    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Rewrite the 'statuslineopt' string: replace the last "maxheight:N" with
 * actual_stlh and remove earlier duplicates.  Keyword order is preserved.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
update_stlo_maxheight(char_u **varp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା actual_stlh)
{
    ଅଚଳ char_u    *src = *varp;
    ଅଚଳ char_u    *last_mh = ଖାଲି;
    ଅଚଳ char_u    *p;

    // Find the last "maxheight:" token.
    p = src;
    ଯେତେବେଳେ (*p != NUL)
    {
	ଯଦି (STRNCMP(p, "maxheight:", 10) == 0)
	    last_mh = p;
	ଯେତେବେଳେ (*p != ',' && *p != NUL)
	    ++p;
	ଯଦି (*p == ',')
	    ++p;
    }

    ଯଦି (last_mh == ଖାଲି)
	ଫେରନ୍ତୁ;

    size_t	bufsize = STRLEN(src) + NUMBUFLEN + 1;
    char_u	*buf = alloc(bufsize);
    ଯଦି (buf == ଖାଲି)
	ଫେରନ୍ତୁ;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len = 0;
    bool	need_comma = false;

    p = src;
    ଯେତେବେଳେ (*p != NUL)
    {
	ଅଚଳ	char_u	*tok = p;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	tok_len;

	ଯେତେବେଳେ (*p != ',' && *p != NUL)
	    ++p;
	tok_len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(p - tok);
	ଯଦି (*p == ',')
	    ++p;

	ଯଦି (STRNCMP(tok, "maxheight:", 10) == 0)
	{
	    ଯଦି (tok == last_mh)
	    {
		// Replace the last occurrence with the actual value.
		ଯଦି (need_comma)
		    buf[len++] = ',';
		len += vim_snprintf((ଅକ୍ଷର *)buf + len,
			bufsize - len, "maxheight:%d", actual_stlh);
		need_comma = true;
	    }
	}
	ଅନ୍ୟଥା
	{
	    ଯଦି (need_comma)
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
    ଅକ୍ଷର *
did_set_statuslineopt(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    win_T       *wp = varp == &curwin->w_p_stlo ? curwin : ଖାଲି;

    ଯଦି (statuslineopt_changed(*varp, wp) == FAIL)
	ଫେରନ୍ତୁ e_invalid_argument;

    // Sync stl_rendered_height with the current statusline.
    update_stl_rendered_height(wp);

    // Update the maxheight value to the actual value set.
    // Note: Must be changed if p_stlo_values are changed.
    ଯଦି (*varp != empty_option)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା actual_stlh;

	ଯଦି (wp != ଖାଲି)
	{
	    frame_change_statusline_height();
	    actual_stlh = MIN(wp->w_p_stlo_mh,
		    wp->w_height + wp->w_status_height - p_wmh);
	}
	ଅନ୍ୟଥା
	    actual_stlh = frame_change_statusline_height();

	// Only re-serialize when the window actually has a status line shown.
	ଯଦି (actual_stlh > 0)
	{
	    update_stlo_maxheight(varp, actual_stlh);
	    // Update the parsed maxheight member directly.
	    ଯଦି (wp != ଖାଲି)
		wp->w_p_stlo_mh = actual_stlh;
	    ଅନ୍ୟଥା
		set_stlo_mh(actual_stlh);
	}
    }
    ଅନ୍ୟଥା
	frame_change_statusline_height();

    ଯଦି ((args->os_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	// :set clears the local values.
	clear_string_option(&curwin->w_p_stlo);
	curwin->w_p_stlo_fh = -1;
	curwin->w_p_stlo_mh = -1;
    }

    ଫେରନ୍ତୁ ଖାଲି;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_set_statuslineopt(optexpand_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_Matches, char_u ***matches)
{
    ଫେରନ୍ତୁ expand_set_opt_string(
	    args,
	    p_stlo_ମୂଲ୍ୟଗୁଡ଼ିକ,
	    ARRAY_LENGTH(p_stlo_ମୂଲ୍ୟଗୁଡ଼ିକ) - 1,
	    ଅଙ୍କ_Matches,
	    matches);
}
#endif

/*
 * The 'swapsync' option is changed.
 */
    ଅକ୍ଷର *
did_set_swapsync(optset_T *args UNUSED)
{
    ଫେରନ୍ତୁ did_set_opt_strings(p_sws, p_sws_ମୂଲ୍ୟଗୁଡ଼ିକ, FALSE);
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_set_swapsync(optexpand_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_Matches, char_u ***matches)
{
    ଫେରନ୍ତୁ expand_set_opt_string(
	    args,
	    p_sws_ମୂଲ୍ୟଗୁଡ଼ିକ,
	    ARRAY_LENGTH(p_sws_ମୂଲ୍ୟଗୁଡ଼ିକ) - 1,
	    ଅଙ୍କ_Matches,
	    matches);
}

/*
 * The 'switchbuf' option is changed.
 */
    ଅକ୍ଷର *
did_set_switchbuf(optset_T *args UNUSED)
{
    ଫେରନ୍ତୁ did_set_opt_flags(p_swb, p_swb_ମୂଲ୍ୟଗୁଡ଼ିକ, &swb_flags, TRUE);
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_set_switchbuf(optexpand_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_Matches, char_u ***matches)
{
    ଫେରନ୍ତୁ expand_set_opt_string(
	    args,
	    p_swb_ମୂଲ୍ୟଗୁଡ଼ିକ,
	    ARRAY_LENGTH(p_swb_ମୂଲ୍ୟଗୁଡ଼ିକ) - 1,
	    ଅଙ୍କ_Matches,
	    matches);
}

/*
 * The 'tabclose' option is changed.
 */
    ଅକ୍ଷର *
did_set_tabclose(optset_T *args UNUSED)
{
    ଫେରନ୍ତୁ did_set_opt_flags(p_tcl, p_tcl_ମୂଲ୍ୟଗୁଡ଼ିକ, &tcl_flags, TRUE);
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_set_tabclose(optexpand_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_Matches, char_u ***matches)
{
    ଫେରନ୍ତୁ expand_set_opt_string(
	    args,
	    p_tcl_ମୂଲ୍ୟଗୁଡ଼ିକ,
	    ARRAY_LENGTH(p_tcl_ମୂଲ୍ୟଗୁଡ଼ିକ) - 1,
	    ଅଙ୍କ_Matches,
	    matches);
}

#ଯଦି defined(FEAT_STL_OPT)
/*
 * The 'tabline' option is changed.
 */
    ଅକ୍ଷର *
did_set_tabline(optset_T *args)
{
    ଫେରନ୍ତୁ parse_statustabline_rulerformat(args, FALSE);
}
#endif

/*
 * The 'tagcase' option is changed.
 */
    ଅକ୍ଷର *
did_set_tagcase(optset_T *args)
{
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା	*flags;
    char_u		*p;

    ଯଦି (args->os_flags & OPT_LOCAL)
    {
	p = curbuf->b_p_tc;
	flags = &curbuf->b_tc_flags;
    }
    ଅନ୍ୟଥା
    {
	p = p_tc;
	flags = &tc_flags;
    }

    ଯଦି ((args->os_flags & OPT_LOCAL) && *p == NUL)
	// make the local value empty: use the global value
	*flags = 0;
    ଅନ୍ୟଥା ଯଦି (*p == NUL
	    || opt_strings_flags(p, p_tc_ମୂଲ୍ୟଗୁଡ଼ିକ, flags, FALSE) != OK)
	ଫେରନ୍ତୁ e_invalid_argument;

    ଫେରନ୍ତୁ ଖାଲି;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_set_tagcase(optexpand_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_Matches, char_u ***matches)
{
    ଫେରନ୍ତୁ expand_set_opt_string(
	    args,
	    p_tc_ମୂଲ୍ୟଗୁଡ଼ିକ,
	    ARRAY_LENGTH(p_tc_ମୂଲ୍ୟଗୁଡ଼ିକ) - 1,
	    ଅଙ୍କ_Matches,
	    matches);
}

/*
 * The 'term' option is changed.
 */
    ଅକ୍ଷର *
did_set_term(optset_T *args UNUSED)
{
    ଯଦି (T_NAME[0] == NUL)
	ଫେରନ୍ତୁ e_cannot_set_term_to_empty_string;
#ifdef FEAT_GUI
    ଯଦି (gui.in_use)
	ଫେରନ୍ତୁ e_cannot_change_term_in_GUI;
    ଯଦି (term_is_gui(T_NAME))
	ଫେରନ୍ତୁ e_use_gui_to_start_GUI;
#endif
    ଯଦି (set_termname(T_NAME) == FAIL)
	ଫେରନ୍ତୁ e_not_found_in_termcap;

    // Screen colors may have changed.
    redraw_later_clear();

    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Some terminal option (t_xxx) is changed
 */
    ଅକ୍ଷର *
did_set_term_option(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    ଯଦି (!full_screen)
	ଫେରନ୍ତୁ ଖାଲି;

    // ":set t_Co=0" and ":set t_Co=1" do ":set t_Co="
    ଯଦି (varp == &T_CCO)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା colors = atoi((ଅକ୍ଷର *)T_CCO);

	// Only reinitialize colors if t_Co value has really changed to
	// avoid expensive reload of colorscheme if t_Co is set to the
	// same value multiple times.
	ଯଦି (colors != t_colors)
	{
	    t_colors = colors;
	    ଯଦି (t_colors <= 1)
	    {
		vim_free(T_CCO);
		T_CCO = empty_option;
	    }
#ଯଦି defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
	    ଯଦି (is_term_win32())
	    {
		swap_tcap();
		args->os_did_swaptcap = TRUE;
	    }
#endif
	    // We now have a different color setup, initialize it again.
	    init_highlight(TRUE, FALSE);
	}
    }
    ttest(FALSE);
    ଯଦି (varp == &T_ME)
    {
	out_str(T_ME);
	redraw_later(UPD_CLEAR);
#ଯଦି defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
	// Since t_me has been set, this probably means that the user
	// wants to use this as default colors.  Need to reset default
	// background/foreground colors.
# ifdef VIMDLL
	ଯଦି (!gui.in_use && !gui.starting)
# endif
	    mch_set_normal_colors();
#endif
    }
    ଯଦି (varp == &T_BE && termcap_active)
    {
	MAY_WANT_TO_LOG_THIS;

	ଯଦି (*T_BE == NUL)
	    // When clearing t_BE we assume the user no longer wants
	    // bracketed paste, thus disable it by writing t_BD.
	    out_str(T_BD);
	ଅନ୍ୟଥା
	    out_str(T_BE);
    }

    ଯଦି (varp == &T_BSU || varp == &T_ESU)
	term_set_sync_output(TERM_SYNC_OUTPUT_OFF);

    ଫେରନ୍ତୁ ଖାଲି;
}


#ifdef UNIX
/*
 * The 'termresize' option is changed.
 */
    ଅକ୍ଷର *
did_set_termresize(optset_T *args UNUSED)
{
    // If empty or "inband", then attempt to enable in-band resize events.
    ଯଦି (*p_trz == NUL || STRCMP(p_trz, "inband") == 0)
	term_set_win_resize(true);
    ଅନ୍ୟଥା ଯଦି (STRCMP(p_trz, "sigwinch") == 0)
	term_set_win_resize(false);
    ଅନ୍ୟଥା
	ଫେରନ୍ତୁ e_invalid_argument;

    ଫେରନ୍ତୁ ଖାଲି;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_set_termresize(optexpand_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_Matches, char_u ***matches)
{
    ଫେରନ୍ତୁ expand_set_opt_string(
	    args,
	    p_trz_ମୂଲ୍ୟଗୁଡ଼ିକ,
	    ARRAY_LENGTH(p_trz_ମୂଲ୍ୟଗୁଡ଼ିକ) - 1,
	    ଅଙ୍କ_Matches,
	    matches);
}
#endif

#ଯଦି defined(FEAT_TERMINAL)
/*
 * The 'termwinkey' option is changed.
 */
    ଅକ୍ଷର *
did_set_termwinkey(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    ଯଦି ((*varp)[0] != NUL && string_to_ଚାବି(*varp, TRUE) == 0)
	ଫେରନ୍ତୁ e_invalid_argument;

    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * The 'termwinsize' option is changed.
 */
    ଅକ୍ଷର *
did_set_termwinsize(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u	*p;

    ଯଦି ((*varp)[0] == NUL)
	ଫେରନ୍ତୁ ଖାଲି;

    p = skipdigits(*varp);
    ଯଦି (p == *varp || (*p != 'x' && *p != '*') || *skipdigits(p + 1) != NUL)
	ଫେରନ୍ତୁ e_invalid_argument;

    ଫେରନ୍ତୁ ଖାଲି;
}

# ଯଦି defined(MSWIN)
/*
 * The 'termwintype' option is changed.
 */
    ଅକ୍ଷର *
did_set_termwintype(optset_T *args UNUSED)
{
    ଫେରନ୍ତୁ did_set_opt_strings(p_twt, p_twt_ମୂଲ୍ୟଗୁଡ଼ିକ, FALSE);
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_set_termwintype(optexpand_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_Matches, char_u ***matches)
{
    ଫେରନ୍ତୁ expand_set_opt_string(
	    args,
	    p_twt_ମୂଲ୍ୟଗୁଡ଼ିକ,
	    ARRAY_LENGTH(p_twt_ମୂଲ୍ୟଗୁଡ଼ିକ) - 1,
	    ଅଙ୍କ_Matches,
	    matches);
}
# endif
#endif

/*
 * The 'titlestring' option is changed.
 */
    ଅକ୍ଷର *
did_set_titlestring(optset_T *args)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା flagval = 0;

#ifdef FEAT_STL_OPT
    flagval = STL_IN_TITLE;
#endif
    ଫେରନ୍ତୁ parse_titleiconstring(args, flagval);
}

#ଯଦି defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
/*
 * The 'toolbar' option is changed.
 */
    ଅକ୍ଷର *
did_set_toolbar(optset_T *args UNUSED)
{
    ଯଦି (opt_strings_flags(p_toolbar, p_toolbar_ମୂଲ୍ୟଗୁଡ଼ିକ, &toolbar_flags,
								TRUE) != OK)
	ଫେରନ୍ତୁ e_invalid_argument;

    out_flush();
    gui_mch_show_toolbar((toolbar_flags &
		(TOOLBAR_TEXT | TOOLBAR_ICONS)) != 0);
    ଫେରନ୍ତୁ ଖାଲି;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_set_toolbar(optexpand_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_Matches, char_u ***matches)
{
    ଫେରନ୍ତୁ expand_set_opt_string(
	    args,
	    p_toolbar_ମୂଲ୍ୟଗୁଡ଼ିକ,
	    ARRAY_LENGTH(p_toolbar_ମୂଲ୍ୟଗୁଡ଼ିକ) - 1,
	    ଅଙ୍କ_Matches,
	    matches);
}
#endif

#ଯଦି defined(FEAT_TOOLBAR) && defined(FEAT_GUI_GTK)
/*
 * The 'toolbariconsize' option is changed.  GTK+ 2 only.
 */
    ଅକ୍ଷର *
did_set_toolbariconsize(optset_T *args UNUSED)
{
    ଯଦି (opt_strings_flags(p_tbis, p_tbis_ମୂଲ୍ୟଗୁଡ଼ିକ, &tbis_flags, FALSE) != OK)
	ଫେରନ୍ତୁ e_invalid_argument;

    out_flush();
    gui_mch_show_toolbar((toolbar_flags &
		(TOOLBAR_TEXT | TOOLBAR_ICONS)) != 0);
    ଫେରନ୍ତୁ ଖାଲି;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_set_toolbariconsize(
    optexpand_T	*args,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*ଅଙ୍କ_Matches,
    char_u	***matches)
{
    ଫେରନ୍ତୁ expand_set_opt_string(
	    args,
	    p_tbis_ମୂଲ୍ୟଗୁଡ଼ିକ,
	    ARRAY_LENGTH(p_tbis_ମୂଲ୍ୟଗୁଡ଼ିକ) - 1,
	    ଅଙ୍କ_Matches,
	    matches);
}
#endif

#ଯଦି defined(UNIX) || defined(VMS)
/*
 * The 'ttymouse' option is changed.
 */
    ଅକ୍ଷର *
did_set_ttymouse(optset_T *args UNUSED)
{
    ଅକ୍ଷର *errmsg = ଖାଲି;

    // Switch the mouse off before changing the escape sequences used for
    // that.
    mch_setmouse(FALSE);
    ଯଦି (opt_strings_flags(p_ttym, p_ttym_ମୂଲ୍ୟଗୁଡ଼ିକ, &ttym_flags, FALSE) != OK)
	errmsg = e_invalid_argument;
    ଅନ୍ୟଥା
	check_mouse_termcode();
    ଯଦି (termcap_active)
	setmouse();		// may switch it on again

    ଫେରନ୍ତୁ errmsg;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_set_ttymouse(optexpand_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_Matches, char_u ***matches)
{
    ଫେରନ୍ତୁ expand_set_opt_string(
	    args,
	    p_ttym_ମୂଲ୍ୟଗୁଡ଼ିକ,
	    ARRAY_LENGTH(p_ttym_ମୂଲ୍ୟଗୁଡ଼ିକ) - 1,
	    ଅଙ୍କ_Matches,
	    matches);
}
#endif

#ଯଦି defined(FEAT_VARTABS)
/*
 * The 'varsofttabstop' option is changed.
 */
    ଅକ୍ଷର *
did_set_varsofttabstop(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u *cp;

    ଯଦି (!((*varp)[0]) || ((*varp)[0] == '0' && !((*varp)[1])))
	VIM_CLEAR(curbuf->b_p_vsts_array);
    ଅନ୍ୟଥା
    {
	ପାଇଁ (cp = *varp; *cp; ++cp)
	{
	    ଯଦି (vim_isdigit(*cp))
		ଜାରି;
	    ଯଦି (*cp == ',' && cp > *varp && *(cp-1) != ',')
		ଜାରି;
	    ଫେରନ୍ତୁ e_invalid_argument;
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା *oldarray = curbuf->b_p_vsts_array;
	ଯଦି (tabstop_set(*varp, &(curbuf->b_p_vsts_array)) == OK)
	{
	    ଯଦି (oldarray)
		vim_free(oldarray);
	}
	ଅନ୍ୟଥା
	    ଫେରନ୍ତୁ e_invalid_argument;
    }

    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * The 'vartabstop' option is changed.
 */
    ଅକ୍ଷର *
did_set_vartabstop(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u *cp;

    ଯଦି (!((*varp)[0]) || ((*varp)[0] == '0' && !((*varp)[1])))
	VIM_CLEAR(curbuf->b_p_vts_array);
    ଅନ୍ୟଥା
    {
	ପାଇଁ (cp = *varp; *cp; ++cp)
	{
	    ଯଦି (vim_isdigit(*cp))
		ଜାରି;
	    ଯଦି (*cp == ',' && cp > *varp && *(cp-1) != ',')
		ଜାରି;
	    ଫେରନ୍ତୁ e_invalid_argument;
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା *oldarray = curbuf->b_p_vts_array;

	ଯଦି (tabstop_set(*varp, &(curbuf->b_p_vts_array)) == OK)
	{
	    vim_free(oldarray);
# ifdef FEAT_FOLDING
	    ଯଦି (foldmethodIsIndent(curwin))
		foldUpdateAll(curwin);
# endif
	}
	ଅନ୍ୟଥା
	    ଫେରନ୍ତୁ e_invalid_argument;
    }

    ଫେରନ୍ତୁ ଖାଲି;
}
#endif

/*
 * The 'verbosefile' option is changed.
 */
    ଅକ୍ଷର *
did_set_verbosefile(optset_T *args UNUSED)
{
    verbose_stop();
    ଯଦି (*p_vfile != NUL && verbose_open() == FAIL)
	ଫେରନ୍ତୁ e_invalid_argument;

    ଫେରନ୍ତୁ ଖାଲି;
}

#ଯଦି defined(FEAT_SESSION)
/*
 * The 'viewoptions' option is changed.
 */
    ଅକ୍ଷର *
did_set_viewoptions(optset_T *args UNUSED)
{
    ଫେରନ୍ତୁ did_set_opt_flags(p_vop, p_ssop_ମୂଲ୍ୟଗୁଡ଼ିକ, &vop_flags, TRUE);
}
#endif

#ଯଦି defined(FEAT_VIMINFO)
/*
 * The 'viminfo' option is changed.
 */
    ଅକ୍ଷର *
did_set_viminfo(optset_T *args)
{
    char_u	*s;
    ଅକ୍ଷର	*errmsg = ଖାଲି;

    ପାଇଁ (s = p_viminfo; *s;)
    {
	// Check it's a valid character
	ଯଦି (vim_strchr((char_u *)"!\"%'/:<@cfhnrs", *s) == ଖାଲି)
	{
	    errmsg = illegal_char(args->os_errbuf, args->os_errbuflen, *s);
	    ଭାଙ୍ଗ;
	}
	ଯଦି (*s == 'n')	// name is always last one
	    ଭାଙ୍ଗ;
	ଅନ୍ୟଥା ଯଦି (*s == 'r') // skip until next ','
	{
	    ଯେତେବେଳେ (*++s && *s != ',')
		;
	}
	ଅନ୍ୟଥା ଯଦି (*s == '%')
	{
	    // optional number
	    ଯେତେବେଳେ (vim_isdigit(*++s))
		;
	}
	ଅନ୍ୟଥା ଯଦି (*s == '!' || *s == 'h' || *s == 'c')
	    ++s;		// no extra chars
	ଅନ୍ୟଥା		// must have a number
	{
	    ଯେତେବେଳେ (vim_isdigit(*++s))
		;

	    ଯଦି (!VIM_ISDIGIT(*(s - 1)))
	    {
		ଯଦି (args->os_errbuf != ଖାଲି)
		{
		    vim_snprintf(args->os_errbuf, args->os_errbuflen,
			    _(e_missing_ସଂଖ୍ୟା_after_angle_str_angle),
			    transchar_byte(*(s - 1)));
		    errmsg = args->os_errbuf;
		}
		ଅନ୍ୟଥା
		    errmsg = "";
		ଭାଙ୍ଗ;
	    }
	}
	ଯଦି (*s == ',')
	    ++s;
	ଅନ୍ୟଥା ଯଦି (*s)
	{
	    ଯଦି (args->os_errbuf != ଖାଲି)
		errmsg = e_missing_comma;
	    ଅନ୍ୟଥା
		errmsg = "";
	    ଭାଙ୍ଗ;
	}
    }
    ଯଦି (*p_viminfo && errmsg == ଖାଲି && get_viminfo_parameter('\'') < 0)
	errmsg = e_must_specify_a_ମୂଲ୍ୟ;

    ଫେରନ୍ତୁ errmsg;
}
#endif

/*
 * The 'virtualedit' option is changed.
 */
    ଅକ୍ଷର *
did_set_virtualedit(optset_T *args)
{
    char_u		*ve = p_ve;
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା	*flags = &ve_flags;

    ଯଦି (args->os_flags & OPT_LOCAL)
    {
	ve = curwin->w_p_ve;
	flags = &curwin->w_ve_flags;
    }

    ଯଦି ((args->os_flags & OPT_LOCAL) && *ve == NUL)
	// make the local value empty: use the global value
	*flags = 0;
    ଅନ୍ୟଥା
    {
	ଯଦି (opt_strings_flags(ve, p_ve_ମୂଲ୍ୟଗୁଡ଼ିକ, flags, TRUE) != OK)
	    ଫେରନ୍ତୁ e_invalid_argument;
	ଅନ୍ୟଥା ଯଦି (STRCMP(ve, args->os_oldval.string) != 0)
	{
	    // Recompute cursor position in case the new 've' setting
	    // changes something.
	    validate_virtcol();
	    coladvance(curwin->w_virtcol);
	}
    }

    ଫେରନ୍ତୁ ଖାଲି;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_set_virtualedit(optexpand_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_Matches, char_u ***matches)
{
    ଫେରନ୍ତୁ expand_set_opt_string(
	    args,
	    p_ve_ମୂଲ୍ୟଗୁଡ଼ିକ,
	    ARRAY_LENGTH(p_ve_ମୂଲ୍ୟଗୁଡ଼ିକ) - 1,
	    ଅଙ୍କ_Matches,
	    matches);
}

/*
 * The 'whichwrap' option is changed.
 */
    ଅକ୍ଷର *
did_set_whichwrap(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    // Add ',' to the list flags because 'whichwrap' is a flag
    // list that is comma-separated.
    ଫେରନ୍ତୁ did_set_option_listflag(*varp, (char_u *)(WW_ALL ","),
		    args->os_errbuf, args->os_errbuflen);
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_set_whichwrap(optexpand_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_Matches, char_u ***matches)
{
    ଫେରନ୍ତୁ expand_set_opt_listflag(args, (char_u*)WW_ALL, ଅଙ୍କ_Matches, matches);
}

/*
 * The 'wildmode' option is changed.
 */
    ଅକ୍ଷର *
did_set_wildmode(optset_T *args UNUSED)
{
    ଯଦି (check_opt_wim() == FAIL)
	ଫେରନ୍ତୁ e_invalid_argument;
    ଫେରନ୍ତୁ ଖାଲି;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_set_wildmode(optexpand_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_Matches, char_u ***matches)
{
    ଫେରନ୍ତୁ expand_set_opt_string(
	    args,
	    p_wim_ମୂଲ୍ୟଗୁଡ଼ିକ,
	    ARRAY_LENGTH(p_wim_ମୂଲ୍ୟଗୁଡ଼ିକ) - 1,
	    ଅଙ୍କ_Matches,
	    matches);
}

/*
 * The 'wildoptions' option is changed.
 */
    ଅକ୍ଷର *
did_set_wildoptions(optset_T *args UNUSED)
{
    ଫେରନ୍ତୁ did_set_opt_strings(p_wop, p_wop_ମୂଲ୍ୟଗୁଡ଼ିକ, TRUE);
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_set_wildoptions(optexpand_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_Matches, char_u ***matches)
{
    ଫେରନ୍ତୁ expand_set_opt_string(
	    args,
	    p_wop_ମୂଲ୍ୟଗୁଡ଼ିକ,
	    ARRAY_LENGTH(p_wop_ମୂଲ୍ୟଗୁଡ଼ିକ) - 1,
	    ଅଙ୍କ_Matches,
	    matches);
}

#ଯଦି defined(FEAT_WAK)
/*
 * The 'winaltkeys' option is changed.
 */
    ଅକ୍ଷର *
did_set_winaltkeys(optset_T *args UNUSED)
{
    ଅକ୍ଷର *errmsg = ଖାଲି;

    ଯଦି (*p_wak == NUL || check_opt_strings(p_wak, p_wak_ମୂଲ୍ୟଗୁଡ଼ିକ, FALSE) != OK)
	errmsg = e_invalid_argument;
# ifdef FEAT_MENU
#  ଯଦି defined(FEAT_GUI_MOTIF)
    ଅନ୍ୟଥା ଯଦି (gui.in_use)
	gui_motif_set_mnemonics(p_wak[0] == 'y' || p_wak[0] == 'm');
#  elif defined(FEAT_GUI_GTK)
    ଅନ୍ୟଥା ଯଦି (gui.in_use)
	gui_gtk_set_mnemonics(p_wak[0] == 'y' || p_wak[0] == 'm');
#  endif
# endif
    ଫେରନ୍ତୁ errmsg;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_set_winaltkeys(optexpand_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_Matches, char_u ***matches)
{
    ଫେରନ୍ତୁ expand_set_opt_string(
	    args,
	    p_wak_ମୂଲ୍ୟଗୁଡ଼ିକ,
	    ARRAY_LENGTH(p_wak_ମୂଲ୍ୟଗୁଡ଼ିକ) - 1,
	    ଅଙ୍କ_Matches,
	    matches);
}
#endif

/*
 * The 'wincolor' option is changed.
 */
    ଅକ୍ଷର *
did_set_wincolor(optset_T *args UNUSED)
{
    update_wincolor(curwin, args->os_newval.string);
    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * The 'winhighlight' option is changed
 */
    ଅକ୍ଷର *
did_set_winhighlight(optset_T *args)
{
    ଫେରନ୍ତୁ update_winhighlight(curwin, args->os_newval.string);
}

/*
 * Expand 'winhighlight' option.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_set_winhighlight(optexpand_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_Matches, char_u ***matches)
{
    expand_T	    *xp = args->oe_xp;

    ଯଦି ((xp->xp_pattern > args->oe_set_arg && *(xp->xp_pattern-1) == ':')
	    || xp->xp_pattern == args->oe_set_arg || *(xp->xp_pattern-1) == ',')
    {
	// After a ':' or after a ',', or at the start, so expand highlight
	// group name.

	// If starts with !, then expand 'highlight' occasions.
	ଯଦି (*xp->xp_pattern == '!')
	    ଫେରନ୍ତୁ expand_hl_occasions(args, ଅଙ୍କ_Matches, matches, '!');
	ଅନ୍ୟଥା
	    ଫେରନ୍ତୁ expand_set_opt_generic(
		    args,
		    get_highlight_name,
		    ଅଙ୍କ_Matches,
		    matches);
    }

    VIM_CLEAR(*matches);
    ଫେରନ୍ତୁ FAIL;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_set_wincolor(optexpand_T *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_Matches, char_u ***matches)
{
    ଫେରନ୍ତୁ expand_set_opt_generic(
	    args,
	    get_highlight_name,
	    ଅଙ୍କ_Matches,
	    matches);
}

#ifdef FEAT_SYN_HL
/*
 * When the 'syntax' option is set, load the syntax of that name.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
do_syntax_autocmd(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ମୂଲ୍ୟ_changed)
{
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା syn_recursive = 0;

    ++syn_recursive;
    // Only pass TRUE for "force" when the value changed or not used
    // recursively, to avoid endless recurrence.
    apply_autocmds(EVENT_SYNTAX, curbuf->b_p_syn, curbuf->b_fname,
	    ମୂଲ୍ୟ_changed || syn_recursive == 1, curbuf);
    curbuf->b_flags |= BF_SYN_SET;
    --syn_recursive;
}
#endif

/*
 * When the 'filetype' option is set, trigger the FileType autocommand.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
do_filetype_autocmd(char_u **varp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା opt_flags, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ମୂଲ୍ୟ_changed)
{
    // Skip this when called from a modeline and the filetype was already set
    // to this value.
    ଯଦି ((opt_flags & OPT_MODELINE) && !ମୂଲ୍ୟ_changed)
	ଫେରନ୍ତୁ;

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା  ft_recursive = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    secure_save = secure;

    // Reset the secure flag, since the value of 'filetype' has
    // been checked to be safe.
    secure = 0;

    ++ft_recursive;
    curbuf->b_did_filetype = true;
    // Only pass TRUE for "force" when the value changed or not
    // used recursively, to avoid endless recurrence.
    apply_autocmds(EVENT_FILETYPE, curbuf->b_p_ft, curbuf->b_fname,
	    ମୂଲ୍ୟ_changed || ft_recursive == 1, curbuf);
    --ft_recursive;
    // Just in case the old "curbuf" is now invalid.
    ଯଦି (varp != &(curbuf->b_p_ft))
	varp = ଖାଲି;

    secure = secure_save;
}

#ifdef FEAT_SPELL
/*
 * When the 'spelllang' option is set, source the spell/LANG.vim file in
 * 'runtimepath'.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
do_spelllang_source(ଶୂନ୍ୟ)
{
    char_u	fname[200];
    char_u	*p;
    char_u	*q = curwin->w_s->b_p_spl;

    // Skip the first name if it is "cjk".
    ଯଦି (STRNCMP(q, "cjk,", 4) == 0)
	q += 4;

    // They could set 'spellcapcheck' depending on the language.  Use the first
    // name in 'spelllang' up to '_region' or '.encoding'.
    ପାଇଁ (p = q; *p != NUL; ++p)
	ଯଦି (!ASCII_ISALNUM(*p) && *p != '-')
	    ଭାଙ୍ଗ;
    ଯଦି (p > q)
    {
	vim_snprintf((ଅକ୍ଷର *)fname, 200, "spell/%.*s.vim",
		(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(p - q), q);
	source_runtime(fname, DIP_ALL);
    }
}
#endif

/*
 * Handle string options that need some action to perform when changed.
 * The new value must be allocated.
 * Returns NULL for success, or an untranslated error message for an error.
 */
    ଅକ୍ଷର *
did_set_string_option(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		opt_idx,		// index in options[] table
    char_u	**varp,			// pointer to the option variable
    char_u	*oldval,		// previous value of the option
    char_u	*ମୂଲ୍ୟ,			// new value of the option
    ଅକ୍ଷର	*errbuf,		// buffer for errors, or NULL
    size_t	errbuflen,		// length of error buffer
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		opt_flags,		// OPT_LOCAL and/or OPT_GLOBAL
    set_op_T    op,			// OP_ADDING/OP_PREPENDING/OP_REMOVING
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*ମୂଲ୍ୟ_checked)		// value was checked to be safe, no
					// need to set P_INSECURE
{
    ଅକ୍ଷର	*errmsg = ଖାଲି;
    long_u	free_oldval = (get_option_flags(opt_idx) & P_ALLOCED);
    opt_did_set_cb_T did_set_cb = get_option_did_set_cb(opt_idx);
    optset_T	args;

    // 'ttytype' is an alias for 'term'.  Both 'term' and 'ttytype' point to
    // T_NAME.  If 'term' or 'ttytype' is modified, then use the index for the
    // 'term' option.  Only set the P_ALLOCED flag on 'term'.
    ଯଦି (varp == &T_NAME)
    {
	opt_idx = findoption((char_u *)"term");
	ଯଦି (opt_idx >= 0)
	{
	    free_oldval = (get_option_flags(opt_idx) & P_ALLOCED);
	    did_set_cb = get_option_did_set_cb(opt_idx);
	}
    }

    CLEAR_FIELD(args);

    // Disallow changing some options from secure mode
    ଯଦି ((secure
#ifdef HAVE_SANDBOX
		|| sandbox != 0
#endif
		) && (get_option_flags(opt_idx) & P_SECURE))
	errmsg = e_not_allowed_here;
    // Check for a "normal" directory or file name in some options.
    ଅନ୍ୟଥା ଯଦି (check_illegal_path_names(opt_idx, varp))
	errmsg = e_invalid_argument;
    ଅନ୍ୟଥା ଯଦି (did_set_cb != ଖାଲି)
    {
	args.os_varp = (char_u *)varp;
	args.os_idx = opt_idx;
	args.os_flags = opt_flags;
	args.os_op = op;
	args.os_oldval.string = oldval;
	args.os_newval.string = ମୂଲ୍ୟ;
	args.os_errbuf = errbuf;
	args.os_errbuflen = errbuflen;
	// Invoke the option specific callback function to validate and apply
	// the new option value.
	errmsg = did_set_cb(&args);

	// The 'keymap', 'filetype' and 'syntax' option callback functions
	// may change the os_value_checked field.
	*ମୂଲ୍ୟ_checked = args.os_ମୂଲ୍ୟ_checked;
    }

    // If an error is detected, restore the previous value.
    ଯଦି (errmsg != ଖାଲି)
    {
	free_string_option(*varp);
	*varp = oldval;
	// When resetting some values, need to act on it.
	ଯଦି (args.os_restore_chartab)
	    (ଶୂନ୍ୟ)init_chartab();
	ଯଦି (varp == &p_hl)
	    (ଶୂନ୍ୟ)highlight_changed();
    }
    ଅନ୍ୟଥା
    {
#ifdef FEAT_EVAL
	// Remember where the option was set.
	set_option_sctx_idx(opt_idx, opt_flags, current_sctx);
#endif
	// Free string options that are in allocated memory.
	// Use "free_oldval", because recursiveness may change the flags under
	// our fingers (esp. init_highlight()).
	ଯଦି (free_oldval)
	    free_string_option(oldval);
	set_option_flag(opt_idx, P_ALLOCED);

	ଯଦି ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0
		&& is_global_local_option(opt_idx))
	{
	    // global option with local value set to use global value; free
	    // the local value and make it empty
	    char_u *p = get_option_varp_scope(opt_idx, OPT_LOCAL);
	    free_string_option(*(char_u **)p);
	    *(char_u **)p = empty_option;
	}

	// May set global value for local option.
	ଅନ୍ୟଥା ଯଦି (!(opt_flags & OPT_LOCAL) && opt_flags != OPT_GLOBAL)
	    set_string_option_global(opt_idx, varp);

	// Trigger the autocommand only after setting the flags.
#ifdef FEAT_SYN_HL
	ଯଦି (varp == &(curbuf->b_p_syn))
	    do_syntax_autocmd(args.os_ମୂଲ୍ୟ_changed);
#endif
	ଅନ୍ୟଥା ଯଦି (varp == &(curbuf->b_p_ft))
	    do_filetype_autocmd(varp, opt_flags, args.os_ମୂଲ୍ୟ_changed);
#ifdef FEAT_SPELL
	ଯଦି (varp == &(curwin->w_s->b_p_spl))
	    do_spelllang_source();
#endif
    }

    ଯଦି (varp == &p_mouse)
    {
	ଯଦି (*p_mouse == NUL)
	    mch_setmouse(FALSE);    // switch mouse off
	ଅନ୍ୟଥା
	    setmouse();		    // in case 'mouse' changed
    }

    ଯଦି (varp == &p_rtp)
    {
	export_myvimdir();
#ଯଦି defined(FEAT_LUA)
	update_package_paths_in_lua();
#endif
    }

#ଯଦି defined(FEAT_LINEBREAK)
    // Changing Formatlistpattern when briopt includes the list setting:
    // redraw
    ଯଦି ((varp == &p_flp || varp == &(curbuf->b_p_flp))
	    && curwin->w_briopt_list)
	redraw_all_later(UPD_NOT_VALID);
#endif

    ଯଦି (curwin->w_curswant != MAXCOL
		   && (get_option_flags(opt_idx) & (P_CURSWANT | P_RALL)) != 0
				&& (get_option_flags(opt_idx) & P_HLONLY) == 0)
	curwin->w_set_curswant = true;

    ଯଦି ((opt_flags & OPT_NO_REDRAW) == 0)
    {
#ifdef FEAT_GUI
	// set when changing an option that only requires a redraw in the GUI
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	redraw_gui_only = FALSE;

	ଯଦି (varp == &p_go			// 'guioptions'
		|| varp == &p_guifont		// 'guifont'
# ifdef FEAT_GUI_TABLINE
		|| varp == &p_gtl		// 'guitablabel'
		|| varp == &p_gtt		// 'guitabtooltip'
# endif
# ifdef FEAT_XFONTSET
		|| varp == &p_guifontset	// 'guifontset'
# endif
		|| varp == &p_guifontwide	// 'guifontwide'
# ଯଦି defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
		|| varp == &p_guiligatures	// 'guiligatures'
# endif
	    )
	    redraw_gui_only = TRUE;

	// check redraw when it's not a GUI option or the GUI is active.
	ଯଦି (!redraw_gui_only || gui.in_use)
#endif
	    check_redraw(get_option_flags(opt_idx));
    }

#ଯଦି defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
    ଯଦି (args.os_did_swaptcap)
    {
	set_termname((char_u *)"win32");
	init_highlight(TRUE, FALSE);
    }
#endif

    ଫେରନ୍ତୁ errmsg;
}

/*
 * Check an option that can be a range of string values.
 *
 * Return OK for correct value, FAIL otherwise.
 * Empty is always OK.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_opt_strings(
    char_u	*val,
    ଅକ୍ଷର	**ମୂଲ୍ୟଗୁଡ଼ିକ,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		list)	    // when TRUE: accept a list of values
{
    ଫେରନ୍ତୁ opt_strings_flags(val, ମୂଲ୍ୟଗୁଡ଼ିକ, ଖାଲି, list);
}

/*
 * Handle an option that can be a range of string values.
 * Set a flag in "*flagp" for each string present.
 *
 * Return OK for correct value, FAIL otherwise.
 * Empty is always OK.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
opt_strings_flags(
    char_u	*val,		// new value
    ଅକ୍ଷର	**ମୂଲ୍ୟଗୁଡ଼ିକ,	// array of valid string values
    unsigned	*flagp,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		list)		// when TRUE: accept a list of values
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
    unsigned	new_flags = 0;

    ଯେତେବେଳେ (*val)
    {
	ପାଇଁ (i = 0; ; ++i)
	{
	    ଯଦି (ମୂଲ୍ୟଗୁଡ଼ିକ[i] == ଖାଲି)	// val not found in values[]
		ଫେରନ୍ତୁ FAIL;

	    len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(ମୂଲ୍ୟଗୁଡ଼ିକ[i]);
	    ଯଦି (STRNCMP(ମୂଲ୍ୟଗୁଡ଼ିକ[i], val, len) == 0
		    && ((list && val[len] == ',') || val[len] == NUL))
	    {
		val += len + (val[len] == ',');
		new_flags |= (1 << i);
		ଭାଙ୍ଗ;		// check next item in val list
	    }
	}
    }
    ଯଦି (flagp != ଖାଲି)
	*flagp = new_flags;

    ଫେରନ୍ତୁ OK;
}

/*
 * return OK if "p" is a valid fileformat name, FAIL otherwise.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_ff_ମୂଲ୍ୟ(char_u *p)
{
    ଫେରନ୍ତୁ check_opt_strings(p, p_ff_ମୂଲ୍ୟଗୁଡ଼ିକ, FALSE);
}

/*
 * Save the actual shortmess Flags and clear them temporarily to avoid that
 * file messages overwrites any output from the following commands.
 *
 * Caller must make sure to first call save_clear_shm_value() and then
 * restore_shm_value() exactly the same number of times.
 */
    ଶୂନ୍ୟ
save_clear_shm_ମୂଲ୍ୟ(ଶୂନ୍ୟ)
{
    ଯଦି (STRLEN(p_shm) >= SHM_LEN)
    {
	iemsg(e_internal_error_shortmess_too_long);
	ଫେରନ୍ତୁ;
    }

    ଯଦି (++set_shm_recursive == 1)
    {
	STRCPY(shm_buf, p_shm);
	set_option_ମୂଲ୍ୟ_give_err((char_u *)"shm", 0L, (char_u *)"", 0);
    }
}

/*
 * Restore the shortmess Flags set from the save_clear_shm_value() function.
 */
    ଶୂନ୍ୟ
restore_shm_ମୂଲ୍ୟ(ଶୂନ୍ୟ)
{
    ଯଦି (--set_shm_recursive == 0)
    {
	set_option_ମୂଲ୍ୟ_give_err((char_u *)"shm", 0L, shm_buf, 0);
	vim_memset(shm_buf, 0, SHM_LEN);
    }
}

/*
 * Export the environment variable $MYVIMDIR to the first item in runtimepath
 */
    ଶୂନ୍ୟ
export_myvimdir(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dofree = FALSE;
    char_u	*p;
    char_u	*q = p_rtp;
    string_T	buf;

    buf.string = alloc(MAXPATHL);
    ଯଦି (buf.string == ଖାଲି)
	ଫେରନ୍ତୁ;

    buf.length = (size_t)copy_option_part(&q, buf.string, MAXPATHL, ",");

    p = vim_getenv((char_u *)"MYVIMDIR", &dofree);

    ଯଦି (p == ଖାଲି || STRCMP(p, buf.string) != 0)
    {
	ଯଦି (*buf.string != NUL && !after_pathsep(buf.string, buf.string + buf.length))
	    STRCPY(buf.string + buf.length, PATHSEPSTR);
#ifdef MSWIN
	// normalize path separators
	ପାଇଁ (q = buf.string; *q != NUL; q++)
	    ଯଦି (*q == '/')
		*q = '\\';
#endif
	vim_setenv((char_u *)"MYVIMDIR", buf.string);
    }
    ଯଦି (dofree)
	vim_free(p);
    vim_free(buf.string);
}
