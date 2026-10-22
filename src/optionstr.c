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

#ضمن "vim.h"

ساكن char_u shm_buf[SHM_LEN];
ساكن صحيح مجموعه_shm_recursive = 0;

ساكن حرف *(p_ambw_مقادیر[]) = {"single", "double", عدم};
ساكن حرف *(p_bg_مقادیر[]) = {"light", "dark", عدم};
ساكن حرف *(p_bkc_مقادیر[]) = {"yes", "auto", "no", "breaksymlink", "breakhardlink", عدم};
ساكن حرف *(p_bo_مقادیر[]) = {"all", "backspace", "cursor", "complete",
				 "copy", "ctrlg", "error", "esc", "ex",
				 "hangul", "insertmode", "lang", "mess",
				 "showmatch", "operator", "register", "shell",
				 "spell", "term", "wildmode", عدم};
#إذا defined(FEAT_LINEBREAK)
// Note: Keep this in sync with briopt_check()
ساكن حرف *(p_briopt_مقادیر[]) = {"shift:", "min:", "sbr", "list:", "column:", عدم};
#پایان_اگر
#إذا defined(FEAT_TABPANEL)
// Note: Keep this in sync with tabpanelopt_changed()
ساكن حرف *(p_tplo_مقادیر[]) = {"align:", "columns:", "scrollbar", "vert", عدم};
ساكن حرف *(p_tplo_align_مقادیر[]) = {"left", "right", عدم};
#پایان_اگر
#إذا defined(FEAT_DIFF)
// Note: Keep this in sync with diffopt_changed()
ساكن حرف *(p_dip_مقادیر[]) = {"filler", "anchor", "context:", "iblank", "icase", "iwhite", "iwhiteall", "iwhiteeol", "horizontal", "vertical", "closeoff", "hiddenoff", "foldcolumn:", "followwrap", "internal", "indent-heuristic", "algorithm:", "inline:", "linematch:", عدم};
ساكن حرف *(p_dip_algorithm_مقادیر[]) = {"myers", "minimal", "patience", "histogram", عدم};
ساكن حرف *(p_dip_inline_مقادیر[]) = {"none", "simple", "char", "word", عدم};
#پایان_اگر
ساكن حرف *(p_nf_مقادیر[]) = {"bin", "octal", "hex", "alpha", "unsigned", "blank", عدم};
ساكن حرف *(p_ff_مقادیر[]) = {FF_UNIX, FF_DOS, FF_MAC, عدم};
#اگر_تعریف_شده HAVE_CLIPMETHOD
// Note: Keep this in sync with did_set_clipboard()
ساكن حرف *(p_cb_مقادیر[]) = {"unnamed", "unnamedplus",
# ifdef FEAT_CLIPBOARD
    "autoselect", "autoselectplus", "autoselectml", "html", "exclude:",
# endif
    عدم};
#پایان_اگر
#اگر_تعریف_شده FEAT_CRYPT
ساكن حرف *(p_cm_مقادیر[]) = {"zip", "blowfish", "blowfish2",
# ifdef FEAT_SODIUM
    "xchacha20", "xchacha20v2",
# endif
    عدم};
#پایان_اگر
ساكن حرف *(p_cmp_مقادیر[]) = {"internal", "keepascii", عدم};
#اگر_تعریف_شده FEAT_SYN_HL
// Note: Keep this in sync with fill_culopt_flags()
ساكن حرف *(p_culopt_مقادیر[]) = {"line", "screenline", "number", "both", عدم};
#پایان_اگر
ساكن حرف *(p_dy_مقادیر[]) = {"lastline", "truncate", "uhex", عدم};
ساكن حرف *(p_jop_مقادیر[]) = {"stack", عدم};
#اگر_تعریف_شده FEAT_FOLDING
ساكن حرف *(p_fdo_مقادیر[]) = {"all", "block", "hor", "mark", "percent",
				 "quickfix", "search", "tag", "insert",
				 "undo", "jump", عدم};
#پایان_اگر
// Note: Keep this in sync with match_keyprotocol()
ساكن حرف *(p_kpc_protocol_مقادیر[]) = {"none", "mok2", "kitty", عدم};
#اگر_تعریف_شده FEAT_IMAGE
// Note: Keep this in sync with match_imageprotocol()
ساكن حرف *(p_ipc_protocol_مقادیر[]) = {"none", "kitty", "sixel", عدم};
#پایان_اگر
#اگر_تعریف_شده FEAT_PROP_POPUP
// Note: Keep this in sync with parse_popup_option()
ساكن حرف *(p_popup_cpp_option_مقادیر[]) = {"align:", "border:",
    "borderhighlight:", "close:", "height:", "highlight:", "opacity:",
    "resize:", "shadow:", "width:", عدم};
ساكن حرف *(p_popup_pvp_option_مقادیر[]) = {"border:",
    "borderhighlight:", "close:", "height:", "highlight:", "opacity:",
    "resize:", "shadow:", "width:", عدم};
ساكن حرف *(p_popup_option_on_off_مقادیر[]) = {"on", "off", عدم};
ساكن حرف *(p_popup_cpp_border_مقادیر[]) = {"single", "double", "round",
    "ascii", "on", "off", "custom:", عدم};
ساكن حرف *(p_popup_pvp_border_مقادیر[]) = {"single", "double", "round",
    "ascii", "on", "off", "custom:", عدم};
ساكن حرف *(p_popup_option_align_مقادیر[]) = {"item", "menu", عدم};
#پایان_اگر
#إذا defined(FEAT_SPELL)
// Note: Keep this in sync with spell_check_sps()
ساكن حرف *(p_sps_مقادیر[]) = {"best", "fast", "double", "expr:", "file:", "timeout:", عدم};
#پایان_اگر
#اگر_تعریف_شده FEAT_SESSION
// Also used for 'viewoptions'!  Keep in sync with SSOP_ flags.
ساكن حرف *(p_ssop_مقادیر[]) = {"buffers", "winpos", "resize", "winsize",
    "localoptions", "options", "help", "blank", "globals", "slash", "unix",
    "sesdir", "curdir", "folds", "cursor", "tabpages", "terminal", "skiprtp",
    عدم};
#پایان_اگر
#إذا defined(FEAT_STL_OPT)
ساكن حرف *(p_stlo_مقادیر[]) = {"fixedheight", "maxheight:", عدم};
#پایان_اگر
// Keep in sync with SWB_ flags in option.h
ساكن حرف *(p_swb_مقادیر[]) = {"useopen", "usetab", "split", "newtab", "vsplit", "uselast", عدم};
ساكن حرف *(p_spk_مقادیر[]) = {"cursor", "screen", "topline", عدم};
ساكن حرف *(p_tc_مقادیر[]) = {"followic", "ignore", "match", "followscs", "smart", عدم};
// Keep in sync with TCL_ flags in option.h
ساكن حرف *(p_tcl_مقادیر[]) = {"left", "uselast", عدم};
#إذا defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
ساكن حرف *(p_toolbar_مقادیر[]) = {"text", "icons", "tooltips", "horiz", عدم};
#پایان_اگر
#إذا defined(FEAT_TOOLBAR) && defined(FEAT_GUI_GTK)
ساكن حرف *(p_tbis_مقادیر[]) = {"tiny", "small", "medium", "large", "huge", "giant", عدم};
#پایان_اگر
#إذا defined(UNIX) || defined(VMS)
ساكن حرف *(p_ttym_مقادیر[]) = {"xterm", "xterm2", "dec", "netterm", "jsbterm", "pterm", "urxvt", "sgr", عدم};
#پایان_اگر
ساكن حرف *(p_ve_مقادیر[]) = {"block", "insert", "all", "onemore", "none", "NONE", عدم};
// Note: Keep this in sync with check_opt_wim()
ساكن حرف *(p_wim_مقادیر[]) = {"full", "longest", "list", "lastused", "noselect", "noinsert", عدم};
ساكن حرف *(p_wop_مقادیر[]) = {"fuzzy", "tagfile", "pum", "exacttext", عدم};
#اگر_تعریف_شده FEAT_WAK
ساكن حرف *(p_wak_مقادیر[]) = {"yes", "menu", "no", عدم};
#پایان_اگر
ساكن حرف *(p_mousem_مقادیر[]) = {"extend", "popup", "popup_setpos", "mac", عدم};
ساكن حرف *(p_sel_مقادیر[]) = {"inclusive", "exclusive", "old", عدم};
ساكن حرف *(p_slm_مقادیر[]) = {"mouse", "key", "cmd", عدم};
ساكن حرف *(p_km_مقادیر[]) = {"startsel", "stopsel", عدم};
#اگر_تعریف_شده FEAT_BROWSE
ساكن حرف *(p_bsdir_مقادیر[]) = {"current", "last", "buffer", عدم};
#پایان_اگر
ساكن حرف *(p_scbopt_مقادیر[]) = {"ver", "hor", "jump", عدم};
ساكن حرف *(p_debug_مقادیر[]) = {"msg", "throw", "beep", عدم};
ساكن حرف *(p_ead_مقادیر[]) = {"both", "ver", "hor", عدم};
ساكن حرف *(p_buftype_مقادیر[]) = {"nofile", "nowrite", "quickfix", "help", "terminal", "acwrite", "prompt", "popup", عدم};
ساكن حرف *(p_bufhidden_مقادیر[]) = {"hide", "unload", "delete", "wipe", عدم};
ساكن حرف *(p_bs_مقادیر[]) = {"indent", "eol", "start", "nostop", عدم};
#اگر_تعریف_شده FEAT_FOLDING
ساكن حرف *(p_fdm_مقادیر[]) = {"manual", "expr", "marker", "indent", "syntax",
# ifdef FEAT_DIFF
				"diff",
# endif
				عدم};
ساكن حرف *(p_fcl_مقادیر[]) = {"all", عدم};
#پایان_اگر
ساكن حرف *(p_cfc_مقادیر[]) = {"keyword", "files", "whole_line", عدم};
ساكن حرف *(p_cot_مقادیر[]) = {"menu", "menuone", "longest", "preview", "popup", "popuphidden", "noinsert", "noselect", "fuzzy", "nosort", "preinsert", "nearest", عدم};
#اگر_تعریف_شده BACKSLASH_درون_FILENAME
ساكن حرف *(p_csl_مقادیر[]) = {"slash", "backslash", عدم};
#پایان_اگر
#اگر_تعریف_شده FEAT_SIGNS
ساكن حرف *(p_scl_مقادیر[]) = {"yes", "no", "auto", "number", عدم};
#پایان_اگر
#اگر_تعریف_شده UNIX
ساكن حرف *(p_trz_مقادیر[]) = {"inband", "sigwinch", "", عدم};
#پایان_اگر
#إذا defined(MSWIN) && defined(FEAT_TERMINAL)
ساكن حرف *(p_twt_مقادیر[]) = {"winpty", "conpty", "", عدم};
#پایان_اگر
ساكن حرف *(p_sloc_مقادیر[]) = {"last", "statusline", "tabline", عدم};
ساكن حرف *(p_sws_مقادیر[]) = {"fsync", "sync", عدم};

ساكن صحيح check_opt_رشتهها(char_u *val, حرف **مقادیر, صحيح list);
ساكن صحيح opt_رشتهها_flags(char_u *val, حرف **مقادیر, بیعلامت *flagp, صحيح list);

/*
 * After setting various option values: recompute variables that depend on
 * option values.
 */
    فراغ
didset_string_options(فراغ)
{
    (فراغ)opt_رشتهها_flags(p_cmp, p_cmp_مقادیر, &cmp_flags, TRUE);
    (فراغ)opt_رشتهها_flags(p_bkc, p_bkc_مقادیر, &bkc_flags, TRUE);
    (فراغ)opt_رشتهها_flags(p_bo, p_bo_مقادیر, &bo_flags, TRUE);
    (فراغ)opt_رشتهها_flags(p_cot, p_cot_مقادیر, &cot_flags, TRUE);
#اگر_تعریف_شده FEAT_SESSION
    (فراغ)opt_رشتهها_flags(p_ssop, p_ssop_مقادیر, &ssop_flags, TRUE);
    (فراغ)opt_رشتهها_flags(p_vop, p_ssop_مقادیر, &vop_flags, TRUE);
#پایان_اگر
#اگر_تعریف_شده FEAT_FOLDING
    (فراغ)opt_رشتهها_flags(p_fdo, p_fdo_مقادیر, &fdo_flags, TRUE);
#پایان_اگر
    (فراغ)opt_رشتهها_flags(p_dy, p_dy_مقادیر, &dy_flags, TRUE);
    (فراغ)opt_رشتهها_flags(p_jop, p_jop_مقادیر, &jop_flags, TRUE);
    (فراغ)opt_رشتهها_flags(p_tc, p_tc_مقادیر, &tc_flags, FALSE);
    (فراغ)opt_رشتهها_flags(p_ve, p_ve_مقادیر, &ve_flags, TRUE);
#إذا defined(UNIX) || defined(VMS)
    (فراغ)opt_رشتهها_flags(p_ttym, p_ttym_مقادیر, &ttym_flags, FALSE);
#پایان_اگر
#إذا defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
    (فراغ)opt_رشتهها_flags(p_toolbar, p_toolbar_مقادیر, &toolbar_flags, TRUE);
#پایان_اگر
#إذا defined(FEAT_TOOLBAR) && defined(FEAT_GUI_GTK)
    (فراغ)opt_رشتهها_flags(p_tbis, p_tbis_مقادیر, &tbis_flags, FALSE);
#پایان_اگر
    (فراغ)opt_رشتهها_flags(p_swb, p_swb_مقادیر, &swb_flags, TRUE);
    (فراغ)opt_رشتهها_flags(p_tcl, p_tcl_مقادیر, &tcl_flags, TRUE);
}

#إذا defined(FEAT_EVAL)
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
    فراغ
trigger_optionset_string(
	صحيح	opt_idx,
	صحيح	opt_flags,
	char_u  *oldval,
	char_u  *oldval_l,
	char_u  *oldval_g,
	char_u  *newval)
{
    // Don't do this recursively.
    إذا (oldval == عدم || newval == عدم
				    || *get_vim_var_str(VV_OPTION_TYPE) != NUL)
	أرجع;

    char_u buf_type[7];
    size_t buf_typelen;
    size_t oldvallen;

    oldvallen = STRLEN(oldval);
    مجموعه_vim_var_string(VV_OPTION_OLD, oldval, (صحيح)oldvallen);
    مجموعه_vim_var_string(VV_OPTION_NEW, newval, -1);
    buf_typelen = vim_snprintf_safelen((حرف *)buf_type, حجم(buf_type),
	"%s", (opt_flags & OPT_LOCAL) ? "local" : "global");
    مجموعه_vim_var_string(VV_OPTION_TYPE, buf_type, (صحيح)buf_typelen);
    إذا (opt_flags & OPT_LOCAL)
    {
	مجموعه_vim_var_string(VV_OPTION_COMMAND, (char_u *)"setlocal", STRLEN_LITERAL("setlocal"));
	مجموعه_vim_var_string(VV_OPTION_OLDLOCAL, oldval, (صحيح)oldvallen);
    }
    إذا (opt_flags & OPT_GLOBAL)
    {
	مجموعه_vim_var_string(VV_OPTION_COMMAND, (char_u *)"setglobal", STRLEN_LITERAL("setglobal"));
	مجموعه_vim_var_string(VV_OPTION_OLDGLOBAL, oldval, (صحيح)oldvallen);
    }
    إذا ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	مجموعه_vim_var_string(VV_OPTION_COMMAND, (char_u *)"set", STRLEN_LITERAL("set"));
	مجموعه_vim_var_string(VV_OPTION_OLDLOCAL, oldval_l, -1);
	مجموعه_vim_var_string(VV_OPTION_OLDGLOBAL, oldval_g, -1);
    }
    إذا (opt_flags & OPT_MODELINE)
    {
	مجموعه_vim_var_string(VV_OPTION_COMMAND, (char_u *)"modeline", STRLEN_LITERAL("modeline"));
	مجموعه_vim_var_string(VV_OPTION_OLDLOCAL, oldval, (صحيح)oldvallen);
    }
    apply_autocmds(EVENT_OPTIONSET,
	    get_option_fullname(opt_idx), عدم, FALSE,
	    عدم);
    reset_v_option_vars();
}
#پایان_اگر

    ساكن حرف *
illegal_char(حرف *errbuf, size_t errbuflen, صحيح c)
{
    إذا (errbuf == عدم)
	أرجع "";
    vim_snprintf(errbuf, errbuflen, _(e_illegal_character_str),
		    (حرف *)transchar(c));
    أرجع errbuf;
}

    ساكن حرف *
illegal_char_after_chr(حرف *errbuf, size_t errbuflen, صحيح c)
{
    إذا (errbuf == عدم)
	أرجع "";
    vim_snprintf(errbuf, errbuflen, _(e_illegal_character_after_chr), c);
    أرجع errbuf;
}

/*
 * Check string options in a buffer for NULL value.
 */
    فراغ
check_buf_options(buf_T *buf)
{
    check_string_option(&buf->b_p_bh);
    check_string_option(&buf->b_p_bt);
    check_string_option(&buf->b_p_fenc);
    check_string_option(&buf->b_p_ff);
#اگر_تعریف_شده FEAT_پیدا_کن_ID
    check_string_option(&buf->b_p_def);
    check_string_option(&buf->b_p_inc);
# ifdef FEAT_EVAL
    check_string_option(&buf->b_p_inex);
# endif
#پایان_اگر
#إذا defined(FEAT_EVAL)
    check_string_option(&buf->b_p_inde);
    check_string_option(&buf->b_p_indk);
#پایان_اگر
#إذا defined(FEAT_BEVAL) && defined(FEAT_EVAL)
    check_string_option(&buf->b_p_bexpr);
#پایان_اگر
#إذا defined(FEAT_CRYPT)
    check_string_option(&buf->b_p_cm);
#پایان_اگر
    check_string_option(&buf->b_p_fp);
#إذا defined(FEAT_EVAL)
    check_string_option(&buf->b_p_fex);
#پایان_اگر
#اگر_تعریف_شده FEAT_CRYPT
    check_string_option(&buf->b_p_کلید);
#پایان_اگر
    check_string_option(&buf->b_p_kp);
    check_string_option(&buf->b_p_mps);
    check_string_option(&buf->b_p_fo);
    check_string_option(&buf->b_p_flp);
    check_string_option(&buf->b_p_isk);
    check_string_option(&buf->b_p_com);
    check_string_option(&buf->b_p_cms);
    check_string_option(&buf->b_p_nf);
    check_string_option(&buf->b_p_qe);
#اگر_تعریف_شده FEAT_SYN_HL
    check_string_option(&buf->b_p_syn);
    check_string_option(&buf->b_s.b_syn_isk);
#پایان_اگر
#اگر_تعریف_شده FEAT_SPELL
    check_string_option(&buf->b_s.b_p_spc);
    check_string_option(&buf->b_s.b_p_spf);
    check_string_option(&buf->b_s.b_p_spl);
    check_string_option(&buf->b_s.b_p_spo);
#پایان_اگر
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
#اگر_تعریف_شده FEAT_COMPL_FUNC
    check_string_option(&buf->b_p_cfu);
    check_string_option(&buf->b_p_ofu);
    check_string_option(&buf->b_p_tsrfu);
#پایان_اگر
#اگر_تعریف_شده FEAT_EVAL
    check_string_option(&buf->b_p_ffu);
    check_string_option(&buf->b_p_tfu);
#پایان_اگر
#اگر_تعریف_شده FEAT_KEYMAP
    check_string_option(&buf->b_p_keymap);
#پایان_اگر
#اگر_تعریف_شده FEAT_QUICKFIX
    check_string_option(&buf->b_p_gefm);
    check_string_option(&buf->b_p_gp);
    check_string_option(&buf->b_p_mp);
    check_string_option(&buf->b_p_efm);
#پایان_اگر
    check_string_option(&buf->b_p_ep);
    check_string_option(&buf->b_p_مسیر);
    check_string_option(&buf->b_p_برچسبها);
    check_string_option(&buf->b_p_tc);
    check_string_option(&buf->b_p_dict);
#اگر_تعریف_شده FEAT_DIFF
    check_string_option(&buf->b_p_dia);
#پایان_اگر
    check_string_option(&buf->b_p_tsr);
    check_string_option(&buf->b_p_lw);
    check_string_option(&buf->b_p_bkc);
    check_string_option(&buf->b_p_menc);
#اگر_تعریف_شده FEAT_VARTABS
    check_string_option(&buf->b_p_vsts);
    check_string_option(&buf->b_p_vts);
#پایان_اگر
}

/*
 * Free the string allocated for an option.
 * Checks for the string being empty_option. This may happen if we're out of
 * memory, vim_strsave() returned NULL, which was replaced by empty_option by
 * check_options().
 * Does NOT check for P_ALLOCED flag!
 */
    فراغ
free_string_option(char_u *p)
{
    إذا (p != خالی_option)
	vim_free(p);
}

    فراغ
clear_string_option(char_u **pp)
{
    إذا (*pp != خالی_option)
	vim_free(*pp);
    *pp = خالی_option;
}

    فراغ
check_string_option(char_u **pp)
{
    إذا (*pp == عدم)
	*pp = خالی_option;
}

/*
 * Set global value for string option when it's a local option.
 */
    ساكن فراغ
مجموعه_string_option_global(
    صحيح		opt_idx,	// option index
    char_u	**varp)		// pointer to option variable
{
    char_u	**p, *s;

    // the global value is always allocated
    إذا (is_پنجره_local_option(opt_idx))
	p = (char_u **)GLOBAL_WO(varp);
    وإلا
	p = (char_u **)get_option_var(opt_idx);
    إذا (!is_global_option(opt_idx)
	    && p != varp
	    && (s = vim_strsave(*varp)) != عدم)
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
    فراغ
مجموعه_string_option_مستقیم(
    char_u	*name,
    صحيح		opt_idx,
    char_u	*val,
    صحيح		opt_flags,	// OPT_FREE, OPT_LOCAL and/or OPT_GLOBAL
    صحيح		مجموعه_sid استفادهنشده)
{
    char_u	*s;
    char_u	**varp;
    صحيح		both = (opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0;
    صحيح		idx = opt_idx;

    إذا (idx == -1)		// use name
    {
	idx = findoption(name);
	إذا (idx < 0)	// not found (should not happen)
	{
	    semsg(_(e_داخلی_error_str), "set_string_option_direct()");
	    siemsg("For option %s", name);
	    أرجع;
	}
    }

    إذا (is_hidden_option(idx))		// can't set hidden option
	أرجع;

    s = vim_strsave(val);
    إذا (s == عدم)
	أرجع;

    varp = (char_u **)get_option_varp_scope(idx,
	    both ? OPT_LOCAL : opt_flags);
    إذا ((opt_flags & OPT_FREE) && (get_option_flags(idx) & P_ALLOCED))
	free_string_option(*varp);
    *varp = s;

    // For buffer/window local option may also set the global value.
    إذا (both)
	مجموعه_string_option_global(idx, varp);

    مجموعه_option_flag(idx, P_ALLOCED);

    // When setting both values of a global option with a local value,
    // make the local value empty, so that the global value is used.
    إذا (is_global_local_option(idx) && both)
    {
	free_string_option(*varp);
	*varp = خالی_option;
    }
#اگر_تعریف_شده FEAT_EVAL
    إذا (مجموعه_sid != SID_NONE)
    {
	sctx_T script_ctx;

	إذا (مجموعه_sid == 0)
	    script_ctx = current_sctx;
	وإلا
	{
	    script_ctx.sc_sid = مجموعه_sid;
	    script_ctx.sc_seq = 0;
	    script_ctx.sc_lnum = 0;
	    script_ctx.sc_نسخه = 1;
	}
	مجموعه_option_sctx_idx(idx, opt_flags, script_ctx);
    }
#پایان_اگر
}

/*
 * Like set_string_option_direct(), but for a window-local option in "wp".
 * Blocks autocommands to avoid the old curwin becoming invalid.
 */
    فراغ
مجموعه_string_option_مستقیم_درون_win(
	win_T		*wp,
	char_u		*name,
	صحيح		opt_idx,
	char_u		*val,
	صحيح		opt_flags,
	صحيح		مجموعه_sid)
{
    win_T	*save_curwin = curwin;

    بلوک_autocmds();
    curwin = wp;
    curbuf = curwin->w_میانگیر;
    مجموعه_string_option_مستقیم(name, opt_idx, val, opt_flags, مجموعه_sid);
    curwin = save_curwin;
    curbuf = curwin->w_میانگیر;
    unblock_autocmds();
}

#إذا defined(FEAT_PROP_POPUP)
/*
 * Like set_string_option_direct(), but for a buffer-local option in "buf".
 * Blocks autocommands to avoid the old curbuf becoming invalid.
 */
    فراغ
مجموعه_string_option_مستقیم_درون_buf(
	buf_T		*buf,
	char_u		*name,
	صحيح		opt_idx,
	char_u		*val,
	صحيح		opt_flags,
	صحيح		مجموعه_sid)
{
    buf_T	*save_curbuf = curbuf;

    بلوک_autocmds();
    curbuf = buf;
    curwin->w_میانگیر = curbuf;
    مجموعه_string_option_مستقیم(name, opt_idx, val, opt_flags, مجموعه_sid);
    curbuf = save_curbuf;
    curwin->w_میانگیر = curbuf;
    unblock_autocmds();
}
#پایان_اگر

/*
 * Set a string option to a new value, and handle the effects.
 *
 * Returns NULL on success or an untranslated error message on error.
 */
    حرف *
مجموعه_string_option(
    صحيح		opt_idx,
    char_u	*مقدار,
    صحيح		opt_flags,	// OPT_LOCAL and/or OPT_GLOBAL
    حرف	*errbuf,
    size_t	errbuflen)
{
    char_u	*s;
    char_u	**varp;
    char_u	*oldval;
#إذا defined(FEAT_EVAL)
    char_u	*oldval_l = عدم;
    char_u	*oldval_g = عدم;
    char_u	*saved_oldval = عدم;
    char_u	*saved_oldval_l = عدم;
    char_u	*saved_oldval_g = عدم;
    char_u	*saved_newval = عدم;
#پایان_اگر
    حرف	*errmsg = عدم;
    صحيح		مقدار_checked = FALSE;

    إذا (is_hidden_option(opt_idx))	// don't set hidden option
	أرجع عدم;

    s = vim_strsave(مقدار == عدم ? (char_u *)"" : مقدار);
    إذا (s == عدم)
	أرجع عدم;

    varp = (char_u **)get_option_varp_scope(opt_idx,
	    (opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0
	    ? (is_global_local_option(opt_idx)
		? OPT_GLOBAL : OPT_LOCAL)
	    : opt_flags);
    oldval = *varp;
#إذا defined(FEAT_EVAL)
    إذا ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	oldval_l = *(char_u **)get_option_varp_scope(opt_idx, OPT_LOCAL);
	oldval_g = *(char_u **)get_option_varp_scope(opt_idx, OPT_GLOBAL);
    }
#پایان_اگر
    *varp = s;

#إذا defined(FEAT_EVAL)
    إذا (!starting
# ifdef FEAT_CRYPT
	    && !is_crypt_کلید_option(opt_idx)
# endif
       )
    {
	إذا (oldval_l != عدم)
	    saved_oldval_l = vim_strsave(oldval_l);
	إذا (oldval_g != عدم)
	    saved_oldval_g = vim_strsave(oldval_g);
	saved_oldval = vim_strsave(oldval);
	saved_newval = vim_strsave(s);
    }
#پایان_اگر
    إذا ((errmsg = did_مجموعه_string_option(opt_idx, varp, oldval, مقدار, errbuf,
		    errbuflen, opt_flags, OP_NONE, &مقدار_checked)) == عدم)
	did_مجموعه_option(opt_idx, opt_flags, TRUE, مقدار_checked);

#إذا defined(FEAT_EVAL)
    // call autocommand after handling side effects
    إذا (errmsg == عدم)
	trigger_optionset_string(opt_idx, opt_flags,
		saved_oldval, saved_oldval_l,
		saved_oldval_g, saved_newval);
    vim_free(saved_oldval);
    vim_free(saved_oldval_l);
    vim_free(saved_oldval_g);
    vim_free(saved_newval);
#پایان_اگر
    أرجع errmsg;
}

/*
 * Return TRUE if "val" is a valid 'filetype' name.
 * Also used for 'syntax' and 'keymap'.
 */
    ساكن صحيح
valid_filetype(char_u *val)
{
    أرجع valid_name(val, ".-_");
}

#اگر_تعریف_شده FEAT_STL_OPT
/*
 * Check validity of options with the 'statusline' format.
 * Return an untranslated error message or NULL.
 */
    ساكن حرف *
check_stl_option(char_u *s)
{
    صحيح		groupdepth = 0;
    ساكن حرف errbuf[ERR_BUFLEN];
    صحيح		errbuflen = ERR_BUFLEN;

    طالما (*s)
    {
	// Check for valid keys after % sequences
	طالما (*s && *s != '%')
	    s++;
	إذا (!*s)
	    اكسر;
	s++;
	إذا (*s == STL_CLICKFUNC)
	{
	    إذا (s[1] == ']')
	    {
		// %[] - end click region
		s += 2;
		استمر;
	    }
	    إذا (ASCII_ISALPHA(s[1]) || s[1] == '_')
	    {
		// %[FuncName] - start click region
		char_u *rb = vim_strchr(s + 2, ']');
		إذا (rb != عدم)
		{
		    s = rb + 1;
		    استمر;
		}
	    }
	    // Bare %[ is invalid
	    أرجع illegal_char(errbuf, errbuflen, *s);
	}
	إذا (*s == STL_LINEBREAK)
	{
	    // Plain %@ - line break
	    s++;
	    استمر;
	}
	إذا (*s == '%' || *s == STL_TRUNCMARK || *s == STL_SEPARATE)
	{
	    s++;
	    استمر;
	}
	إذا (*s == ')')
	{
	    s++;
	    إذا (--groupdepth < 0)
		اكسر;
	    استمر;
	}
	إذا (*s == '-')
	    s++;
	طالما (VIM_ISDIGIT(*s))
	    s++;
	إذا (*s == STL_USER_HL)
	    استمر;
	إذا (*s == STL_CLICKFUNC)
	{
	    // %N[FuncName] or %N[]
	    إذا (s[1] == ']')
	    {
		s += 2;
		استمر;
	    }
	    إذا (ASCII_ISALPHA(s[1]) || s[1] == '_')
	    {
		char_u *rb = vim_strchr(s + 2, ']');
		إذا (rb != عدم)
		{
		    s = rb + 1;
		    استمر;
		}
	    }
	    // Bare %N[ is invalid
	    أرجع illegal_char(errbuf, errbuflen, *s);
	}
	إذا (*s == STL_LINEBREAK)
	{
	    // %N@ - line break
	    s++;
	    استمر;
	}
	إذا (*s == '.')
	{
	    s++;
	    طالما (*s && VIM_ISDIGIT(*s))
		s++;
	}
	إذا (*s == '(')
	{
	    groupdepth++;
	    استمر;
	}
	إذا (vim_strchr(STL_ALL, *s) == عدم)
	{
	    أرجع illegal_char(errbuf, errbuflen, *s);
	}
	إذا (*s == '{')
	{
	    صحيح reevaluate = (*++s == '%');

	    إذا (reevaluate && *++s == '}')
		// "}" is not allowed immediately after "%{%"
		أرجع illegal_char(errbuf, errbuflen, '}');
	    طالما ((*s != '}' || (reevaluate && s[-1] != '%')) && *s)
		s++;
	    إذا (*s != '}')
		أرجع e_unclosed_عبارت_sequence;
	}
    }
    إذا (groupdepth != 0)
	أرجع e_unbalanced_groups;
    أرجع عدم;
}
#پایان_اگر

/*
 * Check for a "normal" directory or file name in some options.  Disallow a
 * path separator (slash and/or backslash), wildcards and characters that are
 * often illegal in a file name. Be more permissive if "secure" is off.
 */
    ساكن صحيح
check_illegal_مسیر_نامها(صحيح opt_idx, char_u **varp)
{
    أرجع (((get_option_flags(opt_idx) & P_NFNAME)
		&& vim_strpbrk(*varp, (char_u *)(secure
			? "/\\*?[|;&<>\r\n" : "/\\*?[<>\r\n")) != عدم)
	    || ((get_option_flags(opt_idx) & P_NDNAME)
		&& vim_strpbrk(*varp, (char_u *)"*?[|;&<>\r\n") != عدم));
}

/*
 * An option that accepts a list of flags is changed.
 * e.g. 'viewoptions', 'switchbuf', 'casemap', etc.
 */
    ساكن حرف *
did_مجموعه_opt_flags(char_u *val, حرف **مقادیر, بیعلامت *flagp, صحيح list)
{
    إذا (opt_رشتهها_flags(val, مقادیر, flagp, list) == FAIL)
	أرجع e_invalid_آرگومان;

    أرجع عدم;
}

/*
 * An option that accepts a list of string values is changed.
 * e.g. 'nrformats', 'scrollopt', 'wildoptions', etc.
 */
    ساكن حرف *
did_مجموعه_opt_رشتهها(char_u *val, حرف **مقادیر, صحيح list)
{
    أرجع did_مجموعه_opt_flags(val, مقادیر, عدم, list);
}

/*
 * An option which is a list of flags is set.  Valid values are in 'flags'.
 */
    ساكن حرف *
did_مجموعه_option_listflag(
	char_u *val,
	char_u *flags,
	حرف *errbuf,
	size_t errbuflen)
{
    char_u	*s;

    لكل (s = val; *s; ++s)
	إذا (vim_strchr(flags, *s) == عدم)
	    أرجع illegal_char(errbuf, errbuflen, *s);

    أرجع عدم;
}

/*
 * Expand an option that accepts a list of fixed string values with known
 * number of items.
 */
    ساكن صحيح
expand_مجموعه_opt_string(
	optexpand_T *آرگومانها,
	حرف **مقادیر,
	size_t رقم_مقادیر,
	صحيح *رقم_Matches,
	char_u ***matches)
{
    char_u	*p;
    regmatch_T	*regmatch = آرگومانها->oe_regmatch;
    صحيح		include_orig_val = آرگومانها->oe_include_orig_val;
    char_u	*option_val = آرگومانها->oe_opt_مقدار;

    // Assume numValues is small since they are fixed enums, so just allocate
    // upfront instead of needing two passes to calculate output size.
    *matches = ALLOC_MULT(char_u *, رقم_مقادیر + 1);
    إذا (*matches == عدم)
	أرجع FAIL;

    صحيح شمار = 0;

    إذا (include_orig_val && *option_val != NUL)
    {
	p = vim_strsave(option_val);
	إذا (p == عدم)
	{
	    VIM_CLEAR(*matches);
	    أرجع FAIL;
	}
	(*matches)[شمار++] = p;
    }

    لكل (حرف **val = مقادیر; *val != عدم; val++)
    {
	إذا (include_orig_val && *option_val != NUL)
	{
	    إذا (STRCMP((char_u*)*val, option_val) == 0)
		استمر;
	}
	إذا (vim_regexec(regmatch, (char_u*)(*val), (colnr_T)0))
	{
	    p = vim_strsave((char_u*)*val);
	    إذا (p == عدم)
	    {
		إذا (شمار == 0)
		{
		    VIM_CLEAR(*matches);
		    أرجع FAIL;
		}
		وإلا
		    اكسر;
	    }
	    (*matches)[شمار++] = p;
	}
    }
    إذا (شمار == 0)
    {
	VIM_CLEAR(*matches);
	أرجع FAIL;
    }
    *رقم_Matches = شمار;
    أرجع OK;
}

ساكن char_u *مجموعه_opt_callback_orig_option = عدم;
ساكن char_u *((*مجموعه_opt_callback_func)(expand_T *, صحيح));

/*
 * Callback used by expand_set_opt_generic to also include the original value
 * as the first item.
 */
    ساكن char_u *
expand_مجموعه_opt_generic_cb(expand_T *xp, صحيح idx)
{
    إذا (idx == 0)
    {
	إذا (مجموعه_opt_callback_orig_option != عدم)
	    أرجع مجموعه_opt_callback_orig_option;
	وإلا
	    أرجع (char_u *)""; // empty strings are ignored
    }
    أرجع مجموعه_opt_callback_func(xp, idx - 1);
}

/*
 * Expand an option with a callback that iterates through a list of possible
 * names using an index.
 */
    ساكن صحيح
expand_مجموعه_opt_generic(
	optexpand_T *آرگومانها,
	char_u *((*func)(expand_T *, صحيح)),
	صحيح *رقم_Matches,
	char_u ***matches)
{
    صحيح ret;

    مجموعه_opt_callback_orig_option = آرگومانها->oe_include_orig_val ?
	آرگومانها->oe_opt_مقدار : عدم;
    مجموعه_opt_callback_func = func;

    ret = ExpandGeneric(
	    (char_u*)"", // not using fuzzy as currently EXPAND_STRING_SETTING doesn't use it
	    آرگومانها->oe_xp,
	    آرگومانها->oe_regmatch,
	    matches,
	    رقم_Matches,
	    expand_مجموعه_opt_generic_cb,
	    FALSE);

    مجموعه_opt_callback_orig_option = عدم;
    مجموعه_opt_callback_func = عدم;
    أرجع ret;
}

#إذا defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
ساكن garray_T *expand_cb_ga;
ساكن optexpand_T *expand_cb_آرگومانها;

/*
 * Callback provided to a function in expand_set_opt_callback. Will perform
 * regex matching against the value and add to the list.
 *
 * Returns OK usually. Returns FAIL if it failed to allocate memory, and the
 * caller should terminate the enumeration.
 */
    ساكن صحيح
expand_مجموعه_opt_callback_cb(char_u *val)
{
    regmatch_T	*regmatch = expand_cb_آرگومانها->oe_regmatch;
    expand_T	*xp = expand_cb_آرگومانها->oe_xp;
    garray_T	*ga = expand_cb_ga;
    char_u	*str;

    إذا (val == عدم || *val == NUL)
	أرجع OK;

    إذا (xp->xp_الگو[0] != NUL &&
	    !vim_regexec(regmatch, val, (colnr_T)0))
	أرجع OK;

    str = vim_strsave_escaped(val, (char_u *)" \t\\");

    إذا (str == عدم)
	أرجع FAIL;

    إذا (ga_grow(ga, 1) == FAIL)
    {
	vim_free(str);
	أرجع FAIL;
    }

    ((char_u **)ga->ga_داده)[ga->ga_len] = str;
    ++ga->ga_len;
    أرجع OK;
}

/*
 * Expand an option with a provided function that takes a callback. The
 * function will enumerate through all options and call the callback to add it
 * to the list.
 *
 * "func" is the enumerator function that will generate the list of options.
 * "func_params" is a single parameter that will be passed to func.
 */
    ساكن صحيح
expand_مجموعه_opt_callback(
	optexpand_T *آرگومانها,
	فراغ (*func)(optexpand_T *, فراغ* params, صحيح (*cb)(char_u *val)),
	فراغ *func_params,
	صحيح *رقم_Matches,
	char_u ***matches)
{
    garray_T	ga;
    صحيح		include_orig_val = آرگومانها->oe_include_orig_val;
    char_u	*option_val = آرگومانها->oe_opt_مقدار;

    ga_init2(&ga, حجم(حرف *), 30);

    إذا (include_orig_val && *option_val != NUL)
    {
	char_u *p = vim_strsave(option_val);
	إذا (p == عدم)
	    أرجع FAIL;
	إذا (ga_grow(&ga, 1) == FAIL)
	{
	    vim_free(p);
	    أرجع FAIL;
	}
	((char_u **)ga.ga_داده)[ga.ga_len] = p;
	++ga.ga_len;
    }

    expand_cb_ga = &ga;
    expand_cb_آرگومانها = آرگومانها;

    func(آرگومانها, func_params, expand_مجموعه_opt_callback_cb);

    expand_cb_ga = عدم;
    expand_cb_آرگومانها = عدم;

    *matches = ga.ga_داده;
    *رقم_Matches = ga.ga_len;
    أرجع OK;
}
#پایان_اگر

/*
 * Expand an option which is a list of flags.
 */
    ساكن صحيح
expand_مجموعه_opt_listflag(
	optexpand_T *آرگومانها,
	char_u *flags,
	صحيح *رقم_Matches,
	char_u ***matches)
{
    char_u  *p;
    char_u  *option_val = آرگومانها->oe_opt_مقدار;
    char_u  *cmdline_val = آرگومانها->oe_مجموعه_arg;
    صحيح	    append = آرگومانها->oe_append;
    صحيح	    include_orig_val = آرگومانها->oe_include_orig_val && (*option_val != NUL);

    size_t رقم_flags = STRLEN(flags);

    // Assume we only have small number of flags, so just allocate max size.
    *matches = ALLOC_MULT(char_u *, رقم_flags + 1);
    إذا (*matches == عدم)
	أرجع FAIL;

    صحيح شمار = 0;

    إذا (include_orig_val)
    {
	p = vim_strsave(option_val);
	إذا (p == عدم)
	{
	    VIM_CLEAR(*matches);
	    أرجع FAIL;
	}
	(*matches)[شمار++] = p;
    }

    لكل (char_u *flag = flags; *flag != NUL; flag++)
    {
	إذا (append && vim_strchr(option_val, *flag) != عدم)
	    استمر;

	إذا (vim_strchr(cmdline_val, *flag) == عدم)
	{
	    إذا (include_orig_val
		    && option_val[1] == NUL
		    && *flag == option_val[0])
	    {
		// This value is already used as the first choice as it's the
		// existing flag. Just skip it to avoid duplicate.
		استمر;
	    }
	    p = vim_strnsave(flag, 1);
	    إذا (p == عدم)
	    {
		إذا (شمار == 0)
		{
		    VIM_CLEAR(*matches);
		    أرجع FAIL;
		}
		وإلا
		    اكسر;
	    }
	    (*matches)[شمار++] = p;
	}
    }

    إذا (شمار == 0)
    {
	VIM_CLEAR(*matches);
	أرجع FAIL;
    }
    *رقم_Matches = شمار;
    أرجع OK;
}

/*
 * The 'ambiwidth' option is changed.
 */
    حرف *
did_مجموعه_ambiwidth(optset_T *آرگومانها استفادهنشده)
{
    إذا (check_opt_رشتهها(p_ambw, p_ambw_مقادیر, FALSE) != OK)
	أرجع e_invalid_آرگومان;

    أرجع check_chars_options();
}

// "name" must be a string literal, the length is computed at compile time.
#تعریف_کن completing_مقدار_for_subopt(آرگومانها, name) \
	  completing_مقدار_for_subopt_len(آرگومانها, name, (صحيح)STRLEN_LITERAL(name))

/*
 * Return true when completing the value of the sub-option "name" with length
 * "len", e.g. the value after "close:" in 'completepopup'.
 */
    ساكن bool
completing_مقدار_for_subopt_len(optexpand_T *آرگومانها, حرف *name, صحيح len)
{
    char_u  *colon = آرگومانها->oe_xp->xp_الگو - 1;
    صحيح	    off = (صحيح)(colon - آرگومانها->oe_مجموعه_arg);

    إذا (off < len)
	أرجع false;
    // The name must follow a comma when it does not start the option value.
    إذا (off > len && *(colon - len - 1) != ',')
	أرجع false;

    أرجع STRNCMP(colon - len, name, len) == 0;
}

    صحيح
expand_مجموعه_ambiwidth(optexpand_T *آرگومانها, صحيح *رقم_Matches, char_u ***matches)
{
    أرجع expand_مجموعه_opt_string(
	    آرگومانها,
	    p_ambw_مقادیر,
	    ARRAY_درازی(p_ambw_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}

/*
 * The 'background' option is changed.
 */
    حرف *
did_مجموعه_background(optset_T *آرگومانها)
{
    إذا (check_opt_رشتهها(p_bg, p_bg_مقادیر, FALSE) == FAIL)
	أرجع e_invalid_آرگومان;

    إذا (آرگومانها->os_oldval.string != عدم && آرگومانها->os_oldval.string[0] == *p_bg)
	// Value was not changed
	أرجع عدم;

#اگر_تعریف_شده FEAT_EVAL
    صحيح dark = (*p_bg == 'd');
#پایان_اگر

    init_highlight(FALSE, FALSE);

#اگر_تعریف_شده FEAT_EVAL
    إذا (dark != (*p_bg == 'd')
	    && get_var_مقدار((char_u *)"g:colors_name") != عدم)
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
#پایان_اگر
#اگر_تعریف_شده FEAT_TERMINAL
    جمله_update_colors_all();
#پایان_اگر

    أرجع عدم;
}

    صحيح
expand_مجموعه_background(optexpand_T *آرگومانها, صحيح *رقم_Matches, char_u ***matches)
{
    أرجع expand_مجموعه_opt_string(
	    آرگومانها,
	    p_bg_مقادیر,
	    ARRAY_درازی(p_bg_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}

/*
 * The 'backspace' option is changed.
 */
    حرف *
did_مجموعه_backspace(optset_T *آرگومانها استفادهنشده)
{
    إذا (VIM_ISDIGIT(*p_bs))
    {
	إذا (*p_bs > '3' || p_bs[1] != NUL)
	    أرجع e_invalid_آرگومان;
    }
    وإلا إذا (check_opt_رشتهها(p_bs, p_bs_مقادیر, TRUE) != OK)
	أرجع e_invalid_آرگومان;

    أرجع عدم;
}

    صحيح
expand_مجموعه_backspace(optexpand_T *آرگومانها, صحيح *رقم_Matches, char_u ***matches)
{
    أرجع expand_مجموعه_opt_string(
	    آرگومانها,
	    p_bs_مقادیر,
	    ARRAY_درازی(p_bs_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}

/*
 * The 'backupcopy' option is changed.
 */
    حرف *
did_مجموعه_backupcopy(optset_T *آرگومانها)
{
    char_u		*bkc = p_bkc;
    بیعلامت صحيح	*flags = &bkc_flags;
    حرف		*errmsg = عدم;

    إذا (آرگومانها->os_flags & OPT_LOCAL)
    {
	bkc = curbuf->b_p_bkc;
	flags = &curbuf->b_bkc_flags;
    }
    وإلا إذا (!(آرگومانها->os_flags & OPT_GLOBAL))
	// When using :set, clear the local flags.
	curbuf->b_bkc_flags = 0;

    إذا ((آرگومانها->os_flags & OPT_LOCAL) && *bkc == NUL)
	// make the local value empty: use the global value
	*flags = 0;
    وإلا
    {
	إذا (opt_رشتهها_flags(bkc, p_bkc_مقادیر, flags, TRUE) != OK)
	    errmsg = e_invalid_آرگومان;
	إذا ((((صحيح)*flags & BKC_AUTO) != 0)
		+ (((صحيح)*flags & BKC_YES) != 0)
		+ (((صحيح)*flags & BKC_NO) != 0) != 1)
	{
	    // Must have exactly one of "auto", "yes"  and "no".
	    (فراغ)opt_رشتهها_flags(آرگومانها->os_oldval.string, p_bkc_مقادیر,
								  flags, TRUE);
	    errmsg = e_invalid_آرگومان;
	}
    }

    أرجع errmsg;
}

    صحيح
expand_مجموعه_backupcopy(optexpand_T *آرگومانها, صحيح *رقم_Matches, char_u ***matches)
{
    أرجع expand_مجموعه_opt_string(
	    آرگومانها,
	    p_bkc_مقادیر,
	    ARRAY_درازی(p_bkc_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}

/*
 * The 'backupext' or the 'patchmode' option is changed.
 */
    حرف *
did_مجموعه_backupext_or_patchmode(optset_T *آرگومانها استفادهنشده)
{
    إذا (STRCMP(*p_bex == '.' ? p_bex + 1 : p_bex,
		*p_pm == '.' ? p_pm + 1 : p_pm) == 0)
	أرجع e_backupext_and_patchmode_are_equal;

    أرجع عدم;
}

/*
 * The 'belloff' option is changed.
 */
    حرف *
did_مجموعه_belloff(optset_T *آرگومانها استفادهنشده)
{
    أرجع did_مجموعه_opt_flags(p_bo, p_bo_مقادیر, &bo_flags, TRUE);
}

    صحيح
expand_مجموعه_belloff(optexpand_T *آرگومانها, صحيح *رقم_Matches, char_u ***matches)
{
    أرجع expand_مجموعه_opt_string(
	    آرگومانها,
	    p_bo_مقادیر,
	    ARRAY_درازی(p_bo_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}

#إذا defined(FEAT_LINEBREAK)
/*
 * The 'breakat' option is changed.
 */
    حرف *
did_مجموعه_breakat(optset_T *آرگومانها استفادهنشده)
{
    char_u	*p;
    صحيح		i;

    لكل (i = 0; i < 256; i++)
	breakat_flags[i] = FALSE;

    إذا (p_breakat != عدم)
	لكل (p = p_breakat; *p; p++)
	    breakat_flags[*p] = TRUE;

    أرجع عدم;
}

/*
 * The 'breakindentopt' option is changed.
 */
    حرف *
did_مجموعه_breakindentopt(optset_T *آرگومانها)
{
    char_u	**varp = (char_u **)آرگومانها->os_varp;

    إذا (briopt_check(*varp, varp == &curwin->w_p_briopt ? curwin : عدم)
								      == FAIL)
	أرجع e_invalid_آرگومان;

    // list setting requires a redraw
    إذا (varp == &curwin->w_p_briopt && curwin->w_briopt_list)
	redraw_all_later(UPD_NOT_VALID);

    أرجع عدم;
}

    صحيح
expand_مجموعه_breakindentopt(
    optexpand_T		*آرگومانها,
    صحيح			*رقم_Matches,
    char_u		***matches)
{
    أرجع expand_مجموعه_opt_string(
	    آرگومانها,
	    p_briopt_مقادیر,
	    ARRAY_درازی(p_briopt_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}
#پایان_اگر

#إذا defined(FEAT_BROWSE)
/*
 * The 'browsedir' option is changed.
 */
    حرف *
did_مجموعه_browsedir(optset_T *آرگومانها استفادهنشده)
{
    إذا (check_opt_رشتهها(p_bsdir, p_bsdir_مقادیر, FALSE) != OK
	    && !mch_isdir(p_bsdir))
	أرجع e_invalid_آرگومان;

    أرجع عدم;
}

    صحيح
expand_مجموعه_browsedir(optexpand_T *آرگومانها, صحيح *رقم_Matches, char_u ***matches)
{
    أرجع expand_مجموعه_opt_string(
	    آرگومانها,
	    p_bsdir_مقادیر,
	    ARRAY_درازی(p_bsdir_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}
#پایان_اگر

/*
 * The 'bufhidden' option is changed.
 */
    حرف *
did_مجموعه_bufhidden(optset_T *آرگومانها استفادهنشده)
{
    أرجع did_مجموعه_opt_رشتهها(curbuf->b_p_bh, p_bufhidden_مقادیر, FALSE);
}

    صحيح
expand_مجموعه_bufhidden(optexpand_T *آرگومانها, صحيح *رقم_Matches, char_u ***matches)
{
    أرجع expand_مجموعه_opt_string(
	    آرگومانها,
	    p_bufhidden_مقادیر,
	    ARRAY_درازی(p_bufhidden_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}

/*
 * The 'buftype' option is changed.
 */
    حرف *
did_مجموعه_buftype(optset_T *آرگومانها استفادهنشده)
{
    إذا (check_opt_رشتهها(curbuf->b_p_bt, p_buftype_مقادیر, FALSE) != OK)
	أرجع e_invalid_آرگومان;

    إذا (curwin->w_وضعیت_ارتفاع)
    {
	curwin->w_redr_وضعیت = true;
	redraw_later(UPD_VALID);
    }
    curbuf->b_help = (curbuf->b_p_bt[0] == 'h');
    redraw_titles();

    أرجع عدم;
}

    صحيح
expand_مجموعه_buftype(optexpand_T *آرگومانها, صحيح *رقم_Matches, char_u ***matches)
{
    أرجع expand_مجموعه_opt_string(
	    آرگومانها,
	    p_buftype_مقادیر,
	    ARRAY_درازی(p_buftype_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}

/*
 * The 'casemap' option is changed.
 */
    حرف *
did_مجموعه_casemap(optset_T *آرگومانها استفادهنشده)
{
    أرجع did_مجموعه_opt_flags(p_cmp, p_cmp_مقادیر, &cmp_flags, TRUE);
}

    صحيح
expand_مجموعه_casemap(optexpand_T *آرگومانها, صحيح *رقم_Matches, char_u ***matches)
{
    أرجع expand_مجموعه_opt_string(
	    آرگومانها,
	    p_cmp_مقادیر,
	    ARRAY_درازی(p_cmp_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}

#إذا defined(HAVE_CLIPMETHOD)
    صحيح
expand_مجموعه_clipboard(optexpand_T *آرگومانها, صحيح *رقم_Matches, char_u ***matches)
{
    أرجع expand_مجموعه_opt_string(
	    آرگومانها,
	    p_cb_مقادیر,
	    ARRAY_درازی(p_cb_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}
#پایان_اگر

#اگر_تعریف_شده HAVE_CLIPMETHOD
    حرف *
did_مجموعه_clipmethod(optset_T *آرگومانها استفادهنشده)
{
    أرجع انتخاب_clipmethod();
}

    صحيح
expand_مجموعه_clipmethod(optexpand_T *آرگومانها, صحيح *رقم_Matches, char_u ***matches)
{
    // We want to expand using the predefined clipmethod values + clipboard
    // provider names.
    صحيح		نتیجه;
    حرف	**مقادیر;
    صحيح		شمار, pos = 0, شروع = 0;
# ifdef FEAT_EVAL
    dict_T	*providers = get_vim_var_dict(VV_CLIPPROVIDERS);
# وإلا
    dict_T	*providers = عدم;
# endif
    hashtab_T	*ht = providers == عدم ? عدم : &providers->dv_hashtab;

    شمار = (ht == عدم ? 0 : ht->ht_استفاده_شده);
# ifdef FEAT_WAYLAND_CLIPBOARD
    شمار++;
    شروع++;
# endif
# ifdef FEAT_XCLIPBOARD
    شمار++;
    شروع++;
# endif
    مقادیر = ALLOC_MULT(حرف *, شمار + 1); // Add NULL terminator too

    إذا (مقادیر == عدم)
	أرجع FAIL;

# ifdef FEAT_WAYLAND_CLIPBOARD
    مقادیر[pos++] = "wayland";
# endif
# ifdef FEAT_XCLIPBOARD
    مقادیر[pos++] = "x11";
# endif

    إذا (ht != عدم)
	لكل (long_u i = 0; i < ht->ht_ماسک + 1; i++)
	{
	    hashitem_T	*hi = ht->ht_array + i;

	    إذا (!HASHITEM_خالی(hi))
		مقادیر[pos++] = (حرف *)vim_strsave(hi->hi_کلید);
	}
    مقادیر[pos++] = عدم;

    نتیجه = expand_مجموعه_opt_string(
	    آرگومانها,
	    مقادیر,
	    شمار,
	    رقم_Matches,
	    matches);

    لكل (صحيح i = شروع; i < شمار; i++)
	vim_free(مقادیر[i]);
    vim_free(مقادیر);

    أرجع نتیجه;
}
#پایان_اگر

/*
 * The global 'listchars' or 'fillchars' option is changed.
 */
    ساكن حرف *
did_مجموعه_global_listfillchars(char_u *val, صحيح opt_lcs, صحيح opt_flags,
						حرف *errbuf, size_t errbuflen)
{
    حرف	*errmsg = عدم;
    char_u	**local_ptr = opt_lcs ? &curwin->w_p_lcs : &curwin->w_p_fcs;

    // only apply the global value to "curwin" when it does not have a
    // local value
    إذا (opt_lcs)
	errmsg = مجموعه_listchars_option(curwin, val,
		**local_ptr == NUL || !(opt_flags & OPT_GLOBAL),
							    errbuf, errbuflen);
    وإلا
	errmsg = مجموعه_fillchars_option(curwin, val,
		**local_ptr == NUL || !(opt_flags & OPT_GLOBAL),
							    errbuf, errbuflen);
    إذا (errmsg != عدم)
	أرجع errmsg;

    tabpage_T	*tp;
    win_T	*wp;

    // If the current window is set to use the global
    // 'listchars'/'fillchars' value, clear the window-local value.
    إذا (!(opt_flags & OPT_GLOBAL))
	clear_string_option(local_ptr);
    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	// If the current window has a local value need to apply it
	// again, it was changed when setting the global value.
	// If no error was returned above, we don't expect an error
	// here, so ignore the return value.
	إذا (opt_lcs)
	{
	    إذا (*wp->w_p_lcs == NUL)
		(فراغ)مجموعه_listchars_option(wp, wp->w_p_lcs, TRUE, عدم, 0);
	}
	وإلا
	{
	    إذا (*wp->w_p_fcs == NUL)
		(فراغ)مجموعه_fillchars_option(wp, wp->w_p_fcs, TRUE, عدم, 0);
	}
    }

    redraw_all_later(UPD_NOT_VALID);

    أرجع عدم;
}

/*
 * The 'fillchars' option or the 'listchars' option is changed.
 */
    حرف *
did_مجموعه_chars_option(optset_T *آرگومانها)
{
    char_u **varp = (char_u **)آرگومانها->os_varp;
    حرف *errmsg = عدم;

    إذا (   varp == &p_lcs		// global 'listchars'
	|| varp == &p_fcs)		// global 'fillchars'
	errmsg = did_مجموعه_global_listfillchars(*varp, varp == &p_lcs,
			  آرگومانها->os_flags, آرگومانها->os_errbuf, آرگومانها->os_errbuflen);
    وإلا إذا (varp == &curwin->w_p_lcs)	// local 'listchars'
	errmsg = مجموعه_listchars_option(curwin, *varp, TRUE,
					  آرگومانها->os_errbuf, آرگومانها->os_errbuflen);
    وإلا إذا (varp == &curwin->w_p_fcs)	// local 'fillchars'
	errmsg = مجموعه_fillchars_option(curwin, *varp, TRUE,
					  آرگومانها->os_errbuf, آرگومانها->os_errbuflen);

    أرجع errmsg;
}

/*
 * Expand 'fillchars' or 'listchars' option value.
 */
    صحيح
expand_مجموعه_chars_option(optexpand_T *آرگومانها, صحيح *رقم_Matches, char_u ***matches)
{
    char_u **varp = (char_u **)آرگومانها->oe_varp;
    صحيح is_lcs = (varp == &p_lcs || varp == &curwin->w_p_lcs);
    أرجع expand_مجموعه_opt_generic(
	    آرگومانها,
	    is_lcs ? get_listchars_name : get_fillchars_name,
	    رقم_Matches,
	    matches);
}

/*
 * The 'cinoptions' option is changed.
 */
    حرف *
did_مجموعه_cinoptions(optset_T *آرگومانها استفادهنشده)
{
    // TODO: recognize errors
    parse_cino(curbuf);

    أرجع عدم;
}

#إذا defined(FEAT_SYN_HL)
/*
 * The 'colorcolumn' option is changed.
 */
    حرف *
did_مجموعه_colorcolumn(optset_T *آرگومانها)
{
    char_u	**varp = (char_u **)آرگومانها->os_varp;

    أرجع check_colorcolumn(*varp, varp == &curwin->w_p_cc ? curwin : عدم);
}
#پایان_اگر

/*
 * The 'comments' option is changed.
 */
    حرف *
did_مجموعه_comments(optset_T *آرگومانها)
{
    char_u	**varp = (char_u **)آرگومانها->os_varp;
    char_u	*s;
    حرف	*errmsg = عدم;

    لكل (s = *varp; *s; )
    {
	طالما (*s && *s != ':')
	{
	    إذا (vim_strchr((char_u *)COM_ALL, *s) == عدم
		    && !VIM_ISDIGIT(*s) && *s != '-')
	    {
		errmsg = illegal_char(آرگومانها->os_errbuf, آرگومانها->os_errbuflen, *s);
		اكسر;
	    }
	    ++s;
	}
	إذا (*s++ == NUL)
	    errmsg = e_missing_colon;
	وإلا إذا (*s == ',' || *s == NUL)
	    errmsg = e_zero_درازی_string;
	إذا (errmsg != عدم)
	    اكسر;
	طالما (*s && *s != ',')
	{
	    إذا (*s == '\\' && s[1] != NUL)
		++s;
	    ++s;
	}
	s = skip_مقصد_option_part(s);
    }

    أرجع errmsg;
}

/*
 * The 'commentstring' option is changed.
 */
    حرف *
did_مجموعه_commentstring(optset_T *آرگومانها)
{
    char_u	**varp = (char_u **)آرگومانها->os_varp;

    إذا (**varp != NUL && جستجوی_رشته((حرف *)*varp, "%s") == عدم)
	أرجع e_commentstring_must_be_خالی_or_contain_str;

    أرجع عدم;
}

/*
 * Check if value for 'complete' is valid when 'complete' option is changed.
 */
    حرف *
did_مجموعه_complete(optset_T *آرگومانها)
{
    char_u	**varp = (char_u **)آرگومانها->os_varp;
    char_u	*p, *t;
    char_u	میانگیر[LSIZE];
    char_u	*buf_ptr;
    char_u	char_before = NUL;
    صحيح		escape;

    لكل (p = *varp; *p; )
    {
	vim_memset(میانگیر, 0, LSIZE);
	buf_ptr = میانگیر;
	escape = 0;

	// Extract substring while handling escaped commas
	طالما (*p && (*p != ',' || escape) && buf_ptr < (میانگیر + LSIZE - 1))
	{
	    إذا (*p == '\\' && *(p + 1) == ',')
	    {
		escape = 1;  // Mark escape mode
		p++;         // Skip '\'
	    }
	    وإلا
	    {
		escape = 0;
		*buf_ptr++ = *p;
	    }
	    p++;
	}
	*buf_ptr = NUL;

	إذا (vim_strchr((char_u *)".wbuksid]tUFo", *میانگیر) == عدم)
	    أرجع illegal_char(آرگومانها->os_errbuf, آرگومانها->os_errbuflen, *میانگیر);

	إذا (vim_strchr((char_u *)"ksF", *میانگیر) == عدم && *(میانگیر + 1) != NUL
		&& *(میانگیر + 1) != '^')
	    char_before = *میانگیر;
	وإلا
	{
	    // Test for a number after '^'
	    إذا ((t = vim_strchr(میانگیر, '^')) != عدم)
	    {
		*t++ = NUL;
		إذا (!*t)
		    char_before = '^';
		وإلا
		{
		    لكل (; *t; t++)
		    {
			إذا (!vim_isdigit(*t))
			{
			    char_before = '^';
			    اكسر;
			}
		    }
		}
	    }
	}
	إذا (char_before != NUL)
	    أرجع illegal_char_after_chr(آرگومانها->os_errbuf, آرگومانها->os_errbuflen,
		    char_before);
	// Skip comma and spaces
	طالما (*p == ',' || *p == ' ')
	    p++;
    }

#اگر_تعریف_شده FEAT_COMPL_FUNC
    إذا (مجموعه_cpt_callbacks(آرگومانها) != OK)
	أرجع illegal_char_after_chr(آرگومانها->os_errbuf, آرگومانها->os_errbuflen,
		'F');
#پایان_اگر
    أرجع عدم;
}

    صحيح
expand_مجموعه_complete(optexpand_T *آرگومانها, صحيح *رقم_Matches, char_u ***matches)
{
    ساكن حرف *(p_cpt_مقادیر[]) = {
	".", "w", "b", "u", "k", "kspell", "s", "i", "d", "]", "t", "U", "F", "o",
	عدم};
    أرجع expand_مجموعه_opt_string(
	    آرگومانها,
	    p_cpt_مقادیر,
	    ARRAY_درازی(p_cpt_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}

/*
 * The 'completeopt' option is changed.
 */
    حرف *
did_مجموعه_completeopt(optset_T *آرگومانها استفادهنشده)
{
    char_u	*cot = p_cot;
    بیعلامت	*flags = &cot_flags;

    إذا (آرگومانها->os_flags & OPT_LOCAL)
    {
	cot = curbuf->b_p_cot;
	flags = &curbuf->b_cot_flags;
    }
    وإلا إذا (!(آرگومانها->os_flags & OPT_GLOBAL))
	// When using :set, clear the local flags.
	curbuf->b_cot_flags = 0;

    إذا (opt_رشتهها_flags(cot, p_cot_مقادیر, flags, TRUE) != OK)
	أرجع e_invalid_آرگومان;

    أرجع عدم;
}

    صحيح
expand_مجموعه_completeopt(optexpand_T *آرگومانها, صحيح *رقم_Matches, char_u ***matches)
{
    أرجع expand_مجموعه_opt_string(
	    آرگومانها,
	    p_cot_مقادیر,
	    ARRAY_درازی(p_cot_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}

/*
 * The 'completefuzzycollect' option is changed.
 */
    حرف *
did_مجموعه_completefuzzycollect(optset_T *آرگومانها استفادهنشده)
{
    إذا (opt_رشتهها_flags(p_cfc, p_cfc_مقادیر, &cfc_flags, TRUE) != OK)
	أرجع e_invalid_آرگومان;
    أرجع عدم;
}

    صحيح
expand_مجموعه_completefuzzycollect(
	optexpand_T *آرگومانها,
	صحيح *رقم_Matches,
	char_u ***matches)
{
    أرجع expand_مجموعه_opt_string(
	    آرگومانها,
	    p_cfc_مقادیر,
	    ARRAY_درازی(p_cfc_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}

/*
 * The 'completeitemalign' option is changed.
 */
    حرف *
did_مجموعه_completeitemalign(optset_T *آرگومانها استفادهنشده)
{
    char_u	*p = p_cia;
    بیعلامت	new_cia_flags = 0;
    صحيح		دیده[3] = { FALSE, FALSE, FALSE };
    صحيح		شمار = 0;
    char_u	buf[10];

    طالما (*p)
    {
	copy_option_part(&p, buf, حجم(buf), ",");
	إذا (شمار >= 3)
	    أرجع e_invalid_آرگومان;

	إذا (STRCMP(buf, "abbr") == 0)
	{
	    إذا (دیده[CPT_ABBR])
		أرجع e_invalid_آرگومان;
	    new_cia_flags = new_cia_flags * 10 + CPT_ABBR;
	    دیده[CPT_ABBR] = TRUE;
	    شمار++;
	}
	وإلا إذا (STRCMP(buf, "kind") == 0)
	{
	    إذا (دیده[CPT_نوع])
		أرجع e_invalid_آرگومان;
	    new_cia_flags = new_cia_flags * 10 + CPT_نوع;
	    دیده[CPT_نوع] = TRUE;
	    شمار++;
	}
	وإلا إذا (STRCMP(buf, "menu") == 0)
	{
	    إذا (دیده[CPT_MENU])
		أرجع e_invalid_آرگومان;
	    new_cia_flags = new_cia_flags * 10 + CPT_MENU;
	    دیده[CPT_MENU] = TRUE;
	    شمار++;
	}
	وإلا
	    أرجع e_invalid_آرگومان;
    }
    إذا (new_cia_flags == 0 || شمار != 3)
	أرجع e_invalid_آرگومان;

    cia_flags = new_cia_flags;
    أرجع عدم;
}

#إذا defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
/*
 * The 'completepopup' option is changed.
 */
    حرف *
did_مجموعه_completepopup(optset_T *آرگومانها استفادهنشده)
{
    إذا (parse_completepopup(عدم) == FAIL)
	أرجع e_invalid_آرگومان;

    popup_close_info();
    أرجع عدم;
}
#پایان_اگر

#إذا defined(BACKSLASH_درون_FILENAME)
/*
 * The 'completeslash' option is changed.
 */
    حرف *
did_مجموعه_completeslash(optset_T *آرگومانها استفادهنشده)
{
    إذا (check_opt_رشتهها(p_csl, p_csl_مقادیر, FALSE) != OK
	    || check_opt_رشتهها(curbuf->b_p_csl, p_csl_مقادیر, FALSE) != OK)
	أرجع e_invalid_آرگومان;

    أرجع عدم;
}

    صحيح
expand_مجموعه_completeslash(optexpand_T *آرگومانها, صحيح *رقم_Matches, char_u ***matches)
{
    أرجع expand_مجموعه_opt_string(
	    آرگومانها,
	    p_csl_مقادیر,
	    ARRAY_درازی(p_csl_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}
#پایان_اگر

#إذا defined(FEAT_CONCEAL)
/*
 * The 'concealcursor' option is changed.
 */
    حرف *
did_مجموعه_concealcursor(optset_T *آرگومانها)
{
    char_u	**varp = (char_u **)آرگومانها->os_varp;

    أرجع did_مجموعه_option_listflag(*varp, (char_u *)COCU_ALL, آرگومانها->os_errbuf,
		    آرگومانها->os_errbuflen);
}

    صحيح
expand_مجموعه_concealcursor(optexpand_T *آرگومانها, صحيح *رقم_Matches, char_u ***matches)
{
    أرجع expand_مجموعه_opt_listflag(آرگومانها, (char_u*)COCU_ALL, رقم_Matches, matches);
}
#پایان_اگر

/*
 * The 'cpoptions' option is changed.
 */
    حرف *
did_مجموعه_cpoptions(optset_T *آرگومانها)
{
    char_u	**varp = (char_u **)آرگومانها->os_varp;

    أرجع did_مجموعه_option_listflag(*varp, (char_u *)CPO_ALL, آرگومانها->os_errbuf,
		    آرگومانها->os_errbuflen);
}

    صحيح
expand_مجموعه_cpoptions(optexpand_T *آرگومانها, صحيح *رقم_Matches, char_u ***matches)
{
    أرجع expand_مجموعه_opt_listflag(آرگومانها, (char_u*)CPO_ALL, رقم_Matches, matches);
}

#إذا defined(FEAT_CRYPT)
/*
 * The 'cryptkey' option is changed.
 */
    حرف *
did_مجموعه_cryptkey(optset_T *آرگومانها)
{
    // Make sure the ":set" command doesn't show the new value in the
    // history.
    remove_کلید_from_history();

    إذا (آرگومانها->os_op != OP_NONE)
	// Don't allow set+=/-=/^= as they can allow for substring guessing
	أرجع e_invalid_آرگومان;

    إذا (STRCMP(curbuf->b_p_کلید, آرگومانها->os_oldval.string) != 0)
    {
	// Need to update the swapfile.
	ml_مجموعه_crypt_کلید(curbuf, آرگومانها->os_oldval.string,
		*curbuf->b_p_cm == NUL ? p_cm : curbuf->b_p_cm);
	تغییریافته_داخلی();
    }
# ifdef FEAT_SODIUM
    إذا (crypt_روش_is_sodium(crypt_get_روش_nr(curbuf)))
	crypt_sodium_قفل_کلید(آرگومانها->os_newval.string);
# endif

    أرجع عدم;
}

/*
 * The 'cryptmethod' option is changed.
 */
    حرف *
did_مجموعه_cryptmethod(optset_T *آرگومانها)
{
    char_u  *p;
    char_u  *s;

    إذا (آرگومانها->os_flags & OPT_LOCAL)
	p = curbuf->b_p_cm;
    وإلا
	p = p_cm;
    إذا (check_opt_رشتهها(p, p_cm_مقادیر, TRUE) != OK)
	أرجع e_invalid_آرگومان;
    وإلا إذا (crypt_self_test() == FAIL)
	أرجع e_invalid_آرگومان;

    // When setting the global value to empty, make it "zip".
    إذا (*p_cm == NUL)
    {
	free_string_option(p_cm);
	p_cm = vim_strsave((char_u *)"zip");
    }
    // When using ":set cm=name" the local value is going to be empty.
    // Do that here, otherwise the crypt functions will still use the
    // local value.
    إذا ((آرگومانها->os_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	free_string_option(curbuf->b_p_cm);
	curbuf->b_p_cm = خالی_option;
    }

    // Need to update the swapfile when the effective method changed.
    // Set "s" to the effective old value, "p" to the effective new
    // method and compare.
    إذا ((آرگومانها->os_flags & OPT_LOCAL) && *آرگومانها->os_oldval.string == NUL)
	s = p_cm;  // was previously using the global value
    وإلا
	s = آرگومانها->os_oldval.string;
    إذا (*curbuf->b_p_cm == NUL)
	p = p_cm;  // is now using the global value
    وإلا
	p = curbuf->b_p_cm;
    إذا (STRCMP(s, p) != 0)
	ml_مجموعه_crypt_کلید(curbuf, curbuf->b_p_کلید, s);

    // If the global value changes need to update the swapfile for all
    // buffers using that value.
    إذا ((آرگومانها->os_flags & OPT_GLOBAL)
	    && STRCMP(p_cm, آرگومانها->os_oldval.string) != 0)
    {
	buf_T	*buf;

	FOR_ALL_BUFFERS(buf)
	    إذا (buf != curbuf && *buf->b_p_cm == NUL)
		ml_مجموعه_crypt_کلید(buf, buf->b_p_کلید, آرگومانها->os_oldval.string);
    }
    أرجع عدم;
}

    صحيح
expand_مجموعه_cryptmethod(optexpand_T *آرگومانها, صحيح *رقم_Matches, char_u ***matches)
{
    أرجع expand_مجموعه_opt_string(
	    آرگومانها,
	    p_cm_مقادیر,
	    ARRAY_درازی(p_cm_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}
#پایان_اگر

#إذا defined(FEAT_CSCOPE) && defined(FEAT_QUICKFIX)
/*
 * The 'cscopequickfix' option is changed.
 */
    حرف *
did_مجموعه_cscopequickfix(optset_T *آرگومانها استفادهنشده)
{
    char_u	*p;

    إذا (p_csqf == عدم)
	أرجع عدم;

    p = p_csqf;
    طالما (*p != NUL)
    {
	إذا (vim_strchr((char_u *)CSQF_CMDS, *p) == عدم
		|| p[1] == NUL
		|| vim_strchr((char_u *)CSQF_FLAGS, p[1]) == عدم
		|| (p[2] != NUL && p[2] != ','))
	    أرجع e_invalid_آرگومان;
	وإلا إذا (p[2] == NUL)
	    اكسر;
	وإلا
	    p += 3;
    }

    أرجع عدم;
}
#پایان_اگر

#إذا defined(FEAT_SYN_HL)
/*
 * The 'cursorlineopt' option is changed.
 */
    حرف *
did_مجموعه_cursorlineopt(optset_T *آرگومانها)
{
    char_u	**varp = (char_u **)آرگومانها->os_varp;

    // This could be changed to use opt_strings_flags() instead.
    إذا (**varp == NUL || fill_culopt_flags(*varp, curwin) != OK)
	أرجع e_invalid_آرگومان;

    أرجع عدم;
}

    صحيح
expand_مجموعه_cursorlineopt(optexpand_T *آرگومانها, صحيح *رقم_Matches, char_u ***matches)
{
    أرجع expand_مجموعه_opt_string(
	    آرگومانها,
	    p_culopt_مقادیر,
	    ARRAY_درازی(p_culopt_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}
#پایان_اگر

/*
 * The 'debug' option is changed.
 */
    حرف *
did_مجموعه_debug(optset_T *آرگومانها استفادهنشده)
{
    أرجع did_مجموعه_opt_رشتهها(p_debug, p_debug_مقادیر, TRUE);
}

    صحيح
expand_مجموعه_debug(optexpand_T *آرگومانها, صحيح *رقم_Matches, char_u ***matches)
{
    أرجع expand_مجموعه_opt_string(
	    آرگومانها,
	    p_debug_مقادیر,
	    ARRAY_درازی(p_debug_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}

#إذا defined(FEAT_DIFF)
/*
 * The 'diffanchors' option is changed.
 */
    حرف *
did_مجموعه_diffanchors(optset_T *آرگومانها)
{
    إذا (diffanchors_تغییریافته(آرگومانها->os_flags & OPT_LOCAL) == FAIL)
	أرجع e_invalid_آرگومان;

    أرجع عدم;
}

/*
 * The 'diffopt' option is changed.
 */
    حرف *
did_مجموعه_diffopt(optset_T *آرگومانها استفادهنشده)
{
    إذا (diffopt_تغییریافته() == FAIL)
	أرجع e_invalid_آرگومان;

    أرجع عدم;
}

    صحيح
expand_مجموعه_diffopt(optexpand_T *آرگومانها, صحيح *رقم_Matches, char_u ***matches)
{
    expand_T *xp = آرگومانها->oe_xp;

    إذا (xp->xp_الگو > آرگومانها->oe_مجموعه_arg && *(xp->xp_الگو-1) == ':')
    {
	إذا (completing_مقدار_for_subopt(آرگومانها, "algorithm"))
	{
	    أرجع expand_مجموعه_opt_string(
		    آرگومانها,
		    p_dip_algorithm_مقادیر,
		    ARRAY_درازی(p_dip_algorithm_مقادیر) - 1,
		    رقم_Matches,
		    matches);
	}
	إذا (completing_مقدار_for_subopt(آرگومانها, "inline"))
	{
	    أرجع expand_مجموعه_opt_string(
		    آرگومانها,
		    p_dip_inline_مقادیر,
		    ARRAY_درازی(p_dip_inline_مقادیر) - 1,
		    رقم_Matches,
		    matches);
	}
	أرجع FAIL;
    }

    أرجع expand_مجموعه_opt_string(
	    آرگومانها,
	    p_dip_مقادیر,
	    ARRAY_درازی(p_dip_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}
#پایان_اگر

/*
 * The 'display' option is changed.
 */
    حرف *
did_مجموعه_display(optset_T *آرگومانها استفادهنشده)
{
    إذا (opt_رشتهها_flags(p_dy, p_dy_مقادیر, &dy_flags, TRUE) != OK)
	أرجع e_invalid_آرگومان;

    (فراغ)init_chartab();
    أرجع عدم;
}

    صحيح
expand_مجموعه_display(optexpand_T *آرگومانها, صحيح *رقم_Matches, char_u ***matches)
{
    أرجع expand_مجموعه_opt_string(
	    آرگومانها,
	    p_dy_مقادیر,
	    ARRAY_درازی(p_dy_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}

/*
 * The 'eadirection' option is changed.
 */
    حرف *
did_مجموعه_eadirection(optset_T *آرگومانها استفادهنشده)
{
    أرجع did_مجموعه_opt_رشتهها(p_ead, p_ead_مقادیر, FALSE);
}

    صحيح
expand_مجموعه_eadirection(optexpand_T *آرگومانها, صحيح *رقم_Matches, char_u ***matches)
{
    أرجع expand_مجموعه_opt_string(
	    آرگومانها,
	    p_ead_مقادیر,
	    ARRAY_درازی(p_ead_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}

/*
 * One of the 'encoding', 'fileencoding', 'termencoding' or 'makeencoding'
 * options is changed.
 */
    حرف *
did_مجموعه_encoding(optset_T *آرگومانها)
{
    char_u	**varp = (char_u **)آرگومانها->os_varp;
    char_u	**gvarp;
    حرف	*errmsg = عدم;
    char_u	*p;

    // Get the global option to compare with, otherwise we would have to check
    // two values for all local options.
    gvarp = (char_u **)get_option_varp_scope(آرگومانها->os_idx, OPT_GLOBAL);

    إذا (gvarp == &p_fenc)
    {
	إذا (!curbuf->b_p_ma && آرگومانها->os_flags != OPT_GLOBAL)
	    errmsg = e_cannot_make_changes_modifiable_is_off;
	وإلا إذا (vim_strchr(*varp, ',') != عدم)
	    // No comma allowed in 'fileencoding'; catches confusing it
	    // with 'fileencodings'.
	    errmsg = e_invalid_آرگومان;
	وإلا
	{
	    // May show a "+" in the title now.
	    redraw_titles();
	    // Add 'fileencoding' to the swap file.
	    ml_setflags(curbuf);
	}
    }
    إذا (errmsg == عدم)
    {
	// canonize the value, so that STRCMP() can be used on it
	p = enc_canonize(*varp);
	إذا (p != عدم)
	{
	    vim_free(*varp);
	    *varp = p;
	}
	إذا (varp == &p_enc)
	{
	    errmsg = mb_init();
	    redraw_titles();
	}
    }

#إذا defined(FEAT_GUI_GTK)
    إذا (errmsg == عدم && varp == &p_tenc && gui.درون_use)
    {
	// GTK uses only a single encoding, and that is UTF-8.
	إذا (STRCMP(p_tenc, "utf-8") != 0)
	    errmsg = e_cannot_be_تغییریافته_درون_gtk_GUI;
    }
#پایان_اگر

    إذا (errmsg == عدم)
    {
#اگر_تعریف_شده FEAT_KEYMAP
	// When 'keymap' is used and 'encoding' changes, reload the keymap
	// (with another encoding).
	إذا (varp == &p_enc && *curbuf->b_p_keymap != NUL)
	    (فراغ)keymap_init();
#پایان_اگر

	// When 'termencoding' is not empty and 'encoding' changes or when
	// 'termencoding' changes, need to setup for keyboard input and
	// display output conversion.
	إذا (((varp == &p_enc && *p_tenc != NUL) || varp == &p_tenc))
	{
	    إذا (convert_راهاندازی(&درون_داد_conv, p_tenc, p_enc) == FAIL
		    || convert_راهاندازی(&output_conv, p_enc, p_tenc) == FAIL)
	    {
		semsg(_(e_cannot_convert_between_str_and_str),
			p_tenc, p_enc);
		errmsg = e_invalid_آرگومان;
	    }
	}

#إذا defined(MSWIN)
	// $HOME, $VIM and $VIMRUNTIME may have characters in active code page.
	إذا (varp == &p_enc)
	{
	    init_homedir();
	    init_vimdir();
	}
#پایان_اگر
    }

    أرجع errmsg;
}

    صحيح
expand_مجموعه_encoding(optexpand_T *آرگومانها, صحيح *رقم_Matches, char_u ***matches)
{
    أرجع expand_مجموعه_opt_generic(
	    آرگومانها,
	    get_encoding_name,
	    رقم_Matches,
	    matches);
}

/*
 * The 'eventignore(win)' option is changed.
 */
    حرف *
did_مجموعه_eventignore(optset_T *آرگومانها)
{
    char_u	**varp = (char_u **)آرگومانها->os_varp;
    char_u	*oldval = آرگومانها->os_oldval.string;

    إذا (check_ei(*varp) == FAIL)
	أرجع e_invalid_آرگومان;

    إذا (oldval == عدم || STRCMP(oldval, *varp) == 0)
	أرجع عدم;

    // Deal with the events that are triggered by comparing against a stored
    // state, with the old value in effect: what happened while an event was
    // ignored must not be reported once it is not ignored anymore, and what
    // happened before must still be reported.
    // Use a copy, the caller owns "oldval" and autocommands may free it.
    char_u	*save_ei = vim_strsave(oldval);
    إذا (save_ei != عدم)
    {
	char_u	*newval = *varp;
	win_T	*wp = is_پنجره_local_option(آرگومانها->os_idx) ? curwin : عدم;

	*varp = save_ei;
	// "varp" points into "wp" for 'eventignorewin'.
	إذا (wp != عدم)
	    ++wp->w_locked;
	may_trigger_deferred_events();
	إذا (wp != عدم)
	    --wp->w_locked;

	free_string_option(*varp);
	*varp = newval;
    }

    أرجع عدم;
}

ساكن صحيح expand_eiw = FALSE;

    ساكن char_u *
get_eventignore_name(expand_T *xp, صحيح idx)
{
    صحيح تفریق = *xp->xp_الگو == '-';
    // 'eventignore(win)' allows special keyword "all" in addition to
    // all event names.
    إذا (!تفریق && idx == 0)
	أرجع (char_u *)"all";

    char_u *name = get_event_name_no_گروه(xp, idx - 1 + تفریق, expand_eiw);
    إذا (name == عدم)
	أرجع عدم;

    sprintf((حرف *)IObuff, "%s%s", تفریق ? "-" : "", name);
    أرجع IObuff;
}

    صحيح
expand_مجموعه_eventignore(optexpand_T *آرگومانها, صحيح *رقم_Matches, char_u ***matches)
{
    expand_eiw = آرگومانها->oe_varp != (char_u *)&p_ei;
    أرجع expand_مجموعه_opt_generic(
	    آرگومانها,
	    get_eventignore_name,
	    رقم_Matches,
	    matches);
}

/*
 * The 'fileformat' option is changed.
 */
    حرف *
did_مجموعه_fileformat(optset_T *آرگومانها)
{
    char_u	**varp = (char_u **)آرگومانها->os_varp;

    إذا (!curbuf->b_p_ma && !(آرگومانها->os_flags & OPT_GLOBAL))
	أرجع e_cannot_make_changes_modifiable_is_off;
    وإلا إذا (check_opt_رشتهها(*varp, p_ff_مقادیر, FALSE) != OK)
	أرجع e_invalid_آرگومان;

    // may also change 'textmode'
    إذا (get_fileformat(curbuf) == EOL_DOS)
	curbuf->b_p_tx = TRUE;
    وإلا
	curbuf->b_p_tx = FALSE;
    redraw_titles();
    // update flag in swap file
    ml_setflags(curbuf);
    // Redraw needed when switching to/from "mac": a CR in the text
    // will be displayed differently.
    إذا (get_fileformat(curbuf) == EOL_MAC || *آرگومانها->os_oldval.string == 'm')
	redraw_curbuf_later(UPD_NOT_VALID);

    أرجع عدم;
}

    صحيح
expand_مجموعه_fileformat(optexpand_T *آرگومانها, صحيح *رقم_Matches, char_u ***matches)
{
    أرجع expand_مجموعه_opt_string(
	    آرگومانها,
	    p_ff_مقادیر,
	    ARRAY_درازی(p_ff_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * fileformat options.
 */
    char_u *
get_fileformat_name(expand_T *xp استفادهنشده, صحيح idx)
{
    إذا (idx >= (صحيح)ARRAY_درازی(p_ff_مقادیر))
	أرجع عدم;

    أرجع (char_u*)p_ff_مقادیر[idx];
}

/*
 * The 'fileformats' option is changed.
 */
    حرف *
did_مجموعه_fileformats(optset_T *آرگومانها استفادهنشده)
{
    إذا (check_opt_رشتهها(p_ffs, p_ff_مقادیر, TRUE) != OK)
	أرجع e_invalid_آرگومان;

    // also change 'textauto'
    إذا (*p_ffs == NUL)
	p_ta = FALSE;
    وإلا
	p_ta = TRUE;

    أرجع عدم;
}

/*
 * The 'filetype' or the 'syntax' option is changed.
 */
    حرف *
did_مجموعه_filetype_or_syntax(optset_T *آرگومانها)
{
    char_u	**varp = (char_u **)آرگومانها->os_varp;

    إذا (!valid_filetype(*varp))
	أرجع e_invalid_آرگومان;

    آرگومانها->os_مقدار_تغییریافته = STRCMP(آرگومانها->os_oldval.string, *varp) != 0;

    // Since we check the value, there is no need to set P_INSECURE,
    // even when the value comes from a modeline.
    آرگومانها->os_مقدار_checked = TRUE;

    أرجع عدم;
}

#إذا defined(FEAT_FOLDING)
/*
 * The 'foldclose' option is changed.
 */
    حرف *
did_مجموعه_foldclose(optset_T *آرگومانها استفادهنشده)
{
    أرجع did_مجموعه_opt_رشتهها(p_fcl, p_fcl_مقادیر, TRUE);
}

    صحيح
expand_مجموعه_foldclose(optexpand_T *آرگومانها, صحيح *رقم_Matches, char_u ***matches)
{
    أرجع expand_مجموعه_opt_string(
	    آرگومانها,
	    p_fcl_مقادیر,
	    ARRAY_درازی(p_fcl_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}
#پایان_اگر

#إذا defined(FEAT_EVAL) && defined(FEAT_FOLDING)
/*
 * The 'foldexpr' option is changed.
 */
    حرف *
did_مجموعه_foldexpr(optset_T *آرگومانها)
{
    (فراغ)did_مجموعه_optexpr(آرگومانها);
    إذا (foldmethod_Is_عبارت(curwin))
	foldUpdateAll(curwin);
    أرجع عدم;
}
#پایان_اگر

#إذا defined(FEAT_FOLDING)
/*
 * The 'foldignore' option is changed.
 */
    حرف *
did_مجموعه_foldignore(optset_T *آرگومانها استفادهنشده)
{
    إذا (foldmethodIsIndent(curwin))
	foldUpdateAll(curwin);
    أرجع عدم;
}

/*
 * The 'foldmarker' option is changed.
 */
    حرف *
did_مجموعه_foldmarker(optset_T *آرگومانها)
{
    char_u	**varp = (char_u **)آرگومانها->os_varp;
    char_u	*p;

    p = vim_strchr(*varp, ',');
    إذا (p == عدم)
	أرجع e_comma_required;
    وإلا إذا (p == *varp || p[1] == NUL)
	أرجع e_invalid_آرگومان;
    وإلا إذا (foldmethodIsMarker(curwin))
	foldUpdateAll(curwin);

    أرجع عدم;
}

/*
 * The 'foldmethod' option is changed.
 */
    حرف *
did_مجموعه_foldmethod(optset_T *آرگومانها)
{
    char_u	**varp = (char_u **)آرگومانها->os_varp;

    إذا (check_opt_رشتهها(*varp, p_fdm_مقادیر, FALSE) != OK || **varp == NUL)
	أرجع e_invalid_آرگومان;

    foldUpdateAll(curwin);
    إذا (foldmethodIsDiff(curwin))
	newFoldLevel();
    أرجع عدم;
}

    صحيح
expand_مجموعه_foldmethod(optexpand_T *آرگومانها, صحيح *رقم_Matches, char_u ***matches)
{
    أرجع expand_مجموعه_opt_string(
	    آرگومانها,
	    p_fdm_مقادیر,
	    ARRAY_درازی(p_fdm_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}

/*
 * The 'foldopen' option is changed.
 */
    حرف *
did_مجموعه_foldopen(optset_T *آرگومانها استفادهنشده)
{
    أرجع did_مجموعه_opt_flags(p_fdo, p_fdo_مقادیر, &fdo_flags, TRUE);
}

    صحيح
expand_مجموعه_foldopen(optexpand_T *آرگومانها, صحيح *رقم_Matches, char_u ***matches)
{
    أرجع expand_مجموعه_opt_string(
	    آرگومانها,
	    p_fdo_مقادیر,
	    ARRAY_درازی(p_fdo_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}
#پایان_اگر

/*
 * The 'formatoptions' option is changed.
 */
    حرف *
did_مجموعه_formatoptions(optset_T *آرگومانها)
{
    char_u	**varp = (char_u **)آرگومانها->os_varp;

    أرجع did_مجموعه_option_listflag(*varp, (char_u *)FO_ALL, آرگومانها->os_errbuf,
		    آرگومانها->os_errbuflen);
}

    صحيح
expand_مجموعه_formatoptions(optexpand_T *آرگومانها, صحيح *رقم_Matches, char_u ***matches)
{
    أرجع expand_مجموعه_opt_listflag(آرگومانها, (char_u*)FO_ALL, رقم_Matches, matches);
}

#إذا defined(CURSOR_SHAPE)
/*
 * The 'guicursor' option is changed.
 */
    حرف *
did_مجموعه_guicursor(optset_T *آرگومانها استفادهنشده)
{
    أرجع parse_shape_opt(SHAPE_CURSOR);
}
#پایان_اگر

#إذا defined(FEAT_GUI)
/*
 * The 'guifont' option is changed.
 */
    حرف *
did_مجموعه_guifont(optset_T *آرگومانها استفادهنشده)
{
    char_u	*p;
    حرف	*errmsg = عدم;

    إذا (gui.درون_use)
    {
	p = p_guifont;
# إذا defined(FEAT_GUI_GTK)
	// Put up a font dialog and let the user select a new value.
	// If this is cancelled go back to the old value but don't
	// give an error message.
	إذا (STRCMP(p, "*") == 0)
	{
	    p = gui_mch_font_dialog(آرگومانها->os_oldval.string);
	    free_string_option(p_guifont);
	    p_guifont = (p != عدم) ? p : vim_strsave(آرگومانها->os_oldval.string);
	}
# endif
	إذا (p != عدم && gui_init_font(p_guifont, FALSE) != OK)
	{
# إذا defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_PHOTON)
	    إذا (STRCMP(p_guifont, "*") == 0)
	    {
		// Dialog was cancelled: Keep the old value without giving
		// an error message.
		free_string_option(p_guifont);
		p_guifont = vim_strsave(آرگومانها->os_oldval.string);
	    }
	    وإلا
# endif
		errmsg = e_invalid_fonts;
	}
    }

    أرجع errmsg;
}

/*
 * Expand the 'guifont' option. Only when GUI is being used. Each platform has
 * specific behaviors.
 */
    صحيح
expand_مجموعه_guifont(optexpand_T *آرگومانها, صحيح *رقم_Matches, char_u ***matches)
{
    إذا (!gui.درون_use)
	أرجع FAIL;

# إذا defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
    char_u **varp = (char_u **)آرگومانها->oe_varp;
    صحيح wide = (varp == &p_guifontwide);

    أرجع expand_مجموعه_opt_callback(
	    آرگومانها, gui_mch_expand_font, &wide, رقم_Matches, matches);
# وإلا
    أرجع FAIL;
# endif
}

# إذا defined(FEAT_XFONTSET)
/*
 * The 'guifontset' option is changed.
 */
    حرف *
did_مجموعه_guifontset(optset_T *آرگومانها استفادهنشده)
{
    حرف *errmsg = عدم;

    إذا (STRCMP(p_guifontset, "*") == 0)
	errmsg = e_cant_select_fontset;
    وإلا إذا (gui.درون_use && gui_init_font(p_guifontset, TRUE) != OK)
	errmsg = e_invalid_fontset;

    أرجع errmsg;
}
# endif

/*
 * The 'guifontwide' option is changed.
 */
    حرف *
did_مجموعه_guifontwide(optset_T *آرگومانها استفادهنشده)
{
    حرف *errmsg = عدم;

    إذا (STRCMP(p_guifontwide, "*") == 0)
	errmsg = e_cant_select_wide_font;
    وإلا إذا (gui_get_wide_font() == FAIL)
	errmsg = e_invalid_wide_font;

    أرجع errmsg;
}
#پایان_اگر

#إذا defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
/*
 * The 'guiligatures' option is changed.
 */
    حرف *
did_مجموعه_guiligatures(optset_T *آرگومانها استفادهنشده)
{
    gui_مجموعه_ligatures();
    أرجع عدم;
}
#پایان_اگر

#إذا defined(FEAT_GUI)
/*
 * The 'guioptions' option is changed.
 */
    حرف *
did_مجموعه_guioptions(optset_T *آرگومانها)
{
    char_u	**varp = (char_u **)آرگومانها->os_varp;
    حرف *errmsg;

    errmsg = did_مجموعه_option_listflag(*varp, (char_u *)GO_ALL, آرگومانها->os_errbuf,
		    آرگومانها->os_errbuflen);
    إذا (errmsg != عدم)
	أرجع errmsg;

    gui_init_which_components(آرگومانها->os_oldval.string);
    أرجع عدم;
}

    صحيح
expand_مجموعه_guioptions(optexpand_T *آرگومانها, صحيح *رقم_Matches, char_u ***matches)
{
    أرجع expand_مجموعه_opt_listflag(آرگومانها, (char_u*)GO_ALL, رقم_Matches, matches);
}
#پایان_اگر

#إذا defined(FEAT_GUI_TABLINE)
/*
 * The 'guitablabel' option is changed.
 */
    حرف *
did_مجموعه_guitablabel(optset_T *آرگومانها استفادهنشده)
{
    redraw_tabline = TRUE;
    أرجع عدم;
}
#پایان_اگر

/*
 * The 'helpfile' option is changed.
 */
    حرف *
did_مجموعه_helpfile(optset_T *آرگومانها استفادهنشده)
{
    // May compute new values for $VIM and $VIMRUNTIME
    إذا (didset_vim)
	vim_unsetenv_ext((char_u *)"VIM");
    إذا (didset_vimruntime)
	vim_unsetenv_ext((char_u *)"VIMRUNTIME");
    أرجع عدم;
}

#إذا defined(FEAT_MULTI_LANG)
/*
 * The 'helplang' option is changed.
 */
    حرف *
did_مجموعه_helplang(optset_T *آرگومانها استفادهنشده)
{
    حرف *errmsg = عدم;

    // Check for "", "ab", "ab,cd", etc.
    لكل (char_u *s = p_hlg; *s != NUL; s += 3)
    {
	إذا (s[1] == NUL || ((s[2] != ',' || s[3] == NUL) && s[2] != NUL))
	{
	    errmsg = e_invalid_آرگومان;
	    اكسر;
	}
	إذا (s[2] == NUL)
	    اكسر;
    }

    أرجع errmsg;
}
#پایان_اگر

/*
 * The 'highlight' option is changed.
 */
    حرف *
did_مجموعه_highlight(optset_T *آرگومانها استفادهنشده)
{
    إذا (highlight_تغییریافته() == FAIL)
	أرجع e_invalid_آرگومان;	// invalid flags

    أرجع عدم;
}

    ساكن صحيح
expand_hl_occasions(
	optexpand_T *آرگومانها,
	صحيح *رقم_Matches,
	char_u ***matches,
	char_u پیشوند)
{
    char_u	    *p;
    ساكن char_u   hl_flags[HLF_شمار] = HL_FLAGS;
    size_t	    i;
    صحيح		    شمار = 0;

    *matches = ALLOC_MULT(char_u *, HLF_شمار + 1);
    إذا (*matches == عدم)
	أرجع FAIL;

    // We still want to return the full option if it's requested.
    إذا (آرگومانها->oe_include_orig_val)
    {
	p = vim_strsave(آرگومانها->oe_opt_مقدار);
	إذا (p == عدم)
	{
	    VIM_CLEAR(*matches);
	    أرجع FAIL;
	}
	(*matches)[شمار++] = p;
    }

    لكل (i = 0; i < HLF_شمار; i++)
    {
	p = alloc((پیشوند == NUL ? 1 : 2) + 1);
	إذا (p == عدم)
	{
	    إذا (شمار == 0)
	    {
		VIM_CLEAR(*matches);
		أرجع FAIL;
	    }
	    وإلا
		اكسر;
	}
	إذا (پیشوند == NUL)
	    sprintf((حرف *)p, "%c", hl_flags[i]);
	وإلا
	    sprintf((حرف *)p, "%c%c", پیشوند, hl_flags[i]);
	(*matches)[شمار++] = p;
    }

    إذا (شمار == 0)
    {
	VIM_CLEAR(*matches);
	أرجع FAIL;
    }
    *رقم_Matches = شمار;
    أرجع OK;
}

/*
 * Expand 'highlight' option.
 */
    صحيح
expand_مجموعه_highlight(optexpand_T *آرگومانها, صحيح *رقم_Matches, char_u ***matches)
{
    char_u	    *p;
    expand_T	    *xp = آرگومانها->oe_xp;
    size_t	    i;
    صحيح		    شمار = 0;

    إذا (xp->xp_الگو > آرگومانها->oe_مجموعه_arg && *(xp->xp_الگو-1) == ':')
    {
	// Right after a ':', meaning we just return all highlight names.
	أرجع expand_مجموعه_opt_generic(
		آرگومانها,
		get_highlight_name,
		رقم_Matches,
		matches);
    }

    إذا (*xp->xp_الگو == NUL)
	// At beginning of a comma-separated list. Return the specific list of
	// supported occasions.
	أرجع expand_hl_occasions(آرگومانها, رقم_Matches, matches, NUL);

    // We are after the initial character (which indicates the occasion). We
    // already made sure we are not matching after a ':' above, so now we want
    // to match against display mode modifiers.
    // Since the xp_pattern starts from the beginning, we need to include it in
    // the returned match.

    // Note: Keep this in sync with highlight_changed()
    ساكن char_u p_hl_نما_مقادیر[] =
	{':', 'b', 'i', '-', 'n', 'r', 's', 'u', 'c', '2', 'd', '=', 't'};
    size_t رقم_hl_modes = ARRAY_درازی(p_hl_نما_مقادیر);

    *matches = ALLOC_MULT(char_u *, رقم_hl_modes);
    إذا (*matches == عدم)
	أرجع FAIL;

    صحيح الگو_len = xp->xp_الگو_len;

    لكل (i = 0; i < رقم_hl_modes; i++)
    {
	// Don't allow duplicates as these are unique flags
	char_u *dup = vim_strchr(xp->xp_الگو + 1, p_hl_نما_مقادیر[i]);
	إذا (dup != عدم && (صحيح)(dup - xp->xp_الگو) < الگو_len)
	    استمر;

	// ':' only works by itself, not with other flags.
	إذا (الگو_len > 1 && p_hl_نما_مقادیر[i] == ':')
	    استمر;

	p = vim_strnsave(xp->xp_الگو, الگو_len + 1);
	إذا (p == عدم)
	{
	    إذا (i == 0)
	    {
		VIM_CLEAR(*matches);
		أرجع FAIL;
	    }
	    وإلا
		اكسر;
	}
	p[الگو_len] = p_hl_نما_مقادیر[i];
	p[الگو_len + 1] = NUL;
	(*matches)[شمار++] = p;
    }
    إذا (شمار == 0)
    {
	VIM_CLEAR(*matches);
	أرجع FAIL;
    }
    *رقم_Matches = شمار;

    أرجع OK;
}

/*
 * The 'titlestring' or the 'iconstring' option is changed.
 */
    ساكن حرف *
parse_titleiconstring(optset_T *آرگومانها استفادهنشده, صحيح flagval استفادهنشده)
{
#اگر_تعریف_شده FEAT_STL_OPT
    char_u	**varp = (char_u **)آرگومانها->os_varp;

    // NULL => statusline syntax
    إذا (vim_strchr(*varp, '%') && check_stl_option(*varp) == عدم)
	stl_syntax |= flagval;
    وإلا
	stl_syntax &= ~flagval;
#پایان_اگر
    did_مجموعه_title();

    أرجع عدم;
}

/*
 * The 'iconstring' option is changed.
 */
    حرف *
did_مجموعه_iconstring(optset_T *آرگومانها)
{
    صحيح flagval = 0;

#اگر_تعریف_شده FEAT_STL_OPT
    flagval = STL_درون_ICON;
#پایان_اگر

    أرجع parse_titleiconstring(آرگومانها, flagval);
}

#إذا defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
/*
 * The 'imactivatekey' option is changed.
 */
    حرف *
did_مجموعه_imactivatekey(optset_T *آرگومانها استفادهنشده)
{
    إذا (!im_xim_isvalid_imactivate())
	أرجع e_invalid_آرگومان;
    أرجع عدم;
}
#پایان_اگر

#اگر_تعریف_شده FEAT_IMAGE
/*
 * The 'imageprotocol' option is changed.
 */
    حرف *
did_مجموعه_imageprotocol(optset_T *آرگومانها استفادهنشده)
{
    إذا (update_image_backend() == FAIL)
	أرجع e_invalid_آرگومان;
    أرجع عدم;
}

    صحيح
expand_مجموعه_imageprotocol(optexpand_T *آرگومانها, صحيح *رقم_Matches, char_u ***matches)
{
    expand_T *xp = آرگومانها->oe_xp;

    إذا (xp->xp_الگو > آرگومانها->oe_مجموعه_arg && *(xp->xp_الگو-1) == ':')
    {
	// 'imageprotocol' only has well-defined terms for completion for the
	// protocol part after the colon.
	أرجع expand_مجموعه_opt_string(
		آرگومانها,
		p_ipc_protocol_مقادیر,
		ARRAY_درازی(p_ipc_protocol_مقادیر) - 1,
		رقم_Matches,
		matches);
    }
    // Use expand_set_opt_string instead of returning FAIL so that we can
    // include the original value if args->oe_include_orig_val is set.
    ساكن حرف *(خالی_2[]) = {عدم};
    أرجع expand_مجموعه_opt_string(آرگومانها, خالی_2, 0, رقم_Matches, matches);
}
#پایان_اگر

/*
 * The 'iskeyword' option is changed.
 */
    حرف *
did_مجموعه_iskeyword(optset_T *آرگومانها)
{
    char_u	**varp = (char_u **)آرگومانها->os_varp;

    إذا (varp == &p_isk)		// only check for global-value
    {
	إذا (check_isopt(*varp) == FAIL)
	    أرجع e_invalid_آرگومان;
    }
    وإلا			// fallthrough for local-value
	أرجع did_مجموعه_isopt(آرگومانها);

    أرجع عدم;
}

/*
 * The 'isident' or the 'iskeyword' or the 'isprint' or the 'isfname' option is
 * changed.
 */
    حرف *
did_مجموعه_isopt(optset_T *آرگومانها)
{
    // 'isident', 'iskeyword', 'isprint' or 'isfname' option: refill g_chartab[]
    // If the new option is invalid, use old value.
    // 'lisp' option: refill g_chartab[] for '-' char.
    إذا (init_chartab() == FAIL)
    {
	آرگومانها->os_restore_chartab = TRUE;// need to restore the chartab.
	أرجع e_invalid_آرگومان;	// error in value
    }

    أرجع عدم;
}

/*
 * The 'jumpoptions' option is changed.
 */
    حرف *
did_مجموعه_jumpoptions(optset_T *آرگومانها استفادهنشده)
{
    إذا (opt_رشتهها_flags(p_jop, p_jop_مقادیر, &jop_flags, TRUE) != OK)
	أرجع e_invalid_آرگومان;

    أرجع عدم;
}

    صحيح
expand_مجموعه_jumpoptions(optexpand_T *آرگومانها, صحيح *رقم_Matches, char_u ***matches)
{
    أرجع expand_مجموعه_opt_string(
	    آرگومانها,
	    p_jop_مقادیر,
	    ARRAY_درازی(p_jop_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}

#إذا defined(FEAT_KEYMAP)
/*
 * The 'keymap' option is changed.
 */
    حرف *
did_مجموعه_keymap(optset_T *آرگومانها)
{
    char_u	**varp = (char_u **)آرگومانها->os_varp;
    حرف *errmsg = عدم;

    إذا (!valid_filetype(*varp))
	errmsg = e_invalid_آرگومان;
    وإلا
    {
	صحيح	    secure_save = secure;

	// Reset the secure flag, since the value of 'keymap' has
	// been checked to be safe.
	secure = 0;

	// load or unload key mapping tables
	errmsg = keymap_init();

	secure = secure_save;

	// Since we check the value, there is no need to set P_INSECURE,
	// even when the value comes from a modeline.
	آرگومانها->os_مقدار_checked = TRUE;
    }

    إذا (errmsg == عدم)
    {
	إذا (*curbuf->b_p_keymap != NUL)
	{
	    // Installed a new keymap, switch on using it.
	    curbuf->b_p_iminsert = B_IMODE_LMAP;
	    إذا (curbuf->b_p_imsearch != B_IMODE_USE_درج)
		curbuf->b_p_imsearch = B_IMODE_LMAP;
	}
	وإلا
	{
	    // Cleared the keymap, may reset 'iminsert' and 'imsearch'.
	    إذا (curbuf->b_p_iminsert == B_IMODE_LMAP)
		curbuf->b_p_iminsert = B_IMODE_NONE;
	    إذا (curbuf->b_p_imsearch == B_IMODE_LMAP)
		curbuf->b_p_imsearch = B_IMODE_USE_درج;
	}
	إذا ((آرگومانها->os_flags & OPT_LOCAL) == 0)
	{
	    مجموعه_iminsert_global();
	    مجموعه_imsearch_global();
	}
	وضعیت_redraw_curbuf();
    }

    أرجع errmsg;
}
#پایان_اگر

/*
 * The 'keymodel' option is changed.
 */
    حرف *
did_مجموعه_keymodel(optset_T *آرگومانها استفادهنشده)
{
    إذا (check_opt_رشتهها(p_km, p_km_مقادیر, TRUE) != OK)
	أرجع e_invalid_آرگومان;

    km_stopsel = (vim_strchr(p_km, 'o') != عدم);
    km_startsel = (vim_strchr(p_km, 'a') != عدم);
    أرجع عدم;
}

    صحيح
expand_مجموعه_keymodel(optexpand_T *آرگومانها, صحيح *رقم_Matches, char_u ***matches)
{
    أرجع expand_مجموعه_opt_string(
	    آرگومانها,
	    p_km_مقادیر,
	    ARRAY_درازی(p_km_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}

/*
 * The 'keyprotocol' option is changed.
 */
    حرف *
did_مجموعه_keyprotocol(optset_T *آرگومانها استفادهنشده)
{
    char_u *جمله = T_NAME;
    keyprot_T kpc = تطابق_keyprotocol(جمله);
    إذا (kpc == KEYPROTOCOL_FAIL)
	أرجع e_invalid_آرگومان;

    apply_keyprotocol(جمله, kpc);

    أرجع عدم;
}

    صحيح
expand_مجموعه_keyprotocol(optexpand_T *آرگومانها, صحيح *رقم_Matches, char_u ***matches)
{
    expand_T *xp = آرگومانها->oe_xp;
    إذا (xp->xp_الگو > آرگومانها->oe_مجموعه_arg && *(xp->xp_الگو-1) == ':')
    {
	// 'keyprotocol' only has well-defined terms for completion for the
	// protocol part after the colon.
	أرجع expand_مجموعه_opt_string(
		آرگومانها,
		p_kpc_protocol_مقادیر,
		ARRAY_درازی(p_kpc_protocol_مقادیر) - 1,
		رقم_Matches,
		matches);
    }
    // Use expand_set_opt_string instead of returning FAIL so that we can
    // include the original value if args->oe_include_orig_val is set.
    ساكن حرف *(خالی_2[]) = {عدم};
    أرجع expand_مجموعه_opt_string(آرگومانها, خالی_2, 0, رقم_Matches, matches);
}

/*
 * The 'lispoptions' option is changed.
 */
    حرف *
did_مجموعه_lispoptions(optset_T *آرگومانها)
{
    char_u	**varp = (char_u **)آرگومانها->os_varp;

    إذا (**varp != NUL
	    && STRCMP(*varp, "expr:0") != 0
	    && STRCMP(*varp, "expr:1") != 0)
	أرجع e_invalid_آرگومان;

    أرجع عدم;
}

    صحيح
expand_مجموعه_lispoptions(optexpand_T *آرگومانها, صحيح *رقم_Matches, char_u ***matches)
{
    ساكن حرف *(p_lop_مقادیر[]) = {"expr:0", "expr:1", عدم};
    أرجع expand_مجموعه_opt_string(
	    آرگومانها,
	    p_lop_مقادیر,
	    ARRAY_درازی(p_lop_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}

/*
 * The 'matchpairs' option is changed.
 */
    حرف *
did_مجموعه_matchpairs(optset_T *آرگومانها)
{
    char_u	**varp = (char_u **)آرگومانها->os_varp;
    char_u	*p;

    إذا (has_mbyte)
    {
	لكل (p = *varp; *p != NUL; ++p)
	{
	    صحيح x2 = -1;
	    صحيح x3 = -1;

	    p += mb_ptr2len(p);
	    إذا (*p != NUL)
		x2 = *p++;
	    إذا (*p != NUL)
	    {
		x3 = mb_ptr2char(p);
		p += mb_ptr2len(p);
	    }
	    إذا (x2 != ':' || x3 == -1 || (*p != NUL && *p != ','))
		أرجع e_invalid_آرگومان;
	    إذا (*p == NUL)
		اكسر;
	}
    }
    وإلا
    {
	// Check for "x:y,x:y"
	لكل (p = *varp; *p != NUL; p += 4)
	{
	    إذا (p[1] != ':' || p[2] == NUL || (p[3] != NUL && p[3] != ','))
		أرجع e_invalid_آرگومان;
	    إذا (p[3] == NUL)
		اكسر;
	}
    }

    أرجع عدم;
}

/*
 * Process the updated 'messagesopt' option value.
 */
    حرف *
did_مجموعه_messagesopt(optset_T *آرگومانها استفادهنشده)
{
    إذا (messagesopt_تغییریافته() == FAIL)
	أرجع e_invalid_آرگومان;

    أرجع عدم;
}

    صحيح
expand_مجموعه_messagesopt(optexpand_T *آرگومانها, صحيح *رقم_Matches, char_u ***matches)
{
    ساكن حرف *(p_mopt_مقادیر[]) = {"hit-enter", "wait:", "history:", عدم};
    أرجع expand_مجموعه_opt_string(
	    آرگومانها,
	    p_mopt_مقادیر,
	    ARRAY_درازی(p_mopt_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}

#إذا defined(FEAT_SPELL)
/*
 * The 'mkspellmem' option is changed.
 */
    حرف *
did_مجموعه_mkspellmem(optset_T *آرگومانها استفادهنشده)
{
    إذا (spell_check_msm() != OK)
	أرجع e_invalid_آرگومان;

    أرجع عدم;
}
#پایان_اگر

/*
 * The 'mouse' option is changed.
 */
    حرف *
did_مجموعه_mouse(optset_T *آرگومانها)
{
    char_u	**varp = (char_u **)آرگومانها->os_varp;
    حرف	*retval;

    retval = did_مجموعه_option_listflag(*varp, (char_u *)MOUSE_ALL, آرگومانها->os_errbuf,
		    آرگومانها->os_errbuflen);
    إذا (retval == عدم)
    {
	redraw_tabline = TRUE;
	إذا (tabline_ارتفاع() > 0)
	    update_screen(UPD_VALID);
#إذا defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
	popup_close_info(); // Close info popup to apply new properties
#پایان_اگر
    }
    أرجع retval;
}

    صحيح
expand_مجموعه_mouse(optexpand_T *آرگومانها, صحيح *رقم_Matches, char_u ***matches)
{
    أرجع expand_مجموعه_opt_listflag(آرگومانها, (char_u*)MOUSE_ALL, رقم_Matches, matches);
}

/*
 * The 'mousemodel' option is changed.
 */
    حرف *
did_مجموعه_mousemodel(optset_T *آرگومانها استفادهنشده)
{
    إذا (check_opt_رشتهها(p_mousem, p_mousem_مقادیر, FALSE) != OK)
	أرجع e_invalid_آرگومان;
#إذا defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU) && (Xm_نسخه <= 1002)
    وإلا إذا (*p_mousem != *آرگومانها->os_oldval.string)
	// Changed from "extend" to "popup" or "popup_setpos" or vv: need
	// to create or delete the popup menus.
	gui_motif_update_mousemodel(ریشه_menu);
#پایان_اگر

    أرجع عدم;
}

    صحيح
expand_مجموعه_mousemodel(optexpand_T *آرگومانها, صحيح *رقم_Matches, char_u ***matches)
{
    أرجع expand_مجموعه_opt_string(
	    آرگومانها,
	    p_mousem_مقادیر,
	    ARRAY_درازی(p_mousem_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}

#إذا defined(FEAT_MOUSESHAPE)
    حرف *
did_مجموعه_mouseshape(optset_T *آرگومانها استفادهنشده)
{
    حرف *errmsg = عدم;

    errmsg = parse_shape_opt(SHAPE_MOUSE);
    update_mouseshape(-1);

    أرجع errmsg;
}
#پایان_اگر

/*
 * The 'nrformats' option is changed.
 */
    حرف *
did_مجموعه_nrformats(optset_T *آرگومانها)
{
    char_u	**varp = (char_u **)آرگومانها->os_varp;

    أرجع did_مجموعه_opt_رشتهها(*varp, p_nf_مقادیر, TRUE);
}

    صحيح
expand_مجموعه_nrformats(optexpand_T *آرگومانها, صحيح *رقم_Matches, char_u ***matches)
{
    أرجع expand_مجموعه_opt_string(
	    آرگومانها,
	    p_nf_مقادیر,
	    ARRAY_درازی(p_nf_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}

#إذا defined(FEAT_EVAL)
/*
 * One of the '*expr' options is changed: 'balloonexpr', 'diffexpr',
 * 'foldexpr', 'foldtext', 'formatexpr', 'includeexpr', 'indentexpr',
 * 'patchexpr', 'printexpr' and 'charconvert'.
 */
    حرف *
did_مجموعه_optexpr(optset_T *آرگومانها)
{
    char_u	**varp = (char_u **)آرگومانها->os_varp;

    // If the option value starts with <SID> or s:, then replace that with
    // the script identifier.
    char_u *name = get_scriptlocal_funcname(*varp);
    إذا (name != عدم)
    {
	free_string_option(*varp);
	*varp = name;
    }

    أرجع عدم;
}
#پایان_اگر

/*
 * The 'pastetoggle' option is changed.
 */
    حرف *
did_مجموعه_pastetoggle(optset_T *آرگومانها استفادهنشده)
{
    char_u	*p;

    // translate key codes like in a mapping
    إذا (*p_pt)
    {
	(فراغ)replace_termcodes(p_pt, &p, 0,
		REPTERM_FROM_PART | REPTERM_DO_LT, عدم);
	إذا (p != عدم)
	{
	    free_string_option(p_pt);
	    p_pt = p;
	}
    }

    أرجع عدم;
}

#إذا defined(FEAT_PROP_POPUP)
/*
 * The 'previewpopup' option is changed.
 */
    حرف *
did_مجموعه_previewpopup(optset_T *آرگومانها استفادهنشده)
{
    إذا (parse_previewpopup(عدم) == FAIL)
	أرجع e_invalid_آرگومان;

# إذا defined(FEAT_QUICKFIX)
    popup_close_info();
# endif
    أرجع عدم;
}

    ساكن صحيح
expand_مجموعه_popupoption(optexpand_T *آرگومانها, صحيح *رقم_Matches, char_u ***matches,
	صحيح previewpopup)
{
    expand_T *xp = آرگومانها->oe_xp;

    إذا (xp->xp_الگو > آرگومانها->oe_مجموعه_arg && *(xp->xp_الگو-1) == ':')
    {
	إذا (completing_مقدار_for_subopt(آرگومانها, "close")
		|| completing_مقدار_for_subopt(آرگومانها, "resize")
		|| completing_مقدار_for_subopt(آرگومانها, "shadow"))
	{
	    أرجع expand_مجموعه_opt_string(
		    آرگومانها,
		    p_popup_option_on_off_مقادیر,
		    ARRAY_درازی(p_popup_option_on_off_مقادیر) - 1,
		    رقم_Matches,
		    matches);
	}
	إذا (completing_مقدار_for_subopt(آرگومانها, "border"))
	{
	    أرجع expand_مجموعه_opt_string(
		    آرگومانها,
		    previewpopup ? p_popup_pvp_border_مقادیر
			: p_popup_cpp_border_مقادیر,
		    (previewpopup ? ARRAY_درازی(p_popup_pvp_border_مقادیر) - 1
			: ARRAY_درازی(p_popup_cpp_border_مقادیر)) - 1,
		    رقم_Matches,
		    matches);
	}
	إذا (completing_مقدار_for_subopt(آرگومانها, "align"))
	{
	    أرجع expand_مجموعه_opt_string(
		    آرگومانها,
		    p_popup_option_align_مقادیر,
		    ARRAY_درازی(p_popup_option_align_مقادیر) - 1,
		    رقم_Matches,
		    matches);
	}
	إذا (completing_مقدار_for_subopt(آرگومانها, "highlight")
		|| completing_مقدار_for_subopt(آرگومانها, "borderhighlight"))
	{
	    // Return the list of all highlight names
	    أرجع expand_مجموعه_opt_generic(
		    آرگومانها,
		    get_highlight_name,
		    رقم_Matches,
		    matches);
	}
	أرجع FAIL;
    }

    أرجع expand_مجموعه_opt_string(
	    آرگومانها,
	    previewpopup ? p_popup_pvp_option_مقادیر
		: p_popup_cpp_option_مقادیر,
	    previewpopup ? ARRAY_درازی(p_popup_pvp_option_مقادیر) - 1
		: ARRAY_درازی(p_popup_cpp_option_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}

    صحيح
expand_مجموعه_previewpopup(optexpand_T *آرگومانها, صحيح *رقم_Matches, char_u ***matches)
{
    أرجع expand_مجموعه_popupoption(آرگومانها, رقم_Matches, matches, TRUE);
}

    صحيح
expand_مجموعه_completepopup(optexpand_T *آرگومانها, صحيح *رقم_Matches, char_u ***matches)
{
    أرجع expand_مجموعه_popupoption(آرگومانها, رقم_Matches, matches, FALSE);
}
#پایان_اگر

#إذا defined(FEAT_POSTSCRIPT)
/*
 * The 'printencoding' option is changed.
 */
    حرف *
did_مجموعه_printencoding(optset_T *آرگومانها استفادهنشده)
{
    char_u	*s, *p;

    // Canonize 'printencoding' if VIM standard one
    p = enc_canonize(p_penc);
    إذا (p != عدم)
    {
	vim_free(p_penc);
	p_penc = p;
    }
    وإلا
    {
	// Ensure lower case and '-' for '_'
	لكل (s = p_penc; *s != NUL; s++)
	{
	    إذا (*s == '_')
		*s = '-';
	    وإلا
		*s = TOLOWER_ASC(*s);
	}
    }

    أرجع عدم;
}
#پایان_اگر

#إذا defined(FEAT_PRINTER)

    ساكن char_u *
get_printoptions_نامها(expand_T *xp استفادهنشده, صحيح idx)
{
    إذا (idx >= (صحيح)ARRAY_درازی(printer_opts))
	أرجع عدم;
    أرجع (char_u*)printer_opts[idx].name;
}

/*
 * Expand 'printoptions' option
 */
    صحيح
expand_مجموعه_printoptions(optexpand_T *آرگومانها, صحيح *رقم_Matches, char_u ***matches)
{
    أرجع expand_مجموعه_opt_generic(
	    آرگومانها,
	    get_printoptions_نامها,
	    رقم_Matches,
	    matches);
}
#پایان_اگر

#إذا defined(FEAT_STL_OPT)
/*
 * The 'statusline' or the 'tabline' or the 'rulerformat' option is changed.
 * "rulerformat" is TRUE if the 'rulerformat' option is changed.
 */
    ساكن حرف *
parse_statustabline_rulerformat(optset_T *آرگومانها, صحيح rulerformat)
{
    char_u	**varp = (char_u **)آرگومانها->os_varp;
    char_u	*s;
    حرف	*errmsg = عدم;
    صحيح		wid;

    إذا (rulerformat)	// reset ru_wid first
	ru_wid = 0;
    s = *varp;
    إذا (rulerformat && *s == '%')
    {
	// set ru_wid if 'ruf' starts with "%99("
	إذا (*++s == '-')	// ignore a '-'
	    s++;
	wid = getdigits(&s);
	إذا (wid && *s == '(' && (errmsg = check_stl_option(p_ruf)) == عدم)
	    ru_wid = wid;
	وإلا
	{
	    // Validate the flags in 'rulerformat' only if it doesn't point to
	    // a custom function ("%!" flag).
	    إذا ((*varp)[1] != '!')
		errmsg = check_stl_option(p_ruf);
	}
    }
    // check 'statusline' or 'tabline' only if it doesn't start with "%!"
    وإلا إذا (rulerformat || s[0] != '%' || s[1] != '!')
	errmsg = check_stl_option(s);
    إذا (rulerformat && errmsg == عدم)
	comp_ستون();

    أرجع errmsg;
}
#پایان_اگر

#إذا defined(FEAT_RENDER_OPTIONS)
/*
 * The 'renderoptions' option is changed.
 */
    حرف *
did_مجموعه_renderoptions(optset_T *آرگومانها استفادهنشده)
{
    إذا (!gui_mch_مجموعه_rendering_options(p_rop))
	أرجع e_invalid_آرگومان;

    أرجع عدم;
}
#پایان_اگر

#إذا defined(FEAT_RIGHTLEFT)
/*
 * The 'rightleftcmd' option is changed.
 */
    حرف *
did_مجموعه_rightleftcmd(optset_T *آرگومانها)
{
    char_u	**varp = (char_u **)آرگومانها->os_varp;

    // Currently only "search" is a supported value.
    إذا (**varp != NUL && STRCMP(*varp, "search") != 0)
	أرجع e_invalid_آرگومان;

    أرجع عدم;
}

    صحيح
expand_مجموعه_rightleftcmd(optexpand_T *آرگومانها, صحيح *رقم_Matches, char_u ***matches)
{
    ساكن حرف *(p_rlc_مقادیر[]) = {"search", عدم};
    أرجع expand_مجموعه_opt_string(
	    آرگومانها,
	    p_rlc_مقادیر,
	    ARRAY_درازی(p_rlc_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}
#پایان_اگر

#تعریف_کن PUM_BORDER_CLEAR()   \
    افعل {                     \
	pum_مجموعه_border(FALSE); \
	pum_مجموعه_shadow(FALSE); \
	pum_مجموعه_margin(FALSE); \
    } طالما (0)

/*
 * Parse a border value from a pumopt "border:" token.
 * Returns OK on success, FAIL on error.
 */
    ساكن صحيح
parse_pumopt_border(char_u *val, صحيح len)
{
    // Use box-drawing characters only when 'encoding' is "utf-8" and
    // 'ambiwidth' is "single".
    صحيح	    can_use_box_chars = (enc_utf8 && *p_ambw == 's');
    char_u  *token;

    token = vim_strnsave(val, len);
    إذا (token == عدم)
	أرجع FAIL;

    إذا (can_use_box_chars && STRCMP(token, "single") == 0)
	pum_مجموعه_border_chars(0x2500, 0x2502, 0x2500, 0x2502, // ─ │ ─ │
		0x250c, 0x2510, 0x2518, 0x2514); // ┌ ┐ ┘ └
    وإلا إذا (can_use_box_chars && STRCMP(token, "double") == 0)
	pum_مجموعه_border_chars(0x2550, 0x2551, 0x2550, 0x2551, // ═ ║ ═ ║
		0x2554, 0x2557, 0x255D, 0x255A); // ╔ ╗ ╝  ╚
    وإلا إذا (can_use_box_chars && STRCMP(token, "round") == 0)
	pum_مجموعه_border_chars(0x2500, 0x2502, 0x2500, 0x2502, // ─ │ ─ │
		0x256d, 0x256e, 0x256f, 0x2570); // ╭ ╮ ╯ ╰
    وإلا إذا (STRCMP(token, "ascii") == 0)
	pum_مجموعه_border_chars('-', '|', '-', '|', '+', '+', '+', '+');
    وإلا إذا (STRNCMP(token, "custom:", 7) == 0)
    {
	char_u	*q = token + 7;
	صحيح	بیرون[8];

	لكل (صحيح i = 0; i < 8; i++)
	{
	    إذا (*q == NUL || *q == ',')
	    {
		vim_free(token);
		أرجع FAIL;
	    }
	    بیرون[i] = mb_ptr2char(q);
	    mb_ptr2char_adv(&q);
	    إذا (i < 7)
	    {
		إذا (*q != ';')
		{
		    vim_free(token);
		    أرجع FAIL;
		}
		q++;
	    }
	}
	إذا (*q != NUL && *q != ',')
	{
	    vim_free(token);
	    أرجع FAIL;
	}
	pum_مجموعه_border_chars(بیرون[0], بیرون[1], بیرون[2], بیرون[3], بیرون[4], بیرون[5],
		بیرون[6], بیرون[7]);
    }
    وإلا
    {
	vim_free(token);
	أرجع FAIL;
    }

    vim_free(token);
    pum_مجموعه_border(TRUE);
    أرجع OK;
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
    حرف *
did_مجموعه_pumopt(optset_T *آرگومانها)
{
    char_u  **varp = (char_u **)آرگومانها->os_varp;
    char_u  *p;
    صحيح	    have_border = FALSE;
    صحيح	    have_margin = FALSE;

    // Reset to defaults.
    PUM_BORDER_CLEAR();
    p_ph = 0;
    p_pw = 15;
    p_pmw = 0;
    p_po = 100;

    إذا (*varp == عدم || **varp == NUL)
	أرجع عدم;

    لكل (p = *varp; p != عدم && *p != NUL; )
    {
	char_u *comma = vim_strchr(p, ',');
	صحيح len;

	إذا (comma != عدم)
	    len = (صحيح)(comma - p);
	وإلا
	    len = (صحيح)STRLEN(p);

	إذا (STRNCMP(p, "border:", 7) == 0)
	{
	    إذا (have_border)
		اذهب error;
	    have_border = TRUE;
	    إذا (parse_pumopt_border(p + 7, len - 7) == FAIL)
		اذهب error;
	}
	وإلا إذا (STRNCMP(p, "height:", 7) == 0)
	{
	    دراز n = atol((حرف *)p + 7);
	    إذا (n < 0)
		اذهب error;
	    p_ph = n;
	}
	وإلا إذا (STRNCMP(p, "width:", 6) == 0)
	{
	    دراز n = atol((حرف *)p + 6);
	    إذا (n < 0)
		اذهب error;
	    p_pw = n;
	}
	وإلا إذا (STRNCMP(p, "maxwidth:", 9) == 0)
	{
	    دراز n = atol((حرف *)p + 9);
	    إذا (n < 0)
		اذهب error;
	    p_pmw = n;
	}
	وإلا إذا (STRNCMP(p, "opacity:", 8) == 0)
	{
	    دراز n = atol((حرف *)p + 8);
	    إذا (n < 0 || n > 100)
		اذهب error;
	    p_po = n;
	}
	وإلا إذا (len == 6 && STRNCMP(p, "shadow", 6) == 0)
	    pum_مجموعه_shadow(TRUE);
	وإلا إذا (len == 6 && STRNCMP(p, "margin", 6) == 0)
	{
	    have_margin = TRUE;
	    pum_مجموعه_margin(TRUE);
	}
	وإلا
	    اذهب error;

	إذا (comma != عدم)
	    p = comma + 1;
	وإلا
	    اكسر;
    }

    إذا (have_margin && !have_border)
	اذهب error;

    // Invalidate cached background for opacity changes.
    pum_opacity_تغییریافته();

    أرجع عدم;

error:
    PUM_BORDER_CLEAR();
    p_ph = 0;
    p_pw = 15;
    p_pmw = 0;
    p_po = 100;
    أرجع e_invalid_آرگومان;
}

    ساكن char_u *
get_pum_border_style(expand_T *xp استفادهنشده, صحيح idx)
{
    ساكن حرف *styles[] = {"ascii", "custom:", "single", "double", "round"};
    أرجع idx < ((enc_utf8 && *p_ambw == 's') ? (صحيح)ARRAY_درازی(styles) : 2)
	    ? (char_u *)styles[idx] : عدم;
}

    صحيح
expand_مجموعه_pumopt(optexpand_T *آرگومانها, صحيح *رقم_Matches, char_u ***matches)
{
    expand_T *xp = آرگومانها->oe_xp;

    إذا (xp->xp_الگو > آرگومانها->oe_مجموعه_arg && *(xp->xp_الگو-1) == ':')
    {
	إذا (completing_مقدار_for_subopt(آرگومانها, "border"))
	{
	    أرجع expand_مجموعه_opt_generic(
		    آرگومانها, get_pum_border_style, رقم_Matches, matches);
	}
	أرجع FAIL;
    }

    ساكن حرف *(p_pumopt_مقادیر[]) = {"border:", "height:", "width:",
	"maxwidth:", "opacity:", "shadow", "margin", عدم};
    أرجع expand_مجموعه_opt_string(
	    آرگومانها,
	    p_pumopt_مقادیر,
	    ARRAY_درازی(p_pumopt_مقادیر) - 1,
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
    حرف *
did_مجموعه_pumborder(optset_T *آرگومانها)
{
    char_u  **varp = (char_u **)آرگومانها->os_varp;
    char_u  *p;
    صحيح	    len;
    صحيح	    have_border = FALSE;
    صحيح	    have_margin = FALSE;

    PUM_BORDER_CLEAR();

    إذا (*varp == عدم || **varp == NUL)
	أرجع عدم;

    لكل (p = *varp; p != عدم && *p != NUL; )
    {
	char_u *comma = vim_strchr(p, ',');
	إذا (comma != عدم)
	    len = (صحيح)(comma - p);
	وإلا
	    len = (صحيح)STRLEN(p);

	إذا (STRNCMP(p, "shadow", len) == 0 && len == 6)
	    pum_مجموعه_shadow(TRUE);
	وإلا إذا (STRNCMP(p, "margin", len) == 0 && len == 6)
	{
	    have_margin = TRUE;
	    pum_مجموعه_margin(TRUE);
	}
	وإلا
	{
	    إذا (have_border)
		اذهب error;
	    have_border = TRUE;
	    إذا (parse_pumopt_border(p, len) == FAIL)
		اذهب error;
	}

	إذا (comma != عدم)
	    p = comma + 1;
	وإلا
	    اكسر;
    }

    إذا (have_margin && !have_border)
	اذهب error;

    أرجع عدم;

error:
    PUM_BORDER_CLEAR();
    pum_مجموعه_border_chars(0, 0, 0, 0, 0, 0, 0, 0);
    أرجع e_invalid_آرگومان;
}

    ساكن char_u *
get_pumborder_token(expand_T *xp, صحيح idx)
{
    أرجع idx == 0 ? (char_u *)"margin"
	 : idx == 1 ? (char_u *)"shadow"
	 : get_pum_border_style(xp, idx - 2);
}

    صحيح
expand_مجموعه_pumborder(optexpand_T *آرگومانها, صحيح *رقم_Matches, char_u ***matches)
{
    أرجع expand_مجموعه_opt_generic(
	    آرگومانها, get_pumborder_token, رقم_Matches, matches);
}

#إذا defined(FEAT_STL_OPT)

/*
 * The 'rulerformat' option is changed.
 */
    حرف *
did_مجموعه_rulerformat(optset_T *آرگومانها)
{
    أرجع parse_statustabline_rulerformat(آرگومانها, TRUE);
}
#پایان_اگر

#إذا defined(FEAT_TABPANEL)
/*
 * Process the new 'tabpanelopt' option value.
 */
    حرف *
did_مجموعه_tabpanelopt(optset_T *آرگومانها استفادهنشده)
{
    إذا (tabpanelopt_تغییریافته() == FAIL)
	أرجع e_invalid_آرگومان;

    أرجع عدم;
}

    صحيح
expand_مجموعه_tabpanelopt(optexpand_T *آرگومانها, صحيح *رقم_Matches, char_u ***matches)
{
    expand_T *xp = آرگومانها->oe_xp;

    إذا (xp->xp_الگو > آرگومانها->oe_مجموعه_arg && *(xp->xp_الگو-1) == ':')
    {
	إذا (completing_مقدار_for_subopt(آرگومانها, "align"))
	{
	    أرجع expand_مجموعه_opt_string(
		    آرگومانها,
		    p_tplo_align_مقادیر,
		    ARRAY_درازی(p_tplo_align_مقادیر) - 1,
		    رقم_Matches,
		    matches);
	}
	أرجع FAIL;
    }

    أرجع expand_مجموعه_opt_string(
	    آرگومانها,
	    p_tplo_مقادیر,
	    ARRAY_درازی(p_tplo_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}
#پایان_اگر

/*
 * The 'scrollopt' option is changed.
 */
    حرف *
did_مجموعه_scrollopt(optset_T *آرگومانها استفادهنشده)
{
    أرجع did_مجموعه_opt_رشتهها(p_sbo, p_scbopt_مقادیر, TRUE);
}

    صحيح
expand_مجموعه_scrollopt(optexpand_T *آرگومانها, صحيح *رقم_Matches, char_u ***matches)
{
    أرجع expand_مجموعه_opt_string(
	    آرگومانها,
	    p_scbopt_مقادیر,
	    ARRAY_درازی(p_scbopt_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}

/*
 * The 'wlseat' option is changed
 */
    حرف *
did_مجموعه_wlseat(optset_T *آرگومانها استفادهنشده)
{
#اگر_تعریف_شده FEAT_WAYLAND_CLIPBOARD
    // If there isn't any seat named 'wlseat', then let the Wayland clipboard be
    // unavailable. Ignore errors returned.
    clip_reset_wayland();
#پایان_اگر

    أرجع عدم;
}

/*
 * The 'selection' option is changed.
 */
    حرف *
did_مجموعه_selection(optset_T *آرگومانها استفادهنشده)
{
    إذا (*p_sel == NUL || check_opt_رشتهها(p_sel, p_sel_مقادیر, FALSE) != OK)
	أرجع e_invalid_آرگومان;

    أرجع عدم;
}

    صحيح
expand_مجموعه_selection(optexpand_T *آرگومانها, صحيح *رقم_Matches, char_u ***matches)
{
    أرجع expand_مجموعه_opt_string(
	    آرگومانها,
	    p_sel_مقادیر,
	    ARRAY_درازی(p_sel_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}

/*
 * The 'selectmode' option is changed.
 */
    حرف *
did_مجموعه_selectmode(optset_T *آرگومانها استفادهنشده)
{
    أرجع did_مجموعه_opt_رشتهها(p_slm, p_slm_مقادیر, TRUE);
}

    صحيح
expand_مجموعه_selectmode(optexpand_T *آرگومانها, صحيح *رقم_Matches, char_u ***matches)
{
    أرجع expand_مجموعه_opt_string(
	    آرگومانها,
	    p_slm_مقادیر,
	    ARRAY_درازی(p_slm_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}

#إذا defined(FEAT_SESSION)
/*
 * The 'sessionoptions' option is changed.
 */
    حرف *
did_مجموعه_sessionoptions(optset_T *آرگومانها)
{
    إذا (opt_رشتهها_flags(p_ssop, p_ssop_مقادیر, &ssop_flags, TRUE) != OK)
	أرجع e_invalid_آرگومان;
    إذا ((ssop_flags & SSOP_CURDIR) && (ssop_flags & SSOP_SESDIR))
    {
	// Don't allow both "sesdir" and "curdir".
	(فراغ)opt_رشتهها_flags(آرگومانها->os_oldval.string, p_ssop_مقادیر,
							&ssop_flags, TRUE);
	أرجع e_invalid_آرگومان;
    }

    أرجع عدم;
}

    صحيح
expand_مجموعه_sessionoptions(
    optexpand_T		*آرگومانها,
    صحيح			*رقم_Matches,
    char_u		***matches)
{
    أرجع expand_مجموعه_opt_string(
	    آرگومانها,
	    p_ssop_مقادیر,
	    ARRAY_درازی(p_ssop_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}
#پایان_اگر

/*
 * Validate 'shellpipe'/'shellredir' option.
 */
    حرف *
did_مجموعه_shellpipe_redir(optset_T *آرگومانها)
{
    char_u	*p;
    bool	دیده = false;

    لكل (p = آرگومانها->os_newval.string; *p != NUL; ++p)
    {
	إذا (*p != '%')
	    استمر;

	إذا (p[1] == NUL)
	    أرجع e_invalid_format_string_single_percent_s;

	إذا (p[1] == '%')
	{
	    ++p;    // skip second %
	    استمر;
	}

	إذا (p[1] == 's')
	{
	    إذا (دیده)
		أرجع e_invalid_format_string_single_percent_s;

	    دیده = true;
	    ++p;    // consume 's'
	    استمر;
	}
	أرجع e_invalid_format_string_single_percent_s;
    }
    أرجع عدم;
}

/*
 * The 'shortmess' option is changed.
 */
    حرف *
did_مجموعه_shortmess(optset_T *آرگومانها)
{
    char_u	**varp = (char_u **)آرگومانها->os_varp;

    أرجع did_مجموعه_option_listflag(*varp, (char_u *)SHM_ALL, آرگومانها->os_errbuf,
		    آرگومانها->os_errbuflen);
}

    صحيح
expand_مجموعه_shortmess(optexpand_T *آرگومانها, صحيح *رقم_Matches, char_u ***matches)
{
    أرجع expand_مجموعه_opt_listflag(آرگومانها, (char_u*)SHM_ALL, رقم_Matches, matches);
}

#إذا defined(FEAT_LINEBREAK)
/*
 * The 'showbreak' option is changed.
 */
    حرف *
did_مجموعه_showbreak(optset_T *آرگومانها)
{
    char_u	**varp = (char_u **)آرگومانها->os_varp;
    char_u	*s;

    لكل (s = *varp; *s; )
    {
	إذا (ptr2cells(s) != 1)
	    أرجع e_showbreak_شامل_است_unprintable_or_wide_character;
	MB_PTR_ADV(s);
    }

    أرجع عدم;
}
#پایان_اگر

/*
 * The 'showcmdloc' option is changed.
 */
    حرف *
did_مجموعه_showcmdloc(optset_T *آرگومانها استفادهنشده)
{
    حرف	*errmsg = did_مجموعه_opt_رشتهها(p_sloc, p_sloc_مقادیر, FALSE);

    إذا (errmsg == عدم)
	comp_ستون();

    أرجع errmsg;
}

    صحيح
expand_مجموعه_showcmdloc(optexpand_T *آرگومانها, صحيح *رقم_Matches, char_u ***matches)
{
    أرجع expand_مجموعه_opt_string(
	    آرگومانها,
	    p_sloc_مقادیر,
	    ARRAY_درازی(p_sloc_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}

#إذا defined(FEAT_SIGNS)
/*
 * The 'signcolumn' option is changed.
 */
    حرف *
did_مجموعه_signcolumn(optset_T *آرگومانها)
{
    char_u	**varp = (char_u **)آرگومانها->os_varp;

# إذا defined(FEAT_LINEBREAK)
    إذا (check_opt_رشتهها(*varp, p_scl_مقادیر, FALSE) != OK)
	أرجع e_invalid_آرگومان;
    // When changing the 'signcolumn' to or from 'number', recompute the
    // width of the number column if 'number' or 'relativenumber' is set.
    إذا (((*آرگومانها->os_oldval.string == 'n' && آرگومانها->os_oldval.string[1] == 'u')
		|| (*curwin->w_p_scl == 'n' && *(curwin->w_p_scl + 1) =='u'))
	    && (curwin->w_p_nu || curwin->w_p_rnu))
	curwin->w_nrwidth_سطر_شمار = 0;
# endif

# إذا defined(FEAT_GUI)
    // In the GUI, when sign icons are used, a full screen clear is needed
    // to properly redraw the sign icons.
    إذا (gui.درون_use && curbuf->b_signlist != عدم)
	redraw_all_later(UPD_CLEAR);
# endif

    أرجع عدم;
}

    صحيح
expand_مجموعه_signcolumn(optexpand_T *آرگومانها, صحيح *رقم_Matches, char_u ***matches)
{
    أرجع expand_مجموعه_opt_string(
	    آرگومانها,
	    p_scl_مقادیر,
	    ARRAY_درازی(p_scl_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}
#پایان_اگر

#إذا defined(FEAT_SPELL)
/*
 * The 'spellcapcheck' option is changed.
 */
    حرف *
did_مجموعه_spellcapcheck(optset_T *آرگومانها استفادهنشده)
{
    // compile the regexp program.
    أرجع compile_cap_prog(curwin->w_s);
}

/*
 * The 'spellfile' option is changed.
 */
    حرف *
did_مجموعه_spellfile(optset_T *آرگومانها)
{
    char_u	**varp = (char_u **)آرگومانها->os_varp;

    إذا (!valid_spellfile(*varp))
	أرجع e_invalid_آرگومان;

    // If there is a window for this buffer in which 'spell' is set load the
    // wordlists.
    أرجع did_مجموعه_spell_option();
}

/*
 * The 'spell' option is changed.
 */
    حرف *
did_مجموعه_spelllang(optset_T *آرگومانها)
{
    char_u	**varp = (char_u **)آرگومانها->os_varp;

    إذا (!valid_spelllang(*varp))
	أرجع e_invalid_آرگومان;

    // If there is a window for this buffer in which 'spell' is set load the
    // wordlists.
    أرجع did_مجموعه_spell_option();
}

/*
 * The 'spelloptions' option is changed.
 */
    حرف *
did_مجموعه_spelloptions(optset_T *آرگومانها)
{
    char_u	**varp = (char_u **)آرگومانها->os_varp;

    إذا (**varp != NUL && STRCMP(*varp, "camel") != 0)
	أرجع e_invalid_آرگومان;

    أرجع عدم;
}

    صحيح
expand_مجموعه_spelloptions(optexpand_T *آرگومانها, صحيح *رقم_Matches, char_u ***matches)
{
    ساكن حرف *(p_spo_مقادیر[]) = {"camel", عدم};
    أرجع expand_مجموعه_opt_string(
	    آرگومانها,
	    p_spo_مقادیر,
	    ARRAY_درازی(p_spo_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}

/*
 * The 'spellsuggest' option is changed.
 */
    حرف *
did_مجموعه_spellsuggest(optset_T *آرگومانها استفادهنشده)
{
    إذا (spell_check_sps() != OK)
	أرجع e_invalid_آرگومان;

    أرجع عدم;
}

    صحيح
expand_مجموعه_spellsuggest(optexpand_T *آرگومانها, صحيح *رقم_Matches, char_u ***matches)
{
    أرجع expand_مجموعه_opt_string(
	    آرگومانها,
	    p_sps_مقادیر,
	    ARRAY_درازی(p_sps_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}
#پایان_اگر

/*
 * The 'splitkeep' option is changed.
 */
    حرف *
did_مجموعه_splitkeep(optset_T *آرگومانها استفادهنشده)
{
    win_T	*wp;
    tabpage_T	*tp;
    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	wp->w_prev_ارتفاع = wp->w_ارتفاع;
	wp->w_prev_winrow = wp->w_winrow;
    }
    أرجع did_مجموعه_opt_رشتهها(p_spk, p_spk_مقادیر, FALSE);
}

    صحيح
expand_مجموعه_splitkeep(optexpand_T *آرگومانها, صحيح *رقم_Matches, char_u ***matches)
{
    أرجع expand_مجموعه_opt_string(
	    آرگومانها,
	    p_spk_مقادیر,
	    ARRAY_درازی(p_spk_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}

#إذا defined(FEAT_STL_OPT)
/*
 * The 'statusline' option is changed.
 */
    حرف *
did_مجموعه_statusline(optset_T *آرگومانها)
{
    char_u  **varp = (char_u **)آرگومانها->os_varp;
    حرف    *ret = parse_statustabline_rulerformat(آرگومانها, FALSE);

    إذا (ret != عدم)
	أرجع ret;
    update_stl_rendered_ارتفاع(varp == &curwin->w_p_stl ? curwin : عدم);
    قاب_change_statusline_ارتفاع();

    أرجع عدم;
}

/*
 * Rewrite the 'statuslineopt' string: replace the last "maxheight:N" with
 * actual_stlh and remove earlier duplicates.  Keyword order is preserved.
 */
    ساكن فراغ
update_stlo_maxheight(char_u **varp, صحيح actual_stlh)
{
    ثابت char_u    *src = *varp;
    ثابت char_u    *آخرین_mh = عدم;
    ثابت char_u    *p;

    // Find the last "maxheight:" token.
    p = src;
    طالما (*p != NUL)
    {
	إذا (STRNCMP(p, "maxheight:", 10) == 0)
	    آخرین_mh = p;
	طالما (*p != ',' && *p != NUL)
	    ++p;
	إذا (*p == ',')
	    ++p;
    }

    إذا (آخرین_mh == عدم)
	أرجع;

    size_t	bufsize = STRLEN(src) + NUMBUFLEN + 1;
    char_u	*buf = alloc(bufsize);
    إذا (buf == عدم)
	أرجع;

    صحيح		len = 0;
    bool	need_comma = false;

    p = src;
    طالما (*p != NUL)
    {
	ثابت	char_u	*tok = p;
	صحيح	tok_len;

	طالما (*p != ',' && *p != NUL)
	    ++p;
	tok_len = (صحيح)(p - tok);
	إذا (*p == ',')
	    ++p;

	إذا (STRNCMP(tok, "maxheight:", 10) == 0)
	{
	    إذا (tok == آخرین_mh)
	    {
		// Replace the last occurrence with the actual value.
		إذا (need_comma)
		    buf[len++] = ',';
		len += vim_snprintf((حرف *)buf + len,
			bufsize - len, "maxheight:%d", actual_stlh);
		need_comma = true;
	    }
	}
	وإلا
	{
	    إذا (need_comma)
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
    حرف *
did_مجموعه_statuslineopt(optset_T *آرگومانها)
{
    char_u	**varp = (char_u **)آرگومانها->os_varp;
    win_T       *wp = varp == &curwin->w_p_stlo ? curwin : عدم;

    إذا (statuslineopt_تغییریافته(*varp, wp) == FAIL)
	أرجع e_invalid_آرگومان;

    // Sync stl_rendered_height with the current statusline.
    update_stl_rendered_ارتفاع(wp);

    // Update the maxheight value to the actual value set.
    // Note: Must be changed if p_stlo_values are changed.
    إذا (*varp != خالی_option)
    {
	صحيح actual_stlh;

	إذا (wp != عدم)
	{
	    قاب_change_statusline_ارتفاع();
	    actual_stlh = MIN(wp->w_p_stlo_mh,
		    wp->w_ارتفاع + wp->w_وضعیت_ارتفاع - p_wmh);
	}
	وإلا
	    actual_stlh = قاب_change_statusline_ارتفاع();

	// Only re-serialize when the window actually has a status line shown.
	إذا (actual_stlh > 0)
	{
	    update_stlo_maxheight(varp, actual_stlh);
	    // Update the parsed maxheight member directly.
	    إذا (wp != عدم)
		wp->w_p_stlo_mh = actual_stlh;
	    وإلا
		مجموعه_stlo_mh(actual_stlh);
	}
    }
    وإلا
	قاب_change_statusline_ارتفاع();

    إذا ((آرگومانها->os_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	// :set clears the local values.
	clear_string_option(&curwin->w_p_stlo);
	curwin->w_p_stlo_fh = -1;
	curwin->w_p_stlo_mh = -1;
    }

    أرجع عدم;
}

    صحيح
expand_مجموعه_statuslineopt(optexpand_T *آرگومانها, صحيح *رقم_Matches, char_u ***matches)
{
    أرجع expand_مجموعه_opt_string(
	    آرگومانها,
	    p_stlo_مقادیر,
	    ARRAY_درازی(p_stlo_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}
#پایان_اگر

/*
 * The 'swapsync' option is changed.
 */
    حرف *
did_مجموعه_swapsync(optset_T *آرگومانها استفادهنشده)
{
    أرجع did_مجموعه_opt_رشتهها(p_sws, p_sws_مقادیر, FALSE);
}

    صحيح
expand_مجموعه_swapsync(optexpand_T *آرگومانها, صحيح *رقم_Matches, char_u ***matches)
{
    أرجع expand_مجموعه_opt_string(
	    آرگومانها,
	    p_sws_مقادیر,
	    ARRAY_درازی(p_sws_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}

/*
 * The 'switchbuf' option is changed.
 */
    حرف *
did_مجموعه_switchbuf(optset_T *آرگومانها استفادهنشده)
{
    أرجع did_مجموعه_opt_flags(p_swb, p_swb_مقادیر, &swb_flags, TRUE);
}

    صحيح
expand_مجموعه_switchbuf(optexpand_T *آرگومانها, صحيح *رقم_Matches, char_u ***matches)
{
    أرجع expand_مجموعه_opt_string(
	    آرگومانها,
	    p_swb_مقادیر,
	    ARRAY_درازی(p_swb_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}

/*
 * The 'tabclose' option is changed.
 */
    حرف *
did_مجموعه_tabclose(optset_T *آرگومانها استفادهنشده)
{
    أرجع did_مجموعه_opt_flags(p_tcl, p_tcl_مقادیر, &tcl_flags, TRUE);
}

    صحيح
expand_مجموعه_tabclose(optexpand_T *آرگومانها, صحيح *رقم_Matches, char_u ***matches)
{
    أرجع expand_مجموعه_opt_string(
	    آرگومانها,
	    p_tcl_مقادیر,
	    ARRAY_درازی(p_tcl_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}

#إذا defined(FEAT_STL_OPT)
/*
 * The 'tabline' option is changed.
 */
    حرف *
did_مجموعه_tabline(optset_T *آرگومانها)
{
    أرجع parse_statustabline_rulerformat(آرگومانها, FALSE);
}
#پایان_اگر

/*
 * The 'tagcase' option is changed.
 */
    حرف *
did_مجموعه_tagcase(optset_T *آرگومانها)
{
    بیعلامت صحيح	*flags;
    char_u		*p;

    إذا (آرگومانها->os_flags & OPT_LOCAL)
    {
	p = curbuf->b_p_tc;
	flags = &curbuf->b_tc_flags;
    }
    وإلا
    {
	p = p_tc;
	flags = &tc_flags;
    }

    إذا ((آرگومانها->os_flags & OPT_LOCAL) && *p == NUL)
	// make the local value empty: use the global value
	*flags = 0;
    وإلا إذا (*p == NUL
	    || opt_رشتهها_flags(p, p_tc_مقادیر, flags, FALSE) != OK)
	أرجع e_invalid_آرگومان;

    أرجع عدم;
}

    صحيح
expand_مجموعه_tagcase(optexpand_T *آرگومانها, صحيح *رقم_Matches, char_u ***matches)
{
    أرجع expand_مجموعه_opt_string(
	    آرگومانها,
	    p_tc_مقادیر,
	    ARRAY_درازی(p_tc_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}

/*
 * The 'term' option is changed.
 */
    حرف *
did_مجموعه_جمله(optset_T *آرگومانها استفادهنشده)
{
    إذا (T_NAME[0] == NUL)
	أرجع e_cannot_مجموعه_جمله_مقصد_خالی_string;
#اگر_تعریف_شده FEAT_GUI
    إذا (gui.درون_use)
	أرجع e_cannot_change_جمله_درون_GUI;
    إذا (جمله_is_gui(T_NAME))
	أرجع e_use_gui_مقصد_شروع_GUI;
#پایان_اگر
    إذا (مجموعه_termname(T_NAME) == FAIL)
	أرجع e_not_یافت_درون_termcap;

    // Screen colors may have changed.
    redraw_later_clear();

    أرجع عدم;
}

/*
 * Some terminal option (t_xxx) is changed
 */
    حرف *
did_مجموعه_جمله_option(optset_T *آرگومانها)
{
    char_u	**varp = (char_u **)آرگومانها->os_varp;

    إذا (!پر_screen)
	أرجع عدم;

    // ":set t_Co=0" and ":set t_Co=1" do ":set t_Co="
    إذا (varp == &T_CCO)
    {
	صحيح colors = atoi((حرف *)T_CCO);

	// Only reinitialize colors if t_Co value has really changed to
	// avoid expensive reload of colorscheme if t_Co is set to the
	// same value multiple times.
	إذا (colors != t_colors)
	{
	    t_colors = colors;
	    إذا (t_colors <= 1)
	    {
		vim_free(T_CCO);
		T_CCO = خالی_option;
	    }
#إذا defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
	    إذا (is_جمله_win32())
	    {
		جابجا_tcap();
		آرگومانها->os_did_swaptcap = TRUE;
	    }
#پایان_اگر
	    // We now have a different color setup, initialize it again.
	    init_highlight(TRUE, FALSE);
	}
    }
    ttest(FALSE);
    إذا (varp == &T_ME)
    {
	بیرون_str(T_ME);
	redraw_later(UPD_CLEAR);
#إذا defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
	// Since t_me has been set, this probably means that the user
	// wants to use this as default colors.  Need to reset default
	// background/foreground colors.
# ifdef VIMDLL
	إذا (!gui.درون_use && !gui.starting)
# endif
	    mch_مجموعه_نرمال_colors();
#پایان_اگر
    }
    إذا (varp == &T_BE && termcap_فعال)
    {
	MAY_WANT_مقصد_LOG_THIS;

	إذا (*T_BE == NUL)
	    // When clearing t_BE we assume the user no longer wants
	    // bracketed paste, thus disable it by writing t_BD.
	    بیرون_str(T_BD);
	وإلا
	    بیرون_str(T_BE);
    }

    إذا (varp == &T_BSU || varp == &T_ESU)
	جمله_مجموعه_sync_output(جمله_SYNC_OUTPUT_OFF);

    أرجع عدم;
}


#اگر_تعریف_شده UNIX
/*
 * The 'termresize' option is changed.
 */
    حرف *
did_مجموعه_termresize(optset_T *آرگومانها استفادهنشده)
{
    // If empty or "inband", then attempt to enable in-band resize events.
    إذا (*p_trz == NUL || STRCMP(p_trz, "inband") == 0)
	جمله_مجموعه_win_resize(true);
    وإلا إذا (STRCMP(p_trz, "sigwinch") == 0)
	جمله_مجموعه_win_resize(false);
    وإلا
	أرجع e_invalid_آرگومان;

    أرجع عدم;
}

    صحيح
expand_مجموعه_termresize(optexpand_T *آرگومانها, صحيح *رقم_Matches, char_u ***matches)
{
    أرجع expand_مجموعه_opt_string(
	    آرگومانها,
	    p_trz_مقادیر,
	    ARRAY_درازی(p_trz_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}
#پایان_اگر

#إذا defined(FEAT_TERMINAL)
/*
 * The 'termwinkey' option is changed.
 */
    حرف *
did_مجموعه_termwinkey(optset_T *آرگومانها)
{
    char_u	**varp = (char_u **)آرگومانها->os_varp;

    إذا ((*varp)[0] != NUL && string_مقصد_کلید(*varp, TRUE) == 0)
	أرجع e_invalid_آرگومان;

    أرجع عدم;
}

/*
 * The 'termwinsize' option is changed.
 */
    حرف *
did_مجموعه_termwinsize(optset_T *آرگومانها)
{
    char_u	**varp = (char_u **)آرگومانها->os_varp;
    char_u	*p;

    إذا ((*varp)[0] == NUL)
	أرجع عدم;

    p = skipdigits(*varp);
    إذا (p == *varp || (*p != 'x' && *p != '*') || *skipdigits(p + 1) != NUL)
	أرجع e_invalid_آرگومان;

    أرجع عدم;
}

# إذا defined(MSWIN)
/*
 * The 'termwintype' option is changed.
 */
    حرف *
did_مجموعه_termwintype(optset_T *آرگومانها استفادهنشده)
{
    أرجع did_مجموعه_opt_رشتهها(p_twt, p_twt_مقادیر, FALSE);
}

    صحيح
expand_مجموعه_termwintype(optexpand_T *آرگومانها, صحيح *رقم_Matches, char_u ***matches)
{
    أرجع expand_مجموعه_opt_string(
	    آرگومانها,
	    p_twt_مقادیر,
	    ARRAY_درازی(p_twt_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}
# endif
#پایان_اگر

/*
 * The 'titlestring' option is changed.
 */
    حرف *
did_مجموعه_titlestring(optset_T *آرگومانها)
{
    صحيح flagval = 0;

#اگر_تعریف_شده FEAT_STL_OPT
    flagval = STL_درون_TITLE;
#پایان_اگر
    أرجع parse_titleiconstring(آرگومانها, flagval);
}

#إذا defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
/*
 * The 'toolbar' option is changed.
 */
    حرف *
did_مجموعه_toolbar(optset_T *آرگومانها استفادهنشده)
{
    إذا (opt_رشتهها_flags(p_toolbar, p_toolbar_مقادیر, &toolbar_flags,
								TRUE) != OK)
	أرجع e_invalid_آرگومان;

    بیرون_flush();
    gui_mch_show_toolbar((toolbar_flags &
		(TOOLBAR_متن | TOOLBAR_ICONS)) != 0);
    أرجع عدم;
}

    صحيح
expand_مجموعه_toolbar(optexpand_T *آرگومانها, صحيح *رقم_Matches, char_u ***matches)
{
    أرجع expand_مجموعه_opt_string(
	    آرگومانها,
	    p_toolbar_مقادیر,
	    ARRAY_درازی(p_toolbar_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}
#پایان_اگر

#إذا defined(FEAT_TOOLBAR) && defined(FEAT_GUI_GTK)
/*
 * The 'toolbariconsize' option is changed.  GTK+ 2 only.
 */
    حرف *
did_مجموعه_toolbariconsize(optset_T *آرگومانها استفادهنشده)
{
    إذا (opt_رشتهها_flags(p_tbis, p_tbis_مقادیر, &tbis_flags, FALSE) != OK)
	أرجع e_invalid_آرگومان;

    بیرون_flush();
    gui_mch_show_toolbar((toolbar_flags &
		(TOOLBAR_متن | TOOLBAR_ICONS)) != 0);
    أرجع عدم;
}

    صحيح
expand_مجموعه_toolbariconsize(
    optexpand_T	*آرگومانها,
    صحيح		*رقم_Matches,
    char_u	***matches)
{
    أرجع expand_مجموعه_opt_string(
	    آرگومانها,
	    p_tbis_مقادیر,
	    ARRAY_درازی(p_tbis_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}
#پایان_اگر

#إذا defined(UNIX) || defined(VMS)
/*
 * The 'ttymouse' option is changed.
 */
    حرف *
did_مجموعه_ttymouse(optset_T *آرگومانها استفادهنشده)
{
    حرف *errmsg = عدم;

    // Switch the mouse off before changing the escape sequences used for
    // that.
    mch_setmouse(FALSE);
    إذا (opt_رشتهها_flags(p_ttym, p_ttym_مقادیر, &ttym_flags, FALSE) != OK)
	errmsg = e_invalid_آرگومان;
    وإلا
	check_mouse_termcode();
    إذا (termcap_فعال)
	setmouse();		// may switch it on again

    أرجع errmsg;
}

    صحيح
expand_مجموعه_ttymouse(optexpand_T *آرگومانها, صحيح *رقم_Matches, char_u ***matches)
{
    أرجع expand_مجموعه_opt_string(
	    آرگومانها,
	    p_ttym_مقادیر,
	    ARRAY_درازی(p_ttym_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}
#پایان_اگر

#إذا defined(FEAT_VARTABS)
/*
 * The 'varsofttabstop' option is changed.
 */
    حرف *
did_مجموعه_varsofttabstop(optset_T *آرگومانها)
{
    char_u	**varp = (char_u **)آرگومانها->os_varp;
    char_u *cp;

    إذا (!((*varp)[0]) || ((*varp)[0] == '0' && !((*varp)[1])))
	VIM_CLEAR(curbuf->b_p_vsts_array);
    وإلا
    {
	لكل (cp = *varp; *cp; ++cp)
	{
	    إذا (vim_isdigit(*cp))
		استمر;
	    إذا (*cp == ',' && cp > *varp && *(cp-1) != ',')
		استمر;
	    أرجع e_invalid_آرگومان;
	}

	صحيح *oldarray = curbuf->b_p_vsts_array;
	إذا (tabstop_مجموعه(*varp, &(curbuf->b_p_vsts_array)) == OK)
	{
	    إذا (oldarray)
		vim_free(oldarray);
	}
	وإلا
	    أرجع e_invalid_آرگومان;
    }

    أرجع عدم;
}

/*
 * The 'vartabstop' option is changed.
 */
    حرف *
did_مجموعه_vartabstop(optset_T *آرگومانها)
{
    char_u	**varp = (char_u **)آرگومانها->os_varp;
    char_u *cp;

    إذا (!((*varp)[0]) || ((*varp)[0] == '0' && !((*varp)[1])))
	VIM_CLEAR(curbuf->b_p_vts_array);
    وإلا
    {
	لكل (cp = *varp; *cp; ++cp)
	{
	    إذا (vim_isdigit(*cp))
		استمر;
	    إذا (*cp == ',' && cp > *varp && *(cp-1) != ',')
		استمر;
	    أرجع e_invalid_آرگومان;
	}

	صحيح *oldarray = curbuf->b_p_vts_array;

	إذا (tabstop_مجموعه(*varp, &(curbuf->b_p_vts_array)) == OK)
	{
	    vim_free(oldarray);
# ifdef FEAT_FOLDING
	    إذا (foldmethodIsIndent(curwin))
		foldUpdateAll(curwin);
# endif
	}
	وإلا
	    أرجع e_invalid_آرگومان;
    }

    أرجع عدم;
}
#پایان_اگر

/*
 * The 'verbosefile' option is changed.
 */
    حرف *
did_مجموعه_verbosefile(optset_T *آرگومانها استفادهنشده)
{
    verbose_stop();
    إذا (*p_vfile != NUL && verbose_open() == FAIL)
	أرجع e_invalid_آرگومان;

    أرجع عدم;
}

#إذا defined(FEAT_SESSION)
/*
 * The 'viewoptions' option is changed.
 */
    حرف *
did_مجموعه_viewoptions(optset_T *آرگومانها استفادهنشده)
{
    أرجع did_مجموعه_opt_flags(p_vop, p_ssop_مقادیر, &vop_flags, TRUE);
}
#پایان_اگر

#إذا defined(FEAT_VIMINFO)
/*
 * The 'viminfo' option is changed.
 */
    حرف *
did_مجموعه_viminfo(optset_T *آرگومانها)
{
    char_u	*s;
    حرف	*errmsg = عدم;

    لكل (s = p_viminfo; *s;)
    {
	// Check it's a valid character
	إذا (vim_strchr((char_u *)"!\"%'/:<@cfhnrs", *s) == عدم)
	{
	    errmsg = illegal_char(آرگومانها->os_errbuf, آرگومانها->os_errbuflen, *s);
	    اكسر;
	}
	إذا (*s == 'n')	// name is always last one
	    اكسر;
	وإلا إذا (*s == 'r') // skip until next ','
	{
	    طالما (*++s && *s != ',')
		;
	}
	وإلا إذا (*s == '%')
	{
	    // optional number
	    طالما (vim_isdigit(*++s))
		;
	}
	وإلا إذا (*s == '!' || *s == 'h' || *s == 'c')
	    ++s;		// no extra chars
	وإلا		// must have a number
	{
	    طالما (vim_isdigit(*++s))
		;

	    إذا (!VIM_ISDIGIT(*(s - 1)))
	    {
		إذا (آرگومانها->os_errbuf != عدم)
		{
		    vim_snprintf(آرگومانها->os_errbuf, آرگومانها->os_errbuflen,
			    _(e_missing_عدد_after_angle_str_angle),
			    transchar_byte(*(s - 1)));
		    errmsg = آرگومانها->os_errbuf;
		}
		وإلا
		    errmsg = "";
		اكسر;
	    }
	}
	إذا (*s == ',')
	    ++s;
	وإلا إذا (*s)
	{
	    إذا (آرگومانها->os_errbuf != عدم)
		errmsg = e_missing_comma;
	    وإلا
		errmsg = "";
	    اكسر;
	}
    }
    إذا (*p_viminfo && errmsg == عدم && get_viminfo_parameter('\'') < 0)
	errmsg = e_must_specify_a_مقدار;

    أرجع errmsg;
}
#پایان_اگر

/*
 * The 'virtualedit' option is changed.
 */
    حرف *
did_مجموعه_virtualedit(optset_T *آرگومانها)
{
    char_u		*ve = p_ve;
    بیعلامت صحيح	*flags = &ve_flags;

    إذا (آرگومانها->os_flags & OPT_LOCAL)
    {
	ve = curwin->w_p_ve;
	flags = &curwin->w_ve_flags;
    }

    إذا ((آرگومانها->os_flags & OPT_LOCAL) && *ve == NUL)
	// make the local value empty: use the global value
	*flags = 0;
    وإلا
    {
	إذا (opt_رشتهها_flags(ve, p_ve_مقادیر, flags, TRUE) != OK)
	    أرجع e_invalid_آرگومان;
	وإلا إذا (STRCMP(ve, آرگومانها->os_oldval.string) != 0)
	{
	    // Recompute cursor position in case the new 've' setting
	    // changes something.
	    validate_virtcol();
	    coladvance(curwin->w_virtcol);
	}
    }

    أرجع عدم;
}

    صحيح
expand_مجموعه_virtualedit(optexpand_T *آرگومانها, صحيح *رقم_Matches, char_u ***matches)
{
    أرجع expand_مجموعه_opt_string(
	    آرگومانها,
	    p_ve_مقادیر,
	    ARRAY_درازی(p_ve_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}

/*
 * The 'whichwrap' option is changed.
 */
    حرف *
did_مجموعه_whichwrap(optset_T *آرگومانها)
{
    char_u	**varp = (char_u **)آرگومانها->os_varp;

    // Add ',' to the list flags because 'whichwrap' is a flag
    // list that is comma-separated.
    أرجع did_مجموعه_option_listflag(*varp, (char_u *)(WW_ALL ","),
		    آرگومانها->os_errbuf, آرگومانها->os_errbuflen);
}

    صحيح
expand_مجموعه_whichwrap(optexpand_T *آرگومانها, صحيح *رقم_Matches, char_u ***matches)
{
    أرجع expand_مجموعه_opt_listflag(آرگومانها, (char_u*)WW_ALL, رقم_Matches, matches);
}

/*
 * The 'wildmode' option is changed.
 */
    حرف *
did_مجموعه_wildmode(optset_T *آرگومانها استفادهنشده)
{
    إذا (check_opt_wim() == FAIL)
	أرجع e_invalid_آرگومان;
    أرجع عدم;
}

    صحيح
expand_مجموعه_wildmode(optexpand_T *آرگومانها, صحيح *رقم_Matches, char_u ***matches)
{
    أرجع expand_مجموعه_opt_string(
	    آرگومانها,
	    p_wim_مقادیر,
	    ARRAY_درازی(p_wim_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}

/*
 * The 'wildoptions' option is changed.
 */
    حرف *
did_مجموعه_wildoptions(optset_T *آرگومانها استفادهنشده)
{
    أرجع did_مجموعه_opt_رشتهها(p_wop, p_wop_مقادیر, TRUE);
}

    صحيح
expand_مجموعه_wildoptions(optexpand_T *آرگومانها, صحيح *رقم_Matches, char_u ***matches)
{
    أرجع expand_مجموعه_opt_string(
	    آرگومانها,
	    p_wop_مقادیر,
	    ARRAY_درازی(p_wop_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}

#إذا defined(FEAT_WAK)
/*
 * The 'winaltkeys' option is changed.
 */
    حرف *
did_مجموعه_winaltkeys(optset_T *آرگومانها استفادهنشده)
{
    حرف *errmsg = عدم;

    إذا (*p_wak == NUL || check_opt_رشتهها(p_wak, p_wak_مقادیر, FALSE) != OK)
	errmsg = e_invalid_آرگومان;
# ifdef FEAT_MENU
#  إذا defined(FEAT_GUI_MOTIF)
    وإلا إذا (gui.درون_use)
	gui_motif_مجموعه_mnemonics(p_wak[0] == 'y' || p_wak[0] == 'm');
#  elif defined(FEAT_GUI_GTK)
    وإلا إذا (gui.درون_use)
	gui_gtk_مجموعه_mnemonics(p_wak[0] == 'y' || p_wak[0] == 'm');
#  endif
# endif
    أرجع errmsg;
}

    صحيح
expand_مجموعه_winaltkeys(optexpand_T *آرگومانها, صحيح *رقم_Matches, char_u ***matches)
{
    أرجع expand_مجموعه_opt_string(
	    آرگومانها,
	    p_wak_مقادیر,
	    ARRAY_درازی(p_wak_مقادیر) - 1,
	    رقم_Matches,
	    matches);
}
#پایان_اگر

/*
 * The 'wincolor' option is changed.
 */
    حرف *
did_مجموعه_wincolor(optset_T *آرگومانها استفادهنشده)
{
    update_wincolor(curwin, آرگومانها->os_newval.string);
    أرجع عدم;
}

/*
 * The 'winhighlight' option is changed
 */
    حرف *
did_مجموعه_winhighlight(optset_T *آرگومانها)
{
    أرجع update_winhighlight(curwin, آرگومانها->os_newval.string);
}

/*
 * Expand 'winhighlight' option.
 */
    صحيح
expand_مجموعه_winhighlight(optexpand_T *آرگومانها, صحيح *رقم_Matches, char_u ***matches)
{
    expand_T	    *xp = آرگومانها->oe_xp;

    إذا ((xp->xp_الگو > آرگومانها->oe_مجموعه_arg && *(xp->xp_الگو-1) == ':')
	    || xp->xp_الگو == آرگومانها->oe_مجموعه_arg || *(xp->xp_الگو-1) == ',')
    {
	// After a ':' or after a ',', or at the start, so expand highlight
	// group name.

	// If starts with !, then expand 'highlight' occasions.
	إذا (*xp->xp_الگو == '!')
	    أرجع expand_hl_occasions(آرگومانها, رقم_Matches, matches, '!');
	وإلا
	    أرجع expand_مجموعه_opt_generic(
		    آرگومانها,
		    get_highlight_name,
		    رقم_Matches,
		    matches);
    }

    VIM_CLEAR(*matches);
    أرجع FAIL;
}

    صحيح
expand_مجموعه_wincolor(optexpand_T *آرگومانها, صحيح *رقم_Matches, char_u ***matches)
{
    أرجع expand_مجموعه_opt_generic(
	    آرگومانها,
	    get_highlight_name,
	    رقم_Matches,
	    matches);
}

#اگر_تعریف_شده FEAT_SYN_HL
/*
 * When the 'syntax' option is set, load the syntax of that name.
 */
    ساكن فراغ
do_syntax_autocmd(صحيح مقدار_تغییریافته)
{
    ساكن صحيح syn_recursive = 0;

    ++syn_recursive;
    // Only pass TRUE for "force" when the value changed or not used
    // recursively, to avoid endless recurrence.
    apply_autocmds(EVENT_SYNTAX, curbuf->b_p_syn, curbuf->b_fname,
	    مقدار_تغییریافته || syn_recursive == 1, curbuf);
    curbuf->b_flags |= BF_SYN_مجموعه;
    --syn_recursive;
}
#پایان_اگر

/*
 * When the 'filetype' option is set, trigger the FileType autocommand.
 */
    ساكن فراغ
do_filetype_autocmd(char_u **varp, صحيح opt_flags, صحيح مقدار_تغییریافته)
{
    // Skip this when called from a modeline and the filetype was already set
    // to this value.
    إذا ((opt_flags & OPT_MODELINE) && !مقدار_تغییریافته)
	أرجع;

    ساكن صحيح  ft_recursive = 0;
    صحيح	    secure_save = secure;

    // Reset the secure flag, since the value of 'filetype' has
    // been checked to be safe.
    secure = 0;

    ++ft_recursive;
    curbuf->b_did_filetype = true;
    // Only pass TRUE for "force" when the value changed or not
    // used recursively, to avoid endless recurrence.
    apply_autocmds(EVENT_FILETYPE, curbuf->b_p_ft, curbuf->b_fname,
	    مقدار_تغییریافته || ft_recursive == 1, curbuf);
    --ft_recursive;
    // Just in case the old "curbuf" is now invalid.
    إذا (varp != &(curbuf->b_p_ft))
	varp = عدم;

    secure = secure_save;
}

#اگر_تعریف_شده FEAT_SPELL
/*
 * When the 'spelllang' option is set, source the spell/LANG.vim file in
 * 'runtimepath'.
 */
    ساكن فراغ
do_spelllang_منبع(فراغ)
{
    char_u	fname[200];
    char_u	*p;
    char_u	*q = curwin->w_s->b_p_spl;

    // Skip the first name if it is "cjk".
    إذا (STRNCMP(q, "cjk,", 4) == 0)
	q += 4;

    // They could set 'spellcapcheck' depending on the language.  Use the first
    // name in 'spelllang' up to '_region' or '.encoding'.
    لكل (p = q; *p != NUL; ++p)
	إذا (!ASCII_ISALNUM(*p) && *p != '-')
	    اكسر;
    إذا (p > q)
    {
	vim_snprintf((حرف *)fname, 200, "spell/%.*s.vim",
		(صحيح)(p - q), q);
	منبع_runtime(fname, DIP_ALL);
    }
}
#پایان_اگر

/*
 * Handle string options that need some action to perform when changed.
 * The new value must be allocated.
 * Returns NULL for success, or an untranslated error message for an error.
 */
    حرف *
did_مجموعه_string_option(
    صحيح		opt_idx,		// index in options[] table
    char_u	**varp,			// pointer to the option variable
    char_u	*oldval,		// previous value of the option
    char_u	*مقدار,			// new value of the option
    حرف	*errbuf,		// buffer for errors, or NULL
    size_t	errbuflen,		// length of error buffer
    صحيح		opt_flags,		// OPT_LOCAL and/or OPT_GLOBAL
    مجموعه_op_T    op,			// OP_ADDING/OP_PREPENDING/OP_REMOVING
    صحيح		*مقدار_checked)		// value was checked to be safe, no
					// need to set P_INSECURE
{
    حرف	*errmsg = عدم;
    long_u	free_oldval = (get_option_flags(opt_idx) & P_ALLOCED);
    opt_did_مجموعه_cb_T did_مجموعه_cb = get_option_did_مجموعه_cb(opt_idx);
    optset_T	آرگومانها;

    // 'ttytype' is an alias for 'term'.  Both 'term' and 'ttytype' point to
    // T_NAME.  If 'term' or 'ttytype' is modified, then use the index for the
    // 'term' option.  Only set the P_ALLOCED flag on 'term'.
    إذا (varp == &T_NAME)
    {
	opt_idx = findoption((char_u *)"term");
	إذا (opt_idx >= 0)
	{
	    free_oldval = (get_option_flags(opt_idx) & P_ALLOCED);
	    did_مجموعه_cb = get_option_did_مجموعه_cb(opt_idx);
	}
    }

    CLEAR_فیلد(آرگومانها);

    // Disallow changing some options from secure mode
    إذا ((secure
#اگر_تعریف_شده HAVE_SANDBOX
		|| sandbox != 0
#پایان_اگر
		) && (get_option_flags(opt_idx) & P_SECURE))
	errmsg = e_not_allowed_اینجا;
    // Check for a "normal" directory or file name in some options.
    وإلا إذا (check_illegal_مسیر_نامها(opt_idx, varp))
	errmsg = e_invalid_آرگومان;
    وإلا إذا (did_مجموعه_cb != عدم)
    {
	آرگومانها.os_varp = (char_u *)varp;
	آرگومانها.os_idx = opt_idx;
	آرگومانها.os_flags = opt_flags;
	آرگومانها.os_op = op;
	آرگومانها.os_oldval.string = oldval;
	آرگومانها.os_newval.string = مقدار;
	آرگومانها.os_errbuf = errbuf;
	آرگومانها.os_errbuflen = errbuflen;
	// Invoke the option specific callback function to validate and apply
	// the new option value.
	errmsg = did_مجموعه_cb(&آرگومانها);

	// The 'keymap', 'filetype' and 'syntax' option callback functions
	// may change the os_value_checked field.
	*مقدار_checked = آرگومانها.os_مقدار_checked;
    }

    // If an error is detected, restore the previous value.
    إذا (errmsg != عدم)
    {
	free_string_option(*varp);
	*varp = oldval;
	// When resetting some values, need to act on it.
	إذا (آرگومانها.os_restore_chartab)
	    (فراغ)init_chartab();
	إذا (varp == &p_hl)
	    (فراغ)highlight_تغییریافته();
    }
    وإلا
    {
#اگر_تعریف_شده FEAT_EVAL
	// Remember where the option was set.
	مجموعه_option_sctx_idx(opt_idx, opt_flags, current_sctx);
#پایان_اگر
	// Free string options that are in allocated memory.
	// Use "free_oldval", because recursiveness may change the flags under
	// our fingers (esp. init_highlight()).
	إذا (free_oldval)
	    free_string_option(oldval);
	مجموعه_option_flag(opt_idx, P_ALLOCED);

	إذا ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0
		&& is_global_local_option(opt_idx))
	{
	    // global option with local value set to use global value; free
	    // the local value and make it empty
	    char_u *p = get_option_varp_scope(opt_idx, OPT_LOCAL);
	    free_string_option(*(char_u **)p);
	    *(char_u **)p = خالی_option;
	}

	// May set global value for local option.
	وإلا إذا (!(opt_flags & OPT_LOCAL) && opt_flags != OPT_GLOBAL)
	    مجموعه_string_option_global(opt_idx, varp);

	// Trigger the autocommand only after setting the flags.
#اگر_تعریف_شده FEAT_SYN_HL
	إذا (varp == &(curbuf->b_p_syn))
	    do_syntax_autocmd(آرگومانها.os_مقدار_تغییریافته);
#پایان_اگر
	وإلا إذا (varp == &(curbuf->b_p_ft))
	    do_filetype_autocmd(varp, opt_flags, آرگومانها.os_مقدار_تغییریافته);
#اگر_تعریف_شده FEAT_SPELL
	إذا (varp == &(curwin->w_s->b_p_spl))
	    do_spelllang_منبع();
#پایان_اگر
    }

    إذا (varp == &p_mouse)
    {
	إذا (*p_mouse == NUL)
	    mch_setmouse(FALSE);    // switch mouse off
	وإلا
	    setmouse();		    // in case 'mouse' changed
    }

    إذا (varp == &p_rtp)
    {
	export_myvimdir();
#إذا defined(FEAT_LUA)
	update_package_paths_درون_lua();
#پایان_اگر
    }

#إذا defined(FEAT_LINEBREAK)
    // Changing Formatlistpattern when briopt includes the list setting:
    // redraw
    إذا ((varp == &p_flp || varp == &(curbuf->b_p_flp))
	    && curwin->w_briopt_list)
	redraw_all_later(UPD_NOT_VALID);
#پایان_اگر

    إذا (curwin->w_curswant != MAXCOL
		   && (get_option_flags(opt_idx) & (P_CURSWANT | P_RALL)) != 0
				&& (get_option_flags(opt_idx) & P_HLONLY) == 0)
	curwin->w_مجموعه_curswant = true;

    إذا ((opt_flags & OPT_NO_REDRAW) == 0)
    {
#اگر_تعریف_شده FEAT_GUI
	// set when changing an option that only requires a redraw in the GUI
	صحيح	redraw_gui_only = FALSE;

	إذا (varp == &p_go			// 'guioptions'
		|| varp == &p_guifont		// 'guifont'
# ifdef FEAT_GUI_TABLINE
		|| varp == &p_gtl		// 'guitablabel'
		|| varp == &p_gtt		// 'guitabtooltip'
# endif
# ifdef FEAT_XFONTSET
		|| varp == &p_guifontset	// 'guifontset'
# endif
		|| varp == &p_guifontwide	// 'guifontwide'
# إذا defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
		|| varp == &p_guiligatures	// 'guiligatures'
# endif
	    )
	    redraw_gui_only = TRUE;

	// check redraw when it's not a GUI option or the GUI is active.
	إذا (!redraw_gui_only || gui.درون_use)
#پایان_اگر
	    check_redraw(get_option_flags(opt_idx));
    }

#إذا defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
    إذا (آرگومانها.os_did_swaptcap)
    {
	مجموعه_termname((char_u *)"win32");
	init_highlight(TRUE, FALSE);
    }
#پایان_اگر

    أرجع errmsg;
}

/*
 * Check an option that can be a range of string values.
 *
 * Return OK for correct value, FAIL otherwise.
 * Empty is always OK.
 */
    ساكن صحيح
check_opt_رشتهها(
    char_u	*val,
    حرف	**مقادیر,
    صحيح		list)	    // when TRUE: accept a list of values
{
    أرجع opt_رشتهها_flags(val, مقادیر, عدم, list);
}

/*
 * Handle an option that can be a range of string values.
 * Set a flag in "*flagp" for each string present.
 *
 * Return OK for correct value, FAIL otherwise.
 * Empty is always OK.
 */
    ساكن صحيح
opt_رشتهها_flags(
    char_u	*val,		// new value
    حرف	**مقادیر,	// array of valid string values
    بیعلامت	*flagp,
    صحيح		list)		// when TRUE: accept a list of values
{
    صحيح		i;
    صحيح		len;
    بیعلامت	new_flags = 0;

    طالما (*val)
    {
	لكل (i = 0; ; ++i)
	{
	    إذا (مقادیر[i] == عدم)	// val not found in values[]
		أرجع FAIL;

	    len = (صحيح)STRLEN(مقادیر[i]);
	    إذا (STRNCMP(مقادیر[i], val, len) == 0
		    && ((list && val[len] == ',') || val[len] == NUL))
	    {
		val += len + (val[len] == ',');
		new_flags |= (1 << i);
		اكسر;		// check next item in val list
	    }
	}
    }
    إذا (flagp != عدم)
	*flagp = new_flags;

    أرجع OK;
}

/*
 * return OK if "p" is a valid fileformat name, FAIL otherwise.
 */
    صحيح
check_ff_مقدار(char_u *p)
{
    أرجع check_opt_رشتهها(p, p_ff_مقادیر, FALSE);
}

/*
 * Save the actual shortmess Flags and clear them temporarily to avoid that
 * file messages overwrites any output from the following commands.
 *
 * Caller must make sure to first call save_clear_shm_value() and then
 * restore_shm_value() exactly the same number of times.
 */
    فراغ
save_clear_shm_مقدار(فراغ)
{
    إذا (STRLEN(p_shm) >= SHM_LEN)
    {
	iemsg(e_داخلی_error_shortmess_too_long);
	أرجع;
    }

    إذا (++مجموعه_shm_recursive == 1)
    {
	STRCPY(shm_buf, p_shm);
	مجموعه_option_مقدار_give_err((char_u *)"shm", 0L, (char_u *)"", 0);
    }
}

/*
 * Restore the shortmess Flags set from the save_clear_shm_value() function.
 */
    فراغ
restore_shm_مقدار(فراغ)
{
    إذا (--مجموعه_shm_recursive == 0)
    {
	مجموعه_option_مقدار_give_err((char_u *)"shm", 0L, shm_buf, 0);
	vim_memset(shm_buf, 0, SHM_LEN);
    }
}

/*
 * Export the environment variable $MYVIMDIR to the first item in runtimepath
 */
    فراغ
export_myvimdir(فراغ)
{
    صحيح		dofree = FALSE;
    char_u	*p;
    char_u	*q = p_rtp;
    string_T	buf;

    buf.string = alloc(MAXPATHL);
    إذا (buf.string == عدم)
	أرجع;

    buf.درازی = (size_t)copy_option_part(&q, buf.string, MAXPATHL, ",");

    p = vim_getenv((char_u *)"MYVIMDIR", &dofree);

    إذا (p == عدم || STRCMP(p, buf.string) != 0)
    {
	إذا (*buf.string != NUL && !after_pathsep(buf.string, buf.string + buf.درازی))
	    STRCPY(buf.string + buf.درازی, PATHSEPSTR);
#اگر_تعریف_شده MSWIN
	// normalize path separators
	لكل (q = buf.string; *q != NUL; q++)
	    إذا (*q == '/')
		*q = '\\';
#پایان_اگر
	vim_setenv((char_u *)"MYVIMDIR", buf.string);
    }
    إذا (dofree)
	vim_free(p);
    vim_free(buf.string);
}
