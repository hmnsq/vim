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

#அந்தர்பாவ "vim.h"

நிலையான char_u shm_buf[SHM_LEN];
நிலையான முழு set_shm_recursive = 0;

நிலையான எழுத்து *(p_ambw_மதிப்புகள்[]) = {"single", "double", வெற்று};
நிலையான எழுத்து *(p_bg_மதிப்புகள்[]) = {"light", "dark", வெற்று};
நிலையான எழுத்து *(p_bkc_மதிப்புகள்[]) = {"yes", "auto", "no", "breaksymlink", "breakhardlink", வெற்று};
நிலையான எழுத்து *(p_bo_மதிப்புகள்[]) = {"all", "backspace", "cursor", "complete",
				 "copy", "ctrlg", "error", "esc", "ex",
				 "hangul", "insertmode", "lang", "mess",
				 "showmatch", "operator", "register", "shell",
				 "spell", "term", "wildmode", வெற்று};
#எனில் defined(FEAT_LINEBREAK)
// Note: Keep this in sync with briopt_check()
நிலையான எழுத்து *(p_briopt_மதிப்புகள்[]) = {"shift:", "min:", "sbr", "list:", "column:", வெற்று};
#endif
#எனில் defined(FEAT_TABPANEL)
// Note: Keep this in sync with tabpanelopt_changed()
நிலையான எழுத்து *(p_tplo_மதிப்புகள்[]) = {"align:", "columns:", "scrollbar", "vert", வெற்று};
நிலையான எழுத்து *(p_tplo_align_மதிப்புகள்[]) = {"left", "right", வெற்று};
#endif
#எனில் defined(FEAT_DIFF)
// Note: Keep this in sync with diffopt_changed()
நிலையான எழுத்து *(p_dip_மதிப்புகள்[]) = {"filler", "anchor", "context:", "iblank", "icase", "iwhite", "iwhiteall", "iwhiteeol", "horizontal", "vertical", "closeoff", "hiddenoff", "foldcolumn:", "followwrap", "internal", "indent-heuristic", "algorithm:", "inline:", "linematch:", வெற்று};
நிலையான எழுத்து *(p_dip_algorithm_மதிப்புகள்[]) = {"myers", "minimal", "patience", "histogram", வெற்று};
நிலையான எழுத்து *(p_dip_inline_மதிப்புகள்[]) = {"none", "simple", "char", "word", வெற்று};
#endif
நிலையான எழுத்து *(p_nf_மதிப்புகள்[]) = {"bin", "octal", "hex", "alpha", "unsigned", "blank", வெற்று};
நிலையான எழுத்து *(p_ff_மதிப்புகள்[]) = {FF_UNIX, FF_DOS, FF_MAC, வெற்று};
#ifdef HAVE_CLIPMETHOD
// Note: Keep this in sync with did_set_clipboard()
நிலையான எழுத்து *(p_cb_மதிப்புகள்[]) = {"unnamed", "unnamedplus",
# ifdef FEAT_CLIPBOARD
    "autoselect", "autoselectplus", "autoselectml", "html", "exclude:",
# endif
    வெற்று};
#endif
#ifdef FEAT_CRYPT
நிலையான எழுத்து *(p_cm_மதிப்புகள்[]) = {"zip", "blowfish", "blowfish2",
# ifdef FEAT_SODIUM
    "xchacha20", "xchacha20v2",
# endif
    வெற்று};
#endif
நிலையான எழுத்து *(p_cmp_மதிப்புகள்[]) = {"internal", "keepascii", வெற்று};
#ifdef FEAT_SYN_HL
// Note: Keep this in sync with fill_culopt_flags()
நிலையான எழுத்து *(p_culopt_மதிப்புகள்[]) = {"line", "screenline", "number", "both", வெற்று};
#endif
நிலையான எழுத்து *(p_dy_மதிப்புகள்[]) = {"lastline", "truncate", "uhex", வெற்று};
நிலையான எழுத்து *(p_jop_மதிப்புகள்[]) = {"stack", வெற்று};
#ifdef FEAT_FOLDING
நிலையான எழுத்து *(p_fdo_மதிப்புகள்[]) = {"all", "block", "hor", "mark", "percent",
				 "quickfix", "search", "tag", "insert",
				 "undo", "jump", வெற்று};
#endif
// Note: Keep this in sync with match_keyprotocol()
நிலையான எழுத்து *(p_kpc_protocol_மதிப்புகள்[]) = {"none", "mok2", "kitty", வெற்று};
#ifdef FEAT_IMAGE
// Note: Keep this in sync with match_imageprotocol()
நிலையான எழுத்து *(p_ipc_protocol_மதிப்புகள்[]) = {"none", "kitty", "sixel", வெற்று};
#endif
#ifdef FEAT_PROP_POPUP
// Note: Keep this in sync with parse_popup_option()
நிலையான எழுத்து *(p_popup_cpp_option_மதிப்புகள்[]) = {"align:", "border:",
    "borderhighlight:", "close:", "height:", "highlight:", "opacity:",
    "resize:", "shadow:", "width:", வெற்று};
நிலையான எழுத்து *(p_popup_pvp_option_மதிப்புகள்[]) = {"border:",
    "borderhighlight:", "close:", "height:", "highlight:", "opacity:",
    "resize:", "shadow:", "width:", வெற்று};
நிலையான எழுத்து *(p_popup_option_on_off_மதிப்புகள்[]) = {"on", "off", வெற்று};
நிலையான எழுத்து *(p_popup_cpp_border_மதிப்புகள்[]) = {"single", "double", "round",
    "ascii", "on", "off", "custom:", வெற்று};
நிலையான எழுத்து *(p_popup_pvp_border_மதிப்புகள்[]) = {"single", "double", "round",
    "ascii", "on", "off", "custom:", வெற்று};
நிலையான எழுத்து *(p_popup_option_align_மதிப்புகள்[]) = {"item", "menu", வெற்று};
#endif
#எனில் defined(FEAT_SPELL)
// Note: Keep this in sync with spell_check_sps()
நிலையான எழுத்து *(p_sps_மதிப்புகள்[]) = {"best", "fast", "double", "expr:", "file:", "timeout:", வெற்று};
#endif
#ifdef FEAT_SESSION
// Also used for 'viewoptions'!  Keep in sync with SSOP_ flags.
நிலையான எழுத்து *(p_ssop_மதிப்புகள்[]) = {"buffers", "winpos", "resize", "winsize",
    "localoptions", "options", "help", "blank", "globals", "slash", "unix",
    "sesdir", "curdir", "folds", "cursor", "tabpages", "terminal", "skiprtp",
    வெற்று};
#endif
#எனில் defined(FEAT_STL_OPT)
நிலையான எழுத்து *(p_stlo_மதிப்புகள்[]) = {"fixedheight", "maxheight:", வெற்று};
#endif
// Keep in sync with SWB_ flags in option.h
நிலையான எழுத்து *(p_swb_மதிப்புகள்[]) = {"useopen", "usetab", "split", "newtab", "vsplit", "uselast", வெற்று};
நிலையான எழுத்து *(p_spk_மதிப்புகள்[]) = {"cursor", "screen", "topline", வெற்று};
நிலையான எழுத்து *(p_tc_மதிப்புகள்[]) = {"followic", "ignore", "match", "followscs", "smart", வெற்று};
// Keep in sync with TCL_ flags in option.h
நிலையான எழுத்து *(p_tcl_மதிப்புகள்[]) = {"left", "uselast", வெற்று};
#எனில் defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
நிலையான எழுத்து *(p_toolbar_மதிப்புகள்[]) = {"text", "icons", "tooltips", "horiz", வெற்று};
#endif
#எனில் defined(FEAT_TOOLBAR) && defined(FEAT_GUI_GTK)
நிலையான எழுத்து *(p_tbis_மதிப்புகள்[]) = {"tiny", "small", "medium", "large", "huge", "giant", வெற்று};
#endif
#எனில் defined(UNIX) || defined(VMS)
நிலையான எழுத்து *(p_ttym_மதிப்புகள்[]) = {"xterm", "xterm2", "dec", "netterm", "jsbterm", "pterm", "urxvt", "sgr", வெற்று};
#endif
நிலையான எழுத்து *(p_ve_மதிப்புகள்[]) = {"block", "insert", "all", "onemore", "none", "NONE", வெற்று};
// Note: Keep this in sync with check_opt_wim()
நிலையான எழுத்து *(p_wim_மதிப்புகள்[]) = {"full", "longest", "list", "lastused", "noselect", "noinsert", வெற்று};
நிலையான எழுத்து *(p_wop_மதிப்புகள்[]) = {"fuzzy", "tagfile", "pum", "exacttext", வெற்று};
#ifdef FEAT_WAK
நிலையான எழுத்து *(p_wak_மதிப்புகள்[]) = {"yes", "menu", "no", வெற்று};
#endif
நிலையான எழுத்து *(p_mousem_மதிப்புகள்[]) = {"extend", "popup", "popup_setpos", "mac", வெற்று};
நிலையான எழுத்து *(p_sel_மதிப்புகள்[]) = {"inclusive", "exclusive", "old", வெற்று};
நிலையான எழுத்து *(p_slm_மதிப்புகள்[]) = {"mouse", "key", "cmd", வெற்று};
நிலையான எழுத்து *(p_km_மதிப்புகள்[]) = {"startsel", "stopsel", வெற்று};
#ifdef FEAT_BROWSE
நிலையான எழுத்து *(p_bsdir_மதிப்புகள்[]) = {"current", "last", "buffer", வெற்று};
#endif
நிலையான எழுத்து *(p_scbopt_மதிப்புகள்[]) = {"ver", "hor", "jump", வெற்று};
நிலையான எழுத்து *(p_debug_மதிப்புகள்[]) = {"msg", "throw", "beep", வெற்று};
நிலையான எழுத்து *(p_ead_மதிப்புகள்[]) = {"both", "ver", "hor", வெற்று};
நிலையான எழுத்து *(p_buftype_மதிப்புகள்[]) = {"nofile", "nowrite", "quickfix", "help", "terminal", "acwrite", "prompt", "popup", வெற்று};
நிலையான எழுத்து *(p_bufhidden_மதிப்புகள்[]) = {"hide", "unload", "delete", "wipe", வெற்று};
நிலையான எழுத்து *(p_bs_மதிப்புகள்[]) = {"indent", "eol", "start", "nostop", வெற்று};
#ifdef FEAT_FOLDING
நிலையான எழுத்து *(p_fdm_மதிப்புகள்[]) = {"manual", "expr", "marker", "indent", "syntax",
# ifdef FEAT_DIFF
				"diff",
# endif
				வெற்று};
நிலையான எழுத்து *(p_fcl_மதிப்புகள்[]) = {"all", வெற்று};
#endif
நிலையான எழுத்து *(p_cfc_மதிப்புகள்[]) = {"keyword", "files", "whole_line", வெற்று};
நிலையான எழுத்து *(p_cot_மதிப்புகள்[]) = {"menu", "menuone", "longest", "preview", "popup", "popuphidden", "noinsert", "noselect", "fuzzy", "nosort", "preinsert", "nearest", வெற்று};
#ifdef BACKSLASH_IN_FILENAME
நிலையான எழுத்து *(p_csl_மதிப்புகள்[]) = {"slash", "backslash", வெற்று};
#endif
#ifdef FEAT_SIGNS
நிலையான எழுத்து *(p_scl_மதிப்புகள்[]) = {"yes", "no", "auto", "number", வெற்று};
#endif
#ifdef UNIX
நிலையான எழுத்து *(p_trz_மதிப்புகள்[]) = {"inband", "sigwinch", "", வெற்று};
#endif
#எனில் defined(MSWIN) && defined(FEAT_TERMINAL)
நிலையான எழுத்து *(p_twt_மதிப்புகள்[]) = {"winpty", "conpty", "", வெற்று};
#endif
நிலையான எழுத்து *(p_sloc_மதிப்புகள்[]) = {"last", "statusline", "tabline", வெற்று};
நிலையான எழுத்து *(p_sws_மதிப்புகள்[]) = {"fsync", "sync", வெற்று};

நிலையான முழு check_opt_strings(char_u *val, எழுத்து **மதிப்புகள், முழு list);
நிலையான முழு opt_strings_flags(char_u *val, எழுத்து **மதிப்புகள், unsigned *flagp, முழு list);

/*
 * After setting various option values: recompute variables that depend on
 * option values.
 */
    வெறுமை
didset_string_options(வெறுமை)
{
    (வெறுமை)opt_strings_flags(p_cmp, p_cmp_மதிப்புகள், &cmp_flags, TRUE);
    (வெறுமை)opt_strings_flags(p_bkc, p_bkc_மதிப்புகள், &bkc_flags, TRUE);
    (வெறுமை)opt_strings_flags(p_bo, p_bo_மதிப்புகள், &bo_flags, TRUE);
    (வெறுமை)opt_strings_flags(p_cot, p_cot_மதிப்புகள், &cot_flags, TRUE);
#ifdef FEAT_SESSION
    (வெறுமை)opt_strings_flags(p_ssop, p_ssop_மதிப்புகள், &ssop_flags, TRUE);
    (வெறுமை)opt_strings_flags(p_vop, p_ssop_மதிப்புகள், &vop_flags, TRUE);
#endif
#ifdef FEAT_FOLDING
    (வெறுமை)opt_strings_flags(p_fdo, p_fdo_மதிப்புகள், &fdo_flags, TRUE);
#endif
    (வெறுமை)opt_strings_flags(p_dy, p_dy_மதிப்புகள், &dy_flags, TRUE);
    (வெறுமை)opt_strings_flags(p_jop, p_jop_மதிப்புகள், &jop_flags, TRUE);
    (வெறுமை)opt_strings_flags(p_tc, p_tc_மதிப்புகள், &tc_flags, FALSE);
    (வெறுமை)opt_strings_flags(p_ve, p_ve_மதிப்புகள், &ve_flags, TRUE);
#எனில் defined(UNIX) || defined(VMS)
    (வெறுமை)opt_strings_flags(p_ttym, p_ttym_மதிப்புகள், &ttym_flags, FALSE);
#endif
#எனில் defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
    (வெறுமை)opt_strings_flags(p_toolbar, p_toolbar_மதிப்புகள், &toolbar_flags, TRUE);
#endif
#எனில் defined(FEAT_TOOLBAR) && defined(FEAT_GUI_GTK)
    (வெறுமை)opt_strings_flags(p_tbis, p_tbis_மதிப்புகள், &tbis_flags, FALSE);
#endif
    (வெறுமை)opt_strings_flags(p_swb, p_swb_மதிப்புகள், &swb_flags, TRUE);
    (வெறுமை)opt_strings_flags(p_tcl, p_tcl_மதிப்புகள், &tcl_flags, TRUE);
}

#எனில் defined(FEAT_EVAL)
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
    வெறுமை
trigger_optionset_string(
	முழு	opt_idx,
	முழு	opt_flags,
	char_u  *oldval,
	char_u  *oldval_l,
	char_u  *oldval_g,
	char_u  *newval)
{
    // Don't do this recursively.
    எனில் (oldval == வெற்று || newval == வெற்று
				    || *get_vim_var_str(VV_OPTION_TYPE) != NUL)
	திரும்பு;

    char_u buf_type[7];
    size_t buf_typelen;
    size_t oldvallen;

    oldvallen = STRLEN(oldval);
    set_vim_var_string(VV_OPTION_OLD, oldval, (முழு)oldvallen);
    set_vim_var_string(VV_OPTION_NEW, newval, -1);
    buf_typelen = vim_snprintf_safelen((எழுத்து *)buf_type, அளவு(buf_type),
	"%s", (opt_flags & OPT_LOCAL) ? "local" : "global");
    set_vim_var_string(VV_OPTION_TYPE, buf_type, (முழு)buf_typelen);
    எனில் (opt_flags & OPT_LOCAL)
    {
	set_vim_var_string(VV_OPTION_COMMAND, (char_u *)"setlocal", STRLEN_LITERAL("setlocal"));
	set_vim_var_string(VV_OPTION_OLDLOCAL, oldval, (முழு)oldvallen);
    }
    எனில் (opt_flags & OPT_GLOBAL)
    {
	set_vim_var_string(VV_OPTION_COMMAND, (char_u *)"setglobal", STRLEN_LITERAL("setglobal"));
	set_vim_var_string(VV_OPTION_OLDGLOBAL, oldval, (முழு)oldvallen);
    }
    எனில் ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	set_vim_var_string(VV_OPTION_COMMAND, (char_u *)"set", STRLEN_LITERAL("set"));
	set_vim_var_string(VV_OPTION_OLDLOCAL, oldval_l, -1);
	set_vim_var_string(VV_OPTION_OLDGLOBAL, oldval_g, -1);
    }
    எனில் (opt_flags & OPT_MODELINE)
    {
	set_vim_var_string(VV_OPTION_COMMAND, (char_u *)"modeline", STRLEN_LITERAL("modeline"));
	set_vim_var_string(VV_OPTION_OLDLOCAL, oldval, (முழு)oldvallen);
    }
    apply_autocmds(EVENT_OPTIONSET,
	    get_option_fullname(opt_idx), வெற்று, FALSE,
	    வெற்று);
    reset_v_option_vars();
}
#endif

    நிலையான எழுத்து *
illegal_char(எழுத்து *errbuf, size_t errbuflen, முழு c)
{
    எனில் (errbuf == வெற்று)
	திரும்பு "";
    vim_snprintf(errbuf, errbuflen, _(e_illegal_character_str),
		    (எழுத்து *)transchar(c));
    திரும்பு errbuf;
}

    நிலையான எழுத்து *
illegal_char_after_chr(எழுத்து *errbuf, size_t errbuflen, முழு c)
{
    எனில் (errbuf == வெற்று)
	திரும்பு "";
    vim_snprintf(errbuf, errbuflen, _(e_illegal_character_after_chr), c);
    திரும்பு errbuf;
}

/*
 * Check string options in a buffer for NULL value.
 */
    வெறுமை
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
#எனில் defined(FEAT_EVAL)
    check_string_option(&buf->b_p_inde);
    check_string_option(&buf->b_p_indk);
#endif
#எனில் defined(FEAT_BEVAL) && defined(FEAT_EVAL)
    check_string_option(&buf->b_p_bexpr);
#endif
#எனில் defined(FEAT_CRYPT)
    check_string_option(&buf->b_p_cm);
#endif
    check_string_option(&buf->b_p_fp);
#எனில் defined(FEAT_EVAL)
    check_string_option(&buf->b_p_fex);
#endif
#ifdef FEAT_CRYPT
    check_string_option(&buf->b_p_சாவி);
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
    வெறுமை
free_string_option(char_u *p)
{
    எனில் (p != empty_option)
	vim_free(p);
}

    வெறுமை
clear_string_option(char_u **pp)
{
    எனில் (*pp != empty_option)
	vim_free(*pp);
    *pp = empty_option;
}

    வெறுமை
check_string_option(char_u **pp)
{
    எனில் (*pp == வெற்று)
	*pp = empty_option;
}

/*
 * Set global value for string option when it's a local option.
 */
    நிலையான வெறுமை
set_string_option_global(
    முழு		opt_idx,	// option index
    char_u	**varp)		// pointer to option variable
{
    char_u	**p, *s;

    // the global value is always allocated
    எனில் (is_window_local_option(opt_idx))
	p = (char_u **)GLOBAL_WO(varp);
    இல்லையெனில்
	p = (char_u **)get_option_var(opt_idx);
    எனில் (!is_global_option(opt_idx)
	    && p != varp
	    && (s = vim_strsave(*varp)) != வெற்று)
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
    வெறுமை
set_string_option_direct(
    char_u	*name,
    முழு		opt_idx,
    char_u	*val,
    முழு		opt_flags,	// OPT_FREE, OPT_LOCAL and/or OPT_GLOBAL
    முழு		set_sid UNUSED)
{
    char_u	*s;
    char_u	**varp;
    முழு		both = (opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0;
    முழு		idx = opt_idx;

    எனில் (idx == -1)		// use name
    {
	idx = findoption(name);
	எனில் (idx < 0)	// not found (should not happen)
	{
	    semsg(_(e_internal_error_str), "set_string_option_direct()");
	    siemsg("For option %s", name);
	    திரும்பு;
	}
    }

    எனில் (is_hidden_option(idx))		// can't set hidden option
	திரும்பு;

    s = vim_strsave(val);
    எனில் (s == வெற்று)
	திரும்பு;

    varp = (char_u **)get_option_varp_scope(idx,
	    both ? OPT_LOCAL : opt_flags);
    எனில் ((opt_flags & OPT_FREE) && (get_option_flags(idx) & P_ALLOCED))
	free_string_option(*varp);
    *varp = s;

    // For buffer/window local option may also set the global value.
    எனில் (both)
	set_string_option_global(idx, varp);

    set_option_flag(idx, P_ALLOCED);

    // When setting both values of a global option with a local value,
    // make the local value empty, so that the global value is used.
    எனில் (is_global_local_option(idx) && both)
    {
	free_string_option(*varp);
	*varp = empty_option;
    }
#ifdef FEAT_EVAL
    எனில் (set_sid != SID_NONE)
    {
	sctx_T script_ctx;

	எனில் (set_sid == 0)
	    script_ctx = current_sctx;
	இல்லையெனில்
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
    வெறுமை
set_string_option_direct_in_win(
	win_T		*wp,
	char_u		*name,
	முழு		opt_idx,
	char_u		*val,
	முழு		opt_flags,
	முழு		set_sid)
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

#எனில் defined(FEAT_PROP_POPUP)
/*
 * Like set_string_option_direct(), but for a buffer-local option in "buf".
 * Blocks autocommands to avoid the old curbuf becoming invalid.
 */
    வெறுமை
set_string_option_direct_in_buf(
	buf_T		*buf,
	char_u		*name,
	முழு		opt_idx,
	char_u		*val,
	முழு		opt_flags,
	முழு		set_sid)
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
    எழுத்து *
set_string_option(
    முழு		opt_idx,
    char_u	*மதிப்பு,
    முழு		opt_flags,	// OPT_LOCAL and/or OPT_GLOBAL
    எழுத்து	*errbuf,
    size_t	errbuflen)
{
    char_u	*s;
    char_u	**varp;
    char_u	*oldval;
#எனில் defined(FEAT_EVAL)
    char_u	*oldval_l = வெற்று;
    char_u	*oldval_g = வெற்று;
    char_u	*saved_oldval = வெற்று;
    char_u	*saved_oldval_l = வெற்று;
    char_u	*saved_oldval_g = வெற்று;
    char_u	*saved_newval = வெற்று;
#endif
    எழுத்து	*errmsg = வெற்று;
    முழு		மதிப்பு_checked = FALSE;

    எனில் (is_hidden_option(opt_idx))	// don't set hidden option
	திரும்பு வெற்று;

    s = vim_strsave(மதிப்பு == வெற்று ? (char_u *)"" : மதிப்பு);
    எனில் (s == வெற்று)
	திரும்பு வெற்று;

    varp = (char_u **)get_option_varp_scope(opt_idx,
	    (opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0
	    ? (is_global_local_option(opt_idx)
		? OPT_GLOBAL : OPT_LOCAL)
	    : opt_flags);
    oldval = *varp;
#எனில் defined(FEAT_EVAL)
    எனில் ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	oldval_l = *(char_u **)get_option_varp_scope(opt_idx, OPT_LOCAL);
	oldval_g = *(char_u **)get_option_varp_scope(opt_idx, OPT_GLOBAL);
    }
#endif
    *varp = s;

#எனில் defined(FEAT_EVAL)
    எனில் (!starting
# ifdef FEAT_CRYPT
	    && !is_crypt_சாவி_option(opt_idx)
# endif
       )
    {
	எனில் (oldval_l != வெற்று)
	    saved_oldval_l = vim_strsave(oldval_l);
	எனில் (oldval_g != வெற்று)
	    saved_oldval_g = vim_strsave(oldval_g);
	saved_oldval = vim_strsave(oldval);
	saved_newval = vim_strsave(s);
    }
#endif
    எனில் ((errmsg = did_set_string_option(opt_idx, varp, oldval, மதிப்பு, errbuf,
		    errbuflen, opt_flags, OP_NONE, &மதிப்பு_checked)) == வெற்று)
	did_set_option(opt_idx, opt_flags, TRUE, மதிப்பு_checked);

#எனில் defined(FEAT_EVAL)
    // call autocommand after handling side effects
    எனில் (errmsg == வெற்று)
	trigger_optionset_string(opt_idx, opt_flags,
		saved_oldval, saved_oldval_l,
		saved_oldval_g, saved_newval);
    vim_free(saved_oldval);
    vim_free(saved_oldval_l);
    vim_free(saved_oldval_g);
    vim_free(saved_newval);
#endif
    திரும்பு errmsg;
}

/*
 * Return TRUE if "val" is a valid 'filetype' name.
 * Also used for 'syntax' and 'keymap'.
 */
    நிலையான முழு
valid_filetype(char_u *val)
{
    திரும்பு valid_name(val, ".-_");
}

#ifdef FEAT_STL_OPT
/*
 * Check validity of options with the 'statusline' format.
 * Return an untranslated error message or NULL.
 */
    நிலையான எழுத்து *
check_stl_option(char_u *s)
{
    முழு		groupdepth = 0;
    நிலையான எழுத்து errbuf[ERR_BUFLEN];
    முழு		errbuflen = ERR_BUFLEN;

    போது (*s)
    {
	// Check for valid keys after % sequences
	போது (*s && *s != '%')
	    s++;
	எனில் (!*s)
	    நிறுத்து;
	s++;
	எனில் (*s == STL_CLICKFUNC)
	{
	    எனில் (s[1] == ']')
	    {
		// %[] - end click region
		s += 2;
		தொடர்;
	    }
	    எனில் (ASCII_ISALPHA(s[1]) || s[1] == '_')
	    {
		// %[FuncName] - start click region
		char_u *rb = vim_strchr(s + 2, ']');
		எனில் (rb != வெற்று)
		{
		    s = rb + 1;
		    தொடர்;
		}
	    }
	    // Bare %[ is invalid
	    திரும்பு illegal_char(errbuf, errbuflen, *s);
	}
	எனில் (*s == STL_LINEBREAK)
	{
	    // Plain %@ - line break
	    s++;
	    தொடர்;
	}
	எனில் (*s == '%' || *s == STL_TRUNCMARK || *s == STL_SEPARATE)
	{
	    s++;
	    தொடர்;
	}
	எனில் (*s == ')')
	{
	    s++;
	    எனில் (--groupdepth < 0)
		நிறுத்து;
	    தொடர்;
	}
	எனில் (*s == '-')
	    s++;
	போது (VIM_ISDIGIT(*s))
	    s++;
	எனில் (*s == STL_USER_HL)
	    தொடர்;
	எனில் (*s == STL_CLICKFUNC)
	{
	    // %N[FuncName] or %N[]
	    எனில் (s[1] == ']')
	    {
		s += 2;
		தொடர்;
	    }
	    எனில் (ASCII_ISALPHA(s[1]) || s[1] == '_')
	    {
		char_u *rb = vim_strchr(s + 2, ']');
		எனில் (rb != வெற்று)
		{
		    s = rb + 1;
		    தொடர்;
		}
	    }
	    // Bare %N[ is invalid
	    திரும்பு illegal_char(errbuf, errbuflen, *s);
	}
	எனில் (*s == STL_LINEBREAK)
	{
	    // %N@ - line break
	    s++;
	    தொடர்;
	}
	எனில் (*s == '.')
	{
	    s++;
	    போது (*s && VIM_ISDIGIT(*s))
		s++;
	}
	எனில் (*s == '(')
	{
	    groupdepth++;
	    தொடர்;
	}
	எனில் (vim_strchr(STL_ALL, *s) == வெற்று)
	{
	    திரும்பு illegal_char(errbuf, errbuflen, *s);
	}
	எனில் (*s == '{')
	{
	    முழு reevaluate = (*++s == '%');

	    எனில் (reevaluate && *++s == '}')
		// "}" is not allowed immediately after "%{%"
		திரும்பு illegal_char(errbuf, errbuflen, '}');
	    போது ((*s != '}' || (reevaluate && s[-1] != '%')) && *s)
		s++;
	    எனில் (*s != '}')
		திரும்பு e_unclosed_expression_sequence;
	}
    }
    எனில் (groupdepth != 0)
	திரும்பு e_unbalanced_groups;
    திரும்பு வெற்று;
}
#endif

/*
 * Check for a "normal" directory or file name in some options.  Disallow a
 * path separator (slash and/or backslash), wildcards and characters that are
 * often illegal in a file name. Be more permissive if "secure" is off.
 */
    நிலையான முழு
check_illegal_path_names(முழு opt_idx, char_u **varp)
{
    திரும்பு (((get_option_flags(opt_idx) & P_NFNAME)
		&& vim_strpbrk(*varp, (char_u *)(secure
			? "/\\*?[|;&<>\r\n" : "/\\*?[<>\r\n")) != வெற்று)
	    || ((get_option_flags(opt_idx) & P_NDNAME)
		&& vim_strpbrk(*varp, (char_u *)"*?[|;&<>\r\n") != வெற்று));
}

/*
 * An option that accepts a list of flags is changed.
 * e.g. 'viewoptions', 'switchbuf', 'casemap', etc.
 */
    நிலையான எழுத்து *
did_set_opt_flags(char_u *val, எழுத்து **மதிப்புகள், unsigned *flagp, முழு list)
{
    எனில் (opt_strings_flags(val, மதிப்புகள், flagp, list) == FAIL)
	திரும்பு e_invalid_argument;

    திரும்பு வெற்று;
}

/*
 * An option that accepts a list of string values is changed.
 * e.g. 'nrformats', 'scrollopt', 'wildoptions', etc.
 */
    நிலையான எழுத்து *
did_set_opt_strings(char_u *val, எழுத்து **மதிப்புகள், முழு list)
{
    திரும்பு did_set_opt_flags(val, மதிப்புகள், வெற்று, list);
}

/*
 * An option which is a list of flags is set.  Valid values are in 'flags'.
 */
    நிலையான எழுத்து *
did_set_option_listflag(
	char_u *val,
	char_u *flags,
	எழுத்து *errbuf,
	size_t errbuflen)
{
    char_u	*s;

    க்கு (s = val; *s; ++s)
	எனில் (vim_strchr(flags, *s) == வெற்று)
	    திரும்பு illegal_char(errbuf, errbuflen, *s);

    திரும்பு வெற்று;
}

/*
 * Expand an option that accepts a list of fixed string values with known
 * number of items.
 */
    நிலையான முழு
expand_set_opt_string(
	optexpand_T *args,
	எழுத்து **மதிப்புகள்,
	size_t இலக்கம்_மதிப்புகள்,
	முழு *இலக்கம்_Matches,
	char_u ***matches)
{
    char_u	*p;
    regmatch_T	*regmatch = args->oe_regmatch;
    முழு		include_orig_val = args->oe_include_orig_val;
    char_u	*option_val = args->oe_opt_மதிப்பு;

    // Assume numValues is small since they are fixed enums, so just allocate
    // upfront instead of needing two passes to calculate output size.
    *matches = ALLOC_MULT(char_u *, இலக்கம்_மதிப்புகள் + 1);
    எனில் (*matches == வெற்று)
	திரும்பு FAIL;

    முழு மொத்தம் = 0;

    எனில் (include_orig_val && *option_val != NUL)
    {
	p = vim_strsave(option_val);
	எனில் (p == வெற்று)
	{
	    VIM_CLEAR(*matches);
	    திரும்பு FAIL;
	}
	(*matches)[மொத்தம்++] = p;
    }

    க்கு (எழுத்து **val = மதிப்புகள்; *val != வெற்று; val++)
    {
	எனில் (include_orig_val && *option_val != NUL)
	{
	    எனில் (STRCMP((char_u*)*val, option_val) == 0)
		தொடர்;
	}
	எனில் (vim_regexec(regmatch, (char_u*)(*val), (colnr_T)0))
	{
	    p = vim_strsave((char_u*)*val);
	    எனில் (p == வெற்று)
	    {
		எனில் (மொத்தம் == 0)
		{
		    VIM_CLEAR(*matches);
		    திரும்பு FAIL;
		}
		இல்லையெனில்
		    நிறுத்து;
	    }
	    (*matches)[மொத்தம்++] = p;
	}
    }
    எனில் (மொத்தம் == 0)
    {
	VIM_CLEAR(*matches);
	திரும்பு FAIL;
    }
    *இலக்கம்_Matches = மொத்தம்;
    திரும்பு OK;
}

நிலையான char_u *set_opt_callback_orig_option = வெற்று;
நிலையான char_u *((*set_opt_callback_func)(expand_T *, முழு));

/*
 * Callback used by expand_set_opt_generic to also include the original value
 * as the first item.
 */
    நிலையான char_u *
expand_set_opt_generic_cb(expand_T *xp, முழு idx)
{
    எனில் (idx == 0)
    {
	எனில் (set_opt_callback_orig_option != வெற்று)
	    திரும்பு set_opt_callback_orig_option;
	இல்லையெனில்
	    திரும்பு (char_u *)""; // empty strings are ignored
    }
    திரும்பு set_opt_callback_func(xp, idx - 1);
}

/*
 * Expand an option with a callback that iterates through a list of possible
 * names using an index.
 */
    நிலையான முழு
expand_set_opt_generic(
	optexpand_T *args,
	char_u *((*func)(expand_T *, முழு)),
	முழு *இலக்கம்_Matches,
	char_u ***matches)
{
    முழு ret;

    set_opt_callback_orig_option = args->oe_include_orig_val ?
	args->oe_opt_மதிப்பு : வெற்று;
    set_opt_callback_func = func;

    ret = ExpandGeneric(
	    (char_u*)"", // not using fuzzy as currently EXPAND_STRING_SETTING doesn't use it
	    args->oe_xp,
	    args->oe_regmatch,
	    matches,
	    இலக்கம்_Matches,
	    expand_set_opt_generic_cb,
	    FALSE);

    set_opt_callback_orig_option = வெற்று;
    set_opt_callback_func = வெற்று;
    திரும்பு ret;
}

#எனில் defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
நிலையான garray_T *expand_cb_ga;
நிலையான optexpand_T *expand_cb_args;

/*
 * Callback provided to a function in expand_set_opt_callback. Will perform
 * regex matching against the value and add to the list.
 *
 * Returns OK usually. Returns FAIL if it failed to allocate memory, and the
 * caller should terminate the enumeration.
 */
    நிலையான முழு
expand_set_opt_callback_cb(char_u *val)
{
    regmatch_T	*regmatch = expand_cb_args->oe_regmatch;
    expand_T	*xp = expand_cb_args->oe_xp;
    garray_T	*ga = expand_cb_ga;
    char_u	*str;

    எனில் (val == வெற்று || *val == NUL)
	திரும்பு OK;

    எனில் (xp->xp_pattern[0] != NUL &&
	    !vim_regexec(regmatch, val, (colnr_T)0))
	திரும்பு OK;

    str = vim_strsave_escaped(val, (char_u *)" \t\\");

    எனில் (str == வெற்று)
	திரும்பு FAIL;

    எனில் (ga_grow(ga, 1) == FAIL)
    {
	vim_free(str);
	திரும்பு FAIL;
    }

    ((char_u **)ga->ga_data)[ga->ga_len] = str;
    ++ga->ga_len;
    திரும்பு OK;
}

/*
 * Expand an option with a provided function that takes a callback. The
 * function will enumerate through all options and call the callback to add it
 * to the list.
 *
 * "func" is the enumerator function that will generate the list of options.
 * "func_params" is a single parameter that will be passed to func.
 */
    நிலையான முழு
expand_set_opt_callback(
	optexpand_T *args,
	வெறுமை (*func)(optexpand_T *, வெறுமை* params, முழு (*cb)(char_u *val)),
	வெறுமை *func_params,
	முழு *இலக்கம்_Matches,
	char_u ***matches)
{
    garray_T	ga;
    முழு		include_orig_val = args->oe_include_orig_val;
    char_u	*option_val = args->oe_opt_மதிப்பு;

    ga_init2(&ga, அளவு(எழுத்து *), 30);

    எனில் (include_orig_val && *option_val != NUL)
    {
	char_u *p = vim_strsave(option_val);
	எனில் (p == வெற்று)
	    திரும்பு FAIL;
	எனில் (ga_grow(&ga, 1) == FAIL)
	{
	    vim_free(p);
	    திரும்பு FAIL;
	}
	((char_u **)ga.ga_data)[ga.ga_len] = p;
	++ga.ga_len;
    }

    expand_cb_ga = &ga;
    expand_cb_args = args;

    func(args, func_params, expand_set_opt_callback_cb);

    expand_cb_ga = வெற்று;
    expand_cb_args = வெற்று;

    *matches = ga.ga_data;
    *இலக்கம்_Matches = ga.ga_len;
    திரும்பு OK;
}
#endif

/*
 * Expand an option which is a list of flags.
 */
    நிலையான முழு
expand_set_opt_listflag(
	optexpand_T *args,
	char_u *flags,
	முழு *இலக்கம்_Matches,
	char_u ***matches)
{
    char_u  *p;
    char_u  *option_val = args->oe_opt_மதிப்பு;
    char_u  *cmdline_val = args->oe_set_arg;
    முழு	    append = args->oe_append;
    முழு	    include_orig_val = args->oe_include_orig_val && (*option_val != NUL);

    size_t இலக்கம்_flags = STRLEN(flags);

    // Assume we only have small number of flags, so just allocate max size.
    *matches = ALLOC_MULT(char_u *, இலக்கம்_flags + 1);
    எனில் (*matches == வெற்று)
	திரும்பு FAIL;

    முழு மொத்தம் = 0;

    எனில் (include_orig_val)
    {
	p = vim_strsave(option_val);
	எனில் (p == வெற்று)
	{
	    VIM_CLEAR(*matches);
	    திரும்பு FAIL;
	}
	(*matches)[மொத்தம்++] = p;
    }

    க்கு (char_u *flag = flags; *flag != NUL; flag++)
    {
	எனில் (append && vim_strchr(option_val, *flag) != வெற்று)
	    தொடர்;

	எனில் (vim_strchr(cmdline_val, *flag) == வெற்று)
	{
	    எனில் (include_orig_val
		    && option_val[1] == NUL
		    && *flag == option_val[0])
	    {
		// This value is already used as the first choice as it's the
		// existing flag. Just skip it to avoid duplicate.
		தொடர்;
	    }
	    p = vim_strnsave(flag, 1);
	    எனில் (p == வெற்று)
	    {
		எனில் (மொத்தம் == 0)
		{
		    VIM_CLEAR(*matches);
		    திரும்பு FAIL;
		}
		இல்லையெனில்
		    நிறுத்து;
	    }
	    (*matches)[மொத்தம்++] = p;
	}
    }

    எனில் (மொத்தம் == 0)
    {
	VIM_CLEAR(*matches);
	திரும்பு FAIL;
    }
    *இலக்கம்_Matches = மொத்தம்;
    திரும்பு OK;
}

/*
 * The 'ambiwidth' option is changed.
 */
    எழுத்து *
did_set_ambiwidth(optset_T *args UNUSED)
{
    எனில் (check_opt_strings(p_ambw, p_ambw_மதிப்புகள், FALSE) != OK)
	திரும்பு e_invalid_argument;

    திரும்பு check_chars_options();
}

// "name" must be a string literal, the length is computed at compile time.
#define completing_மதிப்பு_for_subopt(args, name) \
	  completing_மதிப்பு_for_subopt_len(args, name, (முழு)STRLEN_LITERAL(name))

/*
 * Return true when completing the value of the sub-option "name" with length
 * "len", e.g. the value after "close:" in 'completepopup'.
 */
    நிலையான bool
completing_மதிப்பு_for_subopt_len(optexpand_T *args, எழுத்து *name, முழு len)
{
    char_u  *colon = args->oe_xp->xp_pattern - 1;
    முழு	    off = (முழு)(colon - args->oe_set_arg);

    எனில் (off < len)
	திரும்பு false;
    // The name must follow a comma when it does not start the option value.
    எனில் (off > len && *(colon - len - 1) != ',')
	திரும்பு false;

    திரும்பு STRNCMP(colon - len, name, len) == 0;
}

    முழு
expand_set_ambiwidth(optexpand_T *args, முழு *இலக்கம்_Matches, char_u ***matches)
{
    திரும்பு expand_set_opt_string(
	    args,
	    p_ambw_மதிப்புகள்,
	    ARRAY_LENGTH(p_ambw_மதிப்புகள்) - 1,
	    இலக்கம்_Matches,
	    matches);
}

/*
 * The 'background' option is changed.
 */
    எழுத்து *
did_set_background(optset_T *args)
{
    எனில் (check_opt_strings(p_bg, p_bg_மதிப்புகள், FALSE) == FAIL)
	திரும்பு e_invalid_argument;

    எனில் (args->os_oldval.string != வெற்று && args->os_oldval.string[0] == *p_bg)
	// Value was not changed
	திரும்பு வெற்று;

#ifdef FEAT_EVAL
    முழு dark = (*p_bg == 'd');
#endif

    init_highlight(FALSE, FALSE);

#ifdef FEAT_EVAL
    எனில் (dark != (*p_bg == 'd')
	    && get_var_மதிப்பு((char_u *)"g:colors_name") != வெற்று)
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

    திரும்பு வெற்று;
}

    முழு
expand_set_background(optexpand_T *args, முழு *இலக்கம்_Matches, char_u ***matches)
{
    திரும்பு expand_set_opt_string(
	    args,
	    p_bg_மதிப்புகள்,
	    ARRAY_LENGTH(p_bg_மதிப்புகள்) - 1,
	    இலக்கம்_Matches,
	    matches);
}

/*
 * The 'backspace' option is changed.
 */
    எழுத்து *
did_set_backspace(optset_T *args UNUSED)
{
    எனில் (VIM_ISDIGIT(*p_bs))
    {
	எனில் (*p_bs > '3' || p_bs[1] != NUL)
	    திரும்பு e_invalid_argument;
    }
    இல்லையெனில் எனில் (check_opt_strings(p_bs, p_bs_மதிப்புகள், TRUE) != OK)
	திரும்பு e_invalid_argument;

    திரும்பு வெற்று;
}

    முழு
expand_set_backspace(optexpand_T *args, முழு *இலக்கம்_Matches, char_u ***matches)
{
    திரும்பு expand_set_opt_string(
	    args,
	    p_bs_மதிப்புகள்,
	    ARRAY_LENGTH(p_bs_மதிப்புகள்) - 1,
	    இலக்கம்_Matches,
	    matches);
}

/*
 * The 'backupcopy' option is changed.
 */
    எழுத்து *
did_set_backupcopy(optset_T *args)
{
    char_u		*bkc = p_bkc;
    unsigned முழு	*flags = &bkc_flags;
    எழுத்து		*errmsg = வெற்று;

    எனில் (args->os_flags & OPT_LOCAL)
    {
	bkc = curbuf->b_p_bkc;
	flags = &curbuf->b_bkc_flags;
    }
    இல்லையெனில் எனில் (!(args->os_flags & OPT_GLOBAL))
	// When using :set, clear the local flags.
	curbuf->b_bkc_flags = 0;

    எனில் ((args->os_flags & OPT_LOCAL) && *bkc == NUL)
	// make the local value empty: use the global value
	*flags = 0;
    இல்லையெனில்
    {
	எனில் (opt_strings_flags(bkc, p_bkc_மதிப்புகள், flags, TRUE) != OK)
	    errmsg = e_invalid_argument;
	எனில் ((((முழு)*flags & BKC_AUTO) != 0)
		+ (((முழு)*flags & BKC_YES) != 0)
		+ (((முழு)*flags & BKC_NO) != 0) != 1)
	{
	    // Must have exactly one of "auto", "yes"  and "no".
	    (வெறுமை)opt_strings_flags(args->os_oldval.string, p_bkc_மதிப்புகள்,
								  flags, TRUE);
	    errmsg = e_invalid_argument;
	}
    }

    திரும்பு errmsg;
}

    முழு
expand_set_backupcopy(optexpand_T *args, முழு *இலக்கம்_Matches, char_u ***matches)
{
    திரும்பு expand_set_opt_string(
	    args,
	    p_bkc_மதிப்புகள்,
	    ARRAY_LENGTH(p_bkc_மதிப்புகள்) - 1,
	    இலக்கம்_Matches,
	    matches);
}

/*
 * The 'backupext' or the 'patchmode' option is changed.
 */
    எழுத்து *
did_set_backupext_or_patchmode(optset_T *args UNUSED)
{
    எனில் (STRCMP(*p_bex == '.' ? p_bex + 1 : p_bex,
		*p_pm == '.' ? p_pm + 1 : p_pm) == 0)
	திரும்பு e_backupext_and_patchmode_are_equal;

    திரும்பு வெற்று;
}

/*
 * The 'belloff' option is changed.
 */
    எழுத்து *
did_set_belloff(optset_T *args UNUSED)
{
    திரும்பு did_set_opt_flags(p_bo, p_bo_மதிப்புகள், &bo_flags, TRUE);
}

    முழு
expand_set_belloff(optexpand_T *args, முழு *இலக்கம்_Matches, char_u ***matches)
{
    திரும்பு expand_set_opt_string(
	    args,
	    p_bo_மதிப்புகள்,
	    ARRAY_LENGTH(p_bo_மதிப்புகள்) - 1,
	    இலக்கம்_Matches,
	    matches);
}

#எனில் defined(FEAT_LINEBREAK)
/*
 * The 'breakat' option is changed.
 */
    எழுத்து *
did_set_breakat(optset_T *args UNUSED)
{
    char_u	*p;
    முழு		i;

    க்கு (i = 0; i < 256; i++)
	breakat_flags[i] = FALSE;

    எனில் (p_breakat != வெற்று)
	க்கு (p = p_breakat; *p; p++)
	    breakat_flags[*p] = TRUE;

    திரும்பு வெற்று;
}

/*
 * The 'breakindentopt' option is changed.
 */
    எழுத்து *
did_set_breakindentopt(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    எனில் (briopt_check(*varp, varp == &curwin->w_p_briopt ? curwin : வெற்று)
								      == FAIL)
	திரும்பு e_invalid_argument;

    // list setting requires a redraw
    எனில் (varp == &curwin->w_p_briopt && curwin->w_briopt_list)
	redraw_all_later(UPD_NOT_VALID);

    திரும்பு வெற்று;
}

    முழு
expand_set_breakindentopt(
    optexpand_T		*args,
    முழு			*இலக்கம்_Matches,
    char_u		***matches)
{
    திரும்பு expand_set_opt_string(
	    args,
	    p_briopt_மதிப்புகள்,
	    ARRAY_LENGTH(p_briopt_மதிப்புகள்) - 1,
	    இலக்கம்_Matches,
	    matches);
}
#endif

#எனில் defined(FEAT_BROWSE)
/*
 * The 'browsedir' option is changed.
 */
    எழுத்து *
did_set_browsedir(optset_T *args UNUSED)
{
    எனில் (check_opt_strings(p_bsdir, p_bsdir_மதிப்புகள், FALSE) != OK
	    && !mch_isdir(p_bsdir))
	திரும்பு e_invalid_argument;

    திரும்பு வெற்று;
}

    முழு
expand_set_browsedir(optexpand_T *args, முழு *இலக்கம்_Matches, char_u ***matches)
{
    திரும்பு expand_set_opt_string(
	    args,
	    p_bsdir_மதிப்புகள்,
	    ARRAY_LENGTH(p_bsdir_மதிப்புகள்) - 1,
	    இலக்கம்_Matches,
	    matches);
}
#endif

/*
 * The 'bufhidden' option is changed.
 */
    எழுத்து *
did_set_bufhidden(optset_T *args UNUSED)
{
    திரும்பு did_set_opt_strings(curbuf->b_p_bh, p_bufhidden_மதிப்புகள், FALSE);
}

    முழு
expand_set_bufhidden(optexpand_T *args, முழு *இலக்கம்_Matches, char_u ***matches)
{
    திரும்பு expand_set_opt_string(
	    args,
	    p_bufhidden_மதிப்புகள்,
	    ARRAY_LENGTH(p_bufhidden_மதிப்புகள்) - 1,
	    இலக்கம்_Matches,
	    matches);
}

/*
 * The 'buftype' option is changed.
 */
    எழுத்து *
did_set_buftype(optset_T *args UNUSED)
{
    எனில் (check_opt_strings(curbuf->b_p_bt, p_buftype_மதிப்புகள், FALSE) != OK)
	திரும்பு e_invalid_argument;

    எனில் (curwin->w_status_height)
    {
	curwin->w_redr_status = true;
	redraw_later(UPD_VALID);
    }
    curbuf->b_help = (curbuf->b_p_bt[0] == 'h');
    redraw_titles();

    திரும்பு வெற்று;
}

    முழு
expand_set_buftype(optexpand_T *args, முழு *இலக்கம்_Matches, char_u ***matches)
{
    திரும்பு expand_set_opt_string(
	    args,
	    p_buftype_மதிப்புகள்,
	    ARRAY_LENGTH(p_buftype_மதிப்புகள்) - 1,
	    இலக்கம்_Matches,
	    matches);
}

/*
 * The 'casemap' option is changed.
 */
    எழுத்து *
did_set_casemap(optset_T *args UNUSED)
{
    திரும்பு did_set_opt_flags(p_cmp, p_cmp_மதிப்புகள், &cmp_flags, TRUE);
}

    முழு
expand_set_casemap(optexpand_T *args, முழு *இலக்கம்_Matches, char_u ***matches)
{
    திரும்பு expand_set_opt_string(
	    args,
	    p_cmp_மதிப்புகள்,
	    ARRAY_LENGTH(p_cmp_மதிப்புகள்) - 1,
	    இலக்கம்_Matches,
	    matches);
}

#எனில் defined(HAVE_CLIPMETHOD)
    முழு
expand_set_clipboard(optexpand_T *args, முழு *இலக்கம்_Matches, char_u ***matches)
{
    திரும்பு expand_set_opt_string(
	    args,
	    p_cb_மதிப்புகள்,
	    ARRAY_LENGTH(p_cb_மதிப்புகள்) - 1,
	    இலக்கம்_Matches,
	    matches);
}
#endif

#ifdef HAVE_CLIPMETHOD
    எழுத்து *
did_set_clipmethod(optset_T *args UNUSED)
{
    திரும்பு choose_clipmethod();
}

    முழு
expand_set_clipmethod(optexpand_T *args, முழு *இலக்கம்_Matches, char_u ***matches)
{
    // We want to expand using the predefined clipmethod values + clipboard
    // provider names.
    முழு		result;
    எழுத்து	**மதிப்புகள்;
    முழு		மொத்தம், pos = 0, start = 0;
# ifdef FEAT_EVAL
    dict_T	*providers = get_vim_var_dict(VV_CLIPPROVIDERS);
# இல்லையெனில்
    dict_T	*providers = வெற்று;
# endif
    hashtab_T	*ht = providers == வெற்று ? வெற்று : &providers->dv_hashtab;

    மொத்தம் = (ht == வெற்று ? 0 : ht->ht_used);
# ifdef FEAT_WAYLAND_CLIPBOARD
    மொத்தம்++;
    start++;
# endif
# ifdef FEAT_XCLIPBOARD
    மொத்தம்++;
    start++;
# endif
    மதிப்புகள் = ALLOC_MULT(எழுத்து *, மொத்தம் + 1); // Add NULL terminator too

    எனில் (மதிப்புகள் == வெற்று)
	திரும்பு FAIL;

# ifdef FEAT_WAYLAND_CLIPBOARD
    மதிப்புகள்[pos++] = "wayland";
# endif
# ifdef FEAT_XCLIPBOARD
    மதிப்புகள்[pos++] = "x11";
# endif

    எனில் (ht != வெற்று)
	க்கு (long_u i = 0; i < ht->ht_mask + 1; i++)
	{
	    hashitem_T	*hi = ht->ht_array + i;

	    எனில் (!HASHITEM_EMPTY(hi))
		மதிப்புகள்[pos++] = (எழுத்து *)vim_strsave(hi->hi_சாவி);
	}
    மதிப்புகள்[pos++] = வெற்று;

    result = expand_set_opt_string(
	    args,
	    மதிப்புகள்,
	    மொத்தம்,
	    இலக்கம்_Matches,
	    matches);

    க்கு (முழு i = start; i < மொத்தம்; i++)
	vim_free(மதிப்புகள்[i]);
    vim_free(மதிப்புகள்);

    திரும்பு result;
}
#endif

/*
 * The global 'listchars' or 'fillchars' option is changed.
 */
    நிலையான எழுத்து *
did_set_global_listfillchars(char_u *val, முழு opt_lcs, முழு opt_flags,
						எழுத்து *errbuf, size_t errbuflen)
{
    எழுத்து	*errmsg = வெற்று;
    char_u	**local_ptr = opt_lcs ? &curwin->w_p_lcs : &curwin->w_p_fcs;

    // only apply the global value to "curwin" when it does not have a
    // local value
    எனில் (opt_lcs)
	errmsg = set_listchars_option(curwin, val,
		**local_ptr == NUL || !(opt_flags & OPT_GLOBAL),
							    errbuf, errbuflen);
    இல்லையெனில்
	errmsg = set_fillchars_option(curwin, val,
		**local_ptr == NUL || !(opt_flags & OPT_GLOBAL),
							    errbuf, errbuflen);
    எனில் (errmsg != வெற்று)
	திரும்பு errmsg;

    tabpage_T	*tp;
    win_T	*wp;

    // If the current window is set to use the global
    // 'listchars'/'fillchars' value, clear the window-local value.
    எனில் (!(opt_flags & OPT_GLOBAL))
	clear_string_option(local_ptr);
    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	// If the current window has a local value need to apply it
	// again, it was changed when setting the global value.
	// If no error was returned above, we don't expect an error
	// here, so ignore the return value.
	எனில் (opt_lcs)
	{
	    எனில் (*wp->w_p_lcs == NUL)
		(வெறுமை)set_listchars_option(wp, wp->w_p_lcs, TRUE, வெற்று, 0);
	}
	இல்லையெனில்
	{
	    எனில் (*wp->w_p_fcs == NUL)
		(வெறுமை)set_fillchars_option(wp, wp->w_p_fcs, TRUE, வெற்று, 0);
	}
    }

    redraw_all_later(UPD_NOT_VALID);

    திரும்பு வெற்று;
}

/*
 * The 'fillchars' option or the 'listchars' option is changed.
 */
    எழுத்து *
did_set_chars_option(optset_T *args)
{
    char_u **varp = (char_u **)args->os_varp;
    எழுத்து *errmsg = வெற்று;

    எனில் (   varp == &p_lcs		// global 'listchars'
	|| varp == &p_fcs)		// global 'fillchars'
	errmsg = did_set_global_listfillchars(*varp, varp == &p_lcs,
			  args->os_flags, args->os_errbuf, args->os_errbuflen);
    இல்லையெனில் எனில் (varp == &curwin->w_p_lcs)	// local 'listchars'
	errmsg = set_listchars_option(curwin, *varp, TRUE,
					  args->os_errbuf, args->os_errbuflen);
    இல்லையெனில் எனில் (varp == &curwin->w_p_fcs)	// local 'fillchars'
	errmsg = set_fillchars_option(curwin, *varp, TRUE,
					  args->os_errbuf, args->os_errbuflen);

    திரும்பு errmsg;
}

/*
 * Expand 'fillchars' or 'listchars' option value.
 */
    முழு
expand_set_chars_option(optexpand_T *args, முழு *இலக்கம்_Matches, char_u ***matches)
{
    char_u **varp = (char_u **)args->oe_varp;
    முழு is_lcs = (varp == &p_lcs || varp == &curwin->w_p_lcs);
    திரும்பு expand_set_opt_generic(
	    args,
	    is_lcs ? get_listchars_name : get_fillchars_name,
	    இலக்கம்_Matches,
	    matches);
}

/*
 * The 'cinoptions' option is changed.
 */
    எழுத்து *
did_set_cinoptions(optset_T *args UNUSED)
{
    // TODO: recognize errors
    parse_cino(curbuf);

    திரும்பு வெற்று;
}

#எனில் defined(FEAT_SYN_HL)
/*
 * The 'colorcolumn' option is changed.
 */
    எழுத்து *
did_set_colorcolumn(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    திரும்பு check_colorcolumn(*varp, varp == &curwin->w_p_cc ? curwin : வெற்று);
}
#endif

/*
 * The 'comments' option is changed.
 */
    எழுத்து *
did_set_comments(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u	*s;
    எழுத்து	*errmsg = வெற்று;

    க்கு (s = *varp; *s; )
    {
	போது (*s && *s != ':')
	{
	    எனில் (vim_strchr((char_u *)COM_ALL, *s) == வெற்று
		    && !VIM_ISDIGIT(*s) && *s != '-')
	    {
		errmsg = illegal_char(args->os_errbuf, args->os_errbuflen, *s);
		நிறுத்து;
	    }
	    ++s;
	}
	எனில் (*s++ == NUL)
	    errmsg = e_missing_colon;
	இல்லையெனில் எனில் (*s == ',' || *s == NUL)
	    errmsg = e_zero_length_string;
	எனில் (errmsg != வெற்று)
	    நிறுத்து;
	போது (*s && *s != ',')
	{
	    எனில் (*s == '\\' && s[1] != NUL)
		++s;
	    ++s;
	}
	s = skip_to_option_part(s);
    }

    திரும்பு errmsg;
}

/*
 * The 'commentstring' option is changed.
 */
    எழுத்து *
did_set_commentstring(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    எனில் (**varp != NUL && strstr((எழுத்து *)*varp, "%s") == வெற்று)
	திரும்பு e_commentstring_must_be_empty_or_contain_str;

    திரும்பு வெற்று;
}

/*
 * Check if value for 'complete' is valid when 'complete' option is changed.
 */
    எழுத்து *
did_set_complete(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u	*p, *t;
    char_u	buffer[LSIZE];
    char_u	*buf_ptr;
    char_u	char_before = NUL;
    முழு		escape;

    க்கு (p = *varp; *p; )
    {
	vim_memset(buffer, 0, LSIZE);
	buf_ptr = buffer;
	escape = 0;

	// Extract substring while handling escaped commas
	போது (*p && (*p != ',' || escape) && buf_ptr < (buffer + LSIZE - 1))
	{
	    எனில் (*p == '\\' && *(p + 1) == ',')
	    {
		escape = 1;  // Mark escape mode
		p++;         // Skip '\'
	    }
	    இல்லையெனில்
	    {
		escape = 0;
		*buf_ptr++ = *p;
	    }
	    p++;
	}
	*buf_ptr = NUL;

	எனில் (vim_strchr((char_u *)".wbuksid]tUFo", *buffer) == வெற்று)
	    திரும்பு illegal_char(args->os_errbuf, args->os_errbuflen, *buffer);

	எனில் (vim_strchr((char_u *)"ksF", *buffer) == வெற்று && *(buffer + 1) != NUL
		&& *(buffer + 1) != '^')
	    char_before = *buffer;
	இல்லையெனில்
	{
	    // Test for a number after '^'
	    எனில் ((t = vim_strchr(buffer, '^')) != வெற்று)
	    {
		*t++ = NUL;
		எனில் (!*t)
		    char_before = '^';
		இல்லையெனில்
		{
		    க்கு (; *t; t++)
		    {
			எனில் (!vim_isdigit(*t))
			{
			    char_before = '^';
			    நிறுத்து;
			}
		    }
		}
	    }
	}
	எனில் (char_before != NUL)
	    திரும்பு illegal_char_after_chr(args->os_errbuf, args->os_errbuflen,
		    char_before);
	// Skip comma and spaces
	போது (*p == ',' || *p == ' ')
	    p++;
    }

#ifdef FEAT_COMPL_FUNC
    எனில் (set_cpt_callbacks(args) != OK)
	திரும்பு illegal_char_after_chr(args->os_errbuf, args->os_errbuflen,
		'F');
#endif
    திரும்பு வெற்று;
}

    முழு
expand_set_complete(optexpand_T *args, முழு *இலக்கம்_Matches, char_u ***matches)
{
    நிலையான எழுத்து *(p_cpt_மதிப்புகள்[]) = {
	".", "w", "b", "u", "k", "kspell", "s", "i", "d", "]", "t", "U", "F", "o",
	வெற்று};
    திரும்பு expand_set_opt_string(
	    args,
	    p_cpt_மதிப்புகள்,
	    ARRAY_LENGTH(p_cpt_மதிப்புகள்) - 1,
	    இலக்கம்_Matches,
	    matches);
}

/*
 * The 'completeopt' option is changed.
 */
    எழுத்து *
did_set_completeopt(optset_T *args UNUSED)
{
    char_u	*cot = p_cot;
    unsigned	*flags = &cot_flags;

    எனில் (args->os_flags & OPT_LOCAL)
    {
	cot = curbuf->b_p_cot;
	flags = &curbuf->b_cot_flags;
    }
    இல்லையெனில் எனில் (!(args->os_flags & OPT_GLOBAL))
	// When using :set, clear the local flags.
	curbuf->b_cot_flags = 0;

    எனில் (opt_strings_flags(cot, p_cot_மதிப்புகள், flags, TRUE) != OK)
	திரும்பு e_invalid_argument;

    திரும்பு வெற்று;
}

    முழு
expand_set_completeopt(optexpand_T *args, முழு *இலக்கம்_Matches, char_u ***matches)
{
    திரும்பு expand_set_opt_string(
	    args,
	    p_cot_மதிப்புகள்,
	    ARRAY_LENGTH(p_cot_மதிப்புகள்) - 1,
	    இலக்கம்_Matches,
	    matches);
}

/*
 * The 'completefuzzycollect' option is changed.
 */
    எழுத்து *
did_set_completefuzzycollect(optset_T *args UNUSED)
{
    எனில் (opt_strings_flags(p_cfc, p_cfc_மதிப்புகள், &cfc_flags, TRUE) != OK)
	திரும்பு e_invalid_argument;
    திரும்பு வெற்று;
}

    முழு
expand_set_completefuzzycollect(
	optexpand_T *args,
	முழு *இலக்கம்_Matches,
	char_u ***matches)
{
    திரும்பு expand_set_opt_string(
	    args,
	    p_cfc_மதிப்புகள்,
	    ARRAY_LENGTH(p_cfc_மதிப்புகள்) - 1,
	    இலக்கம்_Matches,
	    matches);
}

/*
 * The 'completeitemalign' option is changed.
 */
    எழுத்து *
did_set_completeitemalign(optset_T *args UNUSED)
{
    char_u	*p = p_cia;
    unsigned	new_cia_flags = 0;
    முழு		seen[3] = { FALSE, FALSE, FALSE };
    முழு		மொத்தம் = 0;
    char_u	buf[10];

    போது (*p)
    {
	copy_option_part(&p, buf, அளவு(buf), ",");
	எனில் (மொத்தம் >= 3)
	    திரும்பு e_invalid_argument;

	எனில் (STRCMP(buf, "abbr") == 0)
	{
	    எனில் (seen[CPT_ABBR])
		திரும்பு e_invalid_argument;
	    new_cia_flags = new_cia_flags * 10 + CPT_ABBR;
	    seen[CPT_ABBR] = TRUE;
	    மொத்தம்++;
	}
	இல்லையெனில் எனில் (STRCMP(buf, "kind") == 0)
	{
	    எனில் (seen[CPT_KIND])
		திரும்பு e_invalid_argument;
	    new_cia_flags = new_cia_flags * 10 + CPT_KIND;
	    seen[CPT_KIND] = TRUE;
	    மொத்தம்++;
	}
	இல்லையெனில் எனில் (STRCMP(buf, "menu") == 0)
	{
	    எனில் (seen[CPT_MENU])
		திரும்பு e_invalid_argument;
	    new_cia_flags = new_cia_flags * 10 + CPT_MENU;
	    seen[CPT_MENU] = TRUE;
	    மொத்தம்++;
	}
	இல்லையெனில்
	    திரும்பு e_invalid_argument;
    }
    எனில் (new_cia_flags == 0 || மொத்தம் != 3)
	திரும்பு e_invalid_argument;

    cia_flags = new_cia_flags;
    திரும்பு வெற்று;
}

#எனில் defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
/*
 * The 'completepopup' option is changed.
 */
    எழுத்து *
did_set_completepopup(optset_T *args UNUSED)
{
    எனில் (parse_completepopup(வெற்று) == FAIL)
	திரும்பு e_invalid_argument;

    popup_close_info();
    திரும்பு வெற்று;
}
#endif

#எனில் defined(BACKSLASH_IN_FILENAME)
/*
 * The 'completeslash' option is changed.
 */
    எழுத்து *
did_set_completeslash(optset_T *args UNUSED)
{
    எனில் (check_opt_strings(p_csl, p_csl_மதிப்புகள், FALSE) != OK
	    || check_opt_strings(curbuf->b_p_csl, p_csl_மதிப்புகள், FALSE) != OK)
	திரும்பு e_invalid_argument;

    திரும்பு வெற்று;
}

    முழு
expand_set_completeslash(optexpand_T *args, முழு *இலக்கம்_Matches, char_u ***matches)
{
    திரும்பு expand_set_opt_string(
	    args,
	    p_csl_மதிப்புகள்,
	    ARRAY_LENGTH(p_csl_மதிப்புகள்) - 1,
	    இலக்கம்_Matches,
	    matches);
}
#endif

#எனில் defined(FEAT_CONCEAL)
/*
 * The 'concealcursor' option is changed.
 */
    எழுத்து *
did_set_concealcursor(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    திரும்பு did_set_option_listflag(*varp, (char_u *)COCU_ALL, args->os_errbuf,
		    args->os_errbuflen);
}

    முழு
expand_set_concealcursor(optexpand_T *args, முழு *இலக்கம்_Matches, char_u ***matches)
{
    திரும்பு expand_set_opt_listflag(args, (char_u*)COCU_ALL, இலக்கம்_Matches, matches);
}
#endif

/*
 * The 'cpoptions' option is changed.
 */
    எழுத்து *
did_set_cpoptions(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    திரும்பு did_set_option_listflag(*varp, (char_u *)CPO_ALL, args->os_errbuf,
		    args->os_errbuflen);
}

    முழு
expand_set_cpoptions(optexpand_T *args, முழு *இலக்கம்_Matches, char_u ***matches)
{
    திரும்பு expand_set_opt_listflag(args, (char_u*)CPO_ALL, இலக்கம்_Matches, matches);
}

#எனில் defined(FEAT_CRYPT)
/*
 * The 'cryptkey' option is changed.
 */
    எழுத்து *
did_set_cryptkey(optset_T *args)
{
    // Make sure the ":set" command doesn't show the new value in the
    // history.
    remove_சாவி_from_history();

    எனில் (args->os_op != OP_NONE)
	// Don't allow set+=/-=/^= as they can allow for substring guessing
	திரும்பு e_invalid_argument;

    எனில் (STRCMP(curbuf->b_p_சாவி, args->os_oldval.string) != 0)
    {
	// Need to update the swapfile.
	ml_set_crypt_சாவி(curbuf, args->os_oldval.string,
		*curbuf->b_p_cm == NUL ? p_cm : curbuf->b_p_cm);
	changed_internal();
    }
# ifdef FEAT_SODIUM
    எனில் (crypt_method_is_sodium(crypt_get_method_nr(curbuf)))
	crypt_sodium_lock_சாவி(args->os_newval.string);
# endif

    திரும்பு வெற்று;
}

/*
 * The 'cryptmethod' option is changed.
 */
    எழுத்து *
did_set_cryptmethod(optset_T *args)
{
    char_u  *p;
    char_u  *s;

    எனில் (args->os_flags & OPT_LOCAL)
	p = curbuf->b_p_cm;
    இல்லையெனில்
	p = p_cm;
    எனில் (check_opt_strings(p, p_cm_மதிப்புகள், TRUE) != OK)
	திரும்பு e_invalid_argument;
    இல்லையெனில் எனில் (crypt_self_test() == FAIL)
	திரும்பு e_invalid_argument;

    // When setting the global value to empty, make it "zip".
    எனில் (*p_cm == NUL)
    {
	free_string_option(p_cm);
	p_cm = vim_strsave((char_u *)"zip");
    }
    // When using ":set cm=name" the local value is going to be empty.
    // Do that here, otherwise the crypt functions will still use the
    // local value.
    எனில் ((args->os_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	free_string_option(curbuf->b_p_cm);
	curbuf->b_p_cm = empty_option;
    }

    // Need to update the swapfile when the effective method changed.
    // Set "s" to the effective old value, "p" to the effective new
    // method and compare.
    எனில் ((args->os_flags & OPT_LOCAL) && *args->os_oldval.string == NUL)
	s = p_cm;  // was previously using the global value
    இல்லையெனில்
	s = args->os_oldval.string;
    எனில் (*curbuf->b_p_cm == NUL)
	p = p_cm;  // is now using the global value
    இல்லையெனில்
	p = curbuf->b_p_cm;
    எனில் (STRCMP(s, p) != 0)
	ml_set_crypt_சாவி(curbuf, curbuf->b_p_சாவி, s);

    // If the global value changes need to update the swapfile for all
    // buffers using that value.
    எனில் ((args->os_flags & OPT_GLOBAL)
	    && STRCMP(p_cm, args->os_oldval.string) != 0)
    {
	buf_T	*buf;

	FOR_ALL_BUFFERS(buf)
	    எனில் (buf != curbuf && *buf->b_p_cm == NUL)
		ml_set_crypt_சாவி(buf, buf->b_p_சாவி, args->os_oldval.string);
    }
    திரும்பு வெற்று;
}

    முழு
expand_set_cryptmethod(optexpand_T *args, முழு *இலக்கம்_Matches, char_u ***matches)
{
    திரும்பு expand_set_opt_string(
	    args,
	    p_cm_மதிப்புகள்,
	    ARRAY_LENGTH(p_cm_மதிப்புகள்) - 1,
	    இலக்கம்_Matches,
	    matches);
}
#endif

#எனில் defined(FEAT_CSCOPE) && defined(FEAT_QUICKFIX)
/*
 * The 'cscopequickfix' option is changed.
 */
    எழுத்து *
did_set_cscopequickfix(optset_T *args UNUSED)
{
    char_u	*p;

    எனில் (p_csqf == வெற்று)
	திரும்பு வெற்று;

    p = p_csqf;
    போது (*p != NUL)
    {
	எனில் (vim_strchr((char_u *)CSQF_CMDS, *p) == வெற்று
		|| p[1] == NUL
		|| vim_strchr((char_u *)CSQF_FLAGS, p[1]) == வெற்று
		|| (p[2] != NUL && p[2] != ','))
	    திரும்பு e_invalid_argument;
	இல்லையெனில் எனில் (p[2] == NUL)
	    நிறுத்து;
	இல்லையெனில்
	    p += 3;
    }

    திரும்பு வெற்று;
}
#endif

#எனில் defined(FEAT_SYN_HL)
/*
 * The 'cursorlineopt' option is changed.
 */
    எழுத்து *
did_set_cursorlineopt(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    // This could be changed to use opt_strings_flags() instead.
    எனில் (**varp == NUL || fill_culopt_flags(*varp, curwin) != OK)
	திரும்பு e_invalid_argument;

    திரும்பு வெற்று;
}

    முழு
expand_set_cursorlineopt(optexpand_T *args, முழு *இலக்கம்_Matches, char_u ***matches)
{
    திரும்பு expand_set_opt_string(
	    args,
	    p_culopt_மதிப்புகள்,
	    ARRAY_LENGTH(p_culopt_மதிப்புகள்) - 1,
	    இலக்கம்_Matches,
	    matches);
}
#endif

/*
 * The 'debug' option is changed.
 */
    எழுத்து *
did_set_debug(optset_T *args UNUSED)
{
    திரும்பு did_set_opt_strings(p_debug, p_debug_மதிப்புகள், TRUE);
}

    முழு
expand_set_debug(optexpand_T *args, முழு *இலக்கம்_Matches, char_u ***matches)
{
    திரும்பு expand_set_opt_string(
	    args,
	    p_debug_மதிப்புகள்,
	    ARRAY_LENGTH(p_debug_மதிப்புகள்) - 1,
	    இலக்கம்_Matches,
	    matches);
}

#எனில் defined(FEAT_DIFF)
/*
 * The 'diffanchors' option is changed.
 */
    எழுத்து *
did_set_diffanchors(optset_T *args)
{
    எனில் (diffanchors_changed(args->os_flags & OPT_LOCAL) == FAIL)
	திரும்பு e_invalid_argument;

    திரும்பு வெற்று;
}

/*
 * The 'diffopt' option is changed.
 */
    எழுத்து *
did_set_diffopt(optset_T *args UNUSED)
{
    எனில் (diffopt_changed() == FAIL)
	திரும்பு e_invalid_argument;

    திரும்பு வெற்று;
}

    முழு
expand_set_diffopt(optexpand_T *args, முழு *இலக்கம்_Matches, char_u ***matches)
{
    expand_T *xp = args->oe_xp;

    எனில் (xp->xp_pattern > args->oe_set_arg && *(xp->xp_pattern-1) == ':')
    {
	எனில் (completing_மதிப்பு_for_subopt(args, "algorithm"))
	{
	    திரும்பு expand_set_opt_string(
		    args,
		    p_dip_algorithm_மதிப்புகள்,
		    ARRAY_LENGTH(p_dip_algorithm_மதிப்புகள்) - 1,
		    இலக்கம்_Matches,
		    matches);
	}
	எனில் (completing_மதிப்பு_for_subopt(args, "inline"))
	{
	    திரும்பு expand_set_opt_string(
		    args,
		    p_dip_inline_மதிப்புகள்,
		    ARRAY_LENGTH(p_dip_inline_மதிப்புகள்) - 1,
		    இலக்கம்_Matches,
		    matches);
	}
	திரும்பு FAIL;
    }

    திரும்பு expand_set_opt_string(
	    args,
	    p_dip_மதிப்புகள்,
	    ARRAY_LENGTH(p_dip_மதிப்புகள்) - 1,
	    இலக்கம்_Matches,
	    matches);
}
#endif

/*
 * The 'display' option is changed.
 */
    எழுத்து *
did_set_display(optset_T *args UNUSED)
{
    எனில் (opt_strings_flags(p_dy, p_dy_மதிப்புகள், &dy_flags, TRUE) != OK)
	திரும்பு e_invalid_argument;

    (வெறுமை)init_chartab();
    திரும்பு வெற்று;
}

    முழு
expand_set_display(optexpand_T *args, முழு *இலக்கம்_Matches, char_u ***matches)
{
    திரும்பு expand_set_opt_string(
	    args,
	    p_dy_மதிப்புகள்,
	    ARRAY_LENGTH(p_dy_மதிப்புகள்) - 1,
	    இலக்கம்_Matches,
	    matches);
}

/*
 * The 'eadirection' option is changed.
 */
    எழுத்து *
did_set_eadirection(optset_T *args UNUSED)
{
    திரும்பு did_set_opt_strings(p_ead, p_ead_மதிப்புகள், FALSE);
}

    முழு
expand_set_eadirection(optexpand_T *args, முழு *இலக்கம்_Matches, char_u ***matches)
{
    திரும்பு expand_set_opt_string(
	    args,
	    p_ead_மதிப்புகள்,
	    ARRAY_LENGTH(p_ead_மதிப்புகள்) - 1,
	    இலக்கம்_Matches,
	    matches);
}

/*
 * One of the 'encoding', 'fileencoding', 'termencoding' or 'makeencoding'
 * options is changed.
 */
    எழுத்து *
did_set_encoding(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u	**gvarp;
    எழுத்து	*errmsg = வெற்று;
    char_u	*p;

    // Get the global option to compare with, otherwise we would have to check
    // two values for all local options.
    gvarp = (char_u **)get_option_varp_scope(args->os_idx, OPT_GLOBAL);

    எனில் (gvarp == &p_fenc)
    {
	எனில் (!curbuf->b_p_ma && args->os_flags != OPT_GLOBAL)
	    errmsg = e_cannot_make_changes_modifiable_is_off;
	இல்லையெனில் எனில் (vim_strchr(*varp, ',') != வெற்று)
	    // No comma allowed in 'fileencoding'; catches confusing it
	    // with 'fileencodings'.
	    errmsg = e_invalid_argument;
	இல்லையெனில்
	{
	    // May show a "+" in the title now.
	    redraw_titles();
	    // Add 'fileencoding' to the swap file.
	    ml_setflags(curbuf);
	}
    }
    எனில் (errmsg == வெற்று)
    {
	// canonize the value, so that STRCMP() can be used on it
	p = enc_canonize(*varp);
	எனில் (p != வெற்று)
	{
	    vim_free(*varp);
	    *varp = p;
	}
	எனில் (varp == &p_enc)
	{
	    errmsg = mb_init();
	    redraw_titles();
	}
    }

#எனில் defined(FEAT_GUI_GTK)
    எனில் (errmsg == வெற்று && varp == &p_tenc && gui.in_use)
    {
	// GTK uses only a single encoding, and that is UTF-8.
	எனில் (STRCMP(p_tenc, "utf-8") != 0)
	    errmsg = e_cannot_be_changed_in_gtk_GUI;
    }
#endif

    எனில் (errmsg == வெற்று)
    {
#ifdef FEAT_KEYMAP
	// When 'keymap' is used and 'encoding' changes, reload the keymap
	// (with another encoding).
	எனில் (varp == &p_enc && *curbuf->b_p_keymap != NUL)
	    (வெறுமை)keymap_init();
#endif

	// When 'termencoding' is not empty and 'encoding' changes or when
	// 'termencoding' changes, need to setup for keyboard input and
	// display output conversion.
	எனில் (((varp == &p_enc && *p_tenc != NUL) || varp == &p_tenc))
	{
	    எனில் (convert_setup(&உள்ளீடு_conv, p_tenc, p_enc) == FAIL
		    || convert_setup(&output_conv, p_enc, p_tenc) == FAIL)
	    {
		semsg(_(e_cannot_convert_between_str_and_str),
			p_tenc, p_enc);
		errmsg = e_invalid_argument;
	    }
	}

#எனில் defined(MSWIN)
	// $HOME, $VIM and $VIMRUNTIME may have characters in active code page.
	எனில் (varp == &p_enc)
	{
	    init_homedir();
	    init_vimdir();
	}
#endif
    }

    திரும்பு errmsg;
}

    முழு
expand_set_encoding(optexpand_T *args, முழு *இலக்கம்_Matches, char_u ***matches)
{
    திரும்பு expand_set_opt_generic(
	    args,
	    get_encoding_name,
	    இலக்கம்_Matches,
	    matches);
}

/*
 * The 'eventignore(win)' option is changed.
 */
    எழுத்து *
did_set_eventignore(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u	*oldval = args->os_oldval.string;

    எனில் (check_ei(*varp) == FAIL)
	திரும்பு e_invalid_argument;

    எனில் (oldval == வெற்று || STRCMP(oldval, *varp) == 0)
	திரும்பு வெற்று;

    // Deal with the events that are triggered by comparing against a stored
    // state, with the old value in effect: what happened while an event was
    // ignored must not be reported once it is not ignored anymore, and what
    // happened before must still be reported.
    // Use a copy, the caller owns "oldval" and autocommands may free it.
    char_u	*save_ei = vim_strsave(oldval);
    எனில் (save_ei != வெற்று)
    {
	char_u	*newval = *varp;
	win_T	*wp = is_window_local_option(args->os_idx) ? curwin : வெற்று;

	*varp = save_ei;
	// "varp" points into "wp" for 'eventignorewin'.
	எனில் (wp != வெற்று)
	    ++wp->w_locked;
	may_trigger_deferred_events();
	எனில் (wp != வெற்று)
	    --wp->w_locked;

	free_string_option(*varp);
	*varp = newval;
    }

    திரும்பு வெற்று;
}

நிலையான முழு expand_eiw = FALSE;

    நிலையான char_u *
get_eventignore_name(expand_T *xp, முழு idx)
{
    முழு subtract = *xp->xp_pattern == '-';
    // 'eventignore(win)' allows special keyword "all" in addition to
    // all event names.
    எனில் (!subtract && idx == 0)
	திரும்பு (char_u *)"all";

    char_u *name = get_event_name_no_group(xp, idx - 1 + subtract, expand_eiw);
    எனில் (name == வெற்று)
	திரும்பு வெற்று;

    sprintf((எழுத்து *)IObuff, "%s%s", subtract ? "-" : "", name);
    திரும்பு IObuff;
}

    முழு
expand_set_eventignore(optexpand_T *args, முழு *இலக்கம்_Matches, char_u ***matches)
{
    expand_eiw = args->oe_varp != (char_u *)&p_ei;
    திரும்பு expand_set_opt_generic(
	    args,
	    get_eventignore_name,
	    இலக்கம்_Matches,
	    matches);
}

/*
 * The 'fileformat' option is changed.
 */
    எழுத்து *
did_set_fileformat(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    எனில் (!curbuf->b_p_ma && !(args->os_flags & OPT_GLOBAL))
	திரும்பு e_cannot_make_changes_modifiable_is_off;
    இல்லையெனில் எனில் (check_opt_strings(*varp, p_ff_மதிப்புகள், FALSE) != OK)
	திரும்பு e_invalid_argument;

    // may also change 'textmode'
    எனில் (get_fileformat(curbuf) == EOL_DOS)
	curbuf->b_p_tx = TRUE;
    இல்லையெனில்
	curbuf->b_p_tx = FALSE;
    redraw_titles();
    // update flag in swap file
    ml_setflags(curbuf);
    // Redraw needed when switching to/from "mac": a CR in the text
    // will be displayed differently.
    எனில் (get_fileformat(curbuf) == EOL_MAC || *args->os_oldval.string == 'm')
	redraw_curbuf_later(UPD_NOT_VALID);

    திரும்பு வெற்று;
}

    முழு
expand_set_fileformat(optexpand_T *args, முழு *இலக்கம்_Matches, char_u ***matches)
{
    திரும்பு expand_set_opt_string(
	    args,
	    p_ff_மதிப்புகள்,
	    ARRAY_LENGTH(p_ff_மதிப்புகள்) - 1,
	    இலக்கம்_Matches,
	    matches);
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * fileformat options.
 */
    char_u *
get_fileformat_name(expand_T *xp UNUSED, முழு idx)
{
    எனில் (idx >= (முழு)ARRAY_LENGTH(p_ff_மதிப்புகள்))
	திரும்பு வெற்று;

    திரும்பு (char_u*)p_ff_மதிப்புகள்[idx];
}

/*
 * The 'fileformats' option is changed.
 */
    எழுத்து *
did_set_fileformats(optset_T *args UNUSED)
{
    எனில் (check_opt_strings(p_ffs, p_ff_மதிப்புகள், TRUE) != OK)
	திரும்பு e_invalid_argument;

    // also change 'textauto'
    எனில் (*p_ffs == NUL)
	p_ta = FALSE;
    இல்லையெனில்
	p_ta = TRUE;

    திரும்பு வெற்று;
}

/*
 * The 'filetype' or the 'syntax' option is changed.
 */
    எழுத்து *
did_set_filetype_or_syntax(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    எனில் (!valid_filetype(*varp))
	திரும்பு e_invalid_argument;

    args->os_மதிப்பு_changed = STRCMP(args->os_oldval.string, *varp) != 0;

    // Since we check the value, there is no need to set P_INSECURE,
    // even when the value comes from a modeline.
    args->os_மதிப்பு_checked = TRUE;

    திரும்பு வெற்று;
}

#எனில் defined(FEAT_FOLDING)
/*
 * The 'foldclose' option is changed.
 */
    எழுத்து *
did_set_foldclose(optset_T *args UNUSED)
{
    திரும்பு did_set_opt_strings(p_fcl, p_fcl_மதிப்புகள், TRUE);
}

    முழு
expand_set_foldclose(optexpand_T *args, முழு *இலக்கம்_Matches, char_u ***matches)
{
    திரும்பு expand_set_opt_string(
	    args,
	    p_fcl_மதிப்புகள்,
	    ARRAY_LENGTH(p_fcl_மதிப்புகள்) - 1,
	    இலக்கம்_Matches,
	    matches);
}
#endif

#எனில் defined(FEAT_EVAL) && defined(FEAT_FOLDING)
/*
 * The 'foldexpr' option is changed.
 */
    எழுத்து *
did_set_foldexpr(optset_T *args)
{
    (வெறுமை)did_set_optexpr(args);
    எனில் (foldmethod_Is_கோவை(curwin))
	foldUpdateAll(curwin);
    திரும்பு வெற்று;
}
#endif

#எனில் defined(FEAT_FOLDING)
/*
 * The 'foldignore' option is changed.
 */
    எழுத்து *
did_set_foldignore(optset_T *args UNUSED)
{
    எனில் (foldmethodIsIndent(curwin))
	foldUpdateAll(curwin);
    திரும்பு வெற்று;
}

/*
 * The 'foldmarker' option is changed.
 */
    எழுத்து *
did_set_foldmarker(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u	*p;

    p = vim_strchr(*varp, ',');
    எனில் (p == வெற்று)
	திரும்பு e_comma_required;
    இல்லையெனில் எனில் (p == *varp || p[1] == NUL)
	திரும்பு e_invalid_argument;
    இல்லையெனில் எனில் (foldmethodIsMarker(curwin))
	foldUpdateAll(curwin);

    திரும்பு வெற்று;
}

/*
 * The 'foldmethod' option is changed.
 */
    எழுத்து *
did_set_foldmethod(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    எனில் (check_opt_strings(*varp, p_fdm_மதிப்புகள், FALSE) != OK || **varp == NUL)
	திரும்பு e_invalid_argument;

    foldUpdateAll(curwin);
    எனில் (foldmethodIsDiff(curwin))
	newFoldLevel();
    திரும்பு வெற்று;
}

    முழு
expand_set_foldmethod(optexpand_T *args, முழு *இலக்கம்_Matches, char_u ***matches)
{
    திரும்பு expand_set_opt_string(
	    args,
	    p_fdm_மதிப்புகள்,
	    ARRAY_LENGTH(p_fdm_மதிப்புகள்) - 1,
	    இலக்கம்_Matches,
	    matches);
}

/*
 * The 'foldopen' option is changed.
 */
    எழுத்து *
did_set_foldopen(optset_T *args UNUSED)
{
    திரும்பு did_set_opt_flags(p_fdo, p_fdo_மதிப்புகள், &fdo_flags, TRUE);
}

    முழு
expand_set_foldopen(optexpand_T *args, முழு *இலக்கம்_Matches, char_u ***matches)
{
    திரும்பு expand_set_opt_string(
	    args,
	    p_fdo_மதிப்புகள்,
	    ARRAY_LENGTH(p_fdo_மதிப்புகள்) - 1,
	    இலக்கம்_Matches,
	    matches);
}
#endif

/*
 * The 'formatoptions' option is changed.
 */
    எழுத்து *
did_set_formatoptions(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    திரும்பு did_set_option_listflag(*varp, (char_u *)FO_ALL, args->os_errbuf,
		    args->os_errbuflen);
}

    முழு
expand_set_formatoptions(optexpand_T *args, முழு *இலக்கம்_Matches, char_u ***matches)
{
    திரும்பு expand_set_opt_listflag(args, (char_u*)FO_ALL, இலக்கம்_Matches, matches);
}

#எனில் defined(CURSOR_SHAPE)
/*
 * The 'guicursor' option is changed.
 */
    எழுத்து *
did_set_guicursor(optset_T *args UNUSED)
{
    திரும்பு parse_shape_opt(SHAPE_CURSOR);
}
#endif

#எனில் defined(FEAT_GUI)
/*
 * The 'guifont' option is changed.
 */
    எழுத்து *
did_set_guifont(optset_T *args UNUSED)
{
    char_u	*p;
    எழுத்து	*errmsg = வெற்று;

    எனில் (gui.in_use)
    {
	p = p_guifont;
# எனில் defined(FEAT_GUI_GTK)
	// Put up a font dialog and let the user select a new value.
	// If this is cancelled go back to the old value but don't
	// give an error message.
	எனில் (STRCMP(p, "*") == 0)
	{
	    p = gui_mch_font_dialog(args->os_oldval.string);
	    free_string_option(p_guifont);
	    p_guifont = (p != வெற்று) ? p : vim_strsave(args->os_oldval.string);
	}
# endif
	எனில் (p != வெற்று && gui_init_font(p_guifont, FALSE) != OK)
	{
# எனில் defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_PHOTON)
	    எனில் (STRCMP(p_guifont, "*") == 0)
	    {
		// Dialog was cancelled: Keep the old value without giving
		// an error message.
		free_string_option(p_guifont);
		p_guifont = vim_strsave(args->os_oldval.string);
	    }
	    இல்லையெனில்
# endif
		errmsg = e_invalid_fonts;
	}
    }

    திரும்பு errmsg;
}

/*
 * Expand the 'guifont' option. Only when GUI is being used. Each platform has
 * specific behaviors.
 */
    முழு
expand_set_guifont(optexpand_T *args, முழு *இலக்கம்_Matches, char_u ***matches)
{
    எனில் (!gui.in_use)
	திரும்பு FAIL;

# எனில் defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
    char_u **varp = (char_u **)args->oe_varp;
    முழு wide = (varp == &p_guifontwide);

    திரும்பு expand_set_opt_callback(
	    args, gui_mch_expand_font, &wide, இலக்கம்_Matches, matches);
# இல்லையெனில்
    திரும்பு FAIL;
# endif
}

# எனில் defined(FEAT_XFONTSET)
/*
 * The 'guifontset' option is changed.
 */
    எழுத்து *
did_set_guifontset(optset_T *args UNUSED)
{
    எழுத்து *errmsg = வெற்று;

    எனில் (STRCMP(p_guifontset, "*") == 0)
	errmsg = e_cant_select_fontset;
    இல்லையெனில் எனில் (gui.in_use && gui_init_font(p_guifontset, TRUE) != OK)
	errmsg = e_invalid_fontset;

    திரும்பு errmsg;
}
# endif

/*
 * The 'guifontwide' option is changed.
 */
    எழுத்து *
did_set_guifontwide(optset_T *args UNUSED)
{
    எழுத்து *errmsg = வெற்று;

    எனில் (STRCMP(p_guifontwide, "*") == 0)
	errmsg = e_cant_select_wide_font;
    இல்லையெனில் எனில் (gui_get_wide_font() == FAIL)
	errmsg = e_invalid_wide_font;

    திரும்பு errmsg;
}
#endif

#எனில் defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
/*
 * The 'guiligatures' option is changed.
 */
    எழுத்து *
did_set_guiligatures(optset_T *args UNUSED)
{
    gui_set_ligatures();
    திரும்பு வெற்று;
}
#endif

#எனில் defined(FEAT_GUI)
/*
 * The 'guioptions' option is changed.
 */
    எழுத்து *
did_set_guioptions(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    எழுத்து *errmsg;

    errmsg = did_set_option_listflag(*varp, (char_u *)GO_ALL, args->os_errbuf,
		    args->os_errbuflen);
    எனில் (errmsg != வெற்று)
	திரும்பு errmsg;

    gui_init_which_components(args->os_oldval.string);
    திரும்பு வெற்று;
}

    முழு
expand_set_guioptions(optexpand_T *args, முழு *இலக்கம்_Matches, char_u ***matches)
{
    திரும்பு expand_set_opt_listflag(args, (char_u*)GO_ALL, இலக்கம்_Matches, matches);
}
#endif

#எனில் defined(FEAT_GUI_TABLINE)
/*
 * The 'guitablabel' option is changed.
 */
    எழுத்து *
did_set_guitablabel(optset_T *args UNUSED)
{
    redraw_tabline = TRUE;
    திரும்பு வெற்று;
}
#endif

/*
 * The 'helpfile' option is changed.
 */
    எழுத்து *
did_set_helpfile(optset_T *args UNUSED)
{
    // May compute new values for $VIM and $VIMRUNTIME
    எனில் (didset_vim)
	vim_unsetenv_ext((char_u *)"VIM");
    எனில் (didset_vimruntime)
	vim_unsetenv_ext((char_u *)"VIMRUNTIME");
    திரும்பு வெற்று;
}

#எனில் defined(FEAT_MULTI_LANG)
/*
 * The 'helplang' option is changed.
 */
    எழுத்து *
did_set_helplang(optset_T *args UNUSED)
{
    எழுத்து *errmsg = வெற்று;

    // Check for "", "ab", "ab,cd", etc.
    க்கு (char_u *s = p_hlg; *s != NUL; s += 3)
    {
	எனில் (s[1] == NUL || ((s[2] != ',' || s[3] == NUL) && s[2] != NUL))
	{
	    errmsg = e_invalid_argument;
	    நிறுத்து;
	}
	எனில் (s[2] == NUL)
	    நிறுத்து;
    }

    திரும்பு errmsg;
}
#endif

/*
 * The 'highlight' option is changed.
 */
    எழுத்து *
did_set_highlight(optset_T *args UNUSED)
{
    எனில் (highlight_changed() == FAIL)
	திரும்பு e_invalid_argument;	// invalid flags

    திரும்பு வெற்று;
}

    நிலையான முழு
expand_hl_occasions(
	optexpand_T *args,
	முழு *இலக்கம்_Matches,
	char_u ***matches,
	char_u prefix)
{
    char_u	    *p;
    நிலையான char_u   hl_flags[HLF_மொத்தம்] = HL_FLAGS;
    size_t	    i;
    முழு		    மொத்தம் = 0;

    *matches = ALLOC_MULT(char_u *, HLF_மொத்தம் + 1);
    எனில் (*matches == வெற்று)
	திரும்பு FAIL;

    // We still want to return the full option if it's requested.
    எனில் (args->oe_include_orig_val)
    {
	p = vim_strsave(args->oe_opt_மதிப்பு);
	எனில் (p == வெற்று)
	{
	    VIM_CLEAR(*matches);
	    திரும்பு FAIL;
	}
	(*matches)[மொத்தம்++] = p;
    }

    க்கு (i = 0; i < HLF_மொத்தம்; i++)
    {
	p = alloc((prefix == NUL ? 1 : 2) + 1);
	எனில் (p == வெற்று)
	{
	    எனில் (மொத்தம் == 0)
	    {
		VIM_CLEAR(*matches);
		திரும்பு FAIL;
	    }
	    இல்லையெனில்
		நிறுத்து;
	}
	எனில் (prefix == NUL)
	    sprintf((எழுத்து *)p, "%c", hl_flags[i]);
	இல்லையெனில்
	    sprintf((எழுத்து *)p, "%c%c", prefix, hl_flags[i]);
	(*matches)[மொத்தம்++] = p;
    }

    எனில் (மொத்தம் == 0)
    {
	VIM_CLEAR(*matches);
	திரும்பு FAIL;
    }
    *இலக்கம்_Matches = மொத்தம்;
    திரும்பு OK;
}

/*
 * Expand 'highlight' option.
 */
    முழு
expand_set_highlight(optexpand_T *args, முழு *இலக்கம்_Matches, char_u ***matches)
{
    char_u	    *p;
    expand_T	    *xp = args->oe_xp;
    size_t	    i;
    முழு		    மொத்தம் = 0;

    எனில் (xp->xp_pattern > args->oe_set_arg && *(xp->xp_pattern-1) == ':')
    {
	// Right after a ':', meaning we just return all highlight names.
	திரும்பு expand_set_opt_generic(
		args,
		get_highlight_name,
		இலக்கம்_Matches,
		matches);
    }

    எனில் (*xp->xp_pattern == NUL)
	// At beginning of a comma-separated list. Return the specific list of
	// supported occasions.
	திரும்பு expand_hl_occasions(args, இலக்கம்_Matches, matches, NUL);

    // We are after the initial character (which indicates the occasion). We
    // already made sure we are not matching after a ':' above, so now we want
    // to match against display mode modifiers.
    // Since the xp_pattern starts from the beginning, we need to include it in
    // the returned match.

    // Note: Keep this in sync with highlight_changed()
    நிலையான char_u p_hl_mode_மதிப்புகள்[] =
	{':', 'b', 'i', '-', 'n', 'r', 's', 'u', 'c', '2', 'd', '=', 't'};
    size_t இலக்கம்_hl_modes = ARRAY_LENGTH(p_hl_mode_மதிப்புகள்);

    *matches = ALLOC_MULT(char_u *, இலக்கம்_hl_modes);
    எனில் (*matches == வெற்று)
	திரும்பு FAIL;

    முழு pattern_len = xp->xp_pattern_len;

    க்கு (i = 0; i < இலக்கம்_hl_modes; i++)
    {
	// Don't allow duplicates as these are unique flags
	char_u *dup = vim_strchr(xp->xp_pattern + 1, p_hl_mode_மதிப்புகள்[i]);
	எனில் (dup != வெற்று && (முழு)(dup - xp->xp_pattern) < pattern_len)
	    தொடர்;

	// ':' only works by itself, not with other flags.
	எனில் (pattern_len > 1 && p_hl_mode_மதிப்புகள்[i] == ':')
	    தொடர்;

	p = vim_strnsave(xp->xp_pattern, pattern_len + 1);
	எனில் (p == வெற்று)
	{
	    எனில் (i == 0)
	    {
		VIM_CLEAR(*matches);
		திரும்பு FAIL;
	    }
	    இல்லையெனில்
		நிறுத்து;
	}
	p[pattern_len] = p_hl_mode_மதிப்புகள்[i];
	p[pattern_len + 1] = NUL;
	(*matches)[மொத்தம்++] = p;
    }
    எனில் (மொத்தம் == 0)
    {
	VIM_CLEAR(*matches);
	திரும்பு FAIL;
    }
    *இலக்கம்_Matches = மொத்தம்;

    திரும்பு OK;
}

/*
 * The 'titlestring' or the 'iconstring' option is changed.
 */
    நிலையான எழுத்து *
parse_titleiconstring(optset_T *args UNUSED, முழு flagval UNUSED)
{
#ifdef FEAT_STL_OPT
    char_u	**varp = (char_u **)args->os_varp;

    // NULL => statusline syntax
    எனில் (vim_strchr(*varp, '%') && check_stl_option(*varp) == வெற்று)
	stl_syntax |= flagval;
    இல்லையெனில்
	stl_syntax &= ~flagval;
#endif
    did_set_title();

    திரும்பு வெற்று;
}

/*
 * The 'iconstring' option is changed.
 */
    எழுத்து *
did_set_iconstring(optset_T *args)
{
    முழு flagval = 0;

#ifdef FEAT_STL_OPT
    flagval = STL_IN_ICON;
#endif

    திரும்பு parse_titleiconstring(args, flagval);
}

#எனில் defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
/*
 * The 'imactivatekey' option is changed.
 */
    எழுத்து *
did_set_imactivatekey(optset_T *args UNUSED)
{
    எனில் (!im_xim_isvalid_imactivate())
	திரும்பு e_invalid_argument;
    திரும்பு வெற்று;
}
#endif

#ifdef FEAT_IMAGE
/*
 * The 'imageprotocol' option is changed.
 */
    எழுத்து *
did_set_imageprotocol(optset_T *args UNUSED)
{
    எனில் (update_image_backend() == FAIL)
	திரும்பு e_invalid_argument;
    திரும்பு வெற்று;
}

    முழு
expand_set_imageprotocol(optexpand_T *args, முழு *இலக்கம்_Matches, char_u ***matches)
{
    expand_T *xp = args->oe_xp;

    எனில் (xp->xp_pattern > args->oe_set_arg && *(xp->xp_pattern-1) == ':')
    {
	// 'imageprotocol' only has well-defined terms for completion for the
	// protocol part after the colon.
	திரும்பு expand_set_opt_string(
		args,
		p_ipc_protocol_மதிப்புகள்,
		ARRAY_LENGTH(p_ipc_protocol_மதிப்புகள்) - 1,
		இலக்கம்_Matches,
		matches);
    }
    // Use expand_set_opt_string instead of returning FAIL so that we can
    // include the original value if args->oe_include_orig_val is set.
    நிலையான எழுத்து *(empty[]) = {வெற்று};
    திரும்பு expand_set_opt_string(args, empty, 0, இலக்கம்_Matches, matches);
}
#endif

/*
 * The 'iskeyword' option is changed.
 */
    எழுத்து *
did_set_iskeyword(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    எனில் (varp == &p_isk)		// only check for global-value
    {
	எனில் (check_isopt(*varp) == FAIL)
	    திரும்பு e_invalid_argument;
    }
    இல்லையெனில்			// fallthrough for local-value
	திரும்பு did_set_isopt(args);

    திரும்பு வெற்று;
}

/*
 * The 'isident' or the 'iskeyword' or the 'isprint' or the 'isfname' option is
 * changed.
 */
    எழுத்து *
did_set_isopt(optset_T *args)
{
    // 'isident', 'iskeyword', 'isprint' or 'isfname' option: refill g_chartab[]
    // If the new option is invalid, use old value.
    // 'lisp' option: refill g_chartab[] for '-' char.
    எனில் (init_chartab() == FAIL)
    {
	args->os_restore_chartab = TRUE;// need to restore the chartab.
	திரும்பு e_invalid_argument;	// error in value
    }

    திரும்பு வெற்று;
}

/*
 * The 'jumpoptions' option is changed.
 */
    எழுத்து *
did_set_jumpoptions(optset_T *args UNUSED)
{
    எனில் (opt_strings_flags(p_jop, p_jop_மதிப்புகள், &jop_flags, TRUE) != OK)
	திரும்பு e_invalid_argument;

    திரும்பு வெற்று;
}

    முழு
expand_set_jumpoptions(optexpand_T *args, முழு *இலக்கம்_Matches, char_u ***matches)
{
    திரும்பு expand_set_opt_string(
	    args,
	    p_jop_மதிப்புகள்,
	    ARRAY_LENGTH(p_jop_மதிப்புகள்) - 1,
	    இலக்கம்_Matches,
	    matches);
}

#எனில் defined(FEAT_KEYMAP)
/*
 * The 'keymap' option is changed.
 */
    எழுத்து *
did_set_keymap(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    எழுத்து *errmsg = வெற்று;

    எனில் (!valid_filetype(*varp))
	errmsg = e_invalid_argument;
    இல்லையெனில்
    {
	முழு	    secure_save = secure;

	// Reset the secure flag, since the value of 'keymap' has
	// been checked to be safe.
	secure = 0;

	// load or unload key mapping tables
	errmsg = keymap_init();

	secure = secure_save;

	// Since we check the value, there is no need to set P_INSECURE,
	// even when the value comes from a modeline.
	args->os_மதிப்பு_checked = TRUE;
    }

    எனில் (errmsg == வெற்று)
    {
	எனில் (*curbuf->b_p_keymap != NUL)
	{
	    // Installed a new keymap, switch on using it.
	    curbuf->b_p_iminsert = B_IMODE_LMAP;
	    எனில் (curbuf->b_p_imsearch != B_IMODE_USE_செருகு)
		curbuf->b_p_imsearch = B_IMODE_LMAP;
	}
	இல்லையெனில்
	{
	    // Cleared the keymap, may reset 'iminsert' and 'imsearch'.
	    எனில் (curbuf->b_p_iminsert == B_IMODE_LMAP)
		curbuf->b_p_iminsert = B_IMODE_NONE;
	    எனில் (curbuf->b_p_imsearch == B_IMODE_LMAP)
		curbuf->b_p_imsearch = B_IMODE_USE_செருகு;
	}
	எனில் ((args->os_flags & OPT_LOCAL) == 0)
	{
	    set_iminsert_global();
	    set_imsearch_global();
	}
	status_redraw_curbuf();
    }

    திரும்பு errmsg;
}
#endif

/*
 * The 'keymodel' option is changed.
 */
    எழுத்து *
did_set_keymodel(optset_T *args UNUSED)
{
    எனில் (check_opt_strings(p_km, p_km_மதிப்புகள், TRUE) != OK)
	திரும்பு e_invalid_argument;

    km_stopsel = (vim_strchr(p_km, 'o') != வெற்று);
    km_startsel = (vim_strchr(p_km, 'a') != வெற்று);
    திரும்பு வெற்று;
}

    முழு
expand_set_keymodel(optexpand_T *args, முழு *இலக்கம்_Matches, char_u ***matches)
{
    திரும்பு expand_set_opt_string(
	    args,
	    p_km_மதிப்புகள்,
	    ARRAY_LENGTH(p_km_மதிப்புகள்) - 1,
	    இலக்கம்_Matches,
	    matches);
}

/*
 * The 'keyprotocol' option is changed.
 */
    எழுத்து *
did_set_keyprotocol(optset_T *args UNUSED)
{
    char_u *term = T_NAME;
    keyprot_T kpc = match_keyprotocol(term);
    எனில் (kpc == KEYPROTOCOL_FAIL)
	திரும்பு e_invalid_argument;

    apply_keyprotocol(term, kpc);

    திரும்பு வெற்று;
}

    முழு
expand_set_keyprotocol(optexpand_T *args, முழு *இலக்கம்_Matches, char_u ***matches)
{
    expand_T *xp = args->oe_xp;
    எனில் (xp->xp_pattern > args->oe_set_arg && *(xp->xp_pattern-1) == ':')
    {
	// 'keyprotocol' only has well-defined terms for completion for the
	// protocol part after the colon.
	திரும்பு expand_set_opt_string(
		args,
		p_kpc_protocol_மதிப்புகள்,
		ARRAY_LENGTH(p_kpc_protocol_மதிப்புகள்) - 1,
		இலக்கம்_Matches,
		matches);
    }
    // Use expand_set_opt_string instead of returning FAIL so that we can
    // include the original value if args->oe_include_orig_val is set.
    நிலையான எழுத்து *(empty[]) = {வெற்று};
    திரும்பு expand_set_opt_string(args, empty, 0, இலக்கம்_Matches, matches);
}

/*
 * The 'lispoptions' option is changed.
 */
    எழுத்து *
did_set_lispoptions(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    எனில் (**varp != NUL
	    && STRCMP(*varp, "expr:0") != 0
	    && STRCMP(*varp, "expr:1") != 0)
	திரும்பு e_invalid_argument;

    திரும்பு வெற்று;
}

    முழு
expand_set_lispoptions(optexpand_T *args, முழு *இலக்கம்_Matches, char_u ***matches)
{
    நிலையான எழுத்து *(p_lop_மதிப்புகள்[]) = {"expr:0", "expr:1", வெற்று};
    திரும்பு expand_set_opt_string(
	    args,
	    p_lop_மதிப்புகள்,
	    ARRAY_LENGTH(p_lop_மதிப்புகள்) - 1,
	    இலக்கம்_Matches,
	    matches);
}

/*
 * The 'matchpairs' option is changed.
 */
    எழுத்து *
did_set_matchpairs(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u	*p;

    எனில் (has_mbyte)
    {
	க்கு (p = *varp; *p != NUL; ++p)
	{
	    முழு x2 = -1;
	    முழு x3 = -1;

	    p += mb_ptr2len(p);
	    எனில் (*p != NUL)
		x2 = *p++;
	    எனில் (*p != NUL)
	    {
		x3 = mb_ptr2char(p);
		p += mb_ptr2len(p);
	    }
	    எனில் (x2 != ':' || x3 == -1 || (*p != NUL && *p != ','))
		திரும்பு e_invalid_argument;
	    எனில் (*p == NUL)
		நிறுத்து;
	}
    }
    இல்லையெனில்
    {
	// Check for "x:y,x:y"
	க்கு (p = *varp; *p != NUL; p += 4)
	{
	    எனில் (p[1] != ':' || p[2] == NUL || (p[3] != NUL && p[3] != ','))
		திரும்பு e_invalid_argument;
	    எனில் (p[3] == NUL)
		நிறுத்து;
	}
    }

    திரும்பு வெற்று;
}

/*
 * Process the updated 'messagesopt' option value.
 */
    எழுத்து *
did_set_messagesopt(optset_T *args UNUSED)
{
    எனில் (messagesopt_changed() == FAIL)
	திரும்பு e_invalid_argument;

    திரும்பு வெற்று;
}

    முழு
expand_set_messagesopt(optexpand_T *args, முழு *இலக்கம்_Matches, char_u ***matches)
{
    நிலையான எழுத்து *(p_mopt_மதிப்புகள்[]) = {"hit-enter", "wait:", "history:", வெற்று};
    திரும்பு expand_set_opt_string(
	    args,
	    p_mopt_மதிப்புகள்,
	    ARRAY_LENGTH(p_mopt_மதிப்புகள்) - 1,
	    இலக்கம்_Matches,
	    matches);
}

#எனில் defined(FEAT_SPELL)
/*
 * The 'mkspellmem' option is changed.
 */
    எழுத்து *
did_set_mkspellmem(optset_T *args UNUSED)
{
    எனில் (spell_check_msm() != OK)
	திரும்பு e_invalid_argument;

    திரும்பு வெற்று;
}
#endif

/*
 * The 'mouse' option is changed.
 */
    எழுத்து *
did_set_mouse(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    எழுத்து	*retval;

    retval = did_set_option_listflag(*varp, (char_u *)MOUSE_ALL, args->os_errbuf,
		    args->os_errbuflen);
    எனில் (retval == வெற்று)
    {
	redraw_tabline = TRUE;
	எனில் (tabline_height() > 0)
	    update_screen(UPD_VALID);
#எனில் defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
	popup_close_info(); // Close info popup to apply new properties
#endif
    }
    திரும்பு retval;
}

    முழு
expand_set_mouse(optexpand_T *args, முழு *இலக்கம்_Matches, char_u ***matches)
{
    திரும்பு expand_set_opt_listflag(args, (char_u*)MOUSE_ALL, இலக்கம்_Matches, matches);
}

/*
 * The 'mousemodel' option is changed.
 */
    எழுத்து *
did_set_mousemodel(optset_T *args UNUSED)
{
    எனில் (check_opt_strings(p_mousem, p_mousem_மதிப்புகள், FALSE) != OK)
	திரும்பு e_invalid_argument;
#எனில் defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU) && (XmVersion <= 1002)
    இல்லையெனில் எனில் (*p_mousem != *args->os_oldval.string)
	// Changed from "extend" to "popup" or "popup_setpos" or vv: need
	// to create or delete the popup menus.
	gui_motif_update_mousemodel(வேர்_menu);
#endif

    திரும்பு வெற்று;
}

    முழு
expand_set_mousemodel(optexpand_T *args, முழு *இலக்கம்_Matches, char_u ***matches)
{
    திரும்பு expand_set_opt_string(
	    args,
	    p_mousem_மதிப்புகள்,
	    ARRAY_LENGTH(p_mousem_மதிப்புகள்) - 1,
	    இலக்கம்_Matches,
	    matches);
}

#எனில் defined(FEAT_MOUSESHAPE)
    எழுத்து *
did_set_mouseshape(optset_T *args UNUSED)
{
    எழுத்து *errmsg = வெற்று;

    errmsg = parse_shape_opt(SHAPE_MOUSE);
    update_mouseshape(-1);

    திரும்பு errmsg;
}
#endif

/*
 * The 'nrformats' option is changed.
 */
    எழுத்து *
did_set_nrformats(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    திரும்பு did_set_opt_strings(*varp, p_nf_மதிப்புகள், TRUE);
}

    முழு
expand_set_nrformats(optexpand_T *args, முழு *இலக்கம்_Matches, char_u ***matches)
{
    திரும்பு expand_set_opt_string(
	    args,
	    p_nf_மதிப்புகள்,
	    ARRAY_LENGTH(p_nf_மதிப்புகள்) - 1,
	    இலக்கம்_Matches,
	    matches);
}

#எனில் defined(FEAT_EVAL)
/*
 * One of the '*expr' options is changed: 'balloonexpr', 'diffexpr',
 * 'foldexpr', 'foldtext', 'formatexpr', 'includeexpr', 'indentexpr',
 * 'patchexpr', 'printexpr' and 'charconvert'.
 */
    எழுத்து *
did_set_optexpr(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    // If the option value starts with <SID> or s:, then replace that with
    // the script identifier.
    char_u *name = get_scriptlocal_funcname(*varp);
    எனில் (name != வெற்று)
    {
	free_string_option(*varp);
	*varp = name;
    }

    திரும்பு வெற்று;
}
#endif

/*
 * The 'pastetoggle' option is changed.
 */
    எழுத்து *
did_set_pastetoggle(optset_T *args UNUSED)
{
    char_u	*p;

    // translate key codes like in a mapping
    எனில் (*p_pt)
    {
	(வெறுமை)replace_termcodes(p_pt, &p, 0,
		REPTERM_FROM_PART | REPTERM_DO_LT, வெற்று);
	எனில் (p != வெற்று)
	{
	    free_string_option(p_pt);
	    p_pt = p;
	}
    }

    திரும்பு வெற்று;
}

#எனில் defined(FEAT_PROP_POPUP)
/*
 * The 'previewpopup' option is changed.
 */
    எழுத்து *
did_set_previewpopup(optset_T *args UNUSED)
{
    எனில் (parse_previewpopup(வெற்று) == FAIL)
	திரும்பு e_invalid_argument;

# எனில் defined(FEAT_QUICKFIX)
    popup_close_info();
# endif
    திரும்பு வெற்று;
}

    நிலையான முழு
expand_set_popupoption(optexpand_T *args, முழு *இலக்கம்_Matches, char_u ***matches,
	முழு previewpopup)
{
    expand_T *xp = args->oe_xp;

    எனில் (xp->xp_pattern > args->oe_set_arg && *(xp->xp_pattern-1) == ':')
    {
	எனில் (completing_மதிப்பு_for_subopt(args, "close")
		|| completing_மதிப்பு_for_subopt(args, "resize")
		|| completing_மதிப்பு_for_subopt(args, "shadow"))
	{
	    திரும்பு expand_set_opt_string(
		    args,
		    p_popup_option_on_off_மதிப்புகள்,
		    ARRAY_LENGTH(p_popup_option_on_off_மதிப்புகள்) - 1,
		    இலக்கம்_Matches,
		    matches);
	}
	எனில் (completing_மதிப்பு_for_subopt(args, "border"))
	{
	    திரும்பு expand_set_opt_string(
		    args,
		    previewpopup ? p_popup_pvp_border_மதிப்புகள்
			: p_popup_cpp_border_மதிப்புகள்,
		    (previewpopup ? ARRAY_LENGTH(p_popup_pvp_border_மதிப்புகள்) - 1
			: ARRAY_LENGTH(p_popup_cpp_border_மதிப்புகள்)) - 1,
		    இலக்கம்_Matches,
		    matches);
	}
	எனில் (completing_மதிப்பு_for_subopt(args, "align"))
	{
	    திரும்பு expand_set_opt_string(
		    args,
		    p_popup_option_align_மதிப்புகள்,
		    ARRAY_LENGTH(p_popup_option_align_மதிப்புகள்) - 1,
		    இலக்கம்_Matches,
		    matches);
	}
	எனில் (completing_மதிப்பு_for_subopt(args, "highlight")
		|| completing_மதிப்பு_for_subopt(args, "borderhighlight"))
	{
	    // Return the list of all highlight names
	    திரும்பு expand_set_opt_generic(
		    args,
		    get_highlight_name,
		    இலக்கம்_Matches,
		    matches);
	}
	திரும்பு FAIL;
    }

    திரும்பு expand_set_opt_string(
	    args,
	    previewpopup ? p_popup_pvp_option_மதிப்புகள்
		: p_popup_cpp_option_மதிப்புகள்,
	    previewpopup ? ARRAY_LENGTH(p_popup_pvp_option_மதிப்புகள்) - 1
		: ARRAY_LENGTH(p_popup_cpp_option_மதிப்புகள்) - 1,
	    இலக்கம்_Matches,
	    matches);
}

    முழு
expand_set_previewpopup(optexpand_T *args, முழு *இலக்கம்_Matches, char_u ***matches)
{
    திரும்பு expand_set_popupoption(args, இலக்கம்_Matches, matches, TRUE);
}

    முழு
expand_set_completepopup(optexpand_T *args, முழு *இலக்கம்_Matches, char_u ***matches)
{
    திரும்பு expand_set_popupoption(args, இலக்கம்_Matches, matches, FALSE);
}
#endif

#எனில் defined(FEAT_POSTSCRIPT)
/*
 * The 'printencoding' option is changed.
 */
    எழுத்து *
did_set_printencoding(optset_T *args UNUSED)
{
    char_u	*s, *p;

    // Canonize 'printencoding' if VIM standard one
    p = enc_canonize(p_penc);
    எனில் (p != வெற்று)
    {
	vim_free(p_penc);
	p_penc = p;
    }
    இல்லையெனில்
    {
	// Ensure lower case and '-' for '_'
	க்கு (s = p_penc; *s != NUL; s++)
	{
	    எனில் (*s == '_')
		*s = '-';
	    இல்லையெனில்
		*s = TOLOWER_ASC(*s);
	}
    }

    திரும்பு வெற்று;
}
#endif

#எனில் defined(FEAT_PRINTER)

    நிலையான char_u *
get_printoptions_names(expand_T *xp UNUSED, முழு idx)
{
    எனில் (idx >= (முழு)ARRAY_LENGTH(printer_opts))
	திரும்பு வெற்று;
    திரும்பு (char_u*)printer_opts[idx].name;
}

/*
 * Expand 'printoptions' option
 */
    முழு
expand_set_printoptions(optexpand_T *args, முழு *இலக்கம்_Matches, char_u ***matches)
{
    திரும்பு expand_set_opt_generic(
	    args,
	    get_printoptions_names,
	    இலக்கம்_Matches,
	    matches);
}
#endif

#எனில் defined(FEAT_STL_OPT)
/*
 * The 'statusline' or the 'tabline' or the 'rulerformat' option is changed.
 * "rulerformat" is TRUE if the 'rulerformat' option is changed.
 */
    நிலையான எழுத்து *
parse_statustabline_rulerformat(optset_T *args, முழு rulerformat)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u	*s;
    எழுத்து	*errmsg = வெற்று;
    முழு		wid;

    எனில் (rulerformat)	// reset ru_wid first
	ru_wid = 0;
    s = *varp;
    எனில் (rulerformat && *s == '%')
    {
	// set ru_wid if 'ruf' starts with "%99("
	எனில் (*++s == '-')	// ignore a '-'
	    s++;
	wid = getdigits(&s);
	எனில் (wid && *s == '(' && (errmsg = check_stl_option(p_ruf)) == வெற்று)
	    ru_wid = wid;
	இல்லையெனில்
	{
	    // Validate the flags in 'rulerformat' only if it doesn't point to
	    // a custom function ("%!" flag).
	    எனில் ((*varp)[1] != '!')
		errmsg = check_stl_option(p_ruf);
	}
    }
    // check 'statusline' or 'tabline' only if it doesn't start with "%!"
    இல்லையெனில் எனில் (rulerformat || s[0] != '%' || s[1] != '!')
	errmsg = check_stl_option(s);
    எனில் (rulerformat && errmsg == வெற்று)
	comp_col();

    திரும்பு errmsg;
}
#endif

#எனில் defined(FEAT_RENDER_OPTIONS)
/*
 * The 'renderoptions' option is changed.
 */
    எழுத்து *
did_set_renderoptions(optset_T *args UNUSED)
{
    எனில் (!gui_mch_set_rendering_options(p_rop))
	திரும்பு e_invalid_argument;

    திரும்பு வெற்று;
}
#endif

#எனில் defined(FEAT_RIGHTLEFT)
/*
 * The 'rightleftcmd' option is changed.
 */
    எழுத்து *
did_set_rightleftcmd(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    // Currently only "search" is a supported value.
    எனில் (**varp != NUL && STRCMP(*varp, "search") != 0)
	திரும்பு e_invalid_argument;

    திரும்பு வெற்று;
}

    முழு
expand_set_rightleftcmd(optexpand_T *args, முழு *இலக்கம்_Matches, char_u ***matches)
{
    நிலையான எழுத்து *(p_rlc_மதிப்புகள்[]) = {"search", வெற்று};
    திரும்பு expand_set_opt_string(
	    args,
	    p_rlc_மதிப்புகள்,
	    ARRAY_LENGTH(p_rlc_மதிப்புகள்) - 1,
	    இலக்கம்_Matches,
	    matches);
}
#endif

#define PUM_BORDER_CLEAR()   \
    செய் {                     \
	pum_set_border(FALSE); \
	pum_set_shadow(FALSE); \
	pum_set_margin(FALSE); \
    } போது (0)

/*
 * Parse a border value from a pumopt "border:" token.
 * Returns OK on success, FAIL on error.
 */
    நிலையான முழு
parse_pumopt_border(char_u *val, முழு len)
{
    // Use box-drawing characters only when 'encoding' is "utf-8" and
    // 'ambiwidth' is "single".
    முழு	    can_use_box_chars = (enc_utf8 && *p_ambw == 's');
    char_u  *token;

    token = vim_strnsave(val, len);
    எனில் (token == வெற்று)
	திரும்பு FAIL;

    எனில் (can_use_box_chars && STRCMP(token, "single") == 0)
	pum_set_border_chars(0x2500, 0x2502, 0x2500, 0x2502, // ─ │ ─ │
		0x250c, 0x2510, 0x2518, 0x2514); // ┌ ┐ ┘ └
    இல்லையெனில் எனில் (can_use_box_chars && STRCMP(token, "double") == 0)
	pum_set_border_chars(0x2550, 0x2551, 0x2550, 0x2551, // ═ ║ ═ ║
		0x2554, 0x2557, 0x255D, 0x255A); // ╔ ╗ ╝  ╚
    இல்லையெனில் எனில் (can_use_box_chars && STRCMP(token, "round") == 0)
	pum_set_border_chars(0x2500, 0x2502, 0x2500, 0x2502, // ─ │ ─ │
		0x256d, 0x256e, 0x256f, 0x2570); // ╭ ╮ ╯ ╰
    இல்லையெனில் எனில் (STRCMP(token, "ascii") == 0)
	pum_set_border_chars('-', '|', '-', '|', '+', '+', '+', '+');
    இல்லையெனில் எனில் (STRNCMP(token, "custom:", 7) == 0)
    {
	char_u	*q = token + 7;
	முழு	out[8];

	க்கு (முழு i = 0; i < 8; i++)
	{
	    எனில் (*q == NUL || *q == ',')
	    {
		vim_free(token);
		திரும்பு FAIL;
	    }
	    out[i] = mb_ptr2char(q);
	    mb_ptr2char_adv(&q);
	    எனில் (i < 7)
	    {
		எனில் (*q != ';')
		{
		    vim_free(token);
		    திரும்பு FAIL;
		}
		q++;
	    }
	}
	எனில் (*q != NUL && *q != ',')
	{
	    vim_free(token);
	    திரும்பு FAIL;
	}
	pum_set_border_chars(out[0], out[1], out[2], out[3], out[4], out[5],
		out[6], out[7]);
    }
    இல்லையெனில்
    {
	vim_free(token);
	திரும்பு FAIL;
    }

    vim_free(token);
    pum_set_border(TRUE);
    திரும்பு OK;
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
    எழுத்து *
did_set_pumopt(optset_T *args)
{
    char_u  **varp = (char_u **)args->os_varp;
    char_u  *p;
    முழு	    have_border = FALSE;
    முழு	    have_margin = FALSE;

    // Reset to defaults.
    PUM_BORDER_CLEAR();
    p_ph = 0;
    p_pw = 15;
    p_pmw = 0;
    p_po = 100;

    எனில் (*varp == வெற்று || **varp == NUL)
	திரும்பு வெற்று;

    க்கு (p = *varp; p != வெற்று && *p != NUL; )
    {
	char_u *comma = vim_strchr(p, ',');
	முழு len;

	எனில் (comma != வெற்று)
	    len = (முழு)(comma - p);
	இல்லையெனில்
	    len = (முழு)STRLEN(p);

	எனில் (STRNCMP(p, "border:", 7) == 0)
	{
	    எனில் (have_border)
		செல் error;
	    have_border = TRUE;
	    எனில் (parse_pumopt_border(p + 7, len - 7) == FAIL)
		செல் error;
	}
	இல்லையெனில் எனில் (STRNCMP(p, "height:", 7) == 0)
	{
	    long n = atol((எழுத்து *)p + 7);
	    எனில் (n < 0)
		செல் error;
	    p_ph = n;
	}
	இல்லையெனில் எனில் (STRNCMP(p, "width:", 6) == 0)
	{
	    long n = atol((எழுத்து *)p + 6);
	    எனில் (n < 0)
		செல் error;
	    p_pw = n;
	}
	இல்லையெனில் எனில் (STRNCMP(p, "maxwidth:", 9) == 0)
	{
	    long n = atol((எழுத்து *)p + 9);
	    எனில் (n < 0)
		செல் error;
	    p_pmw = n;
	}
	இல்லையெனில் எனில் (STRNCMP(p, "opacity:", 8) == 0)
	{
	    long n = atol((எழுத்து *)p + 8);
	    எனில் (n < 0 || n > 100)
		செல் error;
	    p_po = n;
	}
	இல்லையெனில் எனில் (len == 6 && STRNCMP(p, "shadow", 6) == 0)
	    pum_set_shadow(TRUE);
	இல்லையெனில் எனில் (len == 6 && STRNCMP(p, "margin", 6) == 0)
	{
	    have_margin = TRUE;
	    pum_set_margin(TRUE);
	}
	இல்லையெனில்
	    செல் error;

	எனில் (comma != வெற்று)
	    p = comma + 1;
	இல்லையெனில்
	    நிறுத்து;
    }

    எனில் (have_margin && !have_border)
	செல் error;

    // Invalidate cached background for opacity changes.
    pum_opacity_changed();

    திரும்பு வெற்று;

error:
    PUM_BORDER_CLEAR();
    p_ph = 0;
    p_pw = 15;
    p_pmw = 0;
    p_po = 100;
    திரும்பு e_invalid_argument;
}

    நிலையான char_u *
get_pum_border_style(expand_T *xp UNUSED, முழு idx)
{
    நிலையான எழுத்து *styles[] = {"ascii", "custom:", "single", "double", "round"};
    திரும்பு idx < ((enc_utf8 && *p_ambw == 's') ? (முழு)ARRAY_LENGTH(styles) : 2)
	    ? (char_u *)styles[idx] : வெற்று;
}

    முழு
expand_set_pumopt(optexpand_T *args, முழு *இலக்கம்_Matches, char_u ***matches)
{
    expand_T *xp = args->oe_xp;

    எனில் (xp->xp_pattern > args->oe_set_arg && *(xp->xp_pattern-1) == ':')
    {
	எனில் (completing_மதிப்பு_for_subopt(args, "border"))
	{
	    திரும்பு expand_set_opt_generic(
		    args, get_pum_border_style, இலக்கம்_Matches, matches);
	}
	திரும்பு FAIL;
    }

    நிலையான எழுத்து *(p_pumopt_மதிப்புகள்[]) = {"border:", "height:", "width:",
	"maxwidth:", "opacity:", "shadow", "margin", வெற்று};
    திரும்பு expand_set_opt_string(
	    args,
	    p_pumopt_மதிப்புகள்,
	    ARRAY_LENGTH(p_pumopt_மதிப்புகள்) - 1,
	    இலக்கம்_Matches,
	    matches);
}

/*
 * The 'pumborder' option is changed.
 * Rules:
 *   - One of { single, double, round, ascii, custom:XXXXXXXX } may appear.
 *   - "margin" may appear, but only together with exactly one border style.
 *   - "shadow" is independent and can be combined freely.
 */
    எழுத்து *
did_set_pumborder(optset_T *args)
{
    char_u  **varp = (char_u **)args->os_varp;
    char_u  *p;
    முழு	    len;
    முழு	    have_border = FALSE;
    முழு	    have_margin = FALSE;

    PUM_BORDER_CLEAR();

    எனில் (*varp == வெற்று || **varp == NUL)
	திரும்பு வெற்று;

    க்கு (p = *varp; p != வெற்று && *p != NUL; )
    {
	char_u *comma = vim_strchr(p, ',');
	எனில் (comma != வெற்று)
	    len = (முழு)(comma - p);
	இல்லையெனில்
	    len = (முழு)STRLEN(p);

	எனில் (STRNCMP(p, "shadow", len) == 0 && len == 6)
	    pum_set_shadow(TRUE);
	இல்லையெனில் எனில் (STRNCMP(p, "margin", len) == 0 && len == 6)
	{
	    have_margin = TRUE;
	    pum_set_margin(TRUE);
	}
	இல்லையெனில்
	{
	    எனில் (have_border)
		செல் error;
	    have_border = TRUE;
	    எனில் (parse_pumopt_border(p, len) == FAIL)
		செல் error;
	}

	எனில் (comma != வெற்று)
	    p = comma + 1;
	இல்லையெனில்
	    நிறுத்து;
    }

    எனில் (have_margin && !have_border)
	செல் error;

    திரும்பு வெற்று;

error:
    PUM_BORDER_CLEAR();
    pum_set_border_chars(0, 0, 0, 0, 0, 0, 0, 0);
    திரும்பு e_invalid_argument;
}

    நிலையான char_u *
get_pumborder_token(expand_T *xp, முழு idx)
{
    திரும்பு idx == 0 ? (char_u *)"margin"
	 : idx == 1 ? (char_u *)"shadow"
	 : get_pum_border_style(xp, idx - 2);
}

    முழு
expand_set_pumborder(optexpand_T *args, முழு *இலக்கம்_Matches, char_u ***matches)
{
    திரும்பு expand_set_opt_generic(
	    args, get_pumborder_token, இலக்கம்_Matches, matches);
}

#எனில் defined(FEAT_STL_OPT)

/*
 * The 'rulerformat' option is changed.
 */
    எழுத்து *
did_set_rulerformat(optset_T *args)
{
    திரும்பு parse_statustabline_rulerformat(args, TRUE);
}
#endif

#எனில் defined(FEAT_TABPANEL)
/*
 * Process the new 'tabpanelopt' option value.
 */
    எழுத்து *
did_set_tabpanelopt(optset_T *args UNUSED)
{
    எனில் (tabpanelopt_changed() == FAIL)
	திரும்பு e_invalid_argument;

    திரும்பு வெற்று;
}

    முழு
expand_set_tabpanelopt(optexpand_T *args, முழு *இலக்கம்_Matches, char_u ***matches)
{
    expand_T *xp = args->oe_xp;

    எனில் (xp->xp_pattern > args->oe_set_arg && *(xp->xp_pattern-1) == ':')
    {
	எனில் (completing_மதிப்பு_for_subopt(args, "align"))
	{
	    திரும்பு expand_set_opt_string(
		    args,
		    p_tplo_align_மதிப்புகள்,
		    ARRAY_LENGTH(p_tplo_align_மதிப்புகள்) - 1,
		    இலக்கம்_Matches,
		    matches);
	}
	திரும்பு FAIL;
    }

    திரும்பு expand_set_opt_string(
	    args,
	    p_tplo_மதிப்புகள்,
	    ARRAY_LENGTH(p_tplo_மதிப்புகள்) - 1,
	    இலக்கம்_Matches,
	    matches);
}
#endif

/*
 * The 'scrollopt' option is changed.
 */
    எழுத்து *
did_set_scrollopt(optset_T *args UNUSED)
{
    திரும்பு did_set_opt_strings(p_sbo, p_scbopt_மதிப்புகள், TRUE);
}

    முழு
expand_set_scrollopt(optexpand_T *args, முழு *இலக்கம்_Matches, char_u ***matches)
{
    திரும்பு expand_set_opt_string(
	    args,
	    p_scbopt_மதிப்புகள்,
	    ARRAY_LENGTH(p_scbopt_மதிப்புகள்) - 1,
	    இலக்கம்_Matches,
	    matches);
}

/*
 * The 'wlseat' option is changed
 */
    எழுத்து *
did_set_wlseat(optset_T *args UNUSED)
{
#ifdef FEAT_WAYLAND_CLIPBOARD
    // If there isn't any seat named 'wlseat', then let the Wayland clipboard be
    // unavailable. Ignore errors returned.
    clip_reset_wayland();
#endif

    திரும்பு வெற்று;
}

/*
 * The 'selection' option is changed.
 */
    எழுத்து *
did_set_selection(optset_T *args UNUSED)
{
    எனில் (*p_sel == NUL || check_opt_strings(p_sel, p_sel_மதிப்புகள், FALSE) != OK)
	திரும்பு e_invalid_argument;

    திரும்பு வெற்று;
}

    முழு
expand_set_selection(optexpand_T *args, முழு *இலக்கம்_Matches, char_u ***matches)
{
    திரும்பு expand_set_opt_string(
	    args,
	    p_sel_மதிப்புகள்,
	    ARRAY_LENGTH(p_sel_மதிப்புகள்) - 1,
	    இலக்கம்_Matches,
	    matches);
}

/*
 * The 'selectmode' option is changed.
 */
    எழுத்து *
did_set_selectmode(optset_T *args UNUSED)
{
    திரும்பு did_set_opt_strings(p_slm, p_slm_மதிப்புகள், TRUE);
}

    முழு
expand_set_selectmode(optexpand_T *args, முழு *இலக்கம்_Matches, char_u ***matches)
{
    திரும்பு expand_set_opt_string(
	    args,
	    p_slm_மதிப்புகள்,
	    ARRAY_LENGTH(p_slm_மதிப்புகள்) - 1,
	    இலக்கம்_Matches,
	    matches);
}

#எனில் defined(FEAT_SESSION)
/*
 * The 'sessionoptions' option is changed.
 */
    எழுத்து *
did_set_sessionoptions(optset_T *args)
{
    எனில் (opt_strings_flags(p_ssop, p_ssop_மதிப்புகள், &ssop_flags, TRUE) != OK)
	திரும்பு e_invalid_argument;
    எனில் ((ssop_flags & SSOP_CURDIR) && (ssop_flags & SSOP_SESDIR))
    {
	// Don't allow both "sesdir" and "curdir".
	(வெறுமை)opt_strings_flags(args->os_oldval.string, p_ssop_மதிப்புகள்,
							&ssop_flags, TRUE);
	திரும்பு e_invalid_argument;
    }

    திரும்பு வெற்று;
}

    முழு
expand_set_sessionoptions(
    optexpand_T		*args,
    முழு			*இலக்கம்_Matches,
    char_u		***matches)
{
    திரும்பு expand_set_opt_string(
	    args,
	    p_ssop_மதிப்புகள்,
	    ARRAY_LENGTH(p_ssop_மதிப்புகள்) - 1,
	    இலக்கம்_Matches,
	    matches);
}
#endif

/*
 * Validate 'shellpipe'/'shellredir' option.
 */
    எழுத்து *
did_set_shellpipe_redir(optset_T *args)
{
    char_u	*p;
    bool	seen = false;

    க்கு (p = args->os_newval.string; *p != NUL; ++p)
    {
	எனில் (*p != '%')
	    தொடர்;

	எனில் (p[1] == NUL)
	    திரும்பு e_invalid_format_string_single_percent_s;

	எனில் (p[1] == '%')
	{
	    ++p;    // skip second %
	    தொடர்;
	}

	எனில் (p[1] == 's')
	{
	    எனில் (seen)
		திரும்பு e_invalid_format_string_single_percent_s;

	    seen = true;
	    ++p;    // consume 's'
	    தொடர்;
	}
	திரும்பு e_invalid_format_string_single_percent_s;
    }
    திரும்பு வெற்று;
}

/*
 * The 'shortmess' option is changed.
 */
    எழுத்து *
did_set_shortmess(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    திரும்பு did_set_option_listflag(*varp, (char_u *)SHM_ALL, args->os_errbuf,
		    args->os_errbuflen);
}

    முழு
expand_set_shortmess(optexpand_T *args, முழு *இலக்கம்_Matches, char_u ***matches)
{
    திரும்பு expand_set_opt_listflag(args, (char_u*)SHM_ALL, இலக்கம்_Matches, matches);
}

#எனில் defined(FEAT_LINEBREAK)
/*
 * The 'showbreak' option is changed.
 */
    எழுத்து *
did_set_showbreak(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u	*s;

    க்கு (s = *varp; *s; )
    {
	எனில் (ptr2cells(s) != 1)
	    திரும்பு e_showbreak_contains_unprintable_or_wide_character;
	MB_PTR_ADV(s);
    }

    திரும்பு வெற்று;
}
#endif

/*
 * The 'showcmdloc' option is changed.
 */
    எழுத்து *
did_set_showcmdloc(optset_T *args UNUSED)
{
    எழுத்து	*errmsg = did_set_opt_strings(p_sloc, p_sloc_மதிப்புகள், FALSE);

    எனில் (errmsg == வெற்று)
	comp_col();

    திரும்பு errmsg;
}

    முழு
expand_set_showcmdloc(optexpand_T *args, முழு *இலக்கம்_Matches, char_u ***matches)
{
    திரும்பு expand_set_opt_string(
	    args,
	    p_sloc_மதிப்புகள்,
	    ARRAY_LENGTH(p_sloc_மதிப்புகள்) - 1,
	    இலக்கம்_Matches,
	    matches);
}

#எனில் defined(FEAT_SIGNS)
/*
 * The 'signcolumn' option is changed.
 */
    எழுத்து *
did_set_signcolumn(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

# எனில் defined(FEAT_LINEBREAK)
    எனில் (check_opt_strings(*varp, p_scl_மதிப்புகள், FALSE) != OK)
	திரும்பு e_invalid_argument;
    // When changing the 'signcolumn' to or from 'number', recompute the
    // width of the number column if 'number' or 'relativenumber' is set.
    எனில் (((*args->os_oldval.string == 'n' && args->os_oldval.string[1] == 'u')
		|| (*curwin->w_p_scl == 'n' && *(curwin->w_p_scl + 1) =='u'))
	    && (curwin->w_p_nu || curwin->w_p_rnu))
	curwin->w_nrwidth_வரி_மொத்தம் = 0;
# endif

# எனில் defined(FEAT_GUI)
    // In the GUI, when sign icons are used, a full screen clear is needed
    // to properly redraw the sign icons.
    எனில் (gui.in_use && curbuf->b_signlist != வெற்று)
	redraw_all_later(UPD_CLEAR);
# endif

    திரும்பு வெற்று;
}

    முழு
expand_set_signcolumn(optexpand_T *args, முழு *இலக்கம்_Matches, char_u ***matches)
{
    திரும்பு expand_set_opt_string(
	    args,
	    p_scl_மதிப்புகள்,
	    ARRAY_LENGTH(p_scl_மதிப்புகள்) - 1,
	    இலக்கம்_Matches,
	    matches);
}
#endif

#எனில் defined(FEAT_SPELL)
/*
 * The 'spellcapcheck' option is changed.
 */
    எழுத்து *
did_set_spellcapcheck(optset_T *args UNUSED)
{
    // compile the regexp program.
    திரும்பு compile_cap_prog(curwin->w_s);
}

/*
 * The 'spellfile' option is changed.
 */
    எழுத்து *
did_set_spellfile(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    எனில் (!valid_spellfile(*varp))
	திரும்பு e_invalid_argument;

    // If there is a window for this buffer in which 'spell' is set load the
    // wordlists.
    திரும்பு did_set_spell_option();
}

/*
 * The 'spell' option is changed.
 */
    எழுத்து *
did_set_spelllang(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    எனில் (!valid_spelllang(*varp))
	திரும்பு e_invalid_argument;

    // If there is a window for this buffer in which 'spell' is set load the
    // wordlists.
    திரும்பு did_set_spell_option();
}

/*
 * The 'spelloptions' option is changed.
 */
    எழுத்து *
did_set_spelloptions(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    எனில் (**varp != NUL && STRCMP(*varp, "camel") != 0)
	திரும்பு e_invalid_argument;

    திரும்பு வெற்று;
}

    முழு
expand_set_spelloptions(optexpand_T *args, முழு *இலக்கம்_Matches, char_u ***matches)
{
    நிலையான எழுத்து *(p_spo_மதிப்புகள்[]) = {"camel", வெற்று};
    திரும்பு expand_set_opt_string(
	    args,
	    p_spo_மதிப்புகள்,
	    ARRAY_LENGTH(p_spo_மதிப்புகள்) - 1,
	    இலக்கம்_Matches,
	    matches);
}

/*
 * The 'spellsuggest' option is changed.
 */
    எழுத்து *
did_set_spellsuggest(optset_T *args UNUSED)
{
    எனில் (spell_check_sps() != OK)
	திரும்பு e_invalid_argument;

    திரும்பு வெற்று;
}

    முழு
expand_set_spellsuggest(optexpand_T *args, முழு *இலக்கம்_Matches, char_u ***matches)
{
    திரும்பு expand_set_opt_string(
	    args,
	    p_sps_மதிப்புகள்,
	    ARRAY_LENGTH(p_sps_மதிப்புகள்) - 1,
	    இலக்கம்_Matches,
	    matches);
}
#endif

/*
 * The 'splitkeep' option is changed.
 */
    எழுத்து *
did_set_splitkeep(optset_T *args UNUSED)
{
    win_T	*wp;
    tabpage_T	*tp;
    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	wp->w_prev_height = wp->w_height;
	wp->w_prev_winrow = wp->w_winrow;
    }
    திரும்பு did_set_opt_strings(p_spk, p_spk_மதிப்புகள், FALSE);
}

    முழு
expand_set_splitkeep(optexpand_T *args, முழு *இலக்கம்_Matches, char_u ***matches)
{
    திரும்பு expand_set_opt_string(
	    args,
	    p_spk_மதிப்புகள்,
	    ARRAY_LENGTH(p_spk_மதிப்புகள்) - 1,
	    இலக்கம்_Matches,
	    matches);
}

#எனில் defined(FEAT_STL_OPT)
/*
 * The 'statusline' option is changed.
 */
    எழுத்து *
did_set_statusline(optset_T *args)
{
    char_u  **varp = (char_u **)args->os_varp;
    எழுத்து    *ret = parse_statustabline_rulerformat(args, FALSE);

    எனில் (ret != வெற்று)
	திரும்பு ret;
    update_stl_rendered_height(varp == &curwin->w_p_stl ? curwin : வெற்று);
    frame_change_statusline_height();

    திரும்பு வெற்று;
}

/*
 * Rewrite the 'statuslineopt' string: replace the last "maxheight:N" with
 * actual_stlh and remove earlier duplicates.  Keyword order is preserved.
 */
    நிலையான வெறுமை
update_stlo_maxheight(char_u **varp, முழு actual_stlh)
{
    மாறிலி char_u    *src = *varp;
    மாறிலி char_u    *last_mh = வெற்று;
    மாறிலி char_u    *p;

    // Find the last "maxheight:" token.
    p = src;
    போது (*p != NUL)
    {
	எனில் (STRNCMP(p, "maxheight:", 10) == 0)
	    last_mh = p;
	போது (*p != ',' && *p != NUL)
	    ++p;
	எனில் (*p == ',')
	    ++p;
    }

    எனில் (last_mh == வெற்று)
	திரும்பு;

    size_t	bufsize = STRLEN(src) + NUMBUFLEN + 1;
    char_u	*buf = alloc(bufsize);
    எனில் (buf == வெற்று)
	திரும்பு;

    முழு		len = 0;
    bool	need_comma = false;

    p = src;
    போது (*p != NUL)
    {
	மாறிலி	char_u	*tok = p;
	முழு	tok_len;

	போது (*p != ',' && *p != NUL)
	    ++p;
	tok_len = (முழு)(p - tok);
	எனில் (*p == ',')
	    ++p;

	எனில் (STRNCMP(tok, "maxheight:", 10) == 0)
	{
	    எனில் (tok == last_mh)
	    {
		// Replace the last occurrence with the actual value.
		எனில் (need_comma)
		    buf[len++] = ',';
		len += vim_snprintf((எழுத்து *)buf + len,
			bufsize - len, "maxheight:%d", actual_stlh);
		need_comma = true;
	    }
	}
	இல்லையெனில்
	{
	    எனில் (need_comma)
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
    எழுத்து *
did_set_statuslineopt(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    win_T       *wp = varp == &curwin->w_p_stlo ? curwin : வெற்று;

    எனில் (statuslineopt_changed(*varp, wp) == FAIL)
	திரும்பு e_invalid_argument;

    // Sync stl_rendered_height with the current statusline.
    update_stl_rendered_height(wp);

    // Update the maxheight value to the actual value set.
    // Note: Must be changed if p_stlo_values are changed.
    எனில் (*varp != empty_option)
    {
	முழு actual_stlh;

	எனில் (wp != வெற்று)
	{
	    frame_change_statusline_height();
	    actual_stlh = MIN(wp->w_p_stlo_mh,
		    wp->w_height + wp->w_status_height - p_wmh);
	}
	இல்லையெனில்
	    actual_stlh = frame_change_statusline_height();

	// Only re-serialize when the window actually has a status line shown.
	எனில் (actual_stlh > 0)
	{
	    update_stlo_maxheight(varp, actual_stlh);
	    // Update the parsed maxheight member directly.
	    எனில் (wp != வெற்று)
		wp->w_p_stlo_mh = actual_stlh;
	    இல்லையெனில்
		set_stlo_mh(actual_stlh);
	}
    }
    இல்லையெனில்
	frame_change_statusline_height();

    எனில் ((args->os_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	// :set clears the local values.
	clear_string_option(&curwin->w_p_stlo);
	curwin->w_p_stlo_fh = -1;
	curwin->w_p_stlo_mh = -1;
    }

    திரும்பு வெற்று;
}

    முழு
expand_set_statuslineopt(optexpand_T *args, முழு *இலக்கம்_Matches, char_u ***matches)
{
    திரும்பு expand_set_opt_string(
	    args,
	    p_stlo_மதிப்புகள்,
	    ARRAY_LENGTH(p_stlo_மதிப்புகள்) - 1,
	    இலக்கம்_Matches,
	    matches);
}
#endif

/*
 * The 'swapsync' option is changed.
 */
    எழுத்து *
did_set_swapsync(optset_T *args UNUSED)
{
    திரும்பு did_set_opt_strings(p_sws, p_sws_மதிப்புகள், FALSE);
}

    முழு
expand_set_swapsync(optexpand_T *args, முழு *இலக்கம்_Matches, char_u ***matches)
{
    திரும்பு expand_set_opt_string(
	    args,
	    p_sws_மதிப்புகள்,
	    ARRAY_LENGTH(p_sws_மதிப்புகள்) - 1,
	    இலக்கம்_Matches,
	    matches);
}

/*
 * The 'switchbuf' option is changed.
 */
    எழுத்து *
did_set_switchbuf(optset_T *args UNUSED)
{
    திரும்பு did_set_opt_flags(p_swb, p_swb_மதிப்புகள், &swb_flags, TRUE);
}

    முழு
expand_set_switchbuf(optexpand_T *args, முழு *இலக்கம்_Matches, char_u ***matches)
{
    திரும்பு expand_set_opt_string(
	    args,
	    p_swb_மதிப்புகள்,
	    ARRAY_LENGTH(p_swb_மதிப்புகள்) - 1,
	    இலக்கம்_Matches,
	    matches);
}

/*
 * The 'tabclose' option is changed.
 */
    எழுத்து *
did_set_tabclose(optset_T *args UNUSED)
{
    திரும்பு did_set_opt_flags(p_tcl, p_tcl_மதிப்புகள், &tcl_flags, TRUE);
}

    முழு
expand_set_tabclose(optexpand_T *args, முழு *இலக்கம்_Matches, char_u ***matches)
{
    திரும்பு expand_set_opt_string(
	    args,
	    p_tcl_மதிப்புகள்,
	    ARRAY_LENGTH(p_tcl_மதிப்புகள்) - 1,
	    இலக்கம்_Matches,
	    matches);
}

#எனில் defined(FEAT_STL_OPT)
/*
 * The 'tabline' option is changed.
 */
    எழுத்து *
did_set_tabline(optset_T *args)
{
    திரும்பு parse_statustabline_rulerformat(args, FALSE);
}
#endif

/*
 * The 'tagcase' option is changed.
 */
    எழுத்து *
did_set_tagcase(optset_T *args)
{
    unsigned முழு	*flags;
    char_u		*p;

    எனில் (args->os_flags & OPT_LOCAL)
    {
	p = curbuf->b_p_tc;
	flags = &curbuf->b_tc_flags;
    }
    இல்லையெனில்
    {
	p = p_tc;
	flags = &tc_flags;
    }

    எனில் ((args->os_flags & OPT_LOCAL) && *p == NUL)
	// make the local value empty: use the global value
	*flags = 0;
    இல்லையெனில் எனில் (*p == NUL
	    || opt_strings_flags(p, p_tc_மதிப்புகள், flags, FALSE) != OK)
	திரும்பு e_invalid_argument;

    திரும்பு வெற்று;
}

    முழு
expand_set_tagcase(optexpand_T *args, முழு *இலக்கம்_Matches, char_u ***matches)
{
    திரும்பு expand_set_opt_string(
	    args,
	    p_tc_மதிப்புகள்,
	    ARRAY_LENGTH(p_tc_மதிப்புகள்) - 1,
	    இலக்கம்_Matches,
	    matches);
}

/*
 * The 'term' option is changed.
 */
    எழுத்து *
did_set_term(optset_T *args UNUSED)
{
    எனில் (T_NAME[0] == NUL)
	திரும்பு e_cannot_set_term_to_empty_string;
#ifdef FEAT_GUI
    எனில் (gui.in_use)
	திரும்பு e_cannot_change_term_in_GUI;
    எனில் (term_is_gui(T_NAME))
	திரும்பு e_use_gui_to_start_GUI;
#endif
    எனில் (set_termname(T_NAME) == FAIL)
	திரும்பு e_not_found_in_termcap;

    // Screen colors may have changed.
    redraw_later_clear();

    திரும்பு வெற்று;
}

/*
 * Some terminal option (t_xxx) is changed
 */
    எழுத்து *
did_set_term_option(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    எனில் (!full_screen)
	திரும்பு வெற்று;

    // ":set t_Co=0" and ":set t_Co=1" do ":set t_Co="
    எனில் (varp == &T_CCO)
    {
	முழு colors = atoi((எழுத்து *)T_CCO);

	// Only reinitialize colors if t_Co value has really changed to
	// avoid expensive reload of colorscheme if t_Co is set to the
	// same value multiple times.
	எனில் (colors != t_colors)
	{
	    t_colors = colors;
	    எனில் (t_colors <= 1)
	    {
		vim_free(T_CCO);
		T_CCO = empty_option;
	    }
#எனில் defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
	    எனில் (is_term_win32())
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
    எனில் (varp == &T_ME)
    {
	out_str(T_ME);
	redraw_later(UPD_CLEAR);
#எனில் defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
	// Since t_me has been set, this probably means that the user
	// wants to use this as default colors.  Need to reset default
	// background/foreground colors.
# ifdef VIMDLL
	எனில் (!gui.in_use && !gui.starting)
# endif
	    mch_set_normal_colors();
#endif
    }
    எனில் (varp == &T_BE && termcap_active)
    {
	MAY_WANT_TO_LOG_THIS;

	எனில் (*T_BE == NUL)
	    // When clearing t_BE we assume the user no longer wants
	    // bracketed paste, thus disable it by writing t_BD.
	    out_str(T_BD);
	இல்லையெனில்
	    out_str(T_BE);
    }

    எனில் (varp == &T_BSU || varp == &T_ESU)
	term_set_sync_output(TERM_SYNC_OUTPUT_OFF);

    திரும்பு வெற்று;
}


#ifdef UNIX
/*
 * The 'termresize' option is changed.
 */
    எழுத்து *
did_set_termresize(optset_T *args UNUSED)
{
    // If empty or "inband", then attempt to enable in-band resize events.
    எனில் (*p_trz == NUL || STRCMP(p_trz, "inband") == 0)
	term_set_win_resize(true);
    இல்லையெனில் எனில் (STRCMP(p_trz, "sigwinch") == 0)
	term_set_win_resize(false);
    இல்லையெனில்
	திரும்பு e_invalid_argument;

    திரும்பு வெற்று;
}

    முழு
expand_set_termresize(optexpand_T *args, முழு *இலக்கம்_Matches, char_u ***matches)
{
    திரும்பு expand_set_opt_string(
	    args,
	    p_trz_மதிப்புகள்,
	    ARRAY_LENGTH(p_trz_மதிப்புகள்) - 1,
	    இலக்கம்_Matches,
	    matches);
}
#endif

#எனில் defined(FEAT_TERMINAL)
/*
 * The 'termwinkey' option is changed.
 */
    எழுத்து *
did_set_termwinkey(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    எனில் ((*varp)[0] != NUL && string_to_சாவி(*varp, TRUE) == 0)
	திரும்பு e_invalid_argument;

    திரும்பு வெற்று;
}

/*
 * The 'termwinsize' option is changed.
 */
    எழுத்து *
did_set_termwinsize(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u	*p;

    எனில் ((*varp)[0] == NUL)
	திரும்பு வெற்று;

    p = skipdigits(*varp);
    எனில் (p == *varp || (*p != 'x' && *p != '*') || *skipdigits(p + 1) != NUL)
	திரும்பு e_invalid_argument;

    திரும்பு வெற்று;
}

# எனில் defined(MSWIN)
/*
 * The 'termwintype' option is changed.
 */
    எழுத்து *
did_set_termwintype(optset_T *args UNUSED)
{
    திரும்பு did_set_opt_strings(p_twt, p_twt_மதிப்புகள், FALSE);
}

    முழு
expand_set_termwintype(optexpand_T *args, முழு *இலக்கம்_Matches, char_u ***matches)
{
    திரும்பு expand_set_opt_string(
	    args,
	    p_twt_மதிப்புகள்,
	    ARRAY_LENGTH(p_twt_மதிப்புகள்) - 1,
	    இலக்கம்_Matches,
	    matches);
}
# endif
#endif

/*
 * The 'titlestring' option is changed.
 */
    எழுத்து *
did_set_titlestring(optset_T *args)
{
    முழு flagval = 0;

#ifdef FEAT_STL_OPT
    flagval = STL_IN_TITLE;
#endif
    திரும்பு parse_titleiconstring(args, flagval);
}

#எனில் defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
/*
 * The 'toolbar' option is changed.
 */
    எழுத்து *
did_set_toolbar(optset_T *args UNUSED)
{
    எனில் (opt_strings_flags(p_toolbar, p_toolbar_மதிப்புகள், &toolbar_flags,
								TRUE) != OK)
	திரும்பு e_invalid_argument;

    out_flush();
    gui_mch_show_toolbar((toolbar_flags &
		(TOOLBAR_TEXT | TOOLBAR_ICONS)) != 0);
    திரும்பு வெற்று;
}

    முழு
expand_set_toolbar(optexpand_T *args, முழு *இலக்கம்_Matches, char_u ***matches)
{
    திரும்பு expand_set_opt_string(
	    args,
	    p_toolbar_மதிப்புகள்,
	    ARRAY_LENGTH(p_toolbar_மதிப்புகள்) - 1,
	    இலக்கம்_Matches,
	    matches);
}
#endif

#எனில் defined(FEAT_TOOLBAR) && defined(FEAT_GUI_GTK)
/*
 * The 'toolbariconsize' option is changed.  GTK+ 2 only.
 */
    எழுத்து *
did_set_toolbariconsize(optset_T *args UNUSED)
{
    எனில் (opt_strings_flags(p_tbis, p_tbis_மதிப்புகள், &tbis_flags, FALSE) != OK)
	திரும்பு e_invalid_argument;

    out_flush();
    gui_mch_show_toolbar((toolbar_flags &
		(TOOLBAR_TEXT | TOOLBAR_ICONS)) != 0);
    திரும்பு வெற்று;
}

    முழு
expand_set_toolbariconsize(
    optexpand_T	*args,
    முழு		*இலக்கம்_Matches,
    char_u	***matches)
{
    திரும்பு expand_set_opt_string(
	    args,
	    p_tbis_மதிப்புகள்,
	    ARRAY_LENGTH(p_tbis_மதிப்புகள்) - 1,
	    இலக்கம்_Matches,
	    matches);
}
#endif

#எனில் defined(UNIX) || defined(VMS)
/*
 * The 'ttymouse' option is changed.
 */
    எழுத்து *
did_set_ttymouse(optset_T *args UNUSED)
{
    எழுத்து *errmsg = வெற்று;

    // Switch the mouse off before changing the escape sequences used for
    // that.
    mch_setmouse(FALSE);
    எனில் (opt_strings_flags(p_ttym, p_ttym_மதிப்புகள், &ttym_flags, FALSE) != OK)
	errmsg = e_invalid_argument;
    இல்லையெனில்
	check_mouse_termcode();
    எனில் (termcap_active)
	setmouse();		// may switch it on again

    திரும்பு errmsg;
}

    முழு
expand_set_ttymouse(optexpand_T *args, முழு *இலக்கம்_Matches, char_u ***matches)
{
    திரும்பு expand_set_opt_string(
	    args,
	    p_ttym_மதிப்புகள்,
	    ARRAY_LENGTH(p_ttym_மதிப்புகள்) - 1,
	    இலக்கம்_Matches,
	    matches);
}
#endif

#எனில் defined(FEAT_VARTABS)
/*
 * The 'varsofttabstop' option is changed.
 */
    எழுத்து *
did_set_varsofttabstop(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u *cp;

    எனில் (!((*varp)[0]) || ((*varp)[0] == '0' && !((*varp)[1])))
	VIM_CLEAR(curbuf->b_p_vsts_array);
    இல்லையெனில்
    {
	க்கு (cp = *varp; *cp; ++cp)
	{
	    எனில் (vim_isdigit(*cp))
		தொடர்;
	    எனில் (*cp == ',' && cp > *varp && *(cp-1) != ',')
		தொடர்;
	    திரும்பு e_invalid_argument;
	}

	முழு *oldarray = curbuf->b_p_vsts_array;
	எனில் (tabstop_set(*varp, &(curbuf->b_p_vsts_array)) == OK)
	{
	    எனில் (oldarray)
		vim_free(oldarray);
	}
	இல்லையெனில்
	    திரும்பு e_invalid_argument;
    }

    திரும்பு வெற்று;
}

/*
 * The 'vartabstop' option is changed.
 */
    எழுத்து *
did_set_vartabstop(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u *cp;

    எனில் (!((*varp)[0]) || ((*varp)[0] == '0' && !((*varp)[1])))
	VIM_CLEAR(curbuf->b_p_vts_array);
    இல்லையெனில்
    {
	க்கு (cp = *varp; *cp; ++cp)
	{
	    எனில் (vim_isdigit(*cp))
		தொடர்;
	    எனில் (*cp == ',' && cp > *varp && *(cp-1) != ',')
		தொடர்;
	    திரும்பு e_invalid_argument;
	}

	முழு *oldarray = curbuf->b_p_vts_array;

	எனில் (tabstop_set(*varp, &(curbuf->b_p_vts_array)) == OK)
	{
	    vim_free(oldarray);
# ifdef FEAT_FOLDING
	    எனில் (foldmethodIsIndent(curwin))
		foldUpdateAll(curwin);
# endif
	}
	இல்லையெனில்
	    திரும்பு e_invalid_argument;
    }

    திரும்பு வெற்று;
}
#endif

/*
 * The 'verbosefile' option is changed.
 */
    எழுத்து *
did_set_verbosefile(optset_T *args UNUSED)
{
    verbose_stop();
    எனில் (*p_vfile != NUL && verbose_open() == FAIL)
	திரும்பு e_invalid_argument;

    திரும்பு வெற்று;
}

#எனில் defined(FEAT_SESSION)
/*
 * The 'viewoptions' option is changed.
 */
    எழுத்து *
did_set_viewoptions(optset_T *args UNUSED)
{
    திரும்பு did_set_opt_flags(p_vop, p_ssop_மதிப்புகள், &vop_flags, TRUE);
}
#endif

#எனில் defined(FEAT_VIMINFO)
/*
 * The 'viminfo' option is changed.
 */
    எழுத்து *
did_set_viminfo(optset_T *args)
{
    char_u	*s;
    எழுத்து	*errmsg = வெற்று;

    க்கு (s = p_viminfo; *s;)
    {
	// Check it's a valid character
	எனில் (vim_strchr((char_u *)"!\"%'/:<@cfhnrs", *s) == வெற்று)
	{
	    errmsg = illegal_char(args->os_errbuf, args->os_errbuflen, *s);
	    நிறுத்து;
	}
	எனில் (*s == 'n')	// name is always last one
	    நிறுத்து;
	இல்லையெனில் எனில் (*s == 'r') // skip until next ','
	{
	    போது (*++s && *s != ',')
		;
	}
	இல்லையெனில் எனில் (*s == '%')
	{
	    // optional number
	    போது (vim_isdigit(*++s))
		;
	}
	இல்லையெனில் எனில் (*s == '!' || *s == 'h' || *s == 'c')
	    ++s;		// no extra chars
	இல்லையெனில்		// must have a number
	{
	    போது (vim_isdigit(*++s))
		;

	    எனில் (!VIM_ISDIGIT(*(s - 1)))
	    {
		எனில் (args->os_errbuf != வெற்று)
		{
		    vim_snprintf(args->os_errbuf, args->os_errbuflen,
			    _(e_missing_எண்_after_angle_str_angle),
			    transchar_byte(*(s - 1)));
		    errmsg = args->os_errbuf;
		}
		இல்லையெனில்
		    errmsg = "";
		நிறுத்து;
	    }
	}
	எனில் (*s == ',')
	    ++s;
	இல்லையெனில் எனில் (*s)
	{
	    எனில் (args->os_errbuf != வெற்று)
		errmsg = e_missing_comma;
	    இல்லையெனில்
		errmsg = "";
	    நிறுத்து;
	}
    }
    எனில் (*p_viminfo && errmsg == வெற்று && get_viminfo_parameter('\'') < 0)
	errmsg = e_must_specify_a_மதிப்பு;

    திரும்பு errmsg;
}
#endif

/*
 * The 'virtualedit' option is changed.
 */
    எழுத்து *
did_set_virtualedit(optset_T *args)
{
    char_u		*ve = p_ve;
    unsigned முழு	*flags = &ve_flags;

    எனில் (args->os_flags & OPT_LOCAL)
    {
	ve = curwin->w_p_ve;
	flags = &curwin->w_ve_flags;
    }

    எனில் ((args->os_flags & OPT_LOCAL) && *ve == NUL)
	// make the local value empty: use the global value
	*flags = 0;
    இல்லையெனில்
    {
	எனில் (opt_strings_flags(ve, p_ve_மதிப்புகள், flags, TRUE) != OK)
	    திரும்பு e_invalid_argument;
	இல்லையெனில் எனில் (STRCMP(ve, args->os_oldval.string) != 0)
	{
	    // Recompute cursor position in case the new 've' setting
	    // changes something.
	    validate_virtcol();
	    coladvance(curwin->w_virtcol);
	}
    }

    திரும்பு வெற்று;
}

    முழு
expand_set_virtualedit(optexpand_T *args, முழு *இலக்கம்_Matches, char_u ***matches)
{
    திரும்பு expand_set_opt_string(
	    args,
	    p_ve_மதிப்புகள்,
	    ARRAY_LENGTH(p_ve_மதிப்புகள்) - 1,
	    இலக்கம்_Matches,
	    matches);
}

/*
 * The 'whichwrap' option is changed.
 */
    எழுத்து *
did_set_whichwrap(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    // Add ',' to the list flags because 'whichwrap' is a flag
    // list that is comma-separated.
    திரும்பு did_set_option_listflag(*varp, (char_u *)(WW_ALL ","),
		    args->os_errbuf, args->os_errbuflen);
}

    முழு
expand_set_whichwrap(optexpand_T *args, முழு *இலக்கம்_Matches, char_u ***matches)
{
    திரும்பு expand_set_opt_listflag(args, (char_u*)WW_ALL, இலக்கம்_Matches, matches);
}

/*
 * The 'wildmode' option is changed.
 */
    எழுத்து *
did_set_wildmode(optset_T *args UNUSED)
{
    எனில் (check_opt_wim() == FAIL)
	திரும்பு e_invalid_argument;
    திரும்பு வெற்று;
}

    முழு
expand_set_wildmode(optexpand_T *args, முழு *இலக்கம்_Matches, char_u ***matches)
{
    திரும்பு expand_set_opt_string(
	    args,
	    p_wim_மதிப்புகள்,
	    ARRAY_LENGTH(p_wim_மதிப்புகள்) - 1,
	    இலக்கம்_Matches,
	    matches);
}

/*
 * The 'wildoptions' option is changed.
 */
    எழுத்து *
did_set_wildoptions(optset_T *args UNUSED)
{
    திரும்பு did_set_opt_strings(p_wop, p_wop_மதிப்புகள், TRUE);
}

    முழு
expand_set_wildoptions(optexpand_T *args, முழு *இலக்கம்_Matches, char_u ***matches)
{
    திரும்பு expand_set_opt_string(
	    args,
	    p_wop_மதிப்புகள்,
	    ARRAY_LENGTH(p_wop_மதிப்புகள்) - 1,
	    இலக்கம்_Matches,
	    matches);
}

#எனில் defined(FEAT_WAK)
/*
 * The 'winaltkeys' option is changed.
 */
    எழுத்து *
did_set_winaltkeys(optset_T *args UNUSED)
{
    எழுத்து *errmsg = வெற்று;

    எனில் (*p_wak == NUL || check_opt_strings(p_wak, p_wak_மதிப்புகள், FALSE) != OK)
	errmsg = e_invalid_argument;
# ifdef FEAT_MENU
#  எனில் defined(FEAT_GUI_MOTIF)
    இல்லையெனில் எனில் (gui.in_use)
	gui_motif_set_mnemonics(p_wak[0] == 'y' || p_wak[0] == 'm');
#  elif defined(FEAT_GUI_GTK)
    இல்லையெனில் எனில் (gui.in_use)
	gui_gtk_set_mnemonics(p_wak[0] == 'y' || p_wak[0] == 'm');
#  endif
# endif
    திரும்பு errmsg;
}

    முழு
expand_set_winaltkeys(optexpand_T *args, முழு *இலக்கம்_Matches, char_u ***matches)
{
    திரும்பு expand_set_opt_string(
	    args,
	    p_wak_மதிப்புகள்,
	    ARRAY_LENGTH(p_wak_மதிப்புகள்) - 1,
	    இலக்கம்_Matches,
	    matches);
}
#endif

/*
 * The 'wincolor' option is changed.
 */
    எழுத்து *
did_set_wincolor(optset_T *args UNUSED)
{
    update_wincolor(curwin, args->os_newval.string);
    திரும்பு வெற்று;
}

/*
 * The 'winhighlight' option is changed
 */
    எழுத்து *
did_set_winhighlight(optset_T *args)
{
    திரும்பு update_winhighlight(curwin, args->os_newval.string);
}

/*
 * Expand 'winhighlight' option.
 */
    முழு
expand_set_winhighlight(optexpand_T *args, முழு *இலக்கம்_Matches, char_u ***matches)
{
    expand_T	    *xp = args->oe_xp;

    எனில் ((xp->xp_pattern > args->oe_set_arg && *(xp->xp_pattern-1) == ':')
	    || xp->xp_pattern == args->oe_set_arg || *(xp->xp_pattern-1) == ',')
    {
	// After a ':' or after a ',', or at the start, so expand highlight
	// group name.

	// If starts with !, then expand 'highlight' occasions.
	எனில் (*xp->xp_pattern == '!')
	    திரும்பு expand_hl_occasions(args, இலக்கம்_Matches, matches, '!');
	இல்லையெனில்
	    திரும்பு expand_set_opt_generic(
		    args,
		    get_highlight_name,
		    இலக்கம்_Matches,
		    matches);
    }

    VIM_CLEAR(*matches);
    திரும்பு FAIL;
}

    முழு
expand_set_wincolor(optexpand_T *args, முழு *இலக்கம்_Matches, char_u ***matches)
{
    திரும்பு expand_set_opt_generic(
	    args,
	    get_highlight_name,
	    இலக்கம்_Matches,
	    matches);
}

#ifdef FEAT_SYN_HL
/*
 * When the 'syntax' option is set, load the syntax of that name.
 */
    நிலையான வெறுமை
do_syntax_autocmd(முழு மதிப்பு_changed)
{
    நிலையான முழு syn_recursive = 0;

    ++syn_recursive;
    // Only pass TRUE for "force" when the value changed or not used
    // recursively, to avoid endless recurrence.
    apply_autocmds(EVENT_SYNTAX, curbuf->b_p_syn, curbuf->b_fname,
	    மதிப்பு_changed || syn_recursive == 1, curbuf);
    curbuf->b_flags |= BF_SYN_SET;
    --syn_recursive;
}
#endif

/*
 * When the 'filetype' option is set, trigger the FileType autocommand.
 */
    நிலையான வெறுமை
do_filetype_autocmd(char_u **varp, முழு opt_flags, முழு மதிப்பு_changed)
{
    // Skip this when called from a modeline and the filetype was already set
    // to this value.
    எனில் ((opt_flags & OPT_MODELINE) && !மதிப்பு_changed)
	திரும்பு;

    நிலையான முழு  ft_recursive = 0;
    முழு	    secure_save = secure;

    // Reset the secure flag, since the value of 'filetype' has
    // been checked to be safe.
    secure = 0;

    ++ft_recursive;
    curbuf->b_did_filetype = true;
    // Only pass TRUE for "force" when the value changed or not
    // used recursively, to avoid endless recurrence.
    apply_autocmds(EVENT_FILETYPE, curbuf->b_p_ft, curbuf->b_fname,
	    மதிப்பு_changed || ft_recursive == 1, curbuf);
    --ft_recursive;
    // Just in case the old "curbuf" is now invalid.
    எனில் (varp != &(curbuf->b_p_ft))
	varp = வெற்று;

    secure = secure_save;
}

#ifdef FEAT_SPELL
/*
 * When the 'spelllang' option is set, source the spell/LANG.vim file in
 * 'runtimepath'.
 */
    நிலையான வெறுமை
do_spelllang_source(வெறுமை)
{
    char_u	fname[200];
    char_u	*p;
    char_u	*q = curwin->w_s->b_p_spl;

    // Skip the first name if it is "cjk".
    எனில் (STRNCMP(q, "cjk,", 4) == 0)
	q += 4;

    // They could set 'spellcapcheck' depending on the language.  Use the first
    // name in 'spelllang' up to '_region' or '.encoding'.
    க்கு (p = q; *p != NUL; ++p)
	எனில் (!ASCII_ISALNUM(*p) && *p != '-')
	    நிறுத்து;
    எனில் (p > q)
    {
	vim_snprintf((எழுத்து *)fname, 200, "spell/%.*s.vim",
		(முழு)(p - q), q);
	source_runtime(fname, DIP_ALL);
    }
}
#endif

/*
 * Handle string options that need some action to perform when changed.
 * The new value must be allocated.
 * Returns NULL for success, or an untranslated error message for an error.
 */
    எழுத்து *
did_set_string_option(
    முழு		opt_idx,		// index in options[] table
    char_u	**varp,			// pointer to the option variable
    char_u	*oldval,		// previous value of the option
    char_u	*மதிப்பு,			// new value of the option
    எழுத்து	*errbuf,		// buffer for errors, or NULL
    size_t	errbuflen,		// length of error buffer
    முழு		opt_flags,		// OPT_LOCAL and/or OPT_GLOBAL
    set_op_T    op,			// OP_ADDING/OP_PREPENDING/OP_REMOVING
    முழு		*மதிப்பு_checked)		// value was checked to be safe, no
					// need to set P_INSECURE
{
    எழுத்து	*errmsg = வெற்று;
    long_u	free_oldval = (get_option_flags(opt_idx) & P_ALLOCED);
    opt_did_set_cb_T did_set_cb = get_option_did_set_cb(opt_idx);
    optset_T	args;

    // 'ttytype' is an alias for 'term'.  Both 'term' and 'ttytype' point to
    // T_NAME.  If 'term' or 'ttytype' is modified, then use the index for the
    // 'term' option.  Only set the P_ALLOCED flag on 'term'.
    எனில் (varp == &T_NAME)
    {
	opt_idx = findoption((char_u *)"term");
	எனில் (opt_idx >= 0)
	{
	    free_oldval = (get_option_flags(opt_idx) & P_ALLOCED);
	    did_set_cb = get_option_did_set_cb(opt_idx);
	}
    }

    CLEAR_FIELD(args);

    // Disallow changing some options from secure mode
    எனில் ((secure
#ifdef HAVE_SANDBOX
		|| sandbox != 0
#endif
		) && (get_option_flags(opt_idx) & P_SECURE))
	errmsg = e_not_allowed_here;
    // Check for a "normal" directory or file name in some options.
    இல்லையெனில் எனில் (check_illegal_path_names(opt_idx, varp))
	errmsg = e_invalid_argument;
    இல்லையெனில் எனில் (did_set_cb != வெற்று)
    {
	args.os_varp = (char_u *)varp;
	args.os_idx = opt_idx;
	args.os_flags = opt_flags;
	args.os_op = op;
	args.os_oldval.string = oldval;
	args.os_newval.string = மதிப்பு;
	args.os_errbuf = errbuf;
	args.os_errbuflen = errbuflen;
	// Invoke the option specific callback function to validate and apply
	// the new option value.
	errmsg = did_set_cb(&args);

	// The 'keymap', 'filetype' and 'syntax' option callback functions
	// may change the os_value_checked field.
	*மதிப்பு_checked = args.os_மதிப்பு_checked;
    }

    // If an error is detected, restore the previous value.
    எனில் (errmsg != வெற்று)
    {
	free_string_option(*varp);
	*varp = oldval;
	// When resetting some values, need to act on it.
	எனில் (args.os_restore_chartab)
	    (வெறுமை)init_chartab();
	எனில் (varp == &p_hl)
	    (வெறுமை)highlight_changed();
    }
    இல்லையெனில்
    {
#ifdef FEAT_EVAL
	// Remember where the option was set.
	set_option_sctx_idx(opt_idx, opt_flags, current_sctx);
#endif
	// Free string options that are in allocated memory.
	// Use "free_oldval", because recursiveness may change the flags under
	// our fingers (esp. init_highlight()).
	எனில் (free_oldval)
	    free_string_option(oldval);
	set_option_flag(opt_idx, P_ALLOCED);

	எனில் ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0
		&& is_global_local_option(opt_idx))
	{
	    // global option with local value set to use global value; free
	    // the local value and make it empty
	    char_u *p = get_option_varp_scope(opt_idx, OPT_LOCAL);
	    free_string_option(*(char_u **)p);
	    *(char_u **)p = empty_option;
	}

	// May set global value for local option.
	இல்லையெனில் எனில் (!(opt_flags & OPT_LOCAL) && opt_flags != OPT_GLOBAL)
	    set_string_option_global(opt_idx, varp);

	// Trigger the autocommand only after setting the flags.
#ifdef FEAT_SYN_HL
	எனில் (varp == &(curbuf->b_p_syn))
	    do_syntax_autocmd(args.os_மதிப்பு_changed);
#endif
	இல்லையெனில் எனில் (varp == &(curbuf->b_p_ft))
	    do_filetype_autocmd(varp, opt_flags, args.os_மதிப்பு_changed);
#ifdef FEAT_SPELL
	எனில் (varp == &(curwin->w_s->b_p_spl))
	    do_spelllang_source();
#endif
    }

    எனில் (varp == &p_mouse)
    {
	எனில் (*p_mouse == NUL)
	    mch_setmouse(FALSE);    // switch mouse off
	இல்லையெனில்
	    setmouse();		    // in case 'mouse' changed
    }

    எனில் (varp == &p_rtp)
    {
	export_myvimdir();
#எனில் defined(FEAT_LUA)
	update_package_paths_in_lua();
#endif
    }

#எனில் defined(FEAT_LINEBREAK)
    // Changing Formatlistpattern when briopt includes the list setting:
    // redraw
    எனில் ((varp == &p_flp || varp == &(curbuf->b_p_flp))
	    && curwin->w_briopt_list)
	redraw_all_later(UPD_NOT_VALID);
#endif

    எனில் (curwin->w_curswant != MAXCOL
		   && (get_option_flags(opt_idx) & (P_CURSWANT | P_RALL)) != 0
				&& (get_option_flags(opt_idx) & P_HLONLY) == 0)
	curwin->w_set_curswant = true;

    எனில் ((opt_flags & OPT_NO_REDRAW) == 0)
    {
#ifdef FEAT_GUI
	// set when changing an option that only requires a redraw in the GUI
	முழு	redraw_gui_only = FALSE;

	எனில் (varp == &p_go			// 'guioptions'
		|| varp == &p_guifont		// 'guifont'
# ifdef FEAT_GUI_TABLINE
		|| varp == &p_gtl		// 'guitablabel'
		|| varp == &p_gtt		// 'guitabtooltip'
# endif
# ifdef FEAT_XFONTSET
		|| varp == &p_guifontset	// 'guifontset'
# endif
		|| varp == &p_guifontwide	// 'guifontwide'
# எனில் defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
		|| varp == &p_guiligatures	// 'guiligatures'
# endif
	    )
	    redraw_gui_only = TRUE;

	// check redraw when it's not a GUI option or the GUI is active.
	எனில் (!redraw_gui_only || gui.in_use)
#endif
	    check_redraw(get_option_flags(opt_idx));
    }

#எனில் defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
    எனில் (args.os_did_swaptcap)
    {
	set_termname((char_u *)"win32");
	init_highlight(TRUE, FALSE);
    }
#endif

    திரும்பு errmsg;
}

/*
 * Check an option that can be a range of string values.
 *
 * Return OK for correct value, FAIL otherwise.
 * Empty is always OK.
 */
    நிலையான முழு
check_opt_strings(
    char_u	*val,
    எழுத்து	**மதிப்புகள்,
    முழு		list)	    // when TRUE: accept a list of values
{
    திரும்பு opt_strings_flags(val, மதிப்புகள், வெற்று, list);
}

/*
 * Handle an option that can be a range of string values.
 * Set a flag in "*flagp" for each string present.
 *
 * Return OK for correct value, FAIL otherwise.
 * Empty is always OK.
 */
    நிலையான முழு
opt_strings_flags(
    char_u	*val,		// new value
    எழுத்து	**மதிப்புகள்,	// array of valid string values
    unsigned	*flagp,
    முழு		list)		// when TRUE: accept a list of values
{
    முழு		i;
    முழு		len;
    unsigned	new_flags = 0;

    போது (*val)
    {
	க்கு (i = 0; ; ++i)
	{
	    எனில் (மதிப்புகள்[i] == வெற்று)	// val not found in values[]
		திரும்பு FAIL;

	    len = (முழு)STRLEN(மதிப்புகள்[i]);
	    எனில் (STRNCMP(மதிப்புகள்[i], val, len) == 0
		    && ((list && val[len] == ',') || val[len] == NUL))
	    {
		val += len + (val[len] == ',');
		new_flags |= (1 << i);
		நிறுத்து;		// check next item in val list
	    }
	}
    }
    எனில் (flagp != வெற்று)
	*flagp = new_flags;

    திரும்பு OK;
}

/*
 * return OK if "p" is a valid fileformat name, FAIL otherwise.
 */
    முழு
check_ff_மதிப்பு(char_u *p)
{
    திரும்பு check_opt_strings(p, p_ff_மதிப்புகள், FALSE);
}

/*
 * Save the actual shortmess Flags and clear them temporarily to avoid that
 * file messages overwrites any output from the following commands.
 *
 * Caller must make sure to first call save_clear_shm_value() and then
 * restore_shm_value() exactly the same number of times.
 */
    வெறுமை
save_clear_shm_மதிப்பு(வெறுமை)
{
    எனில் (STRLEN(p_shm) >= SHM_LEN)
    {
	iemsg(e_internal_error_shortmess_too_long);
	திரும்பு;
    }

    எனில் (++set_shm_recursive == 1)
    {
	STRCPY(shm_buf, p_shm);
	set_option_மதிப்பு_give_err((char_u *)"shm", 0L, (char_u *)"", 0);
    }
}

/*
 * Restore the shortmess Flags set from the save_clear_shm_value() function.
 */
    வெறுமை
restore_shm_மதிப்பு(வெறுமை)
{
    எனில் (--set_shm_recursive == 0)
    {
	set_option_மதிப்பு_give_err((char_u *)"shm", 0L, shm_buf, 0);
	vim_memset(shm_buf, 0, SHM_LEN);
    }
}

/*
 * Export the environment variable $MYVIMDIR to the first item in runtimepath
 */
    வெறுமை
export_myvimdir(வெறுமை)
{
    முழு		dofree = FALSE;
    char_u	*p;
    char_u	*q = p_rtp;
    string_T	buf;

    buf.string = alloc(MAXPATHL);
    எனில் (buf.string == வெற்று)
	திரும்பு;

    buf.length = (size_t)copy_option_part(&q, buf.string, MAXPATHL, ",");

    p = vim_getenv((char_u *)"MYVIMDIR", &dofree);

    எனில் (p == வெற்று || STRCMP(p, buf.string) != 0)
    {
	எனில் (*buf.string != NUL && !after_pathsep(buf.string, buf.string + buf.length))
	    STRCPY(buf.string + buf.length, PATHSEPSTR);
#ifdef MSWIN
	// normalize path separators
	க்கு (q = buf.string; *q != NUL; q++)
	    எனில் (*q == '/')
		*q = '\\';
#endif
	vim_setenv((char_u *)"MYVIMDIR", buf.string);
    }
    எனில் (dofree)
	vim_free(p);
    vim_free(buf.string);
}
