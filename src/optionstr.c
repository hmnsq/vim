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

#شامل "vim.h"

ایستا char_u shm_buf[SHM_LEN];
ایستا صحیح set_shm_recursive = 0;

ایستا نویسه *(p_ambw_مقادیر[]) = {"single", "double", پوچ};
ایستا نویسه *(p_bg_مقادیر[]) = {"light", "dark", پوچ};
ایستا نویسه *(p_bkc_مقادیر[]) = {"yes", "auto", "no", "breaksymlink", "breakhardlink", پوچ};
ایستا نویسه *(p_bo_مقادیر[]) = {"all", "backspace", "cursor", "complete",
				 "copy", "ctrlg", "error", "esc", "ex",
				 "hangul", "insertmode", "lang", "mess",
				 "showmatch", "operator", "register", "shell",
				 "spell", "term", "wildmode", پوچ};
#اگر defined(FEAT_LINEBREAK)
// Note: Keep this in sync with briopt_check()
ایستا نویسه *(p_briopt_مقادیر[]) = {"shift:", "min:", "sbr", "list:", "column:", پوچ};
#endif
#اگر defined(FEAT_TABPANEL)
// Note: Keep this in sync with tabpanelopt_changed()
ایستا نویسه *(p_tplo_مقادیر[]) = {"align:", "columns:", "scrollbar", "vert", پوچ};
ایستا نویسه *(p_tplo_align_مقادیر[]) = {"left", "right", پوچ};
#endif
#اگر defined(FEAT_DIFF)
// Note: Keep this in sync with diffopt_changed()
ایستا نویسه *(p_dip_مقادیر[]) = {"filler", "anchor", "context:", "iblank", "icase", "iwhite", "iwhiteall", "iwhiteeol", "horizontal", "vertical", "closeoff", "hiddenoff", "foldcolumn:", "followwrap", "internal", "indent-heuristic", "algorithm:", "inline:", "linematch:", پوچ};
ایستا نویسه *(p_dip_algorithm_مقادیر[]) = {"myers", "minimal", "patience", "histogram", پوچ};
ایستا نویسه *(p_dip_inline_مقادیر[]) = {"none", "simple", "char", "word", پوچ};
#endif
ایستا نویسه *(p_nf_مقادیر[]) = {"bin", "octal", "hex", "alpha", "unsigned", "blank", پوچ};
ایستا نویسه *(p_ff_مقادیر[]) = {FF_UNIX, FF_DOS, FF_MAC, پوچ};
#ifdef HAVE_CLIPMETHOD
// Note: Keep this in sync with did_set_clipboard()
ایستا نویسه *(p_cb_مقادیر[]) = {"unnamed", "unnamedplus",
# ifdef FEAT_CLIPBOARD
    "autoselect", "autoselectplus", "autoselectml", "html", "exclude:",
# endif
    پوچ};
#endif
#ifdef FEAT_CRYPT
ایستا نویسه *(p_cm_مقادیر[]) = {"zip", "blowfish", "blowfish2",
# ifdef FEAT_SODIUM
    "xchacha20", "xchacha20v2",
# endif
    پوچ};
#endif
ایستا نویسه *(p_cmp_مقادیر[]) = {"internal", "keepascii", پوچ};
#ifdef FEAT_SYN_HL
// Note: Keep this in sync with fill_culopt_flags()
ایستا نویسه *(p_culopt_مقادیر[]) = {"line", "screenline", "number", "both", پوچ};
#endif
ایستا نویسه *(p_dy_مقادیر[]) = {"lastline", "truncate", "uhex", پوچ};
ایستا نویسه *(p_jop_مقادیر[]) = {"stack", پوچ};
#ifdef FEAT_FOLDING
ایستا نویسه *(p_fdo_مقادیر[]) = {"all", "block", "hor", "mark", "percent",
				 "quickfix", "search", "tag", "insert",
				 "undo", "jump", پوچ};
#endif
// Note: Keep this in sync with match_keyprotocol()
ایستا نویسه *(p_kpc_protocol_مقادیر[]) = {"none", "mok2", "kitty", پوچ};
#ifdef FEAT_IMAGE
// Note: Keep this in sync with match_imageprotocol()
ایستا نویسه *(p_ipc_protocol_مقادیر[]) = {"none", "kitty", "sixel", پوچ};
#endif
#ifdef FEAT_PROP_POPUP
// Note: Keep this in sync with parse_popup_option()
ایستا نویسه *(p_popup_cpp_option_مقادیر[]) = {"align:", "border:",
    "borderhighlight:", "close:", "height:", "highlight:", "opacity:",
    "resize:", "shadow:", "width:", پوچ};
ایستا نویسه *(p_popup_pvp_option_مقادیر[]) = {"border:",
    "borderhighlight:", "close:", "height:", "highlight:", "opacity:",
    "resize:", "shadow:", "width:", پوچ};
ایستا نویسه *(p_popup_option_on_off_مقادیر[]) = {"on", "off", پوچ};
ایستا نویسه *(p_popup_cpp_border_مقادیر[]) = {"single", "double", "round",
    "ascii", "on", "off", "custom:", پوچ};
ایستا نویسه *(p_popup_pvp_border_مقادیر[]) = {"single", "double", "round",
    "ascii", "on", "off", "custom:", پوچ};
ایستا نویسه *(p_popup_option_align_مقادیر[]) = {"item", "menu", پوچ};
#endif
#اگر defined(FEAT_SPELL)
// Note: Keep this in sync with spell_check_sps()
ایستا نویسه *(p_sps_مقادیر[]) = {"best", "fast", "double", "expr:", "file:", "timeout:", پوچ};
#endif
#ifdef FEAT_SESSION
// Also used for 'viewoptions'!  Keep in sync with SSOP_ flags.
ایستا نویسه *(p_ssop_مقادیر[]) = {"buffers", "winpos", "resize", "winsize",
    "localoptions", "options", "help", "blank", "globals", "slash", "unix",
    "sesdir", "curdir", "folds", "cursor", "tabpages", "terminal", "skiprtp",
    پوچ};
#endif
#اگر defined(FEAT_STL_OPT)
ایستا نویسه *(p_stlo_مقادیر[]) = {"fixedheight", "maxheight:", پوچ};
#endif
// Keep in sync with SWB_ flags in option.h
ایستا نویسه *(p_swb_مقادیر[]) = {"useopen", "usetab", "split", "newtab", "vsplit", "uselast", پوچ};
ایستا نویسه *(p_spk_مقادیر[]) = {"cursor", "screen", "topline", پوچ};
ایستا نویسه *(p_tc_مقادیر[]) = {"followic", "ignore", "match", "followscs", "smart", پوچ};
// Keep in sync with TCL_ flags in option.h
ایستا نویسه *(p_tcl_مقادیر[]) = {"left", "uselast", پوچ};
#اگر defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
ایستا نویسه *(p_toolbar_مقادیر[]) = {"text", "icons", "tooltips", "horiz", پوچ};
#endif
#اگر defined(FEAT_TOOLBAR) && defined(FEAT_GUI_GTK)
ایستا نویسه *(p_tbis_مقادیر[]) = {"tiny", "small", "medium", "large", "huge", "giant", پوچ};
#endif
#اگر defined(UNIX) || defined(VMS)
ایستا نویسه *(p_ttym_مقادیر[]) = {"xterm", "xterm2", "dec", "netterm", "jsbterm", "pterm", "urxvt", "sgr", پوچ};
#endif
ایستا نویسه *(p_ve_مقادیر[]) = {"block", "insert", "all", "onemore", "none", "NONE", پوچ};
// Note: Keep this in sync with check_opt_wim()
ایستا نویسه *(p_wim_مقادیر[]) = {"full", "longest", "list", "lastused", "noselect", "noinsert", پوچ};
ایستا نویسه *(p_wop_مقادیر[]) = {"fuzzy", "tagfile", "pum", "exacttext", پوچ};
#ifdef FEAT_WAK
ایستا نویسه *(p_wak_مقادیر[]) = {"yes", "menu", "no", پوچ};
#endif
ایستا نویسه *(p_mousem_مقادیر[]) = {"extend", "popup", "popup_setpos", "mac", پوچ};
ایستا نویسه *(p_sel_مقادیر[]) = {"inclusive", "exclusive", "old", پوچ};
ایستا نویسه *(p_slm_مقادیر[]) = {"mouse", "key", "cmd", پوچ};
ایستا نویسه *(p_km_مقادیر[]) = {"startsel", "stopsel", پوچ};
#ifdef FEAT_BROWSE
ایستا نویسه *(p_bsdir_مقادیر[]) = {"current", "last", "buffer", پوچ};
#endif
ایستا نویسه *(p_scbopt_مقادیر[]) = {"ver", "hor", "jump", پوچ};
ایستا نویسه *(p_debug_مقادیر[]) = {"msg", "throw", "beep", پوچ};
ایستا نویسه *(p_ead_مقادیر[]) = {"both", "ver", "hor", پوچ};
ایستا نویسه *(p_buftype_مقادیر[]) = {"nofile", "nowrite", "quickfix", "help", "terminal", "acwrite", "prompt", "popup", پوچ};
ایستا نویسه *(p_bufhidden_مقادیر[]) = {"hide", "unload", "delete", "wipe", پوچ};
ایستا نویسه *(p_bs_مقادیر[]) = {"indent", "eol", "start", "nostop", پوچ};
#ifdef FEAT_FOLDING
ایستا نویسه *(p_fdm_مقادیر[]) = {"manual", "expr", "marker", "indent", "syntax",
# ifdef FEAT_DIFF
				"diff",
# endif
				پوچ};
ایستا نویسه *(p_fcl_مقادیر[]) = {"all", پوچ};
#endif
ایستا نویسه *(p_cfc_مقادیر[]) = {"keyword", "files", "whole_line", پوچ};
ایستا نویسه *(p_cot_مقادیر[]) = {"menu", "menuone", "longest", "preview", "popup", "popuphidden", "noinsert", "noselect", "fuzzy", "nosort", "preinsert", "nearest", پوچ};
#ifdef BACKSLASH_IN_FILENAME
ایستا نویسه *(p_csl_مقادیر[]) = {"slash", "backslash", پوچ};
#endif
#ifdef FEAT_SIGNS
ایستا نویسه *(p_scl_مقادیر[]) = {"yes", "no", "auto", "number", پوچ};
#endif
#ifdef UNIX
ایستا نویسه *(p_trz_مقادیر[]) = {"inband", "sigwinch", "", پوچ};
#endif
#اگر defined(MSWIN) && defined(FEAT_TERMINAL)
ایستا نویسه *(p_twt_مقادیر[]) = {"winpty", "conpty", "", پوچ};
#endif
ایستا نویسه *(p_sloc_مقادیر[]) = {"last", "statusline", "tabline", پوچ};
ایستا نویسه *(p_sws_مقادیر[]) = {"fsync", "sync", پوچ};

ایستا صحیح check_opt_strings(char_u *val, نویسه **مقادیر, صحیح list);
ایستا صحیح opt_strings_flags(char_u *val, نویسه **مقادیر, unsigned *flagp, صحیح list);

/*
 * After setting various option values: recompute variables that depend on
 * option values.
 */
    تهی
didset_string_options(تهی)
{
    (تهی)opt_strings_flags(p_cmp, p_cmp_مقادیر, &cmp_flags, TRUE);
    (تهی)opt_strings_flags(p_bkc, p_bkc_مقادیر, &bkc_flags, TRUE);
    (تهی)opt_strings_flags(p_bo, p_bo_مقادیر, &bo_flags, TRUE);
    (تهی)opt_strings_flags(p_cot, p_cot_مقادیر, &cot_flags, TRUE);
#ifdef FEAT_SESSION
    (تهی)opt_strings_flags(p_ssop, p_ssop_مقادیر, &ssop_flags, TRUE);
    (تهی)opt_strings_flags(p_vop, p_ssop_مقادیر, &vop_flags, TRUE);
#endif
#ifdef FEAT_FOLDING
    (تهی)opt_strings_flags(p_fdo, p_fdo_مقادیر, &fdo_flags, TRUE);
#endif
    (تهی)opt_strings_flags(p_dy, p_dy_مقادیر, &dy_flags, TRUE);
    (تهی)opt_strings_flags(p_jop, p_jop_مقادیر, &jop_flags, TRUE);
    (تهی)opt_strings_flags(p_tc, p_tc_مقادیر, &tc_flags, FALSE);
    (تهی)opt_strings_flags(p_ve, p_ve_مقادیر, &ve_flags, TRUE);
#اگر defined(UNIX) || defined(VMS)
    (تهی)opt_strings_flags(p_ttym, p_ttym_مقادیر, &ttym_flags, FALSE);
#endif
#اگر defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
    (تهی)opt_strings_flags(p_toolbar, p_toolbar_مقادیر, &toolbar_flags, TRUE);
#endif
#اگر defined(FEAT_TOOLBAR) && defined(FEAT_GUI_GTK)
    (تهی)opt_strings_flags(p_tbis, p_tbis_مقادیر, &tbis_flags, FALSE);
#endif
    (تهی)opt_strings_flags(p_swb, p_swb_مقادیر, &swb_flags, TRUE);
    (تهی)opt_strings_flags(p_tcl, p_tcl_مقادیر, &tcl_flags, TRUE);
}

#اگر defined(FEAT_EVAL)
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
    تهی
trigger_optionset_string(
	صحیح	opt_idx,
	صحیح	opt_flags,
	char_u  *oldval,
	char_u  *oldval_l,
	char_u  *oldval_g,
	char_u  *newval)
{
    // Don't do this recursively.
    اگر (oldval == پوچ || newval == پوچ
				    || *get_vim_var_str(VV_OPTION_TYPE) != NUL)
	بازگشت;

    char_u buf_type[7];
    size_t buf_typelen;
    size_t oldvallen;

    oldvallen = STRLEN(oldval);
    set_vim_var_string(VV_OPTION_OLD, oldval, (صحیح)oldvallen);
    set_vim_var_string(VV_OPTION_NEW, newval, -1);
    buf_typelen = vim_snprintf_safelen((نویسه *)buf_type, اندازه(buf_type),
	"%s", (opt_flags & OPT_LOCAL) ? "local" : "global");
    set_vim_var_string(VV_OPTION_TYPE, buf_type, (صحیح)buf_typelen);
    اگر (opt_flags & OPT_LOCAL)
    {
	set_vim_var_string(VV_OPTION_COMMAND, (char_u *)"setlocal", STRLEN_LITERAL("setlocal"));
	set_vim_var_string(VV_OPTION_OLDLOCAL, oldval, (صحیح)oldvallen);
    }
    اگر (opt_flags & OPT_GLOBAL)
    {
	set_vim_var_string(VV_OPTION_COMMAND, (char_u *)"setglobal", STRLEN_LITERAL("setglobal"));
	set_vim_var_string(VV_OPTION_OLDGLOBAL, oldval, (صحیح)oldvallen);
    }
    اگر ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	set_vim_var_string(VV_OPTION_COMMAND, (char_u *)"set", STRLEN_LITERAL("set"));
	set_vim_var_string(VV_OPTION_OLDLOCAL, oldval_l, -1);
	set_vim_var_string(VV_OPTION_OLDGLOBAL, oldval_g, -1);
    }
    اگر (opt_flags & OPT_MODELINE)
    {
	set_vim_var_string(VV_OPTION_COMMAND, (char_u *)"modeline", STRLEN_LITERAL("modeline"));
	set_vim_var_string(VV_OPTION_OLDLOCAL, oldval, (صحیح)oldvallen);
    }
    apply_autocmds(EVENT_OPTIONSET,
	    get_option_fullname(opt_idx), پوچ, FALSE,
	    پوچ);
    reset_v_option_vars();
}
#endif

    ایستا نویسه *
illegal_char(نویسه *errbuf, size_t errbuflen, صحیح c)
{
    اگر (errbuf == پوچ)
	بازگشت "";
    vim_snprintf(errbuf, errbuflen, _(e_illegal_character_str),
		    (نویسه *)transchar(c));
    بازگشت errbuf;
}

    ایستا نویسه *
illegal_char_after_chr(نویسه *errbuf, size_t errbuflen, صحیح c)
{
    اگر (errbuf == پوچ)
	بازگشت "";
    vim_snprintf(errbuf, errbuflen, _(e_illegal_character_after_chr), c);
    بازگشت errbuf;
}

/*
 * Check string options in a buffer for NULL value.
 */
    تهی
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
#اگر defined(FEAT_EVAL)
    check_string_option(&buf->b_p_inde);
    check_string_option(&buf->b_p_indk);
#endif
#اگر defined(FEAT_BEVAL) && defined(FEAT_EVAL)
    check_string_option(&buf->b_p_bexpr);
#endif
#اگر defined(FEAT_CRYPT)
    check_string_option(&buf->b_p_cm);
#endif
    check_string_option(&buf->b_p_fp);
#اگر defined(FEAT_EVAL)
    check_string_option(&buf->b_p_fex);
#endif
#ifdef FEAT_CRYPT
    check_string_option(&buf->b_p_کلید);
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
    تهی
free_string_option(char_u *p)
{
    اگر (p != empty_option)
	vim_free(p);
}

    تهی
clear_string_option(char_u **pp)
{
    اگر (*pp != empty_option)
	vim_free(*pp);
    *pp = empty_option;
}

    تهی
check_string_option(char_u **pp)
{
    اگر (*pp == پوچ)
	*pp = empty_option;
}

/*
 * Set global value for string option when it's a local option.
 */
    ایستا تهی
set_string_option_global(
    صحیح		opt_idx,	// option index
    char_u	**varp)		// pointer to option variable
{
    char_u	**p, *s;

    // the global value is always allocated
    اگر (is_window_local_option(opt_idx))
	p = (char_u **)GLOBAL_WO(varp);
    وگرنه
	p = (char_u **)get_option_var(opt_idx);
    اگر (!is_global_option(opt_idx)
	    && p != varp
	    && (s = vim_strsave(*varp)) != پوچ)
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
    تهی
set_string_option_direct(
    char_u	*name,
    صحیح		opt_idx,
    char_u	*val,
    صحیح		opt_flags,	// OPT_FREE, OPT_LOCAL and/or OPT_GLOBAL
    صحیح		set_sid UNUSED)
{
    char_u	*s;
    char_u	**varp;
    صحیح		both = (opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0;
    صحیح		idx = opt_idx;

    اگر (idx == -1)		// use name
    {
	idx = findoption(name);
	اگر (idx < 0)	// not found (should not happen)
	{
	    semsg(_(e_internal_error_str), "set_string_option_direct()");
	    siemsg("For option %s", name);
	    بازگشت;
	}
    }

    اگر (is_hidden_option(idx))		// can't set hidden option
	بازگشت;

    s = vim_strsave(val);
    اگر (s == پوچ)
	بازگشت;

    varp = (char_u **)get_option_varp_scope(idx,
	    both ? OPT_LOCAL : opt_flags);
    اگر ((opt_flags & OPT_FREE) && (get_option_flags(idx) & P_ALLOCED))
	free_string_option(*varp);
    *varp = s;

    // For buffer/window local option may also set the global value.
    اگر (both)
	set_string_option_global(idx, varp);

    set_option_flag(idx, P_ALLOCED);

    // When setting both values of a global option with a local value,
    // make the local value empty, so that the global value is used.
    اگر (is_global_local_option(idx) && both)
    {
	free_string_option(*varp);
	*varp = empty_option;
    }
#ifdef FEAT_EVAL
    اگر (set_sid != SID_NONE)
    {
	sctx_T script_ctx;

	اگر (set_sid == 0)
	    script_ctx = current_sctx;
	وگرنه
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
    تهی
set_string_option_direct_in_win(
	win_T		*wp,
	char_u		*name,
	صحیح		opt_idx,
	char_u		*val,
	صحیح		opt_flags,
	صحیح		set_sid)
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

#اگر defined(FEAT_PROP_POPUP)
/*
 * Like set_string_option_direct(), but for a buffer-local option in "buf".
 * Blocks autocommands to avoid the old curbuf becoming invalid.
 */
    تهی
set_string_option_direct_in_buf(
	buf_T		*buf,
	char_u		*name,
	صحیح		opt_idx,
	char_u		*val,
	صحیح		opt_flags,
	صحیح		set_sid)
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
    نویسه *
set_string_option(
    صحیح		opt_idx,
    char_u	*مقدار,
    صحیح		opt_flags,	// OPT_LOCAL and/or OPT_GLOBAL
    نویسه	*errbuf,
    size_t	errbuflen)
{
    char_u	*s;
    char_u	**varp;
    char_u	*oldval;
#اگر defined(FEAT_EVAL)
    char_u	*oldval_l = پوچ;
    char_u	*oldval_g = پوچ;
    char_u	*saved_oldval = پوچ;
    char_u	*saved_oldval_l = پوچ;
    char_u	*saved_oldval_g = پوچ;
    char_u	*saved_newval = پوچ;
#endif
    نویسه	*errmsg = پوچ;
    صحیح		مقدار_checked = FALSE;

    اگر (is_hidden_option(opt_idx))	// don't set hidden option
	بازگشت پوچ;

    s = vim_strsave(مقدار == پوچ ? (char_u *)"" : مقدار);
    اگر (s == پوچ)
	بازگشت پوچ;

    varp = (char_u **)get_option_varp_scope(opt_idx,
	    (opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0
	    ? (is_global_local_option(opt_idx)
		? OPT_GLOBAL : OPT_LOCAL)
	    : opt_flags);
    oldval = *varp;
#اگر defined(FEAT_EVAL)
    اگر ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	oldval_l = *(char_u **)get_option_varp_scope(opt_idx, OPT_LOCAL);
	oldval_g = *(char_u **)get_option_varp_scope(opt_idx, OPT_GLOBAL);
    }
#endif
    *varp = s;

#اگر defined(FEAT_EVAL)
    اگر (!starting
# ifdef FEAT_CRYPT
	    && !is_crypt_کلید_option(opt_idx)
# endif
       )
    {
	اگر (oldval_l != پوچ)
	    saved_oldval_l = vim_strsave(oldval_l);
	اگر (oldval_g != پوچ)
	    saved_oldval_g = vim_strsave(oldval_g);
	saved_oldval = vim_strsave(oldval);
	saved_newval = vim_strsave(s);
    }
#endif
    اگر ((errmsg = did_set_string_option(opt_idx, varp, oldval, مقدار, errbuf,
		    errbuflen, opt_flags, OP_NONE, &مقدار_checked)) == پوچ)
	did_set_option(opt_idx, opt_flags, TRUE, مقدار_checked);

#اگر defined(FEAT_EVAL)
    // call autocommand after handling side effects
    اگر (errmsg == پوچ)
	trigger_optionset_string(opt_idx, opt_flags,
		saved_oldval, saved_oldval_l,
		saved_oldval_g, saved_newval);
    vim_free(saved_oldval);
    vim_free(saved_oldval_l);
    vim_free(saved_oldval_g);
    vim_free(saved_newval);
#endif
    بازگشت errmsg;
}

/*
 * Return TRUE if "val" is a valid 'filetype' name.
 * Also used for 'syntax' and 'keymap'.
 */
    ایستا صحیح
valid_filetype(char_u *val)
{
    بازگشت valid_name(val, ".-_");
}

#ifdef FEAT_STL_OPT
/*
 * Check validity of options with the 'statusline' format.
 * Return an untranslated error message or NULL.
 */
    ایستا نویسه *
check_stl_option(char_u *s)
{
    صحیح		groupdepth = 0;
    ایستا نویسه errbuf[ERR_BUFLEN];
    صحیح		errbuflen = ERR_BUFLEN;

    وقتی (*s)
    {
	// Check for valid keys after % sequences
	وقتی (*s && *s != '%')
	    s++;
	اگر (!*s)
	    بشکن;
	s++;
	اگر (*s == STL_CLICKFUNC)
	{
	    اگر (s[1] == ']')
	    {
		// %[] - end click region
		s += 2;
		ادامه;
	    }
	    اگر (ASCII_ISALPHA(s[1]) || s[1] == '_')
	    {
		// %[FuncName] - start click region
		char_u *rb = vim_strchr(s + 2, ']');
		اگر (rb != پوچ)
		{
		    s = rb + 1;
		    ادامه;
		}
	    }
	    // Bare %[ is invalid
	    بازگشت illegal_char(errbuf, errbuflen, *s);
	}
	اگر (*s == STL_LINEBREAK)
	{
	    // Plain %@ - line break
	    s++;
	    ادامه;
	}
	اگر (*s == '%' || *s == STL_TRUNCMARK || *s == STL_SEPARATE)
	{
	    s++;
	    ادامه;
	}
	اگر (*s == ')')
	{
	    s++;
	    اگر (--groupdepth < 0)
		بشکن;
	    ادامه;
	}
	اگر (*s == '-')
	    s++;
	وقتی (VIM_ISDIGIT(*s))
	    s++;
	اگر (*s == STL_USER_HL)
	    ادامه;
	اگر (*s == STL_CLICKFUNC)
	{
	    // %N[FuncName] or %N[]
	    اگر (s[1] == ']')
	    {
		s += 2;
		ادامه;
	    }
	    اگر (ASCII_ISALPHA(s[1]) || s[1] == '_')
	    {
		char_u *rb = vim_strchr(s + 2, ']');
		اگر (rb != پوچ)
		{
		    s = rb + 1;
		    ادامه;
		}
	    }
	    // Bare %N[ is invalid
	    بازگشت illegal_char(errbuf, errbuflen, *s);
	}
	اگر (*s == STL_LINEBREAK)
	{
	    // %N@ - line break
	    s++;
	    ادامه;
	}
	اگر (*s == '.')
	{
	    s++;
	    وقتی (*s && VIM_ISDIGIT(*s))
		s++;
	}
	اگر (*s == '(')
	{
	    groupdepth++;
	    ادامه;
	}
	اگر (vim_strchr(STL_ALL, *s) == پوچ)
	{
	    بازگشت illegal_char(errbuf, errbuflen, *s);
	}
	اگر (*s == '{')
	{
	    صحیح reevaluate = (*++s == '%');

	    اگر (reevaluate && *++s == '}')
		// "}" is not allowed immediately after "%{%"
		بازگشت illegal_char(errbuf, errbuflen, '}');
	    وقتی ((*s != '}' || (reevaluate && s[-1] != '%')) && *s)
		s++;
	    اگر (*s != '}')
		بازگشت e_unclosed_expression_sequence;
	}
    }
    اگر (groupdepth != 0)
	بازگشت e_unbalanced_groups;
    بازگشت پوچ;
}
#endif

/*
 * Check for a "normal" directory or file name in some options.  Disallow a
 * path separator (slash and/or backslash), wildcards and characters that are
 * often illegal in a file name. Be more permissive if "secure" is off.
 */
    ایستا صحیح
check_illegal_path_names(صحیح opt_idx, char_u **varp)
{
    بازگشت (((get_option_flags(opt_idx) & P_NFNAME)
		&& vim_strpbrk(*varp, (char_u *)(secure
			? "/\\*?[|;&<>\r\n" : "/\\*?[<>\r\n")) != پوچ)
	    || ((get_option_flags(opt_idx) & P_NDNAME)
		&& vim_strpbrk(*varp, (char_u *)"*?[|;&<>\r\n") != پوچ));
}

/*
 * An option that accepts a list of flags is changed.
 * e.g. 'viewoptions', 'switchbuf', 'casemap', etc.
 */
    ایستا نویسه *
did_set_opt_flags(char_u *val, نویسه **مقادیر, unsigned *flagp, صحیح list)
{
    اگر (opt_strings_flags(val, مقادیر, flagp, list) == FAIL)
	بازگشت e_invalid_argument;

    بازگشت پوچ;
}

/*
 * An option that accepts a list of string values is changed.
 * e.g. 'nrformats', 'scrollopt', 'wildoptions', etc.
 */
    ایستا نویسه *
did_set_opt_strings(char_u *val, نویسه **مقادیر, صحیح list)
{
    بازگشت did_set_opt_flags(val, مقادیر, پوچ, list);
}

/*
 * An option which is a list of flags is set.  Valid values are in 'flags'.
 */
    ایستا نویسه *
did_set_option_listflag(
	char_u *val,
	char_u *flags,
	نویسه *errbuf,
	size_t errbuflen)
{
    char_u	*s;

    برای (s = val; *s; ++s)
	اگر (vim_strchr(flags, *s) == پوچ)
	    بازگشت illegal_char(errbuf, errbuflen, *s);

    بازگشت پوچ;
}

/*
 * Expand an option that accepts a list of fixed string values with known
 * number of items.
 */
    ایستا صحیح
expand_set_opt_string(
	optexpand_T *args,
	نویسه **مقادیر,
	size_t رقم_مقادیر,
	صحیح *رقم_Matches,
	char_u ***matches)
{
    char_u	*p;
    regmatch_T	*regmatch = args->oe_regmatch;
    صحیح		include_orig_val = args->oe_include_orig_val;
    char_u	*option_val = args->oe_opt_مقدار;

    // Assume numValues is small since they are fixed enums, so just allocate
    // upfront instead of needing two passes to calculate output size.
    *matches = ALLOC_MULT(char_u *, رقم_مقادیر + 1);
    اگر (*matches == پوچ)
	بازگشت FAIL;

    صحیح شمار = 0;

    اگر (include_orig_val && *option_val != NUL)
    {
	p = vim_strsave(option_val);
	اگر (p == پوچ)
	{
	    VIM_CLEAR(*matches);
	    بازگشت FAIL;
	}
	(*matches)[شمار++] = p;
    }

    برای (نویسه **val = مقادیر; *val != پوچ; val++)
    {
	اگر (include_orig_val && *option_val != NUL)
	{
	    اگر (STRCMP((char_u*)*val, option_val) == 0)
		ادامه;
	}
	اگر (vim_regexec(regmatch, (char_u*)(*val), (colnr_T)0))
	{
	    p = vim_strsave((char_u*)*val);
	    اگر (p == پوچ)
	    {
		اگر (شمار == 0)
		{
		    VIM_CLEAR(*matches);
		    بازگشت FAIL;
		}
		وگرنه
		    بشکن;
	    }
	    (*matches)[شمار++] = p;
	}
    }
    اگر (شمار == 0)
    {
	VIM_CLEAR(*matches);
	بازگشت FAIL;
    }
    *رقم_Matches = شمار;
    بازگشت OK;
}

ایستا char_u *set_opt_callback_orig_option = پوچ;
ایستا char_u *((*set_opt_callback_func)(expand_T *, صحیح));

/*
 * Callback used by expand_set_opt_generic to also include the original value
 * as the first item.
 */
    ایستا char_u *
expand_set_opt_generic_cb(expand_T *xp, صحیح idx)
{
    اگر (idx == 0)
    {
	اگر (set_opt_callback_orig_option != پوچ)
	    بازگشت set_opt_callback_orig_option;
	وگرنه
	    بازگشت (char_u *)""; // empty strings are ignored
    }
    بازگشت set_opt_callback_func(xp, idx - 1);
}

/*
 * Expand an option with a callback that iterates through a list of possible
 * names using an index.
 */
    ایستا صحیح
expand_set_opt_generic(
	optexpand_T *args,
	char_u *((*func)(expand_T *, صحیح)),
	صحیح *رقم_Matches,
	char_u ***matches)
{
    صحیح ret;

    set_opt_callback_orig_option = args->oe_include_orig_val ?
	args->oe_opt_مقدار : پوچ;
    set_opt_callback_func = func;

    ret = ExpandGeneric(
	    (char_u*)"", // not using fuzzy as currently EXPAND_STRING_SETTING doesn't use it
	    args->oe_xp,
	    args->oe_regmatch,
	    matches,
	    رقم_Matches,
	    expand_set_opt_generic_cb,
	    FALSE);

    set_opt_callback_orig_option = پوچ;
    set_opt_callback_func = پوچ;
    بازگشت ret;
}

#اگر defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
ایستا garray_T *expand_cb_ga;
ایستا optexpand_T *expand_cb_args;

/*
 * Callback provided to a function in expand_set_opt_callback. Will perform
 * regex matching against the value and add to the list.
 *
 * Returns OK usually. Returns FAIL if it failed to allocate memory, and the
 * caller should terminate the enumeration.
 */
    ایستا صحیح
expand_set_opt_callback_cb(char_u *val)
{
    regmatch_T	*regmatch = expand_cb_args->oe_regmatch;
    expand_T	*xp = expand_cb_args->oe_xp;
    garray_T	*ga = expand_cb_ga;
    char_u	*str;

    اگر (val == پوچ || *val == NUL)
	بازگشت OK;

    اگر (xp->xp_pattern[0] != NUL &&
	    !vim_regexec(regmatch, val, (colnr_T)0))
	بازگشت OK;

    str = vim_strsave_escaped(val, (char_u *)" \t\\");

    اگر (str == پوچ)
	بازگشت FAIL;

    اگر (ga_grow(ga, 1) == FAIL)
    {
	vim_free(str);
	بازگشت FAIL;
    }

    ((char_u **)ga->ga_data)[ga->ga_len] = str;
    ++ga->ga_len;
    بازگشت OK;
}

/*
 * Expand an option with a provided function that takes a callback. The
 * function will enumerate through all options and call the callback to add it
 * to the list.
 *
 * "func" is the enumerator function that will generate the list of options.
 * "func_params" is a single parameter that will be passed to func.
 */
    ایستا صحیح
expand_set_opt_callback(
	optexpand_T *args,
	تهی (*func)(optexpand_T *, تهی* params, صحیح (*cb)(char_u *val)),
	تهی *func_params,
	صحیح *رقم_Matches,
	char_u ***matches)
{
    garray_T	ga;
    صحیح		include_orig_val = args->oe_include_orig_val;
    char_u	*option_val = args->oe_opt_مقدار;

    ga_init2(&ga, اندازه(نویسه *), 30);

    اگر (include_orig_val && *option_val != NUL)
    {
	char_u *p = vim_strsave(option_val);
	اگر (p == پوچ)
	    بازگشت FAIL;
	اگر (ga_grow(&ga, 1) == FAIL)
	{
	    vim_free(p);
	    بازگشت FAIL;
	}
	((char_u **)ga.ga_data)[ga.ga_len] = p;
	++ga.ga_len;
    }

    expand_cb_ga = &ga;
    expand_cb_args = args;

    func(args, func_params, expand_set_opt_callback_cb);

    expand_cb_ga = پوچ;
    expand_cb_args = پوچ;

    *matches = ga.ga_data;
    *رقم_Matches = ga.ga_len;
    بازگشت OK;
}
#endif

/*
 * Expand an option which is a list of flags.
 */
    ایستا صحیح
expand_set_opt_listflag(
	optexpand_T *args,
	char_u *flags,
	صحیح *رقم_Matches,
	char_u ***matches)
{
    char_u  *p;
    char_u  *option_val = args->oe_opt_مقدار;
    char_u  *cmdline_val = args->oe_set_arg;
    صحیح	    append = args->oe_append;
    صحیح	    include_orig_val = args->oe_include_orig_val && (*option_val != NUL);

    size_t رقم_flags = STRLEN(flags);

    // Assume we only have small number of flags, so just allocate max size.
    *matches = ALLOC_MULT(char_u *, رقم_flags + 1);
    اگر (*matches == پوچ)
	بازگشت FAIL;

    صحیح شمار = 0;

    اگر (include_orig_val)
    {
	p = vim_strsave(option_val);
	اگر (p == پوچ)
	{
	    VIM_CLEAR(*matches);
	    بازگشت FAIL;
	}
	(*matches)[شمار++] = p;
    }

    برای (char_u *flag = flags; *flag != NUL; flag++)
    {
	اگر (append && vim_strchr(option_val, *flag) != پوچ)
	    ادامه;

	اگر (vim_strchr(cmdline_val, *flag) == پوچ)
	{
	    اگر (include_orig_val
		    && option_val[1] == NUL
		    && *flag == option_val[0])
	    {
		// This value is already used as the first choice as it's the
		// existing flag. Just skip it to avoid duplicate.
		ادامه;
	    }
	    p = vim_strnsave(flag, 1);
	    اگر (p == پوچ)
	    {
		اگر (شمار == 0)
		{
		    VIM_CLEAR(*matches);
		    بازگشت FAIL;
		}
		وگرنه
		    بشکن;
	    }
	    (*matches)[شمار++] = p;
	}
    }

    اگر (شمار == 0)
    {
	VIM_CLEAR(*matches);
	بازگشت FAIL;
    }
    *رقم_Matches = شمار;
    بازگشت OK;
}

/*
 * The 'ambiwidth' option is changed.
 */
    نویسه *
did_set_ambiwidth(optset_T *args UNUSED)
{
    اگر (check_opt_strings(p_ambw, p_ambw_مقادیر, FALSE) != OK)
	بازگشت e_invalid_argument;

    بازگشت check_chars_options();
}

// "name" must be a string literal, the length is computed at compile time.
#define completing_مقدار_for_subopt(args, name) \
	  completing_مقدار_for_subopt_len(args, name, (صحیح)STRLEN_LITERAL(name))

/*
 * Return true when completing the value of the sub-option "name" with length
 * "len", e.g. the value after "close:" in 'completepopup'.
 */
    ایستا bool
completing_مقدار_for_subopt_len(optexpand_T *args, نویسه *name, صحیح len)
{
    char_u  *colon = args->oe_xp->xp_pattern - 1;
    صحیح	    off = (صحیح)(colon - args->oe_set_arg);

    اگر (off < len)
	بازگشت false;
    // The name must follow a comma when it does not start the option value.
    اگر (off > len && *(colon - len - 1) != ',')
	بازگشت false;

    بازگشت STRNCMP(colon - len, name, len) == 0;
}

    صحیح
expand_set_ambiwidth(optexpand_T *args, صحیح *رقم_Matches, char_u ***matches)
{
    بازگشت expand_set_opt_string(
	    args,
	    p_ambw_مقادیر,
	    ARRAY_LENGTH(p_ambw_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}

/*
 * The 'background' option is changed.
 */
    نویسه *
did_set_background(optset_T *args)
{
    اگر (check_opt_strings(p_bg, p_bg_مقادیر, FALSE) == FAIL)
	بازگشت e_invalid_argument;

    اگر (args->os_oldval.string != پوچ && args->os_oldval.string[0] == *p_bg)
	// Value was not changed
	بازگشت پوچ;

#ifdef FEAT_EVAL
    صحیح dark = (*p_bg == 'd');
#endif

    init_highlight(FALSE, FALSE);

#ifdef FEAT_EVAL
    اگر (dark != (*p_bg == 'd')
	    && get_var_مقدار((char_u *)"g:colors_name") != پوچ)
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

    بازگشت پوچ;
}

    صحیح
expand_set_background(optexpand_T *args, صحیح *رقم_Matches, char_u ***matches)
{
    بازگشت expand_set_opt_string(
	    args,
	    p_bg_مقادیر,
	    ARRAY_LENGTH(p_bg_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}

/*
 * The 'backspace' option is changed.
 */
    نویسه *
did_set_backspace(optset_T *args UNUSED)
{
    اگر (VIM_ISDIGIT(*p_bs))
    {
	اگر (*p_bs > '3' || p_bs[1] != NUL)
	    بازگشت e_invalid_argument;
    }
    وگرنه اگر (check_opt_strings(p_bs, p_bs_مقادیر, TRUE) != OK)
	بازگشت e_invalid_argument;

    بازگشت پوچ;
}

    صحیح
expand_set_backspace(optexpand_T *args, صحیح *رقم_Matches, char_u ***matches)
{
    بازگشت expand_set_opt_string(
	    args,
	    p_bs_مقادیر,
	    ARRAY_LENGTH(p_bs_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}

/*
 * The 'backupcopy' option is changed.
 */
    نویسه *
did_set_backupcopy(optset_T *args)
{
    char_u		*bkc = p_bkc;
    unsigned صحیح	*flags = &bkc_flags;
    نویسه		*errmsg = پوچ;

    اگر (args->os_flags & OPT_LOCAL)
    {
	bkc = curbuf->b_p_bkc;
	flags = &curbuf->b_bkc_flags;
    }
    وگرنه اگر (!(args->os_flags & OPT_GLOBAL))
	// When using :set, clear the local flags.
	curbuf->b_bkc_flags = 0;

    اگر ((args->os_flags & OPT_LOCAL) && *bkc == NUL)
	// make the local value empty: use the global value
	*flags = 0;
    وگرنه
    {
	اگر (opt_strings_flags(bkc, p_bkc_مقادیر, flags, TRUE) != OK)
	    errmsg = e_invalid_argument;
	اگر ((((صحیح)*flags & BKC_AUTO) != 0)
		+ (((صحیح)*flags & BKC_YES) != 0)
		+ (((صحیح)*flags & BKC_NO) != 0) != 1)
	{
	    // Must have exactly one of "auto", "yes"  and "no".
	    (تهی)opt_strings_flags(args->os_oldval.string, p_bkc_مقادیر,
								  flags, TRUE);
	    errmsg = e_invalid_argument;
	}
    }

    بازگشت errmsg;
}

    صحیح
expand_set_backupcopy(optexpand_T *args, صحیح *رقم_Matches, char_u ***matches)
{
    بازگشت expand_set_opt_string(
	    args,
	    p_bkc_مقادیر,
	    ARRAY_LENGTH(p_bkc_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}

/*
 * The 'backupext' or the 'patchmode' option is changed.
 */
    نویسه *
did_set_backupext_or_patchmode(optset_T *args UNUSED)
{
    اگر (STRCMP(*p_bex == '.' ? p_bex + 1 : p_bex,
		*p_pm == '.' ? p_pm + 1 : p_pm) == 0)
	بازگشت e_backupext_and_patchmode_are_equal;

    بازگشت پوچ;
}

/*
 * The 'belloff' option is changed.
 */
    نویسه *
did_set_belloff(optset_T *args UNUSED)
{
    بازگشت did_set_opt_flags(p_bo, p_bo_مقادیر, &bo_flags, TRUE);
}

    صحیح
expand_set_belloff(optexpand_T *args, صحیح *رقم_Matches, char_u ***matches)
{
    بازگشت expand_set_opt_string(
	    args,
	    p_bo_مقادیر,
	    ARRAY_LENGTH(p_bo_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}

#اگر defined(FEAT_LINEBREAK)
/*
 * The 'breakat' option is changed.
 */
    نویسه *
did_set_breakat(optset_T *args UNUSED)
{
    char_u	*p;
    صحیح		i;

    برای (i = 0; i < 256; i++)
	breakat_flags[i] = FALSE;

    اگر (p_breakat != پوچ)
	برای (p = p_breakat; *p; p++)
	    breakat_flags[*p] = TRUE;

    بازگشت پوچ;
}

/*
 * The 'breakindentopt' option is changed.
 */
    نویسه *
did_set_breakindentopt(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    اگر (briopt_check(*varp, varp == &curwin->w_p_briopt ? curwin : پوچ)
								      == FAIL)
	بازگشت e_invalid_argument;

    // list setting requires a redraw
    اگر (varp == &curwin->w_p_briopt && curwin->w_briopt_list)
	redraw_all_later(UPD_NOT_VALID);

    بازگشت پوچ;
}

    صحیح
expand_set_breakindentopt(
    optexpand_T		*args,
    صحیح			*رقم_Matches,
    char_u		***matches)
{
    بازگشت expand_set_opt_string(
	    args,
	    p_briopt_مقادیر,
	    ARRAY_LENGTH(p_briopt_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}
#endif

#اگر defined(FEAT_BROWSE)
/*
 * The 'browsedir' option is changed.
 */
    نویسه *
did_set_browsedir(optset_T *args UNUSED)
{
    اگر (check_opt_strings(p_bsdir, p_bsdir_مقادیر, FALSE) != OK
	    && !mch_isdir(p_bsdir))
	بازگشت e_invalid_argument;

    بازگشت پوچ;
}

    صحیح
expand_set_browsedir(optexpand_T *args, صحیح *رقم_Matches, char_u ***matches)
{
    بازگشت expand_set_opt_string(
	    args,
	    p_bsdir_مقادیر,
	    ARRAY_LENGTH(p_bsdir_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}
#endif

/*
 * The 'bufhidden' option is changed.
 */
    نویسه *
did_set_bufhidden(optset_T *args UNUSED)
{
    بازگشت did_set_opt_strings(curbuf->b_p_bh, p_bufhidden_مقادیر, FALSE);
}

    صحیح
expand_set_bufhidden(optexpand_T *args, صحیح *رقم_Matches, char_u ***matches)
{
    بازگشت expand_set_opt_string(
	    args,
	    p_bufhidden_مقادیر,
	    ARRAY_LENGTH(p_bufhidden_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}

/*
 * The 'buftype' option is changed.
 */
    نویسه *
did_set_buftype(optset_T *args UNUSED)
{
    اگر (check_opt_strings(curbuf->b_p_bt, p_buftype_مقادیر, FALSE) != OK)
	بازگشت e_invalid_argument;

    اگر (curwin->w_status_height)
    {
	curwin->w_redr_status = true;
	redraw_later(UPD_VALID);
    }
    curbuf->b_help = (curbuf->b_p_bt[0] == 'h');
    redraw_titles();

    بازگشت پوچ;
}

    صحیح
expand_set_buftype(optexpand_T *args, صحیح *رقم_Matches, char_u ***matches)
{
    بازگشت expand_set_opt_string(
	    args,
	    p_buftype_مقادیر,
	    ARRAY_LENGTH(p_buftype_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}

/*
 * The 'casemap' option is changed.
 */
    نویسه *
did_set_casemap(optset_T *args UNUSED)
{
    بازگشت did_set_opt_flags(p_cmp, p_cmp_مقادیر, &cmp_flags, TRUE);
}

    صحیح
expand_set_casemap(optexpand_T *args, صحیح *رقم_Matches, char_u ***matches)
{
    بازگشت expand_set_opt_string(
	    args,
	    p_cmp_مقادیر,
	    ARRAY_LENGTH(p_cmp_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}

#اگر defined(HAVE_CLIPMETHOD)
    صحیح
expand_set_clipboard(optexpand_T *args, صحیح *رقم_Matches, char_u ***matches)
{
    بازگشت expand_set_opt_string(
	    args,
	    p_cb_مقادیر,
	    ARRAY_LENGTH(p_cb_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}
#endif

#ifdef HAVE_CLIPMETHOD
    نویسه *
did_set_clipmethod(optset_T *args UNUSED)
{
    بازگشت choose_clipmethod();
}

    صحیح
expand_set_clipmethod(optexpand_T *args, صحیح *رقم_Matches, char_u ***matches)
{
    // We want to expand using the predefined clipmethod values + clipboard
    // provider names.
    صحیح		result;
    نویسه	**مقادیر;
    صحیح		شمار, pos = 0, start = 0;
# ifdef FEAT_EVAL
    dict_T	*providers = get_vim_var_dict(VV_CLIPPROVIDERS);
# وگرنه
    dict_T	*providers = پوچ;
# endif
    hashtab_T	*ht = providers == پوچ ? پوچ : &providers->dv_hashtab;

    شمار = (ht == پوچ ? 0 : ht->ht_used);
# ifdef FEAT_WAYLAND_CLIPBOARD
    شمار++;
    start++;
# endif
# ifdef FEAT_XCLIPBOARD
    شمار++;
    start++;
# endif
    مقادیر = ALLOC_MULT(نویسه *, شمار + 1); // Add NULL terminator too

    اگر (مقادیر == پوچ)
	بازگشت FAIL;

# ifdef FEAT_WAYLAND_CLIPBOARD
    مقادیر[pos++] = "wayland";
# endif
# ifdef FEAT_XCLIPBOARD
    مقادیر[pos++] = "x11";
# endif

    اگر (ht != پوچ)
	برای (long_u i = 0; i < ht->ht_mask + 1; i++)
	{
	    hashitem_T	*hi = ht->ht_array + i;

	    اگر (!HASHITEM_EMPTY(hi))
		مقادیر[pos++] = (نویسه *)vim_strsave(hi->hi_کلید);
	}
    مقادیر[pos++] = پوچ;

    result = expand_set_opt_string(
	    args,
	    مقادیر,
	    شمار,
	    رقم_Matches,
	    matches);

    برای (صحیح i = start; i < شمار; i++)
	vim_free(مقادیر[i]);
    vim_free(مقادیر);

    بازگشت result;
}
#endif

/*
 * The global 'listchars' or 'fillchars' option is changed.
 */
    ایستا نویسه *
did_set_global_listfillchars(char_u *val, صحیح opt_lcs, صحیح opt_flags,
						نویسه *errbuf, size_t errbuflen)
{
    نویسه	*errmsg = پوچ;
    char_u	**local_ptr = opt_lcs ? &curwin->w_p_lcs : &curwin->w_p_fcs;

    // only apply the global value to "curwin" when it does not have a
    // local value
    اگر (opt_lcs)
	errmsg = set_listchars_option(curwin, val,
		**local_ptr == NUL || !(opt_flags & OPT_GLOBAL),
							    errbuf, errbuflen);
    وگرنه
	errmsg = set_fillchars_option(curwin, val,
		**local_ptr == NUL || !(opt_flags & OPT_GLOBAL),
							    errbuf, errbuflen);
    اگر (errmsg != پوچ)
	بازگشت errmsg;

    tabpage_T	*tp;
    win_T	*wp;

    // If the current window is set to use the global
    // 'listchars'/'fillchars' value, clear the window-local value.
    اگر (!(opt_flags & OPT_GLOBAL))
	clear_string_option(local_ptr);
    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	// If the current window has a local value need to apply it
	// again, it was changed when setting the global value.
	// If no error was returned above, we don't expect an error
	// here, so ignore the return value.
	اگر (opt_lcs)
	{
	    اگر (*wp->w_p_lcs == NUL)
		(تهی)set_listchars_option(wp, wp->w_p_lcs, TRUE, پوچ, 0);
	}
	وگرنه
	{
	    اگر (*wp->w_p_fcs == NUL)
		(تهی)set_fillchars_option(wp, wp->w_p_fcs, TRUE, پوچ, 0);
	}
    }

    redraw_all_later(UPD_NOT_VALID);

    بازگشت پوچ;
}

/*
 * The 'fillchars' option or the 'listchars' option is changed.
 */
    نویسه *
did_set_chars_option(optset_T *args)
{
    char_u **varp = (char_u **)args->os_varp;
    نویسه *errmsg = پوچ;

    اگر (   varp == &p_lcs		// global 'listchars'
	|| varp == &p_fcs)		// global 'fillchars'
	errmsg = did_set_global_listfillchars(*varp, varp == &p_lcs,
			  args->os_flags, args->os_errbuf, args->os_errbuflen);
    وگرنه اگر (varp == &curwin->w_p_lcs)	// local 'listchars'
	errmsg = set_listchars_option(curwin, *varp, TRUE,
					  args->os_errbuf, args->os_errbuflen);
    وگرنه اگر (varp == &curwin->w_p_fcs)	// local 'fillchars'
	errmsg = set_fillchars_option(curwin, *varp, TRUE,
					  args->os_errbuf, args->os_errbuflen);

    بازگشت errmsg;
}

/*
 * Expand 'fillchars' or 'listchars' option value.
 */
    صحیح
expand_set_chars_option(optexpand_T *args, صحیح *رقم_Matches, char_u ***matches)
{
    char_u **varp = (char_u **)args->oe_varp;
    صحیح is_lcs = (varp == &p_lcs || varp == &curwin->w_p_lcs);
    بازگشت expand_set_opt_generic(
	    args,
	    is_lcs ? get_listchars_name : get_fillchars_name,
	    رقم_Matches,
	    matches);
}

/*
 * The 'cinoptions' option is changed.
 */
    نویسه *
did_set_cinoptions(optset_T *args UNUSED)
{
    // TODO: recognize errors
    parse_cino(curbuf);

    بازگشت پوچ;
}

#اگر defined(FEAT_SYN_HL)
/*
 * The 'colorcolumn' option is changed.
 */
    نویسه *
did_set_colorcolumn(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    بازگشت check_colorcolumn(*varp, varp == &curwin->w_p_cc ? curwin : پوچ);
}
#endif

/*
 * The 'comments' option is changed.
 */
    نویسه *
did_set_comments(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u	*s;
    نویسه	*errmsg = پوچ;

    برای (s = *varp; *s; )
    {
	وقتی (*s && *s != ':')
	{
	    اگر (vim_strchr((char_u *)COM_ALL, *s) == پوچ
		    && !VIM_ISDIGIT(*s) && *s != '-')
	    {
		errmsg = illegal_char(args->os_errbuf, args->os_errbuflen, *s);
		بشکن;
	    }
	    ++s;
	}
	اگر (*s++ == NUL)
	    errmsg = e_missing_colon;
	وگرنه اگر (*s == ',' || *s == NUL)
	    errmsg = e_zero_length_string;
	اگر (errmsg != پوچ)
	    بشکن;
	وقتی (*s && *s != ',')
	{
	    اگر (*s == '\\' && s[1] != NUL)
		++s;
	    ++s;
	}
	s = skip_to_option_part(s);
    }

    بازگشت errmsg;
}

/*
 * The 'commentstring' option is changed.
 */
    نویسه *
did_set_commentstring(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    اگر (**varp != NUL && strstr((نویسه *)*varp, "%s") == پوچ)
	بازگشت e_commentstring_must_be_empty_or_contain_str;

    بازگشت پوچ;
}

/*
 * Check if value for 'complete' is valid when 'complete' option is changed.
 */
    نویسه *
did_set_complete(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u	*p, *t;
    char_u	buffer[LSIZE];
    char_u	*buf_ptr;
    char_u	char_before = NUL;
    صحیح		escape;

    برای (p = *varp; *p; )
    {
	vim_memset(buffer, 0, LSIZE);
	buf_ptr = buffer;
	escape = 0;

	// Extract substring while handling escaped commas
	وقتی (*p && (*p != ',' || escape) && buf_ptr < (buffer + LSIZE - 1))
	{
	    اگر (*p == '\\' && *(p + 1) == ',')
	    {
		escape = 1;  // Mark escape mode
		p++;         // Skip '\'
	    }
	    وگرنه
	    {
		escape = 0;
		*buf_ptr++ = *p;
	    }
	    p++;
	}
	*buf_ptr = NUL;

	اگر (vim_strchr((char_u *)".wbuksid]tUFo", *buffer) == پوچ)
	    بازگشت illegal_char(args->os_errbuf, args->os_errbuflen, *buffer);

	اگر (vim_strchr((char_u *)"ksF", *buffer) == پوچ && *(buffer + 1) != NUL
		&& *(buffer + 1) != '^')
	    char_before = *buffer;
	وگرنه
	{
	    // Test for a number after '^'
	    اگر ((t = vim_strchr(buffer, '^')) != پوچ)
	    {
		*t++ = NUL;
		اگر (!*t)
		    char_before = '^';
		وگرنه
		{
		    برای (; *t; t++)
		    {
			اگر (!vim_isdigit(*t))
			{
			    char_before = '^';
			    بشکن;
			}
		    }
		}
	    }
	}
	اگر (char_before != NUL)
	    بازگشت illegal_char_after_chr(args->os_errbuf, args->os_errbuflen,
		    char_before);
	// Skip comma and spaces
	وقتی (*p == ',' || *p == ' ')
	    p++;
    }

#ifdef FEAT_COMPL_FUNC
    اگر (set_cpt_callbacks(args) != OK)
	بازگشت illegal_char_after_chr(args->os_errbuf, args->os_errbuflen,
		'F');
#endif
    بازگشت پوچ;
}

    صحیح
expand_set_complete(optexpand_T *args, صحیح *رقم_Matches, char_u ***matches)
{
    ایستا نویسه *(p_cpt_مقادیر[]) = {
	".", "w", "b", "u", "k", "kspell", "s", "i", "d", "]", "t", "U", "F", "o",
	پوچ};
    بازگشت expand_set_opt_string(
	    args,
	    p_cpt_مقادیر,
	    ARRAY_LENGTH(p_cpt_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}

/*
 * The 'completeopt' option is changed.
 */
    نویسه *
did_set_completeopt(optset_T *args UNUSED)
{
    char_u	*cot = p_cot;
    unsigned	*flags = &cot_flags;

    اگر (args->os_flags & OPT_LOCAL)
    {
	cot = curbuf->b_p_cot;
	flags = &curbuf->b_cot_flags;
    }
    وگرنه اگر (!(args->os_flags & OPT_GLOBAL))
	// When using :set, clear the local flags.
	curbuf->b_cot_flags = 0;

    اگر (opt_strings_flags(cot, p_cot_مقادیر, flags, TRUE) != OK)
	بازگشت e_invalid_argument;

    بازگشت پوچ;
}

    صحیح
expand_set_completeopt(optexpand_T *args, صحیح *رقم_Matches, char_u ***matches)
{
    بازگشت expand_set_opt_string(
	    args,
	    p_cot_مقادیر,
	    ARRAY_LENGTH(p_cot_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}

/*
 * The 'completefuzzycollect' option is changed.
 */
    نویسه *
did_set_completefuzzycollect(optset_T *args UNUSED)
{
    اگر (opt_strings_flags(p_cfc, p_cfc_مقادیر, &cfc_flags, TRUE) != OK)
	بازگشت e_invalid_argument;
    بازگشت پوچ;
}

    صحیح
expand_set_completefuzzycollect(
	optexpand_T *args,
	صحیح *رقم_Matches,
	char_u ***matches)
{
    بازگشت expand_set_opt_string(
	    args,
	    p_cfc_مقادیر,
	    ARRAY_LENGTH(p_cfc_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}

/*
 * The 'completeitemalign' option is changed.
 */
    نویسه *
did_set_completeitemalign(optset_T *args UNUSED)
{
    char_u	*p = p_cia;
    unsigned	new_cia_flags = 0;
    صحیح		seen[3] = { FALSE, FALSE, FALSE };
    صحیح		شمار = 0;
    char_u	buf[10];

    وقتی (*p)
    {
	copy_option_part(&p, buf, اندازه(buf), ",");
	اگر (شمار >= 3)
	    بازگشت e_invalid_argument;

	اگر (STRCMP(buf, "abbr") == 0)
	{
	    اگر (seen[CPT_ABBR])
		بازگشت e_invalid_argument;
	    new_cia_flags = new_cia_flags * 10 + CPT_ABBR;
	    seen[CPT_ABBR] = TRUE;
	    شمار++;
	}
	وگرنه اگر (STRCMP(buf, "kind") == 0)
	{
	    اگر (seen[CPT_KIND])
		بازگشت e_invalid_argument;
	    new_cia_flags = new_cia_flags * 10 + CPT_KIND;
	    seen[CPT_KIND] = TRUE;
	    شمار++;
	}
	وگرنه اگر (STRCMP(buf, "menu") == 0)
	{
	    اگر (seen[CPT_MENU])
		بازگشت e_invalid_argument;
	    new_cia_flags = new_cia_flags * 10 + CPT_MENU;
	    seen[CPT_MENU] = TRUE;
	    شمار++;
	}
	وگرنه
	    بازگشت e_invalid_argument;
    }
    اگر (new_cia_flags == 0 || شمار != 3)
	بازگشت e_invalid_argument;

    cia_flags = new_cia_flags;
    بازگشت پوچ;
}

#اگر defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
/*
 * The 'completepopup' option is changed.
 */
    نویسه *
did_set_completepopup(optset_T *args UNUSED)
{
    اگر (parse_completepopup(پوچ) == FAIL)
	بازگشت e_invalid_argument;

    popup_close_info();
    بازگشت پوچ;
}
#endif

#اگر defined(BACKSLASH_IN_FILENAME)
/*
 * The 'completeslash' option is changed.
 */
    نویسه *
did_set_completeslash(optset_T *args UNUSED)
{
    اگر (check_opt_strings(p_csl, p_csl_مقادیر, FALSE) != OK
	    || check_opt_strings(curbuf->b_p_csl, p_csl_مقادیر, FALSE) != OK)
	بازگشت e_invalid_argument;

    بازگشت پوچ;
}

    صحیح
expand_set_completeslash(optexpand_T *args, صحیح *رقم_Matches, char_u ***matches)
{
    بازگشت expand_set_opt_string(
	    args,
	    p_csl_مقادیر,
	    ARRAY_LENGTH(p_csl_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}
#endif

#اگر defined(FEAT_CONCEAL)
/*
 * The 'concealcursor' option is changed.
 */
    نویسه *
did_set_concealcursor(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    بازگشت did_set_option_listflag(*varp, (char_u *)COCU_ALL, args->os_errbuf,
		    args->os_errbuflen);
}

    صحیح
expand_set_concealcursor(optexpand_T *args, صحیح *رقم_Matches, char_u ***matches)
{
    بازگشت expand_set_opt_listflag(args, (char_u*)COCU_ALL, رقم_Matches, matches);
}
#endif

/*
 * The 'cpoptions' option is changed.
 */
    نویسه *
did_set_cpoptions(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    بازگشت did_set_option_listflag(*varp, (char_u *)CPO_ALL, args->os_errbuf,
		    args->os_errbuflen);
}

    صحیح
expand_set_cpoptions(optexpand_T *args, صحیح *رقم_Matches, char_u ***matches)
{
    بازگشت expand_set_opt_listflag(args, (char_u*)CPO_ALL, رقم_Matches, matches);
}

#اگر defined(FEAT_CRYPT)
/*
 * The 'cryptkey' option is changed.
 */
    نویسه *
did_set_cryptkey(optset_T *args)
{
    // Make sure the ":set" command doesn't show the new value in the
    // history.
    remove_کلید_from_history();

    اگر (args->os_op != OP_NONE)
	// Don't allow set+=/-=/^= as they can allow for substring guessing
	بازگشت e_invalid_argument;

    اگر (STRCMP(curbuf->b_p_کلید, args->os_oldval.string) != 0)
    {
	// Need to update the swapfile.
	ml_set_crypt_کلید(curbuf, args->os_oldval.string,
		*curbuf->b_p_cm == NUL ? p_cm : curbuf->b_p_cm);
	changed_internal();
    }
# ifdef FEAT_SODIUM
    اگر (crypt_method_is_sodium(crypt_get_method_nr(curbuf)))
	crypt_sodium_lock_کلید(args->os_newval.string);
# endif

    بازگشت پوچ;
}

/*
 * The 'cryptmethod' option is changed.
 */
    نویسه *
did_set_cryptmethod(optset_T *args)
{
    char_u  *p;
    char_u  *s;

    اگر (args->os_flags & OPT_LOCAL)
	p = curbuf->b_p_cm;
    وگرنه
	p = p_cm;
    اگر (check_opt_strings(p, p_cm_مقادیر, TRUE) != OK)
	بازگشت e_invalid_argument;
    وگرنه اگر (crypt_self_test() == FAIL)
	بازگشت e_invalid_argument;

    // When setting the global value to empty, make it "zip".
    اگر (*p_cm == NUL)
    {
	free_string_option(p_cm);
	p_cm = vim_strsave((char_u *)"zip");
    }
    // When using ":set cm=name" the local value is going to be empty.
    // Do that here, otherwise the crypt functions will still use the
    // local value.
    اگر ((args->os_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	free_string_option(curbuf->b_p_cm);
	curbuf->b_p_cm = empty_option;
    }

    // Need to update the swapfile when the effective method changed.
    // Set "s" to the effective old value, "p" to the effective new
    // method and compare.
    اگر ((args->os_flags & OPT_LOCAL) && *args->os_oldval.string == NUL)
	s = p_cm;  // was previously using the global value
    وگرنه
	s = args->os_oldval.string;
    اگر (*curbuf->b_p_cm == NUL)
	p = p_cm;  // is now using the global value
    وگرنه
	p = curbuf->b_p_cm;
    اگر (STRCMP(s, p) != 0)
	ml_set_crypt_کلید(curbuf, curbuf->b_p_کلید, s);

    // If the global value changes need to update the swapfile for all
    // buffers using that value.
    اگر ((args->os_flags & OPT_GLOBAL)
	    && STRCMP(p_cm, args->os_oldval.string) != 0)
    {
	buf_T	*buf;

	FOR_ALL_BUFFERS(buf)
	    اگر (buf != curbuf && *buf->b_p_cm == NUL)
		ml_set_crypt_کلید(buf, buf->b_p_کلید, args->os_oldval.string);
    }
    بازگشت پوچ;
}

    صحیح
expand_set_cryptmethod(optexpand_T *args, صحیح *رقم_Matches, char_u ***matches)
{
    بازگشت expand_set_opt_string(
	    args,
	    p_cm_مقادیر,
	    ARRAY_LENGTH(p_cm_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}
#endif

#اگر defined(FEAT_CSCOPE) && defined(FEAT_QUICKFIX)
/*
 * The 'cscopequickfix' option is changed.
 */
    نویسه *
did_set_cscopequickfix(optset_T *args UNUSED)
{
    char_u	*p;

    اگر (p_csqf == پوچ)
	بازگشت پوچ;

    p = p_csqf;
    وقتی (*p != NUL)
    {
	اگر (vim_strchr((char_u *)CSQF_CMDS, *p) == پوچ
		|| p[1] == NUL
		|| vim_strchr((char_u *)CSQF_FLAGS, p[1]) == پوچ
		|| (p[2] != NUL && p[2] != ','))
	    بازگشت e_invalid_argument;
	وگرنه اگر (p[2] == NUL)
	    بشکن;
	وگرنه
	    p += 3;
    }

    بازگشت پوچ;
}
#endif

#اگر defined(FEAT_SYN_HL)
/*
 * The 'cursorlineopt' option is changed.
 */
    نویسه *
did_set_cursorlineopt(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    // This could be changed to use opt_strings_flags() instead.
    اگر (**varp == NUL || fill_culopt_flags(*varp, curwin) != OK)
	بازگشت e_invalid_argument;

    بازگشت پوچ;
}

    صحیح
expand_set_cursorlineopt(optexpand_T *args, صحیح *رقم_Matches, char_u ***matches)
{
    بازگشت expand_set_opt_string(
	    args,
	    p_culopt_مقادیر,
	    ARRAY_LENGTH(p_culopt_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}
#endif

/*
 * The 'debug' option is changed.
 */
    نویسه *
did_set_debug(optset_T *args UNUSED)
{
    بازگشت did_set_opt_strings(p_debug, p_debug_مقادیر, TRUE);
}

    صحیح
expand_set_debug(optexpand_T *args, صحیح *رقم_Matches, char_u ***matches)
{
    بازگشت expand_set_opt_string(
	    args,
	    p_debug_مقادیر,
	    ARRAY_LENGTH(p_debug_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}

#اگر defined(FEAT_DIFF)
/*
 * The 'diffanchors' option is changed.
 */
    نویسه *
did_set_diffanchors(optset_T *args)
{
    اگر (diffanchors_changed(args->os_flags & OPT_LOCAL) == FAIL)
	بازگشت e_invalid_argument;

    بازگشت پوچ;
}

/*
 * The 'diffopt' option is changed.
 */
    نویسه *
did_set_diffopt(optset_T *args UNUSED)
{
    اگر (diffopt_changed() == FAIL)
	بازگشت e_invalid_argument;

    بازگشت پوچ;
}

    صحیح
expand_set_diffopt(optexpand_T *args, صحیح *رقم_Matches, char_u ***matches)
{
    expand_T *xp = args->oe_xp;

    اگر (xp->xp_pattern > args->oe_set_arg && *(xp->xp_pattern-1) == ':')
    {
	اگر (completing_مقدار_for_subopt(args, "algorithm"))
	{
	    بازگشت expand_set_opt_string(
		    args,
		    p_dip_algorithm_مقادیر,
		    ARRAY_LENGTH(p_dip_algorithm_مقادیر) - 1,
		    رقم_Matches,
		    matches);
	}
	اگر (completing_مقدار_for_subopt(args, "inline"))
	{
	    بازگشت expand_set_opt_string(
		    args,
		    p_dip_inline_مقادیر,
		    ARRAY_LENGTH(p_dip_inline_مقادیر) - 1,
		    رقم_Matches,
		    matches);
	}
	بازگشت FAIL;
    }

    بازگشت expand_set_opt_string(
	    args,
	    p_dip_مقادیر,
	    ARRAY_LENGTH(p_dip_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}
#endif

/*
 * The 'display' option is changed.
 */
    نویسه *
did_set_display(optset_T *args UNUSED)
{
    اگر (opt_strings_flags(p_dy, p_dy_مقادیر, &dy_flags, TRUE) != OK)
	بازگشت e_invalid_argument;

    (تهی)init_chartab();
    بازگشت پوچ;
}

    صحیح
expand_set_display(optexpand_T *args, صحیح *رقم_Matches, char_u ***matches)
{
    بازگشت expand_set_opt_string(
	    args,
	    p_dy_مقادیر,
	    ARRAY_LENGTH(p_dy_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}

/*
 * The 'eadirection' option is changed.
 */
    نویسه *
did_set_eadirection(optset_T *args UNUSED)
{
    بازگشت did_set_opt_strings(p_ead, p_ead_مقادیر, FALSE);
}

    صحیح
expand_set_eadirection(optexpand_T *args, صحیح *رقم_Matches, char_u ***matches)
{
    بازگشت expand_set_opt_string(
	    args,
	    p_ead_مقادیر,
	    ARRAY_LENGTH(p_ead_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}

/*
 * One of the 'encoding', 'fileencoding', 'termencoding' or 'makeencoding'
 * options is changed.
 */
    نویسه *
did_set_encoding(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u	**gvarp;
    نویسه	*errmsg = پوچ;
    char_u	*p;

    // Get the global option to compare with, otherwise we would have to check
    // two values for all local options.
    gvarp = (char_u **)get_option_varp_scope(args->os_idx, OPT_GLOBAL);

    اگر (gvarp == &p_fenc)
    {
	اگر (!curbuf->b_p_ma && args->os_flags != OPT_GLOBAL)
	    errmsg = e_cannot_make_changes_modifiable_is_off;
	وگرنه اگر (vim_strchr(*varp, ',') != پوچ)
	    // No comma allowed in 'fileencoding'; catches confusing it
	    // with 'fileencodings'.
	    errmsg = e_invalid_argument;
	وگرنه
	{
	    // May show a "+" in the title now.
	    redraw_titles();
	    // Add 'fileencoding' to the swap file.
	    ml_setflags(curbuf);
	}
    }
    اگر (errmsg == پوچ)
    {
	// canonize the value, so that STRCMP() can be used on it
	p = enc_canonize(*varp);
	اگر (p != پوچ)
	{
	    vim_free(*varp);
	    *varp = p;
	}
	اگر (varp == &p_enc)
	{
	    errmsg = mb_init();
	    redraw_titles();
	}
    }

#اگر defined(FEAT_GUI_GTK)
    اگر (errmsg == پوچ && varp == &p_tenc && gui.in_use)
    {
	// GTK uses only a single encoding, and that is UTF-8.
	اگر (STRCMP(p_tenc, "utf-8") != 0)
	    errmsg = e_cannot_be_changed_in_gtk_GUI;
    }
#endif

    اگر (errmsg == پوچ)
    {
#ifdef FEAT_KEYMAP
	// When 'keymap' is used and 'encoding' changes, reload the keymap
	// (with another encoding).
	اگر (varp == &p_enc && *curbuf->b_p_keymap != NUL)
	    (تهی)keymap_init();
#endif

	// When 'termencoding' is not empty and 'encoding' changes or when
	// 'termencoding' changes, need to setup for keyboard input and
	// display output conversion.
	اگر (((varp == &p_enc && *p_tenc != NUL) || varp == &p_tenc))
	{
	    اگر (convert_setup(&درون_داد_conv, p_tenc, p_enc) == FAIL
		    || convert_setup(&output_conv, p_enc, p_tenc) == FAIL)
	    {
		semsg(_(e_cannot_convert_between_str_and_str),
			p_tenc, p_enc);
		errmsg = e_invalid_argument;
	    }
	}

#اگر defined(MSWIN)
	// $HOME, $VIM and $VIMRUNTIME may have characters in active code page.
	اگر (varp == &p_enc)
	{
	    init_homedir();
	    init_vimdir();
	}
#endif
    }

    بازگشت errmsg;
}

    صحیح
expand_set_encoding(optexpand_T *args, صحیح *رقم_Matches, char_u ***matches)
{
    بازگشت expand_set_opt_generic(
	    args,
	    get_encoding_name,
	    رقم_Matches,
	    matches);
}

/*
 * The 'eventignore(win)' option is changed.
 */
    نویسه *
did_set_eventignore(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u	*oldval = args->os_oldval.string;

    اگر (check_ei(*varp) == FAIL)
	بازگشت e_invalid_argument;

    اگر (oldval == پوچ || STRCMP(oldval, *varp) == 0)
	بازگشت پوچ;

    // Deal with the events that are triggered by comparing against a stored
    // state, with the old value in effect: what happened while an event was
    // ignored must not be reported once it is not ignored anymore, and what
    // happened before must still be reported.
    // Use a copy, the caller owns "oldval" and autocommands may free it.
    char_u	*save_ei = vim_strsave(oldval);
    اگر (save_ei != پوچ)
    {
	char_u	*newval = *varp;
	win_T	*wp = is_window_local_option(args->os_idx) ? curwin : پوچ;

	*varp = save_ei;
	// "varp" points into "wp" for 'eventignorewin'.
	اگر (wp != پوچ)
	    ++wp->w_locked;
	may_trigger_deferred_events();
	اگر (wp != پوچ)
	    --wp->w_locked;

	free_string_option(*varp);
	*varp = newval;
    }

    بازگشت پوچ;
}

ایستا صحیح expand_eiw = FALSE;

    ایستا char_u *
get_eventignore_name(expand_T *xp, صحیح idx)
{
    صحیح subtract = *xp->xp_pattern == '-';
    // 'eventignore(win)' allows special keyword "all" in addition to
    // all event names.
    اگر (!subtract && idx == 0)
	بازگشت (char_u *)"all";

    char_u *name = get_event_name_no_group(xp, idx - 1 + subtract, expand_eiw);
    اگر (name == پوچ)
	بازگشت پوچ;

    sprintf((نویسه *)IObuff, "%s%s", subtract ? "-" : "", name);
    بازگشت IObuff;
}

    صحیح
expand_set_eventignore(optexpand_T *args, صحیح *رقم_Matches, char_u ***matches)
{
    expand_eiw = args->oe_varp != (char_u *)&p_ei;
    بازگشت expand_set_opt_generic(
	    args,
	    get_eventignore_name,
	    رقم_Matches,
	    matches);
}

/*
 * The 'fileformat' option is changed.
 */
    نویسه *
did_set_fileformat(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    اگر (!curbuf->b_p_ma && !(args->os_flags & OPT_GLOBAL))
	بازگشت e_cannot_make_changes_modifiable_is_off;
    وگرنه اگر (check_opt_strings(*varp, p_ff_مقادیر, FALSE) != OK)
	بازگشت e_invalid_argument;

    // may also change 'textmode'
    اگر (get_fileformat(curbuf) == EOL_DOS)
	curbuf->b_p_tx = TRUE;
    وگرنه
	curbuf->b_p_tx = FALSE;
    redraw_titles();
    // update flag in swap file
    ml_setflags(curbuf);
    // Redraw needed when switching to/from "mac": a CR in the text
    // will be displayed differently.
    اگر (get_fileformat(curbuf) == EOL_MAC || *args->os_oldval.string == 'm')
	redraw_curbuf_later(UPD_NOT_VALID);

    بازگشت پوچ;
}

    صحیح
expand_set_fileformat(optexpand_T *args, صحیح *رقم_Matches, char_u ***matches)
{
    بازگشت expand_set_opt_string(
	    args,
	    p_ff_مقادیر,
	    ARRAY_LENGTH(p_ff_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * fileformat options.
 */
    char_u *
get_fileformat_name(expand_T *xp UNUSED, صحیح idx)
{
    اگر (idx >= (صحیح)ARRAY_LENGTH(p_ff_مقادیر))
	بازگشت پوچ;

    بازگشت (char_u*)p_ff_مقادیر[idx];
}

/*
 * The 'fileformats' option is changed.
 */
    نویسه *
did_set_fileformats(optset_T *args UNUSED)
{
    اگر (check_opt_strings(p_ffs, p_ff_مقادیر, TRUE) != OK)
	بازگشت e_invalid_argument;

    // also change 'textauto'
    اگر (*p_ffs == NUL)
	p_ta = FALSE;
    وگرنه
	p_ta = TRUE;

    بازگشت پوچ;
}

/*
 * The 'filetype' or the 'syntax' option is changed.
 */
    نویسه *
did_set_filetype_or_syntax(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    اگر (!valid_filetype(*varp))
	بازگشت e_invalid_argument;

    args->os_مقدار_changed = STRCMP(args->os_oldval.string, *varp) != 0;

    // Since we check the value, there is no need to set P_INSECURE,
    // even when the value comes from a modeline.
    args->os_مقدار_checked = TRUE;

    بازگشت پوچ;
}

#اگر defined(FEAT_FOLDING)
/*
 * The 'foldclose' option is changed.
 */
    نویسه *
did_set_foldclose(optset_T *args UNUSED)
{
    بازگشت did_set_opt_strings(p_fcl, p_fcl_مقادیر, TRUE);
}

    صحیح
expand_set_foldclose(optexpand_T *args, صحیح *رقم_Matches, char_u ***matches)
{
    بازگشت expand_set_opt_string(
	    args,
	    p_fcl_مقادیر,
	    ARRAY_LENGTH(p_fcl_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}
#endif

#اگر defined(FEAT_EVAL) && defined(FEAT_FOLDING)
/*
 * The 'foldexpr' option is changed.
 */
    نویسه *
did_set_foldexpr(optset_T *args)
{
    (تهی)did_set_optexpr(args);
    اگر (foldmethod_Is_عبارت(curwin))
	foldUpdateAll(curwin);
    بازگشت پوچ;
}
#endif

#اگر defined(FEAT_FOLDING)
/*
 * The 'foldignore' option is changed.
 */
    نویسه *
did_set_foldignore(optset_T *args UNUSED)
{
    اگر (foldmethodIsIndent(curwin))
	foldUpdateAll(curwin);
    بازگشت پوچ;
}

/*
 * The 'foldmarker' option is changed.
 */
    نویسه *
did_set_foldmarker(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u	*p;

    p = vim_strchr(*varp, ',');
    اگر (p == پوچ)
	بازگشت e_comma_required;
    وگرنه اگر (p == *varp || p[1] == NUL)
	بازگشت e_invalid_argument;
    وگرنه اگر (foldmethodIsMarker(curwin))
	foldUpdateAll(curwin);

    بازگشت پوچ;
}

/*
 * The 'foldmethod' option is changed.
 */
    نویسه *
did_set_foldmethod(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    اگر (check_opt_strings(*varp, p_fdm_مقادیر, FALSE) != OK || **varp == NUL)
	بازگشت e_invalid_argument;

    foldUpdateAll(curwin);
    اگر (foldmethodIsDiff(curwin))
	newFoldLevel();
    بازگشت پوچ;
}

    صحیح
expand_set_foldmethod(optexpand_T *args, صحیح *رقم_Matches, char_u ***matches)
{
    بازگشت expand_set_opt_string(
	    args,
	    p_fdm_مقادیر,
	    ARRAY_LENGTH(p_fdm_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}

/*
 * The 'foldopen' option is changed.
 */
    نویسه *
did_set_foldopen(optset_T *args UNUSED)
{
    بازگشت did_set_opt_flags(p_fdo, p_fdo_مقادیر, &fdo_flags, TRUE);
}

    صحیح
expand_set_foldopen(optexpand_T *args, صحیح *رقم_Matches, char_u ***matches)
{
    بازگشت expand_set_opt_string(
	    args,
	    p_fdo_مقادیر,
	    ARRAY_LENGTH(p_fdo_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}
#endif

/*
 * The 'formatoptions' option is changed.
 */
    نویسه *
did_set_formatoptions(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    بازگشت did_set_option_listflag(*varp, (char_u *)FO_ALL, args->os_errbuf,
		    args->os_errbuflen);
}

    صحیح
expand_set_formatoptions(optexpand_T *args, صحیح *رقم_Matches, char_u ***matches)
{
    بازگشت expand_set_opt_listflag(args, (char_u*)FO_ALL, رقم_Matches, matches);
}

#اگر defined(CURSOR_SHAPE)
/*
 * The 'guicursor' option is changed.
 */
    نویسه *
did_set_guicursor(optset_T *args UNUSED)
{
    بازگشت parse_shape_opt(SHAPE_CURSOR);
}
#endif

#اگر defined(FEAT_GUI)
/*
 * The 'guifont' option is changed.
 */
    نویسه *
did_set_guifont(optset_T *args UNUSED)
{
    char_u	*p;
    نویسه	*errmsg = پوچ;

    اگر (gui.in_use)
    {
	p = p_guifont;
# اگر defined(FEAT_GUI_GTK)
	// Put up a font dialog and let the user select a new value.
	// If this is cancelled go back to the old value but don't
	// give an error message.
	اگر (STRCMP(p, "*") == 0)
	{
	    p = gui_mch_font_dialog(args->os_oldval.string);
	    free_string_option(p_guifont);
	    p_guifont = (p != پوچ) ? p : vim_strsave(args->os_oldval.string);
	}
# endif
	اگر (p != پوچ && gui_init_font(p_guifont, FALSE) != OK)
	{
# اگر defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_PHOTON)
	    اگر (STRCMP(p_guifont, "*") == 0)
	    {
		// Dialog was cancelled: Keep the old value without giving
		// an error message.
		free_string_option(p_guifont);
		p_guifont = vim_strsave(args->os_oldval.string);
	    }
	    وگرنه
# endif
		errmsg = e_invalid_fonts;
	}
    }

    بازگشت errmsg;
}

/*
 * Expand the 'guifont' option. Only when GUI is being used. Each platform has
 * specific behaviors.
 */
    صحیح
expand_set_guifont(optexpand_T *args, صحیح *رقم_Matches, char_u ***matches)
{
    اگر (!gui.in_use)
	بازگشت FAIL;

# اگر defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
    char_u **varp = (char_u **)args->oe_varp;
    صحیح wide = (varp == &p_guifontwide);

    بازگشت expand_set_opt_callback(
	    args, gui_mch_expand_font, &wide, رقم_Matches, matches);
# وگرنه
    بازگشت FAIL;
# endif
}

# اگر defined(FEAT_XFONTSET)
/*
 * The 'guifontset' option is changed.
 */
    نویسه *
did_set_guifontset(optset_T *args UNUSED)
{
    نویسه *errmsg = پوچ;

    اگر (STRCMP(p_guifontset, "*") == 0)
	errmsg = e_cant_select_fontset;
    وگرنه اگر (gui.in_use && gui_init_font(p_guifontset, TRUE) != OK)
	errmsg = e_invalid_fontset;

    بازگشت errmsg;
}
# endif

/*
 * The 'guifontwide' option is changed.
 */
    نویسه *
did_set_guifontwide(optset_T *args UNUSED)
{
    نویسه *errmsg = پوچ;

    اگر (STRCMP(p_guifontwide, "*") == 0)
	errmsg = e_cant_select_wide_font;
    وگرنه اگر (gui_get_wide_font() == FAIL)
	errmsg = e_invalid_wide_font;

    بازگشت errmsg;
}
#endif

#اگر defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
/*
 * The 'guiligatures' option is changed.
 */
    نویسه *
did_set_guiligatures(optset_T *args UNUSED)
{
    gui_set_ligatures();
    بازگشت پوچ;
}
#endif

#اگر defined(FEAT_GUI)
/*
 * The 'guioptions' option is changed.
 */
    نویسه *
did_set_guioptions(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    نویسه *errmsg;

    errmsg = did_set_option_listflag(*varp, (char_u *)GO_ALL, args->os_errbuf,
		    args->os_errbuflen);
    اگر (errmsg != پوچ)
	بازگشت errmsg;

    gui_init_which_components(args->os_oldval.string);
    بازگشت پوچ;
}

    صحیح
expand_set_guioptions(optexpand_T *args, صحیح *رقم_Matches, char_u ***matches)
{
    بازگشت expand_set_opt_listflag(args, (char_u*)GO_ALL, رقم_Matches, matches);
}
#endif

#اگر defined(FEAT_GUI_TABLINE)
/*
 * The 'guitablabel' option is changed.
 */
    نویسه *
did_set_guitablabel(optset_T *args UNUSED)
{
    redraw_tabline = TRUE;
    بازگشت پوچ;
}
#endif

/*
 * The 'helpfile' option is changed.
 */
    نویسه *
did_set_helpfile(optset_T *args UNUSED)
{
    // May compute new values for $VIM and $VIMRUNTIME
    اگر (didset_vim)
	vim_unsetenv_ext((char_u *)"VIM");
    اگر (didset_vimruntime)
	vim_unsetenv_ext((char_u *)"VIMRUNTIME");
    بازگشت پوچ;
}

#اگر defined(FEAT_MULTI_LANG)
/*
 * The 'helplang' option is changed.
 */
    نویسه *
did_set_helplang(optset_T *args UNUSED)
{
    نویسه *errmsg = پوچ;

    // Check for "", "ab", "ab,cd", etc.
    برای (char_u *s = p_hlg; *s != NUL; s += 3)
    {
	اگر (s[1] == NUL || ((s[2] != ',' || s[3] == NUL) && s[2] != NUL))
	{
	    errmsg = e_invalid_argument;
	    بشکن;
	}
	اگر (s[2] == NUL)
	    بشکن;
    }

    بازگشت errmsg;
}
#endif

/*
 * The 'highlight' option is changed.
 */
    نویسه *
did_set_highlight(optset_T *args UNUSED)
{
    اگر (highlight_changed() == FAIL)
	بازگشت e_invalid_argument;	// invalid flags

    بازگشت پوچ;
}

    ایستا صحیح
expand_hl_occasions(
	optexpand_T *args,
	صحیح *رقم_Matches,
	char_u ***matches,
	char_u prefix)
{
    char_u	    *p;
    ایستا char_u   hl_flags[HLF_شمار] = HL_FLAGS;
    size_t	    i;
    صحیح		    شمار = 0;

    *matches = ALLOC_MULT(char_u *, HLF_شمار + 1);
    اگر (*matches == پوچ)
	بازگشت FAIL;

    // We still want to return the full option if it's requested.
    اگر (args->oe_include_orig_val)
    {
	p = vim_strsave(args->oe_opt_مقدار);
	اگر (p == پوچ)
	{
	    VIM_CLEAR(*matches);
	    بازگشت FAIL;
	}
	(*matches)[شمار++] = p;
    }

    برای (i = 0; i < HLF_شمار; i++)
    {
	p = alloc((prefix == NUL ? 1 : 2) + 1);
	اگر (p == پوچ)
	{
	    اگر (شمار == 0)
	    {
		VIM_CLEAR(*matches);
		بازگشت FAIL;
	    }
	    وگرنه
		بشکن;
	}
	اگر (prefix == NUL)
	    sprintf((نویسه *)p, "%c", hl_flags[i]);
	وگرنه
	    sprintf((نویسه *)p, "%c%c", prefix, hl_flags[i]);
	(*matches)[شمار++] = p;
    }

    اگر (شمار == 0)
    {
	VIM_CLEAR(*matches);
	بازگشت FAIL;
    }
    *رقم_Matches = شمار;
    بازگشت OK;
}

/*
 * Expand 'highlight' option.
 */
    صحیح
expand_set_highlight(optexpand_T *args, صحیح *رقم_Matches, char_u ***matches)
{
    char_u	    *p;
    expand_T	    *xp = args->oe_xp;
    size_t	    i;
    صحیح		    شمار = 0;

    اگر (xp->xp_pattern > args->oe_set_arg && *(xp->xp_pattern-1) == ':')
    {
	// Right after a ':', meaning we just return all highlight names.
	بازگشت expand_set_opt_generic(
		args,
		get_highlight_name,
		رقم_Matches,
		matches);
    }

    اگر (*xp->xp_pattern == NUL)
	// At beginning of a comma-separated list. Return the specific list of
	// supported occasions.
	بازگشت expand_hl_occasions(args, رقم_Matches, matches, NUL);

    // We are after the initial character (which indicates the occasion). We
    // already made sure we are not matching after a ':' above, so now we want
    // to match against display mode modifiers.
    // Since the xp_pattern starts from the beginning, we need to include it in
    // the returned match.

    // Note: Keep this in sync with highlight_changed()
    ایستا char_u p_hl_mode_مقادیر[] =
	{':', 'b', 'i', '-', 'n', 'r', 's', 'u', 'c', '2', 'd', '=', 't'};
    size_t رقم_hl_modes = ARRAY_LENGTH(p_hl_mode_مقادیر);

    *matches = ALLOC_MULT(char_u *, رقم_hl_modes);
    اگر (*matches == پوچ)
	بازگشت FAIL;

    صحیح pattern_len = xp->xp_pattern_len;

    برای (i = 0; i < رقم_hl_modes; i++)
    {
	// Don't allow duplicates as these are unique flags
	char_u *dup = vim_strchr(xp->xp_pattern + 1, p_hl_mode_مقادیر[i]);
	اگر (dup != پوچ && (صحیح)(dup - xp->xp_pattern) < pattern_len)
	    ادامه;

	// ':' only works by itself, not with other flags.
	اگر (pattern_len > 1 && p_hl_mode_مقادیر[i] == ':')
	    ادامه;

	p = vim_strnsave(xp->xp_pattern, pattern_len + 1);
	اگر (p == پوچ)
	{
	    اگر (i == 0)
	    {
		VIM_CLEAR(*matches);
		بازگشت FAIL;
	    }
	    وگرنه
		بشکن;
	}
	p[pattern_len] = p_hl_mode_مقادیر[i];
	p[pattern_len + 1] = NUL;
	(*matches)[شمار++] = p;
    }
    اگر (شمار == 0)
    {
	VIM_CLEAR(*matches);
	بازگشت FAIL;
    }
    *رقم_Matches = شمار;

    بازگشت OK;
}

/*
 * The 'titlestring' or the 'iconstring' option is changed.
 */
    ایستا نویسه *
parse_titleiconstring(optset_T *args UNUSED, صحیح flagval UNUSED)
{
#ifdef FEAT_STL_OPT
    char_u	**varp = (char_u **)args->os_varp;

    // NULL => statusline syntax
    اگر (vim_strchr(*varp, '%') && check_stl_option(*varp) == پوچ)
	stl_syntax |= flagval;
    وگرنه
	stl_syntax &= ~flagval;
#endif
    did_set_title();

    بازگشت پوچ;
}

/*
 * The 'iconstring' option is changed.
 */
    نویسه *
did_set_iconstring(optset_T *args)
{
    صحیح flagval = 0;

#ifdef FEAT_STL_OPT
    flagval = STL_IN_ICON;
#endif

    بازگشت parse_titleiconstring(args, flagval);
}

#اگر defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
/*
 * The 'imactivatekey' option is changed.
 */
    نویسه *
did_set_imactivatekey(optset_T *args UNUSED)
{
    اگر (!im_xim_isvalid_imactivate())
	بازگشت e_invalid_argument;
    بازگشت پوچ;
}
#endif

#ifdef FEAT_IMAGE
/*
 * The 'imageprotocol' option is changed.
 */
    نویسه *
did_set_imageprotocol(optset_T *args UNUSED)
{
    اگر (update_image_backend() == FAIL)
	بازگشت e_invalid_argument;
    بازگشت پوچ;
}

    صحیح
expand_set_imageprotocol(optexpand_T *args, صحیح *رقم_Matches, char_u ***matches)
{
    expand_T *xp = args->oe_xp;

    اگر (xp->xp_pattern > args->oe_set_arg && *(xp->xp_pattern-1) == ':')
    {
	// 'imageprotocol' only has well-defined terms for completion for the
	// protocol part after the colon.
	بازگشت expand_set_opt_string(
		args,
		p_ipc_protocol_مقادیر,
		ARRAY_LENGTH(p_ipc_protocol_مقادیر) - 1,
		رقم_Matches,
		matches);
    }
    // Use expand_set_opt_string instead of returning FAIL so that we can
    // include the original value if args->oe_include_orig_val is set.
    ایستا نویسه *(empty[]) = {پوچ};
    بازگشت expand_set_opt_string(args, empty, 0, رقم_Matches, matches);
}
#endif

/*
 * The 'iskeyword' option is changed.
 */
    نویسه *
did_set_iskeyword(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    اگر (varp == &p_isk)		// only check for global-value
    {
	اگر (check_isopt(*varp) == FAIL)
	    بازگشت e_invalid_argument;
    }
    وگرنه			// fallthrough for local-value
	بازگشت did_set_isopt(args);

    بازگشت پوچ;
}

/*
 * The 'isident' or the 'iskeyword' or the 'isprint' or the 'isfname' option is
 * changed.
 */
    نویسه *
did_set_isopt(optset_T *args)
{
    // 'isident', 'iskeyword', 'isprint' or 'isfname' option: refill g_chartab[]
    // If the new option is invalid, use old value.
    // 'lisp' option: refill g_chartab[] for '-' char.
    اگر (init_chartab() == FAIL)
    {
	args->os_restore_chartab = TRUE;// need to restore the chartab.
	بازگشت e_invalid_argument;	// error in value
    }

    بازگشت پوچ;
}

/*
 * The 'jumpoptions' option is changed.
 */
    نویسه *
did_set_jumpoptions(optset_T *args UNUSED)
{
    اگر (opt_strings_flags(p_jop, p_jop_مقادیر, &jop_flags, TRUE) != OK)
	بازگشت e_invalid_argument;

    بازگشت پوچ;
}

    صحیح
expand_set_jumpoptions(optexpand_T *args, صحیح *رقم_Matches, char_u ***matches)
{
    بازگشت expand_set_opt_string(
	    args,
	    p_jop_مقادیر,
	    ARRAY_LENGTH(p_jop_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}

#اگر defined(FEAT_KEYMAP)
/*
 * The 'keymap' option is changed.
 */
    نویسه *
did_set_keymap(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    نویسه *errmsg = پوچ;

    اگر (!valid_filetype(*varp))
	errmsg = e_invalid_argument;
    وگرنه
    {
	صحیح	    secure_save = secure;

	// Reset the secure flag, since the value of 'keymap' has
	// been checked to be safe.
	secure = 0;

	// load or unload key mapping tables
	errmsg = keymap_init();

	secure = secure_save;

	// Since we check the value, there is no need to set P_INSECURE,
	// even when the value comes from a modeline.
	args->os_مقدار_checked = TRUE;
    }

    اگر (errmsg == پوچ)
    {
	اگر (*curbuf->b_p_keymap != NUL)
	{
	    // Installed a new keymap, switch on using it.
	    curbuf->b_p_iminsert = B_IMODE_LMAP;
	    اگر (curbuf->b_p_imsearch != B_IMODE_USE_درج)
		curbuf->b_p_imsearch = B_IMODE_LMAP;
	}
	وگرنه
	{
	    // Cleared the keymap, may reset 'iminsert' and 'imsearch'.
	    اگر (curbuf->b_p_iminsert == B_IMODE_LMAP)
		curbuf->b_p_iminsert = B_IMODE_NONE;
	    اگر (curbuf->b_p_imsearch == B_IMODE_LMAP)
		curbuf->b_p_imsearch = B_IMODE_USE_درج;
	}
	اگر ((args->os_flags & OPT_LOCAL) == 0)
	{
	    set_iminsert_global();
	    set_imsearch_global();
	}
	status_redraw_curbuf();
    }

    بازگشت errmsg;
}
#endif

/*
 * The 'keymodel' option is changed.
 */
    نویسه *
did_set_keymodel(optset_T *args UNUSED)
{
    اگر (check_opt_strings(p_km, p_km_مقادیر, TRUE) != OK)
	بازگشت e_invalid_argument;

    km_stopsel = (vim_strchr(p_km, 'o') != پوچ);
    km_startsel = (vim_strchr(p_km, 'a') != پوچ);
    بازگشت پوچ;
}

    صحیح
expand_set_keymodel(optexpand_T *args, صحیح *رقم_Matches, char_u ***matches)
{
    بازگشت expand_set_opt_string(
	    args,
	    p_km_مقادیر,
	    ARRAY_LENGTH(p_km_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}

/*
 * The 'keyprotocol' option is changed.
 */
    نویسه *
did_set_keyprotocol(optset_T *args UNUSED)
{
    char_u *term = T_NAME;
    keyprot_T kpc = match_keyprotocol(term);
    اگر (kpc == KEYPROTOCOL_FAIL)
	بازگشت e_invalid_argument;

    apply_keyprotocol(term, kpc);

    بازگشت پوچ;
}

    صحیح
expand_set_keyprotocol(optexpand_T *args, صحیح *رقم_Matches, char_u ***matches)
{
    expand_T *xp = args->oe_xp;
    اگر (xp->xp_pattern > args->oe_set_arg && *(xp->xp_pattern-1) == ':')
    {
	// 'keyprotocol' only has well-defined terms for completion for the
	// protocol part after the colon.
	بازگشت expand_set_opt_string(
		args,
		p_kpc_protocol_مقادیر,
		ARRAY_LENGTH(p_kpc_protocol_مقادیر) - 1,
		رقم_Matches,
		matches);
    }
    // Use expand_set_opt_string instead of returning FAIL so that we can
    // include the original value if args->oe_include_orig_val is set.
    ایستا نویسه *(empty[]) = {پوچ};
    بازگشت expand_set_opt_string(args, empty, 0, رقم_Matches, matches);
}

/*
 * The 'lispoptions' option is changed.
 */
    نویسه *
did_set_lispoptions(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    اگر (**varp != NUL
	    && STRCMP(*varp, "expr:0") != 0
	    && STRCMP(*varp, "expr:1") != 0)
	بازگشت e_invalid_argument;

    بازگشت پوچ;
}

    صحیح
expand_set_lispoptions(optexpand_T *args, صحیح *رقم_Matches, char_u ***matches)
{
    ایستا نویسه *(p_lop_مقادیر[]) = {"expr:0", "expr:1", پوچ};
    بازگشت expand_set_opt_string(
	    args,
	    p_lop_مقادیر,
	    ARRAY_LENGTH(p_lop_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}

/*
 * The 'matchpairs' option is changed.
 */
    نویسه *
did_set_matchpairs(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u	*p;

    اگر (has_mbyte)
    {
	برای (p = *varp; *p != NUL; ++p)
	{
	    صحیح x2 = -1;
	    صحیح x3 = -1;

	    p += mb_ptr2len(p);
	    اگر (*p != NUL)
		x2 = *p++;
	    اگر (*p != NUL)
	    {
		x3 = mb_ptr2char(p);
		p += mb_ptr2len(p);
	    }
	    اگر (x2 != ':' || x3 == -1 || (*p != NUL && *p != ','))
		بازگشت e_invalid_argument;
	    اگر (*p == NUL)
		بشکن;
	}
    }
    وگرنه
    {
	// Check for "x:y,x:y"
	برای (p = *varp; *p != NUL; p += 4)
	{
	    اگر (p[1] != ':' || p[2] == NUL || (p[3] != NUL && p[3] != ','))
		بازگشت e_invalid_argument;
	    اگر (p[3] == NUL)
		بشکن;
	}
    }

    بازگشت پوچ;
}

/*
 * Process the updated 'messagesopt' option value.
 */
    نویسه *
did_set_messagesopt(optset_T *args UNUSED)
{
    اگر (messagesopt_changed() == FAIL)
	بازگشت e_invalid_argument;

    بازگشت پوچ;
}

    صحیح
expand_set_messagesopt(optexpand_T *args, صحیح *رقم_Matches, char_u ***matches)
{
    ایستا نویسه *(p_mopt_مقادیر[]) = {"hit-enter", "wait:", "history:", پوچ};
    بازگشت expand_set_opt_string(
	    args,
	    p_mopt_مقادیر,
	    ARRAY_LENGTH(p_mopt_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}

#اگر defined(FEAT_SPELL)
/*
 * The 'mkspellmem' option is changed.
 */
    نویسه *
did_set_mkspellmem(optset_T *args UNUSED)
{
    اگر (spell_check_msm() != OK)
	بازگشت e_invalid_argument;

    بازگشت پوچ;
}
#endif

/*
 * The 'mouse' option is changed.
 */
    نویسه *
did_set_mouse(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    نویسه	*retval;

    retval = did_set_option_listflag(*varp, (char_u *)MOUSE_ALL, args->os_errbuf,
		    args->os_errbuflen);
    اگر (retval == پوچ)
    {
	redraw_tabline = TRUE;
	اگر (tabline_height() > 0)
	    update_screen(UPD_VALID);
#اگر defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
	popup_close_info(); // Close info popup to apply new properties
#endif
    }
    بازگشت retval;
}

    صحیح
expand_set_mouse(optexpand_T *args, صحیح *رقم_Matches, char_u ***matches)
{
    بازگشت expand_set_opt_listflag(args, (char_u*)MOUSE_ALL, رقم_Matches, matches);
}

/*
 * The 'mousemodel' option is changed.
 */
    نویسه *
did_set_mousemodel(optset_T *args UNUSED)
{
    اگر (check_opt_strings(p_mousem, p_mousem_مقادیر, FALSE) != OK)
	بازگشت e_invalid_argument;
#اگر defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU) && (XmVersion <= 1002)
    وگرنه اگر (*p_mousem != *args->os_oldval.string)
	// Changed from "extend" to "popup" or "popup_setpos" or vv: need
	// to create or delete the popup menus.
	gui_motif_update_mousemodel(ریشه_menu);
#endif

    بازگشت پوچ;
}

    صحیح
expand_set_mousemodel(optexpand_T *args, صحیح *رقم_Matches, char_u ***matches)
{
    بازگشت expand_set_opt_string(
	    args,
	    p_mousem_مقادیر,
	    ARRAY_LENGTH(p_mousem_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}

#اگر defined(FEAT_MOUSESHAPE)
    نویسه *
did_set_mouseshape(optset_T *args UNUSED)
{
    نویسه *errmsg = پوچ;

    errmsg = parse_shape_opt(SHAPE_MOUSE);
    update_mouseshape(-1);

    بازگشت errmsg;
}
#endif

/*
 * The 'nrformats' option is changed.
 */
    نویسه *
did_set_nrformats(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    بازگشت did_set_opt_strings(*varp, p_nf_مقادیر, TRUE);
}

    صحیح
expand_set_nrformats(optexpand_T *args, صحیح *رقم_Matches, char_u ***matches)
{
    بازگشت expand_set_opt_string(
	    args,
	    p_nf_مقادیر,
	    ARRAY_LENGTH(p_nf_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}

#اگر defined(FEAT_EVAL)
/*
 * One of the '*expr' options is changed: 'balloonexpr', 'diffexpr',
 * 'foldexpr', 'foldtext', 'formatexpr', 'includeexpr', 'indentexpr',
 * 'patchexpr', 'printexpr' and 'charconvert'.
 */
    نویسه *
did_set_optexpr(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    // If the option value starts with <SID> or s:, then replace that with
    // the script identifier.
    char_u *name = get_scriptlocal_funcname(*varp);
    اگر (name != پوچ)
    {
	free_string_option(*varp);
	*varp = name;
    }

    بازگشت پوچ;
}
#endif

/*
 * The 'pastetoggle' option is changed.
 */
    نویسه *
did_set_pastetoggle(optset_T *args UNUSED)
{
    char_u	*p;

    // translate key codes like in a mapping
    اگر (*p_pt)
    {
	(تهی)replace_termcodes(p_pt, &p, 0,
		REPTERM_FROM_PART | REPTERM_DO_LT, پوچ);
	اگر (p != پوچ)
	{
	    free_string_option(p_pt);
	    p_pt = p;
	}
    }

    بازگشت پوچ;
}

#اگر defined(FEAT_PROP_POPUP)
/*
 * The 'previewpopup' option is changed.
 */
    نویسه *
did_set_previewpopup(optset_T *args UNUSED)
{
    اگر (parse_previewpopup(پوچ) == FAIL)
	بازگشت e_invalid_argument;

# اگر defined(FEAT_QUICKFIX)
    popup_close_info();
# endif
    بازگشت پوچ;
}

    ایستا صحیح
expand_set_popupoption(optexpand_T *args, صحیح *رقم_Matches, char_u ***matches,
	صحیح previewpopup)
{
    expand_T *xp = args->oe_xp;

    اگر (xp->xp_pattern > args->oe_set_arg && *(xp->xp_pattern-1) == ':')
    {
	اگر (completing_مقدار_for_subopt(args, "close")
		|| completing_مقدار_for_subopt(args, "resize")
		|| completing_مقدار_for_subopt(args, "shadow"))
	{
	    بازگشت expand_set_opt_string(
		    args,
		    p_popup_option_on_off_مقادیر,
		    ARRAY_LENGTH(p_popup_option_on_off_مقادیر) - 1,
		    رقم_Matches,
		    matches);
	}
	اگر (completing_مقدار_for_subopt(args, "border"))
	{
	    بازگشت expand_set_opt_string(
		    args,
		    previewpopup ? p_popup_pvp_border_مقادیر
			: p_popup_cpp_border_مقادیر,
		    (previewpopup ? ARRAY_LENGTH(p_popup_pvp_border_مقادیر) - 1
			: ARRAY_LENGTH(p_popup_cpp_border_مقادیر)) - 1,
		    رقم_Matches,
		    matches);
	}
	اگر (completing_مقدار_for_subopt(args, "align"))
	{
	    بازگشت expand_set_opt_string(
		    args,
		    p_popup_option_align_مقادیر,
		    ARRAY_LENGTH(p_popup_option_align_مقادیر) - 1,
		    رقم_Matches,
		    matches);
	}
	اگر (completing_مقدار_for_subopt(args, "highlight")
		|| completing_مقدار_for_subopt(args, "borderhighlight"))
	{
	    // Return the list of all highlight names
	    بازگشت expand_set_opt_generic(
		    args,
		    get_highlight_name,
		    رقم_Matches,
		    matches);
	}
	بازگشت FAIL;
    }

    بازگشت expand_set_opt_string(
	    args,
	    previewpopup ? p_popup_pvp_option_مقادیر
		: p_popup_cpp_option_مقادیر,
	    previewpopup ? ARRAY_LENGTH(p_popup_pvp_option_مقادیر) - 1
		: ARRAY_LENGTH(p_popup_cpp_option_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}

    صحیح
expand_set_previewpopup(optexpand_T *args, صحیح *رقم_Matches, char_u ***matches)
{
    بازگشت expand_set_popupoption(args, رقم_Matches, matches, TRUE);
}

    صحیح
expand_set_completepopup(optexpand_T *args, صحیح *رقم_Matches, char_u ***matches)
{
    بازگشت expand_set_popupoption(args, رقم_Matches, matches, FALSE);
}
#endif

#اگر defined(FEAT_POSTSCRIPT)
/*
 * The 'printencoding' option is changed.
 */
    نویسه *
did_set_printencoding(optset_T *args UNUSED)
{
    char_u	*s, *p;

    // Canonize 'printencoding' if VIM standard one
    p = enc_canonize(p_penc);
    اگر (p != پوچ)
    {
	vim_free(p_penc);
	p_penc = p;
    }
    وگرنه
    {
	// Ensure lower case and '-' for '_'
	برای (s = p_penc; *s != NUL; s++)
	{
	    اگر (*s == '_')
		*s = '-';
	    وگرنه
		*s = TOLOWER_ASC(*s);
	}
    }

    بازگشت پوچ;
}
#endif

#اگر defined(FEAT_PRINTER)

    ایستا char_u *
get_printoptions_names(expand_T *xp UNUSED, صحیح idx)
{
    اگر (idx >= (صحیح)ARRAY_LENGTH(printer_opts))
	بازگشت پوچ;
    بازگشت (char_u*)printer_opts[idx].name;
}

/*
 * Expand 'printoptions' option
 */
    صحیح
expand_set_printoptions(optexpand_T *args, صحیح *رقم_Matches, char_u ***matches)
{
    بازگشت expand_set_opt_generic(
	    args,
	    get_printoptions_names,
	    رقم_Matches,
	    matches);
}
#endif

#اگر defined(FEAT_STL_OPT)
/*
 * The 'statusline' or the 'tabline' or the 'rulerformat' option is changed.
 * "rulerformat" is TRUE if the 'rulerformat' option is changed.
 */
    ایستا نویسه *
parse_statustabline_rulerformat(optset_T *args, صحیح rulerformat)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u	*s;
    نویسه	*errmsg = پوچ;
    صحیح		wid;

    اگر (rulerformat)	// reset ru_wid first
	ru_wid = 0;
    s = *varp;
    اگر (rulerformat && *s == '%')
    {
	// set ru_wid if 'ruf' starts with "%99("
	اگر (*++s == '-')	// ignore a '-'
	    s++;
	wid = getdigits(&s);
	اگر (wid && *s == '(' && (errmsg = check_stl_option(p_ruf)) == پوچ)
	    ru_wid = wid;
	وگرنه
	{
	    // Validate the flags in 'rulerformat' only if it doesn't point to
	    // a custom function ("%!" flag).
	    اگر ((*varp)[1] != '!')
		errmsg = check_stl_option(p_ruf);
	}
    }
    // check 'statusline' or 'tabline' only if it doesn't start with "%!"
    وگرنه اگر (rulerformat || s[0] != '%' || s[1] != '!')
	errmsg = check_stl_option(s);
    اگر (rulerformat && errmsg == پوچ)
	comp_col();

    بازگشت errmsg;
}
#endif

#اگر defined(FEAT_RENDER_OPTIONS)
/*
 * The 'renderoptions' option is changed.
 */
    نویسه *
did_set_renderoptions(optset_T *args UNUSED)
{
    اگر (!gui_mch_set_rendering_options(p_rop))
	بازگشت e_invalid_argument;

    بازگشت پوچ;
}
#endif

#اگر defined(FEAT_RIGHTLEFT)
/*
 * The 'rightleftcmd' option is changed.
 */
    نویسه *
did_set_rightleftcmd(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    // Currently only "search" is a supported value.
    اگر (**varp != NUL && STRCMP(*varp, "search") != 0)
	بازگشت e_invalid_argument;

    بازگشت پوچ;
}

    صحیح
expand_set_rightleftcmd(optexpand_T *args, صحیح *رقم_Matches, char_u ***matches)
{
    ایستا نویسه *(p_rlc_مقادیر[]) = {"search", پوچ};
    بازگشت expand_set_opt_string(
	    args,
	    p_rlc_مقادیر,
	    ARRAY_LENGTH(p_rlc_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}
#endif

#define PUM_BORDER_CLEAR()   \
    انجامبده {                     \
	pum_set_border(FALSE); \
	pum_set_shadow(FALSE); \
	pum_set_margin(FALSE); \
    } وقتی (0)

/*
 * Parse a border value from a pumopt "border:" token.
 * Returns OK on success, FAIL on error.
 */
    ایستا صحیح
parse_pumopt_border(char_u *val, صحیح len)
{
    // Use box-drawing characters only when 'encoding' is "utf-8" and
    // 'ambiwidth' is "single".
    صحیح	    can_use_box_chars = (enc_utf8 && *p_ambw == 's');
    char_u  *token;

    token = vim_strnsave(val, len);
    اگر (token == پوچ)
	بازگشت FAIL;

    اگر (can_use_box_chars && STRCMP(token, "single") == 0)
	pum_set_border_chars(0x2500, 0x2502, 0x2500, 0x2502, // ─ │ ─ │
		0x250c, 0x2510, 0x2518, 0x2514); // ┌ ┐ ┘ └
    وگرنه اگر (can_use_box_chars && STRCMP(token, "double") == 0)
	pum_set_border_chars(0x2550, 0x2551, 0x2550, 0x2551, // ═ ║ ═ ║
		0x2554, 0x2557, 0x255D, 0x255A); // ╔ ╗ ╝  ╚
    وگرنه اگر (can_use_box_chars && STRCMP(token, "round") == 0)
	pum_set_border_chars(0x2500, 0x2502, 0x2500, 0x2502, // ─ │ ─ │
		0x256d, 0x256e, 0x256f, 0x2570); // ╭ ╮ ╯ ╰
    وگرنه اگر (STRCMP(token, "ascii") == 0)
	pum_set_border_chars('-', '|', '-', '|', '+', '+', '+', '+');
    وگرنه اگر (STRNCMP(token, "custom:", 7) == 0)
    {
	char_u	*q = token + 7;
	صحیح	out[8];

	برای (صحیح i = 0; i < 8; i++)
	{
	    اگر (*q == NUL || *q == ',')
	    {
		vim_free(token);
		بازگشت FAIL;
	    }
	    out[i] = mb_ptr2char(q);
	    mb_ptr2char_adv(&q);
	    اگر (i < 7)
	    {
		اگر (*q != ';')
		{
		    vim_free(token);
		    بازگشت FAIL;
		}
		q++;
	    }
	}
	اگر (*q != NUL && *q != ',')
	{
	    vim_free(token);
	    بازگشت FAIL;
	}
	pum_set_border_chars(out[0], out[1], out[2], out[3], out[4], out[5],
		out[6], out[7]);
    }
    وگرنه
    {
	vim_free(token);
	بازگشت FAIL;
    }

    vim_free(token);
    pum_set_border(TRUE);
    بازگشت OK;
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
    نویسه *
did_set_pumopt(optset_T *args)
{
    char_u  **varp = (char_u **)args->os_varp;
    char_u  *p;
    صحیح	    have_border = FALSE;
    صحیح	    have_margin = FALSE;

    // Reset to defaults.
    PUM_BORDER_CLEAR();
    p_ph = 0;
    p_pw = 15;
    p_pmw = 0;
    p_po = 100;

    اگر (*varp == پوچ || **varp == NUL)
	بازگشت پوچ;

    برای (p = *varp; p != پوچ && *p != NUL; )
    {
	char_u *comma = vim_strchr(p, ',');
	صحیح len;

	اگر (comma != پوچ)
	    len = (صحیح)(comma - p);
	وگرنه
	    len = (صحیح)STRLEN(p);

	اگر (STRNCMP(p, "border:", 7) == 0)
	{
	    اگر (have_border)
		برو error;
	    have_border = TRUE;
	    اگر (parse_pumopt_border(p + 7, len - 7) == FAIL)
		برو error;
	}
	وگرنه اگر (STRNCMP(p, "height:", 7) == 0)
	{
	    long n = atol((نویسه *)p + 7);
	    اگر (n < 0)
		برو error;
	    p_ph = n;
	}
	وگرنه اگر (STRNCMP(p, "width:", 6) == 0)
	{
	    long n = atol((نویسه *)p + 6);
	    اگر (n < 0)
		برو error;
	    p_pw = n;
	}
	وگرنه اگر (STRNCMP(p, "maxwidth:", 9) == 0)
	{
	    long n = atol((نویسه *)p + 9);
	    اگر (n < 0)
		برو error;
	    p_pmw = n;
	}
	وگرنه اگر (STRNCMP(p, "opacity:", 8) == 0)
	{
	    long n = atol((نویسه *)p + 8);
	    اگر (n < 0 || n > 100)
		برو error;
	    p_po = n;
	}
	وگرنه اگر (len == 6 && STRNCMP(p, "shadow", 6) == 0)
	    pum_set_shadow(TRUE);
	وگرنه اگر (len == 6 && STRNCMP(p, "margin", 6) == 0)
	{
	    have_margin = TRUE;
	    pum_set_margin(TRUE);
	}
	وگرنه
	    برو error;

	اگر (comma != پوچ)
	    p = comma + 1;
	وگرنه
	    بشکن;
    }

    اگر (have_margin && !have_border)
	برو error;

    // Invalidate cached background for opacity changes.
    pum_opacity_changed();

    بازگشت پوچ;

error:
    PUM_BORDER_CLEAR();
    p_ph = 0;
    p_pw = 15;
    p_pmw = 0;
    p_po = 100;
    بازگشت e_invalid_argument;
}

    ایستا char_u *
get_pum_border_style(expand_T *xp UNUSED, صحیح idx)
{
    ایستا نویسه *styles[] = {"ascii", "custom:", "single", "double", "round"};
    بازگشت idx < ((enc_utf8 && *p_ambw == 's') ? (صحیح)ARRAY_LENGTH(styles) : 2)
	    ? (char_u *)styles[idx] : پوچ;
}

    صحیح
expand_set_pumopt(optexpand_T *args, صحیح *رقم_Matches, char_u ***matches)
{
    expand_T *xp = args->oe_xp;

    اگر (xp->xp_pattern > args->oe_set_arg && *(xp->xp_pattern-1) == ':')
    {
	اگر (completing_مقدار_for_subopt(args, "border"))
	{
	    بازگشت expand_set_opt_generic(
		    args, get_pum_border_style, رقم_Matches, matches);
	}
	بازگشت FAIL;
    }

    ایستا نویسه *(p_pumopt_مقادیر[]) = {"border:", "height:", "width:",
	"maxwidth:", "opacity:", "shadow", "margin", پوچ};
    بازگشت expand_set_opt_string(
	    args,
	    p_pumopt_مقادیر,
	    ARRAY_LENGTH(p_pumopt_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}

/*
 * The 'pumborder' option is changed.
 * Rules:
 *   - One of { single, double, round, ascii, custom:XXXXXXXX } may appear.
 *   - "margin" may appear, but only together with exactly one border style.
 *   - "shadow" is independent and can be combined freely.
 */
    نویسه *
did_set_pumborder(optset_T *args)
{
    char_u  **varp = (char_u **)args->os_varp;
    char_u  *p;
    صحیح	    len;
    صحیح	    have_border = FALSE;
    صحیح	    have_margin = FALSE;

    PUM_BORDER_CLEAR();

    اگر (*varp == پوچ || **varp == NUL)
	بازگشت پوچ;

    برای (p = *varp; p != پوچ && *p != NUL; )
    {
	char_u *comma = vim_strchr(p, ',');
	اگر (comma != پوچ)
	    len = (صحیح)(comma - p);
	وگرنه
	    len = (صحیح)STRLEN(p);

	اگر (STRNCMP(p, "shadow", len) == 0 && len == 6)
	    pum_set_shadow(TRUE);
	وگرنه اگر (STRNCMP(p, "margin", len) == 0 && len == 6)
	{
	    have_margin = TRUE;
	    pum_set_margin(TRUE);
	}
	وگرنه
	{
	    اگر (have_border)
		برو error;
	    have_border = TRUE;
	    اگر (parse_pumopt_border(p, len) == FAIL)
		برو error;
	}

	اگر (comma != پوچ)
	    p = comma + 1;
	وگرنه
	    بشکن;
    }

    اگر (have_margin && !have_border)
	برو error;

    بازگشت پوچ;

error:
    PUM_BORDER_CLEAR();
    pum_set_border_chars(0, 0, 0, 0, 0, 0, 0, 0);
    بازگشت e_invalid_argument;
}

    ایستا char_u *
get_pumborder_token(expand_T *xp, صحیح idx)
{
    بازگشت idx == 0 ? (char_u *)"margin"
	 : idx == 1 ? (char_u *)"shadow"
	 : get_pum_border_style(xp, idx - 2);
}

    صحیح
expand_set_pumborder(optexpand_T *args, صحیح *رقم_Matches, char_u ***matches)
{
    بازگشت expand_set_opt_generic(
	    args, get_pumborder_token, رقم_Matches, matches);
}

#اگر defined(FEAT_STL_OPT)

/*
 * The 'rulerformat' option is changed.
 */
    نویسه *
did_set_rulerformat(optset_T *args)
{
    بازگشت parse_statustabline_rulerformat(args, TRUE);
}
#endif

#اگر defined(FEAT_TABPANEL)
/*
 * Process the new 'tabpanelopt' option value.
 */
    نویسه *
did_set_tabpanelopt(optset_T *args UNUSED)
{
    اگر (tabpanelopt_changed() == FAIL)
	بازگشت e_invalid_argument;

    بازگشت پوچ;
}

    صحیح
expand_set_tabpanelopt(optexpand_T *args, صحیح *رقم_Matches, char_u ***matches)
{
    expand_T *xp = args->oe_xp;

    اگر (xp->xp_pattern > args->oe_set_arg && *(xp->xp_pattern-1) == ':')
    {
	اگر (completing_مقدار_for_subopt(args, "align"))
	{
	    بازگشت expand_set_opt_string(
		    args,
		    p_tplo_align_مقادیر,
		    ARRAY_LENGTH(p_tplo_align_مقادیر) - 1,
		    رقم_Matches,
		    matches);
	}
	بازگشت FAIL;
    }

    بازگشت expand_set_opt_string(
	    args,
	    p_tplo_مقادیر,
	    ARRAY_LENGTH(p_tplo_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}
#endif

/*
 * The 'scrollopt' option is changed.
 */
    نویسه *
did_set_scrollopt(optset_T *args UNUSED)
{
    بازگشت did_set_opt_strings(p_sbo, p_scbopt_مقادیر, TRUE);
}

    صحیح
expand_set_scrollopt(optexpand_T *args, صحیح *رقم_Matches, char_u ***matches)
{
    بازگشت expand_set_opt_string(
	    args,
	    p_scbopt_مقادیر,
	    ARRAY_LENGTH(p_scbopt_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}

/*
 * The 'wlseat' option is changed
 */
    نویسه *
did_set_wlseat(optset_T *args UNUSED)
{
#ifdef FEAT_WAYLAND_CLIPBOARD
    // If there isn't any seat named 'wlseat', then let the Wayland clipboard be
    // unavailable. Ignore errors returned.
    clip_reset_wayland();
#endif

    بازگشت پوچ;
}

/*
 * The 'selection' option is changed.
 */
    نویسه *
did_set_selection(optset_T *args UNUSED)
{
    اگر (*p_sel == NUL || check_opt_strings(p_sel, p_sel_مقادیر, FALSE) != OK)
	بازگشت e_invalid_argument;

    بازگشت پوچ;
}

    صحیح
expand_set_selection(optexpand_T *args, صحیح *رقم_Matches, char_u ***matches)
{
    بازگشت expand_set_opt_string(
	    args,
	    p_sel_مقادیر,
	    ARRAY_LENGTH(p_sel_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}

/*
 * The 'selectmode' option is changed.
 */
    نویسه *
did_set_selectmode(optset_T *args UNUSED)
{
    بازگشت did_set_opt_strings(p_slm, p_slm_مقادیر, TRUE);
}

    صحیح
expand_set_selectmode(optexpand_T *args, صحیح *رقم_Matches, char_u ***matches)
{
    بازگشت expand_set_opt_string(
	    args,
	    p_slm_مقادیر,
	    ARRAY_LENGTH(p_slm_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}

#اگر defined(FEAT_SESSION)
/*
 * The 'sessionoptions' option is changed.
 */
    نویسه *
did_set_sessionoptions(optset_T *args)
{
    اگر (opt_strings_flags(p_ssop, p_ssop_مقادیر, &ssop_flags, TRUE) != OK)
	بازگشت e_invalid_argument;
    اگر ((ssop_flags & SSOP_CURDIR) && (ssop_flags & SSOP_SESDIR))
    {
	// Don't allow both "sesdir" and "curdir".
	(تهی)opt_strings_flags(args->os_oldval.string, p_ssop_مقادیر,
							&ssop_flags, TRUE);
	بازگشت e_invalid_argument;
    }

    بازگشت پوچ;
}

    صحیح
expand_set_sessionoptions(
    optexpand_T		*args,
    صحیح			*رقم_Matches,
    char_u		***matches)
{
    بازگشت expand_set_opt_string(
	    args,
	    p_ssop_مقادیر,
	    ARRAY_LENGTH(p_ssop_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}
#endif

/*
 * Validate 'shellpipe'/'shellredir' option.
 */
    نویسه *
did_set_shellpipe_redir(optset_T *args)
{
    char_u	*p;
    bool	seen = false;

    برای (p = args->os_newval.string; *p != NUL; ++p)
    {
	اگر (*p != '%')
	    ادامه;

	اگر (p[1] == NUL)
	    بازگشت e_invalid_format_string_single_percent_s;

	اگر (p[1] == '%')
	{
	    ++p;    // skip second %
	    ادامه;
	}

	اگر (p[1] == 's')
	{
	    اگر (seen)
		بازگشت e_invalid_format_string_single_percent_s;

	    seen = true;
	    ++p;    // consume 's'
	    ادامه;
	}
	بازگشت e_invalid_format_string_single_percent_s;
    }
    بازگشت پوچ;
}

/*
 * The 'shortmess' option is changed.
 */
    نویسه *
did_set_shortmess(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    بازگشت did_set_option_listflag(*varp, (char_u *)SHM_ALL, args->os_errbuf,
		    args->os_errbuflen);
}

    صحیح
expand_set_shortmess(optexpand_T *args, صحیح *رقم_Matches, char_u ***matches)
{
    بازگشت expand_set_opt_listflag(args, (char_u*)SHM_ALL, رقم_Matches, matches);
}

#اگر defined(FEAT_LINEBREAK)
/*
 * The 'showbreak' option is changed.
 */
    نویسه *
did_set_showbreak(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u	*s;

    برای (s = *varp; *s; )
    {
	اگر (ptr2cells(s) != 1)
	    بازگشت e_showbreak_contains_unprintable_or_wide_character;
	MB_PTR_ADV(s);
    }

    بازگشت پوچ;
}
#endif

/*
 * The 'showcmdloc' option is changed.
 */
    نویسه *
did_set_showcmdloc(optset_T *args UNUSED)
{
    نویسه	*errmsg = did_set_opt_strings(p_sloc, p_sloc_مقادیر, FALSE);

    اگر (errmsg == پوچ)
	comp_col();

    بازگشت errmsg;
}

    صحیح
expand_set_showcmdloc(optexpand_T *args, صحیح *رقم_Matches, char_u ***matches)
{
    بازگشت expand_set_opt_string(
	    args,
	    p_sloc_مقادیر,
	    ARRAY_LENGTH(p_sloc_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}

#اگر defined(FEAT_SIGNS)
/*
 * The 'signcolumn' option is changed.
 */
    نویسه *
did_set_signcolumn(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

# اگر defined(FEAT_LINEBREAK)
    اگر (check_opt_strings(*varp, p_scl_مقادیر, FALSE) != OK)
	بازگشت e_invalid_argument;
    // When changing the 'signcolumn' to or from 'number', recompute the
    // width of the number column if 'number' or 'relativenumber' is set.
    اگر (((*args->os_oldval.string == 'n' && args->os_oldval.string[1] == 'u')
		|| (*curwin->w_p_scl == 'n' && *(curwin->w_p_scl + 1) =='u'))
	    && (curwin->w_p_nu || curwin->w_p_rnu))
	curwin->w_nrwidth_سطر_شمار = 0;
# endif

# اگر defined(FEAT_GUI)
    // In the GUI, when sign icons are used, a full screen clear is needed
    // to properly redraw the sign icons.
    اگر (gui.in_use && curbuf->b_signlist != پوچ)
	redraw_all_later(UPD_CLEAR);
# endif

    بازگشت پوچ;
}

    صحیح
expand_set_signcolumn(optexpand_T *args, صحیح *رقم_Matches, char_u ***matches)
{
    بازگشت expand_set_opt_string(
	    args,
	    p_scl_مقادیر,
	    ARRAY_LENGTH(p_scl_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}
#endif

#اگر defined(FEAT_SPELL)
/*
 * The 'spellcapcheck' option is changed.
 */
    نویسه *
did_set_spellcapcheck(optset_T *args UNUSED)
{
    // compile the regexp program.
    بازگشت compile_cap_prog(curwin->w_s);
}

/*
 * The 'spellfile' option is changed.
 */
    نویسه *
did_set_spellfile(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    اگر (!valid_spellfile(*varp))
	بازگشت e_invalid_argument;

    // If there is a window for this buffer in which 'spell' is set load the
    // wordlists.
    بازگشت did_set_spell_option();
}

/*
 * The 'spell' option is changed.
 */
    نویسه *
did_set_spelllang(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    اگر (!valid_spelllang(*varp))
	بازگشت e_invalid_argument;

    // If there is a window for this buffer in which 'spell' is set load the
    // wordlists.
    بازگشت did_set_spell_option();
}

/*
 * The 'spelloptions' option is changed.
 */
    نویسه *
did_set_spelloptions(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    اگر (**varp != NUL && STRCMP(*varp, "camel") != 0)
	بازگشت e_invalid_argument;

    بازگشت پوچ;
}

    صحیح
expand_set_spelloptions(optexpand_T *args, صحیح *رقم_Matches, char_u ***matches)
{
    ایستا نویسه *(p_spo_مقادیر[]) = {"camel", پوچ};
    بازگشت expand_set_opt_string(
	    args,
	    p_spo_مقادیر,
	    ARRAY_LENGTH(p_spo_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}

/*
 * The 'spellsuggest' option is changed.
 */
    نویسه *
did_set_spellsuggest(optset_T *args UNUSED)
{
    اگر (spell_check_sps() != OK)
	بازگشت e_invalid_argument;

    بازگشت پوچ;
}

    صحیح
expand_set_spellsuggest(optexpand_T *args, صحیح *رقم_Matches, char_u ***matches)
{
    بازگشت expand_set_opt_string(
	    args,
	    p_sps_مقادیر,
	    ARRAY_LENGTH(p_sps_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}
#endif

/*
 * The 'splitkeep' option is changed.
 */
    نویسه *
did_set_splitkeep(optset_T *args UNUSED)
{
    win_T	*wp;
    tabpage_T	*tp;
    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	wp->w_prev_height = wp->w_height;
	wp->w_prev_winrow = wp->w_winrow;
    }
    بازگشت did_set_opt_strings(p_spk, p_spk_مقادیر, FALSE);
}

    صحیح
expand_set_splitkeep(optexpand_T *args, صحیح *رقم_Matches, char_u ***matches)
{
    بازگشت expand_set_opt_string(
	    args,
	    p_spk_مقادیر,
	    ARRAY_LENGTH(p_spk_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}

#اگر defined(FEAT_STL_OPT)
/*
 * The 'statusline' option is changed.
 */
    نویسه *
did_set_statusline(optset_T *args)
{
    char_u  **varp = (char_u **)args->os_varp;
    نویسه    *ret = parse_statustabline_rulerformat(args, FALSE);

    اگر (ret != پوچ)
	بازگشت ret;
    update_stl_rendered_height(varp == &curwin->w_p_stl ? curwin : پوچ);
    frame_change_statusline_height();

    بازگشت پوچ;
}

/*
 * Rewrite the 'statuslineopt' string: replace the last "maxheight:N" with
 * actual_stlh and remove earlier duplicates.  Keyword order is preserved.
 */
    ایستا تهی
update_stlo_maxheight(char_u **varp, صحیح actual_stlh)
{
    ثابت char_u    *src = *varp;
    ثابت char_u    *last_mh = پوچ;
    ثابت char_u    *p;

    // Find the last "maxheight:" token.
    p = src;
    وقتی (*p != NUL)
    {
	اگر (STRNCMP(p, "maxheight:", 10) == 0)
	    last_mh = p;
	وقتی (*p != ',' && *p != NUL)
	    ++p;
	اگر (*p == ',')
	    ++p;
    }

    اگر (last_mh == پوچ)
	بازگشت;

    size_t	bufsize = STRLEN(src) + NUMBUFLEN + 1;
    char_u	*buf = alloc(bufsize);
    اگر (buf == پوچ)
	بازگشت;

    صحیح		len = 0;
    bool	need_comma = false;

    p = src;
    وقتی (*p != NUL)
    {
	ثابت	char_u	*tok = p;
	صحیح	tok_len;

	وقتی (*p != ',' && *p != NUL)
	    ++p;
	tok_len = (صحیح)(p - tok);
	اگر (*p == ',')
	    ++p;

	اگر (STRNCMP(tok, "maxheight:", 10) == 0)
	{
	    اگر (tok == last_mh)
	    {
		// Replace the last occurrence with the actual value.
		اگر (need_comma)
		    buf[len++] = ',';
		len += vim_snprintf((نویسه *)buf + len,
			bufsize - len, "maxheight:%d", actual_stlh);
		need_comma = true;
	    }
	}
	وگرنه
	{
	    اگر (need_comma)
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
    نویسه *
did_set_statuslineopt(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    win_T       *wp = varp == &curwin->w_p_stlo ? curwin : پوچ;

    اگر (statuslineopt_changed(*varp, wp) == FAIL)
	بازگشت e_invalid_argument;

    // Sync stl_rendered_height with the current statusline.
    update_stl_rendered_height(wp);

    // Update the maxheight value to the actual value set.
    // Note: Must be changed if p_stlo_values are changed.
    اگر (*varp != empty_option)
    {
	صحیح actual_stlh;

	اگر (wp != پوچ)
	{
	    frame_change_statusline_height();
	    actual_stlh = MIN(wp->w_p_stlo_mh,
		    wp->w_height + wp->w_status_height - p_wmh);
	}
	وگرنه
	    actual_stlh = frame_change_statusline_height();

	// Only re-serialize when the window actually has a status line shown.
	اگر (actual_stlh > 0)
	{
	    update_stlo_maxheight(varp, actual_stlh);
	    // Update the parsed maxheight member directly.
	    اگر (wp != پوچ)
		wp->w_p_stlo_mh = actual_stlh;
	    وگرنه
		set_stlo_mh(actual_stlh);
	}
    }
    وگرنه
	frame_change_statusline_height();

    اگر ((args->os_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	// :set clears the local values.
	clear_string_option(&curwin->w_p_stlo);
	curwin->w_p_stlo_fh = -1;
	curwin->w_p_stlo_mh = -1;
    }

    بازگشت پوچ;
}

    صحیح
expand_set_statuslineopt(optexpand_T *args, صحیح *رقم_Matches, char_u ***matches)
{
    بازگشت expand_set_opt_string(
	    args,
	    p_stlo_مقادیر,
	    ARRAY_LENGTH(p_stlo_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}
#endif

/*
 * The 'swapsync' option is changed.
 */
    نویسه *
did_set_swapsync(optset_T *args UNUSED)
{
    بازگشت did_set_opt_strings(p_sws, p_sws_مقادیر, FALSE);
}

    صحیح
expand_set_swapsync(optexpand_T *args, صحیح *رقم_Matches, char_u ***matches)
{
    بازگشت expand_set_opt_string(
	    args,
	    p_sws_مقادیر,
	    ARRAY_LENGTH(p_sws_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}

/*
 * The 'switchbuf' option is changed.
 */
    نویسه *
did_set_switchbuf(optset_T *args UNUSED)
{
    بازگشت did_set_opt_flags(p_swb, p_swb_مقادیر, &swb_flags, TRUE);
}

    صحیح
expand_set_switchbuf(optexpand_T *args, صحیح *رقم_Matches, char_u ***matches)
{
    بازگشت expand_set_opt_string(
	    args,
	    p_swb_مقادیر,
	    ARRAY_LENGTH(p_swb_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}

/*
 * The 'tabclose' option is changed.
 */
    نویسه *
did_set_tabclose(optset_T *args UNUSED)
{
    بازگشت did_set_opt_flags(p_tcl, p_tcl_مقادیر, &tcl_flags, TRUE);
}

    صحیح
expand_set_tabclose(optexpand_T *args, صحیح *رقم_Matches, char_u ***matches)
{
    بازگشت expand_set_opt_string(
	    args,
	    p_tcl_مقادیر,
	    ARRAY_LENGTH(p_tcl_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}

#اگر defined(FEAT_STL_OPT)
/*
 * The 'tabline' option is changed.
 */
    نویسه *
did_set_tabline(optset_T *args)
{
    بازگشت parse_statustabline_rulerformat(args, FALSE);
}
#endif

/*
 * The 'tagcase' option is changed.
 */
    نویسه *
did_set_tagcase(optset_T *args)
{
    unsigned صحیح	*flags;
    char_u		*p;

    اگر (args->os_flags & OPT_LOCAL)
    {
	p = curbuf->b_p_tc;
	flags = &curbuf->b_tc_flags;
    }
    وگرنه
    {
	p = p_tc;
	flags = &tc_flags;
    }

    اگر ((args->os_flags & OPT_LOCAL) && *p == NUL)
	// make the local value empty: use the global value
	*flags = 0;
    وگرنه اگر (*p == NUL
	    || opt_strings_flags(p, p_tc_مقادیر, flags, FALSE) != OK)
	بازگشت e_invalid_argument;

    بازگشت پوچ;
}

    صحیح
expand_set_tagcase(optexpand_T *args, صحیح *رقم_Matches, char_u ***matches)
{
    بازگشت expand_set_opt_string(
	    args,
	    p_tc_مقادیر,
	    ARRAY_LENGTH(p_tc_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}

/*
 * The 'term' option is changed.
 */
    نویسه *
did_set_term(optset_T *args UNUSED)
{
    اگر (T_NAME[0] == NUL)
	بازگشت e_cannot_set_term_to_empty_string;
#ifdef FEAT_GUI
    اگر (gui.in_use)
	بازگشت e_cannot_change_term_in_GUI;
    اگر (term_is_gui(T_NAME))
	بازگشت e_use_gui_to_start_GUI;
#endif
    اگر (set_termname(T_NAME) == FAIL)
	بازگشت e_not_found_in_termcap;

    // Screen colors may have changed.
    redraw_later_clear();

    بازگشت پوچ;
}

/*
 * Some terminal option (t_xxx) is changed
 */
    نویسه *
did_set_term_option(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    اگر (!full_screen)
	بازگشت پوچ;

    // ":set t_Co=0" and ":set t_Co=1" do ":set t_Co="
    اگر (varp == &T_CCO)
    {
	صحیح colors = atoi((نویسه *)T_CCO);

	// Only reinitialize colors if t_Co value has really changed to
	// avoid expensive reload of colorscheme if t_Co is set to the
	// same value multiple times.
	اگر (colors != t_colors)
	{
	    t_colors = colors;
	    اگر (t_colors <= 1)
	    {
		vim_free(T_CCO);
		T_CCO = empty_option;
	    }
#اگر defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
	    اگر (is_term_win32())
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
    اگر (varp == &T_ME)
    {
	out_str(T_ME);
	redraw_later(UPD_CLEAR);
#اگر defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
	// Since t_me has been set, this probably means that the user
	// wants to use this as default colors.  Need to reset default
	// background/foreground colors.
# ifdef VIMDLL
	اگر (!gui.in_use && !gui.starting)
# endif
	    mch_set_normal_colors();
#endif
    }
    اگر (varp == &T_BE && termcap_active)
    {
	MAY_WANT_TO_LOG_THIS;

	اگر (*T_BE == NUL)
	    // When clearing t_BE we assume the user no longer wants
	    // bracketed paste, thus disable it by writing t_BD.
	    out_str(T_BD);
	وگرنه
	    out_str(T_BE);
    }

    اگر (varp == &T_BSU || varp == &T_ESU)
	term_set_sync_output(TERM_SYNC_OUTPUT_OFF);

    بازگشت پوچ;
}


#ifdef UNIX
/*
 * The 'termresize' option is changed.
 */
    نویسه *
did_set_termresize(optset_T *args UNUSED)
{
    // If empty or "inband", then attempt to enable in-band resize events.
    اگر (*p_trz == NUL || STRCMP(p_trz, "inband") == 0)
	term_set_win_resize(true);
    وگرنه اگر (STRCMP(p_trz, "sigwinch") == 0)
	term_set_win_resize(false);
    وگرنه
	بازگشت e_invalid_argument;

    بازگشت پوچ;
}

    صحیح
expand_set_termresize(optexpand_T *args, صحیح *رقم_Matches, char_u ***matches)
{
    بازگشت expand_set_opt_string(
	    args,
	    p_trz_مقادیر,
	    ARRAY_LENGTH(p_trz_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}
#endif

#اگر defined(FEAT_TERMINAL)
/*
 * The 'termwinkey' option is changed.
 */
    نویسه *
did_set_termwinkey(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    اگر ((*varp)[0] != NUL && string_to_کلید(*varp, TRUE) == 0)
	بازگشت e_invalid_argument;

    بازگشت پوچ;
}

/*
 * The 'termwinsize' option is changed.
 */
    نویسه *
did_set_termwinsize(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u	*p;

    اگر ((*varp)[0] == NUL)
	بازگشت پوچ;

    p = skipdigits(*varp);
    اگر (p == *varp || (*p != 'x' && *p != '*') || *skipdigits(p + 1) != NUL)
	بازگشت e_invalid_argument;

    بازگشت پوچ;
}

# اگر defined(MSWIN)
/*
 * The 'termwintype' option is changed.
 */
    نویسه *
did_set_termwintype(optset_T *args UNUSED)
{
    بازگشت did_set_opt_strings(p_twt, p_twt_مقادیر, FALSE);
}

    صحیح
expand_set_termwintype(optexpand_T *args, صحیح *رقم_Matches, char_u ***matches)
{
    بازگشت expand_set_opt_string(
	    args,
	    p_twt_مقادیر,
	    ARRAY_LENGTH(p_twt_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}
# endif
#endif

/*
 * The 'titlestring' option is changed.
 */
    نویسه *
did_set_titlestring(optset_T *args)
{
    صحیح flagval = 0;

#ifdef FEAT_STL_OPT
    flagval = STL_IN_TITLE;
#endif
    بازگشت parse_titleiconstring(args, flagval);
}

#اگر defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
/*
 * The 'toolbar' option is changed.
 */
    نویسه *
did_set_toolbar(optset_T *args UNUSED)
{
    اگر (opt_strings_flags(p_toolbar, p_toolbar_مقادیر, &toolbar_flags,
								TRUE) != OK)
	بازگشت e_invalid_argument;

    out_flush();
    gui_mch_show_toolbar((toolbar_flags &
		(TOOLBAR_TEXT | TOOLBAR_ICONS)) != 0);
    بازگشت پوچ;
}

    صحیح
expand_set_toolbar(optexpand_T *args, صحیح *رقم_Matches, char_u ***matches)
{
    بازگشت expand_set_opt_string(
	    args,
	    p_toolbar_مقادیر,
	    ARRAY_LENGTH(p_toolbar_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}
#endif

#اگر defined(FEAT_TOOLBAR) && defined(FEAT_GUI_GTK)
/*
 * The 'toolbariconsize' option is changed.  GTK+ 2 only.
 */
    نویسه *
did_set_toolbariconsize(optset_T *args UNUSED)
{
    اگر (opt_strings_flags(p_tbis, p_tbis_مقادیر, &tbis_flags, FALSE) != OK)
	بازگشت e_invalid_argument;

    out_flush();
    gui_mch_show_toolbar((toolbar_flags &
		(TOOLBAR_TEXT | TOOLBAR_ICONS)) != 0);
    بازگشت پوچ;
}

    صحیح
expand_set_toolbariconsize(
    optexpand_T	*args,
    صحیح		*رقم_Matches,
    char_u	***matches)
{
    بازگشت expand_set_opt_string(
	    args,
	    p_tbis_مقادیر,
	    ARRAY_LENGTH(p_tbis_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}
#endif

#اگر defined(UNIX) || defined(VMS)
/*
 * The 'ttymouse' option is changed.
 */
    نویسه *
did_set_ttymouse(optset_T *args UNUSED)
{
    نویسه *errmsg = پوچ;

    // Switch the mouse off before changing the escape sequences used for
    // that.
    mch_setmouse(FALSE);
    اگر (opt_strings_flags(p_ttym, p_ttym_مقادیر, &ttym_flags, FALSE) != OK)
	errmsg = e_invalid_argument;
    وگرنه
	check_mouse_termcode();
    اگر (termcap_active)
	setmouse();		// may switch it on again

    بازگشت errmsg;
}

    صحیح
expand_set_ttymouse(optexpand_T *args, صحیح *رقم_Matches, char_u ***matches)
{
    بازگشت expand_set_opt_string(
	    args,
	    p_ttym_مقادیر,
	    ARRAY_LENGTH(p_ttym_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}
#endif

#اگر defined(FEAT_VARTABS)
/*
 * The 'varsofttabstop' option is changed.
 */
    نویسه *
did_set_varsofttabstop(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u *cp;

    اگر (!((*varp)[0]) || ((*varp)[0] == '0' && !((*varp)[1])))
	VIM_CLEAR(curbuf->b_p_vsts_array);
    وگرنه
    {
	برای (cp = *varp; *cp; ++cp)
	{
	    اگر (vim_isdigit(*cp))
		ادامه;
	    اگر (*cp == ',' && cp > *varp && *(cp-1) != ',')
		ادامه;
	    بازگشت e_invalid_argument;
	}

	صحیح *oldarray = curbuf->b_p_vsts_array;
	اگر (tabstop_set(*varp, &(curbuf->b_p_vsts_array)) == OK)
	{
	    اگر (oldarray)
		vim_free(oldarray);
	}
	وگرنه
	    بازگشت e_invalid_argument;
    }

    بازگشت پوچ;
}

/*
 * The 'vartabstop' option is changed.
 */
    نویسه *
did_set_vartabstop(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u *cp;

    اگر (!((*varp)[0]) || ((*varp)[0] == '0' && !((*varp)[1])))
	VIM_CLEAR(curbuf->b_p_vts_array);
    وگرنه
    {
	برای (cp = *varp; *cp; ++cp)
	{
	    اگر (vim_isdigit(*cp))
		ادامه;
	    اگر (*cp == ',' && cp > *varp && *(cp-1) != ',')
		ادامه;
	    بازگشت e_invalid_argument;
	}

	صحیح *oldarray = curbuf->b_p_vts_array;

	اگر (tabstop_set(*varp, &(curbuf->b_p_vts_array)) == OK)
	{
	    vim_free(oldarray);
# ifdef FEAT_FOLDING
	    اگر (foldmethodIsIndent(curwin))
		foldUpdateAll(curwin);
# endif
	}
	وگرنه
	    بازگشت e_invalid_argument;
    }

    بازگشت پوچ;
}
#endif

/*
 * The 'verbosefile' option is changed.
 */
    نویسه *
did_set_verbosefile(optset_T *args UNUSED)
{
    verbose_stop();
    اگر (*p_vfile != NUL && verbose_open() == FAIL)
	بازگشت e_invalid_argument;

    بازگشت پوچ;
}

#اگر defined(FEAT_SESSION)
/*
 * The 'viewoptions' option is changed.
 */
    نویسه *
did_set_viewoptions(optset_T *args UNUSED)
{
    بازگشت did_set_opt_flags(p_vop, p_ssop_مقادیر, &vop_flags, TRUE);
}
#endif

#اگر defined(FEAT_VIMINFO)
/*
 * The 'viminfo' option is changed.
 */
    نویسه *
did_set_viminfo(optset_T *args)
{
    char_u	*s;
    نویسه	*errmsg = پوچ;

    برای (s = p_viminfo; *s;)
    {
	// Check it's a valid character
	اگر (vim_strchr((char_u *)"!\"%'/:<@cfhnrs", *s) == پوچ)
	{
	    errmsg = illegal_char(args->os_errbuf, args->os_errbuflen, *s);
	    بشکن;
	}
	اگر (*s == 'n')	// name is always last one
	    بشکن;
	وگرنه اگر (*s == 'r') // skip until next ','
	{
	    وقتی (*++s && *s != ',')
		;
	}
	وگرنه اگر (*s == '%')
	{
	    // optional number
	    وقتی (vim_isdigit(*++s))
		;
	}
	وگرنه اگر (*s == '!' || *s == 'h' || *s == 'c')
	    ++s;		// no extra chars
	وگرنه		// must have a number
	{
	    وقتی (vim_isdigit(*++s))
		;

	    اگر (!VIM_ISDIGIT(*(s - 1)))
	    {
		اگر (args->os_errbuf != پوچ)
		{
		    vim_snprintf(args->os_errbuf, args->os_errbuflen,
			    _(e_missing_عدد_after_angle_str_angle),
			    transchar_byte(*(s - 1)));
		    errmsg = args->os_errbuf;
		}
		وگرنه
		    errmsg = "";
		بشکن;
	    }
	}
	اگر (*s == ',')
	    ++s;
	وگرنه اگر (*s)
	{
	    اگر (args->os_errbuf != پوچ)
		errmsg = e_missing_comma;
	    وگرنه
		errmsg = "";
	    بشکن;
	}
    }
    اگر (*p_viminfo && errmsg == پوچ && get_viminfo_parameter('\'') < 0)
	errmsg = e_must_specify_a_مقدار;

    بازگشت errmsg;
}
#endif

/*
 * The 'virtualedit' option is changed.
 */
    نویسه *
did_set_virtualedit(optset_T *args)
{
    char_u		*ve = p_ve;
    unsigned صحیح	*flags = &ve_flags;

    اگر (args->os_flags & OPT_LOCAL)
    {
	ve = curwin->w_p_ve;
	flags = &curwin->w_ve_flags;
    }

    اگر ((args->os_flags & OPT_LOCAL) && *ve == NUL)
	// make the local value empty: use the global value
	*flags = 0;
    وگرنه
    {
	اگر (opt_strings_flags(ve, p_ve_مقادیر, flags, TRUE) != OK)
	    بازگشت e_invalid_argument;
	وگرنه اگر (STRCMP(ve, args->os_oldval.string) != 0)
	{
	    // Recompute cursor position in case the new 've' setting
	    // changes something.
	    validate_virtcol();
	    coladvance(curwin->w_virtcol);
	}
    }

    بازگشت پوچ;
}

    صحیح
expand_set_virtualedit(optexpand_T *args, صحیح *رقم_Matches, char_u ***matches)
{
    بازگشت expand_set_opt_string(
	    args,
	    p_ve_مقادیر,
	    ARRAY_LENGTH(p_ve_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}

/*
 * The 'whichwrap' option is changed.
 */
    نویسه *
did_set_whichwrap(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    // Add ',' to the list flags because 'whichwrap' is a flag
    // list that is comma-separated.
    بازگشت did_set_option_listflag(*varp, (char_u *)(WW_ALL ","),
		    args->os_errbuf, args->os_errbuflen);
}

    صحیح
expand_set_whichwrap(optexpand_T *args, صحیح *رقم_Matches, char_u ***matches)
{
    بازگشت expand_set_opt_listflag(args, (char_u*)WW_ALL, رقم_Matches, matches);
}

/*
 * The 'wildmode' option is changed.
 */
    نویسه *
did_set_wildmode(optset_T *args UNUSED)
{
    اگر (check_opt_wim() == FAIL)
	بازگشت e_invalid_argument;
    بازگشت پوچ;
}

    صحیح
expand_set_wildmode(optexpand_T *args, صحیح *رقم_Matches, char_u ***matches)
{
    بازگشت expand_set_opt_string(
	    args,
	    p_wim_مقادیر,
	    ARRAY_LENGTH(p_wim_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}

/*
 * The 'wildoptions' option is changed.
 */
    نویسه *
did_set_wildoptions(optset_T *args UNUSED)
{
    بازگشت did_set_opt_strings(p_wop, p_wop_مقادیر, TRUE);
}

    صحیح
expand_set_wildoptions(optexpand_T *args, صحیح *رقم_Matches, char_u ***matches)
{
    بازگشت expand_set_opt_string(
	    args,
	    p_wop_مقادیر,
	    ARRAY_LENGTH(p_wop_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}

#اگر defined(FEAT_WAK)
/*
 * The 'winaltkeys' option is changed.
 */
    نویسه *
did_set_winaltkeys(optset_T *args UNUSED)
{
    نویسه *errmsg = پوچ;

    اگر (*p_wak == NUL || check_opt_strings(p_wak, p_wak_مقادیر, FALSE) != OK)
	errmsg = e_invalid_argument;
# ifdef FEAT_MENU
#  اگر defined(FEAT_GUI_MOTIF)
    وگرنه اگر (gui.in_use)
	gui_motif_set_mnemonics(p_wak[0] == 'y' || p_wak[0] == 'm');
#  elif defined(FEAT_GUI_GTK)
    وگرنه اگر (gui.in_use)
	gui_gtk_set_mnemonics(p_wak[0] == 'y' || p_wak[0] == 'm');
#  endif
# endif
    بازگشت errmsg;
}

    صحیح
expand_set_winaltkeys(optexpand_T *args, صحیح *رقم_Matches, char_u ***matches)
{
    بازگشت expand_set_opt_string(
	    args,
	    p_wak_مقادیر,
	    ARRAY_LENGTH(p_wak_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}
#endif

/*
 * The 'wincolor' option is changed.
 */
    نویسه *
did_set_wincolor(optset_T *args UNUSED)
{
    update_wincolor(curwin, args->os_newval.string);
    بازگشت پوچ;
}

/*
 * The 'winhighlight' option is changed
 */
    نویسه *
did_set_winhighlight(optset_T *args)
{
    بازگشت update_winhighlight(curwin, args->os_newval.string);
}

/*
 * Expand 'winhighlight' option.
 */
    صحیح
expand_set_winhighlight(optexpand_T *args, صحیح *رقم_Matches, char_u ***matches)
{
    expand_T	    *xp = args->oe_xp;

    اگر ((xp->xp_pattern > args->oe_set_arg && *(xp->xp_pattern-1) == ':')
	    || xp->xp_pattern == args->oe_set_arg || *(xp->xp_pattern-1) == ',')
    {
	// After a ':' or after a ',', or at the start, so expand highlight
	// group name.

	// If starts with !, then expand 'highlight' occasions.
	اگر (*xp->xp_pattern == '!')
	    بازگشت expand_hl_occasions(args, رقم_Matches, matches, '!');
	وگرنه
	    بازگشت expand_set_opt_generic(
		    args,
		    get_highlight_name,
		    رقم_Matches,
		    matches);
    }

    VIM_CLEAR(*matches);
    بازگشت FAIL;
}

    صحیح
expand_set_wincolor(optexpand_T *args, صحیح *رقم_Matches, char_u ***matches)
{
    بازگشت expand_set_opt_generic(
	    args,
	    get_highlight_name,
	    رقم_Matches,
	    matches);
}

#ifdef FEAT_SYN_HL
/*
 * When the 'syntax' option is set, load the syntax of that name.
 */
    ایستا تهی
do_syntax_autocmd(صحیح مقدار_changed)
{
    ایستا صحیح syn_recursive = 0;

    ++syn_recursive;
    // Only pass TRUE for "force" when the value changed or not used
    // recursively, to avoid endless recurrence.
    apply_autocmds(EVENT_SYNTAX, curbuf->b_p_syn, curbuf->b_fname,
	    مقدار_changed || syn_recursive == 1, curbuf);
    curbuf->b_flags |= BF_SYN_SET;
    --syn_recursive;
}
#endif

/*
 * When the 'filetype' option is set, trigger the FileType autocommand.
 */
    ایستا تهی
do_filetype_autocmd(char_u **varp, صحیح opt_flags, صحیح مقدار_changed)
{
    // Skip this when called from a modeline and the filetype was already set
    // to this value.
    اگر ((opt_flags & OPT_MODELINE) && !مقدار_changed)
	بازگشت;

    ایستا صحیح  ft_recursive = 0;
    صحیح	    secure_save = secure;

    // Reset the secure flag, since the value of 'filetype' has
    // been checked to be safe.
    secure = 0;

    ++ft_recursive;
    curbuf->b_did_filetype = true;
    // Only pass TRUE for "force" when the value changed or not
    // used recursively, to avoid endless recurrence.
    apply_autocmds(EVENT_FILETYPE, curbuf->b_p_ft, curbuf->b_fname,
	    مقدار_changed || ft_recursive == 1, curbuf);
    --ft_recursive;
    // Just in case the old "curbuf" is now invalid.
    اگر (varp != &(curbuf->b_p_ft))
	varp = پوچ;

    secure = secure_save;
}

#ifdef FEAT_SPELL
/*
 * When the 'spelllang' option is set, source the spell/LANG.vim file in
 * 'runtimepath'.
 */
    ایستا تهی
do_spelllang_source(تهی)
{
    char_u	fname[200];
    char_u	*p;
    char_u	*q = curwin->w_s->b_p_spl;

    // Skip the first name if it is "cjk".
    اگر (STRNCMP(q, "cjk,", 4) == 0)
	q += 4;

    // They could set 'spellcapcheck' depending on the language.  Use the first
    // name in 'spelllang' up to '_region' or '.encoding'.
    برای (p = q; *p != NUL; ++p)
	اگر (!ASCII_ISALNUM(*p) && *p != '-')
	    بشکن;
    اگر (p > q)
    {
	vim_snprintf((نویسه *)fname, 200, "spell/%.*s.vim",
		(صحیح)(p - q), q);
	source_runtime(fname, DIP_ALL);
    }
}
#endif

/*
 * Handle string options that need some action to perform when changed.
 * The new value must be allocated.
 * Returns NULL for success, or an untranslated error message for an error.
 */
    نویسه *
did_set_string_option(
    صحیح		opt_idx,		// index in options[] table
    char_u	**varp,			// pointer to the option variable
    char_u	*oldval,		// previous value of the option
    char_u	*مقدار,			// new value of the option
    نویسه	*errbuf,		// buffer for errors, or NULL
    size_t	errbuflen,		// length of error buffer
    صحیح		opt_flags,		// OPT_LOCAL and/or OPT_GLOBAL
    set_op_T    op,			// OP_ADDING/OP_PREPENDING/OP_REMOVING
    صحیح		*مقدار_checked)		// value was checked to be safe, no
					// need to set P_INSECURE
{
    نویسه	*errmsg = پوچ;
    long_u	free_oldval = (get_option_flags(opt_idx) & P_ALLOCED);
    opt_did_set_cb_T did_set_cb = get_option_did_set_cb(opt_idx);
    optset_T	args;

    // 'ttytype' is an alias for 'term'.  Both 'term' and 'ttytype' point to
    // T_NAME.  If 'term' or 'ttytype' is modified, then use the index for the
    // 'term' option.  Only set the P_ALLOCED flag on 'term'.
    اگر (varp == &T_NAME)
    {
	opt_idx = findoption((char_u *)"term");
	اگر (opt_idx >= 0)
	{
	    free_oldval = (get_option_flags(opt_idx) & P_ALLOCED);
	    did_set_cb = get_option_did_set_cb(opt_idx);
	}
    }

    CLEAR_FIELD(args);

    // Disallow changing some options from secure mode
    اگر ((secure
#ifdef HAVE_SANDBOX
		|| sandbox != 0
#endif
		) && (get_option_flags(opt_idx) & P_SECURE))
	errmsg = e_not_allowed_here;
    // Check for a "normal" directory or file name in some options.
    وگرنه اگر (check_illegal_path_names(opt_idx, varp))
	errmsg = e_invalid_argument;
    وگرنه اگر (did_set_cb != پوچ)
    {
	args.os_varp = (char_u *)varp;
	args.os_idx = opt_idx;
	args.os_flags = opt_flags;
	args.os_op = op;
	args.os_oldval.string = oldval;
	args.os_newval.string = مقدار;
	args.os_errbuf = errbuf;
	args.os_errbuflen = errbuflen;
	// Invoke the option specific callback function to validate and apply
	// the new option value.
	errmsg = did_set_cb(&args);

	// The 'keymap', 'filetype' and 'syntax' option callback functions
	// may change the os_value_checked field.
	*مقدار_checked = args.os_مقدار_checked;
    }

    // If an error is detected, restore the previous value.
    اگر (errmsg != پوچ)
    {
	free_string_option(*varp);
	*varp = oldval;
	// When resetting some values, need to act on it.
	اگر (args.os_restore_chartab)
	    (تهی)init_chartab();
	اگر (varp == &p_hl)
	    (تهی)highlight_changed();
    }
    وگرنه
    {
#ifdef FEAT_EVAL
	// Remember where the option was set.
	set_option_sctx_idx(opt_idx, opt_flags, current_sctx);
#endif
	// Free string options that are in allocated memory.
	// Use "free_oldval", because recursiveness may change the flags under
	// our fingers (esp. init_highlight()).
	اگر (free_oldval)
	    free_string_option(oldval);
	set_option_flag(opt_idx, P_ALLOCED);

	اگر ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0
		&& is_global_local_option(opt_idx))
	{
	    // global option with local value set to use global value; free
	    // the local value and make it empty
	    char_u *p = get_option_varp_scope(opt_idx, OPT_LOCAL);
	    free_string_option(*(char_u **)p);
	    *(char_u **)p = empty_option;
	}

	// May set global value for local option.
	وگرنه اگر (!(opt_flags & OPT_LOCAL) && opt_flags != OPT_GLOBAL)
	    set_string_option_global(opt_idx, varp);

	// Trigger the autocommand only after setting the flags.
#ifdef FEAT_SYN_HL
	اگر (varp == &(curbuf->b_p_syn))
	    do_syntax_autocmd(args.os_مقدار_changed);
#endif
	وگرنه اگر (varp == &(curbuf->b_p_ft))
	    do_filetype_autocmd(varp, opt_flags, args.os_مقدار_changed);
#ifdef FEAT_SPELL
	اگر (varp == &(curwin->w_s->b_p_spl))
	    do_spelllang_source();
#endif
    }

    اگر (varp == &p_mouse)
    {
	اگر (*p_mouse == NUL)
	    mch_setmouse(FALSE);    // switch mouse off
	وگرنه
	    setmouse();		    // in case 'mouse' changed
    }

    اگر (varp == &p_rtp)
    {
	export_myvimdir();
#اگر defined(FEAT_LUA)
	update_package_paths_in_lua();
#endif
    }

#اگر defined(FEAT_LINEBREAK)
    // Changing Formatlistpattern when briopt includes the list setting:
    // redraw
    اگر ((varp == &p_flp || varp == &(curbuf->b_p_flp))
	    && curwin->w_briopt_list)
	redraw_all_later(UPD_NOT_VALID);
#endif

    اگر (curwin->w_curswant != MAXCOL
		   && (get_option_flags(opt_idx) & (P_CURSWANT | P_RALL)) != 0
				&& (get_option_flags(opt_idx) & P_HLONLY) == 0)
	curwin->w_set_curswant = true;

    اگر ((opt_flags & OPT_NO_REDRAW) == 0)
    {
#ifdef FEAT_GUI
	// set when changing an option that only requires a redraw in the GUI
	صحیح	redraw_gui_only = FALSE;

	اگر (varp == &p_go			// 'guioptions'
		|| varp == &p_guifont		// 'guifont'
# ifdef FEAT_GUI_TABLINE
		|| varp == &p_gtl		// 'guitablabel'
		|| varp == &p_gtt		// 'guitabtooltip'
# endif
# ifdef FEAT_XFONTSET
		|| varp == &p_guifontset	// 'guifontset'
# endif
		|| varp == &p_guifontwide	// 'guifontwide'
# اگر defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
		|| varp == &p_guiligatures	// 'guiligatures'
# endif
	    )
	    redraw_gui_only = TRUE;

	// check redraw when it's not a GUI option or the GUI is active.
	اگر (!redraw_gui_only || gui.in_use)
#endif
	    check_redraw(get_option_flags(opt_idx));
    }

#اگر defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
    اگر (args.os_did_swaptcap)
    {
	set_termname((char_u *)"win32");
	init_highlight(TRUE, FALSE);
    }
#endif

    بازگشت errmsg;
}

/*
 * Check an option that can be a range of string values.
 *
 * Return OK for correct value, FAIL otherwise.
 * Empty is always OK.
 */
    ایستا صحیح
check_opt_strings(
    char_u	*val,
    نویسه	**مقادیر,
    صحیح		list)	    // when TRUE: accept a list of values
{
    بازگشت opt_strings_flags(val, مقادیر, پوچ, list);
}

/*
 * Handle an option that can be a range of string values.
 * Set a flag in "*flagp" for each string present.
 *
 * Return OK for correct value, FAIL otherwise.
 * Empty is always OK.
 */
    ایستا صحیح
opt_strings_flags(
    char_u	*val,		// new value
    نویسه	**مقادیر,	// array of valid string values
    unsigned	*flagp,
    صحیح		list)		// when TRUE: accept a list of values
{
    صحیح		i;
    صحیح		len;
    unsigned	new_flags = 0;

    وقتی (*val)
    {
	برای (i = 0; ; ++i)
	{
	    اگر (مقادیر[i] == پوچ)	// val not found in values[]
		بازگشت FAIL;

	    len = (صحیح)STRLEN(مقادیر[i]);
	    اگر (STRNCMP(مقادیر[i], val, len) == 0
		    && ((list && val[len] == ',') || val[len] == NUL))
	    {
		val += len + (val[len] == ',');
		new_flags |= (1 << i);
		بشکن;		// check next item in val list
	    }
	}
    }
    اگر (flagp != پوچ)
	*flagp = new_flags;

    بازگشت OK;
}

/*
 * return OK if "p" is a valid fileformat name, FAIL otherwise.
 */
    صحیح
check_ff_مقدار(char_u *p)
{
    بازگشت check_opt_strings(p, p_ff_مقادیر, FALSE);
}

/*
 * Save the actual shortmess Flags and clear them temporarily to avoid that
 * file messages overwrites any output from the following commands.
 *
 * Caller must make sure to first call save_clear_shm_value() and then
 * restore_shm_value() exactly the same number of times.
 */
    تهی
save_clear_shm_مقدار(تهی)
{
    اگر (STRLEN(p_shm) >= SHM_LEN)
    {
	iemsg(e_internal_error_shortmess_too_long);
	بازگشت;
    }

    اگر (++set_shm_recursive == 1)
    {
	STRCPY(shm_buf, p_shm);
	set_option_مقدار_give_err((char_u *)"shm", 0L, (char_u *)"", 0);
    }
}

/*
 * Restore the shortmess Flags set from the save_clear_shm_value() function.
 */
    تهی
restore_shm_مقدار(تهی)
{
    اگر (--set_shm_recursive == 0)
    {
	set_option_مقدار_give_err((char_u *)"shm", 0L, shm_buf, 0);
	vim_memset(shm_buf, 0, SHM_LEN);
    }
}

/*
 * Export the environment variable $MYVIMDIR to the first item in runtimepath
 */
    تهی
export_myvimdir(تهی)
{
    صحیح		dofree = FALSE;
    char_u	*p;
    char_u	*q = p_rtp;
    string_T	buf;

    buf.string = alloc(MAXPATHL);
    اگر (buf.string == پوچ)
	بازگشت;

    buf.length = (size_t)copy_option_part(&q, buf.string, MAXPATHL, ",");

    p = vim_getenv((char_u *)"MYVIMDIR", &dofree);

    اگر (p == پوچ || STRCMP(p, buf.string) != 0)
    {
	اگر (*buf.string != NUL && !after_pathsep(buf.string, buf.string + buf.length))
	    STRCPY(buf.string + buf.length, PATHSEPSTR);
#ifdef MSWIN
	// normalize path separators
	برای (q = buf.string; *q != NUL; q++)
	    اگر (*q == '/')
		*q = '\\';
#endif
	vim_setenv((char_u *)"MYVIMDIR", buf.string);
    }
    اگر (dofree)
	vim_free(p);
    vim_free(buf.string);
}
