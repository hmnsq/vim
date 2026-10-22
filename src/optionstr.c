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

#അന്തര്ഭാവ "vim.h"

സ്ഥിതം char_u shm_buf[SHM_LEN];
സ്ഥിതം പൂർണ്ണസംഖ്യ set_shm_recursive = 0;

സ്ഥിതം അക്ഷരം *(p_ambw_മൂല്യങ്ങൾ[]) = {"single", "double", ശൂന്യ};
സ്ഥിതം അക്ഷരം *(p_bg_മൂല്യങ്ങൾ[]) = {"light", "dark", ശൂന്യ};
സ്ഥിതം അക്ഷരം *(p_bkc_മൂല്യങ്ങൾ[]) = {"yes", "auto", "no", "breaksymlink", "breakhardlink", ശൂന്യ};
സ്ഥിതം അക്ഷരം *(p_bo_മൂല്യങ്ങൾ[]) = {"all", "backspace", "cursor", "complete",
				 "copy", "ctrlg", "error", "esc", "ex",
				 "hangul", "insertmode", "lang", "mess",
				 "showmatch", "operator", "register", "shell",
				 "spell", "term", "wildmode", ശൂന്യ};
#എങ്കിൽ defined(FEAT_LINEBREAK)
// Note: Keep this in sync with briopt_check()
സ്ഥിതം അക്ഷരം *(p_briopt_മൂല്യങ്ങൾ[]) = {"shift:", "min:", "sbr", "list:", "column:", ശൂന്യ};
#endif
#എങ്കിൽ defined(FEAT_TABPANEL)
// Note: Keep this in sync with tabpanelopt_changed()
സ്ഥിതം അക്ഷരം *(p_tplo_മൂല്യങ്ങൾ[]) = {"align:", "columns:", "scrollbar", "vert", ശൂന്യ};
സ്ഥിതം അക്ഷരം *(p_tplo_align_മൂല്യങ്ങൾ[]) = {"left", "right", ശൂന്യ};
#endif
#എങ്കിൽ defined(FEAT_DIFF)
// Note: Keep this in sync with diffopt_changed()
സ്ഥിതം അക്ഷരം *(p_dip_മൂല്യങ്ങൾ[]) = {"filler", "anchor", "context:", "iblank", "icase", "iwhite", "iwhiteall", "iwhiteeol", "horizontal", "vertical", "closeoff", "hiddenoff", "foldcolumn:", "followwrap", "internal", "indent-heuristic", "algorithm:", "inline:", "linematch:", ശൂന്യ};
സ്ഥിതം അക്ഷരം *(p_dip_algorithm_മൂല്യങ്ങൾ[]) = {"myers", "minimal", "patience", "histogram", ശൂന്യ};
സ്ഥിതം അക്ഷരം *(p_dip_inline_മൂല്യങ്ങൾ[]) = {"none", "simple", "char", "word", ശൂന്യ};
#endif
സ്ഥിതം അക്ഷരം *(p_nf_മൂല്യങ്ങൾ[]) = {"bin", "octal", "hex", "alpha", "unsigned", "blank", ശൂന്യ};
സ്ഥിതം അക്ഷരം *(p_ff_മൂല്യങ്ങൾ[]) = {FF_UNIX, FF_DOS, FF_MAC, ശൂന്യ};
#ifdef HAVE_CLIPMETHOD
// Note: Keep this in sync with did_set_clipboard()
സ്ഥിതം അക്ഷരം *(p_cb_മൂല്യങ്ങൾ[]) = {"unnamed", "unnamedplus",
# ifdef FEAT_CLIPBOARD
    "autoselect", "autoselectplus", "autoselectml", "html", "exclude:",
# endif
    ശൂന്യ};
#endif
#ifdef FEAT_CRYPT
സ്ഥിതം അക്ഷരം *(p_cm_മൂല്യങ്ങൾ[]) = {"zip", "blowfish", "blowfish2",
# ifdef FEAT_SODIUM
    "xchacha20", "xchacha20v2",
# endif
    ശൂന്യ};
#endif
സ്ഥിതം അക്ഷരം *(p_cmp_മൂല്യങ്ങൾ[]) = {"internal", "keepascii", ശൂന്യ};
#ifdef FEAT_SYN_HL
// Note: Keep this in sync with fill_culopt_flags()
സ്ഥിതം അക്ഷരം *(p_culopt_മൂല്യങ്ങൾ[]) = {"line", "screenline", "number", "both", ശൂന്യ};
#endif
സ്ഥിതം അക്ഷരം *(p_dy_മൂല്യങ്ങൾ[]) = {"lastline", "truncate", "uhex", ശൂന്യ};
സ്ഥിതം അക്ഷരം *(p_jop_മൂല്യങ്ങൾ[]) = {"stack", ശൂന്യ};
#ifdef FEAT_FOLDING
സ്ഥിതം അക്ഷരം *(p_fdo_മൂല്യങ്ങൾ[]) = {"all", "block", "hor", "mark", "percent",
				 "quickfix", "search", "tag", "insert",
				 "undo", "jump", ശൂന്യ};
#endif
// Note: Keep this in sync with match_keyprotocol()
സ്ഥിതം അക്ഷരം *(p_kpc_protocol_മൂല്യങ്ങൾ[]) = {"none", "mok2", "kitty", ശൂന്യ};
#ifdef FEAT_IMAGE
// Note: Keep this in sync with match_imageprotocol()
സ്ഥിതം അക്ഷരം *(p_ipc_protocol_മൂല്യങ്ങൾ[]) = {"none", "kitty", "sixel", ശൂന്യ};
#endif
#ifdef FEAT_PROP_POPUP
// Note: Keep this in sync with parse_popup_option()
സ്ഥിതം അക്ഷരം *(p_popup_cpp_option_മൂല്യങ്ങൾ[]) = {"align:", "border:",
    "borderhighlight:", "close:", "height:", "highlight:", "opacity:",
    "resize:", "shadow:", "width:", ശൂന്യ};
സ്ഥിതം അക്ഷരം *(p_popup_pvp_option_മൂല്യങ്ങൾ[]) = {"border:",
    "borderhighlight:", "close:", "height:", "highlight:", "opacity:",
    "resize:", "shadow:", "width:", ശൂന്യ};
സ്ഥിതം അക്ഷരം *(p_popup_option_on_off_മൂല്യങ്ങൾ[]) = {"on", "off", ശൂന്യ};
സ്ഥിതം അക്ഷരം *(p_popup_cpp_border_മൂല്യങ്ങൾ[]) = {"single", "double", "round",
    "ascii", "on", "off", "custom:", ശൂന്യ};
സ്ഥിതം അക്ഷരം *(p_popup_pvp_border_മൂല്യങ്ങൾ[]) = {"single", "double", "round",
    "ascii", "on", "off", "custom:", ശൂന്യ};
സ്ഥിതം അക്ഷരം *(p_popup_option_align_മൂല്യങ്ങൾ[]) = {"item", "menu", ശൂന്യ};
#endif
#എങ്കിൽ defined(FEAT_SPELL)
// Note: Keep this in sync with spell_check_sps()
സ്ഥിതം അക്ഷരം *(p_sps_മൂല്യങ്ങൾ[]) = {"best", "fast", "double", "expr:", "file:", "timeout:", ശൂന്യ};
#endif
#ifdef FEAT_SESSION
// Also used for 'viewoptions'!  Keep in sync with SSOP_ flags.
സ്ഥിതം അക്ഷരം *(p_ssop_മൂല്യങ്ങൾ[]) = {"buffers", "winpos", "resize", "winsize",
    "localoptions", "options", "help", "blank", "globals", "slash", "unix",
    "sesdir", "curdir", "folds", "cursor", "tabpages", "terminal", "skiprtp",
    ശൂന്യ};
#endif
#എങ്കിൽ defined(FEAT_STL_OPT)
സ്ഥിതം അക്ഷരം *(p_stlo_മൂല്യങ്ങൾ[]) = {"fixedheight", "maxheight:", ശൂന്യ};
#endif
// Keep in sync with SWB_ flags in option.h
സ്ഥിതം അക്ഷരം *(p_swb_മൂല്യങ്ങൾ[]) = {"useopen", "usetab", "split", "newtab", "vsplit", "uselast", ശൂന്യ};
സ്ഥിതം അക്ഷരം *(p_spk_മൂല്യങ്ങൾ[]) = {"cursor", "screen", "topline", ശൂന്യ};
സ്ഥിതം അക്ഷരം *(p_tc_മൂല്യങ്ങൾ[]) = {"followic", "ignore", "match", "followscs", "smart", ശൂന്യ};
// Keep in sync with TCL_ flags in option.h
സ്ഥിതം അക്ഷരം *(p_tcl_മൂല്യങ്ങൾ[]) = {"left", "uselast", ശൂന്യ};
#എങ്കിൽ defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
സ്ഥിതം അക്ഷരം *(p_toolbar_മൂല്യങ്ങൾ[]) = {"text", "icons", "tooltips", "horiz", ശൂന്യ};
#endif
#എങ്കിൽ defined(FEAT_TOOLBAR) && defined(FEAT_GUI_GTK)
സ്ഥിതം അക്ഷരം *(p_tbis_മൂല്യങ്ങൾ[]) = {"tiny", "small", "medium", "large", "huge", "giant", ശൂന്യ};
#endif
#എങ്കിൽ defined(UNIX) || defined(VMS)
സ്ഥിതം അക്ഷരം *(p_ttym_മൂല്യങ്ങൾ[]) = {"xterm", "xterm2", "dec", "netterm", "jsbterm", "pterm", "urxvt", "sgr", ശൂന്യ};
#endif
സ്ഥിതം അക്ഷരം *(p_ve_മൂല്യങ്ങൾ[]) = {"block", "insert", "all", "onemore", "none", "NONE", ശൂന്യ};
// Note: Keep this in sync with check_opt_wim()
സ്ഥിതം അക്ഷരം *(p_wim_മൂല്യങ്ങൾ[]) = {"full", "longest", "list", "lastused", "noselect", "noinsert", ശൂന്യ};
സ്ഥിതം അക്ഷരം *(p_wop_മൂല്യങ്ങൾ[]) = {"fuzzy", "tagfile", "pum", "exacttext", ശൂന്യ};
#ifdef FEAT_WAK
സ്ഥിതം അക്ഷരം *(p_wak_മൂല്യങ്ങൾ[]) = {"yes", "menu", "no", ശൂന്യ};
#endif
സ്ഥിതം അക്ഷരം *(p_mousem_മൂല്യങ്ങൾ[]) = {"extend", "popup", "popup_setpos", "mac", ശൂന്യ};
സ്ഥിതം അക്ഷരം *(p_sel_മൂല്യങ്ങൾ[]) = {"inclusive", "exclusive", "old", ശൂന്യ};
സ്ഥിതം അക്ഷരം *(p_slm_മൂല്യങ്ങൾ[]) = {"mouse", "key", "cmd", ശൂന്യ};
സ്ഥിതം അക്ഷരം *(p_km_മൂല്യങ്ങൾ[]) = {"startsel", "stopsel", ശൂന്യ};
#ifdef FEAT_BROWSE
സ്ഥിതം അക്ഷരം *(p_bsdir_മൂല്യങ്ങൾ[]) = {"current", "last", "buffer", ശൂന്യ};
#endif
സ്ഥിതം അക്ഷരം *(p_scbopt_മൂല്യങ്ങൾ[]) = {"ver", "hor", "jump", ശൂന്യ};
സ്ഥിതം അക്ഷരം *(p_debug_മൂല്യങ്ങൾ[]) = {"msg", "throw", "beep", ശൂന്യ};
സ്ഥിതം അക്ഷരം *(p_ead_മൂല്യങ്ങൾ[]) = {"both", "ver", "hor", ശൂന്യ};
സ്ഥിതം അക്ഷരം *(p_buftype_മൂല്യങ്ങൾ[]) = {"nofile", "nowrite", "quickfix", "help", "terminal", "acwrite", "prompt", "popup", ശൂന്യ};
സ്ഥിതം അക്ഷരം *(p_bufhidden_മൂല്യങ്ങൾ[]) = {"hide", "unload", "delete", "wipe", ശൂന്യ};
സ്ഥിതം അക്ഷരം *(p_bs_മൂല്യങ്ങൾ[]) = {"indent", "eol", "start", "nostop", ശൂന്യ};
#ifdef FEAT_FOLDING
സ്ഥിതം അക്ഷരം *(p_fdm_മൂല്യങ്ങൾ[]) = {"manual", "expr", "marker", "indent", "syntax",
# ifdef FEAT_DIFF
				"diff",
# endif
				ശൂന്യ};
സ്ഥിതം അക്ഷരം *(p_fcl_മൂല്യങ്ങൾ[]) = {"all", ശൂന്യ};
#endif
സ്ഥിതം അക്ഷരം *(p_cfc_മൂല്യങ്ങൾ[]) = {"keyword", "files", "whole_line", ശൂന്യ};
സ്ഥിതം അക്ഷരം *(p_cot_മൂല്യങ്ങൾ[]) = {"menu", "menuone", "longest", "preview", "popup", "popuphidden", "noinsert", "noselect", "fuzzy", "nosort", "preinsert", "nearest", ശൂന്യ};
#ifdef BACKSLASH_IN_FILENAME
സ്ഥിതം അക്ഷരം *(p_csl_മൂല്യങ്ങൾ[]) = {"slash", "backslash", ശൂന്യ};
#endif
#ifdef FEAT_SIGNS
സ്ഥിതം അക്ഷരം *(p_scl_മൂല്യങ്ങൾ[]) = {"yes", "no", "auto", "number", ശൂന്യ};
#endif
#ifdef UNIX
സ്ഥിതം അക്ഷരം *(p_trz_മൂല്യങ്ങൾ[]) = {"inband", "sigwinch", "", ശൂന്യ};
#endif
#എങ്കിൽ defined(MSWIN) && defined(FEAT_TERMINAL)
സ്ഥിതം അക്ഷരം *(p_twt_മൂല്യങ്ങൾ[]) = {"winpty", "conpty", "", ശൂന്യ};
#endif
സ്ഥിതം അക്ഷരം *(p_sloc_മൂല്യങ്ങൾ[]) = {"last", "statusline", "tabline", ശൂന്യ};
സ്ഥിതം അക്ഷരം *(p_sws_മൂല്യങ്ങൾ[]) = {"fsync", "sync", ശൂന്യ};

സ്ഥിതം പൂർണ്ണസംഖ്യ check_opt_strings(char_u *val, അക്ഷരം **മൂല്യങ്ങൾ, പൂർണ്ണസംഖ്യ list);
സ്ഥിതം പൂർണ്ണസംഖ്യ opt_strings_flags(char_u *val, അക്ഷരം **മൂല്യങ്ങൾ, unsigned *flagp, പൂർണ്ണസംഖ്യ list);

/*
 * After setting various option values: recompute variables that depend on
 * option values.
 */
    ശൂന്യം
didset_string_options(ശൂന്യം)
{
    (ശൂന്യം)opt_strings_flags(p_cmp, p_cmp_മൂല്യങ്ങൾ, &cmp_flags, TRUE);
    (ശൂന്യം)opt_strings_flags(p_bkc, p_bkc_മൂല്യങ്ങൾ, &bkc_flags, TRUE);
    (ശൂന്യം)opt_strings_flags(p_bo, p_bo_മൂല്യങ്ങൾ, &bo_flags, TRUE);
    (ശൂന്യം)opt_strings_flags(p_cot, p_cot_മൂല്യങ്ങൾ, &cot_flags, TRUE);
#ifdef FEAT_SESSION
    (ശൂന്യം)opt_strings_flags(p_ssop, p_ssop_മൂല്യങ്ങൾ, &ssop_flags, TRUE);
    (ശൂന്യം)opt_strings_flags(p_vop, p_ssop_മൂല്യങ്ങൾ, &vop_flags, TRUE);
#endif
#ifdef FEAT_FOLDING
    (ശൂന്യം)opt_strings_flags(p_fdo, p_fdo_മൂല്യങ്ങൾ, &fdo_flags, TRUE);
#endif
    (ശൂന്യം)opt_strings_flags(p_dy, p_dy_മൂല്യങ്ങൾ, &dy_flags, TRUE);
    (ശൂന്യം)opt_strings_flags(p_jop, p_jop_മൂല്യങ്ങൾ, &jop_flags, TRUE);
    (ശൂന്യം)opt_strings_flags(p_tc, p_tc_മൂല്യങ്ങൾ, &tc_flags, FALSE);
    (ശൂന്യം)opt_strings_flags(p_ve, p_ve_മൂല്യങ്ങൾ, &ve_flags, TRUE);
#എങ്കിൽ defined(UNIX) || defined(VMS)
    (ശൂന്യം)opt_strings_flags(p_ttym, p_ttym_മൂല്യങ്ങൾ, &ttym_flags, FALSE);
#endif
#എങ്കിൽ defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
    (ശൂന്യം)opt_strings_flags(p_toolbar, p_toolbar_മൂല്യങ്ങൾ, &toolbar_flags, TRUE);
#endif
#എങ്കിൽ defined(FEAT_TOOLBAR) && defined(FEAT_GUI_GTK)
    (ശൂന്യം)opt_strings_flags(p_tbis, p_tbis_മൂല്യങ്ങൾ, &tbis_flags, FALSE);
#endif
    (ശൂന്യം)opt_strings_flags(p_swb, p_swb_മൂല്യങ്ങൾ, &swb_flags, TRUE);
    (ശൂന്യം)opt_strings_flags(p_tcl, p_tcl_മൂല്യങ്ങൾ, &tcl_flags, TRUE);
}

#എങ്കിൽ defined(FEAT_EVAL)
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
    ശൂന്യം
trigger_optionset_string(
	പൂർണ്ണസംഖ്യ	opt_idx,
	പൂർണ്ണസംഖ്യ	opt_flags,
	char_u  *oldval,
	char_u  *oldval_l,
	char_u  *oldval_g,
	char_u  *newval)
{
    // Don't do this recursively.
    എങ്കിൽ (oldval == ശൂന്യ || newval == ശൂന്യ
				    || *get_vim_var_str(VV_OPTION_TYPE) != NUL)
	തിരികെ;

    char_u buf_type[7];
    size_t buf_typelen;
    size_t oldvallen;

    oldvallen = STRLEN(oldval);
    set_vim_var_string(VV_OPTION_OLD, oldval, (പൂർണ്ണസംഖ്യ)oldvallen);
    set_vim_var_string(VV_OPTION_NEW, newval, -1);
    buf_typelen = vim_snprintf_safelen((അക്ഷരം *)buf_type, വലുപ്പം(buf_type),
	"%s", (opt_flags & OPT_LOCAL) ? "local" : "global");
    set_vim_var_string(VV_OPTION_TYPE, buf_type, (പൂർണ്ണസംഖ്യ)buf_typelen);
    എങ്കിൽ (opt_flags & OPT_LOCAL)
    {
	set_vim_var_string(VV_OPTION_COMMAND, (char_u *)"setlocal", STRLEN_LITERAL("setlocal"));
	set_vim_var_string(VV_OPTION_OLDLOCAL, oldval, (പൂർണ്ണസംഖ്യ)oldvallen);
    }
    എങ്കിൽ (opt_flags & OPT_GLOBAL)
    {
	set_vim_var_string(VV_OPTION_COMMAND, (char_u *)"setglobal", STRLEN_LITERAL("setglobal"));
	set_vim_var_string(VV_OPTION_OLDGLOBAL, oldval, (പൂർണ്ണസംഖ്യ)oldvallen);
    }
    എങ്കിൽ ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	set_vim_var_string(VV_OPTION_COMMAND, (char_u *)"set", STRLEN_LITERAL("set"));
	set_vim_var_string(VV_OPTION_OLDLOCAL, oldval_l, -1);
	set_vim_var_string(VV_OPTION_OLDGLOBAL, oldval_g, -1);
    }
    എങ്കിൽ (opt_flags & OPT_MODELINE)
    {
	set_vim_var_string(VV_OPTION_COMMAND, (char_u *)"modeline", STRLEN_LITERAL("modeline"));
	set_vim_var_string(VV_OPTION_OLDLOCAL, oldval, (പൂർണ്ണസംഖ്യ)oldvallen);
    }
    apply_autocmds(EVENT_OPTIONSET,
	    get_option_fullname(opt_idx), ശൂന്യ, FALSE,
	    ശൂന്യ);
    reset_v_option_vars();
}
#endif

    സ്ഥിതം അക്ഷരം *
illegal_char(അക്ഷരം *errbuf, size_t errbuflen, പൂർണ്ണസംഖ്യ c)
{
    എങ്കിൽ (errbuf == ശൂന്യ)
	തിരികെ "";
    vim_snprintf(errbuf, errbuflen, _(e_illegal_character_str),
		    (അക്ഷരം *)transchar(c));
    തിരികെ errbuf;
}

    സ്ഥിതം അക്ഷരം *
illegal_char_after_chr(അക്ഷരം *errbuf, size_t errbuflen, പൂർണ്ണസംഖ്യ c)
{
    എങ്കിൽ (errbuf == ശൂന്യ)
	തിരികെ "";
    vim_snprintf(errbuf, errbuflen, _(e_illegal_character_after_chr), c);
    തിരികെ errbuf;
}

/*
 * Check string options in a buffer for NULL value.
 */
    ശൂന്യം
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
#എങ്കിൽ defined(FEAT_EVAL)
    check_string_option(&buf->b_p_inde);
    check_string_option(&buf->b_p_indk);
#endif
#എങ്കിൽ defined(FEAT_BEVAL) && defined(FEAT_EVAL)
    check_string_option(&buf->b_p_bexpr);
#endif
#എങ്കിൽ defined(FEAT_CRYPT)
    check_string_option(&buf->b_p_cm);
#endif
    check_string_option(&buf->b_p_fp);
#എങ്കിൽ defined(FEAT_EVAL)
    check_string_option(&buf->b_p_fex);
#endif
#ifdef FEAT_CRYPT
    check_string_option(&buf->b_p_താക്കോൽ);
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
    ശൂന്യം
free_string_option(char_u *p)
{
    എങ്കിൽ (p != empty_option)
	vim_free(p);
}

    ശൂന്യം
clear_string_option(char_u **pp)
{
    എങ്കിൽ (*pp != empty_option)
	vim_free(*pp);
    *pp = empty_option;
}

    ശൂന്യം
check_string_option(char_u **pp)
{
    എങ്കിൽ (*pp == ശൂന്യ)
	*pp = empty_option;
}

/*
 * Set global value for string option when it's a local option.
 */
    സ്ഥിതം ശൂന്യം
set_string_option_global(
    പൂർണ്ണസംഖ്യ		opt_idx,	// option index
    char_u	**varp)		// pointer to option variable
{
    char_u	**p, *s;

    // the global value is always allocated
    എങ്കിൽ (is_window_local_option(opt_idx))
	p = (char_u **)GLOBAL_WO(varp);
    അല്ലെങ്കിൽ
	p = (char_u **)get_option_var(opt_idx);
    എങ്കിൽ (!is_global_option(opt_idx)
	    && p != varp
	    && (s = vim_strsave(*varp)) != ശൂന്യ)
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
    ശൂന്യം
set_string_option_direct(
    char_u	*name,
    പൂർണ്ണസംഖ്യ		opt_idx,
    char_u	*val,
    പൂർണ്ണസംഖ്യ		opt_flags,	// OPT_FREE, OPT_LOCAL and/or OPT_GLOBAL
    പൂർണ്ണസംഖ്യ		set_sid UNUSED)
{
    char_u	*s;
    char_u	**varp;
    പൂർണ്ണസംഖ്യ		both = (opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0;
    പൂർണ്ണസംഖ്യ		idx = opt_idx;

    എങ്കിൽ (idx == -1)		// use name
    {
	idx = findoption(name);
	എങ്കിൽ (idx < 0)	// not found (should not happen)
	{
	    semsg(_(e_internal_error_str), "set_string_option_direct()");
	    siemsg("For option %s", name);
	    തിരികെ;
	}
    }

    എങ്കിൽ (is_hidden_option(idx))		// can't set hidden option
	തിരികെ;

    s = vim_strsave(val);
    എങ്കിൽ (s == ശൂന്യ)
	തിരികെ;

    varp = (char_u **)get_option_varp_scope(idx,
	    both ? OPT_LOCAL : opt_flags);
    എങ്കിൽ ((opt_flags & OPT_FREE) && (get_option_flags(idx) & P_ALLOCED))
	free_string_option(*varp);
    *varp = s;

    // For buffer/window local option may also set the global value.
    എങ്കിൽ (both)
	set_string_option_global(idx, varp);

    set_option_flag(idx, P_ALLOCED);

    // When setting both values of a global option with a local value,
    // make the local value empty, so that the global value is used.
    എങ്കിൽ (is_global_local_option(idx) && both)
    {
	free_string_option(*varp);
	*varp = empty_option;
    }
#ifdef FEAT_EVAL
    എങ്കിൽ (set_sid != SID_NONE)
    {
	sctx_T script_ctx;

	എങ്കിൽ (set_sid == 0)
	    script_ctx = current_sctx;
	അല്ലെങ്കിൽ
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
    ശൂന്യം
set_string_option_direct_in_win(
	win_T		*wp,
	char_u		*name,
	പൂർണ്ണസംഖ്യ		opt_idx,
	char_u		*val,
	പൂർണ്ണസംഖ്യ		opt_flags,
	പൂർണ്ണസംഖ്യ		set_sid)
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

#എങ്കിൽ defined(FEAT_PROP_POPUP)
/*
 * Like set_string_option_direct(), but for a buffer-local option in "buf".
 * Blocks autocommands to avoid the old curbuf becoming invalid.
 */
    ശൂന്യം
set_string_option_direct_in_buf(
	buf_T		*buf,
	char_u		*name,
	പൂർണ്ണസംഖ്യ		opt_idx,
	char_u		*val,
	പൂർണ്ണസംഖ്യ		opt_flags,
	പൂർണ്ണസംഖ്യ		set_sid)
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
    അക്ഷരം *
set_string_option(
    പൂർണ്ണസംഖ്യ		opt_idx,
    char_u	*മൂല്യം,
    പൂർണ്ണസംഖ്യ		opt_flags,	// OPT_LOCAL and/or OPT_GLOBAL
    അക്ഷരം	*errbuf,
    size_t	errbuflen)
{
    char_u	*s;
    char_u	**varp;
    char_u	*oldval;
#എങ്കിൽ defined(FEAT_EVAL)
    char_u	*oldval_l = ശൂന്യ;
    char_u	*oldval_g = ശൂന്യ;
    char_u	*saved_oldval = ശൂന്യ;
    char_u	*saved_oldval_l = ശൂന്യ;
    char_u	*saved_oldval_g = ശൂന്യ;
    char_u	*saved_newval = ശൂന്യ;
#endif
    അക്ഷരം	*errmsg = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		മൂല്യം_checked = FALSE;

    എങ്കിൽ (is_hidden_option(opt_idx))	// don't set hidden option
	തിരികെ ശൂന്യ;

    s = vim_strsave(മൂല്യം == ശൂന്യ ? (char_u *)"" : മൂല്യം);
    എങ്കിൽ (s == ശൂന്യ)
	തിരികെ ശൂന്യ;

    varp = (char_u **)get_option_varp_scope(opt_idx,
	    (opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0
	    ? (is_global_local_option(opt_idx)
		? OPT_GLOBAL : OPT_LOCAL)
	    : opt_flags);
    oldval = *varp;
#എങ്കിൽ defined(FEAT_EVAL)
    എങ്കിൽ ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	oldval_l = *(char_u **)get_option_varp_scope(opt_idx, OPT_LOCAL);
	oldval_g = *(char_u **)get_option_varp_scope(opt_idx, OPT_GLOBAL);
    }
#endif
    *varp = s;

#എങ്കിൽ defined(FEAT_EVAL)
    എങ്കിൽ (!starting
# ifdef FEAT_CRYPT
	    && !is_crypt_താക്കോൽ_option(opt_idx)
# endif
       )
    {
	എങ്കിൽ (oldval_l != ശൂന്യ)
	    saved_oldval_l = vim_strsave(oldval_l);
	എങ്കിൽ (oldval_g != ശൂന്യ)
	    saved_oldval_g = vim_strsave(oldval_g);
	saved_oldval = vim_strsave(oldval);
	saved_newval = vim_strsave(s);
    }
#endif
    എങ്കിൽ ((errmsg = did_set_string_option(opt_idx, varp, oldval, മൂല്യം, errbuf,
		    errbuflen, opt_flags, OP_NONE, &മൂല്യം_checked)) == ശൂന്യ)
	did_set_option(opt_idx, opt_flags, TRUE, മൂല്യം_checked);

#എങ്കിൽ defined(FEAT_EVAL)
    // call autocommand after handling side effects
    എങ്കിൽ (errmsg == ശൂന്യ)
	trigger_optionset_string(opt_idx, opt_flags,
		saved_oldval, saved_oldval_l,
		saved_oldval_g, saved_newval);
    vim_free(saved_oldval);
    vim_free(saved_oldval_l);
    vim_free(saved_oldval_g);
    vim_free(saved_newval);
#endif
    തിരികെ errmsg;
}

/*
 * Return TRUE if "val" is a valid 'filetype' name.
 * Also used for 'syntax' and 'keymap'.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
valid_filetype(char_u *val)
{
    തിരികെ valid_name(val, ".-_");
}

#ifdef FEAT_STL_OPT
/*
 * Check validity of options with the 'statusline' format.
 * Return an untranslated error message or NULL.
 */
    സ്ഥിതം അക്ഷരം *
check_stl_option(char_u *s)
{
    പൂർണ്ണസംഖ്യ		groupdepth = 0;
    സ്ഥിതം അക്ഷരം errbuf[ERR_BUFLEN];
    പൂർണ്ണസംഖ്യ		errbuflen = ERR_BUFLEN;

    ആകുമ്പോൾ (*s)
    {
	// Check for valid keys after % sequences
	ആകുമ്പോൾ (*s && *s != '%')
	    s++;
	എങ്കിൽ (!*s)
	    നിർത്തുക;
	s++;
	എങ്കിൽ (*s == STL_CLICKFUNC)
	{
	    എങ്കിൽ (s[1] == ']')
	    {
		// %[] - end click region
		s += 2;
		തുടരുക;
	    }
	    എങ്കിൽ (ASCII_ISALPHA(s[1]) || s[1] == '_')
	    {
		// %[FuncName] - start click region
		char_u *rb = vim_strchr(s + 2, ']');
		എങ്കിൽ (rb != ശൂന്യ)
		{
		    s = rb + 1;
		    തുടരുക;
		}
	    }
	    // Bare %[ is invalid
	    തിരികെ illegal_char(errbuf, errbuflen, *s);
	}
	എങ്കിൽ (*s == STL_LINEBREAK)
	{
	    // Plain %@ - line break
	    s++;
	    തുടരുക;
	}
	എങ്കിൽ (*s == '%' || *s == STL_TRUNCMARK || *s == STL_SEPARATE)
	{
	    s++;
	    തുടരുക;
	}
	എങ്കിൽ (*s == ')')
	{
	    s++;
	    എങ്കിൽ (--groupdepth < 0)
		നിർത്തുക;
	    തുടരുക;
	}
	എങ്കിൽ (*s == '-')
	    s++;
	ആകുമ്പോൾ (VIM_ISDIGIT(*s))
	    s++;
	എങ്കിൽ (*s == STL_USER_HL)
	    തുടരുക;
	എങ്കിൽ (*s == STL_CLICKFUNC)
	{
	    // %N[FuncName] or %N[]
	    എങ്കിൽ (s[1] == ']')
	    {
		s += 2;
		തുടരുക;
	    }
	    എങ്കിൽ (ASCII_ISALPHA(s[1]) || s[1] == '_')
	    {
		char_u *rb = vim_strchr(s + 2, ']');
		എങ്കിൽ (rb != ശൂന്യ)
		{
		    s = rb + 1;
		    തുടരുക;
		}
	    }
	    // Bare %N[ is invalid
	    തിരികെ illegal_char(errbuf, errbuflen, *s);
	}
	എങ്കിൽ (*s == STL_LINEBREAK)
	{
	    // %N@ - line break
	    s++;
	    തുടരുക;
	}
	എങ്കിൽ (*s == '.')
	{
	    s++;
	    ആകുമ്പോൾ (*s && VIM_ISDIGIT(*s))
		s++;
	}
	എങ്കിൽ (*s == '(')
	{
	    groupdepth++;
	    തുടരുക;
	}
	എങ്കിൽ (vim_strchr(STL_ALL, *s) == ശൂന്യ)
	{
	    തിരികെ illegal_char(errbuf, errbuflen, *s);
	}
	എങ്കിൽ (*s == '{')
	{
	    പൂർണ്ണസംഖ്യ reevaluate = (*++s == '%');

	    എങ്കിൽ (reevaluate && *++s == '}')
		// "}" is not allowed immediately after "%{%"
		തിരികെ illegal_char(errbuf, errbuflen, '}');
	    ആകുമ്പോൾ ((*s != '}' || (reevaluate && s[-1] != '%')) && *s)
		s++;
	    എങ്കിൽ (*s != '}')
		തിരികെ e_unclosed_expression_sequence;
	}
    }
    എങ്കിൽ (groupdepth != 0)
	തിരികെ e_unbalanced_groups;
    തിരികെ ശൂന്യ;
}
#endif

/*
 * Check for a "normal" directory or file name in some options.  Disallow a
 * path separator (slash and/or backslash), wildcards and characters that are
 * often illegal in a file name. Be more permissive if "secure" is off.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
check_illegal_path_names(പൂർണ്ണസംഖ്യ opt_idx, char_u **varp)
{
    തിരികെ (((get_option_flags(opt_idx) & P_NFNAME)
		&& vim_strpbrk(*varp, (char_u *)(secure
			? "/\\*?[|;&<>\r\n" : "/\\*?[<>\r\n")) != ശൂന്യ)
	    || ((get_option_flags(opt_idx) & P_NDNAME)
		&& vim_strpbrk(*varp, (char_u *)"*?[|;&<>\r\n") != ശൂന്യ));
}

/*
 * An option that accepts a list of flags is changed.
 * e.g. 'viewoptions', 'switchbuf', 'casemap', etc.
 */
    സ്ഥിതം അക്ഷരം *
did_set_opt_flags(char_u *val, അക്ഷരം **മൂല്യങ്ങൾ, unsigned *flagp, പൂർണ്ണസംഖ്യ list)
{
    എങ്കിൽ (opt_strings_flags(val, മൂല്യങ്ങൾ, flagp, list) == FAIL)
	തിരികെ e_invalid_argument;

    തിരികെ ശൂന്യ;
}

/*
 * An option that accepts a list of string values is changed.
 * e.g. 'nrformats', 'scrollopt', 'wildoptions', etc.
 */
    സ്ഥിതം അക്ഷരം *
did_set_opt_strings(char_u *val, അക്ഷരം **മൂല്യങ്ങൾ, പൂർണ്ണസംഖ്യ list)
{
    തിരികെ did_set_opt_flags(val, മൂല്യങ്ങൾ, ശൂന്യ, list);
}

/*
 * An option which is a list of flags is set.  Valid values are in 'flags'.
 */
    സ്ഥിതം അക്ഷരം *
did_set_option_listflag(
	char_u *val,
	char_u *flags,
	അക്ഷരം *errbuf,
	size_t errbuflen)
{
    char_u	*s;

    വേണ്ടി (s = val; *s; ++s)
	എങ്കിൽ (vim_strchr(flags, *s) == ശൂന്യ)
	    തിരികെ illegal_char(errbuf, errbuflen, *s);

    തിരികെ ശൂന്യ;
}

/*
 * Expand an option that accepts a list of fixed string values with known
 * number of items.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
expand_set_opt_string(
	optexpand_T *args,
	അക്ഷരം **മൂല്യങ്ങൾ,
	size_t അക്കം_മൂല്യങ്ങൾ,
	പൂർണ്ണസംഖ്യ *അക്കം_Matches,
	char_u ***matches)
{
    char_u	*p;
    regmatch_T	*regmatch = args->oe_regmatch;
    പൂർണ്ണസംഖ്യ		include_orig_val = args->oe_include_orig_val;
    char_u	*option_val = args->oe_opt_മൂല്യം;

    // Assume numValues is small since they are fixed enums, so just allocate
    // upfront instead of needing two passes to calculate output size.
    *matches = ALLOC_MULT(char_u *, അക്കം_മൂല്യങ്ങൾ + 1);
    എങ്കിൽ (*matches == ശൂന്യ)
	തിരികെ FAIL;

    പൂർണ്ണസംഖ്യ എണ്ണം = 0;

    എങ്കിൽ (include_orig_val && *option_val != NUL)
    {
	p = vim_strsave(option_val);
	എങ്കിൽ (p == ശൂന്യ)
	{
	    VIM_CLEAR(*matches);
	    തിരികെ FAIL;
	}
	(*matches)[എണ്ണം++] = p;
    }

    വേണ്ടി (അക്ഷരം **val = മൂല്യങ്ങൾ; *val != ശൂന്യ; val++)
    {
	എങ്കിൽ (include_orig_val && *option_val != NUL)
	{
	    എങ്കിൽ (STRCMP((char_u*)*val, option_val) == 0)
		തുടരുക;
	}
	എങ്കിൽ (vim_regexec(regmatch, (char_u*)(*val), (colnr_T)0))
	{
	    p = vim_strsave((char_u*)*val);
	    എങ്കിൽ (p == ശൂന്യ)
	    {
		എങ്കിൽ (എണ്ണം == 0)
		{
		    VIM_CLEAR(*matches);
		    തിരികെ FAIL;
		}
		അല്ലെങ്കിൽ
		    നിർത്തുക;
	    }
	    (*matches)[എണ്ണം++] = p;
	}
    }
    എങ്കിൽ (എണ്ണം == 0)
    {
	VIM_CLEAR(*matches);
	തിരികെ FAIL;
    }
    *അക്കം_Matches = എണ്ണം;
    തിരികെ OK;
}

സ്ഥിതം char_u *set_opt_callback_orig_option = ശൂന്യ;
സ്ഥിതം char_u *((*set_opt_callback_func)(expand_T *, പൂർണ്ണസംഖ്യ));

/*
 * Callback used by expand_set_opt_generic to also include the original value
 * as the first item.
 */
    സ്ഥിതം char_u *
expand_set_opt_generic_cb(expand_T *xp, പൂർണ്ണസംഖ്യ idx)
{
    എങ്കിൽ (idx == 0)
    {
	എങ്കിൽ (set_opt_callback_orig_option != ശൂന്യ)
	    തിരികെ set_opt_callback_orig_option;
	അല്ലെങ്കിൽ
	    തിരികെ (char_u *)""; // empty strings are ignored
    }
    തിരികെ set_opt_callback_func(xp, idx - 1);
}

/*
 * Expand an option with a callback that iterates through a list of possible
 * names using an index.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
expand_set_opt_generic(
	optexpand_T *args,
	char_u *((*func)(expand_T *, പൂർണ്ണസംഖ്യ)),
	പൂർണ്ണസംഖ്യ *അക്കം_Matches,
	char_u ***matches)
{
    പൂർണ്ണസംഖ്യ ret;

    set_opt_callback_orig_option = args->oe_include_orig_val ?
	args->oe_opt_മൂല്യം : ശൂന്യ;
    set_opt_callback_func = func;

    ret = ExpandGeneric(
	    (char_u*)"", // not using fuzzy as currently EXPAND_STRING_SETTING doesn't use it
	    args->oe_xp,
	    args->oe_regmatch,
	    matches,
	    അക്കം_Matches,
	    expand_set_opt_generic_cb,
	    FALSE);

    set_opt_callback_orig_option = ശൂന്യ;
    set_opt_callback_func = ശൂന്യ;
    തിരികെ ret;
}

#എങ്കിൽ defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
സ്ഥിതം garray_T *expand_cb_ga;
സ്ഥിതം optexpand_T *expand_cb_args;

/*
 * Callback provided to a function in expand_set_opt_callback. Will perform
 * regex matching against the value and add to the list.
 *
 * Returns OK usually. Returns FAIL if it failed to allocate memory, and the
 * caller should terminate the enumeration.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
expand_set_opt_callback_cb(char_u *val)
{
    regmatch_T	*regmatch = expand_cb_args->oe_regmatch;
    expand_T	*xp = expand_cb_args->oe_xp;
    garray_T	*ga = expand_cb_ga;
    char_u	*str;

    എങ്കിൽ (val == ശൂന്യ || *val == NUL)
	തിരികെ OK;

    എങ്കിൽ (xp->xp_pattern[0] != NUL &&
	    !vim_regexec(regmatch, val, (colnr_T)0))
	തിരികെ OK;

    str = vim_strsave_escaped(val, (char_u *)" \t\\");

    എങ്കിൽ (str == ശൂന്യ)
	തിരികെ FAIL;

    എങ്കിൽ (ga_grow(ga, 1) == FAIL)
    {
	vim_free(str);
	തിരികെ FAIL;
    }

    ((char_u **)ga->ga_data)[ga->ga_len] = str;
    ++ga->ga_len;
    തിരികെ OK;
}

/*
 * Expand an option with a provided function that takes a callback. The
 * function will enumerate through all options and call the callback to add it
 * to the list.
 *
 * "func" is the enumerator function that will generate the list of options.
 * "func_params" is a single parameter that will be passed to func.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
expand_set_opt_callback(
	optexpand_T *args,
	ശൂന്യം (*func)(optexpand_T *, ശൂന്യം* params, പൂർണ്ണസംഖ്യ (*cb)(char_u *val)),
	ശൂന്യം *func_params,
	പൂർണ്ണസംഖ്യ *അക്കം_Matches,
	char_u ***matches)
{
    garray_T	ga;
    പൂർണ്ണസംഖ്യ		include_orig_val = args->oe_include_orig_val;
    char_u	*option_val = args->oe_opt_മൂല്യം;

    ga_init2(&ga, വലുപ്പം(അക്ഷരം *), 30);

    എങ്കിൽ (include_orig_val && *option_val != NUL)
    {
	char_u *p = vim_strsave(option_val);
	എങ്കിൽ (p == ശൂന്യ)
	    തിരികെ FAIL;
	എങ്കിൽ (ga_grow(&ga, 1) == FAIL)
	{
	    vim_free(p);
	    തിരികെ FAIL;
	}
	((char_u **)ga.ga_data)[ga.ga_len] = p;
	++ga.ga_len;
    }

    expand_cb_ga = &ga;
    expand_cb_args = args;

    func(args, func_params, expand_set_opt_callback_cb);

    expand_cb_ga = ശൂന്യ;
    expand_cb_args = ശൂന്യ;

    *matches = ga.ga_data;
    *അക്കം_Matches = ga.ga_len;
    തിരികെ OK;
}
#endif

/*
 * Expand an option which is a list of flags.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
expand_set_opt_listflag(
	optexpand_T *args,
	char_u *flags,
	പൂർണ്ണസംഖ്യ *അക്കം_Matches,
	char_u ***matches)
{
    char_u  *p;
    char_u  *option_val = args->oe_opt_മൂല്യം;
    char_u  *cmdline_val = args->oe_set_arg;
    പൂർണ്ണസംഖ്യ	    append = args->oe_append;
    പൂർണ്ണസംഖ്യ	    include_orig_val = args->oe_include_orig_val && (*option_val != NUL);

    size_t അക്കം_flags = STRLEN(flags);

    // Assume we only have small number of flags, so just allocate max size.
    *matches = ALLOC_MULT(char_u *, അക്കം_flags + 1);
    എങ്കിൽ (*matches == ശൂന്യ)
	തിരികെ FAIL;

    പൂർണ്ണസംഖ്യ എണ്ണം = 0;

    എങ്കിൽ (include_orig_val)
    {
	p = vim_strsave(option_val);
	എങ്കിൽ (p == ശൂന്യ)
	{
	    VIM_CLEAR(*matches);
	    തിരികെ FAIL;
	}
	(*matches)[എണ്ണം++] = p;
    }

    വേണ്ടി (char_u *flag = flags; *flag != NUL; flag++)
    {
	എങ്കിൽ (append && vim_strchr(option_val, *flag) != ശൂന്യ)
	    തുടരുക;

	എങ്കിൽ (vim_strchr(cmdline_val, *flag) == ശൂന്യ)
	{
	    എങ്കിൽ (include_orig_val
		    && option_val[1] == NUL
		    && *flag == option_val[0])
	    {
		// This value is already used as the first choice as it's the
		// existing flag. Just skip it to avoid duplicate.
		തുടരുക;
	    }
	    p = vim_strnsave(flag, 1);
	    എങ്കിൽ (p == ശൂന്യ)
	    {
		എങ്കിൽ (എണ്ണം == 0)
		{
		    VIM_CLEAR(*matches);
		    തിരികെ FAIL;
		}
		അല്ലെങ്കിൽ
		    നിർത്തുക;
	    }
	    (*matches)[എണ്ണം++] = p;
	}
    }

    എങ്കിൽ (എണ്ണം == 0)
    {
	VIM_CLEAR(*matches);
	തിരികെ FAIL;
    }
    *അക്കം_Matches = എണ്ണം;
    തിരികെ OK;
}

/*
 * The 'ambiwidth' option is changed.
 */
    അക്ഷരം *
did_set_ambiwidth(optset_T *args UNUSED)
{
    എങ്കിൽ (check_opt_strings(p_ambw, p_ambw_മൂല്യങ്ങൾ, FALSE) != OK)
	തിരികെ e_invalid_argument;

    തിരികെ check_chars_options();
}

// "name" must be a string literal, the length is computed at compile time.
#define completing_മൂല്യം_for_subopt(args, name) \
	  completing_മൂല്യം_for_subopt_len(args, name, (പൂർണ്ണസംഖ്യ)STRLEN_LITERAL(name))

/*
 * Return true when completing the value of the sub-option "name" with length
 * "len", e.g. the value after "close:" in 'completepopup'.
 */
    സ്ഥിതം bool
completing_മൂല്യം_for_subopt_len(optexpand_T *args, അക്ഷരം *name, പൂർണ്ണസംഖ്യ len)
{
    char_u  *colon = args->oe_xp->xp_pattern - 1;
    പൂർണ്ണസംഖ്യ	    off = (പൂർണ്ണസംഖ്യ)(colon - args->oe_set_arg);

    എങ്കിൽ (off < len)
	തിരികെ false;
    // The name must follow a comma when it does not start the option value.
    എങ്കിൽ (off > len && *(colon - len - 1) != ',')
	തിരികെ false;

    തിരികെ STRNCMP(colon - len, name, len) == 0;
}

    പൂർണ്ണസംഖ്യ
expand_set_ambiwidth(optexpand_T *args, പൂർണ്ണസംഖ്യ *അക്കം_Matches, char_u ***matches)
{
    തിരികെ expand_set_opt_string(
	    args,
	    p_ambw_മൂല്യങ്ങൾ,
	    ARRAY_LENGTH(p_ambw_മൂല്യങ്ങൾ) - 1,
	    അക്കം_Matches,
	    matches);
}

/*
 * The 'background' option is changed.
 */
    അക്ഷരം *
did_set_background(optset_T *args)
{
    എങ്കിൽ (check_opt_strings(p_bg, p_bg_മൂല്യങ്ങൾ, FALSE) == FAIL)
	തിരികെ e_invalid_argument;

    എങ്കിൽ (args->os_oldval.string != ശൂന്യ && args->os_oldval.string[0] == *p_bg)
	// Value was not changed
	തിരികെ ശൂന്യ;

#ifdef FEAT_EVAL
    പൂർണ്ണസംഖ്യ dark = (*p_bg == 'd');
#endif

    init_highlight(FALSE, FALSE);

#ifdef FEAT_EVAL
    എങ്കിൽ (dark != (*p_bg == 'd')
	    && get_var_മൂല്യം((char_u *)"g:colors_name") != ശൂന്യ)
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

    തിരികെ ശൂന്യ;
}

    പൂർണ്ണസംഖ്യ
expand_set_background(optexpand_T *args, പൂർണ്ണസംഖ്യ *അക്കം_Matches, char_u ***matches)
{
    തിരികെ expand_set_opt_string(
	    args,
	    p_bg_മൂല്യങ്ങൾ,
	    ARRAY_LENGTH(p_bg_മൂല്യങ്ങൾ) - 1,
	    അക്കം_Matches,
	    matches);
}

/*
 * The 'backspace' option is changed.
 */
    അക്ഷരം *
did_set_backspace(optset_T *args UNUSED)
{
    എങ്കിൽ (VIM_ISDIGIT(*p_bs))
    {
	എങ്കിൽ (*p_bs > '3' || p_bs[1] != NUL)
	    തിരികെ e_invalid_argument;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (check_opt_strings(p_bs, p_bs_മൂല്യങ്ങൾ, TRUE) != OK)
	തിരികെ e_invalid_argument;

    തിരികെ ശൂന്യ;
}

    പൂർണ്ണസംഖ്യ
expand_set_backspace(optexpand_T *args, പൂർണ്ണസംഖ്യ *അക്കം_Matches, char_u ***matches)
{
    തിരികെ expand_set_opt_string(
	    args,
	    p_bs_മൂല്യങ്ങൾ,
	    ARRAY_LENGTH(p_bs_മൂല്യങ്ങൾ) - 1,
	    അക്കം_Matches,
	    matches);
}

/*
 * The 'backupcopy' option is changed.
 */
    അക്ഷരം *
did_set_backupcopy(optset_T *args)
{
    char_u		*bkc = p_bkc;
    unsigned പൂർണ്ണസംഖ്യ	*flags = &bkc_flags;
    അക്ഷരം		*errmsg = ശൂന്യ;

    എങ്കിൽ (args->os_flags & OPT_LOCAL)
    {
	bkc = curbuf->b_p_bkc;
	flags = &curbuf->b_bkc_flags;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (!(args->os_flags & OPT_GLOBAL))
	// When using :set, clear the local flags.
	curbuf->b_bkc_flags = 0;

    എങ്കിൽ ((args->os_flags & OPT_LOCAL) && *bkc == NUL)
	// make the local value empty: use the global value
	*flags = 0;
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (opt_strings_flags(bkc, p_bkc_മൂല്യങ്ങൾ, flags, TRUE) != OK)
	    errmsg = e_invalid_argument;
	എങ്കിൽ ((((പൂർണ്ണസംഖ്യ)*flags & BKC_AUTO) != 0)
		+ (((പൂർണ്ണസംഖ്യ)*flags & BKC_YES) != 0)
		+ (((പൂർണ്ണസംഖ്യ)*flags & BKC_NO) != 0) != 1)
	{
	    // Must have exactly one of "auto", "yes"  and "no".
	    (ശൂന്യം)opt_strings_flags(args->os_oldval.string, p_bkc_മൂല്യങ്ങൾ,
								  flags, TRUE);
	    errmsg = e_invalid_argument;
	}
    }

    തിരികെ errmsg;
}

    പൂർണ്ണസംഖ്യ
expand_set_backupcopy(optexpand_T *args, പൂർണ്ണസംഖ്യ *അക്കം_Matches, char_u ***matches)
{
    തിരികെ expand_set_opt_string(
	    args,
	    p_bkc_മൂല്യങ്ങൾ,
	    ARRAY_LENGTH(p_bkc_മൂല്യങ്ങൾ) - 1,
	    അക്കം_Matches,
	    matches);
}

/*
 * The 'backupext' or the 'patchmode' option is changed.
 */
    അക്ഷരം *
did_set_backupext_or_patchmode(optset_T *args UNUSED)
{
    എങ്കിൽ (STRCMP(*p_bex == '.' ? p_bex + 1 : p_bex,
		*p_pm == '.' ? p_pm + 1 : p_pm) == 0)
	തിരികെ e_backupext_and_patchmode_are_equal;

    തിരികെ ശൂന്യ;
}

/*
 * The 'belloff' option is changed.
 */
    അക്ഷരം *
did_set_belloff(optset_T *args UNUSED)
{
    തിരികെ did_set_opt_flags(p_bo, p_bo_മൂല്യങ്ങൾ, &bo_flags, TRUE);
}

    പൂർണ്ണസംഖ്യ
expand_set_belloff(optexpand_T *args, പൂർണ്ണസംഖ്യ *അക്കം_Matches, char_u ***matches)
{
    തിരികെ expand_set_opt_string(
	    args,
	    p_bo_മൂല്യങ്ങൾ,
	    ARRAY_LENGTH(p_bo_മൂല്യങ്ങൾ) - 1,
	    അക്കം_Matches,
	    matches);
}

#എങ്കിൽ defined(FEAT_LINEBREAK)
/*
 * The 'breakat' option is changed.
 */
    അക്ഷരം *
did_set_breakat(optset_T *args UNUSED)
{
    char_u	*p;
    പൂർണ്ണസംഖ്യ		i;

    വേണ്ടി (i = 0; i < 256; i++)
	breakat_flags[i] = FALSE;

    എങ്കിൽ (p_breakat != ശൂന്യ)
	വേണ്ടി (p = p_breakat; *p; p++)
	    breakat_flags[*p] = TRUE;

    തിരികെ ശൂന്യ;
}

/*
 * The 'breakindentopt' option is changed.
 */
    അക്ഷരം *
did_set_breakindentopt(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    എങ്കിൽ (briopt_check(*varp, varp == &curwin->w_p_briopt ? curwin : ശൂന്യ)
								      == FAIL)
	തിരികെ e_invalid_argument;

    // list setting requires a redraw
    എങ്കിൽ (varp == &curwin->w_p_briopt && curwin->w_briopt_list)
	redraw_all_later(UPD_NOT_VALID);

    തിരികെ ശൂന്യ;
}

    പൂർണ്ണസംഖ്യ
expand_set_breakindentopt(
    optexpand_T		*args,
    പൂർണ്ണസംഖ്യ			*അക്കം_Matches,
    char_u		***matches)
{
    തിരികെ expand_set_opt_string(
	    args,
	    p_briopt_മൂല്യങ്ങൾ,
	    ARRAY_LENGTH(p_briopt_മൂല്യങ്ങൾ) - 1,
	    അക്കം_Matches,
	    matches);
}
#endif

#എങ്കിൽ defined(FEAT_BROWSE)
/*
 * The 'browsedir' option is changed.
 */
    അക്ഷരം *
did_set_browsedir(optset_T *args UNUSED)
{
    എങ്കിൽ (check_opt_strings(p_bsdir, p_bsdir_മൂല്യങ്ങൾ, FALSE) != OK
	    && !mch_isdir(p_bsdir))
	തിരികെ e_invalid_argument;

    തിരികെ ശൂന്യ;
}

    പൂർണ്ണസംഖ്യ
expand_set_browsedir(optexpand_T *args, പൂർണ്ണസംഖ്യ *അക്കം_Matches, char_u ***matches)
{
    തിരികെ expand_set_opt_string(
	    args,
	    p_bsdir_മൂല്യങ്ങൾ,
	    ARRAY_LENGTH(p_bsdir_മൂല്യങ്ങൾ) - 1,
	    അക്കം_Matches,
	    matches);
}
#endif

/*
 * The 'bufhidden' option is changed.
 */
    അക്ഷരം *
did_set_bufhidden(optset_T *args UNUSED)
{
    തിരികെ did_set_opt_strings(curbuf->b_p_bh, p_bufhidden_മൂല്യങ്ങൾ, FALSE);
}

    പൂർണ്ണസംഖ്യ
expand_set_bufhidden(optexpand_T *args, പൂർണ്ണസംഖ്യ *അക്കം_Matches, char_u ***matches)
{
    തിരികെ expand_set_opt_string(
	    args,
	    p_bufhidden_മൂല്യങ്ങൾ,
	    ARRAY_LENGTH(p_bufhidden_മൂല്യങ്ങൾ) - 1,
	    അക്കം_Matches,
	    matches);
}

/*
 * The 'buftype' option is changed.
 */
    അക്ഷരം *
did_set_buftype(optset_T *args UNUSED)
{
    എങ്കിൽ (check_opt_strings(curbuf->b_p_bt, p_buftype_മൂല്യങ്ങൾ, FALSE) != OK)
	തിരികെ e_invalid_argument;

    എങ്കിൽ (curwin->w_status_height)
    {
	curwin->w_redr_status = true;
	redraw_later(UPD_VALID);
    }
    curbuf->b_help = (curbuf->b_p_bt[0] == 'h');
    redraw_titles();

    തിരികെ ശൂന്യ;
}

    പൂർണ്ണസംഖ്യ
expand_set_buftype(optexpand_T *args, പൂർണ്ണസംഖ്യ *അക്കം_Matches, char_u ***matches)
{
    തിരികെ expand_set_opt_string(
	    args,
	    p_buftype_മൂല്യങ്ങൾ,
	    ARRAY_LENGTH(p_buftype_മൂല്യങ്ങൾ) - 1,
	    അക്കം_Matches,
	    matches);
}

/*
 * The 'casemap' option is changed.
 */
    അക്ഷരം *
did_set_casemap(optset_T *args UNUSED)
{
    തിരികെ did_set_opt_flags(p_cmp, p_cmp_മൂല്യങ്ങൾ, &cmp_flags, TRUE);
}

    പൂർണ്ണസംഖ്യ
expand_set_casemap(optexpand_T *args, പൂർണ്ണസംഖ്യ *അക്കം_Matches, char_u ***matches)
{
    തിരികെ expand_set_opt_string(
	    args,
	    p_cmp_മൂല്യങ്ങൾ,
	    ARRAY_LENGTH(p_cmp_മൂല്യങ്ങൾ) - 1,
	    അക്കം_Matches,
	    matches);
}

#എങ്കിൽ defined(HAVE_CLIPMETHOD)
    പൂർണ്ണസംഖ്യ
expand_set_clipboard(optexpand_T *args, പൂർണ്ണസംഖ്യ *അക്കം_Matches, char_u ***matches)
{
    തിരികെ expand_set_opt_string(
	    args,
	    p_cb_മൂല്യങ്ങൾ,
	    ARRAY_LENGTH(p_cb_മൂല്യങ്ങൾ) - 1,
	    അക്കം_Matches,
	    matches);
}
#endif

#ifdef HAVE_CLIPMETHOD
    അക്ഷരം *
did_set_clipmethod(optset_T *args UNUSED)
{
    തിരികെ choose_clipmethod();
}

    പൂർണ്ണസംഖ്യ
expand_set_clipmethod(optexpand_T *args, പൂർണ്ണസംഖ്യ *അക്കം_Matches, char_u ***matches)
{
    // We want to expand using the predefined clipmethod values + clipboard
    // provider names.
    പൂർണ്ണസംഖ്യ		result;
    അക്ഷരം	**മൂല്യങ്ങൾ;
    പൂർണ്ണസംഖ്യ		എണ്ണം, pos = 0, start = 0;
# ifdef FEAT_EVAL
    dict_T	*providers = get_vim_var_dict(VV_CLIPPROVIDERS);
# അല്ലെങ്കിൽ
    dict_T	*providers = ശൂന്യ;
# endif
    hashtab_T	*ht = providers == ശൂന്യ ? ശൂന്യ : &providers->dv_hashtab;

    എണ്ണം = (ht == ശൂന്യ ? 0 : ht->ht_used);
# ifdef FEAT_WAYLAND_CLIPBOARD
    എണ്ണം++;
    start++;
# endif
# ifdef FEAT_XCLIPBOARD
    എണ്ണം++;
    start++;
# endif
    മൂല്യങ്ങൾ = ALLOC_MULT(അക്ഷരം *, എണ്ണം + 1); // Add NULL terminator too

    എങ്കിൽ (മൂല്യങ്ങൾ == ശൂന്യ)
	തിരികെ FAIL;

# ifdef FEAT_WAYLAND_CLIPBOARD
    മൂല്യങ്ങൾ[pos++] = "wayland";
# endif
# ifdef FEAT_XCLIPBOARD
    മൂല്യങ്ങൾ[pos++] = "x11";
# endif

    എങ്കിൽ (ht != ശൂന്യ)
	വേണ്ടി (long_u i = 0; i < ht->ht_mask + 1; i++)
	{
	    hashitem_T	*hi = ht->ht_array + i;

	    എങ്കിൽ (!HASHITEM_EMPTY(hi))
		മൂല്യങ്ങൾ[pos++] = (അക്ഷരം *)vim_strsave(hi->hi_താക്കോൽ);
	}
    മൂല്യങ്ങൾ[pos++] = ശൂന്യ;

    result = expand_set_opt_string(
	    args,
	    മൂല്യങ്ങൾ,
	    എണ്ണം,
	    അക്കം_Matches,
	    matches);

    വേണ്ടി (പൂർണ്ണസംഖ്യ i = start; i < എണ്ണം; i++)
	vim_free(മൂല്യങ്ങൾ[i]);
    vim_free(മൂല്യങ്ങൾ);

    തിരികെ result;
}
#endif

/*
 * The global 'listchars' or 'fillchars' option is changed.
 */
    സ്ഥിതം അക്ഷരം *
did_set_global_listfillchars(char_u *val, പൂർണ്ണസംഖ്യ opt_lcs, പൂർണ്ണസംഖ്യ opt_flags,
						അക്ഷരം *errbuf, size_t errbuflen)
{
    അക്ഷരം	*errmsg = ശൂന്യ;
    char_u	**local_ptr = opt_lcs ? &curwin->w_p_lcs : &curwin->w_p_fcs;

    // only apply the global value to "curwin" when it does not have a
    // local value
    എങ്കിൽ (opt_lcs)
	errmsg = set_listchars_option(curwin, val,
		**local_ptr == NUL || !(opt_flags & OPT_GLOBAL),
							    errbuf, errbuflen);
    അല്ലെങ്കിൽ
	errmsg = set_fillchars_option(curwin, val,
		**local_ptr == NUL || !(opt_flags & OPT_GLOBAL),
							    errbuf, errbuflen);
    എങ്കിൽ (errmsg != ശൂന്യ)
	തിരികെ errmsg;

    tabpage_T	*tp;
    win_T	*wp;

    // If the current window is set to use the global
    // 'listchars'/'fillchars' value, clear the window-local value.
    എങ്കിൽ (!(opt_flags & OPT_GLOBAL))
	clear_string_option(local_ptr);
    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	// If the current window has a local value need to apply it
	// again, it was changed when setting the global value.
	// If no error was returned above, we don't expect an error
	// here, so ignore the return value.
	എങ്കിൽ (opt_lcs)
	{
	    എങ്കിൽ (*wp->w_p_lcs == NUL)
		(ശൂന്യം)set_listchars_option(wp, wp->w_p_lcs, TRUE, ശൂന്യ, 0);
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (*wp->w_p_fcs == NUL)
		(ശൂന്യം)set_fillchars_option(wp, wp->w_p_fcs, TRUE, ശൂന്യ, 0);
	}
    }

    redraw_all_later(UPD_NOT_VALID);

    തിരികെ ശൂന്യ;
}

/*
 * The 'fillchars' option or the 'listchars' option is changed.
 */
    അക്ഷരം *
did_set_chars_option(optset_T *args)
{
    char_u **varp = (char_u **)args->os_varp;
    അക്ഷരം *errmsg = ശൂന്യ;

    എങ്കിൽ (   varp == &p_lcs		// global 'listchars'
	|| varp == &p_fcs)		// global 'fillchars'
	errmsg = did_set_global_listfillchars(*varp, varp == &p_lcs,
			  args->os_flags, args->os_errbuf, args->os_errbuflen);
    അല്ലെങ്കിൽ എങ്കിൽ (varp == &curwin->w_p_lcs)	// local 'listchars'
	errmsg = set_listchars_option(curwin, *varp, TRUE,
					  args->os_errbuf, args->os_errbuflen);
    അല്ലെങ്കിൽ എങ്കിൽ (varp == &curwin->w_p_fcs)	// local 'fillchars'
	errmsg = set_fillchars_option(curwin, *varp, TRUE,
					  args->os_errbuf, args->os_errbuflen);

    തിരികെ errmsg;
}

/*
 * Expand 'fillchars' or 'listchars' option value.
 */
    പൂർണ്ണസംഖ്യ
expand_set_chars_option(optexpand_T *args, പൂർണ്ണസംഖ്യ *അക്കം_Matches, char_u ***matches)
{
    char_u **varp = (char_u **)args->oe_varp;
    പൂർണ്ണസംഖ്യ is_lcs = (varp == &p_lcs || varp == &curwin->w_p_lcs);
    തിരികെ expand_set_opt_generic(
	    args,
	    is_lcs ? get_listchars_name : get_fillchars_name,
	    അക്കം_Matches,
	    matches);
}

/*
 * The 'cinoptions' option is changed.
 */
    അക്ഷരം *
did_set_cinoptions(optset_T *args UNUSED)
{
    // TODO: recognize errors
    parse_cino(curbuf);

    തിരികെ ശൂന്യ;
}

#എങ്കിൽ defined(FEAT_SYN_HL)
/*
 * The 'colorcolumn' option is changed.
 */
    അക്ഷരം *
did_set_colorcolumn(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    തിരികെ check_colorcolumn(*varp, varp == &curwin->w_p_cc ? curwin : ശൂന്യ);
}
#endif

/*
 * The 'comments' option is changed.
 */
    അക്ഷരം *
did_set_comments(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u	*s;
    അക്ഷരം	*errmsg = ശൂന്യ;

    വേണ്ടി (s = *varp; *s; )
    {
	ആകുമ്പോൾ (*s && *s != ':')
	{
	    എങ്കിൽ (vim_strchr((char_u *)COM_ALL, *s) == ശൂന്യ
		    && !VIM_ISDIGIT(*s) && *s != '-')
	    {
		errmsg = illegal_char(args->os_errbuf, args->os_errbuflen, *s);
		നിർത്തുക;
	    }
	    ++s;
	}
	എങ്കിൽ (*s++ == NUL)
	    errmsg = e_missing_colon;
	അല്ലെങ്കിൽ എങ്കിൽ (*s == ',' || *s == NUL)
	    errmsg = e_zero_length_string;
	എങ്കിൽ (errmsg != ശൂന്യ)
	    നിർത്തുക;
	ആകുമ്പോൾ (*s && *s != ',')
	{
	    എങ്കിൽ (*s == '\\' && s[1] != NUL)
		++s;
	    ++s;
	}
	s = skip_to_option_part(s);
    }

    തിരികെ errmsg;
}

/*
 * The 'commentstring' option is changed.
 */
    അക്ഷരം *
did_set_commentstring(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    എങ്കിൽ (**varp != NUL && strstr((അക്ഷരം *)*varp, "%s") == ശൂന്യ)
	തിരികെ e_commentstring_must_be_empty_or_contain_str;

    തിരികെ ശൂന്യ;
}

/*
 * Check if value for 'complete' is valid when 'complete' option is changed.
 */
    അക്ഷരം *
did_set_complete(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u	*p, *t;
    char_u	buffer[LSIZE];
    char_u	*buf_ptr;
    char_u	char_before = NUL;
    പൂർണ്ണസംഖ്യ		escape;

    വേണ്ടി (p = *varp; *p; )
    {
	vim_memset(buffer, 0, LSIZE);
	buf_ptr = buffer;
	escape = 0;

	// Extract substring while handling escaped commas
	ആകുമ്പോൾ (*p && (*p != ',' || escape) && buf_ptr < (buffer + LSIZE - 1))
	{
	    എങ്കിൽ (*p == '\\' && *(p + 1) == ',')
	    {
		escape = 1;  // Mark escape mode
		p++;         // Skip '\'
	    }
	    അല്ലെങ്കിൽ
	    {
		escape = 0;
		*buf_ptr++ = *p;
	    }
	    p++;
	}
	*buf_ptr = NUL;

	എങ്കിൽ (vim_strchr((char_u *)".wbuksid]tUFo", *buffer) == ശൂന്യ)
	    തിരികെ illegal_char(args->os_errbuf, args->os_errbuflen, *buffer);

	എങ്കിൽ (vim_strchr((char_u *)"ksF", *buffer) == ശൂന്യ && *(buffer + 1) != NUL
		&& *(buffer + 1) != '^')
	    char_before = *buffer;
	അല്ലെങ്കിൽ
	{
	    // Test for a number after '^'
	    എങ്കിൽ ((t = vim_strchr(buffer, '^')) != ശൂന്യ)
	    {
		*t++ = NUL;
		എങ്കിൽ (!*t)
		    char_before = '^';
		അല്ലെങ്കിൽ
		{
		    വേണ്ടി (; *t; t++)
		    {
			എങ്കിൽ (!vim_isdigit(*t))
			{
			    char_before = '^';
			    നിർത്തുക;
			}
		    }
		}
	    }
	}
	എങ്കിൽ (char_before != NUL)
	    തിരികെ illegal_char_after_chr(args->os_errbuf, args->os_errbuflen,
		    char_before);
	// Skip comma and spaces
	ആകുമ്പോൾ (*p == ',' || *p == ' ')
	    p++;
    }

#ifdef FEAT_COMPL_FUNC
    എങ്കിൽ (set_cpt_callbacks(args) != OK)
	തിരികെ illegal_char_after_chr(args->os_errbuf, args->os_errbuflen,
		'F');
#endif
    തിരികെ ശൂന്യ;
}

    പൂർണ്ണസംഖ്യ
expand_set_complete(optexpand_T *args, പൂർണ്ണസംഖ്യ *അക്കം_Matches, char_u ***matches)
{
    സ്ഥിതം അക്ഷരം *(p_cpt_മൂല്യങ്ങൾ[]) = {
	".", "w", "b", "u", "k", "kspell", "s", "i", "d", "]", "t", "U", "F", "o",
	ശൂന്യ};
    തിരികെ expand_set_opt_string(
	    args,
	    p_cpt_മൂല്യങ്ങൾ,
	    ARRAY_LENGTH(p_cpt_മൂല്യങ്ങൾ) - 1,
	    അക്കം_Matches,
	    matches);
}

/*
 * The 'completeopt' option is changed.
 */
    അക്ഷരം *
did_set_completeopt(optset_T *args UNUSED)
{
    char_u	*cot = p_cot;
    unsigned	*flags = &cot_flags;

    എങ്കിൽ (args->os_flags & OPT_LOCAL)
    {
	cot = curbuf->b_p_cot;
	flags = &curbuf->b_cot_flags;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (!(args->os_flags & OPT_GLOBAL))
	// When using :set, clear the local flags.
	curbuf->b_cot_flags = 0;

    എങ്കിൽ (opt_strings_flags(cot, p_cot_മൂല്യങ്ങൾ, flags, TRUE) != OK)
	തിരികെ e_invalid_argument;

    തിരികെ ശൂന്യ;
}

    പൂർണ്ണസംഖ്യ
expand_set_completeopt(optexpand_T *args, പൂർണ്ണസംഖ്യ *അക്കം_Matches, char_u ***matches)
{
    തിരികെ expand_set_opt_string(
	    args,
	    p_cot_മൂല്യങ്ങൾ,
	    ARRAY_LENGTH(p_cot_മൂല്യങ്ങൾ) - 1,
	    അക്കം_Matches,
	    matches);
}

/*
 * The 'completefuzzycollect' option is changed.
 */
    അക്ഷരം *
did_set_completefuzzycollect(optset_T *args UNUSED)
{
    എങ്കിൽ (opt_strings_flags(p_cfc, p_cfc_മൂല്യങ്ങൾ, &cfc_flags, TRUE) != OK)
	തിരികെ e_invalid_argument;
    തിരികെ ശൂന്യ;
}

    പൂർണ്ണസംഖ്യ
expand_set_completefuzzycollect(
	optexpand_T *args,
	പൂർണ്ണസംഖ്യ *അക്കം_Matches,
	char_u ***matches)
{
    തിരികെ expand_set_opt_string(
	    args,
	    p_cfc_മൂല്യങ്ങൾ,
	    ARRAY_LENGTH(p_cfc_മൂല്യങ്ങൾ) - 1,
	    അക്കം_Matches,
	    matches);
}

/*
 * The 'completeitemalign' option is changed.
 */
    അക്ഷരം *
did_set_completeitemalign(optset_T *args UNUSED)
{
    char_u	*p = p_cia;
    unsigned	new_cia_flags = 0;
    പൂർണ്ണസംഖ്യ		seen[3] = { FALSE, FALSE, FALSE };
    പൂർണ്ണസംഖ്യ		എണ്ണം = 0;
    char_u	buf[10];

    ആകുമ്പോൾ (*p)
    {
	copy_option_part(&p, buf, വലുപ്പം(buf), ",");
	എങ്കിൽ (എണ്ണം >= 3)
	    തിരികെ e_invalid_argument;

	എങ്കിൽ (STRCMP(buf, "abbr") == 0)
	{
	    എങ്കിൽ (seen[CPT_ABBR])
		തിരികെ e_invalid_argument;
	    new_cia_flags = new_cia_flags * 10 + CPT_ABBR;
	    seen[CPT_ABBR] = TRUE;
	    എണ്ണം++;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(buf, "kind") == 0)
	{
	    എങ്കിൽ (seen[CPT_KIND])
		തിരികെ e_invalid_argument;
	    new_cia_flags = new_cia_flags * 10 + CPT_KIND;
	    seen[CPT_KIND] = TRUE;
	    എണ്ണം++;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(buf, "menu") == 0)
	{
	    എങ്കിൽ (seen[CPT_MENU])
		തിരികെ e_invalid_argument;
	    new_cia_flags = new_cia_flags * 10 + CPT_MENU;
	    seen[CPT_MENU] = TRUE;
	    എണ്ണം++;
	}
	അല്ലെങ്കിൽ
	    തിരികെ e_invalid_argument;
    }
    എങ്കിൽ (new_cia_flags == 0 || എണ്ണം != 3)
	തിരികെ e_invalid_argument;

    cia_flags = new_cia_flags;
    തിരികെ ശൂന്യ;
}

#എങ്കിൽ defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
/*
 * The 'completepopup' option is changed.
 */
    അക്ഷരം *
did_set_completepopup(optset_T *args UNUSED)
{
    എങ്കിൽ (parse_completepopup(ശൂന്യ) == FAIL)
	തിരികെ e_invalid_argument;

    popup_close_info();
    തിരികെ ശൂന്യ;
}
#endif

#എങ്കിൽ defined(BACKSLASH_IN_FILENAME)
/*
 * The 'completeslash' option is changed.
 */
    അക്ഷരം *
did_set_completeslash(optset_T *args UNUSED)
{
    എങ്കിൽ (check_opt_strings(p_csl, p_csl_മൂല്യങ്ങൾ, FALSE) != OK
	    || check_opt_strings(curbuf->b_p_csl, p_csl_മൂല്യങ്ങൾ, FALSE) != OK)
	തിരികെ e_invalid_argument;

    തിരികെ ശൂന്യ;
}

    പൂർണ്ണസംഖ്യ
expand_set_completeslash(optexpand_T *args, പൂർണ്ണസംഖ്യ *അക്കം_Matches, char_u ***matches)
{
    തിരികെ expand_set_opt_string(
	    args,
	    p_csl_മൂല്യങ്ങൾ,
	    ARRAY_LENGTH(p_csl_മൂല്യങ്ങൾ) - 1,
	    അക്കം_Matches,
	    matches);
}
#endif

#എങ്കിൽ defined(FEAT_CONCEAL)
/*
 * The 'concealcursor' option is changed.
 */
    അക്ഷരം *
did_set_concealcursor(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    തിരികെ did_set_option_listflag(*varp, (char_u *)COCU_ALL, args->os_errbuf,
		    args->os_errbuflen);
}

    പൂർണ്ണസംഖ്യ
expand_set_concealcursor(optexpand_T *args, പൂർണ്ണസംഖ്യ *അക്കം_Matches, char_u ***matches)
{
    തിരികെ expand_set_opt_listflag(args, (char_u*)COCU_ALL, അക്കം_Matches, matches);
}
#endif

/*
 * The 'cpoptions' option is changed.
 */
    അക്ഷരം *
did_set_cpoptions(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    തിരികെ did_set_option_listflag(*varp, (char_u *)CPO_ALL, args->os_errbuf,
		    args->os_errbuflen);
}

    പൂർണ്ണസംഖ്യ
expand_set_cpoptions(optexpand_T *args, പൂർണ്ണസംഖ്യ *അക്കം_Matches, char_u ***matches)
{
    തിരികെ expand_set_opt_listflag(args, (char_u*)CPO_ALL, അക്കം_Matches, matches);
}

#എങ്കിൽ defined(FEAT_CRYPT)
/*
 * The 'cryptkey' option is changed.
 */
    അക്ഷരം *
did_set_cryptkey(optset_T *args)
{
    // Make sure the ":set" command doesn't show the new value in the
    // history.
    remove_താക്കോൽ_from_history();

    എങ്കിൽ (args->os_op != OP_NONE)
	// Don't allow set+=/-=/^= as they can allow for substring guessing
	തിരികെ e_invalid_argument;

    എങ്കിൽ (STRCMP(curbuf->b_p_താക്കോൽ, args->os_oldval.string) != 0)
    {
	// Need to update the swapfile.
	ml_set_crypt_താക്കോൽ(curbuf, args->os_oldval.string,
		*curbuf->b_p_cm == NUL ? p_cm : curbuf->b_p_cm);
	changed_internal();
    }
# ifdef FEAT_SODIUM
    എങ്കിൽ (crypt_method_is_sodium(crypt_get_method_nr(curbuf)))
	crypt_sodium_lock_താക്കോൽ(args->os_newval.string);
# endif

    തിരികെ ശൂന്യ;
}

/*
 * The 'cryptmethod' option is changed.
 */
    അക്ഷരം *
did_set_cryptmethod(optset_T *args)
{
    char_u  *p;
    char_u  *s;

    എങ്കിൽ (args->os_flags & OPT_LOCAL)
	p = curbuf->b_p_cm;
    അല്ലെങ്കിൽ
	p = p_cm;
    എങ്കിൽ (check_opt_strings(p, p_cm_മൂല്യങ്ങൾ, TRUE) != OK)
	തിരികെ e_invalid_argument;
    അല്ലെങ്കിൽ എങ്കിൽ (crypt_self_test() == FAIL)
	തിരികെ e_invalid_argument;

    // When setting the global value to empty, make it "zip".
    എങ്കിൽ (*p_cm == NUL)
    {
	free_string_option(p_cm);
	p_cm = vim_strsave((char_u *)"zip");
    }
    // When using ":set cm=name" the local value is going to be empty.
    // Do that here, otherwise the crypt functions will still use the
    // local value.
    എങ്കിൽ ((args->os_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	free_string_option(curbuf->b_p_cm);
	curbuf->b_p_cm = empty_option;
    }

    // Need to update the swapfile when the effective method changed.
    // Set "s" to the effective old value, "p" to the effective new
    // method and compare.
    എങ്കിൽ ((args->os_flags & OPT_LOCAL) && *args->os_oldval.string == NUL)
	s = p_cm;  // was previously using the global value
    അല്ലെങ്കിൽ
	s = args->os_oldval.string;
    എങ്കിൽ (*curbuf->b_p_cm == NUL)
	p = p_cm;  // is now using the global value
    അല്ലെങ്കിൽ
	p = curbuf->b_p_cm;
    എങ്കിൽ (STRCMP(s, p) != 0)
	ml_set_crypt_താക്കോൽ(curbuf, curbuf->b_p_താക്കോൽ, s);

    // If the global value changes need to update the swapfile for all
    // buffers using that value.
    എങ്കിൽ ((args->os_flags & OPT_GLOBAL)
	    && STRCMP(p_cm, args->os_oldval.string) != 0)
    {
	buf_T	*buf;

	FOR_ALL_BUFFERS(buf)
	    എങ്കിൽ (buf != curbuf && *buf->b_p_cm == NUL)
		ml_set_crypt_താക്കോൽ(buf, buf->b_p_താക്കോൽ, args->os_oldval.string);
    }
    തിരികെ ശൂന്യ;
}

    പൂർണ്ണസംഖ്യ
expand_set_cryptmethod(optexpand_T *args, പൂർണ്ണസംഖ്യ *അക്കം_Matches, char_u ***matches)
{
    തിരികെ expand_set_opt_string(
	    args,
	    p_cm_മൂല്യങ്ങൾ,
	    ARRAY_LENGTH(p_cm_മൂല്യങ്ങൾ) - 1,
	    അക്കം_Matches,
	    matches);
}
#endif

#എങ്കിൽ defined(FEAT_CSCOPE) && defined(FEAT_QUICKFIX)
/*
 * The 'cscopequickfix' option is changed.
 */
    അക്ഷരം *
did_set_cscopequickfix(optset_T *args UNUSED)
{
    char_u	*p;

    എങ്കിൽ (p_csqf == ശൂന്യ)
	തിരികെ ശൂന്യ;

    p = p_csqf;
    ആകുമ്പോൾ (*p != NUL)
    {
	എങ്കിൽ (vim_strchr((char_u *)CSQF_CMDS, *p) == ശൂന്യ
		|| p[1] == NUL
		|| vim_strchr((char_u *)CSQF_FLAGS, p[1]) == ശൂന്യ
		|| (p[2] != NUL && p[2] != ','))
	    തിരികെ e_invalid_argument;
	അല്ലെങ്കിൽ എങ്കിൽ (p[2] == NUL)
	    നിർത്തുക;
	അല്ലെങ്കിൽ
	    p += 3;
    }

    തിരികെ ശൂന്യ;
}
#endif

#എങ്കിൽ defined(FEAT_SYN_HL)
/*
 * The 'cursorlineopt' option is changed.
 */
    അക്ഷരം *
did_set_cursorlineopt(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    // This could be changed to use opt_strings_flags() instead.
    എങ്കിൽ (**varp == NUL || fill_culopt_flags(*varp, curwin) != OK)
	തിരികെ e_invalid_argument;

    തിരികെ ശൂന്യ;
}

    പൂർണ്ണസംഖ്യ
expand_set_cursorlineopt(optexpand_T *args, പൂർണ്ണസംഖ്യ *അക്കം_Matches, char_u ***matches)
{
    തിരികെ expand_set_opt_string(
	    args,
	    p_culopt_മൂല്യങ്ങൾ,
	    ARRAY_LENGTH(p_culopt_മൂല്യങ്ങൾ) - 1,
	    അക്കം_Matches,
	    matches);
}
#endif

/*
 * The 'debug' option is changed.
 */
    അക്ഷരം *
did_set_debug(optset_T *args UNUSED)
{
    തിരികെ did_set_opt_strings(p_debug, p_debug_മൂല്യങ്ങൾ, TRUE);
}

    പൂർണ്ണസംഖ്യ
expand_set_debug(optexpand_T *args, പൂർണ്ണസംഖ്യ *അക്കം_Matches, char_u ***matches)
{
    തിരികെ expand_set_opt_string(
	    args,
	    p_debug_മൂല്യങ്ങൾ,
	    ARRAY_LENGTH(p_debug_മൂല്യങ്ങൾ) - 1,
	    അക്കം_Matches,
	    matches);
}

#എങ്കിൽ defined(FEAT_DIFF)
/*
 * The 'diffanchors' option is changed.
 */
    അക്ഷരം *
did_set_diffanchors(optset_T *args)
{
    എങ്കിൽ (diffanchors_changed(args->os_flags & OPT_LOCAL) == FAIL)
	തിരികെ e_invalid_argument;

    തിരികെ ശൂന്യ;
}

/*
 * The 'diffopt' option is changed.
 */
    അക്ഷരം *
did_set_diffopt(optset_T *args UNUSED)
{
    എങ്കിൽ (diffopt_changed() == FAIL)
	തിരികെ e_invalid_argument;

    തിരികെ ശൂന്യ;
}

    പൂർണ്ണസംഖ്യ
expand_set_diffopt(optexpand_T *args, പൂർണ്ണസംഖ്യ *അക്കം_Matches, char_u ***matches)
{
    expand_T *xp = args->oe_xp;

    എങ്കിൽ (xp->xp_pattern > args->oe_set_arg && *(xp->xp_pattern-1) == ':')
    {
	എങ്കിൽ (completing_മൂല്യം_for_subopt(args, "algorithm"))
	{
	    തിരികെ expand_set_opt_string(
		    args,
		    p_dip_algorithm_മൂല്യങ്ങൾ,
		    ARRAY_LENGTH(p_dip_algorithm_മൂല്യങ്ങൾ) - 1,
		    അക്കം_Matches,
		    matches);
	}
	എങ്കിൽ (completing_മൂല്യം_for_subopt(args, "inline"))
	{
	    തിരികെ expand_set_opt_string(
		    args,
		    p_dip_inline_മൂല്യങ്ങൾ,
		    ARRAY_LENGTH(p_dip_inline_മൂല്യങ്ങൾ) - 1,
		    അക്കം_Matches,
		    matches);
	}
	തിരികെ FAIL;
    }

    തിരികെ expand_set_opt_string(
	    args,
	    p_dip_മൂല്യങ്ങൾ,
	    ARRAY_LENGTH(p_dip_മൂല്യങ്ങൾ) - 1,
	    അക്കം_Matches,
	    matches);
}
#endif

/*
 * The 'display' option is changed.
 */
    അക്ഷരം *
did_set_display(optset_T *args UNUSED)
{
    എങ്കിൽ (opt_strings_flags(p_dy, p_dy_മൂല്യങ്ങൾ, &dy_flags, TRUE) != OK)
	തിരികെ e_invalid_argument;

    (ശൂന്യം)init_chartab();
    തിരികെ ശൂന്യ;
}

    പൂർണ്ണസംഖ്യ
expand_set_display(optexpand_T *args, പൂർണ്ണസംഖ്യ *അക്കം_Matches, char_u ***matches)
{
    തിരികെ expand_set_opt_string(
	    args,
	    p_dy_മൂല്യങ്ങൾ,
	    ARRAY_LENGTH(p_dy_മൂല്യങ്ങൾ) - 1,
	    അക്കം_Matches,
	    matches);
}

/*
 * The 'eadirection' option is changed.
 */
    അക്ഷരം *
did_set_eadirection(optset_T *args UNUSED)
{
    തിരികെ did_set_opt_strings(p_ead, p_ead_മൂല്യങ്ങൾ, FALSE);
}

    പൂർണ്ണസംഖ്യ
expand_set_eadirection(optexpand_T *args, പൂർണ്ണസംഖ്യ *അക്കം_Matches, char_u ***matches)
{
    തിരികെ expand_set_opt_string(
	    args,
	    p_ead_മൂല്യങ്ങൾ,
	    ARRAY_LENGTH(p_ead_മൂല്യങ്ങൾ) - 1,
	    അക്കം_Matches,
	    matches);
}

/*
 * One of the 'encoding', 'fileencoding', 'termencoding' or 'makeencoding'
 * options is changed.
 */
    അക്ഷരം *
did_set_encoding(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u	**gvarp;
    അക്ഷരം	*errmsg = ശൂന്യ;
    char_u	*p;

    // Get the global option to compare with, otherwise we would have to check
    // two values for all local options.
    gvarp = (char_u **)get_option_varp_scope(args->os_idx, OPT_GLOBAL);

    എങ്കിൽ (gvarp == &p_fenc)
    {
	എങ്കിൽ (!curbuf->b_p_ma && args->os_flags != OPT_GLOBAL)
	    errmsg = e_cannot_make_changes_modifiable_is_off;
	അല്ലെങ്കിൽ എങ്കിൽ (vim_strchr(*varp, ',') != ശൂന്യ)
	    // No comma allowed in 'fileencoding'; catches confusing it
	    // with 'fileencodings'.
	    errmsg = e_invalid_argument;
	അല്ലെങ്കിൽ
	{
	    // May show a "+" in the title now.
	    redraw_titles();
	    // Add 'fileencoding' to the swap file.
	    ml_setflags(curbuf);
	}
    }
    എങ്കിൽ (errmsg == ശൂന്യ)
    {
	// canonize the value, so that STRCMP() can be used on it
	p = enc_canonize(*varp);
	എങ്കിൽ (p != ശൂന്യ)
	{
	    vim_free(*varp);
	    *varp = p;
	}
	എങ്കിൽ (varp == &p_enc)
	{
	    errmsg = mb_init();
	    redraw_titles();
	}
    }

#എങ്കിൽ defined(FEAT_GUI_GTK)
    എങ്കിൽ (errmsg == ശൂന്യ && varp == &p_tenc && gui.in_use)
    {
	// GTK uses only a single encoding, and that is UTF-8.
	എങ്കിൽ (STRCMP(p_tenc, "utf-8") != 0)
	    errmsg = e_cannot_be_changed_in_gtk_GUI;
    }
#endif

    എങ്കിൽ (errmsg == ശൂന്യ)
    {
#ifdef FEAT_KEYMAP
	// When 'keymap' is used and 'encoding' changes, reload the keymap
	// (with another encoding).
	എങ്കിൽ (varp == &p_enc && *curbuf->b_p_keymap != NUL)
	    (ശൂന്യം)keymap_init();
#endif

	// When 'termencoding' is not empty and 'encoding' changes or when
	// 'termencoding' changes, need to setup for keyboard input and
	// display output conversion.
	എങ്കിൽ (((varp == &p_enc && *p_tenc != NUL) || varp == &p_tenc))
	{
	    എങ്കിൽ (convert_setup(&നിവേശം_conv, p_tenc, p_enc) == FAIL
		    || convert_setup(&output_conv, p_enc, p_tenc) == FAIL)
	    {
		semsg(_(e_cannot_convert_between_str_and_str),
			p_tenc, p_enc);
		errmsg = e_invalid_argument;
	    }
	}

#എങ്കിൽ defined(MSWIN)
	// $HOME, $VIM and $VIMRUNTIME may have characters in active code page.
	എങ്കിൽ (varp == &p_enc)
	{
	    init_homedir();
	    init_vimdir();
	}
#endif
    }

    തിരികെ errmsg;
}

    പൂർണ്ണസംഖ്യ
expand_set_encoding(optexpand_T *args, പൂർണ്ണസംഖ്യ *അക്കം_Matches, char_u ***matches)
{
    തിരികെ expand_set_opt_generic(
	    args,
	    get_encoding_name,
	    അക്കം_Matches,
	    matches);
}

/*
 * The 'eventignore(win)' option is changed.
 */
    അക്ഷരം *
did_set_eventignore(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u	*oldval = args->os_oldval.string;

    എങ്കിൽ (check_ei(*varp) == FAIL)
	തിരികെ e_invalid_argument;

    എങ്കിൽ (oldval == ശൂന്യ || STRCMP(oldval, *varp) == 0)
	തിരികെ ശൂന്യ;

    // Deal with the events that are triggered by comparing against a stored
    // state, with the old value in effect: what happened while an event was
    // ignored must not be reported once it is not ignored anymore, and what
    // happened before must still be reported.
    // Use a copy, the caller owns "oldval" and autocommands may free it.
    char_u	*save_ei = vim_strsave(oldval);
    എങ്കിൽ (save_ei != ശൂന്യ)
    {
	char_u	*newval = *varp;
	win_T	*wp = is_window_local_option(args->os_idx) ? curwin : ശൂന്യ;

	*varp = save_ei;
	// "varp" points into "wp" for 'eventignorewin'.
	എങ്കിൽ (wp != ശൂന്യ)
	    ++wp->w_locked;
	may_trigger_deferred_events();
	എങ്കിൽ (wp != ശൂന്യ)
	    --wp->w_locked;

	free_string_option(*varp);
	*varp = newval;
    }

    തിരികെ ശൂന്യ;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ expand_eiw = FALSE;

    സ്ഥിതം char_u *
get_eventignore_name(expand_T *xp, പൂർണ്ണസംഖ്യ idx)
{
    പൂർണ്ണസംഖ്യ subtract = *xp->xp_pattern == '-';
    // 'eventignore(win)' allows special keyword "all" in addition to
    // all event names.
    എങ്കിൽ (!subtract && idx == 0)
	തിരികെ (char_u *)"all";

    char_u *name = get_event_name_no_group(xp, idx - 1 + subtract, expand_eiw);
    എങ്കിൽ (name == ശൂന്യ)
	തിരികെ ശൂന്യ;

    sprintf((അക്ഷരം *)IObuff, "%s%s", subtract ? "-" : "", name);
    തിരികെ IObuff;
}

    പൂർണ്ണസംഖ്യ
expand_set_eventignore(optexpand_T *args, പൂർണ്ണസംഖ്യ *അക്കം_Matches, char_u ***matches)
{
    expand_eiw = args->oe_varp != (char_u *)&p_ei;
    തിരികെ expand_set_opt_generic(
	    args,
	    get_eventignore_name,
	    അക്കം_Matches,
	    matches);
}

/*
 * The 'fileformat' option is changed.
 */
    അക്ഷരം *
did_set_fileformat(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    എങ്കിൽ (!curbuf->b_p_ma && !(args->os_flags & OPT_GLOBAL))
	തിരികെ e_cannot_make_changes_modifiable_is_off;
    അല്ലെങ്കിൽ എങ്കിൽ (check_opt_strings(*varp, p_ff_മൂല്യങ്ങൾ, FALSE) != OK)
	തിരികെ e_invalid_argument;

    // may also change 'textmode'
    എങ്കിൽ (get_fileformat(curbuf) == EOL_DOS)
	curbuf->b_p_tx = TRUE;
    അല്ലെങ്കിൽ
	curbuf->b_p_tx = FALSE;
    redraw_titles();
    // update flag in swap file
    ml_setflags(curbuf);
    // Redraw needed when switching to/from "mac": a CR in the text
    // will be displayed differently.
    എങ്കിൽ (get_fileformat(curbuf) == EOL_MAC || *args->os_oldval.string == 'm')
	redraw_curbuf_later(UPD_NOT_VALID);

    തിരികെ ശൂന്യ;
}

    പൂർണ്ണസംഖ്യ
expand_set_fileformat(optexpand_T *args, പൂർണ്ണസംഖ്യ *അക്കം_Matches, char_u ***matches)
{
    തിരികെ expand_set_opt_string(
	    args,
	    p_ff_മൂല്യങ്ങൾ,
	    ARRAY_LENGTH(p_ff_മൂല്യങ്ങൾ) - 1,
	    അക്കം_Matches,
	    matches);
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * fileformat options.
 */
    char_u *
get_fileformat_name(expand_T *xp UNUSED, പൂർണ്ണസംഖ്യ idx)
{
    എങ്കിൽ (idx >= (പൂർണ്ണസംഖ്യ)ARRAY_LENGTH(p_ff_മൂല്യങ്ങൾ))
	തിരികെ ശൂന്യ;

    തിരികെ (char_u*)p_ff_മൂല്യങ്ങൾ[idx];
}

/*
 * The 'fileformats' option is changed.
 */
    അക്ഷരം *
did_set_fileformats(optset_T *args UNUSED)
{
    എങ്കിൽ (check_opt_strings(p_ffs, p_ff_മൂല്യങ്ങൾ, TRUE) != OK)
	തിരികെ e_invalid_argument;

    // also change 'textauto'
    എങ്കിൽ (*p_ffs == NUL)
	p_ta = FALSE;
    അല്ലെങ്കിൽ
	p_ta = TRUE;

    തിരികെ ശൂന്യ;
}

/*
 * The 'filetype' or the 'syntax' option is changed.
 */
    അക്ഷരം *
did_set_filetype_or_syntax(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    എങ്കിൽ (!valid_filetype(*varp))
	തിരികെ e_invalid_argument;

    args->os_മൂല്യം_changed = STRCMP(args->os_oldval.string, *varp) != 0;

    // Since we check the value, there is no need to set P_INSECURE,
    // even when the value comes from a modeline.
    args->os_മൂല്യം_checked = TRUE;

    തിരികെ ശൂന്യ;
}

#എങ്കിൽ defined(FEAT_FOLDING)
/*
 * The 'foldclose' option is changed.
 */
    അക്ഷരം *
did_set_foldclose(optset_T *args UNUSED)
{
    തിരികെ did_set_opt_strings(p_fcl, p_fcl_മൂല്യങ്ങൾ, TRUE);
}

    പൂർണ്ണസംഖ്യ
expand_set_foldclose(optexpand_T *args, പൂർണ്ണസംഖ്യ *അക്കം_Matches, char_u ***matches)
{
    തിരികെ expand_set_opt_string(
	    args,
	    p_fcl_മൂല്യങ്ങൾ,
	    ARRAY_LENGTH(p_fcl_മൂല്യങ്ങൾ) - 1,
	    അക്കം_Matches,
	    matches);
}
#endif

#എങ്കിൽ defined(FEAT_EVAL) && defined(FEAT_FOLDING)
/*
 * The 'foldexpr' option is changed.
 */
    അക്ഷരം *
did_set_foldexpr(optset_T *args)
{
    (ശൂന്യം)did_set_optexpr(args);
    എങ്കിൽ (foldmethod_Is_വ്യഞ്ജകം(curwin))
	foldUpdateAll(curwin);
    തിരികെ ശൂന്യ;
}
#endif

#എങ്കിൽ defined(FEAT_FOLDING)
/*
 * The 'foldignore' option is changed.
 */
    അക്ഷരം *
did_set_foldignore(optset_T *args UNUSED)
{
    എങ്കിൽ (foldmethodIsIndent(curwin))
	foldUpdateAll(curwin);
    തിരികെ ശൂന്യ;
}

/*
 * The 'foldmarker' option is changed.
 */
    അക്ഷരം *
did_set_foldmarker(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u	*p;

    p = vim_strchr(*varp, ',');
    എങ്കിൽ (p == ശൂന്യ)
	തിരികെ e_comma_required;
    അല്ലെങ്കിൽ എങ്കിൽ (p == *varp || p[1] == NUL)
	തിരികെ e_invalid_argument;
    അല്ലെങ്കിൽ എങ്കിൽ (foldmethodIsMarker(curwin))
	foldUpdateAll(curwin);

    തിരികെ ശൂന്യ;
}

/*
 * The 'foldmethod' option is changed.
 */
    അക്ഷരം *
did_set_foldmethod(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    എങ്കിൽ (check_opt_strings(*varp, p_fdm_മൂല്യങ്ങൾ, FALSE) != OK || **varp == NUL)
	തിരികെ e_invalid_argument;

    foldUpdateAll(curwin);
    എങ്കിൽ (foldmethodIsDiff(curwin))
	newFoldLevel();
    തിരികെ ശൂന്യ;
}

    പൂർണ്ണസംഖ്യ
expand_set_foldmethod(optexpand_T *args, പൂർണ്ണസംഖ്യ *അക്കം_Matches, char_u ***matches)
{
    തിരികെ expand_set_opt_string(
	    args,
	    p_fdm_മൂല്യങ്ങൾ,
	    ARRAY_LENGTH(p_fdm_മൂല്യങ്ങൾ) - 1,
	    അക്കം_Matches,
	    matches);
}

/*
 * The 'foldopen' option is changed.
 */
    അക്ഷരം *
did_set_foldopen(optset_T *args UNUSED)
{
    തിരികെ did_set_opt_flags(p_fdo, p_fdo_മൂല്യങ്ങൾ, &fdo_flags, TRUE);
}

    പൂർണ്ണസംഖ്യ
expand_set_foldopen(optexpand_T *args, പൂർണ്ണസംഖ്യ *അക്കം_Matches, char_u ***matches)
{
    തിരികെ expand_set_opt_string(
	    args,
	    p_fdo_മൂല്യങ്ങൾ,
	    ARRAY_LENGTH(p_fdo_മൂല്യങ്ങൾ) - 1,
	    അക്കം_Matches,
	    matches);
}
#endif

/*
 * The 'formatoptions' option is changed.
 */
    അക്ഷരം *
did_set_formatoptions(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    തിരികെ did_set_option_listflag(*varp, (char_u *)FO_ALL, args->os_errbuf,
		    args->os_errbuflen);
}

    പൂർണ്ണസംഖ്യ
expand_set_formatoptions(optexpand_T *args, പൂർണ്ണസംഖ്യ *അക്കം_Matches, char_u ***matches)
{
    തിരികെ expand_set_opt_listflag(args, (char_u*)FO_ALL, അക്കം_Matches, matches);
}

#എങ്കിൽ defined(CURSOR_SHAPE)
/*
 * The 'guicursor' option is changed.
 */
    അക്ഷരം *
did_set_guicursor(optset_T *args UNUSED)
{
    തിരികെ parse_shape_opt(SHAPE_CURSOR);
}
#endif

#എങ്കിൽ defined(FEAT_GUI)
/*
 * The 'guifont' option is changed.
 */
    അക്ഷരം *
did_set_guifont(optset_T *args UNUSED)
{
    char_u	*p;
    അക്ഷരം	*errmsg = ശൂന്യ;

    എങ്കിൽ (gui.in_use)
    {
	p = p_guifont;
# എങ്കിൽ defined(FEAT_GUI_GTK)
	// Put up a font dialog and let the user select a new value.
	// If this is cancelled go back to the old value but don't
	// give an error message.
	എങ്കിൽ (STRCMP(p, "*") == 0)
	{
	    p = gui_mch_font_dialog(args->os_oldval.string);
	    free_string_option(p_guifont);
	    p_guifont = (p != ശൂന്യ) ? p : vim_strsave(args->os_oldval.string);
	}
# endif
	എങ്കിൽ (p != ശൂന്യ && gui_init_font(p_guifont, FALSE) != OK)
	{
# എങ്കിൽ defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_PHOTON)
	    എങ്കിൽ (STRCMP(p_guifont, "*") == 0)
	    {
		// Dialog was cancelled: Keep the old value without giving
		// an error message.
		free_string_option(p_guifont);
		p_guifont = vim_strsave(args->os_oldval.string);
	    }
	    അല്ലെങ്കിൽ
# endif
		errmsg = e_invalid_fonts;
	}
    }

    തിരികെ errmsg;
}

/*
 * Expand the 'guifont' option. Only when GUI is being used. Each platform has
 * specific behaviors.
 */
    പൂർണ്ണസംഖ്യ
expand_set_guifont(optexpand_T *args, പൂർണ്ണസംഖ്യ *അക്കം_Matches, char_u ***matches)
{
    എങ്കിൽ (!gui.in_use)
	തിരികെ FAIL;

# എങ്കിൽ defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
    char_u **varp = (char_u **)args->oe_varp;
    പൂർണ്ണസംഖ്യ wide = (varp == &p_guifontwide);

    തിരികെ expand_set_opt_callback(
	    args, gui_mch_expand_font, &wide, അക്കം_Matches, matches);
# അല്ലെങ്കിൽ
    തിരികെ FAIL;
# endif
}

# എങ്കിൽ defined(FEAT_XFONTSET)
/*
 * The 'guifontset' option is changed.
 */
    അക്ഷരം *
did_set_guifontset(optset_T *args UNUSED)
{
    അക്ഷരം *errmsg = ശൂന്യ;

    എങ്കിൽ (STRCMP(p_guifontset, "*") == 0)
	errmsg = e_cant_select_fontset;
    അല്ലെങ്കിൽ എങ്കിൽ (gui.in_use && gui_init_font(p_guifontset, TRUE) != OK)
	errmsg = e_invalid_fontset;

    തിരികെ errmsg;
}
# endif

/*
 * The 'guifontwide' option is changed.
 */
    അക്ഷരം *
did_set_guifontwide(optset_T *args UNUSED)
{
    അക്ഷരം *errmsg = ശൂന്യ;

    എങ്കിൽ (STRCMP(p_guifontwide, "*") == 0)
	errmsg = e_cant_select_wide_font;
    അല്ലെങ്കിൽ എങ്കിൽ (gui_get_wide_font() == FAIL)
	errmsg = e_invalid_wide_font;

    തിരികെ errmsg;
}
#endif

#എങ്കിൽ defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
/*
 * The 'guiligatures' option is changed.
 */
    അക്ഷരം *
did_set_guiligatures(optset_T *args UNUSED)
{
    gui_set_ligatures();
    തിരികെ ശൂന്യ;
}
#endif

#എങ്കിൽ defined(FEAT_GUI)
/*
 * The 'guioptions' option is changed.
 */
    അക്ഷരം *
did_set_guioptions(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    അക്ഷരം *errmsg;

    errmsg = did_set_option_listflag(*varp, (char_u *)GO_ALL, args->os_errbuf,
		    args->os_errbuflen);
    എങ്കിൽ (errmsg != ശൂന്യ)
	തിരികെ errmsg;

    gui_init_which_components(args->os_oldval.string);
    തിരികെ ശൂന്യ;
}

    പൂർണ്ണസംഖ്യ
expand_set_guioptions(optexpand_T *args, പൂർണ്ണസംഖ്യ *അക്കം_Matches, char_u ***matches)
{
    തിരികെ expand_set_opt_listflag(args, (char_u*)GO_ALL, അക്കം_Matches, matches);
}
#endif

#എങ്കിൽ defined(FEAT_GUI_TABLINE)
/*
 * The 'guitablabel' option is changed.
 */
    അക്ഷരം *
did_set_guitablabel(optset_T *args UNUSED)
{
    redraw_tabline = TRUE;
    തിരികെ ശൂന്യ;
}
#endif

/*
 * The 'helpfile' option is changed.
 */
    അക്ഷരം *
did_set_helpfile(optset_T *args UNUSED)
{
    // May compute new values for $VIM and $VIMRUNTIME
    എങ്കിൽ (didset_vim)
	vim_unsetenv_ext((char_u *)"VIM");
    എങ്കിൽ (didset_vimruntime)
	vim_unsetenv_ext((char_u *)"VIMRUNTIME");
    തിരികെ ശൂന്യ;
}

#എങ്കിൽ defined(FEAT_MULTI_LANG)
/*
 * The 'helplang' option is changed.
 */
    അക്ഷരം *
did_set_helplang(optset_T *args UNUSED)
{
    അക്ഷരം *errmsg = ശൂന്യ;

    // Check for "", "ab", "ab,cd", etc.
    വേണ്ടി (char_u *s = p_hlg; *s != NUL; s += 3)
    {
	എങ്കിൽ (s[1] == NUL || ((s[2] != ',' || s[3] == NUL) && s[2] != NUL))
	{
	    errmsg = e_invalid_argument;
	    നിർത്തുക;
	}
	എങ്കിൽ (s[2] == NUL)
	    നിർത്തുക;
    }

    തിരികെ errmsg;
}
#endif

/*
 * The 'highlight' option is changed.
 */
    അക്ഷരം *
did_set_highlight(optset_T *args UNUSED)
{
    എങ്കിൽ (highlight_changed() == FAIL)
	തിരികെ e_invalid_argument;	// invalid flags

    തിരികെ ശൂന്യ;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
expand_hl_occasions(
	optexpand_T *args,
	പൂർണ്ണസംഖ്യ *അക്കം_Matches,
	char_u ***matches,
	char_u prefix)
{
    char_u	    *p;
    സ്ഥിതം char_u   hl_flags[HLF_എണ്ണം] = HL_FLAGS;
    size_t	    i;
    പൂർണ്ണസംഖ്യ		    എണ്ണം = 0;

    *matches = ALLOC_MULT(char_u *, HLF_എണ്ണം + 1);
    എങ്കിൽ (*matches == ശൂന്യ)
	തിരികെ FAIL;

    // We still want to return the full option if it's requested.
    എങ്കിൽ (args->oe_include_orig_val)
    {
	p = vim_strsave(args->oe_opt_മൂല്യം);
	എങ്കിൽ (p == ശൂന്യ)
	{
	    VIM_CLEAR(*matches);
	    തിരികെ FAIL;
	}
	(*matches)[എണ്ണം++] = p;
    }

    വേണ്ടി (i = 0; i < HLF_എണ്ണം; i++)
    {
	p = alloc((prefix == NUL ? 1 : 2) + 1);
	എങ്കിൽ (p == ശൂന്യ)
	{
	    എങ്കിൽ (എണ്ണം == 0)
	    {
		VIM_CLEAR(*matches);
		തിരികെ FAIL;
	    }
	    അല്ലെങ്കിൽ
		നിർത്തുക;
	}
	എങ്കിൽ (prefix == NUL)
	    sprintf((അക്ഷരം *)p, "%c", hl_flags[i]);
	അല്ലെങ്കിൽ
	    sprintf((അക്ഷരം *)p, "%c%c", prefix, hl_flags[i]);
	(*matches)[എണ്ണം++] = p;
    }

    എങ്കിൽ (എണ്ണം == 0)
    {
	VIM_CLEAR(*matches);
	തിരികെ FAIL;
    }
    *അക്കം_Matches = എണ്ണം;
    തിരികെ OK;
}

/*
 * Expand 'highlight' option.
 */
    പൂർണ്ണസംഖ്യ
expand_set_highlight(optexpand_T *args, പൂർണ്ണസംഖ്യ *അക്കം_Matches, char_u ***matches)
{
    char_u	    *p;
    expand_T	    *xp = args->oe_xp;
    size_t	    i;
    പൂർണ്ണസംഖ്യ		    എണ്ണം = 0;

    എങ്കിൽ (xp->xp_pattern > args->oe_set_arg && *(xp->xp_pattern-1) == ':')
    {
	// Right after a ':', meaning we just return all highlight names.
	തിരികെ expand_set_opt_generic(
		args,
		get_highlight_name,
		അക്കം_Matches,
		matches);
    }

    എങ്കിൽ (*xp->xp_pattern == NUL)
	// At beginning of a comma-separated list. Return the specific list of
	// supported occasions.
	തിരികെ expand_hl_occasions(args, അക്കം_Matches, matches, NUL);

    // We are after the initial character (which indicates the occasion). We
    // already made sure we are not matching after a ':' above, so now we want
    // to match against display mode modifiers.
    // Since the xp_pattern starts from the beginning, we need to include it in
    // the returned match.

    // Note: Keep this in sync with highlight_changed()
    സ്ഥിതം char_u p_hl_mode_മൂല്യങ്ങൾ[] =
	{':', 'b', 'i', '-', 'n', 'r', 's', 'u', 'c', '2', 'd', '=', 't'};
    size_t അക്കം_hl_modes = ARRAY_LENGTH(p_hl_mode_മൂല്യങ്ങൾ);

    *matches = ALLOC_MULT(char_u *, അക്കം_hl_modes);
    എങ്കിൽ (*matches == ശൂന്യ)
	തിരികെ FAIL;

    പൂർണ്ണസംഖ്യ pattern_len = xp->xp_pattern_len;

    വേണ്ടി (i = 0; i < അക്കം_hl_modes; i++)
    {
	// Don't allow duplicates as these are unique flags
	char_u *dup = vim_strchr(xp->xp_pattern + 1, p_hl_mode_മൂല്യങ്ങൾ[i]);
	എങ്കിൽ (dup != ശൂന്യ && (പൂർണ്ണസംഖ്യ)(dup - xp->xp_pattern) < pattern_len)
	    തുടരുക;

	// ':' only works by itself, not with other flags.
	എങ്കിൽ (pattern_len > 1 && p_hl_mode_മൂല്യങ്ങൾ[i] == ':')
	    തുടരുക;

	p = vim_strnsave(xp->xp_pattern, pattern_len + 1);
	എങ്കിൽ (p == ശൂന്യ)
	{
	    എങ്കിൽ (i == 0)
	    {
		VIM_CLEAR(*matches);
		തിരികെ FAIL;
	    }
	    അല്ലെങ്കിൽ
		നിർത്തുക;
	}
	p[pattern_len] = p_hl_mode_മൂല്യങ്ങൾ[i];
	p[pattern_len + 1] = NUL;
	(*matches)[എണ്ണം++] = p;
    }
    എങ്കിൽ (എണ്ണം == 0)
    {
	VIM_CLEAR(*matches);
	തിരികെ FAIL;
    }
    *അക്കം_Matches = എണ്ണം;

    തിരികെ OK;
}

/*
 * The 'titlestring' or the 'iconstring' option is changed.
 */
    സ്ഥിതം അക്ഷരം *
parse_titleiconstring(optset_T *args UNUSED, പൂർണ്ണസംഖ്യ flagval UNUSED)
{
#ifdef FEAT_STL_OPT
    char_u	**varp = (char_u **)args->os_varp;

    // NULL => statusline syntax
    എങ്കിൽ (vim_strchr(*varp, '%') && check_stl_option(*varp) == ശൂന്യ)
	stl_syntax |= flagval;
    അല്ലെങ്കിൽ
	stl_syntax &= ~flagval;
#endif
    did_set_title();

    തിരികെ ശൂന്യ;
}

/*
 * The 'iconstring' option is changed.
 */
    അക്ഷരം *
did_set_iconstring(optset_T *args)
{
    പൂർണ്ണസംഖ്യ flagval = 0;

#ifdef FEAT_STL_OPT
    flagval = STL_IN_ICON;
#endif

    തിരികെ parse_titleiconstring(args, flagval);
}

#എങ്കിൽ defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
/*
 * The 'imactivatekey' option is changed.
 */
    അക്ഷരം *
did_set_imactivatekey(optset_T *args UNUSED)
{
    എങ്കിൽ (!im_xim_isvalid_imactivate())
	തിരികെ e_invalid_argument;
    തിരികെ ശൂന്യ;
}
#endif

#ifdef FEAT_IMAGE
/*
 * The 'imageprotocol' option is changed.
 */
    അക്ഷരം *
did_set_imageprotocol(optset_T *args UNUSED)
{
    എങ്കിൽ (update_image_backend() == FAIL)
	തിരികെ e_invalid_argument;
    തിരികെ ശൂന്യ;
}

    പൂർണ്ണസംഖ്യ
expand_set_imageprotocol(optexpand_T *args, പൂർണ്ണസംഖ്യ *അക്കം_Matches, char_u ***matches)
{
    expand_T *xp = args->oe_xp;

    എങ്കിൽ (xp->xp_pattern > args->oe_set_arg && *(xp->xp_pattern-1) == ':')
    {
	// 'imageprotocol' only has well-defined terms for completion for the
	// protocol part after the colon.
	തിരികെ expand_set_opt_string(
		args,
		p_ipc_protocol_മൂല്യങ്ങൾ,
		ARRAY_LENGTH(p_ipc_protocol_മൂല്യങ്ങൾ) - 1,
		അക്കം_Matches,
		matches);
    }
    // Use expand_set_opt_string instead of returning FAIL so that we can
    // include the original value if args->oe_include_orig_val is set.
    സ്ഥിതം അക്ഷരം *(empty[]) = {ശൂന്യ};
    തിരികെ expand_set_opt_string(args, empty, 0, അക്കം_Matches, matches);
}
#endif

/*
 * The 'iskeyword' option is changed.
 */
    അക്ഷരം *
did_set_iskeyword(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    എങ്കിൽ (varp == &p_isk)		// only check for global-value
    {
	എങ്കിൽ (check_isopt(*varp) == FAIL)
	    തിരികെ e_invalid_argument;
    }
    അല്ലെങ്കിൽ			// fallthrough for local-value
	തിരികെ did_set_isopt(args);

    തിരികെ ശൂന്യ;
}

/*
 * The 'isident' or the 'iskeyword' or the 'isprint' or the 'isfname' option is
 * changed.
 */
    അക്ഷരം *
did_set_isopt(optset_T *args)
{
    // 'isident', 'iskeyword', 'isprint' or 'isfname' option: refill g_chartab[]
    // If the new option is invalid, use old value.
    // 'lisp' option: refill g_chartab[] for '-' char.
    എങ്കിൽ (init_chartab() == FAIL)
    {
	args->os_restore_chartab = TRUE;// need to restore the chartab.
	തിരികെ e_invalid_argument;	// error in value
    }

    തിരികെ ശൂന്യ;
}

/*
 * The 'jumpoptions' option is changed.
 */
    അക്ഷരം *
did_set_jumpoptions(optset_T *args UNUSED)
{
    എങ്കിൽ (opt_strings_flags(p_jop, p_jop_മൂല്യങ്ങൾ, &jop_flags, TRUE) != OK)
	തിരികെ e_invalid_argument;

    തിരികെ ശൂന്യ;
}

    പൂർണ്ണസംഖ്യ
expand_set_jumpoptions(optexpand_T *args, പൂർണ്ണസംഖ്യ *അക്കം_Matches, char_u ***matches)
{
    തിരികെ expand_set_opt_string(
	    args,
	    p_jop_മൂല്യങ്ങൾ,
	    ARRAY_LENGTH(p_jop_മൂല്യങ്ങൾ) - 1,
	    അക്കം_Matches,
	    matches);
}

#എങ്കിൽ defined(FEAT_KEYMAP)
/*
 * The 'keymap' option is changed.
 */
    അക്ഷരം *
did_set_keymap(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    അക്ഷരം *errmsg = ശൂന്യ;

    എങ്കിൽ (!valid_filetype(*varp))
	errmsg = e_invalid_argument;
    അല്ലെങ്കിൽ
    {
	പൂർണ്ണസംഖ്യ	    secure_save = secure;

	// Reset the secure flag, since the value of 'keymap' has
	// been checked to be safe.
	secure = 0;

	// load or unload key mapping tables
	errmsg = keymap_init();

	secure = secure_save;

	// Since we check the value, there is no need to set P_INSECURE,
	// even when the value comes from a modeline.
	args->os_മൂല്യം_checked = TRUE;
    }

    എങ്കിൽ (errmsg == ശൂന്യ)
    {
	എങ്കിൽ (*curbuf->b_p_keymap != NUL)
	{
	    // Installed a new keymap, switch on using it.
	    curbuf->b_p_iminsert = B_IMODE_LMAP;
	    എങ്കിൽ (curbuf->b_p_imsearch != B_IMODE_USE_തിരുകുക)
		curbuf->b_p_imsearch = B_IMODE_LMAP;
	}
	അല്ലെങ്കിൽ
	{
	    // Cleared the keymap, may reset 'iminsert' and 'imsearch'.
	    എങ്കിൽ (curbuf->b_p_iminsert == B_IMODE_LMAP)
		curbuf->b_p_iminsert = B_IMODE_NONE;
	    എങ്കിൽ (curbuf->b_p_imsearch == B_IMODE_LMAP)
		curbuf->b_p_imsearch = B_IMODE_USE_തിരുകുക;
	}
	എങ്കിൽ ((args->os_flags & OPT_LOCAL) == 0)
	{
	    set_iminsert_global();
	    set_imsearch_global();
	}
	status_redraw_curbuf();
    }

    തിരികെ errmsg;
}
#endif

/*
 * The 'keymodel' option is changed.
 */
    അക്ഷരം *
did_set_keymodel(optset_T *args UNUSED)
{
    എങ്കിൽ (check_opt_strings(p_km, p_km_മൂല്യങ്ങൾ, TRUE) != OK)
	തിരികെ e_invalid_argument;

    km_stopsel = (vim_strchr(p_km, 'o') != ശൂന്യ);
    km_startsel = (vim_strchr(p_km, 'a') != ശൂന്യ);
    തിരികെ ശൂന്യ;
}

    പൂർണ്ണസംഖ്യ
expand_set_keymodel(optexpand_T *args, പൂർണ്ണസംഖ്യ *അക്കം_Matches, char_u ***matches)
{
    തിരികെ expand_set_opt_string(
	    args,
	    p_km_മൂല്യങ്ങൾ,
	    ARRAY_LENGTH(p_km_മൂല്യങ്ങൾ) - 1,
	    അക്കം_Matches,
	    matches);
}

/*
 * The 'keyprotocol' option is changed.
 */
    അക്ഷരം *
did_set_keyprotocol(optset_T *args UNUSED)
{
    char_u *term = T_NAME;
    keyprot_T kpc = match_keyprotocol(term);
    എങ്കിൽ (kpc == KEYPROTOCOL_FAIL)
	തിരികെ e_invalid_argument;

    apply_keyprotocol(term, kpc);

    തിരികെ ശൂന്യ;
}

    പൂർണ്ണസംഖ്യ
expand_set_keyprotocol(optexpand_T *args, പൂർണ്ണസംഖ്യ *അക്കം_Matches, char_u ***matches)
{
    expand_T *xp = args->oe_xp;
    എങ്കിൽ (xp->xp_pattern > args->oe_set_arg && *(xp->xp_pattern-1) == ':')
    {
	// 'keyprotocol' only has well-defined terms for completion for the
	// protocol part after the colon.
	തിരികെ expand_set_opt_string(
		args,
		p_kpc_protocol_മൂല്യങ്ങൾ,
		ARRAY_LENGTH(p_kpc_protocol_മൂല്യങ്ങൾ) - 1,
		അക്കം_Matches,
		matches);
    }
    // Use expand_set_opt_string instead of returning FAIL so that we can
    // include the original value if args->oe_include_orig_val is set.
    സ്ഥിതം അക്ഷരം *(empty[]) = {ശൂന്യ};
    തിരികെ expand_set_opt_string(args, empty, 0, അക്കം_Matches, matches);
}

/*
 * The 'lispoptions' option is changed.
 */
    അക്ഷരം *
did_set_lispoptions(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    എങ്കിൽ (**varp != NUL
	    && STRCMP(*varp, "expr:0") != 0
	    && STRCMP(*varp, "expr:1") != 0)
	തിരികെ e_invalid_argument;

    തിരികെ ശൂന്യ;
}

    പൂർണ്ണസംഖ്യ
expand_set_lispoptions(optexpand_T *args, പൂർണ്ണസംഖ്യ *അക്കം_Matches, char_u ***matches)
{
    സ്ഥിതം അക്ഷരം *(p_lop_മൂല്യങ്ങൾ[]) = {"expr:0", "expr:1", ശൂന്യ};
    തിരികെ expand_set_opt_string(
	    args,
	    p_lop_മൂല്യങ്ങൾ,
	    ARRAY_LENGTH(p_lop_മൂല്യങ്ങൾ) - 1,
	    അക്കം_Matches,
	    matches);
}

/*
 * The 'matchpairs' option is changed.
 */
    അക്ഷരം *
did_set_matchpairs(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u	*p;

    എങ്കിൽ (has_mbyte)
    {
	വേണ്ടി (p = *varp; *p != NUL; ++p)
	{
	    പൂർണ്ണസംഖ്യ x2 = -1;
	    പൂർണ്ണസംഖ്യ x3 = -1;

	    p += mb_ptr2len(p);
	    എങ്കിൽ (*p != NUL)
		x2 = *p++;
	    എങ്കിൽ (*p != NUL)
	    {
		x3 = mb_ptr2char(p);
		p += mb_ptr2len(p);
	    }
	    എങ്കിൽ (x2 != ':' || x3 == -1 || (*p != NUL && *p != ','))
		തിരികെ e_invalid_argument;
	    എങ്കിൽ (*p == NUL)
		നിർത്തുക;
	}
    }
    അല്ലെങ്കിൽ
    {
	// Check for "x:y,x:y"
	വേണ്ടി (p = *varp; *p != NUL; p += 4)
	{
	    എങ്കിൽ (p[1] != ':' || p[2] == NUL || (p[3] != NUL && p[3] != ','))
		തിരികെ e_invalid_argument;
	    എങ്കിൽ (p[3] == NUL)
		നിർത്തുക;
	}
    }

    തിരികെ ശൂന്യ;
}

/*
 * Process the updated 'messagesopt' option value.
 */
    അക്ഷരം *
did_set_messagesopt(optset_T *args UNUSED)
{
    എങ്കിൽ (messagesopt_changed() == FAIL)
	തിരികെ e_invalid_argument;

    തിരികെ ശൂന്യ;
}

    പൂർണ്ണസംഖ്യ
expand_set_messagesopt(optexpand_T *args, പൂർണ്ണസംഖ്യ *അക്കം_Matches, char_u ***matches)
{
    സ്ഥിതം അക്ഷരം *(p_mopt_മൂല്യങ്ങൾ[]) = {"hit-enter", "wait:", "history:", ശൂന്യ};
    തിരികെ expand_set_opt_string(
	    args,
	    p_mopt_മൂല്യങ്ങൾ,
	    ARRAY_LENGTH(p_mopt_മൂല്യങ്ങൾ) - 1,
	    അക്കം_Matches,
	    matches);
}

#എങ്കിൽ defined(FEAT_SPELL)
/*
 * The 'mkspellmem' option is changed.
 */
    അക്ഷരം *
did_set_mkspellmem(optset_T *args UNUSED)
{
    എങ്കിൽ (spell_check_msm() != OK)
	തിരികെ e_invalid_argument;

    തിരികെ ശൂന്യ;
}
#endif

/*
 * The 'mouse' option is changed.
 */
    അക്ഷരം *
did_set_mouse(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    അക്ഷരം	*retval;

    retval = did_set_option_listflag(*varp, (char_u *)MOUSE_ALL, args->os_errbuf,
		    args->os_errbuflen);
    എങ്കിൽ (retval == ശൂന്യ)
    {
	redraw_tabline = TRUE;
	എങ്കിൽ (tabline_height() > 0)
	    update_screen(UPD_VALID);
#എങ്കിൽ defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
	popup_close_info(); // Close info popup to apply new properties
#endif
    }
    തിരികെ retval;
}

    പൂർണ്ണസംഖ്യ
expand_set_mouse(optexpand_T *args, പൂർണ്ണസംഖ്യ *അക്കം_Matches, char_u ***matches)
{
    തിരികെ expand_set_opt_listflag(args, (char_u*)MOUSE_ALL, അക്കം_Matches, matches);
}

/*
 * The 'mousemodel' option is changed.
 */
    അക്ഷരം *
did_set_mousemodel(optset_T *args UNUSED)
{
    എങ്കിൽ (check_opt_strings(p_mousem, p_mousem_മൂല്യങ്ങൾ, FALSE) != OK)
	തിരികെ e_invalid_argument;
#എങ്കിൽ defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU) && (XmVersion <= 1002)
    അല്ലെങ്കിൽ എങ്കിൽ (*p_mousem != *args->os_oldval.string)
	// Changed from "extend" to "popup" or "popup_setpos" or vv: need
	// to create or delete the popup menus.
	gui_motif_update_mousemodel(വേര്_menu);
#endif

    തിരികെ ശൂന്യ;
}

    പൂർണ്ണസംഖ്യ
expand_set_mousemodel(optexpand_T *args, പൂർണ്ണസംഖ്യ *അക്കം_Matches, char_u ***matches)
{
    തിരികെ expand_set_opt_string(
	    args,
	    p_mousem_മൂല്യങ്ങൾ,
	    ARRAY_LENGTH(p_mousem_മൂല്യങ്ങൾ) - 1,
	    അക്കം_Matches,
	    matches);
}

#എങ്കിൽ defined(FEAT_MOUSESHAPE)
    അക്ഷരം *
did_set_mouseshape(optset_T *args UNUSED)
{
    അക്ഷരം *errmsg = ശൂന്യ;

    errmsg = parse_shape_opt(SHAPE_MOUSE);
    update_mouseshape(-1);

    തിരികെ errmsg;
}
#endif

/*
 * The 'nrformats' option is changed.
 */
    അക്ഷരം *
did_set_nrformats(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    തിരികെ did_set_opt_strings(*varp, p_nf_മൂല്യങ്ങൾ, TRUE);
}

    പൂർണ്ണസംഖ്യ
expand_set_nrformats(optexpand_T *args, പൂർണ്ണസംഖ്യ *അക്കം_Matches, char_u ***matches)
{
    തിരികെ expand_set_opt_string(
	    args,
	    p_nf_മൂല്യങ്ങൾ,
	    ARRAY_LENGTH(p_nf_മൂല്യങ്ങൾ) - 1,
	    അക്കം_Matches,
	    matches);
}

#എങ്കിൽ defined(FEAT_EVAL)
/*
 * One of the '*expr' options is changed: 'balloonexpr', 'diffexpr',
 * 'foldexpr', 'foldtext', 'formatexpr', 'includeexpr', 'indentexpr',
 * 'patchexpr', 'printexpr' and 'charconvert'.
 */
    അക്ഷരം *
did_set_optexpr(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    // If the option value starts with <SID> or s:, then replace that with
    // the script identifier.
    char_u *name = get_scriptlocal_funcname(*varp);
    എങ്കിൽ (name != ശൂന്യ)
    {
	free_string_option(*varp);
	*varp = name;
    }

    തിരികെ ശൂന്യ;
}
#endif

/*
 * The 'pastetoggle' option is changed.
 */
    അക്ഷരം *
did_set_pastetoggle(optset_T *args UNUSED)
{
    char_u	*p;

    // translate key codes like in a mapping
    എങ്കിൽ (*p_pt)
    {
	(ശൂന്യം)replace_termcodes(p_pt, &p, 0,
		REPTERM_FROM_PART | REPTERM_DO_LT, ശൂന്യ);
	എങ്കിൽ (p != ശൂന്യ)
	{
	    free_string_option(p_pt);
	    p_pt = p;
	}
    }

    തിരികെ ശൂന്യ;
}

#എങ്കിൽ defined(FEAT_PROP_POPUP)
/*
 * The 'previewpopup' option is changed.
 */
    അക്ഷരം *
did_set_previewpopup(optset_T *args UNUSED)
{
    എങ്കിൽ (parse_previewpopup(ശൂന്യ) == FAIL)
	തിരികെ e_invalid_argument;

# എങ്കിൽ defined(FEAT_QUICKFIX)
    popup_close_info();
# endif
    തിരികെ ശൂന്യ;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
expand_set_popupoption(optexpand_T *args, പൂർണ്ണസംഖ്യ *അക്കം_Matches, char_u ***matches,
	പൂർണ്ണസംഖ്യ previewpopup)
{
    expand_T *xp = args->oe_xp;

    എങ്കിൽ (xp->xp_pattern > args->oe_set_arg && *(xp->xp_pattern-1) == ':')
    {
	എങ്കിൽ (completing_മൂല്യം_for_subopt(args, "close")
		|| completing_മൂല്യം_for_subopt(args, "resize")
		|| completing_മൂല്യം_for_subopt(args, "shadow"))
	{
	    തിരികെ expand_set_opt_string(
		    args,
		    p_popup_option_on_off_മൂല്യങ്ങൾ,
		    ARRAY_LENGTH(p_popup_option_on_off_മൂല്യങ്ങൾ) - 1,
		    അക്കം_Matches,
		    matches);
	}
	എങ്കിൽ (completing_മൂല്യം_for_subopt(args, "border"))
	{
	    തിരികെ expand_set_opt_string(
		    args,
		    previewpopup ? p_popup_pvp_border_മൂല്യങ്ങൾ
			: p_popup_cpp_border_മൂല്യങ്ങൾ,
		    (previewpopup ? ARRAY_LENGTH(p_popup_pvp_border_മൂല്യങ്ങൾ) - 1
			: ARRAY_LENGTH(p_popup_cpp_border_മൂല്യങ്ങൾ)) - 1,
		    അക്കം_Matches,
		    matches);
	}
	എങ്കിൽ (completing_മൂല്യം_for_subopt(args, "align"))
	{
	    തിരികെ expand_set_opt_string(
		    args,
		    p_popup_option_align_മൂല്യങ്ങൾ,
		    ARRAY_LENGTH(p_popup_option_align_മൂല്യങ്ങൾ) - 1,
		    അക്കം_Matches,
		    matches);
	}
	എങ്കിൽ (completing_മൂല്യം_for_subopt(args, "highlight")
		|| completing_മൂല്യം_for_subopt(args, "borderhighlight"))
	{
	    // Return the list of all highlight names
	    തിരികെ expand_set_opt_generic(
		    args,
		    get_highlight_name,
		    അക്കം_Matches,
		    matches);
	}
	തിരികെ FAIL;
    }

    തിരികെ expand_set_opt_string(
	    args,
	    previewpopup ? p_popup_pvp_option_മൂല്യങ്ങൾ
		: p_popup_cpp_option_മൂല്യങ്ങൾ,
	    previewpopup ? ARRAY_LENGTH(p_popup_pvp_option_മൂല്യങ്ങൾ) - 1
		: ARRAY_LENGTH(p_popup_cpp_option_മൂല്യങ്ങൾ) - 1,
	    അക്കം_Matches,
	    matches);
}

    പൂർണ്ണസംഖ്യ
expand_set_previewpopup(optexpand_T *args, പൂർണ്ണസംഖ്യ *അക്കം_Matches, char_u ***matches)
{
    തിരികെ expand_set_popupoption(args, അക്കം_Matches, matches, TRUE);
}

    പൂർണ്ണസംഖ്യ
expand_set_completepopup(optexpand_T *args, പൂർണ്ണസംഖ്യ *അക്കം_Matches, char_u ***matches)
{
    തിരികെ expand_set_popupoption(args, അക്കം_Matches, matches, FALSE);
}
#endif

#എങ്കിൽ defined(FEAT_POSTSCRIPT)
/*
 * The 'printencoding' option is changed.
 */
    അക്ഷരം *
did_set_printencoding(optset_T *args UNUSED)
{
    char_u	*s, *p;

    // Canonize 'printencoding' if VIM standard one
    p = enc_canonize(p_penc);
    എങ്കിൽ (p != ശൂന്യ)
    {
	vim_free(p_penc);
	p_penc = p;
    }
    അല്ലെങ്കിൽ
    {
	// Ensure lower case and '-' for '_'
	വേണ്ടി (s = p_penc; *s != NUL; s++)
	{
	    എങ്കിൽ (*s == '_')
		*s = '-';
	    അല്ലെങ്കിൽ
		*s = TOLOWER_ASC(*s);
	}
    }

    തിരികെ ശൂന്യ;
}
#endif

#എങ്കിൽ defined(FEAT_PRINTER)

    സ്ഥിതം char_u *
get_printoptions_names(expand_T *xp UNUSED, പൂർണ്ണസംഖ്യ idx)
{
    എങ്കിൽ (idx >= (പൂർണ്ണസംഖ്യ)ARRAY_LENGTH(printer_opts))
	തിരികെ ശൂന്യ;
    തിരികെ (char_u*)printer_opts[idx].name;
}

/*
 * Expand 'printoptions' option
 */
    പൂർണ്ണസംഖ്യ
expand_set_printoptions(optexpand_T *args, പൂർണ്ണസംഖ്യ *അക്കം_Matches, char_u ***matches)
{
    തിരികെ expand_set_opt_generic(
	    args,
	    get_printoptions_names,
	    അക്കം_Matches,
	    matches);
}
#endif

#എങ്കിൽ defined(FEAT_STL_OPT)
/*
 * The 'statusline' or the 'tabline' or the 'rulerformat' option is changed.
 * "rulerformat" is TRUE if the 'rulerformat' option is changed.
 */
    സ്ഥിതം അക്ഷരം *
parse_statustabline_rulerformat(optset_T *args, പൂർണ്ണസംഖ്യ rulerformat)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u	*s;
    അക്ഷരം	*errmsg = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		wid;

    എങ്കിൽ (rulerformat)	// reset ru_wid first
	ru_wid = 0;
    s = *varp;
    എങ്കിൽ (rulerformat && *s == '%')
    {
	// set ru_wid if 'ruf' starts with "%99("
	എങ്കിൽ (*++s == '-')	// ignore a '-'
	    s++;
	wid = getdigits(&s);
	എങ്കിൽ (wid && *s == '(' && (errmsg = check_stl_option(p_ruf)) == ശൂന്യ)
	    ru_wid = wid;
	അല്ലെങ്കിൽ
	{
	    // Validate the flags in 'rulerformat' only if it doesn't point to
	    // a custom function ("%!" flag).
	    എങ്കിൽ ((*varp)[1] != '!')
		errmsg = check_stl_option(p_ruf);
	}
    }
    // check 'statusline' or 'tabline' only if it doesn't start with "%!"
    അല്ലെങ്കിൽ എങ്കിൽ (rulerformat || s[0] != '%' || s[1] != '!')
	errmsg = check_stl_option(s);
    എങ്കിൽ (rulerformat && errmsg == ശൂന്യ)
	comp_col();

    തിരികെ errmsg;
}
#endif

#എങ്കിൽ defined(FEAT_RENDER_OPTIONS)
/*
 * The 'renderoptions' option is changed.
 */
    അക്ഷരം *
did_set_renderoptions(optset_T *args UNUSED)
{
    എങ്കിൽ (!gui_mch_set_rendering_options(p_rop))
	തിരികെ e_invalid_argument;

    തിരികെ ശൂന്യ;
}
#endif

#എങ്കിൽ defined(FEAT_RIGHTLEFT)
/*
 * The 'rightleftcmd' option is changed.
 */
    അക്ഷരം *
did_set_rightleftcmd(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    // Currently only "search" is a supported value.
    എങ്കിൽ (**varp != NUL && STRCMP(*varp, "search") != 0)
	തിരികെ e_invalid_argument;

    തിരികെ ശൂന്യ;
}

    പൂർണ്ണസംഖ്യ
expand_set_rightleftcmd(optexpand_T *args, പൂർണ്ണസംഖ്യ *അക്കം_Matches, char_u ***matches)
{
    സ്ഥിതം അക്ഷരം *(p_rlc_മൂല്യങ്ങൾ[]) = {"search", ശൂന്യ};
    തിരികെ expand_set_opt_string(
	    args,
	    p_rlc_മൂല്യങ്ങൾ,
	    ARRAY_LENGTH(p_rlc_മൂല്യങ്ങൾ) - 1,
	    അക്കം_Matches,
	    matches);
}
#endif

#define PUM_BORDER_CLEAR()   \
    ചെയ്യുക {                     \
	pum_set_border(FALSE); \
	pum_set_shadow(FALSE); \
	pum_set_margin(FALSE); \
    } ആകുമ്പോൾ (0)

/*
 * Parse a border value from a pumopt "border:" token.
 * Returns OK on success, FAIL on error.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
parse_pumopt_border(char_u *val, പൂർണ്ണസംഖ്യ len)
{
    // Use box-drawing characters only when 'encoding' is "utf-8" and
    // 'ambiwidth' is "single".
    പൂർണ്ണസംഖ്യ	    can_use_box_chars = (enc_utf8 && *p_ambw == 's');
    char_u  *token;

    token = vim_strnsave(val, len);
    എങ്കിൽ (token == ശൂന്യ)
	തിരികെ FAIL;

    എങ്കിൽ (can_use_box_chars && STRCMP(token, "single") == 0)
	pum_set_border_chars(0x2500, 0x2502, 0x2500, 0x2502, // ─ │ ─ │
		0x250c, 0x2510, 0x2518, 0x2514); // ┌ ┐ ┘ └
    അല്ലെങ്കിൽ എങ്കിൽ (can_use_box_chars && STRCMP(token, "double") == 0)
	pum_set_border_chars(0x2550, 0x2551, 0x2550, 0x2551, // ═ ║ ═ ║
		0x2554, 0x2557, 0x255D, 0x255A); // ╔ ╗ ╝  ╚
    അല്ലെങ്കിൽ എങ്കിൽ (can_use_box_chars && STRCMP(token, "round") == 0)
	pum_set_border_chars(0x2500, 0x2502, 0x2500, 0x2502, // ─ │ ─ │
		0x256d, 0x256e, 0x256f, 0x2570); // ╭ ╮ ╯ ╰
    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(token, "ascii") == 0)
	pum_set_border_chars('-', '|', '-', '|', '+', '+', '+', '+');
    അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(token, "custom:", 7) == 0)
    {
	char_u	*q = token + 7;
	പൂർണ്ണസംഖ്യ	out[8];

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < 8; i++)
	{
	    എങ്കിൽ (*q == NUL || *q == ',')
	    {
		vim_free(token);
		തിരികെ FAIL;
	    }
	    out[i] = mb_ptr2char(q);
	    mb_ptr2char_adv(&q);
	    എങ്കിൽ (i < 7)
	    {
		എങ്കിൽ (*q != ';')
		{
		    vim_free(token);
		    തിരികെ FAIL;
		}
		q++;
	    }
	}
	എങ്കിൽ (*q != NUL && *q != ',')
	{
	    vim_free(token);
	    തിരികെ FAIL;
	}
	pum_set_border_chars(out[0], out[1], out[2], out[3], out[4], out[5],
		out[6], out[7]);
    }
    അല്ലെങ്കിൽ
    {
	vim_free(token);
	തിരികെ FAIL;
    }

    vim_free(token);
    pum_set_border(TRUE);
    തിരികെ OK;
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
    അക്ഷരം *
did_set_pumopt(optset_T *args)
{
    char_u  **varp = (char_u **)args->os_varp;
    char_u  *p;
    പൂർണ്ണസംഖ്യ	    have_border = FALSE;
    പൂർണ്ണസംഖ്യ	    have_margin = FALSE;

    // Reset to defaults.
    PUM_BORDER_CLEAR();
    p_ph = 0;
    p_pw = 15;
    p_pmw = 0;
    p_po = 100;

    എങ്കിൽ (*varp == ശൂന്യ || **varp == NUL)
	തിരികെ ശൂന്യ;

    വേണ്ടി (p = *varp; p != ശൂന്യ && *p != NUL; )
    {
	char_u *comma = vim_strchr(p, ',');
	പൂർണ്ണസംഖ്യ len;

	എങ്കിൽ (comma != ശൂന്യ)
	    len = (പൂർണ്ണസംഖ്യ)(comma - p);
	അല്ലെങ്കിൽ
	    len = (പൂർണ്ണസംഖ്യ)STRLEN(p);

	എങ്കിൽ (STRNCMP(p, "border:", 7) == 0)
	{
	    എങ്കിൽ (have_border)
		പോകുക error;
	    have_border = TRUE;
	    എങ്കിൽ (parse_pumopt_border(p + 7, len - 7) == FAIL)
		പോകുക error;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(p, "height:", 7) == 0)
	{
	    long n = atol((അക്ഷരം *)p + 7);
	    എങ്കിൽ (n < 0)
		പോകുക error;
	    p_ph = n;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(p, "width:", 6) == 0)
	{
	    long n = atol((അക്ഷരം *)p + 6);
	    എങ്കിൽ (n < 0)
		പോകുക error;
	    p_pw = n;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(p, "maxwidth:", 9) == 0)
	{
	    long n = atol((അക്ഷരം *)p + 9);
	    എങ്കിൽ (n < 0)
		പോകുക error;
	    p_pmw = n;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(p, "opacity:", 8) == 0)
	{
	    long n = atol((അക്ഷരം *)p + 8);
	    എങ്കിൽ (n < 0 || n > 100)
		പോകുക error;
	    p_po = n;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (len == 6 && STRNCMP(p, "shadow", 6) == 0)
	    pum_set_shadow(TRUE);
	അല്ലെങ്കിൽ എങ്കിൽ (len == 6 && STRNCMP(p, "margin", 6) == 0)
	{
	    have_margin = TRUE;
	    pum_set_margin(TRUE);
	}
	അല്ലെങ്കിൽ
	    പോകുക error;

	എങ്കിൽ (comma != ശൂന്യ)
	    p = comma + 1;
	അല്ലെങ്കിൽ
	    നിർത്തുക;
    }

    എങ്കിൽ (have_margin && !have_border)
	പോകുക error;

    // Invalidate cached background for opacity changes.
    pum_opacity_changed();

    തിരികെ ശൂന്യ;

error:
    PUM_BORDER_CLEAR();
    p_ph = 0;
    p_pw = 15;
    p_pmw = 0;
    p_po = 100;
    തിരികെ e_invalid_argument;
}

    സ്ഥിതം char_u *
get_pum_border_style(expand_T *xp UNUSED, പൂർണ്ണസംഖ്യ idx)
{
    സ്ഥിതം അക്ഷരം *styles[] = {"ascii", "custom:", "single", "double", "round"};
    തിരികെ idx < ((enc_utf8 && *p_ambw == 's') ? (പൂർണ്ണസംഖ്യ)ARRAY_LENGTH(styles) : 2)
	    ? (char_u *)styles[idx] : ശൂന്യ;
}

    പൂർണ്ണസംഖ്യ
expand_set_pumopt(optexpand_T *args, പൂർണ്ണസംഖ്യ *അക്കം_Matches, char_u ***matches)
{
    expand_T *xp = args->oe_xp;

    എങ്കിൽ (xp->xp_pattern > args->oe_set_arg && *(xp->xp_pattern-1) == ':')
    {
	എങ്കിൽ (completing_മൂല്യം_for_subopt(args, "border"))
	{
	    തിരികെ expand_set_opt_generic(
		    args, get_pum_border_style, അക്കം_Matches, matches);
	}
	തിരികെ FAIL;
    }

    സ്ഥിതം അക്ഷരം *(p_pumopt_മൂല്യങ്ങൾ[]) = {"border:", "height:", "width:",
	"maxwidth:", "opacity:", "shadow", "margin", ശൂന്യ};
    തിരികെ expand_set_opt_string(
	    args,
	    p_pumopt_മൂല്യങ്ങൾ,
	    ARRAY_LENGTH(p_pumopt_മൂല്യങ്ങൾ) - 1,
	    അക്കം_Matches,
	    matches);
}

/*
 * The 'pumborder' option is changed.
 * Rules:
 *   - One of { single, double, round, ascii, custom:XXXXXXXX } may appear.
 *   - "margin" may appear, but only together with exactly one border style.
 *   - "shadow" is independent and can be combined freely.
 */
    അക്ഷരം *
did_set_pumborder(optset_T *args)
{
    char_u  **varp = (char_u **)args->os_varp;
    char_u  *p;
    പൂർണ്ണസംഖ്യ	    len;
    പൂർണ്ണസംഖ്യ	    have_border = FALSE;
    പൂർണ്ണസംഖ്യ	    have_margin = FALSE;

    PUM_BORDER_CLEAR();

    എങ്കിൽ (*varp == ശൂന്യ || **varp == NUL)
	തിരികെ ശൂന്യ;

    വേണ്ടി (p = *varp; p != ശൂന്യ && *p != NUL; )
    {
	char_u *comma = vim_strchr(p, ',');
	എങ്കിൽ (comma != ശൂന്യ)
	    len = (പൂർണ്ണസംഖ്യ)(comma - p);
	അല്ലെങ്കിൽ
	    len = (പൂർണ്ണസംഖ്യ)STRLEN(p);

	എങ്കിൽ (STRNCMP(p, "shadow", len) == 0 && len == 6)
	    pum_set_shadow(TRUE);
	അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(p, "margin", len) == 0 && len == 6)
	{
	    have_margin = TRUE;
	    pum_set_margin(TRUE);
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (have_border)
		പോകുക error;
	    have_border = TRUE;
	    എങ്കിൽ (parse_pumopt_border(p, len) == FAIL)
		പോകുക error;
	}

	എങ്കിൽ (comma != ശൂന്യ)
	    p = comma + 1;
	അല്ലെങ്കിൽ
	    നിർത്തുക;
    }

    എങ്കിൽ (have_margin && !have_border)
	പോകുക error;

    തിരികെ ശൂന്യ;

error:
    PUM_BORDER_CLEAR();
    pum_set_border_chars(0, 0, 0, 0, 0, 0, 0, 0);
    തിരികെ e_invalid_argument;
}

    സ്ഥിതം char_u *
get_pumborder_token(expand_T *xp, പൂർണ്ണസംഖ്യ idx)
{
    തിരികെ idx == 0 ? (char_u *)"margin"
	 : idx == 1 ? (char_u *)"shadow"
	 : get_pum_border_style(xp, idx - 2);
}

    പൂർണ്ണസംഖ്യ
expand_set_pumborder(optexpand_T *args, പൂർണ്ണസംഖ്യ *അക്കം_Matches, char_u ***matches)
{
    തിരികെ expand_set_opt_generic(
	    args, get_pumborder_token, അക്കം_Matches, matches);
}

#എങ്കിൽ defined(FEAT_STL_OPT)

/*
 * The 'rulerformat' option is changed.
 */
    അക്ഷരം *
did_set_rulerformat(optset_T *args)
{
    തിരികെ parse_statustabline_rulerformat(args, TRUE);
}
#endif

#എങ്കിൽ defined(FEAT_TABPANEL)
/*
 * Process the new 'tabpanelopt' option value.
 */
    അക്ഷരം *
did_set_tabpanelopt(optset_T *args UNUSED)
{
    എങ്കിൽ (tabpanelopt_changed() == FAIL)
	തിരികെ e_invalid_argument;

    തിരികെ ശൂന്യ;
}

    പൂർണ്ണസംഖ്യ
expand_set_tabpanelopt(optexpand_T *args, പൂർണ്ണസംഖ്യ *അക്കം_Matches, char_u ***matches)
{
    expand_T *xp = args->oe_xp;

    എങ്കിൽ (xp->xp_pattern > args->oe_set_arg && *(xp->xp_pattern-1) == ':')
    {
	എങ്കിൽ (completing_മൂല്യം_for_subopt(args, "align"))
	{
	    തിരികെ expand_set_opt_string(
		    args,
		    p_tplo_align_മൂല്യങ്ങൾ,
		    ARRAY_LENGTH(p_tplo_align_മൂല്യങ്ങൾ) - 1,
		    അക്കം_Matches,
		    matches);
	}
	തിരികെ FAIL;
    }

    തിരികെ expand_set_opt_string(
	    args,
	    p_tplo_മൂല്യങ്ങൾ,
	    ARRAY_LENGTH(p_tplo_മൂല്യങ്ങൾ) - 1,
	    അക്കം_Matches,
	    matches);
}
#endif

/*
 * The 'scrollopt' option is changed.
 */
    അക്ഷരം *
did_set_scrollopt(optset_T *args UNUSED)
{
    തിരികെ did_set_opt_strings(p_sbo, p_scbopt_മൂല്യങ്ങൾ, TRUE);
}

    പൂർണ്ണസംഖ്യ
expand_set_scrollopt(optexpand_T *args, പൂർണ്ണസംഖ്യ *അക്കം_Matches, char_u ***matches)
{
    തിരികെ expand_set_opt_string(
	    args,
	    p_scbopt_മൂല്യങ്ങൾ,
	    ARRAY_LENGTH(p_scbopt_മൂല്യങ്ങൾ) - 1,
	    അക്കം_Matches,
	    matches);
}

/*
 * The 'wlseat' option is changed
 */
    അക്ഷരം *
did_set_wlseat(optset_T *args UNUSED)
{
#ifdef FEAT_WAYLAND_CLIPBOARD
    // If there isn't any seat named 'wlseat', then let the Wayland clipboard be
    // unavailable. Ignore errors returned.
    clip_reset_wayland();
#endif

    തിരികെ ശൂന്യ;
}

/*
 * The 'selection' option is changed.
 */
    അക്ഷരം *
did_set_selection(optset_T *args UNUSED)
{
    എങ്കിൽ (*p_sel == NUL || check_opt_strings(p_sel, p_sel_മൂല്യങ്ങൾ, FALSE) != OK)
	തിരികെ e_invalid_argument;

    തിരികെ ശൂന്യ;
}

    പൂർണ്ണസംഖ്യ
expand_set_selection(optexpand_T *args, പൂർണ്ണസംഖ്യ *അക്കം_Matches, char_u ***matches)
{
    തിരികെ expand_set_opt_string(
	    args,
	    p_sel_മൂല്യങ്ങൾ,
	    ARRAY_LENGTH(p_sel_മൂല്യങ്ങൾ) - 1,
	    അക്കം_Matches,
	    matches);
}

/*
 * The 'selectmode' option is changed.
 */
    അക്ഷരം *
did_set_selectmode(optset_T *args UNUSED)
{
    തിരികെ did_set_opt_strings(p_slm, p_slm_മൂല്യങ്ങൾ, TRUE);
}

    പൂർണ്ണസംഖ്യ
expand_set_selectmode(optexpand_T *args, പൂർണ്ണസംഖ്യ *അക്കം_Matches, char_u ***matches)
{
    തിരികെ expand_set_opt_string(
	    args,
	    p_slm_മൂല്യങ്ങൾ,
	    ARRAY_LENGTH(p_slm_മൂല്യങ്ങൾ) - 1,
	    അക്കം_Matches,
	    matches);
}

#എങ്കിൽ defined(FEAT_SESSION)
/*
 * The 'sessionoptions' option is changed.
 */
    അക്ഷരം *
did_set_sessionoptions(optset_T *args)
{
    എങ്കിൽ (opt_strings_flags(p_ssop, p_ssop_മൂല്യങ്ങൾ, &ssop_flags, TRUE) != OK)
	തിരികെ e_invalid_argument;
    എങ്കിൽ ((ssop_flags & SSOP_CURDIR) && (ssop_flags & SSOP_SESDIR))
    {
	// Don't allow both "sesdir" and "curdir".
	(ശൂന്യം)opt_strings_flags(args->os_oldval.string, p_ssop_മൂല്യങ്ങൾ,
							&ssop_flags, TRUE);
	തിരികെ e_invalid_argument;
    }

    തിരികെ ശൂന്യ;
}

    പൂർണ്ണസംഖ്യ
expand_set_sessionoptions(
    optexpand_T		*args,
    പൂർണ്ണസംഖ്യ			*അക്കം_Matches,
    char_u		***matches)
{
    തിരികെ expand_set_opt_string(
	    args,
	    p_ssop_മൂല്യങ്ങൾ,
	    ARRAY_LENGTH(p_ssop_മൂല്യങ്ങൾ) - 1,
	    അക്കം_Matches,
	    matches);
}
#endif

/*
 * Validate 'shellpipe'/'shellredir' option.
 */
    അക്ഷരം *
did_set_shellpipe_redir(optset_T *args)
{
    char_u	*p;
    bool	seen = false;

    വേണ്ടി (p = args->os_newval.string; *p != NUL; ++p)
    {
	എങ്കിൽ (*p != '%')
	    തുടരുക;

	എങ്കിൽ (p[1] == NUL)
	    തിരികെ e_invalid_format_string_single_percent_s;

	എങ്കിൽ (p[1] == '%')
	{
	    ++p;    // skip second %
	    തുടരുക;
	}

	എങ്കിൽ (p[1] == 's')
	{
	    എങ്കിൽ (seen)
		തിരികെ e_invalid_format_string_single_percent_s;

	    seen = true;
	    ++p;    // consume 's'
	    തുടരുക;
	}
	തിരികെ e_invalid_format_string_single_percent_s;
    }
    തിരികെ ശൂന്യ;
}

/*
 * The 'shortmess' option is changed.
 */
    അക്ഷരം *
did_set_shortmess(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    തിരികെ did_set_option_listflag(*varp, (char_u *)SHM_ALL, args->os_errbuf,
		    args->os_errbuflen);
}

    പൂർണ്ണസംഖ്യ
expand_set_shortmess(optexpand_T *args, പൂർണ്ണസംഖ്യ *അക്കം_Matches, char_u ***matches)
{
    തിരികെ expand_set_opt_listflag(args, (char_u*)SHM_ALL, അക്കം_Matches, matches);
}

#എങ്കിൽ defined(FEAT_LINEBREAK)
/*
 * The 'showbreak' option is changed.
 */
    അക്ഷരം *
did_set_showbreak(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u	*s;

    വേണ്ടി (s = *varp; *s; )
    {
	എങ്കിൽ (ptr2cells(s) != 1)
	    തിരികെ e_showbreak_contains_unprintable_or_wide_character;
	MB_PTR_ADV(s);
    }

    തിരികെ ശൂന്യ;
}
#endif

/*
 * The 'showcmdloc' option is changed.
 */
    അക്ഷരം *
did_set_showcmdloc(optset_T *args UNUSED)
{
    അക്ഷരം	*errmsg = did_set_opt_strings(p_sloc, p_sloc_മൂല്യങ്ങൾ, FALSE);

    എങ്കിൽ (errmsg == ശൂന്യ)
	comp_col();

    തിരികെ errmsg;
}

    പൂർണ്ണസംഖ്യ
expand_set_showcmdloc(optexpand_T *args, പൂർണ്ണസംഖ്യ *അക്കം_Matches, char_u ***matches)
{
    തിരികെ expand_set_opt_string(
	    args,
	    p_sloc_മൂല്യങ്ങൾ,
	    ARRAY_LENGTH(p_sloc_മൂല്യങ്ങൾ) - 1,
	    അക്കം_Matches,
	    matches);
}

#എങ്കിൽ defined(FEAT_SIGNS)
/*
 * The 'signcolumn' option is changed.
 */
    അക്ഷരം *
did_set_signcolumn(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

# എങ്കിൽ defined(FEAT_LINEBREAK)
    എങ്കിൽ (check_opt_strings(*varp, p_scl_മൂല്യങ്ങൾ, FALSE) != OK)
	തിരികെ e_invalid_argument;
    // When changing the 'signcolumn' to or from 'number', recompute the
    // width of the number column if 'number' or 'relativenumber' is set.
    എങ്കിൽ (((*args->os_oldval.string == 'n' && args->os_oldval.string[1] == 'u')
		|| (*curwin->w_p_scl == 'n' && *(curwin->w_p_scl + 1) =='u'))
	    && (curwin->w_p_nu || curwin->w_p_rnu))
	curwin->w_nrwidth_വരി_എണ്ണം = 0;
# endif

# എങ്കിൽ defined(FEAT_GUI)
    // In the GUI, when sign icons are used, a full screen clear is needed
    // to properly redraw the sign icons.
    എങ്കിൽ (gui.in_use && curbuf->b_signlist != ശൂന്യ)
	redraw_all_later(UPD_CLEAR);
# endif

    തിരികെ ശൂന്യ;
}

    പൂർണ്ണസംഖ്യ
expand_set_signcolumn(optexpand_T *args, പൂർണ്ണസംഖ്യ *അക്കം_Matches, char_u ***matches)
{
    തിരികെ expand_set_opt_string(
	    args,
	    p_scl_മൂല്യങ്ങൾ,
	    ARRAY_LENGTH(p_scl_മൂല്യങ്ങൾ) - 1,
	    അക്കം_Matches,
	    matches);
}
#endif

#എങ്കിൽ defined(FEAT_SPELL)
/*
 * The 'spellcapcheck' option is changed.
 */
    അക്ഷരം *
did_set_spellcapcheck(optset_T *args UNUSED)
{
    // compile the regexp program.
    തിരികെ compile_cap_prog(curwin->w_s);
}

/*
 * The 'spellfile' option is changed.
 */
    അക്ഷരം *
did_set_spellfile(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    എങ്കിൽ (!valid_spellfile(*varp))
	തിരികെ e_invalid_argument;

    // If there is a window for this buffer in which 'spell' is set load the
    // wordlists.
    തിരികെ did_set_spell_option();
}

/*
 * The 'spell' option is changed.
 */
    അക്ഷരം *
did_set_spelllang(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    എങ്കിൽ (!valid_spelllang(*varp))
	തിരികെ e_invalid_argument;

    // If there is a window for this buffer in which 'spell' is set load the
    // wordlists.
    തിരികെ did_set_spell_option();
}

/*
 * The 'spelloptions' option is changed.
 */
    അക്ഷരം *
did_set_spelloptions(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    എങ്കിൽ (**varp != NUL && STRCMP(*varp, "camel") != 0)
	തിരികെ e_invalid_argument;

    തിരികെ ശൂന്യ;
}

    പൂർണ്ണസംഖ്യ
expand_set_spelloptions(optexpand_T *args, പൂർണ്ണസംഖ്യ *അക്കം_Matches, char_u ***matches)
{
    സ്ഥിതം അക്ഷരം *(p_spo_മൂല്യങ്ങൾ[]) = {"camel", ശൂന്യ};
    തിരികെ expand_set_opt_string(
	    args,
	    p_spo_മൂല്യങ്ങൾ,
	    ARRAY_LENGTH(p_spo_മൂല്യങ്ങൾ) - 1,
	    അക്കം_Matches,
	    matches);
}

/*
 * The 'spellsuggest' option is changed.
 */
    അക്ഷരം *
did_set_spellsuggest(optset_T *args UNUSED)
{
    എങ്കിൽ (spell_check_sps() != OK)
	തിരികെ e_invalid_argument;

    തിരികെ ശൂന്യ;
}

    പൂർണ്ണസംഖ്യ
expand_set_spellsuggest(optexpand_T *args, പൂർണ്ണസംഖ്യ *അക്കം_Matches, char_u ***matches)
{
    തിരികെ expand_set_opt_string(
	    args,
	    p_sps_മൂല്യങ്ങൾ,
	    ARRAY_LENGTH(p_sps_മൂല്യങ്ങൾ) - 1,
	    അക്കം_Matches,
	    matches);
}
#endif

/*
 * The 'splitkeep' option is changed.
 */
    അക്ഷരം *
did_set_splitkeep(optset_T *args UNUSED)
{
    win_T	*wp;
    tabpage_T	*tp;
    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	wp->w_prev_height = wp->w_height;
	wp->w_prev_winrow = wp->w_winrow;
    }
    തിരികെ did_set_opt_strings(p_spk, p_spk_മൂല്യങ്ങൾ, FALSE);
}

    പൂർണ്ണസംഖ്യ
expand_set_splitkeep(optexpand_T *args, പൂർണ്ണസംഖ്യ *അക്കം_Matches, char_u ***matches)
{
    തിരികെ expand_set_opt_string(
	    args,
	    p_spk_മൂല്യങ്ങൾ,
	    ARRAY_LENGTH(p_spk_മൂല്യങ്ങൾ) - 1,
	    അക്കം_Matches,
	    matches);
}

#എങ്കിൽ defined(FEAT_STL_OPT)
/*
 * The 'statusline' option is changed.
 */
    അക്ഷരം *
did_set_statusline(optset_T *args)
{
    char_u  **varp = (char_u **)args->os_varp;
    അക്ഷരം    *ret = parse_statustabline_rulerformat(args, FALSE);

    എങ്കിൽ (ret != ശൂന്യ)
	തിരികെ ret;
    update_stl_rendered_height(varp == &curwin->w_p_stl ? curwin : ശൂന്യ);
    frame_change_statusline_height();

    തിരികെ ശൂന്യ;
}

/*
 * Rewrite the 'statuslineopt' string: replace the last "maxheight:N" with
 * actual_stlh and remove earlier duplicates.  Keyword order is preserved.
 */
    സ്ഥിതം ശൂന്യം
update_stlo_maxheight(char_u **varp, പൂർണ്ണസംഖ്യ actual_stlh)
{
    സ്ഥിരം char_u    *src = *varp;
    സ്ഥിരം char_u    *last_mh = ശൂന്യ;
    സ്ഥിരം char_u    *p;

    // Find the last "maxheight:" token.
    p = src;
    ആകുമ്പോൾ (*p != NUL)
    {
	എങ്കിൽ (STRNCMP(p, "maxheight:", 10) == 0)
	    last_mh = p;
	ആകുമ്പോൾ (*p != ',' && *p != NUL)
	    ++p;
	എങ്കിൽ (*p == ',')
	    ++p;
    }

    എങ്കിൽ (last_mh == ശൂന്യ)
	തിരികെ;

    size_t	bufsize = STRLEN(src) + NUMBUFLEN + 1;
    char_u	*buf = alloc(bufsize);
    എങ്കിൽ (buf == ശൂന്യ)
	തിരികെ;

    പൂർണ്ണസംഖ്യ		len = 0;
    bool	need_comma = false;

    p = src;
    ആകുമ്പോൾ (*p != NUL)
    {
	സ്ഥിരം	char_u	*tok = p;
	പൂർണ്ണസംഖ്യ	tok_len;

	ആകുമ്പോൾ (*p != ',' && *p != NUL)
	    ++p;
	tok_len = (പൂർണ്ണസംഖ്യ)(p - tok);
	എങ്കിൽ (*p == ',')
	    ++p;

	എങ്കിൽ (STRNCMP(tok, "maxheight:", 10) == 0)
	{
	    എങ്കിൽ (tok == last_mh)
	    {
		// Replace the last occurrence with the actual value.
		എങ്കിൽ (need_comma)
		    buf[len++] = ',';
		len += vim_snprintf((അക്ഷരം *)buf + len,
			bufsize - len, "maxheight:%d", actual_stlh);
		need_comma = true;
	    }
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (need_comma)
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
    അക്ഷരം *
did_set_statuslineopt(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    win_T       *wp = varp == &curwin->w_p_stlo ? curwin : ശൂന്യ;

    എങ്കിൽ (statuslineopt_changed(*varp, wp) == FAIL)
	തിരികെ e_invalid_argument;

    // Sync stl_rendered_height with the current statusline.
    update_stl_rendered_height(wp);

    // Update the maxheight value to the actual value set.
    // Note: Must be changed if p_stlo_values are changed.
    എങ്കിൽ (*varp != empty_option)
    {
	പൂർണ്ണസംഖ്യ actual_stlh;

	എങ്കിൽ (wp != ശൂന്യ)
	{
	    frame_change_statusline_height();
	    actual_stlh = MIN(wp->w_p_stlo_mh,
		    wp->w_height + wp->w_status_height - p_wmh);
	}
	അല്ലെങ്കിൽ
	    actual_stlh = frame_change_statusline_height();

	// Only re-serialize when the window actually has a status line shown.
	എങ്കിൽ (actual_stlh > 0)
	{
	    update_stlo_maxheight(varp, actual_stlh);
	    // Update the parsed maxheight member directly.
	    എങ്കിൽ (wp != ശൂന്യ)
		wp->w_p_stlo_mh = actual_stlh;
	    അല്ലെങ്കിൽ
		set_stlo_mh(actual_stlh);
	}
    }
    അല്ലെങ്കിൽ
	frame_change_statusline_height();

    എങ്കിൽ ((args->os_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	// :set clears the local values.
	clear_string_option(&curwin->w_p_stlo);
	curwin->w_p_stlo_fh = -1;
	curwin->w_p_stlo_mh = -1;
    }

    തിരികെ ശൂന്യ;
}

    പൂർണ്ണസംഖ്യ
expand_set_statuslineopt(optexpand_T *args, പൂർണ്ണസംഖ്യ *അക്കം_Matches, char_u ***matches)
{
    തിരികെ expand_set_opt_string(
	    args,
	    p_stlo_മൂല്യങ്ങൾ,
	    ARRAY_LENGTH(p_stlo_മൂല്യങ്ങൾ) - 1,
	    അക്കം_Matches,
	    matches);
}
#endif

/*
 * The 'swapsync' option is changed.
 */
    അക്ഷരം *
did_set_swapsync(optset_T *args UNUSED)
{
    തിരികെ did_set_opt_strings(p_sws, p_sws_മൂല്യങ്ങൾ, FALSE);
}

    പൂർണ്ണസംഖ്യ
expand_set_swapsync(optexpand_T *args, പൂർണ്ണസംഖ്യ *അക്കം_Matches, char_u ***matches)
{
    തിരികെ expand_set_opt_string(
	    args,
	    p_sws_മൂല്യങ്ങൾ,
	    ARRAY_LENGTH(p_sws_മൂല്യങ്ങൾ) - 1,
	    അക്കം_Matches,
	    matches);
}

/*
 * The 'switchbuf' option is changed.
 */
    അക്ഷരം *
did_set_switchbuf(optset_T *args UNUSED)
{
    തിരികെ did_set_opt_flags(p_swb, p_swb_മൂല്യങ്ങൾ, &swb_flags, TRUE);
}

    പൂർണ്ണസംഖ്യ
expand_set_switchbuf(optexpand_T *args, പൂർണ്ണസംഖ്യ *അക്കം_Matches, char_u ***matches)
{
    തിരികെ expand_set_opt_string(
	    args,
	    p_swb_മൂല്യങ്ങൾ,
	    ARRAY_LENGTH(p_swb_മൂല്യങ്ങൾ) - 1,
	    അക്കം_Matches,
	    matches);
}

/*
 * The 'tabclose' option is changed.
 */
    അക്ഷരം *
did_set_tabclose(optset_T *args UNUSED)
{
    തിരികെ did_set_opt_flags(p_tcl, p_tcl_മൂല്യങ്ങൾ, &tcl_flags, TRUE);
}

    പൂർണ്ണസംഖ്യ
expand_set_tabclose(optexpand_T *args, പൂർണ്ണസംഖ്യ *അക്കം_Matches, char_u ***matches)
{
    തിരികെ expand_set_opt_string(
	    args,
	    p_tcl_മൂല്യങ്ങൾ,
	    ARRAY_LENGTH(p_tcl_മൂല്യങ്ങൾ) - 1,
	    അക്കം_Matches,
	    matches);
}

#എങ്കിൽ defined(FEAT_STL_OPT)
/*
 * The 'tabline' option is changed.
 */
    അക്ഷരം *
did_set_tabline(optset_T *args)
{
    തിരികെ parse_statustabline_rulerformat(args, FALSE);
}
#endif

/*
 * The 'tagcase' option is changed.
 */
    അക്ഷരം *
did_set_tagcase(optset_T *args)
{
    unsigned പൂർണ്ണസംഖ്യ	*flags;
    char_u		*p;

    എങ്കിൽ (args->os_flags & OPT_LOCAL)
    {
	p = curbuf->b_p_tc;
	flags = &curbuf->b_tc_flags;
    }
    അല്ലെങ്കിൽ
    {
	p = p_tc;
	flags = &tc_flags;
    }

    എങ്കിൽ ((args->os_flags & OPT_LOCAL) && *p == NUL)
	// make the local value empty: use the global value
	*flags = 0;
    അല്ലെങ്കിൽ എങ്കിൽ (*p == NUL
	    || opt_strings_flags(p, p_tc_മൂല്യങ്ങൾ, flags, FALSE) != OK)
	തിരികെ e_invalid_argument;

    തിരികെ ശൂന്യ;
}

    പൂർണ്ണസംഖ്യ
expand_set_tagcase(optexpand_T *args, പൂർണ്ണസംഖ്യ *അക്കം_Matches, char_u ***matches)
{
    തിരികെ expand_set_opt_string(
	    args,
	    p_tc_മൂല്യങ്ങൾ,
	    ARRAY_LENGTH(p_tc_മൂല്യങ്ങൾ) - 1,
	    അക്കം_Matches,
	    matches);
}

/*
 * The 'term' option is changed.
 */
    അക്ഷരം *
did_set_term(optset_T *args UNUSED)
{
    എങ്കിൽ (T_NAME[0] == NUL)
	തിരികെ e_cannot_set_term_to_empty_string;
#ifdef FEAT_GUI
    എങ്കിൽ (gui.in_use)
	തിരികെ e_cannot_change_term_in_GUI;
    എങ്കിൽ (term_is_gui(T_NAME))
	തിരികെ e_use_gui_to_start_GUI;
#endif
    എങ്കിൽ (set_termname(T_NAME) == FAIL)
	തിരികെ e_not_found_in_termcap;

    // Screen colors may have changed.
    redraw_later_clear();

    തിരികെ ശൂന്യ;
}

/*
 * Some terminal option (t_xxx) is changed
 */
    അക്ഷരം *
did_set_term_option(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    എങ്കിൽ (!full_screen)
	തിരികെ ശൂന്യ;

    // ":set t_Co=0" and ":set t_Co=1" do ":set t_Co="
    എങ്കിൽ (varp == &T_CCO)
    {
	പൂർണ്ണസംഖ്യ colors = atoi((അക്ഷരം *)T_CCO);

	// Only reinitialize colors if t_Co value has really changed to
	// avoid expensive reload of colorscheme if t_Co is set to the
	// same value multiple times.
	എങ്കിൽ (colors != t_colors)
	{
	    t_colors = colors;
	    എങ്കിൽ (t_colors <= 1)
	    {
		vim_free(T_CCO);
		T_CCO = empty_option;
	    }
#എങ്കിൽ defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
	    എങ്കിൽ (is_term_win32())
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
    എങ്കിൽ (varp == &T_ME)
    {
	out_str(T_ME);
	redraw_later(UPD_CLEAR);
#എങ്കിൽ defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
	// Since t_me has been set, this probably means that the user
	// wants to use this as default colors.  Need to reset default
	// background/foreground colors.
# ifdef VIMDLL
	എങ്കിൽ (!gui.in_use && !gui.starting)
# endif
	    mch_set_normal_colors();
#endif
    }
    എങ്കിൽ (varp == &T_BE && termcap_active)
    {
	MAY_WANT_TO_LOG_THIS;

	എങ്കിൽ (*T_BE == NUL)
	    // When clearing t_BE we assume the user no longer wants
	    // bracketed paste, thus disable it by writing t_BD.
	    out_str(T_BD);
	അല്ലെങ്കിൽ
	    out_str(T_BE);
    }

    എങ്കിൽ (varp == &T_BSU || varp == &T_ESU)
	term_set_sync_output(TERM_SYNC_OUTPUT_OFF);

    തിരികെ ശൂന്യ;
}


#ifdef UNIX
/*
 * The 'termresize' option is changed.
 */
    അക്ഷരം *
did_set_termresize(optset_T *args UNUSED)
{
    // If empty or "inband", then attempt to enable in-band resize events.
    എങ്കിൽ (*p_trz == NUL || STRCMP(p_trz, "inband") == 0)
	term_set_win_resize(true);
    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(p_trz, "sigwinch") == 0)
	term_set_win_resize(false);
    അല്ലെങ്കിൽ
	തിരികെ e_invalid_argument;

    തിരികെ ശൂന്യ;
}

    പൂർണ്ണസംഖ്യ
expand_set_termresize(optexpand_T *args, പൂർണ്ണസംഖ്യ *അക്കം_Matches, char_u ***matches)
{
    തിരികെ expand_set_opt_string(
	    args,
	    p_trz_മൂല്യങ്ങൾ,
	    ARRAY_LENGTH(p_trz_മൂല്യങ്ങൾ) - 1,
	    അക്കം_Matches,
	    matches);
}
#endif

#എങ്കിൽ defined(FEAT_TERMINAL)
/*
 * The 'termwinkey' option is changed.
 */
    അക്ഷരം *
did_set_termwinkey(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    എങ്കിൽ ((*varp)[0] != NUL && string_to_താക്കോൽ(*varp, TRUE) == 0)
	തിരികെ e_invalid_argument;

    തിരികെ ശൂന്യ;
}

/*
 * The 'termwinsize' option is changed.
 */
    അക്ഷരം *
did_set_termwinsize(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u	*p;

    എങ്കിൽ ((*varp)[0] == NUL)
	തിരികെ ശൂന്യ;

    p = skipdigits(*varp);
    എങ്കിൽ (p == *varp || (*p != 'x' && *p != '*') || *skipdigits(p + 1) != NUL)
	തിരികെ e_invalid_argument;

    തിരികെ ശൂന്യ;
}

# എങ്കിൽ defined(MSWIN)
/*
 * The 'termwintype' option is changed.
 */
    അക്ഷരം *
did_set_termwintype(optset_T *args UNUSED)
{
    തിരികെ did_set_opt_strings(p_twt, p_twt_മൂല്യങ്ങൾ, FALSE);
}

    പൂർണ്ണസംഖ്യ
expand_set_termwintype(optexpand_T *args, പൂർണ്ണസംഖ്യ *അക്കം_Matches, char_u ***matches)
{
    തിരികെ expand_set_opt_string(
	    args,
	    p_twt_മൂല്യങ്ങൾ,
	    ARRAY_LENGTH(p_twt_മൂല്യങ്ങൾ) - 1,
	    അക്കം_Matches,
	    matches);
}
# endif
#endif

/*
 * The 'titlestring' option is changed.
 */
    അക്ഷരം *
did_set_titlestring(optset_T *args)
{
    പൂർണ്ണസംഖ്യ flagval = 0;

#ifdef FEAT_STL_OPT
    flagval = STL_IN_TITLE;
#endif
    തിരികെ parse_titleiconstring(args, flagval);
}

#എങ്കിൽ defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
/*
 * The 'toolbar' option is changed.
 */
    അക്ഷരം *
did_set_toolbar(optset_T *args UNUSED)
{
    എങ്കിൽ (opt_strings_flags(p_toolbar, p_toolbar_മൂല്യങ്ങൾ, &toolbar_flags,
								TRUE) != OK)
	തിരികെ e_invalid_argument;

    out_flush();
    gui_mch_show_toolbar((toolbar_flags &
		(TOOLBAR_TEXT | TOOLBAR_ICONS)) != 0);
    തിരികെ ശൂന്യ;
}

    പൂർണ്ണസംഖ്യ
expand_set_toolbar(optexpand_T *args, പൂർണ്ണസംഖ്യ *അക്കം_Matches, char_u ***matches)
{
    തിരികെ expand_set_opt_string(
	    args,
	    p_toolbar_മൂല്യങ്ങൾ,
	    ARRAY_LENGTH(p_toolbar_മൂല്യങ്ങൾ) - 1,
	    അക്കം_Matches,
	    matches);
}
#endif

#എങ്കിൽ defined(FEAT_TOOLBAR) && defined(FEAT_GUI_GTK)
/*
 * The 'toolbariconsize' option is changed.  GTK+ 2 only.
 */
    അക്ഷരം *
did_set_toolbariconsize(optset_T *args UNUSED)
{
    എങ്കിൽ (opt_strings_flags(p_tbis, p_tbis_മൂല്യങ്ങൾ, &tbis_flags, FALSE) != OK)
	തിരികെ e_invalid_argument;

    out_flush();
    gui_mch_show_toolbar((toolbar_flags &
		(TOOLBAR_TEXT | TOOLBAR_ICONS)) != 0);
    തിരികെ ശൂന്യ;
}

    പൂർണ്ണസംഖ്യ
expand_set_toolbariconsize(
    optexpand_T	*args,
    പൂർണ്ണസംഖ്യ		*അക്കം_Matches,
    char_u	***matches)
{
    തിരികെ expand_set_opt_string(
	    args,
	    p_tbis_മൂല്യങ്ങൾ,
	    ARRAY_LENGTH(p_tbis_മൂല്യങ്ങൾ) - 1,
	    അക്കം_Matches,
	    matches);
}
#endif

#എങ്കിൽ defined(UNIX) || defined(VMS)
/*
 * The 'ttymouse' option is changed.
 */
    അക്ഷരം *
did_set_ttymouse(optset_T *args UNUSED)
{
    അക്ഷരം *errmsg = ശൂന്യ;

    // Switch the mouse off before changing the escape sequences used for
    // that.
    mch_setmouse(FALSE);
    എങ്കിൽ (opt_strings_flags(p_ttym, p_ttym_മൂല്യങ്ങൾ, &ttym_flags, FALSE) != OK)
	errmsg = e_invalid_argument;
    അല്ലെങ്കിൽ
	check_mouse_termcode();
    എങ്കിൽ (termcap_active)
	setmouse();		// may switch it on again

    തിരികെ errmsg;
}

    പൂർണ്ണസംഖ്യ
expand_set_ttymouse(optexpand_T *args, പൂർണ്ണസംഖ്യ *അക്കം_Matches, char_u ***matches)
{
    തിരികെ expand_set_opt_string(
	    args,
	    p_ttym_മൂല്യങ്ങൾ,
	    ARRAY_LENGTH(p_ttym_മൂല്യങ്ങൾ) - 1,
	    അക്കം_Matches,
	    matches);
}
#endif

#എങ്കിൽ defined(FEAT_VARTABS)
/*
 * The 'varsofttabstop' option is changed.
 */
    അക്ഷരം *
did_set_varsofttabstop(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u *cp;

    എങ്കിൽ (!((*varp)[0]) || ((*varp)[0] == '0' && !((*varp)[1])))
	VIM_CLEAR(curbuf->b_p_vsts_array);
    അല്ലെങ്കിൽ
    {
	വേണ്ടി (cp = *varp; *cp; ++cp)
	{
	    എങ്കിൽ (vim_isdigit(*cp))
		തുടരുക;
	    എങ്കിൽ (*cp == ',' && cp > *varp && *(cp-1) != ',')
		തുടരുക;
	    തിരികെ e_invalid_argument;
	}

	പൂർണ്ണസംഖ്യ *oldarray = curbuf->b_p_vsts_array;
	എങ്കിൽ (tabstop_set(*varp, &(curbuf->b_p_vsts_array)) == OK)
	{
	    എങ്കിൽ (oldarray)
		vim_free(oldarray);
	}
	അല്ലെങ്കിൽ
	    തിരികെ e_invalid_argument;
    }

    തിരികെ ശൂന്യ;
}

/*
 * The 'vartabstop' option is changed.
 */
    അക്ഷരം *
did_set_vartabstop(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u *cp;

    എങ്കിൽ (!((*varp)[0]) || ((*varp)[0] == '0' && !((*varp)[1])))
	VIM_CLEAR(curbuf->b_p_vts_array);
    അല്ലെങ്കിൽ
    {
	വേണ്ടി (cp = *varp; *cp; ++cp)
	{
	    എങ്കിൽ (vim_isdigit(*cp))
		തുടരുക;
	    എങ്കിൽ (*cp == ',' && cp > *varp && *(cp-1) != ',')
		തുടരുക;
	    തിരികെ e_invalid_argument;
	}

	പൂർണ്ണസംഖ്യ *oldarray = curbuf->b_p_vts_array;

	എങ്കിൽ (tabstop_set(*varp, &(curbuf->b_p_vts_array)) == OK)
	{
	    vim_free(oldarray);
# ifdef FEAT_FOLDING
	    എങ്കിൽ (foldmethodIsIndent(curwin))
		foldUpdateAll(curwin);
# endif
	}
	അല്ലെങ്കിൽ
	    തിരികെ e_invalid_argument;
    }

    തിരികെ ശൂന്യ;
}
#endif

/*
 * The 'verbosefile' option is changed.
 */
    അക്ഷരം *
did_set_verbosefile(optset_T *args UNUSED)
{
    verbose_stop();
    എങ്കിൽ (*p_vfile != NUL && verbose_open() == FAIL)
	തിരികെ e_invalid_argument;

    തിരികെ ശൂന്യ;
}

#എങ്കിൽ defined(FEAT_SESSION)
/*
 * The 'viewoptions' option is changed.
 */
    അക്ഷരം *
did_set_viewoptions(optset_T *args UNUSED)
{
    തിരികെ did_set_opt_flags(p_vop, p_ssop_മൂല്യങ്ങൾ, &vop_flags, TRUE);
}
#endif

#എങ്കിൽ defined(FEAT_VIMINFO)
/*
 * The 'viminfo' option is changed.
 */
    അക്ഷരം *
did_set_viminfo(optset_T *args)
{
    char_u	*s;
    അക്ഷരം	*errmsg = ശൂന്യ;

    വേണ്ടി (s = p_viminfo; *s;)
    {
	// Check it's a valid character
	എങ്കിൽ (vim_strchr((char_u *)"!\"%'/:<@cfhnrs", *s) == ശൂന്യ)
	{
	    errmsg = illegal_char(args->os_errbuf, args->os_errbuflen, *s);
	    നിർത്തുക;
	}
	എങ്കിൽ (*s == 'n')	// name is always last one
	    നിർത്തുക;
	അല്ലെങ്കിൽ എങ്കിൽ (*s == 'r') // skip until next ','
	{
	    ആകുമ്പോൾ (*++s && *s != ',')
		;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (*s == '%')
	{
	    // optional number
	    ആകുമ്പോൾ (vim_isdigit(*++s))
		;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (*s == '!' || *s == 'h' || *s == 'c')
	    ++s;		// no extra chars
	അല്ലെങ്കിൽ		// must have a number
	{
	    ആകുമ്പോൾ (vim_isdigit(*++s))
		;

	    എങ്കിൽ (!VIM_ISDIGIT(*(s - 1)))
	    {
		എങ്കിൽ (args->os_errbuf != ശൂന്യ)
		{
		    vim_snprintf(args->os_errbuf, args->os_errbuflen,
			    _(e_missing_സംഖ്യ_after_angle_str_angle),
			    transchar_byte(*(s - 1)));
		    errmsg = args->os_errbuf;
		}
		അല്ലെങ്കിൽ
		    errmsg = "";
		നിർത്തുക;
	    }
	}
	എങ്കിൽ (*s == ',')
	    ++s;
	അല്ലെങ്കിൽ എങ്കിൽ (*s)
	{
	    എങ്കിൽ (args->os_errbuf != ശൂന്യ)
		errmsg = e_missing_comma;
	    അല്ലെങ്കിൽ
		errmsg = "";
	    നിർത്തുക;
	}
    }
    എങ്കിൽ (*p_viminfo && errmsg == ശൂന്യ && get_viminfo_parameter('\'') < 0)
	errmsg = e_must_specify_a_മൂല്യം;

    തിരികെ errmsg;
}
#endif

/*
 * The 'virtualedit' option is changed.
 */
    അക്ഷരം *
did_set_virtualedit(optset_T *args)
{
    char_u		*ve = p_ve;
    unsigned പൂർണ്ണസംഖ്യ	*flags = &ve_flags;

    എങ്കിൽ (args->os_flags & OPT_LOCAL)
    {
	ve = curwin->w_p_ve;
	flags = &curwin->w_ve_flags;
    }

    എങ്കിൽ ((args->os_flags & OPT_LOCAL) && *ve == NUL)
	// make the local value empty: use the global value
	*flags = 0;
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (opt_strings_flags(ve, p_ve_മൂല്യങ്ങൾ, flags, TRUE) != OK)
	    തിരികെ e_invalid_argument;
	അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(ve, args->os_oldval.string) != 0)
	{
	    // Recompute cursor position in case the new 've' setting
	    // changes something.
	    validate_virtcol();
	    coladvance(curwin->w_virtcol);
	}
    }

    തിരികെ ശൂന്യ;
}

    പൂർണ്ണസംഖ്യ
expand_set_virtualedit(optexpand_T *args, പൂർണ്ണസംഖ്യ *അക്കം_Matches, char_u ***matches)
{
    തിരികെ expand_set_opt_string(
	    args,
	    p_ve_മൂല്യങ്ങൾ,
	    ARRAY_LENGTH(p_ve_മൂല്യങ്ങൾ) - 1,
	    അക്കം_Matches,
	    matches);
}

/*
 * The 'whichwrap' option is changed.
 */
    അക്ഷരം *
did_set_whichwrap(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    // Add ',' to the list flags because 'whichwrap' is a flag
    // list that is comma-separated.
    തിരികെ did_set_option_listflag(*varp, (char_u *)(WW_ALL ","),
		    args->os_errbuf, args->os_errbuflen);
}

    പൂർണ്ണസംഖ്യ
expand_set_whichwrap(optexpand_T *args, പൂർണ്ണസംഖ്യ *അക്കം_Matches, char_u ***matches)
{
    തിരികെ expand_set_opt_listflag(args, (char_u*)WW_ALL, അക്കം_Matches, matches);
}

/*
 * The 'wildmode' option is changed.
 */
    അക്ഷരം *
did_set_wildmode(optset_T *args UNUSED)
{
    എങ്കിൽ (check_opt_wim() == FAIL)
	തിരികെ e_invalid_argument;
    തിരികെ ശൂന്യ;
}

    പൂർണ്ണസംഖ്യ
expand_set_wildmode(optexpand_T *args, പൂർണ്ണസംഖ്യ *അക്കം_Matches, char_u ***matches)
{
    തിരികെ expand_set_opt_string(
	    args,
	    p_wim_മൂല്യങ്ങൾ,
	    ARRAY_LENGTH(p_wim_മൂല്യങ്ങൾ) - 1,
	    അക്കം_Matches,
	    matches);
}

/*
 * The 'wildoptions' option is changed.
 */
    അക്ഷരം *
did_set_wildoptions(optset_T *args UNUSED)
{
    തിരികെ did_set_opt_strings(p_wop, p_wop_മൂല്യങ്ങൾ, TRUE);
}

    പൂർണ്ണസംഖ്യ
expand_set_wildoptions(optexpand_T *args, പൂർണ്ണസംഖ്യ *അക്കം_Matches, char_u ***matches)
{
    തിരികെ expand_set_opt_string(
	    args,
	    p_wop_മൂല്യങ്ങൾ,
	    ARRAY_LENGTH(p_wop_മൂല്യങ്ങൾ) - 1,
	    അക്കം_Matches,
	    matches);
}

#എങ്കിൽ defined(FEAT_WAK)
/*
 * The 'winaltkeys' option is changed.
 */
    അക്ഷരം *
did_set_winaltkeys(optset_T *args UNUSED)
{
    അക്ഷരം *errmsg = ശൂന്യ;

    എങ്കിൽ (*p_wak == NUL || check_opt_strings(p_wak, p_wak_മൂല്യങ്ങൾ, FALSE) != OK)
	errmsg = e_invalid_argument;
# ifdef FEAT_MENU
#  എങ്കിൽ defined(FEAT_GUI_MOTIF)
    അല്ലെങ്കിൽ എങ്കിൽ (gui.in_use)
	gui_motif_set_mnemonics(p_wak[0] == 'y' || p_wak[0] == 'm');
#  elif defined(FEAT_GUI_GTK)
    അല്ലെങ്കിൽ എങ്കിൽ (gui.in_use)
	gui_gtk_set_mnemonics(p_wak[0] == 'y' || p_wak[0] == 'm');
#  endif
# endif
    തിരികെ errmsg;
}

    പൂർണ്ണസംഖ്യ
expand_set_winaltkeys(optexpand_T *args, പൂർണ്ണസംഖ്യ *അക്കം_Matches, char_u ***matches)
{
    തിരികെ expand_set_opt_string(
	    args,
	    p_wak_മൂല്യങ്ങൾ,
	    ARRAY_LENGTH(p_wak_മൂല്യങ്ങൾ) - 1,
	    അക്കം_Matches,
	    matches);
}
#endif

/*
 * The 'wincolor' option is changed.
 */
    അക്ഷരം *
did_set_wincolor(optset_T *args UNUSED)
{
    update_wincolor(curwin, args->os_newval.string);
    തിരികെ ശൂന്യ;
}

/*
 * The 'winhighlight' option is changed
 */
    അക്ഷരം *
did_set_winhighlight(optset_T *args)
{
    തിരികെ update_winhighlight(curwin, args->os_newval.string);
}

/*
 * Expand 'winhighlight' option.
 */
    പൂർണ്ണസംഖ്യ
expand_set_winhighlight(optexpand_T *args, പൂർണ്ണസംഖ്യ *അക്കം_Matches, char_u ***matches)
{
    expand_T	    *xp = args->oe_xp;

    എങ്കിൽ ((xp->xp_pattern > args->oe_set_arg && *(xp->xp_pattern-1) == ':')
	    || xp->xp_pattern == args->oe_set_arg || *(xp->xp_pattern-1) == ',')
    {
	// After a ':' or after a ',', or at the start, so expand highlight
	// group name.

	// If starts with !, then expand 'highlight' occasions.
	എങ്കിൽ (*xp->xp_pattern == '!')
	    തിരികെ expand_hl_occasions(args, അക്കം_Matches, matches, '!');
	അല്ലെങ്കിൽ
	    തിരികെ expand_set_opt_generic(
		    args,
		    get_highlight_name,
		    അക്കം_Matches,
		    matches);
    }

    VIM_CLEAR(*matches);
    തിരികെ FAIL;
}

    പൂർണ്ണസംഖ്യ
expand_set_wincolor(optexpand_T *args, പൂർണ്ണസംഖ്യ *അക്കം_Matches, char_u ***matches)
{
    തിരികെ expand_set_opt_generic(
	    args,
	    get_highlight_name,
	    അക്കം_Matches,
	    matches);
}

#ifdef FEAT_SYN_HL
/*
 * When the 'syntax' option is set, load the syntax of that name.
 */
    സ്ഥിതം ശൂന്യം
do_syntax_autocmd(പൂർണ്ണസംഖ്യ മൂല്യം_changed)
{
    സ്ഥിതം പൂർണ്ണസംഖ്യ syn_recursive = 0;

    ++syn_recursive;
    // Only pass TRUE for "force" when the value changed or not used
    // recursively, to avoid endless recurrence.
    apply_autocmds(EVENT_SYNTAX, curbuf->b_p_syn, curbuf->b_fname,
	    മൂല്യം_changed || syn_recursive == 1, curbuf);
    curbuf->b_flags |= BF_SYN_SET;
    --syn_recursive;
}
#endif

/*
 * When the 'filetype' option is set, trigger the FileType autocommand.
 */
    സ്ഥിതം ശൂന്യം
do_filetype_autocmd(char_u **varp, പൂർണ്ണസംഖ്യ opt_flags, പൂർണ്ണസംഖ്യ മൂല്യം_changed)
{
    // Skip this when called from a modeline and the filetype was already set
    // to this value.
    എങ്കിൽ ((opt_flags & OPT_MODELINE) && !മൂല്യം_changed)
	തിരികെ;

    സ്ഥിതം പൂർണ്ണസംഖ്യ  ft_recursive = 0;
    പൂർണ്ണസംഖ്യ	    secure_save = secure;

    // Reset the secure flag, since the value of 'filetype' has
    // been checked to be safe.
    secure = 0;

    ++ft_recursive;
    curbuf->b_did_filetype = true;
    // Only pass TRUE for "force" when the value changed or not
    // used recursively, to avoid endless recurrence.
    apply_autocmds(EVENT_FILETYPE, curbuf->b_p_ft, curbuf->b_fname,
	    മൂല്യം_changed || ft_recursive == 1, curbuf);
    --ft_recursive;
    // Just in case the old "curbuf" is now invalid.
    എങ്കിൽ (varp != &(curbuf->b_p_ft))
	varp = ശൂന്യ;

    secure = secure_save;
}

#ifdef FEAT_SPELL
/*
 * When the 'spelllang' option is set, source the spell/LANG.vim file in
 * 'runtimepath'.
 */
    സ്ഥിതം ശൂന്യം
do_spelllang_source(ശൂന്യം)
{
    char_u	fname[200];
    char_u	*p;
    char_u	*q = curwin->w_s->b_p_spl;

    // Skip the first name if it is "cjk".
    എങ്കിൽ (STRNCMP(q, "cjk,", 4) == 0)
	q += 4;

    // They could set 'spellcapcheck' depending on the language.  Use the first
    // name in 'spelllang' up to '_region' or '.encoding'.
    വേണ്ടി (p = q; *p != NUL; ++p)
	എങ്കിൽ (!ASCII_ISALNUM(*p) && *p != '-')
	    നിർത്തുക;
    എങ്കിൽ (p > q)
    {
	vim_snprintf((അക്ഷരം *)fname, 200, "spell/%.*s.vim",
		(പൂർണ്ണസംഖ്യ)(p - q), q);
	source_runtime(fname, DIP_ALL);
    }
}
#endif

/*
 * Handle string options that need some action to perform when changed.
 * The new value must be allocated.
 * Returns NULL for success, or an untranslated error message for an error.
 */
    അക്ഷരം *
did_set_string_option(
    പൂർണ്ണസംഖ്യ		opt_idx,		// index in options[] table
    char_u	**varp,			// pointer to the option variable
    char_u	*oldval,		// previous value of the option
    char_u	*മൂല്യം,			// new value of the option
    അക്ഷരം	*errbuf,		// buffer for errors, or NULL
    size_t	errbuflen,		// length of error buffer
    പൂർണ്ണസംഖ്യ		opt_flags,		// OPT_LOCAL and/or OPT_GLOBAL
    set_op_T    op,			// OP_ADDING/OP_PREPENDING/OP_REMOVING
    പൂർണ്ണസംഖ്യ		*മൂല്യം_checked)		// value was checked to be safe, no
					// need to set P_INSECURE
{
    അക്ഷരം	*errmsg = ശൂന്യ;
    long_u	free_oldval = (get_option_flags(opt_idx) & P_ALLOCED);
    opt_did_set_cb_T did_set_cb = get_option_did_set_cb(opt_idx);
    optset_T	args;

    // 'ttytype' is an alias for 'term'.  Both 'term' and 'ttytype' point to
    // T_NAME.  If 'term' or 'ttytype' is modified, then use the index for the
    // 'term' option.  Only set the P_ALLOCED flag on 'term'.
    എങ്കിൽ (varp == &T_NAME)
    {
	opt_idx = findoption((char_u *)"term");
	എങ്കിൽ (opt_idx >= 0)
	{
	    free_oldval = (get_option_flags(opt_idx) & P_ALLOCED);
	    did_set_cb = get_option_did_set_cb(opt_idx);
	}
    }

    CLEAR_FIELD(args);

    // Disallow changing some options from secure mode
    എങ്കിൽ ((secure
#ifdef HAVE_SANDBOX
		|| sandbox != 0
#endif
		) && (get_option_flags(opt_idx) & P_SECURE))
	errmsg = e_not_allowed_here;
    // Check for a "normal" directory or file name in some options.
    അല്ലെങ്കിൽ എങ്കിൽ (check_illegal_path_names(opt_idx, varp))
	errmsg = e_invalid_argument;
    അല്ലെങ്കിൽ എങ്കിൽ (did_set_cb != ശൂന്യ)
    {
	args.os_varp = (char_u *)varp;
	args.os_idx = opt_idx;
	args.os_flags = opt_flags;
	args.os_op = op;
	args.os_oldval.string = oldval;
	args.os_newval.string = മൂല്യം;
	args.os_errbuf = errbuf;
	args.os_errbuflen = errbuflen;
	// Invoke the option specific callback function to validate and apply
	// the new option value.
	errmsg = did_set_cb(&args);

	// The 'keymap', 'filetype' and 'syntax' option callback functions
	// may change the os_value_checked field.
	*മൂല്യം_checked = args.os_മൂല്യം_checked;
    }

    // If an error is detected, restore the previous value.
    എങ്കിൽ (errmsg != ശൂന്യ)
    {
	free_string_option(*varp);
	*varp = oldval;
	// When resetting some values, need to act on it.
	എങ്കിൽ (args.os_restore_chartab)
	    (ശൂന്യം)init_chartab();
	എങ്കിൽ (varp == &p_hl)
	    (ശൂന്യം)highlight_changed();
    }
    അല്ലെങ്കിൽ
    {
#ifdef FEAT_EVAL
	// Remember where the option was set.
	set_option_sctx_idx(opt_idx, opt_flags, current_sctx);
#endif
	// Free string options that are in allocated memory.
	// Use "free_oldval", because recursiveness may change the flags under
	// our fingers (esp. init_highlight()).
	എങ്കിൽ (free_oldval)
	    free_string_option(oldval);
	set_option_flag(opt_idx, P_ALLOCED);

	എങ്കിൽ ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0
		&& is_global_local_option(opt_idx))
	{
	    // global option with local value set to use global value; free
	    // the local value and make it empty
	    char_u *p = get_option_varp_scope(opt_idx, OPT_LOCAL);
	    free_string_option(*(char_u **)p);
	    *(char_u **)p = empty_option;
	}

	// May set global value for local option.
	അല്ലെങ്കിൽ എങ്കിൽ (!(opt_flags & OPT_LOCAL) && opt_flags != OPT_GLOBAL)
	    set_string_option_global(opt_idx, varp);

	// Trigger the autocommand only after setting the flags.
#ifdef FEAT_SYN_HL
	എങ്കിൽ (varp == &(curbuf->b_p_syn))
	    do_syntax_autocmd(args.os_മൂല്യം_changed);
#endif
	അല്ലെങ്കിൽ എങ്കിൽ (varp == &(curbuf->b_p_ft))
	    do_filetype_autocmd(varp, opt_flags, args.os_മൂല്യം_changed);
#ifdef FEAT_SPELL
	എങ്കിൽ (varp == &(curwin->w_s->b_p_spl))
	    do_spelllang_source();
#endif
    }

    എങ്കിൽ (varp == &p_mouse)
    {
	എങ്കിൽ (*p_mouse == NUL)
	    mch_setmouse(FALSE);    // switch mouse off
	അല്ലെങ്കിൽ
	    setmouse();		    // in case 'mouse' changed
    }

    എങ്കിൽ (varp == &p_rtp)
    {
	export_myvimdir();
#എങ്കിൽ defined(FEAT_LUA)
	update_package_paths_in_lua();
#endif
    }

#എങ്കിൽ defined(FEAT_LINEBREAK)
    // Changing Formatlistpattern when briopt includes the list setting:
    // redraw
    എങ്കിൽ ((varp == &p_flp || varp == &(curbuf->b_p_flp))
	    && curwin->w_briopt_list)
	redraw_all_later(UPD_NOT_VALID);
#endif

    എങ്കിൽ (curwin->w_curswant != MAXCOL
		   && (get_option_flags(opt_idx) & (P_CURSWANT | P_RALL)) != 0
				&& (get_option_flags(opt_idx) & P_HLONLY) == 0)
	curwin->w_set_curswant = true;

    എങ്കിൽ ((opt_flags & OPT_NO_REDRAW) == 0)
    {
#ifdef FEAT_GUI
	// set when changing an option that only requires a redraw in the GUI
	പൂർണ്ണസംഖ്യ	redraw_gui_only = FALSE;

	എങ്കിൽ (varp == &p_go			// 'guioptions'
		|| varp == &p_guifont		// 'guifont'
# ifdef FEAT_GUI_TABLINE
		|| varp == &p_gtl		// 'guitablabel'
		|| varp == &p_gtt		// 'guitabtooltip'
# endif
# ifdef FEAT_XFONTSET
		|| varp == &p_guifontset	// 'guifontset'
# endif
		|| varp == &p_guifontwide	// 'guifontwide'
# എങ്കിൽ defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
		|| varp == &p_guiligatures	// 'guiligatures'
# endif
	    )
	    redraw_gui_only = TRUE;

	// check redraw when it's not a GUI option or the GUI is active.
	എങ്കിൽ (!redraw_gui_only || gui.in_use)
#endif
	    check_redraw(get_option_flags(opt_idx));
    }

#എങ്കിൽ defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
    എങ്കിൽ (args.os_did_swaptcap)
    {
	set_termname((char_u *)"win32");
	init_highlight(TRUE, FALSE);
    }
#endif

    തിരികെ errmsg;
}

/*
 * Check an option that can be a range of string values.
 *
 * Return OK for correct value, FAIL otherwise.
 * Empty is always OK.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
check_opt_strings(
    char_u	*val,
    അക്ഷരം	**മൂല്യങ്ങൾ,
    പൂർണ്ണസംഖ്യ		list)	    // when TRUE: accept a list of values
{
    തിരികെ opt_strings_flags(val, മൂല്യങ്ങൾ, ശൂന്യ, list);
}

/*
 * Handle an option that can be a range of string values.
 * Set a flag in "*flagp" for each string present.
 *
 * Return OK for correct value, FAIL otherwise.
 * Empty is always OK.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
opt_strings_flags(
    char_u	*val,		// new value
    അക്ഷരം	**മൂല്യങ്ങൾ,	// array of valid string values
    unsigned	*flagp,
    പൂർണ്ണസംഖ്യ		list)		// when TRUE: accept a list of values
{
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		len;
    unsigned	new_flags = 0;

    ആകുമ്പോൾ (*val)
    {
	വേണ്ടി (i = 0; ; ++i)
	{
	    എങ്കിൽ (മൂല്യങ്ങൾ[i] == ശൂന്യ)	// val not found in values[]
		തിരികെ FAIL;

	    len = (പൂർണ്ണസംഖ്യ)STRLEN(മൂല്യങ്ങൾ[i]);
	    എങ്കിൽ (STRNCMP(മൂല്യങ്ങൾ[i], val, len) == 0
		    && ((list && val[len] == ',') || val[len] == NUL))
	    {
		val += len + (val[len] == ',');
		new_flags |= (1 << i);
		നിർത്തുക;		// check next item in val list
	    }
	}
    }
    എങ്കിൽ (flagp != ശൂന്യ)
	*flagp = new_flags;

    തിരികെ OK;
}

/*
 * return OK if "p" is a valid fileformat name, FAIL otherwise.
 */
    പൂർണ്ണസംഖ്യ
check_ff_മൂല്യം(char_u *p)
{
    തിരികെ check_opt_strings(p, p_ff_മൂല്യങ്ങൾ, FALSE);
}

/*
 * Save the actual shortmess Flags and clear them temporarily to avoid that
 * file messages overwrites any output from the following commands.
 *
 * Caller must make sure to first call save_clear_shm_value() and then
 * restore_shm_value() exactly the same number of times.
 */
    ശൂന്യം
save_clear_shm_മൂല്യം(ശൂന്യം)
{
    എങ്കിൽ (STRLEN(p_shm) >= SHM_LEN)
    {
	iemsg(e_internal_error_shortmess_too_long);
	തിരികെ;
    }

    എങ്കിൽ (++set_shm_recursive == 1)
    {
	STRCPY(shm_buf, p_shm);
	set_option_മൂല്യം_give_err((char_u *)"shm", 0L, (char_u *)"", 0);
    }
}

/*
 * Restore the shortmess Flags set from the save_clear_shm_value() function.
 */
    ശൂന്യം
restore_shm_മൂല്യം(ശൂന്യം)
{
    എങ്കിൽ (--set_shm_recursive == 0)
    {
	set_option_മൂല്യം_give_err((char_u *)"shm", 0L, shm_buf, 0);
	vim_memset(shm_buf, 0, SHM_LEN);
    }
}

/*
 * Export the environment variable $MYVIMDIR to the first item in runtimepath
 */
    ശൂന്യം
export_myvimdir(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ		dofree = FALSE;
    char_u	*p;
    char_u	*q = p_rtp;
    string_T	buf;

    buf.string = alloc(MAXPATHL);
    എങ്കിൽ (buf.string == ശൂന്യ)
	തിരികെ;

    buf.length = (size_t)copy_option_part(&q, buf.string, MAXPATHL, ",");

    p = vim_getenv((char_u *)"MYVIMDIR", &dofree);

    എങ്കിൽ (p == ശൂന്യ || STRCMP(p, buf.string) != 0)
    {
	എങ്കിൽ (*buf.string != NUL && !after_pathsep(buf.string, buf.string + buf.length))
	    STRCPY(buf.string + buf.length, PATHSEPSTR);
#ifdef MSWIN
	// normalize path separators
	വേണ്ടി (q = buf.string; *q != NUL; q++)
	    എങ്കിൽ (*q == '/')
		*q = '\\';
#endif
	vim_setenv((char_u *)"MYVIMDIR", buf.string);
    }
    എങ്കിൽ (dofree)
	vim_free(p);
    vim_free(buf.string);
}
