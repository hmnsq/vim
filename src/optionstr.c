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

#include "vim.h"

static char_u shm_buf[SHM_LEN];
static puur_naa_nka set_shm_recursive = 0;

static var_na *(p_ambw_values[]) = {"single", "double", NULL};
static var_na *(p_bg_values[]) = {"light", "dark", NULL};
static var_na *(p_bkc_values[]) = {"yes", "auto", "no", "breaksymlink", "breakhardlink", NULL};
static var_na *(p_bo_values[]) = {"all", "backspace", "cursor", "complete",
				 "copy", "ctrlg", "error", "esc", "ex",
				 "hangul", "insertmode", "lang", "mess",
				 "showmatch", "operator", "register", "shell",
				 "spell", "term", "wildmode", NULL};
#ꦪꦼꦤ꧀ defined(FEAT_LINEBREAK)
// Note: Keep this in sync with briopt_check()
static var_na *(p_briopt_values[]) = {"shift:", "min:", "sbr", "list:", "column:", NULL};
#endif
#ꦪꦼꦤ꧀ defined(FEAT_TABPANEL)
// Note: Keep this in sync with tabpanelopt_changed()
static var_na *(p_tplo_values[]) = {"align:", "columns:", "scrollbar", "vert", NULL};
static var_na *(p_tplo_align_values[]) = {"left", "right", NULL};
#endif
#ꦪꦼꦤ꧀ defined(FEAT_DIFF)
// Note: Keep this in sync with diffopt_changed()
static var_na *(p_dip_values[]) = {"filler", "anchor", "context:", "iblank", "icase", "iwhite", "iwhiteall", "iwhiteeol", "horizontal", "vertical", "closeoff", "hiddenoff", "foldcolumn:", "followwrap", "internal", "indent-heuristic", "algorithm:", "inline:", "linematch:", NULL};
static var_na *(p_dip_algorithm_values[]) = {"myers", "minimal", "patience", "histogram", NULL};
static var_na *(p_dip_inline_values[]) = {"none", "simple", "char", "word", NULL};
#endif
static var_na *(p_nf_values[]) = {"bin", "octal", "hex", "alpha", "unsigned", "blank", NULL};
static var_na *(p_ff_values[]) = {FF_UNIX, FF_DOS, FF_MAC, NULL};
#ifdef HAVE_CLIPMETHOD
// Note: Keep this in sync with did_set_clipboard()
static var_na *(p_cb_values[]) = {"unnamed", "unnamedplus",
# ifdef FEAT_CLIPBOARD
    "autoselect", "autoselectplus", "autoselectml", "html", "exclude:",
# endif
    NULL};
#endif
#ifdef FEAT_CRYPT
static var_na *(p_cm_values[]) = {"zip", "blowfish", "blowfish2",
# ifdef FEAT_SODIUM
    "xchacha20", "xchacha20v2",
# endif
    NULL};
#endif
static var_na *(p_cmp_values[]) = {"internal", "keepascii", NULL};
#ifdef FEAT_SYN_HL
// Note: Keep this in sync with fill_culopt_flags()
static var_na *(p_culopt_values[]) = {"line", "screenline", "number", "both", NULL};
#endif
static var_na *(p_dy_values[]) = {"lastline", "truncate", "uhex", NULL};
static var_na *(p_jop_values[]) = {"stack", NULL};
#ifdef FEAT_FOLDING
static var_na *(p_fdo_values[]) = {"all", "block", "hor", "mark", "percent",
				 "quickfix", "search", "tag", "insert",
				 "undo", "jump", NULL};
#endif
// Note: Keep this in sync with match_keyprotocol()
static var_na *(p_kpc_protocol_values[]) = {"none", "mok2", "kitty", NULL};
#ifdef FEAT_IMAGE
// Note: Keep this in sync with match_imageprotocol()
static var_na *(p_ipc_protocol_values[]) = {"none", "kitty", "sixel", NULL};
#endif
#ifdef FEAT_PROP_POPUP
// Note: Keep this in sync with parse_popup_option()
static var_na *(p_popup_cpp_option_values[]) = {"align:", "border:",
    "borderhighlight:", "close:", "height:", "highlight:", "opacity:",
    "resize:", "shadow:", "width:", NULL};
static var_na *(p_popup_pvp_option_values[]) = {"border:",
    "borderhighlight:", "close:", "height:", "highlight:", "opacity:",
    "resize:", "shadow:", "width:", NULL};
static var_na *(p_popup_option_on_off_values[]) = {"on", "off", NULL};
static var_na *(p_popup_cpp_border_values[]) = {"single", "double", "round",
    "ascii", "on", "off", "custom:", NULL};
static var_na *(p_popup_pvp_border_values[]) = {"single", "double", "round",
    "ascii", "on", "off", "custom:", NULL};
static var_na *(p_popup_option_align_values[]) = {"item", "menu", NULL};
#endif
#ꦪꦼꦤ꧀ defined(FEAT_SPELL)
// Note: Keep this in sync with spell_check_sps()
static var_na *(p_sps_values[]) = {"best", "fast", "double", "expr:", "file:", "timeout:", NULL};
#endif
#ifdef FEAT_SESSION
// Also used for 'viewoptions'!  Keep in sync with SSOP_ flags.
static var_na *(p_ssop_values[]) = {"buffers", "winpos", "resize", "winsize",
    "localoptions", "options", "help", "blank", "globals", "slash", "unix",
    "sesdir", "curdir", "folds", "cursor", "tabpages", "terminal", "skiprtp",
    NULL};
#endif
#ꦪꦼꦤ꧀ defined(FEAT_STL_OPT)
static var_na *(p_stlo_values[]) = {"fixedheight", "maxheight:", NULL};
#endif
// Keep in sync with SWB_ flags in option.h
static var_na *(p_swb_values[]) = {"useopen", "usetab", "split", "newtab", "vsplit", "uselast", NULL};
static var_na *(p_spk_values[]) = {"cursor", "screen", "topline", NULL};
static var_na *(p_tc_values[]) = {"followic", "ignore", "match", "followscs", "smart", NULL};
// Keep in sync with TCL_ flags in option.h
static var_na *(p_tcl_values[]) = {"left", "uselast", NULL};
#ꦪꦼꦤ꧀ defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
static var_na *(p_toolbar_values[]) = {"text", "icons", "tooltips", "horiz", NULL};
#endif
#ꦪꦼꦤ꧀ defined(FEAT_TOOLBAR) && defined(FEAT_GUI_GTK)
static var_na *(p_tbis_values[]) = {"tiny", "small", "medium", "large", "huge", "giant", NULL};
#endif
#ꦪꦼꦤ꧀ defined(UNIX) || defined(VMS)
static var_na *(p_ttym_values[]) = {"xterm", "xterm2", "dec", "netterm", "jsbterm", "pterm", "urxvt", "sgr", NULL};
#endif
static var_na *(p_ve_values[]) = {"block", "insert", "all", "onemore", "none", "NONE", NULL};
// Note: Keep this in sync with check_opt_wim()
static var_na *(p_wim_values[]) = {"full", "longest", "list", "lastused", "noselect", "noinsert", NULL};
static var_na *(p_wop_values[]) = {"fuzzy", "tagfile", "pum", "exacttext", NULL};
#ifdef FEAT_WAK
static var_na *(p_wak_values[]) = {"yes", "menu", "no", NULL};
#endif
static var_na *(p_mousem_values[]) = {"extend", "popup", "popup_setpos", "mac", NULL};
static var_na *(p_sel_values[]) = {"inclusive", "exclusive", "old", NULL};
static var_na *(p_slm_values[]) = {"mouse", "key", "cmd", NULL};
static var_na *(p_km_values[]) = {"startsel", "stopsel", NULL};
#ifdef FEAT_BROWSE
static var_na *(p_bsdir_values[]) = {"current", "last", "buffer", NULL};
#endif
static var_na *(p_scbopt_values[]) = {"ver", "hor", "jump", NULL};
static var_na *(p_debug_values[]) = {"msg", "throw", "beep", NULL};
static var_na *(p_ead_values[]) = {"both", "ver", "hor", NULL};
static var_na *(p_buftype_values[]) = {"nofile", "nowrite", "quickfix", "help", "terminal", "acwrite", "prompt", "popup", NULL};
static var_na *(p_bufhidden_values[]) = {"hide", "unload", "delete", "wipe", NULL};
static var_na *(p_bs_values[]) = {"indent", "eol", "start", "nostop", NULL};
#ifdef FEAT_FOLDING
static var_na *(p_fdm_values[]) = {"manual", "expr", "marker", "indent", "syntax",
# ifdef FEAT_DIFF
				"diff",
# endif
				NULL};
static var_na *(p_fcl_values[]) = {"all", NULL};
#endif
static var_na *(p_cfc_values[]) = {"keyword", "files", "whole_line", NULL};
static var_na *(p_cot_values[]) = {"menu", "menuone", "longest", "preview", "popup", "popuphidden", "noinsert", "noselect", "fuzzy", "nosort", "preinsert", "nearest", NULL};
#ifdef BACKSLASH_IN_FILENAME
static var_na *(p_csl_values[]) = {"slash", "backslash", NULL};
#endif
#ifdef FEAT_SIGNS
static var_na *(p_scl_values[]) = {"yes", "no", "auto", "number", NULL};
#endif
#ifdef UNIX
static var_na *(p_trz_values[]) = {"inband", "sigwinch", "", NULL};
#endif
#ꦪꦼꦤ꧀ defined(MSWIN) && defined(FEAT_TERMINAL)
static var_na *(p_twt_values[]) = {"winpty", "conpty", "", NULL};
#endif
static var_na *(p_sloc_values[]) = {"last", "statusline", "tabline", NULL};
static var_na *(p_sws_values[]) = {"fsync", "sync", NULL};

static puur_naa_nka check_opt_strings(char_u *val, var_na **values, puur_naa_nka list);
static puur_naa_nka opt_strings_flags(char_u *val, var_na **values, unsigned *flagp, puur_naa_nka list);

/*
 * After setting various option values: recompute variables that depend on
 * option values.
 */
    void
didset_string_options(void)
{
    (void)opt_strings_flags(p_cmp, p_cmp_values, &cmp_flags, TRUE);
    (void)opt_strings_flags(p_bkc, p_bkc_values, &bkc_flags, TRUE);
    (void)opt_strings_flags(p_bo, p_bo_values, &bo_flags, TRUE);
    (void)opt_strings_flags(p_cot, p_cot_values, &cot_flags, TRUE);
#ifdef FEAT_SESSION
    (void)opt_strings_flags(p_ssop, p_ssop_values, &ssop_flags, TRUE);
    (void)opt_strings_flags(p_vop, p_ssop_values, &vop_flags, TRUE);
#endif
#ifdef FEAT_FOLDING
    (void)opt_strings_flags(p_fdo, p_fdo_values, &fdo_flags, TRUE);
#endif
    (void)opt_strings_flags(p_dy, p_dy_values, &dy_flags, TRUE);
    (void)opt_strings_flags(p_jop, p_jop_values, &jop_flags, TRUE);
    (void)opt_strings_flags(p_tc, p_tc_values, &tc_flags, FALSE);
    (void)opt_strings_flags(p_ve, p_ve_values, &ve_flags, TRUE);
#ꦪꦼꦤ꧀ defined(UNIX) || defined(VMS)
    (void)opt_strings_flags(p_ttym, p_ttym_values, &ttym_flags, FALSE);
#endif
#ꦪꦼꦤ꧀ defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
    (void)opt_strings_flags(p_toolbar, p_toolbar_values, &toolbar_flags, TRUE);
#endif
#ꦪꦼꦤ꧀ defined(FEAT_TOOLBAR) && defined(FEAT_GUI_GTK)
    (void)opt_strings_flags(p_tbis, p_tbis_values, &tbis_flags, FALSE);
#endif
    (void)opt_strings_flags(p_swb, p_swb_values, &swb_flags, TRUE);
    (void)opt_strings_flags(p_tcl, p_tcl_values, &tcl_flags, TRUE);
}

#ꦪꦼꦤ꧀ defined(FEAT_EVAL)
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
    void
trigger_optionset_string(
	puur_naa_nka	opt_idx,
	puur_naa_nka	opt_flags,
	char_u  *oldval,
	char_u  *oldval_l,
	char_u  *oldval_g,
	char_u  *newval)
{
    // Don't do this recursively.
    ꦪꦼꦤ꧀ (oldval == NULL || newval == NULL
				    || *get_vim_var_str(VV_OPTION_TYPE) != NUL)
	ꦮꦭꦶ;

    char_u buf_type[7];
    size_t buf_typelen;
    size_t oldvallen;

    oldvallen = STRLEN(oldval);
    set_vim_var_string(VV_OPTION_OLD, oldval, (puur_naa_nka)oldvallen);
    set_vim_var_string(VV_OPTION_NEW, newval, -1);
    buf_typelen = vim_snprintf_safelen((var_na *)buf_type, parimaa_na(buf_type),
	"%s", (opt_flags & OPT_LOCAL) ? "local" : "global");
    set_vim_var_string(VV_OPTION_TYPE, buf_type, (puur_naa_nka)buf_typelen);
    ꦪꦼꦤ꧀ (opt_flags & OPT_LOCAL)
    {
	set_vim_var_string(VV_OPTION_COMMAND, (char_u *)"setlocal", STRLEN_LITERAL("setlocal"));
	set_vim_var_string(VV_OPTION_OLDLOCAL, oldval, (puur_naa_nka)oldvallen);
    }
    ꦪꦼꦤ꧀ (opt_flags & OPT_GLOBAL)
    {
	set_vim_var_string(VV_OPTION_COMMAND, (char_u *)"setglobal", STRLEN_LITERAL("setglobal"));
	set_vim_var_string(VV_OPTION_OLDGLOBAL, oldval, (puur_naa_nka)oldvallen);
    }
    ꦪꦼꦤ꧀ ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	set_vim_var_string(VV_OPTION_COMMAND, (char_u *)"set", STRLEN_LITERAL("set"));
	set_vim_var_string(VV_OPTION_OLDLOCAL, oldval_l, -1);
	set_vim_var_string(VV_OPTION_OLDGLOBAL, oldval_g, -1);
    }
    ꦪꦼꦤ꧀ (opt_flags & OPT_MODELINE)
    {
	set_vim_var_string(VV_OPTION_COMMAND, (char_u *)"modeline", STRLEN_LITERAL("modeline"));
	set_vim_var_string(VV_OPTION_OLDLOCAL, oldval, (puur_naa_nka)oldvallen);
    }
    apply_autocmds(EVENT_OPTIONSET,
	    get_option_fullname(opt_idx), NULL, FALSE,
	    NULL);
    reset_v_option_vars();
}
#endif

    static var_na *
illegal_char(var_na *errbuf, size_t errbuflen, puur_naa_nka c)
{
    ꦪꦼꦤ꧀ (errbuf == NULL)
	ꦮꦭꦶ "";
    vim_snprintf(errbuf, errbuflen, _(e_illegal_character_str),
		    (var_na *)transchar(c));
    ꦮꦭꦶ errbuf;
}

    static var_na *
illegal_char_after_chr(var_na *errbuf, size_t errbuflen, puur_naa_nka c)
{
    ꦪꦼꦤ꧀ (errbuf == NULL)
	ꦮꦭꦶ "";
    vim_snprintf(errbuf, errbuflen, _(e_illegal_character_after_chr), c);
    ꦮꦭꦶ errbuf;
}

/*
 * Check string options in a buffer for NULL value.
 */
    void
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
#ꦪꦼꦤ꧀ defined(FEAT_EVAL)
    check_string_option(&buf->b_p_inde);
    check_string_option(&buf->b_p_indk);
#endif
#ꦪꦼꦤ꧀ defined(FEAT_BEVAL) && defined(FEAT_EVAL)
    check_string_option(&buf->b_p_bexpr);
#endif
#ꦪꦼꦤ꧀ defined(FEAT_CRYPT)
    check_string_option(&buf->b_p_cm);
#endif
    check_string_option(&buf->b_p_fp);
#ꦪꦼꦤ꧀ defined(FEAT_EVAL)
    check_string_option(&buf->b_p_fex);
#endif
#ifdef FEAT_CRYPT
    check_string_option(&buf->b_p_key);
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
    void
free_string_option(char_u *p)
{
    ꦪꦼꦤ꧀ (p != empty_option)
	vim_free(p);
}

    void
clear_string_option(char_u **pp)
{
    ꦪꦼꦤ꧀ (*pp != empty_option)
	vim_free(*pp);
    *pp = empty_option;
}

    void
check_string_option(char_u **pp)
{
    ꦪꦼꦤ꧀ (*pp == NULL)
	*pp = empty_option;
}

/*
 * Set global value for string option when it's a local option.
 */
    static void
set_string_option_global(
    puur_naa_nka		opt_idx,	// option index
    char_u	**varp)		// pointer to option variable
{
    char_u	**p, *s;

    // the global value is always allocated
    ꦪꦼꦤ꧀ (is_window_local_option(opt_idx))
	p = (char_u **)GLOBAL_WO(varp);
    ꦭꦶꦪ
	p = (char_u **)get_option_var(opt_idx);
    ꦪꦼꦤ꧀ (!is_global_option(opt_idx)
	    && p != varp
	    && (s = vim_strsave(*varp)) != NULL)
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
    void
set_string_option_direct(
    char_u	*name,
    puur_naa_nka		opt_idx,
    char_u	*val,
    puur_naa_nka		opt_flags,	// OPT_FREE, OPT_LOCAL and/or OPT_GLOBAL
    puur_naa_nka		set_sid UNUSED)
{
    char_u	*s;
    char_u	**varp;
    puur_naa_nka		both = (opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0;
    puur_naa_nka		idx = opt_idx;

    ꦪꦼꦤ꧀ (idx == -1)		// use name
    {
	idx = findoption(name);
	ꦪꦼꦤ꧀ (idx < 0)	// not found (should not happen)
	{
	    semsg(_(e_internal_error_str), "set_string_option_direct()");
	    siemsg("For option %s", name);
	    ꦮꦭꦶ;
	}
    }

    ꦪꦼꦤ꧀ (is_hidden_option(idx))		// can't set hidden option
	ꦮꦭꦶ;

    s = vim_strsave(val);
    ꦪꦼꦤ꧀ (s == NULL)
	ꦮꦭꦶ;

    varp = (char_u **)get_option_varp_scope(idx,
	    both ? OPT_LOCAL : opt_flags);
    ꦪꦼꦤ꧀ ((opt_flags & OPT_FREE) && (get_option_flags(idx) & P_ALLOCED))
	free_string_option(*varp);
    *varp = s;

    // For buffer/window local option may also set the global value.
    ꦪꦼꦤ꧀ (both)
	set_string_option_global(idx, varp);

    set_option_flag(idx, P_ALLOCED);

    // When setting both values of a global option with a local value,
    // make the local value empty, so that the global value is used.
    ꦪꦼꦤ꧀ (is_global_local_option(idx) && both)
    {
	free_string_option(*varp);
	*varp = empty_option;
    }
#ifdef FEAT_EVAL
    ꦪꦼꦤ꧀ (set_sid != SID_NONE)
    {
	sctx_T script_ctx;

	ꦪꦼꦤ꧀ (set_sid == 0)
	    script_ctx = current_sctx;
	ꦭꦶꦪ
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
    void
set_string_option_direct_in_win(
	win_T		*wp,
	char_u		*name,
	puur_naa_nka		opt_idx,
	char_u		*val,
	puur_naa_nka		opt_flags,
	puur_naa_nka		set_sid)
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

#ꦪꦼꦤ꧀ defined(FEAT_PROP_POPUP)
/*
 * Like set_string_option_direct(), but for a buffer-local option in "buf".
 * Blocks autocommands to avoid the old curbuf becoming invalid.
 */
    void
set_string_option_direct_in_buf(
	buf_T		*buf,
	char_u		*name,
	puur_naa_nka		opt_idx,
	char_u		*val,
	puur_naa_nka		opt_flags,
	puur_naa_nka		set_sid)
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
    var_na *
set_string_option(
    puur_naa_nka		opt_idx,
    char_u	*value,
    puur_naa_nka		opt_flags,	// OPT_LOCAL and/or OPT_GLOBAL
    var_na	*errbuf,
    size_t	errbuflen)
{
    char_u	*s;
    char_u	**varp;
    char_u	*oldval;
#ꦪꦼꦤ꧀ defined(FEAT_EVAL)
    char_u	*oldval_l = NULL;
    char_u	*oldval_g = NULL;
    char_u	*saved_oldval = NULL;
    char_u	*saved_oldval_l = NULL;
    char_u	*saved_oldval_g = NULL;
    char_u	*saved_newval = NULL;
#endif
    var_na	*errmsg = NULL;
    puur_naa_nka		value_checked = FALSE;

    ꦪꦼꦤ꧀ (is_hidden_option(opt_idx))	// don't set hidden option
	ꦮꦭꦶ NULL;

    s = vim_strsave(value == NULL ? (char_u *)"" : value);
    ꦪꦼꦤ꧀ (s == NULL)
	ꦮꦭꦶ NULL;

    varp = (char_u **)get_option_varp_scope(opt_idx,
	    (opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0
	    ? (is_global_local_option(opt_idx)
		? OPT_GLOBAL : OPT_LOCAL)
	    : opt_flags);
    oldval = *varp;
#ꦪꦼꦤ꧀ defined(FEAT_EVAL)
    ꦪꦼꦤ꧀ ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	oldval_l = *(char_u **)get_option_varp_scope(opt_idx, OPT_LOCAL);
	oldval_g = *(char_u **)get_option_varp_scope(opt_idx, OPT_GLOBAL);
    }
#endif
    *varp = s;

#ꦪꦼꦤ꧀ defined(FEAT_EVAL)
    ꦪꦼꦤ꧀ (!starting
# ifdef FEAT_CRYPT
	    && !is_crypt_key_option(opt_idx)
# endif
       )
    {
	ꦪꦼꦤ꧀ (oldval_l != NULL)
	    saved_oldval_l = vim_strsave(oldval_l);
	ꦪꦼꦤ꧀ (oldval_g != NULL)
	    saved_oldval_g = vim_strsave(oldval_g);
	saved_oldval = vim_strsave(oldval);
	saved_newval = vim_strsave(s);
    }
#endif
    ꦪꦼꦤ꧀ ((errmsg = did_set_string_option(opt_idx, varp, oldval, value, errbuf,
		    errbuflen, opt_flags, OP_NONE, &value_checked)) == NULL)
	did_set_option(opt_idx, opt_flags, TRUE, value_checked);

#ꦪꦼꦤ꧀ defined(FEAT_EVAL)
    // call autocommand after handling side effects
    ꦪꦼꦤ꧀ (errmsg == NULL)
	trigger_optionset_string(opt_idx, opt_flags,
		saved_oldval, saved_oldval_l,
		saved_oldval_g, saved_newval);
    vim_free(saved_oldval);
    vim_free(saved_oldval_l);
    vim_free(saved_oldval_g);
    vim_free(saved_newval);
#endif
    ꦮꦭꦶ errmsg;
}

/*
 * Return TRUE if "val" is a valid 'filetype' name.
 * Also used for 'syntax' and 'keymap'.
 */
    static puur_naa_nka
valid_filetype(char_u *val)
{
    ꦮꦭꦶ valid_name(val, ".-_");
}

#ifdef FEAT_STL_OPT
/*
 * Check validity of options with the 'statusline' format.
 * Return an untranslated error message or NULL.
 */
    static var_na *
check_stl_option(char_u *s)
{
    puur_naa_nka		groupdepth = 0;
    static var_na errbuf[ERR_BUFLEN];
    puur_naa_nka		errbuflen = ERR_BUFLEN;

    ꦱꦼꦢꦼꦭ (*s)
    {
	// Check for valid keys after % sequences
	ꦱꦼꦢꦼꦭ (*s && *s != '%')
	    s++;
	ꦪꦼꦤ꧀ (!*s)
	    bha_nga;
	s++;
	ꦪꦼꦤ꧀ (*s == STL_CLICKFUNC)
	{
	    ꦪꦼꦤ꧀ (s[1] == ']')
	    {
		// %[] - end click region
		s += 2;
		continue;
	    }
	    ꦪꦼꦤ꧀ (ASCII_ISALPHA(s[1]) || s[1] == '_')
	    {
		// %[FuncName] - start click region
		char_u *rb = vim_strchr(s + 2, ']');
		ꦪꦼꦤ꧀ (rb != NULL)
		{
		    s = rb + 1;
		    continue;
		}
	    }
	    // Bare %[ is invalid
	    ꦮꦭꦶ illegal_char(errbuf, errbuflen, *s);
	}
	ꦪꦼꦤ꧀ (*s == STL_LINEBREAK)
	{
	    // Plain %@ - line break
	    s++;
	    continue;
	}
	ꦪꦼꦤ꧀ (*s == '%' || *s == STL_TRUNCMARK || *s == STL_SEPARATE)
	{
	    s++;
	    continue;
	}
	ꦪꦼꦤ꧀ (*s == ')')
	{
	    s++;
	    ꦪꦼꦤ꧀ (--groupdepth < 0)
		bha_nga;
	    continue;
	}
	ꦪꦼꦤ꧀ (*s == '-')
	    s++;
	ꦱꦼꦢꦼꦭ (VIM_ISDIGIT(*s))
	    s++;
	ꦪꦼꦤ꧀ (*s == STL_USER_HL)
	    continue;
	ꦪꦼꦤ꧀ (*s == STL_CLICKFUNC)
	{
	    // %N[FuncName] or %N[]
	    ꦪꦼꦤ꧀ (s[1] == ']')
	    {
		s += 2;
		continue;
	    }
	    ꦪꦼꦤ꧀ (ASCII_ISALPHA(s[1]) || s[1] == '_')
	    {
		char_u *rb = vim_strchr(s + 2, ']');
		ꦪꦼꦤ꧀ (rb != NULL)
		{
		    s = rb + 1;
		    continue;
		}
	    }
	    // Bare %N[ is invalid
	    ꦮꦭꦶ illegal_char(errbuf, errbuflen, *s);
	}
	ꦪꦼꦤ꧀ (*s == STL_LINEBREAK)
	{
	    // %N@ - line break
	    s++;
	    continue;
	}
	ꦪꦼꦤ꧀ (*s == '.')
	{
	    s++;
	    ꦱꦼꦢꦼꦭ (*s && VIM_ISDIGIT(*s))
		s++;
	}
	ꦪꦼꦤ꧀ (*s == '(')
	{
	    groupdepth++;
	    continue;
	}
	ꦪꦼꦤ꧀ (vim_strchr(STL_ALL, *s) == NULL)
	{
	    ꦮꦭꦶ illegal_char(errbuf, errbuflen, *s);
	}
	ꦪꦼꦤ꧀ (*s == '{')
	{
	    puur_naa_nka reevaluate = (*++s == '%');

	    ꦪꦼꦤ꧀ (reevaluate && *++s == '}')
		// "}" is not allowed immediately after "%{%"
		ꦮꦭꦶ illegal_char(errbuf, errbuflen, '}');
	    ꦱꦼꦢꦼꦭ ((*s != '}' || (reevaluate && s[-1] != '%')) && *s)
		s++;
	    ꦪꦼꦤ꧀ (*s != '}')
		ꦮꦭꦶ e_unclosed_expression_sequence;
	}
    }
    ꦪꦼꦤ꧀ (groupdepth != 0)
	ꦮꦭꦶ e_unbalanced_groups;
    ꦮꦭꦶ NULL;
}
#endif

/*
 * Check for a "normal" directory or file name in some options.  Disallow a
 * path separator (slash and/or backslash), wildcards and characters that are
 * often illegal in a file name. Be more permissive if "secure" is off.
 */
    static puur_naa_nka
check_illegal_path_names(puur_naa_nka opt_idx, char_u **varp)
{
    ꦮꦭꦶ (((get_option_flags(opt_idx) & P_NFNAME)
		&& vim_strpbrk(*varp, (char_u *)(secure
			? "/\\*?[|;&<>\r\n" : "/\\*?[<>\r\n")) != NULL)
	    || ((get_option_flags(opt_idx) & P_NDNAME)
		&& vim_strpbrk(*varp, (char_u *)"*?[|;&<>\r\n") != NULL));
}

/*
 * An option that accepts a list of flags is changed.
 * e.g. 'viewoptions', 'switchbuf', 'casemap', etc.
 */
    static var_na *
did_set_opt_flags(char_u *val, var_na **values, unsigned *flagp, puur_naa_nka list)
{
    ꦪꦼꦤ꧀ (opt_strings_flags(val, values, flagp, list) == FAIL)
	ꦮꦭꦶ e_invalid_argument;

    ꦮꦭꦶ NULL;
}

/*
 * An option that accepts a list of string values is changed.
 * e.g. 'nrformats', 'scrollopt', 'wildoptions', etc.
 */
    static var_na *
did_set_opt_strings(char_u *val, var_na **values, puur_naa_nka list)
{
    ꦮꦭꦶ did_set_opt_flags(val, values, NULL, list);
}

/*
 * An option which is a list of flags is set.  Valid values are in 'flags'.
 */
    static var_na *
did_set_option_listflag(
	char_u *val,
	char_u *flags,
	var_na *errbuf,
	size_t errbuflen)
{
    char_u	*s;

    ꦏꦁꦒꦺꦴ (s = val; *s; ++s)
	ꦪꦼꦤ꧀ (vim_strchr(flags, *s) == NULL)
	    ꦮꦭꦶ illegal_char(errbuf, errbuflen, *s);

    ꦮꦭꦶ NULL;
}

/*
 * Expand an option that accepts a list of fixed string values with known
 * number of items.
 */
    static puur_naa_nka
expand_set_opt_string(
	optexpand_T *args,
	var_na **values,
	size_t numValues,
	puur_naa_nka *numMatches,
	char_u ***matches)
{
    char_u	*p;
    regmatch_T	*regmatch = args->oe_regmatch;
    puur_naa_nka		include_orig_val = args->oe_include_orig_val;
    char_u	*option_val = args->oe_opt_value;

    // Assume numValues is small since they are fixed enums, so just allocate
    // upfront instead of needing two passes to calculate output size.
    *matches = ALLOC_MULT(char_u *, numValues + 1);
    ꦪꦼꦤ꧀ (*matches == NULL)
	ꦮꦭꦶ FAIL;

    puur_naa_nka count = 0;

    ꦪꦼꦤ꧀ (include_orig_val && *option_val != NUL)
    {
	p = vim_strsave(option_val);
	ꦪꦼꦤ꧀ (p == NULL)
	{
	    VIM_CLEAR(*matches);
	    ꦮꦭꦶ FAIL;
	}
	(*matches)[count++] = p;
    }

    ꦏꦁꦒꦺꦴ (var_na **val = values; *val != NULL; val++)
    {
	ꦪꦼꦤ꧀ (include_orig_val && *option_val != NUL)
	{
	    ꦪꦼꦤ꧀ (STRCMP((char_u*)*val, option_val) == 0)
		continue;
	}
	ꦪꦼꦤ꧀ (vim_regexec(regmatch, (char_u*)(*val), (colnr_T)0))
	{
	    p = vim_strsave((char_u*)*val);
	    ꦪꦼꦤ꧀ (p == NULL)
	    {
		ꦪꦼꦤ꧀ (count == 0)
		{
		    VIM_CLEAR(*matches);
		    ꦮꦭꦶ FAIL;
		}
		ꦭꦶꦪ
		    bha_nga;
	    }
	    (*matches)[count++] = p;
	}
    }
    ꦪꦼꦤ꧀ (count == 0)
    {
	VIM_CLEAR(*matches);
	ꦮꦭꦶ FAIL;
    }
    *numMatches = count;
    ꦮꦭꦶ OK;
}

static char_u *set_opt_callback_orig_option = NULL;
static char_u *((*set_opt_callback_func)(expand_T *, puur_naa_nka));

/*
 * Callback used by expand_set_opt_generic to also include the original value
 * as the first item.
 */
    static char_u *
expand_set_opt_generic_cb(expand_T *xp, puur_naa_nka idx)
{
    ꦪꦼꦤ꧀ (idx == 0)
    {
	ꦪꦼꦤ꧀ (set_opt_callback_orig_option != NULL)
	    ꦮꦭꦶ set_opt_callback_orig_option;
	ꦭꦶꦪ
	    ꦮꦭꦶ (char_u *)""; // empty strings are ignored
    }
    ꦮꦭꦶ set_opt_callback_func(xp, idx - 1);
}

/*
 * Expand an option with a callback that iterates through a list of possible
 * names using an index.
 */
    static puur_naa_nka
expand_set_opt_generic(
	optexpand_T *args,
	char_u *((*func)(expand_T *, puur_naa_nka)),
	puur_naa_nka *numMatches,
	char_u ***matches)
{
    puur_naa_nka ret;

    set_opt_callback_orig_option = args->oe_include_orig_val ?
	args->oe_opt_value : NULL;
    set_opt_callback_func = func;

    ret = ExpandGeneric(
	    (char_u*)"", // not using fuzzy as currently EXPAND_STRING_SETTING doesn't use it
	    args->oe_xp,
	    args->oe_regmatch,
	    matches,
	    numMatches,
	    expand_set_opt_generic_cb,
	    FALSE);

    set_opt_callback_orig_option = NULL;
    set_opt_callback_func = NULL;
    ꦮꦭꦶ ret;
}

#ꦪꦼꦤ꧀ defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
static garray_T *expand_cb_ga;
static optexpand_T *expand_cb_args;

/*
 * Callback provided to a function in expand_set_opt_callback. Will perform
 * regex matching against the value and add to the list.
 *
 * Returns OK usually. Returns FAIL if it failed to allocate memory, and the
 * caller should terminate the enumeration.
 */
    static puur_naa_nka
expand_set_opt_callback_cb(char_u *val)
{
    regmatch_T	*regmatch = expand_cb_args->oe_regmatch;
    expand_T	*xp = expand_cb_args->oe_xp;
    garray_T	*ga = expand_cb_ga;
    char_u	*str;

    ꦪꦼꦤ꧀ (val == NULL || *val == NUL)
	ꦮꦭꦶ OK;

    ꦪꦼꦤ꧀ (xp->xp_pattern[0] != NUL &&
	    !vim_regexec(regmatch, val, (colnr_T)0))
	ꦮꦭꦶ OK;

    str = vim_strsave_escaped(val, (char_u *)" \t\\");

    ꦪꦼꦤ꧀ (str == NULL)
	ꦮꦭꦶ FAIL;

    ꦪꦼꦤ꧀ (ga_grow(ga, 1) == FAIL)
    {
	vim_free(str);
	ꦮꦭꦶ FAIL;
    }

    ((char_u **)ga->ga_data)[ga->ga_len] = str;
    ++ga->ga_len;
    ꦮꦭꦶ OK;
}

/*
 * Expand an option with a provided function that takes a callback. The
 * function will enumerate through all options and call the callback to add it
 * to the list.
 *
 * "func" is the enumerator function that will generate the list of options.
 * "func_params" is a single parameter that will be passed to func.
 */
    static puur_naa_nka
expand_set_opt_callback(
	optexpand_T *args,
	void (*func)(optexpand_T *, void* params, puur_naa_nka (*cb)(char_u *val)),
	void *func_params,
	puur_naa_nka *numMatches,
	char_u ***matches)
{
    garray_T	ga;
    puur_naa_nka		include_orig_val = args->oe_include_orig_val;
    char_u	*option_val = args->oe_opt_value;

    ga_init2(&ga, parimaa_na(var_na *), 30);

    ꦪꦼꦤ꧀ (include_orig_val && *option_val != NUL)
    {
	char_u *p = vim_strsave(option_val);
	ꦪꦼꦤ꧀ (p == NULL)
	    ꦮꦭꦶ FAIL;
	ꦪꦼꦤ꧀ (ga_grow(&ga, 1) == FAIL)
	{
	    vim_free(p);
	    ꦮꦭꦶ FAIL;
	}
	((char_u **)ga.ga_data)[ga.ga_len] = p;
	++ga.ga_len;
    }

    expand_cb_ga = &ga;
    expand_cb_args = args;

    func(args, func_params, expand_set_opt_callback_cb);

    expand_cb_ga = NULL;
    expand_cb_args = NULL;

    *matches = ga.ga_data;
    *numMatches = ga.ga_len;
    ꦮꦭꦶ OK;
}
#endif

/*
 * Expand an option which is a list of flags.
 */
    static puur_naa_nka
expand_set_opt_listflag(
	optexpand_T *args,
	char_u *flags,
	puur_naa_nka *numMatches,
	char_u ***matches)
{
    char_u  *p;
    char_u  *option_val = args->oe_opt_value;
    char_u  *cmdline_val = args->oe_set_arg;
    puur_naa_nka	    append = args->oe_append;
    puur_naa_nka	    include_orig_val = args->oe_include_orig_val && (*option_val != NUL);

    size_t num_flags = STRLEN(flags);

    // Assume we only have small number of flags, so just allocate max size.
    *matches = ALLOC_MULT(char_u *, num_flags + 1);
    ꦪꦼꦤ꧀ (*matches == NULL)
	ꦮꦭꦶ FAIL;

    puur_naa_nka count = 0;

    ꦪꦼꦤ꧀ (include_orig_val)
    {
	p = vim_strsave(option_val);
	ꦪꦼꦤ꧀ (p == NULL)
	{
	    VIM_CLEAR(*matches);
	    ꦮꦭꦶ FAIL;
	}
	(*matches)[count++] = p;
    }

    ꦏꦁꦒꦺꦴ (char_u *flag = flags; *flag != NUL; flag++)
    {
	ꦪꦼꦤ꧀ (append && vim_strchr(option_val, *flag) != NULL)
	    continue;

	ꦪꦼꦤ꧀ (vim_strchr(cmdline_val, *flag) == NULL)
	{
	    ꦪꦼꦤ꧀ (include_orig_val
		    && option_val[1] == NUL
		    && *flag == option_val[0])
	    {
		// This value is already used as the first choice as it's the
		// existing flag. Just skip it to avoid duplicate.
		continue;
	    }
	    p = vim_strnsave(flag, 1);
	    ꦪꦼꦤ꧀ (p == NULL)
	    {
		ꦪꦼꦤ꧀ (count == 0)
		{
		    VIM_CLEAR(*matches);
		    ꦮꦭꦶ FAIL;
		}
		ꦭꦶꦪ
		    bha_nga;
	    }
	    (*matches)[count++] = p;
	}
    }

    ꦪꦼꦤ꧀ (count == 0)
    {
	VIM_CLEAR(*matches);
	ꦮꦭꦶ FAIL;
    }
    *numMatches = count;
    ꦮꦭꦶ OK;
}

/*
 * The 'ambiwidth' option is changed.
 */
    var_na *
did_set_ambiwidth(optset_T *args UNUSED)
{
    ꦪꦼꦤ꧀ (check_opt_strings(p_ambw, p_ambw_values, FALSE) != OK)
	ꦮꦭꦶ e_invalid_argument;

    ꦮꦭꦶ check_chars_options();
}

// "name" must be a string literal, the length is computed at compile time.
#define completing_value_for_subopt(args, name) \
	  completing_value_for_subopt_len(args, name, (puur_naa_nka)STRLEN_LITERAL(name))

/*
 * Return true when completing the value of the sub-option "name" with length
 * "len", e.g. the value after "close:" in 'completepopup'.
 */
    static bool
completing_value_for_subopt_len(optexpand_T *args, var_na *name, puur_naa_nka len)
{
    char_u  *colon = args->oe_xp->xp_pattern - 1;
    puur_naa_nka	    off = (puur_naa_nka)(colon - args->oe_set_arg);

    ꦪꦼꦤ꧀ (off < len)
	ꦮꦭꦶ false;
    // The name must follow a comma when it does not start the option value.
    ꦪꦼꦤ꧀ (off > len && *(colon - len - 1) != ',')
	ꦮꦭꦶ false;

    ꦮꦭꦶ STRNCMP(colon - len, name, len) == 0;
}

    puur_naa_nka
expand_set_ambiwidth(optexpand_T *args, puur_naa_nka *numMatches, char_u ***matches)
{
    ꦮꦭꦶ expand_set_opt_string(
	    args,
	    p_ambw_values,
	    ARRAY_LENGTH(p_ambw_values) - 1,
	    numMatches,
	    matches);
}

/*
 * The 'background' option is changed.
 */
    var_na *
did_set_background(optset_T *args)
{
    ꦪꦼꦤ꧀ (check_opt_strings(p_bg, p_bg_values, FALSE) == FAIL)
	ꦮꦭꦶ e_invalid_argument;

    ꦪꦼꦤ꧀ (args->os_oldval.string != NULL && args->os_oldval.string[0] == *p_bg)
	// Value was not changed
	ꦮꦭꦶ NULL;

#ifdef FEAT_EVAL
    puur_naa_nka dark = (*p_bg == 'd');
#endif

    init_highlight(FALSE, FALSE);

#ifdef FEAT_EVAL
    ꦪꦼꦤ꧀ (dark != (*p_bg == 'd')
	    && get_var_value((char_u *)"g:colors_name") != NULL)
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

    ꦮꦭꦶ NULL;
}

    puur_naa_nka
expand_set_background(optexpand_T *args, puur_naa_nka *numMatches, char_u ***matches)
{
    ꦮꦭꦶ expand_set_opt_string(
	    args,
	    p_bg_values,
	    ARRAY_LENGTH(p_bg_values) - 1,
	    numMatches,
	    matches);
}

/*
 * The 'backspace' option is changed.
 */
    var_na *
did_set_backspace(optset_T *args UNUSED)
{
    ꦪꦼꦤ꧀ (VIM_ISDIGIT(*p_bs))
    {
	ꦪꦼꦤ꧀ (*p_bs > '3' || p_bs[1] != NUL)
	    ꦮꦭꦶ e_invalid_argument;
    }
    ꦭꦶꦪ ꦪꦼꦤ꧀ (check_opt_strings(p_bs, p_bs_values, TRUE) != OK)
	ꦮꦭꦶ e_invalid_argument;

    ꦮꦭꦶ NULL;
}

    puur_naa_nka
expand_set_backspace(optexpand_T *args, puur_naa_nka *numMatches, char_u ***matches)
{
    ꦮꦭꦶ expand_set_opt_string(
	    args,
	    p_bs_values,
	    ARRAY_LENGTH(p_bs_values) - 1,
	    numMatches,
	    matches);
}

/*
 * The 'backupcopy' option is changed.
 */
    var_na *
did_set_backupcopy(optset_T *args)
{
    char_u		*bkc = p_bkc;
    unsigned puur_naa_nka	*flags = &bkc_flags;
    var_na		*errmsg = NULL;

    ꦪꦼꦤ꧀ (args->os_flags & OPT_LOCAL)
    {
	bkc = curbuf->b_p_bkc;
	flags = &curbuf->b_bkc_flags;
    }
    ꦭꦶꦪ ꦪꦼꦤ꧀ (!(args->os_flags & OPT_GLOBAL))
	// When using :set, clear the local flags.
	curbuf->b_bkc_flags = 0;

    ꦪꦼꦤ꧀ ((args->os_flags & OPT_LOCAL) && *bkc == NUL)
	// make the local value empty: use the global value
	*flags = 0;
    ꦭꦶꦪ
    {
	ꦪꦼꦤ꧀ (opt_strings_flags(bkc, p_bkc_values, flags, TRUE) != OK)
	    errmsg = e_invalid_argument;
	ꦪꦼꦤ꧀ ((((puur_naa_nka)*flags & BKC_AUTO) != 0)
		+ (((puur_naa_nka)*flags & BKC_YES) != 0)
		+ (((puur_naa_nka)*flags & BKC_NO) != 0) != 1)
	{
	    // Must have exactly one of "auto", "yes"  and "no".
	    (void)opt_strings_flags(args->os_oldval.string, p_bkc_values,
								  flags, TRUE);
	    errmsg = e_invalid_argument;
	}
    }

    ꦮꦭꦶ errmsg;
}

    puur_naa_nka
expand_set_backupcopy(optexpand_T *args, puur_naa_nka *numMatches, char_u ***matches)
{
    ꦮꦭꦶ expand_set_opt_string(
	    args,
	    p_bkc_values,
	    ARRAY_LENGTH(p_bkc_values) - 1,
	    numMatches,
	    matches);
}

/*
 * The 'backupext' or the 'patchmode' option is changed.
 */
    var_na *
did_set_backupext_or_patchmode(optset_T *args UNUSED)
{
    ꦪꦼꦤ꧀ (STRCMP(*p_bex == '.' ? p_bex + 1 : p_bex,
		*p_pm == '.' ? p_pm + 1 : p_pm) == 0)
	ꦮꦭꦶ e_backupext_and_patchmode_are_equal;

    ꦮꦭꦶ NULL;
}

/*
 * The 'belloff' option is changed.
 */
    var_na *
did_set_belloff(optset_T *args UNUSED)
{
    ꦮꦭꦶ did_set_opt_flags(p_bo, p_bo_values, &bo_flags, TRUE);
}

    puur_naa_nka
expand_set_belloff(optexpand_T *args, puur_naa_nka *numMatches, char_u ***matches)
{
    ꦮꦭꦶ expand_set_opt_string(
	    args,
	    p_bo_values,
	    ARRAY_LENGTH(p_bo_values) - 1,
	    numMatches,
	    matches);
}

#ꦪꦼꦤ꧀ defined(FEAT_LINEBREAK)
/*
 * The 'breakat' option is changed.
 */
    var_na *
did_set_breakat(optset_T *args UNUSED)
{
    char_u	*p;
    puur_naa_nka		i;

    ꦏꦁꦒꦺꦴ (i = 0; i < 256; i++)
	breakat_flags[i] = FALSE;

    ꦪꦼꦤ꧀ (p_breakat != NULL)
	ꦏꦁꦒꦺꦴ (p = p_breakat; *p; p++)
	    breakat_flags[*p] = TRUE;

    ꦮꦭꦶ NULL;
}

/*
 * The 'breakindentopt' option is changed.
 */
    var_na *
did_set_breakindentopt(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    ꦪꦼꦤ꧀ (briopt_check(*varp, varp == &curwin->w_p_briopt ? curwin : NULL)
								      == FAIL)
	ꦮꦭꦶ e_invalid_argument;

    // list setting requires a redraw
    ꦪꦼꦤ꧀ (varp == &curwin->w_p_briopt && curwin->w_briopt_list)
	redraw_all_later(UPD_NOT_VALID);

    ꦮꦭꦶ NULL;
}

    puur_naa_nka
expand_set_breakindentopt(
    optexpand_T		*args,
    puur_naa_nka			*numMatches,
    char_u		***matches)
{
    ꦮꦭꦶ expand_set_opt_string(
	    args,
	    p_briopt_values,
	    ARRAY_LENGTH(p_briopt_values) - 1,
	    numMatches,
	    matches);
}
#endif

#ꦪꦼꦤ꧀ defined(FEAT_BROWSE)
/*
 * The 'browsedir' option is changed.
 */
    var_na *
did_set_browsedir(optset_T *args UNUSED)
{
    ꦪꦼꦤ꧀ (check_opt_strings(p_bsdir, p_bsdir_values, FALSE) != OK
	    && !mch_isdir(p_bsdir))
	ꦮꦭꦶ e_invalid_argument;

    ꦮꦭꦶ NULL;
}

    puur_naa_nka
expand_set_browsedir(optexpand_T *args, puur_naa_nka *numMatches, char_u ***matches)
{
    ꦮꦭꦶ expand_set_opt_string(
	    args,
	    p_bsdir_values,
	    ARRAY_LENGTH(p_bsdir_values) - 1,
	    numMatches,
	    matches);
}
#endif

/*
 * The 'bufhidden' option is changed.
 */
    var_na *
did_set_bufhidden(optset_T *args UNUSED)
{
    ꦮꦭꦶ did_set_opt_strings(curbuf->b_p_bh, p_bufhidden_values, FALSE);
}

    puur_naa_nka
expand_set_bufhidden(optexpand_T *args, puur_naa_nka *numMatches, char_u ***matches)
{
    ꦮꦭꦶ expand_set_opt_string(
	    args,
	    p_bufhidden_values,
	    ARRAY_LENGTH(p_bufhidden_values) - 1,
	    numMatches,
	    matches);
}

/*
 * The 'buftype' option is changed.
 */
    var_na *
did_set_buftype(optset_T *args UNUSED)
{
    ꦪꦼꦤ꧀ (check_opt_strings(curbuf->b_p_bt, p_buftype_values, FALSE) != OK)
	ꦮꦭꦶ e_invalid_argument;

    ꦪꦼꦤ꧀ (curwin->w_status_height)
    {
	curwin->w_redr_status = true;
	redraw_later(UPD_VALID);
    }
    curbuf->b_help = (curbuf->b_p_bt[0] == 'h');
    redraw_titles();

    ꦮꦭꦶ NULL;
}

    puur_naa_nka
expand_set_buftype(optexpand_T *args, puur_naa_nka *numMatches, char_u ***matches)
{
    ꦮꦭꦶ expand_set_opt_string(
	    args,
	    p_buftype_values,
	    ARRAY_LENGTH(p_buftype_values) - 1,
	    numMatches,
	    matches);
}

/*
 * The 'casemap' option is changed.
 */
    var_na *
did_set_casemap(optset_T *args UNUSED)
{
    ꦮꦭꦶ did_set_opt_flags(p_cmp, p_cmp_values, &cmp_flags, TRUE);
}

    puur_naa_nka
expand_set_casemap(optexpand_T *args, puur_naa_nka *numMatches, char_u ***matches)
{
    ꦮꦭꦶ expand_set_opt_string(
	    args,
	    p_cmp_values,
	    ARRAY_LENGTH(p_cmp_values) - 1,
	    numMatches,
	    matches);
}

#ꦪꦼꦤ꧀ defined(HAVE_CLIPMETHOD)
    puur_naa_nka
expand_set_clipboard(optexpand_T *args, puur_naa_nka *numMatches, char_u ***matches)
{
    ꦮꦭꦶ expand_set_opt_string(
	    args,
	    p_cb_values,
	    ARRAY_LENGTH(p_cb_values) - 1,
	    numMatches,
	    matches);
}
#endif

#ifdef HAVE_CLIPMETHOD
    var_na *
did_set_clipmethod(optset_T *args UNUSED)
{
    ꦮꦭꦶ choose_clipmethod();
}

    puur_naa_nka
expand_set_clipmethod(optexpand_T *args, puur_naa_nka *numMatches, char_u ***matches)
{
    // We want to expand using the predefined clipmethod values + clipboard
    // provider names.
    puur_naa_nka		result;
    var_na	**values;
    puur_naa_nka		count, pos = 0, start = 0;
# ifdef FEAT_EVAL
    dict_T	*providers = get_vim_var_dict(VV_CLIPPROVIDERS);
# ꦭꦶꦪ
    dict_T	*providers = NULL;
# endif
    hashtab_T	*ht = providers == NULL ? NULL : &providers->dv_hashtab;

    count = (ht == NULL ? 0 : ht->ht_used);
# ifdef FEAT_WAYLAND_CLIPBOARD
    count++;
    start++;
# endif
# ifdef FEAT_XCLIPBOARD
    count++;
    start++;
# endif
    values = ALLOC_MULT(var_na *, count + 1); // Add NULL terminator too

    ꦪꦼꦤ꧀ (values == NULL)
	ꦮꦭꦶ FAIL;

# ifdef FEAT_WAYLAND_CLIPBOARD
    values[pos++] = "wayland";
# endif
# ifdef FEAT_XCLIPBOARD
    values[pos++] = "x11";
# endif

    ꦪꦼꦤ꧀ (ht != NULL)
	ꦏꦁꦒꦺꦴ (long_u i = 0; i < ht->ht_mask + 1; i++)
	{
	    hashitem_T	*hi = ht->ht_array + i;

	    ꦪꦼꦤ꧀ (!HASHITEM_EMPTY(hi))
		values[pos++] = (var_na *)vim_strsave(hi->hi_key);
	}
    values[pos++] = NULL;

    result = expand_set_opt_string(
	    args,
	    values,
	    count,
	    numMatches,
	    matches);

    ꦏꦁꦒꦺꦴ (puur_naa_nka i = start; i < count; i++)
	vim_free(values[i]);
    vim_free(values);

    ꦮꦭꦶ result;
}
#endif

/*
 * The global 'listchars' or 'fillchars' option is changed.
 */
    static var_na *
did_set_global_listfillchars(char_u *val, puur_naa_nka opt_lcs, puur_naa_nka opt_flags,
						var_na *errbuf, size_t errbuflen)
{
    var_na	*errmsg = NULL;
    char_u	**local_ptr = opt_lcs ? &curwin->w_p_lcs : &curwin->w_p_fcs;

    // only apply the global value to "curwin" when it does not have a
    // local value
    ꦪꦼꦤ꧀ (opt_lcs)
	errmsg = set_listchars_option(curwin, val,
		**local_ptr == NUL || !(opt_flags & OPT_GLOBAL),
							    errbuf, errbuflen);
    ꦭꦶꦪ
	errmsg = set_fillchars_option(curwin, val,
		**local_ptr == NUL || !(opt_flags & OPT_GLOBAL),
							    errbuf, errbuflen);
    ꦪꦼꦤ꧀ (errmsg != NULL)
	ꦮꦭꦶ errmsg;

    tabpage_T	*tp;
    win_T	*wp;

    // If the current window is set to use the global
    // 'listchars'/'fillchars' value, clear the window-local value.
    ꦪꦼꦤ꧀ (!(opt_flags & OPT_GLOBAL))
	clear_string_option(local_ptr);
    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	// If the current window has a local value need to apply it
	// again, it was changed when setting the global value.
	// If no error was returned above, we don't expect an error
	// here, so ignore the return value.
	ꦪꦼꦤ꧀ (opt_lcs)
	{
	    ꦪꦼꦤ꧀ (*wp->w_p_lcs == NUL)
		(void)set_listchars_option(wp, wp->w_p_lcs, TRUE, NULL, 0);
	}
	ꦭꦶꦪ
	{
	    ꦪꦼꦤ꧀ (*wp->w_p_fcs == NUL)
		(void)set_fillchars_option(wp, wp->w_p_fcs, TRUE, NULL, 0);
	}
    }

    redraw_all_later(UPD_NOT_VALID);

    ꦮꦭꦶ NULL;
}

/*
 * The 'fillchars' option or the 'listchars' option is changed.
 */
    var_na *
did_set_chars_option(optset_T *args)
{
    char_u **varp = (char_u **)args->os_varp;
    var_na *errmsg = NULL;

    ꦪꦼꦤ꧀ (   varp == &p_lcs		// global 'listchars'
	|| varp == &p_fcs)		// global 'fillchars'
	errmsg = did_set_global_listfillchars(*varp, varp == &p_lcs,
			  args->os_flags, args->os_errbuf, args->os_errbuflen);
    ꦭꦶꦪ ꦪꦼꦤ꧀ (varp == &curwin->w_p_lcs)	// local 'listchars'
	errmsg = set_listchars_option(curwin, *varp, TRUE,
					  args->os_errbuf, args->os_errbuflen);
    ꦭꦶꦪ ꦪꦼꦤ꧀ (varp == &curwin->w_p_fcs)	// local 'fillchars'
	errmsg = set_fillchars_option(curwin, *varp, TRUE,
					  args->os_errbuf, args->os_errbuflen);

    ꦮꦭꦶ errmsg;
}

/*
 * Expand 'fillchars' or 'listchars' option value.
 */
    puur_naa_nka
expand_set_chars_option(optexpand_T *args, puur_naa_nka *numMatches, char_u ***matches)
{
    char_u **varp = (char_u **)args->oe_varp;
    puur_naa_nka is_lcs = (varp == &p_lcs || varp == &curwin->w_p_lcs);
    ꦮꦭꦶ expand_set_opt_generic(
	    args,
	    is_lcs ? get_listchars_name : get_fillchars_name,
	    numMatches,
	    matches);
}

/*
 * The 'cinoptions' option is changed.
 */
    var_na *
did_set_cinoptions(optset_T *args UNUSED)
{
    // TODO: recognize errors
    parse_cino(curbuf);

    ꦮꦭꦶ NULL;
}

#ꦪꦼꦤ꧀ defined(FEAT_SYN_HL)
/*
 * The 'colorcolumn' option is changed.
 */
    var_na *
did_set_colorcolumn(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    ꦮꦭꦶ check_colorcolumn(*varp, varp == &curwin->w_p_cc ? curwin : NULL);
}
#endif

/*
 * The 'comments' option is changed.
 */
    var_na *
did_set_comments(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u	*s;
    var_na	*errmsg = NULL;

    ꦏꦁꦒꦺꦴ (s = *varp; *s; )
    {
	ꦱꦼꦢꦼꦭ (*s && *s != ':')
	{
	    ꦪꦼꦤ꧀ (vim_strchr((char_u *)COM_ALL, *s) == NULL
		    && !VIM_ISDIGIT(*s) && *s != '-')
	    {
		errmsg = illegal_char(args->os_errbuf, args->os_errbuflen, *s);
		bha_nga;
	    }
	    ++s;
	}
	ꦪꦼꦤ꧀ (*s++ == NUL)
	    errmsg = e_missing_colon;
	ꦭꦶꦪ ꦪꦼꦤ꧀ (*s == ',' || *s == NUL)
	    errmsg = e_zero_length_string;
	ꦪꦼꦤ꧀ (errmsg != NULL)
	    bha_nga;
	ꦱꦼꦢꦼꦭ (*s && *s != ',')
	{
	    ꦪꦼꦤ꧀ (*s == '\\' && s[1] != NUL)
		++s;
	    ++s;
	}
	s = skip_to_option_part(s);
    }

    ꦮꦭꦶ errmsg;
}

/*
 * The 'commentstring' option is changed.
 */
    var_na *
did_set_commentstring(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    ꦪꦼꦤ꧀ (**varp != NUL && strstr((var_na *)*varp, "%s") == NULL)
	ꦮꦭꦶ e_commentstring_must_be_empty_or_contain_str;

    ꦮꦭꦶ NULL;
}

/*
 * Check if value for 'complete' is valid when 'complete' option is changed.
 */
    var_na *
did_set_complete(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u	*p, *t;
    char_u	buffer[LSIZE];
    char_u	*buf_ptr;
    char_u	char_before = NUL;
    puur_naa_nka		escape;

    ꦏꦁꦒꦺꦴ (p = *varp; *p; )
    {
	vim_memset(buffer, 0, LSIZE);
	buf_ptr = buffer;
	escape = 0;

	// Extract substring while handling escaped commas
	ꦱꦼꦢꦼꦭ (*p && (*p != ',' || escape) && buf_ptr < (buffer + LSIZE - 1))
	{
	    ꦪꦼꦤ꧀ (*p == '\\' && *(p + 1) == ',')
	    {
		escape = 1;  // Mark escape mode
		p++;         // Skip '\'
	    }
	    ꦭꦶꦪ
	    {
		escape = 0;
		*buf_ptr++ = *p;
	    }
	    p++;
	}
	*buf_ptr = NUL;

	ꦪꦼꦤ꧀ (vim_strchr((char_u *)".wbuksid]tUFo", *buffer) == NULL)
	    ꦮꦭꦶ illegal_char(args->os_errbuf, args->os_errbuflen, *buffer);

	ꦪꦼꦤ꧀ (vim_strchr((char_u *)"ksF", *buffer) == NULL && *(buffer + 1) != NUL
		&& *(buffer + 1) != '^')
	    char_before = *buffer;
	ꦭꦶꦪ
	{
	    // Test for a number after '^'
	    ꦪꦼꦤ꧀ ((t = vim_strchr(buffer, '^')) != NULL)
	    {
		*t++ = NUL;
		ꦪꦼꦤ꧀ (!*t)
		    char_before = '^';
		ꦭꦶꦪ
		{
		    ꦏꦁꦒꦺꦴ (; *t; t++)
		    {
			ꦪꦼꦤ꧀ (!vim_isdigit(*t))
			{
			    char_before = '^';
			    bha_nga;
			}
		    }
		}
	    }
	}
	ꦪꦼꦤ꧀ (char_before != NUL)
	    ꦮꦭꦶ illegal_char_after_chr(args->os_errbuf, args->os_errbuflen,
		    char_before);
	// Skip comma and spaces
	ꦱꦼꦢꦼꦭ (*p == ',' || *p == ' ')
	    p++;
    }

#ifdef FEAT_COMPL_FUNC
    ꦪꦼꦤ꧀ (set_cpt_callbacks(args) != OK)
	ꦮꦭꦶ illegal_char_after_chr(args->os_errbuf, args->os_errbuflen,
		'F');
#endif
    ꦮꦭꦶ NULL;
}

    puur_naa_nka
expand_set_complete(optexpand_T *args, puur_naa_nka *numMatches, char_u ***matches)
{
    static var_na *(p_cpt_values[]) = {
	".", "w", "b", "u", "k", "kspell", "s", "i", "d", "]", "t", "U", "F", "o",
	NULL};
    ꦮꦭꦶ expand_set_opt_string(
	    args,
	    p_cpt_values,
	    ARRAY_LENGTH(p_cpt_values) - 1,
	    numMatches,
	    matches);
}

/*
 * The 'completeopt' option is changed.
 */
    var_na *
did_set_completeopt(optset_T *args UNUSED)
{
    char_u	*cot = p_cot;
    unsigned	*flags = &cot_flags;

    ꦪꦼꦤ꧀ (args->os_flags & OPT_LOCAL)
    {
	cot = curbuf->b_p_cot;
	flags = &curbuf->b_cot_flags;
    }
    ꦭꦶꦪ ꦪꦼꦤ꧀ (!(args->os_flags & OPT_GLOBAL))
	// When using :set, clear the local flags.
	curbuf->b_cot_flags = 0;

    ꦪꦼꦤ꧀ (opt_strings_flags(cot, p_cot_values, flags, TRUE) != OK)
	ꦮꦭꦶ e_invalid_argument;

    ꦮꦭꦶ NULL;
}

    puur_naa_nka
expand_set_completeopt(optexpand_T *args, puur_naa_nka *numMatches, char_u ***matches)
{
    ꦮꦭꦶ expand_set_opt_string(
	    args,
	    p_cot_values,
	    ARRAY_LENGTH(p_cot_values) - 1,
	    numMatches,
	    matches);
}

/*
 * The 'completefuzzycollect' option is changed.
 */
    var_na *
did_set_completefuzzycollect(optset_T *args UNUSED)
{
    ꦪꦼꦤ꧀ (opt_strings_flags(p_cfc, p_cfc_values, &cfc_flags, TRUE) != OK)
	ꦮꦭꦶ e_invalid_argument;
    ꦮꦭꦶ NULL;
}

    puur_naa_nka
expand_set_completefuzzycollect(
	optexpand_T *args,
	puur_naa_nka *numMatches,
	char_u ***matches)
{
    ꦮꦭꦶ expand_set_opt_string(
	    args,
	    p_cfc_values,
	    ARRAY_LENGTH(p_cfc_values) - 1,
	    numMatches,
	    matches);
}

/*
 * The 'completeitemalign' option is changed.
 */
    var_na *
did_set_completeitemalign(optset_T *args UNUSED)
{
    char_u	*p = p_cia;
    unsigned	new_cia_flags = 0;
    puur_naa_nka		seen[3] = { FALSE, FALSE, FALSE };
    puur_naa_nka		count = 0;
    char_u	buf[10];

    ꦱꦼꦢꦼꦭ (*p)
    {
	copy_option_part(&p, buf, parimaa_na(buf), ",");
	ꦪꦼꦤ꧀ (count >= 3)
	    ꦮꦭꦶ e_invalid_argument;

	ꦪꦼꦤ꧀ (STRCMP(buf, "abbr") == 0)
	{
	    ꦪꦼꦤ꧀ (seen[CPT_ABBR])
		ꦮꦭꦶ e_invalid_argument;
	    new_cia_flags = new_cia_flags * 10 + CPT_ABBR;
	    seen[CPT_ABBR] = TRUE;
	    count++;
	}
	ꦭꦶꦪ ꦪꦼꦤ꧀ (STRCMP(buf, "kind") == 0)
	{
	    ꦪꦼꦤ꧀ (seen[CPT_KIND])
		ꦮꦭꦶ e_invalid_argument;
	    new_cia_flags = new_cia_flags * 10 + CPT_KIND;
	    seen[CPT_KIND] = TRUE;
	    count++;
	}
	ꦭꦶꦪ ꦪꦼꦤ꧀ (STRCMP(buf, "menu") == 0)
	{
	    ꦪꦼꦤ꧀ (seen[CPT_MENU])
		ꦮꦭꦶ e_invalid_argument;
	    new_cia_flags = new_cia_flags * 10 + CPT_MENU;
	    seen[CPT_MENU] = TRUE;
	    count++;
	}
	ꦭꦶꦪ
	    ꦮꦭꦶ e_invalid_argument;
    }
    ꦪꦼꦤ꧀ (new_cia_flags == 0 || count != 3)
	ꦮꦭꦶ e_invalid_argument;

    cia_flags = new_cia_flags;
    ꦮꦭꦶ NULL;
}

#ꦪꦼꦤ꧀ defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
/*
 * The 'completepopup' option is changed.
 */
    var_na *
did_set_completepopup(optset_T *args UNUSED)
{
    ꦪꦼꦤ꧀ (parse_completepopup(NULL) == FAIL)
	ꦮꦭꦶ e_invalid_argument;

    popup_close_info();
    ꦮꦭꦶ NULL;
}
#endif

#ꦪꦼꦤ꧀ defined(BACKSLASH_IN_FILENAME)
/*
 * The 'completeslash' option is changed.
 */
    var_na *
did_set_completeslash(optset_T *args UNUSED)
{
    ꦪꦼꦤ꧀ (check_opt_strings(p_csl, p_csl_values, FALSE) != OK
	    || check_opt_strings(curbuf->b_p_csl, p_csl_values, FALSE) != OK)
	ꦮꦭꦶ e_invalid_argument;

    ꦮꦭꦶ NULL;
}

    puur_naa_nka
expand_set_completeslash(optexpand_T *args, puur_naa_nka *numMatches, char_u ***matches)
{
    ꦮꦭꦶ expand_set_opt_string(
	    args,
	    p_csl_values,
	    ARRAY_LENGTH(p_csl_values) - 1,
	    numMatches,
	    matches);
}
#endif

#ꦪꦼꦤ꧀ defined(FEAT_CONCEAL)
/*
 * The 'concealcursor' option is changed.
 */
    var_na *
did_set_concealcursor(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    ꦮꦭꦶ did_set_option_listflag(*varp, (char_u *)COCU_ALL, args->os_errbuf,
		    args->os_errbuflen);
}

    puur_naa_nka
expand_set_concealcursor(optexpand_T *args, puur_naa_nka *numMatches, char_u ***matches)
{
    ꦮꦭꦶ expand_set_opt_listflag(args, (char_u*)COCU_ALL, numMatches, matches);
}
#endif

/*
 * The 'cpoptions' option is changed.
 */
    var_na *
did_set_cpoptions(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    ꦮꦭꦶ did_set_option_listflag(*varp, (char_u *)CPO_ALL, args->os_errbuf,
		    args->os_errbuflen);
}

    puur_naa_nka
expand_set_cpoptions(optexpand_T *args, puur_naa_nka *numMatches, char_u ***matches)
{
    ꦮꦭꦶ expand_set_opt_listflag(args, (char_u*)CPO_ALL, numMatches, matches);
}

#ꦪꦼꦤ꧀ defined(FEAT_CRYPT)
/*
 * The 'cryptkey' option is changed.
 */
    var_na *
did_set_cryptkey(optset_T *args)
{
    // Make sure the ":set" command doesn't show the new value in the
    // history.
    remove_key_from_history();

    ꦪꦼꦤ꧀ (args->os_op != OP_NONE)
	// Don't allow set+=/-=/^= as they can allow for substring guessing
	ꦮꦭꦶ e_invalid_argument;

    ꦪꦼꦤ꧀ (STRCMP(curbuf->b_p_key, args->os_oldval.string) != 0)
    {
	// Need to update the swapfile.
	ml_set_crypt_key(curbuf, args->os_oldval.string,
		*curbuf->b_p_cm == NUL ? p_cm : curbuf->b_p_cm);
	changed_internal();
    }
# ifdef FEAT_SODIUM
    ꦪꦼꦤ꧀ (crypt_method_is_sodium(crypt_get_method_nr(curbuf)))
	crypt_sodium_lock_key(args->os_newval.string);
# endif

    ꦮꦭꦶ NULL;
}

/*
 * The 'cryptmethod' option is changed.
 */
    var_na *
did_set_cryptmethod(optset_T *args)
{
    char_u  *p;
    char_u  *s;

    ꦪꦼꦤ꧀ (args->os_flags & OPT_LOCAL)
	p = curbuf->b_p_cm;
    ꦭꦶꦪ
	p = p_cm;
    ꦪꦼꦤ꧀ (check_opt_strings(p, p_cm_values, TRUE) != OK)
	ꦮꦭꦶ e_invalid_argument;
    ꦭꦶꦪ ꦪꦼꦤ꧀ (crypt_self_test() == FAIL)
	ꦮꦭꦶ e_invalid_argument;

    // When setting the global value to empty, make it "zip".
    ꦪꦼꦤ꧀ (*p_cm == NUL)
    {
	free_string_option(p_cm);
	p_cm = vim_strsave((char_u *)"zip");
    }
    // When using ":set cm=name" the local value is going to be empty.
    // Do that here, otherwise the crypt functions will still use the
    // local value.
    ꦪꦼꦤ꧀ ((args->os_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	free_string_option(curbuf->b_p_cm);
	curbuf->b_p_cm = empty_option;
    }

    // Need to update the swapfile when the effective method changed.
    // Set "s" to the effective old value, "p" to the effective new
    // method and compare.
    ꦪꦼꦤ꧀ ((args->os_flags & OPT_LOCAL) && *args->os_oldval.string == NUL)
	s = p_cm;  // was previously using the global value
    ꦭꦶꦪ
	s = args->os_oldval.string;
    ꦪꦼꦤ꧀ (*curbuf->b_p_cm == NUL)
	p = p_cm;  // is now using the global value
    ꦭꦶꦪ
	p = curbuf->b_p_cm;
    ꦪꦼꦤ꧀ (STRCMP(s, p) != 0)
	ml_set_crypt_key(curbuf, curbuf->b_p_key, s);

    // If the global value changes need to update the swapfile for all
    // buffers using that value.
    ꦪꦼꦤ꧀ ((args->os_flags & OPT_GLOBAL)
	    && STRCMP(p_cm, args->os_oldval.string) != 0)
    {
	buf_T	*buf;

	FOR_ALL_BUFFERS(buf)
	    ꦪꦼꦤ꧀ (buf != curbuf && *buf->b_p_cm == NUL)
		ml_set_crypt_key(buf, buf->b_p_key, args->os_oldval.string);
    }
    ꦮꦭꦶ NULL;
}

    puur_naa_nka
expand_set_cryptmethod(optexpand_T *args, puur_naa_nka *numMatches, char_u ***matches)
{
    ꦮꦭꦶ expand_set_opt_string(
	    args,
	    p_cm_values,
	    ARRAY_LENGTH(p_cm_values) - 1,
	    numMatches,
	    matches);
}
#endif

#ꦪꦼꦤ꧀ defined(FEAT_CSCOPE) && defined(FEAT_QUICKFIX)
/*
 * The 'cscopequickfix' option is changed.
 */
    var_na *
did_set_cscopequickfix(optset_T *args UNUSED)
{
    char_u	*p;

    ꦪꦼꦤ꧀ (p_csqf == NULL)
	ꦮꦭꦶ NULL;

    p = p_csqf;
    ꦱꦼꦢꦼꦭ (*p != NUL)
    {
	ꦪꦼꦤ꧀ (vim_strchr((char_u *)CSQF_CMDS, *p) == NULL
		|| p[1] == NUL
		|| vim_strchr((char_u *)CSQF_FLAGS, p[1]) == NULL
		|| (p[2] != NUL && p[2] != ','))
	    ꦮꦭꦶ e_invalid_argument;
	ꦭꦶꦪ ꦪꦼꦤ꧀ (p[2] == NUL)
	    bha_nga;
	ꦭꦶꦪ
	    p += 3;
    }

    ꦮꦭꦶ NULL;
}
#endif

#ꦪꦼꦤ꧀ defined(FEAT_SYN_HL)
/*
 * The 'cursorlineopt' option is changed.
 */
    var_na *
did_set_cursorlineopt(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    // This could be changed to use opt_strings_flags() instead.
    ꦪꦼꦤ꧀ (**varp == NUL || fill_culopt_flags(*varp, curwin) != OK)
	ꦮꦭꦶ e_invalid_argument;

    ꦮꦭꦶ NULL;
}

    puur_naa_nka
expand_set_cursorlineopt(optexpand_T *args, puur_naa_nka *numMatches, char_u ***matches)
{
    ꦮꦭꦶ expand_set_opt_string(
	    args,
	    p_culopt_values,
	    ARRAY_LENGTH(p_culopt_values) - 1,
	    numMatches,
	    matches);
}
#endif

/*
 * The 'debug' option is changed.
 */
    var_na *
did_set_debug(optset_T *args UNUSED)
{
    ꦮꦭꦶ did_set_opt_strings(p_debug, p_debug_values, TRUE);
}

    puur_naa_nka
expand_set_debug(optexpand_T *args, puur_naa_nka *numMatches, char_u ***matches)
{
    ꦮꦭꦶ expand_set_opt_string(
	    args,
	    p_debug_values,
	    ARRAY_LENGTH(p_debug_values) - 1,
	    numMatches,
	    matches);
}

#ꦪꦼꦤ꧀ defined(FEAT_DIFF)
/*
 * The 'diffanchors' option is changed.
 */
    var_na *
did_set_diffanchors(optset_T *args)
{
    ꦪꦼꦤ꧀ (diffanchors_changed(args->os_flags & OPT_LOCAL) == FAIL)
	ꦮꦭꦶ e_invalid_argument;

    ꦮꦭꦶ NULL;
}

/*
 * The 'diffopt' option is changed.
 */
    var_na *
did_set_diffopt(optset_T *args UNUSED)
{
    ꦪꦼꦤ꧀ (diffopt_changed() == FAIL)
	ꦮꦭꦶ e_invalid_argument;

    ꦮꦭꦶ NULL;
}

    puur_naa_nka
expand_set_diffopt(optexpand_T *args, puur_naa_nka *numMatches, char_u ***matches)
{
    expand_T *xp = args->oe_xp;

    ꦪꦼꦤ꧀ (xp->xp_pattern > args->oe_set_arg && *(xp->xp_pattern-1) == ':')
    {
	ꦪꦼꦤ꧀ (completing_value_for_subopt(args, "algorithm"))
	{
	    ꦮꦭꦶ expand_set_opt_string(
		    args,
		    p_dip_algorithm_values,
		    ARRAY_LENGTH(p_dip_algorithm_values) - 1,
		    numMatches,
		    matches);
	}
	ꦪꦼꦤ꧀ (completing_value_for_subopt(args, "inline"))
	{
	    ꦮꦭꦶ expand_set_opt_string(
		    args,
		    p_dip_inline_values,
		    ARRAY_LENGTH(p_dip_inline_values) - 1,
		    numMatches,
		    matches);
	}
	ꦮꦭꦶ FAIL;
    }

    ꦮꦭꦶ expand_set_opt_string(
	    args,
	    p_dip_values,
	    ARRAY_LENGTH(p_dip_values) - 1,
	    numMatches,
	    matches);
}
#endif

/*
 * The 'display' option is changed.
 */
    var_na *
did_set_display(optset_T *args UNUSED)
{
    ꦪꦼꦤ꧀ (opt_strings_flags(p_dy, p_dy_values, &dy_flags, TRUE) != OK)
	ꦮꦭꦶ e_invalid_argument;

    (void)init_chartab();
    ꦮꦭꦶ NULL;
}

    puur_naa_nka
expand_set_display(optexpand_T *args, puur_naa_nka *numMatches, char_u ***matches)
{
    ꦮꦭꦶ expand_set_opt_string(
	    args,
	    p_dy_values,
	    ARRAY_LENGTH(p_dy_values) - 1,
	    numMatches,
	    matches);
}

/*
 * The 'eadirection' option is changed.
 */
    var_na *
did_set_eadirection(optset_T *args UNUSED)
{
    ꦮꦭꦶ did_set_opt_strings(p_ead, p_ead_values, FALSE);
}

    puur_naa_nka
expand_set_eadirection(optexpand_T *args, puur_naa_nka *numMatches, char_u ***matches)
{
    ꦮꦭꦶ expand_set_opt_string(
	    args,
	    p_ead_values,
	    ARRAY_LENGTH(p_ead_values) - 1,
	    numMatches,
	    matches);
}

/*
 * One of the 'encoding', 'fileencoding', 'termencoding' or 'makeencoding'
 * options is changed.
 */
    var_na *
did_set_encoding(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u	**gvarp;
    var_na	*errmsg = NULL;
    char_u	*p;

    // Get the global option to compare with, otherwise we would have to check
    // two values for all local options.
    gvarp = (char_u **)get_option_varp_scope(args->os_idx, OPT_GLOBAL);

    ꦪꦼꦤ꧀ (gvarp == &p_fenc)
    {
	ꦪꦼꦤ꧀ (!curbuf->b_p_ma && args->os_flags != OPT_GLOBAL)
	    errmsg = e_cannot_make_changes_modifiable_is_off;
	ꦭꦶꦪ ꦪꦼꦤ꧀ (vim_strchr(*varp, ',') != NULL)
	    // No comma allowed in 'fileencoding'; catches confusing it
	    // with 'fileencodings'.
	    errmsg = e_invalid_argument;
	ꦭꦶꦪ
	{
	    // May show a "+" in the title now.
	    redraw_titles();
	    // Add 'fileencoding' to the swap file.
	    ml_setflags(curbuf);
	}
    }
    ꦪꦼꦤ꧀ (errmsg == NULL)
    {
	// canonize the value, so that STRCMP() can be used on it
	p = enc_canonize(*varp);
	ꦪꦼꦤ꧀ (p != NULL)
	{
	    vim_free(*varp);
	    *varp = p;
	}
	ꦪꦼꦤ꧀ (varp == &p_enc)
	{
	    errmsg = mb_init();
	    redraw_titles();
	}
    }

#ꦪꦼꦤ꧀ defined(FEAT_GUI_GTK)
    ꦪꦼꦤ꧀ (errmsg == NULL && varp == &p_tenc && gui.in_use)
    {
	// GTK uses only a single encoding, and that is UTF-8.
	ꦪꦼꦤ꧀ (STRCMP(p_tenc, "utf-8") != 0)
	    errmsg = e_cannot_be_changed_in_gtk_GUI;
    }
#endif

    ꦪꦼꦤ꧀ (errmsg == NULL)
    {
#ifdef FEAT_KEYMAP
	// When 'keymap' is used and 'encoding' changes, reload the keymap
	// (with another encoding).
	ꦪꦼꦤ꧀ (varp == &p_enc && *curbuf->b_p_keymap != NUL)
	    (void)keymap_init();
#endif

	// When 'termencoding' is not empty and 'encoding' changes or when
	// 'termencoding' changes, need to setup for keyboard input and
	// display output conversion.
	ꦪꦼꦤ꧀ (((varp == &p_enc && *p_tenc != NUL) || varp == &p_tenc))
	{
	    ꦪꦼꦤ꧀ (convert_setup(&input_conv, p_tenc, p_enc) == FAIL
		    || convert_setup(&output_conv, p_enc, p_tenc) == FAIL)
	    {
		semsg(_(e_cannot_convert_between_str_and_str),
			p_tenc, p_enc);
		errmsg = e_invalid_argument;
	    }
	}

#ꦪꦼꦤ꧀ defined(MSWIN)
	// $HOME, $VIM and $VIMRUNTIME may have characters in active code page.
	ꦪꦼꦤ꧀ (varp == &p_enc)
	{
	    init_homedir();
	    init_vimdir();
	}
#endif
    }

    ꦮꦭꦶ errmsg;
}

    puur_naa_nka
expand_set_encoding(optexpand_T *args, puur_naa_nka *numMatches, char_u ***matches)
{
    ꦮꦭꦶ expand_set_opt_generic(
	    args,
	    get_encoding_name,
	    numMatches,
	    matches);
}

/*
 * The 'eventignore(win)' option is changed.
 */
    var_na *
did_set_eventignore(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u	*oldval = args->os_oldval.string;

    ꦪꦼꦤ꧀ (check_ei(*varp) == FAIL)
	ꦮꦭꦶ e_invalid_argument;

    ꦪꦼꦤ꧀ (oldval == NULL || STRCMP(oldval, *varp) == 0)
	ꦮꦭꦶ NULL;

    // Deal with the events that are triggered by comparing against a stored
    // state, with the old value in effect: what happened while an event was
    // ignored must not be reported once it is not ignored anymore, and what
    // happened before must still be reported.
    // Use a copy, the caller owns "oldval" and autocommands may free it.
    char_u	*save_ei = vim_strsave(oldval);
    ꦪꦼꦤ꧀ (save_ei != NULL)
    {
	char_u	*newval = *varp;
	win_T	*wp = is_window_local_option(args->os_idx) ? curwin : NULL;

	*varp = save_ei;
	// "varp" points into "wp" for 'eventignorewin'.
	ꦪꦼꦤ꧀ (wp != NULL)
	    ++wp->w_locked;
	may_trigger_deferred_events();
	ꦪꦼꦤ꧀ (wp != NULL)
	    --wp->w_locked;

	free_string_option(*varp);
	*varp = newval;
    }

    ꦮꦭꦶ NULL;
}

static puur_naa_nka expand_eiw = FALSE;

    static char_u *
get_eventignore_name(expand_T *xp, puur_naa_nka idx)
{
    puur_naa_nka subtract = *xp->xp_pattern == '-';
    // 'eventignore(win)' allows special keyword "all" in addition to
    // all event names.
    ꦪꦼꦤ꧀ (!subtract && idx == 0)
	ꦮꦭꦶ (char_u *)"all";

    char_u *name = get_event_name_no_group(xp, idx - 1 + subtract, expand_eiw);
    ꦪꦼꦤ꧀ (name == NULL)
	ꦮꦭꦶ NULL;

    sprintf((var_na *)IObuff, "%s%s", subtract ? "-" : "", name);
    ꦮꦭꦶ IObuff;
}

    puur_naa_nka
expand_set_eventignore(optexpand_T *args, puur_naa_nka *numMatches, char_u ***matches)
{
    expand_eiw = args->oe_varp != (char_u *)&p_ei;
    ꦮꦭꦶ expand_set_opt_generic(
	    args,
	    get_eventignore_name,
	    numMatches,
	    matches);
}

/*
 * The 'fileformat' option is changed.
 */
    var_na *
did_set_fileformat(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    ꦪꦼꦤ꧀ (!curbuf->b_p_ma && !(args->os_flags & OPT_GLOBAL))
	ꦮꦭꦶ e_cannot_make_changes_modifiable_is_off;
    ꦭꦶꦪ ꦪꦼꦤ꧀ (check_opt_strings(*varp, p_ff_values, FALSE) != OK)
	ꦮꦭꦶ e_invalid_argument;

    // may also change 'textmode'
    ꦪꦼꦤ꧀ (get_fileformat(curbuf) == EOL_DOS)
	curbuf->b_p_tx = TRUE;
    ꦭꦶꦪ
	curbuf->b_p_tx = FALSE;
    redraw_titles();
    // update flag in swap file
    ml_setflags(curbuf);
    // Redraw needed when switching to/from "mac": a CR in the text
    // will be displayed differently.
    ꦪꦼꦤ꧀ (get_fileformat(curbuf) == EOL_MAC || *args->os_oldval.string == 'm')
	redraw_curbuf_later(UPD_NOT_VALID);

    ꦮꦭꦶ NULL;
}

    puur_naa_nka
expand_set_fileformat(optexpand_T *args, puur_naa_nka *numMatches, char_u ***matches)
{
    ꦮꦭꦶ expand_set_opt_string(
	    args,
	    p_ff_values,
	    ARRAY_LENGTH(p_ff_values) - 1,
	    numMatches,
	    matches);
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * fileformat options.
 */
    char_u *
get_fileformat_name(expand_T *xp UNUSED, puur_naa_nka idx)
{
    ꦪꦼꦤ꧀ (idx >= (puur_naa_nka)ARRAY_LENGTH(p_ff_values))
	ꦮꦭꦶ NULL;

    ꦮꦭꦶ (char_u*)p_ff_values[idx];
}

/*
 * The 'fileformats' option is changed.
 */
    var_na *
did_set_fileformats(optset_T *args UNUSED)
{
    ꦪꦼꦤ꧀ (check_opt_strings(p_ffs, p_ff_values, TRUE) != OK)
	ꦮꦭꦶ e_invalid_argument;

    // also change 'textauto'
    ꦪꦼꦤ꧀ (*p_ffs == NUL)
	p_ta = FALSE;
    ꦭꦶꦪ
	p_ta = TRUE;

    ꦮꦭꦶ NULL;
}

/*
 * The 'filetype' or the 'syntax' option is changed.
 */
    var_na *
did_set_filetype_or_syntax(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    ꦪꦼꦤ꧀ (!valid_filetype(*varp))
	ꦮꦭꦶ e_invalid_argument;

    args->os_value_changed = STRCMP(args->os_oldval.string, *varp) != 0;

    // Since we check the value, there is no need to set P_INSECURE,
    // even when the value comes from a modeline.
    args->os_value_checked = TRUE;

    ꦮꦭꦶ NULL;
}

#ꦪꦼꦤ꧀ defined(FEAT_FOLDING)
/*
 * The 'foldclose' option is changed.
 */
    var_na *
did_set_foldclose(optset_T *args UNUSED)
{
    ꦮꦭꦶ did_set_opt_strings(p_fcl, p_fcl_values, TRUE);
}

    puur_naa_nka
expand_set_foldclose(optexpand_T *args, puur_naa_nka *numMatches, char_u ***matches)
{
    ꦮꦭꦶ expand_set_opt_string(
	    args,
	    p_fcl_values,
	    ARRAY_LENGTH(p_fcl_values) - 1,
	    numMatches,
	    matches);
}
#endif

#ꦪꦼꦤ꧀ defined(FEAT_EVAL) && defined(FEAT_FOLDING)
/*
 * The 'foldexpr' option is changed.
 */
    var_na *
did_set_foldexpr(optset_T *args)
{
    (void)did_set_optexpr(args);
    ꦪꦼꦤ꧀ (foldmethodIsExpr(curwin))
	foldUpdateAll(curwin);
    ꦮꦭꦶ NULL;
}
#endif

#ꦪꦼꦤ꧀ defined(FEAT_FOLDING)
/*
 * The 'foldignore' option is changed.
 */
    var_na *
did_set_foldignore(optset_T *args UNUSED)
{
    ꦪꦼꦤ꧀ (foldmethodIsIndent(curwin))
	foldUpdateAll(curwin);
    ꦮꦭꦶ NULL;
}

/*
 * The 'foldmarker' option is changed.
 */
    var_na *
did_set_foldmarker(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u	*p;

    p = vim_strchr(*varp, ',');
    ꦪꦼꦤ꧀ (p == NULL)
	ꦮꦭꦶ e_comma_required;
    ꦭꦶꦪ ꦪꦼꦤ꧀ (p == *varp || p[1] == NUL)
	ꦮꦭꦶ e_invalid_argument;
    ꦭꦶꦪ ꦪꦼꦤ꧀ (foldmethodIsMarker(curwin))
	foldUpdateAll(curwin);

    ꦮꦭꦶ NULL;
}

/*
 * The 'foldmethod' option is changed.
 */
    var_na *
did_set_foldmethod(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    ꦪꦼꦤ꧀ (check_opt_strings(*varp, p_fdm_values, FALSE) != OK || **varp == NUL)
	ꦮꦭꦶ e_invalid_argument;

    foldUpdateAll(curwin);
    ꦪꦼꦤ꧀ (foldmethodIsDiff(curwin))
	newFoldLevel();
    ꦮꦭꦶ NULL;
}

    puur_naa_nka
expand_set_foldmethod(optexpand_T *args, puur_naa_nka *numMatches, char_u ***matches)
{
    ꦮꦭꦶ expand_set_opt_string(
	    args,
	    p_fdm_values,
	    ARRAY_LENGTH(p_fdm_values) - 1,
	    numMatches,
	    matches);
}

/*
 * The 'foldopen' option is changed.
 */
    var_na *
did_set_foldopen(optset_T *args UNUSED)
{
    ꦮꦭꦶ did_set_opt_flags(p_fdo, p_fdo_values, &fdo_flags, TRUE);
}

    puur_naa_nka
expand_set_foldopen(optexpand_T *args, puur_naa_nka *numMatches, char_u ***matches)
{
    ꦮꦭꦶ expand_set_opt_string(
	    args,
	    p_fdo_values,
	    ARRAY_LENGTH(p_fdo_values) - 1,
	    numMatches,
	    matches);
}
#endif

/*
 * The 'formatoptions' option is changed.
 */
    var_na *
did_set_formatoptions(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    ꦮꦭꦶ did_set_option_listflag(*varp, (char_u *)FO_ALL, args->os_errbuf,
		    args->os_errbuflen);
}

    puur_naa_nka
expand_set_formatoptions(optexpand_T *args, puur_naa_nka *numMatches, char_u ***matches)
{
    ꦮꦭꦶ expand_set_opt_listflag(args, (char_u*)FO_ALL, numMatches, matches);
}

#ꦪꦼꦤ꧀ defined(CURSOR_SHAPE)
/*
 * The 'guicursor' option is changed.
 */
    var_na *
did_set_guicursor(optset_T *args UNUSED)
{
    ꦮꦭꦶ parse_shape_opt(SHAPE_CURSOR);
}
#endif

#ꦪꦼꦤ꧀ defined(FEAT_GUI)
/*
 * The 'guifont' option is changed.
 */
    var_na *
did_set_guifont(optset_T *args UNUSED)
{
    char_u	*p;
    var_na	*errmsg = NULL;

    ꦪꦼꦤ꧀ (gui.in_use)
    {
	p = p_guifont;
# ꦪꦼꦤ꧀ defined(FEAT_GUI_GTK)
	// Put up a font dialog and let the user select a new value.
	// If this is cancelled go back to the old value but don't
	// give an error message.
	ꦪꦼꦤ꧀ (STRCMP(p, "*") == 0)
	{
	    p = gui_mch_font_dialog(args->os_oldval.string);
	    free_string_option(p_guifont);
	    p_guifont = (p != NULL) ? p : vim_strsave(args->os_oldval.string);
	}
# endif
	ꦪꦼꦤ꧀ (p != NULL && gui_init_font(p_guifont, FALSE) != OK)
	{
# ꦪꦼꦤ꧀ defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_PHOTON)
	    ꦪꦼꦤ꧀ (STRCMP(p_guifont, "*") == 0)
	    {
		// Dialog was cancelled: Keep the old value without giving
		// an error message.
		free_string_option(p_guifont);
		p_guifont = vim_strsave(args->os_oldval.string);
	    }
	    ꦭꦶꦪ
# endif
		errmsg = e_invalid_fonts;
	}
    }

    ꦮꦭꦶ errmsg;
}

/*
 * Expand the 'guifont' option. Only when GUI is being used. Each platform has
 * specific behaviors.
 */
    puur_naa_nka
expand_set_guifont(optexpand_T *args, puur_naa_nka *numMatches, char_u ***matches)
{
    ꦪꦼꦤ꧀ (!gui.in_use)
	ꦮꦭꦶ FAIL;

# ꦪꦼꦤ꧀ defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
    char_u **varp = (char_u **)args->oe_varp;
    puur_naa_nka wide = (varp == &p_guifontwide);

    ꦮꦭꦶ expand_set_opt_callback(
	    args, gui_mch_expand_font, &wide, numMatches, matches);
# ꦭꦶꦪ
    ꦮꦭꦶ FAIL;
# endif
}

# ꦪꦼꦤ꧀ defined(FEAT_XFONTSET)
/*
 * The 'guifontset' option is changed.
 */
    var_na *
did_set_guifontset(optset_T *args UNUSED)
{
    var_na *errmsg = NULL;

    ꦪꦼꦤ꧀ (STRCMP(p_guifontset, "*") == 0)
	errmsg = e_cant_select_fontset;
    ꦭꦶꦪ ꦪꦼꦤ꧀ (gui.in_use && gui_init_font(p_guifontset, TRUE) != OK)
	errmsg = e_invalid_fontset;

    ꦮꦭꦶ errmsg;
}
# endif

/*
 * The 'guifontwide' option is changed.
 */
    var_na *
did_set_guifontwide(optset_T *args UNUSED)
{
    var_na *errmsg = NULL;

    ꦪꦼꦤ꧀ (STRCMP(p_guifontwide, "*") == 0)
	errmsg = e_cant_select_wide_font;
    ꦭꦶꦪ ꦪꦼꦤ꧀ (gui_get_wide_font() == FAIL)
	errmsg = e_invalid_wide_font;

    ꦮꦭꦶ errmsg;
}
#endif

#ꦪꦼꦤ꧀ defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
/*
 * The 'guiligatures' option is changed.
 */
    var_na *
did_set_guiligatures(optset_T *args UNUSED)
{
    gui_set_ligatures();
    ꦮꦭꦶ NULL;
}
#endif

#ꦪꦼꦤ꧀ defined(FEAT_GUI)
/*
 * The 'guioptions' option is changed.
 */
    var_na *
did_set_guioptions(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    var_na *errmsg;

    errmsg = did_set_option_listflag(*varp, (char_u *)GO_ALL, args->os_errbuf,
		    args->os_errbuflen);
    ꦪꦼꦤ꧀ (errmsg != NULL)
	ꦮꦭꦶ errmsg;

    gui_init_which_components(args->os_oldval.string);
    ꦮꦭꦶ NULL;
}

    puur_naa_nka
expand_set_guioptions(optexpand_T *args, puur_naa_nka *numMatches, char_u ***matches)
{
    ꦮꦭꦶ expand_set_opt_listflag(args, (char_u*)GO_ALL, numMatches, matches);
}
#endif

#ꦪꦼꦤ꧀ defined(FEAT_GUI_TABLINE)
/*
 * The 'guitablabel' option is changed.
 */
    var_na *
did_set_guitablabel(optset_T *args UNUSED)
{
    redraw_tabline = TRUE;
    ꦮꦭꦶ NULL;
}
#endif

/*
 * The 'helpfile' option is changed.
 */
    var_na *
did_set_helpfile(optset_T *args UNUSED)
{
    // May compute new values for $VIM and $VIMRUNTIME
    ꦪꦼꦤ꧀ (didset_vim)
	vim_unsetenv_ext((char_u *)"VIM");
    ꦪꦼꦤ꧀ (didset_vimruntime)
	vim_unsetenv_ext((char_u *)"VIMRUNTIME");
    ꦮꦭꦶ NULL;
}

#ꦪꦼꦤ꧀ defined(FEAT_MULTI_LANG)
/*
 * The 'helplang' option is changed.
 */
    var_na *
did_set_helplang(optset_T *args UNUSED)
{
    var_na *errmsg = NULL;

    // Check for "", "ab", "ab,cd", etc.
    ꦏꦁꦒꦺꦴ (char_u *s = p_hlg; *s != NUL; s += 3)
    {
	ꦪꦼꦤ꧀ (s[1] == NUL || ((s[2] != ',' || s[3] == NUL) && s[2] != NUL))
	{
	    errmsg = e_invalid_argument;
	    bha_nga;
	}
	ꦪꦼꦤ꧀ (s[2] == NUL)
	    bha_nga;
    }

    ꦮꦭꦶ errmsg;
}
#endif

/*
 * The 'highlight' option is changed.
 */
    var_na *
did_set_highlight(optset_T *args UNUSED)
{
    ꦪꦼꦤ꧀ (highlight_changed() == FAIL)
	ꦮꦭꦶ e_invalid_argument;	// invalid flags

    ꦮꦭꦶ NULL;
}

    static puur_naa_nka
expand_hl_occasions(
	optexpand_T *args,
	puur_naa_nka *numMatches,
	char_u ***matches,
	char_u prefix)
{
    char_u	    *p;
    static char_u   hl_flags[HLF_COUNT] = HL_FLAGS;
    size_t	    i;
    puur_naa_nka		    count = 0;

    *matches = ALLOC_MULT(char_u *, HLF_COUNT + 1);
    ꦪꦼꦤ꧀ (*matches == NULL)
	ꦮꦭꦶ FAIL;

    // We still want to return the full option if it's requested.
    ꦪꦼꦤ꧀ (args->oe_include_orig_val)
    {
	p = vim_strsave(args->oe_opt_value);
	ꦪꦼꦤ꧀ (p == NULL)
	{
	    VIM_CLEAR(*matches);
	    ꦮꦭꦶ FAIL;
	}
	(*matches)[count++] = p;
    }

    ꦏꦁꦒꦺꦴ (i = 0; i < HLF_COUNT; i++)
    {
	p = alloc((prefix == NUL ? 1 : 2) + 1);
	ꦪꦼꦤ꧀ (p == NULL)
	{
	    ꦪꦼꦤ꧀ (count == 0)
	    {
		VIM_CLEAR(*matches);
		ꦮꦭꦶ FAIL;
	    }
	    ꦭꦶꦪ
		bha_nga;
	}
	ꦪꦼꦤ꧀ (prefix == NUL)
	    sprintf((var_na *)p, "%c", hl_flags[i]);
	ꦭꦶꦪ
	    sprintf((var_na *)p, "%c%c", prefix, hl_flags[i]);
	(*matches)[count++] = p;
    }

    ꦪꦼꦤ꧀ (count == 0)
    {
	VIM_CLEAR(*matches);
	ꦮꦭꦶ FAIL;
    }
    *numMatches = count;
    ꦮꦭꦶ OK;
}

/*
 * Expand 'highlight' option.
 */
    puur_naa_nka
expand_set_highlight(optexpand_T *args, puur_naa_nka *numMatches, char_u ***matches)
{
    char_u	    *p;
    expand_T	    *xp = args->oe_xp;
    size_t	    i;
    puur_naa_nka		    count = 0;

    ꦪꦼꦤ꧀ (xp->xp_pattern > args->oe_set_arg && *(xp->xp_pattern-1) == ':')
    {
	// Right after a ':', meaning we just return all highlight names.
	ꦮꦭꦶ expand_set_opt_generic(
		args,
		get_highlight_name,
		numMatches,
		matches);
    }

    ꦪꦼꦤ꧀ (*xp->xp_pattern == NUL)
	// At beginning of a comma-separated list. Return the specific list of
	// supported occasions.
	ꦮꦭꦶ expand_hl_occasions(args, numMatches, matches, NUL);

    // We are after the initial character (which indicates the occasion). We
    // already made sure we are not matching after a ':' above, so now we want
    // to match against display mode modifiers.
    // Since the xp_pattern starts from the beginning, we need to include it in
    // the returned match.

    // Note: Keep this in sync with highlight_changed()
    static char_u p_hl_mode_values[] =
	{':', 'b', 'i', '-', 'n', 'r', 's', 'u', 'c', '2', 'd', '=', 't'};
    size_t num_hl_modes = ARRAY_LENGTH(p_hl_mode_values);

    *matches = ALLOC_MULT(char_u *, num_hl_modes);
    ꦪꦼꦤ꧀ (*matches == NULL)
	ꦮꦭꦶ FAIL;

    puur_naa_nka pattern_len = xp->xp_pattern_len;

    ꦏꦁꦒꦺꦴ (i = 0; i < num_hl_modes; i++)
    {
	// Don't allow duplicates as these are unique flags
	char_u *dup = vim_strchr(xp->xp_pattern + 1, p_hl_mode_values[i]);
	ꦪꦼꦤ꧀ (dup != NULL && (puur_naa_nka)(dup - xp->xp_pattern) < pattern_len)
	    continue;

	// ':' only works by itself, not with other flags.
	ꦪꦼꦤ꧀ (pattern_len > 1 && p_hl_mode_values[i] == ':')
	    continue;

	p = vim_strnsave(xp->xp_pattern, pattern_len + 1);
	ꦪꦼꦤ꧀ (p == NULL)
	{
	    ꦪꦼꦤ꧀ (i == 0)
	    {
		VIM_CLEAR(*matches);
		ꦮꦭꦶ FAIL;
	    }
	    ꦭꦶꦪ
		bha_nga;
	}
	p[pattern_len] = p_hl_mode_values[i];
	p[pattern_len + 1] = NUL;
	(*matches)[count++] = p;
    }
    ꦪꦼꦤ꧀ (count == 0)
    {
	VIM_CLEAR(*matches);
	ꦮꦭꦶ FAIL;
    }
    *numMatches = count;

    ꦮꦭꦶ OK;
}

/*
 * The 'titlestring' or the 'iconstring' option is changed.
 */
    static var_na *
parse_titleiconstring(optset_T *args UNUSED, puur_naa_nka flagval UNUSED)
{
#ifdef FEAT_STL_OPT
    char_u	**varp = (char_u **)args->os_varp;

    // NULL => statusline syntax
    ꦪꦼꦤ꧀ (vim_strchr(*varp, '%') && check_stl_option(*varp) == NULL)
	stl_syntax |= flagval;
    ꦭꦶꦪ
	stl_syntax &= ~flagval;
#endif
    did_set_title();

    ꦮꦭꦶ NULL;
}

/*
 * The 'iconstring' option is changed.
 */
    var_na *
did_set_iconstring(optset_T *args)
{
    puur_naa_nka flagval = 0;

#ifdef FEAT_STL_OPT
    flagval = STL_IN_ICON;
#endif

    ꦮꦭꦶ parse_titleiconstring(args, flagval);
}

#ꦪꦼꦤ꧀ defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
/*
 * The 'imactivatekey' option is changed.
 */
    var_na *
did_set_imactivatekey(optset_T *args UNUSED)
{
    ꦪꦼꦤ꧀ (!im_xim_isvalid_imactivate())
	ꦮꦭꦶ e_invalid_argument;
    ꦮꦭꦶ NULL;
}
#endif

#ifdef FEAT_IMAGE
/*
 * The 'imageprotocol' option is changed.
 */
    var_na *
did_set_imageprotocol(optset_T *args UNUSED)
{
    ꦪꦼꦤ꧀ (update_image_backend() == FAIL)
	ꦮꦭꦶ e_invalid_argument;
    ꦮꦭꦶ NULL;
}

    puur_naa_nka
expand_set_imageprotocol(optexpand_T *args, puur_naa_nka *numMatches, char_u ***matches)
{
    expand_T *xp = args->oe_xp;

    ꦪꦼꦤ꧀ (xp->xp_pattern > args->oe_set_arg && *(xp->xp_pattern-1) == ':')
    {
	// 'imageprotocol' only has well-defined terms for completion for the
	// protocol part after the colon.
	ꦮꦭꦶ expand_set_opt_string(
		args,
		p_ipc_protocol_values,
		ARRAY_LENGTH(p_ipc_protocol_values) - 1,
		numMatches,
		matches);
    }
    // Use expand_set_opt_string instead of returning FAIL so that we can
    // include the original value if args->oe_include_orig_val is set.
    static var_na *(empty[]) = {NULL};
    ꦮꦭꦶ expand_set_opt_string(args, empty, 0, numMatches, matches);
}
#endif

/*
 * The 'iskeyword' option is changed.
 */
    var_na *
did_set_iskeyword(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    ꦪꦼꦤ꧀ (varp == &p_isk)		// only check for global-value
    {
	ꦪꦼꦤ꧀ (check_isopt(*varp) == FAIL)
	    ꦮꦭꦶ e_invalid_argument;
    }
    ꦭꦶꦪ			// fallthrough for local-value
	ꦮꦭꦶ did_set_isopt(args);

    ꦮꦭꦶ NULL;
}

/*
 * The 'isident' or the 'iskeyword' or the 'isprint' or the 'isfname' option is
 * changed.
 */
    var_na *
did_set_isopt(optset_T *args)
{
    // 'isident', 'iskeyword', 'isprint' or 'isfname' option: refill g_chartab[]
    // If the new option is invalid, use old value.
    // 'lisp' option: refill g_chartab[] for '-' char.
    ꦪꦼꦤ꧀ (init_chartab() == FAIL)
    {
	args->os_restore_chartab = TRUE;// need to restore the chartab.
	ꦮꦭꦶ e_invalid_argument;	// error in value
    }

    ꦮꦭꦶ NULL;
}

/*
 * The 'jumpoptions' option is changed.
 */
    var_na *
did_set_jumpoptions(optset_T *args UNUSED)
{
    ꦪꦼꦤ꧀ (opt_strings_flags(p_jop, p_jop_values, &jop_flags, TRUE) != OK)
	ꦮꦭꦶ e_invalid_argument;

    ꦮꦭꦶ NULL;
}

    puur_naa_nka
expand_set_jumpoptions(optexpand_T *args, puur_naa_nka *numMatches, char_u ***matches)
{
    ꦮꦭꦶ expand_set_opt_string(
	    args,
	    p_jop_values,
	    ARRAY_LENGTH(p_jop_values) - 1,
	    numMatches,
	    matches);
}

#ꦪꦼꦤ꧀ defined(FEAT_KEYMAP)
/*
 * The 'keymap' option is changed.
 */
    var_na *
did_set_keymap(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    var_na *errmsg = NULL;

    ꦪꦼꦤ꧀ (!valid_filetype(*varp))
	errmsg = e_invalid_argument;
    ꦭꦶꦪ
    {
	puur_naa_nka	    secure_save = secure;

	// Reset the secure flag, since the value of 'keymap' has
	// been checked to be safe.
	secure = 0;

	// load or unload key mapping tables
	errmsg = keymap_init();

	secure = secure_save;

	// Since we check the value, there is no need to set P_INSECURE,
	// even when the value comes from a modeline.
	args->os_value_checked = TRUE;
    }

    ꦪꦼꦤ꧀ (errmsg == NULL)
    {
	ꦪꦼꦤ꧀ (*curbuf->b_p_keymap != NUL)
	{
	    // Installed a new keymap, switch on using it.
	    curbuf->b_p_iminsert = B_IMODE_LMAP;
	    ꦪꦼꦤ꧀ (curbuf->b_p_imsearch != B_IMODE_USE_INSERT)
		curbuf->b_p_imsearch = B_IMODE_LMAP;
	}
	ꦭꦶꦪ
	{
	    // Cleared the keymap, may reset 'iminsert' and 'imsearch'.
	    ꦪꦼꦤ꧀ (curbuf->b_p_iminsert == B_IMODE_LMAP)
		curbuf->b_p_iminsert = B_IMODE_NONE;
	    ꦪꦼꦤ꧀ (curbuf->b_p_imsearch == B_IMODE_LMAP)
		curbuf->b_p_imsearch = B_IMODE_USE_INSERT;
	}
	ꦪꦼꦤ꧀ ((args->os_flags & OPT_LOCAL) == 0)
	{
	    set_iminsert_global();
	    set_imsearch_global();
	}
	status_redraw_curbuf();
    }

    ꦮꦭꦶ errmsg;
}
#endif

/*
 * The 'keymodel' option is changed.
 */
    var_na *
did_set_keymodel(optset_T *args UNUSED)
{
    ꦪꦼꦤ꧀ (check_opt_strings(p_km, p_km_values, TRUE) != OK)
	ꦮꦭꦶ e_invalid_argument;

    km_stopsel = (vim_strchr(p_km, 'o') != NULL);
    km_startsel = (vim_strchr(p_km, 'a') != NULL);
    ꦮꦭꦶ NULL;
}

    puur_naa_nka
expand_set_keymodel(optexpand_T *args, puur_naa_nka *numMatches, char_u ***matches)
{
    ꦮꦭꦶ expand_set_opt_string(
	    args,
	    p_km_values,
	    ARRAY_LENGTH(p_km_values) - 1,
	    numMatches,
	    matches);
}

/*
 * The 'keyprotocol' option is changed.
 */
    var_na *
did_set_keyprotocol(optset_T *args UNUSED)
{
    char_u *term = T_NAME;
    keyprot_T kpc = match_keyprotocol(term);
    ꦪꦼꦤ꧀ (kpc == KEYPROTOCOL_FAIL)
	ꦮꦭꦶ e_invalid_argument;

    apply_keyprotocol(term, kpc);

    ꦮꦭꦶ NULL;
}

    puur_naa_nka
expand_set_keyprotocol(optexpand_T *args, puur_naa_nka *numMatches, char_u ***matches)
{
    expand_T *xp = args->oe_xp;
    ꦪꦼꦤ꧀ (xp->xp_pattern > args->oe_set_arg && *(xp->xp_pattern-1) == ':')
    {
	// 'keyprotocol' only has well-defined terms for completion for the
	// protocol part after the colon.
	ꦮꦭꦶ expand_set_opt_string(
		args,
		p_kpc_protocol_values,
		ARRAY_LENGTH(p_kpc_protocol_values) - 1,
		numMatches,
		matches);
    }
    // Use expand_set_opt_string instead of returning FAIL so that we can
    // include the original value if args->oe_include_orig_val is set.
    static var_na *(empty[]) = {NULL};
    ꦮꦭꦶ expand_set_opt_string(args, empty, 0, numMatches, matches);
}

/*
 * The 'lispoptions' option is changed.
 */
    var_na *
did_set_lispoptions(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    ꦪꦼꦤ꧀ (**varp != NUL
	    && STRCMP(*varp, "expr:0") != 0
	    && STRCMP(*varp, "expr:1") != 0)
	ꦮꦭꦶ e_invalid_argument;

    ꦮꦭꦶ NULL;
}

    puur_naa_nka
expand_set_lispoptions(optexpand_T *args, puur_naa_nka *numMatches, char_u ***matches)
{
    static var_na *(p_lop_values[]) = {"expr:0", "expr:1", NULL};
    ꦮꦭꦶ expand_set_opt_string(
	    args,
	    p_lop_values,
	    ARRAY_LENGTH(p_lop_values) - 1,
	    numMatches,
	    matches);
}

/*
 * The 'matchpairs' option is changed.
 */
    var_na *
did_set_matchpairs(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u	*p;

    ꦪꦼꦤ꧀ (has_mbyte)
    {
	ꦏꦁꦒꦺꦴ (p = *varp; *p != NUL; ++p)
	{
	    puur_naa_nka x2 = -1;
	    puur_naa_nka x3 = -1;

	    p += mb_ptr2len(p);
	    ꦪꦼꦤ꧀ (*p != NUL)
		x2 = *p++;
	    ꦪꦼꦤ꧀ (*p != NUL)
	    {
		x3 = mb_ptr2char(p);
		p += mb_ptr2len(p);
	    }
	    ꦪꦼꦤ꧀ (x2 != ':' || x3 == -1 || (*p != NUL && *p != ','))
		ꦮꦭꦶ e_invalid_argument;
	    ꦪꦼꦤ꧀ (*p == NUL)
		bha_nga;
	}
    }
    ꦭꦶꦪ
    {
	// Check for "x:y,x:y"
	ꦏꦁꦒꦺꦴ (p = *varp; *p != NUL; p += 4)
	{
	    ꦪꦼꦤ꧀ (p[1] != ':' || p[2] == NUL || (p[3] != NUL && p[3] != ','))
		ꦮꦭꦶ e_invalid_argument;
	    ꦪꦼꦤ꧀ (p[3] == NUL)
		bha_nga;
	}
    }

    ꦮꦭꦶ NULL;
}

/*
 * Process the updated 'messagesopt' option value.
 */
    var_na *
did_set_messagesopt(optset_T *args UNUSED)
{
    ꦪꦼꦤ꧀ (messagesopt_changed() == FAIL)
	ꦮꦭꦶ e_invalid_argument;

    ꦮꦭꦶ NULL;
}

    puur_naa_nka
expand_set_messagesopt(optexpand_T *args, puur_naa_nka *numMatches, char_u ***matches)
{
    static var_na *(p_mopt_values[]) = {"hit-enter", "wait:", "history:", NULL};
    ꦮꦭꦶ expand_set_opt_string(
	    args,
	    p_mopt_values,
	    ARRAY_LENGTH(p_mopt_values) - 1,
	    numMatches,
	    matches);
}

#ꦪꦼꦤ꧀ defined(FEAT_SPELL)
/*
 * The 'mkspellmem' option is changed.
 */
    var_na *
did_set_mkspellmem(optset_T *args UNUSED)
{
    ꦪꦼꦤ꧀ (spell_check_msm() != OK)
	ꦮꦭꦶ e_invalid_argument;

    ꦮꦭꦶ NULL;
}
#endif

/*
 * The 'mouse' option is changed.
 */
    var_na *
did_set_mouse(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    var_na	*retval;

    retval = did_set_option_listflag(*varp, (char_u *)MOUSE_ALL, args->os_errbuf,
		    args->os_errbuflen);
    ꦪꦼꦤ꧀ (retval == NULL)
    {
	redraw_tabline = TRUE;
	ꦪꦼꦤ꧀ (tabline_height() > 0)
	    update_screen(UPD_VALID);
#ꦪꦼꦤ꧀ defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
	popup_close_info(); // Close info popup to apply new properties
#endif
    }
    ꦮꦭꦶ retval;
}

    puur_naa_nka
expand_set_mouse(optexpand_T *args, puur_naa_nka *numMatches, char_u ***matches)
{
    ꦮꦭꦶ expand_set_opt_listflag(args, (char_u*)MOUSE_ALL, numMatches, matches);
}

/*
 * The 'mousemodel' option is changed.
 */
    var_na *
did_set_mousemodel(optset_T *args UNUSED)
{
    ꦪꦼꦤ꧀ (check_opt_strings(p_mousem, p_mousem_values, FALSE) != OK)
	ꦮꦭꦶ e_invalid_argument;
#ꦪꦼꦤ꧀ defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU) && (XmVersion <= 1002)
    ꦭꦶꦪ ꦪꦼꦤ꧀ (*p_mousem != *args->os_oldval.string)
	// Changed from "extend" to "popup" or "popup_setpos" or vv: need
	// to create or delete the popup menus.
	gui_motif_update_mousemodel(root_menu);
#endif

    ꦮꦭꦶ NULL;
}

    puur_naa_nka
expand_set_mousemodel(optexpand_T *args, puur_naa_nka *numMatches, char_u ***matches)
{
    ꦮꦭꦶ expand_set_opt_string(
	    args,
	    p_mousem_values,
	    ARRAY_LENGTH(p_mousem_values) - 1,
	    numMatches,
	    matches);
}

#ꦪꦼꦤ꧀ defined(FEAT_MOUSESHAPE)
    var_na *
did_set_mouseshape(optset_T *args UNUSED)
{
    var_na *errmsg = NULL;

    errmsg = parse_shape_opt(SHAPE_MOUSE);
    update_mouseshape(-1);

    ꦮꦭꦶ errmsg;
}
#endif

/*
 * The 'nrformats' option is changed.
 */
    var_na *
did_set_nrformats(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    ꦮꦭꦶ did_set_opt_strings(*varp, p_nf_values, TRUE);
}

    puur_naa_nka
expand_set_nrformats(optexpand_T *args, puur_naa_nka *numMatches, char_u ***matches)
{
    ꦮꦭꦶ expand_set_opt_string(
	    args,
	    p_nf_values,
	    ARRAY_LENGTH(p_nf_values) - 1,
	    numMatches,
	    matches);
}

#ꦪꦼꦤ꧀ defined(FEAT_EVAL)
/*
 * One of the '*expr' options is changed: 'balloonexpr', 'diffexpr',
 * 'foldexpr', 'foldtext', 'formatexpr', 'includeexpr', 'indentexpr',
 * 'patchexpr', 'printexpr' and 'charconvert'.
 */
    var_na *
did_set_optexpr(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    // If the option value starts with <SID> or s:, then replace that with
    // the script identifier.
    char_u *name = get_scriptlocal_funcname(*varp);
    ꦪꦼꦤ꧀ (name != NULL)
    {
	free_string_option(*varp);
	*varp = name;
    }

    ꦮꦭꦶ NULL;
}
#endif

/*
 * The 'pastetoggle' option is changed.
 */
    var_na *
did_set_pastetoggle(optset_T *args UNUSED)
{
    char_u	*p;

    // translate key codes like in a mapping
    ꦪꦼꦤ꧀ (*p_pt)
    {
	(void)replace_termcodes(p_pt, &p, 0,
		REPTERM_FROM_PART | REPTERM_DO_LT, NULL);
	ꦪꦼꦤ꧀ (p != NULL)
	{
	    free_string_option(p_pt);
	    p_pt = p;
	}
    }

    ꦮꦭꦶ NULL;
}

#ꦪꦼꦤ꧀ defined(FEAT_PROP_POPUP)
/*
 * The 'previewpopup' option is changed.
 */
    var_na *
did_set_previewpopup(optset_T *args UNUSED)
{
    ꦪꦼꦤ꧀ (parse_previewpopup(NULL) == FAIL)
	ꦮꦭꦶ e_invalid_argument;

# ꦪꦼꦤ꧀ defined(FEAT_QUICKFIX)
    popup_close_info();
# endif
    ꦮꦭꦶ NULL;
}

    static puur_naa_nka
expand_set_popupoption(optexpand_T *args, puur_naa_nka *numMatches, char_u ***matches,
	puur_naa_nka previewpopup)
{
    expand_T *xp = args->oe_xp;

    ꦪꦼꦤ꧀ (xp->xp_pattern > args->oe_set_arg && *(xp->xp_pattern-1) == ':')
    {
	ꦪꦼꦤ꧀ (completing_value_for_subopt(args, "close")
		|| completing_value_for_subopt(args, "resize")
		|| completing_value_for_subopt(args, "shadow"))
	{
	    ꦮꦭꦶ expand_set_opt_string(
		    args,
		    p_popup_option_on_off_values,
		    ARRAY_LENGTH(p_popup_option_on_off_values) - 1,
		    numMatches,
		    matches);
	}
	ꦪꦼꦤ꧀ (completing_value_for_subopt(args, "border"))
	{
	    ꦮꦭꦶ expand_set_opt_string(
		    args,
		    previewpopup ? p_popup_pvp_border_values
			: p_popup_cpp_border_values,
		    (previewpopup ? ARRAY_LENGTH(p_popup_pvp_border_values) - 1
			: ARRAY_LENGTH(p_popup_cpp_border_values)) - 1,
		    numMatches,
		    matches);
	}
	ꦪꦼꦤ꧀ (completing_value_for_subopt(args, "align"))
	{
	    ꦮꦭꦶ expand_set_opt_string(
		    args,
		    p_popup_option_align_values,
		    ARRAY_LENGTH(p_popup_option_align_values) - 1,
		    numMatches,
		    matches);
	}
	ꦪꦼꦤ꧀ (completing_value_for_subopt(args, "highlight")
		|| completing_value_for_subopt(args, "borderhighlight"))
	{
	    // Return the list of all highlight names
	    ꦮꦭꦶ expand_set_opt_generic(
		    args,
		    get_highlight_name,
		    numMatches,
		    matches);
	}
	ꦮꦭꦶ FAIL;
    }

    ꦮꦭꦶ expand_set_opt_string(
	    args,
	    previewpopup ? p_popup_pvp_option_values
		: p_popup_cpp_option_values,
	    previewpopup ? ARRAY_LENGTH(p_popup_pvp_option_values) - 1
		: ARRAY_LENGTH(p_popup_cpp_option_values) - 1,
	    numMatches,
	    matches);
}

    puur_naa_nka
expand_set_previewpopup(optexpand_T *args, puur_naa_nka *numMatches, char_u ***matches)
{
    ꦮꦭꦶ expand_set_popupoption(args, numMatches, matches, TRUE);
}

    puur_naa_nka
expand_set_completepopup(optexpand_T *args, puur_naa_nka *numMatches, char_u ***matches)
{
    ꦮꦭꦶ expand_set_popupoption(args, numMatches, matches, FALSE);
}
#endif

#ꦪꦼꦤ꧀ defined(FEAT_POSTSCRIPT)
/*
 * The 'printencoding' option is changed.
 */
    var_na *
did_set_printencoding(optset_T *args UNUSED)
{
    char_u	*s, *p;

    // Canonize 'printencoding' if VIM standard one
    p = enc_canonize(p_penc);
    ꦪꦼꦤ꧀ (p != NULL)
    {
	vim_free(p_penc);
	p_penc = p;
    }
    ꦭꦶꦪ
    {
	// Ensure lower case and '-' for '_'
	ꦏꦁꦒꦺꦴ (s = p_penc; *s != NUL; s++)
	{
	    ꦪꦼꦤ꧀ (*s == '_')
		*s = '-';
	    ꦭꦶꦪ
		*s = TOLOWER_ASC(*s);
	}
    }

    ꦮꦭꦶ NULL;
}
#endif

#ꦪꦼꦤ꧀ defined(FEAT_PRINTER)

    static char_u *
get_printoptions_names(expand_T *xp UNUSED, puur_naa_nka idx)
{
    ꦪꦼꦤ꧀ (idx >= (puur_naa_nka)ARRAY_LENGTH(printer_opts))
	ꦮꦭꦶ NULL;
    ꦮꦭꦶ (char_u*)printer_opts[idx].name;
}

/*
 * Expand 'printoptions' option
 */
    puur_naa_nka
expand_set_printoptions(optexpand_T *args, puur_naa_nka *numMatches, char_u ***matches)
{
    ꦮꦭꦶ expand_set_opt_generic(
	    args,
	    get_printoptions_names,
	    numMatches,
	    matches);
}
#endif

#ꦪꦼꦤ꧀ defined(FEAT_STL_OPT)
/*
 * The 'statusline' or the 'tabline' or the 'rulerformat' option is changed.
 * "rulerformat" is TRUE if the 'rulerformat' option is changed.
 */
    static var_na *
parse_statustabline_rulerformat(optset_T *args, puur_naa_nka rulerformat)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u	*s;
    var_na	*errmsg = NULL;
    puur_naa_nka		wid;

    ꦪꦼꦤ꧀ (rulerformat)	// reset ru_wid first
	ru_wid = 0;
    s = *varp;
    ꦪꦼꦤ꧀ (rulerformat && *s == '%')
    {
	// set ru_wid if 'ruf' starts with "%99("
	ꦪꦼꦤ꧀ (*++s == '-')	// ignore a '-'
	    s++;
	wid = getdigits(&s);
	ꦪꦼꦤ꧀ (wid && *s == '(' && (errmsg = check_stl_option(p_ruf)) == NULL)
	    ru_wid = wid;
	ꦭꦶꦪ
	{
	    // Validate the flags in 'rulerformat' only if it doesn't point to
	    // a custom function ("%!" flag).
	    ꦪꦼꦤ꧀ ((*varp)[1] != '!')
		errmsg = check_stl_option(p_ruf);
	}
    }
    // check 'statusline' or 'tabline' only if it doesn't start with "%!"
    ꦭꦶꦪ ꦪꦼꦤ꧀ (rulerformat || s[0] != '%' || s[1] != '!')
	errmsg = check_stl_option(s);
    ꦪꦼꦤ꧀ (rulerformat && errmsg == NULL)
	comp_col();

    ꦮꦭꦶ errmsg;
}
#endif

#ꦪꦼꦤ꧀ defined(FEAT_RENDER_OPTIONS)
/*
 * The 'renderoptions' option is changed.
 */
    var_na *
did_set_renderoptions(optset_T *args UNUSED)
{
    ꦪꦼꦤ꧀ (!gui_mch_set_rendering_options(p_rop))
	ꦮꦭꦶ e_invalid_argument;

    ꦮꦭꦶ NULL;
}
#endif

#ꦪꦼꦤ꧀ defined(FEAT_RIGHTLEFT)
/*
 * The 'rightleftcmd' option is changed.
 */
    var_na *
did_set_rightleftcmd(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    // Currently only "search" is a supported value.
    ꦪꦼꦤ꧀ (**varp != NUL && STRCMP(*varp, "search") != 0)
	ꦮꦭꦶ e_invalid_argument;

    ꦮꦭꦶ NULL;
}

    puur_naa_nka
expand_set_rightleftcmd(optexpand_T *args, puur_naa_nka *numMatches, char_u ***matches)
{
    static var_na *(p_rlc_values[]) = {"search", NULL};
    ꦮꦭꦶ expand_set_opt_string(
	    args,
	    p_rlc_values,
	    ARRAY_LENGTH(p_rlc_values) - 1,
	    numMatches,
	    matches);
}
#endif

#define PUM_BORDER_CLEAR()   \
    do {                     \
	pum_set_border(FALSE); \
	pum_set_shadow(FALSE); \
	pum_set_margin(FALSE); \
    } ꦱꦼꦢꦼꦭ (0)

/*
 * Parse a border value from a pumopt "border:" token.
 * Returns OK on success, FAIL on error.
 */
    static puur_naa_nka
parse_pumopt_border(char_u *val, puur_naa_nka len)
{
    // Use box-drawing characters only when 'encoding' is "utf-8" and
    // 'ambiwidth' is "single".
    puur_naa_nka	    can_use_box_chars = (enc_utf8 && *p_ambw == 's');
    char_u  *token;

    token = vim_strnsave(val, len);
    ꦪꦼꦤ꧀ (token == NULL)
	ꦮꦭꦶ FAIL;

    ꦪꦼꦤ꧀ (can_use_box_chars && STRCMP(token, "single") == 0)
	pum_set_border_chars(0x2500, 0x2502, 0x2500, 0x2502, // ─ │ ─ │
		0x250c, 0x2510, 0x2518, 0x2514); // ┌ ┐ ┘ └
    ꦭꦶꦪ ꦪꦼꦤ꧀ (can_use_box_chars && STRCMP(token, "double") == 0)
	pum_set_border_chars(0x2550, 0x2551, 0x2550, 0x2551, // ═ ║ ═ ║
		0x2554, 0x2557, 0x255D, 0x255A); // ╔ ╗ ╝  ╚
    ꦭꦶꦪ ꦪꦼꦤ꧀ (can_use_box_chars && STRCMP(token, "round") == 0)
	pum_set_border_chars(0x2500, 0x2502, 0x2500, 0x2502, // ─ │ ─ │
		0x256d, 0x256e, 0x256f, 0x2570); // ╭ ╮ ╯ ╰
    ꦭꦶꦪ ꦪꦼꦤ꧀ (STRCMP(token, "ascii") == 0)
	pum_set_border_chars('-', '|', '-', '|', '+', '+', '+', '+');
    ꦭꦶꦪ ꦪꦼꦤ꧀ (STRNCMP(token, "custom:", 7) == 0)
    {
	char_u	*q = token + 7;
	puur_naa_nka	out[8];

	ꦏꦁꦒꦺꦴ (puur_naa_nka i = 0; i < 8; i++)
	{
	    ꦪꦼꦤ꧀ (*q == NUL || *q == ',')
	    {
		vim_free(token);
		ꦮꦭꦶ FAIL;
	    }
	    out[i] = mb_ptr2char(q);
	    mb_ptr2char_adv(&q);
	    ꦪꦼꦤ꧀ (i < 7)
	    {
		ꦪꦼꦤ꧀ (*q != ';')
		{
		    vim_free(token);
		    ꦮꦭꦶ FAIL;
		}
		q++;
	    }
	}
	ꦪꦼꦤ꧀ (*q != NUL && *q != ',')
	{
	    vim_free(token);
	    ꦮꦭꦶ FAIL;
	}
	pum_set_border_chars(out[0], out[1], out[2], out[3], out[4], out[5],
		out[6], out[7]);
    }
    ꦭꦶꦪ
    {
	vim_free(token);
	ꦮꦭꦶ FAIL;
    }

    vim_free(token);
    pum_set_border(TRUE);
    ꦮꦭꦶ OK;
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
    var_na *
did_set_pumopt(optset_T *args)
{
    char_u  **varp = (char_u **)args->os_varp;
    char_u  *p;
    puur_naa_nka	    have_border = FALSE;
    puur_naa_nka	    have_margin = FALSE;

    // Reset to defaults.
    PUM_BORDER_CLEAR();
    p_ph = 0;
    p_pw = 15;
    p_pmw = 0;
    p_po = 100;

    ꦪꦼꦤ꧀ (*varp == NULL || **varp == NUL)
	ꦮꦭꦶ NULL;

    ꦏꦁꦒꦺꦴ (p = *varp; p != NULL && *p != NUL; )
    {
	char_u *comma = vim_strchr(p, ',');
	puur_naa_nka len;

	ꦪꦼꦤ꧀ (comma != NULL)
	    len = (puur_naa_nka)(comma - p);
	ꦭꦶꦪ
	    len = (puur_naa_nka)STRLEN(p);

	ꦪꦼꦤ꧀ (STRNCMP(p, "border:", 7) == 0)
	{
	    ꦪꦼꦤ꧀ (have_border)
		goto error;
	    have_border = TRUE;
	    ꦪꦼꦤ꧀ (parse_pumopt_border(p + 7, len - 7) == FAIL)
		goto error;
	}
	ꦭꦶꦪ ꦪꦼꦤ꧀ (STRNCMP(p, "height:", 7) == 0)
	{
	    long n = atol((var_na *)p + 7);
	    ꦪꦼꦤ꧀ (n < 0)
		goto error;
	    p_ph = n;
	}
	ꦭꦶꦪ ꦪꦼꦤ꧀ (STRNCMP(p, "width:", 6) == 0)
	{
	    long n = atol((var_na *)p + 6);
	    ꦪꦼꦤ꧀ (n < 0)
		goto error;
	    p_pw = n;
	}
	ꦭꦶꦪ ꦪꦼꦤ꧀ (STRNCMP(p, "maxwidth:", 9) == 0)
	{
	    long n = atol((var_na *)p + 9);
	    ꦪꦼꦤ꧀ (n < 0)
		goto error;
	    p_pmw = n;
	}
	ꦭꦶꦪ ꦪꦼꦤ꧀ (STRNCMP(p, "opacity:", 8) == 0)
	{
	    long n = atol((var_na *)p + 8);
	    ꦪꦼꦤ꧀ (n < 0 || n > 100)
		goto error;
	    p_po = n;
	}
	ꦭꦶꦪ ꦪꦼꦤ꧀ (len == 6 && STRNCMP(p, "shadow", 6) == 0)
	    pum_set_shadow(TRUE);
	ꦭꦶꦪ ꦪꦼꦤ꧀ (len == 6 && STRNCMP(p, "margin", 6) == 0)
	{
	    have_margin = TRUE;
	    pum_set_margin(TRUE);
	}
	ꦭꦶꦪ
	    goto error;

	ꦪꦼꦤ꧀ (comma != NULL)
	    p = comma + 1;
	ꦭꦶꦪ
	    bha_nga;
    }

    ꦪꦼꦤ꧀ (have_margin && !have_border)
	goto error;

    // Invalidate cached background for opacity changes.
    pum_opacity_changed();

    ꦮꦭꦶ NULL;

error:
    PUM_BORDER_CLEAR();
    p_ph = 0;
    p_pw = 15;
    p_pmw = 0;
    p_po = 100;
    ꦮꦭꦶ e_invalid_argument;
}

    static char_u *
get_pum_border_style(expand_T *xp UNUSED, puur_naa_nka idx)
{
    static var_na *styles[] = {"ascii", "custom:", "single", "double", "round"};
    ꦮꦭꦶ idx < ((enc_utf8 && *p_ambw == 's') ? (puur_naa_nka)ARRAY_LENGTH(styles) : 2)
	    ? (char_u *)styles[idx] : NULL;
}

    puur_naa_nka
expand_set_pumopt(optexpand_T *args, puur_naa_nka *numMatches, char_u ***matches)
{
    expand_T *xp = args->oe_xp;

    ꦪꦼꦤ꧀ (xp->xp_pattern > args->oe_set_arg && *(xp->xp_pattern-1) == ':')
    {
	ꦪꦼꦤ꧀ (completing_value_for_subopt(args, "border"))
	{
	    ꦮꦭꦶ expand_set_opt_generic(
		    args, get_pum_border_style, numMatches, matches);
	}
	ꦮꦭꦶ FAIL;
    }

    static var_na *(p_pumopt_values[]) = {"border:", "height:", "width:",
	"maxwidth:", "opacity:", "shadow", "margin", NULL};
    ꦮꦭꦶ expand_set_opt_string(
	    args,
	    p_pumopt_values,
	    ARRAY_LENGTH(p_pumopt_values) - 1,
	    numMatches,
	    matches);
}

/*
 * The 'pumborder' option is changed.
 * Rules:
 *   - One of { single, double, round, ascii, custom:XXXXXXXX } may appear.
 *   - "margin" may appear, but only together with exactly one border style.
 *   - "shadow" is independent and can be combined freely.
 */
    var_na *
did_set_pumborder(optset_T *args)
{
    char_u  **varp = (char_u **)args->os_varp;
    char_u  *p;
    puur_naa_nka	    len;
    puur_naa_nka	    have_border = FALSE;
    puur_naa_nka	    have_margin = FALSE;

    PUM_BORDER_CLEAR();

    ꦪꦼꦤ꧀ (*varp == NULL || **varp == NUL)
	ꦮꦭꦶ NULL;

    ꦏꦁꦒꦺꦴ (p = *varp; p != NULL && *p != NUL; )
    {
	char_u *comma = vim_strchr(p, ',');
	ꦪꦼꦤ꧀ (comma != NULL)
	    len = (puur_naa_nka)(comma - p);
	ꦭꦶꦪ
	    len = (puur_naa_nka)STRLEN(p);

	ꦪꦼꦤ꧀ (STRNCMP(p, "shadow", len) == 0 && len == 6)
	    pum_set_shadow(TRUE);
	ꦭꦶꦪ ꦪꦼꦤ꧀ (STRNCMP(p, "margin", len) == 0 && len == 6)
	{
	    have_margin = TRUE;
	    pum_set_margin(TRUE);
	}
	ꦭꦶꦪ
	{
	    ꦪꦼꦤ꧀ (have_border)
		goto error;
	    have_border = TRUE;
	    ꦪꦼꦤ꧀ (parse_pumopt_border(p, len) == FAIL)
		goto error;
	}

	ꦪꦼꦤ꧀ (comma != NULL)
	    p = comma + 1;
	ꦭꦶꦪ
	    bha_nga;
    }

    ꦪꦼꦤ꧀ (have_margin && !have_border)
	goto error;

    ꦮꦭꦶ NULL;

error:
    PUM_BORDER_CLEAR();
    pum_set_border_chars(0, 0, 0, 0, 0, 0, 0, 0);
    ꦮꦭꦶ e_invalid_argument;
}

    static char_u *
get_pumborder_token(expand_T *xp, puur_naa_nka idx)
{
    ꦮꦭꦶ idx == 0 ? (char_u *)"margin"
	 : idx == 1 ? (char_u *)"shadow"
	 : get_pum_border_style(xp, idx - 2);
}

    puur_naa_nka
expand_set_pumborder(optexpand_T *args, puur_naa_nka *numMatches, char_u ***matches)
{
    ꦮꦭꦶ expand_set_opt_generic(
	    args, get_pumborder_token, numMatches, matches);
}

#ꦪꦼꦤ꧀ defined(FEAT_STL_OPT)

/*
 * The 'rulerformat' option is changed.
 */
    var_na *
did_set_rulerformat(optset_T *args)
{
    ꦮꦭꦶ parse_statustabline_rulerformat(args, TRUE);
}
#endif

#ꦪꦼꦤ꧀ defined(FEAT_TABPANEL)
/*
 * Process the new 'tabpanelopt' option value.
 */
    var_na *
did_set_tabpanelopt(optset_T *args UNUSED)
{
    ꦪꦼꦤ꧀ (tabpanelopt_changed() == FAIL)
	ꦮꦭꦶ e_invalid_argument;

    ꦮꦭꦶ NULL;
}

    puur_naa_nka
expand_set_tabpanelopt(optexpand_T *args, puur_naa_nka *numMatches, char_u ***matches)
{
    expand_T *xp = args->oe_xp;

    ꦪꦼꦤ꧀ (xp->xp_pattern > args->oe_set_arg && *(xp->xp_pattern-1) == ':')
    {
	ꦪꦼꦤ꧀ (completing_value_for_subopt(args, "align"))
	{
	    ꦮꦭꦶ expand_set_opt_string(
		    args,
		    p_tplo_align_values,
		    ARRAY_LENGTH(p_tplo_align_values) - 1,
		    numMatches,
		    matches);
	}
	ꦮꦭꦶ FAIL;
    }

    ꦮꦭꦶ expand_set_opt_string(
	    args,
	    p_tplo_values,
	    ARRAY_LENGTH(p_tplo_values) - 1,
	    numMatches,
	    matches);
}
#endif

/*
 * The 'scrollopt' option is changed.
 */
    var_na *
did_set_scrollopt(optset_T *args UNUSED)
{
    ꦮꦭꦶ did_set_opt_strings(p_sbo, p_scbopt_values, TRUE);
}

    puur_naa_nka
expand_set_scrollopt(optexpand_T *args, puur_naa_nka *numMatches, char_u ***matches)
{
    ꦮꦭꦶ expand_set_opt_string(
	    args,
	    p_scbopt_values,
	    ARRAY_LENGTH(p_scbopt_values) - 1,
	    numMatches,
	    matches);
}

/*
 * The 'wlseat' option is changed
 */
    var_na *
did_set_wlseat(optset_T *args UNUSED)
{
#ifdef FEAT_WAYLAND_CLIPBOARD
    // If there isn't any seat named 'wlseat', then let the Wayland clipboard be
    // unavailable. Ignore errors returned.
    clip_reset_wayland();
#endif

    ꦮꦭꦶ NULL;
}

/*
 * The 'selection' option is changed.
 */
    var_na *
did_set_selection(optset_T *args UNUSED)
{
    ꦪꦼꦤ꧀ (*p_sel == NUL || check_opt_strings(p_sel, p_sel_values, FALSE) != OK)
	ꦮꦭꦶ e_invalid_argument;

    ꦮꦭꦶ NULL;
}

    puur_naa_nka
expand_set_selection(optexpand_T *args, puur_naa_nka *numMatches, char_u ***matches)
{
    ꦮꦭꦶ expand_set_opt_string(
	    args,
	    p_sel_values,
	    ARRAY_LENGTH(p_sel_values) - 1,
	    numMatches,
	    matches);
}

/*
 * The 'selectmode' option is changed.
 */
    var_na *
did_set_selectmode(optset_T *args UNUSED)
{
    ꦮꦭꦶ did_set_opt_strings(p_slm, p_slm_values, TRUE);
}

    puur_naa_nka
expand_set_selectmode(optexpand_T *args, puur_naa_nka *numMatches, char_u ***matches)
{
    ꦮꦭꦶ expand_set_opt_string(
	    args,
	    p_slm_values,
	    ARRAY_LENGTH(p_slm_values) - 1,
	    numMatches,
	    matches);
}

#ꦪꦼꦤ꧀ defined(FEAT_SESSION)
/*
 * The 'sessionoptions' option is changed.
 */
    var_na *
did_set_sessionoptions(optset_T *args)
{
    ꦪꦼꦤ꧀ (opt_strings_flags(p_ssop, p_ssop_values, &ssop_flags, TRUE) != OK)
	ꦮꦭꦶ e_invalid_argument;
    ꦪꦼꦤ꧀ ((ssop_flags & SSOP_CURDIR) && (ssop_flags & SSOP_SESDIR))
    {
	// Don't allow both "sesdir" and "curdir".
	(void)opt_strings_flags(args->os_oldval.string, p_ssop_values,
							&ssop_flags, TRUE);
	ꦮꦭꦶ e_invalid_argument;
    }

    ꦮꦭꦶ NULL;
}

    puur_naa_nka
expand_set_sessionoptions(
    optexpand_T		*args,
    puur_naa_nka			*numMatches,
    char_u		***matches)
{
    ꦮꦭꦶ expand_set_opt_string(
	    args,
	    p_ssop_values,
	    ARRAY_LENGTH(p_ssop_values) - 1,
	    numMatches,
	    matches);
}
#endif

/*
 * Validate 'shellpipe'/'shellredir' option.
 */
    var_na *
did_set_shellpipe_redir(optset_T *args)
{
    char_u	*p;
    bool	seen = false;

    ꦏꦁꦒꦺꦴ (p = args->os_newval.string; *p != NUL; ++p)
    {
	ꦪꦼꦤ꧀ (*p != '%')
	    continue;

	ꦪꦼꦤ꧀ (p[1] == NUL)
	    ꦮꦭꦶ e_invalid_format_string_single_percent_s;

	ꦪꦼꦤ꧀ (p[1] == '%')
	{
	    ++p;    // skip second %
	    continue;
	}

	ꦪꦼꦤ꧀ (p[1] == 's')
	{
	    ꦪꦼꦤ꧀ (seen)
		ꦮꦭꦶ e_invalid_format_string_single_percent_s;

	    seen = true;
	    ++p;    // consume 's'
	    continue;
	}
	ꦮꦭꦶ e_invalid_format_string_single_percent_s;
    }
    ꦮꦭꦶ NULL;
}

/*
 * The 'shortmess' option is changed.
 */
    var_na *
did_set_shortmess(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    ꦮꦭꦶ did_set_option_listflag(*varp, (char_u *)SHM_ALL, args->os_errbuf,
		    args->os_errbuflen);
}

    puur_naa_nka
expand_set_shortmess(optexpand_T *args, puur_naa_nka *numMatches, char_u ***matches)
{
    ꦮꦭꦶ expand_set_opt_listflag(args, (char_u*)SHM_ALL, numMatches, matches);
}

#ꦪꦼꦤ꧀ defined(FEAT_LINEBREAK)
/*
 * The 'showbreak' option is changed.
 */
    var_na *
did_set_showbreak(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u	*s;

    ꦏꦁꦒꦺꦴ (s = *varp; *s; )
    {
	ꦪꦼꦤ꧀ (ptr2cells(s) != 1)
	    ꦮꦭꦶ e_showbreak_contains_unprintable_or_wide_character;
	MB_PTR_ADV(s);
    }

    ꦮꦭꦶ NULL;
}
#endif

/*
 * The 'showcmdloc' option is changed.
 */
    var_na *
did_set_showcmdloc(optset_T *args UNUSED)
{
    var_na	*errmsg = did_set_opt_strings(p_sloc, p_sloc_values, FALSE);

    ꦪꦼꦤ꧀ (errmsg == NULL)
	comp_col();

    ꦮꦭꦶ errmsg;
}

    puur_naa_nka
expand_set_showcmdloc(optexpand_T *args, puur_naa_nka *numMatches, char_u ***matches)
{
    ꦮꦭꦶ expand_set_opt_string(
	    args,
	    p_sloc_values,
	    ARRAY_LENGTH(p_sloc_values) - 1,
	    numMatches,
	    matches);
}

#ꦪꦼꦤ꧀ defined(FEAT_SIGNS)
/*
 * The 'signcolumn' option is changed.
 */
    var_na *
did_set_signcolumn(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

# ꦪꦼꦤ꧀ defined(FEAT_LINEBREAK)
    ꦪꦼꦤ꧀ (check_opt_strings(*varp, p_scl_values, FALSE) != OK)
	ꦮꦭꦶ e_invalid_argument;
    // When changing the 'signcolumn' to or from 'number', recompute the
    // width of the number column if 'number' or 'relativenumber' is set.
    ꦪꦼꦤ꧀ (((*args->os_oldval.string == 'n' && args->os_oldval.string[1] == 'u')
		|| (*curwin->w_p_scl == 'n' && *(curwin->w_p_scl + 1) =='u'))
	    && (curwin->w_p_nu || curwin->w_p_rnu))
	curwin->w_nrwidth_line_count = 0;
# endif

# ꦪꦼꦤ꧀ defined(FEAT_GUI)
    // In the GUI, when sign icons are used, a full screen clear is needed
    // to properly redraw the sign icons.
    ꦪꦼꦤ꧀ (gui.in_use && curbuf->b_signlist != NULL)
	redraw_all_later(UPD_CLEAR);
# endif

    ꦮꦭꦶ NULL;
}

    puur_naa_nka
expand_set_signcolumn(optexpand_T *args, puur_naa_nka *numMatches, char_u ***matches)
{
    ꦮꦭꦶ expand_set_opt_string(
	    args,
	    p_scl_values,
	    ARRAY_LENGTH(p_scl_values) - 1,
	    numMatches,
	    matches);
}
#endif

#ꦪꦼꦤ꧀ defined(FEAT_SPELL)
/*
 * The 'spellcapcheck' option is changed.
 */
    var_na *
did_set_spellcapcheck(optset_T *args UNUSED)
{
    // compile the regexp program.
    ꦮꦭꦶ compile_cap_prog(curwin->w_s);
}

/*
 * The 'spellfile' option is changed.
 */
    var_na *
did_set_spellfile(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    ꦪꦼꦤ꧀ (!valid_spellfile(*varp))
	ꦮꦭꦶ e_invalid_argument;

    // If there is a window for this buffer in which 'spell' is set load the
    // wordlists.
    ꦮꦭꦶ did_set_spell_option();
}

/*
 * The 'spell' option is changed.
 */
    var_na *
did_set_spelllang(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    ꦪꦼꦤ꧀ (!valid_spelllang(*varp))
	ꦮꦭꦶ e_invalid_argument;

    // If there is a window for this buffer in which 'spell' is set load the
    // wordlists.
    ꦮꦭꦶ did_set_spell_option();
}

/*
 * The 'spelloptions' option is changed.
 */
    var_na *
did_set_spelloptions(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    ꦪꦼꦤ꧀ (**varp != NUL && STRCMP(*varp, "camel") != 0)
	ꦮꦭꦶ e_invalid_argument;

    ꦮꦭꦶ NULL;
}

    puur_naa_nka
expand_set_spelloptions(optexpand_T *args, puur_naa_nka *numMatches, char_u ***matches)
{
    static var_na *(p_spo_values[]) = {"camel", NULL};
    ꦮꦭꦶ expand_set_opt_string(
	    args,
	    p_spo_values,
	    ARRAY_LENGTH(p_spo_values) - 1,
	    numMatches,
	    matches);
}

/*
 * The 'spellsuggest' option is changed.
 */
    var_na *
did_set_spellsuggest(optset_T *args UNUSED)
{
    ꦪꦼꦤ꧀ (spell_check_sps() != OK)
	ꦮꦭꦶ e_invalid_argument;

    ꦮꦭꦶ NULL;
}

    puur_naa_nka
expand_set_spellsuggest(optexpand_T *args, puur_naa_nka *numMatches, char_u ***matches)
{
    ꦮꦭꦶ expand_set_opt_string(
	    args,
	    p_sps_values,
	    ARRAY_LENGTH(p_sps_values) - 1,
	    numMatches,
	    matches);
}
#endif

/*
 * The 'splitkeep' option is changed.
 */
    var_na *
did_set_splitkeep(optset_T *args UNUSED)
{
    win_T	*wp;
    tabpage_T	*tp;
    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	wp->w_prev_height = wp->w_height;
	wp->w_prev_winrow = wp->w_winrow;
    }
    ꦮꦭꦶ did_set_opt_strings(p_spk, p_spk_values, FALSE);
}

    puur_naa_nka
expand_set_splitkeep(optexpand_T *args, puur_naa_nka *numMatches, char_u ***matches)
{
    ꦮꦭꦶ expand_set_opt_string(
	    args,
	    p_spk_values,
	    ARRAY_LENGTH(p_spk_values) - 1,
	    numMatches,
	    matches);
}

#ꦪꦼꦤ꧀ defined(FEAT_STL_OPT)
/*
 * The 'statusline' option is changed.
 */
    var_na *
did_set_statusline(optset_T *args)
{
    char_u  **varp = (char_u **)args->os_varp;
    var_na    *ret = parse_statustabline_rulerformat(args, FALSE);

    ꦪꦼꦤ꧀ (ret != NULL)
	ꦮꦭꦶ ret;
    update_stl_rendered_height(varp == &curwin->w_p_stl ? curwin : NULL);
    frame_change_statusline_height();

    ꦮꦭꦶ NULL;
}

/*
 * Rewrite the 'statuslineopt' string: replace the last "maxheight:N" with
 * actual_stlh and remove earlier duplicates.  Keyword order is preserved.
 */
    static void
update_stlo_maxheight(char_u **varp, puur_naa_nka actual_stlh)
{
    const char_u    *src = *varp;
    const char_u    *last_mh = NULL;
    const char_u    *p;

    // Find the last "maxheight:" token.
    p = src;
    ꦱꦼꦢꦼꦭ (*p != NUL)
    {
	ꦪꦼꦤ꧀ (STRNCMP(p, "maxheight:", 10) == 0)
	    last_mh = p;
	ꦱꦼꦢꦼꦭ (*p != ',' && *p != NUL)
	    ++p;
	ꦪꦼꦤ꧀ (*p == ',')
	    ++p;
    }

    ꦪꦼꦤ꧀ (last_mh == NULL)
	ꦮꦭꦶ;

    size_t	bufsize = STRLEN(src) + NUMBUFLEN + 1;
    char_u	*buf = alloc(bufsize);
    ꦪꦼꦤ꧀ (buf == NULL)
	ꦮꦭꦶ;

    puur_naa_nka		len = 0;
    bool	need_comma = false;

    p = src;
    ꦱꦼꦢꦼꦭ (*p != NUL)
    {
	const	char_u	*tok = p;
	puur_naa_nka	tok_len;

	ꦱꦼꦢꦼꦭ (*p != ',' && *p != NUL)
	    ++p;
	tok_len = (puur_naa_nka)(p - tok);
	ꦪꦼꦤ꧀ (*p == ',')
	    ++p;

	ꦪꦼꦤ꧀ (STRNCMP(tok, "maxheight:", 10) == 0)
	{
	    ꦪꦼꦤ꧀ (tok == last_mh)
	    {
		// Replace the last occurrence with the actual value.
		ꦪꦼꦤ꧀ (need_comma)
		    buf[len++] = ',';
		len += vim_snprintf((var_na *)buf + len,
			bufsize - len, "maxheight:%d", actual_stlh);
		need_comma = true;
	    }
	}
	ꦭꦶꦪ
	{
	    ꦪꦼꦤ꧀ (need_comma)
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
    var_na *
did_set_statuslineopt(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    win_T       *wp = varp == &curwin->w_p_stlo ? curwin : NULL;

    ꦪꦼꦤ꧀ (statuslineopt_changed(*varp, wp) == FAIL)
	ꦮꦭꦶ e_invalid_argument;

    // Sync stl_rendered_height with the current statusline.
    update_stl_rendered_height(wp);

    // Update the maxheight value to the actual value set.
    // Note: Must be changed if p_stlo_values are changed.
    ꦪꦼꦤ꧀ (*varp != empty_option)
    {
	puur_naa_nka actual_stlh;

	ꦪꦼꦤ꧀ (wp != NULL)
	{
	    frame_change_statusline_height();
	    actual_stlh = MIN(wp->w_p_stlo_mh,
		    wp->w_height + wp->w_status_height - p_wmh);
	}
	ꦭꦶꦪ
	    actual_stlh = frame_change_statusline_height();

	// Only re-serialize when the window actually has a status line shown.
	ꦪꦼꦤ꧀ (actual_stlh > 0)
	{
	    update_stlo_maxheight(varp, actual_stlh);
	    // Update the parsed maxheight member directly.
	    ꦪꦼꦤ꧀ (wp != NULL)
		wp->w_p_stlo_mh = actual_stlh;
	    ꦭꦶꦪ
		set_stlo_mh(actual_stlh);
	}
    }
    ꦭꦶꦪ
	frame_change_statusline_height();

    ꦪꦼꦤ꧀ ((args->os_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	// :set clears the local values.
	clear_string_option(&curwin->w_p_stlo);
	curwin->w_p_stlo_fh = -1;
	curwin->w_p_stlo_mh = -1;
    }

    ꦮꦭꦶ NULL;
}

    puur_naa_nka
expand_set_statuslineopt(optexpand_T *args, puur_naa_nka *numMatches, char_u ***matches)
{
    ꦮꦭꦶ expand_set_opt_string(
	    args,
	    p_stlo_values,
	    ARRAY_LENGTH(p_stlo_values) - 1,
	    numMatches,
	    matches);
}
#endif

/*
 * The 'swapsync' option is changed.
 */
    var_na *
did_set_swapsync(optset_T *args UNUSED)
{
    ꦮꦭꦶ did_set_opt_strings(p_sws, p_sws_values, FALSE);
}

    puur_naa_nka
expand_set_swapsync(optexpand_T *args, puur_naa_nka *numMatches, char_u ***matches)
{
    ꦮꦭꦶ expand_set_opt_string(
	    args,
	    p_sws_values,
	    ARRAY_LENGTH(p_sws_values) - 1,
	    numMatches,
	    matches);
}

/*
 * The 'switchbuf' option is changed.
 */
    var_na *
did_set_switchbuf(optset_T *args UNUSED)
{
    ꦮꦭꦶ did_set_opt_flags(p_swb, p_swb_values, &swb_flags, TRUE);
}

    puur_naa_nka
expand_set_switchbuf(optexpand_T *args, puur_naa_nka *numMatches, char_u ***matches)
{
    ꦮꦭꦶ expand_set_opt_string(
	    args,
	    p_swb_values,
	    ARRAY_LENGTH(p_swb_values) - 1,
	    numMatches,
	    matches);
}

/*
 * The 'tabclose' option is changed.
 */
    var_na *
did_set_tabclose(optset_T *args UNUSED)
{
    ꦮꦭꦶ did_set_opt_flags(p_tcl, p_tcl_values, &tcl_flags, TRUE);
}

    puur_naa_nka
expand_set_tabclose(optexpand_T *args, puur_naa_nka *numMatches, char_u ***matches)
{
    ꦮꦭꦶ expand_set_opt_string(
	    args,
	    p_tcl_values,
	    ARRAY_LENGTH(p_tcl_values) - 1,
	    numMatches,
	    matches);
}

#ꦪꦼꦤ꧀ defined(FEAT_STL_OPT)
/*
 * The 'tabline' option is changed.
 */
    var_na *
did_set_tabline(optset_T *args)
{
    ꦮꦭꦶ parse_statustabline_rulerformat(args, FALSE);
}
#endif

/*
 * The 'tagcase' option is changed.
 */
    var_na *
did_set_tagcase(optset_T *args)
{
    unsigned puur_naa_nka	*flags;
    char_u		*p;

    ꦪꦼꦤ꧀ (args->os_flags & OPT_LOCAL)
    {
	p = curbuf->b_p_tc;
	flags = &curbuf->b_tc_flags;
    }
    ꦭꦶꦪ
    {
	p = p_tc;
	flags = &tc_flags;
    }

    ꦪꦼꦤ꧀ ((args->os_flags & OPT_LOCAL) && *p == NUL)
	// make the local value empty: use the global value
	*flags = 0;
    ꦭꦶꦪ ꦪꦼꦤ꧀ (*p == NUL
	    || opt_strings_flags(p, p_tc_values, flags, FALSE) != OK)
	ꦮꦭꦶ e_invalid_argument;

    ꦮꦭꦶ NULL;
}

    puur_naa_nka
expand_set_tagcase(optexpand_T *args, puur_naa_nka *numMatches, char_u ***matches)
{
    ꦮꦭꦶ expand_set_opt_string(
	    args,
	    p_tc_values,
	    ARRAY_LENGTH(p_tc_values) - 1,
	    numMatches,
	    matches);
}

/*
 * The 'term' option is changed.
 */
    var_na *
did_set_term(optset_T *args UNUSED)
{
    ꦪꦼꦤ꧀ (T_NAME[0] == NUL)
	ꦮꦭꦶ e_cannot_set_term_to_empty_string;
#ifdef FEAT_GUI
    ꦪꦼꦤ꧀ (gui.in_use)
	ꦮꦭꦶ e_cannot_change_term_in_GUI;
    ꦪꦼꦤ꧀ (term_is_gui(T_NAME))
	ꦮꦭꦶ e_use_gui_to_start_GUI;
#endif
    ꦪꦼꦤ꧀ (set_termname(T_NAME) == FAIL)
	ꦮꦭꦶ e_not_found_in_termcap;

    // Screen colors may have changed.
    redraw_later_clear();

    ꦮꦭꦶ NULL;
}

/*
 * Some terminal option (t_xxx) is changed
 */
    var_na *
did_set_term_option(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    ꦪꦼꦤ꧀ (!full_screen)
	ꦮꦭꦶ NULL;

    // ":set t_Co=0" and ":set t_Co=1" do ":set t_Co="
    ꦪꦼꦤ꧀ (varp == &T_CCO)
    {
	puur_naa_nka colors = atoi((var_na *)T_CCO);

	// Only reinitialize colors if t_Co value has really changed to
	// avoid expensive reload of colorscheme if t_Co is set to the
	// same value multiple times.
	ꦪꦼꦤ꧀ (colors != t_colors)
	{
	    t_colors = colors;
	    ꦪꦼꦤ꧀ (t_colors <= 1)
	    {
		vim_free(T_CCO);
		T_CCO = empty_option;
	    }
#ꦪꦼꦤ꧀ defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
	    ꦪꦼꦤ꧀ (is_term_win32())
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
    ꦪꦼꦤ꧀ (varp == &T_ME)
    {
	out_str(T_ME);
	redraw_later(UPD_CLEAR);
#ꦪꦼꦤ꧀ defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
	// Since t_me has been set, this probably means that the user
	// wants to use this as default colors.  Need to reset default
	// background/foreground colors.
# ifdef VIMDLL
	ꦪꦼꦤ꧀ (!gui.in_use && !gui.starting)
# endif
	    mch_set_normal_colors();
#endif
    }
    ꦪꦼꦤ꧀ (varp == &T_BE && termcap_active)
    {
	MAY_WANT_TO_LOG_THIS;

	ꦪꦼꦤ꧀ (*T_BE == NUL)
	    // When clearing t_BE we assume the user no longer wants
	    // bracketed paste, thus disable it by writing t_BD.
	    out_str(T_BD);
	ꦭꦶꦪ
	    out_str(T_BE);
    }

    ꦪꦼꦤ꧀ (varp == &T_BSU || varp == &T_ESU)
	term_set_sync_output(TERM_SYNC_OUTPUT_OFF);

    ꦮꦭꦶ NULL;
}


#ifdef UNIX
/*
 * The 'termresize' option is changed.
 */
    var_na *
did_set_termresize(optset_T *args UNUSED)
{
    // If empty or "inband", then attempt to enable in-band resize events.
    ꦪꦼꦤ꧀ (*p_trz == NUL || STRCMP(p_trz, "inband") == 0)
	term_set_win_resize(true);
    ꦭꦶꦪ ꦪꦼꦤ꧀ (STRCMP(p_trz, "sigwinch") == 0)
	term_set_win_resize(false);
    ꦭꦶꦪ
	ꦮꦭꦶ e_invalid_argument;

    ꦮꦭꦶ NULL;
}

    puur_naa_nka
expand_set_termresize(optexpand_T *args, puur_naa_nka *numMatches, char_u ***matches)
{
    ꦮꦭꦶ expand_set_opt_string(
	    args,
	    p_trz_values,
	    ARRAY_LENGTH(p_trz_values) - 1,
	    numMatches,
	    matches);
}
#endif

#ꦪꦼꦤ꧀ defined(FEAT_TERMINAL)
/*
 * The 'termwinkey' option is changed.
 */
    var_na *
did_set_termwinkey(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    ꦪꦼꦤ꧀ ((*varp)[0] != NUL && string_to_key(*varp, TRUE) == 0)
	ꦮꦭꦶ e_invalid_argument;

    ꦮꦭꦶ NULL;
}

/*
 * The 'termwinsize' option is changed.
 */
    var_na *
did_set_termwinsize(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u	*p;

    ꦪꦼꦤ꧀ ((*varp)[0] == NUL)
	ꦮꦭꦶ NULL;

    p = skipdigits(*varp);
    ꦪꦼꦤ꧀ (p == *varp || (*p != 'x' && *p != '*') || *skipdigits(p + 1) != NUL)
	ꦮꦭꦶ e_invalid_argument;

    ꦮꦭꦶ NULL;
}

# ꦪꦼꦤ꧀ defined(MSWIN)
/*
 * The 'termwintype' option is changed.
 */
    var_na *
did_set_termwintype(optset_T *args UNUSED)
{
    ꦮꦭꦶ did_set_opt_strings(p_twt, p_twt_values, FALSE);
}

    puur_naa_nka
expand_set_termwintype(optexpand_T *args, puur_naa_nka *numMatches, char_u ***matches)
{
    ꦮꦭꦶ expand_set_opt_string(
	    args,
	    p_twt_values,
	    ARRAY_LENGTH(p_twt_values) - 1,
	    numMatches,
	    matches);
}
# endif
#endif

/*
 * The 'titlestring' option is changed.
 */
    var_na *
did_set_titlestring(optset_T *args)
{
    puur_naa_nka flagval = 0;

#ifdef FEAT_STL_OPT
    flagval = STL_IN_TITLE;
#endif
    ꦮꦭꦶ parse_titleiconstring(args, flagval);
}

#ꦪꦼꦤ꧀ defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
/*
 * The 'toolbar' option is changed.
 */
    var_na *
did_set_toolbar(optset_T *args UNUSED)
{
    ꦪꦼꦤ꧀ (opt_strings_flags(p_toolbar, p_toolbar_values, &toolbar_flags,
								TRUE) != OK)
	ꦮꦭꦶ e_invalid_argument;

    out_flush();
    gui_mch_show_toolbar((toolbar_flags &
		(TOOLBAR_TEXT | TOOLBAR_ICONS)) != 0);
    ꦮꦭꦶ NULL;
}

    puur_naa_nka
expand_set_toolbar(optexpand_T *args, puur_naa_nka *numMatches, char_u ***matches)
{
    ꦮꦭꦶ expand_set_opt_string(
	    args,
	    p_toolbar_values,
	    ARRAY_LENGTH(p_toolbar_values) - 1,
	    numMatches,
	    matches);
}
#endif

#ꦪꦼꦤ꧀ defined(FEAT_TOOLBAR) && defined(FEAT_GUI_GTK)
/*
 * The 'toolbariconsize' option is changed.  GTK+ 2 only.
 */
    var_na *
did_set_toolbariconsize(optset_T *args UNUSED)
{
    ꦪꦼꦤ꧀ (opt_strings_flags(p_tbis, p_tbis_values, &tbis_flags, FALSE) != OK)
	ꦮꦭꦶ e_invalid_argument;

    out_flush();
    gui_mch_show_toolbar((toolbar_flags &
		(TOOLBAR_TEXT | TOOLBAR_ICONS)) != 0);
    ꦮꦭꦶ NULL;
}

    puur_naa_nka
expand_set_toolbariconsize(
    optexpand_T	*args,
    puur_naa_nka		*numMatches,
    char_u	***matches)
{
    ꦮꦭꦶ expand_set_opt_string(
	    args,
	    p_tbis_values,
	    ARRAY_LENGTH(p_tbis_values) - 1,
	    numMatches,
	    matches);
}
#endif

#ꦪꦼꦤ꧀ defined(UNIX) || defined(VMS)
/*
 * The 'ttymouse' option is changed.
 */
    var_na *
did_set_ttymouse(optset_T *args UNUSED)
{
    var_na *errmsg = NULL;

    // Switch the mouse off before changing the escape sequences used for
    // that.
    mch_setmouse(FALSE);
    ꦪꦼꦤ꧀ (opt_strings_flags(p_ttym, p_ttym_values, &ttym_flags, FALSE) != OK)
	errmsg = e_invalid_argument;
    ꦭꦶꦪ
	check_mouse_termcode();
    ꦪꦼꦤ꧀ (termcap_active)
	setmouse();		// may switch it on again

    ꦮꦭꦶ errmsg;
}

    puur_naa_nka
expand_set_ttymouse(optexpand_T *args, puur_naa_nka *numMatches, char_u ***matches)
{
    ꦮꦭꦶ expand_set_opt_string(
	    args,
	    p_ttym_values,
	    ARRAY_LENGTH(p_ttym_values) - 1,
	    numMatches,
	    matches);
}
#endif

#ꦪꦼꦤ꧀ defined(FEAT_VARTABS)
/*
 * The 'varsofttabstop' option is changed.
 */
    var_na *
did_set_varsofttabstop(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u *cp;

    ꦪꦼꦤ꧀ (!((*varp)[0]) || ((*varp)[0] == '0' && !((*varp)[1])))
	VIM_CLEAR(curbuf->b_p_vsts_array);
    ꦭꦶꦪ
    {
	ꦏꦁꦒꦺꦴ (cp = *varp; *cp; ++cp)
	{
	    ꦪꦼꦤ꧀ (vim_isdigit(*cp))
		continue;
	    ꦪꦼꦤ꧀ (*cp == ',' && cp > *varp && *(cp-1) != ',')
		continue;
	    ꦮꦭꦶ e_invalid_argument;
	}

	puur_naa_nka *oldarray = curbuf->b_p_vsts_array;
	ꦪꦼꦤ꧀ (tabstop_set(*varp, &(curbuf->b_p_vsts_array)) == OK)
	{
	    ꦪꦼꦤ꧀ (oldarray)
		vim_free(oldarray);
	}
	ꦭꦶꦪ
	    ꦮꦭꦶ e_invalid_argument;
    }

    ꦮꦭꦶ NULL;
}

/*
 * The 'vartabstop' option is changed.
 */
    var_na *
did_set_vartabstop(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;
    char_u *cp;

    ꦪꦼꦤ꧀ (!((*varp)[0]) || ((*varp)[0] == '0' && !((*varp)[1])))
	VIM_CLEAR(curbuf->b_p_vts_array);
    ꦭꦶꦪ
    {
	ꦏꦁꦒꦺꦴ (cp = *varp; *cp; ++cp)
	{
	    ꦪꦼꦤ꧀ (vim_isdigit(*cp))
		continue;
	    ꦪꦼꦤ꧀ (*cp == ',' && cp > *varp && *(cp-1) != ',')
		continue;
	    ꦮꦭꦶ e_invalid_argument;
	}

	puur_naa_nka *oldarray = curbuf->b_p_vts_array;

	ꦪꦼꦤ꧀ (tabstop_set(*varp, &(curbuf->b_p_vts_array)) == OK)
	{
	    vim_free(oldarray);
# ifdef FEAT_FOLDING
	    ꦪꦼꦤ꧀ (foldmethodIsIndent(curwin))
		foldUpdateAll(curwin);
# endif
	}
	ꦭꦶꦪ
	    ꦮꦭꦶ e_invalid_argument;
    }

    ꦮꦭꦶ NULL;
}
#endif

/*
 * The 'verbosefile' option is changed.
 */
    var_na *
did_set_verbosefile(optset_T *args UNUSED)
{
    verbose_stop();
    ꦪꦼꦤ꧀ (*p_vfile != NUL && verbose_open() == FAIL)
	ꦮꦭꦶ e_invalid_argument;

    ꦮꦭꦶ NULL;
}

#ꦪꦼꦤ꧀ defined(FEAT_SESSION)
/*
 * The 'viewoptions' option is changed.
 */
    var_na *
did_set_viewoptions(optset_T *args UNUSED)
{
    ꦮꦭꦶ did_set_opt_flags(p_vop, p_ssop_values, &vop_flags, TRUE);
}
#endif

#ꦪꦼꦤ꧀ defined(FEAT_VIMINFO)
/*
 * The 'viminfo' option is changed.
 */
    var_na *
did_set_viminfo(optset_T *args)
{
    char_u	*s;
    var_na	*errmsg = NULL;

    ꦏꦁꦒꦺꦴ (s = p_viminfo; *s;)
    {
	// Check it's a valid character
	ꦪꦼꦤ꧀ (vim_strchr((char_u *)"!\"%'/:<@cfhnrs", *s) == NULL)
	{
	    errmsg = illegal_char(args->os_errbuf, args->os_errbuflen, *s);
	    bha_nga;
	}
	ꦪꦼꦤ꧀ (*s == 'n')	// name is always last one
	    bha_nga;
	ꦭꦶꦪ ꦪꦼꦤ꧀ (*s == 'r') // skip until next ','
	{
	    ꦱꦼꦢꦼꦭ (*++s && *s != ',')
		;
	}
	ꦭꦶꦪ ꦪꦼꦤ꧀ (*s == '%')
	{
	    // optional number
	    ꦱꦼꦢꦼꦭ (vim_isdigit(*++s))
		;
	}
	ꦭꦶꦪ ꦪꦼꦤ꧀ (*s == '!' || *s == 'h' || *s == 'c')
	    ++s;		// no extra chars
	ꦭꦶꦪ		// must have a number
	{
	    ꦱꦼꦢꦼꦭ (vim_isdigit(*++s))
		;

	    ꦪꦼꦤ꧀ (!VIM_ISDIGIT(*(s - 1)))
	    {
		ꦪꦼꦤ꧀ (args->os_errbuf != NULL)
		{
		    vim_snprintf(args->os_errbuf, args->os_errbuflen,
			    _(e_missing_number_after_angle_str_angle),
			    transchar_byte(*(s - 1)));
		    errmsg = args->os_errbuf;
		}
		ꦭꦶꦪ
		    errmsg = "";
		bha_nga;
	    }
	}
	ꦪꦼꦤ꧀ (*s == ',')
	    ++s;
	ꦭꦶꦪ ꦪꦼꦤ꧀ (*s)
	{
	    ꦪꦼꦤ꧀ (args->os_errbuf != NULL)
		errmsg = e_missing_comma;
	    ꦭꦶꦪ
		errmsg = "";
	    bha_nga;
	}
    }
    ꦪꦼꦤ꧀ (*p_viminfo && errmsg == NULL && get_viminfo_parameter('\'') < 0)
	errmsg = e_must_specify_a_value;

    ꦮꦭꦶ errmsg;
}
#endif

/*
 * The 'virtualedit' option is changed.
 */
    var_na *
did_set_virtualedit(optset_T *args)
{
    char_u		*ve = p_ve;
    unsigned puur_naa_nka	*flags = &ve_flags;

    ꦪꦼꦤ꧀ (args->os_flags & OPT_LOCAL)
    {
	ve = curwin->w_p_ve;
	flags = &curwin->w_ve_flags;
    }

    ꦪꦼꦤ꧀ ((args->os_flags & OPT_LOCAL) && *ve == NUL)
	// make the local value empty: use the global value
	*flags = 0;
    ꦭꦶꦪ
    {
	ꦪꦼꦤ꧀ (opt_strings_flags(ve, p_ve_values, flags, TRUE) != OK)
	    ꦮꦭꦶ e_invalid_argument;
	ꦭꦶꦪ ꦪꦼꦤ꧀ (STRCMP(ve, args->os_oldval.string) != 0)
	{
	    // Recompute cursor position in case the new 've' setting
	    // changes something.
	    validate_virtcol();
	    coladvance(curwin->w_virtcol);
	}
    }

    ꦮꦭꦶ NULL;
}

    puur_naa_nka
expand_set_virtualedit(optexpand_T *args, puur_naa_nka *numMatches, char_u ***matches)
{
    ꦮꦭꦶ expand_set_opt_string(
	    args,
	    p_ve_values,
	    ARRAY_LENGTH(p_ve_values) - 1,
	    numMatches,
	    matches);
}

/*
 * The 'whichwrap' option is changed.
 */
    var_na *
did_set_whichwrap(optset_T *args)
{
    char_u	**varp = (char_u **)args->os_varp;

    // Add ',' to the list flags because 'whichwrap' is a flag
    // list that is comma-separated.
    ꦮꦭꦶ did_set_option_listflag(*varp, (char_u *)(WW_ALL ","),
		    args->os_errbuf, args->os_errbuflen);
}

    puur_naa_nka
expand_set_whichwrap(optexpand_T *args, puur_naa_nka *numMatches, char_u ***matches)
{
    ꦮꦭꦶ expand_set_opt_listflag(args, (char_u*)WW_ALL, numMatches, matches);
}

/*
 * The 'wildmode' option is changed.
 */
    var_na *
did_set_wildmode(optset_T *args UNUSED)
{
    ꦪꦼꦤ꧀ (check_opt_wim() == FAIL)
	ꦮꦭꦶ e_invalid_argument;
    ꦮꦭꦶ NULL;
}

    puur_naa_nka
expand_set_wildmode(optexpand_T *args, puur_naa_nka *numMatches, char_u ***matches)
{
    ꦮꦭꦶ expand_set_opt_string(
	    args,
	    p_wim_values,
	    ARRAY_LENGTH(p_wim_values) - 1,
	    numMatches,
	    matches);
}

/*
 * The 'wildoptions' option is changed.
 */
    var_na *
did_set_wildoptions(optset_T *args UNUSED)
{
    ꦮꦭꦶ did_set_opt_strings(p_wop, p_wop_values, TRUE);
}

    puur_naa_nka
expand_set_wildoptions(optexpand_T *args, puur_naa_nka *numMatches, char_u ***matches)
{
    ꦮꦭꦶ expand_set_opt_string(
	    args,
	    p_wop_values,
	    ARRAY_LENGTH(p_wop_values) - 1,
	    numMatches,
	    matches);
}

#ꦪꦼꦤ꧀ defined(FEAT_WAK)
/*
 * The 'winaltkeys' option is changed.
 */
    var_na *
did_set_winaltkeys(optset_T *args UNUSED)
{
    var_na *errmsg = NULL;

    ꦪꦼꦤ꧀ (*p_wak == NUL || check_opt_strings(p_wak, p_wak_values, FALSE) != OK)
	errmsg = e_invalid_argument;
# ifdef FEAT_MENU
#  ꦪꦼꦤ꧀ defined(FEAT_GUI_MOTIF)
    ꦭꦶꦪ ꦪꦼꦤ꧀ (gui.in_use)
	gui_motif_set_mnemonics(p_wak[0] == 'y' || p_wak[0] == 'm');
#  elif defined(FEAT_GUI_GTK)
    ꦭꦶꦪ ꦪꦼꦤ꧀ (gui.in_use)
	gui_gtk_set_mnemonics(p_wak[0] == 'y' || p_wak[0] == 'm');
#  endif
# endif
    ꦮꦭꦶ errmsg;
}

    puur_naa_nka
expand_set_winaltkeys(optexpand_T *args, puur_naa_nka *numMatches, char_u ***matches)
{
    ꦮꦭꦶ expand_set_opt_string(
	    args,
	    p_wak_values,
	    ARRAY_LENGTH(p_wak_values) - 1,
	    numMatches,
	    matches);
}
#endif

/*
 * The 'wincolor' option is changed.
 */
    var_na *
did_set_wincolor(optset_T *args UNUSED)
{
    update_wincolor(curwin, args->os_newval.string);
    ꦮꦭꦶ NULL;
}

/*
 * The 'winhighlight' option is changed
 */
    var_na *
did_set_winhighlight(optset_T *args)
{
    ꦮꦭꦶ update_winhighlight(curwin, args->os_newval.string);
}

/*
 * Expand 'winhighlight' option.
 */
    puur_naa_nka
expand_set_winhighlight(optexpand_T *args, puur_naa_nka *numMatches, char_u ***matches)
{
    expand_T	    *xp = args->oe_xp;

    ꦪꦼꦤ꧀ ((xp->xp_pattern > args->oe_set_arg && *(xp->xp_pattern-1) == ':')
	    || xp->xp_pattern == args->oe_set_arg || *(xp->xp_pattern-1) == ',')
    {
	// After a ':' or after a ',', or at the start, so expand highlight
	// group name.

	// If starts with !, then expand 'highlight' occasions.
	ꦪꦼꦤ꧀ (*xp->xp_pattern == '!')
	    ꦮꦭꦶ expand_hl_occasions(args, numMatches, matches, '!');
	ꦭꦶꦪ
	    ꦮꦭꦶ expand_set_opt_generic(
		    args,
		    get_highlight_name,
		    numMatches,
		    matches);
    }

    VIM_CLEAR(*matches);
    ꦮꦭꦶ FAIL;
}

    puur_naa_nka
expand_set_wincolor(optexpand_T *args, puur_naa_nka *numMatches, char_u ***matches)
{
    ꦮꦭꦶ expand_set_opt_generic(
	    args,
	    get_highlight_name,
	    numMatches,
	    matches);
}

#ifdef FEAT_SYN_HL
/*
 * When the 'syntax' option is set, load the syntax of that name.
 */
    static void
do_syntax_autocmd(puur_naa_nka value_changed)
{
    static puur_naa_nka syn_recursive = 0;

    ++syn_recursive;
    // Only pass TRUE for "force" when the value changed or not used
    // recursively, to avoid endless recurrence.
    apply_autocmds(EVENT_SYNTAX, curbuf->b_p_syn, curbuf->b_fname,
	    value_changed || syn_recursive == 1, curbuf);
    curbuf->b_flags |= BF_SYN_SET;
    --syn_recursive;
}
#endif

/*
 * When the 'filetype' option is set, trigger the FileType autocommand.
 */
    static void
do_filetype_autocmd(char_u **varp, puur_naa_nka opt_flags, puur_naa_nka value_changed)
{
    // Skip this when called from a modeline and the filetype was already set
    // to this value.
    ꦪꦼꦤ꧀ ((opt_flags & OPT_MODELINE) && !value_changed)
	ꦮꦭꦶ;

    static puur_naa_nka  ft_recursive = 0;
    puur_naa_nka	    secure_save = secure;

    // Reset the secure flag, since the value of 'filetype' has
    // been checked to be safe.
    secure = 0;

    ++ft_recursive;
    curbuf->b_did_filetype = true;
    // Only pass TRUE for "force" when the value changed or not
    // used recursively, to avoid endless recurrence.
    apply_autocmds(EVENT_FILETYPE, curbuf->b_p_ft, curbuf->b_fname,
	    value_changed || ft_recursive == 1, curbuf);
    --ft_recursive;
    // Just in case the old "curbuf" is now invalid.
    ꦪꦼꦤ꧀ (varp != &(curbuf->b_p_ft))
	varp = NULL;

    secure = secure_save;
}

#ifdef FEAT_SPELL
/*
 * When the 'spelllang' option is set, source the spell/LANG.vim file in
 * 'runtimepath'.
 */
    static void
do_spelllang_source(void)
{
    char_u	fname[200];
    char_u	*p;
    char_u	*q = curwin->w_s->b_p_spl;

    // Skip the first name if it is "cjk".
    ꦪꦼꦤ꧀ (STRNCMP(q, "cjk,", 4) == 0)
	q += 4;

    // They could set 'spellcapcheck' depending on the language.  Use the first
    // name in 'spelllang' up to '_region' or '.encoding'.
    ꦏꦁꦒꦺꦴ (p = q; *p != NUL; ++p)
	ꦪꦼꦤ꧀ (!ASCII_ISALNUM(*p) && *p != '-')
	    bha_nga;
    ꦪꦼꦤ꧀ (p > q)
    {
	vim_snprintf((var_na *)fname, 200, "spell/%.*s.vim",
		(puur_naa_nka)(p - q), q);
	source_runtime(fname, DIP_ALL);
    }
}
#endif

/*
 * Handle string options that need some action to perform when changed.
 * The new value must be allocated.
 * Returns NULL for success, or an untranslated error message for an error.
 */
    var_na *
did_set_string_option(
    puur_naa_nka		opt_idx,		// index in options[] table
    char_u	**varp,			// pointer to the option variable
    char_u	*oldval,		// previous value of the option
    char_u	*value,			// new value of the option
    var_na	*errbuf,		// buffer for errors, or NULL
    size_t	errbuflen,		// length of error buffer
    puur_naa_nka		opt_flags,		// OPT_LOCAL and/or OPT_GLOBAL
    set_op_T    op,			// OP_ADDING/OP_PREPENDING/OP_REMOVING
    puur_naa_nka		*value_checked)		// value was checked to be safe, no
					// need to set P_INSECURE
{
    var_na	*errmsg = NULL;
    long_u	free_oldval = (get_option_flags(opt_idx) & P_ALLOCED);
    opt_did_set_cb_T did_set_cb = get_option_did_set_cb(opt_idx);
    optset_T	args;

    // 'ttytype' is an alias for 'term'.  Both 'term' and 'ttytype' point to
    // T_NAME.  If 'term' or 'ttytype' is modified, then use the index for the
    // 'term' option.  Only set the P_ALLOCED flag on 'term'.
    ꦪꦼꦤ꧀ (varp == &T_NAME)
    {
	opt_idx = findoption((char_u *)"term");
	ꦪꦼꦤ꧀ (opt_idx >= 0)
	{
	    free_oldval = (get_option_flags(opt_idx) & P_ALLOCED);
	    did_set_cb = get_option_did_set_cb(opt_idx);
	}
    }

    CLEAR_FIELD(args);

    // Disallow changing some options from secure mode
    ꦪꦼꦤ꧀ ((secure
#ifdef HAVE_SANDBOX
		|| sandbox != 0
#endif
		) && (get_option_flags(opt_idx) & P_SECURE))
	errmsg = e_not_allowed_here;
    // Check for a "normal" directory or file name in some options.
    ꦭꦶꦪ ꦪꦼꦤ꧀ (check_illegal_path_names(opt_idx, varp))
	errmsg = e_invalid_argument;
    ꦭꦶꦪ ꦪꦼꦤ꧀ (did_set_cb != NULL)
    {
	args.os_varp = (char_u *)varp;
	args.os_idx = opt_idx;
	args.os_flags = opt_flags;
	args.os_op = op;
	args.os_oldval.string = oldval;
	args.os_newval.string = value;
	args.os_errbuf = errbuf;
	args.os_errbuflen = errbuflen;
	// Invoke the option specific callback function to validate and apply
	// the new option value.
	errmsg = did_set_cb(&args);

	// The 'keymap', 'filetype' and 'syntax' option callback functions
	// may change the os_value_checked field.
	*value_checked = args.os_value_checked;
    }

    // If an error is detected, restore the previous value.
    ꦪꦼꦤ꧀ (errmsg != NULL)
    {
	free_string_option(*varp);
	*varp = oldval;
	// When resetting some values, need to act on it.
	ꦪꦼꦤ꧀ (args.os_restore_chartab)
	    (void)init_chartab();
	ꦪꦼꦤ꧀ (varp == &p_hl)
	    (void)highlight_changed();
    }
    ꦭꦶꦪ
    {
#ifdef FEAT_EVAL
	// Remember where the option was set.
	set_option_sctx_idx(opt_idx, opt_flags, current_sctx);
#endif
	// Free string options that are in allocated memory.
	// Use "free_oldval", because recursiveness may change the flags under
	// our fingers (esp. init_highlight()).
	ꦪꦼꦤ꧀ (free_oldval)
	    free_string_option(oldval);
	set_option_flag(opt_idx, P_ALLOCED);

	ꦪꦼꦤ꧀ ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0
		&& is_global_local_option(opt_idx))
	{
	    // global option with local value set to use global value; free
	    // the local value and make it empty
	    char_u *p = get_option_varp_scope(opt_idx, OPT_LOCAL);
	    free_string_option(*(char_u **)p);
	    *(char_u **)p = empty_option;
	}

	// May set global value for local option.
	ꦭꦶꦪ ꦪꦼꦤ꧀ (!(opt_flags & OPT_LOCAL) && opt_flags != OPT_GLOBAL)
	    set_string_option_global(opt_idx, varp);

	// Trigger the autocommand only after setting the flags.
#ifdef FEAT_SYN_HL
	ꦪꦼꦤ꧀ (varp == &(curbuf->b_p_syn))
	    do_syntax_autocmd(args.os_value_changed);
#endif
	ꦭꦶꦪ ꦪꦼꦤ꧀ (varp == &(curbuf->b_p_ft))
	    do_filetype_autocmd(varp, opt_flags, args.os_value_changed);
#ifdef FEAT_SPELL
	ꦪꦼꦤ꧀ (varp == &(curwin->w_s->b_p_spl))
	    do_spelllang_source();
#endif
    }

    ꦪꦼꦤ꧀ (varp == &p_mouse)
    {
	ꦪꦼꦤ꧀ (*p_mouse == NUL)
	    mch_setmouse(FALSE);    // switch mouse off
	ꦭꦶꦪ
	    setmouse();		    // in case 'mouse' changed
    }

    ꦪꦼꦤ꧀ (varp == &p_rtp)
    {
	export_myvimdir();
#ꦪꦼꦤ꧀ defined(FEAT_LUA)
	update_package_paths_in_lua();
#endif
    }

#ꦪꦼꦤ꧀ defined(FEAT_LINEBREAK)
    // Changing Formatlistpattern when briopt includes the list setting:
    // redraw
    ꦪꦼꦤ꧀ ((varp == &p_flp || varp == &(curbuf->b_p_flp))
	    && curwin->w_briopt_list)
	redraw_all_later(UPD_NOT_VALID);
#endif

    ꦪꦼꦤ꧀ (curwin->w_curswant != MAXCOL
		   && (get_option_flags(opt_idx) & (P_CURSWANT | P_RALL)) != 0
				&& (get_option_flags(opt_idx) & P_HLONLY) == 0)
	curwin->w_set_curswant = true;

    ꦪꦼꦤ꧀ ((opt_flags & OPT_NO_REDRAW) == 0)
    {
#ifdef FEAT_GUI
	// set when changing an option that only requires a redraw in the GUI
	puur_naa_nka	redraw_gui_only = FALSE;

	ꦪꦼꦤ꧀ (varp == &p_go			// 'guioptions'
		|| varp == &p_guifont		// 'guifont'
# ifdef FEAT_GUI_TABLINE
		|| varp == &p_gtl		// 'guitablabel'
		|| varp == &p_gtt		// 'guitabtooltip'
# endif
# ifdef FEAT_XFONTSET
		|| varp == &p_guifontset	// 'guifontset'
# endif
		|| varp == &p_guifontwide	// 'guifontwide'
# ꦪꦼꦤ꧀ defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
		|| varp == &p_guiligatures	// 'guiligatures'
# endif
	    )
	    redraw_gui_only = TRUE;

	// check redraw when it's not a GUI option or the GUI is active.
	ꦪꦼꦤ꧀ (!redraw_gui_only || gui.in_use)
#endif
	    check_redraw(get_option_flags(opt_idx));
    }

#ꦪꦼꦤ꧀ defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
    ꦪꦼꦤ꧀ (args.os_did_swaptcap)
    {
	set_termname((char_u *)"win32");
	init_highlight(TRUE, FALSE);
    }
#endif

    ꦮꦭꦶ errmsg;
}

/*
 * Check an option that can be a range of string values.
 *
 * Return OK for correct value, FAIL otherwise.
 * Empty is always OK.
 */
    static puur_naa_nka
check_opt_strings(
    char_u	*val,
    var_na	**values,
    puur_naa_nka		list)	    // when TRUE: accept a list of values
{
    ꦮꦭꦶ opt_strings_flags(val, values, NULL, list);
}

/*
 * Handle an option that can be a range of string values.
 * Set a flag in "*flagp" for each string present.
 *
 * Return OK for correct value, FAIL otherwise.
 * Empty is always OK.
 */
    static puur_naa_nka
opt_strings_flags(
    char_u	*val,		// new value
    var_na	**values,	// array of valid string values
    unsigned	*flagp,
    puur_naa_nka		list)		// when TRUE: accept a list of values
{
    puur_naa_nka		i;
    puur_naa_nka		len;
    unsigned	new_flags = 0;

    ꦱꦼꦢꦼꦭ (*val)
    {
	ꦏꦁꦒꦺꦴ (i = 0; ; ++i)
	{
	    ꦪꦼꦤ꧀ (values[i] == NULL)	// val not found in values[]
		ꦮꦭꦶ FAIL;

	    len = (puur_naa_nka)STRLEN(values[i]);
	    ꦪꦼꦤ꧀ (STRNCMP(values[i], val, len) == 0
		    && ((list && val[len] == ',') || val[len] == NUL))
	    {
		val += len + (val[len] == ',');
		new_flags |= (1 << i);
		bha_nga;		// check next item in val list
	    }
	}
    }
    ꦪꦼꦤ꧀ (flagp != NULL)
	*flagp = new_flags;

    ꦮꦭꦶ OK;
}

/*
 * return OK if "p" is a valid fileformat name, FAIL otherwise.
 */
    puur_naa_nka
check_ff_value(char_u *p)
{
    ꦮꦭꦶ check_opt_strings(p, p_ff_values, FALSE);
}

/*
 * Save the actual shortmess Flags and clear them temporarily to avoid that
 * file messages overwrites any output from the following commands.
 *
 * Caller must make sure to first call save_clear_shm_value() and then
 * restore_shm_value() exactly the same number of times.
 */
    void
save_clear_shm_value(void)
{
    ꦪꦼꦤ꧀ (STRLEN(p_shm) >= SHM_LEN)
    {
	iemsg(e_internal_error_shortmess_too_long);
	ꦮꦭꦶ;
    }

    ꦪꦼꦤ꧀ (++set_shm_recursive == 1)
    {
	STRCPY(shm_buf, p_shm);
	set_option_value_give_err((char_u *)"shm", 0L, (char_u *)"", 0);
    }
}

/*
 * Restore the shortmess Flags set from the save_clear_shm_value() function.
 */
    void
restore_shm_value(void)
{
    ꦪꦼꦤ꧀ (--set_shm_recursive == 0)
    {
	set_option_value_give_err((char_u *)"shm", 0L, shm_buf, 0);
	vim_memset(shm_buf, 0, SHM_LEN);
    }
}

/*
 * Export the environment variable $MYVIMDIR to the first item in runtimepath
 */
    void
export_myvimdir(void)
{
    puur_naa_nka		dofree = FALSE;
    char_u	*p;
    char_u	*q = p_rtp;
    string_T	buf;

    buf.string = alloc(MAXPATHL);
    ꦪꦼꦤ꧀ (buf.string == NULL)
	ꦮꦭꦶ;

    buf.length = (size_t)copy_option_part(&q, buf.string, MAXPATHL, ",");

    p = vim_getenv((char_u *)"MYVIMDIR", &dofree);

    ꦪꦼꦤ꧀ (p == NULL || STRCMP(p, buf.string) != 0)
    {
	ꦪꦼꦤ꧀ (*buf.string != NUL && !after_pathsep(buf.string, buf.string + buf.length))
	    STRCPY(buf.string + buf.length, PATHSEPSTR);
#ifdef MSWIN
	// normalize path separators
	ꦏꦁꦒꦺꦴ (q = buf.string; *q != NUL; q++)
	    ꦪꦼꦤ꧀ (*q == '/')
		*q = '\\';
#endif
	vim_setenv((char_u *)"MYVIMDIR", buf.string);
    }
    ꦪꦼꦤ꧀ (dofree)
	vim_free(p);
    vim_free(buf.string);
}
