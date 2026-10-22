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

#অন্তর্ভাৱ "vim.h"

স্থির char_u shm_buf[SHM_LEN];
স্থির পূর্ণাঙ্ক set_shm_recursive = 0;

স্থির ৱর্ণ *(p_ambw_মানসমূহ[]) = {"single", "double", রিক্ত};
স্থির ৱর্ণ *(p_bg_মানসমূহ[]) = {"light", "dark", রিক্ত};
স্থির ৱর্ণ *(p_bkc_মানসমূহ[]) = {"yes", "auto", "no", "breaksymlink", "breakhardlink", রিক্ত};
স্থির ৱর্ণ *(p_bo_মানসমূহ[]) = {"all", "backspace", "cursor", "complete",
				 "copy", "ctrlg", "error", "esc", "ex",
				 "hangul", "insertmode", "lang", "mess",
				 "showmatch", "operator", "register", "shell",
				 "spell", "term", "wildmode", রিক্ত};
#যদি defined(FEAT_LINEBREAK)
// Note: Keep this in sync with briopt_check()
স্থির ৱর্ণ *(p_briopt_মানসমূহ[]) = {"shift:", "min:", "sbr", "list:", "column:", রিক্ত};
#endif
#যদি defined(FEAT_TABPANEL)
// Note: Keep this in sync with tabpanelopt_changed()
স্থির ৱর্ণ *(p_tplo_মানসমূহ[]) = {"align:", "columns:", "scrollbar", "vert", রিক্ত};
স্থির ৱর্ণ *(p_tplo_align_মানসমূহ[]) = {"left", "right", রিক্ত};
#endif
#যদি defined(FEAT_DIFF)
// Note: Keep this in sync with diffopt_changed()
স্থির ৱর্ণ *(p_dip_মানসমূহ[]) = {"filler", "anchor", "context:", "iblank", "icase", "iwhite", "iwhiteall", "iwhiteeol", "horizontal", "vertical", "closeoff", "hiddenoff", "foldcolumn:", "followwrap", "internal", "indent-heuristic", "algorithm:", "inline:", "linematch:", রিক্ত};
স্থির ৱর্ণ *(p_dip_algorithm_মানসমূহ[]) = {"myers", "minimal", "patience", "histogram", রিক্ত};
স্থির ৱর্ণ *(p_dip_inline_মানসমূহ[]) = {"none", "simple", "char", "word", রিক্ত};
#endif
স্থির ৱর্ণ *(p_nf_মানসমূহ[]) = {"bin", "octal", "hex", "alpha", "unsigned", "blank", রিক্ত};
স্থির ৱর্ণ *(p_ff_মানসমূহ[]) = {FF_UNIX, FF_DOS, FF_MAC, রিক্ত};
#ifdef HAVE_CLIPMETHOD
// Note: Keep this in sync with did_set_clipboard()
স্থির ৱর্ণ *(p_cb_মানসমূহ[]) = {"unnamed", "unnamedplus",
# ifdef FEAT_CLIPBOARD
    "autoselect", "autoselectplus", "autoselectml", "html", "exclude:",
# endif
    রিক্ত};
#endif
#ifdef FEAT_CRYPT
স্থির ৱর্ণ *(p_cm_মানসমূহ[]) = {"zip", "blowfish", "blowfish2",
# ifdef FEAT_SODIUM
    "xchacha20", "xchacha20v2",
# endif
    রিক্ত};
#endif
স্থির ৱর্ণ *(p_cmp_মানসমূহ[]) = {"internal", "keepascii", রিক্ত};
#ifdef FEAT_SYN_HL
// Note: Keep this in sync with fill_culopt_flags()
স্থির ৱর্ণ *(p_culopt_মানসমূহ[]) = {"line", "screenline", "number", "both", রিক্ত};
#endif
স্থির ৱর্ণ *(p_dy_মানসমূহ[]) = {"lastline", "truncate", "uhex", রিক্ত};
স্থির ৱর্ণ *(p_jop_মানসমূহ[]) = {"stack", রিক্ত};
#ifdef FEAT_FOLDING
স্থির ৱর্ণ *(p_fdo_মানসমূহ[]) = {"all", "block", "hor", "mark", "percent",
				 "quickfix", "search", "tag", "insert",
				 "undo", "jump", রিক্ত};
#endif
// Note: Keep this in sync with match_keyprotocol()
স্থির ৱর্ণ *(p_kpc_protocol_মানসমূহ[]) = {"none", "mok2", "kitty", রিক্ত};
#ifdef FEAT_IMAGE
// Note: Keep this in sync with match_imageprotocol()
স্থির ৱর্ণ *(p_ipc_protocol_মানসমূহ[]) = {"none", "kitty", "sixel", রিক্ত};
#endif
#ifdef FEAT_PROP_POPUP
// Note: Keep this in sync with parse_popup_option()
স্থির ৱর্ণ *(p_popup_cpp_option_মানসমূহ[]) = {"align:", "border:",
    "borderhighlight:", "close:", "height:", "highlight:", "opacity:",
    "resize:", "shadow:", "width:", রিক্ত};
স্থির ৱর্ণ *(p_popup_pvp_option_মানসমূহ[]) = {"border:",
    "borderhighlight:", "close:", "height:", "highlight:", "opacity:",
    "resize:", "shadow:", "width:", রিক্ত};
স্থির ৱর্ণ *(p_popup_option_on_off_মানসমূহ[]) = {"on", "off", রিক্ত};
স্থির ৱর্ণ *(p_popup_cpp_border_মানসমূহ[]) = {"single", "double", "round",
    "ascii", "on", "off", "custom:", রিক্ত};
স্থির ৱর্ণ *(p_popup_pvp_border_মানসমূহ[]) = {"single", "double", "round",
    "ascii", "on", "off", "custom:", রিক্ত};
স্থির ৱর্ণ *(p_popup_option_align_মানসমূহ[]) = {"item", "menu", রিক্ত};
#endif
#যদি defined(FEAT_SPELL)
// Note: Keep this in sync with spell_check_sps()
স্থির ৱর্ণ *(p_sps_মানসমূহ[]) = {"best", "fast", "double", "expr:", "file:", "timeout:", রিক্ত};
#endif
#ifdef FEAT_SESSION
// Also used for 'viewoptions'!  Keep in sync with SSOP_ flags.
স্থির ৱর্ণ *(p_ssop_মানসমূহ[]) = {"buffers", "winpos", "resize", "winsize",
    "localoptions", "options", "help", "blank", "globals", "slash", "unix",
    "sesdir", "curdir", "folds", "cursor", "tabpages", "terminal", "skiprtp",
    রিক্ত};
#endif
#যদি defined(FEAT_STL_OPT)
স্থির ৱর্ণ *(p_stlo_মানসমূহ[]) = {"fixedheight", "maxheight:", রিক্ত};
#endif
// Keep in sync with SWB_ flags in option.h
স্থির ৱর্ণ *(p_swb_মানসমূহ[]) = {"useopen", "usetab", "split", "newtab", "vsplit", "uselast", রিক্ত};
স্থির ৱর্ণ *(p_spk_মানসমূহ[]) = {"cursor", "screen", "topline", রিক্ত};
স্থির ৱর্ণ *(p_tc_মানসমূহ[]) = {"followic", "ignore", "match", "followscs", "smart", রিক্ত};
// Keep in sync with TCL_ flags in option.h
স্থির ৱর্ণ *(p_tcl_মানসমূহ[]) = {"left", "uselast", রিক্ত};
#যদি defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
স্থির ৱর্ণ *(p_toolbar_মানসমূহ[]) = {"text", "icons", "tooltips", "horiz", রিক্ত};
#endif
#যদি defined(FEAT_TOOLBAR) && defined(FEAT_GUI_GTK)
স্থির ৱর্ণ *(p_tbis_মানসমূহ[]) = {"tiny", "small", "medium", "large", "huge", "giant", রিক্ত};
#endif
#যদি defined(UNIX) || defined(VMS)
স্থির ৱর্ণ *(p_ttym_মানসমূহ[]) = {"xterm", "xterm2", "dec", "netterm", "jsbterm", "pterm", "urxvt", "sgr", রিক্ত};
#endif
স্থির ৱর্ণ *(p_ve_মানসমূহ[]) = {"block", "insert", "all", "onemore", "none", "NONE", রিক্ত};
// Note: Keep this in sync with check_opt_wim()
স্থির ৱর্ণ *(p_wim_মানসমূহ[]) = {"full", "longest", "list", "lastused", "noselect", "noinsert", রিক্ত};
স্থির ৱর্ণ *(p_wop_মানসমূহ[]) = {"fuzzy", "tagfile", "pum", "exacttext", রিক্ত};
#ifdef FEAT_WAK
স্থির ৱর্ণ *(p_wak_মানসমূহ[]) = {"yes", "menu", "no", রিক্ত};
#endif
স্থির ৱর্ণ *(p_mousem_মানসমূহ[]) = {"extend", "popup", "popup_setpos", "mac", রিক্ত};
স্থির ৱর্ণ *(p_sel_মানসমূহ[]) = {"inclusive", "exclusive", "old", রিক্ত};
স্থির ৱর্ণ *(p_slm_মানসমূহ[]) = {"mouse", "key", "cmd", রিক্ত};
স্থির ৱর্ণ *(p_km_মানসমূহ[]) = {"startsel", "stopsel", রিক্ত};
#ifdef FEAT_BROWSE
স্থির ৱর্ণ *(p_bsdir_মানসমূহ[]) = {"current", "last", "buffer", রিক্ত};
#endif
স্থির ৱর্ণ *(p_scbopt_মানসমূহ[]) = {"ver", "hor", "jump", রিক্ত};
স্থির ৱর্ণ *(p_debug_মানসমূহ[]) = {"msg", "throw", "beep", রিক্ত};
স্থির ৱর্ণ *(p_ead_মানসমূহ[]) = {"both", "ver", "hor", রিক্ত};
স্থির ৱর্ণ *(p_buftype_মানসমূহ[]) = {"nofile", "nowrite", "quickfix", "help", "terminal", "acwrite", "prompt", "popup", রিক্ত};
স্থির ৱর্ণ *(p_bufhidden_মানসমূহ[]) = {"hide", "unload", "delete", "wipe", রিক্ত};
স্থির ৱর্ণ *(p_bs_মানসমূহ[]) = {"indent", "eol", "start", "nostop", রিক্ত};
#ifdef FEAT_FOLDING
স্থির ৱর্ণ *(p_fdm_মানসমূহ[]) = {"manual", "expr", "marker", "indent", "syntax",
# ifdef FEAT_DIFF
				"diff",
# endif
				রিক্ত};
স্থির ৱর্ণ *(p_fcl_মানসমূহ[]) = {"all", রিক্ত};
#endif
স্থির ৱর্ণ *(p_cfc_মানসমূহ[]) = {"keyword", "files", "whole_line", রিক্ত};
স্থির ৱর্ণ *(p_cot_মানসমূহ[]) = {"menu", "menuone", "longest", "preview", "popup", "popuphidden", "noinsert", "noselect", "fuzzy", "nosort", "preinsert", "nearest", রিক্ত};
#ifdef BACKSLASH_IN_FILENAME
স্থির ৱর্ণ *(p_csl_মানসমূহ[]) = {"slash", "backslash", রিক্ত};
#endif
#ifdef FEAT_SIGNS
স্থির ৱর্ণ *(p_scl_মানসমূহ[]) = {"yes", "no", "auto", "number", রিক্ত};
#endif
#ifdef UNIX
স্থির ৱর্ণ *(p_trz_মানসমূহ[]) = {"inband", "sigwinch", "", রিক্ত};
#endif
#যদি defined(MSWIN) && defined(FEAT_TERMINAL)
স্থির ৱর্ণ *(p_twt_মানসমূহ[]) = {"winpty", "conpty", "", রিক্ত};
#endif
স্থির ৱর্ণ *(p_sloc_মানসমূহ[]) = {"last", "statusline", "tabline", রিক্ত};
স্থির ৱর্ণ *(p_sws_মানসমূহ[]) = {"fsync", "sync", রিক্ত};

স্থির পূর্ণাঙ্ক check_opt_strings(char_u *val, ৱর্ণ **মানসমূহ, পূর্ণাঙ্ক list);
স্থির পূর্ণাঙ্ক opt_strings_flags(char_u *val, ৱর্ণ **মানসমূহ, unsigned *flagp, পূর্ণাঙ্ক list);

/*
 * After setting various option values: recompute variables that depend on
 * option values.
 */
    শূন্য
didset_string_options(শূন্য)
{
    (শূন্য)opt_strings_flags(p_cmp, p_cmp_মানসমূহ, &cmp_flags, TRUE);
    (শূন্য)opt_strings_flags(p_bkc, p_bkc_মানসমূহ, &bkc_flags, TRUE);
    (শূন্য)opt_strings_flags(p_bo, p_bo_মানসমূহ, &bo_flags, TRUE);
    (শূন্য)opt_strings_flags(p_cot, p_cot_মানসমূহ, &cot_flags, TRUE);
#ifdef FEAT_SESSION
    (শূন্য)opt_strings_flags(p_ssop, p_ssop_মানসমূহ, &ssop_flags, TRUE);
    (শূন্য)opt_strings_flags(p_vop, p_ssop_মানসমূহ, &vop_flags, TRUE);
#endif
#ifdef FEAT_FOLDING
    (শূন্য)opt_strings_flags(p_fdo, p_fdo_মানসমূহ, &fdo_flags, TRUE);
#endif
    (শূন্য)opt_strings_flags(p_dy, p_dy_মানসমূহ, &dy_flags, TRUE);
    (শূন্য)opt_strings_flags(p_jop, p_jop_মানসমূহ, &jop_flags, TRUE);
    (শূন্য)opt_strings_flags(p_tc, p_tc_মানসমূহ, &tc_flags, FALSE);
    (শূন্য)opt_strings_flags(p_ve, p_ve_মানসমূহ, &ve_flags, TRUE);
#যদি defined(UNIX) || defined(VMS)
    (শূন্য)opt_strings_flags(p_ttym, p_ttym_মানসমূহ, &ttym_flags, FALSE);
#endif
#যদি defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
    (শূন্য)opt_strings_flags(p_toolbar, p_toolbar_মানসমূহ, &toolbar_flags, TRUE);
#endif
#যদি defined(FEAT_TOOLBAR) && defined(FEAT_GUI_GTK)
    (শূন্য)opt_strings_flags(p_tbis, p_tbis_মানসমূহ, &tbis_flags, FALSE);
#endif
    (শূন্য)opt_strings_flags(p_swb, p_swb_মানসমূহ, &swb_flags, TRUE);
    (শূন্য)opt_strings_flags(p_tcl, p_tcl_মানসমূহ, &tcl_flags, TRUE);
}

#যদি defined(FEAT_EVAL)
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
    শূন্য
trigger_optionset_string(
	পূর্ণাঙ্ক	opt_idx,
	পূর্ণাঙ্ক	opt_flags,
	char_u  *oldval,
	char_u  *oldval_l,
	char_u  *oldval_g,
	char_u  *newval)
{
    // Don't do this recursively.
    যদি (oldval == রিক্ত || newval == রিক্ত
				    || *get_vim_var_str(VV_OPTION_TYPE) != NUL)
	ঘূৰাই;

    char_u buf_type[7];
    size_t buf_typelen;
    size_t oldvallen;

    oldvallen = STRLEN(oldval);
    set_vim_var_string(VV_OPTION_OLD, oldval, (পূর্ণাঙ্ক)oldvallen);
    set_vim_var_string(VV_OPTION_NEW, newval, -1);
    buf_typelen = vim_snprintf_safelen((ৱর্ণ *)buf_type, পরিমাণ(buf_type),
	"%s", (opt_flags & OPT_LOCAL) ? "local" : "global");
    set_vim_var_string(VV_OPTION_TYPE, buf_type, (পূর্ণাঙ্ক)buf_typelen);
    যদি (opt_flags & OPT_LOCAL)
    {
	set_vim_var_string(VV_OPTION_COMMAND, (char_u *)"setlocal", STRLEN_LITERAL("setlocal"));
	set_vim_var_string(VV_OPTION_OLDLOCAL, oldval, (পূর্ণাঙ্ক)oldvallen);
    }
    যদি (opt_flags & OPT_GLOBAL)
    {
	set_vim_var_string(VV_OPTION_COMMAND, (char_u *)"setglobal", STRLEN_LITERAL("setglobal"));
	set_vim_var_string(VV_OPTION_OLDGLOBAL, oldval, (পূর্ণাঙ্ক)oldvallen);
    }
    যদি ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	set_vim_var_string(VV_OPTION_COMMAND, (char_u *)"set", STRLEN_LITERAL("set"));
	set_vim_var_string(VV_OPTION_OLDLOCAL, oldval_l, -1);
	set_vim_var_string(VV_OPTION_OLDGLOBAL, oldval_g, -1);
    }
    যদি (opt_flags & OPT_MODELINE)
    {
	set_vim_var_string(VV_OPTION_COMMAND, (char_u *)"modeline", STRLEN_LITERAL("modeline"));
	set_vim_var_string(VV_OPTION_OLDLOCAL, oldval, (পূর্ণাঙ্ক)oldvallen);
    }
    apply_autocmds(EVENT_OPTIONSET,
	    get_option_fullname(opt_idx), রিক্ত, FALSE,
	    রিক্ত);
    reset_v_option_vars();
}
#endif

    স্থির ৱর্ণ *
illegal_char(ৱর্ণ *errbuf, size_t errbuflen, পূর্ণাঙ্ক c)
{
    যদি (errbuf == রিক্ত)
	ঘূৰাই "";
    vim_snprintf(errbuf, errbuflen, _(e_illegal_character_str),
		    (ৱর্ণ *)transchar(c));
    ঘূৰাই errbuf;
}

    স্থির ৱর্ণ *
illegal_char_after_chr(ৱর্ণ *errbuf, size_t errbuflen, পূর্ণাঙ্ক c)
{
    যদি (errbuf == রিক্ত)
	ঘূৰাই "";
    vim_snprintf(errbuf, errbuflen, _(e_illegal_character_after_chr), c);
    ঘূৰাই errbuf;
}

/*
 * Check string options in a buffer for NULL value.
 */
    শূন্য
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
#যদি defined(FEAT_EVAL)
    check_string_option(&buf->b_p_inde);
    check_string_option(&buf->b_p_indk);
#endif
#যদি defined(FEAT_BEVAL) && defined(FEAT_EVAL)
    check_string_option(&buf->b_p_bexpr);
#endif
#যদি defined(FEAT_CRYPT)
    check_string_option(&buf->b_p_cm);
#endif
    check_string_option(&buf->b_p_fp);
#যদি defined(FEAT_EVAL)
    check_string_option(&buf->b_p_fex);
#endif
#ifdef FEAT_CRYPT
    check_string_option(&buf->b_p_চাবি);
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
    শূন্য
free_string_option(char_u *p)
{
    যদি (p != empty_option)
	vim_free(p);
}

    শূন্য
clear_string_option(char_u **pp)
{
    যদি (*pp != empty_option)
	vim_free(*pp);
    *pp = empty_option;
}

    শূন্য
check_string_option(char_u **pp)
{
    যদি (*pp == রিক্ত)
	*pp = empty_option;
}

/*
 * Set global value for string option when it's a local option.
 */
    স্থির শূন্য
set_string_option_global(
    পূর্ণাঙ্ক		opt_idx,	// option index
    char_u	**varp)		// pointer to option variable
{
    char_u	**p, *s;

    // the global value is always allocated
    যদি (is_window_local_option(opt_idx))
	p = (char_u **)GLOBAL_WO(varp);
    অন্যথা
	p = (char_u **)get_option_var(opt_idx);
    যদি (!is_global_option(opt_idx)
	    && p != varp
	    && (s = vim_strsave(*varp)) != রিক্ত)
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
    শূন্য
set_string_option_direct(
    char_u	*name,
    পূর্ণাঙ্ক		opt_idx,
    char_u	*val,
    পূর্ণাঙ্ক		opt_flags,	// OPT_FREE, OPT_LOCAL and/or OPT_GLOBAL
    পূর্ণাঙ্ক		set_sid UNUSED)
{
    char_u	*s;
    char_u	**varp;
    পূর্ণাঙ্ক		both = (opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0;
    পূর্ণাঙ্ক		idx = opt_idx;

    যদি (idx == -1)		// use name
    {
	idx = findoption(name);
	যদি (idx < 0)	// not found (should not happen)
	{
	    semsg(_(e_internal_error_str), "set_string_option_direct()");
	    siemsg("For option %s", name);
	    ঘূৰাই;
	}
    }

    যদি (is_hidden_option(idx))		// can't set hidden option
	ঘূৰাই;

    s = vim_strsave(val);
    যদি (s == রিক্ত)
	ঘূৰাই;

    varp = (char_u **)get_option_varp_scope(idx,
	    both ? OPT_LOCAL : opt_flags);
    যদি ((opt_flags & OPT_FREE) && (get_option_flags(idx) & P_ALLOCED))
	free_string_option(*varp);
    *varp = s;

    // For buffer/window local option may also set the global value.
    যদি (both)
	set_string_option_global(idx, varp);

    set_option_flag(idx, P_ALLOCED);

    // When setting both values of a global option with a local value,
    // make the local value empty, so that the global value is used.
    যদি (is_global_local_option(idx) && both)
    {
	free_string_option(*varp);
	*varp = empty_option;
    }
#ifdef FEAT_EVAL
    যদি (set_sid != SID_NONE)
    {
	sctx_T script_ctx;

	যদি (set_sid == 0)
	    script_ctx = current_sctx;
	অন্যথা
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
    শূন্য
set_string_option_direct_in_win(
	win_T		*wp,
	char_u		*name,
	পূর্ণাঙ্ক		opt_idx,
	char_u		*val,
	পূর্ণাঙ্ক		opt_flags,
	পূর্ণাঙ্ক		set_sid)
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

#যদি defined(FEAT_PROP_POPUP)
/*
 * Like set_string_option_direct(), but for a buffer-local option in "buf".
 * Blocks autocommands to avoid the old curbuf becoming invalid.
 */
    শূন্য
set_string_option_direct_in_buf(
	buf_T		*buf,
	char_u		*name,
	পূর্ণাঙ্ক		opt_idx,
	char_u		*val,
	পূর্ণাঙ্ক		opt_flags,
	পূর্ণাঙ্ক		set_sid)
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
    ৱর্ণ *
set_string_option(
    পূর্ণাঙ্ক		opt_idx,
    char_u	*মান,
    পূর্ণাঙ্ক		opt_flags,	// OPT_LOCAL and/or OPT_GLOBAL
    ৱর্ণ	*errbuf,
    size_t	errbuflen)
{
    char_u	*s;
    char_u	**varp;
    char_u	*oldval;
#যদি defined(FEAT_EVAL)
    char_u	*oldval_l = রিক্ত;
    char_u	*oldval_g = রিক্ত;
    char_u	*saved_oldval = রিক্ত;
    char_u	*saved_oldval_l = রিক্ত;
    char_u	*saved_oldval_g = রিক্ত;
    char_u	*saved_newval = রিক্ত;
#endif
    ৱর্ণ	*errmsg = রিক্ত;
    পূর্ণাঙ্ক		মান_checked = FALSE;

    যদি (is_hidden_option(opt_idx))	// don't set hidden option
	ঘূৰাই রিক্ত;

    s = vim_strsave(মান == রিক্ত ? (char_u *)"" : মান);
    যদি (s == রিক্ত)
	ঘূৰাই রিক্ত;

    varp = (char_u **)get_option_varp_scope(opt_idx,
	    (opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0
	    ? (is_global_local_option(opt_idx)
		? OPT_GLOBAL : OPT_LOCAL)
	    : opt_flags);
    oldval = *varp;
#যদি defined(FEAT_EVAL)
    যদি ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	oldval_l = *(char_u **)get_option_varp_scope(opt_idx, OPT_LOCAL);
	oldval_g = *(char_u **)get_option_varp_scope(opt_idx, OPT_GLOBAL);
    }
#endif
    *varp = s;

#যদি defined(FEAT_EVAL)
    যদি (!starting
# ifdef FEAT_CRYPT
	    && !is_crypt_চাবি_option(opt_idx)
# endif
       )
    {
	যদি (oldval_l != রিক্ত)
	    saved_oldval_l = vim_strsave(oldval_l);
	যদি (oldval_g != রিক্ত)
	    saved_oldval_g = vim_strsave(oldval_g);
	saved_oldval = vim_strsave(oldval);
	saved_newval = vim_strsave(s);
    }
#endif
    যদি ((errmsg = did_set_string_option(opt_idx, varp, oldval, মান, errbuf,
		    errbuflen, opt_flags, OP_NONE, &মান_checked)) == রিক্ত)
	did_set_option(opt_idx, opt_flags, TRUE, মান_checked);

#যদি defined(FEAT_EVAL)
    // call autocommand after handling side effects
    যদি (errmsg == রিক্ত)
	trigger_optionset_string(opt_idx, opt_flags,
		saved_oldval, saved_oldval_l,
		saved_oldval_g, saved_newval);
    vim_free(saved_oldval);
    vim_free(saved_oldval_l);
    vim_free(saved_oldval_g);
    vim_free(saved_newval);
#endif
    ঘূৰাই errmsg;
}

/*
 * Return TRUE if "val" is a valid 'filetype' name.
 * Also used for 'syntax' and 'keymap'.
 */
    স্থির পূর্ণাঙ্ক
valid_filetype(char_u *val)
{
    ঘূৰাই valid_name(val, ".-_");
}

#ifdef FEAT_STL_OPT
/*
 * Check validity of options with the 'statusline' format.
 * Return an untranslated error message or NULL.
 */
    স্থির ৱর্ণ *
check_stl_option(char_u *s)
{
    পূর্ণাঙ্ক		groupdepth = 0;
    স্থির ৱর্ণ errbuf[ERR_BUFLEN];
    পূর্ণাঙ্ক		errbuflen = ERR_BUFLEN;

    যেতিয়ালৈ (*s)
    {
	// Check for valid keys after % sequences
	যেতিয়ালৈ (*s && *s != '%')
	    s++;
	যদি (!*s)
	    ভঙ্গ;
	s++;
	যদি (*s == STL_CLICKFUNC)
	{
	    যদি (s[1] == ']')
	    {
		// %[] - end click region
		s += 2;
		অনুৱর্ত;
	    }
	    যদি (ASCII_ISALPHA(s[1]) || s[1] == '_')
	    {
		// %[FuncName] - start click region
		char_u *rb = vim_strchr(s + 2, ']');
		যদি (rb != রিক্ত)
		{
		    s = rb + 1;
		    অনুৱর্ত;
		}
	    }
	    // Bare %[ is invalid
	    ঘূৰাই illegal_char(errbuf, errbuflen, *s);
	}
	যদি (*s == STL_LINEBREAK)
	{
	    // Plain %@ - line break
	    s++;
	    অনুৱর্ত;
	}
	যদি (*s == '%' || *s == STL_TRUNCMARK || *s == STL_SEPARATE)
	{
	    s++;
	    অনুৱর্ত;
	}
	যদি (*s == ')')
	{
	    s++;
	    যদি (--groupdepth < 0)
		ভঙ্গ;
	    অনুৱর্ত;
	}
	যদি (*s == '-')
	    s++;
	যেতিয়ালৈ (VIM_ISDIGIT(*s))
	    s++;
	যদি (*s == STL_USER_HL)
	    অনুৱর্ত;
	যদি (*s == STL_CLICKFUNC)
	{
	    // %N[FuncName] or %N[]
	    যদি (s[1] == ']')
	    {
		s += 2;
		অনুৱর্ত;
	    }
	    যদি (ASCII_ISALPHA(s[1]) || s[1] == '_')
	    {
		char_u *rb = vim_strchr(s + 2, ']');
		যদি (rb != রিক্ত)
		{
		    s = rb + 1;
		    অনুৱর্ত;
		}
	    }
	    // Bare %N[ is invalid
	    ঘূৰাই illegal_char(errbuf, errbuflen, *s);
	}
	যদি (*s == STL_LINEBREAK)
	{
	    // %N@ - line break
	    s++;
	    অনুৱর্ত;
	}
	যদি (*s == '.')
	{
	    s++;
	    যেতিয়ালৈ (*s && VIM_ISDIGIT(*s))
		s++;
	}
	যদি (*s == '(')
	{
	    groupdepth++;
	    অনুৱর্ত;
	}
	যদি (vim_strchr(STL_ALL, *s) == রিক্ত)
	{
	    ঘূৰাই illegal_char(errbuf, errbuflen, *s);
	}
	যদি (*s == '{')
	{
	    পূর্ণাঙ্ক reevaluate = (*++s == '%');

	    যদি (reevaluate && *++s == '}')
		// "}" is not allowed immediately after "%{%"
		ঘূৰাই illegal_char(errbuf, errbuflen, '}');
	    যেতিয়ালৈ ((*s != '}' || (reevaluate && s[-1] != '%')) && *s)
		s++;
	    যদি (*s != '}')
		ঘূৰাই e_unclosed_expression_sequence;
	}
    }
    যদি (groupdepth != 0)
	ঘূৰাই e_unbalanced_groups;
    ঘূৰাই রিক্ত;
}
#endif

/*
 * Check for a "normal" directory or file name in some options.  Disallow a
 * path separator (slash and/or backslash), wildcards and characters that are
 * often illegal in a file name. Be more permissive if "secure" is off.
 */
    স্থির পূর্ণাঙ্ক
check_illegal_path_names(পূর্ণাঙ্ক opt_idx, char_u **varp)
{
    ঘূৰাই (((get_option_flags(opt_idx) & P_NFNAME)
		&& vim_strpbrk(*varp, (char_u *)(secure
			? "/\\*?[|;&<>\r\n" : "/\\*?[<>\r\n")) != রিক্ত)
	    || ((get_option_flags(opt_idx) & P_NDNAME)
		&& vim_strpbrk(*varp, (char_u *)"*?[|;&<>\r\n") != রিক্ত));
}

/*
 * An option that accepts a list of flags is changed.
 * e.g. 'viewoptions', 'switchbuf', 'casemap', etc.
 */
    স্থির ৱর্ণ *
did_set_opt_flags(char_u *val, ৱর্ণ **মানসমূহ, unsigned *flagp, পূর্ণাঙ্ক list)
{
    যদি (opt_strings_flags(val, মানসমূহ, flagp, list) == FAIL)
	ঘূৰাই e_invalid_argument;

    ঘূৰাই রিক্ত;
}

/*
 * An option that accepts a list of string values is changed.
 * e.g. 'nrformats', 'scrollopt', 'wildoptions', etc.
 */
    স্থির ৱর্ণ *
did_set_opt_strings(char_u *val, ৱর্ণ **মানসমূহ, পূর্ণাঙ্ক list)
{
    ঘূৰাই did_set_opt_flags(val, মানসমূহ, রিক্ত, list);
}

/*
 * An option which is a list of flags is set.  Valid values are in 'flags'.
 */
    স্থির ৱর্ণ *
did_set_option_listflag(
	char_u *val,
	char_u *flags,
	ৱর্ণ *errbuf,
	size_t errbuflen)
{
    char_u	*s;

    বাবে (s = val; *s; ++s)
	যদি (vim_strchr(flags, *s) == রিক্ত)
	    ঘূৰাই illegal_char(errbuf, errbuflen, *s);

    ঘূৰাই রিক্ত;
}

/*
 * Expand an option that accepts a list of fixed string values with known
 * number of items.
 */
    স্থির পূর্ণাঙ্ক
expand_set_opt_string(
	optexpand_T *args,
	ৱর্ণ **মানসমূহ,
	size_t অঙ্ক_মানসমূহ,
	পূর্ণাঙ্ক *অঙ্ক_Matches,
	char_u ***matches)
{
    char_u	*p;
    regmatch_T	*regmatch = args->oe_regmatch;
    পূর্ণাঙ্ক		include_orig_val = args->oe_include_orig_val;
    char_u	*option_val = args->oe_opt_মান;

    // Assume numValues is small since they are fixed enums, so just allocate
    // upfront instead of needing two passes to calculate output size.
    *matches = ALLOC_MULT(char_u *, অঙ্ক_মানসমূহ + 1);
    যদি (*matches == রিক্ত)
	ঘূৰাই FAIL;

    পূর্ণাঙ্ক গুনতি = 0;

    যদি (include_orig_val && *option_val != NUL)
    {
	p = vim_strsave(option_val);
	যদি (p == রিক্ত)
	{
	    VIM_CLEAR(*matches);
	    ঘূৰাই FAIL;
	}
	(*matches)[গুনতি++] = p;
    }

    বাবে (ৱর্ণ **val = মানসমূহ; *val != রিক্ত; val++)
    {
	যদি (include_orig_val && *option_val != NUL)
	{
	    যদি (STRCMP((char_u*)*val, option_val) == 0)
		অনুৱর্ত;
	}
	যদি (vim_regexec(regmatch, (char_u*)(*val), (colnr_T)0))
	{
	    p = vim_strsave((char_u*)*val);
	    যদি (p == রিক্ত)
	    {
		যদি (গুনতি == 0)
		{
		    VIM_CLEAR(*matches);
		    ঘূৰাই FAIL;
		}
		অন্যথা
		    ভঙ্গ;
	    }
	    (*matches)[গুনতি++] = p;
	}
    }
    যদি (গুনতি == 0)
    {
	VIM_CLEAR(*matches);
	ঘূৰাই FAIL;
    }
    *অঙ্ক_Matches = গুনতি;
    ঘূৰাই OK;
}

স্থির char_u *set_opt_callback_orig_option = রিক্ত;
স্থির char_u *((*set_opt_callback_func)(expand_T *, পূর্ণাঙ্ক));

/*
 * Callback used by expand_set_opt_generic to also include the original value
 * as the first item.
 */
    স্থির char_u *
expand_set_opt_generic_cb(expand_T *xp, পূর্ণাঙ্ক idx)
{
    যদি (idx == 0)
    {
	যদি (set_opt_callback_orig_option != রিক্ত)
	    ঘূৰাই set_opt_callback_orig_option;
	অন্যথা
	    ঘূৰাই (char_u *)""; // empty strings are ignored
    }
    ঘূৰাই set_opt_callback_func(xp, idx - 1);
}

/*
 * Expand an option with a callback that iterates through a list of possible
 * names using an index.
 */
    স্থির পূর্ণাঙ্ক
expand_set_opt_generic(
	optexpand_T *args,
	char_u *((*func)(expand_T *, পূর্ণাঙ্ক)),
	পূর্ণাঙ্ক *অঙ্ক_Matches,
	char_u ***matches)
{
    পূর্ণাঙ্ক ret;

    set_opt_callback_orig_option = args->oe_include_orig_val ?
	args->oe_opt_মান : রিক্ত;
    set_opt_callback_func = func;

    ret = ExpandGeneric(
	    (char_u*)"", // not using fuzzy as currently EXPAND_STRING_SETTING doesn't use it
	    args->oe_xp,
	    args->oe_regmatch,
	    matches,
	    অঙ্ক_Matches,
	    expand_set_opt_generic_cb,
	    FALSE);

    set_opt_callback_orig_option = রিক্ত;
    set_opt_callback_func = রিক্ত;
    ঘূৰাই ret;
}

#যদি defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
স্থির garray_T *expand_cb_ga;
স্থির optexpand_T *expand_cb_args;

/*
 * Callback provided to a function in expand_set_opt_callback. Will perform
 * regex matching against the value and add to the list.
 *
 * Returns OK usually. Returns FAIL if it failed to allocate memory, and the
 * caller should terminate the enumeration.
 */
    স্থির পূর্ণাঙ্ক
expand_set_opt_callback_cb(char_u *val)
{
    regmatch_T	*regmatch = expand_cb_args->oe_regmatch;
    expand_T	*xp = expand_cb_args->oe_xp;
    garray_T	*ga = expand_cb_ga;
    char_u	*str;

    যদি (val == রিক্ত || *val == NUL)
	ঘূৰাই OK;

    যদি (xp->xp_pattern[0] != NUL &&
	    !vim_regexec(regmatch, val, (colnr_T)0))
	ঘূৰাই OK;

    str = vim_strsave_escaped(val, (char_u *)" \t\\");

    যদি (str == রিক্ত)
	ঘূৰাই FAIL;

    যদি (ga_grow(ga, 1) == FAIL)
    {
	vim_free(str);
	ঘূৰাই FAIL;
    }

    ((char_u **)ga->ga_data)[ga->ga_len] = str;
    ++ga->ga_len;
    ঘূৰাই OK;
}

/*
 * Expand an option with a provided function that takes a callback. The
 * function will enumerate through all options and call the callback to add it
 * to the list.
 *
 * "func" is the enumerator function that will generate the list of options.
 * "func_params" is a single parameter that will be passed to func.
 */
    স্থির পূর্ণাঙ্ক
expand_set_opt_callback(
	optexpand_T *args,
	শূন্য (*func)(optexpand_T *, শূন্য* params, পূর্ণাঙ্ক (*cb)(char_u *val)),
	শূন্য *func_params,
	পূর্ণাঙ্ক *অঙ্ক_Matches,
	char_u ***matches)
{
    garray_T	ga;
    পূর্ণাঙ্ক		include_orig_val = args->oe_include_orig_val;
    char_u	*option_val = args->oe_opt_মান;

    ga_init2(&ga, পরিমাণ(ৱর্ণ *), 30);

    যদি (include_orig_val && *option_val != NUL)
    {
	char_u *p = vim_strsave(option_val);
	যদি (p == রিক্ত)
	    ঘূৰাই FAIL;
	যদি (ga_grow(&ga, 1) == FAIL)
	{
	    vim_free(p);
	    ঘূৰাই FAIL;
	}
	((char_u **)ga.ga_data)[ga.ga_len] = p;
	++ga.ga_len;
    }

    expand_cb_ga = &ga;
    expand_cb_args = args;

    func(args, func_params, expand_set_opt_callback_cb);

    expand_cb_ga = রিক্ত;
    expand_cb_args = রিক্ত;

    *matches = ga.ga_data;
    *অঙ্ক_Matches = ga.ga_len;
    ঘূৰাই OK;
}
#endif

/*
 * Expand an option which is a list of flags.
 */
    স্থির পূর্ণাঙ্ক
expand_set_opt_listflag(
	optexpand_T *args,
	char_u *flags,
	পূর্ণাঙ্ক *অঙ্ক_Matches,
	char_u ***matches)
{
    char_u  *p;
    char_u  *option_val = args->oe_opt_মান;
    char_u  *cmdline_val = args->oe_set_arg;
    পূর্ণাঙ্ক	    append = args->oe_append;
    পূর্ণাঙ্ক	    include_orig_val = args->oe_include_orig_val && (*option_val != NUL);

    size_t অঙ্ক_flags = STRLEN(flags);

    // Assume we only have small number of flags, so just allocate max size.
    *matches = ALLOC_MULT(char_u *, অঙ্ক_flags + 1);
    যদি (*matches == রিক্ত)
	ঘূৰাই FAIL;

    পূর্ণাঙ্ক গুনতি = 0;

    যদি (include_orig_val)
    {
	p = vim_strsave(option_val);
	যদি (p == রিক্ত)
	{
	    VIM_CLEAR(*matches);
	    ঘূৰাই FAIL;
	}
	(*matches)[গুনতি++] = p;
    }

    বাবে (char_u *flag = flags; *flag != NUL; flag++)
    {
	যদি (append && vim_strchr(option_val, *flag) != রিক্ত)
	    অনুৱর্ত;

	যদি (vim_strchr(cmdline_val, *flag) == রিক্ত)
	{
	    যদি (include_orig_val
		    && option_val[1] == NUL
		    && *flag == option_val[0])
	    {
		// This value is already used as the first choice as it's the
		// existing flag. Just skip it to avoid duplicate.
		অনুৱর্ত;
	    }
	    p = vim_strnsave(flag, 1);
	    যদি (p == রিক্ত)
	    {
		যদি (গুনতি == 0)
		{
		    VIM_CLEAR(*matches);
		    ঘূৰাই FAIL;
		}
		অন্যথা
		    ভঙ্গ;
	    }
	    (*matches)[গুনতি++] = p;
	}
    }

    যদি (গুনতি == 0)
    {
	VIM_CLEAR(*matches);
	ঘূৰাই FAIL;
    }
    *অঙ্ক_Matches = গুনতি;
    ঘূৰাই OK;
}

/*
 * The 'ambiwidth' option is changed.
 */
    ৱর্ণ *
did_set_ambiwidth(optset_T *args UNUSED)
{
    যদি (check_opt_strings(p_ambw, p_ambw_মানসমূহ, FALSE) != OK)
	ঘূৰাই e_invalid_argument;

    ঘূৰাই check_chars_options();
}

// "name" must be a string literal, the length is computed at compile time.
#define completing_মান_for_subopt(args, name) \
	  completing_মান_for_subopt_len(args, name, (পূর্ণাঙ্ক)STRLEN_LITERAL(name))

/*
 * Return true when completing the value of the sub-option "name" with length
 * "len", e.g. the value after "close:" in 'completepopup'.
 */
    স্থির bool
completing_মান_for_subopt_len(optexpand_T *args, ৱর্ণ *name, পূর্ণাঙ্ক len)
{
    char_u  *colon = args->oe_xp->xp_pattern - 1;
    পূর্ণাঙ্ক	    off = (পূর্ণাঙ্ক)(colon - args->oe_set_arg);

    যদি (off < len)
	ঘূৰাই false;
    // The name must follow a comma when it does not start the option value.
    যদি (off > len && *(colon - len - 1) != ',')
	ঘূৰাই false;

    ঘূৰাই STRNCMP(colon - len, name, len) == 0;
}

    পূর্ণাঙ্ক
expand_set_ambiwidth(optexpand_T *args, পূর্ণাঙ্ক *অঙ্ক_Matches, char_u ***matches)
{
    ঘূৰাই expand_set_opt_string(
	    args,
	    p_ambw_মানসমূহ,
	    ARRAY_LENGTH(p_ambw_মানসমূহ) - 1,
	    অঙ্ক_Matches,
	    matches);
}

/*
 * The 'background' option is changed.
 */
    ৱর্ণ *
did_set_background(optset_T *args)
{
    যদি (check_opt_strings(p_bg, p_bg_মানসমূহ, FALSE) == FAIL)
	ঘূৰাই e_invalid_argument;

    যদি (args->os_oldval.string != রিক্ত && args->os_oldval.string[0] == *p_bg)
	// Value was not changed
	ঘূৰাই রিক্ত;

#ifdef FEAT_EVAL
    পূর্ণাঙ্ক dark = (*p_bg == 'd');
#endif

    init_highlight(FALSE, FALSE);

#ifdef FEAT_EVAL
    যদি (dark != (*p_bg == 'd')
	    && get_var_মান((char_u *)"g:colors_name") != রিক্ত)
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

    ঘূৰাই রিক্ত;
}

    পূর্ণাঙ্ক
expand_set_background(optexpand_T *args, পূর্ণাঙ্ক *অঙ্ক_Matches, char_u ***matches)
{
    ঘূৰাই expand_set_opt_string(
	    args,
	    p_bg_মানসমূহ,
	    ARRAY_LENGTH(p_bg_মানসমূহ) - 1,
	    অঙ্ক_Matches,
	    matches);
}

/*
 * The 'backspace' option is changed.
 */
    ৱর্ণ *
did_set_backspace(optset_T *args UNUSED)
{
    যদি (VIM_ISDIGIT(*p_bs))
    {
	যদি (*p_bs > '3' || p_bs[1] != NUL)
	    ঘূৰাই e_invalid_argument;
    }
    অন্যথা যদি (check_opt_strings(p_bs, p_bs_মানসমূহ, TRUE) != OK)
	ঘূৰাই e_invalid_argument;

    ঘূৰাই রিক্ত;
}

    পূর্ণাঙ্ক
expand_set_backspace(optexpand_T *args, পূর্ণাঙ্ক *অঙ্ক_Matches, char_u ***matches)
{
    ঘূৰাই expand_set_opt_string(
	    args,
	    p_bs_মানসমূহ,
	    ARRAY_LENGTH(p_bs_মানসমূহ) - 1,
	    অঙ্ক_Matches,
	    matches);
}

/*
 * The 'backupcopy' option is changed.
 */
    ৱর্ণ *
did_set_backupcopy(optset_T *args)
{
    char_u		*bkc = p_bkc;
    unsigned পূর্ণাঙ্ক	*flags = &bkc_flags;
    ৱর্ণ		*errmsg = রিক্ত;

    যদি (args->os_flags & OPT_LOCAL)
    {
	bkc = curbuf->b_p_bkc;
	flags = &curbuf->b_bkc_flags;
    }
    অন্যথা যদি (!(args->os_flags & OPT_GLOBAL))
	// When using :set, clear the local flags.
	curbuf->b_bkc_flags = 0;

    যদি ((args->os_flags & OPT_LOCAL) && *bkc == NUL)
	// make the local value empty: use the global value
	*flags = 0;
    অন্যথা
    {
	যদি (opt_strings_flags(bkc, p_bkc_মানসমূহ, flags, TRUE) != OK)
	    errmsg = e_invalid_argument;
	যদি ((((পূর্ণাঙ্ক)*flags & BKC_AUTO) != 0)
		+ (((পূর্ণাঙ্ক)*flags & BKC_YES) != 0)
		+ (((পূর্ণাঙ্ক)*flags & BKC_NO) != 0) != 1)
	{
	    // Must have exactly one of "auto", "yes"  and "no".
	    (শূন্য)opt_strings_flags(args->os_oldval.string, p_bkc_মানসমূহ,
								  flags, TRUE);
	    errmsg = e_invalid_argument;
	}
    }

    ঘূৰাই errmsg;
}

    পূর্ণাঙ্ক
expand_set_backupcopy(optexpand_T *args, পূর্ণাঙ্ক *অঙ্ক_Matches, char_u ***matches)
{
    ঘূৰাই expand_set_opt_string(
	    args,
	    p_bkc_মানসমূহ,
	    ARRAY_LENGTH(p_bkc_মানসমূহ) - 1,
	    অঙ্ক_Matches,
	    matches);
}

/*
 * The 'backupext' or the 'patchmode' option is changed.
 */
    ৱর্ণ *
did_set_backupext_or_patchmode(optset_T *args UNUSED)
{
    যদি (STRCMP(*p_bex == '.' ? p_bex + 1 : p_bex,
		*p_pm == '.' ? p_pm + 1 : p_pm) == 0)
	ঘূৰাই e_backupext_and_patchmode_are_equal;

    ঘূৰাই রিক্ত;
}

/*
 * The 'belloff' option is changed.
 */
    ৱর্ণ *
did_set_belloff(optset_T *args UNUSED)
{
    ঘূৰাই did_set_opt_flags(p_bo, p_bo_মানসমূহ, &bo_flags, TRUE);
}

    পূর্ণাঙ্ক
expand_set_belloff(optexpand_T *args, পূর্ণাঙ্ক *অঙ্ক_Matches, char_u ***matches)
{
    ঘূৰাই expand_set_opt_string(
	    args,
	    p_bo_মানসমূহ,
	    ARRAY_LENGTH(p_bo_মানসমূহ) - 1,
	    অঙ্ক_Matches,
	    matches);
}

#যদি defined(FEAT_LINEBREAK)
/*
 * The 'breakat' option is changed.
 */
    ৱর্ণ *
did_set_breakat(optset_T *args UNUSED)
{
    char_u	*p;
    পূর্ণাঙ্ক		i;

    বাবে (i = 0; i < 256; i++)
	breakat_flags[i] = FALSE;

    যদি (p_breakat != রিক্ত)
	বাবে (p = p_breakat; *p; p++)
	    breakat_flags[*p] = TRUE;

    ঘূৰাই রিক্ত;
}

/*
 * The 'breakindentopt' option is changed.
 */
    ৱর্ণ *
did_set_breakindentopt(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    যদি (briopt_check(*varp, varp == &curwin->w_p_briopt ? curwin : রিক্ত)
								      == FAIL)
	ঘূৰাই e_invalid_argument;

    // list setting requires a redraw
    যদি (varp == &curwin->w_p_briopt && curwin->w_briopt_list)
	redraw_all_later(UPD_NOT_VALID);

    ঘূৰাই রিক্ত;
}

    পূর্ণাঙ্ক
expand_set_breakindentopt(
    optexpand_T		*args,
    পূর্ণাঙ্ক			*অঙ্ক_Matches,
    char_u		***matches)
{
    ঘূৰাই expand_set_opt_string(
	    args,
	    p_briopt_মানসমূহ,
	    ARRAY_LENGTH(p_briopt_মানসমূহ) - 1,
	    অঙ্ক_Matches,
	    matches);
}
#endif

#যদি defined(FEAT_BROWSE)
/*
 * The 'browsedir' option is changed.
 */
    ৱর্ণ *
did_set_browsedir(optset_T *args UNUSED)
{
    যদি (check_opt_strings(p_bsdir, p_bsdir_মানসমূহ, FALSE) != OK
	    && !mch_isdir(p_bsdir))
	ঘূৰাই e_invalid_argument;

    ঘূৰাই রিক্ত;
}

    পূর্ণাঙ্ক
expand_set_browsedir(optexpand_T *args, পূর্ণাঙ্ক *অঙ্ক_Matches, char_u ***matches)
{
    ঘূৰাই expand_set_opt_string(
	    args,
	    p_bsdir_মানসমূহ,
	    ARRAY_LENGTH(p_bsdir_মানসমূহ) - 1,
	    অঙ্ক_Matches,
	    matches);
}
#endif

/*
 * The 'bufhidden' option is changed.
 */
    ৱর্ণ *
did_set_bufhidden(optset_T *args UNUSED)
{
    ঘূৰাই did_set_opt_strings(curbuf->b_p_bh, p_bufhidden_মানসমূহ, FALSE);
}

    পূর্ণাঙ্ক
expand_set_bufhidden(optexpand_T *args, পূর্ণাঙ্ক *অঙ্ক_Matches, char_u ***matches)
{
    ঘূৰাই expand_set_opt_string(
	    args,
	    p_bufhidden_মানসমূহ,
	    ARRAY_LENGTH(p_bufhidden_মানসমূহ) - 1,
	    অঙ্ক_Matches,
	    matches);
}

/*
 * The 'buftype' option is changed.
 */
    ৱর্ণ *
did_set_buftype(optset_T *args UNUSED)
{
    যদি (check_opt_strings(curbuf->b_p_bt, p_buftype_মানসমূহ, FALSE) != OK)
	ঘূৰাই e_invalid_argument;

    যদি (curwin->w_status_height)
    {
	curwin->w_redr_status = true;
	redraw_later(UPD_VALID);
    }
    curbuf->b_help = (curbuf->b_p_bt[0] == 'h');
    redraw_titles();

    ঘূৰাই রিক্ত;
}

    পূর্ণাঙ্ক
expand_set_buftype(optexpand_T *args, পূর্ণাঙ্ক *অঙ্ক_Matches, char_u ***matches)
{
    ঘূৰাই expand_set_opt_string(
	    args,
	    p_buftype_মানসমূহ,
	    ARRAY_LENGTH(p_buftype_মানসমূহ) - 1,
	    অঙ্ক_Matches,
	    matches);
}

/*
 * The 'casemap' option is changed.
 */
    ৱর্ণ *
did_set_casemap(optset_T *args UNUSED)
{
    ঘূৰাই did_set_opt_flags(p_cmp, p_cmp_মানসমূহ, &cmp_flags, TRUE);
}

    পূর্ণাঙ্ক
expand_set_casemap(optexpand_T *args, পূর্ণাঙ্ক *অঙ্ক_Matches, char_u ***matches)
{
    ঘূৰাই expand_set_opt_string(
	    args,
	    p_cmp_মানসমূহ,
	    ARRAY_LENGTH(p_cmp_মানসমূহ) - 1,
	    অঙ্ক_Matches,
	    matches);
}

#যদি defined(HAVE_CLIPMETHOD)
    পূর্ণাঙ্ক
expand_set_clipboard(optexpand_T *args, পূর্ণাঙ্ক *অঙ্ক_Matches, char_u ***matches)
{
    ঘূৰাই expand_set_opt_string(
	    args,
	    p_cb_মানসমূহ,
	    ARRAY_LENGTH(p_cb_মানসমূহ) - 1,
	    অঙ্ক_Matches,
	    matches);
}
#endif

#ifdef HAVE_CLIPMETHOD
    ৱর্ণ *
did_set_clipmethod(optset_T *args UNUSED)
{
    ঘূৰাই choose_clipmethod();
}

    পূর্ণাঙ্ক
expand_set_clipmethod(optexpand_T *args, পূর্ণাঙ্ক *অঙ্ক_Matches, char_u ***matches)
{
    // We want to expand using the predefined clipmethod values + clipboard
    // provider names.
    পূর্ণাঙ্ক		result;
    ৱর্ণ	**মানসমূহ;
    পূর্ণাঙ্ক		গুনতি, pos = 0, start = 0;
# ifdef FEAT_EVAL
    dict_T	*providers = get_vim_var_dict(VV_CLIPPROVIDERS);
# অন্যথা
    dict_T	*providers = রিক্ত;
# endif
    hashtab_T	*ht = providers == রিক্ত ? রিক্ত : &providers->dv_hashtab;

    গুনতি = (ht == রিক্ত ? 0 : ht->ht_used);
# ifdef FEAT_WAYLAND_CLIPBOARD
    গুনতি++;
    start++;
# endif
# ifdef FEAT_XCLIPBOARD
    গুনতি++;
    start++;
# endif
    মানসমূহ = ALLOC_MULT(ৱর্ণ *, গুনতি + 1); // Add NULL terminator too

    যদি (মানসমূহ == রিক্ত)
	ঘূৰাই FAIL;

# ifdef FEAT_WAYLAND_CLIPBOARD
    মানসমূহ[pos++] = "wayland";
# endif
# ifdef FEAT_XCLIPBOARD
    মানসমূহ[pos++] = "x11";
# endif

    যদি (ht != রিক্ত)
	বাবে (long_u i = 0; i < ht->ht_mask + 1; i++)
	{
	    hashitem_T	*hi = ht->ht_array + i;

	    যদি (!HASHITEM_EMPTY(hi))
		মানসমূহ[pos++] = (ৱর্ণ *)vim_strsave(hi->hi_চাবি);
	}
    মানসমূহ[pos++] = রিক্ত;

    result = expand_set_opt_string(
	    args,
	    মানসমূহ,
	    গুনতি,
	    অঙ্ক_Matches,
	    matches);

    বাবে (পূর্ণাঙ্ক i = start; i < গুনতি; i++)
	vim_free(মানসমূহ[i]);
    vim_free(মানসমূহ);

    ঘূৰাই result;
}
#endif

/*
 * The global 'listchars' or 'fillchars' option is changed.
 */
    স্থির ৱর্ণ *
did_set_global_listfillchars(char_u *val, পূর্ণাঙ্ক opt_lcs, পূর্ণাঙ্ক opt_flags,
						ৱর্ণ *errbuf, size_t errbuflen)
{
    ৱর্ণ	*errmsg = রিক্ত;
    char_u	**local_ptr = opt_lcs ? &curwin->w_p_lcs : &curwin->w_p_fcs;

    // only apply the global value to "curwin" when it does not have a
    // local value
    যদি (opt_lcs)
	errmsg = set_listchars_option(curwin, val,
		**local_ptr == NUL || !(opt_flags & OPT_GLOBAL),
							    errbuf, errbuflen);
    অন্যথা
	errmsg = set_fillchars_option(curwin, val,
		**local_ptr == NUL || !(opt_flags & OPT_GLOBAL),
							    errbuf, errbuflen);
    যদি (errmsg != রিক্ত)
	ঘূৰাই errmsg;

    tabpage_T	*tp;
    win_T	*wp;

    // If the current window is set to use the global
    // 'listchars'/'fillchars' value, clear the window-local value.
    যদি (!(opt_flags & OPT_GLOBAL))
	clear_string_option(local_ptr);
    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	// If the current window has a local value need to apply it
	// again, it was changed when setting the global value.
	// If no error was returned above, we don't expect an error
	// here, so ignore the return value.
	যদি (opt_lcs)
	{
	    যদি (*wp->w_p_lcs == NUL)
		(শূন্য)set_listchars_option(wp, wp->w_p_lcs, TRUE, রিক্ত, 0);
	}
	অন্যথা
	{
	    যদি (*wp->w_p_fcs == NUL)
		(শূন্য)set_fillchars_option(wp, wp->w_p_fcs, TRUE, রিক্ত, 0);
	}
    }

    redraw_all_later(UPD_NOT_VALID);

    ঘূৰাই রিক্ত;
}

/*
 * The 'fillchars' option or the 'listchars' option is changed.
 */
    ৱর্ণ *
did_set_chars_option(optset_T *args)
{
    char_u **varp = (char_u **)args->os_varp;
    ৱর্ণ *errmsg = রিক্ত;

    যদি (   varp == &p_lcs		// global 'listchars'
	|| varp == &p_fcs)		// global 'fillchars'
	errmsg = did_set_global_listfillchars(*varp, varp == &p_lcs,
			  args->os_flags, args->os_errbuf, args->os_errbuflen);
    অন্যথা যদি (varp == &curwin->w_p_lcs)	// local 'listchars'
	errmsg = set_listchars_option(curwin, *varp, TRUE,
					  args->os_errbuf, args->os_errbuflen);
    অন্যথা যদি (varp == &curwin->w_p_fcs)	// local 'fillchars'
	errmsg = set_fillchars_option(curwin, *varp, TRUE,
					  args->os_errbuf, args->os_errbuflen);

    ঘূৰাই errmsg;
}

/*
 * Expand 'fillchars' or 'listchars' option value.
 */
    পূর্ণাঙ্ক
expand_set_chars_option(optexpand_T *args, পূর্ণাঙ্ক *অঙ্ক_Matches, char_u ***matches)
{
    char_u **varp = (char_u **)args->oe_varp;
    পূর্ণাঙ্ক is_lcs = (varp == &p_lcs || varp == &curwin->w_p_lcs);
    ঘূৰাই expand_set_opt_generic(
	    args,
	    is_lcs ? get_listchars_name : get_fillchars_name,
	    অঙ্ক_Matches,
	    matches);
}

/*
 * The 'cinoptions' option is changed.
 */
    ৱর্ণ *
did_set_cinoptions(optset_T *args UNUSED)
{
    // TODO: recognize errors
    parse_cino(curbuf);

    ঘূৰাই রিক্ত;
}

#যদি defined(FEAT_SYN_HL)
/*
 * The 'colorcolumn' option is changed.
 */
    ৱর্ণ *
did_set_colorcolumn(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    ঘূৰাই check_colorcolumn(*varp, varp == &curwin->w_p_cc ? curwin : রিক্ত);
}
#endif

/*
 * The 'comments' option is changed.
 */
    ৱর্ণ *
did_set_comments(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u	*s;
    ৱর্ণ	*errmsg = রিক্ত;

    বাবে (s = *varp; *s; )
    {
	যেতিয়ালৈ (*s && *s != ':')
	{
	    যদি (vim_strchr((char_u *)COM_ALL, *s) == রিক্ত
		    && !VIM_ISDIGIT(*s) && *s != '-')
	    {
		errmsg = illegal_char(args->os_errbuf, args->os_errbuflen, *s);
		ভঙ্গ;
	    }
	    ++s;
	}
	যদি (*s++ == NUL)
	    errmsg = e_missing_colon;
	অন্যথা যদি (*s == ',' || *s == NUL)
	    errmsg = e_zero_length_string;
	যদি (errmsg != রিক্ত)
	    ভঙ্গ;
	যেতিয়ালৈ (*s && *s != ',')
	{
	    যদি (*s == '\\' && s[1] != NUL)
		++s;
	    ++s;
	}
	s = skip_to_option_part(s);
    }

    ঘূৰাই errmsg;
}

/*
 * The 'commentstring' option is changed.
 */
    ৱর্ণ *
did_set_commentstring(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    যদি (**varp != NUL && strstr((ৱর্ণ *)*varp, "%s") == রিক্ত)
	ঘূৰাই e_commentstring_must_be_empty_or_contain_str;

    ঘূৰাই রিক্ত;
}

/*
 * Check if value for 'complete' is valid when 'complete' option is changed.
 */
    ৱর্ণ *
did_set_complete(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u	*p, *t;
    char_u	buffer[LSIZE];
    char_u	*buf_ptr;
    char_u	char_before = NUL;
    পূর্ণাঙ্ক		escape;

    বাবে (p = *varp; *p; )
    {
	vim_memset(buffer, 0, LSIZE);
	buf_ptr = buffer;
	escape = 0;

	// Extract substring while handling escaped commas
	যেতিয়ালৈ (*p && (*p != ',' || escape) && buf_ptr < (buffer + LSIZE - 1))
	{
	    যদি (*p == '\\' && *(p + 1) == ',')
	    {
		escape = 1;  // Mark escape mode
		p++;         // Skip '\'
	    }
	    অন্যথা
	    {
		escape = 0;
		*buf_ptr++ = *p;
	    }
	    p++;
	}
	*buf_ptr = NUL;

	যদি (vim_strchr((char_u *)".wbuksid]tUFo", *buffer) == রিক্ত)
	    ঘূৰাই illegal_char(args->os_errbuf, args->os_errbuflen, *buffer);

	যদি (vim_strchr((char_u *)"ksF", *buffer) == রিক্ত && *(buffer + 1) != NUL
		&& *(buffer + 1) != '^')
	    char_before = *buffer;
	অন্যথা
	{
	    // Test for a number after '^'
	    যদি ((t = vim_strchr(buffer, '^')) != রিক্ত)
	    {
		*t++ = NUL;
		যদি (!*t)
		    char_before = '^';
		অন্যথা
		{
		    বাবে (; *t; t++)
		    {
			যদি (!vim_isdigit(*t))
			{
			    char_before = '^';
			    ভঙ্গ;
			}
		    }
		}
	    }
	}
	যদি (char_before != NUL)
	    ঘূৰাই illegal_char_after_chr(args->os_errbuf, args->os_errbuflen,
		    char_before);
	// Skip comma and spaces
	যেতিয়ালৈ (*p == ',' || *p == ' ')
	    p++;
    }

#ifdef FEAT_COMPL_FUNC
    যদি (set_cpt_callbacks(args) != OK)
	ঘূৰাই illegal_char_after_chr(args->os_errbuf, args->os_errbuflen,
		'F');
#endif
    ঘূৰাই রিক্ত;
}

    পূর্ণাঙ্ক
expand_set_complete(optexpand_T *args, পূর্ণাঙ্ক *অঙ্ক_Matches, char_u ***matches)
{
    স্থির ৱর্ণ *(p_cpt_মানসমূহ[]) = {
	".", "w", "b", "u", "k", "kspell", "s", "i", "d", "]", "t", "U", "F", "o",
	রিক্ত};
    ঘূৰাই expand_set_opt_string(
	    args,
	    p_cpt_মানসমূহ,
	    ARRAY_LENGTH(p_cpt_মানসমূহ) - 1,
	    অঙ্ক_Matches,
	    matches);
}

/*
 * The 'completeopt' option is changed.
 */
    ৱর্ণ *
did_set_completeopt(optset_T *args UNUSED)
{
    char_u	*cot = p_cot;
    unsigned	*flags = &cot_flags;

    যদি (args->os_flags & OPT_LOCAL)
    {
	cot = curbuf->b_p_cot;
	flags = &curbuf->b_cot_flags;
    }
    অন্যথা যদি (!(args->os_flags & OPT_GLOBAL))
	// When using :set, clear the local flags.
	curbuf->b_cot_flags = 0;

    যদি (opt_strings_flags(cot, p_cot_মানসমূহ, flags, TRUE) != OK)
	ঘূৰাই e_invalid_argument;

    ঘূৰাই রিক্ত;
}

    পূর্ণাঙ্ক
expand_set_completeopt(optexpand_T *args, পূর্ণাঙ্ক *অঙ্ক_Matches, char_u ***matches)
{
    ঘূৰাই expand_set_opt_string(
	    args,
	    p_cot_মানসমূহ,
	    ARRAY_LENGTH(p_cot_মানসমূহ) - 1,
	    অঙ্ক_Matches,
	    matches);
}

/*
 * The 'completefuzzycollect' option is changed.
 */
    ৱর্ণ *
did_set_completefuzzycollect(optset_T *args UNUSED)
{
    যদি (opt_strings_flags(p_cfc, p_cfc_মানসমূহ, &cfc_flags, TRUE) != OK)
	ঘূৰাই e_invalid_argument;
    ঘূৰাই রিক্ত;
}

    পূর্ণাঙ্ক
expand_set_completefuzzycollect(
	optexpand_T *args,
	পূর্ণাঙ্ক *অঙ্ক_Matches,
	char_u ***matches)
{
    ঘূৰাই expand_set_opt_string(
	    args,
	    p_cfc_মানসমূহ,
	    ARRAY_LENGTH(p_cfc_মানসমূহ) - 1,
	    অঙ্ক_Matches,
	    matches);
}

/*
 * The 'completeitemalign' option is changed.
 */
    ৱর্ণ *
did_set_completeitemalign(optset_T *args UNUSED)
{
    char_u	*p = p_cia;
    unsigned	new_cia_flags = 0;
    পূর্ণাঙ্ক		seen[3] = { FALSE, FALSE, FALSE };
    পূর্ণাঙ্ক		গুনতি = 0;
    char_u	buf[10];

    যেতিয়ালৈ (*p)
    {
	copy_option_part(&p, buf, পরিমাণ(buf), ",");
	যদি (গুনতি >= 3)
	    ঘূৰাই e_invalid_argument;

	যদি (STRCMP(buf, "abbr") == 0)
	{
	    যদি (seen[CPT_ABBR])
		ঘূৰাই e_invalid_argument;
	    new_cia_flags = new_cia_flags * 10 + CPT_ABBR;
	    seen[CPT_ABBR] = TRUE;
	    গুনতি++;
	}
	অন্যথা যদি (STRCMP(buf, "kind") == 0)
	{
	    যদি (seen[CPT_KIND])
		ঘূৰাই e_invalid_argument;
	    new_cia_flags = new_cia_flags * 10 + CPT_KIND;
	    seen[CPT_KIND] = TRUE;
	    গুনতি++;
	}
	অন্যথা যদি (STRCMP(buf, "menu") == 0)
	{
	    যদি (seen[CPT_MENU])
		ঘূৰাই e_invalid_argument;
	    new_cia_flags = new_cia_flags * 10 + CPT_MENU;
	    seen[CPT_MENU] = TRUE;
	    গুনতি++;
	}
	অন্যথা
	    ঘূৰাই e_invalid_argument;
    }
    যদি (new_cia_flags == 0 || গুনতি != 3)
	ঘূৰাই e_invalid_argument;

    cia_flags = new_cia_flags;
    ঘূৰাই রিক্ত;
}

#যদি defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
/*
 * The 'completepopup' option is changed.
 */
    ৱর্ণ *
did_set_completepopup(optset_T *args UNUSED)
{
    যদি (parse_completepopup(রিক্ত) == FAIL)
	ঘূৰাই e_invalid_argument;

    popup_close_info();
    ঘূৰাই রিক্ত;
}
#endif

#যদি defined(BACKSLASH_IN_FILENAME)
/*
 * The 'completeslash' option is changed.
 */
    ৱর্ণ *
did_set_completeslash(optset_T *args UNUSED)
{
    যদি (check_opt_strings(p_csl, p_csl_মানসমূহ, FALSE) != OK
	    || check_opt_strings(curbuf->b_p_csl, p_csl_মানসমূহ, FALSE) != OK)
	ঘূৰাই e_invalid_argument;

    ঘূৰাই রিক্ত;
}

    পূর্ণাঙ্ক
expand_set_completeslash(optexpand_T *args, পূর্ণাঙ্ক *অঙ্ক_Matches, char_u ***matches)
{
    ঘূৰাই expand_set_opt_string(
	    args,
	    p_csl_মানসমূহ,
	    ARRAY_LENGTH(p_csl_মানসমূহ) - 1,
	    অঙ্ক_Matches,
	    matches);
}
#endif

#যদি defined(FEAT_CONCEAL)
/*
 * The 'concealcursor' option is changed.
 */
    ৱর্ণ *
did_set_concealcursor(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    ঘূৰাই did_set_option_listflag(*varp, (char_u *)COCU_ALL, args->os_errbuf,
		    args->os_errbuflen);
}

    পূর্ণাঙ্ক
expand_set_concealcursor(optexpand_T *args, পূর্ণাঙ্ক *অঙ্ক_Matches, char_u ***matches)
{
    ঘূৰাই expand_set_opt_listflag(args, (char_u*)COCU_ALL, অঙ্ক_Matches, matches);
}
#endif

/*
 * The 'cpoptions' option is changed.
 */
    ৱর্ণ *
did_set_cpoptions(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    ঘূৰাই did_set_option_listflag(*varp, (char_u *)CPO_ALL, args->os_errbuf,
		    args->os_errbuflen);
}

    পূর্ণাঙ্ক
expand_set_cpoptions(optexpand_T *args, পূর্ণাঙ্ক *অঙ্ক_Matches, char_u ***matches)
{
    ঘূৰাই expand_set_opt_listflag(args, (char_u*)CPO_ALL, অঙ্ক_Matches, matches);
}

#যদি defined(FEAT_CRYPT)
/*
 * The 'cryptkey' option is changed.
 */
    ৱর্ণ *
did_set_cryptkey(optset_T *args)
{
    // Make sure the ":set" command doesn't show the new value in the
    // history.
    remove_চাবি_from_history();

    যদি (args->os_op != OP_NONE)
	// Don't allow set+=/-=/^= as they can allow for substring guessing
	ঘূৰাই e_invalid_argument;

    যদি (STRCMP(curbuf->b_p_চাবি, args->os_oldval.string) != 0)
    {
	// Need to update the swapfile.
	ml_set_crypt_চাবি(curbuf, args->os_oldval.string,
		*curbuf->b_p_cm == NUL ? p_cm : curbuf->b_p_cm);
	changed_internal();
    }
# ifdef FEAT_SODIUM
    যদি (crypt_method_is_sodium(crypt_get_method_nr(curbuf)))
	crypt_sodium_lock_চাবি(args->os_newval.string);
# endif

    ঘূৰাই রিক্ত;
}

/*
 * The 'cryptmethod' option is changed.
 */
    ৱর্ণ *
did_set_cryptmethod(optset_T *args)
{
    char_u  *p;
    char_u  *s;

    যদি (args->os_flags & OPT_LOCAL)
	p = curbuf->b_p_cm;
    অন্যথা
	p = p_cm;
    যদি (check_opt_strings(p, p_cm_মানসমূহ, TRUE) != OK)
	ঘূৰাই e_invalid_argument;
    অন্যথা যদি (crypt_self_test() == FAIL)
	ঘূৰাই e_invalid_argument;

    // When setting the global value to empty, make it "zip".
    যদি (*p_cm == NUL)
    {
	free_string_option(p_cm);
	p_cm = vim_strsave((char_u *)"zip");
    }
    // When using ":set cm=name" the local value is going to be empty.
    // Do that here, otherwise the crypt functions will still use the
    // local value.
    যদি ((args->os_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	free_string_option(curbuf->b_p_cm);
	curbuf->b_p_cm = empty_option;
    }

    // Need to update the swapfile when the effective method changed.
    // Set "s" to the effective old value, "p" to the effective new
    // method and compare.
    যদি ((args->os_flags & OPT_LOCAL) && *args->os_oldval.string == NUL)
	s = p_cm;  // was previously using the global value
    অন্যথা
	s = args->os_oldval.string;
    যদি (*curbuf->b_p_cm == NUL)
	p = p_cm;  // is now using the global value
    অন্যথা
	p = curbuf->b_p_cm;
    যদি (STRCMP(s, p) != 0)
	ml_set_crypt_চাবি(curbuf, curbuf->b_p_চাবি, s);

    // If the global value changes need to update the swapfile for all
    // buffers using that value.
    যদি ((args->os_flags & OPT_GLOBAL)
	    && STRCMP(p_cm, args->os_oldval.string) != 0)
    {
	buf_T	*buf;

	FOR_ALL_BUFFERS(buf)
	    যদি (buf != curbuf && *buf->b_p_cm == NUL)
		ml_set_crypt_চাবি(buf, buf->b_p_চাবি, args->os_oldval.string);
    }
    ঘূৰাই রিক্ত;
}

    পূর্ণাঙ্ক
expand_set_cryptmethod(optexpand_T *args, পূর্ণাঙ্ক *অঙ্ক_Matches, char_u ***matches)
{
    ঘূৰাই expand_set_opt_string(
	    args,
	    p_cm_মানসমূহ,
	    ARRAY_LENGTH(p_cm_মানসমূহ) - 1,
	    অঙ্ক_Matches,
	    matches);
}
#endif

#যদি defined(FEAT_CSCOPE) && defined(FEAT_QUICKFIX)
/*
 * The 'cscopequickfix' option is changed.
 */
    ৱর্ণ *
did_set_cscopequickfix(optset_T *args UNUSED)
{
    char_u	*p;

    যদি (p_csqf == রিক্ত)
	ঘূৰাই রিক্ত;

    p = p_csqf;
    যেতিয়ালৈ (*p != NUL)
    {
	যদি (vim_strchr((char_u *)CSQF_CMDS, *p) == রিক্ত
		|| p[1] == NUL
		|| vim_strchr((char_u *)CSQF_FLAGS, p[1]) == রিক্ত
		|| (p[2] != NUL && p[2] != ','))
	    ঘূৰাই e_invalid_argument;
	অন্যথা যদি (p[2] == NUL)
	    ভঙ্গ;
	অন্যথা
	    p += 3;
    }

    ঘূৰাই রিক্ত;
}
#endif

#যদি defined(FEAT_SYN_HL)
/*
 * The 'cursorlineopt' option is changed.
 */
    ৱর্ণ *
did_set_cursorlineopt(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    // This could be changed to use opt_strings_flags() instead.
    যদি (**varp == NUL || fill_culopt_flags(*varp, curwin) != OK)
	ঘূৰাই e_invalid_argument;

    ঘূৰাই রিক্ত;
}

    পূর্ণাঙ্ক
expand_set_cursorlineopt(optexpand_T *args, পূর্ণাঙ্ক *অঙ্ক_Matches, char_u ***matches)
{
    ঘূৰাই expand_set_opt_string(
	    args,
	    p_culopt_মানসমূহ,
	    ARRAY_LENGTH(p_culopt_মানসমূহ) - 1,
	    অঙ্ক_Matches,
	    matches);
}
#endif

/*
 * The 'debug' option is changed.
 */
    ৱর্ণ *
did_set_debug(optset_T *args UNUSED)
{
    ঘূৰাই did_set_opt_strings(p_debug, p_debug_মানসমূহ, TRUE);
}

    পূর্ণাঙ্ক
expand_set_debug(optexpand_T *args, পূর্ণাঙ্ক *অঙ্ক_Matches, char_u ***matches)
{
    ঘূৰাই expand_set_opt_string(
	    args,
	    p_debug_মানসমূহ,
	    ARRAY_LENGTH(p_debug_মানসমূহ) - 1,
	    অঙ্ক_Matches,
	    matches);
}

#যদি defined(FEAT_DIFF)
/*
 * The 'diffanchors' option is changed.
 */
    ৱর্ণ *
did_set_diffanchors(optset_T *args)
{
    যদি (diffanchors_changed(args->os_flags & OPT_LOCAL) == FAIL)
	ঘূৰাই e_invalid_argument;

    ঘূৰাই রিক্ত;
}

/*
 * The 'diffopt' option is changed.
 */
    ৱর্ণ *
did_set_diffopt(optset_T *args UNUSED)
{
    যদি (diffopt_changed() == FAIL)
	ঘূৰাই e_invalid_argument;

    ঘূৰাই রিক্ত;
}

    পূর্ণাঙ্ক
expand_set_diffopt(optexpand_T *args, পূর্ণাঙ্ক *অঙ্ক_Matches, char_u ***matches)
{
    expand_T *xp = args->oe_xp;

    যদি (xp->xp_pattern > args->oe_set_arg && *(xp->xp_pattern-1) == ':')
    {
	যদি (completing_মান_for_subopt(args, "algorithm"))
	{
	    ঘূৰাই expand_set_opt_string(
		    args,
		    p_dip_algorithm_মানসমূহ,
		    ARRAY_LENGTH(p_dip_algorithm_মানসমূহ) - 1,
		    অঙ্ক_Matches,
		    matches);
	}
	যদি (completing_মান_for_subopt(args, "inline"))
	{
	    ঘূৰাই expand_set_opt_string(
		    args,
		    p_dip_inline_মানসমূহ,
		    ARRAY_LENGTH(p_dip_inline_মানসমূহ) - 1,
		    অঙ্ক_Matches,
		    matches);
	}
	ঘূৰাই FAIL;
    }

    ঘূৰাই expand_set_opt_string(
	    args,
	    p_dip_মানসমূহ,
	    ARRAY_LENGTH(p_dip_মানসমূহ) - 1,
	    অঙ্ক_Matches,
	    matches);
}
#endif

/*
 * The 'display' option is changed.
 */
    ৱর্ণ *
did_set_display(optset_T *args UNUSED)
{
    যদি (opt_strings_flags(p_dy, p_dy_মানসমূহ, &dy_flags, TRUE) != OK)
	ঘূৰাই e_invalid_argument;

    (শূন্য)init_chartab();
    ঘূৰাই রিক্ত;
}

    পূর্ণাঙ্ক
expand_set_display(optexpand_T *args, পূর্ণাঙ্ক *অঙ্ক_Matches, char_u ***matches)
{
    ঘূৰাই expand_set_opt_string(
	    args,
	    p_dy_মানসমূহ,
	    ARRAY_LENGTH(p_dy_মানসমূহ) - 1,
	    অঙ্ক_Matches,
	    matches);
}

/*
 * The 'eadirection' option is changed.
 */
    ৱর্ণ *
did_set_eadirection(optset_T *args UNUSED)
{
    ঘূৰাই did_set_opt_strings(p_ead, p_ead_মানসমূহ, FALSE);
}

    পূর্ণাঙ্ক
expand_set_eadirection(optexpand_T *args, পূর্ণাঙ্ক *অঙ্ক_Matches, char_u ***matches)
{
    ঘূৰাই expand_set_opt_string(
	    args,
	    p_ead_মানসমূহ,
	    ARRAY_LENGTH(p_ead_মানসমূহ) - 1,
	    অঙ্ক_Matches,
	    matches);
}

/*
 * One of the 'encoding', 'fileencoding', 'termencoding' or 'makeencoding'
 * options is changed.
 */
    ৱর্ণ *
did_set_encoding(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u	**gvarp;
    ৱর্ণ	*errmsg = রিক্ত;
    char_u	*p;

    // Get the global option to compare with, otherwise we would have to check
    // two values for all local options.
    gvarp = (char_u **)get_option_varp_scope(args->os_idx, OPT_GLOBAL);

    যদি (gvarp == &p_fenc)
    {
	যদি (!curbuf->b_p_ma && args->os_flags != OPT_GLOBAL)
	    errmsg = e_cannot_make_changes_modifiable_is_off;
	অন্যথা যদি (vim_strchr(*varp, ',') != রিক্ত)
	    // No comma allowed in 'fileencoding'; catches confusing it
	    // with 'fileencodings'.
	    errmsg = e_invalid_argument;
	অন্যথা
	{
	    // May show a "+" in the title now.
	    redraw_titles();
	    // Add 'fileencoding' to the swap file.
	    ml_setflags(curbuf);
	}
    }
    যদি (errmsg == রিক্ত)
    {
	// canonize the value, so that STRCMP() can be used on it
	p = enc_canonize(*varp);
	যদি (p != রিক্ত)
	{
	    vim_free(*varp);
	    *varp = p;
	}
	যদি (varp == &p_enc)
	{
	    errmsg = mb_init();
	    redraw_titles();
	}
    }

#যদি defined(FEAT_GUI_GTK)
    যদি (errmsg == রিক্ত && varp == &p_tenc && gui.in_use)
    {
	// GTK uses only a single encoding, and that is UTF-8.
	যদি (STRCMP(p_tenc, "utf-8") != 0)
	    errmsg = e_cannot_be_changed_in_gtk_GUI;
    }
#endif

    যদি (errmsg == রিক্ত)
    {
#ifdef FEAT_KEYMAP
	// When 'keymap' is used and 'encoding' changes, reload the keymap
	// (with another encoding).
	যদি (varp == &p_enc && *curbuf->b_p_keymap != NUL)
	    (শূন্য)keymap_init();
#endif

	// When 'termencoding' is not empty and 'encoding' changes or when
	// 'termencoding' changes, need to setup for keyboard input and
	// display output conversion.
	যদি (((varp == &p_enc && *p_tenc != NUL) || varp == &p_tenc))
	{
	    যদি (convert_setup(&প্ৰবেশ_conv, p_tenc, p_enc) == FAIL
		    || convert_setup(&output_conv, p_enc, p_tenc) == FAIL)
	    {
		semsg(_(e_cannot_convert_between_str_and_str),
			p_tenc, p_enc);
		errmsg = e_invalid_argument;
	    }
	}

#যদি defined(MSWIN)
	// $HOME, $VIM and $VIMRUNTIME may have characters in active code page.
	যদি (varp == &p_enc)
	{
	    init_homedir();
	    init_vimdir();
	}
#endif
    }

    ঘূৰাই errmsg;
}

    পূর্ণাঙ্ক
expand_set_encoding(optexpand_T *args, পূর্ণাঙ্ক *অঙ্ক_Matches, char_u ***matches)
{
    ঘূৰাই expand_set_opt_generic(
	    args,
	    get_encoding_name,
	    অঙ্ক_Matches,
	    matches);
}

/*
 * The 'eventignore(win)' option is changed.
 */
    ৱর্ণ *
did_set_eventignore(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u	*oldval = args->os_oldval.string;

    যদি (check_ei(*varp) == FAIL)
	ঘূৰাই e_invalid_argument;

    যদি (oldval == রিক্ত || STRCMP(oldval, *varp) == 0)
	ঘূৰাই রিক্ত;

    // Deal with the events that are triggered by comparing against a stored
    // state, with the old value in effect: what happened while an event was
    // ignored must not be reported once it is not ignored anymore, and what
    // happened before must still be reported.
    // Use a copy, the caller owns "oldval" and autocommands may free it.
    char_u	*save_ei = vim_strsave(oldval);
    যদি (save_ei != রিক্ত)
    {
	char_u	*newval = *varp;
	win_T	*wp = is_window_local_option(args->os_idx) ? curwin : রিক্ত;

	*varp = save_ei;
	// "varp" points into "wp" for 'eventignorewin'.
	যদি (wp != রিক্ত)
	    ++wp->w_locked;
	may_trigger_deferred_events();
	যদি (wp != রিক্ত)
	    --wp->w_locked;

	free_string_option(*varp);
	*varp = newval;
    }

    ঘূৰাই রিক্ত;
}

স্থির পূর্ণাঙ্ক expand_eiw = FALSE;

    স্থির char_u *
get_eventignore_name(expand_T *xp, পূর্ণাঙ্ক idx)
{
    পূর্ণাঙ্ক subtract = *xp->xp_pattern == '-';
    // 'eventignore(win)' allows special keyword "all" in addition to
    // all event names.
    যদি (!subtract && idx == 0)
	ঘূৰাই (char_u *)"all";

    char_u *name = get_event_name_no_group(xp, idx - 1 + subtract, expand_eiw);
    যদি (name == রিক্ত)
	ঘূৰাই রিক্ত;

    sprintf((ৱর্ণ *)IObuff, "%s%s", subtract ? "-" : "", name);
    ঘূৰাই IObuff;
}

    পূর্ণাঙ্ক
expand_set_eventignore(optexpand_T *args, পূর্ণাঙ্ক *অঙ্ক_Matches, char_u ***matches)
{
    expand_eiw = args->oe_varp != (char_u *)&p_ei;
    ঘূৰাই expand_set_opt_generic(
	    args,
	    get_eventignore_name,
	    অঙ্ক_Matches,
	    matches);
}

/*
 * The 'fileformat' option is changed.
 */
    ৱর্ণ *
did_set_fileformat(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    যদি (!curbuf->b_p_ma && !(args->os_flags & OPT_GLOBAL))
	ঘূৰাই e_cannot_make_changes_modifiable_is_off;
    অন্যথা যদি (check_opt_strings(*varp, p_ff_মানসমূহ, FALSE) != OK)
	ঘূৰাই e_invalid_argument;

    // may also change 'textmode'
    যদি (get_fileformat(curbuf) == EOL_DOS)
	curbuf->b_p_tx = TRUE;
    অন্যথা
	curbuf->b_p_tx = FALSE;
    redraw_titles();
    // update flag in swap file
    ml_setflags(curbuf);
    // Redraw needed when switching to/from "mac": a CR in the text
    // will be displayed differently.
    যদি (get_fileformat(curbuf) == EOL_MAC || *args->os_oldval.string == 'm')
	redraw_curbuf_later(UPD_NOT_VALID);

    ঘূৰাই রিক্ত;
}

    পূর্ণাঙ্ক
expand_set_fileformat(optexpand_T *args, পূর্ণাঙ্ক *অঙ্ক_Matches, char_u ***matches)
{
    ঘূৰাই expand_set_opt_string(
	    args,
	    p_ff_মানসমূহ,
	    ARRAY_LENGTH(p_ff_মানসমূহ) - 1,
	    অঙ্ক_Matches,
	    matches);
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * fileformat options.
 */
    char_u *
get_fileformat_name(expand_T *xp UNUSED, পূর্ণাঙ্ক idx)
{
    যদি (idx >= (পূর্ণাঙ্ক)ARRAY_LENGTH(p_ff_মানসমূহ))
	ঘূৰাই রিক্ত;

    ঘূৰাই (char_u*)p_ff_মানসমূহ[idx];
}

/*
 * The 'fileformats' option is changed.
 */
    ৱর্ণ *
did_set_fileformats(optset_T *args UNUSED)
{
    যদি (check_opt_strings(p_ffs, p_ff_মানসমূহ, TRUE) != OK)
	ঘূৰাই e_invalid_argument;

    // also change 'textauto'
    যদি (*p_ffs == NUL)
	p_ta = FALSE;
    অন্যথা
	p_ta = TRUE;

    ঘূৰাই রিক্ত;
}

/*
 * The 'filetype' or the 'syntax' option is changed.
 */
    ৱর্ণ *
did_set_filetype_or_syntax(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    যদি (!valid_filetype(*varp))
	ঘূৰাই e_invalid_argument;

    args->os_মান_changed = STRCMP(args->os_oldval.string, *varp) != 0;

    // Since we check the value, there is no need to set P_INSECURE,
    // even when the value comes from a modeline.
    args->os_মান_checked = TRUE;

    ঘূৰাই রিক্ত;
}

#যদি defined(FEAT_FOLDING)
/*
 * The 'foldclose' option is changed.
 */
    ৱর্ণ *
did_set_foldclose(optset_T *args UNUSED)
{
    ঘূৰাই did_set_opt_strings(p_fcl, p_fcl_মানসমূহ, TRUE);
}

    পূর্ণাঙ্ক
expand_set_foldclose(optexpand_T *args, পূর্ণাঙ্ক *অঙ্ক_Matches, char_u ***matches)
{
    ঘূৰাই expand_set_opt_string(
	    args,
	    p_fcl_মানসমূহ,
	    ARRAY_LENGTH(p_fcl_মানসমূহ) - 1,
	    অঙ্ক_Matches,
	    matches);
}
#endif

#যদি defined(FEAT_EVAL) && defined(FEAT_FOLDING)
/*
 * The 'foldexpr' option is changed.
 */
    ৱর্ণ *
did_set_foldexpr(optset_T *args)
{
    (শূন্য)did_set_optexpr(args);
    যদি (foldmethod_Is_ৰাশিমালা(curwin))
	foldUpdateAll(curwin);
    ঘূৰাই রিক্ত;
}
#endif

#যদি defined(FEAT_FOLDING)
/*
 * The 'foldignore' option is changed.
 */
    ৱর্ণ *
did_set_foldignore(optset_T *args UNUSED)
{
    যদি (foldmethodIsIndent(curwin))
	foldUpdateAll(curwin);
    ঘূৰাই রিক্ত;
}

/*
 * The 'foldmarker' option is changed.
 */
    ৱর্ণ *
did_set_foldmarker(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u	*p;

    p = vim_strchr(*varp, ',');
    যদি (p == রিক্ত)
	ঘূৰাই e_comma_required;
    অন্যথা যদি (p == *varp || p[1] == NUL)
	ঘূৰাই e_invalid_argument;
    অন্যথা যদি (foldmethodIsMarker(curwin))
	foldUpdateAll(curwin);

    ঘূৰাই রিক্ত;
}

/*
 * The 'foldmethod' option is changed.
 */
    ৱর্ণ *
did_set_foldmethod(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    যদি (check_opt_strings(*varp, p_fdm_মানসমূহ, FALSE) != OK || **varp == NUL)
	ঘূৰাই e_invalid_argument;

    foldUpdateAll(curwin);
    যদি (foldmethodIsDiff(curwin))
	newFoldLevel();
    ঘূৰাই রিক্ত;
}

    পূর্ণাঙ্ক
expand_set_foldmethod(optexpand_T *args, পূর্ণাঙ্ক *অঙ্ক_Matches, char_u ***matches)
{
    ঘূৰাই expand_set_opt_string(
	    args,
	    p_fdm_মানসমূহ,
	    ARRAY_LENGTH(p_fdm_মানসমূহ) - 1,
	    অঙ্ক_Matches,
	    matches);
}

/*
 * The 'foldopen' option is changed.
 */
    ৱর্ণ *
did_set_foldopen(optset_T *args UNUSED)
{
    ঘূৰাই did_set_opt_flags(p_fdo, p_fdo_মানসমূহ, &fdo_flags, TRUE);
}

    পূর্ণাঙ্ক
expand_set_foldopen(optexpand_T *args, পূর্ণাঙ্ক *অঙ্ক_Matches, char_u ***matches)
{
    ঘূৰাই expand_set_opt_string(
	    args,
	    p_fdo_মানসমূহ,
	    ARRAY_LENGTH(p_fdo_মানসমূহ) - 1,
	    অঙ্ক_Matches,
	    matches);
}
#endif

/*
 * The 'formatoptions' option is changed.
 */
    ৱর্ণ *
did_set_formatoptions(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    ঘূৰাই did_set_option_listflag(*varp, (char_u *)FO_ALL, args->os_errbuf,
		    args->os_errbuflen);
}

    পূর্ণাঙ্ক
expand_set_formatoptions(optexpand_T *args, পূর্ণাঙ্ক *অঙ্ক_Matches, char_u ***matches)
{
    ঘূৰাই expand_set_opt_listflag(args, (char_u*)FO_ALL, অঙ্ক_Matches, matches);
}

#যদি defined(CURSOR_SHAPE)
/*
 * The 'guicursor' option is changed.
 */
    ৱর্ণ *
did_set_guicursor(optset_T *args UNUSED)
{
    ঘূৰাই parse_shape_opt(SHAPE_CURSOR);
}
#endif

#যদি defined(FEAT_GUI)
/*
 * The 'guifont' option is changed.
 */
    ৱর্ণ *
did_set_guifont(optset_T *args UNUSED)
{
    char_u	*p;
    ৱর্ণ	*errmsg = রিক্ত;

    যদি (gui.in_use)
    {
	p = p_guifont;
# যদি defined(FEAT_GUI_GTK)
	// Put up a font dialog and let the user select a new value.
	// If this is cancelled go back to the old value but don't
	// give an error message.
	যদি (STRCMP(p, "*") == 0)
	{
	    p = gui_mch_font_dialog(args->os_oldval.string);
	    free_string_option(p_guifont);
	    p_guifont = (p != রিক্ত) ? p : vim_strsave(args->os_oldval.string);
	}
# endif
	যদি (p != রিক্ত && gui_init_font(p_guifont, FALSE) != OK)
	{
# যদি defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_PHOTON)
	    যদি (STRCMP(p_guifont, "*") == 0)
	    {
		// Dialog was cancelled: Keep the old value without giving
		// an error message.
		free_string_option(p_guifont);
		p_guifont = vim_strsave(args->os_oldval.string);
	    }
	    অন্যথা
# endif
		errmsg = e_invalid_fonts;
	}
    }

    ঘূৰাই errmsg;
}

/*
 * Expand the 'guifont' option. Only when GUI is being used. Each platform has
 * specific behaviors.
 */
    পূর্ণাঙ্ক
expand_set_guifont(optexpand_T *args, পূর্ণাঙ্ক *অঙ্ক_Matches, char_u ***matches)
{
    যদি (!gui.in_use)
	ঘূৰাই FAIL;

# যদি defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
    char_u **varp = (char_u **)args->oe_varp;
    পূর্ণাঙ্ক wide = (varp == &p_guifontwide);

    ঘূৰাই expand_set_opt_callback(
	    args, gui_mch_expand_font, &wide, অঙ্ক_Matches, matches);
# অন্যথা
    ঘূৰাই FAIL;
# endif
}

# যদি defined(FEAT_XFONTSET)
/*
 * The 'guifontset' option is changed.
 */
    ৱর্ণ *
did_set_guifontset(optset_T *args UNUSED)
{
    ৱর্ণ *errmsg = রিক্ত;

    যদি (STRCMP(p_guifontset, "*") == 0)
	errmsg = e_cant_select_fontset;
    অন্যথা যদি (gui.in_use && gui_init_font(p_guifontset, TRUE) != OK)
	errmsg = e_invalid_fontset;

    ঘূৰাই errmsg;
}
# endif

/*
 * The 'guifontwide' option is changed.
 */
    ৱর্ণ *
did_set_guifontwide(optset_T *args UNUSED)
{
    ৱর্ণ *errmsg = রিক্ত;

    যদি (STRCMP(p_guifontwide, "*") == 0)
	errmsg = e_cant_select_wide_font;
    অন্যথা যদি (gui_get_wide_font() == FAIL)
	errmsg = e_invalid_wide_font;

    ঘূৰাই errmsg;
}
#endif

#যদি defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
/*
 * The 'guiligatures' option is changed.
 */
    ৱর্ণ *
did_set_guiligatures(optset_T *args UNUSED)
{
    gui_set_ligatures();
    ঘূৰাই রিক্ত;
}
#endif

#যদি defined(FEAT_GUI)
/*
 * The 'guioptions' option is changed.
 */
    ৱর্ণ *
did_set_guioptions(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    ৱর্ণ *errmsg;

    errmsg = did_set_option_listflag(*varp, (char_u *)GO_ALL, args->os_errbuf,
		    args->os_errbuflen);
    যদি (errmsg != রিক্ত)
	ঘূৰাই errmsg;

    gui_init_which_components(args->os_oldval.string);
    ঘূৰাই রিক্ত;
}

    পূর্ণাঙ্ক
expand_set_guioptions(optexpand_T *args, পূর্ণাঙ্ক *অঙ্ক_Matches, char_u ***matches)
{
    ঘূৰাই expand_set_opt_listflag(args, (char_u*)GO_ALL, অঙ্ক_Matches, matches);
}
#endif

#যদি defined(FEAT_GUI_TABLINE)
/*
 * The 'guitablabel' option is changed.
 */
    ৱর্ণ *
did_set_guitablabel(optset_T *args UNUSED)
{
    redraw_tabline = TRUE;
    ঘূৰাই রিক্ত;
}
#endif

/*
 * The 'helpfile' option is changed.
 */
    ৱর্ণ *
did_set_helpfile(optset_T *args UNUSED)
{
    // May compute new values for $VIM and $VIMRUNTIME
    যদি (didset_vim)
	vim_unsetenv_ext((char_u *)"VIM");
    যদি (didset_vimruntime)
	vim_unsetenv_ext((char_u *)"VIMRUNTIME");
    ঘূৰাই রিক্ত;
}

#যদি defined(FEAT_MULTI_LANG)
/*
 * The 'helplang' option is changed.
 */
    ৱর্ণ *
did_set_helplang(optset_T *args UNUSED)
{
    ৱর্ণ *errmsg = রিক্ত;

    // Check for "", "ab", "ab,cd", etc.
    বাবে (char_u *s = p_hlg; *s != NUL; s += 3)
    {
	যদি (s[1] == NUL || ((s[2] != ',' || s[3] == NUL) && s[2] != NUL))
	{
	    errmsg = e_invalid_argument;
	    ভঙ্গ;
	}
	যদি (s[2] == NUL)
	    ভঙ্গ;
    }

    ঘূৰাই errmsg;
}
#endif

/*
 * The 'highlight' option is changed.
 */
    ৱর্ণ *
did_set_highlight(optset_T *args UNUSED)
{
    যদি (highlight_changed() == FAIL)
	ঘূৰাই e_invalid_argument;	// invalid flags

    ঘূৰাই রিক্ত;
}

    স্থির পূর্ণাঙ্ক
expand_hl_occasions(
	optexpand_T *args,
	পূর্ণাঙ্ক *অঙ্ক_Matches,
	char_u ***matches,
	char_u prefix)
{
    char_u	    *p;
    স্থির char_u   hl_flags[HLF_গুনতি] = HL_FLAGS;
    size_t	    i;
    পূর্ণাঙ্ক		    গুনতি = 0;

    *matches = ALLOC_MULT(char_u *, HLF_গুনতি + 1);
    যদি (*matches == রিক্ত)
	ঘূৰাই FAIL;

    // We still want to return the full option if it's requested.
    যদি (args->oe_include_orig_val)
    {
	p = vim_strsave(args->oe_opt_মান);
	যদি (p == রিক্ত)
	{
	    VIM_CLEAR(*matches);
	    ঘূৰাই FAIL;
	}
	(*matches)[গুনতি++] = p;
    }

    বাবে (i = 0; i < HLF_গুনতি; i++)
    {
	p = alloc((prefix == NUL ? 1 : 2) + 1);
	যদি (p == রিক্ত)
	{
	    যদি (গুনতি == 0)
	    {
		VIM_CLEAR(*matches);
		ঘূৰাই FAIL;
	    }
	    অন্যথা
		ভঙ্গ;
	}
	যদি (prefix == NUL)
	    sprintf((ৱর্ণ *)p, "%c", hl_flags[i]);
	অন্যথা
	    sprintf((ৱর্ণ *)p, "%c%c", prefix, hl_flags[i]);
	(*matches)[গুনতি++] = p;
    }

    যদি (গুনতি == 0)
    {
	VIM_CLEAR(*matches);
	ঘূৰাই FAIL;
    }
    *অঙ্ক_Matches = গুনতি;
    ঘূৰাই OK;
}

/*
 * Expand 'highlight' option.
 */
    পূর্ণাঙ্ক
expand_set_highlight(optexpand_T *args, পূর্ণাঙ্ক *অঙ্ক_Matches, char_u ***matches)
{
    char_u	    *p;
    expand_T	    *xp = args->oe_xp;
    size_t	    i;
    পূর্ণাঙ্ক		    গুনতি = 0;

    যদি (xp->xp_pattern > args->oe_set_arg && *(xp->xp_pattern-1) == ':')
    {
	// Right after a ':', meaning we just return all highlight names.
	ঘূৰাই expand_set_opt_generic(
		args,
		get_highlight_name,
		অঙ্ক_Matches,
		matches);
    }

    যদি (*xp->xp_pattern == NUL)
	// At beginning of a comma-separated list. Return the specific list of
	// supported occasions.
	ঘূৰাই expand_hl_occasions(args, অঙ্ক_Matches, matches, NUL);

    // We are after the initial character (which indicates the occasion). We
    // already made sure we are not matching after a ':' above, so now we want
    // to match against display mode modifiers.
    // Since the xp_pattern starts from the beginning, we need to include it in
    // the returned match.

    // Note: Keep this in sync with highlight_changed()
    স্থির char_u p_hl_mode_মানসমূহ[] =
	{':', 'b', 'i', '-', 'n', 'r', 's', 'u', 'c', '2', 'd', '=', 't'};
    size_t অঙ্ক_hl_modes = ARRAY_LENGTH(p_hl_mode_মানসমূহ);

    *matches = ALLOC_MULT(char_u *, অঙ্ক_hl_modes);
    যদি (*matches == রিক্ত)
	ঘূৰাই FAIL;

    পূর্ণাঙ্ক pattern_len = xp->xp_pattern_len;

    বাবে (i = 0; i < অঙ্ক_hl_modes; i++)
    {
	// Don't allow duplicates as these are unique flags
	char_u *dup = vim_strchr(xp->xp_pattern + 1, p_hl_mode_মানসমূহ[i]);
	যদি (dup != রিক্ত && (পূর্ণাঙ্ক)(dup - xp->xp_pattern) < pattern_len)
	    অনুৱর্ত;

	// ':' only works by itself, not with other flags.
	যদি (pattern_len > 1 && p_hl_mode_মানসমূহ[i] == ':')
	    অনুৱর্ত;

	p = vim_strnsave(xp->xp_pattern, pattern_len + 1);
	যদি (p == রিক্ত)
	{
	    যদি (i == 0)
	    {
		VIM_CLEAR(*matches);
		ঘূৰাই FAIL;
	    }
	    অন্যথা
		ভঙ্গ;
	}
	p[pattern_len] = p_hl_mode_মানসমূহ[i];
	p[pattern_len + 1] = NUL;
	(*matches)[গুনতি++] = p;
    }
    যদি (গুনতি == 0)
    {
	VIM_CLEAR(*matches);
	ঘূৰাই FAIL;
    }
    *অঙ্ক_Matches = গুনতি;

    ঘূৰাই OK;
}

/*
 * The 'titlestring' or the 'iconstring' option is changed.
 */
    স্থির ৱর্ণ *
parse_titleiconstring(optset_T *args UNUSED, পূর্ণাঙ্ক flagval UNUSED)
{
#ifdef FEAT_STL_OPT
    char_u	**varp = (char_u **)args->os_varp;

    // NULL => statusline syntax
    যদি (vim_strchr(*varp, '%') && check_stl_option(*varp) == রিক্ত)
	stl_syntax |= flagval;
    অন্যথা
	stl_syntax &= ~flagval;
#endif
    did_set_title();

    ঘূৰাই রিক্ত;
}

/*
 * The 'iconstring' option is changed.
 */
    ৱর্ণ *
did_set_iconstring(optset_T *args)
{
    পূর্ণাঙ্ক flagval = 0;

#ifdef FEAT_STL_OPT
    flagval = STL_IN_ICON;
#endif

    ঘূৰাই parse_titleiconstring(args, flagval);
}

#যদি defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
/*
 * The 'imactivatekey' option is changed.
 */
    ৱর্ণ *
did_set_imactivatekey(optset_T *args UNUSED)
{
    যদি (!im_xim_isvalid_imactivate())
	ঘূৰাই e_invalid_argument;
    ঘূৰাই রিক্ত;
}
#endif

#ifdef FEAT_IMAGE
/*
 * The 'imageprotocol' option is changed.
 */
    ৱর্ণ *
did_set_imageprotocol(optset_T *args UNUSED)
{
    যদি (update_image_backend() == FAIL)
	ঘূৰাই e_invalid_argument;
    ঘূৰাই রিক্ত;
}

    পূর্ণাঙ্ক
expand_set_imageprotocol(optexpand_T *args, পূর্ণাঙ্ক *অঙ্ক_Matches, char_u ***matches)
{
    expand_T *xp = args->oe_xp;

    যদি (xp->xp_pattern > args->oe_set_arg && *(xp->xp_pattern-1) == ':')
    {
	// 'imageprotocol' only has well-defined terms for completion for the
	// protocol part after the colon.
	ঘূৰাই expand_set_opt_string(
		args,
		p_ipc_protocol_মানসমূহ,
		ARRAY_LENGTH(p_ipc_protocol_মানসমূহ) - 1,
		অঙ্ক_Matches,
		matches);
    }
    // Use expand_set_opt_string instead of returning FAIL so that we can
    // include the original value if args->oe_include_orig_val is set.
    স্থির ৱর্ণ *(empty[]) = {রিক্ত};
    ঘূৰাই expand_set_opt_string(args, empty, 0, অঙ্ক_Matches, matches);
}
#endif

/*
 * The 'iskeyword' option is changed.
 */
    ৱর্ণ *
did_set_iskeyword(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    যদি (varp == &p_isk)		// only check for global-value
    {
	যদি (check_isopt(*varp) == FAIL)
	    ঘূৰাই e_invalid_argument;
    }
    অন্যথা			// fallthrough for local-value
	ঘূৰাই did_set_isopt(args);

    ঘূৰাই রিক্ত;
}

/*
 * The 'isident' or the 'iskeyword' or the 'isprint' or the 'isfname' option is
 * changed.
 */
    ৱর্ণ *
did_set_isopt(optset_T *args)
{
    // 'isident', 'iskeyword', 'isprint' or 'isfname' option: refill g_chartab[]
    // If the new option is invalid, use old value.
    // 'lisp' option: refill g_chartab[] for '-' char.
    যদি (init_chartab() == FAIL)
    {
	args->os_restore_chartab = TRUE;// need to restore the chartab.
	ঘূৰাই e_invalid_argument;	// error in value
    }

    ঘূৰাই রিক্ত;
}

/*
 * The 'jumpoptions' option is changed.
 */
    ৱর্ণ *
did_set_jumpoptions(optset_T *args UNUSED)
{
    যদি (opt_strings_flags(p_jop, p_jop_মানসমূহ, &jop_flags, TRUE) != OK)
	ঘূৰাই e_invalid_argument;

    ঘূৰাই রিক্ত;
}

    পূর্ণাঙ্ক
expand_set_jumpoptions(optexpand_T *args, পূর্ণাঙ্ক *অঙ্ক_Matches, char_u ***matches)
{
    ঘূৰাই expand_set_opt_string(
	    args,
	    p_jop_মানসমূহ,
	    ARRAY_LENGTH(p_jop_মানসমূহ) - 1,
	    অঙ্ক_Matches,
	    matches);
}

#যদি defined(FEAT_KEYMAP)
/*
 * The 'keymap' option is changed.
 */
    ৱর্ণ *
did_set_keymap(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    ৱর্ণ *errmsg = রিক্ত;

    যদি (!valid_filetype(*varp))
	errmsg = e_invalid_argument;
    অন্যথা
    {
	পূর্ণাঙ্ক	    secure_save = secure;

	// Reset the secure flag, since the value of 'keymap' has
	// been checked to be safe.
	secure = 0;

	// load or unload key mapping tables
	errmsg = keymap_init();

	secure = secure_save;

	// Since we check the value, there is no need to set P_INSECURE,
	// even when the value comes from a modeline.
	args->os_মান_checked = TRUE;
    }

    যদি (errmsg == রিক্ত)
    {
	যদি (*curbuf->b_p_keymap != NUL)
	{
	    // Installed a new keymap, switch on using it.
	    curbuf->b_p_iminsert = B_IMODE_LMAP;
	    যদি (curbuf->b_p_imsearch != B_IMODE_USE_ঢোকাও)
		curbuf->b_p_imsearch = B_IMODE_LMAP;
	}
	অন্যথা
	{
	    // Cleared the keymap, may reset 'iminsert' and 'imsearch'.
	    যদি (curbuf->b_p_iminsert == B_IMODE_LMAP)
		curbuf->b_p_iminsert = B_IMODE_NONE;
	    যদি (curbuf->b_p_imsearch == B_IMODE_LMAP)
		curbuf->b_p_imsearch = B_IMODE_USE_ঢোকাও;
	}
	যদি ((args->os_flags & OPT_LOCAL) == 0)
	{
	    set_iminsert_global();
	    set_imsearch_global();
	}
	status_redraw_curbuf();
    }

    ঘূৰাই errmsg;
}
#endif

/*
 * The 'keymodel' option is changed.
 */
    ৱর্ণ *
did_set_keymodel(optset_T *args UNUSED)
{
    যদি (check_opt_strings(p_km, p_km_মানসমূহ, TRUE) != OK)
	ঘূৰাই e_invalid_argument;

    km_stopsel = (vim_strchr(p_km, 'o') != রিক্ত);
    km_startsel = (vim_strchr(p_km, 'a') != রিক্ত);
    ঘূৰাই রিক্ত;
}

    পূর্ণাঙ্ক
expand_set_keymodel(optexpand_T *args, পূর্ণাঙ্ক *অঙ্ক_Matches, char_u ***matches)
{
    ঘূৰাই expand_set_opt_string(
	    args,
	    p_km_মানসমূহ,
	    ARRAY_LENGTH(p_km_মানসমূহ) - 1,
	    অঙ্ক_Matches,
	    matches);
}

/*
 * The 'keyprotocol' option is changed.
 */
    ৱর্ণ *
did_set_keyprotocol(optset_T *args UNUSED)
{
    char_u *term = T_NAME;
    keyprot_T kpc = match_keyprotocol(term);
    যদি (kpc == KEYPROTOCOL_FAIL)
	ঘূৰাই e_invalid_argument;

    apply_keyprotocol(term, kpc);

    ঘূৰাই রিক্ত;
}

    পূর্ণাঙ্ক
expand_set_keyprotocol(optexpand_T *args, পূর্ণাঙ্ক *অঙ্ক_Matches, char_u ***matches)
{
    expand_T *xp = args->oe_xp;
    যদি (xp->xp_pattern > args->oe_set_arg && *(xp->xp_pattern-1) == ':')
    {
	// 'keyprotocol' only has well-defined terms for completion for the
	// protocol part after the colon.
	ঘূৰাই expand_set_opt_string(
		args,
		p_kpc_protocol_মানসমূহ,
		ARRAY_LENGTH(p_kpc_protocol_মানসমূহ) - 1,
		অঙ্ক_Matches,
		matches);
    }
    // Use expand_set_opt_string instead of returning FAIL so that we can
    // include the original value if args->oe_include_orig_val is set.
    স্থির ৱর্ণ *(empty[]) = {রিক্ত};
    ঘূৰাই expand_set_opt_string(args, empty, 0, অঙ্ক_Matches, matches);
}

/*
 * The 'lispoptions' option is changed.
 */
    ৱর্ণ *
did_set_lispoptions(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    যদি (**varp != NUL
	    && STRCMP(*varp, "expr:0") != 0
	    && STRCMP(*varp, "expr:1") != 0)
	ঘূৰাই e_invalid_argument;

    ঘূৰাই রিক্ত;
}

    পূর্ণাঙ্ক
expand_set_lispoptions(optexpand_T *args, পূর্ণাঙ্ক *অঙ্ক_Matches, char_u ***matches)
{
    স্থির ৱর্ণ *(p_lop_মানসমূহ[]) = {"expr:0", "expr:1", রিক্ত};
    ঘূৰাই expand_set_opt_string(
	    args,
	    p_lop_মানসমূহ,
	    ARRAY_LENGTH(p_lop_মানসমূহ) - 1,
	    অঙ্ক_Matches,
	    matches);
}

/*
 * The 'matchpairs' option is changed.
 */
    ৱর্ণ *
did_set_matchpairs(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u	*p;

    যদি (has_mbyte)
    {
	বাবে (p = *varp; *p != NUL; ++p)
	{
	    পূর্ণাঙ্ক x2 = -1;
	    পূর্ণাঙ্ক x3 = -1;

	    p += mb_ptr2len(p);
	    যদি (*p != NUL)
		x2 = *p++;
	    যদি (*p != NUL)
	    {
		x3 = mb_ptr2char(p);
		p += mb_ptr2len(p);
	    }
	    যদি (x2 != ':' || x3 == -1 || (*p != NUL && *p != ','))
		ঘূৰাই e_invalid_argument;
	    যদি (*p == NUL)
		ভঙ্গ;
	}
    }
    অন্যথা
    {
	// Check for "x:y,x:y"
	বাবে (p = *varp; *p != NUL; p += 4)
	{
	    যদি (p[1] != ':' || p[2] == NUL || (p[3] != NUL && p[3] != ','))
		ঘূৰাই e_invalid_argument;
	    যদি (p[3] == NUL)
		ভঙ্গ;
	}
    }

    ঘূৰাই রিক্ত;
}

/*
 * Process the updated 'messagesopt' option value.
 */
    ৱর্ণ *
did_set_messagesopt(optset_T *args UNUSED)
{
    যদি (messagesopt_changed() == FAIL)
	ঘূৰাই e_invalid_argument;

    ঘূৰাই রিক্ত;
}

    পূর্ণাঙ্ক
expand_set_messagesopt(optexpand_T *args, পূর্ণাঙ্ক *অঙ্ক_Matches, char_u ***matches)
{
    স্থির ৱর্ণ *(p_mopt_মানসমূহ[]) = {"hit-enter", "wait:", "history:", রিক্ত};
    ঘূৰাই expand_set_opt_string(
	    args,
	    p_mopt_মানসমূহ,
	    ARRAY_LENGTH(p_mopt_মানসমূহ) - 1,
	    অঙ্ক_Matches,
	    matches);
}

#যদি defined(FEAT_SPELL)
/*
 * The 'mkspellmem' option is changed.
 */
    ৱর্ণ *
did_set_mkspellmem(optset_T *args UNUSED)
{
    যদি (spell_check_msm() != OK)
	ঘূৰাই e_invalid_argument;

    ঘূৰাই রিক্ত;
}
#endif

/*
 * The 'mouse' option is changed.
 */
    ৱর্ণ *
did_set_mouse(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    ৱর্ণ	*retval;

    retval = did_set_option_listflag(*varp, (char_u *)MOUSE_ALL, args->os_errbuf,
		    args->os_errbuflen);
    যদি (retval == রিক্ত)
    {
	redraw_tabline = TRUE;
	যদি (tabline_height() > 0)
	    update_screen(UPD_VALID);
#যদি defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
	popup_close_info(); // Close info popup to apply new properties
#endif
    }
    ঘূৰাই retval;
}

    পূর্ণাঙ্ক
expand_set_mouse(optexpand_T *args, পূর্ণাঙ্ক *অঙ্ক_Matches, char_u ***matches)
{
    ঘূৰাই expand_set_opt_listflag(args, (char_u*)MOUSE_ALL, অঙ্ক_Matches, matches);
}

/*
 * The 'mousemodel' option is changed.
 */
    ৱর্ণ *
did_set_mousemodel(optset_T *args UNUSED)
{
    যদি (check_opt_strings(p_mousem, p_mousem_মানসমূহ, FALSE) != OK)
	ঘূৰাই e_invalid_argument;
#যদি defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU) && (XmVersion <= 1002)
    অন্যথা যদি (*p_mousem != *args->os_oldval.string)
	// Changed from "extend" to "popup" or "popup_setpos" or vv: need
	// to create or delete the popup menus.
	gui_motif_update_mousemodel(মূল_menu);
#endif

    ঘূৰাই রিক্ত;
}

    পূর্ণাঙ্ক
expand_set_mousemodel(optexpand_T *args, পূর্ণাঙ্ক *অঙ্ক_Matches, char_u ***matches)
{
    ঘূৰাই expand_set_opt_string(
	    args,
	    p_mousem_মানসমূহ,
	    ARRAY_LENGTH(p_mousem_মানসমূহ) - 1,
	    অঙ্ক_Matches,
	    matches);
}

#যদি defined(FEAT_MOUSESHAPE)
    ৱর্ণ *
did_set_mouseshape(optset_T *args UNUSED)
{
    ৱর্ণ *errmsg = রিক্ত;

    errmsg = parse_shape_opt(SHAPE_MOUSE);
    update_mouseshape(-1);

    ঘূৰাই errmsg;
}
#endif

/*
 * The 'nrformats' option is changed.
 */
    ৱর্ণ *
did_set_nrformats(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    ঘূৰাই did_set_opt_strings(*varp, p_nf_মানসমূহ, TRUE);
}

    পূর্ণাঙ্ক
expand_set_nrformats(optexpand_T *args, পূর্ণাঙ্ক *অঙ্ক_Matches, char_u ***matches)
{
    ঘূৰাই expand_set_opt_string(
	    args,
	    p_nf_মানসমূহ,
	    ARRAY_LENGTH(p_nf_মানসমূহ) - 1,
	    অঙ্ক_Matches,
	    matches);
}

#যদি defined(FEAT_EVAL)
/*
 * One of the '*expr' options is changed: 'balloonexpr', 'diffexpr',
 * 'foldexpr', 'foldtext', 'formatexpr', 'includeexpr', 'indentexpr',
 * 'patchexpr', 'printexpr' and 'charconvert'.
 */
    ৱর্ণ *
did_set_optexpr(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    // If the option value starts with <SID> or s:, then replace that with
    // the script identifier.
    char_u *name = get_scriptlocal_funcname(*varp);
    যদি (name != রিক্ত)
    {
	free_string_option(*varp);
	*varp = name;
    }

    ঘূৰাই রিক্ত;
}
#endif

/*
 * The 'pastetoggle' option is changed.
 */
    ৱর্ণ *
did_set_pastetoggle(optset_T *args UNUSED)
{
    char_u	*p;

    // translate key codes like in a mapping
    যদি (*p_pt)
    {
	(শূন্য)replace_termcodes(p_pt, &p, 0,
		REPTERM_FROM_PART | REPTERM_DO_LT, রিক্ত);
	যদি (p != রিক্ত)
	{
	    free_string_option(p_pt);
	    p_pt = p;
	}
    }

    ঘূৰাই রিক্ত;
}

#যদি defined(FEAT_PROP_POPUP)
/*
 * The 'previewpopup' option is changed.
 */
    ৱর্ণ *
did_set_previewpopup(optset_T *args UNUSED)
{
    যদি (parse_previewpopup(রিক্ত) == FAIL)
	ঘূৰাই e_invalid_argument;

# যদি defined(FEAT_QUICKFIX)
    popup_close_info();
# endif
    ঘূৰাই রিক্ত;
}

    স্থির পূর্ণাঙ্ক
expand_set_popupoption(optexpand_T *args, পূর্ণাঙ্ক *অঙ্ক_Matches, char_u ***matches,
	পূর্ণাঙ্ক previewpopup)
{
    expand_T *xp = args->oe_xp;

    যদি (xp->xp_pattern > args->oe_set_arg && *(xp->xp_pattern-1) == ':')
    {
	যদি (completing_মান_for_subopt(args, "close")
		|| completing_মান_for_subopt(args, "resize")
		|| completing_মান_for_subopt(args, "shadow"))
	{
	    ঘূৰাই expand_set_opt_string(
		    args,
		    p_popup_option_on_off_মানসমূহ,
		    ARRAY_LENGTH(p_popup_option_on_off_মানসমূহ) - 1,
		    অঙ্ক_Matches,
		    matches);
	}
	যদি (completing_মান_for_subopt(args, "border"))
	{
	    ঘূৰাই expand_set_opt_string(
		    args,
		    previewpopup ? p_popup_pvp_border_মানসমূহ
			: p_popup_cpp_border_মানসমূহ,
		    (previewpopup ? ARRAY_LENGTH(p_popup_pvp_border_মানসমূহ) - 1
			: ARRAY_LENGTH(p_popup_cpp_border_মানসমূহ)) - 1,
		    অঙ্ক_Matches,
		    matches);
	}
	যদি (completing_মান_for_subopt(args, "align"))
	{
	    ঘূৰাই expand_set_opt_string(
		    args,
		    p_popup_option_align_মানসমূহ,
		    ARRAY_LENGTH(p_popup_option_align_মানসমূহ) - 1,
		    অঙ্ক_Matches,
		    matches);
	}
	যদি (completing_মান_for_subopt(args, "highlight")
		|| completing_মান_for_subopt(args, "borderhighlight"))
	{
	    // Return the list of all highlight names
	    ঘূৰাই expand_set_opt_generic(
		    args,
		    get_highlight_name,
		    অঙ্ক_Matches,
		    matches);
	}
	ঘূৰাই FAIL;
    }

    ঘূৰাই expand_set_opt_string(
	    args,
	    previewpopup ? p_popup_pvp_option_মানসমূহ
		: p_popup_cpp_option_মানসমূহ,
	    previewpopup ? ARRAY_LENGTH(p_popup_pvp_option_মানসমূহ) - 1
		: ARRAY_LENGTH(p_popup_cpp_option_মানসমূহ) - 1,
	    অঙ্ক_Matches,
	    matches);
}

    পূর্ণাঙ্ক
expand_set_previewpopup(optexpand_T *args, পূর্ণাঙ্ক *অঙ্ক_Matches, char_u ***matches)
{
    ঘূৰাই expand_set_popupoption(args, অঙ্ক_Matches, matches, TRUE);
}

    পূর্ণাঙ্ক
expand_set_completepopup(optexpand_T *args, পূর্ণাঙ্ক *অঙ্ক_Matches, char_u ***matches)
{
    ঘূৰাই expand_set_popupoption(args, অঙ্ক_Matches, matches, FALSE);
}
#endif

#যদি defined(FEAT_POSTSCRIPT)
/*
 * The 'printencoding' option is changed.
 */
    ৱর্ণ *
did_set_printencoding(optset_T *args UNUSED)
{
    char_u	*s, *p;

    // Canonize 'printencoding' if VIM standard one
    p = enc_canonize(p_penc);
    যদি (p != রিক্ত)
    {
	vim_free(p_penc);
	p_penc = p;
    }
    অন্যথা
    {
	// Ensure lower case and '-' for '_'
	বাবে (s = p_penc; *s != NUL; s++)
	{
	    যদি (*s == '_')
		*s = '-';
	    অন্যথা
		*s = TOLOWER_ASC(*s);
	}
    }

    ঘূৰাই রিক্ত;
}
#endif

#যদি defined(FEAT_PRINTER)

    স্থির char_u *
get_printoptions_names(expand_T *xp UNUSED, পূর্ণাঙ্ক idx)
{
    যদি (idx >= (পূর্ণাঙ্ক)ARRAY_LENGTH(printer_opts))
	ঘূৰাই রিক্ত;
    ঘূৰাই (char_u*)printer_opts[idx].name;
}

/*
 * Expand 'printoptions' option
 */
    পূর্ণাঙ্ক
expand_set_printoptions(optexpand_T *args, পূর্ণাঙ্ক *অঙ্ক_Matches, char_u ***matches)
{
    ঘূৰাই expand_set_opt_generic(
	    args,
	    get_printoptions_names,
	    অঙ্ক_Matches,
	    matches);
}
#endif

#যদি defined(FEAT_STL_OPT)
/*
 * The 'statusline' or the 'tabline' or the 'rulerformat' option is changed.
 * "rulerformat" is TRUE if the 'rulerformat' option is changed.
 */
    স্থির ৱর্ণ *
parse_statustabline_rulerformat(optset_T *args, পূর্ণাঙ্ক rulerformat)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u	*s;
    ৱর্ণ	*errmsg = রিক্ত;
    পূর্ণাঙ্ক		wid;

    যদি (rulerformat)	// reset ru_wid first
	ru_wid = 0;
    s = *varp;
    যদি (rulerformat && *s == '%')
    {
	// set ru_wid if 'ruf' starts with "%99("
	যদি (*++s == '-')	// ignore a '-'
	    s++;
	wid = getdigits(&s);
	যদি (wid && *s == '(' && (errmsg = check_stl_option(p_ruf)) == রিক্ত)
	    ru_wid = wid;
	অন্যথা
	{
	    // Validate the flags in 'rulerformat' only if it doesn't point to
	    // a custom function ("%!" flag).
	    যদি ((*varp)[1] != '!')
		errmsg = check_stl_option(p_ruf);
	}
    }
    // check 'statusline' or 'tabline' only if it doesn't start with "%!"
    অন্যথা যদি (rulerformat || s[0] != '%' || s[1] != '!')
	errmsg = check_stl_option(s);
    যদি (rulerformat && errmsg == রিক্ত)
	comp_col();

    ঘূৰাই errmsg;
}
#endif

#যদি defined(FEAT_RENDER_OPTIONS)
/*
 * The 'renderoptions' option is changed.
 */
    ৱর্ণ *
did_set_renderoptions(optset_T *args UNUSED)
{
    যদি (!gui_mch_set_rendering_options(p_rop))
	ঘূৰাই e_invalid_argument;

    ঘূৰাই রিক্ত;
}
#endif

#যদি defined(FEAT_RIGHTLEFT)
/*
 * The 'rightleftcmd' option is changed.
 */
    ৱর্ণ *
did_set_rightleftcmd(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    // Currently only "search" is a supported value.
    যদি (**varp != NUL && STRCMP(*varp, "search") != 0)
	ঘূৰাই e_invalid_argument;

    ঘূৰাই রিক্ত;
}

    পূর্ণাঙ্ক
expand_set_rightleftcmd(optexpand_T *args, পূর্ণাঙ্ক *অঙ্ক_Matches, char_u ***matches)
{
    স্থির ৱর্ণ *(p_rlc_মানসমূহ[]) = {"search", রিক্ত};
    ঘূৰাই expand_set_opt_string(
	    args,
	    p_rlc_মানসমূহ,
	    ARRAY_LENGTH(p_rlc_মানসমূহ) - 1,
	    অঙ্ক_Matches,
	    matches);
}
#endif

#define PUM_BORDER_CLEAR()   \
    কৰা {                     \
	pum_set_border(FALSE); \
	pum_set_shadow(FALSE); \
	pum_set_margin(FALSE); \
    } যেতিয়ালৈ (0)

/*
 * Parse a border value from a pumopt "border:" token.
 * Returns OK on success, FAIL on error.
 */
    স্থির পূর্ণাঙ্ক
parse_pumopt_border(char_u *val, পূর্ণাঙ্ক len)
{
    // Use box-drawing characters only when 'encoding' is "utf-8" and
    // 'ambiwidth' is "single".
    পূর্ণাঙ্ক	    can_use_box_chars = (enc_utf8 && *p_ambw == 's');
    char_u  *token;

    token = vim_strnsave(val, len);
    যদি (token == রিক্ত)
	ঘূৰাই FAIL;

    যদি (can_use_box_chars && STRCMP(token, "single") == 0)
	pum_set_border_chars(0x2500, 0x2502, 0x2500, 0x2502, // ─ │ ─ │
		0x250c, 0x2510, 0x2518, 0x2514); // ┌ ┐ ┘ └
    অন্যথা যদি (can_use_box_chars && STRCMP(token, "double") == 0)
	pum_set_border_chars(0x2550, 0x2551, 0x2550, 0x2551, // ═ ║ ═ ║
		0x2554, 0x2557, 0x255D, 0x255A); // ╔ ╗ ╝  ╚
    অন্যথা যদি (can_use_box_chars && STRCMP(token, "round") == 0)
	pum_set_border_chars(0x2500, 0x2502, 0x2500, 0x2502, // ─ │ ─ │
		0x256d, 0x256e, 0x256f, 0x2570); // ╭ ╮ ╯ ╰
    অন্যথা যদি (STRCMP(token, "ascii") == 0)
	pum_set_border_chars('-', '|', '-', '|', '+', '+', '+', '+');
    অন্যথা যদি (STRNCMP(token, "custom:", 7) == 0)
    {
	char_u	*q = token + 7;
	পূর্ণাঙ্ক	out[8];

	বাবে (পূর্ণাঙ্ক i = 0; i < 8; i++)
	{
	    যদি (*q == NUL || *q == ',')
	    {
		vim_free(token);
		ঘূৰাই FAIL;
	    }
	    out[i] = mb_ptr2char(q);
	    mb_ptr2char_adv(&q);
	    যদি (i < 7)
	    {
		যদি (*q != ';')
		{
		    vim_free(token);
		    ঘূৰাই FAIL;
		}
		q++;
	    }
	}
	যদি (*q != NUL && *q != ',')
	{
	    vim_free(token);
	    ঘূৰাই FAIL;
	}
	pum_set_border_chars(out[0], out[1], out[2], out[3], out[4], out[5],
		out[6], out[7]);
    }
    অন্যথা
    {
	vim_free(token);
	ঘূৰাই FAIL;
    }

    vim_free(token);
    pum_set_border(TRUE);
    ঘূৰাই OK;
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
    ৱর্ণ *
did_set_pumopt(optset_T *args)
{
    char_u  **varp = (char_u **)args->os_varp;
    char_u  *p;
    পূর্ণাঙ্ক	    have_border = FALSE;
    পূর্ণাঙ্ক	    have_margin = FALSE;

    // Reset to defaults.
    PUM_BORDER_CLEAR();
    p_ph = 0;
    p_pw = 15;
    p_pmw = 0;
    p_po = 100;

    যদি (*varp == রিক্ত || **varp == NUL)
	ঘূৰাই রিক্ত;

    বাবে (p = *varp; p != রিক্ত && *p != NUL; )
    {
	char_u *comma = vim_strchr(p, ',');
	পূর্ণাঙ্ক len;

	যদি (comma != রিক্ত)
	    len = (পূর্ণাঙ্ক)(comma - p);
	অন্যথা
	    len = (পূর্ণাঙ্ক)STRLEN(p);

	যদি (STRNCMP(p, "border:", 7) == 0)
	{
	    যদি (have_border)
		গচ্ছ error;
	    have_border = TRUE;
	    যদি (parse_pumopt_border(p + 7, len - 7) == FAIL)
		গচ্ছ error;
	}
	অন্যথা যদি (STRNCMP(p, "height:", 7) == 0)
	{
	    long n = atol((ৱর্ণ *)p + 7);
	    যদি (n < 0)
		গচ্ছ error;
	    p_ph = n;
	}
	অন্যথা যদি (STRNCMP(p, "width:", 6) == 0)
	{
	    long n = atol((ৱর্ণ *)p + 6);
	    যদি (n < 0)
		গচ্ছ error;
	    p_pw = n;
	}
	অন্যথা যদি (STRNCMP(p, "maxwidth:", 9) == 0)
	{
	    long n = atol((ৱর্ণ *)p + 9);
	    যদি (n < 0)
		গচ্ছ error;
	    p_pmw = n;
	}
	অন্যথা যদি (STRNCMP(p, "opacity:", 8) == 0)
	{
	    long n = atol((ৱর্ণ *)p + 8);
	    যদি (n < 0 || n > 100)
		গচ্ছ error;
	    p_po = n;
	}
	অন্যথা যদি (len == 6 && STRNCMP(p, "shadow", 6) == 0)
	    pum_set_shadow(TRUE);
	অন্যথা যদি (len == 6 && STRNCMP(p, "margin", 6) == 0)
	{
	    have_margin = TRUE;
	    pum_set_margin(TRUE);
	}
	অন্যথা
	    গচ্ছ error;

	যদি (comma != রিক্ত)
	    p = comma + 1;
	অন্যথা
	    ভঙ্গ;
    }

    যদি (have_margin && !have_border)
	গচ্ছ error;

    // Invalidate cached background for opacity changes.
    pum_opacity_changed();

    ঘূৰাই রিক্ত;

error:
    PUM_BORDER_CLEAR();
    p_ph = 0;
    p_pw = 15;
    p_pmw = 0;
    p_po = 100;
    ঘূৰাই e_invalid_argument;
}

    স্থির char_u *
get_pum_border_style(expand_T *xp UNUSED, পূর্ণাঙ্ক idx)
{
    স্থির ৱর্ণ *styles[] = {"ascii", "custom:", "single", "double", "round"};
    ঘূৰাই idx < ((enc_utf8 && *p_ambw == 's') ? (পূর্ণাঙ্ক)ARRAY_LENGTH(styles) : 2)
	    ? (char_u *)styles[idx] : রিক্ত;
}

    পূর্ণাঙ্ক
expand_set_pumopt(optexpand_T *args, পূর্ণাঙ্ক *অঙ্ক_Matches, char_u ***matches)
{
    expand_T *xp = args->oe_xp;

    যদি (xp->xp_pattern > args->oe_set_arg && *(xp->xp_pattern-1) == ':')
    {
	যদি (completing_মান_for_subopt(args, "border"))
	{
	    ঘূৰাই expand_set_opt_generic(
		    args, get_pum_border_style, অঙ্ক_Matches, matches);
	}
	ঘূৰাই FAIL;
    }

    স্থির ৱর্ণ *(p_pumopt_মানসমূহ[]) = {"border:", "height:", "width:",
	"maxwidth:", "opacity:", "shadow", "margin", রিক্ত};
    ঘূৰাই expand_set_opt_string(
	    args,
	    p_pumopt_মানসমূহ,
	    ARRAY_LENGTH(p_pumopt_মানসমূহ) - 1,
	    অঙ্ক_Matches,
	    matches);
}

/*
 * The 'pumborder' option is changed.
 * Rules:
 *   - One of { single, double, round, ascii, custom:XXXXXXXX } may appear.
 *   - "margin" may appear, but only together with exactly one border style.
 *   - "shadow" is independent and can be combined freely.
 */
    ৱর্ণ *
did_set_pumborder(optset_T *args)
{
    char_u  **varp = (char_u **)args->os_varp;
    char_u  *p;
    পূর্ণাঙ্ক	    len;
    পূর্ণাঙ্ক	    have_border = FALSE;
    পূর্ণাঙ্ক	    have_margin = FALSE;

    PUM_BORDER_CLEAR();

    যদি (*varp == রিক্ত || **varp == NUL)
	ঘূৰাই রিক্ত;

    বাবে (p = *varp; p != রিক্ত && *p != NUL; )
    {
	char_u *comma = vim_strchr(p, ',');
	যদি (comma != রিক্ত)
	    len = (পূর্ণাঙ্ক)(comma - p);
	অন্যথা
	    len = (পূর্ণাঙ্ক)STRLEN(p);

	যদি (STRNCMP(p, "shadow", len) == 0 && len == 6)
	    pum_set_shadow(TRUE);
	অন্যথা যদি (STRNCMP(p, "margin", len) == 0 && len == 6)
	{
	    have_margin = TRUE;
	    pum_set_margin(TRUE);
	}
	অন্যথা
	{
	    যদি (have_border)
		গচ্ছ error;
	    have_border = TRUE;
	    যদি (parse_pumopt_border(p, len) == FAIL)
		গচ্ছ error;
	}

	যদি (comma != রিক্ত)
	    p = comma + 1;
	অন্যথা
	    ভঙ্গ;
    }

    যদি (have_margin && !have_border)
	গচ্ছ error;

    ঘূৰাই রিক্ত;

error:
    PUM_BORDER_CLEAR();
    pum_set_border_chars(0, 0, 0, 0, 0, 0, 0, 0);
    ঘূৰাই e_invalid_argument;
}

    স্থির char_u *
get_pumborder_token(expand_T *xp, পূর্ণাঙ্ক idx)
{
    ঘূৰাই idx == 0 ? (char_u *)"margin"
	 : idx == 1 ? (char_u *)"shadow"
	 : get_pum_border_style(xp, idx - 2);
}

    পূর্ণাঙ্ক
expand_set_pumborder(optexpand_T *args, পূর্ণাঙ্ক *অঙ্ক_Matches, char_u ***matches)
{
    ঘূৰাই expand_set_opt_generic(
	    args, get_pumborder_token, অঙ্ক_Matches, matches);
}

#যদি defined(FEAT_STL_OPT)

/*
 * The 'rulerformat' option is changed.
 */
    ৱর্ণ *
did_set_rulerformat(optset_T *args)
{
    ঘূৰাই parse_statustabline_rulerformat(args, TRUE);
}
#endif

#যদি defined(FEAT_TABPANEL)
/*
 * Process the new 'tabpanelopt' option value.
 */
    ৱর্ণ *
did_set_tabpanelopt(optset_T *args UNUSED)
{
    যদি (tabpanelopt_changed() == FAIL)
	ঘূৰাই e_invalid_argument;

    ঘূৰাই রিক্ত;
}

    পূর্ণাঙ্ক
expand_set_tabpanelopt(optexpand_T *args, পূর্ণাঙ্ক *অঙ্ক_Matches, char_u ***matches)
{
    expand_T *xp = args->oe_xp;

    যদি (xp->xp_pattern > args->oe_set_arg && *(xp->xp_pattern-1) == ':')
    {
	যদি (completing_মান_for_subopt(args, "align"))
	{
	    ঘূৰাই expand_set_opt_string(
		    args,
		    p_tplo_align_মানসমূহ,
		    ARRAY_LENGTH(p_tplo_align_মানসমূহ) - 1,
		    অঙ্ক_Matches,
		    matches);
	}
	ঘূৰাই FAIL;
    }

    ঘূৰাই expand_set_opt_string(
	    args,
	    p_tplo_মানসমূহ,
	    ARRAY_LENGTH(p_tplo_মানসমূহ) - 1,
	    অঙ্ক_Matches,
	    matches);
}
#endif

/*
 * The 'scrollopt' option is changed.
 */
    ৱর্ণ *
did_set_scrollopt(optset_T *args UNUSED)
{
    ঘূৰাই did_set_opt_strings(p_sbo, p_scbopt_মানসমূহ, TRUE);
}

    পূর্ণাঙ্ক
expand_set_scrollopt(optexpand_T *args, পূর্ণাঙ্ক *অঙ্ক_Matches, char_u ***matches)
{
    ঘূৰাই expand_set_opt_string(
	    args,
	    p_scbopt_মানসমূহ,
	    ARRAY_LENGTH(p_scbopt_মানসমূহ) - 1,
	    অঙ্ক_Matches,
	    matches);
}

/*
 * The 'wlseat' option is changed
 */
    ৱর্ণ *
did_set_wlseat(optset_T *args UNUSED)
{
#ifdef FEAT_WAYLAND_CLIPBOARD
    // If there isn't any seat named 'wlseat', then let the Wayland clipboard be
    // unavailable. Ignore errors returned.
    clip_reset_wayland();
#endif

    ঘূৰাই রিক্ত;
}

/*
 * The 'selection' option is changed.
 */
    ৱর্ণ *
did_set_selection(optset_T *args UNUSED)
{
    যদি (*p_sel == NUL || check_opt_strings(p_sel, p_sel_মানসমূহ, FALSE) != OK)
	ঘূৰাই e_invalid_argument;

    ঘূৰাই রিক্ত;
}

    পূর্ণাঙ্ক
expand_set_selection(optexpand_T *args, পূর্ণাঙ্ক *অঙ্ক_Matches, char_u ***matches)
{
    ঘূৰাই expand_set_opt_string(
	    args,
	    p_sel_মানসমূহ,
	    ARRAY_LENGTH(p_sel_মানসমূহ) - 1,
	    অঙ্ক_Matches,
	    matches);
}

/*
 * The 'selectmode' option is changed.
 */
    ৱর্ণ *
did_set_selectmode(optset_T *args UNUSED)
{
    ঘূৰাই did_set_opt_strings(p_slm, p_slm_মানসমূহ, TRUE);
}

    পূর্ণাঙ্ক
expand_set_selectmode(optexpand_T *args, পূর্ণাঙ্ক *অঙ্ক_Matches, char_u ***matches)
{
    ঘূৰাই expand_set_opt_string(
	    args,
	    p_slm_মানসমূহ,
	    ARRAY_LENGTH(p_slm_মানসমূহ) - 1,
	    অঙ্ক_Matches,
	    matches);
}

#যদি defined(FEAT_SESSION)
/*
 * The 'sessionoptions' option is changed.
 */
    ৱর্ণ *
did_set_sessionoptions(optset_T *args)
{
    যদি (opt_strings_flags(p_ssop, p_ssop_মানসমূহ, &ssop_flags, TRUE) != OK)
	ঘূৰাই e_invalid_argument;
    যদি ((ssop_flags & SSOP_CURDIR) && (ssop_flags & SSOP_SESDIR))
    {
	// Don't allow both "sesdir" and "curdir".
	(শূন্য)opt_strings_flags(args->os_oldval.string, p_ssop_মানসমূহ,
							&ssop_flags, TRUE);
	ঘূৰাই e_invalid_argument;
    }

    ঘূৰাই রিক্ত;
}

    পূর্ণাঙ্ক
expand_set_sessionoptions(
    optexpand_T		*args,
    পূর্ণাঙ্ক			*অঙ্ক_Matches,
    char_u		***matches)
{
    ঘূৰাই expand_set_opt_string(
	    args,
	    p_ssop_মানসমূহ,
	    ARRAY_LENGTH(p_ssop_মানসমূহ) - 1,
	    অঙ্ক_Matches,
	    matches);
}
#endif

/*
 * Validate 'shellpipe'/'shellredir' option.
 */
    ৱর্ণ *
did_set_shellpipe_redir(optset_T *args)
{
    char_u	*p;
    bool	seen = false;

    বাবে (p = args->os_newval.string; *p != NUL; ++p)
    {
	যদি (*p != '%')
	    অনুৱর্ত;

	যদি (p[1] == NUL)
	    ঘূৰাই e_invalid_format_string_single_percent_s;

	যদি (p[1] == '%')
	{
	    ++p;    // skip second %
	    অনুৱর্ত;
	}

	যদি (p[1] == 's')
	{
	    যদি (seen)
		ঘূৰাই e_invalid_format_string_single_percent_s;

	    seen = true;
	    ++p;    // consume 's'
	    অনুৱর্ত;
	}
	ঘূৰাই e_invalid_format_string_single_percent_s;
    }
    ঘূৰাই রিক্ত;
}

/*
 * The 'shortmess' option is changed.
 */
    ৱর্ণ *
did_set_shortmess(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    ঘূৰাই did_set_option_listflag(*varp, (char_u *)SHM_ALL, args->os_errbuf,
		    args->os_errbuflen);
}

    পূর্ণাঙ্ক
expand_set_shortmess(optexpand_T *args, পূর্ণাঙ্ক *অঙ্ক_Matches, char_u ***matches)
{
    ঘূৰাই expand_set_opt_listflag(args, (char_u*)SHM_ALL, অঙ্ক_Matches, matches);
}

#যদি defined(FEAT_LINEBREAK)
/*
 * The 'showbreak' option is changed.
 */
    ৱর্ণ *
did_set_showbreak(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u	*s;

    বাবে (s = *varp; *s; )
    {
	যদি (ptr2cells(s) != 1)
	    ঘূৰাই e_showbreak_contains_unprintable_or_wide_character;
	MB_PTR_ADV(s);
    }

    ঘূৰাই রিক্ত;
}
#endif

/*
 * The 'showcmdloc' option is changed.
 */
    ৱর্ণ *
did_set_showcmdloc(optset_T *args UNUSED)
{
    ৱর্ণ	*errmsg = did_set_opt_strings(p_sloc, p_sloc_মানসমূহ, FALSE);

    যদি (errmsg == রিক্ত)
	comp_col();

    ঘূৰাই errmsg;
}

    পূর্ণাঙ্ক
expand_set_showcmdloc(optexpand_T *args, পূর্ণাঙ্ক *অঙ্ক_Matches, char_u ***matches)
{
    ঘূৰাই expand_set_opt_string(
	    args,
	    p_sloc_মানসমূহ,
	    ARRAY_LENGTH(p_sloc_মানসমূহ) - 1,
	    অঙ্ক_Matches,
	    matches);
}

#যদি defined(FEAT_SIGNS)
/*
 * The 'signcolumn' option is changed.
 */
    ৱর্ণ *
did_set_signcolumn(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

# যদি defined(FEAT_LINEBREAK)
    যদি (check_opt_strings(*varp, p_scl_মানসমূহ, FALSE) != OK)
	ঘূৰাই e_invalid_argument;
    // When changing the 'signcolumn' to or from 'number', recompute the
    // width of the number column if 'number' or 'relativenumber' is set.
    যদি (((*args->os_oldval.string == 'n' && args->os_oldval.string[1] == 'u')
		|| (*curwin->w_p_scl == 'n' && *(curwin->w_p_scl + 1) =='u'))
	    && (curwin->w_p_nu || curwin->w_p_rnu))
	curwin->w_nrwidth_সাৰি_গুনতি = 0;
# endif

# যদি defined(FEAT_GUI)
    // In the GUI, when sign icons are used, a full screen clear is needed
    // to properly redraw the sign icons.
    যদি (gui.in_use && curbuf->b_signlist != রিক্ত)
	redraw_all_later(UPD_CLEAR);
# endif

    ঘূৰাই রিক্ত;
}

    পূর্ণাঙ্ক
expand_set_signcolumn(optexpand_T *args, পূর্ণাঙ্ক *অঙ্ক_Matches, char_u ***matches)
{
    ঘূৰাই expand_set_opt_string(
	    args,
	    p_scl_মানসমূহ,
	    ARRAY_LENGTH(p_scl_মানসমূহ) - 1,
	    অঙ্ক_Matches,
	    matches);
}
#endif

#যদি defined(FEAT_SPELL)
/*
 * The 'spellcapcheck' option is changed.
 */
    ৱর্ণ *
did_set_spellcapcheck(optset_T *args UNUSED)
{
    // compile the regexp program.
    ঘূৰাই compile_cap_prog(curwin->w_s);
}

/*
 * The 'spellfile' option is changed.
 */
    ৱর্ণ *
did_set_spellfile(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    যদি (!valid_spellfile(*varp))
	ঘূৰাই e_invalid_argument;

    // If there is a window for this buffer in which 'spell' is set load the
    // wordlists.
    ঘূৰাই did_set_spell_option();
}

/*
 * The 'spell' option is changed.
 */
    ৱর্ণ *
did_set_spelllang(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    যদি (!valid_spelllang(*varp))
	ঘূৰাই e_invalid_argument;

    // If there is a window for this buffer in which 'spell' is set load the
    // wordlists.
    ঘূৰাই did_set_spell_option();
}

/*
 * The 'spelloptions' option is changed.
 */
    ৱর্ণ *
did_set_spelloptions(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    যদি (**varp != NUL && STRCMP(*varp, "camel") != 0)
	ঘূৰাই e_invalid_argument;

    ঘূৰাই রিক্ত;
}

    পূর্ণাঙ্ক
expand_set_spelloptions(optexpand_T *args, পূর্ণাঙ্ক *অঙ্ক_Matches, char_u ***matches)
{
    স্থির ৱর্ণ *(p_spo_মানসমূহ[]) = {"camel", রিক্ত};
    ঘূৰাই expand_set_opt_string(
	    args,
	    p_spo_মানসমূহ,
	    ARRAY_LENGTH(p_spo_মানসমূহ) - 1,
	    অঙ্ক_Matches,
	    matches);
}

/*
 * The 'spellsuggest' option is changed.
 */
    ৱর্ণ *
did_set_spellsuggest(optset_T *args UNUSED)
{
    যদি (spell_check_sps() != OK)
	ঘূৰাই e_invalid_argument;

    ঘূৰাই রিক্ত;
}

    পূর্ণাঙ্ক
expand_set_spellsuggest(optexpand_T *args, পূর্ণাঙ্ক *অঙ্ক_Matches, char_u ***matches)
{
    ঘূৰাই expand_set_opt_string(
	    args,
	    p_sps_মানসমূহ,
	    ARRAY_LENGTH(p_sps_মানসমূহ) - 1,
	    অঙ্ক_Matches,
	    matches);
}
#endif

/*
 * The 'splitkeep' option is changed.
 */
    ৱর্ণ *
did_set_splitkeep(optset_T *args UNUSED)
{
    win_T	*wp;
    tabpage_T	*tp;
    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	wp->w_prev_height = wp->w_height;
	wp->w_prev_winrow = wp->w_winrow;
    }
    ঘূৰাই did_set_opt_strings(p_spk, p_spk_মানসমূহ, FALSE);
}

    পূর্ণাঙ্ক
expand_set_splitkeep(optexpand_T *args, পূর্ণাঙ্ক *অঙ্ক_Matches, char_u ***matches)
{
    ঘূৰাই expand_set_opt_string(
	    args,
	    p_spk_মানসমূহ,
	    ARRAY_LENGTH(p_spk_মানসমূহ) - 1,
	    অঙ্ক_Matches,
	    matches);
}

#যদি defined(FEAT_STL_OPT)
/*
 * The 'statusline' option is changed.
 */
    ৱর্ণ *
did_set_statusline(optset_T *args)
{
    char_u  **varp = (char_u **)args->os_varp;
    ৱর্ণ    *ret = parse_statustabline_rulerformat(args, FALSE);

    যদি (ret != রিক্ত)
	ঘূৰাই ret;
    update_stl_rendered_height(varp == &curwin->w_p_stl ? curwin : রিক্ত);
    frame_change_statusline_height();

    ঘূৰাই রিক্ত;
}

/*
 * Rewrite the 'statuslineopt' string: replace the last "maxheight:N" with
 * actual_stlh and remove earlier duplicates.  Keyword order is preserved.
 */
    স্থির শূন্য
update_stlo_maxheight(char_u **varp, পূর্ণাঙ্ক actual_stlh)
{
    ধ্রুৱ char_u    *src = *varp;
    ধ্রুৱ char_u    *last_mh = রিক্ত;
    ধ্রুৱ char_u    *p;

    // Find the last "maxheight:" token.
    p = src;
    যেতিয়ালৈ (*p != NUL)
    {
	যদি (STRNCMP(p, "maxheight:", 10) == 0)
	    last_mh = p;
	যেতিয়ালৈ (*p != ',' && *p != NUL)
	    ++p;
	যদি (*p == ',')
	    ++p;
    }

    যদি (last_mh == রিক্ত)
	ঘূৰাই;

    size_t	bufsize = STRLEN(src) + NUMBUFLEN + 1;
    char_u	*buf = alloc(bufsize);
    যদি (buf == রিক্ত)
	ঘূৰাই;

    পূর্ণাঙ্ক		len = 0;
    bool	need_comma = false;

    p = src;
    যেতিয়ালৈ (*p != NUL)
    {
	ধ্রুৱ	char_u	*tok = p;
	পূর্ণাঙ্ক	tok_len;

	যেতিয়ালৈ (*p != ',' && *p != NUL)
	    ++p;
	tok_len = (পূর্ণাঙ্ক)(p - tok);
	যদি (*p == ',')
	    ++p;

	যদি (STRNCMP(tok, "maxheight:", 10) == 0)
	{
	    যদি (tok == last_mh)
	    {
		// Replace the last occurrence with the actual value.
		যদি (need_comma)
		    buf[len++] = ',';
		len += vim_snprintf((ৱর্ণ *)buf + len,
			bufsize - len, "maxheight:%d", actual_stlh);
		need_comma = true;
	    }
	}
	অন্যথা
	{
	    যদি (need_comma)
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
    ৱর্ণ *
did_set_statuslineopt(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    win_T       *wp = varp == &curwin->w_p_stlo ? curwin : রিক্ত;

    যদি (statuslineopt_changed(*varp, wp) == FAIL)
	ঘূৰাই e_invalid_argument;

    // Sync stl_rendered_height with the current statusline.
    update_stl_rendered_height(wp);

    // Update the maxheight value to the actual value set.
    // Note: Must be changed if p_stlo_values are changed.
    যদি (*varp != empty_option)
    {
	পূর্ণাঙ্ক actual_stlh;

	যদি (wp != রিক্ত)
	{
	    frame_change_statusline_height();
	    actual_stlh = MIN(wp->w_p_stlo_mh,
		    wp->w_height + wp->w_status_height - p_wmh);
	}
	অন্যথা
	    actual_stlh = frame_change_statusline_height();

	// Only re-serialize when the window actually has a status line shown.
	যদি (actual_stlh > 0)
	{
	    update_stlo_maxheight(varp, actual_stlh);
	    // Update the parsed maxheight member directly.
	    যদি (wp != রিক্ত)
		wp->w_p_stlo_mh = actual_stlh;
	    অন্যথা
		set_stlo_mh(actual_stlh);
	}
    }
    অন্যথা
	frame_change_statusline_height();

    যদি ((args->os_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	// :set clears the local values.
	clear_string_option(&curwin->w_p_stlo);
	curwin->w_p_stlo_fh = -1;
	curwin->w_p_stlo_mh = -1;
    }

    ঘূৰাই রিক্ত;
}

    পূর্ণাঙ্ক
expand_set_statuslineopt(optexpand_T *args, পূর্ণাঙ্ক *অঙ্ক_Matches, char_u ***matches)
{
    ঘূৰাই expand_set_opt_string(
	    args,
	    p_stlo_মানসমূহ,
	    ARRAY_LENGTH(p_stlo_মানসমূহ) - 1,
	    অঙ্ক_Matches,
	    matches);
}
#endif

/*
 * The 'swapsync' option is changed.
 */
    ৱর্ণ *
did_set_swapsync(optset_T *args UNUSED)
{
    ঘূৰাই did_set_opt_strings(p_sws, p_sws_মানসমূহ, FALSE);
}

    পূর্ণাঙ্ক
expand_set_swapsync(optexpand_T *args, পূর্ণাঙ্ক *অঙ্ক_Matches, char_u ***matches)
{
    ঘূৰাই expand_set_opt_string(
	    args,
	    p_sws_মানসমূহ,
	    ARRAY_LENGTH(p_sws_মানসমূহ) - 1,
	    অঙ্ক_Matches,
	    matches);
}

/*
 * The 'switchbuf' option is changed.
 */
    ৱর্ণ *
did_set_switchbuf(optset_T *args UNUSED)
{
    ঘূৰাই did_set_opt_flags(p_swb, p_swb_মানসমূহ, &swb_flags, TRUE);
}

    পূর্ণাঙ্ক
expand_set_switchbuf(optexpand_T *args, পূর্ণাঙ্ক *অঙ্ক_Matches, char_u ***matches)
{
    ঘূৰাই expand_set_opt_string(
	    args,
	    p_swb_মানসমূহ,
	    ARRAY_LENGTH(p_swb_মানসমূহ) - 1,
	    অঙ্ক_Matches,
	    matches);
}

/*
 * The 'tabclose' option is changed.
 */
    ৱর্ণ *
did_set_tabclose(optset_T *args UNUSED)
{
    ঘূৰাই did_set_opt_flags(p_tcl, p_tcl_মানসমূহ, &tcl_flags, TRUE);
}

    পূর্ণাঙ্ক
expand_set_tabclose(optexpand_T *args, পূর্ণাঙ্ক *অঙ্ক_Matches, char_u ***matches)
{
    ঘূৰাই expand_set_opt_string(
	    args,
	    p_tcl_মানসমূহ,
	    ARRAY_LENGTH(p_tcl_মানসমূহ) - 1,
	    অঙ্ক_Matches,
	    matches);
}

#যদি defined(FEAT_STL_OPT)
/*
 * The 'tabline' option is changed.
 */
    ৱর্ণ *
did_set_tabline(optset_T *args)
{
    ঘূৰাই parse_statustabline_rulerformat(args, FALSE);
}
#endif

/*
 * The 'tagcase' option is changed.
 */
    ৱর্ণ *
did_set_tagcase(optset_T *args)
{
    unsigned পূর্ণাঙ্ক	*flags;
    char_u		*p;

    যদি (args->os_flags & OPT_LOCAL)
    {
	p = curbuf->b_p_tc;
	flags = &curbuf->b_tc_flags;
    }
    অন্যথা
    {
	p = p_tc;
	flags = &tc_flags;
    }

    যদি ((args->os_flags & OPT_LOCAL) && *p == NUL)
	// make the local value empty: use the global value
	*flags = 0;
    অন্যথা যদি (*p == NUL
	    || opt_strings_flags(p, p_tc_মানসমূহ, flags, FALSE) != OK)
	ঘূৰাই e_invalid_argument;

    ঘূৰাই রিক্ত;
}

    পূর্ণাঙ্ক
expand_set_tagcase(optexpand_T *args, পূর্ণাঙ্ক *অঙ্ক_Matches, char_u ***matches)
{
    ঘূৰাই expand_set_opt_string(
	    args,
	    p_tc_মানসমূহ,
	    ARRAY_LENGTH(p_tc_মানসমূহ) - 1,
	    অঙ্ক_Matches,
	    matches);
}

/*
 * The 'term' option is changed.
 */
    ৱর্ণ *
did_set_term(optset_T *args UNUSED)
{
    যদি (T_NAME[0] == NUL)
	ঘূৰাই e_cannot_set_term_to_empty_string;
#ifdef FEAT_GUI
    যদি (gui.in_use)
	ঘূৰাই e_cannot_change_term_in_GUI;
    যদি (term_is_gui(T_NAME))
	ঘূৰাই e_use_gui_to_start_GUI;
#endif
    যদি (set_termname(T_NAME) == FAIL)
	ঘূৰাই e_not_found_in_termcap;

    // Screen colors may have changed.
    redraw_later_clear();

    ঘূৰাই রিক্ত;
}

/*
 * Some terminal option (t_xxx) is changed
 */
    ৱর্ণ *
did_set_term_option(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    যদি (!full_screen)
	ঘূৰাই রিক্ত;

    // ":set t_Co=0" and ":set t_Co=1" do ":set t_Co="
    যদি (varp == &T_CCO)
    {
	পূর্ণাঙ্ক colors = atoi((ৱর্ণ *)T_CCO);

	// Only reinitialize colors if t_Co value has really changed to
	// avoid expensive reload of colorscheme if t_Co is set to the
	// same value multiple times.
	যদি (colors != t_colors)
	{
	    t_colors = colors;
	    যদি (t_colors <= 1)
	    {
		vim_free(T_CCO);
		T_CCO = empty_option;
	    }
#যদি defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
	    যদি (is_term_win32())
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
    যদি (varp == &T_ME)
    {
	out_str(T_ME);
	redraw_later(UPD_CLEAR);
#যদি defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
	// Since t_me has been set, this probably means that the user
	// wants to use this as default colors.  Need to reset default
	// background/foreground colors.
# ifdef VIMDLL
	যদি (!gui.in_use && !gui.starting)
# endif
	    mch_set_normal_colors();
#endif
    }
    যদি (varp == &T_BE && termcap_active)
    {
	MAY_WANT_TO_LOG_THIS;

	যদি (*T_BE == NUL)
	    // When clearing t_BE we assume the user no longer wants
	    // bracketed paste, thus disable it by writing t_BD.
	    out_str(T_BD);
	অন্যথা
	    out_str(T_BE);
    }

    যদি (varp == &T_BSU || varp == &T_ESU)
	term_set_sync_output(TERM_SYNC_OUTPUT_OFF);

    ঘূৰাই রিক্ত;
}


#ifdef UNIX
/*
 * The 'termresize' option is changed.
 */
    ৱর্ণ *
did_set_termresize(optset_T *args UNUSED)
{
    // If empty or "inband", then attempt to enable in-band resize events.
    যদি (*p_trz == NUL || STRCMP(p_trz, "inband") == 0)
	term_set_win_resize(true);
    অন্যথা যদি (STRCMP(p_trz, "sigwinch") == 0)
	term_set_win_resize(false);
    অন্যথা
	ঘূৰাই e_invalid_argument;

    ঘূৰাই রিক্ত;
}

    পূর্ণাঙ্ক
expand_set_termresize(optexpand_T *args, পূর্ণাঙ্ক *অঙ্ক_Matches, char_u ***matches)
{
    ঘূৰাই expand_set_opt_string(
	    args,
	    p_trz_মানসমূহ,
	    ARRAY_LENGTH(p_trz_মানসমূহ) - 1,
	    অঙ্ক_Matches,
	    matches);
}
#endif

#যদি defined(FEAT_TERMINAL)
/*
 * The 'termwinkey' option is changed.
 */
    ৱর্ণ *
did_set_termwinkey(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    যদি ((*varp)[0] != NUL && string_to_চাবি(*varp, TRUE) == 0)
	ঘূৰাই e_invalid_argument;

    ঘূৰাই রিক্ত;
}

/*
 * The 'termwinsize' option is changed.
 */
    ৱর্ণ *
did_set_termwinsize(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u	*p;

    যদি ((*varp)[0] == NUL)
	ঘূৰাই রিক্ত;

    p = skipdigits(*varp);
    যদি (p == *varp || (*p != 'x' && *p != '*') || *skipdigits(p + 1) != NUL)
	ঘূৰাই e_invalid_argument;

    ঘূৰাই রিক্ত;
}

# যদি defined(MSWIN)
/*
 * The 'termwintype' option is changed.
 */
    ৱর্ণ *
did_set_termwintype(optset_T *args UNUSED)
{
    ঘূৰাই did_set_opt_strings(p_twt, p_twt_মানসমূহ, FALSE);
}

    পূর্ণাঙ্ক
expand_set_termwintype(optexpand_T *args, পূর্ণাঙ্ক *অঙ্ক_Matches, char_u ***matches)
{
    ঘূৰাই expand_set_opt_string(
	    args,
	    p_twt_মানসমূহ,
	    ARRAY_LENGTH(p_twt_মানসমূহ) - 1,
	    অঙ্ক_Matches,
	    matches);
}
# endif
#endif

/*
 * The 'titlestring' option is changed.
 */
    ৱর্ণ *
did_set_titlestring(optset_T *args)
{
    পূর্ণাঙ্ক flagval = 0;

#ifdef FEAT_STL_OPT
    flagval = STL_IN_TITLE;
#endif
    ঘূৰাই parse_titleiconstring(args, flagval);
}

#যদি defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
/*
 * The 'toolbar' option is changed.
 */
    ৱর্ণ *
did_set_toolbar(optset_T *args UNUSED)
{
    যদি (opt_strings_flags(p_toolbar, p_toolbar_মানসমূহ, &toolbar_flags,
								TRUE) != OK)
	ঘূৰাই e_invalid_argument;

    out_flush();
    gui_mch_show_toolbar((toolbar_flags &
		(TOOLBAR_TEXT | TOOLBAR_ICONS)) != 0);
    ঘূৰাই রিক্ত;
}

    পূর্ণাঙ্ক
expand_set_toolbar(optexpand_T *args, পূর্ণাঙ্ক *অঙ্ক_Matches, char_u ***matches)
{
    ঘূৰাই expand_set_opt_string(
	    args,
	    p_toolbar_মানসমূহ,
	    ARRAY_LENGTH(p_toolbar_মানসমূহ) - 1,
	    অঙ্ক_Matches,
	    matches);
}
#endif

#যদি defined(FEAT_TOOLBAR) && defined(FEAT_GUI_GTK)
/*
 * The 'toolbariconsize' option is changed.  GTK+ 2 only.
 */
    ৱর্ণ *
did_set_toolbariconsize(optset_T *args UNUSED)
{
    যদি (opt_strings_flags(p_tbis, p_tbis_মানসমূহ, &tbis_flags, FALSE) != OK)
	ঘূৰাই e_invalid_argument;

    out_flush();
    gui_mch_show_toolbar((toolbar_flags &
		(TOOLBAR_TEXT | TOOLBAR_ICONS)) != 0);
    ঘূৰাই রিক্ত;
}

    পূর্ণাঙ্ক
expand_set_toolbariconsize(
    optexpand_T	*args,
    পূর্ণাঙ্ক		*অঙ্ক_Matches,
    char_u	***matches)
{
    ঘূৰাই expand_set_opt_string(
	    args,
	    p_tbis_মানসমূহ,
	    ARRAY_LENGTH(p_tbis_মানসমূহ) - 1,
	    অঙ্ক_Matches,
	    matches);
}
#endif

#যদি defined(UNIX) || defined(VMS)
/*
 * The 'ttymouse' option is changed.
 */
    ৱর্ণ *
did_set_ttymouse(optset_T *args UNUSED)
{
    ৱর্ণ *errmsg = রিক্ত;

    // Switch the mouse off before changing the escape sequences used for
    // that.
    mch_setmouse(FALSE);
    যদি (opt_strings_flags(p_ttym, p_ttym_মানসমূহ, &ttym_flags, FALSE) != OK)
	errmsg = e_invalid_argument;
    অন্যথা
	check_mouse_termcode();
    যদি (termcap_active)
	setmouse();		// may switch it on again

    ঘূৰাই errmsg;
}

    পূর্ণাঙ্ক
expand_set_ttymouse(optexpand_T *args, পূর্ণাঙ্ক *অঙ্ক_Matches, char_u ***matches)
{
    ঘূৰাই expand_set_opt_string(
	    args,
	    p_ttym_মানসমূহ,
	    ARRAY_LENGTH(p_ttym_মানসমূহ) - 1,
	    অঙ্ক_Matches,
	    matches);
}
#endif

#যদি defined(FEAT_VARTABS)
/*
 * The 'varsofttabstop' option is changed.
 */
    ৱর্ণ *
did_set_varsofttabstop(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u *cp;

    যদি (!((*varp)[0]) || ((*varp)[0] == '0' && !((*varp)[1])))
	VIM_CLEAR(curbuf->b_p_vsts_array);
    অন্যথা
    {
	বাবে (cp = *varp; *cp; ++cp)
	{
	    যদি (vim_isdigit(*cp))
		অনুৱর্ত;
	    যদি (*cp == ',' && cp > *varp && *(cp-1) != ',')
		অনুৱর্ত;
	    ঘূৰাই e_invalid_argument;
	}

	পূর্ণাঙ্ক *oldarray = curbuf->b_p_vsts_array;
	যদি (tabstop_set(*varp, &(curbuf->b_p_vsts_array)) == OK)
	{
	    যদি (oldarray)
		vim_free(oldarray);
	}
	অন্যথা
	    ঘূৰাই e_invalid_argument;
    }

    ঘূৰাই রিক্ত;
}

/*
 * The 'vartabstop' option is changed.
 */
    ৱর্ণ *
did_set_vartabstop(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u *cp;

    যদি (!((*varp)[0]) || ((*varp)[0] == '0' && !((*varp)[1])))
	VIM_CLEAR(curbuf->b_p_vts_array);
    অন্যথা
    {
	বাবে (cp = *varp; *cp; ++cp)
	{
	    যদি (vim_isdigit(*cp))
		অনুৱর্ত;
	    যদি (*cp == ',' && cp > *varp && *(cp-1) != ',')
		অনুৱর্ত;
	    ঘূৰাই e_invalid_argument;
	}

	পূর্ণাঙ্ক *oldarray = curbuf->b_p_vts_array;

	যদি (tabstop_set(*varp, &(curbuf->b_p_vts_array)) == OK)
	{
	    vim_free(oldarray);
# ifdef FEAT_FOLDING
	    যদি (foldmethodIsIndent(curwin))
		foldUpdateAll(curwin);
# endif
	}
	অন্যথা
	    ঘূৰাই e_invalid_argument;
    }

    ঘূৰাই রিক্ত;
}
#endif

/*
 * The 'verbosefile' option is changed.
 */
    ৱর্ণ *
did_set_verbosefile(optset_T *args UNUSED)
{
    verbose_stop();
    যদি (*p_vfile != NUL && verbose_open() == FAIL)
	ঘূৰাই e_invalid_argument;

    ঘূৰাই রিক্ত;
}

#যদি defined(FEAT_SESSION)
/*
 * The 'viewoptions' option is changed.
 */
    ৱর্ণ *
did_set_viewoptions(optset_T *args UNUSED)
{
    ঘূৰাই did_set_opt_flags(p_vop, p_ssop_মানসমূহ, &vop_flags, TRUE);
}
#endif

#যদি defined(FEAT_VIMINFO)
/*
 * The 'viminfo' option is changed.
 */
    ৱর্ণ *
did_set_viminfo(optset_T *args)
{
    char_u	*s;
    ৱর্ণ	*errmsg = রিক্ত;

    বাবে (s = p_viminfo; *s;)
    {
	// Check it's a valid character
	যদি (vim_strchr((char_u *)"!\"%'/:<@cfhnrs", *s) == রিক্ত)
	{
	    errmsg = illegal_char(args->os_errbuf, args->os_errbuflen, *s);
	    ভঙ্গ;
	}
	যদি (*s == 'n')	// name is always last one
	    ভঙ্গ;
	অন্যথা যদি (*s == 'r') // skip until next ','
	{
	    যেতিয়ালৈ (*++s && *s != ',')
		;
	}
	অন্যথা যদি (*s == '%')
	{
	    // optional number
	    যেতিয়ালৈ (vim_isdigit(*++s))
		;
	}
	অন্যথা যদি (*s == '!' || *s == 'h' || *s == 'c')
	    ++s;		// no extra chars
	অন্যথা		// must have a number
	{
	    যেতিয়ালৈ (vim_isdigit(*++s))
		;

	    যদি (!VIM_ISDIGIT(*(s - 1)))
	    {
		যদি (args->os_errbuf != রিক্ত)
		{
		    vim_snprintf(args->os_errbuf, args->os_errbuflen,
			    _(e_missing_সংখ্যা_after_angle_str_angle),
			    transchar_byte(*(s - 1)));
		    errmsg = args->os_errbuf;
		}
		অন্যথা
		    errmsg = "";
		ভঙ্গ;
	    }
	}
	যদি (*s == ',')
	    ++s;
	অন্যথা যদি (*s)
	{
	    যদি (args->os_errbuf != রিক্ত)
		errmsg = e_missing_comma;
	    অন্যথা
		errmsg = "";
	    ভঙ্গ;
	}
    }
    যদি (*p_viminfo && errmsg == রিক্ত && get_viminfo_parameter('\'') < 0)
	errmsg = e_must_specify_a_মান;

    ঘূৰাই errmsg;
}
#endif

/*
 * The 'virtualedit' option is changed.
 */
    ৱর্ণ *
did_set_virtualedit(optset_T *args)
{
    char_u		*ve = p_ve;
    unsigned পূর্ণাঙ্ক	*flags = &ve_flags;

    যদি (args->os_flags & OPT_LOCAL)
    {
	ve = curwin->w_p_ve;
	flags = &curwin->w_ve_flags;
    }

    যদি ((args->os_flags & OPT_LOCAL) && *ve == NUL)
	// make the local value empty: use the global value
	*flags = 0;
    অন্যথা
    {
	যদি (opt_strings_flags(ve, p_ve_মানসমূহ, flags, TRUE) != OK)
	    ঘূৰাই e_invalid_argument;
	অন্যথা যদি (STRCMP(ve, args->os_oldval.string) != 0)
	{
	    // Recompute cursor position in case the new 've' setting
	    // changes something.
	    validate_virtcol();
	    coladvance(curwin->w_virtcol);
	}
    }

    ঘূৰাই রিক্ত;
}

    পূর্ণাঙ্ক
expand_set_virtualedit(optexpand_T *args, পূর্ণাঙ্ক *অঙ্ক_Matches, char_u ***matches)
{
    ঘূৰাই expand_set_opt_string(
	    args,
	    p_ve_মানসমূহ,
	    ARRAY_LENGTH(p_ve_মানসমূহ) - 1,
	    অঙ্ক_Matches,
	    matches);
}

/*
 * The 'whichwrap' option is changed.
 */
    ৱর্ণ *
did_set_whichwrap(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    // Add ',' to the list flags because 'whichwrap' is a flag
    // list that is comma-separated.
    ঘূৰাই did_set_option_listflag(*varp, (char_u *)(WW_ALL ","),
		    args->os_errbuf, args->os_errbuflen);
}

    পূর্ণাঙ্ক
expand_set_whichwrap(optexpand_T *args, পূর্ণাঙ্ক *অঙ্ক_Matches, char_u ***matches)
{
    ঘূৰাই expand_set_opt_listflag(args, (char_u*)WW_ALL, অঙ্ক_Matches, matches);
}

/*
 * The 'wildmode' option is changed.
 */
    ৱর্ণ *
did_set_wildmode(optset_T *args UNUSED)
{
    যদি (check_opt_wim() == FAIL)
	ঘূৰাই e_invalid_argument;
    ঘূৰাই রিক্ত;
}

    পূর্ণাঙ্ক
expand_set_wildmode(optexpand_T *args, পূর্ণাঙ্ক *অঙ্ক_Matches, char_u ***matches)
{
    ঘূৰাই expand_set_opt_string(
	    args,
	    p_wim_মানসমূহ,
	    ARRAY_LENGTH(p_wim_মানসমূহ) - 1,
	    অঙ্ক_Matches,
	    matches);
}

/*
 * The 'wildoptions' option is changed.
 */
    ৱর্ণ *
did_set_wildoptions(optset_T *args UNUSED)
{
    ঘূৰাই did_set_opt_strings(p_wop, p_wop_মানসমূহ, TRUE);
}

    পূর্ণাঙ্ক
expand_set_wildoptions(optexpand_T *args, পূর্ণাঙ্ক *অঙ্ক_Matches, char_u ***matches)
{
    ঘূৰাই expand_set_opt_string(
	    args,
	    p_wop_মানসমূহ,
	    ARRAY_LENGTH(p_wop_মানসমূহ) - 1,
	    অঙ্ক_Matches,
	    matches);
}

#যদি defined(FEAT_WAK)
/*
 * The 'winaltkeys' option is changed.
 */
    ৱর্ণ *
did_set_winaltkeys(optset_T *args UNUSED)
{
    ৱর্ণ *errmsg = রিক্ত;

    যদি (*p_wak == NUL || check_opt_strings(p_wak, p_wak_মানসমূহ, FALSE) != OK)
	errmsg = e_invalid_argument;
# ifdef FEAT_MENU
#  যদি defined(FEAT_GUI_MOTIF)
    অন্যথা যদি (gui.in_use)
	gui_motif_set_mnemonics(p_wak[0] == 'y' || p_wak[0] == 'm');
#  elif defined(FEAT_GUI_GTK)
    অন্যথা যদি (gui.in_use)
	gui_gtk_set_mnemonics(p_wak[0] == 'y' || p_wak[0] == 'm');
#  endif
# endif
    ঘূৰাই errmsg;
}

    পূর্ণাঙ্ক
expand_set_winaltkeys(optexpand_T *args, পূর্ণাঙ্ক *অঙ্ক_Matches, char_u ***matches)
{
    ঘূৰাই expand_set_opt_string(
	    args,
	    p_wak_মানসমূহ,
	    ARRAY_LENGTH(p_wak_মানসমূহ) - 1,
	    অঙ্ক_Matches,
	    matches);
}
#endif

/*
 * The 'wincolor' option is changed.
 */
    ৱর্ণ *
did_set_wincolor(optset_T *args UNUSED)
{
    update_wincolor(curwin, args->os_newval.string);
    ঘূৰাই রিক্ত;
}

/*
 * The 'winhighlight' option is changed
 */
    ৱর্ণ *
did_set_winhighlight(optset_T *args)
{
    ঘূৰাই update_winhighlight(curwin, args->os_newval.string);
}

/*
 * Expand 'winhighlight' option.
 */
    পূর্ণাঙ্ক
expand_set_winhighlight(optexpand_T *args, পূর্ণাঙ্ক *অঙ্ক_Matches, char_u ***matches)
{
    expand_T	    *xp = args->oe_xp;

    যদি ((xp->xp_pattern > args->oe_set_arg && *(xp->xp_pattern-1) == ':')
	    || xp->xp_pattern == args->oe_set_arg || *(xp->xp_pattern-1) == ',')
    {
	// After a ':' or after a ',', or at the start, so expand highlight
	// group name.

	// If starts with !, then expand 'highlight' occasions.
	যদি (*xp->xp_pattern == '!')
	    ঘূৰাই expand_hl_occasions(args, অঙ্ক_Matches, matches, '!');
	অন্যথা
	    ঘূৰাই expand_set_opt_generic(
		    args,
		    get_highlight_name,
		    অঙ্ক_Matches,
		    matches);
    }

    VIM_CLEAR(*matches);
    ঘূৰাই FAIL;
}

    পূর্ণাঙ্ক
expand_set_wincolor(optexpand_T *args, পূর্ণাঙ্ক *অঙ্ক_Matches, char_u ***matches)
{
    ঘূৰাই expand_set_opt_generic(
	    args,
	    get_highlight_name,
	    অঙ্ক_Matches,
	    matches);
}

#ifdef FEAT_SYN_HL
/*
 * When the 'syntax' option is set, load the syntax of that name.
 */
    স্থির শূন্য
do_syntax_autocmd(পূর্ণাঙ্ক মান_changed)
{
    স্থির পূর্ণাঙ্ক syn_recursive = 0;

    ++syn_recursive;
    // Only pass TRUE for "force" when the value changed or not used
    // recursively, to avoid endless recurrence.
    apply_autocmds(EVENT_SYNTAX, curbuf->b_p_syn, curbuf->b_fname,
	    মান_changed || syn_recursive == 1, curbuf);
    curbuf->b_flags |= BF_SYN_SET;
    --syn_recursive;
}
#endif

/*
 * When the 'filetype' option is set, trigger the FileType autocommand.
 */
    স্থির শূন্য
do_filetype_autocmd(char_u **varp, পূর্ণাঙ্ক opt_flags, পূর্ণাঙ্ক মান_changed)
{
    // Skip this when called from a modeline and the filetype was already set
    // to this value.
    যদি ((opt_flags & OPT_MODELINE) && !মান_changed)
	ঘূৰাই;

    স্থির পূর্ণাঙ্ক  ft_recursive = 0;
    পূর্ণাঙ্ক	    secure_save = secure;

    // Reset the secure flag, since the value of 'filetype' has
    // been checked to be safe.
    secure = 0;

    ++ft_recursive;
    curbuf->b_did_filetype = true;
    // Only pass TRUE for "force" when the value changed or not
    // used recursively, to avoid endless recurrence.
    apply_autocmds(EVENT_FILETYPE, curbuf->b_p_ft, curbuf->b_fname,
	    মান_changed || ft_recursive == 1, curbuf);
    --ft_recursive;
    // Just in case the old "curbuf" is now invalid.
    যদি (varp != &(curbuf->b_p_ft))
	varp = রিক্ত;

    secure = secure_save;
}

#ifdef FEAT_SPELL
/*
 * When the 'spelllang' option is set, source the spell/LANG.vim file in
 * 'runtimepath'.
 */
    স্থির শূন্য
do_spelllang_source(শূন্য)
{
    char_u	fname[200];
    char_u	*p;
    char_u	*q = curwin->w_s->b_p_spl;

    // Skip the first name if it is "cjk".
    যদি (STRNCMP(q, "cjk,", 4) == 0)
	q += 4;

    // They could set 'spellcapcheck' depending on the language.  Use the first
    // name in 'spelllang' up to '_region' or '.encoding'.
    বাবে (p = q; *p != NUL; ++p)
	যদি (!ASCII_ISALNUM(*p) && *p != '-')
	    ভঙ্গ;
    যদি (p > q)
    {
	vim_snprintf((ৱর্ণ *)fname, 200, "spell/%.*s.vim",
		(পূর্ণাঙ্ক)(p - q), q);
	source_runtime(fname, DIP_ALL);
    }
}
#endif

/*
 * Handle string options that need some action to perform when changed.
 * The new value must be allocated.
 * Returns NULL for success, or an untranslated error message for an error.
 */
    ৱর্ণ *
did_set_string_option(
    পূর্ণাঙ্ক		opt_idx,		// index in options[] table
    char_u	**varp,			// pointer to the option variable
    char_u	*oldval,		// previous value of the option
    char_u	*মান,			// new value of the option
    ৱর্ণ	*errbuf,		// buffer for errors, or NULL
    size_t	errbuflen,		// length of error buffer
    পূর্ণাঙ্ক		opt_flags,		// OPT_LOCAL and/or OPT_GLOBAL
    set_op_T    op,			// OP_ADDING/OP_PREPENDING/OP_REMOVING
    পূর্ণাঙ্ক		*মান_checked)		// value was checked to be safe, no
					// need to set P_INSECURE
{
    ৱর্ণ	*errmsg = রিক্ত;
    long_u	free_oldval = (get_option_flags(opt_idx) & P_ALLOCED);
    opt_did_set_cb_T did_set_cb = get_option_did_set_cb(opt_idx);
    optset_T	args;

    // 'ttytype' is an alias for 'term'.  Both 'term' and 'ttytype' point to
    // T_NAME.  If 'term' or 'ttytype' is modified, then use the index for the
    // 'term' option.  Only set the P_ALLOCED flag on 'term'.
    যদি (varp == &T_NAME)
    {
	opt_idx = findoption((char_u *)"term");
	যদি (opt_idx >= 0)
	{
	    free_oldval = (get_option_flags(opt_idx) & P_ALLOCED);
	    did_set_cb = get_option_did_set_cb(opt_idx);
	}
    }

    CLEAR_FIELD(args);

    // Disallow changing some options from secure mode
    যদি ((secure
#ifdef HAVE_SANDBOX
		|| sandbox != 0
#endif
		) && (get_option_flags(opt_idx) & P_SECURE))
	errmsg = e_not_allowed_here;
    // Check for a "normal" directory or file name in some options.
    অন্যথা যদি (check_illegal_path_names(opt_idx, varp))
	errmsg = e_invalid_argument;
    অন্যথা যদি (did_set_cb != রিক্ত)
    {
	args.os_varp = (char_u *)varp;
	args.os_idx = opt_idx;
	args.os_flags = opt_flags;
	args.os_op = op;
	args.os_oldval.string = oldval;
	args.os_newval.string = মান;
	args.os_errbuf = errbuf;
	args.os_errbuflen = errbuflen;
	// Invoke the option specific callback function to validate and apply
	// the new option value.
	errmsg = did_set_cb(&args);

	// The 'keymap', 'filetype' and 'syntax' option callback functions
	// may change the os_value_checked field.
	*মান_checked = args.os_মান_checked;
    }

    // If an error is detected, restore the previous value.
    যদি (errmsg != রিক্ত)
    {
	free_string_option(*varp);
	*varp = oldval;
	// When resetting some values, need to act on it.
	যদি (args.os_restore_chartab)
	    (শূন্য)init_chartab();
	যদি (varp == &p_hl)
	    (শূন্য)highlight_changed();
    }
    অন্যথা
    {
#ifdef FEAT_EVAL
	// Remember where the option was set.
	set_option_sctx_idx(opt_idx, opt_flags, current_sctx);
#endif
	// Free string options that are in allocated memory.
	// Use "free_oldval", because recursiveness may change the flags under
	// our fingers (esp. init_highlight()).
	যদি (free_oldval)
	    free_string_option(oldval);
	set_option_flag(opt_idx, P_ALLOCED);

	যদি ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0
		&& is_global_local_option(opt_idx))
	{
	    // global option with local value set to use global value; free
	    // the local value and make it empty
	    char_u *p = get_option_varp_scope(opt_idx, OPT_LOCAL);
	    free_string_option(*(char_u **)p);
	    *(char_u **)p = empty_option;
	}

	// May set global value for local option.
	অন্যথা যদি (!(opt_flags & OPT_LOCAL) && opt_flags != OPT_GLOBAL)
	    set_string_option_global(opt_idx, varp);

	// Trigger the autocommand only after setting the flags.
#ifdef FEAT_SYN_HL
	যদি (varp == &(curbuf->b_p_syn))
	    do_syntax_autocmd(args.os_মান_changed);
#endif
	অন্যথা যদি (varp == &(curbuf->b_p_ft))
	    do_filetype_autocmd(varp, opt_flags, args.os_মান_changed);
#ifdef FEAT_SPELL
	যদি (varp == &(curwin->w_s->b_p_spl))
	    do_spelllang_source();
#endif
    }

    যদি (varp == &p_mouse)
    {
	যদি (*p_mouse == NUL)
	    mch_setmouse(FALSE);    // switch mouse off
	অন্যথা
	    setmouse();		    // in case 'mouse' changed
    }

    যদি (varp == &p_rtp)
    {
	export_myvimdir();
#যদি defined(FEAT_LUA)
	update_package_paths_in_lua();
#endif
    }

#যদি defined(FEAT_LINEBREAK)
    // Changing Formatlistpattern when briopt includes the list setting:
    // redraw
    যদি ((varp == &p_flp || varp == &(curbuf->b_p_flp))
	    && curwin->w_briopt_list)
	redraw_all_later(UPD_NOT_VALID);
#endif

    যদি (curwin->w_curswant != MAXCOL
		   && (get_option_flags(opt_idx) & (P_CURSWANT | P_RALL)) != 0
				&& (get_option_flags(opt_idx) & P_HLONLY) == 0)
	curwin->w_set_curswant = true;

    যদি ((opt_flags & OPT_NO_REDRAW) == 0)
    {
#ifdef FEAT_GUI
	// set when changing an option that only requires a redraw in the GUI
	পূর্ণাঙ্ক	redraw_gui_only = FALSE;

	যদি (varp == &p_go			// 'guioptions'
		|| varp == &p_guifont		// 'guifont'
# ifdef FEAT_GUI_TABLINE
		|| varp == &p_gtl		// 'guitablabel'
		|| varp == &p_gtt		// 'guitabtooltip'
# endif
# ifdef FEAT_XFONTSET
		|| varp == &p_guifontset	// 'guifontset'
# endif
		|| varp == &p_guifontwide	// 'guifontwide'
# যদি defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
		|| varp == &p_guiligatures	// 'guiligatures'
# endif
	    )
	    redraw_gui_only = TRUE;

	// check redraw when it's not a GUI option or the GUI is active.
	যদি (!redraw_gui_only || gui.in_use)
#endif
	    check_redraw(get_option_flags(opt_idx));
    }

#যদি defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
    যদি (args.os_did_swaptcap)
    {
	set_termname((char_u *)"win32");
	init_highlight(TRUE, FALSE);
    }
#endif

    ঘূৰাই errmsg;
}

/*
 * Check an option that can be a range of string values.
 *
 * Return OK for correct value, FAIL otherwise.
 * Empty is always OK.
 */
    স্থির পূর্ণাঙ্ক
check_opt_strings(
    char_u	*val,
    ৱর্ণ	**মানসমূহ,
    পূর্ণাঙ্ক		list)	    // when TRUE: accept a list of values
{
    ঘূৰাই opt_strings_flags(val, মানসমূহ, রিক্ত, list);
}

/*
 * Handle an option that can be a range of string values.
 * Set a flag in "*flagp" for each string present.
 *
 * Return OK for correct value, FAIL otherwise.
 * Empty is always OK.
 */
    স্থির পূর্ণাঙ্ক
opt_strings_flags(
    char_u	*val,		// new value
    ৱর্ণ	**মানসমূহ,	// array of valid string values
    unsigned	*flagp,
    পূর্ণাঙ্ক		list)		// when TRUE: accept a list of values
{
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		len;
    unsigned	new_flags = 0;

    যেতিয়ালৈ (*val)
    {
	বাবে (i = 0; ; ++i)
	{
	    যদি (মানসমূহ[i] == রিক্ত)	// val not found in values[]
		ঘূৰাই FAIL;

	    len = (পূর্ণাঙ্ক)STRLEN(মানসমূহ[i]);
	    যদি (STRNCMP(মানসমূহ[i], val, len) == 0
		    && ((list && val[len] == ',') || val[len] == NUL))
	    {
		val += len + (val[len] == ',');
		new_flags |= (1 << i);
		ভঙ্গ;		// check next item in val list
	    }
	}
    }
    যদি (flagp != রিক্ত)
	*flagp = new_flags;

    ঘূৰাই OK;
}

/*
 * return OK if "p" is a valid fileformat name, FAIL otherwise.
 */
    পূর্ণাঙ্ক
check_ff_মান(char_u *p)
{
    ঘূৰাই check_opt_strings(p, p_ff_মানসমূহ, FALSE);
}

/*
 * Save the actual shortmess Flags and clear them temporarily to avoid that
 * file messages overwrites any output from the following commands.
 *
 * Caller must make sure to first call save_clear_shm_value() and then
 * restore_shm_value() exactly the same number of times.
 */
    শূন্য
save_clear_shm_মান(শূন্য)
{
    যদি (STRLEN(p_shm) >= SHM_LEN)
    {
	iemsg(e_internal_error_shortmess_too_long);
	ঘূৰাই;
    }

    যদি (++set_shm_recursive == 1)
    {
	STRCPY(shm_buf, p_shm);
	set_option_মান_give_err((char_u *)"shm", 0L, (char_u *)"", 0);
    }
}

/*
 * Restore the shortmess Flags set from the save_clear_shm_value() function.
 */
    শূন্য
restore_shm_মান(শূন্য)
{
    যদি (--set_shm_recursive == 0)
    {
	set_option_মান_give_err((char_u *)"shm", 0L, shm_buf, 0);
	vim_memset(shm_buf, 0, SHM_LEN);
    }
}

/*
 * Export the environment variable $MYVIMDIR to the first item in runtimepath
 */
    শূন্য
export_myvimdir(শূন্য)
{
    পূর্ণাঙ্ক		dofree = FALSE;
    char_u	*p;
    char_u	*q = p_rtp;
    string_T	buf;

    buf.string = alloc(MAXPATHL);
    যদি (buf.string == রিক্ত)
	ঘূৰাই;

    buf.length = (size_t)copy_option_part(&q, buf.string, MAXPATHL, ",");

    p = vim_getenv((char_u *)"MYVIMDIR", &dofree);

    যদি (p == রিক্ত || STRCMP(p, buf.string) != 0)
    {
	যদি (*buf.string != NUL && !after_pathsep(buf.string, buf.string + buf.length))
	    STRCPY(buf.string + buf.length, PATHSEPSTR);
#ifdef MSWIN
	// normalize path separators
	বাবে (q = buf.string; *q != NUL; q++)
	    যদি (*q == '/')
		*q = '\\';
#endif
	vim_setenv((char_u *)"MYVIMDIR", buf.string);
    }
    যদি (dofree)
	vim_free(p);
    vim_free(buf.string);
}
