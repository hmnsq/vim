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

ساکن char_u shm_buf[SHM_LEN];
ساکن صحیح سیٹ_shm_recursive = 0;

ساکن حرف *(p_ambw_اقدار[]) = {"single", "double", تہی};
ساکن حرف *(p_bg_اقدار[]) = {"light", "dark", تہی};
ساکن حرف *(p_bkc_اقدار[]) = {"yes", "auto", "no", "breaksymlink", "breakhardlink", تہی};
ساکن حرف *(p_bo_اقدار[]) = {"all", "backspace", "cursor", "complete",
				 "copy", "ctrlg", "error", "esc", "ex",
				 "hangul", "insertmode", "lang", "mess",
				 "showmatch", "operator", "register", "shell",
				 "spell", "term", "wildmode", تہی};
#اگر defined(FEAT_LINEBREAK)
// Note: Keep this in sync with briopt_check()
ساکن حرف *(p_briopt_اقدار[]) = {"shift:", "min:", "sbr", "list:", "column:", تہی};
#اگر_ختم
#اگر defined(FEAT_TABPANEL)
// Note: Keep this in sync with tabpanelopt_changed()
ساکن حرف *(p_tplo_اقدار[]) = {"align:", "columns:", "scrollbar", "vert", تہی};
ساکن حرف *(p_tplo_align_اقدار[]) = {"left", "right", تہی};
#اگر_ختم
#اگر defined(FEAT_DIFF)
// Note: Keep this in sync with diffopt_changed()
ساکن حرف *(p_dip_اقدار[]) = {"filler", "anchor", "context:", "iblank", "icase", "iwhite", "iwhiteall", "iwhiteeol", "horizontal", "vertical", "closeoff", "hiddenoff", "foldcolumn:", "followwrap", "internal", "indent-heuristic", "algorithm:", "inline:", "linematch:", تہی};
ساکن حرف *(p_dip_algorithm_اقدار[]) = {"myers", "minimal", "patience", "histogram", تہی};
ساکن حرف *(p_dip_inline_اقدار[]) = {"none", "simple", "char", "word", تہی};
#اگر_ختم
ساکن حرف *(p_nf_اقدار[]) = {"bin", "octal", "hex", "alpha", "unsigned", "blank", تہی};
ساکن حرف *(p_ff_اقدار[]) = {FF_UNIX, FF_DOS, FF_MAC, تہی};
#اگر_تعریف_شدہ HAVE_CLIPMETHOD
// Note: Keep this in sync with did_set_clipboard()
ساکن حرف *(p_cb_اقدار[]) = {"unnamed", "unnamedplus",
# ifdef FEAT_CLIPBOARD
    "autoselect", "autoselectplus", "autoselectml", "html", "exclude:",
# endif
    تہی};
#اگر_ختم
#اگر_تعریف_شدہ FEAT_CRYPT
ساکن حرف *(p_cm_اقدار[]) = {"zip", "blowfish", "blowfish2",
# ifdef FEAT_SODIUM
    "xchacha20", "xchacha20v2",
# endif
    تہی};
#اگر_ختم
ساکن حرف *(p_cmp_اقدار[]) = {"internal", "keepascii", تہی};
#اگر_تعریف_شدہ FEAT_SYN_HL
// Note: Keep this in sync with fill_culopt_flags()
ساکن حرف *(p_culopt_اقدار[]) = {"line", "screenline", "number", "both", تہی};
#اگر_ختم
ساکن حرف *(p_dy_اقدار[]) = {"lastline", "truncate", "uhex", تہی};
ساکن حرف *(p_jop_اقدار[]) = {"stack", تہی};
#اگر_تعریف_شدہ FEAT_FOLDING
ساکن حرف *(p_fdo_اقدار[]) = {"all", "block", "hor", "mark", "percent",
				 "quickfix", "search", "tag", "insert",
				 "undo", "jump", تہی};
#اگر_ختم
// Note: Keep this in sync with match_keyprotocol()
ساکن حرف *(p_kpc_protocol_اقدار[]) = {"none", "mok2", "kitty", تہی};
#اگر_تعریف_شدہ FEAT_IMAGE
// Note: Keep this in sync with match_imageprotocol()
ساکن حرف *(p_ipc_protocol_اقدار[]) = {"none", "kitty", "sixel", تہی};
#اگر_ختم
#اگر_تعریف_شدہ FEAT_PROP_POPUP
// Note: Keep this in sync with parse_popup_option()
ساکن حرف *(p_popup_cpp_option_اقدار[]) = {"align:", "border:",
    "borderhighlight:", "close:", "height:", "highlight:", "opacity:",
    "resize:", "shadow:", "width:", تہی};
ساکن حرف *(p_popup_pvp_option_اقدار[]) = {"border:",
    "borderhighlight:", "close:", "height:", "highlight:", "opacity:",
    "resize:", "shadow:", "width:", تہی};
ساکن حرف *(p_popup_option_on_off_اقدار[]) = {"on", "off", تہی};
ساکن حرف *(p_popup_cpp_border_اقدار[]) = {"single", "double", "round",
    "ascii", "on", "off", "custom:", تہی};
ساکن حرف *(p_popup_pvp_border_اقدار[]) = {"single", "double", "round",
    "ascii", "on", "off", "custom:", تہی};
ساکن حرف *(p_popup_option_align_اقدار[]) = {"item", "menu", تہی};
#اگر_ختم
#اگر defined(FEAT_SPELL)
// Note: Keep this in sync with spell_check_sps()
ساکن حرف *(p_sps_اقدار[]) = {"best", "fast", "double", "expr:", "file:", "timeout:", تہی};
#اگر_ختم
#اگر_تعریف_شدہ FEAT_SESSION
// Also used for 'viewoptions'!  Keep in sync with SSOP_ flags.
ساکن حرف *(p_ssop_اقدار[]) = {"buffers", "winpos", "resize", "winsize",
    "localoptions", "options", "help", "blank", "globals", "slash", "unix",
    "sesdir", "curdir", "folds", "cursor", "tabpages", "terminal", "skiprtp",
    تہی};
#اگر_ختم
#اگر defined(FEAT_STL_OPT)
ساکن حرف *(p_stlo_اقدار[]) = {"fixedheight", "maxheight:", تہی};
#اگر_ختم
// Keep in sync with SWB_ flags in option.h
ساکن حرف *(p_swb_اقدار[]) = {"useopen", "usetab", "split", "newtab", "vsplit", "uselast", تہی};
ساکن حرف *(p_spk_اقدار[]) = {"cursor", "screen", "topline", تہی};
ساکن حرف *(p_tc_اقدار[]) = {"followic", "ignore", "match", "followscs", "smart", تہی};
// Keep in sync with TCL_ flags in option.h
ساکن حرف *(p_tcl_اقدار[]) = {"left", "uselast", تہی};
#اگر defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
ساکن حرف *(p_toolbar_اقدار[]) = {"text", "icons", "tooltips", "horiz", تہی};
#اگر_ختم
#اگر defined(FEAT_TOOLBAR) && defined(FEAT_GUI_GTK)
ساکن حرف *(p_tbis_اقدار[]) = {"tiny", "small", "medium", "large", "huge", "giant", تہی};
#اگر_ختم
#اگر defined(UNIX) || defined(VMS)
ساکن حرف *(p_ttym_اقدار[]) = {"xterm", "xterm2", "dec", "netterm", "jsbterm", "pterm", "urxvt", "sgr", تہی};
#اگر_ختم
ساکن حرف *(p_ve_اقدار[]) = {"block", "insert", "all", "onemore", "none", "NONE", تہی};
// Note: Keep this in sync with check_opt_wim()
ساکن حرف *(p_wim_اقدار[]) = {"full", "longest", "list", "lastused", "noselect", "noinsert", تہی};
ساکن حرف *(p_wop_اقدار[]) = {"fuzzy", "tagfile", "pum", "exacttext", تہی};
#اگر_تعریف_شدہ FEAT_WAK
ساکن حرف *(p_wak_اقدار[]) = {"yes", "menu", "no", تہی};
#اگر_ختم
ساکن حرف *(p_mousem_اقدار[]) = {"extend", "popup", "popup_setpos", "mac", تہی};
ساکن حرف *(p_sel_اقدار[]) = {"inclusive", "exclusive", "old", تہی};
ساکن حرف *(p_slm_اقدار[]) = {"mouse", "key", "cmd", تہی};
ساکن حرف *(p_km_اقدار[]) = {"startsel", "stopsel", تہی};
#اگر_تعریف_شدہ FEAT_BROWSE
ساکن حرف *(p_bsdir_اقدار[]) = {"current", "last", "buffer", تہی};
#اگر_ختم
ساکن حرف *(p_scbopt_اقدار[]) = {"ver", "hor", "jump", تہی};
ساکن حرف *(p_debug_اقدار[]) = {"msg", "throw", "beep", تہی};
ساکن حرف *(p_ead_اقدار[]) = {"both", "ver", "hor", تہی};
ساکن حرف *(p_buftype_اقدار[]) = {"nofile", "nowrite", "quickfix", "help", "terminal", "acwrite", "prompt", "popup", تہی};
ساکن حرف *(p_bufhidden_اقدار[]) = {"hide", "unload", "delete", "wipe", تہی};
ساکن حرف *(p_bs_اقدار[]) = {"indent", "eol", "start", "nostop", تہی};
#اگر_تعریف_شدہ FEAT_FOLDING
ساکن حرف *(p_fdm_اقدار[]) = {"manual", "expr", "marker", "indent", "syntax",
# ifdef FEAT_DIFF
				"diff",
# endif
				تہی};
ساکن حرف *(p_fcl_اقدار[]) = {"all", تہی};
#اگر_ختم
ساکن حرف *(p_cfc_اقدار[]) = {"keyword", "files", "whole_line", تہی};
ساکن حرف *(p_cot_اقدار[]) = {"menu", "menuone", "longest", "preview", "popup", "popuphidden", "noinsert", "noselect", "fuzzy", "nosort", "preinsert", "nearest", تہی};
#اگر_تعریف_شدہ BACKSLASH_اندر_FILENAME
ساکن حرف *(p_csl_اقدار[]) = {"slash", "backslash", تہی};
#اگر_ختم
#اگر_تعریف_شدہ FEAT_SIGNS
ساکن حرف *(p_scl_اقدار[]) = {"yes", "no", "auto", "number", تہی};
#اگر_ختم
#اگر_تعریف_شدہ UNIX
ساکن حرف *(p_trz_اقدار[]) = {"inband", "sigwinch", "", تہی};
#اگر_ختم
#اگر defined(MSWIN) && defined(FEAT_TERMINAL)
ساکن حرف *(p_twt_اقدار[]) = {"winpty", "conpty", "", تہی};
#اگر_ختم
ساکن حرف *(p_sloc_اقدار[]) = {"last", "statusline", "tabline", تہی};
ساکن حرف *(p_sws_اقدار[]) = {"fsync", "sync", تہی};

ساکن صحیح check_opt_لڑیاں(char_u *val, حرف **اقدار, صحیح list);
ساکن صحیح opt_لڑیاں_flags(char_u *val, حرف **اقدار, غیرعلامتی *flagp, صحیح list);

/*
 * After setting various option values: recompute variables that depend on
 * option values.
 */
    خالی
didset_string_options(خالی)
{
    (خالی)opt_لڑیاں_flags(p_cmp, p_cmp_اقدار, &cmp_flags, TRUE);
    (خالی)opt_لڑیاں_flags(p_bkc, p_bkc_اقدار, &bkc_flags, TRUE);
    (خالی)opt_لڑیاں_flags(p_bo, p_bo_اقدار, &bo_flags, TRUE);
    (خالی)opt_لڑیاں_flags(p_cot, p_cot_اقدار, &cot_flags, TRUE);
#اگر_تعریف_شدہ FEAT_SESSION
    (خالی)opt_لڑیاں_flags(p_ssop, p_ssop_اقدار, &ssop_flags, TRUE);
    (خالی)opt_لڑیاں_flags(p_vop, p_ssop_اقدار, &vop_flags, TRUE);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_FOLDING
    (خالی)opt_لڑیاں_flags(p_fdo, p_fdo_اقدار, &fdo_flags, TRUE);
#اگر_ختم
    (خالی)opt_لڑیاں_flags(p_dy, p_dy_اقدار, &dy_flags, TRUE);
    (خالی)opt_لڑیاں_flags(p_jop, p_jop_اقدار, &jop_flags, TRUE);
    (خالی)opt_لڑیاں_flags(p_tc, p_tc_اقدار, &tc_flags, FALSE);
    (خالی)opt_لڑیاں_flags(p_ve, p_ve_اقدار, &ve_flags, TRUE);
#اگر defined(UNIX) || defined(VMS)
    (خالی)opt_لڑیاں_flags(p_ttym, p_ttym_اقدار, &ttym_flags, FALSE);
#اگر_ختم
#اگر defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
    (خالی)opt_لڑیاں_flags(p_toolbar, p_toolbar_اقدار, &toolbar_flags, TRUE);
#اگر_ختم
#اگر defined(FEAT_TOOLBAR) && defined(FEAT_GUI_GTK)
    (خالی)opt_لڑیاں_flags(p_tbis, p_tbis_اقدار, &tbis_flags, FALSE);
#اگر_ختم
    (خالی)opt_لڑیاں_flags(p_swb, p_swb_اقدار, &swb_flags, TRUE);
    (خالی)opt_لڑیاں_flags(p_tcl, p_tcl_اقدار, &tcl_flags, TRUE);
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
    خالی
trigger_optionset_string(
	صحیح	opt_idx,
	صحیح	opt_flags,
	char_u  *oldval,
	char_u  *oldval_l,
	char_u  *oldval_g,
	char_u  *newval)
{
    // Don't do this recursively.
    اگر (oldval == تہی || newval == تہی
				    || *get_vim_var_str(VV_OPTION_TYPE) != NUL)
	واپس;

    char_u buf_type[7];
    size_t buf_typelen;
    size_t oldvallen;

    oldvallen = STRLEN(oldval);
    سیٹ_vim_var_string(VV_OPTION_OLD, oldval, (صحیح)oldvallen);
    سیٹ_vim_var_string(VV_OPTION_NEW, newval, -1);
    buf_typelen = vim_snprintf_safelen((حرف *)buf_type, حجم(buf_type),
	"%s", (opt_flags & OPT_LOCAL) ? "local" : "global");
    سیٹ_vim_var_string(VV_OPTION_TYPE, buf_type, (صحیح)buf_typelen);
    اگر (opt_flags & OPT_LOCAL)
    {
	سیٹ_vim_var_string(VV_OPTION_COMMAND, (char_u *)"setlocal", STRLEN_LITERAL("setlocal"));
	سیٹ_vim_var_string(VV_OPTION_OLDLOCAL, oldval, (صحیح)oldvallen);
    }
    اگر (opt_flags & OPT_GLOBAL)
    {
	سیٹ_vim_var_string(VV_OPTION_COMMAND, (char_u *)"setglobal", STRLEN_LITERAL("setglobal"));
	سیٹ_vim_var_string(VV_OPTION_OLDGLOBAL, oldval, (صحیح)oldvallen);
    }
    اگر ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	سیٹ_vim_var_string(VV_OPTION_COMMAND, (char_u *)"set", STRLEN_LITERAL("set"));
	سیٹ_vim_var_string(VV_OPTION_OLDLOCAL, oldval_l, -1);
	سیٹ_vim_var_string(VV_OPTION_OLDGLOBAL, oldval_g, -1);
    }
    اگر (opt_flags & OPT_MODELINE)
    {
	سیٹ_vim_var_string(VV_OPTION_COMMAND, (char_u *)"modeline", STRLEN_LITERAL("modeline"));
	سیٹ_vim_var_string(VV_OPTION_OLDLOCAL, oldval, (صحیح)oldvallen);
    }
    apply_autocmds(EVENT_OPTIONSET,
	    get_option_fullname(opt_idx), تہی, FALSE,
	    تہی);
    reset_v_option_vars();
}
#اگر_ختم

    ساکن حرف *
illegal_char(حرف *errbuf, size_t errbuflen, صحیح c)
{
    اگر (errbuf == تہی)
	واپس "";
    vim_snprintf(errbuf, errbuflen, _(e_illegal_character_str),
		    (حرف *)transchar(c));
    واپس errbuf;
}

    ساکن حرف *
illegal_char_after_chr(حرف *errbuf, size_t errbuflen, صحیح c)
{
    اگر (errbuf == تہی)
	واپس "";
    vim_snprintf(errbuf, errbuflen, _(e_illegal_character_after_chr), c);
    واپس errbuf;
}

/*
 * Check string options in a buffer for NULL value.
 */
    خالی
check_buf_options(buf_T *buf)
{
    check_string_option(&buf->b_p_bh);
    check_string_option(&buf->b_p_bt);
    check_string_option(&buf->b_p_fenc);
    check_string_option(&buf->b_p_ff);
#اگر_تعریف_شدہ FEAT_ڈھونڈو_ID
    check_string_option(&buf->b_p_def);
    check_string_option(&buf->b_p_inc);
# ifdef FEAT_EVAL
    check_string_option(&buf->b_p_inex);
# endif
#اگر_ختم
#اگر defined(FEAT_EVAL)
    check_string_option(&buf->b_p_inde);
    check_string_option(&buf->b_p_indk);
#اگر_ختم
#اگر defined(FEAT_BEVAL) && defined(FEAT_EVAL)
    check_string_option(&buf->b_p_bexpr);
#اگر_ختم
#اگر defined(FEAT_CRYPT)
    check_string_option(&buf->b_p_cm);
#اگر_ختم
    check_string_option(&buf->b_p_fp);
#اگر defined(FEAT_EVAL)
    check_string_option(&buf->b_p_fex);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_CRYPT
    check_string_option(&buf->b_p_کلید);
#اگر_ختم
    check_string_option(&buf->b_p_kp);
    check_string_option(&buf->b_p_mps);
    check_string_option(&buf->b_p_fo);
    check_string_option(&buf->b_p_flp);
    check_string_option(&buf->b_p_isk);
    check_string_option(&buf->b_p_com);
    check_string_option(&buf->b_p_cms);
    check_string_option(&buf->b_p_nf);
    check_string_option(&buf->b_p_qe);
#اگر_تعریف_شدہ FEAT_SYN_HL
    check_string_option(&buf->b_p_syn);
    check_string_option(&buf->b_s.b_syn_isk);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_SPELL
    check_string_option(&buf->b_s.b_p_spc);
    check_string_option(&buf->b_s.b_p_spf);
    check_string_option(&buf->b_s.b_p_spl);
    check_string_option(&buf->b_s.b_p_spo);
#اگر_ختم
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
#اگر_تعریف_شدہ FEAT_COMPL_FUNC
    check_string_option(&buf->b_p_cfu);
    check_string_option(&buf->b_p_ofu);
    check_string_option(&buf->b_p_tsrfu);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_EVAL
    check_string_option(&buf->b_p_ffu);
    check_string_option(&buf->b_p_tfu);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_KEYMAP
    check_string_option(&buf->b_p_keymap);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_QUICKFIX
    check_string_option(&buf->b_p_gefm);
    check_string_option(&buf->b_p_gp);
    check_string_option(&buf->b_p_mp);
    check_string_option(&buf->b_p_efm);
#اگر_ختم
    check_string_option(&buf->b_p_ep);
    check_string_option(&buf->b_p_راستہ);
    check_string_option(&buf->b_p_لیبل_فہرست);
    check_string_option(&buf->b_p_tc);
    check_string_option(&buf->b_p_dict);
#اگر_تعریف_شدہ FEAT_DIFF
    check_string_option(&buf->b_p_dia);
#اگر_ختم
    check_string_option(&buf->b_p_tsr);
    check_string_option(&buf->b_p_lw);
    check_string_option(&buf->b_p_bkc);
    check_string_option(&buf->b_p_menc);
#اگر_تعریف_شدہ FEAT_VARTABS
    check_string_option(&buf->b_p_vsts);
    check_string_option(&buf->b_p_vts);
#اگر_ختم
}

/*
 * Free the string allocated for an option.
 * Checks for the string being empty_option. This may happen if we're out of
 * memory, vim_strsave() returned NULL, which was replaced by empty_option by
 * check_options().
 * Does NOT check for P_ALLOCED flag!
 */
    خالی
free_string_option(char_u *p)
{
    اگر (p != خالی_خانے_option)
	vim_free(p);
}

    خالی
clear_string_option(char_u **pp)
{
    اگر (*pp != خالی_خانے_option)
	vim_free(*pp);
    *pp = خالی_خانے_option;
}

    خالی
check_string_option(char_u **pp)
{
    اگر (*pp == تہی)
	*pp = خالی_خانے_option;
}

/*
 * Set global value for string option when it's a local option.
 */
    ساکن خالی
سیٹ_string_option_global(
    صحیح		opt_idx,	// option index
    char_u	**varp)		// pointer to option variable
{
    char_u	**p, *s;

    // the global value is always allocated
    اگر (is_کھڑکی_local_option(opt_idx))
	p = (char_u **)GLOBAL_WO(varp);
    ورنہ
	p = (char_u **)get_option_var(opt_idx);
    اگر (!is_global_option(opt_idx)
	    && p != varp
	    && (s = vim_strsave(*varp)) != تہی)
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
    خالی
سیٹ_string_option_براہ_راست(
    char_u	*name,
    صحیح		opt_idx,
    char_u	*val,
    صحیح		opt_flags,	// OPT_FREE, OPT_LOCAL and/or OPT_GLOBAL
    صحیح		سیٹ_sid غیر_مستعمل)
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
	    semsg(_(e_اندرونی_error_str), "set_string_option_direct()");
	    siemsg("For option %s", name);
	    واپس;
	}
    }

    اگر (is_hidden_option(idx))		// can't set hidden option
	واپس;

    s = vim_strsave(val);
    اگر (s == تہی)
	واپس;

    varp = (char_u **)get_option_varp_scope(idx,
	    both ? OPT_LOCAL : opt_flags);
    اگر ((opt_flags & OPT_FREE) && (get_option_flags(idx) & P_ALLOCED))
	free_string_option(*varp);
    *varp = s;

    // For buffer/window local option may also set the global value.
    اگر (both)
	سیٹ_string_option_global(idx, varp);

    سیٹ_option_flag(idx, P_ALLOCED);

    // When setting both values of a global option with a local value,
    // make the local value empty, so that the global value is used.
    اگر (is_global_local_option(idx) && both)
    {
	free_string_option(*varp);
	*varp = خالی_خانے_option;
    }
#اگر_تعریف_شدہ FEAT_EVAL
    اگر (سیٹ_sid != SID_NONE)
    {
	sctx_T script_ctx;

	اگر (سیٹ_sid == 0)
	    script_ctx = current_sctx;
	ورنہ
	{
	    script_ctx.sc_sid = سیٹ_sid;
	    script_ctx.sc_seq = 0;
	    script_ctx.sc_lnum = 0;
	    script_ctx.sc_نسخہ = 1;
	}
	سیٹ_option_sctx_idx(idx, opt_flags, script_ctx);
    }
#اگر_ختم
}

/*
 * Like set_string_option_direct(), but for a window-local option in "wp".
 * Blocks autocommands to avoid the old curwin becoming invalid.
 */
    خالی
سیٹ_string_option_براہ_راست_اندر_win(
	win_T		*wp,
	char_u		*name,
	صحیح		opt_idx,
	char_u		*val,
	صحیح		opt_flags,
	صحیح		سیٹ_sid)
{
    win_T	*save_curwin = curwin;

    بلاک_autocmds();
    curwin = wp;
    curbuf = curwin->w_بفر;
    سیٹ_string_option_براہ_راست(name, opt_idx, val, opt_flags, سیٹ_sid);
    curwin = save_curwin;
    curbuf = curwin->w_بفر;
    unblock_autocmds();
}

#اگر defined(FEAT_PROP_POPUP)
/*
 * Like set_string_option_direct(), but for a buffer-local option in "buf".
 * Blocks autocommands to avoid the old curbuf becoming invalid.
 */
    خالی
سیٹ_string_option_براہ_راست_اندر_buf(
	buf_T		*buf,
	char_u		*name,
	صحیح		opt_idx,
	char_u		*val,
	صحیح		opt_flags,
	صحیح		سیٹ_sid)
{
    buf_T	*save_curbuf = curbuf;

    بلاک_autocmds();
    curbuf = buf;
    curwin->w_بفر = curbuf;
    سیٹ_string_option_براہ_راست(name, opt_idx, val, opt_flags, سیٹ_sid);
    curbuf = save_curbuf;
    curwin->w_بفر = curbuf;
    unblock_autocmds();
}
#اگر_ختم

/*
 * Set a string option to a new value, and handle the effects.
 *
 * Returns NULL on success or an untranslated error message on error.
 */
    حرف *
سیٹ_string_option(
    صحیح		opt_idx,
    char_u	*قدر,
    صحیح		opt_flags,	// OPT_LOCAL and/or OPT_GLOBAL
    حرف	*errbuf,
    size_t	errbuflen)
{
    char_u	*s;
    char_u	**varp;
    char_u	*oldval;
#اگر defined(FEAT_EVAL)
    char_u	*oldval_l = تہی;
    char_u	*oldval_g = تہی;
    char_u	*saved_oldval = تہی;
    char_u	*saved_oldval_l = تہی;
    char_u	*saved_oldval_g = تہی;
    char_u	*saved_newval = تہی;
#اگر_ختم
    حرف	*errmsg = تہی;
    صحیح		قدر_checked = FALSE;

    اگر (is_hidden_option(opt_idx))	// don't set hidden option
	واپس تہی;

    s = vim_strsave(قدر == تہی ? (char_u *)"" : قدر);
    اگر (s == تہی)
	واپس تہی;

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
#اگر_ختم
    *varp = s;

#اگر defined(FEAT_EVAL)
    اگر (!starting
# ifdef FEAT_CRYPT
	    && !is_crypt_کلید_option(opt_idx)
# endif
       )
    {
	اگر (oldval_l != تہی)
	    saved_oldval_l = vim_strsave(oldval_l);
	اگر (oldval_g != تہی)
	    saved_oldval_g = vim_strsave(oldval_g);
	saved_oldval = vim_strsave(oldval);
	saved_newval = vim_strsave(s);
    }
#اگر_ختم
    اگر ((errmsg = did_سیٹ_string_option(opt_idx, varp, oldval, قدر, errbuf,
		    errbuflen, opt_flags, OP_NONE, &قدر_checked)) == تہی)
	did_سیٹ_option(opt_idx, opt_flags, TRUE, قدر_checked);

#اگر defined(FEAT_EVAL)
    // call autocommand after handling side effects
    اگر (errmsg == تہی)
	trigger_optionset_string(opt_idx, opt_flags,
		saved_oldval, saved_oldval_l,
		saved_oldval_g, saved_newval);
    vim_free(saved_oldval);
    vim_free(saved_oldval_l);
    vim_free(saved_oldval_g);
    vim_free(saved_newval);
#اگر_ختم
    واپس errmsg;
}

/*
 * Return TRUE if "val" is a valid 'filetype' name.
 * Also used for 'syntax' and 'keymap'.
 */
    ساکن صحیح
valid_filetype(char_u *val)
{
    واپس valid_name(val, ".-_");
}

#اگر_تعریف_شدہ FEAT_STL_OPT
/*
 * Check validity of options with the 'statusline' format.
 * Return an untranslated error message or NULL.
 */
    ساکن حرف *
check_stl_option(char_u *s)
{
    صحیح		groupdepth = 0;
    ساکن حرف errbuf[ERR_BUFLEN];
    صحیح		errbuflen = ERR_BUFLEN;

    جب (*s)
    {
	// Check for valid keys after % sequences
	جب (*s && *s != '%')
	    s++;
	اگر (!*s)
	    توڑو;
	s++;
	اگر (*s == STL_CLICKFUNC)
	{
	    اگر (s[1] == ']')
	    {
		// %[] - end click region
		s += 2;
		جاری;
	    }
	    اگر (ASCII_ISALPHA(s[1]) || s[1] == '_')
	    {
		// %[FuncName] - start click region
		char_u *rb = vim_strchr(s + 2, ']');
		اگر (rb != تہی)
		{
		    s = rb + 1;
		    جاری;
		}
	    }
	    // Bare %[ is invalid
	    واپس illegal_char(errbuf, errbuflen, *s);
	}
	اگر (*s == STL_LINEBREAK)
	{
	    // Plain %@ - line break
	    s++;
	    جاری;
	}
	اگر (*s == '%' || *s == STL_TRUNCMARK || *s == STL_SEPARATE)
	{
	    s++;
	    جاری;
	}
	اگر (*s == ')')
	{
	    s++;
	    اگر (--groupdepth < 0)
		توڑو;
	    جاری;
	}
	اگر (*s == '-')
	    s++;
	جب (VIM_ISDIGIT(*s))
	    s++;
	اگر (*s == STL_USER_HL)
	    جاری;
	اگر (*s == STL_CLICKFUNC)
	{
	    // %N[FuncName] or %N[]
	    اگر (s[1] == ']')
	    {
		s += 2;
		جاری;
	    }
	    اگر (ASCII_ISALPHA(s[1]) || s[1] == '_')
	    {
		char_u *rb = vim_strchr(s + 2, ']');
		اگر (rb != تہی)
		{
		    s = rb + 1;
		    جاری;
		}
	    }
	    // Bare %N[ is invalid
	    واپس illegal_char(errbuf, errbuflen, *s);
	}
	اگر (*s == STL_LINEBREAK)
	{
	    // %N@ - line break
	    s++;
	    جاری;
	}
	اگر (*s == '.')
	{
	    s++;
	    جب (*s && VIM_ISDIGIT(*s))
		s++;
	}
	اگر (*s == '(')
	{
	    groupdepth++;
	    جاری;
	}
	اگر (vim_strchr(STL_ALL, *s) == تہی)
	{
	    واپس illegal_char(errbuf, errbuflen, *s);
	}
	اگر (*s == '{')
	{
	    صحیح reevaluate = (*++s == '%');

	    اگر (reevaluate && *++s == '}')
		// "}" is not allowed immediately after "%{%"
		واپس illegal_char(errbuf, errbuflen, '}');
	    جب ((*s != '}' || (reevaluate && s[-1] != '%')) && *s)
		s++;
	    اگر (*s != '}')
		واپس e_unclosed_عبارت_sequence;
	}
    }
    اگر (groupdepth != 0)
	واپس e_unbalanced_groups;
    واپس تہی;
}
#اگر_ختم

/*
 * Check for a "normal" directory or file name in some options.  Disallow a
 * path separator (slash and/or backslash), wildcards and characters that are
 * often illegal in a file name. Be more permissive if "secure" is off.
 */
    ساکن صحیح
check_illegal_راستہ_نام(صحیح opt_idx, char_u **varp)
{
    واپس (((get_option_flags(opt_idx) & P_NFNAME)
		&& vim_strpbrk(*varp, (char_u *)(secure
			? "/\\*?[|;&<>\r\n" : "/\\*?[<>\r\n")) != تہی)
	    || ((get_option_flags(opt_idx) & P_NDNAME)
		&& vim_strpbrk(*varp, (char_u *)"*?[|;&<>\r\n") != تہی));
}

/*
 * An option that accepts a list of flags is changed.
 * e.g. 'viewoptions', 'switchbuf', 'casemap', etc.
 */
    ساکن حرف *
did_سیٹ_opt_flags(char_u *val, حرف **اقدار, غیرعلامتی *flagp, صحیح list)
{
    اگر (opt_لڑیاں_flags(val, اقدار, flagp, list) == FAIL)
	واپس e_invalid_دلیل;

    واپس تہی;
}

/*
 * An option that accepts a list of string values is changed.
 * e.g. 'nrformats', 'scrollopt', 'wildoptions', etc.
 */
    ساکن حرف *
did_سیٹ_opt_لڑیاں(char_u *val, حرف **اقدار, صحیح list)
{
    واپس did_سیٹ_opt_flags(val, اقدار, تہی, list);
}

/*
 * An option which is a list of flags is set.  Valid values are in 'flags'.
 */
    ساکن حرف *
did_سیٹ_option_listflag(
	char_u *val,
	char_u *flags,
	حرف *errbuf,
	size_t errbuflen)
{
    char_u	*s;

    برائے (s = val; *s; ++s)
	اگر (vim_strchr(flags, *s) == تہی)
	    واپس illegal_char(errbuf, errbuflen, *s);

    واپس تہی;
}

/*
 * Expand an option that accepts a list of fixed string values with known
 * number of items.
 */
    ساکن صحیح
expand_سیٹ_opt_string(
	optexpand_T *دلائل,
	حرف **اقدار,
	size_t عدد_اقدار,
	صحیح *عدد_Matches,
	char_u ***matches)
{
    char_u	*p;
    regmatch_T	*regmatch = دلائل->oe_regmatch;
    صحیح		include_orig_val = دلائل->oe_include_orig_val;
    char_u	*option_val = دلائل->oe_opt_قدر;

    // Assume numValues is small since they are fixed enums, so just allocate
    // upfront instead of needing two passes to calculate output size.
    *matches = ALLOC_MULT(char_u *, عدد_اقدار + 1);
    اگر (*matches == تہی)
	واپس FAIL;

    صحیح گنتی = 0;

    اگر (include_orig_val && *option_val != NUL)
    {
	p = vim_strsave(option_val);
	اگر (p == تہی)
	{
	    VIM_CLEAR(*matches);
	    واپس FAIL;
	}
	(*matches)[گنتی++] = p;
    }

    برائے (حرف **val = اقدار; *val != تہی; val++)
    {
	اگر (include_orig_val && *option_val != NUL)
	{
	    اگر (STRCMP((char_u*)*val, option_val) == 0)
		جاری;
	}
	اگر (vim_regexec(regmatch, (char_u*)(*val), (colnr_T)0))
	{
	    p = vim_strsave((char_u*)*val);
	    اگر (p == تہی)
	    {
		اگر (گنتی == 0)
		{
		    VIM_CLEAR(*matches);
		    واپس FAIL;
		}
		ورنہ
		    توڑو;
	    }
	    (*matches)[گنتی++] = p;
	}
    }
    اگر (گنتی == 0)
    {
	VIM_CLEAR(*matches);
	واپس FAIL;
    }
    *عدد_Matches = گنتی;
    واپس OK;
}

ساکن char_u *سیٹ_opt_callback_orig_option = تہی;
ساکن char_u *((*سیٹ_opt_callback_func)(expand_T *, صحیح));

/*
 * Callback used by expand_set_opt_generic to also include the original value
 * as the first item.
 */
    ساکن char_u *
expand_سیٹ_opt_generic_cb(expand_T *xp, صحیح idx)
{
    اگر (idx == 0)
    {
	اگر (سیٹ_opt_callback_orig_option != تہی)
	    واپس سیٹ_opt_callback_orig_option;
	ورنہ
	    واپس (char_u *)""; // empty strings are ignored
    }
    واپس سیٹ_opt_callback_func(xp, idx - 1);
}

/*
 * Expand an option with a callback that iterates through a list of possible
 * names using an index.
 */
    ساکن صحیح
expand_سیٹ_opt_generic(
	optexpand_T *دلائل,
	char_u *((*func)(expand_T *, صحیح)),
	صحیح *عدد_Matches,
	char_u ***matches)
{
    صحیح ret;

    سیٹ_opt_callback_orig_option = دلائل->oe_include_orig_val ?
	دلائل->oe_opt_قدر : تہی;
    سیٹ_opt_callback_func = func;

    ret = ExpandGeneric(
	    (char_u*)"", // not using fuzzy as currently EXPAND_STRING_SETTING doesn't use it
	    دلائل->oe_xp,
	    دلائل->oe_regmatch,
	    matches,
	    عدد_Matches,
	    expand_سیٹ_opt_generic_cb,
	    FALSE);

    سیٹ_opt_callback_orig_option = تہی;
    سیٹ_opt_callback_func = تہی;
    واپس ret;
}

#اگر defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
ساکن garray_T *expand_cb_ga;
ساکن optexpand_T *expand_cb_دلائل;

/*
 * Callback provided to a function in expand_set_opt_callback. Will perform
 * regex matching against the value and add to the list.
 *
 * Returns OK usually. Returns FAIL if it failed to allocate memory, and the
 * caller should terminate the enumeration.
 */
    ساکن صحیح
expand_سیٹ_opt_callback_cb(char_u *val)
{
    regmatch_T	*regmatch = expand_cb_دلائل->oe_regmatch;
    expand_T	*xp = expand_cb_دلائل->oe_xp;
    garray_T	*ga = expand_cb_ga;
    char_u	*str;

    اگر (val == تہی || *val == NUL)
	واپس OK;

    اگر (xp->xp_نمونہ[0] != NUL &&
	    !vim_regexec(regmatch, val, (colnr_T)0))
	واپس OK;

    str = vim_strsave_escaped(val, (char_u *)" \t\\");

    اگر (str == تہی)
	واپس FAIL;

    اگر (ga_grow(ga, 1) == FAIL)
    {
	vim_free(str);
	واپس FAIL;
    }

    ((char_u **)ga->ga_ڈیٹا)[ga->ga_len] = str;
    ++ga->ga_len;
    واپس OK;
}

/*
 * Expand an option with a provided function that takes a callback. The
 * function will enumerate through all options and call the callback to add it
 * to the list.
 *
 * "func" is the enumerator function that will generate the list of options.
 * "func_params" is a single parameter that will be passed to func.
 */
    ساکن صحیح
expand_سیٹ_opt_callback(
	optexpand_T *دلائل,
	خالی (*func)(optexpand_T *, خالی* params, صحیح (*cb)(char_u *val)),
	خالی *func_params,
	صحیح *عدد_Matches,
	char_u ***matches)
{
    garray_T	ga;
    صحیح		include_orig_val = دلائل->oe_include_orig_val;
    char_u	*option_val = دلائل->oe_opt_قدر;

    ga_init2(&ga, حجم(حرف *), 30);

    اگر (include_orig_val && *option_val != NUL)
    {
	char_u *p = vim_strsave(option_val);
	اگر (p == تہی)
	    واپس FAIL;
	اگر (ga_grow(&ga, 1) == FAIL)
	{
	    vim_free(p);
	    واپس FAIL;
	}
	((char_u **)ga.ga_ڈیٹا)[ga.ga_len] = p;
	++ga.ga_len;
    }

    expand_cb_ga = &ga;
    expand_cb_دلائل = دلائل;

    func(دلائل, func_params, expand_سیٹ_opt_callback_cb);

    expand_cb_ga = تہی;
    expand_cb_دلائل = تہی;

    *matches = ga.ga_ڈیٹا;
    *عدد_Matches = ga.ga_len;
    واپس OK;
}
#اگر_ختم

/*
 * Expand an option which is a list of flags.
 */
    ساکن صحیح
expand_سیٹ_opt_listflag(
	optexpand_T *دلائل,
	char_u *flags,
	صحیح *عدد_Matches,
	char_u ***matches)
{
    char_u  *p;
    char_u  *option_val = دلائل->oe_opt_قدر;
    char_u  *cmdline_val = دلائل->oe_سیٹ_arg;
    صحیح	    append = دلائل->oe_append;
    صحیح	    include_orig_val = دلائل->oe_include_orig_val && (*option_val != NUL);

    size_t عدد_flags = STRLEN(flags);

    // Assume we only have small number of flags, so just allocate max size.
    *matches = ALLOC_MULT(char_u *, عدد_flags + 1);
    اگر (*matches == تہی)
	واپس FAIL;

    صحیح گنتی = 0;

    اگر (include_orig_val)
    {
	p = vim_strsave(option_val);
	اگر (p == تہی)
	{
	    VIM_CLEAR(*matches);
	    واپس FAIL;
	}
	(*matches)[گنتی++] = p;
    }

    برائے (char_u *flag = flags; *flag != NUL; flag++)
    {
	اگر (append && vim_strchr(option_val, *flag) != تہی)
	    جاری;

	اگر (vim_strchr(cmdline_val, *flag) == تہی)
	{
	    اگر (include_orig_val
		    && option_val[1] == NUL
		    && *flag == option_val[0])
	    {
		// This value is already used as the first choice as it's the
		// existing flag. Just skip it to avoid duplicate.
		جاری;
	    }
	    p = vim_strnsave(flag, 1);
	    اگر (p == تہی)
	    {
		اگر (گنتی == 0)
		{
		    VIM_CLEAR(*matches);
		    واپس FAIL;
		}
		ورنہ
		    توڑو;
	    }
	    (*matches)[گنتی++] = p;
	}
    }

    اگر (گنتی == 0)
    {
	VIM_CLEAR(*matches);
	واپس FAIL;
    }
    *عدد_Matches = گنتی;
    واپس OK;
}

/*
 * The 'ambiwidth' option is changed.
 */
    حرف *
did_سیٹ_ambiwidth(optset_T *دلائل غیر_مستعمل)
{
    اگر (check_opt_لڑیاں(p_ambw, p_ambw_اقدار, FALSE) != OK)
	واپس e_invalid_دلیل;

    واپس check_chars_options();
}

// "name" must be a string literal, the length is computed at compile time.
#تعریف_کرو completing_قدر_for_subopt(دلائل, name) \
	  completing_قدر_for_subopt_len(دلائل, name, (صحیح)STRLEN_LITERAL(name))

/*
 * Return true when completing the value of the sub-option "name" with length
 * "len", e.g. the value after "close:" in 'completepopup'.
 */
    ساکن bool
completing_قدر_for_subopt_len(optexpand_T *دلائل, حرف *name, صحیح len)
{
    char_u  *colon = دلائل->oe_xp->xp_نمونہ - 1;
    صحیح	    off = (صحیح)(colon - دلائل->oe_سیٹ_arg);

    اگر (off < len)
	واپس false;
    // The name must follow a comma when it does not start the option value.
    اگر (off > len && *(colon - len - 1) != ',')
	واپس false;

    واپس STRNCMP(colon - len, name, len) == 0;
}

    صحیح
expand_سیٹ_ambiwidth(optexpand_T *دلائل, صحیح *عدد_Matches, char_u ***matches)
{
    واپس expand_سیٹ_opt_string(
	    دلائل,
	    p_ambw_اقدار,
	    ARRAY_لمبائی(p_ambw_اقدار) - 1,
	    عدد_Matches,
	    matches);
}

/*
 * The 'background' option is changed.
 */
    حرف *
did_سیٹ_background(optset_T *دلائل)
{
    اگر (check_opt_لڑیاں(p_bg, p_bg_اقدار, FALSE) == FAIL)
	واپس e_invalid_دلیل;

    اگر (دلائل->os_oldval.string != تہی && دلائل->os_oldval.string[0] == *p_bg)
	// Value was not changed
	واپس تہی;

#اگر_تعریف_شدہ FEAT_EVAL
    صحیح dark = (*p_bg == 'd');
#اگر_ختم

    init_highlight(FALSE, FALSE);

#اگر_تعریف_شدہ FEAT_EVAL
    اگر (dark != (*p_bg == 'd')
	    && get_var_قدر((char_u *)"g:colors_name") != تہی)
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
#اگر_ختم
#اگر_تعریف_شدہ FEAT_TERMINAL
    رکن_update_colors_all();
#اگر_ختم

    واپس تہی;
}

    صحیح
expand_سیٹ_background(optexpand_T *دلائل, صحیح *عدد_Matches, char_u ***matches)
{
    واپس expand_سیٹ_opt_string(
	    دلائل,
	    p_bg_اقدار,
	    ARRAY_لمبائی(p_bg_اقدار) - 1,
	    عدد_Matches,
	    matches);
}

/*
 * The 'backspace' option is changed.
 */
    حرف *
did_سیٹ_backspace(optset_T *دلائل غیر_مستعمل)
{
    اگر (VIM_ISDIGIT(*p_bs))
    {
	اگر (*p_bs > '3' || p_bs[1] != NUL)
	    واپس e_invalid_دلیل;
    }
    ورنہ اگر (check_opt_لڑیاں(p_bs, p_bs_اقدار, TRUE) != OK)
	واپس e_invalid_دلیل;

    واپس تہی;
}

    صحیح
expand_سیٹ_backspace(optexpand_T *دلائل, صحیح *عدد_Matches, char_u ***matches)
{
    واپس expand_سیٹ_opt_string(
	    دلائل,
	    p_bs_اقدار,
	    ARRAY_لمبائی(p_bs_اقدار) - 1,
	    عدد_Matches,
	    matches);
}

/*
 * The 'backupcopy' option is changed.
 */
    حرف *
did_سیٹ_backupcopy(optset_T *دلائل)
{
    char_u		*bkc = p_bkc;
    غیرعلامتی صحیح	*flags = &bkc_flags;
    حرف		*errmsg = تہی;

    اگر (دلائل->os_flags & OPT_LOCAL)
    {
	bkc = curbuf->b_p_bkc;
	flags = &curbuf->b_bkc_flags;
    }
    ورنہ اگر (!(دلائل->os_flags & OPT_GLOBAL))
	// When using :set, clear the local flags.
	curbuf->b_bkc_flags = 0;

    اگر ((دلائل->os_flags & OPT_LOCAL) && *bkc == NUL)
	// make the local value empty: use the global value
	*flags = 0;
    ورنہ
    {
	اگر (opt_لڑیاں_flags(bkc, p_bkc_اقدار, flags, TRUE) != OK)
	    errmsg = e_invalid_دلیل;
	اگر ((((صحیح)*flags & BKC_AUTO) != 0)
		+ (((صحیح)*flags & BKC_YES) != 0)
		+ (((صحیح)*flags & BKC_NO) != 0) != 1)
	{
	    // Must have exactly one of "auto", "yes"  and "no".
	    (خالی)opt_لڑیاں_flags(دلائل->os_oldval.string, p_bkc_اقدار,
								  flags, TRUE);
	    errmsg = e_invalid_دلیل;
	}
    }

    واپس errmsg;
}

    صحیح
expand_سیٹ_backupcopy(optexpand_T *دلائل, صحیح *عدد_Matches, char_u ***matches)
{
    واپس expand_سیٹ_opt_string(
	    دلائل,
	    p_bkc_اقدار,
	    ARRAY_لمبائی(p_bkc_اقدار) - 1,
	    عدد_Matches,
	    matches);
}

/*
 * The 'backupext' or the 'patchmode' option is changed.
 */
    حرف *
did_سیٹ_backupext_or_patchmode(optset_T *دلائل غیر_مستعمل)
{
    اگر (STRCMP(*p_bex == '.' ? p_bex + 1 : p_bex,
		*p_pm == '.' ? p_pm + 1 : p_pm) == 0)
	واپس e_backupext_and_patchmode_are_equal;

    واپس تہی;
}

/*
 * The 'belloff' option is changed.
 */
    حرف *
did_سیٹ_belloff(optset_T *دلائل غیر_مستعمل)
{
    واپس did_سیٹ_opt_flags(p_bo, p_bo_اقدار, &bo_flags, TRUE);
}

    صحیح
expand_سیٹ_belloff(optexpand_T *دلائل, صحیح *عدد_Matches, char_u ***matches)
{
    واپس expand_سیٹ_opt_string(
	    دلائل,
	    p_bo_اقدار,
	    ARRAY_لمبائی(p_bo_اقدار) - 1,
	    عدد_Matches,
	    matches);
}

#اگر defined(FEAT_LINEBREAK)
/*
 * The 'breakat' option is changed.
 */
    حرف *
did_سیٹ_breakat(optset_T *دلائل غیر_مستعمل)
{
    char_u	*p;
    صحیح		i;

    برائے (i = 0; i < 256; i++)
	breakat_flags[i] = FALSE;

    اگر (p_breakat != تہی)
	برائے (p = p_breakat; *p; p++)
	    breakat_flags[*p] = TRUE;

    واپس تہی;
}

/*
 * The 'breakindentopt' option is changed.
 */
    حرف *
did_سیٹ_breakindentopt(optset_T *دلائل)
{
    char_u	**varp = (char_u **)دلائل->os_varp;

    اگر (briopt_check(*varp, varp == &curwin->w_p_briopt ? curwin : تہی)
								      == FAIL)
	واپس e_invalid_دلیل;

    // list setting requires a redraw
    اگر (varp == &curwin->w_p_briopt && curwin->w_briopt_list)
	redraw_all_later(UPD_NOT_VALID);

    واپس تہی;
}

    صحیح
expand_سیٹ_breakindentopt(
    optexpand_T		*دلائل,
    صحیح			*عدد_Matches,
    char_u		***matches)
{
    واپس expand_سیٹ_opt_string(
	    دلائل,
	    p_briopt_اقدار,
	    ARRAY_لمبائی(p_briopt_اقدار) - 1,
	    عدد_Matches,
	    matches);
}
#اگر_ختم

#اگر defined(FEAT_BROWSE)
/*
 * The 'browsedir' option is changed.
 */
    حرف *
did_سیٹ_browsedir(optset_T *دلائل غیر_مستعمل)
{
    اگر (check_opt_لڑیاں(p_bsdir, p_bsdir_اقدار, FALSE) != OK
	    && !mch_isdir(p_bsdir))
	واپس e_invalid_دلیل;

    واپس تہی;
}

    صحیح
expand_سیٹ_browsedir(optexpand_T *دلائل, صحیح *عدد_Matches, char_u ***matches)
{
    واپس expand_سیٹ_opt_string(
	    دلائل,
	    p_bsdir_اقدار,
	    ARRAY_لمبائی(p_bsdir_اقدار) - 1,
	    عدد_Matches,
	    matches);
}
#اگر_ختم

/*
 * The 'bufhidden' option is changed.
 */
    حرف *
did_سیٹ_bufhidden(optset_T *دلائل غیر_مستعمل)
{
    واپس did_سیٹ_opt_لڑیاں(curbuf->b_p_bh, p_bufhidden_اقدار, FALSE);
}

    صحیح
expand_سیٹ_bufhidden(optexpand_T *دلائل, صحیح *عدد_Matches, char_u ***matches)
{
    واپس expand_سیٹ_opt_string(
	    دلائل,
	    p_bufhidden_اقدار,
	    ARRAY_لمبائی(p_bufhidden_اقدار) - 1,
	    عدد_Matches,
	    matches);
}

/*
 * The 'buftype' option is changed.
 */
    حرف *
did_سیٹ_buftype(optset_T *دلائل غیر_مستعمل)
{
    اگر (check_opt_لڑیاں(curbuf->b_p_bt, p_buftype_اقدار, FALSE) != OK)
	واپس e_invalid_دلیل;

    اگر (curwin->w_حالت_اونچائی)
    {
	curwin->w_redr_حالت = true;
	redraw_later(UPD_VALID);
    }
    curbuf->b_help = (curbuf->b_p_bt[0] == 'h');
    redraw_titles();

    واپس تہی;
}

    صحیح
expand_سیٹ_buftype(optexpand_T *دلائل, صحیح *عدد_Matches, char_u ***matches)
{
    واپس expand_سیٹ_opt_string(
	    دلائل,
	    p_buftype_اقدار,
	    ARRAY_لمبائی(p_buftype_اقدار) - 1,
	    عدد_Matches,
	    matches);
}

/*
 * The 'casemap' option is changed.
 */
    حرف *
did_سیٹ_casemap(optset_T *دلائل غیر_مستعمل)
{
    واپس did_سیٹ_opt_flags(p_cmp, p_cmp_اقدار, &cmp_flags, TRUE);
}

    صحیح
expand_سیٹ_casemap(optexpand_T *دلائل, صحیح *عدد_Matches, char_u ***matches)
{
    واپس expand_سیٹ_opt_string(
	    دلائل,
	    p_cmp_اقدار,
	    ARRAY_لمبائی(p_cmp_اقدار) - 1,
	    عدد_Matches,
	    matches);
}

#اگر defined(HAVE_CLIPMETHOD)
    صحیح
expand_سیٹ_clipboard(optexpand_T *دلائل, صحیح *عدد_Matches, char_u ***matches)
{
    واپس expand_سیٹ_opt_string(
	    دلائل,
	    p_cb_اقدار,
	    ARRAY_لمبائی(p_cb_اقدار) - 1,
	    عدد_Matches,
	    matches);
}
#اگر_ختم

#اگر_تعریف_شدہ HAVE_CLIPMETHOD
    حرف *
did_سیٹ_clipmethod(optset_T *دلائل غیر_مستعمل)
{
    واپس انتخاب_clipmethod();
}

    صحیح
expand_سیٹ_clipmethod(optexpand_T *دلائل, صحیح *عدد_Matches, char_u ***matches)
{
    // We want to expand using the predefined clipmethod values + clipboard
    // provider names.
    صحیح		نتیجہ;
    حرف	**اقدار;
    صحیح		گنتی, pos = 0, آغاز = 0;
# ifdef FEAT_EVAL
    dict_T	*providers = get_vim_var_dict(VV_CLIPPROVIDERS);
# ورنہ
    dict_T	*providers = تہی;
# endif
    hashtab_T	*ht = providers == تہی ? تہی : &providers->dv_hashtab;

    گنتی = (ht == تہی ? 0 : ht->ht_مستعمل);
# ifdef FEAT_WAYLAND_CLIPBOARD
    گنتی++;
    آغاز++;
# endif
# ifdef FEAT_XCLIPBOARD
    گنتی++;
    آغاز++;
# endif
    اقدار = ALLOC_MULT(حرف *, گنتی + 1); // Add NULL terminator too

    اگر (اقدار == تہی)
	واپس FAIL;

# ifdef FEAT_WAYLAND_CLIPBOARD
    اقدار[pos++] = "wayland";
# endif
# ifdef FEAT_XCLIPBOARD
    اقدار[pos++] = "x11";
# endif

    اگر (ht != تہی)
	برائے (long_u i = 0; i < ht->ht_نقاب + 1; i++)
	{
	    hashitem_T	*hi = ht->ht_array + i;

	    اگر (!HASHITEM_خالی_خانے(hi))
		اقدار[pos++] = (حرف *)vim_strsave(hi->hi_کلید);
	}
    اقدار[pos++] = تہی;

    نتیجہ = expand_سیٹ_opt_string(
	    دلائل,
	    اقدار,
	    گنتی,
	    عدد_Matches,
	    matches);

    برائے (صحیح i = آغاز; i < گنتی; i++)
	vim_free(اقدار[i]);
    vim_free(اقدار);

    واپس نتیجہ;
}
#اگر_ختم

/*
 * The global 'listchars' or 'fillchars' option is changed.
 */
    ساکن حرف *
did_سیٹ_global_listfillchars(char_u *val, صحیح opt_lcs, صحیح opt_flags,
						حرف *errbuf, size_t errbuflen)
{
    حرف	*errmsg = تہی;
    char_u	**local_ptr = opt_lcs ? &curwin->w_p_lcs : &curwin->w_p_fcs;

    // only apply the global value to "curwin" when it does not have a
    // local value
    اگر (opt_lcs)
	errmsg = سیٹ_listchars_option(curwin, val,
		**local_ptr == NUL || !(opt_flags & OPT_GLOBAL),
							    errbuf, errbuflen);
    ورنہ
	errmsg = سیٹ_fillchars_option(curwin, val,
		**local_ptr == NUL || !(opt_flags & OPT_GLOBAL),
							    errbuf, errbuflen);
    اگر (errmsg != تہی)
	واپس errmsg;

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
		(خالی)سیٹ_listchars_option(wp, wp->w_p_lcs, TRUE, تہی, 0);
	}
	ورنہ
	{
	    اگر (*wp->w_p_fcs == NUL)
		(خالی)سیٹ_fillchars_option(wp, wp->w_p_fcs, TRUE, تہی, 0);
	}
    }

    redraw_all_later(UPD_NOT_VALID);

    واپس تہی;
}

/*
 * The 'fillchars' option or the 'listchars' option is changed.
 */
    حرف *
did_سیٹ_chars_option(optset_T *دلائل)
{
    char_u **varp = (char_u **)دلائل->os_varp;
    حرف *errmsg = تہی;

    اگر (   varp == &p_lcs		// global 'listchars'
	|| varp == &p_fcs)		// global 'fillchars'
	errmsg = did_سیٹ_global_listfillchars(*varp, varp == &p_lcs,
			  دلائل->os_flags, دلائل->os_errbuf, دلائل->os_errbuflen);
    ورنہ اگر (varp == &curwin->w_p_lcs)	// local 'listchars'
	errmsg = سیٹ_listchars_option(curwin, *varp, TRUE,
					  دلائل->os_errbuf, دلائل->os_errbuflen);
    ورنہ اگر (varp == &curwin->w_p_fcs)	// local 'fillchars'
	errmsg = سیٹ_fillchars_option(curwin, *varp, TRUE,
					  دلائل->os_errbuf, دلائل->os_errbuflen);

    واپس errmsg;
}

/*
 * Expand 'fillchars' or 'listchars' option value.
 */
    صحیح
expand_سیٹ_chars_option(optexpand_T *دلائل, صحیح *عدد_Matches, char_u ***matches)
{
    char_u **varp = (char_u **)دلائل->oe_varp;
    صحیح is_lcs = (varp == &p_lcs || varp == &curwin->w_p_lcs);
    واپس expand_سیٹ_opt_generic(
	    دلائل,
	    is_lcs ? get_listchars_name : get_fillchars_name,
	    عدد_Matches,
	    matches);
}

/*
 * The 'cinoptions' option is changed.
 */
    حرف *
did_سیٹ_cinoptions(optset_T *دلائل غیر_مستعمل)
{
    // TODO: recognize errors
    parse_cino(curbuf);

    واپس تہی;
}

#اگر defined(FEAT_SYN_HL)
/*
 * The 'colorcolumn' option is changed.
 */
    حرف *
did_سیٹ_colorcolumn(optset_T *دلائل)
{
    char_u	**varp = (char_u **)دلائل->os_varp;

    واپس check_colorcolumn(*varp, varp == &curwin->w_p_cc ? curwin : تہی);
}
#اگر_ختم

/*
 * The 'comments' option is changed.
 */
    حرف *
did_سیٹ_comments(optset_T *دلائل)
{
    char_u	**varp = (char_u **)دلائل->os_varp;
    char_u	*s;
    حرف	*errmsg = تہی;

    برائے (s = *varp; *s; )
    {
	جب (*s && *s != ':')
	{
	    اگر (vim_strchr((char_u *)COM_ALL, *s) == تہی
		    && !VIM_ISDIGIT(*s) && *s != '-')
	    {
		errmsg = illegal_char(دلائل->os_errbuf, دلائل->os_errbuflen, *s);
		توڑو;
	    }
	    ++s;
	}
	اگر (*s++ == NUL)
	    errmsg = e_missing_colon;
	ورنہ اگر (*s == ',' || *s == NUL)
	    errmsg = e_zero_لمبائی_string;
	اگر (errmsg != تہی)
	    توڑو;
	جب (*s && *s != ',')
	{
	    اگر (*s == '\\' && s[1] != NUL)
		++s;
	    ++s;
	}
	s = skip_مقصد_option_part(s);
    }

    واپس errmsg;
}

/*
 * The 'commentstring' option is changed.
 */
    حرف *
did_سیٹ_commentstring(optset_T *دلائل)
{
    char_u	**varp = (char_u **)دلائل->os_varp;

    اگر (**varp != NUL && لڑی_تلاش((حرف *)*varp, "%s") == تہی)
	واپس e_commentstring_must_be_خالی_خانے_or_contain_str;

    واپس تہی;
}

/*
 * Check if value for 'complete' is valid when 'complete' option is changed.
 */
    حرف *
did_سیٹ_complete(optset_T *دلائل)
{
    char_u	**varp = (char_u **)دلائل->os_varp;
    char_u	*p, *t;
    char_u	بفر[LSIZE];
    char_u	*buf_ptr;
    char_u	char_before = NUL;
    صحیح		escape;

    برائے (p = *varp; *p; )
    {
	vim_memset(بفر, 0, LSIZE);
	buf_ptr = بفر;
	escape = 0;

	// Extract substring while handling escaped commas
	جب (*p && (*p != ',' || escape) && buf_ptr < (بفر + LSIZE - 1))
	{
	    اگر (*p == '\\' && *(p + 1) == ',')
	    {
		escape = 1;  // Mark escape mode
		p++;         // Skip '\'
	    }
	    ورنہ
	    {
		escape = 0;
		*buf_ptr++ = *p;
	    }
	    p++;
	}
	*buf_ptr = NUL;

	اگر (vim_strchr((char_u *)".wbuksid]tUFo", *بفر) == تہی)
	    واپس illegal_char(دلائل->os_errbuf, دلائل->os_errbuflen, *بفر);

	اگر (vim_strchr((char_u *)"ksF", *بفر) == تہی && *(بفر + 1) != NUL
		&& *(بفر + 1) != '^')
	    char_before = *بفر;
	ورنہ
	{
	    // Test for a number after '^'
	    اگر ((t = vim_strchr(بفر, '^')) != تہی)
	    {
		*t++ = NUL;
		اگر (!*t)
		    char_before = '^';
		ورنہ
		{
		    برائے (; *t; t++)
		    {
			اگر (!vim_isdigit(*t))
			{
			    char_before = '^';
			    توڑو;
			}
		    }
		}
	    }
	}
	اگر (char_before != NUL)
	    واپس illegal_char_after_chr(دلائل->os_errbuf, دلائل->os_errbuflen,
		    char_before);
	// Skip comma and spaces
	جب (*p == ',' || *p == ' ')
	    p++;
    }

#اگر_تعریف_شدہ FEAT_COMPL_FUNC
    اگر (سیٹ_cpt_callbacks(دلائل) != OK)
	واپس illegal_char_after_chr(دلائل->os_errbuf, دلائل->os_errbuflen,
		'F');
#اگر_ختم
    واپس تہی;
}

    صحیح
expand_سیٹ_complete(optexpand_T *دلائل, صحیح *عدد_Matches, char_u ***matches)
{
    ساکن حرف *(p_cpt_اقدار[]) = {
	".", "w", "b", "u", "k", "kspell", "s", "i", "d", "]", "t", "U", "F", "o",
	تہی};
    واپس expand_سیٹ_opt_string(
	    دلائل,
	    p_cpt_اقدار,
	    ARRAY_لمبائی(p_cpt_اقدار) - 1,
	    عدد_Matches,
	    matches);
}

/*
 * The 'completeopt' option is changed.
 */
    حرف *
did_سیٹ_completeopt(optset_T *دلائل غیر_مستعمل)
{
    char_u	*cot = p_cot;
    غیرعلامتی	*flags = &cot_flags;

    اگر (دلائل->os_flags & OPT_LOCAL)
    {
	cot = curbuf->b_p_cot;
	flags = &curbuf->b_cot_flags;
    }
    ورنہ اگر (!(دلائل->os_flags & OPT_GLOBAL))
	// When using :set, clear the local flags.
	curbuf->b_cot_flags = 0;

    اگر (opt_لڑیاں_flags(cot, p_cot_اقدار, flags, TRUE) != OK)
	واپس e_invalid_دلیل;

    واپس تہی;
}

    صحیح
expand_سیٹ_completeopt(optexpand_T *دلائل, صحیح *عدد_Matches, char_u ***matches)
{
    واپس expand_سیٹ_opt_string(
	    دلائل,
	    p_cot_اقدار,
	    ARRAY_لمبائی(p_cot_اقدار) - 1,
	    عدد_Matches,
	    matches);
}

/*
 * The 'completefuzzycollect' option is changed.
 */
    حرف *
did_سیٹ_completefuzzycollect(optset_T *دلائل غیر_مستعمل)
{
    اگر (opt_لڑیاں_flags(p_cfc, p_cfc_اقدار, &cfc_flags, TRUE) != OK)
	واپس e_invalid_دلیل;
    واپس تہی;
}

    صحیح
expand_سیٹ_completefuzzycollect(
	optexpand_T *دلائل,
	صحیح *عدد_Matches,
	char_u ***matches)
{
    واپس expand_سیٹ_opt_string(
	    دلائل,
	    p_cfc_اقدار,
	    ARRAY_لمبائی(p_cfc_اقدار) - 1,
	    عدد_Matches,
	    matches);
}

/*
 * The 'completeitemalign' option is changed.
 */
    حرف *
did_سیٹ_completeitemalign(optset_T *دلائل غیر_مستعمل)
{
    char_u	*p = p_cia;
    غیرعلامتی	new_cia_flags = 0;
    صحیح		نظر_آیا[3] = { FALSE, FALSE, FALSE };
    صحیح		گنتی = 0;
    char_u	buf[10];

    جب (*p)
    {
	copy_option_part(&p, buf, حجم(buf), ",");
	اگر (گنتی >= 3)
	    واپس e_invalid_دلیل;

	اگر (STRCMP(buf, "abbr") == 0)
	{
	    اگر (نظر_آیا[CPT_ABBR])
		واپس e_invalid_دلیل;
	    new_cia_flags = new_cia_flags * 10 + CPT_ABBR;
	    نظر_آیا[CPT_ABBR] = TRUE;
	    گنتی++;
	}
	ورنہ اگر (STRCMP(buf, "kind") == 0)
	{
	    اگر (نظر_آیا[CPT_قسم])
		واپس e_invalid_دلیل;
	    new_cia_flags = new_cia_flags * 10 + CPT_قسم;
	    نظر_آیا[CPT_قسم] = TRUE;
	    گنتی++;
	}
	ورنہ اگر (STRCMP(buf, "menu") == 0)
	{
	    اگر (نظر_آیا[CPT_MENU])
		واپس e_invalid_دلیل;
	    new_cia_flags = new_cia_flags * 10 + CPT_MENU;
	    نظر_آیا[CPT_MENU] = TRUE;
	    گنتی++;
	}
	ورنہ
	    واپس e_invalid_دلیل;
    }
    اگر (new_cia_flags == 0 || گنتی != 3)
	واپس e_invalid_دلیل;

    cia_flags = new_cia_flags;
    واپس تہی;
}

#اگر defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
/*
 * The 'completepopup' option is changed.
 */
    حرف *
did_سیٹ_completepopup(optset_T *دلائل غیر_مستعمل)
{
    اگر (parse_completepopup(تہی) == FAIL)
	واپس e_invalid_دلیل;

    popup_close_info();
    واپس تہی;
}
#اگر_ختم

#اگر defined(BACKSLASH_اندر_FILENAME)
/*
 * The 'completeslash' option is changed.
 */
    حرف *
did_سیٹ_completeslash(optset_T *دلائل غیر_مستعمل)
{
    اگر (check_opt_لڑیاں(p_csl, p_csl_اقدار, FALSE) != OK
	    || check_opt_لڑیاں(curbuf->b_p_csl, p_csl_اقدار, FALSE) != OK)
	واپس e_invalid_دلیل;

    واپس تہی;
}

    صحیح
expand_سیٹ_completeslash(optexpand_T *دلائل, صحیح *عدد_Matches, char_u ***matches)
{
    واپس expand_سیٹ_opt_string(
	    دلائل,
	    p_csl_اقدار,
	    ARRAY_لمبائی(p_csl_اقدار) - 1,
	    عدد_Matches,
	    matches);
}
#اگر_ختم

#اگر defined(FEAT_CONCEAL)
/*
 * The 'concealcursor' option is changed.
 */
    حرف *
did_سیٹ_concealcursor(optset_T *دلائل)
{
    char_u	**varp = (char_u **)دلائل->os_varp;

    واپس did_سیٹ_option_listflag(*varp, (char_u *)COCU_ALL, دلائل->os_errbuf,
		    دلائل->os_errbuflen);
}

    صحیح
expand_سیٹ_concealcursor(optexpand_T *دلائل, صحیح *عدد_Matches, char_u ***matches)
{
    واپس expand_سیٹ_opt_listflag(دلائل, (char_u*)COCU_ALL, عدد_Matches, matches);
}
#اگر_ختم

/*
 * The 'cpoptions' option is changed.
 */
    حرف *
did_سیٹ_cpoptions(optset_T *دلائل)
{
    char_u	**varp = (char_u **)دلائل->os_varp;

    واپس did_سیٹ_option_listflag(*varp, (char_u *)CPO_ALL, دلائل->os_errbuf,
		    دلائل->os_errbuflen);
}

    صحیح
expand_سیٹ_cpoptions(optexpand_T *دلائل, صحیح *عدد_Matches, char_u ***matches)
{
    واپس expand_سیٹ_opt_listflag(دلائل, (char_u*)CPO_ALL, عدد_Matches, matches);
}

#اگر defined(FEAT_CRYPT)
/*
 * The 'cryptkey' option is changed.
 */
    حرف *
did_سیٹ_cryptkey(optset_T *دلائل)
{
    // Make sure the ":set" command doesn't show the new value in the
    // history.
    remove_کلید_from_history();

    اگر (دلائل->os_op != OP_NONE)
	// Don't allow set+=/-=/^= as they can allow for substring guessing
	واپس e_invalid_دلیل;

    اگر (STRCMP(curbuf->b_p_کلید, دلائل->os_oldval.string) != 0)
    {
	// Need to update the swapfile.
	ml_سیٹ_crypt_کلید(curbuf, دلائل->os_oldval.string,
		*curbuf->b_p_cm == NUL ? p_cm : curbuf->b_p_cm);
	بدلا_اندرونی();
    }
# ifdef FEAT_SODIUM
    اگر (crypt_طریقہ_is_sodium(crypt_get_طریقہ_nr(curbuf)))
	crypt_sodium_تالا_کلید(دلائل->os_newval.string);
# endif

    واپس تہی;
}

/*
 * The 'cryptmethod' option is changed.
 */
    حرف *
did_سیٹ_cryptmethod(optset_T *دلائل)
{
    char_u  *p;
    char_u  *s;

    اگر (دلائل->os_flags & OPT_LOCAL)
	p = curbuf->b_p_cm;
    ورنہ
	p = p_cm;
    اگر (check_opt_لڑیاں(p, p_cm_اقدار, TRUE) != OK)
	واپس e_invalid_دلیل;
    ورنہ اگر (crypt_self_test() == FAIL)
	واپس e_invalid_دلیل;

    // When setting the global value to empty, make it "zip".
    اگر (*p_cm == NUL)
    {
	free_string_option(p_cm);
	p_cm = vim_strsave((char_u *)"zip");
    }
    // When using ":set cm=name" the local value is going to be empty.
    // Do that here, otherwise the crypt functions will still use the
    // local value.
    اگر ((دلائل->os_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	free_string_option(curbuf->b_p_cm);
	curbuf->b_p_cm = خالی_خانے_option;
    }

    // Need to update the swapfile when the effective method changed.
    // Set "s" to the effective old value, "p" to the effective new
    // method and compare.
    اگر ((دلائل->os_flags & OPT_LOCAL) && *دلائل->os_oldval.string == NUL)
	s = p_cm;  // was previously using the global value
    ورنہ
	s = دلائل->os_oldval.string;
    اگر (*curbuf->b_p_cm == NUL)
	p = p_cm;  // is now using the global value
    ورنہ
	p = curbuf->b_p_cm;
    اگر (STRCMP(s, p) != 0)
	ml_سیٹ_crypt_کلید(curbuf, curbuf->b_p_کلید, s);

    // If the global value changes need to update the swapfile for all
    // buffers using that value.
    اگر ((دلائل->os_flags & OPT_GLOBAL)
	    && STRCMP(p_cm, دلائل->os_oldval.string) != 0)
    {
	buf_T	*buf;

	FOR_ALL_BUFFERS(buf)
	    اگر (buf != curbuf && *buf->b_p_cm == NUL)
		ml_سیٹ_crypt_کلید(buf, buf->b_p_کلید, دلائل->os_oldval.string);
    }
    واپس تہی;
}

    صحیح
expand_سیٹ_cryptmethod(optexpand_T *دلائل, صحیح *عدد_Matches, char_u ***matches)
{
    واپس expand_سیٹ_opt_string(
	    دلائل,
	    p_cm_اقدار,
	    ARRAY_لمبائی(p_cm_اقدار) - 1,
	    عدد_Matches,
	    matches);
}
#اگر_ختم

#اگر defined(FEAT_CSCOPE) && defined(FEAT_QUICKFIX)
/*
 * The 'cscopequickfix' option is changed.
 */
    حرف *
did_سیٹ_cscopequickfix(optset_T *دلائل غیر_مستعمل)
{
    char_u	*p;

    اگر (p_csqf == تہی)
	واپس تہی;

    p = p_csqf;
    جب (*p != NUL)
    {
	اگر (vim_strchr((char_u *)CSQF_CMDS, *p) == تہی
		|| p[1] == NUL
		|| vim_strchr((char_u *)CSQF_FLAGS, p[1]) == تہی
		|| (p[2] != NUL && p[2] != ','))
	    واپس e_invalid_دلیل;
	ورنہ اگر (p[2] == NUL)
	    توڑو;
	ورنہ
	    p += 3;
    }

    واپس تہی;
}
#اگر_ختم

#اگر defined(FEAT_SYN_HL)
/*
 * The 'cursorlineopt' option is changed.
 */
    حرف *
did_سیٹ_cursorlineopt(optset_T *دلائل)
{
    char_u	**varp = (char_u **)دلائل->os_varp;

    // This could be changed to use opt_strings_flags() instead.
    اگر (**varp == NUL || fill_culopt_flags(*varp, curwin) != OK)
	واپس e_invalid_دلیل;

    واپس تہی;
}

    صحیح
expand_سیٹ_cursorlineopt(optexpand_T *دلائل, صحیح *عدد_Matches, char_u ***matches)
{
    واپس expand_سیٹ_opt_string(
	    دلائل,
	    p_culopt_اقدار,
	    ARRAY_لمبائی(p_culopt_اقدار) - 1,
	    عدد_Matches,
	    matches);
}
#اگر_ختم

/*
 * The 'debug' option is changed.
 */
    حرف *
did_سیٹ_debug(optset_T *دلائل غیر_مستعمل)
{
    واپس did_سیٹ_opt_لڑیاں(p_debug, p_debug_اقدار, TRUE);
}

    صحیح
expand_سیٹ_debug(optexpand_T *دلائل, صحیح *عدد_Matches, char_u ***matches)
{
    واپس expand_سیٹ_opt_string(
	    دلائل,
	    p_debug_اقدار,
	    ARRAY_لمبائی(p_debug_اقدار) - 1,
	    عدد_Matches,
	    matches);
}

#اگر defined(FEAT_DIFF)
/*
 * The 'diffanchors' option is changed.
 */
    حرف *
did_سیٹ_diffanchors(optset_T *دلائل)
{
    اگر (diffanchors_بدلا(دلائل->os_flags & OPT_LOCAL) == FAIL)
	واپس e_invalid_دلیل;

    واپس تہی;
}

/*
 * The 'diffopt' option is changed.
 */
    حرف *
did_سیٹ_diffopt(optset_T *دلائل غیر_مستعمل)
{
    اگر (diffopt_بدلا() == FAIL)
	واپس e_invalid_دلیل;

    واپس تہی;
}

    صحیح
expand_سیٹ_diffopt(optexpand_T *دلائل, صحیح *عدد_Matches, char_u ***matches)
{
    expand_T *xp = دلائل->oe_xp;

    اگر (xp->xp_نمونہ > دلائل->oe_سیٹ_arg && *(xp->xp_نمونہ-1) == ':')
    {
	اگر (completing_قدر_for_subopt(دلائل, "algorithm"))
	{
	    واپس expand_سیٹ_opt_string(
		    دلائل,
		    p_dip_algorithm_اقدار,
		    ARRAY_لمبائی(p_dip_algorithm_اقدار) - 1,
		    عدد_Matches,
		    matches);
	}
	اگر (completing_قدر_for_subopt(دلائل, "inline"))
	{
	    واپس expand_سیٹ_opt_string(
		    دلائل,
		    p_dip_inline_اقدار,
		    ARRAY_لمبائی(p_dip_inline_اقدار) - 1,
		    عدد_Matches,
		    matches);
	}
	واپس FAIL;
    }

    واپس expand_سیٹ_opt_string(
	    دلائل,
	    p_dip_اقدار,
	    ARRAY_لمبائی(p_dip_اقدار) - 1,
	    عدد_Matches,
	    matches);
}
#اگر_ختم

/*
 * The 'display' option is changed.
 */
    حرف *
did_سیٹ_display(optset_T *دلائل غیر_مستعمل)
{
    اگر (opt_لڑیاں_flags(p_dy, p_dy_اقدار, &dy_flags, TRUE) != OK)
	واپس e_invalid_دلیل;

    (خالی)init_chartab();
    واپس تہی;
}

    صحیح
expand_سیٹ_display(optexpand_T *دلائل, صحیح *عدد_Matches, char_u ***matches)
{
    واپس expand_سیٹ_opt_string(
	    دلائل,
	    p_dy_اقدار,
	    ARRAY_لمبائی(p_dy_اقدار) - 1,
	    عدد_Matches,
	    matches);
}

/*
 * The 'eadirection' option is changed.
 */
    حرف *
did_سیٹ_eadirection(optset_T *دلائل غیر_مستعمل)
{
    واپس did_سیٹ_opt_لڑیاں(p_ead, p_ead_اقدار, FALSE);
}

    صحیح
expand_سیٹ_eadirection(optexpand_T *دلائل, صحیح *عدد_Matches, char_u ***matches)
{
    واپس expand_سیٹ_opt_string(
	    دلائل,
	    p_ead_اقدار,
	    ARRAY_لمبائی(p_ead_اقدار) - 1,
	    عدد_Matches,
	    matches);
}

/*
 * One of the 'encoding', 'fileencoding', 'termencoding' or 'makeencoding'
 * options is changed.
 */
    حرف *
did_سیٹ_encoding(optset_T *دلائل)
{
    char_u	**varp = (char_u **)دلائل->os_varp;
    char_u	**gvarp;
    حرف	*errmsg = تہی;
    char_u	*p;

    // Get the global option to compare with, otherwise we would have to check
    // two values for all local options.
    gvarp = (char_u **)get_option_varp_scope(دلائل->os_idx, OPT_GLOBAL);

    اگر (gvarp == &p_fenc)
    {
	اگر (!curbuf->b_p_ma && دلائل->os_flags != OPT_GLOBAL)
	    errmsg = e_cannot_make_changes_modifiable_is_off;
	ورنہ اگر (vim_strchr(*varp, ',') != تہی)
	    // No comma allowed in 'fileencoding'; catches confusing it
	    // with 'fileencodings'.
	    errmsg = e_invalid_دلیل;
	ورنہ
	{
	    // May show a "+" in the title now.
	    redraw_titles();
	    // Add 'fileencoding' to the swap file.
	    ml_setflags(curbuf);
	}
    }
    اگر (errmsg == تہی)
    {
	// canonize the value, so that STRCMP() can be used on it
	p = enc_canonize(*varp);
	اگر (p != تہی)
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
    اگر (errmsg == تہی && varp == &p_tenc && gui.اندر_use)
    {
	// GTK uses only a single encoding, and that is UTF-8.
	اگر (STRCMP(p_tenc, "utf-8") != 0)
	    errmsg = e_cannot_be_بدلا_اندر_gtk_GUI;
    }
#اگر_ختم

    اگر (errmsg == تہی)
    {
#اگر_تعریف_شدہ FEAT_KEYMAP
	// When 'keymap' is used and 'encoding' changes, reload the keymap
	// (with another encoding).
	اگر (varp == &p_enc && *curbuf->b_p_keymap != NUL)
	    (خالی)keymap_init();
#اگر_ختم

	// When 'termencoding' is not empty and 'encoding' changes or when
	// 'termencoding' changes, need to setup for keyboard input and
	// display output conversion.
	اگر (((varp == &p_enc && *p_tenc != NUL) || varp == &p_tenc))
	{
	    اگر (convert_تیاری(&داخلہ_conv, p_tenc, p_enc) == FAIL
		    || convert_تیاری(&output_conv, p_enc, p_tenc) == FAIL)
	    {
		semsg(_(e_cannot_convert_between_str_and_str),
			p_tenc, p_enc);
		errmsg = e_invalid_دلیل;
	    }
	}

#اگر defined(MSWIN)
	// $HOME, $VIM and $VIMRUNTIME may have characters in active code page.
	اگر (varp == &p_enc)
	{
	    init_homedir();
	    init_vimdir();
	}
#اگر_ختم
    }

    واپس errmsg;
}

    صحیح
expand_سیٹ_encoding(optexpand_T *دلائل, صحیح *عدد_Matches, char_u ***matches)
{
    واپس expand_سیٹ_opt_generic(
	    دلائل,
	    get_encoding_name,
	    عدد_Matches,
	    matches);
}

/*
 * The 'eventignore(win)' option is changed.
 */
    حرف *
did_سیٹ_eventignore(optset_T *دلائل)
{
    char_u	**varp = (char_u **)دلائل->os_varp;
    char_u	*oldval = دلائل->os_oldval.string;

    اگر (check_ei(*varp) == FAIL)
	واپس e_invalid_دلیل;

    اگر (oldval == تہی || STRCMP(oldval, *varp) == 0)
	واپس تہی;

    // Deal with the events that are triggered by comparing against a stored
    // state, with the old value in effect: what happened while an event was
    // ignored must not be reported once it is not ignored anymore, and what
    // happened before must still be reported.
    // Use a copy, the caller owns "oldval" and autocommands may free it.
    char_u	*save_ei = vim_strsave(oldval);
    اگر (save_ei != تہی)
    {
	char_u	*newval = *varp;
	win_T	*wp = is_کھڑکی_local_option(دلائل->os_idx) ? curwin : تہی;

	*varp = save_ei;
	// "varp" points into "wp" for 'eventignorewin'.
	اگر (wp != تہی)
	    ++wp->w_locked;
	may_trigger_deferred_events();
	اگر (wp != تہی)
	    --wp->w_locked;

	free_string_option(*varp);
	*varp = newval;
    }

    واپس تہی;
}

ساکن صحیح expand_eiw = FALSE;

    ساکن char_u *
get_eventignore_name(expand_T *xp, صحیح idx)
{
    صحیح تفریق = *xp->xp_نمونہ == '-';
    // 'eventignore(win)' allows special keyword "all" in addition to
    // all event names.
    اگر (!تفریق && idx == 0)
	واپس (char_u *)"all";

    char_u *name = get_event_name_no_گروہ(xp, idx - 1 + تفریق, expand_eiw);
    اگر (name == تہی)
	واپس تہی;

    sprintf((حرف *)IObuff, "%s%s", تفریق ? "-" : "", name);
    واپس IObuff;
}

    صحیح
expand_سیٹ_eventignore(optexpand_T *دلائل, صحیح *عدد_Matches, char_u ***matches)
{
    expand_eiw = دلائل->oe_varp != (char_u *)&p_ei;
    واپس expand_سیٹ_opt_generic(
	    دلائل,
	    get_eventignore_name,
	    عدد_Matches,
	    matches);
}

/*
 * The 'fileformat' option is changed.
 */
    حرف *
did_سیٹ_fileformat(optset_T *دلائل)
{
    char_u	**varp = (char_u **)دلائل->os_varp;

    اگر (!curbuf->b_p_ma && !(دلائل->os_flags & OPT_GLOBAL))
	واپس e_cannot_make_changes_modifiable_is_off;
    ورنہ اگر (check_opt_لڑیاں(*varp, p_ff_اقدار, FALSE) != OK)
	واپس e_invalid_دلیل;

    // may also change 'textmode'
    اگر (get_fileformat(curbuf) == EOL_DOS)
	curbuf->b_p_tx = TRUE;
    ورنہ
	curbuf->b_p_tx = FALSE;
    redraw_titles();
    // update flag in swap file
    ml_setflags(curbuf);
    // Redraw needed when switching to/from "mac": a CR in the text
    // will be displayed differently.
    اگر (get_fileformat(curbuf) == EOL_MAC || *دلائل->os_oldval.string == 'm')
	redraw_curbuf_later(UPD_NOT_VALID);

    واپس تہی;
}

    صحیح
expand_سیٹ_fileformat(optexpand_T *دلائل, صحیح *عدد_Matches, char_u ***matches)
{
    واپس expand_سیٹ_opt_string(
	    دلائل,
	    p_ff_اقدار,
	    ARRAY_لمبائی(p_ff_اقدار) - 1,
	    عدد_Matches,
	    matches);
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * fileformat options.
 */
    char_u *
get_fileformat_name(expand_T *xp غیر_مستعمل, صحیح idx)
{
    اگر (idx >= (صحیح)ARRAY_لمبائی(p_ff_اقدار))
	واپس تہی;

    واپس (char_u*)p_ff_اقدار[idx];
}

/*
 * The 'fileformats' option is changed.
 */
    حرف *
did_سیٹ_fileformats(optset_T *دلائل غیر_مستعمل)
{
    اگر (check_opt_لڑیاں(p_ffs, p_ff_اقدار, TRUE) != OK)
	واپس e_invalid_دلیل;

    // also change 'textauto'
    اگر (*p_ffs == NUL)
	p_ta = FALSE;
    ورنہ
	p_ta = TRUE;

    واپس تہی;
}

/*
 * The 'filetype' or the 'syntax' option is changed.
 */
    حرف *
did_سیٹ_filetype_or_syntax(optset_T *دلائل)
{
    char_u	**varp = (char_u **)دلائل->os_varp;

    اگر (!valid_filetype(*varp))
	واپس e_invalid_دلیل;

    دلائل->os_قدر_بدلا = STRCMP(دلائل->os_oldval.string, *varp) != 0;

    // Since we check the value, there is no need to set P_INSECURE,
    // even when the value comes from a modeline.
    دلائل->os_قدر_checked = TRUE;

    واپس تہی;
}

#اگر defined(FEAT_FOLDING)
/*
 * The 'foldclose' option is changed.
 */
    حرف *
did_سیٹ_foldclose(optset_T *دلائل غیر_مستعمل)
{
    واپس did_سیٹ_opt_لڑیاں(p_fcl, p_fcl_اقدار, TRUE);
}

    صحیح
expand_سیٹ_foldclose(optexpand_T *دلائل, صحیح *عدد_Matches, char_u ***matches)
{
    واپس expand_سیٹ_opt_string(
	    دلائل,
	    p_fcl_اقدار,
	    ARRAY_لمبائی(p_fcl_اقدار) - 1,
	    عدد_Matches,
	    matches);
}
#اگر_ختم

#اگر defined(FEAT_EVAL) && defined(FEAT_FOLDING)
/*
 * The 'foldexpr' option is changed.
 */
    حرف *
did_سیٹ_foldexpr(optset_T *دلائل)
{
    (خالی)did_سیٹ_optexpr(دلائل);
    اگر (foldmethod_Is_اظہاریہ(curwin))
	foldUpdateAll(curwin);
    واپس تہی;
}
#اگر_ختم

#اگر defined(FEAT_FOLDING)
/*
 * The 'foldignore' option is changed.
 */
    حرف *
did_سیٹ_foldignore(optset_T *دلائل غیر_مستعمل)
{
    اگر (foldmethodIsIndent(curwin))
	foldUpdateAll(curwin);
    واپس تہی;
}

/*
 * The 'foldmarker' option is changed.
 */
    حرف *
did_سیٹ_foldmarker(optset_T *دلائل)
{
    char_u	**varp = (char_u **)دلائل->os_varp;
    char_u	*p;

    p = vim_strchr(*varp, ',');
    اگر (p == تہی)
	واپس e_comma_required;
    ورنہ اگر (p == *varp || p[1] == NUL)
	واپس e_invalid_دلیل;
    ورنہ اگر (foldmethodIsMarker(curwin))
	foldUpdateAll(curwin);

    واپس تہی;
}

/*
 * The 'foldmethod' option is changed.
 */
    حرف *
did_سیٹ_foldmethod(optset_T *دلائل)
{
    char_u	**varp = (char_u **)دلائل->os_varp;

    اگر (check_opt_لڑیاں(*varp, p_fdm_اقدار, FALSE) != OK || **varp == NUL)
	واپس e_invalid_دلیل;

    foldUpdateAll(curwin);
    اگر (foldmethodIsDiff(curwin))
	newFoldLevel();
    واپس تہی;
}

    صحیح
expand_سیٹ_foldmethod(optexpand_T *دلائل, صحیح *عدد_Matches, char_u ***matches)
{
    واپس expand_سیٹ_opt_string(
	    دلائل,
	    p_fdm_اقدار,
	    ARRAY_لمبائی(p_fdm_اقدار) - 1,
	    عدد_Matches,
	    matches);
}

/*
 * The 'foldopen' option is changed.
 */
    حرف *
did_سیٹ_foldopen(optset_T *دلائل غیر_مستعمل)
{
    واپس did_سیٹ_opt_flags(p_fdo, p_fdo_اقدار, &fdo_flags, TRUE);
}

    صحیح
expand_سیٹ_foldopen(optexpand_T *دلائل, صحیح *عدد_Matches, char_u ***matches)
{
    واپس expand_سیٹ_opt_string(
	    دلائل,
	    p_fdo_اقدار,
	    ARRAY_لمبائی(p_fdo_اقدار) - 1,
	    عدد_Matches,
	    matches);
}
#اگر_ختم

/*
 * The 'formatoptions' option is changed.
 */
    حرف *
did_سیٹ_formatoptions(optset_T *دلائل)
{
    char_u	**varp = (char_u **)دلائل->os_varp;

    واپس did_سیٹ_option_listflag(*varp, (char_u *)FO_ALL, دلائل->os_errbuf,
		    دلائل->os_errbuflen);
}

    صحیح
expand_سیٹ_formatoptions(optexpand_T *دلائل, صحیح *عدد_Matches, char_u ***matches)
{
    واپس expand_سیٹ_opt_listflag(دلائل, (char_u*)FO_ALL, عدد_Matches, matches);
}

#اگر defined(CURSOR_SHAPE)
/*
 * The 'guicursor' option is changed.
 */
    حرف *
did_سیٹ_guicursor(optset_T *دلائل غیر_مستعمل)
{
    واپس parse_shape_opt(SHAPE_CURSOR);
}
#اگر_ختم

#اگر defined(FEAT_GUI)
/*
 * The 'guifont' option is changed.
 */
    حرف *
did_سیٹ_guifont(optset_T *دلائل غیر_مستعمل)
{
    char_u	*p;
    حرف	*errmsg = تہی;

    اگر (gui.اندر_use)
    {
	p = p_guifont;
# اگر defined(FEAT_GUI_GTK)
	// Put up a font dialog and let the user select a new value.
	// If this is cancelled go back to the old value but don't
	// give an error message.
	اگر (STRCMP(p, "*") == 0)
	{
	    p = gui_mch_font_dialog(دلائل->os_oldval.string);
	    free_string_option(p_guifont);
	    p_guifont = (p != تہی) ? p : vim_strsave(دلائل->os_oldval.string);
	}
# endif
	اگر (p != تہی && gui_init_font(p_guifont, FALSE) != OK)
	{
# اگر defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_PHOTON)
	    اگر (STRCMP(p_guifont, "*") == 0)
	    {
		// Dialog was cancelled: Keep the old value without giving
		// an error message.
		free_string_option(p_guifont);
		p_guifont = vim_strsave(دلائل->os_oldval.string);
	    }
	    ورنہ
# endif
		errmsg = e_invalid_fonts;
	}
    }

    واپس errmsg;
}

/*
 * Expand the 'guifont' option. Only when GUI is being used. Each platform has
 * specific behaviors.
 */
    صحیح
expand_سیٹ_guifont(optexpand_T *دلائل, صحیح *عدد_Matches, char_u ***matches)
{
    اگر (!gui.اندر_use)
	واپس FAIL;

# اگر defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
    char_u **varp = (char_u **)دلائل->oe_varp;
    صحیح wide = (varp == &p_guifontwide);

    واپس expand_سیٹ_opt_callback(
	    دلائل, gui_mch_expand_font, &wide, عدد_Matches, matches);
# ورنہ
    واپس FAIL;
# endif
}

# اگر defined(FEAT_XFONTSET)
/*
 * The 'guifontset' option is changed.
 */
    حرف *
did_سیٹ_guifontset(optset_T *دلائل غیر_مستعمل)
{
    حرف *errmsg = تہی;

    اگر (STRCMP(p_guifontset, "*") == 0)
	errmsg = e_cant_select_fontset;
    ورنہ اگر (gui.اندر_use && gui_init_font(p_guifontset, TRUE) != OK)
	errmsg = e_invalid_fontset;

    واپس errmsg;
}
# endif

/*
 * The 'guifontwide' option is changed.
 */
    حرف *
did_سیٹ_guifontwide(optset_T *دلائل غیر_مستعمل)
{
    حرف *errmsg = تہی;

    اگر (STRCMP(p_guifontwide, "*") == 0)
	errmsg = e_cant_select_wide_font;
    ورنہ اگر (gui_get_wide_font() == FAIL)
	errmsg = e_invalid_wide_font;

    واپس errmsg;
}
#اگر_ختم

#اگر defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
/*
 * The 'guiligatures' option is changed.
 */
    حرف *
did_سیٹ_guiligatures(optset_T *دلائل غیر_مستعمل)
{
    gui_سیٹ_ligatures();
    واپس تہی;
}
#اگر_ختم

#اگر defined(FEAT_GUI)
/*
 * The 'guioptions' option is changed.
 */
    حرف *
did_سیٹ_guioptions(optset_T *دلائل)
{
    char_u	**varp = (char_u **)دلائل->os_varp;
    حرف *errmsg;

    errmsg = did_سیٹ_option_listflag(*varp, (char_u *)GO_ALL, دلائل->os_errbuf,
		    دلائل->os_errbuflen);
    اگر (errmsg != تہی)
	واپس errmsg;

    gui_init_which_components(دلائل->os_oldval.string);
    واپس تہی;
}

    صحیح
expand_سیٹ_guioptions(optexpand_T *دلائل, صحیح *عدد_Matches, char_u ***matches)
{
    واپس expand_سیٹ_opt_listflag(دلائل, (char_u*)GO_ALL, عدد_Matches, matches);
}
#اگر_ختم

#اگر defined(FEAT_GUI_TABLINE)
/*
 * The 'guitablabel' option is changed.
 */
    حرف *
did_سیٹ_guitablabel(optset_T *دلائل غیر_مستعمل)
{
    redraw_tabline = TRUE;
    واپس تہی;
}
#اگر_ختم

/*
 * The 'helpfile' option is changed.
 */
    حرف *
did_سیٹ_helpfile(optset_T *دلائل غیر_مستعمل)
{
    // May compute new values for $VIM and $VIMRUNTIME
    اگر (didset_vim)
	vim_unsetenv_ext((char_u *)"VIM");
    اگر (didset_vimruntime)
	vim_unsetenv_ext((char_u *)"VIMRUNTIME");
    واپس تہی;
}

#اگر defined(FEAT_MULTI_LANG)
/*
 * The 'helplang' option is changed.
 */
    حرف *
did_سیٹ_helplang(optset_T *دلائل غیر_مستعمل)
{
    حرف *errmsg = تہی;

    // Check for "", "ab", "ab,cd", etc.
    برائے (char_u *s = p_hlg; *s != NUL; s += 3)
    {
	اگر (s[1] == NUL || ((s[2] != ',' || s[3] == NUL) && s[2] != NUL))
	{
	    errmsg = e_invalid_دلیل;
	    توڑو;
	}
	اگر (s[2] == NUL)
	    توڑو;
    }

    واپس errmsg;
}
#اگر_ختم

/*
 * The 'highlight' option is changed.
 */
    حرف *
did_سیٹ_highlight(optset_T *دلائل غیر_مستعمل)
{
    اگر (highlight_بدلا() == FAIL)
	واپس e_invalid_دلیل;	// invalid flags

    واپس تہی;
}

    ساکن صحیح
expand_hl_occasions(
	optexpand_T *دلائل,
	صحیح *عدد_Matches,
	char_u ***matches,
	char_u سابقہ)
{
    char_u	    *p;
    ساکن char_u   hl_flags[HLF_گنتی] = HL_FLAGS;
    size_t	    i;
    صحیح		    گنتی = 0;

    *matches = ALLOC_MULT(char_u *, HLF_گنتی + 1);
    اگر (*matches == تہی)
	واپس FAIL;

    // We still want to return the full option if it's requested.
    اگر (دلائل->oe_include_orig_val)
    {
	p = vim_strsave(دلائل->oe_opt_قدر);
	اگر (p == تہی)
	{
	    VIM_CLEAR(*matches);
	    واپس FAIL;
	}
	(*matches)[گنتی++] = p;
    }

    برائے (i = 0; i < HLF_گنتی; i++)
    {
	p = alloc((سابقہ == NUL ? 1 : 2) + 1);
	اگر (p == تہی)
	{
	    اگر (گنتی == 0)
	    {
		VIM_CLEAR(*matches);
		واپس FAIL;
	    }
	    ورنہ
		توڑو;
	}
	اگر (سابقہ == NUL)
	    sprintf((حرف *)p, "%c", hl_flags[i]);
	ورنہ
	    sprintf((حرف *)p, "%c%c", سابقہ, hl_flags[i]);
	(*matches)[گنتی++] = p;
    }

    اگر (گنتی == 0)
    {
	VIM_CLEAR(*matches);
	واپس FAIL;
    }
    *عدد_Matches = گنتی;
    واپس OK;
}

/*
 * Expand 'highlight' option.
 */
    صحیح
expand_سیٹ_highlight(optexpand_T *دلائل, صحیح *عدد_Matches, char_u ***matches)
{
    char_u	    *p;
    expand_T	    *xp = دلائل->oe_xp;
    size_t	    i;
    صحیح		    گنتی = 0;

    اگر (xp->xp_نمونہ > دلائل->oe_سیٹ_arg && *(xp->xp_نمونہ-1) == ':')
    {
	// Right after a ':', meaning we just return all highlight names.
	واپس expand_سیٹ_opt_generic(
		دلائل,
		get_highlight_name,
		عدد_Matches,
		matches);
    }

    اگر (*xp->xp_نمونہ == NUL)
	// At beginning of a comma-separated list. Return the specific list of
	// supported occasions.
	واپس expand_hl_occasions(دلائل, عدد_Matches, matches, NUL);

    // We are after the initial character (which indicates the occasion). We
    // already made sure we are not matching after a ':' above, so now we want
    // to match against display mode modifiers.
    // Since the xp_pattern starts from the beginning, we need to include it in
    // the returned match.

    // Note: Keep this in sync with highlight_changed()
    ساکن char_u p_hl_منوال_اقدار[] =
	{':', 'b', 'i', '-', 'n', 'r', 's', 'u', 'c', '2', 'd', '=', 't'};
    size_t عدد_hl_modes = ARRAY_لمبائی(p_hl_منوال_اقدار);

    *matches = ALLOC_MULT(char_u *, عدد_hl_modes);
    اگر (*matches == تہی)
	واپس FAIL;

    صحیح نمونہ_len = xp->xp_نمونہ_len;

    برائے (i = 0; i < عدد_hl_modes; i++)
    {
	// Don't allow duplicates as these are unique flags
	char_u *dup = vim_strchr(xp->xp_نمونہ + 1, p_hl_منوال_اقدار[i]);
	اگر (dup != تہی && (صحیح)(dup - xp->xp_نمونہ) < نمونہ_len)
	    جاری;

	// ':' only works by itself, not with other flags.
	اگر (نمونہ_len > 1 && p_hl_منوال_اقدار[i] == ':')
	    جاری;

	p = vim_strnsave(xp->xp_نمونہ, نمونہ_len + 1);
	اگر (p == تہی)
	{
	    اگر (i == 0)
	    {
		VIM_CLEAR(*matches);
		واپس FAIL;
	    }
	    ورنہ
		توڑو;
	}
	p[نمونہ_len] = p_hl_منوال_اقدار[i];
	p[نمونہ_len + 1] = NUL;
	(*matches)[گنتی++] = p;
    }
    اگر (گنتی == 0)
    {
	VIM_CLEAR(*matches);
	واپس FAIL;
    }
    *عدد_Matches = گنتی;

    واپس OK;
}

/*
 * The 'titlestring' or the 'iconstring' option is changed.
 */
    ساکن حرف *
parse_titleiconstring(optset_T *دلائل غیر_مستعمل, صحیح flagval غیر_مستعمل)
{
#اگر_تعریف_شدہ FEAT_STL_OPT
    char_u	**varp = (char_u **)دلائل->os_varp;

    // NULL => statusline syntax
    اگر (vim_strchr(*varp, '%') && check_stl_option(*varp) == تہی)
	stl_syntax |= flagval;
    ورنہ
	stl_syntax &= ~flagval;
#اگر_ختم
    did_سیٹ_title();

    واپس تہی;
}

/*
 * The 'iconstring' option is changed.
 */
    حرف *
did_سیٹ_iconstring(optset_T *دلائل)
{
    صحیح flagval = 0;

#اگر_تعریف_شدہ FEAT_STL_OPT
    flagval = STL_اندر_ICON;
#اگر_ختم

    واپس parse_titleiconstring(دلائل, flagval);
}

#اگر defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
/*
 * The 'imactivatekey' option is changed.
 */
    حرف *
did_سیٹ_imactivatekey(optset_T *دلائل غیر_مستعمل)
{
    اگر (!im_xim_isvalid_imactivate())
	واپس e_invalid_دلیل;
    واپس تہی;
}
#اگر_ختم

#اگر_تعریف_شدہ FEAT_IMAGE
/*
 * The 'imageprotocol' option is changed.
 */
    حرف *
did_سیٹ_imageprotocol(optset_T *دلائل غیر_مستعمل)
{
    اگر (update_image_backend() == FAIL)
	واپس e_invalid_دلیل;
    واپس تہی;
}

    صحیح
expand_سیٹ_imageprotocol(optexpand_T *دلائل, صحیح *عدد_Matches, char_u ***matches)
{
    expand_T *xp = دلائل->oe_xp;

    اگر (xp->xp_نمونہ > دلائل->oe_سیٹ_arg && *(xp->xp_نمونہ-1) == ':')
    {
	// 'imageprotocol' only has well-defined terms for completion for the
	// protocol part after the colon.
	واپس expand_سیٹ_opt_string(
		دلائل,
		p_ipc_protocol_اقدار,
		ARRAY_لمبائی(p_ipc_protocol_اقدار) - 1,
		عدد_Matches,
		matches);
    }
    // Use expand_set_opt_string instead of returning FAIL so that we can
    // include the original value if args->oe_include_orig_val is set.
    ساکن حرف *(خالی_خانے_2[]) = {تہی};
    واپس expand_سیٹ_opt_string(دلائل, خالی_خانے_2, 0, عدد_Matches, matches);
}
#اگر_ختم

/*
 * The 'iskeyword' option is changed.
 */
    حرف *
did_سیٹ_iskeyword(optset_T *دلائل)
{
    char_u	**varp = (char_u **)دلائل->os_varp;

    اگر (varp == &p_isk)		// only check for global-value
    {
	اگر (check_isopt(*varp) == FAIL)
	    واپس e_invalid_دلیل;
    }
    ورنہ			// fallthrough for local-value
	واپس did_سیٹ_isopt(دلائل);

    واپس تہی;
}

/*
 * The 'isident' or the 'iskeyword' or the 'isprint' or the 'isfname' option is
 * changed.
 */
    حرف *
did_سیٹ_isopt(optset_T *دلائل)
{
    // 'isident', 'iskeyword', 'isprint' or 'isfname' option: refill g_chartab[]
    // If the new option is invalid, use old value.
    // 'lisp' option: refill g_chartab[] for '-' char.
    اگر (init_chartab() == FAIL)
    {
	دلائل->os_restore_chartab = TRUE;// need to restore the chartab.
	واپس e_invalid_دلیل;	// error in value
    }

    واپس تہی;
}

/*
 * The 'jumpoptions' option is changed.
 */
    حرف *
did_سیٹ_jumpoptions(optset_T *دلائل غیر_مستعمل)
{
    اگر (opt_لڑیاں_flags(p_jop, p_jop_اقدار, &jop_flags, TRUE) != OK)
	واپس e_invalid_دلیل;

    واپس تہی;
}

    صحیح
expand_سیٹ_jumpoptions(optexpand_T *دلائل, صحیح *عدد_Matches, char_u ***matches)
{
    واپس expand_سیٹ_opt_string(
	    دلائل,
	    p_jop_اقدار,
	    ARRAY_لمبائی(p_jop_اقدار) - 1,
	    عدد_Matches,
	    matches);
}

#اگر defined(FEAT_KEYMAP)
/*
 * The 'keymap' option is changed.
 */
    حرف *
did_سیٹ_keymap(optset_T *دلائل)
{
    char_u	**varp = (char_u **)دلائل->os_varp;
    حرف *errmsg = تہی;

    اگر (!valid_filetype(*varp))
	errmsg = e_invalid_دلیل;
    ورنہ
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
	دلائل->os_قدر_checked = TRUE;
    }

    اگر (errmsg == تہی)
    {
	اگر (*curbuf->b_p_keymap != NUL)
	{
	    // Installed a new keymap, switch on using it.
	    curbuf->b_p_iminsert = B_IMODE_LMAP;
	    اگر (curbuf->b_p_imsearch != B_IMODE_USE_داخل_کرو)
		curbuf->b_p_imsearch = B_IMODE_LMAP;
	}
	ورنہ
	{
	    // Cleared the keymap, may reset 'iminsert' and 'imsearch'.
	    اگر (curbuf->b_p_iminsert == B_IMODE_LMAP)
		curbuf->b_p_iminsert = B_IMODE_NONE;
	    اگر (curbuf->b_p_imsearch == B_IMODE_LMAP)
		curbuf->b_p_imsearch = B_IMODE_USE_داخل_کرو;
	}
	اگر ((دلائل->os_flags & OPT_LOCAL) == 0)
	{
	    سیٹ_iminsert_global();
	    سیٹ_imsearch_global();
	}
	حالت_redraw_curbuf();
    }

    واپس errmsg;
}
#اگر_ختم

/*
 * The 'keymodel' option is changed.
 */
    حرف *
did_سیٹ_keymodel(optset_T *دلائل غیر_مستعمل)
{
    اگر (check_opt_لڑیاں(p_km, p_km_اقدار, TRUE) != OK)
	واپس e_invalid_دلیل;

    km_stopsel = (vim_strchr(p_km, 'o') != تہی);
    km_startsel = (vim_strchr(p_km, 'a') != تہی);
    واپس تہی;
}

    صحیح
expand_سیٹ_keymodel(optexpand_T *دلائل, صحیح *عدد_Matches, char_u ***matches)
{
    واپس expand_سیٹ_opt_string(
	    دلائل,
	    p_km_اقدار,
	    ARRAY_لمبائی(p_km_اقدار) - 1,
	    عدد_Matches,
	    matches);
}

/*
 * The 'keyprotocol' option is changed.
 */
    حرف *
did_سیٹ_keyprotocol(optset_T *دلائل غیر_مستعمل)
{
    char_u *رکن = T_NAME;
    keyprot_T kpc = مطابقت_keyprotocol(رکن);
    اگر (kpc == KEYPROTOCOL_FAIL)
	واپس e_invalid_دلیل;

    apply_keyprotocol(رکن, kpc);

    واپس تہی;
}

    صحیح
expand_سیٹ_keyprotocol(optexpand_T *دلائل, صحیح *عدد_Matches, char_u ***matches)
{
    expand_T *xp = دلائل->oe_xp;
    اگر (xp->xp_نمونہ > دلائل->oe_سیٹ_arg && *(xp->xp_نمونہ-1) == ':')
    {
	// 'keyprotocol' only has well-defined terms for completion for the
	// protocol part after the colon.
	واپس expand_سیٹ_opt_string(
		دلائل,
		p_kpc_protocol_اقدار,
		ARRAY_لمبائی(p_kpc_protocol_اقدار) - 1,
		عدد_Matches,
		matches);
    }
    // Use expand_set_opt_string instead of returning FAIL so that we can
    // include the original value if args->oe_include_orig_val is set.
    ساکن حرف *(خالی_خانے_2[]) = {تہی};
    واپس expand_سیٹ_opt_string(دلائل, خالی_خانے_2, 0, عدد_Matches, matches);
}

/*
 * The 'lispoptions' option is changed.
 */
    حرف *
did_سیٹ_lispoptions(optset_T *دلائل)
{
    char_u	**varp = (char_u **)دلائل->os_varp;

    اگر (**varp != NUL
	    && STRCMP(*varp, "expr:0") != 0
	    && STRCMP(*varp, "expr:1") != 0)
	واپس e_invalid_دلیل;

    واپس تہی;
}

    صحیح
expand_سیٹ_lispoptions(optexpand_T *دلائل, صحیح *عدد_Matches, char_u ***matches)
{
    ساکن حرف *(p_lop_اقدار[]) = {"expr:0", "expr:1", تہی};
    واپس expand_سیٹ_opt_string(
	    دلائل,
	    p_lop_اقدار,
	    ARRAY_لمبائی(p_lop_اقدار) - 1,
	    عدد_Matches,
	    matches);
}

/*
 * The 'matchpairs' option is changed.
 */
    حرف *
did_سیٹ_matchpairs(optset_T *دلائل)
{
    char_u	**varp = (char_u **)دلائل->os_varp;
    char_u	*p;

    اگر (has_mbyte)
    {
	برائے (p = *varp; *p != NUL; ++p)
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
		واپس e_invalid_دلیل;
	    اگر (*p == NUL)
		توڑو;
	}
    }
    ورنہ
    {
	// Check for "x:y,x:y"
	برائے (p = *varp; *p != NUL; p += 4)
	{
	    اگر (p[1] != ':' || p[2] == NUL || (p[3] != NUL && p[3] != ','))
		واپس e_invalid_دلیل;
	    اگر (p[3] == NUL)
		توڑو;
	}
    }

    واپس تہی;
}

/*
 * Process the updated 'messagesopt' option value.
 */
    حرف *
did_سیٹ_messagesopt(optset_T *دلائل غیر_مستعمل)
{
    اگر (messagesopt_بدلا() == FAIL)
	واپس e_invalid_دلیل;

    واپس تہی;
}

    صحیح
expand_سیٹ_messagesopt(optexpand_T *دلائل, صحیح *عدد_Matches, char_u ***matches)
{
    ساکن حرف *(p_mopt_اقدار[]) = {"hit-enter", "wait:", "history:", تہی};
    واپس expand_سیٹ_opt_string(
	    دلائل,
	    p_mopt_اقدار,
	    ARRAY_لمبائی(p_mopt_اقدار) - 1,
	    عدد_Matches,
	    matches);
}

#اگر defined(FEAT_SPELL)
/*
 * The 'mkspellmem' option is changed.
 */
    حرف *
did_سیٹ_mkspellmem(optset_T *دلائل غیر_مستعمل)
{
    اگر (spell_check_msm() != OK)
	واپس e_invalid_دلیل;

    واپس تہی;
}
#اگر_ختم

/*
 * The 'mouse' option is changed.
 */
    حرف *
did_سیٹ_mouse(optset_T *دلائل)
{
    char_u	**varp = (char_u **)دلائل->os_varp;
    حرف	*retval;

    retval = did_سیٹ_option_listflag(*varp, (char_u *)MOUSE_ALL, دلائل->os_errbuf,
		    دلائل->os_errbuflen);
    اگر (retval == تہی)
    {
	redraw_tabline = TRUE;
	اگر (tabline_اونچائی() > 0)
	    update_screen(UPD_VALID);
#اگر defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
	popup_close_info(); // Close info popup to apply new properties
#اگر_ختم
    }
    واپس retval;
}

    صحیح
expand_سیٹ_mouse(optexpand_T *دلائل, صحیح *عدد_Matches, char_u ***matches)
{
    واپس expand_سیٹ_opt_listflag(دلائل, (char_u*)MOUSE_ALL, عدد_Matches, matches);
}

/*
 * The 'mousemodel' option is changed.
 */
    حرف *
did_سیٹ_mousemodel(optset_T *دلائل غیر_مستعمل)
{
    اگر (check_opt_لڑیاں(p_mousem, p_mousem_اقدار, FALSE) != OK)
	واپس e_invalid_دلیل;
#اگر defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU) && (Xm_نسخہ <= 1002)
    ورنہ اگر (*p_mousem != *دلائل->os_oldval.string)
	// Changed from "extend" to "popup" or "popup_setpos" or vv: need
	// to create or delete the popup menus.
	gui_motif_update_mousemodel(جڑ_menu);
#اگر_ختم

    واپس تہی;
}

    صحیح
expand_سیٹ_mousemodel(optexpand_T *دلائل, صحیح *عدد_Matches, char_u ***matches)
{
    واپس expand_سیٹ_opt_string(
	    دلائل,
	    p_mousem_اقدار,
	    ARRAY_لمبائی(p_mousem_اقدار) - 1,
	    عدد_Matches,
	    matches);
}

#اگر defined(FEAT_MOUSESHAPE)
    حرف *
did_سیٹ_mouseshape(optset_T *دلائل غیر_مستعمل)
{
    حرف *errmsg = تہی;

    errmsg = parse_shape_opt(SHAPE_MOUSE);
    update_mouseshape(-1);

    واپس errmsg;
}
#اگر_ختم

/*
 * The 'nrformats' option is changed.
 */
    حرف *
did_سیٹ_nrformats(optset_T *دلائل)
{
    char_u	**varp = (char_u **)دلائل->os_varp;

    واپس did_سیٹ_opt_لڑیاں(*varp, p_nf_اقدار, TRUE);
}

    صحیح
expand_سیٹ_nrformats(optexpand_T *دلائل, صحیح *عدد_Matches, char_u ***matches)
{
    واپس expand_سیٹ_opt_string(
	    دلائل,
	    p_nf_اقدار,
	    ARRAY_لمبائی(p_nf_اقدار) - 1,
	    عدد_Matches,
	    matches);
}

#اگر defined(FEAT_EVAL)
/*
 * One of the '*expr' options is changed: 'balloonexpr', 'diffexpr',
 * 'foldexpr', 'foldtext', 'formatexpr', 'includeexpr', 'indentexpr',
 * 'patchexpr', 'printexpr' and 'charconvert'.
 */
    حرف *
did_سیٹ_optexpr(optset_T *دلائل)
{
    char_u	**varp = (char_u **)دلائل->os_varp;

    // If the option value starts with <SID> or s:, then replace that with
    // the script identifier.
    char_u *name = get_scriptlocal_funcname(*varp);
    اگر (name != تہی)
    {
	free_string_option(*varp);
	*varp = name;
    }

    واپس تہی;
}
#اگر_ختم

/*
 * The 'pastetoggle' option is changed.
 */
    حرف *
did_سیٹ_pastetoggle(optset_T *دلائل غیر_مستعمل)
{
    char_u	*p;

    // translate key codes like in a mapping
    اگر (*p_pt)
    {
	(خالی)replace_termcodes(p_pt, &p, 0,
		REPTERM_FROM_PART | REPTERM_DO_LT, تہی);
	اگر (p != تہی)
	{
	    free_string_option(p_pt);
	    p_pt = p;
	}
    }

    واپس تہی;
}

#اگر defined(FEAT_PROP_POPUP)
/*
 * The 'previewpopup' option is changed.
 */
    حرف *
did_سیٹ_previewpopup(optset_T *دلائل غیر_مستعمل)
{
    اگر (parse_previewpopup(تہی) == FAIL)
	واپس e_invalid_دلیل;

# اگر defined(FEAT_QUICKFIX)
    popup_close_info();
# endif
    واپس تہی;
}

    ساکن صحیح
expand_سیٹ_popupoption(optexpand_T *دلائل, صحیح *عدد_Matches, char_u ***matches,
	صحیح previewpopup)
{
    expand_T *xp = دلائل->oe_xp;

    اگر (xp->xp_نمونہ > دلائل->oe_سیٹ_arg && *(xp->xp_نمونہ-1) == ':')
    {
	اگر (completing_قدر_for_subopt(دلائل, "close")
		|| completing_قدر_for_subopt(دلائل, "resize")
		|| completing_قدر_for_subopt(دلائل, "shadow"))
	{
	    واپس expand_سیٹ_opt_string(
		    دلائل,
		    p_popup_option_on_off_اقدار,
		    ARRAY_لمبائی(p_popup_option_on_off_اقدار) - 1,
		    عدد_Matches,
		    matches);
	}
	اگر (completing_قدر_for_subopt(دلائل, "border"))
	{
	    واپس expand_سیٹ_opt_string(
		    دلائل,
		    previewpopup ? p_popup_pvp_border_اقدار
			: p_popup_cpp_border_اقدار,
		    (previewpopup ? ARRAY_لمبائی(p_popup_pvp_border_اقدار) - 1
			: ARRAY_لمبائی(p_popup_cpp_border_اقدار)) - 1,
		    عدد_Matches,
		    matches);
	}
	اگر (completing_قدر_for_subopt(دلائل, "align"))
	{
	    واپس expand_سیٹ_opt_string(
		    دلائل,
		    p_popup_option_align_اقدار,
		    ARRAY_لمبائی(p_popup_option_align_اقدار) - 1,
		    عدد_Matches,
		    matches);
	}
	اگر (completing_قدر_for_subopt(دلائل, "highlight")
		|| completing_قدر_for_subopt(دلائل, "borderhighlight"))
	{
	    // Return the list of all highlight names
	    واپس expand_سیٹ_opt_generic(
		    دلائل,
		    get_highlight_name,
		    عدد_Matches,
		    matches);
	}
	واپس FAIL;
    }

    واپس expand_سیٹ_opt_string(
	    دلائل,
	    previewpopup ? p_popup_pvp_option_اقدار
		: p_popup_cpp_option_اقدار,
	    previewpopup ? ARRAY_لمبائی(p_popup_pvp_option_اقدار) - 1
		: ARRAY_لمبائی(p_popup_cpp_option_اقدار) - 1,
	    عدد_Matches,
	    matches);
}

    صحیح
expand_سیٹ_previewpopup(optexpand_T *دلائل, صحیح *عدد_Matches, char_u ***matches)
{
    واپس expand_سیٹ_popupoption(دلائل, عدد_Matches, matches, TRUE);
}

    صحیح
expand_سیٹ_completepopup(optexpand_T *دلائل, صحیح *عدد_Matches, char_u ***matches)
{
    واپس expand_سیٹ_popupoption(دلائل, عدد_Matches, matches, FALSE);
}
#اگر_ختم

#اگر defined(FEAT_POSTSCRIPT)
/*
 * The 'printencoding' option is changed.
 */
    حرف *
did_سیٹ_printencoding(optset_T *دلائل غیر_مستعمل)
{
    char_u	*s, *p;

    // Canonize 'printencoding' if VIM standard one
    p = enc_canonize(p_penc);
    اگر (p != تہی)
    {
	vim_free(p_penc);
	p_penc = p;
    }
    ورنہ
    {
	// Ensure lower case and '-' for '_'
	برائے (s = p_penc; *s != NUL; s++)
	{
	    اگر (*s == '_')
		*s = '-';
	    ورنہ
		*s = TOLOWER_ASC(*s);
	}
    }

    واپس تہی;
}
#اگر_ختم

#اگر defined(FEAT_PRINTER)

    ساکن char_u *
get_printoptions_نام(expand_T *xp غیر_مستعمل, صحیح idx)
{
    اگر (idx >= (صحیح)ARRAY_لمبائی(printer_opts))
	واپس تہی;
    واپس (char_u*)printer_opts[idx].name;
}

/*
 * Expand 'printoptions' option
 */
    صحیح
expand_سیٹ_printoptions(optexpand_T *دلائل, صحیح *عدد_Matches, char_u ***matches)
{
    واپس expand_سیٹ_opt_generic(
	    دلائل,
	    get_printoptions_نام,
	    عدد_Matches,
	    matches);
}
#اگر_ختم

#اگر defined(FEAT_STL_OPT)
/*
 * The 'statusline' or the 'tabline' or the 'rulerformat' option is changed.
 * "rulerformat" is TRUE if the 'rulerformat' option is changed.
 */
    ساکن حرف *
parse_statustabline_rulerformat(optset_T *دلائل, صحیح rulerformat)
{
    char_u	**varp = (char_u **)دلائل->os_varp;
    char_u	*s;
    حرف	*errmsg = تہی;
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
	اگر (wid && *s == '(' && (errmsg = check_stl_option(p_ruf)) == تہی)
	    ru_wid = wid;
	ورنہ
	{
	    // Validate the flags in 'rulerformat' only if it doesn't point to
	    // a custom function ("%!" flag).
	    اگر ((*varp)[1] != '!')
		errmsg = check_stl_option(p_ruf);
	}
    }
    // check 'statusline' or 'tabline' only if it doesn't start with "%!"
    ورنہ اگر (rulerformat || s[0] != '%' || s[1] != '!')
	errmsg = check_stl_option(s);
    اگر (rulerformat && errmsg == تہی)
	comp_ستون();

    واپس errmsg;
}
#اگر_ختم

#اگر defined(FEAT_RENDER_OPTIONS)
/*
 * The 'renderoptions' option is changed.
 */
    حرف *
did_سیٹ_renderoptions(optset_T *دلائل غیر_مستعمل)
{
    اگر (!gui_mch_سیٹ_rendering_options(p_rop))
	واپس e_invalid_دلیل;

    واپس تہی;
}
#اگر_ختم

#اگر defined(FEAT_RIGHTLEFT)
/*
 * The 'rightleftcmd' option is changed.
 */
    حرف *
did_سیٹ_rightleftcmd(optset_T *دلائل)
{
    char_u	**varp = (char_u **)دلائل->os_varp;

    // Currently only "search" is a supported value.
    اگر (**varp != NUL && STRCMP(*varp, "search") != 0)
	واپس e_invalid_دلیل;

    واپس تہی;
}

    صحیح
expand_سیٹ_rightleftcmd(optexpand_T *دلائل, صحیح *عدد_Matches, char_u ***matches)
{
    ساکن حرف *(p_rlc_اقدار[]) = {"search", تہی};
    واپس expand_سیٹ_opt_string(
	    دلائل,
	    p_rlc_اقدار,
	    ARRAY_لمبائی(p_rlc_اقدار) - 1,
	    عدد_Matches,
	    matches);
}
#اگر_ختم

#تعریف_کرو PUM_BORDER_CLEAR()   \
    کرو {                     \
	pum_سیٹ_border(FALSE); \
	pum_سیٹ_shadow(FALSE); \
	pum_سیٹ_margin(FALSE); \
    } جب (0)

/*
 * Parse a border value from a pumopt "border:" token.
 * Returns OK on success, FAIL on error.
 */
    ساکن صحیح
parse_pumopt_border(char_u *val, صحیح len)
{
    // Use box-drawing characters only when 'encoding' is "utf-8" and
    // 'ambiwidth' is "single".
    صحیح	    can_use_box_chars = (enc_utf8 && *p_ambw == 's');
    char_u  *token;

    token = vim_strnsave(val, len);
    اگر (token == تہی)
	واپس FAIL;

    اگر (can_use_box_chars && STRCMP(token, "single") == 0)
	pum_سیٹ_border_chars(0x2500, 0x2502, 0x2500, 0x2502, // ─ │ ─ │
		0x250c, 0x2510, 0x2518, 0x2514); // ┌ ┐ ┘ └
    ورنہ اگر (can_use_box_chars && STRCMP(token, "double") == 0)
	pum_سیٹ_border_chars(0x2550, 0x2551, 0x2550, 0x2551, // ═ ║ ═ ║
		0x2554, 0x2557, 0x255D, 0x255A); // ╔ ╗ ╝  ╚
    ورنہ اگر (can_use_box_chars && STRCMP(token, "round") == 0)
	pum_سیٹ_border_chars(0x2500, 0x2502, 0x2500, 0x2502, // ─ │ ─ │
		0x256d, 0x256e, 0x256f, 0x2570); // ╭ ╮ ╯ ╰
    ورنہ اگر (STRCMP(token, "ascii") == 0)
	pum_سیٹ_border_chars('-', '|', '-', '|', '+', '+', '+', '+');
    ورنہ اگر (STRNCMP(token, "custom:", 7) == 0)
    {
	char_u	*q = token + 7;
	صحیح	برآمد[8];

	برائے (صحیح i = 0; i < 8; i++)
	{
	    اگر (*q == NUL || *q == ',')
	    {
		vim_free(token);
		واپس FAIL;
	    }
	    برآمد[i] = mb_ptr2char(q);
	    mb_ptr2char_adv(&q);
	    اگر (i < 7)
	    {
		اگر (*q != ';')
		{
		    vim_free(token);
		    واپس FAIL;
		}
		q++;
	    }
	}
	اگر (*q != NUL && *q != ',')
	{
	    vim_free(token);
	    واپس FAIL;
	}
	pum_سیٹ_border_chars(برآمد[0], برآمد[1], برآمد[2], برآمد[3], برآمد[4], برآمد[5],
		برآمد[6], برآمد[7]);
    }
    ورنہ
    {
	vim_free(token);
	واپس FAIL;
    }

    vim_free(token);
    pum_سیٹ_border(TRUE);
    واپس OK;
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
did_سیٹ_pumopt(optset_T *دلائل)
{
    char_u  **varp = (char_u **)دلائل->os_varp;
    char_u  *p;
    صحیح	    have_border = FALSE;
    صحیح	    have_margin = FALSE;

    // Reset to defaults.
    PUM_BORDER_CLEAR();
    p_ph = 0;
    p_pw = 15;
    p_pmw = 0;
    p_po = 100;

    اگر (*varp == تہی || **varp == NUL)
	واپس تہی;

    برائے (p = *varp; p != تہی && *p != NUL; )
    {
	char_u *comma = vim_strchr(p, ',');
	صحیح len;

	اگر (comma != تہی)
	    len = (صحیح)(comma - p);
	ورنہ
	    len = (صحیح)STRLEN(p);

	اگر (STRNCMP(p, "border:", 7) == 0)
	{
	    اگر (have_border)
		جاؤ error;
	    have_border = TRUE;
	    اگر (parse_pumopt_border(p + 7, len - 7) == FAIL)
		جاؤ error;
	}
	ورنہ اگر (STRNCMP(p, "height:", 7) == 0)
	{
	    طویل n = atol((حرف *)p + 7);
	    اگر (n < 0)
		جاؤ error;
	    p_ph = n;
	}
	ورنہ اگر (STRNCMP(p, "width:", 6) == 0)
	{
	    طویل n = atol((حرف *)p + 6);
	    اگر (n < 0)
		جاؤ error;
	    p_pw = n;
	}
	ورنہ اگر (STRNCMP(p, "maxwidth:", 9) == 0)
	{
	    طویل n = atol((حرف *)p + 9);
	    اگر (n < 0)
		جاؤ error;
	    p_pmw = n;
	}
	ورنہ اگر (STRNCMP(p, "opacity:", 8) == 0)
	{
	    طویل n = atol((حرف *)p + 8);
	    اگر (n < 0 || n > 100)
		جاؤ error;
	    p_po = n;
	}
	ورنہ اگر (len == 6 && STRNCMP(p, "shadow", 6) == 0)
	    pum_سیٹ_shadow(TRUE);
	ورنہ اگر (len == 6 && STRNCMP(p, "margin", 6) == 0)
	{
	    have_margin = TRUE;
	    pum_سیٹ_margin(TRUE);
	}
	ورنہ
	    جاؤ error;

	اگر (comma != تہی)
	    p = comma + 1;
	ورنہ
	    توڑو;
    }

    اگر (have_margin && !have_border)
	جاؤ error;

    // Invalidate cached background for opacity changes.
    pum_opacity_بدلا();

    واپس تہی;

error:
    PUM_BORDER_CLEAR();
    p_ph = 0;
    p_pw = 15;
    p_pmw = 0;
    p_po = 100;
    واپس e_invalid_دلیل;
}

    ساکن char_u *
get_pum_border_style(expand_T *xp غیر_مستعمل, صحیح idx)
{
    ساکن حرف *styles[] = {"ascii", "custom:", "single", "double", "round"};
    واپس idx < ((enc_utf8 && *p_ambw == 's') ? (صحیح)ARRAY_لمبائی(styles) : 2)
	    ? (char_u *)styles[idx] : تہی;
}

    صحیح
expand_سیٹ_pumopt(optexpand_T *دلائل, صحیح *عدد_Matches, char_u ***matches)
{
    expand_T *xp = دلائل->oe_xp;

    اگر (xp->xp_نمونہ > دلائل->oe_سیٹ_arg && *(xp->xp_نمونہ-1) == ':')
    {
	اگر (completing_قدر_for_subopt(دلائل, "border"))
	{
	    واپس expand_سیٹ_opt_generic(
		    دلائل, get_pum_border_style, عدد_Matches, matches);
	}
	واپس FAIL;
    }

    ساکن حرف *(p_pumopt_اقدار[]) = {"border:", "height:", "width:",
	"maxwidth:", "opacity:", "shadow", "margin", تہی};
    واپس expand_سیٹ_opt_string(
	    دلائل,
	    p_pumopt_اقدار,
	    ARRAY_لمبائی(p_pumopt_اقدار) - 1,
	    عدد_Matches,
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
did_سیٹ_pumborder(optset_T *دلائل)
{
    char_u  **varp = (char_u **)دلائل->os_varp;
    char_u  *p;
    صحیح	    len;
    صحیح	    have_border = FALSE;
    صحیح	    have_margin = FALSE;

    PUM_BORDER_CLEAR();

    اگر (*varp == تہی || **varp == NUL)
	واپس تہی;

    برائے (p = *varp; p != تہی && *p != NUL; )
    {
	char_u *comma = vim_strchr(p, ',');
	اگر (comma != تہی)
	    len = (صحیح)(comma - p);
	ورنہ
	    len = (صحیح)STRLEN(p);

	اگر (STRNCMP(p, "shadow", len) == 0 && len == 6)
	    pum_سیٹ_shadow(TRUE);
	ورنہ اگر (STRNCMP(p, "margin", len) == 0 && len == 6)
	{
	    have_margin = TRUE;
	    pum_سیٹ_margin(TRUE);
	}
	ورنہ
	{
	    اگر (have_border)
		جاؤ error;
	    have_border = TRUE;
	    اگر (parse_pumopt_border(p, len) == FAIL)
		جاؤ error;
	}

	اگر (comma != تہی)
	    p = comma + 1;
	ورنہ
	    توڑو;
    }

    اگر (have_margin && !have_border)
	جاؤ error;

    واپس تہی;

error:
    PUM_BORDER_CLEAR();
    pum_سیٹ_border_chars(0, 0, 0, 0, 0, 0, 0, 0);
    واپس e_invalid_دلیل;
}

    ساکن char_u *
get_pumborder_token(expand_T *xp, صحیح idx)
{
    واپس idx == 0 ? (char_u *)"margin"
	 : idx == 1 ? (char_u *)"shadow"
	 : get_pum_border_style(xp, idx - 2);
}

    صحیح
expand_سیٹ_pumborder(optexpand_T *دلائل, صحیح *عدد_Matches, char_u ***matches)
{
    واپس expand_سیٹ_opt_generic(
	    دلائل, get_pumborder_token, عدد_Matches, matches);
}

#اگر defined(FEAT_STL_OPT)

/*
 * The 'rulerformat' option is changed.
 */
    حرف *
did_سیٹ_rulerformat(optset_T *دلائل)
{
    واپس parse_statustabline_rulerformat(دلائل, TRUE);
}
#اگر_ختم

#اگر defined(FEAT_TABPANEL)
/*
 * Process the new 'tabpanelopt' option value.
 */
    حرف *
did_سیٹ_tabpanelopt(optset_T *دلائل غیر_مستعمل)
{
    اگر (tabpanelopt_بدلا() == FAIL)
	واپس e_invalid_دلیل;

    واپس تہی;
}

    صحیح
expand_سیٹ_tabpanelopt(optexpand_T *دلائل, صحیح *عدد_Matches, char_u ***matches)
{
    expand_T *xp = دلائل->oe_xp;

    اگر (xp->xp_نمونہ > دلائل->oe_سیٹ_arg && *(xp->xp_نمونہ-1) == ':')
    {
	اگر (completing_قدر_for_subopt(دلائل, "align"))
	{
	    واپس expand_سیٹ_opt_string(
		    دلائل,
		    p_tplo_align_اقدار,
		    ARRAY_لمبائی(p_tplo_align_اقدار) - 1,
		    عدد_Matches,
		    matches);
	}
	واپس FAIL;
    }

    واپس expand_سیٹ_opt_string(
	    دلائل,
	    p_tplo_اقدار,
	    ARRAY_لمبائی(p_tplo_اقدار) - 1,
	    عدد_Matches,
	    matches);
}
#اگر_ختم

/*
 * The 'scrollopt' option is changed.
 */
    حرف *
did_سیٹ_scrollopt(optset_T *دلائل غیر_مستعمل)
{
    واپس did_سیٹ_opt_لڑیاں(p_sbo, p_scbopt_اقدار, TRUE);
}

    صحیح
expand_سیٹ_scrollopt(optexpand_T *دلائل, صحیح *عدد_Matches, char_u ***matches)
{
    واپس expand_سیٹ_opt_string(
	    دلائل,
	    p_scbopt_اقدار,
	    ARRAY_لمبائی(p_scbopt_اقدار) - 1,
	    عدد_Matches,
	    matches);
}

/*
 * The 'wlseat' option is changed
 */
    حرف *
did_سیٹ_wlseat(optset_T *دلائل غیر_مستعمل)
{
#اگر_تعریف_شدہ FEAT_WAYLAND_CLIPBOARD
    // If there isn't any seat named 'wlseat', then let the Wayland clipboard be
    // unavailable. Ignore errors returned.
    clip_reset_wayland();
#اگر_ختم

    واپس تہی;
}

/*
 * The 'selection' option is changed.
 */
    حرف *
did_سیٹ_selection(optset_T *دلائل غیر_مستعمل)
{
    اگر (*p_sel == NUL || check_opt_لڑیاں(p_sel, p_sel_اقدار, FALSE) != OK)
	واپس e_invalid_دلیل;

    واپس تہی;
}

    صحیح
expand_سیٹ_selection(optexpand_T *دلائل, صحیح *عدد_Matches, char_u ***matches)
{
    واپس expand_سیٹ_opt_string(
	    دلائل,
	    p_sel_اقدار,
	    ARRAY_لمبائی(p_sel_اقدار) - 1,
	    عدد_Matches,
	    matches);
}

/*
 * The 'selectmode' option is changed.
 */
    حرف *
did_سیٹ_selectmode(optset_T *دلائل غیر_مستعمل)
{
    واپس did_سیٹ_opt_لڑیاں(p_slm, p_slm_اقدار, TRUE);
}

    صحیح
expand_سیٹ_selectmode(optexpand_T *دلائل, صحیح *عدد_Matches, char_u ***matches)
{
    واپس expand_سیٹ_opt_string(
	    دلائل,
	    p_slm_اقدار,
	    ARRAY_لمبائی(p_slm_اقدار) - 1,
	    عدد_Matches,
	    matches);
}

#اگر defined(FEAT_SESSION)
/*
 * The 'sessionoptions' option is changed.
 */
    حرف *
did_سیٹ_sessionoptions(optset_T *دلائل)
{
    اگر (opt_لڑیاں_flags(p_ssop, p_ssop_اقدار, &ssop_flags, TRUE) != OK)
	واپس e_invalid_دلیل;
    اگر ((ssop_flags & SSOP_CURDIR) && (ssop_flags & SSOP_SESDIR))
    {
	// Don't allow both "sesdir" and "curdir".
	(خالی)opt_لڑیاں_flags(دلائل->os_oldval.string, p_ssop_اقدار,
							&ssop_flags, TRUE);
	واپس e_invalid_دلیل;
    }

    واپس تہی;
}

    صحیح
expand_سیٹ_sessionoptions(
    optexpand_T		*دلائل,
    صحیح			*عدد_Matches,
    char_u		***matches)
{
    واپس expand_سیٹ_opt_string(
	    دلائل,
	    p_ssop_اقدار,
	    ARRAY_لمبائی(p_ssop_اقدار) - 1,
	    عدد_Matches,
	    matches);
}
#اگر_ختم

/*
 * Validate 'shellpipe'/'shellredir' option.
 */
    حرف *
did_سیٹ_shellpipe_redir(optset_T *دلائل)
{
    char_u	*p;
    bool	نظر_آیا = false;

    برائے (p = دلائل->os_newval.string; *p != NUL; ++p)
    {
	اگر (*p != '%')
	    جاری;

	اگر (p[1] == NUL)
	    واپس e_invalid_format_string_single_percent_s;

	اگر (p[1] == '%')
	{
	    ++p;    // skip second %
	    جاری;
	}

	اگر (p[1] == 's')
	{
	    اگر (نظر_آیا)
		واپس e_invalid_format_string_single_percent_s;

	    نظر_آیا = true;
	    ++p;    // consume 's'
	    جاری;
	}
	واپس e_invalid_format_string_single_percent_s;
    }
    واپس تہی;
}

/*
 * The 'shortmess' option is changed.
 */
    حرف *
did_سیٹ_shortmess(optset_T *دلائل)
{
    char_u	**varp = (char_u **)دلائل->os_varp;

    واپس did_سیٹ_option_listflag(*varp, (char_u *)SHM_ALL, دلائل->os_errbuf,
		    دلائل->os_errbuflen);
}

    صحیح
expand_سیٹ_shortmess(optexpand_T *دلائل, صحیح *عدد_Matches, char_u ***matches)
{
    واپس expand_سیٹ_opt_listflag(دلائل, (char_u*)SHM_ALL, عدد_Matches, matches);
}

#اگر defined(FEAT_LINEBREAK)
/*
 * The 'showbreak' option is changed.
 */
    حرف *
did_سیٹ_showbreak(optset_T *دلائل)
{
    char_u	**varp = (char_u **)دلائل->os_varp;
    char_u	*s;

    برائے (s = *varp; *s; )
    {
	اگر (ptr2cells(s) != 1)
	    واپس e_showbreak_شامل_ہے_unprintable_or_wide_character;
	MB_PTR_ADV(s);
    }

    واپس تہی;
}
#اگر_ختم

/*
 * The 'showcmdloc' option is changed.
 */
    حرف *
did_سیٹ_showcmdloc(optset_T *دلائل غیر_مستعمل)
{
    حرف	*errmsg = did_سیٹ_opt_لڑیاں(p_sloc, p_sloc_اقدار, FALSE);

    اگر (errmsg == تہی)
	comp_ستون();

    واپس errmsg;
}

    صحیح
expand_سیٹ_showcmdloc(optexpand_T *دلائل, صحیح *عدد_Matches, char_u ***matches)
{
    واپس expand_سیٹ_opt_string(
	    دلائل,
	    p_sloc_اقدار,
	    ARRAY_لمبائی(p_sloc_اقدار) - 1,
	    عدد_Matches,
	    matches);
}

#اگر defined(FEAT_SIGNS)
/*
 * The 'signcolumn' option is changed.
 */
    حرف *
did_سیٹ_signcolumn(optset_T *دلائل)
{
    char_u	**varp = (char_u **)دلائل->os_varp;

# اگر defined(FEAT_LINEBREAK)
    اگر (check_opt_لڑیاں(*varp, p_scl_اقدار, FALSE) != OK)
	واپس e_invalid_دلیل;
    // When changing the 'signcolumn' to or from 'number', recompute the
    // width of the number column if 'number' or 'relativenumber' is set.
    اگر (((*دلائل->os_oldval.string == 'n' && دلائل->os_oldval.string[1] == 'u')
		|| (*curwin->w_p_scl == 'n' && *(curwin->w_p_scl + 1) =='u'))
	    && (curwin->w_p_nu || curwin->w_p_rnu))
	curwin->w_nrwidth_سطر_گنتی = 0;
# endif

# اگر defined(FEAT_GUI)
    // In the GUI, when sign icons are used, a full screen clear is needed
    // to properly redraw the sign icons.
    اگر (gui.اندر_use && curbuf->b_signlist != تہی)
	redraw_all_later(UPD_CLEAR);
# endif

    واپس تہی;
}

    صحیح
expand_سیٹ_signcolumn(optexpand_T *دلائل, صحیح *عدد_Matches, char_u ***matches)
{
    واپس expand_سیٹ_opt_string(
	    دلائل,
	    p_scl_اقدار,
	    ARRAY_لمبائی(p_scl_اقدار) - 1,
	    عدد_Matches,
	    matches);
}
#اگر_ختم

#اگر defined(FEAT_SPELL)
/*
 * The 'spellcapcheck' option is changed.
 */
    حرف *
did_سیٹ_spellcapcheck(optset_T *دلائل غیر_مستعمل)
{
    // compile the regexp program.
    واپس compile_cap_prog(curwin->w_s);
}

/*
 * The 'spellfile' option is changed.
 */
    حرف *
did_سیٹ_spellfile(optset_T *دلائل)
{
    char_u	**varp = (char_u **)دلائل->os_varp;

    اگر (!valid_spellfile(*varp))
	واپس e_invalid_دلیل;

    // If there is a window for this buffer in which 'spell' is set load the
    // wordlists.
    واپس did_سیٹ_spell_option();
}

/*
 * The 'spell' option is changed.
 */
    حرف *
did_سیٹ_spelllang(optset_T *دلائل)
{
    char_u	**varp = (char_u **)دلائل->os_varp;

    اگر (!valid_spelllang(*varp))
	واپس e_invalid_دلیل;

    // If there is a window for this buffer in which 'spell' is set load the
    // wordlists.
    واپس did_سیٹ_spell_option();
}

/*
 * The 'spelloptions' option is changed.
 */
    حرف *
did_سیٹ_spelloptions(optset_T *دلائل)
{
    char_u	**varp = (char_u **)دلائل->os_varp;

    اگر (**varp != NUL && STRCMP(*varp, "camel") != 0)
	واپس e_invalid_دلیل;

    واپس تہی;
}

    صحیح
expand_سیٹ_spelloptions(optexpand_T *دلائل, صحیح *عدد_Matches, char_u ***matches)
{
    ساکن حرف *(p_spo_اقدار[]) = {"camel", تہی};
    واپس expand_سیٹ_opt_string(
	    دلائل,
	    p_spo_اقدار,
	    ARRAY_لمبائی(p_spo_اقدار) - 1,
	    عدد_Matches,
	    matches);
}

/*
 * The 'spellsuggest' option is changed.
 */
    حرف *
did_سیٹ_spellsuggest(optset_T *دلائل غیر_مستعمل)
{
    اگر (spell_check_sps() != OK)
	واپس e_invalid_دلیل;

    واپس تہی;
}

    صحیح
expand_سیٹ_spellsuggest(optexpand_T *دلائل, صحیح *عدد_Matches, char_u ***matches)
{
    واپس expand_سیٹ_opt_string(
	    دلائل,
	    p_sps_اقدار,
	    ARRAY_لمبائی(p_sps_اقدار) - 1,
	    عدد_Matches,
	    matches);
}
#اگر_ختم

/*
 * The 'splitkeep' option is changed.
 */
    حرف *
did_سیٹ_splitkeep(optset_T *دلائل غیر_مستعمل)
{
    win_T	*wp;
    tabpage_T	*tp;
    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	wp->w_پچھلا_اونچائی = wp->w_اونچائی;
	wp->w_پچھلا_winrow = wp->w_winrow;
    }
    واپس did_سیٹ_opt_لڑیاں(p_spk, p_spk_اقدار, FALSE);
}

    صحیح
expand_سیٹ_splitkeep(optexpand_T *دلائل, صحیح *عدد_Matches, char_u ***matches)
{
    واپس expand_سیٹ_opt_string(
	    دلائل,
	    p_spk_اقدار,
	    ARRAY_لمبائی(p_spk_اقدار) - 1,
	    عدد_Matches,
	    matches);
}

#اگر defined(FEAT_STL_OPT)
/*
 * The 'statusline' option is changed.
 */
    حرف *
did_سیٹ_statusline(optset_T *دلائل)
{
    char_u  **varp = (char_u **)دلائل->os_varp;
    حرف    *ret = parse_statustabline_rulerformat(دلائل, FALSE);

    اگر (ret != تہی)
	واپس ret;
    update_stl_rendered_اونچائی(varp == &curwin->w_p_stl ? curwin : تہی);
    چوکھٹا_change_statusline_اونچائی();

    واپس تہی;
}

/*
 * Rewrite the 'statuslineopt' string: replace the last "maxheight:N" with
 * actual_stlh and remove earlier duplicates.  Keyword order is preserved.
 */
    ساکن خالی
update_stlo_maxheight(char_u **varp, صحیح actual_stlh)
{
    ثابت char_u    *src = *varp;
    ثابت char_u    *آخری_mh = تہی;
    ثابت char_u    *p;

    // Find the last "maxheight:" token.
    p = src;
    جب (*p != NUL)
    {
	اگر (STRNCMP(p, "maxheight:", 10) == 0)
	    آخری_mh = p;
	جب (*p != ',' && *p != NUL)
	    ++p;
	اگر (*p == ',')
	    ++p;
    }

    اگر (آخری_mh == تہی)
	واپس;

    size_t	bufsize = STRLEN(src) + NUMBUFLEN + 1;
    char_u	*buf = alloc(bufsize);
    اگر (buf == تہی)
	واپس;

    صحیح		len = 0;
    bool	need_comma = false;

    p = src;
    جب (*p != NUL)
    {
	ثابت	char_u	*tok = p;
	صحیح	tok_len;

	جب (*p != ',' && *p != NUL)
	    ++p;
	tok_len = (صحیح)(p - tok);
	اگر (*p == ',')
	    ++p;

	اگر (STRNCMP(tok, "maxheight:", 10) == 0)
	{
	    اگر (tok == آخری_mh)
	    {
		// Replace the last occurrence with the actual value.
		اگر (need_comma)
		    buf[len++] = ',';
		len += vim_snprintf((حرف *)buf + len,
			bufsize - len, "maxheight:%d", actual_stlh);
		need_comma = true;
	    }
	}
	ورنہ
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
    حرف *
did_سیٹ_statuslineopt(optset_T *دلائل)
{
    char_u	**varp = (char_u **)دلائل->os_varp;
    win_T       *wp = varp == &curwin->w_p_stlo ? curwin : تہی;

    اگر (statuslineopt_بدلا(*varp, wp) == FAIL)
	واپس e_invalid_دلیل;

    // Sync stl_rendered_height with the current statusline.
    update_stl_rendered_اونچائی(wp);

    // Update the maxheight value to the actual value set.
    // Note: Must be changed if p_stlo_values are changed.
    اگر (*varp != خالی_خانے_option)
    {
	صحیح actual_stlh;

	اگر (wp != تہی)
	{
	    چوکھٹا_change_statusline_اونچائی();
	    actual_stlh = MIN(wp->w_p_stlo_mh,
		    wp->w_اونچائی + wp->w_حالت_اونچائی - p_wmh);
	}
	ورنہ
	    actual_stlh = چوکھٹا_change_statusline_اونچائی();

	// Only re-serialize when the window actually has a status line shown.
	اگر (actual_stlh > 0)
	{
	    update_stlo_maxheight(varp, actual_stlh);
	    // Update the parsed maxheight member directly.
	    اگر (wp != تہی)
		wp->w_p_stlo_mh = actual_stlh;
	    ورنہ
		سیٹ_stlo_mh(actual_stlh);
	}
    }
    ورنہ
	چوکھٹا_change_statusline_اونچائی();

    اگر ((دلائل->os_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	// :set clears the local values.
	clear_string_option(&curwin->w_p_stlo);
	curwin->w_p_stlo_fh = -1;
	curwin->w_p_stlo_mh = -1;
    }

    واپس تہی;
}

    صحیح
expand_سیٹ_statuslineopt(optexpand_T *دلائل, صحیح *عدد_Matches, char_u ***matches)
{
    واپس expand_سیٹ_opt_string(
	    دلائل,
	    p_stlo_اقدار,
	    ARRAY_لمبائی(p_stlo_اقدار) - 1,
	    عدد_Matches,
	    matches);
}
#اگر_ختم

/*
 * The 'swapsync' option is changed.
 */
    حرف *
did_سیٹ_swapsync(optset_T *دلائل غیر_مستعمل)
{
    واپس did_سیٹ_opt_لڑیاں(p_sws, p_sws_اقدار, FALSE);
}

    صحیح
expand_سیٹ_swapsync(optexpand_T *دلائل, صحیح *عدد_Matches, char_u ***matches)
{
    واپس expand_سیٹ_opt_string(
	    دلائل,
	    p_sws_اقدار,
	    ARRAY_لمبائی(p_sws_اقدار) - 1,
	    عدد_Matches,
	    matches);
}

/*
 * The 'switchbuf' option is changed.
 */
    حرف *
did_سیٹ_switchbuf(optset_T *دلائل غیر_مستعمل)
{
    واپس did_سیٹ_opt_flags(p_swb, p_swb_اقدار, &swb_flags, TRUE);
}

    صحیح
expand_سیٹ_switchbuf(optexpand_T *دلائل, صحیح *عدد_Matches, char_u ***matches)
{
    واپس expand_سیٹ_opt_string(
	    دلائل,
	    p_swb_اقدار,
	    ARRAY_لمبائی(p_swb_اقدار) - 1,
	    عدد_Matches,
	    matches);
}

/*
 * The 'tabclose' option is changed.
 */
    حرف *
did_سیٹ_tabclose(optset_T *دلائل غیر_مستعمل)
{
    واپس did_سیٹ_opt_flags(p_tcl, p_tcl_اقدار, &tcl_flags, TRUE);
}

    صحیح
expand_سیٹ_tabclose(optexpand_T *دلائل, صحیح *عدد_Matches, char_u ***matches)
{
    واپس expand_سیٹ_opt_string(
	    دلائل,
	    p_tcl_اقدار,
	    ARRAY_لمبائی(p_tcl_اقدار) - 1,
	    عدد_Matches,
	    matches);
}

#اگر defined(FEAT_STL_OPT)
/*
 * The 'tabline' option is changed.
 */
    حرف *
did_سیٹ_tabline(optset_T *دلائل)
{
    واپس parse_statustabline_rulerformat(دلائل, FALSE);
}
#اگر_ختم

/*
 * The 'tagcase' option is changed.
 */
    حرف *
did_سیٹ_tagcase(optset_T *دلائل)
{
    غیرعلامتی صحیح	*flags;
    char_u		*p;

    اگر (دلائل->os_flags & OPT_LOCAL)
    {
	p = curbuf->b_p_tc;
	flags = &curbuf->b_tc_flags;
    }
    ورنہ
    {
	p = p_tc;
	flags = &tc_flags;
    }

    اگر ((دلائل->os_flags & OPT_LOCAL) && *p == NUL)
	// make the local value empty: use the global value
	*flags = 0;
    ورنہ اگر (*p == NUL
	    || opt_لڑیاں_flags(p, p_tc_اقدار, flags, FALSE) != OK)
	واپس e_invalid_دلیل;

    واپس تہی;
}

    صحیح
expand_سیٹ_tagcase(optexpand_T *دلائل, صحیح *عدد_Matches, char_u ***matches)
{
    واپس expand_سیٹ_opt_string(
	    دلائل,
	    p_tc_اقدار,
	    ARRAY_لمبائی(p_tc_اقدار) - 1,
	    عدد_Matches,
	    matches);
}

/*
 * The 'term' option is changed.
 */
    حرف *
did_سیٹ_رکن(optset_T *دلائل غیر_مستعمل)
{
    اگر (T_NAME[0] == NUL)
	واپس e_cannot_سیٹ_رکن_مقصد_خالی_خانے_string;
#اگر_تعریف_شدہ FEAT_GUI
    اگر (gui.اندر_use)
	واپس e_cannot_change_رکن_اندر_GUI;
    اگر (رکن_is_gui(T_NAME))
	واپس e_use_gui_مقصد_آغاز_GUI;
#اگر_ختم
    اگر (سیٹ_termname(T_NAME) == FAIL)
	واپس e_not_ملا_اندر_termcap;

    // Screen colors may have changed.
    redraw_later_clear();

    واپس تہی;
}

/*
 * Some terminal option (t_xxx) is changed
 */
    حرف *
did_سیٹ_رکن_option(optset_T *دلائل)
{
    char_u	**varp = (char_u **)دلائل->os_varp;

    اگر (!بھرا_screen)
	واپس تہی;

    // ":set t_Co=0" and ":set t_Co=1" do ":set t_Co="
    اگر (varp == &T_CCO)
    {
	صحیح colors = atoi((حرف *)T_CCO);

	// Only reinitialize colors if t_Co value has really changed to
	// avoid expensive reload of colorscheme if t_Co is set to the
	// same value multiple times.
	اگر (colors != t_colors)
	{
	    t_colors = colors;
	    اگر (t_colors <= 1)
	    {
		vim_free(T_CCO);
		T_CCO = خالی_خانے_option;
	    }
#اگر defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
	    اگر (is_رکن_win32())
	    {
		تبادلہ_tcap();
		دلائل->os_did_swaptcap = TRUE;
	    }
#اگر_ختم
	    // We now have a different color setup, initialize it again.
	    init_highlight(TRUE, FALSE);
	}
    }
    ttest(FALSE);
    اگر (varp == &T_ME)
    {
	برآمد_str(T_ME);
	redraw_later(UPD_CLEAR);
#اگر defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
	// Since t_me has been set, this probably means that the user
	// wants to use this as default colors.  Need to reset default
	// background/foreground colors.
# ifdef VIMDLL
	اگر (!gui.اندر_use && !gui.starting)
# endif
	    mch_سیٹ_معمول_colors();
#اگر_ختم
    }
    اگر (varp == &T_BE && termcap_فعال)
    {
	MAY_WANT_مقصد_LOG_THIS;

	اگر (*T_BE == NUL)
	    // When clearing t_BE we assume the user no longer wants
	    // bracketed paste, thus disable it by writing t_BD.
	    برآمد_str(T_BD);
	ورنہ
	    برآمد_str(T_BE);
    }

    اگر (varp == &T_BSU || varp == &T_ESU)
	رکن_سیٹ_sync_output(رکن_SYNC_OUTPUT_OFF);

    واپس تہی;
}


#اگر_تعریف_شدہ UNIX
/*
 * The 'termresize' option is changed.
 */
    حرف *
did_سیٹ_termresize(optset_T *دلائل غیر_مستعمل)
{
    // If empty or "inband", then attempt to enable in-band resize events.
    اگر (*p_trz == NUL || STRCMP(p_trz, "inband") == 0)
	رکن_سیٹ_win_resize(true);
    ورنہ اگر (STRCMP(p_trz, "sigwinch") == 0)
	رکن_سیٹ_win_resize(false);
    ورنہ
	واپس e_invalid_دلیل;

    واپس تہی;
}

    صحیح
expand_سیٹ_termresize(optexpand_T *دلائل, صحیح *عدد_Matches, char_u ***matches)
{
    واپس expand_سیٹ_opt_string(
	    دلائل,
	    p_trz_اقدار,
	    ARRAY_لمبائی(p_trz_اقدار) - 1,
	    عدد_Matches,
	    matches);
}
#اگر_ختم

#اگر defined(FEAT_TERMINAL)
/*
 * The 'termwinkey' option is changed.
 */
    حرف *
did_سیٹ_termwinkey(optset_T *دلائل)
{
    char_u	**varp = (char_u **)دلائل->os_varp;

    اگر ((*varp)[0] != NUL && string_مقصد_کلید(*varp, TRUE) == 0)
	واپس e_invalid_دلیل;

    واپس تہی;
}

/*
 * The 'termwinsize' option is changed.
 */
    حرف *
did_سیٹ_termwinsize(optset_T *دلائل)
{
    char_u	**varp = (char_u **)دلائل->os_varp;
    char_u	*p;

    اگر ((*varp)[0] == NUL)
	واپس تہی;

    p = skipdigits(*varp);
    اگر (p == *varp || (*p != 'x' && *p != '*') || *skipdigits(p + 1) != NUL)
	واپس e_invalid_دلیل;

    واپس تہی;
}

# اگر defined(MSWIN)
/*
 * The 'termwintype' option is changed.
 */
    حرف *
did_سیٹ_termwintype(optset_T *دلائل غیر_مستعمل)
{
    واپس did_سیٹ_opt_لڑیاں(p_twt, p_twt_اقدار, FALSE);
}

    صحیح
expand_سیٹ_termwintype(optexpand_T *دلائل, صحیح *عدد_Matches, char_u ***matches)
{
    واپس expand_سیٹ_opt_string(
	    دلائل,
	    p_twt_اقدار,
	    ARRAY_لمبائی(p_twt_اقدار) - 1,
	    عدد_Matches,
	    matches);
}
# endif
#اگر_ختم

/*
 * The 'titlestring' option is changed.
 */
    حرف *
did_سیٹ_titlestring(optset_T *دلائل)
{
    صحیح flagval = 0;

#اگر_تعریف_شدہ FEAT_STL_OPT
    flagval = STL_اندر_TITLE;
#اگر_ختم
    واپس parse_titleiconstring(دلائل, flagval);
}

#اگر defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
/*
 * The 'toolbar' option is changed.
 */
    حرف *
did_سیٹ_toolbar(optset_T *دلائل غیر_مستعمل)
{
    اگر (opt_لڑیاں_flags(p_toolbar, p_toolbar_اقدار, &toolbar_flags,
								TRUE) != OK)
	واپس e_invalid_دلیل;

    برآمد_flush();
    gui_mch_show_toolbar((toolbar_flags &
		(TOOLBAR_متن | TOOLBAR_ICONS)) != 0);
    واپس تہی;
}

    صحیح
expand_سیٹ_toolbar(optexpand_T *دلائل, صحیح *عدد_Matches, char_u ***matches)
{
    واپس expand_سیٹ_opt_string(
	    دلائل,
	    p_toolbar_اقدار,
	    ARRAY_لمبائی(p_toolbar_اقدار) - 1,
	    عدد_Matches,
	    matches);
}
#اگر_ختم

#اگر defined(FEAT_TOOLBAR) && defined(FEAT_GUI_GTK)
/*
 * The 'toolbariconsize' option is changed.  GTK+ 2 only.
 */
    حرف *
did_سیٹ_toolbariconsize(optset_T *دلائل غیر_مستعمل)
{
    اگر (opt_لڑیاں_flags(p_tbis, p_tbis_اقدار, &tbis_flags, FALSE) != OK)
	واپس e_invalid_دلیل;

    برآمد_flush();
    gui_mch_show_toolbar((toolbar_flags &
		(TOOLBAR_متن | TOOLBAR_ICONS)) != 0);
    واپس تہی;
}

    صحیح
expand_سیٹ_toolbariconsize(
    optexpand_T	*دلائل,
    صحیح		*عدد_Matches,
    char_u	***matches)
{
    واپس expand_سیٹ_opt_string(
	    دلائل,
	    p_tbis_اقدار,
	    ARRAY_لمبائی(p_tbis_اقدار) - 1,
	    عدد_Matches,
	    matches);
}
#اگر_ختم

#اگر defined(UNIX) || defined(VMS)
/*
 * The 'ttymouse' option is changed.
 */
    حرف *
did_سیٹ_ttymouse(optset_T *دلائل غیر_مستعمل)
{
    حرف *errmsg = تہی;

    // Switch the mouse off before changing the escape sequences used for
    // that.
    mch_setmouse(FALSE);
    اگر (opt_لڑیاں_flags(p_ttym, p_ttym_اقدار, &ttym_flags, FALSE) != OK)
	errmsg = e_invalid_دلیل;
    ورنہ
	check_mouse_termcode();
    اگر (termcap_فعال)
	setmouse();		// may switch it on again

    واپس errmsg;
}

    صحیح
expand_سیٹ_ttymouse(optexpand_T *دلائل, صحیح *عدد_Matches, char_u ***matches)
{
    واپس expand_سیٹ_opt_string(
	    دلائل,
	    p_ttym_اقدار,
	    ARRAY_لمبائی(p_ttym_اقدار) - 1,
	    عدد_Matches,
	    matches);
}
#اگر_ختم

#اگر defined(FEAT_VARTABS)
/*
 * The 'varsofttabstop' option is changed.
 */
    حرف *
did_سیٹ_varsofttabstop(optset_T *دلائل)
{
    char_u	**varp = (char_u **)دلائل->os_varp;
    char_u *cp;

    اگر (!((*varp)[0]) || ((*varp)[0] == '0' && !((*varp)[1])))
	VIM_CLEAR(curbuf->b_p_vsts_array);
    ورنہ
    {
	برائے (cp = *varp; *cp; ++cp)
	{
	    اگر (vim_isdigit(*cp))
		جاری;
	    اگر (*cp == ',' && cp > *varp && *(cp-1) != ',')
		جاری;
	    واپس e_invalid_دلیل;
	}

	صحیح *oldarray = curbuf->b_p_vsts_array;
	اگر (tabstop_سیٹ(*varp, &(curbuf->b_p_vsts_array)) == OK)
	{
	    اگر (oldarray)
		vim_free(oldarray);
	}
	ورنہ
	    واپس e_invalid_دلیل;
    }

    واپس تہی;
}

/*
 * The 'vartabstop' option is changed.
 */
    حرف *
did_سیٹ_vartabstop(optset_T *دلائل)
{
    char_u	**varp = (char_u **)دلائل->os_varp;
    char_u *cp;

    اگر (!((*varp)[0]) || ((*varp)[0] == '0' && !((*varp)[1])))
	VIM_CLEAR(curbuf->b_p_vts_array);
    ورنہ
    {
	برائے (cp = *varp; *cp; ++cp)
	{
	    اگر (vim_isdigit(*cp))
		جاری;
	    اگر (*cp == ',' && cp > *varp && *(cp-1) != ',')
		جاری;
	    واپس e_invalid_دلیل;
	}

	صحیح *oldarray = curbuf->b_p_vts_array;

	اگر (tabstop_سیٹ(*varp, &(curbuf->b_p_vts_array)) == OK)
	{
	    vim_free(oldarray);
# ifdef FEAT_FOLDING
	    اگر (foldmethodIsIndent(curwin))
		foldUpdateAll(curwin);
# endif
	}
	ورنہ
	    واپس e_invalid_دلیل;
    }

    واپس تہی;
}
#اگر_ختم

/*
 * The 'verbosefile' option is changed.
 */
    حرف *
did_سیٹ_verbosefile(optset_T *دلائل غیر_مستعمل)
{
    verbose_stop();
    اگر (*p_vfile != NUL && verbose_open() == FAIL)
	واپس e_invalid_دلیل;

    واپس تہی;
}

#اگر defined(FEAT_SESSION)
/*
 * The 'viewoptions' option is changed.
 */
    حرف *
did_سیٹ_viewoptions(optset_T *دلائل غیر_مستعمل)
{
    واپس did_سیٹ_opt_flags(p_vop, p_ssop_اقدار, &vop_flags, TRUE);
}
#اگر_ختم

#اگر defined(FEAT_VIMINFO)
/*
 * The 'viminfo' option is changed.
 */
    حرف *
did_سیٹ_viminfo(optset_T *دلائل)
{
    char_u	*s;
    حرف	*errmsg = تہی;

    برائے (s = p_viminfo; *s;)
    {
	// Check it's a valid character
	اگر (vim_strchr((char_u *)"!\"%'/:<@cfhnrs", *s) == تہی)
	{
	    errmsg = illegal_char(دلائل->os_errbuf, دلائل->os_errbuflen, *s);
	    توڑو;
	}
	اگر (*s == 'n')	// name is always last one
	    توڑو;
	ورنہ اگر (*s == 'r') // skip until next ','
	{
	    جب (*++s && *s != ',')
		;
	}
	ورنہ اگر (*s == '%')
	{
	    // optional number
	    جب (vim_isdigit(*++s))
		;
	}
	ورنہ اگر (*s == '!' || *s == 'h' || *s == 'c')
	    ++s;		// no extra chars
	ورنہ		// must have a number
	{
	    جب (vim_isdigit(*++s))
		;

	    اگر (!VIM_ISDIGIT(*(s - 1)))
	    {
		اگر (دلائل->os_errbuf != تہی)
		{
		    vim_snprintf(دلائل->os_errbuf, دلائل->os_errbuflen,
			    _(e_missing_نمبر_after_angle_str_angle),
			    transchar_byte(*(s - 1)));
		    errmsg = دلائل->os_errbuf;
		}
		ورنہ
		    errmsg = "";
		توڑو;
	    }
	}
	اگر (*s == ',')
	    ++s;
	ورنہ اگر (*s)
	{
	    اگر (دلائل->os_errbuf != تہی)
		errmsg = e_missing_comma;
	    ورنہ
		errmsg = "";
	    توڑو;
	}
    }
    اگر (*p_viminfo && errmsg == تہی && get_viminfo_parameter('\'') < 0)
	errmsg = e_must_specify_a_قدر;

    واپس errmsg;
}
#اگر_ختم

/*
 * The 'virtualedit' option is changed.
 */
    حرف *
did_سیٹ_virtualedit(optset_T *دلائل)
{
    char_u		*ve = p_ve;
    غیرعلامتی صحیح	*flags = &ve_flags;

    اگر (دلائل->os_flags & OPT_LOCAL)
    {
	ve = curwin->w_p_ve;
	flags = &curwin->w_ve_flags;
    }

    اگر ((دلائل->os_flags & OPT_LOCAL) && *ve == NUL)
	// make the local value empty: use the global value
	*flags = 0;
    ورنہ
    {
	اگر (opt_لڑیاں_flags(ve, p_ve_اقدار, flags, TRUE) != OK)
	    واپس e_invalid_دلیل;
	ورنہ اگر (STRCMP(ve, دلائل->os_oldval.string) != 0)
	{
	    // Recompute cursor position in case the new 've' setting
	    // changes something.
	    validate_virtcol();
	    coladvance(curwin->w_virtcol);
	}
    }

    واپس تہی;
}

    صحیح
expand_سیٹ_virtualedit(optexpand_T *دلائل, صحیح *عدد_Matches, char_u ***matches)
{
    واپس expand_سیٹ_opt_string(
	    دلائل,
	    p_ve_اقدار,
	    ARRAY_لمبائی(p_ve_اقدار) - 1,
	    عدد_Matches,
	    matches);
}

/*
 * The 'whichwrap' option is changed.
 */
    حرف *
did_سیٹ_whichwrap(optset_T *دلائل)
{
    char_u	**varp = (char_u **)دلائل->os_varp;

    // Add ',' to the list flags because 'whichwrap' is a flag
    // list that is comma-separated.
    واپس did_سیٹ_option_listflag(*varp, (char_u *)(WW_ALL ","),
		    دلائل->os_errbuf, دلائل->os_errbuflen);
}

    صحیح
expand_سیٹ_whichwrap(optexpand_T *دلائل, صحیح *عدد_Matches, char_u ***matches)
{
    واپس expand_سیٹ_opt_listflag(دلائل, (char_u*)WW_ALL, عدد_Matches, matches);
}

/*
 * The 'wildmode' option is changed.
 */
    حرف *
did_سیٹ_wildmode(optset_T *دلائل غیر_مستعمل)
{
    اگر (check_opt_wim() == FAIL)
	واپس e_invalid_دلیل;
    واپس تہی;
}

    صحیح
expand_سیٹ_wildmode(optexpand_T *دلائل, صحیح *عدد_Matches, char_u ***matches)
{
    واپس expand_سیٹ_opt_string(
	    دلائل,
	    p_wim_اقدار,
	    ARRAY_لمبائی(p_wim_اقدار) - 1,
	    عدد_Matches,
	    matches);
}

/*
 * The 'wildoptions' option is changed.
 */
    حرف *
did_سیٹ_wildoptions(optset_T *دلائل غیر_مستعمل)
{
    واپس did_سیٹ_opt_لڑیاں(p_wop, p_wop_اقدار, TRUE);
}

    صحیح
expand_سیٹ_wildoptions(optexpand_T *دلائل, صحیح *عدد_Matches, char_u ***matches)
{
    واپس expand_سیٹ_opt_string(
	    دلائل,
	    p_wop_اقدار,
	    ARRAY_لمبائی(p_wop_اقدار) - 1,
	    عدد_Matches,
	    matches);
}

#اگر defined(FEAT_WAK)
/*
 * The 'winaltkeys' option is changed.
 */
    حرف *
did_سیٹ_winaltkeys(optset_T *دلائل غیر_مستعمل)
{
    حرف *errmsg = تہی;

    اگر (*p_wak == NUL || check_opt_لڑیاں(p_wak, p_wak_اقدار, FALSE) != OK)
	errmsg = e_invalid_دلیل;
# ifdef FEAT_MENU
#  اگر defined(FEAT_GUI_MOTIF)
    ورنہ اگر (gui.اندر_use)
	gui_motif_سیٹ_mnemonics(p_wak[0] == 'y' || p_wak[0] == 'm');
#  elif defined(FEAT_GUI_GTK)
    ورنہ اگر (gui.اندر_use)
	gui_gtk_سیٹ_mnemonics(p_wak[0] == 'y' || p_wak[0] == 'm');
#  endif
# endif
    واپس errmsg;
}

    صحیح
expand_سیٹ_winaltkeys(optexpand_T *دلائل, صحیح *عدد_Matches, char_u ***matches)
{
    واپس expand_سیٹ_opt_string(
	    دلائل,
	    p_wak_اقدار,
	    ARRAY_لمبائی(p_wak_اقدار) - 1,
	    عدد_Matches,
	    matches);
}
#اگر_ختم

/*
 * The 'wincolor' option is changed.
 */
    حرف *
did_سیٹ_wincolor(optset_T *دلائل غیر_مستعمل)
{
    update_wincolor(curwin, دلائل->os_newval.string);
    واپس تہی;
}

/*
 * The 'winhighlight' option is changed
 */
    حرف *
did_سیٹ_winhighlight(optset_T *دلائل)
{
    واپس update_winhighlight(curwin, دلائل->os_newval.string);
}

/*
 * Expand 'winhighlight' option.
 */
    صحیح
expand_سیٹ_winhighlight(optexpand_T *دلائل, صحیح *عدد_Matches, char_u ***matches)
{
    expand_T	    *xp = دلائل->oe_xp;

    اگر ((xp->xp_نمونہ > دلائل->oe_سیٹ_arg && *(xp->xp_نمونہ-1) == ':')
	    || xp->xp_نمونہ == دلائل->oe_سیٹ_arg || *(xp->xp_نمونہ-1) == ',')
    {
	// After a ':' or after a ',', or at the start, so expand highlight
	// group name.

	// If starts with !, then expand 'highlight' occasions.
	اگر (*xp->xp_نمونہ == '!')
	    واپس expand_hl_occasions(دلائل, عدد_Matches, matches, '!');
	ورنہ
	    واپس expand_سیٹ_opt_generic(
		    دلائل,
		    get_highlight_name,
		    عدد_Matches,
		    matches);
    }

    VIM_CLEAR(*matches);
    واپس FAIL;
}

    صحیح
expand_سیٹ_wincolor(optexpand_T *دلائل, صحیح *عدد_Matches, char_u ***matches)
{
    واپس expand_سیٹ_opt_generic(
	    دلائل,
	    get_highlight_name,
	    عدد_Matches,
	    matches);
}

#اگر_تعریف_شدہ FEAT_SYN_HL
/*
 * When the 'syntax' option is set, load the syntax of that name.
 */
    ساکن خالی
do_syntax_autocmd(صحیح قدر_بدلا)
{
    ساکن صحیح syn_recursive = 0;

    ++syn_recursive;
    // Only pass TRUE for "force" when the value changed or not used
    // recursively, to avoid endless recurrence.
    apply_autocmds(EVENT_SYNTAX, curbuf->b_p_syn, curbuf->b_fname,
	    قدر_بدلا || syn_recursive == 1, curbuf);
    curbuf->b_flags |= BF_SYN_سیٹ;
    --syn_recursive;
}
#اگر_ختم

/*
 * When the 'filetype' option is set, trigger the FileType autocommand.
 */
    ساکن خالی
do_filetype_autocmd(char_u **varp, صحیح opt_flags, صحیح قدر_بدلا)
{
    // Skip this when called from a modeline and the filetype was already set
    // to this value.
    اگر ((opt_flags & OPT_MODELINE) && !قدر_بدلا)
	واپس;

    ساکن صحیح  ft_recursive = 0;
    صحیح	    secure_save = secure;

    // Reset the secure flag, since the value of 'filetype' has
    // been checked to be safe.
    secure = 0;

    ++ft_recursive;
    curbuf->b_did_filetype = true;
    // Only pass TRUE for "force" when the value changed or not
    // used recursively, to avoid endless recurrence.
    apply_autocmds(EVENT_FILETYPE, curbuf->b_p_ft, curbuf->b_fname,
	    قدر_بدلا || ft_recursive == 1, curbuf);
    --ft_recursive;
    // Just in case the old "curbuf" is now invalid.
    اگر (varp != &(curbuf->b_p_ft))
	varp = تہی;

    secure = secure_save;
}

#اگر_تعریف_شدہ FEAT_SPELL
/*
 * When the 'spelllang' option is set, source the spell/LANG.vim file in
 * 'runtimepath'.
 */
    ساکن خالی
do_spelllang_ماخذ(خالی)
{
    char_u	fname[200];
    char_u	*p;
    char_u	*q = curwin->w_s->b_p_spl;

    // Skip the first name if it is "cjk".
    اگر (STRNCMP(q, "cjk,", 4) == 0)
	q += 4;

    // They could set 'spellcapcheck' depending on the language.  Use the first
    // name in 'spelllang' up to '_region' or '.encoding'.
    برائے (p = q; *p != NUL; ++p)
	اگر (!ASCII_ISALNUM(*p) && *p != '-')
	    توڑو;
    اگر (p > q)
    {
	vim_snprintf((حرف *)fname, 200, "spell/%.*s.vim",
		(صحیح)(p - q), q);
	ماخذ_runtime(fname, DIP_ALL);
    }
}
#اگر_ختم

/*
 * Handle string options that need some action to perform when changed.
 * The new value must be allocated.
 * Returns NULL for success, or an untranslated error message for an error.
 */
    حرف *
did_سیٹ_string_option(
    صحیح		opt_idx,		// index in options[] table
    char_u	**varp,			// pointer to the option variable
    char_u	*oldval,		// previous value of the option
    char_u	*قدر,			// new value of the option
    حرف	*errbuf,		// buffer for errors, or NULL
    size_t	errbuflen,		// length of error buffer
    صحیح		opt_flags,		// OPT_LOCAL and/or OPT_GLOBAL
    سیٹ_op_T    op,			// OP_ADDING/OP_PREPENDING/OP_REMOVING
    صحیح		*قدر_checked)		// value was checked to be safe, no
					// need to set P_INSECURE
{
    حرف	*errmsg = تہی;
    long_u	free_oldval = (get_option_flags(opt_idx) & P_ALLOCED);
    opt_did_سیٹ_cb_T did_سیٹ_cb = get_option_did_سیٹ_cb(opt_idx);
    optset_T	دلائل;

    // 'ttytype' is an alias for 'term'.  Both 'term' and 'ttytype' point to
    // T_NAME.  If 'term' or 'ttytype' is modified, then use the index for the
    // 'term' option.  Only set the P_ALLOCED flag on 'term'.
    اگر (varp == &T_NAME)
    {
	opt_idx = findoption((char_u *)"term");
	اگر (opt_idx >= 0)
	{
	    free_oldval = (get_option_flags(opt_idx) & P_ALLOCED);
	    did_سیٹ_cb = get_option_did_سیٹ_cb(opt_idx);
	}
    }

    CLEAR_میدان(دلائل);

    // Disallow changing some options from secure mode
    اگر ((secure
#اگر_تعریف_شدہ HAVE_SANDBOX
		|| sandbox != 0
#اگر_ختم
		) && (get_option_flags(opt_idx) & P_SECURE))
	errmsg = e_not_allowed_یہاں;
    // Check for a "normal" directory or file name in some options.
    ورنہ اگر (check_illegal_راستہ_نام(opt_idx, varp))
	errmsg = e_invalid_دلیل;
    ورنہ اگر (did_سیٹ_cb != تہی)
    {
	دلائل.os_varp = (char_u *)varp;
	دلائل.os_idx = opt_idx;
	دلائل.os_flags = opt_flags;
	دلائل.os_op = op;
	دلائل.os_oldval.string = oldval;
	دلائل.os_newval.string = قدر;
	دلائل.os_errbuf = errbuf;
	دلائل.os_errbuflen = errbuflen;
	// Invoke the option specific callback function to validate and apply
	// the new option value.
	errmsg = did_سیٹ_cb(&دلائل);

	// The 'keymap', 'filetype' and 'syntax' option callback functions
	// may change the os_value_checked field.
	*قدر_checked = دلائل.os_قدر_checked;
    }

    // If an error is detected, restore the previous value.
    اگر (errmsg != تہی)
    {
	free_string_option(*varp);
	*varp = oldval;
	// When resetting some values, need to act on it.
	اگر (دلائل.os_restore_chartab)
	    (خالی)init_chartab();
	اگر (varp == &p_hl)
	    (خالی)highlight_بدلا();
    }
    ورنہ
    {
#اگر_تعریف_شدہ FEAT_EVAL
	// Remember where the option was set.
	سیٹ_option_sctx_idx(opt_idx, opt_flags, current_sctx);
#اگر_ختم
	// Free string options that are in allocated memory.
	// Use "free_oldval", because recursiveness may change the flags under
	// our fingers (esp. init_highlight()).
	اگر (free_oldval)
	    free_string_option(oldval);
	سیٹ_option_flag(opt_idx, P_ALLOCED);

	اگر ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0
		&& is_global_local_option(opt_idx))
	{
	    // global option with local value set to use global value; free
	    // the local value and make it empty
	    char_u *p = get_option_varp_scope(opt_idx, OPT_LOCAL);
	    free_string_option(*(char_u **)p);
	    *(char_u **)p = خالی_خانے_option;
	}

	// May set global value for local option.
	ورنہ اگر (!(opt_flags & OPT_LOCAL) && opt_flags != OPT_GLOBAL)
	    سیٹ_string_option_global(opt_idx, varp);

	// Trigger the autocommand only after setting the flags.
#اگر_تعریف_شدہ FEAT_SYN_HL
	اگر (varp == &(curbuf->b_p_syn))
	    do_syntax_autocmd(دلائل.os_قدر_بدلا);
#اگر_ختم
	ورنہ اگر (varp == &(curbuf->b_p_ft))
	    do_filetype_autocmd(varp, opt_flags, دلائل.os_قدر_بدلا);
#اگر_تعریف_شدہ FEAT_SPELL
	اگر (varp == &(curwin->w_s->b_p_spl))
	    do_spelllang_ماخذ();
#اگر_ختم
    }

    اگر (varp == &p_mouse)
    {
	اگر (*p_mouse == NUL)
	    mch_setmouse(FALSE);    // switch mouse off
	ورنہ
	    setmouse();		    // in case 'mouse' changed
    }

    اگر (varp == &p_rtp)
    {
	export_myvimdir();
#اگر defined(FEAT_LUA)
	update_package_paths_اندر_lua();
#اگر_ختم
    }

#اگر defined(FEAT_LINEBREAK)
    // Changing Formatlistpattern when briopt includes the list setting:
    // redraw
    اگر ((varp == &p_flp || varp == &(curbuf->b_p_flp))
	    && curwin->w_briopt_list)
	redraw_all_later(UPD_NOT_VALID);
#اگر_ختم

    اگر (curwin->w_curswant != MAXCOL
		   && (get_option_flags(opt_idx) & (P_CURSWANT | P_RALL)) != 0
				&& (get_option_flags(opt_idx) & P_HLONLY) == 0)
	curwin->w_سیٹ_curswant = true;

    اگر ((opt_flags & OPT_NO_REDRAW) == 0)
    {
#اگر_تعریف_شدہ FEAT_GUI
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
	اگر (!redraw_gui_only || gui.اندر_use)
#اگر_ختم
	    check_redraw(get_option_flags(opt_idx));
    }

#اگر defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
    اگر (دلائل.os_did_swaptcap)
    {
	سیٹ_termname((char_u *)"win32");
	init_highlight(TRUE, FALSE);
    }
#اگر_ختم

    واپس errmsg;
}

/*
 * Check an option that can be a range of string values.
 *
 * Return OK for correct value, FAIL otherwise.
 * Empty is always OK.
 */
    ساکن صحیح
check_opt_لڑیاں(
    char_u	*val,
    حرف	**اقدار,
    صحیح		list)	    // when TRUE: accept a list of values
{
    واپس opt_لڑیاں_flags(val, اقدار, تہی, list);
}

/*
 * Handle an option that can be a range of string values.
 * Set a flag in "*flagp" for each string present.
 *
 * Return OK for correct value, FAIL otherwise.
 * Empty is always OK.
 */
    ساکن صحیح
opt_لڑیاں_flags(
    char_u	*val,		// new value
    حرف	**اقدار,	// array of valid string values
    غیرعلامتی	*flagp,
    صحیح		list)		// when TRUE: accept a list of values
{
    صحیح		i;
    صحیح		len;
    غیرعلامتی	new_flags = 0;

    جب (*val)
    {
	برائے (i = 0; ; ++i)
	{
	    اگر (اقدار[i] == تہی)	// val not found in values[]
		واپس FAIL;

	    len = (صحیح)STRLEN(اقدار[i]);
	    اگر (STRNCMP(اقدار[i], val, len) == 0
		    && ((list && val[len] == ',') || val[len] == NUL))
	    {
		val += len + (val[len] == ',');
		new_flags |= (1 << i);
		توڑو;		// check next item in val list
	    }
	}
    }
    اگر (flagp != تہی)
	*flagp = new_flags;

    واپس OK;
}

/*
 * return OK if "p" is a valid fileformat name, FAIL otherwise.
 */
    صحیح
check_ff_قدر(char_u *p)
{
    واپس check_opt_لڑیاں(p, p_ff_اقدار, FALSE);
}

/*
 * Save the actual shortmess Flags and clear them temporarily to avoid that
 * file messages overwrites any output from the following commands.
 *
 * Caller must make sure to first call save_clear_shm_value() and then
 * restore_shm_value() exactly the same number of times.
 */
    خالی
save_clear_shm_قدر(خالی)
{
    اگر (STRLEN(p_shm) >= SHM_LEN)
    {
	iemsg(e_اندرونی_error_shortmess_too_long);
	واپس;
    }

    اگر (++سیٹ_shm_recursive == 1)
    {
	STRCPY(shm_buf, p_shm);
	سیٹ_option_قدر_give_err((char_u *)"shm", 0L, (char_u *)"", 0);
    }
}

/*
 * Restore the shortmess Flags set from the save_clear_shm_value() function.
 */
    خالی
restore_shm_قدر(خالی)
{
    اگر (--سیٹ_shm_recursive == 0)
    {
	سیٹ_option_قدر_give_err((char_u *)"shm", 0L, shm_buf, 0);
	vim_memset(shm_buf, 0, SHM_LEN);
    }
}

/*
 * Export the environment variable $MYVIMDIR to the first item in runtimepath
 */
    خالی
export_myvimdir(خالی)
{
    صحیح		dofree = FALSE;
    char_u	*p;
    char_u	*q = p_rtp;
    string_T	buf;

    buf.string = alloc(MAXPATHL);
    اگر (buf.string == تہی)
	واپس;

    buf.لمبائی = (size_t)copy_option_part(&q, buf.string, MAXPATHL, ",");

    p = vim_getenv((char_u *)"MYVIMDIR", &dofree);

    اگر (p == تہی || STRCMP(p, buf.string) != 0)
    {
	اگر (*buf.string != NUL && !after_pathsep(buf.string, buf.string + buf.لمبائی))
	    STRCPY(buf.string + buf.لمبائی, PATHSEPSTR);
#اگر_تعریف_شدہ MSWIN
	// normalize path separators
	برائے (q = buf.string; *q != NUL; q++)
	    اگر (*q == '/')
		*q = '\\';
#اگر_ختم
	vim_setenv((char_u *)"MYVIMDIR", buf.string);
    }
    اگر (dofree)
	vim_free(p);
    vim_free(buf.string);
}
