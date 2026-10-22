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

#અન્તર્ભાવ "vim.h"

સ્થિર char_u shm_buf[SHM_LEN];
સ્થિર પૂર્ણાંક set_shm_recursive = 0;

સ્થિર અક્ષર *(p_ambw_કિંમતો[]) = {"single", "double", ખાલી};
સ્થિર અક્ષર *(p_bg_કિંમતો[]) = {"light", "dark", ખાલી};
સ્થિર અક્ષર *(p_bkc_કિંમતો[]) = {"yes", "auto", "no", "breaksymlink", "breakhardlink", ખાલી};
સ્થિર અક્ષર *(p_bo_કિંમતો[]) = {"all", "backspace", "cursor", "complete",
				 "copy", "ctrlg", "error", "esc", "ex",
				 "hangul", "insertmode", "lang", "mess",
				 "showmatch", "operator", "register", "shell",
				 "spell", "term", "wildmode", ખાલી};
#જો defined(FEAT_LINEBREAK)
// Note: Keep this in sync with briopt_check()
સ્થિર અક્ષર *(p_briopt_કિંમતો[]) = {"shift:", "min:", "sbr", "list:", "column:", ખાલી};
#endif
#જો defined(FEAT_TABPANEL)
// Note: Keep this in sync with tabpanelopt_changed()
સ્થિર અક્ષર *(p_tplo_કિંમતો[]) = {"align:", "columns:", "scrollbar", "vert", ખાલી};
સ્થિર અક્ષર *(p_tplo_align_કિંમતો[]) = {"left", "right", ખાલી};
#endif
#જો defined(FEAT_DIFF)
// Note: Keep this in sync with diffopt_changed()
સ્થિર અક્ષર *(p_dip_કિંમતો[]) = {"filler", "anchor", "context:", "iblank", "icase", "iwhite", "iwhiteall", "iwhiteeol", "horizontal", "vertical", "closeoff", "hiddenoff", "foldcolumn:", "followwrap", "internal", "indent-heuristic", "algorithm:", "inline:", "linematch:", ખાલી};
સ્થિર અક્ષર *(p_dip_algorithm_કિંમતો[]) = {"myers", "minimal", "patience", "histogram", ખાલી};
સ્થિર અક્ષર *(p_dip_inline_કિંમતો[]) = {"none", "simple", "char", "word", ખાલી};
#endif
સ્થિર અક્ષર *(p_nf_કિંમતો[]) = {"bin", "octal", "hex", "alpha", "unsigned", "blank", ખાલી};
સ્થિર અક્ષર *(p_ff_કિંમતો[]) = {FF_UNIX, FF_DOS, FF_MAC, ખાલી};
#ifdef HAVE_CLIPMETHOD
// Note: Keep this in sync with did_set_clipboard()
સ્થિર અક્ષર *(p_cb_કિંમતો[]) = {"unnamed", "unnamedplus",
# ifdef FEAT_CLIPBOARD
    "autoselect", "autoselectplus", "autoselectml", "html", "exclude:",
# endif
    ખાલી};
#endif
#ifdef FEAT_CRYPT
સ્થિર અક્ષર *(p_cm_કિંમતો[]) = {"zip", "blowfish", "blowfish2",
# ifdef FEAT_SODIUM
    "xchacha20", "xchacha20v2",
# endif
    ખાલી};
#endif
સ્થિર અક્ષર *(p_cmp_કિંમતો[]) = {"internal", "keepascii", ખાલી};
#ifdef FEAT_SYN_HL
// Note: Keep this in sync with fill_culopt_flags()
સ્થિર અક્ષર *(p_culopt_કિંમતો[]) = {"line", "screenline", "number", "both", ખાલી};
#endif
સ્થિર અક્ષર *(p_dy_કિંમતો[]) = {"lastline", "truncate", "uhex", ખાલી};
સ્થિર અક્ષર *(p_jop_કિંમતો[]) = {"stack", ખાલી};
#ifdef FEAT_FOLDING
સ્થિર અક્ષર *(p_fdo_કિંમતો[]) = {"all", "block", "hor", "mark", "percent",
				 "quickfix", "search", "tag", "insert",
				 "undo", "jump", ખાલી};
#endif
// Note: Keep this in sync with match_keyprotocol()
સ્થિર અક્ષર *(p_kpc_protocol_કિંમતો[]) = {"none", "mok2", "kitty", ખાલી};
#ifdef FEAT_IMAGE
// Note: Keep this in sync with match_imageprotocol()
સ્થિર અક્ષર *(p_ipc_protocol_કિંમતો[]) = {"none", "kitty", "sixel", ખાલી};
#endif
#ifdef FEAT_PROP_POPUP
// Note: Keep this in sync with parse_popup_option()
સ્થિર અક્ષર *(p_popup_cpp_option_કિંમતો[]) = {"align:", "border:",
    "borderhighlight:", "close:", "height:", "highlight:", "opacity:",
    "resize:", "shadow:", "width:", ખાલી};
સ્થિર અક્ષર *(p_popup_pvp_option_કિંમતો[]) = {"border:",
    "borderhighlight:", "close:", "height:", "highlight:", "opacity:",
    "resize:", "shadow:", "width:", ખાલી};
સ્થિર અક્ષર *(p_popup_option_on_off_કિંમતો[]) = {"on", "off", ખાલી};
સ્થિર અક્ષર *(p_popup_cpp_border_કિંમતો[]) = {"single", "double", "round",
    "ascii", "on", "off", "custom:", ખાલી};
સ્થિર અક્ષર *(p_popup_pvp_border_કિંમતો[]) = {"single", "double", "round",
    "ascii", "on", "off", "custom:", ખાલી};
સ્થિર અક્ષર *(p_popup_option_align_કિંમતો[]) = {"item", "menu", ખાલી};
#endif
#જો defined(FEAT_SPELL)
// Note: Keep this in sync with spell_check_sps()
સ્થિર અક્ષર *(p_sps_કિંમતો[]) = {"best", "fast", "double", "expr:", "file:", "timeout:", ખાલી};
#endif
#ifdef FEAT_SESSION
// Also used for 'viewoptions'!  Keep in sync with SSOP_ flags.
સ્થિર અક્ષર *(p_ssop_કિંમતો[]) = {"buffers", "winpos", "resize", "winsize",
    "localoptions", "options", "help", "blank", "globals", "slash", "unix",
    "sesdir", "curdir", "folds", "cursor", "tabpages", "terminal", "skiprtp",
    ખાલી};
#endif
#જો defined(FEAT_STL_OPT)
સ્થિર અક્ષર *(p_stlo_કિંમતો[]) = {"fixedheight", "maxheight:", ખાલી};
#endif
// Keep in sync with SWB_ flags in option.h
સ્થિર અક્ષર *(p_swb_કિંમતો[]) = {"useopen", "usetab", "split", "newtab", "vsplit", "uselast", ખાલી};
સ્થિર અક્ષર *(p_spk_કિંમતો[]) = {"cursor", "screen", "topline", ખાલી};
સ્થિર અક્ષર *(p_tc_કિંમતો[]) = {"followic", "ignore", "match", "followscs", "smart", ખાલી};
// Keep in sync with TCL_ flags in option.h
સ્થિર અક્ષર *(p_tcl_કિંમતો[]) = {"left", "uselast", ખાલી};
#જો defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
સ્થિર અક્ષર *(p_toolbar_કિંમતો[]) = {"text", "icons", "tooltips", "horiz", ખાલી};
#endif
#જો defined(FEAT_TOOLBAR) && defined(FEAT_GUI_GTK)
સ્થિર અક્ષર *(p_tbis_કિંમતો[]) = {"tiny", "small", "medium", "large", "huge", "giant", ખાલી};
#endif
#જો defined(UNIX) || defined(VMS)
સ્થિર અક્ષર *(p_ttym_કિંમતો[]) = {"xterm", "xterm2", "dec", "netterm", "jsbterm", "pterm", "urxvt", "sgr", ખાલી};
#endif
સ્થિર અક્ષર *(p_ve_કિંમતો[]) = {"block", "insert", "all", "onemore", "none", "NONE", ખાલી};
// Note: Keep this in sync with check_opt_wim()
સ્થિર અક્ષર *(p_wim_કિંમતો[]) = {"full", "longest", "list", "lastused", "noselect", "noinsert", ખાલી};
સ્થિર અક્ષર *(p_wop_કિંમતો[]) = {"fuzzy", "tagfile", "pum", "exacttext", ખાલી};
#ifdef FEAT_WAK
સ્થિર અક્ષર *(p_wak_કિંમતો[]) = {"yes", "menu", "no", ખાલી};
#endif
સ્થિર અક્ષર *(p_mousem_કિંમતો[]) = {"extend", "popup", "popup_setpos", "mac", ખાલી};
સ્થિર અક્ષર *(p_sel_કિંમતો[]) = {"inclusive", "exclusive", "old", ખાલી};
સ્થિર અક્ષર *(p_slm_કિંમતો[]) = {"mouse", "key", "cmd", ખાલી};
સ્થિર અક્ષર *(p_km_કિંમતો[]) = {"startsel", "stopsel", ખાલી};
#ifdef FEAT_BROWSE
સ્થિર અક્ષર *(p_bsdir_કિંમતો[]) = {"current", "last", "buffer", ખાલી};
#endif
સ્થિર અક્ષર *(p_scbopt_કિંમતો[]) = {"ver", "hor", "jump", ખાલી};
સ્થિર અક્ષર *(p_debug_કિંમતો[]) = {"msg", "throw", "beep", ખાલી};
સ્થિર અક્ષર *(p_ead_કિંમતો[]) = {"both", "ver", "hor", ખાલી};
સ્થિર અક્ષર *(p_buftype_કિંમતો[]) = {"nofile", "nowrite", "quickfix", "help", "terminal", "acwrite", "prompt", "popup", ખાલી};
સ્થિર અક્ષર *(p_bufhidden_કિંમતો[]) = {"hide", "unload", "delete", "wipe", ખાલી};
સ્થિર અક્ષર *(p_bs_કિંમતો[]) = {"indent", "eol", "start", "nostop", ખાલી};
#ifdef FEAT_FOLDING
સ્થિર અક્ષર *(p_fdm_કિંમતો[]) = {"manual", "expr", "marker", "indent", "syntax",
# ifdef FEAT_DIFF
				"diff",
# endif
				ખાલી};
સ્થિર અક્ષર *(p_fcl_કિંમતો[]) = {"all", ખાલી};
#endif
સ્થિર અક્ષર *(p_cfc_કિંમતો[]) = {"keyword", "files", "whole_line", ખાલી};
સ્થિર અક્ષર *(p_cot_કિંમતો[]) = {"menu", "menuone", "longest", "preview", "popup", "popuphidden", "noinsert", "noselect", "fuzzy", "nosort", "preinsert", "nearest", ખાલી};
#ifdef BACKSLASH_IN_FILENAME
સ્થિર અક્ષર *(p_csl_કિંમતો[]) = {"slash", "backslash", ખાલી};
#endif
#ifdef FEAT_SIGNS
સ્થિર અક્ષર *(p_scl_કિંમતો[]) = {"yes", "no", "auto", "number", ખાલી};
#endif
#ifdef UNIX
સ્થિર અક્ષર *(p_trz_કિંમતો[]) = {"inband", "sigwinch", "", ખાલી};
#endif
#જો defined(MSWIN) && defined(FEAT_TERMINAL)
સ્થિર અક્ષર *(p_twt_કિંમતો[]) = {"winpty", "conpty", "", ખાલી};
#endif
સ્થિર અક્ષર *(p_sloc_કિંમતો[]) = {"last", "statusline", "tabline", ખાલી};
સ્થિર અક્ષર *(p_sws_કિંમતો[]) = {"fsync", "sync", ખાલી};

સ્થિર પૂર્ણાંક check_opt_strings(char_u *val, અક્ષર **કિંમતો, પૂર્ણાંક list);
સ્થિર પૂર્ણાંક opt_strings_flags(char_u *val, અક્ષર **કિંમતો, unsigned *flagp, પૂર્ણાંક list);

/*
 * After setting various option values: recompute variables that depend on
 * option values.
 */
    શૂન્ય
didset_string_options(શૂન્ય)
{
    (શૂન્ય)opt_strings_flags(p_cmp, p_cmp_કિંમતો, &cmp_flags, TRUE);
    (શૂન્ય)opt_strings_flags(p_bkc, p_bkc_કિંમતો, &bkc_flags, TRUE);
    (શૂન્ય)opt_strings_flags(p_bo, p_bo_કિંમતો, &bo_flags, TRUE);
    (શૂન્ય)opt_strings_flags(p_cot, p_cot_કિંમતો, &cot_flags, TRUE);
#ifdef FEAT_SESSION
    (શૂન્ય)opt_strings_flags(p_ssop, p_ssop_કિંમતો, &ssop_flags, TRUE);
    (શૂન્ય)opt_strings_flags(p_vop, p_ssop_કિંમતો, &vop_flags, TRUE);
#endif
#ifdef FEAT_FOLDING
    (શૂન્ય)opt_strings_flags(p_fdo, p_fdo_કિંમતો, &fdo_flags, TRUE);
#endif
    (શૂન્ય)opt_strings_flags(p_dy, p_dy_કિંમતો, &dy_flags, TRUE);
    (શૂન્ય)opt_strings_flags(p_jop, p_jop_કિંમતો, &jop_flags, TRUE);
    (શૂન્ય)opt_strings_flags(p_tc, p_tc_કિંમતો, &tc_flags, FALSE);
    (શૂન્ય)opt_strings_flags(p_ve, p_ve_કિંમતો, &ve_flags, TRUE);
#જો defined(UNIX) || defined(VMS)
    (શૂન્ય)opt_strings_flags(p_ttym, p_ttym_કિંમતો, &ttym_flags, FALSE);
#endif
#જો defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
    (શૂન્ય)opt_strings_flags(p_toolbar, p_toolbar_કિંમતો, &toolbar_flags, TRUE);
#endif
#જો defined(FEAT_TOOLBAR) && defined(FEAT_GUI_GTK)
    (શૂન્ય)opt_strings_flags(p_tbis, p_tbis_કિંમતો, &tbis_flags, FALSE);
#endif
    (શૂન્ય)opt_strings_flags(p_swb, p_swb_કિંમતો, &swb_flags, TRUE);
    (શૂન્ય)opt_strings_flags(p_tcl, p_tcl_કિંમતો, &tcl_flags, TRUE);
}

#જો defined(FEAT_EVAL)
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
    શૂન્ય
trigger_optionset_string(
	પૂર્ણાંક	opt_idx,
	પૂર્ણાંક	opt_flags,
	char_u  *oldval,
	char_u  *oldval_l,
	char_u  *oldval_g,
	char_u  *newval)
{
    // Don't do this recursively.
    જો (oldval == ખાલી || newval == ખાલી
				    || *get_vim_var_str(VV_OPTION_TYPE) != NUL)
	પાછા;

    char_u buf_type[7];
    size_t buf_typelen;
    size_t oldvallen;

    oldvallen = STRLEN(oldval);
    set_vim_var_string(VV_OPTION_OLD, oldval, (પૂર્ણાંક)oldvallen);
    set_vim_var_string(VV_OPTION_NEW, newval, -1);
    buf_typelen = vim_snprintf_safelen((અક્ષર *)buf_type, કદ(buf_type),
	"%s", (opt_flags & OPT_LOCAL) ? "local" : "global");
    set_vim_var_string(VV_OPTION_TYPE, buf_type, (પૂર્ણાંક)buf_typelen);
    જો (opt_flags & OPT_LOCAL)
    {
	set_vim_var_string(VV_OPTION_COMMAND, (char_u *)"setlocal", STRLEN_LITERAL("setlocal"));
	set_vim_var_string(VV_OPTION_OLDLOCAL, oldval, (પૂર્ણાંક)oldvallen);
    }
    જો (opt_flags & OPT_GLOBAL)
    {
	set_vim_var_string(VV_OPTION_COMMAND, (char_u *)"setglobal", STRLEN_LITERAL("setglobal"));
	set_vim_var_string(VV_OPTION_OLDGLOBAL, oldval, (પૂર્ણાંક)oldvallen);
    }
    જો ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	set_vim_var_string(VV_OPTION_COMMAND, (char_u *)"set", STRLEN_LITERAL("set"));
	set_vim_var_string(VV_OPTION_OLDLOCAL, oldval_l, -1);
	set_vim_var_string(VV_OPTION_OLDGLOBAL, oldval_g, -1);
    }
    જો (opt_flags & OPT_MODELINE)
    {
	set_vim_var_string(VV_OPTION_COMMAND, (char_u *)"modeline", STRLEN_LITERAL("modeline"));
	set_vim_var_string(VV_OPTION_OLDLOCAL, oldval, (પૂર્ણાંક)oldvallen);
    }
    apply_autocmds(EVENT_OPTIONSET,
	    get_option_fullname(opt_idx), ખાલી, FALSE,
	    ખાલી);
    reset_v_option_vars();
}
#endif

    સ્થિર અક્ષર *
illegal_char(અક્ષર *errbuf, size_t errbuflen, પૂર્ણાંક c)
{
    જો (errbuf == ખાલી)
	પાછા "";
    vim_snprintf(errbuf, errbuflen, _(e_illegal_character_str),
		    (અક્ષર *)transchar(c));
    પાછા errbuf;
}

    સ્થિર અક્ષર *
illegal_char_after_chr(અક્ષર *errbuf, size_t errbuflen, પૂર્ણાંક c)
{
    જો (errbuf == ખાલી)
	પાછા "";
    vim_snprintf(errbuf, errbuflen, _(e_illegal_character_after_chr), c);
    પાછા errbuf;
}

/*
 * Check string options in a buffer for NULL value.
 */
    શૂન્ય
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
#જો defined(FEAT_EVAL)
    check_string_option(&buf->b_p_inde);
    check_string_option(&buf->b_p_indk);
#endif
#જો defined(FEAT_BEVAL) && defined(FEAT_EVAL)
    check_string_option(&buf->b_p_bexpr);
#endif
#જો defined(FEAT_CRYPT)
    check_string_option(&buf->b_p_cm);
#endif
    check_string_option(&buf->b_p_fp);
#જો defined(FEAT_EVAL)
    check_string_option(&buf->b_p_fex);
#endif
#ifdef FEAT_CRYPT
    check_string_option(&buf->b_p_ચાવી);
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
    શૂન્ય
free_string_option(char_u *p)
{
    જો (p != empty_option)
	vim_free(p);
}

    શૂન્ય
clear_string_option(char_u **pp)
{
    જો (*pp != empty_option)
	vim_free(*pp);
    *pp = empty_option;
}

    શૂન્ય
check_string_option(char_u **pp)
{
    જો (*pp == ખાલી)
	*pp = empty_option;
}

/*
 * Set global value for string option when it's a local option.
 */
    સ્થિર શૂન્ય
set_string_option_global(
    પૂર્ણાંક		opt_idx,	// option index
    char_u	**varp)		// pointer to option variable
{
    char_u	**p, *s;

    // the global value is always allocated
    જો (is_window_local_option(opt_idx))
	p = (char_u **)GLOBAL_WO(varp);
    નહીંતર
	p = (char_u **)get_option_var(opt_idx);
    જો (!is_global_option(opt_idx)
	    && p != varp
	    && (s = vim_strsave(*varp)) != ખાલી)
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
    શૂન્ય
set_string_option_direct(
    char_u	*name,
    પૂર્ણાંક		opt_idx,
    char_u	*val,
    પૂર્ણાંક		opt_flags,	// OPT_FREE, OPT_LOCAL and/or OPT_GLOBAL
    પૂર્ણાંક		set_sid UNUSED)
{
    char_u	*s;
    char_u	**varp;
    પૂર્ણાંક		both = (opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0;
    પૂર્ણાંક		idx = opt_idx;

    જો (idx == -1)		// use name
    {
	idx = findoption(name);
	જો (idx < 0)	// not found (should not happen)
	{
	    semsg(_(e_internal_error_str), "set_string_option_direct()");
	    siemsg("For option %s", name);
	    પાછા;
	}
    }

    જો (is_hidden_option(idx))		// can't set hidden option
	પાછા;

    s = vim_strsave(val);
    જો (s == ખાલી)
	પાછા;

    varp = (char_u **)get_option_varp_scope(idx,
	    both ? OPT_LOCAL : opt_flags);
    જો ((opt_flags & OPT_FREE) && (get_option_flags(idx) & P_ALLOCED))
	free_string_option(*varp);
    *varp = s;

    // For buffer/window local option may also set the global value.
    જો (both)
	set_string_option_global(idx, varp);

    set_option_flag(idx, P_ALLOCED);

    // When setting both values of a global option with a local value,
    // make the local value empty, so that the global value is used.
    જો (is_global_local_option(idx) && both)
    {
	free_string_option(*varp);
	*varp = empty_option;
    }
#ifdef FEAT_EVAL
    જો (set_sid != SID_NONE)
    {
	sctx_T script_ctx;

	જો (set_sid == 0)
	    script_ctx = current_sctx;
	નહીંતર
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
    શૂન્ય
set_string_option_direct_in_win(
	win_T		*wp,
	char_u		*name,
	પૂર્ણાંક		opt_idx,
	char_u		*val,
	પૂર્ણાંક		opt_flags,
	પૂર્ણાંક		set_sid)
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

#જો defined(FEAT_PROP_POPUP)
/*
 * Like set_string_option_direct(), but for a buffer-local option in "buf".
 * Blocks autocommands to avoid the old curbuf becoming invalid.
 */
    શૂન્ય
set_string_option_direct_in_buf(
	buf_T		*buf,
	char_u		*name,
	પૂર્ણાંક		opt_idx,
	char_u		*val,
	પૂર્ણાંક		opt_flags,
	પૂર્ણાંક		set_sid)
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
    અક્ષર *
set_string_option(
    પૂર્ણાંક		opt_idx,
    char_u	*કિંમત,
    પૂર્ણાંક		opt_flags,	// OPT_LOCAL and/or OPT_GLOBAL
    અક્ષર	*errbuf,
    size_t	errbuflen)
{
    char_u	*s;
    char_u	**varp;
    char_u	*oldval;
#જો defined(FEAT_EVAL)
    char_u	*oldval_l = ખાલી;
    char_u	*oldval_g = ખાલી;
    char_u	*saved_oldval = ખાલી;
    char_u	*saved_oldval_l = ખાલી;
    char_u	*saved_oldval_g = ખાલી;
    char_u	*saved_newval = ખાલી;
#endif
    અક્ષર	*errmsg = ખાલી;
    પૂર્ણાંક		કિંમત_checked = FALSE;

    જો (is_hidden_option(opt_idx))	// don't set hidden option
	પાછા ખાલી;

    s = vim_strsave(કિંમત == ખાલી ? (char_u *)"" : કિંમત);
    જો (s == ખાલી)
	પાછા ખાલી;

    varp = (char_u **)get_option_varp_scope(opt_idx,
	    (opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0
	    ? (is_global_local_option(opt_idx)
		? OPT_GLOBAL : OPT_LOCAL)
	    : opt_flags);
    oldval = *varp;
#જો defined(FEAT_EVAL)
    જો ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	oldval_l = *(char_u **)get_option_varp_scope(opt_idx, OPT_LOCAL);
	oldval_g = *(char_u **)get_option_varp_scope(opt_idx, OPT_GLOBAL);
    }
#endif
    *varp = s;

#જો defined(FEAT_EVAL)
    જો (!starting
# ifdef FEAT_CRYPT
	    && !is_crypt_ચાવી_option(opt_idx)
# endif
       )
    {
	જો (oldval_l != ખાલી)
	    saved_oldval_l = vim_strsave(oldval_l);
	જો (oldval_g != ખાલી)
	    saved_oldval_g = vim_strsave(oldval_g);
	saved_oldval = vim_strsave(oldval);
	saved_newval = vim_strsave(s);
    }
#endif
    જો ((errmsg = did_set_string_option(opt_idx, varp, oldval, કિંમત, errbuf,
		    errbuflen, opt_flags, OP_NONE, &કિંમત_checked)) == ખાલી)
	did_set_option(opt_idx, opt_flags, TRUE, કિંમત_checked);

#જો defined(FEAT_EVAL)
    // call autocommand after handling side effects
    જો (errmsg == ખાલી)
	trigger_optionset_string(opt_idx, opt_flags,
		saved_oldval, saved_oldval_l,
		saved_oldval_g, saved_newval);
    vim_free(saved_oldval);
    vim_free(saved_oldval_l);
    vim_free(saved_oldval_g);
    vim_free(saved_newval);
#endif
    પાછા errmsg;
}

/*
 * Return TRUE if "val" is a valid 'filetype' name.
 * Also used for 'syntax' and 'keymap'.
 */
    સ્થિર પૂર્ણાંક
valid_filetype(char_u *val)
{
    પાછા valid_name(val, ".-_");
}

#ifdef FEAT_STL_OPT
/*
 * Check validity of options with the 'statusline' format.
 * Return an untranslated error message or NULL.
 */
    સ્થિર અક્ષર *
check_stl_option(char_u *s)
{
    પૂર્ણાંક		groupdepth = 0;
    સ્થિર અક્ષર errbuf[ERR_BUFLEN];
    પૂર્ણાંક		errbuflen = ERR_BUFLEN;

    જ્યારે (*s)
    {
	// Check for valid keys after % sequences
	જ્યારે (*s && *s != '%')
	    s++;
	જો (!*s)
	    તોડો;
	s++;
	જો (*s == STL_CLICKFUNC)
	{
	    જો (s[1] == ']')
	    {
		// %[] - end click region
		s += 2;
		ચાલુ;
	    }
	    જો (ASCII_ISALPHA(s[1]) || s[1] == '_')
	    {
		// %[FuncName] - start click region
		char_u *rb = vim_strchr(s + 2, ']');
		જો (rb != ખાલી)
		{
		    s = rb + 1;
		    ચાલુ;
		}
	    }
	    // Bare %[ is invalid
	    પાછા illegal_char(errbuf, errbuflen, *s);
	}
	જો (*s == STL_LINEBREAK)
	{
	    // Plain %@ - line break
	    s++;
	    ચાલુ;
	}
	જો (*s == '%' || *s == STL_TRUNCMARK || *s == STL_SEPARATE)
	{
	    s++;
	    ચાલુ;
	}
	જો (*s == ')')
	{
	    s++;
	    જો (--groupdepth < 0)
		તોડો;
	    ચાલુ;
	}
	જો (*s == '-')
	    s++;
	જ્યારે (VIM_ISDIGIT(*s))
	    s++;
	જો (*s == STL_USER_HL)
	    ચાલુ;
	જો (*s == STL_CLICKFUNC)
	{
	    // %N[FuncName] or %N[]
	    જો (s[1] == ']')
	    {
		s += 2;
		ચાલુ;
	    }
	    જો (ASCII_ISALPHA(s[1]) || s[1] == '_')
	    {
		char_u *rb = vim_strchr(s + 2, ']');
		જો (rb != ખાલી)
		{
		    s = rb + 1;
		    ચાલુ;
		}
	    }
	    // Bare %N[ is invalid
	    પાછા illegal_char(errbuf, errbuflen, *s);
	}
	જો (*s == STL_LINEBREAK)
	{
	    // %N@ - line break
	    s++;
	    ચાલુ;
	}
	જો (*s == '.')
	{
	    s++;
	    જ્યારે (*s && VIM_ISDIGIT(*s))
		s++;
	}
	જો (*s == '(')
	{
	    groupdepth++;
	    ચાલુ;
	}
	જો (vim_strchr(STL_ALL, *s) == ખાલી)
	{
	    પાછા illegal_char(errbuf, errbuflen, *s);
	}
	જો (*s == '{')
	{
	    પૂર્ણાંક reevaluate = (*++s == '%');

	    જો (reevaluate && *++s == '}')
		// "}" is not allowed immediately after "%{%"
		પાછા illegal_char(errbuf, errbuflen, '}');
	    જ્યારે ((*s != '}' || (reevaluate && s[-1] != '%')) && *s)
		s++;
	    જો (*s != '}')
		પાછા e_unclosed_expression_sequence;
	}
    }
    જો (groupdepth != 0)
	પાછા e_unbalanced_groups;
    પાછા ખાલી;
}
#endif

/*
 * Check for a "normal" directory or file name in some options.  Disallow a
 * path separator (slash and/or backslash), wildcards and characters that are
 * often illegal in a file name. Be more permissive if "secure" is off.
 */
    સ્થિર પૂર્ણાંક
check_illegal_path_names(પૂર્ણાંક opt_idx, char_u **varp)
{
    પાછા (((get_option_flags(opt_idx) & P_NFNAME)
		&& vim_strpbrk(*varp, (char_u *)(secure
			? "/\\*?[|;&<>\r\n" : "/\\*?[<>\r\n")) != ખાલી)
	    || ((get_option_flags(opt_idx) & P_NDNAME)
		&& vim_strpbrk(*varp, (char_u *)"*?[|;&<>\r\n") != ખાલી));
}

/*
 * An option that accepts a list of flags is changed.
 * e.g. 'viewoptions', 'switchbuf', 'casemap', etc.
 */
    સ્થિર અક્ષર *
did_set_opt_flags(char_u *val, અક્ષર **કિંમતો, unsigned *flagp, પૂર્ણાંક list)
{
    જો (opt_strings_flags(val, કિંમતો, flagp, list) == FAIL)
	પાછા e_invalid_argument;

    પાછા ખાલી;
}

/*
 * An option that accepts a list of string values is changed.
 * e.g. 'nrformats', 'scrollopt', 'wildoptions', etc.
 */
    સ્થિર અક્ષર *
did_set_opt_strings(char_u *val, અક્ષર **કિંમતો, પૂર્ણાંક list)
{
    પાછા did_set_opt_flags(val, કિંમતો, ખાલી, list);
}

/*
 * An option which is a list of flags is set.  Valid values are in 'flags'.
 */
    સ્થિર અક્ષર *
did_set_option_listflag(
	char_u *val,
	char_u *flags,
	અક્ષર *errbuf,
	size_t errbuflen)
{
    char_u	*s;

    માટે (s = val; *s; ++s)
	જો (vim_strchr(flags, *s) == ખાલી)
	    પાછા illegal_char(errbuf, errbuflen, *s);

    પાછા ખાલી;
}

/*
 * Expand an option that accepts a list of fixed string values with known
 * number of items.
 */
    સ્થિર પૂર્ણાંક
expand_set_opt_string(
	optexpand_T *args,
	અક્ષર **કિંમતો,
	size_t અંક_કિંમતો,
	પૂર્ણાંક *અંક_Matches,
	char_u ***matches)
{
    char_u	*p;
    regmatch_T	*regmatch = args->oe_regmatch;
    પૂર્ણાંક		include_orig_val = args->oe_include_orig_val;
    char_u	*option_val = args->oe_opt_કિંમત;

    // Assume numValues is small since they are fixed enums, so just allocate
    // upfront instead of needing two passes to calculate output size.
    *matches = ALLOC_MULT(char_u *, અંક_કિંમતો + 1);
    જો (*matches == ખાલી)
	પાછા FAIL;

    પૂર્ણાંક ગણતરી = 0;

    જો (include_orig_val && *option_val != NUL)
    {
	p = vim_strsave(option_val);
	જો (p == ખાલી)
	{
	    VIM_CLEAR(*matches);
	    પાછા FAIL;
	}
	(*matches)[ગણતરી++] = p;
    }

    માટે (અક્ષર **val = કિંમતો; *val != ખાલી; val++)
    {
	જો (include_orig_val && *option_val != NUL)
	{
	    જો (STRCMP((char_u*)*val, option_val) == 0)
		ચાલુ;
	}
	જો (vim_regexec(regmatch, (char_u*)(*val), (colnr_T)0))
	{
	    p = vim_strsave((char_u*)*val);
	    જો (p == ખાલી)
	    {
		જો (ગણતરી == 0)
		{
		    VIM_CLEAR(*matches);
		    પાછા FAIL;
		}
		નહીંતર
		    તોડો;
	    }
	    (*matches)[ગણતરી++] = p;
	}
    }
    જો (ગણતરી == 0)
    {
	VIM_CLEAR(*matches);
	પાછા FAIL;
    }
    *અંક_Matches = ગણતરી;
    પાછા OK;
}

સ્થિર char_u *set_opt_callback_orig_option = ખાલી;
સ્થિર char_u *((*set_opt_callback_func)(expand_T *, પૂર્ણાંક));

/*
 * Callback used by expand_set_opt_generic to also include the original value
 * as the first item.
 */
    સ્થિર char_u *
expand_set_opt_generic_cb(expand_T *xp, પૂર્ણાંક idx)
{
    જો (idx == 0)
    {
	જો (set_opt_callback_orig_option != ખાલી)
	    પાછા set_opt_callback_orig_option;
	નહીંતર
	    પાછા (char_u *)""; // empty strings are ignored
    }
    પાછા set_opt_callback_func(xp, idx - 1);
}

/*
 * Expand an option with a callback that iterates through a list of possible
 * names using an index.
 */
    સ્થિર પૂર્ણાંક
expand_set_opt_generic(
	optexpand_T *args,
	char_u *((*func)(expand_T *, પૂર્ણાંક)),
	પૂર્ણાંક *અંક_Matches,
	char_u ***matches)
{
    પૂર્ણાંક ret;

    set_opt_callback_orig_option = args->oe_include_orig_val ?
	args->oe_opt_કિંમત : ખાલી;
    set_opt_callback_func = func;

    ret = ExpandGeneric(
	    (char_u*)"", // not using fuzzy as currently EXPAND_STRING_SETTING doesn't use it
	    args->oe_xp,
	    args->oe_regmatch,
	    matches,
	    અંક_Matches,
	    expand_set_opt_generic_cb,
	    FALSE);

    set_opt_callback_orig_option = ખાલી;
    set_opt_callback_func = ખાલી;
    પાછા ret;
}

#જો defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
સ્થિર garray_T *expand_cb_ga;
સ્થિર optexpand_T *expand_cb_args;

/*
 * Callback provided to a function in expand_set_opt_callback. Will perform
 * regex matching against the value and add to the list.
 *
 * Returns OK usually. Returns FAIL if it failed to allocate memory, and the
 * caller should terminate the enumeration.
 */
    સ્થિર પૂર્ણાંક
expand_set_opt_callback_cb(char_u *val)
{
    regmatch_T	*regmatch = expand_cb_args->oe_regmatch;
    expand_T	*xp = expand_cb_args->oe_xp;
    garray_T	*ga = expand_cb_ga;
    char_u	*str;

    જો (val == ખાલી || *val == NUL)
	પાછા OK;

    જો (xp->xp_pattern[0] != NUL &&
	    !vim_regexec(regmatch, val, (colnr_T)0))
	પાછા OK;

    str = vim_strsave_escaped(val, (char_u *)" \t\\");

    જો (str == ખાલી)
	પાછા FAIL;

    જો (ga_grow(ga, 1) == FAIL)
    {
	vim_free(str);
	પાછા FAIL;
    }

    ((char_u **)ga->ga_data)[ga->ga_len] = str;
    ++ga->ga_len;
    પાછા OK;
}

/*
 * Expand an option with a provided function that takes a callback. The
 * function will enumerate through all options and call the callback to add it
 * to the list.
 *
 * "func" is the enumerator function that will generate the list of options.
 * "func_params" is a single parameter that will be passed to func.
 */
    સ્થિર પૂર્ણાંક
expand_set_opt_callback(
	optexpand_T *args,
	શૂન્ય (*func)(optexpand_T *, શૂન્ય* params, પૂર્ણાંક (*cb)(char_u *val)),
	શૂન્ય *func_params,
	પૂર્ણાંક *અંક_Matches,
	char_u ***matches)
{
    garray_T	ga;
    પૂર્ણાંક		include_orig_val = args->oe_include_orig_val;
    char_u	*option_val = args->oe_opt_કિંમત;

    ga_init2(&ga, કદ(અક્ષર *), 30);

    જો (include_orig_val && *option_val != NUL)
    {
	char_u *p = vim_strsave(option_val);
	જો (p == ખાલી)
	    પાછા FAIL;
	જો (ga_grow(&ga, 1) == FAIL)
	{
	    vim_free(p);
	    પાછા FAIL;
	}
	((char_u **)ga.ga_data)[ga.ga_len] = p;
	++ga.ga_len;
    }

    expand_cb_ga = &ga;
    expand_cb_args = args;

    func(args, func_params, expand_set_opt_callback_cb);

    expand_cb_ga = ખાલી;
    expand_cb_args = ખાલી;

    *matches = ga.ga_data;
    *અંક_Matches = ga.ga_len;
    પાછા OK;
}
#endif

/*
 * Expand an option which is a list of flags.
 */
    સ્થિર પૂર્ણાંક
expand_set_opt_listflag(
	optexpand_T *args,
	char_u *flags,
	પૂર્ણાંક *અંક_Matches,
	char_u ***matches)
{
    char_u  *p;
    char_u  *option_val = args->oe_opt_કિંમત;
    char_u  *cmdline_val = args->oe_set_arg;
    પૂર્ણાંક	    append = args->oe_append;
    પૂર્ણાંક	    include_orig_val = args->oe_include_orig_val && (*option_val != NUL);

    size_t અંક_flags = STRLEN(flags);

    // Assume we only have small number of flags, so just allocate max size.
    *matches = ALLOC_MULT(char_u *, અંક_flags + 1);
    જો (*matches == ખાલી)
	પાછા FAIL;

    પૂર્ણાંક ગણતરી = 0;

    જો (include_orig_val)
    {
	p = vim_strsave(option_val);
	જો (p == ખાલી)
	{
	    VIM_CLEAR(*matches);
	    પાછા FAIL;
	}
	(*matches)[ગણતરી++] = p;
    }

    માટે (char_u *flag = flags; *flag != NUL; flag++)
    {
	જો (append && vim_strchr(option_val, *flag) != ખાલી)
	    ચાલુ;

	જો (vim_strchr(cmdline_val, *flag) == ખાલી)
	{
	    જો (include_orig_val
		    && option_val[1] == NUL
		    && *flag == option_val[0])
	    {
		// This value is already used as the first choice as it's the
		// existing flag. Just skip it to avoid duplicate.
		ચાલુ;
	    }
	    p = vim_strnsave(flag, 1);
	    જો (p == ખાલી)
	    {
		જો (ગણતરી == 0)
		{
		    VIM_CLEAR(*matches);
		    પાછા FAIL;
		}
		નહીંતર
		    તોડો;
	    }
	    (*matches)[ગણતરી++] = p;
	}
    }

    જો (ગણતરી == 0)
    {
	VIM_CLEAR(*matches);
	પાછા FAIL;
    }
    *અંક_Matches = ગણતરી;
    પાછા OK;
}

/*
 * The 'ambiwidth' option is changed.
 */
    અક્ષર *
did_set_ambiwidth(optset_T *args UNUSED)
{
    જો (check_opt_strings(p_ambw, p_ambw_કિંમતો, FALSE) != OK)
	પાછા e_invalid_argument;

    પાછા check_chars_options();
}

// "name" must be a string literal, the length is computed at compile time.
#define completing_કિંમત_for_subopt(args, name) \
	  completing_કિંમત_for_subopt_len(args, name, (પૂર્ણાંક)STRLEN_LITERAL(name))

/*
 * Return true when completing the value of the sub-option "name" with length
 * "len", e.g. the value after "close:" in 'completepopup'.
 */
    સ્થિર bool
completing_કિંમત_for_subopt_len(optexpand_T *args, અક્ષર *name, પૂર્ણાંક len)
{
    char_u  *colon = args->oe_xp->xp_pattern - 1;
    પૂર્ણાંક	    off = (પૂર્ણાંક)(colon - args->oe_set_arg);

    જો (off < len)
	પાછા false;
    // The name must follow a comma when it does not start the option value.
    જો (off > len && *(colon - len - 1) != ',')
	પાછા false;

    પાછા STRNCMP(colon - len, name, len) == 0;
}

    પૂર્ણાંક
expand_set_ambiwidth(optexpand_T *args, પૂર્ણાંક *અંક_Matches, char_u ***matches)
{
    પાછા expand_set_opt_string(
	    args,
	    p_ambw_કિંમતો,
	    ARRAY_LENGTH(p_ambw_કિંમતો) - 1,
	    અંક_Matches,
	    matches);
}

/*
 * The 'background' option is changed.
 */
    અક્ષર *
did_set_background(optset_T *args)
{
    જો (check_opt_strings(p_bg, p_bg_કિંમતો, FALSE) == FAIL)
	પાછા e_invalid_argument;

    જો (args->os_oldval.string != ખાલી && args->os_oldval.string[0] == *p_bg)
	// Value was not changed
	પાછા ખાલી;

#ifdef FEAT_EVAL
    પૂર્ણાંક dark = (*p_bg == 'd');
#endif

    init_highlight(FALSE, FALSE);

#ifdef FEAT_EVAL
    જો (dark != (*p_bg == 'd')
	    && get_var_કિંમત((char_u *)"g:colors_name") != ખાલી)
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

    પાછા ખાલી;
}

    પૂર્ણાંક
expand_set_background(optexpand_T *args, પૂર્ણાંક *અંક_Matches, char_u ***matches)
{
    પાછા expand_set_opt_string(
	    args,
	    p_bg_કિંમતો,
	    ARRAY_LENGTH(p_bg_કિંમતો) - 1,
	    અંક_Matches,
	    matches);
}

/*
 * The 'backspace' option is changed.
 */
    અક્ષર *
did_set_backspace(optset_T *args UNUSED)
{
    જો (VIM_ISDIGIT(*p_bs))
    {
	જો (*p_bs > '3' || p_bs[1] != NUL)
	    પાછા e_invalid_argument;
    }
    નહીંતર જો (check_opt_strings(p_bs, p_bs_કિંમતો, TRUE) != OK)
	પાછા e_invalid_argument;

    પાછા ખાલી;
}

    પૂર્ણાંક
expand_set_backspace(optexpand_T *args, પૂર્ણાંક *અંક_Matches, char_u ***matches)
{
    પાછા expand_set_opt_string(
	    args,
	    p_bs_કિંમતો,
	    ARRAY_LENGTH(p_bs_કિંમતો) - 1,
	    અંક_Matches,
	    matches);
}

/*
 * The 'backupcopy' option is changed.
 */
    અક્ષર *
did_set_backupcopy(optset_T *args)
{
    char_u		*bkc = p_bkc;
    unsigned પૂર્ણાંક	*flags = &bkc_flags;
    અક્ષર		*errmsg = ખાલી;

    જો (args->os_flags & OPT_LOCAL)
    {
	bkc = curbuf->b_p_bkc;
	flags = &curbuf->b_bkc_flags;
    }
    નહીંતર જો (!(args->os_flags & OPT_GLOBAL))
	// When using :set, clear the local flags.
	curbuf->b_bkc_flags = 0;

    જો ((args->os_flags & OPT_LOCAL) && *bkc == NUL)
	// make the local value empty: use the global value
	*flags = 0;
    નહીંતર
    {
	જો (opt_strings_flags(bkc, p_bkc_કિંમતો, flags, TRUE) != OK)
	    errmsg = e_invalid_argument;
	જો ((((પૂર્ણાંક)*flags & BKC_AUTO) != 0)
		+ (((પૂર્ણાંક)*flags & BKC_YES) != 0)
		+ (((પૂર્ણાંક)*flags & BKC_NO) != 0) != 1)
	{
	    // Must have exactly one of "auto", "yes"  and "no".
	    (શૂન્ય)opt_strings_flags(args->os_oldval.string, p_bkc_કિંમતો,
								  flags, TRUE);
	    errmsg = e_invalid_argument;
	}
    }

    પાછા errmsg;
}

    પૂર્ણાંક
expand_set_backupcopy(optexpand_T *args, પૂર્ણાંક *અંક_Matches, char_u ***matches)
{
    પાછા expand_set_opt_string(
	    args,
	    p_bkc_કિંમતો,
	    ARRAY_LENGTH(p_bkc_કિંમતો) - 1,
	    અંક_Matches,
	    matches);
}

/*
 * The 'backupext' or the 'patchmode' option is changed.
 */
    અક્ષર *
did_set_backupext_or_patchmode(optset_T *args UNUSED)
{
    જો (STRCMP(*p_bex == '.' ? p_bex + 1 : p_bex,
		*p_pm == '.' ? p_pm + 1 : p_pm) == 0)
	પાછા e_backupext_and_patchmode_are_equal;

    પાછા ખાલી;
}

/*
 * The 'belloff' option is changed.
 */
    અક્ષર *
did_set_belloff(optset_T *args UNUSED)
{
    પાછા did_set_opt_flags(p_bo, p_bo_કિંમતો, &bo_flags, TRUE);
}

    પૂર્ણાંક
expand_set_belloff(optexpand_T *args, પૂર્ણાંક *અંક_Matches, char_u ***matches)
{
    પાછા expand_set_opt_string(
	    args,
	    p_bo_કિંમતો,
	    ARRAY_LENGTH(p_bo_કિંમતો) - 1,
	    અંક_Matches,
	    matches);
}

#જો defined(FEAT_LINEBREAK)
/*
 * The 'breakat' option is changed.
 */
    અક્ષર *
did_set_breakat(optset_T *args UNUSED)
{
    char_u	*p;
    પૂર્ણાંક		i;

    માટે (i = 0; i < 256; i++)
	breakat_flags[i] = FALSE;

    જો (p_breakat != ખાલી)
	માટે (p = p_breakat; *p; p++)
	    breakat_flags[*p] = TRUE;

    પાછા ખાલી;
}

/*
 * The 'breakindentopt' option is changed.
 */
    અક્ષર *
did_set_breakindentopt(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    જો (briopt_check(*varp, varp == &curwin->w_p_briopt ? curwin : ખાલી)
								      == FAIL)
	પાછા e_invalid_argument;

    // list setting requires a redraw
    જો (varp == &curwin->w_p_briopt && curwin->w_briopt_list)
	redraw_all_later(UPD_NOT_VALID);

    પાછા ખાલી;
}

    પૂર્ણાંક
expand_set_breakindentopt(
    optexpand_T		*args,
    પૂર્ણાંક			*અંક_Matches,
    char_u		***matches)
{
    પાછા expand_set_opt_string(
	    args,
	    p_briopt_કિંમતો,
	    ARRAY_LENGTH(p_briopt_કિંમતો) - 1,
	    અંક_Matches,
	    matches);
}
#endif

#જો defined(FEAT_BROWSE)
/*
 * The 'browsedir' option is changed.
 */
    અક્ષર *
did_set_browsedir(optset_T *args UNUSED)
{
    જો (check_opt_strings(p_bsdir, p_bsdir_કિંમતો, FALSE) != OK
	    && !mch_isdir(p_bsdir))
	પાછા e_invalid_argument;

    પાછા ખાલી;
}

    પૂર્ણાંક
expand_set_browsedir(optexpand_T *args, પૂર્ણાંક *અંક_Matches, char_u ***matches)
{
    પાછા expand_set_opt_string(
	    args,
	    p_bsdir_કિંમતો,
	    ARRAY_LENGTH(p_bsdir_કિંમતો) - 1,
	    અંક_Matches,
	    matches);
}
#endif

/*
 * The 'bufhidden' option is changed.
 */
    અક્ષર *
did_set_bufhidden(optset_T *args UNUSED)
{
    પાછા did_set_opt_strings(curbuf->b_p_bh, p_bufhidden_કિંમતો, FALSE);
}

    પૂર્ણાંક
expand_set_bufhidden(optexpand_T *args, પૂર્ણાંક *અંક_Matches, char_u ***matches)
{
    પાછા expand_set_opt_string(
	    args,
	    p_bufhidden_કિંમતો,
	    ARRAY_LENGTH(p_bufhidden_કિંમતો) - 1,
	    અંક_Matches,
	    matches);
}

/*
 * The 'buftype' option is changed.
 */
    અક્ષર *
did_set_buftype(optset_T *args UNUSED)
{
    જો (check_opt_strings(curbuf->b_p_bt, p_buftype_કિંમતો, FALSE) != OK)
	પાછા e_invalid_argument;

    જો (curwin->w_status_height)
    {
	curwin->w_redr_status = true;
	redraw_later(UPD_VALID);
    }
    curbuf->b_help = (curbuf->b_p_bt[0] == 'h');
    redraw_titles();

    પાછા ખાલી;
}

    પૂર્ણાંક
expand_set_buftype(optexpand_T *args, પૂર્ણાંક *અંક_Matches, char_u ***matches)
{
    પાછા expand_set_opt_string(
	    args,
	    p_buftype_કિંમતો,
	    ARRAY_LENGTH(p_buftype_કિંમતો) - 1,
	    અંક_Matches,
	    matches);
}

/*
 * The 'casemap' option is changed.
 */
    અક્ષર *
did_set_casemap(optset_T *args UNUSED)
{
    પાછા did_set_opt_flags(p_cmp, p_cmp_કિંમતો, &cmp_flags, TRUE);
}

    પૂર્ણાંક
expand_set_casemap(optexpand_T *args, પૂર્ણાંક *અંક_Matches, char_u ***matches)
{
    પાછા expand_set_opt_string(
	    args,
	    p_cmp_કિંમતો,
	    ARRAY_LENGTH(p_cmp_કિંમતો) - 1,
	    અંક_Matches,
	    matches);
}

#જો defined(HAVE_CLIPMETHOD)
    પૂર્ણાંક
expand_set_clipboard(optexpand_T *args, પૂર્ણાંક *અંક_Matches, char_u ***matches)
{
    પાછા expand_set_opt_string(
	    args,
	    p_cb_કિંમતો,
	    ARRAY_LENGTH(p_cb_કિંમતો) - 1,
	    અંક_Matches,
	    matches);
}
#endif

#ifdef HAVE_CLIPMETHOD
    અક્ષર *
did_set_clipmethod(optset_T *args UNUSED)
{
    પાછા choose_clipmethod();
}

    પૂર્ણાંક
expand_set_clipmethod(optexpand_T *args, પૂર્ણાંક *અંક_Matches, char_u ***matches)
{
    // We want to expand using the predefined clipmethod values + clipboard
    // provider names.
    પૂર્ણાંક		result;
    અક્ષર	**કિંમતો;
    પૂર્ણાંક		ગણતરી, pos = 0, start = 0;
# ifdef FEAT_EVAL
    dict_T	*providers = get_vim_var_dict(VV_CLIPPROVIDERS);
# નહીંતર
    dict_T	*providers = ખાલી;
# endif
    hashtab_T	*ht = providers == ખાલી ? ખાલી : &providers->dv_hashtab;

    ગણતરી = (ht == ખાલી ? 0 : ht->ht_used);
# ifdef FEAT_WAYLAND_CLIPBOARD
    ગણતરી++;
    start++;
# endif
# ifdef FEAT_XCLIPBOARD
    ગણતરી++;
    start++;
# endif
    કિંમતો = ALLOC_MULT(અક્ષર *, ગણતરી + 1); // Add NULL terminator too

    જો (કિંમતો == ખાલી)
	પાછા FAIL;

# ifdef FEAT_WAYLAND_CLIPBOARD
    કિંમતો[pos++] = "wayland";
# endif
# ifdef FEAT_XCLIPBOARD
    કિંમતો[pos++] = "x11";
# endif

    જો (ht != ખાલી)
	માટે (long_u i = 0; i < ht->ht_mask + 1; i++)
	{
	    hashitem_T	*hi = ht->ht_array + i;

	    જો (!HASHITEM_EMPTY(hi))
		કિંમતો[pos++] = (અક્ષર *)vim_strsave(hi->hi_ચાવી);
	}
    કિંમતો[pos++] = ખાલી;

    result = expand_set_opt_string(
	    args,
	    કિંમતો,
	    ગણતરી,
	    અંક_Matches,
	    matches);

    માટે (પૂર્ણાંક i = start; i < ગણતરી; i++)
	vim_free(કિંમતો[i]);
    vim_free(કિંમતો);

    પાછા result;
}
#endif

/*
 * The global 'listchars' or 'fillchars' option is changed.
 */
    સ્થિર અક્ષર *
did_set_global_listfillchars(char_u *val, પૂર્ણાંક opt_lcs, પૂર્ણાંક opt_flags,
						અક્ષર *errbuf, size_t errbuflen)
{
    અક્ષર	*errmsg = ખાલી;
    char_u	**local_ptr = opt_lcs ? &curwin->w_p_lcs : &curwin->w_p_fcs;

    // only apply the global value to "curwin" when it does not have a
    // local value
    જો (opt_lcs)
	errmsg = set_listchars_option(curwin, val,
		**local_ptr == NUL || !(opt_flags & OPT_GLOBAL),
							    errbuf, errbuflen);
    નહીંતર
	errmsg = set_fillchars_option(curwin, val,
		**local_ptr == NUL || !(opt_flags & OPT_GLOBAL),
							    errbuf, errbuflen);
    જો (errmsg != ખાલી)
	પાછા errmsg;

    tabpage_T	*tp;
    win_T	*wp;

    // If the current window is set to use the global
    // 'listchars'/'fillchars' value, clear the window-local value.
    જો (!(opt_flags & OPT_GLOBAL))
	clear_string_option(local_ptr);
    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	// If the current window has a local value need to apply it
	// again, it was changed when setting the global value.
	// If no error was returned above, we don't expect an error
	// here, so ignore the return value.
	જો (opt_lcs)
	{
	    જો (*wp->w_p_lcs == NUL)
		(શૂન્ય)set_listchars_option(wp, wp->w_p_lcs, TRUE, ખાલી, 0);
	}
	નહીંતર
	{
	    જો (*wp->w_p_fcs == NUL)
		(શૂન્ય)set_fillchars_option(wp, wp->w_p_fcs, TRUE, ખાલી, 0);
	}
    }

    redraw_all_later(UPD_NOT_VALID);

    પાછા ખાલી;
}

/*
 * The 'fillchars' option or the 'listchars' option is changed.
 */
    અક્ષર *
did_set_chars_option(optset_T *args)
{
    char_u **varp = (char_u **)args->os_varp;
    અક્ષર *errmsg = ખાલી;

    જો (   varp == &p_lcs		// global 'listchars'
	|| varp == &p_fcs)		// global 'fillchars'
	errmsg = did_set_global_listfillchars(*varp, varp == &p_lcs,
			  args->os_flags, args->os_errbuf, args->os_errbuflen);
    નહીંતર જો (varp == &curwin->w_p_lcs)	// local 'listchars'
	errmsg = set_listchars_option(curwin, *varp, TRUE,
					  args->os_errbuf, args->os_errbuflen);
    નહીંતર જો (varp == &curwin->w_p_fcs)	// local 'fillchars'
	errmsg = set_fillchars_option(curwin, *varp, TRUE,
					  args->os_errbuf, args->os_errbuflen);

    પાછા errmsg;
}

/*
 * Expand 'fillchars' or 'listchars' option value.
 */
    પૂર્ણાંક
expand_set_chars_option(optexpand_T *args, પૂર્ણાંક *અંક_Matches, char_u ***matches)
{
    char_u **varp = (char_u **)args->oe_varp;
    પૂર્ણાંક is_lcs = (varp == &p_lcs || varp == &curwin->w_p_lcs);
    પાછા expand_set_opt_generic(
	    args,
	    is_lcs ? get_listchars_name : get_fillchars_name,
	    અંક_Matches,
	    matches);
}

/*
 * The 'cinoptions' option is changed.
 */
    અક્ષર *
did_set_cinoptions(optset_T *args UNUSED)
{
    // TODO: recognize errors
    parse_cino(curbuf);

    પાછા ખાલી;
}

#જો defined(FEAT_SYN_HL)
/*
 * The 'colorcolumn' option is changed.
 */
    અક્ષર *
did_set_colorcolumn(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    પાછા check_colorcolumn(*varp, varp == &curwin->w_p_cc ? curwin : ખાલી);
}
#endif

/*
 * The 'comments' option is changed.
 */
    અક્ષર *
did_set_comments(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u	*s;
    અક્ષર	*errmsg = ખાલી;

    માટે (s = *varp; *s; )
    {
	જ્યારે (*s && *s != ':')
	{
	    જો (vim_strchr((char_u *)COM_ALL, *s) == ખાલી
		    && !VIM_ISDIGIT(*s) && *s != '-')
	    {
		errmsg = illegal_char(args->os_errbuf, args->os_errbuflen, *s);
		તોડો;
	    }
	    ++s;
	}
	જો (*s++ == NUL)
	    errmsg = e_missing_colon;
	નહીંતર જો (*s == ',' || *s == NUL)
	    errmsg = e_zero_length_string;
	જો (errmsg != ખાલી)
	    તોડો;
	જ્યારે (*s && *s != ',')
	{
	    જો (*s == '\\' && s[1] != NUL)
		++s;
	    ++s;
	}
	s = skip_to_option_part(s);
    }

    પાછા errmsg;
}

/*
 * The 'commentstring' option is changed.
 */
    અક્ષર *
did_set_commentstring(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    જો (**varp != NUL && strstr((અક્ષર *)*varp, "%s") == ખાલી)
	પાછા e_commentstring_must_be_empty_or_contain_str;

    પાછા ખાલી;
}

/*
 * Check if value for 'complete' is valid when 'complete' option is changed.
 */
    અક્ષર *
did_set_complete(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u	*p, *t;
    char_u	buffer[LSIZE];
    char_u	*buf_ptr;
    char_u	char_before = NUL;
    પૂર્ણાંક		escape;

    માટે (p = *varp; *p; )
    {
	vim_memset(buffer, 0, LSIZE);
	buf_ptr = buffer;
	escape = 0;

	// Extract substring while handling escaped commas
	જ્યારે (*p && (*p != ',' || escape) && buf_ptr < (buffer + LSIZE - 1))
	{
	    જો (*p == '\\' && *(p + 1) == ',')
	    {
		escape = 1;  // Mark escape mode
		p++;         // Skip '\'
	    }
	    નહીંતર
	    {
		escape = 0;
		*buf_ptr++ = *p;
	    }
	    p++;
	}
	*buf_ptr = NUL;

	જો (vim_strchr((char_u *)".wbuksid]tUFo", *buffer) == ખાલી)
	    પાછા illegal_char(args->os_errbuf, args->os_errbuflen, *buffer);

	જો (vim_strchr((char_u *)"ksF", *buffer) == ખાલી && *(buffer + 1) != NUL
		&& *(buffer + 1) != '^')
	    char_before = *buffer;
	નહીંતર
	{
	    // Test for a number after '^'
	    જો ((t = vim_strchr(buffer, '^')) != ખાલી)
	    {
		*t++ = NUL;
		જો (!*t)
		    char_before = '^';
		નહીંતર
		{
		    માટે (; *t; t++)
		    {
			જો (!vim_isdigit(*t))
			{
			    char_before = '^';
			    તોડો;
			}
		    }
		}
	    }
	}
	જો (char_before != NUL)
	    પાછા illegal_char_after_chr(args->os_errbuf, args->os_errbuflen,
		    char_before);
	// Skip comma and spaces
	જ્યારે (*p == ',' || *p == ' ')
	    p++;
    }

#ifdef FEAT_COMPL_FUNC
    જો (set_cpt_callbacks(args) != OK)
	પાછા illegal_char_after_chr(args->os_errbuf, args->os_errbuflen,
		'F');
#endif
    પાછા ખાલી;
}

    પૂર્ણાંક
expand_set_complete(optexpand_T *args, પૂર્ણાંક *અંક_Matches, char_u ***matches)
{
    સ્થિર અક્ષર *(p_cpt_કિંમતો[]) = {
	".", "w", "b", "u", "k", "kspell", "s", "i", "d", "]", "t", "U", "F", "o",
	ખાલી};
    પાછા expand_set_opt_string(
	    args,
	    p_cpt_કિંમતો,
	    ARRAY_LENGTH(p_cpt_કિંમતો) - 1,
	    અંક_Matches,
	    matches);
}

/*
 * The 'completeopt' option is changed.
 */
    અક્ષર *
did_set_completeopt(optset_T *args UNUSED)
{
    char_u	*cot = p_cot;
    unsigned	*flags = &cot_flags;

    જો (args->os_flags & OPT_LOCAL)
    {
	cot = curbuf->b_p_cot;
	flags = &curbuf->b_cot_flags;
    }
    નહીંતર જો (!(args->os_flags & OPT_GLOBAL))
	// When using :set, clear the local flags.
	curbuf->b_cot_flags = 0;

    જો (opt_strings_flags(cot, p_cot_કિંમતો, flags, TRUE) != OK)
	પાછા e_invalid_argument;

    પાછા ખાલી;
}

    પૂર્ણાંક
expand_set_completeopt(optexpand_T *args, પૂર્ણાંક *અંક_Matches, char_u ***matches)
{
    પાછા expand_set_opt_string(
	    args,
	    p_cot_કિંમતો,
	    ARRAY_LENGTH(p_cot_કિંમતો) - 1,
	    અંક_Matches,
	    matches);
}

/*
 * The 'completefuzzycollect' option is changed.
 */
    અક્ષર *
did_set_completefuzzycollect(optset_T *args UNUSED)
{
    જો (opt_strings_flags(p_cfc, p_cfc_કિંમતો, &cfc_flags, TRUE) != OK)
	પાછા e_invalid_argument;
    પાછા ખાલી;
}

    પૂર્ણાંક
expand_set_completefuzzycollect(
	optexpand_T *args,
	પૂર્ણાંક *અંક_Matches,
	char_u ***matches)
{
    પાછા expand_set_opt_string(
	    args,
	    p_cfc_કિંમતો,
	    ARRAY_LENGTH(p_cfc_કિંમતો) - 1,
	    અંક_Matches,
	    matches);
}

/*
 * The 'completeitemalign' option is changed.
 */
    અક્ષર *
did_set_completeitemalign(optset_T *args UNUSED)
{
    char_u	*p = p_cia;
    unsigned	new_cia_flags = 0;
    પૂર્ણાંક		seen[3] = { FALSE, FALSE, FALSE };
    પૂર્ણાંક		ગણતરી = 0;
    char_u	buf[10];

    જ્યારે (*p)
    {
	copy_option_part(&p, buf, કદ(buf), ",");
	જો (ગણતરી >= 3)
	    પાછા e_invalid_argument;

	જો (STRCMP(buf, "abbr") == 0)
	{
	    જો (seen[CPT_ABBR])
		પાછા e_invalid_argument;
	    new_cia_flags = new_cia_flags * 10 + CPT_ABBR;
	    seen[CPT_ABBR] = TRUE;
	    ગણતરી++;
	}
	નહીંતર જો (STRCMP(buf, "kind") == 0)
	{
	    જો (seen[CPT_KIND])
		પાછા e_invalid_argument;
	    new_cia_flags = new_cia_flags * 10 + CPT_KIND;
	    seen[CPT_KIND] = TRUE;
	    ગણતરી++;
	}
	નહીંતર જો (STRCMP(buf, "menu") == 0)
	{
	    જો (seen[CPT_MENU])
		પાછા e_invalid_argument;
	    new_cia_flags = new_cia_flags * 10 + CPT_MENU;
	    seen[CPT_MENU] = TRUE;
	    ગણતરી++;
	}
	નહીંતર
	    પાછા e_invalid_argument;
    }
    જો (new_cia_flags == 0 || ગણતરી != 3)
	પાછા e_invalid_argument;

    cia_flags = new_cia_flags;
    પાછા ખાલી;
}

#જો defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
/*
 * The 'completepopup' option is changed.
 */
    અક્ષર *
did_set_completepopup(optset_T *args UNUSED)
{
    જો (parse_completepopup(ખાલી) == FAIL)
	પાછા e_invalid_argument;

    popup_close_info();
    પાછા ખાલી;
}
#endif

#જો defined(BACKSLASH_IN_FILENAME)
/*
 * The 'completeslash' option is changed.
 */
    અક્ષર *
did_set_completeslash(optset_T *args UNUSED)
{
    જો (check_opt_strings(p_csl, p_csl_કિંમતો, FALSE) != OK
	    || check_opt_strings(curbuf->b_p_csl, p_csl_કિંમતો, FALSE) != OK)
	પાછા e_invalid_argument;

    પાછા ખાલી;
}

    પૂર્ણાંક
expand_set_completeslash(optexpand_T *args, પૂર્ણાંક *અંક_Matches, char_u ***matches)
{
    પાછા expand_set_opt_string(
	    args,
	    p_csl_કિંમતો,
	    ARRAY_LENGTH(p_csl_કિંમતો) - 1,
	    અંક_Matches,
	    matches);
}
#endif

#જો defined(FEAT_CONCEAL)
/*
 * The 'concealcursor' option is changed.
 */
    અક્ષર *
did_set_concealcursor(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    પાછા did_set_option_listflag(*varp, (char_u *)COCU_ALL, args->os_errbuf,
		    args->os_errbuflen);
}

    પૂર્ણાંક
expand_set_concealcursor(optexpand_T *args, પૂર્ણાંક *અંક_Matches, char_u ***matches)
{
    પાછા expand_set_opt_listflag(args, (char_u*)COCU_ALL, અંક_Matches, matches);
}
#endif

/*
 * The 'cpoptions' option is changed.
 */
    અક્ષર *
did_set_cpoptions(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    પાછા did_set_option_listflag(*varp, (char_u *)CPO_ALL, args->os_errbuf,
		    args->os_errbuflen);
}

    પૂર્ણાંક
expand_set_cpoptions(optexpand_T *args, પૂર્ણાંક *અંક_Matches, char_u ***matches)
{
    પાછા expand_set_opt_listflag(args, (char_u*)CPO_ALL, અંક_Matches, matches);
}

#જો defined(FEAT_CRYPT)
/*
 * The 'cryptkey' option is changed.
 */
    અક્ષર *
did_set_cryptkey(optset_T *args)
{
    // Make sure the ":set" command doesn't show the new value in the
    // history.
    remove_ચાવી_from_history();

    જો (args->os_op != OP_NONE)
	// Don't allow set+=/-=/^= as they can allow for substring guessing
	પાછા e_invalid_argument;

    જો (STRCMP(curbuf->b_p_ચાવી, args->os_oldval.string) != 0)
    {
	// Need to update the swapfile.
	ml_set_crypt_ચાવી(curbuf, args->os_oldval.string,
		*curbuf->b_p_cm == NUL ? p_cm : curbuf->b_p_cm);
	changed_internal();
    }
# ifdef FEAT_SODIUM
    જો (crypt_method_is_sodium(crypt_get_method_nr(curbuf)))
	crypt_sodium_lock_ચાવી(args->os_newval.string);
# endif

    પાછા ખાલી;
}

/*
 * The 'cryptmethod' option is changed.
 */
    અક્ષર *
did_set_cryptmethod(optset_T *args)
{
    char_u  *p;
    char_u  *s;

    જો (args->os_flags & OPT_LOCAL)
	p = curbuf->b_p_cm;
    નહીંતર
	p = p_cm;
    જો (check_opt_strings(p, p_cm_કિંમતો, TRUE) != OK)
	પાછા e_invalid_argument;
    નહીંતર જો (crypt_self_test() == FAIL)
	પાછા e_invalid_argument;

    // When setting the global value to empty, make it "zip".
    જો (*p_cm == NUL)
    {
	free_string_option(p_cm);
	p_cm = vim_strsave((char_u *)"zip");
    }
    // When using ":set cm=name" the local value is going to be empty.
    // Do that here, otherwise the crypt functions will still use the
    // local value.
    જો ((args->os_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	free_string_option(curbuf->b_p_cm);
	curbuf->b_p_cm = empty_option;
    }

    // Need to update the swapfile when the effective method changed.
    // Set "s" to the effective old value, "p" to the effective new
    // method and compare.
    જો ((args->os_flags & OPT_LOCAL) && *args->os_oldval.string == NUL)
	s = p_cm;  // was previously using the global value
    નહીંતર
	s = args->os_oldval.string;
    જો (*curbuf->b_p_cm == NUL)
	p = p_cm;  // is now using the global value
    નહીંતર
	p = curbuf->b_p_cm;
    જો (STRCMP(s, p) != 0)
	ml_set_crypt_ચાવી(curbuf, curbuf->b_p_ચાવી, s);

    // If the global value changes need to update the swapfile for all
    // buffers using that value.
    જો ((args->os_flags & OPT_GLOBAL)
	    && STRCMP(p_cm, args->os_oldval.string) != 0)
    {
	buf_T	*buf;

	FOR_ALL_BUFFERS(buf)
	    જો (buf != curbuf && *buf->b_p_cm == NUL)
		ml_set_crypt_ચાવી(buf, buf->b_p_ચાવી, args->os_oldval.string);
    }
    પાછા ખાલી;
}

    પૂર્ણાંક
expand_set_cryptmethod(optexpand_T *args, પૂર્ણાંક *અંક_Matches, char_u ***matches)
{
    પાછા expand_set_opt_string(
	    args,
	    p_cm_કિંમતો,
	    ARRAY_LENGTH(p_cm_કિંમતો) - 1,
	    અંક_Matches,
	    matches);
}
#endif

#જો defined(FEAT_CSCOPE) && defined(FEAT_QUICKFIX)
/*
 * The 'cscopequickfix' option is changed.
 */
    અક્ષર *
did_set_cscopequickfix(optset_T *args UNUSED)
{
    char_u	*p;

    જો (p_csqf == ખાલી)
	પાછા ખાલી;

    p = p_csqf;
    જ્યારે (*p != NUL)
    {
	જો (vim_strchr((char_u *)CSQF_CMDS, *p) == ખાલી
		|| p[1] == NUL
		|| vim_strchr((char_u *)CSQF_FLAGS, p[1]) == ખાલી
		|| (p[2] != NUL && p[2] != ','))
	    પાછા e_invalid_argument;
	નહીંતર જો (p[2] == NUL)
	    તોડો;
	નહીંતર
	    p += 3;
    }

    પાછા ખાલી;
}
#endif

#જો defined(FEAT_SYN_HL)
/*
 * The 'cursorlineopt' option is changed.
 */
    અક્ષર *
did_set_cursorlineopt(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    // This could be changed to use opt_strings_flags() instead.
    જો (**varp == NUL || fill_culopt_flags(*varp, curwin) != OK)
	પાછા e_invalid_argument;

    પાછા ખાલી;
}

    પૂર્ણાંક
expand_set_cursorlineopt(optexpand_T *args, પૂર્ણાંક *અંક_Matches, char_u ***matches)
{
    પાછા expand_set_opt_string(
	    args,
	    p_culopt_કિંમતો,
	    ARRAY_LENGTH(p_culopt_કિંમતો) - 1,
	    અંક_Matches,
	    matches);
}
#endif

/*
 * The 'debug' option is changed.
 */
    અક્ષર *
did_set_debug(optset_T *args UNUSED)
{
    પાછા did_set_opt_strings(p_debug, p_debug_કિંમતો, TRUE);
}

    પૂર્ણાંક
expand_set_debug(optexpand_T *args, પૂર્ણાંક *અંક_Matches, char_u ***matches)
{
    પાછા expand_set_opt_string(
	    args,
	    p_debug_કિંમતો,
	    ARRAY_LENGTH(p_debug_કિંમતો) - 1,
	    અંક_Matches,
	    matches);
}

#જો defined(FEAT_DIFF)
/*
 * The 'diffanchors' option is changed.
 */
    અક્ષર *
did_set_diffanchors(optset_T *args)
{
    જો (diffanchors_changed(args->os_flags & OPT_LOCAL) == FAIL)
	પાછા e_invalid_argument;

    પાછા ખાલી;
}

/*
 * The 'diffopt' option is changed.
 */
    અક્ષર *
did_set_diffopt(optset_T *args UNUSED)
{
    જો (diffopt_changed() == FAIL)
	પાછા e_invalid_argument;

    પાછા ખાલી;
}

    પૂર્ણાંક
expand_set_diffopt(optexpand_T *args, પૂર્ણાંક *અંક_Matches, char_u ***matches)
{
    expand_T *xp = args->oe_xp;

    જો (xp->xp_pattern > args->oe_set_arg && *(xp->xp_pattern-1) == ':')
    {
	જો (completing_કિંમત_for_subopt(args, "algorithm"))
	{
	    પાછા expand_set_opt_string(
		    args,
		    p_dip_algorithm_કિંમતો,
		    ARRAY_LENGTH(p_dip_algorithm_કિંમતો) - 1,
		    અંક_Matches,
		    matches);
	}
	જો (completing_કિંમત_for_subopt(args, "inline"))
	{
	    પાછા expand_set_opt_string(
		    args,
		    p_dip_inline_કિંમતો,
		    ARRAY_LENGTH(p_dip_inline_કિંમતો) - 1,
		    અંક_Matches,
		    matches);
	}
	પાછા FAIL;
    }

    પાછા expand_set_opt_string(
	    args,
	    p_dip_કિંમતો,
	    ARRAY_LENGTH(p_dip_કિંમતો) - 1,
	    અંક_Matches,
	    matches);
}
#endif

/*
 * The 'display' option is changed.
 */
    અક્ષર *
did_set_display(optset_T *args UNUSED)
{
    જો (opt_strings_flags(p_dy, p_dy_કિંમતો, &dy_flags, TRUE) != OK)
	પાછા e_invalid_argument;

    (શૂન્ય)init_chartab();
    પાછા ખાલી;
}

    પૂર્ણાંક
expand_set_display(optexpand_T *args, પૂર્ણાંક *અંક_Matches, char_u ***matches)
{
    પાછા expand_set_opt_string(
	    args,
	    p_dy_કિંમતો,
	    ARRAY_LENGTH(p_dy_કિંમતો) - 1,
	    અંક_Matches,
	    matches);
}

/*
 * The 'eadirection' option is changed.
 */
    અક્ષર *
did_set_eadirection(optset_T *args UNUSED)
{
    પાછા did_set_opt_strings(p_ead, p_ead_કિંમતો, FALSE);
}

    પૂર્ણાંક
expand_set_eadirection(optexpand_T *args, પૂર્ણાંક *અંક_Matches, char_u ***matches)
{
    પાછા expand_set_opt_string(
	    args,
	    p_ead_કિંમતો,
	    ARRAY_LENGTH(p_ead_કિંમતો) - 1,
	    અંક_Matches,
	    matches);
}

/*
 * One of the 'encoding', 'fileencoding', 'termencoding' or 'makeencoding'
 * options is changed.
 */
    અક્ષર *
did_set_encoding(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u	**gvarp;
    અક્ષર	*errmsg = ખાલી;
    char_u	*p;

    // Get the global option to compare with, otherwise we would have to check
    // two values for all local options.
    gvarp = (char_u **)get_option_varp_scope(args->os_idx, OPT_GLOBAL);

    જો (gvarp == &p_fenc)
    {
	જો (!curbuf->b_p_ma && args->os_flags != OPT_GLOBAL)
	    errmsg = e_cannot_make_changes_modifiable_is_off;
	નહીંતર જો (vim_strchr(*varp, ',') != ખાલી)
	    // No comma allowed in 'fileencoding'; catches confusing it
	    // with 'fileencodings'.
	    errmsg = e_invalid_argument;
	નહીંતર
	{
	    // May show a "+" in the title now.
	    redraw_titles();
	    // Add 'fileencoding' to the swap file.
	    ml_setflags(curbuf);
	}
    }
    જો (errmsg == ખાલી)
    {
	// canonize the value, so that STRCMP() can be used on it
	p = enc_canonize(*varp);
	જો (p != ખાલી)
	{
	    vim_free(*varp);
	    *varp = p;
	}
	જો (varp == &p_enc)
	{
	    errmsg = mb_init();
	    redraw_titles();
	}
    }

#જો defined(FEAT_GUI_GTK)
    જો (errmsg == ખાલી && varp == &p_tenc && gui.in_use)
    {
	// GTK uses only a single encoding, and that is UTF-8.
	જો (STRCMP(p_tenc, "utf-8") != 0)
	    errmsg = e_cannot_be_changed_in_gtk_GUI;
    }
#endif

    જો (errmsg == ખાલી)
    {
#ifdef FEAT_KEYMAP
	// When 'keymap' is used and 'encoding' changes, reload the keymap
	// (with another encoding).
	જો (varp == &p_enc && *curbuf->b_p_keymap != NUL)
	    (શૂન્ય)keymap_init();
#endif

	// When 'termencoding' is not empty and 'encoding' changes or when
	// 'termencoding' changes, need to setup for keyboard input and
	// display output conversion.
	જો (((varp == &p_enc && *p_tenc != NUL) || varp == &p_tenc))
	{
	    જો (convert_setup(&આગત_conv, p_tenc, p_enc) == FAIL
		    || convert_setup(&output_conv, p_enc, p_tenc) == FAIL)
	    {
		semsg(_(e_cannot_convert_between_str_and_str),
			p_tenc, p_enc);
		errmsg = e_invalid_argument;
	    }
	}

#જો defined(MSWIN)
	// $HOME, $VIM and $VIMRUNTIME may have characters in active code page.
	જો (varp == &p_enc)
	{
	    init_homedir();
	    init_vimdir();
	}
#endif
    }

    પાછા errmsg;
}

    પૂર્ણાંક
expand_set_encoding(optexpand_T *args, પૂર્ણાંક *અંક_Matches, char_u ***matches)
{
    પાછા expand_set_opt_generic(
	    args,
	    get_encoding_name,
	    અંક_Matches,
	    matches);
}

/*
 * The 'eventignore(win)' option is changed.
 */
    અક્ષર *
did_set_eventignore(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u	*oldval = args->os_oldval.string;

    જો (check_ei(*varp) == FAIL)
	પાછા e_invalid_argument;

    જો (oldval == ખાલી || STRCMP(oldval, *varp) == 0)
	પાછા ખાલી;

    // Deal with the events that are triggered by comparing against a stored
    // state, with the old value in effect: what happened while an event was
    // ignored must not be reported once it is not ignored anymore, and what
    // happened before must still be reported.
    // Use a copy, the caller owns "oldval" and autocommands may free it.
    char_u	*save_ei = vim_strsave(oldval);
    જો (save_ei != ખાલી)
    {
	char_u	*newval = *varp;
	win_T	*wp = is_window_local_option(args->os_idx) ? curwin : ખાલી;

	*varp = save_ei;
	// "varp" points into "wp" for 'eventignorewin'.
	જો (wp != ખાલી)
	    ++wp->w_locked;
	may_trigger_deferred_events();
	જો (wp != ખાલી)
	    --wp->w_locked;

	free_string_option(*varp);
	*varp = newval;
    }

    પાછા ખાલી;
}

સ્થિર પૂર્ણાંક expand_eiw = FALSE;

    સ્થિર char_u *
get_eventignore_name(expand_T *xp, પૂર્ણાંક idx)
{
    પૂર્ણાંક subtract = *xp->xp_pattern == '-';
    // 'eventignore(win)' allows special keyword "all" in addition to
    // all event names.
    જો (!subtract && idx == 0)
	પાછા (char_u *)"all";

    char_u *name = get_event_name_no_group(xp, idx - 1 + subtract, expand_eiw);
    જો (name == ખાલી)
	પાછા ખાલી;

    sprintf((અક્ષર *)IObuff, "%s%s", subtract ? "-" : "", name);
    પાછા IObuff;
}

    પૂર્ણાંક
expand_set_eventignore(optexpand_T *args, પૂર્ણાંક *અંક_Matches, char_u ***matches)
{
    expand_eiw = args->oe_varp != (char_u *)&p_ei;
    પાછા expand_set_opt_generic(
	    args,
	    get_eventignore_name,
	    અંક_Matches,
	    matches);
}

/*
 * The 'fileformat' option is changed.
 */
    અક્ષર *
did_set_fileformat(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    જો (!curbuf->b_p_ma && !(args->os_flags & OPT_GLOBAL))
	પાછા e_cannot_make_changes_modifiable_is_off;
    નહીંતર જો (check_opt_strings(*varp, p_ff_કિંમતો, FALSE) != OK)
	પાછા e_invalid_argument;

    // may also change 'textmode'
    જો (get_fileformat(curbuf) == EOL_DOS)
	curbuf->b_p_tx = TRUE;
    નહીંતર
	curbuf->b_p_tx = FALSE;
    redraw_titles();
    // update flag in swap file
    ml_setflags(curbuf);
    // Redraw needed when switching to/from "mac": a CR in the text
    // will be displayed differently.
    જો (get_fileformat(curbuf) == EOL_MAC || *args->os_oldval.string == 'm')
	redraw_curbuf_later(UPD_NOT_VALID);

    પાછા ખાલી;
}

    પૂર્ણાંક
expand_set_fileformat(optexpand_T *args, પૂર્ણાંક *અંક_Matches, char_u ***matches)
{
    પાછા expand_set_opt_string(
	    args,
	    p_ff_કિંમતો,
	    ARRAY_LENGTH(p_ff_કિંમતો) - 1,
	    અંક_Matches,
	    matches);
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * fileformat options.
 */
    char_u *
get_fileformat_name(expand_T *xp UNUSED, પૂર્ણાંક idx)
{
    જો (idx >= (પૂર્ણાંક)ARRAY_LENGTH(p_ff_કિંમતો))
	પાછા ખાલી;

    પાછા (char_u*)p_ff_કિંમતો[idx];
}

/*
 * The 'fileformats' option is changed.
 */
    અક્ષર *
did_set_fileformats(optset_T *args UNUSED)
{
    જો (check_opt_strings(p_ffs, p_ff_કિંમતો, TRUE) != OK)
	પાછા e_invalid_argument;

    // also change 'textauto'
    જો (*p_ffs == NUL)
	p_ta = FALSE;
    નહીંતર
	p_ta = TRUE;

    પાછા ખાલી;
}

/*
 * The 'filetype' or the 'syntax' option is changed.
 */
    અક્ષર *
did_set_filetype_or_syntax(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    જો (!valid_filetype(*varp))
	પાછા e_invalid_argument;

    args->os_કિંમત_changed = STRCMP(args->os_oldval.string, *varp) != 0;

    // Since we check the value, there is no need to set P_INSECURE,
    // even when the value comes from a modeline.
    args->os_કિંમત_checked = TRUE;

    પાછા ખાલી;
}

#જો defined(FEAT_FOLDING)
/*
 * The 'foldclose' option is changed.
 */
    અક્ષર *
did_set_foldclose(optset_T *args UNUSED)
{
    પાછા did_set_opt_strings(p_fcl, p_fcl_કિંમતો, TRUE);
}

    પૂર્ણાંક
expand_set_foldclose(optexpand_T *args, પૂર્ણાંક *અંક_Matches, char_u ***matches)
{
    પાછા expand_set_opt_string(
	    args,
	    p_fcl_કિંમતો,
	    ARRAY_LENGTH(p_fcl_કિંમતો) - 1,
	    અંક_Matches,
	    matches);
}
#endif

#જો defined(FEAT_EVAL) && defined(FEAT_FOLDING)
/*
 * The 'foldexpr' option is changed.
 */
    અક્ષર *
did_set_foldexpr(optset_T *args)
{
    (શૂન્ય)did_set_optexpr(args);
    જો (foldmethod_Is_પદાવલિ(curwin))
	foldUpdateAll(curwin);
    પાછા ખાલી;
}
#endif

#જો defined(FEAT_FOLDING)
/*
 * The 'foldignore' option is changed.
 */
    અક્ષર *
did_set_foldignore(optset_T *args UNUSED)
{
    જો (foldmethodIsIndent(curwin))
	foldUpdateAll(curwin);
    પાછા ખાલી;
}

/*
 * The 'foldmarker' option is changed.
 */
    અક્ષર *
did_set_foldmarker(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u	*p;

    p = vim_strchr(*varp, ',');
    જો (p == ખાલી)
	પાછા e_comma_required;
    નહીંતર જો (p == *varp || p[1] == NUL)
	પાછા e_invalid_argument;
    નહીંતર જો (foldmethodIsMarker(curwin))
	foldUpdateAll(curwin);

    પાછા ખાલી;
}

/*
 * The 'foldmethod' option is changed.
 */
    અક્ષર *
did_set_foldmethod(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    જો (check_opt_strings(*varp, p_fdm_કિંમતો, FALSE) != OK || **varp == NUL)
	પાછા e_invalid_argument;

    foldUpdateAll(curwin);
    જો (foldmethodIsDiff(curwin))
	newFoldLevel();
    પાછા ખાલી;
}

    પૂર્ણાંક
expand_set_foldmethod(optexpand_T *args, પૂર્ણાંક *અંક_Matches, char_u ***matches)
{
    પાછા expand_set_opt_string(
	    args,
	    p_fdm_કિંમતો,
	    ARRAY_LENGTH(p_fdm_કિંમતો) - 1,
	    અંક_Matches,
	    matches);
}

/*
 * The 'foldopen' option is changed.
 */
    અક્ષર *
did_set_foldopen(optset_T *args UNUSED)
{
    પાછા did_set_opt_flags(p_fdo, p_fdo_કિંમતો, &fdo_flags, TRUE);
}

    પૂર્ણાંક
expand_set_foldopen(optexpand_T *args, પૂર્ણાંક *અંક_Matches, char_u ***matches)
{
    પાછા expand_set_opt_string(
	    args,
	    p_fdo_કિંમતો,
	    ARRAY_LENGTH(p_fdo_કિંમતો) - 1,
	    અંક_Matches,
	    matches);
}
#endif

/*
 * The 'formatoptions' option is changed.
 */
    અક્ષર *
did_set_formatoptions(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    પાછા did_set_option_listflag(*varp, (char_u *)FO_ALL, args->os_errbuf,
		    args->os_errbuflen);
}

    પૂર્ણાંક
expand_set_formatoptions(optexpand_T *args, પૂર્ણાંક *અંક_Matches, char_u ***matches)
{
    પાછા expand_set_opt_listflag(args, (char_u*)FO_ALL, અંક_Matches, matches);
}

#જો defined(CURSOR_SHAPE)
/*
 * The 'guicursor' option is changed.
 */
    અક્ષર *
did_set_guicursor(optset_T *args UNUSED)
{
    પાછા parse_shape_opt(SHAPE_CURSOR);
}
#endif

#જો defined(FEAT_GUI)
/*
 * The 'guifont' option is changed.
 */
    અક્ષર *
did_set_guifont(optset_T *args UNUSED)
{
    char_u	*p;
    અક્ષર	*errmsg = ખાલી;

    જો (gui.in_use)
    {
	p = p_guifont;
# જો defined(FEAT_GUI_GTK)
	// Put up a font dialog and let the user select a new value.
	// If this is cancelled go back to the old value but don't
	// give an error message.
	જો (STRCMP(p, "*") == 0)
	{
	    p = gui_mch_font_dialog(args->os_oldval.string);
	    free_string_option(p_guifont);
	    p_guifont = (p != ખાલી) ? p : vim_strsave(args->os_oldval.string);
	}
# endif
	જો (p != ખાલી && gui_init_font(p_guifont, FALSE) != OK)
	{
# જો defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_PHOTON)
	    જો (STRCMP(p_guifont, "*") == 0)
	    {
		// Dialog was cancelled: Keep the old value without giving
		// an error message.
		free_string_option(p_guifont);
		p_guifont = vim_strsave(args->os_oldval.string);
	    }
	    નહીંતર
# endif
		errmsg = e_invalid_fonts;
	}
    }

    પાછા errmsg;
}

/*
 * Expand the 'guifont' option. Only when GUI is being used. Each platform has
 * specific behaviors.
 */
    પૂર્ણાંક
expand_set_guifont(optexpand_T *args, પૂર્ણાંક *અંક_Matches, char_u ***matches)
{
    જો (!gui.in_use)
	પાછા FAIL;

# જો defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
    char_u **varp = (char_u **)args->oe_varp;
    પૂર્ણાંક wide = (varp == &p_guifontwide);

    પાછા expand_set_opt_callback(
	    args, gui_mch_expand_font, &wide, અંક_Matches, matches);
# નહીંતર
    પાછા FAIL;
# endif
}

# જો defined(FEAT_XFONTSET)
/*
 * The 'guifontset' option is changed.
 */
    અક્ષર *
did_set_guifontset(optset_T *args UNUSED)
{
    અક્ષર *errmsg = ખાલી;

    જો (STRCMP(p_guifontset, "*") == 0)
	errmsg = e_cant_select_fontset;
    નહીંતર જો (gui.in_use && gui_init_font(p_guifontset, TRUE) != OK)
	errmsg = e_invalid_fontset;

    પાછા errmsg;
}
# endif

/*
 * The 'guifontwide' option is changed.
 */
    અક્ષર *
did_set_guifontwide(optset_T *args UNUSED)
{
    અક્ષર *errmsg = ખાલી;

    જો (STRCMP(p_guifontwide, "*") == 0)
	errmsg = e_cant_select_wide_font;
    નહીંતર જો (gui_get_wide_font() == FAIL)
	errmsg = e_invalid_wide_font;

    પાછા errmsg;
}
#endif

#જો defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
/*
 * The 'guiligatures' option is changed.
 */
    અક્ષર *
did_set_guiligatures(optset_T *args UNUSED)
{
    gui_set_ligatures();
    પાછા ખાલી;
}
#endif

#જો defined(FEAT_GUI)
/*
 * The 'guioptions' option is changed.
 */
    અક્ષર *
did_set_guioptions(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    અક્ષર *errmsg;

    errmsg = did_set_option_listflag(*varp, (char_u *)GO_ALL, args->os_errbuf,
		    args->os_errbuflen);
    જો (errmsg != ખાલી)
	પાછા errmsg;

    gui_init_which_components(args->os_oldval.string);
    પાછા ખાલી;
}

    પૂર્ણાંક
expand_set_guioptions(optexpand_T *args, પૂર્ણાંક *અંક_Matches, char_u ***matches)
{
    પાછા expand_set_opt_listflag(args, (char_u*)GO_ALL, અંક_Matches, matches);
}
#endif

#જો defined(FEAT_GUI_TABLINE)
/*
 * The 'guitablabel' option is changed.
 */
    અક્ષર *
did_set_guitablabel(optset_T *args UNUSED)
{
    redraw_tabline = TRUE;
    પાછા ખાલી;
}
#endif

/*
 * The 'helpfile' option is changed.
 */
    અક્ષર *
did_set_helpfile(optset_T *args UNUSED)
{
    // May compute new values for $VIM and $VIMRUNTIME
    જો (didset_vim)
	vim_unsetenv_ext((char_u *)"VIM");
    જો (didset_vimruntime)
	vim_unsetenv_ext((char_u *)"VIMRUNTIME");
    પાછા ખાલી;
}

#જો defined(FEAT_MULTI_LANG)
/*
 * The 'helplang' option is changed.
 */
    અક્ષર *
did_set_helplang(optset_T *args UNUSED)
{
    અક્ષર *errmsg = ખાલી;

    // Check for "", "ab", "ab,cd", etc.
    માટે (char_u *s = p_hlg; *s != NUL; s += 3)
    {
	જો (s[1] == NUL || ((s[2] != ',' || s[3] == NUL) && s[2] != NUL))
	{
	    errmsg = e_invalid_argument;
	    તોડો;
	}
	જો (s[2] == NUL)
	    તોડો;
    }

    પાછા errmsg;
}
#endif

/*
 * The 'highlight' option is changed.
 */
    અક્ષર *
did_set_highlight(optset_T *args UNUSED)
{
    જો (highlight_changed() == FAIL)
	પાછા e_invalid_argument;	// invalid flags

    પાછા ખાલી;
}

    સ્થિર પૂર્ણાંક
expand_hl_occasions(
	optexpand_T *args,
	પૂર્ણાંક *અંક_Matches,
	char_u ***matches,
	char_u prefix)
{
    char_u	    *p;
    સ્થિર char_u   hl_flags[HLF_ગણતરી] = HL_FLAGS;
    size_t	    i;
    પૂર્ણાંક		    ગણતરી = 0;

    *matches = ALLOC_MULT(char_u *, HLF_ગણતરી + 1);
    જો (*matches == ખાલી)
	પાછા FAIL;

    // We still want to return the full option if it's requested.
    જો (args->oe_include_orig_val)
    {
	p = vim_strsave(args->oe_opt_કિંમત);
	જો (p == ખાલી)
	{
	    VIM_CLEAR(*matches);
	    પાછા FAIL;
	}
	(*matches)[ગણતરી++] = p;
    }

    માટે (i = 0; i < HLF_ગણતરી; i++)
    {
	p = alloc((prefix == NUL ? 1 : 2) + 1);
	જો (p == ખાલી)
	{
	    જો (ગણતરી == 0)
	    {
		VIM_CLEAR(*matches);
		પાછા FAIL;
	    }
	    નહીંતર
		તોડો;
	}
	જો (prefix == NUL)
	    sprintf((અક્ષર *)p, "%c", hl_flags[i]);
	નહીંતર
	    sprintf((અક્ષર *)p, "%c%c", prefix, hl_flags[i]);
	(*matches)[ગણતરી++] = p;
    }

    જો (ગણતરી == 0)
    {
	VIM_CLEAR(*matches);
	પાછા FAIL;
    }
    *અંક_Matches = ગણતરી;
    પાછા OK;
}

/*
 * Expand 'highlight' option.
 */
    પૂર્ણાંક
expand_set_highlight(optexpand_T *args, પૂર્ણાંક *અંક_Matches, char_u ***matches)
{
    char_u	    *p;
    expand_T	    *xp = args->oe_xp;
    size_t	    i;
    પૂર્ણાંક		    ગણતરી = 0;

    જો (xp->xp_pattern > args->oe_set_arg && *(xp->xp_pattern-1) == ':')
    {
	// Right after a ':', meaning we just return all highlight names.
	પાછા expand_set_opt_generic(
		args,
		get_highlight_name,
		અંક_Matches,
		matches);
    }

    જો (*xp->xp_pattern == NUL)
	// At beginning of a comma-separated list. Return the specific list of
	// supported occasions.
	પાછા expand_hl_occasions(args, અંક_Matches, matches, NUL);

    // We are after the initial character (which indicates the occasion). We
    // already made sure we are not matching after a ':' above, so now we want
    // to match against display mode modifiers.
    // Since the xp_pattern starts from the beginning, we need to include it in
    // the returned match.

    // Note: Keep this in sync with highlight_changed()
    સ્થિર char_u p_hl_mode_કિંમતો[] =
	{':', 'b', 'i', '-', 'n', 'r', 's', 'u', 'c', '2', 'd', '=', 't'};
    size_t અંક_hl_modes = ARRAY_LENGTH(p_hl_mode_કિંમતો);

    *matches = ALLOC_MULT(char_u *, અંક_hl_modes);
    જો (*matches == ખાલી)
	પાછા FAIL;

    પૂર્ણાંક pattern_len = xp->xp_pattern_len;

    માટે (i = 0; i < અંક_hl_modes; i++)
    {
	// Don't allow duplicates as these are unique flags
	char_u *dup = vim_strchr(xp->xp_pattern + 1, p_hl_mode_કિંમતો[i]);
	જો (dup != ખાલી && (પૂર્ણાંક)(dup - xp->xp_pattern) < pattern_len)
	    ચાલુ;

	// ':' only works by itself, not with other flags.
	જો (pattern_len > 1 && p_hl_mode_કિંમતો[i] == ':')
	    ચાલુ;

	p = vim_strnsave(xp->xp_pattern, pattern_len + 1);
	જો (p == ખાલી)
	{
	    જો (i == 0)
	    {
		VIM_CLEAR(*matches);
		પાછા FAIL;
	    }
	    નહીંતર
		તોડો;
	}
	p[pattern_len] = p_hl_mode_કિંમતો[i];
	p[pattern_len + 1] = NUL;
	(*matches)[ગણતરી++] = p;
    }
    જો (ગણતરી == 0)
    {
	VIM_CLEAR(*matches);
	પાછા FAIL;
    }
    *અંક_Matches = ગણતરી;

    પાછા OK;
}

/*
 * The 'titlestring' or the 'iconstring' option is changed.
 */
    સ્થિર અક્ષર *
parse_titleiconstring(optset_T *args UNUSED, પૂર્ણાંક flagval UNUSED)
{
#ifdef FEAT_STL_OPT
    char_u	**varp = (char_u **)args->os_varp;

    // NULL => statusline syntax
    જો (vim_strchr(*varp, '%') && check_stl_option(*varp) == ખાલી)
	stl_syntax |= flagval;
    નહીંતર
	stl_syntax &= ~flagval;
#endif
    did_set_title();

    પાછા ખાલી;
}

/*
 * The 'iconstring' option is changed.
 */
    અક્ષર *
did_set_iconstring(optset_T *args)
{
    પૂર્ણાંક flagval = 0;

#ifdef FEAT_STL_OPT
    flagval = STL_IN_ICON;
#endif

    પાછા parse_titleiconstring(args, flagval);
}

#જો defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
/*
 * The 'imactivatekey' option is changed.
 */
    અક્ષર *
did_set_imactivatekey(optset_T *args UNUSED)
{
    જો (!im_xim_isvalid_imactivate())
	પાછા e_invalid_argument;
    પાછા ખાલી;
}
#endif

#ifdef FEAT_IMAGE
/*
 * The 'imageprotocol' option is changed.
 */
    અક્ષર *
did_set_imageprotocol(optset_T *args UNUSED)
{
    જો (update_image_backend() == FAIL)
	પાછા e_invalid_argument;
    પાછા ખાલી;
}

    પૂર્ણાંક
expand_set_imageprotocol(optexpand_T *args, પૂર્ણાંક *અંક_Matches, char_u ***matches)
{
    expand_T *xp = args->oe_xp;

    જો (xp->xp_pattern > args->oe_set_arg && *(xp->xp_pattern-1) == ':')
    {
	// 'imageprotocol' only has well-defined terms for completion for the
	// protocol part after the colon.
	પાછા expand_set_opt_string(
		args,
		p_ipc_protocol_કિંમતો,
		ARRAY_LENGTH(p_ipc_protocol_કિંમતો) - 1,
		અંક_Matches,
		matches);
    }
    // Use expand_set_opt_string instead of returning FAIL so that we can
    // include the original value if args->oe_include_orig_val is set.
    સ્થિર અક્ષર *(empty[]) = {ખાલી};
    પાછા expand_set_opt_string(args, empty, 0, અંક_Matches, matches);
}
#endif

/*
 * The 'iskeyword' option is changed.
 */
    અક્ષર *
did_set_iskeyword(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    જો (varp == &p_isk)		// only check for global-value
    {
	જો (check_isopt(*varp) == FAIL)
	    પાછા e_invalid_argument;
    }
    નહીંતર			// fallthrough for local-value
	પાછા did_set_isopt(args);

    પાછા ખાલી;
}

/*
 * The 'isident' or the 'iskeyword' or the 'isprint' or the 'isfname' option is
 * changed.
 */
    અક્ષર *
did_set_isopt(optset_T *args)
{
    // 'isident', 'iskeyword', 'isprint' or 'isfname' option: refill g_chartab[]
    // If the new option is invalid, use old value.
    // 'lisp' option: refill g_chartab[] for '-' char.
    જો (init_chartab() == FAIL)
    {
	args->os_restore_chartab = TRUE;// need to restore the chartab.
	પાછા e_invalid_argument;	// error in value
    }

    પાછા ખાલી;
}

/*
 * The 'jumpoptions' option is changed.
 */
    અક્ષર *
did_set_jumpoptions(optset_T *args UNUSED)
{
    જો (opt_strings_flags(p_jop, p_jop_કિંમતો, &jop_flags, TRUE) != OK)
	પાછા e_invalid_argument;

    પાછા ખાલી;
}

    પૂર્ણાંક
expand_set_jumpoptions(optexpand_T *args, પૂર્ણાંક *અંક_Matches, char_u ***matches)
{
    પાછા expand_set_opt_string(
	    args,
	    p_jop_કિંમતો,
	    ARRAY_LENGTH(p_jop_કિંમતો) - 1,
	    અંક_Matches,
	    matches);
}

#જો defined(FEAT_KEYMAP)
/*
 * The 'keymap' option is changed.
 */
    અક્ષર *
did_set_keymap(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    અક્ષર *errmsg = ખાલી;

    જો (!valid_filetype(*varp))
	errmsg = e_invalid_argument;
    નહીંતર
    {
	પૂર્ણાંક	    secure_save = secure;

	// Reset the secure flag, since the value of 'keymap' has
	// been checked to be safe.
	secure = 0;

	// load or unload key mapping tables
	errmsg = keymap_init();

	secure = secure_save;

	// Since we check the value, there is no need to set P_INSECURE,
	// even when the value comes from a modeline.
	args->os_કિંમત_checked = TRUE;
    }

    જો (errmsg == ખાલી)
    {
	જો (*curbuf->b_p_keymap != NUL)
	{
	    // Installed a new keymap, switch on using it.
	    curbuf->b_p_iminsert = B_IMODE_LMAP;
	    જો (curbuf->b_p_imsearch != B_IMODE_USE_ઉમેરો)
		curbuf->b_p_imsearch = B_IMODE_LMAP;
	}
	નહીંતર
	{
	    // Cleared the keymap, may reset 'iminsert' and 'imsearch'.
	    જો (curbuf->b_p_iminsert == B_IMODE_LMAP)
		curbuf->b_p_iminsert = B_IMODE_NONE;
	    જો (curbuf->b_p_imsearch == B_IMODE_LMAP)
		curbuf->b_p_imsearch = B_IMODE_USE_ઉમેરો;
	}
	જો ((args->os_flags & OPT_LOCAL) == 0)
	{
	    set_iminsert_global();
	    set_imsearch_global();
	}
	status_redraw_curbuf();
    }

    પાછા errmsg;
}
#endif

/*
 * The 'keymodel' option is changed.
 */
    અક્ષર *
did_set_keymodel(optset_T *args UNUSED)
{
    જો (check_opt_strings(p_km, p_km_કિંમતો, TRUE) != OK)
	પાછા e_invalid_argument;

    km_stopsel = (vim_strchr(p_km, 'o') != ખાલી);
    km_startsel = (vim_strchr(p_km, 'a') != ખાલી);
    પાછા ખાલી;
}

    પૂર્ણાંક
expand_set_keymodel(optexpand_T *args, પૂર્ણાંક *અંક_Matches, char_u ***matches)
{
    પાછા expand_set_opt_string(
	    args,
	    p_km_કિંમતો,
	    ARRAY_LENGTH(p_km_કિંમતો) - 1,
	    અંક_Matches,
	    matches);
}

/*
 * The 'keyprotocol' option is changed.
 */
    અક્ષર *
did_set_keyprotocol(optset_T *args UNUSED)
{
    char_u *term = T_NAME;
    keyprot_T kpc = match_keyprotocol(term);
    જો (kpc == KEYPROTOCOL_FAIL)
	પાછા e_invalid_argument;

    apply_keyprotocol(term, kpc);

    પાછા ખાલી;
}

    પૂર્ણાંક
expand_set_keyprotocol(optexpand_T *args, પૂર્ણાંક *અંક_Matches, char_u ***matches)
{
    expand_T *xp = args->oe_xp;
    જો (xp->xp_pattern > args->oe_set_arg && *(xp->xp_pattern-1) == ':')
    {
	// 'keyprotocol' only has well-defined terms for completion for the
	// protocol part after the colon.
	પાછા expand_set_opt_string(
		args,
		p_kpc_protocol_કિંમતો,
		ARRAY_LENGTH(p_kpc_protocol_કિંમતો) - 1,
		અંક_Matches,
		matches);
    }
    // Use expand_set_opt_string instead of returning FAIL so that we can
    // include the original value if args->oe_include_orig_val is set.
    સ્થિર અક્ષર *(empty[]) = {ખાલી};
    પાછા expand_set_opt_string(args, empty, 0, અંક_Matches, matches);
}

/*
 * The 'lispoptions' option is changed.
 */
    અક્ષર *
did_set_lispoptions(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    જો (**varp != NUL
	    && STRCMP(*varp, "expr:0") != 0
	    && STRCMP(*varp, "expr:1") != 0)
	પાછા e_invalid_argument;

    પાછા ખાલી;
}

    પૂર્ણાંક
expand_set_lispoptions(optexpand_T *args, પૂર્ણાંક *અંક_Matches, char_u ***matches)
{
    સ્થિર અક્ષર *(p_lop_કિંમતો[]) = {"expr:0", "expr:1", ખાલી};
    પાછા expand_set_opt_string(
	    args,
	    p_lop_કિંમતો,
	    ARRAY_LENGTH(p_lop_કિંમતો) - 1,
	    અંક_Matches,
	    matches);
}

/*
 * The 'matchpairs' option is changed.
 */
    અક્ષર *
did_set_matchpairs(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u	*p;

    જો (has_mbyte)
    {
	માટે (p = *varp; *p != NUL; ++p)
	{
	    પૂર્ણાંક x2 = -1;
	    પૂર્ણાંક x3 = -1;

	    p += mb_ptr2len(p);
	    જો (*p != NUL)
		x2 = *p++;
	    જો (*p != NUL)
	    {
		x3 = mb_ptr2char(p);
		p += mb_ptr2len(p);
	    }
	    જો (x2 != ':' || x3 == -1 || (*p != NUL && *p != ','))
		પાછા e_invalid_argument;
	    જો (*p == NUL)
		તોડો;
	}
    }
    નહીંતર
    {
	// Check for "x:y,x:y"
	માટે (p = *varp; *p != NUL; p += 4)
	{
	    જો (p[1] != ':' || p[2] == NUL || (p[3] != NUL && p[3] != ','))
		પાછા e_invalid_argument;
	    જો (p[3] == NUL)
		તોડો;
	}
    }

    પાછા ખાલી;
}

/*
 * Process the updated 'messagesopt' option value.
 */
    અક્ષર *
did_set_messagesopt(optset_T *args UNUSED)
{
    જો (messagesopt_changed() == FAIL)
	પાછા e_invalid_argument;

    પાછા ખાલી;
}

    પૂર્ણાંક
expand_set_messagesopt(optexpand_T *args, પૂર્ણાંક *અંક_Matches, char_u ***matches)
{
    સ્થિર અક્ષર *(p_mopt_કિંમતો[]) = {"hit-enter", "wait:", "history:", ખાલી};
    પાછા expand_set_opt_string(
	    args,
	    p_mopt_કિંમતો,
	    ARRAY_LENGTH(p_mopt_કિંમતો) - 1,
	    અંક_Matches,
	    matches);
}

#જો defined(FEAT_SPELL)
/*
 * The 'mkspellmem' option is changed.
 */
    અક્ષર *
did_set_mkspellmem(optset_T *args UNUSED)
{
    જો (spell_check_msm() != OK)
	પાછા e_invalid_argument;

    પાછા ખાલી;
}
#endif

/*
 * The 'mouse' option is changed.
 */
    અક્ષર *
did_set_mouse(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    અક્ષર	*retval;

    retval = did_set_option_listflag(*varp, (char_u *)MOUSE_ALL, args->os_errbuf,
		    args->os_errbuflen);
    જો (retval == ખાલી)
    {
	redraw_tabline = TRUE;
	જો (tabline_height() > 0)
	    update_screen(UPD_VALID);
#જો defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
	popup_close_info(); // Close info popup to apply new properties
#endif
    }
    પાછા retval;
}

    પૂર્ણાંક
expand_set_mouse(optexpand_T *args, પૂર્ણાંક *અંક_Matches, char_u ***matches)
{
    પાછા expand_set_opt_listflag(args, (char_u*)MOUSE_ALL, અંક_Matches, matches);
}

/*
 * The 'mousemodel' option is changed.
 */
    અક્ષર *
did_set_mousemodel(optset_T *args UNUSED)
{
    જો (check_opt_strings(p_mousem, p_mousem_કિંમતો, FALSE) != OK)
	પાછા e_invalid_argument;
#જો defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU) && (XmVersion <= 1002)
    નહીંતર જો (*p_mousem != *args->os_oldval.string)
	// Changed from "extend" to "popup" or "popup_setpos" or vv: need
	// to create or delete the popup menus.
	gui_motif_update_mousemodel(મૂળ_menu);
#endif

    પાછા ખાલી;
}

    પૂર્ણાંક
expand_set_mousemodel(optexpand_T *args, પૂર્ણાંક *અંક_Matches, char_u ***matches)
{
    પાછા expand_set_opt_string(
	    args,
	    p_mousem_કિંમતો,
	    ARRAY_LENGTH(p_mousem_કિંમતો) - 1,
	    અંક_Matches,
	    matches);
}

#જો defined(FEAT_MOUSESHAPE)
    અક્ષર *
did_set_mouseshape(optset_T *args UNUSED)
{
    અક્ષર *errmsg = ખાલી;

    errmsg = parse_shape_opt(SHAPE_MOUSE);
    update_mouseshape(-1);

    પાછા errmsg;
}
#endif

/*
 * The 'nrformats' option is changed.
 */
    અક્ષર *
did_set_nrformats(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    પાછા did_set_opt_strings(*varp, p_nf_કિંમતો, TRUE);
}

    પૂર્ણાંક
expand_set_nrformats(optexpand_T *args, પૂર્ણાંક *અંક_Matches, char_u ***matches)
{
    પાછા expand_set_opt_string(
	    args,
	    p_nf_કિંમતો,
	    ARRAY_LENGTH(p_nf_કિંમતો) - 1,
	    અંક_Matches,
	    matches);
}

#જો defined(FEAT_EVAL)
/*
 * One of the '*expr' options is changed: 'balloonexpr', 'diffexpr',
 * 'foldexpr', 'foldtext', 'formatexpr', 'includeexpr', 'indentexpr',
 * 'patchexpr', 'printexpr' and 'charconvert'.
 */
    અક્ષર *
did_set_optexpr(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    // If the option value starts with <SID> or s:, then replace that with
    // the script identifier.
    char_u *name = get_scriptlocal_funcname(*varp);
    જો (name != ખાલી)
    {
	free_string_option(*varp);
	*varp = name;
    }

    પાછા ખાલી;
}
#endif

/*
 * The 'pastetoggle' option is changed.
 */
    અક્ષર *
did_set_pastetoggle(optset_T *args UNUSED)
{
    char_u	*p;

    // translate key codes like in a mapping
    જો (*p_pt)
    {
	(શૂન્ય)replace_termcodes(p_pt, &p, 0,
		REPTERM_FROM_PART | REPTERM_DO_LT, ખાલી);
	જો (p != ખાલી)
	{
	    free_string_option(p_pt);
	    p_pt = p;
	}
    }

    પાછા ખાલી;
}

#જો defined(FEAT_PROP_POPUP)
/*
 * The 'previewpopup' option is changed.
 */
    અક્ષર *
did_set_previewpopup(optset_T *args UNUSED)
{
    જો (parse_previewpopup(ખાલી) == FAIL)
	પાછા e_invalid_argument;

# જો defined(FEAT_QUICKFIX)
    popup_close_info();
# endif
    પાછા ખાલી;
}

    સ્થિર પૂર્ણાંક
expand_set_popupoption(optexpand_T *args, પૂર્ણાંક *અંક_Matches, char_u ***matches,
	પૂર્ણાંક previewpopup)
{
    expand_T *xp = args->oe_xp;

    જો (xp->xp_pattern > args->oe_set_arg && *(xp->xp_pattern-1) == ':')
    {
	જો (completing_કિંમત_for_subopt(args, "close")
		|| completing_કિંમત_for_subopt(args, "resize")
		|| completing_કિંમત_for_subopt(args, "shadow"))
	{
	    પાછા expand_set_opt_string(
		    args,
		    p_popup_option_on_off_કિંમતો,
		    ARRAY_LENGTH(p_popup_option_on_off_કિંમતો) - 1,
		    અંક_Matches,
		    matches);
	}
	જો (completing_કિંમત_for_subopt(args, "border"))
	{
	    પાછા expand_set_opt_string(
		    args,
		    previewpopup ? p_popup_pvp_border_કિંમતો
			: p_popup_cpp_border_કિંમતો,
		    (previewpopup ? ARRAY_LENGTH(p_popup_pvp_border_કિંમતો) - 1
			: ARRAY_LENGTH(p_popup_cpp_border_કિંમતો)) - 1,
		    અંક_Matches,
		    matches);
	}
	જો (completing_કિંમત_for_subopt(args, "align"))
	{
	    પાછા expand_set_opt_string(
		    args,
		    p_popup_option_align_કિંમતો,
		    ARRAY_LENGTH(p_popup_option_align_કિંમતો) - 1,
		    અંક_Matches,
		    matches);
	}
	જો (completing_કિંમત_for_subopt(args, "highlight")
		|| completing_કિંમત_for_subopt(args, "borderhighlight"))
	{
	    // Return the list of all highlight names
	    પાછા expand_set_opt_generic(
		    args,
		    get_highlight_name,
		    અંક_Matches,
		    matches);
	}
	પાછા FAIL;
    }

    પાછા expand_set_opt_string(
	    args,
	    previewpopup ? p_popup_pvp_option_કિંમતો
		: p_popup_cpp_option_કિંમતો,
	    previewpopup ? ARRAY_LENGTH(p_popup_pvp_option_કિંમતો) - 1
		: ARRAY_LENGTH(p_popup_cpp_option_કિંમતો) - 1,
	    અંક_Matches,
	    matches);
}

    પૂર્ણાંક
expand_set_previewpopup(optexpand_T *args, પૂર્ણાંક *અંક_Matches, char_u ***matches)
{
    પાછા expand_set_popupoption(args, અંક_Matches, matches, TRUE);
}

    પૂર્ણાંક
expand_set_completepopup(optexpand_T *args, પૂર્ણાંક *અંક_Matches, char_u ***matches)
{
    પાછા expand_set_popupoption(args, અંક_Matches, matches, FALSE);
}
#endif

#જો defined(FEAT_POSTSCRIPT)
/*
 * The 'printencoding' option is changed.
 */
    અક્ષર *
did_set_printencoding(optset_T *args UNUSED)
{
    char_u	*s, *p;

    // Canonize 'printencoding' if VIM standard one
    p = enc_canonize(p_penc);
    જો (p != ખાલી)
    {
	vim_free(p_penc);
	p_penc = p;
    }
    નહીંતર
    {
	// Ensure lower case and '-' for '_'
	માટે (s = p_penc; *s != NUL; s++)
	{
	    જો (*s == '_')
		*s = '-';
	    નહીંતર
		*s = TOLOWER_ASC(*s);
	}
    }

    પાછા ખાલી;
}
#endif

#જો defined(FEAT_PRINTER)

    સ્થિર char_u *
get_printoptions_names(expand_T *xp UNUSED, પૂર્ણાંક idx)
{
    જો (idx >= (પૂર્ણાંક)ARRAY_LENGTH(printer_opts))
	પાછા ખાલી;
    પાછા (char_u*)printer_opts[idx].name;
}

/*
 * Expand 'printoptions' option
 */
    પૂર્ણાંક
expand_set_printoptions(optexpand_T *args, પૂર્ણાંક *અંક_Matches, char_u ***matches)
{
    પાછા expand_set_opt_generic(
	    args,
	    get_printoptions_names,
	    અંક_Matches,
	    matches);
}
#endif

#જો defined(FEAT_STL_OPT)
/*
 * The 'statusline' or the 'tabline' or the 'rulerformat' option is changed.
 * "rulerformat" is TRUE if the 'rulerformat' option is changed.
 */
    સ્થિર અક્ષર *
parse_statustabline_rulerformat(optset_T *args, પૂર્ણાંક rulerformat)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u	*s;
    અક્ષર	*errmsg = ખાલી;
    પૂર્ણાંક		wid;

    જો (rulerformat)	// reset ru_wid first
	ru_wid = 0;
    s = *varp;
    જો (rulerformat && *s == '%')
    {
	// set ru_wid if 'ruf' starts with "%99("
	જો (*++s == '-')	// ignore a '-'
	    s++;
	wid = getdigits(&s);
	જો (wid && *s == '(' && (errmsg = check_stl_option(p_ruf)) == ખાલી)
	    ru_wid = wid;
	નહીંતર
	{
	    // Validate the flags in 'rulerformat' only if it doesn't point to
	    // a custom function ("%!" flag).
	    જો ((*varp)[1] != '!')
		errmsg = check_stl_option(p_ruf);
	}
    }
    // check 'statusline' or 'tabline' only if it doesn't start with "%!"
    નહીંતર જો (rulerformat || s[0] != '%' || s[1] != '!')
	errmsg = check_stl_option(s);
    જો (rulerformat && errmsg == ખાલી)
	comp_col();

    પાછા errmsg;
}
#endif

#જો defined(FEAT_RENDER_OPTIONS)
/*
 * The 'renderoptions' option is changed.
 */
    અક્ષર *
did_set_renderoptions(optset_T *args UNUSED)
{
    જો (!gui_mch_set_rendering_options(p_rop))
	પાછા e_invalid_argument;

    પાછા ખાલી;
}
#endif

#જો defined(FEAT_RIGHTLEFT)
/*
 * The 'rightleftcmd' option is changed.
 */
    અક્ષર *
did_set_rightleftcmd(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    // Currently only "search" is a supported value.
    જો (**varp != NUL && STRCMP(*varp, "search") != 0)
	પાછા e_invalid_argument;

    પાછા ખાલી;
}

    પૂર્ણાંક
expand_set_rightleftcmd(optexpand_T *args, પૂર્ણાંક *અંક_Matches, char_u ***matches)
{
    સ્થિર અક્ષર *(p_rlc_કિંમતો[]) = {"search", ખાલી};
    પાછા expand_set_opt_string(
	    args,
	    p_rlc_કિંમતો,
	    ARRAY_LENGTH(p_rlc_કિંમતો) - 1,
	    અંક_Matches,
	    matches);
}
#endif

#define PUM_BORDER_CLEAR()   \
    કરો {                     \
	pum_set_border(FALSE); \
	pum_set_shadow(FALSE); \
	pum_set_margin(FALSE); \
    } જ્યારે (0)

/*
 * Parse a border value from a pumopt "border:" token.
 * Returns OK on success, FAIL on error.
 */
    સ્થિર પૂર્ણાંક
parse_pumopt_border(char_u *val, પૂર્ણાંક len)
{
    // Use box-drawing characters only when 'encoding' is "utf-8" and
    // 'ambiwidth' is "single".
    પૂર્ણાંક	    can_use_box_chars = (enc_utf8 && *p_ambw == 's');
    char_u  *token;

    token = vim_strnsave(val, len);
    જો (token == ખાલી)
	પાછા FAIL;

    જો (can_use_box_chars && STRCMP(token, "single") == 0)
	pum_set_border_chars(0x2500, 0x2502, 0x2500, 0x2502, // ─ │ ─ │
		0x250c, 0x2510, 0x2518, 0x2514); // ┌ ┐ ┘ └
    નહીંતર જો (can_use_box_chars && STRCMP(token, "double") == 0)
	pum_set_border_chars(0x2550, 0x2551, 0x2550, 0x2551, // ═ ║ ═ ║
		0x2554, 0x2557, 0x255D, 0x255A); // ╔ ╗ ╝  ╚
    નહીંતર જો (can_use_box_chars && STRCMP(token, "round") == 0)
	pum_set_border_chars(0x2500, 0x2502, 0x2500, 0x2502, // ─ │ ─ │
		0x256d, 0x256e, 0x256f, 0x2570); // ╭ ╮ ╯ ╰
    નહીંતર જો (STRCMP(token, "ascii") == 0)
	pum_set_border_chars('-', '|', '-', '|', '+', '+', '+', '+');
    નહીંતર જો (STRNCMP(token, "custom:", 7) == 0)
    {
	char_u	*q = token + 7;
	પૂર્ણાંક	out[8];

	માટે (પૂર્ણાંક i = 0; i < 8; i++)
	{
	    જો (*q == NUL || *q == ',')
	    {
		vim_free(token);
		પાછા FAIL;
	    }
	    out[i] = mb_ptr2char(q);
	    mb_ptr2char_adv(&q);
	    જો (i < 7)
	    {
		જો (*q != ';')
		{
		    vim_free(token);
		    પાછા FAIL;
		}
		q++;
	    }
	}
	જો (*q != NUL && *q != ',')
	{
	    vim_free(token);
	    પાછા FAIL;
	}
	pum_set_border_chars(out[0], out[1], out[2], out[3], out[4], out[5],
		out[6], out[7]);
    }
    નહીંતર
    {
	vim_free(token);
	પાછા FAIL;
    }

    vim_free(token);
    pum_set_border(TRUE);
    પાછા OK;
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
    અક્ષર *
did_set_pumopt(optset_T *args)
{
    char_u  **varp = (char_u **)args->os_varp;
    char_u  *p;
    પૂર્ણાંક	    have_border = FALSE;
    પૂર્ણાંક	    have_margin = FALSE;

    // Reset to defaults.
    PUM_BORDER_CLEAR();
    p_ph = 0;
    p_pw = 15;
    p_pmw = 0;
    p_po = 100;

    જો (*varp == ખાલી || **varp == NUL)
	પાછા ખાલી;

    માટે (p = *varp; p != ખાલી && *p != NUL; )
    {
	char_u *comma = vim_strchr(p, ',');
	પૂર્ણાંક len;

	જો (comma != ખાલી)
	    len = (પૂર્ણાંક)(comma - p);
	નહીંતર
	    len = (પૂર્ણાંક)STRLEN(p);

	જો (STRNCMP(p, "border:", 7) == 0)
	{
	    જો (have_border)
		જાઓ error;
	    have_border = TRUE;
	    જો (parse_pumopt_border(p + 7, len - 7) == FAIL)
		જાઓ error;
	}
	નહીંતર જો (STRNCMP(p, "height:", 7) == 0)
	{
	    long n = atol((અક્ષર *)p + 7);
	    જો (n < 0)
		જાઓ error;
	    p_ph = n;
	}
	નહીંતર જો (STRNCMP(p, "width:", 6) == 0)
	{
	    long n = atol((અક્ષર *)p + 6);
	    જો (n < 0)
		જાઓ error;
	    p_pw = n;
	}
	નહીંતર જો (STRNCMP(p, "maxwidth:", 9) == 0)
	{
	    long n = atol((અક્ષર *)p + 9);
	    જો (n < 0)
		જાઓ error;
	    p_pmw = n;
	}
	નહીંતર જો (STRNCMP(p, "opacity:", 8) == 0)
	{
	    long n = atol((અક્ષર *)p + 8);
	    જો (n < 0 || n > 100)
		જાઓ error;
	    p_po = n;
	}
	નહીંતર જો (len == 6 && STRNCMP(p, "shadow", 6) == 0)
	    pum_set_shadow(TRUE);
	નહીંતર જો (len == 6 && STRNCMP(p, "margin", 6) == 0)
	{
	    have_margin = TRUE;
	    pum_set_margin(TRUE);
	}
	નહીંતર
	    જાઓ error;

	જો (comma != ખાલી)
	    p = comma + 1;
	નહીંતર
	    તોડો;
    }

    જો (have_margin && !have_border)
	જાઓ error;

    // Invalidate cached background for opacity changes.
    pum_opacity_changed();

    પાછા ખાલી;

error:
    PUM_BORDER_CLEAR();
    p_ph = 0;
    p_pw = 15;
    p_pmw = 0;
    p_po = 100;
    પાછા e_invalid_argument;
}

    સ્થિર char_u *
get_pum_border_style(expand_T *xp UNUSED, પૂર્ણાંક idx)
{
    સ્થિર અક્ષર *styles[] = {"ascii", "custom:", "single", "double", "round"};
    પાછા idx < ((enc_utf8 && *p_ambw == 's') ? (પૂર્ણાંક)ARRAY_LENGTH(styles) : 2)
	    ? (char_u *)styles[idx] : ખાલી;
}

    પૂર્ણાંક
expand_set_pumopt(optexpand_T *args, પૂર્ણાંક *અંક_Matches, char_u ***matches)
{
    expand_T *xp = args->oe_xp;

    જો (xp->xp_pattern > args->oe_set_arg && *(xp->xp_pattern-1) == ':')
    {
	જો (completing_કિંમત_for_subopt(args, "border"))
	{
	    પાછા expand_set_opt_generic(
		    args, get_pum_border_style, અંક_Matches, matches);
	}
	પાછા FAIL;
    }

    સ્થિર અક્ષર *(p_pumopt_કિંમતો[]) = {"border:", "height:", "width:",
	"maxwidth:", "opacity:", "shadow", "margin", ખાલી};
    પાછા expand_set_opt_string(
	    args,
	    p_pumopt_કિંમતો,
	    ARRAY_LENGTH(p_pumopt_કિંમતો) - 1,
	    અંક_Matches,
	    matches);
}

/*
 * The 'pumborder' option is changed.
 * Rules:
 *   - One of { single, double, round, ascii, custom:XXXXXXXX } may appear.
 *   - "margin" may appear, but only together with exactly one border style.
 *   - "shadow" is independent and can be combined freely.
 */
    અક્ષર *
did_set_pumborder(optset_T *args)
{
    char_u  **varp = (char_u **)args->os_varp;
    char_u  *p;
    પૂર્ણાંક	    len;
    પૂર્ણાંક	    have_border = FALSE;
    પૂર્ણાંક	    have_margin = FALSE;

    PUM_BORDER_CLEAR();

    જો (*varp == ખાલી || **varp == NUL)
	પાછા ખાલી;

    માટે (p = *varp; p != ખાલી && *p != NUL; )
    {
	char_u *comma = vim_strchr(p, ',');
	જો (comma != ખાલી)
	    len = (પૂર્ણાંક)(comma - p);
	નહીંતર
	    len = (પૂર્ણાંક)STRLEN(p);

	જો (STRNCMP(p, "shadow", len) == 0 && len == 6)
	    pum_set_shadow(TRUE);
	નહીંતર જો (STRNCMP(p, "margin", len) == 0 && len == 6)
	{
	    have_margin = TRUE;
	    pum_set_margin(TRUE);
	}
	નહીંતર
	{
	    જો (have_border)
		જાઓ error;
	    have_border = TRUE;
	    જો (parse_pumopt_border(p, len) == FAIL)
		જાઓ error;
	}

	જો (comma != ખાલી)
	    p = comma + 1;
	નહીંતર
	    તોડો;
    }

    જો (have_margin && !have_border)
	જાઓ error;

    પાછા ખાલી;

error:
    PUM_BORDER_CLEAR();
    pum_set_border_chars(0, 0, 0, 0, 0, 0, 0, 0);
    પાછા e_invalid_argument;
}

    સ્થિર char_u *
get_pumborder_token(expand_T *xp, પૂર્ણાંક idx)
{
    પાછા idx == 0 ? (char_u *)"margin"
	 : idx == 1 ? (char_u *)"shadow"
	 : get_pum_border_style(xp, idx - 2);
}

    પૂર્ણાંક
expand_set_pumborder(optexpand_T *args, પૂર્ણાંક *અંક_Matches, char_u ***matches)
{
    પાછા expand_set_opt_generic(
	    args, get_pumborder_token, અંક_Matches, matches);
}

#જો defined(FEAT_STL_OPT)

/*
 * The 'rulerformat' option is changed.
 */
    અક્ષર *
did_set_rulerformat(optset_T *args)
{
    પાછા parse_statustabline_rulerformat(args, TRUE);
}
#endif

#જો defined(FEAT_TABPANEL)
/*
 * Process the new 'tabpanelopt' option value.
 */
    અક્ષર *
did_set_tabpanelopt(optset_T *args UNUSED)
{
    જો (tabpanelopt_changed() == FAIL)
	પાછા e_invalid_argument;

    પાછા ખાલી;
}

    પૂર્ણાંક
expand_set_tabpanelopt(optexpand_T *args, પૂર્ણાંક *અંક_Matches, char_u ***matches)
{
    expand_T *xp = args->oe_xp;

    જો (xp->xp_pattern > args->oe_set_arg && *(xp->xp_pattern-1) == ':')
    {
	જો (completing_કિંમત_for_subopt(args, "align"))
	{
	    પાછા expand_set_opt_string(
		    args,
		    p_tplo_align_કિંમતો,
		    ARRAY_LENGTH(p_tplo_align_કિંમતો) - 1,
		    અંક_Matches,
		    matches);
	}
	પાછા FAIL;
    }

    પાછા expand_set_opt_string(
	    args,
	    p_tplo_કિંમતો,
	    ARRAY_LENGTH(p_tplo_કિંમતો) - 1,
	    અંક_Matches,
	    matches);
}
#endif

/*
 * The 'scrollopt' option is changed.
 */
    અક્ષર *
did_set_scrollopt(optset_T *args UNUSED)
{
    પાછા did_set_opt_strings(p_sbo, p_scbopt_કિંમતો, TRUE);
}

    પૂર્ણાંક
expand_set_scrollopt(optexpand_T *args, પૂર્ણાંક *અંક_Matches, char_u ***matches)
{
    પાછા expand_set_opt_string(
	    args,
	    p_scbopt_કિંમતો,
	    ARRAY_LENGTH(p_scbopt_કિંમતો) - 1,
	    અંક_Matches,
	    matches);
}

/*
 * The 'wlseat' option is changed
 */
    અક્ષર *
did_set_wlseat(optset_T *args UNUSED)
{
#ifdef FEAT_WAYLAND_CLIPBOARD
    // If there isn't any seat named 'wlseat', then let the Wayland clipboard be
    // unavailable. Ignore errors returned.
    clip_reset_wayland();
#endif

    પાછા ખાલી;
}

/*
 * The 'selection' option is changed.
 */
    અક્ષર *
did_set_selection(optset_T *args UNUSED)
{
    જો (*p_sel == NUL || check_opt_strings(p_sel, p_sel_કિંમતો, FALSE) != OK)
	પાછા e_invalid_argument;

    પાછા ખાલી;
}

    પૂર્ણાંક
expand_set_selection(optexpand_T *args, પૂર્ણાંક *અંક_Matches, char_u ***matches)
{
    પાછા expand_set_opt_string(
	    args,
	    p_sel_કિંમતો,
	    ARRAY_LENGTH(p_sel_કિંમતો) - 1,
	    અંક_Matches,
	    matches);
}

/*
 * The 'selectmode' option is changed.
 */
    અક્ષર *
did_set_selectmode(optset_T *args UNUSED)
{
    પાછા did_set_opt_strings(p_slm, p_slm_કિંમતો, TRUE);
}

    પૂર્ણાંક
expand_set_selectmode(optexpand_T *args, પૂર્ણાંક *અંક_Matches, char_u ***matches)
{
    પાછા expand_set_opt_string(
	    args,
	    p_slm_કિંમતો,
	    ARRAY_LENGTH(p_slm_કિંમતો) - 1,
	    અંક_Matches,
	    matches);
}

#જો defined(FEAT_SESSION)
/*
 * The 'sessionoptions' option is changed.
 */
    અક્ષર *
did_set_sessionoptions(optset_T *args)
{
    જો (opt_strings_flags(p_ssop, p_ssop_કિંમતો, &ssop_flags, TRUE) != OK)
	પાછા e_invalid_argument;
    જો ((ssop_flags & SSOP_CURDIR) && (ssop_flags & SSOP_SESDIR))
    {
	// Don't allow both "sesdir" and "curdir".
	(શૂન્ય)opt_strings_flags(args->os_oldval.string, p_ssop_કિંમતો,
							&ssop_flags, TRUE);
	પાછા e_invalid_argument;
    }

    પાછા ખાલી;
}

    પૂર્ણાંક
expand_set_sessionoptions(
    optexpand_T		*args,
    પૂર્ણાંક			*અંક_Matches,
    char_u		***matches)
{
    પાછા expand_set_opt_string(
	    args,
	    p_ssop_કિંમતો,
	    ARRAY_LENGTH(p_ssop_કિંમતો) - 1,
	    અંક_Matches,
	    matches);
}
#endif

/*
 * Validate 'shellpipe'/'shellredir' option.
 */
    અક્ષર *
did_set_shellpipe_redir(optset_T *args)
{
    char_u	*p;
    bool	seen = false;

    માટે (p = args->os_newval.string; *p != NUL; ++p)
    {
	જો (*p != '%')
	    ચાલુ;

	જો (p[1] == NUL)
	    પાછા e_invalid_format_string_single_percent_s;

	જો (p[1] == '%')
	{
	    ++p;    // skip second %
	    ચાલુ;
	}

	જો (p[1] == 's')
	{
	    જો (seen)
		પાછા e_invalid_format_string_single_percent_s;

	    seen = true;
	    ++p;    // consume 's'
	    ચાલુ;
	}
	પાછા e_invalid_format_string_single_percent_s;
    }
    પાછા ખાલી;
}

/*
 * The 'shortmess' option is changed.
 */
    અક્ષર *
did_set_shortmess(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    પાછા did_set_option_listflag(*varp, (char_u *)SHM_ALL, args->os_errbuf,
		    args->os_errbuflen);
}

    પૂર્ણાંક
expand_set_shortmess(optexpand_T *args, પૂર્ણાંક *અંક_Matches, char_u ***matches)
{
    પાછા expand_set_opt_listflag(args, (char_u*)SHM_ALL, અંક_Matches, matches);
}

#જો defined(FEAT_LINEBREAK)
/*
 * The 'showbreak' option is changed.
 */
    અક્ષર *
did_set_showbreak(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u	*s;

    માટે (s = *varp; *s; )
    {
	જો (ptr2cells(s) != 1)
	    પાછા e_showbreak_contains_unprintable_or_wide_character;
	MB_PTR_ADV(s);
    }

    પાછા ખાલી;
}
#endif

/*
 * The 'showcmdloc' option is changed.
 */
    અક્ષર *
did_set_showcmdloc(optset_T *args UNUSED)
{
    અક્ષર	*errmsg = did_set_opt_strings(p_sloc, p_sloc_કિંમતો, FALSE);

    જો (errmsg == ખાલી)
	comp_col();

    પાછા errmsg;
}

    પૂર્ણાંક
expand_set_showcmdloc(optexpand_T *args, પૂર્ણાંક *અંક_Matches, char_u ***matches)
{
    પાછા expand_set_opt_string(
	    args,
	    p_sloc_કિંમતો,
	    ARRAY_LENGTH(p_sloc_કિંમતો) - 1,
	    અંક_Matches,
	    matches);
}

#જો defined(FEAT_SIGNS)
/*
 * The 'signcolumn' option is changed.
 */
    અક્ષર *
did_set_signcolumn(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

# જો defined(FEAT_LINEBREAK)
    જો (check_opt_strings(*varp, p_scl_કિંમતો, FALSE) != OK)
	પાછા e_invalid_argument;
    // When changing the 'signcolumn' to or from 'number', recompute the
    // width of the number column if 'number' or 'relativenumber' is set.
    જો (((*args->os_oldval.string == 'n' && args->os_oldval.string[1] == 'u')
		|| (*curwin->w_p_scl == 'n' && *(curwin->w_p_scl + 1) =='u'))
	    && (curwin->w_p_nu || curwin->w_p_rnu))
	curwin->w_nrwidth_લીટી_ગણતરી = 0;
# endif

# જો defined(FEAT_GUI)
    // In the GUI, when sign icons are used, a full screen clear is needed
    // to properly redraw the sign icons.
    જો (gui.in_use && curbuf->b_signlist != ખાલી)
	redraw_all_later(UPD_CLEAR);
# endif

    પાછા ખાલી;
}

    પૂર્ણાંક
expand_set_signcolumn(optexpand_T *args, પૂર્ણાંક *અંક_Matches, char_u ***matches)
{
    પાછા expand_set_opt_string(
	    args,
	    p_scl_કિંમતો,
	    ARRAY_LENGTH(p_scl_કિંમતો) - 1,
	    અંક_Matches,
	    matches);
}
#endif

#જો defined(FEAT_SPELL)
/*
 * The 'spellcapcheck' option is changed.
 */
    અક્ષર *
did_set_spellcapcheck(optset_T *args UNUSED)
{
    // compile the regexp program.
    પાછા compile_cap_prog(curwin->w_s);
}

/*
 * The 'spellfile' option is changed.
 */
    અક્ષર *
did_set_spellfile(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    જો (!valid_spellfile(*varp))
	પાછા e_invalid_argument;

    // If there is a window for this buffer in which 'spell' is set load the
    // wordlists.
    પાછા did_set_spell_option();
}

/*
 * The 'spell' option is changed.
 */
    અક્ષર *
did_set_spelllang(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    જો (!valid_spelllang(*varp))
	પાછા e_invalid_argument;

    // If there is a window for this buffer in which 'spell' is set load the
    // wordlists.
    પાછા did_set_spell_option();
}

/*
 * The 'spelloptions' option is changed.
 */
    અક્ષર *
did_set_spelloptions(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    જો (**varp != NUL && STRCMP(*varp, "camel") != 0)
	પાછા e_invalid_argument;

    પાછા ખાલી;
}

    પૂર્ણાંક
expand_set_spelloptions(optexpand_T *args, પૂર્ણાંક *અંક_Matches, char_u ***matches)
{
    સ્થિર અક્ષર *(p_spo_કિંમતો[]) = {"camel", ખાલી};
    પાછા expand_set_opt_string(
	    args,
	    p_spo_કિંમતો,
	    ARRAY_LENGTH(p_spo_કિંમતો) - 1,
	    અંક_Matches,
	    matches);
}

/*
 * The 'spellsuggest' option is changed.
 */
    અક્ષર *
did_set_spellsuggest(optset_T *args UNUSED)
{
    જો (spell_check_sps() != OK)
	પાછા e_invalid_argument;

    પાછા ખાલી;
}

    પૂર્ણાંક
expand_set_spellsuggest(optexpand_T *args, પૂર્ણાંક *અંક_Matches, char_u ***matches)
{
    પાછા expand_set_opt_string(
	    args,
	    p_sps_કિંમતો,
	    ARRAY_LENGTH(p_sps_કિંમતો) - 1,
	    અંક_Matches,
	    matches);
}
#endif

/*
 * The 'splitkeep' option is changed.
 */
    અક્ષર *
did_set_splitkeep(optset_T *args UNUSED)
{
    win_T	*wp;
    tabpage_T	*tp;
    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	wp->w_prev_height = wp->w_height;
	wp->w_prev_winrow = wp->w_winrow;
    }
    પાછા did_set_opt_strings(p_spk, p_spk_કિંમતો, FALSE);
}

    પૂર્ણાંક
expand_set_splitkeep(optexpand_T *args, પૂર્ણાંક *અંક_Matches, char_u ***matches)
{
    પાછા expand_set_opt_string(
	    args,
	    p_spk_કિંમતો,
	    ARRAY_LENGTH(p_spk_કિંમતો) - 1,
	    અંક_Matches,
	    matches);
}

#જો defined(FEAT_STL_OPT)
/*
 * The 'statusline' option is changed.
 */
    અક્ષર *
did_set_statusline(optset_T *args)
{
    char_u  **varp = (char_u **)args->os_varp;
    અક્ષર    *ret = parse_statustabline_rulerformat(args, FALSE);

    જો (ret != ખાલી)
	પાછા ret;
    update_stl_rendered_height(varp == &curwin->w_p_stl ? curwin : ખાલી);
    frame_change_statusline_height();

    પાછા ખાલી;
}

/*
 * Rewrite the 'statuslineopt' string: replace the last "maxheight:N" with
 * actual_stlh and remove earlier duplicates.  Keyword order is preserved.
 */
    સ્થિર શૂન્ય
update_stlo_maxheight(char_u **varp, પૂર્ણાંક actual_stlh)
{
    અચલ char_u    *src = *varp;
    અચલ char_u    *last_mh = ખાલી;
    અચલ char_u    *p;

    // Find the last "maxheight:" token.
    p = src;
    જ્યારે (*p != NUL)
    {
	જો (STRNCMP(p, "maxheight:", 10) == 0)
	    last_mh = p;
	જ્યારે (*p != ',' && *p != NUL)
	    ++p;
	જો (*p == ',')
	    ++p;
    }

    જો (last_mh == ખાલી)
	પાછા;

    size_t	bufsize = STRLEN(src) + NUMBUFLEN + 1;
    char_u	*buf = alloc(bufsize);
    જો (buf == ખાલી)
	પાછા;

    પૂર્ણાંક		len = 0;
    bool	need_comma = false;

    p = src;
    જ્યારે (*p != NUL)
    {
	અચલ	char_u	*tok = p;
	પૂર્ણાંક	tok_len;

	જ્યારે (*p != ',' && *p != NUL)
	    ++p;
	tok_len = (પૂર્ણાંક)(p - tok);
	જો (*p == ',')
	    ++p;

	જો (STRNCMP(tok, "maxheight:", 10) == 0)
	{
	    જો (tok == last_mh)
	    {
		// Replace the last occurrence with the actual value.
		જો (need_comma)
		    buf[len++] = ',';
		len += vim_snprintf((અક્ષર *)buf + len,
			bufsize - len, "maxheight:%d", actual_stlh);
		need_comma = true;
	    }
	}
	નહીંતર
	{
	    જો (need_comma)
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
    અક્ષર *
did_set_statuslineopt(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    win_T       *wp = varp == &curwin->w_p_stlo ? curwin : ખાલી;

    જો (statuslineopt_changed(*varp, wp) == FAIL)
	પાછા e_invalid_argument;

    // Sync stl_rendered_height with the current statusline.
    update_stl_rendered_height(wp);

    // Update the maxheight value to the actual value set.
    // Note: Must be changed if p_stlo_values are changed.
    જો (*varp != empty_option)
    {
	પૂર્ણાંક actual_stlh;

	જો (wp != ખાલી)
	{
	    frame_change_statusline_height();
	    actual_stlh = MIN(wp->w_p_stlo_mh,
		    wp->w_height + wp->w_status_height - p_wmh);
	}
	નહીંતર
	    actual_stlh = frame_change_statusline_height();

	// Only re-serialize when the window actually has a status line shown.
	જો (actual_stlh > 0)
	{
	    update_stlo_maxheight(varp, actual_stlh);
	    // Update the parsed maxheight member directly.
	    જો (wp != ખાલી)
		wp->w_p_stlo_mh = actual_stlh;
	    નહીંતર
		set_stlo_mh(actual_stlh);
	}
    }
    નહીંતર
	frame_change_statusline_height();

    જો ((args->os_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	// :set clears the local values.
	clear_string_option(&curwin->w_p_stlo);
	curwin->w_p_stlo_fh = -1;
	curwin->w_p_stlo_mh = -1;
    }

    પાછા ખાલી;
}

    પૂર્ણાંક
expand_set_statuslineopt(optexpand_T *args, પૂર્ણાંક *અંક_Matches, char_u ***matches)
{
    પાછા expand_set_opt_string(
	    args,
	    p_stlo_કિંમતો,
	    ARRAY_LENGTH(p_stlo_કિંમતો) - 1,
	    અંક_Matches,
	    matches);
}
#endif

/*
 * The 'swapsync' option is changed.
 */
    અક્ષર *
did_set_swapsync(optset_T *args UNUSED)
{
    પાછા did_set_opt_strings(p_sws, p_sws_કિંમતો, FALSE);
}

    પૂર્ણાંક
expand_set_swapsync(optexpand_T *args, પૂર્ણાંક *અંક_Matches, char_u ***matches)
{
    પાછા expand_set_opt_string(
	    args,
	    p_sws_કિંમતો,
	    ARRAY_LENGTH(p_sws_કિંમતો) - 1,
	    અંક_Matches,
	    matches);
}

/*
 * The 'switchbuf' option is changed.
 */
    અક્ષર *
did_set_switchbuf(optset_T *args UNUSED)
{
    પાછા did_set_opt_flags(p_swb, p_swb_કિંમતો, &swb_flags, TRUE);
}

    પૂર્ણાંક
expand_set_switchbuf(optexpand_T *args, પૂર્ણાંક *અંક_Matches, char_u ***matches)
{
    પાછા expand_set_opt_string(
	    args,
	    p_swb_કિંમતો,
	    ARRAY_LENGTH(p_swb_કિંમતો) - 1,
	    અંક_Matches,
	    matches);
}

/*
 * The 'tabclose' option is changed.
 */
    અક્ષર *
did_set_tabclose(optset_T *args UNUSED)
{
    પાછા did_set_opt_flags(p_tcl, p_tcl_કિંમતો, &tcl_flags, TRUE);
}

    પૂર્ણાંક
expand_set_tabclose(optexpand_T *args, પૂર્ણાંક *અંક_Matches, char_u ***matches)
{
    પાછા expand_set_opt_string(
	    args,
	    p_tcl_કિંમતો,
	    ARRAY_LENGTH(p_tcl_કિંમતો) - 1,
	    અંક_Matches,
	    matches);
}

#જો defined(FEAT_STL_OPT)
/*
 * The 'tabline' option is changed.
 */
    અક્ષર *
did_set_tabline(optset_T *args)
{
    પાછા parse_statustabline_rulerformat(args, FALSE);
}
#endif

/*
 * The 'tagcase' option is changed.
 */
    અક્ષર *
did_set_tagcase(optset_T *args)
{
    unsigned પૂર્ણાંક	*flags;
    char_u		*p;

    જો (args->os_flags & OPT_LOCAL)
    {
	p = curbuf->b_p_tc;
	flags = &curbuf->b_tc_flags;
    }
    નહીંતર
    {
	p = p_tc;
	flags = &tc_flags;
    }

    જો ((args->os_flags & OPT_LOCAL) && *p == NUL)
	// make the local value empty: use the global value
	*flags = 0;
    નહીંતર જો (*p == NUL
	    || opt_strings_flags(p, p_tc_કિંમતો, flags, FALSE) != OK)
	પાછા e_invalid_argument;

    પાછા ખાલી;
}

    પૂર્ણાંક
expand_set_tagcase(optexpand_T *args, પૂર્ણાંક *અંક_Matches, char_u ***matches)
{
    પાછા expand_set_opt_string(
	    args,
	    p_tc_કિંમતો,
	    ARRAY_LENGTH(p_tc_કિંમતો) - 1,
	    અંક_Matches,
	    matches);
}

/*
 * The 'term' option is changed.
 */
    અક્ષર *
did_set_term(optset_T *args UNUSED)
{
    જો (T_NAME[0] == NUL)
	પાછા e_cannot_set_term_to_empty_string;
#ifdef FEAT_GUI
    જો (gui.in_use)
	પાછા e_cannot_change_term_in_GUI;
    જો (term_is_gui(T_NAME))
	પાછા e_use_gui_to_start_GUI;
#endif
    જો (set_termname(T_NAME) == FAIL)
	પાછા e_not_found_in_termcap;

    // Screen colors may have changed.
    redraw_later_clear();

    પાછા ખાલી;
}

/*
 * Some terminal option (t_xxx) is changed
 */
    અક્ષર *
did_set_term_option(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    જો (!full_screen)
	પાછા ખાલી;

    // ":set t_Co=0" and ":set t_Co=1" do ":set t_Co="
    જો (varp == &T_CCO)
    {
	પૂર્ણાંક colors = atoi((અક્ષર *)T_CCO);

	// Only reinitialize colors if t_Co value has really changed to
	// avoid expensive reload of colorscheme if t_Co is set to the
	// same value multiple times.
	જો (colors != t_colors)
	{
	    t_colors = colors;
	    જો (t_colors <= 1)
	    {
		vim_free(T_CCO);
		T_CCO = empty_option;
	    }
#જો defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
	    જો (is_term_win32())
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
    જો (varp == &T_ME)
    {
	out_str(T_ME);
	redraw_later(UPD_CLEAR);
#જો defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
	// Since t_me has been set, this probably means that the user
	// wants to use this as default colors.  Need to reset default
	// background/foreground colors.
# ifdef VIMDLL
	જો (!gui.in_use && !gui.starting)
# endif
	    mch_set_normal_colors();
#endif
    }
    જો (varp == &T_BE && termcap_active)
    {
	MAY_WANT_TO_LOG_THIS;

	જો (*T_BE == NUL)
	    // When clearing t_BE we assume the user no longer wants
	    // bracketed paste, thus disable it by writing t_BD.
	    out_str(T_BD);
	નહીંતર
	    out_str(T_BE);
    }

    જો (varp == &T_BSU || varp == &T_ESU)
	term_set_sync_output(TERM_SYNC_OUTPUT_OFF);

    પાછા ખાલી;
}


#ifdef UNIX
/*
 * The 'termresize' option is changed.
 */
    અક્ષર *
did_set_termresize(optset_T *args UNUSED)
{
    // If empty or "inband", then attempt to enable in-band resize events.
    જો (*p_trz == NUL || STRCMP(p_trz, "inband") == 0)
	term_set_win_resize(true);
    નહીંતર જો (STRCMP(p_trz, "sigwinch") == 0)
	term_set_win_resize(false);
    નહીંતર
	પાછા e_invalid_argument;

    પાછા ખાલી;
}

    પૂર્ણાંક
expand_set_termresize(optexpand_T *args, પૂર્ણાંક *અંક_Matches, char_u ***matches)
{
    પાછા expand_set_opt_string(
	    args,
	    p_trz_કિંમતો,
	    ARRAY_LENGTH(p_trz_કિંમતો) - 1,
	    અંક_Matches,
	    matches);
}
#endif

#જો defined(FEAT_TERMINAL)
/*
 * The 'termwinkey' option is changed.
 */
    અક્ષર *
did_set_termwinkey(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    જો ((*varp)[0] != NUL && string_to_ચાવી(*varp, TRUE) == 0)
	પાછા e_invalid_argument;

    પાછા ખાલી;
}

/*
 * The 'termwinsize' option is changed.
 */
    અક્ષર *
did_set_termwinsize(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u	*p;

    જો ((*varp)[0] == NUL)
	પાછા ખાલી;

    p = skipdigits(*varp);
    જો (p == *varp || (*p != 'x' && *p != '*') || *skipdigits(p + 1) != NUL)
	પાછા e_invalid_argument;

    પાછા ખાલી;
}

# જો defined(MSWIN)
/*
 * The 'termwintype' option is changed.
 */
    અક્ષર *
did_set_termwintype(optset_T *args UNUSED)
{
    પાછા did_set_opt_strings(p_twt, p_twt_કિંમતો, FALSE);
}

    પૂર્ણાંક
expand_set_termwintype(optexpand_T *args, પૂર્ણાંક *અંક_Matches, char_u ***matches)
{
    પાછા expand_set_opt_string(
	    args,
	    p_twt_કિંમતો,
	    ARRAY_LENGTH(p_twt_કિંમતો) - 1,
	    અંક_Matches,
	    matches);
}
# endif
#endif

/*
 * The 'titlestring' option is changed.
 */
    અક્ષર *
did_set_titlestring(optset_T *args)
{
    પૂર્ણાંક flagval = 0;

#ifdef FEAT_STL_OPT
    flagval = STL_IN_TITLE;
#endif
    પાછા parse_titleiconstring(args, flagval);
}

#જો defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
/*
 * The 'toolbar' option is changed.
 */
    અક્ષર *
did_set_toolbar(optset_T *args UNUSED)
{
    જો (opt_strings_flags(p_toolbar, p_toolbar_કિંમતો, &toolbar_flags,
								TRUE) != OK)
	પાછા e_invalid_argument;

    out_flush();
    gui_mch_show_toolbar((toolbar_flags &
		(TOOLBAR_TEXT | TOOLBAR_ICONS)) != 0);
    પાછા ખાલી;
}

    પૂર્ણાંક
expand_set_toolbar(optexpand_T *args, પૂર્ણાંક *અંક_Matches, char_u ***matches)
{
    પાછા expand_set_opt_string(
	    args,
	    p_toolbar_કિંમતો,
	    ARRAY_LENGTH(p_toolbar_કિંમતો) - 1,
	    અંક_Matches,
	    matches);
}
#endif

#જો defined(FEAT_TOOLBAR) && defined(FEAT_GUI_GTK)
/*
 * The 'toolbariconsize' option is changed.  GTK+ 2 only.
 */
    અક્ષર *
did_set_toolbariconsize(optset_T *args UNUSED)
{
    જો (opt_strings_flags(p_tbis, p_tbis_કિંમતો, &tbis_flags, FALSE) != OK)
	પાછા e_invalid_argument;

    out_flush();
    gui_mch_show_toolbar((toolbar_flags &
		(TOOLBAR_TEXT | TOOLBAR_ICONS)) != 0);
    પાછા ખાલી;
}

    પૂર્ણાંક
expand_set_toolbariconsize(
    optexpand_T	*args,
    પૂર્ણાંક		*અંક_Matches,
    char_u	***matches)
{
    પાછા expand_set_opt_string(
	    args,
	    p_tbis_કિંમતો,
	    ARRAY_LENGTH(p_tbis_કિંમતો) - 1,
	    અંક_Matches,
	    matches);
}
#endif

#જો defined(UNIX) || defined(VMS)
/*
 * The 'ttymouse' option is changed.
 */
    અક્ષર *
did_set_ttymouse(optset_T *args UNUSED)
{
    અક્ષર *errmsg = ખાલી;

    // Switch the mouse off before changing the escape sequences used for
    // that.
    mch_setmouse(FALSE);
    જો (opt_strings_flags(p_ttym, p_ttym_કિંમતો, &ttym_flags, FALSE) != OK)
	errmsg = e_invalid_argument;
    નહીંતર
	check_mouse_termcode();
    જો (termcap_active)
	setmouse();		// may switch it on again

    પાછા errmsg;
}

    પૂર્ણાંક
expand_set_ttymouse(optexpand_T *args, પૂર્ણાંક *અંક_Matches, char_u ***matches)
{
    પાછા expand_set_opt_string(
	    args,
	    p_ttym_કિંમતો,
	    ARRAY_LENGTH(p_ttym_કિંમતો) - 1,
	    અંક_Matches,
	    matches);
}
#endif

#જો defined(FEAT_VARTABS)
/*
 * The 'varsofttabstop' option is changed.
 */
    અક્ષર *
did_set_varsofttabstop(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u *cp;

    જો (!((*varp)[0]) || ((*varp)[0] == '0' && !((*varp)[1])))
	VIM_CLEAR(curbuf->b_p_vsts_array);
    નહીંતર
    {
	માટે (cp = *varp; *cp; ++cp)
	{
	    જો (vim_isdigit(*cp))
		ચાલુ;
	    જો (*cp == ',' && cp > *varp && *(cp-1) != ',')
		ચાલુ;
	    પાછા e_invalid_argument;
	}

	પૂર્ણાંક *oldarray = curbuf->b_p_vsts_array;
	જો (tabstop_set(*varp, &(curbuf->b_p_vsts_array)) == OK)
	{
	    જો (oldarray)
		vim_free(oldarray);
	}
	નહીંતર
	    પાછા e_invalid_argument;
    }

    પાછા ખાલી;
}

/*
 * The 'vartabstop' option is changed.
 */
    અક્ષર *
did_set_vartabstop(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u *cp;

    જો (!((*varp)[0]) || ((*varp)[0] == '0' && !((*varp)[1])))
	VIM_CLEAR(curbuf->b_p_vts_array);
    નહીંતર
    {
	માટે (cp = *varp; *cp; ++cp)
	{
	    જો (vim_isdigit(*cp))
		ચાલુ;
	    જો (*cp == ',' && cp > *varp && *(cp-1) != ',')
		ચાલુ;
	    પાછા e_invalid_argument;
	}

	પૂર્ણાંક *oldarray = curbuf->b_p_vts_array;

	જો (tabstop_set(*varp, &(curbuf->b_p_vts_array)) == OK)
	{
	    vim_free(oldarray);
# ifdef FEAT_FOLDING
	    જો (foldmethodIsIndent(curwin))
		foldUpdateAll(curwin);
# endif
	}
	નહીંતર
	    પાછા e_invalid_argument;
    }

    પાછા ખાલી;
}
#endif

/*
 * The 'verbosefile' option is changed.
 */
    અક્ષર *
did_set_verbosefile(optset_T *args UNUSED)
{
    verbose_stop();
    જો (*p_vfile != NUL && verbose_open() == FAIL)
	પાછા e_invalid_argument;

    પાછા ખાલી;
}

#જો defined(FEAT_SESSION)
/*
 * The 'viewoptions' option is changed.
 */
    અક્ષર *
did_set_viewoptions(optset_T *args UNUSED)
{
    પાછા did_set_opt_flags(p_vop, p_ssop_કિંમતો, &vop_flags, TRUE);
}
#endif

#જો defined(FEAT_VIMINFO)
/*
 * The 'viminfo' option is changed.
 */
    અક્ષર *
did_set_viminfo(optset_T *args)
{
    char_u	*s;
    અક્ષર	*errmsg = ખાલી;

    માટે (s = p_viminfo; *s;)
    {
	// Check it's a valid character
	જો (vim_strchr((char_u *)"!\"%'/:<@cfhnrs", *s) == ખાલી)
	{
	    errmsg = illegal_char(args->os_errbuf, args->os_errbuflen, *s);
	    તોડો;
	}
	જો (*s == 'n')	// name is always last one
	    તોડો;
	નહીંતર જો (*s == 'r') // skip until next ','
	{
	    જ્યારે (*++s && *s != ',')
		;
	}
	નહીંતર જો (*s == '%')
	{
	    // optional number
	    જ્યારે (vim_isdigit(*++s))
		;
	}
	નહીંતર જો (*s == '!' || *s == 'h' || *s == 'c')
	    ++s;		// no extra chars
	નહીંતર		// must have a number
	{
	    જ્યારે (vim_isdigit(*++s))
		;

	    જો (!VIM_ISDIGIT(*(s - 1)))
	    {
		જો (args->os_errbuf != ખાલી)
		{
		    vim_snprintf(args->os_errbuf, args->os_errbuflen,
			    _(e_missing_સંખ્યા_after_angle_str_angle),
			    transchar_byte(*(s - 1)));
		    errmsg = args->os_errbuf;
		}
		નહીંતર
		    errmsg = "";
		તોડો;
	    }
	}
	જો (*s == ',')
	    ++s;
	નહીંતર જો (*s)
	{
	    જો (args->os_errbuf != ખાલી)
		errmsg = e_missing_comma;
	    નહીંતર
		errmsg = "";
	    તોડો;
	}
    }
    જો (*p_viminfo && errmsg == ખાલી && get_viminfo_parameter('\'') < 0)
	errmsg = e_must_specify_a_કિંમત;

    પાછા errmsg;
}
#endif

/*
 * The 'virtualedit' option is changed.
 */
    અક્ષર *
did_set_virtualedit(optset_T *args)
{
    char_u		*ve = p_ve;
    unsigned પૂર્ણાંક	*flags = &ve_flags;

    જો (args->os_flags & OPT_LOCAL)
    {
	ve = curwin->w_p_ve;
	flags = &curwin->w_ve_flags;
    }

    જો ((args->os_flags & OPT_LOCAL) && *ve == NUL)
	// make the local value empty: use the global value
	*flags = 0;
    નહીંતર
    {
	જો (opt_strings_flags(ve, p_ve_કિંમતો, flags, TRUE) != OK)
	    પાછા e_invalid_argument;
	નહીંતર જો (STRCMP(ve, args->os_oldval.string) != 0)
	{
	    // Recompute cursor position in case the new 've' setting
	    // changes something.
	    validate_virtcol();
	    coladvance(curwin->w_virtcol);
	}
    }

    પાછા ખાલી;
}

    પૂર્ણાંક
expand_set_virtualedit(optexpand_T *args, પૂર્ણાંક *અંક_Matches, char_u ***matches)
{
    પાછા expand_set_opt_string(
	    args,
	    p_ve_કિંમતો,
	    ARRAY_LENGTH(p_ve_કિંમતો) - 1,
	    અંક_Matches,
	    matches);
}

/*
 * The 'whichwrap' option is changed.
 */
    અક્ષર *
did_set_whichwrap(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    // Add ',' to the list flags because 'whichwrap' is a flag
    // list that is comma-separated.
    પાછા did_set_option_listflag(*varp, (char_u *)(WW_ALL ","),
		    args->os_errbuf, args->os_errbuflen);
}

    પૂર્ણાંક
expand_set_whichwrap(optexpand_T *args, પૂર્ણાંક *અંક_Matches, char_u ***matches)
{
    પાછા expand_set_opt_listflag(args, (char_u*)WW_ALL, અંક_Matches, matches);
}

/*
 * The 'wildmode' option is changed.
 */
    અક્ષર *
did_set_wildmode(optset_T *args UNUSED)
{
    જો (check_opt_wim() == FAIL)
	પાછા e_invalid_argument;
    પાછા ખાલી;
}

    પૂર્ણાંક
expand_set_wildmode(optexpand_T *args, પૂર્ણાંક *અંક_Matches, char_u ***matches)
{
    પાછા expand_set_opt_string(
	    args,
	    p_wim_કિંમતો,
	    ARRAY_LENGTH(p_wim_કિંમતો) - 1,
	    અંક_Matches,
	    matches);
}

/*
 * The 'wildoptions' option is changed.
 */
    અક્ષર *
did_set_wildoptions(optset_T *args UNUSED)
{
    પાછા did_set_opt_strings(p_wop, p_wop_કિંમતો, TRUE);
}

    પૂર્ણાંક
expand_set_wildoptions(optexpand_T *args, પૂર્ણાંક *અંક_Matches, char_u ***matches)
{
    પાછા expand_set_opt_string(
	    args,
	    p_wop_કિંમતો,
	    ARRAY_LENGTH(p_wop_કિંમતો) - 1,
	    અંક_Matches,
	    matches);
}

#જો defined(FEAT_WAK)
/*
 * The 'winaltkeys' option is changed.
 */
    અક્ષર *
did_set_winaltkeys(optset_T *args UNUSED)
{
    અક્ષર *errmsg = ખાલી;

    જો (*p_wak == NUL || check_opt_strings(p_wak, p_wak_કિંમતો, FALSE) != OK)
	errmsg = e_invalid_argument;
# ifdef FEAT_MENU
#  જો defined(FEAT_GUI_MOTIF)
    નહીંતર જો (gui.in_use)
	gui_motif_set_mnemonics(p_wak[0] == 'y' || p_wak[0] == 'm');
#  elif defined(FEAT_GUI_GTK)
    નહીંતર જો (gui.in_use)
	gui_gtk_set_mnemonics(p_wak[0] == 'y' || p_wak[0] == 'm');
#  endif
# endif
    પાછા errmsg;
}

    પૂર્ણાંક
expand_set_winaltkeys(optexpand_T *args, પૂર્ણાંક *અંક_Matches, char_u ***matches)
{
    પાછા expand_set_opt_string(
	    args,
	    p_wak_કિંમતો,
	    ARRAY_LENGTH(p_wak_કિંમતો) - 1,
	    અંક_Matches,
	    matches);
}
#endif

/*
 * The 'wincolor' option is changed.
 */
    અક્ષર *
did_set_wincolor(optset_T *args UNUSED)
{
    update_wincolor(curwin, args->os_newval.string);
    પાછા ખાલી;
}

/*
 * The 'winhighlight' option is changed
 */
    અક્ષર *
did_set_winhighlight(optset_T *args)
{
    પાછા update_winhighlight(curwin, args->os_newval.string);
}

/*
 * Expand 'winhighlight' option.
 */
    પૂર્ણાંક
expand_set_winhighlight(optexpand_T *args, પૂર્ણાંક *અંક_Matches, char_u ***matches)
{
    expand_T	    *xp = args->oe_xp;

    જો ((xp->xp_pattern > args->oe_set_arg && *(xp->xp_pattern-1) == ':')
	    || xp->xp_pattern == args->oe_set_arg || *(xp->xp_pattern-1) == ',')
    {
	// After a ':' or after a ',', or at the start, so expand highlight
	// group name.

	// If starts with !, then expand 'highlight' occasions.
	જો (*xp->xp_pattern == '!')
	    પાછા expand_hl_occasions(args, અંક_Matches, matches, '!');
	નહીંતર
	    પાછા expand_set_opt_generic(
		    args,
		    get_highlight_name,
		    અંક_Matches,
		    matches);
    }

    VIM_CLEAR(*matches);
    પાછા FAIL;
}

    પૂર્ણાંક
expand_set_wincolor(optexpand_T *args, પૂર્ણાંક *અંક_Matches, char_u ***matches)
{
    પાછા expand_set_opt_generic(
	    args,
	    get_highlight_name,
	    અંક_Matches,
	    matches);
}

#ifdef FEAT_SYN_HL
/*
 * When the 'syntax' option is set, load the syntax of that name.
 */
    સ્થિર શૂન્ય
do_syntax_autocmd(પૂર્ણાંક કિંમત_changed)
{
    સ્થિર પૂર્ણાંક syn_recursive = 0;

    ++syn_recursive;
    // Only pass TRUE for "force" when the value changed or not used
    // recursively, to avoid endless recurrence.
    apply_autocmds(EVENT_SYNTAX, curbuf->b_p_syn, curbuf->b_fname,
	    કિંમત_changed || syn_recursive == 1, curbuf);
    curbuf->b_flags |= BF_SYN_SET;
    --syn_recursive;
}
#endif

/*
 * When the 'filetype' option is set, trigger the FileType autocommand.
 */
    સ્થિર શૂન્ય
do_filetype_autocmd(char_u **varp, પૂર્ણાંક opt_flags, પૂર્ણાંક કિંમત_changed)
{
    // Skip this when called from a modeline and the filetype was already set
    // to this value.
    જો ((opt_flags & OPT_MODELINE) && !કિંમત_changed)
	પાછા;

    સ્થિર પૂર્ણાંક  ft_recursive = 0;
    પૂર્ણાંક	    secure_save = secure;

    // Reset the secure flag, since the value of 'filetype' has
    // been checked to be safe.
    secure = 0;

    ++ft_recursive;
    curbuf->b_did_filetype = true;
    // Only pass TRUE for "force" when the value changed or not
    // used recursively, to avoid endless recurrence.
    apply_autocmds(EVENT_FILETYPE, curbuf->b_p_ft, curbuf->b_fname,
	    કિંમત_changed || ft_recursive == 1, curbuf);
    --ft_recursive;
    // Just in case the old "curbuf" is now invalid.
    જો (varp != &(curbuf->b_p_ft))
	varp = ખાલી;

    secure = secure_save;
}

#ifdef FEAT_SPELL
/*
 * When the 'spelllang' option is set, source the spell/LANG.vim file in
 * 'runtimepath'.
 */
    સ્થિર શૂન્ય
do_spelllang_source(શૂન્ય)
{
    char_u	fname[200];
    char_u	*p;
    char_u	*q = curwin->w_s->b_p_spl;

    // Skip the first name if it is "cjk".
    જો (STRNCMP(q, "cjk,", 4) == 0)
	q += 4;

    // They could set 'spellcapcheck' depending on the language.  Use the first
    // name in 'spelllang' up to '_region' or '.encoding'.
    માટે (p = q; *p != NUL; ++p)
	જો (!ASCII_ISALNUM(*p) && *p != '-')
	    તોડો;
    જો (p > q)
    {
	vim_snprintf((અક્ષર *)fname, 200, "spell/%.*s.vim",
		(પૂર્ણાંક)(p - q), q);
	source_runtime(fname, DIP_ALL);
    }
}
#endif

/*
 * Handle string options that need some action to perform when changed.
 * The new value must be allocated.
 * Returns NULL for success, or an untranslated error message for an error.
 */
    અક્ષર *
did_set_string_option(
    પૂર્ણાંક		opt_idx,		// index in options[] table
    char_u	**varp,			// pointer to the option variable
    char_u	*oldval,		// previous value of the option
    char_u	*કિંમત,			// new value of the option
    અક્ષર	*errbuf,		// buffer for errors, or NULL
    size_t	errbuflen,		// length of error buffer
    પૂર્ણાંક		opt_flags,		// OPT_LOCAL and/or OPT_GLOBAL
    set_op_T    op,			// OP_ADDING/OP_PREPENDING/OP_REMOVING
    પૂર્ણાંક		*કિંમત_checked)		// value was checked to be safe, no
					// need to set P_INSECURE
{
    અક્ષર	*errmsg = ખાલી;
    long_u	free_oldval = (get_option_flags(opt_idx) & P_ALLOCED);
    opt_did_set_cb_T did_set_cb = get_option_did_set_cb(opt_idx);
    optset_T	args;

    // 'ttytype' is an alias for 'term'.  Both 'term' and 'ttytype' point to
    // T_NAME.  If 'term' or 'ttytype' is modified, then use the index for the
    // 'term' option.  Only set the P_ALLOCED flag on 'term'.
    જો (varp == &T_NAME)
    {
	opt_idx = findoption((char_u *)"term");
	જો (opt_idx >= 0)
	{
	    free_oldval = (get_option_flags(opt_idx) & P_ALLOCED);
	    did_set_cb = get_option_did_set_cb(opt_idx);
	}
    }

    CLEAR_FIELD(args);

    // Disallow changing some options from secure mode
    જો ((secure
#ifdef HAVE_SANDBOX
		|| sandbox != 0
#endif
		) && (get_option_flags(opt_idx) & P_SECURE))
	errmsg = e_not_allowed_here;
    // Check for a "normal" directory or file name in some options.
    નહીંતર જો (check_illegal_path_names(opt_idx, varp))
	errmsg = e_invalid_argument;
    નહીંતર જો (did_set_cb != ખાલી)
    {
	args.os_varp = (char_u *)varp;
	args.os_idx = opt_idx;
	args.os_flags = opt_flags;
	args.os_op = op;
	args.os_oldval.string = oldval;
	args.os_newval.string = કિંમત;
	args.os_errbuf = errbuf;
	args.os_errbuflen = errbuflen;
	// Invoke the option specific callback function to validate and apply
	// the new option value.
	errmsg = did_set_cb(&args);

	// The 'keymap', 'filetype' and 'syntax' option callback functions
	// may change the os_value_checked field.
	*કિંમત_checked = args.os_કિંમત_checked;
    }

    // If an error is detected, restore the previous value.
    જો (errmsg != ખાલી)
    {
	free_string_option(*varp);
	*varp = oldval;
	// When resetting some values, need to act on it.
	જો (args.os_restore_chartab)
	    (શૂન્ય)init_chartab();
	જો (varp == &p_hl)
	    (શૂન્ય)highlight_changed();
    }
    નહીંતર
    {
#ifdef FEAT_EVAL
	// Remember where the option was set.
	set_option_sctx_idx(opt_idx, opt_flags, current_sctx);
#endif
	// Free string options that are in allocated memory.
	// Use "free_oldval", because recursiveness may change the flags under
	// our fingers (esp. init_highlight()).
	જો (free_oldval)
	    free_string_option(oldval);
	set_option_flag(opt_idx, P_ALLOCED);

	જો ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0
		&& is_global_local_option(opt_idx))
	{
	    // global option with local value set to use global value; free
	    // the local value and make it empty
	    char_u *p = get_option_varp_scope(opt_idx, OPT_LOCAL);
	    free_string_option(*(char_u **)p);
	    *(char_u **)p = empty_option;
	}

	// May set global value for local option.
	નહીંતર જો (!(opt_flags & OPT_LOCAL) && opt_flags != OPT_GLOBAL)
	    set_string_option_global(opt_idx, varp);

	// Trigger the autocommand only after setting the flags.
#ifdef FEAT_SYN_HL
	જો (varp == &(curbuf->b_p_syn))
	    do_syntax_autocmd(args.os_કિંમત_changed);
#endif
	નહીંતર જો (varp == &(curbuf->b_p_ft))
	    do_filetype_autocmd(varp, opt_flags, args.os_કિંમત_changed);
#ifdef FEAT_SPELL
	જો (varp == &(curwin->w_s->b_p_spl))
	    do_spelllang_source();
#endif
    }

    જો (varp == &p_mouse)
    {
	જો (*p_mouse == NUL)
	    mch_setmouse(FALSE);    // switch mouse off
	નહીંતર
	    setmouse();		    // in case 'mouse' changed
    }

    જો (varp == &p_rtp)
    {
	export_myvimdir();
#જો defined(FEAT_LUA)
	update_package_paths_in_lua();
#endif
    }

#જો defined(FEAT_LINEBREAK)
    // Changing Formatlistpattern when briopt includes the list setting:
    // redraw
    જો ((varp == &p_flp || varp == &(curbuf->b_p_flp))
	    && curwin->w_briopt_list)
	redraw_all_later(UPD_NOT_VALID);
#endif

    જો (curwin->w_curswant != MAXCOL
		   && (get_option_flags(opt_idx) & (P_CURSWANT | P_RALL)) != 0
				&& (get_option_flags(opt_idx) & P_HLONLY) == 0)
	curwin->w_set_curswant = true;

    જો ((opt_flags & OPT_NO_REDRAW) == 0)
    {
#ifdef FEAT_GUI
	// set when changing an option that only requires a redraw in the GUI
	પૂર્ણાંક	redraw_gui_only = FALSE;

	જો (varp == &p_go			// 'guioptions'
		|| varp == &p_guifont		// 'guifont'
# ifdef FEAT_GUI_TABLINE
		|| varp == &p_gtl		// 'guitablabel'
		|| varp == &p_gtt		// 'guitabtooltip'
# endif
# ifdef FEAT_XFONTSET
		|| varp == &p_guifontset	// 'guifontset'
# endif
		|| varp == &p_guifontwide	// 'guifontwide'
# જો defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
		|| varp == &p_guiligatures	// 'guiligatures'
# endif
	    )
	    redraw_gui_only = TRUE;

	// check redraw when it's not a GUI option or the GUI is active.
	જો (!redraw_gui_only || gui.in_use)
#endif
	    check_redraw(get_option_flags(opt_idx));
    }

#જો defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
    જો (args.os_did_swaptcap)
    {
	set_termname((char_u *)"win32");
	init_highlight(TRUE, FALSE);
    }
#endif

    પાછા errmsg;
}

/*
 * Check an option that can be a range of string values.
 *
 * Return OK for correct value, FAIL otherwise.
 * Empty is always OK.
 */
    સ્થિર પૂર્ણાંક
check_opt_strings(
    char_u	*val,
    અક્ષર	**કિંમતો,
    પૂર્ણાંક		list)	    // when TRUE: accept a list of values
{
    પાછા opt_strings_flags(val, કિંમતો, ખાલી, list);
}

/*
 * Handle an option that can be a range of string values.
 * Set a flag in "*flagp" for each string present.
 *
 * Return OK for correct value, FAIL otherwise.
 * Empty is always OK.
 */
    સ્થિર પૂર્ણાંક
opt_strings_flags(
    char_u	*val,		// new value
    અક્ષર	**કિંમતો,	// array of valid string values
    unsigned	*flagp,
    પૂર્ણાંક		list)		// when TRUE: accept a list of values
{
    પૂર્ણાંક		i;
    પૂર્ણાંક		len;
    unsigned	new_flags = 0;

    જ્યારે (*val)
    {
	માટે (i = 0; ; ++i)
	{
	    જો (કિંમતો[i] == ખાલી)	// val not found in values[]
		પાછા FAIL;

	    len = (પૂર્ણાંક)STRLEN(કિંમતો[i]);
	    જો (STRNCMP(કિંમતો[i], val, len) == 0
		    && ((list && val[len] == ',') || val[len] == NUL))
	    {
		val += len + (val[len] == ',');
		new_flags |= (1 << i);
		તોડો;		// check next item in val list
	    }
	}
    }
    જો (flagp != ખાલી)
	*flagp = new_flags;

    પાછા OK;
}

/*
 * return OK if "p" is a valid fileformat name, FAIL otherwise.
 */
    પૂર્ણાંક
check_ff_કિંમત(char_u *p)
{
    પાછા check_opt_strings(p, p_ff_કિંમતો, FALSE);
}

/*
 * Save the actual shortmess Flags and clear them temporarily to avoid that
 * file messages overwrites any output from the following commands.
 *
 * Caller must make sure to first call save_clear_shm_value() and then
 * restore_shm_value() exactly the same number of times.
 */
    શૂન્ય
save_clear_shm_કિંમત(શૂન્ય)
{
    જો (STRLEN(p_shm) >= SHM_LEN)
    {
	iemsg(e_internal_error_shortmess_too_long);
	પાછા;
    }

    જો (++set_shm_recursive == 1)
    {
	STRCPY(shm_buf, p_shm);
	set_option_કિંમત_give_err((char_u *)"shm", 0L, (char_u *)"", 0);
    }
}

/*
 * Restore the shortmess Flags set from the save_clear_shm_value() function.
 */
    શૂન્ય
restore_shm_કિંમત(શૂન્ય)
{
    જો (--set_shm_recursive == 0)
    {
	set_option_કિંમત_give_err((char_u *)"shm", 0L, shm_buf, 0);
	vim_memset(shm_buf, 0, SHM_LEN);
    }
}

/*
 * Export the environment variable $MYVIMDIR to the first item in runtimepath
 */
    શૂન્ય
export_myvimdir(શૂન્ય)
{
    પૂર્ણાંક		dofree = FALSE;
    char_u	*p;
    char_u	*q = p_rtp;
    string_T	buf;

    buf.string = alloc(MAXPATHL);
    જો (buf.string == ખાલી)
	પાછા;

    buf.length = (size_t)copy_option_part(&q, buf.string, MAXPATHL, ",");

    p = vim_getenv((char_u *)"MYVIMDIR", &dofree);

    જો (p == ખાલી || STRCMP(p, buf.string) != 0)
    {
	જો (*buf.string != NUL && !after_pathsep(buf.string, buf.string + buf.length))
	    STRCPY(buf.string + buf.length, PATHSEPSTR);
#ifdef MSWIN
	// normalize path separators
	માટે (q = buf.string; *q != NUL; q++)
	    જો (*q == '/')
		*q = '\\';
#endif
	vim_setenv((char_u *)"MYVIMDIR", buf.string);
    }
    જો (dofree)
	vim_free(p);
    vim_free(buf.string);
}
