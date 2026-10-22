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
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಗಣ_shm_recursive = 0;

ಸ್ಥಿರ ಅಕ್ಷರ *(p_ambw_ಮೌಲ್ಯಗಳು[]) = {"single", "double", ಖಾಲಿ};
ಸ್ಥಿರ ಅಕ್ಷರ *(p_bg_ಮೌಲ್ಯಗಳು[]) = {"light", "dark", ಖಾಲಿ};
ಸ್ಥಿರ ಅಕ್ಷರ *(p_bkc_ಮೌಲ್ಯಗಳು[]) = {"yes", "auto", "no", "breaksymlink", "breakhardlink", ಖಾಲಿ};
ಸ್ಥಿರ ಅಕ್ಷರ *(p_bo_ಮೌಲ್ಯಗಳು[]) = {"all", "backspace", "cursor", "complete",
				 "copy", "ctrlg", "error", "esc", "ex",
				 "hangul", "insertmode", "lang", "mess",
				 "showmatch", "operator", "register", "shell",
				 "spell", "term", "wildmode", ಖಾಲಿ};
#ಆದರೆ defined(FEAT_LINEBREAK)
// Note: Keep this in sync with briopt_check()
ಸ್ಥಿರ ಅಕ್ಷರ *(p_briopt_ಮೌಲ್ಯಗಳು[]) = {"shift:", "min:", "sbr", "list:", "column:", ಖಾಲಿ};
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_TABPANEL)
// Note: Keep this in sync with tabpanelopt_changed()
ಸ್ಥಿರ ಅಕ್ಷರ *(p_tplo_ಮೌಲ್ಯಗಳು[]) = {"align:", "columns:", "scrollbar", "vert", ಖಾಲಿ};
ಸ್ಥಿರ ಅಕ್ಷರ *(p_tplo_align_ಮೌಲ್ಯಗಳು[]) = {"left", "right", ಖಾಲಿ};
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_DIFF)
// Note: Keep this in sync with diffopt_changed()
ಸ್ಥಿರ ಅಕ್ಷರ *(p_dip_ಮೌಲ್ಯಗಳು[]) = {"filler", "anchor", "context:", "iblank", "icase", "iwhite", "iwhiteall", "iwhiteeol", "horizontal", "vertical", "closeoff", "hiddenoff", "foldcolumn:", "followwrap", "internal", "indent-heuristic", "algorithm:", "inline:", "linematch:", ಖಾಲಿ};
ಸ್ಥಿರ ಅಕ್ಷರ *(p_dip_algorithm_ಮೌಲ್ಯಗಳು[]) = {"myers", "minimal", "patience", "histogram", ಖಾಲಿ};
ಸ್ಥಿರ ಅಕ್ಷರ *(p_dip_inline_ಮೌಲ್ಯಗಳು[]) = {"none", "simple", "char", "word", ಖಾಲಿ};
#ಆದರೆ_ಅಂತ್ಯ
ಸ್ಥಿರ ಅಕ್ಷರ *(p_nf_ಮೌಲ್ಯಗಳು[]) = {"bin", "octal", "hex", "alpha", "unsigned", "blank", ಖಾಲಿ};
ಸ್ಥಿರ ಅಕ್ಷರ *(p_ff_ಮೌಲ್ಯಗಳು[]) = {FF_UNIX, FF_DOS, FF_MAC, ಖಾಲಿ};
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_CLIPMETHOD
// Note: Keep this in sync with did_set_clipboard()
ಸ್ಥಿರ ಅಕ್ಷರ *(p_cb_ಮೌಲ್ಯಗಳು[]) = {"unnamed", "unnamedplus",
# ifdef FEAT_CLIPBOARD
    "autoselect", "autoselectplus", "autoselectml", "html", "exclude:",
# endif
    ಖಾಲಿ};
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CRYPT
ಸ್ಥಿರ ಅಕ್ಷರ *(p_cm_ಮೌಲ್ಯಗಳು[]) = {"zip", "blowfish", "blowfish2",
# ifdef FEAT_SODIUM
    "xchacha20", "xchacha20v2",
# endif
    ಖಾಲಿ};
#ಆದರೆ_ಅಂತ್ಯ
ಸ್ಥಿರ ಅಕ್ಷರ *(p_cmp_ಮೌಲ್ಯಗಳು[]) = {"internal", "keepascii", ಖಾಲಿ};
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
// Note: Keep this in sync with fill_culopt_flags()
ಸ್ಥಿರ ಅಕ್ಷರ *(p_culopt_ಮೌಲ್ಯಗಳು[]) = {"line", "screenline", "number", "both", ಖಾಲಿ};
#ಆದರೆ_ಅಂತ್ಯ
ಸ್ಥಿರ ಅಕ್ಷರ *(p_dy_ಮೌಲ್ಯಗಳು[]) = {"lastline", "truncate", "uhex", ಖಾಲಿ};
ಸ್ಥಿರ ಅಕ್ಷರ *(p_jop_ಮೌಲ್ಯಗಳು[]) = {"stack", ಖಾಲಿ};
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
ಸ್ಥಿರ ಅಕ್ಷರ *(p_fdo_ಮೌಲ್ಯಗಳು[]) = {"all", "block", "hor", "mark", "percent",
				 "quickfix", "search", "tag", "insert",
				 "undo", "jump", ಖಾಲಿ};
#ಆದರೆ_ಅಂತ್ಯ
// Note: Keep this in sync with match_keyprotocol()
ಸ್ಥಿರ ಅಕ್ಷರ *(p_kpc_protocol_ಮೌಲ್ಯಗಳು[]) = {"none", "mok2", "kitty", ಖಾಲಿ};
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_IMAGE
// Note: Keep this in sync with match_imageprotocol()
ಸ್ಥಿರ ಅಕ್ಷರ *(p_ipc_protocol_ಮೌಲ್ಯಗಳು[]) = {"none", "kitty", "sixel", ಖಾಲಿ};
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
// Note: Keep this in sync with parse_popup_option()
ಸ್ಥಿರ ಅಕ್ಷರ *(p_popup_cpp_option_ಮೌಲ್ಯಗಳು[]) = {"align:", "border:",
    "borderhighlight:", "close:", "height:", "highlight:", "opacity:",
    "resize:", "shadow:", "width:", ಖಾಲಿ};
ಸ್ಥಿರ ಅಕ್ಷರ *(p_popup_pvp_option_ಮೌಲ್ಯಗಳು[]) = {"border:",
    "borderhighlight:", "close:", "height:", "highlight:", "opacity:",
    "resize:", "shadow:", "width:", ಖಾಲಿ};
ಸ್ಥಿರ ಅಕ್ಷರ *(p_popup_option_on_off_ಮೌಲ್ಯಗಳು[]) = {"on", "off", ಖಾಲಿ};
ಸ್ಥಿರ ಅಕ್ಷರ *(p_popup_cpp_border_ಮೌಲ್ಯಗಳು[]) = {"single", "double", "round",
    "ascii", "on", "off", "custom:", ಖಾಲಿ};
ಸ್ಥಿರ ಅಕ್ಷರ *(p_popup_pvp_border_ಮೌಲ್ಯಗಳು[]) = {"single", "double", "round",
    "ascii", "on", "off", "custom:", ಖಾಲಿ};
ಸ್ಥಿರ ಅಕ್ಷರ *(p_popup_option_align_ಮೌಲ್ಯಗಳು[]) = {"item", "menu", ಖಾಲಿ};
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_SPELL)
// Note: Keep this in sync with spell_check_sps()
ಸ್ಥಿರ ಅಕ್ಷರ *(p_sps_ಮೌಲ್ಯಗಳು[]) = {"best", "fast", "double", "expr:", "file:", "timeout:", ಖಾಲಿ};
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SESSION
// Also used for 'viewoptions'!  Keep in sync with SSOP_ flags.
ಸ್ಥಿರ ಅಕ್ಷರ *(p_ssop_ಮೌಲ್ಯಗಳು[]) = {"buffers", "winpos", "resize", "winsize",
    "localoptions", "options", "help", "blank", "globals", "slash", "unix",
    "sesdir", "curdir", "folds", "cursor", "tabpages", "terminal", "skiprtp",
    ಖಾಲಿ};
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_STL_OPT)
ಸ್ಥಿರ ಅಕ್ಷರ *(p_stlo_ಮೌಲ್ಯಗಳು[]) = {"fixedheight", "maxheight:", ಖಾಲಿ};
#ಆದರೆ_ಅಂತ್ಯ
// Keep in sync with SWB_ flags in option.h
ಸ್ಥಿರ ಅಕ್ಷರ *(p_swb_ಮೌಲ್ಯಗಳು[]) = {"useopen", "usetab", "split", "newtab", "vsplit", "uselast", ಖಾಲಿ};
ಸ್ಥಿರ ಅಕ್ಷರ *(p_spk_ಮೌಲ್ಯಗಳು[]) = {"cursor", "screen", "topline", ಖಾಲಿ};
ಸ್ಥಿರ ಅಕ್ಷರ *(p_tc_ಮೌಲ್ಯಗಳು[]) = {"followic", "ignore", "match", "followscs", "smart", ಖಾಲಿ};
// Keep in sync with TCL_ flags in option.h
ಸ್ಥಿರ ಅಕ್ಷರ *(p_tcl_ಮೌಲ್ಯಗಳು[]) = {"left", "uselast", ಖಾಲಿ};
#ಆದರೆ defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
ಸ್ಥಿರ ಅಕ್ಷರ *(p_toolbar_ಮೌಲ್ಯಗಳು[]) = {"text", "icons", "tooltips", "horiz", ಖಾಲಿ};
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_TOOLBAR) && defined(FEAT_GUI_GTK)
ಸ್ಥಿರ ಅಕ್ಷರ *(p_tbis_ಮೌಲ್ಯಗಳು[]) = {"tiny", "small", "medium", "large", "huge", "giant", ಖಾಲಿ};
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(UNIX) || defined(VMS)
ಸ್ಥಿರ ಅಕ್ಷರ *(p_ttym_ಮೌಲ್ಯಗಳು[]) = {"xterm", "xterm2", "dec", "netterm", "jsbterm", "pterm", "urxvt", "sgr", ಖಾಲಿ};
#ಆದರೆ_ಅಂತ್ಯ
ಸ್ಥಿರ ಅಕ್ಷರ *(p_ve_ಮೌಲ್ಯಗಳು[]) = {"block", "insert", "all", "onemore", "none", "NONE", ಖಾಲಿ};
// Note: Keep this in sync with check_opt_wim()
ಸ್ಥಿರ ಅಕ್ಷರ *(p_wim_ಮೌಲ್ಯಗಳು[]) = {"full", "longest", "list", "lastused", "noselect", "noinsert", ಖಾಲಿ};
ಸ್ಥಿರ ಅಕ್ಷರ *(p_wop_ಮೌಲ್ಯಗಳು[]) = {"fuzzy", "tagfile", "pum", "exacttext", ಖಾಲಿ};
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_WAK
ಸ್ಥಿರ ಅಕ್ಷರ *(p_wak_ಮೌಲ್ಯಗಳು[]) = {"yes", "menu", "no", ಖಾಲಿ};
#ಆದರೆ_ಅಂತ್ಯ
ಸ್ಥಿರ ಅಕ್ಷರ *(p_mousem_ಮೌಲ್ಯಗಳು[]) = {"extend", "popup", "popup_setpos", "mac", ಖಾಲಿ};
ಸ್ಥಿರ ಅಕ್ಷರ *(p_sel_ಮೌಲ್ಯಗಳು[]) = {"inclusive", "exclusive", "old", ಖಾಲಿ};
ಸ್ಥಿರ ಅಕ್ಷರ *(p_slm_ಮೌಲ್ಯಗಳು[]) = {"mouse", "key", "cmd", ಖಾಲಿ};
ಸ್ಥಿರ ಅಕ್ಷರ *(p_km_ಮೌಲ್ಯಗಳು[]) = {"startsel", "stopsel", ಖಾಲಿ};
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_BROWSE
ಸ್ಥಿರ ಅಕ್ಷರ *(p_bsdir_ಮೌಲ್ಯಗಳು[]) = {"current", "last", "buffer", ಖಾಲಿ};
#ಆದರೆ_ಅಂತ್ಯ
ಸ್ಥಿರ ಅಕ್ಷರ *(p_scbopt_ಮೌಲ್ಯಗಳು[]) = {"ver", "hor", "jump", ಖಾಲಿ};
ಸ್ಥಿರ ಅಕ್ಷರ *(p_debug_ಮೌಲ್ಯಗಳು[]) = {"msg", "throw", "beep", ಖಾಲಿ};
ಸ್ಥಿರ ಅಕ್ಷರ *(p_ead_ಮೌಲ್ಯಗಳು[]) = {"both", "ver", "hor", ಖಾಲಿ};
ಸ್ಥಿರ ಅಕ್ಷರ *(p_buftype_ಮೌಲ್ಯಗಳು[]) = {"nofile", "nowrite", "quickfix", "help", "terminal", "acwrite", "prompt", "popup", ಖಾಲಿ};
ಸ್ಥಿರ ಅಕ್ಷರ *(p_bufhidden_ಮೌಲ್ಯಗಳು[]) = {"hide", "unload", "delete", "wipe", ಖಾಲಿ};
ಸ್ಥಿರ ಅಕ್ಷರ *(p_bs_ಮೌಲ್ಯಗಳು[]) = {"indent", "eol", "start", "nostop", ಖಾಲಿ};
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
ಸ್ಥಿರ ಅಕ್ಷರ *(p_fdm_ಮೌಲ್ಯಗಳು[]) = {"manual", "expr", "marker", "indent", "syntax",
# ifdef FEAT_DIFF
				"diff",
# endif
				ಖಾಲಿ};
ಸ್ಥಿರ ಅಕ್ಷರ *(p_fcl_ಮೌಲ್ಯಗಳು[]) = {"all", ಖಾಲಿ};
#ಆದರೆ_ಅಂತ್ಯ
ಸ್ಥಿರ ಅಕ್ಷರ *(p_cfc_ಮೌಲ್ಯಗಳು[]) = {"keyword", "files", "whole_line", ಖಾಲಿ};
ಸ್ಥಿರ ಅಕ್ಷರ *(p_cot_ಮೌಲ್ಯಗಳು[]) = {"menu", "menuone", "longest", "preview", "popup", "popuphidden", "noinsert", "noselect", "fuzzy", "nosort", "preinsert", "nearest", ಖಾಲಿ};
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ BACKSLASH_ಒಳ_FILENAME
ಸ್ಥಿರ ಅಕ್ಷರ *(p_csl_ಮೌಲ್ಯಗಳು[]) = {"slash", "backslash", ಖಾಲಿ};
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SIGNS
ಸ್ಥಿರ ಅಕ್ಷರ *(p_scl_ಮೌಲ್ಯಗಳು[]) = {"yes", "no", "auto", "number", ಖಾಲಿ};
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
ಸ್ಥಿರ ಅಕ್ಷರ *(p_trz_ಮೌಲ್ಯಗಳು[]) = {"inband", "sigwinch", "", ಖಾಲಿ};
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(MSWIN) && defined(FEAT_TERMINAL)
ಸ್ಥಿರ ಅಕ್ಷರ *(p_twt_ಮೌಲ್ಯಗಳು[]) = {"winpty", "conpty", "", ಖಾಲಿ};
#ಆದರೆ_ಅಂತ್ಯ
ಸ್ಥಿರ ಅಕ್ಷರ *(p_sloc_ಮೌಲ್ಯಗಳು[]) = {"last", "statusline", "tabline", ಖಾಲಿ};
ಸ್ಥಿರ ಅಕ್ಷರ *(p_sws_ಮೌಲ್ಯಗಳು[]) = {"fsync", "sync", ಖಾಲಿ};

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ check_opt_ಅಕ್ಷರಸರಣಿಗಳು(char_u *val, ಅಕ್ಷರ **ಮೌಲ್ಯಗಳು, ಪೂರ್ಣಾಂಕ list);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ opt_ಅಕ್ಷರಸರಣಿಗಳು_flags(char_u *val, ಅಕ್ಷರ **ಮೌಲ್ಯಗಳು, ಚಿಹ್ನೆರಹಿತ *flagp, ಪೂರ್ಣಾಂಕ list);

/*
 * After setting various option values: recompute variables that depend on
 * option values.
 */
    ಶೂನ್ಯ
didset_string_options(ಶೂನ್ಯ)
{
    (ಶೂನ್ಯ)opt_ಅಕ್ಷರಸರಣಿಗಳು_flags(p_cmp, p_cmp_ಮೌಲ್ಯಗಳು, &cmp_flags, TRUE);
    (ಶೂನ್ಯ)opt_ಅಕ್ಷರಸರಣಿಗಳು_flags(p_bkc, p_bkc_ಮೌಲ್ಯಗಳು, &bkc_flags, TRUE);
    (ಶೂನ್ಯ)opt_ಅಕ್ಷರಸರಣಿಗಳು_flags(p_bo, p_bo_ಮೌಲ್ಯಗಳು, &bo_flags, TRUE);
    (ಶೂನ್ಯ)opt_ಅಕ್ಷರಸರಣಿಗಳು_flags(p_cot, p_cot_ಮೌಲ್ಯಗಳು, &cot_flags, TRUE);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SESSION
    (ಶೂನ್ಯ)opt_ಅಕ್ಷರಸರಣಿಗಳು_flags(p_ssop, p_ssop_ಮೌಲ್ಯಗಳು, &ssop_flags, TRUE);
    (ಶೂನ್ಯ)opt_ಅಕ್ಷರಸರಣಿಗಳು_flags(p_vop, p_ssop_ಮೌಲ್ಯಗಳು, &vop_flags, TRUE);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
    (ಶೂನ್ಯ)opt_ಅಕ್ಷರಸರಣಿಗಳು_flags(p_fdo, p_fdo_ಮೌಲ್ಯಗಳು, &fdo_flags, TRUE);
#ಆದರೆ_ಅಂತ್ಯ
    (ಶೂನ್ಯ)opt_ಅಕ್ಷರಸರಣಿಗಳು_flags(p_dy, p_dy_ಮೌಲ್ಯಗಳು, &dy_flags, TRUE);
    (ಶೂನ್ಯ)opt_ಅಕ್ಷರಸರಣಿಗಳು_flags(p_jop, p_jop_ಮೌಲ್ಯಗಳು, &jop_flags, TRUE);
    (ಶೂನ್ಯ)opt_ಅಕ್ಷರಸರಣಿಗಳು_flags(p_tc, p_tc_ಮೌಲ್ಯಗಳು, &tc_flags, FALSE);
    (ಶೂನ್ಯ)opt_ಅಕ್ಷರಸರಣಿಗಳು_flags(p_ve, p_ve_ಮೌಲ್ಯಗಳು, &ve_flags, TRUE);
#ಆದರೆ defined(UNIX) || defined(VMS)
    (ಶೂನ್ಯ)opt_ಅಕ್ಷರಸರಣಿಗಳು_flags(p_ttym, p_ttym_ಮೌಲ್ಯಗಳು, &ttym_flags, FALSE);
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
    (ಶೂನ್ಯ)opt_ಅಕ್ಷರಸರಣಿಗಳು_flags(p_toolbar, p_toolbar_ಮೌಲ್ಯಗಳು, &toolbar_flags, TRUE);
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_TOOLBAR) && defined(FEAT_GUI_GTK)
    (ಶೂನ್ಯ)opt_ಅಕ್ಷರಸರಣಿಗಳು_flags(p_tbis, p_tbis_ಮೌಲ್ಯಗಳು, &tbis_flags, FALSE);
#ಆದರೆ_ಅಂತ್ಯ
    (ಶೂನ್ಯ)opt_ಅಕ್ಷರಸರಣಿಗಳು_flags(p_swb, p_swb_ಮೌಲ್ಯಗಳು, &swb_flags, TRUE);
    (ಶೂನ್ಯ)opt_ಅಕ್ಷರಸರಣಿಗಳು_flags(p_tcl, p_tcl_ಮೌಲ್ಯಗಳು, &tcl_flags, TRUE);
}

#ಆದರೆ defined(FEAT_EVAL)
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
	ಪೂರ್ಣಾಂಕ	opt_idx,
	ಪೂರ್ಣಾಂಕ	opt_flags,
	char_u  *oldval,
	char_u  *oldval_l,
	char_u  *oldval_g,
	char_u  *newval)
{
    // Don't do this recursively.
    ಆದರೆ (oldval == ಖಾಲಿ || newval == ಖಾಲಿ
				    || *get_vim_var_str(VV_OPTION_TYPE) != NUL)
	ಹಿಂತಿರುಗಿ;

    char_u buf_type[7];
    size_t buf_typelen;
    size_t oldvallen;

    oldvallen = STRLEN(oldval);
    ಗಣ_vim_var_string(VV_OPTION_OLD, oldval, (ಪೂರ್ಣಾಂಕ)oldvallen);
    ಗಣ_vim_var_string(VV_OPTION_NEW, newval, -1);
    buf_typelen = vim_snprintf_safelen((ಅಕ್ಷರ *)buf_type, ಗಾತ್ರ(buf_type),
	"%s", (opt_flags & OPT_LOCAL) ? "local" : "global");
    ಗಣ_vim_var_string(VV_OPTION_TYPE, buf_type, (ಪೂರ್ಣಾಂಕ)buf_typelen);
    ಆದರೆ (opt_flags & OPT_LOCAL)
    {
	ಗಣ_vim_var_string(VV_OPTION_COMMAND, (char_u *)"setlocal", STRLEN_LITERAL("setlocal"));
	ಗಣ_vim_var_string(VV_OPTION_OLDLOCAL, oldval, (ಪೂರ್ಣಾಂಕ)oldvallen);
    }
    ಆದರೆ (opt_flags & OPT_GLOBAL)
    {
	ಗಣ_vim_var_string(VV_OPTION_COMMAND, (char_u *)"setglobal", STRLEN_LITERAL("setglobal"));
	ಗಣ_vim_var_string(VV_OPTION_OLDGLOBAL, oldval, (ಪೂರ್ಣಾಂಕ)oldvallen);
    }
    ಆದರೆ ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	ಗಣ_vim_var_string(VV_OPTION_COMMAND, (char_u *)"set", STRLEN_LITERAL("set"));
	ಗಣ_vim_var_string(VV_OPTION_OLDLOCAL, oldval_l, -1);
	ಗಣ_vim_var_string(VV_OPTION_OLDGLOBAL, oldval_g, -1);
    }
    ಆದರೆ (opt_flags & OPT_MODELINE)
    {
	ಗಣ_vim_var_string(VV_OPTION_COMMAND, (char_u *)"modeline", STRLEN_LITERAL("modeline"));
	ಗಣ_vim_var_string(VV_OPTION_OLDLOCAL, oldval, (ಪೂರ್ಣಾಂಕ)oldvallen);
    }
    apply_autocmds(EVENT_OPTIONSET,
	    get_option_fullname(opt_idx), ಖಾಲಿ, FALSE,
	    ಖಾಲಿ);
    reset_v_option_vars();
}
#ಆದರೆ_ಅಂತ್ಯ

    ಸ್ಥಿರ ಅಕ್ಷರ *
illegal_char(ಅಕ್ಷರ *errbuf, size_t errbuflen, ಪೂರ್ಣಾಂಕ c)
{
    ಆದರೆ (errbuf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ "";
    vim_snprintf(errbuf, errbuflen, _(e_illegal_character_str),
		    (ಅಕ್ಷರ *)transchar(c));
    ಹಿಂತಿರುಗಿ errbuf;
}

    ಸ್ಥಿರ ಅಕ್ಷರ *
illegal_char_after_chr(ಅಕ್ಷರ *errbuf, size_t errbuflen, ಪೂರ್ಣಾಂಕ c)
{
    ಆದರೆ (errbuf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ "";
    vim_snprintf(errbuf, errbuflen, _(e_illegal_character_after_chr), c);
    ಹಿಂತಿರುಗಿ errbuf;
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
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕಂಡುಹಿಡಿ_ID
    check_string_option(&buf->b_p_def);
    check_string_option(&buf->b_p_inc);
# ifdef FEAT_EVAL
    check_string_option(&buf->b_p_inex);
# endif
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_EVAL)
    check_string_option(&buf->b_p_inde);
    check_string_option(&buf->b_p_indk);
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_BEVAL) && defined(FEAT_EVAL)
    check_string_option(&buf->b_p_bexpr);
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_CRYPT)
    check_string_option(&buf->b_p_cm);
#ಆದರೆ_ಅಂತ್ಯ
    check_string_option(&buf->b_p_fp);
#ಆದರೆ defined(FEAT_EVAL)
    check_string_option(&buf->b_p_fex);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CRYPT
    check_string_option(&buf->b_p_ಕೀಲಿ);
#ಆದರೆ_ಅಂತ್ಯ
    check_string_option(&buf->b_p_kp);
    check_string_option(&buf->b_p_mps);
    check_string_option(&buf->b_p_fo);
    check_string_option(&buf->b_p_flp);
    check_string_option(&buf->b_p_isk);
    check_string_option(&buf->b_p_com);
    check_string_option(&buf->b_p_cms);
    check_string_option(&buf->b_p_nf);
    check_string_option(&buf->b_p_qe);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
    check_string_option(&buf->b_p_syn);
    check_string_option(&buf->b_s.b_syn_isk);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SPELL
    check_string_option(&buf->b_s.b_p_spc);
    check_string_option(&buf->b_s.b_p_spf);
    check_string_option(&buf->b_s.b_p_spl);
    check_string_option(&buf->b_s.b_p_spo);
#ಆದರೆ_ಅಂತ್ಯ
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
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_COMPL_FUNC
    check_string_option(&buf->b_p_cfu);
    check_string_option(&buf->b_p_ofu);
    check_string_option(&buf->b_p_tsrfu);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    check_string_option(&buf->b_p_ffu);
    check_string_option(&buf->b_p_tfu);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_KEYMAP
    check_string_option(&buf->b_p_keymap);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
    check_string_option(&buf->b_p_gefm);
    check_string_option(&buf->b_p_gp);
    check_string_option(&buf->b_p_mp);
    check_string_option(&buf->b_p_efm);
#ಆದರೆ_ಅಂತ್ಯ
    check_string_option(&buf->b_p_ep);
    check_string_option(&buf->b_p_ಮಾರ್ಗ);
    check_string_option(&buf->b_p_ಟ್ಯಾಗುಗಳು);
    check_string_option(&buf->b_p_tc);
    check_string_option(&buf->b_p_dict);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
    check_string_option(&buf->b_p_dia);
#ಆದರೆ_ಅಂತ್ಯ
    check_string_option(&buf->b_p_tsr);
    check_string_option(&buf->b_p_lw);
    check_string_option(&buf->b_p_bkc);
    check_string_option(&buf->b_p_menc);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VARTABS
    check_string_option(&buf->b_p_vsts);
    check_string_option(&buf->b_p_vts);
#ಆದರೆ_ಅಂತ್ಯ
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
    ಆದರೆ (p != ಖಾಲಿಗಳು_option)
	vim_free(p);
}

    ಶೂನ್ಯ
clear_string_option(char_u **pp)
{
    ಆದರೆ (*pp != ಖಾಲಿಗಳು_option)
	vim_free(*pp);
    *pp = ಖಾಲಿಗಳು_option;
}

    ಶೂನ್ಯ
check_string_option(char_u **pp)
{
    ಆದರೆ (*pp == ಖಾಲಿ)
	*pp = ಖಾಲಿಗಳು_option;
}

/*
 * Set global value for string option when it's a local option.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಗಣ_string_option_global(
    ಪೂರ್ಣಾಂಕ		opt_idx,	// option index
    char_u	**varp)		// pointer to option variable
{
    char_u	**p, *s;

    // the global value is always allocated
    ಆದರೆ (is_ಕಿಟಕಿ_local_option(opt_idx))
	p = (char_u **)GLOBAL_WO(varp);
    ಇಲ್ಲದಿದ್ದರೆ
	p = (char_u **)get_option_var(opt_idx);
    ಆದರೆ (!is_global_option(opt_idx)
	    && p != varp
	    && (s = vim_strsave(*varp)) != ಖಾಲಿ)
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
ಗಣ_string_option_ನೇರ(
    char_u	*name,
    ಪೂರ್ಣಾಂಕ		opt_idx,
    char_u	*val,
    ಪೂರ್ಣಾಂಕ		opt_flags,	// OPT_FREE, OPT_LOCAL and/or OPT_GLOBAL
    ಪೂರ್ಣಾಂಕ		ಗಣ_sid ಬಳಸದ)
{
    char_u	*s;
    char_u	**varp;
    ಪೂರ್ಣಾಂಕ		both = (opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0;
    ಪೂರ್ಣಾಂಕ		idx = opt_idx;

    ಆದರೆ (idx == -1)		// use name
    {
	idx = findoption(name);
	ಆದರೆ (idx < 0)	// not found (should not happen)
	{
	    semsg(_(e_ಆಂತರಿಕ_error_str), "set_string_option_direct()");
	    siemsg("For option %s", name);
	    ಹಿಂತಿರುಗಿ;
	}
    }

    ಆದರೆ (is_hidden_option(idx))		// can't set hidden option
	ಹಿಂತಿರುಗಿ;

    s = vim_strsave(val);
    ಆದರೆ (s == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    varp = (char_u **)get_option_varp_scope(idx,
	    both ? OPT_LOCAL : opt_flags);
    ಆದರೆ ((opt_flags & OPT_FREE) && (get_option_flags(idx) & P_ALLOCED))
	free_string_option(*varp);
    *varp = s;

    // For buffer/window local option may also set the global value.
    ಆದರೆ (both)
	ಗಣ_string_option_global(idx, varp);

    ಗಣ_option_flag(idx, P_ALLOCED);

    // When setting both values of a global option with a local value,
    // make the local value empty, so that the global value is used.
    ಆದರೆ (is_global_local_option(idx) && both)
    {
	free_string_option(*varp);
	*varp = ಖಾಲಿಗಳು_option;
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಆದರೆ (ಗಣ_sid != SID_NONE)
    {
	sctx_T script_ctx;

	ಆದರೆ (ಗಣ_sid == 0)
	    script_ctx = current_sctx;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    script_ctx.sc_sid = ಗಣ_sid;
	    script_ctx.sc_seq = 0;
	    script_ctx.sc_lnum = 0;
	    script_ctx.sc_ಆವೃತ್ತಿ = 1;
	}
	ಗಣ_option_sctx_idx(idx, opt_flags, script_ctx);
    }
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Like set_string_option_direct(), but for a window-local option in "wp".
 * Blocks autocommands to avoid the old curwin becoming invalid.
 */
    ಶೂನ್ಯ
ಗಣ_string_option_ನೇರ_ಒಳ_win(
	win_T		*wp,
	char_u		*name,
	ಪೂರ್ಣಾಂಕ		opt_idx,
	char_u		*val,
	ಪೂರ್ಣಾಂಕ		opt_flags,
	ಪೂರ್ಣಾಂಕ		ಗಣ_sid)
{
    win_T	*save_curwin = curwin;

    ಖಂಡ_autocmds();
    curwin = wp;
    curbuf = curwin->w_ಬಫರ್;
    ಗಣ_string_option_ನೇರ(name, opt_idx, val, opt_flags, ಗಣ_sid);
    curwin = save_curwin;
    curbuf = curwin->w_ಬಫರ್;
    unblock_autocmds();
}

#ಆದರೆ defined(FEAT_PROP_POPUP)
/*
 * Like set_string_option_direct(), but for a buffer-local option in "buf".
 * Blocks autocommands to avoid the old curbuf becoming invalid.
 */
    ಶೂನ್ಯ
ಗಣ_string_option_ನೇರ_ಒಳ_buf(
	buf_T		*buf,
	char_u		*name,
	ಪೂರ್ಣಾಂಕ		opt_idx,
	char_u		*val,
	ಪೂರ್ಣಾಂಕ		opt_flags,
	ಪೂರ್ಣಾಂಕ		ಗಣ_sid)
{
    buf_T	*save_curbuf = curbuf;

    ಖಂಡ_autocmds();
    curbuf = buf;
    curwin->w_ಬಫರ್ = curbuf;
    ಗಣ_string_option_ನೇರ(name, opt_idx, val, opt_flags, ಗಣ_sid);
    curbuf = save_curbuf;
    curwin->w_ಬಫರ್ = curbuf;
    unblock_autocmds();
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Set a string option to a new value, and handle the effects.
 *
 * Returns NULL on success or an untranslated error message on error.
 */
    ಅಕ್ಷರ *
ಗಣ_string_option(
    ಪೂರ್ಣಾಂಕ		opt_idx,
    char_u	*ಮೌಲ್ಯ,
    ಪೂರ್ಣಾಂಕ		opt_flags,	// OPT_LOCAL and/or OPT_GLOBAL
    ಅಕ್ಷರ	*errbuf,
    size_t	errbuflen)
{
    char_u	*s;
    char_u	**varp;
    char_u	*oldval;
#ಆದರೆ defined(FEAT_EVAL)
    char_u	*oldval_l = ಖಾಲಿ;
    char_u	*oldval_g = ಖಾಲಿ;
    char_u	*saved_oldval = ಖಾಲಿ;
    char_u	*saved_oldval_l = ಖಾಲಿ;
    char_u	*saved_oldval_g = ಖಾಲಿ;
    char_u	*saved_newval = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ
    ಅಕ್ಷರ	*errmsg = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		ಮೌಲ್ಯ_checked = FALSE;

    ಆದರೆ (is_hidden_option(opt_idx))	// don't set hidden option
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    s = vim_strsave(ಮೌಲ್ಯ == ಖಾಲಿ ? (char_u *)"" : ಮೌಲ್ಯ);
    ಆದರೆ (s == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    varp = (char_u **)get_option_varp_scope(opt_idx,
	    (opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0
	    ? (is_global_local_option(opt_idx)
		? OPT_GLOBAL : OPT_LOCAL)
	    : opt_flags);
    oldval = *varp;
#ಆದರೆ defined(FEAT_EVAL)
    ಆದರೆ ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	oldval_l = *(char_u **)get_option_varp_scope(opt_idx, OPT_LOCAL);
	oldval_g = *(char_u **)get_option_varp_scope(opt_idx, OPT_GLOBAL);
    }
#ಆದರೆ_ಅಂತ್ಯ
    *varp = s;

#ಆದರೆ defined(FEAT_EVAL)
    ಆದರೆ (!starting
# ifdef FEAT_CRYPT
	    && !is_crypt_ಕೀಲಿ_option(opt_idx)
# endif
       )
    {
	ಆದರೆ (oldval_l != ಖಾಲಿ)
	    saved_oldval_l = vim_strsave(oldval_l);
	ಆದರೆ (oldval_g != ಖಾಲಿ)
	    saved_oldval_g = vim_strsave(oldval_g);
	saved_oldval = vim_strsave(oldval);
	saved_newval = vim_strsave(s);
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ ((errmsg = did_ಗಣ_string_option(opt_idx, varp, oldval, ಮೌಲ್ಯ, errbuf,
		    errbuflen, opt_flags, OP_NONE, &ಮೌಲ್ಯ_checked)) == ಖಾಲಿ)
	did_ಗಣ_option(opt_idx, opt_flags, TRUE, ಮೌಲ್ಯ_checked);

#ಆದರೆ defined(FEAT_EVAL)
    // call autocommand after handling side effects
    ಆದರೆ (errmsg == ಖಾಲಿ)
	trigger_optionset_string(opt_idx, opt_flags,
		saved_oldval, saved_oldval_l,
		saved_oldval_g, saved_newval);
    vim_free(saved_oldval);
    vim_free(saved_oldval_l);
    vim_free(saved_oldval_g);
    vim_free(saved_newval);
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ errmsg;
}

/*
 * Return TRUE if "val" is a valid 'filetype' name.
 * Also used for 'syntax' and 'keymap'.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
valid_filetype(char_u *val)
{
    ಹಿಂತಿರುಗಿ valid_name(val, ".-_");
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_STL_OPT
/*
 * Check validity of options with the 'statusline' format.
 * Return an untranslated error message or NULL.
 */
    ಸ್ಥಿರ ಅಕ್ಷರ *
check_stl_option(char_u *s)
{
    ಪೂರ್ಣಾಂಕ		groupdepth = 0;
    ಸ್ಥಿರ ಅಕ್ಷರ errbuf[ERR_BUFLEN];
    ಪೂರ್ಣಾಂಕ		errbuflen = ERR_BUFLEN;

    ಆಗ (*s)
    {
	// Check for valid keys after % sequences
	ಆಗ (*s && *s != '%')
	    s++;
	ಆದರೆ (!*s)
	    ನಿಲ್ಲಿಸು;
	s++;
	ಆದರೆ (*s == STL_CLICKFUNC)
	{
	    ಆದರೆ (s[1] == ']')
	    {
		// %[] - end click region
		s += 2;
		ಮುಂದುವರಿ;
	    }
	    ಆದರೆ (ASCII_ISALPHA(s[1]) || s[1] == '_')
	    {
		// %[FuncName] - start click region
		char_u *rb = vim_strchr(s + 2, ']');
		ಆದರೆ (rb != ಖಾಲಿ)
		{
		    s = rb + 1;
		    ಮುಂದುವರಿ;
		}
	    }
	    // Bare %[ is invalid
	    ಹಿಂತಿರುಗಿ illegal_char(errbuf, errbuflen, *s);
	}
	ಆದರೆ (*s == STL_LINEBREAK)
	{
	    // Plain %@ - line break
	    s++;
	    ಮುಂದುವರಿ;
	}
	ಆದರೆ (*s == '%' || *s == STL_TRUNCMARK || *s == STL_SEPARATE)
	{
	    s++;
	    ಮುಂದುವರಿ;
	}
	ಆದರೆ (*s == ')')
	{
	    s++;
	    ಆದರೆ (--groupdepth < 0)
		ನಿಲ್ಲಿಸು;
	    ಮುಂದುವರಿ;
	}
	ಆದರೆ (*s == '-')
	    s++;
	ಆಗ (VIM_ISDIGIT(*s))
	    s++;
	ಆದರೆ (*s == STL_USER_HL)
	    ಮುಂದುವರಿ;
	ಆದರೆ (*s == STL_CLICKFUNC)
	{
	    // %N[FuncName] or %N[]
	    ಆದರೆ (s[1] == ']')
	    {
		s += 2;
		ಮುಂದುವರಿ;
	    }
	    ಆದರೆ (ASCII_ISALPHA(s[1]) || s[1] == '_')
	    {
		char_u *rb = vim_strchr(s + 2, ']');
		ಆದರೆ (rb != ಖಾಲಿ)
		{
		    s = rb + 1;
		    ಮುಂದುವರಿ;
		}
	    }
	    // Bare %N[ is invalid
	    ಹಿಂತಿರುಗಿ illegal_char(errbuf, errbuflen, *s);
	}
	ಆದರೆ (*s == STL_LINEBREAK)
	{
	    // %N@ - line break
	    s++;
	    ಮುಂದುವರಿ;
	}
	ಆದರೆ (*s == '.')
	{
	    s++;
	    ಆಗ (*s && VIM_ISDIGIT(*s))
		s++;
	}
	ಆದರೆ (*s == '(')
	{
	    groupdepth++;
	    ಮುಂದುವರಿ;
	}
	ಆದರೆ (vim_strchr(STL_ALL, *s) == ಖಾಲಿ)
	{
	    ಹಿಂತಿರುಗಿ illegal_char(errbuf, errbuflen, *s);
	}
	ಆದರೆ (*s == '{')
	{
	    ಪೂರ್ಣಾಂಕ reevaluate = (*++s == '%');

	    ಆದರೆ (reevaluate && *++s == '}')
		// "}" is not allowed immediately after "%{%"
		ಹಿಂತಿರುಗಿ illegal_char(errbuf, errbuflen, '}');
	    ಆಗ ((*s != '}' || (reevaluate && s[-1] != '%')) && *s)
		s++;
	    ಆದರೆ (*s != '}')
		ಹಿಂತಿರುಗಿ e_unclosed_ಅಭಿವ್ಯಕ್ತಿ_sequence;
	}
    }
    ಆದರೆ (groupdepth != 0)
	ಹಿಂತಿರುಗಿ e_unbalanced_groups;
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Check for a "normal" directory or file name in some options.  Disallow a
 * path separator (slash and/or backslash), wildcards and characters that are
 * often illegal in a file name. Be more permissive if "secure" is off.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
check_illegal_ಮಾರ್ಗ_ಹೆಸರುಗಳು(ಪೂರ್ಣಾಂಕ opt_idx, char_u **varp)
{
    ಹಿಂತಿರುಗಿ (((get_option_flags(opt_idx) & P_NFNAME)
		&& vim_strpbrk(*varp, (char_u *)(secure
			? "/\\*?[|;&<>\r\n" : "/\\*?[<>\r\n")) != ಖಾಲಿ)
	    || ((get_option_flags(opt_idx) & P_NDNAME)
		&& vim_strpbrk(*varp, (char_u *)"*?[|;&<>\r\n") != ಖಾಲಿ));
}

/*
 * An option that accepts a list of flags is changed.
 * e.g. 'viewoptions', 'switchbuf', 'casemap', etc.
 */
    ಸ್ಥಿರ ಅಕ್ಷರ *
did_ಗಣ_opt_flags(char_u *val, ಅಕ್ಷರ **ಮೌಲ್ಯಗಳು, ಚಿಹ್ನೆರಹಿತ *flagp, ಪೂರ್ಣಾಂಕ list)
{
    ಆದರೆ (opt_ಅಕ್ಷರಸರಣಿಗಳು_flags(val, ಮೌಲ್ಯಗಳು, flagp, list) == FAIL)
	ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * An option that accepts a list of string values is changed.
 * e.g. 'nrformats', 'scrollopt', 'wildoptions', etc.
 */
    ಸ್ಥಿರ ಅಕ್ಷರ *
did_ಗಣ_opt_ಅಕ್ಷರಸರಣಿಗಳು(char_u *val, ಅಕ್ಷರ **ಮೌಲ್ಯಗಳು, ಪೂರ್ಣಾಂಕ list)
{
    ಹಿಂತಿರುಗಿ did_ಗಣ_opt_flags(val, ಮೌಲ್ಯಗಳು, ಖಾಲಿ, list);
}

/*
 * An option which is a list of flags is set.  Valid values are in 'flags'.
 */
    ಸ್ಥಿರ ಅಕ್ಷರ *
did_ಗಣ_option_listflag(
	char_u *val,
	char_u *flags,
	ಅಕ್ಷರ *errbuf,
	size_t errbuflen)
{
    char_u	*s;

    ಗಾಗಿ (s = val; *s; ++s)
	ಆದರೆ (vim_strchr(flags, *s) == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ illegal_char(errbuf, errbuflen, *s);

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Expand an option that accepts a list of fixed string values with known
 * number of items.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
expand_ಗಣ_opt_string(
	optexpand_T *ವಾದಗಳು,
	ಅಕ್ಷರ **ಮೌಲ್ಯಗಳು,
	size_t ಅಂಕೆ_ಮೌಲ್ಯಗಳು,
	ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches,
	char_u ***matches)
{
    char_u	*p;
    regmatch_T	*regmatch = ವಾದಗಳು->oe_regmatch;
    ಪೂರ್ಣಾಂಕ		include_orig_val = ವಾದಗಳು->oe_include_orig_val;
    char_u	*option_val = ವಾದಗಳು->oe_opt_ಮೌಲ್ಯ;

    // Assume numValues is small since they are fixed enums, so just allocate
    // upfront instead of needing two passes to calculate output size.
    *matches = ALLOC_MULT(char_u *, ಅಂಕೆ_ಮೌಲ್ಯಗಳು + 1);
    ಆದರೆ (*matches == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2 = 0;

    ಆದರೆ (include_orig_val && *option_val != NUL)
    {
	p = vim_strsave(option_val);
	ಆದರೆ (p == ಖಾಲಿ)
	{
	    VIM_CLEAR(*matches);
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	(*matches)[ಎಣಿಕೆ_2++] = p;
    }

    ಗಾಗಿ (ಅಕ್ಷರ **val = ಮೌಲ್ಯಗಳು; *val != ಖಾಲಿ; val++)
    {
	ಆದರೆ (include_orig_val && *option_val != NUL)
	{
	    ಆದರೆ (STRCMP((char_u*)*val, option_val) == 0)
		ಮುಂದುವರಿ;
	}
	ಆದರೆ (vim_regexec(regmatch, (char_u*)(*val), (colnr_T)0))
	{
	    p = vim_strsave((char_u*)*val);
	    ಆದರೆ (p == ಖಾಲಿ)
	    {
		ಆದರೆ (ಎಣಿಕೆ_2 == 0)
		{
		    VIM_CLEAR(*matches);
		    ಹಿಂತಿರುಗಿ FAIL;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    ನಿಲ್ಲಿಸು;
	    }
	    (*matches)[ಎಣಿಕೆ_2++] = p;
	}
    }
    ಆದರೆ (ಎಣಿಕೆ_2 == 0)
    {
	VIM_CLEAR(*matches);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    *ಅಂಕೆ_Matches = ಎಣಿಕೆ_2;
    ಹಿಂತಿರುಗಿ OK;
}

ಸ್ಥಿರ char_u *ಗಣ_opt_callback_orig_option = ಖಾಲಿ;
ಸ್ಥಿರ char_u *((*ಗಣ_opt_callback_func)(expand_T *, ಪೂರ್ಣಾಂಕ));

/*
 * Callback used by expand_set_opt_generic to also include the original value
 * as the first item.
 */
    ಸ್ಥಿರ char_u *
expand_ಗಣ_opt_generic_cb(expand_T *xp, ಪೂರ್ಣಾಂಕ idx)
{
    ಆದರೆ (idx == 0)
    {
	ಆದರೆ (ಗಣ_opt_callback_orig_option != ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ ಗಣ_opt_callback_orig_option;
	ಇಲ್ಲದಿದ್ದರೆ
	    ಹಿಂತಿರುಗಿ (char_u *)""; // empty strings are ignored
    }
    ಹಿಂತಿರುಗಿ ಗಣ_opt_callback_func(xp, idx - 1);
}

/*
 * Expand an option with a callback that iterates through a list of possible
 * names using an index.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
expand_ಗಣ_opt_generic(
	optexpand_T *ವಾದಗಳು,
	char_u *((*func)(expand_T *, ಪೂರ್ಣಾಂಕ)),
	ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches,
	char_u ***matches)
{
    ಪೂರ್ಣಾಂಕ ret;

    ಗಣ_opt_callback_orig_option = ವಾದಗಳು->oe_include_orig_val ?
	ವಾದಗಳು->oe_opt_ಮೌಲ್ಯ : ಖಾಲಿ;
    ಗಣ_opt_callback_func = func;

    ret = ExpandGeneric(
	    (char_u*)"", // not using fuzzy as currently EXPAND_STRING_SETTING doesn't use it
	    ವಾದಗಳು->oe_xp,
	    ವಾದಗಳು->oe_regmatch,
	    matches,
	    ಅಂಕೆ_Matches,
	    expand_ಗಣ_opt_generic_cb,
	    FALSE);

    ಗಣ_opt_callback_orig_option = ಖಾಲಿ;
    ಗಣ_opt_callback_func = ಖಾಲಿ;
    ಹಿಂತಿರುಗಿ ret;
}

#ಆದರೆ defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
ಸ್ಥಿರ garray_T *expand_cb_ga;
ಸ್ಥಿರ optexpand_T *expand_cb_ವಾದಗಳು;

/*
 * Callback provided to a function in expand_set_opt_callback. Will perform
 * regex matching against the value and add to the list.
 *
 * Returns OK usually. Returns FAIL if it failed to allocate memory, and the
 * caller should terminate the enumeration.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
expand_ಗಣ_opt_callback_cb(char_u *val)
{
    regmatch_T	*regmatch = expand_cb_ವಾದಗಳು->oe_regmatch;
    expand_T	*xp = expand_cb_ವಾದಗಳು->oe_xp;
    garray_T	*ga = expand_cb_ga;
    char_u	*str;

    ಆದರೆ (val == ಖಾಲಿ || *val == NUL)
	ಹಿಂತಿರುಗಿ OK;

    ಆದರೆ (xp->xp_ನಮೂನೆ[0] != NUL &&
	    !vim_regexec(regmatch, val, (colnr_T)0))
	ಹಿಂತಿರುಗಿ OK;

    str = vim_strsave_escaped(val, (char_u *)" \t\\");

    ಆದರೆ (str == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    ಆದರೆ (ga_grow(ga, 1) == FAIL)
    {
	vim_free(str);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ((char_u **)ga->ga_ದತ್ತಾಂಶ)[ga->ga_len] = str;
    ++ga->ga_len;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Expand an option with a provided function that takes a callback. The
 * function will enumerate through all options and call the callback to add it
 * to the list.
 *
 * "func" is the enumerator function that will generate the list of options.
 * "func_params" is a single parameter that will be passed to func.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
expand_ಗಣ_opt_callback(
	optexpand_T *ವಾದಗಳು,
	ಶೂನ್ಯ (*func)(optexpand_T *, ಶೂನ್ಯ* params, ಪೂರ್ಣಾಂಕ (*cb)(char_u *val)),
	ಶೂನ್ಯ *func_params,
	ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches,
	char_u ***matches)
{
    garray_T	ga;
    ಪೂರ್ಣಾಂಕ		include_orig_val = ವಾದಗಳು->oe_include_orig_val;
    char_u	*option_val = ವಾದಗಳು->oe_opt_ಮೌಲ್ಯ;

    ga_init2(&ga, ಗಾತ್ರ(ಅಕ್ಷರ *), 30);

    ಆದರೆ (include_orig_val && *option_val != NUL)
    {
	char_u *p = vim_strsave(option_val);
	ಆದರೆ (p == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FAIL;
	ಆದರೆ (ga_grow(&ga, 1) == FAIL)
	{
	    vim_free(p);
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	((char_u **)ga.ga_ದತ್ತಾಂಶ)[ga.ga_len] = p;
	++ga.ga_len;
    }

    expand_cb_ga = &ga;
    expand_cb_ವಾದಗಳು = ವಾದಗಳು;

    func(ವಾದಗಳು, func_params, expand_ಗಣ_opt_callback_cb);

    expand_cb_ga = ಖಾಲಿ;
    expand_cb_ವಾದಗಳು = ಖಾಲಿ;

    *matches = ga.ga_ದತ್ತಾಂಶ;
    *ಅಂಕೆ_Matches = ga.ga_len;
    ಹಿಂತಿರುಗಿ OK;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Expand an option which is a list of flags.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
expand_ಗಣ_opt_listflag(
	optexpand_T *ವಾದಗಳು,
	char_u *flags,
	ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches,
	char_u ***matches)
{
    char_u  *p;
    char_u  *option_val = ವಾದಗಳು->oe_opt_ಮೌಲ್ಯ;
    char_u  *cmdline_val = ವಾದಗಳು->oe_ಗಣ_arg;
    ಪೂರ್ಣಾಂಕ	    append = ವಾದಗಳು->oe_append;
    ಪೂರ್ಣಾಂಕ	    include_orig_val = ವಾದಗಳು->oe_include_orig_val && (*option_val != NUL);

    size_t ಅಂಕೆ_flags = STRLEN(flags);

    // Assume we only have small number of flags, so just allocate max size.
    *matches = ALLOC_MULT(char_u *, ಅಂಕೆ_flags + 1);
    ಆದರೆ (*matches == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2 = 0;

    ಆದರೆ (include_orig_val)
    {
	p = vim_strsave(option_val);
	ಆದರೆ (p == ಖಾಲಿ)
	{
	    VIM_CLEAR(*matches);
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	(*matches)[ಎಣಿಕೆ_2++] = p;
    }

    ಗಾಗಿ (char_u *flag = flags; *flag != NUL; flag++)
    {
	ಆದರೆ (append && vim_strchr(option_val, *flag) != ಖಾಲಿ)
	    ಮುಂದುವರಿ;

	ಆದರೆ (vim_strchr(cmdline_val, *flag) == ಖಾಲಿ)
	{
	    ಆದರೆ (include_orig_val
		    && option_val[1] == NUL
		    && *flag == option_val[0])
	    {
		// This value is already used as the first choice as it's the
		// existing flag. Just skip it to avoid duplicate.
		ಮುಂದುವರಿ;
	    }
	    p = vim_strnsave(flag, 1);
	    ಆದರೆ (p == ಖಾಲಿ)
	    {
		ಆದರೆ (ಎಣಿಕೆ_2 == 0)
		{
		    VIM_CLEAR(*matches);
		    ಹಿಂತಿರುಗಿ FAIL;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    ನಿಲ್ಲಿಸು;
	    }
	    (*matches)[ಎಣಿಕೆ_2++] = p;
	}
    }

    ಆದರೆ (ಎಣಿಕೆ_2 == 0)
    {
	VIM_CLEAR(*matches);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    *ಅಂಕೆ_Matches = ಎಣಿಕೆ_2;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * The 'ambiwidth' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_ambiwidth(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಆದರೆ (check_opt_ಅಕ್ಷರಸರಣಿಗಳು(p_ambw, p_ambw_ಮೌಲ್ಯಗಳು, FALSE) != OK)
	ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;

    ಹಿಂತಿರುಗಿ check_chars_options();
}

// "name" must be a string literal, the length is computed at compile time.
#ವ್ಯಾಖ್ಯಾನಿಸು completing_ಮೌಲ್ಯ_for_subopt(ವಾದಗಳು, name) \
	  completing_ಮೌಲ್ಯ_for_subopt_len(ವಾದಗಳು, name, (ಪೂರ್ಣಾಂಕ)STRLEN_LITERAL(name))

/*
 * Return true when completing the value of the sub-option "name" with length
 * "len", e.g. the value after "close:" in 'completepopup'.
 */
    ಸ್ಥಿರ bool
completing_ಮೌಲ್ಯ_for_subopt_len(optexpand_T *ವಾದಗಳು, ಅಕ್ಷರ *name, ಪೂರ್ಣಾಂಕ len)
{
    char_u  *colon = ವಾದಗಳು->oe_xp->xp_ನಮೂನೆ - 1;
    ಪೂರ್ಣಾಂಕ	    off = (ಪೂರ್ಣಾಂಕ)(colon - ವಾದಗಳು->oe_ಗಣ_arg);

    ಆದರೆ (off < len)
	ಹಿಂತಿರುಗಿ false;
    // The name must follow a comma when it does not start the option value.
    ಆದರೆ (off > len && *(colon - len - 1) != ',')
	ಹಿಂತಿರುಗಿ false;

    ಹಿಂತಿರುಗಿ STRNCMP(colon - len, name, len) == 0;
}

    ಪೂರ್ಣಾಂಕ
expand_ಗಣ_ambiwidth(optexpand_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಹಿಂತಿರುಗಿ expand_ಗಣ_opt_string(
	    ವಾದಗಳು,
	    p_ambw_ಮೌಲ್ಯಗಳು,
	    ARRAY_ಉದ್ದಳತೆ(p_ambw_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}

/*
 * The 'background' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_background(optset_T *ವಾದಗಳು)
{
    ಆದರೆ (check_opt_ಅಕ್ಷರಸರಣಿಗಳು(p_bg, p_bg_ಮೌಲ್ಯಗಳು, FALSE) == FAIL)
	ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;

    ಆದರೆ (ವಾದಗಳು->os_oldval.string != ಖಾಲಿ && ವಾದಗಳು->os_oldval.string[0] == *p_bg)
	// Value was not changed
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಪೂರ್ಣಾಂಕ dark = (*p_bg == 'd');
#ಆದರೆ_ಅಂತ್ಯ

    init_highlight(FALSE, FALSE);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಆದರೆ (dark != (*p_bg == 'd')
	    && get_var_ಮೌಲ್ಯ((char_u *)"g:colors_name") != ಖಾಲಿ)
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
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
    ಪದ_update_colors_all();
#ಆದರೆ_ಅಂತ್ಯ

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

    ಪೂರ್ಣಾಂಕ
expand_ಗಣ_background(optexpand_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಹಿಂತಿರುಗಿ expand_ಗಣ_opt_string(
	    ವಾದಗಳು,
	    p_bg_ಮೌಲ್ಯಗಳು,
	    ARRAY_ಉದ್ದಳತೆ(p_bg_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}

/*
 * The 'backspace' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_backspace(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಆದರೆ (VIM_ISDIGIT(*p_bs))
    {
	ಆದರೆ (*p_bs > '3' || p_bs[1] != NUL)
	    ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (check_opt_ಅಕ್ಷರಸರಣಿಗಳು(p_bs, p_bs_ಮೌಲ್ಯಗಳು, TRUE) != OK)
	ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

    ಪೂರ್ಣಾಂಕ
expand_ಗಣ_backspace(optexpand_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಹಿಂತಿರುಗಿ expand_ಗಣ_opt_string(
	    ವಾದಗಳು,
	    p_bs_ಮೌಲ್ಯಗಳು,
	    ARRAY_ಉದ್ದಳತೆ(p_bs_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}

/*
 * The 'backupcopy' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_backupcopy(optset_T *ವಾದಗಳು)
{
    char_u		*bkc = p_bkc;
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ	*flags = &bkc_flags;
    ಅಕ್ಷರ		*errmsg = ಖಾಲಿ;

    ಆದರೆ (ವಾದಗಳು->os_flags & OPT_LOCAL)
    {
	bkc = curbuf->b_p_bkc;
	flags = &curbuf->b_bkc_flags;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!(ವಾದಗಳು->os_flags & OPT_GLOBAL))
	// When using :set, clear the local flags.
	curbuf->b_bkc_flags = 0;

    ಆದರೆ ((ವಾದಗಳು->os_flags & OPT_LOCAL) && *bkc == NUL)
	// make the local value empty: use the global value
	*flags = 0;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (opt_ಅಕ್ಷರಸರಣಿಗಳು_flags(bkc, p_bkc_ಮೌಲ್ಯಗಳು, flags, TRUE) != OK)
	    errmsg = e_invalid_ವಾದ;
	ಆದರೆ ((((ಪೂರ್ಣಾಂಕ)*flags & BKC_AUTO) != 0)
		+ (((ಪೂರ್ಣಾಂಕ)*flags & BKC_YES) != 0)
		+ (((ಪೂರ್ಣಾಂಕ)*flags & BKC_NO) != 0) != 1)
	{
	    // Must have exactly one of "auto", "yes"  and "no".
	    (ಶೂನ್ಯ)opt_ಅಕ್ಷರಸರಣಿಗಳು_flags(ವಾದಗಳು->os_oldval.string, p_bkc_ಮೌಲ್ಯಗಳು,
								  flags, TRUE);
	    errmsg = e_invalid_ವಾದ;
	}
    }

    ಹಿಂತಿರುಗಿ errmsg;
}

    ಪೂರ್ಣಾಂಕ
expand_ಗಣ_backupcopy(optexpand_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಹಿಂತಿರುಗಿ expand_ಗಣ_opt_string(
	    ವಾದಗಳು,
	    p_bkc_ಮೌಲ್ಯಗಳು,
	    ARRAY_ಉದ್ದಳತೆ(p_bkc_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}

/*
 * The 'backupext' or the 'patchmode' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_backupext_or_patchmode(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಆದರೆ (STRCMP(*p_bex == '.' ? p_bex + 1 : p_bex,
		*p_pm == '.' ? p_pm + 1 : p_pm) == 0)
	ಹಿಂತಿರುಗಿ e_backupext_and_patchmode_are_equal;

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * The 'belloff' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_belloff(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ did_ಗಣ_opt_flags(p_bo, p_bo_ಮೌಲ್ಯಗಳು, &bo_flags, TRUE);
}

    ಪೂರ್ಣಾಂಕ
expand_ಗಣ_belloff(optexpand_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಹಿಂತಿರುಗಿ expand_ಗಣ_opt_string(
	    ವಾದಗಳು,
	    p_bo_ಮೌಲ್ಯಗಳು,
	    ARRAY_ಉದ್ದಳತೆ(p_bo_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}

#ಆದರೆ defined(FEAT_LINEBREAK)
/*
 * The 'breakat' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_breakat(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		i;

    ಗಾಗಿ (i = 0; i < 256; i++)
	breakat_flags[i] = FALSE;

    ಆದರೆ (p_breakat != ಖಾಲಿ)
	ಗಾಗಿ (p = p_breakat; *p; p++)
	    breakat_flags[*p] = TRUE;

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * The 'breakindentopt' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_breakindentopt(optset_T *ವಾದಗಳು)
{
    char_u	**varp = (char_u **)ವಾದಗಳು->os_varp;

    ಆದರೆ (briopt_check(*varp, varp == &curwin->w_p_briopt ? curwin : ಖಾಲಿ)
								      == FAIL)
	ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;

    // list setting requires a redraw
    ಆದರೆ (varp == &curwin->w_p_briopt && curwin->w_briopt_list)
	redraw_all_later(UPD_NOT_VALID);

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

    ಪೂರ್ಣಾಂಕ
expand_ಗಣ_breakindentopt(
    optexpand_T		*ವಾದಗಳು,
    ಪೂರ್ಣಾಂಕ			*ಅಂಕೆ_Matches,
    char_u		***matches)
{
    ಹಿಂತಿರುಗಿ expand_ಗಣ_opt_string(
	    ವಾದಗಳು,
	    p_briopt_ಮೌಲ್ಯಗಳು,
	    ARRAY_ಉದ್ದಳತೆ(p_briopt_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_BROWSE)
/*
 * The 'browsedir' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_browsedir(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಆದರೆ (check_opt_ಅಕ್ಷರಸರಣಿಗಳು(p_bsdir, p_bsdir_ಮೌಲ್ಯಗಳು, FALSE) != OK
	    && !mch_isdir(p_bsdir))
	ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

    ಪೂರ್ಣಾಂಕ
expand_ಗಣ_browsedir(optexpand_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಹಿಂತಿರುಗಿ expand_ಗಣ_opt_string(
	    ವಾದಗಳು,
	    p_bsdir_ಮೌಲ್ಯಗಳು,
	    ARRAY_ಉದ್ದಳತೆ(p_bsdir_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * The 'bufhidden' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_bufhidden(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ did_ಗಣ_opt_ಅಕ್ಷರಸರಣಿಗಳು(curbuf->b_p_bh, p_bufhidden_ಮೌಲ್ಯಗಳು, FALSE);
}

    ಪೂರ್ಣಾಂಕ
expand_ಗಣ_bufhidden(optexpand_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಹಿಂತಿರುಗಿ expand_ಗಣ_opt_string(
	    ವಾದಗಳು,
	    p_bufhidden_ಮೌಲ್ಯಗಳು,
	    ARRAY_ಉದ್ದಳತೆ(p_bufhidden_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}

/*
 * The 'buftype' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_buftype(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಆದರೆ (check_opt_ಅಕ್ಷರಸರಣಿಗಳು(curbuf->b_p_bt, p_buftype_ಮೌಲ್ಯಗಳು, FALSE) != OK)
	ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;

    ಆದರೆ (curwin->w_ಸ್ಥಿತಿ_ಎತ್ತರ)
    {
	curwin->w_redr_ಸ್ಥಿತಿ = true;
	redraw_later(UPD_VALID);
    }
    curbuf->b_help = (curbuf->b_p_bt[0] == 'h');
    redraw_titles();

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

    ಪೂರ್ಣಾಂಕ
expand_ಗಣ_buftype(optexpand_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಹಿಂತಿರುಗಿ expand_ಗಣ_opt_string(
	    ವಾದಗಳು,
	    p_buftype_ಮೌಲ್ಯಗಳು,
	    ARRAY_ಉದ್ದಳತೆ(p_buftype_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}

/*
 * The 'casemap' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_casemap(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ did_ಗಣ_opt_flags(p_cmp, p_cmp_ಮೌಲ್ಯಗಳು, &cmp_flags, TRUE);
}

    ಪೂರ್ಣಾಂಕ
expand_ಗಣ_casemap(optexpand_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಹಿಂತಿರುಗಿ expand_ಗಣ_opt_string(
	    ವಾದಗಳು,
	    p_cmp_ಮೌಲ್ಯಗಳು,
	    ARRAY_ಉದ್ದಳತೆ(p_cmp_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}

#ಆದರೆ defined(HAVE_CLIPMETHOD)
    ಪೂರ್ಣಾಂಕ
expand_ಗಣ_clipboard(optexpand_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಹಿಂತಿರುಗಿ expand_ಗಣ_opt_string(
	    ವಾದಗಳು,
	    p_cb_ಮೌಲ್ಯಗಳು,
	    ARRAY_ಉದ್ದಳತೆ(p_cb_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_CLIPMETHOD
    ಅಕ್ಷರ *
did_ಗಣ_clipmethod(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ ಆರಿಸು_clipmethod();
}

    ಪೂರ್ಣಾಂಕ
expand_ಗಣ_clipmethod(optexpand_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    // We want to expand using the predefined clipmethod values + clipboard
    // provider names.
    ಪೂರ್ಣಾಂಕ		ಫಲಿತಾಂಶ;
    ಅಕ್ಷರ	**ಮೌಲ್ಯಗಳು;
    ಪೂರ್ಣಾಂಕ		ಎಣಿಕೆ_2, pos = 0, ಆರಂಭ = 0;
# ifdef FEAT_EVAL
    dict_T	*providers = get_vim_var_dict(VV_CLIPPROVIDERS);
# ಇಲ್ಲದಿದ್ದರೆ
    dict_T	*providers = ಖಾಲಿ;
# endif
    hashtab_T	*ht = providers == ಖಾಲಿ ? ಖಾಲಿ : &providers->dv_hashtab;

    ಎಣಿಕೆ_2 = (ht == ಖಾಲಿ ? 0 : ht->ht_ಬಳಸಿದ);
# ifdef FEAT_WAYLAND_CLIPBOARD
    ಎಣಿಕೆ_2++;
    ಆರಂಭ++;
# endif
# ifdef FEAT_XCLIPBOARD
    ಎಣಿಕೆ_2++;
    ಆರಂಭ++;
# endif
    ಮೌಲ್ಯಗಳು = ALLOC_MULT(ಅಕ್ಷರ *, ಎಣಿಕೆ_2 + 1); // Add NULL terminator too

    ಆದರೆ (ಮೌಲ್ಯಗಳು == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

# ifdef FEAT_WAYLAND_CLIPBOARD
    ಮೌಲ್ಯಗಳು[pos++] = "wayland";
# endif
# ifdef FEAT_XCLIPBOARD
    ಮೌಲ್ಯಗಳು[pos++] = "x11";
# endif

    ಆದರೆ (ht != ಖಾಲಿ)
	ಗಾಗಿ (long_u i = 0; i < ht->ht_ಮುಖವಾಡ + 1; i++)
	{
	    hashitem_T	*hi = ht->ht_array + i;

	    ಆದರೆ (!HASHITEM_ಖಾಲಿಗಳು(hi))
		ಮೌಲ್ಯಗಳು[pos++] = (ಅಕ್ಷರ *)vim_strsave(hi->hi_ಕೀಲಿ);
	}
    ಮೌಲ್ಯಗಳು[pos++] = ಖಾಲಿ;

    ಫಲಿತಾಂಶ = expand_ಗಣ_opt_string(
	    ವಾದಗಳು,
	    ಮೌಲ್ಯಗಳು,
	    ಎಣಿಕೆ_2,
	    ಅಂಕೆ_Matches,
	    matches);

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = ಆರಂಭ; i < ಎಣಿಕೆ_2; i++)
	vim_free(ಮೌಲ್ಯಗಳು[i]);
    vim_free(ಮೌಲ್ಯಗಳು);

    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * The global 'listchars' or 'fillchars' option is changed.
 */
    ಸ್ಥಿರ ಅಕ್ಷರ *
did_ಗಣ_global_listfillchars(char_u *val, ಪೂರ್ಣಾಂಕ opt_lcs, ಪೂರ್ಣಾಂಕ opt_flags,
						ಅಕ್ಷರ *errbuf, size_t errbuflen)
{
    ಅಕ್ಷರ	*errmsg = ಖಾಲಿ;
    char_u	**local_ptr = opt_lcs ? &curwin->w_p_lcs : &curwin->w_p_fcs;

    // only apply the global value to "curwin" when it does not have a
    // local value
    ಆದರೆ (opt_lcs)
	errmsg = ಗಣ_listchars_option(curwin, val,
		**local_ptr == NUL || !(opt_flags & OPT_GLOBAL),
							    errbuf, errbuflen);
    ಇಲ್ಲದಿದ್ದರೆ
	errmsg = ಗಣ_fillchars_option(curwin, val,
		**local_ptr == NUL || !(opt_flags & OPT_GLOBAL),
							    errbuf, errbuflen);
    ಆದರೆ (errmsg != ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ errmsg;

    tabpage_T	*tp;
    win_T	*wp;

    // If the current window is set to use the global
    // 'listchars'/'fillchars' value, clear the window-local value.
    ಆದರೆ (!(opt_flags & OPT_GLOBAL))
	clear_string_option(local_ptr);
    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	// If the current window has a local value need to apply it
	// again, it was changed when setting the global value.
	// If no error was returned above, we don't expect an error
	// here, so ignore the return value.
	ಆದರೆ (opt_lcs)
	{
	    ಆದರೆ (*wp->w_p_lcs == NUL)
		(ಶೂನ್ಯ)ಗಣ_listchars_option(wp, wp->w_p_lcs, TRUE, ಖಾಲಿ, 0);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (*wp->w_p_fcs == NUL)
		(ಶೂನ್ಯ)ಗಣ_fillchars_option(wp, wp->w_p_fcs, TRUE, ಖಾಲಿ, 0);
	}
    }

    redraw_all_later(UPD_NOT_VALID);

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * The 'fillchars' option or the 'listchars' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_chars_option(optset_T *ವಾದಗಳು)
{
    char_u **varp = (char_u **)ವಾದಗಳು->os_varp;
    ಅಕ್ಷರ *errmsg = ಖಾಲಿ;

    ಆದರೆ (   varp == &p_lcs		// global 'listchars'
	|| varp == &p_fcs)		// global 'fillchars'
	errmsg = did_ಗಣ_global_listfillchars(*varp, varp == &p_lcs,
			  ವಾದಗಳು->os_flags, ವಾದಗಳು->os_errbuf, ವಾದಗಳು->os_errbuflen);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (varp == &curwin->w_p_lcs)	// local 'listchars'
	errmsg = ಗಣ_listchars_option(curwin, *varp, TRUE,
					  ವಾದಗಳು->os_errbuf, ವಾದಗಳು->os_errbuflen);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (varp == &curwin->w_p_fcs)	// local 'fillchars'
	errmsg = ಗಣ_fillchars_option(curwin, *varp, TRUE,
					  ವಾದಗಳು->os_errbuf, ವಾದಗಳು->os_errbuflen);

    ಹಿಂತಿರುಗಿ errmsg;
}

/*
 * Expand 'fillchars' or 'listchars' option value.
 */
    ಪೂರ್ಣಾಂಕ
expand_ಗಣ_chars_option(optexpand_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    char_u **varp = (char_u **)ವಾದಗಳು->oe_varp;
    ಪೂರ್ಣಾಂಕ is_lcs = (varp == &p_lcs || varp == &curwin->w_p_lcs);
    ಹಿಂತಿರುಗಿ expand_ಗಣ_opt_generic(
	    ವಾದಗಳು,
	    is_lcs ? get_listchars_name : get_fillchars_name,
	    ಅಂಕೆ_Matches,
	    matches);
}

/*
 * The 'cinoptions' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_cinoptions(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    // TODO: recognize errors
    parse_cino(curbuf);

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

#ಆದರೆ defined(FEAT_SYN_HL)
/*
 * The 'colorcolumn' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_colorcolumn(optset_T *ವಾದಗಳು)
{
    char_u	**varp = (char_u **)ವಾದಗಳು->os_varp;

    ಹಿಂತಿರುಗಿ check_colorcolumn(*varp, varp == &curwin->w_p_cc ? curwin : ಖಾಲಿ);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * The 'comments' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_comments(optset_T *ವಾದಗಳು)
{
    char_u	**varp = (char_u **)ವಾದಗಳು->os_varp;
    char_u	*s;
    ಅಕ್ಷರ	*errmsg = ಖಾಲಿ;

    ಗಾಗಿ (s = *varp; *s; )
    {
	ಆಗ (*s && *s != ':')
	{
	    ಆದರೆ (vim_strchr((char_u *)COM_ALL, *s) == ಖಾಲಿ
		    && !VIM_ISDIGIT(*s) && *s != '-')
	    {
		errmsg = illegal_char(ವಾದಗಳು->os_errbuf, ವಾದಗಳು->os_errbuflen, *s);
		ನಿಲ್ಲಿಸು;
	    }
	    ++s;
	}
	ಆದರೆ (*s++ == NUL)
	    errmsg = e_missing_colon;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*s == ',' || *s == NUL)
	    errmsg = e_zero_ಉದ್ದಳತೆ_string;
	ಆದರೆ (errmsg != ಖಾಲಿ)
	    ನಿಲ್ಲಿಸು;
	ಆಗ (*s && *s != ',')
	{
	    ಆದರೆ (*s == '\\' && s[1] != NUL)
		++s;
	    ++s;
	}
	s = skip_ಗಮ್ಯ_option_part(s);
    }

    ಹಿಂತಿರುಗಿ errmsg;
}

/*
 * The 'commentstring' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_commentstring(optset_T *ವಾದಗಳು)
{
    char_u	**varp = (char_u **)ವಾದಗಳು->os_varp;

    ಆದರೆ (**varp != NUL && ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು((ಅಕ್ಷರ *)*varp, "%s") == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ e_commentstring_must_be_ಖಾಲಿಗಳು_or_contain_str;

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Check if value for 'complete' is valid when 'complete' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_complete(optset_T *ವಾದಗಳು)
{
    char_u	**varp = (char_u **)ವಾದಗಳು->os_varp;
    char_u	*p, *t;
    char_u	ಬಫರ್[LSIZE];
    char_u	*buf_ptr;
    char_u	char_before = NUL;
    ಪೂರ್ಣಾಂಕ		escape;

    ಗಾಗಿ (p = *varp; *p; )
    {
	vim_memset(ಬಫರ್, 0, LSIZE);
	buf_ptr = ಬಫರ್;
	escape = 0;

	// Extract substring while handling escaped commas
	ಆಗ (*p && (*p != ',' || escape) && buf_ptr < (ಬಫರ್ + LSIZE - 1))
	{
	    ಆದರೆ (*p == '\\' && *(p + 1) == ',')
	    {
		escape = 1;  // Mark escape mode
		p++;         // Skip '\'
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		escape = 0;
		*buf_ptr++ = *p;
	    }
	    p++;
	}
	*buf_ptr = NUL;

	ಆದರೆ (vim_strchr((char_u *)".wbuksid]tUFo", *ಬಫರ್) == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ illegal_char(ವಾದಗಳು->os_errbuf, ವಾದಗಳು->os_errbuflen, *ಬಫರ್);

	ಆದರೆ (vim_strchr((char_u *)"ksF", *ಬಫರ್) == ಖಾಲಿ && *(ಬಫರ್ + 1) != NUL
		&& *(ಬಫರ್ + 1) != '^')
	    char_before = *ಬಫರ್;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // Test for a number after '^'
	    ಆದರೆ ((t = vim_strchr(ಬಫರ್, '^')) != ಖಾಲಿ)
	    {
		*t++ = NUL;
		ಆದರೆ (!*t)
		    char_before = '^';
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ಗಾಗಿ (; *t; t++)
		    {
			ಆದರೆ (!vim_isdigit(*t))
			{
			    char_before = '^';
			    ನಿಲ್ಲಿಸು;
			}
		    }
		}
	    }
	}
	ಆದರೆ (char_before != NUL)
	    ಹಿಂತಿರುಗಿ illegal_char_after_chr(ವಾದಗಳು->os_errbuf, ವಾದಗಳು->os_errbuflen,
		    char_before);
	// Skip comma and spaces
	ಆಗ (*p == ',' || *p == ' ')
	    p++;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_COMPL_FUNC
    ಆದರೆ (ಗಣ_cpt_callbacks(ವಾದಗಳು) != OK)
	ಹಿಂತಿರುಗಿ illegal_char_after_chr(ವಾದಗಳು->os_errbuf, ವಾದಗಳು->os_errbuflen,
		'F');
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

    ಪೂರ್ಣಾಂಕ
expand_ಗಣ_complete(optexpand_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಸ್ಥಿರ ಅಕ್ಷರ *(p_cpt_ಮೌಲ್ಯಗಳು[]) = {
	".", "w", "b", "u", "k", "kspell", "s", "i", "d", "]", "t", "U", "F", "o",
	ಖಾಲಿ};
    ಹಿಂತಿರುಗಿ expand_ಗಣ_opt_string(
	    ವಾದಗಳು,
	    p_cpt_ಮೌಲ್ಯಗಳು,
	    ARRAY_ಉದ್ದಳತೆ(p_cpt_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}

/*
 * The 'completeopt' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_completeopt(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    char_u	*cot = p_cot;
    ಚಿಹ್ನೆರಹಿತ	*flags = &cot_flags;

    ಆದರೆ (ವಾದಗಳು->os_flags & OPT_LOCAL)
    {
	cot = curbuf->b_p_cot;
	flags = &curbuf->b_cot_flags;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!(ವಾದಗಳು->os_flags & OPT_GLOBAL))
	// When using :set, clear the local flags.
	curbuf->b_cot_flags = 0;

    ಆದರೆ (opt_ಅಕ್ಷರಸರಣಿಗಳು_flags(cot, p_cot_ಮೌಲ್ಯಗಳು, flags, TRUE) != OK)
	ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

    ಪೂರ್ಣಾಂಕ
expand_ಗಣ_completeopt(optexpand_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಹಿಂತಿರುಗಿ expand_ಗಣ_opt_string(
	    ವಾದಗಳು,
	    p_cot_ಮೌಲ್ಯಗಳು,
	    ARRAY_ಉದ್ದಳತೆ(p_cot_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}

/*
 * The 'completefuzzycollect' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_completefuzzycollect(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಆದರೆ (opt_ಅಕ್ಷರಸರಣಿಗಳು_flags(p_cfc, p_cfc_ಮೌಲ್ಯಗಳು, &cfc_flags, TRUE) != OK)
	ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

    ಪೂರ್ಣಾಂಕ
expand_ಗಣ_completefuzzycollect(
	optexpand_T *ವಾದಗಳು,
	ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches,
	char_u ***matches)
{
    ಹಿಂತಿರುಗಿ expand_ಗಣ_opt_string(
	    ವಾದಗಳು,
	    p_cfc_ಮೌಲ್ಯಗಳು,
	    ARRAY_ಉದ್ದಳತೆ(p_cfc_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}

/*
 * The 'completeitemalign' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_completeitemalign(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    char_u	*p = p_cia;
    ಚಿಹ್ನೆರಹಿತ	new_cia_flags = 0;
    ಪೂರ್ಣಾಂಕ		ನೋಡಿದ[3] = { FALSE, FALSE, FALSE };
    ಪೂರ್ಣಾಂಕ		ಎಣಿಕೆ_2 = 0;
    char_u	buf[10];

    ಆಗ (*p)
    {
	copy_option_part(&p, buf, ಗಾತ್ರ(buf), ",");
	ಆದರೆ (ಎಣಿಕೆ_2 >= 3)
	    ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;

	ಆದರೆ (STRCMP(buf, "abbr") == 0)
	{
	    ಆದರೆ (ನೋಡಿದ[CPT_ABBR])
		ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;
	    new_cia_flags = new_cia_flags * 10 + CPT_ABBR;
	    ನೋಡಿದ[CPT_ABBR] = TRUE;
	    ಎಣಿಕೆ_2++;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(buf, "kind") == 0)
	{
	    ಆದರೆ (ನೋಡಿದ[CPT_ಬಗೆ])
		ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;
	    new_cia_flags = new_cia_flags * 10 + CPT_ಬಗೆ;
	    ನೋಡಿದ[CPT_ಬಗೆ] = TRUE;
	    ಎಣಿಕೆ_2++;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(buf, "menu") == 0)
	{
	    ಆದರೆ (ನೋಡಿದ[CPT_MENU])
		ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;
	    new_cia_flags = new_cia_flags * 10 + CPT_MENU;
	    ನೋಡಿದ[CPT_MENU] = TRUE;
	    ಎಣಿಕೆ_2++;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;
    }
    ಆದರೆ (new_cia_flags == 0 || ಎಣಿಕೆ_2 != 3)
	ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;

    cia_flags = new_cia_flags;
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

#ಆದರೆ defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
/*
 * The 'completepopup' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_completepopup(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಆದರೆ (parse_completepopup(ಖಾಲಿ) == FAIL)
	ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;

    popup_close_info();
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(BACKSLASH_ಒಳ_FILENAME)
/*
 * The 'completeslash' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_completeslash(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಆದರೆ (check_opt_ಅಕ್ಷರಸರಣಿಗಳು(p_csl, p_csl_ಮೌಲ್ಯಗಳು, FALSE) != OK
	    || check_opt_ಅಕ್ಷರಸರಣಿಗಳು(curbuf->b_p_csl, p_csl_ಮೌಲ್ಯಗಳು, FALSE) != OK)
	ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

    ಪೂರ್ಣಾಂಕ
expand_ಗಣ_completeslash(optexpand_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಹಿಂತಿರುಗಿ expand_ಗಣ_opt_string(
	    ವಾದಗಳು,
	    p_csl_ಮೌಲ್ಯಗಳು,
	    ARRAY_ಉದ್ದಳತೆ(p_csl_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_CONCEAL)
/*
 * The 'concealcursor' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_concealcursor(optset_T *ವಾದಗಳು)
{
    char_u	**varp = (char_u **)ವಾದಗಳು->os_varp;

    ಹಿಂತಿರುಗಿ did_ಗಣ_option_listflag(*varp, (char_u *)COCU_ALL, ವಾದಗಳು->os_errbuf,
		    ವಾದಗಳು->os_errbuflen);
}

    ಪೂರ್ಣಾಂಕ
expand_ಗಣ_concealcursor(optexpand_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಹಿಂತಿರುಗಿ expand_ಗಣ_opt_listflag(ವಾದಗಳು, (char_u*)COCU_ALL, ಅಂಕೆ_Matches, matches);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * The 'cpoptions' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_cpoptions(optset_T *ವಾದಗಳು)
{
    char_u	**varp = (char_u **)ವಾದಗಳು->os_varp;

    ಹಿಂತಿರುಗಿ did_ಗಣ_option_listflag(*varp, (char_u *)CPO_ALL, ವಾದಗಳು->os_errbuf,
		    ವಾದಗಳು->os_errbuflen);
}

    ಪೂರ್ಣಾಂಕ
expand_ಗಣ_cpoptions(optexpand_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಹಿಂತಿರುಗಿ expand_ಗಣ_opt_listflag(ವಾದಗಳು, (char_u*)CPO_ALL, ಅಂಕೆ_Matches, matches);
}

#ಆದರೆ defined(FEAT_CRYPT)
/*
 * The 'cryptkey' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_cryptkey(optset_T *ವಾದಗಳು)
{
    // Make sure the ":set" command doesn't show the new value in the
    // history.
    remove_ಕೀಲಿ_from_history();

    ಆದರೆ (ವಾದಗಳು->os_op != OP_NONE)
	// Don't allow set+=/-=/^= as they can allow for substring guessing
	ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;

    ಆದರೆ (STRCMP(curbuf->b_p_ಕೀಲಿ, ವಾದಗಳು->os_oldval.string) != 0)
    {
	// Need to update the swapfile.
	ml_ಗಣ_crypt_ಕೀಲಿ(curbuf, ವಾದಗಳು->os_oldval.string,
		*curbuf->b_p_cm == NUL ? p_cm : curbuf->b_p_cm);
	ಬದಲಾಯಿತು_ಆಂತರಿಕ();
    }
# ifdef FEAT_SODIUM
    ಆದರೆ (crypt_ವಿಧಾನ_is_sodium(crypt_get_ವಿಧಾನ_nr(curbuf)))
	crypt_sodium_ಬೀಗ_ಕೀಲಿ(ವಾದಗಳು->os_newval.string);
# endif

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * The 'cryptmethod' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_cryptmethod(optset_T *ವಾದಗಳು)
{
    char_u  *p;
    char_u  *s;

    ಆದರೆ (ವಾದಗಳು->os_flags & OPT_LOCAL)
	p = curbuf->b_p_cm;
    ಇಲ್ಲದಿದ್ದರೆ
	p = p_cm;
    ಆದರೆ (check_opt_ಅಕ್ಷರಸರಣಿಗಳು(p, p_cm_ಮೌಲ್ಯಗಳು, TRUE) != OK)
	ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (crypt_self_test() == FAIL)
	ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;

    // When setting the global value to empty, make it "zip".
    ಆದರೆ (*p_cm == NUL)
    {
	free_string_option(p_cm);
	p_cm = vim_strsave((char_u *)"zip");
    }
    // When using ":set cm=name" the local value is going to be empty.
    // Do that here, otherwise the crypt functions will still use the
    // local value.
    ಆದರೆ ((ವಾದಗಳು->os_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	free_string_option(curbuf->b_p_cm);
	curbuf->b_p_cm = ಖಾಲಿಗಳು_option;
    }

    // Need to update the swapfile when the effective method changed.
    // Set "s" to the effective old value, "p" to the effective new
    // method and compare.
    ಆದರೆ ((ವಾದಗಳು->os_flags & OPT_LOCAL) && *ವಾದಗಳು->os_oldval.string == NUL)
	s = p_cm;  // was previously using the global value
    ಇಲ್ಲದಿದ್ದರೆ
	s = ವಾದಗಳು->os_oldval.string;
    ಆದರೆ (*curbuf->b_p_cm == NUL)
	p = p_cm;  // is now using the global value
    ಇಲ್ಲದಿದ್ದರೆ
	p = curbuf->b_p_cm;
    ಆದರೆ (STRCMP(s, p) != 0)
	ml_ಗಣ_crypt_ಕೀಲಿ(curbuf, curbuf->b_p_ಕೀಲಿ, s);

    // If the global value changes need to update the swapfile for all
    // buffers using that value.
    ಆದರೆ ((ವಾದಗಳು->os_flags & OPT_GLOBAL)
	    && STRCMP(p_cm, ವಾದಗಳು->os_oldval.string) != 0)
    {
	buf_T	*buf;

	FOR_ALL_BUFFERS(buf)
	    ಆದರೆ (buf != curbuf && *buf->b_p_cm == NUL)
		ml_ಗಣ_crypt_ಕೀಲಿ(buf, buf->b_p_ಕೀಲಿ, ವಾದಗಳು->os_oldval.string);
    }
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

    ಪೂರ್ಣಾಂಕ
expand_ಗಣ_cryptmethod(optexpand_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಹಿಂತಿರುಗಿ expand_ಗಣ_opt_string(
	    ವಾದಗಳು,
	    p_cm_ಮೌಲ್ಯಗಳು,
	    ARRAY_ಉದ್ದಳತೆ(p_cm_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_CSCOPE) && defined(FEAT_QUICKFIX)
/*
 * The 'cscopequickfix' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_cscopequickfix(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    char_u	*p;

    ಆದರೆ (p_csqf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    p = p_csqf;
    ಆಗ (*p != NUL)
    {
	ಆದರೆ (vim_strchr((char_u *)CSQF_CMDS, *p) == ಖಾಲಿ
		|| p[1] == NUL
		|| vim_strchr((char_u *)CSQF_FLAGS, p[1]) == ಖಾಲಿ
		|| (p[2] != NUL && p[2] != ','))
	    ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p[2] == NUL)
	    ನಿಲ್ಲಿಸು;
	ಇಲ್ಲದಿದ್ದರೆ
	    p += 3;
    }

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_SYN_HL)
/*
 * The 'cursorlineopt' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_cursorlineopt(optset_T *ವಾದಗಳು)
{
    char_u	**varp = (char_u **)ವಾದಗಳು->os_varp;

    // This could be changed to use opt_strings_flags() instead.
    ಆದರೆ (**varp == NUL || fill_culopt_flags(*varp, curwin) != OK)
	ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

    ಪೂರ್ಣಾಂಕ
expand_ಗಣ_cursorlineopt(optexpand_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಹಿಂತಿರುಗಿ expand_ಗಣ_opt_string(
	    ವಾದಗಳು,
	    p_culopt_ಮೌಲ್ಯಗಳು,
	    ARRAY_ಉದ್ದಳತೆ(p_culopt_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * The 'debug' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_debug(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ did_ಗಣ_opt_ಅಕ್ಷರಸರಣಿಗಳು(p_debug, p_debug_ಮೌಲ್ಯಗಳು, TRUE);
}

    ಪೂರ್ಣಾಂಕ
expand_ಗಣ_debug(optexpand_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಹಿಂತಿರುಗಿ expand_ಗಣ_opt_string(
	    ವಾದಗಳು,
	    p_debug_ಮೌಲ್ಯಗಳು,
	    ARRAY_ಉದ್ದಳತೆ(p_debug_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}

#ಆದರೆ defined(FEAT_DIFF)
/*
 * The 'diffanchors' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_diffanchors(optset_T *ವಾದಗಳು)
{
    ಆದರೆ (diffanchors_ಬದಲಾಯಿತು(ವಾದಗಳು->os_flags & OPT_LOCAL) == FAIL)
	ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * The 'diffopt' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_diffopt(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಆದರೆ (diffopt_ಬದಲಾಯಿತು() == FAIL)
	ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

    ಪೂರ್ಣಾಂಕ
expand_ಗಣ_diffopt(optexpand_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    expand_T *xp = ವಾದಗಳು->oe_xp;

    ಆದರೆ (xp->xp_ನಮೂನೆ > ವಾದಗಳು->oe_ಗಣ_arg && *(xp->xp_ನಮೂನೆ-1) == ':')
    {
	ಆದರೆ (completing_ಮೌಲ್ಯ_for_subopt(ವಾದಗಳು, "algorithm"))
	{
	    ಹಿಂತಿರುಗಿ expand_ಗಣ_opt_string(
		    ವಾದಗಳು,
		    p_dip_algorithm_ಮೌಲ್ಯಗಳು,
		    ARRAY_ಉದ್ದಳತೆ(p_dip_algorithm_ಮೌಲ್ಯಗಳು) - 1,
		    ಅಂಕೆ_Matches,
		    matches);
	}
	ಆದರೆ (completing_ಮೌಲ್ಯ_for_subopt(ವಾದಗಳು, "inline"))
	{
	    ಹಿಂತಿರುಗಿ expand_ಗಣ_opt_string(
		    ವಾದಗಳು,
		    p_dip_inline_ಮೌಲ್ಯಗಳು,
		    ARRAY_ಉದ್ದಳತೆ(p_dip_inline_ಮೌಲ್ಯಗಳು) - 1,
		    ಅಂಕೆ_Matches,
		    matches);
	}
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಹಿಂತಿರುಗಿ expand_ಗಣ_opt_string(
	    ವಾದಗಳು,
	    p_dip_ಮೌಲ್ಯಗಳು,
	    ARRAY_ಉದ್ದಳತೆ(p_dip_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * The 'display' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_display(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಆದರೆ (opt_ಅಕ್ಷರಸರಣಿಗಳು_flags(p_dy, p_dy_ಮೌಲ್ಯಗಳು, &dy_flags, TRUE) != OK)
	ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;

    (ಶೂನ್ಯ)init_chartab();
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

    ಪೂರ್ಣಾಂಕ
expand_ಗಣ_display(optexpand_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಹಿಂತಿರುಗಿ expand_ಗಣ_opt_string(
	    ವಾದಗಳು,
	    p_dy_ಮೌಲ್ಯಗಳು,
	    ARRAY_ಉದ್ದಳತೆ(p_dy_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}

/*
 * The 'eadirection' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_eadirection(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ did_ಗಣ_opt_ಅಕ್ಷರಸರಣಿಗಳು(p_ead, p_ead_ಮೌಲ್ಯಗಳು, FALSE);
}

    ಪೂರ್ಣಾಂಕ
expand_ಗಣ_eadirection(optexpand_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಹಿಂತಿರುಗಿ expand_ಗಣ_opt_string(
	    ವಾದಗಳು,
	    p_ead_ಮೌಲ್ಯಗಳು,
	    ARRAY_ಉದ್ದಳತೆ(p_ead_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}

/*
 * One of the 'encoding', 'fileencoding', 'termencoding' or 'makeencoding'
 * options is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_encoding(optset_T *ವಾದಗಳು)
{
    char_u	**varp = (char_u **)ವಾದಗಳು->os_varp;
    char_u	**gvarp;
    ಅಕ್ಷರ	*errmsg = ಖಾಲಿ;
    char_u	*p;

    // Get the global option to compare with, otherwise we would have to check
    // two values for all local options.
    gvarp = (char_u **)get_option_varp_scope(ವಾದಗಳು->os_idx, OPT_GLOBAL);

    ಆದರೆ (gvarp == &p_fenc)
    {
	ಆದರೆ (!curbuf->b_p_ma && ವಾದಗಳು->os_flags != OPT_GLOBAL)
	    errmsg = e_cannot_make_changes_modifiable_is_off;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (vim_strchr(*varp, ',') != ಖಾಲಿ)
	    // No comma allowed in 'fileencoding'; catches confusing it
	    // with 'fileencodings'.
	    errmsg = e_invalid_ವಾದ;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // May show a "+" in the title now.
	    redraw_titles();
	    // Add 'fileencoding' to the swap file.
	    ml_setflags(curbuf);
	}
    }
    ಆದರೆ (errmsg == ಖಾಲಿ)
    {
	// canonize the value, so that STRCMP() can be used on it
	p = enc_canonize(*varp);
	ಆದರೆ (p != ಖಾಲಿ)
	{
	    vim_free(*varp);
	    *varp = p;
	}
	ಆದರೆ (varp == &p_enc)
	{
	    errmsg = mb_init();
	    redraw_titles();
	}
    }

#ಆದರೆ defined(FEAT_GUI_GTK)
    ಆದರೆ (errmsg == ಖಾಲಿ && varp == &p_tenc && gui.ಒಳ_use)
    {
	// GTK uses only a single encoding, and that is UTF-8.
	ಆದರೆ (STRCMP(p_tenc, "utf-8") != 0)
	    errmsg = e_cannot_be_ಬದಲಾಯಿತು_ಒಳ_gtk_GUI;
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (errmsg == ಖಾಲಿ)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_KEYMAP
	// When 'keymap' is used and 'encoding' changes, reload the keymap
	// (with another encoding).
	ಆದರೆ (varp == &p_enc && *curbuf->b_p_keymap != NUL)
	    (ಶೂನ್ಯ)keymap_init();
#ಆದರೆ_ಅಂತ್ಯ

	// When 'termencoding' is not empty and 'encoding' changes or when
	// 'termencoding' changes, need to setup for keyboard input and
	// display output conversion.
	ಆದರೆ (((varp == &p_enc && *p_tenc != NUL) || varp == &p_tenc))
	{
	    ಆದರೆ (convert_ಸಿದ್ಧತೆ(&ಒಳಹರಿವು_conv, p_tenc, p_enc) == FAIL
		    || convert_ಸಿದ್ಧತೆ(&output_conv, p_enc, p_tenc) == FAIL)
	    {
		semsg(_(e_cannot_convert_between_str_and_str),
			p_tenc, p_enc);
		errmsg = e_invalid_ವಾದ;
	    }
	}

#ಆದರೆ defined(MSWIN)
	// $HOME, $VIM and $VIMRUNTIME may have characters in active code page.
	ಆದರೆ (varp == &p_enc)
	{
	    init_homedir();
	    init_vimdir();
	}
#ಆದರೆ_ಅಂತ್ಯ
    }

    ಹಿಂತಿರುಗಿ errmsg;
}

    ಪೂರ್ಣಾಂಕ
expand_ಗಣ_encoding(optexpand_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಹಿಂತಿರುಗಿ expand_ಗಣ_opt_generic(
	    ವಾದಗಳು,
	    get_encoding_name,
	    ಅಂಕೆ_Matches,
	    matches);
}

/*
 * The 'eventignore(win)' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_eventignore(optset_T *ವಾದಗಳು)
{
    char_u	**varp = (char_u **)ವಾದಗಳು->os_varp;
    char_u	*oldval = ವಾದಗಳು->os_oldval.string;

    ಆದರೆ (check_ei(*varp) == FAIL)
	ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;

    ಆದರೆ (oldval == ಖಾಲಿ || STRCMP(oldval, *varp) == 0)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    // Deal with the events that are triggered by comparing against a stored
    // state, with the old value in effect: what happened while an event was
    // ignored must not be reported once it is not ignored anymore, and what
    // happened before must still be reported.
    // Use a copy, the caller owns "oldval" and autocommands may free it.
    char_u	*save_ei = vim_strsave(oldval);
    ಆದರೆ (save_ei != ಖಾಲಿ)
    {
	char_u	*newval = *varp;
	win_T	*wp = is_ಕಿಟಕಿ_local_option(ವಾದಗಳು->os_idx) ? curwin : ಖಾಲಿ;

	*varp = save_ei;
	// "varp" points into "wp" for 'eventignorewin'.
	ಆದರೆ (wp != ಖಾಲಿ)
	    ++wp->w_locked;
	may_trigger_deferred_events();
	ಆದರೆ (wp != ಖಾಲಿ)
	    --wp->w_locked;

	free_string_option(*varp);
	*varp = newval;
    }

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ expand_eiw = FALSE;

    ಸ್ಥಿರ char_u *
get_eventignore_name(expand_T *xp, ಪೂರ್ಣಾಂಕ idx)
{
    ಪೂರ್ಣಾಂಕ ಕಳೆ = *xp->xp_ನಮೂನೆ == '-';
    // 'eventignore(win)' allows special keyword "all" in addition to
    // all event names.
    ಆದರೆ (!ಕಳೆ && idx == 0)
	ಹಿಂತಿರುಗಿ (char_u *)"all";

    char_u *name = get_event_name_no_ಗುಂಪು(xp, idx - 1 + ಕಳೆ, expand_eiw);
    ಆದರೆ (name == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    sprintf((ಅಕ್ಷರ *)IObuff, "%s%s", ಕಳೆ ? "-" : "", name);
    ಹಿಂತಿರುಗಿ IObuff;
}

    ಪೂರ್ಣಾಂಕ
expand_ಗಣ_eventignore(optexpand_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    expand_eiw = ವಾದಗಳು->oe_varp != (char_u *)&p_ei;
    ಹಿಂತಿರುಗಿ expand_ಗಣ_opt_generic(
	    ವಾದಗಳು,
	    get_eventignore_name,
	    ಅಂಕೆ_Matches,
	    matches);
}

/*
 * The 'fileformat' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_fileformat(optset_T *ವಾದಗಳು)
{
    char_u	**varp = (char_u **)ವಾದಗಳು->os_varp;

    ಆದರೆ (!curbuf->b_p_ma && !(ವಾದಗಳು->os_flags & OPT_GLOBAL))
	ಹಿಂತಿರುಗಿ e_cannot_make_changes_modifiable_is_off;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (check_opt_ಅಕ್ಷರಸರಣಿಗಳು(*varp, p_ff_ಮೌಲ್ಯಗಳು, FALSE) != OK)
	ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;

    // may also change 'textmode'
    ಆದರೆ (get_fileformat(curbuf) == EOL_DOS)
	curbuf->b_p_tx = TRUE;
    ಇಲ್ಲದಿದ್ದರೆ
	curbuf->b_p_tx = FALSE;
    redraw_titles();
    // update flag in swap file
    ml_setflags(curbuf);
    // Redraw needed when switching to/from "mac": a CR in the text
    // will be displayed differently.
    ಆದರೆ (get_fileformat(curbuf) == EOL_MAC || *ವಾದಗಳು->os_oldval.string == 'm')
	redraw_curbuf_later(UPD_NOT_VALID);

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

    ಪೂರ್ಣಾಂಕ
expand_ಗಣ_fileformat(optexpand_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಹಿಂತಿರುಗಿ expand_ಗಣ_opt_string(
	    ವಾದಗಳು,
	    p_ff_ಮೌಲ್ಯಗಳು,
	    ARRAY_ಉದ್ದಳತೆ(p_ff_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * fileformat options.
 */
    char_u *
get_fileformat_name(expand_T *xp ಬಳಸದ, ಪೂರ್ಣಾಂಕ idx)
{
    ಆದರೆ (idx >= (ಪೂರ್ಣಾಂಕ)ARRAY_ಉದ್ದಳತೆ(p_ff_ಮೌಲ್ಯಗಳು))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಹಿಂತಿರುಗಿ (char_u*)p_ff_ಮೌಲ್ಯಗಳು[idx];
}

/*
 * The 'fileformats' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_fileformats(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಆದರೆ (check_opt_ಅಕ್ಷರಸರಣಿಗಳು(p_ffs, p_ff_ಮೌಲ್ಯಗಳು, TRUE) != OK)
	ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;

    // also change 'textauto'
    ಆದರೆ (*p_ffs == NUL)
	p_ta = FALSE;
    ಇಲ್ಲದಿದ್ದರೆ
	p_ta = TRUE;

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * The 'filetype' or the 'syntax' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_filetype_or_syntax(optset_T *ವಾದಗಳು)
{
    char_u	**varp = (char_u **)ವಾದಗಳು->os_varp;

    ಆದರೆ (!valid_filetype(*varp))
	ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;

    ವಾದಗಳು->os_ಮೌಲ್ಯ_ಬದಲಾಯಿತು = STRCMP(ವಾದಗಳು->os_oldval.string, *varp) != 0;

    // Since we check the value, there is no need to set P_INSECURE,
    // even when the value comes from a modeline.
    ವಾದಗಳು->os_ಮೌಲ್ಯ_checked = TRUE;

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

#ಆದರೆ defined(FEAT_FOLDING)
/*
 * The 'foldclose' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_foldclose(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ did_ಗಣ_opt_ಅಕ್ಷರಸರಣಿಗಳು(p_fcl, p_fcl_ಮೌಲ್ಯಗಳು, TRUE);
}

    ಪೂರ್ಣಾಂಕ
expand_ಗಣ_foldclose(optexpand_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಹಿಂತಿರುಗಿ expand_ಗಣ_opt_string(
	    ವಾದಗಳು,
	    p_fcl_ಮೌಲ್ಯಗಳು,
	    ARRAY_ಉದ್ದಳತೆ(p_fcl_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_EVAL) && defined(FEAT_FOLDING)
/*
 * The 'foldexpr' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_foldexpr(optset_T *ವಾದಗಳು)
{
    (ಶೂನ್ಯ)did_ಗಣ_optexpr(ವಾದಗಳು);
    ಆದರೆ (foldmethod_Is_ಅಭಿವ್ಯಕ್ತಿ(curwin))
	foldUpdateAll(curwin);
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_FOLDING)
/*
 * The 'foldignore' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_foldignore(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಆದರೆ (foldmethodIsIndent(curwin))
	foldUpdateAll(curwin);
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * The 'foldmarker' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_foldmarker(optset_T *ವಾದಗಳು)
{
    char_u	**varp = (char_u **)ವಾದಗಳು->os_varp;
    char_u	*p;

    p = vim_strchr(*varp, ',');
    ಆದರೆ (p == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ e_comma_required;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p == *varp || p[1] == NUL)
	ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (foldmethodIsMarker(curwin))
	foldUpdateAll(curwin);

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * The 'foldmethod' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_foldmethod(optset_T *ವಾದಗಳು)
{
    char_u	**varp = (char_u **)ವಾದಗಳು->os_varp;

    ಆದರೆ (check_opt_ಅಕ್ಷರಸರಣಿಗಳು(*varp, p_fdm_ಮೌಲ್ಯಗಳು, FALSE) != OK || **varp == NUL)
	ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;

    foldUpdateAll(curwin);
    ಆದರೆ (foldmethodIsDiff(curwin))
	newFoldLevel();
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

    ಪೂರ್ಣಾಂಕ
expand_ಗಣ_foldmethod(optexpand_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಹಿಂತಿರುಗಿ expand_ಗಣ_opt_string(
	    ವಾದಗಳು,
	    p_fdm_ಮೌಲ್ಯಗಳು,
	    ARRAY_ಉದ್ದಳತೆ(p_fdm_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}

/*
 * The 'foldopen' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_foldopen(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ did_ಗಣ_opt_flags(p_fdo, p_fdo_ಮೌಲ್ಯಗಳು, &fdo_flags, TRUE);
}

    ಪೂರ್ಣಾಂಕ
expand_ಗಣ_foldopen(optexpand_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಹಿಂತಿರುಗಿ expand_ಗಣ_opt_string(
	    ವಾದಗಳು,
	    p_fdo_ಮೌಲ್ಯಗಳು,
	    ARRAY_ಉದ್ದಳತೆ(p_fdo_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * The 'formatoptions' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_formatoptions(optset_T *ವಾದಗಳು)
{
    char_u	**varp = (char_u **)ವಾದಗಳು->os_varp;

    ಹಿಂತಿರುಗಿ did_ಗಣ_option_listflag(*varp, (char_u *)FO_ALL, ವಾದಗಳು->os_errbuf,
		    ವಾದಗಳು->os_errbuflen);
}

    ಪೂರ್ಣಾಂಕ
expand_ಗಣ_formatoptions(optexpand_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಹಿಂತಿರುಗಿ expand_ಗಣ_opt_listflag(ವಾದಗಳು, (char_u*)FO_ALL, ಅಂಕೆ_Matches, matches);
}

#ಆದರೆ defined(CURSOR_SHAPE)
/*
 * The 'guicursor' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_guicursor(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ parse_shape_opt(SHAPE_CURSOR);
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_GUI)
/*
 * The 'guifont' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_guifont(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    char_u	*p;
    ಅಕ್ಷರ	*errmsg = ಖಾಲಿ;

    ಆದರೆ (gui.ಒಳ_use)
    {
	p = p_guifont;
# ಆದರೆ defined(FEAT_GUI_GTK)
	// Put up a font dialog and let the user select a new value.
	// If this is cancelled go back to the old value but don't
	// give an error message.
	ಆದರೆ (STRCMP(p, "*") == 0)
	{
	    p = gui_mch_font_dialog(ವಾದಗಳು->os_oldval.string);
	    free_string_option(p_guifont);
	    p_guifont = (p != ಖಾಲಿ) ? p : vim_strsave(ವಾದಗಳು->os_oldval.string);
	}
# endif
	ಆದರೆ (p != ಖಾಲಿ && gui_init_font(p_guifont, FALSE) != OK)
	{
# ಆದರೆ defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_PHOTON)
	    ಆದರೆ (STRCMP(p_guifont, "*") == 0)
	    {
		// Dialog was cancelled: Keep the old value without giving
		// an error message.
		free_string_option(p_guifont);
		p_guifont = vim_strsave(ವಾದಗಳು->os_oldval.string);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
# endif
		errmsg = e_invalid_fonts;
	}
    }

    ಹಿಂತಿರುಗಿ errmsg;
}

/*
 * Expand the 'guifont' option. Only when GUI is being used. Each platform has
 * specific behaviors.
 */
    ಪೂರ್ಣಾಂಕ
expand_ಗಣ_guifont(optexpand_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಆದರೆ (!gui.ಒಳ_use)
	ಹಿಂತಿರುಗಿ FAIL;

# ಆದರೆ defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
    char_u **varp = (char_u **)ವಾದಗಳು->oe_varp;
    ಪೂರ್ಣಾಂಕ wide = (varp == &p_guifontwide);

    ಹಿಂತಿರುಗಿ expand_ಗಣ_opt_callback(
	    ವಾದಗಳು, gui_mch_expand_font, &wide, ಅಂಕೆ_Matches, matches);
# ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ FAIL;
# endif
}

# ಆದರೆ defined(FEAT_XFONTSET)
/*
 * The 'guifontset' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_guifontset(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಅಕ್ಷರ *errmsg = ಖಾಲಿ;

    ಆದರೆ (STRCMP(p_guifontset, "*") == 0)
	errmsg = e_cant_select_fontset;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (gui.ಒಳ_use && gui_init_font(p_guifontset, TRUE) != OK)
	errmsg = e_invalid_fontset;

    ಹಿಂತಿರುಗಿ errmsg;
}
# endif

/*
 * The 'guifontwide' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_guifontwide(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಅಕ್ಷರ *errmsg = ಖಾಲಿ;

    ಆದರೆ (STRCMP(p_guifontwide, "*") == 0)
	errmsg = e_cant_select_wide_font;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (gui_get_wide_font() == FAIL)
	errmsg = e_invalid_wide_font;

    ಹಿಂತಿರುಗಿ errmsg;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
/*
 * The 'guiligatures' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_guiligatures(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    gui_ಗಣ_ligatures();
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_GUI)
/*
 * The 'guioptions' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_guioptions(optset_T *ವಾದಗಳು)
{
    char_u	**varp = (char_u **)ವಾದಗಳು->os_varp;
    ಅಕ್ಷರ *errmsg;

    errmsg = did_ಗಣ_option_listflag(*varp, (char_u *)GO_ALL, ವಾದಗಳು->os_errbuf,
		    ವಾದಗಳು->os_errbuflen);
    ಆದರೆ (errmsg != ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ errmsg;

    gui_init_which_components(ವಾದಗಳು->os_oldval.string);
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

    ಪೂರ್ಣಾಂಕ
expand_ಗಣ_guioptions(optexpand_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಹಿಂತಿರುಗಿ expand_ಗಣ_opt_listflag(ವಾದಗಳು, (char_u*)GO_ALL, ಅಂಕೆ_Matches, matches);
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_GUI_TABLINE)
/*
 * The 'guitablabel' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_guitablabel(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    redraw_tabline = TRUE;
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * The 'helpfile' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_helpfile(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    // May compute new values for $VIM and $VIMRUNTIME
    ಆದರೆ (didset_vim)
	vim_unsetenv_ext((char_u *)"VIM");
    ಆದರೆ (didset_vimruntime)
	vim_unsetenv_ext((char_u *)"VIMRUNTIME");
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

#ಆದರೆ defined(FEAT_MULTI_LANG)
/*
 * The 'helplang' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_helplang(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಅಕ್ಷರ *errmsg = ಖಾಲಿ;

    // Check for "", "ab", "ab,cd", etc.
    ಗಾಗಿ (char_u *s = p_hlg; *s != NUL; s += 3)
    {
	ಆದರೆ (s[1] == NUL || ((s[2] != ',' || s[3] == NUL) && s[2] != NUL))
	{
	    errmsg = e_invalid_ವಾದ;
	    ನಿಲ್ಲಿಸು;
	}
	ಆದರೆ (s[2] == NUL)
	    ನಿಲ್ಲಿಸು;
    }

    ಹಿಂತಿರುಗಿ errmsg;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * The 'highlight' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_highlight(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಆದರೆ (highlight_ಬದಲಾಯಿತು() == FAIL)
	ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;	// invalid flags

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
expand_hl_occasions(
	optexpand_T *ವಾದಗಳು,
	ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches,
	char_u ***matches,
	char_u ಪೂರ್ವಪ್ರತ್ಯಯ)
{
    char_u	    *p;
    ಸ್ಥಿರ char_u   hl_flags[HLF_ಎಣಿಕೆ] = HL_FLAGS;
    size_t	    i;
    ಪೂರ್ಣಾಂಕ		    ಎಣಿಕೆ_2 = 0;

    *matches = ALLOC_MULT(char_u *, HLF_ಎಣಿಕೆ + 1);
    ಆದರೆ (*matches == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    // We still want to return the full option if it's requested.
    ಆದರೆ (ವಾದಗಳು->oe_include_orig_val)
    {
	p = vim_strsave(ವಾದಗಳು->oe_opt_ಮೌಲ್ಯ);
	ಆದರೆ (p == ಖಾಲಿ)
	{
	    VIM_CLEAR(*matches);
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	(*matches)[ಎಣಿಕೆ_2++] = p;
    }

    ಗಾಗಿ (i = 0; i < HLF_ಎಣಿಕೆ; i++)
    {
	p = alloc((ಪೂರ್ವಪ್ರತ್ಯಯ == NUL ? 1 : 2) + 1);
	ಆದರೆ (p == ಖಾಲಿ)
	{
	    ಆದರೆ (ಎಣಿಕೆ_2 == 0)
	    {
		VIM_CLEAR(*matches);
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		ನಿಲ್ಲಿಸು;
	}
	ಆದರೆ (ಪೂರ್ವಪ್ರತ್ಯಯ == NUL)
	    sprintf((ಅಕ್ಷರ *)p, "%c", hl_flags[i]);
	ಇಲ್ಲದಿದ್ದರೆ
	    sprintf((ಅಕ್ಷರ *)p, "%c%c", ಪೂರ್ವಪ್ರತ್ಯಯ, hl_flags[i]);
	(*matches)[ಎಣಿಕೆ_2++] = p;
    }

    ಆದರೆ (ಎಣಿಕೆ_2 == 0)
    {
	VIM_CLEAR(*matches);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    *ಅಂಕೆ_Matches = ಎಣಿಕೆ_2;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Expand 'highlight' option.
 */
    ಪೂರ್ಣಾಂಕ
expand_ಗಣ_highlight(optexpand_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    char_u	    *p;
    expand_T	    *xp = ವಾದಗಳು->oe_xp;
    size_t	    i;
    ಪೂರ್ಣಾಂಕ		    ಎಣಿಕೆ_2 = 0;

    ಆದರೆ (xp->xp_ನಮೂನೆ > ವಾದಗಳು->oe_ಗಣ_arg && *(xp->xp_ನಮೂನೆ-1) == ':')
    {
	// Right after a ':', meaning we just return all highlight names.
	ಹಿಂತಿರುಗಿ expand_ಗಣ_opt_generic(
		ವಾದಗಳು,
		get_highlight_name,
		ಅಂಕೆ_Matches,
		matches);
    }

    ಆದರೆ (*xp->xp_ನಮೂನೆ == NUL)
	// At beginning of a comma-separated list. Return the specific list of
	// supported occasions.
	ಹಿಂತಿರುಗಿ expand_hl_occasions(ವಾದಗಳು, ಅಂಕೆ_Matches, matches, NUL);

    // We are after the initial character (which indicates the occasion). We
    // already made sure we are not matching after a ':' above, so now we want
    // to match against display mode modifiers.
    // Since the xp_pattern starts from the beginning, we need to include it in
    // the returned match.

    // Note: Keep this in sync with highlight_changed()
    ಸ್ಥಿರ char_u p_hl_ಬಹುಲಕ_ಮೌಲ್ಯಗಳು[] =
	{':', 'b', 'i', '-', 'n', 'r', 's', 'u', 'c', '2', 'd', '=', 't'};
    size_t ಅಂಕೆ_hl_modes = ARRAY_ಉದ್ದಳತೆ(p_hl_ಬಹುಲಕ_ಮೌಲ್ಯಗಳು);

    *matches = ALLOC_MULT(char_u *, ಅಂಕೆ_hl_modes);
    ಆದರೆ (*matches == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    ಪೂರ್ಣಾಂಕ ನಮೂನೆ_len = xp->xp_ನಮೂನೆ_len;

    ಗಾಗಿ (i = 0; i < ಅಂಕೆ_hl_modes; i++)
    {
	// Don't allow duplicates as these are unique flags
	char_u *dup = vim_strchr(xp->xp_ನಮೂನೆ + 1, p_hl_ಬಹುಲಕ_ಮೌಲ್ಯಗಳು[i]);
	ಆದರೆ (dup != ಖಾಲಿ && (ಪೂರ್ಣಾಂಕ)(dup - xp->xp_ನಮೂನೆ) < ನಮೂನೆ_len)
	    ಮುಂದುವರಿ;

	// ':' only works by itself, not with other flags.
	ಆದರೆ (ನಮೂನೆ_len > 1 && p_hl_ಬಹುಲಕ_ಮೌಲ್ಯಗಳು[i] == ':')
	    ಮುಂದುವರಿ;

	p = vim_strnsave(xp->xp_ನಮೂನೆ, ನಮೂನೆ_len + 1);
	ಆದರೆ (p == ಖಾಲಿ)
	{
	    ಆದರೆ (i == 0)
	    {
		VIM_CLEAR(*matches);
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		ನಿಲ್ಲಿಸು;
	}
	p[ನಮೂನೆ_len] = p_hl_ಬಹುಲಕ_ಮೌಲ್ಯಗಳು[i];
	p[ನಮೂನೆ_len + 1] = NUL;
	(*matches)[ಎಣಿಕೆ_2++] = p;
    }
    ಆದರೆ (ಎಣಿಕೆ_2 == 0)
    {
	VIM_CLEAR(*matches);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    *ಅಂಕೆ_Matches = ಎಣಿಕೆ_2;

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * The 'titlestring' or the 'iconstring' option is changed.
 */
    ಸ್ಥಿರ ಅಕ್ಷರ *
parse_titleiconstring(optset_T *ವಾದಗಳು ಬಳಸದ, ಪೂರ್ಣಾಂಕ flagval ಬಳಸದ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_STL_OPT
    char_u	**varp = (char_u **)ವಾದಗಳು->os_varp;

    // NULL => statusline syntax
    ಆದರೆ (vim_strchr(*varp, '%') && check_stl_option(*varp) == ಖಾಲಿ)
	stl_syntax |= flagval;
    ಇಲ್ಲದಿದ್ದರೆ
	stl_syntax &= ~flagval;
#ಆದರೆ_ಅಂತ್ಯ
    did_ಗಣ_title();

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * The 'iconstring' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_iconstring(optset_T *ವಾದಗಳು)
{
    ಪೂರ್ಣಾಂಕ flagval = 0;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_STL_OPT
    flagval = STL_ಒಳ_ICON;
#ಆದರೆ_ಅಂತ್ಯ

    ಹಿಂತಿರುಗಿ parse_titleiconstring(ವಾದಗಳು, flagval);
}

#ಆದರೆ defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
/*
 * The 'imactivatekey' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_imactivatekey(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಆದರೆ (!im_xim_isvalid_imactivate())
	ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_IMAGE
/*
 * The 'imageprotocol' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_imageprotocol(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಆದರೆ (update_image_backend() == FAIL)
	ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

    ಪೂರ್ಣಾಂಕ
expand_ಗಣ_imageprotocol(optexpand_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    expand_T *xp = ವಾದಗಳು->oe_xp;

    ಆದರೆ (xp->xp_ನಮೂನೆ > ವಾದಗಳು->oe_ಗಣ_arg && *(xp->xp_ನಮೂನೆ-1) == ':')
    {
	// 'imageprotocol' only has well-defined terms for completion for the
	// protocol part after the colon.
	ಹಿಂತಿರುಗಿ expand_ಗಣ_opt_string(
		ವಾದಗಳು,
		p_ipc_protocol_ಮೌಲ್ಯಗಳು,
		ARRAY_ಉದ್ದಳತೆ(p_ipc_protocol_ಮೌಲ್ಯಗಳು) - 1,
		ಅಂಕೆ_Matches,
		matches);
    }
    // Use expand_set_opt_string instead of returning FAIL so that we can
    // include the original value if args->oe_include_orig_val is set.
    ಸ್ಥಿರ ಅಕ್ಷರ *(ಖಾಲಿಗಳು_2[]) = {ಖಾಲಿ};
    ಹಿಂತಿರುಗಿ expand_ಗಣ_opt_string(ವಾದಗಳು, ಖಾಲಿಗಳು_2, 0, ಅಂಕೆ_Matches, matches);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * The 'iskeyword' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_iskeyword(optset_T *ವಾದಗಳು)
{
    char_u	**varp = (char_u **)ವಾದಗಳು->os_varp;

    ಆದರೆ (varp == &p_isk)		// only check for global-value
    {
	ಆದರೆ (check_isopt(*varp) == FAIL)
	    ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;
    }
    ಇಲ್ಲದಿದ್ದರೆ			// fallthrough for local-value
	ಹಿಂತಿರುಗಿ did_ಗಣ_isopt(ವಾದಗಳು);

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * The 'isident' or the 'iskeyword' or the 'isprint' or the 'isfname' option is
 * changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_isopt(optset_T *ವಾದಗಳು)
{
    // 'isident', 'iskeyword', 'isprint' or 'isfname' option: refill g_chartab[]
    // If the new option is invalid, use old value.
    // 'lisp' option: refill g_chartab[] for '-' char.
    ಆದರೆ (init_chartab() == FAIL)
    {
	ವಾದಗಳು->os_restore_chartab = TRUE;// need to restore the chartab.
	ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;	// error in value
    }

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * The 'jumpoptions' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_jumpoptions(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಆದರೆ (opt_ಅಕ್ಷರಸರಣಿಗಳು_flags(p_jop, p_jop_ಮೌಲ್ಯಗಳು, &jop_flags, TRUE) != OK)
	ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

    ಪೂರ್ಣಾಂಕ
expand_ಗಣ_jumpoptions(optexpand_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಹಿಂತಿರುಗಿ expand_ಗಣ_opt_string(
	    ವಾದಗಳು,
	    p_jop_ಮೌಲ್ಯಗಳು,
	    ARRAY_ಉದ್ದಳತೆ(p_jop_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}

#ಆದರೆ defined(FEAT_KEYMAP)
/*
 * The 'keymap' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_keymap(optset_T *ವಾದಗಳು)
{
    char_u	**varp = (char_u **)ವಾದಗಳು->os_varp;
    ಅಕ್ಷರ *errmsg = ಖಾಲಿ;

    ಆದರೆ (!valid_filetype(*varp))
	errmsg = e_invalid_ವಾದ;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಪೂರ್ಣಾಂಕ	    secure_save = secure;

	// Reset the secure flag, since the value of 'keymap' has
	// been checked to be safe.
	secure = 0;

	// load or unload key mapping tables
	errmsg = keymap_init();

	secure = secure_save;

	// Since we check the value, there is no need to set P_INSECURE,
	// even when the value comes from a modeline.
	ವಾದಗಳು->os_ಮೌಲ್ಯ_checked = TRUE;
    }

    ಆದರೆ (errmsg == ಖಾಲಿ)
    {
	ಆದರೆ (*curbuf->b_p_keymap != NUL)
	{
	    // Installed a new keymap, switch on using it.
	    curbuf->b_p_iminsert = B_IMODE_LMAP;
	    ಆದರೆ (curbuf->b_p_imsearch != B_IMODE_USE_ಸೇರಿಸು)
		curbuf->b_p_imsearch = B_IMODE_LMAP;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // Cleared the keymap, may reset 'iminsert' and 'imsearch'.
	    ಆದರೆ (curbuf->b_p_iminsert == B_IMODE_LMAP)
		curbuf->b_p_iminsert = B_IMODE_NONE;
	    ಆದರೆ (curbuf->b_p_imsearch == B_IMODE_LMAP)
		curbuf->b_p_imsearch = B_IMODE_USE_ಸೇರಿಸು;
	}
	ಆದರೆ ((ವಾದಗಳು->os_flags & OPT_LOCAL) == 0)
	{
	    ಗಣ_iminsert_global();
	    ಗಣ_imsearch_global();
	}
	ಸ್ಥಿತಿ_redraw_curbuf();
    }

    ಹಿಂತಿರುಗಿ errmsg;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * The 'keymodel' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_keymodel(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಆದರೆ (check_opt_ಅಕ್ಷರಸರಣಿಗಳು(p_km, p_km_ಮೌಲ್ಯಗಳು, TRUE) != OK)
	ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;

    km_stopsel = (vim_strchr(p_km, 'o') != ಖಾಲಿ);
    km_startsel = (vim_strchr(p_km, 'a') != ಖಾಲಿ);
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

    ಪೂರ್ಣಾಂಕ
expand_ಗಣ_keymodel(optexpand_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಹಿಂತಿರುಗಿ expand_ಗಣ_opt_string(
	    ವಾದಗಳು,
	    p_km_ಮೌಲ್ಯಗಳು,
	    ARRAY_ಉದ್ದಳತೆ(p_km_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}

/*
 * The 'keyprotocol' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_keyprotocol(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    char_u *ಪದ = T_NAME;
    keyprot_T kpc = ಹೊಂದಿಕೆ_keyprotocol(ಪದ);
    ಆದರೆ (kpc == KEYPROTOCOL_FAIL)
	ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;

    apply_keyprotocol(ಪದ, kpc);

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

    ಪೂರ್ಣಾಂಕ
expand_ಗಣ_keyprotocol(optexpand_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    expand_T *xp = ವಾದಗಳು->oe_xp;
    ಆದರೆ (xp->xp_ನಮೂನೆ > ವಾದಗಳು->oe_ಗಣ_arg && *(xp->xp_ನಮೂನೆ-1) == ':')
    {
	// 'keyprotocol' only has well-defined terms for completion for the
	// protocol part after the colon.
	ಹಿಂತಿರುಗಿ expand_ಗಣ_opt_string(
		ವಾದಗಳು,
		p_kpc_protocol_ಮೌಲ್ಯಗಳು,
		ARRAY_ಉದ್ದಳತೆ(p_kpc_protocol_ಮೌಲ್ಯಗಳು) - 1,
		ಅಂಕೆ_Matches,
		matches);
    }
    // Use expand_set_opt_string instead of returning FAIL so that we can
    // include the original value if args->oe_include_orig_val is set.
    ಸ್ಥಿರ ಅಕ್ಷರ *(ಖಾಲಿಗಳು_2[]) = {ಖಾಲಿ};
    ಹಿಂತಿರುಗಿ expand_ಗಣ_opt_string(ವಾದಗಳು, ಖಾಲಿಗಳು_2, 0, ಅಂಕೆ_Matches, matches);
}

/*
 * The 'lispoptions' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_lispoptions(optset_T *ವಾದಗಳು)
{
    char_u	**varp = (char_u **)ವಾದಗಳು->os_varp;

    ಆದರೆ (**varp != NUL
	    && STRCMP(*varp, "expr:0") != 0
	    && STRCMP(*varp, "expr:1") != 0)
	ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

    ಪೂರ್ಣಾಂಕ
expand_ಗಣ_lispoptions(optexpand_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಸ್ಥಿರ ಅಕ್ಷರ *(p_lop_ಮೌಲ್ಯಗಳು[]) = {"expr:0", "expr:1", ಖಾಲಿ};
    ಹಿಂತಿರುಗಿ expand_ಗಣ_opt_string(
	    ವಾದಗಳು,
	    p_lop_ಮೌಲ್ಯಗಳು,
	    ARRAY_ಉದ್ದಳತೆ(p_lop_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}

/*
 * The 'matchpairs' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_matchpairs(optset_T *ವಾದಗಳು)
{
    char_u	**varp = (char_u **)ವಾದಗಳು->os_varp;
    char_u	*p;

    ಆದರೆ (has_mbyte)
    {
	ಗಾಗಿ (p = *varp; *p != NUL; ++p)
	{
	    ಪೂರ್ಣಾಂಕ x2 = -1;
	    ಪೂರ್ಣಾಂಕ x3 = -1;

	    p += mb_ptr2len(p);
	    ಆದರೆ (*p != NUL)
		x2 = *p++;
	    ಆದರೆ (*p != NUL)
	    {
		x3 = mb_ptr2char(p);
		p += mb_ptr2len(p);
	    }
	    ಆದರೆ (x2 != ':' || x3 == -1 || (*p != NUL && *p != ','))
		ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;
	    ಆದರೆ (*p == NUL)
		ನಿಲ್ಲಿಸು;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Check for "x:y,x:y"
	ಗಾಗಿ (p = *varp; *p != NUL; p += 4)
	{
	    ಆದರೆ (p[1] != ':' || p[2] == NUL || (p[3] != NUL && p[3] != ','))
		ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;
	    ಆದರೆ (p[3] == NUL)
		ನಿಲ್ಲಿಸು;
	}
    }

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Process the updated 'messagesopt' option value.
 */
    ಅಕ್ಷರ *
did_ಗಣ_messagesopt(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಆದರೆ (messagesopt_ಬದಲಾಯಿತು() == FAIL)
	ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

    ಪೂರ್ಣಾಂಕ
expand_ಗಣ_messagesopt(optexpand_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಸ್ಥಿರ ಅಕ್ಷರ *(p_mopt_ಮೌಲ್ಯಗಳು[]) = {"hit-enter", "wait:", "history:", ಖಾಲಿ};
    ಹಿಂತಿರುಗಿ expand_ಗಣ_opt_string(
	    ವಾದಗಳು,
	    p_mopt_ಮೌಲ್ಯಗಳು,
	    ARRAY_ಉದ್ದಳತೆ(p_mopt_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}

#ಆದರೆ defined(FEAT_SPELL)
/*
 * The 'mkspellmem' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_mkspellmem(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಆದರೆ (spell_check_msm() != OK)
	ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * The 'mouse' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_mouse(optset_T *ವಾದಗಳು)
{
    char_u	**varp = (char_u **)ವಾದಗಳು->os_varp;
    ಅಕ್ಷರ	*retval;

    retval = did_ಗಣ_option_listflag(*varp, (char_u *)MOUSE_ALL, ವಾದಗಳು->os_errbuf,
		    ವಾದಗಳು->os_errbuflen);
    ಆದರೆ (retval == ಖಾಲಿ)
    {
	redraw_tabline = TRUE;
	ಆದರೆ (tabline_ಎತ್ತರ() > 0)
	    update_screen(UPD_VALID);
#ಆದರೆ defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
	popup_close_info(); // Close info popup to apply new properties
#ಆದರೆ_ಅಂತ್ಯ
    }
    ಹಿಂತಿರುಗಿ retval;
}

    ಪೂರ್ಣಾಂಕ
expand_ಗಣ_mouse(optexpand_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಹಿಂತಿರುಗಿ expand_ಗಣ_opt_listflag(ವಾದಗಳು, (char_u*)MOUSE_ALL, ಅಂಕೆ_Matches, matches);
}

/*
 * The 'mousemodel' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_mousemodel(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಆದರೆ (check_opt_ಅಕ್ಷರಸರಣಿಗಳು(p_mousem, p_mousem_ಮೌಲ್ಯಗಳು, FALSE) != OK)
	ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;
#ಆದರೆ defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU) && (Xm_ಆವೃತ್ತಿ <= 1002)
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p_mousem != *ವಾದಗಳು->os_oldval.string)
	// Changed from "extend" to "popup" or "popup_setpos" or vv: need
	// to create or delete the popup menus.
	gui_motif_update_mousemodel(ಬೇರು_menu);
#ಆದರೆ_ಅಂತ್ಯ

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

    ಪೂರ್ಣಾಂಕ
expand_ಗಣ_mousemodel(optexpand_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಹಿಂತಿರುಗಿ expand_ಗಣ_opt_string(
	    ವಾದಗಳು,
	    p_mousem_ಮೌಲ್ಯಗಳು,
	    ARRAY_ಉದ್ದಳತೆ(p_mousem_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}

#ಆದರೆ defined(FEAT_MOUSESHAPE)
    ಅಕ್ಷರ *
did_ಗಣ_mouseshape(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಅಕ್ಷರ *errmsg = ಖಾಲಿ;

    errmsg = parse_shape_opt(SHAPE_MOUSE);
    update_mouseshape(-1);

    ಹಿಂತಿರುಗಿ errmsg;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * The 'nrformats' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_nrformats(optset_T *ವಾದಗಳು)
{
    char_u	**varp = (char_u **)ವಾದಗಳು->os_varp;

    ಹಿಂತಿರುಗಿ did_ಗಣ_opt_ಅಕ್ಷರಸರಣಿಗಳು(*varp, p_nf_ಮೌಲ್ಯಗಳು, TRUE);
}

    ಪೂರ್ಣಾಂಕ
expand_ಗಣ_nrformats(optexpand_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಹಿಂತಿರುಗಿ expand_ಗಣ_opt_string(
	    ವಾದಗಳು,
	    p_nf_ಮೌಲ್ಯಗಳು,
	    ARRAY_ಉದ್ದಳತೆ(p_nf_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}

#ಆದರೆ defined(FEAT_EVAL)
/*
 * One of the '*expr' options is changed: 'balloonexpr', 'diffexpr',
 * 'foldexpr', 'foldtext', 'formatexpr', 'includeexpr', 'indentexpr',
 * 'patchexpr', 'printexpr' and 'charconvert'.
 */
    ಅಕ್ಷರ *
did_ಗಣ_optexpr(optset_T *ವಾದಗಳು)
{
    char_u	**varp = (char_u **)ವಾದಗಳು->os_varp;

    // If the option value starts with <SID> or s:, then replace that with
    // the script identifier.
    char_u *name = get_scriptlocal_funcname(*varp);
    ಆದರೆ (name != ಖಾಲಿ)
    {
	free_string_option(*varp);
	*varp = name;
    }

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * The 'pastetoggle' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_pastetoggle(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    char_u	*p;

    // translate key codes like in a mapping
    ಆದರೆ (*p_pt)
    {
	(ಶೂನ್ಯ)replace_termcodes(p_pt, &p, 0,
		REPTERM_FROM_PART | REPTERM_DO_LT, ಖಾಲಿ);
	ಆದರೆ (p != ಖಾಲಿ)
	{
	    free_string_option(p_pt);
	    p_pt = p;
	}
    }

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

#ಆದರೆ defined(FEAT_PROP_POPUP)
/*
 * The 'previewpopup' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_previewpopup(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಆದರೆ (parse_previewpopup(ಖಾಲಿ) == FAIL)
	ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;

# ಆದರೆ defined(FEAT_QUICKFIX)
    popup_close_info();
# endif
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
expand_ಗಣ_popupoption(optexpand_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches, char_u ***matches,
	ಪೂರ್ಣಾಂಕ previewpopup)
{
    expand_T *xp = ವಾದಗಳು->oe_xp;

    ಆದರೆ (xp->xp_ನಮೂನೆ > ವಾದಗಳು->oe_ಗಣ_arg && *(xp->xp_ನಮೂನೆ-1) == ':')
    {
	ಆದರೆ (completing_ಮೌಲ್ಯ_for_subopt(ವಾದಗಳು, "close")
		|| completing_ಮೌಲ್ಯ_for_subopt(ವಾದಗಳು, "resize")
		|| completing_ಮೌಲ್ಯ_for_subopt(ವಾದಗಳು, "shadow"))
	{
	    ಹಿಂತಿರುಗಿ expand_ಗಣ_opt_string(
		    ವಾದಗಳು,
		    p_popup_option_on_off_ಮೌಲ್ಯಗಳು,
		    ARRAY_ಉದ್ದಳತೆ(p_popup_option_on_off_ಮೌಲ್ಯಗಳು) - 1,
		    ಅಂಕೆ_Matches,
		    matches);
	}
	ಆದರೆ (completing_ಮೌಲ್ಯ_for_subopt(ವಾದಗಳು, "border"))
	{
	    ಹಿಂತಿರುಗಿ expand_ಗಣ_opt_string(
		    ವಾದಗಳು,
		    previewpopup ? p_popup_pvp_border_ಮೌಲ್ಯಗಳು
			: p_popup_cpp_border_ಮೌಲ್ಯಗಳು,
		    (previewpopup ? ARRAY_ಉದ್ದಳತೆ(p_popup_pvp_border_ಮೌಲ್ಯಗಳು) - 1
			: ARRAY_ಉದ್ದಳತೆ(p_popup_cpp_border_ಮೌಲ್ಯಗಳು)) - 1,
		    ಅಂಕೆ_Matches,
		    matches);
	}
	ಆದರೆ (completing_ಮೌಲ್ಯ_for_subopt(ವಾದಗಳು, "align"))
	{
	    ಹಿಂತಿರುಗಿ expand_ಗಣ_opt_string(
		    ವಾದಗಳು,
		    p_popup_option_align_ಮೌಲ್ಯಗಳು,
		    ARRAY_ಉದ್ದಳತೆ(p_popup_option_align_ಮೌಲ್ಯಗಳು) - 1,
		    ಅಂಕೆ_Matches,
		    matches);
	}
	ಆದರೆ (completing_ಮೌಲ್ಯ_for_subopt(ವಾದಗಳು, "highlight")
		|| completing_ಮೌಲ್ಯ_for_subopt(ವಾದಗಳು, "borderhighlight"))
	{
	    // Return the list of all highlight names
	    ಹಿಂತಿರುಗಿ expand_ಗಣ_opt_generic(
		    ವಾದಗಳು,
		    get_highlight_name,
		    ಅಂಕೆ_Matches,
		    matches);
	}
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಹಿಂತಿರುಗಿ expand_ಗಣ_opt_string(
	    ವಾದಗಳು,
	    previewpopup ? p_popup_pvp_option_ಮೌಲ್ಯಗಳು
		: p_popup_cpp_option_ಮೌಲ್ಯಗಳು,
	    previewpopup ? ARRAY_ಉದ್ದಳತೆ(p_popup_pvp_option_ಮೌಲ್ಯಗಳು) - 1
		: ARRAY_ಉದ್ದಳತೆ(p_popup_cpp_option_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}

    ಪೂರ್ಣಾಂಕ
expand_ಗಣ_previewpopup(optexpand_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಹಿಂತಿರುಗಿ expand_ಗಣ_popupoption(ವಾದಗಳು, ಅಂಕೆ_Matches, matches, TRUE);
}

    ಪೂರ್ಣಾಂಕ
expand_ಗಣ_completepopup(optexpand_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಹಿಂತಿರುಗಿ expand_ಗಣ_popupoption(ವಾದಗಳು, ಅಂಕೆ_Matches, matches, FALSE);
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_POSTSCRIPT)
/*
 * The 'printencoding' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_printencoding(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    char_u	*s, *p;

    // Canonize 'printencoding' if VIM standard one
    p = enc_canonize(p_penc);
    ಆದರೆ (p != ಖಾಲಿ)
    {
	vim_free(p_penc);
	p_penc = p;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Ensure lower case and '-' for '_'
	ಗಾಗಿ (s = p_penc; *s != NUL; s++)
	{
	    ಆದರೆ (*s == '_')
		*s = '-';
	    ಇಲ್ಲದಿದ್ದರೆ
		*s = TOLOWER_ASC(*s);
	}
    }

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_PRINTER)

    ಸ್ಥಿರ char_u *
get_printoptions_ಹೆಸರುಗಳು(expand_T *xp ಬಳಸದ, ಪೂರ್ಣಾಂಕ idx)
{
    ಆದರೆ (idx >= (ಪೂರ್ಣಾಂಕ)ARRAY_ಉದ್ದಳತೆ(printer_opts))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಹಿಂತಿರುಗಿ (char_u*)printer_opts[idx].name;
}

/*
 * Expand 'printoptions' option
 */
    ಪೂರ್ಣಾಂಕ
expand_ಗಣ_printoptions(optexpand_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಹಿಂತಿರುಗಿ expand_ಗಣ_opt_generic(
	    ವಾದಗಳು,
	    get_printoptions_ಹೆಸರುಗಳು,
	    ಅಂಕೆ_Matches,
	    matches);
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_STL_OPT)
/*
 * The 'statusline' or the 'tabline' or the 'rulerformat' option is changed.
 * "rulerformat" is TRUE if the 'rulerformat' option is changed.
 */
    ಸ್ಥಿರ ಅಕ್ಷರ *
parse_statustabline_rulerformat(optset_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ rulerformat)
{
    char_u	**varp = (char_u **)ವಾದಗಳು->os_varp;
    char_u	*s;
    ಅಕ್ಷರ	*errmsg = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		wid;

    ಆದರೆ (rulerformat)	// reset ru_wid first
	ru_wid = 0;
    s = *varp;
    ಆದರೆ (rulerformat && *s == '%')
    {
	// set ru_wid if 'ruf' starts with "%99("
	ಆದರೆ (*++s == '-')	// ignore a '-'
	    s++;
	wid = getdigits(&s);
	ಆದರೆ (wid && *s == '(' && (errmsg = check_stl_option(p_ruf)) == ಖಾಲಿ)
	    ru_wid = wid;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // Validate the flags in 'rulerformat' only if it doesn't point to
	    // a custom function ("%!" flag).
	    ಆದರೆ ((*varp)[1] != '!')
		errmsg = check_stl_option(p_ruf);
	}
    }
    // check 'statusline' or 'tabline' only if it doesn't start with "%!"
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (rulerformat || s[0] != '%' || s[1] != '!')
	errmsg = check_stl_option(s);
    ಆದರೆ (rulerformat && errmsg == ಖಾಲಿ)
	comp_ಲಂಬಸಾಲು();

    ಹಿಂತಿರುಗಿ errmsg;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_RENDER_OPTIONS)
/*
 * The 'renderoptions' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_renderoptions(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಆದರೆ (!gui_mch_ಗಣ_rendering_options(p_rop))
	ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_RIGHTLEFT)
/*
 * The 'rightleftcmd' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_rightleftcmd(optset_T *ವಾದಗಳು)
{
    char_u	**varp = (char_u **)ವಾದಗಳು->os_varp;

    // Currently only "search" is a supported value.
    ಆದರೆ (**varp != NUL && STRCMP(*varp, "search") != 0)
	ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

    ಪೂರ್ಣಾಂಕ
expand_ಗಣ_rightleftcmd(optexpand_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಸ್ಥಿರ ಅಕ್ಷರ *(p_rlc_ಮೌಲ್ಯಗಳು[]) = {"search", ಖಾಲಿ};
    ಹಿಂತಿರುಗಿ expand_ಗಣ_opt_string(
	    ವಾದಗಳು,
	    p_rlc_ಮೌಲ್ಯಗಳು,
	    ARRAY_ಉದ್ದಳತೆ(p_rlc_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು PUM_BORDER_CLEAR()   \
    ಮಾಡು {                     \
	pum_ಗಣ_border(FALSE); \
	pum_ಗಣ_shadow(FALSE); \
	pum_ಗಣ_margin(FALSE); \
    } ಆಗ (0)

/*
 * Parse a border value from a pumopt "border:" token.
 * Returns OK on success, FAIL on error.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
parse_pumopt_border(char_u *val, ಪೂರ್ಣಾಂಕ len)
{
    // Use box-drawing characters only when 'encoding' is "utf-8" and
    // 'ambiwidth' is "single".
    ಪೂರ್ಣಾಂಕ	    can_use_box_chars = (enc_utf8 && *p_ambw == 's');
    char_u  *token;

    token = vim_strnsave(val, len);
    ಆದರೆ (token == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    ಆದರೆ (can_use_box_chars && STRCMP(token, "single") == 0)
	pum_ಗಣ_border_chars(0x2500, 0x2502, 0x2500, 0x2502, // ─ │ ─ │
		0x250c, 0x2510, 0x2518, 0x2514); // ┌ ┐ ┘ └
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (can_use_box_chars && STRCMP(token, "double") == 0)
	pum_ಗಣ_border_chars(0x2550, 0x2551, 0x2550, 0x2551, // ═ ║ ═ ║
		0x2554, 0x2557, 0x255D, 0x255A); // ╔ ╗ ╝  ╚
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (can_use_box_chars && STRCMP(token, "round") == 0)
	pum_ಗಣ_border_chars(0x2500, 0x2502, 0x2500, 0x2502, // ─ │ ─ │
		0x256d, 0x256e, 0x256f, 0x2570); // ╭ ╮ ╯ ╰
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(token, "ascii") == 0)
	pum_ಗಣ_border_chars('-', '|', '-', '|', '+', '+', '+', '+');
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(token, "custom:", 7) == 0)
    {
	char_u	*q = token + 7;
	ಪೂರ್ಣಾಂಕ	ಹೊರಹೋಗು[8];

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < 8; i++)
	{
	    ಆದರೆ (*q == NUL || *q == ',')
	    {
		vim_free(token);
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	    ಹೊರಹೋಗು[i] = mb_ptr2char(q);
	    mb_ptr2char_adv(&q);
	    ಆದರೆ (i < 7)
	    {
		ಆದರೆ (*q != ';')
		{
		    vim_free(token);
		    ಹಿಂತಿರುಗಿ FAIL;
		}
		q++;
	    }
	}
	ಆದರೆ (*q != NUL && *q != ',')
	{
	    vim_free(token);
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	pum_ಗಣ_border_chars(ಹೊರಹೋಗು[0], ಹೊರಹೋಗು[1], ಹೊರಹೋಗು[2], ಹೊರಹೋಗು[3], ಹೊರಹೋಗು[4], ಹೊರಹೋಗು[5],
		ಹೊರಹೋಗು[6], ಹೊರಹೋಗು[7]);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	vim_free(token);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    vim_free(token);
    pum_ಗಣ_border(TRUE);
    ಹಿಂತಿರುಗಿ OK;
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
    ಅಕ್ಷರ *
did_ಗಣ_pumopt(optset_T *ವಾದಗಳು)
{
    char_u  **varp = (char_u **)ವಾದಗಳು->os_varp;
    char_u  *p;
    ಪೂರ್ಣಾಂಕ	    have_border = FALSE;
    ಪೂರ್ಣಾಂಕ	    have_margin = FALSE;

    // Reset to defaults.
    PUM_BORDER_CLEAR();
    p_ph = 0;
    p_pw = 15;
    p_pmw = 0;
    p_po = 100;

    ಆದರೆ (*varp == ಖಾಲಿ || **varp == NUL)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಗಾಗಿ (p = *varp; p != ಖಾಲಿ && *p != NUL; )
    {
	char_u *comma = vim_strchr(p, ',');
	ಪೂರ್ಣಾಂಕ len;

	ಆದರೆ (comma != ಖಾಲಿ)
	    len = (ಪೂರ್ಣಾಂಕ)(comma - p);
	ಇಲ್ಲದಿದ್ದರೆ
	    len = (ಪೂರ್ಣಾಂಕ)STRLEN(p);

	ಆದರೆ (STRNCMP(p, "border:", 7) == 0)
	{
	    ಆದರೆ (have_border)
		ಹೋಗು error;
	    have_border = TRUE;
	    ಆದರೆ (parse_pumopt_border(p + 7, len - 7) == FAIL)
		ಹೋಗು error;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(p, "height:", 7) == 0)
	{
	    ದೀರ್ಘ n = atol((ಅಕ್ಷರ *)p + 7);
	    ಆದರೆ (n < 0)
		ಹೋಗು error;
	    p_ph = n;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(p, "width:", 6) == 0)
	{
	    ದೀರ್ಘ n = atol((ಅಕ್ಷರ *)p + 6);
	    ಆದರೆ (n < 0)
		ಹೋಗು error;
	    p_pw = n;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(p, "maxwidth:", 9) == 0)
	{
	    ದೀರ್ಘ n = atol((ಅಕ್ಷರ *)p + 9);
	    ಆದರೆ (n < 0)
		ಹೋಗು error;
	    p_pmw = n;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(p, "opacity:", 8) == 0)
	{
	    ದೀರ್ಘ n = atol((ಅಕ್ಷರ *)p + 8);
	    ಆದರೆ (n < 0 || n > 100)
		ಹೋಗು error;
	    p_po = n;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (len == 6 && STRNCMP(p, "shadow", 6) == 0)
	    pum_ಗಣ_shadow(TRUE);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (len == 6 && STRNCMP(p, "margin", 6) == 0)
	{
	    have_margin = TRUE;
	    pum_ಗಣ_margin(TRUE);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ಹೋಗು error;

	ಆದರೆ (comma != ಖಾಲಿ)
	    p = comma + 1;
	ಇಲ್ಲದಿದ್ದರೆ
	    ನಿಲ್ಲಿಸು;
    }

    ಆದರೆ (have_margin && !have_border)
	ಹೋಗು error;

    // Invalidate cached background for opacity changes.
    pum_opacity_ಬದಲಾಯಿತು();

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

error:
    PUM_BORDER_CLEAR();
    p_ph = 0;
    p_pw = 15;
    p_pmw = 0;
    p_po = 100;
    ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;
}

    ಸ್ಥಿರ char_u *
get_pum_border_style(expand_T *xp ಬಳಸದ, ಪೂರ್ಣಾಂಕ idx)
{
    ಸ್ಥಿರ ಅಕ್ಷರ *styles[] = {"ascii", "custom:", "single", "double", "round"};
    ಹಿಂತಿರುಗಿ idx < ((enc_utf8 && *p_ambw == 's') ? (ಪೂರ್ಣಾಂಕ)ARRAY_ಉದ್ದಳತೆ(styles) : 2)
	    ? (char_u *)styles[idx] : ಖಾಲಿ;
}

    ಪೂರ್ಣಾಂಕ
expand_ಗಣ_pumopt(optexpand_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    expand_T *xp = ವಾದಗಳು->oe_xp;

    ಆದರೆ (xp->xp_ನಮೂನೆ > ವಾದಗಳು->oe_ಗಣ_arg && *(xp->xp_ನಮೂನೆ-1) == ':')
    {
	ಆದರೆ (completing_ಮೌಲ್ಯ_for_subopt(ವಾದಗಳು, "border"))
	{
	    ಹಿಂತಿರುಗಿ expand_ಗಣ_opt_generic(
		    ವಾದಗಳು, get_pum_border_style, ಅಂಕೆ_Matches, matches);
	}
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಸ್ಥಿರ ಅಕ್ಷರ *(p_pumopt_ಮೌಲ್ಯಗಳು[]) = {"border:", "height:", "width:",
	"maxwidth:", "opacity:", "shadow", "margin", ಖಾಲಿ};
    ಹಿಂತಿರುಗಿ expand_ಗಣ_opt_string(
	    ವಾದಗಳು,
	    p_pumopt_ಮೌಲ್ಯಗಳು,
	    ARRAY_ಉದ್ದಳತೆ(p_pumopt_ಮೌಲ್ಯಗಳು) - 1,
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
    ಅಕ್ಷರ *
did_ಗಣ_pumborder(optset_T *ವಾದಗಳು)
{
    char_u  **varp = (char_u **)ವಾದಗಳು->os_varp;
    char_u  *p;
    ಪೂರ್ಣಾಂಕ	    len;
    ಪೂರ್ಣಾಂಕ	    have_border = FALSE;
    ಪೂರ್ಣಾಂಕ	    have_margin = FALSE;

    PUM_BORDER_CLEAR();

    ಆದರೆ (*varp == ಖಾಲಿ || **varp == NUL)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಗಾಗಿ (p = *varp; p != ಖಾಲಿ && *p != NUL; )
    {
	char_u *comma = vim_strchr(p, ',');
	ಆದರೆ (comma != ಖಾಲಿ)
	    len = (ಪೂರ್ಣಾಂಕ)(comma - p);
	ಇಲ್ಲದಿದ್ದರೆ
	    len = (ಪೂರ್ಣಾಂಕ)STRLEN(p);

	ಆದರೆ (STRNCMP(p, "shadow", len) == 0 && len == 6)
	    pum_ಗಣ_shadow(TRUE);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(p, "margin", len) == 0 && len == 6)
	{
	    have_margin = TRUE;
	    pum_ಗಣ_margin(TRUE);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (have_border)
		ಹೋಗು error;
	    have_border = TRUE;
	    ಆದರೆ (parse_pumopt_border(p, len) == FAIL)
		ಹೋಗು error;
	}

	ಆದರೆ (comma != ಖಾಲಿ)
	    p = comma + 1;
	ಇಲ್ಲದಿದ್ದರೆ
	    ನಿಲ್ಲಿಸು;
    }

    ಆದರೆ (have_margin && !have_border)
	ಹೋಗು error;

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

error:
    PUM_BORDER_CLEAR();
    pum_ಗಣ_border_chars(0, 0, 0, 0, 0, 0, 0, 0);
    ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;
}

    ಸ್ಥಿರ char_u *
get_pumborder_token(expand_T *xp, ಪೂರ್ಣಾಂಕ idx)
{
    ಹಿಂತಿರುಗಿ idx == 0 ? (char_u *)"margin"
	 : idx == 1 ? (char_u *)"shadow"
	 : get_pum_border_style(xp, idx - 2);
}

    ಪೂರ್ಣಾಂಕ
expand_ಗಣ_pumborder(optexpand_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಹಿಂತಿರುಗಿ expand_ಗಣ_opt_generic(
	    ವಾದಗಳು, get_pumborder_token, ಅಂಕೆ_Matches, matches);
}

#ಆದರೆ defined(FEAT_STL_OPT)

/*
 * The 'rulerformat' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_rulerformat(optset_T *ವಾದಗಳು)
{
    ಹಿಂತಿರುಗಿ parse_statustabline_rulerformat(ವಾದಗಳು, TRUE);
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_TABPANEL)
/*
 * Process the new 'tabpanelopt' option value.
 */
    ಅಕ್ಷರ *
did_ಗಣ_tabpanelopt(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಆದರೆ (tabpanelopt_ಬದಲಾಯಿತು() == FAIL)
	ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

    ಪೂರ್ಣಾಂಕ
expand_ಗಣ_tabpanelopt(optexpand_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    expand_T *xp = ವಾದಗಳು->oe_xp;

    ಆದರೆ (xp->xp_ನಮೂನೆ > ವಾದಗಳು->oe_ಗಣ_arg && *(xp->xp_ನಮೂನೆ-1) == ':')
    {
	ಆದರೆ (completing_ಮೌಲ್ಯ_for_subopt(ವಾದಗಳು, "align"))
	{
	    ಹಿಂತಿರುಗಿ expand_ಗಣ_opt_string(
		    ವಾದಗಳು,
		    p_tplo_align_ಮೌಲ್ಯಗಳು,
		    ARRAY_ಉದ್ದಳತೆ(p_tplo_align_ಮೌಲ್ಯಗಳು) - 1,
		    ಅಂಕೆ_Matches,
		    matches);
	}
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಹಿಂತಿರುಗಿ expand_ಗಣ_opt_string(
	    ವಾದಗಳು,
	    p_tplo_ಮೌಲ್ಯಗಳು,
	    ARRAY_ಉದ್ದಳತೆ(p_tplo_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * The 'scrollopt' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_scrollopt(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ did_ಗಣ_opt_ಅಕ್ಷರಸರಣಿಗಳು(p_sbo, p_scbopt_ಮೌಲ್ಯಗಳು, TRUE);
}

    ಪೂರ್ಣಾಂಕ
expand_ಗಣ_scrollopt(optexpand_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಹಿಂತಿರುಗಿ expand_ಗಣ_opt_string(
	    ವಾದಗಳು,
	    p_scbopt_ಮೌಲ್ಯಗಳು,
	    ARRAY_ಉದ್ದಳತೆ(p_scbopt_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}

/*
 * The 'wlseat' option is changed
 */
    ಅಕ್ಷರ *
did_ಗಣ_wlseat(optset_T *ವಾದಗಳು ಬಳಸದ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_WAYLAND_CLIPBOARD
    // If there isn't any seat named 'wlseat', then let the Wayland clipboard be
    // unavailable. Ignore errors returned.
    clip_reset_wayland();
#ಆದರೆ_ಅಂತ್ಯ

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * The 'selection' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_selection(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಆದರೆ (*p_sel == NUL || check_opt_ಅಕ್ಷರಸರಣಿಗಳು(p_sel, p_sel_ಮೌಲ್ಯಗಳು, FALSE) != OK)
	ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

    ಪೂರ್ಣಾಂಕ
expand_ಗಣ_selection(optexpand_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಹಿಂತಿರುಗಿ expand_ಗಣ_opt_string(
	    ವಾದಗಳು,
	    p_sel_ಮೌಲ್ಯಗಳು,
	    ARRAY_ಉದ್ದಳತೆ(p_sel_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}

/*
 * The 'selectmode' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_selectmode(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ did_ಗಣ_opt_ಅಕ್ಷರಸರಣಿಗಳು(p_slm, p_slm_ಮೌಲ್ಯಗಳು, TRUE);
}

    ಪೂರ್ಣಾಂಕ
expand_ಗಣ_selectmode(optexpand_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಹಿಂತಿರುಗಿ expand_ಗಣ_opt_string(
	    ವಾದಗಳು,
	    p_slm_ಮೌಲ್ಯಗಳು,
	    ARRAY_ಉದ್ದಳತೆ(p_slm_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}

#ಆದರೆ defined(FEAT_SESSION)
/*
 * The 'sessionoptions' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_sessionoptions(optset_T *ವಾದಗಳು)
{
    ಆದರೆ (opt_ಅಕ್ಷರಸರಣಿಗಳು_flags(p_ssop, p_ssop_ಮೌಲ್ಯಗಳು, &ssop_flags, TRUE) != OK)
	ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;
    ಆದರೆ ((ssop_flags & SSOP_CURDIR) && (ssop_flags & SSOP_SESDIR))
    {
	// Don't allow both "sesdir" and "curdir".
	(ಶೂನ್ಯ)opt_ಅಕ್ಷರಸರಣಿಗಳು_flags(ವಾದಗಳು->os_oldval.string, p_ssop_ಮೌಲ್ಯಗಳು,
							&ssop_flags, TRUE);
	ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;
    }

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

    ಪೂರ್ಣಾಂಕ
expand_ಗಣ_sessionoptions(
    optexpand_T		*ವಾದಗಳು,
    ಪೂರ್ಣಾಂಕ			*ಅಂಕೆ_Matches,
    char_u		***matches)
{
    ಹಿಂತಿರುಗಿ expand_ಗಣ_opt_string(
	    ವಾದಗಳು,
	    p_ssop_ಮೌಲ್ಯಗಳು,
	    ARRAY_ಉದ್ದಳತೆ(p_ssop_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Validate 'shellpipe'/'shellredir' option.
 */
    ಅಕ್ಷರ *
did_ಗಣ_shellpipe_redir(optset_T *ವಾದಗಳು)
{
    char_u	*p;
    bool	ನೋಡಿದ = false;

    ಗಾಗಿ (p = ವಾದಗಳು->os_newval.string; *p != NUL; ++p)
    {
	ಆದರೆ (*p != '%')
	    ಮುಂದುವರಿ;

	ಆದರೆ (p[1] == NUL)
	    ಹಿಂತಿರುಗಿ e_invalid_format_string_single_percent_s;

	ಆದರೆ (p[1] == '%')
	{
	    ++p;    // skip second %
	    ಮುಂದುವರಿ;
	}

	ಆದರೆ (p[1] == 's')
	{
	    ಆದರೆ (ನೋಡಿದ)
		ಹಿಂತಿರುಗಿ e_invalid_format_string_single_percent_s;

	    ನೋಡಿದ = true;
	    ++p;    // consume 's'
	    ಮುಂದುವರಿ;
	}
	ಹಿಂತಿರುಗಿ e_invalid_format_string_single_percent_s;
    }
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * The 'shortmess' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_shortmess(optset_T *ವಾದಗಳು)
{
    char_u	**varp = (char_u **)ವಾದಗಳು->os_varp;

    ಹಿಂತಿರುಗಿ did_ಗಣ_option_listflag(*varp, (char_u *)SHM_ALL, ವಾದಗಳು->os_errbuf,
		    ವಾದಗಳು->os_errbuflen);
}

    ಪೂರ್ಣಾಂಕ
expand_ಗಣ_shortmess(optexpand_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಹಿಂತಿರುಗಿ expand_ಗಣ_opt_listflag(ವಾದಗಳು, (char_u*)SHM_ALL, ಅಂಕೆ_Matches, matches);
}

#ಆದರೆ defined(FEAT_LINEBREAK)
/*
 * The 'showbreak' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_showbreak(optset_T *ವಾದಗಳು)
{
    char_u	**varp = (char_u **)ವಾದಗಳು->os_varp;
    char_u	*s;

    ಗಾಗಿ (s = *varp; *s; )
    {
	ಆದರೆ (ptr2cells(s) != 1)
	    ಹಿಂತಿರುಗಿ e_showbreak_ಒಳಗೊಂಡಿದೆ_unprintable_or_wide_character;
	MB_PTR_ADV(s);
    }

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * The 'showcmdloc' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_showcmdloc(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಅಕ್ಷರ	*errmsg = did_ಗಣ_opt_ಅಕ್ಷರಸರಣಿಗಳು(p_sloc, p_sloc_ಮೌಲ್ಯಗಳು, FALSE);

    ಆದರೆ (errmsg == ಖಾಲಿ)
	comp_ಲಂಬಸಾಲು();

    ಹಿಂತಿರುಗಿ errmsg;
}

    ಪೂರ್ಣಾಂಕ
expand_ಗಣ_showcmdloc(optexpand_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಹಿಂತಿರುಗಿ expand_ಗಣ_opt_string(
	    ವಾದಗಳು,
	    p_sloc_ಮೌಲ್ಯಗಳು,
	    ARRAY_ಉದ್ದಳತೆ(p_sloc_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}

#ಆದರೆ defined(FEAT_SIGNS)
/*
 * The 'signcolumn' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_signcolumn(optset_T *ವಾದಗಳು)
{
    char_u	**varp = (char_u **)ವಾದಗಳು->os_varp;

# ಆದರೆ defined(FEAT_LINEBREAK)
    ಆದರೆ (check_opt_ಅಕ್ಷರಸರಣಿಗಳು(*varp, p_scl_ಮೌಲ್ಯಗಳು, FALSE) != OK)
	ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;
    // When changing the 'signcolumn' to or from 'number', recompute the
    // width of the number column if 'number' or 'relativenumber' is set.
    ಆದರೆ (((*ವಾದಗಳು->os_oldval.string == 'n' && ವಾದಗಳು->os_oldval.string[1] == 'u')
		|| (*curwin->w_p_scl == 'n' && *(curwin->w_p_scl + 1) =='u'))
	    && (curwin->w_p_nu || curwin->w_p_rnu))
	curwin->w_nrwidth_ಸಾಲು_ಎಣಿಕೆ = 0;
# endif

# ಆದರೆ defined(FEAT_GUI)
    // In the GUI, when sign icons are used, a full screen clear is needed
    // to properly redraw the sign icons.
    ಆದರೆ (gui.ಒಳ_use && curbuf->b_signlist != ಖಾಲಿ)
	redraw_all_later(UPD_CLEAR);
# endif

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

    ಪೂರ್ಣಾಂಕ
expand_ಗಣ_signcolumn(optexpand_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಹಿಂತಿರುಗಿ expand_ಗಣ_opt_string(
	    ವಾದಗಳು,
	    p_scl_ಮೌಲ್ಯಗಳು,
	    ARRAY_ಉದ್ದಳತೆ(p_scl_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_SPELL)
/*
 * The 'spellcapcheck' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_spellcapcheck(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    // compile the regexp program.
    ಹಿಂತಿರುಗಿ compile_cap_prog(curwin->w_s);
}

/*
 * The 'spellfile' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_spellfile(optset_T *ವಾದಗಳು)
{
    char_u	**varp = (char_u **)ವಾದಗಳು->os_varp;

    ಆದರೆ (!valid_spellfile(*varp))
	ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;

    // If there is a window for this buffer in which 'spell' is set load the
    // wordlists.
    ಹಿಂತಿರುಗಿ did_ಗಣ_spell_option();
}

/*
 * The 'spell' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_spelllang(optset_T *ವಾದಗಳು)
{
    char_u	**varp = (char_u **)ವಾದಗಳು->os_varp;

    ಆದರೆ (!valid_spelllang(*varp))
	ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;

    // If there is a window for this buffer in which 'spell' is set load the
    // wordlists.
    ಹಿಂತಿರುಗಿ did_ಗಣ_spell_option();
}

/*
 * The 'spelloptions' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_spelloptions(optset_T *ವಾದಗಳು)
{
    char_u	**varp = (char_u **)ವಾದಗಳು->os_varp;

    ಆದರೆ (**varp != NUL && STRCMP(*varp, "camel") != 0)
	ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

    ಪೂರ್ಣಾಂಕ
expand_ಗಣ_spelloptions(optexpand_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಸ್ಥಿರ ಅಕ್ಷರ *(p_spo_ಮೌಲ್ಯಗಳು[]) = {"camel", ಖಾಲಿ};
    ಹಿಂತಿರುಗಿ expand_ಗಣ_opt_string(
	    ವಾದಗಳು,
	    p_spo_ಮೌಲ್ಯಗಳು,
	    ARRAY_ಉದ್ದಳತೆ(p_spo_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}

/*
 * The 'spellsuggest' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_spellsuggest(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಆದರೆ (spell_check_sps() != OK)
	ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

    ಪೂರ್ಣಾಂಕ
expand_ಗಣ_spellsuggest(optexpand_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಹಿಂತಿರುಗಿ expand_ಗಣ_opt_string(
	    ವಾದಗಳು,
	    p_sps_ಮೌಲ್ಯಗಳು,
	    ARRAY_ಉದ್ದಳತೆ(p_sps_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * The 'splitkeep' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_splitkeep(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    win_T	*wp;
    tabpage_T	*tp;
    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	wp->w_prev_ಎತ್ತರ = wp->w_ಎತ್ತರ;
	wp->w_prev_winrow = wp->w_winrow;
    }
    ಹಿಂತಿರುಗಿ did_ಗಣ_opt_ಅಕ್ಷರಸರಣಿಗಳು(p_spk, p_spk_ಮೌಲ್ಯಗಳು, FALSE);
}

    ಪೂರ್ಣಾಂಕ
expand_ಗಣ_splitkeep(optexpand_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಹಿಂತಿರುಗಿ expand_ಗಣ_opt_string(
	    ವಾದಗಳು,
	    p_spk_ಮೌಲ್ಯಗಳು,
	    ARRAY_ಉದ್ದಳತೆ(p_spk_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}

#ಆದರೆ defined(FEAT_STL_OPT)
/*
 * The 'statusline' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_statusline(optset_T *ವಾದಗಳು)
{
    char_u  **varp = (char_u **)ವಾದಗಳು->os_varp;
    ಅಕ್ಷರ    *ret = parse_statustabline_rulerformat(ವಾದಗಳು, FALSE);

    ಆದರೆ (ret != ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ret;
    update_stl_rendered_ಎತ್ತರ(varp == &curwin->w_p_stl ? curwin : ಖಾಲಿ);
    ಚೌಕಟ್ಟು_change_statusline_ಎತ್ತರ();

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Rewrite the 'statuslineopt' string: replace the last "maxheight:N" with
 * actual_stlh and remove earlier duplicates.  Keyword order is preserved.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
update_stlo_maxheight(char_u **varp, ಪೂರ್ಣಾಂಕ actual_stlh)
{
    ಸ್ಥಿರಾಂಕ char_u    *src = *varp;
    ಸ್ಥಿರಾಂಕ char_u    *ಕೊನೆಯ_mh = ಖಾಲಿ;
    ಸ್ಥಿರಾಂಕ char_u    *p;

    // Find the last "maxheight:" token.
    p = src;
    ಆಗ (*p != NUL)
    {
	ಆದರೆ (STRNCMP(p, "maxheight:", 10) == 0)
	    ಕೊನೆಯ_mh = p;
	ಆಗ (*p != ',' && *p != NUL)
	    ++p;
	ಆದರೆ (*p == ',')
	    ++p;
    }

    ಆದರೆ (ಕೊನೆಯ_mh == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    size_t	bufsize = STRLEN(src) + NUMBUFLEN + 1;
    char_u	*buf = alloc(bufsize);
    ಆದರೆ (buf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಪೂರ್ಣಾಂಕ		len = 0;
    bool	need_comma = false;

    p = src;
    ಆಗ (*p != NUL)
    {
	ಸ್ಥಿರಾಂಕ	char_u	*tok = p;
	ಪೂರ್ಣಾಂಕ	tok_len;

	ಆಗ (*p != ',' && *p != NUL)
	    ++p;
	tok_len = (ಪೂರ್ಣಾಂಕ)(p - tok);
	ಆದರೆ (*p == ',')
	    ++p;

	ಆದರೆ (STRNCMP(tok, "maxheight:", 10) == 0)
	{
	    ಆದರೆ (tok == ಕೊನೆಯ_mh)
	    {
		// Replace the last occurrence with the actual value.
		ಆದರೆ (need_comma)
		    buf[len++] = ',';
		len += vim_snprintf((ಅಕ್ಷರ *)buf + len,
			bufsize - len, "maxheight:%d", actual_stlh);
		need_comma = true;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (need_comma)
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
    ಅಕ್ಷರ *
did_ಗಣ_statuslineopt(optset_T *ವಾದಗಳು)
{
    char_u	**varp = (char_u **)ವಾದಗಳು->os_varp;
    win_T       *wp = varp == &curwin->w_p_stlo ? curwin : ಖಾಲಿ;

    ಆದರೆ (statuslineopt_ಬದಲಾಯಿತು(*varp, wp) == FAIL)
	ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;

    // Sync stl_rendered_height with the current statusline.
    update_stl_rendered_ಎತ್ತರ(wp);

    // Update the maxheight value to the actual value set.
    // Note: Must be changed if p_stlo_values are changed.
    ಆದರೆ (*varp != ಖಾಲಿಗಳು_option)
    {
	ಪೂರ್ಣಾಂಕ actual_stlh;

	ಆದರೆ (wp != ಖಾಲಿ)
	{
	    ಚೌಕಟ್ಟು_change_statusline_ಎತ್ತರ();
	    actual_stlh = MIN(wp->w_p_stlo_mh,
		    wp->w_ಎತ್ತರ + wp->w_ಸ್ಥಿತಿ_ಎತ್ತರ - p_wmh);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    actual_stlh = ಚೌಕಟ್ಟು_change_statusline_ಎತ್ತರ();

	// Only re-serialize when the window actually has a status line shown.
	ಆದರೆ (actual_stlh > 0)
	{
	    update_stlo_maxheight(varp, actual_stlh);
	    // Update the parsed maxheight member directly.
	    ಆದರೆ (wp != ಖಾಲಿ)
		wp->w_p_stlo_mh = actual_stlh;
	    ಇಲ್ಲದಿದ್ದರೆ
		ಗಣ_stlo_mh(actual_stlh);
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
	ಚೌಕಟ್ಟು_change_statusline_ಎತ್ತರ();

    ಆದರೆ ((ವಾದಗಳು->os_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	// :set clears the local values.
	clear_string_option(&curwin->w_p_stlo);
	curwin->w_p_stlo_fh = -1;
	curwin->w_p_stlo_mh = -1;
    }

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

    ಪೂರ್ಣಾಂಕ
expand_ಗಣ_statuslineopt(optexpand_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಹಿಂತಿರುಗಿ expand_ಗಣ_opt_string(
	    ವಾದಗಳು,
	    p_stlo_ಮೌಲ್ಯಗಳು,
	    ARRAY_ಉದ್ದಳತೆ(p_stlo_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * The 'swapsync' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_swapsync(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ did_ಗಣ_opt_ಅಕ್ಷರಸರಣಿಗಳು(p_sws, p_sws_ಮೌಲ್ಯಗಳು, FALSE);
}

    ಪೂರ್ಣಾಂಕ
expand_ಗಣ_swapsync(optexpand_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಹಿಂತಿರುಗಿ expand_ಗಣ_opt_string(
	    ವಾದಗಳು,
	    p_sws_ಮೌಲ್ಯಗಳು,
	    ARRAY_ಉದ್ದಳತೆ(p_sws_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}

/*
 * The 'switchbuf' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_switchbuf(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ did_ಗಣ_opt_flags(p_swb, p_swb_ಮೌಲ್ಯಗಳು, &swb_flags, TRUE);
}

    ಪೂರ್ಣಾಂಕ
expand_ಗಣ_switchbuf(optexpand_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಹಿಂತಿರುಗಿ expand_ಗಣ_opt_string(
	    ವಾದಗಳು,
	    p_swb_ಮೌಲ್ಯಗಳು,
	    ARRAY_ಉದ್ದಳತೆ(p_swb_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}

/*
 * The 'tabclose' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_tabclose(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ did_ಗಣ_opt_flags(p_tcl, p_tcl_ಮೌಲ್ಯಗಳು, &tcl_flags, TRUE);
}

    ಪೂರ್ಣಾಂಕ
expand_ಗಣ_tabclose(optexpand_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಹಿಂತಿರುಗಿ expand_ಗಣ_opt_string(
	    ವಾದಗಳು,
	    p_tcl_ಮೌಲ್ಯಗಳು,
	    ARRAY_ಉದ್ದಳತೆ(p_tcl_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}

#ಆದರೆ defined(FEAT_STL_OPT)
/*
 * The 'tabline' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_tabline(optset_T *ವಾದಗಳು)
{
    ಹಿಂತಿರುಗಿ parse_statustabline_rulerformat(ವಾದಗಳು, FALSE);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * The 'tagcase' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_tagcase(optset_T *ವಾದಗಳು)
{
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ	*flags;
    char_u		*p;

    ಆದರೆ (ವಾದಗಳು->os_flags & OPT_LOCAL)
    {
	p = curbuf->b_p_tc;
	flags = &curbuf->b_tc_flags;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	p = p_tc;
	flags = &tc_flags;
    }

    ಆದರೆ ((ವಾದಗಳು->os_flags & OPT_LOCAL) && *p == NUL)
	// make the local value empty: use the global value
	*flags = 0;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p == NUL
	    || opt_ಅಕ್ಷರಸರಣಿಗಳು_flags(p, p_tc_ಮೌಲ್ಯಗಳು, flags, FALSE) != OK)
	ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

    ಪೂರ್ಣಾಂಕ
expand_ಗಣ_tagcase(optexpand_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಹಿಂತಿರುಗಿ expand_ಗಣ_opt_string(
	    ವಾದಗಳು,
	    p_tc_ಮೌಲ್ಯಗಳು,
	    ARRAY_ಉದ್ದಳತೆ(p_tc_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}

/*
 * The 'term' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_ಪದ(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಆದರೆ (T_NAME[0] == NUL)
	ಹಿಂತಿರುಗಿ e_cannot_ಗಣ_ಪದ_ಗಮ್ಯ_ಖಾಲಿಗಳು_string;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    ಆದರೆ (gui.ಒಳ_use)
	ಹಿಂತಿರುಗಿ e_cannot_change_ಪದ_ಒಳ_GUI;
    ಆದರೆ (ಪದ_is_gui(T_NAME))
	ಹಿಂತಿರುಗಿ e_use_gui_ಗಮ್ಯ_ಆರಂಭ_GUI;
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (ಗಣ_termname(T_NAME) == FAIL)
	ಹಿಂತಿರುಗಿ e_not_ಸಿಕ್ಕಿತು_ಒಳ_termcap;

    // Screen colors may have changed.
    redraw_later_clear();

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Some terminal option (t_xxx) is changed
 */
    ಅಕ್ಷರ *
did_ಗಣ_ಪದ_option(optset_T *ವಾದಗಳು)
{
    char_u	**varp = (char_u **)ವಾದಗಳು->os_varp;

    ಆದರೆ (!ತುಂಬಿದ_screen)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    // ":set t_Co=0" and ":set t_Co=1" do ":set t_Co="
    ಆದರೆ (varp == &T_CCO)
    {
	ಪೂರ್ಣಾಂಕ colors = atoi((ಅಕ್ಷರ *)T_CCO);

	// Only reinitialize colors if t_Co value has really changed to
	// avoid expensive reload of colorscheme if t_Co is set to the
	// same value multiple times.
	ಆದರೆ (colors != t_colors)
	{
	    t_colors = colors;
	    ಆದರೆ (t_colors <= 1)
	    {
		vim_free(T_CCO);
		T_CCO = ಖಾಲಿಗಳು_option;
	    }
#ಆದರೆ defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
	    ಆದರೆ (is_ಪದ_win32())
	    {
		ಅದಲುಬದಲು_tcap();
		ವಾದಗಳು->os_did_swaptcap = TRUE;
	    }
#ಆದರೆ_ಅಂತ್ಯ
	    // We now have a different color setup, initialize it again.
	    init_highlight(TRUE, FALSE);
	}
    }
    ttest(FALSE);
    ಆದರೆ (varp == &T_ME)
    {
	ಹೊರಹೋಗು_str(T_ME);
	redraw_later(UPD_CLEAR);
#ಆದರೆ defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
	// Since t_me has been set, this probably means that the user
	// wants to use this as default colors.  Need to reset default
	// background/foreground colors.
# ifdef VIMDLL
	ಆದರೆ (!gui.ಒಳ_use && !gui.starting)
# endif
	    mch_ಗಣ_ಸಾಮಾನ್ಯ_colors();
#ಆದರೆ_ಅಂತ್ಯ
    }
    ಆದರೆ (varp == &T_BE && termcap_ಸಕ್ರಿಯ)
    {
	MAY_WANT_ಗಮ್ಯ_LOG_THIS;

	ಆದರೆ (*T_BE == NUL)
	    // When clearing t_BE we assume the user no longer wants
	    // bracketed paste, thus disable it by writing t_BD.
	    ಹೊರಹೋಗು_str(T_BD);
	ಇಲ್ಲದಿದ್ದರೆ
	    ಹೊರಹೋಗು_str(T_BE);
    }

    ಆದರೆ (varp == &T_BSU || varp == &T_ESU)
	ಪದ_ಗಣ_sync_output(ಪದ_SYNC_OUTPUT_OFF);

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}


#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
/*
 * The 'termresize' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_termresize(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    // If empty or "inband", then attempt to enable in-band resize events.
    ಆದರೆ (*p_trz == NUL || STRCMP(p_trz, "inband") == 0)
	ಪದ_ಗಣ_win_resize(true);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(p_trz, "sigwinch") == 0)
	ಪದ_ಗಣ_win_resize(false);
    ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

    ಪೂರ್ಣಾಂಕ
expand_ಗಣ_termresize(optexpand_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಹಿಂತಿರುಗಿ expand_ಗಣ_opt_string(
	    ವಾದಗಳು,
	    p_trz_ಮೌಲ್ಯಗಳು,
	    ARRAY_ಉದ್ದಳತೆ(p_trz_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_TERMINAL)
/*
 * The 'termwinkey' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_termwinkey(optset_T *ವಾದಗಳು)
{
    char_u	**varp = (char_u **)ವಾದಗಳು->os_varp;

    ಆದರೆ ((*varp)[0] != NUL && string_ಗಮ್ಯ_ಕೀಲಿ(*varp, TRUE) == 0)
	ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * The 'termwinsize' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_termwinsize(optset_T *ವಾದಗಳು)
{
    char_u	**varp = (char_u **)ವಾದಗಳು->os_varp;
    char_u	*p;

    ಆದರೆ ((*varp)[0] == NUL)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    p = skipdigits(*varp);
    ಆದರೆ (p == *varp || (*p != 'x' && *p != '*') || *skipdigits(p + 1) != NUL)
	ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

# ಆದರೆ defined(MSWIN)
/*
 * The 'termwintype' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_termwintype(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ did_ಗಣ_opt_ಅಕ್ಷರಸರಣಿಗಳು(p_twt, p_twt_ಮೌಲ್ಯಗಳು, FALSE);
}

    ಪೂರ್ಣಾಂಕ
expand_ಗಣ_termwintype(optexpand_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಹಿಂತಿರುಗಿ expand_ಗಣ_opt_string(
	    ವಾದಗಳು,
	    p_twt_ಮೌಲ್ಯಗಳು,
	    ARRAY_ಉದ್ದಳತೆ(p_twt_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}
# endif
#ಆದರೆ_ಅಂತ್ಯ

/*
 * The 'titlestring' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_titlestring(optset_T *ವಾದಗಳು)
{
    ಪೂರ್ಣಾಂಕ flagval = 0;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_STL_OPT
    flagval = STL_ಒಳ_TITLE;
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ parse_titleiconstring(ವಾದಗಳು, flagval);
}

#ಆದರೆ defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
/*
 * The 'toolbar' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_toolbar(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಆದರೆ (opt_ಅಕ್ಷರಸರಣಿಗಳು_flags(p_toolbar, p_toolbar_ಮೌಲ್ಯಗಳು, &toolbar_flags,
								TRUE) != OK)
	ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;

    ಹೊರಹೋಗು_flush();
    gui_mch_show_toolbar((toolbar_flags &
		(TOOLBAR_ಪಠ್ಯ | TOOLBAR_ICONS)) != 0);
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

    ಪೂರ್ಣಾಂಕ
expand_ಗಣ_toolbar(optexpand_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಹಿಂತಿರುಗಿ expand_ಗಣ_opt_string(
	    ವಾದಗಳು,
	    p_toolbar_ಮೌಲ್ಯಗಳು,
	    ARRAY_ಉದ್ದಳತೆ(p_toolbar_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_TOOLBAR) && defined(FEAT_GUI_GTK)
/*
 * The 'toolbariconsize' option is changed.  GTK+ 2 only.
 */
    ಅಕ್ಷರ *
did_ಗಣ_toolbariconsize(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಆದರೆ (opt_ಅಕ್ಷರಸರಣಿಗಳು_flags(p_tbis, p_tbis_ಮೌಲ್ಯಗಳು, &tbis_flags, FALSE) != OK)
	ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;

    ಹೊರಹೋಗು_flush();
    gui_mch_show_toolbar((toolbar_flags &
		(TOOLBAR_ಪಠ್ಯ | TOOLBAR_ICONS)) != 0);
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

    ಪೂರ್ಣಾಂಕ
expand_ಗಣ_toolbariconsize(
    optexpand_T	*ವಾದಗಳು,
    ಪೂರ್ಣಾಂಕ		*ಅಂಕೆ_Matches,
    char_u	***matches)
{
    ಹಿಂತಿರುಗಿ expand_ಗಣ_opt_string(
	    ವಾದಗಳು,
	    p_tbis_ಮೌಲ್ಯಗಳು,
	    ARRAY_ಉದ್ದಳತೆ(p_tbis_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(UNIX) || defined(VMS)
/*
 * The 'ttymouse' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_ttymouse(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಅಕ್ಷರ *errmsg = ಖಾಲಿ;

    // Switch the mouse off before changing the escape sequences used for
    // that.
    mch_setmouse(FALSE);
    ಆದರೆ (opt_ಅಕ್ಷರಸರಣಿಗಳು_flags(p_ttym, p_ttym_ಮೌಲ್ಯಗಳು, &ttym_flags, FALSE) != OK)
	errmsg = e_invalid_ವಾದ;
    ಇಲ್ಲದಿದ್ದರೆ
	check_mouse_termcode();
    ಆದರೆ (termcap_ಸಕ್ರಿಯ)
	setmouse();		// may switch it on again

    ಹಿಂತಿರುಗಿ errmsg;
}

    ಪೂರ್ಣಾಂಕ
expand_ಗಣ_ttymouse(optexpand_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಹಿಂತಿರುಗಿ expand_ಗಣ_opt_string(
	    ವಾದಗಳು,
	    p_ttym_ಮೌಲ್ಯಗಳು,
	    ARRAY_ಉದ್ದಳತೆ(p_ttym_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_VARTABS)
/*
 * The 'varsofttabstop' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_varsofttabstop(optset_T *ವಾದಗಳು)
{
    char_u	**varp = (char_u **)ವಾದಗಳು->os_varp;
    char_u *cp;

    ಆದರೆ (!((*varp)[0]) || ((*varp)[0] == '0' && !((*varp)[1])))
	VIM_CLEAR(curbuf->b_p_vsts_array);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಗಾಗಿ (cp = *varp; *cp; ++cp)
	{
	    ಆದರೆ (vim_isdigit(*cp))
		ಮುಂದುವರಿ;
	    ಆದರೆ (*cp == ',' && cp > *varp && *(cp-1) != ',')
		ಮುಂದುವರಿ;
	    ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;
	}

	ಪೂರ್ಣಾಂಕ *oldarray = curbuf->b_p_vsts_array;
	ಆದರೆ (tabstop_ಗಣ(*varp, &(curbuf->b_p_vsts_array)) == OK)
	{
	    ಆದರೆ (oldarray)
		vim_free(oldarray);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;
    }

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * The 'vartabstop' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_vartabstop(optset_T *ವಾದಗಳು)
{
    char_u	**varp = (char_u **)ವಾದಗಳು->os_varp;
    char_u *cp;

    ಆದರೆ (!((*varp)[0]) || ((*varp)[0] == '0' && !((*varp)[1])))
	VIM_CLEAR(curbuf->b_p_vts_array);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಗಾಗಿ (cp = *varp; *cp; ++cp)
	{
	    ಆದರೆ (vim_isdigit(*cp))
		ಮುಂದುವರಿ;
	    ಆದರೆ (*cp == ',' && cp > *varp && *(cp-1) != ',')
		ಮುಂದುವರಿ;
	    ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;
	}

	ಪೂರ್ಣಾಂಕ *oldarray = curbuf->b_p_vts_array;

	ಆದರೆ (tabstop_ಗಣ(*varp, &(curbuf->b_p_vts_array)) == OK)
	{
	    vim_free(oldarray);
# ifdef FEAT_FOLDING
	    ಆದರೆ (foldmethodIsIndent(curwin))
		foldUpdateAll(curwin);
# endif
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;
    }

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * The 'verbosefile' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_verbosefile(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    verbose_stop();
    ಆದರೆ (*p_vfile != NUL && verbose_open() == FAIL)
	ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

#ಆದರೆ defined(FEAT_SESSION)
/*
 * The 'viewoptions' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_viewoptions(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ did_ಗಣ_opt_flags(p_vop, p_ssop_ಮೌಲ್ಯಗಳು, &vop_flags, TRUE);
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_VIMINFO)
/*
 * The 'viminfo' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_viminfo(optset_T *ವಾದಗಳು)
{
    char_u	*s;
    ಅಕ್ಷರ	*errmsg = ಖಾಲಿ;

    ಗಾಗಿ (s = p_viminfo; *s;)
    {
	// Check it's a valid character
	ಆದರೆ (vim_strchr((char_u *)"!\"%'/:<@cfhnrs", *s) == ಖಾಲಿ)
	{
	    errmsg = illegal_char(ವಾದಗಳು->os_errbuf, ವಾದಗಳು->os_errbuflen, *s);
	    ನಿಲ್ಲಿಸು;
	}
	ಆದರೆ (*s == 'n')	// name is always last one
	    ನಿಲ್ಲಿಸು;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*s == 'r') // skip until next ','
	{
	    ಆಗ (*++s && *s != ',')
		;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*s == '%')
	{
	    // optional number
	    ಆಗ (vim_isdigit(*++s))
		;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*s == '!' || *s == 'h' || *s == 'c')
	    ++s;		// no extra chars
	ಇಲ್ಲದಿದ್ದರೆ		// must have a number
	{
	    ಆಗ (vim_isdigit(*++s))
		;

	    ಆದರೆ (!VIM_ISDIGIT(*(s - 1)))
	    {
		ಆದರೆ (ವಾದಗಳು->os_errbuf != ಖಾಲಿ)
		{
		    vim_snprintf(ವಾದಗಳು->os_errbuf, ವಾದಗಳು->os_errbuflen,
			    _(e_missing_ಸಂಖ್ಯೆ_after_angle_str_angle),
			    transchar_byte(*(s - 1)));
		    errmsg = ವಾದಗಳು->os_errbuf;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    errmsg = "";
		ನಿಲ್ಲಿಸು;
	    }
	}
	ಆದರೆ (*s == ',')
	    ++s;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*s)
	{
	    ಆದರೆ (ವಾದಗಳು->os_errbuf != ಖಾಲಿ)
		errmsg = e_missing_comma;
	    ಇಲ್ಲದಿದ್ದರೆ
		errmsg = "";
	    ನಿಲ್ಲಿಸು;
	}
    }
    ಆದರೆ (*p_viminfo && errmsg == ಖಾಲಿ && get_viminfo_parameter('\'') < 0)
	errmsg = e_must_specify_a_ಮೌಲ್ಯ;

    ಹಿಂತಿರುಗಿ errmsg;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * The 'virtualedit' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_virtualedit(optset_T *ವಾದಗಳು)
{
    char_u		*ve = p_ve;
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ	*flags = &ve_flags;

    ಆದರೆ (ವಾದಗಳು->os_flags & OPT_LOCAL)
    {
	ve = curwin->w_p_ve;
	flags = &curwin->w_ve_flags;
    }

    ಆದರೆ ((ವಾದಗಳು->os_flags & OPT_LOCAL) && *ve == NUL)
	// make the local value empty: use the global value
	*flags = 0;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (opt_ಅಕ್ಷರಸರಣಿಗಳು_flags(ve, p_ve_ಮೌಲ್ಯಗಳು, flags, TRUE) != OK)
	    ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(ve, ವಾದಗಳು->os_oldval.string) != 0)
	{
	    // Recompute cursor position in case the new 've' setting
	    // changes something.
	    validate_virtcol();
	    coladvance(curwin->w_virtcol);
	}
    }

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

    ಪೂರ್ಣಾಂಕ
expand_ಗಣ_virtualedit(optexpand_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಹಿಂತಿರುಗಿ expand_ಗಣ_opt_string(
	    ವಾದಗಳು,
	    p_ve_ಮೌಲ್ಯಗಳು,
	    ARRAY_ಉದ್ದಳತೆ(p_ve_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}

/*
 * The 'whichwrap' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_whichwrap(optset_T *ವಾದಗಳು)
{
    char_u	**varp = (char_u **)ವಾದಗಳು->os_varp;

    // Add ',' to the list flags because 'whichwrap' is a flag
    // list that is comma-separated.
    ಹಿಂತಿರುಗಿ did_ಗಣ_option_listflag(*varp, (char_u *)(WW_ALL ","),
		    ವಾದಗಳು->os_errbuf, ವಾದಗಳು->os_errbuflen);
}

    ಪೂರ್ಣಾಂಕ
expand_ಗಣ_whichwrap(optexpand_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಹಿಂತಿರುಗಿ expand_ಗಣ_opt_listflag(ವಾದಗಳು, (char_u*)WW_ALL, ಅಂಕೆ_Matches, matches);
}

/*
 * The 'wildmode' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_wildmode(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಆದರೆ (check_opt_wim() == FAIL)
	ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

    ಪೂರ್ಣಾಂಕ
expand_ಗಣ_wildmode(optexpand_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಹಿಂತಿರುಗಿ expand_ಗಣ_opt_string(
	    ವಾದಗಳು,
	    p_wim_ಮೌಲ್ಯಗಳು,
	    ARRAY_ಉದ್ದಳತೆ(p_wim_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}

/*
 * The 'wildoptions' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_wildoptions(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ did_ಗಣ_opt_ಅಕ್ಷರಸರಣಿಗಳು(p_wop, p_wop_ಮೌಲ್ಯಗಳು, TRUE);
}

    ಪೂರ್ಣಾಂಕ
expand_ಗಣ_wildoptions(optexpand_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಹಿಂತಿರುಗಿ expand_ಗಣ_opt_string(
	    ವಾದಗಳು,
	    p_wop_ಮೌಲ್ಯಗಳು,
	    ARRAY_ಉದ್ದಳತೆ(p_wop_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}

#ಆದರೆ defined(FEAT_WAK)
/*
 * The 'winaltkeys' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_winaltkeys(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಅಕ್ಷರ *errmsg = ಖಾಲಿ;

    ಆದರೆ (*p_wak == NUL || check_opt_ಅಕ್ಷರಸರಣಿಗಳು(p_wak, p_wak_ಮೌಲ್ಯಗಳು, FALSE) != OK)
	errmsg = e_invalid_ವಾದ;
# ifdef FEAT_MENU
#  ಆದರೆ defined(FEAT_GUI_MOTIF)
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (gui.ಒಳ_use)
	gui_motif_ಗಣ_mnemonics(p_wak[0] == 'y' || p_wak[0] == 'm');
#  elif defined(FEAT_GUI_GTK)
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (gui.ಒಳ_use)
	gui_gtk_ಗಣ_mnemonics(p_wak[0] == 'y' || p_wak[0] == 'm');
#  endif
# endif
    ಹಿಂತಿರುಗಿ errmsg;
}

    ಪೂರ್ಣಾಂಕ
expand_ಗಣ_winaltkeys(optexpand_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಹಿಂತಿರುಗಿ expand_ಗಣ_opt_string(
	    ವಾದಗಳು,
	    p_wak_ಮೌಲ್ಯಗಳು,
	    ARRAY_ಉದ್ದಳತೆ(p_wak_ಮೌಲ್ಯಗಳು) - 1,
	    ಅಂಕೆ_Matches,
	    matches);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * The 'wincolor' option is changed.
 */
    ಅಕ್ಷರ *
did_ಗಣ_wincolor(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    update_wincolor(curwin, ವಾದಗಳು->os_newval.string);
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * The 'winhighlight' option is changed
 */
    ಅಕ್ಷರ *
did_ಗಣ_winhighlight(optset_T *ವಾದಗಳು)
{
    ಹಿಂತಿರುಗಿ update_winhighlight(curwin, ವಾದಗಳು->os_newval.string);
}

/*
 * Expand 'winhighlight' option.
 */
    ಪೂರ್ಣಾಂಕ
expand_ಗಣ_winhighlight(optexpand_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    expand_T	    *xp = ವಾದಗಳು->oe_xp;

    ಆದರೆ ((xp->xp_ನಮೂನೆ > ವಾದಗಳು->oe_ಗಣ_arg && *(xp->xp_ನಮೂನೆ-1) == ':')
	    || xp->xp_ನಮೂನೆ == ವಾದಗಳು->oe_ಗಣ_arg || *(xp->xp_ನಮೂನೆ-1) == ',')
    {
	// After a ':' or after a ',', or at the start, so expand highlight
	// group name.

	// If starts with !, then expand 'highlight' occasions.
	ಆದರೆ (*xp->xp_ನಮೂನೆ == '!')
	    ಹಿಂತಿರುಗಿ expand_hl_occasions(ವಾದಗಳು, ಅಂಕೆ_Matches, matches, '!');
	ಇಲ್ಲದಿದ್ದರೆ
	    ಹಿಂತಿರುಗಿ expand_ಗಣ_opt_generic(
		    ವಾದಗಳು,
		    get_highlight_name,
		    ಅಂಕೆ_Matches,
		    matches);
    }

    VIM_CLEAR(*matches);
    ಹಿಂತಿರುಗಿ FAIL;
}

    ಪೂರ್ಣಾಂಕ
expand_ಗಣ_wincolor(optexpand_T *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    ಹಿಂತಿರುಗಿ expand_ಗಣ_opt_generic(
	    ವಾದಗಳು,
	    get_highlight_name,
	    ಅಂಕೆ_Matches,
	    matches);
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
/*
 * When the 'syntax' option is set, load the syntax of that name.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
do_syntax_autocmd(ಪೂರ್ಣಾಂಕ ಮೌಲ್ಯ_ಬದಲಾಯಿತು)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ syn_recursive = 0;

    ++syn_recursive;
    // Only pass TRUE for "force" when the value changed or not used
    // recursively, to avoid endless recurrence.
    apply_autocmds(EVENT_SYNTAX, curbuf->b_p_syn, curbuf->b_fname,
	    ಮೌಲ್ಯ_ಬದಲಾಯಿತು || syn_recursive == 1, curbuf);
    curbuf->b_flags |= BF_SYN_ಗಣ;
    --syn_recursive;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * When the 'filetype' option is set, trigger the FileType autocommand.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
do_filetype_autocmd(char_u **varp, ಪೂರ್ಣಾಂಕ opt_flags, ಪೂರ್ಣಾಂಕ ಮೌಲ್ಯ_ಬದಲಾಯಿತು)
{
    // Skip this when called from a modeline and the filetype was already set
    // to this value.
    ಆದರೆ ((opt_flags & OPT_MODELINE) && !ಮೌಲ್ಯ_ಬದಲಾಯಿತು)
	ಹಿಂತಿರುಗಿ;

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ  ft_recursive = 0;
    ಪೂರ್ಣಾಂಕ	    secure_save = secure;

    // Reset the secure flag, since the value of 'filetype' has
    // been checked to be safe.
    secure = 0;

    ++ft_recursive;
    curbuf->b_did_filetype = true;
    // Only pass TRUE for "force" when the value changed or not
    // used recursively, to avoid endless recurrence.
    apply_autocmds(EVENT_FILETYPE, curbuf->b_p_ft, curbuf->b_fname,
	    ಮೌಲ್ಯ_ಬದಲಾಯಿತು || ft_recursive == 1, curbuf);
    --ft_recursive;
    // Just in case the old "curbuf" is now invalid.
    ಆದರೆ (varp != &(curbuf->b_p_ft))
	varp = ಖಾಲಿ;

    secure = secure_save;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SPELL
/*
 * When the 'spelllang' option is set, source the spell/LANG.vim file in
 * 'runtimepath'.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
do_spelllang_ಮೂಲ(ಶೂನ್ಯ)
{
    char_u	fname[200];
    char_u	*p;
    char_u	*q = curwin->w_s->b_p_spl;

    // Skip the first name if it is "cjk".
    ಆದರೆ (STRNCMP(q, "cjk,", 4) == 0)
	q += 4;

    // They could set 'spellcapcheck' depending on the language.  Use the first
    // name in 'spelllang' up to '_region' or '.encoding'.
    ಗಾಗಿ (p = q; *p != NUL; ++p)
	ಆದರೆ (!ASCII_ISALNUM(*p) && *p != '-')
	    ನಿಲ್ಲಿಸು;
    ಆದರೆ (p > q)
    {
	vim_snprintf((ಅಕ್ಷರ *)fname, 200, "spell/%.*s.vim",
		(ಪೂರ್ಣಾಂಕ)(p - q), q);
	ಮೂಲ_runtime(fname, DIP_ALL);
    }
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Handle string options that need some action to perform when changed.
 * The new value must be allocated.
 * Returns NULL for success, or an untranslated error message for an error.
 */
    ಅಕ್ಷರ *
did_ಗಣ_string_option(
    ಪೂರ್ಣಾಂಕ		opt_idx,		// index in options[] table
    char_u	**varp,			// pointer to the option variable
    char_u	*oldval,		// previous value of the option
    char_u	*ಮೌಲ್ಯ,			// new value of the option
    ಅಕ್ಷರ	*errbuf,		// buffer for errors, or NULL
    size_t	errbuflen,		// length of error buffer
    ಪೂರ್ಣಾಂಕ		opt_flags,		// OPT_LOCAL and/or OPT_GLOBAL
    ಗಣ_op_T    op,			// OP_ADDING/OP_PREPENDING/OP_REMOVING
    ಪೂರ್ಣಾಂಕ		*ಮೌಲ್ಯ_checked)		// value was checked to be safe, no
					// need to set P_INSECURE
{
    ಅಕ್ಷರ	*errmsg = ಖಾಲಿ;
    long_u	free_oldval = (get_option_flags(opt_idx) & P_ALLOCED);
    opt_did_ಗಣ_cb_T did_ಗಣ_cb = get_option_did_ಗಣ_cb(opt_idx);
    optset_T	ವಾದಗಳು;

    // 'ttytype' is an alias for 'term'.  Both 'term' and 'ttytype' point to
    // T_NAME.  If 'term' or 'ttytype' is modified, then use the index for the
    // 'term' option.  Only set the P_ALLOCED flag on 'term'.
    ಆದರೆ (varp == &T_NAME)
    {
	opt_idx = findoption((char_u *)"term");
	ಆದರೆ (opt_idx >= 0)
	{
	    free_oldval = (get_option_flags(opt_idx) & P_ALLOCED);
	    did_ಗಣ_cb = get_option_did_ಗಣ_cb(opt_idx);
	}
    }

    CLEAR_ಕ್ಷೇತ್ರ(ವಾದಗಳು);

    // Disallow changing some options from secure mode
    ಆದರೆ ((secure
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SANDBOX
		|| sandbox != 0
#ಆದರೆ_ಅಂತ್ಯ
		) && (get_option_flags(opt_idx) & P_SECURE))
	errmsg = e_not_allowed_ಇಲ್ಲಿ;
    // Check for a "normal" directory or file name in some options.
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (check_illegal_ಮಾರ್ಗ_ಹೆಸರುಗಳು(opt_idx, varp))
	errmsg = e_invalid_ವಾದ;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (did_ಗಣ_cb != ಖಾಲಿ)
    {
	ವಾದಗಳು.os_varp = (char_u *)varp;
	ವಾದಗಳು.os_idx = opt_idx;
	ವಾದಗಳು.os_flags = opt_flags;
	ವಾದಗಳು.os_op = op;
	ವಾದಗಳು.os_oldval.string = oldval;
	ವಾದಗಳು.os_newval.string = ಮೌಲ್ಯ;
	ವಾದಗಳು.os_errbuf = errbuf;
	ವಾದಗಳು.os_errbuflen = errbuflen;
	// Invoke the option specific callback function to validate and apply
	// the new option value.
	errmsg = did_ಗಣ_cb(&ವಾದಗಳು);

	// The 'keymap', 'filetype' and 'syntax' option callback functions
	// may change the os_value_checked field.
	*ಮೌಲ್ಯ_checked = ವಾದಗಳು.os_ಮೌಲ್ಯ_checked;
    }

    // If an error is detected, restore the previous value.
    ಆದರೆ (errmsg != ಖಾಲಿ)
    {
	free_string_option(*varp);
	*varp = oldval;
	// When resetting some values, need to act on it.
	ಆದರೆ (ವಾದಗಳು.os_restore_chartab)
	    (ಶೂನ್ಯ)init_chartab();
	ಆದರೆ (varp == &p_hl)
	    (ಶೂನ್ಯ)highlight_ಬದಲಾಯಿತು();
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	// Remember where the option was set.
	ಗಣ_option_sctx_idx(opt_idx, opt_flags, current_sctx);
#ಆದರೆ_ಅಂತ್ಯ
	// Free string options that are in allocated memory.
	// Use "free_oldval", because recursiveness may change the flags under
	// our fingers (esp. init_highlight()).
	ಆದರೆ (free_oldval)
	    free_string_option(oldval);
	ಗಣ_option_flag(opt_idx, P_ALLOCED);

	ಆದರೆ ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0
		&& is_global_local_option(opt_idx))
	{
	    // global option with local value set to use global value; free
	    // the local value and make it empty
	    char_u *p = get_option_varp_scope(opt_idx, OPT_LOCAL);
	    free_string_option(*(char_u **)p);
	    *(char_u **)p = ಖಾಲಿಗಳು_option;
	}

	// May set global value for local option.
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!(opt_flags & OPT_LOCAL) && opt_flags != OPT_GLOBAL)
	    ಗಣ_string_option_global(opt_idx, varp);

	// Trigger the autocommand only after setting the flags.
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
	ಆದರೆ (varp == &(curbuf->b_p_syn))
	    do_syntax_autocmd(ವಾದಗಳು.os_ಮೌಲ್ಯ_ಬದಲಾಯಿತು);
#ಆದರೆ_ಅಂತ್ಯ
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (varp == &(curbuf->b_p_ft))
	    do_filetype_autocmd(varp, opt_flags, ವಾದಗಳು.os_ಮೌಲ್ಯ_ಬದಲಾಯಿತು);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SPELL
	ಆದರೆ (varp == &(curwin->w_s->b_p_spl))
	    do_spelllang_ಮೂಲ();
#ಆದರೆ_ಅಂತ್ಯ
    }

    ಆದರೆ (varp == &p_mouse)
    {
	ಆದರೆ (*p_mouse == NUL)
	    mch_setmouse(FALSE);    // switch mouse off
	ಇಲ್ಲದಿದ್ದರೆ
	    setmouse();		    // in case 'mouse' changed
    }

    ಆದರೆ (varp == &p_rtp)
    {
	export_myvimdir();
#ಆದರೆ defined(FEAT_LUA)
	update_package_paths_ಒಳ_lua();
#ಆದರೆ_ಅಂತ್ಯ
    }

#ಆದರೆ defined(FEAT_LINEBREAK)
    // Changing Formatlistpattern when briopt includes the list setting:
    // redraw
    ಆದರೆ ((varp == &p_flp || varp == &(curbuf->b_p_flp))
	    && curwin->w_briopt_list)
	redraw_all_later(UPD_NOT_VALID);
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (curwin->w_curswant != MAXCOL
		   && (get_option_flags(opt_idx) & (P_CURSWANT | P_RALL)) != 0
				&& (get_option_flags(opt_idx) & P_HLONLY) == 0)
	curwin->w_ಗಣ_curswant = true;

    ಆದರೆ ((opt_flags & OPT_NO_REDRAW) == 0)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
	// set when changing an option that only requires a redraw in the GUI
	ಪೂರ್ಣಾಂಕ	redraw_gui_only = FALSE;

	ಆದರೆ (varp == &p_go			// 'guioptions'
		|| varp == &p_guifont		// 'guifont'
# ifdef FEAT_GUI_TABLINE
		|| varp == &p_gtl		// 'guitablabel'
		|| varp == &p_gtt		// 'guitabtooltip'
# endif
# ifdef FEAT_XFONTSET
		|| varp == &p_guifontset	// 'guifontset'
# endif
		|| varp == &p_guifontwide	// 'guifontwide'
# ಆದರೆ defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
		|| varp == &p_guiligatures	// 'guiligatures'
# endif
	    )
	    redraw_gui_only = TRUE;

	// check redraw when it's not a GUI option or the GUI is active.
	ಆದರೆ (!redraw_gui_only || gui.ಒಳ_use)
#ಆದರೆ_ಅಂತ್ಯ
	    check_redraw(get_option_flags(opt_idx));
    }

#ಆದರೆ defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
    ಆದರೆ (ವಾದಗಳು.os_did_swaptcap)
    {
	ಗಣ_termname((char_u *)"win32");
	init_highlight(TRUE, FALSE);
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಹಿಂತಿರುಗಿ errmsg;
}

/*
 * Check an option that can be a range of string values.
 *
 * Return OK for correct value, FAIL otherwise.
 * Empty is always OK.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
check_opt_ಅಕ್ಷರಸರಣಿಗಳು(
    char_u	*val,
    ಅಕ್ಷರ	**ಮೌಲ್ಯಗಳು,
    ಪೂರ್ಣಾಂಕ		list)	    // when TRUE: accept a list of values
{
    ಹಿಂತಿರುಗಿ opt_ಅಕ್ಷರಸರಣಿಗಳು_flags(val, ಮೌಲ್ಯಗಳು, ಖಾಲಿ, list);
}

/*
 * Handle an option that can be a range of string values.
 * Set a flag in "*flagp" for each string present.
 *
 * Return OK for correct value, FAIL otherwise.
 * Empty is always OK.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
opt_ಅಕ್ಷರಸರಣಿಗಳು_flags(
    char_u	*val,		// new value
    ಅಕ್ಷರ	**ಮೌಲ್ಯಗಳು,	// array of valid string values
    ಚಿಹ್ನೆರಹಿತ	*flagp,
    ಪೂರ್ಣಾಂಕ		list)		// when TRUE: accept a list of values
{
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		len;
    ಚಿಹ್ನೆರಹಿತ	new_flags = 0;

    ಆಗ (*val)
    {
	ಗಾಗಿ (i = 0; ; ++i)
	{
	    ಆದರೆ (ಮೌಲ್ಯಗಳು[i] == ಖಾಲಿ)	// val not found in values[]
		ಹಿಂತಿರುಗಿ FAIL;

	    len = (ಪೂರ್ಣಾಂಕ)STRLEN(ಮೌಲ್ಯಗಳು[i]);
	    ಆದರೆ (STRNCMP(ಮೌಲ್ಯಗಳು[i], val, len) == 0
		    && ((list && val[len] == ',') || val[len] == NUL))
	    {
		val += len + (val[len] == ',');
		new_flags |= (1 << i);
		ನಿಲ್ಲಿಸು;		// check next item in val list
	    }
	}
    }
    ಆದರೆ (flagp != ಖಾಲಿ)
	*flagp = new_flags;

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * return OK if "p" is a valid fileformat name, FAIL otherwise.
 */
    ಪೂರ್ಣಾಂಕ
check_ff_ಮೌಲ್ಯ(char_u *p)
{
    ಹಿಂತಿರುಗಿ check_opt_ಅಕ್ಷರಸರಣಿಗಳು(p, p_ff_ಮೌಲ್ಯಗಳು, FALSE);
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
    ಆದರೆ (STRLEN(p_shm) >= SHM_LEN)
    {
	iemsg(e_ಆಂತರಿಕ_error_shortmess_too_long);
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (++ಗಣ_shm_recursive == 1)
    {
	STRCPY(shm_buf, p_shm);
	ಗಣ_option_ಮೌಲ್ಯ_give_err((char_u *)"shm", 0L, (char_u *)"", 0);
    }
}

/*
 * Restore the shortmess Flags set from the save_clear_shm_value() function.
 */
    ಶೂನ್ಯ
restore_shm_ಮೌಲ್ಯ(ಶೂನ್ಯ)
{
    ಆದರೆ (--ಗಣ_shm_recursive == 0)
    {
	ಗಣ_option_ಮೌಲ್ಯ_give_err((char_u *)"shm", 0L, shm_buf, 0);
	vim_memset(shm_buf, 0, SHM_LEN);
    }
}

/*
 * Export the environment variable $MYVIMDIR to the first item in runtimepath
 */
    ಶೂನ್ಯ
export_myvimdir(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		dofree = FALSE;
    char_u	*p;
    char_u	*q = p_rtp;
    string_T	buf;

    buf.string = alloc(MAXPATHL);
    ಆದರೆ (buf.string == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    buf.ಉದ್ದಳತೆ = (size_t)copy_option_part(&q, buf.string, MAXPATHL, ",");

    p = vim_getenv((char_u *)"MYVIMDIR", &dofree);

    ಆದರೆ (p == ಖಾಲಿ || STRCMP(p, buf.string) != 0)
    {
	ಆದರೆ (*buf.string != NUL && !after_pathsep(buf.string, buf.string + buf.ಉದ್ದಳತೆ))
	    STRCPY(buf.string + buf.ಉದ್ದಳತೆ, PATHSEPSTR);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
	// normalize path separators
	ಗಾಗಿ (q = buf.string; *q != NUL; q++)
	    ಆದರೆ (*q == '/')
		*q = '\\';
#ಆದರೆ_ಅಂತ್ಯ
	vim_setenv((char_u *)"MYVIMDIR", buf.string);
    }
    ಆದರೆ (dofree)
	vim_free(p);
    vim_free(buf.string);
}
