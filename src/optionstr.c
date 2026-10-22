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

#अन्तर्भाव "vim.h"

स्थिर char_u shm_buf[SHM_LEN];
स्थिर पूर्णाङ्क set_shm_recursive = 0;

स्थिर वर्ण *(p_ambw_मानानि[]) = {"single", "double", रिक्त};
स्थिर वर्ण *(p_bg_मानानि[]) = {"light", "dark", रिक्त};
स्थिर वर्ण *(p_bkc_मानानि[]) = {"yes", "auto", "no", "breaksymlink", "breakhardlink", रिक्त};
स्थिर वर्ण *(p_bo_मानानि[]) = {"all", "backspace", "cursor", "complete",
				 "copy", "ctrlg", "error", "esc", "ex",
				 "hangul", "insertmode", "lang", "mess",
				 "showmatch", "operator", "register", "shell",
				 "spell", "term", "wildmode", रिक्त};
#जइ defined(FEAT_LINEBREAK)
// Note: Keep this in sync with briopt_check()
स्थिर वर्ण *(p_briopt_मानानि[]) = {"shift:", "min:", "sbr", "list:", "column:", रिक्त};
#endif
#जइ defined(FEAT_TABPANEL)
// Note: Keep this in sync with tabpanelopt_changed()
स्थिर वर्ण *(p_tplo_मानानि[]) = {"align:", "columns:", "scrollbar", "vert", रिक्त};
स्थिर वर्ण *(p_tplo_align_मानानि[]) = {"left", "right", रिक्त};
#endif
#जइ defined(FEAT_DIFF)
// Note: Keep this in sync with diffopt_changed()
स्थिर वर्ण *(p_dip_मानानि[]) = {"filler", "anchor", "context:", "iblank", "icase", "iwhite", "iwhiteall", "iwhiteeol", "horizontal", "vertical", "closeoff", "hiddenoff", "foldcolumn:", "followwrap", "internal", "indent-heuristic", "algorithm:", "inline:", "linematch:", रिक्त};
स्थिर वर्ण *(p_dip_algorithm_मानानि[]) = {"myers", "minimal", "patience", "histogram", रिक्त};
स्थिर वर्ण *(p_dip_inline_मानानि[]) = {"none", "simple", "char", "word", रिक्त};
#endif
स्थिर वर्ण *(p_nf_मानानि[]) = {"bin", "octal", "hex", "alpha", "unsigned", "blank", रिक्त};
स्थिर वर्ण *(p_ff_मानानि[]) = {FF_UNIX, FF_DOS, FF_MAC, रिक्त};
#ifdef HAVE_CLIPMETHOD
// Note: Keep this in sync with did_set_clipboard()
स्थिर वर्ण *(p_cb_मानानि[]) = {"unnamed", "unnamedplus",
# ifdef FEAT_CLIPBOARD
    "autoselect", "autoselectplus", "autoselectml", "html", "exclude:",
# endif
    रिक्त};
#endif
#ifdef FEAT_CRYPT
स्थिर वर्ण *(p_cm_मानानि[]) = {"zip", "blowfish", "blowfish2",
# ifdef FEAT_SODIUM
    "xchacha20", "xchacha20v2",
# endif
    रिक्त};
#endif
स्थिर वर्ण *(p_cmp_मानानि[]) = {"internal", "keepascii", रिक्त};
#ifdef FEAT_SYN_HL
// Note: Keep this in sync with fill_culopt_flags()
स्थिर वर्ण *(p_culopt_मानानि[]) = {"line", "screenline", "number", "both", रिक्त};
#endif
स्थिर वर्ण *(p_dy_मानानि[]) = {"lastline", "truncate", "uhex", रिक्त};
स्थिर वर्ण *(p_jop_मानानि[]) = {"stack", रिक्त};
#ifdef FEAT_FOLDING
स्थिर वर्ण *(p_fdo_मानानि[]) = {"all", "block", "hor", "mark", "percent",
				 "quickfix", "search", "tag", "insert",
				 "undo", "jump", रिक्त};
#endif
// Note: Keep this in sync with match_keyprotocol()
स्थिर वर्ण *(p_kpc_protocol_मानानि[]) = {"none", "mok2", "kitty", रिक्त};
#ifdef FEAT_IMAGE
// Note: Keep this in sync with match_imageprotocol()
स्थिर वर्ण *(p_ipc_protocol_मानानि[]) = {"none", "kitty", "sixel", रिक्त};
#endif
#ifdef FEAT_PROP_POPUP
// Note: Keep this in sync with parse_popup_option()
स्थिर वर्ण *(p_popup_cpp_option_मानानि[]) = {"align:", "border:",
    "borderhighlight:", "close:", "height:", "highlight:", "opacity:",
    "resize:", "shadow:", "width:", रिक्त};
स्थिर वर्ण *(p_popup_pvp_option_मानानि[]) = {"border:",
    "borderhighlight:", "close:", "height:", "highlight:", "opacity:",
    "resize:", "shadow:", "width:", रिक्त};
स्थिर वर्ण *(p_popup_option_on_off_मानानि[]) = {"on", "off", रिक्त};
स्थिर वर्ण *(p_popup_cpp_border_मानानि[]) = {"single", "double", "round",
    "ascii", "on", "off", "custom:", रिक्त};
स्थिर वर्ण *(p_popup_pvp_border_मानानि[]) = {"single", "double", "round",
    "ascii", "on", "off", "custom:", रिक्त};
स्थिर वर्ण *(p_popup_option_align_मानानि[]) = {"item", "menu", रिक्त};
#endif
#जइ defined(FEAT_SPELL)
// Note: Keep this in sync with spell_check_sps()
स्थिर वर्ण *(p_sps_मानानि[]) = {"best", "fast", "double", "expr:", "file:", "timeout:", रिक्त};
#endif
#ifdef FEAT_SESSION
// Also used for 'viewoptions'!  Keep in sync with SSOP_ flags.
स्थिर वर्ण *(p_ssop_मानानि[]) = {"buffers", "winpos", "resize", "winsize",
    "localoptions", "options", "help", "blank", "globals", "slash", "unix",
    "sesdir", "curdir", "folds", "cursor", "tabpages", "terminal", "skiprtp",
    रिक्त};
#endif
#जइ defined(FEAT_STL_OPT)
स्थिर वर्ण *(p_stlo_मानानि[]) = {"fixedheight", "maxheight:", रिक्त};
#endif
// Keep in sync with SWB_ flags in option.h
स्थिर वर्ण *(p_swb_मानानि[]) = {"useopen", "usetab", "split", "newtab", "vsplit", "uselast", रिक्त};
स्थिर वर्ण *(p_spk_मानानि[]) = {"cursor", "screen", "topline", रिक्त};
स्थिर वर्ण *(p_tc_मानानि[]) = {"followic", "ignore", "match", "followscs", "smart", रिक्त};
// Keep in sync with TCL_ flags in option.h
स्थिर वर्ण *(p_tcl_मानानि[]) = {"left", "uselast", रिक्त};
#जइ defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
स्थिर वर्ण *(p_toolbar_मानानि[]) = {"text", "icons", "tooltips", "horiz", रिक्त};
#endif
#जइ defined(FEAT_TOOLBAR) && defined(FEAT_GUI_GTK)
स्थिर वर्ण *(p_tbis_मानानि[]) = {"tiny", "small", "medium", "large", "huge", "giant", रिक्त};
#endif
#जइ defined(UNIX) || defined(VMS)
स्थिर वर्ण *(p_ttym_मानानि[]) = {"xterm", "xterm2", "dec", "netterm", "jsbterm", "pterm", "urxvt", "sgr", रिक्त};
#endif
स्थिर वर्ण *(p_ve_मानानि[]) = {"block", "insert", "all", "onemore", "none", "NONE", रिक्त};
// Note: Keep this in sync with check_opt_wim()
स्थिर वर्ण *(p_wim_मानानि[]) = {"full", "longest", "list", "lastused", "noselect", "noinsert", रिक्त};
स्थिर वर्ण *(p_wop_मानानि[]) = {"fuzzy", "tagfile", "pum", "exacttext", रिक्त};
#ifdef FEAT_WAK
स्थिर वर्ण *(p_wak_मानानि[]) = {"yes", "menu", "no", रिक्त};
#endif
स्थिर वर्ण *(p_mousem_मानानि[]) = {"extend", "popup", "popup_setpos", "mac", रिक्त};
स्थिर वर्ण *(p_sel_मानानि[]) = {"inclusive", "exclusive", "old", रिक्त};
स्थिर वर्ण *(p_slm_मानानि[]) = {"mouse", "key", "cmd", रिक्त};
स्थिर वर्ण *(p_km_मानानि[]) = {"startsel", "stopsel", रिक्त};
#ifdef FEAT_BROWSE
स्थिर वर्ण *(p_bsdir_मानानि[]) = {"current", "last", "buffer", रिक्त};
#endif
स्थिर वर्ण *(p_scbopt_मानानि[]) = {"ver", "hor", "jump", रिक्त};
स्थिर वर्ण *(p_debug_मानानि[]) = {"msg", "throw", "beep", रिक्त};
स्थिर वर्ण *(p_ead_मानानि[]) = {"both", "ver", "hor", रिक्त};
स्थिर वर्ण *(p_buftype_मानानि[]) = {"nofile", "nowrite", "quickfix", "help", "terminal", "acwrite", "prompt", "popup", रिक्त};
स्थिर वर्ण *(p_bufhidden_मानानि[]) = {"hide", "unload", "delete", "wipe", रिक्त};
स्थिर वर्ण *(p_bs_मानानि[]) = {"indent", "eol", "start", "nostop", रिक्त};
#ifdef FEAT_FOLDING
स्थिर वर्ण *(p_fdm_मानानि[]) = {"manual", "expr", "marker", "indent", "syntax",
# ifdef FEAT_DIFF
				"diff",
# endif
				रिक्त};
स्थिर वर्ण *(p_fcl_मानानि[]) = {"all", रिक्त};
#endif
स्थिर वर्ण *(p_cfc_मानानि[]) = {"keyword", "files", "whole_line", रिक्त};
स्थिर वर्ण *(p_cot_मानानि[]) = {"menu", "menuone", "longest", "preview", "popup", "popuphidden", "noinsert", "noselect", "fuzzy", "nosort", "preinsert", "nearest", रिक्त};
#ifdef BACKSLASH_IN_FILENAME
स्थिर वर्ण *(p_csl_मानानि[]) = {"slash", "backslash", रिक्त};
#endif
#ifdef FEAT_SIGNS
स्थिर वर्ण *(p_scl_मानानि[]) = {"yes", "no", "auto", "number", रिक्त};
#endif
#ifdef UNIX
स्थिर वर्ण *(p_trz_मानानि[]) = {"inband", "sigwinch", "", रिक्त};
#endif
#जइ defined(MSWIN) && defined(FEAT_TERMINAL)
स्थिर वर्ण *(p_twt_मानानि[]) = {"winpty", "conpty", "", रिक्त};
#endif
स्थिर वर्ण *(p_sloc_मानानि[]) = {"last", "statusline", "tabline", रिक्त};
स्थिर वर्ण *(p_sws_मानानि[]) = {"fsync", "sync", रिक्त};

स्थिर पूर्णाङ्क check_opt_strings(char_u *val, वर्ण **मानानि, पूर्णाङ्क list);
स्थिर पूर्णाङ्क opt_strings_flags(char_u *val, वर्ण **मानानि, unsigned *flagp, पूर्णाङ्क list);

/*
 * After setting various option values: recompute variables that depend on
 * option values.
 */
    शून्य
didset_string_options(शून्य)
{
    (शून्य)opt_strings_flags(p_cmp, p_cmp_मानानि, &cmp_flags, TRUE);
    (शून्य)opt_strings_flags(p_bkc, p_bkc_मानानि, &bkc_flags, TRUE);
    (शून्य)opt_strings_flags(p_bo, p_bo_मानानि, &bo_flags, TRUE);
    (शून्य)opt_strings_flags(p_cot, p_cot_मानानि, &cot_flags, TRUE);
#ifdef FEAT_SESSION
    (शून्य)opt_strings_flags(p_ssop, p_ssop_मानानि, &ssop_flags, TRUE);
    (शून्य)opt_strings_flags(p_vop, p_ssop_मानानि, &vop_flags, TRUE);
#endif
#ifdef FEAT_FOLDING
    (शून्य)opt_strings_flags(p_fdo, p_fdo_मानानि, &fdo_flags, TRUE);
#endif
    (शून्य)opt_strings_flags(p_dy, p_dy_मानानि, &dy_flags, TRUE);
    (शून्य)opt_strings_flags(p_jop, p_jop_मानानि, &jop_flags, TRUE);
    (शून्य)opt_strings_flags(p_tc, p_tc_मानानि, &tc_flags, FALSE);
    (शून्य)opt_strings_flags(p_ve, p_ve_मानानि, &ve_flags, TRUE);
#जइ defined(UNIX) || defined(VMS)
    (शून्य)opt_strings_flags(p_ttym, p_ttym_मानानि, &ttym_flags, FALSE);
#endif
#जइ defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
    (शून्य)opt_strings_flags(p_toolbar, p_toolbar_मानानि, &toolbar_flags, TRUE);
#endif
#जइ defined(FEAT_TOOLBAR) && defined(FEAT_GUI_GTK)
    (शून्य)opt_strings_flags(p_tbis, p_tbis_मानानि, &tbis_flags, FALSE);
#endif
    (शून्य)opt_strings_flags(p_swb, p_swb_मानानि, &swb_flags, TRUE);
    (शून्य)opt_strings_flags(p_tcl, p_tcl_मानानि, &tcl_flags, TRUE);
}

#जइ defined(FEAT_EVAL)
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
    शून्य
trigger_optionset_string(
	पूर्णाङ्क	opt_idx,
	पूर्णाङ्क	opt_flags,
	char_u  *oldval,
	char_u  *oldval_l,
	char_u  *oldval_g,
	char_u  *newval)
{
    // Don't do this recursively.
    जइ (oldval == रिक्त || newval == रिक्त
				    || *get_vim_var_str(VV_OPTION_TYPE) != NUL)
	प्रत्यागम;

    char_u buf_type[7];
    size_t buf_typelen;
    size_t oldvallen;

    oldvallen = STRLEN(oldval);
    set_vim_var_string(VV_OPTION_OLD, oldval, (पूर्णाङ्क)oldvallen);
    set_vim_var_string(VV_OPTION_NEW, newval, -1);
    buf_typelen = vim_snprintf_safelen((वर्ण *)buf_type, परिमाण(buf_type),
	"%s", (opt_flags & OPT_LOCAL) ? "local" : "global");
    set_vim_var_string(VV_OPTION_TYPE, buf_type, (पूर्णाङ्क)buf_typelen);
    जइ (opt_flags & OPT_LOCAL)
    {
	set_vim_var_string(VV_OPTION_COMMAND, (char_u *)"setlocal", STRLEN_LITERAL("setlocal"));
	set_vim_var_string(VV_OPTION_OLDLOCAL, oldval, (पूर्णाङ्क)oldvallen);
    }
    जइ (opt_flags & OPT_GLOBAL)
    {
	set_vim_var_string(VV_OPTION_COMMAND, (char_u *)"setglobal", STRLEN_LITERAL("setglobal"));
	set_vim_var_string(VV_OPTION_OLDGLOBAL, oldval, (पूर्णाङ्क)oldvallen);
    }
    जइ ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	set_vim_var_string(VV_OPTION_COMMAND, (char_u *)"set", STRLEN_LITERAL("set"));
	set_vim_var_string(VV_OPTION_OLDLOCAL, oldval_l, -1);
	set_vim_var_string(VV_OPTION_OLDGLOBAL, oldval_g, -1);
    }
    जइ (opt_flags & OPT_MODELINE)
    {
	set_vim_var_string(VV_OPTION_COMMAND, (char_u *)"modeline", STRLEN_LITERAL("modeline"));
	set_vim_var_string(VV_OPTION_OLDLOCAL, oldval, (पूर्णाङ्क)oldvallen);
    }
    apply_autocmds(EVENT_OPTIONSET,
	    get_option_fullname(opt_idx), रिक्त, FALSE,
	    रिक्त);
    reset_v_option_vars();
}
#endif

    स्थिर वर्ण *
illegal_char(वर्ण *errbuf, size_t errbuflen, पूर्णाङ्क c)
{
    जइ (errbuf == रिक्त)
	प्रत्यागम "";
    vim_snprintf(errbuf, errbuflen, _(e_illegal_character_str),
		    (वर्ण *)transchar(c));
    प्रत्यागम errbuf;
}

    स्थिर वर्ण *
illegal_char_after_chr(वर्ण *errbuf, size_t errbuflen, पूर्णाङ्क c)
{
    जइ (errbuf == रिक्त)
	प्रत्यागम "";
    vim_snprintf(errbuf, errbuflen, _(e_illegal_character_after_chr), c);
    प्रत्यागम errbuf;
}

/*
 * Check string options in a buffer for NULL value.
 */
    शून्य
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
#जइ defined(FEAT_EVAL)
    check_string_option(&buf->b_p_inde);
    check_string_option(&buf->b_p_indk);
#endif
#जइ defined(FEAT_BEVAL) && defined(FEAT_EVAL)
    check_string_option(&buf->b_p_bexpr);
#endif
#जइ defined(FEAT_CRYPT)
    check_string_option(&buf->b_p_cm);
#endif
    check_string_option(&buf->b_p_fp);
#जइ defined(FEAT_EVAL)
    check_string_option(&buf->b_p_fex);
#endif
#ifdef FEAT_CRYPT
    check_string_option(&buf->b_p_कुञ्चिका);
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
    शून्य
free_string_option(char_u *p)
{
    जइ (p != empty_option)
	vim_free(p);
}

    शून्य
clear_string_option(char_u **pp)
{
    जइ (*pp != empty_option)
	vim_free(*pp);
    *pp = empty_option;
}

    शून्य
check_string_option(char_u **pp)
{
    जइ (*pp == रिक्त)
	*pp = empty_option;
}

/*
 * Set global value for string option when it's a local option.
 */
    स्थिर शून्य
set_string_option_global(
    पूर्णाङ्क		opt_idx,	// option index
    char_u	**varp)		// pointer to option variable
{
    char_u	**p, *s;

    // the global value is always allocated
    जइ (is_window_local_option(opt_idx))
	p = (char_u **)GLOBAL_WO(varp);
    अण्णहा
	p = (char_u **)get_option_var(opt_idx);
    जइ (!is_global_option(opt_idx)
	    && p != varp
	    && (s = vim_strsave(*varp)) != रिक्त)
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
    शून्य
set_string_option_direct(
    char_u	*name,
    पूर्णाङ्क		opt_idx,
    char_u	*val,
    पूर्णाङ्क		opt_flags,	// OPT_FREE, OPT_LOCAL and/or OPT_GLOBAL
    पूर्णाङ्क		set_sid UNUSED)
{
    char_u	*s;
    char_u	**varp;
    पूर्णाङ्क		both = (opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0;
    पूर्णाङ्क		idx = opt_idx;

    जइ (idx == -1)		// use name
    {
	idx = findoption(name);
	जइ (idx < 0)	// not found (should not happen)
	{
	    semsg(_(e_internal_error_str), "set_string_option_direct()");
	    siemsg("For option %s", name);
	    प्रत्यागम;
	}
    }

    जइ (is_hidden_option(idx))		// can't set hidden option
	प्रत्यागम;

    s = vim_strsave(val);
    जइ (s == रिक्त)
	प्रत्यागम;

    varp = (char_u **)get_option_varp_scope(idx,
	    both ? OPT_LOCAL : opt_flags);
    जइ ((opt_flags & OPT_FREE) && (get_option_flags(idx) & P_ALLOCED))
	free_string_option(*varp);
    *varp = s;

    // For buffer/window local option may also set the global value.
    जइ (both)
	set_string_option_global(idx, varp);

    set_option_flag(idx, P_ALLOCED);

    // When setting both values of a global option with a local value,
    // make the local value empty, so that the global value is used.
    जइ (is_global_local_option(idx) && both)
    {
	free_string_option(*varp);
	*varp = empty_option;
    }
#ifdef FEAT_EVAL
    जइ (set_sid != SID_NONE)
    {
	sctx_T script_ctx;

	जइ (set_sid == 0)
	    script_ctx = current_sctx;
	अण्णहा
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
    शून्य
set_string_option_direct_in_win(
	win_T		*wp,
	char_u		*name,
	पूर्णाङ्क		opt_idx,
	char_u		*val,
	पूर्णाङ्क		opt_flags,
	पूर्णाङ्क		set_sid)
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

#जइ defined(FEAT_PROP_POPUP)
/*
 * Like set_string_option_direct(), but for a buffer-local option in "buf".
 * Blocks autocommands to avoid the old curbuf becoming invalid.
 */
    शून्य
set_string_option_direct_in_buf(
	buf_T		*buf,
	char_u		*name,
	पूर्णाङ्क		opt_idx,
	char_u		*val,
	पूर्णाङ्क		opt_flags,
	पूर्णाङ्क		set_sid)
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
    वर्ण *
set_string_option(
    पूर्णाङ्क		opt_idx,
    char_u	*मानम्,
    पूर्णाङ्क		opt_flags,	// OPT_LOCAL and/or OPT_GLOBAL
    वर्ण	*errbuf,
    size_t	errbuflen)
{
    char_u	*s;
    char_u	**varp;
    char_u	*oldval;
#जइ defined(FEAT_EVAL)
    char_u	*oldval_l = रिक्त;
    char_u	*oldval_g = रिक्त;
    char_u	*saved_oldval = रिक्त;
    char_u	*saved_oldval_l = रिक्त;
    char_u	*saved_oldval_g = रिक्त;
    char_u	*saved_newval = रिक्त;
#endif
    वर्ण	*errmsg = रिक्त;
    पूर्णाङ्क		मानम्_checked = FALSE;

    जइ (is_hidden_option(opt_idx))	// don't set hidden option
	प्रत्यागम रिक्त;

    s = vim_strsave(मानम् == रिक्त ? (char_u *)"" : मानम्);
    जइ (s == रिक्त)
	प्रत्यागम रिक्त;

    varp = (char_u **)get_option_varp_scope(opt_idx,
	    (opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0
	    ? (is_global_local_option(opt_idx)
		? OPT_GLOBAL : OPT_LOCAL)
	    : opt_flags);
    oldval = *varp;
#जइ defined(FEAT_EVAL)
    जइ ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	oldval_l = *(char_u **)get_option_varp_scope(opt_idx, OPT_LOCAL);
	oldval_g = *(char_u **)get_option_varp_scope(opt_idx, OPT_GLOBAL);
    }
#endif
    *varp = s;

#जइ defined(FEAT_EVAL)
    जइ (!starting
# ifdef FEAT_CRYPT
	    && !is_crypt_कुञ्चिका_option(opt_idx)
# endif
       )
    {
	जइ (oldval_l != रिक्त)
	    saved_oldval_l = vim_strsave(oldval_l);
	जइ (oldval_g != रिक्त)
	    saved_oldval_g = vim_strsave(oldval_g);
	saved_oldval = vim_strsave(oldval);
	saved_newval = vim_strsave(s);
    }
#endif
    जइ ((errmsg = did_set_string_option(opt_idx, varp, oldval, मानम्, errbuf,
		    errbuflen, opt_flags, OP_NONE, &मानम्_checked)) == रिक्त)
	did_set_option(opt_idx, opt_flags, TRUE, मानम्_checked);

#जइ defined(FEAT_EVAL)
    // call autocommand after handling side effects
    जइ (errmsg == रिक्त)
	trigger_optionset_string(opt_idx, opt_flags,
		saved_oldval, saved_oldval_l,
		saved_oldval_g, saved_newval);
    vim_free(saved_oldval);
    vim_free(saved_oldval_l);
    vim_free(saved_oldval_g);
    vim_free(saved_newval);
#endif
    प्रत्यागम errmsg;
}

/*
 * Return TRUE if "val" is a valid 'filetype' name.
 * Also used for 'syntax' and 'keymap'.
 */
    स्थिर पूर्णाङ्क
valid_filetype(char_u *val)
{
    प्रत्यागम valid_name(val, ".-_");
}

#ifdef FEAT_STL_OPT
/*
 * Check validity of options with the 'statusline' format.
 * Return an untranslated error message or NULL.
 */
    स्थिर वर्ण *
check_stl_option(char_u *s)
{
    पूर्णाङ्क		groupdepth = 0;
    स्थिर वर्ण errbuf[ERR_BUFLEN];
    पूर्णाङ्क		errbuflen = ERR_BUFLEN;

    जाव (*s)
    {
	// Check for valid keys after % sequences
	जाव (*s && *s != '%')
	    s++;
	जइ (!*s)
	    भङ्ग;
	s++;
	जइ (*s == STL_CLICKFUNC)
	{
	    जइ (s[1] == ']')
	    {
		// %[] - end click region
		s += 2;
		अनुवर्त;
	    }
	    जइ (ASCII_ISALPHA(s[1]) || s[1] == '_')
	    {
		// %[FuncName] - start click region
		char_u *rb = vim_strchr(s + 2, ']');
		जइ (rb != रिक्त)
		{
		    s = rb + 1;
		    अनुवर्त;
		}
	    }
	    // Bare %[ is invalid
	    प्रत्यागम illegal_char(errbuf, errbuflen, *s);
	}
	जइ (*s == STL_LINEBREAK)
	{
	    // Plain %@ - line break
	    s++;
	    अनुवर्त;
	}
	जइ (*s == '%' || *s == STL_TRUNCMARK || *s == STL_SEPARATE)
	{
	    s++;
	    अनुवर्त;
	}
	जइ (*s == ')')
	{
	    s++;
	    जइ (--groupdepth < 0)
		भङ्ग;
	    अनुवर्त;
	}
	जइ (*s == '-')
	    s++;
	जाव (VIM_ISDIGIT(*s))
	    s++;
	जइ (*s == STL_USER_HL)
	    अनुवर्त;
	जइ (*s == STL_CLICKFUNC)
	{
	    // %N[FuncName] or %N[]
	    जइ (s[1] == ']')
	    {
		s += 2;
		अनुवर्त;
	    }
	    जइ (ASCII_ISALPHA(s[1]) || s[1] == '_')
	    {
		char_u *rb = vim_strchr(s + 2, ']');
		जइ (rb != रिक्त)
		{
		    s = rb + 1;
		    अनुवर्त;
		}
	    }
	    // Bare %N[ is invalid
	    प्रत्यागम illegal_char(errbuf, errbuflen, *s);
	}
	जइ (*s == STL_LINEBREAK)
	{
	    // %N@ - line break
	    s++;
	    अनुवर्त;
	}
	जइ (*s == '.')
	{
	    s++;
	    जाव (*s && VIM_ISDIGIT(*s))
		s++;
	}
	जइ (*s == '(')
	{
	    groupdepth++;
	    अनुवर्त;
	}
	जइ (vim_strchr(STL_ALL, *s) == रिक्त)
	{
	    प्रत्यागम illegal_char(errbuf, errbuflen, *s);
	}
	जइ (*s == '{')
	{
	    पूर्णाङ्क reevaluate = (*++s == '%');

	    जइ (reevaluate && *++s == '}')
		// "}" is not allowed immediately after "%{%"
		प्रत्यागम illegal_char(errbuf, errbuflen, '}');
	    जाव ((*s != '}' || (reevaluate && s[-1] != '%')) && *s)
		s++;
	    जइ (*s != '}')
		प्रत्यागम e_unclosed_expression_sequence;
	}
    }
    जइ (groupdepth != 0)
	प्रत्यागम e_unbalanced_groups;
    प्रत्यागम रिक्त;
}
#endif

/*
 * Check for a "normal" directory or file name in some options.  Disallow a
 * path separator (slash and/or backslash), wildcards and characters that are
 * often illegal in a file name. Be more permissive if "secure" is off.
 */
    स्थिर पूर्णाङ्क
check_illegal_path_names(पूर्णाङ्क opt_idx, char_u **varp)
{
    प्रत्यागम (((get_option_flags(opt_idx) & P_NFNAME)
		&& vim_strpbrk(*varp, (char_u *)(secure
			? "/\\*?[|;&<>\r\n" : "/\\*?[<>\r\n")) != रिक्त)
	    || ((get_option_flags(opt_idx) & P_NDNAME)
		&& vim_strpbrk(*varp, (char_u *)"*?[|;&<>\r\n") != रिक्त));
}

/*
 * An option that accepts a list of flags is changed.
 * e.g. 'viewoptions', 'switchbuf', 'casemap', etc.
 */
    स्थिर वर्ण *
did_set_opt_flags(char_u *val, वर्ण **मानानि, unsigned *flagp, पूर्णाङ्क list)
{
    जइ (opt_strings_flags(val, मानानि, flagp, list) == FAIL)
	प्रत्यागम e_invalid_argument;

    प्रत्यागम रिक्त;
}

/*
 * An option that accepts a list of string values is changed.
 * e.g. 'nrformats', 'scrollopt', 'wildoptions', etc.
 */
    स्थिर वर्ण *
did_set_opt_strings(char_u *val, वर्ण **मानानि, पूर्णाङ्क list)
{
    प्रत्यागम did_set_opt_flags(val, मानानि, रिक्त, list);
}

/*
 * An option which is a list of flags is set.  Valid values are in 'flags'.
 */
    स्थिर वर्ण *
did_set_option_listflag(
	char_u *val,
	char_u *flags,
	वर्ण *errbuf,
	size_t errbuflen)
{
    char_u	*s;

    प्रति (s = val; *s; ++s)
	जइ (vim_strchr(flags, *s) == रिक्त)
	    प्रत्यागम illegal_char(errbuf, errbuflen, *s);

    प्रत्यागम रिक्त;
}

/*
 * Expand an option that accepts a list of fixed string values with known
 * number of items.
 */
    स्थिर पूर्णाङ्क
expand_set_opt_string(
	optexpand_T *args,
	वर्ण **मानानि,
	size_t अङ्कः_मानानि,
	पूर्णाङ्क *अङ्कः_Matches,
	char_u ***matches)
{
    char_u	*p;
    regmatch_T	*regmatch = args->oe_regmatch;
    पूर्णाङ्क		include_orig_val = args->oe_include_orig_val;
    char_u	*option_val = args->oe_opt_मानम्;

    // Assume numValues is small since they are fixed enums, so just allocate
    // upfront instead of needing two passes to calculate output size.
    *matches = ALLOC_MULT(char_u *, अङ्कः_मानानि + 1);
    जइ (*matches == रिक्त)
	प्रत्यागम FAIL;

    पूर्णाङ्क गणनम् = 0;

    जइ (include_orig_val && *option_val != NUL)
    {
	p = vim_strsave(option_val);
	जइ (p == रिक्त)
	{
	    VIM_CLEAR(*matches);
	    प्रत्यागम FAIL;
	}
	(*matches)[गणनम्++] = p;
    }

    प्रति (वर्ण **val = मानानि; *val != रिक्त; val++)
    {
	जइ (include_orig_val && *option_val != NUL)
	{
	    जइ (STRCMP((char_u*)*val, option_val) == 0)
		अनुवर्त;
	}
	जइ (vim_regexec(regmatch, (char_u*)(*val), (colnr_T)0))
	{
	    p = vim_strsave((char_u*)*val);
	    जइ (p == रिक्त)
	    {
		जइ (गणनम् == 0)
		{
		    VIM_CLEAR(*matches);
		    प्रत्यागम FAIL;
		}
		अण्णहा
		    भङ्ग;
	    }
	    (*matches)[गणनम्++] = p;
	}
    }
    जइ (गणनम् == 0)
    {
	VIM_CLEAR(*matches);
	प्रत्यागम FAIL;
    }
    *अङ्कः_Matches = गणनम्;
    प्रत्यागम OK;
}

स्थिर char_u *set_opt_callback_orig_option = रिक्त;
स्थिर char_u *((*set_opt_callback_func)(expand_T *, पूर्णाङ्क));

/*
 * Callback used by expand_set_opt_generic to also include the original value
 * as the first item.
 */
    स्थिर char_u *
expand_set_opt_generic_cb(expand_T *xp, पूर्णाङ्क idx)
{
    जइ (idx == 0)
    {
	जइ (set_opt_callback_orig_option != रिक्त)
	    प्रत्यागम set_opt_callback_orig_option;
	अण्णहा
	    प्रत्यागम (char_u *)""; // empty strings are ignored
    }
    प्रत्यागम set_opt_callback_func(xp, idx - 1);
}

/*
 * Expand an option with a callback that iterates through a list of possible
 * names using an index.
 */
    स्थिर पूर्णाङ्क
expand_set_opt_generic(
	optexpand_T *args,
	char_u *((*func)(expand_T *, पूर्णाङ्क)),
	पूर्णाङ्क *अङ्कः_Matches,
	char_u ***matches)
{
    पूर्णाङ्क ret;

    set_opt_callback_orig_option = args->oe_include_orig_val ?
	args->oe_opt_मानम् : रिक्त;
    set_opt_callback_func = func;

    ret = ExpandGeneric(
	    (char_u*)"", // not using fuzzy as currently EXPAND_STRING_SETTING doesn't use it
	    args->oe_xp,
	    args->oe_regmatch,
	    matches,
	    अङ्कः_Matches,
	    expand_set_opt_generic_cb,
	    FALSE);

    set_opt_callback_orig_option = रिक्त;
    set_opt_callback_func = रिक्त;
    प्रत्यागम ret;
}

#जइ defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
स्थिर garray_T *expand_cb_ga;
स्थिर optexpand_T *expand_cb_args;

/*
 * Callback provided to a function in expand_set_opt_callback. Will perform
 * regex matching against the value and add to the list.
 *
 * Returns OK usually. Returns FAIL if it failed to allocate memory, and the
 * caller should terminate the enumeration.
 */
    स्थिर पूर्णाङ्क
expand_set_opt_callback_cb(char_u *val)
{
    regmatch_T	*regmatch = expand_cb_args->oe_regmatch;
    expand_T	*xp = expand_cb_args->oe_xp;
    garray_T	*ga = expand_cb_ga;
    char_u	*str;

    जइ (val == रिक्त || *val == NUL)
	प्रत्यागम OK;

    जइ (xp->xp_pattern[0] != NUL &&
	    !vim_regexec(regmatch, val, (colnr_T)0))
	प्रत्यागम OK;

    str = vim_strsave_escaped(val, (char_u *)" \t\\");

    जइ (str == रिक्त)
	प्रत्यागम FAIL;

    जइ (ga_grow(ga, 1) == FAIL)
    {
	vim_free(str);
	प्रत्यागम FAIL;
    }

    ((char_u **)ga->ga_data)[ga->ga_len] = str;
    ++ga->ga_len;
    प्रत्यागम OK;
}

/*
 * Expand an option with a provided function that takes a callback. The
 * function will enumerate through all options and call the callback to add it
 * to the list.
 *
 * "func" is the enumerator function that will generate the list of options.
 * "func_params" is a single parameter that will be passed to func.
 */
    स्थिर पूर्णाङ्क
expand_set_opt_callback(
	optexpand_T *args,
	शून्य (*func)(optexpand_T *, शून्य* params, पूर्णाङ्क (*cb)(char_u *val)),
	शून्य *func_params,
	पूर्णाङ्क *अङ्कः_Matches,
	char_u ***matches)
{
    garray_T	ga;
    पूर्णाङ्क		include_orig_val = args->oe_include_orig_val;
    char_u	*option_val = args->oe_opt_मानम्;

    ga_init2(&ga, परिमाण(वर्ण *), 30);

    जइ (include_orig_val && *option_val != NUL)
    {
	char_u *p = vim_strsave(option_val);
	जइ (p == रिक्त)
	    प्रत्यागम FAIL;
	जइ (ga_grow(&ga, 1) == FAIL)
	{
	    vim_free(p);
	    प्रत्यागम FAIL;
	}
	((char_u **)ga.ga_data)[ga.ga_len] = p;
	++ga.ga_len;
    }

    expand_cb_ga = &ga;
    expand_cb_args = args;

    func(args, func_params, expand_set_opt_callback_cb);

    expand_cb_ga = रिक्त;
    expand_cb_args = रिक्त;

    *matches = ga.ga_data;
    *अङ्कः_Matches = ga.ga_len;
    प्रत्यागम OK;
}
#endif

/*
 * Expand an option which is a list of flags.
 */
    स्थिर पूर्णाङ्क
expand_set_opt_listflag(
	optexpand_T *args,
	char_u *flags,
	पूर्णाङ्क *अङ्कः_Matches,
	char_u ***matches)
{
    char_u  *p;
    char_u  *option_val = args->oe_opt_मानम्;
    char_u  *cmdline_val = args->oe_set_arg;
    पूर्णाङ्क	    append = args->oe_append;
    पूर्णाङ्क	    include_orig_val = args->oe_include_orig_val && (*option_val != NUL);

    size_t अङ्कः_flags = STRLEN(flags);

    // Assume we only have small number of flags, so just allocate max size.
    *matches = ALLOC_MULT(char_u *, अङ्कः_flags + 1);
    जइ (*matches == रिक्त)
	प्रत्यागम FAIL;

    पूर्णाङ्क गणनम् = 0;

    जइ (include_orig_val)
    {
	p = vim_strsave(option_val);
	जइ (p == रिक्त)
	{
	    VIM_CLEAR(*matches);
	    प्रत्यागम FAIL;
	}
	(*matches)[गणनम्++] = p;
    }

    प्रति (char_u *flag = flags; *flag != NUL; flag++)
    {
	जइ (append && vim_strchr(option_val, *flag) != रिक्त)
	    अनुवर्त;

	जइ (vim_strchr(cmdline_val, *flag) == रिक्त)
	{
	    जइ (include_orig_val
		    && option_val[1] == NUL
		    && *flag == option_val[0])
	    {
		// This value is already used as the first choice as it's the
		// existing flag. Just skip it to avoid duplicate.
		अनुवर्त;
	    }
	    p = vim_strnsave(flag, 1);
	    जइ (p == रिक्त)
	    {
		जइ (गणनम् == 0)
		{
		    VIM_CLEAR(*matches);
		    प्रत्यागम FAIL;
		}
		अण्णहा
		    भङ्ग;
	    }
	    (*matches)[गणनम्++] = p;
	}
    }

    जइ (गणनम् == 0)
    {
	VIM_CLEAR(*matches);
	प्रत्यागम FAIL;
    }
    *अङ्कः_Matches = गणनम्;
    प्रत्यागम OK;
}

/*
 * The 'ambiwidth' option is changed.
 */
    वर्ण *
did_set_ambiwidth(optset_T *args UNUSED)
{
    जइ (check_opt_strings(p_ambw, p_ambw_मानानि, FALSE) != OK)
	प्रत्यागम e_invalid_argument;

    प्रत्यागम check_chars_options();
}

// "name" must be a string literal, the length is computed at compile time.
#define completing_मानम्_for_subopt(args, name) \
	  completing_मानम्_for_subopt_len(args, name, (पूर्णाङ्क)STRLEN_LITERAL(name))

/*
 * Return true when completing the value of the sub-option "name" with length
 * "len", e.g. the value after "close:" in 'completepopup'.
 */
    स्थिर bool
completing_मानम्_for_subopt_len(optexpand_T *args, वर्ण *name, पूर्णाङ्क len)
{
    char_u  *colon = args->oe_xp->xp_pattern - 1;
    पूर्णाङ्क	    off = (पूर्णाङ्क)(colon - args->oe_set_arg);

    जइ (off < len)
	प्रत्यागम false;
    // The name must follow a comma when it does not start the option value.
    जइ (off > len && *(colon - len - 1) != ',')
	प्रत्यागम false;

    प्रत्यागम STRNCMP(colon - len, name, len) == 0;
}

    पूर्णाङ्क
expand_set_ambiwidth(optexpand_T *args, पूर्णाङ्क *अङ्कः_Matches, char_u ***matches)
{
    प्रत्यागम expand_set_opt_string(
	    args,
	    p_ambw_मानानि,
	    ARRAY_LENGTH(p_ambw_मानानि) - 1,
	    अङ्कः_Matches,
	    matches);
}

/*
 * The 'background' option is changed.
 */
    वर्ण *
did_set_background(optset_T *args)
{
    जइ (check_opt_strings(p_bg, p_bg_मानानि, FALSE) == FAIL)
	प्रत्यागम e_invalid_argument;

    जइ (args->os_oldval.string != रिक्त && args->os_oldval.string[0] == *p_bg)
	// Value was not changed
	प्रत्यागम रिक्त;

#ifdef FEAT_EVAL
    पूर्णाङ्क dark = (*p_bg == 'd');
#endif

    init_highlight(FALSE, FALSE);

#ifdef FEAT_EVAL
    जइ (dark != (*p_bg == 'd')
	    && get_var_मानम्((char_u *)"g:colors_name") != रिक्त)
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

    प्रत्यागम रिक्त;
}

    पूर्णाङ्क
expand_set_background(optexpand_T *args, पूर्णाङ्क *अङ्कः_Matches, char_u ***matches)
{
    प्रत्यागम expand_set_opt_string(
	    args,
	    p_bg_मानानि,
	    ARRAY_LENGTH(p_bg_मानानि) - 1,
	    अङ्कः_Matches,
	    matches);
}

/*
 * The 'backspace' option is changed.
 */
    वर्ण *
did_set_backspace(optset_T *args UNUSED)
{
    जइ (VIM_ISDIGIT(*p_bs))
    {
	जइ (*p_bs > '3' || p_bs[1] != NUL)
	    प्रत्यागम e_invalid_argument;
    }
    अण्णहा जइ (check_opt_strings(p_bs, p_bs_मानानि, TRUE) != OK)
	प्रत्यागम e_invalid_argument;

    प्रत्यागम रिक्त;
}

    पूर्णाङ्क
expand_set_backspace(optexpand_T *args, पूर्णाङ्क *अङ्कः_Matches, char_u ***matches)
{
    प्रत्यागम expand_set_opt_string(
	    args,
	    p_bs_मानानि,
	    ARRAY_LENGTH(p_bs_मानानि) - 1,
	    अङ्कः_Matches,
	    matches);
}

/*
 * The 'backupcopy' option is changed.
 */
    वर्ण *
did_set_backupcopy(optset_T *args)
{
    char_u		*bkc = p_bkc;
    unsigned पूर्णाङ्क	*flags = &bkc_flags;
    वर्ण		*errmsg = रिक्त;

    जइ (args->os_flags & OPT_LOCAL)
    {
	bkc = curbuf->b_p_bkc;
	flags = &curbuf->b_bkc_flags;
    }
    अण्णहा जइ (!(args->os_flags & OPT_GLOBAL))
	// When using :set, clear the local flags.
	curbuf->b_bkc_flags = 0;

    जइ ((args->os_flags & OPT_LOCAL) && *bkc == NUL)
	// make the local value empty: use the global value
	*flags = 0;
    अण्णहा
    {
	जइ (opt_strings_flags(bkc, p_bkc_मानानि, flags, TRUE) != OK)
	    errmsg = e_invalid_argument;
	जइ ((((पूर्णाङ्क)*flags & BKC_AUTO) != 0)
		+ (((पूर्णाङ्क)*flags & BKC_YES) != 0)
		+ (((पूर्णाङ्क)*flags & BKC_NO) != 0) != 1)
	{
	    // Must have exactly one of "auto", "yes"  and "no".
	    (शून्य)opt_strings_flags(args->os_oldval.string, p_bkc_मानानि,
								  flags, TRUE);
	    errmsg = e_invalid_argument;
	}
    }

    प्रत्यागम errmsg;
}

    पूर्णाङ्क
expand_set_backupcopy(optexpand_T *args, पूर्णाङ्क *अङ्कः_Matches, char_u ***matches)
{
    प्रत्यागम expand_set_opt_string(
	    args,
	    p_bkc_मानानि,
	    ARRAY_LENGTH(p_bkc_मानानि) - 1,
	    अङ्कः_Matches,
	    matches);
}

/*
 * The 'backupext' or the 'patchmode' option is changed.
 */
    वर्ण *
did_set_backupext_or_patchmode(optset_T *args UNUSED)
{
    जइ (STRCMP(*p_bex == '.' ? p_bex + 1 : p_bex,
		*p_pm == '.' ? p_pm + 1 : p_pm) == 0)
	प्रत्यागम e_backupext_and_patchmode_are_equal;

    प्रत्यागम रिक्त;
}

/*
 * The 'belloff' option is changed.
 */
    वर्ण *
did_set_belloff(optset_T *args UNUSED)
{
    प्रत्यागम did_set_opt_flags(p_bo, p_bo_मानानि, &bo_flags, TRUE);
}

    पूर्णाङ्क
expand_set_belloff(optexpand_T *args, पूर्णाङ्क *अङ्कः_Matches, char_u ***matches)
{
    प्रत्यागम expand_set_opt_string(
	    args,
	    p_bo_मानानि,
	    ARRAY_LENGTH(p_bo_मानानि) - 1,
	    अङ्कः_Matches,
	    matches);
}

#जइ defined(FEAT_LINEBREAK)
/*
 * The 'breakat' option is changed.
 */
    वर्ण *
did_set_breakat(optset_T *args UNUSED)
{
    char_u	*p;
    पूर्णाङ्क		i;

    प्रति (i = 0; i < 256; i++)
	breakat_flags[i] = FALSE;

    जइ (p_breakat != रिक्त)
	प्रति (p = p_breakat; *p; p++)
	    breakat_flags[*p] = TRUE;

    प्रत्यागम रिक्त;
}

/*
 * The 'breakindentopt' option is changed.
 */
    वर्ण *
did_set_breakindentopt(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    जइ (briopt_check(*varp, varp == &curwin->w_p_briopt ? curwin : रिक्त)
								      == FAIL)
	प्रत्यागम e_invalid_argument;

    // list setting requires a redraw
    जइ (varp == &curwin->w_p_briopt && curwin->w_briopt_list)
	redraw_all_later(UPD_NOT_VALID);

    प्रत्यागम रिक्त;
}

    पूर्णाङ्क
expand_set_breakindentopt(
    optexpand_T		*args,
    पूर्णाङ्क			*अङ्कः_Matches,
    char_u		***matches)
{
    प्रत्यागम expand_set_opt_string(
	    args,
	    p_briopt_मानानि,
	    ARRAY_LENGTH(p_briopt_मानानि) - 1,
	    अङ्कः_Matches,
	    matches);
}
#endif

#जइ defined(FEAT_BROWSE)
/*
 * The 'browsedir' option is changed.
 */
    वर्ण *
did_set_browsedir(optset_T *args UNUSED)
{
    जइ (check_opt_strings(p_bsdir, p_bsdir_मानानि, FALSE) != OK
	    && !mch_isdir(p_bsdir))
	प्रत्यागम e_invalid_argument;

    प्रत्यागम रिक्त;
}

    पूर्णाङ्क
expand_set_browsedir(optexpand_T *args, पूर्णाङ्क *अङ्कः_Matches, char_u ***matches)
{
    प्रत्यागम expand_set_opt_string(
	    args,
	    p_bsdir_मानानि,
	    ARRAY_LENGTH(p_bsdir_मानानि) - 1,
	    अङ्कः_Matches,
	    matches);
}
#endif

/*
 * The 'bufhidden' option is changed.
 */
    वर्ण *
did_set_bufhidden(optset_T *args UNUSED)
{
    प्रत्यागम did_set_opt_strings(curbuf->b_p_bh, p_bufhidden_मानानि, FALSE);
}

    पूर्णाङ्क
expand_set_bufhidden(optexpand_T *args, पूर्णाङ्क *अङ्कः_Matches, char_u ***matches)
{
    प्रत्यागम expand_set_opt_string(
	    args,
	    p_bufhidden_मानानि,
	    ARRAY_LENGTH(p_bufhidden_मानानि) - 1,
	    अङ्कः_Matches,
	    matches);
}

/*
 * The 'buftype' option is changed.
 */
    वर्ण *
did_set_buftype(optset_T *args UNUSED)
{
    जइ (check_opt_strings(curbuf->b_p_bt, p_buftype_मानानि, FALSE) != OK)
	प्रत्यागम e_invalid_argument;

    जइ (curwin->w_status_height)
    {
	curwin->w_redr_status = true;
	redraw_later(UPD_VALID);
    }
    curbuf->b_help = (curbuf->b_p_bt[0] == 'h');
    redraw_titles();

    प्रत्यागम रिक्त;
}

    पूर्णाङ्क
expand_set_buftype(optexpand_T *args, पूर्णाङ्क *अङ्कः_Matches, char_u ***matches)
{
    प्रत्यागम expand_set_opt_string(
	    args,
	    p_buftype_मानानि,
	    ARRAY_LENGTH(p_buftype_मानानि) - 1,
	    अङ्कः_Matches,
	    matches);
}

/*
 * The 'casemap' option is changed.
 */
    वर्ण *
did_set_casemap(optset_T *args UNUSED)
{
    प्रत्यागम did_set_opt_flags(p_cmp, p_cmp_मानानि, &cmp_flags, TRUE);
}

    पूर्णाङ्क
expand_set_casemap(optexpand_T *args, पूर्णाङ्क *अङ्कः_Matches, char_u ***matches)
{
    प्रत्यागम expand_set_opt_string(
	    args,
	    p_cmp_मानानि,
	    ARRAY_LENGTH(p_cmp_मानानि) - 1,
	    अङ्कः_Matches,
	    matches);
}

#जइ defined(HAVE_CLIPMETHOD)
    पूर्णाङ्क
expand_set_clipboard(optexpand_T *args, पूर्णाङ्क *अङ्कः_Matches, char_u ***matches)
{
    प्रत्यागम expand_set_opt_string(
	    args,
	    p_cb_मानानि,
	    ARRAY_LENGTH(p_cb_मानानि) - 1,
	    अङ्कः_Matches,
	    matches);
}
#endif

#ifdef HAVE_CLIPMETHOD
    वर्ण *
did_set_clipmethod(optset_T *args UNUSED)
{
    प्रत्यागम choose_clipmethod();
}

    पूर्णाङ्क
expand_set_clipmethod(optexpand_T *args, पूर्णाङ्क *अङ्कः_Matches, char_u ***matches)
{
    // We want to expand using the predefined clipmethod values + clipboard
    // provider names.
    पूर्णाङ्क		result;
    वर्ण	**मानानि;
    पूर्णाङ्क		गणनम्, pos = 0, start = 0;
# ifdef FEAT_EVAL
    dict_T	*providers = get_vim_var_dict(VV_CLIPPROVIDERS);
# अण्णहा
    dict_T	*providers = रिक्त;
# endif
    hashtab_T	*ht = providers == रिक्त ? रिक्त : &providers->dv_hashtab;

    गणनम् = (ht == रिक्त ? 0 : ht->ht_used);
# ifdef FEAT_WAYLAND_CLIPBOARD
    गणनम्++;
    start++;
# endif
# ifdef FEAT_XCLIPBOARD
    गणनम्++;
    start++;
# endif
    मानानि = ALLOC_MULT(वर्ण *, गणनम् + 1); // Add NULL terminator too

    जइ (मानानि == रिक्त)
	प्रत्यागम FAIL;

# ifdef FEAT_WAYLAND_CLIPBOARD
    मानानि[pos++] = "wayland";
# endif
# ifdef FEAT_XCLIPBOARD
    मानानि[pos++] = "x11";
# endif

    जइ (ht != रिक्त)
	प्रति (long_u i = 0; i < ht->ht_mask + 1; i++)
	{
	    hashitem_T	*hi = ht->ht_array + i;

	    जइ (!HASHITEM_EMPTY(hi))
		मानानि[pos++] = (वर्ण *)vim_strsave(hi->hi_कुञ्चिका);
	}
    मानानि[pos++] = रिक्त;

    result = expand_set_opt_string(
	    args,
	    मानानि,
	    गणनम्,
	    अङ्कः_Matches,
	    matches);

    प्रति (पूर्णाङ्क i = start; i < गणनम्; i++)
	vim_free(मानानि[i]);
    vim_free(मानानि);

    प्रत्यागम result;
}
#endif

/*
 * The global 'listchars' or 'fillchars' option is changed.
 */
    स्थिर वर्ण *
did_set_global_listfillchars(char_u *val, पूर्णाङ्क opt_lcs, पूर्णाङ्क opt_flags,
						वर्ण *errbuf, size_t errbuflen)
{
    वर्ण	*errmsg = रिक्त;
    char_u	**local_ptr = opt_lcs ? &curwin->w_p_lcs : &curwin->w_p_fcs;

    // only apply the global value to "curwin" when it does not have a
    // local value
    जइ (opt_lcs)
	errmsg = set_listchars_option(curwin, val,
		**local_ptr == NUL || !(opt_flags & OPT_GLOBAL),
							    errbuf, errbuflen);
    अण्णहा
	errmsg = set_fillchars_option(curwin, val,
		**local_ptr == NUL || !(opt_flags & OPT_GLOBAL),
							    errbuf, errbuflen);
    जइ (errmsg != रिक्त)
	प्रत्यागम errmsg;

    tabpage_T	*tp;
    win_T	*wp;

    // If the current window is set to use the global
    // 'listchars'/'fillchars' value, clear the window-local value.
    जइ (!(opt_flags & OPT_GLOBAL))
	clear_string_option(local_ptr);
    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	// If the current window has a local value need to apply it
	// again, it was changed when setting the global value.
	// If no error was returned above, we don't expect an error
	// here, so ignore the return value.
	जइ (opt_lcs)
	{
	    जइ (*wp->w_p_lcs == NUL)
		(शून्य)set_listchars_option(wp, wp->w_p_lcs, TRUE, रिक्त, 0);
	}
	अण्णहा
	{
	    जइ (*wp->w_p_fcs == NUL)
		(शून्य)set_fillchars_option(wp, wp->w_p_fcs, TRUE, रिक्त, 0);
	}
    }

    redraw_all_later(UPD_NOT_VALID);

    प्रत्यागम रिक्त;
}

/*
 * The 'fillchars' option or the 'listchars' option is changed.
 */
    वर्ण *
did_set_chars_option(optset_T *args)
{
    char_u **varp = (char_u **)args->os_varp;
    वर्ण *errmsg = रिक्त;

    जइ (   varp == &p_lcs		// global 'listchars'
	|| varp == &p_fcs)		// global 'fillchars'
	errmsg = did_set_global_listfillchars(*varp, varp == &p_lcs,
			  args->os_flags, args->os_errbuf, args->os_errbuflen);
    अण्णहा जइ (varp == &curwin->w_p_lcs)	// local 'listchars'
	errmsg = set_listchars_option(curwin, *varp, TRUE,
					  args->os_errbuf, args->os_errbuflen);
    अण्णहा जइ (varp == &curwin->w_p_fcs)	// local 'fillchars'
	errmsg = set_fillchars_option(curwin, *varp, TRUE,
					  args->os_errbuf, args->os_errbuflen);

    प्रत्यागम errmsg;
}

/*
 * Expand 'fillchars' or 'listchars' option value.
 */
    पूर्णाङ्क
expand_set_chars_option(optexpand_T *args, पूर्णाङ्क *अङ्कः_Matches, char_u ***matches)
{
    char_u **varp = (char_u **)args->oe_varp;
    पूर्णाङ्क is_lcs = (varp == &p_lcs || varp == &curwin->w_p_lcs);
    प्रत्यागम expand_set_opt_generic(
	    args,
	    is_lcs ? get_listchars_name : get_fillchars_name,
	    अङ्कः_Matches,
	    matches);
}

/*
 * The 'cinoptions' option is changed.
 */
    वर्ण *
did_set_cinoptions(optset_T *args UNUSED)
{
    // TODO: recognize errors
    parse_cino(curbuf);

    प्रत्यागम रिक्त;
}

#जइ defined(FEAT_SYN_HL)
/*
 * The 'colorcolumn' option is changed.
 */
    वर्ण *
did_set_colorcolumn(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    प्रत्यागम check_colorcolumn(*varp, varp == &curwin->w_p_cc ? curwin : रिक्त);
}
#endif

/*
 * The 'comments' option is changed.
 */
    वर्ण *
did_set_comments(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u	*s;
    वर्ण	*errmsg = रिक्त;

    प्रति (s = *varp; *s; )
    {
	जाव (*s && *s != ':')
	{
	    जइ (vim_strchr((char_u *)COM_ALL, *s) == रिक्त
		    && !VIM_ISDIGIT(*s) && *s != '-')
	    {
		errmsg = illegal_char(args->os_errbuf, args->os_errbuflen, *s);
		भङ्ग;
	    }
	    ++s;
	}
	जइ (*s++ == NUL)
	    errmsg = e_missing_colon;
	अण्णहा जइ (*s == ',' || *s == NUL)
	    errmsg = e_zero_length_string;
	जइ (errmsg != रिक्त)
	    भङ्ग;
	जाव (*s && *s != ',')
	{
	    जइ (*s == '\\' && s[1] != NUL)
		++s;
	    ++s;
	}
	s = skip_to_option_part(s);
    }

    प्रत्यागम errmsg;
}

/*
 * The 'commentstring' option is changed.
 */
    वर्ण *
did_set_commentstring(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    जइ (**varp != NUL && strstr((वर्ण *)*varp, "%s") == रिक्त)
	प्रत्यागम e_commentstring_must_be_empty_or_contain_str;

    प्रत्यागम रिक्त;
}

/*
 * Check if value for 'complete' is valid when 'complete' option is changed.
 */
    वर्ण *
did_set_complete(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u	*p, *t;
    char_u	buffer[LSIZE];
    char_u	*buf_ptr;
    char_u	char_before = NUL;
    पूर्णाङ्क		escape;

    प्रति (p = *varp; *p; )
    {
	vim_memset(buffer, 0, LSIZE);
	buf_ptr = buffer;
	escape = 0;

	// Extract substring while handling escaped commas
	जाव (*p && (*p != ',' || escape) && buf_ptr < (buffer + LSIZE - 1))
	{
	    जइ (*p == '\\' && *(p + 1) == ',')
	    {
		escape = 1;  // Mark escape mode
		p++;         // Skip '\'
	    }
	    अण्णहा
	    {
		escape = 0;
		*buf_ptr++ = *p;
	    }
	    p++;
	}
	*buf_ptr = NUL;

	जइ (vim_strchr((char_u *)".wbuksid]tUFo", *buffer) == रिक्त)
	    प्रत्यागम illegal_char(args->os_errbuf, args->os_errbuflen, *buffer);

	जइ (vim_strchr((char_u *)"ksF", *buffer) == रिक्त && *(buffer + 1) != NUL
		&& *(buffer + 1) != '^')
	    char_before = *buffer;
	अण्णहा
	{
	    // Test for a number after '^'
	    जइ ((t = vim_strchr(buffer, '^')) != रिक्त)
	    {
		*t++ = NUL;
		जइ (!*t)
		    char_before = '^';
		अण्णहा
		{
		    प्रति (; *t; t++)
		    {
			जइ (!vim_isdigit(*t))
			{
			    char_before = '^';
			    भङ्ग;
			}
		    }
		}
	    }
	}
	जइ (char_before != NUL)
	    प्रत्यागम illegal_char_after_chr(args->os_errbuf, args->os_errbuflen,
		    char_before);
	// Skip comma and spaces
	जाव (*p == ',' || *p == ' ')
	    p++;
    }

#ifdef FEAT_COMPL_FUNC
    जइ (set_cpt_callbacks(args) != OK)
	प्रत्यागम illegal_char_after_chr(args->os_errbuf, args->os_errbuflen,
		'F');
#endif
    प्रत्यागम रिक्त;
}

    पूर्णाङ्क
expand_set_complete(optexpand_T *args, पूर्णाङ्क *अङ्कः_Matches, char_u ***matches)
{
    स्थिर वर्ण *(p_cpt_मानानि[]) = {
	".", "w", "b", "u", "k", "kspell", "s", "i", "d", "]", "t", "U", "F", "o",
	रिक्त};
    प्रत्यागम expand_set_opt_string(
	    args,
	    p_cpt_मानानि,
	    ARRAY_LENGTH(p_cpt_मानानि) - 1,
	    अङ्कः_Matches,
	    matches);
}

/*
 * The 'completeopt' option is changed.
 */
    वर्ण *
did_set_completeopt(optset_T *args UNUSED)
{
    char_u	*cot = p_cot;
    unsigned	*flags = &cot_flags;

    जइ (args->os_flags & OPT_LOCAL)
    {
	cot = curbuf->b_p_cot;
	flags = &curbuf->b_cot_flags;
    }
    अण्णहा जइ (!(args->os_flags & OPT_GLOBAL))
	// When using :set, clear the local flags.
	curbuf->b_cot_flags = 0;

    जइ (opt_strings_flags(cot, p_cot_मानानि, flags, TRUE) != OK)
	प्रत्यागम e_invalid_argument;

    प्रत्यागम रिक्त;
}

    पूर्णाङ्क
expand_set_completeopt(optexpand_T *args, पूर्णाङ्क *अङ्कः_Matches, char_u ***matches)
{
    प्रत्यागम expand_set_opt_string(
	    args,
	    p_cot_मानानि,
	    ARRAY_LENGTH(p_cot_मानानि) - 1,
	    अङ्कः_Matches,
	    matches);
}

/*
 * The 'completefuzzycollect' option is changed.
 */
    वर्ण *
did_set_completefuzzycollect(optset_T *args UNUSED)
{
    जइ (opt_strings_flags(p_cfc, p_cfc_मानानि, &cfc_flags, TRUE) != OK)
	प्रत्यागम e_invalid_argument;
    प्रत्यागम रिक्त;
}

    पूर्णाङ्क
expand_set_completefuzzycollect(
	optexpand_T *args,
	पूर्णाङ्क *अङ्कः_Matches,
	char_u ***matches)
{
    प्रत्यागम expand_set_opt_string(
	    args,
	    p_cfc_मानानि,
	    ARRAY_LENGTH(p_cfc_मानानि) - 1,
	    अङ्कः_Matches,
	    matches);
}

/*
 * The 'completeitemalign' option is changed.
 */
    वर्ण *
did_set_completeitemalign(optset_T *args UNUSED)
{
    char_u	*p = p_cia;
    unsigned	new_cia_flags = 0;
    पूर्णाङ्क		seen[3] = { FALSE, FALSE, FALSE };
    पूर्णाङ्क		गणनम् = 0;
    char_u	buf[10];

    जाव (*p)
    {
	copy_option_part(&p, buf, परिमाण(buf), ",");
	जइ (गणनम् >= 3)
	    प्रत्यागम e_invalid_argument;

	जइ (STRCMP(buf, "abbr") == 0)
	{
	    जइ (seen[CPT_ABBR])
		प्रत्यागम e_invalid_argument;
	    new_cia_flags = new_cia_flags * 10 + CPT_ABBR;
	    seen[CPT_ABBR] = TRUE;
	    गणनम्++;
	}
	अण्णहा जइ (STRCMP(buf, "kind") == 0)
	{
	    जइ (seen[CPT_KIND])
		प्रत्यागम e_invalid_argument;
	    new_cia_flags = new_cia_flags * 10 + CPT_KIND;
	    seen[CPT_KIND] = TRUE;
	    गणनम्++;
	}
	अण्णहा जइ (STRCMP(buf, "menu") == 0)
	{
	    जइ (seen[CPT_MENU])
		प्रत्यागम e_invalid_argument;
	    new_cia_flags = new_cia_flags * 10 + CPT_MENU;
	    seen[CPT_MENU] = TRUE;
	    गणनम्++;
	}
	अण्णहा
	    प्रत्यागम e_invalid_argument;
    }
    जइ (new_cia_flags == 0 || गणनम् != 3)
	प्रत्यागम e_invalid_argument;

    cia_flags = new_cia_flags;
    प्रत्यागम रिक्त;
}

#जइ defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
/*
 * The 'completepopup' option is changed.
 */
    वर्ण *
did_set_completepopup(optset_T *args UNUSED)
{
    जइ (parse_completepopup(रिक्त) == FAIL)
	प्रत्यागम e_invalid_argument;

    popup_close_info();
    प्रत्यागम रिक्त;
}
#endif

#जइ defined(BACKSLASH_IN_FILENAME)
/*
 * The 'completeslash' option is changed.
 */
    वर्ण *
did_set_completeslash(optset_T *args UNUSED)
{
    जइ (check_opt_strings(p_csl, p_csl_मानानि, FALSE) != OK
	    || check_opt_strings(curbuf->b_p_csl, p_csl_मानानि, FALSE) != OK)
	प्रत्यागम e_invalid_argument;

    प्रत्यागम रिक्त;
}

    पूर्णाङ्क
expand_set_completeslash(optexpand_T *args, पूर्णाङ्क *अङ्कः_Matches, char_u ***matches)
{
    प्रत्यागम expand_set_opt_string(
	    args,
	    p_csl_मानानि,
	    ARRAY_LENGTH(p_csl_मानानि) - 1,
	    अङ्कः_Matches,
	    matches);
}
#endif

#जइ defined(FEAT_CONCEAL)
/*
 * The 'concealcursor' option is changed.
 */
    वर्ण *
did_set_concealcursor(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    प्रत्यागम did_set_option_listflag(*varp, (char_u *)COCU_ALL, args->os_errbuf,
		    args->os_errbuflen);
}

    पूर्णाङ्क
expand_set_concealcursor(optexpand_T *args, पूर्णाङ्क *अङ्कः_Matches, char_u ***matches)
{
    प्रत्यागम expand_set_opt_listflag(args, (char_u*)COCU_ALL, अङ्कः_Matches, matches);
}
#endif

/*
 * The 'cpoptions' option is changed.
 */
    वर्ण *
did_set_cpoptions(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    प्रत्यागम did_set_option_listflag(*varp, (char_u *)CPO_ALL, args->os_errbuf,
		    args->os_errbuflen);
}

    पूर्णाङ्क
expand_set_cpoptions(optexpand_T *args, पूर्णाङ्क *अङ्कः_Matches, char_u ***matches)
{
    प्रत्यागम expand_set_opt_listflag(args, (char_u*)CPO_ALL, अङ्कः_Matches, matches);
}

#जइ defined(FEAT_CRYPT)
/*
 * The 'cryptkey' option is changed.
 */
    वर्ण *
did_set_cryptkey(optset_T *args)
{
    // Make sure the ":set" command doesn't show the new value in the
    // history.
    remove_कुञ्चिका_from_history();

    जइ (args->os_op != OP_NONE)
	// Don't allow set+=/-=/^= as they can allow for substring guessing
	प्रत्यागम e_invalid_argument;

    जइ (STRCMP(curbuf->b_p_कुञ्चिका, args->os_oldval.string) != 0)
    {
	// Need to update the swapfile.
	ml_set_crypt_कुञ्चिका(curbuf, args->os_oldval.string,
		*curbuf->b_p_cm == NUL ? p_cm : curbuf->b_p_cm);
	changed_internal();
    }
# ifdef FEAT_SODIUM
    जइ (crypt_method_is_sodium(crypt_get_method_nr(curbuf)))
	crypt_sodium_lock_कुञ्चिका(args->os_newval.string);
# endif

    प्रत्यागम रिक्त;
}

/*
 * The 'cryptmethod' option is changed.
 */
    वर्ण *
did_set_cryptmethod(optset_T *args)
{
    char_u  *p;
    char_u  *s;

    जइ (args->os_flags & OPT_LOCAL)
	p = curbuf->b_p_cm;
    अण्णहा
	p = p_cm;
    जइ (check_opt_strings(p, p_cm_मानानि, TRUE) != OK)
	प्रत्यागम e_invalid_argument;
    अण्णहा जइ (crypt_self_test() == FAIL)
	प्रत्यागम e_invalid_argument;

    // When setting the global value to empty, make it "zip".
    जइ (*p_cm == NUL)
    {
	free_string_option(p_cm);
	p_cm = vim_strsave((char_u *)"zip");
    }
    // When using ":set cm=name" the local value is going to be empty.
    // Do that here, otherwise the crypt functions will still use the
    // local value.
    जइ ((args->os_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	free_string_option(curbuf->b_p_cm);
	curbuf->b_p_cm = empty_option;
    }

    // Need to update the swapfile when the effective method changed.
    // Set "s" to the effective old value, "p" to the effective new
    // method and compare.
    जइ ((args->os_flags & OPT_LOCAL) && *args->os_oldval.string == NUL)
	s = p_cm;  // was previously using the global value
    अण्णहा
	s = args->os_oldval.string;
    जइ (*curbuf->b_p_cm == NUL)
	p = p_cm;  // is now using the global value
    अण्णहा
	p = curbuf->b_p_cm;
    जइ (STRCMP(s, p) != 0)
	ml_set_crypt_कुञ्चिका(curbuf, curbuf->b_p_कुञ्चिका, s);

    // If the global value changes need to update the swapfile for all
    // buffers using that value.
    जइ ((args->os_flags & OPT_GLOBAL)
	    && STRCMP(p_cm, args->os_oldval.string) != 0)
    {
	buf_T	*buf;

	FOR_ALL_BUFFERS(buf)
	    जइ (buf != curbuf && *buf->b_p_cm == NUL)
		ml_set_crypt_कुञ्चिका(buf, buf->b_p_कुञ्चिका, args->os_oldval.string);
    }
    प्रत्यागम रिक्त;
}

    पूर्णाङ्क
expand_set_cryptmethod(optexpand_T *args, पूर्णाङ्क *अङ्कः_Matches, char_u ***matches)
{
    प्रत्यागम expand_set_opt_string(
	    args,
	    p_cm_मानानि,
	    ARRAY_LENGTH(p_cm_मानानि) - 1,
	    अङ्कः_Matches,
	    matches);
}
#endif

#जइ defined(FEAT_CSCOPE) && defined(FEAT_QUICKFIX)
/*
 * The 'cscopequickfix' option is changed.
 */
    वर्ण *
did_set_cscopequickfix(optset_T *args UNUSED)
{
    char_u	*p;

    जइ (p_csqf == रिक्त)
	प्रत्यागम रिक्त;

    p = p_csqf;
    जाव (*p != NUL)
    {
	जइ (vim_strchr((char_u *)CSQF_CMDS, *p) == रिक्त
		|| p[1] == NUL
		|| vim_strchr((char_u *)CSQF_FLAGS, p[1]) == रिक्त
		|| (p[2] != NUL && p[2] != ','))
	    प्रत्यागम e_invalid_argument;
	अण्णहा जइ (p[2] == NUL)
	    भङ्ग;
	अण्णहा
	    p += 3;
    }

    प्रत्यागम रिक्त;
}
#endif

#जइ defined(FEAT_SYN_HL)
/*
 * The 'cursorlineopt' option is changed.
 */
    वर्ण *
did_set_cursorlineopt(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    // This could be changed to use opt_strings_flags() instead.
    जइ (**varp == NUL || fill_culopt_flags(*varp, curwin) != OK)
	प्रत्यागम e_invalid_argument;

    प्रत्यागम रिक्त;
}

    पूर्णाङ्क
expand_set_cursorlineopt(optexpand_T *args, पूर्णाङ्क *अङ्कः_Matches, char_u ***matches)
{
    प्रत्यागम expand_set_opt_string(
	    args,
	    p_culopt_मानानि,
	    ARRAY_LENGTH(p_culopt_मानानि) - 1,
	    अङ्कः_Matches,
	    matches);
}
#endif

/*
 * The 'debug' option is changed.
 */
    वर्ण *
did_set_debug(optset_T *args UNUSED)
{
    प्रत्यागम did_set_opt_strings(p_debug, p_debug_मानानि, TRUE);
}

    पूर्णाङ्क
expand_set_debug(optexpand_T *args, पूर्णाङ्क *अङ्कः_Matches, char_u ***matches)
{
    प्रत्यागम expand_set_opt_string(
	    args,
	    p_debug_मानानि,
	    ARRAY_LENGTH(p_debug_मानानि) - 1,
	    अङ्कः_Matches,
	    matches);
}

#जइ defined(FEAT_DIFF)
/*
 * The 'diffanchors' option is changed.
 */
    वर्ण *
did_set_diffanchors(optset_T *args)
{
    जइ (diffanchors_changed(args->os_flags & OPT_LOCAL) == FAIL)
	प्रत्यागम e_invalid_argument;

    प्रत्यागम रिक्त;
}

/*
 * The 'diffopt' option is changed.
 */
    वर्ण *
did_set_diffopt(optset_T *args UNUSED)
{
    जइ (diffopt_changed() == FAIL)
	प्रत्यागम e_invalid_argument;

    प्रत्यागम रिक्त;
}

    पूर्णाङ्क
expand_set_diffopt(optexpand_T *args, पूर्णाङ्क *अङ्कः_Matches, char_u ***matches)
{
    expand_T *xp = args->oe_xp;

    जइ (xp->xp_pattern > args->oe_set_arg && *(xp->xp_pattern-1) == ':')
    {
	जइ (completing_मानम्_for_subopt(args, "algorithm"))
	{
	    प्रत्यागम expand_set_opt_string(
		    args,
		    p_dip_algorithm_मानानि,
		    ARRAY_LENGTH(p_dip_algorithm_मानानि) - 1,
		    अङ्कः_Matches,
		    matches);
	}
	जइ (completing_मानम्_for_subopt(args, "inline"))
	{
	    प्रत्यागम expand_set_opt_string(
		    args,
		    p_dip_inline_मानानि,
		    ARRAY_LENGTH(p_dip_inline_मानानि) - 1,
		    अङ्कः_Matches,
		    matches);
	}
	प्रत्यागम FAIL;
    }

    प्रत्यागम expand_set_opt_string(
	    args,
	    p_dip_मानानि,
	    ARRAY_LENGTH(p_dip_मानानि) - 1,
	    अङ्कः_Matches,
	    matches);
}
#endif

/*
 * The 'display' option is changed.
 */
    वर्ण *
did_set_display(optset_T *args UNUSED)
{
    जइ (opt_strings_flags(p_dy, p_dy_मानानि, &dy_flags, TRUE) != OK)
	प्रत्यागम e_invalid_argument;

    (शून्य)init_chartab();
    प्रत्यागम रिक्त;
}

    पूर्णाङ्क
expand_set_display(optexpand_T *args, पूर्णाङ्क *अङ्कः_Matches, char_u ***matches)
{
    प्रत्यागम expand_set_opt_string(
	    args,
	    p_dy_मानानि,
	    ARRAY_LENGTH(p_dy_मानानि) - 1,
	    अङ्कः_Matches,
	    matches);
}

/*
 * The 'eadirection' option is changed.
 */
    वर्ण *
did_set_eadirection(optset_T *args UNUSED)
{
    प्रत्यागम did_set_opt_strings(p_ead, p_ead_मानानि, FALSE);
}

    पूर्णाङ्क
expand_set_eadirection(optexpand_T *args, पूर्णाङ्क *अङ्कः_Matches, char_u ***matches)
{
    प्रत्यागम expand_set_opt_string(
	    args,
	    p_ead_मानानि,
	    ARRAY_LENGTH(p_ead_मानानि) - 1,
	    अङ्कः_Matches,
	    matches);
}

/*
 * One of the 'encoding', 'fileencoding', 'termencoding' or 'makeencoding'
 * options is changed.
 */
    वर्ण *
did_set_encoding(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u	**gvarp;
    वर्ण	*errmsg = रिक्त;
    char_u	*p;

    // Get the global option to compare with, otherwise we would have to check
    // two values for all local options.
    gvarp = (char_u **)get_option_varp_scope(args->os_idx, OPT_GLOBAL);

    जइ (gvarp == &p_fenc)
    {
	जइ (!curbuf->b_p_ma && args->os_flags != OPT_GLOBAL)
	    errmsg = e_cannot_make_changes_modifiable_is_off;
	अण्णहा जइ (vim_strchr(*varp, ',') != रिक्त)
	    // No comma allowed in 'fileencoding'; catches confusing it
	    // with 'fileencodings'.
	    errmsg = e_invalid_argument;
	अण्णहा
	{
	    // May show a "+" in the title now.
	    redraw_titles();
	    // Add 'fileencoding' to the swap file.
	    ml_setflags(curbuf);
	}
    }
    जइ (errmsg == रिक्त)
    {
	// canonize the value, so that STRCMP() can be used on it
	p = enc_canonize(*varp);
	जइ (p != रिक्त)
	{
	    vim_free(*varp);
	    *varp = p;
	}
	जइ (varp == &p_enc)
	{
	    errmsg = mb_init();
	    redraw_titles();
	}
    }

#जइ defined(FEAT_GUI_GTK)
    जइ (errmsg == रिक्त && varp == &p_tenc && gui.in_use)
    {
	// GTK uses only a single encoding, and that is UTF-8.
	जइ (STRCMP(p_tenc, "utf-8") != 0)
	    errmsg = e_cannot_be_changed_in_gtk_GUI;
    }
#endif

    जइ (errmsg == रिक्त)
    {
#ifdef FEAT_KEYMAP
	// When 'keymap' is used and 'encoding' changes, reload the keymap
	// (with another encoding).
	जइ (varp == &p_enc && *curbuf->b_p_keymap != NUL)
	    (शून्य)keymap_init();
#endif

	// When 'termencoding' is not empty and 'encoding' changes or when
	// 'termencoding' changes, need to setup for keyboard input and
	// display output conversion.
	जइ (((varp == &p_enc && *p_tenc != NUL) || varp == &p_tenc))
	{
	    जइ (convert_setup(&निवेशः_conv, p_tenc, p_enc) == FAIL
		    || convert_setup(&output_conv, p_enc, p_tenc) == FAIL)
	    {
		semsg(_(e_cannot_convert_between_str_and_str),
			p_tenc, p_enc);
		errmsg = e_invalid_argument;
	    }
	}

#जइ defined(MSWIN)
	// $HOME, $VIM and $VIMRUNTIME may have characters in active code page.
	जइ (varp == &p_enc)
	{
	    init_homedir();
	    init_vimdir();
	}
#endif
    }

    प्रत्यागम errmsg;
}

    पूर्णाङ्क
expand_set_encoding(optexpand_T *args, पूर्णाङ्क *अङ्कः_Matches, char_u ***matches)
{
    प्रत्यागम expand_set_opt_generic(
	    args,
	    get_encoding_name,
	    अङ्कः_Matches,
	    matches);
}

/*
 * The 'eventignore(win)' option is changed.
 */
    वर्ण *
did_set_eventignore(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u	*oldval = args->os_oldval.string;

    जइ (check_ei(*varp) == FAIL)
	प्रत्यागम e_invalid_argument;

    जइ (oldval == रिक्त || STRCMP(oldval, *varp) == 0)
	प्रत्यागम रिक्त;

    // Deal with the events that are triggered by comparing against a stored
    // state, with the old value in effect: what happened while an event was
    // ignored must not be reported once it is not ignored anymore, and what
    // happened before must still be reported.
    // Use a copy, the caller owns "oldval" and autocommands may free it.
    char_u	*save_ei = vim_strsave(oldval);
    जइ (save_ei != रिक्त)
    {
	char_u	*newval = *varp;
	win_T	*wp = is_window_local_option(args->os_idx) ? curwin : रिक्त;

	*varp = save_ei;
	// "varp" points into "wp" for 'eventignorewin'.
	जइ (wp != रिक्त)
	    ++wp->w_locked;
	may_trigger_deferred_events();
	जइ (wp != रिक्त)
	    --wp->w_locked;

	free_string_option(*varp);
	*varp = newval;
    }

    प्रत्यागम रिक्त;
}

स्थिर पूर्णाङ्क expand_eiw = FALSE;

    स्थिर char_u *
get_eventignore_name(expand_T *xp, पूर्णाङ्क idx)
{
    पूर्णाङ्क subtract = *xp->xp_pattern == '-';
    // 'eventignore(win)' allows special keyword "all" in addition to
    // all event names.
    जइ (!subtract && idx == 0)
	प्रत्यागम (char_u *)"all";

    char_u *name = get_event_name_no_group(xp, idx - 1 + subtract, expand_eiw);
    जइ (name == रिक्त)
	प्रत्यागम रिक्त;

    sprintf((वर्ण *)IObuff, "%s%s", subtract ? "-" : "", name);
    प्रत्यागम IObuff;
}

    पूर्णाङ्क
expand_set_eventignore(optexpand_T *args, पूर्णाङ्क *अङ्कः_Matches, char_u ***matches)
{
    expand_eiw = args->oe_varp != (char_u *)&p_ei;
    प्रत्यागम expand_set_opt_generic(
	    args,
	    get_eventignore_name,
	    अङ्कः_Matches,
	    matches);
}

/*
 * The 'fileformat' option is changed.
 */
    वर्ण *
did_set_fileformat(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    जइ (!curbuf->b_p_ma && !(args->os_flags & OPT_GLOBAL))
	प्रत्यागम e_cannot_make_changes_modifiable_is_off;
    अण्णहा जइ (check_opt_strings(*varp, p_ff_मानानि, FALSE) != OK)
	प्रत्यागम e_invalid_argument;

    // may also change 'textmode'
    जइ (get_fileformat(curbuf) == EOL_DOS)
	curbuf->b_p_tx = TRUE;
    अण्णहा
	curbuf->b_p_tx = FALSE;
    redraw_titles();
    // update flag in swap file
    ml_setflags(curbuf);
    // Redraw needed when switching to/from "mac": a CR in the text
    // will be displayed differently.
    जइ (get_fileformat(curbuf) == EOL_MAC || *args->os_oldval.string == 'm')
	redraw_curbuf_later(UPD_NOT_VALID);

    प्रत्यागम रिक्त;
}

    पूर्णाङ्क
expand_set_fileformat(optexpand_T *args, पूर्णाङ्क *अङ्कः_Matches, char_u ***matches)
{
    प्रत्यागम expand_set_opt_string(
	    args,
	    p_ff_मानानि,
	    ARRAY_LENGTH(p_ff_मानानि) - 1,
	    अङ्कः_Matches,
	    matches);
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * fileformat options.
 */
    char_u *
get_fileformat_name(expand_T *xp UNUSED, पूर्णाङ्क idx)
{
    जइ (idx >= (पूर्णाङ्क)ARRAY_LENGTH(p_ff_मानानि))
	प्रत्यागम रिक्त;

    प्रत्यागम (char_u*)p_ff_मानानि[idx];
}

/*
 * The 'fileformats' option is changed.
 */
    वर्ण *
did_set_fileformats(optset_T *args UNUSED)
{
    जइ (check_opt_strings(p_ffs, p_ff_मानानि, TRUE) != OK)
	प्रत्यागम e_invalid_argument;

    // also change 'textauto'
    जइ (*p_ffs == NUL)
	p_ta = FALSE;
    अण्णहा
	p_ta = TRUE;

    प्रत्यागम रिक्त;
}

/*
 * The 'filetype' or the 'syntax' option is changed.
 */
    वर्ण *
did_set_filetype_or_syntax(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    जइ (!valid_filetype(*varp))
	प्रत्यागम e_invalid_argument;

    args->os_मानम्_changed = STRCMP(args->os_oldval.string, *varp) != 0;

    // Since we check the value, there is no need to set P_INSECURE,
    // even when the value comes from a modeline.
    args->os_मानम्_checked = TRUE;

    प्रत्यागम रिक्त;
}

#जइ defined(FEAT_FOLDING)
/*
 * The 'foldclose' option is changed.
 */
    वर्ण *
did_set_foldclose(optset_T *args UNUSED)
{
    प्रत्यागम did_set_opt_strings(p_fcl, p_fcl_मानानि, TRUE);
}

    पूर्णाङ्क
expand_set_foldclose(optexpand_T *args, पूर्णाङ्क *अङ्कः_Matches, char_u ***matches)
{
    प्रत्यागम expand_set_opt_string(
	    args,
	    p_fcl_मानानि,
	    ARRAY_LENGTH(p_fcl_मानानि) - 1,
	    अङ्कः_Matches,
	    matches);
}
#endif

#जइ defined(FEAT_EVAL) && defined(FEAT_FOLDING)
/*
 * The 'foldexpr' option is changed.
 */
    वर्ण *
did_set_foldexpr(optset_T *args)
{
    (शून्य)did_set_optexpr(args);
    जइ (foldmethod_Is_व्यञ्जकम्(curwin))
	foldUpdateAll(curwin);
    प्रत्यागम रिक्त;
}
#endif

#जइ defined(FEAT_FOLDING)
/*
 * The 'foldignore' option is changed.
 */
    वर्ण *
did_set_foldignore(optset_T *args UNUSED)
{
    जइ (foldmethodIsIndent(curwin))
	foldUpdateAll(curwin);
    प्रत्यागम रिक्त;
}

/*
 * The 'foldmarker' option is changed.
 */
    वर्ण *
did_set_foldmarker(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u	*p;

    p = vim_strchr(*varp, ',');
    जइ (p == रिक्त)
	प्रत्यागम e_comma_required;
    अण्णहा जइ (p == *varp || p[1] == NUL)
	प्रत्यागम e_invalid_argument;
    अण्णहा जइ (foldmethodIsMarker(curwin))
	foldUpdateAll(curwin);

    प्रत्यागम रिक्त;
}

/*
 * The 'foldmethod' option is changed.
 */
    वर्ण *
did_set_foldmethod(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    जइ (check_opt_strings(*varp, p_fdm_मानानि, FALSE) != OK || **varp == NUL)
	प्रत्यागम e_invalid_argument;

    foldUpdateAll(curwin);
    जइ (foldmethodIsDiff(curwin))
	newFoldLevel();
    प्रत्यागम रिक्त;
}

    पूर्णाङ्क
expand_set_foldmethod(optexpand_T *args, पूर्णाङ्क *अङ्कः_Matches, char_u ***matches)
{
    प्रत्यागम expand_set_opt_string(
	    args,
	    p_fdm_मानानि,
	    ARRAY_LENGTH(p_fdm_मानानि) - 1,
	    अङ्कः_Matches,
	    matches);
}

/*
 * The 'foldopen' option is changed.
 */
    वर्ण *
did_set_foldopen(optset_T *args UNUSED)
{
    प्रत्यागम did_set_opt_flags(p_fdo, p_fdo_मानानि, &fdo_flags, TRUE);
}

    पूर्णाङ्क
expand_set_foldopen(optexpand_T *args, पूर्णाङ्क *अङ्कः_Matches, char_u ***matches)
{
    प्रत्यागम expand_set_opt_string(
	    args,
	    p_fdo_मानानि,
	    ARRAY_LENGTH(p_fdo_मानानि) - 1,
	    अङ्कः_Matches,
	    matches);
}
#endif

/*
 * The 'formatoptions' option is changed.
 */
    वर्ण *
did_set_formatoptions(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    प्रत्यागम did_set_option_listflag(*varp, (char_u *)FO_ALL, args->os_errbuf,
		    args->os_errbuflen);
}

    पूर्णाङ्क
expand_set_formatoptions(optexpand_T *args, पूर्णाङ्क *अङ्कः_Matches, char_u ***matches)
{
    प्रत्यागम expand_set_opt_listflag(args, (char_u*)FO_ALL, अङ्कः_Matches, matches);
}

#जइ defined(CURSOR_SHAPE)
/*
 * The 'guicursor' option is changed.
 */
    वर्ण *
did_set_guicursor(optset_T *args UNUSED)
{
    प्रत्यागम parse_shape_opt(SHAPE_CURSOR);
}
#endif

#जइ defined(FEAT_GUI)
/*
 * The 'guifont' option is changed.
 */
    वर्ण *
did_set_guifont(optset_T *args UNUSED)
{
    char_u	*p;
    वर्ण	*errmsg = रिक्त;

    जइ (gui.in_use)
    {
	p = p_guifont;
# जइ defined(FEAT_GUI_GTK)
	// Put up a font dialog and let the user select a new value.
	// If this is cancelled go back to the old value but don't
	// give an error message.
	जइ (STRCMP(p, "*") == 0)
	{
	    p = gui_mch_font_dialog(args->os_oldval.string);
	    free_string_option(p_guifont);
	    p_guifont = (p != रिक्त) ? p : vim_strsave(args->os_oldval.string);
	}
# endif
	जइ (p != रिक्त && gui_init_font(p_guifont, FALSE) != OK)
	{
# जइ defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_PHOTON)
	    जइ (STRCMP(p_guifont, "*") == 0)
	    {
		// Dialog was cancelled: Keep the old value without giving
		// an error message.
		free_string_option(p_guifont);
		p_guifont = vim_strsave(args->os_oldval.string);
	    }
	    अण्णहा
# endif
		errmsg = e_invalid_fonts;
	}
    }

    प्रत्यागम errmsg;
}

/*
 * Expand the 'guifont' option. Only when GUI is being used. Each platform has
 * specific behaviors.
 */
    पूर्णाङ्क
expand_set_guifont(optexpand_T *args, पूर्णाङ्क *अङ्कः_Matches, char_u ***matches)
{
    जइ (!gui.in_use)
	प्रत्यागम FAIL;

# जइ defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
    char_u **varp = (char_u **)args->oe_varp;
    पूर्णाङ्क wide = (varp == &p_guifontwide);

    प्रत्यागम expand_set_opt_callback(
	    args, gui_mch_expand_font, &wide, अङ्कः_Matches, matches);
# अण्णहा
    प्रत्यागम FAIL;
# endif
}

# जइ defined(FEAT_XFONTSET)
/*
 * The 'guifontset' option is changed.
 */
    वर्ण *
did_set_guifontset(optset_T *args UNUSED)
{
    वर्ण *errmsg = रिक्त;

    जइ (STRCMP(p_guifontset, "*") == 0)
	errmsg = e_cant_select_fontset;
    अण्णहा जइ (gui.in_use && gui_init_font(p_guifontset, TRUE) != OK)
	errmsg = e_invalid_fontset;

    प्रत्यागम errmsg;
}
# endif

/*
 * The 'guifontwide' option is changed.
 */
    वर्ण *
did_set_guifontwide(optset_T *args UNUSED)
{
    वर्ण *errmsg = रिक्त;

    जइ (STRCMP(p_guifontwide, "*") == 0)
	errmsg = e_cant_select_wide_font;
    अण्णहा जइ (gui_get_wide_font() == FAIL)
	errmsg = e_invalid_wide_font;

    प्रत्यागम errmsg;
}
#endif

#जइ defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
/*
 * The 'guiligatures' option is changed.
 */
    वर्ण *
did_set_guiligatures(optset_T *args UNUSED)
{
    gui_set_ligatures();
    प्रत्यागम रिक्त;
}
#endif

#जइ defined(FEAT_GUI)
/*
 * The 'guioptions' option is changed.
 */
    वर्ण *
did_set_guioptions(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    वर्ण *errmsg;

    errmsg = did_set_option_listflag(*varp, (char_u *)GO_ALL, args->os_errbuf,
		    args->os_errbuflen);
    जइ (errmsg != रिक्त)
	प्रत्यागम errmsg;

    gui_init_which_components(args->os_oldval.string);
    प्रत्यागम रिक्त;
}

    पूर्णाङ्क
expand_set_guioptions(optexpand_T *args, पूर्णाङ्क *अङ्कः_Matches, char_u ***matches)
{
    प्रत्यागम expand_set_opt_listflag(args, (char_u*)GO_ALL, अङ्कः_Matches, matches);
}
#endif

#जइ defined(FEAT_GUI_TABLINE)
/*
 * The 'guitablabel' option is changed.
 */
    वर्ण *
did_set_guitablabel(optset_T *args UNUSED)
{
    redraw_tabline = TRUE;
    प्रत्यागम रिक्त;
}
#endif

/*
 * The 'helpfile' option is changed.
 */
    वर्ण *
did_set_helpfile(optset_T *args UNUSED)
{
    // May compute new values for $VIM and $VIMRUNTIME
    जइ (didset_vim)
	vim_unsetenv_ext((char_u *)"VIM");
    जइ (didset_vimruntime)
	vim_unsetenv_ext((char_u *)"VIMRUNTIME");
    प्रत्यागम रिक्त;
}

#जइ defined(FEAT_MULTI_LANG)
/*
 * The 'helplang' option is changed.
 */
    वर्ण *
did_set_helplang(optset_T *args UNUSED)
{
    वर्ण *errmsg = रिक्त;

    // Check for "", "ab", "ab,cd", etc.
    प्रति (char_u *s = p_hlg; *s != NUL; s += 3)
    {
	जइ (s[1] == NUL || ((s[2] != ',' || s[3] == NUL) && s[2] != NUL))
	{
	    errmsg = e_invalid_argument;
	    भङ्ग;
	}
	जइ (s[2] == NUL)
	    भङ्ग;
    }

    प्रत्यागम errmsg;
}
#endif

/*
 * The 'highlight' option is changed.
 */
    वर्ण *
did_set_highlight(optset_T *args UNUSED)
{
    जइ (highlight_changed() == FAIL)
	प्रत्यागम e_invalid_argument;	// invalid flags

    प्रत्यागम रिक्त;
}

    स्थिर पूर्णाङ्क
expand_hl_occasions(
	optexpand_T *args,
	पूर्णाङ्क *अङ्कः_Matches,
	char_u ***matches,
	char_u prefix)
{
    char_u	    *p;
    स्थिर char_u   hl_flags[HLF_गणनम्] = HL_FLAGS;
    size_t	    i;
    पूर्णाङ्क		    गणनम् = 0;

    *matches = ALLOC_MULT(char_u *, HLF_गणनम् + 1);
    जइ (*matches == रिक्त)
	प्रत्यागम FAIL;

    // We still want to return the full option if it's requested.
    जइ (args->oe_include_orig_val)
    {
	p = vim_strsave(args->oe_opt_मानम्);
	जइ (p == रिक्त)
	{
	    VIM_CLEAR(*matches);
	    प्रत्यागम FAIL;
	}
	(*matches)[गणनम्++] = p;
    }

    प्रति (i = 0; i < HLF_गणनम्; i++)
    {
	p = alloc((prefix == NUL ? 1 : 2) + 1);
	जइ (p == रिक्त)
	{
	    जइ (गणनम् == 0)
	    {
		VIM_CLEAR(*matches);
		प्रत्यागम FAIL;
	    }
	    अण्णहा
		भङ्ग;
	}
	जइ (prefix == NUL)
	    sprintf((वर्ण *)p, "%c", hl_flags[i]);
	अण्णहा
	    sprintf((वर्ण *)p, "%c%c", prefix, hl_flags[i]);
	(*matches)[गणनम्++] = p;
    }

    जइ (गणनम् == 0)
    {
	VIM_CLEAR(*matches);
	प्रत्यागम FAIL;
    }
    *अङ्कः_Matches = गणनम्;
    प्रत्यागम OK;
}

/*
 * Expand 'highlight' option.
 */
    पूर्णाङ्क
expand_set_highlight(optexpand_T *args, पूर्णाङ्क *अङ्कः_Matches, char_u ***matches)
{
    char_u	    *p;
    expand_T	    *xp = args->oe_xp;
    size_t	    i;
    पूर्णाङ्क		    गणनम् = 0;

    जइ (xp->xp_pattern > args->oe_set_arg && *(xp->xp_pattern-1) == ':')
    {
	// Right after a ':', meaning we just return all highlight names.
	प्रत्यागम expand_set_opt_generic(
		args,
		get_highlight_name,
		अङ्कः_Matches,
		matches);
    }

    जइ (*xp->xp_pattern == NUL)
	// At beginning of a comma-separated list. Return the specific list of
	// supported occasions.
	प्रत्यागम expand_hl_occasions(args, अङ्कः_Matches, matches, NUL);

    // We are after the initial character (which indicates the occasion). We
    // already made sure we are not matching after a ':' above, so now we want
    // to match against display mode modifiers.
    // Since the xp_pattern starts from the beginning, we need to include it in
    // the returned match.

    // Note: Keep this in sync with highlight_changed()
    स्थिर char_u p_hl_mode_मानानि[] =
	{':', 'b', 'i', '-', 'n', 'r', 's', 'u', 'c', '2', 'd', '=', 't'};
    size_t अङ्कः_hl_modes = ARRAY_LENGTH(p_hl_mode_मानानि);

    *matches = ALLOC_MULT(char_u *, अङ्कः_hl_modes);
    जइ (*matches == रिक्त)
	प्रत्यागम FAIL;

    पूर्णाङ्क pattern_len = xp->xp_pattern_len;

    प्रति (i = 0; i < अङ्कः_hl_modes; i++)
    {
	// Don't allow duplicates as these are unique flags
	char_u *dup = vim_strchr(xp->xp_pattern + 1, p_hl_mode_मानानि[i]);
	जइ (dup != रिक्त && (पूर्णाङ्क)(dup - xp->xp_pattern) < pattern_len)
	    अनुवर्त;

	// ':' only works by itself, not with other flags.
	जइ (pattern_len > 1 && p_hl_mode_मानानि[i] == ':')
	    अनुवर्त;

	p = vim_strnsave(xp->xp_pattern, pattern_len + 1);
	जइ (p == रिक्त)
	{
	    जइ (i == 0)
	    {
		VIM_CLEAR(*matches);
		प्रत्यागम FAIL;
	    }
	    अण्णहा
		भङ्ग;
	}
	p[pattern_len] = p_hl_mode_मानानि[i];
	p[pattern_len + 1] = NUL;
	(*matches)[गणनम्++] = p;
    }
    जइ (गणनम् == 0)
    {
	VIM_CLEAR(*matches);
	प्रत्यागम FAIL;
    }
    *अङ्कः_Matches = गणनम्;

    प्रत्यागम OK;
}

/*
 * The 'titlestring' or the 'iconstring' option is changed.
 */
    स्थिर वर्ण *
parse_titleiconstring(optset_T *args UNUSED, पूर्णाङ्क flagval UNUSED)
{
#ifdef FEAT_STL_OPT
    char_u	**varp = (char_u **)args->os_varp;

    // NULL => statusline syntax
    जइ (vim_strchr(*varp, '%') && check_stl_option(*varp) == रिक्त)
	stl_syntax |= flagval;
    अण्णहा
	stl_syntax &= ~flagval;
#endif
    did_set_title();

    प्रत्यागम रिक्त;
}

/*
 * The 'iconstring' option is changed.
 */
    वर्ण *
did_set_iconstring(optset_T *args)
{
    पूर्णाङ्क flagval = 0;

#ifdef FEAT_STL_OPT
    flagval = STL_IN_ICON;
#endif

    प्रत्यागम parse_titleiconstring(args, flagval);
}

#जइ defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
/*
 * The 'imactivatekey' option is changed.
 */
    वर्ण *
did_set_imactivatekey(optset_T *args UNUSED)
{
    जइ (!im_xim_isvalid_imactivate())
	प्रत्यागम e_invalid_argument;
    प्रत्यागम रिक्त;
}
#endif

#ifdef FEAT_IMAGE
/*
 * The 'imageprotocol' option is changed.
 */
    वर्ण *
did_set_imageprotocol(optset_T *args UNUSED)
{
    जइ (update_image_backend() == FAIL)
	प्रत्यागम e_invalid_argument;
    प्रत्यागम रिक्त;
}

    पूर्णाङ्क
expand_set_imageprotocol(optexpand_T *args, पूर्णाङ्क *अङ्कः_Matches, char_u ***matches)
{
    expand_T *xp = args->oe_xp;

    जइ (xp->xp_pattern > args->oe_set_arg && *(xp->xp_pattern-1) == ':')
    {
	// 'imageprotocol' only has well-defined terms for completion for the
	// protocol part after the colon.
	प्रत्यागम expand_set_opt_string(
		args,
		p_ipc_protocol_मानानि,
		ARRAY_LENGTH(p_ipc_protocol_मानानि) - 1,
		अङ्कः_Matches,
		matches);
    }
    // Use expand_set_opt_string instead of returning FAIL so that we can
    // include the original value if args->oe_include_orig_val is set.
    स्थिर वर्ण *(empty[]) = {रिक्त};
    प्रत्यागम expand_set_opt_string(args, empty, 0, अङ्कः_Matches, matches);
}
#endif

/*
 * The 'iskeyword' option is changed.
 */
    वर्ण *
did_set_iskeyword(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    जइ (varp == &p_isk)		// only check for global-value
    {
	जइ (check_isopt(*varp) == FAIL)
	    प्रत्यागम e_invalid_argument;
    }
    अण्णहा			// fallthrough for local-value
	प्रत्यागम did_set_isopt(args);

    प्रत्यागम रिक्त;
}

/*
 * The 'isident' or the 'iskeyword' or the 'isprint' or the 'isfname' option is
 * changed.
 */
    वर्ण *
did_set_isopt(optset_T *args)
{
    // 'isident', 'iskeyword', 'isprint' or 'isfname' option: refill g_chartab[]
    // If the new option is invalid, use old value.
    // 'lisp' option: refill g_chartab[] for '-' char.
    जइ (init_chartab() == FAIL)
    {
	args->os_restore_chartab = TRUE;// need to restore the chartab.
	प्रत्यागम e_invalid_argument;	// error in value
    }

    प्रत्यागम रिक्त;
}

/*
 * The 'jumpoptions' option is changed.
 */
    वर्ण *
did_set_jumpoptions(optset_T *args UNUSED)
{
    जइ (opt_strings_flags(p_jop, p_jop_मानानि, &jop_flags, TRUE) != OK)
	प्रत्यागम e_invalid_argument;

    प्रत्यागम रिक्त;
}

    पूर्णाङ्क
expand_set_jumpoptions(optexpand_T *args, पूर्णाङ्क *अङ्कः_Matches, char_u ***matches)
{
    प्रत्यागम expand_set_opt_string(
	    args,
	    p_jop_मानानि,
	    ARRAY_LENGTH(p_jop_मानानि) - 1,
	    अङ्कः_Matches,
	    matches);
}

#जइ defined(FEAT_KEYMAP)
/*
 * The 'keymap' option is changed.
 */
    वर्ण *
did_set_keymap(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    वर्ण *errmsg = रिक्त;

    जइ (!valid_filetype(*varp))
	errmsg = e_invalid_argument;
    अण्णहा
    {
	पूर्णाङ्क	    secure_save = secure;

	// Reset the secure flag, since the value of 'keymap' has
	// been checked to be safe.
	secure = 0;

	// load or unload key mapping tables
	errmsg = keymap_init();

	secure = secure_save;

	// Since we check the value, there is no need to set P_INSECURE,
	// even when the value comes from a modeline.
	args->os_मानम्_checked = TRUE;
    }

    जइ (errmsg == रिक्त)
    {
	जइ (*curbuf->b_p_keymap != NUL)
	{
	    // Installed a new keymap, switch on using it.
	    curbuf->b_p_iminsert = B_IMODE_LMAP;
	    जइ (curbuf->b_p_imsearch != B_IMODE_USE_निवेशय)
		curbuf->b_p_imsearch = B_IMODE_LMAP;
	}
	अण्णहा
	{
	    // Cleared the keymap, may reset 'iminsert' and 'imsearch'.
	    जइ (curbuf->b_p_iminsert == B_IMODE_LMAP)
		curbuf->b_p_iminsert = B_IMODE_NONE;
	    जइ (curbuf->b_p_imsearch == B_IMODE_LMAP)
		curbuf->b_p_imsearch = B_IMODE_USE_निवेशय;
	}
	जइ ((args->os_flags & OPT_LOCAL) == 0)
	{
	    set_iminsert_global();
	    set_imsearch_global();
	}
	status_redraw_curbuf();
    }

    प्रत्यागम errmsg;
}
#endif

/*
 * The 'keymodel' option is changed.
 */
    वर्ण *
did_set_keymodel(optset_T *args UNUSED)
{
    जइ (check_opt_strings(p_km, p_km_मानानि, TRUE) != OK)
	प्रत्यागम e_invalid_argument;

    km_stopsel = (vim_strchr(p_km, 'o') != रिक्त);
    km_startsel = (vim_strchr(p_km, 'a') != रिक्त);
    प्रत्यागम रिक्त;
}

    पूर्णाङ्क
expand_set_keymodel(optexpand_T *args, पूर्णाङ्क *अङ्कः_Matches, char_u ***matches)
{
    प्रत्यागम expand_set_opt_string(
	    args,
	    p_km_मानानि,
	    ARRAY_LENGTH(p_km_मानानि) - 1,
	    अङ्कः_Matches,
	    matches);
}

/*
 * The 'keyprotocol' option is changed.
 */
    वर्ण *
did_set_keyprotocol(optset_T *args UNUSED)
{
    char_u *term = T_NAME;
    keyprot_T kpc = match_keyprotocol(term);
    जइ (kpc == KEYPROTOCOL_FAIL)
	प्रत्यागम e_invalid_argument;

    apply_keyprotocol(term, kpc);

    प्रत्यागम रिक्त;
}

    पूर्णाङ्क
expand_set_keyprotocol(optexpand_T *args, पूर्णाङ्क *अङ्कः_Matches, char_u ***matches)
{
    expand_T *xp = args->oe_xp;
    जइ (xp->xp_pattern > args->oe_set_arg && *(xp->xp_pattern-1) == ':')
    {
	// 'keyprotocol' only has well-defined terms for completion for the
	// protocol part after the colon.
	प्रत्यागम expand_set_opt_string(
		args,
		p_kpc_protocol_मानानि,
		ARRAY_LENGTH(p_kpc_protocol_मानानि) - 1,
		अङ्कः_Matches,
		matches);
    }
    // Use expand_set_opt_string instead of returning FAIL so that we can
    // include the original value if args->oe_include_orig_val is set.
    स्थिर वर्ण *(empty[]) = {रिक्त};
    प्रत्यागम expand_set_opt_string(args, empty, 0, अङ्कः_Matches, matches);
}

/*
 * The 'lispoptions' option is changed.
 */
    वर्ण *
did_set_lispoptions(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    जइ (**varp != NUL
	    && STRCMP(*varp, "expr:0") != 0
	    && STRCMP(*varp, "expr:1") != 0)
	प्रत्यागम e_invalid_argument;

    प्रत्यागम रिक्त;
}

    पूर्णाङ्क
expand_set_lispoptions(optexpand_T *args, पूर्णाङ्क *अङ्कः_Matches, char_u ***matches)
{
    स्थिर वर्ण *(p_lop_मानानि[]) = {"expr:0", "expr:1", रिक्त};
    प्रत्यागम expand_set_opt_string(
	    args,
	    p_lop_मानानि,
	    ARRAY_LENGTH(p_lop_मानानि) - 1,
	    अङ्कः_Matches,
	    matches);
}

/*
 * The 'matchpairs' option is changed.
 */
    वर्ण *
did_set_matchpairs(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u	*p;

    जइ (has_mbyte)
    {
	प्रति (p = *varp; *p != NUL; ++p)
	{
	    पूर्णाङ्क x2 = -1;
	    पूर्णाङ्क x3 = -1;

	    p += mb_ptr2len(p);
	    जइ (*p != NUL)
		x2 = *p++;
	    जइ (*p != NUL)
	    {
		x3 = mb_ptr2char(p);
		p += mb_ptr2len(p);
	    }
	    जइ (x2 != ':' || x3 == -1 || (*p != NUL && *p != ','))
		प्रत्यागम e_invalid_argument;
	    जइ (*p == NUL)
		भङ्ग;
	}
    }
    अण्णहा
    {
	// Check for "x:y,x:y"
	प्रति (p = *varp; *p != NUL; p += 4)
	{
	    जइ (p[1] != ':' || p[2] == NUL || (p[3] != NUL && p[3] != ','))
		प्रत्यागम e_invalid_argument;
	    जइ (p[3] == NUL)
		भङ्ग;
	}
    }

    प्रत्यागम रिक्त;
}

/*
 * Process the updated 'messagesopt' option value.
 */
    वर्ण *
did_set_messagesopt(optset_T *args UNUSED)
{
    जइ (messagesopt_changed() == FAIL)
	प्रत्यागम e_invalid_argument;

    प्रत्यागम रिक्त;
}

    पूर्णाङ्क
expand_set_messagesopt(optexpand_T *args, पूर्णाङ्क *अङ्कः_Matches, char_u ***matches)
{
    स्थिर वर्ण *(p_mopt_मानानि[]) = {"hit-enter", "wait:", "history:", रिक्त};
    प्रत्यागम expand_set_opt_string(
	    args,
	    p_mopt_मानानि,
	    ARRAY_LENGTH(p_mopt_मानानि) - 1,
	    अङ्कः_Matches,
	    matches);
}

#जइ defined(FEAT_SPELL)
/*
 * The 'mkspellmem' option is changed.
 */
    वर्ण *
did_set_mkspellmem(optset_T *args UNUSED)
{
    जइ (spell_check_msm() != OK)
	प्रत्यागम e_invalid_argument;

    प्रत्यागम रिक्त;
}
#endif

/*
 * The 'mouse' option is changed.
 */
    वर्ण *
did_set_mouse(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    वर्ण	*retval;

    retval = did_set_option_listflag(*varp, (char_u *)MOUSE_ALL, args->os_errbuf,
		    args->os_errbuflen);
    जइ (retval == रिक्त)
    {
	redraw_tabline = TRUE;
	जइ (tabline_height() > 0)
	    update_screen(UPD_VALID);
#जइ defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
	popup_close_info(); // Close info popup to apply new properties
#endif
    }
    प्रत्यागम retval;
}

    पूर्णाङ्क
expand_set_mouse(optexpand_T *args, पूर्णाङ्क *अङ्कः_Matches, char_u ***matches)
{
    प्रत्यागम expand_set_opt_listflag(args, (char_u*)MOUSE_ALL, अङ्कः_Matches, matches);
}

/*
 * The 'mousemodel' option is changed.
 */
    वर्ण *
did_set_mousemodel(optset_T *args UNUSED)
{
    जइ (check_opt_strings(p_mousem, p_mousem_मानानि, FALSE) != OK)
	प्रत्यागम e_invalid_argument;
#जइ defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU) && (XmVersion <= 1002)
    अण्णहा जइ (*p_mousem != *args->os_oldval.string)
	// Changed from "extend" to "popup" or "popup_setpos" or vv: need
	// to create or delete the popup menus.
	gui_motif_update_mousemodel(मूलम्_menu);
#endif

    प्रत्यागम रिक्त;
}

    पूर्णाङ्क
expand_set_mousemodel(optexpand_T *args, पूर्णाङ्क *अङ्कः_Matches, char_u ***matches)
{
    प्रत्यागम expand_set_opt_string(
	    args,
	    p_mousem_मानानि,
	    ARRAY_LENGTH(p_mousem_मानानि) - 1,
	    अङ्कः_Matches,
	    matches);
}

#जइ defined(FEAT_MOUSESHAPE)
    वर्ण *
did_set_mouseshape(optset_T *args UNUSED)
{
    वर्ण *errmsg = रिक्त;

    errmsg = parse_shape_opt(SHAPE_MOUSE);
    update_mouseshape(-1);

    प्रत्यागम errmsg;
}
#endif

/*
 * The 'nrformats' option is changed.
 */
    वर्ण *
did_set_nrformats(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    प्रत्यागम did_set_opt_strings(*varp, p_nf_मानानि, TRUE);
}

    पूर्णाङ्क
expand_set_nrformats(optexpand_T *args, पूर्णाङ्क *अङ्कः_Matches, char_u ***matches)
{
    प्रत्यागम expand_set_opt_string(
	    args,
	    p_nf_मानानि,
	    ARRAY_LENGTH(p_nf_मानानि) - 1,
	    अङ्कः_Matches,
	    matches);
}

#जइ defined(FEAT_EVAL)
/*
 * One of the '*expr' options is changed: 'balloonexpr', 'diffexpr',
 * 'foldexpr', 'foldtext', 'formatexpr', 'includeexpr', 'indentexpr',
 * 'patchexpr', 'printexpr' and 'charconvert'.
 */
    वर्ण *
did_set_optexpr(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    // If the option value starts with <SID> or s:, then replace that with
    // the script identifier.
    char_u *name = get_scriptlocal_funcname(*varp);
    जइ (name != रिक्त)
    {
	free_string_option(*varp);
	*varp = name;
    }

    प्रत्यागम रिक्त;
}
#endif

/*
 * The 'pastetoggle' option is changed.
 */
    वर्ण *
did_set_pastetoggle(optset_T *args UNUSED)
{
    char_u	*p;

    // translate key codes like in a mapping
    जइ (*p_pt)
    {
	(शून्य)replace_termcodes(p_pt, &p, 0,
		REPTERM_FROM_PART | REPTERM_DO_LT, रिक्त);
	जइ (p != रिक्त)
	{
	    free_string_option(p_pt);
	    p_pt = p;
	}
    }

    प्रत्यागम रिक्त;
}

#जइ defined(FEAT_PROP_POPUP)
/*
 * The 'previewpopup' option is changed.
 */
    वर्ण *
did_set_previewpopup(optset_T *args UNUSED)
{
    जइ (parse_previewpopup(रिक्त) == FAIL)
	प्रत्यागम e_invalid_argument;

# जइ defined(FEAT_QUICKFIX)
    popup_close_info();
# endif
    प्रत्यागम रिक्त;
}

    स्थिर पूर्णाङ्क
expand_set_popupoption(optexpand_T *args, पूर्णाङ्क *अङ्कः_Matches, char_u ***matches,
	पूर्णाङ्क previewpopup)
{
    expand_T *xp = args->oe_xp;

    जइ (xp->xp_pattern > args->oe_set_arg && *(xp->xp_pattern-1) == ':')
    {
	जइ (completing_मानम्_for_subopt(args, "close")
		|| completing_मानम्_for_subopt(args, "resize")
		|| completing_मानम्_for_subopt(args, "shadow"))
	{
	    प्रत्यागम expand_set_opt_string(
		    args,
		    p_popup_option_on_off_मानानि,
		    ARRAY_LENGTH(p_popup_option_on_off_मानानि) - 1,
		    अङ्कः_Matches,
		    matches);
	}
	जइ (completing_मानम्_for_subopt(args, "border"))
	{
	    प्रत्यागम expand_set_opt_string(
		    args,
		    previewpopup ? p_popup_pvp_border_मानानि
			: p_popup_cpp_border_मानानि,
		    (previewpopup ? ARRAY_LENGTH(p_popup_pvp_border_मानानि) - 1
			: ARRAY_LENGTH(p_popup_cpp_border_मानानि)) - 1,
		    अङ्कः_Matches,
		    matches);
	}
	जइ (completing_मानम्_for_subopt(args, "align"))
	{
	    प्रत्यागम expand_set_opt_string(
		    args,
		    p_popup_option_align_मानानि,
		    ARRAY_LENGTH(p_popup_option_align_मानानि) - 1,
		    अङ्कः_Matches,
		    matches);
	}
	जइ (completing_मानम्_for_subopt(args, "highlight")
		|| completing_मानम्_for_subopt(args, "borderhighlight"))
	{
	    // Return the list of all highlight names
	    प्रत्यागम expand_set_opt_generic(
		    args,
		    get_highlight_name,
		    अङ्कः_Matches,
		    matches);
	}
	प्रत्यागम FAIL;
    }

    प्रत्यागम expand_set_opt_string(
	    args,
	    previewpopup ? p_popup_pvp_option_मानानि
		: p_popup_cpp_option_मानानि,
	    previewpopup ? ARRAY_LENGTH(p_popup_pvp_option_मानानि) - 1
		: ARRAY_LENGTH(p_popup_cpp_option_मानानि) - 1,
	    अङ्कः_Matches,
	    matches);
}

    पूर्णाङ्क
expand_set_previewpopup(optexpand_T *args, पूर्णाङ्क *अङ्कः_Matches, char_u ***matches)
{
    प्रत्यागम expand_set_popupoption(args, अङ्कः_Matches, matches, TRUE);
}

    पूर्णाङ्क
expand_set_completepopup(optexpand_T *args, पूर्णाङ्क *अङ्कः_Matches, char_u ***matches)
{
    प्रत्यागम expand_set_popupoption(args, अङ्कः_Matches, matches, FALSE);
}
#endif

#जइ defined(FEAT_POSTSCRIPT)
/*
 * The 'printencoding' option is changed.
 */
    वर्ण *
did_set_printencoding(optset_T *args UNUSED)
{
    char_u	*s, *p;

    // Canonize 'printencoding' if VIM standard one
    p = enc_canonize(p_penc);
    जइ (p != रिक्त)
    {
	vim_free(p_penc);
	p_penc = p;
    }
    अण्णहा
    {
	// Ensure lower case and '-' for '_'
	प्रति (s = p_penc; *s != NUL; s++)
	{
	    जइ (*s == '_')
		*s = '-';
	    अण्णहा
		*s = TOLOWER_ASC(*s);
	}
    }

    प्रत्यागम रिक्त;
}
#endif

#जइ defined(FEAT_PRINTER)

    स्थिर char_u *
get_printoptions_names(expand_T *xp UNUSED, पूर्णाङ्क idx)
{
    जइ (idx >= (पूर्णाङ्क)ARRAY_LENGTH(printer_opts))
	प्रत्यागम रिक्त;
    प्रत्यागम (char_u*)printer_opts[idx].name;
}

/*
 * Expand 'printoptions' option
 */
    पूर्णाङ्क
expand_set_printoptions(optexpand_T *args, पूर्णाङ्क *अङ्कः_Matches, char_u ***matches)
{
    प्रत्यागम expand_set_opt_generic(
	    args,
	    get_printoptions_names,
	    अङ्कः_Matches,
	    matches);
}
#endif

#जइ defined(FEAT_STL_OPT)
/*
 * The 'statusline' or the 'tabline' or the 'rulerformat' option is changed.
 * "rulerformat" is TRUE if the 'rulerformat' option is changed.
 */
    स्थिर वर्ण *
parse_statustabline_rulerformat(optset_T *args, पूर्णाङ्क rulerformat)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u	*s;
    वर्ण	*errmsg = रिक्त;
    पूर्णाङ्क		wid;

    जइ (rulerformat)	// reset ru_wid first
	ru_wid = 0;
    s = *varp;
    जइ (rulerformat && *s == '%')
    {
	// set ru_wid if 'ruf' starts with "%99("
	जइ (*++s == '-')	// ignore a '-'
	    s++;
	wid = getdigits(&s);
	जइ (wid && *s == '(' && (errmsg = check_stl_option(p_ruf)) == रिक्त)
	    ru_wid = wid;
	अण्णहा
	{
	    // Validate the flags in 'rulerformat' only if it doesn't point to
	    // a custom function ("%!" flag).
	    जइ ((*varp)[1] != '!')
		errmsg = check_stl_option(p_ruf);
	}
    }
    // check 'statusline' or 'tabline' only if it doesn't start with "%!"
    अण्णहा जइ (rulerformat || s[0] != '%' || s[1] != '!')
	errmsg = check_stl_option(s);
    जइ (rulerformat && errmsg == रिक्त)
	comp_col();

    प्रत्यागम errmsg;
}
#endif

#जइ defined(FEAT_RENDER_OPTIONS)
/*
 * The 'renderoptions' option is changed.
 */
    वर्ण *
did_set_renderoptions(optset_T *args UNUSED)
{
    जइ (!gui_mch_set_rendering_options(p_rop))
	प्रत्यागम e_invalid_argument;

    प्रत्यागम रिक्त;
}
#endif

#जइ defined(FEAT_RIGHTLEFT)
/*
 * The 'rightleftcmd' option is changed.
 */
    वर्ण *
did_set_rightleftcmd(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    // Currently only "search" is a supported value.
    जइ (**varp != NUL && STRCMP(*varp, "search") != 0)
	प्रत्यागम e_invalid_argument;

    प्रत्यागम रिक्त;
}

    पूर्णाङ्क
expand_set_rightleftcmd(optexpand_T *args, पूर्णाङ्क *अङ्कः_Matches, char_u ***matches)
{
    स्थिर वर्ण *(p_rlc_मानानि[]) = {"search", रिक्त};
    प्रत्यागम expand_set_opt_string(
	    args,
	    p_rlc_मानानि,
	    ARRAY_LENGTH(p_rlc_मानानि) - 1,
	    अङ्कः_Matches,
	    matches);
}
#endif

#define PUM_BORDER_CLEAR()   \
    करेहि {                     \
	pum_set_border(FALSE); \
	pum_set_shadow(FALSE); \
	pum_set_margin(FALSE); \
    } जाव (0)

/*
 * Parse a border value from a pumopt "border:" token.
 * Returns OK on success, FAIL on error.
 */
    स्थिर पूर्णाङ्क
parse_pumopt_border(char_u *val, पूर्णाङ्क len)
{
    // Use box-drawing characters only when 'encoding' is "utf-8" and
    // 'ambiwidth' is "single".
    पूर्णाङ्क	    can_use_box_chars = (enc_utf8 && *p_ambw == 's');
    char_u  *token;

    token = vim_strnsave(val, len);
    जइ (token == रिक्त)
	प्रत्यागम FAIL;

    जइ (can_use_box_chars && STRCMP(token, "single") == 0)
	pum_set_border_chars(0x2500, 0x2502, 0x2500, 0x2502, // ─ │ ─ │
		0x250c, 0x2510, 0x2518, 0x2514); // ┌ ┐ ┘ └
    अण्णहा जइ (can_use_box_chars && STRCMP(token, "double") == 0)
	pum_set_border_chars(0x2550, 0x2551, 0x2550, 0x2551, // ═ ║ ═ ║
		0x2554, 0x2557, 0x255D, 0x255A); // ╔ ╗ ╝  ╚
    अण्णहा जइ (can_use_box_chars && STRCMP(token, "round") == 0)
	pum_set_border_chars(0x2500, 0x2502, 0x2500, 0x2502, // ─ │ ─ │
		0x256d, 0x256e, 0x256f, 0x2570); // ╭ ╮ ╯ ╰
    अण्णहा जइ (STRCMP(token, "ascii") == 0)
	pum_set_border_chars('-', '|', '-', '|', '+', '+', '+', '+');
    अण्णहा जइ (STRNCMP(token, "custom:", 7) == 0)
    {
	char_u	*q = token + 7;
	पूर्णाङ्क	out[8];

	प्रति (पूर्णाङ्क i = 0; i < 8; i++)
	{
	    जइ (*q == NUL || *q == ',')
	    {
		vim_free(token);
		प्रत्यागम FAIL;
	    }
	    out[i] = mb_ptr2char(q);
	    mb_ptr2char_adv(&q);
	    जइ (i < 7)
	    {
		जइ (*q != ';')
		{
		    vim_free(token);
		    प्रत्यागम FAIL;
		}
		q++;
	    }
	}
	जइ (*q != NUL && *q != ',')
	{
	    vim_free(token);
	    प्रत्यागम FAIL;
	}
	pum_set_border_chars(out[0], out[1], out[2], out[3], out[4], out[5],
		out[6], out[7]);
    }
    अण्णहा
    {
	vim_free(token);
	प्रत्यागम FAIL;
    }

    vim_free(token);
    pum_set_border(TRUE);
    प्रत्यागम OK;
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
    वर्ण *
did_set_pumopt(optset_T *args)
{
    char_u  **varp = (char_u **)args->os_varp;
    char_u  *p;
    पूर्णाङ्क	    have_border = FALSE;
    पूर्णाङ्क	    have_margin = FALSE;

    // Reset to defaults.
    PUM_BORDER_CLEAR();
    p_ph = 0;
    p_pw = 15;
    p_pmw = 0;
    p_po = 100;

    जइ (*varp == रिक्त || **varp == NUL)
	प्रत्यागम रिक्त;

    प्रति (p = *varp; p != रिक्त && *p != NUL; )
    {
	char_u *comma = vim_strchr(p, ',');
	पूर्णाङ्क len;

	जइ (comma != रिक्त)
	    len = (पूर्णाङ्क)(comma - p);
	अण्णहा
	    len = (पूर्णाङ्क)STRLEN(p);

	जइ (STRNCMP(p, "border:", 7) == 0)
	{
	    जइ (have_border)
		गच्छ error;
	    have_border = TRUE;
	    जइ (parse_pumopt_border(p + 7, len - 7) == FAIL)
		गच्छ error;
	}
	अण्णहा जइ (STRNCMP(p, "height:", 7) == 0)
	{
	    long n = atol((वर्ण *)p + 7);
	    जइ (n < 0)
		गच्छ error;
	    p_ph = n;
	}
	अण्णहा जइ (STRNCMP(p, "width:", 6) == 0)
	{
	    long n = atol((वर्ण *)p + 6);
	    जइ (n < 0)
		गच्छ error;
	    p_pw = n;
	}
	अण्णहा जइ (STRNCMP(p, "maxwidth:", 9) == 0)
	{
	    long n = atol((वर्ण *)p + 9);
	    जइ (n < 0)
		गच्छ error;
	    p_pmw = n;
	}
	अण्णहा जइ (STRNCMP(p, "opacity:", 8) == 0)
	{
	    long n = atol((वर्ण *)p + 8);
	    जइ (n < 0 || n > 100)
		गच्छ error;
	    p_po = n;
	}
	अण्णहा जइ (len == 6 && STRNCMP(p, "shadow", 6) == 0)
	    pum_set_shadow(TRUE);
	अण्णहा जइ (len == 6 && STRNCMP(p, "margin", 6) == 0)
	{
	    have_margin = TRUE;
	    pum_set_margin(TRUE);
	}
	अण्णहा
	    गच्छ error;

	जइ (comma != रिक्त)
	    p = comma + 1;
	अण्णहा
	    भङ्ग;
    }

    जइ (have_margin && !have_border)
	गच्छ error;

    // Invalidate cached background for opacity changes.
    pum_opacity_changed();

    प्रत्यागम रिक्त;

error:
    PUM_BORDER_CLEAR();
    p_ph = 0;
    p_pw = 15;
    p_pmw = 0;
    p_po = 100;
    प्रत्यागम e_invalid_argument;
}

    स्थिर char_u *
get_pum_border_style(expand_T *xp UNUSED, पूर्णाङ्क idx)
{
    स्थिर वर्ण *styles[] = {"ascii", "custom:", "single", "double", "round"};
    प्रत्यागम idx < ((enc_utf8 && *p_ambw == 's') ? (पूर्णाङ्क)ARRAY_LENGTH(styles) : 2)
	    ? (char_u *)styles[idx] : रिक्त;
}

    पूर्णाङ्क
expand_set_pumopt(optexpand_T *args, पूर्णाङ्क *अङ्कः_Matches, char_u ***matches)
{
    expand_T *xp = args->oe_xp;

    जइ (xp->xp_pattern > args->oe_set_arg && *(xp->xp_pattern-1) == ':')
    {
	जइ (completing_मानम्_for_subopt(args, "border"))
	{
	    प्रत्यागम expand_set_opt_generic(
		    args, get_pum_border_style, अङ्कः_Matches, matches);
	}
	प्रत्यागम FAIL;
    }

    स्थिर वर्ण *(p_pumopt_मानानि[]) = {"border:", "height:", "width:",
	"maxwidth:", "opacity:", "shadow", "margin", रिक्त};
    प्रत्यागम expand_set_opt_string(
	    args,
	    p_pumopt_मानानि,
	    ARRAY_LENGTH(p_pumopt_मानानि) - 1,
	    अङ्कः_Matches,
	    matches);
}

/*
 * The 'pumborder' option is changed.
 * Rules:
 *   - One of { single, double, round, ascii, custom:XXXXXXXX } may appear.
 *   - "margin" may appear, but only together with exactly one border style.
 *   - "shadow" is independent and can be combined freely.
 */
    वर्ण *
did_set_pumborder(optset_T *args)
{
    char_u  **varp = (char_u **)args->os_varp;
    char_u  *p;
    पूर्णाङ्क	    len;
    पूर्णाङ्क	    have_border = FALSE;
    पूर्णाङ्क	    have_margin = FALSE;

    PUM_BORDER_CLEAR();

    जइ (*varp == रिक्त || **varp == NUL)
	प्रत्यागम रिक्त;

    प्रति (p = *varp; p != रिक्त && *p != NUL; )
    {
	char_u *comma = vim_strchr(p, ',');
	जइ (comma != रिक्त)
	    len = (पूर्णाङ्क)(comma - p);
	अण्णहा
	    len = (पूर्णाङ्क)STRLEN(p);

	जइ (STRNCMP(p, "shadow", len) == 0 && len == 6)
	    pum_set_shadow(TRUE);
	अण्णहा जइ (STRNCMP(p, "margin", len) == 0 && len == 6)
	{
	    have_margin = TRUE;
	    pum_set_margin(TRUE);
	}
	अण्णहा
	{
	    जइ (have_border)
		गच्छ error;
	    have_border = TRUE;
	    जइ (parse_pumopt_border(p, len) == FAIL)
		गच्छ error;
	}

	जइ (comma != रिक्त)
	    p = comma + 1;
	अण्णहा
	    भङ्ग;
    }

    जइ (have_margin && !have_border)
	गच्छ error;

    प्रत्यागम रिक्त;

error:
    PUM_BORDER_CLEAR();
    pum_set_border_chars(0, 0, 0, 0, 0, 0, 0, 0);
    प्रत्यागम e_invalid_argument;
}

    स्थिर char_u *
get_pumborder_token(expand_T *xp, पूर्णाङ्क idx)
{
    प्रत्यागम idx == 0 ? (char_u *)"margin"
	 : idx == 1 ? (char_u *)"shadow"
	 : get_pum_border_style(xp, idx - 2);
}

    पूर्णाङ्क
expand_set_pumborder(optexpand_T *args, पूर्णाङ्क *अङ्कः_Matches, char_u ***matches)
{
    प्रत्यागम expand_set_opt_generic(
	    args, get_pumborder_token, अङ्कः_Matches, matches);
}

#जइ defined(FEAT_STL_OPT)

/*
 * The 'rulerformat' option is changed.
 */
    वर्ण *
did_set_rulerformat(optset_T *args)
{
    प्रत्यागम parse_statustabline_rulerformat(args, TRUE);
}
#endif

#जइ defined(FEAT_TABPANEL)
/*
 * Process the new 'tabpanelopt' option value.
 */
    वर्ण *
did_set_tabpanelopt(optset_T *args UNUSED)
{
    जइ (tabpanelopt_changed() == FAIL)
	प्रत्यागम e_invalid_argument;

    प्रत्यागम रिक्त;
}

    पूर्णाङ्क
expand_set_tabpanelopt(optexpand_T *args, पूर्णाङ्क *अङ्कः_Matches, char_u ***matches)
{
    expand_T *xp = args->oe_xp;

    जइ (xp->xp_pattern > args->oe_set_arg && *(xp->xp_pattern-1) == ':')
    {
	जइ (completing_मानम्_for_subopt(args, "align"))
	{
	    प्रत्यागम expand_set_opt_string(
		    args,
		    p_tplo_align_मानानि,
		    ARRAY_LENGTH(p_tplo_align_मानानि) - 1,
		    अङ्कः_Matches,
		    matches);
	}
	प्रत्यागम FAIL;
    }

    प्रत्यागम expand_set_opt_string(
	    args,
	    p_tplo_मानानि,
	    ARRAY_LENGTH(p_tplo_मानानि) - 1,
	    अङ्कः_Matches,
	    matches);
}
#endif

/*
 * The 'scrollopt' option is changed.
 */
    वर्ण *
did_set_scrollopt(optset_T *args UNUSED)
{
    प्रत्यागम did_set_opt_strings(p_sbo, p_scbopt_मानानि, TRUE);
}

    पूर्णाङ्क
expand_set_scrollopt(optexpand_T *args, पूर्णाङ्क *अङ्कः_Matches, char_u ***matches)
{
    प्रत्यागम expand_set_opt_string(
	    args,
	    p_scbopt_मानानि,
	    ARRAY_LENGTH(p_scbopt_मानानि) - 1,
	    अङ्कः_Matches,
	    matches);
}

/*
 * The 'wlseat' option is changed
 */
    वर्ण *
did_set_wlseat(optset_T *args UNUSED)
{
#ifdef FEAT_WAYLAND_CLIPBOARD
    // If there isn't any seat named 'wlseat', then let the Wayland clipboard be
    // unavailable. Ignore errors returned.
    clip_reset_wayland();
#endif

    प्रत्यागम रिक्त;
}

/*
 * The 'selection' option is changed.
 */
    वर्ण *
did_set_selection(optset_T *args UNUSED)
{
    जइ (*p_sel == NUL || check_opt_strings(p_sel, p_sel_मानानि, FALSE) != OK)
	प्रत्यागम e_invalid_argument;

    प्रत्यागम रिक्त;
}

    पूर्णाङ्क
expand_set_selection(optexpand_T *args, पूर्णाङ्क *अङ्कः_Matches, char_u ***matches)
{
    प्रत्यागम expand_set_opt_string(
	    args,
	    p_sel_मानानि,
	    ARRAY_LENGTH(p_sel_मानानि) - 1,
	    अङ्कः_Matches,
	    matches);
}

/*
 * The 'selectmode' option is changed.
 */
    वर्ण *
did_set_selectmode(optset_T *args UNUSED)
{
    प्रत्यागम did_set_opt_strings(p_slm, p_slm_मानानि, TRUE);
}

    पूर्णाङ्क
expand_set_selectmode(optexpand_T *args, पूर्णाङ्क *अङ्कः_Matches, char_u ***matches)
{
    प्रत्यागम expand_set_opt_string(
	    args,
	    p_slm_मानानि,
	    ARRAY_LENGTH(p_slm_मानानि) - 1,
	    अङ्कः_Matches,
	    matches);
}

#जइ defined(FEAT_SESSION)
/*
 * The 'sessionoptions' option is changed.
 */
    वर्ण *
did_set_sessionoptions(optset_T *args)
{
    जइ (opt_strings_flags(p_ssop, p_ssop_मानानि, &ssop_flags, TRUE) != OK)
	प्रत्यागम e_invalid_argument;
    जइ ((ssop_flags & SSOP_CURDIR) && (ssop_flags & SSOP_SESDIR))
    {
	// Don't allow both "sesdir" and "curdir".
	(शून्य)opt_strings_flags(args->os_oldval.string, p_ssop_मानानि,
							&ssop_flags, TRUE);
	प्रत्यागम e_invalid_argument;
    }

    प्रत्यागम रिक्त;
}

    पूर्णाङ्क
expand_set_sessionoptions(
    optexpand_T		*args,
    पूर्णाङ्क			*अङ्कः_Matches,
    char_u		***matches)
{
    प्रत्यागम expand_set_opt_string(
	    args,
	    p_ssop_मानानि,
	    ARRAY_LENGTH(p_ssop_मानानि) - 1,
	    अङ्कः_Matches,
	    matches);
}
#endif

/*
 * Validate 'shellpipe'/'shellredir' option.
 */
    वर्ण *
did_set_shellpipe_redir(optset_T *args)
{
    char_u	*p;
    bool	seen = false;

    प्रति (p = args->os_newval.string; *p != NUL; ++p)
    {
	जइ (*p != '%')
	    अनुवर्त;

	जइ (p[1] == NUL)
	    प्रत्यागम e_invalid_format_string_single_percent_s;

	जइ (p[1] == '%')
	{
	    ++p;    // skip second %
	    अनुवर्त;
	}

	जइ (p[1] == 's')
	{
	    जइ (seen)
		प्रत्यागम e_invalid_format_string_single_percent_s;

	    seen = true;
	    ++p;    // consume 's'
	    अनुवर्त;
	}
	प्रत्यागम e_invalid_format_string_single_percent_s;
    }
    प्रत्यागम रिक्त;
}

/*
 * The 'shortmess' option is changed.
 */
    वर्ण *
did_set_shortmess(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    प्रत्यागम did_set_option_listflag(*varp, (char_u *)SHM_ALL, args->os_errbuf,
		    args->os_errbuflen);
}

    पूर्णाङ्क
expand_set_shortmess(optexpand_T *args, पूर्णाङ्क *अङ्कः_Matches, char_u ***matches)
{
    प्रत्यागम expand_set_opt_listflag(args, (char_u*)SHM_ALL, अङ्कः_Matches, matches);
}

#जइ defined(FEAT_LINEBREAK)
/*
 * The 'showbreak' option is changed.
 */
    वर्ण *
did_set_showbreak(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u	*s;

    प्रति (s = *varp; *s; )
    {
	जइ (ptr2cells(s) != 1)
	    प्रत्यागम e_showbreak_contains_unprintable_or_wide_character;
	MB_PTR_ADV(s);
    }

    प्रत्यागम रिक्त;
}
#endif

/*
 * The 'showcmdloc' option is changed.
 */
    वर्ण *
did_set_showcmdloc(optset_T *args UNUSED)
{
    वर्ण	*errmsg = did_set_opt_strings(p_sloc, p_sloc_मानानि, FALSE);

    जइ (errmsg == रिक्त)
	comp_col();

    प्रत्यागम errmsg;
}

    पूर्णाङ्क
expand_set_showcmdloc(optexpand_T *args, पूर्णाङ्क *अङ्कः_Matches, char_u ***matches)
{
    प्रत्यागम expand_set_opt_string(
	    args,
	    p_sloc_मानानि,
	    ARRAY_LENGTH(p_sloc_मानानि) - 1,
	    अङ्कः_Matches,
	    matches);
}

#जइ defined(FEAT_SIGNS)
/*
 * The 'signcolumn' option is changed.
 */
    वर्ण *
did_set_signcolumn(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

# जइ defined(FEAT_LINEBREAK)
    जइ (check_opt_strings(*varp, p_scl_मानानि, FALSE) != OK)
	प्रत्यागम e_invalid_argument;
    // When changing the 'signcolumn' to or from 'number', recompute the
    // width of the number column if 'number' or 'relativenumber' is set.
    जइ (((*args->os_oldval.string == 'n' && args->os_oldval.string[1] == 'u')
		|| (*curwin->w_p_scl == 'n' && *(curwin->w_p_scl + 1) =='u'))
	    && (curwin->w_p_nu || curwin->w_p_rnu))
	curwin->w_nrwidth_पङ्क्तिः_गणनम् = 0;
# endif

# जइ defined(FEAT_GUI)
    // In the GUI, when sign icons are used, a full screen clear is needed
    // to properly redraw the sign icons.
    जइ (gui.in_use && curbuf->b_signlist != रिक्त)
	redraw_all_later(UPD_CLEAR);
# endif

    प्रत्यागम रिक्त;
}

    पूर्णाङ्क
expand_set_signcolumn(optexpand_T *args, पूर्णाङ्क *अङ्कः_Matches, char_u ***matches)
{
    प्रत्यागम expand_set_opt_string(
	    args,
	    p_scl_मानानि,
	    ARRAY_LENGTH(p_scl_मानानि) - 1,
	    अङ्कः_Matches,
	    matches);
}
#endif

#जइ defined(FEAT_SPELL)
/*
 * The 'spellcapcheck' option is changed.
 */
    वर्ण *
did_set_spellcapcheck(optset_T *args UNUSED)
{
    // compile the regexp program.
    प्रत्यागम compile_cap_prog(curwin->w_s);
}

/*
 * The 'spellfile' option is changed.
 */
    वर्ण *
did_set_spellfile(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    जइ (!valid_spellfile(*varp))
	प्रत्यागम e_invalid_argument;

    // If there is a window for this buffer in which 'spell' is set load the
    // wordlists.
    प्रत्यागम did_set_spell_option();
}

/*
 * The 'spell' option is changed.
 */
    वर्ण *
did_set_spelllang(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    जइ (!valid_spelllang(*varp))
	प्रत्यागम e_invalid_argument;

    // If there is a window for this buffer in which 'spell' is set load the
    // wordlists.
    प्रत्यागम did_set_spell_option();
}

/*
 * The 'spelloptions' option is changed.
 */
    वर्ण *
did_set_spelloptions(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    जइ (**varp != NUL && STRCMP(*varp, "camel") != 0)
	प्रत्यागम e_invalid_argument;

    प्रत्यागम रिक्त;
}

    पूर्णाङ्क
expand_set_spelloptions(optexpand_T *args, पूर्णाङ्क *अङ्कः_Matches, char_u ***matches)
{
    स्थिर वर्ण *(p_spo_मानानि[]) = {"camel", रिक्त};
    प्रत्यागम expand_set_opt_string(
	    args,
	    p_spo_मानानि,
	    ARRAY_LENGTH(p_spo_मानानि) - 1,
	    अङ्कः_Matches,
	    matches);
}

/*
 * The 'spellsuggest' option is changed.
 */
    वर्ण *
did_set_spellsuggest(optset_T *args UNUSED)
{
    जइ (spell_check_sps() != OK)
	प्रत्यागम e_invalid_argument;

    प्रत्यागम रिक्त;
}

    पूर्णाङ्क
expand_set_spellsuggest(optexpand_T *args, पूर्णाङ्क *अङ्कः_Matches, char_u ***matches)
{
    प्रत्यागम expand_set_opt_string(
	    args,
	    p_sps_मानानि,
	    ARRAY_LENGTH(p_sps_मानानि) - 1,
	    अङ्कः_Matches,
	    matches);
}
#endif

/*
 * The 'splitkeep' option is changed.
 */
    वर्ण *
did_set_splitkeep(optset_T *args UNUSED)
{
    win_T	*wp;
    tabpage_T	*tp;
    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	wp->w_prev_height = wp->w_height;
	wp->w_prev_winrow = wp->w_winrow;
    }
    प्रत्यागम did_set_opt_strings(p_spk, p_spk_मानानि, FALSE);
}

    पूर्णाङ्क
expand_set_splitkeep(optexpand_T *args, पूर्णाङ्क *अङ्कः_Matches, char_u ***matches)
{
    प्रत्यागम expand_set_opt_string(
	    args,
	    p_spk_मानानि,
	    ARRAY_LENGTH(p_spk_मानानि) - 1,
	    अङ्कः_Matches,
	    matches);
}

#जइ defined(FEAT_STL_OPT)
/*
 * The 'statusline' option is changed.
 */
    वर्ण *
did_set_statusline(optset_T *args)
{
    char_u  **varp = (char_u **)args->os_varp;
    वर्ण    *ret = parse_statustabline_rulerformat(args, FALSE);

    जइ (ret != रिक्त)
	प्रत्यागम ret;
    update_stl_rendered_height(varp == &curwin->w_p_stl ? curwin : रिक्त);
    frame_change_statusline_height();

    प्रत्यागम रिक्त;
}

/*
 * Rewrite the 'statuslineopt' string: replace the last "maxheight:N" with
 * actual_stlh and remove earlier duplicates.  Keyword order is preserved.
 */
    स्थिर शून्य
update_stlo_maxheight(char_u **varp, पूर्णाङ्क actual_stlh)
{
    ध्रुव char_u    *src = *varp;
    ध्रुव char_u    *last_mh = रिक्त;
    ध्रुव char_u    *p;

    // Find the last "maxheight:" token.
    p = src;
    जाव (*p != NUL)
    {
	जइ (STRNCMP(p, "maxheight:", 10) == 0)
	    last_mh = p;
	जाव (*p != ',' && *p != NUL)
	    ++p;
	जइ (*p == ',')
	    ++p;
    }

    जइ (last_mh == रिक्त)
	प्रत्यागम;

    size_t	bufsize = STRLEN(src) + NUMBUFLEN + 1;
    char_u	*buf = alloc(bufsize);
    जइ (buf == रिक्त)
	प्रत्यागम;

    पूर्णाङ्क		len = 0;
    bool	need_comma = false;

    p = src;
    जाव (*p != NUL)
    {
	ध्रुव	char_u	*tok = p;
	पूर्णाङ्क	tok_len;

	जाव (*p != ',' && *p != NUL)
	    ++p;
	tok_len = (पूर्णाङ्क)(p - tok);
	जइ (*p == ',')
	    ++p;

	जइ (STRNCMP(tok, "maxheight:", 10) == 0)
	{
	    जइ (tok == last_mh)
	    {
		// Replace the last occurrence with the actual value.
		जइ (need_comma)
		    buf[len++] = ',';
		len += vim_snprintf((वर्ण *)buf + len,
			bufsize - len, "maxheight:%d", actual_stlh);
		need_comma = true;
	    }
	}
	अण्णहा
	{
	    जइ (need_comma)
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
    वर्ण *
did_set_statuslineopt(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    win_T       *wp = varp == &curwin->w_p_stlo ? curwin : रिक्त;

    जइ (statuslineopt_changed(*varp, wp) == FAIL)
	प्रत्यागम e_invalid_argument;

    // Sync stl_rendered_height with the current statusline.
    update_stl_rendered_height(wp);

    // Update the maxheight value to the actual value set.
    // Note: Must be changed if p_stlo_values are changed.
    जइ (*varp != empty_option)
    {
	पूर्णाङ्क actual_stlh;

	जइ (wp != रिक्त)
	{
	    frame_change_statusline_height();
	    actual_stlh = MIN(wp->w_p_stlo_mh,
		    wp->w_height + wp->w_status_height - p_wmh);
	}
	अण्णहा
	    actual_stlh = frame_change_statusline_height();

	// Only re-serialize when the window actually has a status line shown.
	जइ (actual_stlh > 0)
	{
	    update_stlo_maxheight(varp, actual_stlh);
	    // Update the parsed maxheight member directly.
	    जइ (wp != रिक्त)
		wp->w_p_stlo_mh = actual_stlh;
	    अण्णहा
		set_stlo_mh(actual_stlh);
	}
    }
    अण्णहा
	frame_change_statusline_height();

    जइ ((args->os_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	// :set clears the local values.
	clear_string_option(&curwin->w_p_stlo);
	curwin->w_p_stlo_fh = -1;
	curwin->w_p_stlo_mh = -1;
    }

    प्रत्यागम रिक्त;
}

    पूर्णाङ्क
expand_set_statuslineopt(optexpand_T *args, पूर्णाङ्क *अङ्कः_Matches, char_u ***matches)
{
    प्रत्यागम expand_set_opt_string(
	    args,
	    p_stlo_मानानि,
	    ARRAY_LENGTH(p_stlo_मानानि) - 1,
	    अङ्कः_Matches,
	    matches);
}
#endif

/*
 * The 'swapsync' option is changed.
 */
    वर्ण *
did_set_swapsync(optset_T *args UNUSED)
{
    प्रत्यागम did_set_opt_strings(p_sws, p_sws_मानानि, FALSE);
}

    पूर्णाङ्क
expand_set_swapsync(optexpand_T *args, पूर्णाङ्क *अङ्कः_Matches, char_u ***matches)
{
    प्रत्यागम expand_set_opt_string(
	    args,
	    p_sws_मानानि,
	    ARRAY_LENGTH(p_sws_मानानि) - 1,
	    अङ्कः_Matches,
	    matches);
}

/*
 * The 'switchbuf' option is changed.
 */
    वर्ण *
did_set_switchbuf(optset_T *args UNUSED)
{
    प्रत्यागम did_set_opt_flags(p_swb, p_swb_मानानि, &swb_flags, TRUE);
}

    पूर्णाङ्क
expand_set_switchbuf(optexpand_T *args, पूर्णाङ्क *अङ्कः_Matches, char_u ***matches)
{
    प्रत्यागम expand_set_opt_string(
	    args,
	    p_swb_मानानि,
	    ARRAY_LENGTH(p_swb_मानानि) - 1,
	    अङ्कः_Matches,
	    matches);
}

/*
 * The 'tabclose' option is changed.
 */
    वर्ण *
did_set_tabclose(optset_T *args UNUSED)
{
    प्रत्यागम did_set_opt_flags(p_tcl, p_tcl_मानानि, &tcl_flags, TRUE);
}

    पूर्णाङ्क
expand_set_tabclose(optexpand_T *args, पूर्णाङ्क *अङ्कः_Matches, char_u ***matches)
{
    प्रत्यागम expand_set_opt_string(
	    args,
	    p_tcl_मानानि,
	    ARRAY_LENGTH(p_tcl_मानानि) - 1,
	    अङ्कः_Matches,
	    matches);
}

#जइ defined(FEAT_STL_OPT)
/*
 * The 'tabline' option is changed.
 */
    वर्ण *
did_set_tabline(optset_T *args)
{
    प्रत्यागम parse_statustabline_rulerformat(args, FALSE);
}
#endif

/*
 * The 'tagcase' option is changed.
 */
    वर्ण *
did_set_tagcase(optset_T *args)
{
    unsigned पूर्णाङ्क	*flags;
    char_u		*p;

    जइ (args->os_flags & OPT_LOCAL)
    {
	p = curbuf->b_p_tc;
	flags = &curbuf->b_tc_flags;
    }
    अण्णहा
    {
	p = p_tc;
	flags = &tc_flags;
    }

    जइ ((args->os_flags & OPT_LOCAL) && *p == NUL)
	// make the local value empty: use the global value
	*flags = 0;
    अण्णहा जइ (*p == NUL
	    || opt_strings_flags(p, p_tc_मानानि, flags, FALSE) != OK)
	प्रत्यागम e_invalid_argument;

    प्रत्यागम रिक्त;
}

    पूर्णाङ्क
expand_set_tagcase(optexpand_T *args, पूर्णाङ्क *अङ्कः_Matches, char_u ***matches)
{
    प्रत्यागम expand_set_opt_string(
	    args,
	    p_tc_मानानि,
	    ARRAY_LENGTH(p_tc_मानानि) - 1,
	    अङ्कः_Matches,
	    matches);
}

/*
 * The 'term' option is changed.
 */
    वर्ण *
did_set_term(optset_T *args UNUSED)
{
    जइ (T_NAME[0] == NUL)
	प्रत्यागम e_cannot_set_term_to_empty_string;
#ifdef FEAT_GUI
    जइ (gui.in_use)
	प्रत्यागम e_cannot_change_term_in_GUI;
    जइ (term_is_gui(T_NAME))
	प्रत्यागम e_use_gui_to_start_GUI;
#endif
    जइ (set_termname(T_NAME) == FAIL)
	प्रत्यागम e_not_found_in_termcap;

    // Screen colors may have changed.
    redraw_later_clear();

    प्रत्यागम रिक्त;
}

/*
 * Some terminal option (t_xxx) is changed
 */
    वर्ण *
did_set_term_option(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    जइ (!full_screen)
	प्रत्यागम रिक्त;

    // ":set t_Co=0" and ":set t_Co=1" do ":set t_Co="
    जइ (varp == &T_CCO)
    {
	पूर्णाङ्क colors = atoi((वर्ण *)T_CCO);

	// Only reinitialize colors if t_Co value has really changed to
	// avoid expensive reload of colorscheme if t_Co is set to the
	// same value multiple times.
	जइ (colors != t_colors)
	{
	    t_colors = colors;
	    जइ (t_colors <= 1)
	    {
		vim_free(T_CCO);
		T_CCO = empty_option;
	    }
#जइ defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
	    जइ (is_term_win32())
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
    जइ (varp == &T_ME)
    {
	out_str(T_ME);
	redraw_later(UPD_CLEAR);
#जइ defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
	// Since t_me has been set, this probably means that the user
	// wants to use this as default colors.  Need to reset default
	// background/foreground colors.
# ifdef VIMDLL
	जइ (!gui.in_use && !gui.starting)
# endif
	    mch_set_normal_colors();
#endif
    }
    जइ (varp == &T_BE && termcap_active)
    {
	MAY_WANT_TO_LOG_THIS;

	जइ (*T_BE == NUL)
	    // When clearing t_BE we assume the user no longer wants
	    // bracketed paste, thus disable it by writing t_BD.
	    out_str(T_BD);
	अण्णहा
	    out_str(T_BE);
    }

    जइ (varp == &T_BSU || varp == &T_ESU)
	term_set_sync_output(TERM_SYNC_OUTPUT_OFF);

    प्रत्यागम रिक्त;
}


#ifdef UNIX
/*
 * The 'termresize' option is changed.
 */
    वर्ण *
did_set_termresize(optset_T *args UNUSED)
{
    // If empty or "inband", then attempt to enable in-band resize events.
    जइ (*p_trz == NUL || STRCMP(p_trz, "inband") == 0)
	term_set_win_resize(true);
    अण्णहा जइ (STRCMP(p_trz, "sigwinch") == 0)
	term_set_win_resize(false);
    अण्णहा
	प्रत्यागम e_invalid_argument;

    प्रत्यागम रिक्त;
}

    पूर्णाङ्क
expand_set_termresize(optexpand_T *args, पूर्णाङ्क *अङ्कः_Matches, char_u ***matches)
{
    प्रत्यागम expand_set_opt_string(
	    args,
	    p_trz_मानानि,
	    ARRAY_LENGTH(p_trz_मानानि) - 1,
	    अङ्कः_Matches,
	    matches);
}
#endif

#जइ defined(FEAT_TERMINAL)
/*
 * The 'termwinkey' option is changed.
 */
    वर्ण *
did_set_termwinkey(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    जइ ((*varp)[0] != NUL && string_to_कुञ्चिका(*varp, TRUE) == 0)
	प्रत्यागम e_invalid_argument;

    प्रत्यागम रिक्त;
}

/*
 * The 'termwinsize' option is changed.
 */
    वर्ण *
did_set_termwinsize(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u	*p;

    जइ ((*varp)[0] == NUL)
	प्रत्यागम रिक्त;

    p = skipdigits(*varp);
    जइ (p == *varp || (*p != 'x' && *p != '*') || *skipdigits(p + 1) != NUL)
	प्रत्यागम e_invalid_argument;

    प्रत्यागम रिक्त;
}

# जइ defined(MSWIN)
/*
 * The 'termwintype' option is changed.
 */
    वर्ण *
did_set_termwintype(optset_T *args UNUSED)
{
    प्रत्यागम did_set_opt_strings(p_twt, p_twt_मानानि, FALSE);
}

    पूर्णाङ्क
expand_set_termwintype(optexpand_T *args, पूर्णाङ्क *अङ्कः_Matches, char_u ***matches)
{
    प्रत्यागम expand_set_opt_string(
	    args,
	    p_twt_मानानि,
	    ARRAY_LENGTH(p_twt_मानानि) - 1,
	    अङ्कः_Matches,
	    matches);
}
# endif
#endif

/*
 * The 'titlestring' option is changed.
 */
    वर्ण *
did_set_titlestring(optset_T *args)
{
    पूर्णाङ्क flagval = 0;

#ifdef FEAT_STL_OPT
    flagval = STL_IN_TITLE;
#endif
    प्रत्यागम parse_titleiconstring(args, flagval);
}

#जइ defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
/*
 * The 'toolbar' option is changed.
 */
    वर्ण *
did_set_toolbar(optset_T *args UNUSED)
{
    जइ (opt_strings_flags(p_toolbar, p_toolbar_मानानि, &toolbar_flags,
								TRUE) != OK)
	प्रत्यागम e_invalid_argument;

    out_flush();
    gui_mch_show_toolbar((toolbar_flags &
		(TOOLBAR_TEXT | TOOLBAR_ICONS)) != 0);
    प्रत्यागम रिक्त;
}

    पूर्णाङ्क
expand_set_toolbar(optexpand_T *args, पूर्णाङ्क *अङ्कः_Matches, char_u ***matches)
{
    प्रत्यागम expand_set_opt_string(
	    args,
	    p_toolbar_मानानि,
	    ARRAY_LENGTH(p_toolbar_मानानि) - 1,
	    अङ्कः_Matches,
	    matches);
}
#endif

#जइ defined(FEAT_TOOLBAR) && defined(FEAT_GUI_GTK)
/*
 * The 'toolbariconsize' option is changed.  GTK+ 2 only.
 */
    वर्ण *
did_set_toolbariconsize(optset_T *args UNUSED)
{
    जइ (opt_strings_flags(p_tbis, p_tbis_मानानि, &tbis_flags, FALSE) != OK)
	प्रत्यागम e_invalid_argument;

    out_flush();
    gui_mch_show_toolbar((toolbar_flags &
		(TOOLBAR_TEXT | TOOLBAR_ICONS)) != 0);
    प्रत्यागम रिक्त;
}

    पूर्णाङ्क
expand_set_toolbariconsize(
    optexpand_T	*args,
    पूर्णाङ्क		*अङ्कः_Matches,
    char_u	***matches)
{
    प्रत्यागम expand_set_opt_string(
	    args,
	    p_tbis_मानानि,
	    ARRAY_LENGTH(p_tbis_मानानि) - 1,
	    अङ्कः_Matches,
	    matches);
}
#endif

#जइ defined(UNIX) || defined(VMS)
/*
 * The 'ttymouse' option is changed.
 */
    वर्ण *
did_set_ttymouse(optset_T *args UNUSED)
{
    वर्ण *errmsg = रिक्त;

    // Switch the mouse off before changing the escape sequences used for
    // that.
    mch_setmouse(FALSE);
    जइ (opt_strings_flags(p_ttym, p_ttym_मानानि, &ttym_flags, FALSE) != OK)
	errmsg = e_invalid_argument;
    अण्णहा
	check_mouse_termcode();
    जइ (termcap_active)
	setmouse();		// may switch it on again

    प्रत्यागम errmsg;
}

    पूर्णाङ्क
expand_set_ttymouse(optexpand_T *args, पूर्णाङ्क *अङ्कः_Matches, char_u ***matches)
{
    प्रत्यागम expand_set_opt_string(
	    args,
	    p_ttym_मानानि,
	    ARRAY_LENGTH(p_ttym_मानानि) - 1,
	    अङ्कः_Matches,
	    matches);
}
#endif

#जइ defined(FEAT_VARTABS)
/*
 * The 'varsofttabstop' option is changed.
 */
    वर्ण *
did_set_varsofttabstop(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u *cp;

    जइ (!((*varp)[0]) || ((*varp)[0] == '0' && !((*varp)[1])))
	VIM_CLEAR(curbuf->b_p_vsts_array);
    अण्णहा
    {
	प्रति (cp = *varp; *cp; ++cp)
	{
	    जइ (vim_isdigit(*cp))
		अनुवर्त;
	    जइ (*cp == ',' && cp > *varp && *(cp-1) != ',')
		अनुवर्त;
	    प्रत्यागम e_invalid_argument;
	}

	पूर्णाङ्क *oldarray = curbuf->b_p_vsts_array;
	जइ (tabstop_set(*varp, &(curbuf->b_p_vsts_array)) == OK)
	{
	    जइ (oldarray)
		vim_free(oldarray);
	}
	अण्णहा
	    प्रत्यागम e_invalid_argument;
    }

    प्रत्यागम रिक्त;
}

/*
 * The 'vartabstop' option is changed.
 */
    वर्ण *
did_set_vartabstop(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u *cp;

    जइ (!((*varp)[0]) || ((*varp)[0] == '0' && !((*varp)[1])))
	VIM_CLEAR(curbuf->b_p_vts_array);
    अण्णहा
    {
	प्रति (cp = *varp; *cp; ++cp)
	{
	    जइ (vim_isdigit(*cp))
		अनुवर्त;
	    जइ (*cp == ',' && cp > *varp && *(cp-1) != ',')
		अनुवर्त;
	    प्रत्यागम e_invalid_argument;
	}

	पूर्णाङ्क *oldarray = curbuf->b_p_vts_array;

	जइ (tabstop_set(*varp, &(curbuf->b_p_vts_array)) == OK)
	{
	    vim_free(oldarray);
# ifdef FEAT_FOLDING
	    जइ (foldmethodIsIndent(curwin))
		foldUpdateAll(curwin);
# endif
	}
	अण्णहा
	    प्रत्यागम e_invalid_argument;
    }

    प्रत्यागम रिक्त;
}
#endif

/*
 * The 'verbosefile' option is changed.
 */
    वर्ण *
did_set_verbosefile(optset_T *args UNUSED)
{
    verbose_stop();
    जइ (*p_vfile != NUL && verbose_open() == FAIL)
	प्रत्यागम e_invalid_argument;

    प्रत्यागम रिक्त;
}

#जइ defined(FEAT_SESSION)
/*
 * The 'viewoptions' option is changed.
 */
    वर्ण *
did_set_viewoptions(optset_T *args UNUSED)
{
    प्रत्यागम did_set_opt_flags(p_vop, p_ssop_मानानि, &vop_flags, TRUE);
}
#endif

#जइ defined(FEAT_VIMINFO)
/*
 * The 'viminfo' option is changed.
 */
    वर्ण *
did_set_viminfo(optset_T *args)
{
    char_u	*s;
    वर्ण	*errmsg = रिक्त;

    प्रति (s = p_viminfo; *s;)
    {
	// Check it's a valid character
	जइ (vim_strchr((char_u *)"!\"%'/:<@cfhnrs", *s) == रिक्त)
	{
	    errmsg = illegal_char(args->os_errbuf, args->os_errbuflen, *s);
	    भङ्ग;
	}
	जइ (*s == 'n')	// name is always last one
	    भङ्ग;
	अण्णहा जइ (*s == 'r') // skip until next ','
	{
	    जाव (*++s && *s != ',')
		;
	}
	अण्णहा जइ (*s == '%')
	{
	    // optional number
	    जाव (vim_isdigit(*++s))
		;
	}
	अण्णहा जइ (*s == '!' || *s == 'h' || *s == 'c')
	    ++s;		// no extra chars
	अण्णहा		// must have a number
	{
	    जाव (vim_isdigit(*++s))
		;

	    जइ (!VIM_ISDIGIT(*(s - 1)))
	    {
		जइ (args->os_errbuf != रिक्त)
		{
		    vim_snprintf(args->os_errbuf, args->os_errbuflen,
			    _(e_missing_सङ्ख्या_after_angle_str_angle),
			    transchar_byte(*(s - 1)));
		    errmsg = args->os_errbuf;
		}
		अण्णहा
		    errmsg = "";
		भङ्ग;
	    }
	}
	जइ (*s == ',')
	    ++s;
	अण्णहा जइ (*s)
	{
	    जइ (args->os_errbuf != रिक्त)
		errmsg = e_missing_comma;
	    अण्णहा
		errmsg = "";
	    भङ्ग;
	}
    }
    जइ (*p_viminfo && errmsg == रिक्त && get_viminfo_parameter('\'') < 0)
	errmsg = e_must_specify_a_मानम्;

    प्रत्यागम errmsg;
}
#endif

/*
 * The 'virtualedit' option is changed.
 */
    वर्ण *
did_set_virtualedit(optset_T *args)
{
    char_u		*ve = p_ve;
    unsigned पूर्णाङ्क	*flags = &ve_flags;

    जइ (args->os_flags & OPT_LOCAL)
    {
	ve = curwin->w_p_ve;
	flags = &curwin->w_ve_flags;
    }

    जइ ((args->os_flags & OPT_LOCAL) && *ve == NUL)
	// make the local value empty: use the global value
	*flags = 0;
    अण्णहा
    {
	जइ (opt_strings_flags(ve, p_ve_मानानि, flags, TRUE) != OK)
	    प्रत्यागम e_invalid_argument;
	अण्णहा जइ (STRCMP(ve, args->os_oldval.string) != 0)
	{
	    // Recompute cursor position in case the new 've' setting
	    // changes something.
	    validate_virtcol();
	    coladvance(curwin->w_virtcol);
	}
    }

    प्रत्यागम रिक्त;
}

    पूर्णाङ्क
expand_set_virtualedit(optexpand_T *args, पूर्णाङ्क *अङ्कः_Matches, char_u ***matches)
{
    प्रत्यागम expand_set_opt_string(
	    args,
	    p_ve_मानानि,
	    ARRAY_LENGTH(p_ve_मानानि) - 1,
	    अङ्कः_Matches,
	    matches);
}

/*
 * The 'whichwrap' option is changed.
 */
    वर्ण *
did_set_whichwrap(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    // Add ',' to the list flags because 'whichwrap' is a flag
    // list that is comma-separated.
    प्रत्यागम did_set_option_listflag(*varp, (char_u *)(WW_ALL ","),
		    args->os_errbuf, args->os_errbuflen);
}

    पूर्णाङ्क
expand_set_whichwrap(optexpand_T *args, पूर्णाङ्क *अङ्कः_Matches, char_u ***matches)
{
    प्रत्यागम expand_set_opt_listflag(args, (char_u*)WW_ALL, अङ्कः_Matches, matches);
}

/*
 * The 'wildmode' option is changed.
 */
    वर्ण *
did_set_wildmode(optset_T *args UNUSED)
{
    जइ (check_opt_wim() == FAIL)
	प्रत्यागम e_invalid_argument;
    प्रत्यागम रिक्त;
}

    पूर्णाङ्क
expand_set_wildmode(optexpand_T *args, पूर्णाङ्क *अङ्कः_Matches, char_u ***matches)
{
    प्रत्यागम expand_set_opt_string(
	    args,
	    p_wim_मानानि,
	    ARRAY_LENGTH(p_wim_मानानि) - 1,
	    अङ्कः_Matches,
	    matches);
}

/*
 * The 'wildoptions' option is changed.
 */
    वर्ण *
did_set_wildoptions(optset_T *args UNUSED)
{
    प्रत्यागम did_set_opt_strings(p_wop, p_wop_मानानि, TRUE);
}

    पूर्णाङ्क
expand_set_wildoptions(optexpand_T *args, पूर्णाङ्क *अङ्कः_Matches, char_u ***matches)
{
    प्रत्यागम expand_set_opt_string(
	    args,
	    p_wop_मानानि,
	    ARRAY_LENGTH(p_wop_मानानि) - 1,
	    अङ्कः_Matches,
	    matches);
}

#जइ defined(FEAT_WAK)
/*
 * The 'winaltkeys' option is changed.
 */
    वर्ण *
did_set_winaltkeys(optset_T *args UNUSED)
{
    वर्ण *errmsg = रिक्त;

    जइ (*p_wak == NUL || check_opt_strings(p_wak, p_wak_मानानि, FALSE) != OK)
	errmsg = e_invalid_argument;
# ifdef FEAT_MENU
#  जइ defined(FEAT_GUI_MOTIF)
    अण्णहा जइ (gui.in_use)
	gui_motif_set_mnemonics(p_wak[0] == 'y' || p_wak[0] == 'm');
#  elif defined(FEAT_GUI_GTK)
    अण्णहा जइ (gui.in_use)
	gui_gtk_set_mnemonics(p_wak[0] == 'y' || p_wak[0] == 'm');
#  endif
# endif
    प्रत्यागम errmsg;
}

    पूर्णाङ्क
expand_set_winaltkeys(optexpand_T *args, पूर्णाङ्क *अङ्कः_Matches, char_u ***matches)
{
    प्रत्यागम expand_set_opt_string(
	    args,
	    p_wak_मानानि,
	    ARRAY_LENGTH(p_wak_मानानि) - 1,
	    अङ्कः_Matches,
	    matches);
}
#endif

/*
 * The 'wincolor' option is changed.
 */
    वर्ण *
did_set_wincolor(optset_T *args UNUSED)
{
    update_wincolor(curwin, args->os_newval.string);
    प्रत्यागम रिक्त;
}

/*
 * The 'winhighlight' option is changed
 */
    वर्ण *
did_set_winhighlight(optset_T *args)
{
    प्रत्यागम update_winhighlight(curwin, args->os_newval.string);
}

/*
 * Expand 'winhighlight' option.
 */
    पूर्णाङ्क
expand_set_winhighlight(optexpand_T *args, पूर्णाङ्क *अङ्कः_Matches, char_u ***matches)
{
    expand_T	    *xp = args->oe_xp;

    जइ ((xp->xp_pattern > args->oe_set_arg && *(xp->xp_pattern-1) == ':')
	    || xp->xp_pattern == args->oe_set_arg || *(xp->xp_pattern-1) == ',')
    {
	// After a ':' or after a ',', or at the start, so expand highlight
	// group name.

	// If starts with !, then expand 'highlight' occasions.
	जइ (*xp->xp_pattern == '!')
	    प्रत्यागम expand_hl_occasions(args, अङ्कः_Matches, matches, '!');
	अण्णहा
	    प्रत्यागम expand_set_opt_generic(
		    args,
		    get_highlight_name,
		    अङ्कः_Matches,
		    matches);
    }

    VIM_CLEAR(*matches);
    प्रत्यागम FAIL;
}

    पूर्णाङ्क
expand_set_wincolor(optexpand_T *args, पूर्णाङ्क *अङ्कः_Matches, char_u ***matches)
{
    प्रत्यागम expand_set_opt_generic(
	    args,
	    get_highlight_name,
	    अङ्कः_Matches,
	    matches);
}

#ifdef FEAT_SYN_HL
/*
 * When the 'syntax' option is set, load the syntax of that name.
 */
    स्थिर शून्य
do_syntax_autocmd(पूर्णाङ्क मानम्_changed)
{
    स्थिर पूर्णाङ्क syn_recursive = 0;

    ++syn_recursive;
    // Only pass TRUE for "force" when the value changed or not used
    // recursively, to avoid endless recurrence.
    apply_autocmds(EVENT_SYNTAX, curbuf->b_p_syn, curbuf->b_fname,
	    मानम्_changed || syn_recursive == 1, curbuf);
    curbuf->b_flags |= BF_SYN_SET;
    --syn_recursive;
}
#endif

/*
 * When the 'filetype' option is set, trigger the FileType autocommand.
 */
    स्थिर शून्य
do_filetype_autocmd(char_u **varp, पूर्णाङ्क opt_flags, पूर्णाङ्क मानम्_changed)
{
    // Skip this when called from a modeline and the filetype was already set
    // to this value.
    जइ ((opt_flags & OPT_MODELINE) && !मानम्_changed)
	प्रत्यागम;

    स्थिर पूर्णाङ्क  ft_recursive = 0;
    पूर्णाङ्क	    secure_save = secure;

    // Reset the secure flag, since the value of 'filetype' has
    // been checked to be safe.
    secure = 0;

    ++ft_recursive;
    curbuf->b_did_filetype = true;
    // Only pass TRUE for "force" when the value changed or not
    // used recursively, to avoid endless recurrence.
    apply_autocmds(EVENT_FILETYPE, curbuf->b_p_ft, curbuf->b_fname,
	    मानम्_changed || ft_recursive == 1, curbuf);
    --ft_recursive;
    // Just in case the old "curbuf" is now invalid.
    जइ (varp != &(curbuf->b_p_ft))
	varp = रिक्त;

    secure = secure_save;
}

#ifdef FEAT_SPELL
/*
 * When the 'spelllang' option is set, source the spell/LANG.vim file in
 * 'runtimepath'.
 */
    स्थिर शून्य
do_spelllang_source(शून्य)
{
    char_u	fname[200];
    char_u	*p;
    char_u	*q = curwin->w_s->b_p_spl;

    // Skip the first name if it is "cjk".
    जइ (STRNCMP(q, "cjk,", 4) == 0)
	q += 4;

    // They could set 'spellcapcheck' depending on the language.  Use the first
    // name in 'spelllang' up to '_region' or '.encoding'.
    प्रति (p = q; *p != NUL; ++p)
	जइ (!ASCII_ISALNUM(*p) && *p != '-')
	    भङ्ग;
    जइ (p > q)
    {
	vim_snprintf((वर्ण *)fname, 200, "spell/%.*s.vim",
		(पूर्णाङ्क)(p - q), q);
	source_runtime(fname, DIP_ALL);
    }
}
#endif

/*
 * Handle string options that need some action to perform when changed.
 * The new value must be allocated.
 * Returns NULL for success, or an untranslated error message for an error.
 */
    वर्ण *
did_set_string_option(
    पूर्णाङ्क		opt_idx,		// index in options[] table
    char_u	**varp,			// pointer to the option variable
    char_u	*oldval,		// previous value of the option
    char_u	*मानम्,			// new value of the option
    वर्ण	*errbuf,		// buffer for errors, or NULL
    size_t	errbuflen,		// length of error buffer
    पूर्णाङ्क		opt_flags,		// OPT_LOCAL and/or OPT_GLOBAL
    set_op_T    op,			// OP_ADDING/OP_PREPENDING/OP_REMOVING
    पूर्णाङ्क		*मानम्_checked)		// value was checked to be safe, no
					// need to set P_INSECURE
{
    वर्ण	*errmsg = रिक्त;
    long_u	free_oldval = (get_option_flags(opt_idx) & P_ALLOCED);
    opt_did_set_cb_T did_set_cb = get_option_did_set_cb(opt_idx);
    optset_T	args;

    // 'ttytype' is an alias for 'term'.  Both 'term' and 'ttytype' point to
    // T_NAME.  If 'term' or 'ttytype' is modified, then use the index for the
    // 'term' option.  Only set the P_ALLOCED flag on 'term'.
    जइ (varp == &T_NAME)
    {
	opt_idx = findoption((char_u *)"term");
	जइ (opt_idx >= 0)
	{
	    free_oldval = (get_option_flags(opt_idx) & P_ALLOCED);
	    did_set_cb = get_option_did_set_cb(opt_idx);
	}
    }

    CLEAR_FIELD(args);

    // Disallow changing some options from secure mode
    जइ ((secure
#ifdef HAVE_SANDBOX
		|| sandbox != 0
#endif
		) && (get_option_flags(opt_idx) & P_SECURE))
	errmsg = e_not_allowed_here;
    // Check for a "normal" directory or file name in some options.
    अण्णहा जइ (check_illegal_path_names(opt_idx, varp))
	errmsg = e_invalid_argument;
    अण्णहा जइ (did_set_cb != रिक्त)
    {
	args.os_varp = (char_u *)varp;
	args.os_idx = opt_idx;
	args.os_flags = opt_flags;
	args.os_op = op;
	args.os_oldval.string = oldval;
	args.os_newval.string = मानम्;
	args.os_errbuf = errbuf;
	args.os_errbuflen = errbuflen;
	// Invoke the option specific callback function to validate and apply
	// the new option value.
	errmsg = did_set_cb(&args);

	// The 'keymap', 'filetype' and 'syntax' option callback functions
	// may change the os_value_checked field.
	*मानम्_checked = args.os_मानम्_checked;
    }

    // If an error is detected, restore the previous value.
    जइ (errmsg != रिक्त)
    {
	free_string_option(*varp);
	*varp = oldval;
	// When resetting some values, need to act on it.
	जइ (args.os_restore_chartab)
	    (शून्य)init_chartab();
	जइ (varp == &p_hl)
	    (शून्य)highlight_changed();
    }
    अण्णहा
    {
#ifdef FEAT_EVAL
	// Remember where the option was set.
	set_option_sctx_idx(opt_idx, opt_flags, current_sctx);
#endif
	// Free string options that are in allocated memory.
	// Use "free_oldval", because recursiveness may change the flags under
	// our fingers (esp. init_highlight()).
	जइ (free_oldval)
	    free_string_option(oldval);
	set_option_flag(opt_idx, P_ALLOCED);

	जइ ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0
		&& is_global_local_option(opt_idx))
	{
	    // global option with local value set to use global value; free
	    // the local value and make it empty
	    char_u *p = get_option_varp_scope(opt_idx, OPT_LOCAL);
	    free_string_option(*(char_u **)p);
	    *(char_u **)p = empty_option;
	}

	// May set global value for local option.
	अण्णहा जइ (!(opt_flags & OPT_LOCAL) && opt_flags != OPT_GLOBAL)
	    set_string_option_global(opt_idx, varp);

	// Trigger the autocommand only after setting the flags.
#ifdef FEAT_SYN_HL
	जइ (varp == &(curbuf->b_p_syn))
	    do_syntax_autocmd(args.os_मानम्_changed);
#endif
	अण्णहा जइ (varp == &(curbuf->b_p_ft))
	    do_filetype_autocmd(varp, opt_flags, args.os_मानम्_changed);
#ifdef FEAT_SPELL
	जइ (varp == &(curwin->w_s->b_p_spl))
	    do_spelllang_source();
#endif
    }

    जइ (varp == &p_mouse)
    {
	जइ (*p_mouse == NUL)
	    mch_setmouse(FALSE);    // switch mouse off
	अण्णहा
	    setmouse();		    // in case 'mouse' changed
    }

    जइ (varp == &p_rtp)
    {
	export_myvimdir();
#जइ defined(FEAT_LUA)
	update_package_paths_in_lua();
#endif
    }

#जइ defined(FEAT_LINEBREAK)
    // Changing Formatlistpattern when briopt includes the list setting:
    // redraw
    जइ ((varp == &p_flp || varp == &(curbuf->b_p_flp))
	    && curwin->w_briopt_list)
	redraw_all_later(UPD_NOT_VALID);
#endif

    जइ (curwin->w_curswant != MAXCOL
		   && (get_option_flags(opt_idx) & (P_CURSWANT | P_RALL)) != 0
				&& (get_option_flags(opt_idx) & P_HLONLY) == 0)
	curwin->w_set_curswant = true;

    जइ ((opt_flags & OPT_NO_REDRAW) == 0)
    {
#ifdef FEAT_GUI
	// set when changing an option that only requires a redraw in the GUI
	पूर्णाङ्क	redraw_gui_only = FALSE;

	जइ (varp == &p_go			// 'guioptions'
		|| varp == &p_guifont		// 'guifont'
# ifdef FEAT_GUI_TABLINE
		|| varp == &p_gtl		// 'guitablabel'
		|| varp == &p_gtt		// 'guitabtooltip'
# endif
# ifdef FEAT_XFONTSET
		|| varp == &p_guifontset	// 'guifontset'
# endif
		|| varp == &p_guifontwide	// 'guifontwide'
# जइ defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
		|| varp == &p_guiligatures	// 'guiligatures'
# endif
	    )
	    redraw_gui_only = TRUE;

	// check redraw when it's not a GUI option or the GUI is active.
	जइ (!redraw_gui_only || gui.in_use)
#endif
	    check_redraw(get_option_flags(opt_idx));
    }

#जइ defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
    जइ (args.os_did_swaptcap)
    {
	set_termname((char_u *)"win32");
	init_highlight(TRUE, FALSE);
    }
#endif

    प्रत्यागम errmsg;
}

/*
 * Check an option that can be a range of string values.
 *
 * Return OK for correct value, FAIL otherwise.
 * Empty is always OK.
 */
    स्थिर पूर्णाङ्क
check_opt_strings(
    char_u	*val,
    वर्ण	**मानानि,
    पूर्णाङ्क		list)	    // when TRUE: accept a list of values
{
    प्रत्यागम opt_strings_flags(val, मानानि, रिक्त, list);
}

/*
 * Handle an option that can be a range of string values.
 * Set a flag in "*flagp" for each string present.
 *
 * Return OK for correct value, FAIL otherwise.
 * Empty is always OK.
 */
    स्थिर पूर्णाङ्क
opt_strings_flags(
    char_u	*val,		// new value
    वर्ण	**मानानि,	// array of valid string values
    unsigned	*flagp,
    पूर्णाङ्क		list)		// when TRUE: accept a list of values
{
    पूर्णाङ्क		i;
    पूर्णाङ्क		len;
    unsigned	new_flags = 0;

    जाव (*val)
    {
	प्रति (i = 0; ; ++i)
	{
	    जइ (मानानि[i] == रिक्त)	// val not found in values[]
		प्रत्यागम FAIL;

	    len = (पूर्णाङ्क)STRLEN(मानानि[i]);
	    जइ (STRNCMP(मानानि[i], val, len) == 0
		    && ((list && val[len] == ',') || val[len] == NUL))
	    {
		val += len + (val[len] == ',');
		new_flags |= (1 << i);
		भङ्ग;		// check next item in val list
	    }
	}
    }
    जइ (flagp != रिक्त)
	*flagp = new_flags;

    प्रत्यागम OK;
}

/*
 * return OK if "p" is a valid fileformat name, FAIL otherwise.
 */
    पूर्णाङ्क
check_ff_मानम्(char_u *p)
{
    प्रत्यागम check_opt_strings(p, p_ff_मानानि, FALSE);
}

/*
 * Save the actual shortmess Flags and clear them temporarily to avoid that
 * file messages overwrites any output from the following commands.
 *
 * Caller must make sure to first call save_clear_shm_value() and then
 * restore_shm_value() exactly the same number of times.
 */
    शून्य
save_clear_shm_मानम्(शून्य)
{
    जइ (STRLEN(p_shm) >= SHM_LEN)
    {
	iemsg(e_internal_error_shortmess_too_long);
	प्रत्यागम;
    }

    जइ (++set_shm_recursive == 1)
    {
	STRCPY(shm_buf, p_shm);
	set_option_मानम्_give_err((char_u *)"shm", 0L, (char_u *)"", 0);
    }
}

/*
 * Restore the shortmess Flags set from the save_clear_shm_value() function.
 */
    शून्य
restore_shm_मानम्(शून्य)
{
    जइ (--set_shm_recursive == 0)
    {
	set_option_मानम्_give_err((char_u *)"shm", 0L, shm_buf, 0);
	vim_memset(shm_buf, 0, SHM_LEN);
    }
}

/*
 * Export the environment variable $MYVIMDIR to the first item in runtimepath
 */
    शून्य
export_myvimdir(शून्य)
{
    पूर्णाङ्क		dofree = FALSE;
    char_u	*p;
    char_u	*q = p_rtp;
    string_T	buf;

    buf.string = alloc(MAXPATHL);
    जइ (buf.string == रिक्त)
	प्रत्यागम;

    buf.length = (size_t)copy_option_part(&q, buf.string, MAXPATHL, ",");

    p = vim_getenv((char_u *)"MYVIMDIR", &dofree);

    जइ (p == रिक्त || STRCMP(p, buf.string) != 0)
    {
	जइ (*buf.string != NUL && !after_pathsep(buf.string, buf.string + buf.length))
	    STRCPY(buf.string + buf.length, PATHSEPSTR);
#ifdef MSWIN
	// normalize path separators
	प्रति (q = buf.string; *q != NUL; q++)
	    जइ (*q == '/')
		*q = '\\';
#endif
	vim_setenv((char_u *)"MYVIMDIR", buf.string);
    }
    जइ (dofree)
	vim_free(p);
    vim_free(buf.string);
}
