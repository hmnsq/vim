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

#అన్తర్భావ "vim.h"

స్థిర char_u shm_buf[SHM_LEN];
స్థిర పూర్ణాంకం సమితి_shm_recursive = 0;

స్థిర అక్షరం *(p_ambw_విలువలు[]) = {"single", "double", ఖాళీ};
స్థిర అక్షరం *(p_bg_విలువలు[]) = {"light", "dark", ఖాళీ};
స్థిర అక్షరం *(p_bkc_విలువలు[]) = {"yes", "auto", "no", "breaksymlink", "breakhardlink", ఖాళీ};
స్థిర అక్షరం *(p_bo_విలువలు[]) = {"all", "backspace", "cursor", "complete",
				 "copy", "ctrlg", "error", "esc", "ex",
				 "hangul", "insertmode", "lang", "mess",
				 "showmatch", "operator", "register", "shell",
				 "spell", "term", "wildmode", ఖాళీ};
#ఒకవేళ defined(FEAT_LINEBREAK)
// Note: Keep this in sync with briopt_check()
స్థిర అక్షరం *(p_briopt_విలువలు[]) = {"shift:", "min:", "sbr", "list:", "column:", ఖాళీ};
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_TABPANEL)
// Note: Keep this in sync with tabpanelopt_changed()
స్థిర అక్షరం *(p_tplo_విలువలు[]) = {"align:", "columns:", "scrollbar", "vert", ఖాళీ};
స్థిర అక్షరం *(p_tplo_align_విలువలు[]) = {"left", "right", ఖాళీ};
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_DIFF)
// Note: Keep this in sync with diffopt_changed()
స్థిర అక్షరం *(p_dip_విలువలు[]) = {"filler", "anchor", "context:", "iblank", "icase", "iwhite", "iwhiteall", "iwhiteeol", "horizontal", "vertical", "closeoff", "hiddenoff", "foldcolumn:", "followwrap", "internal", "indent-heuristic", "algorithm:", "inline:", "linematch:", ఖాళీ};
స్థిర అక్షరం *(p_dip_algorithm_విలువలు[]) = {"myers", "minimal", "patience", "histogram", ఖాళీ};
స్థిర అక్షరం *(p_dip_inline_విలువలు[]) = {"none", "simple", "char", "word", ఖాళీ};
#అయితే_ముగింపు
స్థిర అక్షరం *(p_nf_విలువలు[]) = {"bin", "octal", "hex", "alpha", "unsigned", "blank", ఖాళీ};
స్థిర అక్షరం *(p_ff_విలువలు[]) = {FF_UNIX, FF_DOS, FF_MAC, ఖాళీ};
#నిర్వచితమైతే HAVE_CLIPMETHOD
// Note: Keep this in sync with did_set_clipboard()
స్థిర అక్షరం *(p_cb_విలువలు[]) = {"unnamed", "unnamedplus",
# ifdef FEAT_CLIPBOARD
    "autoselect", "autoselectplus", "autoselectml", "html", "exclude:",
# endif
    ఖాళీ};
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_CRYPT
స్థిర అక్షరం *(p_cm_విలువలు[]) = {"zip", "blowfish", "blowfish2",
# ifdef FEAT_SODIUM
    "xchacha20", "xchacha20v2",
# endif
    ఖాళీ};
#అయితే_ముగింపు
స్థిర అక్షరం *(p_cmp_విలువలు[]) = {"internal", "keepascii", ఖాళీ};
#నిర్వచితమైతే FEAT_SYN_HL
// Note: Keep this in sync with fill_culopt_flags()
స్థిర అక్షరం *(p_culopt_విలువలు[]) = {"line", "screenline", "number", "both", ఖాళీ};
#అయితే_ముగింపు
స్థిర అక్షరం *(p_dy_విలువలు[]) = {"lastline", "truncate", "uhex", ఖాళీ};
స్థిర అక్షరం *(p_jop_విలువలు[]) = {"stack", ఖాళీ};
#నిర్వచితమైతే FEAT_FOLDING
స్థిర అక్షరం *(p_fdo_విలువలు[]) = {"all", "block", "hor", "mark", "percent",
				 "quickfix", "search", "tag", "insert",
				 "undo", "jump", ఖాళీ};
#అయితే_ముగింపు
// Note: Keep this in sync with match_keyprotocol()
స్థిర అక్షరం *(p_kpc_protocol_విలువలు[]) = {"none", "mok2", "kitty", ఖాళీ};
#నిర్వచితమైతే FEAT_IMAGE
// Note: Keep this in sync with match_imageprotocol()
స్థిర అక్షరం *(p_ipc_protocol_విలువలు[]) = {"none", "kitty", "sixel", ఖాళీ};
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_PROP_POPUP
// Note: Keep this in sync with parse_popup_option()
స్థిర అక్షరం *(p_popup_cpp_option_విలువలు[]) = {"align:", "border:",
    "borderhighlight:", "close:", "height:", "highlight:", "opacity:",
    "resize:", "shadow:", "width:", ఖాళీ};
స్థిర అక్షరం *(p_popup_pvp_option_విలువలు[]) = {"border:",
    "borderhighlight:", "close:", "height:", "highlight:", "opacity:",
    "resize:", "shadow:", "width:", ఖాళీ};
స్థిర అక్షరం *(p_popup_option_on_off_విలువలు[]) = {"on", "off", ఖాళీ};
స్థిర అక్షరం *(p_popup_cpp_border_విలువలు[]) = {"single", "double", "round",
    "ascii", "on", "off", "custom:", ఖాళీ};
స్థిర అక్షరం *(p_popup_pvp_border_విలువలు[]) = {"single", "double", "round",
    "ascii", "on", "off", "custom:", ఖాళీ};
స్థిర అక్షరం *(p_popup_option_align_విలువలు[]) = {"item", "menu", ఖాళీ};
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_SPELL)
// Note: Keep this in sync with spell_check_sps()
స్థిర అక్షరం *(p_sps_విలువలు[]) = {"best", "fast", "double", "expr:", "file:", "timeout:", ఖాళీ};
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_SESSION
// Also used for 'viewoptions'!  Keep in sync with SSOP_ flags.
స్థిర అక్షరం *(p_ssop_విలువలు[]) = {"buffers", "winpos", "resize", "winsize",
    "localoptions", "options", "help", "blank", "globals", "slash", "unix",
    "sesdir", "curdir", "folds", "cursor", "tabpages", "terminal", "skiprtp",
    ఖాళీ};
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_STL_OPT)
స్థిర అక్షరం *(p_stlo_విలువలు[]) = {"fixedheight", "maxheight:", ఖాళీ};
#అయితే_ముగింపు
// Keep in sync with SWB_ flags in option.h
స్థిర అక్షరం *(p_swb_విలువలు[]) = {"useopen", "usetab", "split", "newtab", "vsplit", "uselast", ఖాళీ};
స్థిర అక్షరం *(p_spk_విలువలు[]) = {"cursor", "screen", "topline", ఖాళీ};
స్థిర అక్షరం *(p_tc_విలువలు[]) = {"followic", "ignore", "match", "followscs", "smart", ఖాళీ};
// Keep in sync with TCL_ flags in option.h
స్థిర అక్షరం *(p_tcl_విలువలు[]) = {"left", "uselast", ఖాళీ};
#ఒకవేళ defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
స్థిర అక్షరం *(p_toolbar_విలువలు[]) = {"text", "icons", "tooltips", "horiz", ఖాళీ};
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_TOOLBAR) && defined(FEAT_GUI_GTK)
స్థిర అక్షరం *(p_tbis_విలువలు[]) = {"tiny", "small", "medium", "large", "huge", "giant", ఖాళీ};
#అయితే_ముగింపు
#ఒకవేళ defined(UNIX) || defined(VMS)
స్థిర అక్షరం *(p_ttym_విలువలు[]) = {"xterm", "xterm2", "dec", "netterm", "jsbterm", "pterm", "urxvt", "sgr", ఖాళీ};
#అయితే_ముగింపు
స్థిర అక్షరం *(p_ve_విలువలు[]) = {"block", "insert", "all", "onemore", "none", "NONE", ఖాళీ};
// Note: Keep this in sync with check_opt_wim()
స్థిర అక్షరం *(p_wim_విలువలు[]) = {"full", "longest", "list", "lastused", "noselect", "noinsert", ఖాళీ};
స్థిర అక్షరం *(p_wop_విలువలు[]) = {"fuzzy", "tagfile", "pum", "exacttext", ఖాళీ};
#నిర్వచితమైతే FEAT_WAK
స్థిర అక్షరం *(p_wak_విలువలు[]) = {"yes", "menu", "no", ఖాళీ};
#అయితే_ముగింపు
స్థిర అక్షరం *(p_mousem_విలువలు[]) = {"extend", "popup", "popup_setpos", "mac", ఖాళీ};
స్థిర అక్షరం *(p_sel_విలువలు[]) = {"inclusive", "exclusive", "old", ఖాళీ};
స్థిర అక్షరం *(p_slm_విలువలు[]) = {"mouse", "key", "cmd", ఖాళీ};
స్థిర అక్షరం *(p_km_విలువలు[]) = {"startsel", "stopsel", ఖాళీ};
#నిర్వచితమైతే FEAT_BROWSE
స్థిర అక్షరం *(p_bsdir_విలువలు[]) = {"current", "last", "buffer", ఖాళీ};
#అయితే_ముగింపు
స్థిర అక్షరం *(p_scbopt_విలువలు[]) = {"ver", "hor", "jump", ఖాళీ};
స్థిర అక్షరం *(p_debug_విలువలు[]) = {"msg", "throw", "beep", ఖాళీ};
స్థిర అక్షరం *(p_ead_విలువలు[]) = {"both", "ver", "hor", ఖాళీ};
స్థిర అక్షరం *(p_buftype_విలువలు[]) = {"nofile", "nowrite", "quickfix", "help", "terminal", "acwrite", "prompt", "popup", ఖాళీ};
స్థిర అక్షరం *(p_bufhidden_విలువలు[]) = {"hide", "unload", "delete", "wipe", ఖాళీ};
స్థిర అక్షరం *(p_bs_విలువలు[]) = {"indent", "eol", "start", "nostop", ఖాళీ};
#నిర్వచితమైతే FEAT_FOLDING
స్థిర అక్షరం *(p_fdm_విలువలు[]) = {"manual", "expr", "marker", "indent", "syntax",
# ifdef FEAT_DIFF
				"diff",
# endif
				ఖాళీ};
స్థిర అక్షరం *(p_fcl_విలువలు[]) = {"all", ఖాళీ};
#అయితే_ముగింపు
స్థిర అక్షరం *(p_cfc_విలువలు[]) = {"keyword", "files", "whole_line", ఖాళీ};
స్థిర అక్షరం *(p_cot_విలువలు[]) = {"menu", "menuone", "longest", "preview", "popup", "popuphidden", "noinsert", "noselect", "fuzzy", "nosort", "preinsert", "nearest", ఖాళీ};
#నిర్వచితమైతే BACKSLASH_లోపల_FILENAME
స్థిర అక్షరం *(p_csl_విలువలు[]) = {"slash", "backslash", ఖాళీ};
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_SIGNS
స్థిర అక్షరం *(p_scl_విలువలు[]) = {"yes", "no", "auto", "number", ఖాళీ};
#అయితే_ముగింపు
#నిర్వచితమైతే UNIX
స్థిర అక్షరం *(p_trz_విలువలు[]) = {"inband", "sigwinch", "", ఖాళీ};
#అయితే_ముగింపు
#ఒకవేళ defined(MSWIN) && defined(FEAT_TERMINAL)
స్థిర అక్షరం *(p_twt_విలువలు[]) = {"winpty", "conpty", "", ఖాళీ};
#అయితే_ముగింపు
స్థిర అక్షరం *(p_sloc_విలువలు[]) = {"last", "statusline", "tabline", ఖాళీ};
స్థిర అక్షరం *(p_sws_విలువలు[]) = {"fsync", "sync", ఖాళీ};

స్థిర పూర్ణాంకం check_opt_అక్షరశ్రేణులు(char_u *val, అక్షరం **విలువలు, పూర్ణాంకం list);
స్థిర పూర్ణాంకం opt_అక్షరశ్రేణులు_flags(char_u *val, అక్షరం **విలువలు, గుర్తులేని *flagp, పూర్ణాంకం list);

/*
 * After setting various option values: recompute variables that depend on
 * option values.
 */
    శూన్య
didset_string_options(శూన్య)
{
    (శూన్య)opt_అక్షరశ్రేణులు_flags(p_cmp, p_cmp_విలువలు, &cmp_flags, TRUE);
    (శూన్య)opt_అక్షరశ్రేణులు_flags(p_bkc, p_bkc_విలువలు, &bkc_flags, TRUE);
    (శూన్య)opt_అక్షరశ్రేణులు_flags(p_bo, p_bo_విలువలు, &bo_flags, TRUE);
    (శూన్య)opt_అక్షరశ్రేణులు_flags(p_cot, p_cot_విలువలు, &cot_flags, TRUE);
#నిర్వచితమైతే FEAT_SESSION
    (శూన్య)opt_అక్షరశ్రేణులు_flags(p_ssop, p_ssop_విలువలు, &ssop_flags, TRUE);
    (శూన్య)opt_అక్షరశ్రేణులు_flags(p_vop, p_ssop_విలువలు, &vop_flags, TRUE);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_FOLDING
    (శూన్య)opt_అక్షరశ్రేణులు_flags(p_fdo, p_fdo_విలువలు, &fdo_flags, TRUE);
#అయితే_ముగింపు
    (శూన్య)opt_అక్షరశ్రేణులు_flags(p_dy, p_dy_విలువలు, &dy_flags, TRUE);
    (శూన్య)opt_అక్షరశ్రేణులు_flags(p_jop, p_jop_విలువలు, &jop_flags, TRUE);
    (శూన్య)opt_అక్షరశ్రేణులు_flags(p_tc, p_tc_విలువలు, &tc_flags, FALSE);
    (శూన్య)opt_అక్షరశ్రేణులు_flags(p_ve, p_ve_విలువలు, &ve_flags, TRUE);
#ఒకవేళ defined(UNIX) || defined(VMS)
    (శూన్య)opt_అక్షరశ్రేణులు_flags(p_ttym, p_ttym_విలువలు, &ttym_flags, FALSE);
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
    (శూన్య)opt_అక్షరశ్రేణులు_flags(p_toolbar, p_toolbar_విలువలు, &toolbar_flags, TRUE);
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_TOOLBAR) && defined(FEAT_GUI_GTK)
    (శూన్య)opt_అక్షరశ్రేణులు_flags(p_tbis, p_tbis_విలువలు, &tbis_flags, FALSE);
#అయితే_ముగింపు
    (శూన్య)opt_అక్షరశ్రేణులు_flags(p_swb, p_swb_విలువలు, &swb_flags, TRUE);
    (శూన్య)opt_అక్షరశ్రేణులు_flags(p_tcl, p_tcl_విలువలు, &tcl_flags, TRUE);
}

#ఒకవేళ defined(FEAT_EVAL)
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
    శూన్య
trigger_optionset_string(
	పూర్ణాంకం	opt_idx,
	పూర్ణాంకం	opt_flags,
	char_u  *oldval,
	char_u  *oldval_l,
	char_u  *oldval_g,
	char_u  *newval)
{
    // Don't do this recursively.
    ఒకవేళ (oldval == ఖాళీ || newval == ఖాళీ
				    || *get_vim_var_str(VV_OPTION_TYPE) != NUL)
	తిరిగి;

    char_u buf_type[7];
    size_t buf_typelen;
    size_t oldvallen;

    oldvallen = STRLEN(oldval);
    సమితి_vim_var_string(VV_OPTION_OLD, oldval, (పూర్ణాంకం)oldvallen);
    సమితి_vim_var_string(VV_OPTION_NEW, newval, -1);
    buf_typelen = vim_snprintf_safelen((అక్షరం *)buf_type, పరిమాణం(buf_type),
	"%s", (opt_flags & OPT_LOCAL) ? "local" : "global");
    సమితి_vim_var_string(VV_OPTION_TYPE, buf_type, (పూర్ణాంకం)buf_typelen);
    ఒకవేళ (opt_flags & OPT_LOCAL)
    {
	సమితి_vim_var_string(VV_OPTION_COMMAND, (char_u *)"setlocal", STRLEN_LITERAL("setlocal"));
	సమితి_vim_var_string(VV_OPTION_OLDLOCAL, oldval, (పూర్ణాంకం)oldvallen);
    }
    ఒకవేళ (opt_flags & OPT_GLOBAL)
    {
	సమితి_vim_var_string(VV_OPTION_COMMAND, (char_u *)"setglobal", STRLEN_LITERAL("setglobal"));
	సమితి_vim_var_string(VV_OPTION_OLDGLOBAL, oldval, (పూర్ణాంకం)oldvallen);
    }
    ఒకవేళ ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	సమితి_vim_var_string(VV_OPTION_COMMAND, (char_u *)"set", STRLEN_LITERAL("set"));
	సమితి_vim_var_string(VV_OPTION_OLDLOCAL, oldval_l, -1);
	సమితి_vim_var_string(VV_OPTION_OLDGLOBAL, oldval_g, -1);
    }
    ఒకవేళ (opt_flags & OPT_MODELINE)
    {
	సమితి_vim_var_string(VV_OPTION_COMMAND, (char_u *)"modeline", STRLEN_LITERAL("modeline"));
	సమితి_vim_var_string(VV_OPTION_OLDLOCAL, oldval, (పూర్ణాంకం)oldvallen);
    }
    apply_autocmds(EVENT_OPTIONSET,
	    get_option_fullname(opt_idx), ఖాళీ, FALSE,
	    ఖాళీ);
    reset_v_option_vars();
}
#అయితే_ముగింపు

    స్థిర అక్షరం *
illegal_char(అక్షరం *errbuf, size_t errbuflen, పూర్ణాంకం c)
{
    ఒకవేళ (errbuf == ఖాళీ)
	తిరిగి "";
    vim_snprintf(errbuf, errbuflen, _(e_illegal_character_str),
		    (అక్షరం *)transchar(c));
    తిరిగి errbuf;
}

    స్థిర అక్షరం *
illegal_char_after_chr(అక్షరం *errbuf, size_t errbuflen, పూర్ణాంకం c)
{
    ఒకవేళ (errbuf == ఖాళీ)
	తిరిగి "";
    vim_snprintf(errbuf, errbuflen, _(e_illegal_character_after_chr), c);
    తిరిగి errbuf;
}

/*
 * Check string options in a buffer for NULL value.
 */
    శూన్య
check_buf_options(buf_T *buf)
{
    check_string_option(&buf->b_p_bh);
    check_string_option(&buf->b_p_bt);
    check_string_option(&buf->b_p_fenc);
    check_string_option(&buf->b_p_ff);
#నిర్వచితమైతే FEAT_కనుగొను_ID
    check_string_option(&buf->b_p_def);
    check_string_option(&buf->b_p_inc);
# ifdef FEAT_EVAL
    check_string_option(&buf->b_p_inex);
# endif
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_EVAL)
    check_string_option(&buf->b_p_inde);
    check_string_option(&buf->b_p_indk);
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_BEVAL) && defined(FEAT_EVAL)
    check_string_option(&buf->b_p_bexpr);
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_CRYPT)
    check_string_option(&buf->b_p_cm);
#అయితే_ముగింపు
    check_string_option(&buf->b_p_fp);
#ఒకవేళ defined(FEAT_EVAL)
    check_string_option(&buf->b_p_fex);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_CRYPT
    check_string_option(&buf->b_p_కీలకం);
#అయితే_ముగింపు
    check_string_option(&buf->b_p_kp);
    check_string_option(&buf->b_p_mps);
    check_string_option(&buf->b_p_fo);
    check_string_option(&buf->b_p_flp);
    check_string_option(&buf->b_p_isk);
    check_string_option(&buf->b_p_com);
    check_string_option(&buf->b_p_cms);
    check_string_option(&buf->b_p_nf);
    check_string_option(&buf->b_p_qe);
#నిర్వచితమైతే FEAT_SYN_HL
    check_string_option(&buf->b_p_syn);
    check_string_option(&buf->b_s.b_syn_isk);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_SPELL
    check_string_option(&buf->b_s.b_p_spc);
    check_string_option(&buf->b_s.b_p_spf);
    check_string_option(&buf->b_s.b_p_spl);
    check_string_option(&buf->b_s.b_p_spo);
#అయితే_ముగింపు
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
#నిర్వచితమైతే FEAT_COMPL_FUNC
    check_string_option(&buf->b_p_cfu);
    check_string_option(&buf->b_p_ofu);
    check_string_option(&buf->b_p_tsrfu);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_EVAL
    check_string_option(&buf->b_p_ffu);
    check_string_option(&buf->b_p_tfu);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_KEYMAP
    check_string_option(&buf->b_p_keymap);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_QUICKFIX
    check_string_option(&buf->b_p_gefm);
    check_string_option(&buf->b_p_gp);
    check_string_option(&buf->b_p_mp);
    check_string_option(&buf->b_p_efm);
#అయితే_ముగింపు
    check_string_option(&buf->b_p_ep);
    check_string_option(&buf->b_p_మార్గం);
    check_string_option(&buf->b_p_ట్యాగులు);
    check_string_option(&buf->b_p_tc);
    check_string_option(&buf->b_p_dict);
#నిర్వచితమైతే FEAT_DIFF
    check_string_option(&buf->b_p_dia);
#అయితే_ముగింపు
    check_string_option(&buf->b_p_tsr);
    check_string_option(&buf->b_p_lw);
    check_string_option(&buf->b_p_bkc);
    check_string_option(&buf->b_p_menc);
#నిర్వచితమైతే FEAT_VARTABS
    check_string_option(&buf->b_p_vsts);
    check_string_option(&buf->b_p_vts);
#అయితే_ముగింపు
}

/*
 * Free the string allocated for an option.
 * Checks for the string being empty_option. This may happen if we're out of
 * memory, vim_strsave() returned NULL, which was replaced by empty_option by
 * check_options().
 * Does NOT check for P_ALLOCED flag!
 */
    శూన్య
free_string_option(char_u *p)
{
    ఒకవేళ (p != ఖాళీలు_option)
	vim_free(p);
}

    శూన్య
clear_string_option(char_u **pp)
{
    ఒకవేళ (*pp != ఖాళీలు_option)
	vim_free(*pp);
    *pp = ఖాళీలు_option;
}

    శూన్య
check_string_option(char_u **pp)
{
    ఒకవేళ (*pp == ఖాళీ)
	*pp = ఖాళీలు_option;
}

/*
 * Set global value for string option when it's a local option.
 */
    స్థిర శూన్య
సమితి_string_option_global(
    పూర్ణాంకం		opt_idx,	// option index
    char_u	**varp)		// pointer to option variable
{
    char_u	**p, *s;

    // the global value is always allocated
    ఒకవేళ (is_కిటికీ_local_option(opt_idx))
	p = (char_u **)GLOBAL_WO(varp);
    లేకపోతే
	p = (char_u **)get_option_var(opt_idx);
    ఒకవేళ (!is_global_option(opt_idx)
	    && p != varp
	    && (s = vim_strsave(*varp)) != ఖాళీ)
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
    శూన్య
సమితి_string_option_ప్రత్యక్ష(
    char_u	*name,
    పూర్ణాంకం		opt_idx,
    char_u	*val,
    పూర్ణాంకం		opt_flags,	// OPT_FREE, OPT_LOCAL and/or OPT_GLOBAL
    పూర్ణాంకం		సమితి_sid వాడని)
{
    char_u	*s;
    char_u	**varp;
    పూర్ణాంకం		both = (opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0;
    పూర్ణాంకం		idx = opt_idx;

    ఒకవేళ (idx == -1)		// use name
    {
	idx = findoption(name);
	ఒకవేళ (idx < 0)	// not found (should not happen)
	{
	    semsg(_(e_అంతర్గత_error_str), "set_string_option_direct()");
	    siemsg("For option %s", name);
	    తిరిగి;
	}
    }

    ఒకవేళ (is_hidden_option(idx))		// can't set hidden option
	తిరిగి;

    s = vim_strsave(val);
    ఒకవేళ (s == ఖాళీ)
	తిరిగి;

    varp = (char_u **)get_option_varp_scope(idx,
	    both ? OPT_LOCAL : opt_flags);
    ఒకవేళ ((opt_flags & OPT_FREE) && (get_option_flags(idx) & P_ALLOCED))
	free_string_option(*varp);
    *varp = s;

    // For buffer/window local option may also set the global value.
    ఒకవేళ (both)
	సమితి_string_option_global(idx, varp);

    సమితి_option_flag(idx, P_ALLOCED);

    // When setting both values of a global option with a local value,
    // make the local value empty, so that the global value is used.
    ఒకవేళ (is_global_local_option(idx) && both)
    {
	free_string_option(*varp);
	*varp = ఖాళీలు_option;
    }
#నిర్వచితమైతే FEAT_EVAL
    ఒకవేళ (సమితి_sid != SID_NONE)
    {
	sctx_T script_ctx;

	ఒకవేళ (సమితి_sid == 0)
	    script_ctx = current_sctx;
	లేకపోతే
	{
	    script_ctx.sc_sid = సమితి_sid;
	    script_ctx.sc_seq = 0;
	    script_ctx.sc_lnum = 0;
	    script_ctx.sc_సంస్కరణ = 1;
	}
	సమితి_option_sctx_idx(idx, opt_flags, script_ctx);
    }
#అయితే_ముగింపు
}

/*
 * Like set_string_option_direct(), but for a window-local option in "wp".
 * Blocks autocommands to avoid the old curwin becoming invalid.
 */
    శూన్య
సమితి_string_option_ప్రత్యక్ష_లోపల_win(
	win_T		*wp,
	char_u		*name,
	పూర్ణాంకం		opt_idx,
	char_u		*val,
	పూర్ణాంకం		opt_flags,
	పూర్ణాంకం		సమితి_sid)
{
    win_T	*save_curwin = curwin;

    ఖండం_autocmds();
    curwin = wp;
    curbuf = curwin->w_బఫరు;
    సమితి_string_option_ప్రత్యక్ష(name, opt_idx, val, opt_flags, సమితి_sid);
    curwin = save_curwin;
    curbuf = curwin->w_బఫరు;
    unblock_autocmds();
}

#ఒకవేళ defined(FEAT_PROP_POPUP)
/*
 * Like set_string_option_direct(), but for a buffer-local option in "buf".
 * Blocks autocommands to avoid the old curbuf becoming invalid.
 */
    శూన్య
సమితి_string_option_ప్రత్యక్ష_లోపల_buf(
	buf_T		*buf,
	char_u		*name,
	పూర్ణాంకం		opt_idx,
	char_u		*val,
	పూర్ణాంకం		opt_flags,
	పూర్ణాంకం		సమితి_sid)
{
    buf_T	*save_curbuf = curbuf;

    ఖండం_autocmds();
    curbuf = buf;
    curwin->w_బఫరు = curbuf;
    సమితి_string_option_ప్రత్యక్ష(name, opt_idx, val, opt_flags, సమితి_sid);
    curbuf = save_curbuf;
    curwin->w_బఫరు = curbuf;
    unblock_autocmds();
}
#అయితే_ముగింపు

/*
 * Set a string option to a new value, and handle the effects.
 *
 * Returns NULL on success or an untranslated error message on error.
 */
    అక్షరం *
సమితి_string_option(
    పూర్ణాంకం		opt_idx,
    char_u	*విలువ,
    పూర్ణాంకం		opt_flags,	// OPT_LOCAL and/or OPT_GLOBAL
    అక్షరం	*errbuf,
    size_t	errbuflen)
{
    char_u	*s;
    char_u	**varp;
    char_u	*oldval;
#ఒకవేళ defined(FEAT_EVAL)
    char_u	*oldval_l = ఖాళీ;
    char_u	*oldval_g = ఖాళీ;
    char_u	*saved_oldval = ఖాళీ;
    char_u	*saved_oldval_l = ఖాళీ;
    char_u	*saved_oldval_g = ఖాళీ;
    char_u	*saved_newval = ఖాళీ;
#అయితే_ముగింపు
    అక్షరం	*errmsg = ఖాళీ;
    పూర్ణాంకం		విలువ_checked = FALSE;

    ఒకవేళ (is_hidden_option(opt_idx))	// don't set hidden option
	తిరిగి ఖాళీ;

    s = vim_strsave(విలువ == ఖాళీ ? (char_u *)"" : విలువ);
    ఒకవేళ (s == ఖాళీ)
	తిరిగి ఖాళీ;

    varp = (char_u **)get_option_varp_scope(opt_idx,
	    (opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0
	    ? (is_global_local_option(opt_idx)
		? OPT_GLOBAL : OPT_LOCAL)
	    : opt_flags);
    oldval = *varp;
#ఒకవేళ defined(FEAT_EVAL)
    ఒకవేళ ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	oldval_l = *(char_u **)get_option_varp_scope(opt_idx, OPT_LOCAL);
	oldval_g = *(char_u **)get_option_varp_scope(opt_idx, OPT_GLOBAL);
    }
#అయితే_ముగింపు
    *varp = s;

#ఒకవేళ defined(FEAT_EVAL)
    ఒకవేళ (!starting
# ifdef FEAT_CRYPT
	    && !is_crypt_కీలకం_option(opt_idx)
# endif
       )
    {
	ఒకవేళ (oldval_l != ఖాళీ)
	    saved_oldval_l = vim_strsave(oldval_l);
	ఒకవేళ (oldval_g != ఖాళీ)
	    saved_oldval_g = vim_strsave(oldval_g);
	saved_oldval = vim_strsave(oldval);
	saved_newval = vim_strsave(s);
    }
#అయితే_ముగింపు
    ఒకవేళ ((errmsg = did_సమితి_string_option(opt_idx, varp, oldval, విలువ, errbuf,
		    errbuflen, opt_flags, OP_NONE, &విలువ_checked)) == ఖాళీ)
	did_సమితి_option(opt_idx, opt_flags, TRUE, విలువ_checked);

#ఒకవేళ defined(FEAT_EVAL)
    // call autocommand after handling side effects
    ఒకవేళ (errmsg == ఖాళీ)
	trigger_optionset_string(opt_idx, opt_flags,
		saved_oldval, saved_oldval_l,
		saved_oldval_g, saved_newval);
    vim_free(saved_oldval);
    vim_free(saved_oldval_l);
    vim_free(saved_oldval_g);
    vim_free(saved_newval);
#అయితే_ముగింపు
    తిరిగి errmsg;
}

/*
 * Return TRUE if "val" is a valid 'filetype' name.
 * Also used for 'syntax' and 'keymap'.
 */
    స్థిర పూర్ణాంకం
valid_filetype(char_u *val)
{
    తిరిగి valid_name(val, ".-_");
}

#నిర్వచితమైతే FEAT_STL_OPT
/*
 * Check validity of options with the 'statusline' format.
 * Return an untranslated error message or NULL.
 */
    స్థిర అక్షరం *
check_stl_option(char_u *s)
{
    పూర్ణాంకం		groupdepth = 0;
    స్థిర అక్షరం errbuf[ERR_BUFLEN];
    పూర్ణాంకం		errbuflen = ERR_BUFLEN;

    అయితే (*s)
    {
	// Check for valid keys after % sequences
	అయితే (*s && *s != '%')
	    s++;
	ఒకవేళ (!*s)
	    ఆపు;
	s++;
	ఒకవేళ (*s == STL_CLICKFUNC)
	{
	    ఒకవేళ (s[1] == ']')
	    {
		// %[] - end click region
		s += 2;
		కొనసాగు;
	    }
	    ఒకవేళ (ASCII_ISALPHA(s[1]) || s[1] == '_')
	    {
		// %[FuncName] - start click region
		char_u *rb = vim_strchr(s + 2, ']');
		ఒకవేళ (rb != ఖాళీ)
		{
		    s = rb + 1;
		    కొనసాగు;
		}
	    }
	    // Bare %[ is invalid
	    తిరిగి illegal_char(errbuf, errbuflen, *s);
	}
	ఒకవేళ (*s == STL_LINEBREAK)
	{
	    // Plain %@ - line break
	    s++;
	    కొనసాగు;
	}
	ఒకవేళ (*s == '%' || *s == STL_TRUNCMARK || *s == STL_SEPARATE)
	{
	    s++;
	    కొనసాగు;
	}
	ఒకవేళ (*s == ')')
	{
	    s++;
	    ఒకవేళ (--groupdepth < 0)
		ఆపు;
	    కొనసాగు;
	}
	ఒకవేళ (*s == '-')
	    s++;
	అయితే (VIM_ISDIGIT(*s))
	    s++;
	ఒకవేళ (*s == STL_USER_HL)
	    కొనసాగు;
	ఒకవేళ (*s == STL_CLICKFUNC)
	{
	    // %N[FuncName] or %N[]
	    ఒకవేళ (s[1] == ']')
	    {
		s += 2;
		కొనసాగు;
	    }
	    ఒకవేళ (ASCII_ISALPHA(s[1]) || s[1] == '_')
	    {
		char_u *rb = vim_strchr(s + 2, ']');
		ఒకవేళ (rb != ఖాళీ)
		{
		    s = rb + 1;
		    కొనసాగు;
		}
	    }
	    // Bare %N[ is invalid
	    తిరిగి illegal_char(errbuf, errbuflen, *s);
	}
	ఒకవేళ (*s == STL_LINEBREAK)
	{
	    // %N@ - line break
	    s++;
	    కొనసాగు;
	}
	ఒకవేళ (*s == '.')
	{
	    s++;
	    అయితే (*s && VIM_ISDIGIT(*s))
		s++;
	}
	ఒకవేళ (*s == '(')
	{
	    groupdepth++;
	    కొనసాగు;
	}
	ఒకవేళ (vim_strchr(STL_ALL, *s) == ఖాళీ)
	{
	    తిరిగి illegal_char(errbuf, errbuflen, *s);
	}
	ఒకవేళ (*s == '{')
	{
	    పూర్ణాంకం reevaluate = (*++s == '%');

	    ఒకవేళ (reevaluate && *++s == '}')
		// "}" is not allowed immediately after "%{%"
		తిరిగి illegal_char(errbuf, errbuflen, '}');
	    అయితే ((*s != '}' || (reevaluate && s[-1] != '%')) && *s)
		s++;
	    ఒకవేళ (*s != '}')
		తిరిగి e_unclosed_వ్యక్తీకరణ_sequence;
	}
    }
    ఒకవేళ (groupdepth != 0)
	తిరిగి e_unbalanced_groups;
    తిరిగి ఖాళీ;
}
#అయితే_ముగింపు

/*
 * Check for a "normal" directory or file name in some options.  Disallow a
 * path separator (slash and/or backslash), wildcards and characters that are
 * often illegal in a file name. Be more permissive if "secure" is off.
 */
    స్థిర పూర్ణాంకం
check_illegal_మార్గం_పేర్లు(పూర్ణాంకం opt_idx, char_u **varp)
{
    తిరిగి (((get_option_flags(opt_idx) & P_NFNAME)
		&& vim_strpbrk(*varp, (char_u *)(secure
			? "/\\*?[|;&<>\r\n" : "/\\*?[<>\r\n")) != ఖాళీ)
	    || ((get_option_flags(opt_idx) & P_NDNAME)
		&& vim_strpbrk(*varp, (char_u *)"*?[|;&<>\r\n") != ఖాళీ));
}

/*
 * An option that accepts a list of flags is changed.
 * e.g. 'viewoptions', 'switchbuf', 'casemap', etc.
 */
    స్థిర అక్షరం *
did_సమితి_opt_flags(char_u *val, అక్షరం **విలువలు, గుర్తులేని *flagp, పూర్ణాంకం list)
{
    ఒకవేళ (opt_అక్షరశ్రేణులు_flags(val, విలువలు, flagp, list) == FAIL)
	తిరిగి e_invalid_వాదం;

    తిరిగి ఖాళీ;
}

/*
 * An option that accepts a list of string values is changed.
 * e.g. 'nrformats', 'scrollopt', 'wildoptions', etc.
 */
    స్థిర అక్షరం *
did_సమితి_opt_అక్షరశ్రేణులు(char_u *val, అక్షరం **విలువలు, పూర్ణాంకం list)
{
    తిరిగి did_సమితి_opt_flags(val, విలువలు, ఖాళీ, list);
}

/*
 * An option which is a list of flags is set.  Valid values are in 'flags'.
 */
    స్థిర అక్షరం *
did_సమితి_option_listflag(
	char_u *val,
	char_u *flags,
	అక్షరం *errbuf,
	size_t errbuflen)
{
    char_u	*s;

    కొరకు (s = val; *s; ++s)
	ఒకవేళ (vim_strchr(flags, *s) == ఖాళీ)
	    తిరిగి illegal_char(errbuf, errbuflen, *s);

    తిరిగి ఖాళీ;
}

/*
 * Expand an option that accepts a list of fixed string values with known
 * number of items.
 */
    స్థిర పూర్ణాంకం
expand_సమితి_opt_string(
	optexpand_T *వాదనలు,
	అక్షరం **విలువలు,
	size_t అంకె_విలువలు,
	పూర్ణాంకం *అంకె_Matches,
	char_u ***matches)
{
    char_u	*p;
    regmatch_T	*regmatch = వాదనలు->oe_regmatch;
    పూర్ణాంకం		include_orig_val = వాదనలు->oe_include_orig_val;
    char_u	*option_val = వాదనలు->oe_opt_విలువ;

    // Assume numValues is small since they are fixed enums, so just allocate
    // upfront instead of needing two passes to calculate output size.
    *matches = ALLOC_MULT(char_u *, అంకె_విలువలు + 1);
    ఒకవేళ (*matches == ఖాళీ)
	తిరిగి FAIL;

    పూర్ణాంకం లెక్క = 0;

    ఒకవేళ (include_orig_val && *option_val != NUL)
    {
	p = vim_strsave(option_val);
	ఒకవేళ (p == ఖాళీ)
	{
	    VIM_CLEAR(*matches);
	    తిరిగి FAIL;
	}
	(*matches)[లెక్క++] = p;
    }

    కొరకు (అక్షరం **val = విలువలు; *val != ఖాళీ; val++)
    {
	ఒకవేళ (include_orig_val && *option_val != NUL)
	{
	    ఒకవేళ (STRCMP((char_u*)*val, option_val) == 0)
		కొనసాగు;
	}
	ఒకవేళ (vim_regexec(regmatch, (char_u*)(*val), (colnr_T)0))
	{
	    p = vim_strsave((char_u*)*val);
	    ఒకవేళ (p == ఖాళీ)
	    {
		ఒకవేళ (లెక్క == 0)
		{
		    VIM_CLEAR(*matches);
		    తిరిగి FAIL;
		}
		లేకపోతే
		    ఆపు;
	    }
	    (*matches)[లెక్క++] = p;
	}
    }
    ఒకవేళ (లెక్క == 0)
    {
	VIM_CLEAR(*matches);
	తిరిగి FAIL;
    }
    *అంకె_Matches = లెక్క;
    తిరిగి OK;
}

స్థిర char_u *సమితి_opt_callback_orig_option = ఖాళీ;
స్థిర char_u *((*సమితి_opt_callback_func)(expand_T *, పూర్ణాంకం));

/*
 * Callback used by expand_set_opt_generic to also include the original value
 * as the first item.
 */
    స్థిర char_u *
expand_సమితి_opt_generic_cb(expand_T *xp, పూర్ణాంకం idx)
{
    ఒకవేళ (idx == 0)
    {
	ఒకవేళ (సమితి_opt_callback_orig_option != ఖాళీ)
	    తిరిగి సమితి_opt_callback_orig_option;
	లేకపోతే
	    తిరిగి (char_u *)""; // empty strings are ignored
    }
    తిరిగి సమితి_opt_callback_func(xp, idx - 1);
}

/*
 * Expand an option with a callback that iterates through a list of possible
 * names using an index.
 */
    స్థిర పూర్ణాంకం
expand_సమితి_opt_generic(
	optexpand_T *వాదనలు,
	char_u *((*func)(expand_T *, పూర్ణాంకం)),
	పూర్ణాంకం *అంకె_Matches,
	char_u ***matches)
{
    పూర్ణాంకం ret;

    సమితి_opt_callback_orig_option = వాదనలు->oe_include_orig_val ?
	వాదనలు->oe_opt_విలువ : ఖాళీ;
    సమితి_opt_callback_func = func;

    ret = ExpandGeneric(
	    (char_u*)"", // not using fuzzy as currently EXPAND_STRING_SETTING doesn't use it
	    వాదనలు->oe_xp,
	    వాదనలు->oe_regmatch,
	    matches,
	    అంకె_Matches,
	    expand_సమితి_opt_generic_cb,
	    FALSE);

    సమితి_opt_callback_orig_option = ఖాళీ;
    సమితి_opt_callback_func = ఖాళీ;
    తిరిగి ret;
}

#ఒకవేళ defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
స్థిర garray_T *expand_cb_ga;
స్థిర optexpand_T *expand_cb_వాదనలు;

/*
 * Callback provided to a function in expand_set_opt_callback. Will perform
 * regex matching against the value and add to the list.
 *
 * Returns OK usually. Returns FAIL if it failed to allocate memory, and the
 * caller should terminate the enumeration.
 */
    స్థిర పూర్ణాంకం
expand_సమితి_opt_callback_cb(char_u *val)
{
    regmatch_T	*regmatch = expand_cb_వాదనలు->oe_regmatch;
    expand_T	*xp = expand_cb_వాదనలు->oe_xp;
    garray_T	*ga = expand_cb_ga;
    char_u	*str;

    ఒకవేళ (val == ఖాళీ || *val == NUL)
	తిరిగి OK;

    ఒకవేళ (xp->xp_నమూనా[0] != NUL &&
	    !vim_regexec(regmatch, val, (colnr_T)0))
	తిరిగి OK;

    str = vim_strsave_escaped(val, (char_u *)" \t\\");

    ఒకవేళ (str == ఖాళీ)
	తిరిగి FAIL;

    ఒకవేళ (ga_grow(ga, 1) == FAIL)
    {
	vim_free(str);
	తిరిగి FAIL;
    }

    ((char_u **)ga->ga_దత్తాంశం)[ga->ga_len] = str;
    ++ga->ga_len;
    తిరిగి OK;
}

/*
 * Expand an option with a provided function that takes a callback. The
 * function will enumerate through all options and call the callback to add it
 * to the list.
 *
 * "func" is the enumerator function that will generate the list of options.
 * "func_params" is a single parameter that will be passed to func.
 */
    స్థిర పూర్ణాంకం
expand_సమితి_opt_callback(
	optexpand_T *వాదనలు,
	శూన్య (*func)(optexpand_T *, శూన్య* params, పూర్ణాంకం (*cb)(char_u *val)),
	శూన్య *func_params,
	పూర్ణాంకం *అంకె_Matches,
	char_u ***matches)
{
    garray_T	ga;
    పూర్ణాంకం		include_orig_val = వాదనలు->oe_include_orig_val;
    char_u	*option_val = వాదనలు->oe_opt_విలువ;

    ga_init2(&ga, పరిమాణం(అక్షరం *), 30);

    ఒకవేళ (include_orig_val && *option_val != NUL)
    {
	char_u *p = vim_strsave(option_val);
	ఒకవేళ (p == ఖాళీ)
	    తిరిగి FAIL;
	ఒకవేళ (ga_grow(&ga, 1) == FAIL)
	{
	    vim_free(p);
	    తిరిగి FAIL;
	}
	((char_u **)ga.ga_దత్తాంశం)[ga.ga_len] = p;
	++ga.ga_len;
    }

    expand_cb_ga = &ga;
    expand_cb_వాదనలు = వాదనలు;

    func(వాదనలు, func_params, expand_సమితి_opt_callback_cb);

    expand_cb_ga = ఖాళీ;
    expand_cb_వాదనలు = ఖాళీ;

    *matches = ga.ga_దత్తాంశం;
    *అంకె_Matches = ga.ga_len;
    తిరిగి OK;
}
#అయితే_ముగింపు

/*
 * Expand an option which is a list of flags.
 */
    స్థిర పూర్ణాంకం
expand_సమితి_opt_listflag(
	optexpand_T *వాదనలు,
	char_u *flags,
	పూర్ణాంకం *అంకె_Matches,
	char_u ***matches)
{
    char_u  *p;
    char_u  *option_val = వాదనలు->oe_opt_విలువ;
    char_u  *cmdline_val = వాదనలు->oe_సమితి_arg;
    పూర్ణాంకం	    append = వాదనలు->oe_append;
    పూర్ణాంకం	    include_orig_val = వాదనలు->oe_include_orig_val && (*option_val != NUL);

    size_t అంకె_flags = STRLEN(flags);

    // Assume we only have small number of flags, so just allocate max size.
    *matches = ALLOC_MULT(char_u *, అంకె_flags + 1);
    ఒకవేళ (*matches == ఖాళీ)
	తిరిగి FAIL;

    పూర్ణాంకం లెక్క = 0;

    ఒకవేళ (include_orig_val)
    {
	p = vim_strsave(option_val);
	ఒకవేళ (p == ఖాళీ)
	{
	    VIM_CLEAR(*matches);
	    తిరిగి FAIL;
	}
	(*matches)[లెక్క++] = p;
    }

    కొరకు (char_u *flag = flags; *flag != NUL; flag++)
    {
	ఒకవేళ (append && vim_strchr(option_val, *flag) != ఖాళీ)
	    కొనసాగు;

	ఒకవేళ (vim_strchr(cmdline_val, *flag) == ఖాళీ)
	{
	    ఒకవేళ (include_orig_val
		    && option_val[1] == NUL
		    && *flag == option_val[0])
	    {
		// This value is already used as the first choice as it's the
		// existing flag. Just skip it to avoid duplicate.
		కొనసాగు;
	    }
	    p = vim_strnsave(flag, 1);
	    ఒకవేళ (p == ఖాళీ)
	    {
		ఒకవేళ (లెక్క == 0)
		{
		    VIM_CLEAR(*matches);
		    తిరిగి FAIL;
		}
		లేకపోతే
		    ఆపు;
	    }
	    (*matches)[లెక్క++] = p;
	}
    }

    ఒకవేళ (లెక్క == 0)
    {
	VIM_CLEAR(*matches);
	తిరిగి FAIL;
    }
    *అంకె_Matches = లెక్క;
    తిరిగి OK;
}

/*
 * The 'ambiwidth' option is changed.
 */
    అక్షరం *
did_సమితి_ambiwidth(optset_T *వాదనలు వాడని)
{
    ఒకవేళ (check_opt_అక్షరశ్రేణులు(p_ambw, p_ambw_విలువలు, FALSE) != OK)
	తిరిగి e_invalid_వాదం;

    తిరిగి check_chars_options();
}

// "name" must be a string literal, the length is computed at compile time.
#నిర్వచించు completing_విలువ_for_subopt(వాదనలు, name) \
	  completing_విలువ_for_subopt_len(వాదనలు, name, (పూర్ణాంకం)STRLEN_LITERAL(name))

/*
 * Return true when completing the value of the sub-option "name" with length
 * "len", e.g. the value after "close:" in 'completepopup'.
 */
    స్థిర bool
completing_విలువ_for_subopt_len(optexpand_T *వాదనలు, అక్షరం *name, పూర్ణాంకం len)
{
    char_u  *colon = వాదనలు->oe_xp->xp_నమూనా - 1;
    పూర్ణాంకం	    off = (పూర్ణాంకం)(colon - వాదనలు->oe_సమితి_arg);

    ఒకవేళ (off < len)
	తిరిగి false;
    // The name must follow a comma when it does not start the option value.
    ఒకవేళ (off > len && *(colon - len - 1) != ',')
	తిరిగి false;

    తిరిగి STRNCMP(colon - len, name, len) == 0;
}

    పూర్ణాంకం
expand_సమితి_ambiwidth(optexpand_T *వాదనలు, పూర్ణాంకం *అంకె_Matches, char_u ***matches)
{
    తిరిగి expand_సమితి_opt_string(
	    వాదనలు,
	    p_ambw_విలువలు,
	    ARRAY_నిడివి(p_ambw_విలువలు) - 1,
	    అంకె_Matches,
	    matches);
}

/*
 * The 'background' option is changed.
 */
    అక్షరం *
did_సమితి_background(optset_T *వాదనలు)
{
    ఒకవేళ (check_opt_అక్షరశ్రేణులు(p_bg, p_bg_విలువలు, FALSE) == FAIL)
	తిరిగి e_invalid_వాదం;

    ఒకవేళ (వాదనలు->os_oldval.string != ఖాళీ && వాదనలు->os_oldval.string[0] == *p_bg)
	// Value was not changed
	తిరిగి ఖాళీ;

#నిర్వచితమైతే FEAT_EVAL
    పూర్ణాంకం dark = (*p_bg == 'd');
#అయితే_ముగింపు

    init_highlight(FALSE, FALSE);

#నిర్వచితమైతే FEAT_EVAL
    ఒకవేళ (dark != (*p_bg == 'd')
	    && get_var_విలువ((char_u *)"g:colors_name") != ఖాళీ)
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
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_TERMINAL
    పదం_update_colors_all();
#అయితే_ముగింపు

    తిరిగి ఖాళీ;
}

    పూర్ణాంకం
expand_సమితి_background(optexpand_T *వాదనలు, పూర్ణాంకం *అంకె_Matches, char_u ***matches)
{
    తిరిగి expand_సమితి_opt_string(
	    వాదనలు,
	    p_bg_విలువలు,
	    ARRAY_నిడివి(p_bg_విలువలు) - 1,
	    అంకె_Matches,
	    matches);
}

/*
 * The 'backspace' option is changed.
 */
    అక్షరం *
did_సమితి_backspace(optset_T *వాదనలు వాడని)
{
    ఒకవేళ (VIM_ISDIGIT(*p_bs))
    {
	ఒకవేళ (*p_bs > '3' || p_bs[1] != NUL)
	    తిరిగి e_invalid_వాదం;
    }
    లేకపోతే ఒకవేళ (check_opt_అక్షరశ్రేణులు(p_bs, p_bs_విలువలు, TRUE) != OK)
	తిరిగి e_invalid_వాదం;

    తిరిగి ఖాళీ;
}

    పూర్ణాంకం
expand_సమితి_backspace(optexpand_T *వాదనలు, పూర్ణాంకం *అంకె_Matches, char_u ***matches)
{
    తిరిగి expand_సమితి_opt_string(
	    వాదనలు,
	    p_bs_విలువలు,
	    ARRAY_నిడివి(p_bs_విలువలు) - 1,
	    అంకె_Matches,
	    matches);
}

/*
 * The 'backupcopy' option is changed.
 */
    అక్షరం *
did_సమితి_backupcopy(optset_T *వాదనలు)
{
    char_u		*bkc = p_bkc;
    గుర్తులేని పూర్ణాంకం	*flags = &bkc_flags;
    అక్షరం		*errmsg = ఖాళీ;

    ఒకవేళ (వాదనలు->os_flags & OPT_LOCAL)
    {
	bkc = curbuf->b_p_bkc;
	flags = &curbuf->b_bkc_flags;
    }
    లేకపోతే ఒకవేళ (!(వాదనలు->os_flags & OPT_GLOBAL))
	// When using :set, clear the local flags.
	curbuf->b_bkc_flags = 0;

    ఒకవేళ ((వాదనలు->os_flags & OPT_LOCAL) && *bkc == NUL)
	// make the local value empty: use the global value
	*flags = 0;
    లేకపోతే
    {
	ఒకవేళ (opt_అక్షరశ్రేణులు_flags(bkc, p_bkc_విలువలు, flags, TRUE) != OK)
	    errmsg = e_invalid_వాదం;
	ఒకవేళ ((((పూర్ణాంకం)*flags & BKC_AUTO) != 0)
		+ (((పూర్ణాంకం)*flags & BKC_YES) != 0)
		+ (((పూర్ణాంకం)*flags & BKC_NO) != 0) != 1)
	{
	    // Must have exactly one of "auto", "yes"  and "no".
	    (శూన్య)opt_అక్షరశ్రేణులు_flags(వాదనలు->os_oldval.string, p_bkc_విలువలు,
								  flags, TRUE);
	    errmsg = e_invalid_వాదం;
	}
    }

    తిరిగి errmsg;
}

    పూర్ణాంకం
expand_సమితి_backupcopy(optexpand_T *వాదనలు, పూర్ణాంకం *అంకె_Matches, char_u ***matches)
{
    తిరిగి expand_సమితి_opt_string(
	    వాదనలు,
	    p_bkc_విలువలు,
	    ARRAY_నిడివి(p_bkc_విలువలు) - 1,
	    అంకె_Matches,
	    matches);
}

/*
 * The 'backupext' or the 'patchmode' option is changed.
 */
    అక్షరం *
did_సమితి_backupext_or_patchmode(optset_T *వాదనలు వాడని)
{
    ఒకవేళ (STRCMP(*p_bex == '.' ? p_bex + 1 : p_bex,
		*p_pm == '.' ? p_pm + 1 : p_pm) == 0)
	తిరిగి e_backupext_and_patchmode_are_equal;

    తిరిగి ఖాళీ;
}

/*
 * The 'belloff' option is changed.
 */
    అక్షరం *
did_సమితి_belloff(optset_T *వాదనలు వాడని)
{
    తిరిగి did_సమితి_opt_flags(p_bo, p_bo_విలువలు, &bo_flags, TRUE);
}

    పూర్ణాంకం
expand_సమితి_belloff(optexpand_T *వాదనలు, పూర్ణాంకం *అంకె_Matches, char_u ***matches)
{
    తిరిగి expand_సమితి_opt_string(
	    వాదనలు,
	    p_bo_విలువలు,
	    ARRAY_నిడివి(p_bo_విలువలు) - 1,
	    అంకె_Matches,
	    matches);
}

#ఒకవేళ defined(FEAT_LINEBREAK)
/*
 * The 'breakat' option is changed.
 */
    అక్షరం *
did_సమితి_breakat(optset_T *వాదనలు వాడని)
{
    char_u	*p;
    పూర్ణాంకం		i;

    కొరకు (i = 0; i < 256; i++)
	breakat_flags[i] = FALSE;

    ఒకవేళ (p_breakat != ఖాళీ)
	కొరకు (p = p_breakat; *p; p++)
	    breakat_flags[*p] = TRUE;

    తిరిగి ఖాళీ;
}

/*
 * The 'breakindentopt' option is changed.
 */
    అక్షరం *
did_సమితి_breakindentopt(optset_T *వాదనలు)
{
    char_u	**varp = (char_u **)వాదనలు->os_varp;

    ఒకవేళ (briopt_check(*varp, varp == &curwin->w_p_briopt ? curwin : ఖాళీ)
								      == FAIL)
	తిరిగి e_invalid_వాదం;

    // list setting requires a redraw
    ఒకవేళ (varp == &curwin->w_p_briopt && curwin->w_briopt_list)
	redraw_all_later(UPD_NOT_VALID);

    తిరిగి ఖాళీ;
}

    పూర్ణాంకం
expand_సమితి_breakindentopt(
    optexpand_T		*వాదనలు,
    పూర్ణాంకం			*అంకె_Matches,
    char_u		***matches)
{
    తిరిగి expand_సమితి_opt_string(
	    వాదనలు,
	    p_briopt_విలువలు,
	    ARRAY_నిడివి(p_briopt_విలువలు) - 1,
	    అంకె_Matches,
	    matches);
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_BROWSE)
/*
 * The 'browsedir' option is changed.
 */
    అక్షరం *
did_సమితి_browsedir(optset_T *వాదనలు వాడని)
{
    ఒకవేళ (check_opt_అక్షరశ్రేణులు(p_bsdir, p_bsdir_విలువలు, FALSE) != OK
	    && !mch_isdir(p_bsdir))
	తిరిగి e_invalid_వాదం;

    తిరిగి ఖాళీ;
}

    పూర్ణాంకం
expand_సమితి_browsedir(optexpand_T *వాదనలు, పూర్ణాంకం *అంకె_Matches, char_u ***matches)
{
    తిరిగి expand_సమితి_opt_string(
	    వాదనలు,
	    p_bsdir_విలువలు,
	    ARRAY_నిడివి(p_bsdir_విలువలు) - 1,
	    అంకె_Matches,
	    matches);
}
#అయితే_ముగింపు

/*
 * The 'bufhidden' option is changed.
 */
    అక్షరం *
did_సమితి_bufhidden(optset_T *వాదనలు వాడని)
{
    తిరిగి did_సమితి_opt_అక్షరశ్రేణులు(curbuf->b_p_bh, p_bufhidden_విలువలు, FALSE);
}

    పూర్ణాంకం
expand_సమితి_bufhidden(optexpand_T *వాదనలు, పూర్ణాంకం *అంకె_Matches, char_u ***matches)
{
    తిరిగి expand_సమితి_opt_string(
	    వాదనలు,
	    p_bufhidden_విలువలు,
	    ARRAY_నిడివి(p_bufhidden_విలువలు) - 1,
	    అంకె_Matches,
	    matches);
}

/*
 * The 'buftype' option is changed.
 */
    అక్షరం *
did_సమితి_buftype(optset_T *వాదనలు వాడని)
{
    ఒకవేళ (check_opt_అక్షరశ్రేణులు(curbuf->b_p_bt, p_buftype_విలువలు, FALSE) != OK)
	తిరిగి e_invalid_వాదం;

    ఒకవేళ (curwin->w_స్థితి_ఎత్తు)
    {
	curwin->w_redr_స్థితి = true;
	redraw_later(UPD_VALID);
    }
    curbuf->b_help = (curbuf->b_p_bt[0] == 'h');
    redraw_titles();

    తిరిగి ఖాళీ;
}

    పూర్ణాంకం
expand_సమితి_buftype(optexpand_T *వాదనలు, పూర్ణాంకం *అంకె_Matches, char_u ***matches)
{
    తిరిగి expand_సమితి_opt_string(
	    వాదనలు,
	    p_buftype_విలువలు,
	    ARRAY_నిడివి(p_buftype_విలువలు) - 1,
	    అంకె_Matches,
	    matches);
}

/*
 * The 'casemap' option is changed.
 */
    అక్షరం *
did_సమితి_casemap(optset_T *వాదనలు వాడని)
{
    తిరిగి did_సమితి_opt_flags(p_cmp, p_cmp_విలువలు, &cmp_flags, TRUE);
}

    పూర్ణాంకం
expand_సమితి_casemap(optexpand_T *వాదనలు, పూర్ణాంకం *అంకె_Matches, char_u ***matches)
{
    తిరిగి expand_సమితి_opt_string(
	    వాదనలు,
	    p_cmp_విలువలు,
	    ARRAY_నిడివి(p_cmp_విలువలు) - 1,
	    అంకె_Matches,
	    matches);
}

#ఒకవేళ defined(HAVE_CLIPMETHOD)
    పూర్ణాంకం
expand_సమితి_clipboard(optexpand_T *వాదనలు, పూర్ణాంకం *అంకె_Matches, char_u ***matches)
{
    తిరిగి expand_సమితి_opt_string(
	    వాదనలు,
	    p_cb_విలువలు,
	    ARRAY_నిడివి(p_cb_విలువలు) - 1,
	    అంకె_Matches,
	    matches);
}
#అయితే_ముగింపు

#నిర్వచితమైతే HAVE_CLIPMETHOD
    అక్షరం *
did_సమితి_clipmethod(optset_T *వాదనలు వాడని)
{
    తిరిగి ఎంచుకో_clipmethod();
}

    పూర్ణాంకం
expand_సమితి_clipmethod(optexpand_T *వాదనలు, పూర్ణాంకం *అంకె_Matches, char_u ***matches)
{
    // We want to expand using the predefined clipmethod values + clipboard
    // provider names.
    పూర్ణాంకం		ఫలితం;
    అక్షరం	**విలువలు;
    పూర్ణాంకం		లెక్క, pos = 0, ప్రారంభం = 0;
# ifdef FEAT_EVAL
    dict_T	*providers = get_vim_var_dict(VV_CLIPPROVIDERS);
# లేకపోతే
    dict_T	*providers = ఖాళీ;
# endif
    hashtab_T	*ht = providers == ఖాళీ ? ఖాళీ : &providers->dv_hashtab;

    లెక్క = (ht == ఖాళీ ? 0 : ht->ht_వాడిన);
# ifdef FEAT_WAYLAND_CLIPBOARD
    లెక్క++;
    ప్రారంభం++;
# endif
# ifdef FEAT_XCLIPBOARD
    లెక్క++;
    ప్రారంభం++;
# endif
    విలువలు = ALLOC_MULT(అక్షరం *, లెక్క + 1); // Add NULL terminator too

    ఒకవేళ (విలువలు == ఖాళీ)
	తిరిగి FAIL;

# ifdef FEAT_WAYLAND_CLIPBOARD
    విలువలు[pos++] = "wayland";
# endif
# ifdef FEAT_XCLIPBOARD
    విలువలు[pos++] = "x11";
# endif

    ఒకవేళ (ht != ఖాళీ)
	కొరకు (long_u i = 0; i < ht->ht_ముసుగు + 1; i++)
	{
	    hashitem_T	*hi = ht->ht_array + i;

	    ఒకవేళ (!HASHITEM_ఖాళీలు(hi))
		విలువలు[pos++] = (అక్షరం *)vim_strsave(hi->hi_కీలకం);
	}
    విలువలు[pos++] = ఖాళీ;

    ఫలితం = expand_సమితి_opt_string(
	    వాదనలు,
	    విలువలు,
	    లెక్క,
	    అంకె_Matches,
	    matches);

    కొరకు (పూర్ణాంకం i = ప్రారంభం; i < లెక్క; i++)
	vim_free(విలువలు[i]);
    vim_free(విలువలు);

    తిరిగి ఫలితం;
}
#అయితే_ముగింపు

/*
 * The global 'listchars' or 'fillchars' option is changed.
 */
    స్థిర అక్షరం *
did_సమితి_global_listfillchars(char_u *val, పూర్ణాంకం opt_lcs, పూర్ణాంకం opt_flags,
						అక్షరం *errbuf, size_t errbuflen)
{
    అక్షరం	*errmsg = ఖాళీ;
    char_u	**local_ptr = opt_lcs ? &curwin->w_p_lcs : &curwin->w_p_fcs;

    // only apply the global value to "curwin" when it does not have a
    // local value
    ఒకవేళ (opt_lcs)
	errmsg = సమితి_listchars_option(curwin, val,
		**local_ptr == NUL || !(opt_flags & OPT_GLOBAL),
							    errbuf, errbuflen);
    లేకపోతే
	errmsg = సమితి_fillchars_option(curwin, val,
		**local_ptr == NUL || !(opt_flags & OPT_GLOBAL),
							    errbuf, errbuflen);
    ఒకవేళ (errmsg != ఖాళీ)
	తిరిగి errmsg;

    tabpage_T	*tp;
    win_T	*wp;

    // If the current window is set to use the global
    // 'listchars'/'fillchars' value, clear the window-local value.
    ఒకవేళ (!(opt_flags & OPT_GLOBAL))
	clear_string_option(local_ptr);
    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	// If the current window has a local value need to apply it
	// again, it was changed when setting the global value.
	// If no error was returned above, we don't expect an error
	// here, so ignore the return value.
	ఒకవేళ (opt_lcs)
	{
	    ఒకవేళ (*wp->w_p_lcs == NUL)
		(శూన్య)సమితి_listchars_option(wp, wp->w_p_lcs, TRUE, ఖాళీ, 0);
	}
	లేకపోతే
	{
	    ఒకవేళ (*wp->w_p_fcs == NUL)
		(శూన్య)సమితి_fillchars_option(wp, wp->w_p_fcs, TRUE, ఖాళీ, 0);
	}
    }

    redraw_all_later(UPD_NOT_VALID);

    తిరిగి ఖాళీ;
}

/*
 * The 'fillchars' option or the 'listchars' option is changed.
 */
    అక్షరం *
did_సమితి_chars_option(optset_T *వాదనలు)
{
    char_u **varp = (char_u **)వాదనలు->os_varp;
    అక్షరం *errmsg = ఖాళీ;

    ఒకవేళ (   varp == &p_lcs		// global 'listchars'
	|| varp == &p_fcs)		// global 'fillchars'
	errmsg = did_సమితి_global_listfillchars(*varp, varp == &p_lcs,
			  వాదనలు->os_flags, వాదనలు->os_errbuf, వాదనలు->os_errbuflen);
    లేకపోతే ఒకవేళ (varp == &curwin->w_p_lcs)	// local 'listchars'
	errmsg = సమితి_listchars_option(curwin, *varp, TRUE,
					  వాదనలు->os_errbuf, వాదనలు->os_errbuflen);
    లేకపోతే ఒకవేళ (varp == &curwin->w_p_fcs)	// local 'fillchars'
	errmsg = సమితి_fillchars_option(curwin, *varp, TRUE,
					  వాదనలు->os_errbuf, వాదనలు->os_errbuflen);

    తిరిగి errmsg;
}

/*
 * Expand 'fillchars' or 'listchars' option value.
 */
    పూర్ణాంకం
expand_సమితి_chars_option(optexpand_T *వాదనలు, పూర్ణాంకం *అంకె_Matches, char_u ***matches)
{
    char_u **varp = (char_u **)వాదనలు->oe_varp;
    పూర్ణాంకం is_lcs = (varp == &p_lcs || varp == &curwin->w_p_lcs);
    తిరిగి expand_సమితి_opt_generic(
	    వాదనలు,
	    is_lcs ? get_listchars_name : get_fillchars_name,
	    అంకె_Matches,
	    matches);
}

/*
 * The 'cinoptions' option is changed.
 */
    అక్షరం *
did_సమితి_cinoptions(optset_T *వాదనలు వాడని)
{
    // TODO: recognize errors
    parse_cino(curbuf);

    తిరిగి ఖాళీ;
}

#ఒకవేళ defined(FEAT_SYN_HL)
/*
 * The 'colorcolumn' option is changed.
 */
    అక్షరం *
did_సమితి_colorcolumn(optset_T *వాదనలు)
{
    char_u	**varp = (char_u **)వాదనలు->os_varp;

    తిరిగి check_colorcolumn(*varp, varp == &curwin->w_p_cc ? curwin : ఖాళీ);
}
#అయితే_ముగింపు

/*
 * The 'comments' option is changed.
 */
    అక్షరం *
did_సమితి_comments(optset_T *వాదనలు)
{
    char_u	**varp = (char_u **)వాదనలు->os_varp;
    char_u	*s;
    అక్షరం	*errmsg = ఖాళీ;

    కొరకు (s = *varp; *s; )
    {
	అయితే (*s && *s != ':')
	{
	    ఒకవేళ (vim_strchr((char_u *)COM_ALL, *s) == ఖాళీ
		    && !VIM_ISDIGIT(*s) && *s != '-')
	    {
		errmsg = illegal_char(వాదనలు->os_errbuf, వాదనలు->os_errbuflen, *s);
		ఆపు;
	    }
	    ++s;
	}
	ఒకవేళ (*s++ == NUL)
	    errmsg = e_missing_colon;
	లేకపోతే ఒకవేళ (*s == ',' || *s == NUL)
	    errmsg = e_zero_నిడివి_string;
	ఒకవేళ (errmsg != ఖాళీ)
	    ఆపు;
	అయితే (*s && *s != ',')
	{
	    ఒకవేళ (*s == '\\' && s[1] != NUL)
		++s;
	    ++s;
	}
	s = skip_గమ్యం_option_part(s);
    }

    తిరిగి errmsg;
}

/*
 * The 'commentstring' option is changed.
 */
    అక్షరం *
did_సమితి_commentstring(optset_T *వాదనలు)
{
    char_u	**varp = (char_u **)వాదనలు->os_varp;

    ఒకవేళ (**varp != NUL && అక్షరశ్రేణి_శోధన((అక్షరం *)*varp, "%s") == ఖాళీ)
	తిరిగి e_commentstring_must_be_ఖాళీలు_or_contain_str;

    తిరిగి ఖాళీ;
}

/*
 * Check if value for 'complete' is valid when 'complete' option is changed.
 */
    అక్షరం *
did_సమితి_complete(optset_T *వాదనలు)
{
    char_u	**varp = (char_u **)వాదనలు->os_varp;
    char_u	*p, *t;
    char_u	బఫరు[LSIZE];
    char_u	*buf_ptr;
    char_u	char_before = NUL;
    పూర్ణాంకం		escape;

    కొరకు (p = *varp; *p; )
    {
	vim_memset(బఫరు, 0, LSIZE);
	buf_ptr = బఫరు;
	escape = 0;

	// Extract substring while handling escaped commas
	అయితే (*p && (*p != ',' || escape) && buf_ptr < (బఫరు + LSIZE - 1))
	{
	    ఒకవేళ (*p == '\\' && *(p + 1) == ',')
	    {
		escape = 1;  // Mark escape mode
		p++;         // Skip '\'
	    }
	    లేకపోతే
	    {
		escape = 0;
		*buf_ptr++ = *p;
	    }
	    p++;
	}
	*buf_ptr = NUL;

	ఒకవేళ (vim_strchr((char_u *)".wbuksid]tUFo", *బఫరు) == ఖాళీ)
	    తిరిగి illegal_char(వాదనలు->os_errbuf, వాదనలు->os_errbuflen, *బఫరు);

	ఒకవేళ (vim_strchr((char_u *)"ksF", *బఫరు) == ఖాళీ && *(బఫరు + 1) != NUL
		&& *(బఫరు + 1) != '^')
	    char_before = *బఫరు;
	లేకపోతే
	{
	    // Test for a number after '^'
	    ఒకవేళ ((t = vim_strchr(బఫరు, '^')) != ఖాళీ)
	    {
		*t++ = NUL;
		ఒకవేళ (!*t)
		    char_before = '^';
		లేకపోతే
		{
		    కొరకు (; *t; t++)
		    {
			ఒకవేళ (!vim_isdigit(*t))
			{
			    char_before = '^';
			    ఆపు;
			}
		    }
		}
	    }
	}
	ఒకవేళ (char_before != NUL)
	    తిరిగి illegal_char_after_chr(వాదనలు->os_errbuf, వాదనలు->os_errbuflen,
		    char_before);
	// Skip comma and spaces
	అయితే (*p == ',' || *p == ' ')
	    p++;
    }

#నిర్వచితమైతే FEAT_COMPL_FUNC
    ఒకవేళ (సమితి_cpt_callbacks(వాదనలు) != OK)
	తిరిగి illegal_char_after_chr(వాదనలు->os_errbuf, వాదనలు->os_errbuflen,
		'F');
#అయితే_ముగింపు
    తిరిగి ఖాళీ;
}

    పూర్ణాంకం
expand_సమితి_complete(optexpand_T *వాదనలు, పూర్ణాంకం *అంకె_Matches, char_u ***matches)
{
    స్థిర అక్షరం *(p_cpt_విలువలు[]) = {
	".", "w", "b", "u", "k", "kspell", "s", "i", "d", "]", "t", "U", "F", "o",
	ఖాళీ};
    తిరిగి expand_సమితి_opt_string(
	    వాదనలు,
	    p_cpt_విలువలు,
	    ARRAY_నిడివి(p_cpt_విలువలు) - 1,
	    అంకె_Matches,
	    matches);
}

/*
 * The 'completeopt' option is changed.
 */
    అక్షరం *
did_సమితి_completeopt(optset_T *వాదనలు వాడని)
{
    char_u	*cot = p_cot;
    గుర్తులేని	*flags = &cot_flags;

    ఒకవేళ (వాదనలు->os_flags & OPT_LOCAL)
    {
	cot = curbuf->b_p_cot;
	flags = &curbuf->b_cot_flags;
    }
    లేకపోతే ఒకవేళ (!(వాదనలు->os_flags & OPT_GLOBAL))
	// When using :set, clear the local flags.
	curbuf->b_cot_flags = 0;

    ఒకవేళ (opt_అక్షరశ్రేణులు_flags(cot, p_cot_విలువలు, flags, TRUE) != OK)
	తిరిగి e_invalid_వాదం;

    తిరిగి ఖాళీ;
}

    పూర్ణాంకం
expand_సమితి_completeopt(optexpand_T *వాదనలు, పూర్ణాంకం *అంకె_Matches, char_u ***matches)
{
    తిరిగి expand_సమితి_opt_string(
	    వాదనలు,
	    p_cot_విలువలు,
	    ARRAY_నిడివి(p_cot_విలువలు) - 1,
	    అంకె_Matches,
	    matches);
}

/*
 * The 'completefuzzycollect' option is changed.
 */
    అక్షరం *
did_సమితి_completefuzzycollect(optset_T *వాదనలు వాడని)
{
    ఒకవేళ (opt_అక్షరశ్రేణులు_flags(p_cfc, p_cfc_విలువలు, &cfc_flags, TRUE) != OK)
	తిరిగి e_invalid_వాదం;
    తిరిగి ఖాళీ;
}

    పూర్ణాంకం
expand_సమితి_completefuzzycollect(
	optexpand_T *వాదనలు,
	పూర్ణాంకం *అంకె_Matches,
	char_u ***matches)
{
    తిరిగి expand_సమితి_opt_string(
	    వాదనలు,
	    p_cfc_విలువలు,
	    ARRAY_నిడివి(p_cfc_విలువలు) - 1,
	    అంకె_Matches,
	    matches);
}

/*
 * The 'completeitemalign' option is changed.
 */
    అక్షరం *
did_సమితి_completeitemalign(optset_T *వాదనలు వాడని)
{
    char_u	*p = p_cia;
    గుర్తులేని	new_cia_flags = 0;
    పూర్ణాంకం		చూసిన[3] = { FALSE, FALSE, FALSE };
    పూర్ణాంకం		లెక్క = 0;
    char_u	buf[10];

    అయితే (*p)
    {
	copy_option_part(&p, buf, పరిమాణం(buf), ",");
	ఒకవేళ (లెక్క >= 3)
	    తిరిగి e_invalid_వాదం;

	ఒకవేళ (STRCMP(buf, "abbr") == 0)
	{
	    ఒకవేళ (చూసిన[CPT_ABBR])
		తిరిగి e_invalid_వాదం;
	    new_cia_flags = new_cia_flags * 10 + CPT_ABBR;
	    చూసిన[CPT_ABBR] = TRUE;
	    లెక్క++;
	}
	లేకపోతే ఒకవేళ (STRCMP(buf, "kind") == 0)
	{
	    ఒకవేళ (చూసిన[CPT_రకం])
		తిరిగి e_invalid_వాదం;
	    new_cia_flags = new_cia_flags * 10 + CPT_రకం;
	    చూసిన[CPT_రకం] = TRUE;
	    లెక్క++;
	}
	లేకపోతే ఒకవేళ (STRCMP(buf, "menu") == 0)
	{
	    ఒకవేళ (చూసిన[CPT_MENU])
		తిరిగి e_invalid_వాదం;
	    new_cia_flags = new_cia_flags * 10 + CPT_MENU;
	    చూసిన[CPT_MENU] = TRUE;
	    లెక్క++;
	}
	లేకపోతే
	    తిరిగి e_invalid_వాదం;
    }
    ఒకవేళ (new_cia_flags == 0 || లెక్క != 3)
	తిరిగి e_invalid_వాదం;

    cia_flags = new_cia_flags;
    తిరిగి ఖాళీ;
}

#ఒకవేళ defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
/*
 * The 'completepopup' option is changed.
 */
    అక్షరం *
did_సమితి_completepopup(optset_T *వాదనలు వాడని)
{
    ఒకవేళ (parse_completepopup(ఖాళీ) == FAIL)
	తిరిగి e_invalid_వాదం;

    popup_close_info();
    తిరిగి ఖాళీ;
}
#అయితే_ముగింపు

#ఒకవేళ defined(BACKSLASH_లోపల_FILENAME)
/*
 * The 'completeslash' option is changed.
 */
    అక్షరం *
did_సమితి_completeslash(optset_T *వాదనలు వాడని)
{
    ఒకవేళ (check_opt_అక్షరశ్రేణులు(p_csl, p_csl_విలువలు, FALSE) != OK
	    || check_opt_అక్షరశ్రేణులు(curbuf->b_p_csl, p_csl_విలువలు, FALSE) != OK)
	తిరిగి e_invalid_వాదం;

    తిరిగి ఖాళీ;
}

    పూర్ణాంకం
expand_సమితి_completeslash(optexpand_T *వాదనలు, పూర్ణాంకం *అంకె_Matches, char_u ***matches)
{
    తిరిగి expand_సమితి_opt_string(
	    వాదనలు,
	    p_csl_విలువలు,
	    ARRAY_నిడివి(p_csl_విలువలు) - 1,
	    అంకె_Matches,
	    matches);
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_CONCEAL)
/*
 * The 'concealcursor' option is changed.
 */
    అక్షరం *
did_సమితి_concealcursor(optset_T *వాదనలు)
{
    char_u	**varp = (char_u **)వాదనలు->os_varp;

    తిరిగి did_సమితి_option_listflag(*varp, (char_u *)COCU_ALL, వాదనలు->os_errbuf,
		    వాదనలు->os_errbuflen);
}

    పూర్ణాంకం
expand_సమితి_concealcursor(optexpand_T *వాదనలు, పూర్ణాంకం *అంకె_Matches, char_u ***matches)
{
    తిరిగి expand_సమితి_opt_listflag(వాదనలు, (char_u*)COCU_ALL, అంకె_Matches, matches);
}
#అయితే_ముగింపు

/*
 * The 'cpoptions' option is changed.
 */
    అక్షరం *
did_సమితి_cpoptions(optset_T *వాదనలు)
{
    char_u	**varp = (char_u **)వాదనలు->os_varp;

    తిరిగి did_సమితి_option_listflag(*varp, (char_u *)CPO_ALL, వాదనలు->os_errbuf,
		    వాదనలు->os_errbuflen);
}

    పూర్ణాంకం
expand_సమితి_cpoptions(optexpand_T *వాదనలు, పూర్ణాంకం *అంకె_Matches, char_u ***matches)
{
    తిరిగి expand_సమితి_opt_listflag(వాదనలు, (char_u*)CPO_ALL, అంకె_Matches, matches);
}

#ఒకవేళ defined(FEAT_CRYPT)
/*
 * The 'cryptkey' option is changed.
 */
    అక్షరం *
did_సమితి_cryptkey(optset_T *వాదనలు)
{
    // Make sure the ":set" command doesn't show the new value in the
    // history.
    remove_కీలకం_from_history();

    ఒకవేళ (వాదనలు->os_op != OP_NONE)
	// Don't allow set+=/-=/^= as they can allow for substring guessing
	తిరిగి e_invalid_వాదం;

    ఒకవేళ (STRCMP(curbuf->b_p_కీలకం, వాదనలు->os_oldval.string) != 0)
    {
	// Need to update the swapfile.
	ml_సమితి_crypt_కీలకం(curbuf, వాదనలు->os_oldval.string,
		*curbuf->b_p_cm == NUL ? p_cm : curbuf->b_p_cm);
	మారింది_అంతర్గత();
    }
# ifdef FEAT_SODIUM
    ఒకవేళ (crypt_పద్ధతి_is_sodium(crypt_get_పద్ధతి_nr(curbuf)))
	crypt_sodium_తాళం_కీలకం(వాదనలు->os_newval.string);
# endif

    తిరిగి ఖాళీ;
}

/*
 * The 'cryptmethod' option is changed.
 */
    అక్షరం *
did_సమితి_cryptmethod(optset_T *వాదనలు)
{
    char_u  *p;
    char_u  *s;

    ఒకవేళ (వాదనలు->os_flags & OPT_LOCAL)
	p = curbuf->b_p_cm;
    లేకపోతే
	p = p_cm;
    ఒకవేళ (check_opt_అక్షరశ్రేణులు(p, p_cm_విలువలు, TRUE) != OK)
	తిరిగి e_invalid_వాదం;
    లేకపోతే ఒకవేళ (crypt_self_test() == FAIL)
	తిరిగి e_invalid_వాదం;

    // When setting the global value to empty, make it "zip".
    ఒకవేళ (*p_cm == NUL)
    {
	free_string_option(p_cm);
	p_cm = vim_strsave((char_u *)"zip");
    }
    // When using ":set cm=name" the local value is going to be empty.
    // Do that here, otherwise the crypt functions will still use the
    // local value.
    ఒకవేళ ((వాదనలు->os_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	free_string_option(curbuf->b_p_cm);
	curbuf->b_p_cm = ఖాళీలు_option;
    }

    // Need to update the swapfile when the effective method changed.
    // Set "s" to the effective old value, "p" to the effective new
    // method and compare.
    ఒకవేళ ((వాదనలు->os_flags & OPT_LOCAL) && *వాదనలు->os_oldval.string == NUL)
	s = p_cm;  // was previously using the global value
    లేకపోతే
	s = వాదనలు->os_oldval.string;
    ఒకవేళ (*curbuf->b_p_cm == NUL)
	p = p_cm;  // is now using the global value
    లేకపోతే
	p = curbuf->b_p_cm;
    ఒకవేళ (STRCMP(s, p) != 0)
	ml_సమితి_crypt_కీలకం(curbuf, curbuf->b_p_కీలకం, s);

    // If the global value changes need to update the swapfile for all
    // buffers using that value.
    ఒకవేళ ((వాదనలు->os_flags & OPT_GLOBAL)
	    && STRCMP(p_cm, వాదనలు->os_oldval.string) != 0)
    {
	buf_T	*buf;

	FOR_ALL_BUFFERS(buf)
	    ఒకవేళ (buf != curbuf && *buf->b_p_cm == NUL)
		ml_సమితి_crypt_కీలకం(buf, buf->b_p_కీలకం, వాదనలు->os_oldval.string);
    }
    తిరిగి ఖాళీ;
}

    పూర్ణాంకం
expand_సమితి_cryptmethod(optexpand_T *వాదనలు, పూర్ణాంకం *అంకె_Matches, char_u ***matches)
{
    తిరిగి expand_సమితి_opt_string(
	    వాదనలు,
	    p_cm_విలువలు,
	    ARRAY_నిడివి(p_cm_విలువలు) - 1,
	    అంకె_Matches,
	    matches);
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_CSCOPE) && defined(FEAT_QUICKFIX)
/*
 * The 'cscopequickfix' option is changed.
 */
    అక్షరం *
did_సమితి_cscopequickfix(optset_T *వాదనలు వాడని)
{
    char_u	*p;

    ఒకవేళ (p_csqf == ఖాళీ)
	తిరిగి ఖాళీ;

    p = p_csqf;
    అయితే (*p != NUL)
    {
	ఒకవేళ (vim_strchr((char_u *)CSQF_CMDS, *p) == ఖాళీ
		|| p[1] == NUL
		|| vim_strchr((char_u *)CSQF_FLAGS, p[1]) == ఖాళీ
		|| (p[2] != NUL && p[2] != ','))
	    తిరిగి e_invalid_వాదం;
	లేకపోతే ఒకవేళ (p[2] == NUL)
	    ఆపు;
	లేకపోతే
	    p += 3;
    }

    తిరిగి ఖాళీ;
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_SYN_HL)
/*
 * The 'cursorlineopt' option is changed.
 */
    అక్షరం *
did_సమితి_cursorlineopt(optset_T *వాదనలు)
{
    char_u	**varp = (char_u **)వాదనలు->os_varp;

    // This could be changed to use opt_strings_flags() instead.
    ఒకవేళ (**varp == NUL || fill_culopt_flags(*varp, curwin) != OK)
	తిరిగి e_invalid_వాదం;

    తిరిగి ఖాళీ;
}

    పూర్ణాంకం
expand_సమితి_cursorlineopt(optexpand_T *వాదనలు, పూర్ణాంకం *అంకె_Matches, char_u ***matches)
{
    తిరిగి expand_సమితి_opt_string(
	    వాదనలు,
	    p_culopt_విలువలు,
	    ARRAY_నిడివి(p_culopt_విలువలు) - 1,
	    అంకె_Matches,
	    matches);
}
#అయితే_ముగింపు

/*
 * The 'debug' option is changed.
 */
    అక్షరం *
did_సమితి_debug(optset_T *వాదనలు వాడని)
{
    తిరిగి did_సమితి_opt_అక్షరశ్రేణులు(p_debug, p_debug_విలువలు, TRUE);
}

    పూర్ణాంకం
expand_సమితి_debug(optexpand_T *వాదనలు, పూర్ణాంకం *అంకె_Matches, char_u ***matches)
{
    తిరిగి expand_సమితి_opt_string(
	    వాదనలు,
	    p_debug_విలువలు,
	    ARRAY_నిడివి(p_debug_విలువలు) - 1,
	    అంకె_Matches,
	    matches);
}

#ఒకవేళ defined(FEAT_DIFF)
/*
 * The 'diffanchors' option is changed.
 */
    అక్షరం *
did_సమితి_diffanchors(optset_T *వాదనలు)
{
    ఒకవేళ (diffanchors_మారింది(వాదనలు->os_flags & OPT_LOCAL) == FAIL)
	తిరిగి e_invalid_వాదం;

    తిరిగి ఖాళీ;
}

/*
 * The 'diffopt' option is changed.
 */
    అక్షరం *
did_సమితి_diffopt(optset_T *వాదనలు వాడని)
{
    ఒకవేళ (diffopt_మారింది() == FAIL)
	తిరిగి e_invalid_వాదం;

    తిరిగి ఖాళీ;
}

    పూర్ణాంకం
expand_సమితి_diffopt(optexpand_T *వాదనలు, పూర్ణాంకం *అంకె_Matches, char_u ***matches)
{
    expand_T *xp = వాదనలు->oe_xp;

    ఒకవేళ (xp->xp_నమూనా > వాదనలు->oe_సమితి_arg && *(xp->xp_నమూనా-1) == ':')
    {
	ఒకవేళ (completing_విలువ_for_subopt(వాదనలు, "algorithm"))
	{
	    తిరిగి expand_సమితి_opt_string(
		    వాదనలు,
		    p_dip_algorithm_విలువలు,
		    ARRAY_నిడివి(p_dip_algorithm_విలువలు) - 1,
		    అంకె_Matches,
		    matches);
	}
	ఒకవేళ (completing_విలువ_for_subopt(వాదనలు, "inline"))
	{
	    తిరిగి expand_సమితి_opt_string(
		    వాదనలు,
		    p_dip_inline_విలువలు,
		    ARRAY_నిడివి(p_dip_inline_విలువలు) - 1,
		    అంకె_Matches,
		    matches);
	}
	తిరిగి FAIL;
    }

    తిరిగి expand_సమితి_opt_string(
	    వాదనలు,
	    p_dip_విలువలు,
	    ARRAY_నిడివి(p_dip_విలువలు) - 1,
	    అంకె_Matches,
	    matches);
}
#అయితే_ముగింపు

/*
 * The 'display' option is changed.
 */
    అక్షరం *
did_సమితి_display(optset_T *వాదనలు వాడని)
{
    ఒకవేళ (opt_అక్షరశ్రేణులు_flags(p_dy, p_dy_విలువలు, &dy_flags, TRUE) != OK)
	తిరిగి e_invalid_వాదం;

    (శూన్య)init_chartab();
    తిరిగి ఖాళీ;
}

    పూర్ణాంకం
expand_సమితి_display(optexpand_T *వాదనలు, పూర్ణాంకం *అంకె_Matches, char_u ***matches)
{
    తిరిగి expand_సమితి_opt_string(
	    వాదనలు,
	    p_dy_విలువలు,
	    ARRAY_నిడివి(p_dy_విలువలు) - 1,
	    అంకె_Matches,
	    matches);
}

/*
 * The 'eadirection' option is changed.
 */
    అక్షరం *
did_సమితి_eadirection(optset_T *వాదనలు వాడని)
{
    తిరిగి did_సమితి_opt_అక్షరశ్రేణులు(p_ead, p_ead_విలువలు, FALSE);
}

    పూర్ణాంకం
expand_సమితి_eadirection(optexpand_T *వాదనలు, పూర్ణాంకం *అంకె_Matches, char_u ***matches)
{
    తిరిగి expand_సమితి_opt_string(
	    వాదనలు,
	    p_ead_విలువలు,
	    ARRAY_నిడివి(p_ead_విలువలు) - 1,
	    అంకె_Matches,
	    matches);
}

/*
 * One of the 'encoding', 'fileencoding', 'termencoding' or 'makeencoding'
 * options is changed.
 */
    అక్షరం *
did_సమితి_encoding(optset_T *వాదనలు)
{
    char_u	**varp = (char_u **)వాదనలు->os_varp;
    char_u	**gvarp;
    అక్షరం	*errmsg = ఖాళీ;
    char_u	*p;

    // Get the global option to compare with, otherwise we would have to check
    // two values for all local options.
    gvarp = (char_u **)get_option_varp_scope(వాదనలు->os_idx, OPT_GLOBAL);

    ఒకవేళ (gvarp == &p_fenc)
    {
	ఒకవేళ (!curbuf->b_p_ma && వాదనలు->os_flags != OPT_GLOBAL)
	    errmsg = e_cannot_make_changes_modifiable_is_off;
	లేకపోతే ఒకవేళ (vim_strchr(*varp, ',') != ఖాళీ)
	    // No comma allowed in 'fileencoding'; catches confusing it
	    // with 'fileencodings'.
	    errmsg = e_invalid_వాదం;
	లేకపోతే
	{
	    // May show a "+" in the title now.
	    redraw_titles();
	    // Add 'fileencoding' to the swap file.
	    ml_setflags(curbuf);
	}
    }
    ఒకవేళ (errmsg == ఖాళీ)
    {
	// canonize the value, so that STRCMP() can be used on it
	p = enc_canonize(*varp);
	ఒకవేళ (p != ఖాళీ)
	{
	    vim_free(*varp);
	    *varp = p;
	}
	ఒకవేళ (varp == &p_enc)
	{
	    errmsg = mb_init();
	    redraw_titles();
	}
    }

#ఒకవేళ defined(FEAT_GUI_GTK)
    ఒకవేళ (errmsg == ఖాళీ && varp == &p_tenc && gui.లోపల_use)
    {
	// GTK uses only a single encoding, and that is UTF-8.
	ఒకవేళ (STRCMP(p_tenc, "utf-8") != 0)
	    errmsg = e_cannot_be_మారింది_లోపల_gtk_GUI;
    }
#అయితే_ముగింపు

    ఒకవేళ (errmsg == ఖాళీ)
    {
#నిర్వచితమైతే FEAT_KEYMAP
	// When 'keymap' is used and 'encoding' changes, reload the keymap
	// (with another encoding).
	ఒకవేళ (varp == &p_enc && *curbuf->b_p_keymap != NUL)
	    (శూన్య)keymap_init();
#అయితే_ముగింపు

	// When 'termencoding' is not empty and 'encoding' changes or when
	// 'termencoding' changes, need to setup for keyboard input and
	// display output conversion.
	ఒకవేళ (((varp == &p_enc && *p_tenc != NUL) || varp == &p_tenc))
	{
	    ఒకవేళ (convert_అమరిక(&ఆదానం_conv, p_tenc, p_enc) == FAIL
		    || convert_అమరిక(&output_conv, p_enc, p_tenc) == FAIL)
	    {
		semsg(_(e_cannot_convert_between_str_and_str),
			p_tenc, p_enc);
		errmsg = e_invalid_వాదం;
	    }
	}

#ఒకవేళ defined(MSWIN)
	// $HOME, $VIM and $VIMRUNTIME may have characters in active code page.
	ఒకవేళ (varp == &p_enc)
	{
	    init_homedir();
	    init_vimdir();
	}
#అయితే_ముగింపు
    }

    తిరిగి errmsg;
}

    పూర్ణాంకం
expand_సమితి_encoding(optexpand_T *వాదనలు, పూర్ణాంకం *అంకె_Matches, char_u ***matches)
{
    తిరిగి expand_సమితి_opt_generic(
	    వాదనలు,
	    get_encoding_name,
	    అంకె_Matches,
	    matches);
}

/*
 * The 'eventignore(win)' option is changed.
 */
    అక్షరం *
did_సమితి_eventignore(optset_T *వాదనలు)
{
    char_u	**varp = (char_u **)వాదనలు->os_varp;
    char_u	*oldval = వాదనలు->os_oldval.string;

    ఒకవేళ (check_ei(*varp) == FAIL)
	తిరిగి e_invalid_వాదం;

    ఒకవేళ (oldval == ఖాళీ || STRCMP(oldval, *varp) == 0)
	తిరిగి ఖాళీ;

    // Deal with the events that are triggered by comparing against a stored
    // state, with the old value in effect: what happened while an event was
    // ignored must not be reported once it is not ignored anymore, and what
    // happened before must still be reported.
    // Use a copy, the caller owns "oldval" and autocommands may free it.
    char_u	*save_ei = vim_strsave(oldval);
    ఒకవేళ (save_ei != ఖాళీ)
    {
	char_u	*newval = *varp;
	win_T	*wp = is_కిటికీ_local_option(వాదనలు->os_idx) ? curwin : ఖాళీ;

	*varp = save_ei;
	// "varp" points into "wp" for 'eventignorewin'.
	ఒకవేళ (wp != ఖాళీ)
	    ++wp->w_locked;
	may_trigger_deferred_events();
	ఒకవేళ (wp != ఖాళీ)
	    --wp->w_locked;

	free_string_option(*varp);
	*varp = newval;
    }

    తిరిగి ఖాళీ;
}

స్థిర పూర్ణాంకం expand_eiw = FALSE;

    స్థిర char_u *
get_eventignore_name(expand_T *xp, పూర్ణాంకం idx)
{
    పూర్ణాంకం వ్యవకలనం = *xp->xp_నమూనా == '-';
    // 'eventignore(win)' allows special keyword "all" in addition to
    // all event names.
    ఒకవేళ (!వ్యవకలనం && idx == 0)
	తిరిగి (char_u *)"all";

    char_u *name = get_event_name_no_సమూహం(xp, idx - 1 + వ్యవకలనం, expand_eiw);
    ఒకవేళ (name == ఖాళీ)
	తిరిగి ఖాళీ;

    sprintf((అక్షరం *)IObuff, "%s%s", వ్యవకలనం ? "-" : "", name);
    తిరిగి IObuff;
}

    పూర్ణాంకం
expand_సమితి_eventignore(optexpand_T *వాదనలు, పూర్ణాంకం *అంకె_Matches, char_u ***matches)
{
    expand_eiw = వాదనలు->oe_varp != (char_u *)&p_ei;
    తిరిగి expand_సమితి_opt_generic(
	    వాదనలు,
	    get_eventignore_name,
	    అంకె_Matches,
	    matches);
}

/*
 * The 'fileformat' option is changed.
 */
    అక్షరం *
did_సమితి_fileformat(optset_T *వాదనలు)
{
    char_u	**varp = (char_u **)వాదనలు->os_varp;

    ఒకవేళ (!curbuf->b_p_ma && !(వాదనలు->os_flags & OPT_GLOBAL))
	తిరిగి e_cannot_make_changes_modifiable_is_off;
    లేకపోతే ఒకవేళ (check_opt_అక్షరశ్రేణులు(*varp, p_ff_విలువలు, FALSE) != OK)
	తిరిగి e_invalid_వాదం;

    // may also change 'textmode'
    ఒకవేళ (get_fileformat(curbuf) == EOL_DOS)
	curbuf->b_p_tx = TRUE;
    లేకపోతే
	curbuf->b_p_tx = FALSE;
    redraw_titles();
    // update flag in swap file
    ml_setflags(curbuf);
    // Redraw needed when switching to/from "mac": a CR in the text
    // will be displayed differently.
    ఒకవేళ (get_fileformat(curbuf) == EOL_MAC || *వాదనలు->os_oldval.string == 'm')
	redraw_curbuf_later(UPD_NOT_VALID);

    తిరిగి ఖాళీ;
}

    పూర్ణాంకం
expand_సమితి_fileformat(optexpand_T *వాదనలు, పూర్ణాంకం *అంకె_Matches, char_u ***matches)
{
    తిరిగి expand_సమితి_opt_string(
	    వాదనలు,
	    p_ff_విలువలు,
	    ARRAY_నిడివి(p_ff_విలువలు) - 1,
	    అంకె_Matches,
	    matches);
}

/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * fileformat options.
 */
    char_u *
get_fileformat_name(expand_T *xp వాడని, పూర్ణాంకం idx)
{
    ఒకవేళ (idx >= (పూర్ణాంకం)ARRAY_నిడివి(p_ff_విలువలు))
	తిరిగి ఖాళీ;

    తిరిగి (char_u*)p_ff_విలువలు[idx];
}

/*
 * The 'fileformats' option is changed.
 */
    అక్షరం *
did_సమితి_fileformats(optset_T *వాదనలు వాడని)
{
    ఒకవేళ (check_opt_అక్షరశ్రేణులు(p_ffs, p_ff_విలువలు, TRUE) != OK)
	తిరిగి e_invalid_వాదం;

    // also change 'textauto'
    ఒకవేళ (*p_ffs == NUL)
	p_ta = FALSE;
    లేకపోతే
	p_ta = TRUE;

    తిరిగి ఖాళీ;
}

/*
 * The 'filetype' or the 'syntax' option is changed.
 */
    అక్షరం *
did_సమితి_filetype_or_syntax(optset_T *వాదనలు)
{
    char_u	**varp = (char_u **)వాదనలు->os_varp;

    ఒకవేళ (!valid_filetype(*varp))
	తిరిగి e_invalid_వాదం;

    వాదనలు->os_విలువ_మారింది = STRCMP(వాదనలు->os_oldval.string, *varp) != 0;

    // Since we check the value, there is no need to set P_INSECURE,
    // even when the value comes from a modeline.
    వాదనలు->os_విలువ_checked = TRUE;

    తిరిగి ఖాళీ;
}

#ఒకవేళ defined(FEAT_FOLDING)
/*
 * The 'foldclose' option is changed.
 */
    అక్షరం *
did_సమితి_foldclose(optset_T *వాదనలు వాడని)
{
    తిరిగి did_సమితి_opt_అక్షరశ్రేణులు(p_fcl, p_fcl_విలువలు, TRUE);
}

    పూర్ణాంకం
expand_సమితి_foldclose(optexpand_T *వాదనలు, పూర్ణాంకం *అంకె_Matches, char_u ***matches)
{
    తిరిగి expand_సమితి_opt_string(
	    వాదనలు,
	    p_fcl_విలువలు,
	    ARRAY_నిడివి(p_fcl_విలువలు) - 1,
	    అంకె_Matches,
	    matches);
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_EVAL) && defined(FEAT_FOLDING)
/*
 * The 'foldexpr' option is changed.
 */
    అక్షరం *
did_సమితి_foldexpr(optset_T *వాదనలు)
{
    (శూన్య)did_సమితి_optexpr(వాదనలు);
    ఒకవేళ (foldmethod_Is_సమాసం(curwin))
	foldUpdateAll(curwin);
    తిరిగి ఖాళీ;
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_FOLDING)
/*
 * The 'foldignore' option is changed.
 */
    అక్షరం *
did_సమితి_foldignore(optset_T *వాదనలు వాడని)
{
    ఒకవేళ (foldmethodIsIndent(curwin))
	foldUpdateAll(curwin);
    తిరిగి ఖాళీ;
}

/*
 * The 'foldmarker' option is changed.
 */
    అక్షరం *
did_సమితి_foldmarker(optset_T *వాదనలు)
{
    char_u	**varp = (char_u **)వాదనలు->os_varp;
    char_u	*p;

    p = vim_strchr(*varp, ',');
    ఒకవేళ (p == ఖాళీ)
	తిరిగి e_comma_required;
    లేకపోతే ఒకవేళ (p == *varp || p[1] == NUL)
	తిరిగి e_invalid_వాదం;
    లేకపోతే ఒకవేళ (foldmethodIsMarker(curwin))
	foldUpdateAll(curwin);

    తిరిగి ఖాళీ;
}

/*
 * The 'foldmethod' option is changed.
 */
    అక్షరం *
did_సమితి_foldmethod(optset_T *వాదనలు)
{
    char_u	**varp = (char_u **)వాదనలు->os_varp;

    ఒకవేళ (check_opt_అక్షరశ్రేణులు(*varp, p_fdm_విలువలు, FALSE) != OK || **varp == NUL)
	తిరిగి e_invalid_వాదం;

    foldUpdateAll(curwin);
    ఒకవేళ (foldmethodIsDiff(curwin))
	newFoldLevel();
    తిరిగి ఖాళీ;
}

    పూర్ణాంకం
expand_సమితి_foldmethod(optexpand_T *వాదనలు, పూర్ణాంకం *అంకె_Matches, char_u ***matches)
{
    తిరిగి expand_సమితి_opt_string(
	    వాదనలు,
	    p_fdm_విలువలు,
	    ARRAY_నిడివి(p_fdm_విలువలు) - 1,
	    అంకె_Matches,
	    matches);
}

/*
 * The 'foldopen' option is changed.
 */
    అక్షరం *
did_సమితి_foldopen(optset_T *వాదనలు వాడని)
{
    తిరిగి did_సమితి_opt_flags(p_fdo, p_fdo_విలువలు, &fdo_flags, TRUE);
}

    పూర్ణాంకం
expand_సమితి_foldopen(optexpand_T *వాదనలు, పూర్ణాంకం *అంకె_Matches, char_u ***matches)
{
    తిరిగి expand_సమితి_opt_string(
	    వాదనలు,
	    p_fdo_విలువలు,
	    ARRAY_నిడివి(p_fdo_విలువలు) - 1,
	    అంకె_Matches,
	    matches);
}
#అయితే_ముగింపు

/*
 * The 'formatoptions' option is changed.
 */
    అక్షరం *
did_సమితి_formatoptions(optset_T *వాదనలు)
{
    char_u	**varp = (char_u **)వాదనలు->os_varp;

    తిరిగి did_సమితి_option_listflag(*varp, (char_u *)FO_ALL, వాదనలు->os_errbuf,
		    వాదనలు->os_errbuflen);
}

    పూర్ణాంకం
expand_సమితి_formatoptions(optexpand_T *వాదనలు, పూర్ణాంకం *అంకె_Matches, char_u ***matches)
{
    తిరిగి expand_సమితి_opt_listflag(వాదనలు, (char_u*)FO_ALL, అంకె_Matches, matches);
}

#ఒకవేళ defined(CURSOR_SHAPE)
/*
 * The 'guicursor' option is changed.
 */
    అక్షరం *
did_సమితి_guicursor(optset_T *వాదనలు వాడని)
{
    తిరిగి parse_shape_opt(SHAPE_CURSOR);
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_GUI)
/*
 * The 'guifont' option is changed.
 */
    అక్షరం *
did_సమితి_guifont(optset_T *వాదనలు వాడని)
{
    char_u	*p;
    అక్షరం	*errmsg = ఖాళీ;

    ఒకవేళ (gui.లోపల_use)
    {
	p = p_guifont;
# ఒకవేళ defined(FEAT_GUI_GTK)
	// Put up a font dialog and let the user select a new value.
	// If this is cancelled go back to the old value but don't
	// give an error message.
	ఒకవేళ (STRCMP(p, "*") == 0)
	{
	    p = gui_mch_font_dialog(వాదనలు->os_oldval.string);
	    free_string_option(p_guifont);
	    p_guifont = (p != ఖాళీ) ? p : vim_strsave(వాదనలు->os_oldval.string);
	}
# endif
	ఒకవేళ (p != ఖాళీ && gui_init_font(p_guifont, FALSE) != OK)
	{
# ఒకవేళ defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_PHOTON)
	    ఒకవేళ (STRCMP(p_guifont, "*") == 0)
	    {
		// Dialog was cancelled: Keep the old value without giving
		// an error message.
		free_string_option(p_guifont);
		p_guifont = vim_strsave(వాదనలు->os_oldval.string);
	    }
	    లేకపోతే
# endif
		errmsg = e_invalid_fonts;
	}
    }

    తిరిగి errmsg;
}

/*
 * Expand the 'guifont' option. Only when GUI is being used. Each platform has
 * specific behaviors.
 */
    పూర్ణాంకం
expand_సమితి_guifont(optexpand_T *వాదనలు, పూర్ణాంకం *అంకె_Matches, char_u ***matches)
{
    ఒకవేళ (!gui.లోపల_use)
	తిరిగి FAIL;

# ఒకవేళ defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
    char_u **varp = (char_u **)వాదనలు->oe_varp;
    పూర్ణాంకం wide = (varp == &p_guifontwide);

    తిరిగి expand_సమితి_opt_callback(
	    వాదనలు, gui_mch_expand_font, &wide, అంకె_Matches, matches);
# లేకపోతే
    తిరిగి FAIL;
# endif
}

# ఒకవేళ defined(FEAT_XFONTSET)
/*
 * The 'guifontset' option is changed.
 */
    అక్షరం *
did_సమితి_guifontset(optset_T *వాదనలు వాడని)
{
    అక్షరం *errmsg = ఖాళీ;

    ఒకవేళ (STRCMP(p_guifontset, "*") == 0)
	errmsg = e_cant_select_fontset;
    లేకపోతే ఒకవేళ (gui.లోపల_use && gui_init_font(p_guifontset, TRUE) != OK)
	errmsg = e_invalid_fontset;

    తిరిగి errmsg;
}
# endif

/*
 * The 'guifontwide' option is changed.
 */
    అక్షరం *
did_సమితి_guifontwide(optset_T *వాదనలు వాడని)
{
    అక్షరం *errmsg = ఖాళీ;

    ఒకవేళ (STRCMP(p_guifontwide, "*") == 0)
	errmsg = e_cant_select_wide_font;
    లేకపోతే ఒకవేళ (gui_get_wide_font() == FAIL)
	errmsg = e_invalid_wide_font;

    తిరిగి errmsg;
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
/*
 * The 'guiligatures' option is changed.
 */
    అక్షరం *
did_సమితి_guiligatures(optset_T *వాదనలు వాడని)
{
    gui_సమితి_ligatures();
    తిరిగి ఖాళీ;
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_GUI)
/*
 * The 'guioptions' option is changed.
 */
    అక్షరం *
did_సమితి_guioptions(optset_T *వాదనలు)
{
    char_u	**varp = (char_u **)వాదనలు->os_varp;
    అక్షరం *errmsg;

    errmsg = did_సమితి_option_listflag(*varp, (char_u *)GO_ALL, వాదనలు->os_errbuf,
		    వాదనలు->os_errbuflen);
    ఒకవేళ (errmsg != ఖాళీ)
	తిరిగి errmsg;

    gui_init_which_components(వాదనలు->os_oldval.string);
    తిరిగి ఖాళీ;
}

    పూర్ణాంకం
expand_సమితి_guioptions(optexpand_T *వాదనలు, పూర్ణాంకం *అంకె_Matches, char_u ***matches)
{
    తిరిగి expand_సమితి_opt_listflag(వాదనలు, (char_u*)GO_ALL, అంకె_Matches, matches);
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_GUI_TABLINE)
/*
 * The 'guitablabel' option is changed.
 */
    అక్షరం *
did_సమితి_guitablabel(optset_T *వాదనలు వాడని)
{
    redraw_tabline = TRUE;
    తిరిగి ఖాళీ;
}
#అయితే_ముగింపు

/*
 * The 'helpfile' option is changed.
 */
    అక్షరం *
did_సమితి_helpfile(optset_T *వాదనలు వాడని)
{
    // May compute new values for $VIM and $VIMRUNTIME
    ఒకవేళ (didset_vim)
	vim_unsetenv_ext((char_u *)"VIM");
    ఒకవేళ (didset_vimruntime)
	vim_unsetenv_ext((char_u *)"VIMRUNTIME");
    తిరిగి ఖాళీ;
}

#ఒకవేళ defined(FEAT_MULTI_LANG)
/*
 * The 'helplang' option is changed.
 */
    అక్షరం *
did_సమితి_helplang(optset_T *వాదనలు వాడని)
{
    అక్షరం *errmsg = ఖాళీ;

    // Check for "", "ab", "ab,cd", etc.
    కొరకు (char_u *s = p_hlg; *s != NUL; s += 3)
    {
	ఒకవేళ (s[1] == NUL || ((s[2] != ',' || s[3] == NUL) && s[2] != NUL))
	{
	    errmsg = e_invalid_వాదం;
	    ఆపు;
	}
	ఒకవేళ (s[2] == NUL)
	    ఆపు;
    }

    తిరిగి errmsg;
}
#అయితే_ముగింపు

/*
 * The 'highlight' option is changed.
 */
    అక్షరం *
did_సమితి_highlight(optset_T *వాదనలు వాడని)
{
    ఒకవేళ (highlight_మారింది() == FAIL)
	తిరిగి e_invalid_వాదం;	// invalid flags

    తిరిగి ఖాళీ;
}

    స్థిర పూర్ణాంకం
expand_hl_occasions(
	optexpand_T *వాదనలు,
	పూర్ణాంకం *అంకె_Matches,
	char_u ***matches,
	char_u ఉపసర్గ)
{
    char_u	    *p;
    స్థిర char_u   hl_flags[HLF_లెక్క] = HL_FLAGS;
    size_t	    i;
    పూర్ణాంకం		    లెక్క = 0;

    *matches = ALLOC_MULT(char_u *, HLF_లెక్క + 1);
    ఒకవేళ (*matches == ఖాళీ)
	తిరిగి FAIL;

    // We still want to return the full option if it's requested.
    ఒకవేళ (వాదనలు->oe_include_orig_val)
    {
	p = vim_strsave(వాదనలు->oe_opt_విలువ);
	ఒకవేళ (p == ఖాళీ)
	{
	    VIM_CLEAR(*matches);
	    తిరిగి FAIL;
	}
	(*matches)[లెక్క++] = p;
    }

    కొరకు (i = 0; i < HLF_లెక్క; i++)
    {
	p = alloc((ఉపసర్గ == NUL ? 1 : 2) + 1);
	ఒకవేళ (p == ఖాళీ)
	{
	    ఒకవేళ (లెక్క == 0)
	    {
		VIM_CLEAR(*matches);
		తిరిగి FAIL;
	    }
	    లేకపోతే
		ఆపు;
	}
	ఒకవేళ (ఉపసర్గ == NUL)
	    sprintf((అక్షరం *)p, "%c", hl_flags[i]);
	లేకపోతే
	    sprintf((అక్షరం *)p, "%c%c", ఉపసర్గ, hl_flags[i]);
	(*matches)[లెక్క++] = p;
    }

    ఒకవేళ (లెక్క == 0)
    {
	VIM_CLEAR(*matches);
	తిరిగి FAIL;
    }
    *అంకె_Matches = లెక్క;
    తిరిగి OK;
}

/*
 * Expand 'highlight' option.
 */
    పూర్ణాంకం
expand_సమితి_highlight(optexpand_T *వాదనలు, పూర్ణాంకం *అంకె_Matches, char_u ***matches)
{
    char_u	    *p;
    expand_T	    *xp = వాదనలు->oe_xp;
    size_t	    i;
    పూర్ణాంకం		    లెక్క = 0;

    ఒకవేళ (xp->xp_నమూనా > వాదనలు->oe_సమితి_arg && *(xp->xp_నమూనా-1) == ':')
    {
	// Right after a ':', meaning we just return all highlight names.
	తిరిగి expand_సమితి_opt_generic(
		వాదనలు,
		get_highlight_name,
		అంకె_Matches,
		matches);
    }

    ఒకవేళ (*xp->xp_నమూనా == NUL)
	// At beginning of a comma-separated list. Return the specific list of
	// supported occasions.
	తిరిగి expand_hl_occasions(వాదనలు, అంకె_Matches, matches, NUL);

    // We are after the initial character (which indicates the occasion). We
    // already made sure we are not matching after a ':' above, so now we want
    // to match against display mode modifiers.
    // Since the xp_pattern starts from the beginning, we need to include it in
    // the returned match.

    // Note: Keep this in sync with highlight_changed()
    స్థిర char_u p_hl_బహుళకం_విలువలు[] =
	{':', 'b', 'i', '-', 'n', 'r', 's', 'u', 'c', '2', 'd', '=', 't'};
    size_t అంకె_hl_modes = ARRAY_నిడివి(p_hl_బహుళకం_విలువలు);

    *matches = ALLOC_MULT(char_u *, అంకె_hl_modes);
    ఒకవేళ (*matches == ఖాళీ)
	తిరిగి FAIL;

    పూర్ణాంకం నమూనా_len = xp->xp_నమూనా_len;

    కొరకు (i = 0; i < అంకె_hl_modes; i++)
    {
	// Don't allow duplicates as these are unique flags
	char_u *dup = vim_strchr(xp->xp_నమూనా + 1, p_hl_బహుళకం_విలువలు[i]);
	ఒకవేళ (dup != ఖాళీ && (పూర్ణాంకం)(dup - xp->xp_నమూనా) < నమూనా_len)
	    కొనసాగు;

	// ':' only works by itself, not with other flags.
	ఒకవేళ (నమూనా_len > 1 && p_hl_బహుళకం_విలువలు[i] == ':')
	    కొనసాగు;

	p = vim_strnsave(xp->xp_నమూనా, నమూనా_len + 1);
	ఒకవేళ (p == ఖాళీ)
	{
	    ఒకవేళ (i == 0)
	    {
		VIM_CLEAR(*matches);
		తిరిగి FAIL;
	    }
	    లేకపోతే
		ఆపు;
	}
	p[నమూనా_len] = p_hl_బహుళకం_విలువలు[i];
	p[నమూనా_len + 1] = NUL;
	(*matches)[లెక్క++] = p;
    }
    ఒకవేళ (లెక్క == 0)
    {
	VIM_CLEAR(*matches);
	తిరిగి FAIL;
    }
    *అంకె_Matches = లెక్క;

    తిరిగి OK;
}

/*
 * The 'titlestring' or the 'iconstring' option is changed.
 */
    స్థిర అక్షరం *
parse_titleiconstring(optset_T *వాదనలు వాడని, పూర్ణాంకం flagval వాడని)
{
#నిర్వచితమైతే FEAT_STL_OPT
    char_u	**varp = (char_u **)వాదనలు->os_varp;

    // NULL => statusline syntax
    ఒకవేళ (vim_strchr(*varp, '%') && check_stl_option(*varp) == ఖాళీ)
	stl_syntax |= flagval;
    లేకపోతే
	stl_syntax &= ~flagval;
#అయితే_ముగింపు
    did_సమితి_title();

    తిరిగి ఖాళీ;
}

/*
 * The 'iconstring' option is changed.
 */
    అక్షరం *
did_సమితి_iconstring(optset_T *వాదనలు)
{
    పూర్ణాంకం flagval = 0;

#నిర్వచితమైతే FEAT_STL_OPT
    flagval = STL_లోపల_ICON;
#అయితే_ముగింపు

    తిరిగి parse_titleiconstring(వాదనలు, flagval);
}

#ఒకవేళ defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
/*
 * The 'imactivatekey' option is changed.
 */
    అక్షరం *
did_సమితి_imactivatekey(optset_T *వాదనలు వాడని)
{
    ఒకవేళ (!im_xim_isvalid_imactivate())
	తిరిగి e_invalid_వాదం;
    తిరిగి ఖాళీ;
}
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_IMAGE
/*
 * The 'imageprotocol' option is changed.
 */
    అక్షరం *
did_సమితి_imageprotocol(optset_T *వాదనలు వాడని)
{
    ఒకవేళ (update_image_backend() == FAIL)
	తిరిగి e_invalid_వాదం;
    తిరిగి ఖాళీ;
}

    పూర్ణాంకం
expand_సమితి_imageprotocol(optexpand_T *వాదనలు, పూర్ణాంకం *అంకె_Matches, char_u ***matches)
{
    expand_T *xp = వాదనలు->oe_xp;

    ఒకవేళ (xp->xp_నమూనా > వాదనలు->oe_సమితి_arg && *(xp->xp_నమూనా-1) == ':')
    {
	// 'imageprotocol' only has well-defined terms for completion for the
	// protocol part after the colon.
	తిరిగి expand_సమితి_opt_string(
		వాదనలు,
		p_ipc_protocol_విలువలు,
		ARRAY_నిడివి(p_ipc_protocol_విలువలు) - 1,
		అంకె_Matches,
		matches);
    }
    // Use expand_set_opt_string instead of returning FAIL so that we can
    // include the original value if args->oe_include_orig_val is set.
    స్థిర అక్షరం *(ఖాళీలు_2[]) = {ఖాళీ};
    తిరిగి expand_సమితి_opt_string(వాదనలు, ఖాళీలు_2, 0, అంకె_Matches, matches);
}
#అయితే_ముగింపు

/*
 * The 'iskeyword' option is changed.
 */
    అక్షరం *
did_సమితి_iskeyword(optset_T *వాదనలు)
{
    char_u	**varp = (char_u **)వాదనలు->os_varp;

    ఒకవేళ (varp == &p_isk)		// only check for global-value
    {
	ఒకవేళ (check_isopt(*varp) == FAIL)
	    తిరిగి e_invalid_వాదం;
    }
    లేకపోతే			// fallthrough for local-value
	తిరిగి did_సమితి_isopt(వాదనలు);

    తిరిగి ఖాళీ;
}

/*
 * The 'isident' or the 'iskeyword' or the 'isprint' or the 'isfname' option is
 * changed.
 */
    అక్షరం *
did_సమితి_isopt(optset_T *వాదనలు)
{
    // 'isident', 'iskeyword', 'isprint' or 'isfname' option: refill g_chartab[]
    // If the new option is invalid, use old value.
    // 'lisp' option: refill g_chartab[] for '-' char.
    ఒకవేళ (init_chartab() == FAIL)
    {
	వాదనలు->os_restore_chartab = TRUE;// need to restore the chartab.
	తిరిగి e_invalid_వాదం;	// error in value
    }

    తిరిగి ఖాళీ;
}

/*
 * The 'jumpoptions' option is changed.
 */
    అక్షరం *
did_సమితి_jumpoptions(optset_T *వాదనలు వాడని)
{
    ఒకవేళ (opt_అక్షరశ్రేణులు_flags(p_jop, p_jop_విలువలు, &jop_flags, TRUE) != OK)
	తిరిగి e_invalid_వాదం;

    తిరిగి ఖాళీ;
}

    పూర్ణాంకం
expand_సమితి_jumpoptions(optexpand_T *వాదనలు, పూర్ణాంకం *అంకె_Matches, char_u ***matches)
{
    తిరిగి expand_సమితి_opt_string(
	    వాదనలు,
	    p_jop_విలువలు,
	    ARRAY_నిడివి(p_jop_విలువలు) - 1,
	    అంకె_Matches,
	    matches);
}

#ఒకవేళ defined(FEAT_KEYMAP)
/*
 * The 'keymap' option is changed.
 */
    అక్షరం *
did_సమితి_keymap(optset_T *వాదనలు)
{
    char_u	**varp = (char_u **)వాదనలు->os_varp;
    అక్షరం *errmsg = ఖాళీ;

    ఒకవేళ (!valid_filetype(*varp))
	errmsg = e_invalid_వాదం;
    లేకపోతే
    {
	పూర్ణాంకం	    secure_save = secure;

	// Reset the secure flag, since the value of 'keymap' has
	// been checked to be safe.
	secure = 0;

	// load or unload key mapping tables
	errmsg = keymap_init();

	secure = secure_save;

	// Since we check the value, there is no need to set P_INSECURE,
	// even when the value comes from a modeline.
	వాదనలు->os_విలువ_checked = TRUE;
    }

    ఒకవేళ (errmsg == ఖాళీ)
    {
	ఒకవేళ (*curbuf->b_p_keymap != NUL)
	{
	    // Installed a new keymap, switch on using it.
	    curbuf->b_p_iminsert = B_IMODE_LMAP;
	    ఒకవేళ (curbuf->b_p_imsearch != B_IMODE_USE_చొప్పించు)
		curbuf->b_p_imsearch = B_IMODE_LMAP;
	}
	లేకపోతే
	{
	    // Cleared the keymap, may reset 'iminsert' and 'imsearch'.
	    ఒకవేళ (curbuf->b_p_iminsert == B_IMODE_LMAP)
		curbuf->b_p_iminsert = B_IMODE_NONE;
	    ఒకవేళ (curbuf->b_p_imsearch == B_IMODE_LMAP)
		curbuf->b_p_imsearch = B_IMODE_USE_చొప్పించు;
	}
	ఒకవేళ ((వాదనలు->os_flags & OPT_LOCAL) == 0)
	{
	    సమితి_iminsert_global();
	    సమితి_imsearch_global();
	}
	స్థితి_redraw_curbuf();
    }

    తిరిగి errmsg;
}
#అయితే_ముగింపు

/*
 * The 'keymodel' option is changed.
 */
    అక్షరం *
did_సమితి_keymodel(optset_T *వాదనలు వాడని)
{
    ఒకవేళ (check_opt_అక్షరశ్రేణులు(p_km, p_km_విలువలు, TRUE) != OK)
	తిరిగి e_invalid_వాదం;

    km_stopsel = (vim_strchr(p_km, 'o') != ఖాళీ);
    km_startsel = (vim_strchr(p_km, 'a') != ఖాళీ);
    తిరిగి ఖాళీ;
}

    పూర్ణాంకం
expand_సమితి_keymodel(optexpand_T *వాదనలు, పూర్ణాంకం *అంకె_Matches, char_u ***matches)
{
    తిరిగి expand_సమితి_opt_string(
	    వాదనలు,
	    p_km_విలువలు,
	    ARRAY_నిడివి(p_km_విలువలు) - 1,
	    అంకె_Matches,
	    matches);
}

/*
 * The 'keyprotocol' option is changed.
 */
    అక్షరం *
did_సమితి_keyprotocol(optset_T *వాదనలు వాడని)
{
    char_u *పదం = T_NAME;
    keyprot_T kpc = సరిపోలిక_keyprotocol(పదం);
    ఒకవేళ (kpc == KEYPROTOCOL_FAIL)
	తిరిగి e_invalid_వాదం;

    apply_keyprotocol(పదం, kpc);

    తిరిగి ఖాళీ;
}

    పూర్ణాంకం
expand_సమితి_keyprotocol(optexpand_T *వాదనలు, పూర్ణాంకం *అంకె_Matches, char_u ***matches)
{
    expand_T *xp = వాదనలు->oe_xp;
    ఒకవేళ (xp->xp_నమూనా > వాదనలు->oe_సమితి_arg && *(xp->xp_నమూనా-1) == ':')
    {
	// 'keyprotocol' only has well-defined terms for completion for the
	// protocol part after the colon.
	తిరిగి expand_సమితి_opt_string(
		వాదనలు,
		p_kpc_protocol_విలువలు,
		ARRAY_నిడివి(p_kpc_protocol_విలువలు) - 1,
		అంకె_Matches,
		matches);
    }
    // Use expand_set_opt_string instead of returning FAIL so that we can
    // include the original value if args->oe_include_orig_val is set.
    స్థిర అక్షరం *(ఖాళీలు_2[]) = {ఖాళీ};
    తిరిగి expand_సమితి_opt_string(వాదనలు, ఖాళీలు_2, 0, అంకె_Matches, matches);
}

/*
 * The 'lispoptions' option is changed.
 */
    అక్షరం *
did_సమితి_lispoptions(optset_T *వాదనలు)
{
    char_u	**varp = (char_u **)వాదనలు->os_varp;

    ఒకవేళ (**varp != NUL
	    && STRCMP(*varp, "expr:0") != 0
	    && STRCMP(*varp, "expr:1") != 0)
	తిరిగి e_invalid_వాదం;

    తిరిగి ఖాళీ;
}

    పూర్ణాంకం
expand_సమితి_lispoptions(optexpand_T *వాదనలు, పూర్ణాంకం *అంకె_Matches, char_u ***matches)
{
    స్థిర అక్షరం *(p_lop_విలువలు[]) = {"expr:0", "expr:1", ఖాళీ};
    తిరిగి expand_సమితి_opt_string(
	    వాదనలు,
	    p_lop_విలువలు,
	    ARRAY_నిడివి(p_lop_విలువలు) - 1,
	    అంకె_Matches,
	    matches);
}

/*
 * The 'matchpairs' option is changed.
 */
    అక్షరం *
did_సమితి_matchpairs(optset_T *వాదనలు)
{
    char_u	**varp = (char_u **)వాదనలు->os_varp;
    char_u	*p;

    ఒకవేళ (has_mbyte)
    {
	కొరకు (p = *varp; *p != NUL; ++p)
	{
	    పూర్ణాంకం x2 = -1;
	    పూర్ణాంకం x3 = -1;

	    p += mb_ptr2len(p);
	    ఒకవేళ (*p != NUL)
		x2 = *p++;
	    ఒకవేళ (*p != NUL)
	    {
		x3 = mb_ptr2char(p);
		p += mb_ptr2len(p);
	    }
	    ఒకవేళ (x2 != ':' || x3 == -1 || (*p != NUL && *p != ','))
		తిరిగి e_invalid_వాదం;
	    ఒకవేళ (*p == NUL)
		ఆపు;
	}
    }
    లేకపోతే
    {
	// Check for "x:y,x:y"
	కొరకు (p = *varp; *p != NUL; p += 4)
	{
	    ఒకవేళ (p[1] != ':' || p[2] == NUL || (p[3] != NUL && p[3] != ','))
		తిరిగి e_invalid_వాదం;
	    ఒకవేళ (p[3] == NUL)
		ఆపు;
	}
    }

    తిరిగి ఖాళీ;
}

/*
 * Process the updated 'messagesopt' option value.
 */
    అక్షరం *
did_సమితి_messagesopt(optset_T *వాదనలు వాడని)
{
    ఒకవేళ (messagesopt_మారింది() == FAIL)
	తిరిగి e_invalid_వాదం;

    తిరిగి ఖాళీ;
}

    పూర్ణాంకం
expand_సమితి_messagesopt(optexpand_T *వాదనలు, పూర్ణాంకం *అంకె_Matches, char_u ***matches)
{
    స్థిర అక్షరం *(p_mopt_విలువలు[]) = {"hit-enter", "wait:", "history:", ఖాళీ};
    తిరిగి expand_సమితి_opt_string(
	    వాదనలు,
	    p_mopt_విలువలు,
	    ARRAY_నిడివి(p_mopt_విలువలు) - 1,
	    అంకె_Matches,
	    matches);
}

#ఒకవేళ defined(FEAT_SPELL)
/*
 * The 'mkspellmem' option is changed.
 */
    అక్షరం *
did_సమితి_mkspellmem(optset_T *వాదనలు వాడని)
{
    ఒకవేళ (spell_check_msm() != OK)
	తిరిగి e_invalid_వాదం;

    తిరిగి ఖాళీ;
}
#అయితే_ముగింపు

/*
 * The 'mouse' option is changed.
 */
    అక్షరం *
did_సమితి_mouse(optset_T *వాదనలు)
{
    char_u	**varp = (char_u **)వాదనలు->os_varp;
    అక్షరం	*retval;

    retval = did_సమితి_option_listflag(*varp, (char_u *)MOUSE_ALL, వాదనలు->os_errbuf,
		    వాదనలు->os_errbuflen);
    ఒకవేళ (retval == ఖాళీ)
    {
	redraw_tabline = TRUE;
	ఒకవేళ (tabline_ఎత్తు() > 0)
	    update_screen(UPD_VALID);
#ఒకవేళ defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
	popup_close_info(); // Close info popup to apply new properties
#అయితే_ముగింపు
    }
    తిరిగి retval;
}

    పూర్ణాంకం
expand_సమితి_mouse(optexpand_T *వాదనలు, పూర్ణాంకం *అంకె_Matches, char_u ***matches)
{
    తిరిగి expand_సమితి_opt_listflag(వాదనలు, (char_u*)MOUSE_ALL, అంకె_Matches, matches);
}

/*
 * The 'mousemodel' option is changed.
 */
    అక్షరం *
did_సమితి_mousemodel(optset_T *వాదనలు వాడని)
{
    ఒకవేళ (check_opt_అక్షరశ్రేణులు(p_mousem, p_mousem_విలువలు, FALSE) != OK)
	తిరిగి e_invalid_వాదం;
#ఒకవేళ defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU) && (Xm_సంస్కరణ <= 1002)
    లేకపోతే ఒకవేళ (*p_mousem != *వాదనలు->os_oldval.string)
	// Changed from "extend" to "popup" or "popup_setpos" or vv: need
	// to create or delete the popup menus.
	gui_motif_update_mousemodel(మూలం_menu);
#అయితే_ముగింపు

    తిరిగి ఖాళీ;
}

    పూర్ణాంకం
expand_సమితి_mousemodel(optexpand_T *వాదనలు, పూర్ణాంకం *అంకె_Matches, char_u ***matches)
{
    తిరిగి expand_సమితి_opt_string(
	    వాదనలు,
	    p_mousem_విలువలు,
	    ARRAY_నిడివి(p_mousem_విలువలు) - 1,
	    అంకె_Matches,
	    matches);
}

#ఒకవేళ defined(FEAT_MOUSESHAPE)
    అక్షరం *
did_సమితి_mouseshape(optset_T *వాదనలు వాడని)
{
    అక్షరం *errmsg = ఖాళీ;

    errmsg = parse_shape_opt(SHAPE_MOUSE);
    update_mouseshape(-1);

    తిరిగి errmsg;
}
#అయితే_ముగింపు

/*
 * The 'nrformats' option is changed.
 */
    అక్షరం *
did_సమితి_nrformats(optset_T *వాదనలు)
{
    char_u	**varp = (char_u **)వాదనలు->os_varp;

    తిరిగి did_సమితి_opt_అక్షరశ్రేణులు(*varp, p_nf_విలువలు, TRUE);
}

    పూర్ణాంకం
expand_సమితి_nrformats(optexpand_T *వాదనలు, పూర్ణాంకం *అంకె_Matches, char_u ***matches)
{
    తిరిగి expand_సమితి_opt_string(
	    వాదనలు,
	    p_nf_విలువలు,
	    ARRAY_నిడివి(p_nf_విలువలు) - 1,
	    అంకె_Matches,
	    matches);
}

#ఒకవేళ defined(FEAT_EVAL)
/*
 * One of the '*expr' options is changed: 'balloonexpr', 'diffexpr',
 * 'foldexpr', 'foldtext', 'formatexpr', 'includeexpr', 'indentexpr',
 * 'patchexpr', 'printexpr' and 'charconvert'.
 */
    అక్షరం *
did_సమితి_optexpr(optset_T *వాదనలు)
{
    char_u	**varp = (char_u **)వాదనలు->os_varp;

    // If the option value starts with <SID> or s:, then replace that with
    // the script identifier.
    char_u *name = get_scriptlocal_funcname(*varp);
    ఒకవేళ (name != ఖాళీ)
    {
	free_string_option(*varp);
	*varp = name;
    }

    తిరిగి ఖాళీ;
}
#అయితే_ముగింపు

/*
 * The 'pastetoggle' option is changed.
 */
    అక్షరం *
did_సమితి_pastetoggle(optset_T *వాదనలు వాడని)
{
    char_u	*p;

    // translate key codes like in a mapping
    ఒకవేళ (*p_pt)
    {
	(శూన్య)replace_termcodes(p_pt, &p, 0,
		REPTERM_FROM_PART | REPTERM_DO_LT, ఖాళీ);
	ఒకవేళ (p != ఖాళీ)
	{
	    free_string_option(p_pt);
	    p_pt = p;
	}
    }

    తిరిగి ఖాళీ;
}

#ఒకవేళ defined(FEAT_PROP_POPUP)
/*
 * The 'previewpopup' option is changed.
 */
    అక్షరం *
did_సమితి_previewpopup(optset_T *వాదనలు వాడని)
{
    ఒకవేళ (parse_previewpopup(ఖాళీ) == FAIL)
	తిరిగి e_invalid_వాదం;

# ఒకవేళ defined(FEAT_QUICKFIX)
    popup_close_info();
# endif
    తిరిగి ఖాళీ;
}

    స్థిర పూర్ణాంకం
expand_సమితి_popupoption(optexpand_T *వాదనలు, పూర్ణాంకం *అంకె_Matches, char_u ***matches,
	పూర్ణాంకం previewpopup)
{
    expand_T *xp = వాదనలు->oe_xp;

    ఒకవేళ (xp->xp_నమూనా > వాదనలు->oe_సమితి_arg && *(xp->xp_నమూనా-1) == ':')
    {
	ఒకవేళ (completing_విలువ_for_subopt(వాదనలు, "close")
		|| completing_విలువ_for_subopt(వాదనలు, "resize")
		|| completing_విలువ_for_subopt(వాదనలు, "shadow"))
	{
	    తిరిగి expand_సమితి_opt_string(
		    వాదనలు,
		    p_popup_option_on_off_విలువలు,
		    ARRAY_నిడివి(p_popup_option_on_off_విలువలు) - 1,
		    అంకె_Matches,
		    matches);
	}
	ఒకవేళ (completing_విలువ_for_subopt(వాదనలు, "border"))
	{
	    తిరిగి expand_సమితి_opt_string(
		    వాదనలు,
		    previewpopup ? p_popup_pvp_border_విలువలు
			: p_popup_cpp_border_విలువలు,
		    (previewpopup ? ARRAY_నిడివి(p_popup_pvp_border_విలువలు) - 1
			: ARRAY_నిడివి(p_popup_cpp_border_విలువలు)) - 1,
		    అంకె_Matches,
		    matches);
	}
	ఒకవేళ (completing_విలువ_for_subopt(వాదనలు, "align"))
	{
	    తిరిగి expand_సమితి_opt_string(
		    వాదనలు,
		    p_popup_option_align_విలువలు,
		    ARRAY_నిడివి(p_popup_option_align_విలువలు) - 1,
		    అంకె_Matches,
		    matches);
	}
	ఒకవేళ (completing_విలువ_for_subopt(వాదనలు, "highlight")
		|| completing_విలువ_for_subopt(వాదనలు, "borderhighlight"))
	{
	    // Return the list of all highlight names
	    తిరిగి expand_సమితి_opt_generic(
		    వాదనలు,
		    get_highlight_name,
		    అంకె_Matches,
		    matches);
	}
	తిరిగి FAIL;
    }

    తిరిగి expand_సమితి_opt_string(
	    వాదనలు,
	    previewpopup ? p_popup_pvp_option_విలువలు
		: p_popup_cpp_option_విలువలు,
	    previewpopup ? ARRAY_నిడివి(p_popup_pvp_option_విలువలు) - 1
		: ARRAY_నిడివి(p_popup_cpp_option_విలువలు) - 1,
	    అంకె_Matches,
	    matches);
}

    పూర్ణాంకం
expand_సమితి_previewpopup(optexpand_T *వాదనలు, పూర్ణాంకం *అంకె_Matches, char_u ***matches)
{
    తిరిగి expand_సమితి_popupoption(వాదనలు, అంకె_Matches, matches, TRUE);
}

    పూర్ణాంకం
expand_సమితి_completepopup(optexpand_T *వాదనలు, పూర్ణాంకం *అంకె_Matches, char_u ***matches)
{
    తిరిగి expand_సమితి_popupoption(వాదనలు, అంకె_Matches, matches, FALSE);
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_POSTSCRIPT)
/*
 * The 'printencoding' option is changed.
 */
    అక్షరం *
did_సమితి_printencoding(optset_T *వాదనలు వాడని)
{
    char_u	*s, *p;

    // Canonize 'printencoding' if VIM standard one
    p = enc_canonize(p_penc);
    ఒకవేళ (p != ఖాళీ)
    {
	vim_free(p_penc);
	p_penc = p;
    }
    లేకపోతే
    {
	// Ensure lower case and '-' for '_'
	కొరకు (s = p_penc; *s != NUL; s++)
	{
	    ఒకవేళ (*s == '_')
		*s = '-';
	    లేకపోతే
		*s = TOLOWER_ASC(*s);
	}
    }

    తిరిగి ఖాళీ;
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_PRINTER)

    స్థిర char_u *
get_printoptions_పేర్లు(expand_T *xp వాడని, పూర్ణాంకం idx)
{
    ఒకవేళ (idx >= (పూర్ణాంకం)ARRAY_నిడివి(printer_opts))
	తిరిగి ఖాళీ;
    తిరిగి (char_u*)printer_opts[idx].name;
}

/*
 * Expand 'printoptions' option
 */
    పూర్ణాంకం
expand_సమితి_printoptions(optexpand_T *వాదనలు, పూర్ణాంకం *అంకె_Matches, char_u ***matches)
{
    తిరిగి expand_సమితి_opt_generic(
	    వాదనలు,
	    get_printoptions_పేర్లు,
	    అంకె_Matches,
	    matches);
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_STL_OPT)
/*
 * The 'statusline' or the 'tabline' or the 'rulerformat' option is changed.
 * "rulerformat" is TRUE if the 'rulerformat' option is changed.
 */
    స్థిర అక్షరం *
parse_statustabline_rulerformat(optset_T *వాదనలు, పూర్ణాంకం rulerformat)
{
    char_u	**varp = (char_u **)వాదనలు->os_varp;
    char_u	*s;
    అక్షరం	*errmsg = ఖాళీ;
    పూర్ణాంకం		wid;

    ఒకవేళ (rulerformat)	// reset ru_wid first
	ru_wid = 0;
    s = *varp;
    ఒకవేళ (rulerformat && *s == '%')
    {
	// set ru_wid if 'ruf' starts with "%99("
	ఒకవేళ (*++s == '-')	// ignore a '-'
	    s++;
	wid = getdigits(&s);
	ఒకవేళ (wid && *s == '(' && (errmsg = check_stl_option(p_ruf)) == ఖాళీ)
	    ru_wid = wid;
	లేకపోతే
	{
	    // Validate the flags in 'rulerformat' only if it doesn't point to
	    // a custom function ("%!" flag).
	    ఒకవేళ ((*varp)[1] != '!')
		errmsg = check_stl_option(p_ruf);
	}
    }
    // check 'statusline' or 'tabline' only if it doesn't start with "%!"
    లేకపోతే ఒకవేళ (rulerformat || s[0] != '%' || s[1] != '!')
	errmsg = check_stl_option(s);
    ఒకవేళ (rulerformat && errmsg == ఖాళీ)
	comp_నిలువు();

    తిరిగి errmsg;
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_RENDER_OPTIONS)
/*
 * The 'renderoptions' option is changed.
 */
    అక్షరం *
did_సమితి_renderoptions(optset_T *వాదనలు వాడని)
{
    ఒకవేళ (!gui_mch_సమితి_rendering_options(p_rop))
	తిరిగి e_invalid_వాదం;

    తిరిగి ఖాళీ;
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_RIGHTLEFT)
/*
 * The 'rightleftcmd' option is changed.
 */
    అక్షరం *
did_సమితి_rightleftcmd(optset_T *వాదనలు)
{
    char_u	**varp = (char_u **)వాదనలు->os_varp;

    // Currently only "search" is a supported value.
    ఒకవేళ (**varp != NUL && STRCMP(*varp, "search") != 0)
	తిరిగి e_invalid_వాదం;

    తిరిగి ఖాళీ;
}

    పూర్ణాంకం
expand_సమితి_rightleftcmd(optexpand_T *వాదనలు, పూర్ణాంకం *అంకె_Matches, char_u ***matches)
{
    స్థిర అక్షరం *(p_rlc_విలువలు[]) = {"search", ఖాళీ};
    తిరిగి expand_సమితి_opt_string(
	    వాదనలు,
	    p_rlc_విలువలు,
	    ARRAY_నిడివి(p_rlc_విలువలు) - 1,
	    అంకె_Matches,
	    matches);
}
#అయితే_ముగింపు

#నిర్వచించు PUM_BORDER_CLEAR()   \
    చేయి {                     \
	pum_సమితి_border(FALSE); \
	pum_సమితి_shadow(FALSE); \
	pum_సమితి_margin(FALSE); \
    } అయితే (0)

/*
 * Parse a border value from a pumopt "border:" token.
 * Returns OK on success, FAIL on error.
 */
    స్థిర పూర్ణాంకం
parse_pumopt_border(char_u *val, పూర్ణాంకం len)
{
    // Use box-drawing characters only when 'encoding' is "utf-8" and
    // 'ambiwidth' is "single".
    పూర్ణాంకం	    can_use_box_chars = (enc_utf8 && *p_ambw == 's');
    char_u  *token;

    token = vim_strnsave(val, len);
    ఒకవేళ (token == ఖాళీ)
	తిరిగి FAIL;

    ఒకవేళ (can_use_box_chars && STRCMP(token, "single") == 0)
	pum_సమితి_border_chars(0x2500, 0x2502, 0x2500, 0x2502, // ─ │ ─ │
		0x250c, 0x2510, 0x2518, 0x2514); // ┌ ┐ ┘ └
    లేకపోతే ఒకవేళ (can_use_box_chars && STRCMP(token, "double") == 0)
	pum_సమితి_border_chars(0x2550, 0x2551, 0x2550, 0x2551, // ═ ║ ═ ║
		0x2554, 0x2557, 0x255D, 0x255A); // ╔ ╗ ╝  ╚
    లేకపోతే ఒకవేళ (can_use_box_chars && STRCMP(token, "round") == 0)
	pum_సమితి_border_chars(0x2500, 0x2502, 0x2500, 0x2502, // ─ │ ─ │
		0x256d, 0x256e, 0x256f, 0x2570); // ╭ ╮ ╯ ╰
    లేకపోతే ఒకవేళ (STRCMP(token, "ascii") == 0)
	pum_సమితి_border_chars('-', '|', '-', '|', '+', '+', '+', '+');
    లేకపోతే ఒకవేళ (STRNCMP(token, "custom:", 7) == 0)
    {
	char_u	*q = token + 7;
	పూర్ణాంకం	బహిర్గతం[8];

	కొరకు (పూర్ణాంకం i = 0; i < 8; i++)
	{
	    ఒకవేళ (*q == NUL || *q == ',')
	    {
		vim_free(token);
		తిరిగి FAIL;
	    }
	    బహిర్గతం[i] = mb_ptr2char(q);
	    mb_ptr2char_adv(&q);
	    ఒకవేళ (i < 7)
	    {
		ఒకవేళ (*q != ';')
		{
		    vim_free(token);
		    తిరిగి FAIL;
		}
		q++;
	    }
	}
	ఒకవేళ (*q != NUL && *q != ',')
	{
	    vim_free(token);
	    తిరిగి FAIL;
	}
	pum_సమితి_border_chars(బహిర్గతం[0], బహిర్గతం[1], బహిర్గతం[2], బహిర్గతం[3], బహిర్గతం[4], బహిర్గతం[5],
		బహిర్గతం[6], బహిర్గతం[7]);
    }
    లేకపోతే
    {
	vim_free(token);
	తిరిగి FAIL;
    }

    vim_free(token);
    pum_సమితి_border(TRUE);
    తిరిగి OK;
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
    అక్షరం *
did_సమితి_pumopt(optset_T *వాదనలు)
{
    char_u  **varp = (char_u **)వాదనలు->os_varp;
    char_u  *p;
    పూర్ణాంకం	    have_border = FALSE;
    పూర్ణాంకం	    have_margin = FALSE;

    // Reset to defaults.
    PUM_BORDER_CLEAR();
    p_ph = 0;
    p_pw = 15;
    p_pmw = 0;
    p_po = 100;

    ఒకవేళ (*varp == ఖాళీ || **varp == NUL)
	తిరిగి ఖాళీ;

    కొరకు (p = *varp; p != ఖాళీ && *p != NUL; )
    {
	char_u *comma = vim_strchr(p, ',');
	పూర్ణాంకం len;

	ఒకవేళ (comma != ఖాళీ)
	    len = (పూర్ణాంకం)(comma - p);
	లేకపోతే
	    len = (పూర్ణాంకం)STRLEN(p);

	ఒకవేళ (STRNCMP(p, "border:", 7) == 0)
	{
	    ఒకవేళ (have_border)
		వెళ్ళు error;
	    have_border = TRUE;
	    ఒకవేళ (parse_pumopt_border(p + 7, len - 7) == FAIL)
		వెళ్ళు error;
	}
	లేకపోతే ఒకవేళ (STRNCMP(p, "height:", 7) == 0)
	{
	    దీర్ఘ n = atol((అక్షరం *)p + 7);
	    ఒకవేళ (n < 0)
		వెళ్ళు error;
	    p_ph = n;
	}
	లేకపోతే ఒకవేళ (STRNCMP(p, "width:", 6) == 0)
	{
	    దీర్ఘ n = atol((అక్షరం *)p + 6);
	    ఒకవేళ (n < 0)
		వెళ్ళు error;
	    p_pw = n;
	}
	లేకపోతే ఒకవేళ (STRNCMP(p, "maxwidth:", 9) == 0)
	{
	    దీర్ఘ n = atol((అక్షరం *)p + 9);
	    ఒకవేళ (n < 0)
		వెళ్ళు error;
	    p_pmw = n;
	}
	లేకపోతే ఒకవేళ (STRNCMP(p, "opacity:", 8) == 0)
	{
	    దీర్ఘ n = atol((అక్షరం *)p + 8);
	    ఒకవేళ (n < 0 || n > 100)
		వెళ్ళు error;
	    p_po = n;
	}
	లేకపోతే ఒకవేళ (len == 6 && STRNCMP(p, "shadow", 6) == 0)
	    pum_సమితి_shadow(TRUE);
	లేకపోతే ఒకవేళ (len == 6 && STRNCMP(p, "margin", 6) == 0)
	{
	    have_margin = TRUE;
	    pum_సమితి_margin(TRUE);
	}
	లేకపోతే
	    వెళ్ళు error;

	ఒకవేళ (comma != ఖాళీ)
	    p = comma + 1;
	లేకపోతే
	    ఆపు;
    }

    ఒకవేళ (have_margin && !have_border)
	వెళ్ళు error;

    // Invalidate cached background for opacity changes.
    pum_opacity_మారింది();

    తిరిగి ఖాళీ;

error:
    PUM_BORDER_CLEAR();
    p_ph = 0;
    p_pw = 15;
    p_pmw = 0;
    p_po = 100;
    తిరిగి e_invalid_వాదం;
}

    స్థిర char_u *
get_pum_border_style(expand_T *xp వాడని, పూర్ణాంకం idx)
{
    స్థిర అక్షరం *styles[] = {"ascii", "custom:", "single", "double", "round"};
    తిరిగి idx < ((enc_utf8 && *p_ambw == 's') ? (పూర్ణాంకం)ARRAY_నిడివి(styles) : 2)
	    ? (char_u *)styles[idx] : ఖాళీ;
}

    పూర్ణాంకం
expand_సమితి_pumopt(optexpand_T *వాదనలు, పూర్ణాంకం *అంకె_Matches, char_u ***matches)
{
    expand_T *xp = వాదనలు->oe_xp;

    ఒకవేళ (xp->xp_నమూనా > వాదనలు->oe_సమితి_arg && *(xp->xp_నమూనా-1) == ':')
    {
	ఒకవేళ (completing_విలువ_for_subopt(వాదనలు, "border"))
	{
	    తిరిగి expand_సమితి_opt_generic(
		    వాదనలు, get_pum_border_style, అంకె_Matches, matches);
	}
	తిరిగి FAIL;
    }

    స్థిర అక్షరం *(p_pumopt_విలువలు[]) = {"border:", "height:", "width:",
	"maxwidth:", "opacity:", "shadow", "margin", ఖాళీ};
    తిరిగి expand_సమితి_opt_string(
	    వాదనలు,
	    p_pumopt_విలువలు,
	    ARRAY_నిడివి(p_pumopt_విలువలు) - 1,
	    అంకె_Matches,
	    matches);
}

/*
 * The 'pumborder' option is changed.
 * Rules:
 *   - One of { single, double, round, ascii, custom:XXXXXXXX } may appear.
 *   - "margin" may appear, but only together with exactly one border style.
 *   - "shadow" is independent and can be combined freely.
 */
    అక్షరం *
did_సమితి_pumborder(optset_T *వాదనలు)
{
    char_u  **varp = (char_u **)వాదనలు->os_varp;
    char_u  *p;
    పూర్ణాంకం	    len;
    పూర్ణాంకం	    have_border = FALSE;
    పూర్ణాంకం	    have_margin = FALSE;

    PUM_BORDER_CLEAR();

    ఒకవేళ (*varp == ఖాళీ || **varp == NUL)
	తిరిగి ఖాళీ;

    కొరకు (p = *varp; p != ఖాళీ && *p != NUL; )
    {
	char_u *comma = vim_strchr(p, ',');
	ఒకవేళ (comma != ఖాళీ)
	    len = (పూర్ణాంకం)(comma - p);
	లేకపోతే
	    len = (పూర్ణాంకం)STRLEN(p);

	ఒకవేళ (STRNCMP(p, "shadow", len) == 0 && len == 6)
	    pum_సమితి_shadow(TRUE);
	లేకపోతే ఒకవేళ (STRNCMP(p, "margin", len) == 0 && len == 6)
	{
	    have_margin = TRUE;
	    pum_సమితి_margin(TRUE);
	}
	లేకపోతే
	{
	    ఒకవేళ (have_border)
		వెళ్ళు error;
	    have_border = TRUE;
	    ఒకవేళ (parse_pumopt_border(p, len) == FAIL)
		వెళ్ళు error;
	}

	ఒకవేళ (comma != ఖాళీ)
	    p = comma + 1;
	లేకపోతే
	    ఆపు;
    }

    ఒకవేళ (have_margin && !have_border)
	వెళ్ళు error;

    తిరిగి ఖాళీ;

error:
    PUM_BORDER_CLEAR();
    pum_సమితి_border_chars(0, 0, 0, 0, 0, 0, 0, 0);
    తిరిగి e_invalid_వాదం;
}

    స్థిర char_u *
get_pumborder_token(expand_T *xp, పూర్ణాంకం idx)
{
    తిరిగి idx == 0 ? (char_u *)"margin"
	 : idx == 1 ? (char_u *)"shadow"
	 : get_pum_border_style(xp, idx - 2);
}

    పూర్ణాంకం
expand_సమితి_pumborder(optexpand_T *వాదనలు, పూర్ణాంకం *అంకె_Matches, char_u ***matches)
{
    తిరిగి expand_సమితి_opt_generic(
	    వాదనలు, get_pumborder_token, అంకె_Matches, matches);
}

#ఒకవేళ defined(FEAT_STL_OPT)

/*
 * The 'rulerformat' option is changed.
 */
    అక్షరం *
did_సమితి_rulerformat(optset_T *వాదనలు)
{
    తిరిగి parse_statustabline_rulerformat(వాదనలు, TRUE);
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_TABPANEL)
/*
 * Process the new 'tabpanelopt' option value.
 */
    అక్షరం *
did_సమితి_tabpanelopt(optset_T *వాదనలు వాడని)
{
    ఒకవేళ (tabpanelopt_మారింది() == FAIL)
	తిరిగి e_invalid_వాదం;

    తిరిగి ఖాళీ;
}

    పూర్ణాంకం
expand_సమితి_tabpanelopt(optexpand_T *వాదనలు, పూర్ణాంకం *అంకె_Matches, char_u ***matches)
{
    expand_T *xp = వాదనలు->oe_xp;

    ఒకవేళ (xp->xp_నమూనా > వాదనలు->oe_సమితి_arg && *(xp->xp_నమూనా-1) == ':')
    {
	ఒకవేళ (completing_విలువ_for_subopt(వాదనలు, "align"))
	{
	    తిరిగి expand_సమితి_opt_string(
		    వాదనలు,
		    p_tplo_align_విలువలు,
		    ARRAY_నిడివి(p_tplo_align_విలువలు) - 1,
		    అంకె_Matches,
		    matches);
	}
	తిరిగి FAIL;
    }

    తిరిగి expand_సమితి_opt_string(
	    వాదనలు,
	    p_tplo_విలువలు,
	    ARRAY_నిడివి(p_tplo_విలువలు) - 1,
	    అంకె_Matches,
	    matches);
}
#అయితే_ముగింపు

/*
 * The 'scrollopt' option is changed.
 */
    అక్షరం *
did_సమితి_scrollopt(optset_T *వాదనలు వాడని)
{
    తిరిగి did_సమితి_opt_అక్షరశ్రేణులు(p_sbo, p_scbopt_విలువలు, TRUE);
}

    పూర్ణాంకం
expand_సమితి_scrollopt(optexpand_T *వాదనలు, పూర్ణాంకం *అంకె_Matches, char_u ***matches)
{
    తిరిగి expand_సమితి_opt_string(
	    వాదనలు,
	    p_scbopt_విలువలు,
	    ARRAY_నిడివి(p_scbopt_విలువలు) - 1,
	    అంకె_Matches,
	    matches);
}

/*
 * The 'wlseat' option is changed
 */
    అక్షరం *
did_సమితి_wlseat(optset_T *వాదనలు వాడని)
{
#నిర్వచితమైతే FEAT_WAYLAND_CLIPBOARD
    // If there isn't any seat named 'wlseat', then let the Wayland clipboard be
    // unavailable. Ignore errors returned.
    clip_reset_wayland();
#అయితే_ముగింపు

    తిరిగి ఖాళీ;
}

/*
 * The 'selection' option is changed.
 */
    అక్షరం *
did_సమితి_selection(optset_T *వాదనలు వాడని)
{
    ఒకవేళ (*p_sel == NUL || check_opt_అక్షరశ్రేణులు(p_sel, p_sel_విలువలు, FALSE) != OK)
	తిరిగి e_invalid_వాదం;

    తిరిగి ఖాళీ;
}

    పూర్ణాంకం
expand_సమితి_selection(optexpand_T *వాదనలు, పూర్ణాంకం *అంకె_Matches, char_u ***matches)
{
    తిరిగి expand_సమితి_opt_string(
	    వాదనలు,
	    p_sel_విలువలు,
	    ARRAY_నిడివి(p_sel_విలువలు) - 1,
	    అంకె_Matches,
	    matches);
}

/*
 * The 'selectmode' option is changed.
 */
    అక్షరం *
did_సమితి_selectmode(optset_T *వాదనలు వాడని)
{
    తిరిగి did_సమితి_opt_అక్షరశ్రేణులు(p_slm, p_slm_విలువలు, TRUE);
}

    పూర్ణాంకం
expand_సమితి_selectmode(optexpand_T *వాదనలు, పూర్ణాంకం *అంకె_Matches, char_u ***matches)
{
    తిరిగి expand_సమితి_opt_string(
	    వాదనలు,
	    p_slm_విలువలు,
	    ARRAY_నిడివి(p_slm_విలువలు) - 1,
	    అంకె_Matches,
	    matches);
}

#ఒకవేళ defined(FEAT_SESSION)
/*
 * The 'sessionoptions' option is changed.
 */
    అక్షరం *
did_సమితి_sessionoptions(optset_T *వాదనలు)
{
    ఒకవేళ (opt_అక్షరశ్రేణులు_flags(p_ssop, p_ssop_విలువలు, &ssop_flags, TRUE) != OK)
	తిరిగి e_invalid_వాదం;
    ఒకవేళ ((ssop_flags & SSOP_CURDIR) && (ssop_flags & SSOP_SESDIR))
    {
	// Don't allow both "sesdir" and "curdir".
	(శూన్య)opt_అక్షరశ్రేణులు_flags(వాదనలు->os_oldval.string, p_ssop_విలువలు,
							&ssop_flags, TRUE);
	తిరిగి e_invalid_వాదం;
    }

    తిరిగి ఖాళీ;
}

    పూర్ణాంకం
expand_సమితి_sessionoptions(
    optexpand_T		*వాదనలు,
    పూర్ణాంకం			*అంకె_Matches,
    char_u		***matches)
{
    తిరిగి expand_సమితి_opt_string(
	    వాదనలు,
	    p_ssop_విలువలు,
	    ARRAY_నిడివి(p_ssop_విలువలు) - 1,
	    అంకె_Matches,
	    matches);
}
#అయితే_ముగింపు

/*
 * Validate 'shellpipe'/'shellredir' option.
 */
    అక్షరం *
did_సమితి_shellpipe_redir(optset_T *వాదనలు)
{
    char_u	*p;
    bool	చూసిన = false;

    కొరకు (p = వాదనలు->os_newval.string; *p != NUL; ++p)
    {
	ఒకవేళ (*p != '%')
	    కొనసాగు;

	ఒకవేళ (p[1] == NUL)
	    తిరిగి e_invalid_format_string_single_percent_s;

	ఒకవేళ (p[1] == '%')
	{
	    ++p;    // skip second %
	    కొనసాగు;
	}

	ఒకవేళ (p[1] == 's')
	{
	    ఒకవేళ (చూసిన)
		తిరిగి e_invalid_format_string_single_percent_s;

	    చూసిన = true;
	    ++p;    // consume 's'
	    కొనసాగు;
	}
	తిరిగి e_invalid_format_string_single_percent_s;
    }
    తిరిగి ఖాళీ;
}

/*
 * The 'shortmess' option is changed.
 */
    అక్షరం *
did_సమితి_shortmess(optset_T *వాదనలు)
{
    char_u	**varp = (char_u **)వాదనలు->os_varp;

    తిరిగి did_సమితి_option_listflag(*varp, (char_u *)SHM_ALL, వాదనలు->os_errbuf,
		    వాదనలు->os_errbuflen);
}

    పూర్ణాంకం
expand_సమితి_shortmess(optexpand_T *వాదనలు, పూర్ణాంకం *అంకె_Matches, char_u ***matches)
{
    తిరిగి expand_సమితి_opt_listflag(వాదనలు, (char_u*)SHM_ALL, అంకె_Matches, matches);
}

#ఒకవేళ defined(FEAT_LINEBREAK)
/*
 * The 'showbreak' option is changed.
 */
    అక్షరం *
did_సమితి_showbreak(optset_T *వాదనలు)
{
    char_u	**varp = (char_u **)వాదనలు->os_varp;
    char_u	*s;

    కొరకు (s = *varp; *s; )
    {
	ఒకవేళ (ptr2cells(s) != 1)
	    తిరిగి e_showbreak_కలిగియుంది_unprintable_or_wide_character;
	MB_PTR_ADV(s);
    }

    తిరిగి ఖాళీ;
}
#అయితే_ముగింపు

/*
 * The 'showcmdloc' option is changed.
 */
    అక్షరం *
did_సమితి_showcmdloc(optset_T *వాదనలు వాడని)
{
    అక్షరం	*errmsg = did_సమితి_opt_అక్షరశ్రేణులు(p_sloc, p_sloc_విలువలు, FALSE);

    ఒకవేళ (errmsg == ఖాళీ)
	comp_నిలువు();

    తిరిగి errmsg;
}

    పూర్ణాంకం
expand_సమితి_showcmdloc(optexpand_T *వాదనలు, పూర్ణాంకం *అంకె_Matches, char_u ***matches)
{
    తిరిగి expand_సమితి_opt_string(
	    వాదనలు,
	    p_sloc_విలువలు,
	    ARRAY_నిడివి(p_sloc_విలువలు) - 1,
	    అంకె_Matches,
	    matches);
}

#ఒకవేళ defined(FEAT_SIGNS)
/*
 * The 'signcolumn' option is changed.
 */
    అక్షరం *
did_సమితి_signcolumn(optset_T *వాదనలు)
{
    char_u	**varp = (char_u **)వాదనలు->os_varp;

# ఒకవేళ defined(FEAT_LINEBREAK)
    ఒకవేళ (check_opt_అక్షరశ్రేణులు(*varp, p_scl_విలువలు, FALSE) != OK)
	తిరిగి e_invalid_వాదం;
    // When changing the 'signcolumn' to or from 'number', recompute the
    // width of the number column if 'number' or 'relativenumber' is set.
    ఒకవేళ (((*వాదనలు->os_oldval.string == 'n' && వాదనలు->os_oldval.string[1] == 'u')
		|| (*curwin->w_p_scl == 'n' && *(curwin->w_p_scl + 1) =='u'))
	    && (curwin->w_p_nu || curwin->w_p_rnu))
	curwin->w_nrwidth_పంక్తి_లెక్క = 0;
# endif

# ఒకవేళ defined(FEAT_GUI)
    // In the GUI, when sign icons are used, a full screen clear is needed
    // to properly redraw the sign icons.
    ఒకవేళ (gui.లోపల_use && curbuf->b_signlist != ఖాళీ)
	redraw_all_later(UPD_CLEAR);
# endif

    తిరిగి ఖాళీ;
}

    పూర్ణాంకం
expand_సమితి_signcolumn(optexpand_T *వాదనలు, పూర్ణాంకం *అంకె_Matches, char_u ***matches)
{
    తిరిగి expand_సమితి_opt_string(
	    వాదనలు,
	    p_scl_విలువలు,
	    ARRAY_నిడివి(p_scl_విలువలు) - 1,
	    అంకె_Matches,
	    matches);
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_SPELL)
/*
 * The 'spellcapcheck' option is changed.
 */
    అక్షరం *
did_సమితి_spellcapcheck(optset_T *వాదనలు వాడని)
{
    // compile the regexp program.
    తిరిగి compile_cap_prog(curwin->w_s);
}

/*
 * The 'spellfile' option is changed.
 */
    అక్షరం *
did_సమితి_spellfile(optset_T *వాదనలు)
{
    char_u	**varp = (char_u **)వాదనలు->os_varp;

    ఒకవేళ (!valid_spellfile(*varp))
	తిరిగి e_invalid_వాదం;

    // If there is a window for this buffer in which 'spell' is set load the
    // wordlists.
    తిరిగి did_సమితి_spell_option();
}

/*
 * The 'spell' option is changed.
 */
    అక్షరం *
did_సమితి_spelllang(optset_T *వాదనలు)
{
    char_u	**varp = (char_u **)వాదనలు->os_varp;

    ఒకవేళ (!valid_spelllang(*varp))
	తిరిగి e_invalid_వాదం;

    // If there is a window for this buffer in which 'spell' is set load the
    // wordlists.
    తిరిగి did_సమితి_spell_option();
}

/*
 * The 'spelloptions' option is changed.
 */
    అక్షరం *
did_సమితి_spelloptions(optset_T *వాదనలు)
{
    char_u	**varp = (char_u **)వాదనలు->os_varp;

    ఒకవేళ (**varp != NUL && STRCMP(*varp, "camel") != 0)
	తిరిగి e_invalid_వాదం;

    తిరిగి ఖాళీ;
}

    పూర్ణాంకం
expand_సమితి_spelloptions(optexpand_T *వాదనలు, పూర్ణాంకం *అంకె_Matches, char_u ***matches)
{
    స్థిర అక్షరం *(p_spo_విలువలు[]) = {"camel", ఖాళీ};
    తిరిగి expand_సమితి_opt_string(
	    వాదనలు,
	    p_spo_విలువలు,
	    ARRAY_నిడివి(p_spo_విలువలు) - 1,
	    అంకె_Matches,
	    matches);
}

/*
 * The 'spellsuggest' option is changed.
 */
    అక్షరం *
did_సమితి_spellsuggest(optset_T *వాదనలు వాడని)
{
    ఒకవేళ (spell_check_sps() != OK)
	తిరిగి e_invalid_వాదం;

    తిరిగి ఖాళీ;
}

    పూర్ణాంకం
expand_సమితి_spellsuggest(optexpand_T *వాదనలు, పూర్ణాంకం *అంకె_Matches, char_u ***matches)
{
    తిరిగి expand_సమితి_opt_string(
	    వాదనలు,
	    p_sps_విలువలు,
	    ARRAY_నిడివి(p_sps_విలువలు) - 1,
	    అంకె_Matches,
	    matches);
}
#అయితే_ముగింపు

/*
 * The 'splitkeep' option is changed.
 */
    అక్షరం *
did_సమితి_splitkeep(optset_T *వాదనలు వాడని)
{
    win_T	*wp;
    tabpage_T	*tp;
    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	wp->w_prev_ఎత్తు = wp->w_ఎత్తు;
	wp->w_prev_winrow = wp->w_winrow;
    }
    తిరిగి did_సమితి_opt_అక్షరశ్రేణులు(p_spk, p_spk_విలువలు, FALSE);
}

    పూర్ణాంకం
expand_సమితి_splitkeep(optexpand_T *వాదనలు, పూర్ణాంకం *అంకె_Matches, char_u ***matches)
{
    తిరిగి expand_సమితి_opt_string(
	    వాదనలు,
	    p_spk_విలువలు,
	    ARRAY_నిడివి(p_spk_విలువలు) - 1,
	    అంకె_Matches,
	    matches);
}

#ఒకవేళ defined(FEAT_STL_OPT)
/*
 * The 'statusline' option is changed.
 */
    అక్షరం *
did_సమితి_statusline(optset_T *వాదనలు)
{
    char_u  **varp = (char_u **)వాదనలు->os_varp;
    అక్షరం    *ret = parse_statustabline_rulerformat(వాదనలు, FALSE);

    ఒకవేళ (ret != ఖాళీ)
	తిరిగి ret;
    update_stl_rendered_ఎత్తు(varp == &curwin->w_p_stl ? curwin : ఖాళీ);
    చట్రం_change_statusline_ఎత్తు();

    తిరిగి ఖాళీ;
}

/*
 * Rewrite the 'statuslineopt' string: replace the last "maxheight:N" with
 * actual_stlh and remove earlier duplicates.  Keyword order is preserved.
 */
    స్థిర శూన్య
update_stlo_maxheight(char_u **varp, పూర్ణాంకం actual_stlh)
{
    స్థిరరాశి char_u    *src = *varp;
    స్థిరరాశి char_u    *చివరి_mh = ఖాళీ;
    స్థిరరాశి char_u    *p;

    // Find the last "maxheight:" token.
    p = src;
    అయితే (*p != NUL)
    {
	ఒకవేళ (STRNCMP(p, "maxheight:", 10) == 0)
	    చివరి_mh = p;
	అయితే (*p != ',' && *p != NUL)
	    ++p;
	ఒకవేళ (*p == ',')
	    ++p;
    }

    ఒకవేళ (చివరి_mh == ఖాళీ)
	తిరిగి;

    size_t	bufsize = STRLEN(src) + NUMBUFLEN + 1;
    char_u	*buf = alloc(bufsize);
    ఒకవేళ (buf == ఖాళీ)
	తిరిగి;

    పూర్ణాంకం		len = 0;
    bool	need_comma = false;

    p = src;
    అయితే (*p != NUL)
    {
	స్థిరరాశి	char_u	*tok = p;
	పూర్ణాంకం	tok_len;

	అయితే (*p != ',' && *p != NUL)
	    ++p;
	tok_len = (పూర్ణాంకం)(p - tok);
	ఒకవేళ (*p == ',')
	    ++p;

	ఒకవేళ (STRNCMP(tok, "maxheight:", 10) == 0)
	{
	    ఒకవేళ (tok == చివరి_mh)
	    {
		// Replace the last occurrence with the actual value.
		ఒకవేళ (need_comma)
		    buf[len++] = ',';
		len += vim_snprintf((అక్షరం *)buf + len,
			bufsize - len, "maxheight:%d", actual_stlh);
		need_comma = true;
	    }
	}
	లేకపోతే
	{
	    ఒకవేళ (need_comma)
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
    అక్షరం *
did_సమితి_statuslineopt(optset_T *వాదనలు)
{
    char_u	**varp = (char_u **)వాదనలు->os_varp;
    win_T       *wp = varp == &curwin->w_p_stlo ? curwin : ఖాళీ;

    ఒకవేళ (statuslineopt_మారింది(*varp, wp) == FAIL)
	తిరిగి e_invalid_వాదం;

    // Sync stl_rendered_height with the current statusline.
    update_stl_rendered_ఎత్తు(wp);

    // Update the maxheight value to the actual value set.
    // Note: Must be changed if p_stlo_values are changed.
    ఒకవేళ (*varp != ఖాళీలు_option)
    {
	పూర్ణాంకం actual_stlh;

	ఒకవేళ (wp != ఖాళీ)
	{
	    చట్రం_change_statusline_ఎత్తు();
	    actual_stlh = MIN(wp->w_p_stlo_mh,
		    wp->w_ఎత్తు + wp->w_స్థితి_ఎత్తు - p_wmh);
	}
	లేకపోతే
	    actual_stlh = చట్రం_change_statusline_ఎత్తు();

	// Only re-serialize when the window actually has a status line shown.
	ఒకవేళ (actual_stlh > 0)
	{
	    update_stlo_maxheight(varp, actual_stlh);
	    // Update the parsed maxheight member directly.
	    ఒకవేళ (wp != ఖాళీ)
		wp->w_p_stlo_mh = actual_stlh;
	    లేకపోతే
		సమితి_stlo_mh(actual_stlh);
	}
    }
    లేకపోతే
	చట్రం_change_statusline_ఎత్తు();

    ఒకవేళ ((వాదనలు->os_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0)
    {
	// :set clears the local values.
	clear_string_option(&curwin->w_p_stlo);
	curwin->w_p_stlo_fh = -1;
	curwin->w_p_stlo_mh = -1;
    }

    తిరిగి ఖాళీ;
}

    పూర్ణాంకం
expand_సమితి_statuslineopt(optexpand_T *వాదనలు, పూర్ణాంకం *అంకె_Matches, char_u ***matches)
{
    తిరిగి expand_సమితి_opt_string(
	    వాదనలు,
	    p_stlo_విలువలు,
	    ARRAY_నిడివి(p_stlo_విలువలు) - 1,
	    అంకె_Matches,
	    matches);
}
#అయితే_ముగింపు

/*
 * The 'swapsync' option is changed.
 */
    అక్షరం *
did_సమితి_swapsync(optset_T *వాదనలు వాడని)
{
    తిరిగి did_సమితి_opt_అక్షరశ్రేణులు(p_sws, p_sws_విలువలు, FALSE);
}

    పూర్ణాంకం
expand_సమితి_swapsync(optexpand_T *వాదనలు, పూర్ణాంకం *అంకె_Matches, char_u ***matches)
{
    తిరిగి expand_సమితి_opt_string(
	    వాదనలు,
	    p_sws_విలువలు,
	    ARRAY_నిడివి(p_sws_విలువలు) - 1,
	    అంకె_Matches,
	    matches);
}

/*
 * The 'switchbuf' option is changed.
 */
    అక్షరం *
did_సమితి_switchbuf(optset_T *వాదనలు వాడని)
{
    తిరిగి did_సమితి_opt_flags(p_swb, p_swb_విలువలు, &swb_flags, TRUE);
}

    పూర్ణాంకం
expand_సమితి_switchbuf(optexpand_T *వాదనలు, పూర్ణాంకం *అంకె_Matches, char_u ***matches)
{
    తిరిగి expand_సమితి_opt_string(
	    వాదనలు,
	    p_swb_విలువలు,
	    ARRAY_నిడివి(p_swb_విలువలు) - 1,
	    అంకె_Matches,
	    matches);
}

/*
 * The 'tabclose' option is changed.
 */
    అక్షరం *
did_సమితి_tabclose(optset_T *వాదనలు వాడని)
{
    తిరిగి did_సమితి_opt_flags(p_tcl, p_tcl_విలువలు, &tcl_flags, TRUE);
}

    పూర్ణాంకం
expand_సమితి_tabclose(optexpand_T *వాదనలు, పూర్ణాంకం *అంకె_Matches, char_u ***matches)
{
    తిరిగి expand_సమితి_opt_string(
	    వాదనలు,
	    p_tcl_విలువలు,
	    ARRAY_నిడివి(p_tcl_విలువలు) - 1,
	    అంకె_Matches,
	    matches);
}

#ఒకవేళ defined(FEAT_STL_OPT)
/*
 * The 'tabline' option is changed.
 */
    అక్షరం *
did_సమితి_tabline(optset_T *వాదనలు)
{
    తిరిగి parse_statustabline_rulerformat(వాదనలు, FALSE);
}
#అయితే_ముగింపు

/*
 * The 'tagcase' option is changed.
 */
    అక్షరం *
did_సమితి_tagcase(optset_T *వాదనలు)
{
    గుర్తులేని పూర్ణాంకం	*flags;
    char_u		*p;

    ఒకవేళ (వాదనలు->os_flags & OPT_LOCAL)
    {
	p = curbuf->b_p_tc;
	flags = &curbuf->b_tc_flags;
    }
    లేకపోతే
    {
	p = p_tc;
	flags = &tc_flags;
    }

    ఒకవేళ ((వాదనలు->os_flags & OPT_LOCAL) && *p == NUL)
	// make the local value empty: use the global value
	*flags = 0;
    లేకపోతే ఒకవేళ (*p == NUL
	    || opt_అక్షరశ్రేణులు_flags(p, p_tc_విలువలు, flags, FALSE) != OK)
	తిరిగి e_invalid_వాదం;

    తిరిగి ఖాళీ;
}

    పూర్ణాంకం
expand_సమితి_tagcase(optexpand_T *వాదనలు, పూర్ణాంకం *అంకె_Matches, char_u ***matches)
{
    తిరిగి expand_సమితి_opt_string(
	    వాదనలు,
	    p_tc_విలువలు,
	    ARRAY_నిడివి(p_tc_విలువలు) - 1,
	    అంకె_Matches,
	    matches);
}

/*
 * The 'term' option is changed.
 */
    అక్షరం *
did_సమితి_పదం(optset_T *వాదనలు వాడని)
{
    ఒకవేళ (T_NAME[0] == NUL)
	తిరిగి e_cannot_సమితి_పదం_గమ్యం_ఖాళీలు_string;
#నిర్వచితమైతే FEAT_GUI
    ఒకవేళ (gui.లోపల_use)
	తిరిగి e_cannot_change_పదం_లోపల_GUI;
    ఒకవేళ (పదం_is_gui(T_NAME))
	తిరిగి e_use_gui_గమ్యం_ప్రారంభం_GUI;
#అయితే_ముగింపు
    ఒకవేళ (సమితి_termname(T_NAME) == FAIL)
	తిరిగి e_not_దొరికింది_లోపల_termcap;

    // Screen colors may have changed.
    redraw_later_clear();

    తిరిగి ఖాళీ;
}

/*
 * Some terminal option (t_xxx) is changed
 */
    అక్షరం *
did_సమితి_పదం_option(optset_T *వాదనలు)
{
    char_u	**varp = (char_u **)వాదనలు->os_varp;

    ఒకవేళ (!నిండిన_screen)
	తిరిగి ఖాళీ;

    // ":set t_Co=0" and ":set t_Co=1" do ":set t_Co="
    ఒకవేళ (varp == &T_CCO)
    {
	పూర్ణాంకం colors = atoi((అక్షరం *)T_CCO);

	// Only reinitialize colors if t_Co value has really changed to
	// avoid expensive reload of colorscheme if t_Co is set to the
	// same value multiple times.
	ఒకవేళ (colors != t_colors)
	{
	    t_colors = colors;
	    ఒకవేళ (t_colors <= 1)
	    {
		vim_free(T_CCO);
		T_CCO = ఖాళీలు_option;
	    }
#ఒకవేళ defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
	    ఒకవేళ (is_పదం_win32())
	    {
		మార్పిడి_tcap();
		వాదనలు->os_did_swaptcap = TRUE;
	    }
#అయితే_ముగింపు
	    // We now have a different color setup, initialize it again.
	    init_highlight(TRUE, FALSE);
	}
    }
    ttest(FALSE);
    ఒకవేళ (varp == &T_ME)
    {
	బహిర్గతం_str(T_ME);
	redraw_later(UPD_CLEAR);
#ఒకవేళ defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
	// Since t_me has been set, this probably means that the user
	// wants to use this as default colors.  Need to reset default
	// background/foreground colors.
# ifdef VIMDLL
	ఒకవేళ (!gui.లోపల_use && !gui.starting)
# endif
	    mch_సమితి_సాధారణ_colors();
#అయితే_ముగింపు
    }
    ఒకవేళ (varp == &T_BE && termcap_చురుకైన)
    {
	MAY_WANT_గమ్యం_LOG_THIS;

	ఒకవేళ (*T_BE == NUL)
	    // When clearing t_BE we assume the user no longer wants
	    // bracketed paste, thus disable it by writing t_BD.
	    బహిర్గతం_str(T_BD);
	లేకపోతే
	    బహిర్గతం_str(T_BE);
    }

    ఒకవేళ (varp == &T_BSU || varp == &T_ESU)
	పదం_సమితి_sync_output(పదం_SYNC_OUTPUT_OFF);

    తిరిగి ఖాళీ;
}


#నిర్వచితమైతే UNIX
/*
 * The 'termresize' option is changed.
 */
    అక్షరం *
did_సమితి_termresize(optset_T *వాదనలు వాడని)
{
    // If empty or "inband", then attempt to enable in-band resize events.
    ఒకవేళ (*p_trz == NUL || STRCMP(p_trz, "inband") == 0)
	పదం_సమితి_win_resize(true);
    లేకపోతే ఒకవేళ (STRCMP(p_trz, "sigwinch") == 0)
	పదం_సమితి_win_resize(false);
    లేకపోతే
	తిరిగి e_invalid_వాదం;

    తిరిగి ఖాళీ;
}

    పూర్ణాంకం
expand_సమితి_termresize(optexpand_T *వాదనలు, పూర్ణాంకం *అంకె_Matches, char_u ***matches)
{
    తిరిగి expand_సమితి_opt_string(
	    వాదనలు,
	    p_trz_విలువలు,
	    ARRAY_నిడివి(p_trz_విలువలు) - 1,
	    అంకె_Matches,
	    matches);
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_TERMINAL)
/*
 * The 'termwinkey' option is changed.
 */
    అక్షరం *
did_సమితి_termwinkey(optset_T *వాదనలు)
{
    char_u	**varp = (char_u **)వాదనలు->os_varp;

    ఒకవేళ ((*varp)[0] != NUL && string_గమ్యం_కీలకం(*varp, TRUE) == 0)
	తిరిగి e_invalid_వాదం;

    తిరిగి ఖాళీ;
}

/*
 * The 'termwinsize' option is changed.
 */
    అక్షరం *
did_సమితి_termwinsize(optset_T *వాదనలు)
{
    char_u	**varp = (char_u **)వాదనలు->os_varp;
    char_u	*p;

    ఒకవేళ ((*varp)[0] == NUL)
	తిరిగి ఖాళీ;

    p = skipdigits(*varp);
    ఒకవేళ (p == *varp || (*p != 'x' && *p != '*') || *skipdigits(p + 1) != NUL)
	తిరిగి e_invalid_వాదం;

    తిరిగి ఖాళీ;
}

# ఒకవేళ defined(MSWIN)
/*
 * The 'termwintype' option is changed.
 */
    అక్షరం *
did_సమితి_termwintype(optset_T *వాదనలు వాడని)
{
    తిరిగి did_సమితి_opt_అక్షరశ్రేణులు(p_twt, p_twt_విలువలు, FALSE);
}

    పూర్ణాంకం
expand_సమితి_termwintype(optexpand_T *వాదనలు, పూర్ణాంకం *అంకె_Matches, char_u ***matches)
{
    తిరిగి expand_సమితి_opt_string(
	    వాదనలు,
	    p_twt_విలువలు,
	    ARRAY_నిడివి(p_twt_విలువలు) - 1,
	    అంకె_Matches,
	    matches);
}
# endif
#అయితే_ముగింపు

/*
 * The 'titlestring' option is changed.
 */
    అక్షరం *
did_సమితి_titlestring(optset_T *వాదనలు)
{
    పూర్ణాంకం flagval = 0;

#నిర్వచితమైతే FEAT_STL_OPT
    flagval = STL_లోపల_TITLE;
#అయితే_ముగింపు
    తిరిగి parse_titleiconstring(వాదనలు, flagval);
}

#ఒకవేళ defined(FEAT_TOOLBAR) && !defined(FEAT_GUI_MSWIN)
/*
 * The 'toolbar' option is changed.
 */
    అక్షరం *
did_సమితి_toolbar(optset_T *వాదనలు వాడని)
{
    ఒకవేళ (opt_అక్షరశ్రేణులు_flags(p_toolbar, p_toolbar_విలువలు, &toolbar_flags,
								TRUE) != OK)
	తిరిగి e_invalid_వాదం;

    బహిర్గతం_flush();
    gui_mch_show_toolbar((toolbar_flags &
		(TOOLBAR_పాఠ్యం | TOOLBAR_ICONS)) != 0);
    తిరిగి ఖాళీ;
}

    పూర్ణాంకం
expand_సమితి_toolbar(optexpand_T *వాదనలు, పూర్ణాంకం *అంకె_Matches, char_u ***matches)
{
    తిరిగి expand_సమితి_opt_string(
	    వాదనలు,
	    p_toolbar_విలువలు,
	    ARRAY_నిడివి(p_toolbar_విలువలు) - 1,
	    అంకె_Matches,
	    matches);
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_TOOLBAR) && defined(FEAT_GUI_GTK)
/*
 * The 'toolbariconsize' option is changed.  GTK+ 2 only.
 */
    అక్షరం *
did_సమితి_toolbariconsize(optset_T *వాదనలు వాడని)
{
    ఒకవేళ (opt_అక్షరశ్రేణులు_flags(p_tbis, p_tbis_విలువలు, &tbis_flags, FALSE) != OK)
	తిరిగి e_invalid_వాదం;

    బహిర్గతం_flush();
    gui_mch_show_toolbar((toolbar_flags &
		(TOOLBAR_పాఠ్యం | TOOLBAR_ICONS)) != 0);
    తిరిగి ఖాళీ;
}

    పూర్ణాంకం
expand_సమితి_toolbariconsize(
    optexpand_T	*వాదనలు,
    పూర్ణాంకం		*అంకె_Matches,
    char_u	***matches)
{
    తిరిగి expand_సమితి_opt_string(
	    వాదనలు,
	    p_tbis_విలువలు,
	    ARRAY_నిడివి(p_tbis_విలువలు) - 1,
	    అంకె_Matches,
	    matches);
}
#అయితే_ముగింపు

#ఒకవేళ defined(UNIX) || defined(VMS)
/*
 * The 'ttymouse' option is changed.
 */
    అక్షరం *
did_సమితి_ttymouse(optset_T *వాదనలు వాడని)
{
    అక్షరం *errmsg = ఖాళీ;

    // Switch the mouse off before changing the escape sequences used for
    // that.
    mch_setmouse(FALSE);
    ఒకవేళ (opt_అక్షరశ్రేణులు_flags(p_ttym, p_ttym_విలువలు, &ttym_flags, FALSE) != OK)
	errmsg = e_invalid_వాదం;
    లేకపోతే
	check_mouse_termcode();
    ఒకవేళ (termcap_చురుకైన)
	setmouse();		// may switch it on again

    తిరిగి errmsg;
}

    పూర్ణాంకం
expand_సమితి_ttymouse(optexpand_T *వాదనలు, పూర్ణాంకం *అంకె_Matches, char_u ***matches)
{
    తిరిగి expand_సమితి_opt_string(
	    వాదనలు,
	    p_ttym_విలువలు,
	    ARRAY_నిడివి(p_ttym_విలువలు) - 1,
	    అంకె_Matches,
	    matches);
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_VARTABS)
/*
 * The 'varsofttabstop' option is changed.
 */
    అక్షరం *
did_సమితి_varsofttabstop(optset_T *వాదనలు)
{
    char_u	**varp = (char_u **)వాదనలు->os_varp;
    char_u *cp;

    ఒకవేళ (!((*varp)[0]) || ((*varp)[0] == '0' && !((*varp)[1])))
	VIM_CLEAR(curbuf->b_p_vsts_array);
    లేకపోతే
    {
	కొరకు (cp = *varp; *cp; ++cp)
	{
	    ఒకవేళ (vim_isdigit(*cp))
		కొనసాగు;
	    ఒకవేళ (*cp == ',' && cp > *varp && *(cp-1) != ',')
		కొనసాగు;
	    తిరిగి e_invalid_వాదం;
	}

	పూర్ణాంకం *oldarray = curbuf->b_p_vsts_array;
	ఒకవేళ (tabstop_సమితి(*varp, &(curbuf->b_p_vsts_array)) == OK)
	{
	    ఒకవేళ (oldarray)
		vim_free(oldarray);
	}
	లేకపోతే
	    తిరిగి e_invalid_వాదం;
    }

    తిరిగి ఖాళీ;
}

/*
 * The 'vartabstop' option is changed.
 */
    అక్షరం *
did_సమితి_vartabstop(optset_T *వాదనలు)
{
    char_u	**varp = (char_u **)వాదనలు->os_varp;
    char_u *cp;

    ఒకవేళ (!((*varp)[0]) || ((*varp)[0] == '0' && !((*varp)[1])))
	VIM_CLEAR(curbuf->b_p_vts_array);
    లేకపోతే
    {
	కొరకు (cp = *varp; *cp; ++cp)
	{
	    ఒకవేళ (vim_isdigit(*cp))
		కొనసాగు;
	    ఒకవేళ (*cp == ',' && cp > *varp && *(cp-1) != ',')
		కొనసాగు;
	    తిరిగి e_invalid_వాదం;
	}

	పూర్ణాంకం *oldarray = curbuf->b_p_vts_array;

	ఒకవేళ (tabstop_సమితి(*varp, &(curbuf->b_p_vts_array)) == OK)
	{
	    vim_free(oldarray);
# ifdef FEAT_FOLDING
	    ఒకవేళ (foldmethodIsIndent(curwin))
		foldUpdateAll(curwin);
# endif
	}
	లేకపోతే
	    తిరిగి e_invalid_వాదం;
    }

    తిరిగి ఖాళీ;
}
#అయితే_ముగింపు

/*
 * The 'verbosefile' option is changed.
 */
    అక్షరం *
did_సమితి_verbosefile(optset_T *వాదనలు వాడని)
{
    verbose_stop();
    ఒకవేళ (*p_vfile != NUL && verbose_open() == FAIL)
	తిరిగి e_invalid_వాదం;

    తిరిగి ఖాళీ;
}

#ఒకవేళ defined(FEAT_SESSION)
/*
 * The 'viewoptions' option is changed.
 */
    అక్షరం *
did_సమితి_viewoptions(optset_T *వాదనలు వాడని)
{
    తిరిగి did_సమితి_opt_flags(p_vop, p_ssop_విలువలు, &vop_flags, TRUE);
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_VIMINFO)
/*
 * The 'viminfo' option is changed.
 */
    అక్షరం *
did_సమితి_viminfo(optset_T *వాదనలు)
{
    char_u	*s;
    అక్షరం	*errmsg = ఖాళీ;

    కొరకు (s = p_viminfo; *s;)
    {
	// Check it's a valid character
	ఒకవేళ (vim_strchr((char_u *)"!\"%'/:<@cfhnrs", *s) == ఖాళీ)
	{
	    errmsg = illegal_char(వాదనలు->os_errbuf, వాదనలు->os_errbuflen, *s);
	    ఆపు;
	}
	ఒకవేళ (*s == 'n')	// name is always last one
	    ఆపు;
	లేకపోతే ఒకవేళ (*s == 'r') // skip until next ','
	{
	    అయితే (*++s && *s != ',')
		;
	}
	లేకపోతే ఒకవేళ (*s == '%')
	{
	    // optional number
	    అయితే (vim_isdigit(*++s))
		;
	}
	లేకపోతే ఒకవేళ (*s == '!' || *s == 'h' || *s == 'c')
	    ++s;		// no extra chars
	లేకపోతే		// must have a number
	{
	    అయితే (vim_isdigit(*++s))
		;

	    ఒకవేళ (!VIM_ISDIGIT(*(s - 1)))
	    {
		ఒకవేళ (వాదనలు->os_errbuf != ఖాళీ)
		{
		    vim_snprintf(వాదనలు->os_errbuf, వాదనలు->os_errbuflen,
			    _(e_missing_సంఖ్య_after_angle_str_angle),
			    transchar_byte(*(s - 1)));
		    errmsg = వాదనలు->os_errbuf;
		}
		లేకపోతే
		    errmsg = "";
		ఆపు;
	    }
	}
	ఒకవేళ (*s == ',')
	    ++s;
	లేకపోతే ఒకవేళ (*s)
	{
	    ఒకవేళ (వాదనలు->os_errbuf != ఖాళీ)
		errmsg = e_missing_comma;
	    లేకపోతే
		errmsg = "";
	    ఆపు;
	}
    }
    ఒకవేళ (*p_viminfo && errmsg == ఖాళీ && get_viminfo_parameter('\'') < 0)
	errmsg = e_must_specify_a_విలువ;

    తిరిగి errmsg;
}
#అయితే_ముగింపు

/*
 * The 'virtualedit' option is changed.
 */
    అక్షరం *
did_సమితి_virtualedit(optset_T *వాదనలు)
{
    char_u		*ve = p_ve;
    గుర్తులేని పూర్ణాంకం	*flags = &ve_flags;

    ఒకవేళ (వాదనలు->os_flags & OPT_LOCAL)
    {
	ve = curwin->w_p_ve;
	flags = &curwin->w_ve_flags;
    }

    ఒకవేళ ((వాదనలు->os_flags & OPT_LOCAL) && *ve == NUL)
	// make the local value empty: use the global value
	*flags = 0;
    లేకపోతే
    {
	ఒకవేళ (opt_అక్షరశ్రేణులు_flags(ve, p_ve_విలువలు, flags, TRUE) != OK)
	    తిరిగి e_invalid_వాదం;
	లేకపోతే ఒకవేళ (STRCMP(ve, వాదనలు->os_oldval.string) != 0)
	{
	    // Recompute cursor position in case the new 've' setting
	    // changes something.
	    validate_virtcol();
	    coladvance(curwin->w_virtcol);
	}
    }

    తిరిగి ఖాళీ;
}

    పూర్ణాంకం
expand_సమితి_virtualedit(optexpand_T *వాదనలు, పూర్ణాంకం *అంకె_Matches, char_u ***matches)
{
    తిరిగి expand_సమితి_opt_string(
	    వాదనలు,
	    p_ve_విలువలు,
	    ARRAY_నిడివి(p_ve_విలువలు) - 1,
	    అంకె_Matches,
	    matches);
}

/*
 * The 'whichwrap' option is changed.
 */
    అక్షరం *
did_సమితి_whichwrap(optset_T *వాదనలు)
{
    char_u	**varp = (char_u **)వాదనలు->os_varp;

    // Add ',' to the list flags because 'whichwrap' is a flag
    // list that is comma-separated.
    తిరిగి did_సమితి_option_listflag(*varp, (char_u *)(WW_ALL ","),
		    వాదనలు->os_errbuf, వాదనలు->os_errbuflen);
}

    పూర్ణాంకం
expand_సమితి_whichwrap(optexpand_T *వాదనలు, పూర్ణాంకం *అంకె_Matches, char_u ***matches)
{
    తిరిగి expand_సమితి_opt_listflag(వాదనలు, (char_u*)WW_ALL, అంకె_Matches, matches);
}

/*
 * The 'wildmode' option is changed.
 */
    అక్షరం *
did_సమితి_wildmode(optset_T *వాదనలు వాడని)
{
    ఒకవేళ (check_opt_wim() == FAIL)
	తిరిగి e_invalid_వాదం;
    తిరిగి ఖాళీ;
}

    పూర్ణాంకం
expand_సమితి_wildmode(optexpand_T *వాదనలు, పూర్ణాంకం *అంకె_Matches, char_u ***matches)
{
    తిరిగి expand_సమితి_opt_string(
	    వాదనలు,
	    p_wim_విలువలు,
	    ARRAY_నిడివి(p_wim_విలువలు) - 1,
	    అంకె_Matches,
	    matches);
}

/*
 * The 'wildoptions' option is changed.
 */
    అక్షరం *
did_సమితి_wildoptions(optset_T *వాదనలు వాడని)
{
    తిరిగి did_సమితి_opt_అక్షరశ్రేణులు(p_wop, p_wop_విలువలు, TRUE);
}

    పూర్ణాంకం
expand_సమితి_wildoptions(optexpand_T *వాదనలు, పూర్ణాంకం *అంకె_Matches, char_u ***matches)
{
    తిరిగి expand_సమితి_opt_string(
	    వాదనలు,
	    p_wop_విలువలు,
	    ARRAY_నిడివి(p_wop_విలువలు) - 1,
	    అంకె_Matches,
	    matches);
}

#ఒకవేళ defined(FEAT_WAK)
/*
 * The 'winaltkeys' option is changed.
 */
    అక్షరం *
did_సమితి_winaltkeys(optset_T *వాదనలు వాడని)
{
    అక్షరం *errmsg = ఖాళీ;

    ఒకవేళ (*p_wak == NUL || check_opt_అక్షరశ్రేణులు(p_wak, p_wak_విలువలు, FALSE) != OK)
	errmsg = e_invalid_వాదం;
# ifdef FEAT_MENU
#  ఒకవేళ defined(FEAT_GUI_MOTIF)
    లేకపోతే ఒకవేళ (gui.లోపల_use)
	gui_motif_సమితి_mnemonics(p_wak[0] == 'y' || p_wak[0] == 'm');
#  elif defined(FEAT_GUI_GTK)
    లేకపోతే ఒకవేళ (gui.లోపల_use)
	gui_gtk_సమితి_mnemonics(p_wak[0] == 'y' || p_wak[0] == 'm');
#  endif
# endif
    తిరిగి errmsg;
}

    పూర్ణాంకం
expand_సమితి_winaltkeys(optexpand_T *వాదనలు, పూర్ణాంకం *అంకె_Matches, char_u ***matches)
{
    తిరిగి expand_సమితి_opt_string(
	    వాదనలు,
	    p_wak_విలువలు,
	    ARRAY_నిడివి(p_wak_విలువలు) - 1,
	    అంకె_Matches,
	    matches);
}
#అయితే_ముగింపు

/*
 * The 'wincolor' option is changed.
 */
    అక్షరం *
did_సమితి_wincolor(optset_T *వాదనలు వాడని)
{
    update_wincolor(curwin, వాదనలు->os_newval.string);
    తిరిగి ఖాళీ;
}

/*
 * The 'winhighlight' option is changed
 */
    అక్షరం *
did_సమితి_winhighlight(optset_T *వాదనలు)
{
    తిరిగి update_winhighlight(curwin, వాదనలు->os_newval.string);
}

/*
 * Expand 'winhighlight' option.
 */
    పూర్ణాంకం
expand_సమితి_winhighlight(optexpand_T *వాదనలు, పూర్ణాంకం *అంకె_Matches, char_u ***matches)
{
    expand_T	    *xp = వాదనలు->oe_xp;

    ఒకవేళ ((xp->xp_నమూనా > వాదనలు->oe_సమితి_arg && *(xp->xp_నమూనా-1) == ':')
	    || xp->xp_నమూనా == వాదనలు->oe_సమితి_arg || *(xp->xp_నమూనా-1) == ',')
    {
	// After a ':' or after a ',', or at the start, so expand highlight
	// group name.

	// If starts with !, then expand 'highlight' occasions.
	ఒకవేళ (*xp->xp_నమూనా == '!')
	    తిరిగి expand_hl_occasions(వాదనలు, అంకె_Matches, matches, '!');
	లేకపోతే
	    తిరిగి expand_సమితి_opt_generic(
		    వాదనలు,
		    get_highlight_name,
		    అంకె_Matches,
		    matches);
    }

    VIM_CLEAR(*matches);
    తిరిగి FAIL;
}

    పూర్ణాంకం
expand_సమితి_wincolor(optexpand_T *వాదనలు, పూర్ణాంకం *అంకె_Matches, char_u ***matches)
{
    తిరిగి expand_సమితి_opt_generic(
	    వాదనలు,
	    get_highlight_name,
	    అంకె_Matches,
	    matches);
}

#నిర్వచితమైతే FEAT_SYN_HL
/*
 * When the 'syntax' option is set, load the syntax of that name.
 */
    స్థిర శూన్య
do_syntax_autocmd(పూర్ణాంకం విలువ_మారింది)
{
    స్థిర పూర్ణాంకం syn_recursive = 0;

    ++syn_recursive;
    // Only pass TRUE for "force" when the value changed or not used
    // recursively, to avoid endless recurrence.
    apply_autocmds(EVENT_SYNTAX, curbuf->b_p_syn, curbuf->b_fname,
	    విలువ_మారింది || syn_recursive == 1, curbuf);
    curbuf->b_flags |= BF_SYN_సమితి;
    --syn_recursive;
}
#అయితే_ముగింపు

/*
 * When the 'filetype' option is set, trigger the FileType autocommand.
 */
    స్థిర శూన్య
do_filetype_autocmd(char_u **varp, పూర్ణాంకం opt_flags, పూర్ణాంకం విలువ_మారింది)
{
    // Skip this when called from a modeline and the filetype was already set
    // to this value.
    ఒకవేళ ((opt_flags & OPT_MODELINE) && !విలువ_మారింది)
	తిరిగి;

    స్థిర పూర్ణాంకం  ft_recursive = 0;
    పూర్ణాంకం	    secure_save = secure;

    // Reset the secure flag, since the value of 'filetype' has
    // been checked to be safe.
    secure = 0;

    ++ft_recursive;
    curbuf->b_did_filetype = true;
    // Only pass TRUE for "force" when the value changed or not
    // used recursively, to avoid endless recurrence.
    apply_autocmds(EVENT_FILETYPE, curbuf->b_p_ft, curbuf->b_fname,
	    విలువ_మారింది || ft_recursive == 1, curbuf);
    --ft_recursive;
    // Just in case the old "curbuf" is now invalid.
    ఒకవేళ (varp != &(curbuf->b_p_ft))
	varp = ఖాళీ;

    secure = secure_save;
}

#నిర్వచితమైతే FEAT_SPELL
/*
 * When the 'spelllang' option is set, source the spell/LANG.vim file in
 * 'runtimepath'.
 */
    స్థిర శూన్య
do_spelllang_ఆధారం(శూన్య)
{
    char_u	fname[200];
    char_u	*p;
    char_u	*q = curwin->w_s->b_p_spl;

    // Skip the first name if it is "cjk".
    ఒకవేళ (STRNCMP(q, "cjk,", 4) == 0)
	q += 4;

    // They could set 'spellcapcheck' depending on the language.  Use the first
    // name in 'spelllang' up to '_region' or '.encoding'.
    కొరకు (p = q; *p != NUL; ++p)
	ఒకవేళ (!ASCII_ISALNUM(*p) && *p != '-')
	    ఆపు;
    ఒకవేళ (p > q)
    {
	vim_snprintf((అక్షరం *)fname, 200, "spell/%.*s.vim",
		(పూర్ణాంకం)(p - q), q);
	ఆధారం_runtime(fname, DIP_ALL);
    }
}
#అయితే_ముగింపు

/*
 * Handle string options that need some action to perform when changed.
 * The new value must be allocated.
 * Returns NULL for success, or an untranslated error message for an error.
 */
    అక్షరం *
did_సమితి_string_option(
    పూర్ణాంకం		opt_idx,		// index in options[] table
    char_u	**varp,			// pointer to the option variable
    char_u	*oldval,		// previous value of the option
    char_u	*విలువ,			// new value of the option
    అక్షరం	*errbuf,		// buffer for errors, or NULL
    size_t	errbuflen,		// length of error buffer
    పూర్ణాంకం		opt_flags,		// OPT_LOCAL and/or OPT_GLOBAL
    సమితి_op_T    op,			// OP_ADDING/OP_PREPENDING/OP_REMOVING
    పూర్ణాంకం		*విలువ_checked)		// value was checked to be safe, no
					// need to set P_INSECURE
{
    అక్షరం	*errmsg = ఖాళీ;
    long_u	free_oldval = (get_option_flags(opt_idx) & P_ALLOCED);
    opt_did_సమితి_cb_T did_సమితి_cb = get_option_did_సమితి_cb(opt_idx);
    optset_T	వాదనలు;

    // 'ttytype' is an alias for 'term'.  Both 'term' and 'ttytype' point to
    // T_NAME.  If 'term' or 'ttytype' is modified, then use the index for the
    // 'term' option.  Only set the P_ALLOCED flag on 'term'.
    ఒకవేళ (varp == &T_NAME)
    {
	opt_idx = findoption((char_u *)"term");
	ఒకవేళ (opt_idx >= 0)
	{
	    free_oldval = (get_option_flags(opt_idx) & P_ALLOCED);
	    did_సమితి_cb = get_option_did_సమితి_cb(opt_idx);
	}
    }

    CLEAR_క్షేత్రం(వాదనలు);

    // Disallow changing some options from secure mode
    ఒకవేళ ((secure
#నిర్వచితమైతే HAVE_SANDBOX
		|| sandbox != 0
#అయితే_ముగింపు
		) && (get_option_flags(opt_idx) & P_SECURE))
	errmsg = e_not_allowed_ఇక్కడ;
    // Check for a "normal" directory or file name in some options.
    లేకపోతే ఒకవేళ (check_illegal_మార్గం_పేర్లు(opt_idx, varp))
	errmsg = e_invalid_వాదం;
    లేకపోతే ఒకవేళ (did_సమితి_cb != ఖాళీ)
    {
	వాదనలు.os_varp = (char_u *)varp;
	వాదనలు.os_idx = opt_idx;
	వాదనలు.os_flags = opt_flags;
	వాదనలు.os_op = op;
	వాదనలు.os_oldval.string = oldval;
	వాదనలు.os_newval.string = విలువ;
	వాదనలు.os_errbuf = errbuf;
	వాదనలు.os_errbuflen = errbuflen;
	// Invoke the option specific callback function to validate and apply
	// the new option value.
	errmsg = did_సమితి_cb(&వాదనలు);

	// The 'keymap', 'filetype' and 'syntax' option callback functions
	// may change the os_value_checked field.
	*విలువ_checked = వాదనలు.os_విలువ_checked;
    }

    // If an error is detected, restore the previous value.
    ఒకవేళ (errmsg != ఖాళీ)
    {
	free_string_option(*varp);
	*varp = oldval;
	// When resetting some values, need to act on it.
	ఒకవేళ (వాదనలు.os_restore_chartab)
	    (శూన్య)init_chartab();
	ఒకవేళ (varp == &p_hl)
	    (శూన్య)highlight_మారింది();
    }
    లేకపోతే
    {
#నిర్వచితమైతే FEAT_EVAL
	// Remember where the option was set.
	సమితి_option_sctx_idx(opt_idx, opt_flags, current_sctx);
#అయితే_ముగింపు
	// Free string options that are in allocated memory.
	// Use "free_oldval", because recursiveness may change the flags under
	// our fingers (esp. init_highlight()).
	ఒకవేళ (free_oldval)
	    free_string_option(oldval);
	సమితి_option_flag(opt_idx, P_ALLOCED);

	ఒకవేళ ((opt_flags & (OPT_LOCAL | OPT_GLOBAL)) == 0
		&& is_global_local_option(opt_idx))
	{
	    // global option with local value set to use global value; free
	    // the local value and make it empty
	    char_u *p = get_option_varp_scope(opt_idx, OPT_LOCAL);
	    free_string_option(*(char_u **)p);
	    *(char_u **)p = ఖాళీలు_option;
	}

	// May set global value for local option.
	లేకపోతే ఒకవేళ (!(opt_flags & OPT_LOCAL) && opt_flags != OPT_GLOBAL)
	    సమితి_string_option_global(opt_idx, varp);

	// Trigger the autocommand only after setting the flags.
#నిర్వచితమైతే FEAT_SYN_HL
	ఒకవేళ (varp == &(curbuf->b_p_syn))
	    do_syntax_autocmd(వాదనలు.os_విలువ_మారింది);
#అయితే_ముగింపు
	లేకపోతే ఒకవేళ (varp == &(curbuf->b_p_ft))
	    do_filetype_autocmd(varp, opt_flags, వాదనలు.os_విలువ_మారింది);
#నిర్వచితమైతే FEAT_SPELL
	ఒకవేళ (varp == &(curwin->w_s->b_p_spl))
	    do_spelllang_ఆధారం();
#అయితే_ముగింపు
    }

    ఒకవేళ (varp == &p_mouse)
    {
	ఒకవేళ (*p_mouse == NUL)
	    mch_setmouse(FALSE);    // switch mouse off
	లేకపోతే
	    setmouse();		    // in case 'mouse' changed
    }

    ఒకవేళ (varp == &p_rtp)
    {
	export_myvimdir();
#ఒకవేళ defined(FEAT_LUA)
	update_package_paths_లోపల_lua();
#అయితే_ముగింపు
    }

#ఒకవేళ defined(FEAT_LINEBREAK)
    // Changing Formatlistpattern when briopt includes the list setting:
    // redraw
    ఒకవేళ ((varp == &p_flp || varp == &(curbuf->b_p_flp))
	    && curwin->w_briopt_list)
	redraw_all_later(UPD_NOT_VALID);
#అయితే_ముగింపు

    ఒకవేళ (curwin->w_curswant != MAXCOL
		   && (get_option_flags(opt_idx) & (P_CURSWANT | P_RALL)) != 0
				&& (get_option_flags(opt_idx) & P_HLONLY) == 0)
	curwin->w_సమితి_curswant = true;

    ఒకవేళ ((opt_flags & OPT_NO_REDRAW) == 0)
    {
#నిర్వచితమైతే FEAT_GUI
	// set when changing an option that only requires a redraw in the GUI
	పూర్ణాంకం	redraw_gui_only = FALSE;

	ఒకవేళ (varp == &p_go			// 'guioptions'
		|| varp == &p_guifont		// 'guifont'
# ifdef FEAT_GUI_TABLINE
		|| varp == &p_gtl		// 'guitablabel'
		|| varp == &p_gtt		// 'guitabtooltip'
# endif
# ifdef FEAT_XFONTSET
		|| varp == &p_guifontset	// 'guifontset'
# endif
		|| varp == &p_guifontwide	// 'guifontwide'
# ఒకవేళ defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
		|| varp == &p_guiligatures	// 'guiligatures'
# endif
	    )
	    redraw_gui_only = TRUE;

	// check redraw when it's not a GUI option or the GUI is active.
	ఒకవేళ (!redraw_gui_only || gui.లోపల_use)
#అయితే_ముగింపు
	    check_redraw(get_option_flags(opt_idx));
    }

#ఒకవేళ defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
    ఒకవేళ (వాదనలు.os_did_swaptcap)
    {
	సమితి_termname((char_u *)"win32");
	init_highlight(TRUE, FALSE);
    }
#అయితే_ముగింపు

    తిరిగి errmsg;
}

/*
 * Check an option that can be a range of string values.
 *
 * Return OK for correct value, FAIL otherwise.
 * Empty is always OK.
 */
    స్థిర పూర్ణాంకం
check_opt_అక్షరశ్రేణులు(
    char_u	*val,
    అక్షరం	**విలువలు,
    పూర్ణాంకం		list)	    // when TRUE: accept a list of values
{
    తిరిగి opt_అక్షరశ్రేణులు_flags(val, విలువలు, ఖాళీ, list);
}

/*
 * Handle an option that can be a range of string values.
 * Set a flag in "*flagp" for each string present.
 *
 * Return OK for correct value, FAIL otherwise.
 * Empty is always OK.
 */
    స్థిర పూర్ణాంకం
opt_అక్షరశ్రేణులు_flags(
    char_u	*val,		// new value
    అక్షరం	**విలువలు,	// array of valid string values
    గుర్తులేని	*flagp,
    పూర్ణాంకం		list)		// when TRUE: accept a list of values
{
    పూర్ణాంకం		i;
    పూర్ణాంకం		len;
    గుర్తులేని	new_flags = 0;

    అయితే (*val)
    {
	కొరకు (i = 0; ; ++i)
	{
	    ఒకవేళ (విలువలు[i] == ఖాళీ)	// val not found in values[]
		తిరిగి FAIL;

	    len = (పూర్ణాంకం)STRLEN(విలువలు[i]);
	    ఒకవేళ (STRNCMP(విలువలు[i], val, len) == 0
		    && ((list && val[len] == ',') || val[len] == NUL))
	    {
		val += len + (val[len] == ',');
		new_flags |= (1 << i);
		ఆపు;		// check next item in val list
	    }
	}
    }
    ఒకవేళ (flagp != ఖాళీ)
	*flagp = new_flags;

    తిరిగి OK;
}

/*
 * return OK if "p" is a valid fileformat name, FAIL otherwise.
 */
    పూర్ణాంకం
check_ff_విలువ(char_u *p)
{
    తిరిగి check_opt_అక్షరశ్రేణులు(p, p_ff_విలువలు, FALSE);
}

/*
 * Save the actual shortmess Flags and clear them temporarily to avoid that
 * file messages overwrites any output from the following commands.
 *
 * Caller must make sure to first call save_clear_shm_value() and then
 * restore_shm_value() exactly the same number of times.
 */
    శూన్య
save_clear_shm_విలువ(శూన్య)
{
    ఒకవేళ (STRLEN(p_shm) >= SHM_LEN)
    {
	iemsg(e_అంతర్గత_error_shortmess_too_long);
	తిరిగి;
    }

    ఒకవేళ (++సమితి_shm_recursive == 1)
    {
	STRCPY(shm_buf, p_shm);
	సమితి_option_విలువ_give_err((char_u *)"shm", 0L, (char_u *)"", 0);
    }
}

/*
 * Restore the shortmess Flags set from the save_clear_shm_value() function.
 */
    శూన్య
restore_shm_విలువ(శూన్య)
{
    ఒకవేళ (--సమితి_shm_recursive == 0)
    {
	సమితి_option_విలువ_give_err((char_u *)"shm", 0L, shm_buf, 0);
	vim_memset(shm_buf, 0, SHM_LEN);
    }
}

/*
 * Export the environment variable $MYVIMDIR to the first item in runtimepath
 */
    శూన్య
export_myvimdir(శూన్య)
{
    పూర్ణాంకం		dofree = FALSE;
    char_u	*p;
    char_u	*q = p_rtp;
    string_T	buf;

    buf.string = alloc(MAXPATHL);
    ఒకవేళ (buf.string == ఖాళీ)
	తిరిగి;

    buf.నిడివి = (size_t)copy_option_part(&q, buf.string, MAXPATHL, ",");

    p = vim_getenv((char_u *)"MYVIMDIR", &dofree);

    ఒకవేళ (p == ఖాళీ || STRCMP(p, buf.string) != 0)
    {
	ఒకవేళ (*buf.string != NUL && !after_pathsep(buf.string, buf.string + buf.నిడివి))
	    STRCPY(buf.string + buf.నిడివి, PATHSEPSTR);
#నిర్వచితమైతే MSWIN
	// normalize path separators
	కొరకు (q = buf.string; *q != NUL; q++)
	    ఒకవేళ (*q == '/')
		*q = '\\';
#అయితే_ముగింపు
	vim_setenv((char_u *)"MYVIMDIR", buf.string);
    }
    ఒకవేళ (dofree)
	vim_free(p);
    vim_free(buf.string);
}
