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
ساكن صحيح سیٹ_shm_recursive = 0;

ساكن حرف *(p_ambw_اقدار[]) = {"single", "double", عدم};
ساكن حرف *(p_bg_اقدار[]) = {"light", "dark", عدم};
ساكن حرف *(p_bkc_اقدار[]) = {"yes", "auto", "no", "breaksymlink", "breakhardlink", عدم};
ساكن حرف *(p_bo_اقدار[]) = {"all", "backspace", "cursor", "complete",
				 "copy", "ctrlg", "error", "esc", "ex",
				 "hangul", "insertmode", "lang", "mess",
				 "showmatch", "operator", "register", "shell",
				 "spell", "term", "wildmode", عدم};
#إذا defined(FEAT_LINEBREAK)
// Note: Keep this in sync with briopt_check()
ساكن حرف *(p_briopt_اقدار[]) = {"shift:", "min:", "sbr", "list:", "column:", عدم};
#جے_انت
#إذا defined(FEAT_TABPANEL)
// Note: Keep this in sync with tabpanelopt_changed()
ساكن حرف *(p_tplo_اقدار[]) = {"align:", "columns:", "scrollbar", "vert", عدم};
ساكن حرف *(p_tplo_align_اقدار[]) = {"left", "right", عدم};
#جے_انت
#إذا defined(FEAT_DIFF)
// Note: Keep this in sync with diffopt_changed()
ساكن حرف *(p_dip_اقدار[]) = {"filler", "anchor", "context:", "iblank", "icase", "iwhite", "iwhiteall", "iwhiteeol", "horizontal", "vertical", "closeoff", "hiddenoff", "foldcolumn:", "followwrap", "internal", "indent-heuristic", "algorithm:", "inline:", "linematch:", عدم};
ساكن حرف *(p_dip_algorithm_اقدار[]) = {"myers", "minimal", "patience", "histogram", عدم};
ساكن حرف *(p_dip_inline_اقدار[]) = {"none", "simple", "char", "word", عدم};
#جے_انت
ساكن حرف *(p_nf_اقدار[]) = {"bin", "octal", "hex", "alpha", "unsigned", "blank", عدم};
ساكن حرف *(p_ff_اقدار[]) = {FF_UNIX, FF_DOS, FF_MAC, عدم};
#جے_تعریف_شدہ HAVE_CLIPMETHOD
// Note: Keep this in sync with did_set_clipboard()
ساكن حرف *(p_cb_اقدار[]) = {"unnamed", "unnamedplus",
# ifdef FEAT_CLIPBOARD
    "autoselect", "autoselectplus", "autoselectml", "html", "exclude:",
# endif
    عدم};
#جے_انت
#جے_تعریف_شدہ FEAT_CRYPT
ساكن حرف *(p_cm_اقدار[]) = {"zip", "blowfish", "blowfish2",
# ifdef FEAT_SODIUM
    "xchacha20", "xchacha20v2",
# endif
    عدم};
#جے_انت
ساكن حرف *(p_cmp_اقدار[]) = {"internal", "keepascii", عدم};
#جے_تعریف_شدہ FEAT_SYN_HL
// Note: Keep this in sync with fill_culopt_flags()
ساكن حرف *(p_culopt_اقدار[]) = {"line", "screenline", "number", "both", عدم};
#جے_انت
ساكن حرف *(p_dy_اقدار[]) = {"lastline", "truncate", "uhex", عدم};
ساكن حرف *(p_jop_اقدار[]) = {"stack", عدم};
#جے_تعریف_شدہ FEAT_FOLDING
ساكن حرف *(p_fdo_اقدار[]) = {"all", "block", "hor", "mark", "percent",
				 "quickfix", "search", "tag", "insert",
				 "undo", "jump", عدم};
#جے_انت
// Note: Keep this in sync with match_keyprotocol()
ساكن حرف *(p_kpc_protocol_اقدار[]) = {"none", "mok2", "kitty", عدم};
#جے_تعریف_شدہ FEAT_IMAGE
// Note: Keep this in sync with match_imageprotocol()
ساكن حرف *(p_ipc_protocol_اقدار[]) = {"none", "kitty", "sixel", عدم};
#جے_انت
#جے_تعریف_شدہ FEAT_PROP_POPUP
// Note: Keep this in sync with parse_popup_option()
ساكن حرف *(p_popup_cpp_option_اقدار[]) = {"align:", "border:",
    "borderhighlight:", "close:", "height:", "highlight:", "opacity:",
    "resize:", "shadow:", "width:", عدم};
ساكن حرف *(p_popup_pvp_option_اقدار[]) = {"border:",
    "borderhighlight:", "close:", "height:", "highlight:", "opacity:",
    "resize:", "shadow:", "width:", عدم};
ساكن حرف *(p_popup_option_on_off_اقدار[]) = {"on", "off", عدم};
ساكن حرف *(p_popup_cpp_border_اقدار[]) = {"single", "double", "round",
    "ascii", "on", "off", "custom:", عدم};
ساكن حرف *(p_popup_pvp_border_اقدار[]) = {"single", "double", "round",
    "ascii", "on", "off", "custom:", عدم};
ساكن حرف *(p_popup_option_align_اقدار[]) = {"item", "menu", عدم};
#جے_انت
#إذا defined(FEAT_SPELL)
// Note: Keep this in sync with spell_check_sps()
ساكن حرف *(p_sps_اقدار[]) = {"best", "fast", "double", "expr:", "file:", "timeout:", عدم};
#جے_انت
#جے_تعریف_شدہ FEAT_SESSION
// Also used for 'viewoptions'!  Keep in sync with SSOP_ flags.
ساكن حرف *(p_ssop_اقدار[]) = {"buffers", "winpos", "resize", "winsize",
    "localoptions", "options", "help", "blank", "globals", "slash", "unix",
    "sesdir", "curdir", "folds", "cursor", "tabpages", "terminal", "skiprtp",
    عدم};
#جے_انت
#إذا defined(FEAT_STL_OPT)
ساكن حرف *(p_stlo_اقدار[]) = {"fixedheight", "maxheight:", عدم};
#جے_انت
// Keep in sync with SWB_ flags in option.h
ساكن حرف *(p_swb_اقدار[]) = {"useopen", "usetab", "split", "newtab", "vsplit", "uselast", عدم};
ساكن حرف *(p_spk_اقدار[]) = {"cursor", "screen", "topline", عدم};
ساكن حرف *(p_tc_اقدار[]) = {"followic", "ignore", "match", "followscs", "smart", عدم};
// Keep in sync with TCL_ flags in option.h
ساكن حرف *(p_tcl_اقدار[]) = {"left", "uselast", عدم};
#إذا defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
ساكن حرف *(p_toolbar_اقدار[]) = {"text", "icons", "tooltips", "horiz", عدم};
#جے_انت
#إذا defined(FEAT_TOOLBAR) && defined(FEAT_GUI_GTK)
ساكن حرف *(p_tbis_اقدار[]) = {"tiny", "small", "medium", "large", "huge", "giant", عدم};
#جے_انت
#إذا defined(UNIX) || defined(VMS)
ساكن حرف *(p_ttym_اقدار[]) = {"xterm", "xterm2", "dec", "netterm", "jsbterm", "pterm", "urxvt", "sgr", عدم};
#جے_انت
ساكن حرف *(p_ve_اقدار[]) = {"block", "insert", "all", "onemore", "none", "NONE", عدم};
// Note: Keep this in sync with check_opt_wim()
ساكن حرف *(p_wim_اقدار[]) = {"full", "longest", "list", "lastused", "noselect", "noinsert", عدم};
ساكن حرف *(p_wop_اقدار[]) = {"fuzzy", "tagfile", "pum", "exacttext", عدم};
#جے_تعریف_شدہ FEAT_WAK
ساكن حرف *(p_wak_اقدار[]) = {"yes", "menu", "no", عدم};
#جے_انت
ساكن حرف *(p_mousem_اقدار[]) = {"extend", "popup", "popup_setpos", "mac", عدم};
ساكن حرف *(p_sel_اقدار[]) = {"inclusive", "exclusive", "old", عدم};
ساكن حرف *(p_slm_اقدار[]) = {"mouse", "key", "cmd", عدم};
ساكن حرف *(p_km_اقدار[]) = {"startsel", "stopsel", عدم};
#جے_تعریف_شدہ FEAT_BROWSE
ساكن حرف *(p_bsdir_اقدار[]) = {"current", "last", "buffer", عدم};
#جے_انت
ساكن حرف *(p_scbopt_اقدار[]) = {"ver", "hor", "jump", عدم};
ساكن حرف *(p_debug_اقدار[]) = {"msg", "throw", "beep", عدم};
ساكن حرف *(p_ead_اقدار[]) = {"both", "ver", "hor", عدم};
ساكن حرف *(p_buftype_اقدار[]) = {"nofile", "nowrite", "quickfix", "help", "terminal", "acwrite", "prompt", "popup", عدم};
ساكن حرف *(p_bufhidden_اقدار[]) = {"hide", "unload", "delete", "wipe", عدم};
ساكن حرف *(p_bs_اقدار[]) = {"indent", "eol", "start", "nostop", عدم};
#جے_تعریف_شدہ FEAT_FOLDING
ساكن حرف *(p_fdm_اقدار[]) = {"manual", "expr", "marker", "indent", "syntax",
# ifdef FEAT_DIFF
				"diff",
# endif
				عدم};
ساكن حرف *(p_fcl_اقدار[]) = {"all", عدم};
#جے_انت
ساكن حرف *(p_cfc_اقدار[]) = {"keyword", "files", "whole_line", عدم};
ساكن حرف *(p_cot_اقدار[]) = {"menu", "menuone", "longest", "preview", "popup", "popuphidden", "noinsert", "noselect", "fuzzy", "nosort", "preinsert", "nearest", عدم};
#جے_تعریف_شدہ BACKSLASH_وچ_FILENAME
ساكن حرف *(p_csl_اقدار[]) = {"slash", "backslash", عدم};
#جے_انت
#جے_تعریف_شدہ FEAT_SIGNS
ساكن حرف *(p_scl_اقدار[]) = {"yes", "no", "auto", "number", عدم};
#جے_انت
#جے_تعریف_شدہ UNIX
ساكن حرف *(p_trz_اقدار[]) = {"inband", "sigwinch", "", عدم};
#جے_انت
#إذا defined(MSWIN) && defined(FEAT_TERMINAL)
ساكن حرف *(p_twt_اقدار[]) = {"winpty", "conpty", "", عدم};
#جے_انت
ساكن حرف *(p_sloc_اقدار[]) = {"last", "statusline", "tabline", عدم};
ساكن حرف *(p_sws_اقدار[]) = {"fsync", "sync", عدم};

ساكن صحيح check_opt_لڑیاں(char_u *val, حرف **اقدار, صحيح list);
ساكن صحيح opt_لڑیاں_flags(char_u *val, حرف **اقدار, بنانشانی *flagp, صحيح list);

/*
 * After setting various option values: recompute variables that depend on
 * option values.
 */
    فراغ
didset_string_options(فراغ)
{
    (فراغ)opt_لڑیاں_flags(p_cmp, p_cmp_اقدار, &cmp_flags, TRUE);
    (فراغ)opt_لڑیاں_flags(p_bkc, p_bkc_اقدار, &bkc_flags, TRUE);
    (فراغ)opt_لڑیاں_flags(p_bo, p_bo_اقدار, &bo_flags, TRUE);
    (فراغ)opt_لڑیاں_flags(p_cot, p_cot_اقدار, &cot_flags, TRUE);
#جے_تعریف_شدہ FEAT_SESSION
    (فراغ)opt_لڑیاں_flags(p_ssop, p_ssop_اقدار, &ssop_flags, TRUE);
    (فراغ)opt_لڑیاں_flags(p_vop, p_ssop_اقدار, &vop_flags, TRUE);
#جے_انت
#جے_تعریف_شدہ FEAT_FOLDING
    (فراغ)opt_لڑیاں_flags(p_fdo, p_fdo_اقدار, &fdo_flags, TRUE);
#جے_انت
    (فراغ)opt_لڑیاں_flags(p_dy, p_dy_اقدار, &dy_flags, TRUE);
    (فراغ)opt_لڑیاں_flags(p_jop, p_jop_اقدار, &jop_flags, TRUE);
    (فراغ)opt_لڑیاں_flags(p_tc, p_tc_اقدار, &tc_flags, FALSE);
    (فراغ)opt_لڑیاں_flags(p_ve, p_ve_اقدار, &ve_flags, TRUE);
#إذا defined(UNIX) || defined(VMS)
    (فراغ)opt_لڑیاں_flags(p_ttym, p_ttym_اقدار, &ttym_flags, FALSE);
#جے_انت
#إذا defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
    (فراغ)opt_لڑیاں_flags(p_toolbar, p_toolbar_اقدار, &toolbar_flags, TRUE);
#جے_انت
#إذا defined(FEAT_TOOLBAR) && defined(FEAT_GUI_GTK)
    (فراغ)opt_لڑیاں_flags(p_tbis, p_tbis_اقدار, &tbis_flags, FALSE);
#جے_انت
    (فراغ)opt_لڑیاں_flags(p_swb, p_swb_اقدار, &swb_flags, TRUE);
    (فراغ)opt_لڑیاں_flags(p_tcl, p_tcl_اقدار, &tcl_flags, TRUE);
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
    سیٹ_vim_var_string(VV_OPTION_OLD, oldval, (صحيح)oldvallen);
    سیٹ_vim_var_string(VV_OPTION_NEW, newval, -1);
    buf_typelen = vim_snprintf_safelen((حرف *)buf_type, حجم(buf_type),
	"%s", (opt_flags & OPT_LOCAL) ? "local" : "global");
    سیٹ_vim_var_string(VV_OPTION_TYPE, buf_type, (صحيح)buf_typelen);
    إذا (opt_flags & OPT_LOCAL)
    {
	سیٹ_vim_var_string(VV_OPTION_COMMAND, (char_u *)"setlocal", STRLEN_LITERAL("setlocal"));
	سیٹ_vim_var_string(VV_OPTION_OLDLOCAL, oldval, (صحيح)oldvallen);
    }
    إذا (opt_flags & OPT_GLOBAL)
    {
	سیٹ_vim_var_string(VV_OPTION_COMMAND, (char_u *)"setglobal", STRLEN_LITERAL("setglobal"));
	سیٹ_vim_var_string(VV_OPTION_OLDGLOBAL, oldval, (صحيح)oldvallen);
    }
    إذا ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	سیٹ_vim_var_string(VV_OPTION_COMMAND, (char_u *)"set", STRLEN_LITERAL("set"));
	سیٹ_vim_var_string(VV_OPTION_OLDLOCAL, oldval_l, -1);
	سیٹ_vim_var_string(VV_OPTION_OLDGLOBAL, oldval_g, -1);
    }
    إذا (opt_flags & OPT_MODELINE)
    {
	سیٹ_vim_var_string(VV_OPTION_COMMAND, (char_u *)"modeline", STRLEN_LITERAL("modeline"));
	سیٹ_vim_var_string(VV_OPTION_OLDLOCAL, oldval, (صحيح)oldvallen);
    }
    apply_autocmds(EVENT_OPTIONSET,
	    get_option_fullname(opt_idx), عدم, FALSE,
	    عدم);
    reset_v_option_vars();
}
#جے_انت

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
#جے_تعریف_شدہ FEAT_لبھو_ID
    check_string_option(&buf->b_p_def);
    check_string_option(&buf->b_p_inc);
# ifdef FEAT_EVAL
    check_string_option(&buf->b_p_inex);
# endif
#جے_انت
#إذا defined(FEAT_EVAL)
    check_string_option(&buf->b_p_inde);
    check_string_option(&buf->b_p_indk);
#جے_انت
#إذا defined(FEAT_BEVAL) && defined(FEAT_EVAL)
    check_string_option(&buf->b_p_bexpr);
#جے_انت
#إذا defined(FEAT_CRYPT)
    check_string_option(&buf->b_p_cm);
#جے_انت
    check_string_option(&buf->b_p_fp);
#إذا defined(FEAT_EVAL)
    check_string_option(&buf->b_p_fex);
#جے_انت
#جے_تعریف_شدہ FEAT_CRYPT
    check_string_option(&buf->b_p_کلید);
#جے_انت
    check_string_option(&buf->b_p_kp);
    check_string_option(&buf->b_p_mps);
    check_string_option(&buf->b_p_fo);
    check_string_option(&buf->b_p_flp);
    check_string_option(&buf->b_p_isk);
    check_string_option(&buf->b_p_com);
    check_string_option(&buf->b_p_cms);
    check_string_option(&buf->b_p_nf);
    check_string_option(&buf->b_p_qe);
#جے_تعریف_شدہ FEAT_SYN_HL
    check_string_option(&buf->b_p_syn);
    check_string_option(&buf->b_s.b_syn_isk);
#جے_انت
#جے_تعریف_شدہ FEAT_SPELL
    check_string_option(&buf->b_s.b_p_spc);
    check_string_option(&buf->b_s.b_p_spf);
    check_string_option(&buf->b_s.b_p_spl);
    check_string_option(&buf->b_s.b_p_spo);
#جے_انت
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
#جے_تعریف_شدہ FEAT_COMPL_FUNC
    check_string_option(&buf->b_p_cfu);
    check_string_option(&buf->b_p_ofu);
    check_string_option(&buf->b_p_tsrfu);
#جے_انت
#جے_تعریف_شدہ FEAT_EVAL
    check_string_option(&buf->b_p_ffu);
    check_string_option(&buf->b_p_tfu);
#جے_انت
#جے_تعریف_شدہ FEAT_KEYMAP
    check_string_option(&buf->b_p_keymap);
#جے_انت
#جے_تعریف_شدہ FEAT_QUICKFIX
    check_string_option(&buf->b_p_gefm);
    check_string_option(&buf->b_p_gp);
    check_string_option(&buf->b_p_mp);
    check_string_option(&buf->b_p_efm);
#جے_انت
    check_string_option(&buf->b_p_ep);
    check_string_option(&buf->b_p_رستہ);
    check_string_option(&buf->b_p_ٹیگ_سوچی);
    check_string_option(&buf->b_p_tc);
    check_string_option(&buf->b_p_dict);
#جے_تعریف_شدہ FEAT_DIFF
    check_string_option(&buf->b_p_dia);
#جے_انت
    check_string_option(&buf->b_p_tsr);
    check_string_option(&buf->b_p_lw);
    check_string_option(&buf->b_p_bkc);
    check_string_option(&buf->b_p_menc);
#جے_تعریف_شدہ FEAT_VARTABS
    check_string_option(&buf->b_p_vsts);
    check_string_option(&buf->b_p_vts);
#جے_انت
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
    إذا (p != خالی_تھاواں_option)
	vim_free(p);
}

    فراغ
clear_string_option(char_u **pp)
{
    إذا (*pp != خالی_تھاواں_option)
	vim_free(*pp);
    *pp = خالی_تھاواں_option;
}

    فراغ
check_string_option(char_u **pp)
{
    إذا (*pp == عدم)
	*pp = خالی_تھاواں_option;
}

/*
 * Set global value for string option when it's a local option.
 */
    ساكن فراغ
سیٹ_string_option_global(
    صحيح		opt_idx,	// option index
    char_u	**varp)		// pointer to option variable
{
    char_u	**p, *s;

    // the global value is always allocated
    إذا (is_کھڑکی_local_option(opt_idx))
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
سیٹ_string_option_سدھا(
    char_u	*name,
    صحيح		opt_idx,
    char_u	*val,
    صحيح		opt_flags,	// OPT_FREE, OPT_LOCAL and/or OPT_GLOBAL
    صحيح		سیٹ_sid بن_ورتیا)
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
	    semsg(_(e_اندرونی_error_str), "set_string_option_direct()");
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
	سیٹ_string_option_global(idx, varp);

    سیٹ_option_flag(idx, P_ALLOCED);

    // When setting both values of a global option with a local value,
    // make the local value empty, so that the global value is used.
    إذا (is_global_local_option(idx) && both)
    {
	free_string_option(*varp);
	*varp = خالی_تھاواں_option;
    }
#جے_تعریف_شدہ FEAT_EVAL
    إذا (سیٹ_sid != SID_NONE)
    {
	sctx_T script_ctx;

	إذا (سیٹ_sid == 0)
	    script_ctx = current_sctx;
	وإلا
	{
	    script_ctx.sc_sid = سیٹ_sid;
	    script_ctx.sc_seq = 0;
	    script_ctx.sc_lnum = 0;
	    script_ctx.sc_نسخہ = 1;
	}
	سیٹ_option_sctx_idx(idx, opt_flags, script_ctx);
    }
#جے_انت
}

/*
 * Like set_string_option_direct(), but for a window-local option in "wp".
 * Blocks autocommands to avoid the old curwin becoming invalid.
 */
    فراغ
سیٹ_string_option_سدھا_وچ_win(
	win_T		*wp,
	char_u		*name,
	صحيح		opt_idx,
	char_u		*val,
	صحيح		opt_flags,
	صحيح		سیٹ_sid)
{
    win_T	*save_curwin = curwin;

    بلاک_autocmds();
    curwin = wp;
    curbuf = curwin->w_بفر;
    سیٹ_string_option_سدھا(name, opt_idx, val, opt_flags, سیٹ_sid);
    curwin = save_curwin;
    curbuf = curwin->w_بفر;
    unblock_autocmds();
}

#إذا defined(FEAT_PROP_POPUP)
/*
 * Like set_string_option_direct(), but for a buffer-local option in "buf".
 * Blocks autocommands to avoid the old curbuf becoming invalid.
 */
    فراغ
سیٹ_string_option_سدھا_وچ_buf(
	buf_T		*buf,
	char_u		*name,
	صحيح		opt_idx,
	char_u		*val,
	صحيح		opt_flags,
	صحيح		سیٹ_sid)
{
    buf_T	*save_curbuf = curbuf;

    بلاک_autocmds();
    curbuf = buf;
    curwin->w_بفر = curbuf;
    سیٹ_string_option_سدھا(name, opt_idx, val, opt_flags, سیٹ_sid);
    curbuf = save_curbuf;
    curwin->w_بفر = curbuf;
    unblock_autocmds();
}
#جے_انت

/*
 * Set a string option to a new value, and handle the effects.
 *
 * Returns NULL on success or an untranslated error message on error.
 */
    حرف *
سیٹ_string_option(
    صحيح		opt_idx,
    char_u	*قدر,
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
#جے_انت
    حرف	*errmsg = عدم;
    صحيح		قدر_checked = FALSE;

    إذا (is_hidden_option(opt_idx))	// don't set hidden option
	أرجع عدم;

    s = vim_strsave(قدر == عدم ? (char_u *)"" : قدر);
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
#جے_انت
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
#جے_انت
    إذا ((errmsg = did_سیٹ_string_option(opt_idx, varp, oldval, قدر, errbuf,
		    errbuflen, opt_flags, OP_NONE, &قدر_checked)) == عدم)
	did_سیٹ_option(opt_idx, opt_flags, TRUE, قدر_checked);

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
#جے_انت
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

#جے_تعریف_شدہ FEAT_STL_OPT
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
#جے_انت

/*
 * Check for a "normal" directory or file name in some options.  Disallow a
 * path separator (slash and/or backslash), wildcards and characters that are
 * often illegal in a file name. Be more permissive if "secure" is off.
 */
    ساكن صحيح
check_illegal_رستہ_ناں(صحيح opt_idx, char_u **varp)
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
did_سیٹ_opt_flags(char_u *val, حرف **اقدار, بنانشانی *flagp, صحيح list)
{
    إذا (opt_لڑیاں_flags(val, اقدار, flagp, list) == FAIL)
	أرجع e_invalid_دلیل;

    أرجع عدم;
}

/*
 * An option that accepts a list of string values is changed.
 * e.g. 'nrformats', 'scrollopt', 'wildoptions', etc.
 */
    ساكن حرف *
did_سیٹ_opt_لڑیاں(char_u *val, حرف **اقدار, صحيح list)
{
    أرجع did_سیٹ_opt_flags(val, اقدار, عدم, list);
}

/*
 * An option which is a list of flags is set.  Valid values are in 'flags'.
 */
    ساكن حرف *
did_سیٹ_option_listflag(
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
expand_سیٹ_opt_string(
	optexpand_T *دلیلاں,
	حرف **اقدار,
	size_t عدد_اقدار,
	صحيح *عدد_Matches,
	char_u ***matches)
{
    char_u	*p;
    regmatch_T	*regmatch = دلیلاں->oe_regmatch;
    صحيح		include_orig_val = دلیلاں->oe_include_orig_val;
    char_u	*option_val = دلیلاں->oe_opt_قدر;

    // Assume numValues is small since they are fixed enums, so just allocate
    // upfront instead of needing two passes to calculate output size.
    *matches = ALLOC_MULT(char_u *, عدد_اقدار + 1);
    إذا (*matches == عدم)
	أرجع FAIL;

    صحيح گنتی = 0;

    إذا (include_orig_val && *option_val != NUL)
    {
	p = vim_strsave(option_val);
	إذا (p == عدم)
	{
	    VIM_CLEAR(*matches);
	    أرجع FAIL;
	}
	(*matches)[گنتی++] = p;
    }

    لكل (حرف **val = اقدار; *val != عدم; val++)
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
		إذا (گنتی == 0)
		{
		    VIM_CLEAR(*matches);
		    أرجع FAIL;
		}
		وإلا
		    اكسر;
	    }
	    (*matches)[گنتی++] = p;
	}
    }
    إذا (گنتی == 0)
    {
	VIM_CLEAR(*matches);
	أرجع FAIL;
    }
    *عدد_Matches = گنتی;
    أرجع OK;
}

ساكن char_u *سیٹ_opt_callback_orig_option = عدم;
ساكن char_u *((*سیٹ_opt_callback_func)(expand_T *, صحيح));

/*
 * Callback used by expand_set_opt_generic to also include the original value
 * as the first item.
 */
    ساكن char_u *
expand_سیٹ_opt_generic_cb(expand_T *xp, صحيح idx)
{
    إذا (idx == 0)
    {
	إذا (سیٹ_opt_callback_orig_option != عدم)
	    أرجع سیٹ_opt_callback_orig_option;
	وإلا
	    أرجع (char_u *)""; // empty strings are ignored
    }
    أرجع سیٹ_opt_callback_func(xp, idx - 1);
}

/*
 * Expand an option with a callback that iterates through a list of possible
 * names using an index.
 */
    ساكن صحيح
expand_سیٹ_opt_generic(
	optexpand_T *دلیلاں,
	char_u *((*func)(expand_T *, صحيح)),
	صحيح *عدد_Matches,
	char_u ***matches)
{
    صحيح ret;

    سیٹ_opt_callback_orig_option = دلیلاں->oe_include_orig_val ?
	دلیلاں->oe_opt_قدر : عدم;
    سیٹ_opt_callback_func = func;

    ret = ExpandGeneric(
	    (char_u*)"", // not using fuzzy as currently EXPAND_STRING_SETTING doesn't use it
	    دلیلاں->oe_xp,
	    دلیلاں->oe_regmatch,
	    matches,
	    عدد_Matches,
	    expand_سیٹ_opt_generic_cb,
	    FALSE);

    سیٹ_opt_callback_orig_option = عدم;
    سیٹ_opt_callback_func = عدم;
    أرجع ret;
}

#إذا defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
ساكن garray_T *expand_cb_ga;
ساكن optexpand_T *expand_cb_دلیلاں;

/*
 * Callback provided to a function in expand_set_opt_callback. Will perform
 * regex matching against the value and add to the list.
 *
 * Returns OK usually. Returns FAIL if it failed to allocate memory, and the
 * caller should terminate the enumeration.
 */
    ساكن صحيح
expand_سیٹ_opt_callback_cb(char_u *val)
{
    regmatch_T	*regmatch = expand_cb_دلیلاں->oe_regmatch;
    expand_T	*xp = expand_cb_دلیلاں->oe_xp;
    garray_T	*ga = expand_cb_ga;
    char_u	*str;

    إذا (val == عدم || *val == NUL)
	أرجع OK;

    إذا (xp->xp_نمونہ[0] != NUL &&
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

    ((char_u **)ga->ga_انکڑے)[ga->ga_len] = str;
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
expand_سیٹ_opt_callback(
	optexpand_T *دلیلاں,
	فراغ (*func)(optexpand_T *, فراغ* params, صحيح (*cb)(char_u *val)),
	فراغ *func_params,
	صحيح *عدد_Matches,
	char_u ***matches)
{
    garray_T	ga;
    صحيح		include_orig_val = دلیلاں->oe_include_orig_val;
    char_u	*option_val = دلیلاں->oe_opt_قدر;

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
	((char_u **)ga.ga_انکڑے)[ga.ga_len] = p;
	++ga.ga_len;
    }

    expand_cb_ga = &ga;
    expand_cb_دلیلاں = دلیلاں;

    func(دلیلاں, func_params, expand_سیٹ_opt_callback_cb);

    expand_cb_ga = عدم;
    expand_cb_دلیلاں = عدم;

    *matches = ga.ga_انکڑے;
    *عدد_Matches = ga.ga_len;
    أرجع OK;
}
#جے_انت

/*
 * Expand an option which is a list of flags.
 */
    ساكن صحيح
expand_سیٹ_opt_listflag(
	optexpand_T *دلیلاں,
	char_u *flags,
	صحيح *عدد_Matches,
	char_u ***matches)
{
    char_u  *p;
    char_u  *option_val = دلیلاں->oe_opt_قدر;
    char_u  *cmdline_val = دلیلاں->oe_سیٹ_arg;
    صحيح	    append = دلیلاں->oe_append;
    صحيح	    include_orig_val = دلیلاں->oe_include_orig_val && (*option_val != NUL);

    size_t عدد_flags = STRLEN(flags);

    // Assume we only have small number of flags, so just allocate max size.
    *matches = ALLOC_MULT(char_u *, عدد_flags + 1);
    إذا (*matches == عدم)
	أرجع FAIL;

    صحيح گنتی = 0;

    إذا (include_orig_val)
    {
	p = vim_strsave(option_val);
	إذا (p == عدم)
	{
	    VIM_CLEAR(*matches);
	    أرجع FAIL;
	}
	(*matches)[گنتی++] = p;
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
		إذا (گنتی == 0)
		{
		    VIM_CLEAR(*matches);
		    أرجع FAIL;
		}
		وإلا
		    اكسر;
	    }
	    (*matches)[گنتی++] = p;
	}
    }

    إذا (گنتی == 0)
    {
	VIM_CLEAR(*matches);
	أرجع FAIL;
    }
    *عدد_Matches = گنتی;
    أرجع OK;
}

/*
 * The 'ambiwidth' option is changed.
 */
    حرف *
did_سیٹ_ambiwidth(optset_T *دلیلاں بن_ورتیا)
{
    إذا (check_opt_لڑیاں(p_ambw, p_ambw_اقدار, FALSE) != OK)
	أرجع e_invalid_دلیل;

    أرجع check_chars_options();
}

// "name" must be a string literal, the length is computed at compile time.
#تعریف_کرو completing_قدر_for_subopt(دلیلاں, name) \
	  completing_قدر_for_subopt_len(دلیلاں, name, (صحيح)STRLEN_LITERAL(name))

/*
 * Return true when completing the value of the sub-option "name" with length
 * "len", e.g. the value after "close:" in 'completepopup'.
 */
    ساكن bool
completing_قدر_for_subopt_len(optexpand_T *دلیلاں, حرف *name, صحيح len)
{
    char_u  *colon = دلیلاں->oe_xp->xp_نمونہ - 1;
    صحيح	    off = (صحيح)(colon - دلیلاں->oe_سیٹ_arg);

    إذا (off < len)
	أرجع false;
    // The name must follow a comma when it does not start the option value.
    إذا (off > len && *(colon - len - 1) != ',')
	أرجع false;

    أرجع STRNCMP(colon - len, name, len) == 0;
}

    صحيح
expand_سیٹ_ambiwidth(optexpand_T *دلیلاں, صحيح *عدد_Matches, char_u ***matches)
{
    أرجع expand_سیٹ_opt_string(
	    دلیلاں,
	    p_ambw_اقدار,
	    ARRAY_لمائی(p_ambw_اقدار) - 1,
	    عدد_Matches,
	    matches);
}

/*
 * The 'background' option is changed.
 */
    حرف *
did_سیٹ_background(optset_T *دلیلاں)
{
    إذا (check_opt_لڑیاں(p_bg, p_bg_اقدار, FALSE) == FAIL)
	أرجع e_invalid_دلیل;

    إذا (دلیلاں->os_oldval.string != عدم && دلیلاں->os_oldval.string[0] == *p_bg)
	// Value was not changed
	أرجع عدم;

#جے_تعریف_شدہ FEAT_EVAL
    صحيح dark = (*p_bg == 'd');
#جے_انت

    init_highlight(FALSE, FALSE);

#جے_تعریف_شدہ FEAT_EVAL
    إذا (dark != (*p_bg == 'd')
	    && get_var_قدر((char_u *)"g:colors_name") != عدم)
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
#جے_انت
#جے_تعریف_شدہ FEAT_TERMINAL
    رکن_update_colors_all();
#جے_انت

    أرجع عدم;
}

    صحيح
expand_سیٹ_background(optexpand_T *دلیلاں, صحيح *عدد_Matches, char_u ***matches)
{
    أرجع expand_سیٹ_opt_string(
	    دلیلاں,
	    p_bg_اقدار,
	    ARRAY_لمائی(p_bg_اقدار) - 1,
	    عدد_Matches,
	    matches);
}

/*
 * The 'backspace' option is changed.
 */
    حرف *
did_سیٹ_backspace(optset_T *دلیلاں بن_ورتیا)
{
    إذا (VIM_ISDIGIT(*p_bs))
    {
	إذا (*p_bs > '3' || p_bs[1] != NUL)
	    أرجع e_invalid_دلیل;
    }
    وإلا إذا (check_opt_لڑیاں(p_bs, p_bs_اقدار, TRUE) != OK)
	أرجع e_invalid_دلیل;

    أرجع عدم;
}

    صحيح
expand_سیٹ_backspace(optexpand_T *دلیلاں, صحيح *عدد_Matches, char_u ***matches)
{
    أرجع expand_سیٹ_opt_string(
	    دلیلاں,
	    p_bs_اقدار,
	    ARRAY_لمائی(p_bs_اقدار) - 1,
	    عدد_Matches,
	    matches);
}

/*
 * The 'backupcopy' option is changed.
 */
    حرف *
did_سیٹ_backupcopy(optset_T *دلیلاں)
{
    char_u		*bkc = p_bkc;
    بنانشانی صحيح	*flags = &bkc_flags;
    حرف		*errmsg = عدم;

    إذا (دلیلاں->os_flags & OPT_LOCAL)
    {
	bkc = curbuf->b_p_bkc;
	flags = &curbuf->b_bkc_flags;
    }
    وإلا إذا (!(دلیلاں->os_flags & OPT_GLOBAL))
	// When using :set, clear the local flags.
	curbuf->b_bkc_flags = 0;

    إذا ((دلیلاں->os_flags & OPT_LOCAL) && *bkc == NUL)
	// make the local value empty: use the global value
	*flags = 0;
    وإلا
    {
	إذا (opt_لڑیاں_flags(bkc, p_bkc_اقدار, flags, TRUE) != OK)
	    errmsg = e_invalid_دلیل;
	إذا ((((صحيح)*flags & BKC_AUTO) != 0)
		+ (((صحيح)*flags & BKC_YES) != 0)
		+ (((صحيح)*flags & BKC_NO) != 0) != 1)
	{
	    // Must have exactly one of "auto", "yes"  and "no".
	    (فراغ)opt_لڑیاں_flags(دلیلاں->os_oldval.string, p_bkc_اقدار,
								  flags, TRUE);
	    errmsg = e_invalid_دلیل;
	}
    }

    أرجع errmsg;
}

    صحيح
expand_سیٹ_backupcopy(optexpand_T *دلیلاں, صحيح *عدد_Matches, char_u ***matches)
{
    أرجع expand_سیٹ_opt_string(
	    دلیلاں,
	    p_bkc_اقدار,
	    ARRAY_لمائی(p_bkc_اقدار) - 1,
	    عدد_Matches,
	    matches);
}

/*
 * The 'backupext' or the 'patchmode' option is changed.
 */
    حرف *
did_سیٹ_backupext_or_patchmode(optset_T *دلیلاں بن_ورتیا)
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
did_سیٹ_belloff(optset_T *دلیلاں بن_ورتیا)
{
    أرجع did_سیٹ_opt_flags(p_bo, p_bo_اقدار, &bo_flags, TRUE);
}

    صحيح
expand_سیٹ_belloff(optexpand_T *دلیلاں, صحيح *عدد_Matches, char_u ***matches)
{
    أرجع expand_سیٹ_opt_string(
	    دلیلاں,
	    p_bo_اقدار,
	    ARRAY_لمائی(p_bo_اقدار) - 1,
	    عدد_Matches,
	    matches);
}

#إذا defined(FEAT_LINEBREAK)
/*
 * The 'breakat' option is changed.
 */
    حرف *
did_سیٹ_breakat(optset_T *دلیلاں بن_ورتیا)
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
did_سیٹ_breakindentopt(optset_T *دلیلاں)
{
    char_u	**varp = (char_u **)دلیلاں->os_varp;

    إذا (briopt_check(*varp, varp == &curwin->w_p_briopt ? curwin : عدم)
								      == FAIL)
	أرجع e_invalid_دلیل;

    // list setting requires a redraw
    إذا (varp == &curwin->w_p_briopt && curwin->w_briopt_list)
	redraw_all_later(UPD_NOT_VALID);

    أرجع عدم;
}

    صحيح
expand_سیٹ_breakindentopt(
    optexpand_T		*دلیلاں,
    صحيح			*عدد_Matches,
    char_u		***matches)
{
    أرجع expand_سیٹ_opt_string(
	    دلیلاں,
	    p_briopt_اقدار,
	    ARRAY_لمائی(p_briopt_اقدار) - 1,
	    عدد_Matches,
	    matches);
}
#جے_انت

#إذا defined(FEAT_BROWSE)
/*
 * The 'browsedir' option is changed.
 */
    حرف *
did_سیٹ_browsedir(optset_T *دلیلاں بن_ورتیا)
{
    إذا (check_opt_لڑیاں(p_bsdir, p_bsdir_اقدار, FALSE) != OK
	    && !mch_isdir(p_bsdir))
	أرجع e_invalid_دلیل;

    أرجع عدم;
}

    صحيح
expand_سیٹ_browsedir(optexpand_T *دلیلاں, صحيح *عدد_Matches, char_u ***matches)
{
    أرجع expand_سیٹ_opt_string(
	    دلیلاں,
	    p_bsdir_اقدار,
	    ARRAY_لمائی(p_bsdir_اقدار) - 1,
	    عدد_Matches,
	    matches);
}
#جے_انت

/*
 * The 'bufhidden' option is changed.
 */
    حرف *
did_سیٹ_bufhidden(optset_T *دلیلاں بن_ورتیا)
{
    أرجع did_سیٹ_opt_لڑیاں(curbuf->b_p_bh, p_bufhidden_اقدار, FALSE);
}

    صحيح
expand_سیٹ_bufhidden(optexpand_T *دلیلاں, صحيح *عدد_Matches, char_u ***matches)
{
    أرجع expand_سیٹ_opt_string(
	    دلیلاں,
	    p_bufhidden_اقدار,
	    ARRAY_لمائی(p_bufhidden_اقدار) - 1,
	    عدد_Matches,
	    matches);
}

/*
 * The 'buftype' option is changed.
 */
    حرف *
did_سیٹ_buftype(optset_T *دلیلاں بن_ورتیا)
{
    إذا (check_opt_لڑیاں(curbuf->b_p_bt, p_buftype_اقدار, FALSE) != OK)
	أرجع e_invalid_دلیل;

    إذا (curwin->w_حالت_اچائی)
    {
	curwin->w_redr_حالت = true;
	redraw_later(UPD_VALID);
    }
    curbuf->b_help = (curbuf->b_p_bt[0] == 'h');
    redraw_titles();

    أرجع عدم;
}

    صحيح
expand_سیٹ_buftype(optexpand_T *دلیلاں, صحيح *عدد_Matches, char_u ***matches)
{
    أرجع expand_سیٹ_opt_string(
	    دلیلاں,
	    p_buftype_اقدار,
	    ARRAY_لمائی(p_buftype_اقدار) - 1,
	    عدد_Matches,
	    matches);
}

/*
 * The 'casemap' option is changed.
 */
    حرف *
did_سیٹ_casemap(optset_T *دلیلاں بن_ورتیا)
{
    أرجع did_سیٹ_opt_flags(p_cmp, p_cmp_اقدار, &cmp_flags, TRUE);
}

    صحيح
expand_سیٹ_casemap(optexpand_T *دلیلاں, صحيح *عدد_Matches, char_u ***matches)
{
    أرجع expand_سیٹ_opt_string(
	    دلیلاں,
	    p_cmp_اقدار,
	    ARRAY_لمائی(p_cmp_اقدار) - 1,
	    عدد_Matches,
	    matches);
}

#إذا defined(HAVE_CLIPMETHOD)
    صحيح
expand_سیٹ_clipboard(optexpand_T *دلیلاں, صحيح *عدد_Matches, char_u ***matches)
{
    أرجع expand_سیٹ_opt_string(
	    دلیلاں,
	    p_cb_اقدار,
	    ARRAY_لمائی(p_cb_اقدار) - 1,
	    عدد_Matches,
	    matches);
}
#جے_انت

#جے_تعریف_شدہ HAVE_CLIPMETHOD
    حرف *
did_سیٹ_clipmethod(optset_T *دلیلاں بن_ورتیا)
{
    أرجع چنو_clipmethod();
}

    صحيح
expand_سیٹ_clipmethod(optexpand_T *دلیلاں, صحيح *عدد_Matches, char_u ***matches)
{
    // We want to expand using the predefined clipmethod values + clipboard
    // provider names.
    صحيح		نتیجہ;
    حرف	**اقدار;
    صحيح		گنتی, pos = 0, شروع = 0;
# ifdef FEAT_EVAL
    dict_T	*providers = get_vim_var_dict(VV_CLIPPROVIDERS);
# وإلا
    dict_T	*providers = عدم;
# endif
    hashtab_T	*ht = providers == عدم ? عدم : &providers->dv_hashtab;

    گنتی = (ht == عدم ? 0 : ht->ht_ورتیا);
# ifdef FEAT_WAYLAND_CLIPBOARD
    گنتی++;
    شروع++;
# endif
# ifdef FEAT_XCLIPBOARD
    گنتی++;
    شروع++;
# endif
    اقدار = ALLOC_MULT(حرف *, گنتی + 1); // Add NULL terminator too

    إذا (اقدار == عدم)
	أرجع FAIL;

# ifdef FEAT_WAYLAND_CLIPBOARD
    اقدار[pos++] = "wayland";
# endif
# ifdef FEAT_XCLIPBOARD
    اقدار[pos++] = "x11";
# endif

    إذا (ht != عدم)
	لكل (long_u i = 0; i < ht->ht_نقاب + 1; i++)
	{
	    hashitem_T	*hi = ht->ht_array + i;

	    إذا (!HASHITEM_خالی_تھاواں(hi))
		اقدار[pos++] = (حرف *)vim_strsave(hi->hi_کلید);
	}
    اقدار[pos++] = عدم;

    نتیجہ = expand_سیٹ_opt_string(
	    دلیلاں,
	    اقدار,
	    گنتی,
	    عدد_Matches,
	    matches);

    لكل (صحيح i = شروع; i < گنتی; i++)
	vim_free(اقدار[i]);
    vim_free(اقدار);

    أرجع نتیجہ;
}
#جے_انت

/*
 * The global 'listchars' or 'fillchars' option is changed.
 */
    ساكن حرف *
did_سیٹ_global_listfillchars(char_u *val, صحيح opt_lcs, صحيح opt_flags,
						حرف *errbuf, size_t errbuflen)
{
    حرف	*errmsg = عدم;
    char_u	**local_ptr = opt_lcs ? &curwin->w_p_lcs : &curwin->w_p_fcs;

    // only apply the global value to "curwin" when it does not have a
    // local value
    إذا (opt_lcs)
	errmsg = سیٹ_listchars_option(curwin, val,
		**local_ptr == NUL || !(opt_flags & OPT_GLOBAL),
							    errbuf, errbuflen);
    وإلا
	errmsg = سیٹ_fillchars_option(curwin, val,
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
		(فراغ)سیٹ_listchars_option(wp, wp->w_p_lcs, TRUE, عدم, 0);
	}
	وإلا
	{
	    إذا (*wp->w_p_fcs == NUL)
		(فراغ)سیٹ_fillchars_option(wp, wp->w_p_fcs, TRUE, عدم, 0);
	}
    }

    redraw_all_later(UPD_NOT_VALID);

    أرجع عدم;
}

/*
 * The 'fillchars' option or the 'listchars' option is changed.
 */
    حرف *
did_سیٹ_chars_option(optset_T *دلیلاں)
{
    char_u **varp = (char_u **)دلیلاں->os_varp;
    حرف *errmsg = عدم;

    إذا (   varp == &p_lcs		// global 'listchars'
	|| varp == &p_fcs)		// global 'fillchars'
	errmsg = did_سیٹ_global_listfillchars(*varp, varp == &p_lcs,
			  دلیلاں->os_flags, دلیلاں->os_errbuf, دلیلاں->os_errbuflen);
    وإلا إذا (varp == &curwin->w_p_lcs)	// local 'listchars'
	errmsg = سیٹ_listchars_option(curwin, *varp, TRUE,
					  دلیلاں->os_errbuf, دلیلاں->os_errbuflen);
    وإلا إذا (varp == &curwin->w_p_fcs)	// local 'fillchars'
	errmsg = سیٹ_fillchars_option(curwin, *varp, TRUE,
					  دلیلاں->os_errbuf, دلیلاں->os_errbuflen);

    أرجع errmsg;
}

/*
 * Expand 'fillchars' or 'listchars' option value.
 */
    صحيح
expand_سیٹ_chars_option(optexpand_T *دلیلاں, صحيح *عدد_Matches, char_u ***matches)
{
    char_u **varp = (char_u **)دلیلاں->oe_varp;
    صحيح is_lcs = (varp == &p_lcs || varp == &curwin->w_p_lcs);
    أرجع expand_سیٹ_opt_generic(
	    دلیلاں,
	    is_lcs ? get_listchars_name : get_fillchars_name,
	    عدد_Matches,
	    matches);
}

/*
 * The 'cinoptions' option is changed.
 */
    حرف *
did_سیٹ_cinoptions(optset_T *دلیلاں بن_ورتیا)
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
did_سیٹ_colorcolumn(optset_T *دلیلاں)
{
    char_u	**varp = (char_u **)دلیلاں->os_varp;

    أرجع check_colorcolumn(*varp, varp == &curwin->w_p_cc ? curwin : عدم);
}
#جے_انت

/*
 * The 'comments' option is changed.
 */
    حرف *
did_سیٹ_comments(optset_T *دلیلاں)
{
    char_u	**varp = (char_u **)دلیلاں->os_varp;
    char_u	*s;
    حرف	*errmsg = عدم;

    لكل (s = *varp; *s; )
    {
	طالما (*s && *s != ':')
	{
	    إذا (vim_strchr((char_u *)COM_ALL, *s) == عدم
		    && !VIM_ISDIGIT(*s) && *s != '-')
	    {
		errmsg = illegal_char(دلیلاں->os_errbuf, دلیلاں->os_errbuflen, *s);
		اكسر;
	    }
	    ++s;
	}
	إذا (*s++ == NUL)
	    errmsg = e_missing_colon;
	وإلا إذا (*s == ',' || *s == NUL)
	    errmsg = e_zero_لمائی_string;
	إذا (errmsg != عدم)
	    اكسر;
	طالما (*s && *s != ',')
	{
	    إذا (*s == '\\' && s[1] != NUL)
		++s;
	    ++s;
	}
	s = skip_منزل_option_part(s);
    }

    أرجع errmsg;
}

/*
 * The 'commentstring' option is changed.
 */
    حرف *
did_سیٹ_commentstring(optset_T *دلیلاں)
{
    char_u	**varp = (char_u **)دلیلاں->os_varp;

    إذا (**varp != NUL && لڑی_کھوج((حرف *)*varp, "%s") == عدم)
	أرجع e_commentstring_must_be_خالی_تھاواں_or_contain_str;

    أرجع عدم;
}

/*
 * Check if value for 'complete' is valid when 'complete' option is changed.
 */
    حرف *
did_سیٹ_complete(optset_T *دلیلاں)
{
    char_u	**varp = (char_u **)دلیلاں->os_varp;
    char_u	*p, *t;
    char_u	بفر[LSIZE];
    char_u	*buf_ptr;
    char_u	char_before = NUL;
    صحيح		escape;

    لكل (p = *varp; *p; )
    {
	vim_memset(بفر, 0, LSIZE);
	buf_ptr = بفر;
	escape = 0;

	// Extract substring while handling escaped commas
	طالما (*p && (*p != ',' || escape) && buf_ptr < (بفر + LSIZE - 1))
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

	إذا (vim_strchr((char_u *)".wbuksid]tUFo", *بفر) == عدم)
	    أرجع illegal_char(دلیلاں->os_errbuf, دلیلاں->os_errbuflen, *بفر);

	إذا (vim_strchr((char_u *)"ksF", *بفر) == عدم && *(بفر + 1) != NUL
		&& *(بفر + 1) != '^')
	    char_before = *بفر;
	وإلا
	{
	    // Test for a number after '^'
	    إذا ((t = vim_strchr(بفر, '^')) != عدم)
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
	    أرجع illegal_char_after_chr(دلیلاں->os_errbuf, دلیلاں->os_errbuflen,
		    char_before);
	// Skip comma and spaces
	طالما (*p == ',' || *p == ' ')
	    p++;
    }

#جے_تعریف_شدہ FEAT_COMPL_FUNC
    إذا (سیٹ_cpt_callbacks(دلیلاں) != OK)
	أرجع illegal_char_after_chr(دلیلاں->os_errbuf, دلیلاں->os_errbuflen,
		'F');
#جے_انت
    أرجع عدم;
}

    صحيح
expand_سیٹ_complete(optexpand_T *دلیلاں, صحيح *عدد_Matches, char_u ***matches)
{
    ساكن حرف *(p_cpt_اقدار[]) = {
	".", "w", "b", "u", "k", "kspell", "s", "i", "d", "]", "t", "U", "F", "o",
	عدم};
    أرجع expand_سیٹ_opt_string(
	    دلیلاں,
	    p_cpt_اقدار,
	    ARRAY_لمائی(p_cpt_اقدار) - 1,
	    عدد_Matches,
	    matches);
}

/*
 * The 'completeopt' option is changed.
 */
    حرف *
did_سیٹ_completeopt(optset_T *دلیلاں بن_ورتیا)
{
    char_u	*cot = p_cot;
    بنانشانی	*flags = &cot_flags;

    إذا (دلیلاں->os_flags & OPT_LOCAL)
    {
	cot = curbuf->b_p_cot;
	flags = &curbuf->b_cot_flags;
    }
    وإلا إذا (!(دلیلاں->os_flags & OPT_GLOBAL))
	// When using :set, clear the local flags.
	curbuf->b_cot_flags = 0;

    إذا (opt_لڑیاں_flags(cot, p_cot_اقدار, flags, TRUE) != OK)
	أرجع e_invalid_دلیل;

    أرجع عدم;
}

    صحيح
expand_سیٹ_completeopt(optexpand_T *دلیلاں, صحيح *عدد_Matches, char_u ***matches)
{
    أرجع expand_سیٹ_opt_string(
	    دلیلاں,
	    p_cot_اقدار,
	    ARRAY_لمائی(p_cot_اقدار) - 1,
	    عدد_Matches,
	    matches);
}

/*
 * The 'completefuzzycollect' option is changed.
 */
    حرف *
did_سیٹ_completefuzzycollect(optset_T *دلیلاں بن_ورتیا)
{
    إذا (opt_لڑیاں_flags(p_cfc, p_cfc_اقدار, &cfc_flags, TRUE) != OK)
	أرجع e_invalid_دلیل;
    أرجع عدم;
}

    صحيح
expand_سیٹ_completefuzzycollect(
	optexpand_T *دلیلاں,
	صحيح *عدد_Matches,
	char_u ***matches)
{
    أرجع expand_سیٹ_opt_string(
	    دلیلاں,
	    p_cfc_اقدار,
	    ARRAY_لمائی(p_cfc_اقدار) - 1,
	    عدد_Matches,
	    matches);
}

/*
 * The 'completeitemalign' option is changed.
 */
    حرف *
did_سیٹ_completeitemalign(optset_T *دلیلاں بن_ورتیا)
{
    char_u	*p = p_cia;
    بنانشانی	new_cia_flags = 0;
    صحيح		تکیا[3] = { FALSE, FALSE, FALSE };
    صحيح		گنتی = 0;
    char_u	buf[10];

    طالما (*p)
    {
	copy_option_part(&p, buf, حجم(buf), ",");
	إذا (گنتی >= 3)
	    أرجع e_invalid_دلیل;

	إذا (STRCMP(buf, "abbr") == 0)
	{
	    إذا (تکیا[CPT_ABBR])
		أرجع e_invalid_دلیل;
	    new_cia_flags = new_cia_flags * 10 + CPT_ABBR;
	    تکیا[CPT_ABBR] = TRUE;
	    گنتی++;
	}
	وإلا إذا (STRCMP(buf, "kind") == 0)
	{
	    إذا (تکیا[CPT_قسم])
		أرجع e_invalid_دلیل;
	    new_cia_flags = new_cia_flags * 10 + CPT_قسم;
	    تکیا[CPT_قسم] = TRUE;
	    گنتی++;
	}
	وإلا إذا (STRCMP(buf, "menu") == 0)
	{
	    إذا (تکیا[CPT_MENU])
		أرجع e_invalid_دلیل;
	    new_cia_flags = new_cia_flags * 10 + CPT_MENU;
	    تکیا[CPT_MENU] = TRUE;
	    گنتی++;
	}
	وإلا
	    أرجع e_invalid_دلیل;
    }
    إذا (new_cia_flags == 0 || گنتی != 3)
	أرجع e_invalid_دلیل;

    cia_flags = new_cia_flags;
    أرجع عدم;
}

#إذا defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
/*
 * The 'completepopup' option is changed.
 */
    حرف *
did_سیٹ_completepopup(optset_T *دلیلاں بن_ورتیا)
{
    إذا (parse_completepopup(عدم) == FAIL)
	أرجع e_invalid_دلیل;

    popup_close_info();
    أرجع عدم;
}
#جے_انت

#إذا defined(BACKSLASH_وچ_FILENAME)
/*
 * The 'completeslash' option is changed.
 */
    حرف *
did_سیٹ_completeslash(optset_T *دلیلاں بن_ورتیا)
{
    إذا (check_opt_لڑیاں(p_csl, p_csl_اقدار, FALSE) != OK
	    || check_opt_لڑیاں(curbuf->b_p_csl, p_csl_اقدار, FALSE) != OK)
	أرجع e_invalid_دلیل;

    أرجع عدم;
}

    صحيح
expand_سیٹ_completeslash(optexpand_T *دلیلاں, صحيح *عدد_Matches, char_u ***matches)
{
    أرجع expand_سیٹ_opt_string(
	    دلیلاں,
	    p_csl_اقدار,
	    ARRAY_لمائی(p_csl_اقدار) - 1,
	    عدد_Matches,
	    matches);
}
#جے_انت

#إذا defined(FEAT_CONCEAL)
/*
 * The 'concealcursor' option is changed.
 */
    حرف *
did_سیٹ_concealcursor(optset_T *دلیلاں)
{
    char_u	**varp = (char_u **)دلیلاں->os_varp;

    أرجع did_سیٹ_option_listflag(*varp, (char_u *)COCU_ALL, دلیلاں->os_errbuf,
		    دلیلاں->os_errbuflen);
}

    صحيح
expand_سیٹ_concealcursor(optexpand_T *دلیلاں, صحيح *عدد_Matches, char_u ***matches)
{
    أرجع expand_سیٹ_opt_listflag(دلیلاں, (char_u*)COCU_ALL, عدد_Matches, matches);
}
#جے_انت

/*
 * The 'cpoptions' option is changed.
 */
    حرف *
did_سیٹ_cpoptions(optset_T *دلیلاں)
{
    char_u	**varp = (char_u **)دلیلاں->os_varp;

    أرجع did_سیٹ_option_listflag(*varp, (char_u *)CPO_ALL, دلیلاں->os_errbuf,
		    دلیلاں->os_errbuflen);
}

    صحيح
expand_سیٹ_cpoptions(optexpand_T *دلیلاں, صحيح *عدد_Matches, char_u ***matches)
{
    أرجع expand_سیٹ_opt_listflag(دلیلاں, (char_u*)CPO_ALL, عدد_Matches, matches);
}

#إذا defined(FEAT_CRYPT)
/*
 * The 'cryptkey' option is changed.
 */
    حرف *
did_سیٹ_cryptkey(optset_T *دلیلاں)
{
    // Make sure the ":set" command doesn't show the new value in the
    // history.
    remove_کلید_from_history();

    إذا (دلیلاں->os_op != OP_NONE)
	// Don't allow set+=/-=/^= as they can allow for substring guessing
	أرجع e_invalid_دلیل;

    إذا (STRCMP(curbuf->b_p_کلید, دلیلاں->os_oldval.string) != 0)
    {
	// Need to update the swapfile.
	ml_سیٹ_crypt_کلید(curbuf, دلیلاں->os_oldval.string,
		*curbuf->b_p_cm == NUL ? p_cm : curbuf->b_p_cm);
	بدلیا_اندرونی();
    }
# ifdef FEAT_SODIUM
    إذا (crypt_طریقہ_is_sodium(crypt_get_طریقہ_nr(curbuf)))
	crypt_sodium_جندرا_کلید(دلیلاں->os_newval.string);
# endif

    أرجع عدم;
}

/*
 * The 'cryptmethod' option is changed.
 */
    حرف *
did_سیٹ_cryptmethod(optset_T *دلیلاں)
{
    char_u  *p;
    char_u  *s;

    إذا (دلیلاں->os_flags & OPT_LOCAL)
	p = curbuf->b_p_cm;
    وإلا
	p = p_cm;
    إذا (check_opt_لڑیاں(p, p_cm_اقدار, TRUE) != OK)
	أرجع e_invalid_دلیل;
    وإلا إذا (crypt_self_test() == FAIL)
	أرجع e_invalid_دلیل;

    // When setting the global value to empty, make it "zip".
    إذا (*p_cm == NUL)
    {
	free_string_option(p_cm);
	p_cm = vim_strsave((char_u *)"zip");
    }
    // When using ":set cm=name" the local value is going to be empty.
    // Do that here, otherwise the crypt functions will still use the
    // local value.
    إذا ((دلیلاں->os_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	free_string_option(curbuf->b_p_cm);
	curbuf->b_p_cm = خالی_تھاواں_option;
    }

    // Need to update the swapfile when the effective method changed.
    // Set "s" to the effective old value, "p" to the effective new
    // method and compare.
    إذا ((دلیلاں->os_flags & OPT_LOCAL) && *دلیلاں->os_oldval.string == NUL)
	s = p_cm;  // was previously using the global value
    وإلا
	s = دلیلاں->os_oldval.string;
    إذا (*curbuf->b_p_cm == NUL)
	p = p_cm;  // is now using the global value
    وإلا
	p = curbuf->b_p_cm;
    إذا (STRCMP(s, p) != 0)
	ml_سیٹ_crypt_کلید(curbuf, curbuf->b_p_کلید, s);

    // If the global value changes need to update the swapfile for all
    // buffers using that value.
    إذا ((دلیلاں->os_flags & OPT_GLOBAL)
	    && STRCMP(p_cm, دلیلاں->os_oldval.string) != 0)
    {
	buf_T	*buf;

	FOR_ALL_BUFFERS(buf)
	    إذا (buf != curbuf && *buf->b_p_cm == NUL)
		ml_سیٹ_crypt_کلید(buf, buf->b_p_کلید, دلیلاں->os_oldval.string);
    }
    أرجع عدم;
}

    صحيح
expand_سیٹ_cryptmethod(optexpand_T *دلیلاں, صحيح *عدد_Matches, char_u ***matches)
{
    أرجع expand_سیٹ_opt_string(
	    دلیلاں,
	    p_cm_اقدار,
	    ARRAY_لمائی(p_cm_اقدار) - 1,
	    عدد_Matches,
	    matches);
}
#جے_انت

#إذا defined(FEAT_CSCOPE) && defined(FEAT_QUICKFIX)
/*
 * The 'cscopequickfix' option is changed.
 */
    حرف *
did_سیٹ_cscopequickfix(optset_T *دلیلاں بن_ورتیا)
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
	    أرجع e_invalid_دلیل;
	وإلا إذا (p[2] == NUL)
	    اكسر;
	وإلا
	    p += 3;
    }

    أرجع عدم;
}
#جے_انت

#إذا defined(FEAT_SYN_HL)
/*
 * The 'cursorlineopt' option is changed.
 */
    حرف *
did_سیٹ_cursorlineopt(optset_T *دلیلاں)
{
    char_u	**varp = (char_u **)دلیلاں->os_varp;

    // This could be changed to use opt_strings_flags() instead.
    إذا (**varp == NUL || fill_culopt_flags(*varp, curwin) != OK)
	أرجع e_invalid_دلیل;

    أرجع عدم;
}

    صحيح
expand_سیٹ_cursorlineopt(optexpand_T *دلیلاں, صحيح *عدد_Matches, char_u ***matches)
{
    أرجع expand_سیٹ_opt_string(
	    دلیلاں,
	    p_culopt_اقدار,
	    ARRAY_لمائی(p_culopt_اقدار) - 1,
	    عدد_Matches,
	    matches);
}
#جے_انت

/*
 * The 'debug' option is changed.
 */
    حرف *
did_سیٹ_debug(optset_T *دلیلاں بن_ورتیا)
{
    أرجع did_سیٹ_opt_لڑیاں(p_debug, p_debug_اقدار, TRUE);
}

    صحيح
expand_سیٹ_debug(optexpand_T *دلیلاں, صحيح *عدد_Matches, char_u ***matches)
{
    أرجع expand_سیٹ_opt_string(
	    دلیلاں,
	    p_debug_اقدار,
	    ARRAY_لمائی(p_debug_اقدار) - 1,
	    عدد_Matches,
	    matches);
}

#إذا defined(FEAT_DIFF)
/*
 * The 'diffanchors' option is changed.
 */
    حرف *
did_سیٹ_diffanchors(optset_T *دلیلاں)
{
    إذا (diffanchors_بدلیا(دلیلاں->os_flags & OPT_LOCAL) == FAIL)
	أرجع e_invalid_دلیل;

    أرجع عدم;
}

/*
 * The 'diffopt' option is changed.
 */
    حرف *
did_سیٹ_diffopt(optset_T *دلیلاں بن_ورتیا)
{
    إذا (diffopt_بدلیا() == FAIL)
	أرجع e_invalid_دلیل;

    أرجع عدم;
}

    صحيح
expand_سیٹ_diffopt(optexpand_T *دلیلاں, صحيح *عدد_Matches, char_u ***matches)
{
    expand_T *xp = دلیلاں->oe_xp;

    إذا (xp->xp_نمونہ > دلیلاں->oe_سیٹ_arg && *(xp->xp_نمونہ-1) == ':')
    {
	إذا (completing_قدر_for_subopt(دلیلاں, "algorithm"))
	{
	    أرجع expand_سیٹ_opt_string(
		    دلیلاں,
		    p_dip_algorithm_اقدار,
		    ARRAY_لمائی(p_dip_algorithm_اقدار) - 1,
		    عدد_Matches,
		    matches);
	}
	إذا (completing_قدر_for_subopt(دلیلاں, "inline"))
	{
	    أرجع expand_سیٹ_opt_string(
		    دلیلاں,
		    p_dip_inline_اقدار,
		    ARRAY_لمائی(p_dip_inline_اقدار) - 1,
		    عدد_Matches,
		    matches);
	}
	أرجع FAIL;
    }

    أرجع expand_سیٹ_opt_string(
	    دلیلاں,
	    p_dip_اقدار,
	    ARRAY_لمائی(p_dip_اقدار) - 1,
	    عدد_Matches,
	    matches);
}
#جے_انت

/*
 * The 'display' option is changed.
 */
    حرف *
did_سیٹ_display(optset_T *دلیلاں بن_ورتیا)
{
    إذا (opt_لڑیاں_flags(p_dy, p_dy_اقدار, &dy_flags, TRUE) != OK)
	أرجع e_invalid_دلیل;

    (فراغ)init_chartab();
    أرجع عدم;
}

    صحيح
expand_سیٹ_display(optexpand_T *دلیلاں, صحيح *عدد_Matches, char_u ***matches)
{
    أرجع expand_سیٹ_opt_string(
	    دلیلاں,
	    p_dy_اقدار,
	    ARRAY_لمائی(p_dy_اقدار) - 1,
	    عدد_Matches,
	    matches);
}

/*
 * The 'eadirection' option is changed.
 */
    حرف *
did_سیٹ_eadirection(optset_T *دلیلاں بن_ورتیا)
{
    أرجع did_سیٹ_opt_لڑیاں(p_ead, p_ead_اقدار, FALSE);
}

    صحيح
expand_سیٹ_eadirection(optexpand_T *دلیلاں, صحيح *عدد_Matches, char_u ***matches)
{
    أرجع expand_سیٹ_opt_string(
	    دلیلاں,
	    p_ead_اقدار,
	    ARRAY_لمائی(p_ead_اقدار) - 1,
	    عدد_Matches,
	    matches);
}

/*
 * One of the 'encoding', 'fileencoding', 'termencoding' or 'makeencoding'
 * options is changed.
 */
    حرف *
did_سیٹ_encoding(optset_T *دلیلاں)
{
    char_u	**varp = (char_u **)دلیلاں->os_varp;
    char_u	**gvarp;
    حرف	*errmsg = عدم;
    char_u	*p;

    // Get the global option to compare with, otherwise we would have to check
    // two values for all local options.
    gvarp = (char_u **)get_option_varp_scope(دلیلاں->os_idx, OPT_GLOBAL);

    إذا (gvarp == &p_fenc)
    {
	إذا (!curbuf->b_p_ma && دلیلاں->os_flags != OPT_GLOBAL)
	    errmsg = e_cannot_make_changes_modifiable_is_off;
	وإلا إذا (vim_strchr(*varp, ',') != عدم)
	    // No comma allowed in 'fileencoding'; catches confusing it
	    // with 'fileencodings'.
	    errmsg = e_invalid_دلیل;
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
    إذا (errmsg == عدم && varp == &p_tenc && gui.وچ_use)
    {
	// GTK uses only a single encoding, and that is UTF-8.
	إذا (STRCMP(p_tenc, "utf-8") != 0)
	    errmsg = e_cannot_be_بدلیا_وچ_gtk_GUI;
    }
#جے_انت

    إذا (errmsg == عدم)
    {
#جے_تعریف_شدہ FEAT_KEYMAP
	// When 'keymap' is used and 'encoding' changes, reload the keymap
	// (with another encoding).
	إذا (varp == &p_enc && *curbuf->b_p_keymap != NUL)
	    (فراغ)keymap_init();
#جے_انت

	// When 'termencoding' is not empty and 'encoding' changes or when
	// 'termencoding' changes, need to setup for keyboard input and
	// display output conversion.
	إذا (((varp == &p_enc && *p_tenc != NUL) || varp == &p_tenc))
	{
	    إذا (convert_تیاری(&داخلہ_conv, p_tenc, p_enc) == FAIL
		    || convert_تیاری(&output_conv, p_enc, p_tenc) == FAIL)
	    {
		semsg(_(e_cannot_convert_between_str_and_str),
			p_tenc, p_enc);
		errmsg = e_invalid_دلیل;
	    }
	}

#إذا defined(MSWIN)
	// $HOME, $VIM and $VIMRUNTIME may have characters in active code page.
	إذا (varp == &p_enc)
	{
	    init_homedir();
	    init_vimdir();
	}
#جے_انت
    }

    أرجع errmsg;
}

    صحيح
expand_سیٹ_encoding(optexpand_T *دلیلاں, صحيح *عدد_Matches, char_u ***matches)
{
    أرجع expand_سیٹ_opt_generic(
	    دلیلاں,
	    get_encoding_name,
	    عدد_Matches,
	    matches);
}

/*
 * The 'eventignore(win)' option is changed.
 */
    حرف *
did_سیٹ_eventignore(optset_T *دلیلاں)
{
    char_u	**varp = (char_u **)دلیلاں->os_varp;
    char_u	*oldval = دلیلاں->os_oldval.string;

    إذا (check_ei(*varp) == FAIL)
	أرجع e_invalid_دلیل;

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
	win_T	*wp = is_کھڑکی_local_option(دلیلاں->os_idx) ? curwin : عدم;

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
    صحيح گھٹاؤ = *xp->xp_نمونہ == '-';
    // 'eventignore(win)' allows special keyword "all" in addition to
    // all event names.
    إذا (!گھٹاؤ && idx == 0)
	أرجع (char_u *)"all";

    char_u *name = get_event_name_no_گروہ(xp, idx - 1 + گھٹاؤ, expand_eiw);
    إذا (name == عدم)
	أرجع عدم;

    sprintf((حرف *)IObuff, "%s%s", گھٹاؤ ? "-" : "", name);
    أرجع IObuff;
}

    صحيح
expand_سیٹ_eventignore(optexpand_T *دلیلاں, صحيح *عدد_Matches, char_u ***matches)
{
    expand_eiw = دلیلاں->oe_varp != (char_u *)&p_ei;
    أرجع expand_سیٹ_opt_generic(
	    دلیلاں,
	    get_eventignore_name,
	    عدد_Matches,
	    matches);
}

/*
 * The 'fileformat' option is changed.
 */
    حرف *
did_سیٹ_fileformat(optset_T *دلیلاں)
{
    char_u	**varp = (char_u **)دلیلاں->os_varp;

    إذا (!curbuf->b_p_ma && !(دلیلاں->os_flags & OPT_GLOBAL))
	أرجع e_cannot_make_changes_modifiable_is_off;
    وإلا إذا (check_opt_لڑیاں(*varp, p_ff_اقدار, FALSE) != OK)
	أرجع e_invalid_دلیل;

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
    إذا (get_fileformat(curbuf) == EOL_MAC || *دلیلاں->os_oldval.string == 'm')
	redraw_curbuf_later(UPD_NOT_VALID);

    أرجع عدم;
}

    صحيح
expand_سیٹ_fileformat(optexpand_T *دلیلاں, صحيح *عدد_Matches, char_u ***matches)
{
    أرجع expand_سیٹ_opt_string(
	    دلیلاں,
	    p_ff_اقدار,
	    ARRAY_لمائی(p_ff_اقدار) - 1,
	    عدد_Matches,
	    matches);
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * fileformat options.
 */
    char_u *
get_fileformat_name(expand_T *xp بن_ورتیا, صحيح idx)
{
    إذا (idx >= (صحيح)ARRAY_لمائی(p_ff_اقدار))
	أرجع عدم;

    أرجع (char_u*)p_ff_اقدار[idx];
}

/*
 * The 'fileformats' option is changed.
 */
    حرف *
did_سیٹ_fileformats(optset_T *دلیلاں بن_ورتیا)
{
    إذا (check_opt_لڑیاں(p_ffs, p_ff_اقدار, TRUE) != OK)
	أرجع e_invalid_دلیل;

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
did_سیٹ_filetype_or_syntax(optset_T *دلیلاں)
{
    char_u	**varp = (char_u **)دلیلاں->os_varp;

    إذا (!valid_filetype(*varp))
	أرجع e_invalid_دلیل;

    دلیلاں->os_قدر_بدلیا = STRCMP(دلیلاں->os_oldval.string, *varp) != 0;

    // Since we check the value, there is no need to set P_INSECURE,
    // even when the value comes from a modeline.
    دلیلاں->os_قدر_checked = TRUE;

    أرجع عدم;
}

#إذا defined(FEAT_FOLDING)
/*
 * The 'foldclose' option is changed.
 */
    حرف *
did_سیٹ_foldclose(optset_T *دلیلاں بن_ورتیا)
{
    أرجع did_سیٹ_opt_لڑیاں(p_fcl, p_fcl_اقدار, TRUE);
}

    صحيح
expand_سیٹ_foldclose(optexpand_T *دلیلاں, صحيح *عدد_Matches, char_u ***matches)
{
    أرجع expand_سیٹ_opt_string(
	    دلیلاں,
	    p_fcl_اقدار,
	    ARRAY_لمائی(p_fcl_اقدار) - 1,
	    عدد_Matches,
	    matches);
}
#جے_انت

#إذا defined(FEAT_EVAL) && defined(FEAT_FOLDING)
/*
 * The 'foldexpr' option is changed.
 */
    حرف *
did_سیٹ_foldexpr(optset_T *دلیلاں)
{
    (فراغ)did_سیٹ_optexpr(دلیلاں);
    إذا (foldmethod_Is_اظہاریہ(curwin))
	foldUpdateAll(curwin);
    أرجع عدم;
}
#جے_انت

#إذا defined(FEAT_FOLDING)
/*
 * The 'foldignore' option is changed.
 */
    حرف *
did_سیٹ_foldignore(optset_T *دلیلاں بن_ورتیا)
{
    إذا (foldmethodIsIndent(curwin))
	foldUpdateAll(curwin);
    أرجع عدم;
}

/*
 * The 'foldmarker' option is changed.
 */
    حرف *
did_سیٹ_foldmarker(optset_T *دلیلاں)
{
    char_u	**varp = (char_u **)دلیلاں->os_varp;
    char_u	*p;

    p = vim_strchr(*varp, ',');
    إذا (p == عدم)
	أرجع e_comma_required;
    وإلا إذا (p == *varp || p[1] == NUL)
	أرجع e_invalid_دلیل;
    وإلا إذا (foldmethodIsMarker(curwin))
	foldUpdateAll(curwin);

    أرجع عدم;
}

/*
 * The 'foldmethod' option is changed.
 */
    حرف *
did_سیٹ_foldmethod(optset_T *دلیلاں)
{
    char_u	**varp = (char_u **)دلیلاں->os_varp;

    إذا (check_opt_لڑیاں(*varp, p_fdm_اقدار, FALSE) != OK || **varp == NUL)
	أرجع e_invalid_دلیل;

    foldUpdateAll(curwin);
    إذا (foldmethodIsDiff(curwin))
	newFoldLevel();
    أرجع عدم;
}

    صحيح
expand_سیٹ_foldmethod(optexpand_T *دلیلاں, صحيح *عدد_Matches, char_u ***matches)
{
    أرجع expand_سیٹ_opt_string(
	    دلیلاں,
	    p_fdm_اقدار,
	    ARRAY_لمائی(p_fdm_اقدار) - 1,
	    عدد_Matches,
	    matches);
}

/*
 * The 'foldopen' option is changed.
 */
    حرف *
did_سیٹ_foldopen(optset_T *دلیلاں بن_ورتیا)
{
    أرجع did_سیٹ_opt_flags(p_fdo, p_fdo_اقدار, &fdo_flags, TRUE);
}

    صحيح
expand_سیٹ_foldopen(optexpand_T *دلیلاں, صحيح *عدد_Matches, char_u ***matches)
{
    أرجع expand_سیٹ_opt_string(
	    دلیلاں,
	    p_fdo_اقدار,
	    ARRAY_لمائی(p_fdo_اقدار) - 1,
	    عدد_Matches,
	    matches);
}
#جے_انت

/*
 * The 'formatoptions' option is changed.
 */
    حرف *
did_سیٹ_formatoptions(optset_T *دلیلاں)
{
    char_u	**varp = (char_u **)دلیلاں->os_varp;

    أرجع did_سیٹ_option_listflag(*varp, (char_u *)FO_ALL, دلیلاں->os_errbuf,
		    دلیلاں->os_errbuflen);
}

    صحيح
expand_سیٹ_formatoptions(optexpand_T *دلیلاں, صحيح *عدد_Matches, char_u ***matches)
{
    أرجع expand_سیٹ_opt_listflag(دلیلاں, (char_u*)FO_ALL, عدد_Matches, matches);
}

#إذا defined(CURSOR_SHAPE)
/*
 * The 'guicursor' option is changed.
 */
    حرف *
did_سیٹ_guicursor(optset_T *دلیلاں بن_ورتیا)
{
    أرجع parse_shape_opt(SHAPE_CURSOR);
}
#جے_انت

#إذا defined(FEAT_GUI)
/*
 * The 'guifont' option is changed.
 */
    حرف *
did_سیٹ_guifont(optset_T *دلیلاں بن_ورتیا)
{
    char_u	*p;
    حرف	*errmsg = عدم;

    إذا (gui.وچ_use)
    {
	p = p_guifont;
# إذا defined(FEAT_GUI_GTK)
	// Put up a font dialog and let the user select a new value.
	// If this is cancelled go back to the old value but don't
	// give an error message.
	إذا (STRCMP(p, "*") == 0)
	{
	    p = gui_mch_font_dialog(دلیلاں->os_oldval.string);
	    free_string_option(p_guifont);
	    p_guifont = (p != عدم) ? p : vim_strsave(دلیلاں->os_oldval.string);
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
		p_guifont = vim_strsave(دلیلاں->os_oldval.string);
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
expand_سیٹ_guifont(optexpand_T *دلیلاں, صحيح *عدد_Matches, char_u ***matches)
{
    إذا (!gui.وچ_use)
	أرجع FAIL;

# إذا defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
    char_u **varp = (char_u **)دلیلاں->oe_varp;
    صحيح wide = (varp == &p_guifontwide);

    أرجع expand_سیٹ_opt_callback(
	    دلیلاں, gui_mch_expand_font, &wide, عدد_Matches, matches);
# وإلا
    أرجع FAIL;
# endif
}

# إذا defined(FEAT_XFONTSET)
/*
 * The 'guifontset' option is changed.
 */
    حرف *
did_سیٹ_guifontset(optset_T *دلیلاں بن_ورتیا)
{
    حرف *errmsg = عدم;

    إذا (STRCMP(p_guifontset, "*") == 0)
	errmsg = e_cant_select_fontset;
    وإلا إذا (gui.وچ_use && gui_init_font(p_guifontset, TRUE) != OK)
	errmsg = e_invalid_fontset;

    أرجع errmsg;
}
# endif

/*
 * The 'guifontwide' option is changed.
 */
    حرف *
did_سیٹ_guifontwide(optset_T *دلیلاں بن_ورتیا)
{
    حرف *errmsg = عدم;

    إذا (STRCMP(p_guifontwide, "*") == 0)
	errmsg = e_cant_select_wide_font;
    وإلا إذا (gui_get_wide_font() == FAIL)
	errmsg = e_invalid_wide_font;

    أرجع errmsg;
}
#جے_انت

#إذا defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
/*
 * The 'guiligatures' option is changed.
 */
    حرف *
did_سیٹ_guiligatures(optset_T *دلیلاں بن_ورتیا)
{
    gui_سیٹ_ligatures();
    أرجع عدم;
}
#جے_انت

#إذا defined(FEAT_GUI)
/*
 * The 'guioptions' option is changed.
 */
    حرف *
did_سیٹ_guioptions(optset_T *دلیلاں)
{
    char_u	**varp = (char_u **)دلیلاں->os_varp;
    حرف *errmsg;

    errmsg = did_سیٹ_option_listflag(*varp, (char_u *)GO_ALL, دلیلاں->os_errbuf,
		    دلیلاں->os_errbuflen);
    إذا (errmsg != عدم)
	أرجع errmsg;

    gui_init_which_components(دلیلاں->os_oldval.string);
    أرجع عدم;
}

    صحيح
expand_سیٹ_guioptions(optexpand_T *دلیلاں, صحيح *عدد_Matches, char_u ***matches)
{
    أرجع expand_سیٹ_opt_listflag(دلیلاں, (char_u*)GO_ALL, عدد_Matches, matches);
}
#جے_انت

#إذا defined(FEAT_GUI_TABLINE)
/*
 * The 'guitablabel' option is changed.
 */
    حرف *
did_سیٹ_guitablabel(optset_T *دلیلاں بن_ورتیا)
{
    redraw_tabline = TRUE;
    أرجع عدم;
}
#جے_انت

/*
 * The 'helpfile' option is changed.
 */
    حرف *
did_سیٹ_helpfile(optset_T *دلیلاں بن_ورتیا)
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
did_سیٹ_helplang(optset_T *دلیلاں بن_ورتیا)
{
    حرف *errmsg = عدم;

    // Check for "", "ab", "ab,cd", etc.
    لكل (char_u *s = p_hlg; *s != NUL; s += 3)
    {
	إذا (s[1] == NUL || ((s[2] != ',' || s[3] == NUL) && s[2] != NUL))
	{
	    errmsg = e_invalid_دلیل;
	    اكسر;
	}
	إذا (s[2] == NUL)
	    اكسر;
    }

    أرجع errmsg;
}
#جے_انت

/*
 * The 'highlight' option is changed.
 */
    حرف *
did_سیٹ_highlight(optset_T *دلیلاں بن_ورتیا)
{
    إذا (highlight_بدلیا() == FAIL)
	أرجع e_invalid_دلیل;	// invalid flags

    أرجع عدم;
}

    ساكن صحيح
expand_hl_occasions(
	optexpand_T *دلیلاں,
	صحيح *عدد_Matches,
	char_u ***matches,
	char_u اگیتر)
{
    char_u	    *p;
    ساكن char_u   hl_flags[HLF_گنتی] = HL_FLAGS;
    size_t	    i;
    صحيح		    گنتی = 0;

    *matches = ALLOC_MULT(char_u *, HLF_گنتی + 1);
    إذا (*matches == عدم)
	أرجع FAIL;

    // We still want to return the full option if it's requested.
    إذا (دلیلاں->oe_include_orig_val)
    {
	p = vim_strsave(دلیلاں->oe_opt_قدر);
	إذا (p == عدم)
	{
	    VIM_CLEAR(*matches);
	    أرجع FAIL;
	}
	(*matches)[گنتی++] = p;
    }

    لكل (i = 0; i < HLF_گنتی; i++)
    {
	p = alloc((اگیتر == NUL ? 1 : 2) + 1);
	إذا (p == عدم)
	{
	    إذا (گنتی == 0)
	    {
		VIM_CLEAR(*matches);
		أرجع FAIL;
	    }
	    وإلا
		اكسر;
	}
	إذا (اگیتر == NUL)
	    sprintf((حرف *)p, "%c", hl_flags[i]);
	وإلا
	    sprintf((حرف *)p, "%c%c", اگیتر, hl_flags[i]);
	(*matches)[گنتی++] = p;
    }

    إذا (گنتی == 0)
    {
	VIM_CLEAR(*matches);
	أرجع FAIL;
    }
    *عدد_Matches = گنتی;
    أرجع OK;
}

/*
 * Expand 'highlight' option.
 */
    صحيح
expand_سیٹ_highlight(optexpand_T *دلیلاں, صحيح *عدد_Matches, char_u ***matches)
{
    char_u	    *p;
    expand_T	    *xp = دلیلاں->oe_xp;
    size_t	    i;
    صحيح		    گنتی = 0;

    إذا (xp->xp_نمونہ > دلیلاں->oe_سیٹ_arg && *(xp->xp_نمونہ-1) == ':')
    {
	// Right after a ':', meaning we just return all highlight names.
	أرجع expand_سیٹ_opt_generic(
		دلیلاں,
		get_highlight_name,
		عدد_Matches,
		matches);
    }

    إذا (*xp->xp_نمونہ == NUL)
	// At beginning of a comma-separated list. Return the specific list of
	// supported occasions.
	أرجع expand_hl_occasions(دلیلاں, عدد_Matches, matches, NUL);

    // We are after the initial character (which indicates the occasion). We
    // already made sure we are not matching after a ':' above, so now we want
    // to match against display mode modifiers.
    // Since the xp_pattern starts from the beginning, we need to include it in
    // the returned match.

    // Note: Keep this in sync with highlight_changed()
    ساكن char_u p_hl_منوال_اقدار[] =
	{':', 'b', 'i', '-', 'n', 'r', 's', 'u', 'c', '2', 'd', '=', 't'};
    size_t عدد_hl_modes = ARRAY_لمائی(p_hl_منوال_اقدار);

    *matches = ALLOC_MULT(char_u *, عدد_hl_modes);
    إذا (*matches == عدم)
	أرجع FAIL;

    صحيح نمونہ_len = xp->xp_نمونہ_len;

    لكل (i = 0; i < عدد_hl_modes; i++)
    {
	// Don't allow duplicates as these are unique flags
	char_u *dup = vim_strchr(xp->xp_نمونہ + 1, p_hl_منوال_اقدار[i]);
	إذا (dup != عدم && (صحيح)(dup - xp->xp_نمونہ) < نمونہ_len)
	    استمر;

	// ':' only works by itself, not with other flags.
	إذا (نمونہ_len > 1 && p_hl_منوال_اقدار[i] == ':')
	    استمر;

	p = vim_strnsave(xp->xp_نمونہ, نمونہ_len + 1);
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
	p[نمونہ_len] = p_hl_منوال_اقدار[i];
	p[نمونہ_len + 1] = NUL;
	(*matches)[گنتی++] = p;
    }
    إذا (گنتی == 0)
    {
	VIM_CLEAR(*matches);
	أرجع FAIL;
    }
    *عدد_Matches = گنتی;

    أرجع OK;
}

/*
 * The 'titlestring' or the 'iconstring' option is changed.
 */
    ساكن حرف *
parse_titleiconstring(optset_T *دلیلاں بن_ورتیا, صحيح flagval بن_ورتیا)
{
#جے_تعریف_شدہ FEAT_STL_OPT
    char_u	**varp = (char_u **)دلیلاں->os_varp;

    // NULL => statusline syntax
    إذا (vim_strchr(*varp, '%') && check_stl_option(*varp) == عدم)
	stl_syntax |= flagval;
    وإلا
	stl_syntax &= ~flagval;
#جے_انت
    did_سیٹ_title();

    أرجع عدم;
}

/*
 * The 'iconstring' option is changed.
 */
    حرف *
did_سیٹ_iconstring(optset_T *دلیلاں)
{
    صحيح flagval = 0;

#جے_تعریف_شدہ FEAT_STL_OPT
    flagval = STL_وچ_ICON;
#جے_انت

    أرجع parse_titleiconstring(دلیلاں, flagval);
}

#إذا defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
/*
 * The 'imactivatekey' option is changed.
 */
    حرف *
did_سیٹ_imactivatekey(optset_T *دلیلاں بن_ورتیا)
{
    إذا (!im_xim_isvalid_imactivate())
	أرجع e_invalid_دلیل;
    أرجع عدم;
}
#جے_انت

#جے_تعریف_شدہ FEAT_IMAGE
/*
 * The 'imageprotocol' option is changed.
 */
    حرف *
did_سیٹ_imageprotocol(optset_T *دلیلاں بن_ورتیا)
{
    إذا (update_image_backend() == FAIL)
	أرجع e_invalid_دلیل;
    أرجع عدم;
}

    صحيح
expand_سیٹ_imageprotocol(optexpand_T *دلیلاں, صحيح *عدد_Matches, char_u ***matches)
{
    expand_T *xp = دلیلاں->oe_xp;

    إذا (xp->xp_نمونہ > دلیلاں->oe_سیٹ_arg && *(xp->xp_نمونہ-1) == ':')
    {
	// 'imageprotocol' only has well-defined terms for completion for the
	// protocol part after the colon.
	أرجع expand_سیٹ_opt_string(
		دلیلاں,
		p_ipc_protocol_اقدار,
		ARRAY_لمائی(p_ipc_protocol_اقدار) - 1,
		عدد_Matches,
		matches);
    }
    // Use expand_set_opt_string instead of returning FAIL so that we can
    // include the original value if args->oe_include_orig_val is set.
    ساكن حرف *(خالی_تھاواں_2[]) = {عدم};
    أرجع expand_سیٹ_opt_string(دلیلاں, خالی_تھاواں_2, 0, عدد_Matches, matches);
}
#جے_انت

/*
 * The 'iskeyword' option is changed.
 */
    حرف *
did_سیٹ_iskeyword(optset_T *دلیلاں)
{
    char_u	**varp = (char_u **)دلیلاں->os_varp;

    إذا (varp == &p_isk)		// only check for global-value
    {
	إذا (check_isopt(*varp) == FAIL)
	    أرجع e_invalid_دلیل;
    }
    وإلا			// fallthrough for local-value
	أرجع did_سیٹ_isopt(دلیلاں);

    أرجع عدم;
}

/*
 * The 'isident' or the 'iskeyword' or the 'isprint' or the 'isfname' option is
 * changed.
 */
    حرف *
did_سیٹ_isopt(optset_T *دلیلاں)
{
    // 'isident', 'iskeyword', 'isprint' or 'isfname' option: refill g_chartab[]
    // If the new option is invalid, use old value.
    // 'lisp' option: refill g_chartab[] for '-' char.
    إذا (init_chartab() == FAIL)
    {
	دلیلاں->os_restore_chartab = TRUE;// need to restore the chartab.
	أرجع e_invalid_دلیل;	// error in value
    }

    أرجع عدم;
}

/*
 * The 'jumpoptions' option is changed.
 */
    حرف *
did_سیٹ_jumpoptions(optset_T *دلیلاں بن_ورتیا)
{
    إذا (opt_لڑیاں_flags(p_jop, p_jop_اقدار, &jop_flags, TRUE) != OK)
	أرجع e_invalid_دلیل;

    أرجع عدم;
}

    صحيح
expand_سیٹ_jumpoptions(optexpand_T *دلیلاں, صحيح *عدد_Matches, char_u ***matches)
{
    أرجع expand_سیٹ_opt_string(
	    دلیلاں,
	    p_jop_اقدار,
	    ARRAY_لمائی(p_jop_اقدار) - 1,
	    عدد_Matches,
	    matches);
}

#إذا defined(FEAT_KEYMAP)
/*
 * The 'keymap' option is changed.
 */
    حرف *
did_سیٹ_keymap(optset_T *دلیلاں)
{
    char_u	**varp = (char_u **)دلیلاں->os_varp;
    حرف *errmsg = عدم;

    إذا (!valid_filetype(*varp))
	errmsg = e_invalid_دلیل;
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
	دلیلاں->os_قدر_checked = TRUE;
    }

    إذا (errmsg == عدم)
    {
	إذا (*curbuf->b_p_keymap != NUL)
	{
	    // Installed a new keymap, switch on using it.
	    curbuf->b_p_iminsert = B_IMODE_LMAP;
	    إذا (curbuf->b_p_imsearch != B_IMODE_USE_داخل_کرو)
		curbuf->b_p_imsearch = B_IMODE_LMAP;
	}
	وإلا
	{
	    // Cleared the keymap, may reset 'iminsert' and 'imsearch'.
	    إذا (curbuf->b_p_iminsert == B_IMODE_LMAP)
		curbuf->b_p_iminsert = B_IMODE_NONE;
	    إذا (curbuf->b_p_imsearch == B_IMODE_LMAP)
		curbuf->b_p_imsearch = B_IMODE_USE_داخل_کرو;
	}
	إذا ((دلیلاں->os_flags & OPT_LOCAL) == 0)
	{
	    سیٹ_iminsert_global();
	    سیٹ_imsearch_global();
	}
	حالت_redraw_curbuf();
    }

    أرجع errmsg;
}
#جے_انت

/*
 * The 'keymodel' option is changed.
 */
    حرف *
did_سیٹ_keymodel(optset_T *دلیلاں بن_ورتیا)
{
    إذا (check_opt_لڑیاں(p_km, p_km_اقدار, TRUE) != OK)
	أرجع e_invalid_دلیل;

    km_stopsel = (vim_strchr(p_km, 'o') != عدم);
    km_startsel = (vim_strchr(p_km, 'a') != عدم);
    أرجع عدم;
}

    صحيح
expand_سیٹ_keymodel(optexpand_T *دلیلاں, صحيح *عدد_Matches, char_u ***matches)
{
    أرجع expand_سیٹ_opt_string(
	    دلیلاں,
	    p_km_اقدار,
	    ARRAY_لمائی(p_km_اقدار) - 1,
	    عدد_Matches,
	    matches);
}

/*
 * The 'keyprotocol' option is changed.
 */
    حرف *
did_سیٹ_keyprotocol(optset_T *دلیلاں بن_ورتیا)
{
    char_u *رکن = T_NAME;
    keyprot_T kpc = میل_keyprotocol(رکن);
    إذا (kpc == KEYPROTOCOL_FAIL)
	أرجع e_invalid_دلیل;

    apply_keyprotocol(رکن, kpc);

    أرجع عدم;
}

    صحيح
expand_سیٹ_keyprotocol(optexpand_T *دلیلاں, صحيح *عدد_Matches, char_u ***matches)
{
    expand_T *xp = دلیلاں->oe_xp;
    إذا (xp->xp_نمونہ > دلیلاں->oe_سیٹ_arg && *(xp->xp_نمونہ-1) == ':')
    {
	// 'keyprotocol' only has well-defined terms for completion for the
	// protocol part after the colon.
	أرجع expand_سیٹ_opt_string(
		دلیلاں,
		p_kpc_protocol_اقدار,
		ARRAY_لمائی(p_kpc_protocol_اقدار) - 1,
		عدد_Matches,
		matches);
    }
    // Use expand_set_opt_string instead of returning FAIL so that we can
    // include the original value if args->oe_include_orig_val is set.
    ساكن حرف *(خالی_تھاواں_2[]) = {عدم};
    أرجع expand_سیٹ_opt_string(دلیلاں, خالی_تھاواں_2, 0, عدد_Matches, matches);
}

/*
 * The 'lispoptions' option is changed.
 */
    حرف *
did_سیٹ_lispoptions(optset_T *دلیلاں)
{
    char_u	**varp = (char_u **)دلیلاں->os_varp;

    إذا (**varp != NUL
	    && STRCMP(*varp, "expr:0") != 0
	    && STRCMP(*varp, "expr:1") != 0)
	أرجع e_invalid_دلیل;

    أرجع عدم;
}

    صحيح
expand_سیٹ_lispoptions(optexpand_T *دلیلاں, صحيح *عدد_Matches, char_u ***matches)
{
    ساكن حرف *(p_lop_اقدار[]) = {"expr:0", "expr:1", عدم};
    أرجع expand_سیٹ_opt_string(
	    دلیلاں,
	    p_lop_اقدار,
	    ARRAY_لمائی(p_lop_اقدار) - 1,
	    عدد_Matches,
	    matches);
}

/*
 * The 'matchpairs' option is changed.
 */
    حرف *
did_سیٹ_matchpairs(optset_T *دلیلاں)
{
    char_u	**varp = (char_u **)دلیلاں->os_varp;
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
		أرجع e_invalid_دلیل;
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
		أرجع e_invalid_دلیل;
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
did_سیٹ_messagesopt(optset_T *دلیلاں بن_ورتیا)
{
    إذا (messagesopt_بدلیا() == FAIL)
	أرجع e_invalid_دلیل;

    أرجع عدم;
}

    صحيح
expand_سیٹ_messagesopt(optexpand_T *دلیلاں, صحيح *عدد_Matches, char_u ***matches)
{
    ساكن حرف *(p_mopt_اقدار[]) = {"hit-enter", "wait:", "history:", عدم};
    أرجع expand_سیٹ_opt_string(
	    دلیلاں,
	    p_mopt_اقدار,
	    ARRAY_لمائی(p_mopt_اقدار) - 1,
	    عدد_Matches,
	    matches);
}

#إذا defined(FEAT_SPELL)
/*
 * The 'mkspellmem' option is changed.
 */
    حرف *
did_سیٹ_mkspellmem(optset_T *دلیلاں بن_ورتیا)
{
    إذا (spell_check_msm() != OK)
	أرجع e_invalid_دلیل;

    أرجع عدم;
}
#جے_انت

/*
 * The 'mouse' option is changed.
 */
    حرف *
did_سیٹ_mouse(optset_T *دلیلاں)
{
    char_u	**varp = (char_u **)دلیلاں->os_varp;
    حرف	*retval;

    retval = did_سیٹ_option_listflag(*varp, (char_u *)MOUSE_ALL, دلیلاں->os_errbuf,
		    دلیلاں->os_errbuflen);
    إذا (retval == عدم)
    {
	redraw_tabline = TRUE;
	إذا (tabline_اچائی() > 0)
	    update_screen(UPD_VALID);
#إذا defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
	popup_close_info(); // Close info popup to apply new properties
#جے_انت
    }
    أرجع retval;
}

    صحيح
expand_سیٹ_mouse(optexpand_T *دلیلاں, صحيح *عدد_Matches, char_u ***matches)
{
    أرجع expand_سیٹ_opt_listflag(دلیلاں, (char_u*)MOUSE_ALL, عدد_Matches, matches);
}

/*
 * The 'mousemodel' option is changed.
 */
    حرف *
did_سیٹ_mousemodel(optset_T *دلیلاں بن_ورتیا)
{
    إذا (check_opt_لڑیاں(p_mousem, p_mousem_اقدار, FALSE) != OK)
	أرجع e_invalid_دلیل;
#إذا defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU) && (Xm_نسخہ <= 1002)
    وإلا إذا (*p_mousem != *دلیلاں->os_oldval.string)
	// Changed from "extend" to "popup" or "popup_setpos" or vv: need
	// to create or delete the popup menus.
	gui_motif_update_mousemodel(جڑ_menu);
#جے_انت

    أرجع عدم;
}

    صحيح
expand_سیٹ_mousemodel(optexpand_T *دلیلاں, صحيح *عدد_Matches, char_u ***matches)
{
    أرجع expand_سیٹ_opt_string(
	    دلیلاں,
	    p_mousem_اقدار,
	    ARRAY_لمائی(p_mousem_اقدار) - 1,
	    عدد_Matches,
	    matches);
}

#إذا defined(FEAT_MOUSESHAPE)
    حرف *
did_سیٹ_mouseshape(optset_T *دلیلاں بن_ورتیا)
{
    حرف *errmsg = عدم;

    errmsg = parse_shape_opt(SHAPE_MOUSE);
    update_mouseshape(-1);

    أرجع errmsg;
}
#جے_انت

/*
 * The 'nrformats' option is changed.
 */
    حرف *
did_سیٹ_nrformats(optset_T *دلیلاں)
{
    char_u	**varp = (char_u **)دلیلاں->os_varp;

    أرجع did_سیٹ_opt_لڑیاں(*varp, p_nf_اقدار, TRUE);
}

    صحيح
expand_سیٹ_nrformats(optexpand_T *دلیلاں, صحيح *عدد_Matches, char_u ***matches)
{
    أرجع expand_سیٹ_opt_string(
	    دلیلاں,
	    p_nf_اقدار,
	    ARRAY_لمائی(p_nf_اقدار) - 1,
	    عدد_Matches,
	    matches);
}

#إذا defined(FEAT_EVAL)
/*
 * One of the '*expr' options is changed: 'balloonexpr', 'diffexpr',
 * 'foldexpr', 'foldtext', 'formatexpr', 'includeexpr', 'indentexpr',
 * 'patchexpr', 'printexpr' and 'charconvert'.
 */
    حرف *
did_سیٹ_optexpr(optset_T *دلیلاں)
{
    char_u	**varp = (char_u **)دلیلاں->os_varp;

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
#جے_انت

/*
 * The 'pastetoggle' option is changed.
 */
    حرف *
did_سیٹ_pastetoggle(optset_T *دلیلاں بن_ورتیا)
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
did_سیٹ_previewpopup(optset_T *دلیلاں بن_ورتیا)
{
    إذا (parse_previewpopup(عدم) == FAIL)
	أرجع e_invalid_دلیل;

# إذا defined(FEAT_QUICKFIX)
    popup_close_info();
# endif
    أرجع عدم;
}

    ساكن صحيح
expand_سیٹ_popupoption(optexpand_T *دلیلاں, صحيح *عدد_Matches, char_u ***matches,
	صحيح previewpopup)
{
    expand_T *xp = دلیلاں->oe_xp;

    إذا (xp->xp_نمونہ > دلیلاں->oe_سیٹ_arg && *(xp->xp_نمونہ-1) == ':')
    {
	إذا (completing_قدر_for_subopt(دلیلاں, "close")
		|| completing_قدر_for_subopt(دلیلاں, "resize")
		|| completing_قدر_for_subopt(دلیلاں, "shadow"))
	{
	    أرجع expand_سیٹ_opt_string(
		    دلیلاں,
		    p_popup_option_on_off_اقدار,
		    ARRAY_لمائی(p_popup_option_on_off_اقدار) - 1,
		    عدد_Matches,
		    matches);
	}
	إذا (completing_قدر_for_subopt(دلیلاں, "border"))
	{
	    أرجع expand_سیٹ_opt_string(
		    دلیلاں,
		    previewpopup ? p_popup_pvp_border_اقدار
			: p_popup_cpp_border_اقدار,
		    (previewpopup ? ARRAY_لمائی(p_popup_pvp_border_اقدار) - 1
			: ARRAY_لمائی(p_popup_cpp_border_اقدار)) - 1,
		    عدد_Matches,
		    matches);
	}
	إذا (completing_قدر_for_subopt(دلیلاں, "align"))
	{
	    أرجع expand_سیٹ_opt_string(
		    دلیلاں,
		    p_popup_option_align_اقدار,
		    ARRAY_لمائی(p_popup_option_align_اقدار) - 1,
		    عدد_Matches,
		    matches);
	}
	إذا (completing_قدر_for_subopt(دلیلاں, "highlight")
		|| completing_قدر_for_subopt(دلیلاں, "borderhighlight"))
	{
	    // Return the list of all highlight names
	    أرجع expand_سیٹ_opt_generic(
		    دلیلاں,
		    get_highlight_name,
		    عدد_Matches,
		    matches);
	}
	أرجع FAIL;
    }

    أرجع expand_سیٹ_opt_string(
	    دلیلاں,
	    previewpopup ? p_popup_pvp_option_اقدار
		: p_popup_cpp_option_اقدار,
	    previewpopup ? ARRAY_لمائی(p_popup_pvp_option_اقدار) - 1
		: ARRAY_لمائی(p_popup_cpp_option_اقدار) - 1,
	    عدد_Matches,
	    matches);
}

    صحيح
expand_سیٹ_previewpopup(optexpand_T *دلیلاں, صحيح *عدد_Matches, char_u ***matches)
{
    أرجع expand_سیٹ_popupoption(دلیلاں, عدد_Matches, matches, TRUE);
}

    صحيح
expand_سیٹ_completepopup(optexpand_T *دلیلاں, صحيح *عدد_Matches, char_u ***matches)
{
    أرجع expand_سیٹ_popupoption(دلیلاں, عدد_Matches, matches, FALSE);
}
#جے_انت

#إذا defined(FEAT_POSTSCRIPT)
/*
 * The 'printencoding' option is changed.
 */
    حرف *
did_سیٹ_printencoding(optset_T *دلیلاں بن_ورتیا)
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
#جے_انت

#إذا defined(FEAT_PRINTER)

    ساكن char_u *
get_printoptions_ناں(expand_T *xp بن_ورتیا, صحيح idx)
{
    إذا (idx >= (صحيح)ARRAY_لمائی(printer_opts))
	أرجع عدم;
    أرجع (char_u*)printer_opts[idx].name;
}

/*
 * Expand 'printoptions' option
 */
    صحيح
expand_سیٹ_printoptions(optexpand_T *دلیلاں, صحيح *عدد_Matches, char_u ***matches)
{
    أرجع expand_سیٹ_opt_generic(
	    دلیلاں,
	    get_printoptions_ناں,
	    عدد_Matches,
	    matches);
}
#جے_انت

#إذا defined(FEAT_STL_OPT)
/*
 * The 'statusline' or the 'tabline' or the 'rulerformat' option is changed.
 * "rulerformat" is TRUE if the 'rulerformat' option is changed.
 */
    ساكن حرف *
parse_statustabline_rulerformat(optset_T *دلیلاں, صحيح rulerformat)
{
    char_u	**varp = (char_u **)دلیلاں->os_varp;
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
	comp_خانہ();

    أرجع errmsg;
}
#جے_انت

#إذا defined(FEAT_RENDER_OPTIONS)
/*
 * The 'renderoptions' option is changed.
 */
    حرف *
did_سیٹ_renderoptions(optset_T *دلیلاں بن_ورتیا)
{
    إذا (!gui_mch_سیٹ_rendering_options(p_rop))
	أرجع e_invalid_دلیل;

    أرجع عدم;
}
#جے_انت

#إذا defined(FEAT_RIGHTLEFT)
/*
 * The 'rightleftcmd' option is changed.
 */
    حرف *
did_سیٹ_rightleftcmd(optset_T *دلیلاں)
{
    char_u	**varp = (char_u **)دلیلاں->os_varp;

    // Currently only "search" is a supported value.
    إذا (**varp != NUL && STRCMP(*varp, "search") != 0)
	أرجع e_invalid_دلیل;

    أرجع عدم;
}

    صحيح
expand_سیٹ_rightleftcmd(optexpand_T *دلیلاں, صحيح *عدد_Matches, char_u ***matches)
{
    ساكن حرف *(p_rlc_اقدار[]) = {"search", عدم};
    أرجع expand_سیٹ_opt_string(
	    دلیلاں,
	    p_rlc_اقدار,
	    ARRAY_لمائی(p_rlc_اقدار) - 1,
	    عدد_Matches,
	    matches);
}
#جے_انت

#تعریف_کرو PUM_BORDER_CLEAR()   \
    افعل {                     \
	pum_سیٹ_border(FALSE); \
	pum_سیٹ_shadow(FALSE); \
	pum_سیٹ_margin(FALSE); \
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
	pum_سیٹ_border_chars(0x2500, 0x2502, 0x2500, 0x2502, // ─ │ ─ │
		0x250c, 0x2510, 0x2518, 0x2514); // ┌ ┐ ┘ └
    وإلا إذا (can_use_box_chars && STRCMP(token, "double") == 0)
	pum_سیٹ_border_chars(0x2550, 0x2551, 0x2550, 0x2551, // ═ ║ ═ ║
		0x2554, 0x2557, 0x255D, 0x255A); // ╔ ╗ ╝  ╚
    وإلا إذا (can_use_box_chars && STRCMP(token, "round") == 0)
	pum_سیٹ_border_chars(0x2500, 0x2502, 0x2500, 0x2502, // ─ │ ─ │
		0x256d, 0x256e, 0x256f, 0x2570); // ╭ ╮ ╯ ╰
    وإلا إذا (STRCMP(token, "ascii") == 0)
	pum_سیٹ_border_chars('-', '|', '-', '|', '+', '+', '+', '+');
    وإلا إذا (STRNCMP(token, "custom:", 7) == 0)
    {
	char_u	*q = token + 7;
	صحيح	باہرلا[8];

	لكل (صحيح i = 0; i < 8; i++)
	{
	    إذا (*q == NUL || *q == ',')
	    {
		vim_free(token);
		أرجع FAIL;
	    }
	    باہرلا[i] = mb_ptr2char(q);
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
	pum_سیٹ_border_chars(باہرلا[0], باہرلا[1], باہرلا[2], باہرلا[3], باہرلا[4], باہرلا[5],
		باہرلا[6], باہرلا[7]);
    }
    وإلا
    {
	vim_free(token);
	أرجع FAIL;
    }

    vim_free(token);
    pum_سیٹ_border(TRUE);
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
did_سیٹ_pumopt(optset_T *دلیلاں)
{
    char_u  **varp = (char_u **)دلیلاں->os_varp;
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
	    لما n = atol((حرف *)p + 7);
	    إذا (n < 0)
		اذهب error;
	    p_ph = n;
	}
	وإلا إذا (STRNCMP(p, "width:", 6) == 0)
	{
	    لما n = atol((حرف *)p + 6);
	    إذا (n < 0)
		اذهب error;
	    p_pw = n;
	}
	وإلا إذا (STRNCMP(p, "maxwidth:", 9) == 0)
	{
	    لما n = atol((حرف *)p + 9);
	    إذا (n < 0)
		اذهب error;
	    p_pmw = n;
	}
	وإلا إذا (STRNCMP(p, "opacity:", 8) == 0)
	{
	    لما n = atol((حرف *)p + 8);
	    إذا (n < 0 || n > 100)
		اذهب error;
	    p_po = n;
	}
	وإلا إذا (len == 6 && STRNCMP(p, "shadow", 6) == 0)
	    pum_سیٹ_shadow(TRUE);
	وإلا إذا (len == 6 && STRNCMP(p, "margin", 6) == 0)
	{
	    have_margin = TRUE;
	    pum_سیٹ_margin(TRUE);
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
    pum_opacity_بدلیا();

    أرجع عدم;

error:
    PUM_BORDER_CLEAR();
    p_ph = 0;
    p_pw = 15;
    p_pmw = 0;
    p_po = 100;
    أرجع e_invalid_دلیل;
}

    ساكن char_u *
get_pum_border_style(expand_T *xp بن_ورتیا, صحيح idx)
{
    ساكن حرف *styles[] = {"ascii", "custom:", "single", "double", "round"};
    أرجع idx < ((enc_utf8 && *p_ambw == 's') ? (صحيح)ARRAY_لمائی(styles) : 2)
	    ? (char_u *)styles[idx] : عدم;
}

    صحيح
expand_سیٹ_pumopt(optexpand_T *دلیلاں, صحيح *عدد_Matches, char_u ***matches)
{
    expand_T *xp = دلیلاں->oe_xp;

    إذا (xp->xp_نمونہ > دلیلاں->oe_سیٹ_arg && *(xp->xp_نمونہ-1) == ':')
    {
	إذا (completing_قدر_for_subopt(دلیلاں, "border"))
	{
	    أرجع expand_سیٹ_opt_generic(
		    دلیلاں, get_pum_border_style, عدد_Matches, matches);
	}
	أرجع FAIL;
    }

    ساكن حرف *(p_pumopt_اقدار[]) = {"border:", "height:", "width:",
	"maxwidth:", "opacity:", "shadow", "margin", عدم};
    أرجع expand_سیٹ_opt_string(
	    دلیلاں,
	    p_pumopt_اقدار,
	    ARRAY_لمائی(p_pumopt_اقدار) - 1,
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
did_سیٹ_pumborder(optset_T *دلیلاں)
{
    char_u  **varp = (char_u **)دلیلاں->os_varp;
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
	    pum_سیٹ_shadow(TRUE);
	وإلا إذا (STRNCMP(p, "margin", len) == 0 && len == 6)
	{
	    have_margin = TRUE;
	    pum_سیٹ_margin(TRUE);
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
    pum_سیٹ_border_chars(0, 0, 0, 0, 0, 0, 0, 0);
    أرجع e_invalid_دلیل;
}

    ساكن char_u *
get_pumborder_token(expand_T *xp, صحيح idx)
{
    أرجع idx == 0 ? (char_u *)"margin"
	 : idx == 1 ? (char_u *)"shadow"
	 : get_pum_border_style(xp, idx - 2);
}

    صحيح
expand_سیٹ_pumborder(optexpand_T *دلیلاں, صحيح *عدد_Matches, char_u ***matches)
{
    أرجع expand_سیٹ_opt_generic(
	    دلیلاں, get_pumborder_token, عدد_Matches, matches);
}

#إذا defined(FEAT_STL_OPT)

/*
 * The 'rulerformat' option is changed.
 */
    حرف *
did_سیٹ_rulerformat(optset_T *دلیلاں)
{
    أرجع parse_statustabline_rulerformat(دلیلاں, TRUE);
}
#جے_انت

#إذا defined(FEAT_TABPANEL)
/*
 * Process the new 'tabpanelopt' option value.
 */
    حرف *
did_سیٹ_tabpanelopt(optset_T *دلیلاں بن_ورتیا)
{
    إذا (tabpanelopt_بدلیا() == FAIL)
	أرجع e_invalid_دلیل;

    أرجع عدم;
}

    صحيح
expand_سیٹ_tabpanelopt(optexpand_T *دلیلاں, صحيح *عدد_Matches, char_u ***matches)
{
    expand_T *xp = دلیلاں->oe_xp;

    إذا (xp->xp_نمونہ > دلیلاں->oe_سیٹ_arg && *(xp->xp_نمونہ-1) == ':')
    {
	إذا (completing_قدر_for_subopt(دلیلاں, "align"))
	{
	    أرجع expand_سیٹ_opt_string(
		    دلیلاں,
		    p_tplo_align_اقدار,
		    ARRAY_لمائی(p_tplo_align_اقدار) - 1,
		    عدد_Matches,
		    matches);
	}
	أرجع FAIL;
    }

    أرجع expand_سیٹ_opt_string(
	    دلیلاں,
	    p_tplo_اقدار,
	    ARRAY_لمائی(p_tplo_اقدار) - 1,
	    عدد_Matches,
	    matches);
}
#جے_انت

/*
 * The 'scrollopt' option is changed.
 */
    حرف *
did_سیٹ_scrollopt(optset_T *دلیلاں بن_ورتیا)
{
    أرجع did_سیٹ_opt_لڑیاں(p_sbo, p_scbopt_اقدار, TRUE);
}

    صحيح
expand_سیٹ_scrollopt(optexpand_T *دلیلاں, صحيح *عدد_Matches, char_u ***matches)
{
    أرجع expand_سیٹ_opt_string(
	    دلیلاں,
	    p_scbopt_اقدار,
	    ARRAY_لمائی(p_scbopt_اقدار) - 1,
	    عدد_Matches,
	    matches);
}

/*
 * The 'wlseat' option is changed
 */
    حرف *
did_سیٹ_wlseat(optset_T *دلیلاں بن_ورتیا)
{
#جے_تعریف_شدہ FEAT_WAYLAND_CLIPBOARD
    // If there isn't any seat named 'wlseat', then let the Wayland clipboard be
    // unavailable. Ignore errors returned.
    clip_reset_wayland();
#جے_انت

    أرجع عدم;
}

/*
 * The 'selection' option is changed.
 */
    حرف *
did_سیٹ_selection(optset_T *دلیلاں بن_ورتیا)
{
    إذا (*p_sel == NUL || check_opt_لڑیاں(p_sel, p_sel_اقدار, FALSE) != OK)
	أرجع e_invalid_دلیل;

    أرجع عدم;
}

    صحيح
expand_سیٹ_selection(optexpand_T *دلیلاں, صحيح *عدد_Matches, char_u ***matches)
{
    أرجع expand_سیٹ_opt_string(
	    دلیلاں,
	    p_sel_اقدار,
	    ARRAY_لمائی(p_sel_اقدار) - 1,
	    عدد_Matches,
	    matches);
}

/*
 * The 'selectmode' option is changed.
 */
    حرف *
did_سیٹ_selectmode(optset_T *دلیلاں بن_ورتیا)
{
    أرجع did_سیٹ_opt_لڑیاں(p_slm, p_slm_اقدار, TRUE);
}

    صحيح
expand_سیٹ_selectmode(optexpand_T *دلیلاں, صحيح *عدد_Matches, char_u ***matches)
{
    أرجع expand_سیٹ_opt_string(
	    دلیلاں,
	    p_slm_اقدار,
	    ARRAY_لمائی(p_slm_اقدار) - 1,
	    عدد_Matches,
	    matches);
}

#إذا defined(FEAT_SESSION)
/*
 * The 'sessionoptions' option is changed.
 */
    حرف *
did_سیٹ_sessionoptions(optset_T *دلیلاں)
{
    إذا (opt_لڑیاں_flags(p_ssop, p_ssop_اقدار, &ssop_flags, TRUE) != OK)
	أرجع e_invalid_دلیل;
    إذا ((ssop_flags & SSOP_CURDIR) && (ssop_flags & SSOP_SESDIR))
    {
	// Don't allow both "sesdir" and "curdir".
	(فراغ)opt_لڑیاں_flags(دلیلاں->os_oldval.string, p_ssop_اقدار,
							&ssop_flags, TRUE);
	أرجع e_invalid_دلیل;
    }

    أرجع عدم;
}

    صحيح
expand_سیٹ_sessionoptions(
    optexpand_T		*دلیلاں,
    صحيح			*عدد_Matches,
    char_u		***matches)
{
    أرجع expand_سیٹ_opt_string(
	    دلیلاں,
	    p_ssop_اقدار,
	    ARRAY_لمائی(p_ssop_اقدار) - 1,
	    عدد_Matches,
	    matches);
}
#جے_انت

/*
 * Validate 'shellpipe'/'shellredir' option.
 */
    حرف *
did_سیٹ_shellpipe_redir(optset_T *دلیلاں)
{
    char_u	*p;
    bool	تکیا = false;

    لكل (p = دلیلاں->os_newval.string; *p != NUL; ++p)
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
	    إذا (تکیا)
		أرجع e_invalid_format_string_single_percent_s;

	    تکیا = true;
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
did_سیٹ_shortmess(optset_T *دلیلاں)
{
    char_u	**varp = (char_u **)دلیلاں->os_varp;

    أرجع did_سیٹ_option_listflag(*varp, (char_u *)SHM_ALL, دلیلاں->os_errbuf,
		    دلیلاں->os_errbuflen);
}

    صحيح
expand_سیٹ_shortmess(optexpand_T *دلیلاں, صحيح *عدد_Matches, char_u ***matches)
{
    أرجع expand_سیٹ_opt_listflag(دلیلاں, (char_u*)SHM_ALL, عدد_Matches, matches);
}

#إذا defined(FEAT_LINEBREAK)
/*
 * The 'showbreak' option is changed.
 */
    حرف *
did_سیٹ_showbreak(optset_T *دلیلاں)
{
    char_u	**varp = (char_u **)دلیلاں->os_varp;
    char_u	*s;

    لكل (s = *varp; *s; )
    {
	إذا (ptr2cells(s) != 1)
	    أرجع e_showbreak_شامل_اے_unprintable_or_wide_character;
	MB_PTR_ADV(s);
    }

    أرجع عدم;
}
#جے_انت

/*
 * The 'showcmdloc' option is changed.
 */
    حرف *
did_سیٹ_showcmdloc(optset_T *دلیلاں بن_ورتیا)
{
    حرف	*errmsg = did_سیٹ_opt_لڑیاں(p_sloc, p_sloc_اقدار, FALSE);

    إذا (errmsg == عدم)
	comp_خانہ();

    أرجع errmsg;
}

    صحيح
expand_سیٹ_showcmdloc(optexpand_T *دلیلاں, صحيح *عدد_Matches, char_u ***matches)
{
    أرجع expand_سیٹ_opt_string(
	    دلیلاں,
	    p_sloc_اقدار,
	    ARRAY_لمائی(p_sloc_اقدار) - 1,
	    عدد_Matches,
	    matches);
}

#إذا defined(FEAT_SIGNS)
/*
 * The 'signcolumn' option is changed.
 */
    حرف *
did_سیٹ_signcolumn(optset_T *دلیلاں)
{
    char_u	**varp = (char_u **)دلیلاں->os_varp;

# إذا defined(FEAT_LINEBREAK)
    إذا (check_opt_لڑیاں(*varp, p_scl_اقدار, FALSE) != OK)
	أرجع e_invalid_دلیل;
    // When changing the 'signcolumn' to or from 'number', recompute the
    // width of the number column if 'number' or 'relativenumber' is set.
    إذا (((*دلیلاں->os_oldval.string == 'n' && دلیلاں->os_oldval.string[1] == 'u')
		|| (*curwin->w_p_scl == 'n' && *(curwin->w_p_scl + 1) =='u'))
	    && (curwin->w_p_nu || curwin->w_p_rnu))
	curwin->w_nrwidth_سطر_گنتی = 0;
# endif

# إذا defined(FEAT_GUI)
    // In the GUI, when sign icons are used, a full screen clear is needed
    // to properly redraw the sign icons.
    إذا (gui.وچ_use && curbuf->b_signlist != عدم)
	redraw_all_later(UPD_CLEAR);
# endif

    أرجع عدم;
}

    صحيح
expand_سیٹ_signcolumn(optexpand_T *دلیلاں, صحيح *عدد_Matches, char_u ***matches)
{
    أرجع expand_سیٹ_opt_string(
	    دلیلاں,
	    p_scl_اقدار,
	    ARRAY_لمائی(p_scl_اقدار) - 1,
	    عدد_Matches,
	    matches);
}
#جے_انت

#إذا defined(FEAT_SPELL)
/*
 * The 'spellcapcheck' option is changed.
 */
    حرف *
did_سیٹ_spellcapcheck(optset_T *دلیلاں بن_ورتیا)
{
    // compile the regexp program.
    أرجع compile_cap_prog(curwin->w_s);
}

/*
 * The 'spellfile' option is changed.
 */
    حرف *
did_سیٹ_spellfile(optset_T *دلیلاں)
{
    char_u	**varp = (char_u **)دلیلاں->os_varp;

    إذا (!valid_spellfile(*varp))
	أرجع e_invalid_دلیل;

    // If there is a window for this buffer in which 'spell' is set load the
    // wordlists.
    أرجع did_سیٹ_spell_option();
}

/*
 * The 'spell' option is changed.
 */
    حرف *
did_سیٹ_spelllang(optset_T *دلیلاں)
{
    char_u	**varp = (char_u **)دلیلاں->os_varp;

    إذا (!valid_spelllang(*varp))
	أرجع e_invalid_دلیل;

    // If there is a window for this buffer in which 'spell' is set load the
    // wordlists.
    أرجع did_سیٹ_spell_option();
}

/*
 * The 'spelloptions' option is changed.
 */
    حرف *
did_سیٹ_spelloptions(optset_T *دلیلاں)
{
    char_u	**varp = (char_u **)دلیلاں->os_varp;

    إذا (**varp != NUL && STRCMP(*varp, "camel") != 0)
	أرجع e_invalid_دلیل;

    أرجع عدم;
}

    صحيح
expand_سیٹ_spelloptions(optexpand_T *دلیلاں, صحيح *عدد_Matches, char_u ***matches)
{
    ساكن حرف *(p_spo_اقدار[]) = {"camel", عدم};
    أرجع expand_سیٹ_opt_string(
	    دلیلاں,
	    p_spo_اقدار,
	    ARRAY_لمائی(p_spo_اقدار) - 1,
	    عدد_Matches,
	    matches);
}

/*
 * The 'spellsuggest' option is changed.
 */
    حرف *
did_سیٹ_spellsuggest(optset_T *دلیلاں بن_ورتیا)
{
    إذا (spell_check_sps() != OK)
	أرجع e_invalid_دلیل;

    أرجع عدم;
}

    صحيح
expand_سیٹ_spellsuggest(optexpand_T *دلیلاں, صحيح *عدد_Matches, char_u ***matches)
{
    أرجع expand_سیٹ_opt_string(
	    دلیلاں,
	    p_sps_اقدار,
	    ARRAY_لمائی(p_sps_اقدار) - 1,
	    عدد_Matches,
	    matches);
}
#جے_انت

/*
 * The 'splitkeep' option is changed.
 */
    حرف *
did_سیٹ_splitkeep(optset_T *دلیلاں بن_ورتیا)
{
    win_T	*wp;
    tabpage_T	*tp;
    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	wp->w_prev_اچائی = wp->w_اچائی;
	wp->w_prev_winrow = wp->w_winrow;
    }
    أرجع did_سیٹ_opt_لڑیاں(p_spk, p_spk_اقدار, FALSE);
}

    صحيح
expand_سیٹ_splitkeep(optexpand_T *دلیلاں, صحيح *عدد_Matches, char_u ***matches)
{
    أرجع expand_سیٹ_opt_string(
	    دلیلاں,
	    p_spk_اقدار,
	    ARRAY_لمائی(p_spk_اقدار) - 1,
	    عدد_Matches,
	    matches);
}

#إذا defined(FEAT_STL_OPT)
/*
 * The 'statusline' option is changed.
 */
    حرف *
did_سیٹ_statusline(optset_T *دلیلاں)
{
    char_u  **varp = (char_u **)دلیلاں->os_varp;
    حرف    *ret = parse_statustabline_rulerformat(دلیلاں, FALSE);

    إذا (ret != عدم)
	أرجع ret;
    update_stl_rendered_اچائی(varp == &curwin->w_p_stl ? curwin : عدم);
    چوکھٹا_change_statusline_اچائی();

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
    ثابت char_u    *آخری_mh = عدم;
    ثابت char_u    *p;

    // Find the last "maxheight:" token.
    p = src;
    طالما (*p != NUL)
    {
	إذا (STRNCMP(p, "maxheight:", 10) == 0)
	    آخری_mh = p;
	طالما (*p != ',' && *p != NUL)
	    ++p;
	إذا (*p == ',')
	    ++p;
    }

    إذا (آخری_mh == عدم)
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
	    إذا (tok == آخری_mh)
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
did_سیٹ_statuslineopt(optset_T *دلیلاں)
{
    char_u	**varp = (char_u **)دلیلاں->os_varp;
    win_T       *wp = varp == &curwin->w_p_stlo ? curwin : عدم;

    إذا (statuslineopt_بدلیا(*varp, wp) == FAIL)
	أرجع e_invalid_دلیل;

    // Sync stl_rendered_height with the current statusline.
    update_stl_rendered_اچائی(wp);

    // Update the maxheight value to the actual value set.
    // Note: Must be changed if p_stlo_values are changed.
    إذا (*varp != خالی_تھاواں_option)
    {
	صحيح actual_stlh;

	إذا (wp != عدم)
	{
	    چوکھٹا_change_statusline_اچائی();
	    actual_stlh = MIN(wp->w_p_stlo_mh,
		    wp->w_اچائی + wp->w_حالت_اچائی - p_wmh);
	}
	وإلا
	    actual_stlh = چوکھٹا_change_statusline_اچائی();

	// Only re-serialize when the window actually has a status line shown.
	إذا (actual_stlh > 0)
	{
	    update_stlo_maxheight(varp, actual_stlh);
	    // Update the parsed maxheight member directly.
	    إذا (wp != عدم)
		wp->w_p_stlo_mh = actual_stlh;
	    وإلا
		سیٹ_stlo_mh(actual_stlh);
	}
    }
    وإلا
	چوکھٹا_change_statusline_اچائی();

    إذا ((دلیلاں->os_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	// :set clears the local values.
	clear_string_option(&curwin->w_p_stlo);
	curwin->w_p_stlo_fh = -1;
	curwin->w_p_stlo_mh = -1;
    }

    أرجع عدم;
}

    صحيح
expand_سیٹ_statuslineopt(optexpand_T *دلیلاں, صحيح *عدد_Matches, char_u ***matches)
{
    أرجع expand_سیٹ_opt_string(
	    دلیلاں,
	    p_stlo_اقدار,
	    ARRAY_لمائی(p_stlo_اقدار) - 1,
	    عدد_Matches,
	    matches);
}
#جے_انت

/*
 * The 'swapsync' option is changed.
 */
    حرف *
did_سیٹ_swapsync(optset_T *دلیلاں بن_ورتیا)
{
    أرجع did_سیٹ_opt_لڑیاں(p_sws, p_sws_اقدار, FALSE);
}

    صحيح
expand_سیٹ_swapsync(optexpand_T *دلیلاں, صحيح *عدد_Matches, char_u ***matches)
{
    أرجع expand_سیٹ_opt_string(
	    دلیلاں,
	    p_sws_اقدار,
	    ARRAY_لمائی(p_sws_اقدار) - 1,
	    عدد_Matches,
	    matches);
}

/*
 * The 'switchbuf' option is changed.
 */
    حرف *
did_سیٹ_switchbuf(optset_T *دلیلاں بن_ورتیا)
{
    أرجع did_سیٹ_opt_flags(p_swb, p_swb_اقدار, &swb_flags, TRUE);
}

    صحيح
expand_سیٹ_switchbuf(optexpand_T *دلیلاں, صحيح *عدد_Matches, char_u ***matches)
{
    أرجع expand_سیٹ_opt_string(
	    دلیلاں,
	    p_swb_اقدار,
	    ARRAY_لمائی(p_swb_اقدار) - 1,
	    عدد_Matches,
	    matches);
}

/*
 * The 'tabclose' option is changed.
 */
    حرف *
did_سیٹ_tabclose(optset_T *دلیلاں بن_ورتیا)
{
    أرجع did_سیٹ_opt_flags(p_tcl, p_tcl_اقدار, &tcl_flags, TRUE);
}

    صحيح
expand_سیٹ_tabclose(optexpand_T *دلیلاں, صحيح *عدد_Matches, char_u ***matches)
{
    أرجع expand_سیٹ_opt_string(
	    دلیلاں,
	    p_tcl_اقدار,
	    ARRAY_لمائی(p_tcl_اقدار) - 1,
	    عدد_Matches,
	    matches);
}

#إذا defined(FEAT_STL_OPT)
/*
 * The 'tabline' option is changed.
 */
    حرف *
did_سیٹ_tabline(optset_T *دلیلاں)
{
    أرجع parse_statustabline_rulerformat(دلیلاں, FALSE);
}
#جے_انت

/*
 * The 'tagcase' option is changed.
 */
    حرف *
did_سیٹ_tagcase(optset_T *دلیلاں)
{
    بنانشانی صحيح	*flags;
    char_u		*p;

    إذا (دلیلاں->os_flags & OPT_LOCAL)
    {
	p = curbuf->b_p_tc;
	flags = &curbuf->b_tc_flags;
    }
    وإلا
    {
	p = p_tc;
	flags = &tc_flags;
    }

    إذا ((دلیلاں->os_flags & OPT_LOCAL) && *p == NUL)
	// make the local value empty: use the global value
	*flags = 0;
    وإلا إذا (*p == NUL
	    || opt_لڑیاں_flags(p, p_tc_اقدار, flags, FALSE) != OK)
	أرجع e_invalid_دلیل;

    أرجع عدم;
}

    صحيح
expand_سیٹ_tagcase(optexpand_T *دلیلاں, صحيح *عدد_Matches, char_u ***matches)
{
    أرجع expand_سیٹ_opt_string(
	    دلیلاں,
	    p_tc_اقدار,
	    ARRAY_لمائی(p_tc_اقدار) - 1,
	    عدد_Matches,
	    matches);
}

/*
 * The 'term' option is changed.
 */
    حرف *
did_سیٹ_رکن(optset_T *دلیلاں بن_ورتیا)
{
    إذا (T_NAME[0] == NUL)
	أرجع e_cannot_سیٹ_رکن_منزل_خالی_تھاواں_string;
#جے_تعریف_شدہ FEAT_GUI
    إذا (gui.وچ_use)
	أرجع e_cannot_change_رکن_وچ_GUI;
    إذا (رکن_is_gui(T_NAME))
	أرجع e_use_gui_منزل_شروع_GUI;
#جے_انت
    إذا (سیٹ_termname(T_NAME) == FAIL)
	أرجع e_not_لبھیا_وچ_termcap;

    // Screen colors may have changed.
    redraw_later_clear();

    أرجع عدم;
}

/*
 * Some terminal option (t_xxx) is changed
 */
    حرف *
did_سیٹ_رکن_option(optset_T *دلیلاں)
{
    char_u	**varp = (char_u **)دلیلاں->os_varp;

    إذا (!بھریا_screen)
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
		T_CCO = خالی_تھاواں_option;
	    }
#إذا defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
	    إذا (is_رکن_win32())
	    {
		وٹاندرا_tcap();
		دلیلاں->os_did_swaptcap = TRUE;
	    }
#جے_انت
	    // We now have a different color setup, initialize it again.
	    init_highlight(TRUE, FALSE);
	}
    }
    ttest(FALSE);
    إذا (varp == &T_ME)
    {
	باہرلا_str(T_ME);
	redraw_later(UPD_CLEAR);
#إذا defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
	// Since t_me has been set, this probably means that the user
	// wants to use this as default colors.  Need to reset default
	// background/foreground colors.
# ifdef VIMDLL
	إذا (!gui.وچ_use && !gui.starting)
# endif
	    mch_سیٹ_عام_colors();
#جے_انت
    }
    إذا (varp == &T_BE && termcap_سرگرم)
    {
	MAY_WANT_منزل_LOG_THIS;

	إذا (*T_BE == NUL)
	    // When clearing t_BE we assume the user no longer wants
	    // bracketed paste, thus disable it by writing t_BD.
	    باہرلا_str(T_BD);
	وإلا
	    باہرلا_str(T_BE);
    }

    إذا (varp == &T_BSU || varp == &T_ESU)
	رکن_سیٹ_sync_output(رکن_SYNC_OUTPUT_OFF);

    أرجع عدم;
}


#جے_تعریف_شدہ UNIX
/*
 * The 'termresize' option is changed.
 */
    حرف *
did_سیٹ_termresize(optset_T *دلیلاں بن_ورتیا)
{
    // If empty or "inband", then attempt to enable in-band resize events.
    إذا (*p_trz == NUL || STRCMP(p_trz, "inband") == 0)
	رکن_سیٹ_win_resize(true);
    وإلا إذا (STRCMP(p_trz, "sigwinch") == 0)
	رکن_سیٹ_win_resize(false);
    وإلا
	أرجع e_invalid_دلیل;

    أرجع عدم;
}

    صحيح
expand_سیٹ_termresize(optexpand_T *دلیلاں, صحيح *عدد_Matches, char_u ***matches)
{
    أرجع expand_سیٹ_opt_string(
	    دلیلاں,
	    p_trz_اقدار,
	    ARRAY_لمائی(p_trz_اقدار) - 1,
	    عدد_Matches,
	    matches);
}
#جے_انت

#إذا defined(FEAT_TERMINAL)
/*
 * The 'termwinkey' option is changed.
 */
    حرف *
did_سیٹ_termwinkey(optset_T *دلیلاں)
{
    char_u	**varp = (char_u **)دلیلاں->os_varp;

    إذا ((*varp)[0] != NUL && string_منزل_کلید(*varp, TRUE) == 0)
	أرجع e_invalid_دلیل;

    أرجع عدم;
}

/*
 * The 'termwinsize' option is changed.
 */
    حرف *
did_سیٹ_termwinsize(optset_T *دلیلاں)
{
    char_u	**varp = (char_u **)دلیلاں->os_varp;
    char_u	*p;

    إذا ((*varp)[0] == NUL)
	أرجع عدم;

    p = skipdigits(*varp);
    إذا (p == *varp || (*p != 'x' && *p != '*') || *skipdigits(p + 1) != NUL)
	أرجع e_invalid_دلیل;

    أرجع عدم;
}

# إذا defined(MSWIN)
/*
 * The 'termwintype' option is changed.
 */
    حرف *
did_سیٹ_termwintype(optset_T *دلیلاں بن_ورتیا)
{
    أرجع did_سیٹ_opt_لڑیاں(p_twt, p_twt_اقدار, FALSE);
}

    صحيح
expand_سیٹ_termwintype(optexpand_T *دلیلاں, صحيح *عدد_Matches, char_u ***matches)
{
    أرجع expand_سیٹ_opt_string(
	    دلیلاں,
	    p_twt_اقدار,
	    ARRAY_لمائی(p_twt_اقدار) - 1,
	    عدد_Matches,
	    matches);
}
# endif
#جے_انت

/*
 * The 'titlestring' option is changed.
 */
    حرف *
did_سیٹ_titlestring(optset_T *دلیلاں)
{
    صحيح flagval = 0;

#جے_تعریف_شدہ FEAT_STL_OPT
    flagval = STL_وچ_TITLE;
#جے_انت
    أرجع parse_titleiconstring(دلیلاں, flagval);
}

#إذا defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
/*
 * The 'toolbar' option is changed.
 */
    حرف *
did_سیٹ_toolbar(optset_T *دلیلاں بن_ورتیا)
{
    إذا (opt_لڑیاں_flags(p_toolbar, p_toolbar_اقدار, &toolbar_flags,
								TRUE) != OK)
	أرجع e_invalid_دلیل;

    باہرلا_flush();
    gui_mch_show_toolbar((toolbar_flags &
		(TOOLBAR_متن | TOOLBAR_ICONS)) != 0);
    أرجع عدم;
}

    صحيح
expand_سیٹ_toolbar(optexpand_T *دلیلاں, صحيح *عدد_Matches, char_u ***matches)
{
    أرجع expand_سیٹ_opt_string(
	    دلیلاں,
	    p_toolbar_اقدار,
	    ARRAY_لمائی(p_toolbar_اقدار) - 1,
	    عدد_Matches,
	    matches);
}
#جے_انت

#إذا defined(FEAT_TOOLBAR) && defined(FEAT_GUI_GTK)
/*
 * The 'toolbariconsize' option is changed.  GTK+ 2 only.
 */
    حرف *
did_سیٹ_toolbariconsize(optset_T *دلیلاں بن_ورتیا)
{
    إذا (opt_لڑیاں_flags(p_tbis, p_tbis_اقدار, &tbis_flags, FALSE) != OK)
	أرجع e_invalid_دلیل;

    باہرلا_flush();
    gui_mch_show_toolbar((toolbar_flags &
		(TOOLBAR_متن | TOOLBAR_ICONS)) != 0);
    أرجع عدم;
}

    صحيح
expand_سیٹ_toolbariconsize(
    optexpand_T	*دلیلاں,
    صحيح		*عدد_Matches,
    char_u	***matches)
{
    أرجع expand_سیٹ_opt_string(
	    دلیلاں,
	    p_tbis_اقدار,
	    ARRAY_لمائی(p_tbis_اقدار) - 1,
	    عدد_Matches,
	    matches);
}
#جے_انت

#إذا defined(UNIX) || defined(VMS)
/*
 * The 'ttymouse' option is changed.
 */
    حرف *
did_سیٹ_ttymouse(optset_T *دلیلاں بن_ورتیا)
{
    حرف *errmsg = عدم;

    // Switch the mouse off before changing the escape sequences used for
    // that.
    mch_setmouse(FALSE);
    إذا (opt_لڑیاں_flags(p_ttym, p_ttym_اقدار, &ttym_flags, FALSE) != OK)
	errmsg = e_invalid_دلیل;
    وإلا
	check_mouse_termcode();
    إذا (termcap_سرگرم)
	setmouse();		// may switch it on again

    أرجع errmsg;
}

    صحيح
expand_سیٹ_ttymouse(optexpand_T *دلیلاں, صحيح *عدد_Matches, char_u ***matches)
{
    أرجع expand_سیٹ_opt_string(
	    دلیلاں,
	    p_ttym_اقدار,
	    ARRAY_لمائی(p_ttym_اقدار) - 1,
	    عدد_Matches,
	    matches);
}
#جے_انت

#إذا defined(FEAT_VARTABS)
/*
 * The 'varsofttabstop' option is changed.
 */
    حرف *
did_سیٹ_varsofttabstop(optset_T *دلیلاں)
{
    char_u	**varp = (char_u **)دلیلاں->os_varp;
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
	    أرجع e_invalid_دلیل;
	}

	صحيح *oldarray = curbuf->b_p_vsts_array;
	إذا (tabstop_سیٹ(*varp, &(curbuf->b_p_vsts_array)) == OK)
	{
	    إذا (oldarray)
		vim_free(oldarray);
	}
	وإلا
	    أرجع e_invalid_دلیل;
    }

    أرجع عدم;
}

/*
 * The 'vartabstop' option is changed.
 */
    حرف *
did_سیٹ_vartabstop(optset_T *دلیلاں)
{
    char_u	**varp = (char_u **)دلیلاں->os_varp;
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
	    أرجع e_invalid_دلیل;
	}

	صحيح *oldarray = curbuf->b_p_vts_array;

	إذا (tabstop_سیٹ(*varp, &(curbuf->b_p_vts_array)) == OK)
	{
	    vim_free(oldarray);
# ifdef FEAT_FOLDING
	    إذا (foldmethodIsIndent(curwin))
		foldUpdateAll(curwin);
# endif
	}
	وإلا
	    أرجع e_invalid_دلیل;
    }

    أرجع عدم;
}
#جے_انت

/*
 * The 'verbosefile' option is changed.
 */
    حرف *
did_سیٹ_verbosefile(optset_T *دلیلاں بن_ورتیا)
{
    verbose_stop();
    إذا (*p_vfile != NUL && verbose_open() == FAIL)
	أرجع e_invalid_دلیل;

    أرجع عدم;
}

#إذا defined(FEAT_SESSION)
/*
 * The 'viewoptions' option is changed.
 */
    حرف *
did_سیٹ_viewoptions(optset_T *دلیلاں بن_ورتیا)
{
    أرجع did_سیٹ_opt_flags(p_vop, p_ssop_اقدار, &vop_flags, TRUE);
}
#جے_انت

#إذا defined(FEAT_VIMINFO)
/*
 * The 'viminfo' option is changed.
 */
    حرف *
did_سیٹ_viminfo(optset_T *دلیلاں)
{
    char_u	*s;
    حرف	*errmsg = عدم;

    لكل (s = p_viminfo; *s;)
    {
	// Check it's a valid character
	إذا (vim_strchr((char_u *)"!\"%'/:<@cfhnrs", *s) == عدم)
	{
	    errmsg = illegal_char(دلیلاں->os_errbuf, دلیلاں->os_errbuflen, *s);
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
		إذا (دلیلاں->os_errbuf != عدم)
		{
		    vim_snprintf(دلیلاں->os_errbuf, دلیلاں->os_errbuflen,
			    _(e_missing_نمبر_after_angle_str_angle),
			    transchar_byte(*(s - 1)));
		    errmsg = دلیلاں->os_errbuf;
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
	    إذا (دلیلاں->os_errbuf != عدم)
		errmsg = e_missing_comma;
	    وإلا
		errmsg = "";
	    اكسر;
	}
    }
    إذا (*p_viminfo && errmsg == عدم && get_viminfo_parameter('\'') < 0)
	errmsg = e_must_specify_a_قدر;

    أرجع errmsg;
}
#جے_انت

/*
 * The 'virtualedit' option is changed.
 */
    حرف *
did_سیٹ_virtualedit(optset_T *دلیلاں)
{
    char_u		*ve = p_ve;
    بنانشانی صحيح	*flags = &ve_flags;

    إذا (دلیلاں->os_flags & OPT_LOCAL)
    {
	ve = curwin->w_p_ve;
	flags = &curwin->w_ve_flags;
    }

    إذا ((دلیلاں->os_flags & OPT_LOCAL) && *ve == NUL)
	// make the local value empty: use the global value
	*flags = 0;
    وإلا
    {
	إذا (opt_لڑیاں_flags(ve, p_ve_اقدار, flags, TRUE) != OK)
	    أرجع e_invalid_دلیل;
	وإلا إذا (STRCMP(ve, دلیلاں->os_oldval.string) != 0)
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
expand_سیٹ_virtualedit(optexpand_T *دلیلاں, صحيح *عدد_Matches, char_u ***matches)
{
    أرجع expand_سیٹ_opt_string(
	    دلیلاں,
	    p_ve_اقدار,
	    ARRAY_لمائی(p_ve_اقدار) - 1,
	    عدد_Matches,
	    matches);
}

/*
 * The 'whichwrap' option is changed.
 */
    حرف *
did_سیٹ_whichwrap(optset_T *دلیلاں)
{
    char_u	**varp = (char_u **)دلیلاں->os_varp;

    // Add ',' to the list flags because 'whichwrap' is a flag
    // list that is comma-separated.
    أرجع did_سیٹ_option_listflag(*varp, (char_u *)(WW_ALL ","),
		    دلیلاں->os_errbuf, دلیلاں->os_errbuflen);
}

    صحيح
expand_سیٹ_whichwrap(optexpand_T *دلیلاں, صحيح *عدد_Matches, char_u ***matches)
{
    أرجع expand_سیٹ_opt_listflag(دلیلاں, (char_u*)WW_ALL, عدد_Matches, matches);
}

/*
 * The 'wildmode' option is changed.
 */
    حرف *
did_سیٹ_wildmode(optset_T *دلیلاں بن_ورتیا)
{
    إذا (check_opt_wim() == FAIL)
	أرجع e_invalid_دلیل;
    أرجع عدم;
}

    صحيح
expand_سیٹ_wildmode(optexpand_T *دلیلاں, صحيح *عدد_Matches, char_u ***matches)
{
    أرجع expand_سیٹ_opt_string(
	    دلیلاں,
	    p_wim_اقدار,
	    ARRAY_لمائی(p_wim_اقدار) - 1,
	    عدد_Matches,
	    matches);
}

/*
 * The 'wildoptions' option is changed.
 */
    حرف *
did_سیٹ_wildoptions(optset_T *دلیلاں بن_ورتیا)
{
    أرجع did_سیٹ_opt_لڑیاں(p_wop, p_wop_اقدار, TRUE);
}

    صحيح
expand_سیٹ_wildoptions(optexpand_T *دلیلاں, صحيح *عدد_Matches, char_u ***matches)
{
    أرجع expand_سیٹ_opt_string(
	    دلیلاں,
	    p_wop_اقدار,
	    ARRAY_لمائی(p_wop_اقدار) - 1,
	    عدد_Matches,
	    matches);
}

#إذا defined(FEAT_WAK)
/*
 * The 'winaltkeys' option is changed.
 */
    حرف *
did_سیٹ_winaltkeys(optset_T *دلیلاں بن_ورتیا)
{
    حرف *errmsg = عدم;

    إذا (*p_wak == NUL || check_opt_لڑیاں(p_wak, p_wak_اقدار, FALSE) != OK)
	errmsg = e_invalid_دلیل;
# ifdef FEAT_MENU
#  إذا defined(FEAT_GUI_MOTIF)
    وإلا إذا (gui.وچ_use)
	gui_motif_سیٹ_mnemonics(p_wak[0] == 'y' || p_wak[0] == 'm');
#  elif defined(FEAT_GUI_GTK)
    وإلا إذا (gui.وچ_use)
	gui_gtk_سیٹ_mnemonics(p_wak[0] == 'y' || p_wak[0] == 'm');
#  endif
# endif
    أرجع errmsg;
}

    صحيح
expand_سیٹ_winaltkeys(optexpand_T *دلیلاں, صحيح *عدد_Matches, char_u ***matches)
{
    أرجع expand_سیٹ_opt_string(
	    دلیلاں,
	    p_wak_اقدار,
	    ARRAY_لمائی(p_wak_اقدار) - 1,
	    عدد_Matches,
	    matches);
}
#جے_انت

/*
 * The 'wincolor' option is changed.
 */
    حرف *
did_سیٹ_wincolor(optset_T *دلیلاں بن_ورتیا)
{
    update_wincolor(curwin, دلیلاں->os_newval.string);
    أرجع عدم;
}

/*
 * The 'winhighlight' option is changed
 */
    حرف *
did_سیٹ_winhighlight(optset_T *دلیلاں)
{
    أرجع update_winhighlight(curwin, دلیلاں->os_newval.string);
}

/*
 * Expand 'winhighlight' option.
 */
    صحيح
expand_سیٹ_winhighlight(optexpand_T *دلیلاں, صحيح *عدد_Matches, char_u ***matches)
{
    expand_T	    *xp = دلیلاں->oe_xp;

    إذا ((xp->xp_نمونہ > دلیلاں->oe_سیٹ_arg && *(xp->xp_نمونہ-1) == ':')
	    || xp->xp_نمونہ == دلیلاں->oe_سیٹ_arg || *(xp->xp_نمونہ-1) == ',')
    {
	// After a ':' or after a ',', or at the start, so expand highlight
	// group name.

	// If starts with !, then expand 'highlight' occasions.
	إذا (*xp->xp_نمونہ == '!')
	    أرجع expand_hl_occasions(دلیلاں, عدد_Matches, matches, '!');
	وإلا
	    أرجع expand_سیٹ_opt_generic(
		    دلیلاں,
		    get_highlight_name,
		    عدد_Matches,
		    matches);
    }

    VIM_CLEAR(*matches);
    أرجع FAIL;
}

    صحيح
expand_سیٹ_wincolor(optexpand_T *دلیلاں, صحيح *عدد_Matches, char_u ***matches)
{
    أرجع expand_سیٹ_opt_generic(
	    دلیلاں,
	    get_highlight_name,
	    عدد_Matches,
	    matches);
}

#جے_تعریف_شدہ FEAT_SYN_HL
/*
 * When the 'syntax' option is set, load the syntax of that name.
 */
    ساكن فراغ
do_syntax_autocmd(صحيح قدر_بدلیا)
{
    ساكن صحيح syn_recursive = 0;

    ++syn_recursive;
    // Only pass TRUE for "force" when the value changed or not used
    // recursively, to avoid endless recurrence.
    apply_autocmds(EVENT_SYNTAX, curbuf->b_p_syn, curbuf->b_fname,
	    قدر_بدلیا || syn_recursive == 1, curbuf);
    curbuf->b_flags |= BF_SYN_سیٹ;
    --syn_recursive;
}
#جے_انت

/*
 * When the 'filetype' option is set, trigger the FileType autocommand.
 */
    ساكن فراغ
do_filetype_autocmd(char_u **varp, صحيح opt_flags, صحيح قدر_بدلیا)
{
    // Skip this when called from a modeline and the filetype was already set
    // to this value.
    إذا ((opt_flags & OPT_MODELINE) && !قدر_بدلیا)
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
	    قدر_بدلیا || ft_recursive == 1, curbuf);
    --ft_recursive;
    // Just in case the old "curbuf" is now invalid.
    إذا (varp != &(curbuf->b_p_ft))
	varp = عدم;

    secure = secure_save;
}

#جے_تعریف_شدہ FEAT_SPELL
/*
 * When the 'spelllang' option is set, source the spell/LANG.vim file in
 * 'runtimepath'.
 */
    ساكن فراغ
do_spelllang_سروت(فراغ)
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
	سروت_runtime(fname, DIP_ALL);
    }
}
#جے_انت

/*
 * Handle string options that need some action to perform when changed.
 * The new value must be allocated.
 * Returns NULL for success, or an untranslated error message for an error.
 */
    حرف *
did_سیٹ_string_option(
    صحيح		opt_idx,		// index in options[] table
    char_u	**varp,			// pointer to the option variable
    char_u	*oldval,		// previous value of the option
    char_u	*قدر,			// new value of the option
    حرف	*errbuf,		// buffer for errors, or NULL
    size_t	errbuflen,		// length of error buffer
    صحيح		opt_flags,		// OPT_LOCAL and/or OPT_GLOBAL
    سیٹ_op_T    op,			// OP_ADDING/OP_PREPENDING/OP_REMOVING
    صحيح		*قدر_checked)		// value was checked to be safe, no
					// need to set P_INSECURE
{
    حرف	*errmsg = عدم;
    long_u	free_oldval = (get_option_flags(opt_idx) & P_ALLOCED);
    opt_did_سیٹ_cb_T did_سیٹ_cb = get_option_did_سیٹ_cb(opt_idx);
    optset_T	دلیلاں;

    // 'ttytype' is an alias for 'term'.  Both 'term' and 'ttytype' point to
    // T_NAME.  If 'term' or 'ttytype' is modified, then use the index for the
    // 'term' option.  Only set the P_ALLOCED flag on 'term'.
    إذا (varp == &T_NAME)
    {
	opt_idx = findoption((char_u *)"term");
	إذا (opt_idx >= 0)
	{
	    free_oldval = (get_option_flags(opt_idx) & P_ALLOCED);
	    did_سیٹ_cb = get_option_did_سیٹ_cb(opt_idx);
	}
    }

    CLEAR_میدان(دلیلاں);

    // Disallow changing some options from secure mode
    إذا ((secure
#جے_تعریف_شدہ HAVE_SANDBOX
		|| sandbox != 0
#جے_انت
		) && (get_option_flags(opt_idx) & P_SECURE))
	errmsg = e_not_allowed_ایتھے;
    // Check for a "normal" directory or file name in some options.
    وإلا إذا (check_illegal_رستہ_ناں(opt_idx, varp))
	errmsg = e_invalid_دلیل;
    وإلا إذا (did_سیٹ_cb != عدم)
    {
	دلیلاں.os_varp = (char_u *)varp;
	دلیلاں.os_idx = opt_idx;
	دلیلاں.os_flags = opt_flags;
	دلیلاں.os_op = op;
	دلیلاں.os_oldval.string = oldval;
	دلیلاں.os_newval.string = قدر;
	دلیلاں.os_errbuf = errbuf;
	دلیلاں.os_errbuflen = errbuflen;
	// Invoke the option specific callback function to validate and apply
	// the new option value.
	errmsg = did_سیٹ_cb(&دلیلاں);

	// The 'keymap', 'filetype' and 'syntax' option callback functions
	// may change the os_value_checked field.
	*قدر_checked = دلیلاں.os_قدر_checked;
    }

    // If an error is detected, restore the previous value.
    إذا (errmsg != عدم)
    {
	free_string_option(*varp);
	*varp = oldval;
	// When resetting some values, need to act on it.
	إذا (دلیلاں.os_restore_chartab)
	    (فراغ)init_chartab();
	إذا (varp == &p_hl)
	    (فراغ)highlight_بدلیا();
    }
    وإلا
    {
#جے_تعریف_شدہ FEAT_EVAL
	// Remember where the option was set.
	سیٹ_option_sctx_idx(opt_idx, opt_flags, current_sctx);
#جے_انت
	// Free string options that are in allocated memory.
	// Use "free_oldval", because recursiveness may change the flags under
	// our fingers (esp. init_highlight()).
	إذا (free_oldval)
	    free_string_option(oldval);
	سیٹ_option_flag(opt_idx, P_ALLOCED);

	إذا ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0
		&& is_global_local_option(opt_idx))
	{
	    // global option with local value set to use global value; free
	    // the local value and make it empty
	    char_u *p = get_option_varp_scope(opt_idx, OPT_LOCAL);
	    free_string_option(*(char_u **)p);
	    *(char_u **)p = خالی_تھاواں_option;
	}

	// May set global value for local option.
	وإلا إذا (!(opt_flags & OPT_LOCAL) && opt_flags != OPT_GLOBAL)
	    سیٹ_string_option_global(opt_idx, varp);

	// Trigger the autocommand only after setting the flags.
#جے_تعریف_شدہ FEAT_SYN_HL
	إذا (varp == &(curbuf->b_p_syn))
	    do_syntax_autocmd(دلیلاں.os_قدر_بدلیا);
#جے_انت
	وإلا إذا (varp == &(curbuf->b_p_ft))
	    do_filetype_autocmd(varp, opt_flags, دلیلاں.os_قدر_بدلیا);
#جے_تعریف_شدہ FEAT_SPELL
	إذا (varp == &(curwin->w_s->b_p_spl))
	    do_spelllang_سروت();
#جے_انت
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
	update_package_paths_وچ_lua();
#جے_انت
    }

#إذا defined(FEAT_LINEBREAK)
    // Changing Formatlistpattern when briopt includes the list setting:
    // redraw
    إذا ((varp == &p_flp || varp == &(curbuf->b_p_flp))
	    && curwin->w_briopt_list)
	redraw_all_later(UPD_NOT_VALID);
#جے_انت

    إذا (curwin->w_curswant != MAXCOL
		   && (get_option_flags(opt_idx) & (P_CURSWANT | P_RALL)) != 0
				&& (get_option_flags(opt_idx) & P_HLONLY) == 0)
	curwin->w_سیٹ_curswant = true;

    إذا ((opt_flags & OPT_NO_REDRAW) == 0)
    {
#جے_تعریف_شدہ FEAT_GUI
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
	إذا (!redraw_gui_only || gui.وچ_use)
#جے_انت
	    check_redraw(get_option_flags(opt_idx));
    }

#إذا defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
    إذا (دلیلاں.os_did_swaptcap)
    {
	سیٹ_termname((char_u *)"win32");
	init_highlight(TRUE, FALSE);
    }
#جے_انت

    أرجع errmsg;
}

/*
 * Check an option that can be a range of string values.
 *
 * Return OK for correct value, FAIL otherwise.
 * Empty is always OK.
 */
    ساكن صحيح
check_opt_لڑیاں(
    char_u	*val,
    حرف	**اقدار,
    صحيح		list)	    // when TRUE: accept a list of values
{
    أرجع opt_لڑیاں_flags(val, اقدار, عدم, list);
}

/*
 * Handle an option that can be a range of string values.
 * Set a flag in "*flagp" for each string present.
 *
 * Return OK for correct value, FAIL otherwise.
 * Empty is always OK.
 */
    ساكن صحيح
opt_لڑیاں_flags(
    char_u	*val,		// new value
    حرف	**اقدار,	// array of valid string values
    بنانشانی	*flagp,
    صحيح		list)		// when TRUE: accept a list of values
{
    صحيح		i;
    صحيح		len;
    بنانشانی	new_flags = 0;

    طالما (*val)
    {
	لكل (i = 0; ; ++i)
	{
	    إذا (اقدار[i] == عدم)	// val not found in values[]
		أرجع FAIL;

	    len = (صحيح)STRLEN(اقدار[i]);
	    إذا (STRNCMP(اقدار[i], val, len) == 0
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
check_ff_قدر(char_u *p)
{
    أرجع check_opt_لڑیاں(p, p_ff_اقدار, FALSE);
}

/*
 * Save the actual shortmess Flags and clear them temporarily to avoid that
 * file messages overwrites any output from the following commands.
 *
 * Caller must make sure to first call save_clear_shm_value() and then
 * restore_shm_value() exactly the same number of times.
 */
    فراغ
save_clear_shm_قدر(فراغ)
{
    إذا (STRLEN(p_shm) >= SHM_LEN)
    {
	iemsg(e_اندرونی_error_shortmess_too_long);
	أرجع;
    }

    إذا (++سیٹ_shm_recursive == 1)
    {
	STRCPY(shm_buf, p_shm);
	سیٹ_option_قدر_give_err((char_u *)"shm", 0L, (char_u *)"", 0);
    }
}

/*
 * Restore the shortmess Flags set from the save_clear_shm_value() function.
 */
    فراغ
restore_shm_قدر(فراغ)
{
    إذا (--سیٹ_shm_recursive == 0)
    {
	سیٹ_option_قدر_give_err((char_u *)"shm", 0L, shm_buf, 0);
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

    buf.لمائی = (size_t)copy_option_part(&q, buf.string, MAXPATHL, ",");

    p = vim_getenv((char_u *)"MYVIMDIR", &dofree);

    إذا (p == عدم || STRCMP(p, buf.string) != 0)
    {
	إذا (*buf.string != NUL && !after_pathsep(buf.string, buf.string + buf.لمائی))
	    STRCPY(buf.string + buf.لمائی, PATHSEPSTR);
#جے_تعریف_شدہ MSWIN
	// normalize path separators
	لكل (q = buf.string; *q != NUL; q++)
	    إذا (*q == '/')
		*q = '\\';
#جے_انت
	vim_setenv((char_u *)"MYVIMDIR", buf.string);
    }
    إذا (dofree)
	vim_free(p);
    vim_free(buf.string);
}
