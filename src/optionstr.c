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

#ಅನ್ತರ್ಭಾವ "vim.h"

ಸ್ಥಿರ char_u shm_buf[SHM_LEN];
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ set_shm_recursive = 0;

ಸ್ಥಿರ ವರ್ಣ *(p_ambw_ಮೌಲ್ಯಗಳು[]) = {"single", "double", ರಿಕ್ತ};
ಸ್ಥಿರ ವರ್ಣ *(p_bg_ಮೌಲ್ಯಗಳು[]) = {"light", "dark", ರಿಕ್ತ};
ಸ್ಥಿರ ವರ್ಣ *(p_bkc_ಮೌಲ್ಯಗಳು[]) = {"yes", "auto", "no", "breaksymlink", "breakhardlink", ರಿಕ್ತ};
ಸ್ಥಿರ ವರ್ಣ *(p_bo_ಮೌಲ್ಯಗಳು[]) = {"all", "backspace", "cursor", "complete",
				 "copy", "ctrlg", "error", "esc", "ex",
				 "hangul", "insertmode", "lang", "mess",
				 "showmatch", "operator", "register", "shell",
				 "spell", "term", "wildmode", ರಿಕ್ತ};
#ಯದಿ defined(FEAT_LINEBREAK)
// Note: Keep this in sync with briopt_check()
ಸ್ಥಿರ ವರ್ಣ *(p_briopt_ಮೌಲ್ಯಗಳು[]) = {"shift:", "min:", "sbr", "list:", "column:", ರಿಕ್ತ};
#endif
#ಯದಿ defined(FEAT_TABPANEL)
// Note: Keep this in sync with tabpanelopt_changed()
ಸ್ಥಿರ ವರ್ಣ *(p_tplo_ಮೌಲ್ಯಗಳು[]) = {"align:", "columns:", "scrollbar", "vert", ರಿಕ್ತ};
ಸ್ಥಿರ ವರ್ಣ *(p_tplo_align_ಮೌಲ್ಯಗಳು[]) = {"left", "right", ರಿಕ್ತ};
#endif
#ಯದಿ defined(FEAT_DIFF)
// Note: Keep this in sync with diffopt_changed()
ಸ್ಥಿರ ವರ್ಣ *(p_dip_ಮೌಲ್ಯಗಳು[]) = {"filler", "anchor", "context:", "iblank", "icase", "iwhite", "iwhiteall", "iwhiteeol", "horizontal", "vertical", "closeoff", "hiddenoff", "foldcolumn:", "followwrap", "internal", "indent-heuristic", "algorithm:", "inline:", "linematch:", ರಿಕ್ತ};
ಸ್ಥಿರ ವರ್ಣ *(p_dip_algorithm_ಮೌಲ್ಯಗಳು[]) = {"myers", "minimal", "patience", "histogram", ರಿಕ್ತ};
ಸ್ಥಿರ ವರ್ಣ *(p_dip_inline_ಮೌಲ್ಯಗಳು[]) = {"none", "simple", "char", "word", ರಿಕ್ತ};
#endif
ಸ್ಥಿರ ವರ್ಣ *(p_nf_ಮೌಲ್ಯಗಳು[]) = {"bin", "octal", "hex", "alpha", "unsigned", "blank", ರಿಕ್ತ};
ಸ್ಥಿರ ವರ್ಣ *(p_ff_ಮೌಲ್ಯಗಳು[]) = {FF_UNIX, FF_DOS, FF_MAC, ರಿಕ್ತ};
#ifdef HAVE_CLIPMETHOD
// Note: Keep this in sync with did_set_clipboard()
ಸ್ಥಿರ ವರ್ಣ *(p_cb_ಮೌಲ್ಯಗಳು[]) = {"unnamed", "unnamedplus",
# ifdef FEAT_CLIPBOARD
    "autoselect", "autoselectplus", "autoselectml", "html", "exclude:",
# endif
    ರಿಕ್ತ};
#endif
#ifdef FEAT_CRYPT
ಸ್ಥಿರ ವರ್ಣ *(p_cm_ಮೌಲ್ಯಗಳು[]) = {"zip", "blowfish", "blowfish2",
# ifdef FEAT_SODIUM
    "xchacha20", "xchacha20v2",
# endif
    ರಿಕ್ತ};
#endif
ಸ್ಥಿರ ವರ್ಣ *(p_cmp_ಮೌಲ್ಯಗಳು[]) = {"internal", "keepascii", ರಿಕ್ತ};
#ifdef FEAT_SYN_HL
// Note: Keep this in sync with fill_culopt_flags()
ಸ್ಥಿರ ವರ್ಣ *(p_culopt_ಮೌಲ್ಯಗಳು[]) = {"line", "screenline", "number", "both", ರಿಕ್ತ};
#endif
ಸ್ಥಿರ ವರ್ಣ *(p_dy_ಮೌಲ್ಯಗಳು[]) = {"lastline", "truncate", "uhex", ರಿಕ್ತ};
ಸ್ಥಿರ ವರ್ಣ *(p_jop_ಮೌಲ್ಯಗಳು[]) = {"stack", ರಿಕ್ತ};
#ifdef FEAT_FOLDING
ಸ್ಥಿರ ವರ್ಣ *(p_fdo_ಮೌಲ್ಯಗಳು[]) = {"all", "block", "hor", "mark", "percent",
				 "quickfix", "search", "tag", "insert",
				 "undo", "jump", ರಿಕ್ತ};
#endif
// Note: Keep this in sync with match_keyprotocol()
ಸ್ಥಿರ ವರ್ಣ *(p_kpc_protocol_ಮೌಲ್ಯಗಳು[]) = {"none", "mok2", "kitty", ರಿಕ್ತ};
#ifdef FEAT_IMAGE
// Note: Keep this in sync with match_imageprotocol()
ಸ್ಥಿರ ವರ್ಣ *(p_ipc_protocol_ಮೌಲ್ಯಗಳು[]) = {"none", "kitty", "sixel", ರಿಕ್ತ};
#endif
#ifdef FEAT_PROP_POPUP
// Note: Keep this in sync with parse_popup_option()
ಸ್ಥಿರ ವರ್ಣ *(p_popup_cpp_option_ಮೌಲ್ಯಗಳು[]) = {"align:", "border:",
    "borderhighlight:", "close:", "height:", "highlight:", "opacity:",
    "resize:", "shadow:", "width:", ರಿಕ್ತ};
ಸ್ಥಿರ ವರ್ಣ *(p_popup_pvp_option_ಮೌಲ್ಯಗಳು[]) = {"border:",
    "borderhighlight:", "close:", "height:", "highlight:", "opacity:",
    "resize:", "shadow:", "width:", ರಿಕ್ತ};
ಸ್ಥಿರ ವರ್ಣ *(p_popup_option_on_off_ಮೌಲ್ಯಗಳು[]) = {"on", "off", ರಿಕ್ತ};
ಸ್ಥಿರ ವರ್ಣ *(p_popup_cpp_border_ಮೌಲ್ಯಗಳು[]) = {"single", "double", "round",
    "ascii", "on", "off", "custom:", ರಿಕ್ತ};
ಸ್ಥಿರ ವರ್ಣ *(p_popup_pvp_border_ಮೌಲ್ಯಗಳು[]) = {"single", "double", "round",
    "ascii", "on", "off", "custom:", ರಿಕ್ತ};
ಸ್ಥಿರ ವರ್ಣ *(p_popup_option_align_ಮೌಲ್ಯಗಳು[]) = {"item", "menu", ರಿಕ್ತ};
#endif
#ಯದಿ defined(FEAT_SPELL)
// Note: Keep this in sync with spell_check_sps()
ಸ್ಥಿರ ವರ್ಣ *(p_sps_ಮೌಲ್ಯಗಳು[]) = {"best", "fast", "double", "expr:", "file:", "timeout:", ರಿಕ್ತ};
#endif
#ifdef FEAT_SESSION
// Also used for 'viewoptions'!  Keep in sync with SSOP_ flags.
ಸ್ಥಿರ ವರ್ಣ *(p_ssop_ಮೌಲ್ಯಗಳು[]) = {"buffers", "winpos", "resize", "winsize",
    "localoptions", "options", "help", "blank", "globals", "slash", "unix",
    "sesdir", "curdir", "folds", "cursor", "tabpages", "terminal", "skiprtp",
    ರಿಕ್ತ};
#endif
#ಯದಿ defined(FEAT_STL_OPT)
ಸ್ಥಿರ ವರ್ಣ *(p_stlo_ಮೌಲ್ಯಗಳು[]) = {"fixedheight", "maxheight:", ರಿಕ್ತ};
#endif
// Keep in sync with SWB_ flags in option.h
ಸ್ಥಿರ ವರ್ಣ *(p_swb_ಮೌಲ್ಯಗಳು[]) = {"useopen", "usetab", "split", "newtab", "vsplit", "uselast", ರಿಕ್ತ};
ಸ್ಥಿರ ವರ್ಣ *(p_spk_ಮೌಲ್ಯಗಳು[]) = {"cursor", "screen", "topline", ರಿಕ್ತ};
ಸ್ಥಿರ ವರ್ಣ *(p_tc_ಮೌಲ್ಯಗಳು[]) = {"followic", "ignore", "match", "followscs", "smart", ರಿಕ್ತ};
// Keep in sync with TCL_ flags in option.h
ಸ್ಥಿರ ವರ್ಣ *(p_tcl_ಮೌಲ್ಯಗಳು[]) = {"left", "uselast", ರಿಕ್ತ};
#ಯದಿ defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
ಸ್ಥಿರ ವರ್ಣ *(p_toolbar_ಮೌಲ್ಯಗಳು[]) = {"text", "icons", "tooltips", "horiz", ರಿಕ್ತ};
#endif
#ಯದಿ defined(FEAT_TOOLBAR) && defined(FEAT_GUI_GTK)
ಸ್ಥಿರ ವರ್ಣ *(p_tbis_ಮೌಲ್ಯಗಳು[]) = {"tiny", "small", "medium", "large", "huge", "giant", ರಿಕ್ತ};
#endif
#ಯದಿ defined(UNIX) || defined(VMS)
ಸ್ಥಿರ ವರ್ಣ *(p_ttym_ಮೌಲ್ಯಗಳು[]) = {"xterm", "xterm2", "dec", "netterm", "jsbterm", "pterm", "urxvt", "sgr", ರಿಕ್ತ};
#endif
ಸ್ಥಿರ ವರ್ಣ *(p_ve_ಮೌಲ್ಯಗಳು[]) = {"block", "insert", "all", "onemore", "none", "NONE", ರಿಕ್ತ};
// Note: Keep this in sync with check_opt_wim()
ಸ್ಥಿರ ವರ್ಣ *(p_wim_ಮೌಲ್ಯಗಳು[]) = {"full", "longest", "list", "lastused", "noselect", "noinsert", ರಿಕ್ತ};
ಸ್ಥಿರ ವರ್ಣ *(p_wop_ಮೌಲ್ಯಗಳು[]) = {"fuzzy", "tagfile", "pum", "exacttext", ರಿಕ್ತ};
#ifdef FEAT_WAK
ಸ್ಥಿರ ವರ್ಣ *(p_wak_ಮೌಲ್ಯಗಳು[]) = {"yes", "menu", "no", ರಿಕ್ತ};
#endif
ಸ್ಥಿರ ವರ್ಣ *(p_mousem_ಮೌಲ್ಯಗಳು[]) = {"extend", "popup", "popup_setpos", "mac", ರಿಕ್ತ};
ಸ್ಥಿರ ವರ್ಣ *(p_sel_ಮೌಲ್ಯಗಳು[]) = {"inclusive", "exclusive", "old", ರಿಕ್ತ};
ಸ್ಥಿರ ವರ್ಣ *(p_slm_ಮೌಲ್ಯಗಳು[]) = {"mouse", "key", "cmd", ರಿಕ್ತ};
ಸ್ಥಿರ ವರ್ಣ *(p_km_ಮೌಲ್ಯಗಳು[]) = {"startsel", "stopsel", ರಿಕ್ತ};
#ifdef FEAT_BROWSE
ಸ್ಥಿರ ವರ್ಣ *(p_bsdir_ಮೌಲ್ಯಗಳು[]) = {"current", "last", "buffer", ರಿಕ್ತ};
#endif
ಸ್ಥಿರ ವರ್ಣ *(p_scbopt_ಮೌಲ್ಯಗಳು[]) = {"ver", "hor", "jump", ರಿಕ್ತ};
ಸ್ಥಿರ ವರ್ಣ *(p_debug_ಮೌಲ್ಯಗಳು[]) = {"msg", "throw", "beep", ರಿಕ್ತ};
ಸ್ಥಿರ ವರ್ಣ *(p_ead_ಮೌಲ್ಯಗಳು[]) = {"both", "ver", "hor", ರಿಕ್ತ};
ಸ್ಥಿರ ವರ್ಣ *(p_buftype_ಮೌಲ್ಯಗಳು[]) = {"nofile", "nowrite", "quickfix", "help", "terminal", "acwrite", "prompt", "popup", ರಿಕ್ತ};
ಸ್ಥಿರ ವರ್ಣ *(p_bufhidden_ಮೌಲ್ಯಗಳು[]) = {"hide", "unload", "delete", "wipe", ರಿಕ್ತ};
ಸ್ಥಿರ ವರ್ಣ *(p_bs_ಮೌಲ್ಯಗಳು[]) = {"indent", "eol", "start", "nostop", ರಿಕ್ತ};
#ifdef FEAT_FOLDING
ಸ್ಥಿರ ವರ್ಣ *(p_fdm_ಮೌಲ್ಯಗಳು[]) = {"manual", "expr", "marker", "indent", "syntax",
# ifdef FEAT_DIFF
				"diff",
# endif
				ರಿಕ್ತ};
ಸ್ಥಿರ ವರ್ಣ *(p_fcl_ಮೌಲ್ಯಗಳು[]) = {"all", ರಿಕ್ತ};
#endif
ಸ್ಥಿರ ವರ್ಣ *(p_cfc_ಮೌಲ್ಯಗಳು[]) = {"keyword", "files", "whole_line", ರಿಕ್ತ};
ಸ್ಥಿರ ವರ್ಣ *(p_cot_ಮೌಲ್ಯಗಳು[]) = {"menu", "menuone", "longest", "preview", "popup", "popuphidden", "noinsert", "noselect", "fuzzy", "nosort", "preinsert", "nearest", ರಿಕ್ತ};
#ifdef BACKSLASH_IN_FILENAME
ಸ್ಥಿರ ವರ್ಣ *(p_csl_ಮೌಲ್ಯಗಳು[]) = {"slash", "backslash", ರಿಕ್ತ};
#endif
#ifdef FEAT_SIGNS
ಸ್ಥಿರ ವರ್ಣ *(p_scl_ಮೌಲ್ಯಗಳು[]) = {"yes", "no", "auto", "number", ರಿಕ್ತ};
#endif
#ifdef UNIX
ಸ್ಥಿರ ವರ್ಣ *(p_trz_ಮೌಲ್ಯಗಳು[]) = {"inband", "sigwinch", "", ರಿಕ್ತ};
#endif
#ಯದಿ defined(MSWIN) && defined(FEAT_TERMINAL)
ಸ್ಥಿರ ವರ್ಣ *(p_twt_ಮೌಲ್ಯಗಳು[]) = {"winpty", "conpty", "", ರಿಕ್ತ};
#endif
ಸ್ಥಿರ ವರ್ಣ *(p_sloc_ಮೌಲ್ಯಗಳು[]) = {"last", "statusline", "tabline", ರಿಕ್ತ};
ಸ್ಥಿರ ವರ್ಣ *(p_sws_ಮೌಲ್ಯಗಳು[]) = {"fsync", "sync", ರಿಕ್ತ};

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ check_opt_strings(char_u *val, ವರ್ಣ **ಮೌಲ್ಯಗಳು, ಪೂರ್ಣಾಙ್ಕ list);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ opt_strings_flags(char_u *val, ವರ್ಣ **ಮೌಲ್ಯಗಳು, unsigned *flagp, ಪೂರ್ಣಾಙ್ಕ list);

/*
 * After setting various option values: recompute variables that depend on
 * option values.
 */
    ಶೂನ್ಯ
didset_string_options(ಶೂನ್ಯ)
{
    (ಶೂನ್ಯ)opt_strings_flags(p_cmp, p_cmp_ಮೌಲ್ಯಗಳು, &cmp_flags, TRUE);
    (ಶೂನ್ಯ)opt_strings_flags(p_bkc, p_bkc_ಮೌಲ್ಯಗಳು, &bkc_flags, TRUE);
    (ಶೂನ್ಯ)opt_strings_flags(p_bo, p_bo_ಮೌಲ್ಯಗಳು, &bo_flags, TRUE);
    (ಶೂನ್ಯ)opt_strings_flags(p_cot, p_cot_ಮೌಲ್ಯಗಳು, &cot_flags, TRUE);
#ifdef FEAT_SESSION
    (ಶೂನ್ಯ)opt_strings_flags(p_ssop, p_ssop_ಮೌಲ್ಯಗಳು, &ssop_flags, TRUE);
    (ಶೂನ್ಯ)opt_strings_flags(p_vop, p_ssop_ಮೌಲ್ಯಗಳು, &vop_flags, TRUE);
#endif
#ifdef FEAT_FOLDING
    (ಶೂನ್ಯ)opt_strings_flags(p_fdo, p_fdo_ಮೌಲ್ಯಗಳು, &fdo_flags, TRUE);
#endif
    (ಶೂನ್ಯ)opt_strings_flags(p_dy, p_dy_ಮೌಲ್ಯಗಳು, &dy_flags, TRUE);
    (ಶೂನ್ಯ)opt_strings_flags(p_jop, p_jop_ಮೌಲ್ಯಗಳು, &jop_flags, TRUE);
    (ಶೂನ್ಯ)opt_strings_flags(p_tc, p_tc_ಮೌಲ್ಯಗಳು, &tc_flags, FALSE);
    (ಶೂನ್ಯ)opt_strings_flags(p_ve, p_ve_ಮೌಲ್ಯಗಳು, &ve_flags, TRUE);
#ಯದಿ defined(UNIX) || defined(VMS)
    (ಶೂನ್ಯ)opt_strings_flags(p_ttym, p_ttym_ಮೌಲ್ಯಗಳು, &ttym_flags, FALSE);
#endif
#ಯದಿ defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
    (ಶೂನ್ಯ)opt_strings_flags(p_toolbar, p_toolbar_ಮೌಲ್ಯಗಳು, &toolbar_flags, TRUE);
#endif
#ಯದಿ defined(FEAT_TOOLBAR) && defined(FEAT_GUI_GTK)
    (ಶೂನ್ಯ)opt_strings_flags(p_tbis, p_tbis_ಮೌಲ್ಯಗಳು, &tbis_flags, FALSE);
#endif
    (ಶೂನ್ಯ)opt_strings_flags(p_swb, p_swb_ಮೌಲ್ಯಗಳು, &swb_flags, TRUE);
    (ಶೂನ್ಯ)opt_strings_flags(p_tcl, p_tcl_ಮೌಲ್ಯಗಳು, &tcl_flags, TRUE);
}

#ಯದಿ defined(FEAT_EVAL)
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
    ಶೂನ್ಯ
trigger_optionset_string(
	ಪೂರ್ಣಾಙ್ಕ	opt_idx,
	ಪೂರ್ಣಾಙ್ಕ	opt_flags,
	char_u  *oldval,
	char_u  *oldval_l,
	char_u  *oldval_g,
	char_u  *newval)
{
    // Don't do this recursively.
    ಯದಿ (oldval == ರಿಕ್ತ || newval == ರಿಕ್ತ
				    || *get_vim_var_str(VV_OPTION_TYPE) != NUL)
	ಪ್ರತ್ಯಾಗಮ;

    char_u buf_type[7];
    size_t buf_typelen;
    size_t oldvallen;

    oldvallen = STRLEN(oldval);
    set_vim_var_string(VV_OPTION_OLD, oldval, (ಪೂರ್ಣಾಙ್ಕ)oldvallen);
    set_vim_var_string(VV_OPTION_NEW, newval, -1);
    buf_typelen = vim_snprintf_safelen((ವರ್ಣ *)buf_type, ಪರಿಮಾಣ(buf_type),
	"%s", (opt_flags & OPT_LOCAL) ? "local" : "global");
    set_vim_var_string(VV_OPTION_TYPE, buf_type, (ಪೂರ್ಣಾಙ್ಕ)buf_typelen);
    ಯದಿ (opt_flags & OPT_LOCAL)
    {
	set_vim_var_string(VV_OPTION_COMMAND, (char_u *)"setlocal", STRLEN_LITERAL("setlocal"));
	set_vim_var_string(VV_OPTION_OLDLOCAL, oldval, (ಪೂರ್ಣಾಙ್ಕ)oldvallen);
    }
    ಯದಿ (opt_flags & OPT_GLOBAL)
    {
	set_vim_var_string(VV_OPTION_COMMAND, (char_u *)"setglobal", STRLEN_LITERAL("setglobal"));
	set_vim_var_string(VV_OPTION_OLDGLOBAL, oldval, (ಪೂರ್ಣಾಙ್ಕ)oldvallen);
    }
    ಯದಿ ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	set_vim_var_string(VV_OPTION_COMMAND, (char_u *)"set", STRLEN_LITERAL("set"));
	set_vim_var_string(VV_OPTION_OLDLOCAL, oldval_l, -1);
	set_vim_var_string(VV_OPTION_OLDGLOBAL, oldval_g, -1);
    }
    ಯದಿ (opt_flags & OPT_MODELINE)
    {
	set_vim_var_string(VV_OPTION_COMMAND, (char_u *)"modeline", STRLEN_LITERAL("modeline"));
	set_vim_var_string(VV_OPTION_OLDLOCAL, oldval, (ಪೂರ್ಣಾಙ್ಕ)oldvallen);
    }
    apply_autocmds(EVENT_OPTIONSET,
	    get_option_fullname(opt_idx), ರಿಕ್ತ, FALSE,
	    ರಿಕ್ತ);
    reset_v_option_vars();
}
#endif

    ಸ್ಥಿರ ವರ್ಣ *
illegal_char(ವರ್ಣ *errbuf, size_t errbuflen, ಪೂರ್ಣಾಙ್ಕ c)
{
    ಯದಿ (errbuf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ "";
    vim_snprintf(errbuf, errbuflen, _(e_illegal_character_str),
		    (ವರ್ಣ *)transchar(c));
    ಪ್ರತ್ಯಾಗಮ errbuf;
}

    ಸ್ಥಿರ ವರ್ಣ *
illegal_char_after_chr(ವರ್ಣ *errbuf, size_t errbuflen, ಪೂರ್ಣಾಙ್ಕ c)
{
    ಯದಿ (errbuf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ "";
    vim_snprintf(errbuf, errbuflen, _(e_illegal_character_after_chr), c);
    ಪ್ರತ್ಯಾಗಮ errbuf;
}

/*
 * Check string options in a buffer for NULL value.
 */
    ಶೂನ್ಯ
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
#ಯದಿ defined(FEAT_EVAL)
    check_string_option(&buf->b_p_inde);
    check_string_option(&buf->b_p_indk);
#endif
#ಯದಿ defined(FEAT_BEVAL) && defined(FEAT_EVAL)
    check_string_option(&buf->b_p_bexpr);
#endif
#ಯದಿ defined(FEAT_CRYPT)
    check_string_option(&buf->b_p_cm);
#endif
    check_string_option(&buf->b_p_fp);
#ಯದಿ defined(FEAT_EVAL)
    check_string_option(&buf->b_p_fex);
#endif
#ifdef FEAT_CRYPT
    check_string_option(&buf->b_p_ಕೀಲಿ);
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
    ಶೂನ್ಯ
free_string_option(char_u *p)
{
    ಯದಿ (p != empty_option)
	vim_free(p);
}

    ಶೂನ್ಯ
clear_string_option(char_u **pp)
{
    ಯದಿ (*pp != empty_option)
	vim_free(*pp);
    *pp = empty_option;
}

    ಶೂನ್ಯ
check_string_option(char_u **pp)
{
    ಯದಿ (*pp == ರಿಕ್ತ)
	*pp = empty_option;
}

/*
 * Set global value for string option when it's a local option.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
set_string_option_global(
    ಪೂರ್ಣಾಙ್ಕ		opt_idx,	// option index
    char_u	**varp)		// pointer to option variable
{
    char_u	**p, *s;

    // the global value is always allocated
    ಯದಿ (is_window_local_option(opt_idx))
	p = (char_u **)GLOBAL_WO(varp);
    ಅನ್ಯಥಾ
	p = (char_u **)get_option_var(opt_idx);
    ಯದಿ (!is_global_option(opt_idx)
	    && p != varp
	    && (s = vim_strsave(*varp)) != ರಿಕ್ತ)
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
    ಶೂನ್ಯ
set_string_option_direct(
    char_u	*name,
    ಪೂರ್ಣಾಙ್ಕ		opt_idx,
    char_u	*val,
    ಪೂರ್ಣಾಙ್ಕ		opt_flags,	// OPT_FREE, OPT_LOCAL and/or OPT_GLOBAL
    ಪೂರ್ಣಾಙ್ಕ		set_sid UNUSED)
{
    char_u	*s;
    char_u	**varp;
    ಪೂರ್ಣಾಙ್ಕ		both = (opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0;
    ಪೂರ್ಣಾಙ್ಕ		idx = opt_idx;

    ಯದಿ (idx == -1)		// use name
    {
	idx = findoption(name);
	ಯದಿ (idx < 0)	// not found (should not happen)
	{
	    semsg(_(e_internal_error_str), "set_string_option_direct()");
	    siemsg("For option %s", name);
	    ಪ್ರತ್ಯಾಗಮ;
	}
    }

    ಯದಿ (is_hidden_option(idx))		// can't set hidden option
	ಪ್ರತ್ಯಾಗಮ;

    s = vim_strsave(val);
    ಯದಿ (s == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    varp = (char_u **)get_option_varp_scope(idx,
	    both ? OPT_LOCAL : opt_flags);
    ಯದಿ ((opt_flags & OPT_FREE) && (get_option_flags(idx) & P_ALLOCED))
	free_string_option(*varp);
    *varp = s;

    // For buffer/window local option may also set the global value.
    ಯದಿ (both)
	set_string_option_global(idx, varp);

    set_option_flag(idx, P_ALLOCED);

    // When setting both values of a global option with a local value,
    // make the local value empty, so that the global value is used.
    ಯದಿ (is_global_local_option(idx) && both)
    {
	free_string_option(*varp);
	*varp = empty_option;
    }
#ifdef FEAT_EVAL
    ಯದಿ (set_sid != SID_NONE)
    {
	sctx_T script_ctx;

	ಯದಿ (set_sid == 0)
	    script_ctx = current_sctx;
	ಅನ್ಯಥಾ
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
    ಶೂನ್ಯ
set_string_option_direct_in_win(
	win_T		*wp,
	char_u		*name,
	ಪೂರ್ಣಾಙ್ಕ		opt_idx,
	char_u		*val,
	ಪೂರ್ಣಾಙ್ಕ		opt_flags,
	ಪೂರ್ಣಾಙ್ಕ		set_sid)
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

#ಯದಿ defined(FEAT_PROP_POPUP)
/*
 * Like set_string_option_direct(), but for a buffer-local option in "buf".
 * Blocks autocommands to avoid the old curbuf becoming invalid.
 */
    ಶೂನ್ಯ
set_string_option_direct_in_buf(
	buf_T		*buf,
	char_u		*name,
	ಪೂರ್ಣಾಙ್ಕ		opt_idx,
	char_u		*val,
	ಪೂರ್ಣಾಙ್ಕ		opt_flags,
	ಪೂರ್ಣಾಙ್ಕ		set_sid)
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
    ವರ್ಣ *
set_string_option(
    ಪೂರ್ಣಾಙ್ಕ		opt_idx,
    char_u	*ಮೌಲ್ಯ,
    ಪೂರ್ಣಾಙ್ಕ		opt_flags,	// OPT_LOCAL and/or OPT_GLOBAL
    ವರ್ಣ	*errbuf,
    size_t	errbuflen)
{
    char_u	*s;
    char_u	**varp;
    char_u	*oldval;
#ಯದಿ defined(FEAT_EVAL)
    char_u	*oldval_l = ರಿಕ್ತ;
    char_u	*oldval_g = ರಿಕ್ತ;
    char_u	*saved_oldval = ರಿಕ್ತ;
    char_u	*saved_oldval_l = ರಿಕ್ತ;
    char_u	*saved_oldval_g = ರಿಕ್ತ;
    char_u	*saved_newval = ರಿಕ್ತ;
#endif
    ವರ್ಣ	*errmsg = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		ಮೌಲ್ಯ_checked = FALSE;

    ಯದಿ (is_hidden_option(opt_idx))	// don't set hidden option
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    s = vim_strsave(ಮೌಲ್ಯ == ರಿಕ್ತ ? (char_u *)"" : ಮೌಲ್ಯ);
    ಯದಿ (s == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    varp = (char_u **)get_option_varp_scope(opt_idx,
	    (opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0
	    ? (is_global_local_option(opt_idx)
		? OPT_GLOBAL : OPT_LOCAL)
	    : opt_flags);
    oldval = *varp;
#ಯದಿ defined(FEAT_EVAL)
    ಯದಿ ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	oldval_l = *(char_u **)get_option_varp_scope(opt_idx, OPT_LOCAL);
	oldval_g = *(char_u **)get_option_varp_scope(opt_idx, OPT_GLOBAL);
    }
#endif
    *varp = s;

#ಯದಿ defined(FEAT_EVAL)
    ಯದಿ (!starting
# ifdef FEAT_CRYPT
	    && !is_crypt_ಕೀಲಿ_option(opt_idx)
# endif
       )
    {
	ಯದಿ (oldval_l != ರಿಕ್ತ)
	    saved_oldval_l = vim_strsave(oldval_l);
	ಯದಿ (oldval_g != ರಿಕ್ತ)
	    saved_oldval_g = vim_strsave(oldval_g);
	saved_oldval = vim_strsave(oldval);
	saved_newval = vim_strsave(s);
    }
#endif
    ಯದಿ ((errmsg = did_set_string_option(opt_idx, varp, oldval, ಮೌಲ್ಯ, errbuf,
		    errbuflen, opt_flags, OP_NONE, &ಮೌಲ್ಯ_checked)) == ರಿಕ್ತ)
	did_set_option(opt_idx, opt_flags, TRUE, ಮೌಲ್ಯ_checked);

#ಯದಿ defined(FEAT_EVAL)
    // call autocommand after handling side effects
    ಯದಿ (errmsg == ರಿಕ್ತ)
	trigger_optionset_string(opt_idx, opt_flags,
		saved_oldval, saved_oldval_l,
		saved_oldval_g, saved_newval);
    vim_free(saved_oldval);
    vim_free(saved_oldval_l);
    vim_free(saved_oldval_g);
    vim_free(saved_newval);
#endif
    ಪ್ರತ್ಯಾಗಮ errmsg;
}

/*
 * Return TRUE if "val" is a valid 'filetype' name.
 * Also used for 'syntax' and 'keymap'.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
valid_filetype(char_u *val)
{
    ಪ್ರತ್ಯಾಗಮ valid_name(val, ".-_");
}

#ifdef FEAT_STL_OPT
/*
 * Check validity of options with the 'statusline' format.
 * Return an untranslated error message or NULL.
 */
    ಸ್ಥಿರ ವರ್ಣ *
check_stl_option(char_u *s)
{
    ಪೂರ್ಣಾಙ್ಕ		groupdepth = 0;
    ಸ್ಥಿರ ವರ್ಣ errbuf[ERR_BUFLEN];
    ಪೂರ್ಣಾಙ್ಕ		errbuflen = ERR_BUFLEN;

    ಯಾವತ್ (*s)
    {
	// Check for valid keys after % sequences
	ಯಾವತ್ (*s && *s != '%')
	    s++;
	ಯದಿ (!*s)
	    ಭಙ್ಗ;
	s++;
	ಯದಿ (*s == STL_CLICKFUNC)
	{
	    ಯದಿ (s[1] == ']')
	    {
		// %[] - end click region
		s += 2;
		ಅನುವರ್ತ;
	    }
	    ಯದಿ (ASCII_ISALPHA(s[1]) || s[1] == '_')
	    {
		// %[FuncName] - start click region
		char_u *rb = vim_strchr(s + 2, ']');
		ಯದಿ (rb != ರಿಕ್ತ)
		{
		    s = rb + 1;
		    ಅನುವರ್ತ;
		}
	    }
	    // Bare %[ is invalid
	    ಪ್ರತ್ಯಾಗಮ illegal_char(errbuf, errbuflen, *s);
	}
	ಯದಿ (*s == STL_LINEBREAK)
	{
	    // Plain %@ - line break
	    s++;
	    ಅನುವರ್ತ;
	}
	ಯದಿ (*s == '%' || *s == STL_TRUNCMARK || *s == STL_SEPARATE)
	{
	    s++;
	    ಅನುವರ್ತ;
	}
	ಯದಿ (*s == ')')
	{
	    s++;
	    ಯದಿ (--groupdepth < 0)
		ಭಙ್ಗ;
	    ಅನುವರ್ತ;
	}
	ಯದಿ (*s == '-')
	    s++;
	ಯಾವತ್ (VIM_ISDIGIT(*s))
	    s++;
	ಯದಿ (*s == STL_USER_HL)
	    ಅನುವರ್ತ;
	ಯದಿ (*s == STL_CLICKFUNC)
	{
	    // %N[FuncName] or %N[]
	    ಯದಿ (s[1] == ']')
	    {
		s += 2;
		ಅನುವರ್ತ;
	    }
	    ಯದಿ (ASCII_ISALPHA(s[1]) || s[1] == '_')
	    {
		char_u *rb = vim_strchr(s + 2, ']');
		ಯದಿ (rb != ರಿಕ್ತ)
		{
		    s = rb + 1;
		    ಅನುವರ್ತ;
		}
	    }
	    // Bare %N[ is invalid
	    ಪ್ರತ್ಯಾಗಮ illegal_char(errbuf, errbuflen, *s);
	}
	ಯದಿ (*s == STL_LINEBREAK)
	{
	    // %N@ - line break
	    s++;
	    ಅನುವರ್ತ;
	}
	ಯದಿ (*s == '.')
	{
	    s++;
	    ಯಾವತ್ (*s && VIM_ISDIGIT(*s))
		s++;
	}
	ಯದಿ (*s == '(')
	{
	    groupdepth++;
	    ಅನುವರ್ತ;
	}
	ಯದಿ (vim_strchr(STL_ALL, *s) == ರಿಕ್ತ)
	{
	    ಪ್ರತ್ಯಾಗಮ illegal_char(errbuf, errbuflen, *s);
	}
	ಯದಿ (*s == '{')
	{
	    ಪೂರ್ಣಾಙ್ಕ reevaluate = (*++s == '%');

	    ಯದಿ (reevaluate && *++s == '}')
		// "}" is not allowed immediately after "%{%"
		ಪ್ರತ್ಯಾಗಮ illegal_char(errbuf, errbuflen, '}');
	    ಯಾವತ್ ((*s != '}' || (reevaluate && s[-1] != '%')) && *s)
		s++;
	    ಯದಿ (*s != '}')
		ಪ್ರತ್ಯಾಗಮ e_unclosed_expression_sequence;
	}
    }
    ಯದಿ (groupdepth != 0)
	ಪ್ರತ್ಯಾಗಮ e_unbalanced_groups;
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}
#endif

/*
 * Check for a "normal" directory or file name in some options.  Disallow a
 * path separator (slash and/or backslash), wildcards and characters that are
 * often illegal in a file name. Be more permissive if "secure" is off.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
check_illegal_path_names(ಪೂರ್ಣಾಙ್ಕ opt_idx, char_u **varp)
{
    ಪ್ರತ್ಯಾಗಮ (((get_option_flags(opt_idx) & P_NFNAME)
		&& vim_strpbrk(*varp, (char_u *)(secure
			? "/\\*?[|;&<>\r\n" : "/\\*?[<>\r\n")) != ರಿಕ್ತ)
	    || ((get_option_flags(opt_idx) & P_NDNAME)
		&& vim_strpbrk(*varp, (char_u *)"*?[|;&<>\r\n") != ರಿಕ್ತ));
}

/*
 * An option that accepts a list of flags is changed.
 * e.g. 'viewoptions', 'switchbuf', 'casemap', etc.
 */
    ಸ್ಥಿರ ವರ್ಣ *
did_set_opt_flags(char_u *val, ವರ್ಣ **ಮೌಲ್ಯಗಳು, unsigned *flagp, ಪೂರ್ಣಾಙ್ಕ list)
{
    ಯದಿ (opt_strings_flags(val, ಮೌಲ್ಯಗಳು, flagp, list) == FAIL)
	ಪ್ರತ್ಯಾಗಮ e_invalid_argument;

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * An option that accepts a list of string values is changed.
 * e.g. 'nrformats', 'scrollopt', 'wildoptions', etc.
 */
    ಸ್ಥಿರ ವರ್ಣ *
did_set_opt_strings(char_u *val, ವರ್ಣ **ಮೌಲ್ಯಗಳು, ಪೂರ್ಣಾಙ್ಕ list)
{
    ಪ್ರತ್ಯಾಗಮ did_set_opt_flags(val, ಮೌಲ್ಯಗಳು, ರಿಕ್ತ, list);
}

/*
 * An option which is a list of flags is set.  Valid values are in 'flags'.
 */
    ಸ್ಥಿರ ವರ್ಣ *
did_set_option_listflag(
	char_u *val,
	char_u *flags,
	ವರ್ಣ *errbuf,
	size_t errbuflen)
{
    char_u	*s;

    ಪ್ರತಿ (s = val; *s; ++s)
	ಯದಿ (vim_strchr(flags, *s) == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ illegal_char(errbuf, errbuflen, *s);

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Expand an option that accepts a list of fixed string values with known
 * number of items.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
expand_set_opt_string(
	optexpand_T *args,
	ವರ್ಣ **ಮೌಲ್ಯಗಳು,
	size_t ಅಂಕೆ_ಮೌಲ್ಯಗಳು,
	ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_Matches,
	char_u ***matches)
{
    char_u	*p;
    regmatch_T	*regmatch = args->oe_regmatch;
    ಪೂರ್ಣಾಙ್ಕ		include_orig_val = args->oe_include_orig_val;
    char_u	*option_val = args->oe_opt_ಮೌಲ್ಯ;

    // Assume numValues is small since they are fixed enums, so just allocate
    // upfront instead of needing two passes to calculate output size.
    *matches = ALLOC_MULT(char_u *, ಅಂಕೆ_ಮೌಲ್ಯಗಳು + 1);
    ಯದಿ (*matches == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಪೂರ್ಣಾಙ್ಕ ಎಣಿಕೆ = 0;

    ಯದಿ (include_orig_val && *option_val != NUL)
    {
	p = vim_strsave(option_val);
	ಯದಿ (p == ರಿಕ್ತ)
	{
	    VIM_CLEAR(*matches);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	(*matches)[ಎಣಿಕೆ++] = p;
    }

    ಪ್ರತಿ (ವರ್ಣ **val = ಮೌಲ್ಯಗಳು; *val != ರಿಕ್ತ; val++)
    {
	ಯದಿ (include_orig_val && *option_val != NUL)
	{
	    ಯದಿ (STRCMP((char_u*)*val, option_val) == 0)
		ಅನುವರ್ತ;
	}
	ಯದಿ (vim_regexec(regmatch, (char_u*)(*val), (colnr_T)0))
	{
	    p = vim_strsave((char_u*)*val);
	    ಯದಿ (p == ರಿಕ್ತ)
	    {
		ಯದಿ (ಎಣಿಕೆ == 0)
		{
		    VIM_CLEAR(*matches);
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		}
		ಅನ್ಯಥಾ
		    ಭಙ್ಗ;
	    }
	    (*matches)[ಎಣಿಕೆ++] = p;
	}
    }
    ಯದಿ (ಎಣಿಕೆ == 0)
    {
	VIM_CLEAR(*matches);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    *ಅಂಕೆ_Matches = ಎಣಿಕೆ;
    ಪ್ರತ್ಯಾಗಮ OK;
}

ಸ್ಥಿರ char_u *set_opt_callback_orig_option = ರಿಕ್ತ;
ಸ್ಥಿರ char_u *((*set_opt_callback_func)(expand_T *, ಪೂರ್ಣಾಙ್ಕ));

/*
 * Callback used by expand_set_opt_generic to also include the original value
 * as the first item.
 */
    ಸ್ಥಿರ char_u *
expand_set_opt_generic_cb(expand_T *xp, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಯದಿ (idx == 0)
    {
	ಯದಿ (set_opt_callback_orig_option != ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ set_opt_callback_orig_option;
	ಅನ್ಯಥಾ
	    ಪ್ರತ್ಯಾಗಮ (char_u *)""; // empty strings are ignored
    }
    ಪ್ರತ್ಯಾಗಮ set_opt_callback_func(xp, idx - 1);
}

/*
 * Expand an option with a callback that iterates through a list of possible
 * names using an index.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
expand_set_opt_generic(
	optexpand_T *args,
	char_u *((*func)(expand_T *, ಪೂರ್ಣಾಙ್ಕ)),
	ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_Matches,
	char_u ***matches)
{
    ಪೂರ್ಣಾಙ್ಕ ret;

    set_opt_callback_orig_option = args->oe_include_orig_val ?
	args->oe_opt_ಮೌಲ್ಯ : ರಿಕ್ತ;
    set_opt_callback_func = func;

    ret = ExpandGeneric(
	    (char_u*)"", // not using fuzzy as currently EXPAND_STRING_SETTING doesn't use it
	    args->oe_xp,
	    args->oe_regmatch,
	    matches,
	    ಅಂಕೆ_Matches,
	    expand_set_opt_generic_cb,
	    FALSE);

    set_opt_callback_orig_option = ರಿಕ್ತ;
    set_opt_callback_func = ರಿಕ್ತ;
    ಪ್ರತ್ಯಾಗಮ ret;
}

#ಯದಿ defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
ಸ್ಥಿರ garray_T *expand_cb_ga;
ಸ್ಥಿರ optexpand_T *expand_cb_args;

/*
 * Callback provided to a function in expand_set_opt_callback. Will perform
 * regex matching against the value and add to the list.
 *
 * Returns OK usually. Returns FAIL if it failed to allocate memory, and the
 * caller should terminate the enumeration.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
expand_set_opt_callback_cb(char_u *val)
{
    regmatch_T	*regmatch = expand_cb_args->oe_regmatch;
    expand_T	*xp = expand_cb_args->oe_xp;
    garray_T	*ga = expand_cb_ga;
    char_u	*str;

    ಯದಿ (val == ರಿಕ್ತ || *val == NUL)
	ಪ್ರತ್ಯಾಗಮ OK;

    ಯದಿ (xp->xp_pattern[0] != NUL &&
	    !vim_regexec(regmatch, val, (colnr_T)0))
	ಪ್ರತ್ಯಾಗಮ OK;

    str = vim_strsave_escaped(val, (char_u *)" \t\\");

    ಯದಿ (str == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಯದಿ (ga_grow(ga, 1) == FAIL)
    {
	vim_free(str);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ((char_u **)ga->ga_data)[ga->ga_len] = str;
    ++ga->ga_len;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Expand an option with a provided function that takes a callback. The
 * function will enumerate through all options and call the callback to add it
 * to the list.
 *
 * "func" is the enumerator function that will generate the list of options.
 * "func_params" is a single parameter that will be passed to func.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
expand_set_opt_callback(
	optexpand_T *args,
	ಶೂನ್ಯ (*func)(optexpand_T *, ಶೂನ್ಯ* params, ಪೂರ್ಣಾಙ್ಕ (*cb)(char_u *val)),
	ಶೂನ್ಯ *func_params,
	ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_Matches,
	char_u ***matches)
{
    garray_T	ga;
    ಪೂರ್ಣಾಙ್ಕ		include_orig_val = args->oe_include_orig_val;
    char_u	*option_val = args->oe_opt_ಮೌಲ್ಯ;

    ga_init2(&ga, ಪರಿಮಾಣ(ವರ್ಣ *), 30);

    ಯದಿ (include_orig_val && *option_val != NUL)
    {
	char_u *p = vim_strsave(option_val);
	ಯದಿ (p == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	ಯದಿ (ga_grow(&ga, 1) == FAIL)
	{
	    vim_free(p);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	((char_u **)ga.ga_data)[ga.ga_len] = p;
	++ga.ga_len;
    }

    expand_cb_ga = &ga;
    expand_cb_args = args;

    func(args, func_params, expand_set_opt_callback_cb);

    expand_cb_ga = ರಿಕ್ತ;
    expand_cb_args = ರಿಕ್ತ;

    *matches = ga.ga_data;
    *ಅಂಕೆ_Matches = ga.ga_len;
    ಪ್ರತ್ಯಾಗಮ OK;
}
#endif

/*
 * Expand an option which is a list of flags.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
expand_set_opt_listflag(
	optexpand_T *args,
	char_u *flags,
	ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_Matches,
	char_u ***matches)
{
    char_u  *p;
    char_u  *option_val = args->oe_opt_ಮೌಲ್ಯ;
    char_u  *cmdline_val = args->oe_set_arg;
    ಪೂರ್ಣಾಙ್ಕ	    append = args->oe_append;
    ಪೂರ್ಣಾಙ್ಕ	    include_orig_val = args->oe_include_orig_val && (*option_val != NUL);

    size_t ಅಂಕೆ_flags = STRLEN(flags);

    // Assume we only have small number of flags, so just allocate max size.
    *matches = ALLOC_MULT(char_u *, ಅಂಕೆ_flags + 1);
    ಯದಿ (*matches == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಪೂರ್ಣಾಙ್ಕ ಎಣಿಕೆ = 0;

    ಯದಿ (include_orig_val)
    {
	p = vim_strsave(option_val);
	ಯದಿ (p == ರಿಕ್ತ)
	{
	    VIM_CLEAR(*matches);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	(*matches)[ಎಣಿಕೆ++] = p;
    }

    ಪ್ರತಿ (char_u *flag = flags; *flag != NUL; flag++)
    {
	ಯದಿ (append && vim_strchr(option_val, *flag) != ರಿಕ್ತ)
	    ಅನುವರ್ತ;

	ಯದಿ (vim_strchr(cmdline_val, *flag) == ರಿಕ್ತ)
	{
	    ಯದಿ (include_orig_val
		    && option_val[1] == NUL
		    && *flag == option_val[0])
	    {
		// This value is already used as the first choice as it's the
		// existing flag. Just skip it to avoid duplicate.
		ಅನುವರ್ತ;
	    }
	    p = vim_strnsave(flag, 1);
	    ಯದಿ (p == ರಿಕ್ತ)
	    {
		ಯದಿ (ಎಣಿಕೆ == 0)
		{
		    VIM_CLEAR(*matches);
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		}
		ಅನ್ಯಥಾ
		    ಭಙ್ಗ;
	    }
	    (*matches)[ಎಣಿಕೆ++] = p;
	}
    }

    ಯದಿ (ಎಣಿಕೆ == 0)
    {
	VIM_CLEAR(*matches);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    *ಅಂಕೆ_Matches = ಎಣಿಕೆ;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * The 'ambiwidth' option is changed.
 */
    ವರ್ಣ *
did_set_ambiwidth(optset_T *args UNUSED)
{
    ಯದಿ (check_opt_strings(p_ambw, p_ambw_ಮೌಲ್ಯಗಳು, FALSE) != OK)
	ಪ್ರತ್ಯಾಗಮ e_invalid_argument;

    ಪ್ರತ್ಯಾಗಮ check_chars_options();
}

// "name" must be a string literal, the length is computed at compile time.
#define completing_ಮೌಲ್ಯ_for_subopt(args, name) \
	  completing_ಮೌಲ್ಯ_for_subopt_len(args, name, (ಪೂರ್ಣಾಙ್ಕ)STRLEN_LITERAL(name))

/*
 * Return true when completing the value of the sub-option "name" with length
 * "len", e.g. the value after "close:" in 'completepopup'.
 */
    ಸ್ಥಿರ bool
completing_ಮೌಲ್ಯ_for_subopt_len(optexpand_T *args, ವರ್ಣ *name, ಪೂರ್ಣಾಙ್ಕ len)
{
    char_u  *colon = args->oe_xp->xp_pattern - 1;
    ಪೂರ್ಣಾಙ್ಕ	    off = (ಪೂರ್ಣಾಙ್ಕ)(colon - args->oe_set_arg);

    ಯದಿ (off < len)
	ಪ್ರತ್ಯಾಗಮ false;
    // The name must follow a comma when it does not start the option value.
    ಯದಿ (off > len && *(colon - len - 1) != ',')
	ಪ್ರತ್ಯಾಗಮ false;

    ಪ್ರತ್ಯಾಗಮ STRNCMP(colon - len, name, len) == 0;
}

    ಪೂರ್ಣಾಙ್ಕ
expand_set_ambiwidth(optexpand_T *args, ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಪ್ರತ್ಯಾಗಮ expand_set_opt_string(
	    args,
	    p_ambw_ಮೌಲ್ಯಗಳು,
	    ARRAY_LENGTH(p_ambw_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}

/*
 * The 'background' option is changed.
 */
    ವರ್ಣ *
did_set_background(optset_T *args)
{
    ಯದಿ (check_opt_strings(p_bg, p_bg_ಮೌಲ್ಯಗಳು, FALSE) == FAIL)
	ಪ್ರತ್ಯಾಗಮ e_invalid_argument;

    ಯದಿ (args->os_oldval.string != ರಿಕ್ತ && args->os_oldval.string[0] == *p_bg)
	// Value was not changed
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

#ifdef FEAT_EVAL
    ಪೂರ್ಣಾಙ್ಕ dark = (*p_bg == 'd');
#endif

    init_highlight(FALSE, FALSE);

#ifdef FEAT_EVAL
    ಯದಿ (dark != (*p_bg == 'd')
	    && get_var_ಮೌಲ್ಯ((char_u *)"g:colors_name") != ರಿಕ್ತ)
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

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

    ಪೂರ್ಣಾಙ್ಕ
expand_set_background(optexpand_T *args, ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಪ್ರತ್ಯಾಗಮ expand_set_opt_string(
	    args,
	    p_bg_ಮೌಲ್ಯಗಳು,
	    ARRAY_LENGTH(p_bg_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}

/*
 * The 'backspace' option is changed.
 */
    ವರ್ಣ *
did_set_backspace(optset_T *args UNUSED)
{
    ಯದಿ (VIM_ISDIGIT(*p_bs))
    {
	ಯದಿ (*p_bs > '3' || p_bs[1] != NUL)
	    ಪ್ರತ್ಯಾಗಮ e_invalid_argument;
    }
    ಅನ್ಯಥಾ ಯದಿ (check_opt_strings(p_bs, p_bs_ಮೌಲ್ಯಗಳು, TRUE) != OK)
	ಪ್ರತ್ಯಾಗಮ e_invalid_argument;

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

    ಪೂರ್ಣಾಙ್ಕ
expand_set_backspace(optexpand_T *args, ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಪ್ರತ್ಯಾಗಮ expand_set_opt_string(
	    args,
	    p_bs_ಮೌಲ್ಯಗಳು,
	    ARRAY_LENGTH(p_bs_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}

/*
 * The 'backupcopy' option is changed.
 */
    ವರ್ಣ *
did_set_backupcopy(optset_T *args)
{
    char_u		*bkc = p_bkc;
    unsigned ಪೂರ್ಣಾಙ್ಕ	*flags = &bkc_flags;
    ವರ್ಣ		*errmsg = ರಿಕ್ತ;

    ಯದಿ (args->os_flags & OPT_LOCAL)
    {
	bkc = curbuf->b_p_bkc;
	flags = &curbuf->b_bkc_flags;
    }
    ಅನ್ಯಥಾ ಯದಿ (!(args->os_flags & OPT_GLOBAL))
	// When using :set, clear the local flags.
	curbuf->b_bkc_flags = 0;

    ಯದಿ ((args->os_flags & OPT_LOCAL) && *bkc == NUL)
	// make the local value empty: use the global value
	*flags = 0;
    ಅನ್ಯಥಾ
    {
	ಯದಿ (opt_strings_flags(bkc, p_bkc_ಮೌಲ್ಯಗಳು, flags, TRUE) != OK)
	    errmsg = e_invalid_argument;
	ಯದಿ ((((ಪೂರ್ಣಾಙ್ಕ)*flags & BKC_AUTO) != 0)
		+ (((ಪೂರ್ಣಾಙ್ಕ)*flags & BKC_YES) != 0)
		+ (((ಪೂರ್ಣಾಙ್ಕ)*flags & BKC_NO) != 0) != 1)
	{
	    // Must have exactly one of "auto", "yes"  and "no".
	    (ಶೂನ್ಯ)opt_strings_flags(args->os_oldval.string, p_bkc_ಮೌಲ್ಯಗಳು,
								  flags, TRUE);
	    errmsg = e_invalid_argument;
	}
    }

    ಪ್ರತ್ಯಾಗಮ errmsg;
}

    ಪೂರ್ಣಾಙ್ಕ
expand_set_backupcopy(optexpand_T *args, ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಪ್ರತ್ಯಾಗಮ expand_set_opt_string(
	    args,
	    p_bkc_ಮೌಲ್ಯಗಳು,
	    ARRAY_LENGTH(p_bkc_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}

/*
 * The 'backupext' or the 'patchmode' option is changed.
 */
    ವರ್ಣ *
did_set_backupext_or_patchmode(optset_T *args UNUSED)
{
    ಯದಿ (STRCMP(*p_bex == '.' ? p_bex + 1 : p_bex,
		*p_pm == '.' ? p_pm + 1 : p_pm) == 0)
	ಪ್ರತ್ಯಾಗಮ e_backupext_and_patchmode_are_equal;

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * The 'belloff' option is changed.
 */
    ವರ್ಣ *
did_set_belloff(optset_T *args UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ did_set_opt_flags(p_bo, p_bo_ಮೌಲ್ಯಗಳು, &bo_flags, TRUE);
}

    ಪೂರ್ಣಾಙ್ಕ
expand_set_belloff(optexpand_T *args, ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಪ್ರತ್ಯಾಗಮ expand_set_opt_string(
	    args,
	    p_bo_ಮೌಲ್ಯಗಳು,
	    ARRAY_LENGTH(p_bo_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}

#ಯದಿ defined(FEAT_LINEBREAK)
/*
 * The 'breakat' option is changed.
 */
    ವರ್ಣ *
did_set_breakat(optset_T *args UNUSED)
{
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		i;

    ಪ್ರತಿ (i = 0; i < 256; i++)
	breakat_flags[i] = FALSE;

    ಯದಿ (p_breakat != ರಿಕ್ತ)
	ಪ್ರತಿ (p = p_breakat; *p; p++)
	    breakat_flags[*p] = TRUE;

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * The 'breakindentopt' option is changed.
 */
    ವರ್ಣ *
did_set_breakindentopt(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    ಯದಿ (briopt_check(*varp, varp == &curwin->w_p_briopt ? curwin : ರಿಕ್ತ)
								      == FAIL)
	ಪ್ರತ್ಯಾಗಮ e_invalid_argument;

    // list setting requires a redraw
    ಯದಿ (varp == &curwin->w_p_briopt && curwin->w_briopt_list)
	redraw_all_later(UPD_NOT_VALID);

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

    ಪೂರ್ಣಾಙ್ಕ
expand_set_breakindentopt(
    optexpand_T		*args,
    ಪೂರ್ಣಾಙ್ಕ			*ಅಂಕೆ_Matches,
    char_u		***matches)
{
    ಪ್ರತ್ಯಾಗಮ expand_set_opt_string(
	    args,
	    p_briopt_ಮೌಲ್ಯಗಳು,
	    ARRAY_LENGTH(p_briopt_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}
#endif

#ಯದಿ defined(FEAT_BROWSE)
/*
 * The 'browsedir' option is changed.
 */
    ವರ್ಣ *
did_set_browsedir(optset_T *args UNUSED)
{
    ಯದಿ (check_opt_strings(p_bsdir, p_bsdir_ಮೌಲ್ಯಗಳು, FALSE) != OK
	    && !mch_isdir(p_bsdir))
	ಪ್ರತ್ಯಾಗಮ e_invalid_argument;

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

    ಪೂರ್ಣಾಙ್ಕ
expand_set_browsedir(optexpand_T *args, ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಪ್ರತ್ಯಾಗಮ expand_set_opt_string(
	    args,
	    p_bsdir_ಮೌಲ್ಯಗಳು,
	    ARRAY_LENGTH(p_bsdir_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}
#endif

/*
 * The 'bufhidden' option is changed.
 */
    ವರ್ಣ *
did_set_bufhidden(optset_T *args UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ did_set_opt_strings(curbuf->b_p_bh, p_bufhidden_ಮೌಲ್ಯಗಳು, FALSE);
}

    ಪೂರ್ಣಾಙ್ಕ
expand_set_bufhidden(optexpand_T *args, ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಪ್ರತ್ಯಾಗಮ expand_set_opt_string(
	    args,
	    p_bufhidden_ಮೌಲ್ಯಗಳು,
	    ARRAY_LENGTH(p_bufhidden_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}

/*
 * The 'buftype' option is changed.
 */
    ವರ್ಣ *
did_set_buftype(optset_T *args UNUSED)
{
    ಯದಿ (check_opt_strings(curbuf->b_p_bt, p_buftype_ಮೌಲ್ಯಗಳು, FALSE) != OK)
	ಪ್ರತ್ಯಾಗಮ e_invalid_argument;

    ಯದಿ (curwin->w_status_height)
    {
	curwin->w_redr_status = true;
	redraw_later(UPD_VALID);
    }
    curbuf->b_help = (curbuf->b_p_bt[0] == 'h');
    redraw_titles();

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

    ಪೂರ್ಣಾಙ್ಕ
expand_set_buftype(optexpand_T *args, ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಪ್ರತ್ಯಾಗಮ expand_set_opt_string(
	    args,
	    p_buftype_ಮೌಲ್ಯಗಳು,
	    ARRAY_LENGTH(p_buftype_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}

/*
 * The 'casemap' option is changed.
 */
    ವರ್ಣ *
did_set_casemap(optset_T *args UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ did_set_opt_flags(p_cmp, p_cmp_ಮೌಲ್ಯಗಳು, &cmp_flags, TRUE);
}

    ಪೂರ್ಣಾಙ್ಕ
expand_set_casemap(optexpand_T *args, ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಪ್ರತ್ಯಾಗಮ expand_set_opt_string(
	    args,
	    p_cmp_ಮೌಲ್ಯಗಳು,
	    ARRAY_LENGTH(p_cmp_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}

#ಯದಿ defined(HAVE_CLIPMETHOD)
    ಪೂರ್ಣಾಙ್ಕ
expand_set_clipboard(optexpand_T *args, ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಪ್ರತ್ಯಾಗಮ expand_set_opt_string(
	    args,
	    p_cb_ಮೌಲ್ಯಗಳು,
	    ARRAY_LENGTH(p_cb_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}
#endif

#ifdef HAVE_CLIPMETHOD
    ವರ್ಣ *
did_set_clipmethod(optset_T *args UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ choose_clipmethod();
}

    ಪೂರ್ಣಾಙ್ಕ
expand_set_clipmethod(optexpand_T *args, ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    // We want to expand using the predefined clipmethod values + clipboard
    // provider names.
    ಪೂರ್ಣಾಙ್ಕ		result;
    ವರ್ಣ	**ಮೌಲ್ಯಗಳು;
    ಪೂರ್ಣಾಙ್ಕ		ಎಣಿಕೆ, pos = 0, start = 0;
# ifdef FEAT_EVAL
    dict_T	*providers = get_vim_var_dict(VV_CLIPPROVIDERS);
# ಅನ್ಯಥಾ
    dict_T	*providers = ರಿಕ್ತ;
# endif
    hashtab_T	*ht = providers == ರಿಕ್ತ ? ರಿಕ್ತ : &providers->dv_hashtab;

    ಎಣಿಕೆ = (ht == ರಿಕ್ತ ? 0 : ht->ht_used);
# ifdef FEAT_WAYLAND_CLIPBOARD
    ಎಣಿಕೆ++;
    start++;
# endif
# ifdef FEAT_XCLIPBOARD
    ಎಣಿಕೆ++;
    start++;
# endif
    ಮೌಲ್ಯಗಳು = ALLOC_MULT(ವರ್ಣ *, ಎಣಿಕೆ + 1); // Add NULL terminator too

    ಯದಿ (ಮೌಲ್ಯಗಳು == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

# ifdef FEAT_WAYLAND_CLIPBOARD
    ಮೌಲ್ಯಗಳು[pos++] = "wayland";
# endif
# ifdef FEAT_XCLIPBOARD
    ಮೌಲ್ಯಗಳು[pos++] = "x11";
# endif

    ಯದಿ (ht != ರಿಕ್ತ)
	ಪ್ರತಿ (long_u i = 0; i < ht->ht_mask + 1; i++)
	{
	    hashitem_T	*hi = ht->ht_array + i;

	    ಯದಿ (!HASHITEM_EMPTY(hi))
		ಮೌಲ್ಯಗಳು[pos++] = (ವರ್ಣ *)vim_strsave(hi->hi_ಕೀಲಿ);
	}
    ಮೌಲ್ಯಗಳು[pos++] = ರಿಕ್ತ;

    result = expand_set_opt_string(
	    args,
	    ಮೌಲ್ಯಗಳು,
	    ಎಣಿಕೆ,
	    ಅಂಕೆ_Matches,
	    matches);

    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = start; i < ಎಣಿಕೆ; i++)
	vim_free(ಮೌಲ್ಯಗಳು[i]);
    vim_free(ಮೌಲ್ಯಗಳು);

    ಪ್ರತ್ಯಾಗಮ result;
}
#endif

/*
 * The global 'listchars' or 'fillchars' option is changed.
 */
    ಸ್ಥಿರ ವರ್ಣ *
did_set_global_listfillchars(char_u *val, ಪೂರ್ಣಾಙ್ಕ opt_lcs, ಪೂರ್ಣಾಙ್ಕ opt_flags,
						ವರ್ಣ *errbuf, size_t errbuflen)
{
    ವರ್ಣ	*errmsg = ರಿಕ್ತ;
    char_u	**local_ptr = opt_lcs ? &curwin->w_p_lcs : &curwin->w_p_fcs;

    // only apply the global value to "curwin" when it does not have a
    // local value
    ಯದಿ (opt_lcs)
	errmsg = set_listchars_option(curwin, val,
		**local_ptr == NUL || !(opt_flags & OPT_GLOBAL),
							    errbuf, errbuflen);
    ಅನ್ಯಥಾ
	errmsg = set_fillchars_option(curwin, val,
		**local_ptr == NUL || !(opt_flags & OPT_GLOBAL),
							    errbuf, errbuflen);
    ಯದಿ (errmsg != ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ errmsg;

    tabpage_T	*tp;
    win_T	*wp;

    // If the current window is set to use the global
    // 'listchars'/'fillchars' value, clear the window-local value.
    ಯದಿ (!(opt_flags & OPT_GLOBAL))
	clear_string_option(local_ptr);
    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	// If the current window has a local value need to apply it
	// again, it was changed when setting the global value.
	// If no error was returned above, we don't expect an error
	// here, so ignore the return value.
	ಯದಿ (opt_lcs)
	{
	    ಯದಿ (*wp->w_p_lcs == NUL)
		(ಶೂನ್ಯ)set_listchars_option(wp, wp->w_p_lcs, TRUE, ರಿಕ್ತ, 0);
	}
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (*wp->w_p_fcs == NUL)
		(ಶೂನ್ಯ)set_fillchars_option(wp, wp->w_p_fcs, TRUE, ರಿಕ್ತ, 0);
	}
    }

    redraw_all_later(UPD_NOT_VALID);

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * The 'fillchars' option or the 'listchars' option is changed.
 */
    ವರ್ಣ *
did_set_chars_option(optset_T *args)
{
    char_u **varp = (char_u **)args->os_varp;
    ವರ್ಣ *errmsg = ರಿಕ್ತ;

    ಯದಿ (   varp == &p_lcs		// global 'listchars'
	|| varp == &p_fcs)		// global 'fillchars'
	errmsg = did_set_global_listfillchars(*varp, varp == &p_lcs,
			  args->os_flags, args->os_errbuf, args->os_errbuflen);
    ಅನ್ಯಥಾ ಯದಿ (varp == &curwin->w_p_lcs)	// local 'listchars'
	errmsg = set_listchars_option(curwin, *varp, TRUE,
					  args->os_errbuf, args->os_errbuflen);
    ಅನ್ಯಥಾ ಯದಿ (varp == &curwin->w_p_fcs)	// local 'fillchars'
	errmsg = set_fillchars_option(curwin, *varp, TRUE,
					  args->os_errbuf, args->os_errbuflen);

    ಪ್ರತ್ಯಾಗಮ errmsg;
}

/*
 * Expand 'fillchars' or 'listchars' option value.
 */
    ಪೂರ್ಣಾಙ್ಕ
expand_set_chars_option(optexpand_T *args, ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    char_u **varp = (char_u **)args->oe_varp;
    ಪೂರ್ಣಾಙ್ಕ is_lcs = (varp == &p_lcs || varp == &curwin->w_p_lcs);
    ಪ್ರತ್ಯಾಗಮ expand_set_opt_generic(
	    args,
	    is_lcs ? get_listchars_name : get_fillchars_name,
	    ಅಂಕೆ_Matches,
	    matches);
}

/*
 * The 'cinoptions' option is changed.
 */
    ವರ್ಣ *
did_set_cinoptions(optset_T *args UNUSED)
{
    // TODO: recognize errors
    parse_cino(curbuf);

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

#ಯದಿ defined(FEAT_SYN_HL)
/*
 * The 'colorcolumn' option is changed.
 */
    ವರ್ಣ *
did_set_colorcolumn(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    ಪ್ರತ್ಯಾಗಮ check_colorcolumn(*varp, varp == &curwin->w_p_cc ? curwin : ರಿಕ್ತ);
}
#endif

/*
 * The 'comments' option is changed.
 */
    ವರ್ಣ *
did_set_comments(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u	*s;
    ವರ್ಣ	*errmsg = ರಿಕ್ತ;

    ಪ್ರತಿ (s = *varp; *s; )
    {
	ಯಾವತ್ (*s && *s != ':')
	{
	    ಯದಿ (vim_strchr((char_u *)COM_ALL, *s) == ರಿಕ್ತ
		    && !VIM_ISDIGIT(*s) && *s != '-')
	    {
		errmsg = illegal_char(args->os_errbuf, args->os_errbuflen, *s);
		ಭಙ್ಗ;
	    }
	    ++s;
	}
	ಯದಿ (*s++ == NUL)
	    errmsg = e_missing_colon;
	ಅನ್ಯಥಾ ಯದಿ (*s == ',' || *s == NUL)
	    errmsg = e_zero_length_string;
	ಯದಿ (errmsg != ರಿಕ್ತ)
	    ಭಙ್ಗ;
	ಯಾವತ್ (*s && *s != ',')
	{
	    ಯದಿ (*s == '\\' && s[1] != NUL)
		++s;
	    ++s;
	}
	s = skip_to_option_part(s);
    }

    ಪ್ರತ್ಯಾಗಮ errmsg;
}

/*
 * The 'commentstring' option is changed.
 */
    ವರ್ಣ *
did_set_commentstring(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    ಯದಿ (**varp != NUL && strstr((ವರ್ಣ *)*varp, "%s") == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ e_commentstring_must_be_empty_or_contain_str;

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Check if value for 'complete' is valid when 'complete' option is changed.
 */
    ವರ್ಣ *
did_set_complete(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u	*p, *t;
    char_u	buffer[LSIZE];
    char_u	*buf_ptr;
    char_u	char_before = NUL;
    ಪೂರ್ಣಾಙ್ಕ		escape;

    ಪ್ರತಿ (p = *varp; *p; )
    {
	vim_memset(buffer, 0, LSIZE);
	buf_ptr = buffer;
	escape = 0;

	// Extract substring while handling escaped commas
	ಯಾವತ್ (*p && (*p != ',' || escape) && buf_ptr < (buffer + LSIZE - 1))
	{
	    ಯದಿ (*p == '\\' && *(p + 1) == ',')
	    {
		escape = 1;  // Mark escape mode
		p++;         // Skip '\'
	    }
	    ಅನ್ಯಥಾ
	    {
		escape = 0;
		*buf_ptr++ = *p;
	    }
	    p++;
	}
	*buf_ptr = NUL;

	ಯದಿ (vim_strchr((char_u *)".wbuksid]tUFo", *buffer) == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ illegal_char(args->os_errbuf, args->os_errbuflen, *buffer);

	ಯದಿ (vim_strchr((char_u *)"ksF", *buffer) == ರಿಕ್ತ && *(buffer + 1) != NUL
		&& *(buffer + 1) != '^')
	    char_before = *buffer;
	ಅನ್ಯಥಾ
	{
	    // Test for a number after '^'
	    ಯದಿ ((t = vim_strchr(buffer, '^')) != ರಿಕ್ತ)
	    {
		*t++ = NUL;
		ಯದಿ (!*t)
		    char_before = '^';
		ಅನ್ಯಥಾ
		{
		    ಪ್ರತಿ (; *t; t++)
		    {
			ಯದಿ (!vim_isdigit(*t))
			{
			    char_before = '^';
			    ಭಙ್ಗ;
			}
		    }
		}
	    }
	}
	ಯದಿ (char_before != NUL)
	    ಪ್ರತ್ಯಾಗಮ illegal_char_after_chr(args->os_errbuf, args->os_errbuflen,
		    char_before);
	// Skip comma and spaces
	ಯಾವತ್ (*p == ',' || *p == ' ')
	    p++;
    }

#ifdef FEAT_COMPL_FUNC
    ಯದಿ (set_cpt_callbacks(args) != OK)
	ಪ್ರತ್ಯಾಗಮ illegal_char_after_chr(args->os_errbuf, args->os_errbuflen,
		'F');
#endif
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

    ಪೂರ್ಣಾಙ್ಕ
expand_set_complete(optexpand_T *args, ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಸ್ಥಿರ ವರ್ಣ *(p_cpt_ಮೌಲ್ಯಗಳು[]) = {
	".", "w", "b", "u", "k", "kspell", "s", "i", "d", "]", "t", "U", "F", "o",
	ರಿಕ್ತ};
    ಪ್ರತ್ಯಾಗಮ expand_set_opt_string(
	    args,
	    p_cpt_ಮೌಲ್ಯಗಳು,
	    ARRAY_LENGTH(p_cpt_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}

/*
 * The 'completeopt' option is changed.
 */
    ವರ್ಣ *
did_set_completeopt(optset_T *args UNUSED)
{
    char_u	*cot = p_cot;
    unsigned	*flags = &cot_flags;

    ಯದಿ (args->os_flags & OPT_LOCAL)
    {
	cot = curbuf->b_p_cot;
	flags = &curbuf->b_cot_flags;
    }
    ಅನ್ಯಥಾ ಯದಿ (!(args->os_flags & OPT_GLOBAL))
	// When using :set, clear the local flags.
	curbuf->b_cot_flags = 0;

    ಯದಿ (opt_strings_flags(cot, p_cot_ಮೌಲ್ಯಗಳು, flags, TRUE) != OK)
	ಪ್ರತ್ಯಾಗಮ e_invalid_argument;

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

    ಪೂರ್ಣಾಙ್ಕ
expand_set_completeopt(optexpand_T *args, ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಪ್ರತ್ಯಾಗಮ expand_set_opt_string(
	    args,
	    p_cot_ಮೌಲ್ಯಗಳು,
	    ARRAY_LENGTH(p_cot_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}

/*
 * The 'completefuzzycollect' option is changed.
 */
    ವರ್ಣ *
did_set_completefuzzycollect(optset_T *args UNUSED)
{
    ಯದಿ (opt_strings_flags(p_cfc, p_cfc_ಮೌಲ್ಯಗಳು, &cfc_flags, TRUE) != OK)
	ಪ್ರತ್ಯಾಗಮ e_invalid_argument;
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

    ಪೂರ್ಣಾಙ್ಕ
expand_set_completefuzzycollect(
	optexpand_T *args,
	ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_Matches,
	char_u ***matches)
{
    ಪ್ರತ್ಯಾಗಮ expand_set_opt_string(
	    args,
	    p_cfc_ಮೌಲ್ಯಗಳು,
	    ARRAY_LENGTH(p_cfc_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}

/*
 * The 'completeitemalign' option is changed.
 */
    ವರ್ಣ *
did_set_completeitemalign(optset_T *args UNUSED)
{
    char_u	*p = p_cia;
    unsigned	new_cia_flags = 0;
    ಪೂರ್ಣಾಙ್ಕ		seen[3] = { FALSE, FALSE, FALSE };
    ಪೂರ್ಣಾಙ್ಕ		ಎಣಿಕೆ = 0;
    char_u	buf[10];

    ಯಾವತ್ (*p)
    {
	copy_option_part(&p, buf, ಪರಿಮಾಣ(buf), ",");
	ಯದಿ (ಎಣಿಕೆ >= 3)
	    ಪ್ರತ್ಯಾಗಮ e_invalid_argument;

	ಯದಿ (STRCMP(buf, "abbr") == 0)
	{
	    ಯದಿ (seen[CPT_ABBR])
		ಪ್ರತ್ಯಾಗಮ e_invalid_argument;
	    new_cia_flags = new_cia_flags * 10 + CPT_ABBR;
	    seen[CPT_ABBR] = TRUE;
	    ಎಣಿಕೆ++;
	}
	ಅನ್ಯಥಾ ಯದಿ (STRCMP(buf, "kind") == 0)
	{
	    ಯದಿ (seen[CPT_KIND])
		ಪ್ರತ್ಯಾಗಮ e_invalid_argument;
	    new_cia_flags = new_cia_flags * 10 + CPT_KIND;
	    seen[CPT_KIND] = TRUE;
	    ಎಣಿಕೆ++;
	}
	ಅನ್ಯಥಾ ಯದಿ (STRCMP(buf, "menu") == 0)
	{
	    ಯದಿ (seen[CPT_MENU])
		ಪ್ರತ್ಯಾಗಮ e_invalid_argument;
	    new_cia_flags = new_cia_flags * 10 + CPT_MENU;
	    seen[CPT_MENU] = TRUE;
	    ಎಣಿಕೆ++;
	}
	ಅನ್ಯಥಾ
	    ಪ್ರತ್ಯಾಗಮ e_invalid_argument;
    }
    ಯದಿ (new_cia_flags == 0 || ಎಣಿಕೆ != 3)
	ಪ್ರತ್ಯಾಗಮ e_invalid_argument;

    cia_flags = new_cia_flags;
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

#ಯದಿ defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
/*
 * The 'completepopup' option is changed.
 */
    ವರ್ಣ *
did_set_completepopup(optset_T *args UNUSED)
{
    ಯದಿ (parse_completepopup(ರಿಕ್ತ) == FAIL)
	ಪ್ರತ್ಯಾಗಮ e_invalid_argument;

    popup_close_info();
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}
#endif

#ಯದಿ defined(BACKSLASH_IN_FILENAME)
/*
 * The 'completeslash' option is changed.
 */
    ವರ್ಣ *
did_set_completeslash(optset_T *args UNUSED)
{
    ಯದಿ (check_opt_strings(p_csl, p_csl_ಮೌಲ್ಯಗಳು, FALSE) != OK
	    || check_opt_strings(curbuf->b_p_csl, p_csl_ಮೌಲ್ಯಗಳು, FALSE) != OK)
	ಪ್ರತ್ಯಾಗಮ e_invalid_argument;

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

    ಪೂರ್ಣಾಙ್ಕ
expand_set_completeslash(optexpand_T *args, ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಪ್ರತ್ಯಾಗಮ expand_set_opt_string(
	    args,
	    p_csl_ಮೌಲ್ಯಗಳು,
	    ARRAY_LENGTH(p_csl_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}
#endif

#ಯದಿ defined(FEAT_CONCEAL)
/*
 * The 'concealcursor' option is changed.
 */
    ವರ್ಣ *
did_set_concealcursor(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    ಪ್ರತ್ಯಾಗಮ did_set_option_listflag(*varp, (char_u *)COCU_ALL, args->os_errbuf,
		    args->os_errbuflen);
}

    ಪೂರ್ಣಾಙ್ಕ
expand_set_concealcursor(optexpand_T *args, ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಪ್ರತ್ಯಾಗಮ expand_set_opt_listflag(args, (char_u*)COCU_ALL, ಅಂಕೆ_Matches, matches);
}
#endif

/*
 * The 'cpoptions' option is changed.
 */
    ವರ್ಣ *
did_set_cpoptions(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    ಪ್ರತ್ಯಾಗಮ did_set_option_listflag(*varp, (char_u *)CPO_ALL, args->os_errbuf,
		    args->os_errbuflen);
}

    ಪೂರ್ಣಾಙ್ಕ
expand_set_cpoptions(optexpand_T *args, ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಪ್ರತ್ಯಾಗಮ expand_set_opt_listflag(args, (char_u*)CPO_ALL, ಅಂಕೆ_Matches, matches);
}

#ಯದಿ defined(FEAT_CRYPT)
/*
 * The 'cryptkey' option is changed.
 */
    ವರ್ಣ *
did_set_cryptkey(optset_T *args)
{
    // Make sure the ":set" command doesn't show the new value in the
    // history.
    remove_ಕೀಲಿ_from_history();

    ಯದಿ (args->os_op != OP_NONE)
	// Don't allow set+=/-=/^= as they can allow for substring guessing
	ಪ್ರತ್ಯಾಗಮ e_invalid_argument;

    ಯದಿ (STRCMP(curbuf->b_p_ಕೀಲಿ, args->os_oldval.string) != 0)
    {
	// Need to update the swapfile.
	ml_set_crypt_ಕೀಲಿ(curbuf, args->os_oldval.string,
		*curbuf->b_p_cm == NUL ? p_cm : curbuf->b_p_cm);
	changed_internal();
    }
# ifdef FEAT_SODIUM
    ಯದಿ (crypt_method_is_sodium(crypt_get_method_nr(curbuf)))
	crypt_sodium_lock_ಕೀಲಿ(args->os_newval.string);
# endif

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * The 'cryptmethod' option is changed.
 */
    ವರ್ಣ *
did_set_cryptmethod(optset_T *args)
{
    char_u  *p;
    char_u  *s;

    ಯದಿ (args->os_flags & OPT_LOCAL)
	p = curbuf->b_p_cm;
    ಅನ್ಯಥಾ
	p = p_cm;
    ಯದಿ (check_opt_strings(p, p_cm_ಮೌಲ್ಯಗಳು, TRUE) != OK)
	ಪ್ರತ್ಯಾಗಮ e_invalid_argument;
    ಅನ್ಯಥಾ ಯದಿ (crypt_self_test() == FAIL)
	ಪ್ರತ್ಯಾಗಮ e_invalid_argument;

    // When setting the global value to empty, make it "zip".
    ಯದಿ (*p_cm == NUL)
    {
	free_string_option(p_cm);
	p_cm = vim_strsave((char_u *)"zip");
    }
    // When using ":set cm=name" the local value is going to be empty.
    // Do that here, otherwise the crypt functions will still use the
    // local value.
    ಯದಿ ((args->os_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	free_string_option(curbuf->b_p_cm);
	curbuf->b_p_cm = empty_option;
    }

    // Need to update the swapfile when the effective method changed.
    // Set "s" to the effective old value, "p" to the effective new
    // method and compare.
    ಯದಿ ((args->os_flags & OPT_LOCAL) && *args->os_oldval.string == NUL)
	s = p_cm;  // was previously using the global value
    ಅನ್ಯಥಾ
	s = args->os_oldval.string;
    ಯದಿ (*curbuf->b_p_cm == NUL)
	p = p_cm;  // is now using the global value
    ಅನ್ಯಥಾ
	p = curbuf->b_p_cm;
    ಯದಿ (STRCMP(s, p) != 0)
	ml_set_crypt_ಕೀಲಿ(curbuf, curbuf->b_p_ಕೀಲಿ, s);

    // If the global value changes need to update the swapfile for all
    // buffers using that value.
    ಯದಿ ((args->os_flags & OPT_GLOBAL)
	    && STRCMP(p_cm, args->os_oldval.string) != 0)
    {
	buf_T	*buf;

	FOR_ALL_BUFFERS(buf)
	    ಯದಿ (buf != curbuf && *buf->b_p_cm == NUL)
		ml_set_crypt_ಕೀಲಿ(buf, buf->b_p_ಕೀಲಿ, args->os_oldval.string);
    }
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

    ಪೂರ್ಣಾಙ್ಕ
expand_set_cryptmethod(optexpand_T *args, ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಪ್ರತ್ಯಾಗಮ expand_set_opt_string(
	    args,
	    p_cm_ಮೌಲ್ಯಗಳು,
	    ARRAY_LENGTH(p_cm_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}
#endif

#ಯದಿ defined(FEAT_CSCOPE) && defined(FEAT_QUICKFIX)
/*
 * The 'cscopequickfix' option is changed.
 */
    ವರ್ಣ *
did_set_cscopequickfix(optset_T *args UNUSED)
{
    char_u	*p;

    ಯದಿ (p_csqf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    p = p_csqf;
    ಯಾವತ್ (*p != NUL)
    {
	ಯದಿ (vim_strchr((char_u *)CSQF_CMDS, *p) == ರಿಕ್ತ
		|| p[1] == NUL
		|| vim_strchr((char_u *)CSQF_FLAGS, p[1]) == ರಿಕ್ತ
		|| (p[2] != NUL && p[2] != ','))
	    ಪ್ರತ್ಯಾಗಮ e_invalid_argument;
	ಅನ್ಯಥಾ ಯದಿ (p[2] == NUL)
	    ಭಙ್ಗ;
	ಅನ್ಯಥಾ
	    p += 3;
    }

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}
#endif

#ಯದಿ defined(FEAT_SYN_HL)
/*
 * The 'cursorlineopt' option is changed.
 */
    ವರ್ಣ *
did_set_cursorlineopt(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    // This could be changed to use opt_strings_flags() instead.
    ಯದಿ (**varp == NUL || fill_culopt_flags(*varp, curwin) != OK)
	ಪ್ರತ್ಯಾಗಮ e_invalid_argument;

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

    ಪೂರ್ಣಾಙ್ಕ
expand_set_cursorlineopt(optexpand_T *args, ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಪ್ರತ್ಯಾಗಮ expand_set_opt_string(
	    args,
	    p_culopt_ಮೌಲ್ಯಗಳು,
	    ARRAY_LENGTH(p_culopt_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}
#endif

/*
 * The 'debug' option is changed.
 */
    ವರ್ಣ *
did_set_debug(optset_T *args UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ did_set_opt_strings(p_debug, p_debug_ಮೌಲ್ಯಗಳು, TRUE);
}

    ಪೂರ್ಣಾಙ್ಕ
expand_set_debug(optexpand_T *args, ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಪ್ರತ್ಯಾಗಮ expand_set_opt_string(
	    args,
	    p_debug_ಮೌಲ್ಯಗಳು,
	    ARRAY_LENGTH(p_debug_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}

#ಯದಿ defined(FEAT_DIFF)
/*
 * The 'diffanchors' option is changed.
 */
    ವರ್ಣ *
did_set_diffanchors(optset_T *args)
{
    ಯದಿ (diffanchors_changed(args->os_flags & OPT_LOCAL) == FAIL)
	ಪ್ರತ್ಯಾಗಮ e_invalid_argument;

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * The 'diffopt' option is changed.
 */
    ವರ್ಣ *
did_set_diffopt(optset_T *args UNUSED)
{
    ಯದಿ (diffopt_changed() == FAIL)
	ಪ್ರತ್ಯಾಗಮ e_invalid_argument;

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

    ಪೂರ್ಣಾಙ್ಕ
expand_set_diffopt(optexpand_T *args, ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    expand_T *xp = args->oe_xp;

    ಯದಿ (xp->xp_pattern > args->oe_set_arg && *(xp->xp_pattern-1) == ':')
    {
	ಯದಿ (completing_ಮೌಲ್ಯ_for_subopt(args, "algorithm"))
	{
	    ಪ್ರತ್ಯಾಗಮ expand_set_opt_string(
		    args,
		    p_dip_algorithm_ಮೌಲ್ಯಗಳು,
		    ARRAY_LENGTH(p_dip_algorithm_ಮೌಲ್ಯಗಳು) - 1,
		    ಅಂಕೆ_Matches,
		    matches);
	}
	ಯದಿ (completing_ಮೌಲ್ಯ_for_subopt(args, "inline"))
	{
	    ಪ್ರತ್ಯಾಗಮ expand_set_opt_string(
		    args,
		    p_dip_inline_ಮೌಲ್ಯಗಳು,
		    ARRAY_LENGTH(p_dip_inline_ಮೌಲ್ಯಗಳು) - 1,
		    ಅಂಕೆ_Matches,
		    matches);
	}
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಪ್ರತ್ಯಾಗಮ expand_set_opt_string(
	    args,
	    p_dip_ಮೌಲ್ಯಗಳು,
	    ARRAY_LENGTH(p_dip_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}
#endif

/*
 * The 'display' option is changed.
 */
    ವರ್ಣ *
did_set_display(optset_T *args UNUSED)
{
    ಯದಿ (opt_strings_flags(p_dy, p_dy_ಮೌಲ್ಯಗಳು, &dy_flags, TRUE) != OK)
	ಪ್ರತ್ಯಾಗಮ e_invalid_argument;

    (ಶೂನ್ಯ)init_chartab();
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

    ಪೂರ್ಣಾಙ್ಕ
expand_set_display(optexpand_T *args, ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಪ್ರತ್ಯಾಗಮ expand_set_opt_string(
	    args,
	    p_dy_ಮೌಲ್ಯಗಳು,
	    ARRAY_LENGTH(p_dy_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}

/*
 * The 'eadirection' option is changed.
 */
    ವರ್ಣ *
did_set_eadirection(optset_T *args UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ did_set_opt_strings(p_ead, p_ead_ಮೌಲ್ಯಗಳು, FALSE);
}

    ಪೂರ್ಣಾಙ್ಕ
expand_set_eadirection(optexpand_T *args, ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಪ್ರತ್ಯಾಗಮ expand_set_opt_string(
	    args,
	    p_ead_ಮೌಲ್ಯಗಳು,
	    ARRAY_LENGTH(p_ead_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}

/*
 * One of the 'encoding', 'fileencoding', 'termencoding' or 'makeencoding'
 * options is changed.
 */
    ವರ್ಣ *
did_set_encoding(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u	**gvarp;
    ವರ್ಣ	*errmsg = ರಿಕ್ತ;
    char_u	*p;

    // Get the global option to compare with, otherwise we would have to check
    // two values for all local options.
    gvarp = (char_u **)get_option_varp_scope(args->os_idx, OPT_GLOBAL);

    ಯದಿ (gvarp == &p_fenc)
    {
	ಯದಿ (!curbuf->b_p_ma && args->os_flags != OPT_GLOBAL)
	    errmsg = e_cannot_make_changes_modifiable_is_off;
	ಅನ್ಯಥಾ ಯದಿ (vim_strchr(*varp, ',') != ರಿಕ್ತ)
	    // No comma allowed in 'fileencoding'; catches confusing it
	    // with 'fileencodings'.
	    errmsg = e_invalid_argument;
	ಅನ್ಯಥಾ
	{
	    // May show a "+" in the title now.
	    redraw_titles();
	    // Add 'fileencoding' to the swap file.
	    ml_setflags(curbuf);
	}
    }
    ಯದಿ (errmsg == ರಿಕ್ತ)
    {
	// canonize the value, so that STRCMP() can be used on it
	p = enc_canonize(*varp);
	ಯದಿ (p != ರಿಕ್ತ)
	{
	    vim_free(*varp);
	    *varp = p;
	}
	ಯದಿ (varp == &p_enc)
	{
	    errmsg = mb_init();
	    redraw_titles();
	}
    }

#ಯದಿ defined(FEAT_GUI_GTK)
    ಯದಿ (errmsg == ರಿಕ್ತ && varp == &p_tenc && gui.in_use)
    {
	// GTK uses only a single encoding, and that is UTF-8.
	ಯದಿ (STRCMP(p_tenc, "utf-8") != 0)
	    errmsg = e_cannot_be_changed_in_gtk_GUI;
    }
#endif

    ಯದಿ (errmsg == ರಿಕ್ತ)
    {
#ifdef FEAT_KEYMAP
	// When 'keymap' is used and 'encoding' changes, reload the keymap
	// (with another encoding).
	ಯದಿ (varp == &p_enc && *curbuf->b_p_keymap != NUL)
	    (ಶೂನ್ಯ)keymap_init();
#endif

	// When 'termencoding' is not empty and 'encoding' changes or when
	// 'termencoding' changes, need to setup for keyboard input and
	// display output conversion.
	ಯದಿ (((varp == &p_enc && *p_tenc != NUL) || varp == &p_tenc))
	{
	    ಯದಿ (convert_setup(&ಒಳಹರಿವು_conv, p_tenc, p_enc) == FAIL
		    || convert_setup(&output_conv, p_enc, p_tenc) == FAIL)
	    {
		semsg(_(e_cannot_convert_between_str_and_str),
			p_tenc, p_enc);
		errmsg = e_invalid_argument;
	    }
	}

#ಯದಿ defined(MSWIN)
	// $HOME, $VIM and $VIMRUNTIME may have characters in active code page.
	ಯದಿ (varp == &p_enc)
	{
	    init_homedir();
	    init_vimdir();
	}
#endif
    }

    ಪ್ರತ್ಯಾಗಮ errmsg;
}

    ಪೂರ್ಣಾಙ್ಕ
expand_set_encoding(optexpand_T *args, ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಪ್ರತ್ಯಾಗಮ expand_set_opt_generic(
	    args,
	    get_encoding_name,
	    ಅಂಕೆ_Matches,
	    matches);
}

/*
 * The 'eventignore(win)' option is changed.
 */
    ವರ್ಣ *
did_set_eventignore(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u	*oldval = args->os_oldval.string;

    ಯದಿ (check_ei(*varp) == FAIL)
	ಪ್ರತ್ಯಾಗಮ e_invalid_argument;

    ಯದಿ (oldval == ರಿಕ್ತ || STRCMP(oldval, *varp) == 0)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    // Deal with the events that are triggered by comparing against a stored
    // state, with the old value in effect: what happened while an event was
    // ignored must not be reported once it is not ignored anymore, and what
    // happened before must still be reported.
    // Use a copy, the caller owns "oldval" and autocommands may free it.
    char_u	*save_ei = vim_strsave(oldval);
    ಯದಿ (save_ei != ರಿಕ್ತ)
    {
	char_u	*newval = *varp;
	win_T	*wp = is_window_local_option(args->os_idx) ? curwin : ರಿಕ್ತ;

	*varp = save_ei;
	// "varp" points into "wp" for 'eventignorewin'.
	ಯದಿ (wp != ರಿಕ್ತ)
	    ++wp->w_locked;
	may_trigger_deferred_events();
	ಯದಿ (wp != ರಿಕ್ತ)
	    --wp->w_locked;

	free_string_option(*varp);
	*varp = newval;
    }

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ expand_eiw = FALSE;

    ಸ್ಥಿರ char_u *
get_eventignore_name(expand_T *xp, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಪೂರ್ಣಾಙ್ಕ subtract = *xp->xp_pattern == '-';
    // 'eventignore(win)' allows special keyword "all" in addition to
    // all event names.
    ಯದಿ (!subtract && idx == 0)
	ಪ್ರತ್ಯಾಗಮ (char_u *)"all";

    char_u *name = get_event_name_no_group(xp, idx - 1 + subtract, expand_eiw);
    ಯದಿ (name == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    sprintf((ವರ್ಣ *)IObuff, "%s%s", subtract ? "-" : "", name);
    ಪ್ರತ್ಯಾಗಮ IObuff;
}

    ಪೂರ್ಣಾಙ್ಕ
expand_set_eventignore(optexpand_T *args, ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    expand_eiw = args->oe_varp != (char_u *)&p_ei;
    ಪ್ರತ್ಯಾಗಮ expand_set_opt_generic(
	    args,
	    get_eventignore_name,
	    ಅಂಕೆ_Matches,
	    matches);
}

/*
 * The 'fileformat' option is changed.
 */
    ವರ್ಣ *
did_set_fileformat(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    ಯದಿ (!curbuf->b_p_ma && !(args->os_flags & OPT_GLOBAL))
	ಪ್ರತ್ಯಾಗಮ e_cannot_make_changes_modifiable_is_off;
    ಅನ್ಯಥಾ ಯದಿ (check_opt_strings(*varp, p_ff_ಮೌಲ್ಯಗಳು, FALSE) != OK)
	ಪ್ರತ್ಯಾಗಮ e_invalid_argument;

    // may also change 'textmode'
    ಯದಿ (get_fileformat(curbuf) == EOL_DOS)
	curbuf->b_p_tx = TRUE;
    ಅನ್ಯಥಾ
	curbuf->b_p_tx = FALSE;
    redraw_titles();
    // update flag in swap file
    ml_setflags(curbuf);
    // Redraw needed when switching to/from "mac": a CR in the text
    // will be displayed differently.
    ಯದಿ (get_fileformat(curbuf) == EOL_MAC || *args->os_oldval.string == 'm')
	redraw_curbuf_later(UPD_NOT_VALID);

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

    ಪೂರ್ಣಾಙ್ಕ
expand_set_fileformat(optexpand_T *args, ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಪ್ರತ್ಯಾಗಮ expand_set_opt_string(
	    args,
	    p_ff_ಮೌಲ್ಯಗಳು,
	    ARRAY_LENGTH(p_ff_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * fileformat options.
 */
    char_u *
get_fileformat_name(expand_T *xp UNUSED, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಯದಿ (idx >= (ಪೂರ್ಣಾಙ್ಕ)ARRAY_LENGTH(p_ff_ಮೌಲ್ಯಗಳು))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಪ್ರತ್ಯಾಗಮ (char_u*)p_ff_ಮೌಲ್ಯಗಳು[idx];
}

/*
 * The 'fileformats' option is changed.
 */
    ವರ್ಣ *
did_set_fileformats(optset_T *args UNUSED)
{
    ಯದಿ (check_opt_strings(p_ffs, p_ff_ಮೌಲ್ಯಗಳು, TRUE) != OK)
	ಪ್ರತ್ಯಾಗಮ e_invalid_argument;

    // also change 'textauto'
    ಯದಿ (*p_ffs == NUL)
	p_ta = FALSE;
    ಅನ್ಯಥಾ
	p_ta = TRUE;

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * The 'filetype' or the 'syntax' option is changed.
 */
    ವರ್ಣ *
did_set_filetype_or_syntax(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    ಯದಿ (!valid_filetype(*varp))
	ಪ್ರತ್ಯಾಗಮ e_invalid_argument;

    args->os_ಮೌಲ್ಯ_changed = STRCMP(args->os_oldval.string, *varp) != 0;

    // Since we check the value, there is no need to set P_INSECURE,
    // even when the value comes from a modeline.
    args->os_ಮೌಲ್ಯ_checked = TRUE;

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

#ಯದಿ defined(FEAT_FOLDING)
/*
 * The 'foldclose' option is changed.
 */
    ವರ್ಣ *
did_set_foldclose(optset_T *args UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ did_set_opt_strings(p_fcl, p_fcl_ಮೌಲ್ಯಗಳು, TRUE);
}

    ಪೂರ್ಣಾಙ್ಕ
expand_set_foldclose(optexpand_T *args, ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಪ್ರತ್ಯಾಗಮ expand_set_opt_string(
	    args,
	    p_fcl_ಮೌಲ್ಯಗಳು,
	    ARRAY_LENGTH(p_fcl_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}
#endif

#ಯದಿ defined(FEAT_EVAL) && defined(FEAT_FOLDING)
/*
 * The 'foldexpr' option is changed.
 */
    ವರ್ಣ *
did_set_foldexpr(optset_T *args)
{
    (ಶೂನ್ಯ)did_set_optexpr(args);
    ಯದಿ (foldmethod_Is_ಅಭಿವ್ಯಕ್ತಿ(curwin))
	foldUpdateAll(curwin);
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}
#endif

#ಯದಿ defined(FEAT_FOLDING)
/*
 * The 'foldignore' option is changed.
 */
    ವರ್ಣ *
did_set_foldignore(optset_T *args UNUSED)
{
    ಯದಿ (foldmethodIsIndent(curwin))
	foldUpdateAll(curwin);
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * The 'foldmarker' option is changed.
 */
    ವರ್ಣ *
did_set_foldmarker(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u	*p;

    p = vim_strchr(*varp, ',');
    ಯದಿ (p == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ e_comma_required;
    ಅನ್ಯಥಾ ಯದಿ (p == *varp || p[1] == NUL)
	ಪ್ರತ್ಯಾಗಮ e_invalid_argument;
    ಅನ್ಯಥಾ ಯದಿ (foldmethodIsMarker(curwin))
	foldUpdateAll(curwin);

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * The 'foldmethod' option is changed.
 */
    ವರ್ಣ *
did_set_foldmethod(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    ಯದಿ (check_opt_strings(*varp, p_fdm_ಮೌಲ್ಯಗಳು, FALSE) != OK || **varp == NUL)
	ಪ್ರತ್ಯಾಗಮ e_invalid_argument;

    foldUpdateAll(curwin);
    ಯದಿ (foldmethodIsDiff(curwin))
	newFoldLevel();
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

    ಪೂರ್ಣಾಙ್ಕ
expand_set_foldmethod(optexpand_T *args, ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಪ್ರತ್ಯಾಗಮ expand_set_opt_string(
	    args,
	    p_fdm_ಮೌಲ್ಯಗಳು,
	    ARRAY_LENGTH(p_fdm_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}

/*
 * The 'foldopen' option is changed.
 */
    ವರ್ಣ *
did_set_foldopen(optset_T *args UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ did_set_opt_flags(p_fdo, p_fdo_ಮೌಲ್ಯಗಳು, &fdo_flags, TRUE);
}

    ಪೂರ್ಣಾಙ್ಕ
expand_set_foldopen(optexpand_T *args, ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಪ್ರತ್ಯಾಗಮ expand_set_opt_string(
	    args,
	    p_fdo_ಮೌಲ್ಯಗಳು,
	    ARRAY_LENGTH(p_fdo_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}
#endif

/*
 * The 'formatoptions' option is changed.
 */
    ವರ್ಣ *
did_set_formatoptions(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    ಪ್ರತ್ಯಾಗಮ did_set_option_listflag(*varp, (char_u *)FO_ALL, args->os_errbuf,
		    args->os_errbuflen);
}

    ಪೂರ್ಣಾಙ್ಕ
expand_set_formatoptions(optexpand_T *args, ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಪ್ರತ್ಯಾಗಮ expand_set_opt_listflag(args, (char_u*)FO_ALL, ಅಂಕೆ_Matches, matches);
}

#ಯದಿ defined(CURSOR_SHAPE)
/*
 * The 'guicursor' option is changed.
 */
    ವರ್ಣ *
did_set_guicursor(optset_T *args UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ parse_shape_opt(SHAPE_CURSOR);
}
#endif

#ಯದಿ defined(FEAT_GUI)
/*
 * The 'guifont' option is changed.
 */
    ವರ್ಣ *
did_set_guifont(optset_T *args UNUSED)
{
    char_u	*p;
    ವರ್ಣ	*errmsg = ರಿಕ್ತ;

    ಯದಿ (gui.in_use)
    {
	p = p_guifont;
# ಯದಿ defined(FEAT_GUI_GTK)
	// Put up a font dialog and let the user select a new value.
	// If this is cancelled go back to the old value but don't
	// give an error message.
	ಯದಿ (STRCMP(p, "*") == 0)
	{
	    p = gui_mch_font_dialog(args->os_oldval.string);
	    free_string_option(p_guifont);
	    p_guifont = (p != ರಿಕ್ತ) ? p : vim_strsave(args->os_oldval.string);
	}
# endif
	ಯದಿ (p != ರಿಕ್ತ && gui_init_font(p_guifont, FALSE) != OK)
	{
# ಯದಿ defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_PHOTON)
	    ಯದಿ (STRCMP(p_guifont, "*") == 0)
	    {
		// Dialog was cancelled: Keep the old value without giving
		// an error message.
		free_string_option(p_guifont);
		p_guifont = vim_strsave(args->os_oldval.string);
	    }
	    ಅನ್ಯಥಾ
# endif
		errmsg = e_invalid_fonts;
	}
    }

    ಪ್ರತ್ಯಾಗಮ errmsg;
}

/*
 * Expand the 'guifont' option. Only when GUI is being used. Each platform has
 * specific behaviors.
 */
    ಪೂರ್ಣಾಙ್ಕ
expand_set_guifont(optexpand_T *args, ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಯದಿ (!gui.in_use)
	ಪ್ರತ್ಯಾಗಮ FAIL;

# ಯದಿ defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
    char_u **varp = (char_u **)args->oe_varp;
    ಪೂರ್ಣಾಙ್ಕ wide = (varp == &p_guifontwide);

    ಪ್ರತ್ಯಾಗಮ expand_set_opt_callback(
	    args, gui_mch_expand_font, &wide, ಅಂಕೆ_Matches, matches);
# ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ FAIL;
# endif
}

# ಯದಿ defined(FEAT_XFONTSET)
/*
 * The 'guifontset' option is changed.
 */
    ವರ್ಣ *
did_set_guifontset(optset_T *args UNUSED)
{
    ವರ್ಣ *errmsg = ರಿಕ್ತ;

    ಯದಿ (STRCMP(p_guifontset, "*") == 0)
	errmsg = e_cant_select_fontset;
    ಅನ್ಯಥಾ ಯದಿ (gui.in_use && gui_init_font(p_guifontset, TRUE) != OK)
	errmsg = e_invalid_fontset;

    ಪ್ರತ್ಯಾಗಮ errmsg;
}
# endif

/*
 * The 'guifontwide' option is changed.
 */
    ವರ್ಣ *
did_set_guifontwide(optset_T *args UNUSED)
{
    ವರ್ಣ *errmsg = ರಿಕ್ತ;

    ಯದಿ (STRCMP(p_guifontwide, "*") == 0)
	errmsg = e_cant_select_wide_font;
    ಅನ್ಯಥಾ ಯದಿ (gui_get_wide_font() == FAIL)
	errmsg = e_invalid_wide_font;

    ಪ್ರತ್ಯಾಗಮ errmsg;
}
#endif

#ಯದಿ defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
/*
 * The 'guiligatures' option is changed.
 */
    ವರ್ಣ *
did_set_guiligatures(optset_T *args UNUSED)
{
    gui_set_ligatures();
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}
#endif

#ಯದಿ defined(FEAT_GUI)
/*
 * The 'guioptions' option is changed.
 */
    ವರ್ಣ *
did_set_guioptions(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    ವರ್ಣ *errmsg;

    errmsg = did_set_option_listflag(*varp, (char_u *)GO_ALL, args->os_errbuf,
		    args->os_errbuflen);
    ಯದಿ (errmsg != ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ errmsg;

    gui_init_which_components(args->os_oldval.string);
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

    ಪೂರ್ಣಾಙ್ಕ
expand_set_guioptions(optexpand_T *args, ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಪ್ರತ್ಯಾಗಮ expand_set_opt_listflag(args, (char_u*)GO_ALL, ಅಂಕೆ_Matches, matches);
}
#endif

#ಯದಿ defined(FEAT_GUI_TABLINE)
/*
 * The 'guitablabel' option is changed.
 */
    ವರ್ಣ *
did_set_guitablabel(optset_T *args UNUSED)
{
    redraw_tabline = TRUE;
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}
#endif

/*
 * The 'helpfile' option is changed.
 */
    ವರ್ಣ *
did_set_helpfile(optset_T *args UNUSED)
{
    // May compute new values for $VIM and $VIMRUNTIME
    ಯದಿ (didset_vim)
	vim_unsetenv_ext((char_u *)"VIM");
    ಯದಿ (didset_vimruntime)
	vim_unsetenv_ext((char_u *)"VIMRUNTIME");
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

#ಯದಿ defined(FEAT_MULTI_LANG)
/*
 * The 'helplang' option is changed.
 */
    ವರ್ಣ *
did_set_helplang(optset_T *args UNUSED)
{
    ವರ್ಣ *errmsg = ರಿಕ್ತ;

    // Check for "", "ab", "ab,cd", etc.
    ಪ್ರತಿ (char_u *s = p_hlg; *s != NUL; s += 3)
    {
	ಯದಿ (s[1] == NUL || ((s[2] != ',' || s[3] == NUL) && s[2] != NUL))
	{
	    errmsg = e_invalid_argument;
	    ಭಙ್ಗ;
	}
	ಯದಿ (s[2] == NUL)
	    ಭಙ್ಗ;
    }

    ಪ್ರತ್ಯಾಗಮ errmsg;
}
#endif

/*
 * The 'highlight' option is changed.
 */
    ವರ್ಣ *
did_set_highlight(optset_T *args UNUSED)
{
    ಯದಿ (highlight_changed() == FAIL)
	ಪ್ರತ್ಯಾಗಮ e_invalid_argument;	// invalid flags

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
expand_hl_occasions(
	optexpand_T *args,
	ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_Matches,
	char_u ***matches,
	char_u prefix)
{
    char_u	    *p;
    ಸ್ಥಿರ char_u   hl_flags[HLF_ಎಣಿಕೆ] = HL_FLAGS;
    size_t	    i;
    ಪೂರ್ಣಾಙ್ಕ		    ಎಣಿಕೆ = 0;

    *matches = ALLOC_MULT(char_u *, HLF_ಎಣಿಕೆ + 1);
    ಯದಿ (*matches == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    // We still want to return the full option if it's requested.
    ಯದಿ (args->oe_include_orig_val)
    {
	p = vim_strsave(args->oe_opt_ಮೌಲ್ಯ);
	ಯದಿ (p == ರಿಕ್ತ)
	{
	    VIM_CLEAR(*matches);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	(*matches)[ಎಣಿಕೆ++] = p;
    }

    ಪ್ರತಿ (i = 0; i < HLF_ಎಣಿಕೆ; i++)
    {
	p = alloc((prefix == NUL ? 1 : 2) + 1);
	ಯದಿ (p == ರಿಕ್ತ)
	{
	    ಯದಿ (ಎಣಿಕೆ == 0)
	    {
		VIM_CLEAR(*matches);
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	    ಅನ್ಯಥಾ
		ಭಙ್ಗ;
	}
	ಯದಿ (prefix == NUL)
	    sprintf((ವರ್ಣ *)p, "%c", hl_flags[i]);
	ಅನ್ಯಥಾ
	    sprintf((ವರ್ಣ *)p, "%c%c", prefix, hl_flags[i]);
	(*matches)[ಎಣಿಕೆ++] = p;
    }

    ಯದಿ (ಎಣಿಕೆ == 0)
    {
	VIM_CLEAR(*matches);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    *ಅಂಕೆ_Matches = ಎಣಿಕೆ;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Expand 'highlight' option.
 */
    ಪೂರ್ಣಾಙ್ಕ
expand_set_highlight(optexpand_T *args, ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    char_u	    *p;
    expand_T	    *xp = args->oe_xp;
    size_t	    i;
    ಪೂರ್ಣಾಙ್ಕ		    ಎಣಿಕೆ = 0;

    ಯದಿ (xp->xp_pattern > args->oe_set_arg && *(xp->xp_pattern-1) == ':')
    {
	// Right after a ':', meaning we just return all highlight names.
	ಪ್ರತ್ಯಾಗಮ expand_set_opt_generic(
		args,
		get_highlight_name,
		ಅಂಕೆ_Matches,
		matches);
    }

    ಯದಿ (*xp->xp_pattern == NUL)
	// At beginning of a comma-separated list. Return the specific list of
	// supported occasions.
	ಪ್ರತ್ಯಾಗಮ expand_hl_occasions(args, ಅಂಕೆ_Matches, matches, NUL);

    // We are after the initial character (which indicates the occasion). We
    // already made sure we are not matching after a ':' above, so now we want
    // to match against display mode modifiers.
    // Since the xp_pattern starts from the beginning, we need to include it in
    // the returned match.

    // Note: Keep this in sync with highlight_changed()
    ಸ್ಥಿರ char_u p_hl_mode_ಮೌಲ್ಯಗಳು[] =
	{':', 'b', 'i', '-', 'n', 'r', 's', 'u', 'c', '2', 'd', '=', 't'};
    size_t ಅಂಕೆ_hl_modes = ARRAY_LENGTH(p_hl_mode_ಮೌಲ್ಯಗಳು);

    *matches = ALLOC_MULT(char_u *, ಅಂಕೆ_hl_modes);
    ಯದಿ (*matches == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಪೂರ್ಣಾಙ್ಕ pattern_len = xp->xp_pattern_len;

    ಪ್ರತಿ (i = 0; i < ಅಂಕೆ_hl_modes; i++)
    {
	// Don't allow duplicates as these are unique flags
	char_u *dup = vim_strchr(xp->xp_pattern + 1, p_hl_mode_ಮೌಲ್ಯಗಳು[i]);
	ಯದಿ (dup != ರಿಕ್ತ && (ಪೂರ್ಣಾಙ್ಕ)(dup - xp->xp_pattern) < pattern_len)
	    ಅನುವರ್ತ;

	// ':' only works by itself, not with other flags.
	ಯದಿ (pattern_len > 1 && p_hl_mode_ಮೌಲ್ಯಗಳು[i] == ':')
	    ಅನುವರ್ತ;

	p = vim_strnsave(xp->xp_pattern, pattern_len + 1);
	ಯದಿ (p == ರಿಕ್ತ)
	{
	    ಯದಿ (i == 0)
	    {
		VIM_CLEAR(*matches);
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	    ಅನ್ಯಥಾ
		ಭಙ್ಗ;
	}
	p[pattern_len] = p_hl_mode_ಮೌಲ್ಯಗಳು[i];
	p[pattern_len + 1] = NUL;
	(*matches)[ಎಣಿಕೆ++] = p;
    }
    ಯದಿ (ಎಣಿಕೆ == 0)
    {
	VIM_CLEAR(*matches);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    *ಅಂಕೆ_Matches = ಎಣಿಕೆ;

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * The 'titlestring' or the 'iconstring' option is changed.
 */
    ಸ್ಥಿರ ವರ್ಣ *
parse_titleiconstring(optset_T *args UNUSED, ಪೂರ್ಣಾಙ್ಕ flagval UNUSED)
{
#ifdef FEAT_STL_OPT
    char_u	**varp = (char_u **)args->os_varp;

    // NULL => statusline syntax
    ಯದಿ (vim_strchr(*varp, '%') && check_stl_option(*varp) == ರಿಕ್ತ)
	stl_syntax |= flagval;
    ಅನ್ಯಥಾ
	stl_syntax &= ~flagval;
#endif
    did_set_title();

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * The 'iconstring' option is changed.
 */
    ವರ್ಣ *
did_set_iconstring(optset_T *args)
{
    ಪೂರ್ಣಾಙ್ಕ flagval = 0;

#ifdef FEAT_STL_OPT
    flagval = STL_IN_ICON;
#endif

    ಪ್ರತ್ಯಾಗಮ parse_titleiconstring(args, flagval);
}

#ಯದಿ defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
/*
 * The 'imactivatekey' option is changed.
 */
    ವರ್ಣ *
did_set_imactivatekey(optset_T *args UNUSED)
{
    ಯದಿ (!im_xim_isvalid_imactivate())
	ಪ್ರತ್ಯಾಗಮ e_invalid_argument;
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}
#endif

#ifdef FEAT_IMAGE
/*
 * The 'imageprotocol' option is changed.
 */
    ವರ್ಣ *
did_set_imageprotocol(optset_T *args UNUSED)
{
    ಯದಿ (update_image_backend() == FAIL)
	ಪ್ರತ್ಯಾಗಮ e_invalid_argument;
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

    ಪೂರ್ಣಾಙ್ಕ
expand_set_imageprotocol(optexpand_T *args, ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    expand_T *xp = args->oe_xp;

    ಯದಿ (xp->xp_pattern > args->oe_set_arg && *(xp->xp_pattern-1) == ':')
    {
	// 'imageprotocol' only has well-defined terms for completion for the
	// protocol part after the colon.
	ಪ್ರತ್ಯಾಗಮ expand_set_opt_string(
		args,
		p_ipc_protocol_ಮೌಲ್ಯಗಳು,
		ARRAY_LENGTH(p_ipc_protocol_ಮೌಲ್ಯಗಳು) - 1,
		ಅಂಕೆ_Matches,
		matches);
    }
    // Use expand_set_opt_string instead of returning FAIL so that we can
    // include the original value if args->oe_include_orig_val is set.
    ಸ್ಥಿರ ವರ್ಣ *(empty[]) = {ರಿಕ್ತ};
    ಪ್ರತ್ಯಾಗಮ expand_set_opt_string(args, empty, 0, ಅಂಕೆ_Matches, matches);
}
#endif

/*
 * The 'iskeyword' option is changed.
 */
    ವರ್ಣ *
did_set_iskeyword(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    ಯದಿ (varp == &p_isk)		// only check for global-value
    {
	ಯದಿ (check_isopt(*varp) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ e_invalid_argument;
    }
    ಅನ್ಯಥಾ			// fallthrough for local-value
	ಪ್ರತ್ಯಾಗಮ did_set_isopt(args);

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * The 'isident' or the 'iskeyword' or the 'isprint' or the 'isfname' option is
 * changed.
 */
    ವರ್ಣ *
did_set_isopt(optset_T *args)
{
    // 'isident', 'iskeyword', 'isprint' or 'isfname' option: refill g_chartab[]
    // If the new option is invalid, use old value.
    // 'lisp' option: refill g_chartab[] for '-' char.
    ಯದಿ (init_chartab() == FAIL)
    {
	args->os_restore_chartab = TRUE;// need to restore the chartab.
	ಪ್ರತ್ಯಾಗಮ e_invalid_argument;	// error in value
    }

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * The 'jumpoptions' option is changed.
 */
    ವರ್ಣ *
did_set_jumpoptions(optset_T *args UNUSED)
{
    ಯದಿ (opt_strings_flags(p_jop, p_jop_ಮೌಲ್ಯಗಳು, &jop_flags, TRUE) != OK)
	ಪ್ರತ್ಯಾಗಮ e_invalid_argument;

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

    ಪೂರ್ಣಾಙ್ಕ
expand_set_jumpoptions(optexpand_T *args, ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಪ್ರತ್ಯಾಗಮ expand_set_opt_string(
	    args,
	    p_jop_ಮೌಲ್ಯಗಳು,
	    ARRAY_LENGTH(p_jop_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}

#ಯದಿ defined(FEAT_KEYMAP)
/*
 * The 'keymap' option is changed.
 */
    ವರ್ಣ *
did_set_keymap(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    ವರ್ಣ *errmsg = ರಿಕ್ತ;

    ಯದಿ (!valid_filetype(*varp))
	errmsg = e_invalid_argument;
    ಅನ್ಯಥಾ
    {
	ಪೂರ್ಣಾಙ್ಕ	    secure_save = secure;

	// Reset the secure flag, since the value of 'keymap' has
	// been checked to be safe.
	secure = 0;

	// load or unload key mapping tables
	errmsg = keymap_init();

	secure = secure_save;

	// Since we check the value, there is no need to set P_INSECURE,
	// even when the value comes from a modeline.
	args->os_ಮೌಲ್ಯ_checked = TRUE;
    }

    ಯದಿ (errmsg == ರಿಕ್ತ)
    {
	ಯದಿ (*curbuf->b_p_keymap != NUL)
	{
	    // Installed a new keymap, switch on using it.
	    curbuf->b_p_iminsert = B_IMODE_LMAP;
	    ಯದಿ (curbuf->b_p_imsearch != B_IMODE_USE_ಸೇರಿಸು)
		curbuf->b_p_imsearch = B_IMODE_LMAP;
	}
	ಅನ್ಯಥಾ
	{
	    // Cleared the keymap, may reset 'iminsert' and 'imsearch'.
	    ಯದಿ (curbuf->b_p_iminsert == B_IMODE_LMAP)
		curbuf->b_p_iminsert = B_IMODE_NONE;
	    ಯದಿ (curbuf->b_p_imsearch == B_IMODE_LMAP)
		curbuf->b_p_imsearch = B_IMODE_USE_ಸೇರಿಸು;
	}
	ಯದಿ ((args->os_flags & OPT_LOCAL) == 0)
	{
	    set_iminsert_global();
	    set_imsearch_global();
	}
	status_redraw_curbuf();
    }

    ಪ್ರತ್ಯಾಗಮ errmsg;
}
#endif

/*
 * The 'keymodel' option is changed.
 */
    ವರ್ಣ *
did_set_keymodel(optset_T *args UNUSED)
{
    ಯದಿ (check_opt_strings(p_km, p_km_ಮೌಲ್ಯಗಳು, TRUE) != OK)
	ಪ್ರತ್ಯಾಗಮ e_invalid_argument;

    km_stopsel = (vim_strchr(p_km, 'o') != ರಿಕ್ತ);
    km_startsel = (vim_strchr(p_km, 'a') != ರಿಕ್ತ);
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

    ಪೂರ್ಣಾಙ್ಕ
expand_set_keymodel(optexpand_T *args, ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಪ್ರತ್ಯಾಗಮ expand_set_opt_string(
	    args,
	    p_km_ಮೌಲ್ಯಗಳು,
	    ARRAY_LENGTH(p_km_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}

/*
 * The 'keyprotocol' option is changed.
 */
    ವರ್ಣ *
did_set_keyprotocol(optset_T *args UNUSED)
{
    char_u *term = T_NAME;
    keyprot_T kpc = match_keyprotocol(term);
    ಯದಿ (kpc == KEYPROTOCOL_FAIL)
	ಪ್ರತ್ಯಾಗಮ e_invalid_argument;

    apply_keyprotocol(term, kpc);

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

    ಪೂರ್ಣಾಙ್ಕ
expand_set_keyprotocol(optexpand_T *args, ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    expand_T *xp = args->oe_xp;
    ಯದಿ (xp->xp_pattern > args->oe_set_arg && *(xp->xp_pattern-1) == ':')
    {
	// 'keyprotocol' only has well-defined terms for completion for the
	// protocol part after the colon.
	ಪ್ರತ್ಯಾಗಮ expand_set_opt_string(
		args,
		p_kpc_protocol_ಮೌಲ್ಯಗಳು,
		ARRAY_LENGTH(p_kpc_protocol_ಮೌಲ್ಯಗಳು) - 1,
		ಅಂಕೆ_Matches,
		matches);
    }
    // Use expand_set_opt_string instead of returning FAIL so that we can
    // include the original value if args->oe_include_orig_val is set.
    ಸ್ಥಿರ ವರ್ಣ *(empty[]) = {ರಿಕ್ತ};
    ಪ್ರತ್ಯಾಗಮ expand_set_opt_string(args, empty, 0, ಅಂಕೆ_Matches, matches);
}

/*
 * The 'lispoptions' option is changed.
 */
    ವರ್ಣ *
did_set_lispoptions(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    ಯದಿ (**varp != NUL
	    && STRCMP(*varp, "expr:0") != 0
	    && STRCMP(*varp, "expr:1") != 0)
	ಪ್ರತ್ಯಾಗಮ e_invalid_argument;

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

    ಪೂರ್ಣಾಙ್ಕ
expand_set_lispoptions(optexpand_T *args, ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಸ್ಥಿರ ವರ್ಣ *(p_lop_ಮೌಲ್ಯಗಳು[]) = {"expr:0", "expr:1", ರಿಕ್ತ};
    ಪ್ರತ್ಯಾಗಮ expand_set_opt_string(
	    args,
	    p_lop_ಮೌಲ್ಯಗಳು,
	    ARRAY_LENGTH(p_lop_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}

/*
 * The 'matchpairs' option is changed.
 */
    ವರ್ಣ *
did_set_matchpairs(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u	*p;

    ಯದಿ (has_mbyte)
    {
	ಪ್ರತಿ (p = *varp; *p != NUL; ++p)
	{
	    ಪೂರ್ಣಾಙ್ಕ x2 = -1;
	    ಪೂರ್ಣಾಙ್ಕ x3 = -1;

	    p += mb_ptr2len(p);
	    ಯದಿ (*p != NUL)
		x2 = *p++;
	    ಯದಿ (*p != NUL)
	    {
		x3 = mb_ptr2char(p);
		p += mb_ptr2len(p);
	    }
	    ಯದಿ (x2 != ':' || x3 == -1 || (*p != NUL && *p != ','))
		ಪ್ರತ್ಯಾಗಮ e_invalid_argument;
	    ಯದಿ (*p == NUL)
		ಭಙ್ಗ;
	}
    }
    ಅನ್ಯಥಾ
    {
	// Check for "x:y,x:y"
	ಪ್ರತಿ (p = *varp; *p != NUL; p += 4)
	{
	    ಯದಿ (p[1] != ':' || p[2] == NUL || (p[3] != NUL && p[3] != ','))
		ಪ್ರತ್ಯಾಗಮ e_invalid_argument;
	    ಯದಿ (p[3] == NUL)
		ಭಙ್ಗ;
	}
    }

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Process the updated 'messagesopt' option value.
 */
    ವರ್ಣ *
did_set_messagesopt(optset_T *args UNUSED)
{
    ಯದಿ (messagesopt_changed() == FAIL)
	ಪ್ರತ್ಯಾಗಮ e_invalid_argument;

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

    ಪೂರ್ಣಾಙ್ಕ
expand_set_messagesopt(optexpand_T *args, ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಸ್ಥಿರ ವರ್ಣ *(p_mopt_ಮೌಲ್ಯಗಳು[]) = {"hit-enter", "wait:", "history:", ರಿಕ್ತ};
    ಪ್ರತ್ಯಾಗಮ expand_set_opt_string(
	    args,
	    p_mopt_ಮೌಲ್ಯಗಳು,
	    ARRAY_LENGTH(p_mopt_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}

#ಯದಿ defined(FEAT_SPELL)
/*
 * The 'mkspellmem' option is changed.
 */
    ವರ್ಣ *
did_set_mkspellmem(optset_T *args UNUSED)
{
    ಯದಿ (spell_check_msm() != OK)
	ಪ್ರತ್ಯಾಗಮ e_invalid_argument;

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}
#endif

/*
 * The 'mouse' option is changed.
 */
    ವರ್ಣ *
did_set_mouse(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    ವರ್ಣ	*retval;

    retval = did_set_option_listflag(*varp, (char_u *)MOUSE_ALL, args->os_errbuf,
		    args->os_errbuflen);
    ಯದಿ (retval == ರಿಕ್ತ)
    {
	redraw_tabline = TRUE;
	ಯದಿ (tabline_height() > 0)
	    update_screen(UPD_VALID);
#ಯದಿ defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
	popup_close_info(); // Close info popup to apply new properties
#endif
    }
    ಪ್ರತ್ಯಾಗಮ retval;
}

    ಪೂರ್ಣಾಙ್ಕ
expand_set_mouse(optexpand_T *args, ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಪ್ರತ್ಯಾಗಮ expand_set_opt_listflag(args, (char_u*)MOUSE_ALL, ಅಂಕೆ_Matches, matches);
}

/*
 * The 'mousemodel' option is changed.
 */
    ವರ್ಣ *
did_set_mousemodel(optset_T *args UNUSED)
{
    ಯದಿ (check_opt_strings(p_mousem, p_mousem_ಮೌಲ್ಯಗಳು, FALSE) != OK)
	ಪ್ರತ್ಯಾಗಮ e_invalid_argument;
#ಯದಿ defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU) && (XmVersion <= 1002)
    ಅನ್ಯಥಾ ಯದಿ (*p_mousem != *args->os_oldval.string)
	// Changed from "extend" to "popup" or "popup_setpos" or vv: need
	// to create or delete the popup menus.
	gui_motif_update_mousemodel(ಬೇರು_menu);
#endif

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

    ಪೂರ್ಣಾಙ್ಕ
expand_set_mousemodel(optexpand_T *args, ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಪ್ರತ್ಯಾಗಮ expand_set_opt_string(
	    args,
	    p_mousem_ಮೌಲ್ಯಗಳು,
	    ARRAY_LENGTH(p_mousem_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}

#ಯದಿ defined(FEAT_MOUSESHAPE)
    ವರ್ಣ *
did_set_mouseshape(optset_T *args UNUSED)
{
    ವರ್ಣ *errmsg = ರಿಕ್ತ;

    errmsg = parse_shape_opt(SHAPE_MOUSE);
    update_mouseshape(-1);

    ಪ್ರತ್ಯಾಗಮ errmsg;
}
#endif

/*
 * The 'nrformats' option is changed.
 */
    ವರ್ಣ *
did_set_nrformats(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    ಪ್ರತ್ಯಾಗಮ did_set_opt_strings(*varp, p_nf_ಮೌಲ್ಯಗಳು, TRUE);
}

    ಪೂರ್ಣಾಙ್ಕ
expand_set_nrformats(optexpand_T *args, ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಪ್ರತ್ಯಾಗಮ expand_set_opt_string(
	    args,
	    p_nf_ಮೌಲ್ಯಗಳು,
	    ARRAY_LENGTH(p_nf_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}

#ಯದಿ defined(FEAT_EVAL)
/*
 * One of the '*expr' options is changed: 'balloonexpr', 'diffexpr',
 * 'foldexpr', 'foldtext', 'formatexpr', 'includeexpr', 'indentexpr',
 * 'patchexpr', 'printexpr' and 'charconvert'.
 */
    ವರ್ಣ *
did_set_optexpr(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    // If the option value starts with <SID> or s:, then replace that with
    // the script identifier.
    char_u *name = get_scriptlocal_funcname(*varp);
    ಯದಿ (name != ರಿಕ್ತ)
    {
	free_string_option(*varp);
	*varp = name;
    }

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}
#endif

/*
 * The 'pastetoggle' option is changed.
 */
    ವರ್ಣ *
did_set_pastetoggle(optset_T *args UNUSED)
{
    char_u	*p;

    // translate key codes like in a mapping
    ಯದಿ (*p_pt)
    {
	(ಶೂನ್ಯ)replace_termcodes(p_pt, &p, 0,
		REPTERM_FROM_PART | REPTERM_DO_LT, ರಿಕ್ತ);
	ಯದಿ (p != ರಿಕ್ತ)
	{
	    free_string_option(p_pt);
	    p_pt = p;
	}
    }

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

#ಯದಿ defined(FEAT_PROP_POPUP)
/*
 * The 'previewpopup' option is changed.
 */
    ವರ್ಣ *
did_set_previewpopup(optset_T *args UNUSED)
{
    ಯದಿ (parse_previewpopup(ರಿಕ್ತ) == FAIL)
	ಪ್ರತ್ಯಾಗಮ e_invalid_argument;

# ಯದಿ defined(FEAT_QUICKFIX)
    popup_close_info();
# endif
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
expand_set_popupoption(optexpand_T *args, ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_Matches, char_u ***matches,
	ಪೂರ್ಣಾಙ್ಕ previewpopup)
{
    expand_T *xp = args->oe_xp;

    ಯದಿ (xp->xp_pattern > args->oe_set_arg && *(xp->xp_pattern-1) == ':')
    {
	ಯದಿ (completing_ಮೌಲ್ಯ_for_subopt(args, "close")
		|| completing_ಮೌಲ್ಯ_for_subopt(args, "resize")
		|| completing_ಮೌಲ್ಯ_for_subopt(args, "shadow"))
	{
	    ಪ್ರತ್ಯಾಗಮ expand_set_opt_string(
		    args,
		    p_popup_option_on_off_ಮೌಲ್ಯಗಳು,
		    ARRAY_LENGTH(p_popup_option_on_off_ಮೌಲ್ಯಗಳು) - 1,
		    ಅಂಕೆ_Matches,
		    matches);
	}
	ಯದಿ (completing_ಮೌಲ್ಯ_for_subopt(args, "border"))
	{
	    ಪ್ರತ್ಯಾಗಮ expand_set_opt_string(
		    args,
		    previewpopup ? p_popup_pvp_border_ಮೌಲ್ಯಗಳು
			: p_popup_cpp_border_ಮೌಲ್ಯಗಳು,
		    (previewpopup ? ARRAY_LENGTH(p_popup_pvp_border_ಮೌಲ್ಯಗಳು) - 1
			: ARRAY_LENGTH(p_popup_cpp_border_ಮೌಲ್ಯಗಳು)) - 1,
		    ಅಂಕೆ_Matches,
		    matches);
	}
	ಯದಿ (completing_ಮೌಲ್ಯ_for_subopt(args, "align"))
	{
	    ಪ್ರತ್ಯಾಗಮ expand_set_opt_string(
		    args,
		    p_popup_option_align_ಮೌಲ್ಯಗಳು,
		    ARRAY_LENGTH(p_popup_option_align_ಮೌಲ್ಯಗಳು) - 1,
		    ಅಂಕೆ_Matches,
		    matches);
	}
	ಯದಿ (completing_ಮೌಲ್ಯ_for_subopt(args, "highlight")
		|| completing_ಮೌಲ್ಯ_for_subopt(args, "borderhighlight"))
	{
	    // Return the list of all highlight names
	    ಪ್ರತ್ಯಾಗಮ expand_set_opt_generic(
		    args,
		    get_highlight_name,
		    ಅಂಕೆ_Matches,
		    matches);
	}
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಪ್ರತ್ಯಾಗಮ expand_set_opt_string(
	    args,
	    previewpopup ? p_popup_pvp_option_ಮೌಲ್ಯಗಳು
		: p_popup_cpp_option_ಮೌಲ್ಯಗಳು,
	    previewpopup ? ARRAY_LENGTH(p_popup_pvp_option_ಮೌಲ್ಯಗಳು) - 1
		: ARRAY_LENGTH(p_popup_cpp_option_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}

    ಪೂರ್ಣಾಙ್ಕ
expand_set_previewpopup(optexpand_T *args, ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಪ್ರತ್ಯಾಗಮ expand_set_popupoption(args, ಅಂಕೆ_Matches, matches, TRUE);
}

    ಪೂರ್ಣಾಙ್ಕ
expand_set_completepopup(optexpand_T *args, ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಪ್ರತ್ಯಾಗಮ expand_set_popupoption(args, ಅಂಕೆ_Matches, matches, FALSE);
}
#endif

#ಯದಿ defined(FEAT_POSTSCRIPT)
/*
 * The 'printencoding' option is changed.
 */
    ವರ್ಣ *
did_set_printencoding(optset_T *args UNUSED)
{
    char_u	*s, *p;

    // Canonize 'printencoding' if VIM standard one
    p = enc_canonize(p_penc);
    ಯದಿ (p != ರಿಕ್ತ)
    {
	vim_free(p_penc);
	p_penc = p;
    }
    ಅನ್ಯಥಾ
    {
	// Ensure lower case and '-' for '_'
	ಪ್ರತಿ (s = p_penc; *s != NUL; s++)
	{
	    ಯದಿ (*s == '_')
		*s = '-';
	    ಅನ್ಯಥಾ
		*s = TOLOWER_ASC(*s);
	}
    }

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}
#endif

#ಯದಿ defined(FEAT_PRINTER)

    ಸ್ಥಿರ char_u *
get_printoptions_names(expand_T *xp UNUSED, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಯದಿ (idx >= (ಪೂರ್ಣಾಙ್ಕ)ARRAY_LENGTH(printer_opts))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ಪ್ರತ್ಯಾಗಮ (char_u*)printer_opts[idx].name;
}

/*
 * Expand 'printoptions' option
 */
    ಪೂರ್ಣಾಙ್ಕ
expand_set_printoptions(optexpand_T *args, ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಪ್ರತ್ಯಾಗಮ expand_set_opt_generic(
	    args,
	    get_printoptions_names,
	    ಅಂಕೆ_Matches,
	    matches);
}
#endif

#ಯದಿ defined(FEAT_STL_OPT)
/*
 * The 'statusline' or the 'tabline' or the 'rulerformat' option is changed.
 * "rulerformat" is TRUE if the 'rulerformat' option is changed.
 */
    ಸ್ಥಿರ ವರ್ಣ *
parse_statustabline_rulerformat(optset_T *args, ಪೂರ್ಣಾಙ್ಕ rulerformat)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u	*s;
    ವರ್ಣ	*errmsg = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		wid;

    ಯದಿ (rulerformat)	// reset ru_wid first
	ru_wid = 0;
    s = *varp;
    ಯದಿ (rulerformat && *s == '%')
    {
	// set ru_wid if 'ruf' starts with "%99("
	ಯದಿ (*++s == '-')	// ignore a '-'
	    s++;
	wid = getdigits(&s);
	ಯದಿ (wid && *s == '(' && (errmsg = check_stl_option(p_ruf)) == ರಿಕ್ತ)
	    ru_wid = wid;
	ಅನ್ಯಥಾ
	{
	    // Validate the flags in 'rulerformat' only if it doesn't point to
	    // a custom function ("%!" flag).
	    ಯದಿ ((*varp)[1] != '!')
		errmsg = check_stl_option(p_ruf);
	}
    }
    // check 'statusline' or 'tabline' only if it doesn't start with "%!"
    ಅನ್ಯಥಾ ಯದಿ (rulerformat || s[0] != '%' || s[1] != '!')
	errmsg = check_stl_option(s);
    ಯದಿ (rulerformat && errmsg == ರಿಕ್ತ)
	comp_col();

    ಪ್ರತ್ಯಾಗಮ errmsg;
}
#endif

#ಯದಿ defined(FEAT_RENDER_OPTIONS)
/*
 * The 'renderoptions' option is changed.
 */
    ವರ್ಣ *
did_set_renderoptions(optset_T *args UNUSED)
{
    ಯದಿ (!gui_mch_set_rendering_options(p_rop))
	ಪ್ರತ್ಯಾಗಮ e_invalid_argument;

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}
#endif

#ಯದಿ defined(FEAT_RIGHTLEFT)
/*
 * The 'rightleftcmd' option is changed.
 */
    ವರ್ಣ *
did_set_rightleftcmd(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    // Currently only "search" is a supported value.
    ಯದಿ (**varp != NUL && STRCMP(*varp, "search") != 0)
	ಪ್ರತ್ಯಾಗಮ e_invalid_argument;

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

    ಪೂರ್ಣಾಙ್ಕ
expand_set_rightleftcmd(optexpand_T *args, ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಸ್ಥಿರ ವರ್ಣ *(p_rlc_ಮೌಲ್ಯಗಳು[]) = {"search", ರಿಕ್ತ};
    ಪ್ರತ್ಯಾಗಮ expand_set_opt_string(
	    args,
	    p_rlc_ಮೌಲ್ಯಗಳು,
	    ARRAY_LENGTH(p_rlc_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}
#endif

#define PUM_BORDER_CLEAR()   \
    ಕುರು {                     \
	pum_set_border(FALSE); \
	pum_set_shadow(FALSE); \
	pum_set_margin(FALSE); \
    } ಯಾವತ್ (0)

/*
 * Parse a border value from a pumopt "border:" token.
 * Returns OK on success, FAIL on error.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
parse_pumopt_border(char_u *val, ಪೂರ್ಣಾಙ್ಕ len)
{
    // Use box-drawing characters only when 'encoding' is "utf-8" and
    // 'ambiwidth' is "single".
    ಪೂರ್ಣಾಙ್ಕ	    can_use_box_chars = (enc_utf8 && *p_ambw == 's');
    char_u  *token;

    token = vim_strnsave(val, len);
    ಯದಿ (token == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಯದಿ (can_use_box_chars && STRCMP(token, "single") == 0)
	pum_set_border_chars(0x2500, 0x2502, 0x2500, 0x2502, // ─ │ ─ │
		0x250c, 0x2510, 0x2518, 0x2514); // ┌ ┐ ┘ └
    ಅನ್ಯಥಾ ಯದಿ (can_use_box_chars && STRCMP(token, "double") == 0)
	pum_set_border_chars(0x2550, 0x2551, 0x2550, 0x2551, // ═ ║ ═ ║
		0x2554, 0x2557, 0x255D, 0x255A); // ╔ ╗ ╝  ╚
    ಅನ್ಯಥಾ ಯದಿ (can_use_box_chars && STRCMP(token, "round") == 0)
	pum_set_border_chars(0x2500, 0x2502, 0x2500, 0x2502, // ─ │ ─ │
		0x256d, 0x256e, 0x256f, 0x2570); // ╭ ╮ ╯ ╰
    ಅನ್ಯಥಾ ಯದಿ (STRCMP(token, "ascii") == 0)
	pum_set_border_chars('-', '|', '-', '|', '+', '+', '+', '+');
    ಅನ್ಯಥಾ ಯದಿ (STRNCMP(token, "custom:", 7) == 0)
    {
	char_u	*q = token + 7;
	ಪೂರ್ಣಾಙ್ಕ	out[8];

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < 8; i++)
	{
	    ಯದಿ (*q == NUL || *q == ',')
	    {
		vim_free(token);
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	    out[i] = mb_ptr2char(q);
	    mb_ptr2char_adv(&q);
	    ಯದಿ (i < 7)
	    {
		ಯದಿ (*q != ';')
		{
		    vim_free(token);
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		}
		q++;
	    }
	}
	ಯದಿ (*q != NUL && *q != ',')
	{
	    vim_free(token);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	pum_set_border_chars(out[0], out[1], out[2], out[3], out[4], out[5],
		out[6], out[7]);
    }
    ಅನ್ಯಥಾ
    {
	vim_free(token);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    vim_free(token);
    pum_set_border(TRUE);
    ಪ್ರತ್ಯಾಗಮ OK;
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
    ವರ್ಣ *
did_set_pumopt(optset_T *args)
{
    char_u  **varp = (char_u **)args->os_varp;
    char_u  *p;
    ಪೂರ್ಣಾಙ್ಕ	    have_border = FALSE;
    ಪೂರ್ಣಾಙ್ಕ	    have_margin = FALSE;

    // Reset to defaults.
    PUM_BORDER_CLEAR();
    p_ph = 0;
    p_pw = 15;
    p_pmw = 0;
    p_po = 100;

    ಯದಿ (*varp == ರಿಕ್ತ || **varp == NUL)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಪ್ರತಿ (p = *varp; p != ರಿಕ್ತ && *p != NUL; )
    {
	char_u *comma = vim_strchr(p, ',');
	ಪೂರ್ಣಾಙ್ಕ len;

	ಯದಿ (comma != ರಿಕ್ತ)
	    len = (ಪೂರ್ಣಾಙ್ಕ)(comma - p);
	ಅನ್ಯಥಾ
	    len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(p);

	ಯದಿ (STRNCMP(p, "border:", 7) == 0)
	{
	    ಯದಿ (have_border)
		ಗಚ್ಛ error;
	    have_border = TRUE;
	    ಯದಿ (parse_pumopt_border(p + 7, len - 7) == FAIL)
		ಗಚ್ಛ error;
	}
	ಅನ್ಯಥಾ ಯದಿ (STRNCMP(p, "height:", 7) == 0)
	{
	    long n = atol((ವರ್ಣ *)p + 7);
	    ಯದಿ (n < 0)
		ಗಚ್ಛ error;
	    p_ph = n;
	}
	ಅನ್ಯಥಾ ಯದಿ (STRNCMP(p, "width:", 6) == 0)
	{
	    long n = atol((ವರ್ಣ *)p + 6);
	    ಯದಿ (n < 0)
		ಗಚ್ಛ error;
	    p_pw = n;
	}
	ಅನ್ಯಥಾ ಯದಿ (STRNCMP(p, "maxwidth:", 9) == 0)
	{
	    long n = atol((ವರ್ಣ *)p + 9);
	    ಯದಿ (n < 0)
		ಗಚ್ಛ error;
	    p_pmw = n;
	}
	ಅನ್ಯಥಾ ಯದಿ (STRNCMP(p, "opacity:", 8) == 0)
	{
	    long n = atol((ವರ್ಣ *)p + 8);
	    ಯದಿ (n < 0 || n > 100)
		ಗಚ್ಛ error;
	    p_po = n;
	}
	ಅನ್ಯಥಾ ಯದಿ (len == 6 && STRNCMP(p, "shadow", 6) == 0)
	    pum_set_shadow(TRUE);
	ಅನ್ಯಥಾ ಯದಿ (len == 6 && STRNCMP(p, "margin", 6) == 0)
	{
	    have_margin = TRUE;
	    pum_set_margin(TRUE);
	}
	ಅನ್ಯಥಾ
	    ಗಚ್ಛ error;

	ಯದಿ (comma != ರಿಕ್ತ)
	    p = comma + 1;
	ಅನ್ಯಥಾ
	    ಭಙ್ಗ;
    }

    ಯದಿ (have_margin && !have_border)
	ಗಚ್ಛ error;

    // Invalidate cached background for opacity changes.
    pum_opacity_changed();

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

error:
    PUM_BORDER_CLEAR();
    p_ph = 0;
    p_pw = 15;
    p_pmw = 0;
    p_po = 100;
    ಪ್ರತ್ಯಾಗಮ e_invalid_argument;
}

    ಸ್ಥಿರ char_u *
get_pum_border_style(expand_T *xp UNUSED, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಸ್ಥಿರ ವರ್ಣ *styles[] = {"ascii", "custom:", "single", "double", "round"};
    ಪ್ರತ್ಯಾಗಮ idx < ((enc_utf8 && *p_ambw == 's') ? (ಪೂರ್ಣಾಙ್ಕ)ARRAY_LENGTH(styles) : 2)
	    ? (char_u *)styles[idx] : ರಿಕ್ತ;
}

    ಪೂರ್ಣಾಙ್ಕ
expand_set_pumopt(optexpand_T *args, ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    expand_T *xp = args->oe_xp;

    ಯದಿ (xp->xp_pattern > args->oe_set_arg && *(xp->xp_pattern-1) == ':')
    {
	ಯದಿ (completing_ಮೌಲ್ಯ_for_subopt(args, "border"))
	{
	    ಪ್ರತ್ಯಾಗಮ expand_set_opt_generic(
		    args, get_pum_border_style, ಅಂಕೆ_Matches, matches);
	}
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಸ್ಥಿರ ವರ್ಣ *(p_pumopt_ಮೌಲ್ಯಗಳು[]) = {"border:", "height:", "width:",
	"maxwidth:", "opacity:", "shadow", "margin", ರಿಕ್ತ};
    ಪ್ರತ್ಯಾಗಮ expand_set_opt_string(
	    args,
	    p_pumopt_ಮೌಲ್ಯಗಳು,
	    ARRAY_LENGTH(p_pumopt_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}

/*
 * The 'pumborder' option is changed.
 * Rules:
 *   - One of { single, double, round, ascii, custom:XXXXXXXX } may appear.
 *   - "margin" may appear, but only together with exactly one border style.
 *   - "shadow" is independent and can be combined freely.
 */
    ವರ್ಣ *
did_set_pumborder(optset_T *args)
{
    char_u  **varp = (char_u **)args->os_varp;
    char_u  *p;
    ಪೂರ್ಣಾಙ್ಕ	    len;
    ಪೂರ್ಣಾಙ್ಕ	    have_border = FALSE;
    ಪೂರ್ಣಾಙ್ಕ	    have_margin = FALSE;

    PUM_BORDER_CLEAR();

    ಯದಿ (*varp == ರಿಕ್ತ || **varp == NUL)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಪ್ರತಿ (p = *varp; p != ರಿಕ್ತ && *p != NUL; )
    {
	char_u *comma = vim_strchr(p, ',');
	ಯದಿ (comma != ರಿಕ್ತ)
	    len = (ಪೂರ್ಣಾಙ್ಕ)(comma - p);
	ಅನ್ಯಥಾ
	    len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(p);

	ಯದಿ (STRNCMP(p, "shadow", len) == 0 && len == 6)
	    pum_set_shadow(TRUE);
	ಅನ್ಯಥಾ ಯದಿ (STRNCMP(p, "margin", len) == 0 && len == 6)
	{
	    have_margin = TRUE;
	    pum_set_margin(TRUE);
	}
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (have_border)
		ಗಚ್ಛ error;
	    have_border = TRUE;
	    ಯದಿ (parse_pumopt_border(p, len) == FAIL)
		ಗಚ್ಛ error;
	}

	ಯದಿ (comma != ರಿಕ್ತ)
	    p = comma + 1;
	ಅನ್ಯಥಾ
	    ಭಙ್ಗ;
    }

    ಯದಿ (have_margin && !have_border)
	ಗಚ್ಛ error;

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

error:
    PUM_BORDER_CLEAR();
    pum_set_border_chars(0, 0, 0, 0, 0, 0, 0, 0);
    ಪ್ರತ್ಯಾಗಮ e_invalid_argument;
}

    ಸ್ಥಿರ char_u *
get_pumborder_token(expand_T *xp, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಪ್ರತ್ಯಾಗಮ idx == 0 ? (char_u *)"margin"
	 : idx == 1 ? (char_u *)"shadow"
	 : get_pum_border_style(xp, idx - 2);
}

    ಪೂರ್ಣಾಙ್ಕ
expand_set_pumborder(optexpand_T *args, ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಪ್ರತ್ಯಾಗಮ expand_set_opt_generic(
	    args, get_pumborder_token, ಅಂಕೆ_Matches, matches);
}

#ಯದಿ defined(FEAT_STL_OPT)

/*
 * The 'rulerformat' option is changed.
 */
    ವರ್ಣ *
did_set_rulerformat(optset_T *args)
{
    ಪ್ರತ್ಯಾಗಮ parse_statustabline_rulerformat(args, TRUE);
}
#endif

#ಯದಿ defined(FEAT_TABPANEL)
/*
 * Process the new 'tabpanelopt' option value.
 */
    ವರ್ಣ *
did_set_tabpanelopt(optset_T *args UNUSED)
{
    ಯದಿ (tabpanelopt_changed() == FAIL)
	ಪ್ರತ್ಯಾಗಮ e_invalid_argument;

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

    ಪೂರ್ಣಾಙ್ಕ
expand_set_tabpanelopt(optexpand_T *args, ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    expand_T *xp = args->oe_xp;

    ಯದಿ (xp->xp_pattern > args->oe_set_arg && *(xp->xp_pattern-1) == ':')
    {
	ಯದಿ (completing_ಮೌಲ್ಯ_for_subopt(args, "align"))
	{
	    ಪ್ರತ್ಯಾಗಮ expand_set_opt_string(
		    args,
		    p_tplo_align_ಮೌಲ್ಯಗಳು,
		    ARRAY_LENGTH(p_tplo_align_ಮೌಲ್ಯಗಳು) - 1,
		    ಅಂಕೆ_Matches,
		    matches);
	}
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಪ್ರತ್ಯಾಗಮ expand_set_opt_string(
	    args,
	    p_tplo_ಮೌಲ್ಯಗಳು,
	    ARRAY_LENGTH(p_tplo_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}
#endif

/*
 * The 'scrollopt' option is changed.
 */
    ವರ್ಣ *
did_set_scrollopt(optset_T *args UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ did_set_opt_strings(p_sbo, p_scbopt_ಮೌಲ್ಯಗಳು, TRUE);
}

    ಪೂರ್ಣಾಙ್ಕ
expand_set_scrollopt(optexpand_T *args, ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಪ್ರತ್ಯಾಗಮ expand_set_opt_string(
	    args,
	    p_scbopt_ಮೌಲ್ಯಗಳು,
	    ARRAY_LENGTH(p_scbopt_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}

/*
 * The 'wlseat' option is changed
 */
    ವರ್ಣ *
did_set_wlseat(optset_T *args UNUSED)
{
#ifdef FEAT_WAYLAND_CLIPBOARD
    // If there isn't any seat named 'wlseat', then let the Wayland clipboard be
    // unavailable. Ignore errors returned.
    clip_reset_wayland();
#endif

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * The 'selection' option is changed.
 */
    ವರ್ಣ *
did_set_selection(optset_T *args UNUSED)
{
    ಯದಿ (*p_sel == NUL || check_opt_strings(p_sel, p_sel_ಮೌಲ್ಯಗಳು, FALSE) != OK)
	ಪ್ರತ್ಯಾಗಮ e_invalid_argument;

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

    ಪೂರ್ಣಾಙ್ಕ
expand_set_selection(optexpand_T *args, ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಪ್ರತ್ಯಾಗಮ expand_set_opt_string(
	    args,
	    p_sel_ಮೌಲ್ಯಗಳು,
	    ARRAY_LENGTH(p_sel_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}

/*
 * The 'selectmode' option is changed.
 */
    ವರ್ಣ *
did_set_selectmode(optset_T *args UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ did_set_opt_strings(p_slm, p_slm_ಮೌಲ್ಯಗಳು, TRUE);
}

    ಪೂರ್ಣಾಙ್ಕ
expand_set_selectmode(optexpand_T *args, ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಪ್ರತ್ಯಾಗಮ expand_set_opt_string(
	    args,
	    p_slm_ಮೌಲ್ಯಗಳು,
	    ARRAY_LENGTH(p_slm_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}

#ಯದಿ defined(FEAT_SESSION)
/*
 * The 'sessionoptions' option is changed.
 */
    ವರ್ಣ *
did_set_sessionoptions(optset_T *args)
{
    ಯದಿ (opt_strings_flags(p_ssop, p_ssop_ಮೌಲ್ಯಗಳು, &ssop_flags, TRUE) != OK)
	ಪ್ರತ್ಯಾಗಮ e_invalid_argument;
    ಯದಿ ((ssop_flags & SSOP_CURDIR) && (ssop_flags & SSOP_SESDIR))
    {
	// Don't allow both "sesdir" and "curdir".
	(ಶೂನ್ಯ)opt_strings_flags(args->os_oldval.string, p_ssop_ಮೌಲ್ಯಗಳು,
							&ssop_flags, TRUE);
	ಪ್ರತ್ಯಾಗಮ e_invalid_argument;
    }

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

    ಪೂರ್ಣಾಙ್ಕ
expand_set_sessionoptions(
    optexpand_T		*args,
    ಪೂರ್ಣಾಙ್ಕ			*ಅಂಕೆ_Matches,
    char_u		***matches)
{
    ಪ್ರತ್ಯಾಗಮ expand_set_opt_string(
	    args,
	    p_ssop_ಮೌಲ್ಯಗಳು,
	    ARRAY_LENGTH(p_ssop_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}
#endif

/*
 * Validate 'shellpipe'/'shellredir' option.
 */
    ವರ್ಣ *
did_set_shellpipe_redir(optset_T *args)
{
    char_u	*p;
    bool	seen = false;

    ಪ್ರತಿ (p = args->os_newval.string; *p != NUL; ++p)
    {
	ಯದಿ (*p != '%')
	    ಅನುವರ್ತ;

	ಯದಿ (p[1] == NUL)
	    ಪ್ರತ್ಯಾಗಮ e_invalid_format_string_single_percent_s;

	ಯದಿ (p[1] == '%')
	{
	    ++p;    // skip second %
	    ಅನುವರ್ತ;
	}

	ಯದಿ (p[1] == 's')
	{
	    ಯದಿ (seen)
		ಪ್ರತ್ಯಾಗಮ e_invalid_format_string_single_percent_s;

	    seen = true;
	    ++p;    // consume 's'
	    ಅನುವರ್ತ;
	}
	ಪ್ರತ್ಯಾಗಮ e_invalid_format_string_single_percent_s;
    }
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * The 'shortmess' option is changed.
 */
    ವರ್ಣ *
did_set_shortmess(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    ಪ್ರತ್ಯಾಗಮ did_set_option_listflag(*varp, (char_u *)SHM_ALL, args->os_errbuf,
		    args->os_errbuflen);
}

    ಪೂರ್ಣಾಙ್ಕ
expand_set_shortmess(optexpand_T *args, ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಪ್ರತ್ಯಾಗಮ expand_set_opt_listflag(args, (char_u*)SHM_ALL, ಅಂಕೆ_Matches, matches);
}

#ಯದಿ defined(FEAT_LINEBREAK)
/*
 * The 'showbreak' option is changed.
 */
    ವರ್ಣ *
did_set_showbreak(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u	*s;

    ಪ್ರತಿ (s = *varp; *s; )
    {
	ಯದಿ (ptr2cells(s) != 1)
	    ಪ್ರತ್ಯಾಗಮ e_showbreak_contains_unprintable_or_wide_character;
	MB_PTR_ADV(s);
    }

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}
#endif

/*
 * The 'showcmdloc' option is changed.
 */
    ವರ್ಣ *
did_set_showcmdloc(optset_T *args UNUSED)
{
    ವರ್ಣ	*errmsg = did_set_opt_strings(p_sloc, p_sloc_ಮೌಲ್ಯಗಳು, FALSE);

    ಯದಿ (errmsg == ರಿಕ್ತ)
	comp_col();

    ಪ್ರತ್ಯಾಗಮ errmsg;
}

    ಪೂರ್ಣಾಙ್ಕ
expand_set_showcmdloc(optexpand_T *args, ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಪ್ರತ್ಯಾಗಮ expand_set_opt_string(
	    args,
	    p_sloc_ಮೌಲ್ಯಗಳು,
	    ARRAY_LENGTH(p_sloc_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}

#ಯದಿ defined(FEAT_SIGNS)
/*
 * The 'signcolumn' option is changed.
 */
    ವರ್ಣ *
did_set_signcolumn(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

# ಯದಿ defined(FEAT_LINEBREAK)
    ಯದಿ (check_opt_strings(*varp, p_scl_ಮೌಲ್ಯಗಳು, FALSE) != OK)
	ಪ್ರತ್ಯಾಗಮ e_invalid_argument;
    // When changing the 'signcolumn' to or from 'number', recompute the
    // width of the number column if 'number' or 'relativenumber' is set.
    ಯದಿ (((*args->os_oldval.string == 'n' && args->os_oldval.string[1] == 'u')
		|| (*curwin->w_p_scl == 'n' && *(curwin->w_p_scl + 1) =='u'))
	    && (curwin->w_p_nu || curwin->w_p_rnu))
	curwin->w_nrwidth_ಸಾಲು_ಎಣಿಕೆ = 0;
# endif

# ಯದಿ defined(FEAT_GUI)
    // In the GUI, when sign icons are used, a full screen clear is needed
    // to properly redraw the sign icons.
    ಯದಿ (gui.in_use && curbuf->b_signlist != ರಿಕ್ತ)
	redraw_all_later(UPD_CLEAR);
# endif

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

    ಪೂರ್ಣಾಙ್ಕ
expand_set_signcolumn(optexpand_T *args, ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಪ್ರತ್ಯಾಗಮ expand_set_opt_string(
	    args,
	    p_scl_ಮೌಲ್ಯಗಳು,
	    ARRAY_LENGTH(p_scl_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}
#endif

#ಯದಿ defined(FEAT_SPELL)
/*
 * The 'spellcapcheck' option is changed.
 */
    ವರ್ಣ *
did_set_spellcapcheck(optset_T *args UNUSED)
{
    // compile the regexp program.
    ಪ್ರತ್ಯಾಗಮ compile_cap_prog(curwin->w_s);
}

/*
 * The 'spellfile' option is changed.
 */
    ವರ್ಣ *
did_set_spellfile(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    ಯದಿ (!valid_spellfile(*varp))
	ಪ್ರತ್ಯಾಗಮ e_invalid_argument;

    // If there is a window for this buffer in which 'spell' is set load the
    // wordlists.
    ಪ್ರತ್ಯಾಗಮ did_set_spell_option();
}

/*
 * The 'spell' option is changed.
 */
    ವರ್ಣ *
did_set_spelllang(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    ಯದಿ (!valid_spelllang(*varp))
	ಪ್ರತ್ಯಾಗಮ e_invalid_argument;

    // If there is a window for this buffer in which 'spell' is set load the
    // wordlists.
    ಪ್ರತ್ಯಾಗಮ did_set_spell_option();
}

/*
 * The 'spelloptions' option is changed.
 */
    ವರ್ಣ *
did_set_spelloptions(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    ಯದಿ (**varp != NUL && STRCMP(*varp, "camel") != 0)
	ಪ್ರತ್ಯಾಗಮ e_invalid_argument;

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

    ಪೂರ್ಣಾಙ್ಕ
expand_set_spelloptions(optexpand_T *args, ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಸ್ಥಿರ ವರ್ಣ *(p_spo_ಮೌಲ್ಯಗಳು[]) = {"camel", ರಿಕ್ತ};
    ಪ್ರತ್ಯಾಗಮ expand_set_opt_string(
	    args,
	    p_spo_ಮೌಲ್ಯಗಳು,
	    ARRAY_LENGTH(p_spo_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}

/*
 * The 'spellsuggest' option is changed.
 */
    ವರ್ಣ *
did_set_spellsuggest(optset_T *args UNUSED)
{
    ಯದಿ (spell_check_sps() != OK)
	ಪ್ರತ್ಯಾಗಮ e_invalid_argument;

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

    ಪೂರ್ಣಾಙ್ಕ
expand_set_spellsuggest(optexpand_T *args, ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಪ್ರತ್ಯಾಗಮ expand_set_opt_string(
	    args,
	    p_sps_ಮೌಲ್ಯಗಳು,
	    ARRAY_LENGTH(p_sps_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}
#endif

/*
 * The 'splitkeep' option is changed.
 */
    ವರ್ಣ *
did_set_splitkeep(optset_T *args UNUSED)
{
    win_T	*wp;
    tabpage_T	*tp;
    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	wp->w_prev_height = wp->w_height;
	wp->w_prev_winrow = wp->w_winrow;
    }
    ಪ್ರತ್ಯಾಗಮ did_set_opt_strings(p_spk, p_spk_ಮೌಲ್ಯಗಳು, FALSE);
}

    ಪೂರ್ಣಾಙ್ಕ
expand_set_splitkeep(optexpand_T *args, ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಪ್ರತ್ಯಾಗಮ expand_set_opt_string(
	    args,
	    p_spk_ಮೌಲ್ಯಗಳು,
	    ARRAY_LENGTH(p_spk_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}

#ಯದಿ defined(FEAT_STL_OPT)
/*
 * The 'statusline' option is changed.
 */
    ವರ್ಣ *
did_set_statusline(optset_T *args)
{
    char_u  **varp = (char_u **)args->os_varp;
    ವರ್ಣ    *ret = parse_statustabline_rulerformat(args, FALSE);

    ಯದಿ (ret != ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ret;
    update_stl_rendered_height(varp == &curwin->w_p_stl ? curwin : ರಿಕ್ತ);
    frame_change_statusline_height();

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Rewrite the 'statuslineopt' string: replace the last "maxheight:N" with
 * actual_stlh and remove earlier duplicates.  Keyword order is preserved.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
update_stlo_maxheight(char_u **varp, ಪೂರ್ಣಾಙ್ಕ actual_stlh)
{
    ಧ್ರುವ char_u    *src = *varp;
    ಧ್ರುವ char_u    *last_mh = ರಿಕ್ತ;
    ಧ್ರುವ char_u    *p;

    // Find the last "maxheight:" token.
    p = src;
    ಯಾವತ್ (*p != NUL)
    {
	ಯದಿ (STRNCMP(p, "maxheight:", 10) == 0)
	    last_mh = p;
	ಯಾವತ್ (*p != ',' && *p != NUL)
	    ++p;
	ಯದಿ (*p == ',')
	    ++p;
    }

    ಯದಿ (last_mh == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    size_t	bufsize = STRLEN(src) + NUMBUFLEN + 1;
    char_u	*buf = alloc(bufsize);
    ಯದಿ (buf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಪೂರ್ಣಾಙ್ಕ		len = 0;
    bool	need_comma = false;

    p = src;
    ಯಾವತ್ (*p != NUL)
    {
	ಧ್ರುವ	char_u	*tok = p;
	ಪೂರ್ಣಾಙ್ಕ	tok_len;

	ಯಾವತ್ (*p != ',' && *p != NUL)
	    ++p;
	tok_len = (ಪೂರ್ಣಾಙ್ಕ)(p - tok);
	ಯದಿ (*p == ',')
	    ++p;

	ಯದಿ (STRNCMP(tok, "maxheight:", 10) == 0)
	{
	    ಯದಿ (tok == last_mh)
	    {
		// Replace the last occurrence with the actual value.
		ಯದಿ (need_comma)
		    buf[len++] = ',';
		len += vim_snprintf((ವರ್ಣ *)buf + len,
			bufsize - len, "maxheight:%d", actual_stlh);
		need_comma = true;
	    }
	}
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (need_comma)
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
    ವರ್ಣ *
did_set_statuslineopt(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    win_T       *wp = varp == &curwin->w_p_stlo ? curwin : ರಿಕ್ತ;

    ಯದಿ (statuslineopt_changed(*varp, wp) == FAIL)
	ಪ್ರತ್ಯಾಗಮ e_invalid_argument;

    // Sync stl_rendered_height with the current statusline.
    update_stl_rendered_height(wp);

    // Update the maxheight value to the actual value set.
    // Note: Must be changed if p_stlo_values are changed.
    ಯದಿ (*varp != empty_option)
    {
	ಪೂರ್ಣಾಙ್ಕ actual_stlh;

	ಯದಿ (wp != ರಿಕ್ತ)
	{
	    frame_change_statusline_height();
	    actual_stlh = MIN(wp->w_p_stlo_mh,
		    wp->w_height + wp->w_status_height - p_wmh);
	}
	ಅನ್ಯಥಾ
	    actual_stlh = frame_change_statusline_height();

	// Only re-serialize when the window actually has a status line shown.
	ಯದಿ (actual_stlh > 0)
	{
	    update_stlo_maxheight(varp, actual_stlh);
	    // Update the parsed maxheight member directly.
	    ಯದಿ (wp != ರಿಕ್ತ)
		wp->w_p_stlo_mh = actual_stlh;
	    ಅನ್ಯಥಾ
		set_stlo_mh(actual_stlh);
	}
    }
    ಅನ್ಯಥಾ
	frame_change_statusline_height();

    ಯದಿ ((args->os_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	// :set clears the local values.
	clear_string_option(&curwin->w_p_stlo);
	curwin->w_p_stlo_fh = -1;
	curwin->w_p_stlo_mh = -1;
    }

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

    ಪೂರ್ಣಾಙ್ಕ
expand_set_statuslineopt(optexpand_T *args, ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಪ್ರತ್ಯಾಗಮ expand_set_opt_string(
	    args,
	    p_stlo_ಮೌಲ್ಯಗಳು,
	    ARRAY_LENGTH(p_stlo_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}
#endif

/*
 * The 'swapsync' option is changed.
 */
    ವರ್ಣ *
did_set_swapsync(optset_T *args UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ did_set_opt_strings(p_sws, p_sws_ಮೌಲ್ಯಗಳು, FALSE);
}

    ಪೂರ್ಣಾಙ್ಕ
expand_set_swapsync(optexpand_T *args, ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಪ್ರತ್ಯಾಗಮ expand_set_opt_string(
	    args,
	    p_sws_ಮೌಲ್ಯಗಳು,
	    ARRAY_LENGTH(p_sws_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}

/*
 * The 'switchbuf' option is changed.
 */
    ವರ್ಣ *
did_set_switchbuf(optset_T *args UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ did_set_opt_flags(p_swb, p_swb_ಮೌಲ್ಯಗಳು, &swb_flags, TRUE);
}

    ಪೂರ್ಣಾಙ್ಕ
expand_set_switchbuf(optexpand_T *args, ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಪ್ರತ್ಯಾಗಮ expand_set_opt_string(
	    args,
	    p_swb_ಮೌಲ್ಯಗಳು,
	    ARRAY_LENGTH(p_swb_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}

/*
 * The 'tabclose' option is changed.
 */
    ವರ್ಣ *
did_set_tabclose(optset_T *args UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ did_set_opt_flags(p_tcl, p_tcl_ಮೌಲ್ಯಗಳು, &tcl_flags, TRUE);
}

    ಪೂರ್ಣಾಙ್ಕ
expand_set_tabclose(optexpand_T *args, ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಪ್ರತ್ಯಾಗಮ expand_set_opt_string(
	    args,
	    p_tcl_ಮೌಲ್ಯಗಳು,
	    ARRAY_LENGTH(p_tcl_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}

#ಯದಿ defined(FEAT_STL_OPT)
/*
 * The 'tabline' option is changed.
 */
    ವರ್ಣ *
did_set_tabline(optset_T *args)
{
    ಪ್ರತ್ಯಾಗಮ parse_statustabline_rulerformat(args, FALSE);
}
#endif

/*
 * The 'tagcase' option is changed.
 */
    ವರ್ಣ *
did_set_tagcase(optset_T *args)
{
    unsigned ಪೂರ್ಣಾಙ್ಕ	*flags;
    char_u		*p;

    ಯದಿ (args->os_flags & OPT_LOCAL)
    {
	p = curbuf->b_p_tc;
	flags = &curbuf->b_tc_flags;
    }
    ಅನ್ಯಥಾ
    {
	p = p_tc;
	flags = &tc_flags;
    }

    ಯದಿ ((args->os_flags & OPT_LOCAL) && *p == NUL)
	// make the local value empty: use the global value
	*flags = 0;
    ಅನ್ಯಥಾ ಯದಿ (*p == NUL
	    || opt_strings_flags(p, p_tc_ಮೌಲ್ಯಗಳು, flags, FALSE) != OK)
	ಪ್ರತ್ಯಾಗಮ e_invalid_argument;

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

    ಪೂರ್ಣಾಙ್ಕ
expand_set_tagcase(optexpand_T *args, ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಪ್ರತ್ಯಾಗಮ expand_set_opt_string(
	    args,
	    p_tc_ಮೌಲ್ಯಗಳು,
	    ARRAY_LENGTH(p_tc_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}

/*
 * The 'term' option is changed.
 */
    ವರ್ಣ *
did_set_term(optset_T *args UNUSED)
{
    ಯದಿ (T_NAME[0] == NUL)
	ಪ್ರತ್ಯಾಗಮ e_cannot_set_term_to_empty_string;
#ifdef FEAT_GUI
    ಯದಿ (gui.in_use)
	ಪ್ರತ್ಯಾಗಮ e_cannot_change_term_in_GUI;
    ಯದಿ (term_is_gui(T_NAME))
	ಪ್ರತ್ಯಾಗಮ e_use_gui_to_start_GUI;
#endif
    ಯದಿ (set_termname(T_NAME) == FAIL)
	ಪ್ರತ್ಯಾಗಮ e_not_found_in_termcap;

    // Screen colors may have changed.
    redraw_later_clear();

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Some terminal option (t_xxx) is changed
 */
    ವರ್ಣ *
did_set_term_option(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    ಯದಿ (!full_screen)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    // ":set t_Co=0" and ":set t_Co=1" do ":set t_Co="
    ಯದಿ (varp == &T_CCO)
    {
	ಪೂರ್ಣಾಙ್ಕ colors = atoi((ವರ್ಣ *)T_CCO);

	// Only reinitialize colors if t_Co value has really changed to
	// avoid expensive reload of colorscheme if t_Co is set to the
	// same value multiple times.
	ಯದಿ (colors != t_colors)
	{
	    t_colors = colors;
	    ಯದಿ (t_colors <= 1)
	    {
		vim_free(T_CCO);
		T_CCO = empty_option;
	    }
#ಯದಿ defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
	    ಯದಿ (is_term_win32())
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
    ಯದಿ (varp == &T_ME)
    {
	out_str(T_ME);
	redraw_later(UPD_CLEAR);
#ಯದಿ defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
	// Since t_me has been set, this probably means that the user
	// wants to use this as default colors.  Need to reset default
	// background/foreground colors.
# ifdef VIMDLL
	ಯದಿ (!gui.in_use && !gui.starting)
# endif
	    mch_set_normal_colors();
#endif
    }
    ಯದಿ (varp == &T_BE && termcap_active)
    {
	MAY_WANT_TO_LOG_THIS;

	ಯದಿ (*T_BE == NUL)
	    // When clearing t_BE we assume the user no longer wants
	    // bracketed paste, thus disable it by writing t_BD.
	    out_str(T_BD);
	ಅನ್ಯಥಾ
	    out_str(T_BE);
    }

    ಯದಿ (varp == &T_BSU || varp == &T_ESU)
	term_set_sync_output(TERM_SYNC_OUTPUT_OFF);

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}


#ifdef UNIX
/*
 * The 'termresize' option is changed.
 */
    ವರ್ಣ *
did_set_termresize(optset_T *args UNUSED)
{
    // If empty or "inband", then attempt to enable in-band resize events.
    ಯದಿ (*p_trz == NUL || STRCMP(p_trz, "inband") == 0)
	term_set_win_resize(true);
    ಅನ್ಯಥಾ ಯದಿ (STRCMP(p_trz, "sigwinch") == 0)
	term_set_win_resize(false);
    ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ e_invalid_argument;

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

    ಪೂರ್ಣಾಙ್ಕ
expand_set_termresize(optexpand_T *args, ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಪ್ರತ್ಯಾಗಮ expand_set_opt_string(
	    args,
	    p_trz_ಮೌಲ್ಯಗಳು,
	    ARRAY_LENGTH(p_trz_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}
#endif

#ಯದಿ defined(FEAT_TERMINAL)
/*
 * The 'termwinkey' option is changed.
 */
    ವರ್ಣ *
did_set_termwinkey(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    ಯದಿ ((*varp)[0] != NUL && string_to_ಕೀಲಿ(*varp, TRUE) == 0)
	ಪ್ರತ್ಯಾಗಮ e_invalid_argument;

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * The 'termwinsize' option is changed.
 */
    ವರ್ಣ *
did_set_termwinsize(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u	*p;

    ಯದಿ ((*varp)[0] == NUL)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    p = skipdigits(*varp);
    ಯದಿ (p == *varp || (*p != 'x' && *p != '*') || *skipdigits(p + 1) != NUL)
	ಪ್ರತ್ಯಾಗಮ e_invalid_argument;

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

# ಯದಿ defined(MSWIN)
/*
 * The 'termwintype' option is changed.
 */
    ವರ್ಣ *
did_set_termwintype(optset_T *args UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ did_set_opt_strings(p_twt, p_twt_ಮೌಲ್ಯಗಳು, FALSE);
}

    ಪೂರ್ಣಾಙ್ಕ
expand_set_termwintype(optexpand_T *args, ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಪ್ರತ್ಯಾಗಮ expand_set_opt_string(
	    args,
	    p_twt_ಮೌಲ್ಯಗಳು,
	    ARRAY_LENGTH(p_twt_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}
# endif
#endif

/*
 * The 'titlestring' option is changed.
 */
    ವರ್ಣ *
did_set_titlestring(optset_T *args)
{
    ಪೂರ್ಣಾಙ್ಕ flagval = 0;

#ifdef FEAT_STL_OPT
    flagval = STL_IN_TITLE;
#endif
    ಪ್ರತ್ಯಾಗಮ parse_titleiconstring(args, flagval);
}

#ಯದಿ defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
/*
 * The 'toolbar' option is changed.
 */
    ವರ್ಣ *
did_set_toolbar(optset_T *args UNUSED)
{
    ಯದಿ (opt_strings_flags(p_toolbar, p_toolbar_ಮೌಲ್ಯಗಳು, &toolbar_flags,
								TRUE) != OK)
	ಪ್ರತ್ಯಾಗಮ e_invalid_argument;

    out_flush();
    gui_mch_show_toolbar((toolbar_flags &
		(TOOLBAR_TEXT | TOOLBAR_ICONS)) != 0);
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

    ಪೂರ್ಣಾಙ್ಕ
expand_set_toolbar(optexpand_T *args, ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಪ್ರತ್ಯಾಗಮ expand_set_opt_string(
	    args,
	    p_toolbar_ಮೌಲ್ಯಗಳು,
	    ARRAY_LENGTH(p_toolbar_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}
#endif

#ಯದಿ defined(FEAT_TOOLBAR) && defined(FEAT_GUI_GTK)
/*
 * The 'toolbariconsize' option is changed.  GTK+ 2 only.
 */
    ವರ್ಣ *
did_set_toolbariconsize(optset_T *args UNUSED)
{
    ಯದಿ (opt_strings_flags(p_tbis, p_tbis_ಮೌಲ್ಯಗಳು, &tbis_flags, FALSE) != OK)
	ಪ್ರತ್ಯಾಗಮ e_invalid_argument;

    out_flush();
    gui_mch_show_toolbar((toolbar_flags &
		(TOOLBAR_TEXT | TOOLBAR_ICONS)) != 0);
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

    ಪೂರ್ಣಾಙ್ಕ
expand_set_toolbariconsize(
    optexpand_T	*args,
    ಪೂರ್ಣಾಙ್ಕ		*ಅಂಕೆ_Matches,
    char_u	***matches)
{
    ಪ್ರತ್ಯಾಗಮ expand_set_opt_string(
	    args,
	    p_tbis_ಮೌಲ್ಯಗಳು,
	    ARRAY_LENGTH(p_tbis_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}
#endif

#ಯದಿ defined(UNIX) || defined(VMS)
/*
 * The 'ttymouse' option is changed.
 */
    ವರ್ಣ *
did_set_ttymouse(optset_T *args UNUSED)
{
    ವರ್ಣ *errmsg = ರಿಕ್ತ;

    // Switch the mouse off before changing the escape sequences used for
    // that.
    mch_setmouse(FALSE);
    ಯದಿ (opt_strings_flags(p_ttym, p_ttym_ಮೌಲ್ಯಗಳು, &ttym_flags, FALSE) != OK)
	errmsg = e_invalid_argument;
    ಅನ್ಯಥಾ
	check_mouse_termcode();
    ಯದಿ (termcap_active)
	setmouse();		// may switch it on again

    ಪ್ರತ್ಯಾಗಮ errmsg;
}

    ಪೂರ್ಣಾಙ್ಕ
expand_set_ttymouse(optexpand_T *args, ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಪ್ರತ್ಯಾಗಮ expand_set_opt_string(
	    args,
	    p_ttym_ಮೌಲ್ಯಗಳು,
	    ARRAY_LENGTH(p_ttym_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}
#endif

#ಯದಿ defined(FEAT_VARTABS)
/*
 * The 'varsofttabstop' option is changed.
 */
    ವರ್ಣ *
did_set_varsofttabstop(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u *cp;

    ಯದಿ (!((*varp)[0]) || ((*varp)[0] == '0' && !((*varp)[1])))
	VIM_CLEAR(curbuf->b_p_vsts_array);
    ಅನ್ಯಥಾ
    {
	ಪ್ರತಿ (cp = *varp; *cp; ++cp)
	{
	    ಯದಿ (vim_isdigit(*cp))
		ಅನುವರ್ತ;
	    ಯದಿ (*cp == ',' && cp > *varp && *(cp-1) != ',')
		ಅನುವರ್ತ;
	    ಪ್ರತ್ಯಾಗಮ e_invalid_argument;
	}

	ಪೂರ್ಣಾಙ್ಕ *oldarray = curbuf->b_p_vsts_array;
	ಯದಿ (tabstop_set(*varp, &(curbuf->b_p_vsts_array)) == OK)
	{
	    ಯದಿ (oldarray)
		vim_free(oldarray);
	}
	ಅನ್ಯಥಾ
	    ಪ್ರತ್ಯಾಗಮ e_invalid_argument;
    }

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * The 'vartabstop' option is changed.
 */
    ವರ್ಣ *
did_set_vartabstop(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u *cp;

    ಯದಿ (!((*varp)[0]) || ((*varp)[0] == '0' && !((*varp)[1])))
	VIM_CLEAR(curbuf->b_p_vts_array);
    ಅನ್ಯಥಾ
    {
	ಪ್ರತಿ (cp = *varp; *cp; ++cp)
	{
	    ಯದಿ (vim_isdigit(*cp))
		ಅನುವರ್ತ;
	    ಯದಿ (*cp == ',' && cp > *varp && *(cp-1) != ',')
		ಅನುವರ್ತ;
	    ಪ್ರತ್ಯಾಗಮ e_invalid_argument;
	}

	ಪೂರ್ಣಾಙ್ಕ *oldarray = curbuf->b_p_vts_array;

	ಯದಿ (tabstop_set(*varp, &(curbuf->b_p_vts_array)) == OK)
	{
	    vim_free(oldarray);
# ifdef FEAT_FOLDING
	    ಯದಿ (foldmethodIsIndent(curwin))
		foldUpdateAll(curwin);
# endif
	}
	ಅನ್ಯಥಾ
	    ಪ್ರತ್ಯಾಗಮ e_invalid_argument;
    }

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}
#endif

/*
 * The 'verbosefile' option is changed.
 */
    ವರ್ಣ *
did_set_verbosefile(optset_T *args UNUSED)
{
    verbose_stop();
    ಯದಿ (*p_vfile != NUL && verbose_open() == FAIL)
	ಪ್ರತ್ಯಾಗಮ e_invalid_argument;

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

#ಯದಿ defined(FEAT_SESSION)
/*
 * The 'viewoptions' option is changed.
 */
    ವರ್ಣ *
did_set_viewoptions(optset_T *args UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ did_set_opt_flags(p_vop, p_ssop_ಮೌಲ್ಯಗಳು, &vop_flags, TRUE);
}
#endif

#ಯದಿ defined(FEAT_VIMINFO)
/*
 * The 'viminfo' option is changed.
 */
    ವರ್ಣ *
did_set_viminfo(optset_T *args)
{
    char_u	*s;
    ವರ್ಣ	*errmsg = ರಿಕ್ತ;

    ಪ್ರತಿ (s = p_viminfo; *s;)
    {
	// Check it's a valid character
	ಯದಿ (vim_strchr((char_u *)"!\"%'/:<@cfhnrs", *s) == ರಿಕ್ತ)
	{
	    errmsg = illegal_char(args->os_errbuf, args->os_errbuflen, *s);
	    ಭಙ್ಗ;
	}
	ಯದಿ (*s == 'n')	// name is always last one
	    ಭಙ್ಗ;
	ಅನ್ಯಥಾ ಯದಿ (*s == 'r') // skip until next ','
	{
	    ಯಾವತ್ (*++s && *s != ',')
		;
	}
	ಅನ್ಯಥಾ ಯದಿ (*s == '%')
	{
	    // optional number
	    ಯಾವತ್ (vim_isdigit(*++s))
		;
	}
	ಅನ್ಯಥಾ ಯದಿ (*s == '!' || *s == 'h' || *s == 'c')
	    ++s;		// no extra chars
	ಅನ್ಯಥಾ		// must have a number
	{
	    ಯಾವತ್ (vim_isdigit(*++s))
		;

	    ಯದಿ (!VIM_ISDIGIT(*(s - 1)))
	    {
		ಯದಿ (args->os_errbuf != ರಿಕ್ತ)
		{
		    vim_snprintf(args->os_errbuf, args->os_errbuflen,
			    _(e_missing_ಸಂಖ್ಯೆ_after_angle_str_angle),
			    transchar_byte(*(s - 1)));
		    errmsg = args->os_errbuf;
		}
		ಅನ್ಯಥಾ
		    errmsg = "";
		ಭಙ್ಗ;
	    }
	}
	ಯದಿ (*s == ',')
	    ++s;
	ಅನ್ಯಥಾ ಯದಿ (*s)
	{
	    ಯದಿ (args->os_errbuf != ರಿಕ್ತ)
		errmsg = e_missing_comma;
	    ಅನ್ಯಥಾ
		errmsg = "";
	    ಭಙ್ಗ;
	}
    }
    ಯದಿ (*p_viminfo && errmsg == ರಿಕ್ತ && get_viminfo_parameter('\'') < 0)
	errmsg = e_must_specify_a_ಮೌಲ್ಯ;

    ಪ್ರತ್ಯಾಗಮ errmsg;
}
#endif

/*
 * The 'virtualedit' option is changed.
 */
    ವರ್ಣ *
did_set_virtualedit(optset_T *args)
{
    char_u		*ve = p_ve;
    unsigned ಪೂರ್ಣಾಙ್ಕ	*flags = &ve_flags;

    ಯದಿ (args->os_flags & OPT_LOCAL)
    {
	ve = curwin->w_p_ve;
	flags = &curwin->w_ve_flags;
    }

    ಯದಿ ((args->os_flags & OPT_LOCAL) && *ve == NUL)
	// make the local value empty: use the global value
	*flags = 0;
    ಅನ್ಯಥಾ
    {
	ಯದಿ (opt_strings_flags(ve, p_ve_ಮೌಲ್ಯಗಳು, flags, TRUE) != OK)
	    ಪ್ರತ್ಯಾಗಮ e_invalid_argument;
	ಅನ್ಯಥಾ ಯದಿ (STRCMP(ve, args->os_oldval.string) != 0)
	{
	    // Recompute cursor position in case the new 've' setting
	    // changes something.
	    validate_virtcol();
	    coladvance(curwin->w_virtcol);
	}
    }

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

    ಪೂರ್ಣಾಙ್ಕ
expand_set_virtualedit(optexpand_T *args, ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಪ್ರತ್ಯಾಗಮ expand_set_opt_string(
	    args,
	    p_ve_ಮೌಲ್ಯಗಳು,
	    ARRAY_LENGTH(p_ve_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}

/*
 * The 'whichwrap' option is changed.
 */
    ವರ್ಣ *
did_set_whichwrap(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    // Add ',' to the list flags because 'whichwrap' is a flag
    // list that is comma-separated.
    ಪ್ರತ್ಯಾಗಮ did_set_option_listflag(*varp, (char_u *)(WW_ALL ","),
		    args->os_errbuf, args->os_errbuflen);
}

    ಪೂರ್ಣಾಙ್ಕ
expand_set_whichwrap(optexpand_T *args, ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಪ್ರತ್ಯಾಗಮ expand_set_opt_listflag(args, (char_u*)WW_ALL, ಅಂಕೆ_Matches, matches);
}

/*
 * The 'wildmode' option is changed.
 */
    ವರ್ಣ *
did_set_wildmode(optset_T *args UNUSED)
{
    ಯದಿ (check_opt_wim() == FAIL)
	ಪ್ರತ್ಯಾಗಮ e_invalid_argument;
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

    ಪೂರ್ಣಾಙ್ಕ
expand_set_wildmode(optexpand_T *args, ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಪ್ರತ್ಯಾಗಮ expand_set_opt_string(
	    args,
	    p_wim_ಮೌಲ್ಯಗಳು,
	    ARRAY_LENGTH(p_wim_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}

/*
 * The 'wildoptions' option is changed.
 */
    ವರ್ಣ *
did_set_wildoptions(optset_T *args UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ did_set_opt_strings(p_wop, p_wop_ಮೌಲ್ಯಗಳು, TRUE);
}

    ಪೂರ್ಣಾಙ್ಕ
expand_set_wildoptions(optexpand_T *args, ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಪ್ರತ್ಯಾಗಮ expand_set_opt_string(
	    args,
	    p_wop_ಮೌಲ್ಯಗಳು,
	    ARRAY_LENGTH(p_wop_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}

#ಯದಿ defined(FEAT_WAK)
/*
 * The 'winaltkeys' option is changed.
 */
    ವರ್ಣ *
did_set_winaltkeys(optset_T *args UNUSED)
{
    ವರ್ಣ *errmsg = ರಿಕ್ತ;

    ಯದಿ (*p_wak == NUL || check_opt_strings(p_wak, p_wak_ಮೌಲ್ಯಗಳು, FALSE) != OK)
	errmsg = e_invalid_argument;
# ifdef FEAT_MENU
#  ಯದಿ defined(FEAT_GUI_MOTIF)
    ಅನ್ಯಥಾ ಯದಿ (gui.in_use)
	gui_motif_set_mnemonics(p_wak[0] == 'y' || p_wak[0] == 'm');
#  elif defined(FEAT_GUI_GTK)
    ಅನ್ಯಥಾ ಯದಿ (gui.in_use)
	gui_gtk_set_mnemonics(p_wak[0] == 'y' || p_wak[0] == 'm');
#  endif
# endif
    ಪ್ರತ್ಯಾಗಮ errmsg;
}

    ಪೂರ್ಣಾಙ್ಕ
expand_set_winaltkeys(optexpand_T *args, ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಪ್ರತ್ಯಾಗಮ expand_set_opt_string(
	    args,
	    p_wak_ಮೌಲ್ಯಗಳು,
	    ARRAY_LENGTH(p_wak_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}
#endif

/*
 * The 'wincolor' option is changed.
 */
    ವರ್ಣ *
did_set_wincolor(optset_T *args UNUSED)
{
    update_wincolor(curwin, args->os_newval.string);
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * The 'winhighlight' option is changed
 */
    ವರ್ಣ *
did_set_winhighlight(optset_T *args)
{
    ಪ್ರತ್ಯಾಗಮ update_winhighlight(curwin, args->os_newval.string);
}

/*
 * Expand 'winhighlight' option.
 */
    ಪೂರ್ಣಾಙ್ಕ
expand_set_winhighlight(optexpand_T *args, ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    expand_T	    *xp = args->oe_xp;

    ಯದಿ ((xp->xp_pattern > args->oe_set_arg && *(xp->xp_pattern-1) == ':')
	    || xp->xp_pattern == args->oe_set_arg || *(xp->xp_pattern-1) == ',')
    {
	// After a ':' or after a ',', or at the start, so expand highlight
	// group name.

	// If starts with !, then expand 'highlight' occasions.
	ಯದಿ (*xp->xp_pattern == '!')
	    ಪ್ರತ್ಯಾಗಮ expand_hl_occasions(args, ಅಂಕೆ_Matches, matches, '!');
	ಅನ್ಯಥಾ
	    ಪ್ರತ್ಯಾಗಮ expand_set_opt_generic(
		    args,
		    get_highlight_name,
		    ಅಂಕೆ_Matches,
		    matches);
    }

    VIM_CLEAR(*matches);
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

    ಪೂರ್ಣಾಙ್ಕ
expand_set_wincolor(optexpand_T *args, ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಪ್ರತ್ಯಾಗಮ expand_set_opt_generic(
	    args,
	    get_highlight_name,
	    ಅಂಕೆ_Matches,
	    matches);
}

#ifdef FEAT_SYN_HL
/*
 * When the 'syntax' option is set, load the syntax of that name.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
do_syntax_autocmd(ಪೂರ್ಣಾಙ್ಕ ಮೌಲ್ಯ_changed)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ syn_recursive = 0;

    ++syn_recursive;
    // Only pass TRUE for "force" when the value changed or not used
    // recursively, to avoid endless recurrence.
    apply_autocmds(EVENT_SYNTAX, curbuf->b_p_syn, curbuf->b_fname,
	    ಮೌಲ್ಯ_changed || syn_recursive == 1, curbuf);
    curbuf->b_flags |= BF_SYN_SET;
    --syn_recursive;
}
#endif

/*
 * When the 'filetype' option is set, trigger the FileType autocommand.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
do_filetype_autocmd(char_u **varp, ಪೂರ್ಣಾಙ್ಕ opt_flags, ಪೂರ್ಣಾಙ್ಕ ಮೌಲ್ಯ_changed)
{
    // Skip this when called from a modeline and the filetype was already set
    // to this value.
    ಯದಿ ((opt_flags & OPT_MODELINE) && !ಮೌಲ್ಯ_changed)
	ಪ್ರತ್ಯಾಗಮ;

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ  ft_recursive = 0;
    ಪೂರ್ಣಾಙ್ಕ	    secure_save = secure;

    // Reset the secure flag, since the value of 'filetype' has
    // been checked to be safe.
    secure = 0;

    ++ft_recursive;
    curbuf->b_did_filetype = true;
    // Only pass TRUE for "force" when the value changed or not
    // used recursively, to avoid endless recurrence.
    apply_autocmds(EVENT_FILETYPE, curbuf->b_p_ft, curbuf->b_fname,
	    ಮೌಲ್ಯ_changed || ft_recursive == 1, curbuf);
    --ft_recursive;
    // Just in case the old "curbuf" is now invalid.
    ಯದಿ (varp != &(curbuf->b_p_ft))
	varp = ರಿಕ್ತ;

    secure = secure_save;
}

#ifdef FEAT_SPELL
/*
 * When the 'spelllang' option is set, source the spell/LANG.vim file in
 * 'runtimepath'.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
do_spelllang_source(ಶೂನ್ಯ)
{
    char_u	fname[200];
    char_u	*p;
    char_u	*q = curwin->w_s->b_p_spl;

    // Skip the first name if it is "cjk".
    ಯದಿ (STRNCMP(q, "cjk,", 4) == 0)
	q += 4;

    // They could set 'spellcapcheck' depending on the language.  Use the first
    // name in 'spelllang' up to '_region' or '.encoding'.
    ಪ್ರತಿ (p = q; *p != NUL; ++p)
	ಯದಿ (!ASCII_ISALNUM(*p) && *p != '-')
	    ಭಙ್ಗ;
    ಯದಿ (p > q)
    {
	vim_snprintf((ವರ್ಣ *)fname, 200, "spell/%.*s.vim",
		(ಪೂರ್ಣಾಙ್ಕ)(p - q), q);
	source_runtime(fname, DIP_ALL);
    }
}
#endif

/*
 * Handle string options that need some action to perform when changed.
 * The new value must be allocated.
 * Returns NULL for success, or an untranslated error message for an error.
 */
    ವರ್ಣ *
did_set_string_option(
    ಪೂರ್ಣಾಙ್ಕ		opt_idx,		// index in options[] table
    char_u	**varp,			// pointer to the option variable
    char_u	*oldval,		// previous value of the option
    char_u	*ಮೌಲ್ಯ,			// new value of the option
    ವರ್ಣ	*errbuf,		// buffer for errors, or NULL
    size_t	errbuflen,		// length of error buffer
    ಪೂರ್ಣಾಙ್ಕ		opt_flags,		// OPT_LOCAL and/or OPT_GLOBAL
    set_op_T    op,			// OP_ADDING/OP_PREPENDING/OP_REMOVING
    ಪೂರ್ಣಾಙ್ಕ		*ಮೌಲ್ಯ_checked)		// value was checked to be safe, no
					// need to set P_INSECURE
{
    ವರ್ಣ	*errmsg = ರಿಕ್ತ;
    long_u	free_oldval = (get_option_flags(opt_idx) & P_ALLOCED);
    opt_did_set_cb_T did_set_cb = get_option_did_set_cb(opt_idx);
    optset_T	args;

    // 'ttytype' is an alias for 'term'.  Both 'term' and 'ttytype' point to
    // T_NAME.  If 'term' or 'ttytype' is modified, then use the index for the
    // 'term' option.  Only set the P_ALLOCED flag on 'term'.
    ಯದಿ (varp == &T_NAME)
    {
	opt_idx = findoption((char_u *)"term");
	ಯದಿ (opt_idx >= 0)
	{
	    free_oldval = (get_option_flags(opt_idx) & P_ALLOCED);
	    did_set_cb = get_option_did_set_cb(opt_idx);
	}
    }

    CLEAR_FIELD(args);

    // Disallow changing some options from secure mode
    ಯದಿ ((secure
#ifdef HAVE_SANDBOX
		|| sandbox != 0
#endif
		) && (get_option_flags(opt_idx) & P_SECURE))
	errmsg = e_not_allowed_here;
    // Check for a "normal" directory or file name in some options.
    ಅನ್ಯಥಾ ಯದಿ (check_illegal_path_names(opt_idx, varp))
	errmsg = e_invalid_argument;
    ಅನ್ಯಥಾ ಯದಿ (did_set_cb != ರಿಕ್ತ)
    {
	args.os_varp = (char_u *)varp;
	args.os_idx = opt_idx;
	args.os_flags = opt_flags;
	args.os_op = op;
	args.os_oldval.string = oldval;
	args.os_newval.string = ಮೌಲ್ಯ;
	args.os_errbuf = errbuf;
	args.os_errbuflen = errbuflen;
	// Invoke the option specific callback function to validate and apply
	// the new option value.
	errmsg = did_set_cb(&args);

	// The 'keymap', 'filetype' and 'syntax' option callback functions
	// may change the os_value_checked field.
	*ಮೌಲ್ಯ_checked = args.os_ಮೌಲ್ಯ_checked;
    }

    // If an error is detected, restore the previous value.
    ಯದಿ (errmsg != ರಿಕ್ತ)
    {
	free_string_option(*varp);
	*varp = oldval;
	// When resetting some values, need to act on it.
	ಯದಿ (args.os_restore_chartab)
	    (ಶೂನ್ಯ)init_chartab();
	ಯದಿ (varp == &p_hl)
	    (ಶೂನ್ಯ)highlight_changed();
    }
    ಅನ್ಯಥಾ
    {
#ifdef FEAT_EVAL
	// Remember where the option was set.
	set_option_sctx_idx(opt_idx, opt_flags, current_sctx);
#endif
	// Free string options that are in allocated memory.
	// Use "free_oldval", because recursiveness may change the flags under
	// our fingers (esp. init_highlight()).
	ಯದಿ (free_oldval)
	    free_string_option(oldval);
	set_option_flag(opt_idx, P_ALLOCED);

	ಯದಿ ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0
		&& is_global_local_option(opt_idx))
	{
	    // global option with local value set to use global value; free
	    // the local value and make it empty
	    char_u *p = get_option_varp_scope(opt_idx, OPT_LOCAL);
	    free_string_option(*(char_u **)p);
	    *(char_u **)p = empty_option;
	}

	// May set global value for local option.
	ಅನ್ಯಥಾ ಯದಿ (!(opt_flags & OPT_LOCAL) && opt_flags != OPT_GLOBAL)
	    set_string_option_global(opt_idx, varp);

	// Trigger the autocommand only after setting the flags.
#ifdef FEAT_SYN_HL
	ಯದಿ (varp == &(curbuf->b_p_syn))
	    do_syntax_autocmd(args.os_ಮೌಲ್ಯ_changed);
#endif
	ಅನ್ಯಥಾ ಯದಿ (varp == &(curbuf->b_p_ft))
	    do_filetype_autocmd(varp, opt_flags, args.os_ಮೌಲ್ಯ_changed);
#ifdef FEAT_SPELL
	ಯದಿ (varp == &(curwin->w_s->b_p_spl))
	    do_spelllang_source();
#endif
    }

    ಯದಿ (varp == &p_mouse)
    {
	ಯದಿ (*p_mouse == NUL)
	    mch_setmouse(FALSE);    // switch mouse off
	ಅನ್ಯಥಾ
	    setmouse();		    // in case 'mouse' changed
    }

    ಯದಿ (varp == &p_rtp)
    {
	export_myvimdir();
#ಯದಿ defined(FEAT_LUA)
	update_package_paths_in_lua();
#endif
    }

#ಯದಿ defined(FEAT_LINEBREAK)
    // Changing Formatlistpattern when briopt includes the list setting:
    // redraw
    ಯದಿ ((varp == &p_flp || varp == &(curbuf->b_p_flp))
	    && curwin->w_briopt_list)
	redraw_all_later(UPD_NOT_VALID);
#endif

    ಯದಿ (curwin->w_curswant != MAXCOL
		   && (get_option_flags(opt_idx) & (P_CURSWANT | P_RALL)) != 0
				&& (get_option_flags(opt_idx) & P_HLONLY) == 0)
	curwin->w_set_curswant = true;

    ಯದಿ ((opt_flags & OPT_NO_REDRAW) == 0)
    {
#ifdef FEAT_GUI
	// set when changing an option that only requires a redraw in the GUI
	ಪೂರ್ಣಾಙ್ಕ	redraw_gui_only = FALSE;

	ಯದಿ (varp == &p_go			// 'guioptions'
		|| varp == &p_guifont		// 'guifont'
# ifdef FEAT_GUI_TABLINE
		|| varp == &p_gtl		// 'guitablabel'
		|| varp == &p_gtt		// 'guitabtooltip'
# endif
# ifdef FEAT_XFONTSET
		|| varp == &p_guifontset	// 'guifontset'
# endif
		|| varp == &p_guifontwide	// 'guifontwide'
# ಯದಿ defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
		|| varp == &p_guiligatures	// 'guiligatures'
# endif
	    )
	    redraw_gui_only = TRUE;

	// check redraw when it's not a GUI option or the GUI is active.
	ಯದಿ (!redraw_gui_only || gui.in_use)
#endif
	    check_redraw(get_option_flags(opt_idx));
    }

#ಯದಿ defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
    ಯದಿ (args.os_did_swaptcap)
    {
	set_termname((char_u *)"win32");
	init_highlight(TRUE, FALSE);
    }
#endif

    ಪ್ರತ್ಯಾಗಮ errmsg;
}

/*
 * Check an option that can be a range of string values.
 *
 * Return OK for correct value, FAIL otherwise.
 * Empty is always OK.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
check_opt_strings(
    char_u	*val,
    ವರ್ಣ	**ಮೌಲ್ಯಗಳು,
    ಪೂರ್ಣಾಙ್ಕ		list)	    // when TRUE: accept a list of values
{
    ಪ್ರತ್ಯಾಗಮ opt_strings_flags(val, ಮೌಲ್ಯಗಳು, ರಿಕ್ತ, list);
}

/*
 * Handle an option that can be a range of string values.
 * Set a flag in "*flagp" for each string present.
 *
 * Return OK for correct value, FAIL otherwise.
 * Empty is always OK.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
opt_strings_flags(
    char_u	*val,		// new value
    ವರ್ಣ	**ಮೌಲ್ಯಗಳು,	// array of valid string values
    unsigned	*flagp,
    ಪೂರ್ಣಾಙ್ಕ		list)		// when TRUE: accept a list of values
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		len;
    unsigned	new_flags = 0;

    ಯಾವತ್ (*val)
    {
	ಪ್ರತಿ (i = 0; ; ++i)
	{
	    ಯದಿ (ಮೌಲ್ಯಗಳು[i] == ರಿಕ್ತ)	// val not found in values[]
		ಪ್ರತ್ಯಾಗಮ FAIL;

	    len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(ಮೌಲ್ಯಗಳು[i]);
	    ಯದಿ (STRNCMP(ಮೌಲ್ಯಗಳು[i], val, len) == 0
		    && ((list && val[len] == ',') || val[len] == NUL))
	    {
		val += len + (val[len] == ',');
		new_flags |= (1 << i);
		ಭಙ್ಗ;		// check next item in val list
	    }
	}
    }
    ಯದಿ (flagp != ರಿಕ್ತ)
	*flagp = new_flags;

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * return OK if "p" is a valid fileformat name, FAIL otherwise.
 */
    ಪೂರ್ಣಾಙ್ಕ
check_ff_ಮೌಲ್ಯ(char_u *p)
{
    ಪ್ರತ್ಯಾಗಮ check_opt_strings(p, p_ff_ಮೌಲ್ಯಗಳು, FALSE);
}

/*
 * Save the actual shortmess Flags and clear them temporarily to avoid that
 * file messages overwrites any output from the following commands.
 *
 * Caller must make sure to first call save_clear_shm_value() and then
 * restore_shm_value() exactly the same number of times.
 */
    ಶೂನ್ಯ
save_clear_shm_ಮೌಲ್ಯ(ಶೂನ್ಯ)
{
    ಯದಿ (STRLEN(p_shm) >= SHM_LEN)
    {
	iemsg(e_internal_error_shortmess_too_long);
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (++set_shm_recursive == 1)
    {
	STRCPY(shm_buf, p_shm);
	set_option_ಮೌಲ್ಯ_give_err((char_u *)"shm", 0L, (char_u *)"", 0);
    }
}

/*
 * Restore the shortmess Flags set from the save_clear_shm_value() function.
 */
    ಶೂನ್ಯ
restore_shm_ಮೌಲ್ಯ(ಶೂನ್ಯ)
{
    ಯದಿ (--set_shm_recursive == 0)
    {
	set_option_ಮೌಲ್ಯ_give_err((char_u *)"shm", 0L, shm_buf, 0);
	vim_memset(shm_buf, 0, SHM_LEN);
    }
}

/*
 * Export the environment variable $MYVIMDIR to the first item in runtimepath
 */
    ಶೂನ್ಯ
export_myvimdir(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		dofree = FALSE;
    char_u	*p;
    char_u	*q = p_rtp;
    string_T	buf;

    buf.string = alloc(MAXPATHL);
    ಯದಿ (buf.string == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    buf.length = (size_t)copy_option_part(&q, buf.string, MAXPATHL, ",");

    p = vim_getenv((char_u *)"MYVIMDIR", &dofree);

    ಯದಿ (p == ರಿಕ್ತ || STRCMP(p, buf.string) != 0)
    {
	ಯದಿ (*buf.string != NUL && !after_pathsep(buf.string, buf.string + buf.length))
	    STRCPY(buf.string + buf.length, PATHSEPSTR);
#ifdef MSWIN
	// normalize path separators
	ಪ್ರತಿ (q = buf.string; *q != NUL; q++)
	    ಯದಿ (*q == '/')
		*q = '\\';
#endif
	vim_setenv((char_u *)"MYVIMDIR", buf.string);
    }
    ಯದಿ (dofree)
	vim_free(p);
    vim_free(buf.string);
}
